//-----------------------------------------------------------------------------
// File: xfpmesh.cpp
//
// Desc: Facilitates drawing of 'fuzz' patches (hair, fur, grass, etc.)
//       by allowing you to 'paint' on an existing mesh and use those
//       triangles for the fuzz.
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include "xfpmesh.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

//-----------------------------------------------------------------------------
// Name: Constructor & Destructor
//-----------------------------------------------------------------------------
CXFPatchMesh::CXFPatchMesh()
{
	memset(this, 0, sizeof(CXFPatchMesh));

	m_dwVShader = FVF_XYZNORMTEX1;
}

CXFPatchMesh::~CXFPatchMesh()
{
	if(m_pVB)
		m_pVB->Release();
	if(m_Indices)
		delete m_Indices;
	if(m_UVs)
		delete m_UVs;
	if(m_pTexture)
		m_pTexture->Release();
}

//-----------------------------------------------------------------------------
// Name: LineEQ
// Desc: Determines a line equation to facilitate picking triangles
//-----------------------------------------------------------------------------
void LineEQ(D3DXVECTOR3 *p1, D3DXVECTOR3 *p2, float *a, float *b, float *c)
{
	*a = -(p1->y - p2->y);
	*b = p1->x - p2->x;
	*c = -(*a*p1->x + *b*p1->y);
}

//-----------------------------------------------------------------------------
// Name: PickTriangle
// Desc: Searches an objects model for a triangle that contains the point
//       specified in p (in screen space).
//
// Note: This routine would be used in a tool to pick which triangles
//       you want for this pmesh.
//-----------------------------------------------------------------------------
DWORD CXFPatchMesh::PickTriangle(CXObject *obj, POINT p, DWORD flag, D3DVIEWPORT8 *Viewport, D3DXMATRIX *matProj, D3DXMATRIX *matView, D3DXMATRIX *matWorld)
{
	BYTE *vptr, *iptr;
	DWORD i, j, ret;
	int dup;
	D3DVECTOR v1, v2, v3;
	D3DXVECTOR3 vp1, vp2, vp3;
	WORD i1, i2, i3, *tris;
	float a, b, c, d;

	obj->m_Model->LockVB(&vptr, 0);
	obj->m_Model->LockIB(&iptr, 0);
	tris = (WORD *)iptr;

	for(i=0; i<obj->m_Model->m_dwNumIndices/3; i++)
	{
		i1 = tris[i*3];
		i2 = tris[i*3+1];
		i3 = tris[i*3+2];

		FVF_GetVert(vptr, &v1, i1, 1, obj->m_Model->m_dwFVF);
		FVF_GetVert(vptr, &v2, i2, 1, obj->m_Model->m_dwFVF);
		FVF_GetVert(vptr, &v3, i3, 1, obj->m_Model->m_dwFVF);

		D3DXVec3Project(&vp1, (D3DXVECTOR3 *)&v1, Viewport, matProj, matView, matWorld);
		D3DXVec3Project(&vp2, (D3DXVECTOR3 *)&v2, Viewport, matProj, matView, matWorld);
		D3DXVec3Project(&vp3, (D3DXVECTOR3 *)&v3, Viewport, matProj, matView, matWorld);

		// check if point inside triangle
		LineEQ(&vp1, &vp2, &a, &b, &c);
		d = a*(float)p.x + b*(float)p.y + c;
		if(d>0)
			continue;
		LineEQ(&vp2, &vp3, &a, &b, &c);
		d = a*(float)p.x + b*(float)p.y + c;
		if(d>0)
			continue;
		LineEQ(&vp3, &vp1, &a, &b, &c);
		d = a*(float)p.x + b*(float)p.y + c;
		if(d>0)
			continue;

		// check for duplicate tri
		dup = -1;
		for(j=0; j<m_dwNumIndices/3; j++)
		{
			if((m_Indices[j*3]==i1) && (m_Indices[j*3+1]==i2) && (m_Indices[j*3+2]==i3))
			{
				dup = j;
				break;
			}
		}

		// flag==0, do add or delete
		// flag==1, do only add
		// flag==2, do only delete
		if((flag==0 || flag==1) && (dup==-1))			// do add
		{
			// make sure we have space
			if(m_dwNumIndices>=(m_dwMaxIndices-5))
			{
				// expand index space
				m_dwMaxIndices = m_dwNumIndices+300;

				// copy indices
				WORD *newindices = new WORD[m_dwMaxIndices];
				memcpy(newindices, m_Indices, m_dwNumIndices*sizeof(WORD));
				if(m_Indices)
					delete m_Indices;
				m_Indices = newindices;
	
				// copy texture samples
				float *newuvs = new float[m_dwMaxIndices*2];
				memcpy(newuvs, m_UVs, m_dwNumIndices*2*sizeof(float));
				if(m_UVs)
					delete m_UVs;
				m_UVs = newuvs;
			}

			// add triangle
			m_Indices[m_dwNumIndices++] = i1;
			m_Indices[m_dwNumIndices++] = i2;
			m_Indices[m_dwNumIndices++] = i3;

			ret = 1;
			m_bBuildFlag = TRUE;
			goto bail;
		}
		else if((flag==0 || flag==2) && (dup>=0))		// do delete
		{
			m_Indices[dup*3+2] = m_Indices[--m_dwNumIndices];
			m_Indices[dup*3+1] = m_Indices[--m_dwNumIndices];
			m_Indices[dup*3] = m_Indices[--m_dwNumIndices];

			ret = 2;
			m_bBuildFlag = TRUE;
			goto bail;
		}
	}

bail:
	obj->m_Model->UnlockVB();
	obj->m_Model->UnlockIB();

	return ret;											// return last op
}

//-----------------------------------------------------------------------------
// Name: File routines
// Desc: Routines to save and load patch mesh descriptions
//-----------------------------------------------------------------------------
struct _pmhdr
{
	char sig[4];
	char fpatchname[32];
	DWORD nidx;
	D3DVECTOR texcenter;
	D3DVECTOR texsize;
	DWORD mapping;
	DWORD axis;
};

void CXFPatchMesh::Save(char *fname)
{
	FILE *fp;
	struct _pmhdr hdr;

	fp = fopen(fname, "wb");
	if(!fp)
		return;

	strcpy(hdr.sig, "PMSH");
	strcpy(hdr.fpatchname, m_strFPatchName);

	hdr.nidx = m_dwNumIndices;

	hdr.texcenter = m_TextureCenter;
	hdr.texsize = m_TextureSize;
	hdr.mapping = m_dwMapping;
	hdr.axis = m_dwAxis;

	fwrite(&hdr, sizeof(struct _pmhdr), 1, fp);
	fwrite(m_Indices, sizeof(WORD), m_dwNumIndices, fp);
	fwrite(m_UVs, sizeof(float), m_dwNumIndices*2, fp);

	fclose(fp);
}

void CXFPatchMesh::Load(char *fname)
{
	FILE *fp;
	struct _pmhdr hdr;

	fp = fopen(fname, "rb");
	if(!fp)
		return;

	fread(&hdr, sizeof(struct _pmhdr), 1, fp);
	if(strncmp(hdr.sig, "PMSH", 4))
	{
		fclose(fp);
		return;
	}

	strcpy(m_strFPatchName, hdr.fpatchname);

	m_dwNumIndices = hdr.nidx;
	m_dwMaxIndices = hdr.nidx;

	m_TextureCenter = hdr.texcenter;
	m_TextureSize = hdr.texsize;
	m_dwMapping = hdr.mapping;
	m_dwAxis = hdr.axis;

	// allocate memory
	if(m_Indices)
		delete m_Indices;
	m_Indices = new WORD[m_dwNumIndices];

	if(m_UVs)
		delete m_UVs;
	m_UVs = new float[2*m_dwNumIndices];

	// read in the data
	fread(m_Indices, sizeof(WORD), m_dwNumIndices, fp);
	fread(m_UVs, sizeof(float), m_dwNumIndices*2, fp);
	fclose(fp);

	// build index buffer
	BuildIndexBuffer();

	m_bBuildFlag = TRUE;
}

//-----------------------------------------------------------------------------
// Name: BuildIndexBuffer
// Desc: Builds the pmesh index buffer for rendering
//-----------------------------------------------------------------------------
void CXFPatchMesh::BuildIndexBuffer()
{
/*
	WORD idx, *ibuf, *tbuf;
	WORD wMaxIndex;
	DWORD i;

	// scan indices to get max index
	wMaxIndex = 0;
	for(i=0; i<m_dwNumIndices; i++)
		if(m_Indices[i]>wMaxIndex)
			wMaxIndex = m_Indices[i];

	tbuf = new WORD[wMaxIndex];
	memset(tbuf, 0xffffffff, wMaxIndex*sizeof(WORD));

	g_pd3dDevice->CreateIndexBuffer(m_dwNumIndices*sizeof(WORD), 0, D3DFMT_INDEX16, 0, &m_pIB);
	m_pIB->Lock(0, m_dwNumIndices*sizeof(WORD), (BYTE **)&ibuf, 0);

	idx = 0;
	for(i=0; i<m_dwNumIndices; i++)
	{
		if(tbuf[m_Indices[i]]==0xffff)
		{
			tbuf[m_Indices[i]] = idx;
			ibuf[i] = idx++;
		}
		else
			ibuf[i] = tbuf[m_Indices[i]];
	}

	m_wMaxIndex = idx;			// keep track of biggest index
	m_pIB->Unlock();

	delete tbuf;
*/
}

//-----------------------------------------------------------------------------
// Name: Build
// Desc: Gets vertices and normals from the underlying object for our
//       patch mesh.
//-----------------------------------------------------------------------------
void CXFPatchMesh::Build(int firstflag)
{
	FVFT_XYZNORMTEX1 *vptr, *vptr2;
	DWORD i;

	if(!m_dwNumIndices || !m_bBuildFlag)
		return;

	// make sure we are using correct FVF type
	assert(m_Obj->m_Model->m_dwFVF==FVF_XYZNORMTEX1);

	// get memory for our vertices
	if(m_dwNumVertices<m_dwNumIndices)
	{
		if(m_pVB)
			m_pVB->Release();

		m_dwNumVertices = m_dwNumIndices;
		g_pd3dDevice->CreateVertexBuffer(m_dwNumVertices*sizeof(FVFT_XYZNORMTEX1), 0, FVF_XYZNORMTEX1, 0, &m_pVB);
	}

	// get the vertices and normals from our underlying model
	m_pVB->Lock(0, m_dwNumVertices*sizeof(FVFT_XYZNORMTEX1), (BYTE **)&vptr, 0);
	m_Obj->m_Model->LockVB((BYTE **)&vptr2, 0);
	for(i=0; i<m_dwNumIndices; i++)
	{
		vptr[i].v = vptr2[m_Indices[i]].v;
		vptr[i].norm = vptr2[m_Indices[i]].norm;
	}

	// if this is the first build, copy the UV's into the PMesh VB
	if(firstflag)
		for(i=0; i<m_dwNumIndices; i++)
		{
			vptr[i].tu = m_UVs[i<<1];
			vptr[i].tv = m_UVs[(i<<1)+1];
		}

	// unlock vertex buffers
	m_pVB->Unlock();
	m_Obj->m_Model->UnlockVB();

	m_bBuildFlag = FALSE;
}

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Draw the patch mesh on its own. Used to see exactly which triangles
//       are selected. (Normally, another class would use the pmesh data only
//       to do its rendering)
//-----------------------------------------------------------------------------
void CXFPatchMesh::Render()
{
	if(!m_dwNumIndices)			// make sure we have stuff to draw
		return;

	g_pd3dDevice->SetTexture(0, m_pTexture);
	g_pd3dDevice->SetVertexShader(m_dwVShader);
	g_pd3dDevice->SetStreamSource(0, m_pVB, sizeof(FVFT_XYZNORMTEX1));
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwNumIndices/3);
}

