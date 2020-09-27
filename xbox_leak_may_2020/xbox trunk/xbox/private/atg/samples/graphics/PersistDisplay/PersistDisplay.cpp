//-----------------------------------------------------------------------------
// File: PersistDisplay.cpp
//
// Desc: Xbox sample showing how to persist the display during the launching
//       of a title, and then using that persisted surface in a startup
//       transition effect when the title starts. For simplicity, this sample
//       relaunches itslelf.
//
// Hist: 10.08.01 - New for the November XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBMesh.h>
#include <XBResource.h>
#include <XBUtil.h>
#include "Resource.h" // Resource header produced by the Bundler tool




//-----------------------------------------------------------------------------
// Globals variables and definitions
//-----------------------------------------------------------------------------
DWORD g_dwWaterColor     = 0x00004080; // Color of the water
FLOAT g_fLightAngle      = 0.0f;       // Orientation of light from above


// Vertex for the 3D meshes
struct SEAFLOORVERTEX
{
    D3DXVECTOR3 p;           // Position
    D3DXVECTOR3 n;           // Normal
    FLOAT       tu, tv;      // Tex coords
};

// Persisted surface transition constants
#define TRANSITION_FADEIN_END    2.0f    // Dolphin finishes fading-in in 3 seconds
#define TRANSITION_FADEOUT_BEGIN 4.0f    // Transition begins fade at 5 seconds
#define TRANSITION_FADEOUT_END   6.0f    // Transition ends fade at 7 seconds
#define RELAUNCH_FADE_TIME       2.0f    // Relaunch screen takes 2 seconds to fade




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    // Packed resources for the app
    CXBPackedResource       m_xprResource;

    // Font
    CXBFont                 m_Font;

    // Transform matrices
    D3DXMATRIX              m_matWorld;
    D3DXMATRIX              m_matView;
    D3DXMATRIX              m_matProj;

    // Dolphin object
    LPDIRECT3DTEXTURE8      m_pDolphinTexture;
    CXBMesh                 m_DolphinMesh1;
    CXBMesh                 m_DolphinMesh2;
    CXBMesh                 m_DolphinMesh3;
    LPDIRECT3DVERTEXBUFFER8 m_pDolphinVB1;
    LPDIRECT3DVERTEXBUFFER8 m_pDolphinVB2;
    LPDIRECT3DVERTEXBUFFER8 m_pDolphinVB3;
    LPDIRECT3DINDEXBUFFER8  m_pDolphinIB;
    DWORD                   m_dwNumDolphinVertices;
    DWORD                   m_dwNumDolphinIndices;
    DWORD                   m_dwDolphinVertexShader;
    DWORD                   m_dwDolphinVertexSize;

    LPDIRECT3DVERTEXBUFFER8 m_pDolphinStream1;
    LPDIRECT3DVERTEXBUFFER8 m_pDolphinStream2;

    // Seafloor object
    LPDIRECT3DTEXTURE8      m_pSeaFloorTexture;
    CXBMesh                 m_SeaFloorMesh;
    LPDIRECT3DVERTEXBUFFER8 m_pSeaFloorVB;
    LPDIRECT3DINDEXBUFFER8  m_pSeaFloorIB;
    DWORD                   m_dwNumSeaFloorVertices;
    DWORD                   m_dwNumSeaFloorIndices;
    DWORD                   m_dwSeaFloorVertexShader;
    DWORD                   m_dwSeaFloorVertexSize;

    // Water caustics
    LPDIRECT3DTEXTURE8      m_pCausticTextures[32];
    LPDIRECT3DTEXTURE8      m_pCurrentCausticTexture;
    DWORD                   m_dwUnderWaterPixelShader;

    // Image to persist when launching new title
    LPDIRECT3DTEXTURE8      m_pTextureToPersist;
    CHAR                    m_strLaunchTitleName[80];
    BOOL                    m_bAboutToLaunchNewTitle;
    FLOAT                   m_fRelaunchCountdownTime;

    // Startup animation transition
    BOOL                    m_bDoingStartupTransition;
    LPDIRECT3DSURFACE8      m_pPersistedSurface;   // Persisted surface from the startup screen
    LPDIRECT3DTEXTURE8      m_pPersistedTexture;   // Texture interface for the surface

    // Internal functions
    HRESULT CreatePixelShader();
    HRESULT RenderQuad(  LPDIRECT3DTEXTURE8 pTexture, FLOAT fAlpha );

public:
    HRESULT Initialize();
    HRESULT Render();
    HRESULT FrameMove();

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

    // Dolphin object
    m_pDolphinTexture        = NULL;
    m_pDolphinVB1            = NULL;
    m_pDolphinVB2            = NULL;
    m_pDolphinVB3            = NULL;
    m_pDolphinIB             = NULL;
    m_dwDolphinVertexShader  = 0L;

    // SeaFloor object
    m_pSeaFloorTexture       = NULL;
    m_pSeaFloorVB            = NULL;
    m_pSeaFloorIB            = NULL;
    m_dwSeaFloorVertexShader = 0L;

    // Water caustics
    for( DWORD t=0; t<32; t++ )
        m_pCausticTextures[t] = NULL;
    m_dwUnderWaterPixelShader = 0L;

    // Startup animation transition
    m_pPersistedSurface = NULL;
    m_pPersistedTexture = NULL;

    m_bAboutToLaunchNewTitle  = FALSE;
    m_bDoingStartupTransition = FALSE;
}




//-----------------------------------------------------------------------------
// Name: CreatePixelShader()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreatePixelShader()
{
    //
    // Setup the pixel shader. To blend in water caustics, we want the
    // following effect:
    //    result = t0 * ( t1 * v0 + c0 )
    // where:
    //    t0 = Base texture
    //    t1 = Water caustics texture
    //    c0 = Ambient light contribution
    //    v0 = Directional light contribution
    //
    // This can be encoded into a pixel shader with:
    //    1st stage: r0 = (t1*v0 + c0*1)
    //    Final:     A*B + (1-A)*C + D
    //               E*F = r0 * t0
    // where:
    //    A = Fog factor
    //    B = EF
    //    C = Fog color
    //    D = 0
    //    E = r0
    //    F = t0
    //
    D3DPIXELSHADERDEF psd;
    ZeroMemory( &psd, sizeof(psd) );
    psd.PSCombinerCount = PS_COMBINERCOUNT( 1,
                                            PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
    psd.PSTextureModes  = PS_TEXTUREMODES( PS_TEXTUREMODES_PROJECT2D, PS_TEXTUREMODES_PROJECT2D,
                                           PS_TEXTUREMODES_NONE, PS_TEXTUREMODES_NONE );
    psd.PSInputTexture  = PS_INPUTTEXTURE( 0, 0, 0, 0 );
    psd.PSDotMapping    = PS_DOTMAPPING( 0, PS_DOTMAPPING_ZERO_TO_ONE,
                                         PS_DOTMAPPING_ZERO_TO_ONE, PS_DOTMAPPING_ZERO_TO_ONE );
    psd.PSCompareMode   = PS_COMPAREMODE( PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
                                          PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
                                          PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
                                          PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT );

    //------------- Stage 0 -------------
    // Build factor from caustics and lighting (R0 = T1*V0 + C0)
    psd.PSRGBInputs[0]    = PS_COMBINERINPUTS( PS_REGISTER_T1  | PS_CHANNEL_RGB,
                                               PS_REGISTER_V0  | PS_CHANNEL_RGB,
                                               PS_REGISTER_C0  | PS_CHANNEL_RGB,
                                               PS_REGISTER_ONE | PS_CHANNEL_RGB );
    psd.PSAlphaInputs[0]  = PS_COMBINERINPUTS( PS_REGISTER_T1  | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_V0  | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_C0  | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA );
    psd.PSRGBOutputs[0]   = PS_COMBINEROUTPUTS( PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_R0,
                                                PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );
    psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS( PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_R0,
                                                PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );

    psd.PSC0Mapping = 0xfffffff0;
    psd.PSC1Mapping = 0xffffffff;

    //------------- Final combiner -------------
    
    // EF is R0*T0. G is alpha and is set to 1.
    psd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS( PS_REGISTER_R0  | PS_CHANNEL_RGB,
                                                      PS_REGISTER_T0  | PS_CHANNEL_RGB,
                                                      PS_REGISTER_ONE | PS_CHANNEL_RGB,
                                                      0 | 0 | 0 );

    // Just factor the fog in with the results from the last stage:
    //    Result = Fog.a * R0 + (1-Fog.a)*Fog.rgb + 0
    psd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS( PS_REGISTER_FOG     | PS_CHANNEL_ALPHA,
                                                       PS_REGISTER_EF_PROD | PS_CHANNEL_RGB,
                                                       PS_REGISTER_FOG     | PS_CHANNEL_RGB,
                                                       PS_REGISTER_ZERO    | PS_CHANNEL_RGB );

    // Create the pixel shader, as defined above.
    if( FAILED( m_pd3dDevice->CreatePixelShader( &psd, &m_dwUnderWaterPixelShader ) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT    hr;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create texture for the image to persiste
    m_pTextureToPersist = m_xprResource.GetTexture( resource_TextureToPersist_OFFSET );

    // Create texture for the dolphin
    m_pDolphinTexture = m_xprResource.GetTexture( resource_DolphinTexture_OFFSET );

    // Load the file-based mesh objects for the dolphin
    if( FAILED( m_DolphinMesh1.Create( m_pd3dDevice, "Models\\Dolphin1.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( m_DolphinMesh2.Create( m_pd3dDevice, "Models\\Dolphin2.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( m_DolphinMesh3.Create( m_pd3dDevice, "Models\\Dolphin3.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    m_pDolphinVB1 = &m_DolphinMesh1.GetMesh(0)->m_VB;
    m_pDolphinVB2 = &m_DolphinMesh2.GetMesh(0)->m_VB;
    m_pDolphinVB3 = &m_DolphinMesh3.GetMesh(0)->m_VB;
    m_pDolphinIB  = &m_DolphinMesh1.GetMesh(0)->m_IB;
    m_dwNumDolphinVertices = m_DolphinMesh1.GetMesh(0)->m_dwNumVertices;
    m_dwNumDolphinIndices  = m_DolphinMesh1.GetMesh(0)->m_dwNumIndices;
    m_dwDolphinVertexSize  = m_DolphinMesh1.GetMesh(0)->m_dwVertexSize;

    // Create vertex shader for the dolphin
    DWORD dwDolphinVertexDecl[] =
    {
        // m_pDolphinVB1
        D3DVSD_STREAM( 0 ),              // This data comes from stream 0
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // v0 = Position of first mesh
        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), // v3 = Normal
        D3DVSD_REG( 6, D3DVSDT_FLOAT2 ), // v6 = Tex coords

        // m_pDolphinVB2
        D3DVSD_STREAM( 1 ),              // This data comes from stream 1
        D3DVSD_REG( 1, D3DVSDT_FLOAT3 ), // v1 = Position of second mesh
        D3DVSD_REG( 4, D3DVSDT_FLOAT3 ), // v4 = Normal
        D3DVSD_REG( 7, D3DVSDT_FLOAT2 ), // v7 = Tex coords

        // m_pDolphinVB3
        D3DVSD_STREAM( 2 ),              // This data comes from stream 2
        D3DVSD_REG( 2, D3DVSDT_FLOAT3 ), // v2 = Position of second mesh
        D3DVSD_REG( 5, D3DVSDT_FLOAT3 ), // v5 = Normal
        D3DVSD_REG( 8, D3DVSDT_FLOAT2 ), // v8 = Tex coords
        D3DVSD_END()
    };

    hr = XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\Dolphin.xvu",
                                    dwDolphinVertexDecl, &m_dwDolphinVertexShader );
    if( FAILED(hr) )
        return hr;

    // Create textures for the seafloor
    m_pSeaFloorTexture = m_xprResource.GetTexture( resource_SeaFloorTexture_OFFSET );
    
    // Load the file-based mesh object for the seafloor
    if( FAILED( m_SeaFloorMesh.Create( m_pd3dDevice, "Models\\SeaFloor.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    m_pSeaFloorVB = &m_SeaFloorMesh.GetMesh(0)->m_VB;
    m_pSeaFloorIB = &m_SeaFloorMesh.GetMesh(0)->m_IB;
    m_dwNumSeaFloorVertices = m_SeaFloorMesh.GetMesh(0)->m_dwNumVertices;
    m_dwNumSeaFloorIndices  = m_SeaFloorMesh.GetMesh(0)->m_dwNumIndices;
    m_dwSeaFloorVertexSize  = m_SeaFloorMesh.GetMesh(0)->m_dwVertexSize;

    // Add some bumpiness to the seafloor mesh
    SEAFLOORVERTEX* pDst;
    m_pSeaFloorVB->Lock( 0, 0, (BYTE**)&pDst, 0 );
    srand(5);
    for( DWORD i=0; i<m_dwNumSeaFloorVertices; i++ )
    {
        pDst[i].p.y += (rand()/(FLOAT)RAND_MAX);
        pDst[i].p.y += (rand()/(FLOAT)RAND_MAX);
        pDst[i].p.y += (rand()/(FLOAT)RAND_MAX);
        pDst[i].tu  *= 10;
        pDst[i].tv  *= 10;
    }
    m_pSeaFloorVB->Unlock();

    // Create vertex shader for the seafloor
    DWORD dwSeaFloorVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position of first mesh
        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), // Normal
        D3DVSD_REG( 6, D3DVSDT_FLOAT2 ), // Tex coords
        D3DVSD_END()
    };

    if( FAILED( hr = XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\SeaFloor.xvu",
                                                dwSeaFloorVertexDecl,
                                                &m_dwSeaFloorVertexShader ) ) )
        return hr;

    // Create a pixel shader for the underwater effect
    hr = CreatePixelShader();
    if( FAILED(hr) )
        return hr;

    // Create textures for the water caustics
    m_pCausticTextures[ 0] = m_xprResource.GetTexture( resource_WaterCaustic00_OFFSET );
    m_pCausticTextures[ 1] = m_xprResource.GetTexture( resource_WaterCaustic01_OFFSET );
    m_pCausticTextures[ 2] = m_xprResource.GetTexture( resource_WaterCaustic02_OFFSET );
    m_pCausticTextures[ 3] = m_xprResource.GetTexture( resource_WaterCaustic03_OFFSET );
    m_pCausticTextures[ 4] = m_xprResource.GetTexture( resource_WaterCaustic04_OFFSET );
    m_pCausticTextures[ 5] = m_xprResource.GetTexture( resource_WaterCaustic05_OFFSET );
    m_pCausticTextures[ 6] = m_xprResource.GetTexture( resource_WaterCaustic06_OFFSET );
    m_pCausticTextures[ 7] = m_xprResource.GetTexture( resource_WaterCaustic07_OFFSET );
    m_pCausticTextures[ 8] = m_xprResource.GetTexture( resource_WaterCaustic08_OFFSET );
    m_pCausticTextures[ 9] = m_xprResource.GetTexture( resource_WaterCaustic09_OFFSET );
    m_pCausticTextures[10] = m_xprResource.GetTexture( resource_WaterCaustic10_OFFSET );
    m_pCausticTextures[11] = m_xprResource.GetTexture( resource_WaterCaustic11_OFFSET );
    m_pCausticTextures[12] = m_xprResource.GetTexture( resource_WaterCaustic12_OFFSET );
    m_pCausticTextures[13] = m_xprResource.GetTexture( resource_WaterCaustic13_OFFSET );
    m_pCausticTextures[14] = m_xprResource.GetTexture( resource_WaterCaustic14_OFFSET );
    m_pCausticTextures[15] = m_xprResource.GetTexture( resource_WaterCaustic15_OFFSET );
    m_pCausticTextures[16] = m_xprResource.GetTexture( resource_WaterCaustic16_OFFSET );
    m_pCausticTextures[17] = m_xprResource.GetTexture( resource_WaterCaustic17_OFFSET );
    m_pCausticTextures[18] = m_xprResource.GetTexture( resource_WaterCaustic18_OFFSET );
    m_pCausticTextures[19] = m_xprResource.GetTexture( resource_WaterCaustic19_OFFSET );
    m_pCausticTextures[20] = m_xprResource.GetTexture( resource_WaterCaustic20_OFFSET );
    m_pCausticTextures[21] = m_xprResource.GetTexture( resource_WaterCaustic21_OFFSET );
    m_pCausticTextures[22] = m_xprResource.GetTexture( resource_WaterCaustic22_OFFSET );
    m_pCausticTextures[23] = m_xprResource.GetTexture( resource_WaterCaustic23_OFFSET );
    m_pCausticTextures[24] = m_xprResource.GetTexture( resource_WaterCaustic24_OFFSET );
    m_pCausticTextures[25] = m_xprResource.GetTexture( resource_WaterCaustic25_OFFSET );
    m_pCausticTextures[26] = m_xprResource.GetTexture( resource_WaterCaustic26_OFFSET );
    m_pCausticTextures[27] = m_xprResource.GetTexture( resource_WaterCaustic27_OFFSET );
    m_pCausticTextures[28] = m_xprResource.GetTexture( resource_WaterCaustic28_OFFSET );
    m_pCausticTextures[29] = m_xprResource.GetTexture( resource_WaterCaustic29_OFFSET );
    m_pCausticTextures[30] = m_xprResource.GetTexture( resource_WaterCaustic30_OFFSET );
    m_pCausticTextures[31] = m_xprResource.GetTexture( resource_WaterCaustic31_OFFSET );

    // Set the transform matrices
    D3DXVECTOR3 vEyePt      = D3DXVECTOR3( 0.0f, 0.0f,-5.0f );
    D3DXVECTOR3 vLookatPt   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec      = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &vUpVec );
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/3, 4.0f/3.0f, 1.0f, 10000.0f );

    // Create a texture surface for the persisted surface
    LPDIRECT3DSURFACE8 pPersistedSurface;
    m_pd3dDevice->GetPersistedSurface( &pPersistedSurface );
    if( pPersistedSurface )
    {
        // Create an empty surface to hold the persistent screen
        m_pd3dDevice->CreateTexture( 640, 480, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &m_pPersistedTexture );
        m_pPersistedTexture->GetSurfaceLevel( 0, &m_pPersistedSurface );
    
        // Copy persisted bits to our surface, scaling and converting if necessary
        D3DXLoadSurfaceFromSurface( m_pPersistedSurface, NULL, NULL, pPersistedSurface, 
                                    NULL, NULL, D3DX_FILTER_TRIANGLE, 0 );

        // Note that the persisted surface is not reference-counted, so we do
        // not need to call Release() on it

        // With a valid surface, we can safely do the transition
        m_bDoingStartupTransition = TRUE;
    }

    // Find the name of the executable. We'll use this later when we call
    // XLaunchNewImage() to re-launch this app.
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile( "d:\\*.xbe", &wfd );
    if( INVALID_HANDLE_VALUE == hFind )
    {
        OUTPUT_DEBUG_STRING( "Error: Could not find any .xbe files.\n" );
        return E_FAIL;
    }
    sprintf( m_strLaunchTitleName, "d:\\%s", wfd.cFileName );
    FindClose( hFind );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    if( !m_bDoingStartupTransition )
    {
        // When the user presses the Y button, kick off the process to persist the
        // display and relaunch the title.
        if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
        {
            m_bAboutToLaunchNewTitle = TRUE;
            m_fRelaunchCountdownTime = RELAUNCH_FADE_TIME;
        }
        
        // Keep the countdown going
        m_fRelaunchCountdownTime -= m_fElapsedAppTime;
    }
    
    // Animation attributes for the dolphin
    FLOAT fKickFreq    = 2*m_fAppTime;
    FLOAT fPhase       = m_fAppTime/3-1.3f;
    FLOAT fBlendWeight = sinf( fKickFreq );

    // Move the dolphin in a circle
    D3DXMATRIX matDolphin, matTrans, matRotate1, matRotate2;
    D3DXMatrixScaling( &matDolphin, 0.01f, 0.01f, 0.01f );
    D3DXMatrixRotationZ( &matRotate1, -cosf(fKickFreq)/6 );
    D3DXMatrixMultiply( &matDolphin, &matDolphin, &matRotate1 );
    D3DXMatrixRotationY( &matRotate2, fPhase );
    D3DXMatrixMultiply( &matDolphin, &matDolphin, &matRotate2 );
    D3DXMatrixTranslation( &matTrans, -5.0f*sinf(fPhase), sinf(fKickFreq)/2, 10.0f-10.0f*cosf(fPhase) );
    D3DXMatrixMultiply( &matDolphin, &matDolphin, &matTrans );

    // Blend weight and vertex buffer selection for vertex tweening
    FLOAT fWeight1, fWeight2;
    if ( fBlendWeight > 0.0f )
    {
        // Use vertex sets one the two.
        fWeight1 = fBlendWeight;
        fWeight2 = 1.0f - fBlendWeight;

        m_pDolphinStream1 = m_pDolphinVB1;
        m_pDolphinStream2 = m_pDolphinVB2;
    }
    else
    {
        // Use vertex sets two and three.
        fWeight1 = 1.0f + fBlendWeight;
        fWeight2 = -fBlendWeight;

        m_pDolphinStream1 = m_pDolphinVB2;
        m_pDolphinStream2 = m_pDolphinVB3;
    }

    // Values passed into the vertex shader for lighting, etc.
    D3DXVECTOR4 fConstants0( 0.0f, 0.0f, 0.0f, 0.00f );
    D3DXVECTOR4 fConstants1( 1.0f, 0.5f, 0.2f, 0.05f );
    D3DXVECTOR4 fLight( sinf(g_fLightAngle), cosf(g_fLightAngle), 0.0f, 0.0f );
    D3DXVECTOR4 fDiffuse( 1.00f, 1.00f, 1.00f, 1.00f );
    D3DXVECTOR4 fAmbient( 0.25f, 0.25f, 0.25f, 0.25f );
    D3DXVECTOR4 fFog( 0.5f, 50.0f, 1.0f/(50.0f-1.0f), 0.0f );
    D3DXVECTOR4 fCaustics( 0.05f, 0.05f, sinf(m_fAppTime)/8, cosf(m_fAppTime)/10 - m_fAppTime/10 );
    D3DXVECTOR4 fWeight( fWeight1, fWeight2, 0.0f, 0.0f );

    // Calculate and set composite matrix for dolphin.
    D3DXMATRIX matComposite;
    D3DXMatrixMultiply( &matComposite, &matDolphin, &m_matView );
    D3DXMatrixMultiply( &matComposite, &matComposite, &m_matProj );
    D3DXMatrixTranspose( &matComposite, &matComposite );
    m_pd3dDevice->SetVertexShaderConstant( 4, &matComposite, 4 );

    // Scale and offset fog start and end by values from projection matrix.
    float fFogStart = 1.0f;
    float fFogEnd = 50.0f;

    fFogStart = fFogStart * m_matProj._33 + m_matProj._43;
    fFogEnd = fFogEnd * m_matProj._33 + m_matProj._43;

    fFog[2] = 1.0f / (fFogEnd - fFogStart);
    fFog[1] = fFogEnd * fFog[2];

    // Calculate and set composite matrix for seafloor.
    D3DXMatrixMultiply( &matComposite, &m_matView, &m_matProj );
    D3DXMatrixTranspose( &matComposite, &matComposite );
    m_pd3dDevice->SetVertexShaderConstant( 8, &matComposite, 4 );

    // Calculate and set texgen matrix for dolphin.
    D3DXMatrixMultiply( &matComposite, &matDolphin, &m_matView );
    matComposite *= 0.5f;
    D3DXMatrixTranspose( &matComposite, &matComposite );
    m_pd3dDevice->SetVertexShaderConstant( 12, &matComposite, 4 );

    // Set the vertex shader constants
    m_pd3dDevice->SetVertexShaderConstant(  0, &fConstants0, 1 ); // Some constants
    m_pd3dDevice->SetVertexShaderConstant(  1, &fConstants1, 1 ); // More constants
    m_pd3dDevice->SetVertexShaderConstant(  2, &fWeight,     1 );

    m_pd3dDevice->SetVertexShaderConstant( 20, &fLight,      1 ); // Light direction
    m_pd3dDevice->SetVertexShaderConstant( 21, &fDiffuse,    1 ); // Diffuse color
    m_pd3dDevice->SetVertexShaderConstant( 23, &fFog,        1 ); // Fog factors
    m_pd3dDevice->SetVertexShaderConstant( 24, &fCaustics,   1 ); // Misc constants
    
    // Animate the caustic textures
    DWORD tex = ((DWORD)(m_fAppTime*32))%32;
    m_pCurrentCausticTexture = m_pCausticTextures[tex];

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderQuad()
// Desc: Renders a quad textured with the persisted surface
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderQuad( LPDIRECT3DTEXTURE8 pTexture, FLOAT fAlpha )
{
    // Set up the vertices (notice the pixel centers are shifted by -0.5f to
    // line them up with the texel centers). The texture coordinates assume
    // a linear texture will be used.
    struct VERTEX { D3DXVECTOR4 p; FLOAT tu, tv; };
    VERTEX v[4];
    v[0].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 0.0f, 0.0f ); v[0].tu =   0; v[0].tv =   0;
    v[1].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 0.0f, 0.0f ); v[1].tu = 640; v[1].tv =   0;
    v[2].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 0.0f, 0.0f ); v[2].tu = 640; v[2].tv = 480;
    v[3].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 0.0f, 0.0f ); v[3].tu =   0; v[3].tv = 480;

    // Set state to render the image
    m_pd3dDevice->SetTexture( 0, pTexture );
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, (DWORD)(255.0f*fAlpha)<<24L );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );

    // Render the quad
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, v, sizeof(v[0]) );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport (normal clear, no transition)
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         g_dwWaterColor, 1.0f, 0L );

    // Set default render states
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,         g_dwWaterColor );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    // Turn on the pixel shader for the underwater effect, which blends in the
    // current caustic texture.
    m_pd3dDevice->SetPixelShader( m_dwUnderWaterPixelShader );
    m_pd3dDevice->SetTexture( 1, m_pCurrentCausticTexture );

    // Set the ambient color input to the pixel shader.
    // Note: This must be done after the pixel shader is set.
    D3DXVECTOR4 fAmbient( 0.25f, 0.25f, 0.25f, 0.25f );
    m_pd3dDevice->SetPixelShaderConstant( 0, &fAmbient, 1 );

    // Render the seafloor. The vertex shader does the transformation, texture
    // projection (for the water caustics) and fog calculations.
    m_pd3dDevice->SetTexture( 0, m_pSeaFloorTexture );
    m_pd3dDevice->SetVertexShader( m_dwSeaFloorVertexShader );
    m_pd3dDevice->SetStreamSource( 0, m_pSeaFloorVB, m_dwSeaFloorVertexSize );
    m_pd3dDevice->SetIndices( m_pSeaFloorIB, 0 );
    m_pd3dDevice->DrawIndexedVertices( D3DPT_TRIANGLESTRIP, m_dwNumSeaFloorIndices, 
                                       D3D__IndexData );

    // At the beginning of the app, we keep access to the persisted surface, so
    // we can have a transistion effect. Render that effect here, after the
    // seafloor, but before the dolphin to get a cool effect.
    if( m_bDoingStartupTransition )
    {
        if( m_fAppTime < TRANSITION_FADEOUT_BEGIN )
        {
            // For the first few seconds, keep the persisted display as is
            RenderQuad( m_pPersistedTexture, 1.0f );
        }
        else if( m_fAppTime < TRANSITION_FADEOUT_END )
        {
            // Over the next few seconds, fade out the persisted display
            RenderQuad( m_pPersistedTexture, 
                        1.0f-(m_fAppTime-TRANSITION_FADEOUT_BEGIN)/(TRANSITION_FADEOUT_END-TRANSITION_FADEOUT_BEGIN) );
        }
        else
        {
            // We're done with the surface
            m_pPersistedTexture->Release();
            m_pPersistedTexture = NULL;
            m_bDoingStartupTransition = FALSE;
        }
    }

    // Restore state that would've gotten trashed rendering a quad above
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,         g_dwWaterColor );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTexture( 1, m_pCurrentCausticTexture );
    m_pd3dDevice->SetPixelShader( m_dwUnderWaterPixelShader );
    m_pd3dDevice->SetPixelShaderConstant( 0, &fAmbient, 1 );

    // Render the dolphin. The vertex shader does the vertex tweening, 
    // transformation, texture projection (for the water caustics) and fog
    // calculations.
    m_pd3dDevice->SetTexture( 0, m_pDolphinTexture );
    m_pd3dDevice->SetVertexShader( m_dwDolphinVertexShader );
    m_pd3dDevice->SetStreamSource( 0, m_pDolphinStream1, m_dwDolphinVertexSize );
    m_pd3dDevice->SetStreamSource( 1, m_pDolphinStream2, m_dwDolphinVertexSize );
    m_pd3dDevice->SetIndices( m_pDolphinIB, 0 );
    m_pd3dDevice->DrawIndexedVertices( D3DPT_TRIANGLESTRIP, m_dwNumDolphinIndices, 
                                       D3D__IndexData );

    // If we're in the startup transition, let's render the persisted surface
    // again, so it looks like the dolphin fades into existance.
    if( m_bDoingStartupTransition )
    {
        if( m_fAppTime < TRANSITION_FADEIN_END )
        {
            // Over the first few seconds, fade out the persisted display which
            // is kinda like the same thing as fading in the dolphin
            RenderQuad( m_pPersistedTexture, 
                        1.0f - (m_fAppTime)/(TRANSITION_FADEIN_END) );
        }
    }

    // Draw the title and framerate
    if( !m_bDoingStartupTransition && !m_bAboutToLaunchNewTitle )
    {
        m_Font.Begin();
        m_Font.DrawText(  64,  50, 0xffffffff, L"PersistDisplay" );
        m_Font.DrawText( 450,  50, 0xffffff00, m_strFrameRate );
        m_Font.DrawText(  64,  75, 0xffffff00, L"Press the Y button to persist the display" );
        m_Font.DrawText(  64, 100, 0xffffff00, L"and relaunch this title" );
        m_Font.End();
    }

    // Before launching a new title, fade in an image to persist. The image will
    // be something akin to "Loading next level..."
    if( m_bAboutToLaunchNewTitle )
    {
        if( m_fRelaunchCountdownTime > 0.0f )
        {
            // Over the first few seconds, fade in the image to persist
            RenderQuad( m_pTextureToPersist, 1.0f - m_fRelaunchCountdownTime/RELAUNCH_FADE_TIME );
        }
        else
        {
            // When the countdown is done, persist the  display and relaunch
            // the title.
            RenderQuad( m_pTextureToPersist, 1.0f );
            m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
            m_pd3dDevice->PersistDisplay();
            XLaunchNewImage( m_strLaunchTitleName, NULL );
        }
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



