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

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

#define irand(a) ((rand()*(a))>>15)
#define frand(a) ((float)rand()*(a)/32768.0f)

float g_fOneInch = 0.01f;

D3DXVECTOR4 *g_pVolume;					// used for generating volume
D3DXMATRIX g_matProj;					// current projection matrix
D3DXMATRIX g_matView;					// current view matrix
D3DXMATRIX g_matWorld;					// current world matrix
D3DXMATRIX g_matDisplay;				// current display matrix (View*World*Proj)
D3DVECTOR g_LightPos;					// current light position

DWORD g_dwFuzzVS;						// fuzz vertex shader

//-----------------------------------------------------------------------------
// Name: Constructor
// Desc: 
//-----------------------------------------------------------------------------
CXFPatch::CXFPatch()
{
	DWORD i;

	// init default patch
	m_fXSize = 0.1f;
	m_fZSize = 0.1f;

	// init default fuzzlib
	m_dwNumFuzzLib = 0;
	m_pFuzzLib = NULL;
	memset(&m_vFVelC, 0, 4*sizeof(D3DVECTOR));
	m_vFVelC.y = 1.0f;

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
		if(m_pVolTexture[i])
			m_pVolTexture[i]->Release();

	if(m_pTexture)
		m_pTexture->Release();	
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
		m_pFuzz = new FuzzInst[nfuzz];	// and get new fuzz memory
	}

	if(m_dwNumFuzzLib!=nfuzzlib)
	{
		if(m_pFuzzLib)
			delete m_pFuzzLib;

		m_pFuzzLib = new Fuzz[nfuzzlib];
	}

	m_dwNumFuzz = nfuzz;
	m_dwNumFuzzLib = nfuzzlib;

	// generate the individual fuzzes in the library
	m_fYSize = 0.0f;
	srand(m_dwSeed);
	for(i=0; i<m_dwNumFuzzLib; i++)
	{
		m_pFuzzLib[i].dp.x = (m_vFVelC.x + m_vFVelR.x*(2*frand(1.0f)-1.0f))*g_fOneInch;
		m_pFuzzLib[i].dp.y = (m_vFVelC.y + m_vFVelR.y*(2*frand(1.0f)-1.0f))*g_fOneInch;
		m_pFuzzLib[i].dp.z = (m_vFVelC.z + m_vFVelR.z*(2*frand(1.0f)-1.0f))*g_fOneInch;

		m_pFuzzLib[i].ddp.x = (m_vFAccC.x + m_vFAccR.x*(2*frand(1.0f)-1.0f))*g_fOneInch;
		m_pFuzzLib[i].ddp.y = (m_vFAccC.y + m_vFAccR.y*(2*frand(1.0f)-1.0f))*g_fOneInch;
		m_pFuzzLib[i].ddp.z = (m_vFAccC.z + m_vFAccR.z*(2*frand(1.0f)-1.0f))*g_fOneInch;

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
		for(i=0; i<m_dwNumLayers; i++)
		{
			if(m_pVolTexture[i])
				m_pVolTexture[i]->Release();
			m_pVolTexture[i] = NULL;
		}
		
		if(m_pTexture)
			m_pTexture->Release();
		m_pTexture = NULL;
		m_dwNumLayers = 0;
	}

	m_dwVolXSize = volxsize;
	m_dwVolZSize = volzsize;
	m_dwVolSize = volxsize*volzsize;

	// create textures if necessary
	if(m_dwNumLayers!=nlayers)
	{
		for(i=0; i<nlayers; i++)
			if(!m_pVolTexture[i])
				g_pd3dDevice->CreateTexture(volxsize, volzsize, 0, 0, D3DFMT_A8R8G8B8, 0, &m_pVolTexture[i]);

		for(i=nlayers; i<m_dwNumLayers; i++)
		{
			m_pVolTexture[i]->Release();
			m_pVolTexture[i] = NULL;
		}

		// create lowest LOD texture
		g_pd3dDevice->CreateTexture(volxsize, volzsize, 0, 0, D3DFMT_A8R8G8B8, 0, &m_pTexture);
	}

	m_dwNumLayers = nlayers;

	// now splat into volume texture
	g_pVolume = new D3DXVECTOR4[m_dwNumLayers*m_dwVolSize];
	memset(g_pVolume, 0, m_dwNumLayers*m_dwVolSize*sizeof(D3DXVECTOR4));

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
			Splat(v, 1.0f-cp, color);					// splat into the texture
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

		vlayer = &g_pVolume[y*m_dwVolSize];				// find ourselves in the fp volume
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
		D3DXFilterTexture(m_pVolTexture[y], NULL, 0, D3DX_FILTER_BOX);
	}

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
	g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0x00c0a070);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetVertexShader(FVF_XYZRHWTEX1);

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
	D3DXFilterTexture(m_pTexture, NULL, 0, D3DX_FILTER_BOX);

	delete g_pVolume;
}

//-----------------------------------------------------------------------------
// Name: Splat
// Desc: Splat a fuzz into the texture. This function attempts to take
//       a very small 'sphere' and splat it into our texture volume by
//       specifying an rgb color and alpha. 
//-----------------------------------------------------------------------------
void CXFPatch::Splat(D3DVECTOR p, float falpha, D3DVECTOR fcolor)
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

		vlayer = &g_pVolume[y*m_dwVolSize];

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
	strcpy(hdr.sig, "FUZ1");
	hdr.flags = flags;
	fwrite(&hdr, sizeof(struct _fphdr), 1, fp);

	// write fpatch data
	fwrite(&m_dwSeed, sizeof(DWORD), 1, fp);
	
	fwrite(&m_fXSize, sizeof(float), 1, fp);
	fwrite(&m_fYSize, sizeof(float), 1, fp);
	fwrite(&m_fZSize, sizeof(float), 1, fp);

	fwrite(&m_dwNumSegments, sizeof(DWORD), 1, fp);
	fwrite(&m_vFuzzColor, sizeof(D3DVECTOR), 1, fp);

	fwrite(&m_vFVelC, sizeof(D3DVECTOR), 1, fp);
	fwrite(&m_vFVelR, sizeof(D3DVECTOR), 1, fp);
	fwrite(&m_vFAccC, sizeof(D3DVECTOR), 1, fp);
	fwrite(&m_vFAccR, sizeof(D3DVECTOR), 1, fp);

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
	if(strcmp(hdr.sig, "FUZ1"))
		return;

	// read patch data
	fread(&m_dwSeed, sizeof(DWORD), 1, fp);
	
	fread(&m_fXSize, sizeof(float), 1, fp);
	fread(&m_fYSize, sizeof(float), 1, fp);
	fread(&m_fZSize, sizeof(float), 1, fp);

	fread(&m_dwNumSegments, sizeof(DWORD), 1, fp);
	fread(&m_vFuzzColor, sizeof(D3DVECTOR), 1, fp);

	fread(&m_vFVelC, sizeof(D3DVECTOR), 1, fp);
	fread(&m_vFVelR, sizeof(D3DVECTOR), 1, fp);
	fread(&m_vFAccC, sizeof(D3DVECTOR), 1, fp);
	fread(&m_vFAccR, sizeof(D3DVECTOR), 1, fp);

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

//-----------------------------------------------------------------------------
// Name: RenderLines
// Desc: Draw the fuzz patch as a series of individual lines.
//-----------------------------------------------------------------------------
void CXFPatch::RenderLines()
{
	DWORD i, j, vidx, alpha, dalpha, lidx, color;
	FVFT_XYZDIFF *verts;
	float x0, y0, z0;
	float dx, dy, dz;
	float ddx, ddy, ddz;
	float step, cp;
	UINT startseg = 0;

	verts = new FVFT_XYZDIFF[m_dwNumFuzz*m_dwNumSegments*2];
	assert(verts!=NULL);

	g_pd3dDevice->SetTexture(0, NULL);
	g_pd3dDevice->SetVertexShader(FVF_XYZDIFF);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	color = (DWORD)(m_vFuzzColor.x*255.0f)&0xff;
	color <<= 8;
	color |= (DWORD)(m_vFuzzColor.y*255.0f)&0xff;
	color <<= 8;
	color |= (DWORD)(m_vFuzzColor.z*255.0f)&0xff;

	step = 1.0f/(float)m_dwNumSegments;
	dalpha = 255/m_dwNumSegments;

	vidx = 0;
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

		// build linelist
		alpha = 255-startseg*dalpha;
		cp = (float)startseg*step;

		for(j=startseg; j<m_dwNumSegments; j++)
		{
			verts[vidx].v.x = x0 + cp*dx + 0.5f*cp*cp*ddx;
			verts[vidx].v.y = y0 + cp*dy + 0.5f*cp*cp*ddy;
			verts[vidx].v.z = z0 + cp*dz + 0.5f*cp*cp*ddz;
			verts[vidx].diff = (alpha<<24) | color;
			vidx++;

			alpha -= dalpha;
			cp += step;

			verts[vidx].v.x = x0 + cp*dx + 0.5f*cp*cp*ddx;
			verts[vidx].v.y = y0 + cp*dy + 0.5f*cp*cp*ddy;
			verts[vidx].v.z = z0 + cp*dz + 0.5f*cp*cp*ddz;
			verts[vidx].diff = (alpha<<24) | color;
			vidx++;
		}
	}
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, vidx/2, verts, sizeof(FVFT_XYZDIFF));
	
	delete verts;
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Draw the fuzz patch as layers of texture
//       lod varies from 0 to 1 where 1 = highest LOD (most detail)
//       and 0 = lowest LOD (least detail)
//-----------------------------------------------------------------------------
void CXFPatch::Render(float lod, CXFPatchMesh *pmesh)
{
	D3DXVECTOR4 f;
	D3DXVECTOR3 v;
	DWORD i, lflag;
	D3DXMATRIX tmp;

	// if lod==1.0f render the individual fuzzes
	if(lod==1.0f)
	{
		RenderLines();
		return;
	}

	// color and alpha both come from our volume texture
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);


	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING, &lflag);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	// VERTEX SHADER SETUP

	// set display matrix
	D3DXMatrixMultiply(&tmp, (D3DXMATRIX *)&g_matWorld, &g_matView);
	D3DXMatrixMultiply(&g_matDisplay, &tmp, &g_matProj);
	D3DXMatrixTranspose(&g_matDisplay, &g_matDisplay);
	g_pd3dDevice->SetVertexShaderConstant(0, &g_matDisplay, 4);

	// world matrix	
	D3DXMatrixTranspose(&tmp, (D3DXMATRIX *)&g_matWorld);
	g_pd3dDevice->SetVertexShaderConstant(4, &tmp, 4);		// world matrix

	// normalize light vector & set VS constant
	D3DXVec3Normalize(&v, (D3DXVECTOR3 *)&g_LightPos);
	g_pd3dDevice->SetVertexShaderConstant(13, &v, 1);

	if(lod>0.25f)	// render m_dwNumLayers of textures using alpha blending
	{
		g_pd3dDevice->SetVertexShader(g_dwFuzzVS);
		g_pd3dDevice->SetStreamSource(0, pmesh->m_pVB, sizeof(FVFT_XYZNORMTEX1));

		for(i=0; i<m_dwNumLayers; i++)
		{
			f.x = (float)i*m_fYSize/(float)(m_dwNumLayers-1);
			f.w = 0.0f;
			g_pd3dDevice->SetVertexShaderConstant(12, &f, 1);
					
			g_pd3dDevice->SetTexture(0, m_pVolTexture[i]);
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, pmesh->m_dwNumIndices/3);
		}
	}
	else			// lowest LOD render
	{
		g_pd3dDevice->SetVertexShader(pmesh->m_dwVShader);
		g_pd3dDevice->SetStreamSource(0, pmesh->m_pVB, sizeof(FVFT_XYZNORMTEX1));

		f.x = (float)m_fYSize/(float)(m_dwNumLayers-1);
		f.w = 0.0f;
		g_pd3dDevice->SetVertexShaderConstant(12, &f, 1);

		g_pd3dDevice->SetTexture(0, m_pTexture);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, pmesh->m_dwNumIndices/3);
	}

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, lflag);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
}




























