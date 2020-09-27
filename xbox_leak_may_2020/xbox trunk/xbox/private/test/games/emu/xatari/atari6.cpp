// Atari6.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

HRESULT DrawScreen(void );
HRESULT DrawFooter( void );
HRESULT DrawHeader( void );
HRESULT ProcessInput( void );
HRESULT SetupMenusAndItems( void );

uInt8 *atariImage;

Console* theConsole;
Event theEvent;
uInt32 theDesiredFrameRate = 60;
Color *palette;

USBManager  Controllers;
CXBoxVideo  g_Screen;

bool   g_bBreakLoop     = false;    // Did the user select an item? (Used to get out of front-end loop)
WCHAR* g_pszCurrentItem = NULL;     // Pointer to the currently selected item

DWORD g_OrigStateBlock = 0L;        // Original State of D3D

static uInt32 theWidth;
static uInt32 theHeight;

// Root Menu
CMenuScreen g_RootMenu ( NULL, L"Root", L"Root Menu of the Program" );

void startup()
{
	// Get the desired width and height of the display
	theWidth = theConsole->mediaSource().width();
	theHeight = theConsole->mediaSource().height();
	
	int totalSize = theWidth * theHeight;
	atariImage = new uInt8[totalSize];
	for(int i = 0; i < totalSize; i++)
		atariImage[i] = 0x00;

	//setup color palette
	palette = new Color[256];
	const uInt32* atariPalette = theConsole->mediaSource().palette();
	for(uInt32 t = 0; t < 256; t++)
	{		
		palette[t].red = (BYTE)((atariPalette[t] & 0x00ff0000) >> 16) ;
		palette[t].green = (BYTE)((atariPalette[t] & 0x0000ff00) >> 8);
		palette[t].blue = (BYTE)(atariPalette[t] & 0x000000ff);
	}
}

void shutdown()
{


}

void updateDisplay(MediaSource& mediaSource)
{
	atariImage = (uInt8*)mediaSource.currentFrameBuffer();

}

bool color = true;
char left = 'A';
char right = 'A';

void handleEvents()
{
	Controllers.CheckForHotplugs();

    theEvent.set(Event::JoystickZeroFire, (0));
    theEvent.set(Event::JoystickZeroLeft, (0));
    theEvent.set(Event::JoystickZeroRight, (0));
    theEvent.set(Event::JoystickZeroUp, (0));
    theEvent.set(Event::JoystickZeroDown, (0));

	if( Controllers.IsButtonPressed( PORT_1, BUTTON_D, TRUE_ON_REPEAT ) )
		theEvent.set( Event::JoystickZeroFire, (1) );

	if( Controllers.IsControlPressed( PORT_1, CONTROL_DPAD_LEFT, TRUE_ON_REPEAT ) )
		theEvent.set( Event::JoystickZeroLeft, (1) );

	if( Controllers.IsControlPressed( PORT_1, CONTROL_DPAD_RIGHT, TRUE_ON_REPEAT ) )
		theEvent.set( Event::JoystickZeroRight, (1) );

	if( Controllers.IsControlPressed( PORT_1, CONTROL_DPAD_TOP, TRUE_ON_REPEAT ) )
		theEvent.set( Event::JoystickZeroUp, (1) );

	if( Controllers.IsControlPressed( PORT_1, CONTROL_DPAD_BOTTOM, TRUE_ON_REPEAT ) )
		theEvent.set( Event::JoystickZeroDown, (1) );

    theEvent.set(Event::JoystickOneFire, (0));
    theEvent.set(Event::JoystickOneLeft, (0));
    theEvent.set(Event::JoystickOneRight, (0));
    theEvent.set(Event::JoystickOneUp, (0));
    theEvent.set(Event::JoystickOneDown, (0));

	if( Controllers.IsButtonPressed( PORT_2, BUTTON_D, TRUE_ON_REPEAT ) )
		theEvent.set( Event::JoystickOneFire, (1) );

	if( Controllers.IsControlPressed( PORT_2, CONTROL_DPAD_LEFT, TRUE_ON_REPEAT ) )
		theEvent.set( Event::JoystickOneLeft, (1) );

	if( Controllers.IsControlPressed( PORT_2, CONTROL_DPAD_RIGHT, TRUE_ON_REPEAT ) )
		theEvent.set( Event::JoystickOneRight, (1) );

	if( Controllers.IsControlPressed( PORT_2, CONTROL_DPAD_TOP, TRUE_ON_REPEAT ) )
		theEvent.set( Event::JoystickOneUp, (1) );

	if( Controllers.IsControlPressed( PORT_2, CONTROL_DPAD_BOTTOM, TRUE_ON_REPEAT ) )
		theEvent.set( Event::JoystickOneDown, (1) );

    theEvent.set(Event::ConsoleSelect, (0));
    theEvent.set(Event::ConsoleReset, (0));
//    theEvent.set(Event::ConsoleColor, (0));
//    theEvent.set(Event::ConsoleBlackWhite, (0));
//    theEvent.set(Event::ConsoleLeftDifficultyA, (0));
//    theEvent.set(Event::ConsoleLeftDifficultyB, (0));
//    theEvent.set(Event::ConsoleRightDifficultyA, (0));
//    theEvent.set(Event::ConsoleRightDifficultyB, (0));

	if( Controllers.IsControlPressed( PORT_1, CONTROL_SELECT, FALSE_ON_REPEAT ) )
		theEvent.set( Event::ConsoleSelect, (1) );
	
	if( Controllers.IsControlPressed( PORT_1, CONTROL_START, FALSE_ON_REPEAT ) )
		theEvent.set( Event::ConsoleReset, (1) );
	
	if( Controllers.IsButtonPressed( PORT_1, BUTTON_C, FALSE_ON_REPEAT ) ) {
		if (color) {
			theEvent.set(Event::ConsoleBlackWhite, (1));
			theEvent.set(Event::ConsoleColor, (0));
			color = false;
		}
		else {
			theEvent.set(Event::ConsoleBlackWhite, (0));
			theEvent.set(Event::ConsoleColor, (1));
			color = true;
		}
	}


	if(Controllers.IsButtonPressed( PORT_1, BUTTON_E, FALSE_ON_REPEAT )) {
		if(left == 'B') {
			theEvent.set(Event::ConsoleLeftDifficultyA, (1));
			theEvent.set(Event::ConsoleLeftDifficultyB, (0));
			left = 'A';
		}
		else {
			theEvent.set(Event::ConsoleLeftDifficultyA, (0));
			theEvent.set(Event::ConsoleLeftDifficultyB, (1));
			left = 'B';
		}
	}


	if(Controllers.IsButtonPressed( PORT_1, BUTTON_F, FALSE_ON_REPEAT )) {
		if(right == 'B') {
			theEvent.set(Event::ConsoleRightDifficultyA, (1));
			theEvent.set(Event::ConsoleRightDifficultyB, (0));
			right = 'A';
		}
		else {
			theEvent.set(Event::ConsoleRightDifficultyA, (0));
			theEvent.set(Event::ConsoleRightDifficultyB, (1));
			right = 'B';
		}
	}

/*
  	//maybe paddle resistance range from 2000 - 1000000
    theEvent.set(Event::PaddleZeroResistance, resistance);
    theEvent.set(Event::PaddleZeroFire, (0));
    theEvent.set(Event::PaddleOneResistance, resistance);
    theEvent.set(Event::PaddleOneFire, (0));
    theEvent.set(Event::PaddleTwoResistance, resistance);
    theEvent.set(Event::PaddleTwoFire, (0));
    theEvent.set(Event::PaddleThreeResistance, resistance);
    theEvent.set(Event::PaddleThreeFire, (0));

*/
	
}

void setupProperties(PropertiesSet& set)
{
  // Try to load the file a2600.dat file
/*  string filename = "t:\\a2600.dat";

  // See if we can open the file
  ifstream2 stream(filename.c_str()); 
  if(!stream.eof())
  {
    // File was opened so load properties from it
    set.load(stream, &Console::defaultProperties());
  }
  else
  {
     //Couldn't open the file
	  exit(1);
  }  */
}

SoundXBOX sound;
PropertiesSet propertiesSet("Cartridge.Name");
uInt32 size;
const char* filename;

int atari()
{
    int counter = 0;
  //  char file[MAX_PATH];
  //  ZeroMemory( file, MAX_PATH );

    // Get a pointer to the file which contains the cartridge ROM
     const char* file = "d:\\default.bin";
  //  sprintf( file, "%S\\%S.%S", GAME_PATH, g_pszCurrentItem, ROM_EXT );

    DebugPrint( "atari():file = '%s'\n", file );

    // Open the cartridge image and read it in
    ifstream2 in;
    //in.open(file, ios::in | ios::nocreate | ios::binary); 
    in.open(file);
    if(in.eof())
    {
        DebugPrint( "atari():Unexpected EOF found.\n" );
        exit(1);
    }

    uInt8* image = new uInt8[512 * 1024];
    in.read(image, 512 * 1024);
    //  uInt32 size = in.gcount();
    size = in.gcount();
    in.close();

    //  PropertiesSet propertiesSet("Cartridge.Name");
    setupProperties(propertiesSet);

    //  Sound tempsound;

    // Get just the filename of the file containing the ROM image
    //  const char* filename = (!strrchr(file, '\\')) ? file : strrchr(file, '\\') + 1;
    // sprintf( filename, "%S.%S", g_pszCurrentItem, ROM_EXT );
    filename = (!strrchr(file, '\\')) ? file : strrchr(file, '\\') + 1;

    DebugPrint( "atari():filename = '%s'\n", filename );

    // Create the 2600 game console
    theConsole = new Console(image, size, filename, theEvent, propertiesSet, sound);

    // Free the image since we don't need it any longer
    delete[] image;

    startup();

    return 0;
}

void render(void) {
	int numFrames = 0;

    // Ask the media source to prepare the next frame
    theConsole->mediaSource().update();

    // Update the display and handle events
    updateDisplay(theConsole->mediaSource());
    handleEvents();
}


// Windows Message handling procedure
long FAR PASCAL WndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam );
HRESULT Initialize( HWND hwnd );
void InitializeObjects(void);
IDirect3DDevice8* pD3DDevice;
DWORD dwFvf = D3DFVF_XYZ  | D3DFVF_NORMAL | D3DFVF_DIFFUSE |
              D3DFVF_TEX0 | D3DFVF_TEXCOORDSIZE2(0);
Sprite3D newsprite;

void GfxInitialize()
{
	if (pD3DDevice == NULL)
	{
		// Create D3D 8.
		IDirect3D8 *pD3D = Direct3DCreate8( D3D_SDK_VERSION );
		
        if ( !pD3D )
        {
            DebugPrint( "Failed to create pD3D!\n" );
			return;
        }

		// Set the screen mode.
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory( &d3dpp, sizeof( d3dpp ) );

		d3dpp.BackBufferWidth            = SCREEN_WIDTH;
		d3dpp.BackBufferHeight           = SCREEN_HEIGHT;
		d3dpp.BackBufferFormat           = D3DFMT_X8R8G8B8;
		d3dpp.BackBufferCount            = 1;
		d3dpp.Windowed                   = false;    // Must be false for Xbox
		d3dpp.EnableAutoDepthStencil     = true;
        //d3dpp.AutoDepthStencilFormat     = D3DFMT_D24X8;
        d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
		d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
		d3dpp.FullScreen_RefreshRateInHz = 60;	//or 72?
		d3dpp.hDeviceWindow              = NULL;   // Ignored on Xbox

		// Create the m_Device.
		if ( S_OK != pD3D->CreateDevice( D3DADAPTER_DEFAULT,
                                         D3DDEVTYPE_HAL,
                                         NULL,                                 // Ignored on Xbox
                                         D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                         &d3dpp,
                                         &pD3DDevice ) )
		{
            DebugPrint( "Failed to CreateDevice!\n" );
			pD3D->Release();
			return;
		}

		// Set the stream source for our vertex buffer
		pD3DDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
		
		D3DVIEWPORT8 D3DViewport;
		D3DViewport.X      = 0;
		D3DViewport.Y      = 0;
		D3DViewport.Width  = SCREEN_WIDTH;
		D3DViewport.Height = SCREEN_HEIGHT;
		D3DViewport.MinZ   = 0.0f;
		D3DViewport.MaxZ   = 1.0f;
		if( FAILED( pD3DDevice->SetViewport( &D3DViewport ) ) )
		{
			DebugPrint( "Couldn't set the viewpoint\n" );
		}
		
		// Release the D3D Object
		if( pD3D )
		{
			pD3D->Release();
			pD3D = NULL;
		}
	}

}

D3DRASTER_STATUS vsyncCheck;

void __cdecl main()
{
	DWORD timer = GetTickCount();
	DWORD fpstimer = GetTickCount();
	unsigned int fps=0;

 	// Initalize sound
	sound.Initialize();

	
	// Initialize our Screen
    GfxInitialize();

    // Storing the original state of D3D
    if( FAILED( pD3DDevice->CreateStateBlock( D3DSBT_ALL, &g_OrigStateBlock ) ) )
    {
        DebugPrint( "CXBoxVideo::Initialize():pD3DDevice->CreateStateBlock( g_OrigStateBlock ) Failed!!\n" );
    }

	// atari();
	//InitializeObjects();
    SetupMenusAndItems();
    
    // Screen object for our menu front-end
    g_Screen.Initialize( pD3DDevice );

    do
    {
	//	sound.mute( true );

        // Apply the D3D State needed for the Front End
        DebugPrint( "main():pD3DDevice->ApplyStateBlock( g_Screen.GetState() )\n" );
        if( FAILED( pD3DDevice->ApplyStateBlock( g_Screen.GetState() ) ) )
        {
            DebugPrint( "main():pD3DDevice->ApplyStateBlock( g_Screen.GetState() ) Failed!!\n" );
        }

      /*  do
        {
            ProcessInput();
            DrawScreen();
        } while( !g_bBreakLoop );
        g_bBreakLoop = false;
    */
        DebugPrint( "main():atari()\n" );

	//	sound.mute( false );
        atari();
        
        // Apply the D3D State needed for Atari
        DebugPrint( "main():pD3DDevice->ApplyStateBlock( g_OrigStateBlock )\n" );
        if( FAILED( pD3DDevice->ApplyStateBlock( g_OrigStateBlock ) ) )
        {
            DebugPrint( "main():pD3DDevice->ApplyStateBlock( g_OrigStateBlock ) Failed!!\n" );
        }

        DebugPrint( "main():InitializeObjects()\n" );
        InitializeObjects();

        do
        {
		    pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
            pD3DDevice->BeginScene();

            // Rendering of scene objects happens here.
		    render();
		    newsprite.drawSprite( pD3DDevice, atariImage );

            pD3DDevice->EndScene();
/*
			//frame delay code
			do {
				pD3DDevice->GetRasterStatus(&vsyncCheck);
			} while (!vsyncCheck.InVBlank);
			do {
				pD3DDevice->GetRasterStatus(&vsyncCheck);
			} while (vsyncCheck.InVBlank);
*/
			pD3DDevice->Present( NULL, NULL, NULL, NULL );

        } while( !( Controllers.IsControlPressed( PORT_ANY, CONTROL_TRIGGER_LEFT ) && Controllers.IsControlPressed( PORT_ANY, CONTROL_TRIGGER_RIGHT ) ) );
	
    } while( 1 );
};

void InitializeObjects(void) {
	newsprite.initialize(pD3DDevice, theWidth, theHeight, palette);
}


// Draw the header information on our screen
HRESULT DrawHeader( void )
{
    float fFontHeight = 0.0;
    float fHeaderTextYPos = 0.0;

    g_Screen.GetFontSize( 0, &fFontHeight, 0, 0 );
    fHeaderTextYPos = ( HEADER_BOTTOM_Y_POSITION / 2 ) - ( fFontHeight / 2 );

    // Fill the header background with the appropriate color
    g_Screen.DrawBox( 0, 0, SCREEN_WIDTH, HEADER_BOTTOM_Y_POSITION, HEADER_BG_COLOR );

    // Draw a line along the bottom of the background box
    g_Screen.DrawLine( 0, HEADER_LINE_Y_POSITION, SCREEN_WIDTH, HEADER_LINE_Y_POSITION, HEADER_LINE_WIDTH, HEADER_LINE_COLOR );

    // Put the Application Title on the Header Bar
    g_Screen.DrawText( HEADER_TEXT_X_POSITION, fHeaderTextYPos, HEADER_TEXT_FG_COLOR, HEADER_TEXT_BG_COLOR, TITLE_NAME );

    SYSTEMTIME sysTime;
    ZeroMemory( &sysTime, sizeof( SYSTEMTIME ) );
    WCHAR timeString[255];

    GetSystemTime( &sysTime );
    wsprintf( timeString, L"%02d / %02d / %d  %02d : %02d : %02d", sysTime.wMonth, sysTime.wDay, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond );

    // Put the system time and date on the header
    g_Screen.DrawText( SCREEN_WIDTH - g_Screen.GetStringPixelWidth( timeString ) - MENU_VERT_SPACING, fHeaderTextYPos, HEADER_TEXT_FG_COLOR, HEADER_TEXT_BG_COLOR, timeString );

    return S_OK;
}


// Draw the footer information on our screen
HRESULT DrawFooter( void )
{
    float fFontHeight = 0.0;
    float fFooterTextYPos = 0.0;

    g_Screen.GetFontSize( 0, &fFontHeight, 0, 0 );
    fFooterTextYPos = ( FOOTER_TOP_Y_POSITION - ( ( SCREEN_HEIGHT - FOOTER_TOP_Y_POSITION ) / 2 ) ) - ( fFontHeight / 2 );

    // Fill the footer background with the appropriate color
    g_Screen.DrawBox( 0, FOOTER_TOP_Y_POSITION, SCREEN_WIDTH, SCREEN_HEIGHT, FOOTER_BG_COLOR );

    // Draw the description for the currently highlighted item
    g_Screen.DrawLine( 0, FOOTER_LINE_Y_POSITION, SCREEN_WIDTH, FOOTER_LINE_Y_POSITION, FOOTER_LINE_WIDTH, FOOTER_LINE_COLOR );

    if( g_RootMenu.GetFooterText() )
    {
        g_Screen.DrawText( SCREEN_X_CENTER - ( g_Screen.GetStringPixelWidth( g_RootMenu.GetFooterText() ) / 2.0f ), fFooterTextYPos, FOOTER_TEXT_FG_COLOR, FOOTER_TEXT_BG_COLOR, g_RootMenu.GetFooterText() );
    }

    return S_OK;
}


// Draw our screen
HRESULT DrawScreen( void )
{
    g_Screen.ClearScreen( 0xFF000000 );     // Clear the screen
    g_RootMenu.Action( &g_Screen );			// Current menu action
    DrawHeader();                           // Draw the screen header
    DrawFooter();                           // Draw our footer
    g_Screen.ShowScreen();                  // Move the screen from the backbuffer to the foreground

    return S_OK;
}

// Setup our Menus and Items for our application
HRESULT SetupMenusAndItems( void )
{
    // Setup the Root Menu

	// TODO: Add all the items in the directory to our menu

	CHAR szWild [MAX_PATH];
	CHAR* pszCurrentFile;
	WIN32_FIND_DATA fd;
	HANDLE h;

	strcpy( szWild, GAME_PATH_A );
	strcat( szWild, "\\*." ROM_EXT_A );

	h = FindFirstFile( szWild, &fd );

	if (h == INVALID_HANDLE_VALUE)
	{
		DebugPrint( "Cannot read directory: '%s' (%d)\n", GAME_PATH, GetLastError() );
		return E_FAIL;
	}

	do
	{
		if ( fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM )
		{
			continue;
		}

		if ( fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )
		{
			continue;
		}

		if ( fd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY )
		{
			continue;
		}

		if ( fd.cFileName[0] == '.' )
		{
			continue;
		}

		if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			continue;
		}

		unsigned int strLen = strlen( fd.cFileName ) - 3; // Removing the extention of the filename
		pszCurrentFile = new CHAR[strLen];
		
		strncpy( pszCurrentFile, fd.cFileName, strlen( fd.cFileName ) - 4 ); // Removing the file extension and '.'
		pszCurrentFile[strLen - 1] = '\0';

		// Add the filename to our menu

		WCHAR* pwsz = new WCHAR[strlen( pszCurrentFile ) + 1];
		swprintf( pwsz, L"%S", pszCurrentFile );

		g_RootMenu.AddItem( pwsz );
	}
	while ( FindNextFile( h, &fd ) );

    g_RootMenu.SetFooterText( FOOTER_TEXT );

    return S_OK;
};

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
        g_RootMenu.HandleInput( BUTTON_A, bFirstPress );
    }

    // Handle the user making a selection with the B Button
    bFirstPress = Controllers.IsButtonPressed( PORT_ANY, BUTTON_B, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_B, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( BUTTON_B, bFirstPress );
    }

    // Handle the user making a selection with the C Button
    bFirstPress = Controllers.IsButtonPressed( PORT_ANY, BUTTON_C, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_C, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( BUTTON_C, bFirstPress );
    }

    // Handle the user making a selection with the D Button
    bFirstPress = Controllers.IsButtonPressed( PORT_ANY, BUTTON_D, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_D, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( BUTTON_D, bFirstPress );
    }

    // Handle the user making a selection with the E Button
    bFirstPress = Controllers.IsButtonPressed( PORT_ANY, BUTTON_E, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_E, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( BUTTON_E, bFirstPress );
    }

    // Handle the user making a selection with the F Button
    bFirstPress = Controllers.IsButtonPressed( PORT_ANY, BUTTON_F, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_F, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( BUTTON_F, bFirstPress );
    }

    // Handle the user making a selection with the 7 Button
    bFirstPress = Controllers.IsButtonPressed( PORT_ANY, BUTTON_7, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_7, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( BUTTON_7, bFirstPress );
    }

    // Handle the user making a selection with the 8 Button
    bFirstPress = Controllers.IsButtonPressed( PORT_ANY, BUTTON_8, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_8, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( BUTTON_8, bFirstPress );
    }

    // Handle the user making a selection with the 9 Button
    bFirstPress = Controllers.IsButtonPressed( PORT_ANY, BUTTON_9, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_9, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( BUTTON_9, bFirstPress );
    }

    // Handle the user making a selection with the 10 Button
    bFirstPress = Controllers.IsButtonPressed( PORT_ANY, BUTTON_10, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_10, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( BUTTON_10, bFirstPress );
    }

    // Handle the user pressing UP on the DPad
    bFirstPress = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_TOP, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_TOP, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( CONTROL_DPAD_TOP, bFirstPress );
    }


    // Handle the user pressing DOWN on the DPad
    bFirstPress = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_BOTTOM, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_BOTTOM, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( CONTROL_DPAD_BOTTOM, bFirstPress );
    }

    // Handle the user pressing LEFT on the DPad
    bFirstPress = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_LEFT, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_LEFT, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( CONTROL_DPAD_LEFT, bFirstPress );
    }

    // Handle the user pressing RIGHT on the DPad
    bFirstPress = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_RIGHT, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_RIGHT, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( CONTROL_DPAD_RIGHT, bFirstPress );
    }

    // Handle the user pressing START button
    bFirstPress = Controllers.IsControlPressed( PORT_ANY, CONTROL_START, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_START, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( CONTROL_START, bFirstPress );
    }

    // Handle the user pressing SELECT button
    bFirstPress = Controllers.IsControlPressed( PORT_ANY, CONTROL_SELECT, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_SELECT, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( CONTROL_SELECT, bFirstPress );
    }

    // Handle the user pressing the Left Trigger button
    bFirstPress = Controllers.IsControlPressed( PORT_ANY, CONTROL_TRIGGER_LEFT, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_TRIGGER_LEFT, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( CONTROL_TRIGGER_LEFT, bFirstPress );
    }

    // Handle the user pressing the Right Trigger button
    bFirstPress = Controllers.IsControlPressed( PORT_ANY, CONTROL_TRIGGER_RIGHT, FALSE_ON_REPEAT );
    bButtonPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_TRIGGER_RIGHT, TRUE_ON_REPEAT );
    if( bButtonPressed )
    {
        g_RootMenu.HandleInput( CONTROL_TRIGGER_RIGHT, bFirstPress );
    }

    return S_OK;
}