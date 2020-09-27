/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    util.h

Description:

    Utility functions.

*******************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

#ifndef LPD3DMATRIX
#define LPD3DMATRIX D3DMATRIX*
#endif

//******************************************************************************
// Precomputed fractions of pi
//******************************************************************************

#define M_PI        3.14159265359f
#define M_2PI       6.28318530718f
#define M_PIDIV2    1.57079632680f
#define M_3PIDIV2   4.71238898039f
#define M_PIDIV4    0.78539816340f
#define M_3PIDIV4   2.35619449019f
#define M_5PIDIV4   3.92699081699f
#define M_7PIDIV4   5.49778714378f

//******************************************************************************
// Macros
//******************************************************************************

#define RND()       ((float)rand() / (float)RAND_MAX)
#define FRND(x)     (RND() * x)
#define FABS(x)     (x < 0.0f ? -x : x)

#ifndef RGB_MAKE
/*
 * Format of CI colors is
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    alpha      |         color index           |   fraction    |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#define CI_GETALPHA(ci)    ((ci) >> 24)
#define CI_GETINDEX(ci)    (((ci) >> 8) & 0xffff)
#define CI_GETFRACTION(ci) ((ci) & 0xff)
#define CI_ROUNDINDEX(ci)  CI_GETINDEX((ci) + 0x80)
#define CI_MASKALPHA(ci)   ((ci) & 0xffffff)
#define CI_MAKE(a, i, f)    (((a) << 24) | ((i) << 8) | (f))

/*
 * Format of RGBA colors is
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    alpha      |      red      |     green     |     blue      |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#define RGBA_GETALPHA(rgb)      ((rgb) >> 24)
#define RGBA_GETRED(rgb)        (((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      (((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       ((rgb) & 0xff)
#define RGBA_MAKE(r, g, b, a)   ((D3DCOLOR) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

/* D3DRGB and D3DRGBA may be used as initialisers for D3DCOLORs
 * The float values must be in the range 0..1
 */
#define D3DRGB(r, g, b) \
    (0xff000000L | ( ((long)((r) * 255)) << 16) | (((long)((g) * 255)) << 8) | (long)((b) * 255))
#define D3DRGBA(r, g, b, a) \
    (   (((long)((a) * 255)) << 24) | (((long)((r) * 255)) << 16) \
    |   (((long)((g) * 255)) << 8) | (long)((b) * 255) \
    )

/*
 * Format of RGB colors is
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    ignored    |      red      |     green     |     blue      |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#define RGB_GETRED(rgb)         (((rgb) >> 16) & 0xff)
#define RGB_GETGREEN(rgb)       (((rgb) >> 8) & 0xff)
#define RGB_GETBLUE(rgb)        ((rgb) & 0xff)
#define RGBA_SETALPHA(rgba, x) (((x) << 24) | ((rgba) & 0x00ffffff))
#define RGB_MAKE(r, g, b)       ((D3DCOLOR) (((r) << 16) | ((g) << 8) | (b)))
#define RGBA_TORGB(rgba)       ((D3DCOLOR) ((rgba) & 0xffffff))
#define RGB_TORGBA(rgb)        ((D3DCOLOR) ((rgb) | 0xff000000))

#endif

namespace DXCONIO {

//******************************************************************************
// Debugging function prototypes
//******************************************************************************

#if defined(DEBUG) || defined(_DEBUG)
void                DebugString(LPCTSTR szFormat, ...);
BOOL                ResultFailed(HRESULT hr, LPCTSTR sz);
#else
inline void         DebugString(LPCTSTR szFormat, ...) {}
inline BOOL         ResultFailed(HRESULT hr, LPCTSTR sz) {return FALSE;};
#endif

//******************************************************************************
// Vertex transformation routine
//******************************************************************************
/*
HRESULT TransformVertices(LPDIRECT3DDEVICE8 pd3dd, LPVOID pvDst, LPVOID pvSrc, 
                        DWORD dwNumVertices, 
                        DWORD dwStrideDst = sizeof(D3DTLVERTEX), 
                        DWORD dwStrideSrc = sizeof(D3DVERTEX), 
                        LPDWORD pdwClip = NULL);
*/
//******************************************************************************
// Memory allocation prototypes
//******************************************************************************

BOOL                CreateHeap();
void                ReleaseHeap();
LPVOID              MemAlloc(UINT uNumBytes);
void                MemFree(LPVOID pvMem);
LPVOID              MemAlloc32(UINT uNumBytes);
void                MemFree32(LPVOID pvMem);

//******************************************************************************
// Matrix utility function prototypes
//******************************************************************************

void                InitMatrix(LPD3DMATRIX pd3dm,
                        float _11, float _12, float _13, float _14,
                        float _21, float _22, float _23, float _24,
                        float _31, float _32, float _33, float _34,
                        float _41, float _42, float _43, float _44);

void                SetView(LPD3DXMATRIX pd3dm, LPD3DXVECTOR3 pvPos, 
                        LPD3DXVECTOR3 pvAt, LPD3DXVECTOR3 pvUp);

void                SetPerspectiveProjection(LPD3DMATRIX pd3dm, float fFront, 
                        float fBack, float fFieldOfView, float fAspect);

void                SetParallelProjection(LPD3DMATRIX pd3dm, float fFront, 
                        float fBack, float fFieldOfView, float fAspect);

//******************************************************************************
// Miscellaneous functions
//******************************************************************************

inline DWORD        F2DW(float f) {return *((LPDWORD)&f);}

} // namespace DXCONIO

#endif // __UTIL_H__
