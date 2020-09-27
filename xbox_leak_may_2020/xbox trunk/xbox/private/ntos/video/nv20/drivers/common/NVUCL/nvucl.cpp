/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVUCL.CPP                                                         *
*   Basic interface fulfillment routines                                    *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 07/06/98 - Created                      *
*                                                                           *
\***************************************************************************/
#include "precomp.h"
#include "disasm.h"
#include "matrix.h"
#include "profiler.h"

DWORD g_dwLastError;

/*
 * nvuclGetInterface
 *
 * instanciate a object of class dwIdentifier & returns it through 
 * ppInterface
 */
NVUCL_RESULT NVUCL_EXPORT NVUCL_STDCALL nvuclGetInterface
(
    DWORD   dwIdentifier,
    void  **ppInterface
)
{
    /*
     * setup
     */
    *ppInterface  = NULL;
    g_dwLastError = NVUCL_ERROR_UNKNOWN;

    /*
     * create object
     */
    switch (dwIdentifier)
    {
        /*
         * profiler
         */
        case NVUCL_PROFILER_ID:
            *ppInterface = (void*)(new UCL_PROFILER);
            break;
        /*
         * matrix
         */
        case NVUCL_MATRIX_ID:
            *ppInterface = (void*)(new UCL_MATRIX);
            break;
        case NVUCL_DISASM_ID:
            *ppInterface = (void*)(new UCL_DISASM);
            break;
        /*
         * unknown
         */
        default:
            return NVUCL_ERROR_UNKNOWNINTERFACE;
    }

    /*
     * return
     */
    return (*ppInterface) ? NVUCL_OK : g_dwLastError;
}
