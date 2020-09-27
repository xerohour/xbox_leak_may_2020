

//  Xbox app that cycles box power
//  by using the SMC power cycle command.
//





#include <xtl.h>
#include <dxconio.h>
#include <i2clib.h>
#include <xlog.h>
#include <xbgpinput.h>


#define FILENAME "t:\\reboots.txt"




DWORD ReadFromFile( LPSTR lpFilename )
{
	HANDLE hFile;
	DWORD result;
	DWORD dwBytesRead;

	// open the file for reading
	hFile = CreateFile( "t:\\reboots.txt", 
							GENERIC_READ, 
							0,
							NULL, 
							OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL );


	// set reboots to 0 if file doesn't exist,
	// otherwise, read reboots from the file
	if( INVALID_HANDLE_VALUE == hFile )
	{
		result = 0;
	}
	else
	{
		// get current # of reboots
		ReadFile( hFile, (LPVOID)(&result), 4, &dwBytesRead, NULL );
	}


	// close the file
	CloseHandle( hFile );
	hFile = INVALID_HANDLE_VALUE;

	return result;
}





VOID WriteToFile( LPSTR lpFilename, DWORD value )
{
	HANDLE hFile;
	DWORD dwBytesWritten;

	// open the file for writing
	hFile = CreateFile(		lpFilename, 
							GENERIC_WRITE, 
							0, 
							NULL,
							CREATE_ALWAYS, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL );


	WriteFile( hFile, (LPCVOID)(&value), 4, &dwBytesWritten, NULL );
	

	// close the file
	CloseHandle( hFile );
	hFile = INVALID_HANDLE_VALUE;

}







void __cdecl main()
{

	HANDLE hLog;
	DWORD dwPressedButtons = 0;
	DWORD dwPressedBlackButton = 0;
	XBGAMEPAD* pGamepads = NULL;
	DWORD dwReboots = 0;
	
	// create console
	xCreateConsole( NULL );
	xSetBackgroundImage( NULL );
	xSetFont( 15.0, 18.0, NULL );

	// create gamepads
	XBGPInput_InitGamepads();
	XBGPInput_CreateGamepads( &pGamepads );

	
	
	// create log
	hLog = xCreateLog( "t:\\SMCPowerCycle.log", NULL, INVALID_SOCKET, XLL_PASS, XLO_DEBUG );
	if( !hLog )
	{
		printf( "  Could not create Log\n");
		Sleep( INFINITE );
	}


	// get (& increment) the current # of reboots
	dwReboots = ReadFromFile( FILENAME );
	dwReboots++;
	WriteToFile( FILENAME, dwReboots );



	// log the reboot
	xSetOwnerAlias( hLog, "" );
	xSetComponent( hLog, "", "" );
	xSetFunctionName( hLog, "" );
	xStartVariation( hLog, "" );
	if( !xLog( hLog, XLL_PASS, "%d reboots", dwReboots ) )
	{
		printf( "  Could not write to log\n" );
		Sleep( INFINITE );
	}


	// check for Gamepad input.
	// don't reboot if there is gamepad input
	Sleep( 500 );
	XBGPInput_GetInput( pGamepads );
	for( int i = 0; i < 4; i++ )
	{
		if( pGamepads[i].hDevice )
		{
			dwPressedButtons = dwPressedButtons | pGamepads[i].wPressedButtons;
			dwPressedButtons = dwPressedButtons | pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_X ];
			dwPressedButtons = dwPressedButtons | pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_Y ];
			dwPressedButtons = dwPressedButtons | pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_A ];
			dwPressedButtons = dwPressedButtons | pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_B ];
			dwPressedButtons = dwPressedButtons | pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_BLACK ];
			dwPressedButtons = dwPressedButtons | pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_WHITE ];

			dwPressedBlackButton = pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_BLACK ];
		}
	}

	// if gamepad input, don't reboot, delete the reboot file
	if( dwPressedButtons )
	{
		printf("  Controller input detected\n");

		// reset count if black button was pressed
		if( dwPressedBlackButton )
		{
			printf("  Black button pressed\n");
			printf("  Resetting reboot count\n" );
			DeleteFile( FILENAME );
		}
		printf("\n  Stopped at %d reboots\n", dwReboots);
		printf("  Sleeping ..." );
		Sleep( INFINITE );
	}


	printf("  Reboot# %d\n", dwReboots );
	printf("  Initiating SMC \n  Power Cycle...");
	Sleep( 500 );
	
	

	WriteSMC( 0x02, 0x40 );


	Sleep( INFINITE );

}

