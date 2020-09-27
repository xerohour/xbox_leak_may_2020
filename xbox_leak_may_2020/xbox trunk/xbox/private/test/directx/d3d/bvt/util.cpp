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
#include "log.h"
#include "util.h"

namespace D3DBVT {

//******************************************************************************
// Function prototypes
//******************************************************************************

//******************************************************************************
// Globals
//******************************************************************************

static HANDLE           g_hHeap = NULL;

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

//    float    f2FOV = fFieldOfView + fFieldOfView;
//    float    fFar = fBack - fFront;

    InitMatrix(pd3dm,
        2.0f / fWidth, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / fHeight, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / (fBack - fFront), 0.0f,
        0.0f, 0.0f, fFront / (fFront - fBack), 1.0f
    );
/*
    InitMatrix(pd3dm,
        1.0f, 0.0f, 0.0f,   0.0f,
        0.0f, 1.0f, 0.0f,   0.0f,
        0.0f, 0.0f, 0.001f, 0.0f,
        0.0f, 0.0f, 0.5f,   1.0f
    );
*/
/*
    InitMatrix(pd3dm,
        2.0f / f2FOV, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / f2FOV, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / fFar, 0.0f,
        0.0f, 0.0f, -fFront / fFar, 1.0f
    );
*/
}

} // namespace D3DBVT
