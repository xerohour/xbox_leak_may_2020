#include "stdafx.h"
#include "xlaunch.h"
#include "launchmenu.h"
#include "errormsgmenu.h"
#include "camera.h"
#include "gouraudgroup.h"
#include "gouraudobject.h"
#include "gouraudtexobject.h"
#include "xboxverp.h"

// Other globals
IDirect3DDevice8*      g_pD3DDevice = NULL;                 // Pointer to our Direct3D Device Object
LPDIRECTSOUND          g_pDSound = NULL;                    // Pointer to our DirectSound Object
USBManager             Controllers;                         // Used to determine what buttons / direction the user is pressing
CXSettings             g_XboxSettings;                      // Settings that are stored on the Xbox
CSoundCollection       g_Sounds;                            // Used to generate all sounds for the XShell
CHardDrive             g_XboxHardDrive;                     // Used to configure the Xbox Hard Drive
CMemoryUnit            g_MemoryUnits[8];                    // Memory Units that can be inserted, or removed
extern XFONT*          g_pFont;                             // Our Default Font Pointer
CUDTexture             g_MainTexture;                       // Main Texture for our menu data that all screens will render to
CUDTexture             g_XDKTitleTexture;                   // Texture for our XDK Title bar
CUDTexture             g_XDKAddressTexture;                 // Texture for our XDK Address information
CUDTexture             g_BuildNoTexture;                    // Texture that holds our Build Number
CXItem*                g_pPrevScreen = NULL;                // Used to determine when a user enters a new menu
CXItem*                g_pCurrentScreen = NULL;	            // Pointer to the current menu object
CXItem*                g_pRootMenu = NULL;                  // Pointer to the menu that is our "main, or root" menu
BOOL                   g_bNetOK = FALSE;                    // Determines if we should display the IP address
DWORD                  g_dwShellState = SHELLSTATE_NORMAL;  // Defines the current state of the XShell
BOOL                   g_bUpdateNameAddrTexture = TRUE;     // Determines if we need to update the machine name & IP Address
BOOL                   g_bHighlightAButton = FALSE;         // Used to render the A Button Highlight
BOOL                   g_bHighlightBButton = FALSE;         // Used to render the B Button Highlight
BOOL                   g_bDisplayBButton = TRUE;            // Used to determine if we should render the B Button
BOOL                   g_bDisplayAButton = TRUE;            // Used to determine if we should render the A Button
BOOL                   g_bDisplayUpArrow = FALSE;           // Used to determine if we should render the Up Arrow
BOOL                   g_bDisplayDownArrow = FALSE;         // Used to determine if we should render the Down Arrow
BOOL                   g_bUpdateTitleTexture = TRUE;        // Update the Title Texture
BOOL                   g_bToggleIPInfo = TRUE;              // Menu's can use this to turn off toggling of the IP Address info
int                    g_iDisplayAddress = 0;               // debug ip, debug enet, title ip, title enet

BOOL                   g_bMUsChanged = TRUE;                // Tracks whether MU's have been inserted or removed
DWORD                  g_dwInsertedMUs;                     // Tracks which MUs are inserted or removed

char                   g_pszKeyCombo[5];                    // Used to determine the last 4 keys the user has entered
unsigned int           g_uiKeyPos = 0;                      // Used to track which keypress the user is on


// Global 3D Objects
Camera                 g_mainCamera;
GouraudGroup           g_GGroup0;
GouraudGroup           g_GGroup1;
GouraudGroup           g_GTubes;    // Tubes for all objects
GouraudGroup           g_GGroup3;   // App Title, Up/Down Arrows, X/Y button info panel
GouraudGroup           g_GGroup4;
GouraudGroup           g_GGroup5;
GouraudGroup           g_GGroup6;
GouraudGroup           g_GGroup7;   // Outer Cell

GouraudTexObject       g_GTOMovieScreen;
GouraudTexObject       g_XDKtitle;
GouraudTexObject       g_XDKaddress;
GouraudTexObject       g_InnerCell;
GouraudTexObject       g_BuildNo;

// Global Menus are created here
CLaunchMenu            g_LaunchMenu( NULL );                // XBE Launch Menu
CErrorMsgMenu          g_ErrorMsgMenu( NULL );              // Error Msg Menu
CGenKeyboardMenu       g_GenKeyboardMenu( NULL );           // Generic Virutal Keyboard menu
                                                            // TODO: Replace other, non-generic keyboard menus with this
CMessageMenu           g_MessageMenu( NULL );               // Used to display generic messages
                                                            // TODO: Replace other, non-generic message menus with this
CMADisplayMenu         g_MADisplayMenu( NULL );             // Used to display our Memory Areas
CYesNoMenu             g_YesNoMenu( NULL );                 // Used to ask the user a YES / NO question
                                                            // TODO: Replace all current yes / no type menus with this


void _cdecl main()
{
    XDBGTRC( APP_TITLE_NAME_A, "***** main()" );

    DWORD dwEdgeCurrentState = 0;

    Init();

    do
    {
        // Check to see if the user has inserted or removed any Memory Units
        CheckMUs();

        // If the user switched menus, make sure we call "Enter" for the new menu
        if( g_pPrevScreen != g_pCurrentScreen )
        {
            g_pPrevScreen = g_pCurrentScreen;
            g_pCurrentScreen->Enter();
        }
        
		g_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0 );
		g_pD3DDevice->BeginScene();

        // g_mainCamera.noise( g_pD3DDevice );

        g_pD3DDevice->SetVertexShader( D3DFVF_D3DVERTEX );

        g_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, FALSE );

		g_GGroup7.renderStrip( g_pD3DDevice );      // Outer Cell

        g_pD3DDevice->SetTexture( 0, g_InnerCell.m_pTexture );
		g_InnerCell.render( g_pD3DDevice );  // Inner cell
		g_pD3DDevice->SetTexture( 0, NULL );

        /*
        ////////////////////////////////
        // Rotate Cell
        ////////////////////////////////

        float fNow = (float)GetTickCount();
        float fField = D3DX_PI / 4.0f;
        float fRpm = 0.05f;
        float fAngle = sinf((float)(fNow * D3DX_PI * fRpm / 60.0f)) * fField / 2.0f;

        g_GGroup7.m_pObjects->localRotateY( fAngle );
        g_InnerCell.localRotateY( fAngle );
        */

        ////////////////////////////////
        ////////////////////////////////

        g_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, TRUE );

        g_GGroup6.renderStrip( g_pD3DDevice );  //Arm

        g_pD3DDevice->SetVertexShader( D3DFVF_D3DVERTEX );
       
        // g_GGroup0.renderStrip( g_pD3DDevice );      // A & B Button Spheres (4 objects)

        // Should we display the A Button?
        if( g_bDisplayAButton )
        {
            g_GGroup0.m_pObjects[2].renderStrip( g_pD3DDevice );        // A Button Inner Sphere
            g_GGroup0.m_pObjects[3].renderStrip( g_pD3DDevice );        // A Button Outer Sphere
            g_GTubes.m_pObjects[1].renderStrip( g_pD3DDevice );         // A button Tube
            g_GGroup3.m_pObjects[1].renderStrip( g_pD3DDevice );        // A Button Text

            // Check to see if we should highlight the A
            if( g_bHighlightAButton )
            {
                g_GGroup5.m_pObjects[0].renderStrip( g_pD3DDevice );    // A Button Highlight
            }

            g_GGroup4.m_pObjects[1].renderStrip( g_pD3DDevice );        // A Button Full Text

            g_GGroup1.m_pObjects[2].renderStrip( g_pD3DDevice );        // A Button tube joint
            g_GGroup1.m_pObjects[3].renderStrip( g_pD3DDevice );        // A Button tube joint
        }

        // Should we display the B Button
        if( g_bDisplayBButton )
        {
            g_GGroup0.m_pObjects[0].renderStrip( g_pD3DDevice );        // B Button Sphere
            g_GGroup0.m_pObjects[1].renderStrip( g_pD3DDevice );        // B Button Outer Sphere
            g_GTubes.m_pObjects[0].renderStrip( g_pD3DDevice );         // B button Tube
            g_GGroup3.m_pObjects[0].renderStrip( g_pD3DDevice );        // B Button Text

            // Check to see if we should highlight the B 
            if( g_bHighlightBButton )
            {
                g_GGroup5.m_pObjects[1].renderStrip( g_pD3DDevice );    // B Button Highlight
            }

            g_GGroup4.m_pObjects[0].renderStrip( g_pD3DDevice );        // B Button Full Text

            g_GGroup1.m_pObjects[0].renderStrip( g_pD3DDevice );        // B Button tube joint
            g_GGroup1.m_pObjects[1].renderStrip( g_pD3DDevice );        // B Button tube joint
        }

		// g_GGroup1.renderStrip( g_pD3DDevice );  // Frame around texture -- Main body of object (10 objects)
        // Object 0 - B Button tube joint
        // Object 1 - B Button tube joint
        // Object 2 - A Button tube joint
        // Object 3 - A Button tube joint
        g_GGroup1.m_pObjects[4].renderStrip( g_pD3DDevice );    // Frame - Left Panel
        g_GGroup1.m_pObjects[5].renderStrip( g_pD3DDevice );    // Frame - Top Panel (Title Panel)
        g_GGroup1.m_pObjects[6].renderStrip( g_pD3DDevice );    // Frame - Bottom Panel (Name/Address Panel)
        g_GGroup1.m_pObjects[7].renderStrip( g_pD3DDevice );    // Frame - Lower Left Corner Panel
        g_GGroup1.m_pObjects[8].renderStrip( g_pD3DDevice );    // Frame - Lower Right Corner Panel
        g_GGroup1.m_pObjects[9].renderStrip( g_pD3DDevice );    // Frame - Upper Right Corner Panel (X/Y Button Panel)

        g_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, FALSE );

        // g_GTubes.renderStrip( g_pD3DDevice );       // Tubes (4 objects)
        // Object 0 is B Button related (B Button Tube)
        g_GTubes.m_pObjects[2].renderStrip( g_pD3DDevice );     // Frame tube - Bottom (lower left corner)
        g_GTubes.m_pObjects[3].renderStrip( g_pD3DDevice );     // Frame tube - Arm (upper left corner)

        g_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, TRUE );

        // g_GGroup3.renderStrip( g_pD3DDevice );   // Text and Title info (7 objects)
        // Object 0 is B Button related (B Button Text)
        g_GGroup3.m_pObjects[4].renderStrip( g_pD3DDevice );    // Application Title

        // Only render these items if we are on the Launcher Menu
        if( g_pCurrentScreen == &g_LaunchMenu )
        {
            g_GGroup3.m_pObjects[2].renderStrip( g_pD3DDevice );    // Y Button Info (Screen text)
            g_GGroup3.m_pObjects[3].renderStrip( g_pD3DDevice );    // X Button Info (Screen text)
        }

        // Check to see if we should display the "Up" arrow
        if( g_bDisplayUpArrow )
        {
            g_GGroup3.m_pObjects[5].renderStrip( g_pD3DDevice );    // Up Arrow
        }

        // Check to see if we should display the "Down" arrow
        if( g_bDisplayDownArrow )
        {
            g_GGroup3.m_pObjects[6].renderStrip( g_pD3DDevice );    // Down Arrow
        }

		// g_GGroup4.renderStrip( g_pD3DDevice );   // A & B full text (2 Objects)

        // Render our main movie-texture and process any input from the current menu / screen
        g_pCurrentScreen->Action( &g_MainTexture );
        ProcessInput();

        g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

        
        // Make sure that edge antialiasing is off for the following objects
        g_pD3DDevice->GetRenderState( D3DRS_EDGEANTIALIAS, &dwEdgeCurrentState );
        g_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, FALSE );

        //
        // Render the Main Movie Screen
        //
        g_pD3DDevice->SetTexture( 0, g_MainTexture.GetTexture() );
        g_GTOMovieScreen.render( g_pD3DDevice );
		g_pD3DDevice->SetTexture( 0, NULL );

        // Render the XDK Title
        if( g_bUpdateTitleTexture )
        {
            g_bUpdateTitleTexture = FALSE;  // Make sure we only update the texture once

            UpdateTitleTexture( &g_XDKTitleTexture );
        }
		g_pD3DDevice->SetTexture( 0, g_XDKTitleTexture.GetTexture() );
		g_XDKtitle.render( g_pD3DDevice );
		g_pD3DDevice->SetTexture( 0, NULL );

        // Render the XDK Address Information
        // This is updated when the user changes the IP Address or the Machine Name ONLY
        if( g_bUpdateNameAddrTexture )
        {
            g_bUpdateNameAddrTexture = FALSE;  // Make sure we only update the texture once

            UpdateAddressTexture( &g_XDKAddressTexture );
        }

        g_pD3DDevice->SetTexture( 0, g_XDKAddressTexture.GetTexture() );
		g_XDKaddress.render( g_pD3DDevice );
		g_pD3DDevice->SetTexture( 0, NULL );

        g_pD3DDevice->SetTexture( 0, g_BuildNoTexture.GetTexture() );
        g_BuildNo.render( g_pD3DDevice );
        g_pD3DDevice->SetTexture( 0, NULL );
		
        g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
        g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

        // Reset the EdgeAntialias information
        g_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, dwEdgeCurrentState );

        g_pD3DDevice->EndScene();
		g_pD3DDevice->Present( NULL, NULL, NULL, NULL );

        ProcessKeySequences();

    } while( TRUE );

    CleanUp();

    XDBGTRC( APP_TITLE_NAME_A, "***** main()" );
}


// Initialize DSound
HRESULT InitDSound()
{
	HRESULT hr = S_OK;

    if( NULL != g_pDSound )
    {
        g_pDSound->Release();
        g_pDSound = NULL;
    }

	if (FAILED(hr = DirectSoundCreate(NULL, &g_pDSound, NULL)))
	{
        XDBGWRN( APP_TITLE_NAME_A, "DirectSoundCreate FAILED!! Error - '0x%.08X'", hr );

        return hr;
	}

    return hr;
}


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
        XDBGWRN( APP_TITLE_NAME_A, "InitD3D():Failed to create our D3D Object!!" );

        return E_FAIL;
    }

    // Always do interlaced output on an HDTV.
    if( XGetAVPack() == XC_AV_PACK_HDTV )
    {
        dwFlags |= D3DPRESENTFLAG_INTERLACED;
    }

    // Check to see if the user wants widescreen mode
    if( g_XboxSettings.GetWidescreen() )
    {
        dwFlags |= D3DPRESENTFLAG_WIDESCREEN;
    }

	// Create our D3D Device Object
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	
	d3dpp.BackBufferWidth                 = uiWidth;    // SCREEN_WIDTH;
	d3dpp.BackBufferHeight                = uiHeight;   // SCREEN_HEIGHT;
	d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    if( uiWidth <= 1280 )
    {
        d3dpp.MultiSampleType             = D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN; // D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR; // D3DMULTISAMPLE_NONE; // D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN; 
    }
	d3dpp.BackBufferCount                 = 1;
    d3dpp.Flags                           |= dwFlags;
	
    d3dpp.hDeviceWindow                   = NULL;                    // Ignored on Xbox
    d3dpp.Windowed                        = FALSE;                   // Must be FALSE for Xbox
	d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.EnableAutoDepthStencil          = FALSE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT; // D3DPRESENT_INTERVAL_IMMEDIATE;
    
  	if ( FAILED ( hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT,
                                           D3DDEVTYPE_HAL,
                                           NULL,
                                           D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                           &d3dpp,
                                           &g_pD3DDevice ) ) )
	{
		XDBGWRN( APP_TITLE_NAME_A, "InitD3D():Failed to create our D3D Device Object!!" );
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
        XDBGWRN( APP_TITLE_NAME_A, "InitD3D():Failed to set the D3D Viewport!!" );
    }

	g_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    g_pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    g_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_pD3DDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , FALSE ); // Must do this to enable Edge Antialiasing
    g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
    g_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, TRUE );
    if( uiWidth <= 1280 )
    {
        // g_pD3DDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , TRUE );
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


// Initialize our 3D Objects for our scene
HRESULT Init3DObjects( void )
{
	g_GGroup0.loadStrippedMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj0.mdl", g_pD3DDevice );
	g_GGroup1.loadStrippedMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj1.mdl", g_pD3DDevice );
	g_GTubes.loadStrippedMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj2.mdl", g_pD3DDevice );
	g_GGroup3.loadStrippedMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj3.mdl", g_pD3DDevice );
	g_GGroup4.loadStrippedMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj4.mdl", g_pD3DDevice );
	g_GGroup5.loadStrippedMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj5.mdl", g_pD3DDevice );
	g_GGroup6.loadStrippedMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj6.mdl", g_pD3DDevice );
	g_GGroup7.loadStrippedMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj11.mdl", g_pD3DDevice );

    g_GTOMovieScreen.loadMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj7.mdl", g_pD3DDevice );
	// g_GTOMovieScreen.loadTexture( FILE_DATA_3DINFO_DIRECTORY_A "\\screenshot.bmp", g_pD3DDevice );

    g_XDKtitle.loadMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj8.mdl", g_pD3DDevice );
	// g_XDKtitle.loadTexture("t:\\screenshot.bmp", g_pD3DDevice );
    
    g_XDKaddress.loadMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj9.mdl", g_pD3DDevice );
	// g_XDKaddress.loadTexture("t:\\screenshot.bmp", g_pD3DDevice );

    g_InnerCell.loadMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj10.mdl", g_pD3DDevice );
    g_InnerCell.loadTexture( FILE_DATA_3DINFO_DIRECTORY_A  "\\cellwall.bmp", g_pD3DDevice );

    g_BuildNo.loadMDL( FILE_DATA_3DINFO_DIRECTORY_A "\\matobj12.mdl", g_pD3DDevice );

    //////////////////////////
    // Initialize Our Textures
    //////////////////////////
    g_MainTexture.Initialize( g_pD3DDevice, NULL, TEXTURE_MOVIESCREEN_WIDTH, TEXTURE_MOVIESCREEN_HEIGHT );
    g_XDKTitleTexture.Initialize( g_pD3DDevice, NULL, TEXTURE_XDKTITLE_WIDTH, TEXTURE_XDKTITLE_HEIGHT );
    g_XDKAddressTexture.Initialize( g_pD3DDevice, NULL, TEXTURE_XDKADDRESS_WIDTH, TEXTURE_XDKADDRESS_HEIGHT );
    g_BuildNoTexture.Initialize( g_pD3DDevice, NULL, TEXTURE_BUILDNO_WIDTH, TEXTURE_BUILDNO_HEIGHT );

    // Render the Build Number to the Texture
    g_BuildNoTexture.Lock();
    g_BuildNoTexture.Clear( COLOR_BLACK );
    XFONT_SetTextHeight( g_pFont, MENUBOX_BUILDNO_TEXT_HEIGHT );
    g_BuildNoTexture.DrawText( MENUBOX_BUILDNO_TEXT_XPOS, MENUBOX_BUILDNO_TEXT_YPOS, MENUBOX_BUILDNO_TEXT_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%hs", VER_PRODUCTVERSION_STR );
    XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );
    g_BuildNoTexture.Unlock();
    AlphaTexture( g_BuildNoTexture.GetTexture(), SCREEN_DEFAULT_TEXT_FG_COLOR );

    //////////////////////////////////////
    // Adjust the camera to our video mode
    //////////////////////////////////////
    g_mainCamera.readCAM( FILE_DATA_3DINFO_DIRECTORY_A "\\cameramainTV.cam" );

    // Set the Camera to Widescreen if needed
    if( g_XboxSettings.GetWidescreen() )
        g_mainCamera.SetWidescreen( TRUE );

	g_mainCamera.update( g_pD3DDevice );

    //
    // Make sure our textures are up to date
    //
    g_bUpdateNameAddrTexture = TRUE;
    g_bUpdateTitleTexture = TRUE;

    return S_OK;
}

#define FADE_LENGTH 500

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

    delta = NtGetTickCount() - startTime;

    if (delta >= FADE_LENGTH) {
        SetRamp(FADE_LENGTH);
        return FALSE;
    } else {
        SetRamp(delta);
        return TRUE;
    }
}

// Initialize our application - video, sound, etc
HRESULT Init( void )
{
    XDBGTRC( APP_TITLE_NAME_A, "Init()" );


    ////////////////////////////
    // Set the File System Cache
    ////////////////////////////
    XSetFileCacheSize( FILE_DATA_CACHE_SIZE );

    ///////////////////
    // Mount any MUs
    ///////////////////

    // Mount any MU that is inserted
    g_dwInsertedMUs = XGetDevices( XDEVICE_TYPE_MEMORY_UNIT );
    for( unsigned int x = 0; x < NUM_XBOX_MU_SLOTS; ++x )
    {
        if( g_dwInsertedMUs & g_dwMUBitMask[x] )
            g_MemoryUnits[x].Mount();
    }


    ///////////////////
    // Mount our Drives
    ///////////////////

    // Mount the C and the Y Drives
    g_XboxHardDrive.MapDrive( 'C', FILE_EXECUTABLE_DEVICE_PATH_A );
    g_XboxHardDrive.MapDrive( 'Y', FILE_DASH_PARTITION_PATH_A );
    
    /////////////////
    // Initialize D3D
    /////////////////
    if( FAILED( InitD3D( SCREEN_WIDTH, SCREEN_HEIGHT, 0 ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "Init():Failed to Initialize D3D!!" );

    ////////////////////////////////////////
    // Setup and Initialize our Default FONT
    ////////////////////////////////////////
    if( NULL != g_pFont )
    {
        XFONT_Release( g_pFont );
        g_pFont = NULL;
    }

    XFONT_OpenTrueTypeFont( FILE_DATA_DEFAULT_FONT_FILENAME, 4096, &g_pFont );

	if( NULL != g_pFont )
    {
        XFONT_SetTextColor( g_pFont, SCREEN_DEFAULT_TEXT_FG_COLOR );
        XFONT_SetBkColor( g_pFont, SCREEN_DEFAULT_TEXT_BG_COLOR );
        XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );
        XFONT_SetTextAntialiasLevel( g_pFont, FONT_ALIAS_LEVEL );
        XFONT_SetTextStyle( g_pFont, FONT_DEFAULT_STYLE );
    }
    else
    {
        XDBGWRN( APP_TITLE_NAME_A, "Init():Failed to Initialize our Font!!" );
    }

    ////////////////////////////
    // Initialize our 3D Objects
    ////////////////////////////
    if( FAILED( Init3DObjects() ) )
        XDBGWRN( APP_TITLE_NAME_A, "Init():Failed to Initialize our 3D Objects!!" );


    ////////////////////
    // Initialize DSound
    ////////////////////
    if( FAILED( InitDSound() ) )
        XDBGWRN( APP_TITLE_NAME_A, "Init():Failed to Initialize DSound!!" );
    
    ////////////////////////
    // Initialize our Sounds
    ////////////////////////
    if( FAILED( g_Sounds.Init( g_pDSound ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "Init():Failed to Initialize our Sounds!!" );
    

    //////////////////////////////
    // Initialize our Memory Units
    //////////////////////////////
    g_MemoryUnits[0].SetPortSlot( XDEVICE_PORT0, XDEVICE_TOP_SLOT );
    g_MemoryUnits[1].SetPortSlot( XDEVICE_PORT0, XDEVICE_BOTTOM_SLOT );
    g_MemoryUnits[2].SetPortSlot( XDEVICE_PORT1, XDEVICE_TOP_SLOT );
    g_MemoryUnits[3].SetPortSlot( XDEVICE_PORT1, XDEVICE_BOTTOM_SLOT );
    g_MemoryUnits[4].SetPortSlot( XDEVICE_PORT2, XDEVICE_TOP_SLOT );
    g_MemoryUnits[5].SetPortSlot( XDEVICE_PORT2, XDEVICE_BOTTOM_SLOT );
    g_MemoryUnits[6].SetPortSlot( XDEVICE_PORT3, XDEVICE_TOP_SLOT );
    g_MemoryUnits[7].SetPortSlot( XDEVICE_PORT3, XDEVICE_BOTTOM_SLOT );

    ////////////////////////
    // Get our Xbox Settings
    ////////////////////////
    g_XboxSettings.Init();

    /////////////////////////
    // Initialize Xbox Online
    /////////////////////////
    XONLINE_STARTUP_PARAMS xoStartup;
    ZeroMemory( &xoStartup, sizeof( xoStartup) );

    if( FAILED( XOnlineStartup( &xoStartup ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "Init():Failed to Initialize XOnline! (Network Too!)" );
    }
    else
    {
        g_bNetOK = TRUE;
    }
        
    /*
    //////////////////
    // Initialize XNet
    //////////////////
    XNetStartupParams xnsp = { sizeof(XNetStartupParams), XNET_STARTUP_BYPASS_SECURITY };
    int nXnetResult = XNetStartup( &xnsp );

    if( NO_ERROR == nXnetResult )
    {
        WSADATA wsaData;

        int WSAErr = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );

        if( 0 == WSAErr )
            g_bNetOK = TRUE;
        else
            XDBGWRN( APP_TITLE_NAME_A, "Init():WSAStartup had a failure!!" );
    }
    else
    {
        XDBGWRN( APP_TITLE_NAME_A, "Init():XNetStartup had a failure!!" );
    }
    */
    
    ////////////////////////
    // Initialize Key Combo
    ////////////////////////
    ZeroMemory( g_pszKeyCombo, 5 );
    
    ////////////////////////
    // Setup Menus and Items
    ////////////////////////
    SetupMenusAndItems();

    ///////////////////////////////////////////////
    // Find out if any data was passed to the Shell
    ///////////////////////////////////////////////
    DWORD dwLaunchDataType = 0;
    LD_LAUNCH_DASHBOARD launchDashboard;

    ZeroMemory( &launchDashboard, sizeof( launchDashboard ) );

    // Determine if something is passing the shell some information
    if( ERROR_SUCCESS == XGetLaunchInfoShell( &dwLaunchDataType, &launchDashboard ) )
    {
        // Check to see what booted us, the Dashboard or a Title
        if( LDT_LAUNCH_DASHBOARD == dwLaunchDataType )
        {
            XDBGWRN( APP_TITLE_NAME_A, "Init():LDT_LAUNCH_DASHBOARD" );
            XDBGWRN( APP_TITLE_NAME_A, "    dwReason     - '%d'", launchDashboard.dwReason );
            XDBGWRN( APP_TITLE_NAME_A, "    dwContext    - '%d'", launchDashboard.dwContext );
            XDBGWRN( APP_TITLE_NAME_A, "    dwParameter1 - '%d'", launchDashboard.dwParameter1 );
            XDBGWRN( APP_TITLE_NAME_A, "    dwParameter2 - '%d'", launchDashboard.dwParameter2 );
        
            // Check to see if the error was "Invalid XBE" -- That's the only error
            // we care about for now.  Anything else is ignored
            if( ( XLD_LAUNCH_DASHBOARD_ERROR == launchDashboard.dwReason ) && ( XLD_ERROR_INVALID_XBE == launchDashboard.dwParameter1 ) )
            {
                g_dwShellState |= SHELLSTATE_ERROR;                         // Enables the "Error" bitmask in the shell state
                g_ErrorMsgMenu.SetParent( g_pRootMenu );                    // Set the Parent menu of the Error Message menu to the "root" menu
                g_pCurrentScreen = &g_ErrorMsgMenu;                         // Set the current screen to the error message

                g_ErrorMsgMenu.SetErrorCode( ERRORCODE_INVALID_XBE );       // Set the error code
            }
            else
            {
                // Check to see if we need to prompt the user for the DATE/TIME info
                if( ( XLD_LAUNCH_DASHBOARD_SETTINGS == launchDashboard.dwReason ) || ( XLD_LAUNCH_DASHBOARD_ERROR == launchDashboard.dwReason ) )
                {
                    // Check to see if the Dashboard exists
                    XDBGTRC( APP_TITLE_NAME_A, "Init(): Need to reboot to Dashboard to set settings..." );
                    if( g_XboxHardDrive.FileExists( FILE_DATA_DASHBOARD_LOCATION_A ) )
                    {
                        // If so, reboot to the Dashboard passing the relevant information
                        char pszDirname[MAX_PATH+1];
                        pszDirname[MAX_PATH] = 0;

                        _snprintf( pszDirname, MAX_PATH, "%s", FILE_DASH_PARTITION_PATH_A );
                        NTSTATUS status = XWriteTitleInfoAndReboot( FILE_DATA_DASHBOARD_FILENAME_A, pszDirname, LDT_LAUNCH_DASHBOARD, 0, (PLAUNCH_DATA)&launchDashboard );
                    }
                    else
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "Init(): Dashboard didn't exist, so settings might be incorrect!!" );
                    }
                }
            }
        }
        else if( LDT_FROM_DASHBOARD == dwLaunchDataType )
        {
            LD_FROM_DASHBOARD* pFromDash = (LD_FROM_DASHBOARD*)&launchDashboard;

            XDBGWRN( APP_TITLE_NAME_A, "Init():LD_FROM_DASHBOARD" );
            XDBGWRN( APP_TITLE_NAME_A, "    dwContext    - '%d'", pFromDash->dwContext );
        }
    }


    //////////////////////////////////////////////////////////////////////////////////
    // Check to see if the user has configured their Xbox by checking the machine name
    //////////////////////////////////////////////////////////////////////////////////
    if( strcmp( g_XboxSettings.GetMachineName(), "" ) == 0 )
    {
        g_dwShellState |= SHELLSTATE_NOCONFIG;  // Enables the "No Config" bitmask in the shell state
        
        // Determine if we are already in an error state
        if( SHELLSTATE_ERROR & g_dwShellState )
        {
            // Set the Parent of the Error Message menu to the "Machine Name" menu
            g_ErrorMsgMenu.SetParent( g_LaunchMenu.GetItemPtr( ENUM_SHELLMENUID_OPTIONS )->GetItemPtr( ENUM_SHELLMENUID_OPTIONS_NETWORK )->GetItemPtr( ENUM_SHELLMENUID_NETWORK_MACHINENAME ) );
        }
        else
        {
            // Set the Current Menu to the Machine Name menu
            g_pCurrentScreen = g_LaunchMenu.GetItemPtr( ENUM_SHELLMENUID_OPTIONS )->GetItemPtr( ENUM_SHELLMENUID_OPTIONS_NETWORK )->GetItemPtr( ENUM_SHELLMENUID_NETWORK_MACHINENAME );  // Set the current screen to the Machine Name menu
        }
    }

    
    //////////////////////////////////////////////////
    // Check to see if we are in a good (normal) state
    //////////////////////////////////////////////////
    if( SHELLSTATE_NORMAL == g_dwShellState )
    {
        g_pCurrentScreen = g_pRootMenu; // Set the current menu to the "Root Menu"
    }

    return S_OK;
}

// Clean up our application (close drivers, etc)
HRESULT CleanUp( void )
{
    XDBGTRC( APP_TITLE_NAME_A, "CleanUp()" );

    // Clean up D3D
    if( NULL != g_pD3DDevice )
    {
        g_pD3DDevice->Release();
        g_pD3DDevice = NULL;
    }

    // Clean up DSound
    if( NULL != g_pDSound )
    {
        g_pDSound->Release();
        g_pDSound = NULL;
    }

    // Clean up the Font
    if( NULL != g_pFont )
    {
        XFONT_Release( g_pFont );
    }
    
    // Clean up XOnline
    if( g_bNetOK )
    {
        if( FAILED( XOnlineCleanup() ) )
            XDBGWRN( APP_TITLE_NAME_A, "CleanUp():Failed to cleanup XOnline!!" );
    }

    /*
    // Clean up XNet and WS
    // BUGBUG -- What happens if one or both of these fail, and we are calling cleanup??
    XNetCleanup();
    WSACleanup();
    */

    // Unmap our drives
    g_XboxHardDrive.UnMapDrive( 'C' );
    g_XboxHardDrive.UnMapDrive( 'Y' );

    return S_OK;
}


// Handle input from the user
HRESULT ProcessInput( void )
{
    BOOL bFirstPress;
    BOOL bFirstPress2;
    BOOL bFirstPress3;
    BOOL bFirstXPress;
    BOOL bFirstYPress;
    BOOL bRemoteButtonPressed = FALSE;  // Used to deal with the multiple controls mappings (A/OK/START, B/BACK)
    BOOL bControlPressed = FALSE;       // Used to deal with the multiple controls mappings (A/OK/START, B/BACK)
    BOOL bButtonPressed = FALSE;        // Used to deal with the multiple controls mappings (A/OK/START, B/BACK)
    int  nThumbY = 0;
    int  nThumbX = 0;

    // Update the state of all controllers
    Controllers.ProcessInput();

    // Check to see if our KeyPress position is the end of the buffer
    // If so, reset it
    if( g_uiKeyPos == 4 )
    {
        g_uiKeyPos = 0;
    }

    // START CONTROL / A BUTTON
    bControlPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_START, &bFirstPress );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_A, BUTTON_THRESHOLD, &bFirstPress2 );
    bRemoteButtonPressed = Controllers.IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_OK, &bFirstPress3 );
    if( bControlPressed || bButtonPressed || bRemoteButtonPressed )
    {
        XDBGTRC( APP_TITLE_NAME_A, "A/OK/START Pressed" );

        g_bHighlightAButton = TRUE; // Highlight the A Button

        // Check to see if one of the buttons is a 'repeat' press.  If so, report FALSE for first press
        if( bControlPressed && ( !bFirstPress ) )
        {
            bFirstPress = FALSE;
        }
        else if( bButtonPressed && ( !bFirstPress2 ) )
        {
            bFirstPress = FALSE;
        }
        else if( bRemoteButtonPressed && ( !bFirstPress3 ) )
        {
            bFirstPress = FALSE;
        }
        else
        {
            bFirstPress = TRUE;
        }

        // Update the combo with the A Button Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'A';
        }

        // For the "Start" Button, we will send the "A" button signal to the menu, so that act identically
        g_pCurrentScreen->HandleInput( BUTTON_A, bFirstPress );
    }
    else
    {
        g_bHighlightAButton = FALSE; // Make sure the A Button isn't highlighted
    }

    // BACK CONTROL / B BUTTON
    bControlPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_BACK, &bFirstPress );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_B, BUTTON_THRESHOLD, &bFirstPress2 );
    bRemoteButtonPressed = Controllers.IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_REPLAY, &bFirstPress3 );
    if( bControlPressed || bButtonPressed || bRemoteButtonPressed )
    {
        XDBGTRC( APP_TITLE_NAME_A, "B/BACK AGAIN/BACK Pressed" );

        g_bHighlightBButton = TRUE; // Highlight the B Button

        // Check to see if one of the buttons is a 'repeat' press.  If so, report FALSE for first press
        if( bControlPressed && ( !bFirstPress ) )
        {
            bFirstPress = FALSE;
        }
        else if( bButtonPressed && ( !bFirstPress2 ) )
        {
            bFirstPress = FALSE;
        }
        else if( bRemoteButtonPressed && ( !bFirstPress3 ) )
        {
            bFirstPress = FALSE;
        }
        else
        {
            bFirstPress = TRUE;
        }

        // Update the combo with the B Button Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'B';
        }

        // For the "Back" Button, we will send the "B" button signal to the menu, so that act identically
        g_pCurrentScreen->HandleInput( BUTTON_B, bFirstPress );
    }
    else
    {
        g_bHighlightBButton = FALSE; // Make sure the B Button isn't highlighted
    }

    // X MENU BUTTON
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_X, BUTTON_THRESHOLD, &bFirstPress );
    bRemoteButtonPressed = Controllers.IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_MENU, &bFirstPress2 );
    if( bButtonPressed || bRemoteButtonPressed )
    {
        if( bButtonPressed && ( !bFirstPress ) )
        {
            bFirstPress = FALSE;
        }
        else if( bRemoteButtonPressed && ( !bFirstPress2 ) )
        {
            bFirstPress = FALSE;
        }
        else
        {
            bFirstPress = TRUE;
        }

        // Update the combo with the X Button Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'X';
        }

        g_pCurrentScreen->HandleInput( BUTTON_X, bFirstPress );
    }

    // Y BUTTON
    if( Controllers.IsButtonPressed( PORT_ANY, BUTTON_Y, BUTTON_THRESHOLD, &bFirstPress ) )
    {
        // Update the combo with the Y Button Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'Y';
        }

        g_pCurrentScreen->HandleInput( BUTTON_Y, bFirstPress );
    }

    // BLACK BUTTON
    if( Controllers.IsButtonPressed( PORT_ANY, BUTTON_BLACK, BUTTON_THRESHOLD, &bFirstPress ) )
    {
        // HACK: toggle between IP address and MAC address
        if( ( bFirstPress ) && ( g_bToggleIPInfo ) )
        {
            g_iDisplayAddress += 1;
            if (g_iDisplayAddress >= 4)
                g_iDisplayAddress = 0;
            g_bUpdateNameAddrTexture = TRUE;
        }

        // Update the combo with the Black Button Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'L';
        }

        g_pCurrentScreen->HandleInput( BUTTON_BLACK, bFirstPress );
    }

    // WHITE BUTTON
    if( Controllers.IsButtonPressed( PORT_ANY, BUTTON_WHITE, BUTTON_THRESHOLD, &bFirstPress ) )
    {
        // Update the combo with the White Button Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'W';
        }

        g_pCurrentScreen->HandleInput( BUTTON_WHITE, bFirstPress );
    }


    // UP DPAD
    bControlPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_UP, &bFirstPress );
    bRemoteButtonPressed = Controllers.IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_UP, &bFirstPress2 );
    if( bControlPressed || bRemoteButtonPressed )
    {
        if( bControlPressed && ( !bFirstPress ) )
        {
            bFirstPress = FALSE;
        }
        else if( bRemoteButtonPressed && ( !bFirstPress2 ) )
        {
            bFirstPress = FALSE;
        }
        else
        {
            bFirstPress = TRUE;
        }

        // Update the combo with the DPAD Up Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'U';
        }

        g_pCurrentScreen->HandleInput( CONTROL_DPAD_UP, bFirstPress );
    }

    // DOWN DPAD / REMOTE DOWN
    bControlPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_DOWN, &bFirstPress );
    bRemoteButtonPressed = Controllers.IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_DOWN, &bFirstPress2 );
    if( bControlPressed || bRemoteButtonPressed )
    {
        if( bControlPressed && ( !bFirstPress ) )
        {
            bFirstPress = FALSE;
        }
        else if( bRemoteButtonPressed && ( !bFirstPress2 ) )
        {
            bFirstPress = FALSE;
        }
        else
        {
            bFirstPress = TRUE;
        }

        // Update the combo with the DPAD Down Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'D';
        }

        g_pCurrentScreen->HandleInput( CONTROL_DPAD_DOWN, bFirstPress );
    }

    // LEFT DPAD
    bControlPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_LEFT, &bFirstPress );
    bRemoteButtonPressed = Controllers.IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_LEFT, &bFirstPress2 );
    if( bControlPressed || bRemoteButtonPressed )
    {
        if( bControlPressed && ( !bFirstPress ) )
        {
            bFirstPress = FALSE;
        }
        else if( bRemoteButtonPressed && ( !bFirstPress2 ) )
        {
            bFirstPress = FALSE;
        }
        else
        {
            bFirstPress = TRUE;
        }

        // Update the combo with the DPAD Left Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'F';
        }

        g_pCurrentScreen->HandleInput( CONTROL_DPAD_LEFT, bFirstPress );
    }

    // RIGHT DPAD
    bControlPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_RIGHT, &bFirstPress );
    bRemoteButtonPressed = Controllers.IsRemoteButtonPressed( PORT_ANY, REMOTE_BUTTON_RIGHT, &bFirstPress2 );
    if( bControlPressed || bRemoteButtonPressed )
    {
        if( bControlPressed && ( !bFirstPress ) )
        {
            bFirstPress = FALSE;
        }
        else if( bRemoteButtonPressed && ( !bFirstPress2 ) )
        {
            bFirstPress = FALSE;
        }
        else
        {
            bFirstPress = TRUE;
        }

        // Update the combo with the DPAD Right Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'R';
        }

        g_pCurrentScreen->HandleInput( CONTROL_DPAD_RIGHT, bFirstPress );
    }

    // LEFT THUMB BUTTON
    bControlPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_LEFT_THUMB, &bFirstPress );
    if( bControlPressed )
    {
        // Update the combo with the Left Thumb Button Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'T';
        }

        g_pCurrentScreen->HandleInput( CONTROL_LEFT_THUMB, bFirstPress );
    }

    // RIGHT THUMB BUTTON
    bControlPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_RIGHT_THUMB, &bFirstPress );
    if( bControlPressed )
    {
        // Update the combo with the Right Thumb Button Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'I';
        }

        g_pCurrentScreen->HandleInput( CONTROL_RIGHT_THUMB, bFirstPress );
    }

    // LEFT TRIGGER BUTTON
    if( Controllers.IsButtonPressed( PORT_ANY, BUTTON_LEFT_TRIGGER, BUTTON_THRESHOLD, &bFirstPress ) )
    {
        // Update the combo with the Left-Trigger Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'E';
        }

        g_pCurrentScreen->HandleInput( BUTTON_LEFT_TRIGGER, bFirstPress );
    }

    // Right Trigger Button
    if( Controllers.IsButtonPressed( PORT_ANY, BUTTON_RIGHT_TRIGGER, BUTTON_THRESHOLD, &bFirstPress ) )
    {
        // Update the combo with the Right Trigger Button Position
        if( bFirstPress )
        {
            g_pszKeyCombo[g_uiKeyPos] = 'O';
        }

        g_pCurrentScreen->HandleInput( BUTTON_RIGHT_TRIGGER, bFirstPress );
    }


    // LEFT THUMB STICK
    nThumbY = Controllers.GetJoystickY( PORT_ANY, JOYSTICK_LEFT, JOYSTICK_DEAD_ZONE, &bFirstYPress );
    nThumbX = Controllers.GetJoystickX( PORT_ANY, JOYSTICK_LEFT, JOYSTICK_DEAD_ZONE, &bFirstXPress );
    if( ( nThumbY != 0 ) || ( nThumbX != 0 ) )
    {
        g_pCurrentScreen->HandleInput( JOYSTICK_LEFT, nThumbY, nThumbX, bFirstYPress, bFirstXPress );
    }
    
    // RIGHT THUMB STICK
    nThumbY = Controllers.GetJoystickY( PORT_ANY, JOYSTICK_RIGHT, JOYSTICK_DEAD_ZONE, &bFirstYPress );
    nThumbX = Controllers.GetJoystickX( PORT_ANY, JOYSTICK_RIGHT, JOYSTICK_DEAD_ZONE, &bFirstXPress );
    if( ( nThumbY != 0 ) || ( nThumbX != 0 ) )
    {
        g_pCurrentScreen->HandleInput( JOYSTICK_RIGHT, nThumbY, nThumbX, bFirstYPress, bFirstXPress );
    }

    return S_OK;
}

// Setup our Menus and Items for our application
HRESULT SetupMenusAndItems( void )
{
    XDBGTRC( APP_TITLE_NAME_A, "  SetupMenusAndItems()" );

    HRESULT hr = S_OK;

    g_pRootMenu = &g_LaunchMenu;    // Set the launch menu as our "root" menu

    // Setup the Launch Menu
    hr = g_LaunchMenu.Init( FILE_DATA_MENU_DIRECTORY_A "\\root.mnu" );
    if( FAILED( hr ) )
        XDBGERR( APP_TITLE_NAME_A, "SetupMenusAndItems():Failed to add the launch item info!!" );

    // Setup the Error Msg Menu
    hr = g_ErrorMsgMenu.Init( FILE_DATA_MENU_DIRECTORY_A "\\errormsg.mnu" );
    if( FAILED( hr ) )
        XDBGERR( APP_TITLE_NAME_A, "SetupMenusAndItems():Failed to add the error message item info!!" );

    // Setup the Generic Keyboard Menu
    hr = g_GenKeyboardMenu.Init( MENU_GENKEYBOARD_MENU_FILENAME );
    if( FAILED( hr ) )
        XDBGERR( APP_TITLE_NAME_A, "SetupMenusAndItems():Failed to Init the Generic Keyboard menu!!" );

    // Setup our Message Menu
    hr = g_MessageMenu.Init( MENU_MESSAGE_MENU_FILENAME );
    if( FAILED( hr ) )
        XDBGERR( APP_TITLE_NAME_A, "SetupMenusAndItems():Failed to Init the Generic Message menu!!" );

    // Setup our Memory Display Menu
    hr = g_MADisplayMenu.Init( MADISPLAYMENU_FILENAME_A );
    if( FAILED( hr ) )
        XDBGERR( APP_TITLE_NAME_A, "SetupMenusAndItems():Failed to Init the Memory Area Display menu!!" );

    // Setup our Yes / No Menu
    hr = g_YesNoMenu.Init( MENU_YESNO_MENU_FILENAME_A );
    if( FAILED( hr ) )
        XDBGERR( APP_TITLE_NAME_A, "SetupMenusAndItems():Failed to Init the Yes / No menu!!" );

    return hr;
};


// This is used to obtain any information that is being passed to the
// XShell
extern "C" {
extern PLAUNCH_DATA_PAGE *LaunchDataPage;
}

DWORD
WINAPI
XGetLaunchInfoShell(
    OUT PDWORD pdwLaunchDataType,
    OUT PLD_LAUNCH_DASHBOARD pLaunchDashboard
    )
{
    RIP_ON_NOT_TRUE("XGetLaunchInfoShell()", (pdwLaunchDataType != NULL));
    RIP_ON_NOT_TRUE("XGetLaunchInfoShell()", (pLaunchDashboard != NULL));

    if( *LaunchDataPage )
    {
        *pdwLaunchDataType = (*LaunchDataPage)->Header.dwLaunchDataType;

        if( (*LaunchDataPage)->Header.dwLaunchDataType == LDT_LAUNCH_DASHBOARD )
            memcpy( pLaunchDashboard, (*LaunchDataPage)->LaunchData, sizeof( LD_LAUNCH_DASHBOARD ) );
        else if( (*LaunchDataPage)->Header.dwLaunchDataType == LDT_FROM_DASHBOARD )
            memcpy( pLaunchDashboard, (*LaunchDataPage)->LaunchData, sizeof( LD_FROM_DASHBOARD ) );

        PLAUNCH_DATA_PAGE pTemp = *LaunchDataPage;
        *LaunchDataPage = NULL;
        MmFreeContiguousMemory(pTemp);

        return ERROR_SUCCESS;
    }
    else
    {
        return ERROR_NOT_FOUND;
    }
}


// Will update the texture with the XDK TITLE on it
void UpdateTitleTexture( CUDTexture* pTexture )
{
    // Render the title to the texture
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    XFONT_SetTextHeight( g_pFont, MENUBOX_TITLE_TEXT_FONT_HEIGHT );
    pTexture->DrawText( MENUBOX_TITLE_TEXT_XPOS, MENUBOX_TITLE_TEXT_YPOS, MENUBOX_TITLE_TEXT_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%ls", g_pCurrentScreen->GetTitle() );
    XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

    pTexture->Unlock();

    AlphaTexture( pTexture->GetTexture(), SCREEN_DEFAULT_TEXT_FG_COLOR );
}


// Will update the texture with the machine name and IP Address of the current Xbox
void UpdateAddressTexture( CUDTexture* pTexture )
{
    /////////////////////////////////////
    // Draw the machine name & IP Address
    /////////////////////////////////////

    // Lock the Address Texture so we can render on to it
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    #define ADR_TEX_BUFFSIZE    128

    WCHAR tempBuff[ADR_TEX_BUFFSIZE];
    ZeroMemory( tempBuff, ADR_TEX_BUFFSIZE * sizeof( WCHAR ) );

    char* machineName = g_XboxSettings.GetMachineName();

    if (g_bNetOK)
    {
        XNADDR xnaddr;
        UINT iTimeout = 500;    // Five seconds maximum just in case

        if (g_iDisplayAddress >= 2)
            while (XNetGetTitleXnAddr(&xnaddr) == 0 && iTimeout-- > 0) Sleep(10);
        else
            while (XNetGetDebugXnAddr(&xnaddr) == 0 && iTimeout-- > 0) Sleep(10);

        if (g_iDisplayAddress & 1)
            _snwprintf(tempBuff, ADR_TEX_BUFFSIZE, L"%S (%02x-%02x-%02x-%02x-%02x-%02x%S",
                       machineName, xnaddr.abEnet[0], xnaddr.abEnet[1], xnaddr.abEnet[2],
                       xnaddr.abEnet[3], xnaddr.abEnet[4], xnaddr.abEnet[5],
                       (g_iDisplayAddress >= 2) ? ") T" : ") D");
        else
            _snwprintf(tempBuff, ADR_TEX_BUFFSIZE, L"%S (%d.%d.%d.%d%S", machineName,
                       xnaddr.ina.S_un.S_un_b.s_b1, xnaddr.ina.S_un.S_un_b.s_b2, 
                       xnaddr.ina.S_un.S_un_b.s_b3, xnaddr.ina.S_un.S_un_b.s_b4,
                       (g_iDisplayAddress >= 2) ? ") T" : ") D");
    }
    else
    {
        _snwprintf(tempBuff, ADR_TEX_BUFFSIZE, L"%S", machineName);
    }

    XFONT_SetTextHeight( g_pFont, MENUBOX_NAMEADDR_TEXT_HEIGHT );

    float XPos = ( TEXTURE_XDKADDRESS_PIXEL_WIDTH / 2.0f ) - ( GetStringPixelWidth( tempBuff ) / 2.0f );
    pTexture->DrawText( XPos, MENUBOX_NAMEADDR_TEXT_YPOS, MENUBOX_NAMEADDR_TEXT_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", tempBuff );

    XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

    pTexture->Unlock();

    // Make the texture background transparent
    AlphaTexture( pTexture->GetTexture(), SCREEN_DEFAULT_TEXT_FG_COLOR );
}


// Process Special Key-Sequences
void ProcessKeySequences()
{
    BOOL bFirstPress = FALSE;

    if( Controllers.IsButtonPressed( PORT_ANY, BUTTON_LEFT_TRIGGER, BUTTON_THRESHOLD, &bFirstPress ) &&
        Controllers.IsButtonPressed( PORT_ANY, BUTTON_RIGHT_TRIGGER, BUTTON_THRESHOLD, &bFirstPress ) )
    {
        if( Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_UP, &bFirstPress ) )
        {
            // PAL-I
            g_XboxSettings.SetAVRegion( XSETTINGS_DISPLAY_SETTING_PAL_I );
            XLaunchNewImage( NULL, NULL );
        }
        else if( Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_RIGHT, &bFirstPress ) )
        {
            // NTSC-M
            g_XboxSettings.SetAVRegion( XSETTINGS_DISPLAY_SETTING_NTSC_M );
            XLaunchNewImage( NULL, NULL );
        }
        else if( Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_LEFT, &bFirstPress ) )
        {
            // NTSC-J
            g_XboxSettings.SetAVRegion( XSETTINGS_DISPLAY_SETTING_NTSC_J );
            XLaunchNewImage( NULL, NULL );
        }
    }
}


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
            XDBGWRN( APP_TITLE_NAME_A, "AlphaTexture():Failed to lock the texture!!" );
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
        XDBGWRN( APP_TITLE_NAME_A, "AlphaTexture():Invalid Texture Format!!" );
	}

}


// Get the current Width of the screen
unsigned int GetScreenWidth()
{
    D3DDISPLAYMODE  d3dDisplayMode;
    ZeroMemory( &d3dDisplayMode, sizeof( d3dDisplayMode ) );

    HRESULT hr = g_pD3DDevice->GetDisplayMode( &d3dDisplayMode );
    if( FAILED( hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::GetScreenWidth():Failed to get the Display Mode!! Error - '0x%.8X (%d)'", hr, hr );

        return 0;
    }

    return d3dDisplayMode.Width;
}


// Get the current Height of the screen
unsigned int GetScreenHeight()
{
    D3DDISPLAYMODE  d3dDisplayMode;
    ZeroMemory( &d3dDisplayMode, sizeof( d3dDisplayMode ) );

    HRESULT hr = g_pD3DDevice->GetDisplayMode( &d3dDisplayMode );
    if( FAILED( hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertMenu::GetScreenHeight():Failed to get the Display Mode!! Error - '0x%.8X (%d)'", hr, hr );

        return 0;
    }

    return d3dDisplayMode.Height;
}


// Check to see if an MU has been inserted or removed, and properly Mounts, or
// Dismounts the MUs
void CheckMUs( void )
{
    DWORD dwMUInsertions;
    DWORD dwMURemovals;

    // If the state has changed, update our texture and our previous mask
    g_bMUsChanged = XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT, &dwMUInsertions, &dwMURemovals );
    if( g_bMUsChanged )
    {
        // Mount or Unmount our MU's
        for( unsigned int x = 0; x < NUM_XBOX_MU_SLOTS; ++x )
        {
            // Check to see if the MU was removed
            if( dwMURemovals & g_dwMUBitMask[x] )
            {
                // Remove the MU from our Mask
                g_dwInsertedMUs = g_dwInsertedMUs & ( ~ g_dwMUBitMask[x] );

                // Unmount the MU
                g_MemoryUnits[x].Unmount();

            }

            // Check to see if the MU is inserted
            if( dwMUInsertions & g_dwMUBitMask[x] )
            {
                // Add the MU to our Mask
                g_dwInsertedMUs = g_dwInsertedMUs | g_dwMUBitMask[x];

                // Mount the MU
                g_MemoryUnits[x].Mount();
            }
        }
    }
}


// Used to display a generic message to the user
void DisplayMessage( WCHAR* pwszTitle, WCHAR* pwszFormat, ... )
{
    if( NULL == pwszFormat )
        return;

    WCHAR pwszBuffer[4096];

    va_list args;
    va_start( args, pwszFormat );

    vswprintf( pwszBuffer, pwszFormat, args );

    if( NULL == pwszTitle )
        g_MessageMenu.SetTitle( MENU_TITLE_MESSAGE );
    else
        g_MessageMenu.SetTitle( pwszTitle );

    g_MessageMenu.SetText( pwszBuffer );
    g_MessageMenu.SetParent( g_pCurrentScreen );

    g_pCurrentScreen->Leave( &g_MessageMenu );

    va_end(args);
}


// Used to display a generic error message to the user
void DisplayErrorMessage( WCHAR* pwszFormat, ... )
{
    if( NULL == pwszFormat )
        return;

    va_list args;
    va_start( args, pwszFormat );

    DisplayMessage( L"Error", pwszFormat, args );

    va_end(args);
}

// Used to display a generic error message to the user
void DisplayYesNoMessage( WCHAR* pwszTitle, WCHAR* pwszFormat, ... )
{
    if( NULL == pwszFormat )
        return;

    WCHAR pwszBuffer[4096];

    va_list args;
    va_start( args, pwszFormat );

    vswprintf( pwszBuffer, pwszFormat, args );

    if( NULL == pwszTitle )
        g_YesNoMenu.SetTitle( MENU_TITLE_YESNO );
    else
        g_YesNoMenu.SetTitle( pwszTitle );

    g_YesNoMenu.SetText( pwszBuffer );
    g_YesNoMenu.SetParent( g_pCurrentScreen );

    g_pCurrentScreen->Leave( &g_YesNoMenu );

    va_end(args);
}


// Used to determine if the Yes/No menu was cancelled
BOOL YesNoMenuCancelled()
{
    return g_YesNoMenu.GetCancelled();
}