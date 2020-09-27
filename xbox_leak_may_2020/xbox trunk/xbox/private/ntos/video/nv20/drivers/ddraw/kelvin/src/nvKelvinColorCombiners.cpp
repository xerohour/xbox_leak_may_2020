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
//  Module: nvKelvinColorCombiners.cpp
//      routines for setting up the Kelvin color combiners from
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

// Set a color input to to 1 or -1

void nvKelvinCCSetColorInputOne (PNVD3DCONTEXT pContext, kelvinCombinerInput eInput,
                                 DWORD dwCombinerStage, BOOL bNegate)
{
    DWORD dwICW = 0;

    dwICW |= bNegate ? DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_MAP, _EXPAND_NORMAL) :
                       DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_MAP, _UNSIGNED_INVERT);

    dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_ALPHA, _FALSE);
    dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_0);

    pContext->hwState.dwColorICW[dwCombinerStage] |= (dwICW << kelvinCombinerInputShift[eInput]);
}

//---------------------------------------------------------------------------

// Select the source for combiner input eInput in combiner stage
// dwCombinerStage from argument dwD3DTSSArg in D3D stage dwD3DStage.
// return TSS_USES_NULL_TEXTURE if we reference a texture we don't have

HRESULT nvKelvinCCSelectColorInput (PNVD3DCONTEXT pContext, kelvinCombinerInput eInput,
                                    DWORD dwCombinerStage, DWORD dwD3DStage, DWORD dwD3DTSSArg,
                                    BOOL bComplement, BOOL bExpandNormal, BOOL bAlphaReplicate)
{
    DWORD dwICW = 0;
    DWORD dwCurrentUsed = FALSE; //slight hack to fix D3DTA_CURRENT issues.  This should be cleaner.
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
            if (dwCurrentUsed == TRUE) 
            {
                // Output from previous stage
                dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_C);
                break;
            }
            // else, default to diffuse below
        case D3DTA_DIFFUSE:
            dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_4);
            break;
        case D3DTA_TEXTURE:
            if (!(pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXTUREMAP])) {
                // if we don't actually have a texture, default to the 'current'.
                // yes, this is actually the d3d default: select texture with no texture.
                // my mama done told me, my name was duh microsoft.
                return (TSS_USES_NULL_TEXTURE);
            }
            else {
                if (pContext->hwState.dwNextAvailableTextureUnit < KELVIN_NUM_TEXTURES) {
                    pContext->hwState.dwTexUnitToTexStageMapping[pContext->hwState.dwNextAvailableTextureUnit] = dwD3DStage;
                    dwICW |= DRF_NUM (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, KELVIN_COMBINER_REG_TEX0 + pContext->hwState.dwNextAvailableTextureUnit);
                }
                else {
                    pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
                    dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_0);
                }
            }
            break;
        case D3DTA_TFACTOR:
            dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_1);
            break;
        case D3DTA_SPECULAR:
            dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_5);
            pContext->hwState.dwStateFlags |= KELVIN_FLAG_COMBINERSNEEDSPECULAR;
            break;
        case D3DTA_TEMP:
            dwICW |= DRF_DEF(056, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_D);
            break;
        default:
            DPF ("unknown / illegal color argument '%d' in nvKelvinCCSelectColorInput",
                 pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_SELECTMASK);
            dbgD3DError();
            pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
            break;
    }

    // Now handle the alpha replicate and complement flags

    bAlphaReplicate |= (pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_ALPHAREPLICATE);

    dwICW |= bAlphaReplicate ? DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_ALPHA, _TRUE) :
                               DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_ALPHA, _FALSE);

    bComplement = ((pContext->tssState[dwD3DStage].dwValue[dwD3DTSSArg] & D3DTA_COMPLEMENT) ^
                   ((bComplement) ? D3DTA_COMPLEMENT : 0))
                  ?
                  TRUE : FALSE;

    if (bExpandNormal) {
        dwICW |= bComplement ? DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_MAP, _EXPAND_NEGATE) :
                               DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_MAP, _EXPAND_NORMAL);
    }
    else {
        dwICW |= bComplement ? DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_MAP, _UNSIGNED_INVERT) :
                               DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_MAP, _UNSIGNED_IDENTITY);
    }

    pContext->hwState.dwColorICW[dwCombinerStage] |= (dwICW << kelvinCombinerInputShift[eInput]);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// Select the input into A
static HRESULT nvKelvinCCSelect1 (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // A * 1
    hr = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
    // C & D default to zero
    return (hr);
}

//---------------------------------------------------------------------------

// Select the input into D
static HRESULT nvKelvinCCSelect2 (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply D * 1
    nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputC, dwHWStage, FALSE);
    hr = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    // A & B default to zero
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCModulate (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * B
    hr  = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    // C & D default to zero
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCAdd (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * 1(B)
    hr = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    // Make B 1
    nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
    // Multiply 1(C) * D
    nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputC, dwHWStage, FALSE);
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    // Implicit A + D
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCSub (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * 1
    hr = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
    // Multiply D * -1
    nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputC, dwHWStage, TRUE);
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    // Implicit A + -D
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCAddSmooth (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * 1
    hr = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
    // Make C: (1-arg1), make D: arg2
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputC, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, TRUE, FALSE, FALSE);
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCBlendAlpha (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage, DWORD dwType)
{
    HRESULT hr;
    DWORD   dwTemp;

    // Multiply A * B(alpha)
    hr = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);

    if (dwType == TEXTUREPM) // this type defined by us
    {
        // Make B 1 since texture is already pre-multiplied
        nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
        // set type again for making C (1-alpha) below
        dwType = D3DTA_TEXTURE;
    }
    else
    {
        // Make B alpha, appropriate type
        dwTemp = pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLORARG2];
        pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLORARG2] = dwType;
        hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, TRUE);
        // copy arg2 back
        pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLORARG2] = dwTemp;
    }

    // Make C: 1-alpha, make D: arg2
    dwTemp = pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLORARG1];
    pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLORARG1] = dwType;
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputC, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, TRUE, FALSE, TRUE);
    // copy back to colorarg1
    pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLORARG1] = dwTemp;
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

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

static HRESULT nvKelvinCCPremodulate (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    DWORD   dwTemp;

    DPF ("pre-modulationis totally broken");
    dbgD3DError();

    if (dwD3DStage == 0) {
        if (pContext->tssState[1].dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE) {
            // This modulates selectarg1(stage0) with texture(stage1) now!
            // The right solution is really to wait and see if there is a stage 1
            // make A: colorArg1(0), make B: texture(nextstage)
            hr = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
            // This is a bit of a hack here since we use colorarg from stage 0
            // but we get the texture from stage 1 (told to in SelectB)
            dwTemp = pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLORARG2];
            pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLORARG2] = D3DTA_TEXTURE;
            hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
            // reset back arg2
            pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLORARG2] = dwTemp;
        }
        else {
            // asked to do premodulate on a single texture stage -- just pass down
            // the colorarg1
            hr = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
            nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
        }
    }
    else // well, right now we only handle two stages (we could compress these)
    {
        // if pre-modulate is called in stage1, we could modulate with
        // texture from next stage. But, we'd have to handle settexture a bit
        // differently. For now, we'll just use selectarg1
        hr = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
        nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);

        //DCR check for premodulate on stage 0?
    }

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCModulateAlpha (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage, BOOL bComplement)
{
    HRESULT hr;
    // A * 1
    hr = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
    // Set C alpha(arg1), Set D: arg2
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputC, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, bComplement, FALSE, TRUE);
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCModulateColor (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage, BOOL bComplement)
{
    HRESULT hr;
    // A * B
    hr  = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, bComplement, FALSE, FALSE);
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    // Set C alpha(arg1), Set D: 1
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputC, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, TRUE);
    nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputD, dwHWStage, FALSE);
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinBumpMap (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    DWORD   dwArg1Cache;

    // this is terribly inefficient. we actually burn up a combiner stage doing nothing at all.
    // to make this better, we need a combiner analog to "nextAvailableTextureUnit" and to allow
    // for the possibility that this doesn't get incremented when we parse a D3D combiner stage.
    if (pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXTUREMAP] == NULL) {
        pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
        hr = TSS_USES_NULL_TEXTURE;
    }
    else if (pContext->hwState.dwNextAvailableTextureUnit >= KELVIN_NUM_TEXTURES) {
        pContext->hwState.dwStateFlags |= KELVIN_FLAG_SETUPFAILURE;
        // not really OK, but there's nothing we can do about it.
        hr = D3D_OK;
    }
    else {
        // just pass the result of the last stage on to the next
        dwArg1Cache = pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLORARG1];
        pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLORARG1] = D3DTA_CURRENT;
        hr = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
        pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLORARG1] = dwArg1Cache;
        nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
        pContext->hwState.dwTexUnitToTexStageMapping[pContext->hwState.dwNextAvailableTextureUnit] = dwD3DStage;
    }

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCDotProduct (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply expanded A * expanded B
    hr  = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, TRUE, FALSE);
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, TRUE, FALSE);
    // C & D default to zero
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCMultiplyAdd (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // set A to arg1
    hr  = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG0, FALSE, FALSE, FALSE);
    // set B to 1
    nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputB, hdStage, FALSE);
    // set C to arg2
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputC, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    // set D to arg3
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputD, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCLerp (PNVD3DCONTEXT pContext, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * B
    hr  = nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG0, FALSE, FALSE, FALSE);
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputB, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    // set C to 1 - arg1
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputC, hdStage, dxStage, D3DTSS_COLORARG0, TRUE, FALSE, FALSE);
    // set D to arg3
    hr |= nvKelvinCCSelectColorInput (pContext, kelvinCombinerInputD, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

// Set up output combiners

void nvConstructKelvinOutputColorCombiners (PNVD3DCONTEXT pContext, BOOL bEnable, DWORD dwStage,
                                            BOOL bBias, DWORD dwShift, BOOL bDotAB, BOOL bDotCD, BOOL bOutTemp)
{
    bOutTemp &= (dwStage != 8); // last stage must go to normal output

    if (!bEnable) {
        nvAssert (dwStage > 0);  // stage 0 should always be enabled
        pContext->hwState.dwColorOCW[dwStage] |= DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_0);
        return;
    }

    // select bias operation
    switch (dwShift) {
        case 0:
            pContext->hwState.dwColorOCW[dwStage] |= bBias ?
                                                     DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_OP, _NOSHIFT_BIAS) :
                                                     DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_OP, _NOSHIFT);
            break;
        case 1:
            pContext->hwState.dwColorOCW[dwStage] |= bBias ?
                                                     DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_OP, _SHIFTLEFTBY1_BIAS) :
                                                     DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_OP, _SHIFTLEFTBY1);
            break;
        case 2:
            pContext->hwState.dwColorOCW[dwStage] |= bBias ?
                                                     // we can't shift by 2 and bias. do the best we can...
                                                     DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_OP, _SHIFTLEFTBY1_BIAS) :
                                                     DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_OP, _SHIFTLEFTBY2);
            break;
        default:
            DPF ("illagal shift in nvConstructKelvinOutputColorCombiners");
            dbgD3DError();
            break;
    }  // switch

    // select mux enable
    pContext->hwState.dwColorOCW[dwStage] |= DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_MUX_ENABLE, _FALSE);

    // select dot enables
    if (bDotAB && bDotCD) {
        pContext->hwState.dwColorOCW[dwStage] |= (DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_AB_DOT_ENABLE, _TRUE)           |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_CD_DOT_ENABLE, _TRUE)           |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_AB, _AB_DST_ENABLE) |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_CD, _CD_DST_ENABLE));
        pContext->hwState.dwColorOCW[dwStage] |= bOutTemp ?
                                                 DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_D) :
                                                 DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_C);
    }
    else if (bDotAB) {
        pContext->hwState.dwColorOCW[dwStage] |= (DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_AB_DOT_ENABLE, _TRUE)           |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_CD_DOT_ENABLE, _FALSE)          |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_AB, _AB_DST_ENABLE) |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_CD, _DISABLE));
        pContext->hwState.dwColorOCW[dwStage] |= bOutTemp ?
                                                 DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_AB_DST, _REG_D) :
                                                 DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_AB_DST, _REG_C);
    }
    else {
        // select the sum as final result
        pContext->hwState.dwColorOCW[dwStage] |= (DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_AB_DOT_ENABLE, _FALSE)    |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_CD_DOT_ENABLE, _FALSE)    |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_AB, _DISABLE) |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_CD, _DISABLE));
        pContext->hwState.dwColorOCW[dwStage] |= bOutTemp ?
                                                 DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_D) :
                                                 DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_C);
    }

}

//---------------------------------------------------------------------------

BOOL nvConstructKelvinColorCombiners (PNVD3DCONTEXT pContext, DWORD dwHWStage, DWORD dwD3DStage)
{
    DWORD   dwShift;
    BOOL    bDotProduct, bBias, bEnable, bStageActive;
    HRESULT hr = 0;

    bEnable      = TRUE;
    bStageActive = TRUE;
    bDotProduct  = FALSE;
    bBias        = FALSE;
    dwShift      = 0;

    pContext->hwState.dwColorICW[dwHWStage] = 0;
    pContext->hwState.dwColorOCW[dwHWStage] = 0;

    switch (pContext->tssState[dwD3DStage].dwValue[D3DTSS_COLOROP]) {
        default:
        case D3DTOP_DISABLE:
            if (dwD3DStage == 0) { // only construct for stage 0
                // Default is diffuse color, make A=diffuse, B=1, C=D=0
                pContext->hwState.dwColorICW[dwHWStage] |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_A_SOURCE, _REG_4);
                nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
            }
            else {
                bEnable = FALSE; // don't enable output combiner
            }
            bStageActive = FALSE;
            break;
        case D3DTOP_SELECTARG1:
            hr = nvKelvinCCSelect1 (pContext, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_SELECTARG2: // same as 2 from above
            hr = nvKelvinCCSelect2 (pContext, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_MODULATE4X:
            dwShift++;
        case D3DTOP_MODULATE2X:
            dwShift++;
        case D3DTOP_MODULATE:
            hr = nvKelvinCCModulate (pContext, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_ADDSIGNED2X: // same as addsigned with following change
            dwShift = 1;
        case D3DTOP_ADDSIGNED:   // same as add with following change
            bBias = TRUE;
        case D3DTOP_ADD:
            hr = nvKelvinCCAdd (pContext, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_SUBTRACT:
            hr = nvKelvinCCSub (pContext, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_ADDSMOOTH:
            hr = nvKelvinCCAddSmooth (pContext, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_BLENDDIFFUSEALPHA:
            hr = nvKelvinCCBlendAlpha (pContext, dwHWStage, dwD3DStage, D3DTA_DIFFUSE);
            break;
        case D3DTOP_BLENDTEXTUREALPHA:
            hr = nvKelvinCCBlendAlpha (pContext, dwHWStage , dwD3DStage, D3DTA_TEXTURE);
            break;
        case D3DTOP_BLENDFACTORALPHA:
            hr = nvKelvinCCBlendAlpha (pContext, dwHWStage, dwD3DStage, D3DTA_TFACTOR);
            break;
        case D3DTOP_BLENDTEXTUREALPHAPM:
            hr = nvKelvinCCBlendAlpha (pContext, dwHWStage, dwD3DStage, TEXTUREPM);
            break;
        case D3DTOP_BLENDCURRENTALPHA:
            hr = nvKelvinCCBlendAlpha (pContext, dwHWStage, dwD3DStage, D3DTA_CURRENT);
            break;
        case D3DTOP_PREMODULATE:
            hr = nvKelvinCCPremodulate (pContext, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_MODULATEALPHA_ADDCOLOR:
            hr = nvKelvinCCModulateAlpha (pContext, dwHWStage, dwD3DStage, FALSE);
            break;
        case D3DTOP_MODULATECOLOR_ADDALPHA:
            hr = nvKelvinCCModulateColor (pContext, dwHWStage, dwD3DStage, FALSE);
            break;
        case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
            hr = nvKelvinCCModulateAlpha (pContext, dwHWStage, dwD3DStage, TRUE); // complement
            break;
        case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
            hr = nvKelvinCCModulateColor (pContext, dwHWStage, dwD3DStage, TRUE); // complement
            break;
        case D3DTOP_BUMPENVMAP:
        case D3DTOP_BUMPENVMAPLUMINANCE:
            hr = nvKelvinBumpMap (pContext, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_DOTPRODUCT3:
            hr = nvKelvinCCDotProduct (pContext, dwHWStage, dwD3DStage);
            bDotProduct = TRUE;
            break;
        case D3DTOP_MULTIPLYADD:
            hr = nvKelvinCCMultiplyAdd (pContext, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_LERP:
            hr = nvKelvinCCLerp (pContext, dwHWStage, dwD3DStage);
            break;
    }

    if (hr == TSS_USES_NULL_TEXTURE) {
        // this stage references a non-existent texture. just set it back to "select current"
        if (dwHWStage == 0) {
            // current = diffuse in stage 0
            pContext->hwState.dwColorICW[dwHWStage] = DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_A_SOURCE, _REG_4);
        }
        else {
            pContext->hwState.dwColorICW[dwHWStage] = DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_A_SOURCE, _REG_C);
        }
        nvKelvinCCSetColorInputOne (pContext, kelvinCombinerInputB, dwHWStage, FALSE);
    }

    nvConstructKelvinOutputColorCombiners (pContext, bEnable, dwHWStage, bBias, dwShift, bDotProduct, FALSE,
                                           pContext->tssState[dwD3DStage].dwValue[D3DTSS_RESULTARG] == D3DTA_TEMP);

    return (bStageActive);
}

#endif  // NVARCH == 0x20


