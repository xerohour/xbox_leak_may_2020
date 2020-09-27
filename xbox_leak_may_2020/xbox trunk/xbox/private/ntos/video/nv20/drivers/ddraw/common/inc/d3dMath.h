/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

/********************************* Direct 3D *******************************\
*                                                                           *
* Module: d3dMath.h                                                         *
*       header file for d3dmath.cpp                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler    (bertrem)   09Sep99     created                 *
*                                                                           *
\***************************************************************************/

#ifndef __D3DMATH_H
#define __D3DMATH_H

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

// constants

#define EPSILON 0.0000001f

//---------------------------------------------------------------------------

// augment D3D with some sorely missing types

typedef struct _D3DVECTOR4 {
    D3DVALUE dvX;
    D3DVALUE dvY;
    D3DVALUE dvZ;
    D3DVALUE dvW;
} D3DVECTOR4;

typedef struct _D3DMATRIX3 {
    D3DVALUE _11, _12, _13;
    D3DVALUE _21, _22, _23;
    D3DVALUE _31, _32, _33;
} D3DMATRIX3;

//---------------------------------------------------------------------------

// basic matrices and vectors

static D3DMATRIX matrixIdent =
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

static D3DMATRIX matrixZero =
{
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
};

static D3DVECTOR vectorZero = {0.0f, 0.0f, 0.0f};

//---------------------------------------------------------------------------

// prototypes

float    __fastcall JBInvSqrt (const float x);
float    __fastcall nvSqrt (float x);
float    __fastcall nvInv (float x);
void     __fastcall ReverseVector3 (D3DVECTOR *out, D3DVECTOR *in);
void     __fastcall ScaleVector3 (D3DVECTOR *out, D3DVECTOR *v1, D3DVALUE scale);
void     __fastcall AddVectors3 (D3DVECTOR *out, D3DVECTOR *v1, D3DVECTOR *v2);
void     __fastcall SubtractVectors3 (D3DVECTOR *out, D3DVECTOR *v1, D3DVECTOR *v2);
D3DVALUE __fastcall SquareMagnitude3 (D3DVECTOR *v);
D3DVALUE __fastcall Magnitude3 (D3DVECTOR *v);
void     __fastcall NormalizeVector3 (D3DVECTOR *v);
D3DVALUE __fastcall DotProduct3 (D3DVECTOR *v1, D3DVECTOR *v2);
void     __fastcall XformBy4x3 (D3DVECTOR *res, D3DVECTOR *v, D3DVALUE w, D3DMATRIX *m);
void     __fastcall XformVector3 (D3DVECTOR *res, D3DVECTOR *v, D3DMATRIX *m);
void     __fastcall XformVector4 (D3DVALUE *res, D3DVALUE *v, D3DMATRIX *m);
void     __fastcall MatrixProduct3x3 (D3DMATRIX *res, D3DMATRIX *a, D3DMATRIX *b);
void     __fastcall MatrixProduct4x4 (D3DMATRIX *res, D3DMATRIX *a, D3DMATRIX *b);
void     __fastcall MatrixTranspose4x4 (D3DMATRIX *res, D3DMATRIX *m);
int      __fastcall Inverse3x3 (D3DMATRIX *inverse, D3DMATRIX *src, BOOL bNormalize);
int      __fastcall Inverse4x4 (D3DMATRIX *inverse, D3DMATRIX *src, BOOL bNormalize);
int      __fastcall STInverse4x4 (D3DMATRIX *inverse, D3DMATRIX *src);

// other prototypes
void nvComputeBoundingBox    (BYTE *pVertices,DWORD dwCount,DWORD dwVertexStride,float *pafMinMaxInfo);
void nvComputeXfmAndOutCodes (D3DMATRIX *pXfm,float *pfVerticesIn,float *pfVerticesOut,DWORD dwCount,DWORD *pdwOutCodes);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __D3DMATH_H

