//-----------------------------------------------------------------------------
// File: PerPixelLighting.cpp
//
// Desc: Example code showing how to do perpixel lighting using vertex shaders.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBResource.h>
#include <XBUtil.h>
#include <xgraphics.h>

// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Move point\nlight" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Move dir. light" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle base\ntexture" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle ambient\nlight" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle dir.\nlight" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle pt.\nlight" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS 8




//-----------------------------------------------------------------------------
// Name: struct CUSTOMVERTEX
// Desc: A position, normal, and tex coords for each vertex
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    D3DXVECTOR3 p;         // Position
    D3DXVECTOR3 n;         // Normal
    FLOAT       tu, tv;    // Texture coords
}; 




//-----------------------------------------------------------------------------
// Name: struct TANGENTSPACE
// Desc: Three orthogonal tangent space vectors for each vertex
//-----------------------------------------------------------------------------
struct TANGENTSPACE
{
    D3DXVECTOR3 vTangent;
    D3DXVECTOR3 vBinormal;
    D3DXVECTOR3 vNormal;
}; 




//-----------------------------------------------------------------------------
// Name: class CBumpyObject
// Desc: 
//-----------------------------------------------------------------------------
class CBumpyObject
{
    LPDIRECT3DDEVICE8       m_pd3dDevice;        // Local copy of the d3d device

    LPDIRECT3DVERTEXBUFFER8 m_pTangentSpaceVB;   // Hold tangent space vectors
    LPDIRECT3DVERTEXBUFFER8 m_pSphereVerticesVB; // Hold geometry
    LPDIRECT3DINDEXBUFFER8  m_pSphereIndicesIB;
    DWORD                   m_dwNumVertices;
    DWORD                   m_dwNumIndices;

    LPDIRECT3DTEXTURE8      m_pBaseTexture;      // Base texture
    LPDIRECT3DTEXTURE8      m_pNormalMap;        // Normal texture (bumpmap)
    LPDIRECT3DCUBETEXTURE8  m_pCubeMap;          // Normalization cubemap

    DWORD                   m_dwVertexShader;    // Custom vertex shader

public:
    CBumpyObject();
    virtual ~CBumpyObject();

    HRESULT Init( LPDIRECT3DDEVICE8 pd3dDevice, CXBPackedResource* pResource );
    VOID    InitSphere( FLOAT radius, DWORD nLat, DWORD nLong );
    VOID    CreateBasisMatrices();

    VOID    ProcessVertices( D3DXVECTOR3* pLightDir, D3DXVECTOR3* pLightPos );

    HRESULT Render( LPDIRECT3DDEVICE8 m_pd3dDevice );
    HRESULT RenderObject( LPDIRECT3DDEVICE8 m_pd3dDevice );
};




//-----------------------------------------------------------------------------
// Globally accessed attributes
//-----------------------------------------------------------------------------
D3DXVECTOR3 g_vPtLightPos;        // Point light position
DWORD       g_dwPtLightColor;     // Point light color
D3DXVECTOR3 g_vDirLightDirection; // Directional light direction
DWORD       g_dwDirLightColor;    // Color of directional light
DWORD       g_dwAmbientColor;     // Ambient light value

BOOL        g_bEnableBaseTexturePass  = TRUE;
BOOL        g_bEnableAmbientLightPass = TRUE;
BOOL        g_bEnableDirLightPass     = TRUE;
BOOL        g_bEnablePointLightPass   = TRUE;




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource   m_xprResource;        // Packed resources for the app
    CXBFont             m_Font;               // Font class
    CXBHelp             m_Help;               // Help class
    BOOL                m_bDrawHelp;          // Whether to draw help

    CBumpyObject        m_BumpyObject;

    D3DXMATRIX          m_matWorld;

    VOID DrawLight();

protected:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

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
    m_bDrawHelp          = FALSE;

    // Initial light attributes
    g_vPtLightPos        = D3DXVECTOR3( -1.0f, 1.0f, -2.75f );
    g_dwPtLightColor     = 0x00ff0000;

    g_vDirLightDirection = D3DXVECTOR3( 1.0f, 1.0f, 1.5f );
    g_dwDirLightColor    = 0x008080ff;

    g_dwAmbientColor     = 0x00404040;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: This creates all device-dependant objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT hr;
    
    // Create the font
    if( FAILED( hr = m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( hr = m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the bumpy object
    if( FAILED( hr = m_BumpyObject.Init( m_pd3dDevice, &m_xprResource ) ) )
        return hr;

    // Set the transform matrices
    D3DXMATRIX matView, matProj;
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixTranslation( &matView, 0.0f, 0.0f, 5.0f );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/3, 640.0f/480.0f, 1.0f, 20.0f );

    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &m_matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Rotate the scene
    D3DXMATRIX matRotate;
    D3DXMatrixRotationY( &matRotate, -m_fElapsedAppTime/2 );
    D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &matRotate );

    // Toggle the render passes
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
        g_bEnableDirLightPass     = !g_bEnableDirLightPass;
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
        g_bEnablePointLightPass   = !g_bEnablePointLightPass;
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        g_bEnableBaseTexturePass  = !g_bEnableBaseTexturePass;
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
        g_bEnableAmbientLightPass = !g_bEnableAmbientLightPass;

    // Adjust the point light's position
    static FLOAT phi1 = 0.5f;
    phi1 += 3.0f * m_fElapsedTime * m_DefaultGamepad.fX1;
    if( phi1 < -2.3f ) phi1 = -2.3f;
    if( phi1 > -0.9f ) phi1 = -0.9f;

    static FLOAT theta1 = 0.0f;
    theta1 -= 3.0f * m_fElapsedTime * m_DefaultGamepad.fY1;
    if( theta1 < +0.9f ) theta1 = +0.9f;
    if( theta1 > +2.3f ) theta1 = +2.3f;

    g_vPtLightPos.x = 2*cosf( phi1 );
    g_vPtLightPos.y = 2*cosf( theta1 );
    g_vPtLightPos.z = 2*sinf( phi1 ) * sinf( theta1 );

    // Adjust the directional light's direction
    static FLOAT phi2 = 0.5f;
    phi2 += 3.0f * m_fElapsedTime * m_DefaultGamepad.fX2;
    if( phi2 < -2.3f ) phi2 = -2.3f;
    if( phi2 > -0.9f ) phi2 = -0.9f;

    static FLOAT theta2 = 0.0f;
    theta2 -= 3.0f * m_fElapsedTime * m_DefaultGamepad.fY2;
    if( theta2 < +0.9f ) theta2 = +0.9f;
    if( theta2 > +2.3f ) theta2 = +2.3f;

    g_vDirLightDirection.x = -2*cosf( phi2 );
    g_vDirLightDirection.y = -2*cosf( theta2 );
    g_vDirLightDirection.z = -2*sinf( phi2 ) * sinf( theta2 );
    D3DXVec3Normalize( &g_vDirLightDirection, &g_vDirLightDirection );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawLight()
{
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

    // Get the inverse of the world matrix
    D3DXMATRIX matInvWorld;
    D3DXMatrixInverse( &matInvWorld, NULL, &m_matWorld );

    // Setup some points to draw crosshairs
    D3DXVECTOR3 line[6];
    D3DXVec3TransformCoord( &line[0], &(g_vPtLightPos+D3DXVECTOR3(+0.2f, 0.0f, 0.0f)), &matInvWorld );
    D3DXVec3TransformCoord( &line[1], &(g_vPtLightPos+D3DXVECTOR3(-0.2f, 0.0f, 0.0f)), &matInvWorld );
    D3DXVec3TransformCoord( &line[2], &(g_vPtLightPos+D3DXVECTOR3( 0.0f,+0.2f, 0.0f)), &matInvWorld );
    D3DXVec3TransformCoord( &line[3], &(g_vPtLightPos+D3DXVECTOR3( 0.0f,-0.2f, 0.0f)), &matInvWorld );
    D3DXVec3TransformCoord( &line[4], &(g_vPtLightPos+D3DXVECTOR3( 0.0f, 0.0f,+0.2f)), &matInvWorld );
    D3DXVec3TransformCoord( &line[5], &(g_vPtLightPos+D3DXVECTOR3( 0.0f, 0.0f,-0.2f)), &matInvWorld );

    // Set the crosshair's color 
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, g_dwPtLightColor );

    // Draw the crosshairs
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZ );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 3, line, sizeof(D3DXVECTOR3) );
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
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff000000, 0xff888844 );

    // Draw the main object
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_BumpyObject.Render( m_pd3dDevice );

    // Draw the position of the point light
    if( g_bEnablePointLightPass )
        DrawLight();

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"PerPixelLightingVS" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CBumpyObject::CBumpyObject()
{
    m_pd3dDevice        = NULL;
    m_pTangentSpaceVB   = NULL;
    m_pSphereVerticesVB = NULL;
    m_pSphereIndicesIB  = NULL;
    m_pBaseTexture      = NULL;
    m_pNormalMap        = NULL;
    m_pCubeMap          = NULL;
    m_dwVertexShader    = 0L;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CBumpyObject::~CBumpyObject()
{
    SAFE_RELEASE( m_pTangentSpaceVB );
    SAFE_RELEASE( m_pSphereVerticesVB );
    SAFE_RELEASE( m_pSphereIndicesIB );
    SAFE_RELEASE( m_pBaseTexture );
    SAFE_RELEASE( m_pNormalMap );
    SAFE_RELEASE( m_pCubeMap );
    
    if( m_pd3dDevice )
        m_pd3dDevice->DeleteVertexShader( m_dwVertexShader );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CBumpyObject::Init( LPDIRECT3DDEVICE8 pd3dDevice, 
                            CXBPackedResource* pResource )
{
    HRESULT hr;

    // Keep track of the device
    m_pd3dDevice = pd3dDevice;

    // Initialize the sphere's geometry, and create it's basis matrices
    InitSphere( 1.7f, 24, 24 );
    CreateBasisMatrices();

    // Create the base texture
    m_pBaseTexture = pResource->GetTexture( resource_Earth_OFFSET );
    
    // Load grayscale texture to be used for making the normal map
    m_pNormalMap = pResource->GetTexture( resource_EarthBmp_OFFSET );

    // Compute the normal map from the gray scale texture
    D3DSURFACE_DESC desc;
    D3DLOCKED_RECT  lock;
    m_pNormalMap->GetLevelDesc( 0, &desc );
    m_pNormalMap->LockRect( 0, &lock, 0, 0L );
    XBUtil_UnswizzleTexture2D( &lock, &desc );
    DWORD* pDstBits = (DWORD*)lock.pBits;
    DWORD* pSrcBits = new DWORD[ desc.Height * desc.Width ];
    memcpy( pSrcBits, pDstBits, sizeof(DWORD) * desc.Height * desc.Width );

    for( DWORD y=0; y<desc.Height; y++ )
    {
        for( DWORD x=0; x<desc.Width; x++ )
        {
            DWORD x0 = x,   x1 = (x+1<desc.Width)  ? x+1 : 0;
            DWORD y0 = y,   y1 = (y+1<desc.Height) ? y+1 : 0;

            DWORD* p00 = ((DWORD*)pSrcBits) + x0 + desc.Width*y0;
            DWORD* p10 = ((DWORD*)pSrcBits) + x1 + desc.Width*y0;
            DWORD* p01 = ((DWORD*)pSrcBits) + x0 + desc.Width*y1;

            FLOAT fHeight00 = (FLOAT)(((*p00)&0x00ff0000)>>16)/255.0f;
            FLOAT fHeight10 = (FLOAT)(((*p10)&0x00ff0000)>>16)/255.0f;
            FLOAT fHeight01 = (FLOAT)(((*p01)&0x00ff0000)>>16)/255.0f;

            D3DXVECTOR3 vPoint00( x+0.0f, y+0.0f, fHeight00 );
            D3DXVECTOR3 vPoint10( x+0.1f, y+0.0f, fHeight10 );
            D3DXVECTOR3 vPoint01( x+0.0f, y+0.1f, fHeight01 );
            D3DXVECTOR3 v10 = vPoint10 - vPoint00;
            D3DXVECTOR3 v01 = vPoint01 - vPoint00;

            D3DXVECTOR3 v;
            D3DXVec3Cross( &v, &v10, &v01 );
            D3DXVec3Normalize( &v, &v );

            *pDstBits++ = XBUtil_VectorToRGBA( &v );
        }
    }
    delete[] pSrcBits;
    XBUtil_SwizzleTexture2D( &lock, &desc );
    m_pNormalMap->UnlockRect( 0 );

    // Create the normalization cube map
    hr = XBUtil_CreateNormalizationCubeMap( m_pd3dDevice, 256, &m_pCubeMap );
    if( FAILED(hr) )
        return E_FAIL;

    // Create a vertex shader
    DWORD dwVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),     // v0 = Position
        D3DVSD_REG( 1, D3DVSDT_FLOAT3 ),     // v1 = Normal
        D3DVSD_REG( 2, D3DVSDT_FLOAT2 ),     // v2 = Base tex coords
        D3DVSD_STREAM( 1 ),
        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ),     // v3 = Tangent space tangent
        D3DVSD_REG( 4, D3DVSDT_FLOAT3 ),     // v4 = Tangent space binormal
        D3DVSD_REG( 5, D3DVSDT_FLOAT3 ),     // v5 = Tangent space normal
        D3DVSD_END()
    };

    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\PPLight.xvu",
                                           dwVertexDecl,
                                           &m_dwVertexShader ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitSphere()
// Desc: 
//-----------------------------------------------------------------------------
VOID CBumpyObject::InitSphere( FLOAT fRadius, DWORD dwNumSphereRings, 
                               DWORD dwNumSphereSegments )
{
    // Establish constants used in sphere generation
    FLOAT fDeltaRingAngle = ( D3DX_PI / dwNumSphereRings );
    FLOAT fDeltaSegAngle  = ( 2.0f * D3DX_PI / dwNumSphereSegments );

    m_dwNumVertices = dwNumSphereRings*(dwNumSphereSegments+1)*2;
    m_dwNumIndices  = 3*(m_dwNumVertices-2);

    // Create the vertex buffer and fill it
    m_pd3dDevice->CreateVertexBuffer( m_dwNumVertices*sizeof(CUSTOMVERTEX),
                                      D3DUSAGE_WRITEONLY, 0L,
                                      D3DPOOL_MANAGED, &m_pSphereVerticesVB );

    CUSTOMVERTEX* pVertices;
    m_pSphereVerticesVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

    // Generate the group of rings for the sphere
    for( DWORD ring = 0; ring < dwNumSphereRings; ring++ )
    {
        FLOAT r0 = sinf( (ring+0) * fDeltaRingAngle );
        FLOAT r1 = sinf( (ring+1) * fDeltaRingAngle );
        FLOAT y0 = cosf( (ring+0) * fDeltaRingAngle );
        FLOAT y1 = cosf( (ring+1) * fDeltaRingAngle );

        // Generate the group of segments for the current ring
        for( DWORD seg = 0; seg < (dwNumSphereSegments+1); seg++ )
        {
            FLOAT x0 =  r0 * sinf( seg * fDeltaSegAngle );
            FLOAT z0 =  r0 * cosf( seg * fDeltaSegAngle );
            FLOAT x1 =  r1 * sinf( seg * fDeltaSegAngle );
            FLOAT z1 =  r1 * cosf( seg * fDeltaSegAngle );

            // Add two vertices to the strip which makes up the sphere
            // (using the transformed normal to generate texture coords)
            pVertices->p  = fRadius * D3DXVECTOR3(x0,y0,z0);
            pVertices->n  = D3DXVECTOR3(x0,y0,z0);
            pVertices->tu = -((FLOAT)seg)/dwNumSphereSegments;
            pVertices->tv = (ring+0)/(FLOAT)dwNumSphereRings;
            pVertices++;

            pVertices->p  = fRadius * D3DXVECTOR3(x1,y1,z1);
            pVertices->n  = D3DXVECTOR3(x1,y1,z1);
            pVertices->tu = -((FLOAT)seg)/dwNumSphereSegments;
            pVertices->tv = (ring+1)/(FLOAT)dwNumSphereRings;
            pVertices++;
        }
    }

    m_pSphereVerticesVB->Unlock();

    // Create the index buffer and fill it
    m_pd3dDevice->CreateIndexBuffer( m_dwNumIndices*sizeof(WORD),
                                     D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
                                     D3DPOOL_MANAGED, &m_pSphereIndicesIB );

    WORD* pIndices;
    m_pSphereIndicesIB->Lock( 0, 0, (BYTE**)&pIndices, 0 );

    for( DWORD i=0; i<m_dwNumVertices-2; i++ )
    {
        (*pIndices++) = (WORD)(i+0);
        (*pIndices++) = (WORD)(i+1+(i%2));
        (*pIndices++) = (WORD)(i+2-(i%2));
    }

    m_pSphereIndicesIB->Unlock();
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
VOID CBumpyObject::CreateBasisMatrices()
{
    WORD i,j;

    m_pd3dDevice->CreateVertexBuffer( m_dwNumVertices*sizeof(TANGENTSPACE),
                                      D3DUSAGE_WRITEONLY, 0L,
                                      D3DPOOL_MANAGED, &m_pTangentSpaceVB );


    TANGENTSPACE* pTangentSpace;
    CUSTOMVERTEX* pVertices;
    WORD*         pIndices;
    m_pTangentSpaceVB->Lock( 0, 0, (BYTE**)&pTangentSpace, 0 );
    m_pSphereVerticesVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );
    m_pSphereIndicesIB->Lock( 0, 0, (BYTE**)&pIndices, 0 );

    // Clear out the tangent space area
    ZeroMemory( pTangentSpace, m_dwNumVertices*sizeof(TANGENTSPACE) );

    // Loop through all triangles, accumulating du and dv offsets to build
    // basis vectors
    for( i = 0; i < m_dwNumIndices; i += 3 )
    {       
        WORD i0 = pIndices[i+0];
        WORD i1 = pIndices[i+1];
        WORD i2 = pIndices[i+2];

        if( i0<m_dwNumVertices && i1<m_dwNumVertices  && i2<m_dwNumVertices )
        { 
            CUSTOMVERTEX* v0 = &pVertices[i0];
            CUSTOMVERTEX* v1 = &pVertices[i1];
            CUSTOMVERTEX* v2 = &pVertices[i2];
            D3DXVECTOR3   du, dv;
            D3DXVECTOR3   cp;

            // Skip degnerate triangles
            if( fabs(v0->p.x-v1->p.x)<1e-6 && fabs(v0->p.y-v1->p.y)<1e-6 && fabs(v0->p.z-v1->p.z)<1e-6 )
                continue;
            if( fabs(v1->p.x-v2->p.x)<1e-6 && fabs(v1->p.y-v2->p.y)<1e-6 && fabs(v1->p.z-v2->p.z)<1e-6 )
                continue;
            if( fabs(v2->p.x-v0->p.x)<1e-6 && fabs(v2->p.y-v0->p.y)<1e-6 && fabs(v2->p.z-v0->p.z)<1e-6 )
                continue;

            D3DXVECTOR3 edge01( v1->p.x - v0->p.x, v1->tu - v0->tu, v1->tv - v0->tv );
            D3DXVECTOR3 edge02( v2->p.x - v0->p.x, v2->tu - v0->tu, v2->tv - v0->tv );
            D3DXVec3Cross( &cp, &edge01, &edge02 );
            if( fabs(cp.x) > 1e-8 )
            {
                du.x = -cp.y / cp.x;        
                dv.x = -cp.z / cp.x;
            }

            edge01 = D3DXVECTOR3( v1->p.y - v0->p.y, v1->tu - v0->tu, v1->tv - v0->tv );
            edge02 = D3DXVECTOR3( v2->p.y - v0->p.y, v2->tu - v0->tu, v2->tv - v0->tv );
            D3DXVec3Cross( &cp, &edge01, &edge02 );
            if( fabs(cp.x) > 1e-8 )
            {
                du.y = -cp.y / cp.x;
                dv.y = -cp.z / cp.x;
            }

            edge01 = D3DXVECTOR3( v1->p.z - v0->p.z, v1->tu - v0->tu, v1->tv - v0->tv );
            edge02 = D3DXVECTOR3( v2->p.z - v0->p.z, v2->tu - v0->tu, v2->tv - v0->tv );
            D3DXVec3Cross( &cp, &edge01, &edge02 );
            if( fabs(cp.x) > 1e-8 )
            {
                du.z = -cp.y / cp.x;
                dv.z = -cp.z / cp.x;
            }

            pTangentSpace[i0].vTangent += du;
            pTangentSpace[i1].vTangent += du;
            pTangentSpace[i2].vTangent += du;

            pTangentSpace[i0].vNormal  += dv;
            pTangentSpace[i1].vNormal  += dv;
            pTangentSpace[i2].vNormal  += dv;
        }
    }

    for( i = 0; i < m_dwNumVertices; i++)
    {       
        // vBinormal = vTangent x vNormal
        D3DXVec3Normalize( &pTangentSpace[i].vTangent, &pTangentSpace[i].vTangent );
        D3DXVec3Normalize( &pTangentSpace[i].vNormal,  &pTangentSpace[i].vNormal );
        D3DXVec3Cross( &pTangentSpace[i].vBinormal, &pTangentSpace[i].vTangent, 
                       &pTangentSpace[i].vNormal );

        // Get the vertex normal (make sure it's normalized)
        D3DXVECTOR3 normal;
        D3DXVec3Normalize( &normal, &pVertices[i].n );

        // Make sure the basis vector and normal point in the same direction
        if( D3DXVec3Dot( &pTangentSpace[i].vBinormal, &normal ) < 0.0f )
            pTangentSpace[i].vBinormal = -pTangentSpace[i].vBinormal;
    }

    // Find duplicate vertices in the mesh, and average their tangent spaces
    // together. This is necessary to avoid discontinuities at the seams.
    for( i=0; i < m_dwNumVertices; i++ )
    {
        D3DXVECTOR3 vT = pTangentSpace[i].vTangent;
        D3DXVECTOR3 vB = pTangentSpace[i].vBinormal;
        D3DXVECTOR3 vN = pTangentSpace[i].vNormal;

        for( j=i+1; j < m_dwNumVertices; j++ )
        {
            FLOAT dist = D3DXVec3LengthSq( &(pVertices[i].p - pVertices[j].p) );

            if( dist < 1.0e-8f )
            {
                vT += pTangentSpace[j].vTangent;
                vB += pTangentSpace[j].vBinormal;
                vN += pTangentSpace[j].vNormal;
            }
        }

        // Normalize the vectors of the basis matrix
        D3DXVec3Normalize( &vT, &vT );
        D3DXVec3Normalize( &vB, &vB );
        D3DXVec3Normalize( &vN, &vN );

        for( j=i; j < m_dwNumVertices; j++ )
        {
            FLOAT dist = D3DXVec3LengthSq( &(pVertices[i].p - pVertices[j].p) );

            if( dist < 1.0e-8f )
            {
                pTangentSpace[j].vTangent  = vT;
                pTangentSpace[j].vBinormal = vB;
                pTangentSpace[j].vNormal   = vN;
            }
        }
    }

    m_pTangentSpaceVB->Unlock();
    m_pSphereVerticesVB->Unlock();
    m_pSphereIndicesIB->Unlock();
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CBumpyObject::RenderObject( LPDIRECT3DDEVICE8 m_pd3dDevice )
{
    // Render the object
    m_pd3dDevice->SetVertexShader( m_dwVertexShader ); 
    m_pd3dDevice->SetStreamSource( 0, m_pSphereVerticesVB, sizeof(CUSTOMVERTEX) );
    m_pd3dDevice->SetStreamSource( 1, m_pTangentSpaceVB, sizeof(TANGENTSPACE) );
    m_pd3dDevice->SetIndices( m_pSphereIndicesIB, 0 );
    m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumVertices,
                                        0, m_dwNumIndices/3 );
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CBumpyObject::Render( LPDIRECT3DDEVICE8 m_pd3dDevice )
{
    m_pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,      TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         FALSE );              
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );        
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );        

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    // Process the vertices for the perpixel lighting effect.
    ProcessVertices( &g_vDirLightDirection, &g_vPtLightPos );

    // Render the ambient lighting
    if( g_bEnableAmbientLightPass )
    {
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, g_dwAmbientColor );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        RenderObject( m_pd3dDevice );
    }

    // Set blending to add the upcoming passes to the previous pass
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );  
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    // Render the bumps lit by the directional light. The normal from the
    // normal map is dotproducted with the directional light vector, which
    // was stored in the vertex's diffuse component during vertex processing.
    // The second texture stage is used to modulate the result with the 
    // directional light's color.
    if( g_bEnableDirLightPass )
    {
        m_pd3dDevice->SetTexture( 0, m_pNormalMap );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DOTPRODUCT3 );
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, g_dwDirLightColor );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TFACTOR );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        RenderObject( m_pd3dDevice );
    }

    // Set blending to add the upcoming passes to the previous pass
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );  
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    // Render the bumps lit by the point light. This normalizes the
    // normal from the normal map, by dotproducting it with the
    // normalization cubemap
    if( g_bEnablePointLightPass )
    {
        m_pd3dDevice->SetTexture( 0, m_pNormalMap );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTexture( 1, m_pCubeMap );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DOTPRODUCT3 );
        RenderObject( m_pd3dDevice );
    }

    // Set blending to blend in the base texture
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ); 
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_DESTCOLOR );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_ZERO );

    // Finally, render the object with the base texture
    if( g_bEnableBaseTexturePass )
    {
        m_pd3dDevice->SetTexture( 0, m_pBaseTexture );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        RenderObject( m_pd3dDevice );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ProcessVertices()
// Desc: 
//-----------------------------------------------------------------------------
VOID CBumpyObject::ProcessVertices( D3DXVECTOR3* pDirLightDir, 
                                    D3DXVECTOR3* pPtLightPos )
{
    // Compute the matrix set
    D3DXMATRIX matMatrixSet, matWorld, matView, matProj;
    m_pd3dDevice->GetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->GetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
    D3DXMatrixMultiply( &matMatrixSet, &matWorld, &matView );
    D3DXMatrixMultiply( &matMatrixSet, &matMatrixSet, &matProj );
    D3DXMatrixTranspose( &matMatrixSet, &matMatrixSet );
    
    // Get inverse of world matrix
    D3DXMATRIX matInvWorld;
    D3DXMatrixInverse( &matInvWorld, NULL, &matWorld );

    // Transform point light position into object space
    D3DXVECTOR3 vPtLightWorldPos;
    D3DXVec3TransformCoord( &vPtLightWorldPos, pPtLightPos, &matInvWorld );

    // Transform directional light direction into object space
    matInvWorld._41 = matInvWorld._42 = matInvWorld._43 = 0;
    D3DXVECTOR3 vDirLightWorldDir;
    D3DXVec3TransformCoord( &vDirLightWorldDir, &(-(*pDirLightDir)), &matInvWorld );
    D3DXVec3Normalize( &vDirLightWorldDir, &vDirLightWorldDir );

    // If we are using a vertex shader, we simply pass variables to
    // the vertex shader, and the vertex shader will do the rest.
    D3DXVECTOR4 vConstants0( 0.0f, 0.0f, 0.0f, 0.0f );
    D3DXVECTOR4 vConstants1( 0.0f, 0.5f, 1.0f, 2.0f );
    m_pd3dDevice->SetVertexShaderConstant(  0, &vConstants0,       1 );
    m_pd3dDevice->SetVertexShaderConstant(  1, &vConstants1,       1 );
    m_pd3dDevice->SetVertexShaderConstant(  4, &matMatrixSet,      4 );
    m_pd3dDevice->SetVertexShaderConstant( 10, &vDirLightWorldDir, 1 );
    m_pd3dDevice->SetVertexShaderConstant( 11, &vPtLightWorldPos,  1 );
}



