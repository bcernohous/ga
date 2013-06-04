#include <stdio.h>
#include <stdlib.h>

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

