/*
 * Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: d3dState.cpp                                                      *
*   The Direct3D state routines.                                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/02/97 - re-wrote and cleaned up.     *
*                                                                           *
\***************************************************************************/

#include "nvprecomp.h"
#if (NVARCH >= 0x04)


//-------------------------------------------------------------------------

#ifndef WINNT  // these calls are obsolete on NT (DX7)

DWORD __stdcall nvGetState
(
    LPD3DHAL_GETSTATEDATA pgsd
)
{
    /*
     * NOTES:
     * This callback is called when Direct3D requires information about
     * the state of a particular stage in the pipeline. If you only handle
     * rasterisation then you only need to respond to D3DHALSTATE_GET_RENDER
     * calls.
     * The state wanted is in pgsd->ddState.drstRenderStateType.
     * Return the answer in pgsd->ddState.dwArg[0].
     */
    if (!pgsd->dwhContext)
    {
        pgsd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    if (pgsd->dwWhich != D3DHALSTATE_GET_RENDER)
    {
        pgsd->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }
    pgsd->ddState.dwArg[0] = 1;
    pgsd->ddrval           = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}

#endif  // !WINNT

//remove these defines when they get into the MS runtime.
typedef struct    _D3DDEVINFO_VCACHEPARAM
{
DWORD Pattern; // bit pattern,return value must be FOUR_CC('C', 'A', 'C', 'H')
DWORD OptMethod;//optimization method 0 means longest strips, 1 means vertex cache based
DWORD CacheSize;// cache size to optimize for  (only required if type is 1)
DWORD MagicNumber; // used to determine when to restart strips 
} D3DDEVINFO_VCACHEPARAM;
 
#define D3DDEVINFOID_VCACHEPARAM   4
//end remove


//-------------------------------------------------------------------------
// This fdunction is a general purpose callback.  Almost ANYTHING can be passed
// back and forth.  It is currently used for Vertex Cache information and texture
// management, but is easily extended.


DWORD __stdcall nvGetDriverState
(
    LPDDHAL_GETDRIVERSTATEDATA pgdsd
)
{
    DWORD dwHandled=DDHAL_DRIVER_NOTHANDLED;
    PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)pgdsd->dwhContext;

    dbgTracePush ("nvGetDriverState");
    
    nvSetDriverDataPtrFromContext(pContext);

#if (NVARCH >= 0x010)    
    if (pgdsd->dwFlags == D3DDEVINFOID_VCACHEPARAM && 
        pgdsd->dwLength >= sizeof(D3DDEVINFO_VCACHEPARAM) )
    {
        D3DDEVINFO_VCACHEPARAM *lpVCache = (D3DDEVINFO_VCACHEPARAM *)pgdsd->lpdwStates;

        lpVCache->Pattern = mmioFOURCC('C', 'A', 'C', 'H');
        lpVCache->OptMethod = 1;
        lpVCache->MagicNumber = 7;
#if (NVARCH >= 0x020)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) 
        {
            lpVCache->CacheSize = 16;
        }
        else 
#endif
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) 
        {
            lpVCache->CacheSize = 12;
        }
        pgdsd->ddRVal = DD_OK;
        dwHandled = DDHAL_DRIVER_HANDLED;
    }
#endif
    dbgTracePop();
    DDHAL_EXIT(dwHandled);
}

#endif  // NVARCH >= 0x04

