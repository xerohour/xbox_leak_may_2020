//-----------------------------------------------------------------------------
// File: xmodel.cpp
//
// Desc: routines for loading, saving, and building models
//       vertex buffers, etc...
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>

#include "xmodel.h"
#include "xparse.h"
#include "xpath.h"
#include "xfvf.h"

#ifndef _XBOX
#include <d3dx8.h>
#endif

float g_fNormLength = 0.005f;
extern LPDIRECT3DDEVICE8 g_pd3dDevice;

CXBPackedResource *g_pModelResource = NULL;


//-----------------------------------------------------------------------------
// Constructor and destructor
//-----------------------------------------------------------------------------
CXModel::CXModel()
{
	m_pVB = NULL;
	m_pIB = NULL;
	m_pTexture = NULL;
	m_dwRefCount = 1;
}

CXModel::~CXModel()
{
	assert(m_dwRefCount==0);

	if(m_pVB)
		m_pVB->Release();
	if(m_pIB)
		m_pIB->Release();
	if(m_pTexture)
		m_pTexture->Release();
}

//-----------------------------------------------------------------------------
// Name: Release
// Desc: Release someones claim to this model
//-----------------------------------------------------------------------------
int CXModel::Release()
{
	if(--m_dwRefCount)
		return m_dwRefCount;

	delete this;
	return 0;
}

//-----------------------------------------------------------------------------
// Name: Build
// Desc: builds a model from vertices, a triangle list, a list
//		 of texture coordinates and an FVF. Vertices and the 
//		 trilist are required. UV's & colors are optional
//-----------------------------------------------------------------------------
DWORD CXModel::Build(DWORD nvert, D3DVECTOR *vert, DWORD ntri, DWORD nidx, WORD *trilist, float *tc, D3DVECTOR *norm, DWORD *diff, DWORD fvf, DWORD flags)
{
	byte *bptr;
	DWORD i;
	D3DVECTOR *norm2 = NULL;
	float r, maxr;

	m_dwNumVertices = nvert;
	m_dwNumTriangles = ntri;
	m_dwFVF = fvf;
	m_dwVShader = fvf;				// set vshader to fixed function (user can chang manually)
	m_dwFlags = flags;

	// calculate radius
	maxr = 0.0f;
	for(i=0; i<nvert; i++)
	{	
		r = vert[i].x*vert[i].x + vert[i].y*vert[i].y + vert[i].z*vert[i].z;
		if(r>maxr)
			maxr = r;
	}
	m_fRadius2 = maxr;
	m_fRadius = (float)sqrt(maxr);

	// crunch normals if desired
	if(m_dwFVF&D3DFVF_NORMAL && !norm)
		norm2 = GenNormals(nvert, vert, ntri, trilist);

	// allocate vertex buffer
	m_dwVBStride = FVF_VSize(m_dwFVF);
	m_dwVBSize = m_dwNumVertices*m_dwVBStride;
#ifdef _XBOX
	g_pd3dDevice->CreateVertexBuffer(m_dwVBSize, 0, m_dwFVF, D3DPOOL_DEFAULT, &m_pVB);
#else
	g_pd3dDevice->CreateVertexBuffer(m_dwVBSize, D3DUSAGE_DYNAMIC, m_dwFVF, D3DPOOL_DEFAULT, &m_pVB);
#endif

	m_dwNumIndices = nidx;

	// allocate index buffer
	m_dwIBSize = m_dwNumIndices*sizeof(WORD);
#ifdef _XBOX
	g_pd3dDevice->CreateIndexBuffer(m_dwIBSize, 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB);
#else
	g_pd3dDevice->CreateIndexBuffer(m_dwIBSize, D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB);
#endif
	// fill vertex buffer
	LockVB(&bptr, 0L);
	FVF_SetVert(bptr, vert, 0, m_dwNumVertices, m_dwFVF);
	if(m_dwFVF&D3DFVF_DIFFUSE)
		FVF_SetDiff(bptr, diff, 0, m_dwNumVertices, m_dwFVF);
	if(m_dwFVF&D3DFVF_NORMAL)
	{
		if(norm2)
		{
			FVF_SetNorm(bptr, norm2, 0, m_dwNumVertices, m_dwFVF);
			delete norm2;		// free up memory allocated from GenNormals
		}
		else		// user specified own normals
			FVF_SetNorm(bptr, norm, 0, m_dwNumVertices, m_dwFVF);
	}
	if(m_dwFVF&D3DFVF_TEX1)
		FVF_SetUV(bptr, tc, 0, m_dwNumVertices, m_dwFVF);
	UnlockVB();
	
	// fill index buffer
	LockIB(&bptr, 0L);
	memcpy(bptr, trilist, m_dwIBSize);
	UnlockIB();

	m_PrimType = D3DPT_TRIANGLELIST;

	return 1;
}

//-----------------------------------------------------------------------------
// Name: GenNormals
// Desc: Generate vertex normals for a model and returns a pointer to
//       the memory allocated to store them.
//       *Note that this routine allocates memory that must be freed
//       manually.
//-----------------------------------------------------------------------------
D3DVECTOR *CXModel::GenNormals(DWORD nvert, D3DVECTOR *vert, DWORD ntri, WORD *trilist)
{
	DWORD i, v0, v1, v2;
	D3DVECTOR *tnorm, *vnorm;
	D3DVECTOR d, e;

	tnorm = new D3DVECTOR[ntri];
	memset(tnorm, 0, ntri*sizeof(D3DVECTOR));
	vnorm = new D3DVECTOR[nvert];
	memset(vnorm, 0, nvert*sizeof(D3DVECTOR));

	// calculate triangle normals
	for(i=0; i<ntri; i++)
	{
		v0 = trilist[i*3+0];
		v1 = trilist[i*3+1];
		v2 = trilist[i*3+2];

		// cross product
		d.x = vert[v1].x - vert[v0].x;
		d.y = vert[v1].y - vert[v0].y;
		d.z = vert[v1].z - vert[v0].z;
		e.x = vert[v2].x - vert[v1].x;
		e.y = vert[v2].y - vert[v1].y;
		e.z = vert[v2].z - vert[v1].z;

		tnorm[i].x = d.y*e.z - d.z*e.y;
		tnorm[i].y = d.z*e.x - d.x*e.z;
		tnorm[i].z = d.x*e.y - d.y*e.x;

		// normalize result
		float ool = 1.0f/(float)sqrt(tnorm[i].x*tnorm[i].x + tnorm[i].y*tnorm[i].y + tnorm[i].z*tnorm[i].z);
		tnorm[i].x *= ool;
		tnorm[i].y *= ool;
		tnorm[i].z *= ool;

		// contribute to vertex normals
		vnorm[v0].x += tnorm[i].x;
		vnorm[v0].y += tnorm[i].y;
		vnorm[v0].z += tnorm[i].z;
		vnorm[v1].x += tnorm[i].x;
		vnorm[v1].y += tnorm[i].y;
		vnorm[v1].z += tnorm[i].z;
		vnorm[v2].x += tnorm[i].x;
		vnorm[v2].y += tnorm[i].y;
		vnorm[v2].z += tnorm[i].z;
	}

	// normalize vertex normals
	for(i=0; i<nvert; i++)
		D3DXVec3Normalize((D3DXVECTOR3 *)&vnorm[i], (D3DXVECTOR3 *)&vnorm[i]);

	delete tnorm;
	return vnorm;
}

//-----------------------------------------------------------------------------
// Name: DrawNormals
// Desc: Draw a models normals. Note that g_fNormLength is defined at the
//       top of this file and should be set appropriately based on your world
//       dimensions.
//-----------------------------------------------------------------------------
void CXModel::DrawNormals()
{
	FVFT_XYZDIFF *verts;
	D3DVECTOR *vbuf, *nbuf;
	BYTE *bptr;
	DWORD i;

	verts = new FVFT_XYZDIFF[m_dwNumVertices*2];
	vbuf = new D3DVECTOR[m_dwNumVertices];
	nbuf = new D3DVECTOR[m_dwNumVertices];

	LockVB(&bptr, 0L);
	FVF_GetVert(bptr, vbuf, 0, m_dwNumVertices, m_dwFVF);
	FVF_GetNorm(bptr, nbuf, 0, m_dwNumVertices, m_dwFVF);
	UnlockVB();

	// draw normals
	g_pd3dDevice->SetTexture(0, NULL);
	g_pd3dDevice->SetVertexShader(FVF_XYZDIFF);

    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	for(i=0; i<m_dwNumVertices; i++)
	{
		verts[i*2].v.x = vbuf[i].x;
		verts[i*2].v.y = vbuf[i].y;
		verts[i*2].v.z = vbuf[i].z;
		verts[i*2].diff = 0xffff0000;

		verts[i*2+1].v.x = g_fNormLength*nbuf[i].x + verts[i*2].v.x;
		verts[i*2+1].v.y = g_fNormLength*nbuf[i].y + verts[i*2].v.y;
		verts[i*2+1].v.z = g_fNormLength*nbuf[i].z + verts[i*2].v.z;
		verts[i*2+1].diff = 0xffff0000;
	}

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, m_dwNumVertices, verts, sizeof(FVFT_XYZDIFF));

	// draw unit vector
	verts[0].v.x = 0.0f;
	verts[0].v.y = 0.0f;
	verts[0].v.z = 0.0f;
	verts[0].diff = 0xffffffff;
	verts[1].v.x = 0.0f;
	verts[1].v.y = 0.0f;
	verts[1].v.z = g_fNormLength;
	verts[1].diff = 0xffffffff;
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, verts, sizeof(FVFT_XYZDIFF));

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	delete verts;
	delete vbuf;
	delete nbuf;
}

//-----------------------------------------------------------------------------
// Name: Read_M
// Desc: Read a .m model file.
//-----------------------------------------------------------------------------
#define MAX_MVERTS 32767
#define MAX_MTRIS 65000
DWORD CXModel::Read_M(char *fname, char *imgname, DWORD fvf, DWORD flags, void (*vfunc)(int, int, float))
{
	FILE *fp;
	D3DVECTOR *vert = NULL;
	float *uvv = NULL, f;
	WORD *trilist = NULL;
	DWORD *diff = NULL, d;
	D3DVECTOR *norm = NULL;
	bool bFoundNorm = false;

	char buf[512];
	DWORD nt, vidx, i, nvert = 0;
	DWORD tidx = 0, v0, v1, v2;

	char texname[80];
	
	// load texture
	if(imgname)
		strcpy(texname, imgname);
	else
		texname[0] = '\0';

	// load mesh
	fp = fopen(fname, "rt");
	assert(fp!=NULL);

	vert = new D3DVECTOR[MAX_MVERTS];
	memset(vert, 0, MAX_MVERTS*sizeof(D3DVECTOR));
	trilist = new WORD[MAX_MTRIS*3];
	memset(trilist, 0, MAX_MTRIS*3*sizeof(WORD));
	if(fvf&D3DFVF_NORMAL)
	{
		norm = new D3DVECTOR[MAX_MVERTS];
		memset(norm, 0, MAX_MVERTS*sizeof(D3DVECTOR));
	}
	if(fvf&D3DFVF_TEX1)
	{
		uvv = new float[MAX_MVERTS*2];
		memset(uvv, 0, MAX_MVERTS*2*sizeof(float));
	}
	if(fvf&D3DFVF_DIFFUSE)
	{
		diff = new DWORD[MAX_MVERTS];
		memset(diff, 0, MAX_MVERTS*sizeof(DWORD));
	}

	while(1)
	{
		fgets(buf, 512, fp);
		if(feof(fp))
			break;

		nt = Parse(buf, NULL);
		if(!nt)
			continue;

		if(!strcmp(tokens[0].str, "IMAGE"))
		{
			LoadTexture(tokens[1].str);
		}
		else if(!strcmp(tokens[0].str, "VERTEX"))
		{
			vidx = atoi(tokens[1].str);

			vert[vidx].x = (float)atof(tokens[2].str);
			vert[vidx].y = (float)atof(tokens[3].str);
			vert[vidx].z = (float)atof(tokens[4].str);

			for(i=5; i<nt; i++)
			{
				if(fvf&D3DFVF_NORMAL && (!strcmp(tokens[i].str, "{NORMAL=") || !strcmp(tokens[i].str, "NORMAL=")))
				{
					bFoundNorm = true;
					norm[vidx].x = (float)atof(tokens[i+1].str);
					norm[vidx].y = (float)atof(tokens[i+2].str);
					norm[vidx].z = (float)atof(tokens[i+3].str);
					i+=3;
				}
				else if(fvf&D3DFVF_TEX1 && (!strcmp(tokens[i].str, "{UV=") || !strcmp(tokens[i].str, "UV=")))
				{
					uvv[vidx*2] = (float)atof(tokens[i+1].str);
					uvv[vidx*2+1] = (float)atof(tokens[i+2].str);
					i+=2;
				}
				else if(!strncmp(tokens[i].str, "EYELIDWT", 8) && vfunc)			// check for eyelid weights & receiving function
				{
					f = (float)atof(tokens[i+1].str);
					if(tokens[i].str[8]=='L')
						vfunc(MFP_EYEWT, 0x10000|vidx, -f);
					else
						vfunc(MFP_EYEWT, vidx, f);
					i+=1;
				}
			}

			if(vidx+1>nvert)
				nvert = vidx+1;
		}
		else if(!strcmp(tokens[0].str, "FACE"))
		{
			v0 = atoi(tokens[2].str);
			v1 = atoi(tokens[3].str);
			v2 = atoi(tokens[4].str);

			if(fvf&D3DFVF_DIFFUSE)
				for(i=5; i<nt; i++)
				{
					if(!strcmp(tokens[i].str, "RGB="))
					{
						d = (DWORD)((float)atof(tokens[i+1].str)*255.0f);
						d<<=8;
						d |= (DWORD)((float)atof(tokens[i+2].str)*255.0f);
						d<<=8;
						d |= (DWORD)((float)atof(tokens[i+3].str)*255.0f);

						diff[v0] = d;
						diff[v1] = d;
						diff[v2] = d;

						i+=3;
					}
				}

			trilist[tidx] = (WORD)v0;
			tidx++;
			trilist[tidx] = (WORD)v1;
			tidx++;
			trilist[tidx] = (WORD)v2;
			tidx++;
		}
	}

	fclose(fp);

	Build(nvert, vert, tidx/3, tidx, trilist, uvv, bFoundNorm ? norm : NULL, diff, fvf, flags);
	
	if(texname[0])
		LoadTexture(texname);

	if(vert)
		delete vert;
	if(trilist)
		delete trilist;
	if(norm)
		delete norm;
	if(uvv)
		delete uvv;
	if(diff)
		delete diff;

	return 1;
}
//-----------------------------------------------------------------------------
// Name: Sphere
// Desc: Generate a sphere model and texture it with imgname.
//-----------------------------------------------------------------------------
#define PI 3.14159f
#define TWOPI (2*PI)
DWORD CXModel::Sphere(float radius, DWORD tessx, DWORD tessy, DWORD fvf, char *imgname, float txscale)
{
	DWORD tidx;
	D3DVECTOR *vert;		// vertices
	WORD *tlist;			// triangle indices
	float *tc;				// index texcoords
	DWORD row, col;
	float ra, rr, ss, cc;
	DWORD vidx, *diff;

	vert = new D3DVECTOR[tessx*tessy];
	tlist = new WORD[(tessy-1)*(tessx-1)*6];
	tc = new float[tessx*tessy*2];

	// vertices
	for(row=0; row<tessy; row++)
	{
		ra = (float)row/(float)(tessy-1)*(PI*0.875f)+(PI*0.0625f);
		rr = radius * (float)sin(ra);

		for(col=0; col<tessx; col++)
		{
			vidx = row*tessx+col;
			ss = (float)sin(TWOPI*(float)(col)/(float)(tessx-1));
			cc = (float)cos(TWOPI*(float)(col)/(float)(tessx-1));

			vert[vidx].x = -rr*ss;
			vert[vidx].y = radius*(float)cos(ra);
			vert[vidx].z = rr*cc;

			tc[vidx*2] = txscale*(float)col/(float)(tessx-1);
			tc[vidx*2+1] = txscale*(float)row/(float)(tessy-1);
		}
	}

	// triangles
	tidx = 0;
	for(row=0; row<tessy-1; row++)
		for(col=0; col<tessx-1; col++)
		{
			tlist[tidx++] = (WORD)(row*tessx+col);
			tlist[tidx++] = (WORD)(row*tessx+col+1);
			tlist[tidx++] = (WORD)((row+1)*tessx+col);
			tlist[tidx++] = (WORD)((row+1)*tessx+col);
			tlist[tidx++] = (WORD)(row*tessx+col+1);
			tlist[tidx++] = (WORD)((row+1)*tessx+col+1);
		}

	if(fvf&D3DFVF_DIFFUSE)
	{
		diff = new DWORD[tessx*tessy];
		for(row=0; row<tessx*tessy; row++)
			diff[row] = (rand()<<16) | rand() | 0xff000000; 
	}
	else
		diff = NULL;


	Build(tessx*tessy, vert, tidx/3, tidx, tlist, tc, NULL, diff, fvf, 0);

	if(fvf&D3DFVF_DIFFUSE)
		delete diff;

    if(imgname)
		LoadTexture(imgname);

	delete vert;
	delete tlist;
	delete tc;

	return 1;
}

//-----------------------------------------------------------------------------
// Name: GridXZ
// Desc: Generate an XZ grid model.
//-----------------------------------------------------------------------------
DWORD CXModel::GridXZ(DWORD tessx, DWORD tessz, float xsize, float zsize, DWORD fvf, char *imgname, float texx, float texz)
{
	DWORD tidx, i;
	D3DVECTOR *vert;		// vertices
	D3DVECTOR *norm;
	WORD *tlist;			// triangle indices
	float *tc;				// index texcoords
	DWORD row, col;
	DWORD vidx;

	// clear normal flag because GenNormals does not support tristrips
	if(fvf&D3DFVF_NORMAL)
	{
		norm = new D3DVECTOR[tessx*tessz];
		ZeroMemory(norm, tessx*tessz*sizeof(D3DVECTOR));
		for(i=0; i<tessx*tessz; i++)
			norm[i].y = 1.0f;
	}
	else
		norm = NULL;

	vert = new D3DVECTOR[tessx*tessz];
	tlist = new WORD[(tessz-1)*(tessx-1)*6];
	tc = new float[tessx*tessz*2];

	// vertices, texcoords
	for(row=0; row<tessz; row++)
	{
		for(col=0; col<tessx; col++)
		{
			vidx = row*tessx+col;

			vert[vidx].x = (float)col*(xsize/(float)(tessx-1)) - (xsize/2.0f);
			vert[vidx].y = 0.0f;
			vert[vidx].z = (float)row*(zsize/(float)(tessz-1)) - (zsize/2.0f);

			tc[vidx*2] = (vert[vidx].z+(xsize/2.0f))/texz;
			tc[vidx*2+1] = (vert[vidx].x+(zsize/2.0f))/texx;
		}
	}
	
	// triangles
	tidx = 0;
	for(row=0; row<tessz-1; row++)
	{
		for(col=0; col<tessx; col++)
		{
			// add duplicate vertex for new row
			// (except on first row)
			if(col==0 && row!=0)
				tlist[tidx++] = (WORD)((row*tessx+col)&0xffff);

			tlist[tidx++] = (WORD)((row*tessx+col)&0xffff);
			tlist[tidx++] = (WORD)(((row+1)*tessx+col)&0xffff);
		}

		// add duplicate vertex at end of row
		// (except on last row)
		if(row!=(tessz-1))
		{
			tlist[tidx] = tlist[tidx-1];
			tidx++;
		}
	}

	Build(tessx*tessz, vert, tidx-2, tidx, tlist, tc, norm, NULL, fvf, 0);
	m_PrimType = D3DPT_TRIANGLESTRIP;

	if(imgname)
		LoadTexture(imgname);

	if(norm)
		delete norm;

	delete vert;
	delete tlist;
	delete tc;

	return 1;
}

//-----------------------------------------------------------------------------
// Name: Cylinder
// Desc: Generate a cylinder model and texture it with imgname.
//-----------------------------------------------------------------------------
DWORD CXModel::Cylinder(float rad0, DWORD diff0, float rad1, DWORD diff1, float len, DWORD tess, DWORD fvf, char *imgname, float txscale)
{
	DWORD tidx;
	D3DVECTOR *vert;		// vertices
	WORD *tlist;			// triangle indices
	float *tc;				// index texcoords
	DWORD col;
	float ss, cc;
	DWORD *diff;

	vert = new D3DVECTOR[tess*2];		// top & bottom disc
	tlist = new WORD[tess*2*3];			// 2 * tess * 3/tri
	tc = new float[tess*2*2];			// 2 * tess * 2/tri

	// vertices
	for(col=0; col<tess; col++)
	{
		ss = (float)sin(TWOPI*(float)(col)/(float)(tess-1));
		cc = (float)cos(TWOPI*(float)(col)/(float)(tess-1));

		// top disc
		vert[col].x = -rad0*ss;
		vert[col].y = rad0*cc;
		vert[col].z = 0.0f;

		// bottom disc
		vert[col+tess].x = -rad1*ss;
		vert[col+tess].y = rad1*cc;
		vert[col+tess].z = len;

		// texture samples
		tc[col*2] = txscale*(float)col/(float)(tess-1);
		tc[col*2+1] = 0.0f;
		tc[(col+tess)*2] = tc[col*2];
		tc[(col+tess)*2+1] = 1.0f;
	}

	// triangles
	tidx = 0;
	for(col=0; col<tess; col++)
	{
		tlist[tidx++] = (WORD)(col);
		tlist[tidx++] = (WORD)((col+1)%tess);
		tlist[tidx++] = (WORD)((col+tess+1)%(tess*2));
		tlist[tidx++] = tlist[tidx-3];
		tlist[tidx++] = tlist[tidx-2];
		tlist[tidx++] = (WORD)((col+tess)%(tess*2));
	}

	if(fvf&D3DFVF_DIFFUSE)
	{
		diff = new DWORD[tess*2];
		for(col=0; col<tess; col++)
		{
			diff[col] = diff0;
			diff[col+tess] = diff1;
		}
	}
	else
		diff = NULL;


	Build(tess*2, vert, tidx/3, tidx, tlist, tc, NULL, diff, fvf, 0);

	if(fvf&D3DFVF_DIFFUSE)
		delete diff;

    if(imgname)
		LoadTexture(imgname);

	delete vert;
	delete tlist;
	delete tc;

	return 1;
}

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Render a model. Note that render/texture state needs to be
//       set before you call this routine.
//-----------------------------------------------------------------------------
DWORD CXModel::Render()
{
	g_pd3dDevice->SetTexture(0, m_pTexture);
	g_pd3dDevice->SetVertexShader(m_dwVShader);
	g_pd3dDevice->SetIndices(m_pIB, 0);
	g_pd3dDevice->SetStreamSource(0, m_pVB, m_dwVBStride);
	g_pd3dDevice->DrawIndexedPrimitive(m_PrimType, 0, m_dwNumVertices, 0, m_dwNumTriangles);

	return 1;
}

//-----------------------------------------------------------------------------
// Name: LoadTexture
// Desc: Load a texture for use in a model.
//-----------------------------------------------------------------------------
void CXModel::LoadTexture(char *imgname)
{
	if(g_pModelResource!=NULL)
		m_pTexture = g_pModelResource->GetTexture(imgname);

	if(!m_pTexture)
	{
		OutputDebugStringA("XModel: Error getting texture from resource file: ");
		OutputDebugStringA(imgname);
		OutputDebugStringA("\n");
	}
}
