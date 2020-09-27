// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: nvhwState.kelvin.cpp
//      Kelvin state management routines.
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        26Apr2000         NV20 development
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x20)

//---------------------------------------------------------------------------

#define KELVIN_VALIDATE_FAIL(reason)    \
{                                       \
    pvtssd->dwNumPasses = 0xffffffff;   \
    pvtssd->ddrval = reason;            \
    dbgTracePop();                      \
    return (DDHAL_DRIVER_HANDLED);      \
}

//---------------------------------------------------------------------------

DWORD nvKelvinValidateTextureStageState
(
    LPD3DHAL_VALIDATETEXTURESTAGESTATEDATA pvtssd
)
{
    dbgTracePush ("nvKelvinValidateTextureStageState");

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pvtssd);

    // update everything
    pContext->hwState.dwDirtyFlags = KELVIN_DIRTY_REALLY_FILTHY;
    nvSetKelvinState (pContext);

    // make sure the setup went smoothly
    if (pContext->hwState.dwStateFlags & KELVIN_FLAG_SETUPFAILURE) {
        KELVIN_VALIDATE_FAIL (D3DERR_CONFLICTINGRENDERSTATE);
    }

    // everything is kosher. set return values and return OK
    pvtssd->dwNumPasses = 1;
    pvtssd->ddrval      = D3D_OK;

    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

//---------------------------------------------------------------------------

// returns the size (in bytes) of an inlined vertex

DWORD nvKelvinInlineVertexStride
(
    PNVD3DCONTEXT pContext
)
{
    DWORD dwStride, dwHWStage, dwD3DStage, dwTCIndex, dwVAIndex;

    CVertexShader *pShader = pContext->pCurrentVShader;

    dwStride  = 0;
    dwStride += pShader->getVASize(defaultInputRegMap[D3DVSDE_POSITION]);
    dwStride += pShader->getVASize(defaultInputRegMap[D3DVSDE_BLENDWEIGHT]);
    dwStride += pShader->getVASize(defaultInputRegMap[D3DVSDE_NORMAL]);
    dwStride += pShader->getVASize(defaultInputRegMap[D3DVSDE_DIFFUSE]);
    dwStride += pShader->getVASize(defaultInputRegMap[D3DVSDE_SPECULAR]);
    // fog distance
    dwStride += pShader->getVASize(defaultInputRegMap[D3DVSDE_PSIZE]);
    // back diffuse
    // back specular
    for (dwHWStage=0; dwHWStage < KELVIN_NUM_TEXTURES; dwHWStage++) {
        if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(dwHWStage)) {
            // the app needs texture N and provided us with one
            dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
            nvAssert (dwD3DStage != KELVIN_UNUSED);
            dwTCIndex = (pContext->hwState.dwTexCoordIndices >> (8*dwHWStage)) & 0xff;
            dwVAIndex = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            nvAssert (pShader->getVASize(dwVAIndex));  // they better have given us coordinates
            dwStride += pShader->getVASize(dwVAIndex);
        }
    }

    return (dwStride);
}

#endif // (NVARCH >= 0x020)

