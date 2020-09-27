//-----------------------------------------------------------------------------
// File: GameMenu.cpp
//
// Desc: Code for in-game menus
//
// Copyright (C) Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xgraphics.h>
#include <XBUtil.h>
#include "GameMenu.h"




//-----------------------------------------------------------------------------
// Name: CMenuItem()
// Desc: 
//-----------------------------------------------------------------------------
CMenuItem::CMenuItem( LPDIRECT3DTEXTURE8 pTexture, DWORD dwID )
{
	m_pTexture       = pTexture;
	m_dwID           = dwID;
	m_pParent        = NULL;
	m_dwNumChildren  = 0L;
	m_dwSelectedMenu = 0L;
}




//-----------------------------------------------------------------------------
// Name: ~CMenuItem()
// Desc: 
//-----------------------------------------------------------------------------
CMenuItem::~CMenuItem()
{
	SAFE_RELEASE( m_pTexture );

	while( m_dwNumChildren )
		delete m_pChild[--m_dwNumChildren];
}




//-----------------------------------------------------------------------------
// Name: Add()
// Desc: 
//-----------------------------------------------------------------------------
CMenuItem* CMenuItem::Add( CMenuItem* pNewChild )
{
	m_pChild[m_dwNumChildren++] = pNewChild;
	pNewChild->m_pParent        = this;

	return pNewChild;
}



//-----------------------------------------------------------------------------
// Name: RenderLabel()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CMenuItem::RenderLabel( LPDIRECT3DDEVICE8 pd3dDevice )
{
	// Get texture dimensions
	D3DSURFACE_DESC desc;
	m_pTexture->GetLevelDesc( 0, &desc );
	FLOAT fWidth  = (FLOAT)desc.Width;
	FLOAT fHeight = (FLOAT)desc.Height;

	struct VERTEX { D3DXVECTOR3 p; FLOAT tu, tv; } v[4];
	v[0].p = D3DXVECTOR3( -fWidth/2, +fHeight/2, 0.0f ); v[0].tu = 0.0f; v[0].tv = 0.0f; 
	v[1].p = D3DXVECTOR3( +fWidth/2, +fHeight/2, 0.0f ); v[1].tu = 1.0f; v[1].tv = 0.0f; 
	v[2].p = D3DXVECTOR3( -fWidth/2, -fHeight/2, 0.0f ); v[2].tu = 0.0f; v[2].tv = 1.0f; 
	v[3].p = D3DXVECTOR3( +fWidth/2, -fHeight/2, 0.0f ); v[3].tu = 1.0f; v[3].tv = 1.0f; 

	// Scale tex coordinates for linear textures
	if( FALSE == XGIsSwizzledFormat( desc.Format ) )
	{
		v[0].tu *= fWidth;   v[0].tv *= fHeight; 
		v[1].tu *= fWidth;   v[1].tv *= fHeight; 
		v[2].tu *= fWidth;   v[2].tv *= fHeight; 
		v[3].tu *= fWidth;   v[3].tv *= fHeight; 
	}
	
	// Render the menuitem's label
	pd3dDevice->SetTexture( 0, m_pTexture );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetVertexShader( D3DFVF_XYZ|D3DFVF_TEX1 );
	pd3dDevice->DrawPrimitiveUP( D3DPT_QUADSTRIP, 1, v, sizeof(VERTEX) );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CMenuItem::Render( LPDIRECT3DDEVICE8 pd3dDevice )
{
	// Save current matrices
	D3DXMATRIX matViewSaved, matProjSaved;
	pd3dDevice->GetTransform( D3DTS_VIEW,       &matViewSaved );
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProjSaved );


	FLOAT z = 320.0f / tanf(D3DX_PI/8);

	// Setup new view and proj matrices for head-on viewing
	D3DXMATRIX matView, matProj;
	D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 0.0f,-30.0f ),
								  &D3DXVECTOR3( 0.0f, 0.0f,  0.0f ),
				 				  &D3DXVECTOR3( 0.0f, 1.0f,  0.0f ) );
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
	pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// Establish colors for selected vs. normal menu items
	D3DMATERIAL8 mtrlNormal, mtrlSelected, mtrlTitle;
	XBUtil_InitMaterial( mtrlTitle,    1.0f, 0.0f, 0.0f, 1.0f );
	XBUtil_InitMaterial( mtrlNormal,   1.0f, 1.0f, 1.0f, 0.5f );
	XBUtil_InitMaterial( mtrlSelected, 1.0f, 1.0f, 0.0f, 1.0f );

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0xffffffff );

	// Translate the menuitem into place
	D3DXMATRIX matWorld;
	D3DXMatrixScaling( &matWorld, 20.0f/320.0f, 20.0f/240.0f, 20.0f/320.0f );
	matWorld._42 = (m_dwNumChildren*1.0f) + 2.0f;
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	pd3dDevice->SetMaterial( &mtrlTitle );

	// Render the menuitem's label
	RenderLabel( pd3dDevice );

	// Loop through and render all menuitem lables
	for( DWORD i=0; i<m_dwNumChildren; i++ )
	{
		D3DXMATRIX matWorld;
		D3DXMatrixScaling( &matWorld, 15.0f/320.0f, 15.0f/240.0f, 15.0f/320.0f );
		pd3dDevice->SetMaterial( &mtrlNormal );

		// Give a different effect for selected items
		if( m_dwSelectedMenu == i )
		{
			D3DXMATRIX matRotate;
			D3DXMatrixRotationY( &matRotate, (D3DX_PI/3)*sinf(timeGetTime()/200.0f) );
			D3DXMatrixMultiply( &matWorld, &matWorld, &matRotate );
			pd3dDevice->SetMaterial( &mtrlSelected );
		}

		// Translate the menuitem into place
		matWorld._42 = (m_dwNumChildren*1.0f) - (i*2.0f);
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// Render the menuitem's label
		m_pChild[i]->RenderLabel( pd3dDevice );
	}

	// Restore matrices
	pd3dDevice->SetTransform( D3DTS_VIEW,       &matViewSaved );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjSaved );

	return S_OK;
}
