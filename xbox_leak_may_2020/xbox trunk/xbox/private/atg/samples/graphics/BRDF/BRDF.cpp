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
    LPDIRECT3DDEVICE8        m_pd3dDevice;        // Local copy of the d3d device

    LPDIRECT3DVERTEXBUFFER8  m_pTangentSpaceVB;   // Hold tangent space vectors
    LPDIRECT3DVERTEXBUFFER8  m_pSphereVerticesVB; // Hold geometry
    LPDIRECT3DINDEXBUFFER8   m_pSphereIndicesIB;
    DWORD                    m_dwNumVertices;
    DWORD                    m_dwNumIndices;

    LPDIRECT3DTEXTURE8       m_pBaseTexture;          // Base texture
    LPDIRECT3DTEXTURE8       m_pNormalMap;            // Normal texture (bumpmap)
    LPDIRECT3DCUBETEXTURE8   m_pNormalizationCubeMap; // Normalization cubemap
    LPDIRECT3DCUBETEXTURE8   m_pAngularizingCubeMap1; // Angularizing cubemap
    LPDIRECT3DCUBETEXTURE8   m_pAngularizingCubeMap2; // Angularizing cubemap
    LPDIRECT3DVOLUMETEXTURE8 m_pBRDFVolumeTexture;

    DWORD                    m_dwVertexShader;    // Custom vertex shader
    DWORD                    m_dwBRDFPixelShader;

    HRESULT CreateBRDFPixelShader();

public:
    CBumpyObject();
    virtual ~CBumpyObject();

    HRESULT Init( LPDIRECT3DDEVICE8 pd3dDevice, CXBPackedResource* pResource );
    VOID    InitSphere( FLOAT radius, DWORD nLat, DWORD nLong );
    VOID    CreateBasisMatrices();

    VOID    SetVertexShaderConstants( D3DXVECTOR3* pLightPos );

    HRESULT Render( LPDIRECT3DDEVICE8 m_pd3dDevice );
    HRESULT RenderObject( LPDIRECT3DDEVICE8 m_pd3dDevice );
};




//-----------------------------------------------------------------------------
// Globally accessed attributes
//-----------------------------------------------------------------------------
D3DXVECTOR3 g_vPtLightPos;        // Point light position
DWORD       g_dwPtLightColor;     // Point light color
DWORD       g_dwAmbientColor;     // Ambient light value

BOOL        g_bPointFiltering = FALSE;



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

    // Toggle options

    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
        g_bPointFiltering          = !g_bPointFiltering;
    

    // Adjust the point light's position
    static FLOAT fLightTheta = 0.0f;
    fLightTheta += 3.0f * m_fElapsedTime * m_DefaultGamepad.fX1;
    if( fLightTheta < -3.14f ) fLightTheta = -3.14f;
    if( fLightTheta > +0.00f ) fLightTheta = +0.00f;

    static FLOAT fLightPhi = 0.0f;
    fLightPhi -= 3.0f * m_fElapsedTime * m_DefaultGamepad.fY1;
    if( fLightPhi < +0.00f ) fLightPhi = +0.0f;
    if( fLightPhi > +3.14f ) fLightPhi = +3.14f;

    g_vPtLightPos.x = 2*cosf( fLightTheta );
    g_vPtLightPos.y = 2*cosf( fLightPhi );
    g_vPtLightPos.z = 2*sinf( fLightTheta ) * sinf( fLightPhi );

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
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff000000, 0xff888844 );

    // Draw the main object
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_BumpyObject.Render( m_pd3dDevice );

    // Draw the position of the point light
    DrawLight();

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  48, 36, 0xffffffff, L"BRDF" );
        m_Font.DrawText( 464, 36, 0xffffff00, m_strFrameRate );
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
    m_pNormalizationCubeMap = NULL;
    m_pAngularizingCubeMap1 = NULL;
    m_pAngularizingCubeMap2 = NULL;
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
    SAFE_RELEASE( m_pNormalizationCubeMap );
    SAFE_RELEASE( m_pAngularizingCubeMap1 );
    SAFE_RELEASE( m_pAngularizingCubeMap2 );
    
    if( m_pd3dDevice )
        m_pd3dDevice->DeleteVertexShader( m_dwVertexShader );
}




//-----------------------------------------------------------------------------
// Name: XBUtil_CreateAngularizingCubeMap()
// Desc: Creates a cubemap and fills it with normalized RGBA vectors
//-----------------------------------------------------------------------------
HRESULT XBUtil_CreateAngularizingCubeMap1( LPDIRECT3DDEVICE8 pd3dDevice, 
                                          DWORD dwSize, 
                                          LPDIRECT3DCUBETEXTURE8* ppCubeMap )
{
    HRESULT hr;

    // Create the cube map. Texture format doesn't appear to matter, as long as
    // it's 32-bit
//    if( FAILED( hr = pd3dDevice->CreateCubeTexture( dwSize, 1, 0, D3DFMT_V16U16, 
    if( FAILED( hr = pd3dDevice->CreateCubeTexture( dwSize, 1, 0, D3DFMT_A8R8G8B8, 
                                                    D3DPOOL_DEFAULT, ppCubeMap ) ) )
        return E_FAIL;
    
    // Allocate temp space for swizzling the cubemap surfaces
    DWORD* pSourceBits = new DWORD[ dwSize * dwSize ];

    // Fill all six sides of the cubemap
    for( DWORD i=0; i<6; i++ )
    {
        // Lock the i'th cubemap surface
        LPDIRECT3DSURFACE8 pCubeMapFace;
        (*ppCubeMap)->GetCubeMapSurface( (D3DCUBEMAP_FACES)i, 0, &pCubeMapFace );

        // Write the RGBA-encoded normals to the surface pixels
        WORD*       pPixel = (WORD*)pSourceBits;
//        BYTE*       pPixel = (BYTE*)pSourceBits;
        D3DXVECTOR3 n;
        FLOAT       w, h;

        for( DWORD y = 0; y < dwSize; y++ )
        {
            h  = (FLOAT)y / (FLOAT)(dwSize-1);  // 0 to 1
            h  = ( h * 2.0f ) - 1.0f;           // -1 to 1
            
            for( DWORD x = 0; x < dwSize; x++ )
            {
                w = (FLOAT)x / (FLOAT)(dwSize-1);   // 0 to 1
                w = ( w * 2.0f ) - 1.0f;            // -1 to 1

                // Calc the normal for this texel
                switch( i )
                {
                    case D3DCUBEMAP_FACE_POSITIVE_X:    // +x
                        n.x = +1.0;
                        n.y = -h;
                        n.z = -w;
                        break;
                        
                    case D3DCUBEMAP_FACE_NEGATIVE_X:    // -x
                        n.x = -1.0;
                        n.y = -h;
                        n.z = +w;
                        break;
                        
                    case D3DCUBEMAP_FACE_POSITIVE_Y:    // y
                        n.x = +w;
                        n.y = +1.0;
                        n.z = +h;
                        break;
                        
                    case D3DCUBEMAP_FACE_NEGATIVE_Y:    // -y
                        n.x = +w;
                        n.y = -1.0;
                        n.z = -h;
                        break;
                        
                    case D3DCUBEMAP_FACE_POSITIVE_Z:    // +z
                        n.x = +w;
                        n.y = -h;
                        n.z = +1.0;
                        break;
                        
                    case D3DCUBEMAP_FACE_NEGATIVE_Z:    // -z
                        n.x = -w;
                        n.y = -h;
                        n.z = -1.0;
                        break;
                }

                // Convert the normal to spherical coordinates
                D3DXVec3Normalize( &n, &n );
                FLOAT fTheta = atan2f( n.y, n.x ); // Returns range [-pi, +pi]
                FLOAT fPhi   = acosf( n.z );       // Returns range [0, pi]

                // Write the pixel in HiLo format
                {
                    *pPixel++ = (WORD)(65535.0f * (fPhi/(2*D3DX_PI)));
//                  *pPixel++ = (WORD)(65535.0f * ((fTheta+D3DX_PI)/(2*D3DX_PI)));
//                  *pPixel++ = (WORD)0;
                    *pPixel++ = (WORD)0;
                }
            }
        }
        
        // Swizzle the result into the cubemap face surface
        D3DLOCKED_RECT lock;
        pCubeMapFace->LockRect( &lock, 0, 0L );
        XGSwizzleRect( pSourceBits, 0, NULL, lock.pBits, dwSize, dwSize,
                       NULL, sizeof(DWORD) );
        pCubeMapFace->UnlockRect();

        // Release the cubemap face
        pCubeMapFace->Release();
    }

    // Free temp space
    SAFE_DELETE_ARRAY( pSourceBits );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: XBUtil_CreateAngularizingCubeMap()
// Desc: Creates a cubemap and fills it with normalized RGBA vectors
//-----------------------------------------------------------------------------
HRESULT XBUtil_CreateAngularizingCubeMap2( LPDIRECT3DDEVICE8 pd3dDevice, 
                                          DWORD dwSize, 
                                          LPDIRECT3DCUBETEXTURE8* ppCubeMap )
{
    HRESULT hr;

    // Create the cube map
    if( FAILED( hr = pd3dDevice->CreateCubeTexture( dwSize, 1, 0, D3DFMT_V16U16, 
//    if( FAILED( hr = pd3dDevice->CreateCubeTexture( dwSize, 1, 0, D3DFMT_A8R8G8B8, 
                                                    D3DPOOL_DEFAULT, ppCubeMap ) ) )
        return E_FAIL;
    
    // Allocate temp space for swizzling the cubemap surfaces
    DWORD* pSourceBits = new DWORD[ dwSize * dwSize ];

    // Fill all six sides of the cubemap
    for( DWORD i=0; i<6; i++ )
    {
        // Lock the i'th cubemap surface
        LPDIRECT3DSURFACE8 pCubeMapFace;
        (*ppCubeMap)->GetCubeMapSurface( (D3DCUBEMAP_FACES)i, 0, &pCubeMapFace );

        // Write the RGBA-encoded normals to the surface pixels
        WORD*       pPixel = (WORD*)pSourceBits;
//        BYTE*       pPixel = (BYTE*)pSourceBits;
        D3DXVECTOR3 n;
        FLOAT       w, h;

        for( DWORD y = 0; y < dwSize; y++ )
        {
            h  = (FLOAT)y / (FLOAT)(dwSize-1);  // 0 to 1
            h  = ( h * 2.0f ) - 1.0f;           // -1 to 1
            
            for( DWORD x = 0; x < dwSize; x++ )
            {
                w = (FLOAT)x / (FLOAT)(dwSize-1);   // 0 to 1
                w = ( w * 2.0f ) - 1.0f;            // -1 to 1

                // Calc the normal for this texel
                switch( i )
                {
                    case D3DCUBEMAP_FACE_POSITIVE_X:    // +x
                        n.x = +1.0;
                        n.y = -h;
                        n.z = -w;
                        break;
                        
                    case D3DCUBEMAP_FACE_NEGATIVE_X:    // -x
                        n.x = -1.0;
                        n.y = -h;
                        n.z = +w;
                        break;
                        
                    case D3DCUBEMAP_FACE_POSITIVE_Y:    // y
                        n.x = +w;
                        n.y = +1.0;
                        n.z = +h;
                        break;
                        
                    case D3DCUBEMAP_FACE_NEGATIVE_Y:    // -y
                        n.x = +w;
                        n.y = -1.0;
                        n.z = -h;
                        break;
                        
                    case D3DCUBEMAP_FACE_POSITIVE_Z:    // +z
                        n.x = +w;
                        n.y = -h;
                        n.z = +1.0;
                        break;
                        
                    case D3DCUBEMAP_FACE_NEGATIVE_Z:    // -z
                        n.x = -w;
                        n.y = -h;
                        n.z = -1.0;
                        break;
                }

                // Convert the normal to spherical coordinates
                D3DXVec3Normalize( &n, &n );
                FLOAT fTheta = atan2f( n.y, n.x ); // Returns range [-pi, +pi]
                FLOAT fPhi   = acosf( n.z );       // Returns range [0, pi]

                // Write the pixel in HiLo format
                {
                    *pPixel++ = (WORD)(65535.0f * (fPhi/D3DX_PI));
//                  *pPixel++ = (WORD)(65535.0f * ((fTheta+D3DX_PI)/(2*D3DX_PI)));
                    *pPixel++ = (WORD)0;
                }
            }
        }
        
        // Swizzle the result into the cubemap face surface
        D3DLOCKED_RECT lock;
        pCubeMapFace->LockRect( &lock, 0, 0L );
        XGSwizzleRect( pSourceBits, 0, NULL, lock.pBits, dwSize, dwSize,
                       NULL, sizeof(DWORD) );
        pCubeMapFace->UnlockRect();

        // Release the cubemap face
        pCubeMapFace->Release();
    }

    // Free temp space
    SAFE_DELETE_ARRAY( pSourceBits );

    return S_OK;
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
    m_pBaseTexture = pResource->GetTexture( resource_Woman_OFFSET );
    
    // Load grayscale texture to be used for making the normal map
    m_pNormalMap = pResource->GetTexture( resource_Woman_OFFSET );

    // Compute the normal map from the gray scale texture
    D3DSURFACE_DESC desc;
    D3DLOCKED_RECT  lock;
    m_pNormalMap->GetLevelDesc( 0, &desc );
    m_pNormalMap->LockRect( 0, &lock, 0, 0L );
    XBUtil_UnswizzleTexture2D( &lock, &desc );
    DWORD* pBits = (DWORD*)lock.pBits;

    for( DWORD y=0; y<desc.Height; y++ )
    {
        for( DWORD x=0; x<desc.Width; x++ )
        {
            DWORD* p00 = ((DWORD*)pBits) + (x+0) + desc.Width*(y+0);
            DWORD* p10 = ((DWORD*)pBits) + (x+1) + desc.Width*(y+0);
            DWORD* p01 = ((DWORD*)pBits) + (x+0) + desc.Width*(y+1);

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

            *p00 = XBUtil_VectorToRGBA( &v );
        }
    }

    XBUtil_SwizzleTexture2D( &lock, &desc );
    m_pNormalMap->UnlockRect( 0 );

    // Create the normalization cube map
    hr = XBUtil_CreateNormalizationCubeMap( m_pd3dDevice, 256, &m_pNormalizationCubeMap );
    if( FAILED(hr) )
        return E_FAIL;

    // Create the angularizing cube map
    hr = XBUtil_CreateAngularizingCubeMap1( m_pd3dDevice, 256, &m_pAngularizingCubeMap1 );
    if( FAILED(hr) )
        return E_FAIL;

    // Create the angularizing cube map
    hr = XBUtil_CreateAngularizingCubeMap2( m_pd3dDevice, 256, &m_pAngularizingCubeMap2 );
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

    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\BRDF.xvu",
                                           dwVertexDecl,
                                           &m_dwVertexShader ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( CreateBRDFPixelShader() ) )
        return E_FAIL;



    const DWORD VOLTEXSIZE = 64;

    if( FAILED( m_pd3dDevice->CreateVolumeTexture( VOLTEXSIZE, VOLTEXSIZE, VOLTEXSIZE, 
                                                   1, 0, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, 
                                                   &m_pBRDFVolumeTexture ) ) )
        return E_FAIL;

    // Fill the volume texture
    {
        D3DVOLUME_DESC  desc;
        D3DLOCKED_BOX   lock;
        m_pBRDFVolumeTexture->GetLevelDesc( 0, &desc );
        m_pBRDFVolumeTexture->LockBox( 0, &lock, 0, 0L );
        DWORD* pBits = (DWORD*)lock.pBits;

        srand( 123456 );

        for( UINT w=0; w<VOLTEXSIZE; w++ )
        {
            for( UINT v=0; v<VOLTEXSIZE; v++ )
            {
                for( UINT u=0; u<VOLTEXSIZE; u++ )
                {
                    FLOAT du = (2.0f*u)/(VOLTEXSIZE-1) - 1.0f;
                    FLOAT dv = (2.0f*v)/(VOLTEXSIZE-1) - 1.0f;
                    FLOAT dw = (2.0f*w)/(VOLTEXSIZE-1) - 1.0f;

                    FLOAT fu = (1.0f*u)/(VOLTEXSIZE-1);
                    FLOAT fv = (1.0f*v)/(VOLTEXSIZE-1);
                    FLOAT fw = (1.0f*w)/(VOLTEXSIZE-1);
                
                    FLOAT fScale = ((FLOAT)v)/(VOLTEXSIZE-1);

                    DWORD r = (DWORD)(0xff*fu);
                    DWORD g = (DWORD)(0xff*fv);
                    DWORD b = (DWORD)(0xff*fw);
                    DWORD a = 0xff;

                    // Write the texel
                    *pBits++ = (a<<24) + (r<<16) + (g<<8) + (b<<0);
                }
            }
        }

        // Swizzle and unlock the texture
        XBUtil_SwizzleTexture3D( &lock, &desc );
        m_pBRDFVolumeTexture->UnlockBox( 0 );
    }

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
// Name: SetVertexShaderConstants()
// Desc: 
//-----------------------------------------------------------------------------
VOID CBumpyObject::SetVertexShaderConstants( D3DXVECTOR3* pPtLightPos )
{
    // Compute the matrix set
    D3DXMATRIX matWorldViewProj, matWorldView, matWorld, matView, matProj;
    m_pd3dDevice->GetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->GetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
    D3DXMatrixMultiply( &matWorldView, &matWorld, &matView );
    D3DXMatrixMultiply( &matWorldViewProj, &matWorldView, &matProj );
    
    // Get inverse of world matrix
    D3DXMATRIX matInvWorld;
    D3DXMatrixInverse( &matInvWorld, NULL, &matWorld );

    // Transform point light position into object space
    D3DXVECTOR3 vPtLightWorldPos;
    D3DXVec3TransformCoord( &vPtLightWorldPos, pPtLightPos, &matInvWorld );

    // Transform eye position into object space
    D3DXVECTOR3 vPtEyePos( -matView._41, -matView._42, -matView._43 );
    D3DXVECTOR3 vPtEyeWorldPos;
    D3DXVec3TransformCoord( &vPtEyeWorldPos, &vPtEyePos, &matInvWorld );

    // If we are using a vertex shader, we simply pass variables to
    // the vertex shader, and the vertex shader will do the rest.
    D3DXMatrixTranspose( &matWorld, &matWorld );
    D3DXMatrixTranspose( &matWorldView, &matWorldView );
    D3DXMatrixTranspose( &matWorldViewProj, &matWorldViewProj );
    D3DXVECTOR4 vConstants0( 0.0f, 0.0f, 0.0f, 0.0f );
    D3DXVECTOR4 vConstants1( 0.0f, 0.5f, 1.0f, 2.0f );
    m_pd3dDevice->SetVertexShaderConstant(  0, &vConstants0,      1 );
    m_pd3dDevice->SetVertexShaderConstant(  1, &vConstants1,      1 );
    m_pd3dDevice->SetVertexShaderConstant( 11, &vPtEyeWorldPos,   1 );
    m_pd3dDevice->SetVertexShaderConstant( 12, &vPtLightWorldPos, 1 );
    m_pd3dDevice->SetVertexShaderConstant( 20, &matWorld,         4 );
    m_pd3dDevice->SetVertexShaderConstant( 30, &matWorldView,     4 );
    m_pd3dDevice->SetVertexShaderConstant( 40, &matWorldViewProj, 4 );
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
    SetVertexShaderConstants( &g_vPtLightPos );

    // Set blending to add the upcoming passes to the previous pass
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );  
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    // Render the BRDF effect
    {
        m_pd3dDevice->SetTexture( 0, m_pNormalMap );
        m_pd3dDevice->SetTexture( 1, m_pAngularizingCubeMap2 );
        m_pd3dDevice->SetTexture( 2, m_pAngularizingCubeMap1 );
        m_pd3dDevice->SetTexture( 3, m_pBRDFVolumeTexture );
        m_pd3dDevice->SetPixelShader( m_dwBRDFPixelShader );

        // Set clamp mode for the cubemaps
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP );

        if( g_bPointFiltering )
        {
            m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_POINT );
            m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_POINT );
            m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MINFILTER, D3DTEXF_POINT );
            m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MAGFILTER, D3DTEXF_POINT );
        }
        else
        {
            m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
            m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
            m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
            m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        }

        m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSW, D3DTADDRESS_WRAP );

/*
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
        m_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2 );
        m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT );
*/
        RenderObject( m_pd3dDevice );
//        m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );

        m_pd3dDevice->SetPixelShader( NULL );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateBRDFPixelShader()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CBumpyObject::CreateBRDFPixelShader()
{
    D3DPIXELSHADERDEF psd;
    ZeroMemory(&psd, sizeof(psd));

    psd.PSCombinerCount=PS_COMBINERCOUNT(
        3,
        PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
    psd.PSTextureModes=PS_TEXTUREMODES(
        PS_TEXTUREMODES_PROJECT2D,
        PS_TEXTUREMODES_CUBEMAP, 
        PS_TEXTUREMODES_CUBEMAP,
        PS_TEXTUREMODES_BRDF );
    psd.PSInputTexture=PS_INPUTTEXTURE(0,0,0,0);
    psd.PSDotMapping=PS_DOTMAPPING(
        0,
        PS_DOTMAPPING_MINUS1_TO_1_D3D,
        PS_DOTMAPPING_MINUS1_TO_1_D3D,
        PS_DOTMAPPING_MINUS1_TO_1_D3D );
    psd.PSCompareMode=PS_COMPAREMODE(
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT);

    //------------- Stage 0 -------------
    psd.PSRGBInputs[0]=PS_COMBINERINPUTS(
        PS_REGISTER_T3   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ONE  | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
    psd.PSAlphaInputs[0]=PS_COMBINERINPUTS(
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

    psd.PSRGBOutputs[0]=PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_AB_MULTIPLY );
    psd.PSAlphaOutputs[0]=PS_COMBINEROUTPUTS(
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY );

    //------------- Final combiner -------------
    psd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS( PS_REGISTER_ONE | PS_CHANNEL_RGB,
                                                      PS_REGISTER_ONE | PS_CHANNEL_RGB,
                                                      PS_REGISTER_R0  | PS_CHANNEL_ALPHA,
                                                      0 | 0 | 0 );

    psd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS( PS_REGISTER_ONE   | PS_CHANNEL_ALPHA,
                                                       PS_REGISTER_R0    | PS_CHANNEL_RGB,
                                                       PS_REGISTER_ZERO  | PS_CHANNEL_RGB,
                                                       PS_REGISTER_ZERO  | PS_CHANNEL_RGB );

    // Create the pixel shader
    if( FAILED( m_pd3dDevice->CreatePixelShader( &psd, &m_dwBRDFPixelShader ) ) )
        return E_FAIL;

    return S_OK;
}




