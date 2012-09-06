
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



int ARMCI_Acc(int optype, void *scale, void *src, void *dst, int bytes, int proc)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE], 1, proc);
    }
    ret = PARMCI_Acc(optype, scale, src, dst, bytes, proc);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_AccS(int optype, void *scale, void *src_ptr, int *src_stride_arr, void *dst_ptr, int *dst_stride_arr, int *count, int stride_levels, int proc)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE], 1, proc);
    }
    ret = PARMCI_AccS(optype, scale, src_ptr, src_stride_arr, dst_ptr, dst_stride_arr, count, stride_levels, proc);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_AccV(int op, void *scale, armci_giov_t *darr, int len, int proc)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE], 1, proc);
    }
    ret = PARMCI_AccV(op, scale, darr, len, proc);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


void ARMCI_AllFence()
{
    
    PARMCI_AllFence();
    
}


void ARMCI_Barrier()
{
    
    PARMCI_Barrier();
    
}


int ARMCI_Create_mutexes(int num)
{
    int ret;
    ret = PARMCI_Create_mutexes(num);
    return ret;
}


int ARMCI_Destroy_mutexes()
{
    int ret;
    ret = PARMCI_Destroy_mutexes();
    return ret;
}


void ARMCI_Fence(int proc)
{
    
    PARMCI_Fence(proc);
    
}


void ARMCI_Finalize()
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
    PARMCI_Finalize();
}


int ARMCI_Free(void *ptr)
{
    int ret;
    ret = PARMCI_Free(ptr);
    return ret;
}


int ARMCI_Free_local(void *ptr)
{
    int ret;
    ret = PARMCI_Free_local(ptr);
    return ret;
}


int ARMCI_Get(void *src, void *dst, int bytes, int proc)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ], 1, proc);
    }
    ret = PARMCI_Get(src, dst, bytes, proc);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_GetS(void *src_ptr, int *src_stride_arr, void *dst_ptr, int *dst_stride_arr, int *count, int stride_levels, int proc)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ], 1, proc);
    }
    ret = PARMCI_GetS(src_ptr, src_stride_arr, dst_ptr, dst_stride_arr, count, stride_levels, proc);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_GetV(armci_giov_t *darr, int len, int proc)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ], 1, proc);
    }
    ret = PARMCI_GetV(darr, len, proc);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


double ARMCI_GetValueDouble(void *src, int proc)
{
    double ret;
    ret = PARMCI_GetValueDouble(src, proc);
    return ret;
}


float ARMCI_GetValueFloat(void *src, int proc)
{
    float ret;
    ret = PARMCI_GetValueFloat(src, proc);
    return ret;
}


int ARMCI_GetValueInt(void *src, int proc)
{
    int ret;
    ret = PARMCI_GetValueInt(src, proc);
    return ret;
}


long ARMCI_GetValueLong(void *src, int proc)
{
    long ret;
    ret = PARMCI_GetValueLong(src, proc);
    return ret;
}


int ARMCI_Init()
{
    int status = MPI_SUCCESS;
    int hash=0;
    size_t hostname_length;
    size_t i;
    char hostname[HOST_NAME_MAX+1];
    int ret;
    char *env_armci_sample_frequency=NULL;

    /* init ARMCI */
    ret = PARMCI_Init();

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


int ARMCI_Init_args(int *argc, char ***argv)
{
    int status = MPI_SUCCESS;
    int hash=0;
    size_t hostname_length;
    size_t i;
    char hostname[HOST_NAME_MAX+1];
    int ret;
    char *env_armci_sample_frequency=NULL;

    /* init ARMCI */
    ret = PARMCI_Init_args(argc, argv);

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


int ARMCI_Initialized()
{
    int ret;
    ret = PARMCI_Initialized();
    return ret;
}


void ARMCI_Lock(int mutex, int proc)
{
    
    PARMCI_Lock(mutex, proc);
    
}


int ARMCI_Malloc(void **ptr_arr, armci_size_t bytes)
{
    int ret;
    ret = PARMCI_Malloc(ptr_arr, bytes);
    return ret;
}


void* ARMCI_Malloc_local(armci_size_t bytes)
{
    void* ret;
    ret = PARMCI_Malloc_local(bytes);
    return ret;
}


int ARMCI_NbAccS(int optype, void *scale, void *src_ptr, int *src_stride_arr, void *dst_ptr, int *dst_stride_arr, int *count, int stride_levels, int proc, armci_hdl_t *nb_handle)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE], 1, proc);
    }
    ret = PARMCI_NbAccS(optype, scale, src_ptr, src_stride_arr, dst_ptr, dst_stride_arr, count, stride_levels, proc, nb_handle);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_NbAccV(int op, void *scale, armci_giov_t *darr, int len, int proc, armci_hdl_t *nb_handle)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE], 1, proc);
    }
    ret = PARMCI_NbAccV(op, scale, darr, len, proc, nb_handle);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_NbGet(void *src, void *dst, int bytes, int proc, armci_hdl_t *nb_handle)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ], 1, proc);
    }
    ret = PARMCI_NbGet(src, dst, bytes, proc, nb_handle);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_NbGetS(void *src_ptr, int *src_stride_arr, void *dst_ptr, int *dst_stride_arr, int *count, int stride_levels, int proc, armci_hdl_t *nb_handle)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ], 1, proc);
    }
    ret = PARMCI_NbGetS(src_ptr, src_stride_arr, dst_ptr, dst_stride_arr, count, stride_levels, proc, nb_handle);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_NbGetV(armci_giov_t *darr, int len, int proc, armci_hdl_t *nb_handle)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ], 1, proc);
    }
    ret = PARMCI_NbGetV(darr, len, proc, nb_handle);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_READ_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_READ_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_NbPut(void *src, void *dst, int bytes, int proc, armci_hdl_t *nb_handle)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE], 1, proc);
    }
    ret = PARMCI_NbPut(src, dst, bytes, proc, nb_handle);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_NbPutS(void *src_ptr, int *src_stride_arr, void *dst_ptr, int *dst_stride_arr, int *count, int stride_levels, int proc, armci_hdl_t *nb_handle)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE], 1, proc);
    }
    ret = PARMCI_NbPutS(src_ptr, src_stride_arr, dst_ptr, dst_stride_arr, count, stride_levels, proc, nb_handle);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_NbPutV(armci_giov_t *darr, int len, int proc, armci_hdl_t *nb_handle)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE], 1, proc);
    }
    ret = PARMCI_NbPutV(darr, len, proc, nb_handle);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_NbPutValueDouble(double src, void *dst, int proc, armci_hdl_t *nb_handle)
{
    int ret;
    ret = PARMCI_NbPutValueDouble(src, dst, proc, nb_handle);
    return ret;
}


int ARMCI_NbPutValueFloat(float src, void *dst, int proc, armci_hdl_t *nb_handle)
{
    int ret;
    ret = PARMCI_NbPutValueFloat(src, dst, proc, nb_handle);
    return ret;
}


int ARMCI_NbPutValueInt(int src, void *dst, int proc, armci_hdl_t *nb_handle)
{
    int ret;
    ret = PARMCI_NbPutValueInt(src, dst, proc, nb_handle);
    return ret;
}


int ARMCI_NbPutValueLong(long src, void *dst, int proc, armci_hdl_t *nb_handle)
{
    int ret;
    ret = PARMCI_NbPutValueLong(src, dst, proc, nb_handle);
    return ret;
}


int ARMCI_Put(void *src, void *dst, int bytes, int proc)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE], 1, proc);
    }
    ret = PARMCI_Put(src, dst, bytes, proc);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_PutS(void *src_ptr, int *src_stride_arr, void *dst_ptr, int *dst_stride_arr, int *count, int stride_levels, int proc)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE], 1, proc);
    }
    ret = PARMCI_PutS(src_ptr, src_stride_arr, dst_ptr, dst_stride_arr, count, stride_levels, proc);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_PutS_flag(void *src_ptr, int *src_stride_arr, void *dst_ptr, int *dst_stride_arr, int *count, int stride_levels, int *flag, int val, int proc)
{
    int ret;
    ret = PARMCI_PutS_flag(src_ptr, src_stride_arr, dst_ptr, dst_stride_arr, count, stride_levels, flag, val, proc);
    return ret;
}


int ARMCI_PutS_flag_dir(void *src_ptr, int *src_stride_arr, void *dst_ptr, int *dst_stride_arr, int *count, int stride_levels, int *flag, int val, int proc)
{
    int ret;
    ret = PARMCI_PutS_flag_dir(src_ptr, src_stride_arr, dst_ptr, dst_stride_arr, count, stride_levels, flag, val, proc);
    return ret;
}


int ARMCI_PutV(armci_giov_t *darr, int len, int proc)
{
    int ret;
    ++message_counter;
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE], 1, proc);
    }
    ret = PARMCI_PutV(darr, len, proc);
    if (message_counter >= message_sample) {
        ++my_counters[LOCAL_WRITE_END];
        PARMCI_Rmw(ARMCI_FETCH_AND_ADD_LONG, &my_counters[RMW_DUMMY],
                &counters[proc][REMOTE_WRITE_END], 1, proc);
        message_counter = 0;
    }
    return ret;
}


int ARMCI_PutValueDouble(double src, void *dst, int proc)
{
    int ret;
    ret = PARMCI_PutValueDouble(src, dst, proc);
    return ret;
}


int ARMCI_PutValueFloat(float src, void *dst, int proc)
{
    int ret;
    ret = PARMCI_PutValueFloat(src, dst, proc);
    return ret;
}


int ARMCI_PutValueInt(int src, void *dst, int proc)
{
    int ret;
    ret = PARMCI_PutValueInt(src, dst, proc);
    return ret;
}


int ARMCI_PutValueLong(long src, void *dst, int proc)
{
    int ret;
    ret = PARMCI_PutValueLong(src, dst, proc);
    return ret;
}


int ARMCI_Put_flag(void *src, void *dst, int bytes, int *f, int v, int proc)
{
    int ret;
    ret = PARMCI_Put_flag(src, dst, bytes, f, v, proc);
    return ret;
}


int ARMCI_Rmw(int op, void *ploc, void *prem, int extra, int proc)
{
    int ret;
    ret = PARMCI_Rmw(op, ploc, prem, extra, proc);
    return ret;
}


int ARMCI_Test(armci_hdl_t *nb_handle)
{
    int ret;
    ret = PARMCI_Test(nb_handle);
    return ret;
}


void ARMCI_Unlock(int mutex, int proc)
{
    
    PARMCI_Unlock(mutex, proc);
    
}


int ARMCI_Wait(armci_hdl_t *nb_handle)
{
    int ret;
    ret = PARMCI_Wait(nb_handle);
    return ret;
}


int ARMCI_WaitAll()
{
    int ret;
    ret = PARMCI_WaitAll();
    return ret;
}


int ARMCI_WaitProc(int proc)
{
    int ret;
    ret = PARMCI_WaitProc(proc);
    return ret;
}


void armci_msg_barrier()
{
    
    parmci_msg_barrier();
    
}


void armci_msg_group_barrier(ARMCI_Group *group)
{
    
    parmci_msg_group_barrier(group);
    
}


int armci_notify(int proc)
{
    int ret;
    ret = parmci_notify(proc);
    return ret;
}


int armci_notify_wait(int proc, int *pval)
{
    int ret;
    ret = parmci_notify_wait(proc, pval);
    return ret;
}

