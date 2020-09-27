//-----------------------------------------------------------------------------
// File: Dolphin.cpp
//
// Desc: Xbox sample of a swimming dolphin using vertex shaders and animated
//       textures for some nice underwater effects.
//
// Hist: 11.01.00 - Port for XBox for November XDK release
//       12.15.00 - Now using shaders for December release
//       12.20.00 - Added pixel shader, spline path and better models
//       03.10.01 - Using new geometry functions
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBMesh.h>
#include <XBResource.h>
#include <XBUtil.h>

// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"




//-----------------------------------------------------------------------------
// Globals variables and definitions
//-----------------------------------------------------------------------------
DWORD g_dwWaterColor = 0x00004080;

FLOAT g_fCycle      = 120.0f;     // Time for dolphin to complete a path cycle
FLOAT g_fLightAngle = D3DX_PI/4;  // Orientation of light from above



#define SHADOW_SIZE  256
DWORD g_dwShadowColor = 0xff000000;


// Vertex for the 3D meshes
struct D3DVERTEX
{
    D3DXVECTOR3 p;           // Position
    D3DXVECTOR3 n;           // Normal
    FLOAT       tu, tv;      // Tex coords
};

#define D3DFVF_D3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)


// Structure for the bubbles
struct BUBBLE
{
    FLOAT       age, lifespan;
    FLOAT       angle, spread, wobble;
    D3DXVECTOR3 vPos;
    D3DXVECTOR3 vPos0;
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    // Font
    CXBFont                 m_Font;

    CXBPackedResource       m_xprResources;

    // Transform matrices
    D3DXMATRIX              m_matWorld;
    D3DXMATRIX              m_matView;
    D3DXMATRIX              m_matProj;

    // Dolphin object
    LPDIRECT3DTEXTURE8      m_pDolphinTexture;
    DWORD                   m_dwDolphinVertexShader;
    CXBMesh                 m_DolphinObject1;
    CXBMesh                 m_DolphinObject2;
    CXBMesh                 m_DolphinObject3;
    XBMESH_DATA*            m_pDolphinMesh1;
    XBMESH_DATA*            m_pDolphinMesh2;
    XBMESH_DATA*            m_pDolphinMesh3;

    // Seafloor object
    LPDIRECT3DTEXTURE8      m_pSeaFloorTexture;
    DWORD                   m_dwSeaFloorVertexShader;
    CXBMesh                 m_SeaFloorObject;
    XBMESH_DATA*            m_pSeaFloorMesh;

    // Water caustics
    LPDIRECT3DTEXTURE8      m_pCausticTextures[32];
    LPDIRECT3DTEXTURE8      m_pCurrentCausticTexture;
    DWORD                   m_dwDolphinPixelShader;
    DWORD                   m_dwSeaFloorPixelShader;

    // Dolphin shadow
    LPDIRECT3DTEXTURE8      m_pShadowTexture;
    LPDIRECT3DSURFACE8      m_pShadowSurface;

    // Bubbles
    D3DXVECTOR3        m_vBubbleEmitterPos;
    BUBBLE*            m_pBubbles;
    DWORD              m_dwNumBubbles;
    LPDIRECT3DVERTEXBUFFER8 m_pBubbleVB;
    LPDIRECT3DTEXTURE8 m_pBubbleTexture;

    HRESULT CreatePixelShader();
    HRESULT UpdateBubbles( FLOAT fAddBubbles );
    HRESULT RenderBubbles();

    // Spline path for the dolphin
    LPDIRECT3DVERTEXBUFFER8 m_pDolphinSplineVB;
    D3DXVECTOR3* m_DolphinSpline;
    DWORD        m_dwNumDolphinSplinePts;

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
    m_dwDolphinVertexShader  = 0L;

    // SeaFloor object
    m_dwSeaFloorVertexShader = 0L;

    // Water caustics
    m_dwDolphinPixelShader  = 0L;
    m_dwSeaFloorPixelShader = 0L;

    m_pBubbleTexture     = NULL;
    m_pBubbleVB          = NULL;
    m_dwNumBubbles       = 100;
    m_pBubbles           = new BUBBLE[m_dwNumBubbles];
    m_vBubbleEmitterPos  = D3DXVECTOR3( 0, -1, -6 );

    for( DWORD i=0; i<m_dwNumBubbles; i++ )
    {
        m_pBubbles[i].lifespan = 5.0f + ((FLOAT)rand()/(FLOAT)RAND_MAX) * 2.0f;
        m_pBubbles[i].age      = (i*m_pBubbles[i].lifespan)/m_dwNumBubbles;
        m_pBubbles[i].angle    = ((FLOAT)rand()/(FLOAT)RAND_MAX) * 2.0f * D3DX_PI;
        m_pBubbles[i].spread   = ((FLOAT)rand()/(FLOAT)RAND_MAX) * 1.0f;
        m_pBubbles[i].wobble   = ((FLOAT)rand()/(FLOAT)RAND_MAX) * 2.0f * D3DX_PI - D3DX_PI;

        FLOAT y = m_pBubbles[i].age;
        FLOAT r = m_pBubbles[i].spread * sqrtf( y / 10.0f );
        FLOAT x = r*sinf( m_pBubbles[i].angle );
        FLOAT z = r*cosf( m_pBubbles[i].angle );

        x += 0.1f*sqrtf( y / 4.0f ) * sinf( 1 * y * m_pBubbles[i].wobble );
        z += 0.1f*sqrtf( y / 4.0f ) * cosf( 1 * y * m_pBubbles[i].wobble );

        m_pBubbles[i].vPos0 = m_vBubbleEmitterPos;

        m_pBubbles[i].vPos.x = m_pBubbles[i].vPos0.x + x;
        m_pBubbles[i].vPos.y = m_pBubbles[i].vPos0.y + y;
        m_pBubbles[i].vPos.z = m_pBubbles[i].vPos0.z + z;
    }
}




//-----------------------------------------------------------------------------
// Name: CreatePixelShader()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreatePixelShader()
{
    // Setup the pixel shader for the dolphin. To blend in water caustics, we
    // want the following effect:
    //    result = t0 * ( v0 + t1 * v1 )
    // where:
    //    t0 = Base texture
    //    t1 = Water caustics texture
    //    v0 = Ambient light contribution
    //    v1 = Directional light contribution
    //
    // This can be encoded into a pixel shader with:
    //    1st stage: r0 = (t1*v1 + v0*1)
    //    Final:     A*B + (1-A)*C + D
    //               E*F = r0 * t0
    // where:
    //    A = Fog factor
    //    B = EF
    //    C = Fog color
    //    D = 0
    //    E = r0
    //    F = t0
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
    // Build factor from caustics and lighting (R0 = T1*V1 + V0)
    psd.PSRGBInputs[0]    = PS_COMBINERINPUTS( PS_REGISTER_T1  | PS_CHANNEL_RGB,
                                               PS_REGISTER_V1  | PS_CHANNEL_RGB,
                                               PS_REGISTER_V0  | PS_CHANNEL_RGB,
                                               PS_REGISTER_ONE | PS_CHANNEL_RGB );
    psd.PSAlphaInputs[0]  = PS_COMBINERINPUTS( PS_REGISTER_T1  | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_V1  | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_V0  | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA );
    psd.PSRGBOutputs[0]   = PS_COMBINEROUTPUTS( PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_R0,
                                                PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );
    psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS( PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_R0,
                                                PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );

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
    if( FAILED( m_pd3dDevice->CreatePixelShader( &psd, &m_dwDolphinPixelShader ) ) )
        return E_FAIL;


    
    // Setup the pixel shader for the seafloor. This is the same as above,
    // except we're also modulating in the dolphin shadow, stored in t2.
    ZeroMemory( &psd, sizeof(psd) );
    psd.PSCombinerCount = PS_COMBINERCOUNT( 2,
                                            PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
    psd.PSTextureModes  = PS_TEXTUREMODES( PS_TEXTUREMODES_PROJECT2D, PS_TEXTUREMODES_PROJECT2D,
                                           PS_TEXTUREMODES_PROJECT2D, PS_TEXTUREMODES_NONE );

    //------------- Stage 0 -------------
    // Build factor from caustics and lighting (R0 = T1*V1, R1=V0)
    psd.PSRGBInputs[0]    = PS_COMBINERINPUTS( PS_REGISTER_T1  | PS_CHANNEL_RGB,
                                               PS_REGISTER_V1  | PS_CHANNEL_RGB,
                                               PS_REGISTER_V0  | PS_CHANNEL_RGB,
                                               PS_REGISTER_ONE | PS_CHANNEL_RGB );
    psd.PSAlphaInputs[0]  = PS_COMBINERINPUTS( PS_REGISTER_T1  | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_V1  | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_V0  | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA );
    psd.PSRGBOutputs[0]   = PS_COMBINEROUTPUTS( PS_REGISTER_R0,
                                                PS_REGISTER_R1,
                                                PS_REGISTER_DISCARD,
                                                PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );
    psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS( PS_REGISTER_R0,
                                                PS_REGISTER_R1,
                                                PS_REGISTER_DISCARD,
                                                PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );

    //------------- Stage 1 -------------
    // Build factor from caustics and lighting (R0 = R0 * T2 + R1 )
    psd.PSRGBInputs[1]    = PS_COMBINERINPUTS( PS_REGISTER_R0  | PS_CHANNEL_RGB,
                                               PS_REGISTER_T2  | PS_CHANNEL_RGB,
                                               PS_REGISTER_R1  | PS_CHANNEL_RGB,
                                               PS_REGISTER_ONE | PS_CHANNEL_RGB );
    psd.PSAlphaInputs[1]  = PS_COMBINERINPUTS( PS_REGISTER_R0  | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_T2  | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_R1  | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA );
    psd.PSRGBOutputs[1]   = PS_COMBINEROUTPUTS( PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_R0,
                                                PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );
    psd.PSAlphaOutputs[1] = PS_COMBINEROUTPUTS( PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_R0,
                                                PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );

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
    if( FAILED( m_pd3dDevice->CreatePixelShader( &psd, &m_dwSeaFloorPixelShader ) ) )
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

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the file-based mesh objects for the dolphin
    if( FAILED( m_DolphinObject1.Create( m_pd3dDevice, "Models\\Dolphin1.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    if( FAILED( m_DolphinObject2.Create( m_pd3dDevice, "Models\\Dolphin2.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    if( FAILED( m_DolphinObject3.Create( m_pd3dDevice, "Models\\Dolphin3.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Get access to the mesh data (VB, IB, num indices, etc.). Note that we
    // can safely do this because we know this file contains only one mesh.
    m_pDolphinMesh1 = m_DolphinObject1.GetMesh(0);
    m_pDolphinMesh2 = m_DolphinObject2.GetMesh(0);
    m_pDolphinMesh3 = m_DolphinObject3.GetMesh(0);

    // Create vertex shader for the dolphin
    DWORD dwDolphinVertexDecl[] =
    {
        // First dolphin position
        D3DVSD_STREAM( 0 ),              // This data comes from stream 0
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // v0 = Position of first mesh
        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), // v3 = Normal
        D3DVSD_REG( 6, D3DVSDT_FLOAT2 ), // v6 = Tex coords

        // Second dolphin position
        D3DVSD_STREAM( 1 ),              // This data comes from stream 1
        D3DVSD_REG( 1, D3DVSDT_FLOAT3 ), // v1 = Position of second mesh
        D3DVSD_REG( 4, D3DVSDT_FLOAT3 ), // v4 = Normal
        D3DVSD_REG( 7, D3DVSDT_FLOAT2 ), // v7 = Tex coords

        // Third dolphin position
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

    // Load the file-based mesh object for the seafloor
    if( FAILED( m_SeaFloorObject.Create( m_pd3dDevice, "Models\\SeaFloor.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Get access to the mesh data (VB, IB, num indices, etc.). Note that we
    // can safely do this because we know this file contains only one mesh.
    m_pSeaFloorMesh = m_SeaFloorObject.GetMesh(0);

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

    // Set the transform matrices
    D3DXVECTOR3 vEyePt      = D3DXVECTOR3( 0.0f, 0.0f,-10.0f );
    D3DXVECTOR3 vLookatPt   = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
    D3DXVECTOR3 vUpVec      = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &vUpVec );
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 1000.0f );

    // Create the particle system's vertex buffer
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( m_dwNumBubbles*sizeof(D3DXVECTOR3),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_XYZ,
                                                  D3DPOOL_MANAGED, &m_pBubbleVB ) ) )
        return E_FAIL;


    // Load the file-based mesh object which holds the dolphin path
    CXBMesh xbgPath;
    if( FAILED( xbgPath.Create( m_pd3dDevice, "Models\\Path.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    m_dwNumDolphinSplinePts = xbgPath.GetMesh(0)->m_dwNumVertices/2;
    m_DolphinSpline         = new D3DXVECTOR3[m_dwNumDolphinSplinePts];

    D3DXVECTOR3* pPathData;
    xbgPath.GetMesh(0)->m_VB.Lock( 0, 0, (BYTE**)&pPathData, 0 );
    for( DWORD i = 0; i < m_dwNumDolphinSplinePts; i++ )
    {
        m_DolphinSpline[i] = pPathData[i];
    }
    xbgPath.GetMesh(0)->m_VB.Unlock();

    // Create the dolphin's shadow texture
    if( FAILED( m_pd3dDevice->CreateTexture( SHADOW_SIZE, SHADOW_SIZE, 1, 0, 
                                             D3DFMT_X8R8G8B8, 0, &m_pShadowTexture ) ) )
        return E_FAIL;
    m_pShadowTexture->GetSurfaceLevel( 0, &m_pShadowSurface );

    // Load resources from the packed resource file. Resources (textures, vertex
    // buffers,etc.) can be packed this way so they load super quick and require
    // no load-time processing (like parsing a .bmp file, changing formats,
    // swizzling, etc.). See the code in XBResource.cpp to see how simple it is.
    if( FAILED( m_xprResources.Create( m_pd3dDevice, "Resource.xpr", 
                                       resource_NUM_RESOURCES ) ) )
        return E_FAIL;

    // Get pointers to our textures
    m_pDolphinTexture      = m_xprResources.GetTexture( resource_DolphinTexture_OFFSET );
    m_pSeaFloorTexture     = m_xprResources.GetTexture( resource_SeaFloorTexture_OFFSET );
    m_pBubbleTexture       = m_xprResources.GetTexture( resource_BubbleTexture_OFFSET );
    m_pCausticTextures[ 0] = m_xprResources.GetTexture( resource_WaterCaustic00_OFFSET );
    m_pCausticTextures[ 1] = m_xprResources.GetTexture( resource_WaterCaustic01_OFFSET );
    m_pCausticTextures[ 2] = m_xprResources.GetTexture( resource_WaterCaustic02_OFFSET );
    m_pCausticTextures[ 3] = m_xprResources.GetTexture( resource_WaterCaustic03_OFFSET );
    m_pCausticTextures[ 4] = m_xprResources.GetTexture( resource_WaterCaustic04_OFFSET );
    m_pCausticTextures[ 5] = m_xprResources.GetTexture( resource_WaterCaustic05_OFFSET );
    m_pCausticTextures[ 6] = m_xprResources.GetTexture( resource_WaterCaustic06_OFFSET );
    m_pCausticTextures[ 7] = m_xprResources.GetTexture( resource_WaterCaustic07_OFFSET );
    m_pCausticTextures[ 8] = m_xprResources.GetTexture( resource_WaterCaustic08_OFFSET );
    m_pCausticTextures[ 9] = m_xprResources.GetTexture( resource_WaterCaustic09_OFFSET );
    m_pCausticTextures[10] = m_xprResources.GetTexture( resource_WaterCaustic10_OFFSET );
    m_pCausticTextures[11] = m_xprResources.GetTexture( resource_WaterCaustic11_OFFSET );
    m_pCausticTextures[12] = m_xprResources.GetTexture( resource_WaterCaustic12_OFFSET );
    m_pCausticTextures[13] = m_xprResources.GetTexture( resource_WaterCaustic13_OFFSET );
    m_pCausticTextures[14] = m_xprResources.GetTexture( resource_WaterCaustic14_OFFSET );
    m_pCausticTextures[15] = m_xprResources.GetTexture( resource_WaterCaustic15_OFFSET );
    m_pCausticTextures[16] = m_xprResources.GetTexture( resource_WaterCaustic16_OFFSET );
    m_pCausticTextures[17] = m_xprResources.GetTexture( resource_WaterCaustic17_OFFSET );
    m_pCausticTextures[18] = m_xprResources.GetTexture( resource_WaterCaustic18_OFFSET );
    m_pCausticTextures[19] = m_xprResources.GetTexture( resource_WaterCaustic19_OFFSET );
    m_pCausticTextures[20] = m_xprResources.GetTexture( resource_WaterCaustic20_OFFSET );
    m_pCausticTextures[21] = m_xprResources.GetTexture( resource_WaterCaustic21_OFFSET );
    m_pCausticTextures[22] = m_xprResources.GetTexture( resource_WaterCaustic22_OFFSET );
    m_pCausticTextures[23] = m_xprResources.GetTexture( resource_WaterCaustic23_OFFSET );
    m_pCausticTextures[24] = m_xprResources.GetTexture( resource_WaterCaustic24_OFFSET );
    m_pCausticTextures[25] = m_xprResources.GetTexture( resource_WaterCaustic25_OFFSET );
    m_pCausticTextures[26] = m_xprResources.GetTexture( resource_WaterCaustic26_OFFSET );
    m_pCausticTextures[27] = m_xprResources.GetTexture( resource_WaterCaustic27_OFFSET );
    m_pCausticTextures[28] = m_xprResources.GetTexture( resource_WaterCaustic28_OFFSET );
    m_pCausticTextures[29] = m_xprResources.GetTexture( resource_WaterCaustic29_OFFSET );
    m_pCausticTextures[30] = m_xprResources.GetTexture( resource_WaterCaustic30_OFFSET );
    m_pCausticTextures[31] = m_xprResources.GetTexture( resource_WaterCaustic31_OFFSET );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Animation attributes for the dolphin
    FLOAT fPhase       = m_fAppTime/3;
    FLOAT fKickFreq    = 6*fPhase;
    FLOAT fBlendWeight = sinf( fKickFreq );

    // Get the parametric value u for the time-based dolphin position on it's
    // path, where 0 < u < NUM_SPLINE_POINTS.
    FLOAT u = m_dwNumDolphinSplinePts * ((m_fAppTime/g_fCycle) - floorf(m_fAppTime/g_fCycle));

    // Get the dolphin position and tangent from the spline path
    D3DXVECTOR3 vDolphinPos, vDolphinDir;
    XBUtil_GetSplinePoint( m_DolphinSpline, m_dwNumDolphinSplinePts, u, 
                           &vDolphinPos, &vDolphinDir );

    // Scale and orient the dolphin model
    D3DXMATRIX matScale, matRotateY, matDolphin;
    D3DXMatrixScaling( &matScale, 0.01f, 0.01f, 0.01f );
    D3DXMatrixRotationY( &matRotateY, D3DX_PI/2 );
    D3DXMatrixMultiply( &matDolphin, &matScale, &matRotateY );

    // Add the rotation and translation for the dolphin kick
    D3DXMATRIX matKickRotate;
    D3DXMatrixRotationX( &matKickRotate, -cosf(fKickFreq)/6 );
    D3DXMatrixMultiply( &matDolphin, &matDolphin, &matKickRotate );
    vDolphinPos.y += sinf(fKickFreq)/2;

    // Add the spline path. We can use the inverse of a lookat matrix to generate the
    // position and orientation of the dolphin on the path
    D3DXMATRIX matPathLookAt, matDolphinRotate, matDolphinTrans;
    D3DXMatrixLookAtLH( &matPathLookAt, &D3DXVECTOR3(0,0,0), &vDolphinDir, &D3DXVECTOR3(0,1,0) );
    D3DXMatrixInverse( &matDolphinRotate, NULL, &matPathLookAt );
    D3DXMatrixTranslation( &matDolphinTrans, vDolphinPos.x, vDolphinPos.y, vDolphinPos.z );
    D3DXMatrixMultiply( &matDolphin, &matDolphin, &matDolphinRotate );
    D3DXMatrixMultiply( &matDolphin, &matDolphin, &matDolphinTrans );

    // Position the bubble emitter position to correspond to the dolphin's blowhole.
    D3DXVECTOR3 vBlowHole( -150.0f, 40.0f, 0.0f );
    D3DXVec3TransformCoord( &m_vBubbleEmitterPos, &vBlowHole, &matDolphin );

    // Position the camera
    FLOAT u0 = m_dwNumDolphinSplinePts * (((m_fAppTime-1.5f)/g_fCycle) - floorf((m_fAppTime-1.5f)/g_fCycle));
    FLOAT u1 = m_dwNumDolphinSplinePts * (((m_fAppTime+0.2f)/g_fCycle) - floorf((m_fAppTime+0.2f)/g_fCycle));
    D3DXVECTOR3 vEyePt, vLookatPt;
    XBUtil_GetSplinePoint( m_DolphinSpline, m_dwNumDolphinSplinePts, u0, &vEyePt,    NULL );
    XBUtil_GetSplinePoint( m_DolphinSpline, m_dwNumDolphinSplinePts, u1, &vLookatPt, NULL );
    vEyePt *= 0.9f;
    D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &D3DXVECTOR3(0,1,0) );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

    // Vary the fog range, color, and ambient lighting based on depth
    FLOAT fDepthFactor = 1.0f + vEyePt.y/51.0f;
    FLOAT fFogEnd   = 50.0f + 100.0f * fDepthFactor;
    FLOAT fFogStart = 1.0f/(fFogEnd-1.0f);
    DWORD green = 0x0000ff00 & (DWORD)(0x00008000 * fDepthFactor);
    DWORD blue  = 0x000000ff & (DWORD)(0x000000ff * fDepthFactor);
    g_dwWaterColor = ( green | blue );

    DWORD dwColorFactor = (DWORD)( 255.0f * (1.0f-0.5f*fDepthFactor) );
    g_dwShadowColor = (dwColorFactor<<16) | (dwColorFactor<<8) | (dwColorFactor<<0);

    // Blend weight for vertex tweening
    FLOAT fWeight1 = ( fBlendWeight > 0.0f ) ? fabsf(fBlendWeight) : 0.0f;
    FLOAT fWeight2 = 1.0f - fabsf(fBlendWeight);
    FLOAT fWeight3 = ( fBlendWeight > 0.0f ) ? 0.0f : fabsf(fBlendWeight);

    // Values passed into the vertex shader for lighting, etc.
    D3DXVECTOR4 fConstants0( 0.0f, 0.0f, 0.0f, 0.00f );
    D3DXVECTOR4 fConstants1( 1.0f, 0.5f, 0.2f, 0.05f );
    D3DXVECTOR4 fLight( sinf(g_fLightAngle), cosf(g_fLightAngle), 0.0f, 0.0f );
    D3DXVECTOR4 fDiffuse( 1.00f, 1.00f, 1.00f, 1.00f );
    D3DXVECTOR4 fAmbient( 0.5f*fDepthFactor, 0.5f*fDepthFactor, 0.5f*fDepthFactor, 1.0f );
    D3DXVECTOR4 fFog( 0.5f, fFogEnd*fFogStart, fFogStart, 0.0f );
    D3DXVECTOR4 fCaustics( 0.05f, 0.05f, sinf(m_fAppTime)/8, cosf(m_fAppTime)/10 - m_fAppTime/10 );
    D3DXVECTOR4 fWeight( fWeight1, fWeight2, fWeight3, 0.0f );

    // Build matrix sets (and transpose them) for the vertex shaders
    D3DXMATRIX matDolphinWV, matDolphinWVP;
    D3DXMatrixMultiply( &matDolphinWV, &matDolphin, &m_matView );
    D3DXMatrixMultiply( &matDolphinWVP, &matDolphinWV, &m_matProj );
    D3DXMatrixTranspose( &matDolphinWV,  &matDolphinWV );
    D3DXMatrixTranspose( &matDolphinWVP,  &matDolphinWVP );

    D3DXMATRIX matSeaFloorWVP;
    D3DXMatrixMultiply( &matSeaFloorWVP, &m_matView, &m_matProj );
    D3DXMatrixTranspose( &matSeaFloorWVP,  &matSeaFloorWVP );

    // Set the vertex shader constants
    m_pd3dDevice->SetVertexShaderConstant(  0, &fConstants0,    1 ); // Some constants
    m_pd3dDevice->SetVertexShaderConstant(  1, &fConstants1,    1 ); // More constants
    m_pd3dDevice->SetVertexShaderConstant(  2, &fWeight,        1 );
    m_pd3dDevice->SetVertexShaderConstant( 20, &matDolphinWV,   4 ); // Dolphin WV matrix set
    m_pd3dDevice->SetVertexShaderConstant( 24, &matDolphinWVP,  4 ); // Dolphin WVP matrix set
    m_pd3dDevice->SetVertexShaderConstant( 30, &matSeaFloorWVP, 4 ); // Seafloor WVP matrix set
    m_pd3dDevice->SetVertexShaderConstant( 40, &fLight,         1 ); // Light direction
    m_pd3dDevice->SetVertexShaderConstant( 41, &fDiffuse,       1 ); // Diffuse color
    m_pd3dDevice->SetVertexShaderConstant( 42, &fAmbient,       1 ); // Ambient color
    m_pd3dDevice->SetVertexShaderConstant( 43, &fFog,           1 ); // Fog factors
    m_pd3dDevice->SetVertexShaderConstant( 44, &fCaustics,      1 ); // Misc constants

    // Animate the caustic textures
    DWORD tex = ((DWORD)(m_fAppTime*32))%32;
    m_pCurrentCausticTexture = m_pCausticTextures[tex];

    // Make this into a curve!
    FLOAT fAddBubbles = 0.0f;
    if( ((DWORD)m_fAppTime)%7 < 2 )
        fAddBubbles = m_fElapsedAppTime*50;

    UpdateBubbles( fAddBubbles );

    // Render the dolphin's shadow
    {
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,         g_dwShadowColor );

        // Get the current backbuffer and zbuffer
        LPDIRECT3DSURFACE8 pBackBuffer, pZBuffer;
        m_pd3dDevice->GetRenderTarget( &pBackBuffer );
        m_pd3dDevice->GetDepthStencilSurface( &pZBuffer );

        // Set the shadow texture as the rendertarget
        m_pd3dDevice->SetRenderTarget( m_pShadowSurface, NULL );

        // Set the viewport to be the correct size and clear it
        D3DVIEWPORT8 vpShadow = { 0, 0, SHADOW_SIZE, SHADOW_SIZE, 0.0f, 1.0f };
        m_pd3dDevice->SetViewport( &vpShadow );
        m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0L );

        // Set new vertex shader constants so that (a) the camera is at the
        // light position, looking at the dolphin and (b) we use fog to control
        // the only color that gets written is the shadow color.
        D3DXVECTOR3 vShadowEyePt = vDolphinPos + 10 * D3DXVECTOR3( sinf(g_fLightAngle), cosf(g_fLightAngle), 0.0f );
        D3DXMATRIX  matShadowView;
        D3DXMATRIX  matShadowProj;
        D3DXMatrixLookAtLH( &matShadowView, &vShadowEyePt, &vDolphinPos, &vDolphinDir );
        D3DXMatrixOrthoLH( &matShadowProj, 3.5f, 7.0f, 1.0f, 200.0f );
        D3DXVECTOR4 fFog( 0.5f, 0.1f, 0.0f, 0.0f );
                   
        D3DXMATRIX matDolphinShadowWV, matDolphinShadowWVP;
        D3DXMatrixMultiply( &matDolphinShadowWV,  &matDolphin,         &matShadowView );
        D3DXMatrixMultiply( &matDolphinShadowWVP, &matDolphinShadowWV, &matShadowProj );
        D3DXMatrixTranspose( &matDolphinShadowWV,  &matDolphinShadowWV );
        D3DXMatrixTranspose( &matDolphinShadowWVP, &matDolphinShadowWVP );

        m_pd3dDevice->SetVertexShaderConstant( 20, &matDolphinShadowWV,  4 ); // Dolphin WV matrix set
        m_pd3dDevice->SetVertexShaderConstant( 24, &matDolphinShadowWVP, 4 ); // Dolphin WVP matrix set
        m_pd3dDevice->SetVertexShaderConstant( 43, &fFog,                1 ); // Fog factors

        // Render the dolphin into the shadow texture
        m_pd3dDevice->SetPixelShader( NULL );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
        m_pd3dDevice->SetVertexShader( m_dwDolphinVertexShader );
        m_pd3dDevice->SetStreamSource( 0, &m_pDolphinMesh1->m_VB, m_pDolphinMesh1->m_dwVertexSize );
        m_pd3dDevice->SetStreamSource( 1, &m_pDolphinMesh2->m_VB, m_pDolphinMesh2->m_dwVertexSize );
        m_pd3dDevice->SetStreamSource( 2, &m_pDolphinMesh3->m_VB, m_pDolphinMesh3->m_dwVertexSize );
        m_pd3dDevice->SetIndices( &m_pDolphinMesh1->m_IB, 0 );
        m_pd3dDevice->DrawIndexedVertices( m_pDolphinMesh1->m_dwPrimType, 
                                           m_pDolphinMesh1->m_dwNumIndices,
                                           D3D__IndexData );

        // Change the rendertarget back to the main backbuffer
        D3DVIEWPORT8 vpBackBuffer = { 0, 0, 640, 480, 0.0f, 1.0f };
        m_pd3dDevice->SetRenderTarget( pBackBuffer, pZBuffer );
        m_pd3dDevice->SetViewport( &vpBackBuffer );
        pBackBuffer->Release();
        pZBuffer->Release();

        // Now, generate the texture matrix to project shadow onto the
        // seafloor. Here we just set up the matrix, and the vertex shader will
        // do the per-vertex computation for the texture coordinates.

        // Build shadow projection matrix
        D3DXMatrixOrthoLH( &matShadowProj, 15.0f*3.5f, 15.0f*7.0f, 1.0f, 200.0f );

        // Build matrix to shift coordinates from viewport space (-1,+1) to
        // texture space (0,1)
        D3DXMATRIX matTexShift;
        D3DXMatrixIdentity( &matTexShift );
        matTexShift._11 = +0.5f;   matTexShift._12 =  0.0f;
        matTexShift._21 =  0.0f;   matTexShift._22 = -0.5f;
        matTexShift._31 = +0.5f;   matTexShift._32 = +0.5f;

        // Concat the pieces to make the final texture matrix
        D3DXMATRIX matTexProj;
        D3DXMatrixIdentity( &matTexProj );
        D3DXMatrixMultiply( &matTexProj, &matTexProj, &matShadowView );
        D3DXMatrixMultiply( &matTexProj, &matTexProj, &matShadowProj );
        D3DXMatrixMultiply( &matTexProj, &matTexProj, &matTexShift );

        // Pass the matrix to the vertex shader via a vertex shader constant
        D3DXMATRIX matTexProjT;
        D3DXMatrixTranspose( &matTexProjT, &matTexProj );
        m_pd3dDevice->SetVertexShaderConstant( 50, &matTexProjT, 4 );
    }

    // Set the vertex shader constants
    m_pd3dDevice->SetVertexShaderConstant( 20, &matDolphinWV,  4 ); // Dolphin WV matrix set
    m_pd3dDevice->SetVertexShaderConstant( 24, &matDolphinWVP, 4 ); // Dolphin WVP matrix set
    m_pd3dDevice->SetVertexShaderConstant( 43, &fFog,          1 ); // Fog factors

    return S_OK;
}





//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::UpdateBubbles( FLOAT fAddBubbles )
{
    static FLOAT fBubblesToAdd = 0.0f;
    fBubblesToAdd += fAddBubbles;

    for( DWORD i=0; i<m_dwNumBubbles; i++ )
    {
        if( m_pBubbles[i].age > m_pBubbles[i].lifespan )
        {
            if( fBubblesToAdd > 1.0f )
            {
                fBubblesToAdd -= 1.0f;

                m_pBubbles[i].age      = m_pBubbles[i].age - m_pBubbles[i].lifespan;
                m_pBubbles[i].lifespan = 3.0f + ((FLOAT)rand()/(FLOAT)RAND_MAX) * 1.0f;
                m_pBubbles[i].vPos0    = m_vBubbleEmitterPos;
            }
        }
        else
            m_pBubbles[i].age += m_fElapsedAppTime;

        BUBBLE* pBubble = &m_pBubbles[i];

        FLOAT y = 1.2f*m_pBubbles[i].age;
        FLOAT r = m_pBubbles[i].spread * sqrtf( y / 10.0f );
        FLOAT x = r*sinf( m_pBubbles[i].angle );
        FLOAT z = r*cosf( m_pBubbles[i].angle );

        x += 0.1f*sqrtf( y / 4.0f ) * sinf( 1 * y * m_pBubbles[i].wobble );
        z += 0.1f*sqrtf( y / 4.0f ) * cosf( 1 * y * m_pBubbles[i].wobble );

        m_pBubbles[i].vPos.x = m_pBubbles[i].vPos0.x + x;
        m_pBubbles[i].vPos.y = m_pBubbles[i].vPos0.y + y;
        m_pBubbles[i].vPos.z = m_pBubbles[i].vPos0.z + z;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderBubbles()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderBubbles()
{
    // Set the world matrix
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set the view matrix
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

    // Set projection matrix
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
    
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

    // Set states for rendering the particles. Note that point sprites use 
    // stage 3.
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    m_pd3dDevice->SetTexture( 3, m_pBubbleTexture );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

    // Set the render states for using point sprites
    m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE,         FtoDW(0.15f) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,      FtoDW(0.00f) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,      FtoDW(0.00f) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,      FtoDW(1.00f) );

    // Set up the vertex buffer to be rendered
    D3DXVECTOR3* pVertices;
    DWORD        dwNumParticlesToRender = 0;
    m_pBubbleVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

    // Render each particle
    for( DWORD i=0; i<m_dwNumBubbles; i++ )
    {
        if( m_pBubbles[i].age < m_pBubbles[i].lifespan )
        {
            pVertices->x = m_pBubbles[i].vPos.x;
            pVertices->y = m_pBubbles[i].vPos.y;
            pVertices->z = m_pBubbles[i].vPos.z;
            pVertices++;

            dwNumParticlesToRender++;
        }
    }

    // Unlock the vertex buffer
    m_pBubbleVB->Unlock();
    if( dwNumParticlesToRender > 0 )
    {
        m_pd3dDevice->SetStreamSource( 0, m_pBubbleVB, sizeof(D3DXVECTOR3) );
        m_pd3dDevice->SetVertexShader( D3DFVF_XYZ );
        m_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 0, dwNumParticlesToRender );
    }

    // Reset render states
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
    m_pd3dDevice->SetTexture( 3, NULL );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

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
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MIPFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

    // Turn on the pixel shader for the underwater effect, which blends in the
    // current caustic texture and the dolphin's shadow. Note that specular
    // must be enabled for the pixel shader to use the v1 register.
    m_pd3dDevice->SetTexture( 1, m_pCurrentCausticTexture );
    m_pd3dDevice->SetTexture( 2, m_pShadowTexture );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,   TRUE );

    // Render the seafloor. The vertex shader does the transformation, texture
    // projection (for the water caustics) and fog calculations.
    m_pd3dDevice->SetTexture( 0, m_pSeaFloorTexture );
    m_pd3dDevice->SetPixelShader( m_dwSeaFloorPixelShader );
    m_pd3dDevice->SetVertexShader( m_dwSeaFloorVertexShader );
    m_pd3dDevice->SetStreamSource( 0, &m_pSeaFloorMesh->m_VB, m_pSeaFloorMesh->m_dwVertexSize );
    m_pd3dDevice->SetIndices( &m_pSeaFloorMesh->m_IB, 0 );
    m_pd3dDevice->DrawIndexedVertices( m_pSeaFloorMesh->m_dwPrimType, 
                                       m_pSeaFloorMesh->m_dwNumIndices,
                                       D3D__IndexData );

    // Render the dolphin. The vertex shader does the vertex tweening, 
    // transformation, texture projection (for the water caustics) and fog
    // calculations.
    m_pd3dDevice->SetTexture( 0, m_pDolphinTexture );
    m_pd3dDevice->SetPixelShader( m_dwDolphinPixelShader );
    m_pd3dDevice->SetVertexShader( m_dwDolphinVertexShader );
    m_pd3dDevice->SetStreamSource( 0, &m_pDolphinMesh1->m_VB, m_pDolphinMesh1->m_dwVertexSize );
    m_pd3dDevice->SetStreamSource( 1, &m_pDolphinMesh2->m_VB, m_pDolphinMesh2->m_dwVertexSize );
    m_pd3dDevice->SetStreamSource( 2, &m_pDolphinMesh3->m_VB, m_pDolphinMesh3->m_dwVertexSize );
    m_pd3dDevice->SetIndices( &m_pDolphinMesh1->m_IB, 0 );
    m_pd3dDevice->DrawIndexedVertices( m_pDolphinMesh1->m_dwPrimType, 
                                       m_pDolphinMesh1->m_dwNumIndices,
                                       D3D__IndexData );

    // Render the bubble system
    RenderBubbles();

    // Show frame rate
    m_Font.Begin();
    m_Font.DrawText(  64, 50, 0xffffffff, L"Dolphin" );
    m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
    m_Font.End();

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




