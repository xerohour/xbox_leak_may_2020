//-----------------------------------------------------------------------------
// File: Swizzle.cpp
//
// Desc: Example code showing how to lock and access data from swizzled
//       textures.
//
// Hist: 12.15.00 - New for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBUtil.h>
#include <xgraphics.h>




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Rotate\ntexture" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\nformat" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\nlinear" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, L"Lock every frame" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Toggle use of\nswizzler class" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"Toggle use of\nswizzler API" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display\nhelp" },
};

#define NUM_HELP_CALLOUTS 7




//-----------------------------------------------------------------------------
// Vertex structure
//-----------------------------------------------------------------------------
struct VERTEX 
{ 
    D3DXVECTOR3 p; 
    FLOAT       tu, tv; 
};

#define D3DFVF_VERTEX (D3DFVF_XYZ|D3DFVF_TEX1)




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    // Font
    CXBFont                 m_Font;
    CXBHelp                 m_Help;
    BOOL                    m_bDrawHelp;

    // Geometry
    LPDIRECT3DVERTEXBUFFER8 m_pVB;
    LPDIRECT3DTEXTURE8      m_pTexture;

    // Support for textures
    D3DFORMAT               m_d3dFormat;         // Current texture format
    BOOL                    m_bUpdateEveryFrame; // Whether to update the texture every frame
    BOOL                    m_bUseSwizzlerClass; // Whether to use the swizzler class
    BOOL                    m_bUseSwizzlerAPI;   // Whether to use the swizzler API
	DWORD*                  m_pTextureData;      // Intermediate storage for texture data
    
	HRESULT CreateGeometry();
    HRESULT UpdateTexture();

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
    // Override base members
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Initialize members
    m_bDrawHelp            = FALSE;
    m_pVB                  = NULL;
    m_pTexture             = NULL;
    m_d3dFormat            = D3DFMT_LIN_R5G6B5;
	m_bUpdateEveryFrame    = FALSE;
    m_bUseSwizzlerClass    = FALSE;
    m_bUseSwizzlerAPI      = FALSE;
	m_pTextureData         = NULL;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT hr;

    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the texture and vertex buffer
    if( FAILED( hr = CreateGeometry() ) )
        return hr;

    // Set the transform matrices
    D3DXMATRIX matWorld, matView, matProj;
    D3DXVECTOR3 vFrom( 0.0f, 0.0f, 3.0f );
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vFrom, &vAt, &vUp );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, 4.0f/3.0f, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateGeometry()
// Desc: To test out different lock schemes
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateGeometry()
{
    // Release the existing objects
    SAFE_RELEASE( m_pTexture );
    SAFE_RELEASE( m_pVB );
        
    // Generate some geometry for the app
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 4*5*sizeof(FLOAT), D3DUSAGE_WRITEONLY, 
                                                  0L, D3DPOOL_DEFAULT, &m_pVB ) ) )
        return E_FAIL;

    VERTEX* v;
    m_pVB->Lock( 0, 0, (BYTE**)&v, 0L );
    v[0].p = D3DXVECTOR3(-1.0f,-1.0f, 0.0f ); v[0].tu = 0.0f; v[0].tv = 0.0f;
    v[1].p = D3DXVECTOR3( 1.0f,-1.0f, 0.0f ); v[1].tu = 1.0f; v[1].tv = 0.0f;
    v[2].p = D3DXVECTOR3(-1.0f, 1.0f, 0.0f ); v[2].tu = 0.0f; v[2].tv = 1.0f;
    v[3].p = D3DXVECTOR3( 1.0f, 1.0f, 0.0f ); v[3].tu = 1.0f; v[3].tv = 1.0f;

    // Linear textures use literal textcoords (range from 0 to width or height)
    if( m_d3dFormat == D3DFMT_LIN_R5G6B5 || m_d3dFormat ==D3DFMT_LIN_A8R8G8B8 )
    {
        v[0].tu =   0.0f; v[0].tv =   0.0f;
        v[1].tu = 255.0f; v[1].tv =   0.0f;
        v[2].tu =   0.0f; v[2].tv = 255.0f;
        v[3].tu = 255.0f; v[3].tv = 255.0f;
    }
    
    m_pVB->Unlock();

    // Create the texture
    if( FAILED( m_pd3dDevice->CreateTexture( 256, 256, 0, 0L, m_d3dFormat, 
                                             D3DPOOL_DEFAULT, &m_pTexture ) ) )
        return E_FAIL;

	// Fill the newly created texture
	UpdateTexture();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateTexture()
// Desc: To test out different lock schemes
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::UpdateTexture()
{
    // Get the texture dimensions
    D3DSURFACE_DESC desc;
    m_pTexture->GetLevelDesc( 0, &desc );
    DWORD dwTexWidth  = desc.Width;
    DWORD dwTexHeight = desc.Height;
    DWORD dwTexDepth  = 0;

    // Lock the texture. D3D on Xbox only returns rawdata, that may be in
    // swizzled form.
    D3DLOCKED_RECT lock;
    m_pTexture->LockRect( 0, &lock, 0, 0L );
	VOID* pBits = lock.pBits;

	// If using the swizzler API, we need to use the intermediate storage area
	// area for the texture data.
    if( m_bUseSwizzlerAPI && XGIsSwizzledFormat(m_d3dFormat) )
		pBits = m_pTextureData;

	// If using the swizzler class, we will swizzle the texture on the fly.
    if( m_bUseSwizzlerClass && XGIsSwizzledFormat(m_d3dFormat) )
    {
        // Initialize a swizzler. The swizzler lets us easily convert texel
		// addresses when the texture is swizzled.
        Swizzler s( dwTexWidth, dwTexHeight, dwTexDepth );

        // Loop through and touch the texels. Note that SetU()/SetV() and
		// IncU()/IncV() are used to control texture addressed. This way,
		// the Get2D() function can be used to get the current address.

        s.SetV( 0 ); // Initialize the V texture coordinate

        for( DWORD y = 0; y < dwTexHeight; y++ )
        {
            s.SetU( 0 ); // Reset the U texture coordinate

            for( DWORD x = 0; x < dwTexWidth; x++ )
            {
                if( m_d3dFormat == D3DFMT_R5G6B5 || m_d3dFormat == D3DFMT_LIN_R5G6B5 )
                {
                    ((WORD*)pBits)[s.Get2D()] = (WORD)(((x>>3)<<11) | ((y>>3)<<0));
                }
                else
                {
                    ((DWORD*)pBits)[s.Get2D()] = (DWORD)(((x>>0)<<16) | ((y>>0)<<0));
                }

                s.IncU(); // Increment the U texture coordinate
            }

            s.IncV(); // Increment the V texture coordinate
        }
    }
    else
    {
        // Loop through and touch the texels.
        for( DWORD y = 0; y < dwTexHeight; y++ )
        {
            for( DWORD x = 0; x < dwTexWidth; x++ )
            {
                if( m_d3dFormat == D3DFMT_R5G6B5 || m_d3dFormat == D3DFMT_LIN_R5G6B5 )
                {
                    ((WORD*)pBits)[y*dwTexWidth+x] = (WORD)(((x>>3)<<11) | ((y>>3)<<0));
                }
                else
                {
                    ((DWORD*)pBits)[y*dwTexWidth+x] = (DWORD)(((x>>0)<<16) | ((y>>0)<<0));
                }
            }
        }
    }

    if( m_bUseSwizzlerAPI && XGIsSwizzledFormat(m_d3dFormat) )
	{
        if( m_d3dFormat == D3DFMT_R5G6B5 || m_d3dFormat == D3DFMT_LIN_R5G6B5 )
		    XGSwizzleRect( pBits, 0, NULL, lock.pBits, dwTexWidth, dwTexHeight, NULL, sizeof(WORD) );
        else
		    XGSwizzleRect( pBits, 0, NULL, lock.pBits, dwTexWidth, dwTexHeight, NULL, sizeof(DWORD) );
	}

	// Unlock the texture
    m_pTexture->UnlockRect( 0 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    BOOL bRecreateGeometry = FALSE;

    // Rotate the world
    static FLOAT fRotateAngle = 0.0f;
    fRotateAngle += m_DefaultGamepad.fX1*m_fElapsedTime;
    D3DXMATRIX matWorld;
    D3DXMatrixRotationZ( &matWorld, fRotateAngle );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Change texture format bit depth
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
    {
        switch( m_d3dFormat )
        {
            case D3DFMT_R5G6B5:       m_d3dFormat = D3DFMT_A8R8G8B8;     break;
            case D3DFMT_LIN_R5G6B5:   m_d3dFormat = D3DFMT_LIN_A8R8G8B8; break;
            case D3DFMT_A8R8G8B8:     m_d3dFormat = D3DFMT_R5G6B5;       break;
            case D3DFMT_LIN_A8R8G8B8: m_d3dFormat = D3DFMT_LIN_R5G6B5;   break;
        }

        bRecreateGeometry = TRUE;
    }

    // Toggle between linear and swizzled textures
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
    {
        switch( m_d3dFormat )
        {
            case D3DFMT_R5G6B5:       m_d3dFormat = D3DFMT_LIN_R5G6B5;   break;
            case D3DFMT_LIN_R5G6B5:   m_d3dFormat = D3DFMT_R5G6B5;       break;
            case D3DFMT_A8R8G8B8:     m_d3dFormat = D3DFMT_LIN_A8R8G8B8; break;
            case D3DFMT_LIN_A8R8G8B8: m_d3dFormat = D3DFMT_A8R8G8B8;     break;
        }

        bRecreateGeometry = TRUE;
    }

	// Toggle updating the texture every frame
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
        m_bUpdateEveryFrame = !m_bUpdateEveryFrame;

	// Toggle use of swizzler class
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] )
	{
		m_bUseSwizzlerClass = !m_bUseSwizzlerClass;

		// Swizzler class is mutually exclusive with using swizzler API
		if( m_bUseSwizzlerClass )
			m_bUseSwizzlerAPI = FALSE;
	}

	// Toggle use of swizzler API
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] )
	{
		m_bUseSwizzlerAPI = !m_bUseSwizzlerAPI;

		// If using the swizzler API, we need an intermediate storage area for the
		// texture data.
		if( m_bUseSwizzlerAPI )
			m_pTextureData = new DWORD[ 256 * 256 ];
		else
			SAFE_DELETE( m_pTextureData );

		// Swizzler class is mutually exclusive with using swizzler API
		if( m_bUseSwizzlerAPI )
			m_bUseSwizzlerClass = FALSE;
	}

	// Only use swizzler if also using swizzled textures
	m_bUseSwizzlerClass &= XGIsSwizzledFormat( m_d3dFormat );
	m_bUseSwizzlerAPI   &= XGIsSwizzledFormat( m_d3dFormat );

	// Re-create the geometry, if necessary
	if( bRecreateGeometry )
		CreateGeometry();

	// Test the swizzle texture lock
	if( m_bUpdateEveryFrame )
		UpdateTexture();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0xff000044, 1.0f, 0L );

    // Set state
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_NONE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

    // Note: The hardware requires CLAMP for linear textures
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

    // Note: swizzled textures (the default) use texture coordinates that range
    // [0,1], but when using linear textures, they range [0, width or height].
    // To see this in code, see the VB creation in the CreateGeometry() function.

    // Render a rotating square
    m_pd3dDevice->SetTexture( 0, m_pTexture );
    m_pd3dDevice->SetVertexShader( D3DFVF_VERTEX );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(VERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
		m_Font.Begin();

        m_Font.DrawText(  64, 48, 0xffffffff, L"Swizzle" );
        m_Font.DrawText( 450, 48, 0xffffff00, m_strFrameRate );

        m_Font.DrawText(  64, 100, 0xffffffff, L"Texture format:" );
        switch( m_d3dFormat )
        {
            case D3DFMT_R5G6B5:       m_Font.DrawText( 250, 100, 0xffffff00, L"D3DFMT_R5G6B5" );       break;
            case D3DFMT_A8R8G8B8:     m_Font.DrawText( 250, 100, 0xffffff00, L"D3DFMT_A8R8G8B8" );     break;
            case D3DFMT_LIN_R5G6B5:   m_Font.DrawText( 250, 100, 0xffffff00, L"D3DFMT_LIN_R5G6B5" );   break;
            case D3DFMT_LIN_A8R8G8B8: m_Font.DrawText( 250, 100, 0xffffff00, L"D3DFMT_LIN_A8R8G8B8" ); break;
        }

		m_Font.DrawText(  64, 125, 0xffffffff, L"Update every frame:" );
		m_Font.DrawText( 250, 125, 0xffffff00, m_bUpdateEveryFrame ? L"TRUE" : L"FALSE" );

		if( m_bUpdateEveryFrame )
		{
			if( XGIsSwizzledFormat( m_d3dFormat ) )
			{
				m_Font.DrawText(  64, 150, 0xffffffff, L"Using swizzler class:" );
				m_Font.DrawText( 250, 150, 0xffffff00, m_bUseSwizzlerClass ? L"TRUE" : L"FALSE" );

				m_Font.DrawText(  64, 175, 0xffffffff, L"Using swizzler API:" );
				m_Font.DrawText( 250, 175, 0xffffff00, m_bUseSwizzlerAPI ? L"TRUE" : L"FALSE" );
			}
		}

		m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




