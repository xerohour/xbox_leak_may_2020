/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvCelsiusColorCombiner.cpp                                        *
*       Routines for programming the celsius color combiner                 *
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

// Set a color input to to 1 or -1

void SetColorInputOne (PNVD3DCONTEXT pContext, celsiusCombinerInput eInput,
                       DWORD dwCombinerStage, BOOL bNegate)
{
    DWORD dwICW = 0;

    dwICW |= bNegate ? DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_MAP, _EXPAND_NORMAL) :
                       DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_MAP, _UNSIGNED_INVERT);

    dwICW |= DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_ALPHA, _FALSE);
    dwICW |= DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_0);

    pContext->hwState.dwColorICW[dwCombinerStage] &= ~(DRF_NUM(056, _SET_COMBINER_COLOR, _ICW_D_MAP, ~0) << celsiusCombinerInputShift[eInput]);
    pContext->hwState.dwColorICW[dwCombinerStage] &= ~(DRF_NUM(056, _SET_COMBINER_COLOR, _ICW_D_ALPHA, ~0) << celsiusCombinerInputShift[eInput]);
    pContext->hwState.dwColorICW[dwCombinerStage] &= ~(DRF_NUM(056, _SET_COMBINER_COLOR, _ICW_D_SOURCE, ~0) << celsiusCombinerInputShift[eInput]);

    pContext->hwState.dwColorICW[dwCombinerStage] |= (dwICW << celsiusCombinerInputShift[eInput]);
}

//---------------------------------------------------------------------------

// Set the input mapping mode for the given stage and input

void SetColorMapping (PNVD3DCONTEXT pContext, celsiusCombinerInput eInput,
                       DWORD dwCombinerStage, DWORD dwMapping)
{
    pContext->hwState.dwColorICW[dwCombinerStage] &= ~(DRF_NUM(056, _SET_COMBINER_COLOR, _ICW_D_MAP, 0xffffffff)  << celsiusCombinerInputShift[eInput]);
    pContext->hwState.dwColorICW[dwCombinerStage] |= (DRF_NUM(056, _SET_COMBINER_COLOR, _ICW_D_MAP, dwMapping) << celsiusCombinerInputShift[eInput]);
}

// Select the source for combiner input eInput in combiner stage
// dwCombinerStage from argument dwD3DTSSArg in D3D stage dwD3DStage.
// return TSS_USES_NULL_TEXTURE if we reference a texture we don't have

HRESULT SelectColorInput (PNVD3DCONTEXT pContext, celsiusCombinerInput eInput,
                          DWORD dwCombinerStage, DWORD dwD3DStage, DWORD dwD3DTSSArg,
                          BOOL bComplement, BOOL bExpandNormal, BOOL bAlphaReplicate)
{
    DWORD dwICW = 0;

    switch (pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_SELECTMASK) {
        case D3DTA_CURRENT:
            if (dwD3DStage != 0 && (pContext->tssState[0].dwValue[D3DTSS_RESULTARG] != D3DTA_TEMP) ) {
                // Output from previous stage
                dwICW |= DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_C);
                break;
            }
            // else, default to diffuse below
        case D3DTA_DIFFUSE:
            dwICW |= DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_4);
            break;
        case D3DTA_TEXTURE:
            if (!(pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXTUREMAP])) {
                // if we don't actually have a texture, default to the 'current'.
                // yes, this is actually the d3d default: select texture with no texture.
                // my mama done told me, my name was duh microsoft.
                return (TSS_USES_NULL_TEXTURE);
            }
            else {
                pContext->hwState.dwTexUnitToTexStageMapping[pContext->hwState.dwNextAvailableTextureUnit] = dwD3DStage;
                dwICW |= DRF_NUM(056, _SET_COMBINER_COLOR, _ICW_D_SOURCE, CELSIUS_COMBINER_REG_TEX0 + pContext->hwState.dwNextAvailableTextureUnit);
            }
            break;
        case D3DTA_TFACTOR:
            dwICW |= DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_1);
            break;
        case D3DTA_SPECULAR:
            dwICW |= DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_5);
            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_COMBINERSNEEDSPECULAR;
            break;
        case D3DTA_TEMP:
            dwICW |= DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_D);
            break;
        default:
            DPF ("unknown / illegal color argument '%d' in SelectColorInput",
                 pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_SELECTMASK);
            dbgD3DError();
            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_SETUPFAILURE;
            break;
    }

    // Now handle the alpha replicate and complement flags

    bAlphaReplicate |= (pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_ALPHAREPLICATE);

    dwICW |= bAlphaReplicate ? DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_ALPHA, _TRUE) :
                               DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_ALPHA, _FALSE);

    bComplement = ((pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_COMPLEMENT) ^
                   ((bComplement) ? D3DTA_COMPLEMENT : 0))
                  ?
                  TRUE : FALSE;

    if (bExpandNormal) {
        dwICW |= bComplement ? DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_MAP, _EXPAND_NEGATE) :
                               DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_MAP, _EXPAND_NORMAL);
    }
    else {
        dwICW |= bComplement ? DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_MAP, _UNSIGNED_INVERT) :
                               DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_MAP, _UNSIGNED_IDENTITY);
    }

    pContext->hwState.dwColorICW[dwCombinerStage] |= (dwICW << celsiusCombinerInputShift[eInput]);
    return (0);
}

//---------------------------------------------------------------------------

// Select the input into A
static HRESULT combineselect1 (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // A * 1
    hr = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    SetColorInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    // C & D default to zero

    return (hr);
}

//---------------------------------------------------------------------------

// Select the input into D
static HRESULT combineselect2 (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply D * 1
    SetColorInputOne (pContext, celsiusCombinerInputC, hdStage, FALSE);
    hr = SelectColorInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    // A & B default to zero

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combinemodulate (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * B
    hr  = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    hr |= SelectColorInput (pContext, celsiusCombinerInputB, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    // C & D default to zero

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combineadd (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * 1(B)
    hr = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    // Make B 1
    SetColorInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    // Multiply 1(C) * D
    SetColorInputOne (pContext, celsiusCombinerInputC, hdStage, FALSE);
    hr |= SelectColorInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    // Implicit A + D

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combinesub (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * 1
    hr = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    SetColorInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    // Multiply D * -1
    SetColorInputOne (pContext, celsiusCombinerInputC, hdStage, TRUE);
    hr |= SelectColorInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    // Implicit A + -D

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combineaddsmooth (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * 1
    hr = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    SetColorInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    // Make C: (1-arg1), make D: arg2
    hr |= SelectColorInput (pContext, celsiusCombinerInputC, hdStage, dxStage, D3DTSS_COLORARG1, TRUE, FALSE, FALSE);
    hr |= SelectColorInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combineblendalpha (PNVD3DCONTEXT pContext, int hdStage, int dxStage, int type)
{
    HRESULT hr = 0;
    int     temp;
    BOOL    bAlphaReplicate;

    // if the last stage involved a dot3 operation, pull alpha_current from the last stage's color channels
    // rather than its alpha channel, because the copy into the latter will have been deferred and the value
    // will not yet be available there.
    bAlphaReplicate = ((type==D3DTA_CURRENT) &&
                       (hdStage > 0) &&
                       (pContext->hwState.dwStateFlags & CELSIUS_FLAG_DOTPRODUCT3(hdStage-1))) ? FALSE : TRUE;

    // A gets alpha, as appropriate
    if (type == TEXTUREPM) {
        // Make A=1 since texture is already pre-multiplied
        SetColorInputOne (pContext, celsiusCombinerInputA, hdStage, FALSE);
        // set type again for making C (1-alpha) below
        type = D3DTA_TEXTURE;
    }
    else {
        temp = pContext->tssState[dxStage].dwValue[D3DTSS_COLORARG2];
        pContext->tssState[dxStage].dwValue[D3DTSS_COLORARG2] = type;
        hr |= SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, bAlphaReplicate);
        pContext->tssState[dxStage].dwValue[D3DTSS_COLORARG2] = temp;
    }

    // B gets arg1
    hr |= SelectColorInput (pContext, celsiusCombinerInputB, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);

    // C gets 1-alpha
    temp = pContext->tssState[dxStage].dwValue[D3DTSS_COLORARG1];
    pContext->tssState[dxStage].dwValue[D3DTSS_COLORARG1] = type;
    hr |= SelectColorInput (pContext, celsiusCombinerInputC, hdStage, dxStage, D3DTSS_COLORARG1, TRUE, FALSE, bAlphaReplicate);
    pContext->tssState[dxStage].dwValue[D3DTSS_COLORARG1] = temp;

    // D gets arg2
    hr |= SelectColorInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

// The way D3D does this, it essentially allows using texture from stage 1
// in stage1 twice.  D3D does not normally allow you to use a texture in
// stage 0 and stage1.  This way, D3D allows you to modulate in (effectively)
// stage 0 and then do whatever it is you are going to do in stage1.  The
// key is that we use the texture from stage1 (since there is really no texture
// in stage 0)

// Premodulate in stage (i-1) is effectively selectarg1
// Then, in stage i, modulate current color (the above) with texture in stage i
// Then, do the operation chosen for stage i

static HRESULT combinepremodulate (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;
    int temp, premodulate = 1;

    DPF ("pre-modulationis totally broken");
    dbgD3DError();

    if (dxStage == 0) {
        if (pContext->tssState[1].dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE) {
            // This modulates selectarg1(stage0) with texture(stage1) now!
            // The right solution is really to wait and see if there is a stage 1
            // make A: colorArg1(0), make B: texture(nextstage)
            hr = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
            // This is a bit of a hack here since we use colorarg from stage 0
            // but we get the texture from stage 1 (told to in SelectB)
            temp = pContext->tssState[dxStage].dwValue[D3DTSS_COLORARG2];
            pContext->tssState[dxStage].dwValue[D3DTSS_COLORARG2] = D3DTA_TEXTURE;
            hr |= SelectColorInput (pContext, celsiusCombinerInputB, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
            // reset back arg2
            pContext->tssState[dxStage].dwValue[D3DTSS_COLORARG2] = temp;
        }
        else {
            // asked to do premodulate on a single texture stage -- just pass down
            // the colorarg1
            hr = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
            SetColorInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
        }
    }
    else // well, right now we only handle two stages (we could compress these)
    {
        // if pre-modulate is called in stage1, we could modulate with
        // texture from next stage. But, we'd have to handle settexture a bit
        // differently. For now, we'll just use selectarg1
        hr = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
        SetColorInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);

        //DCR check for premodulate on stage 0?
    }

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combinemodulatealpha (PNVD3DCONTEXT pContext, int hdStage, int dxStage, int complement)
{
    HRESULT hr;

    // A * 1
    hr = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    SetColorInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    // Set C alpha(arg1), Set D: arg2
    hr |= SelectColorInput (pContext, celsiusCombinerInputC, hdStage, dxStage, D3DTSS_COLORARG1, complement, FALSE, TRUE);
    hr |= SelectColorInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combinemodulatecolor (PNVD3DCONTEXT pContext, int hdStage, int dxStage, int complement)
{
    HRESULT hr;

    // A * B
    hr  = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG1, complement, FALSE, FALSE);
    hr |= SelectColorInput (pContext, celsiusCombinerInputB, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    // Set C alpha(arg1), Set D: 1
    hr |= SelectColorInput (pContext, celsiusCombinerInputC, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, TRUE);
    SetColorInputOne (pContext, celsiusCombinerInputD, hdStage, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combinedotproduct (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply expanded A * expanded B
    hr  = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, TRUE, FALSE);
    hr |= SelectColorInput (pContext, celsiusCombinerInputB, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, TRUE, FALSE);
    // C & D default to zero

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combinemultiplyadd (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // set A to arg1
    hr  = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG0, FALSE, FALSE, FALSE);
    //set B to 1
    SetColorInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    // set C to arg2
    hr |= SelectColorInput (pContext, celsiusCombinerInputC, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    //set D to arg3
    hr |= SelectColorInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT combinelerp (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;
    
    // Multiply A * B
    hr  = SelectColorInput (pContext, celsiusCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG0, FALSE, FALSE, FALSE);
    hr |= SelectColorInput (pContext, celsiusCombinerInputB, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    //set C to 1 - arg1
    hr |= SelectColorInput (pContext, celsiusCombinerInputC, hdStage, dxStage, D3DTSS_COLORARG0, TRUE, FALSE, FALSE);
    //set D to arg3
    hr |= SelectColorInput (pContext, celsiusCombinerInputD, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    
    return (hr);
}

//---------------------------------------------------------------------------

// Set up output combiners

void ConstructOutputColorCombiners(PNVD3DCONTEXT pContext, int enable, int stage, int bias, int shift, int dotAB, int dotCD, int outtemp)
{
    outtemp  &= (stage != 1);

    if (!enable) {
        assert(stage > 0);  // stage 0 should always be enabled
        pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_SUM_DST, _REG_0);
        return;
    }

    // select bias operation
    if (stage == 0) {

        if (shift == 0) {
            if (bias == 0)
                pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_OPERATION, _NOSHIFT);
            else
                pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_OPERATION, _NOSHIFT_BIAS);
        }
        else if (shift == 1) {
            if (bias == 0)
                pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_OPERATION, _SHIFTLEFTBY1);
            else
                pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_OPERATION, _SHIFTLEFTBY1_BIAS);
        }
        else if (shift == 2) {
            if (bias == 0)
                pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_OPERATION, _SHIFTLEFTBY2);
            else
                // we can't shift by 2 and bias. do the best we can...
                pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_OPERATION, _SHIFTLEFTBY1_BIAS);
        }

        // select mux enable
        pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_MUX_SELECT, _LSB);  // the '1' is not a typo
        pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_MUX_ENABLE, _FALSE);

        // select AB dot enable
        if (dotAB)
        {
            pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_AB_DOT_ENABLE, _TRUE);

            if(dotCD)//select CD dot enable, and the sum as final result
            {
                if (outtemp)
                    pContext->hwState.dwColorOCW[stage] |= (DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_CD_DOT_ENABLE, _TRUE)|
                                                            DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_SUM_DST, _REG_D));
                else
                    pContext->hwState.dwColorOCW[stage] |= (DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_CD_DOT_ENABLE, _TRUE)|
                                                            DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_SUM_DST, _REG_C));
            }
            else //if only AB dot enabled, put A*B as the final result
            {
                if (outtemp)
                    pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_AB_DST, _REG_D); 
                else
                    pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_AB_DST, _REG_C); 
            }
        }
        else //select the sum as final result
        {
            if (outtemp)
                pContext->hwState.dwColorOCW[stage] |=  DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_SUM_DST, _REG_D);
            else
                pContext->hwState.dwColorOCW[stage] |=  DRF_DEF(056, _SET_COMBINER0_COLOR, _OCW_SUM_DST, _REG_C);
        }
    }

    else { // stage 1

        if (shift == 0) {
            if (bias == 0)
                pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_OPERATION, _NOSHIFT);
            else
                pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_OPERATION, _NOSHIFT_BIAS);
        }
        else if (shift == 1) {
            if (bias == 0)
                pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_OPERATION, _SHIFTLEFTBY1);
            else
                pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_OPERATION, _SHIFTLEFTBY1_BIAS);
        }
        else if (shift == 2) {
            if (bias == 0)
                pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_OPERATION, _SHIFTLEFTBY2);
            else
                pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_OPERATION, _SHIFTLEFTBY1);
        }

        // select mux enable
        pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_MUX_SELECT, _LSB);
        pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_MUX_ENABLE, _FALSE);

        // select AB dot enable
        if (dotAB)
        {
            pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_AB_DOT_ENABLE, _TRUE);

            if(dotCD)// select CD dot enable, and sum as final result
            {
                if (outtemp)
                    pContext->hwState.dwColorOCW[stage] |= (DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_CD_DOT_ENABLE, _TRUE)|
                                                                DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_SUM_DST, _REG_D));
                else
                    pContext->hwState.dwColorOCW[stage] |= (DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_CD_DOT_ENABLE, _TRUE)|
                                                                DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_SUM_DST, _REG_C));
            }
            else //if only AB dot enabled, put A*B as the final result
            {
                if (outtemp)
                    pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_AB_DST, _REG_D);
                else
                    pContext->hwState.dwColorOCW[stage] |= DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_AB_DST, _REG_C); 
            }
        }
        else //select the sum as final result
        {
            if (outtemp)
                pContext->hwState.dwColorOCW[stage] |=  DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_SUM_DST, _REG_D);
            else
                pContext->hwState.dwColorOCW[stage] |=  DRF_DEF(056, _SET_COMBINER1_COLOR, _OCW_SUM_DST, _REG_C);
        }
    }
}

//---------------------------------------------------------------------------

BOOL ConstructColorCombiners (PNVD3DCONTEXT pContext, int hdStage, int dxStage )
{
    int     dotproduct, bias, shift;
    BOOL    enable, stageactive;
    HRESULT hr = 0;

    dotproduct = 0;
    bias = 0;
    shift = 0;
    enable = TRUE;
    stageactive = TRUE;

    pContext->hwState.dwColorICW[hdStage] = 0;
    pContext->hwState.dwColorOCW[hdStage] = 0;

    switch (pContext->tssState[dxStage].dwValue[D3DTSS_COLOROP]) {
        default:
        case D3DTOP_DISABLE:
            if (dxStage == 0) { // only construct for stage 0
                // Default is diffuse color, make A=diffuse, B=1, C=D=0
                pContext->hwState.dwColorICW[hdStage] |= DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_A_SOURCE, _REG_4);
                SetColorInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
            }
            else {
                enable = FALSE; // don't enable output combiner
            }
            stageactive = FALSE;
            break;
        case D3DTOP_SELECTARG1:
            hr = combineselect1 (pContext, hdStage, dxStage);
            break;
        case D3DTOP_SELECTARG2: // same as 2 from above
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
            hr = combineblendalpha (pContext, hdStage, dxStage, TEXTUREPM);
            break;
        case D3DTOP_BLENDCURRENTALPHA:
            hr = combineblendalpha (pContext, hdStage, dxStage, D3DTA_CURRENT);
            break;
        case D3DTOP_PREMODULATE:
            hr = combinepremodulate (pContext, hdStage, dxStage);
            break;
        case D3DTOP_MODULATEALPHA_ADDCOLOR:
            hr = combinemodulatealpha (pContext, hdStage, dxStage, 0);
            break;
        case D3DTOP_MODULATECOLOR_ADDALPHA:
            hr = combinemodulatecolor (pContext, hdStage, dxStage, 0);
            break;
        case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
            hr = combinemodulatealpha (pContext, hdStage, dxStage, 1); // complement
            break;
        case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
            hr = combinemodulatecolor (pContext, hdStage, dxStage, 1); // complement
            break;
        case D3DTOP_BUMPENVMAP:
            break;
        case D3DTOP_BUMPENVMAPLUMINANCE:
            break;
        case D3DTOP_DOTPRODUCT3:
            hr = combinedotproduct (pContext, hdStage, dxStage);
            dotproduct = 1;
            pContext->hwState.dwStateFlags |= CELSIUS_FLAG_DOTPRODUCT3(hdStage);
            break;
        case D3DTOP_MULTIPLYADD:
            hr = combinemultiplyadd (pContext, hdStage, dxStage);
            break;
        case D3DTOP_LERP:
            hr = combinelerp (pContext, hdStage, dxStage);
            break;
        }

    if (hr == TSS_USES_NULL_TEXTURE) {
        // this stage references a non-existent texture. just set it back to "select current"
        if (hdStage == 0) {
            // current = diffuse in stage 0
            pContext->hwState.dwColorICW[hdStage] = DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_A_SOURCE, _REG_4);
        }
        else {
            pContext->hwState.dwColorICW[hdStage] = DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_A_SOURCE, _REG_C);
        }
        SetColorInputOne (pContext, celsiusCombinerInputB, hdStage, FALSE);
    }

    ConstructOutputColorCombiners (pContext, enable, hdStage, bias, shift, dotproduct, 0, 
                                   pContext->tssState[dxStage].dwValue[D3DTSS_RESULTARG] == D3DTA_TEMP);

    return (stageactive);
}

#endif  // NVARCH == 0x10

