#include "stdafx.h"
#include "polyobject.h"
#include "udtexture.h"
#include "indtests.h"
#include "keyboardmenu.h"
#include "choosesavemenu.h"

// Forward declarations
HRESULT Init( CPolyObject* poMainRenderSurface, CUDTexture* poMainTexture );
HRESULT InitD3D( UINT uiWidth, UINT uiHeight, DWORD dwFlags );
BOOL LaunchFromSave( char* pszPath, WCHAR* pwszSaveName );
// HRESULT InitDSound( void );
HRESULT ProcessInput( USBManager* pUSBManager );
HRESULT ProcessGlobalKeySequences( USBManager* pUSBManager );
HRESULT CleanUp( void );
void DisplaySystemInfo( void );   // Sets up and goes to the help menu

// BOOL GammaFade(int startTime);
// unsigned int GetScreenWidth( void );      // Get the Width of the screen
// unsigned int GetScreenHeight( void );     // Get the Height of the screen
// void AlphaTexture( IDirect3DTexture8* pTexture, DWORD dwColor );

// Other globals
IDirect3DDevice8*      g_pD3DDevice     = NULL;         // Pointer to our Direct3D Device Object
// LPDIRECTSOUND          g_pDSound        = NULL;         // Pointer to our DirectSound Object
CXItem*                g_pRootMenu      = NULL;         // Global Pointer to our 'ROOT' menu

void _cdecl main()
{
    USBManager  oUSBManager;                // Handles all input devices for the Xbox
    CPolyObject oMainRenderSurface;         // Poly that will be used as our main render surface
    CUDTexture  oMainTexture;               // Main Texture for our menu data that all screens will render to

    if( FAILED( Init( &oMainRenderSurface, &oMainTexture ) ) )
    {
        DebugPrint( "Failed to Initialize Program!!\n" );

        return; // This is invalid on the Xbox, but if we can't init, we might as well break
    }

    // Menu's
    CIndTests oMenuIndTests;

    // Initialize Root-Menu
    oMenuIndTests.Init( GetBodyFont(), GetHeaderFont() );
    SetCurrentMenu( &oMenuIndTests );
    g_pRootMenu = &oMenuIndTests;

    // Check to see if we should launch a test
    WCHAR pwszSaveName[MAX_PATH+1];
    char pszPath[4];
    
    pszPath[3] = '\0';
    pwszSaveName[MAX_PATH] = L'\0';

    if( LaunchFromSave( pszPath, pwszSaveName ) )
    {
        // Check to see if the Suite is on an MU.  If so, try for a while before giving up
        DWORD dwGetTickCount = GetTickCount();

        while( ( GetTickCount() - dwGetTickCount ) < 2500 ) // 2.5 seconds
        {
            // Attempt to Launch the Game
            if( FAILED( oMenuIndTests.LaunchSuiteFromSave( pszPath, pwszSaveName ) ) )
                DebugPrint( "main():Failed to LaunchSuiteFromSave!! Path - '%hs', Save Name - '%ls'\n", pszPath, pwszSaveName );

            // Don't wait if it's the Hard Drive
            if( 0 == _stricmp( pszPath, "u:\\" ) )
                break;

            // Make sure we are looking for the MU's
            ProcessMUs();

            Sleep( 500 ); // 1/2 second
        }
        WCHAR pszMessage[MAX_PATH+1];
        pszMessage[MAX_PATH] = L'\0';

        _snwprintf( pszMessage, MAX_PATH, L"Unable to launch the specified configuration.\nPath = '%hs'\nConfig Name = '%ls'", pszPath, pwszSaveName );
        GetMessagePtr()->SetBackPtr( GetCurrentMenu() );
        GetMessagePtr()->SetText( pszMessage );

        SetCurrentMenu( GetMessagePtr() );
    }

    // Render Loop
	do
    {
        // Handle any global key sequences
        ProcessGlobalKeySequences( &oUSBManager );

        // Deal with our MUs (any changes)
        ProcessMUs();

        GetCurrentMenu()->Action( &oMainTexture );

		g_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, gc_dwCOLOR_BLACK, 1.0f, 0 );
		g_pD3DDevice->BeginScene();

        g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

        g_pD3DDevice->SetTexture( 0, oMainTexture.GetTexture() );
        oMainRenderSurface.Render( g_pD3DDevice );
		g_pD3DDevice->SetTexture( 0, NULL );

        g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
        g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

        g_pD3DDevice->EndScene();
		g_pD3DDevice->Present( NULL, NULL, NULL, NULL );

        // Handle any input from the user
        ProcessInput( &oUSBManager );
    } while( true );

    CleanUp();

    DebugPrint( "***** main()\n" );
}


/*
// Initialize DSound
HRESULT InitDSound( void )
{
	HRESULT hr = S_OK;

    if( NULL != g_pDSound )
    {
        g_pDSound->Release();
        g_pDSound = NULL;
    }

	if (FAILED(hr = DirectSoundCreate(NULL, &g_pDSound, NULL)))
	{
        DebugPrint( "DirectSoundCreate FAILED!! Error - '0x%.08X'\n", hr );

        return hr;
	}

    return hr;
}
*/

// Initialize Direct3D
HRESULT InitD3D( UINT uiWidth, UINT uiHeight, DWORD dwFlags )
{
    // Check to see if we are already initialized, and if so, release
    // the previous object
    if( NULL != g_pD3DDevice )
    {
        g_pD3DDevice->Release();
        g_pD3DDevice = NULL;
    }

    IDirect3D8* pD3D;
    HRESULT hr = S_OK;

    // Create our Direct3D Object
    if( NULL == ( pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
    {
        DebugPrint( "InitD3D():Failed to create our D3D Object!!\n" );

        return E_FAIL;
    }

    // Always do interlaced output on an HDTV.
    if( XGetAVPack() == XC_AV_PACK_HDTV )
    {
        dwFlags |= D3DPRESENTFLAG_INTERLACED;
    }

	// Create our D3D Device Object
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	
	d3dpp.BackBufferWidth                 = uiWidth;    // SCREEN_WIDTH;
	d3dpp.BackBufferHeight                = uiHeight;   // SCREEN_HEIGHT;
	d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    if( uiWidth <= 1280 )
    {
        d3dpp.MultiSampleType             = D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN; // D3DMULTISAMPLE_NONE; // D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR; // D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN; 
    }
	d3dpp.BackBufferCount                 = 1;
    d3dpp.Flags                           |= dwFlags;
	
    d3dpp.hDeviceWindow                   = NULL;                    // Ignored on Xbox
    d3dpp.Windowed                        = FALSE;                   // Must be FALSE for Xbox
	d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.EnableAutoDepthStencil          = FALSE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; // D3DPRESENT_INTERVAL_DEFAULT;
    
  	if ( FAILED ( hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT,
                                           D3DDEVTYPE_HAL,
                                           NULL,
                                           D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                           &d3dpp,
                                           &g_pD3DDevice ) ) )
	{
		DebugPrint( "InitD3D():Failed to create our D3D Device Object!!\n" );
	}

    D3DDevice_SetSoftDisplayFilter(TRUE);

    // Release the D3D Object
    // It's ok to simply release this object, as we know we cannot be here if the D3D Object failed creation
    pD3D->Release();
    pD3D = NULL;


    // Create our ViewPort
    D3DVIEWPORT8 D3DViewport;

    D3DViewport.X      = 0;
    D3DViewport.Y      = 0;
    D3DViewport.Width  = uiWidth;   // SCREEN_WIDTH;
    D3DViewport.Height = uiHeight;  // SCREEN_HEIGHT; 
    D3DViewport.MinZ   = 0.0f;
    D3DViewport.MaxZ   = 1.0f;

    // Set our Viewport
    if( FAILED( hr = g_pD3DDevice->SetViewport( &D3DViewport ) ) )
    {
        DebugPrint( "InitD3D():Failed to set the D3D Viewport!!\n" );
    }

	g_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    g_pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    g_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    // g_pD3DDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , FALSE ); // Must do this to enable Edge Antialiasing
    g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
    g_pD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
    // g_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, TRUE );
    if( uiWidth <= 1280 )
    {
        g_pD3DDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , TRUE );
    }

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

    return hr;
}

// Initialize our application - video, sound, etc
HRESULT Init( CPolyObject* poMainRenderSurface, CUDTexture* poMainTexture )
{
    ////////////////////////////
    // Set the File System Cache
    ////////////////////////////
    XSetFileCacheSize( gc_uiCACHE_SIZE );

    /////////////////
    // Initialize D3D
    /////////////////
    if( FAILED( InitD3D( gc_uiSCREEN_WIDTH, gc_uiSCREEN_HEIGHT, 0 ) ) )
    {
        DebugPrint( "Init():Failed to Initialize D3D!!\n" );

        _asm { int 3 };
    }

    // Initialize our Main Render Surface Polygon
    poMainRenderSurface->Initialize( g_pD3DDevice, (float)gc_uiSCREEN_WIDTH, (float)gc_uiSCREEN_HEIGHT );

    // Initial our main texture
    poMainTexture->Initialize( g_pD3DDevice, NULL, 1024, 1024 );

    ////////////////////////////////////////
    // Setup and Initialize our Default FONT
    ////////////////////////////////////////
    InitializeFonts();

    ////////////////////
    // Memory Units
    ////////////////////
    InitializeMUs();

    /*
    ////////////////////
    // Initialize DSound
    ////////////////////
    if( FAILED( InitDSound() ) )
    {
        DebugPrint( "Init():Failed to Initialize DSound!!\n" );
    }
    
    ////////////////////////
    // Initialize our Sounds
    ////////////////////////
    if( FAILED( g_Sounds.Init( g_pDSound ) ) )
    {
        DebugPrint( "Init():Failed to Initialize our Sounds!!\n" );
    }
    */

    ////////////////////////
    // Setup Menus and Items
    ////////////////////////
    InitializeMenus();

    ///////////////////////////////////////
    // Initialize our HVS Launcher XBE Info
    ///////////////////////////////////////
    InitLauncherXBEInfo();

    return S_OK;
}

// Clean up our application (close drivers, etc)
HRESULT CleanUp( void )
{
    // Clean up D3D
    if( NULL != g_pD3DDevice )
    {
        g_pD3DDevice->Release();
        g_pD3DDevice = NULL;
    }

	/*
    // Clean up DSound
    if( NULL != g_pDSound )
    {
        g_pDSound->Release();
        g_pDSound = NULL;
    }
    */

    // Clean up the Fonts
    CleanupFonts();
    
    return S_OK;
}

// Process any global key sequences
HRESULT ProcessGlobalKeySequences( USBManager* pUSBManager )
{
    if( pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_LEFT_TRIGGER, gc_nBUTTON_THRESHOLD ) &&
        pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_RIGHT_TRIGGER, gc_nBUTTON_THRESHOLD ) &&
        pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_BLACK, gc_nBUTTON_THRESHOLD ) )
    {
        // Reboot to the XDK Launcher
        XLaunchNewImage( NULL, NULL );
    }
    else if( pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_LEFT_TRIGGER, gc_nBUTTON_THRESHOLD ) &&
             pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_RIGHT_TRIGGER, gc_nBUTTON_THRESHOLD ) &&
             pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_WHITE, gc_nBUTTON_THRESHOLD ) )
    {
        // Display System Information
        DisplaySystemInfo();

    }

    return S_OK;
}

// Handle input from the user
HRESULT ProcessInput( USBManager* pUSBManager )
{
    BOOL bFirstPress;                   // Used to determine if one button has been pressed for the first time
    BOOL bFirstPress2;                  // Used to handle Two Buttons
    BOOL bFirstXPress;                  // Used to determine if the X Axis has been pressed for the first time
    BOOL bFirstYPress;                  // Used to determine if the Y Axis has been pressed for the first time
    BOOL bRemoteButtonPressed = FALSE;  // Used to deal with the multiple controls mappings (A/OK/START, B/BACK)
    BOOL bControlPressed = FALSE;       // Used to deal with the multiple controls mappings (A/OK/START, B/BACK)
    BOOL bButtonPressed = FALSE;        // Used to deal with the multiple controls mappings (A/OK/START, B/BACK)
    int  nThumbY = 0;
    int  nThumbX = 0;

    // Update the state of all controllers
    pUSBManager->ProcessInput();

    // START Button
    if( pUSBManager->IsControlPressed( PORT_ANY, CONTROL_START, &bFirstPress ) )
        GetCurrentMenu()->HandleInput( CONTROL_START, bFirstPress );

    // BACK Button
    if( bControlPressed = pUSBManager->IsControlPressed( PORT_ANY, CONTROL_BACK, &bFirstPress ) )
        GetCurrentMenu()->HandleInput( CONTROL_BACK, bFirstPress );

    // A BUTTON
    bButtonPressed       = pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_A, gc_nBUTTON_THRESHOLD, &bFirstPress );
    bRemoteButtonPressed = pUSBManager->IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_OK, &bFirstPress2 );
    if( bButtonPressed || bRemoteButtonPressed )
    {
        // Check to see if one of the buttons is a 'repeat' press.  If so, report FALSE for first press
        if( bButtonPressed && ( !bFirstPress ) )
            bFirstPress = FALSE;
        else if( bRemoteButtonPressed && ( !bFirstPress2 ) )
            bFirstPress = FALSE;
        else
            bFirstPress = TRUE;

        GetCurrentMenu()->HandleInput( BUTTON_A, bFirstPress );
    }

    // B BUTTON
    bButtonPressed = pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_B, gc_nBUTTON_THRESHOLD, &bFirstPress );
    bRemoteButtonPressed = pUSBManager->IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_REPLAY, &bFirstPress2 );
    if( bButtonPressed || bRemoteButtonPressed )
    {
        // Check to see if one of the buttons is a 'repeat' press.  If so, report FALSE for first press
        if( bButtonPressed && ( !bFirstPress ) )
            bFirstPress = FALSE;
        else if( bRemoteButtonPressed && ( !bFirstPress2 ) )
            bFirstPress = FALSE;
        else
            bFirstPress = TRUE;

        GetCurrentMenu()->HandleInput( BUTTON_B, bFirstPress );
    }

    // X MENU BUTTON
    bButtonPressed = pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_X, gc_nBUTTON_THRESHOLD, &bFirstPress );
    bRemoteButtonPressed = pUSBManager->IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_MENU, &bFirstPress2 );
    if( bButtonPressed || bRemoteButtonPressed )
    {
        if( bButtonPressed && ( !bFirstPress ) )
            bFirstPress = FALSE;
        else if( bRemoteButtonPressed && ( !bFirstPress2 ) )
            bFirstPress = FALSE;
        else
            bFirstPress = TRUE;

        GetCurrentMenu()->HandleInput( BUTTON_X, bFirstPress );
    }

    // Y BUTTON
    if( pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_Y, gc_nBUTTON_THRESHOLD, &bFirstPress ) )
        GetCurrentMenu()->HandleInput( BUTTON_Y, bFirstPress );

    // BLACK BUTTON
    if( pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_BLACK, gc_nBUTTON_THRESHOLD, &bFirstPress ) )
        GetCurrentMenu()->HandleInput( BUTTON_BLACK, bFirstPress );

    // WHITE BUTTON
    if( pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_WHITE, gc_nBUTTON_THRESHOLD, &bFirstPress ) )
        GetCurrentMenu()->HandleInput( BUTTON_WHITE, bFirstPress );


    // UP DPAD
    bControlPressed = pUSBManager->IsControlPressed( PORT_ANY, CONTROL_DPAD_UP, &bFirstPress );
    bRemoteButtonPressed = pUSBManager->IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_UP, &bFirstPress2 );
    if( bControlPressed || bRemoteButtonPressed )
    {
        if( bControlPressed && ( !bFirstPress ) )
            bFirstPress = FALSE;
        else if( bRemoteButtonPressed && ( !bFirstPress2 ) )
            bFirstPress = FALSE;
        else
            bFirstPress = TRUE;

        GetCurrentMenu()->HandleInput( CONTROL_DPAD_UP, bFirstPress );
    }

    // DOWN DPAD / REMOTE DOWN
    bControlPressed = pUSBManager->IsControlPressed( PORT_ANY, CONTROL_DPAD_DOWN, &bFirstPress );
    bRemoteButtonPressed = pUSBManager->IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_DOWN, &bFirstPress2 );
    if( bControlPressed || bRemoteButtonPressed )
    {
        if( bControlPressed && ( !bFirstPress ) )
            bFirstPress = FALSE;
        else if( bRemoteButtonPressed && ( !bFirstPress2 ) )
            bFirstPress = FALSE;
        else
            bFirstPress = TRUE;

        GetCurrentMenu()->HandleInput( CONTROL_DPAD_DOWN, bFirstPress );
    }

    // LEFT DPAD
    bControlPressed = pUSBManager->IsControlPressed( PORT_ANY, CONTROL_DPAD_LEFT, &bFirstPress );
    bRemoteButtonPressed = pUSBManager->IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_LEFT, &bFirstPress2 );
    if( bControlPressed || bRemoteButtonPressed )
    {
        if( bControlPressed && ( !bFirstPress ) )
            bFirstPress = FALSE;
        else if( bRemoteButtonPressed && ( !bFirstPress2 ) )
            bFirstPress = FALSE;
        else
            bFirstPress = TRUE;

        GetCurrentMenu()->HandleInput( CONTROL_DPAD_LEFT, bFirstPress );
    }

    // RIGHT DPAD
    bControlPressed = pUSBManager->IsControlPressed( PORT_ANY, CONTROL_DPAD_RIGHT, &bFirstPress );
    bRemoteButtonPressed = pUSBManager->IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_RIGHT, &bFirstPress2 );
    if( bControlPressed || bRemoteButtonPressed )
    {
        if( bControlPressed && ( !bFirstPress ) )
            bFirstPress = FALSE;
        else if( bRemoteButtonPressed && ( !bFirstPress2 ) )
            bFirstPress = FALSE;
        else
            bFirstPress = TRUE;

        GetCurrentMenu()->HandleInput( CONTROL_DPAD_RIGHT, bFirstPress );
    }

    // LEFT THUMB BUTTON
    bControlPressed = pUSBManager->IsControlPressed( PORT_ANY, CONTROL_LEFT_THUMB, &bFirstPress );
    if( bControlPressed )
        GetCurrentMenu()->HandleInput( CONTROL_LEFT_THUMB, bFirstPress );

    // RIGHT THUMB BUTTON
    bControlPressed = pUSBManager->IsControlPressed( PORT_ANY, CONTROL_RIGHT_THUMB, &bFirstPress );
    if( bControlPressed )
        GetCurrentMenu()->HandleInput( CONTROL_RIGHT_THUMB, bFirstPress );

    // LEFT TRIGGER BUTTON
    if( pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_LEFT_TRIGGER, gc_nBUTTON_THRESHOLD, &bFirstPress ) )
        GetCurrentMenu()->HandleInput( BUTTON_LEFT_TRIGGER, bFirstPress );

    // Right Trigger Button
    if( pUSBManager->IsButtonPressed( PORT_ANY, BUTTON_RIGHT_TRIGGER, gc_nBUTTON_THRESHOLD, &bFirstPress ) )
        GetCurrentMenu()->HandleInput( BUTTON_RIGHT_TRIGGER, bFirstPress );


    // LEFT THUMB STICK
    nThumbY = pUSBManager->GetJoystickY( PORT_ANY, JOYSTICK_LEFT, gc_nJOYSTICK_DEADZONE, &bFirstYPress );
    nThumbX = pUSBManager->GetJoystickX( PORT_ANY, JOYSTICK_LEFT, gc_nJOYSTICK_DEADZONE, &bFirstXPress );
    if( ( nThumbY != 0 ) || ( nThumbX != 0 ) )
        GetCurrentMenu()->HandleInput( JOYSTICK_LEFT, nThumbY, nThumbX, bFirstYPress, bFirstXPress );
    
    // RIGHT THUMB STICK
    nThumbY = pUSBManager->GetJoystickY( PORT_ANY, JOYSTICK_RIGHT, gc_nJOYSTICK_DEADZONE, &bFirstYPress );
    nThumbX = pUSBManager->GetJoystickX( PORT_ANY, JOYSTICK_RIGHT, gc_nJOYSTICK_DEADZONE, &bFirstXPress );
    if( ( nThumbY != 0 ) || ( nThumbX != 0 ) )
        GetCurrentMenu()->HandleInput( JOYSTICK_RIGHT, nThumbY, nThumbX, bFirstYPress, bFirstXPress );

    return S_OK;
}


/*
void AlphaTexture( IDirect3DTexture8* pTexture, DWORD dwColor )
{
    D3DXCOLOR newColor = D3DXCOLOR(dwColor);

	HRESULT          hr;
	D3DSURFACE_DESC  desc;
	
	pTexture->GetLevelDesc(0, &desc);
	D3DFORMAT format = desc.Format;

    if(format == D3DFMT_LIN_A8R8G8B8)
	{	
		pTexture->GetLevelDesc(0, &desc);
		int width        = desc.Width;
		int height       = desc.Height;

		D3DLOCKED_RECT textureInfo;
		hr = pTexture->LockRect(0, &textureInfo, NULL, 0);
		if( FAILED( hr ))
		{
            DebugPrint( "AlphaTexture():Failed to lock the texture!!\n" );
		}
		
		DWORD *p = (DWORD*)textureInfo.pBits;
		DWORD counter = 0;

		D3DXCOLOR pixelColor;

		// for(int j = 0; j < (width * height); j++)
        for(int j = 0; j < (width * 22); j++)
		{
			pixelColor = D3DCOLOR(p[j]);
			pixelColor.a = pixelColor.r;  // assuming pixels are white, set alpha
			pixelColor.r = newColor.r;
			pixelColor.g = newColor.g;
			pixelColor.b = newColor.b;
			p[j] = pixelColor;
		}

		pTexture->UnlockRect(0);
		D3DXFilterTexture(pTexture, NULL, 0, D3DX_FILTER_LINEAR);
	}
	else
	{
        DebugPrint( "AlphaTexture():Invalid Texture Format!!\n" );
	}

}

// Get the current Width of the screen
unsigned int GetScreenWidth( void )
{
    D3DDISPLAYMODE  d3dDisplayMode;
    ZeroMemory( &d3dDisplayMode, sizeof( d3dDisplayMode ) );

    HRESULT hr = g_pD3DDevice->GetDisplayMode( &d3dDisplayMode );
    if( FAILED( hr ) )
    {
        DebugPrint( "GetScreenWidth():Failed to get the Display Mode!! Error - '0x%.8X (%d)'\n", hr, hr );

        return 0;
    }

    return d3dDisplayMode.Width;
}


// Get the current Height of the screen
unsigned int GetScreenHeight( void )
{
    D3DDISPLAYMODE  d3dDisplayMode;
    ZeroMemory( &d3dDisplayMode, sizeof( d3dDisplayMode ) );

    HRESULT hr = g_pD3DDevice->GetDisplayMode( &d3dDisplayMode );
    if( FAILED( hr ) )
    {
        DebugPrint( "GetScreenHeight():Failed to get the Display Mode!! Error - '0x%.8X (%d)'\n", hr, hr );

        return 0;
    }

    return d3dDisplayMode.Height;
}


#define FADE_LENGTH 5000

void SetRamp(int coefficient)
{
    int i, value;
    D3DGAMMARAMP ramp;

    for (i = 0; i < 256; i++) {
        value = i * coefficient / FADE_LENGTH;
        ramp.red[i] = (BYTE)value;
        ramp.green[i] = (BYTE)value;
        ramp.blue[i] = (BYTE)value;
    }

    IDirect3DDevice8_SetGammaRamp(g_pD3DDevice,
            D3DSGR_NO_CALIBRATION | D3DSGR_IMMEDIATE,
            &ramp);
}

BOOL GammaFade(int startTime)
{
    int delta;

    delta = GetTickCount() - startTime;

    if (delta >= FADE_LENGTH) {
        SetRamp(FADE_LENGTH);
        return FALSE;
    } else {
        SetRamp(delta);
        return TRUE;
    }
}
*/

// Launch a Test Suite from a Saved Game
BOOL LaunchFromSave( char* pszPath, WCHAR* pwszSaveName )
{
    if( NULL == pszPath || NULL == pwszSaveName )
    {
        DebugPrint( "LaunchFromSave():Invalid argument(s) passed in!!\n" );

        return FALSE;
    }

    BOOL bReturn = FALSE;

    // Check to see if we should launch a specific config
    if( FileExists( gc_pszAutoRunFile ) )
    {
        FILE* fAutoFile = fopen( gc_pszAutoRunFile, "rt" );
        if( NULL == fAutoFile )
            DebugPrint( "LaunchFromSave():Failed to open the AutoRun file!! - '%hs'\n", gc_pszAutoRunFile );
        else
        {
            if( fscanf( fAutoFile, "%3[^,], \"%l[^\"]\"", pszPath, pwszSaveName ) != 2 )
                DebugPrint( "LaunchFromSave():Failed to process the AutoRun file properly!! - '%hs'\n", gc_pszAutoRunFile );
            else
                bReturn = TRUE;
            
            fclose( fAutoFile );
            fAutoFile = NULL;

            DebugPrint( "LaunchFromSave():pszPath = '%hs', pszSaveName - '%ls'\n", pszPath, pwszSaveName );
        }

        // Delete the file to make sure we don't run the same config again
        if( !DeleteFile( gc_pszAutoRunFile ) )
        {
            DebugPrint( "LaunchFromSave():Failed to delete the AutoRun file!! - '%hs'\n", gc_pszAutoRunFile );

            bReturn = FALSE;
        }
    } 
    
    return bReturn;
}

// Sets up and goes to the help menu
void DisplaySystemInfo( void )
{
    // Set the menu title
    GetHelpMenuPtr()->SetMenuTitle( GetHeaderFont(), L"Xbox System Info" );

    // Set the back pointer for the help menu
    GetHelpMenuPtr()->SetBackPtr( g_pRootMenu );

    // Clear the current help
    GetHelpMenuPtr()->ClearHelpItems();

    // Populate the help menu with our items
    char* pszBoxInfo = NULL;
    pszBoxInfo = GetBoxInformation();

    char* pszCurrentLoc = pszBoxInfo;
    char* pszNewLinePtr = NULL;

    if( NULL != pszBoxInfo )
    {
        /*
        WCHAR pwszTempStr[MAX_PATH+1];
        pwszTempStr[MAX_PATH] = L'\0';
        */

        WCHAR pwszItemName[MAX_PATH+1];
        WCHAR pwszItemValue[MAX_PATH+1];
        pwszItemName[MAX_PATH] = L'\0';
        pwszItemValue[MAX_PATH] = L'\0';

        while( pszNewLinePtr = strstr( pszCurrentLoc, "\n" ) )
        {
            *pszNewLinePtr = '\0';
            // _snwprintf( pwszTempStr, MAX_PATH, L"%hs", pszCurrentLoc );
            
            if( sscanf( pszCurrentLoc, "%l255[^=] = %l255[^\0]", pwszItemName, pwszItemValue ) == 2 )
            {
                // Wrap on each comma/space
                WCHAR* pszCommaPtr = pwszItemValue;
                WCHAR* pwszSplit = NULL;
                WCHAR pwszWrapString[3];

                // Determine if we are dealing with Peripherals.  If so, wrap on a semi-colon
                if( 0 == _wcsicmp( pwszItemName, L"Peripherals " ) )
                    wcscpy( pwszWrapString, L"; " );
                else
                    wcscpy( pwszWrapString, L", " );

                while( pwszSplit = wcsstr( pszCommaPtr, pwszWrapString ) )
                {
                    *pwszSplit = L'\\';
                    *(pwszSplit+1) = L'n';

                    // Skip over our newly created "\n"
                    pszCommaPtr += 2;
                }

                GetHelpMenuPtr()->AddHelpItem( GetBodyFont(),
                                               pwszItemName,
                                               GetBodyFont(),
                                               pwszItemValue );
            }

            // Skip over the '\0' char we created
            pszCurrentLoc = pszNewLinePtr + 1;
        }

        delete[] pszBoxInfo;
        pszBoxInfo = NULL;
    }

    // navigate to the menu
    GetCurrentMenu()->Leave( GetHelpMenuPtr() );
}