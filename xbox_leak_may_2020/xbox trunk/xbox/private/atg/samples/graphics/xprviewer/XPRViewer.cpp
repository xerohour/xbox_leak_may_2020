//-----------------------------------------------------------------------------
// File: XPRViewer.cpp
//
// Desc: Demonstrates how to load resources from an XPR (Xbox Packed Resource)
//       file.  
//
// Hist: 02.12.01 - New for March XDK release
//
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
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Change texture\nsize" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Reload\nresources" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Cycle through\nresources" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Choose mipmap\nlevel" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
};

#define NUM_HELP_CALLOUTS 5




#define MAX_NUM_RESOURCES 1024      // maximum this sample can handle

#define FMT_SWIZZLED   0x1
#define FMT_LINEAR     0x2
#define FMT_COMPRESSED 0x4


const struct TEXTUREFORMAT
{
    WCHAR* name;
    DWORD  id;
    DWORD  type;
} g_TextureFormats[] = 
{
    { L"D3DFMT_A8R8G8B8",        D3DFMT_A8R8G8B8,      FMT_SWIZZLED    },
    { L"D3DFMT_LIN_A8R8G8B8",    D3DFMT_LIN_A8R8G8B8,  FMT_LINEAR      },
    { L"D3DFMT_X8R8G8B8",        D3DFMT_X8R8G8B8,      FMT_SWIZZLED    },
    { L"D3DFMT_LIN_X8R8G8B8",    D3DFMT_LIN_X8R8G8B8,  FMT_LINEAR      },
    { L"D3DFMT_R5G6B5",          D3DFMT_R5G6B5,        FMT_SWIZZLED    },
    { L"D3DFMT_LIN_R5G6B5",      D3DFMT_LIN_R5G6B5,    FMT_LINEAR      },
    { L"D3DFMT_A1R5G5B5",        D3DFMT_A1R5G5B5,      FMT_SWIZZLED    },
    { L"D3DFMT_LIN_A1R5G5B5",    D3DFMT_LIN_A1R5G5B5,  FMT_LINEAR      },
    { L"D3DFMT_A4R4G4B4",        D3DFMT_A4R4G4B4,      FMT_SWIZZLED    },
    { L"D3DFMT_LIN_A4R4G4B4",    D3DFMT_LIN_A4R4G4B4,  FMT_LINEAR      },
    { L"D3DFMT_DXT1",            D3DFMT_DXT1,          FMT_COMPRESSED  },
    { L"D3DFMT_DXT2",            D3DFMT_DXT2,          FMT_COMPRESSED  },
    { L"D3DFMT_DXT4",            D3DFMT_DXT4,          FMT_COMPRESSED  },
    { L"",                       0,                    0               },
};


// Define our vertex format
struct CUSTOMVERTEX
{
    D3DXVECTOR3 p;          // Position
    D3DXVECTOR3 n;          // normal
    D3DCOLOR    diffuse;    // Diffuse color
    FLOAT       tu, tv;     // Texture coordinates
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)



DWORD  g_ResourceCount = 0;     // count of loaded resources

// Macro to simplify reads from file
#define READFROMFILE( file, buff, size ) if( !ReadFile( file, buff, size, &cb, NULL ) ) { hr = E_FAIL; goto Done;}

DWORD exptbl[] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,0,0,0,0,0,0};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    // Member variables
    CXBHelp                 m_Help;                 // Help data
    BOOL                    m_bDrawHelp;            // Should we draw help info?
    CXBFont                 m_Font;                 // Font for rendering text

    LPDIRECT3DTEXTURE8      m_ppResources[MAX_NUM_RESOURCES]; // Array of pointers to resources
    DWORD                   m_dwTypes[MAX_NUM_RESOURCES]; // Array of resource types
    DWORD                   m_nCurrent;             // Currently displayed texture
    char                    m_strBundle[MAX_PATH];  // Name of the xpr bundle to load (w/out extension)

    LPDIRECT3DVERTEXBUFFER8 m_pQuadVB;              // Quad for displaying textures
    FLOAT                   m_fWidth;
    FLOAT                   m_fHeight;
    DWORD                   m_nMipLevel;            // which texture mipmap level to display

    HANDLE                  m_hfXPR;                // File handle for async i/o
    BYTE *                  m_pbHeaders;            // Header memory
    DWORD                   m_cbHeaders;            // Count of bytes of resource headers
    BYTE *                  m_pbData;               // Texture data memory
    DWORD                   m_cbData;               // Count of bytes of data
    OVERLAPPED              m_overlapped;           // OVERLAPPED structure for async I/O
    BOOL                    m_bLoading;             // Still doing async I/O?

    // Creates and sets up our vertex buffer
    HRESULT InitGeometry( FLOAT fWidth, FLOAT fHeight, FLOAT fMaxU, FLOAT fMaxV );

    // Loads the resources out of the specified bundle
    HRESULT LoadResourcesFromXPR( LPDIRECT3DDEVICE8 pDevice, LPSTR strFileBase );

    // Called when Async I/O is complete to register the resources
    HRESULT OnIOComplete();

public:
    CXBoxSample();
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();
};




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
void __cdecl main()
{
    CXBoxSample xbApp;

    if( FAILED( xbApp.Create() ) )
        return;

    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Initialize member varaibles
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    m_bDrawHelp     = FALSE;
    m_pQuadVB       = NULL;
    m_fWidth        = 1.0f;
    m_fHeight       = 1.0f;
    m_nCurrent      = 0;
    m_pbHeaders     = NULL;
    m_cbHeaders     = 0;
    m_pbData        = NULL;
    m_bLoading      = FALSE;
    m_nMipLevel     = -1;
    lstrcpyA( m_strBundle, "D:\\Media\\Textures\\textures.xpr" );
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::InitGeometry( FLOAT fWidth, FLOAT fHeight, FLOAT fMaxU, FLOAT fMaxV )
{
    // Create and lock our vertex buffer
    if( NULL == m_pQuadVB )
    {
        if( FAILED( m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( CUSTOMVERTEX ), D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &m_pQuadVB ) ) )
            return E_FAIL;
    }
    
    // Set up a quad in the vertex buffer
    CUSTOMVERTEX* pVertices;
    if( FAILED( m_pQuadVB->Lock( 0, 0, (BYTE **)&pVertices, 0L ) ) )
        return E_FAIL;

    pVertices[0].p  = D3DXVECTOR3( -fWidth/2, -fHeight/2, 1.0f );
    pVertices[0].n  = D3DXVECTOR3(  0.0f, 0.0f, -1.0f );
    pVertices[0].diffuse = 0xFFFFFFFF;
    pVertices[0].tu = 0.0f; 
    pVertices[0].tv = fMaxV;

    pVertices[1].p  = D3DXVECTOR3( -fWidth/2, fHeight/2, 1.0f );
    pVertices[1].n  = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    pVertices[1].diffuse = 0xFFFFFFFF;
    pVertices[1].tu = 0.0f; 
    pVertices[1].tv = 0.0f;

    pVertices[2].p  = D3DXVECTOR3( fWidth/2, -fHeight/2, 1.0f );
    pVertices[2].n  = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    pVertices[2].diffuse = 0xFFFFFFFF;
    pVertices[2].tu = fMaxU; 
    pVertices[2].tv = fMaxV;

    pVertices[3].p  = D3DXVECTOR3( fWidth/2, fHeight/2, 1.0f );
    pVertices[3].n  = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    pVertices[3].diffuse = 0xFFFFFFFF;
    pVertices[3].tu = fMaxU; 
    pVertices[3].tv = 0.0f;

    m_pQuadVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Performs whatever set-up is needed, for example, loading textures,
//       creating vertex buffers, etc.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set up our vertex buffer
    if( FAILED( InitGeometry( m_fWidth, m_fHeight, 1.0f, 1.0f ) ) )
        return E_FAIL;

    // Set up our view, projection, and world matrices
    D3DXMATRIX matView;
    D3DXMatrixTranslation( &matView, 0.0f, 0.0f, 1.0f );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/3, 1.0f, 1.0f, 10.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up a light
    D3DLIGHT8 m_Light;

    ZeroMemory( &m_Light, sizeof( D3DLIGHT8 ) );
    m_Light.Type = D3DLIGHT_DIRECTIONAL;
    m_Light.Diffuse.r = 1.0f;
    m_Light.Diffuse.g = 1.0f;
    m_Light.Diffuse.b = 1.0f;
    m_Light.Diffuse.a = 1.0f;
    m_Light.Direction = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    m_pd3dDevice->SetLight( 0, &m_Light );
    m_pd3dDevice->LightEnable( 0, TRUE );

    // Load our resources from the file
    if( FAILED( LoadResourcesFromXPR( m_pd3dDevice, m_strBundle ) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Handles controller input each frame
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Check for IO completion
    if( m_bLoading && HasOverlappedIoCompleted( &m_overlapped ) )
        OnIOComplete();

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    if( !m_bLoading )
    {
        // Left thumbstick resizes quad
        m_fWidth  = max( 0.01f, min( 2.0f, m_fWidth  *= (0.01f*m_DefaultGamepad.fX1+1) ) );
        m_fHeight = max( 0.01f, min( 2.0f, m_fHeight *= (0.01f*m_DefaultGamepad.fY1+1) ) );

        // A button reloads the resources
        if( m_DefaultGamepad.bPressedAnalogButtons[0] )
        {
            if( FAILED( LoadResourcesFromXPR( m_pd3dDevice, m_strBundle ) ) )
                return E_FAIL;
            m_nCurrent  = 0;
            m_fWidth    = 1.0f;
            m_fHeight   = 1.0f;
            m_nMipLevel = -1;
        }

        // B button cycles between resources
        if( m_DefaultGamepad.bPressedAnalogButtons[1] )
        {
            m_nCurrent  = ( m_nCurrent + 1 ) % g_ResourceCount;
            m_fWidth    = 1.0f;
            m_fHeight   = 1.0f;
            m_nMipLevel = -1;
        }

        // DPAD moves between mipmap levels
        if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP)
            m_nMipLevel++;
        if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN)
        {
            if( m_nMipLevel != -1 )
                m_nMipLevel--;
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Performs the graphics operations to render the texture on screen
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    WCHAR strDescription[MAX_PATH];

    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    RenderGradientBackground( 0x0000ffff, 0x00ffffff );

    wcscpy(strDescription, L"");
    if( m_bLoading )
    {
        // If we're still reading from disk, display a loading message
        m_Font.DrawText( 150, 200, 0xffff0000, L"Loading resources from XPR..." );
    }
    else
    {
        DWORD dwFmt, *pRes, dwDim, dwU, dwV, dwP, dwLevels, dwCube;
        WCHAR *pTexType, strRes[256], strLevels[256];

        // Get info about resource and prepare description string
        switch( m_dwTypes[m_nCurrent] )
        {
            case D3DCOMMON_TYPE_TEXTURE:
                pRes  = (DWORD *)m_ppResources[m_nCurrent];
                dwFmt = (*(pRes+3) & D3DFORMAT_FORMAT_MASK) >> D3DFORMAT_FORMAT_SHIFT;
                for(UINT x=0; g_TextureFormats[x].id != 0; x++)
                    if(g_TextureFormats[x].id == dwFmt)
                        break;
                if(g_TextureFormats[x].id != 0)
                {
                    // get information about the texture
                    dwDim    = (*(pRes+3) & D3DFORMAT_DIMENSION_MASK) >> D3DFORMAT_DIMENSION_SHIFT;
                    dwLevels = (*(pRes+3) & D3DFORMAT_MIPMAP_MASK) >> D3DFORMAT_MIPMAP_SHIFT;
                    dwCube   = (*(pRes+3) & D3DFORMAT_CUBEMAP);
                    if(g_TextureFormats[x].type == FMT_LINEAR)
                    {
                        dwU = (*(pRes+4) & D3DSIZE_WIDTH_MASK)+1;
                        dwV = ((*(pRes+4) & D3DSIZE_HEIGHT_MASK) >> D3DSIZE_HEIGHT_SHIFT) + 1;
                    }
                    else
                    {
                        dwU = exptbl[(*(pRes+3) & D3DFORMAT_USIZE_MASK) >> D3DFORMAT_USIZE_SHIFT];
                        dwV = exptbl[(*(pRes+3) & D3DFORMAT_VSIZE_MASK) >> D3DFORMAT_VSIZE_SHIFT];
                        dwP = exptbl[(*(pRes+3) & D3DFORMAT_PSIZE_MASK) >> D3DFORMAT_PSIZE_SHIFT];
                    }

                    if(dwDim == 2)
                    {
                        swprintf( strRes, L"%dx%d", dwU, dwV );
                        pTexType = L"2D Texture";
                    }
                    else if(dwCube)
                    {
                        swprintf( strRes, L"%dx%d", dwU, dwV );
                        pTexType = L"Cubemap";
                    }
                    else
                    {
                        pTexType = L"3D Texture";
                        swprintf( strRes, L"%dx%dx%d", dwU, dwV, dwP );
                    }

                    if( dwLevels == 1 )
                        swprintf( strLevels, L"1 level");
                    else
                    {
                        if( m_nMipLevel == -1 )
                            swprintf( strLevels, L"%d levels", dwLevels );
                        else
                        {
                            if( m_nMipLevel >= dwLevels )
                                m_nMipLevel = dwLevels - 1; // constrain choice of miplevel to appropriate range
                            swprintf( strLevels, L"level %d of %d levels", m_nMipLevel, dwLevels );
                        }
                    }

                    swprintf( strDescription, L"Resource %d of %d is a %s\n%s %s %s\n", 
                                       m_nCurrent + 1, g_ResourceCount, pTexType, strRes, 
                                       g_TextureFormats[x].name, strLevels );
                }
                else
                {
                    swprintf( strDescription, L"Resource %d of %d is a Texture\n", 
                                       m_nCurrent + 1, g_ResourceCount );
                }
                break;

            case D3DCOMMON_TYPE_VERTEXBUFFER:
                swprintf( strDescription, L"Resource %d of %d is a VertexBuffer\n", 
                                   m_nCurrent + 1, g_ResourceCount);
                break;
            case D3DCOMMON_TYPE_INDEXBUFFER:
                swprintf( strDescription, L"Resource %d of %d is an Index Buffer\n",
                                   m_nCurrent + 1, g_ResourceCount );
        }

        // Draw quad
        if( m_dwTypes[m_nCurrent] == D3DCOMMON_TYPE_TEXTURE )
        {
            pRes  = (DWORD *)m_ppResources[m_nCurrent];
            dwFmt = (*(pRes+3) & D3DFORMAT_FORMAT_MASK) >> D3DFORMAT_FORMAT_SHIFT;
            for(UINT x=0; g_TextureFormats[x].id != 0; x++)
                if(g_TextureFormats[x].id == dwFmt)
                    break;
            if( g_TextureFormats[x].id != 0 )
            {
                // Set up quad to render
                if(g_TextureFormats[x].type == FMT_LINEAR)
                {
                    dwU = (*(pRes+4) & D3DSIZE_WIDTH_MASK)+1;
                    dwV = ((*(pRes+4) & D3DSIZE_HEIGHT_MASK) >> D3DSIZE_HEIGHT_SHIFT) + 1;
                    InitGeometry( m_fWidth, m_fHeight, (FLOAT)dwU, (FLOAT)dwV );
                }
                else
                {
                    InitGeometry( m_fWidth, m_fHeight, 1.0f, 1.0f );
                }
            }

            // Set up our render and texture stage states
            m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
            m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
    
            m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,    D3DBLEND_SRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,   D3DBLEND_INVSRCALPHA );
            
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSW,  D3DTADDRESS_CLAMP );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
            if( m_nMipLevel == -1 )
            {
                // blend between miplevels for ordinary texture mapping
                m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
                m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAXMIPLEVEL, 0);  // use the most-detailed level as appropriate
                m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPMAPLODBIAS, 0);        
            }
            else
            {
                // draw just the desired miplevel
                m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
                m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
                m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT );
                m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAXMIPLEVEL, m_nMipLevel);    // use just the desired miplevel
                m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPMAPLODBIAS, FtoDW(-100.f));    // bias choice to desired miplevel
            }
 
            // Select our texture and quad
            m_pd3dDevice->SetTexture( 0, m_ppResources[m_nCurrent] );
            m_pd3dDevice->SetStreamSource( 0, m_pQuadVB, sizeof( CUSTOMVERTEX ) );

            // Draw the quad
            m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

            // Unselect the texture
            m_pd3dDevice->SetTexture( 0, NULL );
        }
    }

    // Show title, frame rate, resource description, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"XPRViewer" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.DrawText(  64, 80, 0xff0000ff, strDescription );
       m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SizeOfResource()
// Desc: Determines the size, in bytes, of the D3DResource pointed to by 
//       pRes.
//-----------------------------------------------------------------------------
static DWORD SizeOfResource( LPDIRECT3DRESOURCE8 pResource )
{
    switch( pResource->GetType() )
    {
        case D3DRTYPE_TEXTURE:
            return sizeof(D3DTexture);
        case D3DRTYPE_VOLUMETEXTURE:
            return sizeof(D3DVolumeTexture);
        case D3DRTYPE_CUBETEXTURE:
            return sizeof(D3DCubeTexture);
        case D3DRTYPE_VERTEXBUFFER:
            return sizeof(D3DVertexBuffer);
        case D3DRTYPE_INDEXBUFFER:
            return sizeof(D3DIndexBuffer);
        case D3DRTYPE_PALETTE:
            return sizeof(D3DPalette);
        default:
            return 0;
    }
}




//-----------------------------------------------------------------------------
// Name: LoadResourcesFromXPR()
// Desc: Loads all the texture resources from the given XPR. ppResources should 
//       be large enough to hold all the LPDIRECT3DRESOURCE8 pointers.  
//       The read is performed asynchronously, so the data isn't available
//       until OnIOComplete().
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::LoadResourcesFromXPR( LPDIRECT3DDEVICE8 pDevice, LPSTR strFileName )
{
    HRESULT     hr = S_OK;
    XPR_HEADER  xprh;
    HANDLE      hfHeader = INVALID_HANDLE_VALUE;
    DWORD       cb;

    // Free up the previous allocation, if necessary
    if( m_pbHeaders )
    {
        delete[] m_pbHeaders;
        m_pbHeaders = NULL;
        m_cbHeaders = 0;
    }
    if( m_pbData )
    {
        D3D_FreeContiguousMemory( m_pbData );
        m_pbData = NULL;
        m_cbData = 0;
    }

    // Read out the headers first
    hfHeader = CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                           OPEN_EXISTING, 0, NULL );
    if( hfHeader == INVALID_HANDLE_VALUE )
    {
        hr = E_FAIL;
        goto Done;
    }

    // Verify the XPR magic header
    READFROMFILE( hfHeader, &xprh, sizeof( XPR_HEADER) );
    if( xprh.dwMagic != XPR_MAGIC_VALUE )
    {
        hr = E_INVALIDARG;
        goto Done;
    }

    // Allocate memory for the headers
    m_cbHeaders = xprh.dwHeaderSize - 3 * sizeof( DWORD );
    m_pbHeaders = new BYTE[m_cbHeaders];
    if( !m_pbHeaders )
    {
        hr = E_OUTOFMEMORY;
        goto Done;
    }

    // Read in the headers
    READFROMFILE( hfHeader, m_pbHeaders, m_cbHeaders );

    CloseHandle( hfHeader );

    // Now read the data
    // File is opened with overlapped i/o and no buffering
    m_hfXPR = CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                          OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL );
    if( m_hfXPR == INVALID_HANDLE_VALUE )
    {
        hr = E_FAIL;
        goto Done;
    }

    // Allocate contiguous memory for the texture data
    m_cbData = xprh.dwTotalSize - xprh.dwHeaderSize;
    m_pbData = (BYTE *)D3D_AllocContiguousMemory( m_cbData, D3DTEXTURE_ALIGNMENT );
    if( !m_pbData )
    {
        hr = E_OUTOFMEMORY;
        goto Done;
    }

    // Set up our overlapped i/o struct
    ZeroMemory( &m_overlapped, sizeof( OVERLAPPED ) );
    m_overlapped.Offset = xprh.dwHeaderSize;

    // Start the read of the texture data
    if( !ReadFile( m_hfXPR, m_pbData, m_cbData, &cb, &m_overlapped ) )
    {
        if( GetLastError() == ERROR_IO_PENDING )
            m_bLoading = TRUE;
        else
        {
            // Error we weren't expecting
            hr = E_FAIL;
            goto Done;
        }
    }

Done:
    // Lots of cleanup to do
    if( INVALID_HANDLE_VALUE != hfHeader )
        CloseHandle( hfHeader );

    // If we had an error condition, we need to 
    // free memory and close the XPR file
    if( FAILED( hr ) )
    {
        m_bLoading = FALSE;
        if( INVALID_HANDLE_VALUE != m_hfXPR )
            CloseHandle( m_hfXPR );

        delete[] m_pbHeaders;
        if( m_pbData )
            D3D_FreeContiguousMemory( m_pbData );
    }

    return hr;
}




//-----------------------------------------------------------------------------
// Name: OnIOComplete()
// Desc: Called when async i/o is complete, so that we can copy the
//       texture data to video memory and register the resources.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::OnIOComplete()
{
    BYTE      * pbCurrent;
    LPDIRECT3DRESOURCE8 * ppResources = (LPDIRECT3DRESOURCE8 *)m_ppResources;

    // Loop over resources, calling Register()
    pbCurrent = m_pbHeaders;
    for( int i = 0; i < MAX_NUM_RESOURCES; i++ )
    {
        DWORD type = *((DWORD *)pbCurrent) & D3DCOMMON_TYPE_MASK;

        if((type != D3DCOMMON_TYPE_VERTEXBUFFER) &&
           (type != D3DCOMMON_TYPE_TEXTURE) &&
           (type != D3DCOMMON_TYPE_INDEXBUFFER))
            break;
        
        ppResources[i] = (LPDIRECT3DRESOURCE8)pbCurrent;
        m_dwTypes[i] = type;
        pbCurrent += SizeOfResource( ppResources[i] );

        // Index Buffers should not be Register()'d
        if( type != D3DCOMMON_TYPE_INDEXBUFFER )
            ppResources[i]->Register( m_pbData );
    }

    g_ResourceCount = i;

    // Done with async XPR load
    m_bLoading = FALSE;
    CloseHandle( m_hfXPR );
    m_hfXPR = INVALID_HANDLE_VALUE;

    return S_OK;
}

