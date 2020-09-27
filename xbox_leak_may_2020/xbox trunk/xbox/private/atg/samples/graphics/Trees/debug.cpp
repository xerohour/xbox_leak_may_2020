//-----------------------------------------------------------------------------
//  
//  File: debug.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Media viewers for debugging texture generation
//-----------------------------------------------------------------------------
#include "debug.h"
#include "SwizzleFormat.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

//////////////////////////////////////////////////////////////////////
// State set and restore on scope exit
//
#define SET_D3DTSS(STAGE, NAME, VALUE)\
	struct _D3DTSS_STAGE##STAGE##_##NAME {\
 		DWORD m_dw; \
		 _D3DTSS_STAGE##STAGE##_##NAME(DWORD dw) { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetTextureStageState(STAGE, NAME, &m_dw);\
			g_pd3dDevice->SetTextureStageState(STAGE, NAME, dw);\
		}\
		 ~_D3DTSS_STAGE##STAGE##_##NAME() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetTextureStageState(STAGE, NAME, m_dw);\
		}\
	} __D3DTSS_STAGE##STAGE##_##NAME(VALUE)
#define SET_D3DTEXTURE(STAGE, TEXTURE)\
	struct _D3DTEXTURE_STAGE##STAGE {\
 		D3DBaseTexture *m_pTexture; \
		 _D3DTEXTURE_STAGE##STAGE(D3DBaseTexture *pTexture) { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetTexture(STAGE, &m_pTexture);\
			g_pd3dDevice->SetTexture(STAGE, pTexture);\
		}\
		 ~_D3DTEXTURE_STAGE##STAGE() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetTexture(STAGE, m_pTexture);\
			 if (m_pTexture != NULL) m_pTexture->Release();\
		}\
	} __D3DTEXTURE_STAGE##STAGE(TEXTURE)
#define SET_D3DRS(NAME, VALUE)\
	struct _D3DRS_##NAME {\
 		DWORD m_dw; \
		 _D3DRS_##NAME(DWORD dw) { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetRenderState(NAME, &m_dw);\
			g_pd3dDevice->SetRenderState(NAME, dw);\
		}\
		 ~_D3DRS_##NAME() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetRenderState(NAME, m_dw);\
		}\
	} __D3DRS_##NAME(VALUE)
#define SET_D3DVERTEXSHADER(VALUE)\
	struct _D3DVERTEXSHADER {\
 		DWORD m_dw; \
		 _D3DVERTEXSHADER(DWORD dw) { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetVertexShader(&m_dw);\
			g_pd3dDevice->SetVertexShader(dw);\
		}\
		 ~_D3DVERTEXSHADER() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetVertexShader(m_dw);\
		}\
	} __D3DVERTEXSHADER(VALUE)
#define SET_D3DPIXELSHADER(VALUE)\
	struct _D3DPIXELSHADER {\
 		DWORD m_dw; \
		 _D3DPIXELSHADER(DWORD dw) { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetPixelShader(&m_dw);\
			g_pd3dDevice->SetPixelShader(dw);\
		}\
		 ~_D3DPIXELSHADER() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetPixelShader(m_dw);\
		}\
	} __D3DPIXELSHADER(VALUE)
#define SET_D3DVIEWPORT(PVIEWPORT)\
	struct _D3DVIEWPORT {\
 		D3DVIEWPORT8 m_viewport; \
		 _D3DVIEWPORT(D3DVIEWPORT *pViewport) { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetViewport(&m_viewport);\
			g_pd3dDevice->SetViewport(pViewport);\
		}\
		 ~_D3DVIEWPORT() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetViewport(&m_viewport);\
		}\
	} __D3DVIEWPORT(PVIEWPORT)
#define SET_D3DTRANSFORM(NAME, PTRANSFORM)\
	struct _D3DTRANSFORM_##NAME {\
 		D3DMATRIX m_transform; \
		 _D3DTRANSFORM_##NAME(D3DMATRIX *pTransform) { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetTransform(NAME, &m_transform);\
			g_pd3dDevice->SetTransform(NAME, pTransform);\
		}\
		 ~_D3DTRANSFORM_##NAME() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetTransform(NAME, &m_transform);\
		}\
	} __D3DTRANSFORM_##NAME(PTRANSFORM)

//////////////////////////////////////////////////////////////////////
// Make the backbuffer the render target temporarily
//
#define DEBUG_NSTACK 10
struct DebugSave {
	IDirect3DSurface8 *pBackBuffer, *pZBuffer;
	bool bBeginScene;
} g_rDebugSave[DEBUG_NSTACK];
UINT g_iDebugSave = 0;

HRESULT DebugPush(bool bSetRenderTarget = true)
{
	if (g_iDebugSave >= DEBUG_NSTACK)
		return E_FAIL;	// stack overflow
	
	// make sure we're not inside a BeginScene block
	g_rDebugSave[g_iDebugSave].bBeginScene = false;
	HRESULT hr = g_pd3dDevice->BeginScene();
	if (FAILED(hr))
		g_rDebugSave[g_iDebugSave].bBeginScene = true; // assume BeginScene failed because we're already inside a BeginScene
	g_pd3dDevice->EndScene();

	// make the backbuffer the current render target
	g_pd3dDevice->GetRenderTarget(&g_rDebugSave[g_iDebugSave].pBackBuffer);
	g_pd3dDevice->GetDepthStencilSurface(&g_rDebugSave[g_iDebugSave].pZBuffer);
	IDirect3DSurface8 *pBackBuffer = NULL;
	g_pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	g_pd3dDevice->SetRenderTarget(pBackBuffer, NULL);
	pBackBuffer->Release();
	
	// make sure all the commands so far are done
	g_pd3dDevice->BlockUntilIdle();
	
	g_iDebugSave++;
	return S_OK;
}

HRESULT DebugPop()
{
	if (g_iDebugSave == 0 || g_iDebugSave >= DEBUG_NSTACK)
		return E_FAIL;	// stack underflow or wackiness
	g_iDebugSave--;

	// restore render target, etc.
	g_pd3dDevice->SetRenderTarget(g_rDebugSave[g_iDebugSave].pBackBuffer, g_rDebugSave[g_iDebugSave].pZBuffer);
	if (g_rDebugSave[g_iDebugSave].pBackBuffer) g_rDebugSave[g_iDebugSave].pBackBuffer->Release();
	if (g_rDebugSave[g_iDebugSave].pZBuffer) g_rDebugSave[g_iDebugSave].pZBuffer->Release();
	if (g_rDebugSave[g_iDebugSave].bBeginScene)
		g_pd3dDevice->BeginScene();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Clear backbuffer
//
D3DXCOLOR g_DebugClearColor(0.3f, 0.3f, 0.4f, 1.f);
float g_fDebugClearZ = 1.f;
DWORD g_dwDebugClearStencil = 0L;

HRESULT DebugClear()
{
	HRESULT hr = DebugPush();
	if (FAILED(hr))
		return hr;

	// clear
	g_pd3dDevice->BeginScene();
	hr = g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, g_DebugClearColor, g_fDebugClearZ, g_dwDebugClearStencil);
	g_pd3dDevice->EndScene();
	
	return DebugPop();
}

//////////////////////////////////////////////////////////////////////
// Clear region of current render target (even if it's swizzled)
//
RECT g_DebugRect;	// scratch rectangle
HRESULT DebugRect(RECT *pRect, DWORD dwColor)
{
	struct quad {
		float x, y, z, w1;
	} aQuad[4] =
	  { //   X                  Y                     Z     1/W
		  {pRect->left  - 0.5f, pRect->top - 0.5f,    1.0f, 1.0f },
		  {pRect->right - 0.5f, pRect->top - 0.5f,    1.0f, 1.0f },
		  {pRect->left  - 0.5f, pRect->bottom - 0.5f, 1.0f, 1.0f },
		  {pRect->right - 0.5f, pRect->bottom - 0.5f, 1.0f, 1.0f }
	  };
	SET_D3DVERTEXSHADER( D3DFVF_XYZRHW );
	SET_D3DPIXELSHADER(0);
	SET_D3DRS( D3DRS_ZFUNC, D3DCMP_ALWAYS );
	SET_D3DRS( D3DRS_ZWRITEENABLE, FALSE );
	SET_D3DRS( D3DRS_ALPHATESTENABLE, FALSE );
	SET_D3DRS( D3DRS_NORMALIZENORMALS, FALSE );
	SET_D3DRS( D3DRS_ALPHABLENDENABLE, TRUE );
	SET_D3DRS( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	SET_D3DRS( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	SET_D3DRS( D3DRS_LIGHTING, FALSE );
	SET_D3DRS( D3DRS_TEXTUREFACTOR, dwColor );
	SET_D3DTEXTURE( 0, NULL );
	SET_D3DTSS( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	SET_D3DTSS( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
	SET_D3DTSS( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	SET_D3DTSS( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	SET_D3DTSS( 0, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 1, NULL );
	SET_D3DTSS( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 1, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 2, NULL );
	SET_D3DTSS( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 2, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 3, NULL );
	SET_D3DTSS( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 3, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );

	// make sure we're not inside a BeginScene block
	bool bBeginScene = false;
	HRESULT hr = g_pd3dDevice->BeginScene();
	if (FAILED(hr))
		bBeginScene = true; // assume BeginScene failed because we're already inside a BeginScene
	g_pd3dDevice->EndScene();

	// make sure all the commands so far are done before we draw the texture
	g_pd3dDevice->BlockUntilIdle();
	
	// clear rectangle
	g_pd3dDevice->BeginScene();
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, aQuad, sizeof(quad));
	g_pd3dDevice->EndScene();

	// clean up
	if (bBeginScene)
		g_pd3dDevice->BeginScene();
	return hr;
}

//////////////////////////////////////////////////////////////////////
// Swap backbuffer to the front
//
DWORD g_dwDebugPresentPause = 0;
HRESULT DebugPresent()
{
	HRESULT hr;
	hr = g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	if (g_dwDebugPresentPause)
		Sleep(g_dwDebugPresentPause);
	return hr;
}

//////////////////////////////////////////////////////////////////////
// Display texture
//
bool g_bDebugExpand = true;
bool g_bDebugExpandAspect = true;	// preserve aspect ratio when expanding
bool g_bDebugAlpha = false;
D3DXCOLOR g_bDebugTFACTOR(1.f, 1.f, 1.f, 1.f);

HRESULT DebugTexture(LPDIRECT3DTEXTURE8 pTexture, UINT iLevel)
{
	if (pTexture == NULL)
		return S_FALSE;
	DWORD nLevel = pTexture->GetLevelCount();
	if (iLevel >= nLevel) 
		return S_FALSE;
	HRESULT	hr = DebugPush();
	if (FAILED(hr))
		return hr;
	D3DDISPLAYMODE DisplayMode;
	g_pd3dDevice->GetDisplayMode(&DisplayMode);
	UINT Width, Height;
	if (g_bDebugExpand)
	{
		if (!g_bDebugExpandAspect)
			Width = Height = 400;
		else
		{
			// Maintain aspect ratio.  Shrink longest dimension to screen.
			D3DSURFACE_DESC desc;
			pTexture->GetLevelDesc(iLevel, &desc);
			if (desc.Width > desc.Height)
			{
				Width = 400;
				Height = desc.Height * Width / desc.Width;
			}
			else
			{
				Height = 400;
				Width = desc.Width * Height / desc.Height;
			}
		}
	}
	else
	{
		D3DSURFACE_DESC desc;
		pTexture->GetLevelDesc(iLevel, &desc);
		Width = desc.Width;
		Height = desc.Height;
	}
	UINT WidthOffset = (DisplayMode.Width - Width) / 2;		// center texture
	UINT HeightOffset = (DisplayMode.Height - Height) / 2;
	struct quad {
		float x, y, z, w1;
		float u, v;
	} aQuad[4] =
	  { //   X             Y                                          Z    1/W    u     v
		  {WidthOffset - 0.5f,         HeightOffset - 0.5f,          1.0f, 1.0f, 0.0f, 0.0f },
		  {WidthOffset + Width - 0.5f, HeightOffset - 0.5f,          1.0f, 1.0f, 1.0f, 0.0f },
		  {WidthOffset - 0.5f,         HeightOffset + Height - 0.5f, 1.0f, 1.0f, 0.0f, 1.0f },
		  {WidthOffset + Width - 0.5f, HeightOffset + Height - 0.5f, 1.0f, 1.0f, 1.0f, 1.0f }
	  };
	SET_D3DVERTEXSHADER( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	SET_D3DPIXELSHADER(0);
	SET_D3DRS( D3DRS_ZFUNC, D3DCMP_ALWAYS );
	SET_D3DRS( D3DRS_ZWRITEENABLE, FALSE );
	SET_D3DRS( D3DRS_ALPHATESTENABLE, FALSE );
	SET_D3DRS( D3DRS_NORMALIZENORMALS, FALSE );
	SET_D3DRS( D3DRS_ALPHABLENDENABLE, FALSE );
	SET_D3DRS( D3DRS_LIGHTING, FALSE );
	SET_D3DRS( D3DRS_TEXTUREFACTOR, g_bDebugTFACTOR );
	SET_D3DTEXTURE( 0, pTexture );
	SET_D3DTSS( 0, D3DTSS_MAXMIPLEVEL, iLevel);
	float fBias = -1000.f; // bias mipmap toward the more detailed level
	SET_D3DTSS( 0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
	SET_D3DTSS( 0, D3DTSS_MINFILTER, D3DTEXF_POINT);
	SET_D3DTSS( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	SET_D3DTSS( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	SET_D3DTSS( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	SET_D3DTSS( 0, D3DTSS_COLORARG1, g_bDebugAlpha ? D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE : D3DTA_TEXTURE );
	SET_D3DTSS( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
	SET_D3DTSS( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	SET_D3DTSS( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	SET_D3DTSS( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
	SET_D3DTSS( 0, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 1, NULL );
	SET_D3DTSS( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 1, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 2, NULL );
	SET_D3DTSS( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 2, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 3, NULL );
	SET_D3DTSS( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 3, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );

	// draw the texture
	g_pd3dDevice->BeginScene();
	g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
		g_DebugClearColor, g_fDebugClearZ, g_dwDebugClearStencil);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, aQuad, sizeof(quad));
	g_pd3dDevice->EndScene();
	DebugPresent();
	
	return DebugPop();
}

//////////////////////////////////////////////////////////////////////
// Draw bounding box in world coords
//
D3DXVECTOR3 g_vDebugMin, g_vDebugMax;	// used if NULL is passed as pvMin or pvMax
HRESULT DebugBoundingBox(const D3DXVECTOR3 *pvMin, const D3DXVECTOR3 *pvMax, D3DCOLOR color)
{
	// set and save state
	SET_D3DRS(D3DRS_LIGHTING, FALSE);
//	SET_D3DRS(D3DRS_ZENABLE, D3DZB_FALSE);
//	SET_D3DRS(D3DRS_ZWRITEENABLE, FALSE );
	SET_D3DRS(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
	SET_D3DVERTEXSHADER(D3DFVF_XYZ|D3DFVF_DIFFUSE); // vertex shader
	SET_D3DTEXTURE( 0, NULL);
	SET_D3DTSS( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	SET_D3DTSS( 0,D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	SET_D3DTSS( 0,D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 1, NULL);
	SET_D3DTSS( 1,D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 1,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 2, NULL);
	SET_D3DTSS( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 2, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 3, NULL);
	SET_D3DTSS( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 3, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );

	// draw world-coordinate bounding box
	D3DXVECTOR3 vMin = pvMin ? *pvMin : g_vDebugMin;
	D3DXVECTOR3 vMax = pvMax ? *pvMax : g_vDebugMax;
	struct BoundingBoxVertex {
		D3DVECTOR xyz;
		D3DCOLOR color;
	} rBoundingBoxVertex[8] = {
		{ vMin.x, vMin.y, vMin.z, 0xff444444 & color },	// masks highlight the different corners
		{ vMax.x, vMin.y, vMin.z, 0xffff4444 & color },
		{ vMin.x, vMax.y, vMin.z, 0xff44ff44 & color },
		{ vMax.x, vMax.y, vMin.z, 0xffffff44 & color },
		{ vMin.x, vMin.y, vMax.z, 0xff4444ff & color },
		{ vMax.x, vMin.y, vMax.z, 0xffff44ff & color },
		{ vMin.x, vMax.y, vMax.z, 0xff44ffff & color },
		{ vMax.x, vMax.y, vMax.z, 0xffffffff & color },
	};
	static WORD rBoundingBoxLines[] = {
		0, 1,	1, 3,	3, 2,	2, 0,
		0, 4,	1, 5,	2, 6,	3, 7,
		4, 5,	5, 7,	7, 6,	6, 4,
	};
	static UINT nBoundingBoxLines = sizeof(rBoundingBoxLines) / sizeof(WORD) / 2;
    g_pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 8, nBoundingBoxLines, rBoundingBoxLines, 
		D3DFMT_INDEX16, rBoundingBoxVertex, sizeof(BoundingBoxVertex));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Draw line segment in world coords
//
D3DXVECTOR3 g_vDebugStart, g_vDebugEnd;	// used if NULL is passed as pvStart or pvEnd

HRESULT DebugLine(const D3DXVECTOR3 *pvStart, const D3DXVECTOR3 *pvEnd)
{
	return DebugLineColor(pvStart, 0xffff0000, pvEnd, 0xff00ffff);	// red to cyan
}

HRESULT DebugLineColor(const D3DXVECTOR3 *pvStart, D3DCOLOR colorStart, const D3DXVECTOR3 *pvEnd, D3DCOLOR colorEnd)
{
	// set and save state
	SET_D3DRS(D3DRS_LIGHTING, FALSE);
//	SET_D3DRS(D3DRS_ZENABLE, D3DZB_FALSE);
//	SET_D3DRS(D3DRS_ZWRITEENABLE, FALSE );
	SET_D3DRS(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
	SET_D3DVERTEXSHADER(D3DFVF_XYZ|D3DFVF_DIFFUSE); // vertex shader
	SET_D3DTEXTURE( 0, NULL);
	SET_D3DTSS( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	SET_D3DTSS( 0,D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	SET_D3DTSS( 0,D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 1, NULL);
	SET_D3DTSS( 1,D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 1,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 2, NULL);
	SET_D3DTSS( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 2, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 3, NULL);
	SET_D3DTSS( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 3, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );

	// draw world-coordinate line
 	D3DXVECTOR3 vStart = pvStart ? *pvStart : g_vDebugStart;
	D3DXVECTOR3 vEnd = pvEnd ? *pvEnd : g_vDebugEnd;
	struct LineVertex {
		D3DVECTOR xyz;
		D3DCOLOR color;
	} rLineVertex[2] = {
		{ vStart.x, vStart.y, vStart.z, colorStart },
		{ vEnd.x, vEnd.y, vEnd.z, colorEnd }
	};
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, rLineVertex, sizeof(LineVertex));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Debug frustum drawing
//
D3DXVECTOR3 g_vDebugFromDir(0.1f, 0.2f, -0.9f);
DebugFrustumMode g_DebugFrustumMode = FRUSTUM_CENTER;
D3DXVECTOR3 g_vDebugFrustumWorldPoint(0.f, 0.f, 0.f);
bool g_bDebugFromFront = false;	// if set, the debug frustum is offset from the front of the clip region rather than the center
bool g_bDebugFrustumOverrideClip = true;
float g_bDebugFrustumNear = 0.1f;	// used when g_bDebugFrustumOverrideClip is true
float g_bDebugFrustumFar = 10000.f;
D3DXVECTOR3 g_vDebugFrom; // set by DebugFrustumBegin to center of frustum + radius * g_vDebugFromDir
D3DXVECTOR3 g_vDebugAt; // set by DebugFrustumBegin to center of frustum
D3DXVECTOR3 g_vDebugUp(0.f, 1.f, 0.f);
D3DXMATRIX g_matDebugFrustumSave;
D3DXMATRIX g_matDebugFrustumSaveInv;
D3DXMATRIX g_matDebugFrustumOutside;	// modified from g_matDebugFrustumSave to view from "outside" of original frustum
bool g_bDebugFrustumBegin = false;
bool g_bDebugFrustumBeginScene;

static struct FrustumVertex {
	D3DVECTOR xyz;
	D3DCOLOR color;
} rFrustumVertex[8] = {
	{ -1.f, -1.f, 0.f, 0xff444400 },
	{  1.f, -1.f, 0.f, 0xffff4400 },
	{ -1.f,  1.f, 0.f, 0xff44ff00 },
	{  1.f,  1.f, 0.f, 0xffffff00 },
	{ -1.f, -1.f, 1.f, 0xff4444ff },
	{  1.f, -1.f, 1.f, 0xffff44ff },
	{ -1.f,  1.f, 1.f, 0xff44ffff },
	{  1.f,  1.f, 1.f, 0xffffffff },
};
static WORD rFrustumLines[] = {
	0, 1,	1, 3,	3, 2,	2, 0,
	0, 4,	1, 5,	2, 6,	3, 7,
	4, 5,	5, 7,	7, 6,	6, 4,
};
static UINT nFrustumLines = sizeof(rFrustumLines) / sizeof(WORD) / 2;

HRESULT DebugDrawFrustum()
{
	// set and save state
	SET_D3DRS(D3DRS_LIGHTING, FALSE);
//	SET_D3DRS(D3DRS_ZENABLE, D3DZB_FALSE);
//	SET_D3DRS(D3DRS_ZWRITEENABLE, FALSE );
	SET_D3DRS(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
	SET_D3DVERTEXSHADER(D3DFVF_XYZ|D3DFVF_DIFFUSE); // vertex shader
	SET_D3DTEXTURE( 0, NULL);
	SET_D3DTSS( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	SET_D3DTSS( 0,D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	SET_D3DTSS( 0,D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 1, NULL);
	SET_D3DTSS( 1,D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 1,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 2, NULL);
	SET_D3DTSS( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 2, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE( 3, NULL);
	SET_D3DTSS( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 3, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	SET_D3DTRANSFORM( D3DTS_WORLD, &matIdentity);
	SET_D3DTRANSFORM( D3DTS_VIEW, &matIdentity);

	// transform frustum vertices
	FrustumVertex rVertex[8];
	for (UINT i = 0; i < 8; i++)
	{
		D3DXVec3TransformCoord((D3DXVECTOR3 *)&rVertex[i].xyz, (D3DXVECTOR3 *)&rFrustumVertex[i].xyz, &g_matDebugFrustumSaveInv);
		rVertex[i].color = rFrustumVertex[i].color;
	}
	
	// draw world-coordinate frustum
    g_pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 8, nFrustumLines, rFrustumLines, D3DFMT_INDEX16, rVertex, sizeof(FrustumVertex));
	return S_OK;
}

inline float MAX(float a, float b) { return a > b ? a : b; }
inline float MIN(float a, float b) { return a < b ? a : b; }
static void UnionBox(D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax, const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax)
{
	pvMin->x = MIN(pvMin->x, vMin.x);
	pvMin->y = MIN(pvMin->y, vMin.y);
	pvMin->z = MIN(pvMin->z, vMin.z);
	pvMax->x = MAX(pvMax->x, vMax.x);
	pvMax->y = MAX(pvMax->y, vMax.y);
	pvMax->z = MAX(pvMax->z, vMax.z);
}

HRESULT DebugSetFrustum()
{
	// Invert current frustum
	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &g_matDebugFrustumSave );
	float fDet;
	D3DXMatrixInverse(&g_matDebugFrustumSaveInv, &fDet, &g_matDebugFrustumSave);
	
	// Get bounding box of inverse frustum in view coords
	D3DXVECTOR3 vMin(FLT_MAX, FLT_MAX, FLT_MAX), vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	FrustumVertex rVertex[8];
	for (UINT i = 0; i < 8; i++)
	{
		D3DXVec3TransformCoord((D3DXVECTOR3 *)&rVertex[i].xyz, (D3DXVECTOR3 *)&rFrustumVertex[i].xyz, &g_matDebugFrustumSaveInv);
		UnionBox(&vMin, &vMax, rVertex[i].xyz, rVertex[i].xyz);
	}

	// Find a reasonable viewing position 
	switch (g_DebugFrustumMode)
	{
	case FRUSTUM_CENTER:
	{
		// offset from center of bounding box
		D3DXVECTOR3 vCenter = 0.5f * (vMin + vMax);
		D3DXVECTOR3 vCorner = vMax - vCenter;
		float fRadius = D3DXVec3Length(&vCorner);
		g_vDebugAt = vCenter;
		g_vDebugFrom = vCenter + fRadius * g_vDebugFromDir;
		break;
	}

	case FRUSTUM_FRONT:
	{
		// offset from center of near clip plane
		D3DXVECTOR3 vCenter = 0.5f * (vMin + vMax);
		vCenter.z = vMin.z;
		g_vDebugAt = vCenter;
		g_vDebugFrom = vCenter + g_vDebugFromDir;
		break;
	}

	case FRUSTUM_POINT:
		// leave From and At as is
		break;
	}
	D3DXMATRIX matViewOutside;
	D3DXMatrixLookAtLH( &matViewOutside, &g_vDebugFrom, &g_vDebugAt, &g_vDebugUp);

	// Get clipping planes
	D3DXVECTOR3 vEye = g_vDebugAt - g_vDebugFrom;
	D3DXVec3Normalize(&vEye, &vEye);
	float fMin = FLT_MAX, fMax = -FLT_MAX;
	for (i = 0; i < 8; i++)
	{
		D3DXVECTOR3 vOffset = *(D3DXVECTOR3 *)&rVertex[i].xyz - g_vDebugFrom;
		float f = D3DXVec3Dot(&vOffset, &vEye);
		if (f < fMin) fMin = f;
		if (f > fMax) fMax = f;
	}
	// override clip
	if (g_bDebugFrustumOverrideClip)
	{
		fMin = g_bDebugFrustumNear;
		fMax = g_bDebugFrustumFar;
	}
	
	// Make new projection that maps to "outside view"
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.f/480.f, MAX(0.1f, fMin), fMax );
	g_matDebugFrustumOutside = /* g_matDebugFrustumSave * g_matDebugFrustumSaveInv * */ matViewOutside * matProj;
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_matDebugFrustumOutside );

	float fEps = 1e-6f;
	if (fabs(fDet) < fEps)
		return S_FALSE;	// singular matrix
	else
		return S_OK;
}

HRESULT DebugFrustumBegin()
{
	HRESULT hr;
	if (g_bDebugFrustumBegin)
		return E_FAIL;	// DebugFrustumBegin called twice
	g_bDebugFrustumBegin = true;
	hr = DebugPush();
	if (FAILED(hr))
		return hr;

	hr = DebugSetFrustum();
	if (FAILED(hr))
		return hr;

//	// Draw the frustum
//	g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, g_DebugClearColor, g_fDebugClearZ, g_dwDebugClearStencil);
//	DebugDrawFrustum();

	return hr;
}

HRESULT DebugFrustumEnd()
{
	if (!g_bDebugFrustumBegin)
		return E_FAIL;
	
	// Draw the frustum at the end
	DebugDrawFrustum();

	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_matDebugFrustumSave );	// restore projection
	g_bDebugFrustumBegin = false;
	return DebugPop();
}

//////////////////////////////////////////////////////////////////////
// Debug save texture level to a file
//
HRESULT DebugSaveTexture(LPDIRECT3DTEXTURE8 pTexture, UINT iLevel, CHAR *strPath)
{
	HRESULT hr;
	if (pTexture == NULL)
		return S_FALSE;
	DWORD nLevel = pTexture->GetLevelCount();
	if (iLevel >= nLevel) 
		return S_FALSE;
	D3DSURFACE_DESC desc;
	hr = pTexture->GetLevelDesc(iLevel, &desc);
	if (FAILED(hr))
		return hr;
	LPDIRECT3DSURFACE8 pSurface = NULL;
	hr = pTexture->GetSurfaceLevel(iLevel, &pSurface);
	if (FAILED(hr))
		return hr;
	if (XGIsSwizzledFormat(desc.Format))
	{
		D3DFORMAT fmt = MapSwizzledToLinearFormat(desc.Format);
		if (fmt == desc.Format)
		{
			pSurface->Release();
			return E_FAIL;	// format not supported
		}

		// Create temporary surface to unswizzle
		LPDIRECT3DSURFACE8 pSurfaceLinear = NULL;
		hr = g_pd3dDevice->CreateImageSurface(desc.Width, desc.Height, fmt, &pSurfaceLinear);
		if (FAILED(hr))
		{
			pSurface->Release();
			return hr;
		}

		// Unswizzle the texture
		D3DLOCKED_RECT lockedRect;
		hr = pSurface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY);
		if (FAILED(hr))
		{
			pSurface->Release();
			pSurfaceLinear->Release();
			return hr;
		}
		D3DLOCKED_RECT lockedRectLinear;
		hr = pSurfaceLinear->LockRect(&lockedRectLinear, NULL, 0);
		if (FAILED(hr))
		{
			pSurface->UnlockRect();
			pSurface->Release();
			pSurfaceLinear->Release();
			return hr;
		}
		XGUnswizzleRect(lockedRect.pBits, desc.Width, desc.Height, NULL, 
			lockedRectLinear.pBits, lockedRectLinear.Pitch, NULL, 
			XGBytesPerPixelFromFormat(desc.Format));
		pSurfaceLinear->UnlockRect();
		pSurface->UnlockRect();

		// Use the linear surface instead of the swizzled one
		pSurface->Release();
		pSurface = pSurfaceLinear; // this will be Release'd below
	}
	hr = XGWriteSurfaceToFile(pSurface, strPath);
	pSurface->Release();
	return hr;
}

