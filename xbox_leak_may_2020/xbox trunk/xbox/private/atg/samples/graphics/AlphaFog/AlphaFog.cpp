//-----------------------------------------------------------------------------
// File: AlphaFog.cpp
//
// Desc: Example code showing how to do a type fogging that varies the alpha
//		 value of far away objects. This is useful in scene-culling techniques
//		 to avoid the visible "pop" that occurs when far-away objects (like
//		 trees and buildings) get added to a scene.
//
// Hist: 06.07.01 - New for XFest June, 2001
//		 09.12.01 - Adding to XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBUtil.h>
#include <XBResource.h>
#include "Resource.h"


//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
	{ "Seafloor.bmp", Resource_Seafloor_OFFSET },
	{ "SkyBoxXP.bmp", Resource_SkyBoxXP_OFFSET },
	{ "SkyBoxXN.bmp", Resource_SkyBoxXN_OFFSET },
	{ "SkyBoxYP.bmp", Resource_SkyBoxYP_OFFSET },
	{ "SkyBoxYN.bmp", Resource_SkyBoxYN_OFFSET },
	{ "SkyBoxZP.bmp", Resource_SkyBoxZP_OFFSET },
	{ "SkyBoxZN.bmp", Resource_SkyBoxZN_OFFSET },
	{ NULL, 0 },
};


   

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
	{ XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Change alphafog\nstart" },
	{ XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Change alphafog\nend" },
	{ XBHELP_X_BUTTON,	   XBHELP_PLACEMENT_2, L"Toggle\nskybox" },
	{ XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display\nhelp" },
	{ XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS 5




//-----------------------------------------------------------------------------
// Structures and Macros
//-----------------------------------------------------------------------------
struct FOGVERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	FLOAT		tu, tv;
};

#define NEAR_PLANE	 1.0f
#define FAR_PLANE  250.0f




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//		 functionality we want, so we're just supplying stubs to interface with
//		 the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
	CXBPackedResource  m_xprResource;	   // Packed resources for the app
	CXBFont 		   m_Font;			   // Font class
	CXBHelp 		   m_Help;			   // Help class
	BOOL			   m_bDrawHelp; 	   // Whether to draw help

	// Geometry
	LPDIRECT3DVERTEXBUFFER8 m_pColumnVB;
	DWORD					m_dwNumColumnVertices;

	CXBMesh 		   m_SkyboxObject;
	BOOL			   m_bDrawSkybox;

	CXBMesh 		   m_TerrainObject;
	LPDIRECT3DTEXTURE8 m_pTerrainTexture;

	// Fog parameters
	FLOAT			   m_fFogStartValue;
	FLOAT			   m_fFogEndValue;

	DWORD			   m_dwVertexShader;

	D3DXMATRIX		   m_matWorld;
	D3DXMATRIX		   m_matView;
	D3DXMATRIX		   m_matProj;

	HRESULT GenerateColumn( DWORD dwNumSegments, FLOAT fRadius, FLOAT fHeight );

protected:
	HRESULT Initialize();
	HRESULT Render();
	HRESULT FrameMove();

public:
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
	m_bDrawHelp 		   = FALSE;

	m_fFogStartValue	   = 0.8f*FAR_PLANE;
	m_fFogEndValue		   = FAR_PLANE;

	m_pColumnVB 		   = NULL;
	m_pTerrainTexture	   = NULL;

	m_bDrawSkybox		   = TRUE;
}




//----------------------------------------------------------------------------
// Name: GenerateColumn()
// Desc: Generates a trianglestrip for a column
//----------------------------------------------------------------------------
HRESULT CXBoxSample::GenerateColumn( DWORD dwNumSegments, FLOAT fRadius, FLOAT fHeight )
{
	HRESULT hr;

	m_dwNumColumnVertices = 2 * (dwNumSegments+1);

	// Create a vertex buffer
	hr = m_pd3dDevice->CreateVertexBuffer( m_dwNumColumnVertices*sizeof(FOGVERTEX),
										   D3DUSAGE_WRITEONLY, 0,
										   D3DPOOL_DEFAULT, &m_pColumnVB );
	if( FAILED(hr) )
		return hr;

	FOGVERTEX* pVertices = NULL;
	hr = m_pColumnVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );
	if( FAILED(hr) )
		return hr;

	// Generate a trianglestrip
	for( DWORD seg=0; seg<=dwNumSegments; seg++ )
	{
		FLOAT fTheta = (2*D3DX_PI*seg)/dwNumSegments;
		FLOAT nx	 = sinf(fTheta);
		FLOAT nz	 = cosf(fTheta);
		FLOAT r 	 = fRadius;
		FLOAT u 	 = (1.0f*seg)/dwNumSegments;

		// Add two vertices to the strip at each step
		pVertices->p.x = r*nx;
		pVertices->p.y = fHeight;
		pVertices->p.z = r*nz;
		pVertices->n.x = nx;
		pVertices->n.y = 0;
		pVertices->n.z = nz;
		pVertices->tu  = u;
		pVertices->tv  = 1;
		pVertices++;

		pVertices->p.x = r*nx;
		pVertices->p.y = -1;
		pVertices->p.z = r*nz;
		pVertices->n.x = nx;
		pVertices->n.y = 0;
		pVertices->n.z = nz;
		pVertices->tu  = u;
		pVertices->tv  = 0;
		pVertices++;
	}

	m_pColumnVB->Unlock();

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
	// Create the font
	if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
		return XBAPPERR_MEDIANOTFOUND;

	// Create the help
	if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
		return XBAPPERR_MEDIANOTFOUND;

	// Create the resources
	if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
									  Resource_NUM_RESOURCES, g_ResourceNames ) ) )
		return XBAPPERR_MEDIANOTFOUND;

	// Create the terrain texture
	m_pTerrainTexture = m_xprResource.GetTexture( "SeaFloor.bmp" );

	// Load the skybox
	if( FAILED( m_SkyboxObject.Create( m_pd3dDevice, "Models\\SkyBox.xbg", &m_xprResource ) ) )
		return XBAPPERR_MEDIANOTFOUND;

	// Load the terrain
	if( FAILED( m_TerrainObject.Create( m_pd3dDevice, "Models\\Terrain.xbg", &m_xprResource ) ) )
		return XBAPPERR_MEDIANOTFOUND;

	// Generate some geometry for the app
	GenerateColumn( 30, 1.0f, 10.0f );

	// Set up the object material
	D3DMATERIAL8 mtrl;
	XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
	m_pd3dDevice->SetMaterial( &mtrl );
	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x44444444 );

	// Set the transform matrices
	D3DXMatrixIdentity( &m_matWorld );
	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4.0f, 4.0f/3.0f, NEAR_PLANE, FAR_PLANE );
	m_pd3dDevice->SetTransform( D3DTS_WORLD,	  &m_matWorld );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

	// Create the vertex shader
	DWORD dwVertexDecl[] =
	{
		D3DVSD_STREAM( 0 ),
		D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position of first mesh
		D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), // Normal
		D3DVSD_REG( 6, D3DVSDT_FLOAT2 ), // Tex coords
		D3DVSD_END()
	};

	if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\AlphaFog.xvu",
										   dwVertexDecl,
										   &m_dwVertexShader ) ) )
		return E_FAIL;

	// Set up the light
	D3DLIGHT8 light;
	XBUtil_InitLight( light, D3DLIGHT_POINT, 0.0f, 50.0f, 0.0f );
	light.Attenuation0 =  0.1f;
	light.Range 	   = 200.0f;
	m_pd3dDevice->SetLight( 0, &light );
	m_pd3dDevice->LightEnable( 0, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,	 TRUE );

	// Set misc states
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,	TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//		 the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	// Toggle help
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
		m_bDrawHelp = !m_bDrawHelp;

	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
		m_bDrawSkybox = !m_bDrawSkybox;

	// Update fog parameters from GamePad input
	m_fFogStartValue += m_DefaultGamepad.fY1*m_fElapsedTime*100.0f;
	if( m_fFogStartValue < NEAR_PLANE ) 	 m_fFogStartValue = NEAR_PLANE;
	if( m_fFogStartValue > FAR_PLANE )		 m_fFogStartValue = FAR_PLANE;
	if( m_fFogStartValue > m_fFogEndValue )  m_fFogStartValue = m_fFogEndValue;

	m_fFogEndValue += m_DefaultGamepad.fY2*m_fElapsedTime*100.0f;
	if( m_fFogEndValue < NEAR_PLANE )		 m_fFogEndValue = NEAR_PLANE;
	if( m_fFogEndValue > FAR_PLANE )		 m_fFogEndValue = FAR_PLANE;
	if( m_fFogEndValue < m_fFogStartValue )  m_fFogEndValue = m_fFogStartValue;
	
	// Move the camera along an ellipse
	D3DXVECTOR3 vFrom( 50*sinf(m_fAppTime/2), 3.0f, 60*cosf(m_fAppTime/2) );
	D3DXVECTOR3 vAt( 50*sinf(m_fAppTime/2+1.5f), 2.0f, 60*cosf(m_fAppTime/2+1.5f) );
	D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &m_matView, &vFrom, &vAt, &vUp );
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

	FLOAT g_fLightAngle = 1.0f;
	FLOAT fFogEnd	= m_fFogEndValue;
	FLOAT fFogStart = m_fFogStartValue;

	// Values passed into the vertex shader for lighting, etc.
	D3DXVECTOR4 fConstants0( 0.0f, 0.0f, 0.0f, 0.00f );
	D3DXVECTOR4 fConstants1( 1.0f, 0.5f, 0.2f, 0.05f );
	D3DXVECTOR4 fLight( sinf(g_fLightAngle), cosf(g_fLightAngle), 0.0f, 0.0f );
	D3DXVECTOR4 fDiffuse( 0.5f, 0.5f, 0.5f, 1.00f );
	D3DXVECTOR4 fAmbient( 0.5f, 0.5f, 0.5f, 1.0f );
	D3DXVECTOR4 fAlphaFog( -1.0f/(fFogEnd-fFogStart), fFogEnd/(fFogEnd-fFogStart), 0.0f, 0.0f );

	D3DXMATRIX matWV, matWVP;
	D3DXMatrixMultiply( &matWV, &m_matWorld, &m_matView );
	D3DXMatrixMultiply( &matWVP, &matWV, &m_matProj );
	D3DXMatrixTranspose( &matWV, &matWV );
	D3DXMatrixTranspose( &matWVP, &matWVP );

	// Set the vertex shader constants
	m_pd3dDevice->SetVertexShaderConstant(	0, &fConstants0, 1 ); // Some constants
	m_pd3dDevice->SetVertexShaderConstant(	1, &fConstants1, 1 ); // More constants
	m_pd3dDevice->SetVertexShaderConstant( 20, &matWV,		 4 ); // Transforms
	m_pd3dDevice->SetVertexShaderConstant( 30, &matWVP, 	 4 ); // Transforms
	m_pd3dDevice->SetVertexShaderConstant( 40, &fLight, 	 1 ); // Light direction
	m_pd3dDevice->SetVertexShaderConstant( 41, &fDiffuse,	 1 ); // Diffuse color
	m_pd3dDevice->SetVertexShaderConstant( 42, &fAmbient,	 1 ); // Ambient color
	m_pd3dDevice->SetVertexShaderConstant( 44, &fAlphaFog,	 1 ); // Alpha fog factors

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

	// Set default state
	m_pd3dDevice->SetTexture( 0, NULL );
	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x44444444 );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,		  TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		  TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,	  TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,   FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	 D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	 D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	 D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,	 D3DTOP_DISABLE );

	// Render the Skybox
	if( m_bDrawSkybox )
	{
		D3DXMatrixIdentity( &m_matWorld );
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
	
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		
		// Center view matrix for skybox and disable zbuffer
		D3DXMATRIX matView, matViewSave;
		m_pd3dDevice->GetTransform( D3DTS_VIEW, 	 &matViewSave );
		matView = matViewSave;
		matView._41 = 0.0f; matView._42 = -0.0f; matView._43 = 0.0f;
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

		// Render the skybox
		m_SkyboxObject.Render( m_pd3dDevice );

		// Restore the render states
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &matViewSave );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	}

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

	// Turn on alpha blending
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// Build the matrix transforms
	D3DXMatrixIdentity( &m_matWorld );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

	D3DXMATRIX matWV, matWVP;
	D3DXMatrixMultiply( &matWV, &m_matWorld, &m_matView );
	D3DXMatrixMultiply( &matWVP, &matWV, &m_matProj );
	D3DXMatrixTranspose( &matWV, &matWV );
	D3DXMatrixTranspose( &matWVP, &matWVP );
	m_pd3dDevice->SetVertexShaderConstant( 20, &matWV,	4 ); // WorldView Transform
	m_pd3dDevice->SetVertexShaderConstant( 30, &matWVP, 4 ); // WorldViewProj Transform

	// Render the terrain
	m_pd3dDevice->SetTexture( 0, m_pTerrainTexture );
	m_pd3dDevice->SetVertexShader( m_dwVertexShader );
	m_TerrainObject.Render( m_pd3dDevice, XBMESH_NOFVF|XBMESH_NOTEXTURES );

	// Draw the columns
	for( DWORD i=0; i<100; i++ )
	{
		FLOAT tx = (i%50)*20.0f - 500.0f;
		FLOAT ty =	0.0f;
		FLOAT tz = (i<=50) ? 40.0f : -40.0f;

		if( i<50 )
			D3DXMatrixTranslation( &m_matWorld, (i%50)*20.0f - 500.0f, ty, 10.0f );
		else
			D3DXMatrixTranslation( &m_matWorld, 10.0f, ty, (i%50)*20.0f - 500.0f );

		
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

		D3DXMATRIX matWV, matWVP;
		D3DXMatrixMultiply( &matWV, &m_matWorld, &m_matView );
		D3DXMatrixMultiply( &matWVP, &matWV, &m_matProj );
		D3DXMatrixTranspose( &matWV, &matWV );
		D3DXMatrixTranspose( &matWVP, &matWVP );
		m_pd3dDevice->SetVertexShaderConstant( 20, &matWV,			4 ); // Transforms
		m_pd3dDevice->SetVertexShaderConstant( 30, &matWVP, 		4 ); // Transforms

		m_pd3dDevice->SetVertexShader( m_dwVertexShader );
		m_pd3dDevice->SetStreamSource( 0, m_pColumnVB, sizeof(FOGVERTEX) );
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,
									 0, m_dwNumColumnVertices-2 );
	}

	// Show title, frame rate, and help
	if( m_bDrawHelp )
		m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
	else
	{
		m_Font.Begin();
		m_Font.DrawText(  64,  48, 0xff000000, L"AlphaFog" );
		m_Font.DrawText( 450,  48, 0xff008000, m_strFrameRate );

		WCHAR strBuffer[80];
		swprintf( strBuffer, L"%0.2f", m_fFogStartValue );
		m_Font.DrawText(  64,  75, 0xff000000, L"Start:" );
		m_Font.DrawText( 150,  75, 0xff008000, strBuffer  );
		swprintf( strBuffer, L"%0.2f", m_fFogEndValue );
		m_Font.DrawText(  64, 100, 0xff000000, L"End:" );
		m_Font.DrawText( 150, 100, 0xff008000, strBuffer  );
		m_Font.End();
	}

	// Present the scene
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	return S_OK;
}




