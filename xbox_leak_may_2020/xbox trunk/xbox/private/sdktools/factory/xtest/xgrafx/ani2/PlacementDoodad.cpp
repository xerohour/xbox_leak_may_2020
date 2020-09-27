///////////////////////////////////////////////////////////////////////////////
// File: PlacementDoodad.cpp
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#include "precomp.h"
#include "defines.h"
#ifdef INCLUDE_PLACEMENT_DOODAD

#include "xbs_math.h"
#include "renderer.h"
#include "PlacementDoodad.h"
///////////////////////////////////////////////////////////////////////////////
#define FVF_xyzrgba (D3DFVF_XYZ|D3DFVF_DIFFUSE)
struct xyzrgba_vertex
{
	float x,y,z;
	DWORD diffuse;
};
///////////////////////////////////////////////////////////////////////////////
void PlacementDoodad::Init()
{
    m_pVB = NULL;
    m_pIB = NULL;
    m_dwNumVertices = 0;
    m_dwNumIndices = 0;
}
///////////////////////////////////////////////////////////////////////////////
void PlacementDoodad::create()
{
	m_dwNumVertices = 6;
	m_dwNumIndices = 6;
	
	gpd3dDev->CreateVertexBuffer(sizeof(xyzrgba_vertex) * m_dwNumVertices, NULL, FVF_xyzrgba,NULL, &m_pVB);
	gpd3dDev->CreateIndexBuffer(m_dwNumIndices, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB);

	xyzrgba_vertex* p_vb;
	m_pVB->Lock(0, 0, (BYTE**) &p_vb, 0);

	int i;
	for (i=0; i<3; i++)
	{
		p_vb[2*i+0].x = (i==0) ? -100.0f : 0.0f;
		p_vb[2*i+1].x = (i==0) ? +100.0f : 0.0f;
		p_vb[2*i+0].y = (i==1) ? -100.0f : 0.0f;
		p_vb[2*i+1].y = (i==1) ? +100.0f : 0.0f;
		p_vb[2*i+0].z = (i==2) ? -100.0f : 0.0f;
		p_vb[2*i+1].z = (i==2) ? +100.0f : 0.0f;
		p_vb[2*i+0].diffuse = p_vb[2*i+1].diffuse = 0xFFFFFFFF;
	}

	m_pVB->Unlock();


	WORD* p_ib;
	m_pIB->Lock(0, 0, (BYTE**) &p_ib, 0);

	for (i=0; i<6; i++) p_ib[i] = (WORD)i;

	m_pIB->Unlock();
}
///////////////////////////////////////////////////////////////////////////////
void PlacementDoodad::destroy()
{
	m_pVB->Release();
	m_pIB->Release();
	m_pVB = NULL;
	m_pIB = NULL;
}
///////////////////////////////////////////////////////////////////////////////
void PlacementDoodad::render(const D3DVECTOR* p_pos, const D3DMATRIX* view_mat, const D3DMATRIX* proj_mat)
{
    // Set default states
    gpd3dDev->SetRenderState( D3DRS_LIGHTING,			FALSE );
    gpd3dDev->SetRenderState( D3DRS_ZENABLE,			TRUE );
    gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );

	gpd3dDev->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
    gpd3dDev->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

	gpd3dDev->SetVertexShader(FVF_xyzrgba);

	D3DMATRIX iden;
	SetIdentity(&iden);
	*((D3DVECTOR*)(&iden._41)) = *p_pos;
	gpd3dDev->SetTransform(D3DTS_WORLD, &iden);

	if (view_mat) gpd3dDev->SetTransform(D3DTS_VIEW, view_mat);
	if (proj_mat) gpd3dDev->SetTransform(D3DTS_PROJECTION, proj_mat);
	
	gpd3dDev->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
	gpd3dDev->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
	gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1);
	gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE);
	gpd3dDev->SetTextureStageState(1,D3DTSS_COLOROP,	D3DTOP_DISABLE);

	gpd3dDev->SetIndices( m_pIB, 0 );
	gpd3dDev->SetStreamSource(0, m_pVB, sizeof(xyzrgba_vertex));
	gpd3dDev->DrawIndexedPrimitive(D3DPT_LINELIST, 0, m_dwNumVertices, 0, m_dwNumIndices / 2);

	gpd3dDev->SetPixelShader(NULL);
	gpd3dDev->SetVertexShader(NULL);

    gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );
	gpd3dDev->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_DISABLE);
}
///////////////////////////////////////////////////////////////////////////////
#endif // INCLUDE_PLACEMENT_DOODAD