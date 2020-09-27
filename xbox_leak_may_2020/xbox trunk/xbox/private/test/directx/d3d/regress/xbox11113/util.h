/*******************************************************************************

Copyright (c) 2001 Microsoft Corporation.  All rights reserved.

File Name:

    util.h

Description:

    Helper functions.

*******************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

#define TWOPI 6.283185f

#define FRND() ((float)rand() / (float)RAND_MAX)

#define FABS(x)     ((x) < 0.0f ? -(x) : (x))

#define FVF_VERTEX              (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define FVF_TLVERTEX            (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | \
                                 D3DFVF_SPECULAR | D3DFVF_TEX1)

namespace XBOX11113 {

//******************************************************************************
typedef struct _TVERTEX {
    XGVECTOR3   vPosition;
#ifdef COMPRESS_TERRAIN_NORMALS
    DWORD       dwNormal;
#else
    XGVECTOR3   vNormal;
#endif
    float       u0, v0;

    _TVERTEX() {}
    _TVERTEX(const D3DVECTOR& v, const D3DVECTOR& n, const float _u0, const float _v0) { 
        vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z;
#ifdef COMPRESS_TERRAIN_NORMALS
        dwNormal = ((((DWORD)(n.x * 1023.0f) & 0x7FF)) |
                    (((DWORD)(n.y * 1023.0f) & 0x7FF) << 11) |
                    (((DWORD)(n.z *  511.0f) & 0x3FF) << 22));
#else
        vNormal = n;
#endif
        u0 = _u0;
        v0 = _v0;
    }
} TVERTEX, *PTVERTEX;

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

#if defined(DEBUG) || defined(_DEBUG) || defined(DBG)
void        DebugString(LPCTSTR szFormat, ...);
#else
static 
inline void DebugString(LPCTSTR szFormat, ...) {}
#endif

BOOL        LoadResourceFile(LPCSTR szName, LPVOID* ppvData, LPDWORD pdwSize);
void        FreeResourceFile(LPCSTR szName);

BOOL        LoadTexture(LPDIRECT3DDEVICE8 pDevice, LPCSTR szName, LPDIRECT3DTEXTURE8* ppd3dt);

}

#endif //__UTIL_H__
