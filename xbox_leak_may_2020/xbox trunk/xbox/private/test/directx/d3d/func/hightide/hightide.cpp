/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    hightide.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "revolve.h"
#include "hightide.h"

//******************************************************************************
// Revolution profiles
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     BaseRadius
//
// Description:
//
//     Describe the function profile of the lighthouse base.  This function will
//     return the radius of the base at a given height.
//
// Arguments:
//
//     float y               - Height at which to evaluate the radius
//
//     float fLength         - Maximum height
//
//     LPVOID pvContext         - Variable supplied by the calling function.
//
// Return Value:
//
//     The radius at the given height.
//
//******************************************************************************
float BaseRadius(float y, float fHeight, LPVOID pvContext) {

    return 0.45f;
}

//******************************************************************************
//
// Function:
//
//     BalconyRadius
//
// Description:
//
//     Describe the function profile of the lighthouse balcony.  This function
//     will return the radius of the balcony at a given height.
//
// Arguments:
//
//     float y               - Height at which to evaluate the radius
//
//     float fLength         - Maximum height
//
//     LPVOID pvContext         - Variable supplied by the calling function.
//
// Return Value:
//
//     The radius at the given height.
//
//******************************************************************************
float BalconyRadius(float y, float fHeight, LPVOID pvContext) {

    return 0.2f;
}

//******************************************************************************
//
// Function:
//
//     PortalRadius
//
// Description:
//
//     Describe the function profile of the lighthouse portal.  This function 
//     will return the radius of the portal at a given height.
//
// Arguments:
//
//     float y               - Height at which to evaluate the radius
//
//     float fLength         - Maximum height
//
//     LPVOID pvContext         - Variable supplied by the calling function.
//
// Return Value:
//
//     The radius at the given height.
//
//******************************************************************************
float PortalRadius(float y, float fHeight, LPVOID pvContext) {

    return 0.125f;
}

//******************************************************************************
//
// Function:
//
//     RoofRadius
//
// Description:
//
//     Describe the function profile of the lighthouse roof.  This function will
//     return the radius of the roof at a given height.
//
// Arguments:
//
//     float y               - Height at which to evaluate the radius
//
//     float fLength         - Maximum height
//
//     LPVOID pvContext         - Variable supplied by the calling function.
//
// Return Value:
//
//     The radius at the given height.
//
//******************************************************************************
float RoofRadius(float y, float fHeight, LPVOID pvContext) {

    return 0.2f * ((0.25f - y) / 0.25f);
}

//******************************************************************************
//
// Function:
//
//     TrunkRadius
//
// Description:
//
//     Describe the function profile of the lighthouse trunk.  This function
//     will return the radius of the trunk at a given height.
//
// Arguments:
//
//     float y               - Height at which to evaluate the radius
//
//     float fLength         - Maximum height
//
//     LPVOID pvContext         - Variable supplied by the calling function.
//
// Return Value:
//
//     The radius at the given height.
//
//******************************************************************************
float TrunkRadius(float y, float fHeight, LPVOID pvContext) {

    return 0.15f + 0.125f * ((2.4f - y) / 2.4f);
}

//******************************************************************************
//
// Function:
//
//     LightConeRadius
//
// Description:
//
//     Describe the function profile of the lighthouse spotlight cone.  This
//     function will return the radius of the light cone at a given height.
//
// Arguments:
//
//     float y               - Height at which to evaluate the radius
//
//     float fLength         - Maximum height
//
//     LPVOID pvContext         - Variable supplied by the calling function.
//
// Return Value:
//
//     The radius at the given height.
//
//******************************************************************************
float LightConeRadius(float y, float fHeight, LPVOID pvContext) {

    return (fHeight - y) / fHeight * 1.6f;
}

//******************************************************************************
//
// Function:
//
//     CreateLighthouse
//
// Description:
//
//     Create the necessary meshes, materials, and textures to model a 
//     lighthouse.
//
// Arguments:
//
//     PLIGHTHOUSE plh          - Structure to be filled in with the created
//                                lighthouse objects.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CreateLighthouse(CDevice8* pDevice, PLIGHTHOUSE plh) {

    UINT i;

    // Clear out the lighthouse structure
    memset(plh, 0, sizeof(LIGHTHOUSE));

    InitMatrix(&plh->mWorld,
        (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 1.0f), (float)( 0.0f),
        (float)(-1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );

    // Initialize the lighthouse base
    if (!CreateRevolution(&plh->rvBase, 0.21f, 1, 6, BaseRadius, NULL, 0.001f, 
                            TRUE, 2.0f, 2.0f)) 
    {
        return FALSE;
    }

    // Initialize the lighthouse trunk
    if (!CreateRevolution(&plh->rvTrunk, 2.4f, 1, 16, TrunkRadius, NULL, 0.001f,
                            TRUE, 2.0f, 8.0f)) 
    {
        return FALSE;
    }
    for (i = 0; i < plh->rvTrunk.uCenterVertices; i++) {
        plh->rvTrunk.prCenter[i].vPosition.y += 0.2f;
    }

    // Initialize the lighthouse balcony
    if (!CreateRevolution(&plh->rvBalcony, 0.10f, 1, 6, BalconyRadius, NULL, 
                            0.001f, TRUE, 1.0f, 0.25f)) 
    {
        return FALSE;
    }
    for (i = 0; i < plh->rvBalcony.uCenterVertices; i++) {
        plh->rvBalcony.prCenter[i].vPosition.y += 2.6f;
    }

    // Initialize the lighthouse portal
    if (!CreateRevolution(&plh->rvPortal, 0.20f, 1, 6, PortalRadius, NULL, 
                            0.001f, TRUE, 1.0f, 0.25f)) 
    {
        return FALSE;
    }

    plh->prPortal = (PLVERTEX)MemAlloc32(plh->rvPortal.uCenterVertices * sizeof(LVERTEX));
    if (!plh->prPortal) {
        return FALSE;
    }

    for (i = 0; i < plh->rvPortal.uCenterVertices; i++) {
        plh->rvPortal.prCenter[i].vPosition.y += 2.7f;
        plh->prPortal[i].vPosition = plh->rvPortal.prCenter[i].vPosition;
        plh->prPortal[i].cDiffuse = RGB_MAKE(64, 64, 64);
    }

    // Initialize the lighthouse roof
    if (!CreateRevolution(&plh->rvRoof, 0.25f, 1, 6, RoofRadius, NULL, 0.001f, 
                            TRUE, 0.5f, 0.25f))
    {
        return FALSE;
    }
    for (i = 0; i < plh->rvRoof.uCenterVertices; i++) {
        plh->rvRoof.prCenter[i].vPosition.y += 2.9f;
    }

    // Initialize the material and textures
    SetMaterial(&plh->material, RGB_MAKE(255, 255, 255),
                            RGB_MAKE(16, 16, 16), RGB_MAKE(128, 128, 128), 
                            RGB_MAKE(128, 128, 128), (float)(10.0f));

    plh->pd3dtTrunk = (CTexture8*)CreateTexture(pDevice, TEXT("rockwall.bmp"));
    if (!plh->pd3dtTrunk) {
        return FALSE;
    }

    plh->pd3dtRoof  = (CTexture8*)CreateTexture(pDevice, TEXT("redbrick.bmp"));
    if (!plh->pd3dtRoof) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     RenderLighthouse
//
// Description:
//
//     Render the given lighthouse using the given light intensity for the 
//     lighthouse portal.
//
// Arguments:
//
//     PLIGHTHOUSE plh          - Structure describing the lighthouse to render.
//
//     float fIntensity      - Light intensity of the lighthouse portal.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL RenderLighthouse(CDevice8* pDevice, PLIGHTHOUSE plh, float fIntensity) {

    BYTE        color;
    UINT        i;
    HRESULT     hr;

    // Render the base of the lighthouse
    pDevice->SetTransform(D3DTS_WORLD, &plh->mWorld);
    pDevice->SetMaterial(&plh->material);
    pDevice->SetTexture(0, plh->pd3dtTrunk);

    hr = pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 
            plh->rvBase.uCenterVertices, plh->rvBase.uCenterIndices / 3, 
            plh->rvBase.pwCenter, D3DFMT_INDEX16, plh->rvBase.prCenter,
            sizeof(HHVERTEX));
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawIndexedPrimitiveUP"))) {
        return FALSE;
    }

    hr = pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, plh->rvBase.uTopVertices - 2, 
            plh->rvBase.prTop, sizeof(VERTEX));
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawPrimitiveUP"))) {
        return FALSE;
    }

    // Render the trunk of the lighthouse
    hr = pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 
            plh->rvTrunk.uCenterVertices, plh->rvTrunk.uCenterIndices / 3, 
            plh->rvTrunk.pwCenter, D3DFMT_INDEX16, plh->rvTrunk.prCenter,
            sizeof(HHVERTEX));
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawIndexedPrimitiveUP"))) {
        return FALSE;
    }

    // Render the portal of the lighthouse
    pDevice->SetTexture(0, NULL);

    color = (BYTE)(fIntensity * 200.0f);
    for (i = 0; i < plh->rvPortal.uCenterVertices; i++) {
        plh->prPortal[i].cSpecular = RGB_MAKE(color, color, color);
    }

    pDevice->SetVertexShader(FVF_LVERTEX);

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    hr = pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 
            plh->rvPortal.uCenterVertices, plh->rvPortal.uCenterIndices / 3, 
            plh->rvPortal.pwCenter, D3DFMT_INDEX16, plh->prPortal,
            sizeof(LVERTEX));
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawIndexedPrimitiveUP"))) {
        return FALSE;
    }

    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

    // Render the roof of the lighthouse
    pDevice->SetTexture(0, plh->pd3dtRoof);

    pDevice->SetVertexShader(FVF_VERTEX);

    hr = pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 
            plh->rvBalcony.uCenterVertices, plh->rvBalcony.uCenterIndices / 3, 
            plh->rvBalcony.pwCenter, D3DFMT_INDEX16, plh->rvBalcony.prCenter,
            sizeof(HHVERTEX));
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawIndexedPrimitiveUP"))) {
        return FALSE;
    }

    hr = pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 
            plh->rvRoof.uCenterVertices, plh->rvRoof.uCenterIndices / 3, 
            plh->rvRoof.pwCenter, D3DFMT_INDEX16, plh->rvRoof.prCenter,
            sizeof(HHVERTEX));
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawIndexedPrimitiveUP"))) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     ReleaseLighthouse
//
// Description:
//
//     Release all objects associated with the given lighthouse structure.
//
// Arguments:
//
//     PLIGHTHOUSE plh          - Structure describing the lighthouse to 
//                                release.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseLighthouse(PLIGHTHOUSE plh) {

    MemFree32(plh->prPortal);
    if (plh->pd3dtTrunk) {
        ReleaseTexture(plh->pd3dtTrunk);
    }
    if (plh->pd3dtRoof) {
        ReleaseTexture(plh->pd3dtRoof);
    }
    ReleaseRevolution(&plh->rvBase);
    ReleaseRevolution(&plh->rvTrunk);
    ReleaseRevolution(&plh->rvBalcony);
    ReleaseRevolution(&plh->rvPortal);
    ReleaseRevolution(&plh->rvRoof);
}

//******************************************************************************
//
// Function:
//
//     InitWaveSources
//
// Description:
//
//     Initialize the wave sources for the water.
//
// Arguments:
//
//     PWAVESOURCE pws          - Array of wave sources to initialize
//
// Return Value:
//
//     None.
//
//******************************************************************************
void InitWaveSources(PWAVESOURCE pws) {
    
    UINT i;

    pws[0].vPos = D3DXVECTOR3(0.0f, 0.0f, 20.0f);
        pws[0].fWavelength = 1.0f;

    for (i = 0; i < NUM_WAVE_SOURCES; i++) {
        pws[i].fFrequency = M_PI / 48.0f;
        pws[i].fAmplitude = 0.05f;
        pws[i].fPhase = 0.0f;
    }
}

//******************************************************************************
//
// Function:
//
//     SubtractVertex
//
// Description:
//
//     Subtract the positional vector components of the two vertices and
//     return the resulting vector.
//
// Arguments:
//
//     D3DVERTEX r1             - First vector.
//
//     D3DVERTEX r2             - Second vector.
//
// Return Value:
//
//     The resulting difference vector.
//
//******************************************************************************
static D3DXVECTOR3 SubtractVertex(PHHVERTEX pr1, PHHVERTEX pr2) {

    D3DXVECTOR3 v;

    v.x = pr1->x - pr2->x;
    v.y = pr1->y - pr2->y;
    v.z = pr1->z - pr2->z;

    return v;
}

//******************************************************************************
//
// Function:
//
//     GenerateCliffNormals
//
// Description:
//
//     Generate vertex normals for the given cliff mesh.
//
// Arguments:
//
//     LPD3DVERTEX pr           - Array of vertices describing the cliff mesh.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void GenerateCliffNormals(PHHVERTEX pr) {

    D3DXVECTOR3* pvFaceNormal;
    D3DXVECTOR3  v1, v2, r;
    UINT         x, y;

    pvFaceNormal = (D3DXVECTOR3*)MemAlloc32(((CLIFF_GRID_DIM_Y - 1) 
                                    * (CLIFF_GRID_DIM_X - 1) * 2) * sizeof(D3DXVECTOR3));
    if (!pvFaceNormal) {
        return;
    }

    // Generate face normals for the triangles
    for (y = 0; y < CLIFF_GRID_DIM_Y - 1; y++) {

        for (x = 0; x < CLIFF_GRID_DIM_X - 1; x++) {

            v1 = SubtractVertex(&pr[y * CLIFF_GRID_DIM_X + x + 1], 
                                &pr[y * CLIFF_GRID_DIM_X + x]);
            v2 = SubtractVertex(&pr[(y + 1) * CLIFF_GRID_DIM_X + x], 
                                &pr[y * CLIFF_GRID_DIM_X + x]);
            D3DXVec3Normalize(&pvFaceNormal[y * (CLIFF_GRID_DIM_X - 1) * 2 + x * 2], D3DXVec3Cross(&r, &v1, &v2));

            v1 = SubtractVertex(&pr[y * CLIFF_GRID_DIM_X + x + 1], 
                                &pr[(y + 1) * CLIFF_GRID_DIM_X + x + 1]);
            v2 = SubtractVertex(&pr[(y + 1) * CLIFF_GRID_DIM_X + x], 
                                &pr[(y + 1) * CLIFF_GRID_DIM_X + x + 1]);
            D3DXVec3Normalize(&pvFaceNormal[y * (CLIFF_GRID_DIM_X - 1) * 2 + x * 2 + 1], D3DXVec3Cross(&r, &v2, &v1));
        }
    }

    // Generate vertex normals by averaging the adjoining face normals
    for (y = 1; y < CLIFF_GRID_DIM_Y - 1; y++) {

        for (x = 1; x < CLIFF_GRID_DIM_X - 1; x++) {

            v1 = pvFaceNormal[(y - 1) * (CLIFF_GRID_DIM_X - 1) * 2 
                                + (x - 1) * 2 + 1];
            v1 += pvFaceNormal[(y - 1) * (CLIFF_GRID_DIM_X - 1) * 2 + x * 2];
            v1 += pvFaceNormal[(y - 1) * (CLIFF_GRID_DIM_X - 1) * 2 
                                + x * 2 + 1];
            v1 += pvFaceNormal[y * (CLIFF_GRID_DIM_X - 1) * 2 + (x - 1) * 2];
            v1 += pvFaceNormal[y * (CLIFF_GRID_DIM_X - 1) * 2 
                                + (x - 1) * 2 + 1];
            v1 += pvFaceNormal[y * (CLIFF_GRID_DIM_X - 1) * 2 + x * 2];
            v1 = v1 / 6.0f;
            pr[y * CLIFF_GRID_DIM_X + x].nx = v1.x;
            pr[y * CLIFF_GRID_DIM_X + x].ny = v1.y;
            pr[y * CLIFF_GRID_DIM_X + x].nz = v1.z;
        }
    }

    MemFree32(pvFaceNormal);
}

//******************************************************************************
//
// Function:
//
//     GenerateBeachNormals
//
// Description:
//
//     Generate vertex normals for the given beach mesh.
//
// Arguments:
//
//     LPD3DVERTEX pr           - Array of vertices describing the beach mesh.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void GenerateBeachNormals(PHHVERTEX pr) {

    D3DXVECTOR3* pvFaceNormal;
    D3DXVECTOR3  v1, v2, r;
    UINT         x, y;

    pvFaceNormal = (D3DXVECTOR3*)MemAlloc32(((BEACH_GRID_DIM_Y - 1) 
                                    * (BEACH_GRID_DIM_X - 1) * 2) * sizeof(D3DXVECTOR3));
    if (!pvFaceNormal) {
        return;
    }

    // Generate face normals for the triangles
    for (y = 0; y < BEACH_GRID_DIM_Y - 1; y++) {

        for (x = 0; x < BEACH_GRID_DIM_X - 1; x++) {

            v1 = SubtractVertex(&pr[y * BEACH_GRID_DIM_X + x + 1], 
                                &pr[y * BEACH_GRID_DIM_X + x]);
            v2 = SubtractVertex(&pr[(y + 1) * BEACH_GRID_DIM_X + x], 
                                &pr[y * BEACH_GRID_DIM_X + x]);
            D3DXVec3Normalize(&pvFaceNormal[y * (BEACH_GRID_DIM_X - 1) * 2 + x * 2],
                                D3DXVec3Cross(&r, &v1, &v2));

            v1 = SubtractVertex(&pr[y * BEACH_GRID_DIM_X + x + 1], 
                                &pr[(y + 1) * BEACH_GRID_DIM_X + x + 1]);
            v2 = SubtractVertex(&pr[(y + 1) * BEACH_GRID_DIM_X + x], 
                                &pr[(y + 1) * BEACH_GRID_DIM_X + x + 1]);
            D3DXVec3Normalize(&pvFaceNormal[y * (BEACH_GRID_DIM_X - 1) * 2 + x * 2 + 1],
                                D3DXVec3Cross(&r, &v2, &v1));
        }
    }

    // Generate vertex normals by averaging the adjoining face normals
    for (y = 1; y < BEACH_GRID_DIM_Y - 1; y++) {

        for (x = 1; x < BEACH_GRID_DIM_X - 1; x++) {

            v1 = pvFaceNormal[(y - 1) * (BEACH_GRID_DIM_X - 1) * 2 
                                + (x - 1) * 2 + 1];
            v1 += pvFaceNormal[(y - 1) * (BEACH_GRID_DIM_X - 1) * 2 
                                + x * 2];
            v1 += pvFaceNormal[(y - 1) * (BEACH_GRID_DIM_X - 1) * 2 
                                + x * 2 + 1];
            v1 += pvFaceNormal[y * (BEACH_GRID_DIM_X - 1) * 2 + (x - 1) * 2];
            v1 += pvFaceNormal[y * (BEACH_GRID_DIM_X - 1) * 2 
                                + (x - 1) * 2 + 1];
            v1 += pvFaceNormal[y * (BEACH_GRID_DIM_X - 1) * 2 + x * 2];
            v1 = v1 / 6.0f;
            pr[y * BEACH_GRID_DIM_X + x].nx = v1.x;
            pr[y * BEACH_GRID_DIM_X + x].ny = v1.y;
            pr[y * BEACH_GRID_DIM_X + x].nz = v1.z;
        }
    }

    MemFree32(pvFaceNormal);
}

//******************************************************************************
//
// Function:
//
//     GenerateWaveNormals
//
// Description:
//
//     Generate vertex normals for the given wave mesh.
//
// Arguments:
//
//     LPD3DVERTEX pr           - Array of vertices describing the wave mesh.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void GenerateWaveNormals(PHHVERTEX pr, float* m_pfSqrtLookup) {

    D3DXVECTOR3 *pvFaceNormal, *pv;
    D3DXVECTOR3 *pv1, *pv2, *pv3, *pv4, *pv5, *pv6;
    PHHVERTEX   prn;
    D3DXVECTOR3 v1, v2, v3, v4, v5, v6;
    float       f1, f2, fMag;
    UINT        x, y, uBase, uBase2, uBase3, x2;

    pvFaceNormal = (D3DXVECTOR3*)MemAlloc32(((WAVE_GRID_DIM_Y - 1) 
                                    * (WAVE_GRID_DIM_X - 1) * 2) * sizeof(D3DXVECTOR3));
    if (!pvFaceNormal) {
        return;
    }

    v1.x = WAVE_UNIT_LENGTH_X;
    v1.z = 0.0f;
    v2.x = 0.0f;
    v2.z = -WAVE_UNIT_LENGTH_Y;
    v3.x = 0.0f;
    v3.z = WAVE_UNIT_LENGTH_Y;
    v4.x = -WAVE_UNIT_LENGTH_X;
    v4.z = 0.0f;
    v5.y = v1.z * v2.x - v1.x * v2.z;
    v6.y = v4.z * v3.x - v4.x * v3.z;
    f1 = v5.y * v5.y;
    f2 = v6.y * v6.y;

    // Generate face normals for the triangles
    for (y = 0; y < WAVE_GRID_DIM_Y - 1; y++) {

        uBase = y * WAVE_GRID_DIM_X;
        uBase2 = y * (WAVE_GRID_DIM_X - 1) * 2;

        for (x = 0, x2 = 0; x < WAVE_GRID_DIM_X - 1; x++, x2 += 2) {

            v1.y = pr[uBase + x + 1].y - pr[uBase + x].y;
            v2.y = pr[uBase + WAVE_GRID_DIM_X + x].y - pr[uBase + x].y;
            v5.x = v1.y * v2.z - v1.z * v2.y; 
            v5.z = v1.x * v2.y - v2.y * v1.x;
            pv = &pvFaceNormal[uBase2 + x2];
            fMag = 6.0f * m_pfSqrtLookup[(UINT)(((v5.x * v5.x + f1 + v5.z * v5.z) 
                        - 0.000368) / 0.000132 * SQRT_TABLE_SIZE)];
            pv->x = v5.x / fMag;
            pv->y = v5.y / fMag;
            pv->z = v5.z / fMag;

            v3.y = pr[uBase + x + 1].y - pr[uBase + WAVE_GRID_DIM_X + x + 1].y;
            v4.y = pr[uBase + WAVE_GRID_DIM_X + x].y 
                        - pr[uBase + WAVE_GRID_DIM_X + x + 1].y;
            v6.x = v4.y * v3.z - v4.z * v3.y;
            v6.z = v4.x * v3.y - v4.y * v3.x;
            pv = &pvFaceNormal[uBase2 + x2 + 1];
            fMag = 6.0f * m_pfSqrtLookup[(UINT)(((v6.x * v6.x + f2 + v6.z * v6.z) 
                        - 0.000368) / 0.000132 * SQRT_TABLE_SIZE)];
            pv->x = v6.x / fMag;
            pv->y = v6.y / fMag;
            pv->z = v6.z / fMag;
        }
    }

    // Generate vertex normals by averaging the adjoining face normals
    for (y = 1; y < WAVE_GRID_DIM_Y - 1; y++) {

        uBase = y * (WAVE_GRID_DIM_X - 1) * 2;
        uBase2 = (y - 1) * (WAVE_GRID_DIM_X - 1) * 2;
        uBase3 = y * WAVE_GRID_DIM_X;

        for (x = 1, x2 = 2; x < WAVE_GRID_DIM_X - 1; x++, x2 += 2) {

            pv1 = &pvFaceNormal[uBase2 + x2 - 1];
            pv2 = &pvFaceNormal[uBase2 + x2];
            pv3 = &pvFaceNormal[uBase2 + x2 + 1];
            pv4 = &pvFaceNormal[uBase + x2 - 2];
            pv5 = &pvFaceNormal[uBase + x2 - 1];
            pv6 = &pvFaceNormal[uBase + x2];
            prn = &pr[uBase3 + x];
            prn->nx = pv1->x + pv2->x + pv3->x + pv4->x + pv5->x + pv6->x;
            prn->ny = pv1->y + pv2->y + pv3->y + pv4->y + pv5->y + pv6->y;
            prn->nz = pv1->z + pv2->z + pv3->z + pv4->z + pv5->z + pv6->z;
        }
    }

    MemFree32(pvFaceNormal);
}

//******************************************************************************
//
// Function:
//
//     ExhibitScene
//
// Description:
//
//     Create the scene, pump messages, process user input,
//     update the scene, render the scene, and release the scene when finished.
//
// Arguments:
//
//     CDisplay* pDisplay           - Pointer to the Display object
//
//     int* pnExitCode              - Optional pointer to an integer that will
//                                    be set to the exit value contained in the 
//                                    wParam parameter of the WM_QUIT message 
//                                    (if received)
//
// Return Value:
//
//     TRUE if the display remains functional on exit, FALSE otherwise.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ExhibitScene)(CDisplay* pDisplay, int* pnExitCode) {

    CHighTide*   pHighTide;
    BOOL         bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

#ifdef UNDER_XBOX
//    pDisplay->ReleaseDevice();
//    pDisplay->GetDirect3D8()->SetPushBufferSize(8388608, 32768);
//    pDisplay->CreateDevice();
#endif

    // Create the scene
    pHighTide = new CHighTide();
    if (!pHighTide) {
        return FALSE;
    }

    // Initialize the scene
    if (!pHighTide->Create(pDisplay)) {
        pHighTide->Release();
        return FALSE;
    }

    bRet = pHighTide->Exhibit(pnExitCode);

    // Clean up the scene
    pHighTide->Release();

#ifdef UNDER_XBOX
//    pDisplay->ReleaseDevice();
//    pDisplay->GetDirect3D8()->SetPushBufferSize(524288, 32768);
//    pDisplay->CreateDevice();
#endif

    return bRet;
}

//******************************************************************************
//
// Function:
//
//     ValidateDisplay
//
// Description:
//
//     Evaluate the given display information in order to determine whether or
//     not the display is capable of rendering the scene.  If not, the given 
//     display will not be included in the display list.
//
// Arguments:
//
//     CDirect3D8* pd3d                 - Pointer to the Direct3D object
//
//     D3DCAPS8* pd3dcaps               - Capabilities of the device
//
//     D3DDISPLAYMODE*                  - Display mode into which the device
//                                        will be placed
//
// Return Value:
//
//     TRUE if the scene can be rendered using the given display, FALSE if
//     it cannot.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ValidateDisplay)(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    return TRUE;
}

//******************************************************************************
// CHighTide
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CHighTide
//
// Description:
//
//     Initialize the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CHighTide::CHighTide() {

    m_fRotationDelta = 0.0f;
    m_fElevationDelta = 0.0f;
    m_bRotateLight = TRUE;

    m_pfBaseAngle = NULL;
    m_prCliff = NULL;
    m_pwCliff = NULL;
    m_prBeach = NULL;
    m_pwBeach = NULL;
    m_pws = NULL;
//    m_prWave = NULL;
    m_pd3drWave = NULL;
    m_pwWave = NULL;

    m_pd3dtBeach = NULL;
    m_pd3dtBackground = NULL;

    memset(&m_lhLighthouse, 0, sizeof(LIGHTHOUSE));
    memset(&m_rvLightCone, 0, sizeof(REVOLUTION));
}

//******************************************************************************
//
// Method:
//
//     ~CHighTide
//
// Description:
//
//     Clean up the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CHighTide::~CHighTide() {

    if (m_pfBaseAngle) {
        MemFree(m_pfBaseAngle);
    }
    if (m_prCliff) {
        MemFree32((LPBYTE)m_prCliff);
    }
    if (m_pwCliff) {
        MemFree(m_pwCliff);
    }
    if (m_prBeach) {
        MemFree32((LPBYTE)m_prBeach);
    }
    if (m_pwBeach) {
        MemFree(m_pwBeach);
    }
    if (m_pws) {
        MemFree(m_pws);
    }
//    if (m_prWave) {
//        MemFree32((LPBYTE)m_prWave);
//    }
    if (m_pwWave) {
        MemFree(m_pwWave);
    }
}

//******************************************************************************
//
// Method:
//
//     Create
//
// Description:
//
//     Prepare the test for rendering.
//
// Arguments:
//
//     CDisplay* pDisplay               - Pointer to a Display object.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CHighTide::Create(CDisplay* pDisplay) {

    UINT i;

    m_uNumCliffVertices = CLIFF_GRID_DIM_Y * CLIFF_GRID_DIM_X;
    m_prCliff = (PHHVERTEX)MemAlloc32(m_uNumCliffVertices * sizeof(HHVERTEX));
    if (!m_prCliff) {
        return FALSE;
    }
    m_uNumCliffIndices = (CLIFF_GRID_DIM_Y - 1) * (CLIFF_GRID_DIM_X + 1) * 2 - 2;
    m_pwCliff = (LPWORD)MemAlloc(((CLIFF_GRID_DIM_Y - 1) * (CLIFF_GRID_DIM_X + 1) * 2) * sizeof(WORD));
    if (!m_pwCliff) {
        return FALSE;
    }
    m_uNumBeachVertices = BEACH_GRID_DIM_Y * BEACH_GRID_DIM_X;
    m_prBeach = (PHHVERTEX)MemAlloc32(m_uNumBeachVertices * sizeof(HHVERTEX));
    if (!m_prBeach) {
        return FALSE;
    }
    m_uNumBeachIndices = (BEACH_GRID_DIM_Y - 1) * (BEACH_GRID_DIM_X + 1) * 2 - 2;
    m_pwBeach = (LPWORD)MemAlloc(((BEACH_GRID_DIM_Y - 1) * (BEACH_GRID_DIM_X + 1) * 2) * sizeof(WORD));
    if (!m_pwBeach) {
        return FALSE;
    }
    m_pws = (PWAVESOURCE)MemAlloc(NUM_WAVE_SOURCES * sizeof(WAVESOURCE));
    if (!m_pws) {
        return FALSE;
    }
    m_uNumWaveVertices = WAVE_GRID_DIM_Y * WAVE_GRID_DIM_X;
//    m_prWave = (PHHVERTEX)MemAlloc32(m_uNumWaveVertices * sizeof(HHVERTEX));
//    if (!m_prWave) {
//        return FALSE;
//    }
    m_uNumWaveIndices = (WAVE_GRID_DIM_Y - 1) * (WAVE_GRID_DIM_X + 1) * 2 - 2;
    m_pwWave = (LPWORD)MemAlloc(((WAVE_GRID_DIM_Y - 1) * (WAVE_GRID_DIM_X + 1) * 2) * sizeof(WORD));
    if (!m_pwWave) {
        return FALSE;
    }
    m_pfBaseAngle = (float*)MemAlloc((WAVE_GRID_DIM_Y * WAVE_GRID_DIM_X 
                                * NUM_WAVE_SOURCES) * sizeof(float));
    if (!m_pfBaseAngle) {
        return FALSE;
    }

    // Initialize the sin lookup table
    for (i = 0; i < 1023; i++) {
        m_pfSinLookup[i] = (float)(sin((float)(i) / 1023.0f * M_2PI));
    }
    m_pfSinLookup[1023] = m_pfSinLookup[0];

    // Intialize the sqrt lookup table
    // NOTE: This table only works for sqrt parameter values 
    // between 0.000368 - 0.0005
    for (i = 0; i < SQRT_TABLE_SIZE; i++) {
        m_pfSqrtLookup[i] = (float)(sqrt((float)(i) / (float)(SQRT_TABLE_SIZE) 
                                    * 0.000132 + 0.000368));
    }

    return CScene::Create(pDisplay);
}

//******************************************************************************
//
// Method:
//
//     Exhibit
//
// Description:
//
//     Execute the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     The exit value contained in the wParam parameter of the WM_QUIT message.
//
//******************************************************************************
int CHighTide::Exhibit(int *pnExitCode) {

    return CScene::Exhibit(pnExitCode);
}

//******************************************************************************
//
// Method:
//
//     Setup
//
// Description:
//
//     Obtain the device interface pointer from the display, save the current
//     state of the device, and initialize the background vertices to the
//     dimensions of the render target.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the scene was successfully prepared for initialization, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CHighTide::Setup() {

    return CScene::Setup();
}

//******************************************************************************
//
// Method:
//
//     Initialize
//
// Description:
//
//     Initialize the device and all device objects to be used in the test (or
//     at least all device resource objects to be created in the video, 
//     non-local video, or default memory pools).
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the test was successfully initialized for rendering, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CHighTide::Initialize() {

    D3DXVECTOR3 vLightDir;
    D3DXVECTOR3 vDistance, vPoint;
    float       fDistance;
    UINT        i, x, y;
    HHVERTEX*   prWave;

    // Intialize the lighthouse
    if (!CreateLighthouse(m_pDevice, &m_lhLighthouse)) {
        return FALSE;
    }

    // Initialize the light cone
    if (!CreateRevolution(&m_rvLightCone, 20.0f, 1, 32, LightConeRadius)) {
        return FALSE;
    }
    for (i = 0; i < m_rvLightCone.uCenterVertices; i++) {
        m_rvLightCone.prCenter[i].vPosition.y -= 20.0f;
    }

    // Rotate the light cone: angle it toward the water 
    m_fLightElevation = LIGHT_CONE_ANGLE;
    m_fLightRotation = LIGHT_ROTATION_DELTA1;

    InitMatrix(&m_mLightCone,
        (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( cos(m_fLightElevation)), 
        (float)( sin(m_fLightElevation)), (float)( 0.0f),
        (float)( 0.0f), (float)(-sin(m_fLightElevation)), 
        (float)( cos(m_fLightElevation)), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );

    InitMatrix(&m_mConeRotation,
        (float)( cos(m_fLightRotation)), (float)( 0.0f), 
        (float)(-sin(m_fLightRotation)), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( sin(m_fLightRotation)), (float)( 0.0f), 
        (float)( cos(m_fLightRotation)), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );

    InitMatrix(&m_mConePosition,
        (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 1.0f), (float)( 0.0f),
        (float)(-1.0f), (float)( 2.85f), (float)( 0.0f), (float)( 1.0f)
    );

    SetMaterial(&m_matLightCone, RGB_MAKE(255, 255, 255), 
                                    RGB_MAKE(0, 0, 0), RGB_MAKE(196, 196, 196), 
                                    RGB_MAKE(255, 255, 255), 10.0f);
    SetMaterialTranslucency(&m_matLightCone, 0.15f);

    // Create the lighthouse spotlight
    vLightDir = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
    D3DXVec3TransformCoord(&vLightDir, &vLightDir, &m_mLightCone);
    SetSpotLight(&m_light, D3DXVECTOR3(-1.0f, 2.85f, 0.0f), 
                                     vLightDir, RGB_MAKE(255, 255, 255), 
                                     D3DLIGHT_RANGE_MAX, 1.0f, 1.0f, 0.0f, 0.0f, 
                                     M_PI / 22.0f, M_PI / 19.0f);
    m_fMaxIntensity = -vLightDir.z;

    // Initialize the cliff vertices
    for (y = 0; y < CLIFF_GRID_DIM_Y; y++) {

        for (x = 0; x < CLIFF_GRID_DIM_X; x++) {

            m_prCliff[y * CLIFF_GRID_DIM_X + x] = HHVERTEX(D3DXVECTOR3(3.5f 
                            + (float)(x) / (float)(CLIFF_GRID_DIM_X) 
                           * CLIFF_GRID_WIDTH - (CLIFF_GRID_WIDTH / 2.0f), 
                           (CLIFF_GRID_HEIGHT - 
                           ((float)(y) / (float)(CLIFF_GRID_DIM_Y)
                           * CLIFF_GRID_HEIGHT))  - CLIFF_GRID_OFFSET / 2.0f 
                           + RND() * CLIFF_GRID_OFFSET, 
                           0.5f - BEACH_GRID_HEIGHT - CLIFF_GRID_OFFSET / 2.0f 
                           + RND() * CLIFF_GRID_OFFSET), 
                           D3DXVECTOR3(0.0f, 0.0f, -1.0f), 
                           (float)(x) / (float)(CLIFF_GRID_DIM_X),
                           (float)(y) / (float)(CLIFF_GRID_DIM_Y));
        }
    }

    GenerateCliffNormals(&m_prCliff[0]);

    // Initialize the cliff indices
    for (y = 0; y < CLIFF_GRID_DIM_Y - 1; y++) {

        for (x = 0; x < CLIFF_GRID_DIM_X; x++) {

            m_pwCliff[y * (CLIFF_GRID_DIM_X + 1) * 2 + x * 2 + 1] = 
                            (y + 1) * CLIFF_GRID_DIM_X + x;
            m_pwCliff[y * (CLIFF_GRID_DIM_X + 1) * 2 + x * 2] = 
                            y * CLIFF_GRID_DIM_X + x;
        }

        m_pwCliff[y * (CLIFF_GRID_DIM_X + 1) * 2 + CLIFF_GRID_DIM_X * 2] = 
                            y * CLIFF_GRID_DIM_X + CLIFF_GRID_DIM_X - 1;
        m_pwCliff[y * (CLIFF_GRID_DIM_X + 1) * 2 + CLIFF_GRID_DIM_X * 2 + 1] = 
                            (y + 2) * CLIFF_GRID_DIM_X;
    }

    // Initialize the beach vertices
    for (y = 0; y < BEACH_GRID_DIM_Y; y++) {

        for (x = 0; x < BEACH_GRID_DIM_X; x++) {

            m_prBeach[y * BEACH_GRID_DIM_X + x] = HHVERTEX(D3DXVECTOR3(3.5f 
                            + (float)(x) / (float)(BEACH_GRID_DIM_X) 
                           * BEACH_GRID_WIDTH - (BEACH_GRID_WIDTH / 2.0f), 
                           ((float)(y) / (float)(BEACH_GRID_DIM_Y)
                           * BEACH_GRID_DEPTH) - BEACH_GRID_OFFSET / 2.0f 
                           + RND() * BEACH_GRID_OFFSET, 
                           0.5f - ((float)(y) / (float)(BEACH_GRID_DIM_Y)
                           * BEACH_GRID_HEIGHT)), 
                           D3DXVECTOR3(0.0f, 1.0f, 0.0f), 
                           (float)(x) / (float)(BEACH_GRID_DIM_X),
                           (float)(y) / (float)(BEACH_GRID_DIM_Y));
        }
    }

    GenerateBeachNormals(&m_prBeach[0]);

    // Initialize the beach indices
    for (y = 0; y < BEACH_GRID_DIM_Y - 1; y++) {

        for (x = 0; x < BEACH_GRID_DIM_X; x++) {

            m_pwBeach[y * (BEACH_GRID_DIM_X + 1) * 2 + x * 2] = 
                            (y + 1) * BEACH_GRID_DIM_X + x;
            m_pwBeach[y * (BEACH_GRID_DIM_X + 1) * 2 + x * 2 + 1] = 
                            y * BEACH_GRID_DIM_X + x;
        }

        m_pwBeach[y * (BEACH_GRID_DIM_X + 1) * 2 + BEACH_GRID_DIM_X * 2] = 
                            y * BEACH_GRID_DIM_X + BEACH_GRID_DIM_X - 1;
        m_pwBeach[y * (BEACH_GRID_DIM_X + 1) * 2 + BEACH_GRID_DIM_X * 2 + 1] = 
                            (y + 2) * BEACH_GRID_DIM_X;
    }

    // Create a white material
    SetMaterial(&m_matWhite, RGB_MAKE(255, 255, 255),
                                RGB_MAKE(16, 16, 16), RGB_MAKE(128, 128, 128), 
                                RGB_MAKE(0, 0, 0), (float)(10.0f));

    // Initialize the wave sources
    InitWaveSources(m_pws);

    // Create the wave vertex buffer
    m_pd3drWave = CreateVertexBuffer(m_pDevice, NULL, m_uNumWaveVertices * sizeof(HHVERTEX), 0, FVF_HHVERTEX);
    if (!m_pd3drWave) {
        return FALSE;
    }

    m_pd3drWave->Lock(0, 0, (LPBYTE*)&prWave, 0);

    // Initialize the wave vertices
    for (y = 0; y < WAVE_GRID_DIM_Y; y++) {

        for (x = 0; x < WAVE_GRID_DIM_X; x++) {

            prWave[y * WAVE_GRID_DIM_X + x] = HHVERTEX(D3DXVECTOR3((float)(x) 
                            / (float)(WAVE_GRID_DIM_X) 
                           * WAVE_GRID_WIDTH - (WAVE_GRID_WIDTH / 2.0f), 
                           0.0f, 
                           WAVE_GRID_HEIGHT 
                           - ((float)(y) / (float)(WAVE_GRID_DIM_Y)
                           * WAVE_GRID_HEIGHT)), 
                           D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
        }
    }

    // Initialize the wave indices
    for (y = 0; y < WAVE_GRID_DIM_Y - 1; y++) {

        for (x = 0; x < WAVE_GRID_DIM_X; x++) {

            m_pwWave[y * (WAVE_GRID_DIM_X + 1) * 2 + x * 2] = 
                            (y + 1) * WAVE_GRID_DIM_X + x;
            m_pwWave[y * (WAVE_GRID_DIM_X + 1) * 2 + x * 2 + 1] = 
                            y * WAVE_GRID_DIM_X + x;
        }

        m_pwWave[y * (WAVE_GRID_DIM_X + 1) * 2 + WAVE_GRID_DIM_X * 2] = 
                            y * WAVE_GRID_DIM_X + WAVE_GRID_DIM_X - 1;
        m_pwWave[y * (WAVE_GRID_DIM_X + 1) * 2 + WAVE_GRID_DIM_X * 2 + 1] = 
                            (y + 2) * WAVE_GRID_DIM_X;
    }

    // Initialize the wave base angles
    for (y = 0; y < WAVE_GRID_DIM_Y; y++) {

        for (x = 0; x < WAVE_GRID_DIM_X; x++) {

            for (i = 0; i < NUM_WAVE_SOURCES; i++) {
                
                vPoint = D3DXVECTOR3(prWave[y * WAVE_GRID_DIM_X + x].x, 0.0f, 
                            prWave[y * WAVE_GRID_DIM_X + x].z);
                vDistance = vPoint - m_pws[i].vPos;
                fDistance = D3DXVec3Length(&vDistance);
                while (fDistance > m_pws[i].fWavelength) {
                    fDistance -= m_pws[i].fWavelength;
                }
                m_pfBaseAngle[y * WAVE_GRID_DIM_X * NUM_WAVE_SOURCES 
                            + x * NUM_WAVE_SOURCES + i] = 
                            M_2PI * (fDistance / m_pws[i].fWavelength);
            }
        }
    }

    m_pd3drWave->Unlock();

    // Create a wave material
    SetMaterial(&m_matWave, RGB_MAKE(0, 0, 128), 
                                RGB_MAKE(0, 0, 16), RGB_MAKE(255, 255, 255), 
                                RGB_MAKE(0, 0, 0), (float)(40.0f));

    // Create a background texture
    m_pd3dtBackground = (CTexture8*)CreateTexture(m_pDevice, TEXT("stars.bmp"));
    if (!m_pd3dtBackground) {
        return FALSE;
    }

    // Create a texture
    m_pd3dtBeach = (CTexture8*)CreateTexture(m_pDevice, TEXT("beach.bmp"));
    if (!m_pd3dtBeach) {
        return FALSE;
    }

    if (!SetDirectionalLight(m_pDevice, 1, D3DXVECTOR3(-0.1f, -1.0f, 1.0f), RGB_MAKE(128, 128, 128))) {
        return FALSE;
    }

    m_pDevice->LightEnable(0, TRUE);

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     Efface
//
// Description:
//
//     Release all device resource objects (or at least those objects created
//     in video memory, non-local video memory, or the default memory pools)
//     and restore the device to its initial state.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CHighTide::Efface() {

    ReleaseLighthouse(&m_lhLighthouse);
    ReleaseRevolution(&m_rvLightCone);
    ReleaseTexture(m_pd3dtBeach);
    ReleaseTexture(m_pd3dtBackground);
    ReleaseVertexBuffer(m_pd3drWave);

    CScene::Efface();
}

//******************************************************************************
//
// Method:
//
//     Update
//
// Description:
//
//     Update the state of the scene to coincide with the given time.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CHighTide::Update() {

    CScene::Update();
}

//******************************************************************************
//
// Method:
//
//     Render
//
// Description:
//
//     Render the test scene.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CHighTide::Render() {

    PHHVERTEX   prWave;
    D3DXVECTOR3 r;
    D3DXMATRIX  m;
    float       fAngle;
    float       fHeight;
    float       fIntensity;
    UINT        i, x, y;
    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    HRESULT     hr;

    hr = m_pDevice->TestCooperativeLevel();
    if (FAILED(hr)) {
        if (hr == D3DERR_DEVICELOST) {
            return TRUE;
        }
        if (hr == D3DERR_DEVICENOTRESET) {
            if (!Reset()) {
                return FALSE;
            }
        }
    }

    m_pd3drWave->Lock(0, 0, (LPBYTE*)&prWave, 0);

    // Update the position of the waves
    for (y = 0; y < WAVE_GRID_DIM_Y; y++) {

        for (x = 0; x < WAVE_GRID_DIM_X; x++) {

            fHeight = 0.0f;

            for (i = 0; i < NUM_WAVE_SOURCES; i++) {

                fAngle = (m_pfBaseAngle[y * WAVE_GRID_DIM_X * NUM_WAVE_SOURCES 
                            + x * NUM_WAVE_SOURCES + i] + m_pws[i].fPhase);
                fHeight += m_pws[i].fAmplitude 
                            * m_pfSinLookup[(UINT)((fAngle < 0.0f?
                            (fAngle+M_2PI):fAngle) / M_2PI * 1023.0)];
            }

            prWave[y * WAVE_GRID_DIM_X + x].y = fHeight;
        }
    }

    // Update wave normals
    GenerateWaveNormals(&prWave[0], m_pfSqrtLookup);

    m_pd3drWave->Unlock();

    // Update phases
    for (i = 0; i < NUM_WAVE_SOURCES; i++) {

        m_pws[i].fPhase -= m_pws[i].fFrequency;

        if (m_pws[i].fPhase < -M_2PI) {
            m_pws[i].fPhase += M_2PI;
        }
    }

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    m_mLightCone._22 = (float)(cos(m_fLightElevation));
    m_mLightCone._23 = (float)(sin(m_fLightElevation));
    m_mLightCone._32 = -m_mLightCone._23;
    m_mLightCone._33 = m_mLightCone._22;

    m_mConeRotation._11 = (float)(cos(m_fLightRotation));
    m_mConeRotation._13 = (float)(-sin(m_fLightRotation));
    m_mConeRotation._31 = -m_mConeRotation._13;
    m_mConeRotation._33 = m_mConeRotation._11;

    // Update the spotlight position
    D3DXVec3TransformCoord((D3DXVECTOR3*)&m_light.Direction, D3DXVec3TransformCoord(&r, &D3DXVECTOR3(0.0f, -1.0f, 0.0f), &m_mLightCone), 
                            &m_mConeRotation);

    m_pDevice->SetLight(0, &m_light);

    // Begin the scene
    m_pDevice->BeginScene();

    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->SetTexture(0, m_pd3dtBackground);

    // Draw the background
    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_HHVERTEX);

    // Render the cliffs
    m_pDevice->SetTransform(D3DTS_WORLD, &m_mIdentity);
    m_pDevice->SetMaterial(&m_matWhite);
    m_pDevice->SetTexture(0, m_pd3dtBeach);

    hr = m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, 
            m_uNumCliffVertices, m_uNumCliffIndices - 2, 
            &m_pwCliff[0], D3DFMT_INDEX16, &m_prCliff[0], sizeof(HHVERTEX));
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawIndexedPrimitiveUP"))) {
        return FALSE;
    }

    // Render the beach
    hr = m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, 
            m_uNumBeachVertices, m_uNumBeachIndices - 2, 
            &m_pwBeach[0], D3DFMT_INDEX16, &m_prBeach[0], sizeof(HHVERTEX));
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawIndexedPrimitiveUP"))) {
        return FALSE;
    }

    // Render the lighthouse
    fIntensity = m_light.Direction.z;
    if (fIntensity > 0.0f) {
        fIntensity = fIntensity / m_fMaxIntensity;
    }
    else {
        fIntensity = 0.0f;
    }
    if (!RenderLighthouse(m_pDevice, &m_lhLighthouse, fIntensity)) {
        return FALSE;
    }

    // Render the waves
    m_pDevice->SetTransform(D3DTS_WORLD, &m_mIdentity);
    m_pDevice->SetMaterial(&m_matWave);
    m_pDevice->SetTexture(0, NULL);

#ifndef UNDER_XBOX
    m_pd3drWave->Lock(0, 0, (LPBYTE*)&prWave, 0);

    hr = m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, 
            m_uNumWaveVertices, m_uNumWaveIndices - 2, 
            &m_pwWave[0], D3DFMT_INDEX16, &prWave[0], sizeof(HHVERTEX));

    m_pd3drWave->Unlock();

    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawIndexedPrimitive"))) {
        return FALSE;
    }
#else

    m_pDevice->SetStreamSource(0, m_pd3drWave, sizeof(HHVERTEX));

    hr = m_pDevice->DrawIndexedVertices(D3DPT_TRIANGLESTRIP, m_uNumWaveIndices, 
            &m_pwWave[0]);

    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawIndexedVertices"))) {
        return FALSE;
    }
#endif

    // Simulate the light volume for the lighthouse spotlight
    D3DXMatrixMultiply(&m_mLightDirection, &m_mLightCone, &m_mConeRotation);
    D3DXMatrixMultiply(&m, &m_mLightDirection, &m_mConePosition);
    m_pDevice->SetTransform(D3DTS_WORLD, &m);
    m_pDevice->SetMaterial(&m_matLightCone);

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    hr = m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 
            m_rvLightCone.uCenterVertices, m_rvLightCone.uCenterIndices / 3, 
            m_rvLightCone.pwCenter, D3DFMT_INDEX16, m_rvLightCone.prCenter, sizeof(HHVERTEX));
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawIndexedPrimitiveUP"))) {
        return FALSE;
    }

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // Fade out the scene on exit
    if (m_pfnFade) {
        (this->*m_pfnFade)();
    }

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     ProcessInput
//
// Description:
//
//     Process user input for the scene.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CHighTide::ProcessInput() {

    KEYSDOWN        kdKeys;
    static KEYSDOWN kdLast = 0;

    CScene::ProcessInput();

    kdKeys = m_pDisplay->GetKeyState();

    if ((kdKeys & KEY_LEFT) && !(kdLast & KEY_LEFT)) {
        m_fRotationDelta = LIGHT_ROTATION_DELTA2;
        m_bRotateLight = FALSE;
    }
    else if (!(kdKeys & KEY_LEFT) && (kdLast & KEY_LEFT)) {
        m_fRotationDelta = 0.0f;
    }
    if ((kdKeys & KEY_RIGHT) && !(kdLast & KEY_RIGHT)) {
        m_fRotationDelta = -LIGHT_ROTATION_DELTA2;
        m_bRotateLight = FALSE;
    }
    else if (!(kdKeys & KEY_RIGHT) && (kdLast & KEY_RIGHT)) {
        m_fRotationDelta = 0.0f;
    }
    if ((kdKeys & KEY_UP) && !(kdLast & KEY_UP)) {
        m_fElevationDelta = LIGHT_ELEVATION_DELTA;
    }
    else if (!(kdKeys & KEY_UP) && (kdLast & KEY_UP)) {
        m_fElevationDelta = 0.0f;
    }
    if ((kdKeys & KEY_DOWN) && !(kdLast & KEY_DOWN)) {
        m_fElevationDelta = -LIGHT_ELEVATION_DELTA;
    }
    else if (!(kdKeys & KEY_DOWN) && (kdLast & KEY_DOWN)) {
        m_fElevationDelta = 0.0f;
    }
//    if (kdKeys & KEY_SPACE) {
//        m_bRotateLight = TRUE;
//    }

    kdLast = kdKeys;

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_X)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_X)) {
            m_bRotateLight = !m_bRotateLight;
        }
    }

    // Left keypad
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_LEFT)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_LEFT)) {
            m_fRotationDelta = LIGHT_ROTATION_DELTA2;
            m_bRotateLight = FALSE;
        }
        else {
            m_fRotationDelta = 0.0f;
        }
    }

    // Right keypad
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_RIGHT)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_RIGHT)) {
            m_fRotationDelta = -LIGHT_ROTATION_DELTA2;
            m_bRotateLight = FALSE;
        }
        else {
            m_fRotationDelta = 0.0f;
        }
    }

    // Up keypad
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_UP)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_UP)) {
            m_fElevationDelta = LIGHT_ELEVATION_DELTA;
        }
        else {
            m_fElevationDelta = 0.0f;
        }
    }

    // Down keypad
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_DOWN)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_DOWN)) {
            m_fElevationDelta = -LIGHT_ELEVATION_DELTA;
        }
        else {
            m_fElevationDelta = 0.0f;
        }
    }

    if (m_bRotateLight) {
        m_fRotationDelta = LIGHT_ROTATION_DELTA1;
    }

    m_fLightRotation += m_fRotationDelta;
    if (m_fLightRotation > M_2PI) {
        m_fLightRotation -= M_2PI;
    }
    else if (m_fLightRotation < 0) {
        m_fLightRotation += M_2PI;
    }

    m_fLightElevation += m_fElevationDelta;
    if (m_fLightElevation < M_PIDIV4) {
        m_fLightElevation = M_PIDIV4;
    }
    else if (m_fLightElevation > 1.208305f) {
        m_fLightElevation = 1.208305f;
    }
}

//******************************************************************************
//
// Method:
//
//     InitView
//
// Description:
//
//     Initialize the camera view in the scene.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CHighTide::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(-6.0f, 3.0f, 16.0f);
    m_camInitial.vInterest     = D3DXVECTOR3(-2.5f, 0.5f, 5.0f);
    m_camInitial.fRoll         = 0.0f;
    m_camInitial.fFieldOfView  = M_PI / 4.0f;
    m_camInitial.fNearPlane    = 0.1f;
    m_camInitial.fFarPlane     = 1000.0f;
    m_pDisplay->SetCamera(&m_camInitial);

    return m_pDisplay->SetView(&m_camInitial);
}

//******************************************************************************
// Scene window procedure (pseudo-subclassed off the main window procedure)
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     WndProc
//
// Description:
//
//     Scene window procedure to process messages received by the main 
//     application window.
//
// Arguments:
//
//     LRESULT* plr             - Result of the message processing
//
//     HWND hWnd                - Application window
//
//     UINT uMsg                - Message to process
//
//     WPARAM wParam            - First message parameter
//
//     LPARAM lParam            - Second message parameter
//
// Return Value:
//
//     TRUE if the message was handled, FALSE otherwise.
//
//******************************************************************************
BOOL CHighTide::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;

#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_F12: // Toggle pause state
                    m_bPaused = !m_bPaused;
                    return TRUE;
            }

            break;
    }

#endif // !UNDER_XBOX

    return CScene::WndProc(plr, hWnd, uMsg, wParam, lParam);
}
