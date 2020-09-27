#include "stdafx.h"
#include "menuscreen.h"

// Forward declarations
HRESULT DrawScreen(void );
HRESULT SetupMenusAndItems( void );
HRESULT CleanUp( void );
HRESULT Init( void );

// Menus are created here
CMenuScreen g_RootMenu( NULL );     // Only screen in our app

// Other globals
CXBoxVideo Screen;                  // Screen object where all text / info is displayed
USBManager Controllers;             // Used to determine what buttons / direction the user is pressing
CXItem* g_pCurrentScreen = NULL;	// Pointer to the current menu object

void _cdecl main()
{
    XDBGTRC( APP_TITLE_NAME_A, "***** main()" );

    Init();

    do
    {
        DrawScreen();
    } while( true );

    CleanUp();

    XDBGTRC( APP_TITLE_NAME_A, "***** main()" );
}

// Draw our screen
HRESULT DrawScreen( void )
{
    g_pCurrentScreen->Action( &Screen );        // Current menu action
    Screen.ShowScreen();                        // Move the screen from the backbuffer to the foreground

    return S_OK;
}

// Initialize our application - video, sound, etc
HRESULT Init( void )
{
    XDBGTRC( APP_TITLE_NAME_A, "Init()" );

    // Initialize our drawing routines (d3d, etc)
    Screen.Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );

    // Display our 'splash' screen
    BitmapFile oSplashScreen;
    oSplashScreen.read( 0, 0, FILE_SPLASHSCREEN_LOCATION_A );

    Screen.ClearScreen( COLOR_BLACK );
    oSplashScreen.render( Screen.GetD3DDevicePtr() );
    Screen.ShowScreen();

    // Setup Menus and Items
    SetupMenusAndItems();

    return S_OK;
}

// Clean up our application (close drivers, etc)
HRESULT CleanUp( void )
{
    XDBGTRC( APP_TITLE_NAME_A, "CleanUp()" );

    Screen.DeInitialize();

    return S_OK;
}

// Setup our Menus and Items for our application
HRESULT SetupMenusAndItems( void )
{
    XDBGTRC( APP_TITLE_NAME_A, "  SetupMenusAndItems()" );

    HRESULT hr = S_OK;

    // Setup the Root Menu
    hr = g_RootMenu.Init( &Screen, FILE_DATA_MENU_DIRECTORY_A "\\root.mnu" );
    if( FAILED( hr ) )
    {
        XDBGERR( APP_TITLE_NAME_A, "SetupMenusAndItems():Failed to add the root item info!!" );
    }

    // Establish our root menu
    g_pCurrentScreen = &g_RootMenu;

    return hr;
};
