/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvStateSet.cpp                                                    *
*   Functions called to manage DP2 statesets                                *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler         26Jan99           created                  *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"


#if (NVARCH >= 0x04)

//---------------------------------------------------------------------------

// grow the stateset to at least the size specified

HRESULT growStateSetData (PNVD3DCONTEXT pContext, DWORD dwHandle, DWORD desiredSize)
{
    STATESET *pss;
    DWORD newSize;
    LPBYTE pNewData;

    pss = pContext->pStateSets + dwHandle;

    newSize = pss->allocatedSize;
    while (newSize < desiredSize) newSize += INCREMENTAL_STATESET_SIZE;

    pNewData = (LPBYTE) new BYTE[newSize];
    if (pNewData == NULL) {
        DPF ("Could not allocate more memory for stateset data");
        return (DDERR_OUTOFMEMORY);
    }   

    // transfer data
    pss->allocatedSize = newSize;
    nvMemCopy (pNewData, pss->pData, pss->dataSize);
    delete []pss->pData;
    pss->pData = pNewData;

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordState (PNVD3DCONTEXT pContext, LPBYTE pData, DWORD dwSize)
{
    DWORD     dwHandle;
    STATESET *pss;
    int       newSize;
    LPBYTE    pDest;
    HRESULT   ret;

    dwHandle = pContext->dwCurrentStateSet;
    pss      = pContext->pStateSets + dwHandle;
    assert (pss != NULL);

    newSize = pss->dataSize + dwSize;
    if (newSize > pss->allocatedSize) {
        ret = growStateSetData (pContext, dwHandle, newSize);
        if (ret != D3D_OK) return (ret);
    }

    // copy the data and update the ptr.
    pDest = pss->pData + pss->dataSize;
    nvMemCopy(pDest, pData, dwSize);
    pss->dataSize = newSize;

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordLastStateOnly (PNVD3DCONTEXT pContext, LPD3DHAL_DP2COMMAND pCmd, DWORD dwUnitSize)
{
    WORD    wStateCount;
    LPBYTE  pSrc;
    HRESULT ret;

    wStateCount = pCmd->wStateCount;
    assert (wStateCount > 0);

    if (wStateCount == 1) {
        // record the command and the single state
        return (recordState (pContext, (LPBYTE)pCmd, sizeof(D3DHAL_DP2COMMAND)+dwUnitSize));
    }

    else {
        pCmd->wStateCount = 1;

        // record the command itself
        ret = recordState (pContext, (LPBYTE)pCmd, sizeof(D3DHAL_DP2COMMAND));
        if (ret != D3D_OK) return (ret);

        // get the last piece of state
        pSrc = (LPBYTE)(pCmd+1) + dwUnitSize*(wStateCount-1);
        ret = recordState (pContext, pSrc, dwUnitSize);
        if (ret != D3D_OK) return (ret);

        pCmd->wStateCount = wStateCount;
        return (D3D_OK);
    }
}

//---------------------------------------------------------------------------

#ifdef DEBUG
static DWORD dwStateSetSerial[2] = { 0xabcd1234, 0};
void recordDebugMarker (PNVD3DCONTEXT pContext)
{
    dwStateSetSerial[1]++;
    recordState (pContext, (BYTE*)dwStateSetSerial, sizeof(dwStateSetSerial));
    DPF_LEVEL (NVDBG_LEVEL_INFO,"Recording StateSet Serial %d",dwStateSetSerial[1]);
}
#endif

//---------------------------------------------------------------------------

HRESULT recordRenderStates (PNVD3DCONTEXT pContext, D3DRENDERSTATETYPE rstates[], DWORD dwNumStates)
{
    DWORD                  dwDataSize;
    DWORD                  i;
    LPBYTE                 pData;
    LPD3DHAL_DP2COMMAND    pCmd;
    D3DHAL_DP2RENDERSTATE* pRS;

    dwDataSize = sizeof(D3DHAL_DP2COMMAND) + (sizeof(D3DHAL_DP2RENDERSTATE) * dwNumStates);
    pData = (LPBYTE) new BYTE[dwDataSize];

    pCmd = (LPD3DHAL_DP2COMMAND)&(pData[0]);
    pCmd->wPrimitiveCount = dwNumStates;
    pCmd->bCommand        = D3DDP2OP_RENDERSTATE;

    pRS = (D3DHAL_DP2RENDERSTATE*)(pCmd+1);
    for (i=0; i<dwNumStates; i++) {
        pRS->RenderState = rstates[i];
        pRS->dwState = pContext->dwRenderState[rstates[i]];
        pRS++;
    }

    recordState (pContext, pData, dwDataSize);
    recordDebugMarker (pContext);

    delete []pData;

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordTextureStageStates (PNVD3DCONTEXT pContext, D3DTEXTURESTAGESTATETYPE tsstates[], DWORD dwNumStates)
{
    DWORD                        dwDataSize;
    DWORD                        i, j;
    LPBYTE                       pData;
    LPD3DHAL_DP2COMMAND          pCmd;
    D3DHAL_DP2TEXTURESTAGESTATE* pTSS;

    dwDataSize = sizeof(D3DHAL_DP2COMMAND) + (sizeof(D3DHAL_DP2TEXTURESTAGESTATE) * dwNumStates * D3DHAL_TSS_MAXSTAGES);
    pData = (LPBYTE) new BYTE[dwDataSize];

    pCmd = (LPD3DHAL_DP2COMMAND)&(pData[0]);
    pCmd->wPrimitiveCount = dwNumStates * D3DHAL_TSS_MAXSTAGES;
    pCmd->bCommand        = D3DDP2OP_TEXTURESTAGESTATE;

    pTSS = (D3DHAL_DP2TEXTURESTAGESTATE*)(pCmd+1);
    for (i=0; i<D3DHAL_TSS_MAXSTAGES; i++) {
        for (j=0; j<dwNumStates; j++) {
            pTSS->wStage  = i;
            pTSS->TSState = tsstates[j];
            pTSS->dwValue = pContext->tssState[i].dwValue[tsstates[j]];
            pTSS++;
        }
    }

    recordState (pContext, pData, dwDataSize);
    recordDebugMarker (pContext);

    delete []pData;

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordViewport (PNVD3DCONTEXT pContext)
{
    DWORD                   dwDataSize;
    LPBYTE                  pData;
    LPD3DHAL_DP2COMMAND     pCmd;
    D3DHAL_DP2VIEWPORTINFO* pVP;

    dwDataSize = sizeof(D3DHAL_DP2COMMAND) + sizeof(D3DHAL_DP2VIEWPORTINFO);
    pData = (LPBYTE) new BYTE[dwDataSize];

    pCmd = (LPD3DHAL_DP2COMMAND)&(pData[0]);
    pCmd->wPrimitiveCount = 1;
    pCmd->bCommand        = D3DDP2OP_VIEWPORTINFO;

    pVP = (D3DHAL_DP2VIEWPORTINFO*)(pCmd+1);
    pVP->dwX      = pContext->surfaceViewport.clipHorizontal.wX;
    pVP->dwY      = pContext->surfaceViewport.clipVertical.wY;
    pVP->dwWidth  = pContext->surfaceViewport.clipHorizontal.wWidth;
    pVP->dwHeight = pContext->surfaceViewport.clipVertical.wHeight;

    recordState (pContext, pData, dwDataSize);
    recordDebugMarker (pContext);

    delete []pData;

    dwDataSize = sizeof(D3DHAL_DP2COMMAND) + sizeof(D3DHAL_DP2ZRANGE);
    pData = (LPBYTE) new BYTE[dwDataSize];

    pCmd = (LPD3DHAL_DP2COMMAND)&(pData[0]);
    pCmd->wPrimitiveCount = 1;
    pCmd->bCommand        = D3DDP2OP_ZRANGE;

    D3DHAL_DP2ZRANGE* pZR = (D3DHAL_DP2ZRANGE*)(pCmd+1);
    pZR->dvMinZ = pContext->surfaceViewport.dvMinZ;
    pZR->dvMaxZ = pContext->surfaceViewport.dvMaxZ;

    recordState (pContext, pData, dwDataSize);
    recordDebugMarker (pContext);

    delete []pData;

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordTransforms (PNVD3DCONTEXT pContext)
{
    DWORD                   dwDataSize;
    DWORD                   i;
    LPBYTE                  pData;
    LPD3DHAL_DP2COMMAND     pCmd;
    D3DHAL_DP2SETTRANSFORM* pST;

    dwDataSize = sizeof(D3DHAL_DP2COMMAND) + (sizeof(D3DHAL_DP2SETTRANSFORM) * (NV_CAPS_MAX_MATRICES + D3DHAL_TSS_MAXSTAGES + 2));
    pData = (LPBYTE) new BYTE[dwDataSize];

    pCmd = (LPD3DHAL_DP2COMMAND)&(pData[0]);
    pCmd->wPrimitiveCount = NV_CAPS_MAX_MATRICES + D3DHAL_TSS_MAXSTAGES + 2;
    pCmd->bCommand        = D3DDP2OP_SETTRANSFORM;

    pST = (D3DHAL_DP2SETTRANSFORM*)(pCmd+1);
    // world
    for (i=0; i<NV_CAPS_MAX_MATRICES; i++) {
        pST->xfrmType = D3DTS_WORLDMATRIX(i);;
        pST->matrix = pContext->xfmWorld[i];
        pST++;
    }
    // view
    pST->xfrmType = D3DTRANSFORMSTATE_VIEW;
    pST->matrix = pContext->xfmView;
    pST++;
    // projection
    pST->xfrmType = D3DTRANSFORMSTATE_PROJECTION;
    pST->matrix = pContext->xfmProj;
    pST++;
    // texture
    for (i=0; i<D3DHAL_TSS_MAXSTAGES; i++) {
        pST->xfrmType = (D3DTRANSFORMSTATETYPE)(D3DTRANSFORMSTATE_TEXTURE0 + i);
        pST->matrix = pContext->tssState[i].mTexTransformMatrix;
        pST++;
    }

    recordState (pContext, pData, dwDataSize);
    recordDebugMarker (pContext);

    delete []pData;

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordClipPlanes (PNVD3DCONTEXT pContext)
{
    DWORD                   dwDataSize;
    DWORD                   i, j;
    LPBYTE                  pData;
    LPD3DHAL_DP2COMMAND     pCmd;
    D3DHAL_DP2SETCLIPPLANE* pSCP;

    dwDataSize = sizeof(D3DHAL_DP2COMMAND) + sizeof(D3DHAL_DP2SETCLIPPLANE) * NV_CAPS_MAX_CLIPPLANES;
    pData = (LPBYTE) new BYTE[dwDataSize];

    pCmd = (LPD3DHAL_DP2COMMAND)&(pData[0]);
    pCmd->wPrimitiveCount = NV_CAPS_MAX_CLIPPLANES;
    pCmd->bCommand        = D3DDP2OP_SETCLIPPLANE;

    pSCP = (D3DHAL_DP2SETCLIPPLANE*)(pCmd+1);
    for (i=0; i<NV_CAPS_MAX_CLIPPLANES; i++) {
        pSCP->dwIndex = i;
        for (j=0; j<4; j++) {
            pSCP->plane[j] = pContext->ppClipPlane[i][j];
        }
        pSCP++;
    }

    recordState (pContext, pData, dwDataSize);
    recordDebugMarker (pContext);

    delete []pData;

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordMaterial (PNVD3DCONTEXT pContext)
{
    DWORD                  dwDataSize;
    LPBYTE                 pData;
    LPD3DHAL_DP2COMMAND    pCmd;
    D3DHAL_DP2SETMATERIAL* pSM;

    dwDataSize = sizeof(D3DHAL_DP2COMMAND) + sizeof(D3DHAL_DP2SETMATERIAL);
    pData = (LPBYTE) new BYTE[dwDataSize];

    pCmd = (LPD3DHAL_DP2COMMAND)&(pData[0]);
    pCmd->wPrimitiveCount = 1;
    pCmd->bCommand        = D3DDP2OP_SETMATERIAL;

    pSM = (D3DHAL_DP2SETMATERIAL*)(pCmd+1);
    *pSM = pContext->Material;

    recordState (pContext, pData, dwDataSize);
    recordDebugMarker (pContext);

    delete []pData;

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordLights (PNVD3DCONTEXT pContext)
{
    DWORD               dwDataSize;
    DWORD               i;
    LPBYTE              pData;
    LPD3DHAL_DP2COMMAND pCmd;
    D3DHAL_DP2SETLIGHT* pSLEnable;
    D3DHAL_DP2SETLIGHT* pSLData;

    dwDataSize = sizeof(D3DHAL_DP2COMMAND) + sizeof(D3DLIGHT7) + 2*sizeof(D3DHAL_DP2SETLIGHT);
    pData = (LPBYTE) new BYTE[dwDataSize];

    pCmd = (LPD3DHAL_DP2COMMAND)&(pData[0]);
    pCmd->wPrimitiveCount = 2;
    pCmd->bCommand = D3DDP2OP_SETLIGHT;

    pSLEnable = (D3DHAL_DP2SETLIGHT *)(pCmd + 1);
    pSLData   = pSLEnable + 1;
    pSLData->dwDataType = D3DHAL_SETLIGHT_DATA;

    for (i=0; i<pContext->dwLightArraySize; i++) {
        if (nvLightIsDefined(&(pContext->pLightArray[i]))) {
            pSLEnable->dwIndex = pSLData->dwIndex = i;
            pSLEnable->dwDataType = (nvLightIsEnabled(&(pContext->pLightArray[i]))) ?
                                    D3DHAL_SETLIGHT_ENABLE :
                                    D3DHAL_SETLIGHT_DISABLE;
            *((D3DLIGHT7*)(pSLData + 1)) = pContext->pLightArray[i].Light7;
            recordState (pContext, pData, dwDataSize);
            recordDebugMarker (pContext);
        }
    }

    delete []pData;

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordVShader (PNVD3DCONTEXT pContext)
{
    DWORD                           dwDataSize;
    LPBYTE                          pData;
    LPD3DHAL_DP2COMMAND             pCmd;
    D3DHAL_DP2VERTEXSHADER*         pVS;
    D3DHAL_DP2SETVERTEXSHADERCONST* pVSC;

    dwDataSize = sizeof(D3DHAL_DP2COMMAND) + sizeof(D3DHAL_DP2VERTEXSHADER);
    pData = (LPBYTE) new BYTE[dwDataSize];

    pCmd = (LPD3DHAL_DP2COMMAND)&(pData[0]);
    pCmd->wPrimitiveCount = 1;
    pCmd->bCommand        = D3DDP2OP_SETVERTEXSHADER;

    pVS = (D3DHAL_DP2VERTEXSHADER*)(pCmd+1);
    pVS->dwHandle = pContext->pCurrentVShader->isFvfShader() ?
        pContext->pCurrentVShader->getFVF():
        pContext->pCurrentVShader->getHandle();

    recordState (pContext, pData, dwDataSize);
    recordDebugMarker (pContext);

    delete []pData;

    dwDataSize = sizeof(D3DHAL_DP2COMMAND) + sizeof(D3DHAL_DP2SETVERTEXSHADERCONST) + (NV_CAPS_MAX_VSHADER_CONSTS << 4);
    pData = (LPBYTE) new BYTE[dwDataSize];

    pCmd = (LPD3DHAL_DP2COMMAND)&(pData[0]);
    pCmd->wPrimitiveCount = 1;
    pCmd->bCommand        = D3DDP2OP_SETVERTEXSHADERCONST;

    pVSC = (D3DHAL_DP2SETVERTEXSHADERCONST*)(pCmd+1);
    pVSC->dwRegister = 0;
    pVSC->dwCount    = NV_CAPS_MAX_VSHADER_CONSTS;
    nvAssert(pContext->pVShaderConsts);
    nvMemCopy ((DWORD)(pVSC+1), (DWORD)(&(pContext->pVShaderConsts->vertexShaderConstants[0])), (NV_CAPS_MAX_VSHADER_CONSTS << 4));

    recordState (pContext, pData, dwDataSize);
    recordDebugMarker (pContext);

    delete []pData;

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordPShader (PNVD3DCONTEXT pContext)
{
    DWORD                          dwDataSize;
    DWORD                          i;
    LPBYTE                         pData;
    LPD3DHAL_DP2COMMAND            pCmd;
    D3DHAL_DP2PIXELSHADER*         pPS;
    D3DHAL_DP2SETPIXELSHADERCONST* pPSC;
    D3DVALUE*                      pfData;

    dwDataSize = sizeof(D3DHAL_DP2COMMAND) + sizeof(D3DHAL_DP2PIXELSHADER);
    pData = (LPBYTE) new BYTE[dwDataSize];

    pCmd = (LPD3DHAL_DP2COMMAND)&(pData[0]);
    pCmd->wPrimitiveCount = 1;
    pCmd->bCommand        = D3DDP2OP_SETPIXELSHADER;

    pPS = (D3DHAL_DP2PIXELSHADER*)(pCmd+1);
    pPS->dwHandle = pContext->pCurrentPShader ? pContext->pCurrentPShader->getHandle() : 0;

    recordState (pContext, pData, dwDataSize);
    recordDebugMarker (pContext);

    delete []pData;

    dwDataSize = sizeof(D3DHAL_DP2COMMAND) + sizeof(D3DHAL_DP2SETPIXELSHADERCONST) + (NV_CAPS_MAX_PSHADER_CONSTS << 4);
    pData = (LPBYTE) new BYTE[dwDataSize];

    pCmd = (LPD3DHAL_DP2COMMAND)&(pData[0]);
    pCmd->wPrimitiveCount = 1;
    pCmd->bCommand        = D3DDP2OP_SETPIXELSHADERCONST;

    pPSC = (D3DHAL_DP2SETPIXELSHADERCONST*)(pCmd+1);
    pPSC->dwRegister = 0;
    pPSC->dwCount    = NV_CAPS_MAX_PSHADER_CONSTS;
    pfData = (D3DVALUE*)(pPSC+1);
    for (i=0; i<NV_CAPS_MAX_PSHADER_CONSTS; i++) {
        *(pfData+0) = pContext->pixelShaderConsts[i].dvA;
        *(pfData+1) = pContext->pixelShaderConsts[i].dvR;
        *(pfData+2) = pContext->pixelShaderConsts[i].dvG;
        *(pfData+3) = pContext->pixelShaderConsts[i].dvB;
        pfData += 4;
    }

    recordState (pContext, pData, dwDataSize);
    recordDebugMarker (pContext);

    delete []pData;

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordAllState (PNVD3DCONTEXT pContext)
{
    static D3DRENDERSTATETYPE rstates[] =
    {
        D3DRENDERSTATE_SPECULARENABLE,
        D3DRENDERSTATE_ZENABLE,
        D3DRENDERSTATE_FILLMODE,
        D3DRENDERSTATE_SHADEMODE,
        D3DRENDERSTATE_LINEPATTERN,
        D3DRENDERSTATE_ZWRITEENABLE,
        D3DRENDERSTATE_ALPHATESTENABLE,
        D3DRENDERSTATE_LASTPIXEL,
        D3DRENDERSTATE_SRCBLEND,
        D3DRENDERSTATE_DESTBLEND,
        D3DRENDERSTATE_CULLMODE,
        D3DRENDERSTATE_ZFUNC,
        D3DRENDERSTATE_ALPHAREF,
        D3DRENDERSTATE_ALPHAFUNC,
        D3DRENDERSTATE_DITHERENABLE,
        D3DRENDERSTATE_FOGENABLE,
        D3DRENDERSTATE_STIPPLEDALPHA,
        D3DRENDERSTATE_FOGCOLOR,
        D3DRENDERSTATE_FOGTABLEMODE,
        D3DRENDERSTATE_FOGSTART,
        D3DRENDERSTATE_FOGEND,
        D3DRENDERSTATE_FOGDENSITY,
        D3DRENDERSTATE_EDGEANTIALIAS,
        D3DRENDERSTATE_ALPHABLENDENABLE,
        D3DRENDERSTATE_ZBIAS,
        D3DRENDERSTATE_RANGEFOGENABLE,
        D3DRENDERSTATE_STENCILENABLE,
        D3DRENDERSTATE_STENCILFAIL,
        D3DRENDERSTATE_STENCILZFAIL,
        D3DRENDERSTATE_STENCILPASS,
        D3DRENDERSTATE_STENCILFUNC,
        D3DRENDERSTATE_STENCILREF,
        D3DRENDERSTATE_STENCILMASK,
        D3DRENDERSTATE_STENCILWRITEMASK,
        D3DRENDERSTATE_TEXTUREFACTOR,
        D3DRENDERSTATE_WRAP0,
        D3DRENDERSTATE_WRAP1,
        D3DRENDERSTATE_WRAP2,
        D3DRENDERSTATE_WRAP3,
        D3DRENDERSTATE_WRAP4,
        D3DRENDERSTATE_WRAP5,
        D3DRENDERSTATE_WRAP6,
        D3DRENDERSTATE_WRAP7,
        D3DRENDERSTATE_AMBIENT,
        D3DRENDERSTATE_COLORVERTEX,
        D3DRENDERSTATE_FOGVERTEXMODE,
        D3DRENDERSTATE_CLIPPING,
        D3DRENDERSTATE_LIGHTING,
        D3DRENDERSTATE_NORMALIZENORMALS,
        D3DRENDERSTATE_LOCALVIEWER,
        D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,
        D3DRENDERSTATE_AMBIENTMATERIALSOURCE,
        D3DRENDERSTATE_DIFFUSEMATERIALSOURCE,
        D3DRENDERSTATE_SPECULARMATERIALSOURCE,
        D3DRENDERSTATE_VERTEXBLEND,
        D3DRENDERSTATE_CLIPPLANEENABLE,
        D3DRS_SOFTWAREVERTEXPROCESSING,
        D3DRS_POINTSIZE,
        D3DRS_POINTSIZE_MIN,
        D3DRS_POINTSPRITEENABLE,
        D3DRS_POINTSCALEENABLE,
        D3DRS_POINTSCALE_A,
        D3DRS_POINTSCALE_B,
        D3DRS_POINTSCALE_C,
        D3DRS_MULTISAMPLEANTIALIAS,
        D3DRS_MULTISAMPLEMASK,
        D3DRS_PATCHEDGESTYLE,
        D3DRS_PATCHSEGMENTS,
        D3DRS_POINTSIZE_MAX,
        D3DRS_INDEXEDVERTEXBLENDENABLE,
        D3DRS_COLORWRITEENABLE,
        D3DRS_TWEENFACTOR,
        D3DRS_BLENDOP,
    };
    static D3DTEXTURESTAGESTATETYPE tsstates[] =
    {
        D3DTSS_COLOROP,
        D3DTSS_COLORARG1,
        D3DTSS_COLORARG2,
        D3DTSS_ALPHAOP,
        D3DTSS_ALPHAARG1,
        D3DTSS_ALPHAARG2,
        D3DTSS_BUMPENVMAT00,
        D3DTSS_BUMPENVMAT01,
        D3DTSS_BUMPENVMAT10,
        D3DTSS_BUMPENVMAT11,
        D3DTSS_TEXCOORDINDEX,
        D3DTSS_ADDRESSU,
        D3DTSS_ADDRESSV,
        D3DTSS_BORDERCOLOR,
        D3DTSS_MAGFILTER,
        D3DTSS_MINFILTER,
        D3DTSS_MIPFILTER,
        D3DTSS_MIPMAPLODBIAS,
        D3DTSS_MAXMIPLEVEL,
        D3DTSS_MAXANISOTROPY,
        D3DTSS_BUMPENVLSCALE,
        D3DTSS_BUMPENVLOFFSET,
        D3DTSS_TEXTURETRANSFORMFLAGS,
        D3DTSS_ADDRESSW,
        D3DTSS_COLORARG0,
        D3DTSS_ALPHAARG0,
        D3DTSS_RESULTARG,
    };

    recordRenderStates       (pContext, rstates, sizeof(rstates) / sizeof(D3DRENDERSTATETYPE));
    recordTextureStageStates (pContext, tsstates, sizeof(tsstates) / sizeof(D3DTEXTURESTAGESTATETYPE));
    recordViewport           (pContext);
    recordTransforms         (pContext);
    recordClipPlanes         (pContext);
    recordMaterial           (pContext);
    recordLights             (pContext);
    recordVShader            (pContext);
    recordPShader            (pContext);

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordPixelState (PNVD3DCONTEXT pContext)
{
    static D3DRENDERSTATETYPE rstates[] =
    {
        D3DRENDERSTATE_ZENABLE,
        D3DRENDERSTATE_FILLMODE,
        D3DRENDERSTATE_SHADEMODE,
        D3DRENDERSTATE_LINEPATTERN,
        D3DRENDERSTATE_ZWRITEENABLE,
        D3DRENDERSTATE_ALPHATESTENABLE,
        D3DRENDERSTATE_LASTPIXEL,
        D3DRENDERSTATE_SRCBLEND,
        D3DRENDERSTATE_DESTBLEND,
        D3DRENDERSTATE_ZFUNC,
        D3DRENDERSTATE_ALPHAREF,
        D3DRENDERSTATE_ALPHAFUNC,
        D3DRENDERSTATE_DITHERENABLE,
        D3DRENDERSTATE_STIPPLEDALPHA,
        D3DRENDERSTATE_FOGSTART,
        D3DRENDERSTATE_FOGEND,
        D3DRENDERSTATE_FOGDENSITY,
        D3DRENDERSTATE_EDGEANTIALIAS,
        D3DRENDERSTATE_ALPHABLENDENABLE,
        D3DRENDERSTATE_ZBIAS,
        D3DRENDERSTATE_STENCILENABLE,
        D3DRENDERSTATE_STENCILFAIL,
        D3DRENDERSTATE_STENCILZFAIL,
        D3DRENDERSTATE_STENCILPASS,
        D3DRENDERSTATE_STENCILFUNC,
        D3DRENDERSTATE_STENCILREF,
        D3DRENDERSTATE_STENCILMASK,
        D3DRENDERSTATE_STENCILWRITEMASK,
        D3DRENDERSTATE_TEXTUREFACTOR,
        D3DRENDERSTATE_WRAP0,
        D3DRENDERSTATE_WRAP1,
        D3DRENDERSTATE_WRAP2,
        D3DRENDERSTATE_WRAP3,
        D3DRENDERSTATE_WRAP4,
        D3DRENDERSTATE_WRAP5,
        D3DRENDERSTATE_WRAP6,
        D3DRENDERSTATE_WRAP7,
        D3DRS_COLORWRITEENABLE,
        D3DRS_BLENDOP,
    };
    static D3DTEXTURESTAGESTATETYPE tsstates[] =
    {
        D3DTSS_COLOROP,
        D3DTSS_COLORARG1,
        D3DTSS_COLORARG2,
        D3DTSS_ALPHAOP,
        D3DTSS_ALPHAARG1,
        D3DTSS_ALPHAARG2,
        D3DTSS_BUMPENVMAT00,
        D3DTSS_BUMPENVMAT01,
        D3DTSS_BUMPENVMAT10,
        D3DTSS_BUMPENVMAT11,
        D3DTSS_TEXCOORDINDEX,
        D3DTSS_ADDRESSU,
        D3DTSS_ADDRESSV,
        D3DTSS_BORDERCOLOR,
        D3DTSS_MAGFILTER,
        D3DTSS_MINFILTER,
        D3DTSS_MIPFILTER,
        D3DTSS_MIPMAPLODBIAS,
        D3DTSS_MAXMIPLEVEL,
        D3DTSS_MAXANISOTROPY,
        D3DTSS_BUMPENVLSCALE,
        D3DTSS_BUMPENVLOFFSET,
        D3DTSS_TEXTURETRANSFORMFLAGS,
        D3DTSS_ADDRESSW,
        D3DTSS_COLORARG0,
        D3DTSS_ALPHAARG0,
        D3DTSS_RESULTARG,
    };

    recordRenderStates       (pContext, rstates, sizeof(rstates) / sizeof(D3DRENDERSTATETYPE));
    recordTextureStageStates (pContext, tsstates, sizeof(tsstates) / sizeof(D3DTEXTURESTAGESTATETYPE));
    recordPShader            (pContext);

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT recordVertexState (PNVD3DCONTEXT pContext)
{
    static D3DRENDERSTATETYPE rstates[] =
    {
        D3DRENDERSTATE_SHADEMODE,
        D3DRENDERSTATE_SPECULARENABLE,
        D3DRENDERSTATE_CULLMODE,
        D3DRENDERSTATE_FOGENABLE,
        D3DRENDERSTATE_FOGCOLOR,
        D3DRENDERSTATE_FOGTABLEMODE,
        D3DRENDERSTATE_FOGSTART,
        D3DRENDERSTATE_FOGEND,
        D3DRENDERSTATE_FOGDENSITY,
        D3DRENDERSTATE_RANGEFOGENABLE,
        D3DRENDERSTATE_AMBIENT,
        D3DRENDERSTATE_COLORVERTEX,
        D3DRENDERSTATE_FOGVERTEXMODE,
        D3DRENDERSTATE_CLIPPING,
        D3DRENDERSTATE_LIGHTING,
        D3DRENDERSTATE_NORMALIZENORMALS,
        D3DRENDERSTATE_LOCALVIEWER,
        D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,
        D3DRENDERSTATE_AMBIENTMATERIALSOURCE,
        D3DRENDERSTATE_DIFFUSEMATERIALSOURCE,
        D3DRENDERSTATE_SPECULARMATERIALSOURCE,
        D3DRENDERSTATE_VERTEXBLEND,
        D3DRENDERSTATE_CLIPPLANEENABLE,
        D3DRS_SOFTWAREVERTEXPROCESSING,
        D3DRS_POINTSIZE,
        D3DRS_POINTSIZE_MIN,
        D3DRS_POINTSPRITEENABLE,
        D3DRS_POINTSCALEENABLE,
        D3DRS_POINTSCALE_A,
        D3DRS_POINTSCALE_B,
        D3DRS_POINTSCALE_C,
        D3DRS_MULTISAMPLEANTIALIAS,
        D3DRS_MULTISAMPLEMASK,
        D3DRS_PATCHEDGESTYLE,
        D3DRS_PATCHSEGMENTS,
        D3DRS_POINTSIZE_MAX,
        D3DRS_INDEXEDVERTEXBLENDENABLE,
        D3DRS_TWEENFACTOR,
    };
    static D3DTEXTURESTAGESTATETYPE tsstates[] =
    {
        D3DTSS_TEXCOORDINDEX,
        D3DTSS_TEXTURETRANSFORMFLAGS
    };

    recordRenderStates       (pContext, rstates, sizeof(rstates) / sizeof(D3DRENDERSTATETYPE));
    recordTextureStageStates (pContext, tsstates, sizeof(tsstates) / sizeof(D3DTEXTURESTAGESTATETYPE));
    recordLights             (pContext);
    recordVShader            (pContext);

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT stateSetBegin (PNVD3DCONTEXT pContext, DWORD dwHandle)
{
    STATESET *pss;

    // grow state set handle list if the given one exceeds our maximum
    if (dwHandle >= pContext->dwMaxStateSetHandles)
    {
        // grow
        DWORD     dwCount = (dwHandle + 1 + 15) & ~15;
        STATESET *pNew    = (STATESET*)AllocIPM(sizeof(STATESET) * dwCount);
        if (!pNew) {
            DPF ("stateSetBegin: out of memory to allow more handles");
            dbgD3DError();
            return (DDERR_GENERIC);
        }
        if (pContext->pStateSets) {
            nvMemCopy (pNew,pContext->pStateSets,sizeof(STATESET) * pContext->dwMaxStateSetHandles);
        }
        memset (pNew + pContext->dwMaxStateSetHandles, 0,sizeof(STATESET) * (dwCount - pContext->dwMaxStateSetHandles));
        if (pContext->pStateSets) {
            FreeIPM (pContext->pStateSets);
        }
        pContext->pStateSets           = pNew;
        pContext->dwMaxStateSetHandles = dwCount;
    }

    // get the pointer to the stateset
    pss = pContext->pStateSets + dwHandle;
    if (pss == NULL) {
        DPF ("Could not allocate memory for stateset");
        return (DDERR_OUTOFMEMORY);
    }

    // allocate memory for the data
    pss->pData = (LPBYTE) new BYTE[DEFAULT_STATESET_SIZE];
    if (pss->pData == NULL) {
        DPF ("Could not allocate memory for stateset data");
        return (DDERR_OUTOFMEMORY);
    }

    // initialize contents
    pss->allocatedSize = DEFAULT_STATESET_SIZE;
    pss->dataSize = 0;

    // store current stateset
    pContext->dwCurrentStateSet = dwHandle;

    // switch to record mode
    pContext->pDP2FunctionTable = &nvDP2RecordFuncs;

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT stateSetEnd (PNVD3DCONTEXT pContext)
{
    // switch to execute mode
    pContext->pDP2FunctionTable = &nvDP2SetFuncs;
    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT stateSetExecute (PNVD3DCONTEXT pContext, DWORD dwHandle)
{

    LPD3DHAL_DP2COMMAND pCommands;
    STATESET *pss;
    LPBYTE    pCommandBufferEnd;

    if (dwHandle >= pContext->dwMaxStateSetHandles) {
        DPF ("Stateset index exceeded MAXSTATESETS");
        dbgD3DError();
        return (DDERR_GENERIC);
    }

    pss = pContext->pStateSets + dwHandle;
    pCommands = (LPD3DHAL_DP2COMMAND)(pss->pData);
    pCommandBufferEnd = (LPBYTE)pCommands + pss->dataSize;

    // parse the commands
    while ((LPBYTE)pCommands < pCommandBufferEnd) {

        dbgDisplayDrawPrimitives2Info(pCommands->bCommand, pCommands->wPrimitiveCount);

        if (nvDP2SetFuncs[pCommands->bCommand] != NULL) {
            (nvDP2SetFuncs[pCommands->bCommand]) (pContext,
                                                  &pCommands,
                                                  pCommandBufferEnd,
                                                  NULL,
                                                  0);
#ifdef DEBUG
            DWORD *p = (DWORD*)pCommands;
            if (p[0] == 0xabcd1234)
            {
                DPF_LEVEL (NVDBG_LEVEL_INFO,"StateSet<stateSetExecute> Serial %d parsed succesfully",p[1]);
                pCommands = (LPD3DHAL_DP2COMMAND)(p + 2);
            }
            else
            {
                DPF ("StateSet debug marker not found");
                dbgD3DError();
            }
#endif
        }
        else {
            DPF ("got unhandled command in stateset (%d)",pCommands->bCommand);
            dbgD3DError();
            break;
        }

    }  // while

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT stateSetCapture (PNVD3DCONTEXT pContext, DWORD dwHandle)
{
    LPD3DHAL_DP2COMMAND pCommands;
    STATESET *pss;
    LPBYTE    pCommandBufferEnd;

    if (dwHandle >= pContext->dwMaxStateSetHandles) {
        DPF ("Stateset index exceeded MAXSTATESETS");
        dbgD3DError();
        return (DDERR_GENERIC);
    }

    pss = pContext->pStateSets + dwHandle;
    pCommands = (LPD3DHAL_DP2COMMAND)(pss->pData);
    pCommandBufferEnd = (LPBYTE)pCommands + pss->dataSize;

    // parse the commands
    while ((LPBYTE)pCommands < pCommandBufferEnd) {

        dbgDisplayDrawPrimitives2Info(pCommands->bCommand, pCommands->wPrimitiveCount);

        if (nvDP2CaptureFuncs[pCommands->bCommand] != NULL) {
            (nvDP2CaptureFuncs[pCommands->bCommand]) (pContext,
                                                      &pCommands,
                                                      pCommandBufferEnd,
                                                      NULL,
                                                      0);
#ifdef DEBUG
            DWORD *p = (DWORD*)pCommands;
            if (p[0] == 0xabcd1234)
            {
                DPF_LEVEL (NVDBG_LEVEL_INFO,"StateSet<stateSetCapture> Serial %d parsed succesfully",p[1]);
                pCommands = (LPD3DHAL_DP2COMMAND)(p + 2);
            }
            else
            {
                DPF ("StateSet debug marker not found");
                dbgD3DError();
            }
#endif
        }
        else {
            DPF ("got unhandled command in stateset (%d)",pCommands->bCommand);
            dbgD3DError();
            break;
        }

    }  // while

    return (D3D_OK);
}

//---------------------------------------------------------------------------

HRESULT stateSetCreate (PNVD3DCONTEXT pContext, DWORD dwHandle, D3DSTATEBLOCKTYPE sbType)
{
    HRESULT hr;

    stateSetBegin (pContext, dwHandle);

    switch (sbType) {
        case D3DSBT_ALL:
            hr = recordAllState (pContext);
            break;
        case D3DSBT_PIXELSTATE:
            hr = recordPixelState (pContext);
            break;
        case D3DSBT_VERTEXSTATE:
            hr = recordVertexState (pContext);
            break;
        default:
            DPF ("unknown stateset type");
            hr = DDERR_GENERIC;
            break;
    }  // switch

    stateSetEnd (pContext);

    return (hr);
}

//---------------------------------------------------------------------------

HRESULT stateSetDelete (PNVD3DCONTEXT pContext, DWORD dwHandle)
{
    STATESET *pss;

    if (dwHandle >= pContext->dwMaxStateSetHandles) {
        DPF ("Stateset index exceeded MAXSTATESETS");
        dbgD3DError();
        return (DDERR_GENERIC);
    }

    pss = pContext->pStateSets + dwHandle;

    if (pss != NULL) {
        if (pss->pData)
        {
            delete []pss->pData;
        }
#ifdef DEBUG
        memset (pss,0,sizeof(STATESET));
#else
        pss->pData = NULL;
#endif
    }

    return (D3D_OK);
}

#endif  // NVARCH >= 0x04

