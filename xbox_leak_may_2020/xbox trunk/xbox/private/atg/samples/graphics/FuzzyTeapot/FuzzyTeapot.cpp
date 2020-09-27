//-----------------------------------------------------------------------------
// File: FuzzyTeapot.cpp
//
// Desc: Example code showing how to use volume textures for simulating fuzzy
//       surfaces.
//
// Hist: 01.13.00 - New for February XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBUtil.h>
#include <xgraphics.h>




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Rotate" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_1, L"Zoom" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, L"Add fuzz" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_1, L"Remove fuzz" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Toggle help" },
};

#define NUM_HELP_CALLOUTS 5




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
const int   MAX_LAYERS =  8;
const FLOAT LAYER_BIAS =  0.02f;
const int   VOLTEXSIZE = 16;

#define irand(a) ((rand()*(a))>>15)
#define frand(a) ((float)rand()*(a)/32768.0f)

struct FUZZVERTEX
{
    FLOAT x, y, z;
    FLOAT tu, tv, tw;
};

#define D3DFVF_FUZZVERTEX (D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0))




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont                  m_Font;                     // Font class
    CXBHelp                  m_Help;                     // Help class
    BOOL                     m_bDrawHelp;                // Whether to draw help

    CXBMesh                  m_Mesh;                     // Mesh geometry
    LPDIRECT3DVERTEXBUFFER8  m_pMeshVB;
    LPDIRECT3DINDEXBUFFER8   m_pMeshIB;
    DWORD                    m_dwNumMeshVertices;
    DWORD                    m_dwNumMeshIndices;
    DWORD                    m_dwMeshFVF;
    DWORD                    m_dwMeshVertexSize;
    D3DPRIMITIVETYPE         m_dwMeshPrimType;
    DWORD                    m_dwNumMeshPrimitives;

    DWORD                    m_dwNumFuzzLayers;           // Num fuzz layers to render
    LPDIRECT3DVERTEXBUFFER8  m_pMeshFuzzVB[MAX_LAYERS];   // VBs for fuzz layers
    LPDIRECT3DVOLUMETEXTURE8 m_pFuzzTexture;              // Volume texture for fuzz

    HRESULT CreateFuzzLayers();                           // Creates fuzz layer VBs
    HRESULT CreateFuzzTexture();                          // Creates fuzz volume texture

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
    m_bDrawHelp          = FALSE;
    m_dwNumFuzzLayers    = MAX_LAYERS;
    m_pFuzzTexture       = NULL;
}




//-----------------------------------------------------------------------------
// Name: CreateFuzzLayers()
// Desc: Creates a separate VB for each fuzz layer. Each VB is a copy of the
//       original mesh's VB, but scaled slightly along vertex normals.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateFuzzLayers()
{
    struct MESHVERTEX
    {
        D3DXVECTOR3 p;
        D3DXVECTOR3 n;
    };

    MESHVERTEX* pSrcVertices;
    m_pMeshVB->Lock( 0, 0, (BYTE**)&pSrcVertices, 0 );

    for( DWORD dwLayer = 0; dwLayer < MAX_LAYERS; dwLayer++ )
    {
        if( FAILED( m_pd3dDevice->CreateVertexBuffer( m_dwNumMeshVertices * sizeof(FUZZVERTEX),
                                                      D3DUSAGE_WRITEONLY ,
                                                      D3DFVF_FUZZVERTEX,
                                                      D3DPOOL_DEFAULT, &m_pMeshFuzzVB[dwLayer] ) ) )
            return E_FAIL;

        FUZZVERTEX* pDstVertices;
        m_pMeshFuzzVB[dwLayer]->Lock( 0, 0, (BYTE**)&pDstVertices, 0 );

        for( DWORD i=0; i<m_dwNumMeshVertices; i++ )
        {
            pDstVertices[i].x  = pSrcVertices[i].p.x + dwLayer * LAYER_BIAS * pSrcVertices[i].n.x;
            pDstVertices[i].y  = pSrcVertices[i].p.y + dwLayer * LAYER_BIAS * pSrcVertices[i].n.y;
            pDstVertices[i].z  = pSrcVertices[i].p.z + dwLayer * LAYER_BIAS * pSrcVertices[i].n.z;

            pDstVertices[i].tu = 3.0f * atanf( pDstVertices[i].x / pDstVertices[i].z );
            pDstVertices[i].tv = 3.0f * atanf( pDstVertices[i].y / pDstVertices[i].x );
            pDstVertices[i].tw = ((FLOAT)dwLayer)/MAX_LAYERS;
        }

        m_pMeshFuzzVB[dwLayer]->Unlock();
    }
    
    m_pMeshVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateFuzzTexture()
// Desc: Creates the volume texture for the fuzz
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateFuzzTexture()
{
    HRESULT hr;

    // Create a volume texture
    hr = m_pd3dDevice->CreateVolumeTexture( VOLTEXSIZE, VOLTEXSIZE, VOLTEXSIZE, 
                                            1, 0, D3DFMT_A8R8G8B8, 
                                            D3DPOOL_MANAGED, &m_pFuzzTexture );
    if( FAILED(hr) )
        return hr;

    // Fill the volume texture with hair
    D3DVOLUME_DESC desc;
    D3DLOCKED_BOX lock;
    m_pFuzzTexture->GetLevelDesc( 0, &desc );
    m_pFuzzTexture->LockBox( 0, &lock, 0, 0 );
    ZeroMemory( lock.pBits, VOLTEXSIZE*VOLTEXSIZE*VOLTEXSIZE*4 );
    
    for( UINT u=0; u<VOLTEXSIZE; u++ )
    {
        for( UINT v=0; v<VOLTEXSIZE; v++ )
        {
            if( frand(1) >= 0.33f )
            {
                DWORD r = 0xff;
                DWORD g = 0xcc;
                DWORD b = 0x44;
                DWORD a = 0xff;
                DWORD dwADec = 0xff/VOLTEXSIZE + irand(8);

                for( UINT w=0; w<VOLTEXSIZE; w++ )
                {
                    BYTE* pBits = (BYTE *)lock.pBits + u*4 + lock.RowPitch * v + lock.SlicePitch * w ;
                    // Write the texel
                    *(DWORD*)pBits = (a<<24) + (r<<16) + (g<<8) + (b<<0);
                    
                    if( a > dwADec ) a -= dwADec;
                    else             a  = 0;
                }
            }
        }
    }

    // Swizzle and unlock the texture
    XBUtil_SwizzleTexture3D( &lock, &desc );
    m_pFuzzTexture->UnlockBox( 0 );

    return S_OK;
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

    // Create the font
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create a mesh (vertex and index buffers)
    if( FAILED( m_Mesh.Create( m_pd3dDevice, "Models\\Teapot.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    m_pMeshVB             = &m_Mesh.GetMesh(0)->m_VB;
    m_dwNumMeshVertices   =  m_Mesh.GetMesh(0)->m_dwNumVertices;
    m_pMeshIB             = &m_Mesh.GetMesh(0)->m_IB;
    m_dwNumMeshIndices    =  m_Mesh.GetMesh(0)->m_dwNumIndices;
    m_dwMeshFVF           =  m_Mesh.GetMesh(0)->m_dwFVF;
    m_dwMeshVertexSize    =  m_Mesh.GetMesh(0)->m_dwVertexSize;
    m_dwMeshPrimType      =  m_Mesh.GetMesh(0)->m_dwPrimType;
    m_dwNumMeshPrimitives = (D3DPT_TRIANGLELIST==m_dwMeshPrimType) ? m_dwNumMeshIndices/3 : m_dwNumMeshIndices-2;

    // Create the geoemtry for the fuzz
    if( FAILED( CreateFuzzLayers() ) )
        return E_FAIL;

    // Create the volume texture for the fuzz
    if( FAILED( CreateFuzzTexture() ) )
        return E_FAIL;

    // Set up proj matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 200.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Setup a base material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, .5f, .5f, .5f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Setup a light
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -0.5f, -1.0f, 1.0f );
    light.Diffuse.r = 1.0f;
    light.Diffuse.g = 1.0f;
    light.Diffuse.b = 0.0f;
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

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

    // Add or remove fuzz layers
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
    {
        if( m_dwNumFuzzLayers < MAX_LAYERS )
            m_dwNumFuzzLayers++;
    }
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
    {
        if( m_dwNumFuzzLayers > 0 )
            m_dwNumFuzzLayers--;
    }

    // Set up view matrix
    static D3DXVECTOR3 vEyePt( 0,0,-6 );
    if( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP )
        vEyePt.z += 10.0f*m_fElapsedTime;
    if( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN )
        vEyePt.z -= 10.0f*m_fElapsedTime;

    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0) );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Setup object matrix
    D3DXMATRIX matRotate;
    FLOAT fXRotate = m_DefaultGamepad.fX1*D3DX_PI*m_fElapsedTime;
    FLOAT fYRotate = m_DefaultGamepad.fY1*D3DX_PI*m_fElapsedTime;
    D3DXMatrixRotationYawPitchRoll( &matRotate, -fXRotate, -fYRotate, 0.0f );
    
    static D3DXMATRIX  matWorld( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
    matWorld._41 = matWorld._42 = 0.f;
    D3DXMatrixMultiply( &matWorld, &matWorld, &matRotate );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

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
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x00000000, 1.0f, 0L );
    
    // Draw a gradient filled background
    RenderGradientBackground( 0xff000000, 0xff0000ff );

    // Set default state
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,    D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,    D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSW,    D3DTADDRESS_WRAP );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE ); 
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,          0xffffffff );

    // Draw the underlying (fuzzless) mesh
    m_pd3dDevice->SetVertexShader( m_dwMeshFVF );
    m_pd3dDevice->SetIndices( m_pMeshIB, 0 );
    m_pd3dDevice->SetStreamSource( 0, m_pMeshVB, m_dwMeshVertexSize );
    m_pd3dDevice->DrawIndexedPrimitive( m_dwMeshPrimType, 0, m_dwNumMeshIndices,
                                        0, m_dwNumMeshPrimitives );

    // Setup states for drawing the fuzz, using the volume texture
    m_pd3dDevice->SetTexture( 0, m_pFuzzTexture );
    m_pd3dDevice->SetVertexShader( D3DFVF_FUZZVERTEX );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );

    // Draw the fuzz layers
    for( DWORD i = 0; i < m_dwNumFuzzLayers; i++ )
    {
        m_pd3dDevice->SetIndices( m_pMeshIB, 0 );
        m_pd3dDevice->SetStreamSource( 0, m_pMeshFuzzVB[i], sizeof(FUZZVERTEX) );
        m_pd3dDevice->DrawIndexedPrimitive( m_dwMeshPrimType, 0, m_dwNumMeshIndices,
                                            0, m_dwNumMeshPrimitives );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"FuzzyTeapot" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        WCHAR str[40];
        swprintf( str, L"Fuzz Layers: %d", m_dwNumFuzzLayers );
        m_Font.DrawText( 64, 90, 0xffffff00, str );

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




