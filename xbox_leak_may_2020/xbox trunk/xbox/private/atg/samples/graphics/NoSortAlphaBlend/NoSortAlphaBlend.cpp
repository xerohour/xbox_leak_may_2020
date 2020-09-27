//-----------------------------------------------------------------------------
// File: NoSortAlphaBlend.cpp
//
// Desc: Example showinging how get correct alpha blending without sorting.
//       Uses depth peeling to seperate the alpha into layers.
//
// Hist: 07.16.01 - New for August XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBResource.h>
#include <XBUtil.h>
#include <assert.h>




// For the farthest layer of alpha, blend then write z values in seperate passes
// so that nearer alpha values won't prevent farther alpha values from being
// blended into the scene (even though they are blended incorrectly).
#define FARTHEST_LAYER_SEPERATE_Z 1

// Maximum number of layers to blend.
const int MAX_LAYERS = 6;




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
	{ XBHELP_LEFTSTICK,   XBHELP_PLACEMENT_1, L"Move camera" },
	{ XBHELP_A_BUTTON,	  XBHELP_PLACEMENT_2, L"Toggle Depth\nPeeling" },
	{ XBHELP_B_BUTTON,	  XBHELP_PLACEMENT_1, L"Toggle Z\nReplace" },
	{ XBHELP_X_BUTTON,	  XBHELP_PLACEMENT_2, L"Decrease #\nof Layers" },
	{ XBHELP_Y_BUTTON,	  XBHELP_PLACEMENT_2, L"Increase #\nof Layers" },
	{ XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_1, L"Display help" },
	{ XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Show Previous\nLayer" },
	{ XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_1, L"Show Next\nLayer" },
};

#define NUM_HELP_CALLOUTS (sizeof(g_HelpCallouts)/sizeof(g_HelpCallouts[0]))




//-----------------------------------------------------------------------------
// Name: class CXBAlphaMesh
// Desc: Extension to mesh that sets constant zero with diffuse material value.
//-----------------------------------------------------------------------------
class CXBAlphaMesh : public CXBMesh
{
public:
	BOOL RenderCallback( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwSubset,
                         XBMESH_SUBSET* pSubset, DWORD dwFlags )
	{
		pd3dDevice->SetPixelShaderConstant( 0, &pSubset->mtrl.Diffuse, 1 );

		return TRUE;
	}
	
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//		 from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
	CXBPackedResource  m_xprResource;		 // Packed resources for the app
	CXBFont 		   m_Font;				 // Font class
	CXBHelp 		   m_Help;				 // Help class
	bool			   m_bDrawHelp; 		 // Whether to draw help

	bool               m_bUseDepthPeeling;	 // Use depth peeling for alpha
	int				   m_iNumLayers;		 // Number of unique layers of alpha

	bool               m_bUseDotZW;			 // Use z replace to eliminate artifacts
	
	int				   m_iDrawLayer;		 // Limit drawing to one layer.

	CXBMesh 		   m_Room;				 // XBG file object to render
	CXBAlphaMesh       m_AlphaVolume; 		 // Alpha volume

	D3DXVECTOR3 	   m_vEye;
	D3DXMATRIX		   m_matWorld;
	D3DXMATRIX		   m_matView;
	D3DXMATRIX		   m_matProjection;

	DWORD			   m_dwAlphaVertexShader;
	D3DPIXELSHADERDEF  m_PeelPixelShader;
	D3DPIXELSHADERDEF  m_ConstantPixelShader;

    IDirect3DTexture8 *m_pDepthBuffer[MAX_LAYERS-1];

    IDirect3DSurface8 *m_pColorBuffer;
    IDirect3DSurface8 *m_pZBuffer;

	D3DPIXELSHADERDEF  m_ZRPeelPixelShader;
	D3DPIXELSHADERDEF  m_ZRConstantPixelShader;
    IDirect3DTexture8 *m_pDummyTexture;

	HRESULT RenderScene();

public:
	HRESULT Initialize();
	HRESULT Render();
	HRESULT FrameMove();

	HRESULT SetupPixelShaders();
	HRESULT CreateBuffers();

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
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
			:CXBApplication()
{
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	m_iNumLayers = 4;
	m_bUseDepthPeeling = true;

	m_bDrawHelp = false;

	m_bUseDotZW = false;

	m_iDrawLayer = -1;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
	// Create the font
	if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
		return XBAPPERR_MEDIANOTFOUND;

	// Create help
	if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
		return XBAPPERR_MEDIANOTFOUND;

	// Load the main file object
	if( FAILED( m_Room.Create( m_pd3dDevice, "Models\\Room2.xbg", &m_xprResource ) ) )
		return XBAPPERR_MEDIANOTFOUND;

	// Load the main file object
	if( FAILED( m_AlphaVolume.Create( m_pd3dDevice, "Models\\alphaobjs.xbg", &m_xprResource ) ) )
		return XBAPPERR_MEDIANOTFOUND;

	// Create vertex shader.
    DWORD vdecl[] =
    {
        D3DVSD_STREAM(0),
        D3DVSD_REG(0, D3DVSDT_FLOAT3), // v0 = XYZ
        D3DVSD_REG(1, D3DVSDT_FLOAT3), // v1 = normals
        D3DVSD_END()
    };

    if ( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\AlphaShader.xvu", vdecl, &m_dwAlphaVertexShader ) ) )
        return E_FAIL;

	// Create pixel shader for alpha drawing.
	SetupPixelShaders();

	// Create color buffers and z buffers.
	CreateBuffers();

	// Dummy texture for z-replace.
    m_pd3dDevice->CreateTexture( 1, 1, 1, 0, D3DFMT_V16U16, 0, &m_pDummyTexture );

    // Get the original color and z-buffer.
    m_pd3dDevice->GetRenderTarget( &m_pColorBuffer );
    m_pd3dDevice->GetDepthStencilSurface( &m_pZBuffer );

	// Set the matrices
	D3DXMatrixIdentity( &m_matWorld );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

	m_vEye = D3DXVECTOR3( 10.0f, 1.0f, 0.0f );
	D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

	D3DXMatrixLookAtLH( &m_matView, &m_vEye, &vAt, &vUp );
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

	D3DXMatrixPerspectiveFovLH( &m_matProjection, D3DX_PI/3, 4.0f/3.0f, 0.1f, 100.0f );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjection );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//		 the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	D3DXMATRIX matView, matRotate;

	// Toggle help
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
		m_bDrawHelp = !m_bDrawHelp;

	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
	{
		m_bUseDepthPeeling = !m_bUseDepthPeeling;
	}

	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
	{
		m_bUseDotZW = !m_bUseDotZW;
	}


	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
	{
		m_iNumLayers++;

		if( m_iNumLayers > MAX_LAYERS )
			m_iNumLayers = MAX_LAYERS;
	}

	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
	{
		m_iNumLayers--;

		if( m_iNumLayers < 2 )
			m_iNumLayers = 2;
	}

	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] )
	{
		m_iDrawLayer++;

		if( m_iDrawLayer > m_iNumLayers-1 )
			m_iDrawLayer = m_iNumLayers-1;
	}

	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] )
	{
		m_iDrawLayer--;

		if( m_iDrawLayer < -1 )
			m_iDrawLayer = -1;
	}

	D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

	// Rotate eye around up axis.
	D3DXMatrixRotationAxis( &matRotate, &vUp, 
										m_DefaultGamepad.fX1*m_fElapsedTime );
	D3DXVec3TransformCoord( &m_vEye, &m_vEye, &matRotate );

	// Rotate eye points around side axis.
	D3DXVECTOR3 vView = (m_vEye - vAt);
	D3DXVec3Normalize( &vView, &vView );

	// Place limits so we dont go over the top or under the bottom.
	FLOAT dot = D3DXVec3Dot( &vView, &vUp );
	if( (dot > 0.0f || m_DefaultGamepad.fY1 < 0.0f) && (dot < 0.99f || m_DefaultGamepad.fY1 > 0.0f) )
	{
		D3DXVECTOR3 axis;
		D3DXVec3Cross( &axis, &vUp, &vView );
		D3DXMatrixRotationAxis( &matRotate, &axis, m_DefaultGamepad.fY1*m_fElapsedTime );
		D3DXVec3TransformCoord( &m_vEye, &m_vEye, &matRotate );
	}

	D3DXMatrixLookAtLH( &m_matView, &m_vEye, &vAt, &vUp );

	m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetupPixelShader()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::SetupPixelShaders()
{
	//
	// Setup the pixel shader for constant alpha with depth peeling.
	//
	// a = c0.a
	// rgb = c0.rgb
	//
	D3DPIXELSHADERDEF psd;
	ZeroMemory( &psd, sizeof(psd) );
	psd.PSCombinerCount = PS_COMBINERCOUNT( 1, PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);

	psd.PSTextureModes	= PS_TEXTUREMODES( PS_TEXTUREMODES_PROJECT3D, PS_TEXTUREMODES_NONE, 
										   PS_TEXTUREMODES_NONE, PS_TEXTUREMODES_NONE );

	psd.PSC0Mapping = 0xfffffff0;
	psd.PSC1Mapping = 0xffffffff;
	psd.PSFinalCombinerConstants = 0x000000ff;

	//------------- Stage 0 -------------
	// r0.rgb = c0.rgb
	psd.PSRGBInputs[0]	  = PS_COMBINERINPUTS( PS_REGISTER_C0 | PS_CHANNEL_RGB,
											   PS_REGISTER_ONE | PS_CHANNEL_RGB,
											   PS_REGISTER_ZERO | PS_CHANNEL_RGB,
											   PS_REGISTER_ZERO | PS_CHANNEL_RGB );

	psd.PSRGBOutputs[0]   = PS_COMBINEROUTPUTS( PS_REGISTER_R0,
												PS_REGISTER_DISCARD,
												PS_REGISTER_DISCARD,
												0 );

	// r0.a = c0.a
	psd.PSAlphaInputs[0]  = PS_COMBINERINPUTS( PS_REGISTER_C0 | PS_CHANNEL_ALPHA,
											   PS_REGISTER_ONE | PS_CHANNEL_ALPHA,
											   PS_REGISTER_ZERO | PS_CHANNEL_ALPHA,
											   PS_REGISTER_ZERO | PS_CHANNEL_ALPHA );

	psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS( PS_REGISTER_R0,
												PS_REGISTER_DISCARD,
												PS_REGISTER_DISCARD,
												0 );

	psd.PSConstant0[0] = 0xffffffff;


	//------------- Final combiner -------------
	psd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS( PS_REGISTER_ZERO	| PS_CHANNEL_RGB,
													   PS_REGISTER_ZERO | PS_CHANNEL_RGB,
													   PS_REGISTER_ZERO	| PS_CHANNEL_RGB,
													   PS_REGISTER_R0   | PS_CHANNEL_RGB );

	psd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS( PS_REGISTER_ZERO | PS_CHANNEL_RGB,
													  PS_REGISTER_ZERO | PS_CHANNEL_RGB,
													  PS_REGISTER_R0   | PS_CHANNEL_ALPHA,
													  0 );

	memcpy( &m_PeelPixelShader, &psd, sizeof(D3DPIXELSHADERDEF) );

	// Make version that uses z-replace.
	psd.PSTextureModes	= PS_TEXTUREMODES( PS_TEXTUREMODES_PROJECT3D, PS_TEXTUREMODES_DOTPRODUCT,
										   PS_TEXTUREMODES_DOT_ZW, PS_TEXTUREMODES_NONE );

    psd.PSInputTexture  = PS_INPUTTEXTURE( 0, 0, 0, 0 );

    psd.PSDotMapping    = PS_DOTMAPPING( 0, PS_DOTMAPPING_HILO_1, PS_DOTMAPPING_HILO_1, 0 );

	memcpy( &m_ZRPeelPixelShader, &psd, sizeof(D3DPIXELSHADERDEF) );
	
	//
	// Setup the pixel shader for constant alpha.
	//
	// a = c0.a
	// rgb = c0.rgb
	//
	ZeroMemory( &psd, sizeof(psd) );
	psd.PSCombinerCount = PS_COMBINERCOUNT( 1, PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);

	psd.PSTextureModes	= PS_TEXTUREMODES( PS_TEXTUREMODES_NONE, PS_TEXTUREMODES_NONE, 
										   PS_TEXTUREMODES_NONE, PS_TEXTUREMODES_NONE );

	psd.PSC0Mapping = 0xfffffff0;
	psd.PSC1Mapping = 0xffffffff;
	psd.PSFinalCombinerConstants = 0x000000ff;

	//------------- Stage 0 -------------
	// r0.rgb = c0.rgb
	psd.PSRGBInputs[0]	  = PS_COMBINERINPUTS( PS_REGISTER_C0 | PS_CHANNEL_RGB,
											   PS_REGISTER_ONE | PS_CHANNEL_RGB,
											   PS_REGISTER_ZERO | PS_CHANNEL_RGB,
											   PS_REGISTER_ZERO | PS_CHANNEL_RGB );

	psd.PSRGBOutputs[0]   = PS_COMBINEROUTPUTS( PS_REGISTER_R0,
												PS_REGISTER_DISCARD,
												PS_REGISTER_DISCARD,
												0 );

	// r0.a = c0.a
	psd.PSAlphaInputs[0]  = PS_COMBINERINPUTS( PS_REGISTER_C0 | PS_CHANNEL_ALPHA,
											   PS_REGISTER_ONE | PS_CHANNEL_ALPHA,
											   PS_REGISTER_ZERO | PS_CHANNEL_ALPHA,
											   PS_REGISTER_ZERO | PS_CHANNEL_ALPHA );

	psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS( PS_REGISTER_R0,
												PS_REGISTER_DISCARD,
												PS_REGISTER_DISCARD,
												0 );

	psd.PSConstant0[0] = 0xffffffff;

	//------------- Final combiner -------------
	psd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS( PS_REGISTER_ZERO	| PS_CHANNEL_RGB,
													   PS_REGISTER_ZERO | PS_CHANNEL_RGB,
													   PS_REGISTER_ZERO	| PS_CHANNEL_RGB,
													   PS_REGISTER_R0   | PS_CHANNEL_RGB );

	psd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS( PS_REGISTER_ZERO | PS_CHANNEL_RGB,
													  PS_REGISTER_ZERO | PS_CHANNEL_RGB,
													  PS_REGISTER_R0   | PS_CHANNEL_ALPHA,
													  0 );

	memcpy( &m_ConstantPixelShader, &psd, sizeof(D3DPIXELSHADERDEF) );

	// Make version that uses z-replace.
	psd.PSTextureModes	= PS_TEXTUREMODES( PS_TEXTUREMODES_PROJECT3D, PS_TEXTUREMODES_DOTPRODUCT,
										   PS_TEXTUREMODES_DOT_ZW, PS_TEXTUREMODES_NONE );

    psd.PSInputTexture  = PS_INPUTTEXTURE( 0, 0, 0, 0 );


    psd.PSDotMapping    = PS_DOTMAPPING( 0, PS_DOTMAPPING_HILO_1, PS_DOTMAPPING_HILO_1, 0 );

	memcpy( &m_ZRConstantPixelShader, &psd, sizeof(D3DPIXELSHADERDEF) );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateBuffers()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateBuffers()
{
	// Create and tile z buffers.
	for( int i = 0; i < MAX_LAYERS-1; i++ )
	{
		m_pd3dDevice->CreateTexture( 640, 480, 1, D3DUSAGE_RENDERTARGET, D3DFMT_LIN_D16, 0, &m_pDepthBuffer[i] );
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderScene()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderScene()
{
	// Set up the light
	D3DLIGHT8 light;
	XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.7071067f, 0.7071067f, 0.0f );
	m_pd3dDevice->SetLight( 0, &light );
	m_pd3dDevice->LightEnable( 0, TRUE );

	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00555555 );

	// Draw the room.
	m_Room.Render( m_pd3dDevice );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
	double tot_elapsed = 0.0f;
	double num_counts = 0.0f;

	// Clear the viewport, zbuffer, and stencil buffer
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
						 0xff1f1f7f, 1.0f, 0L );

	// Set up misc render states
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	// Note: Z less is necessary.
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESS );

	// Render the opaque parts of the scene normally.
	RenderScene();

	for( int i = 0; i < 4; i++ )
		m_pd3dDevice->SetTexture( i, 0 );

	if( m_bUseDepthPeeling )
	{
		// Now setup to do depth peeling.
		m_pd3dDevice->SetVertexShader( m_dwAlphaVertexShader );

		if( m_bUseDotZW )
			m_pd3dDevice->SetPixelShaderProgram( &m_ZRConstantPixelShader );
		else
			m_pd3dDevice->SetPixelShaderProgram( &m_ConstantPixelShader );

		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );

		// Calculate and set composite matrix and texture matrix.
		D3DXMATRIX matComposite;
		D3DXMatrixMultiply( &matComposite, &m_matWorld, &m_matView );
		D3DXMatrixMultiply( &matComposite, &matComposite, &m_matProjection );
		D3DXMatrixTranspose( &matComposite, &matComposite );
		m_pd3dDevice->SetVertexShaderConstant( 0, &matComposite, 4 );

		D3DXVECTOR4 v4Scale(640.0f * 0.5f, -480.0f * 0.5f, float(D3DZ_MAX_D16), 1.0f);
		D3DXVECTOR4 v4Offset(640.0f * 0.5f + 0.5f, 480.0f * 0.5f + 0.5f, float(-0.00002f * D3DZ_MAX_D16), 0.0f);
		m_pd3dDevice->SetVertexShaderConstant( 4, &v4Scale, 1 );
		m_pd3dDevice->SetVertexShaderConstant( 5, &v4Offset, 1 );

		// Alpha objects are double sided.
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

		// Disable color writes.
		m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0 );

		if( m_bUseDotZW )
		{
			float fConstants[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			m_pd3dDevice->SetVertexShaderConstant( 6, fConstants, 1 );

			m_pd3dDevice->SetTexture( 0, m_pDepthBuffer[0] );
			m_pd3dDevice->SetTexture( 2, m_pDummyTexture );
		}

		// Compute z values for first n-1 layers.
		for( int i = 0; i < m_iNumLayers-1; i++ )
		{
			// Switch to z-buffer for this layer.
			IDirect3DSurface8* pDepthSurf;
			m_pDepthBuffer[i]->GetSurfaceLevel( 0, &pDepthSurf );
			m_pd3dDevice->SetRenderTarget( NULL, pDepthSurf );
			pDepthSurf->Release();

			// Clear the z buffer.
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0, 1.0f, 0L );

			// Only use shadow texture to peel off layers after the first pass.
			if (i > 0)
			{
				// Use previous buffer as shadowbuffer texture.
				m_pd3dDevice->SetTexture( 0, m_pDepthBuffer[i-1] );

				// Keep pixels where (Buffer Z < Coordinate Z)
				m_pd3dDevice->SetRenderState( D3DRS_SHADOWFUNC, D3DCMP_LESS );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAKILL, D3DTALPHAKILL_ENABLE );

				if( m_bUseDotZW )
					m_pd3dDevice->SetPixelShaderProgram( &m_ZRPeelPixelShader );
				else
					m_pd3dDevice->SetPixelShaderProgram( &m_PeelPixelShader );
			}

			// Draw any alpha blended parts of the scene (roughly front to back).
			m_AlphaVolume.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOTEXTURES | XBMESH_NOMATERIALS );
		}

		// Turn on alpha blending.
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		// Enable color writes.
		m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL );

		// Restore regular z buffer.
		m_pd3dDevice->SetRenderTarget( NULL, m_pZBuffer );

		if( m_bUseDotZW )
		{
			// Disable z writes.
			m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

			// Make sure alpha kill is on.
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAKILL, D3DTALPHAKILL_ENABLE );

			// and shader is selected.
			m_pd3dDevice->SetPixelShaderProgram( &m_PeelPixelShader );

			// Blend in alpha values back to front.
			for( int i = m_iNumLayers-1; i >= 0; i-- )
			{
				if( i == m_iNumLayers-1 )
				{
					// Switch to depth texture for this layer.
					m_pd3dDevice->SetTexture( 0, m_pDepthBuffer[i-1] );

					// Keep pixels where (Buffer Z < Coordinate Z)
					m_pd3dDevice->SetRenderState( D3DRS_SHADOWFUNC, D3DCMP_LESS );
				}
				else
				{	
					// Switch to depth texture for this layer.
					m_pd3dDevice->SetTexture( 0, m_pDepthBuffer[i] );

					// Keep pixels where (Buffer Z == Coordinate Z)
					m_pd3dDevice->SetRenderState( D3DRS_SHADOWFUNC, D3DCMP_EQUAL );
				}

				if( m_iDrawLayer != -1 )
				{
					// Disable drawing to all but one layer for debugging.
					if( i != m_iDrawLayer )
						m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0 );
					else
						m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL );
				}

				// Draw any alpha blended parts of the scene (roughly back to front).
				m_AlphaVolume.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOTEXTURES | XBMESH_NOMATERIALS );
			}
		}
		else
		{
			// Blend in alpha values back to front.
			for( int i = m_iNumLayers-1; i >= 0; i-- )
			{
				if( i > 0 )
				{
					// Change to the appropriate depth texture.
					m_pd3dDevice->SetTexture( 0, m_pDepthBuffer[i-1] );

					// Keep any pixels where (Buffer Z < Coordinate Z)
					m_pd3dDevice->SetRenderState( D3DRS_SHADOWFUNC, D3DCMP_LESS );
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAKILL, D3DTALPHAKILL_ENABLE );

					m_pd3dDevice->SetPixelShaderProgram( &m_PeelPixelShader );
				}
				else
				{	
					// Switch off depth peeling.
					m_pd3dDevice->SetPixelShaderProgram( &m_ConstantPixelShader );
					m_pd3dDevice->SetTexture( 0, NULL );
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
				}
				
				if( m_iDrawLayer != -1 )
				{
					// Disable drawing to all but one layer for debugging.
					if( i != m_iDrawLayer )
						m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0 );
					else
						m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL );
				}

#if (FARTHEST_LAYER_SEPERATE_Z)
				if( i == m_iNumLayers-1 )
				{
					// Blend color values.
					m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

					// Draw any alpha blended parts of the scene (roughly back to front).
					m_AlphaVolume.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOTEXTURES | XBMESH_NOMATERIALS );

					// Then write z values.
					m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
					m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0 );

					// Draw any alpha blended parts of the scene.
					m_AlphaVolume.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOTEXTURES | XBMESH_NOMATERIALS );

					m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL );
				}
				else
				{
					// Draw any alpha blended parts of the scene (roughly back to front).
					m_AlphaVolume.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOTEXTURES | XBMESH_NOMATERIALS );
				}
#else
				// Draw any alpha blended parts of the scene (roughly back to front).
				m_AlphaVolume.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOTEXTURES | XBMESH_NOMATERIALS );
#endif
			}
		}

		if (m_iDrawLayer != -1)
			m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL );
	}
	else
	{
		// Use constant alpha blend.
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

		// Turn on alpha blending.
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		// Use our constant alpha pixel shader.
		m_pd3dDevice->SetPixelShaderProgram( &m_ConstantPixelShader );

		// Draw any alpha blended parts of the scene.
		m_AlphaVolume.Render( m_pd3dDevice, XBMESH_NOTEXTURES | XBMESH_NOMATERIALS );
	}	

	// Restore state.
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetPixelShader( NULL );

	// Show title, frame rate, and help
	if( m_bDrawHelp )
	{
		m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
	}
	else
	{
		if( m_bUseDepthPeeling )
		{
			if( m_iDrawLayer != -1 )
			{
				WCHAR buf[80];
				swprintf( buf, L"Layer = %d", m_iDrawLayer+1 );
				m_Font.DrawText( 64, 70, 0xffffffff, buf );
			}
			else
			{
				WCHAR buf[80];
				swprintf( buf, L"NumLayers = %d", m_iNumLayers );
				m_Font.DrawText( 64, 70, 0xffffffff, buf );
			}

			if( m_bUseDotZW )
			{
				m_Font.DrawText( 280, 70, 0xffffffff, L"Depth Replace" );
			}
		}
		else
		{
			m_Font.DrawText( 64, 70, 0xffffffff, L"Incorrect Blending" );
		}

		// Show frame rate
		m_Font.DrawText(  64, 50, 0xffffffff, L"NoSortAlphaBlend" );
		m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
	}

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}
