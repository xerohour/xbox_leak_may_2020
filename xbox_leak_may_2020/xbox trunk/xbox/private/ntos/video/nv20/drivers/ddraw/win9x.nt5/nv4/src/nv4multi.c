#ifdef  NV4
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4MULTI.C                                                        *
*   NV4 DX6 Multi-texture routines.                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       04/24/98 - created                      *
*                                                                           *
\***************************************************************************/
#include <math.h>
#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "nvddobj.h"
#include "d3dinc.h"
#include "nvd3dmac.h"
#include "nv3ddbg.h"
#include "nv4vxmac.h"

#ifdef  NVD3D_DX6

#ifndef NV_FASTLOOPS
void nvDX6FlexTriangleSetup
(
    WORD            wPrimCount,
    DWORD           dwStrides,
    LPBYTE          lpVertices
)
{
    if (pCurrentContext->dwStateChange)
    {
        nvSetMultiTextureHardwareState();
        if (!pCurrentContext->bUseDX6Class)
        {
            /*
             * The number of texture stages was reduced to 1.
             * Use DX5 Class instead of DX6 class.
             */
            nvDX5FlexTriangleSetup(wPrimCount, dwStrides, lpVertices);
            return;
        }
    }
    nvSetDx6MultiTextureState(pCurrentContext);
    /*
     * Now calculate the appropriate rendering routine and call it.
     *
     * Determine the fog table mode based on if fog is enabled and the
     * selected fog table mode.  A computed value of 0 = vertex or no fog,
     * non-zero values indicate the fog table mode.
     */
    CALC_FUNCTION_INDEX (pCurrentContext);

    fnDX6FlexTable[pCurrentContext->dwFunctionLookup](wPrimCount, dwStrides, lpVertices);
    return;
}

void nvDX6FlexIndexedTriangleSetup
(
    DWORD           dwPrimCount,
    LPWORD          lpwIndices,
    DWORD           dwStrides,
    LPBYTE          lpVertices
)
{
    if (pCurrentContext->dwStateChange)
    {
        nvSetMultiTextureHardwareState();
        if (!pCurrentContext->bUseDX6Class)
        {
            /*
             * The number of texture stages was reduced to 1.
             * Use DX5 Class instead of DX6 class.
             */
            nvDX5FlexIndexedTriangleSetup(dwPrimCount, lpwIndices, dwStrides, lpVertices);
            return;
        }
    }
    nvSetDx6MultiTextureState(pCurrentContext);
    /*
     * Now calculate the appropriate rendering routine and call it.
     *
     * Determine the fog table mode based on if fog is enabled and the
     * selected fog table mode.  A computed value of 0 = vertex or no fog,
     * non-zero values indicate the fog table mode.
     */
    CALC_FUNCTION_INDEX (pCurrentContext);

    fnDX6FlexIndexedTable[pCurrentContext->dwFunctionLookup](dwPrimCount, lpwIndices, dwStrides, lpVertices);
    return;
}

#endif //!NV_FASTLOOPS

#endif  // NVD3D_DX6

#endif  // NV4