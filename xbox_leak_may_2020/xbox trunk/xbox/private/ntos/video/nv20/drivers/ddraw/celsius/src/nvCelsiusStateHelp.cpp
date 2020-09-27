/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvCelsiusStateHelp.cpp                                            *
*       Celsius state management helper routines.                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler        26May99         NV10 development            *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

#include "nvCelsiusCaps.h"

//---------------------------------------------------------------------------

// this function comes from erik.

static float lar[32] =
{
     0.000000f,  // error=0.687808
    -0.023353f,  // error=0.070106
    -0.095120f,  // error=0.010402
    -0.170208f,  // error=0.016597
    -0.251038f,  // error=0.021605
    -0.336208f,  // error=0.025186
    -0.421539f,  // error=0.027635
    -0.503634f,  // error=0.029262
    -0.579592f,  // error=0.030311
    -0.647660f,  // error=0.030994
    -0.708580f,  // error=0.031427
    -0.760208f,  // error=0.031702
    -0.803673f,  // error=0.031889
    -0.840165f,  // error=0.031995
    -0.871344f,  // error=0.032067
    -0.896105f,  // error=0.032105
    -0.916457f,  // error=0.032139
    -0.933262f,  // error=0.032165
    -0.946507f,  // error=0.032173
    -0.957755f,  // error=0.032285
    -0.966165f,  // error=0.032230
    -0.972848f,  // error=0.032189
    -0.978413f,  // error=0.032191
    -0.983217f,  // error=0.032718
    -0.986471f,  // error=0.032289
    -0.988778f,  // error=0.033091
    -0.991837f,  // error=0.035067
    -0.993452f,  // error=0.034156
    -0.994839f,  // error=0.034863
    -0.995434f,  // error=0.034785
    -0.996690f,  // error=0.033426
    -1.000000f
};

static float mar[32] =
{
    -0.494592f,  // error=0.687808
    -0.494592f,  // error=0.070106
    -0.570775f,  // error=0.010402
    -0.855843f,  // error=0.016597
    -1.152452f,  // error=0.021605
    -1.436778f,  // error=0.025186
    -1.705918f,  // error=0.027635
    -1.948316f,  // error=0.029262
    -2.167573f,  // error=0.030311
    -2.361987f,  // error=0.030994
    -2.512236f,  // error=0.031427
    -2.652873f,  // error=0.031702
    -2.781295f,  // error=0.031889
    -2.890906f,  // error=0.031995
    -2.938739f,  // error=0.032067
    -3.017491f,  // error=0.032105
    -3.077762f,  // error=0.032139
    -3.099087f,  // error=0.032165
    -3.144977f,  // error=0.032173
    -3.100986f,  // error=0.032285
    -3.151608f,  // error=0.032230
    -3.212636f,  // error=0.032189
    -3.219419f,  // error=0.032191
    -3.079402f,  // error=0.032718
    -3.174922f,  // error=0.032289
    -3.469706f,  // error=0.033091
    -2.895668f,  // error=0.035067
    -2.959919f,  // error=0.034156
    -2.917150f,  // error=0.034863
    -3.600301f,  // error=0.034785
    -3.024990f,  // error=0.033426
    -3.300000f
};

#define LOG_64F    4.15888308336f
#define INV_LOG_2F 1.44269504089f

void explut (float n, float *l, float *m)
{
    float idx,f,a;
    long  i;

    if (n < 1.f) {
        a  = (n == 0.f) ? 0.f : (float)exp(-LOG_64F/n);
        *l = -a;
        *m = 1.f - (1.f-a)*n;
    }
    else {
        idx = 3.f * (float)log(n) * INV_LOG_2F;
        i = (long)idx;
        f = idx-i;

        *l = lar[i]*(1.f-f) + lar[i+1]*f;
        *m = mar[i]*(1.f-f) + mar[i+1]*f;
    }
}

//---------------------------------------------------------------------------

DWORD nvCelsiusBuildTSSMask (PNVD3DCONTEXT pContext, DWORD dwStage)
{
    PNVD3DTEXSTAGESTATE pTSS;
    DWORD dwMask, dwColorOp, dwAlphaOp;

    pTSS = &(pContext->tssState[dwStage]);
    dwColorOp = pTSS->dwValue[D3DTSS_COLOROP];
    dwAlphaOp = pTSS->dwValue[D3DTSS_ALPHAOP];

    dwMask = ( ((pTSS->dwValue[D3DTSS_TEXTUREMAP] != NULL) ? 0x80000000 : 0x0) |
               ((dwColorOp & 0x0000001f) << 26) |
               ((dwAlphaOp & 0x0000001f) << 10) );

    // don't bother with args unless op is not disable. this reduces
    // several otherwise inconsequentially distinct cases to one case
    switch (dwColorOp) {
        case D3DTOP_DISABLE:
            break;
        case D3DTOP_SELECTARG1:
            dwMask |= (NVTSSARGMASK(pTSS->dwValue[D3DTSS_COLORARG1]) << 21);
            break;
        case D3DTOP_SELECTARG2:
            dwMask |= (NVTSSARGMASK(pTSS->dwValue[D3DTSS_COLORARG2]) << 16);
            break;
        default:
            dwMask |= ((NVTSSARGMASK(pTSS->dwValue[D3DTSS_COLORARG1]) << 21) |
                       (NVTSSARGMASK(pTSS->dwValue[D3DTSS_COLORARG2]) << 16));
            break;
    }

    switch (dwAlphaOp) {
        case D3DTOP_DISABLE:
            break;
        case D3DTOP_SELECTARG1:
            dwMask |= (NVTSSARGMASK(pTSS->dwValue[D3DTSS_ALPHAARG1]) << 5);
            break;
        case D3DTOP_SELECTARG2:
            dwMask |= (NVTSSARGMASK(pTSS->dwValue[D3DTSS_ALPHAARG2]) << 0);
            break;
        default:
            dwMask |= ((NVTSSARGMASK(pTSS->dwValue[D3DTSS_ALPHAARG1]) << 5) |
                       (NVTSSARGMASK(pTSS->dwValue[D3DTSS_ALPHAARG2]) << 0));
            break;
    }

    //dwMask |= ((pTSS->dwValue[D3DTSS_RESULTARG] == D3DTA_CURRENT) ? 0 : 1) << 15; 

    return (dwMask);
}

//---------------------------------------------------------------------------

DWORD nvCelsiusCombinersGetHashIndex
(
    DWORD dwTSSMask0,
    DWORD dwTSSMask1
)
{
    DWORD dwIndex;

    // generate an index. until i think of something better,
    // just add the 8 bytes together and mask off the lower N bits
    dwIndex = ((dwTSSMask0 >>  0) & 0xff) +
              ((dwTSSMask0 >>  8) & 0xff) +
              ((dwTSSMask0 >> 16) & 0xff) +
              ((dwTSSMask0 >> 24) & 0xff) +
              ((dwTSSMask1 >>  0) & 0xff) +
              ((dwTSSMask1 >>  8) & 0xff) +
              ((dwTSSMask1 >> 16) & 0xff) +
              ((dwTSSMask1 >> 24) & 0xff);
    dwIndex &= CELSIUSCPTABLE_INDEXMASK;

    return (dwIndex);
}

//---------------------------------------------------------------------------

// lookup a combiner program in the hash table

PCELSIUSCOMBINERPROGRAM nvCelsiusCombinersLookupProgram
(
    DWORD dwTSSMask0,
    DWORD dwTSSMask1
)
{
    PCELSIUSCOMBINERPROGRAM pProgram;
    DWORD dwIndex;

    dwIndex  = nvCelsiusCombinersGetHashIndex (dwTSSMask0, dwTSSMask1);
    pProgram = global.celsiusCombinerProgramTable[dwIndex];

    while (pProgram) {
        if ((pProgram->dwTSSMask0 == dwTSSMask0) &&
            (pProgram->dwTSSMask1 == dwTSSMask1)) {
            return (pProgram);
        }
        pProgram = pProgram->pNext;
    }

    return (NULL);
}

//---------------------------------------------------------------------------

// attempt to set celsius combiners from the hash table.
// return TRUE if successful

BOOL nvCelsiusCombinersSetFromHashTable
(
    PNVD3DCONTEXT pContext,
    DWORD dwTSSMask0,
    DWORD dwTSSMask1
)
{
    PCELSIUSCOMBINERPROGRAM pProgram = nvCelsiusCombinersLookupProgram (dwTSSMask0, dwTSSMask1);

    if (pProgram) {
        // set celsius state from the program
        pContext->hwState.dwStateFlags                 &= ~CELSIUS_MASK_COMBINERPROGRAMUNIQUENESS;
        pContext->hwState.dwStateFlags                 |= pProgram->dwStateFlags;
        pContext->hwState.dwColorICW[0]                 = pProgram->dwColorICW[0];
        pContext->hwState.dwColorOCW[0]                 = pProgram->dwColorOCW[0];
        pContext->hwState.dwAlphaICW[0]                 = pProgram->dwAlphaICW[0];
        pContext->hwState.dwAlphaOCW[0]                 = pProgram->dwAlphaOCW[0];
        pContext->hwState.dwColorICW[1]                 = pProgram->dwColorICW[1];
        pContext->hwState.dwColorOCW[1]                 = pProgram->dwColorOCW[1];
        pContext->hwState.dwAlphaICW[1]                 = pProgram->dwAlphaICW[1];
        pContext->hwState.dwAlphaOCW[1]                 = pProgram->dwAlphaOCW[1];
        pContext->hwState.dwTexUnitToTexStageMapping[0] = pProgram->dwTexUnitMapping[0];
        pContext->hwState.dwTexUnitToTexStageMapping[1] = pProgram->dwTexUnitMapping[1];
        pContext->hwState.dwNumActiveCombinerStages     = pProgram->dwNumActiveStages;
        return TRUE;
    }
    else {
        return FALSE;
    }
}

//---------------------------------------------------------------------------

// add a combiner program to the hash table

void nvCelsiusCombinersAddProgramToHashTable
(
    PCELSIUSCOMBINERPROGRAM pProgram
)
{
    DWORD dwIndex;

    // make sure it's not already in the hash table (needed when using running apps)
    if (nvCelsiusCombinersLookupProgram (pProgram->dwTSSMask0, pProgram->dwTSSMask1)) {
        delete pProgram;
        return;
    }

    // hash out an index
    dwIndex = nvCelsiusCombinersGetHashIndex (pProgram->dwTSSMask0, pProgram->dwTSSMask1);

    // add the new program to the front of the list
    pProgram->pNext = global.celsiusCombinerProgramTable[dwIndex];
    global.celsiusCombinerProgramTable[dwIndex] = pProgram;
}

//---------------------------------------------------------------------------

// add the current combiner settings to the hash table as a new program.
// return true if successful.

BOOL nvCelsiusCombinersAddToHashTable
(
    PNVD3DCONTEXT pContext,
    DWORD dwTSSMask0,
    DWORD dwTSSMask1
)
{
    PCELSIUSCOMBINERPROGRAM pProgram;

/*    
#ifdef DEBUG
    // make sure it's not already in the hash table
    if (nvCelsiusCombinersLookupProgram (dwTSSMask0, dwTSSMask1)) {
        DPF ("nvCelsiusCombinersAddToHashTable: attempt to add something redundant");
        dbgD3DError();
    }
#endif
*/
    // create the program
    pProgram = (PCELSIUSCOMBINERPROGRAM) AllocIPM (sizeof(CELSIUSCOMBINERPROGRAM));

    if (!pProgram) {
        // couldn't get memory. just return
        DPF ("failure to allocate memory for combiner program hash table entry");
        dbgD3DError();
        return (FALSE);
    }

    pProgram->dwTSSMask0          = dwTSSMask0;
    pProgram->dwTSSMask1          = dwTSSMask1;
    pProgram->dwStateFlags        = pContext->hwState.dwStateFlags & CELSIUS_MASK_COMBINERPROGRAMUNIQUENESS;
    pProgram->dwColorICW[0]       = pContext->hwState.dwColorICW[0];
    pProgram->dwColorOCW[0]       = pContext->hwState.dwColorOCW[0];
    pProgram->dwAlphaICW[0]       = pContext->hwState.dwAlphaICW[0];
    pProgram->dwAlphaOCW[0]       = pContext->hwState.dwAlphaOCW[0];
    pProgram->dwColorICW[1]       = pContext->hwState.dwColorICW[1];
    pProgram->dwColorOCW[1]       = pContext->hwState.dwColorOCW[1];
    pProgram->dwAlphaICW[1]       = pContext->hwState.dwAlphaICW[1];
    pProgram->dwAlphaOCW[1]       = pContext->hwState.dwAlphaOCW[1];
    pProgram->dwTexUnitMapping[0] = pContext->hwState.dwTexUnitToTexStageMapping[0];
    pProgram->dwTexUnitMapping[1] = pContext->hwState.dwTexUnitToTexStageMapping[1];
    pProgram->dwNumActiveStages   = pContext->hwState.dwNumActiveCombinerStages;

    nvCelsiusCombinersAddProgramToHashTable (pProgram);

    return (TRUE);
}

//---------------------------------------------------------------------------

#define CELSIUS_VALIDATE_FAIL(reason)   \
{                                       \
    pvtssd->dwNumPasses = 0xffffffff;   \
    pvtssd->ddrval = reason;            \
    dbgTracePop();                      \
    return (DDHAL_DRIVER_HANDLED);      \
}

//---------------------------------------------------------------------------

DWORD nvCelsiusValidateTextureStageState
(
    LPD3DHAL_VALIDATETEXTURESTAGESTATEDATA pvtssd
)
{
    DWORD dwHWStage, dwD3DStage;

    dbgTracePush ("nvCelsiusValidateTextureStageState");

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pvtssd);

    // update everything
    pContext->hwState.dwDirtyFlags = CELSIUS_DIRTY_REALLY_FILTHY;
    nvSetCelsiusState (pContext);

    // make sure the setup went smoothly
    if (pContext->hwState.dwStateFlags & CELSIUS_FLAG_SETUPFAILURE) {
        CELSIUS_VALIDATE_FAIL (D3DERR_CONFLICTINGRENDERSTATE);
    }

#ifndef CELSIUS_NVCOMBINER
    // make sure we don't have more than two texture stages in use
    DWORD dwStageCount = 0;
    if (!pContext->bUseTBlendSettings) {
        while ((dwStageCount < 8) && (pContext->tssState[dwStageCount].dwValue[D3DTSS_COLOROP] != D3DTOP_DISABLE)) {
            dwStageCount++;
        }
        if (dwStageCount > CELSIUS_CAPS_MAX_SIMULTANEOUS_TEXTURES) {
            CELSIUS_VALIDATE_FAIL (D3DERR_TOOMANYOPERATIONS);
        }
    }
#endif

    // count free stages
    DWORD dwFreeStages = 0;
    for (dwHWStage = 0; dwHWStage < 2; dwHWStage++) {
        // which D3D stage is handled by this HW stage
        dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];

        if (dwD3DStage == CELSIUS_UNUSED) {
            dwFreeStages++;
        }
    }

    // check clip planes
    DWORD dwClipPlaneEnable = pContext->dwRenderState[D3DRENDERSTATE_CLIPPLANEENABLE];
    DWORD dwClipPlaneCount = 0;

    // count the planes
    while (dwClipPlaneEnable) {
        if (dwClipPlaneEnable & 0x1) dwClipPlaneCount++;
        dwClipPlaneEnable >>= 1;
    }

    if (dwClipPlaneCount > 2*dwFreeStages) {
        CELSIUS_VALIDATE_FAIL (D3DERR_TOOMANYOPERATIONS);
    }

    // verify texture transform
    for (dwHWStage=0; dwHWStage < 2; dwHWStage++) {

        DWORD dwTexUnitStatus = pContext->hwState.dwTextureUnitStatus[dwHWStage];

        if (dwTexUnitStatus == CELSIUS_TEXUNITSTATUS_USER) {

            dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];

            DWORD dwOutCount = pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS] & 0xff;
            BOOL  bXForm     = (dwOutCount != D3DTTFF_DISABLE);

            DWORD dwTexgenMode = pContext->tssState[dwD3DStage].dwValue[D3DTSS_TEXCOORDINDEX] & 0xffff0000;
            BOOL  bTexgen      = (dwTexgenMode != D3DTSS_TCI_PASSTHRU);

            if ((bXForm || bTexgen) &&
                (pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE) &&
                (!pContext->pCurrentVShader->hasProgram()))
            {
                CELSIUS_VALIDATE_FAIL (D3DERR_CONFLICTINGRENDERSTATE);
            }

        }

        // check for cube maps and w-buffering
        if (pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage] != CELSIUS_UNUSED) {
            DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[dwHWStage];
            if (dwD3DStage != CELSIUS_UNUSED) {
                PNVD3DTEXSTAGESTATE pTSSState = &(pContext->tssState[dwD3DStage]);
                CTexture* pTexture = ((CNvObject *)(pTSSState->dwValue[D3DTSS_TEXTUREMAP]))->getTexture();
                DWORD dwZEnable = pContext->pZetaBuffer ? pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] : 0;
                if (pTexture->isCubeMap() && (dwZEnable == D3DZB_USEW)) {
                    CELSIUS_VALIDATE_FAIL (D3DERR_CONFLICTINGRENDERSTATE);
                }
            }

        }

    }

    // verify color material
    if (pContext->dwRenderState[D3DRENDERSTATE_COLORVERTEX]) {

        if (pContext->dwRenderState[D3DRENDERSTATE_AMBIENTMATERIALSOURCE] == D3DMCS_COLOR2) {
            if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                CELSIUS_VALIDATE_FAIL (D3DERR_CONFLICTINGRENDERSTATE);
            }
        }

        if (pContext->dwRenderState[D3DRENDERSTATE_DIFFUSEMATERIALSOURCE] == D3DMCS_COLOR2) {
            if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                CELSIUS_VALIDATE_FAIL (D3DERR_CONFLICTINGRENDERSTATE);
            }
        }

        if (pContext->dwRenderState[D3DRENDERSTATE_SPECULARMATERIALSOURCE] == D3DMCS_COLOR1) {
            if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE])) {
                if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                    // if vertex specular is valid, enabling NV056_SET_COLOR_MATERIAL_SPECULAR
                    // will make the HW pull material specular from the vertex specular,
                    // NOT vertex diffuse as desired.
                    CELSIUS_VALIDATE_FAIL (D3DERR_CONFLICTINGRENDERSTATE);
                }
            }
        }

        if (pContext->dwRenderState[D3DRENDERSTATE_EMISSIVEMATERIALSOURCE] == D3DMCS_COLOR2) {
            if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])) {
                CELSIUS_VALIDATE_FAIL (D3DERR_CONFLICTINGRENDERSTATE);
            }
        }

    }

    // check lights
    if (GET_LIGHTING_STATE(pContext) && (!(pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE))) {

        DWORD    dwNumLights = 0;
        pnvLight pLight = pContext->lighting.pActiveLights;

        while (pLight) {
            dwNumLights++;
            pLight = pLight->pNext;
        }

        if (dwNumLights > 8) {
            CELSIUS_VALIDATE_FAIL (D3DERR_CONFLICTINGRENDERSTATE);
        }

    }

    // miscellaneous sanity checks (are these legit or should we just quietly tolerate such crap?)
    if (pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]) {
        if ((pContext->pZetaBuffer == NULL) ||
            (pContext->pZetaBuffer->hasStencil() == FALSE)) {
            CELSIUS_VALIDATE_FAIL (D3DERR_CONFLICTINGRENDERSTATE);
        }
    }

    if (pContext->hwState.celsius.dwZEnable != D3DZB_FALSE) {
        if (pContext->pZetaBuffer == NULL) {
            CELSIUS_VALIDATE_FAIL (D3DERR_CONFLICTINGRENDERSTATE);
        }
    }

    // everything is kosher. get the number of passes and return OK
    pvtssd->dwNumPasses = 1;    // don't want this... implies multipass when it is not. pContext->hwState.celsius.dwNumActiveCombinerStages;
    pvtssd->ddrval      = D3D_OK;

    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);


}
#endif  // NVARCH >= 0x010
