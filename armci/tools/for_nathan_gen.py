#!/usr/bin/env python

'''Generate the armci_prof.c source from the parmci.h header.'''

import sys

def get_signatures(header):
    # first, gather all function signatures from parmci.h aka argv[1]
    accumulating = False
    signatures = []
    current_signature = ''
    EXTERN = 'extern'
    SEMICOLON = ';'
    for line in open(header):
        line = line.strip() # remove whitespace before and after line
        if not line:
            continue # skip blank lines
        if EXTERN in line and SEMICOLON in line:
            signatures.append(line)
        elif EXTERN in line:
            current_signature = line
            accumulating = True
        elif SEMICOLON in line and accumulating:
            current_signature += line
            signatures.append(current_signature)
            accumulating = False
        elif accumulating:
            current_signature += line
    return signatures

class FunctionArgument(object):
    def __init__(self, signature):
        self.pointer = signature.count('*')
        self.array = '[' in signature
        signature = signature.replace('*','').strip()
        signature = signature.replace('[','').strip()
        signature = signature.replace(']','').strip()
        self.type,self.name = signature.split()

    def __str__(self):
        ret = self.type[:]
        ret += ' '
        for p in range(self.pointer):
            ret += '*'
        ret += self.name
        if self.array:
            ret += '[]'
        return ret

class Function(object):
    def __init__(self, signature):
        signature = signature.replace('extern','').strip()
        self.return_type,signature = signature.split(None,1)
        self.return_type = self.return_type.strip()
        signature = signature.strip()
        if '*' not in self.return_type and signature[0] == '*':
            # return type is void* not void
            self.return_type += '*'
            signature = signature[1:].strip()
        self.name,signature = signature.split('(',1)
        self.name = self.name.strip()
        signature = signature.replace(')','').strip()
        signature = signature.replace(';','').strip()
        self.args = []
        if signature:
            for arg in signature.split(','):
                self.args.append(FunctionArgument(arg.strip()))

    def get_call(self, name=None):
        sig = ''
        if not name:
            sig += self.name
        else:
            sig += name
        sig += '('
        if self.args:
            for arg in self.args:
                sig += arg.name
                sig += ', '
            sig = sig[:-2] # remove last ', '
        sig += ')'
        return sig

    def get_signature(self, name=None):
        sig = self.return_type[:]
        sig += ' '
        if not name:
            sig += self.name
        else:
            sig += name
        sig += '('
        if self.args:
            for arg in self.args:
                sig += str(arg)
                sig += ', '
            sig = sig[:-2] # remove last ', '
        sig += ')'
        return sig

    def __str__(self):
        return self.get_signature()

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print 'incorrect number of arguments'
        print 'usage: prof_gen.py <parmci.h> > <armci_prof.c>'
        sys.exit(len(sys.argv))

    # print headers
    print '''
#if HAVE_CONFIG_H
#   include "config.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "armci.h"
#include "parmci.h"
#include "armci_profile.h"
#include "armci_profile.c"

MPI_Comm comm_node = MPI_COMM_NULL;
int rank = 0;
int size = 0;
int node_rank = 0;
int node_size = 0;
int *node_ranks = NULL;
int message_counter = 0;
int message_sample = 5;
long **counters = NULL;
long *my_counters = NULL;
long **node_counters = NULL;
#define LOCAL_WRITE      0
#define LOCAL_READ       1
#define REMOTE_WRITE     2
#define REMOTE_READ      3
#define LOCAL_WRITE_END  4
#define LOCAL_READ_END   5
#define REMOTE_WRITE_END 6
#define REMOTE_READ_END  7
#define RMW_DUMMY        8
#define NUM_COUNTERS     9
#ifndef HOST_NAME_MAX
#   define HOST_NAME_MAX 256
#endif

typedef struct {
    long local_write;
    long local_read;
    long remote_write;
    long remote_read;
} armci_congestion_t;

void armci_get_congestion(armci_congestion_t *data)
{
    int i;

    /* get counters from all procs on my node */
    for (i=0; i<node_size; ++i) {
        if (i == node_rank) {
        }
        else {
            PARMCI_Get(counters[node_ranks[i]], node_counters[i],
                    sizeof(long) * NUM_COUNTERS, node_ranks[i]);
        }
    }

    /* do something with the counters */
    for (i=0; i<node_size; ++i) {
        data->local_write = node_counters[i][LOCAL_WRITE] -
                node_counters[i][LOCAL_WRITE_END];
        data->local_read = node_counters[i][LOCAL_READ] -
                node_counters[i][LOCAL_READ_END];
        data->remote_write = node_counters[i][LOCAL_WRITE] -
                node_counters[i][REMOTE_WRITE_END];
        data->remote_read = node_counters[i][REMOTE_READ] -
                node_counters[i][REMOTE_READ_END];
    }
}

'''

    functions = {}
    # parse signatures into the Function class
    for sig in get_signatures(sys.argv[1]):
        function = Function(sig)
        functions[function.name] = function

    # now process the functions
    for name in sorted(functions):
        func = functions[name]
        maybe_declare = ''
        maybe_assign = ''
        maybe_return = ''
        if '*' in func.return_type or 'void' not in func.return_type:
            maybe_declare = '%s ret;' % func.return_type
            maybe_assign = 'ret = '
            maybe_return = 'return ret;'
        func = functions[name]
        new_name = None
        if 'PARMCI_' in name:
            new_name = name.replace('PARMCI_','ARMCI_')
        elif 'parmci_' in name:
            new_name = name.replace('parmci_','armci_')
        if new_name in ['ARMCI_Init','ARMCI_Init_args']:
            print '''
%s
{
    int status = MPI_SUCCESS;
    int hash=0;
    size_t hostname_length;
    size_t i;
    char hostname[HOST_NAME_MAX+1];
    int ret;
    char *env_armci_sample_frequency=NULL;

    /* init ARMCI */
    ret = %s;

    /* rank and size */
    rank = armci_msg_me();
    size = armci_msg_nproc();

    /* allocate and initialize all counters */
    counters = (long**)(malloc(sizeof(long*) * size));
    PARMCI_Malloc((void**)counters, sizeof(long) * NUM_COUNTERS);
    my_counters = counters[rank];
    memset(my_counters, 0, sizeof(long) * NUM_COUNTERS);
    message_counter = 0;
    PARMCI_Barrier(); /* to make sure everyone finished memset */

    /* create MPI_Comm for each node, get rank and size for new comm */
    assert(0 == gethostname(hostname, HOST_NAME_MAX));
    hostname_length = strlen(hostname);
    for (i=0; i<hostname_length; ++i) {
        hash += (int)(hostname[i]);
    }
    status = MPI_Comm_split(MPI_COMM_WORLD, hash, rank, &comm_node);
    assert(status == MPI_SUCCESS);
    status = MPI_Comm_rank(comm_node, &node_rank);
    assert(status == MPI_SUCCESS);
    status = MPI_Comm_size(comm_node, &node_size);
    assert(status == MPI_SUCCESS);
    node_ranks = (int*)(malloc(sizeof(int) * node_size));
    node_ranks[node_rank] = rank;
    status = MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, 
            node_ranks, node_size, MPI_INT, comm_node);
    assert(status == MPI_SUCCESS);

    /* allocate counters used during summation (node_counters) */
    node_counters = (long**)malloc(sizeof(long*) * node_size);
    for (i=0; i<node_size; ++i) {
        if (i == node_rank) {
            node_counters[i] = my_counters;
        }
        else {
            node_counters[i] = (long*)malloc(sizeof(long) * NUM_COUNTERS);
        }
    }

    /* tune how frequent messages are measured */
    env_armci_sample_frequency = getenv("ARMCI_SAMPLE_FREQUENCY");
    if (NULL != env_armci_sample_frequency) {
        message_sample = atoi(env_armci_sample_frequency);
    }

    return ret;
}
''' % (func.get_signature(new_name), func.get_call())
        elif new_name in ['ARMCI_Finalize']:
            print '''
%s
{
    int i;
    my_counters = NULL;
    PARMCI_Free(counters[rank]);
    free(node_ranks);
    /* free node_counters */
    for (i=0; i<node_size; ++i) {
        if (i == node_rank) {
            node_counters[i] = NULL;
        }
        else {
            free(node_counters[i]);
        }
    }
    free(node_counters);
    %s;
}
''' % (func.get_signature(new_name), func.get_call())
        elif new_name in ['ARMCI_Get', 'ARMCI_GetS', 'ARMCI_GetV',
                          'ARMCI_NbGet', 'ARMCI_NbGetS', 'ARMCI_NbGetV']:
            print '''
%s
{
    %s
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ], 1, proc);
    }
    %s%s;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ_END], 1, proc);
        message_counter = 0;
    }
    %s
}
''' % (func.get_signature(new_name),
                maybe_declare,
                maybe_assign, func.get_call(),
                maybe_return)
        elif new_name in ['ARMCI_Put', 'ARMCI_PutS', 'ARMCI_PutV',
                          'ARMCI_NbPut', 'ARMCI_NbPutS', 'ARMCI_NbPutV',
                          'ARMCI_Acc', 'ARMCI_AccS', 'ARMCI_AccV',
                          'ARMCI_NbAcc', 'ARMCI_NbAccS', 'ARMCI_NbAccV']:
            print '''
%s
{
    %s
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE], 1, proc);
    }
    %s%s;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE_END], 1, proc);
        message_counter = 0;
    }
    %s
}
''' % (func.get_signature(new_name),
                maybe_declare,
                maybe_assign, func.get_call(),
                maybe_return)
        else:
            print '''
%s
{
    %s
    %s%s;
    %s
}
''' % (func.get_signature(new_name),
        maybe_declare,
        maybe_assign, func.get_call(),
        maybe_return)
