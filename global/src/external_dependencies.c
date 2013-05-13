#if HAVE_CONFIG_H
#   include "config.h"
#endif

/*
 * author: Ryan Olson
 *
 * DISCLAIMER
 * 
 * This material was prepared as an account of work sponsored by an
 * agency of the United States Government.  Neither the United States
 * Government nor the United States Department of Energy, nor Battelle,
 * nor any of their employees, MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
 * ASSUMES ANY LEGAL LIABILITY OR RESPONSIBILITY FOR THE ACCURACY,
 * COMPLETENESS, OR USEFULNESS OF ANY INFORMATION, APPARATUS, PRODUCT,
 * SOFTWARE, OR PROCESS DISCLOSED, OR REPRESENTS THAT ITS USE WOULD NOT
 * INFRINGE PRIVATELY OWNED RIGHTS.
 * 
 * 
 * ACKNOWLEDGMENT
 * 
 * This software and its documentation were produced with United States
 * Government support under Contract Number DE-AC06-76RLO-1830 awarded by
 * the United States Department of Energy.  The United States Government
 * retains a paid-up non-exclusive, irrevocable worldwide license to
 * reproduce, prepare derivative works, perform publicly and display
 * publicly by or for the US Government, including the right to
 * distribute to other US Government contractors.
 */

#if HAVE_STDIO_H
#   include <stdio.h>
#endif
#if HAVE_STRING_H
#   include <string.h>
#endif
#if HAVE_SYS_TYPES_H
#   include <sys/types.h>
#endif
#if HAVE_UNISTD_H
#   include <unistd.h>
#endif

#include "farg.h"
#include "globalp.h"
#include <armci.h>
#include "ga-papi.h"
#include "ga-wapi.h"

#define ARMCI 1

#if defined(SUN)
  void fflush();
#endif


void ghost_pdspgv_(void* arg1, void* arg2, void* arg3, void* arg4,
                   void* arg5, void* arg6,
                   void* arg7, void* arg8,
                   void* arg9,
                   void* arg10, void* arg11,
                   void* arg12, void* arg13,
                   void* arg14, void* arg15, void* arg16)
{
     fprintf(stdout,"pdspgv_ is not implemented in this GA build.\nThis function is provided by NWChem. If you are running NWChem, check the link order and ensure pdspgv_ does not come from the GA library.");
     fflush(stdout);
     abort();
}
void pdspgv_(void* arg1, void* arg2, void* arg3, void* arg4,
                   void* arg5, void* arg6,
                   void* arg7, void* arg8,
                   void* arg9,
                   void* arg10, void* arg11,
                   void* arg12, void* arg13,
                   void* arg14, void* arg15, void* arg16) __attribute__ ((weak, alias ("ghost_pdspgv_")));


void ghost_pdspev_(void* arg1, void* arg2, void* arg3, void* arg4,
                   void* arg5, void* arg6,
                   void* arg7, void* arg8,
                   void* arg9,
                   void* arg10, void* arg11,
                   void* arg12, void* arg13)
{
     fprintf(stdout,"pdspev_ is not implemented in this GA build.\nThis function is provided by NWChem. If you are running NWChem, check the link order and ensure pdspev_ does not come from the GA library.");
     fflush(stdout);
     abort();
}
void pdspev_(void* arg1, void* arg2, void* arg3, void* arg4,
             void* arg5, void* arg6,
             void* arg7, void* arg8,
             void* arg9,
             void* arg10, void* arg11,
             void* arg12, void* arg13) __attribute__ ((weak, alias ("ghost_pdspev_")));


void ghost_fmemreq_(void* arg1, void* arg2, void* arg3, void* arg4,
                    void* arg5, void* arg6,
                    void* arg7, void* arg8,
                    void* arg9)
{
     fprintf(stdout,"fmemreq_ is not implemented in this GA build.\nThis function is provided by NWChem. If you are running NWChem, check the link order and ensure fmemreq_ does not come from the GA library.");
     fflush(stdout);
     abort();
}
void fmemreq_(void* arg1, void* arg2, void* arg3, void* arg4,
                    void* arg5, void* arg6,
                    void* arg7, void* arg8,
                    void* arg9) __attribute__ ((weak, alias ("ghost_fmemreq_")));

