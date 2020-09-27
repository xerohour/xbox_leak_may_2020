/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    dtypes.h

Author:

    Matt Bronder

Description:

    Various type definitions.

*******************************************************************************/

#ifndef __DTYPES_H__
#define __DTYPES_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define FVF_VERTEX              (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define FVF_BVERTEX             (D3DFVF_XYZ | D3DFVF_TEX3)
#define FVF_BVERTEX2            (D3DFVF_XYZ | D3DFVF_TEX2)
#define FVF_NVERTEX             (D3DFVF_XYZ | D3DFVF_TEX3 | \
                                              D3DFVF_TEXCOORDSIZE2(0) | \
                                              D3DFVF_TEXCOORDSIZE2(1) | \
                                              D3DFVF_TEXCOORDSIZE3(2))
#define FVF_LVERTEX             (D3DFVF_XYZ | D3DFVF_DIFFUSE | \
                                 D3DFVF_SPECULAR | D3DFVF_TEX1)
#define FVF_TLVERTEX            (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | \
                                 D3DFVF_SPECULAR | D3DFVF_TEX1)
#define FVF_TLBVERTEX           (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | \
                                 D3DFVF_SPECULAR | D3DFVF_TEX2)
#define FVF_CVERTEX             (D3DFVF_XYZ | D3DFVF_NORMAL)
#define FVF_CVERTEX1_3          (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | \
                                 D3DFVF_TEXCOORDSIZE3(0))

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
typedef struct _TCOORD {
    float               u;
    float               v;
} TCOORD, *PTCOORD;

//******************************************************************************
typedef struct _TCOORD2 {
    float               u;
    float               v;
} TCOORD2, *PTCOORD2;

//******************************************************************************
typedef struct _TCOORD3 {
    float               u;
    float               v;
    float               t;
} TCOORD3, *PTCOORD3;

//******************************************************************************
typedef struct _MATRIX3X3 {
    union {
        struct {
            float _11, _12, _13;
            float _21, _22, _23;
            float _31, _32, _33;
        };
        float m[3][3];
        struct {
            D3DXVECTOR3 r1;
            D3DXVECTOR3 r2;
            D3DXVECTOR3 r3;
        };
    };
} MATRIX3X3, *PMATRIX3X3;

//******************************************************************************
typedef struct _VERTEX {
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vNormal;
    float       u0, v0;

    _VERTEX() {}
    _VERTEX(const D3DVECTOR& v, const D3DVECTOR& n, float _u0, float _v0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        vNormal.x = n.x; vNormal.y = n.y; vNormal.z = n.z;
        u0 = _u0; v0 = _v0;
    }
} VERTEX, *PVERTEX;

//******************************************************************************
typedef struct _BVERTEX {
    D3DXVECTOR3 vPosition;
    float       u0, v0;         // Base texture coordinates
    float       u1, v1;         // Height (bump) map coordinates
    float       u2, v2;         // Diffuse light map coordinates

    _BVERTEX() {}
    _BVERTEX(const D3DVECTOR& v, float _u0, float _v0, float _u1, float _v1, float _u2, float _v2) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        u0 = _u0; v0 = _v0; u1 = _u1; v1 = _v1; u2 = _u2; v2 = _v2;
    }
} BVERTEX, *PBVERTEX;

//******************************************************************************
typedef struct _BVERTEX2 {
    D3DXVECTOR3 vPosition;
    float       u0, v0;         // Base texture coordinates and height (bump) map coordinates
    float       u1, v1;         // Diffuse light map coordinates

    _BVERTEX2() {}
    _BVERTEX2(const D3DVECTOR& v, float _u0, float _v0, float _u1, float _v1) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        u0 = _u0; v0 = _v0; u1 = _u1; v1 = _v1;
    }
} BVERTEX2, *PBVERTEX2;

//******************************************************************************
typedef struct _NVERTEX {
    D3DXVECTOR3 vPosition;
    float       u0, v0;         // Base texture coordinates
    float       u1, v1;         // Height (normal) map coordinates
    float       u2, v2, t2;     // Light direction in tangent space

    _NVERTEX() {}
    _NVERTEX(const D3DVECTOR& v, float _u0, float _v0, float _u1, float _v1, float _u2, float _v2, float _t2) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        u0 = _u0; v0 = _v0; u1 = _u1; v1 = _v1; u2 = _u2; v2 = _v2; t2 = _t2;
    }
} NVERTEX, *PNVERTEX;

//******************************************************************************
typedef struct _LVERTEX {
    D3DXVECTOR3 vPosition;
    D3DCOLOR    cDiffuse;
    D3DCOLOR    cSpecular;
    float       u0, v0;

    _LVERTEX() { }
    _LVERTEX(const D3DVECTOR& v, D3DCOLOR _cDiffuse, D3DCOLOR _cSpecular, float _u0, float _v0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        cDiffuse = _cDiffuse; cSpecular = _cSpecular;
        u0 = _u0; v0 = _v0;
    }
} LVERTEX, *PLVERTEX;

//******************************************************************************
typedef struct _TLVERTEX {
    D3DXVECTOR3 vPosition;
    float       fRHW;
    D3DCOLOR    cDiffuse;
    D3DCOLOR    cSpecular;
    float       u0, v0;

    _TLVERTEX() {}
    _TLVERTEX(const D3DVECTOR& v, float _fRHW, D3DCOLOR _cDiffuse, 
              D3DCOLOR _cSpecular, float _u0, float _v0) 
    { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z; fRHW = _fRHW;
        cDiffuse = _cDiffuse; cSpecular = _cSpecular;
        u0 = _u0; v0 = _v0;
    }
} TLVERTEX, *PTLVERTEX;

//******************************************************************************
typedef struct _TLBVERTEX {
    D3DXVECTOR3 vPosition;
    float       fRHW;
    D3DCOLOR    cDiffuse;
    D3DCOLOR    cSpecular;
    float       u0, v0;
    float       u1, v1;

    _TLBVERTEX() {}
    _TLBVERTEX(const D3DVECTOR& v, float _fRHW, D3DCOLOR _cDiffuse, 
              D3DCOLOR _cSpecular, float _u0, float _v0, float _u1, float _v1) 
    { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z; fRHW = _fRHW;
        cDiffuse = _cDiffuse; cSpecular = _cSpecular;
        u0 = _u0; v0 = _v0; u1 = _u1; v1 = _v1;
    }
} TLBVERTEX, *PTLBVERTEX;

//******************************************************************************
typedef struct _CVERTEX {
    D3DXVECTOR3   vPosition;
    D3DXVECTOR3   vNormal;

    _CVERTEX() { }
    _CVERTEX(const D3DVECTOR& v, const D3DVECTOR& n) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        vNormal.x = n.x; vNormal.y = n.y; vNormal.z = n.z;
    }
} CVERTEX, *PCVERTEX;

//******************************************************************************
typedef struct _VERTEX1_3 {
    D3DXVECTOR3   vPosition;
    D3DXVECTOR3   vNormal;
    float         u0, v0, t0;

    _VERTEX1_3() { }
    _VERTEX1_3(const D3DVECTOR& v, const D3DVECTOR& n, float _u0, float _v0, float _t0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        vNormal.x = n.x; vNormal.y = n.y; vNormal.z = n.z;
        u0 = _u0; v0 = _v0; t0 = _t0;
    }
} VERTEX1_3, *PVERTEX1_3;

#endif //__DTYPES_H__
