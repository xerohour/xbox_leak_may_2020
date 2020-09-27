/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    misc.cpp

Author:

    Matt Bronder

Description:

    Miscellaneous functions.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
//
// Function:
//
//     CheckCurrentDeviceFormat
//
// Description:
//
//     Check if the given format is available in the current device
//     configuration. 
//
// Arguments:
//
//     CDevice8* pDevice        - Pointer to the device object
//
//     DWORD dwUsage            - Usage for the resource
//
//     D3DRESOURCETYPE d3drt    - Type of the resource
//
//     D3DFORMAT fmt            - Format of the resource
//
// Return Value:
//
//     An HRESULT set to D3D_OK if the format is available, D3DERR_NOTAVAILABLE
//     if the format is not available, or the code of the failed method if a 
//     failure occurs.
//
//******************************************************************************
HRESULT CheckCurrentDeviceFormat(CDevice8* pDevice, DWORD dwUsage, D3DRESOURCETYPE d3drt, D3DFORMAT fmt) {

    CDirect3D8*     pDirect3D;
    D3DCAPS8        d3dcaps;
    D3DDISPLAYMODE  d3ddm;
    HRESULT         hr;

    hr = pDevice->GetDeviceCaps(&d3dcaps);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDeviceCaps"))) {
        return hr;
    }

    hr = pDevice->GetDirect3D(&pDirect3D);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDirect3D"))) {
        return hr;
    }

    hr = pDirect3D->GetAdapterDisplayMode(d3dcaps.AdapterOrdinal, &d3ddm);
    if (ResultFailed(hr, TEXT("IDirect3D8::GetAdapterDisplayMode"))) {
        pDirect3D->Release();
        return hr;
    }

    // Check if the current device supports the given format
    hr = pDirect3D->CheckDeviceFormat(d3dcaps.AdapterOrdinal, 
                                    d3dcaps.DeviceType, 
                                    d3ddm.Format, 
                                    dwUsage, d3drt, fmt);

    pDirect3D->Release();

    return hr;
}

//******************************************************************************
BOOL IsCompressedFormat(D3DFORMAT d3dfmt) {

    switch (d3dfmt) {
        case D3DFMT_DXT1:
        case D3DFMT_DXT2:
        case D3DFMT_DXT4:
#ifndef UNDER_XBOX
        case D3DFMT_DXT3:
        case D3DFMT_DXT5:
#endif
            return TRUE;
        default:
            return FALSE;
    }
}

//******************************************************************************
// Vertex transformation routine
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     TransformVertices
//
// Description:
//
//     Transform a set of vertices using the current world, view, and projection
//     matrices set in the device. 
//
// Arguments:
//
//     CDevice8* pDevice        - Pointer to the device object
//
//     LPVOID pvDst (Out)       - A pointer to the first vertex in the set
//                                that will contain the transformed vertices.
//                                (These vertices must consist of a D3DVECTOR
//                                to contain the position followed by a
//                                float to contain the reciprocal of 
//                                homogeneous w)
//
//     LPVOID pvSrc             - A pointer to the first position vector 
//                                in the set of vertices to be transformed
//
//     DWORD dwNumVertices      - Number of vertices to be transformed
//
//     DWORD dwStrideDst        - Size (in bytes) from one destination vertex
//                                to the next
//
//     DWORD dwStrideSrc        - Size (in bytes) from one source vertex
//                                (position vector) to the next
//
//     LPDWORD pdwClip (Out)    - A pointer to an array of DWORDs which, on
//                                return, will contain clip flags for each of
//                                the transformed vertices
//
// Return Value:
//
//     An HRESULT set to D3D_OK if the transformation is successful, or the error
//     code of the failed method if a failure occurs.
//
//******************************************************************************
HRESULT TransformVertices(CDevice8* pDevice, LPVOID pvDst, LPVOID pvSrc, 
                          DWORD dwNumVertices, DWORD dwStrideDst, DWORD dwStrideSrc, 
                          LPDWORD pdwClip) 
{
    D3DXMATRIX              mWorld, mView, mProj, mClip, mR1, mR2;
    D3DVIEWPORT8            viewport;
    UINT                    i;
    HRESULT                 hr;
    float                   fW;
    LPBYTE                  pdst, psrc;
    D3DVECTOR*              prSrc;
    struct _TVERTEX {
        D3DVECTOR v;
        float     fRHW;
    }                       *prDst;

    // Get the viewport dimensions
    memset(&viewport, 0, sizeof(D3DVIEWPORT8));
    hr = pDevice->GetViewport(&viewport);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetViewport"))) {
        return hr;
    }

    // Build a transformation matrix from the current world, view, and
    // projection matrices

    hr = pDevice->GetTransform(D3DTS_PROJECTION, &mProj);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetTransform"))) {
        return hr;
    }

    InitMatrix(&mClip,
         0.5f,  0.0f,  0.0f,  0.0f,
         0.0f,  0.5f,  0.0f,  0.0f,
         0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.0f,  1.0f
    );

    D3DXMatrixMultiply(&mR1, &mProj, &mClip);

    hr = pDevice->GetTransform(D3DTS_VIEW, &mView);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetTransform"))) {
        return hr;
    }

    D3DXMatrixMultiply(&mR2, &mView, &mR1);

    hr = pDevice->GetTransform(D3DTS_WORLD, &mWorld);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetTransform"))) {
        return hr;
    }

    D3DXMatrixMultiply(&mR1, &mWorld, &mR2);

    // Clear the clipping flags
    if (pdwClip) {
        memset(pdwClip, 0, dwNumVertices * sizeof(DWORD));
    }

    // Apply the transformation matrix to the given vertices
    for (i = 0, pdst = (LPBYTE)pvDst, psrc = (LPBYTE)pvSrc; 
         i < dwNumVertices; 
         i++, pdst += dwStrideDst, psrc += dwStrideSrc) 
    {
        prSrc = (D3DVECTOR*)psrc;
        prDst = (struct _TVERTEX*)pdst;

        // Calculate the homogeneous coordinates
        fW = mR1._14 * prSrc->x + mR1._24 * prSrc->y + mR1._34 * prSrc->z + mR1._44;
        prDst->fRHW = 1.0f / fW;
        prDst->v.x = (prSrc->x * mR1._11 + prSrc->y * mR1._21 + prSrc->z * mR1._31 + mR1._41) * prDst->fRHW;
        prDst->v.y = (prSrc->x * mR1._12 + prSrc->y * mR1._22 + prSrc->z * mR1._32 + mR1._42) * prDst->fRHW;
        prDst->v.z = (prSrc->x * mR1._13 + prSrc->y * mR1._23 + prSrc->z * mR1._33 + mR1._43) * prDst->fRHW;

        // Clip test the coordinates
        if (pdwClip) {
            pdwClip[i] |= (prDst->v.x > 0.0f) ? 0 : CLIP_LEFT;
            pdwClip[i] |= (prDst->v.x < fW)   ? 0 : CLIP_RIGHT;
            pdwClip[i] |= (prDst->v.y > 0.0f) ? 0 : CLIP_BOTTOM;
            pdwClip[i] |= (prDst->v.y < fW)   ? 0 : CLIP_TOP;
            pdwClip[i] |= (prDst->v.z > 0.0f) ? 0 : CLIP_FRONT;
            pdwClip[i] |= (prDst->v.z < fW)   ? 0 : CLIP_BACK;
        }

        // Scale and offset x and y into screen coordinates
        prDst->v.x = prDst->v.x *  (float)viewport.Width  + (float)viewport.X;
        prDst->v.y = prDst->v.y * -(float)viewport.Height + (float)viewport.Height + (float)viewport.Y;
    }

    return hr;
}

//******************************************************************************
// Directional lighting function
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     LightVertices
//
// Description:
//
//     Calculate the diffuse and specular lighting intensities for the given
//     vertices using the given material and light.  The lighting information
//     is stored in the vertices, overwriting the normal information.  The
//     light is assumed to be directional and the vertices are assume to be
//     in world space.
//
// Arguments:
//
//     PVERTEX prSrc            - List of vertices to be lit
//
//     PLVERTEX prDst           - Result of the lighting operation
//
//     UINT uNumVertices        - Number of vertices to light
//
//     D3DMATERIAL8* pmaterial  - Material color
//
//     D3DLIGHT8* plight        - Light color and direction
//
//     D3DCOLOR cAmbient        - Ambient color
//
//     D3DXVECTOR3* pvViewPos   - View position
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL LightVertices(PVERTEX prSrc, PLVERTEX prDst, UINT uNumVertices, 
                      D3DMATERIAL8* pmaterial, D3DLIGHT8* plight, 
                      D3DCOLOR cAmbient, D3DXVECTOR3* pvViewPos)
{
    return LightVertices((LPVOID)&prSrc->vNormal, sizeof(VERTEX),
                         &prDst->cDiffuse, sizeof(LVERTEX),
                         uNumVertices, pmaterial, plight,
                         cAmbient, (LPVOID)&prSrc->vPosition, 
                         sizeof(VERTEX), &prDst->cSpecular,
                         sizeof(LVERTEX), pvViewPos);
}

//******************************************************************************
//
// Function:
//
//     LightVertices
//
// Description:
//
//     Calculate the diffuse and specular lighting intensities for the given
//     vertices using the given material and light.  The lighting information
//     is stored in the vertices, overwriting the normal information.  The
//     light is assumed to be directional and the vertices are assume to be
//     in world space.
//
// Arguments:
//
//     LPVOID pvNormal          - Pointer to the first vertex normal component
//                                in the source vertex list
//
//     DWORD dwStrideNormal     - Distance in bytes to the next vertex normal
//
//     LPVOID pcDiffuse         - Pointer to the first diffuse color component
//                                in the destination vertex list
//
//     DWORD dwStrideDiffuse    - Distance in bytes to the next vertex diffuse 
//                                color
//
//     UINT uNumVertices        - Number of vertices to light
//
//     D3DMATERIAL8* pmaterial  - Material color
//
//     D3DLIGHT8* plight        - Light color and direction
//
//     D3DCOLOR cAmbient        - Ambient color
//
//     LPVOID pvPosition        - Pointer to the first vertex position component
//                                in the source vertex list (for specular
//                                lighting.  If NULL specular lighting will not
//                                be performed)
//
//     DWORD dwStridePosition   - Distance in bytes to the next vertex position
//
//     LPVOID pcSpecular        - Pointer to the first specular color component
//                                in the destination vertex list
//
//     DWORD dwStrideSpecular   - Distance in bytes to the next vertex specular
//                                color
//
//     D3DXVECTOR3* pvViewPos   - View position
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL LightVertices(LPVOID pvNormal, DWORD dwStrideNormal,
                   D3DCOLOR* pcDiffuse, DWORD dwStrideDiffuse,
                   UINT uNumVertices, D3DMATERIAL8* pmaterial, 
                   D3DLIGHT8* plight, D3DCOLOR cAmbient, 
                   LPVOID pvPosition, DWORD dwStridePosition,
                   D3DCOLOR* pcSpecular, DWORD dwStrideSpecular,
                   D3DXVECTOR3* pvViewPos)
{
    D3DXVECTOR3  vMDiffuse, vMAmbient, vMSpecular, vMEmissive, vRAmbient, vLDiffuse, 
                 vLSpecular, vLAmbient, vBase, vSBase, vOffset, vLightDir, vNorm, v1, v2;
    D3DXVECTOR3* pvPos, *pvNorm;
    float        fIntensity, fSIntensity;
    UINT         i;

    if (!pvNormal || !pcDiffuse || !pmaterial || !plight) {
        return FALSE;
    }
    if (pvPosition && !(pcSpecular && pvViewPos)) {
        return FALSE;
    }

    vRAmbient.x = ((float)(RGB_GETRED(cAmbient)) / 255.0f);
    vRAmbient.y = ((float)(RGB_GETGREEN(cAmbient)) / 255.0f);
    vRAmbient.z = ((float)(RGB_GETBLUE(cAmbient)) / 255.0f);

    vMDiffuse.x = pmaterial->Diffuse.r;
    vMDiffuse.y = pmaterial->Diffuse.g;
    vMDiffuse.z = pmaterial->Diffuse.b;

    vMAmbient.x = pmaterial->Ambient.r;
    vMAmbient.y = pmaterial->Ambient.g;
    vMAmbient.z = pmaterial->Ambient.b;

    vMSpecular.x = pmaterial->Specular.r;
    vMSpecular.y = pmaterial->Specular.g;
    vMSpecular.z = pmaterial->Specular.b;

    vMEmissive.x = pmaterial->Emissive.r;
    vMEmissive.y = pmaterial->Emissive.g;
    vMEmissive.z = pmaterial->Emissive.b;

    vLDiffuse.x = plight->Diffuse.r;
    vLDiffuse.y = plight->Diffuse.g;
    vLDiffuse.z = plight->Diffuse.b;

    vLAmbient.x = plight->Ambient.r;
    vLAmbient.y = plight->Ambient.g;
    vLAmbient.z = plight->Ambient.b;

    vLSpecular.x = plight->Specular.r;
    vLSpecular.y = plight->Specular.g;
    vLSpecular.z = plight->Specular.b;

    v1 = vRAmbient + vLAmbient;
    v2.x = v1.x * vMAmbient.x;
    v2.y = v1.y * vMAmbient.y;
    v2.z = v1.z * vMAmbient.z;

    vOffset = v2 + vMEmissive;

    vBase.x = vLDiffuse.x * vMDiffuse.x;
    vBase.y = vLDiffuse.y * vMDiffuse.y;
    vBase.z = vLDiffuse.z * vMDiffuse.z;

    vSBase.x = vLSpecular.x * vMSpecular.x;
    vSBase.y = vLSpecular.y * vMSpecular.y;
    vSBase.z = vLSpecular.z * vMSpecular.z;

    D3DXVec3Normalize(&vLightDir, (D3DXVECTOR3*)&plight->Direction);

    for (i = 0, pvPos = (D3DXVECTOR3*)pvPosition, pvNorm = (D3DXVECTOR3*)pvNormal; 
         i < uNumVertices; 
         i++, pvPos = (D3DXVECTOR3*)((LPBYTE)pvPos + dwStridePosition), 
         pvNorm = (D3DXVECTOR3*)((LPBYTE)pvNorm + dwStrideNormal), 
         pcDiffuse = (D3DCOLOR*)((LPBYTE)pcDiffuse + dwStrideDiffuse), 
         pcSpecular = (D3DCOLOR*)((LPBYTE)pcSpecular + dwStrideSpecular)) 
    {
        fIntensity = -D3DXVec3Dot(pvNorm, &vLightDir);
        if (fIntensity < 0.0f) fIntensity = 0.0f;
        vMDiffuse = (fIntensity * vBase + vOffset) * 255.0f;
        if (vMDiffuse.x > 255.0f) vMDiffuse.x = 255.0f;
        if (vMDiffuse.y > 255.0f) vMDiffuse.y = 255.0f;
        if (vMDiffuse.z > 255.0f) vMDiffuse.z = 255.0f;
        *pcDiffuse = RGBA_MAKE((BYTE)vMDiffuse.x, (BYTE)vMDiffuse.y, (BYTE)vMDiffuse.z, 255);
        if (pvPosition) {
            D3DXVec3Normalize(&vNorm, &(*pvPos - *pvViewPos));
            D3DXVec3Normalize(&vNorm, &(vNorm + vLightDir));
            fSIntensity = -D3DXVec3Dot(pvNorm, &vNorm);
            if (fSIntensity < 0.0f) fSIntensity = 0.0f;
            fSIntensity = (float)(pow(fSIntensity, pmaterial->Power));
            vMSpecular = fSIntensity * vSBase * 255.0f;
            if (vMSpecular.x > 255.0f) vMSpecular.x = 255.0f;
            if (vMSpecular.y > 255.0f) vMSpecular.y = 255.0f;
            if (vMSpecular.z > 255.0f) vMSpecular.z = 255.0f;
            *pcSpecular = RGBA_MAKE((BYTE)vMSpecular.x, (BYTE)vMSpecular.y, (BYTE)vMSpecular.z, 255);
        }
    }

    return TRUE;
}

//******************************************************************************
// Matrix utility functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     InitMatrix
//
// Description:
//
//     Initialize the given matrix with the given values.
//
// Arguments:
//
//     LPD3DMATRIX pd3dm        - Matrix to initialize
//
//     float _11 to _44         - Initialization values
//
// Return Value:
//
//     None.
//
//******************************************************************************
void InitMatrix(D3DMATRIX* pd3dm,
                float _11, float _12, float _13, float _14,
                float _21, float _22, float _23, float _24,
                float _31, float _32, float _33, float _34,
                float _41, float _42, float _43, float _44) 
{
    if (pd3dm) {

        pd3dm->_11 = _11;
        pd3dm->_12 = _12;
        pd3dm->_13 = _13;
        pd3dm->_14 = _14;
        pd3dm->_21 = _21;
        pd3dm->_22 = _22;
        pd3dm->_23 = _23;
        pd3dm->_24 = _24;
        pd3dm->_31 = _31;
        pd3dm->_32 = _32;
        pd3dm->_33 = _33;
        pd3dm->_34 = _34;
        pd3dm->_41 = _41;
        pd3dm->_42 = _42;
        pd3dm->_43 = _43;
        pd3dm->_44 = _44;
    }
}

//******************************************************************************
//
// Function:
//
//     SetView
//
// Description:
//
//     Align the given view matrix along the given direction and up vectors with
//     the position vector as the origin.
//
// Arguments:
//
//     LPD3DXMATRIX pd3dm       - View matrix to initialize
//
//     LPD3DXVECTOR3 pvPos      - View origin
//
//     LPD3DXVECTOR3 pvAt       - View interest vector
//
//     LPD3DXVECTOR3 pvUp       - View up vector
//
// Return Value:
//
//     Initialized view matrix.
//
//******************************************************************************
void SetView(D3DXMATRIX* pd3dm, LPD3DXVECTOR3 pvPos, LPD3DXVECTOR3 pvAt, 
                                                      LPD3DXVECTOR3 pvUp) 
{
    D3DXVECTOR3  d, u, r;

    if (pd3dm && pvPos && pvAt && pvUp && !(*pvPos == *pvAt) 
                                 && !(*pvUp == D3DXVECTOR3(0.0f, 0.0f, 0.0f))) {

        D3DXVec3Normalize(&d, &(*pvAt - *pvPos));
        u = *pvUp;

        // Project the up vector into the plane of the direction vector
        D3DXVec3Normalize(&u, &(u - (d * D3DXVec3Dot(&u, &d))));

        // Get the cross product
        D3DXVec3Cross(&r, &u, &d);

        // Initialize the view transform
        InitMatrix(pd3dm,
            r.x, u.x, d.x, 0.0f,
            r.y, u.y, d.y, 0.0f,
            r.z, u.z, d.z, 0.0f,
            -(pvPos->x * r.x + pvPos->y * r.y + pvPos->z * r.z), 
            -(pvPos->x * u.x + pvPos->y * u.y + pvPos->z * u.z), 
            -(pvPos->x * d.x + pvPos->y * d.y + pvPos->z * d.z), 
            1.0f
        );
    }
}

//******************************************************************************
//
// Function:
//
//     SetPerspectiveProjection
//
// Description:
//
//     Initialize the given projection matrix using the given front and back
//     clipping planes, field of view, and aspect ratio.
//
// Arguments:
//
//     LPD3DMATRIX pd3dm        - Projection matrix to initialize
//
//     float fFront             - Front clipping plane
//
//     float fBack              - Back clipping plane
//
//     float fFieldOfView       - Angle, in radians, of the field of view
//
//     float fAspect            - Aspect ratio (y / x) of the view plane
//
// Return Value:
//
//     Initialized projection matrix.
//
//******************************************************************************
void SetPerspectiveProjection(D3DMATRIX* pd3dm, float fFront, float fBack, 
                                    float fFieldOfView, float fAspect) {

    float fTanHalfFOV = (float)tan((double)fFieldOfView / 2.0);
    float fFar = fBack / (fBack - fFront);

    InitMatrix(pd3dm,
        1.0f, 0.0f,           0.0f,                         0.0f,
        0.0f, 1.0f / fAspect, 0.0f,                         0.0f,
        0.0f, 0.0f,           fTanHalfFOV * fFar,           fTanHalfFOV,
        0.0f, 0.0f,           -fFront * fTanHalfFOV * fFar, 0.0f
    );
}

//******************************************************************************
//
// Function:
//
//     SetParallelProjection
//
// Description:
//
//     Initialize the given projection matrix using the given front and back
//     clipping planes, field of view, and aspect ratio.
//
// Arguments:
//
//     LPD3DMATRIX pd3dm        - Projection matrix to initialize
//
//     float fFront             - Front clipping plane
//
//     float fBack              - Back clipping plane
//
//     float fFieldOfView       - Angle, in radians, of the field of view
//
//     float fAspect            - Aspect ratio (y / x) of the view plane
//
// Return Value:
//
//     Initialized projection matrix.
//
//******************************************************************************
void SetParallelProjection(D3DMATRIX* pd3dm, float fFront, float fBack, 
                                    float fWidth, float fHeight) {

    InitMatrix(pd3dm,
        2.0f / fWidth, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / fHeight, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / (fBack - fFront), 0.0f,
        0.0f, 0.0f, fFront / (fFront - fBack), 1.0f
    );
}

//******************************************************************************
BOOL CreateSphere(PVERTEX* pprVertices, LPDWORD pdwNumVertices,
                  LPWORD* ppwIndices, LPDWORD pdwNumIndices,
                  float fRadius, UINT uStepsU, UINT uStepsV,
                  float fTileU, float fTileV) {

    PVERTEX     prVertices;
    PVERTEX     pr;
    DWORD       dwNumVertices;
    LPWORD      pwIndices;
    DWORD       dwNumIndices;
    UINT        uIndex = 0;
    UINT        i, j;
    float       fX, fY, fTX, fSinY, fCosY;

    if (!pprVertices || !pdwNumVertices || !ppwIndices || !pdwNumIndices) {
        return FALSE;
    }

    *pprVertices = NULL;
    *pdwNumVertices = 0;
    *ppwIndices = NULL;
    *pdwNumIndices = 0;

    dwNumVertices = (uStepsU + 1) * uStepsV;

    // Allocate memory for the vertices
    prVertices = (PVERTEX)MemAlloc32(dwNumVertices * sizeof(VERTEX));
    if (!prVertices) {
        return FALSE;
    }

    // Allocate memory for the indices
    dwNumIndices = uStepsU * (uStepsV - 1) * 6;

    pwIndices = (LPWORD)MemAlloc32(dwNumIndices * sizeof(WORD));
    if (!pwIndices) {
        MemFree32(prVertices);
        return FALSE;
    }

    // Create the sphere
    for (j = 0; j < uStepsV; j++) {

        fY = (float)j / (float)(uStepsV - 1);
        fSinY = (float)(sin(fY * M_PI));
        fCosY = (float)(cos(fY * M_PI));

        for (i = 0; i <= uStepsU; i++) {

            pr = &prVertices[(uStepsU + 1) * j + i];
            fX = (float)i / (float)uStepsU;
            fTX = fX * M_2PI;

            pr->vNormal = D3DXVECTOR3((float)cos(fTX) * fSinY, fCosY, (float)sin(fTX) * fSinY);
            pr->vPosition = pr->vNormal * fRadius;
            pr->u0 = fX * fTileU;
            pr->v0 = fY * fTileV;
        }
    }

    for (j = 0; j < uStepsV - 1; j++) {

        for (i = 0; i < uStepsU; i++) {

            pwIndices[uIndex++] = (j + 1) * (uStepsU + 1) + i;
            pwIndices[uIndex++] = j * (uStepsU + 1) + i;
            pwIndices[uIndex++] = (j + 1) * (uStepsU + 1) + i + 1;
            pwIndices[uIndex++] = j * (uStepsU + 1) + i;
            pwIndices[uIndex++] = j * (uStepsU + 1) + i + 1;
            pwIndices[uIndex++] = (j + 1) * (uStepsU + 1) + i + 1;
        }
    }

    *pprVertices = prVertices;
    *pdwNumVertices = dwNumVertices;
    *ppwIndices = pwIndices;
    *pdwNumIndices = dwNumIndices;

    return TRUE;
}

//******************************************************************************
void ReleaseSphere(PVERTEX* ppr, LPWORD* ppw) {

    if (ppr && *ppr) {
        MemFree32(*ppr);
        *ppr = NULL;
    }
    if (ppw && *ppw) {
        MemFree32(*ppw);
        *ppw = NULL;
    }
}

//******************************************************************************
BOOL CaptureTexture(LPDIRECT3DTEXTURE8 pd3dt, LPCTSTR szName, BOOL bTiled) {

    LPDIRECT3DSURFACE8 pd3ds;
    BOOL bRet;
    HRESULT hr;

    hr = pd3dt->GetSurfaceLevel(0, &pd3ds);
    if (FAILED(hr)) {
        return FALSE;
    }

    bRet = CaptureSurface(pd3ds, szName, bTiled);

    pd3ds->Release();

    return bRet;
}

//******************************************************************************
BOOL CaptureSurface(LPDIRECT3DSURFACE8 pd3ds, LPCTSTR szName, BOOL bTiled) {

    BITMAPFILEHEADER    bfh;
    BITMAPINFOHEADER    bmih;
    HANDLE              hFile;
    DWORD               dwWritten;
    D3DSURFACE_DESC     d3dsd;
    D3DLOCKED_RECT      d3dlr;
    UINT                uBPP;
    char                szFile[MAX_PATH];
    UINT                uAlpha, i, j, k;
    LPVOID              pvBuffer;
    DWORD               dwSize;
    struct _COLOR24 {
        BYTE blue;
        BYTE green;
        BYTE red;
    } c, *pc;

    if (!pd3ds || !szName || *szName == TEXT('\0')) {
        return FALSE;
    }

    pd3ds->GetDesc(&d3dsd);

#ifdef UNDER_XBOX
    uBPP = XGBytesPerPixelFromFormat(d3dsd.Format);
#else
    uBPP = FormatToBitDepth(d3dsd.Format) / 8;
#endif
    if (!(uBPP == 2 || uBPP == 4)) {
        OutputDebugString(TEXT("CAPTURETEXTURE: Unsupported texture format"));
        return FALSE;
    }

#ifdef UNDER_XBOX
    Swizzler swz(d3dsd.Width, d3dsd.Height, 1);
#endif

    memset(&bfh, 0, sizeof(BITMAPFILEHEADER));
    memset(&bmih, 0, sizeof(BITMAPINFOHEADER));

    bfh.bfType = 0x4D42;
    bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + d3dsd.Width * d3dsd.Height * 3;
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = d3dsd.Width;
    bmih.biHeight = d3dsd.Height;
    bmih.biPlanes = 1;
    bmih.biBitCount = 24;

#ifndef UNDER_XBOX
    pd3ds->LockRect(&d3dlr, NULL, 0);
#else
    pd3ds->LockRect(&d3dlr, NULL, bTiled ? D3DLOCK_TILED : 0);
#endif

    for (uAlpha = 0; uAlpha < 2; uAlpha++) {

        if (uAlpha && (
#ifdef UNDER_XBOX
            d3dsd.Format == D3DFMT_LIN_X8R8G8B8 ||
            d3dsd.Format == D3DFMT_LIN_R5G6B5 ||
            d3dsd.Format == D3DFMT_LIN_X1R5G5B5 ||
#endif
            d3dsd.Format == D3DFMT_X8R8G8B8 ||
            d3dsd.Format == D3DFMT_R5G6B5 ||
            d3dsd.Format == D3DFMT_X1R5G5B5
        )) {
            break;
        }

        if (!uAlpha) {
            sprintf(szFile, "t:\\%S.bmp", szName);
        }
        else {
            sprintf(szFile, "t:\\%S_alpha.bmp", szName);
        }

        // Create the bitmap file
        hFile = CreateFile(szFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            pd3ds->UnlockRect();
            return FALSE;
        }

        if (!WriteFile(hFile, &bfh, sizeof(BITMAPFILEHEADER), &dwWritten, NULL)) {
            pd3ds->UnlockRect();
            CloseHandle(hFile);
            return FALSE;
        }

        if (!WriteFile(hFile, &bmih, sizeof(BITMAPINFOHEADER), &dwWritten, NULL)) {
            pd3ds->UnlockRect();
            CloseHandle(hFile);
            return FALSE;
        }

        dwSize = d3dsd.Width * d3dsd.Height * sizeof(c);
        pvBuffer = MemAlloc(dwSize);
        if (!pvBuffer) {
            pd3ds->UnlockRect();
            CloseHandle(hFile);
            return FALSE;
        }
        pc = (struct _COLOR24*)pvBuffer;

#ifdef UNDER_XBOX
        if (XGIsSwizzledFormat(d3dsd.Format)) {

            swz.SetU(0);
            swz.SetV(swz.SwizzleV(d3dsd.Height - 1));

            switch (d3dsd.Format) {

                case D3DFMT_X1R5G5B5:
                case D3DFMT_A1R5G5B5: {
                    LPWORD pwTexel = (LPWORD)d3dlr.pBits;
                    WORD wTexel;
                    for (i = 0; i < d3dsd.Height; i++, swz.DecV()) {
                        for (j = 0; j < d3dsd.Width; j++, swz.IncU(), pc++) {
                            wTexel = pwTexel[swz.Get2D()];
                            if (!uAlpha) {
                                pc->red = (BYTE)((wTexel & 0x7C00) >> 7);
                                pc->green = (BYTE)((wTexel & 0x03E0) >> 2);
                                pc->blue = (BYTE)((wTexel & 0x1F) << 3);
                            }
                            else {
                                if (wTexel >> 15) {
                                    pc->red = pc->green = pc->blue = 0xFF;
                                }
                                else {
                                    pc->red = pc->green = pc->blue = 0;
                                }
                            }
                        }
                    }
                    break;
                }

                case D3DFMT_R5G6B5: {
                    LPWORD pwTexel = (LPWORD)d3dlr.pBits;
                    WORD wTexel;
                    for (i = 0; i < d3dsd.Height; i++, swz.DecV()) {
                        for (j = 0; j < d3dsd.Width; j++, swz.IncU(), pc++) {
                            wTexel = pwTexel[swz.Get2D()];
                            pc->red = (BYTE)((wTexel & 0xF800) >> 8);
                            pc->green = (BYTE)((wTexel & 0x07E0) >> 3);
                            pc->blue = (BYTE)((wTexel & 0x1F) << 3);
                        }
                    }
                    break;
                }

                case D3DFMT_X8R8G8B8:
                case D3DFMT_A8R8G8B8: {
                    LPDWORD pdwTexel = (LPDWORD)d3dlr.pBits;
                    DWORD dwTexel;
                    for (i = 0; i < d3dsd.Height; i++, swz.DecV()) {
                        for (j = 0; j < d3dsd.Width; j++, swz.IncU(), pc++) {
                            dwTexel = pdwTexel[swz.Get2D()];
                            if (!uAlpha) {
                                pc->red = (BYTE)((dwTexel >> 16) & 0xFF);
                                pc->green = (BYTE)((dwTexel >> 8) & 0xFF);
                                pc->blue = (BYTE)(dwTexel & 0xFF);
                            }
                            else {
                                pc->red = pc->green = pc->blue = (BYTE)((dwTexel >> 24) & 0xFF);
                            }
                        }
                    }
                }
            }
        }
        else {
#endif
            switch (d3dsd.Format) {

#ifdef UNDER_XBOX
                case D3DFMT_LIN_X1R5G5B5:
                case D3DFMT_LIN_A1R5G5B5: {
#else
                case D3DFMT_X1R5G5B5:
                case D3DFMT_A1R5G5B5: {
#endif
                    LPWORD pwTexel = (LPWORD)d3dlr.pBits + (d3dsd.Height - 1) * (d3dlr.Pitch >> 1);
                    WORD wTexel;
                    for (i = 0; i < d3dsd.Height; i++) {
                        for (j = 0; j < d3dsd.Width; j++, pc++) {
                            wTexel = pwTexel[j];
                            if (!uAlpha) {
                                pc->red = (BYTE)((wTexel & 0x7C00) >> 7);
                                pc->green = (BYTE)((wTexel & 0x03E0) >> 2);
                                pc->blue = (BYTE)((wTexel & 0x1F) << 3);
                            }
                            else {
                                if (wTexel >> 15) {
                                    pc->red = pc->green = pc->blue = 0xFF;
                                }
                                else {
                                    pc->red = pc->green = pc->blue = 0;
                                }
                            }
                        }
                        pwTexel -= d3dlr.Pitch >> 1;
                    }
                    break;
                }

#ifdef UNDER_XBOX
                case D3DFMT_LIN_R5G6B5: {
#else
                case D3DFMT_R5G6B5: {
#endif
                    LPWORD pwTexel = (LPWORD)d3dlr.pBits + (d3dsd.Height - 1) * (d3dlr.Pitch >> 1);
                    WORD wTexel;
                    for (i = 0; i < d3dsd.Height; i++) {
                        for (j = 0; j < d3dsd.Width; j++, pc++) {
                            wTexel = pwTexel[j];
                            pc->red = (BYTE)((wTexel & 0xF800) >> 8);
                            pc->green = (BYTE)((wTexel & 0x07E0) >> 3);
                            pc->blue = (BYTE)((wTexel & 0x1F) << 3);
                        }
                        pwTexel -= d3dlr.Pitch >> 1;
                    }
                    break;
                }

#ifdef UNDER_XBOX
                case D3DFMT_LIN_X8R8G8B8:
                case D3DFMT_LIN_A8R8G8B8: {
#else
                case D3DFMT_X8R8G8B8:
                case D3DFMT_A8R8G8B8: {
#endif
                    LPDWORD pdwTexel = (LPDWORD)d3dlr.pBits + (d3dsd.Height - 1) * (d3dlr.Pitch >> 2);
                    DWORD dwTexel;
                    for (i = 0; i < d3dsd.Height; i++) {
                        for (j = 0; j < d3dsd.Width; j++, pc++) {
                            dwTexel = pdwTexel[j];
                            if (!uAlpha) {
                                pc->red = (BYTE)((dwTexel >> 16) & 0xFF);
                                pc->green = (BYTE)((dwTexel >> 8) & 0xFF);
                                pc->blue = (BYTE)(dwTexel & 0xFF);
                            }
                            else {
                                pc->red = pc->green = pc->blue = (BYTE)((dwTexel >> 24) & 0xFF);
                            }
                        }
                        pdwTexel -= d3dlr.Pitch >> 2;
                    }
                    break;
                }
            }
#ifdef UNDER_XBOX
        }
#endif

        if (!WriteFile(hFile, pvBuffer, dwSize, &dwWritten, NULL)) {
            pd3ds->UnlockRect();
            CloseHandle(hFile);
            return FALSE;
        }

        MemFree(pvBuffer);

        CloseHandle(hFile);
    }

    pd3ds->UnlockRect();

    return TRUE;
}

