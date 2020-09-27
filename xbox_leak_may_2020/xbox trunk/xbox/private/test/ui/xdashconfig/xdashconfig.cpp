#include "stdafx.h"

// Forward declarations
// void DisplayMemStats( void );
HRESULT DrawScreen(void );
HRESULT DrawBackground( void );
HRESULT DrawFooter( void );
HRESULT DrawHeader( void );
HRESULT ProcessInput( void );
HRESULT SetupMenusAndItems( void );
HRESULT XDashCfgCleanUp( void );
HRESULT XDashCfgInit( void );

// Menus are created here
CMenuScreen g_RootMenu             ( NULL,               L"Root",               L"Root Menu of the XDash Config Program" );

// Root Items
CMenuScreen g_ConfigMenu           ( &g_RootMenu,        L"Configuration",      L"Configure various items on the XBox" );
CMenuScreen g_RebootMenu           ( &g_RootMenu,        L"Reboot",             L"Reboot the XBox to various XDash entry points" );
CMenuScreen g_MemoryMenu           ( &g_RootMenu,        L"Memory Management",  L"Manage memory area of the XBox" );

// Config Items
CClockMenuScreen g_ClockManagement ( &g_ConfigMenu,      L"Clock Management",   L"Configure Time and Date settings on the XBox" );

// Reboot Menu Items
CRebootCommand g_RebootXbox        ( &g_RebootMenu,      L"Reboot XBox",        L"Reboots the XBox as if the user had pressed 'Power'" );

// Memory Management
CMAFormatCommand g_MAFormat        ( &g_ConfigMenu,      L"Format Memory Area", L"Formats a Memory Area on the XBox" );

// Other globals
CXBoxVideo Screen;                                              // Screen object where all text / info is displayed
USBManager  Controllers;                                        // Used to determine what buttons / direction the user is pressing
CXDCItem* g_pCurrentScreen = NULL;                              // Pointer to the current menu object

void __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
    DebugPrint( "***** XDashConfig(): wWinMain()\n" );

    XDashCfgInit();

    do
    {
        DrawScreen();
        ProcessInput();
    } while( !Controllers.IsButtonPressed( PORT_ANY, BUTTON_F ) );

    XDashCfgCleanUp();

    // Reboot the XBox
    DebugPrint( "***** XDashConfig(): Rebooting...\n" );
    XLaunchNewImage( NULL, NULL );

    DebugPrint( "***** XDashConfig(): wWinMain()\n" );
}

// Draw our screen
HRESULT DrawScreen( void )
{
    // DebugPrint( "  DrawScreen()\n" );

    Screen.ClearScreen();   // Clear the screen
    DrawBackground();       // Draw our background
    DrawHeader();           // Draw the screen header
    g_pCurrentScreen->Action( &Screen );
    DrawFooter();
    Screen.ShowScreen();    // Move the screen from the backbuffer to the foreground

    return S_OK;
}

// Draw the background of our application
HRESULT DrawBackground( void )
{
    // DebugPrint( "  DrawBackground()\n" );

    Screen.DrawBox( 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, (DWORD)BACKGROUND_COLOR );

    return S_OK;
}

// Initialize our application - video, sound, etc
HRESULT XDashCfgInit( void )
{
    DebugPrint( "  XDashCfgInit()\n" );

    // Setup Menus and Items
    SetupMenusAndItems();

    g_pCurrentScreen = &g_RootMenu;

    // Initialize our drawing routines (d3d, etc)
    Screen.Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );

    return S_OK;
}

// Clean up our application (close drivers, etc)
HRESULT XDashCfgCleanUp( void )
{
    DebugPrint( "  XDashCfgCleanUp()\n" );

    Screen.DeInitialize();

    return S_OK;
}

// Draw the header information on our screen
HRESULT DrawHeader( void )
{
    float fFontHeight = 0.0;
    float fHeaderTextYPos = 0.0;

    Screen.GetFontSize( 0, &fFontHeight, 0, 0 );
    fHeaderTextYPos = ( HEADER_BOTTOM_Y_POSITION / 2 ) - ( fFontHeight / 2 );

    // Fill the header background with the appropriate color
    Screen.DrawBox( 0, 0, SCREEN_WIDTH, HEADER_BOTTOM_Y_POSITION, HEADER_BG_COLOR );

    // Draw a header bar with the title and time
    Screen.DrawText( HEADER_TEXT_X_POSITION, fHeaderTextYPos, HEADER_TEXT_FG_COLOR, HEADER_TEXT_BG_COLOR, TITLE_NAME );
    Screen.DrawLine( 0, HEADER_LINE_Y_POSITION, SCREEN_WIDTH, HEADER_LINE_Y_POSITION, HEADER_LINE_WIDTH, HEADER_LINE_COLOR );

    SYSTEMTIME sysTime;
    ZeroMemory( &sysTime, sizeof( SYSTEMTIME ) );
    WCHAR timeString[255];

    GetSystemTime( &sysTime );
    wsprintf( timeString, L"%02d / %02d / %d  %02d : %02d : %02d", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond );
    Screen.DrawText( SCREEN_WIDTH - Screen.GetStringPixelWidth( timeString ) - MENU_VERT_SPACING, fHeaderTextYPos, HEADER_TEXT_FG_COLOR, HEADER_TEXT_BG_COLOR, timeString );

    return S_OK;
}

// Draw the footer information on our screen
HRESULT DrawFooter( void )
{
    float fFontHeight = 0.0;
    float fFooterTextYPos = 0.0;

    Screen.GetFontSize( 0, &fFontHeight, 0, 0 );
    fFooterTextYPos = ( FOOTER_TOP_Y_POSITION - ( ( SCREEN_HEIGHT - FOOTER_TOP_Y_POSITION ) / 2 ) ) - ( fFontHeight / 2 );

    // Fill the footer background with the appropriate color
    Screen.DrawBox( 0, FOOTER_TOP_Y_POSITION, SCREEN_WIDTH, SCREEN_HEIGHT, FOOTER_BG_COLOR );

    // Draw the description for the currently highlighted item
    Screen.DrawLine( 0, FOOTER_LINE_Y_POSITION, SCREEN_WIDTH, FOOTER_LINE_Y_POSITION, FOOTER_LINE_WIDTH, FOOTER_LINE_COLOR );
    if( g_pCurrentScreen->GetFooterText() )
    {
        Screen.DrawText( FOOTER_TEXT_X_POSITION, fFooterTextYPos, FOOTER_TEXT_FG_COLOR, FOOTER_TEXT_BG_COLOR, g_pCurrentScreen->GetFooterText() );
    }

    return S_OK;
}

/*
// Output the current memory usage to the Debugger
void DisplayMemStats( void )
{
    unsigned int pageSize = 4096;
    MM_STATISTICS mmStats;
    memset(&mmStats, 0, sizeof(MM_STATISTICS));
    mmStats.Length = sizeof(MM_STATISTICS);
    MmQueryStatistics(&mmStats);
    DebugPrint( "Memory Status----------------------------------------------------------------\r\n" );
    DebugPrint( "   Total:       %12lu B\r\n", mmStats.TotalPhysicalPages * pageSize );
    DebugPrint( "   Available:   %12lu B\r\n", mmStats.AvailablePages * pageSize );
    DebugPrint( "   Cache:       %12lu B\r\n", mmStats.CachePagesCommitted * pageSize );
    DebugPrint( "   Pool:        %12lu B\r\n", mmStats.PoolPagesCommitted * pageSize );
    DebugPrint( "   Stack:       %12lu B\r\n", mmStats.StackPagesCommitted * pageSize );
    DebugPrint( "   Image:       %12lu B\r\n", mmStats.ImagePagesCommitted * pageSize );
}
*/

// Handle input from the user
HRESULT ProcessInput( void )
{
    bool bButtonPressed;
    bool bFirstPress;

    // Check to see if anything was removed or inserted
    Controllers.CheckForHotplugs();

    // Handle the user making a selection with the A Button
    bFirstPress = Controllers.IsButtonPressed( PORT_ANY, BUTTON_A, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_A, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_pCurrentScreen->HandleInput( BUTTON_A, bFirstPress );
    }

    // Handle the user making a selection with the B Button
    bFirstPress = Controllers.IsButtonPressed( PORT_ANY, BUTTON_B, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_B, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_pCurrentScreen->HandleInput( BUTTON_B, bFirstPress );
    }


    // Handle the user pressing UP on the DPad
    bFirstPress = Controllers.IsDPadPressed( PORT_ANY, DPAD_TOP, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsDPadPressed( PORT_ANY, DPAD_TOP, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_pCurrentScreen->HandleInput( DPAD_TOP, bFirstPress );
    }


    // Handle the user pressing DOWN on the DPad
    bFirstPress = Controllers.IsDPadPressed( PORT_ANY, DPAD_BOTTOM, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsDPadPressed( PORT_ANY, DPAD_BOTTOM, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_pCurrentScreen->HandleInput( DPAD_BOTTOM, bFirstPress );
    }

    // Handle the user pressing LEFT on the DPad
    bFirstPress = Controllers.IsDPadPressed( PORT_ANY, DPAD_LEFT, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsDPadPressed( PORT_ANY, DPAD_LEFT, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_pCurrentScreen->HandleInput( DPAD_LEFT, bFirstPress );
    }

    // Handle the user pressing RIGHT on the DPad
    bFirstPress = Controllers.IsDPadPressed( PORT_ANY, DPAD_RIGHT, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsDPadPressed( PORT_ANY, DPAD_RIGHT, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_pCurrentScreen->HandleInput( DPAD_RIGHT, bFirstPress );
    }

    return S_OK;
}

// Setup our Menus and Items for our application
HRESULT SetupMenusAndItems( void )
{
    // Setup the Root Menu
    g_RootMenu.AddItem( &g_ConfigMenu );
    g_RootMenu.AddItem( &g_RebootMenu );
    g_RootMenu.AddItem( &g_MemoryMenu );

    // Setup the Configuration Menu
    g_ConfigMenu.AddItem( &g_ClockManagement );

    // Setup the Reboot Menu
    g_RebootMenu.AddItem( &g_RebootXbox );

    // Memory Management
    g_MemoryMenu.AddItem( &g_MAFormat );

    return S_OK;
};