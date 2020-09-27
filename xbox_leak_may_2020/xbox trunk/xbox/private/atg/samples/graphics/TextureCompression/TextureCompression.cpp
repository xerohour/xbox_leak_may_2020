//-----------------------------------------------------------------------------
// File: TextureCompression.cpp
//
// Desc: 
//
// Hist: 3.29.01 - Created
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
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Next method" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Next texture" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Next format" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"show difference" },
};

#define NUM_HELP_CALLOUTS 4


typedef struct 
{
    D3DXVECTOR4 p;
    D3DXVECTOR2 t;
} CUSTOMVERTEX;
#define FVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1)

WCHAR * g_aszLabels[] = {
    L"Original",
    L"D3DX",
    L"S3TCEncode",
};
#define NUM_LABELS 3

char * g_aszFiles[] = {
    "Textures\\tuscan.tga",
    "Textures\\gamepad.tga",
    "Textures\\Flame1.bmp",
    "Textures\\forestground.bmp",
    "Textures\\grydirt1.bmp",
    "Textures\\LightOakSm.bmp",
    "Textures\\procwood4.bmp",
    "Textures\\sdirt.bmp",
    "Textures\\StonehengeGround.bmp",
    "Textures\\stonehengerock.bmp",
    "Textures\\temprock.bmp",
    "Textures\\volslice003.bmp",
    "Textures\\Waterbumps.bmp",
};

D3DFORMAT g_adwFormats[] = { 
    D3DFMT_DXT1,
    D3DFMT_DXT2,
    D3DFMT_DXT3,
    D3DFMT_DXT4,
    D3DFMT_DXT5,
};


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

    HRESULT ReloadTexture();

    // Font and help
    CXBFont     m_Font;
    CXBHelp     m_Help;

    DWORD       m_dwCompression;
    DWORD       m_dwTexture;
    DWORD       m_dwFormat;
	DWORD		m_bDifference;
    LPDIRECT3DTEXTURE8 m_pTexOrig;
    LPDIRECT3DTEXTURE8 m_pTexD3DX;
    LPDIRECT3DTEXTURE8 m_pTexCompressed;
    LPDIRECT3DVERTEXBUFFER8 m_pvb;

    DWORD       m_dwCompPShader;     // Handle for pixel shader 1

    BOOL        m_bDrawHelp;
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

D3DFORMAT LinearFromSwizzled( D3DFORMAT fmt )
{
    switch( fmt )
    {
    case D3DFMT_A1R5G5B5:
        return D3DFMT_LIN_A1R5G5B5;
    case D3DFMT_A4R4G4B4:
        return D3DFMT_LIN_A4R4G4B4;
    case D3DFMT_A8B8G8R8:
        return D3DFMT_LIN_A8B8G8R8;
    case D3DFMT_A8R8G8B8:
        return D3DFMT_LIN_A8R8G8B8;
    case D3DFMT_B8G8R8A8:
        return D3DFMT_LIN_B8G8R8A8;
    case D3DFMT_R4G4B4A4:
        return D3DFMT_LIN_R4G4B4A4;
    case D3DFMT_R5G5B5A1:
        return D3DFMT_LIN_R5G5B5A1;
    case D3DFMT_R5G6B5:
        return D3DFMT_LIN_R5G6B5;
    case D3DFMT_R6G5B5:
        return D3DFMT_LIN_R6G5B5;
    case D3DFMT_R8G8B8A8:
        return D3DFMT_LIN_R8G8B8A8;
    case D3DFMT_X1R5G5B5:
        return D3DFMT_LIN_X1R5G5B5;
    case D3DFMT_X8R8G8B8:
        return D3DFMT_LIN_X8R8G8B8;
    }
    return D3DFMT_UNKNOWN;
}


//-----------------------------------------------------------------------------
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplication()
{
    m_bDrawHelp = FALSE;

    m_dwFormat       = NULL;
    m_dwTexture      = NULL;
    m_dwCompression  = 0;
    m_pTexOrig       = NULL;
    m_pTexD3DX       = NULL;
    m_pTexCompressed = NULL;

    m_bDifference    = 0;
}


HRESULT
CXBoxSample::ReloadTexture()
{
    D3DSURFACE_DESC sdSrc, sdDest, sdD3DX;
    D3DLOCKED_RECT lrSrc, lrDest, lrD3DX;
    BYTE * pb;
    LARGE_INTEGER liFreq, liA, liB, liC, liD;
    FLOAT fSecsPerTick, f1, f2;

    QueryPerformanceFrequency( &liFreq );
    fSecsPerTick = 1.0f / liFreq.QuadPart;

    if( m_pTexOrig )
        m_pTexOrig->Release();
    if( m_pTexD3DX )
        m_pTexD3DX->Release();
    if( m_pTexCompressed )
        m_pTexCompressed->Release();

    if( FAILED( XBUtil_CreateTexture( m_pd3dDevice, g_aszFiles[ m_dwTexture ], &m_pTexOrig, D3DFMT_A8R8G8B8 ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    m_pTexOrig->GetLevelDesc( 0, &sdSrc );
    m_pTexOrig->LockRect( 0, &lrSrc, NULL, D3DLOCK_READONLY );

    if( FAILED( m_pd3dDevice->CreateTexture( sdSrc.Width, sdSrc.Height, 1, 0, g_adwFormats[ m_dwFormat ], 0, &m_pTexD3DX ) ) )
        return E_OUTOFMEMORY;

    m_pTexD3DX->GetLevelDesc( 0, &sdD3DX );
    m_pTexD3DX->LockRect( 0, &lrD3DX, NULL, D3DLOCK_READONLY );
    LPDIRECT3DSURFACE8 pDest, pSrc;

    m_pTexD3DX->GetSurfaceLevel( 0, &pDest );
    m_pTexOrig->GetSurfaceLevel( 0, &pSrc );

    QueryPerformanceCounter( &liA );
    D3DXLoadSurfaceFromSurface( pDest, NULL, NULL, pSrc, NULL, NULL, D3DX_DEFAULT, 0 );
    QueryPerformanceCounter( &liB );

    D3DXSetDXT3DXT5( m_dwFormat == 1 || m_dwFormat == 3 );
    if( FAILED( m_pd3dDevice->CreateTexture( sdSrc.Width, sdSrc.Height, 1, 0, g_adwFormats[ m_dwFormat ], 0, &m_pTexCompressed ) ) )
        return E_OUTOFMEMORY;

    m_pTexCompressed->GetLevelDesc( 0, &sdDest );
    m_pTexCompressed->LockRect( 0, &lrDest, NULL, D3DLOCK_READONLY );

    XBUtil_UnswizzleTexture2D( &lrSrc, &sdSrc );

    DWORD dwSize = sdSrc.Width * sdSrc.Height;
    if( m_dwFormat == 0 )
        dwSize /= 2;
    
    dwSize = max( dwSize, m_dwFormat==0 ? DWORD(8) : DWORD(16) );

    pb = new BYTE[ dwSize ];
    QueryPerformanceCounter( &liC );
    XGCompressRect( pb, 
                    g_adwFormats[ m_dwFormat ], 
                    0,
                    sdSrc.Width, 
                    sdSrc.Height, 
                    lrSrc.pBits, 
                    LinearFromSwizzled( sdSrc.Format ), 
                    lrSrc.Pitch, 
                    0.5f,
                    ( m_dwFormat == 1 || m_dwFormat == 3 ) ? XGCOMPRESS_PREMULTIPLY : 0);
    QueryPerformanceCounter( &liD );
    memcpy( lrDest.pBits, pb, dwSize );
    delete[] pb;

    XBUtil_SwizzleTexture2D( &lrSrc, &sdSrc );

    m_pTexOrig->UnlockRect( 0 );
    m_pTexD3DX->UnlockRect( 0 );
    m_pTexCompressed->UnlockRect( 0 );

    f1 = fSecsPerTick * ( liB.QuadPart - liA.QuadPart );
    f2 = fSecsPerTick * ( liD.QuadPart - liC.QuadPart );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
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

    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( CUSTOMVERTEX ), 0, 0, 0, &m_pvb ) ) )
        return E_FAIL;

    CUSTOMVERTEX * pVertices;
    m_pvb->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = D3DXVECTOR4(  60 - 0.5f, 440 - 0.5f, 1.0f, 1.0f );  pVertices[0].t = D3DXVECTOR2( 0.0f, 1.0f );
    pVertices[1].p = D3DXVECTOR4(  60 - 0.5f,  40 - 0.5f, 1.0f, 1.0f );  pVertices[1].t = D3DXVECTOR2( 0.0f, 0.0f );
    pVertices[2].p = D3DXVECTOR4( 580 - 0.5f, 440 - 0.5f, 1.0f, 1.0f );  pVertices[2].t = D3DXVECTOR2( 1.0f, 1.0f );
    pVertices[3].p = D3DXVECTOR4( 580 - 0.5f,  40 - 0.5f, 1.0f, 1.0f );  pVertices[3].t = D3DXVECTOR2( 1.0f, 0.0f );
    m_pvb->Unlock();

    if( FAILED( XBUtil_CreatePixelShader( m_pd3dDevice, "Shaders\\comp.xpu", &m_dwCompPShader ) ) )
		return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( ReloadTexture() ) )
        return XBAPPERR_MEDIANOTFOUND;


    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        m_dwCompression = ( m_dwCompression + 1 ) % NUM_LABELS;
    }
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
    {
        m_dwTexture = ( m_dwTexture + 1 ) % ( sizeof( g_aszFiles ) / sizeof( g_aszFiles[0] ) );
        if( FAILED( ReloadTexture() ) )
            return XBAPPERR_MEDIANOTFOUND;
    }
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
    {
        m_dwFormat = ( m_dwFormat + 1 ) % ( sizeof( g_adwFormats ) / sizeof( g_adwFormats[0] ) );
        ReloadTexture();
    }
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_BLACK ] )
    {
        m_bDifference = (m_bDifference + 1) % 3;
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ); 
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xa0a0a0a0 ) ;

    if(m_bDifference == 2) {
        m_pd3dDevice->SetTexture( 1, m_dwCompression == 0 ? m_pTexOrig : ( m_dwCompression == 1 ? m_pTexD3DX : m_pTexCompressed ) );
        m_pd3dDevice->SetTexture( 0, m_pTexOrig );
    } else {
        m_pd3dDevice->SetTexture( 0, m_dwCompression == 0 ? m_pTexOrig : ( m_dwCompression == 1 ? m_pTexD3DX : m_pTexCompressed ) );
        m_pd3dDevice->SetTexture( 1, m_pTexOrig );
    }

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    m_pd3dDevice->SetVertexShader( FVF_CUSTOMVERTEX );
    m_pd3dDevice->SetStreamSource( 0, m_pvb, sizeof( CUSTOMVERTEX ) );

    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0); 
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 0);
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_TEXCOORDINDEX, 0);  

    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT  );


    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT  ); 
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_CURRENT  ); 

    if(!m_bDifference) {
        m_pd3dDevice->SetPixelShader( 0 ); 
	
    } else {
        m_pd3dDevice->SetPixelShader( m_dwCompPShader ); 

/*
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SUBTRACT );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT  );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );

        m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_ADDSIGNED2X  );
        m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT  );
        m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TFACTOR  );
        m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_ADDSIGNED2X );
        m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_CURRENT  );
        m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_CURRENT  );

*/		
    }
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );  

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        WCHAR ch[100];

        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"TextureCompression" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        swprintf( ch, L"%S", g_aszFiles[ m_dwTexture ] );
        m_Font.DrawText(  64, 80,  0xffffff00, ch );
        swprintf( ch, L"DXT%d", m_dwFormat + 1 );
        m_Font.DrawText(  64, 110, 0xffffff00, ch );
        m_Font.DrawText(  64, 140, 0xffffff00, g_aszLabels[ m_dwCompression ] );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

