#include "stdafx.h"
#include "polyobject.h"

// Forward declarations
void Render();              // Render the video on to our screen and present it to the user
void CleanUp();             // Clean up memory, etc, when neccessary
BOOL Initialize();          // Initialize anything necessary for the MU Config program
BOOL InitializeD3D();       // Initalize D3D to be used by our application

// Globals
IDirect3DDevice8* g_pD3DDevice          = NULL;     // Pointer to our Direct3D Device Object
CPolyObject       g_mainDisplayPoly;                // Our main display (poly) object
CUDTexture        g_mainDisplayTexture;             // Out main display (texture) object
XFONT*            g_pFont               = NULL;     // Our Default Font Pointer
CHardDrive        g_HardDrive;                      // Xbox Hard Drive

void _cdecl main()
{
    XDBGWRN( APP_TITLE_NAME_A, "main() Entered..." );

    // Initialize the Application
    if( !Initialize() )
    {
        XDBGWRN( APP_TITLE_NAME_A, "main():Failed to initialize!!" );

        return; // Should never return from main.  This is a bad situation
    }

    // Render Loop
    while( TRUE )
    {
        // Render the video on our screen and present it to the user
        Render();
    }

    XDBGWRN( APP_TITLE_NAME_A, "main() Exiting." );
}

void Render()
{
    g_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0 );
    g_pD3DDevice->BeginScene();

    // Render the main screen object here
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

    g_pD3DDevice->SetTexture( 0, g_mainDisplayTexture.GetTexture() );
    g_mainDisplayPoly.Render( g_pD3DDevice );
    g_pD3DDevice->SetTexture( 0, NULL );

    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

    g_pD3DDevice->EndScene();
    g_pD3DDevice->Present( NULL, NULL, NULL, NULL );
}


// Clean up memory, etc, when neccessary
void CleanUp()
{
    // Clean up our D3D Device object
    if( g_pD3DDevice )
    {
        g_pD3DDevice->Release();
        g_pD3DDevice = NULL;
    }

    if( g_pFont )
    {
        XFONT_Release( g_pFont );
        g_pFont = NULL;
    }
}


// Initialize anything necessary for the MU Config program
BOOL Initialize()
{
    // Set the file cache size
    XSetFileCacheSize( FILE_DATA_CACHE_SIZE );

    // Get any launch info and display it in to the debugger
    DWORD dwLDataType;
    LAUNCH_DATA lData;
    ZeroMemory( &lData, sizeof( lData ) );

    if( ERROR_SUCCESS == XGetLaunchInfo( &dwLDataType, &lData ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "Initialize():Received Launch Data:" );

        if( LDT_FROM_DASHBOARD == dwLDataType )
        {
            LD_FROM_DASHBOARD* pDashData = (LD_FROM_DASHBOARD*)&lData;
            XDBGWRN( APP_TITLE_NAME_A, "  Data from DASHBOARD" );
            XDBGWRN( APP_TITLE_NAME_A, "  Context - '%d'", pDashData->dwContext );

        }
        else if( LDT_FROM_DEBUGGER_CMDLINE == dwLDataType )
        {
            XDBGWRN( APP_TITLE_NAME_A, "  Data from DEBUGGER CMDLINE" );
        }
        else if( LDT_TITLE == dwLDataType )
        {
            XDBGWRN( APP_TITLE_NAME_A, "  Data from TITLE" );
        }
        else
        {
            XDBGWRN( APP_TITLE_NAME_A, "  Data from UNKNOWN" );
        }
    }

    // Check to see if our media exists on the HD -- If not, Copy it
    if( !g_HardDrive.FileExists( FILE_DATA_DEFAULT_FONT_FILENAME_A ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "Initialize(): Must Copy Media!!" );

        char pszSource[MAX_PATH+1];
        
        ZeroMemory( pszSource, MAX_PATH+1 );

        _snprintf( pszSource, MAX_PATH, "d:\\%0.8X", XeImageHeader()->Certificate->TitleID );

        // Copy the media -- Assume in it's our Title Dir
        g_HardDrive.CopyDirs( pszSource, "t:" );

        // Copy the DEFAULT.XBE to the user's E drive
        g_HardDrive.UnMapDrive( 'E' );
        g_HardDrive.MapDrive( 'E', "\\Device\\Harddisk0\\Partition1\\devkit" );
        CopyFile( "d:\\default.xbe", "e:\\GRate1AO.xbe", FALSE );
        g_HardDrive.UnMapDrive( 'E' );
    }

    // Initialize Direct3D
    if( !InitializeD3D() )
    {
        XDBGWRN( APP_TITLE_NAME_A, "Initialize():Failed to InitializeD3D!!" );

        return FALSE;
    }

    ////////////////////////////////////////
    // Setup and Initialize our Default FONT
    ////////////////////////////////////////
    XFONT_OpenTrueTypeFont( FILE_DATA_DEFAULT_FONT_FILENAME, 4096, &g_pFont );

    if( g_pFont )
    {
        // XFONT_SetDrawingSurface( m_BackBuffer );
        XFONT_SetTextColor( g_pFont, SCREEN_DEFAULT_FOREGROUND_COLOR );
        XFONT_SetBkColor( g_pFont, SCREEN_DEFAULT_BACKGROUND_COLOR );
        XFONT_SetTextHeight( g_pFont, SCREEN_DEFAULT_FONT_HEIGHT );
        XFONT_SetTextAntialiasLevel( g_pFont, SCREEN_DEFAULT_FONT_ALIAS_LEVEL );
        XFONT_SetTextStyle( g_pFont, SCREEN_DEFAULT_FONT_STYLE );
    }
    else
    {
        XDBGWRN( APP_TITLE_NAME_A, "Init():Failed to Initialize our Font!!" );
    }

    // Create our display poly and our display texture
    g_mainDisplayPoly.Initialize( g_pD3DDevice );
    g_mainDisplayTexture.Initialize( g_pD3DDevice, NULL, 1024, 1024 );

    // TODO: Render the bitmap to the poly
    BitmapFile Bitmap;
    Bitmap.read( 0, 0, FILE_DATA_DEFAULT_BG_IMAGE_FILENAME );

    g_mainDisplayTexture.Lock();

    Bitmap.render( g_pD3DDevice, g_mainDisplayTexture.GetTextureSurface() );

    void DrawText( float fX1,                               // Prints formatted text, using the default font size
                   float fY1,
                   DWORD dwFGColor,
                   DWORD dwBGColor,
                   const WCHAR* pwszFormat,
                   ... );

    
    void DrawBox( float fX1,                                // Draw a box on the screen
                  float fY1,
                  float fX2,
                  float fY2,
                  DWORD dwColor );

    int iStringLen = GetStringPixelWidth( TEXT_GAME_RATING_INFO );

    unsigned int uiFontHeight;
    unsigned int uiFontWidth;
    GetFontSize( &uiFontHeight, &uiFontWidth );

    XDBGWRN( APP_TITLE_NAME_A, "Init():uiFontHeight - '%u'", uiFontHeight );
    XDBGWRN( APP_TITLE_NAME_A, "Init():uiFontWidth  - '%u'", uiFontWidth );

    float fTopBotYAdjust   = 100.0f;
    float fTopBotBorder    = 5.0f;
    float fLeftRightBorder = 10.0f;

    float X1 = SCREEN_X_CENTER - ( iStringLen / 2.0f ) - fLeftRightBorder;
    float Y1 = SCREEN_HEIGHT - fTopBotYAdjust;

    float X2 = SCREEN_X_CENTER + ( iStringLen / 2.0f ) + fLeftRightBorder;
    float Y2 = Y1 + uiFontHeight + ( fTopBotBorder * 2 );

    g_mainDisplayTexture.DrawBox( X1, Y1, X2, Y2, COLOR_BLACK );
    g_mainDisplayTexture.DrawText( X1 + fLeftRightBorder, Y1 + fTopBotBorder, SCREEN_DEFAULT_FOREGROUND_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, TEXT_GAME_RATING_INFO );

    g_mainDisplayTexture.Unlock();

    return TRUE;
}


// Initalize D3D to be used by our application
BOOL InitializeD3D()
{
    IDirect3D8* pD3D;
    HRESULT hr = S_OK;

    // Create our Direct3D Object
    pD3D = Direct3DCreate8( D3D_SDK_VERSION );
    if( NULL == pD3D )
    {
        XDBGWRN( APP_TITLE_NAME_A, "InitializeD3D():Failed to create our D3D Object!!" );

        return FALSE;
    }

    // Create our D3D Device Object
    D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );

    d3dpp.BackBufferWidth                 = SCREEN_WIDTH;
    d3dpp.BackBufferHeight                = SCREEN_HEIGHT;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.MultiSampleType                 = D3DMULTISAMPLE_4_SAMPLES;
    d3dpp.BackBufferCount                 = 1;

    d3dpp.hDeviceWindow                   = NULL;                    // Ignored on Xbox
    d3dpp.Windowed                        = FALSE;                   // Must be FALSE for Xbox
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.EnableAutoDepthStencil          = FALSE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
	d3dpp.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE; // D3DPRESENT_INTERVAL_IMMEDIATE;

    if ( FAILED ( hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT,
                                           D3DDEVTYPE_HAL,
                                           NULL,
                                           D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                           &d3dpp,
                                           &g_pD3DDevice ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "InitializeD3D():Failed to create our D3D Device Object!!" );
    }

    // Release the D3D Object
    // It's ok to simply release this object, as we know we cannot be here if the D3D Object failed creation
    pD3D->Release();
    pD3D = NULL;

    // If the device failed to create, simply abort the rest of the D3D Initialization
    if( !g_pD3DDevice )
    {
        return FALSE;
    }

    // Create our Viewport
    D3DVIEWPORT8 D3DViewport;

    D3DViewport.X      = 0;
    D3DViewport.Y      = 0;
    D3DViewport.Width  = SCREEN_WIDTH;
    D3DViewport.Height = SCREEN_HEIGHT;
    D3DViewport.MinZ   = 0.0f;
    D3DViewport.MaxZ   = 1.0f;

    // Set our Viewport
    if( FAILED( hr = g_pD3DDevice->SetViewport( &D3DViewport ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "InitializeD3D():Failed to set the D3D Viewport!!" );
    }

    // Set our Renderstates and our Texture Stage States
    //BUGBUG: Tweak these and remove the ones uneccessary
    g_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    g_pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    g_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    g_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
    g_pD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
    g_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, TRUE );

    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

    return TRUE;
}
