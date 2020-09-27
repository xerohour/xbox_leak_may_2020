//-----------------------------------------------------------------------------
// File: SkinnedMesh.cpp
//
// Desc: 
//
// Hist: 06.27.01 - New
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include "XBSkinnedMesh.h"
#include "Resource.h"


//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "Tiny_Skin.bmp", resource_TinySkin_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource m_xprResource;        // Packed resources for the app
    CXBFont           m_Font;               // Font class

	CXBSkinnedMesh    m_SkinnedMesh;

public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

    CXBoxSample();
};




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: This creates all device-dependant display objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    m_SkinnedMesh.LoadMeshHierarchy( "D:\\Media\\Models\\Tiny.x", &m_xprResource );

    D3DXMATRIX matView, matProj;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 0.0f, m_SkinnedMesh.m_pdeSelected->m_fRadius*2.8f ),
			                      &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
			                      &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f / 480.0f, 
			                    m_SkinnedMesh.m_pdeSelected->m_fRadius / 64, 
								m_SkinnedMesh.m_pdeSelected->m_fRadius * 200 );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    D3DLIGHT8 light;
    ZeroMemory( &light, sizeof(light) );
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Direction = D3DXVECTOR3( 0.0f, 0.0f, -1.0f);
    light.Diffuse.r  = 1.00f;
    light.Diffuse.g  = 1.00f;
    light.Diffuse.b  = 1.00f;
    light.Ambient.r  = 0.25f;
    light.Ambient.g  = 0.25f;
    light.Ambient.b  = 0.25f;
    m_pd3dDevice->SetLight(0, &light );
    m_pd3dDevice->LightEnable(0, TRUE);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	for( SDrawElement* pDrawElement = m_SkinnedMesh.m_pdeHead; pDrawElement; pDrawElement = pDrawElement->m_pNext )
	{
		// Update the times
		pDrawElement->m_fCurrentTime += m_fElapsedTime * 4800;
		if( pDrawElement->m_fCurrentTime > 1.0e15f )
			pDrawElement->m_fCurrentTime = 0.0f;

		SFrame* pFrame = pDrawElement->m_pframeAnimHead;
		while( pFrame )
		{
			pFrame->SetTime( pDrawElement->m_fCurrentTime );
			pFrame = pFrame->m_pframeAnimNext;
		}

		// Set up viewing postion
		static FLOAT angle = -D3DX_PI/4;
		static FLOAT trans = -100.0f;
		D3DXMatrixRotationX( &pDrawElement->m_pRootFrame->m_matRot, angle );
		D3DXMatrixTranslation( &pDrawElement->m_pRootFrame->m_matTrans, 0.0f, trans, 0.0f );
    
		// Update the frames
		D3DXMATRIX matTrans, matCurrent;
		D3DXMatrixTranslation( &matTrans, -pDrawElement->m_vCenter.x, -pDrawElement->m_vCenter.y, -pDrawElement->m_vCenter.z );
		D3DXMatrixIdentity( &matCurrent );

		m_SkinnedMesh.UpdateFrames( pDrawElement->m_pRootFrame, matCurrent );
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
		                 0xff0000ff, 1.0f, 0L );

    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,     TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,      FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

    // Draw the skinned mesh
    m_SkinnedMesh.Render();

    // Show frame rate
    m_Font.DrawText(  64, 50, 0xffffff00, L"SkinnedMesh" );
    m_Font.DrawText( 420, 50, 0xffffff00, m_strFrameRate );
  
    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




