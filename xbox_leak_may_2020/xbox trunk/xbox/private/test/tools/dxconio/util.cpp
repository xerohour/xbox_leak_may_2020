/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    util.cpp

Description:

    Utility functions.

*******************************************************************************/

#ifndef UNDER_XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif // UNDER_XBOX
#include <d3dx8.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include "util.h"

namespace DXCONIO {

//******************************************************************************
// Function prototypes
//******************************************************************************

//******************************************************************************
// Globals
//******************************************************************************

static HANDLE           g_hHeap = NULL;

//******************************************************************************
// Debugging functions
//******************************************************************************

#if defined(DEBUG) || defined(_DEBUG)
//******************************************************************************
//
// Function:
//
//     DebugString
//
// Description:
//
//     Take the formatted output, prepend the application name, and send the
//     output to the debugger.
//
// Arguments:
//
//     LPCTSTR szFormat         - Formatting string describing the output
//
//     Variable argument list   - Data to be placed in the output string
//
// Return Value:
//
//     None.
//
//******************************************************************************
void DebugString(LPCTSTR szFormat, ...) {

    TCHAR       szBuffer[2048];
    va_list     vl;

    _tcscpy(szBuffer, TEXT("D3DAPP: "));

    va_start(vl, szFormat);
#ifdef UNDER_CE
    wvsprintf(szBuffer+8, szFormat, vl);
#else
    _vstprintf(szBuffer+8, szFormat, vl);
#endif // DRAGON
    va_end(vl);

    _tcscat(szBuffer, TEXT("\r\n"));

    OutputDebugString(szBuffer);
}

//******************************************************************************
//
// Function:
//
//     ResultFailed
//
// Description:
//
//     Test a given return code: if the code is an error, output a debug 
//     message with the error value.  If the code is a warning, output
//     a debug message with the warning value.
//
// Arguments:
//
//     HRESULT hr               - Return code to test for an error
//
//     LPCTSTR sz               - String describing the method that produced 
//                                the return code
//
// Return Value:
//
//     TRUE if the given return code is an error, FALSE otherwise.
//
//******************************************************************************
BOOL ResultFailed(HRESULT hr, LPCTSTR sz) {

    TCHAR szError[512];

    if (SUCCEEDED(hr)) {
        return FALSE;
    }

    D3DXGetErrorString(hr, szError, 512);

    if (FAILED(hr)) {
        DebugString(TEXT("%s failed with %s [0x%X]"), sz, szError, hr);
        return TRUE;
    }
    else {
        DebugString(TEXT("Warning - %s returned %s [0x%X]"), sz, szError, hr);
    }

    return FALSE;
}
#endif // DEBUG || _DEBUG

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
//     LPDIRECT3DDEVICE7 pd3dd  - Pointer to the device object
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
/*
HRESULT TransformVertices(LPDIRECT3DDEVICE8 pd3dd, LPVOID pvDst, LPVOID pvSrc, 
                          DWORD dwNumVertices, DWORD dwStrideDst, DWORD dwStrideSrc, 
                          LPDWORD pdwClip) 
{
    D3DMATRIX               mWorld, mView, mProj, mClip, mR1, mR2;
    D3DVIEWPORT7            viewport;
    UINT                    i;
    HRESULT                 hr;
    float                   fW;
    LPBYTE                  pdst, psrc;
    LPD3DVECTOR             prSrc;
    struct _TVERTEX {
        D3DVECTOR v;
        float     fRHW;
    }                       *prDst;

    // Get the viewport dimensions
    memset(&viewport, 0, sizeof(D3DVIEWPORT7));
    hr = pd3dd->GetViewport(&viewport);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetViewport"))) {
        return hr;
    }

    // Build a transformation matrix from the current world, view, and
    // projection matrices

    hr = pd3dd->GetTransform(D3DTRANSFORMSTATE_PROJECTION, &mProj);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetTransform"))) {
        return hr;
    }

    InitMatrix(&mClip,
         0.5f,  0.0f,  0.0f,  0.0f,
         0.0f,  0.5f,  0.0f,  0.0f,
         0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.0f,  1.0f
    );

    MultiplyMatrix(&mR1, &mProj, &mClip);

    hr = pd3dd->GetTransform(D3DTRANSFORMSTATE_VIEW, &mView);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetTransform"))) {
        return hr;
    }

    MultiplyMatrix(&mR2, &mView, &mR1);

    hr = pd3dd->GetTransform(D3DTRANSFORMSTATE_WORLD, &mWorld);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetTransform"))) {
        return hr;
    }

    MultiplyMatrix(&mR1, &mWorld, &mR2);

    // Clear the clipping flags
    if (pdwClip) {
        memset(pdwClip, 0, dwNumVertices * sizeof(DWORD));
    }

    // Apply the transformation matrix to the given vertices
    for (i = 0, pdst = (LPBYTE)pvDst, psrc = (LPBYTE)pvSrc; 
         i < dwNumVertices; 
         i++, pdst += dwStrideDst, psrc += dwStrideSrc) 
    {
        prSrc = (LPD3DVECTOR)psrc;
        prDst = (struct _TVERTEX*)pdst;

        // Calculate the homogeneous coordinates
        fW = mR1._14 * prSrc->x + mR1._24 * prSrc->y + mR1._34 * prSrc->z + mR1._44;
        prDst->fRHW = 1.0f / fW;
        prDst->v.x = (prSrc->x * mR1._11 + prSrc->y * mR1._21 + prSrc->z * mR1._31 + mR1._41) * prDst->fRHW;
        prDst->v.y = (prSrc->x * mR1._12 + prSrc->y * mR1._22 + prSrc->z * mR1._32 + mR1._42) * prDst->fRHW;
        prDst->v.z = (prSrc->x * mR1._13 + prSrc->y * mR1._23 + prSrc->z * mR1._33 + mR1._43) * prDst->fRHW;

        // Clip test the coordinates
        if (pdwClip) {
            pdwClip[i] |= (prDst->v.x > 0.0f) ? 0 : D3DCLIP_LEFT;
            pdwClip[i] |= (prDst->v.x < fW)   ? 0 : D3DCLIP_RIGHT;
            pdwClip[i] |= (prDst->v.y > 0.0f) ? 0 : D3DCLIP_BOTTOM;
            pdwClip[i] |= (prDst->v.y < fW)   ? 0 : D3DCLIP_TOP;
            pdwClip[i] |= (prDst->v.z > 0.0f) ? 0 : D3DCLIP_FRONT;
            pdwClip[i] |= (prDst->v.z < fW)   ? 0 : D3DCLIP_BACK;
        }

        // Scale and offset x and y into screen coordinates
        prDst->v.x = prDst->v.x *  (float)viewport.dwWidth  + (float)viewport.dwX;
        prDst->v.y = prDst->v.y * -(float)viewport.dwHeight + (float)viewport.dwHeight + (float)viewport.dwY;
    }

    return hr;
}
*/

//******************************************************************************
// Memory allocation routines
//******************************************************************************

//******************************************************************************
BOOL CreateHeap() {

    if (g_hHeap) {
        return FALSE;
    }

    g_hHeap = HeapCreate(0, 0, 0);
    if (!g_hHeap) {
        DebugString(TEXT("HeapCreate failed [%d]"), GetLastError());
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
void ReleaseHeap() {

    if (g_hHeap) {
        if (!HeapDestroy(g_hHeap)) {
            DebugString(TEXT("HeapDestroy failed [%d]"), GetLastError());
        }
        g_hHeap = NULL;
    }
}

//******************************************************************************
LPVOID MemAlloc(UINT uNumBytes) {

    LPVOID pvMem = NULL;
    if (g_hHeap) {
        pvMem = HeapAlloc(g_hHeap, 0, uNumBytes);
    }
    return pvMem;
}

//******************************************************************************
void MemFree(LPVOID pvMem) {

    if (g_hHeap) {
        if (!HeapFree(g_hHeap, 0, pvMem)) {
            DebugString(TEXT("HeapFree failed [%d]"), GetLastError());
        }
    }
}

//******************************************************************************
//
// Function:
//
//     MemAlloc32
//
// Description:
//
//     Allocate a given amount of memory whose base address is
//     aligned along a 32 byte boundary.  This memory must later be freed
//     using the MemFree32 function.
//
// Arguments:
//
//     UINT uNumBytes           - Number of bytes to allocate
//
// Return Value:
//
//     Base address of the allocated memory.
//
//******************************************************************************
LPVOID MemAlloc32(UINT uNumBytes)
{
	LPBYTE pbyAllocated, pbyAligned = NULL;

	pbyAllocated = (LPBYTE)MemAlloc(uNumBytes + 32);
    if (pbyAllocated) {
	    pbyAligned   = (LPBYTE)((((DWORD)pbyAllocated) + 32) & ~31);
	    *(((LPDWORD)pbyAligned)-1) = (DWORD)pbyAllocated;
    }
	return pbyAligned;
}

//******************************************************************************
//
// Function:
//
//     MemFree32
//
// Description:
//
//     Free an aligned block of memory that was allocated using the MemAlloc32
//     function.
//
// Arguments:
//
//     LPVOID pvAligned         - Base address of the aligned memory
//
// Return Value:
//
//     None.
//
//******************************************************************************
void MemFree32(LPVOID pvMem)
{
	LPBYTE pbyFree;

    if (pvMem) {
	    pbyFree = (LPBYTE)*(((LPDWORD)pvMem)-1);
	    MemFree(pbyFree);
    }
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
void InitMatrix(LPD3DMATRIX pd3dm,
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
void SetView(LPD3DXMATRIX pd3dm, LPD3DXVECTOR3 pvPos, LPD3DXVECTOR3 pvAt, 
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
void SetPerspectiveProjection(LPD3DMATRIX pd3dm, float fFront, float fBack, 
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
void SetParallelProjection(LPD3DMATRIX pd3dm, float fFront, float fBack, 
                                    float fFieldOfView, float fAspect) {

//    float    f2FOV = fFieldOfView + fFieldOfView;
//    float    fFar = fBack - fFront;

    InitMatrix(pd3dm,
        1.0f, 0.0f, 0.0f,   0.0f,
        0.0f, 1.0f, 0.0f,   0.0f,
        0.0f, 0.0f, 0.001f, 0.0f,
        0.0f, 0.0f, 0.5f,   1.0f
    );
/*
    InitMatrix(pd3dm,
        2.0f / f2FOV, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / f2FOV, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / fFar, 0.0f,
        0.0f, 0.0f, -fFront / fFar, 1.0f
    );
*/
}

} // namespace DXCONIO
