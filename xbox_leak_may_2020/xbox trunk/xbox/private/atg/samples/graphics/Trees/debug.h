//-----------------------------------------------------------------------------
//  
//  File: debug.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xgraphics.h>

//////////////////////////////////////////////////////////////////////
// Clear backbuffer
//
extern D3DXCOLOR g_DebugClearColor;
extern float g_fDebugClearZ;
extern DWORD g_dwDebugClearStencil;
extern HRESULT DebugClear();

//////////////////////////////////////////////////////////////////////
// Clear region of current render target (even if it's swizzled)
//
extern HRESULT DebugRect(RECT *pRect, DWORD dwColor);

//////////////////////////////////////////////////////////////////////
// Swap backbuffer to the front
//
extern DWORD g_dwDebugPresentPause;	// Sleep delay after presentation
extern HRESULT DebugPresent();

//////////////////////////////////////////////////////////////////////
// Display texture
//
extern bool g_bDebugExpand ;		// expand to fill screen
extern bool g_bDebugExpandAspect;	// preserve aspect ratio when expanding
extern bool g_bDebugAlpha;			// replicate alpha to the color channels
extern D3DXCOLOR g_bDebugTFACTOR;	// modulation factor
HRESULT DebugTexture(LPDIRECT3DTEXTURE8 pTexture, UINT iLevel);

//////////////////////////////////////////////////////////////////////
// Draw bounding box in world coords
//
extern D3DXVECTOR3 g_vDebugMin, g_vDebugMax;	// used if NULL is passed as pvMin or pvMax
extern HRESULT DebugBoundingBox(const D3DXVECTOR3 *pvMin, const D3DXVECTOR3 *pvMax, D3DCOLOR color);

//////////////////////////////////////////////////////////////////////
// Draw line segment in world coords
//
extern D3DXVECTOR3 g_vDebugStart, g_vDebugEnd;	// used if NULL is passed as pvStart or pvEnd
extern HRESULT DebugLine(const D3DXVECTOR3 *pvStart, const D3DXVECTOR3 *pvEnd);
extern HRESULT DebugLineColor(const D3DXVECTOR3 *pvStart, D3DCOLOR colorStart, const D3DXVECTOR3 *pvEnd, D3DCOLOR colorEnd);

//////////////////////////////////////////////////////////////////////
// Debug frustum
//
extern D3DXVECTOR3 g_vDebugFromDir;		// offset for viewing from "outside" of current frustum
extern D3DXVECTOR3 g_vDebugFrom;	// camera coords (post world and view), set by DebugSetFrustum depending on g_DebugFrustumMode
extern D3DXVECTOR3 g_vDebugAt;		// also set by DebugSetFrustum
extern D3DXVECTOR3 g_vDebugUp;		// used as up direction in DebugSetFrustum
enum DebugFrustumMode { 
	FRUSTUM_CENTER, // default is FRUSTUM_CENTER, where FromDir is scaled and offset from center of frustum
	FRUSTUM_FRONT,	// FromDir is offset (without scaling) from center of front clip plane
	FRUSTUM_POINT	// From and Up are left unchanged and used to set the offset frustum matrix
};
extern DebugFrustumMode g_DebugFrustumMode; 
HRESULT DebugSetFrustum();		// replaces current projection matrix with offset projection

HRESULT DebugFrustumBegin();	// set render target to back buffer and calls DebugSetFrustum
HRESULT DebugFrustumEnd();		// restores original projection and render target

/* Example use:
  // set projection before we get to here
  DebugFrustumBegin();
  // regular drawing calls 
  DebugFrustumEnd();
  DebugPresent(); // to show results
*/

//////////////////////////////////////////////////////////////////////
// Debug save texture
//
// Calls XGWriteSurfaceToFile after unswizzling, if needed.
//
HRESULT DebugSaveTexture(LPDIRECT3DTEXTURE8 pTexture, UINT iLevel, CHAR *strPath);

