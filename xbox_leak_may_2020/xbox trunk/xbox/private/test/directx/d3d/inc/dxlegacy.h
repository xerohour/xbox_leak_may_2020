
#ifndef __DXLEGACY_H__
#define __DXLEGACY_H__

#define D3DTRANSFORMSTATE_WORLD                 D3DTS_WORLD
#define D3DTRANSFORMSTATE_VIEW                  D3DTS_VIEW
#define D3DTRANSFORMSTATE_PROJECTION            D3DTS_PROJECTION
#define D3DTRANSFORMSTATE_WORLD1                D3DTS_WORLD1
#define D3DTRANSFORMSTATE_WORLD2                D3DTS_WORLD2
#define D3DTRANSFORMSTATE_WORLD3                D3DTS_WORLD3
#define D3DTRANSFORMSTATE_TEXTURE0              D3DTS_TEXTURE0
#define D3DTRANSFORMSTATE_TEXTURE1              D3DTS_TEXTURE1
#define D3DTRANSFORMSTATE_TEXTURE2              D3DTS_TEXTURE2
#define D3DTRANSFORMSTATE_TEXTURE3              D3DTS_TEXTURE3
#define D3DTRANSFORMSTATE_TEXTURE4              D3DTS_TEXTURE4
#define D3DTRANSFORMSTATE_TEXTURE5              D3DTS_TEXTURE5
#define D3DTRANSFORMSTATE_TEXTURE6              D3DTS_TEXTURE6
#define D3DTRANSFORMSTATE_TEXTURE7              D3DTS_TEXTURE7
#define D3DTRANSFORMSTATE_FORCE_DWORD           D3DTS_FORCE_DWORD

#define D3DRENDERSTATE_ZENABLE                  D3DRS_ZENABLE
#define D3DRENDERSTATE_FILLMODE                 D3DRS_FILLMODE
#define D3DRENDERSTATE_SHADEMODE                D3DRS_SHADEMODE
#define D3DRENDERSTATE_LINEPATTERN              D3DRS_LINEPATTERN
#define D3DRENDERSTATE_ZWRITEENABLE             D3DRS_ZWRITEENABLE
#define D3DRENDERSTATE_ALPHATESTENABLE          D3DRS_ALPHATESTENABLE
#define D3DRENDERSTATE_LASTPIXEL                D3DRS_LASTPIXEL
#define D3DRENDERSTATE_SRCBLEND                 D3DRS_SRCBLEND
#define D3DRENDERSTATE_DESTBLEND                D3DRS_DESTBLEND
#define D3DRENDERSTATE_CULLMODE                 D3DRS_CULLMODE
#define D3DRENDERSTATE_ZFUNC                    D3DRS_ZFUNC
#define D3DRENDERSTATE_ALPHAREF                 D3DRS_ALPHAREF
#define D3DRENDERSTATE_ALPHAFUNC                D3DRS_ALPHAFUNC
#define D3DRENDERSTATE_DITHERENABLE             D3DRS_DITHERENABLE
#define D3DRENDERSTATE_ALPHABLENDENABLE         D3DRS_ALPHABLENDENABLE
#define D3DRENDERSTATE_FOGENABLE                D3DRS_FOGENABLE
#define D3DRENDERSTATE_SPECULARENABLE           D3DRS_SPECULARENABLE
#define D3DRENDERSTATE_ZVISIBLE                 D3DRS_ZVISIBLE
#define D3DRENDERSTATE_FOGCOLOR                 D3DRS_FOGCOLOR
#define D3DRENDERSTATE_FOGTABLEMODE             D3DRS_FOGTABLEMODE
#define D3DRENDERSTATE_FOGSTART                 D3DRS_FOGSTART
#define D3DRENDERSTATE_FOGEND                   D3DRS_FOGEND
#define D3DRENDERSTATE_FOGDENSITY               D3DRS_FOGDENSITY
#define D3DRENDERSTATE_EDGEANTIALIAS            D3DRS_EDGEANTIALIAS
#define D3DRENDERSTATE_ZBIAS                    D3DRS_ZBIAS
#define D3DRENDERSTATE_RANGEFOGENABLE           D3DRS_RANGEFOGENABLE
#define D3DRENDERSTATE_STENCILENABLE            D3DRS_STENCILENABLE
#define D3DRENDERSTATE_STENCILFAIL              D3DRS_STENCILFAIL
#define D3DRENDERSTATE_STENCILZFAIL             D3DRS_STENCILZFAIL
#define D3DRENDERSTATE_STENCILPASS              D3DRS_STENCILPASS
#define D3DRENDERSTATE_STENCILFUNC              D3DRS_STENCILFUNC
#define D3DRENDERSTATE_STENCILREF               D3DRS_STENCILREF
#define D3DRENDERSTATE_STENCILMASK              D3DRS_STENCILMASK
#define D3DRENDERSTATE_STENCILWRITEMASK         D3DRS_STENCILWRITEMASK
#define D3DRENDERSTATE_TEXTUREFACTOR            D3DRS_TEXTUREFACTOR
#define D3DRENDERSTATE_WRAP0                    D3DRS_WRAP0
#define D3DRENDERSTATE_WRAP1                    D3DRS_WRAP1
#define D3DRENDERSTATE_WRAP2                    D3DRS_WRAP2
#define D3DRENDERSTATE_WRAP3                    D3DRS_WRAP3
#ifndef UNDER_XBOX
#define D3DRENDERSTATE_WRAP4                    D3DRS_WRAP4
#define D3DRENDERSTATE_WRAP5                    D3DRS_WRAP5
#define D3DRENDERSTATE_WRAP6                    D3DRS_WRAP6
#define D3DRENDERSTATE_WRAP7                    D3DRS_WRAP7
#endif
#define D3DRENDERSTATE_CLIPPING                 D3DRS_CLIPPING
#define D3DRENDERSTATE_LIGHTING                 D3DRS_LIGHTING
#define D3DRENDERSTATE_EXTENTS                  D3DRS_EXTENTS
#define D3DRENDERSTATE_AMBIENT                  D3DRS_AMBIENT
#define D3DRENDERSTATE_FOGVERTEXMODE            D3DRS_FOGVERTEXMODE
#define D3DRENDERSTATE_COLORVERTEX              D3DRS_COLORVERTEX
#define D3DRENDERSTATE_LOCALVIEWER              D3DRS_LOCALVIEWER
#define D3DRENDERSTATE_NORMALIZENORMALS         D3DRS_NORMALIZENORMALS
#define D3DRENDERSTATE_DIFFUSEMATERIALSOURCE    D3DRS_DIFFUSEMATERIALSOURCE
#define D3DRENDERSTATE_SPECULARMATERIALSOURCE   D3DRS_SPECULARMATERIALSOURCE
#define D3DRENDERSTATE_AMBIENTMATERIALSOURCE    D3DRS_AMBIENTMATERIALSOURCE
#define D3DRENDERSTATE_EMISSIVEMATERIALSOURCE   D3DRS_EMISSIVEMATERIALSOURCE
#define D3DRENDERSTATE_VERTEXBLEND              D3DRS_VERTEXBLEND
#define D3DRENDERSTATE_CLIPPLANEENABLE          D3DRS_CLIPPLANEENABLE

#define D3DVBLEND_1WEIGHT                       D3DVBF_1WEIGHTS
#define D3DVBLEND_2WEIGHTS                      D3DVBF_2WEIGHTS
#define D3DVBLEND_3WEIGHTS                      D3DVBF_3WEIGHTS
#define D3DVBLEND_DISABLE                       D3DVBF_DISABLE

#define D3DFVF_VERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )
//#define D3DFVF_LVERTEX ( D3DFVF_XYZ | D3DFVF_RESERVED1 | D3DFVF_DIFFUSE | \
//                         D3DFVF_SPECULAR | D3DFVF_TEX1 )
#define D3DFVF_TLVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | \
                          D3DFVF_TEX1 )

typedef struct _D3DDP_PTRSTRIDE {
    LPVOID lpvData;
    DWORD  dwStride;
} D3DDP_PTRSTRIDE; 

typedef struct D3DDRAWPRIMITIVESTRIDEDDATA  {
    D3DDP_PTRSTRIDE position;
    D3DDP_PTRSTRIDE normal;
    D3DDP_PTRSTRIDE diffuse;
    D3DDP_PTRSTRIDE specular;
    D3DDP_PTRSTRIDE textureCoords[D3DDP_MAXTEXCOORD];
} D3DDRAWPRIMITIVESTRIDEDDATA , *LPD3DDRAWPRIMITIVESTRIDEDDATA;

typedef struct _D3DVERTEX {
    union { 
        float x; 
        float dvX; 
    }; 
    union { 
        float y; 
        float dvY; 
    }; 
    union { 
        float z; 
        float dvZ; 
    }; 
    union { 
        float nx; 
        float dvNX; 
    }; 
    union { 
        float ny; 
        float dvNY; 
    }; 
    union { 
        float nz; 
        float dvNZ; 
    }; 
    union { 
        float tu; 
        float dvTU; 
    }; 
    union { 
        float tv; 
        float dvTV; 
    }; 
    _D3DVERTEX() { }
    _D3DVERTEX(const D3DVECTOR& v, const D3DVECTOR& n, float _tu, float _tv)
        { x = v.x; y = v.y; z = v.z;
          nx = n.x; ny = n.y; nz = n.z;
          tu = _tu; tv = _tv;
        }
} D3DVERTEX, *LPD3DVERTEX;

typedef struct _D3DTLVERTEX { 
    union { 
        float sx; 
        float dvSX; 
    }; 
    union { 
        float sy; 
        float dvSY; 
    }; 
    union { 
        float sz; 
        float dvSZ; 
    }; 
    union { 
        float rhw; 
        float dvRHW; 
    }; 
    union { 
        D3DCOLOR color; 
        D3DCOLOR dcColor; 
    }; 
    union { 
        D3DCOLOR specular; 
        D3DCOLOR dcSpecular; 
    }; 
    union { 
        float tu; 
        float dvTU; 
    }; 
    union { 
        float tv; 
        float dvTV; 
    }; 
} D3DTLVERTEX, *LPD3DTLVERTEX; 


inline D3DVECTOR
cD3DVECTOR(float x, float y, float z)
{
    D3DVECTOR v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

inline D3DVERTEX 
cD3DVERTEX(D3DVECTOR vPosition, D3DVECTOR vNormal, float u, float v) 
{
    D3DVERTEX r;
    r.x = vPosition.x;
    r.y = vPosition.y;
    r.z = vPosition.z;
    r.nx = vNormal.x;
    r.ny = vNormal.y;
    r.nz = vNormal.z;
    r.tu = u;
    r.tv = v;
    return r;
}

inline D3DTLVERTEX 
cD3DTLVERTEX(D3DVECTOR vPosition, float rhw, D3DCOLOR color, D3DCOLOR specular, float u, float v) 
{
    D3DTLVERTEX r;
    r.sx = vPosition.x;
    r.sy = vPosition.y;
    r.sz = vPosition.z;
    r.rhw = rhw;
    r.color = color;
    r.specular = specular;
    r.tu = u;
    r.tv = v;
    return r;
}

inline float
DotProduct (const _D3DVECTOR& v1, const _D3DVECTOR& v2)
{
   return v1.x*v2.x + v1.y * v2.y + v1.z*v2.z;
}

inline _D3DVECTOR
CrossProduct (const _D3DVECTOR& v1, const _D3DVECTOR& v2)
{
    _D3DVECTOR result;
 
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
 
    return result;
}

inline float
SquareMagnitude (const _D3DVECTOR& v)
{
   return v.x*v.x + v.y*v.y + v.z*v.z;
}


inline float
Magnitude (const _D3DVECTOR& v)
{
   return (float)sqrt(SquareMagnitude(v));
}


inline _D3DVECTOR
Normalize (const _D3DVECTOR& v)
{
   D3DVECTOR result;
   float     len = Magnitude(v);
   result.x = v.x / len;
   result.y = v.y / len;
   result.z = v.z / len;
   return result;
}

#endif // __DXLEGACY_H__
