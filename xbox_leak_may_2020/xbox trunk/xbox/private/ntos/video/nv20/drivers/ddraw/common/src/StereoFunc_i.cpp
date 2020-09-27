/***************************************************************************\
* Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
*  Portions: Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.*
*                                                                           *
* Module: StereoFunc_i.cpp                                                  *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Andrei Osnovich    02/23/00  Created                                *
*                                                                           *
\***************************************************************************/

#ifndef STEREOFUNC_GENERAL_MACROS
#define STEREOFUNC_GENERAL_MACROS
#define EQUAL       0
#define GREATER     1
#define LESS        2
#define X(pV)       (((LPD3DVALUE)pV)[0])
#define Y(pV)       (((LPD3DVALUE)pV)[1])
#define Z(pV)       (((LPD3DVALUE)pV)[2])
#define Z_I(pV)     (((DWORD *)pV)[2])
#define RHW(pV)     (((LPD3DVALUE)pV)[3])
#define RHW_I(pV)   (((DWORD *)pV)[3])
#define GET_REAL_NAME(token) #token
#endif  //STEREOFUNC_GENERAL_MACROS

#ifdef  FAN
    #define FAN_OR_STRIP
#else
#ifdef  STRIP
    #define FAN_OR_STRIP
#endif
#endif

#ifdef  INDEX2
#define INDEX
#endif  //INDEX2

#ifdef  INDEX
#ifdef  FAN_OR_STRIP
#define DEFINE_LOCALS   \
    WORD        wVertNum1; \
    LPBYTE      pVertex1;
#define FOR_ALL_VERTICES \
    for (i = 0; i < dwCount; i++, pIndices += sizeof(WORD))
#define GET_V1_POINTER \
    wVertNum1 = ((WORD *)pIndices)[0];          \
    nvAssert(wVertNum1 < pStereoData->dwVertexRegisterSize); \
    pVertex1 = &pVertices[wVertNum1*pContext->pCurrentVShader->getStride()];
#else   //FAN_OR_STRIP==0
#ifdef  LINE
#define DEFINE_LOCALS   \
    WORD        wVertNum1, wVertNum2; \
    LPBYTE      pVertex1, pVertex2;
#else   //LINE==0
#define DEFINE_LOCALS   \
    WORD        wVertNum1, wVertNum2, wVertNum3; \
    LPBYTE      pVertex1, pVertex2, pVertex3;
#endif  //LINE
#ifdef  INDEX2
#define FOR_ALL_VERTICES \
    for (i = 0; i < dwCount; i++, pIndices += sizeof(D3DHAL_DP2INDEXEDTRIANGLELIST2))
#define GET_V1_POINTER \
    wVertNum1 = ((LPD3DHAL_DP2INDEXEDTRIANGLELIST2)pIndices)->wV1;          \
    nvAssert(wVertNum1 < pStereoData->dwVertexRegisterSize); \
    pVertex1 = &pVertices[wVertNum1*pContext->pCurrentVShader->getStride()];
#define GET_V2_POINTER \
    wVertNum2 = ((LPD3DHAL_DP2INDEXEDTRIANGLELIST2)pIndices)->wV2;          \
    nvAssert(wVertNum2 < pStereoData->dwVertexRegisterSize); \
    pVertex2 = &pVertices[wVertNum2*pContext->pCurrentVShader->getStride()];
#define GET_V3_POINTER \
    wVertNum3 = ((LPD3DHAL_DP2INDEXEDTRIANGLELIST2)pIndices)->wV3;          \
    nvAssert(wVertNum3 < pStereoData->dwVertexRegisterSize); \
    pVertex3 = &pVertices[wVertNum3*pContext->pCurrentVShader->getStride()];
#else   //INDEX2==0
#ifdef  LINE
#define FOR_ALL_VERTICES \
    for (i = 0; i < dwCount; i++, pIndices += sizeof(D3DHAL_DP2INDEXEDLINELIST))
#define GET_V1_POINTER \
    wVertNum1 = ((LPD3DHAL_DP2INDEXEDLINELIST)pIndices)->wV1;           \
    nvAssert(wVertNum1 < pStereoData->dwVertexRegisterSize); \
    pVertex1 = &pVertices[wVertNum1*pContext->pCurrentVShader->getStride()];
#define GET_V2_POINTER \
    wVertNum2 = ((LPD3DHAL_DP2INDEXEDLINELIST)pIndices)->wV2;           \
    nvAssert(wVertNum2 < pStereoData->dwVertexRegisterSize); \
    pVertex2 = &pVertices[wVertNum2*pContext->pCurrentVShader->getStride()];
#else   //LINE==0
#define FOR_ALL_VERTICES \
    for (i = 0; i < dwCount; i++, pIndices += sizeof(D3DHAL_DP2INDEXEDTRIANGLELIST))
#define GET_V1_POINTER \
    wVertNum1 = ((LPD3DHAL_DP2INDEXEDTRIANGLELIST)pIndices)->wV1;           \
    nvAssert(wVertNum1 < pStereoData->dwVertexRegisterSize); \
    pVertex1 = &pVertices[wVertNum1*pContext->pCurrentVShader->getStride()];
#define GET_V2_POINTER \
    wVertNum2 = ((LPD3DHAL_DP2INDEXEDTRIANGLELIST)pIndices)->wV2;           \
    nvAssert(wVertNum2 < pStereoData->dwVertexRegisterSize); \
    pVertex2 = &pVertices[wVertNum2*pContext->pCurrentVShader->getStride()];
#define GET_V3_POINTER \
    wVertNum3 = ((LPD3DHAL_DP2INDEXEDTRIANGLELIST)pIndices)->wV3;           \
    nvAssert(wVertNum3 < pStereoData->dwVertexRegisterSize); \
    pVertex3 = &pVertices[wVertNum3*pContext->pCurrentVShader->getStride()];
#endif  //LINE
#endif  //INDEX2
#endif  //FAN_OR_STRIP
#else   //INDEX=0
#define DEFINE_LOCALS
#define FOR_ALL_VERTICES \
    for (i = 0; i < dwCount; i++, pVertices += pContext->pCurrentVShader->getStride())
#endif  //INDEX

BOOL
CHECK_RHW_CONDITION (
        PNVD3DCONTEXT pContext,
        LPBYTE pVertices,
    #ifdef INDEX
        LPBYTE pIndices,
    #endif //INDEX
        DWORD dwCount,
        DWORD dwCondition)
{
    DWORD       i;
    D3DVALUE    fRHW, fZ;
#ifdef  INDEX
    DEFINE_LOCALS;
#ifdef  FAN_OR_STRIP
    dwCount += 2;
#endif  //FAN_OR_STRIP
#else   //INDEX=0
#ifdef  FAN_OR_STRIP
#ifdef  LINE
    dwCount += 1;
#else   //LINE==0 (TRIANGLE)
    dwCount += 2;
#endif  //LINE
#else   //FAN_OR_STRIP=0
#ifndef POINT
#ifdef  LINE
    dwCount *= 2;
#else   //LINE==0 (TRIANGLE)
    dwCount *= 3;
#endif  //LINE
#endif  //POINT
#endif  //FAN_OR_STRIP
#endif  //INDEX
    switch (dwCondition)
    {
#ifdef  STEREO_CONFIG_ASSIST
    case EQUAL:
        if (pStereoData->StereoSettings.dwFlags & STEREO_FAVORSZOVERRHW)
        {
        //Check if the farthest 2D object is farther than the closest 3D object.
        //If so we need to reinitialize the farthest 2D object.
        if (ConfigAssistInfo.szMax2D > ConfigAssistInfo.szMin)
                ConfigAssistInfo.szMax2D = -9999.f;
        //Check if the closest 2D object is closer than the farthest 3D object.
        //If so we need to reinitialize the closest 2D object.
        if (ConfigAssistInfo.szMin2D < ConfigAssistInfo.szMax)
            ConfigAssistInfo.szMin2D = 9999.f;
            DWORD dwZ;
            BOOL  bForce2D;
            bForce2D = FALSE;
#ifdef  INDEX
            GET_V1_POINTER;
            dwZ = Z_I(pVertex1);
#else   //INDEX=0
            dwZ = Z_I(pVertices);
#endif  //INDEX
            if (pStereoData->StereoSettings.dwFlags & STEREO_RHWEQUALATSCREEN)
            {
#if 0
                if (*(LPD3DVALUE)&dwZ >= pStereoData->fRHW2DDetectionMin)
                    bForce2D = TRUE;
#endif
                if (*(LPD3DVALUE)&dwZ > ConfigAssistInfo.szMin
                    && *(LPD3DVALUE)&dwZ < ConfigAssistInfo.szMax) //This object is inside the 3D object range.
                    bForce2D = TRUE;
            } else
            {
#if 0
                if (*(LPD3DVALUE)&dwZ >= pStereoData->fRHW2DDetectionMin)
                    return FALSE;
#endif
                if (*(LPD3DVALUE)&dwZ > ConfigAssistInfo.szMin
                    && *(LPD3DVALUE)&dwZ < ConfigAssistInfo.szMax) //This object is inside the 3D object range.
                    return FALSE;
            }
            FOR_ALL_VERTICES {
#ifdef  INDEX
                GET_V1_POINTER;
                if (Z_I(pVertex1) != dwZ)
                    return FALSE;
#ifndef FAN_OR_STRIP
                GET_V2_POINTER;
                if (Z_I(pVertex2) != dwZ)
                    return FALSE;
#ifndef LINE
                GET_V3_POINTER;
                if (Z_I(pVertex3) != dwZ)
                    return FALSE;
#endif  //LINE
#endif  //FAN_OR_STRIP
#else   //INDEX=0
                if (Z_I(pVertices) != dwZ)
                    return FALSE;
#endif  //INDEX
            }
            if (!bForce2D)
            {
                if (ConfigAssistInfo.szMin > *(LPD3DVALUE)&dwZ)
                {
                    //Closer than all 3D
                    if (ConfigAssistInfo.szMax2D < *(LPD3DVALUE)&dwZ)
                        ConfigAssistInfo.szMax2D = *(LPD3DVALUE)&dwZ;
                } else
                {
                    //Farther than all 3D
                    if (ConfigAssistInfo.szMin2D > *(LPD3DVALUE)&dwZ)
                        ConfigAssistInfo.szMin2D = *(LPD3DVALUE)&dwZ;
                }
            }
#ifdef  FLAT_STAT
            RegisterFlatObject(*(LPD3DVALUE)&dwZ, dwCount);
#endif  //FLAT_STAT
            return TRUE;
        }
        //Check if the farthest 2D object is farther than the closest 3D object.
        //If so we need to reinitialize the farthest 2D object.
        if (ConfigAssistInfo.rhwMin2D < ConfigAssistInfo.rhwMax)
            ConfigAssistInfo.rhwMin2D = 9999.f;
        //Check if the closest 2D object is closer than the farthest 3D object.
        //If so we need to reinitialize the closest 2D object.
        if (ConfigAssistInfo.rhwMax2D > ConfigAssistInfo.rhwMin)
            ConfigAssistInfo.rhwMax2D = -9999.f;
        DWORD dwRHW;
        BOOL  bForce2D;
        bForce2D = FALSE;
#ifdef  INDEX
        GET_V1_POINTER;
        dwRHW = RHW_I(pVertex1);
#else   //INDEX=0
        dwRHW = RHW_I(pVertices);
#endif  //INDEX
        if (pStereoData->StereoSettings.dwFlags & STEREO_RHWEQUALATSCREEN)
        {
            if (*(LPD3DVALUE)&dwRHW <= pStereoData->fRHW2DDetectionMin)
                bForce2D = TRUE;
            if (*(LPD3DVALUE)&dwRHW < ConfigAssistInfo.rhwMax
                && *(LPD3DVALUE)&dwRHW > ConfigAssistInfo.rhwMin) //This object is inside the 3D object range.
                bForce2D = TRUE;
        } else
        {
            if (*(LPD3DVALUE)&dwRHW <= pStereoData->fRHW2DDetectionMin)
                return FALSE;
            if (*(LPD3DVALUE)&dwRHW < ConfigAssistInfo.rhwMax
                && *(LPD3DVALUE)&dwRHW > ConfigAssistInfo.rhwMin) //This object is inside the 3D object range.
                return FALSE;
        }
        FOR_ALL_VERTICES {
#ifdef  INDEX
            GET_V1_POINTER;
            if (RHW_I(pVertex1) != dwRHW)
                return FALSE;
#ifndef FAN_OR_STRIP
            GET_V2_POINTER;
            if (RHW_I(pVertex2) != dwRHW)
                return FALSE;
#ifndef LINE
            GET_V3_POINTER;
            if (RHW_I(pVertex3) != dwRHW)
                return FALSE;
#endif  //LINE
#endif  //FAN_OR_STRIP
#else   //INDEX=0
            if (RHW_I(pVertices) != dwRHW)
                return FALSE;
#endif  //INDEX
        }
        if (!bForce2D)
        {
            if (ConfigAssistInfo.rhwMax < *(LPD3DVALUE)&dwRHW)
            {
                //Closer than all 3D
                if (ConfigAssistInfo.rhwMin2D > *(LPD3DVALUE)&dwRHW)
                    ConfigAssistInfo.rhwMin2D = *(LPD3DVALUE)&dwRHW;
            } else
            {
                //Farther than all 3D
                if (ConfigAssistInfo.rhwMax2D < *(LPD3DVALUE)&dwRHW)
                    ConfigAssistInfo.rhwMax2D = *(LPD3DVALUE)&dwRHW;
            }
        }
#ifdef  FLAT_STAT
        RegisterFlatObject(*(LPD3DVALUE)&dwRHW, dwCount);
#endif  //FLAT_STAT
        return TRUE;
#endif  //STEREO_CONFIG_ASSIST

    case GREATER:
        if (pStereoData->StereoSettings.dwFlags & STEREO_FAVORSZOVERRHW)
        {
            fZ = pStereoData->StereoSettings.fRHWGreaterAtScreen;
            FOR_ALL_VERTICES {
#ifdef  INDEX
                GET_V1_POINTER;
                if (Z(pVertex1) > fZ)
                    return FALSE;
#ifndef FAN_OR_STRIP
                GET_V2_POINTER;
                if (Z(pVertex2) > fZ)
                    return FALSE;
#ifndef LINE
                GET_V3_POINTER;
                if (Z(pVertex3) > fZ)
                    return FALSE;
#endif  //LINE
#endif  //FAN_OR_STRIP
#else   //INDEX=0
                if (Z(pVertices) > fZ)
                    return FALSE;
#endif  //INDEX
            }
            return TRUE;
        }
        fRHW = pStereoData->StereoSettings.fRHWGreaterAtScreen;
        FOR_ALL_VERTICES {
#ifdef  INDEX
            GET_V1_POINTER;
            if (RHW(pVertex1) < fRHW)
                return FALSE;
#ifndef FAN_OR_STRIP
            GET_V2_POINTER;
            if (RHW(pVertex2) < fRHW)
                return FALSE;
#ifndef LINE
            GET_V3_POINTER;
            if (RHW(pVertex3) < fRHW)
                return FALSE;
#endif  //LINE
#endif  //FAN_OR_STRIP
#else   //INDEX=0
            if (RHW(pVertices) < fRHW)
                return FALSE;
#endif  //INDEX
        }
        return TRUE;

    case LESS:
        if (pStereoData->StereoSettings.dwFlags & STEREO_FAVORSZOVERRHW)
        {
            fZ = pStereoData->StereoSettings.fRHWLessAtScreen;
            FOR_ALL_VERTICES {
#ifdef  INDEX
                GET_V1_POINTER;
                if (Z(pVertex1) < fZ)
                    return FALSE;
#ifndef FAN_OR_STRIP
                GET_V2_POINTER;
                if (Z(pVertex2) < fZ)
                    return FALSE;
#ifndef LINE
                GET_V3_POINTER;
                if (Z(pVertex3) < fZ)
                    return FALSE;
#endif  //LINE
#endif  //FAN_OR_STRIP
#else   //INDEX=0
                if (Z(pVertices) < fZ)
                    return FALSE;
#endif  //INDEX
            }
            return TRUE;
        }
        //Some games like Triple Play 2001 put pop up 2D menus as
        //farthest objects in the scene. Stupid but we have to handle it.
        fRHW = pStereoData->StereoSettings.fRHWLessAtScreen;
        FOR_ALL_VERTICES {
#ifdef  INDEX
            GET_V1_POINTER;
            if (RHW(pVertex1) > fRHW)
                return FALSE;
#ifndef FAN_OR_STRIP
            GET_V2_POINTER;
            if (RHW(pVertex2) > fRHW)
                return FALSE;
#ifndef LINE
            GET_V3_POINTER;
            if (RHW(pVertex3) > fRHW)
                return FALSE;
#endif  //LINE
#endif  //FAN_OR_STRIP
#else   //INDEX=0
            if (RHW(pVertices) > fRHW)
                return FALSE;
#endif  //INDEX
        }
        return TRUE;

    default:
        LOG("%s: unsupported case", GET_REAL_NAME(CHECK_RHW_CONDITION) );
		nvAssert(0);
        break;
    }
    return FALSE;
}

DWORD
STEREO_EYE_FUNCTION_FLIP (
        PNVD3DCONTEXT pContext,
        LPBYTE pVertices,
    #ifdef INDEX
        LPBYTE pIndices,
    #endif //INDEX
        DWORD dwCount)
{
    DWORD       i;
    DWORD       res = 0;
    D3DVALUE    XCorrection;
    DEFINE_LOCALS;

    nvAssert(pStereoData->pVertexRegister);
    if (pStereoData->pVertexRegister->DoAutomaticStage_Flip())
    {
        //Everything has been done automatically.
        return 0;
    }
    //We're required to fill out the stage for the first eye.
#ifdef  INDEX
    pStereoData->pVertexRegister->cleanupInUse();
#ifdef  FAN_OR_STRIP
    dwCount += 2;
#endif  //FAN_OR_STRIP
#else   //INDEX=0
#ifdef  FAN_OR_STRIP
#ifdef  LINE
    dwCount += 1;
#else   //LINE==0 (TRIANGLE)
    dwCount += 2;
#endif  //LINE
#else   //FAN_OR_STRIP=0
#ifndef POINT
#ifdef  LINE
    dwCount *= 2;
#else   //LINE==0 (TRIANGLE)
    dwCount *= 3;
#endif  //LINE
#endif  //POINT
#endif  //FAN_OR_STRIP
#endif  //INDEX

    if (pStereoData->dwLastEye == EYE_LEFT)
    {
        //We're doing the left eye first
        FOR_ALL_VERTICES {
#ifdef  INDEX
            GET_V1_POINTER;
            if (!pStereoData->pVertexRegister->isInUseTag(wVertNum1))
            {
                //Unique vertex.
                pStereoData->pVertexRegister->SaveX((LPD3DVALUE)pVertex1);
                XCorrection = pStereoData->StereoSettings.fScreenStereoSeparation*(1.0f - RHW(pVertex1)*pStereoData->StereoSettings.fPostStereoConvergence);
                pStereoData->pVertexRegister->SaveXCorrection_Flip(XCorrection);
                X(pVertex1) = X(pVertex1) * pStereoData->StereoSettings.fPostStereoXStretch - pStereoData->StereoSettings.fPostStereoXShift - XCorrection;
#ifdef  STEREO_CONFIG_ASSIST
                if (!(ConfigAssistInfo.dwFlags & _2DSUSPECT))
                {
#endif  //STEREO_CONFIG_ASSIST
                    if (pStereoData->fFrameRHWMax < RHW(pVertex1))
                        pStereoData->fFrameRHWMax = RHW(pVertex1);
#ifdef  STEREO_CONFIG_ASSIST
                    if (ConfigAssistInfo.rhwMin > RHW(pVertex1))
                        ConfigAssistInfo.rhwMin = RHW(pVertex1);
                    if (ConfigAssistInfo.szMin > Z(pVertex1))
                        ConfigAssistInfo.szMin = Z(pVertex1);
                    if (ConfigAssistInfo.szMax < Z(pVertex1))
                        ConfigAssistInfo.szMax = Z(pVertex1);
                }
#endif  //STEREO_CONFIG_ASSIST
            }
#ifndef FAN_OR_STRIP
            GET_V2_POINTER;
            if (!pStereoData->pVertexRegister->isInUseTag(wVertNum2))
            {
                //Unique vertex.
                pStereoData->pVertexRegister->SaveX((LPD3DVALUE)pVertex2);
                XCorrection = pStereoData->StereoSettings.fScreenStereoSeparation*(1.0f - RHW(pVertex2)*pStereoData->StereoSettings.fPostStereoConvergence);
                pStereoData->pVertexRegister->SaveXCorrection_Flip(XCorrection);
                X(pVertex2) = X(pVertex2) * pStereoData->StereoSettings.fPostStereoXStretch - pStereoData->StereoSettings.fPostStereoXShift - XCorrection;
#ifdef  STEREO_CONFIG_ASSIST
                if (!(ConfigAssistInfo.dwFlags & _2DSUSPECT))
                {
#endif  //STEREO_CONFIG_ASSIST
                    if (pStereoData->fFrameRHWMax < RHW(pVertex2))
                        pStereoData->fFrameRHWMax = RHW(pVertex2);
#ifdef  STEREO_CONFIG_ASSIST
                    if (ConfigAssistInfo.rhwMin > RHW(pVertex2))
                        ConfigAssistInfo.rhwMin = RHW(pVertex2);
                    if (ConfigAssistInfo.szMin > Z(pVertex2))
                        ConfigAssistInfo.szMin = Z(pVertex2);
                    if (ConfigAssistInfo.szMax < Z(pVertex2))
                        ConfigAssistInfo.szMax = Z(pVertex2);
                }
#endif  //STEREO_CONFIG_ASSIST
            }
#ifndef LINE
            GET_V3_POINTER;
            if (!pStereoData->pVertexRegister->isInUseTag(wVertNum3))
            {
                //Unique vertex.
                pStereoData->pVertexRegister->SaveX((LPD3DVALUE)pVertex3);
                XCorrection = pStereoData->StereoSettings.fScreenStereoSeparation*(1.0f - RHW(pVertex3)*pStereoData->StereoSettings.fPostStereoConvergence);
                pStereoData->pVertexRegister->SaveXCorrection_Flip(XCorrection);
                X(pVertex3) = X(pVertex3) * pStereoData->StereoSettings.fPostStereoXStretch - pStereoData->StereoSettings.fPostStereoXShift - XCorrection;
#ifdef  STEREO_CONFIG_ASSIST
                if (!(ConfigAssistInfo.dwFlags & _2DSUSPECT))
                {
#endif  //STEREO_CONFIG_ASSIST
                    if (pStereoData->fFrameRHWMax < RHW(pVertex3))
                        pStereoData->fFrameRHWMax = RHW(pVertex3);
#ifdef  STEREO_CONFIG_ASSIST
                    if (ConfigAssistInfo.rhwMin > RHW(pVertex3))
                        ConfigAssistInfo.rhwMin = RHW(pVertex3);
                    if (ConfigAssistInfo.szMin > Z(pVertex3))
                        ConfigAssistInfo.szMin = Z(pVertex3);
                    if (ConfigAssistInfo.szMax < Z(pVertex3))
                        ConfigAssistInfo.szMax = Z(pVertex3);
                }
#endif  //STEREO_CONFIG_ASSIST
            }
#endif  //LINE
#endif  //FAN_OR_STRIP
#else   //INDEX=0
            pStereoData->pVertexRegister->SaveX((LPD3DVALUE)pVertices);
            XCorrection = pStereoData->StereoSettings.fScreenStereoSeparation*(1.0f - RHW(pVertices)*pStereoData->StereoSettings.fPostStereoConvergence);
            pStereoData->pVertexRegister->SaveXCorrection_Flip(XCorrection);
            X(pVertices) = X(pVertices) * pStereoData->StereoSettings.fPostStereoXStretch - pStereoData->StereoSettings.fPostStereoXShift - XCorrection;
#ifdef  STEREO_CONFIG_ASSIST
            if (!(ConfigAssistInfo.dwFlags & _2DSUSPECT))
            {
#endif  //STEREO_CONFIG_ASSIST
                if (pStereoData->fFrameRHWMax < RHW(pVertices))
                    pStereoData->fFrameRHWMax = RHW(pVertices);
#ifdef  STEREO_CONFIG_ASSIST
                if (ConfigAssistInfo.rhwMin > RHW(pVertices))
                    ConfigAssistInfo.rhwMin = RHW(pVertices);
                if (ConfigAssistInfo.szMin > Z(pVertices))
                    ConfigAssistInfo.szMin = Z(pVertices);
                if (ConfigAssistInfo.szMax < Z(pVertices))
                    ConfigAssistInfo.szMax = Z(pVertices);
            }
#endif  //STEREO_CONFIG_ASSIST
#endif  //INDEX
        }
    } else
    {
        //We're doing the right eye first
        FOR_ALL_VERTICES {
#ifdef  INDEX
            GET_V1_POINTER;
            if (!pStereoData->pVertexRegister->isInUseTag(wVertNum1))
            {
                //Unique vertex.
                pStereoData->pVertexRegister->SaveX((LPD3DVALUE)pVertex1);
                XCorrection = pStereoData->StereoSettings.fScreenStereoSeparation*(1.0f - RHW(pVertex1)*pStereoData->StereoSettings.fPostStereoConvergence);
                pStereoData->pVertexRegister->SaveXCorrection_Flip(XCorrection);
                X(pVertex1) = X(pVertex1) * pStereoData->StereoSettings.fPostStereoXStretch - pStereoData->StereoSettings.fPostStereoXShift + XCorrection;
#ifdef  STEREO_CONFIG_ASSIST
                if (!(ConfigAssistInfo.dwFlags & _2DSUSPECT))
                {
#endif  //STEREO_CONFIG_ASSIST
                    if (pStereoData->fFrameRHWMax < RHW(pVertex1))
                        pStereoData->fFrameRHWMax = RHW(pVertex1);
#ifdef  STEREO_CONFIG_ASSIST
                    if (ConfigAssistInfo.rhwMin > RHW(pVertex1))
                        ConfigAssistInfo.rhwMin = RHW(pVertex1);
                    if (ConfigAssistInfo.szMin > Z(pVertex1))
                        ConfigAssistInfo.szMin = Z(pVertex1);
                    if (ConfigAssistInfo.szMax < Z(pVertex1))
                        ConfigAssistInfo.szMax = Z(pVertex1);
                }
#endif  //STEREO_CONFIG_ASSIST
            }
#ifndef FAN_OR_STRIP
            GET_V2_POINTER;
            if (!pStereoData->pVertexRegister->isInUseTag(wVertNum2))
            {
                //Unique vertex.
                pStereoData->pVertexRegister->SaveX((LPD3DVALUE)pVertex2);
                XCorrection = pStereoData->StereoSettings.fScreenStereoSeparation*(1.0f - RHW(pVertex2)*pStereoData->StereoSettings.fPostStereoConvergence);
                pStereoData->pVertexRegister->SaveXCorrection_Flip(XCorrection);
                X(pVertex2) = X(pVertex2) * pStereoData->StereoSettings.fPostStereoXStretch - pStereoData->StereoSettings.fPostStereoXShift + XCorrection;
#ifdef  STEREO_CONFIG_ASSIST
                if (!(ConfigAssistInfo.dwFlags & _2DSUSPECT))
                {
#endif  //STEREO_CONFIG_ASSIST
                    if (pStereoData->fFrameRHWMax < RHW(pVertex2))
                        pStereoData->fFrameRHWMax = RHW(pVertex2);
#ifdef  STEREO_CONFIG_ASSIST
                    if (ConfigAssistInfo.rhwMin > RHW(pVertex2))
                        ConfigAssistInfo.rhwMin = RHW(pVertex2);
                    if (ConfigAssistInfo.szMin > Z(pVertex2))
                        ConfigAssistInfo.szMin = Z(pVertex2);
                    if (ConfigAssistInfo.szMax < Z(pVertex2))
                        ConfigAssistInfo.szMax = Z(pVertex2);
                }
#endif  //STEREO_CONFIG_ASSIST
            }
#ifndef LINE
            GET_V3_POINTER;
            if (!pStereoData->pVertexRegister->isInUseTag(wVertNum3))
            {
                //Unique vertex.
                pStereoData->pVertexRegister->SaveX((LPD3DVALUE)pVertex3);
                XCorrection = pStereoData->StereoSettings.fScreenStereoSeparation*(1.0f - RHW(pVertex3)*pStereoData->StereoSettings.fPostStereoConvergence);
                pStereoData->pVertexRegister->SaveXCorrection_Flip(XCorrection);
                X(pVertex3) = X(pVertex3) * pStereoData->StereoSettings.fPostStereoXStretch - pStereoData->StereoSettings.fPostStereoXShift + XCorrection;
#ifdef  STEREO_CONFIG_ASSIST
                if (!(ConfigAssistInfo.dwFlags & _2DSUSPECT))
                {
#endif  //STEREO_CONFIG_ASSIST
                    if (pStereoData->fFrameRHWMax < RHW(pVertex3))
                        pStereoData->fFrameRHWMax = RHW(pVertex3);
#ifdef  STEREO_CONFIG_ASSIST
                    if (ConfigAssistInfo.rhwMin > RHW(pVertex3))
                        ConfigAssistInfo.rhwMin = RHW(pVertex3);
                    if (ConfigAssistInfo.szMin > Z(pVertex3))
                        ConfigAssistInfo.szMin = Z(pVertex3);
                    if (ConfigAssistInfo.szMax < Z(pVertex3))
                        ConfigAssistInfo.szMax = Z(pVertex3);
                }
#endif  //STEREO_CONFIG_ASSIST
            }
#endif  //LINE
#endif  //FAN_OR_STRIP
#else   //INDEX=0
            pStereoData->pVertexRegister->SaveX((LPD3DVALUE)pVertices);
            XCorrection = pStereoData->StereoSettings.fScreenStereoSeparation*(1.0f - RHW(pVertices)*pStereoData->StereoSettings.fPostStereoConvergence);
            pStereoData->pVertexRegister->SaveXCorrection_Flip(XCorrection);
            X(pVertices) = X(pVertices) * pStereoData->StereoSettings.fPostStereoXStretch - pStereoData->StereoSettings.fPostStereoXShift + XCorrection;
#ifdef  STEREO_CONFIG_ASSIST
            if (!(ConfigAssistInfo.dwFlags & _2DSUSPECT))
            {
#endif  //STEREO_CONFIG_ASSIST
                if (pStereoData->fFrameRHWMax < RHW(pVertices))
                    pStereoData->fFrameRHWMax = RHW(pVertices);
#ifdef  STEREO_CONFIG_ASSIST
                if (ConfigAssistInfo.rhwMin > RHW(pVertices))
                    ConfigAssistInfo.rhwMin = RHW(pVertices);
                if (ConfigAssistInfo.szMin > Z(pVertices))
                    ConfigAssistInfo.szMin = Z(pVertices);
                if (ConfigAssistInfo.szMax < Z(pVertices))
                    ConfigAssistInfo.szMax = Z(pVertices);
            }
#endif  //STEREO_CONFIG_ASSIST
#endif  //INDEX
        }
    }
    return res;
}

#ifndef	DRAWPRIMITIVE
#ifndef POINT
HRESULT STEREO_DP2_FUNCTION_FLIP (NV_DP2FUNCTION_ARGLIST)
{
    HRESULT             RetVal;
    DWORD               dwDP2Op = (DWORD)((*ppCommands)->bCommand);
    LPD3DHAL_DP2COMMAND pSaveCommandsPtr = *ppCommands;

#ifdef	STEREO_DEBUG
	//Make sure it is not a rendering to a texture
	CNvObject *pNvObj = pContext->pRenderTarget->getWrapper();
	if (pNvObj->getClass() == CNvObject::NVOBJ_TEXTURE)
		__asm int 3
	//Make sure we render to a stereo surface
	if (pContext->pRenderTarget->isStereo() == 0)
		__asm int 3
	//Do we render to the Z buffer?
	if (pContext->pRenderTarget->isZBuffer())
		__asm int 3
#endif	//STEREO_DEBUG

    StereoLockSubstituteFlush();

    // First thing to determine is whether we take a HW T&L or SW T&L route. It breaks
    // down all our code into two vertually mutually exclusive cases. It is better to keep 
    // it this way for optimal performance
    if (!STEREO_R_US(pContext))
    {
        RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                                  ppCommands,
                                                  pCommandBufferEnd,
                                                  pdwDP2RStates,
                                                  dwDP2Flags);
        return RetVal;
    }
    if (pStereoData->dwHWTnL)
    {
        //This case is going to be the most frequently used in future. We want it fast.
        SetupStereoContext(pContext);
        RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                                  ppCommands,
                                                  pCommandBufferEnd,
                                                  pdwDP2RStates,
                                                  dwDP2Flags);
        *ppCommands = pSaveCommandsPtr; //Restore it for the second pass.
        AlternateStereoContext(pContext);
        RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                                  ppCommands,
                                                  pCommandBufferEnd,
                                                  pdwDP2RStates,
                                                  dwDP2Flags);
        return RetVal;
    }
    
    // And now is SW T&L or HW T&L with transformed vertices
    LPBYTE pVertexBufferBase = (LPBYTE)(pContext->ppDX8Streams[0]->getAddress());

#ifdef  PARTIALRENDERING
    if (PrimitiveStat[4] == PrimitiveStat[1])
       nvAssert(0);
#endif  //PARTIALRENDERING

#ifdef  NOT_IMPLEMENTED
    //LOG("%s: not implemented yet",GET_REAL_NAME( STEREO_DP2_FUNCTION_FLIP ));
    nvAssert (0);
    return ((*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                             ppCommands,
                                             pCommandBufferEnd,
                                             pdwDP2RStates,
                                             dwDP2Flags));
#else   //~NOT_IMPLEMENTED
    //nvAssert (0);
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
#ifdef  INDEX
#ifdef  START_VERTEX
    DWORD dwVertexBufferOffset = pContext->dp2.dwVertexBufferOffset;
    DWORD  dwTotalOffset = dwVertexBufferOffset + ((DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwVStart = (DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = dwTotalOffset;
    LPBYTE pVertices     = (LPBYTE)(pVertexBufferBase+dwTotalOffset);
    LPBYTE pIndices      = lpPrim + sizeof(D3DHAL_DP2STARTVERTEX);
#else   //START_VERTEX==0
    DWORD dwVertexBufferOffset = pContext->dp2.dwVertexBufferOffset;
    pContext->dp2.dwVStart = 0;
    pContext->dp2.dwTotalOffset = dwVertexBufferOffset;
    LPBYTE pVertices     = (LPBYTE)(pVertexBufferBase+dwVertexBufferOffset);
    LPBYTE pIndices      = lpPrim;
#endif  //START_VERTEX
    PSTEREOEYEFUNCINDEX pStereoEyeFunc = NULL;
#else   //INDEX==0
#ifdef  FAN_OR_STRIP
#ifdef  START_VERTEX
    DWORD dwVertexBufferOffset = pContext->dp2.dwVertexBufferOffset;
    DWORD  dwTotalOffset = dwVertexBufferOffset + ((DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwVStart = (DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = dwTotalOffset;
    LPBYTE pVertices     = (LPBYTE)(pVertexBufferBase+dwTotalOffset);
#else   //START_VERTEX==0
    LPBYTE pVertices     = lpPrim;
    pContext->dp2.dwVStart = 0;
    pContext->dp2.dwTotalOffset = 0;
#endif  //START_VERTEX
#ifdef  ADVANCE
    pVertices += sizeof(ADVANCE);
#endif  //ADVANCE
#ifdef  ALIGN
    pVertices = (LPBYTE)(((DWORD)pVertices + 3) & ~3);
#endif  //ALIGN
#else   //FAN_OR_STRIP==0
    DWORD dwVertexBufferOffset = pContext->dp2.dwVertexBufferOffset;
    DWORD  dwTotalOffset = dwVertexBufferOffset + ((DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwVStart = (DWORD)((LPD3DHAL_DP2STARTVERTEX)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = dwTotalOffset;
    LPBYTE pVertices     = (LPBYTE)(pVertexBufferBase+dwTotalOffset);
#endif  //FAN_OR_STRIP==0
    PSTEREOEYEFUNC      pStereoEyeFunc = NULL;
#endif  //INDEX

#ifdef  STEREO_CONFIG_ASSIST
    ConfigAssistInfo.dwFlags &= ~_2DSUSPECT;
#endif  //STEREO_CONFIG_ASSIST

    SetupStereoContext(pContext);
    if (NV_VERTEX_TRANSFORMED(pContext->pCurrentVShader))
    {
        pStereoEyeFunc = STEREO_EYE_FUNCTION_FLIP;
#ifdef  INDEX
#ifdef  STEREO_CONFIG_ASSIST
#if 1
        if (CHECK_RHW_CONDITION (pContext, pVertices, pIndices, (DWORD)wCommandCount, EQUAL))
            ConfigAssistInfo.dwFlags |= _2DSUSPECT;
#else
        if (CHECK_RHW_CONDITION (pVertices, pIndices, (DWORD)wCommandCount, EQUAL))
            pStereoEyeFunc = NULL;
#endif
#endif  //STEREO_CONFIG_ASSIST
        if (pStereoData->StereoSettings.dwFlags & STEREO_RHWGREATERATSCREEN)
        {
            if (CHECK_RHW_CONDITION (pContext, pVertices, pIndices, (DWORD)wCommandCount, GREATER))
                pStereoEyeFunc = NULL;
        }
        if (pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN)
        {
            if (CHECK_RHW_CONDITION (pContext, pVertices, pIndices, (DWORD)wCommandCount, LESS))
                pStereoEyeFunc = NULL;
        }
        if (pStereoEyeFunc)
		{
#ifdef	STEREO_DEBUG
			//Make sure vertices are in the system memory. Otherwise it is going to be sooo slow.
			CVertexBuffer      *pVertexBuffer = pContext->ppDX8Streams[0];
			if (pVertexBuffer && pVertexBuffer->getHeapLocation() != CSimpleSurface::HEAP_SYS)
				__asm int 3
#endif	//STEREO_DEBUG
            (*pStereoEyeFunc)(pContext, pVertices, pIndices, (DWORD)wCommandCount);
		}
#else   //INDEX==0
#ifdef  STEREO_CONFIG_ASSIST
#if 1
        if (CHECK_RHW_CONDITION (pContext, pVertices, (DWORD)wCommandCount, EQUAL))
            ConfigAssistInfo.dwFlags |= _2DSUSPECT;
#else
        if (CHECK_RHW_CONDITION (pContext, pVertices, (DWORD)wCommandCount, EQUAL))
            pStereoEyeFunc = NULL;
#endif
#endif  //STEREO_CONFIG_ASSIST
        if (pStereoData->StereoSettings.dwFlags & STEREO_RHWGREATERATSCREEN)
        {
            if (CHECK_RHW_CONDITION (pContext, pVertices, (DWORD)wCommandCount, GREATER))
                pStereoEyeFunc = NULL;
        }
        if (pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN)
        {
            if (CHECK_RHW_CONDITION (pContext, pVertices, (DWORD)wCommandCount, LESS))
                pStereoEyeFunc = NULL;
        }
        if (pStereoEyeFunc)
            (*pStereoEyeFunc)(pContext, pVertices, (DWORD)wCommandCount);
#endif  //INDEX
    }
    RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                              ppCommands,
                                              pCommandBufferEnd,
                                              pdwDP2RStates,
                                              dwDP2Flags);
#ifdef  PER_PRIMITIVE_SYNC
    StereoSync();
#endif  //PER_PRIMITIVE_SYNC

#ifdef  PARTIALRENDERING
    PrimitiveStat[1]++;
    if (PrimitiveStat[2] > PrimitiveStat[1] || PrimitiveStat[3] < PrimitiveStat[1])
    {
        if (pStereoEyeFunc)
        {
#ifdef  INDEX
            (*pStereoEyeFunc)(pContext, pVertices, pIndices, (DWORD)wCommandCount);
            (*pStereoEyeFunc)(pContext, pVertices, pIndices, (DWORD)wCommandCount);
#else   //INDEX==0
            (*pStereoEyeFunc)(pContext, pVertices, (DWORD)wCommandCount);
            (*pStereoEyeFunc)(pContext, pVertices, (DWORD)wCommandCount);
#endif  //INDEX
        }
        return RetVal;
    }
#endif  //PARTIALRENDERING

#ifndef ONE_EYE
    AlternateStereoContext(pContext);
#endif  //ONE_EYE

    if (pStereoEyeFunc)
    {
#ifdef  INDEX
        (*pStereoEyeFunc)(pContext, pVertices, pIndices, (DWORD)wCommandCount);
#else   //INDEX==0
        (*pStereoEyeFunc)(pContext, pVertices, (DWORD)wCommandCount);
#endif  //INDEX
    }

#ifndef NULL_RIGHT_EYE
    *ppCommands = pSaveCommandsPtr; //Restore it for the second pass.
    RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                              ppCommands,
                                              pCommandBufferEnd,
                                              pdwDP2RStates,
                                              dwDP2Flags);
#ifdef  PER_PRIMITIVE_SYNC
    StereoSync();
#endif  //PER_PRIMITIVE_SYNC
#endif  //NULL_RIGHT_EYE

    if (pStereoEyeFunc)
    {
#ifdef  INDEX
        (*pStereoEyeFunc)(pContext, pVertices, pIndices, (DWORD)wCommandCount);
#else   //INDEX==0
        (*pStereoEyeFunc)(pContext, pVertices, (DWORD)wCommandCount);
#endif  //INDEX
    }
    return RetVal;
#endif  //~NOT_IMPLEMENTED
}

#else   //POINT=1
HRESULT STEREO_DP2_FUNCTION_FLIP (NV_DP2FUNCTION_ARGLIST)
{
    HRESULT             RetVal;
    DWORD               dwDP2Op = (DWORD)((*ppCommands)->bCommand);
    LPD3DHAL_DP2COMMAND pSaveCommandsPtr = *ppCommands;

    StereoLockSubstituteFlush();

    // First thing to determine is whether we take a HW T&L or SW T&L route. It breaks
    // down all our code into two vertually mutually exclusive cases. It is better to keep 
    // it this way for optimal performance
    if (!STEREO_R_US(pContext))
    {
        RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                                  ppCommands,
                                                  pCommandBufferEnd,
                                                  pdwDP2RStates,
                                                  dwDP2Flags);
        return RetVal;
    }
    if (pStereoData->dwHWTnL)
    {
        //This case is going to be the most frequently used in future. We want it fast.
        SetupStereoContext(pContext);
        RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                                  ppCommands,
                                                  pCommandBufferEnd,
                                                  pdwDP2RStates,
                                                  dwDP2Flags);
        *ppCommands = pSaveCommandsPtr; //Restore it for the second pass.
        AlternateStereoContext(pContext);
        RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                                  ppCommands,
                                                  pCommandBufferEnd,
                                                  pdwDP2RStates,
                                                  dwDP2Flags);
        return RetVal;
    }
    
#ifdef  NOT_IMPLEMENTED
    //LOG("%s: not implemented yet",GET_REAL_NAME( STEREO_DP2_FUNCTION_FLIP ));
	nvAssert (0);
    return ((*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                             ppCommands,
                                             pCommandBufferEnd,
                                             pdwDP2RStates,
                                             dwDP2Flags));
#else   //~NOT_IMPLEMENTED
    // And now is SW T&L or HW T&L with transformed vertices
    LPBYTE pVertexBufferBase = (LPBYTE)(pContext->ppDX8Streams[0]->getAddress());
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);
    DWORD dwVertexBufferOffset = pContext->dp2.dwVertexBufferOffset;
    DWORD  dwTotalOffset = dwVertexBufferOffset + ((DWORD)((LPD3DHAL_DP2POINTS)lpPrim)->wVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwVStart = (DWORD)((LPD3DHAL_DP2POINTS)lpPrim)->wVStart;
    pContext->dp2.dwTotalOffset = dwTotalOffset;
    LPBYTE pVertices     = (LPBYTE)(pVertexBufferBase+dwTotalOffset);
    PSTEREOEYEFUNC      pStereoEyeFunc = NULL;
    nvAssert (wCommandCount == 1); //Haven't implemented more than 1 yet. What a shame!
    wCommandCount = ((LPD3DHAL_DP2POINTS)lpPrim)->wCount;
#ifdef  STEREO_CONFIG_ASSIST
    ConfigAssistInfo.dwFlags &= ~_2DSUSPECT;
#endif  //STEREO_CONFIG_ASSIST

    SetupStereoContext(pContext);
    if (NV_VERTEX_TRANSFORMED(pContext->pCurrentVShader))
    {
        pStereoEyeFunc = STEREO_EYE_FUNCTION_FLIP;
#ifdef  STEREO_CONFIG_ASSIST
#if 1
        if (CHECK_RHW_CONDITION (pContext, pVertices, (DWORD)wCommandCount, EQUAL))
            ConfigAssistInfo.dwFlags |= _2DSUSPECT;
#else
        if (CHECK_RHW_CONDITION (pContext, pVertices, (DWORD)wCommandCount, EQUAL))
            pStereoEyeFunc = NULL;
#endif
#endif  //STEREO_CONFIG_ASSIST
        if (pStereoData->StereoSettings.dwFlags & STEREO_RHWGREATERATSCREEN)
        {
            if (CHECK_RHW_CONDITION (pContext, pVertices, (DWORD)wCommandCount, GREATER))
                pStereoEyeFunc = NULL;
        }
        if (pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN)
        {
            if (CHECK_RHW_CONDITION (pContext, pVertices, (DWORD)wCommandCount, LESS))
                pStereoEyeFunc = NULL;
        }
        if (pStereoEyeFunc)
            (*pStereoEyeFunc)(pContext, pVertices, (DWORD)wCommandCount);
    }
    RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                              ppCommands,
                                              pCommandBufferEnd,
                                              pdwDP2RStates,
                                              dwDP2Flags);
#ifdef  PER_PRIMITIVE_SYNC
    StereoSync();
#endif  //PER_PRIMITIVE_SYNC

#ifndef ONE_EYE
    AlternateStereoContext(pContext);
#endif  //ONE_EYE

    if (pStereoEyeFunc)
    {
        (*pStereoEyeFunc)(pContext, pVertices, (DWORD)wCommandCount);
    }

#ifndef NULL_RIGHT_EYE
    *ppCommands = pSaveCommandsPtr; //Restore it for the second pass.
    RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                              ppCommands,
                                              pCommandBufferEnd,
                                              pdwDP2RStates,
                                              dwDP2Flags);
#ifdef  PER_PRIMITIVE_SYNC
    StereoSync();
#endif  //PER_PRIMITIVE_SYNC
#endif  //NULL_RIGHT_EYE

    if (pStereoEyeFunc)
    {
        (*pStereoEyeFunc)(pContext, pVertices, (DWORD)wCommandCount);
    }
    return RetVal;
#endif  //~NOT_IMPLEMENTED
}

#endif  //POINT

#else	//DRAWPRIMITIVE
HRESULT STEREO_DP2_FUNCTION_FLIP (NV_DP2FUNCTION_ARGLIST)
{
    HRESULT             RetVal;
    DWORD               dwDP2Op = (DWORD)((*ppCommands)->bCommand);
    LPD3DHAL_DP2COMMAND pSaveCommandsPtr = *ppCommands;

#ifdef	STEREO_DEBUG
	//Make sure it is not a rendering to a texture
	CNvObject *pNvObj = pContext->pRenderTarget->getWrapper();
	if (pNvObj->getClass() == CNvObject::NVOBJ_TEXTURE)
		__asm int 3
	//Make sure we render to a stereo surface
	if (pContext->pRenderTarget->isStereo() == 0)
		__asm int 3
	//Do we render to the Z buffer?
	if (pContext->pRenderTarget->isZBuffer())
		__asm int 3
#endif	//STEREO_DEBUG

    StereoLockSubstituteFlush();

    // First thing to determine is whether we take a HW T&L or SW T&L route. It breaks
    // down all our code into two vertually mutually exclusive cases. It is better to keep 
    // it this way for optimal performance
    if (!STEREO_R_US(pContext))
    {
        RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                                  ppCommands,
                                                  pCommandBufferEnd,
                                                  pdwDP2RStates,
                                                  dwDP2Flags);
        return RetVal;
    }
	BOOL UseHWTnLPath = FALSE;
	if (STEREODATA(StereoSettings.dwFlags) & STEREO_MIXEDTNLS)
	{
		if (!NV_VERTEX_TRANSFORMED(pContext->pCurrentVShader))
			UseHWTnLPath = TRUE;
	} else
	{
		if (pStereoData->dwHWTnL)
			UseHWTnLPath = TRUE;
	}

    if (UseHWTnLPath)
    {
        //This case is going to be the most frequently used in future. We want it fast.
        SetupStereoContext(pContext);
        RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                                  ppCommands,
                                                  pCommandBufferEnd,
                                                  pdwDP2RStates,
                                                  dwDP2Flags);
        *ppCommands = pSaveCommandsPtr; //Restore it for the second pass.
        AlternateStereoContext(pContext);
        RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                                  ppCommands,
                                                  pCommandBufferEnd,
                                                  pdwDP2RStates,
                                                  dwDP2Flags);
        return RetVal;
    }
    
#ifdef	STEREO_DEBUG
	// Once we got here make sure vertices are in the system memory. 
	// Otherwise it is going to be sooo slow.
	CVertexBuffer      *pVertexBuffer = pContext->ppDX8Streams[0];
	if (pVertexBuffer && pVertexBuffer->getHeapLocation() != CSimpleSurface::HEAP_SYS)
		__asm int 3
#endif	//STEREO_DEBUG

#if 0
	__asm int 3
#endif

    // And now is SW T&L or HW T&L with transformed vertices
    LPBYTE pVertexBufferBase = (LPBYTE)(pContext->ppDX8Streams[0]->getAddress());

#ifdef  NOT_IMPLEMENTED
    //LOG("%s: not implemented yet",GET_REAL_NAME( STEREO_DP2_FUNCTION_FLIP ));
    nvAssert (0);
    return ((*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                             ppCommands,
                                             pCommandBufferEnd,
                                             pdwDP2RStates,
                                             dwDP2Flags));
#else   //~NOT_IMPLEMENTED
    //nvAssert (0);
    WORD   wCommandCount = (*ppCommands)->wPrimitiveCount;
    LPBYTE lpPrim        = (LPBYTE)(*ppCommands) + sizeof(D3DHAL_DP2COMMAND);

#ifdef	STEREO_DEBUG
    if (wCommandCount != 1)
    {
		__asm int 3
	} 
#endif	//STEREO_DEBUG
#ifdef  INDEX
    LPD3DHAL_DP2DRAWINDEXEDPRIMITIVE lpDPCommand = (LPD3DHAL_DP2DRAWINDEXEDPRIMITIVE)(lpPrim);
    PSTEREOEYEFUNCINDEX      pStereoEyeFunc = NULL;
    PSTEREORHWFUNCINDEX      pStereoRHWFunc = NULL;
#ifdef  START_VERTEX
    DWORD dwVertexBufferOffset = pContext->dp2.dwVertexBufferOffset;
	pContext->dp2.dwVStart      = lpDPCommand->BaseVertexIndex;
    DWORD  dwTotalOffset = dwVertexBufferOffset + ((DWORD)pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwTotalOffset = dwTotalOffset;
    LPBYTE pIndices      = (LPBYTE)(pContext->pIndexBuffer->getAddress() + pContext->pIndexBuffer->getVertexStride()*lpDPCommand->StartIndex);
    pContext->dp2.dwIndexStride = pContext->pIndexBuffer->getVertexStride();
    LPBYTE pVertices     = (LPBYTE)(pVertexBufferBase+dwTotalOffset);
#else   //START_VERTEX==0
    DWORD  dwTotalOffset = ((LPD3DHAL_DP2DRAWINDEXEDPRIMITIVE2)lpDPCommand)->BaseVertexOffset;
    pContext->dp2.dwTotalOffset = dwTotalOffset;
    LPBYTE pIndices      = (LPBYTE)(pContext->pIndexBuffer->getAddress() + ((LPD3DHAL_DP2DRAWINDEXEDPRIMITIVE2)lpDPCommand)->StartIndexOffset);
    pContext->dp2.dwIndexStride = pContext->pIndexBuffer->getVertexStride();
    LPBYTE pVertices     = (LPBYTE)(pVertexBufferBase+dwTotalOffset);
#endif  //START_VERTEX
#else   //INDEX==0
    LPD3DHAL_DP2DRAWPRIMITIVE lpDPCommand = (LPD3DHAL_DP2DRAWPRIMITIVE)(lpPrim);
    PSTEREOEYEFUNC      pStereoEyeFunc = NULL;
    PSTEREORHWFUNC      pStereoRHWFunc = NULL;
#ifdef  START_VERTEX
    DWORD dwVertexBufferOffset = pContext->dp2.dwVertexBufferOffset;
	pContext->dp2.dwVStart      = lpDPCommand->VStart;
    DWORD  dwTotalOffset = dwVertexBufferOffset + ((DWORD)pContext->dp2.dwVStart * pContext->pCurrentVShader->getStride());
    pContext->dp2.dwTotalOffset = dwTotalOffset;
    LPBYTE pVertices     = (LPBYTE)(pVertexBufferBase+dwTotalOffset);
#else   //START_VERTEX==0
    DWORD  dwTotalOffset = ((LPD3DHAL_DP2DRAWPRIMITIVE2)lpDPCommand)->FirstVertexOffset;
    pContext->dp2.dwTotalOffset = dwTotalOffset;
    LPBYTE pVertices     = (LPBYTE)(pVertexBufferBase+dwTotalOffset);
#endif  //START_VERTEX
#endif  //INDEX
#ifdef	STEREO_DEBUG
    if (!lpDPCommand->primType || lpDPCommand->primType == 1)
    {
		__asm int 3
	} 
#endif	//STEREO_DEBUG
#ifdef  INDEX
	pStereoEyeFunc = PrimTypeToStereoDP2Index[lpDPCommand->primType].pStereoEyeFuncIndex;
	pStereoRHWFunc = PrimTypeToStereoDP2Index[lpDPCommand->primType].pStereoRHWFuncIndex;

    SetupStereoContext(pContext);

#ifdef  STEREO_CONFIG_ASSIST
    if (pStereoRHWFunc (pContext, pVertices, pIndices, (DWORD)lpDPCommand->PrimitiveCount, EQUAL))
        ConfigAssistInfo.dwFlags |= _2DSUSPECT;
#endif  //STEREO_CONFIG_ASSIST
    if (pStereoData->StereoSettings.dwFlags & STEREO_RHWGREATERATSCREEN)
    {
        if (pStereoRHWFunc (pContext, pVertices, pIndices, (DWORD)lpDPCommand->PrimitiveCount, GREATER))
            pStereoEyeFunc = NULL;
    }
    if (pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN)
    {
        if (pStereoRHWFunc (pContext, pVertices, pIndices, (DWORD)lpDPCommand->PrimitiveCount, LESS))
            pStereoEyeFunc = NULL;
    }

    if (pStereoEyeFunc)
	{
        (*pStereoEyeFunc)(pContext, pVertices, pIndices, (DWORD)lpDPCommand->PrimitiveCount);
	}
    RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                              ppCommands,
                                              pCommandBufferEnd,
                                              pdwDP2RStates,
                                              dwDP2Flags);
    AlternateStereoContext(pContext);
    if (pStereoEyeFunc)
    {
        (*pStereoEyeFunc)(pContext, pVertices, pIndices, (DWORD)lpDPCommand->PrimitiveCount);
    }

    *ppCommands = pSaveCommandsPtr; //Restore it for the second pass.
    RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                              ppCommands,
                                              pCommandBufferEnd,
                                              pdwDP2RStates,
                                              dwDP2Flags);
    if (pStereoEyeFunc)
    {
        (*pStereoEyeFunc)(pContext, pVertices, pIndices, (DWORD)lpDPCommand->PrimitiveCount);
    }
    return RetVal;
#else   //INDEX==0
	pStereoEyeFunc = PrimTypeToStereoDP2[lpDPCommand->primType].pStereoEyeFunc;
	pStereoRHWFunc = PrimTypeToStereoDP2[lpDPCommand->primType].pStereoRHWFunc;

    SetupStereoContext(pContext);

#ifdef  STEREO_CONFIG_ASSIST
    if (pStereoRHWFunc (pContext, pVertices, (DWORD)lpDPCommand->PrimitiveCount, EQUAL))
        ConfigAssistInfo.dwFlags |= _2DSUSPECT;
#endif  //STEREO_CONFIG_ASSIST
    if (pStereoData->StereoSettings.dwFlags & STEREO_RHWGREATERATSCREEN)
    {
        if (pStereoRHWFunc (pContext, pVertices, (DWORD)lpDPCommand->PrimitiveCount, GREATER))
            pStereoEyeFunc = NULL;
    }
    if (pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN)
    {
        if (pStereoRHWFunc (pContext, pVertices, (DWORD)lpDPCommand->PrimitiveCount, LESS))
            pStereoEyeFunc = NULL;
    }

    if (pStereoEyeFunc)
	{
        (*pStereoEyeFunc)(pContext, pVertices, (DWORD)lpDPCommand->PrimitiveCount);
	}
    RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                              ppCommands,
                                              pCommandBufferEnd,
                                              pdwDP2RStates,
                                              dwDP2Flags);
    AlternateStereoContext(pContext);
    if (pStereoEyeFunc)
    {
        (*pStereoEyeFunc)(pContext, pVertices, (DWORD)lpDPCommand->PrimitiveCount);
    }

    *ppCommands = pSaveCommandsPtr; //Restore it for the second pass.
    RetVal = (*(nvDP2SetFuncs_Orig[dwDP2Op]))(pContext,
                                              ppCommands,
                                              pCommandBufferEnd,
                                              pdwDP2RStates,
                                              dwDP2Flags);
    if (pStereoEyeFunc)
    {
        (*pStereoEyeFunc)(pContext, pVertices, (DWORD)lpDPCommand->PrimitiveCount);
    }
    return RetVal;
#endif  //INDEX
#endif  //~NOT_IMPLEMENTED
}

#endif	//DRAWPRIMITIVE

#undef  STEREO_DP2_FUNCTION_OAU
#undef  STEREO_EYE_FUNCTION_OAU
#undef  STEREO_2D_FUNCTION_OAU
#undef  STEREO_DP2_FUNCTION_FLIP
#undef  STEREO_EYE_FUNCTION_FLIP
#undef  CHECK_RHW_CONDITION
#undef  INDEX
#undef  INDEX2
#undef  FAN
#undef  STRIP
#undef  FAN_OR_STRIP
#undef  FOR_ALL_VERTICES
#undef  GET_V1_POINTER
#undef  GET_V2_POINTER
#undef  GET_V3_POINTER
#undef  ADVANCE
#undef  ALIGN
#undef  START_VERTEX
#undef  LINE
#undef  POINT
#undef  DEFINE_LOCALS
#undef  NOT_IMPLEMENTED
#undef	DRAWPRIMITIVE