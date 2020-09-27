//-----------------------------------------------------------------------------
// File: AntiAlias.cpp
//
// Desc: Demonstrates the various anti-alias modes
//
// Hist: 11.29.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <xgraphics.h>

#include "Resource.h"

//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "Scene.bmp",      resource_Scene_OFFSET },
    { "Scene1.bmp",     resource_Scene1_OFFSET },
    { NULL, 0 },
};

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_A_BUTTON,  XBHELP_PLACEMENT_2, L"Change AntiAlias\nMode" },
    { XBHELP_X_BUTTON,  XBHELP_PLACEMENT_1, L"Change Model" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Rotate Object" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Zoom" },

};

#define NUM_HELP_CALLOUTS ( sizeof( g_HelpCallouts ) / sizeof( g_HelpCallouts[0] ) )


#define FAR_PLANE  20.0f
#define EARTH_FAR_PLANE 200000.0f
#define NEAR_PLANE 1.0f

#define XBMESH_RENDERSHAPENUM 0xffff0000 // Render ShapeNumber is MSB portion

#define DRAW_TEAPOT							0
#define DRAW_CAR							1
#define MAX_DRAW_OBJECTS					DRAW_CAR

typedef struct
{
    DWORD           m_dwAntiAliasModeId;
    WCHAR*          m_wstrAntiAliasModeName;
} AntiAliasMode;

AntiAliasMode g_AntiAliasModes[] =
{
    { D3DMULTISAMPLE_NONE,                                    L"None"                                   },
    { D3DMULTISAMPLE_NONE,                                    L"Edge Anti-Alias"                        },
    { D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR,            L"2x MultiSample Linear Filter"           },
    { D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX,          L"2x MultiSample Quincunx Filter"         },
    { D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR, L"2x SuperSample Linear Horizontal Filter"},
    { D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR,   L"2x SuperSample Linear Vertical Filter"  },
    { D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR,            L"4x MultiSample Linear Filter"           },
    { D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN,          L"4x MultiSample Gaussian Filter"         },
    { D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR,            L"4x SuperSample Linear Filter"           },
    { D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN,          L"4x SuperSample Gaussian Filter"         },
    { D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN,          L"9x MultiSample Gaussian Filter"         },
    { D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN,          L"9x SuperSample Gaussian Filter"         },
};

#define EDGE                                1
#define MAX_ANTIALIAS_MODES					11

//-----------------------------------------------------------------------------
// Name: class CCarMesh
// Desc: Class to load and render geometry. Most functionality is inherited
//       from the CXBMesh base class.  Override renderMesh so I can change
//       the materials on the pieces of the car.
//-----------------------------------------------------------------------------
class CCarMesh : public CXBMesh
{

public:

    virtual HRESULT RenderMesh( LPDIRECT3DDEVICE8 pd3dDevice, XBMESH_DATA* pMesh, 
                                DWORD dwFlags );

};




//-----------------------------------------------------------------------------
// Name: CCarMesh::RenderMesh()
// Desc: Renders the mesh geometry.
//-----------------------------------------------------------------------------
HRESULT CCarMesh::RenderMesh( LPDIRECT3DDEVICE8 pd3dDevice, XBMESH_DATA* pMesh, 
                             DWORD dwFlags )
{
    D3DVertexBuffer* pVB           = &pMesh->m_VB;
    DWORD            dwNumVertices =  pMesh->m_dwNumVertices;
    D3DIndexBuffer*  pIB           = &pMesh->m_IB;
    DWORD            dwNumIndices  =  pMesh->m_dwNumIndices;
    DWORD            dwFVF         =  pMesh->m_dwFVF;
    DWORD            dwVertexSize  =  pMesh->m_dwVertexSize;
    D3DPRIMITIVETYPE dwPrimType    =  pMesh->m_dwPrimType;
    DWORD            dwNumSubsets  =  pMesh->m_dwNumSubsets;
    XBMESH_SUBSET*   pSubsets      = &pMesh->m_pSubsets[0];

    (VOID)dwNumIndices; // not used

    if( dwNumVertices == 0 )
        return S_OK;

    // Set the vertex stream
    pd3dDevice->SetStreamSource( 0, pVB, dwVertexSize );
    pd3dDevice->SetIndices( pIB, 0 );

    // Set the FVF code, unless the user asked us not to
    if( 0 == ( dwFlags & XBMESH_NOFVF ) )
        pd3dDevice->SetVertexShader( dwFVF );

    // Render the subsets
    DWORD dwStartShape = 0;
    DWORD dwEndShape = dwNumSubsets;

    if (dwFlags & XBMESH_RENDERSHAPENUM)
    {
        dwStartShape = (dwFlags >> 16) - 1;
        dwEndShape = dwStartShape + 1;

        // If they passed us in a bad subset,
        // just start drawing at zero and draw all.
        if (dwStartShape >= dwNumSubsets)
        {
            dwStartShape = 0;
            dwEndShape = dwNumSubsets;
        }
    }

    for( DWORD i = dwStartShape; i < dwEndShape; i++ )
    {
        BOOL bRender = FALSE;

        // Render the opaque subsets, unless the user asked us not to
        if( 0 == ( dwFlags & XBMESH_ALPHAONLY ) )
        {
            if( 0 == ( dwFlags & XBMESH_NOMATERIALS ) )
            {
                if( pSubsets[i].mtrl.Diffuse.a >= 1.0f )
                    bRender = TRUE;
            }
            else
                bRender = TRUE;
        }

        // Render the transparent subsets, unless the user asked us not to
        if( 0 == ( dwFlags & XBMESH_OPAQUEONLY ) )
        {
            if( 0 == ( dwFlags & XBMESH_NOMATERIALS ) )
            {
                if( pSubsets[i].mtrl.Diffuse.a < 1.0f )
                    bRender = TRUE;
            }
        }

        if( bRender )
        {
            // Set the material, unless the user asked us not to
            if( 0 == ( dwFlags & XBMESH_NOMATERIALS ) )
                pd3dDevice->SetMaterial( &pSubsets[i].mtrl );

            // Set the texture, unless the user asked us not to
            if( 0 == ( dwFlags & XBMESH_NOTEXTURES ) )
                pd3dDevice->SetTexture( 0, pSubsets[i].pTexture );

            // Call the callback, so the app can tweak state before rendering
            // each subset
            BOOL bRenderSubset = RenderCallback( pd3dDevice, i, &pSubsets[i], dwFlags );

            // Draw the mesh subset
            if( bRenderSubset )
            {
                DWORD dwNumPrimitives = ( D3DPT_TRIANGLESTRIP == dwPrimType ) ? 
                                    pSubsets[i].dwIndexCount-2 : pSubsets[i].dwIndexCount/3;
                pd3dDevice->DrawIndexedPrimitive( dwPrimType, 0, pSubsets[i].dwIndexCount,
                                                  pSubsets[i].dwIndexStart, dwNumPrimitives );
            }
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

    CXBFont     m_Font;             // Font object
    CXBHelp     m_Help;             // Help object
    BOOL        m_bDrawHelp;        // TRUE to draw help screen


	CXBPackedResource  m_xprResource;      			// Packed resources for the app

    CXBMesh    		   m_Teapot;					// Test Teapot
    LPDIRECT3DTEXTURE8 m_pSphereMapTexture;			// Texture for Sphere mapping

    CCarMesh           m_Car;						// Test Car

    D3DXMATRIX         m_matWorld;					// Transform matrices
    D3DXMATRIX         m_matView;
    D3DXMATRIX         m_matProj;

	D3DXVECTOR3        m_vCameraPosition;			// Camera position and look vector
	D3DXVECTOR3        m_vLookPosition;

	FLOAT              m_fXObjRotate;				// Rotation values for Teapot
	FLOAT              m_fYObjRotate;

	DWORD              m_dwFresnelShader;           // Handle to the Fresnel Vertex Shader

	DWORD              m_dwCarBody; 	  			// Handle to Pixel Shader for car body
	DWORD              m_dwCarWindow;	  			// Handle to Pixel Shader for car Windows

	DWORD         	   m_dwDrawObject;	  	   		// Tells me which object to draw.

	DWORD			   m_dwAntiAliasMode;           // Tells me what anti-alias mode to use.
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
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplication()
{
    m_bDrawHelp = FALSE;

    // Do not wait for VSync.  
    // Show changes in framerate from different antiAlias Modes.
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}




//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create a mesh (vertex and index buffers) for the teapot
    if( FAILED( m_Teapot.Create( m_pd3dDevice, "Models\\Teapot.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create a mesh (vertex and index buffers) for the car
    if( FAILED( m_Car.Create( m_pd3dDevice, "Models\\GenericSedan.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the spheremap texture
    m_pSphereMapTexture = m_xprResource.GetTexture( "Scene1.bmp" );

    // Set the transform matrices
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixIdentity( &m_matView );
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4.0f, 4.0f/3.0f, 
                                NEAR_PLANE, FAR_PLANE );

	// Setup the camera look and position vectors
	m_vLookPosition.x = 
	m_vLookPosition.y = 
	m_vLookPosition.z = 0.f;

	m_vCameraPosition.x = 5.f;
	m_vCameraPosition.y = m_vCameraPosition.z = 0.f;

	// Set initial rotation of object
	m_fXObjRotate = m_fYObjRotate = 0.f;

	// Set which object to draw
	m_dwDrawObject = DRAW_TEAPOT;

    // Set initial Anti-Alias mode to none
    m_dwAntiAliasMode = 0;

    // Create vertex shaders
    {
        DWORD dwDecl[] =
        {
            D3DVSD_STREAM( 0 ),
            D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),
            D3DVSD_REG( 1, D3DVSDT_FLOAT3 ),
			D3DVSD_REG( 2, D3DVSDT_FLOAT2 ),
            D3DVSD_END()
        };

        if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\fresnel.xvu",
                                               dwDecl, &m_dwFresnelShader ) ) )
            return XBAPPERR_MEDIANOTFOUND;
    }

    // Create the pixel shaders
    if( FAILED( XBUtil_CreatePixelShader( m_pd3dDevice, "Shaders\\caropaque.xpu",
                                          &m_dwCarBody ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( XBUtil_CreatePixelShader( m_pd3dDevice, "Shaders\\cartransparent.xpu",
                                          &m_dwCarWindow ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

	//Toggle Anti-Alias mode
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
	{
		++m_dwAntiAliasMode;

		if ( m_dwAntiAliasMode > MAX_ANTIALIAS_MODES)
		{
			m_dwAntiAliasMode = 0;
		}

		// Just reset the MultiSampleType in m_d3dpp.
		m_d3dpp.MultiSampleType = g_AntiAliasModes[m_dwAntiAliasMode].m_dwAntiAliasModeId;

		// Reset the device - Just changes the screen resolution and 
		// Backbuffer size on Xbox.
        HRESULT hr = S_OK;
		if( FAILED( hr = m_pd3dDevice->Reset( &m_d3dpp ) ) )
		{
			return hr;
		}
	}

	//Toggle Object
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
	{
		++m_dwDrawObject;

		if( m_dwDrawObject > MAX_DRAW_OBJECTS )
		{
			m_dwDrawObject = 0;
		}

		switch( m_dwDrawObject )
		{
			case DRAW_TEAPOT:
				m_vCameraPosition.x = m_Teapot.ComputeRadius() * 3.0f;
				break;

			case DRAW_CAR:
				m_vCameraPosition.x = m_Car.ComputeRadius() * 3.0f;
				break;
		}
	}

    // Setup the projection matrix
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4.0f, 4.0f/3.0f, 
        NEAR_PLANE, FAR_PLANE );

	// Setup the view Matrix
	D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &m_matView, &m_vCameraPosition, &m_vLookPosition, &vUp );

	// Check if we should rotate the object
    m_fXObjRotate += m_DefaultGamepad.fX1 * D3DX_PI * m_fElapsedTime;
    m_fYObjRotate += m_DefaultGamepad.fY1 * D3DX_PI * m_fElapsedTime;

	// Check if we should move the camera in or out.
	FLOAT fCameraZoomFactor = 1.0f;
	m_vCameraPosition.x += m_DefaultGamepad.fY2 * fCameraZoomFactor * m_fElapsedTime;

    // Setup object matrix
    D3DXMATRIX matObjRotate;
    D3DXMatrixRotationYawPitchRoll( &matObjRotate, -m_fXObjRotate, -m_fYObjRotate, 0.0f );
    
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &matObjRotate );

	// Setup the fresnel rotate matrix
    D3DXMATRIX matFresnelRotate;

	FLOAT fDeterminant = 0.f;
	D3DXMatrixInverse( &matFresnelRotate, &fDeterminant, &matObjRotate );

    // Setup the vertex shader constants
    {
		// Create the World/view/projection matrix concatenation
        D3DXMATRIX mat;
		D3DXMatrixMultiply( &mat, &m_matWorld, &m_matView );
        D3DXMatrixMultiply( &mat, &mat, &m_matProj );
        D3DXMatrixTranspose( &mat, &mat );

		// Rotate the camera normal into the Object's rotational frame.
		// Saves us having to transform all of the vectors on the object in the shader!
		D3DXVECTOR3 vCameraNormal( 1.0f, 0.0f, 0.0f );
		D3DXVECTOR3 vRotatedNormal;
		D3DXVec3TransformNormal( &vRotatedNormal, &vCameraNormal, &matFresnelRotate );

        m_pd3dDevice->SetVertexShaderConstant( 0, &mat, 4 );
        m_pd3dDevice->SetVertexShaderConstant( 4, &vRotatedNormal, 1 );

		// Generate spheremap texture coords from the position
		D3DXMATRIX matTexture;
		D3DXMatrixIdentity( &matTexture );
		matTexture._11 = 0.5f; matTexture._12 = 0.0f;
		matTexture._21 = 0.0f; matTexture._22 =-0.5f;
		matTexture._14 = 0.5f; matTexture._24 = 0.5f;

		D3DXMatrixMultiply( &matTexture, &matTexture, &mat );
		m_pd3dDevice->SetVertexShaderConstant( 15, &matTexture, 4 );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
                         0x00404040, 1.0f, 0L );

    // Setup render state
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );

    // Draw a gradient filled background
    if ( (m_dwDrawObject == DRAW_TEAPOT) || m_bDrawHelp)
        RenderGradientBackground( 0xff404040, 0xff4040C0 );

    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

    // Draw the Object desired
	switch( m_dwDrawObject )
	{
		case DRAW_TEAPOT:
		{
			// If we are in edge anti-alias mode, set the render states
			// Done somewhat redundantly here for clarity.
			if (m_dwAntiAliasMode == EDGE)
			{
                //--------------------------------------------------------------------------------
                // NOTE:  THIS IS VERY IMPORTANT
                //
                // Edge anti-aliasing works by drawing an alpha edge to each triangle and
                // line which is rendered.  For objects which are drawn WITHOUT zBuffer
                // writes and in alpha mode Src Alpha, Dest One, like the teapot, this works 
                // great. For any other kind of object, if you do not draw the object 
                // un-aliased first, you will get a wire frame set of artifacts because the 
                // alphas write to the zbuffer as well as the normal triangles.  The alphaed 
                // edges take on the color behind them which is the background of the 
                // backbuffer.  You need the background color to be the actual object color for
                // that location.  This is still a big win over the normal anit-aliasing modes 
                // because you don't need a giant backbuffer to get the aliasing you want.  
                // Also, you can pick and choose what objects to anti-alias!
                //
                //--------------------------------------------------------------------------------
				m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
				m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS, TRUE );
			}

			// Set new constants for this object.
            // Setup constants
            D3DXVECTOR4 vForceColor( 0.45f, 0.45f, 0.45f, 1.0f );
            D3DXVECTOR4 vConstants( 1.0f, 0.5f, 3.0f, 0.15f );
            D3DXVECTOR4 vConstants1( 1.0f, 0.0f, 0.0f, 0.0f );

            m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
            m_pd3dDevice->SetVertexShaderConstant( 6, &vConstants, 1 );
            m_pd3dDevice->SetVertexShaderConstant( 7, &vConstants1, 1 );

			// Set the vertex Shader
			m_pd3dDevice->SetVertexShader( m_dwFresnelShader );

			// Adds in the teapot's sphere map
			m_pd3dDevice->SetTexture( 0, m_pSphereMapTexture );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	
			m_Teapot.Render( m_pd3dDevice, XBMESH_NOTEXTURES | XBMESH_NOFVF );
		}
		break;

		case DRAW_CAR:
		{
			// Set render state changes for the car
    		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
            m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	
			// Set new constants for the additional work.
			{
				D3DXVECTOR4 vForceColor( 0.0f, 0.23f, 0.70f, 1.0f );
				D3DXVECTOR4 vConstants( 1.0f, 0.5f, 1.5f, 0.15f );
                D3DXVECTOR4 vConstants1( 0.35f, 0.0f, 0.0f, 0.0f );
	
				m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 6, &vConstants, 1 );
                m_pd3dDevice->SetVertexShaderConstant( 7, &vConstants1, 1 );
			}
	
			// Set the Vertex and Pixel Shader
			m_pd3dDevice->SetVertexShader( m_dwFresnelShader );
            m_pd3dDevice->SetPixelShader( m_dwCarBody );
	
			// Adds in the car's sphere map
			m_pd3dDevice->SetTexture( 0, m_pSphereMapTexture );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );


            // Draw the roof and sides of car - 
            m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (1<<16) );
	
			// If we are in edge anti-alias mode, set the render states
			// Done somewhat redundantly here for clarity.
			if( m_dwAntiAliasMode == EDGE )
			{
				m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
				m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS, TRUE );

                //--------------------------------------------------------------------------------
                // NOTE:  THIS IS VERY IMPORTANT
                //
                // Edge anti-aliasing works by drawing an alpha edge to each triangle and
                // line which is rendered.  For objects which are drawn WITHOUT zBuffer
                // writes and in alpha mode Src Alpha, Dest One, like the teapot, this works 
                // great. For any other kind of object, if you do not draw the object 
                // un-aliased first, you will get a wire frame set of artifacts because the 
                // alphas write to the zbuffer as well as the normal triangles.  The alphaed 
                // edges take on the color behind them which is the background of the 
                // backbuffer.  You need the background color to be the actual object color for
                // that location.  This is still a big win over the normal anit-aliasing modes 
                // because you don't need a giant backbuffer to get the aliasing you want.  
                // Also, you can pick and choose what objects to anti-alias!
                //
                // To see what happens if you don't do this, comment out the m_Car.RenderMesh
                // call directly above the if( m_dwAntiAlias == EDGE ) statement.
                //
                //--------------------------------------------------------------------------------
                m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                                  XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (1<<16) );

                //Reset the render states to draw the next piece of car.
				m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
				m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS, FALSE );
			}

            // Draw the underside of the car - Much darker color
			{
				D3DXVECTOR4 vForceColor( 0.0f, 0.03f, 0.15f, 1.0f );
				D3DXVECTOR4 vConstants( 1.0f, 0.5f, 1.5f, 0.15f );
                D3DXVECTOR4 vConstants1( 0.25f, 0.0f, 0.0f, 0.0f );
	
				m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 6, &vConstants, 1 );
                m_pd3dDevice->SetVertexShaderConstant( 7, &vConstants1, 1 );
			}

            m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (2<<16) );

			// If we are in edge anti-alias mode, set the render states
			// Done somewhat redundantly here for clarity.
			if( m_dwAntiAliasMode == EDGE )
			{
				m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
				m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS, TRUE );

                //--------------------------------------------------------------------------------
                // NOTE:  THIS IS VERY IMPORTANT
                //
                // Edge anti-aliasing works by drawing an alpha edge to each triangle and
                // line which is rendered.  For objects which are drawn WITHOUT zBuffer
                // writes and in alpha mode Src Alpha, Dest One, like the teapot, this works 
                // great. For any other kind of object, if you do not draw the object 
                // un-aliased first, you will get a wire frame set of artifacts because the 
                // alphas write to the zbuffer as well as the normal triangles.  The alphaed 
                // edges take on the color behind them which is the background of the 
                // backbuffer.  You need the background color to be the actual object color for
                // that location.  This is still a big win over the normal anit-aliasing modes 
                // because you don't need a giant backbuffer to get the aliasing you want.  
                // Also, you can pick and choose what objects to anti-alias!
                //
                // To see what happens if you don't do this, comment out the m_Car.RenderMesh
                // call directly above the if( m_dwAntiAlias == EDGE ) statement.
                //
                //--------------------------------------------------------------------------------
                m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                                  XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (2<<16) );

                //Reset the render states to draw the next piece of car.
				m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
				m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS, FALSE );
			}

            // Draw the Window seals of the car - Black.  No reflection
			{
				D3DXVECTOR4 vForceColor( 0.0f, 0.0f, 0.0f, 0.0f );
				D3DXVECTOR4 vConstants( 1.0f, 0.5f, 1.5f, 0.15f );
                D3DXVECTOR4 vConstants1( 0.15f, 0.0f, 0.0f, 0.0f );
	
				m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 6, &vConstants, 1 );
                m_pd3dDevice->SetVertexShaderConstant( 7, &vConstants1, 1 );
			}

            m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (5<<16) );

            m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (6<<16) );

			// If we are in edge anti-alias mode, set the render states
			// Done somewhat redundantly here for clarity.
			if( m_dwAntiAliasMode == EDGE )
			{
				m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
				m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS, TRUE );

                //--------------------------------------------------------------------------------
                // NOTE:  THIS IS VERY IMPORTANT
                //
                // Edge anti-aliasing works by drawing an alpha edge to each triangle and
                // line which is rendered.  For objects which are drawn WITHOUT zBuffer
                // writes and in alpha mode Src Alpha, Dest One, like the teapot, this works 
                // great. For any other kind of object, if you do not draw the object 
                // un-aliased first, you will get a wire frame set of artifacts because the 
                // alphas write to the zbuffer as well as the normal triangles.  The alphaed 
                // edges take on the color behind them which is the background of the 
                // backbuffer.  You need the background color to be the actual object color for
                // that location.  This is still a big win over the normal anit-aliasing modes 
                // because you don't need a giant backbuffer to get the aliasing you want.  
                // Also, you can pick and choose what objects to anti-alias!
                //
                // To see what happens if you don't do this, comment out the m_Car.RenderMesh
                // calls directly above the if( m_dwAntiAlias == EDGE ) statement.
                //
                //--------------------------------------------------------------------------------
                m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                                  XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (5<<16) );

                m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                                  XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (6<<16) );

                //Reset the render states to draw the next piece of car.
				m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
				m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS, FALSE );
			}

			// Set the Pixel Shader for the windows and mirrors
            m_pd3dDevice->SetPixelShader( m_dwCarWindow );
	
            // Draw the Windows and mirrors of the car - Not drawn above so transparent
            // There is almost no difference between drawing the windows Edge anti-aliased
            // and drawing them without the edge anti-aliasing.  This is because the "edges"
            // of the windows were already drawn above with edge anti-aliasing on.  Since
            // the zbuffer values for the "edges" are already written, the other edges do
            // not contribute much to the final appearance.  Games should check what parts
            // of an object need aliasing to help performance.
			{
				D3DXVECTOR4 vConstants( 1.0f, 0.5f, 1.5f, 0.85f );
				D3DXVECTOR4 vConstants1( 1.0f, 0.0f, 0.0f, 0.0f );
				D3DXVECTOR4 vForceColor( 0.75f, 0.75f, 0.75f, 1.0f );

				m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 6, &vConstants, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 7, &vConstants1, 1 );
			}

            m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (3<<16) );

			//Make mirrors more reflective then transparent
			{
				D3DXVECTOR4 vConstants( 1.0f, 0.5f, 1.5f, 0.15f );
				D3DXVECTOR4 vConstants1( 0.75f, 0.0f, 0.0f, 0.0f );
				D3DXVECTOR4 vForceColor( 0.75f, 0.75f, 0.75f, 1.0f );

				m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 6, &vConstants, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 7, &vConstants1, 1 );
			}

  			m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (4<<16) );
		}
		break;

	}

    // Turn off the anti-alias mode so that the fonts look better
    m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"AntiAlias" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        WCHAR msg[1024];
        swprintf(msg,L"AntiAlias Mode - %s",g_AntiAliasModes[m_dwAntiAliasMode].m_wstrAntiAliasModeName);

        m_Font.DrawText(  64, 70, 0xffffffff, msg);

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

