/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 * PORTIONS COPYRIGHT (C) Microsoft Corporation, 1998.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvLight.cpp                                                       *
*   NV4 T&L lighting routines                                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler    01Dec98    created                              *
*                                                                           *
\***************************************************************************/

#include "nvprecomp.h"

//---------------------------------------------------------------------------

BOOL nvLightIsDefined(nvLight *pLight)
{
    return (pLight->dwFlags & NVLIGHT_DEFINED);
}

//---------------------------------------------------------------------------

BOOL nvLightIsEnabled(nvLight *pLight)
{
    return (pLight->dwFlags & NVLIGHT_ENABLED);
}

#if (NVARCH >= 0x10)

//---------------------------------------------------------------------------

HRESULT nvLightSetLight(nvLight *pnvLight, LPD3DLIGHT7 pLight7)
{
    float phi2, theta2;

    nvAssert (pLight7);

    // Validate the parameters passed
    switch (pLight7->dltType) {

        case D3DLIGHT_POINT:

            pnvLight->Light7 = *pLight7;

            // sanity-check attenuation
            if ((DWORD_FROM_FLOAT(pnvLight->Light7.dvAttenuation0) == 0) &&
                (DWORD_FROM_FLOAT(pnvLight->Light7.dvAttenuation1) == 0) &&
                (DWORD_FROM_FLOAT(pnvLight->Light7.dvAttenuation2) == 0)) {
                DPF ("d3d gave us a divide by 0 error b/c all attenuations are 0. nice.");
                pnvLight->Light7.dvAttenuation0 = 1.f;
            }

            break;

        case D3DLIGHT_SPOT:

            pnvLight->Light7 = *pLight7;

            // sanity-check attenuation
            if ((DWORD_FROM_FLOAT(pnvLight->Light7.dvAttenuation0) == 0) &&
                (DWORD_FROM_FLOAT(pnvLight->Light7.dvAttenuation1) == 0) &&
                (DWORD_FROM_FLOAT(pnvLight->Light7.dvAttenuation2) == 0)) {
                DPF ("d3d gave us a divide by 0 error b/c all attenuations are 0. nice.");
                pnvLight->Light7.dvAttenuation0 = 1.f;
            }

            // negate direction vector to align with OpenGL
            ReverseVector3 (&(pnvLight->direction), &(pnvLight->Light7.dvDirection));
            NormalizeVector3 (&(pnvLight->direction));  // necessary??

            // calculate falloff and direction
            explut (pnvLight->Light7.dvFalloff, &(pnvLight->falloffParams.L), &(pnvLight->falloffParams.M));
            pnvLight->falloffParams.N = 1.f + pnvLight->falloffParams.L - pnvLight->falloffParams.M;

            // Attenuate the spot direction to get falloff to work
            theta2 = (float) cos(0.5*pnvLight->Light7.dvTheta);
            phi2   = (float) cos(0.5*pnvLight->Light7.dvPhi);
            // Handle case in which theta gets close to or overtakes phi, since hardware can't.
            if (phi2 >= theta2) {        // outer angle <= inner angle, oops
                phi2 = 0.999f * theta2;  // make outer angle cosine slightly smaller
            }

            pnvLight->dvScale = nvInv (theta2 - phi2);
            pnvLight->dvW     = -phi2 * pnvLight->dvScale;

            break;

        case D3DLIGHT_DIRECTIONAL:

            pnvLight->Light7 = *pLight7;

            // negate direction vector to align with OpenGL
            ReverseVector3 (&(pnvLight->direction), &(pnvLight->Light7.dvDirection));
            NormalizeVector3 (&(pnvLight->direction));  // necessary??

            break;

        default:

            // No other light types are allowed
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "Invalid light type passed");
            return DDERR_INVALIDPARAMS;
            break;

    }

    pnvLight->dwFlags |= NVLIGHT_DEFINED;

    return DD_OK;
}

//---------------------------------------------------------------------------

void nvLightInit(nvLight *pnvLight)
{
    pnvLight->dwFlags = 0;
    pnvLight->pNext   = NULL;

    ZeroMemory(&(pnvLight->Light7), sizeof(pnvLight->Light7));
    return;
}

//---------------------------------------------------------------------------

void nvLightEnable(nvLight *pnvLight, nvLight **ppRoot)
{
    nvLight *pTmp;

    // Assert that it is not already enabled
    if (nvLightIsEnabled(pnvLight)) return;

    // Assert that Root Ptr is not Null
    if (ppRoot == NULL) return;

    pTmp = *ppRoot;
    *ppRoot = pnvLight;
    pnvLight->pNext = pTmp;
    pnvLight->dwFlags |= NVLIGHT_ENABLED;

    return;
}

//---------------------------------------------------------------------------

void nvLightDisable(nvLight *pnvLight, nvLight **ppRoot)
{
    nvLight *pLightPrev;

    // Assert that the light is enabled
    if (!nvLightIsEnabled(pnvLight)) return;

    // Assert that Root Ptr is not Null
    if (ppRoot == NULL) return;

    pLightPrev = *ppRoot;

    // If this is the first light in the active list
    if (pLightPrev == pnvLight)
    {
        *ppRoot = pnvLight->pNext;
        pnvLight->dwFlags &= ~NVLIGHT_ENABLED;
        return;
    }

    while (pLightPrev->pNext != pnvLight)
    {
        // Though this light was marked as enabled, it is not on
        // the active list. Assert this.
        if (pLightPrev->pNext == NULL) {
            pnvLight->dwFlags &= ~NVLIGHT_ENABLED;
            return;
        }

        // Else get the next pointer
        pLightPrev = pLightPrev->pNext;
    }

    pLightPrev->pNext = pnvLight->pNext;
    pnvLight->dwFlags &= ~NVLIGHT_ENABLED;

    return;
}

//---------------------------------------------------------------------------

HRESULT nvGrowLightArray (PNVD3DCONTEXT pContext, DWORD dwIndex)
{
    DWORD dwNewArraySize;
    nvLight *pTmpLightArray;
    nvLight *pTmp;
    DWORD i;

    // allocate a few extra in anticipation of more lights being used in the future
    dwNewArraySize = dwIndex+16;
    pTmpLightArray = (nvLight *) new BYTE[dwNewArraySize * sizeof(nvLight)];
    if (pTmpLightArray == NULL) return (DDERR_OUTOFMEMORY);

    // Start the active light list from scratch
    pContext->lighting.pActiveLights = NULL;

    // copy all the current lights into the new array
    for (i=0; i<pContext->dwLightArraySize; i++)
    {
        pTmpLightArray[i] = pContext->pLightArray[i];

        // if the light is enabled, add it to the active list
        if (nvLightIsEnabled(&(pContext->pLightArray[i])))
        {
            pTmp = pContext->lighting.pActiveLights;
            pContext->lighting.pActiveLights = &pTmpLightArray[i];
            pTmpLightArray[i].pNext = pTmp;
        }
    }

    // initialize the remainder of the array
    for (i=pContext->dwLightArraySize; i<dwNewArraySize; i++) {
        nvLightInit(&(pTmpLightArray[i]));
    }

    // free the old lights (if any) and set pointer to the new
    if (pContext->pLightArray) {
        delete [](pContext->pLightArray);
    }
    pContext->pLightArray = pTmpLightArray;
    pContext->dwLightArraySize = dwNewArraySize;

    return (D3D_OK);
}

#endif  // NVARCH >= 0x10

