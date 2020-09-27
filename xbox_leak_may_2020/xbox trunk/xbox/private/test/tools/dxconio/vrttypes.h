/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    vrttypes.h

Description:

    Various vertex type definitions.

*******************************************************************************/

#ifndef __VRTTYPES_H__
#define __VRTTYPES_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define FVF_VERTEX              (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define FVF_BVERTEX             (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2)
#define FVF_LVERTEX             (D3DFVF_XYZ | D3DFVF_DIFFUSE | \
                                 D3DFVF_SPECULAR | D3DFVF_TEX1)
#define FVF_TLVERTEX            (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | \
                                 D3DFVF_SPECULAR | D3DFVF_TEX1)
#define FVF_TLBVERTEX           (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | \
                                 D3DFVF_SPECULAR | D3DFVF_TEX2)

namespace DXCONIO {

//******************************************************************************
// Structures
//******************************************************************************

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
    D3DXVECTOR3 vNormal;
    float       u0, v0;
    float       u1, v1;

    _BVERTEX() {}
    _BVERTEX(const D3DVECTOR& v, const D3DVECTOR& n, float _u0, float _v0, float _u1, float _v1) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
        vNormal.x = n.x; vNormal.y = n.y; vNormal.z = n.z;
        u0 = _u0; v0 = _v0; u1 = _u1; v1 = _v1;
    }
} BVERTEX, *PBVERTEX;

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

} // namespace DXCONIO

#endif //__VRTTYPES_H__
