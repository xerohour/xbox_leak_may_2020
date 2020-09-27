/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       combiner.cpp
 *  Content:    Handles the setting of the combiner registers for the
 *              D3D fixed function pipeline
 *
 ***************************************************************************/
 
#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#if DBG

#define KELVIN_COMBINER_REG_TEX0               0x00000008      // the combiner's first texture register (data from texture unit 0)

#define TEXTUREPM                       0xCAFEBEEF   // arbitrary identifier
#define TSS_USES_NULL_TEXTURE           0x1

// combiner inputs
typedef enum _kelvinCombinerInput {
    kelvinCombinerInputA = 0,
    kelvinCombinerInputB = 1,
    kelvinCombinerInputC = 2,
    kelvinCombinerInputD = 3,
} kelvinCombinerInput;

// the positions of the inputs in the input control words
static DWORD kelvinCombinerInputShift[4] = {24, 16, 8, 0};

DWORD g_ColorICW[8];
DWORD g_ColorOCW[8];
DWORD g_AlphaICW[8];
DWORD g_AlphaOCW[8];

//---------------------------------------------------------------------------

// Set an alpha input to to 1 or -1

void nvKelvinACSetAlphaInputOne (CDevice* pDevice, kelvinCombinerInput eInput,
                                 DWORD dwCombinerStage, BOOL bNegate)
{
    DWORD dwICW = 0;

    dwICW |= bNegate ? DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_MAP, _EXPAND_NORMAL) :
                       DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_MAP, _UNSIGNED_INVERT);

    dwICW |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_ALPHA, _FALSE);
    dwICW |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_0);

    g_AlphaICW[dwCombinerStage] |= (dwICW << kelvinCombinerInputShift[eInput]);
}

//---------------------------------------------------------------------------

// Select the source for combiner input eInput in combiner stage
// dwCombinerStage from argument dwD3DTSSArg in D3D stage dwD3DStage.
// return TSS_USES_NULL_TEXTURE if we reference a texture we don't have

HRESULT nvKelvinACSelectAlphaInput (CDevice* pDevice, kelvinCombinerInput eInput,
                                    DWORD dwCombinerStage, DWORD dwD3DStage, DWORD dwD3DTSSArg,
                                    BOOL bComplement, BOOL bExpandNormal)
{
    DWORD dwICW = 0;

    switch (D3D__TextureState[dwD3DStage][dwD3DTSSArg] & D3DTA_SELECTMASK) {
        case D3DTA_CURRENT:
            if (dwCombinerStage != 0) { // XBOX: Changed from dwD3DStage
                // Output from previous stage
                dwICW |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_C);
                break;
            }
            // else, default to diffuse below
        case D3DTA_DIFFUSE:
            dwICW |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, _REG_4);
            break;
        case D3DTA_TEXTURE:
            if (!(pDevice->m_Textures[dwD3DStage])) {
                // if we don't actually have a texture, default to the 'current' alpha.
                // yes, this is actually the d3d default: select texture with no texture.
                // my mama done told me, my name was duh microsoft.
                return (TSS_USES_NULL_TEXTURE);
            }
            else {
                dwICW |= DRF_NUM (097, _SET_COMBINER_ALPHA, _ICW_D_SOURCE, KELVIN_COMBINER_REG_TEX0 + dwD3DStage);
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
            pDevice->m_StateFlags |= STATE_COMBINERNEEDSSPECULAR;
            break;
        default:
            DXGRIP("unknown / illegal alpha argument '%d' in nvKelvinACSelectAlphaInput",
                 D3D__TextureState[dwD3DStage][dwD3DTSSArg] & D3DTA_SELECTMASK);
    }

    // handle the alpha replicate and complement flags

    dwICW |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_D_ALPHA, _TRUE);

    bComplement = ((D3D__TextureState[dwD3DStage][dwD3DTSSArg] & D3DTA_COMPLEMENT) ^
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

    g_AlphaICW[dwCombinerStage] |= (dwICW << kelvinCombinerInputShift[eInput]);
    return (0);
}

//---------------------------------------------------------------------------

// Select the input into A
static HRESULT nvKelvinACSelect1 (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Make A input
    hr = nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    // Make B 1
    nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    // C & D default to zero
    return (hr);
}

//---------------------------------------------------------------------------

// Select the input into D
static HRESULT nvKelvinACSelect2 (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply D * 1
    nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputC, dwHWStage, FALSE);
    hr = nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    // A & B default to zero
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACModulate (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * B
    hr  = nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    // C & D default to zero
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACAdd (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * 1
    hr = nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    // Make B 1
    nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    // Multiply D * 1
    nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputC, dwHWStage, FALSE);
    hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    // Implicit A + D
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACSub (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * 1
    hr = nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    // Multiply D * -1
    nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputC, dwHWStage, TRUE);
    hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    // Implicit A + -D
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACAddSmooth (CDevice* pDevice,DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * 1
    hr = nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    // Make C: (1-arg1), make D: arg2
    hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputC, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, TRUE, FALSE);
    hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACBlendAlpha (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage, DWORD dwType)
{
    HRESULT hr;
    DWORD dwTemp;

    // Multiply A * B(alpha)
    hr = nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);

    if (dwType == TEXTUREPM) { // this type defined by us
        // Make B 1 since texture is already pre-multiplied
        nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
        // set type again for making C (1-alpha) below
        dwType = D3DTA_TEXTURE;
    }
    else {
        // Make B alpha, appropriate type
        dwTemp = D3D__TextureState[dwD3DStage][D3DTSS_ALPHAARG2];
        D3D__TextureState[dwD3DStage][D3DTSS_ALPHAARG2] = dwType;
        hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
        // copy arg2 back
        D3D__TextureState[dwD3DStage][D3DTSS_ALPHAARG2] = dwTemp;
    }

    // Make C: 1-alpha, make D: arg2
    dwTemp = D3D__TextureState[dwD3DStage][D3DTSS_ALPHAARG1];
    D3D__TextureState[dwD3DStage][D3DTSS_ALPHAARG1] = dwType;
    hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputC, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, TRUE, FALSE);
    // copy back to alphaarg1
    D3D__TextureState[dwD3DStage][D3DTSS_ALPHAARG1] = dwTemp;
    hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

// The way D3D does this, it essentially allows using texture from stage 1
// in stage1 twice.  D3D does not normally allow you to use a texture in
// stage 0 and stage1.  This way, D3D allows you to modulate in (effectively)
// stage 0 and then do whatever it is you are going to do in stage1.  The
// key is that we use the texture from stage1 (since there is really no texture
// in stage 0)
static HRESULT nvKelvinACPremodulate (CDevice* pDevice,DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    DWORD   dwTemp;

    if (dwHWStage == 0) { // XBOX: Changed from dwD3DStage
        if(1) { // Xbox removed: D3D__TextureState[dwD3DStage+1][D3DTSS_ALPHAOP] != D3DTOP_DISABLE){
            // make A: alphaArg1(0), make B: texture(1)
             hr = nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
             dwTemp = D3D__TextureState[dwD3DStage][D3DTSS_ALPHAARG2];
             D3D__TextureState[dwD3DStage][D3DTSS_ALPHAARG2] = D3DTA_TEXTURE;
             hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG2, FALSE, FALSE);
             // copy back arg2
             D3D__TextureState[dwD3DStage][D3DTSS_ALPHAARG2] = dwTemp;
        }
        else{
            //asked to do premodulate on a single texture stage -- just pass down
            //the colorarg1
            hr = nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
            nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
        }

    }
    else { // well, right now we only handle two stages (we could compress these)
        // if pre-modulate is called in stage1, we could modulate with
        // texture from next stage. But, we'd have to handle settexture a bit
        // differently. For now, we'll just use selectarg1
        hr = nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
        nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    }

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACMultiplyAdd (CDevice* pDevice, int hdStage, int dxStage)
{
    HRESULT hr;

    // set A to arg1
    hr  = nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG0, FALSE, FALSE);
    //set B to 1
    nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputB, hdStage, FALSE);
    // set C to arg2
    hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputC, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    //set D to arg3
    hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputD, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinACLerp (CDevice* pDevice, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * B
    hr  = nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputA, hdStage, dxStage, D3DTSS_ALPHAARG0, FALSE, FALSE);
    hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputB, hdStage, dxStage, D3DTSS_ALPHAARG1, FALSE, FALSE);
    //set C to 1 - arg1
    hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputC, hdStage, dxStage, D3DTSS_ALPHAARG0, TRUE, FALSE);
    //set D to arg3
    hr |= nvKelvinACSelectAlphaInput (pDevice, kelvinCombinerInputD, hdStage, dxStage, D3DTSS_ALPHAARG2, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

void nvConstructKelvinOutputAlphaCombiners (CDevice* pDevice, BOOL bEnable,
                                            DWORD dwHWStage, DWORD dwD3DStage, BOOL bBias, DWORD dwShift, BOOL bOutTemp)
{
    if ((!bEnable) ||
        (D3D__TextureState[dwD3DStage][D3DTSS_COLOROP] == D3DTOP_DOTPRODUCT3))
    {
        g_AlphaICW[dwHWStage] = 0; // Xbox added
        g_AlphaOCW[dwHWStage] |= (DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_SUM_DST, _REG_0)  |
                                                        DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_AB_DST, _REG_0)   |
                                                        DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_CD_DST, _REG_0));
        return;
    }

    // select bias operation
    switch (dwShift) {
        case 0:
            g_AlphaOCW[dwHWStage] |= bBias ?
                                                           DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_OPERATION, _NOSHIFT_BIAS) :
                                                           DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_OPERATION, _NOSHIFT);
            break;
        case 1:
            g_AlphaOCW[dwHWStage] |= bBias ?
                                                           DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_OPERATION, _SHIFTLEFTBY1_BIAS) :
                                                           DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_OPERATION, _SHIFTLEFTBY1);
            break;
        case 2:
            g_AlphaOCW[dwHWStage] |= bBias ?
                                                           // we can't shift by 2 and bias. do the best we can...
                                                           DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_OPERATION, _SHIFTLEFTBY1_BIAS) :
                                                           DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_OPERATION, _SHIFTLEFTBY2);
            break;
        default:
            DXGRIP("illegal shift in nvConstructKelvinOutputAlphaCombiners");
            break;
    }  // switch

    g_AlphaOCW[dwHWStage] |= (DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_MUX_ENABLE, _FALSE) |
                                                    DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_SUM_DST, _REG_C)    |
                                                    DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_AB_DST, _REG_0)     |
                                                    DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_CD_DST, _REG_0));

    g_AlphaOCW[dwHWStage] |= bOutTemp ?
                                               DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_SUM_DST, _REG_D) :
                                               DRF_DEF (097, _SET_COMBINER_ALPHA, _OCW_SUM_DST, _REG_C);
}

//---------------------------------------------------------------------------

void ConstructAlphaCombiners (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage, BOOL bStageActive)
{
    DWORD   dwShift;
    BOOL    bBias, bEnable;
    HRESULT hr = 0;

    dwShift = 0;
    bBias   = FALSE;
    bEnable = TRUE;

    g_AlphaICW[dwHWStage] = 0;
    g_AlphaOCW[dwHWStage] = 0;

    if (!bStageActive) {
        // colorop was set to disable
        if (dwHWStage == 0) { // only construct default for stage 0
            // Default is diffuse alpha, make A=diffuse alpha, B=1, C=D=0
            g_AlphaICW[dwHWStage] |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_SOURCE, _REG_4);
            g_AlphaICW[dwHWStage] |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE);
            nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
        }
        else {
            bEnable = FALSE;
        }
    }

    else {

        switch (D3D__TextureState[dwD3DStage][D3DTSS_ALPHAOP]) {
            default:
            case D3DTOP_DOTPRODUCT3:
                // We treat dot3 like disable because it doesn't really matter.
                // if the colorop is also dot3, then alpha will be overwritten momentarily anyhow.
                // BUGBUG if the colorop is NOT dot3, what the heck does this even mean???
            case D3DTOP_DISABLE:
                if (dwHWStage == 0) { // only construct default for stage 0
                    // Default is diffuse alpha, make A=diffuse alpha, B=1, C=D=0
                    g_AlphaICW[dwHWStage] |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_SOURCE, _REG_4);
                    g_AlphaICW[dwHWStage] |= DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE);
                    nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
                }
                else {
                    bEnable = FALSE;
                }
                break;
            case D3DTOP_SELECTARG1:
                hr = nvKelvinACSelect1 (pDevice, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_SELECTARG2:
                hr = nvKelvinACSelect2 (pDevice, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_MODULATE4X:
                dwShift++;
            case D3DTOP_MODULATE2X:
                dwShift++;
            case D3DTOP_MODULATE:
                hr = nvKelvinACModulate (pDevice, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_ADDSIGNED2X: // same as addsigned with following change
                dwShift = 1;
            case D3DTOP_ADDSIGNED:   // same as add with following change
                bBias = TRUE;
            case D3DTOP_ADD:
                hr = nvKelvinACAdd (pDevice, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_SUBTRACT:
                hr = nvKelvinACSub (pDevice, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_ADDSMOOTH:
                hr = nvKelvinACAddSmooth (pDevice, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_BLENDDIFFUSEALPHA:
                hr = nvKelvinACBlendAlpha (pDevice, dwHWStage, dwD3DStage, D3DTA_DIFFUSE);
                break;
            case D3DTOP_BLENDTEXTUREALPHA:
                hr = nvKelvinACBlendAlpha (pDevice, dwHWStage, dwD3DStage, D3DTA_TEXTURE);
                break;
            case D3DTOP_BLENDFACTORALPHA:
                hr = nvKelvinACBlendAlpha (pDevice, dwHWStage, dwD3DStage, D3DTA_TFACTOR);
                break;
            case D3DTOP_BLENDTEXTUREALPHAPM:
                hr = nvKelvinACBlendAlpha (pDevice,dwHWStage, dwD3DStage, TEXTUREPM);
                break;
            case D3DTOP_BLENDCURRENTALPHA:
                hr = nvKelvinACBlendAlpha (pDevice, dwHWStage, dwD3DStage, D3DTA_CURRENT);
                break;
            case D3DTOP_PREMODULATE:
                hr = nvKelvinACPremodulate (pDevice, dwHWStage, dwD3DStage);
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
                hr = nvKelvinACMultiplyAdd (pDevice, dwHWStage, dwD3DStage);
                break;
            case D3DTOP_LERP:
                hr = nvKelvinACLerp (pDevice, dwHWStage, dwD3DStage);
                break;
        }  // switch

    }

    if (hr == TSS_USES_NULL_TEXTURE) {
        // this stage references a non-existent texture. just set it back to "select current"
        if (dwHWStage == 0) {
            // current = diffuse in stage 0
            g_AlphaICW[dwHWStage] = DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_SOURCE, _REG_4) |
                                                          DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE);
        }
        else {
            g_AlphaICW[dwHWStage] = DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_SOURCE, _REG_C) |
                                                          DRF_DEF (097, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE);
        }
        nvKelvinACSetAlphaInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    }

    nvConstructKelvinOutputAlphaCombiners (pDevice, bEnable, dwHWStage, dwD3DStage, bBias, dwShift,
                                           D3D__TextureState[dwD3DStage][D3DTSS_RESULTARG] == D3DTA_TEMP);

}

////////////////////////////////////////////////////////////////////////////////

// Set a color input to to 1 or -1

void nvKelvinCCSetColorInputOne (CDevice* pDevice, kelvinCombinerInput eInput,
                                 DWORD dwCombinerStage, BOOL bNegate)
{
    DWORD dwICW = 0;

    dwICW |= bNegate ? DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_MAP, _EXPAND_NORMAL) :
                       DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_MAP, _UNSIGNED_INVERT);

    dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_ALPHA, _FALSE);
    dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_0);

    g_ColorICW[dwCombinerStage] |= (dwICW << kelvinCombinerInputShift[eInput]);
}

//---------------------------------------------------------------------------

// Select the source for combiner input eInput in combiner stage
// dwCombinerStage from argument dwD3DTSSArg in D3D stage dwD3DStage.
// return TSS_USES_NULL_TEXTURE if we reference a texture we don't have

HRESULT nvKelvinCCSelectColorInput (CDevice* pDevice, kelvinCombinerInput eInput,
                                    DWORD dwCombinerStage, DWORD dwD3DStage, DWORD dwD3DTSSArg,
                                    BOOL bComplement, BOOL bExpandNormal, BOOL bAlphaReplicate)
{
    DWORD dwICW = 0;

    switch (D3D__TextureState[dwD3DStage][dwD3DTSSArg] & D3DTA_SELECTMASK) {
        case D3DTA_CURRENT:
            if (dwCombinerStage != 0) { // XBOX: Changed from dwD3DStage
                // Output from previous stage
                dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_C);
                break;
            }
            // else, default to diffuse below
        case D3DTA_DIFFUSE:
            dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_4);
            break;
        case D3DTA_TEXTURE:
            if (!(pDevice->m_Textures[dwD3DStage])) {
                // if we don't actually have a texture, default to the 'current'.
                // yes, this is actually the d3d default: select texture with no texture.
                // my mama done told me, my name was duh microsoft.
                return (TSS_USES_NULL_TEXTURE);
            }
            else {
                dwICW |= DRF_NUM (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, KELVIN_COMBINER_REG_TEX0 + dwD3DStage);
            }
            break;
        case D3DTA_TFACTOR:
            dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_1);
            break;
        case D3DTA_SPECULAR:
            dwICW |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_5);
            pDevice->m_StateFlags |= STATE_COMBINERNEEDSSPECULAR;
            break;
        case D3DTA_TEMP:
            dwICW |= DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_D);
            break;
        default:
            DXGRIP("unknown / illegal color argument '%d' in nvKelvinCCSelectColorInput",
                 D3D__TextureState[dwD3DStage][dwD3DTSSArg] & D3DTA_SELECTMASK);
    }

    // Now handle the alpha replicate and complement flags

    bAlphaReplicate |= (D3D__TextureState[dwD3DStage][dwD3DTSSArg] & D3DTA_ALPHAREPLICATE);

    dwICW |= bAlphaReplicate ? DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_ALPHA, _TRUE) :
                               DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_D_ALPHA, _FALSE);

    bComplement = ((D3D__TextureState[dwD3DStage][dwD3DTSSArg] & D3DTA_COMPLEMENT) ^
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

    g_ColorICW[dwCombinerStage] |= (dwICW << kelvinCombinerInputShift[eInput]);
    return (D3D_OK);
}

//---------------------------------------------------------------------------

// Select the input into A
static HRESULT nvKelvinCCSelect1 (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // A * 1
    hr = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    // C & D default to zero
    return (hr);
}

//---------------------------------------------------------------------------

// Select the input into D
static HRESULT nvKelvinCCSelect2 (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply D * 1
    nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputC, dwHWStage, FALSE);
    hr = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    // A & B default to zero
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCModulate (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * B
    hr  = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    // C & D default to zero
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCAdd (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * 1(B)
    hr = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    // Make B 1
    nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    // Multiply 1(C) * D
    nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputC, dwHWStage, FALSE);
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    // Implicit A + D
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCSub (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * 1
    hr = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    // Multiply D * -1
    nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputC, dwHWStage, TRUE);
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    // Implicit A + -D
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCAddSmooth (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply A * 1
    hr = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    // Make C: (1-arg1), make D: arg2
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputC, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, TRUE, FALSE, FALSE);
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCBlendAlpha (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage, DWORD dwType)
{
    HRESULT hr;
    DWORD   dwTemp;

    // Multiply A * B(alpha)
    hr = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);

    if (dwType == TEXTUREPM) // this type defined by us
    {
        // Make B 1 since texture is already pre-multiplied
        nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
        // set type again for making C (1-alpha) below
        dwType = D3DTA_TEXTURE;
    }
    else
    {
        // Make B alpha, appropriate type
        dwTemp = D3D__TextureState[dwD3DStage][D3DTSS_COLORARG2];
        D3D__TextureState[dwD3DStage][D3DTSS_COLORARG2] = dwType;
        hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, TRUE);
        // copy arg2 back
        D3D__TextureState[dwD3DStage][D3DTSS_COLORARG2] = dwTemp;
    }

    // Make C: 1-alpha, make D: arg2
    dwTemp = D3D__TextureState[dwD3DStage][D3DTSS_COLORARG1];
    D3D__TextureState[dwD3DStage][D3DTSS_COLORARG1] = dwType;
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputC, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, TRUE, FALSE, TRUE);
    // copy back to colorarg1
    D3D__TextureState[dwD3DStage][D3DTSS_COLORARG1] = dwTemp;
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

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

static HRESULT nvKelvinCCPremodulate (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    DWORD   dwTemp;

    if (dwHWStage == 0) { // XBOX: Changed from dwD3DStage
        if (1) { // Xbox removed: D3D__TextureState[1][D3DTSS_COLOROP] != D3DTOP_DISABLE) {
            // This modulates selectarg1(stage0) with texture(stage1) now!
            // The right solution is really to wait and see if there is a stage 1
            // make A: colorArg1(0), make B: texture(nextstage)
            hr = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
            // This is a bit of a hack here since we use colorarg from stage 0
            // but we get the texture from stage 1 (told to in SelectB)
            dwTemp = D3D__TextureState[dwD3DStage][D3DTSS_COLORARG2];
            D3D__TextureState[dwD3DStage][D3DTSS_COLORARG2] = D3DTA_TEXTURE;
            hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
            // reset back arg2
            D3D__TextureState[dwD3DStage][D3DTSS_COLORARG2] = dwTemp;
        }
        else {
            // asked to do premodulate on a single texture stage -- just pass down
            // the colorarg1
            hr = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
            nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
        }
    }
    else // well, right now we only handle two stages (we could compress these)
    {
        // if pre-modulate is called in stage1, we could modulate with
        // texture from next stage. But, we'd have to handle settexture a bit
        // differently. For now, we'll just use selectarg1
        hr = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
        nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);

        //DCR check for premodulate on stage 0?
    }

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCModulateAlpha (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage, BOOL bComplement)
{
    HRESULT hr;
    // A * 1
    hr = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    // Set C alpha(arg1), Set D: arg2
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputC, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, bComplement, FALSE, TRUE);
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputD, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCModulateColor (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage, BOOL bComplement)
{
    HRESULT hr;
    // A * B
    hr  = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, bComplement, FALSE, FALSE);
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    // Set C alpha(arg1), Set D: 1
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputC, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, TRUE);
    nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputD, dwHWStage, FALSE);
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinBumpMap (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    DWORD   dwArg1Cache;

    // this is terribly inefficient. we actually burn up a combiner stage doing nothing at all.
    // to make this better, we need a combiner analog to "nextAvailableTextureUnit" and to allow
    // for the possibility that this doesn't get incremented when we parse a D3D combiner stage.
    if (pDevice->m_Textures[dwD3DStage] == NULL) {
        DXGRIP("Setup failure");
        hr = TSS_USES_NULL_TEXTURE;
    }
    else {
        // just pass the result of the last stage on to the next
        dwArg1Cache = D3D__TextureState[dwD3DStage][D3DTSS_COLORARG1];
        D3D__TextureState[dwD3DStage][D3DTSS_COLORARG1] = D3DTA_CURRENT;
        hr = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
        D3D__TextureState[dwD3DStage][D3DTSS_COLORARG1] = dwArg1Cache;
        nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    }

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCDotProduct (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    HRESULT hr;
    // Multiply expanded A * expanded B
    hr  = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, dwHWStage, dwD3DStage, D3DTSS_COLORARG1, FALSE, TRUE, FALSE);
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputB, dwHWStage, dwD3DStage, D3DTSS_COLORARG2, FALSE, TRUE, FALSE);
    // C & D default to zero
    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCMultiplyAdd (CDevice* pDevice, int hdStage, int dxStage)
{
    HRESULT hr;

    // set A to arg1
    hr  = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG0, FALSE, FALSE, FALSE);
    // set B to 1
    nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputB, hdStage, FALSE);
    // set C to arg2
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputC, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    // set D to arg3
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputD, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

static HRESULT nvKelvinCCLerp (CDevice* pDevice, int hdStage, int dxStage)
{
    HRESULT hr;

    // Multiply A * B
    hr  = nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputA, hdStage, dxStage, D3DTSS_COLORARG0, FALSE, FALSE, FALSE);
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputB, hdStage, dxStage, D3DTSS_COLORARG1, FALSE, FALSE, FALSE);
    // set C to 1 - arg1
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputC, hdStage, dxStage, D3DTSS_COLORARG0, TRUE, FALSE, FALSE);
    // set D to arg3
    hr |= nvKelvinCCSelectColorInput (pDevice, kelvinCombinerInputD, hdStage, dxStage, D3DTSS_COLORARG2, FALSE, FALSE, FALSE);

    return (hr);
}

//---------------------------------------------------------------------------

// Set up output combiners

void nvConstructKelvinOutputColorCombiners (CDevice* pDevice, BOOL bEnable, DWORD dwStage,
                                            BOOL bBias, DWORD dwShift, BOOL bDotAB, BOOL bDotCD, BOOL bOutTemp)
{
    if (!bEnable) {
        ASSERT (dwStage > 0);  // stage 0 should always be enabled
        g_ColorOCW[dwStage] |= DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_0);
        return;
    }

    // select bias operation
    switch (dwShift) {
        case 0:
            g_ColorOCW[dwStage] |= bBias ?
                                                         DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_OP, _NOSHIFT_BIAS) :
                                                         DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_OP, _NOSHIFT);
            break;
        case 1:
            g_ColorOCW[dwStage] |= bBias ?
                                                         DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_OP, _SHIFTLEFTBY1_BIAS) :
                                                         DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_OP, _SHIFTLEFTBY1);
            break;
        case 2:
            g_ColorOCW[dwStage] |= bBias ?
                                                         // we can't shift by 2 and bias. do the best we can...
                                                         DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_OP, _SHIFTLEFTBY1_BIAS) :
                                                         DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_OP, _SHIFTLEFTBY2);
            break;
        default:
            DXGRIP("illagal shift in nvConstructKelvinOutputColorCombiners");
    }  // switch

    // select mux enable
    g_ColorOCW[dwStage] |= DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_MUX_ENABLE, _FALSE);

    // select dot enables
    if (bDotAB && bDotCD) {
        g_ColorOCW[dwStage] |= (DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_AB_DOT_ENABLE, _TRUE)           |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_CD_DOT_ENABLE, _TRUE)           |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_AB, _AB_DST_ENABLE) |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_CD, _CD_DST_ENABLE));
        g_ColorOCW[dwStage] |= bOutTemp ?
                                                 DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_D) :
                                                 DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_C);
    }
    else if (bDotAB) {
        g_ColorOCW[dwStage] |= (DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_AB_DOT_ENABLE, _TRUE)           |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_CD_DOT_ENABLE, _FALSE)          |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_AB, _AB_DST_ENABLE) |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_CD, _DISABLE));
        g_ColorOCW[dwStage] |= bOutTemp ?
                                                 DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_AB_DST, _REG_D) :
                                                 DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_AB_DST, _REG_C);
    }
    else {
        // select the sum as final result
        g_ColorOCW[dwStage] |= (DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_AB_DOT_ENABLE, _FALSE)    |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_CD_DOT_ENABLE, _FALSE)    |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_AB, _DISABLE) |
                                                  DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_CD, _DISABLE));
        g_ColorOCW[dwStage] |= bOutTemp ?
                                                 DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_D) :
                                                 DRF_DEF (097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_C);
    }

}

//---------------------------------------------------------------------------

BOOL ConstructColorCombiners (CDevice* pDevice, DWORD dwHWStage, DWORD dwD3DStage)
{
    DWORD   dwShift;
    BOOL    bDotProduct, bBias, bEnable, bStageActive;
    HRESULT hr = 0;

    bEnable      = TRUE;
    bStageActive = TRUE;
    bDotProduct  = FALSE;
    bBias        = FALSE;
    dwShift      = 0;

    g_ColorICW[dwHWStage] = 0;
    g_ColorOCW[dwHWStage] = 0;

    switch (D3D__TextureState[dwD3DStage][D3DTSS_COLOROP]) {
        default:
        case D3DTOP_DISABLE:
            if (dwHWStage == 0) { // only construct for stage 0 // XBOX: Changed from dwD3DStage
                // Default is diffuse color, make A=diffuse, B=1, C=D=0
                g_ColorICW[dwHWStage] |= DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_A_SOURCE, _REG_4);
                nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
            }
            else {
                bEnable = FALSE; // don't enable output combiner
            }
            bStageActive = FALSE;
            break;
        case D3DTOP_SELECTARG1:
            hr = nvKelvinCCSelect1 (pDevice, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_SELECTARG2: // same as 2 from above
            hr = nvKelvinCCSelect2 (pDevice, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_MODULATE4X:
            dwShift++;
        case D3DTOP_MODULATE2X:
            dwShift++;
        case D3DTOP_MODULATE:
            hr = nvKelvinCCModulate (pDevice, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_ADDSIGNED2X: // same as addsigned with following change
            dwShift = 1;
        case D3DTOP_ADDSIGNED:   // same as add with following change
            bBias = TRUE;
        case D3DTOP_ADD:
            hr = nvKelvinCCAdd (pDevice, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_SUBTRACT:
            hr = nvKelvinCCSub (pDevice, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_ADDSMOOTH:
            hr = nvKelvinCCAddSmooth (pDevice, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_BLENDDIFFUSEALPHA:
            hr = nvKelvinCCBlendAlpha (pDevice, dwHWStage, dwD3DStage, D3DTA_DIFFUSE);
            break;
        case D3DTOP_BLENDTEXTUREALPHA:
            hr = nvKelvinCCBlendAlpha (pDevice, dwHWStage , dwD3DStage, D3DTA_TEXTURE);
            break;
        case D3DTOP_BLENDFACTORALPHA:
            hr = nvKelvinCCBlendAlpha (pDevice, dwHWStage, dwD3DStage, D3DTA_TFACTOR);
            break;
        case D3DTOP_BLENDTEXTUREALPHAPM:
            hr = nvKelvinCCBlendAlpha (pDevice, dwHWStage, dwD3DStage, TEXTUREPM);
            break;
        case D3DTOP_BLENDCURRENTALPHA:
            hr = nvKelvinCCBlendAlpha (pDevice, dwHWStage, dwD3DStage, D3DTA_CURRENT);
            break;
        case D3DTOP_PREMODULATE:
            hr = nvKelvinCCPremodulate (pDevice, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_MODULATEALPHA_ADDCOLOR:
            hr = nvKelvinCCModulateAlpha (pDevice, dwHWStage, dwD3DStage, FALSE);
            break;
        case D3DTOP_MODULATECOLOR_ADDALPHA:
            hr = nvKelvinCCModulateColor (pDevice, dwHWStage, dwD3DStage, FALSE);
            break;
        case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
            hr = nvKelvinCCModulateAlpha (pDevice, dwHWStage, dwD3DStage, TRUE); // complement
            break;
        case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
            hr = nvKelvinCCModulateColor (pDevice, dwHWStage, dwD3DStage, TRUE); // complement
            break;
        case D3DTOP_BUMPENVMAP:
        case D3DTOP_BUMPENVMAPLUMINANCE:
            hr = nvKelvinBumpMap (pDevice, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_DOTPRODUCT3:
            hr = nvKelvinCCDotProduct (pDevice, dwHWStage, dwD3DStage);
            bDotProduct = TRUE;
            break;
        case D3DTOP_MULTIPLYADD:
            hr = nvKelvinCCMultiplyAdd (pDevice, dwHWStage, dwD3DStage);
            break;
        case D3DTOP_LERP:
            hr = nvKelvinCCLerp (pDevice, dwHWStage, dwD3DStage);
            break;
    }

    if (hr == TSS_USES_NULL_TEXTURE) {
        // this stage references a non-existent texture. just set it back to "select current"
        if (dwHWStage == 0) {
            // current = diffuse in stage 0
            g_ColorICW[dwHWStage] = DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_A_SOURCE, _REG_4);
        }
        else {
            g_ColorICW[dwHWStage] = DRF_DEF (097, _SET_COMBINER_COLOR, _ICW_A_SOURCE, _REG_C);
        }
        nvKelvinCCSetColorInputOne (pDevice, kelvinCombinerInputB, dwHWStage, FALSE);
    }

    nvConstructKelvinOutputColorCombiners (pDevice, bEnable, dwHWStage, bBias, dwShift, bDotProduct, FALSE,
                                           D3D__TextureState[dwD3DStage][D3DTSS_RESULTARG] == D3DTA_TEMP);

    return (bStageActive);
}


//------------------------------------------------------------------------------
// LazySetCombiners - corresponds to nvSetKelvinColorCombiners
//
// PERF: Lotsa material here for improvement

BOOL OldLazySetCombiners(
    CDevice* pDevice,
    DWORD* pBuffer)
{
    COUNT_PERF(PERF_SETSTATE_COMBINERS);

    // Only update combiners if there is no active pixel shader:

    if (pDevice->m_pPixelShader == NULL)
    {
        BOOL isStageActive;
        DWORD activeCombinerStagesCount;
        DWORD d3dStage;

        activeCombinerStagesCount = 0;

        do {
            d3dStage = activeCombinerStagesCount;
            if (D3D__RenderState[D3DRS_POINTSPRITEENABLE])
            {
                d3dStage = 3;

                if (pDevice->m_Textures[3] == NULL)
                {
                #if DBG
                    static WasWarned = FALSE;
                    if (!WasWarned)
                    {
                        WARNING("Point sprites are enabled but no texture is "
                                 "selected in stage 3.\n"
                                 "Xbox requires point sprites to use stage 3, "
                                 "not stage 0.");
                        WasWarned = TRUE;
                    }
                #endif
                }
            }

            isStageActive = ConstructColorCombiners(pDevice,
                                                    activeCombinerStagesCount,
                                                    d3dStage);

            ConstructAlphaCombiners(pDevice,
                                    activeCombinerStagesCount,
                                    d3dStage,
                                    isStageActive);

            if ((isStageActive) || (activeCombinerStagesCount == 0)) 
            {
                activeCombinerStagesCount++;
            }

        } while ((isStageActive) && (d3dStage < 3));

        // clear all the unused stages:

        for (DWORD i = activeCombinerStagesCount; i < NUM_COMBINERS; i++) 
        {
            g_ColorICW[i] = 0;
            g_ColorOCW[i] = 0;
            g_AlphaICW[i] = 0;
            g_AlphaOCW[i] = 0;
        }

        for (i = 0; i < 8; i++)
        {
            if (g_ColorICW[i] != pBuffer[1 + i])
            {
                _asm int 3;
                return FALSE;
            }
            if (g_ColorOCW[i] != pBuffer[10 + i])
            {
                _asm int 3;
                return FALSE;
            }
            if (g_AlphaICW[i] != pBuffer[19 + i])
            {
                _asm int 3;
                return FALSE;
            }
            if (g_AlphaOCW[i] != pBuffer[28 + i])
            {
                _asm int 3;
                return FALSE;
            }
        }
    }

    return TRUE;
}

#endif

























//------------------------------------------------------------------------------
// Combiner flags:

#define CFLAG_COMPLEMENT        0x010   // D3DTA_COMPLEMENT
#define CFLAG_ALPHAREPLICATE    0x020   // D3DTA_ALPHAREPLICATE
#define CFLAG_EXPANDNORMAL      0x040   // NV097_SET_COMBINER_ALPHA_ICW_D_MAP_EXPAND_NORMAL

#define CFLAG_INPUT_SHIFT       16
#define CFLAG_INPUT_MASK        0xf0000
#define CFLAG_INPUT_A           0x30000
#define CFLAG_INPUT_B           0x20000
#define CFLAG_INPUT_C           0x10000
#define CFLAG_INPUT_D           0x00000

//------------------------------------------------------------------------------
// Combiner state:

#define CSTATE_ALPHAOFFSET      0x048   // 18*4 = 72 bytes
#define CSTATE_STAGE_MASK       0x003   // D3D stage
#define CSTATE_FIRSTSTAGE       0x010
#define CSTATE_ALPHAREPLICATE   0x020   // D3DTA_ALPHAREPLICATE
#define CSTATE_NEXTSTAGE        0x080

//------------------------------------------------------------------------------
// Set a color input to to 1 or -1

static DWORD CombinerOne(
    DWORD Flags)
{
    DWORD icw;

    if (Flags & CFLAG_COMPLEMENT)
    {
        icw = DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_D_MAP, _EXPAND_NORMAL);
    }
    else
    {
        icw = DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_D_MAP, _UNSIGNED_INVERT);
    }

    // Shift left by (8 * input):

    return icw << ((Flags & CFLAG_INPUT_MASK) >> (CFLAG_INPUT_SHIFT - 3));
}

//------------------------------------------------------------------------------
// Select the source for combiner input 

static DWORD CombinerInput(
    DWORD Flags,
    DWORD State,
    DWORD Arg)
{
    DWORD icw;
    DWORD stage;

    switch (Arg & D3DTA_SELECTMASK)
    {
    case D3DTA_TEXTURE:
        stage = State & CSTATE_STAGE_MASK;
        if (g_Device.m_Textures[stage] == NULL)
        {
            // If there's no texture, default to the current.  We'll catch
            // this invalid value later:

            icw = 0xffffffff; 
        }
        else
        {
            icw = DRF_NUMFAST(097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, 8 + stage);
        }
        break;
    case D3DTA_TFACTOR:
        icw = DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_1);
        break;
    case D3DTA_SPECULAR:
        icw = DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_5);
        g_Device.m_StateFlags |= STATE_COMBINERNEEDSSPECULAR;
        break;
    case D3DTA_TEMP:
        icw = DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_D);
        break;
    case D3DTA_CURRENT:
        if (!(State & CSTATE_FIRSTSTAGE))
        {
            // Output from previous stage:

            icw = DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_C);
            break;
        }
        // Fall through...
    case D3DTA_DIFFUSE:
        icw = DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_D_SOURCE, _REG_4);
        break;
    default:
        NODEFAULT("Unexpected color argument\n");
    }

    // Now handle the alpha replicate and complement flags:

    ASSERT(CFLAG_ALPHAREPLICATE == D3DTA_ALPHAREPLICATE);
    ASSERT(CFLAG_ALPHAREPLICATE == CSTATE_ALPHAREPLICATE);
    ASSERT((CFLAG_ALPHAREPLICATE >> 1) 
            == DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_D_ALPHA, _TRUE));

    DWORD alphaReplication = (Flags | Arg | State) & CFLAG_ALPHAREPLICATE;
    icw |= (alphaReplication >> 1);

    ASSERT(CFLAG_COMPLEMENT == D3DTA_COMPLEMENT);
    ASSERT((CFLAG_COMPLEMENT << 1)
           == DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_D_MAP, _UNSIGNED_INVERT));
    ASSERT(CFLAG_EXPANDNORMAL 
           == DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_D_MAP, _EXPAND_NORMAL));

    BOOL complement = (Flags ^ Arg) & CFLAG_COMPLEMENT;
    icw |= ((complement << 1) | (Flags & CFLAG_EXPANDNORMAL));

    // Shift left by (8 * input):

    return icw << ((Flags & CFLAG_INPUT_MASK) >> (CFLAG_INPUT_SHIFT - 3));
}

//------------------------------------------------------------------------------
// Select the input into A

static DWORD CombinerSelect1(
    DWORD State,
    DWORD Arg1)
{
    // A * 1

    DWORD icw = CombinerInput(CFLAG_INPUT_A, State, Arg1);
    icw |= CombinerOne(CFLAG_INPUT_B);

    // C & D default to zero

    return icw;
}

//------------------------------------------------------------------------------
// Select the input into D

static DWORD CombinerSelect2(
    DWORD State,
    DWORD Arg2)
{
    // Multiply D * 1

    DWORD icw = CombinerOne(CFLAG_INPUT_C);
    icw |= CombinerInput(CFLAG_INPUT_D, State, Arg2);

    // A & B default to zero

    return icw;
}

//------------------------------------------------------------------------------

static DWORD CombinerModulate(
    DWORD State,
    DWORD Arg1,
    DWORD Arg2)
{
    // Multiply A * B

    DWORD icw = CombinerInput(CFLAG_INPUT_A, State, Arg1);
    icw |= CombinerInput(CFLAG_INPUT_B, State, Arg2);

    // C & D default to zero

    return icw;
}

//------------------------------------------------------------------------------

static DWORD CombinerAdd(
    DWORD State,
    DWORD Arg1,
    DWORD Arg2)
{
    // Multiply A * 1(B)

    DWORD icw = CombinerInput(CFLAG_INPUT_A, State, Arg1);

    // Make B 1

    icw |= CombinerOne(CFLAG_INPUT_B);

    // Multiply 1(C) * D

    icw |= CombinerOne(CFLAG_INPUT_C);
    icw |= CombinerInput(CFLAG_INPUT_D, State, Arg2);

    // Implicit A + D

    return icw;
}

//------------------------------------------------------------------------------

static DWORD CombinerSub(
    DWORD State,
    DWORD Arg1,
    DWORD Arg2)
{
    // Multiply A * 1

    DWORD icw = CombinerInput(CFLAG_INPUT_A, State, Arg1);
    icw |= CombinerOne(CFLAG_INPUT_B);

    // Multiply D * -1

    icw |= CombinerOne(CFLAG_INPUT_C | CFLAG_COMPLEMENT);
    icw |= CombinerInput(CFLAG_INPUT_D, State, Arg2);

    // Implicit A + -D

    return icw;
}

//------------------------------------------------------------------------------

static DWORD CombinerAddSmooth(
    DWORD State,
    DWORD Arg1,
    DWORD Arg2)
{
    // Multiply A * 1

    DWORD icw = CombinerInput(CFLAG_INPUT_A, State, Arg1);
    icw |= CombinerOne(CFLAG_INPUT_B);

    // Make C: (1-arg1), make D: arg2

    icw |= CombinerInput(CFLAG_INPUT_C | CFLAG_COMPLEMENT, State, Arg1);
    icw |= CombinerInput(CFLAG_INPUT_D, State, Arg2);

    return icw;
}

//------------------------------------------------------------------------------

static DWORD CombinerBlendAlpha(
    DWORD State,
    DWORD Arg1,
    DWORD Arg2,
    DWORD Type)
{
    DWORD icw = CombinerInput(CFLAG_INPUT_A, State, Arg1);
    icw |= CombinerInput(CFLAG_INPUT_B | CFLAG_ALPHAREPLICATE, State, Type);

    // Make C: 1-alpha, make D: arg2

    icw |= CombinerInput(
                CFLAG_INPUT_C | CFLAG_COMPLEMENT | CFLAG_ALPHAREPLICATE,
                State, Type);
    icw |= CombinerInput(CFLAG_INPUT_D, State, Arg2);

    return icw;
}

//------------------------------------------------------------------------------

static DWORD CombinerBlendAlphaPM(
    DWORD State,
    DWORD Arg1,
    DWORD Arg2)
{
    DWORD icw = CombinerInput(CFLAG_INPUT_A, State, Arg1);

    // Make B 1 since texture is already pre-multiplied

    icw |= CombinerOne(CFLAG_INPUT_B);

    // Make C: 1-alpha, make D: arg2

    icw |= CombinerInput(
                CFLAG_INPUT_C | CFLAG_COMPLEMENT | CFLAG_ALPHAREPLICATE,
                State, D3DTA_TEXTURE);
    icw |= CombinerInput(CFLAG_INPUT_D, State, Arg2);

    return icw;
}

//------------------------------------------------------------------------------
// The way D3D does this, it essentially allows using texture from stage 1
// in stage1 twice.  D3D does not normally allow you to use a texture in
// stage 0 and stage1.  This way, D3D allows you to modulate in (effectively)
// stage 0 and then do whatever it is you are going to do in stage1.  The
// key is that we use the texture from stage1 (since there is really no texture
// in stage 0)
//
// Premodulate in stage (i-1) is effectively selectarg1
// Then, in stage i, modulate current color (the above) with texture in stage i
// Then, do the operation chosen for stage i

static DWORD CombinerPremodulate(
    DWORD State,
    DWORD Arg1,
    DWORD Arg2)
{
    DWORD icw = 0;

    if (State & CSTATE_FIRSTSTAGE)
    {
        DWORD stage = State & CSTATE_STAGE_MASK;
        DWORD op = (State & CSTATE_ALPHAREPLICATE) ? D3DTSS_ALPHAOP : D3DTSS_COLOROP;

        ASSERT((stage < 3) && 
               (D3D__TextureState[stage + 1][op] != D3DTOP_DISABLE));

        // This modulates selectarg1(stage0) with texture(stage1) now!
        // The right solution is really to wait and see if there is a stage 1
        // make A: colorArg1(0), make B: texture(nextstage)

        icw |= CombinerInput(CFLAG_INPUT_A, State, Arg1);

        // This is a bit of a hack here since we use colorarg from stage 0
        // but we get the texture from stage 1 (told to in SelectB)

        icw |= CombinerInput(CFLAG_INPUT_B, State, D3DTA_TEXTURE);
    }
    else 
    {
        // Well, right now we only handle two stages (we could compress these)
        //
        // if pre-modulate is called in stage1, we could modulate with
        // texture from next stage. But, we'd have to handle settexture a bit
        // differently. For now, we'll just use selectarg1

        icw |= CombinerInput(CFLAG_INPUT_A, State, Arg1);

        icw |= CombinerOne(CFLAG_INPUT_B);

        // Check for premodulate on stage 0?
    }

    return icw;
}

//------------------------------------------------------------------------------

static DWORD CombinerModulateAlpha(
    DWORD State,
    DWORD Arg1,
    DWORD Arg2,
    DWORD Flags)    // CFLAG_COMPLEMENT or 0
{
    // A * 1

    DWORD icw = CombinerInput(CFLAG_INPUT_A, State, Arg1);
    icw |= CombinerOne(CFLAG_INPUT_B);

    // Set C alpha(arg1), Set D: arg2

    icw |= CombinerInput(CFLAG_INPUT_C | CFLAG_ALPHAREPLICATE | Flags, 
                          State, Arg1);
    icw |= CombinerInput(CFLAG_INPUT_D, State, Arg2);

    return icw;
}

//------------------------------------------------------------------------------

static DWORD CombinerModulateColor(
    DWORD State,
    DWORD Arg1,
    DWORD Arg2,
    DWORD Flags)    // CFLAG_COMPLEMENT or 0
{
    // A * B

    DWORD icw = CombinerInput(CFLAG_INPUT_A | Flags, State, Arg1);
    icw |= CombinerInput(CFLAG_INPUT_B, State, Arg2);

    // Set C alpha(arg1), Set D: 1

    icw |= CombinerInput(CFLAG_INPUT_C | CFLAG_ALPHAREPLICATE, State, Arg1);
    icw |= CombinerOne(CFLAG_INPUT_D);

    return icw;
}

//------------------------------------------------------------------------------

static DWORD NewKelvinBumpMap(
    DWORD State,
    DWORD Arg1)
{
    // This is terribly inefficient. we actually burn up a combiner stage doing 
    // nothing at all.  

    DWORD stage = (State & CSTATE_STAGE_MASK);

    if (DBG_CHECK(g_pDevice->m_Textures[stage] == NULL))
    {
        DXGRIP("D3DTOP_BUMPENVMAP requires a texture to be set in the "
               "corresponding stage\n");
    }

    // Just pass the result of the last stage on to the next:

    DWORD icw = CombinerInput(CFLAG_INPUT_A, State, D3DTA_CURRENT);
    icw |= CombinerOne(CFLAG_INPUT_B);

    return icw;
}

//------------------------------------------------------------------------------

static DWORD CombinerDotProduct(
    DWORD State,
    DWORD Arg1,
    DWORD Arg2)
{
    // Multiply expanded A * expanded B

    DWORD icw = CombinerInput(CFLAG_INPUT_A | CFLAG_EXPANDNORMAL, State, Arg1);
    icw |= CombinerInput(CFLAG_INPUT_B | CFLAG_EXPANDNORMAL, State, Arg2);

    // C & D default to zero

    return icw;
}

//------------------------------------------------------------------------------

static DWORD CombinerMultiplyAdd(
    DWORD State,
    DWORD Arg0,
    DWORD Arg1,
    DWORD Arg2)
{
    // Set A to arg1

    DWORD icw = CombinerInput(CFLAG_INPUT_A, State, Arg0);

    // Set B to 1

    icw |= CombinerOne(CFLAG_INPUT_B);

    // Set C to arg2

    icw |= CombinerInput(CFLAG_INPUT_C, State, Arg1);

    // Set D to arg3

    icw |= CombinerInput(CFLAG_INPUT_D, State, Arg2);

    return icw;
}

//------------------------------------------------------------------------------

static DWORD CombinerLerp(
    DWORD State,
    DWORD Arg0,
    DWORD Arg1,
    DWORD Arg2)
{

    // Multiply A * B

    DWORD icw = CombinerInput(CFLAG_INPUT_A, State, Arg0);
    icw |= CombinerInput(CFLAG_INPUT_B, State, Arg1);

    // Set C to 1 - arg1

    icw |= CombinerInput(CFLAG_INPUT_C | CFLAG_COMPLEMENT, State, Arg0);

    // Set D to arg3

    icw |= CombinerInput(CFLAG_INPUT_D, State, Arg2);

    return icw;
}

//------------------------------------------------------------------------------
// LazySetCombiners

VOID LazySetCombiners(
    CDevice* pDevice)
{
    DWORD controlWords[4*9];

    COUNT_PERF(PERF_SETSTATE_COMBINERS);

    // Only update combiners if there is no active pixel shader:

    if (pDevice->m_pPixelShader != NULL)
        return;

    // Reset the combiner-needs-specular flag, because we're going to
    // recompute it:

    DWORD oldStateFlags = pDevice->m_StateFlags;
    pDevice->m_StateFlags &= ~STATE_COMBINERNEEDSSPECULAR;

    DWORD startStage = 0;
    if (D3D__RenderState[D3DRS_POINTSPRITEENABLE])
    {
        startStage = 3;
    #if DBG
        if (pDevice->m_Textures[3] == NULL)
        {
            static WasWarned = FALSE;
            if (!WasWarned)
            {
                WARNING("Point sprites are enabled but no texture is "
                         "selected in stage 3.\n"
                         "Xbox requires point sprites to use stage 3, "
                         "not stage 0.");
                WasWarned = TRUE;
            }
        }
    #endif
    }

    DWORD* pBuffer = &controlWords[1];
    DWORD stage = startStage;
    DWORD state = stage | CSTATE_FIRSTSTAGE;

    DWORD* pState = &D3D__TextureState[stage][0];
    DWORD op = pState[D3DTSS_COLOROP];

    do {
        DWORD arg0 = pState[D3DTSS_COLORARG0];
        DWORD arg1 = pState[D3DTSS_COLORARG1];
        DWORD arg2 = pState[D3DTSS_COLORARG2];
        DWORD ocwResult = (pState[D3DTSS_RESULTARG] == D3DTA_TEMP)
                        ? DRF_DEF(097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_D)
                        : DRF_DEF(097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_C);

        while (TRUE)
        {
            if (DBG_CHECK(state & CSTATE_ALPHAREPLICATE))
            {
                if ((op == D3DTOP_MODULATEALPHA_ADDCOLOR) ||
                    (op == D3DTOP_MODULATECOLOR_ADDALPHA) ||
                    (op == D3DTOP_MODULATEINVALPHA_ADDCOLOR) ||
                    (op == D3DTOP_MODULATEINVCOLOR_ADDALPHA) ||
                    (op == D3DTOP_DOTPRODUCT3) ||
                    (op == D3DTOP_BUMPENVMAP) ||
                    (op == D3DTOP_BUMPENVMAPLUMINANCE))
                {
                    DXGRIP("Illegal ALPHAOP: %li\n", op);
                }
            }

            DWORD icw;
            DWORD ocw = ocwResult;

            switch (op)
            {
            case D3DTOP_DISABLE:
                if (state & CSTATE_FIRSTSTAGE)
                {
                    // Default is diffuse color, make A=diffuse, B=1, C=0, D=0:

                    ASSERT((CSTATE_ALPHAREPLICATE << 23)
                        == DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_A_ALPHA, _TRUE));

                    icw = DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_A_SOURCE, _REG_4)
                        | DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_B_MAP, _UNSIGNED_INVERT)
                        | ((state & CSTATE_ALPHAREPLICATE) << 23);

                    // If COLOROP is DISABLE, set the defaults and finish:

                    if (!(state & CSTATE_ALPHAREPLICATE))
                    {
                        *(pBuffer + 0) = icw;
                        *(pBuffer + 9) = ocw;
                        *(pBuffer + 18) = icw 
                            | DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_A_ALPHA, _TRUE);
                        *(pBuffer + 27) = ocw;
                        pBuffer++;
                        stage++;

                        goto AllDone;
                    }
                }
                else
                {
                    // The COLOROP is enabled, but this is the ALPHAOP and it's
                    // disabled.  Don't enable the combiner:

                    ocw = 0;
                    icw = 0;
                }
                break;
            case D3DTOP_SELECTARG1:
                icw = CombinerSelect1(state, arg1);
                break;
            case D3DTOP_SELECTARG2:
                icw = CombinerSelect2(state, arg2);
                break;
            case D3DTOP_MODULATE4X:
                ocw |= DRF_DEF(097, _SET_COMBINER_COLOR, _OCW_OP, _SHIFTLEFTBY2);
                icw = CombinerModulate(state, arg1, arg2);
                break;
            case D3DTOP_MODULATE2X:
                ocw |= DRF_DEF(097, _SET_COMBINER_COLOR, _OCW_OP, _SHIFTLEFTBY1);
                // Fall through...
            case D3DTOP_MODULATE:
                icw = CombinerModulate(state, arg1, arg2);
                break;
            case D3DTOP_ADDSIGNED2X:
                ocw |= DRF_DEF(097, _SET_COMBINER_COLOR, _OCW_OP, _SHIFTLEFTBY1_BIAS);
                // Fall through...
            case D3DTOP_ADDSIGNED:
                ocw |= DRF_DEF(097, _SET_COMBINER_COLOR, _OCW_OP, _NOSHIFT_BIAS);
                // Fall through...
            case D3DTOP_ADD:
                icw = CombinerAdd(state, arg1, arg2);
                break;
            case D3DTOP_SUBTRACT:
                icw = CombinerSub(state, arg1, arg2);
                break;
            case D3DTOP_ADDSMOOTH:
                icw = CombinerAddSmooth(state, arg1, arg2);
                break;
            case D3DTOP_BLENDDIFFUSEALPHA:
            case D3DTOP_BLENDCURRENTALPHA:
            case D3DTOP_BLENDTEXTUREALPHA:
            case D3DTOP_BLENDFACTORALPHA:
                ASSERT(D3DTOP_BLENDCURRENTALPHA - D3DTOP_BLENDDIFFUSEALPHA
                        == D3DTA_CURRENT - D3DTA_DIFFUSE);
                ASSERT(D3DTOP_BLENDTEXTUREALPHA - D3DTOP_BLENDDIFFUSEALPHA
                        == D3DTA_TEXTURE - D3DTA_DIFFUSE);
                ASSERT(D3DTOP_BLENDFACTORALPHA - D3DTOP_BLENDDIFFUSEALPHA
                        == D3DTA_TFACTOR - D3DTA_DIFFUSE);

                icw = CombinerBlendAlpha(state, arg1, arg2, 
                            op - D3DTOP_BLENDDIFFUSEALPHA + D3DTA_DIFFUSE);
                break;
            case D3DTOP_BLENDTEXTUREALPHAPM:
                icw = CombinerBlendAlphaPM(state, arg1, arg2);
                break;
            case D3DTOP_PREMODULATE:
                icw = CombinerPremodulate(state, arg1, arg2);
                break;
            case D3DTOP_MODULATEALPHA_ADDCOLOR:
                icw = CombinerModulateAlpha(state, arg1, arg2, 0);
                break;
            case D3DTOP_MODULATECOLOR_ADDALPHA:
                icw = CombinerModulateColor(state, arg1, arg2, 0);
                break;
            case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
                icw = CombinerModulateAlpha(state, arg1, arg2, CFLAG_COMPLEMENT);
                break;
            case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
                icw = CombinerModulateColor(state, arg1, arg2, CFLAG_COMPLEMENT);
                break;
            case D3DTOP_BUMPENVMAP:
            case D3DTOP_BUMPENVMAPLUMINANCE:
                icw = NewKelvinBumpMap(state, arg1);
                break;
            case D3DTOP_DOTPRODUCT3:
                icw = CombinerDotProduct(state, arg1, arg2);
                ASSERT(((DRF_DEF(097, _SET_COMBINER_COLOR, _OCW_SUM_DST, _REG_D)) >> 4)
                      == DRF_DEF(097, _SET_COMBINER_COLOR, _OCW_AB_DST, _REG_D));
                ocw >>= 4;
                ocw |= (DRF_DEF(097, _SET_COMBINER_COLOR, _OCW_AB_DOT_ENABLE, _TRUE)           |
                        DRF_DEF(097, _SET_COMBINER_COLOR, _OCW_CD_DOT_ENABLE, _FALSE)          |
                        DRF_DEF(097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_AB, _AB_DST_ENABLE) |
                        DRF_DEF(097, _SET_COMBINER_COLOR, _OCW_BLUETOALPHA_CD, _DISABLE));

                // Disable ICW and OCW for alpha:

                *(pBuffer + 18) = 0;    
                *(pBuffer + 27) = 0;
                state |= CSTATE_NEXTSTAGE;
                break;
            case D3DTOP_MULTIPLYADD:
                icw = CombinerMultiplyAdd(state, arg0, arg1, arg2);
                break;
            case D3DTOP_LERP:
                icw = CombinerLerp(state, arg0, arg1, arg2);
                break;
            default:
                NODEFAULT("Unexpected OP\n");
            }

            if ((icw & 0xff000000) == 0xff000000)
            {
                // This stage references a non-existent texture.  Just set it 
                // back to "select current":

                if (state & CSTATE_FIRSTSTAGE)
                {
                    // Current = diffuse in stage 0

                    icw = DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_A_SOURCE, _REG_4)
                        | DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_B_MAP, _UNSIGNED_INVERT);
                }
                else
                {
                    icw = DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_A_SOURCE, _REG_C)
                        | DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_B_MAP, _UNSIGNED_INVERT);
                }

                ASSERT((CSTATE_ALPHAREPLICATE << 23)
                    == DRF_DEF(097, _SET_COMBINER_COLOR, _ICW_A_ALPHA, _TRUE));

                icw |= ((state & CSTATE_ALPHAREPLICATE) << 23);
            }

            DWORD alphaOffset = (state & CSTATE_ALPHAOFFSET);

            *(DWORD*)((BYTE*) pBuffer + alphaOffset) = icw;
            *(DWORD*)((BYTE*) pBuffer + alphaOffset + 9*4) = ocw;

            if (state & CSTATE_NEXTSTAGE)
                goto NextStage;         //=====>

            // For alpha, always enable alpha replication:

            state |= (CSTATE_ALPHAREPLICATE | CSTATE_ALPHAOFFSET | CSTATE_NEXTSTAGE);

            arg0 = pState[D3DTSS_ALPHAARG0];
            arg1 = pState[D3DTSS_ALPHAARG1];
            arg2 = pState[D3DTSS_ALPHAARG2];
            op = pState[D3DTSS_ALPHAOP];
        }

NextStage:

        pBuffer++;
        stage++;
        if (stage == 4)
            goto AllDone;

        state = stage;
        pState = &D3D__TextureState[stage][0];
        op = pState[D3DTSS_COLOROP];

    } while (op != D3DTOP_DISABLE);

AllDone:

    // Clear any unused stages:

    DWORD usedStages = stage - startStage;
    DWORD unusedStages = 8 - usedStages;

    ASSERT((usedStages >= 1) && (usedStages <= 4));

    do {
        *(pBuffer) = 0;
        *(pBuffer + 9) = 0;
        *(pBuffer + 18) = 0;
        *(pBuffer + 27) = 0;
        pBuffer++;

    } while (--unusedStages != 0);

    controlWords[0] = PUSHER_METHOD(SUBCH_3D, NV097_SET_COMBINER_COLOR_ICW(0), 8);
    controlWords[9] = PUSHER_METHOD(SUBCH_3D, NV097_SET_COMBINER_COLOR_OCW(0), 8);
    controlWords[18] = PUSHER_METHOD(SUBCH_3D, NV097_SET_COMBINER_ALPHA_ICW(0), 8);
    controlWords[27] = PUSHER_METHOD(SUBCH_3D, NV097_SET_COMBINER_ALPHA_OCW(0), 8);

#if DBG
    if (!OldLazySetCombiners(pDevice, &controlWords[0]))
        LazySetCombiners(pDevice);
#endif

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, 
          NV097_SET_COMBINER_CONTROL, 
          (DRF_NUMFAST(097, _SET_COMBINER_CONTROL, _ITERATION_COUNT, usedStages) |
           DRF_DEF(097, _SET_COMBINER_CONTROL, _MUX_SELECT, _LSB) |
           DRF_DEF(097, _SET_COMBINER_CONTROL, _FACTOR0, _SAME_FACTOR_ALL) |
           DRF_DEF(097, _SET_COMBINER_CONTROL, _FACTOR1, _SAME_FACTOR_ALL)));

    memcpy((VOID*) (pPush + 2), &controlWords[0], sizeof(controlWords));

    pPush += 2 + sizeof(controlWords) / 4;

    PushedRaw(pPush);

    // Hit the specular-enable register if the specular-needed state changes,
    // and specular isn't already enabled via the render state:

    if ((pDevice->m_StateFlags ^ oldStateFlags) & STATE_COMBINERNEEDSSPECULAR)
    {
        if (!D3D__RenderState[D3DRS_SPECULARENABLE])
        {
            Push1(pPush, 
                  NV097_SET_SPECULAR_ENABLE, 
                  (pDevice->m_StateFlags & STATE_COMBINERNEEDSSPECULAR) != 0);

            pPush += 2;
        }
    }

    pDevice->EndPush(pPush);
}

} // end namespace
