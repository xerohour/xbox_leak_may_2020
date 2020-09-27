/*
 * Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: D3DRENDR.C                                                        *
*   The Direct 3d Rendereing routines.                                      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/02/97 - re-wrote and cleaned up.     *
*                                                                           *
\***************************************************************************/
#include <windows.h>
#include "ddrawi.h"
#include "d3dhal.h"

DWORD __stdcall nvGetState
(
    LPD3DHAL_GETSTATEDATA   pgsd
)
{
    /*
     * NOTES:
     *
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
