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
	if(m_pVBTex)
		m_pVBTex->Release();
	if(m_pIB)
		m_pIB->Release();
	if(m_Indices)
		delete m_Indices;
	if(m_UVs)
		delete m_UVs;
	if(m_pTexture)
		m_pTexture->Release();
}

//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Keep pointer to object and fill initial arrays from selection.
//       If aIndices==NULL, do whole mesh.
//       This routine does the work of Build, but skips the creation of
//       the m_Indices and m_UVs arrays.			BUGBUG: save won't work
//-----------------------------------------------------------------------------
HRESULT CXFPatchMesh::Initialize(CXObject *obj, WORD *aIndices, DWORD dwNumIndices)
{
	HRESULT hr;
	m_Obj = obj;
	assert(m_Obj->m_Model->m_dwFVF==FVF_XYZNORMTEX1);	// make sure we are using correct FVF type
	if (aIndices)
		return E_NOTIMPL;	// all or nothing at the moment
	m_dwNumVertices = m_Obj->m_Model->m_dwNumVertices;
	m_dwNumIndices = 3 * m_Obj->m_Model->m_dwNumTriangles;
	if(m_pVB) m_pVB->Release();
	m_pVB = m_Obj->m_Model->m_pVB;
	m_pVB->AddRef();
	if (m_pIB) m_pIB->Release();
	m_pIB = m_Obj->m_Model->m_pIB;
	m_pIB->AddRef();

	// Create texture coordinate frame vertex buffer
	if (m_pVBTex)
		SAFE_RELEASE(m_pVBTex);
	hr = g_pd3dDevice->CreateVertexBuffer(m_dwNumVertices * 2 * sizeof(D3DVECTOR), 0, 0, 0, &m_pVBTex);
	if (FAILED(hr))
		return E_FAIL;
	hr = CalculateTextureCoordinateFrames();
	if (FAILED(hr))
		return hr;
	
	m_bBuildFlag = FALSE;
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CalculateTextureCoordinateFrames
// Desc: Compute S and T vectors for all the vertices, based on the
//       local triangle coordinate frames.
//-----------------------------------------------------------------------------
HRESULT CXFPatchMesh::CalculateTextureCoordinateFrames()
{
	HRESULT hr;

	// Keep a count of how many times each vertex was touched.
	UINT *rCount = new UINT [ m_dwNumVertices ];
	if (rCount == NULL)
		return E_OUTOFMEMORY;
	ZeroMemory(rCount, m_dwNumVertices * sizeof(UINT));

	// Lock the buffers we need to iterate over
	struct XYZNORMTEX1 { 
		D3DXVECTOR3 v;
		D3DXVECTOR3 norm;
		D3DXVECTOR2 tex1;
	} *rVertex = NULL;
	struct TEXCOORDFRAME {
		D3DXVECTOR3 S;
		D3DXVECTOR3 T;
	} *rTexCoordFrame = NULL;
	WORD *riFace = NULL;
	
	hr = m_pVB->Lock(0, m_dwNumVertices, (BYTE **)&rVertex, 0);
	if (FAILED(hr))
		goto e_Exit;

	hr = m_pVBTex->Lock(0, m_dwNumVertices, (BYTE **)&rTexCoordFrame, 0);
	if (FAILED(hr))
		goto e_Exit;
	ZeroMemory(rTexCoordFrame, m_dwNumVertices * sizeof(TEXCOORDFRAME));

	hr = m_pIB->Lock(0, m_dwNumIndices*sizeof(WORD), (BYTE **)&riFace, 0);
	if (FAILED(hr))
		goto e_Exit;

	// Iterate through the triangles, updating the texture
	// coordinate frame for each touched vertex.
	static bool bAverage = false;
	for (UINT iFace = 0; iFace < m_dwNumIndices/3; iFace++)
	{
// DEBUG
#if 0
static UINT iFaceStart = 400;
static UINT nFace = 1;
if (iFace < iFaceStart) continue;
if (iFace >= iFaceStart + nFace) break;
#endif
		WORD *aiv = riFace + iFace * 3;
		for (UINT iCorner = 0; iCorner < 3; iCorner++)
		{
			WORD iVertex0 = aiv[iCorner];
			WORD iVertex1 = aiv[(iCorner + 1) % 3];
			WORD iVertex2 = aiv[(iCorner + 2) % 3];
			
			// A, B, C is coordinate frame in 3D object coords
			D3DXVECTOR3 A = rVertex[iVertex1].v - rVertex[iVertex0].v;
			D3DXVECTOR3 B = rVertex[iVertex2].v - rVertex[iVertex0].v;
			D3DXVECTOR3 C;
			D3DXVec3Cross(&C, &A, &B);
			float CLengthSq = D3DXVec3LengthSq(&C);

			// a, b, c is coordinate frame in texture coords
			D3DXVECTOR2 a = rVertex[iVertex1].tex1 - rVertex[iVertex0].tex1;
			D3DXVECTOR2 b = rVertex[iVertex2].tex1 - rVertex[iVertex0].tex1;
			float cz = D3DXVec2CCW( &a, &b );	// c has just one component out of the texture plane

            // Texture matrix M maps from texture coords to object coords.
            //
            // [ ax ay 0  ] M = [ Ax Ay Az ]
            // [ bx by 0  ]     [ Bx By Bz ]
            // [ 0  0  cz ]     [ Cx Cy Cz ]
			//
			// Get M by inverting texture coord matrix and multiplying by triangle frame.
            //
            // M = (1/cz) * [  by -ay 0 ] [ Ax Ay Az ]
            //              [ -bx  ax 0 ] [ Bx By Bz ]
            //              [  0   0  1 ] [ Cx Cy Cz ]
			//
			// Then, pretend matrix M is a scale * rotation (which will be the
			// case if the texture mapping is reasonable) and use scaled
			// transpose as inverse to transform from object coords back
			// to texture coords.
			//
			float fScale = cz * cz / CLengthSq;
			D3DXVECTOR3 S = (b.y * A - a.y * B) * fScale;
			D3DXVECTOR3 T = (-b.x * A + a.x * B) * fScale;
#ifdef _DEBUG
			// The simple solution is not working. Do the
			// full blown inverse to compare.
			D3DXMATRIX X(
				A.x, A.y, A.z, 0.f,
				B.x, B.y, B.z, 0.f,
				C.x, C.y, C.z, 0.f,
				0.f, 0.f, 0.f, 1.f);
			D3DXMATRIX XInv;
			float fXDet;
			D3DXMatrixInverse(&XInv, &fXDet, &X);
			D3DXMATRIX I;
			D3DXMatrixMultiply(&I, &XInv, &X);
			D3DXMATRIX Tex(
				a.x, a.y, 0.f, 0.f,
				b.x, b.y, 0.f, 0.f,
				0.f, 0.f, cz, 0.f,
				0.f, 0.f, 0.f, 1.f);
			D3DXMATRIX MInv;
			D3DXMatrixMultiply(&MInv, &XInv, &Tex);
			D3DXMATRIX Tex2;
			D3DXMatrixMultiply(&Tex2, &X, &MInv);
			static enum { SIMPLE, FULL, AB } e = FULL;
			if (e == FULL)
			{
				S = D3DXVECTOR3(MInv._11, MInv._21, MInv._31);
				T = D3DXVECTOR3(MInv._12, MInv._22, MInv._32);
			}
			else if (e == AB)
			{
				// For debugging, stick in A and B vectors instead of S and T
				S = A;
				T = B;
			}
#endif

			// To get texture coordinates for object space point p, use
			//     q = p - v0			// v0 is vertex position
			//     s = dot(S, q) + s0	// s0 and t0 are the vertex texture coords
			//     t = dot(T, q) + t0
			
#ifdef _DEBUG
			// Debug checking
			D3DXVECTOR3 SxT;
			D3DXVec3Cross(&SxT, &S, &T);
			float AdotS = D3DXVec3Dot(&A, &S);
			float AdotT = D3DXVec3Dot(&A, &T);
			float AdotSxT = D3DXVec3Dot(&A, &SxT);
			float BdotS = D3DXVec3Dot(&B, &S);
			float BdotT = D3DXVec3Dot(&B, &T);
			float BdotSxT = D3DXVec3Dot(&B, &SxT);
			float CdotS = D3DXVec3Dot(&C, &S);
			float CdotT = D3DXVec3Dot(&C, &T);
			float CdotSxT = D3DXVec3Dot(&C, &SxT);
#endif
			
			// Assign or accumulate S and T frames
			if (bAverage)
			{
				rTexCoordFrame[iVertex0].S += S;
				rTexCoordFrame[iVertex0].T += T;
			}
			else
			{
				rTexCoordFrame[iVertex0].S = S;
				rTexCoordFrame[iVertex0].T = T;
			}
			rCount[iVertex0]++;
		}
	}

	// Take average value for final frame
	if (bAverage)
	{
		for (UINT iVertex = 0; iVertex < m_dwNumVertices; iVertex++)
		{
			if (rCount[iVertex] == 0)
				continue;	// vertex was not touched
			float fScale = 1.f / (float)rCount[iVertex];
			rTexCoordFrame[iVertex].S *= fScale;
			rTexCoordFrame[iVertex].T *= fScale;
		}
	}
	
 e_Exit:
	if (rVertex != NULL)
		m_pVB->Unlock();
	if (rTexCoordFrame != NULL)
		m_pVBTex->Unlock();
	if (riFace != NULL)
		m_pIB->Unlock();
	delete [] rCount;
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ExtractFins
// Desc: Find all the edges of the mesh and create an IB to draw them.
//-----------------------------------------------------------------------------
HRESULT CXFPatchMesh::ExtractFins()
{
	HRESULT hr = S_OK;
	if (!m_pVB || !m_pIB) 
		return E_FAIL;
	UINT nFin = 0;
	UINT nFinMax = m_dwNumIndices; // 3 * nFace
	DWORD *aFin = new DWORD [nFinMax];
	if (aFin == NULL)
		return E_OUTOFMEMORY;

	// count the number of fins by adding all the edges to a set
	WORD *piFace;
	hr = m_pIB->Lock(0, m_dwNumIndices*sizeof(WORD), (BYTE **)&piFace, 0);
	if (FAILED(hr))
		goto e_Exit;
	for (UINT iFace = 0; iFace < m_dwNumIndices/3; iFace++)
	{
		WORD *aiv = piFace + iFace * 3;
		for (UINT iCorner = 0; iCorner < 3; iCorner++)
		{
			WORD iVertex0 = aiv[iCorner];
			WORD iVertex1 = aiv[(iCorner + 1) % 3];
			if (iVertex0 > iVertex1)
			{
				// swap so the iVertex0 is always lower than iVertex1
				WORD iVertexSave = iVertex0;
				iVertex0 = iVertex1;
				iVertex1 = iVertexSave;
			}
			DWORD finKey = (((DWORD)iVertex0) << 16) | iVertex1;
			// find the right position
			UINT i = 0, j, k = nFin;
			while (i != k)
			{
				j = (i + k) >> 1;
				if (aFin[j] < finKey)
					i = j + 1;
				else if (aFin[j] > finKey)
					k = j;
				else // (m_aFin[i] == key)
					break; // already in table
			}
			if (i == k)
			{
				// move all the slots one up, then add new key
				assert(nFin < nFinMax);
				for (j = nFin; j > i; j--)
					aFin[j] = aFin[j-1];
				aFin[i] = finKey;
				nFin++;
			}
		}
	}
	m_pIB->Unlock();

	// create the fin index buffer
	m_dwNumFins = nFin;
	if (m_pFinIB)
		SAFE_RELEASE(m_pFinIB);
	hr = g_pd3dDevice->CreateIndexBuffer(4*m_dwNumFins*sizeof(WORD), 0, D3DFMT_INDEX16, 0, &m_pFinIB);
	if (FAILED(hr))
		goto e_Exit;
	WORD *piFin;
	hr = m_pFinIB->Lock(0, m_dwNumFins*4*sizeof(WORD), (BYTE **)&piFin, 0);
	if (FAILED(hr))
		goto e_Exit;
	UINT iFin;
	for (iFin = 0; iFin < nFin; iFin++)
	{	
		DWORD finKey = aFin[iFin];
		WORD iVertex0 = (WORD)(finKey >> 16);
		WORD iVertex1 = (WORD)(finKey & 0xffff);
		// Indices are times two so drawing with half-stride and selector stream works.
		// This assumes that the indices are base-1, with 0 never actually used.
		assert(iVertex0 > 0 && iVertex1 > 0);
		*piFin++ = iVertex0 * 2;
		*piFin++ = iVertex1 * 2;
		*piFin++ = iVertex1 * 2 - 1;
		*piFin++ = iVertex0 * 2 - 1;
	}
	m_pFinIB->Unlock();

	// Create the fin selection stream.
	if (m_pFinVB)
		SAFE_RELEASE(m_pFinVB);
	hr = g_pd3dDevice->CreateVertexBuffer(m_dwNumVertices*2*sizeof(float), 0, 0, 0, &m_pFinVB);
	if (FAILED(hr))
		goto e_Exit;
	float *pfSelector;
	hr = m_pFinVB->Lock(0, m_dwNumVertices*2*sizeof(float), (BYTE **)&pfSelector, 0);
	if (FAILED(hr))
		goto e_Exit;
	for (UINT iVertex = 0; iVertex < m_dwNumVertices; iVertex++)
	{	
		*pfSelector++ = 1.f;	// select base point
		*pfSelector++ = 0.f;	// select base point + offset normal
	}
	m_pFinVB->Unlock();
e_Exit:
	if (aFin) delete [] aFin;
	return hr;
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
	if (!m_pIB)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwNumIndices/3);
	else
	{
		g_pd3dDevice->SetIndices(m_pIB, 0);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwNumVertices, 0, m_dwNumIndices/3);
	}
}

