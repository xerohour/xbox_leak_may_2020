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
//  Module: nvKelvinAlphaCombiners.cpp
//      routines for setting up the Kelvin alpha combiners from
//      DX6-style texture stages
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        29Mar00         NV20 development
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x20)

//---------------------------------------------------------------------------

// Set an alpha input to to 1 or -1

void nvKelvinACSetAlphaInputOne (PNVD3DCONTEXT pContext, kelvinCombinerInput eInput,
                                 DWORD dwCombinerStage, BOOL bNegate)
{
    DWORD dwICW = 0;

    dwICW |= bNegate ? DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_MAP, _EXPAND_NORMAL) :
                       DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_MAP, _UNSIGNED_INVERT);

    dwICW |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_ALPHA, _FALSE);
    dwICW |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_0);

    pContext->hwState.dwAlphaICW[dwCombinerStage] |= (dwICW << kelvinCombinerInputShift[eInput]);
}

//---------------------------------------------------------------------------

// Select the source for combiner input eInput in combiner stage
// dwCombinerStage from argument dwD3DTSSArg in D3D stage dwD3DStage.
// return TSS_USES_NULL_TEXTURE if we reference a texture we don't have

HRESULT nvKelvinACSelectAlphaInput (PNVD3DCONTEXT pContext, kelvinCombinerInput eInput,
                                    DWORD dwCombinerStage, DWORD dwD3DStage, DWORD dwD3DTSSArg,
                                    BOOL bComplement, BOOL bExpandNormal)
{
    DWORD dwICW = 0;
    DWORD dwCurrentUsed =FALSE;
    int i;

    switch (pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_SELECTMASK) {
        case D3DTA_CURRENT:
            if (dwD3DStage != 0)
            {
                for (i=dwD3DStage - 1;i>=0;i--)
                {
                    if (pContext->tssState[i].dwValue[D3DTSS_RESULTARG] != D3DTA_TEMP)
                    {
                        dwCurrentUsed=TRUE;
                        break;
                    }
                }
            
            }
            if (dwCurrentUsed == TRUE) {
                // Output from previous stage
                dwICW |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_C);
                break;
            }
            // else, default to diffuse below
        case D3DTA_DIFFUSE:
            dwICW |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_4);
            break;
        case D3DTA_TEXTURE:
            if (!(pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXTUREMAP])) {
                // if we don't actually have a texture, default to the 'current' alpha.
                // yes, this is actually the d3d default: select texture with no texture.
                // my mama done told me, my name was duh microsoft.
                return (TSS_USES_NULL_TEXTURE);
            }
            else {
                if (pContext->hwState.dwNextAvailableTextureUnit < KELVIN_NUM_TEXTURES) {
                    pContext->hwState.dwTexUnitToTexStageMapping[pContext->hwState.dwNextAvailableTextureUnit] = dwD3DStage;
                    dwICW |= DRF_NUM (097, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, KELVIN_COMBINER_REG_TEX0 + pContext->hwState.dwNextAvailableTextureUnit);
                }
                else {
                    pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
                    dwICW |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_0);
                }
            }
            break;
        case D3DTA_TFACTOR:
            dwICW |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_1);
            break;
        case D3DTA_TEMP:
            dwICW |= DRF_DEF(097, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_D);
            break;
        case D3DTA_SPECULAR:
            dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_5);
            pContext->hwState.dwStateFlags |= KELVIN_FLAG_COMBINERSNEEDSPECULAR;
            DPF ("Specular Alpha doesn't exist in Kelvin, rendering corruption will result.",
                 pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_SELECTMASK);
            dbgD3DError();
            pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
            break;
        default:
            DPF ("unknown / illegal alpha argument '%d' in nvKelvinACSelectAlphaInput",
                 pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_SELECTMASK);
            dbgD3DError();
            pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
            break;
    }

    // handle the alpha replicate and complement flags

    dwICW |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_ALPHA, _TRUE);

    bComplement = ((pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_COMPLEMENT) ^
                   ((bComplement) ? D3DTA_COMPLEMENT : 0))
                  ?
                  TRUE : FALSE;

    if (bExpandNormal) {
        dwICW |= bComplement ? DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_MAP, _EXPAND_NEGATE) :
                               DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_MAP, _EXPAND_NORMAL);
    }
    else {
        dwICW |= bComplement ? DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_MAP, _UNSIGNED_INVERT) :
                               DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_MAP, _UNSIGNED_IDENTITY);
    }

    pContext->hwState.dwAlphaICW[dwCombinerStage] |= (dwICW << kelvinCombinerInputShift[eInput]);
    return (0);
}

//---------------------------------------------------------------------------

// Select the input into A
static HRESULT nvKelvinACSelect1 (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Make A input
    hr = nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    // Make B 1
    nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
    // C & D default to zero
    return (hr);
}

//---------------------------------------------------------------------------

// Select the input into D
static HRESULT nvKelvinACSelect2 (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply D * 1
    nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputC, dwHWStage, FALSE);
    hr = nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    // A & B default to zero
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACModulate (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * B
    hr  = nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    // C & D default to zero
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACAdd (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * 1
    hr = nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    // Make B 1
    nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
    // Multiply D * 1
    nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputC, dwHWStage, FALSE);
    hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    // Implicit A + D
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACSub (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * 1
    hr = nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
    // Multiply D * -1
    nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputC, dwHWStage, TRUE);
    hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    // Implicit A + -D
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACAddSmooth (PNVD3DCONTEXT pContext,DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * 1
    hr = nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
    // Make C: (1-arg1), make D: arg2
    hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputC, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, TRUE, FALSE);
    hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACBlendAlpha (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage, DWORD dwType)
{
    HRESULT hr;
    DWORD dwTemp;

    // Multiply A * B(alpha)
    hr = nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);

    if (dwType == TEXTUREPM) { // this type defined by us
        // Make B 1 since texture is already pre-multiplied
        nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
        // set type again for making C (1-alpha) below
        dwType = D3DTA_TEXTURE;
    }
    else {
        // Make B alpha, appropriate type
        dwTemp = pContext->tssState[dwD3DStage].dwValue[D3DTSS_ALPHAARG2];
        pContext->tssState[dwD3DStage].dwValue[D3DTSS_ALPHAARG2] = dwType;
        hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
        // copy arg2 back
        pContext->tssState[dwD3DStage].dwValue[D3DTSS_ALPHAARG2] = dwTemp;
    }

    // Make C: 1-alpha, make D: arg2
    dwTemp = pContext->tssState[dwD3DStage].dwValue[D3DTSS_ALPHAARG1];
    pContext->tssState[dwD3DStage].dwValue[D3DTSS_ALPHAARG1] = dwType;
    hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputC, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, TRUE, FALSE);
    // copy back to alphaarg1
    pContext->tssState[dwD3DStage].dwValue[D3DTSS_ALPHAARG1] = dwTemp;
    hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

// The way D3D does this, it essentially allows using texture from stage 1
// in stage1 twice.  D3D does not normally allow you to use a texture in
// stage 0 and stage1.  This way, D3D allows you to modulate in (effectively)
// stage 0 and then do whatever it is you are going to do in stage1.  The
// key is that we use the texture from stage1 (since there is really no texture
// in stage 0)
static HRESULT nvKelvinACPremodulate (PNVD3DCONTEXT pContext,DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    DWORD   dwTemp;

    DPF ("pre-modulation is totally broken right now");
    dbgD3DError();

    if (dwD3DStage == 0) {
        if(pContext->tssState[dwD3DStage+1].dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE){
            // make A: alphaArg1(0), make B: texture(1)
             hr = nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
             dwTemp = pContext->tssState[dwD3DStage].dwValue[D3DTSS_ALPHAARG2];
             pContext->tssState[dwD3DStage].dwValue[D3DTSS_ALPHAARG2] = D3DTA_TEXTURE;
             hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
             // copy back arg2
             pContext->tssState[dwD3DStage].dwValue[D3DTSS_ALPHAARG2] = dwTemp;
        }
        else{
            //asked to do premodulate on a single texture stage -- just pass down
            //the colorarg1
            hr = nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
            nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
        }

    }
    else { // well, right now we only handle two stages (we could compress these)
        // if pre-modulate is called in stage1, we could modulate with
        // texture from next stage. But, we'd have to handle settexture a bit
        // differently. For now, we'll just use selectarg1
        hr = nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
        nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
    }

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACMultiplyAdd (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // set A to arg1
    hr  = nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG0, FALSE, FALSE);
    //set B to 1
    nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputB, hdStage, FALSE);
    // set C to arg2
    hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputC, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    //set D to arg3
    hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputD, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACLerp (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * B
    hr  = nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG0, FALSE, FALSE);
    hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputB, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    //set C to 1 - arg1
    hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputC, hdStage, dxStage, D3DTSS_ALPHAARG0, TRUE, FALSE);
    //set D to arg3
    hr |= nvKelvinACSelectAlphaInput (pContext, kelvinCombinerInputD, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

void nvConstructKelvinOutputAlphaCombiners (PNVD3DCONTEXT pContext, BOOL bEnable,
                                            DWORD dwHWStage, DWORD dwD3DStage,
                                            BOOL bBias, DWORD dwShift, BOOL bOutTemp)
{
    bOutTemp &= (dwD3DStage != 8);

    if ((!bEnable) ||
        (pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLOROP] == D3DTOP_DOTPRODUCT3))
    {
        pContext->hwState.dwAlphaOCW[dwHWStage] |= (DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_SUM_DST, _REG_0)  |
                                                    DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_AB_DST, _REG_0)   |
                                                    DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_CD_DST, _REG_0));
        return;
    }

    // select bias operation
    switch (dwShift) {
        case 0:
            pContext->hwState.dwAlphaOCW[dwHWStage] |= bBias ?
                                                       DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_OPERATION, _NOSHIFT_BIAS) :
                                                       DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_OPERATION, _NOSHIFT);
            break;
        case 1:
            pContext->hwState.dwAlphaOCW[dwHWStage] |= bBias ?
                                                       DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_OPERATION, _SHIFTLEFTBY1_BIAS) :
                                                       DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_OPERATION, _SHIFTLEFTBY1);
            break;
        case 2:
            pContext->hwState.dwAlphaOCW[dwHWStage] |= bBias ?
                                                       // we can't shift by 2 and bias. do the best we can...
                                                       DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_OPERATION, _SHIFTLEFTBY1_BIAS) :
                                                       DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_OPERATION, _SHIFTLEFTBY2);
            break;
        default:
            DPF ("illegal shift in nvConstructKelvinOutputAlphaCombiners");
            dbgD3DError();
            break;
    }  // switch

    pContext->hwState.dwAlphaOCW[dwHWStage] |= (DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_MUX_ENABLE, _FALSE) |
                                                DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_AB_DST, _REG_0)     |
                                                DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_CD_DST, _REG_0));
    pContext->hwState.dwAlphaOCW[dwHWStage] |= bOutTemp ?
                                               DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_SUM_DST, _REG_D) :
                                               DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_SUM_DST, _REG_C);
}

//---------------------------------------------------------------------------

void nvConstructKelvinAlphaCombiners (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage, BOOL bStageActive)
{
    DWORD   dwShift;
    BOOL    bBias, bEnable;
    HRESULT hr = 0;

    dwShift = 0;
    bBias   = FALSE;
    bEnable = TRUE;

    pContext->hwState.dwAlphaICW[dwHWStage] = 0;
    pContext->hwState.dwAlphaOCW[dwHWStage] = 0;

    if (!bStageActive) {
        // colorop was set to disable
        if (dwHWStage == 0) { // only construct default for stage 0
            // Default is diffuse alpha, make A=diffuse alpha, B=1, C=D=0
            pContext->hwState.dwAlphaICW[dwHWStage] |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_SOURCE, _REG_4);
            pContext->hwState.dwAlphaICW[dwHWStage] |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE);
            nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
        }
        else {
            bEnable = FALSE;
        }
    }

    else {

        switch (pContext->tssState[dwD3DStage].dwValue[D3DTSS_ALPHAOP]) {
            default:
            case D3DTOP_DOTPRODUCT3:
                // We treat dot3 like disable because it doesn't really matter.
                // if the colorop is also dot3, then alpha will be overwritten momentarily anyhow.
                // BUGBUG if the colorop is NOT dot3, what the heck does this even mean???
            case D3DTOP_DISABLE:
                if (dwHWStage == 0) { // only construct default for stage 0
                    // Default is diffuse alpha, make A=diffuse alpha, B=1, C=D=0
                    pContext->hwState.dwAlphaICW[dwHWStage] |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_SOURCE, _REG_4);
                    pContext->hwState.dwAlphaICW[dwHWStage] |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE);
                    nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
                }
                else {
                    bEnable = FALSE;
                }
                break;
            case D3DTOP_SELECTARG1:
                hr = nvKelvinACSelect1 (pContext, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_SELECTARG2:
                hr = nvKelvinACSelect2 (pContext, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_MODULATE4X:
                dwShift++;
            case D3DTOP_MODULATE2X:
                dwShift++;
            case D3DTOP_MODULATE:
                hr = nvKelvinACModulate (pContext, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_ADDSIGNED2X: // same as addsigned with following change
                dwShift = 1;
            case D3DTOP_ADDSIGNED:   // same as add with following change
                bBias = TRUE;
            case D3DTOP_ADD:
                hr = nvKelvinACAdd (pContext, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_SUBTRACT:
                hr = nvKelvinACSub (pContext, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_ADDSMOOTH:
                hr = nvKelvinACAddSmooth (pContext, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_BLENDDIFFUSEALPHA:
                hr = nvKelvinACBlendAlpha (pContext, dwHWStage, dwD3DStage, D3DTA_DIFFUSE);
                break;
            case D3DTOP_BLENDTEXTUREALPHA:
                hr = nvKelvinACBlendAlpha (pContext, dwHWStage, dwD3DStage, D3DTA_TEXTURE);
                break;
            case D3DTOP_BLENDFACTORALPHA:
                hr = nvKelvinACBlendAlpha (pContext, dwHWStage, dwD3DStage, D3DTA_TFACTOR);
                break;
            case D3DTOP_BLENDTEXTUREALPHAPM:
                hr = nvKelvinACBlendAlpha (pContext,dwHWStage, dwD3DStage, TEXTUREPM);
                break;
            case D3DTOP_BLENDCURRENTALPHA:
                hr = nvKelvinACBlendAlpha (pContext, dwHWStage, dwD3DStage, D3DTA_CURRENT);
                break;
            case D3DTOP_PREMODULATE:
                hr = nvKelvinACPremodulate (pContext, dwHWStage, dwD3DStage);
                break;
// BUGBUG
#if 0
            case D3DTOP_ALPHAMODULATE:
                // This is the case where the old D3DRENDERSTATE_TEXTUREMAPBLEND was used with
                // the D3DTBLEND_MODULATE op because the dump translates this into TSS we define
                // a new op that properly handles D3DTBLEND_MODULATE
                // Alpha is obtained by the texture, if it has alpha otherwise it is obtained from
                // the vertex. The dump always sets arg1 to be texture and arg2 to be current
                if (statev.ptexture[0] && statev.ptexture[0]->sd.ddpfPixelFormat.dwRGBAlphaBitMask) {
                    nvKelvinACSelect1(dwHWStage);
                }
                else {
                    nvKelvinACSelect2(dwHWStage);
                }
                break;
#endif
            case D3DTOP_MULTIPLYADD:
                hr = nvKelvinACMultiplyAdd (pContext, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_LERP:
                hr = nvKelvinACLerp (pContext, dwHWStage, dwD3DStage);
                break;
        }  // switch

    }

    if (hr == TSS_USES_NULL_TEXTURE) {
        // this stage references a non-existent texture. just set it back to "select current"
        if (dwHWStage == 0) {
            // current = diffuse in stage 0
            pContext->hwState.dwAlphaICW[dwHWStage] = DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_SOURCE, _REG_4) |
                                                          DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE);
        }
        else {
            pContext->hwState.dwAlphaICW[dwHWStage] = DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_SOURCE, _REG_C) |
                                                          DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE);
        }
        nvKelvinACSetAlphaInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
    }

    nvConstructKelvinOutputAlphaCombiners (pContext, bEnable, dwHWStage, dwD3DStage, bBias, dwShift,
                                           pContext->tssState[dwD3DStage].dwValue[D3DTSS_RESULTARG] == D3DTA_TEMP);

}

#endif  // NVARCH == 0x20

