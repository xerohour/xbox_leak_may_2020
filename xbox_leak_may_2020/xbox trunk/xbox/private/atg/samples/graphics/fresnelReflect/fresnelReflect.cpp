//-----------------------------------------------------------------------------
// File: fresnel.cpp
//
// Desc: Demonstrates Fresnel Reflection for more realistic shiny surfaces
//
// Hist: 9.25.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XGraphics.h>
#include <XObj.h>
#include <XBMesh.h>

#include "Resource.h"
//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "Scene.bmp",      resource_Scene_OFFSET },
    { "Scene1.bmp",      resource_Scene1_OFFSET },
    { "earthmap2k.bmp", resource_Earth_OFFSET },
    { "cloudcolor.tga", resource_Clouds_OFFSET },
    { NULL, 0 },
};

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, L"Switch Object" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Rotate Object" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Zoom/Rotate Light" },
};

#define NUM_HELP_CALLOUTS 4

#define FAR_PLANE  20.0f
#define EARTH_FAR_PLANE 200000.0f
#define NEAR_PLANE 1.0f

#define XBMESH_RENDERSHAPENUM 0xffff0000 // Render ShapeNumber is MSB portion

enum DrawObject
{
	Draw_EnvTeapot,
	Draw_EnvCar,
	Draw_Earth
};




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
    DWORD dwEndShape = 0;

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
                DWORD dwNumPrimitives = ( D3DPT_TRIANGLESTRIP == dwPrimType ) ? pSubsets[i].dwIndexCount-2 : pSubsets[i].dwIndexCount/3;
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

    // Font and help
    CXBFont     m_Font;
    CXBHelp     m_Help;

    BOOL        m_bDrawHelp;

	CXBPackedResource  m_xprResource;      			// Packed resources for the app

    CXBMesh    		   m_Teapot;					// Test Teapot
    LPDIRECT3DTEXTURE8 m_pSphereMapTexture;			// Texture for Sphere mapping

    CCarMesh           m_Car;						// Test Car

	CXBMesh            m_Earth;						// Test Earth Globe
	LPDIRECT3DTEXTURE8 m_pEarthMapTexture;			// Texture for the earth
	LPDIRECT3DTEXTURE8 m_pEarthCloudTexture;		// Texture for the clouds

    D3DXMATRIX         m_matWorld;					// Transform matrices
    D3DXMATRIX         m_matView;
    D3DXMATRIX         m_matProj;

	D3DXVECTOR3        m_vCameraPosition;			// Camera position and look vector
	D3DXVECTOR3        m_vLookPosition;

	FLOAT              m_fXObjRotate;				// Rotation values for Teapot
	FLOAT              m_fYObjRotate;

	DWORD              m_dwFresnelShader;	            // Handle to the Fresnel Vertex Shader

	DWORD              m_dwEarthShader;					// Handle to the Shader for 
														// the Earth color and cloud layers

	DWORD              m_dwCarShader;					// Handle to the Shader for car's body

	DWORD              m_dwFresnelEarthShader;			// Handle to the Fresnel Shader for 
														// the Earth's atmosphere

	FLOAT              m_fLightRotate;					// Current Rotation of light source

	DrawObject         m_DrawObject;				// Tells me which object to draw.
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

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create a mesh (vertex and index buffers) for the teapot
    if( FAILED( m_Teapot.Create( m_pd3dDevice, "Models\\Teapot.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create a mesh (vertex and index buffers) for the car
    if( FAILED( m_Car.Create( m_pd3dDevice, "Models\\GenericSedan.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create a mesh (vertex and index buffers) for the earth globe
    if( FAILED( m_Earth.Create( m_pd3dDevice, "Models\\gearth.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the spheremap texture
    m_pSphereMapTexture = m_xprResource.GetTexture( "Scene1.bmp" );

	//Load the Earth's textures
	m_pEarthMapTexture = m_xprResource.GetTexture( "earthmap2k.bmp" );
	m_pEarthCloudTexture = m_xprResource.GetTexture( "cloudcolor.tga" );

    // Set the transform matrices
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixIdentity( &m_matView );
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4.0f, 4.0f/3.0f, NEAR_PLANE, FAR_PLANE );

	// Setup the camera look and position vectors
	m_vLookPosition.x = 
	m_vLookPosition.y = 
	m_vLookPosition.z = 0.f;

	m_vCameraPosition.x = 5.f;
	m_vCameraPosition.y = m_vCameraPosition.z = 0.f;

	// Set initial rotation of object
	m_fXObjRotate = m_fYObjRotate = m_fLightRotate = 0.f;

	// Set which object to draw
	m_DrawObject = Draw_EnvTeapot;

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

        if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\fresnel3.xvu",
                                               dwDecl, &m_dwFresnelShader ) ) )
            return XBAPPERR_MEDIANOTFOUND;

        if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\fresnel4.xvu",
                                               dwDecl, &m_dwFresnelEarthShader ) ) )
            return XBAPPERR_MEDIANOTFOUND;

        if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\planet.xvu",
                                               dwDecl, &m_dwEarthShader ) ) )
            return XBAPPERR_MEDIANOTFOUND;

        if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\car.xvu",
                                               dwDecl, &m_dwCarShader ) ) )
            return XBAPPERR_MEDIANOTFOUND;
    }

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

	// Check for buttons pressed 
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
	{
		switch( m_DrawObject )
		{
			case Draw_EnvTeapot:
				m_vCameraPosition.x = m_Car.ComputeRadius() * 3.0f;
				m_DrawObject = Draw_EnvCar;
				break;

			case Draw_EnvCar:
				m_vCameraPosition.x = m_Earth.ComputeRadius() * 3.0f;
				m_DrawObject = Draw_Earth;
				break;

			case Draw_Earth:
				m_vCameraPosition.x = 5.0f;
				m_DrawObject = Draw_EnvTeapot;
				break;
		}
	}

    // Setup the projection matrix
    if (m_DrawObject == Draw_Earth)
        D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4.0f, 4.0f/3.0f, NEAR_PLANE, EARTH_FAR_PLANE );
    else
        D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4.0f, 4.0f/3.0f, NEAR_PLANE, FAR_PLANE );

	// Setup the view Matrix
	D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &m_matView, &m_vCameraPosition, &m_vLookPosition, &vUp );

	// Check if we should rotate the object
    m_fXObjRotate += m_DefaultGamepad.fX1 * D3DX_PI * m_fElapsedTime;
    m_fYObjRotate += m_DefaultGamepad.fY1 * D3DX_PI * m_fElapsedTime;

	// Check if we should move the camera in or out.
	FLOAT fCameraZoomFactor = ( m_DrawObject == Draw_Earth ) ? 2000.0f : 1.0f;
	m_vCameraPosition.x += m_DefaultGamepad.fY2 * fCameraZoomFactor * m_fElapsedTime;

	// Check if we should rotate the light source.
	m_fLightRotate += m_DefaultGamepad.fX2 * D3DX_PI * m_fElapsedTime;

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

		// Setup constants
		D3DXVECTOR4 vForceColor( 0.45f, 0.45f, 0.45f, 1.0f );
		D3DXVECTOR4 vConstants( 1.0f, 0.5f, 3.0f, 0.15f );
		D3DXVECTOR4 vConstants1( 1.0f, 0.0f, 0.0f, 0.0f );

        m_pd3dDevice->SetVertexShaderConstant( 0, &mat, 4 );
        m_pd3dDevice->SetVertexShaderConstant( 4, &vRotatedNormal, 1 );
        m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
        m_pd3dDevice->SetVertexShaderConstant( 6, &vConstants, 1 );
        m_pd3dDevice->SetVertexShaderConstant( 7, &vConstants1, 1 );

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
    if ( m_DrawObject == Draw_Earth)
        m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
                             0x00000000, 1.0f, 0L );
    else
        m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
                             0xff404040, 1.0f, 0L );

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
	if( m_DrawObject == Draw_EnvTeapot )
    {
        m_pd3dDevice->SetVertexShader( NULL );
        RenderGradientBackground( 0xff404040, 0xff4040C0 );
    }

    // Draw the Object desired
	switch( m_DrawObject )
	{
		case Draw_EnvTeapot:
		{
			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

			// Set new constants for this object.
			D3DXVECTOR4 vForceColor( 0.55f, 0.55f, 0.55f, 1.0f );

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

		case Draw_EnvCar:
		{
			// Set render state changes for the car
			m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
            m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	
			// Set new constants for the additional work.
			{
				m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	
				D3DXMATRIX matLightRotate;
				D3DXMatrixRotationY( &matLightRotate, m_fLightRotate );
	
				D3DXVECTOR4 vForceColor( 0.0f, 0.23f, 0.70f, 1.0f );
				D3DXVECTOR3 vLightDirection( 1.0f, 0.0f, 0.0f );
				D3DXVECTOR4 vAmbientLight( 0.0f, 0.23f, 0.70f, 1.0f );
	
				D3DXVec3TransformNormal( &vLightDirection, &vLightDirection, &matLightRotate );
	
				m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 8, &vLightDirection, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 9, &vAmbientLight, 1 );
			}
	
			// Set the vertex Shader
			m_pd3dDevice->SetVertexShader( m_dwCarShader );
	
			// Adds in the car's color map
			m_pd3dDevice->SetTexture( 0, 0 );

            // Draw the roof and sides of car
            m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (1<<16) );
	
            // Draw the underside of the car - Much darker color
			{
				D3DXVECTOR4 vForceColor( 0.0f, 0.03f, 0.15f, 1.0f );
				D3DXVECTOR4 vAmbientLight( 0.0f, 0.0f, 0.05f, 1.0f );
	
				m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 9, &vAmbientLight, 1 );
			}

            m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (2<<16) );

            // Draw the Window seals of the car - Black.  No reflection
			{
				D3DXVECTOR4 vForceColor( 0.0f, 0.0f, 0.0f, 0.0f );
				D3DXVECTOR4 vAmbientLight( 0.0f, 0.0f, 0.0f, 1.0f );
	
				m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 9, &vAmbientLight, 1 );
			}

            m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (5<<16) );

            m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (6<<16) );

			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	
			// Set new constants for this object and the "shiny" pass.
			{
				D3DXVECTOR4 vConstants( 1.0f, 0.5f, 1.5f, 0.15f );
				D3DXVECTOR4 vConstants1( 0.75f, 0.0f, 0.0f, 0.0f );
		
				D3DXVECTOR4 vForceColor( 0.55f, 0.55f, 0.55f, 1.0f );
				m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 6, &vConstants, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 7, &vConstants1, 1 );
			}
	
			// Set the vertex Shader
			m_pd3dDevice->SetVertexShader( m_dwFresnelShader );
	
			// Adds in the car's sphere map
			m_pd3dDevice->SetTexture( 0, m_pSphereMapTexture );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	
            // Draw the roof and sides of car - Quite Shiny
            m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (1<<16) );
	
            // Draw the underside of the car - MUCH less shiny
			{
				D3DXVECTOR4 vConstants( 1.0f, 0.5f, 1.5f, 0.05f );
				D3DXVECTOR4 vConstants1( 0.15f, 0.0f, 0.0f, 0.0f );
		
				D3DXVECTOR4 vForceColor( 0.55f, 0.55f, 0.55f, 1.0f );
				m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 6, &vConstants, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 7, &vConstants1, 1 );
			}
            m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (2<<16) );

            // Draw the Windows and mirrors of the car - Not drawn above so transparent
			{
				D3DXVECTOR4 vConstants( 1.0f, 0.5f, 1.5f, 0.15f );
				D3DXVECTOR4 vConstants1( 0.75f, 0.0f, 0.0f, 0.0f );
		
				D3DXVECTOR4 vForceColor( 0.75f, 0.75f, 0.75f, 1.0f );
				m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 6, &vConstants, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 7, &vConstants1, 1 );
			}
            m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (3<<16) );
	
  			m_Car.RenderMesh( m_pd3dDevice, m_Car.GetMesh(0),
                              XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF | (4<<16) );
		}
		break;
	
		case Draw_Earth:
		{
			// Set render state changes for earth
			m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			// Set new constants for the additional work.
			{
				D3DXMATRIX matLightRotate;
			    D3DXMatrixRotationY( &matLightRotate, m_fLightRotate );

 				D3DXVECTOR4 vForceColor( 0.6f, 0.6f, 0.6f, 1.0f );
				D3DXVECTOR3 vLightDirection( 1.5f, 0.0f, 0.0f );
				D3DXVECTOR4 vAmbientLight( 0.15f, 0.15f, 0.15f, 1.0f );

				D3DXVec3TransformNormal( &vLightDirection, &vLightDirection, &matLightRotate );

				m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 8, &vLightDirection, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 9, &vAmbientLight, 1 );
			}

			// Set the vertex Shader
			m_pd3dDevice->SetVertexShader( m_dwEarthShader );

			// Adds in the earth's color map
			m_pd3dDevice->SetTexture( 0, m_pEarthMapTexture );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
		    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
	
			m_Earth.Render( m_pd3dDevice, XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF );

			// Add in the earth's cloud map
			m_pd3dDevice->SetTexture( 0, m_pEarthCloudTexture );
			m_Earth.Render( m_pd3dDevice, XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF );

			// Add in the Fresnel Transparency layer
			m_pd3dDevice->SetTexture( 0, 0 );

			// Setup the constants for this shader
			{
				D3DXVECTOR4 vForceColor( 0.15f, 0.45f, 0.75f, 1.0f);
				D3DXVECTOR4 vConstants( 1.0f, 0.5f, 1.2f, 0.15f);
				D3DXVECTOR4 vConstants1( 0.75f, 0.0f, 0.0f, 0.0f);

		        m_pd3dDevice->SetVertexShaderConstant( 5, &vForceColor, 1 );
			    m_pd3dDevice->SetVertexShaderConstant( 6, &vConstants, 1 );
				m_pd3dDevice->SetVertexShaderConstant( 7, &vConstants1, 1 );
			}

			// Set the vertex Shader
			m_pd3dDevice->SetVertexShader( m_dwFresnelEarthShader );
			m_Earth.Render( m_pd3dDevice, XBMESH_NOTEXTURES | XBMESH_NOMATERIALS | XBMESH_NOFVF );
		}
		break;
	}

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();

		if( m_DrawObject != Draw_Earth )
		{
   			m_Font.DrawText( 64, 50, 0xffffffff, L"Fresnel Reflectivity" );
			m_Font.DrawText( 64, 70, 0xffffffff, L"Using Shader Fresnel3" );

			m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
		}
		else
		{
			m_Font.DrawText( 64, 50, 0xffffffff, L"Fresnel Transparency" );
			m_Font.DrawText( 64, 70, 0xffffffff, L"Using Shader Fresnel4" );
			m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
		}
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

