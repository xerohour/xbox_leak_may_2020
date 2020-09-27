/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvCelsiusAlphaCombiner.cpp                                        *
*       Routines for programming the celsiusalpha combiner                  *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler        02Mar99         NV10 development            *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

//---------------------------------------------------------------------------

// Set an alpha input to to 1 or -1

void SetAlphaInputOne (PNVD3DCONTEXT pContext, celsiusCombinerInput eInput,
                       DWORD dwCombinerStage, BOOL bNegate)
{
    DWORD dwICW = 0;

    dwICW |= bNegate ? DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_MAP, _EXPAND_NORMAL) :
                       DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_MAP, _UNSIGNED_INVERT);

    dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_ALPHA, _FALSE);
    dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_0);

    pContext->hwState.dwAlphaICW[dwCombinerStage] |= (dwICW << celsiusCombinerInputShift[eInput]);
}

//---------------------------------------------------------------------------

// Select the source for combiner input eInput in combiner stage
// dwCombinerStage from argument dwD3DTSSArg in D3D stage dwD3DStage.
// return TSS_USES_NULL_TEXTURE if we reference a texture we don't have

HRESULT SelectAlphaInput (PNVD3DCONTEXT pContext, celsiusCombinerInput eInput,
                          DWORD dwCombinerStage, DWORD dwD3DStage, DWORD dwD3DTSSArg,
                          BOOL bComplement, BOOL bExpandNormal)
{
    DWORD dwICW = 0;

    switch (pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_SELECTMASK) {
        case D3DTA_CURRENT:
            if (dwD3DStage != 0 && (pContext->tssState[0].dwValue[D3DTSS_RESULTARG] != D3DTA_TEMP)) {
            // Output from previous stage
                dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_C);
                break;
            }
            // else, default to diffuse below
        case D3DTA_DIFFUSE:
            dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_4);
            break;
        case D3DTA_TEXTURE:
            if (!(pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXTUREMAP])) {
                // if we don't actually have a texture, default to the 'current' alpha.
                // yes, this is actually the d3d default: select texture with no texture.
                // my mama done told me, my name was duh microsoft.
                return (TSS_USES_NULL_TEXTURE);
            }
            else {
                pContext->hwState.dwTexUnitToTexStageMapping[pContext->hwState.dwNextAvailableTextureUnit] = dwD3DStage;
                dwICW |= DRF_NUM(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, CELSIUS_COMBINER_REG_TEX0 + pContext->hwState.dwNextAvailableTextureUnit);
            }
            break;
        case D3DTA_TFACTOR:
            dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_1);
            break;
        case D3DTA_TEMP:
            dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_D);
            break;
        case D3DTA_SPECULAR: //specular is an invalid arg.
            dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_5);
            
            DPF ("Specular Alpha doesn't exist in Celsius, rendering corruption will result.",
                 pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_SELECTMASK);
            
            dbgD3DError();
            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_COMBINERSNEEDSPECULAR;
            
            break;
        default:
            DPF ("unknown / illegal alpha argument '%d' in SelectAlphaInput",
                 pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_SELECTMASK);
            dbgD3DError();
            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
            break;
    }

    // now handle the alpha replicate and complement flags

    if (((pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_SELECTMASK) == D3DTA_CURRENT) &&
         (dwCombinerStage != 0) &&
         (pContext->hwState.dwStateFlags & CELSIUS_FLAG_DOTPRODUCT3(dwCombinerStage - 1))) {
        // if the last stage produced a dot product, we were supposed to have replicated the result
        // into alpha, but couldn't b/c celsius can't. get it from the color channels now.
        dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_ALPHA, _FALSE);
    }
    else {
        // just get alpha from alpha
        dwICW |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_ALPHA, _TRUE);
    }

    bComplement = ((pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_COMPLEMENT) ^
                   ((bComplement) ? D3DTA_COMPLEMENT : 0))
                  ?
                  TRUE : FALSE;

    if (bExpandNormal) {
        dwICW |= bComplement ? DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_MAP, _EXPAND_NEGATE) :
                               DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_MAP, _EXPAND_NORMAL);
    }
    else {
        dwICW |= bComplement ? DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_MAP, _UNSIGNED_INVERT) :
                               DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_D_MAP, _UNSIGNED_IDENTITY);
    }

    pContext->hwState.dwAlphaICW[dwCombinerStage] |= (dwICW << celsiusCombinerInputShift[eInput]);
    return (0);
}

//---------------------------------------------------------------------------

// Select the input into A
static HRESULT combineselect1 (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Make A input
    hr = SelectAlphaInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    // Make B 1
    SetAlphaInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    // C & D default to zero

    return (hr);
}

//---------------------------------------------------------------------------

// Select the input into D
static HRESULT combineselect2 (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply D * 1
    SetAlphaInputOne (pContext, celsiusCombinerInputC, hdStage, FALSE);
    hr = SelectAlphaInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    // A & B default to zero

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combinemodulate (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * B
    hr  = SelectAlphaInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    hr |= SelectAlphaInput (pContext, celsiusCombinerInputB, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    // C & D default to zero

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combineadd (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * 1
    hr = SelectAlphaInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    // Make B 1
    SetAlphaInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    // Multiply D * 1
    SetAlphaInputOne (pContext, celsiusCombinerInputC, hdStage, FALSE);
    hr |= SelectAlphaInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    // Implicit A + D

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combinesub (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * 1
    hr = SelectAlphaInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    SetAlphaInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    // Multiply D * -1
    SetAlphaInputOne (pContext, celsiusCombinerInputC, hdStage, TRUE);
    hr |= SelectAlphaInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    // Implicit A + -D

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combineaddsmooth (PNVD3DCONTEXT pContext,int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * 1
    hr = SelectAlphaInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    SetAlphaInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    // Make C: (1-arg1), make D: arg2
    hr |= SelectAlphaInput (pContext, celsiusCombinerInputC, hdStage, dxStage, D3DTSS_ALPHAARG1, TRUE, FALSE);
    hr |= SelectAlphaInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combineblendalpha (PNVD3DCONTEXT pContext, int hdStage, int dxStage, int type)
{
    HRESULT hr;
    int temp;

    // Multiply A * B(alpha)
    hr = SelectAlphaInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);

    if (type == TEXTUREPM) { // this type defined by us
        // Make B 1 since texture is already pre-multiplied
        SetAlphaInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
        // set type again for making C (1-alpha) below
        type = D3DTA_TEXTURE;
    }
    else {
        // Make B alpha, appropriate type
        temp = pContext->tssState[dxStage].dwValue[D3DTSS_ALPHAARG2];
        pContext->tssState[dxStage].dwValue[D3DTSS_ALPHAARG2] = type;
        hr |= SelectAlphaInput (pContext, celsiusCombinerInputB, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
        // copy arg2 back
        pContext->tssState[dxStage].dwValue[D3DTSS_ALPHAARG2] = temp;
    }

    // Make C: 1-alpha, make D: arg2
    temp = pContext->tssState[dxStage].dwValue[D3DTSS_ALPHAARG1];
    pContext->tssState[dxStage].dwValue[D3DTSS_ALPHAARG1] = type;
    hr |= SelectAlphaInput (pContext, celsiusCombinerInputC, hdStage, dxStage, D3DTSS_ALPHAARG1, TRUE, FALSE);
    // copy back to alphaarg1
    pContext->tssState[dxStage].dwValue[D3DTSS_ALPHAARG1] = temp;
    hr |= SelectAlphaInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

// The way D3D does this, it essentially allows using texture from stage 1
// in stage1 twice.  D3D does not normally allow you to use a texture in
// stage 0 and stage1.  This way, D3D allows you to modulate in (effectively)
// stage 0 and then do whatever it is you are going to do in stage1.  The
// key is that we use the texture from stage1 (since there is really no texture
// in stage 0)
static HRESULT combinepremodulate (PNVD3DCONTEXT pContext,int hdStage, int dxStage)
{
    HRESULT hr;
    int temp, premodulate = 1;

    DPF ("pre-modulation is totally broken right now");
    dbgD3DError();

    if (dxStage == 0) {
        if(pContext->tssState[dxStage+1].dwValue[D3DTSS_ALPHAOP] != D3DTOP_DISABLE){
            // make A: alphaArg1(0), make B: texture(1)
             hr = SelectAlphaInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
             temp = pContext->tssState[dxStage].dwValue[D3DTSS_ALPHAARG2];
             pContext->tssState[dxStage].dwValue[D3DTSS_ALPHAARG2] = D3DTA_TEXTURE;
             hr |= SelectAlphaInput (pContext, celsiusCombinerInputB, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
             // copy back arg2
             pContext->tssState[dxStage].dwValue[D3DTSS_ALPHAARG2] = temp;
        }
        else{
            //asked to do premodulate on a single texture stage -- just pass down
            //the colorarg1
            hr = SelectAlphaInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
            SetAlphaInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
        }

    }
    else { // well, right now we only handle two stages (we could compress these)
        // if pre-modulate is called in stage1, we could modulate with
        // texture from next stage. But, we'd have to handle settexture a bit
        // differently. For now, we'll just use selectarg1
        hr = SelectAlphaInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
        SetAlphaInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    }

    return (hr);
}
//---------------------------------------------------------------------------

static HRESULT combinemultiplyadd (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // set A to arg1
    hr  = SelectAlphaInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG0, FALSE, FALSE);
    //set B to 1
    SetAlphaInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    // set C to arg2
    hr |= SelectAlphaInput (pContext, celsiusCombinerInputC, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    //set D to arg3
    hr |= SelectAlphaInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combinelerp (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * B
    hr  = SelectAlphaInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG0, FALSE, FALSE);
    hr |= SelectAlphaInput (pContext, celsiusCombinerInputB, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    //set C to 1 - arg1
    hr |= SelectAlphaInput (pContext, celsiusCombinerInputC, hdStage, dxStage, D3DTSS_ALPHAARG0, TRUE, FALSE);
    //set D to arg3
    hr |= SelectAlphaInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

void ConstructOutputAlphaCombiners (PNVD3DCONTEXT pContext, BOOL enable, int stage, int bias, int shift, int outtemp)
{
    outtemp  &= (stage != 1);

    if (!enable) {
        pContext->hwState.dwAlphaOCW[stage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _OCW_SUM_DST, _REG_0);
        return;
    }

    // select bias operation
    if (shift == 0) {
        if (bias == 0)
            pContext->hwState.dwAlphaOCW[stage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _OCW_OPERATION, _NOSHIFT);
        else
            pContext->hwState.dwAlphaOCW[stage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _OCW_OPERATION, _NOSHIFT_BIAS);
    }
    else if (shift == 1) {
        if (bias == 0)
            pContext->hwState.dwAlphaOCW[stage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _OCW_OPERATION, _SHIFTLEFTBY1);
        else
            pContext->hwState.dwAlphaOCW[stage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _OCW_OPERATION, _SHIFTLEFTBY1_BIAS);
    }
    else if (shift == 2) {
        if (bias == 0)
            pContext->hwState.dwAlphaOCW[stage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _OCW_OPERATION, _SHIFTLEFTBY2);
        else
            // we can't shift by 2 and bias. do the best we can...
            pContext->hwState.dwAlphaOCW[stage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _OCW_OPERATION, _SHIFTLEFTBY1_BIAS);
    }
    pContext->hwState.dwAlphaOCW[stage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _OCW_MUX_ENABLE, _FALSE);
    if (outtemp)
        pContext->hwState.dwAlphaOCW[stage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _OCW_SUM_DST, _REG_D);
    else
        pContext->hwState.dwAlphaOCW[stage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _OCW_SUM_DST, _REG_C);
    pContext->hwState.dwAlphaOCW[stage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _OCW_AB_DST, _REG_0);
    pContext->hwState.dwAlphaOCW[stage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _OCW_CD_DST, _REG_0);
}

//---------------------------------------------------------------------------

void ConstructAlphaCombiners (PNVD3DCONTEXT pContext, int hdStage, int dxStage, BOOL bStageActive)
{
    int     bias, shift;
    BOOL    enable;
    HRESULT hr = 0;

    bias = 0;
    shift = 0;
    enable = TRUE;

    pContext->hwState.dwAlphaICW[hdStage] = 0;
    pContext->hwState.dwAlphaOCW[hdStage] = 0;

    if (!bStageActive) {
        // colorop was set to disable
        if (hdStage == 0) { // only construct default for stage 0
            // Default is diffuse alpha, make A=diffuse alpha, B=1, C=D=0
            pContext->hwState.dwAlphaICW[hdStage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_A_SOURCE, _REG_4);
            pContext->hwState.dwAlphaICW[hdStage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE);
            SetAlphaInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
        }
        else {
            enable = FALSE;
        }
    }

    else {

        switch (pContext->tssState[dxStage].dwValue[D3DTSS_ALPHAOP]) {
            default:
            case D3DTOP_DOTPRODUCT3:
                // We treat dot3 like disable because it doesn't really matter.
                // if the colorop is also dot3, then alpha will be overwritten momentarily anyhow.
                // BUGBUG if the colorop is NOT dot3, what the heck does this even mean???
            case D3DTOP_DISABLE:
                if (hdStage == 0) { // only construct default for stage 0
                    // Default is diffuse alpha, make A=diffuse alpha, B=1, C=D=0
                    pContext->hwState.dwAlphaICW[hdStage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_A_SOURCE, _REG_4);
                    pContext->hwState.dwAlphaICW[hdStage] |= DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE);
                    SetAlphaInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
                }
                else {
                    enable = FALSE;
                }
                break;
            case D3DTOP_SELECTARG1:
                hr = combineselect1 (pContext, hdStage, dxStage);
                break;
            case D3DTOP_SELECTARG2:
                hr = combineselect2 (pContext, hdStage, dxStage);
                break;
            case D3DTOP_MODULATE4X:
                shift++;
            case D3DTOP_MODULATE2X:
                shift++;
            case D3DTOP_MODULATE:
                hr = combinemodulate (pContext, hdStage, dxStage);
                break;
            case D3DTOP_ADDSIGNED2X: // same as addsigned with following change
                shift = 1;
            case D3DTOP_ADDSIGNED:   // same as add with following change
                bias = 1;
            case D3DTOP_ADD:
                hr = combineadd (pContext, hdStage, dxStage);
                break;
            case D3DTOP_SUBTRACT:
                hr = combinesub (pContext, hdStage, dxStage);
                break;
            case D3DTOP_ADDSMOOTH:
                hr = combineaddsmooth (pContext, hdStage, dxStage);
                break;
            case D3DTOP_BLENDDIFFUSEALPHA:
                hr = combineblendalpha (pContext, hdStage, dxStage, D3DTA_DIFFUSE);
                break;
            case D3DTOP_BLENDTEXTUREALPHA:
                hr = combineblendalpha (pContext, hdStage, dxStage, D3DTA_TEXTURE);
                break;
            case D3DTOP_BLENDFACTORALPHA:
                hr = combineblendalpha (pContext, hdStage, dxStage, D3DTA_TFACTOR);
                break;
            case D3DTOP_BLENDTEXTUREALPHAPM:
                hr = combineblendalpha (pContext,hdStage, dxStage, TEXTUREPM);
                break;
            case D3DTOP_BLENDCURRENTALPHA:
                hr = combineblendalpha (pContext, hdStage, dxStage, D3DTA_CURRENT);
                break;
            case D3DTOP_PREMODULATE:
                hr = combinepremodulate (pContext, hdStage, dxStage);
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
                    combineselect1(hdStage);
                }
                else {
                    combineselect2(hdStage);
                }
                break;
#endif
            case D3DTOP_MULTIPLYADD:
                hr = combinemultiplyadd (pContext, hdStage, dxStage);
                break;
            case D3DTOP_LERP:
                hr = combinelerp (pContext, hdStage, dxStage);
                break;
        }  // switch

    }

    if (hr == TSS_USES_NULL_TEXTURE) {
        // this stage references a non-existent texture. just set it back to "select current"
        if (hdStage == 0) {
            // current = diffuse in stage 0
            pContext->hwState.dwAlphaICW[hdStage] = DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_A_SOURCE, _REG_4) |
                                                    DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE);
        }
        else {
            pContext->hwState.dwAlphaICW[hdStage]  = DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_A_SOURCE, _REG_C);
            pContext->hwState.dwAlphaICW[hdStage] |= (pContext->hwState.dwStateFlags & CELSIUS_FLAG_DOTPRODUCT3(0)) ?
                                                     DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _FALSE) :
                                                     DRF_DEF(056, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE);
        }
        SetAlphaInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    }

    ConstructOutputAlphaCombiners (pContext, enable, hdStage, bias, shift,
                                  pContext->tssState[dxStage].dwValue[D3DTSS_RESULTARG] == D3DTA_TEMP);
}

#endif  // NVARCH == 0x10

