#include "xshadow.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

#define MAX_SHADOWTRIS 32767

// full screen shadow poly
FVFT_XYZRHWDIFF shadowpoly[4] =
{
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0x3f000000 },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0x3f000000 },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0x3f000000 },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0x3f000000 }
};

CXShadowVolume::CXShadowVolume(float w, float h)
{
	memset(this, 0, sizeof(CXShadowVolume));
	m_fWidth = w;
	m_fHeight = h;
}

CXShadowVolume::~CXShadowVolume()
{
}

//
// shadow volume code
//
void CXShadowVolume::AddEdge(DWORD *edgelist, WORD v0, WORD v1)
{
	DWORD i, key;

	if(v0<v1)
		key = (v0<<16)|v1;
	else
		key = (v1<<16)|v0;

	// remove duplicate edges from list		
	for(i=0; i<m_dwNumEdges; i++)
		if(edgelist[i*3]==key)
		{
			m_dwNumEdges--;
			edgelist[i*3+0] = edgelist[3*m_dwNumEdges+0];
			edgelist[i*3+1] = edgelist[3*m_dwNumEdges+1];
			edgelist[i*3+2] = edgelist[3*m_dwNumEdges+2];
			return;
		}

	// no dup found, add this edge
	if(v0<v1)
		edgelist[m_dwNumEdges*3] = (v0<<16)|v1;
	else
		edgelist[m_dwNumEdges*3] = (v1<<16)|v0;

	edgelist[m_dwNumEdges*3+1] = (DWORD)v0;
	edgelist[m_dwNumEdges*3+2] = (DWORD)v1;
	m_dwNumEdges++;
}

void CXShadowVolume::AddObj(CXObject *obj, D3DXVECTOR3 *lightpos)
{
	DWORD *edgelist;
	D3DXMATRIX im;									// inverse matrix
	D3DXVECTOR4 lpos;								// light position
	BYTE *vptr, *iptr;								// vertex/index pointer
	WORD *tris;										// trilist pointer
	DWORD stride, i, nsi, fvf;
	WORD i0, i1, i2, nsvert;
	D3DXVECTOR3 v, v0, v1, v2;
	D3DXVECTOR3 norm, e0, e1;

	// allocate memory for edge list
	edgelist = new DWORD[2*MAX_SHADOWTRIS];

	// calculate inverse matrix (->object space)
	D3DXMatrixInverse(&im, NULL, (D3DXMATRIX *)&obj->m_matOrientation);

	// transform light into object space
	D3DXVec3Transform(&lpos, lightpos, &im);

	// get object info
	obj->m_Model->LockVB(&vptr, 0);
	obj->m_Model->LockIB(&iptr, 0);
	tris = (WORD *)iptr;
	fvf = obj->m_Model->m_dwFVF;
	stride = FVF_VSize(fvf);		// get vertex size

	// add edges
	m_dwNumEdges = 0;
	for(i=0; i<obj->m_Model->m_dwNumTriangles; i++)
	{
		i0 = tris[i*3+0];
		i1 = tris[i*3+1];
		i2 = tris[i*3+2];

		v0 = *((D3DXVECTOR3 *)(vptr + stride*i0));
		v1 = *((D3DXVECTOR3 *)(vptr + stride*i1));
		v2 = *((D3DXVECTOR3 *)(vptr + stride*i2));

		// calc tri normal
		D3DXVec3Cross(&norm, &(v2-v1), &(v1-v0));
		D3DXVec3Normalize(&norm, &norm);

		// dot light vector with tri norm to see if 
		// triangle faces the light
		v.x = lpos.x - v0.x;		// light-vert
		v.y = lpos.y - v0.y;
		v.z = lpos.z - v0.z;

		if(D3DXVec3Dot(&norm, &v)>0.0f)
		{
			AddEdge(edgelist, i0, i1);
			AddEdge(edgelist, i1, i2);
			AddEdge(edgelist, i2, i0);
		}
	}

	// allocate shadowvolume object
	if(m_Vert)
		delete m_Vert;
	if(m_Tris)
		delete m_Tris;

	m_Vert = new FVFT_XYZDIFF[m_dwNumEdges*4];			// allocate vertex memory
	stride = FVF_VSize(FVF_XYZDIFF);
	m_Tris = new WORD[m_dwNumEdges*6];					// allocate triangle memory

	m_dwNumTriangles = m_dwNumEdges*2;
	m_dwNumVertices = m_dwNumEdges*4;
	m_Matrix = obj->m_matOrientation;

	// extrude edges
	nsvert = 0;
	nsi = 0;
	for(i=0; i<m_dwNumEdges; i++)
	{
		FVF_GetVert(vptr, &e0, edgelist[i*3+1], 1, fvf);
		FVF_GetVert(vptr, &e1, edgelist[i*3+2], 1, fvf);

		m_Tris[nsi++] = nsvert+0;
		m_Tris[nsi++] = nsvert+1;
		m_Tris[nsi++] = nsvert+2;
		m_Tris[nsi++] = nsvert+1;
		m_Tris[nsi++] = nsvert+3;
		m_Tris[nsi++] = nsvert+2;

		// e0
		m_Vert[nsvert].v.x = e0.x;
		m_Vert[nsvert].v.y = e0.y;
		m_Vert[nsvert++].v.z = e0.z;

		// e1
		m_Vert[nsvert].v.x = e1.x;
		m_Vert[nsvert].v.y = e1.y;
		m_Vert[nsvert++].v.z = e1.z;

		// e2
		m_Vert[nsvert].v.x = 20*(e0.x - lpos.x);
		m_Vert[nsvert].v.y = 20*(e0.y - lpos.y);
		m_Vert[nsvert++].v.z = 20*(e0.z - lpos.z);

		// e3
		m_Vert[nsvert].v.x = 20*(e1.x - lpos.x);
		m_Vert[nsvert].v.y = 20*(e1.y - lpos.y);
		m_Vert[nsvert++].v.z = 20*(e1.z - lpos.z);
	}

	obj->m_Model->UnlockVB();
	obj->m_Model->UnlockIB();

	delete edgelist;
}

void CXShadowVolume::Render()
{
	shadowpoly[0].v.y = m_fHeight;
	shadowpoly[2].v.x = m_fWidth;
	shadowpoly[2].v.y = m_fHeight;
	shadowpoly[3].v.x = m_fWidth;

	// set up for shadow volume render
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);		// disable writes, tests still occur
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);		// enable stencil buffer
	g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);	// dont interpolate colors
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);	// stencil test always passes
																	// dont need fail setup since always passes
	g_pd3dDevice->SetRenderState(D3DRS_STENCILREF, 0x1);			// bump stencil count for each pixel
	g_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0Xffffffff);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0Xffffffff);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);		// dont draw any pixels
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// draw fronts w/ stencil increment
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	g_pd3dDevice->SetVertexShader(FVF_XYZDIFF);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_Matrix);

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
	g_pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_dwNumVertices, m_dwNumTriangles, m_Tris,
							D3DFMT_INDEX16, m_Vert, sizeof(FVFT_XYZDIFF));

	// draw rears with stencil decrement
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
	g_pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_dwNumVertices, m_dwNumTriangles, m_Tris,
							D3DFMT_INDEX16, m_Vert, sizeof(FVFT_XYZDIFF));

	// attenuate pixels in stencil region (shadow)
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);				// no zbuffer
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);		// uses stencil compare
	g_pd3dDevice->SetRenderState(D3DRS_STENCILREF, 0X1);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL); // <= 1
	g_pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	g_pd3dDevice->SetVertexShader(FVF_XYZRHWDIFF);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, shadowpoly, sizeof(FVFT_XYZRHWDIFF));

	// reset render state
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}
