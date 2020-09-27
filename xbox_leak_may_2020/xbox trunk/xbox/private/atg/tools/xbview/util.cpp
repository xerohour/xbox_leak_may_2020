//-----------------------------------------------------------------------------
// File: util.cpp
// Desc: Handy utility functions
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "util.h"
#include <tchar.h>

extern LPDIRECT3DDEVICE8 g_pd3dDevice;	// Defined by XbApp

//-----------------------------------------------------------------------------
// Macros for setting state and restoring on scope exit, which are
// used to set just the state needed for a debug routine.
//-----------------------------------------------------------------------------
#define SET_D3DTSS(STAGE, NAME, VALUE)\
	struct _D3DTSS_STAGE##STAGE##_##NAME {\
 		DWORD dw; \
		 _D3DTSS_STAGE##STAGE##_##NAME() { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetTextureStageState(STAGE, NAME, &dw);\
			g_pd3dDevice->SetTextureStageState(STAGE, NAME, VALUE);\
		}\
		 ~_D3DTSS_STAGE##STAGE##_##NAME() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetTextureStageState(STAGE, NAME, dw);\
		}\
	} __D3DTSS_STAGE##STAGE##_##NAME
#define SET_D3DTEXTURE(STAGE, TEXTURE)\
	struct _D3DTEXTURE_STAGE##STAGE {\
 		D3DBaseTexture *__pTexture; \
		 _D3DTEXTURE_STAGE##STAGE(D3DBaseTexture *pTexture) { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetTexture(STAGE, &__pTexture);\
			g_pd3dDevice->SetTexture(STAGE, pTexture);\
		}\
		 ~_D3DTEXTURE_STAGE##STAGE() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetTexture(STAGE, __pTexture);\
			 if (__pTexture != NULL) __pTexture->Release();\
		}\
	} __D3DTEXTURE_STAGE##STAGE(TEXTURE)
#define SET_D3DTEXTURE_NULL(STAGE)\
	struct _D3DTEXTURE_STAGE##STAGE {\
 		D3DBaseTexture *__pTexture; \
		 _D3DTEXTURE_STAGE##STAGE() { /* constructor is called to save current value and set NULL value */ \
 			g_pd3dDevice->GetTexture(STAGE, &__pTexture);\
			g_pd3dDevice->SetTexture(STAGE, NULL);\
		}\
		 ~_D3DTEXTURE_STAGE##STAGE() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetTexture(STAGE, __pTexture);\
			 if (__pTexture != NULL) __pTexture->Release();\
		}\
	} __D3DTEXTURE_NULL_STAGE##STAGE
#define SET_D3DRS(NAME, VALUE)\
	struct _D3DRS_##NAME {\
 		DWORD dw; \
		 _D3DRS_##NAME() { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetRenderState(NAME, &dw);\
			g_pd3dDevice->SetRenderState(NAME, VALUE);\
		}\
		 ~_D3DRS_##NAME() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetRenderState(NAME, dw);\
		}\
	} __D3DRS_##NAME
#define SET_D3DVS(VALUE)\
	struct _D3DVS {\
 		DWORD dw; \
		 _D3DVS() { /* constructor is called to save current value and set new value */ \
 			g_pd3dDevice->GetVertexShader(&dw);\
			g_pd3dDevice->SetVertexShader(VALUE);\
		}\
		 ~_D3DVS() { /* on scope exit, saved value is restored */ \
			 g_pd3dDevice->SetVertexShader(dw);\
		}\
	} __D3DVS

//-----------------------------------------------------------------------------
// Macros used by the bounding sphere calculation routine
//-----------------------------------------------------------------------------
inline float MAX(float a, float b) { return a > b ? a : b; }
inline float MIN(float a, float b) { return a < b ? a : b; }

//-----------------------------------------------------------------------------
//  Take the union of two spheres
//-----------------------------------------------------------------------------
void UnionSphere(D3DXVECTOR3 *pvCenter, float *pfRadius, 
				 const D3DXVECTOR3 &vCenter, const float &fRadius)
{
	D3DXVECTOR3 v =  vCenter - *pvCenter;
	float vlen = D3DXVec3Length(&v);
	float a = MIN(-*pfRadius, vlen - fRadius);
	float b = MAX(*pfRadius, vlen + fRadius);
	*pfRadius = 0.5f * (b - a);
	*pvCenter += (0.5f * (b + a) / vlen) * v;
}

//-----------------------------------------------------------------------------
//  Find the bounding sphere for the vertices
//-----------------------------------------------------------------------------
HRESULT ComputeBoundingSphere(
        PVOID pPointsFVF, 
        DWORD NumVertices, 
        DWORD FVF,
        D3DXVECTOR3 *pvCenter, 
        FLOAT *pfRadius)
{
	if (NumVertices == 0)
	{
		pvCenter->x = pvCenter->y = pvCenter->z = 0.f;
		*pfRadius = 0;
		return S_FALSE;
	}
	UINT cbVertexSize = D3DXGetFVFVertexSize(FVF);
	// use first vertex as starting point
	float feps = 1e-6f;  // use a small epsilon as zero radius
	*pvCenter = *(D3DXVECTOR3 *)pPointsFVF;
	*pfRadius = feps;
	// union in the rest of the points as zero-radius spheres
	for (UINT iVertex = 1; iVertex < NumVertices; iVertex++)
		UnionSphere(pvCenter, pfRadius, *(D3DXVECTOR3 *)((BYTE *)pPointsFVF + cbVertexSize * iVertex), feps);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Draw hedgehog vectors with origin at each vertex.
// Lines go from Position to Position + fVectorScale * Vector.
//-----------------------------------------------------------------------------
HRESULT DrawVertexVectors(DWORD nVertex,		// number of lines to draw
						  LPDIRECT3DVERTEXBUFFER8 pVBPosition, DWORD cbPositionSize, DWORD cbPositionOffset,	// origin of lines
						  LPDIRECT3DVERTEXBUFFER8 pVBVector, DWORD cbVectorSize, DWORD cbVectorOffset,		// offset of lines
						  float fVectorScale,	// scale
						  DWORD color0,		// color at base
						  DWORD color1)		// color at tip
{
	HRESULT hr = S_OK;
	const UINT nLineBuf = 1000;
	struct Vertex {
		float x, y, z;
		D3DCOLOR diffuse;
	} aVertex[nLineBuf*2];
	Vertex *pVertex = &aVertex[0];
	BYTE *pVertexPosition = NULL;
	BYTE *pVertexVector = NULL;
	UINT nLine = 0;
	SET_D3DRS( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	SET_D3DRS( D3DRS_ZWRITEENABLE, TRUE);
	SET_D3DRS( D3DRS_ALPHATESTENABLE, FALSE);
	SET_D3DRS( D3DRS_NORMALIZENORMALS, FALSE);
	SET_D3DRS( D3DRS_ALPHABLENDENABLE, FALSE );
	SET_D3DRS( D3DRS_LIGHTING, FALSE);
	SET_D3DVS(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	SET_D3DTEXTURE_NULL( 0 );
	SET_D3DTSS( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	SET_D3DTSS( 0,D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	SET_D3DTSS( 0,D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE_NULL( 1 );
	SET_D3DTSS( 1,D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 1,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE_NULL( 2 );
	SET_D3DTSS( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 2, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE_NULL( 3 );
	SET_D3DTSS( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 3, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	hr = pVBPosition->Lock(NULL, NULL, (BYTE **)&pVertexPosition, 0l );
	if (FAILED(hr))
		goto e_Exit;
	hr = pVBVector->Lock(NULL, NULL, (BYTE **)&pVertexVector, 0l );
	if (FAILED(hr))
		goto e_Exit;
	UINT iVertex;
	for (iVertex = 0; iVertex < nVertex; iVertex++)
	{
		float *pfPosition = (float *)(pVertexPosition + cbPositionSize * iVertex + cbPositionOffset);
		float *pfVector = (float *)(pVertexVector + cbVectorSize * iVertex + cbVectorOffset);
		pVertex[0].x = pfPosition[0];
		pVertex[0].y = pfPosition[1];
		pVertex[0].z = pfPosition[2];
		pVertex[0].diffuse = color0;
		pVertex[1].x = pVertex[0].x + pfVector[0] * fVectorScale;
		pVertex[1].y = pVertex[0].y + pfVector[1] * fVectorScale;
		pVertex[1].z = pVertex[0].z + pfVector[2] * fVectorScale;
		pVertex[1].diffuse = color1;
		pVertex += 2;
		nLine++;
		if (nLine == nLineBuf)
		{
			// buffer is full, so draw what we have
			hr = g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, nLine, aVertex, sizeof(Vertex));
			if (FAILED(hr))
				goto e_Exit;
			nLine = 0;
			pVertex = &aVertex[0];
		}
	}
	if (nLine != 0)
	{
		// draw any left over lines
		hr = g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, nLine, aVertex, sizeof(Vertex));
		if (FAILED(hr))
			goto e_Exit;
	}
e_Exit:
	if (pVertexPosition)
		pVBPosition->Unlock();
	if (pVertexVector)
		pVBVector->Unlock();
	return hr;
}

HRESULT DrawNormals(DWORD nVertex, LPDIRECT3DVERTEXBUFFER8 pVB, DWORD dwFVF, float fNormalScale)
{
	if (!((dwFVF & D3DFVF_XYZ) && (dwFVF & D3DFVF_NORMAL)))
		return S_FALSE; // no normals to draw
	UINT cbVertexSize = D3DXGetFVFVertexSize(dwFVF);
	return DrawVertexVectors(nVertex,
							 pVB, cbVertexSize, 0 /* position offset */,
							 pVB, cbVertexSize, 12 /* normal offset */,
							 fNormalScale,
							 D3DCOLOR_RGBA(0x00, 0xff, 0x00, 0xff),
							 D3DCOLOR_RGBA(0x00, 0xff, 0xff, 0xff));
}

//-----------------------------------------------------------------------------
//  Draw coordinate axes.
//-----------------------------------------------------------------------------

static struct CAxesState {
	SET_D3DRS(D3DRS_LIGHTING, FALSE);
	SET_D3DRS(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
	SET_D3DVS(D3DFVF_XYZ|D3DFVF_DIFFUSE); // vertex shader
	SET_D3DTEXTURE_NULL( 0 );
	SET_D3DTSS( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	SET_D3DTSS( 0,D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	SET_D3DTSS( 0,D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	SET_D3DTSS( 0,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE_NULL( 1 );
	SET_D3DTSS( 1,D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 1,D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE_NULL( 2 );
	SET_D3DTSS( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 2, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	SET_D3DTEXTURE_NULL( 3 );
	SET_D3DTSS( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SET_D3DTSS( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	SET_D3DTSS( 3, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
} *s_pAxesState = NULL;

HRESULT BeginAxes()
{
	if (s_pAxesState != NULL)
		return E_FAIL;
	s_pAxesState = new CAxesState;
	if (s_pAxesState == NULL)
		return E_OUTOFMEMORY;
	return S_OK;
}

HRESULT EndAxes()
{
	if (s_pAxesState == NULL)
		return E_FAIL;
	delete s_pAxesState;
	s_pAxesState = NULL;
	return S_OK;
}

HRESULT DrawAxes()
{
	const float fAxisRodRadius = 0.05f;
	const float fAxisRodLength = 0.8f;
	const float fAxisConeRadius = fAxisRodRadius * 3.f;
	const float fAxisConeLength = 1.f;
	static struct AxisVertex {
		float x, y, z;
		D3DCOLOR color;
	} XAxis[] = {
		{ 0.f,             0.f,				 0.f }, 
		{ fAxisRodLength,  fAxisRodRadius,   0.f }, 
		{ fAxisRodLength,  fAxisConeRadius,  0.f }, 
		{ fAxisConeLength, 0.f,              0.f }, 
		{ fAxisRodLength,  0.f,              fAxisConeRadius }, 
		{ fAxisRodLength,  0.f,              fAxisRodRadius }, 
		{ 0.f,             0.f,              0.f },
		{ fAxisConeLength, 0.f,              0.f }, 
	};
	const int nVertex = sizeof(XAxis)/sizeof(AxisVertex);
	static AxisVertex YAxis[nVertex];
	static AxisVertex ZAxis[nVertex];
	static bool bInitialized = false;
	if (!bInitialized)
	{
		const D3DCOLOR XColor = 0xffff0000;
		const D3DCOLOR YColor = 0xff00ff00;
		const D3DCOLOR ZColor = 0xff0000ff;
		for (UINT i = 0; i < nVertex; i++)
		{
			XAxis[i].color = XColor;
			YAxis[i].x = XAxis[i].y; YAxis[i].y = XAxis[i].x; YAxis[i].z = XAxis[i].z; // swap x and y
			YAxis[i].color = YColor;
			ZAxis[i].x = XAxis[i].z; ZAxis[i].y = XAxis[i].y; ZAxis[i].z = XAxis[i].x; // swap x and z
			ZAxis[i].color = ZColor;
		}
		bInitialized = true;
	}

	if (s_pAxesState != NULL)
	{
		// BeginAxes was called, so assume state is set and just draw the axes
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, nVertex - 1, XAxis, sizeof(AxisVertex));
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, nVertex - 1, YAxis, sizeof(AxisVertex));
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, nVertex - 1, ZAxis, sizeof(AxisVertex));	
	}
	else
	{
		// set and save state
		CAxesState AxesState;

		// draw the axes
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, nVertex - 1, XAxis, sizeof(AxisVertex));
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, nVertex - 1, YAxis, sizeof(AxisVertex));
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, nVertex - 1, ZAxis, sizeof(AxisVertex));	
	}
	return S_OK;
}

//=========================================================================
// Prepare a device for a DrawSpriteSimple operation
//=========================================================================
void PrepareDeviceForDrawSprite(IDirect3DDevice8* pDevice)
{
    pDevice->SetPixelShader(0);

    pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    pDevice->SetRenderState(D3DRS_ALPHAREF, 0x00000001);

    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);

    pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
}

//=========================================================================
// Draw a screen space texture sprite
//=========================================================================
void DrawSpriteSimple(LPDIRECT3DDEVICE8 pDevice,
    LPDIRECT3DTEXTURE8 pTexture, const D3DXVECTOR3 &ptDest,
    float alpha, float scale, const RECT *pSourceRect)
{
    SIZE size;
    RECT rcSrc;
    D3DSURFACE_DESC Desc;

    (void)pTexture->GetLevelDesc(0, &Desc);
    if(!pSourceRect)
    {
        rcSrc.left = 0;
        rcSrc.top = 0;
        rcSrc.right = Desc.Width;
        rcSrc.bottom = Desc.Height;

        pSourceRect = &rcSrc;
    }

    size.cx = pSourceRect->right - pSourceRect->left;
    size.cy = pSourceRect->bottom - pSourceRect->top;

    // Coordinates for Rendering Target in Screen space
    float left   = ptDest.x;
    float right  = ptDest.x + (float)size.cx*scale;
    float top    = ptDest.y;
    float bottom = ptDest.y + (float)size.cy*scale;

    // Setup texture coordinates to use
    float fTexLeft   = (float)pSourceRect->left   / (float)Desc.Width;
    float fTexTop    = (float)pSourceRect->top    / (float)Desc.Height;
    float fTexRight  = (float)pSourceRect->right  / (float)Desc.Width;
    float fTexBottom = (float)pSourceRect->bottom / (float)Desc.Height;

    // Setup alpha term to modulate in
    DWORD dwColor = ((DWORD)(alpha * 255.0f) << 24) | 0x00FFFFFF;

    struct MYD3DTLVERTEX
    {
        float   sx, sy, sz; // Screen coordinates
        float   rhw;        // Reciprocal of homogeneous w
        DWORD   color;      // Vertex color
        float   tu, tv;     // Texture coordinates
    } pvSprite[4] =
    {
        { left-0.5f,    top-0.5f, ptDest.z, 1.0f, dwColor, fTexLeft,  fTexTop},
        {right-0.5f,    top-0.5f, ptDest.z, 1.0f, dwColor, fTexRight, fTexTop},
        {right-0.5f, bottom-0.5f, ptDest.z, 1.0f, dwColor, fTexRight, fTexBottom},
        { left-0.5f, bottom-0.5f, ptDest.z, 1.0f, dwColor, fTexLeft,  fTexBottom},
    };

    pDevice->SetTexture(0, pTexture);
    pDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

    // Display the sprite
    pDevice->DrawVerticesUP(D3DPT_QUADLIST, 4, pvSprite, sizeof(pvSprite[0]));
}

//-----------------------------------------------------------------------------
//  Return string corresponding to texture op
//-----------------------------------------------------------------------------
WCHAR *GetString_D3DTEXTUREOP(DWORD d3dtop)
{
	switch (d3dtop)
	{
	case 0:
		return L"<NONE>";
    case D3DTOP_DISABLE:
		 return L"D3DTOP_DISABLE";
    case D3DTOP_SELECTARG1:
		 return L"D3DTOP_SELECTARG1";
    case D3DTOP_SELECTARG2:
		 return L"D3DTOP_SELECTARG2";
    case D3DTOP_MODULATE:
		 return L"D3DTOP_MODULATE";
    case D3DTOP_MODULATE2X:
		 return L"D3DTOP_MODULATE2X";
    case D3DTOP_MODULATE4X:
		 return L"D3DTOP_MODULATE4X";
    case D3DTOP_ADD:
		 return L"D3DTOP_ADD";
    case D3DTOP_ADDSIGNED:
		 return L"D3DTOP_ADDSIGNED";
    case D3DTOP_ADDSIGNED2X:
		 return L"D3DTOP_ADDSIGNED2X";
    case D3DTOP_SUBTRACT:
		 return L"D3DTOP_SUBTRACT";
    case D3DTOP_ADDSMOOTH:
		 return L"D3DTOP_ADDSMOOTH";
    case D3DTOP_BLENDDIFFUSEALPHA:
		 return L"D3DTOP_BLENDDIFFUSEALPHA";
    case D3DTOP_BLENDTEXTUREALPHA:
		 return L"D3DTOP_BLENDTEXTUREALPHA";
    case D3DTOP_BLENDFACTORALPHA:
		 return L"D3DTOP_BLENDFACTORALPHA";
    case D3DTOP_BLENDTEXTUREALPHAPM:
		 return L"D3DTOP_BLENDTEXTUREALPHAPM";
    case D3DTOP_BLENDCURRENTALPHA:
		 return L"D3DTOP_BLENDCURRENTALPHA";
    case D3DTOP_PREMODULATE:
		 return L"D3DTOP_PREMODULATE";
    case D3DTOP_MODULATEALPHA_ADDCOLOR:
		 return L"D3DTOP_MODULATEALPHA_ADDCOLOR";
    case D3DTOP_MODULATECOLOR_ADDALPHA:
		 return L"D3DTOP_MODULATECOLOR_ADDALPHA";
    case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
		 return L"D3DTOP_MODULATEINVALPHA_ADDCOLOR";
    case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
		 return L"D3DTOP_MODULATEINVCOLOR_ADDALPHA";
    case D3DTOP_BUMPENVMAP:
		 return L"D3DTOP_BUMPENVMAP";
    case D3DTOP_BUMPENVMAPLUMINANCE:
		 return L"D3DTOP_BUMPENVMAPLUMINANCE";
    case D3DTOP_DOTPRODUCT3:
		 return L"D3DTOP_DOTPRODUCT3";
    case D3DTOP_MULTIPLYADD:
		 return L"D3DTOP_MULTIPLYADD";
    case D3DTOP_LERP:
		 return L"D3DTOP_LERP";
	default:
		return L"<UNKNOWN>";
	}
}

