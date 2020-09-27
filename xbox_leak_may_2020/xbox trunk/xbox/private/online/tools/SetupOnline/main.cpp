#include <xtl.h>
#include <xdbg.h>
#include "UserInterface.h"
#include "UserInterfaceScreen.h"
#include "UserInterfaceMenu.h"
#include "UserInterfaceText.h"
#include "PingScreen.h"
#include "AccountsScreen.h"
#include "CreationStateScreen.h"
#include "ExitScreen.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: main
//
// Input: None
//
// Output: None
//
// Description: the real work horse. set up the UI, Renders the UI, 
//              gets input for the UI, and updates the UI
//
///////////////////////////////////////////////////////////////////////////////

void __cdecl main( void )
{
	HRESULT	hr = S_OK;
	int iRet = 0;

	WSADATA wsaData;
	XNetStartupParams xnsp;

	XNADDR xnaddr;
	DWORD dwResult = 0L;

	//
	// zero out the winsock data
	//

	ZeroMemory( &wsaData, sizeof( WSADATA ) );
	ZeroMemory( &xnsp, sizeof( XNetStartupParams ) );

	//
	// Setup net stack
	//

	if ( SUCCEEDED( hr ) )
	{
		//
		// we want to disable secure communication
		//

		xnsp.cfgSizeOfStruct = sizeof( XNetStartupParams );
		xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;

		hr = XNetStartup( &xnsp );
		ASSERT( SUCCEEDED( hr ) || !"Could not start up network" );
	}

	//
	// wait for DNS to succeed
	//
 
	do
	{
		dwResult = XNetGetTitleXnAddr ( &xnaddr );  
	} 
	while ( XNET_GET_XNADDR_PENDING == dwResult );

	//
	// setup online
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = XOnlineStartup( NULL );
		ASSERT( SUCCEEDED( hr ) || !"Could not startup online" );
	}

	//
	// create the screens
	//

	CUserInterfaceScreen mainScreen( NULL, 
		                             0xFFFFFFFF, 
									 0x00000000, 
									 320, 
									 250 );

	CPingScreen pingScreen( &mainScreen );
	CAccountsScreen accountsScreen( &mainScreen );
	CCreationStateScreen creationStateScreen( &mainScreen );
	CExitScreen exitScreen( &mainScreen );

	//
	// create the menus
	//

	CUserInterfaceMenu mainMenu( NULL, 0xFFFFFFFF, 0x00000000, 320, 250 );

	//
	// create the text
	//

	CUserInterfaceText aText( "(A) Select", 500, 400, 0xFF00FF00, 0xFF000000 );

	//
	// add items to the menus
	//

	mainMenu.AddItem( "Ping PartnerNet", &pingScreen );
	mainMenu.AddItem( "Create User Accounts", &creationStateScreen );
	mainMenu.AddItem( "Manage User Accounts", &accountsScreen );
	mainMenu.AddItem( "Exit SetupOnline", &exitScreen );

	//
	// add menus to screens
	//

	mainScreen.AddMenu( &mainMenu );

	//
	// add ui text ot screens
	//

	mainScreen.AddUIText( &aText );

	//
	// Setup the UI
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = CUserInterface::Instance()->Initialize( &mainScreen,
													 640, 
													 480, 
													 0,
													 "d:\\media\\images\\back.bmp" );

		ASSERT( SUCCEEDED( hr ) || !"Could not setup user interface" );
	}

	//
	// Begin the app loop
	//

	while ( SUCCEEDED( hr ) )
	{
		//
		// Update the scene
		//

		hr = CUserInterface::Instance()->Update();
		ASSERT( SUCCEEDED( hr ) || !"Could not update the scene" );
	}

	//
	// Clean up the net stack
	//

	hr = XNetCleanup();
	ASSERT( SUCCEEDED( hr ) || !"Could not release network" );

	hr = XOnlineCleanup();
	ASSERT( SUCCEEDED( hr ) || !"Could not release online" );
}