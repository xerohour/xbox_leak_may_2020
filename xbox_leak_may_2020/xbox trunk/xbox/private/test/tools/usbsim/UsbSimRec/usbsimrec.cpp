/*************************************************************************************************

  Copyright (c) 2001 Microsoft Corporation

  Module Name:

	usbsimrec.cpp

  Abstract:

	Defines the USB Simulator Recorder functions which allow real time USB interaction and
	logging for future playback.

  Author:

	Mabel Santos (t-msanto) 25-May-2001

  Revision History:

*************************************************************************************************/

#include "usbsimrec.h"

DWORD DebugOn;
LONG MaxDrift;
DWORD ScriptType;
const DWORD BuffLen = 1024;
char *SimIp = new char[BuffLen];
char *FileName = new char[BuffLen];
STATE_INFO DukeStates[NUM_PORTS];
XINPUT_STATE CurrState;
USBSimulator *UsbSim;
Script *ScriptFile;

/*************************************************************************************************

Routine Description:

	StringTrim

	Extracts a value from a line in the testini.ini file.  Trims off spaces, tabs, and comments.

Arguments:

	char *String				String to be trimmed.

Return Value:

	none

*************************************************************************************************/

void StringTrim( char *String )
{
	DWORD Pos;

	// Find the first occurrence of a whitespace character
	Pos = strcspn( String, " \t" );

	// Set first whitespace character and all trailing characters to NULL
	memset( &String[Pos], '\0', BuffLen - Pos );
}

/*************************************************************************************************

Routine Description:

	DebugOutput
 
	Displays debug output statements when the DebugOn flag is set.

Arguments:

	LPTSTR ptszFormat,...		Format string to be displayed.

Return Value:

	none

*************************************************************************************************/

void DebugOutput( char *ptszFormat, ...)
{
	// Is the DebugOn flag set?
	if( DebugOn )
	{
		char tszBuf[MAX_PATH*2];
		va_list arglist;

	    va_start( arglist, ptszFormat );
		_vsnprintf( tszBuf, sizeof(tszBuf), ptszFormat, arglist );
		OutputDebugStringA( tszBuf );
		va_end(arglist);
	}
}

/*************************************************************************************************

Routine Description:

	main function

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void __cdecl main( void )
{
	XNetStartupParams xnsp = { sizeof(XNetStartupParams), XNET_STARTUP_BYPASS_SECURITY };
    int nXnetResult = XNetStartup( &xnsp );
    if(nXnetResult != NO_ERROR)
    {
		DebugOutput("XNetStartup() Error: %u\n", nXnetResult);
    }
	// Sleep for 5 secs because sockets are not fully initialized immediately after call to
	// XNetStartup()
    Sleep(5000);

	UsbSim = new USBSimulator;

	DebugOutput( "Getting INI file settings\r\n" );
    // Get INI file settings

	// Default value for DebugOn = 0
	DebugOn = GetProfileIntA( "Settings", "DebugOn", 0 );
	if( DebugOn < 0 || DebugOn > 1 ) { DebugOn = 0; }

	// Default value for MaxDrift = 5000
	MaxDrift = GetProfileIntA( "Settings", "MaxDrift", 5000 );
	if( MaxDrift < 5000 ) { MaxDrift = 5000; }

	// Default value for ScriptType = 1 (JScript)
	ScriptType = GetProfileIntA( "Settings", "ScriptType", 1 );
	if( ScriptType != 1 ) { ScriptType = 1; }

	// Default value for SimIp = "0.0.0.0" 
	GetProfileStringA( "Settings", "SimIp", "0.0.0.0", SimIp, BuffLen );
	StringTrim( SimIp );	

	// Default value for FileName = "U:\script.js"
	GetProfileStringA( "Settings", "FileName", "U:\\script.js", FileName, BuffLen );
	StringTrim( FileName );
	if( ( memcmp( (void*) FileName, "U:\\", 3 ) != 0 ) && 
		( memcmp( (void*) FileName, "T:\\", 3 ) != 0 ) ) { FileName = "U:\\script.js"; }

	//DebugOutput( "Calling InitGamepads()\r\n" );
	InitGamepads();

	while( TRUE )
	{
		//DebugOutput( "Calling DetectChanges()\r\n" );
		DetectChanges();
		//DebugOutput( "Calling ReadGamepads()\r\n" );
		ReadGamepads();
	}
}


