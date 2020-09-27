#include <xtl.h>
#include <xdbg.h>
#include <xonline.h>
#include <UserInterface.h>

char* GetBoxInformation( void );

void __cdecl main( void )
{
	HRESULT hr = S_OK;

	char* szBoxInfo = NULL;
	char* szTemp = NULL;
	char* szStr = NULL;

	DWORD dwY = 30;

	//
	// create the screens
	//

	CUserInterfaceScreen mainScreen( NULL, 
		                             0xFFFFFFFF, 
									 0x00000000, 
									 320, 
									 250 );

	//
	// setup online
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = XOnlineStartup( NULL );
		ASSERT( SUCCEEDED( hr ) || !"Could not startup online" );
	}

	//
	// create user interface
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = CUserInterface::Instance()->Initialize( &mainScreen,
													 640, 
													 480, 
													 0,
													 NULL );

		ASSERT( SUCCEEDED( hr ) || !"Could not setup user interface" );
	}

	//
	// set empty text
	//

	mainScreen.SetText( "" );

	//
	// get the box info
	//

	szBoxInfo = GetBoxInformation();

	//
	// make sure box info is good
	//

	if ( NULL == szBoxInfo )
	{
		ASSERT( !"Could not get box information" );
		hr = E_OUTOFMEMORY;
	}

	//
	// get the first string
	//

	if ( SUCCEEDED( hr ) )
	{
		szStr = szBoxInfo;
		szTemp = strstr( szBoxInfo, "\n" );
	}

	//
	// keep cycling through strings, adding them to the ui
	//

	while ( NULL != szTemp && SUCCEEDED( hr ) )
	{
		//
		// convert next return to null char
		//

		*szTemp = '\0';

		//
		// add the string to the ui text, move down one line
		//

		mainScreen.AddUIText( new CUserInterfaceText( szStr, 320, dwY, 0xFFFFFFFF, 0x00000000 ) );
		dwY += 20;

		//
		// point to the beginning of the next string
		//

		szStr = szTemp + 1;

		//
		// find the next line feed
		//

		szTemp = strstr( szTemp + 1, "\n" );

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
	// kill the box info
	//

	delete [] szBoxInfo;

	//
	// kill online
	//

	XOnlineCleanup();
}
