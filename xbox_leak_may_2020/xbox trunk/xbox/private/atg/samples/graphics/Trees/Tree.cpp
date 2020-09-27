//-----------------------------------------------------------------------------
//  
//  File: Tree.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
// A tree represented by three different levels of detail:
//   full geometry, slice-texture branches, and whole tree slice textures
//-----------------------------------------------------------------------------
#ifdef _XBOX
#include <xtl.h>
#include <xgraphics.h>
#else
#include <d3d8.h>
#endif
#include <XBApp.h>
#include "Tree.h"
#include "Xfvf.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

//////////////////////////////////////////////////////////////////////
// Simple branch rotations and offsets. In a game, these would
// be defined from a model file or procedurally.
//
static struct BranchXfm {
	float x, y, z;	// translation
	float yrot; // rotation in y
} s_rBranchXfm[] = {
	{0.2f, 4.65f, 0.25f,	-63.f * D3DX_PI / 180.f },
	{0.0f, 7.f, -0.1f,		49.6f * D3DX_PI / 180.f },
	{-0.4f, 8.4f, 0.6f,		167.f * D3DX_PI / 180.f },
};
static UINT s_nBranch = sizeof(s_rBranchXfm) / sizeof(BranchXfm);

//////////////////////////////////////////////////////////////////////
// load mesh and create slice textures
//
HRESULT CTree::Create(CHAR *strName, CXBPackedResource *pResource)
{
	HRESULT hr;
	if (strcmp(strName, "tree1"))
		return E_NOTIMPL;

	// Load the tree vertex shader
	{
		DWORD vsdecl[] = 
		{
			D3DVSD_STREAM(0),
			D3DVSD_REG(0, D3DVSDT_FLOAT3),		// position
			D3DVSD_REG(1, D3DVSDT_FLOAT3),		// normal
			D3DVSD_REG(2, D3DVSDT_FLOAT2),		// texcoords
			D3DVSD_END()
		};
		m_dwVertexShader = 0;
		hr = XBUtil_CreateVertexShader(g_pd3dDevice, "Shaders\\Tree.xvu", vsdecl, &m_dwVertexShader);
		if(FAILED(hr) || !m_dwVertexShader)
		{
			OUTPUT_DEBUG_STRING( "CTree::Create : error loading Tree.xvu\n" );
			return XBAPPERR_MEDIANOTFOUND;
		}
	}

	// Load the tree branch
	CHAR *strBranch = "Models\\branch4.xbg";
	hr = m_TreeBranch.Create(strBranch, pResource);
	if (FAILED(hr))
		return hr;
	// Put the instances in place
	m_nBranch = s_nBranch;
	m_rBranch = new Branch [ m_nBranch ];
	if (m_rBranch == NULL)
		return E_OUTOFMEMORY;
	for (UINT iBranch = 0; iBranch < m_nBranch; iBranch++)
	{
		D3DXMATRIX matRotY;
		D3DXMatrixRotationY(&matRotY, s_rBranchXfm[iBranch].yrot);
		D3DXMATRIX matTranslate;
		D3DXMatrixTranslation(&matTranslate, s_rBranchXfm[iBranch].x, s_rBranchXfm[iBranch].y, s_rBranchXfm[iBranch].z);
		m_rBranch[iBranch].m_matXfm =  matRotY * matTranslate;
		D3DXMatrixInverse(&m_rBranch[iBranch].m_matXfmInv, NULL, &m_rBranch[iBranch].m_matXfm);
	}

	// Load the trunk
	CHAR *strTrunk = "Models\\trunk.xbg";
	m_pMeshTrunk = NULL;
	m_pMeshTrunk = new CTreeShaderMesh;
	if (!m_pMeshTrunk)
		return E_OUTOFMEMORY;
	if( FAILED( m_pMeshTrunk->Create( g_pd3dDevice, strTrunk, pResource )))
		return XBAPPERR_MEDIANOTFOUND;
	
	// Get the overall bounding box
	hr = ComputeBoundingBox(&m_vMin, &m_vMax);
	if (FAILED(hr))
		return hr;

	// Slice the branch
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld);
	Begin(true);	// setup vertex shader, etc.
	hr = m_TreeBranch.Slice();	// turn geometry into texture by taking slices
	End();
	if (FAILED(hr))
		return hr;
	D3DXMatrixIdentity(&matWorld);
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld);
	return hr;
}

//////////////////////////////////////////////////////////////////////
//  Take the union of two boxes
//
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

//////////////////////////////////////////////////////////////////////
// Calculate bounding box of tree trunk and foliage
//
HRESULT CTree::ComputeBoundingBox(D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax)
{
	// start with bounding box of trunk
	HRESULT hr = m_pMeshTrunk->ComputeBoundingBox(pvMin, pvMax);
	if (FAILED(hr))
		return hr;

	// union in branch bounding boxes
#define MAXMAT 100	
	D3DXMATRIX matSave[MAXMAT];
	if (m_TreeBranch.m_pMesh->m_dwNumFrames > MAXMAT) 
		return E_NOTIMPL;
	// Save original xfms
	for (UINT iFrame = 0; iFrame < m_TreeBranch.m_pMesh->m_dwNumFrames; iFrame++)
	{
		XBMESH_FRAME *pFrame = &m_TreeBranch.m_pMesh->m_pMeshFrames[iFrame];
		matSave[iFrame] = pFrame->m_matTransform;
	}
	for (UINT iBranch = 0; iBranch < m_nBranch; iBranch++)
	{
		// Brain surgery on top-level frame of branch mesh
		for (UINT iFrame = 0; iFrame < m_TreeBranch.m_pMesh->m_dwNumFrames; iFrame++)
		{
			XBMESH_FRAME *pFrame = &m_TreeBranch.m_pMesh->m_pMeshFrames[iFrame];
			pFrame->m_matTransform = matSave[iFrame] * m_rBranch[iBranch].m_matXfm;
		}

		// Get new bounding box of transformed mesh
		m_TreeBranch.m_pMesh->ComputeBoundingBox(&m_rBranch[iBranch].m_vMin, &m_rBranch[iBranch].m_vMax);
		UnionBox(pvMin, pvMax, m_rBranch[iBranch].m_vMin, m_rBranch[iBranch].m_vMax);

	}
	// Restore original xfms
	for (iFrame = 0; iFrame < m_TreeBranch.m_pMesh->m_dwNumFrames; iFrame++)
	{
		XBMESH_FRAME *pFrame = &m_TreeBranch.m_pMesh->m_pMeshFrames[iFrame];
		pFrame->m_matTransform = matSave[iFrame];
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Compute slice texture from whole tree geometry
//
HRESULT CTree::Slice()
{
	SliceInfo rSliceInfo[3] = {
		{ 8, 128, 128, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, D3DFMT_DXT4,
		  { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, 0.4f, 0.6f },
		{ 8, 128, 128, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, D3DFMT_DXT4,
		  { 0.f, 1.f, 0.f }, { 1.f, 0.f, 0.f }, 0.4f, 0.6f },
		{ 8, 128, 128, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, D3DFMT_DXT4,
		  { 0.f, 0.f, 1.f }, { -1.f, 0.f, 0.f }, 0.4f, 0.6f },
	};
	return CSliceObject::Slice(3, rSliceInfo);
}

//////////////////////////////////////////////////////////////////////
// Prepare for drawing into slice textures or for drawing
// the full geometry.
//
HRESULT CTree::Begin(bool bWorldCoords)
{
	HRESULT hr;
	hr = CSliceObject::Begin(bWorldCoords);
	if (FAILED(hr))
		return hr;
	
	DWORD xx = 0;
#if 1
	/* Use global light */
	g_pd3dDevice->LightEnable(0, TRUE);
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLOROP, D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
#else
   g_pd3dDevice->LightEnable(0, FALSE);
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
#endif
 	g_pd3dDevice->SetPixelShader(0);
 	g_pd3dDevice->SetTexture( xx, NULL );
	for (xx = 1; xx < 4; xx++)
	{
		g_pd3dDevice->SetTexture( xx, NULL );
		g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLOROP, D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	}
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
	g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	static DWORD dwAlphaRef = 0;
	g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, dwAlphaRef );
	static BOOL bAlphaBlend = TRUE;
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, bAlphaBlend );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	SetupVertexShader();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Setup the vertex shader to prepare for drawing
//
HRESULT CTree::SetupVertexShader()
{
	D3DXVECTOR3 vCenter = 0.5f * (m_vMin + m_vMax);
	static float fFractionY = 0.3f; // scale y to be near base of tree
	D3DXVECTOR3 vCenterOffset(vCenter.x, fFractionY * vCenter.y, vCenter.z);
	D3DXVECTOR4 vShadowCenter(vCenterOffset.x, vCenterOffset.y, vCenterOffset.z, 1.f);
	D3DXVECTOR3 vCorner = m_vMax - vCenterOffset;
	static float fColorScaleExtra = 1.f;
	float f1_R2 = fColorScaleExtra / D3DXVec3LengthSq(&vCorner);
	static D3DXCOLOR colorOffset(0.2f, 0.2f, 0.2f, 1.f);
	D3DXCOLOR colorScale = f1_R2 * (D3DXCOLOR(1.f, 1.f, 1.f, 1.f) - colorOffset);
	m_pMeshTrunk->m_dwVertexShader = m_dwVertexShader;
	m_pMeshTrunk->m_vShadowCenter = vShadowCenter;
	m_pMeshTrunk->m_colorOffset = colorOffset;
	m_pMeshTrunk->m_colorScale = colorScale;
	
	// take average of shadow center mapped back into branch coords
	D3DXVECTOR3 vBranchShadowCenterAvg(0.f, 0.f, 0.f);
	for (UINT iBranch = 0; iBranch < m_nBranch; iBranch++)
	{
		D3DXVECTOR3 vBranchShadowCenter;
		D3DXVec3TransformCoord(&vBranchShadowCenter, &vCenterOffset, &m_rBranch[iBranch].m_matXfmInv);
		vBranchShadowCenterAvg += (1.f / m_nBranch) * vBranchShadowCenter;
	}
	m_TreeBranch.m_pMesh->m_dwVertexShader = m_dwVertexShader;
	m_TreeBranch.m_pMesh->m_vShadowCenter = D3DXVECTOR4(vBranchShadowCenterAvg.x, vBranchShadowCenterAvg.y, vBranchShadowCenterAvg.z, 1.f);
	m_TreeBranch.m_pMesh->m_colorOffset = colorOffset;
	m_TreeBranch.m_pMesh->m_colorScale = colorScale;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Draw tree into slice texture
//
HRESULT CTree::Draw(const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax)		// draw all that intersects range
{
	D3DXVECTOR3 vFrom = 0.5f * (m_vMin + m_vMax); // start at center
	D3DXVECTOR3 vDiag = 0.5f * (m_vMax - m_vMin);
	static float fScale = 1000.f;
	((float *)vFrom)[m_iDirection] += fScale * ((float *)vDiag)[m_iDirection];	// offset in current slice direction
	return DrawLOD(vFrom, vFrom, TREE_DRAWFULLGEOMETRY);
}

//////////////////////////////////////////////////////////////////////
// Cleanup after drawing into slice textures
//
HRESULT CTree::End()
{
	return CSliceObject::End();
}

//////////////////////////////////////////////////////////////////////
// Used by SortBranches as an ordering function to sort models from
// far to near.
static int __cdecl CompareDist2(const void *arg1, const void *arg2 )
{
    float f1 = *(float *)arg1;
    float f2 = *(float *)arg2;
    if (f1 > f2) 
        return -1;
    else if (f1 < f2) 
        return 1;
    else
        return 0;
}

//////////////////////////////////////////////////////////////////////
// Sort the instances by distance from the eye
//
HRESULT CTree::SortBranches(const D3DXVECTOR3 &vFrom)
{
	for (UINT iBranch = 0; iBranch < m_nBranch; iBranch++)
	{
		D3DXVECTOR3 vCenter = 0.5f * (m_rBranch[iBranch].m_vMin + m_rBranch[iBranch].m_vMax);
		D3DXVECTOR3 vDiff = vCenter - vFrom;
		m_rBranch[iBranch].m_fDistance2 = D3DXVec3LengthSq(&vDiff);
	}
    qsort( (void *)m_rBranch, m_nBranch, sizeof(Branch), &CompareDist2 );
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// draw the trunk geometry
//
HRESULT CTree::DrawTrunk(const D3DXVECTOR3 &vFrom)
{
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	static DWORD dwFlags = XBMESH_NOFVF;
	return m_pMeshTrunk->Render( g_pd3dDevice, dwFlags);
}

//////////////////////////////////////////////////////////////////////
// draw the branch geometry
//
HRESULT CTree::DrawBranches(const D3DXVECTOR3 &vFrom)
{
	HRESULT hr = S_OK;
	extern D3DLIGHT8 g_d3dLight;
	D3DXMATRIX matWorldSaved;
	g_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorldSaved);
	m_TreeBranch.Begin(true);
	for (UINT iBranch = 0; iBranch < m_nBranch; iBranch++)
	{
		D3DXMATRIX matWorld = m_rBranch[iBranch].m_matXfm * matWorldSaved;
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld);
		D3DXVECTOR3 vFrom2;
		D3DXVec3TransformCoord(&vFrom2, &vFrom, &m_rBranch[iBranch].m_matXfmInv);
	
		// put global light into local coords
		D3DLIGHT8 light = g_d3dLight;
		D3DXVECTOR3 vDirection(light.Direction);
		D3DXVec3TransformNormal((D3DXVECTOR3 *)&light.Direction, &vDirection, &m_rBranch[iBranch].m_matXfmInv);
		D3DXVECTOR3 vPosition(light.Position);
		D3DXVec3TransformCoord((D3DXVECTOR3 *)&light.Position, &vPosition, &m_rBranch[iBranch].m_matXfmInv);
		g_pd3dDevice->SetLight(0, &light);
	
		SetupVertexShader();
		
		hr = m_TreeBranch.DrawLOD(vFrom2, vFrom2, TREEBRANCH_DRAWFULLGEOMETRY);
		if (FAILED(hr))
			break;
	}
	m_TreeBranch.End();
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldSaved);
	g_pd3dDevice->SetLight(0, &g_d3dLight);
	return hr;
}

//////////////////////////////////////////////////////////////////////
// draw the whole tree as slices, or the trunk with slice branches, or
// the full geometry
//
HRESULT CTree::DrawLOD(const D3DXVECTOR3 &vFromFade, const D3DXVECTOR3 &vFromSliceOrder, DWORD dwFlags)
{
	HRESULT hr;
	if (m_pMeshTrunk == NULL)
		return E_FAIL;	// not initialized properly
	
	if (dwFlags == 0)	// draw the whole tree as slices
		return CSliceObject::DrawSlices(vFromFade, vFromSliceOrder, 0);

	// draw the tree trunk
	if (dwFlags & TREE_DRAWTRUNK)
	{
		hr = DrawTrunk(vFromFade);
		if (FAILED(hr))
			return hr;
	}

	// save current transformation
	if (dwFlags & TREE_DRAWBRANCHES)	// TODO: if the branches have opaque limbs, we will need to render the opaque subset here
	{
		// draw branch geometry
		hr = DrawBranches(vFromFade);
		if (FAILED(hr))
			return hr;
	}
	else
	{
		// draw branches as slice textures
		D3DXMATRIX matWorldSaved;
		g_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorldSaved);
		SortBranches(vFromSliceOrder);	// sort branches by distance from local eye
		m_TreeBranch.BeginDrawCubeSlices();
		g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		for (UINT iBranch = 0; iBranch < m_nBranch; iBranch++)
		{
			D3DXMATRIX matWorld = m_rBranch[iBranch].m_matXfm * matWorldSaved;
			g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld);
			D3DXVECTOR3 vFromFade2;
			D3DXVec3TransformCoord(&vFromFade2, &vFromFade, &m_rBranch[iBranch].m_matXfmInv);
			hr = m_TreeBranch.SetCubeFade(vFromFade2, 0);
			if (FAILED(hr))
				break;
			D3DXVECTOR3 vFromSliceOrder2;
			D3DXVec3TransformCoord(&vFromSliceOrder2, &vFromSliceOrder, &m_rBranch[iBranch].m_matXfmInv);
			hr = m_TreeBranch.DrawCubeSlices(vFromSliceOrder2);
			if (FAILED(hr))
				break;
		}
		m_TreeBranch.EndDrawCubeSlices();
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldSaved);
	}

	return hr;
}
