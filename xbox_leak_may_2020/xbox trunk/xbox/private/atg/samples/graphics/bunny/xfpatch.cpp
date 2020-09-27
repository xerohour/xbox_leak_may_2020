//-----------------------------------------------------------------------------
// File: xfpatch.cpp
//
// Desc: routines for generating and displaying a 'fuzz' patch.
//       which is a series of layers that give the appearance of
//       hair, fur, grass, or other fuzzy things.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _XBOX
#include <xtl.h>
#include <xgraphics.h>
#else
#include <d3d8.h>
#endif
#include "xfpatch.h"
#include "xbutil.h"
#include "mipmap.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

#define irand(a) ((rand()*(a))>>15)
#define frand(a) ((float)rand()*(a)/32768.0f)

float g_fOneInch = 0.01f;

D3DXMATRIX g_matProj;					// current projection matrix
D3DXMATRIX g_matView;					// current view matrix
D3DXMATRIX g_matWorld;					// current world matrix
D3DXMATRIX g_matDisplay;				// current display matrix (View*World*Proj)
D3DXVECTOR3 g_LightPos;					// current light position
D3DXVECTOR3 g_EyePos;					// current eye position

DWORD g_dwFuzzVS;						// fuzz vertex shader
DWORD g_dwFinVS;						// fin vertex shader

//-----------------------------------------------------------------------------
// Name: Constructor
// Desc: 
//-----------------------------------------------------------------------------
CXFPatch::CXFPatch()
{
	DWORD i;
	ZeroMemory(this, sizeof(CXFPatch));

	// init default patch
	m_fXSize = 0.1f;
	m_fZSize = 0.1f;

	// init default fuzzlib
	m_dwNumFuzzLib = 0;
	m_pFuzzLib = NULL;
	m_fuzzCenter.colorBase = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	m_fuzzCenter.colorTip = D3DXCOLOR(1.f, 1.f, 1.f, 0.f);
	m_fuzzCenter.dp.y = 1.0f;

	// init default fuzz
	m_dwNumSegments = 4;
	m_pFuzz = NULL;

	// init default volume
	m_dwNumLayers = 0;
	m_dwVolXSize = 0;
	m_dwVolZSize = 0;

	for(i=0; i<MAX_VLEVEL; i++)
		m_pVolTexture[i] = NULL;
	m_pTexture = NULL;
	m_pHairLightingTexture = NULL;
	m_pFinTexture = NULL;

	InitFuzz(1, 1);
}

//-----------------------------------------------------------------------------
// Name: Destructor
// Desc: 
//-----------------------------------------------------------------------------
CXFPatch::~CXFPatch()
{
	DWORD i;
	if(m_pFuzzLib)
		delete m_pFuzzLib;
	if(m_pFuzz)
		delete m_pFuzz;
	for(i=0; i<m_dwNumLayers; i++)
		SAFE_RELEASE(m_pVolTexture[i]);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pHairLightingTexture);
}

//-----------------------------------------------------------------------------
// Name: InitFuzz
// Desc: Initializes the individual strands of fuzz in the patch.
//       Only a small number of individual fuzzes are generated
//       (determined by m_dwNumFuzzLib) because each one in the patch
//       does not need to be unique.
//-----------------------------------------------------------------------------
void CXFPatch::InitFuzz(DWORD nfuzz, DWORD nfuzzlib)
{
	DWORD i;
	float y;

	if(nfuzz<=0 || nfuzzlib<0)
		return;
		
	// handle memory allocation
	if(m_dwNumFuzz!=nfuzz)				// if nfuzz has changed
	{
		if(m_pFuzz)
			delete m_pFuzz;				// nuke existing
		m_pFuzz = new SFuzzInst[nfuzz];	// and get new fuzz memory
	}

	if(m_dwNumFuzzLib!=nfuzzlib)
	{
		if(m_pFuzzLib)
			delete m_pFuzzLib;

		m_pFuzzLib = new SFuzz[nfuzzlib];
	}

	m_dwNumFuzz = nfuzz;
	m_dwNumFuzzLib = nfuzzlib;

	// generate the individual fuzzes in the library
	m_fYSize = 0.0f;
	srand(m_dwSeed);
	for(i=0; i<m_dwNumFuzzLib; i++)
	{
		m_pFuzzLib[i].dp.x = (m_fuzzCenter.dp.x + m_fuzzRandom.dp.x*(2*frand(1.0f)-1.0f))*g_fOneInch;
		m_pFuzzLib[i].dp.y = (m_fuzzCenter.dp.y + m_fuzzRandom.dp.y*(2*frand(1.0f)-1.0f))*g_fOneInch;
		m_pFuzzLib[i].dp.z = (m_fuzzCenter.dp.z + m_fuzzRandom.dp.z*(2*frand(1.0f)-1.0f))*g_fOneInch;

		m_pFuzzLib[i].ddp.x = (m_fuzzCenter.ddp.x + m_fuzzRandom.ddp.x*(2*frand(1.0f)-1.0f))*g_fOneInch;
		m_pFuzzLib[i].ddp.y = (m_fuzzCenter.ddp.y + m_fuzzRandom.ddp.y*(2*frand(1.0f)-1.0f))*g_fOneInch;
		m_pFuzzLib[i].ddp.z = (m_fuzzCenter.ddp.z + m_fuzzRandom.ddp.z*(2*frand(1.0f)-1.0f))*g_fOneInch;

		m_pFuzzLib[i].colorBase = m_fuzzCenter.colorBase + (2*frand(1.f)-1.f)*m_fuzzRandom.colorBase;
		m_pFuzzLib[i].colorTip = m_fuzzCenter.colorTip + (2*frand(1.f)-1.f)*m_fuzzRandom.colorTip;

		y = m_pFuzzLib[i].dp.y + 0.5f*m_pFuzzLib[i].ddp.y;
		if(y>m_fYSize)
			m_fYSize = y;
	}

 	// initialize the fuzz locations & pick a random fuzz from the library
	srand(m_dwSeed*54795);
	for(i=0; i<m_dwNumFuzz; i++)
	{
		m_pFuzz[i].x = (frand(1.0f)-0.5f)*m_fXSize;
		m_pFuzz[i].z = (frand(1.0f)-0.5f)*m_fZSize;
		m_pFuzz[i].lidx = irand(m_dwNumFuzzLib);
	}
}

//-----------------------------------------------------------------------------
// Name: GenVolume
// Desc: Generate the fuzz volume. This is done by 'growing' a fuzz 
//       through several layers of texture (determined by nlayers)
//       and 'splatting' it in to the relevant layer with the proper alpha.
//       This is done in two stages at present. The first stage builds
//       a floating point 'volume' of rgb and alpha. The next stage
//       puts the proper values into our texture layers.
//-----------------------------------------------------------------------------
void CXFPatch::GenVolume(DWORD nlayers, DWORD volxsize, DWORD volzsize)
{
#define USE_HW 1
#define DRAW_LOWEST_LOD 1
#if !USE_HW
	float x0, y0, z0;
	float dx, dy, dz;
	float ddx, ddy, ddz;
	float step, cp, scalex, scaley, scalez;
	DWORD b, i, j, lidx, *tex;
	DWORD x, y, z, idx, dwcolor;
	D3DXVECTOR4 *vlayer;
	D3DVECTOR v, color;
	D3DLOCKED_RECT lr;
	float nsplotch;
#ifdef _XBOX
	Swizzler *swiz;
#endif
#endif
	FVFT_XYZRHWTEX1 quad[4*sizeof(FVFT_XYZRHWTEX1)] =
	{
		{0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f},
		{(float)volxsize, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
		{0.0f, (float)volzsize, 1.0f, 1.0f, 0.0f, 1.0f},
		{(float)volxsize, (float)volzsize, 1.0f, 1.0f, 1.0f, 1.0f}
	};

	// make sure volume info is up to date
	if(m_dwVolXSize!=volxsize || m_dwVolZSize!=volzsize)
	{
		for(UINT i=0; i<m_dwNumLayers; i++)
		{
			if(m_pVolTexture[i])
				m_pVolTexture[i]->Release();
			m_pVolTexture[i] = NULL;
		}
		
		if(m_pTexture)
			m_pTexture->Release();
		m_pTexture = NULL;
		m_dwNumLayers = 0;

		if(m_pFinTexture)
			m_pFinTexture->Release();
		m_pFinTexture = NULL;
	}

	m_dwVolXSize = volxsize;
	m_dwVolZSize = volzsize;
	m_dwVolSize = volxsize*volzsize;

	// create textures if necessary
	DWORD finWidth = 256; // 512; // volxsize; /* TODO: add parameters for fin texture size */
	DWORD finHeight = volzsize;
	if(m_dwNumLayers!=nlayers)
	{
		UINT i;

		// create new layers
		for(i=0; i<nlayers; i++)
			if(!m_pVolTexture[i])
				g_pd3dDevice->CreateTexture(volxsize, volzsize, 0, 0, D3DFMT_A8R8G8B8, 0, &m_pVolTexture[i]);

		// release unused layers
		for(i=nlayers; i<m_dwNumLayers; i++)
		{
			if (m_pVolTexture[i])
			{
				m_pVolTexture[i]->Release();
				m_pVolTexture[i] = NULL;
			}
		}

		// create lowest LOD texture
		if (m_pTexture) m_pTexture->Release();
		g_pd3dDevice->CreateTexture(volxsize, volzsize, 0, 0, D3DFMT_A8R8G8B8, 0, &m_pTexture);
		
		// Create fin texture
		g_pd3dDevice->CreateTexture(finWidth, finHeight, 0, 0, D3DFMT_A8R8G8B8, 0, &m_pFinTexture);
	}
	m_dwNumLayers = nlayers;

#if USE_HW
	// Render into volume slices
	{
		// save current back buffer, z buffer, and transforms
		struct {
			IDirect3DSurface8 *pBackBuffer, *pZBuffer;
			D3DMATRIX matWorld, matView, matProjection;
		} save;
		g_pd3dDevice->GetRenderTarget(&save.pBackBuffer);
		g_pd3dDevice->GetDepthStencilSurface(&save.pZBuffer);
		g_pd3dDevice->GetTransform( D3DTS_WORLD, &save.matWorld);
		g_pd3dDevice->GetTransform( D3DTS_VIEW, &save.matView);
		g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &save.matProjection);
		// make a new depth buffer
		IDirect3DSurface8 *pZBuffer = NULL;
		g_pd3dDevice->CreateDepthStencilSurface(volxsize, volzsize, D3DFMT_LIN_D24S8, D3DMULTISAMPLE_NONE, &pZBuffer);
#if 0
		// compute bounds of fuzz patch
		D3DXVECTOR3 va, vb;
		{
			DWORD i, j, lidx;
			float x0, y0, z0;
			float dx, dy, dz;
			float ddx, ddy, ddz;
			float step, cp;
			UINT startseg = 0;
			step = 1.0f/(float)m_dwNumSegments;
			// init bound from first fuzz
			D3DXVECTOR3 v(m_pFuzz[0].x, 0.f, m_pFuzz[0].z);
			va = vb = v;
			for(i=0; i<m_dwNumFuzz; i++)
			{
				// get location and index from fuzz instances
				lidx = m_pFuzz[i].lidx;
				x0 = m_pFuzz[i].x;
				y0 = 0.0f;
				z0 = m_pFuzz[i].z;

				// get params from fuzz lib
				dx = m_pFuzzLib[lidx].dp.x;
				dy = m_pFuzzLib[lidx].dp.y;
				dz = m_pFuzzLib[lidx].dp.z;
				ddx = m_pFuzzLib[lidx].ddp.x;
				ddy = m_pFuzzLib[lidx].ddp.y;
				ddz = m_pFuzzLib[lidx].ddp.z;

				// compute line points
				cp = (float)startseg*step;
				for(j=startseg; j<=m_dwNumSegments; j++)
				{
					v.x = x0 + cp*dx + 0.5f*cp*cp*ddx;
					v.y = y0 + cp*dy + 0.5f*cp*cp*ddy;
					v.z = z0 + cp*dz + 0.5f*cp*cp*ddz;
					D3DXVec3Minimize(&va, &va, &v);
					D3DXVec3Maximize(&vb, &vb, &v);
					cp += step;
				}
			}
		}
#else
		D3DXVECTOR3 va(-0.5f*m_fXSize, 0.f, -0.5f*m_fZSize), vb(0.5f*m_fXSize, m_fYSize, 0.5f*m_fZSize); // bounds of fuzz patch
#endif
		D3DXVECTOR3 vwidth(vb-va), vcenter(0.5f*(vb+va)); // width and center of fuzz patch
		// set world transformation to scale model to unit cube [-1,1] in all dimensions
		D3DXMATRIX matWorld, matTranslate, matScale;
		D3DXMatrixTranslation(&matTranslate, -vcenter.x, -vcenter.y, -vcenter.z);
		D3DXMatrixScaling(&matScale, 2.f/vwidth.x, 2.f/vwidth.y, 2.f/vwidth.z);
		matWorld = matTranslate * matScale;
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld);
		// set view transformation to flip z and look down y axis, with bottom-most slice scaled and translated to map to [0,1]
		D3DMATRIX matView;
		matView.m[0][0] = 1.f;	matView.m[0][1] =  0.f;	matView.m[0][2] = 0.f;				matView.m[0][3] = 0.f;
		matView.m[1][0] = 0.f;	matView.m[1][1] =  0.f;	matView.m[1][2] = 0.5f * nlayers;	matView.m[1][3] = 0.f;
		matView.m[2][0] = 0.f;	matView.m[2][1] = -1.f;	matView.m[2][2] = 0.f;				matView.m[2][3] = 0.f;
		matView.m[3][0] = 0.f;	matView.m[3][1] =  0.f;	matView.m[3][2] = 0.5f * nlayers;	matView.m[3][3] = 1.f;
		// set projection matrix to orthographic
		D3DXMATRIX matProjection;
		D3DXMatrixIdentity(&matProjection);
		g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjection);
		UINT numLines = m_dwNumFuzz*m_dwNumSegments;
		IDirect3DVertexBuffer8 *pVB;
		GetLinesVertexBuffer(&pVB);
		g_pd3dDevice->SetTexture(0, NULL);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE /*FALSE*/);
		g_pd3dDevice->SetVertexShader(FVF_XYZDIFF);
		g_pd3dDevice->SetStreamSource(0, pVB, sizeof(FVFT_XYZDIFF));
		// draw each slice
		for (int i = 0/*-1*/; i < (int)nlayers; i++)
		{
			// get destination surface & set as render target, then draw the fuzz slice
			LPDIRECT3DTEXTURE8 pTexture = m_pVolTexture[i];
			IDirect3DSurface8 *pSurface;
			pTexture->GetSurfaceLevel(0, &pSurface);
			g_pd3dDevice->SetRenderTarget(pSurface, pZBuffer);
			g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0);
			matView.m[1][2] = 0.5f * nlayers;
			matView.m[3][2] = 0.5f * nlayers - (float)i;	// offset to next slice
			// We want the texture to wrap, so draw multiple times with offsets in the plane so that 
			// the boundaries will be filled in by the overlapping geometry.
			for (int iX = -1; iX <= 1; iX++)
			{
				for (int iY = -1; iY <= 1; iY++)
				{
					matView.m[3][0] = 2.f * iX;
					matView.m[3][1] = 2.f * iY;
					g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView);
					g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, numLines);
				}
			}
			pSurface->Release();
			GenerateMipmaps(pTexture, 0);
		}

#if DRAW_LOWEST_LOD
		// draw all the hairs into the lowest LOD texture
		{
			// get destination surface & set as render target, then draw the fuzz slice
			LPDIRECT3DTEXTURE8 pTexture = m_pTexture;
			IDirect3DSurface8 *pSurface;
			pTexture->GetSurfaceLevel(0, &pSurface);
			g_pd3dDevice->SetRenderTarget(pSurface, pZBuffer);
			g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0);
			// scale and translate to cover whole z range [0,1]
			matView.m[1][2] = 0.5f; // 4.f
			matView.m[3][2] = 0.5f; // 4.f;
			// We want the texture to wrap, so draw multiple times with offsets in the plane so that 
			// the boundaries will be filled in by the overlapping geometry.
			for (int iX = -1; iX <= 1; iX++)
			{
				for (int iY = -1; iY <= 1; iY++)
				{
					matView.m[3][0] = 2.f * iX;
					matView.m[3][1] = 2.f * iY;
					g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView);
					g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, numLines);
				}
			}
			pSurface->Release();
			GenerateMipmaps(pTexture, 0);
		}
#endif		

#if 1
		// draw fuzz into the fin texture, looking from the side
		{
			LPDIRECT3DTEXTURE8 pTexture = m_pFinTexture;
			IDirect3DSurface8 *pSurface;
			pTexture->GetSurfaceLevel(0, &pSurface);
			if (finWidth != volxsize
				|| finHeight != volzsize)
			{
				// make a zbuffer that's the right size for the render target.  TODO: do we even need a zbuffer?
				pZBuffer->Release();
				g_pd3dDevice->CreateDepthStencilSurface(finWidth, finHeight, D3DFMT_LIN_D24S8, D3DMULTISAMPLE_NONE, &pZBuffer);
			}
			g_pd3dDevice->SetRenderTarget(pSurface, pZBuffer);
			g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0);
			// set view transformation to look down z axis, with z range [0,1]
			float fXFraction = 0.25f;
			float fYFraction = 1.f;
			float fZFraction = 0.05f;
			float fXScale = 1.f / fXFraction;
			float fYScale = 1.f / fYFraction;
			float fZScale = 1.f / fZFraction;
			matView.m[0][0] = fXScale;	matView.m[0][1] =  0.f;		matView.m[0][2] = 0.f;				matView.m[0][3] = 0.f;
			matView.m[1][0] = 0.f;		matView.m[1][1] =  fYScale;	matView.m[1][2] = 0.f;				matView.m[1][3] = 0.f;
			matView.m[2][0] = 0.f;		matView.m[2][1] =  0.f;		matView.m[2][2] = 0.5f * fZScale;	matView.m[2][3] = 0.f;
			matView.m[3][0] = 0.f;		matView.m[3][1] =  0.f;		matView.m[3][2] = 0.5f * fZScale;	matView.m[3][3] = 1.f;
			g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView);
			g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, numLines);
			pSurface->Release();
			GenerateMipmaps(pTexture, 0, D3DTADDRESS_WRAP, D3DTADDRESS_CLAMP);
		}
#endif		

		pVB->Release();
		pZBuffer->Release();
		// restore back and z buffer and transforms
		g_pd3dDevice->SetRenderTarget(save.pBackBuffer, save.pZBuffer);
		save.pBackBuffer->Release();
		save.pZBuffer->Release();
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &save.matWorld);
		g_pd3dDevice->SetTransform( D3DTS_VIEW, &save.matView);
		g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &save.matProjection);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	}
#else

	// now splat into volume texture
	D3DXVECTOR4 *pVolume = NULL;					// used for generating volume
	pVolume = new D3DXVECTOR4[m_dwNumLayers*m_dwVolSize];
	memset(pVolume, 0, m_dwNumLayers*m_dwVolSize*sizeof(D3DXVECTOR4));

	nsplotch = 200;
	step = 1.0f/nsplotch;
	color = m_vFuzzColor;
	scalex = (float)m_dwVolXSize/m_fXSize;
	scaley = (float)m_dwNumLayers/m_fYSize;
	scalez = (float)m_dwVolZSize/m_fZSize;

	for(i=0; i<m_dwNumFuzz; i++)
	{
		// get location and index from fuzz instances
		lidx = m_pFuzz[i].lidx;

		x0 = m_pFuzz[i].x;		// scale source location
		y0 = 0.0f;
		z0 = m_pFuzz[i].z;		// and offset

		// get params from fuzz lib
		dx = m_pFuzzLib[lidx].dp.x;
		dy = m_pFuzzLib[lidx].dp.y;
		dz = m_pFuzzLib[lidx].dp.z;
		ddx = m_pFuzzLib[lidx].ddp.x;
		ddy = m_pFuzzLib[lidx].ddp.y;
		ddz = m_pFuzzLib[lidx].ddp.z;

		cp = 0.0f;
		for(j=0; j<nsplotch; j++)
		{
			v.x = (x0 + cp*dx + 0.5f*cp*cp*ddx) * scalex + m_dwVolXSize/2.0f;
			v.y = (y0 + cp*dy + 0.5f*cp*cp*ddy) * scaley;
			v.z = (z0 + cp*dz + 0.5f*cp*cp*ddz) * scalez + m_dwVolZSize/2.0f;
			Splat(v, 1.0f-cp, color, pVolume);					// splat into the texture
			cp += step;
		}
	}

#ifdef _XBOX
	swiz = new Swizzler(m_dwVolXSize, m_dwVolZSize, 0);
#endif

	// put the floating point volume into the textures
	for(y=0; y<m_dwNumLayers; y++)
	{
		m_pVolTexture[y]->LockRect(0, &lr, NULL, 0);	// lock this layer
		tex = (DWORD *)lr.pBits;						// get a pointer to its bits

		vlayer = &pVolume[y*m_dwVolSize];				// find ourselves in the fp volume
		idx = 0;

#ifdef _XBOX
		swiz->SetV(0);									// set swizzle v = 0
#endif
		for(z=0; z<m_dwVolZSize; z++)					// fill it
		{
#ifdef _XBOX
			swiz->SetU(0);								// set swizzle u = 0
#endif

			for(x=0; x<m_dwVolXSize; x++)
			{
				// convert to non-pre-multiplied alpha
				float w1 = 255.0f/vlayer[idx].w;

				b = (DWORD)(vlayer[idx].x*w1);
				if(b>255) b = 255;
				dwcolor = b<<16;

				b = (DWORD)(vlayer[idx].y*w1);
				if(b>255) b = 255;
				dwcolor |= b<<8;

				b = (DWORD)(vlayer[idx].z*w1);
				if(b>255) b = 255;
				dwcolor |= b;

				b = (DWORD)(vlayer[idx].w*255.0f);
				if(b>255) b = 255;
				dwcolor |= b<<24;

#ifdef _XBOX
				tex[swiz->Get2D()] = dwcolor;
				swiz->IncU();							// bump swizzle u
#else
				tex[idx] = dwcolor;
#endif
				idx++;
			}

#ifdef _XBOX
			swiz->IncV();								// bump swizzle v
#endif
		}

		m_pVolTexture[y]->UnlockRect(0);

		// have d3dx do all the mip filtering
		GenerateMipmaps(m_pVolTexture[y], 0);
	}
	
	delete pVolume;
#endif


#if !DRAW_LOWEST_LOD

	// now generate lowest lod texture by rendering 
	// the existing layers into it
	IDirect3DSurface8 *surf, *pBackBuffer, *pZBuffer;

	// get current back and z buffers
	g_pd3dDevice->GetRenderTarget(&pBackBuffer);
	g_pd3dDevice->GetDepthStencilSurface(&pZBuffer);

	// get destination surface & set as render target
	m_pTexture->GetSurfaceLevel(0, &surf);
	g_pd3dDevice->SetRenderTarget(surf, NULL);

	// set up for rendering
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0 /*0x00c0a070*/);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetVertexShader(FVF_XYZRHWTEX1);

	//g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);

	// render background color into the texture to clear it out
	g_pd3dDevice->SetTexture(0, NULL);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(FVFT_XYZRHWTEX1));

	// set up for alpha blending
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA);	// just alpha

	for(y=0; y<m_dwNumLayers; y++)
	{
		// render it
		g_pd3dDevice->SetTexture(0, m_pVolTexture[y]);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(FVFT_XYZRHWTEX1));
	}

	// release destination surface
	surf->Release();

	// restore original render states
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderTarget(pBackBuffer, pZBuffer);
	pBackBuffer->Release();
	pZBuffer->Release();

	// have d3dx do all the mip filtering
	GenerateMipmaps(m_pTexture, 0);
#endif
}

//-----------------------------------------------------------------------------
// Name: Splat
// Desc: Splat a fuzz into the texture. This function attempts to take
//       a very small 'sphere' and splat it into our texture volume by
//       specifying an rgb color and alpha. 
//-----------------------------------------------------------------------------
void CXFPatch::Splat(D3DVECTOR p, float falpha, D3DVECTOR fcolor, D3DXVECTOR4 *pVolume)
{
	D3DVECTOR pmin, pmax;
	float dx, dy, dz;
	float yscale;
	DWORD idx;
	int x0, x1, y0, y1, z0, z1;
	int x, y, z, xx, zz;
	float r2, e;
	float cx, cy, cz;
	D3DXVECTOR4 *vlayer;

	float fscale = ((float)m_dwVolXSize+(float)m_dwVolZSize)/1024.0f;
	yscale = 1.0f/m_dwNumLayers;

	// find region of pixels covered by point
	pmin.x = p.x - 2.0f*fscale;
	pmin.y = p.y - 2.0f*fscale;
	pmin.z = p.z - 2.0f*fscale;
	pmax.x = p.x + 2.0f*fscale;
	pmax.y = p.y + 2.0f*fscale;
	pmax.z = p.z + 2.0f*fscale;

	// make sure level in range
	if(pmax.y<0.0f || pmin.y>m_dwNumLayers)
		return;

	// find range of points that this splat touches
	x0 = (int)floor(pmin.x);
	y0 = (int)floor(pmin.y);
	z0 = (int)floor(pmin.z);
	x1 = (int)ceil(pmax.x);
	y1 = (int)ceil(pmax.y);
	z1 = (int)ceil(pmax.z);

	// splat into volume
	for(y=y0; y<y1; y++)
	{
		if(y<0)
			continue;
		if(y>=(int)m_dwNumLayers)
			continue;

		vlayer = &pVolume[y*m_dwVolSize];

		for(z=z0; z<z1; z++)
		{
			for(x=x0; x<=x1; x++)
			{
				dx = (float)x-p.x;
				dy = ((float)y-p.y); // *yscale;
				dz = (float)z-p.z;
				r2 = dx*dx + dy*dy + dz*dz;
				e = r2/(fscale*fscale);
				if(e>15.0)
					continue;

				// Wrap pixel coordinates
				xx = x & (m_dwVolXSize-1);
				zz = z & (m_dwVolZSize-1);

				// calc color contribution
				e = falpha * expf(-e);
				cx = fcolor.x*e;
				cy = fcolor.y*e;
				cz = fcolor.z*e;

				// add into volume
				idx = zz*m_dwVolXSize+xx;
				vlayer[idx].x += cx;
				vlayer[idx].y += cy;
				vlayer[idx].z += cz;
				vlayer[idx].w += e;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Desc: File routines for saving and loading patch files.
//       Saving is only relevant if used on a Windows platform.
//-----------------------------------------------------------------------------
struct _fphdr
{
	char sig[5];
	int flags;
};

void CXFPatch::Save(char *fname, int flags)
{
	FILE *fp;
	struct _fphdr hdr;

	fp = fopen(fname, "wb");

	// write file header
	strcpy(hdr.sig, "FUZ2");
	hdr.flags = flags;
	fwrite(&hdr, sizeof(struct _fphdr), 1, fp);

	// write fpatch data
	fwrite(&m_dwSeed, sizeof(DWORD), 1, fp);
	
	fwrite(&m_fXSize, sizeof(float), 1, fp);
	fwrite(&m_fYSize, sizeof(float), 1, fp);
	fwrite(&m_fZSize, sizeof(float), 1, fp);

	fwrite(&m_dwNumSegments, sizeof(DWORD), 1, fp);
	fwrite(&m_fuzzCenter.colorBase, sizeof(D3DXCOLOR), 1, fp);
	fwrite(&m_fuzzRandom.colorBase, sizeof(D3DXCOLOR), 1, fp);
	fwrite(&m_fuzzCenter.colorTip, sizeof(D3DXCOLOR), 1, fp);
	fwrite(&m_fuzzRandom.colorTip, sizeof(D3DXCOLOR), 1, fp);
	
	fwrite(&m_fuzzCenter.dp, sizeof(D3DVECTOR), 1, fp);
	fwrite(&m_fuzzRandom.dp, sizeof(D3DVECTOR), 1, fp);
	fwrite(&m_fuzzCenter.ddp, sizeof(D3DVECTOR), 1, fp);
	fwrite(&m_fuzzRandom.ddp, sizeof(D3DVECTOR), 1, fp);

	fwrite(&m_dwNumFuzzLib, sizeof(DWORD), 1, fp);
	fwrite(&m_dwNumFuzz, sizeof(DWORD), 1, fp);

	fwrite(&m_dwNumLayers, sizeof(DWORD), 1, fp);
	fwrite(&m_dwVolXSize, sizeof(DWORD), 1, fp);
	fwrite(&m_dwVolZSize, sizeof(DWORD), 1, fp);

	// write volume data if available and flag is set
	{
	}

	fclose(fp);
}

void CXFPatch::Load(char *fname)
{
	FILE *fp;
	struct _fphdr hdr;
	DWORD numfuzzlib, numfuzz;
	DWORD numlayers, volxsize, volzsize;

	fp = fopen(fname, "rb");
	if(!fp)
		return;

	// read file header
	fread(&hdr, sizeof(struct _fphdr), 1, fp);

	// verify signature
	bool bFUZ1 = !strcmp(hdr.sig, "FUZ1");
	bool bFUZ2 = !strcmp(hdr.sig, "FUZ2");
	if (!bFUZ1 && !bFUZ2)
		return;	// signature not understood

	// read patch data
	fread(&m_dwSeed, sizeof(DWORD), 1, fp);
	
	fread(&m_fXSize, sizeof(float), 1, fp);
	fread(&m_fYSize, sizeof(float), 1, fp);
	fread(&m_fZSize, sizeof(float), 1, fp);

	fread(&m_dwNumSegments, sizeof(DWORD), 1, fp);
	if (bFUZ1)
	{
		fread(&m_fuzzCenter.colorBase, sizeof(D3DXCOLOR), 1, fp);
		m_fuzzCenter.colorTip = m_fuzzCenter.colorBase;
		m_fuzzRandom.colorBase = m_fuzzRandom.colorTip = 0ul;
	}
	else
	{
		fread(&m_fuzzCenter.colorBase, sizeof(D3DXCOLOR), 1, fp);
		fread(&m_fuzzRandom.colorBase, sizeof(D3DXCOLOR), 1, fp);
		fread(&m_fuzzCenter.colorTip, sizeof(D3DXCOLOR), 1, fp);
		fread(&m_fuzzRandom.colorTip, sizeof(D3DXCOLOR), 1, fp);
	}

	fread(&m_fuzzCenter.dp, sizeof(D3DVECTOR), 1, fp);	// velocity center
	fread(&m_fuzzRandom.dp, sizeof(D3DVECTOR), 1, fp);	// velocity random
	fread(&m_fuzzCenter.ddp, sizeof(D3DVECTOR), 1, fp);	// acceleration center
	fread(&m_fuzzRandom.ddp, sizeof(D3DVECTOR), 1, fp);	// acceleration random

	fread(&numfuzzlib, sizeof(DWORD), 1, fp);
	fread(&numfuzz, sizeof(DWORD), 1, fp);

	fread(&numlayers, sizeof(DWORD), 1, fp);
	fread(&volxsize, sizeof(DWORD), 1, fp);
	fread(&volzsize, sizeof(DWORD), 1, fp);

	// read volume data if available and flag is set
	{
	}

	fclose(fp);

	InitFuzz(numfuzz, numfuzzlib);
	GenVolume(numlayers, volxsize, volzsize);
}

//////////////////////////////////////////////////////////////////////
// Lighting Model
//////////////////////////////////////////////////////////////////////

inline float
Luminance(const D3DXCOLOR &c)
{
    return (0.229f * c.r) + (0.587f * c.g) + (0.114f * c.b);
}

inline float
MaxChannel(const D3DXCOLOR &c)
{
	if (c.r > c.g)
	{
		if (c.r > c.b)
			return c.r;
		else
			return c.b;
	}
	else
	{
		if (c.g > c.b)
			return c.g;
		else
			return c.b;
	}
}

inline D3DXCOLOR 
Lerp(const D3DXCOLOR &c1, const D3DXCOLOR &c2, float s)
{
    return c1 + s * (c2 - c1);
}

inline D3DXCOLOR
Desaturate(const D3DXCOLOR &rgba)
{
    float alpha = rgba.a;
    if (alpha > 1.f)
        alpha = 1.f;
    float fMaxChan = MaxChannel(rgba);
    if (fMaxChan > alpha) 
    {
        D3DXCOLOR rgbGray(alpha, alpha, alpha, alpha);
        float fYOld = Luminance(rgba);
        if (fYOld >= alpha)
            return rgbGray;
        // scale color to preserve hue
        D3DXCOLOR rgbNew;
        float fInvMaxChan = 1.f / fMaxChan;
        rgbNew.r = rgba.r * fInvMaxChan;
        rgbNew.g = rgba.g * fInvMaxChan;
        rgbNew.b = rgba.b * fInvMaxChan;
		rgbNew.a = alpha;
        float fYNew = Luminance(rgbNew);
        // add gray to preserve luminance
        return Lerp(rgbNew, rgbGray, (fYOld - fYNew) / (alpha - fYNew));
    }
    return rgba;
}

inline D3DXCOLOR &operator *=(D3DXCOLOR &p, const D3DXCOLOR &q)
{
	p.r *= q.r;
	p.g *= q.g;
	p.b *= q.b;
	p.a *= q.a;
	return p;
}

inline D3DXCOLOR operator *(const D3DXCOLOR &p, const D3DXCOLOR &q)
{
	D3DXCOLOR r;
	r.r = p.r * q.r;
	r.g = p.g * q.g;
	r.b = p.b * q.b;
	r.a = p.a * q.a;
	return r;
}

//-----------------------------------------------------------------------------
//  CHairLighting
//-----------------------------------------------------------------------------
struct CHairLighting {
	// like D3DMATERIAL8, except populated with D3DXCOLOR's so that color arithmetic works
	D3DXCOLOR m_colorDiffuse;
	D3DXCOLOR m_colorSpecular;
	float m_fSpecularExponent;
	D3DXCOLOR m_colorAmbient;
	D3DXCOLOR m_colorEmissive;

	HRESULT Initialize(LPDIRECT3DDEVICE8 pDevice, D3DMATERIAL8 *pMaterial);
	HRESULT CalculateColor(D3DXCOLOR *pColor, float LT, float HT);
	HRESULT CalculateClampedColor(D3DXCOLOR *pColor, float LT, float HT);
	
	static HRESULT FillTexture(D3DMATERIAL8 *pMaterial, LPDIRECT3DTEXTURE8 pTexture);
};

HRESULT CHairLighting::Initialize(LPDIRECT3DDEVICE8 pDevice, D3DMATERIAL8 *pMaterial)
{
	// Get current colors modulated by light 0 and global ambient
	// ignore current alphas
	m_colorDiffuse = D3DXCOLOR(pMaterial->Diffuse.r, pMaterial->Diffuse.g, pMaterial->Diffuse.b, 1.f);
	m_colorSpecular = D3DXCOLOR(pMaterial->Specular.r, pMaterial->Specular.g, pMaterial->Specular.b, 1.f);
	m_fSpecularExponent = pMaterial->Power;
	m_colorAmbient = D3DXCOLOR(pMaterial->Ambient.r, pMaterial->Ambient.g, pMaterial->Ambient.b, 1.f);
	m_colorEmissive = D3DXCOLOR(pMaterial->Emissive.r, pMaterial->Emissive.g, pMaterial->Emissive.b, 1.f);
#if 0
	D3DLIGHT8 light;
	(void)pDevice->GetLight(0, &light);		
	D3DXCOLOR colorLightSpecular(light.Specular.r, light.Specular.g, light.Specular.b, 1.f);
	D3DXCOLOR colorLightDiffuse(light.Diffuse.r, light.Diffuse.g, light.Diffuse.b, 1.f);
	D3DXCOLOR colorLightAmbient(light.Ambient.r, light.Ambient.g, light.Ambient.b, 1.f);
	D3DCOLOR dcGlobalAmbient;
	(void)pDevice->GetRenderState(D3DRS_AMBIENT, &dcGlobalAmbient);
	D3DXCOLOR colorGlobalAmbient(dcGlobalAmbient);
	colorGlobalAmbient.a = 1.f;
	m_colorSpecular *= colorLightSpecular;
	m_colorDiffuse *= colorLightDiffuse;
	m_colorAmbient *= colorLightAmbient;
	m_colorEmissive += colorLightAmbient * colorGlobalAmbient;	// why do we add this term?
#endif
	return S_OK;
}

/* Zockler et al's technique worked with streamlines, and used dot(V,T) instead of the simpler dot(H,T) */
#define ZOCKLER 0 
#if ZOCKLER

HRESULT CHairLighting::CalculateColor(D3DXCOLOR *pColor, float LT, float VT)
{
	// Zockler et al 1996
	float fDiffuseExponent = 2.f;	// Banks used 4.8
	float fDiffuse = powf(sqrtf(1.f - LT*LT), fDiffuseExponent);
	float VR = LT*VT - sqrtf(1.f - LT*LT) * sqrtf(1.f - VT*VT);
	float fSpecular = powf(VR, m_fSpecularExponent);
	*pColor = m_colorEmissive + m_colorAmbient + fSpecular * m_colorSpecular + fDiffuse * m_colorDiffuse;
	return S_OK;
}

#else

HRESULT CHairLighting::CalculateColor(D3DXCOLOR *pColor, float LT, float HT)
{
	float fDiffuseExponent = 2.f;	// Banks used 4.8
	float fDiffuse = powf(sqrtf(1.f - LT*LT), fDiffuseExponent);
	float fSpecular = powf(sqrtf(1.f - HT*HT), m_fSpecularExponent);
	*pColor = m_colorEmissive + m_colorAmbient + fSpecular * m_colorSpecular + fDiffuse * m_colorDiffuse;
	return S_OK;
}

#endif

HRESULT CHairLighting::CalculateClampedColor(D3DXCOLOR *pColor, float LT, float HT)
{
	D3DXCOLOR color;
	HRESULT hr = CalculateColor(&color, LT, HT);
	*pColor = Desaturate(color);	// bring back to 0,1 range
	return hr;
}

//////////////////////////////////////////////////////////////////////
// 
// Fill the hair lighting lookup-table texture
// 
//////////////////////////////////////////////////////////////////////
HRESULT CHairLighting::FillTexture(D3DMATERIAL8 *pMaterial, LPDIRECT3DTEXTURE8 pTexture)
{
	/* 
	The hair lighting texture maps U as the dot product of the hair
	tangent T with the light direction L and maps V as the dot product
	of the tangent with the half vector H.  Since the lighting is a
	maximum when the tangent is perpendicular to L (or H), the maximum
	is at zero.  The minimum is at 0.5, 0.5 (or -0.5, -0.5, since
	wrapping is turned on.)  For the mapped T.H, we raise the map
	value to a specular power.
	*/
	HRESULT hr;
	D3DSURFACE_DESC desc;
	pTexture->GetLevelDesc(0, &desc);
	if (desc.Format != D3DFMT_A8R8G8B8)
		return E_NOTIMPL;
	DWORD dwPixelStride = 4;
	D3DLOCKED_RECT lockedRect;
	hr = pTexture->LockRect(0, &lockedRect, NULL, 0L);
	if (FAILED(hr))
		return hr;
	CHairLighting lighting;
	lighting.Initialize(g_pd3dDevice, pMaterial);
	Swizzler s(desc.Width, desc.Height, 0);
	s.SetV(s.SwizzleV(0));
	s.SetU(s.SwizzleU(0));
	for (UINT v = 0; v < desc.Height; v++)
	{
		for (UINT u = 0; u < desc.Width; u++)
		{
			BYTE *p = (BYTE *)lockedRect.pBits + dwPixelStride * s.Get2D();
			// vertical is specular lighting
			// horizontal is diffuse lighting
			D3DXCOLOR color;
#if ZOCKLER
			// Zockler et al 1996
			float LT = 2.f * u / desc.Width - 1.0f;
			float VT = 2.f * v / desc.Height - 1.0f;
			lighting.CalculateClampedColor(&color, LT, VT);
#else
			float LT = 2.f * u / desc.Width;
			if (LT > 1.f) LT -= 2.f;
			float HT = 2.f * v / desc.Height;
			if (HT > 1.f) HT -= 2.f;
			lighting.CalculateClampedColor(&color, LT, HT);
#endif
			int r, g, b;
			r = (BYTE)(255 * color.r);
			g = (BYTE)(255 * color.g);
			b = (BYTE)(255 * color.b);
			*p++ = (BYTE)b;
			*p++ = (BYTE)g;
			*p++ = (BYTE)r;
			*p++ = 255;	// alpha
			s.IncU();
		}
		s.IncV();
	}
	pTexture->UnlockRect(0);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SetHairLightingMaterial
// Desc: Create and fill the hair lighting texture
//-----------------------------------------------------------------------------
HRESULT CXFPatch::SetHairLightingMaterial(D3DMATERIAL8 *pMaterial)
{
	// Create and fill the hair lighting texture
	HRESULT hr;
	m_HairLightingMaterial = *pMaterial;
	if (!m_pHairLightingTexture)
	{
		DWORD dwFlags = 0;
		DWORD dwWidth = 128;
		DWORD dwHeight = 128;
		D3DFORMAT surfaceFormat = D3DFMT_A8R8G8B8;
		DWORD nMipMap = 1;
		hr = D3DXCreateTexture(g_pd3dDevice, dwWidth, dwHeight, nMipMap, 0, surfaceFormat, 0, &m_pHairLightingTexture);
		if (FAILED(hr)) goto e_Exit;
	}
	hr = CHairLighting::FillTexture(pMaterial, m_pHairLightingTexture);
	if (FAILED(hr)) goto e_Exit;
e_Exit:
	if (FAILED(hr))
		SAFE_RELEASE(m_pHairLightingTexture);
	return hr;
}


//-----------------------------------------------------------------------------
// Name: GetLinesVertexBuffer
// Desc: Create and fill in a vertex buffer as a series of individual lines from the fuzz library.
//-----------------------------------------------------------------------------
void CXFPatch::GetLinesVertexBuffer(IDirect3DVertexBuffer8 **ppVB)
{
	IDirect3DVertexBuffer8 *pVB;
	UINT numVertices = m_dwNumFuzz*m_dwNumSegments*2;
	g_pd3dDevice->CreateVertexBuffer(numVertices*sizeof(FVFT_XYZDIFF), 0, FVF_XYZDIFF, 0, &pVB);
	assert(pVB!=NULL);
	DWORD i, j, vidx, lidx;
	DWORD alpha0 = 128;
	FVFT_XYZDIFF *verts;
	float x0, y0, z0;
	float dx, dy, dz;
	float ddx, ddy, ddz;
	float step, cp;
	UINT startseg = 0;
	step = 1.0f/(float)m_dwNumSegments;
	pVB->Lock(0, numVertices*sizeof(FVFT_XYZDIFF), (BYTE **)&verts, 0);
	vidx = 0;
	D3DXCOLOR colorBase, colorDelta;
	for(i=0; i<m_dwNumFuzz; i++)
	{
		// get location and index from fuzz instances
		lidx = m_pFuzz[i].lidx;
		x0 = m_pFuzz[i].x;
		y0 = 0.0f;
		z0 = m_pFuzz[i].z;

		// get params from fuzz lib
		dx = m_pFuzzLib[lidx].dp.x;
		dy = m_pFuzzLib[lidx].dp.y;
		dz = m_pFuzzLib[lidx].dp.z;
		ddx = m_pFuzzLib[lidx].ddp.x;
		ddy = m_pFuzzLib[lidx].ddp.y;
		ddz = m_pFuzzLib[lidx].ddp.z;
		colorBase = m_pFuzzLib[lidx].colorBase;
		colorDelta = m_pFuzzLib[lidx].colorTip - colorBase;

		// build linelist
		cp = (float)startseg*step;

		for(j=startseg; j<m_dwNumSegments; j++)
		{
			verts[vidx].v.x = x0 + cp*dx + 0.5f*cp*cp*ddx;
			verts[vidx].v.y = y0 + cp*dy + 0.5f*cp*cp*ddy;
			verts[vidx].v.z = z0 + cp*dz + 0.5f*cp*cp*ddz;
			verts[vidx].diff = colorBase + cp * cp * colorDelta;
			vidx++;
			cp += step;

			verts[vidx].v.x = x0 + cp*dx + 0.5f*cp*cp*ddx;
			verts[vidx].v.y = y0 + cp*dy + 0.5f*cp*cp*ddy;
			verts[vidx].v.z = z0 + cp*dz + 0.5f*cp*cp*ddz;
			verts[vidx].diff = colorBase + cp * cp * colorDelta;
			vidx++;
		}
	}
	pVB->Unlock();
	*ppVB = pVB;
}

//-----------------------------------------------------------------------------
// Name: RenderLines
// Desc: Draw the fuzz patch as a series of individual lines.
//-----------------------------------------------------------------------------
void CXFPatch::RenderLines()
{
	UINT numLines = m_dwNumFuzz*m_dwNumSegments;
	IDirect3DVertexBuffer8 *pVB;
	GetLinesVertexBuffer(&pVB);
	g_pd3dDevice->SetTexture(0, NULL);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	g_pd3dDevice->SetVertexShader(FVF_XYZDIFF);
	g_pd3dDevice->SetStreamSource(0, pVB, sizeof(FVFT_XYZDIFF));
	g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, numLines);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pVB->Release();
}

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Draw the fuzz patch as layers of texture
//       lod varies from 0 to 1 where 1 = highest LOD (most detail)
//       and 0 = lowest LOD (least detail)
//-----------------------------------------------------------------------------
void CXFPatch::Render(float lod, CXFPatchMesh *pmesh)
{
	DWORD i, lflag;
	// if lod==1.0f render the individual fuzzes
	if(lod==1.0f)
	{
		RenderLines();
		return;
	}

	DWORD iTexStage;
	extern bool g_bMultiShell;

	// Set up volume texture state.  The texture is set per-shell below.
	DWORD iTexStageVolumeTexture = 0;
	iTexStage= 0;
	//g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ALPHAKILL, D3DTALPHAKILL_ENABLE );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_TEXCOORDINDEX, 0);
	extern DWORD g_iCOLOROP1;
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_COLOROP, g_iCOLOROP1);
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING, &lflag);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	static DWORD dwAlphaRef = 0;
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, dwAlphaRef);

	// Set up hair lighting texture state
	iTexStage = 1;
	g_pd3dDevice->SetTexture( iTexStage, m_pHairLightingTexture);
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_TEXCOORDINDEX, 1);
#if ZOCKLER
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ADDRESSU, D3DTADDRESS_MIRROR );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ADDRESSV, D3DTADDRESS_MIRROR );
#else
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
#endif
	extern DWORD g_iCOLOROP2;
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_COLOROP, g_iCOLOROP2);
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_COLORARG2, D3DTA_CURRENT );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

	{
		// Default disables the 2 and 3 stages
		g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}
	
	
	// VERTEX SHADER SETUP
#define VSC_OFFSET 12
#define VSC_EYE 13
#define VSC_LIGHT 14
#define VSC_HALF 15 /* used only for directional lighting */
#define VSC_DIFFUSE 16
#define VSC_AMBIENT 17
#define VSC_SELFSHADOW 18
#define VSC_MULTISHELL 19
#define VSC_WIND1 20
#define VSC_WIND2 21
#define VSC_WIND3 22

	// set display matrix
	D3DXMATRIX matWorldView;
	D3DXMatrixMultiply(&matWorldView, &g_matWorld, &g_matView);
	D3DXMatrixMultiply(&g_matDisplay, &matWorldView, &g_matProj);
	D3DXMatrixTranspose(&g_matDisplay, &g_matDisplay);
	g_pd3dDevice->SetVertexShaderConstant(0, &g_matDisplay, 4);

	// set vertex shader light and eye positions in object coords
	D3DXMATRIX matWorldInverse;
	D3DXMatrixInverse(&matWorldInverse, NULL, &g_matWorld);
	D3DXVECTOR4 vEye;
	D3DXVec3TransformCoord((D3DXVECTOR3 *)&vEye, &g_EyePos, &matWorldInverse);
	vEye.w = 1.f;
	D3DXVECTOR4 vLight;
	D3DXVec3TransformCoord((D3DXVECTOR3 *)&vLight, &g_LightPos, &matWorldInverse);
	vLight.w = 1.f;
	extern bool g_bLocalLighting;	// DEBUG
	if (!g_bLocalLighting)
	{
		// directional lighting
		extern D3DXVECTOR3 g_vLookAt;
		D3DXVECTOR4 vAt;
		D3DXVec3TransformCoord((D3DXVECTOR3 *)&vAt, &g_vLookAt, &matWorldInverse);
		vAt.w = 1.f;
		vEye -= vAt;
		D3DXVec4Normalize(&vEye, &vEye);
		vLight -= vAt;
		D3DXVec4Normalize(&vLight, &vLight);
		D3DXVECTOR4 vHalf = (vEye + vLight) * 0.5f;
		D3DXVec4Normalize(&vHalf, &vHalf);
		g_pd3dDevice->SetVertexShaderConstant(VSC_HALF, &vHalf, 1);
	}
	g_pd3dDevice->SetVertexShaderConstant(VSC_EYE, &vEye, 1);
	g_pd3dDevice->SetVertexShaderConstant(VSC_LIGHT, &vLight, 1);
	static D3DXCOLOR diffuseConditioning(0.8f, 0.8f, 0.8f, 1.f);
	g_pd3dDevice->SetVertexShaderConstant(VSC_DIFFUSE, &diffuseConditioning, 1);
	static D3DXCOLOR ambientConditioning(0.125f, 0.125f, 0.125f, 1.f);
	g_pd3dDevice->SetVertexShaderConstant(VSC_AMBIENT, &ambientConditioning, 1);
	static float fExtra = 1.25f;
	D3DXVECTOR4 vOffset(m_fYSize, fExtra * m_fYSize, 0.5f, 0.f);

	// For the Banks' self-shadowing attenuation factor, inside the vertex shader, 
	// we want (1-p)^d, where d = h / NdotL.   Since we have only expp which gives 
	// the result to the power of two, we use 2^(log2(1-p)*d), 
	// Since expp doesn't accept negative exponents, we negate it here and divide 
	// by the factor inside the shader.
	D3DXVECTOR4 vSelfShadow;
	static float fEps = 0.1f;
	static float fRho = 0.02f;	// attenuation factor, Banks
	static float fDEBUGSCALE = 255.f;
	float fSelfShadowLOG2Attenuation = -logf(1.f-fRho)/logf(2.f);
	float h = fDEBUGSCALE*(vOffset.y - vOffset.x);
	vSelfShadow.x = fEps;	// min value for N dot L
	extern bool g_bSelfShadow;	// DEBUG
	if (!g_bSelfShadow)
		vSelfShadow.y = 0.f;
	else
		vSelfShadow.y = h * fSelfShadowLOG2Attenuation;
	g_pd3dDevice->SetVertexShaderConstant(VSC_SELFSHADOW, &vSelfShadow, 1);

	extern D3DXVECTOR4 g_vWind1; // source position in x,y,z, local magnitude in w
	extern D3DXVECTOR4 g_vWind2; // source up, w = out-of-tangent plane fraction
	extern D3DXVECTOR4 g_vWind3; // source left
	g_pd3dDevice->SetVertexShaderConstant(VSC_WIND1, &g_vWind1, 1);
	g_pd3dDevice->SetVertexShaderConstant(VSC_WIND2, &g_vWind2, 1);
	g_pd3dDevice->SetVertexShaderConstant(VSC_WIND3, &g_vWind3, 1);

	if(lod>0.25f)	// render m_dwNumLayers of textures using alpha blending
	{
		// Draw the fins
		extern bool g_bDrawFins;	// DEBUG
		if (g_bDrawFins && pmesh->m_pFinIB)
		{
			// This is a hack that might be "too clever".   We use the same vertex buffer as the underlying mesh, but 
			// we use it twice: Once for the base points and once for the points extruded along the normals. To
			// make the indexing work, the IB indices are doubled and the stride given to SetStreamSource is halved.
			// Then, using the fin selection stream, we can choose which vertex to use.
			vOffset.x = (float)m_fYSize/(float)m_dwNumLayers;	// use one-layer epsilon off the surface for the fin
			g_pd3dDevice->SetVertexShaderConstant(VSC_OFFSET, &vOffset, 1);
			g_pd3dDevice->SetVertexShader(g_dwFinVS);
			g_pd3dDevice->SetIndices(pmesh->m_pFinIB, 0);
			assert(2 * (sizeof(FVFT_XYZNORMTEX1) / 2) == sizeof(FVFT_XYZNORMTEX1));	// size must be divisible by 2
			g_pd3dDevice->SetStreamSource(0, pmesh->m_pVB, sizeof(FVFT_XYZNORMTEX1) / 2);
			g_pd3dDevice->SetStreamSource(1, pmesh->m_pVB, sizeof(FVFT_XYZNORMTEX1) / 2);
			g_pd3dDevice->SetStreamSource(2, pmesh->m_pFinVB, sizeof(float));	 // selector
			g_pd3dDevice->SetTexture(iTexStageVolumeTexture, m_pFinTexture);
			g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			g_pd3dDevice->SetTextureStageState( iTexStageVolumeTexture, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
			g_pd3dDevice->DrawIndexedPrimitive(D3DPT_QUADLIST, 0, pmesh->m_dwNumVertices * 2, 0, pmesh->m_dwNumFins);
			g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			g_pd3dDevice->SetTextureStageState( iTexStageVolumeTexture, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
		}
		// Draw the shells
		extern bool g_bDrawShells;	// DEBUG
		if (g_bDrawShells)
		{
			if (g_bMultiShell)
			{
				// Set up blending of multiple texture layers at once
				for (iTexStage = 2; iTexStage <= 3; iTexStage++)
				{
					// g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ALPHAKILL, D3DTALPHAKILL_ENABLE );
					g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_TEXCOORDINDEX, iTexStage );
					g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
					g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
					g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
					g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
					g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
				}

				// Overwrite texture function with pixel shader
#include "fur.inl"
				g_pd3dDevice->SetPixelShaderProgram(&psd);
			}

			g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			g_pd3dDevice->SetVertexShader(g_dwFuzzVS);
			if (pmesh->m_pIB) g_pd3dDevice->SetIndices(pmesh->m_pIB, 0);
			g_pd3dDevice->SetStreamSource(0, pmesh->m_pVB, sizeof(FVFT_XYZNORMTEX1));
			float fYSizeDelta = m_fYSize/(float)m_dwNumLayers;
			if (g_bMultiShell)
			{
				D3DXVECTOR4 vShellOffsetDeltas(fYSizeDelta, 0.f, -1.f*fYSizeDelta, -2.f*fYSizeDelta);
				g_pd3dDevice->SetVertexShaderConstant(VSC_MULTISHELL, &vShellOffsetDeltas, 1);
				g_pd3dDevice->SetStreamSource(1, pmesh->m_pVBTex, sizeof(D3DVECTOR) * 2);
			}
/*			
			UINT iStart = 0;
			UINT iEnd = m_dwNumLayers - 1;
bool bRange = true;
UINT iStart0 = 5;
UINT iStart1 = 5;
if (bRange)
{
	iStart = iStart0;
	iEnd = iEnd0;
}
*/
static bool bFlatten = true;
			for(i=0; i<m_dwNumLayers; i++)
			{
				UINT ShellCount;
				if (g_bMultiShell)
				{
					// skip two extra shells each time, for a total of 3 each pass
					UINT iStart = i;
					i += 2;
					if (i >= m_dwNumLayers)
						i = m_dwNumLayers - 1;
					ShellCount = i - iStart + 1;
					vOffset.x = (float)(i)*fYSizeDelta;
				}
				else
					vOffset.x = (float)(i+1)*fYSizeDelta;
				g_pd3dDevice->SetVertexShaderConstant(VSC_OFFSET, &vOffset, 1);
				if (g_bSelfShadow)
				{
					float h = fDEBUGSCALE*(vOffset.y - vOffset.x);
					vSelfShadow.y = h * fSelfShadowLOG2Attenuation;
					g_pd3dDevice->SetVertexShaderConstant(VSC_SELFSHADOW, &vSelfShadow, 1);
				}
				g_pd3dDevice->SetTexture(iTexStageVolumeTexture, m_pVolTexture[bFlatten ? (i / 3)*3 : i]);
				if (g_bMultiShell)
				{
					// Set textures to do multiple shells at once
					if (ShellCount > 1)
						g_pd3dDevice->SetTexture(2, m_pVolTexture[bFlatten ? ((i - 1) / 3) * 3 : (i-1)]);
					else
						g_pd3dDevice->SetTexture(2, NULL);
					if (ShellCount > 2)
						g_pd3dDevice->SetTexture(3, m_pVolTexture[bFlatten ? ((i - 2) / 3) * 3 : (i-2)]);
					else
						g_pd3dDevice->SetTexture(3, NULL);
				}
				if (!pmesh->m_pIB)
					g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, pmesh->m_dwNumIndices/3);
				else
					g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, pmesh->m_dwNumVertices, 0, pmesh->m_dwNumIndices/3);
			}

			g_pd3dDevice->SetPixelShader(NULL);		// use legacy pixel shader
		}
	}
	else // lowest LOD render
	{
		g_pd3dDevice->SetVertexShader(g_dwFuzzVS);
		g_pd3dDevice->SetStreamSource(0, pmesh->m_pVB, sizeof(FVFT_XYZNORMTEX1));
		if (pmesh->m_pIB) g_pd3dDevice->SetIndices(pmesh->m_pIB, 0);
		g_pd3dDevice->SetTexture(iTexStageVolumeTexture, m_pTexture);
		if (!pmesh->m_pIB)
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, pmesh->m_dwNumIndices/3);
		else
			g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, pmesh->m_dwNumVertices, 0, pmesh->m_dwNumIndices/3);
	}

	// Cleanup render and texture stage states.  These may be redundant if multiple calls to patch render are used
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, lflag);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	for (iTexStage = 0; iTexStage < 4; iTexStage++)
	{
		g_pd3dDevice->SetTexture( iTexStage, NULL);
		g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_MIPFILTER, D3DTEXF_NONE);
		g_pd3dDevice->SetTextureStageState( iTexStage, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	}
}


