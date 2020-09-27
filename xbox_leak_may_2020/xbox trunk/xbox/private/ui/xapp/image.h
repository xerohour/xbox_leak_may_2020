#pragma once

#if !defined(_WINDOWS) && !defined(_WIN32_WCE)
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define PALETTERGB(r,g,b)   (0x02000000 | RGB(r,g,b))
#define PALETTEINDEX(i)     ((COLORREF)(0x01000000 | (DWORD)(WORD)(i)))
#define GetRValue(rgb)      ((BYTE)(rgb))
#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      ((BYTE)((rgb)>>16))

#define SetRect(pRect, nLeft, nTop, nRight, nBottom) \
{ \
	(pRect)->left = (nLeft); \
	(pRect)->top = (nTop); \
	(pRect)->right = (nRight); \
	(pRect)->bottom = (nBottom); \
}

/* Background Modes */
#define TRANSPARENT         1
#define OPAQUE              2

/* Text Alignment Options */
#define TA_LEFT              0
#define TA_RIGHT             2
#define TA_TOP               0
#define TA_BOTTOM            8
#define TA_BASELINE          24

#endif



#if defined(_WINDOWS) || defined(_WIN32_WCE)
#define _USEGDI
#endif

#ifdef _USEGDI
#define HDRAW HDC
#else
#define XFONT_TRUETYPE
#include <xfont.h>

#ifndef __cplusplus
typedef struct IDirect3DSurface8 * LPDIRECT3DSURFACE8;
typedef unsigned long D3DCOLOR;
#endif

#ifdef __cplusplus
struct DRAW
{
	LPDIRECT3DSURFACE8 pSurface;
	D3DSURFACE_DESC Desc;
	D3DLOCKED_RECT Lock;
};
#endif

typedef struct DRAW* /*LPDIRECT3DSURFACE8*/ HDRAW;
#endif


#ifdef _USEGDI
typedef HBITMAP HIMAGE;
#endif

#ifdef _XBOX
EXTERN_C HRESULT X_BitBlt(HDRAW hDraw, int x, int y, int cx, int cy, LPDIRECT3DTEXTURE8 pSrcSurface, int xSrc, int ySrc);
EXTERN_C HRESULT X_FillRect(HDRAW hDraw, int x, int y, int cx, int cy, D3DCOLOR color);
typedef LPDIRECT3DTEXTURE8 HIMAGE;
#endif



EXTERN_C void DrawImage(HDRAW hDC, const TCHAR* szImgFile, int x, int y, int align, WORD* pcx, WORD* pcy);
EXTERN_C BOOL GetImageSize(const TCHAR* szImgFile, SIZE* pSize);
