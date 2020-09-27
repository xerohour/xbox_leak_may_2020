typedef float D3DVALUE, *LPD3DVALUE;

#define D3DFVF_VERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )

#if 0

#define D3DFVF_LVERTEX ( D3DFVF_XYZ | D3DFVF_RESERVED1 | D3DFVF_DIFFUSE | \
                         D3DFVF_SPECULAR | D3DFVF_TEX1 )
#define D3DFVF_TLVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | \
                          D3DFVF_TEX1 )

/*
 * Vertex data types supported in an ExecuteBuffer.
 */

/*
 * Homogeneous vertices
 */

typedef struct _D3DHVERTEX {
    DWORD           dwFlags;        /* Homogeneous clipping flags */
    union {
    D3DVALUE    hx;
    D3DVALUE    dvHX;
    };
    union {
    D3DVALUE    hy;
    D3DVALUE    dvHY;
    };
    union {
    D3DVALUE    hz;
    D3DVALUE    dvHZ;
    };
} D3DHVERTEX, *LPD3DHVERTEX;

/*
 * Transformed/lit vertices
 */
typedef struct _D3DTLVERTEX {
    union {
    D3DVALUE    sx;             /* Screen coordinates */
    D3DVALUE    dvSX;
    };
    union {
    D3DVALUE    sy;
    D3DVALUE    dvSY;
    };
    union {
    D3DVALUE    sz;
    D3DVALUE    dvSZ;
    };
    union {
    D3DVALUE    rhw;        /* Reciprocal of homogeneous w */
    D3DVALUE    dvRHW;
    };
    union {
    D3DCOLOR    color;          /* Vertex color */
    D3DCOLOR    dcColor;
    };
    union {
    D3DCOLOR    specular;       /* Specular component of vertex */
    D3DCOLOR    dcSpecular;
    };
    union {
    D3DVALUE    tu;             /* Texture coordinates */
    D3DVALUE    dvTU;
    };
    union {
    D3DVALUE    tv;
    D3DVALUE    dvTV;
    };
#if(DIRECT3D_VERSION >= 0x0500)
#if (defined __cplusplus) && (defined D3D_OVERLOADS)
    _D3DTLVERTEX() { }
    _D3DTLVERTEX(const D3DVECTOR& v, float _rhw,
                 D3DCOLOR _color, D3DCOLOR _specular,
                 float _tu, float _tv)
        { sx = v.x; sy = v.y; sz = v.z; rhw = _rhw;
          color = _color; specular = _specular;
          tu = _tu; tv = _tv;
        }
#endif
#endif /* DIRECT3D_VERSION >= 0x0500 */
} D3DTLVERTEX, *LPD3DTLVERTEX;

/*
 * Untransformed/lit vertices
 */
typedef struct _D3DLVERTEX {
    union {
    D3DVALUE     x;             /* Homogeneous coordinates */
    D3DVALUE     dvX;
    };
    union {
    D3DVALUE     y;
    D3DVALUE     dvY;
    };
    union {
    D3DVALUE     z;
    D3DVALUE     dvZ;
    };
    DWORD            dwReserved;
    union {
    D3DCOLOR     color;         /* Vertex color */
    D3DCOLOR     dcColor;
    };
    union {
    D3DCOLOR     specular;      /* Specular component of vertex */
    D3DCOLOR     dcSpecular;
    };
    union {
    D3DVALUE     tu;            /* Texture coordinates */
    D3DVALUE     dvTU;
    };
    union {
    D3DVALUE     tv;
    D3DVALUE     dvTV;
    };
#if(DIRECT3D_VERSION >= 0x0500)
#if (defined __cplusplus) && (defined D3D_OVERLOADS)
    _D3DLVERTEX() { }
    _D3DLVERTEX(const D3DVECTOR& v,
                D3DCOLOR _color, D3DCOLOR _specular,
                float _tu, float _tv)
        { x = v.x; y = v.y; z = v.z; dwReserved = 0;
          color = _color; specular = _specular;
          tu = _tu; tv = _tv;
        }
#endif
#endif /* DIRECT3D_VERSION >= 0x0500 */
} D3DLVERTEX, *LPD3DLVERTEX;

#endif



/*
 * Untransformed/unlit vertices
 */

typedef struct _D3DVERTEX {
    union {
    D3DVALUE     x;             /* Homogeneous coordinates */
    D3DVALUE     dvX;
    };
    union {
    D3DVALUE     y;
    D3DVALUE     dvY;
    };
    union {
    D3DVALUE     z;
    D3DVALUE     dvZ;
    };
    union {
    D3DVALUE     nx;            /* Normal */
    D3DVALUE     dvNX;
    };
    union {
    D3DVALUE     ny;
    D3DVALUE     dvNY;
    };
    union {
    D3DVALUE     nz;
    D3DVALUE     dvNZ;
    };
    union {
    D3DVALUE     tu;            /* Texture coordinates */
    D3DVALUE     dvTU;
    };
    union {
    D3DVALUE     tv;
    D3DVALUE     dvTV;
    };
#if(DIRECT3D_VERSION >= 0x0500)
#if (defined __cplusplus) && (defined D3D_OVERLOADS)
    _D3DVERTEX() { }
    _D3DVERTEX(const D3DVECTOR& v, const D3DVECTOR& n, float _tu, float _tv)
        { x = v.x; y = v.y; z = v.z;
          nx = n.x; ny = n.y; nz = n.z;
          tu = _tu; tv = _tv;
        }
#endif
#endif /* DIRECT3D_VERSION >= 0x0500 */
} D3DVERTEX, *LPD3DVERTEX;
