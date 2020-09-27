//-----------------------------------------------------------------------------
// File: BenchMark.cpp
//
// Desc: Sample to draw a large number of triangles under various conditions
//       (number of textures, number of lights, fill mode, drawprim type, etc.)
//       to benchmark the performance of the graphics pipeline.
//
// Hist: 11.01.00 - Port from NVidia test for November XDK release
//       12.15.00 - Changes for December XDK release
//       02.16.00 - Changes for March XDK release
//
// Copyright (c) 2000 NVIDIA Corporation. All rights reserved.
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\ntri list" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\nfillmode" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Cycle num\ntextures" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Add/remove\ndir. light" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Add/remove\npt. light" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"Add/remove\nspot light" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Use to add or\nremove lights" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_2, L"Cycle display\ninfo" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
};

#define NUM_HELP_CALLOUTS 9




//-----------------------------------------------------------------------------
// Options for displaying info
//-----------------------------------------------------------------------------
enum DISPLAY_OPTIONS { DISPLAY_TITLEONLY=0, DISPLAY_INSTANTANEOUSSTATS, 
                       DISPLAY_RENDEROPTIONS, DISPLAY_ENUM_MAX };




//-----------------------------------------------------------------------------
// Geometry Constants
//-----------------------------------------------------------------------------
const DWORD g_dwRibbonCount  = 25;
const FLOAT g_fRibbonCurl    = 30.0f * 2.0f * D3DX_PI;
const FLOAT g_fRibbonPitch   = 3.25f;
const FLOAT g_fRibbonWidth   = 0.04f;
const FLOAT g_fRibbonRadius  = 0.05f;
const DWORD g_dwSubsInLength = 1200;
const DWORD g_dwSubsInWidth  = 5;




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont            m_Font;               // Font class
    CXBHelp            m_Help;               // Help class
    BOOL               m_bDrawHelp;          // Whether to draw help

    DWORD       m_dwNumTextures;
    BOOL        m_bUseTriList;

    int         m_dwNumLights;
    int         m_dwNumDirectionalLights;
    int         m_dwNumSpotLights;
    int         m_dwNumPointLights;

    FLOAT       m_fCumulativeTriPerSec;
    FLOAT       m_fMaxTriPerSec;
    FLOAT       m_fMinTriPerSec;
    DWORD       m_dwFrame;

    DISPLAY_OPTIONS m_dwDisplayInfo;

    // Timing stats
    FLOAT       m_fTPS;
    FLOAT       m_fSPS;
    FLOAT       m_fIPS;
    FLOAT       m_fVPS;

private:
    HRESULT InitVBs();
    HRESULT InitLights();
    HRESULT InitMaterials();

    LPDIRECT3DTEXTURE8 CreateTexture( DWORD dwWidth, DWORD dwHeight );

    VOID    DisplayInfo();

    WORD*                   m_pIndices;         // User ptr to indices
    LPDIRECT3DINDEXBUFFER8  m_pIndexBuffer;     // Index buffer

    BYTE*                   m_pRibbon1Vertices; // User ptr to verts
    BYTE*                   m_pRibbon2Vertices; // User ptr to verts
    LPDIRECT3DVERTEXBUFFER8 m_pRibbon1VB;       // VB of verts
    LPDIRECT3DVERTEXBUFFER8 m_pRibbon2VB;       // VB of verts

    LPDIRECT3DTEXTURE8      m_pTexture1;        // Texture 1
    LPDIRECT3DTEXTURE8      m_pTexture2;        // Texture 2

    DWORD                   m_dwFVF;            // Our current FVF
    DWORD                   m_dwFVFSize;        // FVF size

    DWORD                   m_dwNumIndices;     // # indices
    DWORD                   m_dwNumVertices;    // # verts

    D3DFILLMODE             m_wFillMode;        // Current fill mode

    D3DMATERIAL8 m_mtrlInside[g_dwRibbonCount];
    D3DMATERIAL8 m_mtrlOutside[g_dwRibbonCount];

    VOID InitIndices();

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
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
{
    // Initialize base class members
    m_d3dpp.BackBufferFormat       = D3DFMT_R5G6B5;
    m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Initialize members
    m_dwDisplayInfo          = DISPLAY_TITLEONLY;
    m_bDrawHelp              = FALSE;

    m_dwNumLights            = 0L;
    m_dwNumDirectionalLights = 1L;
    m_dwNumSpotLights        = 0L;
    m_dwNumPointLights       = 0L;

    m_pIndices               = NULL;
    m_pIndexBuffer           = NULL;

    m_pRibbon1Vertices       = NULL;
    m_pRibbon2Vertices       = NULL;
    m_pRibbon1VB             = NULL;
    m_pRibbon2VB             = NULL;
    
    m_pTexture1              = NULL;
    m_pTexture2              = NULL;

    m_wFillMode              = D3DFILL_SOLID;

    m_dwNumTextures          = 0;
    m_bUseTriList            = FALSE;

    m_fCumulativeTriPerSec   = 0.0f;
    m_fMaxTriPerSec          = 0.0f;
    m_fMinTriPerSec          = 1e10f;
    m_dwFrame                = 0L;

    m_fTPS                   = 0.0f;
    m_fSPS                   = 0.0f;
    m_fIPS                   = 0.0f;
    m_fVPS                   = 0.0f;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::InitMaterials()
{
    // set material
    ZeroMemory( m_mtrlInside,  sizeof(D3DMATERIAL8) * g_dwRibbonCount );
    ZeroMemory( m_mtrlOutside, sizeof(D3DMATERIAL8) * g_dwRibbonCount );
    
    for( DWORD i = 0; i<g_dwRibbonCount; i++ )
    {
        FLOAT r = 0.3f + 0.5f * float(rand()) / float(RAND_MAX);
        FLOAT g = 0.3f + 0.5f * float(rand()) / float(RAND_MAX);
        FLOAT b = 0.3f + 0.5f * float(rand()) / float(RAND_MAX);

        m_mtrlOutside[i].Diffuse.r = r;
        m_mtrlOutside[i].Diffuse.g = g;
        m_mtrlOutside[i].Diffuse.b = b;
        m_mtrlOutside[i].Diffuse.a = 1.0f;
        m_mtrlOutside[i].Ambient.r = r * 0.3f;
        m_mtrlOutside[i].Ambient.g = g * 0.3f;
        m_mtrlOutside[i].Ambient.b = b * 0.3f;
        m_mtrlOutside[i].Ambient.a = 1.0f;

        m_mtrlInside[i].Diffuse.r  = 1.0f;
        m_mtrlInside[i].Diffuse.g  = 0.0f;
        m_mtrlInside[i].Diffuse.b  = 0.0f;
        m_mtrlInside[i].Diffuse.a  = 1.0f;
        m_mtrlInside[i].Ambient.r  = 1.0f;
        m_mtrlInside[i].Ambient.g  = 0.0f;
        m_mtrlInside[i].Ambient.b  = 0.0f;
        m_mtrlInside[i].Ambient.a  = 1.0f;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitMaterials()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::InitLights()
{
    // Setup lights
    FLOAT fIntensity = 1.0f / ( m_dwNumDirectionalLights + m_dwNumPointLights + m_dwNumSpotLights );

    // Disable all our existing lights
    while( m_dwNumLights-- )
        m_pd3dDevice->LightEnable( m_dwNumLights, FALSE );

    // Set the new lights, keeping a count of the number of lights
    m_dwNumLights = 0;

    for( int i = 0; i < m_dwNumDirectionalLights; i++ )
    {
        D3DLIGHT8 light;
        ZeroMemory( &light, sizeof(light) );
        light.Type        = D3DLIGHT_DIRECTIONAL;
        light.Diffuse.r   = fIntensity;
        light.Diffuse.g   = fIntensity;
        light.Diffuse.b   = fIntensity;
        light.Direction.x =  0.2f;
        light.Direction.y =  0.3f;
        light.Direction.z = -0.8f;

        m_pd3dDevice->SetLight( m_dwNumLights, &light );
        m_pd3dDevice->LightEnable( m_dwNumLights, TRUE );
        m_dwNumLights++;
    }
    for( i = 0; i < m_dwNumPointLights; i++ )
    {
        D3DLIGHT8 light;
        ZeroMemory( &light, sizeof(light) );
        light.Type         = D3DLIGHT_POINT;
        light.Diffuse.r    = fIntensity;
        light.Diffuse.g    = fIntensity;
        light.Diffuse.b    = fIntensity;
        light.Position.x   =   0.0f;
        light.Position.y   =   0.0f;
        light.Position.z   =   1.0f;
        light.Range        = 999.0f;
        light.Attenuation0 =   0.0f;
        light.Attenuation1 =   0.0f;
        light.Attenuation2 =   0.8f;

        m_pd3dDevice->SetLight( m_dwNumLights, &light );
        m_pd3dDevice->LightEnable( m_dwNumLights, TRUE );
        m_dwNumLights++;
    }
    for( i = 0; i < m_dwNumSpotLights; i++ )
    {
        D3DLIGHT8 light;
        ZeroMemory( &light, sizeof(light) );
        light.Type         = D3DLIGHT_SPOT;
        light.Diffuse.r    = fIntensity;
        light.Diffuse.g    = fIntensity;
        light.Diffuse.b    = fIntensity;
        light.Position.x   =  -1.0f;
        light.Position.y   =  -5.0f;
        light.Position.z   =   6.0f;
        light.Direction.x  =   0.1f;
        light.Direction.y  =   0.5f;
        light.Direction.z  =  -0.6f;
        light.Range        = 999.0f;
        light.Theta        =   0.1f;
        light.Phi          =   0.5f;
        light.Falloff      =   1.0f;
        light.Attenuation0 =   1.0f;

        m_pd3dDevice->SetLight( m_dwNumLights, &light );
        m_pd3dDevice->LightEnable( m_dwNumLights, TRUE );
        m_dwNumLights++;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitVBs()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::InitVBs()
{
    // Clean these guys up if they've already been inited
    SAFE_DELETE_ARRAY( m_pIndices );
    SAFE_RELEASE( m_pIndexBuffer );
    SAFE_RELEASE( m_pRibbon1VB );
    SAFE_RELEASE( m_pRibbon1VB );

    // create vertex buffer
    m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL;

    if( m_dwNumTextures > 1 )
        m_dwFVF |= D3DFVF_TEX2;
    else if( m_dwNumTextures )
        m_dwFVF |= D3DFVF_TEX1;

    m_dwFVFSize     = D3DXGetFVFVertexSize(m_dwFVF);
    m_dwNumVertices = (g_dwSubsInLength + 1) * (g_dwSubsInWidth + 1);

    // Create our vertex buffers
    m_pd3dDevice->CreateVertexBuffer( m_dwNumVertices * m_dwFVFSize, D3DUSAGE_WRITEONLY, 
		                              m_dwFVF, D3DPOOL_DEFAULT, &m_pRibbon1VB );
    m_pd3dDevice->CreateVertexBuffer( m_dwNumVertices * m_dwFVFSize, D3DUSAGE_WRITEONLY, 
		                              m_dwFVF, D3DPOOL_DEFAULT, &m_pRibbon2VB );

    BYTE* pRibbon1Vertices;
    BYTE* pRibbon2Vertices;
    m_pRibbon1VB->Lock( 0, 0, (BYTE**)&pRibbon1Vertices, 0L );
    m_pRibbon2VB->Lock( 0, 0, (BYTE**)&pRibbon2Vertices, 0L );

    // Populate with vertices
    for( DWORD y = 0; y <= g_dwSubsInLength; y++ )
    {
        for( DWORD x = 0; x <= g_dwSubsInWidth; x++ )
        {
            // Compute point
            FLOAT fAngle    = g_fRibbonCurl  * y / g_dwSubsInLength;
            FLOAT fHeight   = g_fRibbonWidth * x / g_dwSubsInWidth  +
                              g_fRibbonPitch * y / g_dwSubsInLength -
                              g_fRibbonPitch * 0.5f;
            FLOAT fCosAngle = cosf(fAngle);
            FLOAT fSinAngle = sinf(fAngle);

            D3DXVECTOR3 p( fHeight, g_fRibbonRadius * fCosAngle, g_fRibbonRadius * fSinAngle );
	        D3DXVECTOR3 n( 0.0f, fCosAngle, fSinAngle );

            // Copy position
            *(D3DXVECTOR3*)pRibbon1Vertices =  p;   pRibbon1Vertices += 3*sizeof(FLOAT);
            *(D3DXVECTOR3*)pRibbon2Vertices =  p;   pRibbon2Vertices += 3*sizeof(FLOAT);

			// Copy normals
		    *(D3DXVECTOR3*)pRibbon1Vertices =  n;   pRibbon1Vertices += 3*sizeof(FLOAT);
			*(D3DXVECTOR3*)pRibbon2Vertices = -n;   pRibbon2Vertices += 3*sizeof(FLOAT);

			// Copy texture coords
            for( DWORD t=0; t<m_dwNumTextures; t++ )
            {
                FLOAT tv = ((FLOAT)y) / ((FLOAT)g_dwSubsInLength);
                FLOAT tu = ((FLOAT)x) / ((FLOAT)g_dwSubsInWidth);

                *(FLOAT*)pRibbon1Vertices = tu;   pRibbon1Vertices += 1*sizeof(FLOAT);
                *(FLOAT*)pRibbon1Vertices = tv;   pRibbon1Vertices += 1*sizeof(FLOAT);

                *(FLOAT*)pRibbon2Vertices = tu;   pRibbon2Vertices += 1*sizeof(FLOAT);
                *(FLOAT*)pRibbon2Vertices = tv;   pRibbon2Vertices += 1*sizeof(FLOAT);
            }
        }
    }

    m_pRibbon1VB->Unlock();
    m_pRibbon2VB->Unlock();

    // Setup indices
    m_dwNumIndices  = 1 + g_dwSubsInLength * (g_dwSubsInWidth * 2 + 1);
    m_pIndices      = new WORD[m_dwNumIndices];
    WORD* pwIndex   = m_pIndices;
    DWORD dwCurrent = 0;

    pwIndex[0] = 0; pwIndex++;
    for( y = 0; y < g_dwSubsInLength; y++ )
    {
        if( dwCurrent > 0xffff )
        {
            OUTPUT_DEBUG_STRING( "Index out of range - reduce geometry complexity" );
            return E_FAIL;
        }

        pwIndex[0] = WORD(dwCurrent + (g_dwSubsInWidth + 1));
        pwIndex++;
        dwCurrent++;
        for (DWORD x = 0; x < g_dwSubsInWidth; x++)
        {
            pwIndex[0]  = WORD(dwCurrent);
            pwIndex[1]  = WORD(dwCurrent + (g_dwSubsInWidth + 1));
            pwIndex    += 2;
            dwCurrent  ++;
        }

        dwCurrent += (g_dwSubsInWidth + 1) - 1;
        y++;
        if (y < g_dwSubsInLength)
        {
            pwIndex[0] = WORD(dwCurrent + (g_dwSubsInWidth + 1));
            pwIndex++;
            dwCurrent--;
            for (DWORD x = 0; x < g_dwSubsInWidth; x++)
            {
                pwIndex[0]  = WORD(dwCurrent);
                pwIndex[1]  = WORD(dwCurrent + (g_dwSubsInWidth + 1));
                pwIndex+= 2;
                dwCurrent --;
            }

            dwCurrent += (g_dwSubsInWidth + 1) + 1;
        }
    }

    // If we use tri lists, we convert the strip to independent triangles here
    if( m_bUseTriList )
    {
        DWORD dwNewIndexCount = (m_dwNumIndices - 2) * 3;
        WORD *pwNewIndices    = new WORD[dwNewIndexCount];

        DWORD a = m_pIndices[0];
        DWORD b = m_pIndices[1];
        for (DWORD i = 2,j = 0; i < m_dwNumIndices; i++,j+=3)
        {
            DWORD c = m_pIndices[i];
            pwNewIndices[j + 0] = (WORD)a;
            pwNewIndices[j + 1] = (WORD)b;
            pwNewIndices[j + 2] = (WORD)c;
            if (i & 1)
                b = c;
            else
                a = c;
        }

        // adopt new
        SAFE_DELETE_ARRAY( m_pIndices );
        m_dwNumIndices = dwNewIndexCount;
        m_pIndices     = pwNewIndices;
    }

    // Create our index buffer if we're using DrawIndexedPrimitive.
    m_pd3dDevice->CreateIndexBuffer( m_dwNumIndices * sizeof(WORD),
                                     D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
                                     D3DPOOL_DEFAULT, &m_pIndexBuffer );

    WORD* pIndices;
    m_pIndexBuffer->Lock( 0, m_dwNumIndices * sizeof(WORD),
                          (BYTE**)&pIndices, 0L );

    for( DWORD i = 0; i < m_dwNumIndices; i++ )
        pIndices[i] = m_pIndices[i];

    m_pIndexBuffer->Unlock();

    SAFE_DELETE_ARRAY( m_pIndices );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateTexture()
// Desc: Create a texture with a pattern in it.
//-----------------------------------------------------------------------------
LPDIRECT3DTEXTURE8 CXBoxSample::CreateTexture( DWORD dwWidth, DWORD dwHeight )
{
    LPDIRECT3DTEXTURE8 pTexture = NULL;
    D3DSURFACE_DESC    desc;
    D3DLOCKED_RECT     lock;

    // Create a texture
    if( FAILED( m_pd3dDevice->CreateTexture( dwWidth, dwHeight, 1, 0, 
                                             D3DFMT_A4R4G4B4, D3DPOOL_DEFAULT, 
                                             &pTexture ) ) )
        return NULL;

    // Lock and fill the texture
    pTexture->GetLevelDesc( 0, &desc );
    pTexture->LockRect( 0, &lock, NULL, 0L );
    WORD* pData16 = (WORD*)lock.pBits;

    for( DWORD y = 0; y < dwHeight; y++ )
    {
        for( DWORD x = 0; x < dwWidth; x++ )
        {
            DWORD c = x + (y>>5);

            if( y & 1 )
                *pData16++ = (WORD)( ( (c<<8) | (c<<4) | (c<<0) | 0xf000) ^ 0x0777 );
            else
                *pData16++ = (WORD)( ( (c<<8) | (c<<4) | (c<<0) | 0xf000) );
        }
    }

    // Swizzle, unlock and return the texture
    XBUtil_SwizzleTexture2D( &lock, &desc );
    pTexture->UnlockRect( 0 );

    return pTexture;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create lights, materials, textures, and vertex buffers
    InitLights();
    InitVBs();
    InitMaterials();
    m_pTexture1 = CreateTexture( 8, 256 );
    m_pTexture2 = CreateTexture( 8, 256 );

    // Set transforms
    D3DXMATRIX matView, matProj;
    D3DXMatrixTranslation( &matView, 0.0f, 0.0f, -3.1f );
    D3DXMatrixOrthoRH( &matProj, 3.35f, 2.60f, 1.0f, 20.0f );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    int  dwOldNumDirectionalLights = m_dwNumDirectionalLights;
    int  dwOldNumSpotLights        = m_dwNumSpotLights;
    int  dwOldNumPointLights       = m_dwNumPointLights;
    BOOL bOptionsChanged = FALSE;

    // Toggle triangle lists vs. triangle strips
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] ) 
    {
        m_bUseTriList = !m_bUseTriList;
        InitVBs();
        bOptionsChanged = TRUE;
    }

    // Cycle fill mode options
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] ) 
    {
        switch( m_wFillMode )
        {
            case D3DFILL_POINT:     m_wFillMode = D3DFILL_WIREFRAME; break;
            case D3DFILL_WIREFRAME: m_wFillMode = D3DFILL_SOLID;     break;
            case D3DFILL_SOLID:     m_wFillMode = D3DFILL_POINT;     break;
        }

        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, m_wFillMode );
        bOptionsChanged = TRUE;
    }

    // Cycle number of textures
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] ) 
    {
        m_dwNumTextures = (m_dwNumTextures+1) % 3;
        InitVBs();
        bOptionsChanged = TRUE;
    }

    // Use DPad UP with buttons A,B, and BLACK to add lights
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
    {
        if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > 0x00 ) 
            if( m_dwNumDirectionalLights + m_dwNumPointLights + m_dwNumSpotLights < 8 )
                m_dwNumDirectionalLights++;

        if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_B] > 0x00 ) 
            if( m_dwNumDirectionalLights + m_dwNumPointLights + m_dwNumSpotLights < 8 )
                m_dwNumPointLights++;

        if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 0x00 ) 
            if( m_dwNumDirectionalLights + m_dwNumPointLights + m_dwNumSpotLights < 8 )
                m_dwNumSpotLights++;
    }

    // Use DPad DOWN with buttons A,B, and BLACK to remove lights
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
    {
        if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > 0x00 ) 
            m_dwNumDirectionalLights = max( m_dwNumDirectionalLights-1, 0 );

        if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_B] > 0x00 ) 
            m_dwNumPointLights = max( m_dwNumPointLights-1, 0 );

        if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 0x00 ) 
            m_dwNumSpotLights = max( m_dwNumSpotLights-1, 0 );
    }

    // If any lights were added or removed, reinit the lights
    if( ( dwOldNumDirectionalLights != m_dwNumDirectionalLights ) ||
        ( dwOldNumSpotLights        != m_dwNumSpotLights )        ||
        ( dwOldNumPointLights       != m_dwNumPointLights ) )
    {
        InitLights();
        bOptionsChanged = TRUE;
    }

    // Cycle through the information displayed
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START ) 
    {
        m_dwDisplayInfo = (DISPLAY_OPTIONS)( (m_dwDisplayInfo+1) % DISPLAY_ENUM_MAX );
        bOptionsChanged = TRUE;
    }

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
        bOptionsChanged = TRUE;
    }

    // Reset stats counters if the options changed
    if( bOptionsChanged )
    {
        m_dwFrame              = 0L;
        m_fCumulativeTriPerSec = 0.0f;
        m_fMaxTriPerSec        = 0.0f;
        m_fMinTriPerSec        = 1e10f;
    }

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
    // Calculate number of primitives (per ribbon) to render
    DWORD            dwTriCount = m_bUseTriList ? m_dwNumIndices/3 : m_dwNumIndices-2;
    D3DPRIMITIVETYPE dwPrimType = m_bUseTriList ? D3DPT_TRIANGLELIST : D3DPT_TRIANGLESTRIP;

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x00000000, 1.0f, 0L );

    // Set states for rendering the outside of the ribbons
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CW );

    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    if( m_dwNumTextures > 0 )
    {
        m_pd3dDevice->SetTexture( 0, m_pTexture1 );

        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

        if( m_dwNumTextures > 1 )
        {
            m_pd3dDevice->SetTexture( 1, m_pTexture2 );
            m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
            m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD );
            m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
            m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
            m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_ADD );
            m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
        }
    }

    // Setup to render vertex buffers before any DrawPrim() calls take place
    m_pd3dDevice->SetVertexShader( m_dwFVF );
    m_pd3dDevice->SetStreamSource( 0, m_pRibbon1VB, m_dwFVFSize );
    m_pd3dDevice->SetIndices( m_pIndexBuffer, 0 );

    // Draw outsides of ribbons
    for( DWORD i = 0; i < g_dwRibbonCount; i++ )
    {
        // Position and rotation of ribbon
        FLOAT y      = 0.75f * g_fRibbonPitch * ( ((FLOAT)i)/(g_dwRibbonCount-1) - 0.5f );
        FLOAT fAngle = ( 3.0f * m_fTime ) + ( i * 0.4f );

        // Set our world transform
        D3DXMATRIX matWorld, matTrans, matRotate;
        D3DXMatrixTranslation( &matTrans, 0.0f, y, 0.0f );
        D3DXMatrixRotationX( &matRotate, fAngle );
        D3DXMatrixMultiply( &matWorld, &matRotate, &matTrans );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

        // Set material
        m_pd3dDevice->SetMaterial( &m_mtrlOutside[i] );

        // Draw outside ribbon
        m_pd3dDevice->DrawIndexedPrimitive( dwPrimType, 0, m_dwNumVertices, 0, 
                                            dwTriCount );
    }

    // Set states for rendering the insides of the ribbons
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

    if( m_dwNumTextures )
    {
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    }

    // Setup to render vertex buffers before any DrawPrim() calls take place
    m_pd3dDevice->SetVertexShader( m_dwFVF );
    m_pd3dDevice->SetStreamSource( 0, m_pRibbon2VB, m_dwFVFSize );
    m_pd3dDevice->SetIndices( m_pIndexBuffer, 0 );
    
    // Draw insides of ribbons
    for( i = 0; i < g_dwRibbonCount; i++ )
    {
        // Position and rotation of ribbon
        FLOAT y      = 0.75f * g_fRibbonPitch * ( ((FLOAT)i)/(g_dwRibbonCount-1) - 0.5f );
        FLOAT fAngle = ( 3.0f * m_fTime ) + ( i * 0.4f );

        // Set our world transform
        D3DXMATRIX matWorld, matTrans, matRotate;
        D3DXMatrixTranslation( &matTrans, 0.0f, y, 0.0f );
        D3DXMatrixRotationX( &matRotate, fAngle );
        D3DXMatrixMultiply( &matWorld, &matRotate, &matTrans );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

        // Set material
        m_pd3dDevice->SetMaterial( &m_mtrlInside[i] );

        // Draw inside ribbon
        m_pd3dDevice->DrawIndexedPrimitive( dwPrimType, 0, m_dwNumVertices, 0, 
                                            dwTriCount );
    }

    // Display help/args/stats
    DisplayInfo();

    // Get stats
    DWORD dwNumTrianglesRendered = 2 * g_dwRibbonCount * dwTriCount;
    DWORD dwNumVerticesRendered  = 2 * g_dwRibbonCount * m_dwNumIndices;
    DWORD dwNumBytesForIndices   = dwNumVerticesRendered * sizeof(WORD);
    DWORD dwNumBytesForVertices  = dwNumVerticesRendered * m_dwFVFSize;

    // Get timing stats
    m_fTPS = ((FLOAT)dwNumTrianglesRendered) / m_fElapsedTime;
    m_fSPS = ((FLOAT)dwNumVerticesRendered) / m_fElapsedTime;
    m_fIPS = ((FLOAT)dwNumBytesForIndices) / m_fElapsedTime;
    m_fVPS = ((FLOAT)dwNumBytesForVertices) / m_fElapsedTime;
    m_fCumulativeTriPerSec += m_fTPS;
    m_fMaxTriPerSec = max( m_fTPS, m_fMaxTriPerSec );
    m_fMinTriPerSec = min( m_fTPS, m_fMinTriPerSec );
    m_dwFrame++;

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DisplayInfo()
// Desc
//-----------------------------------------------------------------------------
VOID CXBoxSample::DisplayInfo()
{
    WCHAR str[128];
    FLOAT iY = 90;

    // Begin text drawing
    m_Font.Begin();

    // Draw help or the diaply info
    if( m_bDrawHelp )
    {
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    }
    else
    {
        // Show frame rate
        m_Font.DrawText(  64, 50, 0xffffffff, L"BenchMark" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        if( m_dwDisplayInfo == DISPLAY_INSTANTANEOUSSTATS )
        {
            m_Font.DrawText(  180, 50, 0xffffffff, L"- Instantaneous Stats" );

            swprintf( str, L"%d x %d", 640, 480 );
            m_Font.DrawText(  64, iY, 0xffffff00, L"Size:" );
            m_Font.DrawText( 384, iY, 0xffffffff, str, XBFONT_RIGHT );
            iY += 20;

            m_Font.DrawText(  64, iY, 0xffffff00, L"Z-Depth:" );
            switch( m_d3dpp.AutoDepthStencilFormat )
            {
                case D3DFMT_D16:
                case D3DFMT_LIN_D16:
                    m_Font.DrawText( 384, iY, 0xffffffff, L"16-bit", XBFONT_RIGHT );
                    break;
                case D3DFMT_D24S8:
                case D3DFMT_LIN_D24S8:
                    m_Font.DrawText( 384, iY, 0xffffffff, L"24-bit w/stencil", XBFONT_RIGHT );
                    break;
                default:
                    m_Font.DrawText( 384, iY, 0xffffffff, L"Unknown", XBFONT_RIGHT );
                    break;
            }
            iY += 40;

            swprintf( str, L"%0.3lf", m_fTPS * 0.000001f );
            m_Font.DrawText(  64, iY, 0xffffff00, L"Triangles per sec:\n" );
            m_Font.DrawText( 384, iY, 0xffffffff, str, XBFONT_RIGHT );
            m_Font.DrawText( 384, iY, 0xffffffff, L" MTris/s" );
            iY += 20;
            swprintf( str, L"%0.3lf", m_fSPS * 0.000001f );
            m_Font.DrawText(  64, iY, 0xffffff00, L"Vertices per sec:\n" );
            m_Font.DrawText( 384, iY, 0xffffffff, str, XBFONT_RIGHT );
            m_Font.DrawText( 384, iY, 0xffffffff, L" MVerts/s" );
            iY += 40;

            swprintf( str, L"%0.3lf", m_fIPS * 0.000001f );
            m_Font.DrawText(  64, iY, 0xffffff00, L"Index traffic:\n" );
            m_Font.DrawText( 384, iY, 0xffffffff, str, XBFONT_RIGHT );
            m_Font.DrawText( 384, iY, 0xffffffff, L" MBytes/s" );
            iY += 20;
            swprintf( str, L"%0.3lf", m_fVPS * 0.000001f );
            m_Font.DrawText(  64, iY, 0xffffff00, L"Vertex traffic:\n" );
            m_Font.DrawText( 384, iY, 0xffffffff, str, XBFONT_RIGHT );
            m_Font.DrawText( 384, iY, 0xffffffff, L" MBytes/s" );
            iY += 20;
        }

        if( m_dwDisplayInfo == DISPLAY_RENDEROPTIONS )
        {
            m_Font.DrawText(  180, 50, 0xffffffff, L"- Rendering Options" );

            m_Font.DrawText(  64, iY, 0xffffff00, L"Primitive Type:" );
            m_Font.DrawText( 264, iY, 0xffffffff, m_bUseTriList ? L"Triangle List" : L"Triangle Strip" );
            iY += 20;
            m_Font.DrawText(  64, iY, 0xffffff00, L"Fillmode:" );
            if( m_wFillMode == D3DFILL_POINT )     m_Font.DrawText( 264, iY, 0xffffffff, L"Point" );
            if( m_wFillMode == D3DFILL_WIREFRAME ) m_Font.DrawText( 264, iY, 0xffffffff, L"Wireframe" );
            if( m_wFillMode == D3DFILL_SOLID )     m_Font.DrawText( 264, iY, 0xffffffff, L"Solid" );
            iY += 20;
            swprintf( str, L"%d", m_dwNumTextures );
            m_Font.DrawText(  64, iY, 0xffffff00, L"Num Textures:" );
            m_Font.DrawText( 264, iY, 0xffffffff, str );
            iY += 20;
            swprintf( str, L"%d", m_dwNumDirectionalLights );
            m_Font.DrawText(  64, iY, 0xffffff00, L"Num Dir Lights:" );
            m_Font.DrawText( 264, iY, 0xffffffff, str );
            iY += 20;
            swprintf( str, L"%d", m_dwNumPointLights );
            m_Font.DrawText(  64, iY, 0xffffff00, L"Num Point Lights:" );
            m_Font.DrawText( 264, iY, 0xffffffff, str );
            iY += 20;
            swprintf( str, L"%d", m_dwNumSpotLights );
            m_Font.DrawText(  64, iY, 0xffffff00, L"Num Spot Lights:" );
            m_Font.DrawText( 264, iY, 0xffffffff, str );
            iY += 20;
            iY += 10;

            DWORD dwTriCount  = m_bUseTriList ? m_dwNumIndices/3 : m_dwNumIndices-2;
            swprintf( str, L"%d triangles", 2 * g_dwRibbonCount * dwTriCount );
            m_Font.DrawText(  64, iY, 0xffffff00, L"Triangles per frame:" );
            m_Font.DrawText( 264, iY, 0xffffffff, str );
            iY += 20;
            swprintf( str, L"%d indices", 2 * g_dwRibbonCount * m_dwNumIndices );
            m_Font.DrawText(  64, iY, 0xffffff00, L"Indices per frame:" );
            m_Font.DrawText( 264, iY, 0xffffffff, str );
            iY += 20;
            swprintf( str, L"%d bytes", m_dwFVFSize );
            m_Font.DrawText(  64, iY, 0xffffff00, L"FVFSize:" );
            m_Font.DrawText( 264, iY, 0xffffffff, str );
            iY += 20;
            iY += 10;
    
            swprintf( str, L"%0.3f Mtps", m_fCumulativeTriPerSec * 0.000001f / m_dwFrame );
            m_Font.DrawText(  64, iY, 0xffffff00, L"AvgTriPerSec:" );
            m_Font.DrawText( 264, iY, 0xffffffff, str );
            iY += 20;

            swprintf( str, L"%0.3f Mtps", m_fMaxTriPerSec * 0.000001f );
            m_Font.DrawText(  64, iY, 0xffffff00, L"MaxTriPerSec:" );
            m_Font.DrawText( 264, iY, 0xffffffff, str );
            iY += 20;

            swprintf( str, L"%0.3f Mtps", m_fMinTriPerSec * 0.000001f );
            m_Font.DrawText(  64, iY, 0xffffff00, L"MinTriPerSec:" );
            m_Font.DrawText( 264, iY, 0xffffffff, str );
        }
    }

    // End text drawing
    m_Font.End();
}




