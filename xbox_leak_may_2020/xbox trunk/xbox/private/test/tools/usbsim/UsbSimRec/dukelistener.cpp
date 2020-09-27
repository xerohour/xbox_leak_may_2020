/*************************************************************************************************

  Copyright (c) 2001 Microsoft Corporation

  Module Name:

	dukelistener.cpp

  Abstract:

	Defines the USB Simulator Recorder functions which talk to USB devices and pass input events
	to the SimMessenger and ScriptWriter

  Author:

	Mabel Santos (t-msanto) 25-May-2001

  Revision History:

*************************************************************************************************/

#include "usbsimrec.h"

DWORD ConnectedDukes = 0;
LARGE_INTEGER Frequency;
XINPUT_GAMEPAD BaseState = {0};
XDEVICE_PREALLOC_TYPE Dukes[] = { { XDEVICE_TYPE_GAMEPAD, NUM_PORTS } };

/*************************************************************************************************

Routine Description:

	InitGamepads
	
	Detects controllers connected at startup.  Calls DukeInsert(), DukeRemove(), SimInit(), 
	SimInsert(), SimRemove(), Script(), and ScriptInsert() to initialize the duke, simulator,
	and script data structures.
	
Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void InitGamepads( void )
{
	DWORD Port;
	
	DebugOutput( "Calling SimInit()\r\n" );

	// Initialize simulator stuff
	SimInit();

	switch( ScriptType )
	{

	case 1:
		DebugOutput( "Calling JScript() constructor\r\n" );
		ScriptFile = new JScript();
		break;

	default:
		DebugOutput( "InitGamepads() Error: Unknown script type\r\n" );
		DebugOutput( "Please specify a valid script type in the .INI file and restart the program\r\n" );
		return;
	
	// Add other script type cases here
	}

	DebugOutput( "Initializing peripheral control software\r\n" );

	// Initialize peripheral control software
	XInitDevices( sizeof( Dukes ) / sizeof( XDEVICE_PREALLOC_TYPE ), Dukes );
	//XInitDevices( 1, Dukes );
	DebugOutput( "Determining which ports have connected controllers\r\n" );

	// Determine which ports have connected controllers
	ConnectedDukes = XGetDevices( XDEVICE_TYPE_GAMEPAD );

	DebugOutput( "Getting performance counter frequency\r\n" );

	// Get the performance counter frequency
	QueryPerformanceFrequency( &Frequency );

	DebugOutput( "Converting frequency to ticks/millisecond\r\n" );

	// Convert frequency from ticks/second to ticks/millisecond
	Frequency.QuadPart /= 1000;

	DebugOutput( "Setting information for each port\r\n" );

	// Set information for each port
	for( Port = 0; Port < NUM_PORTS; ++Port )
	{
		// Is there a controller connected to the port?
		if( 1 << Port & ConnectedDukes )
		{	
			DebugOutput( "Port %u: Found a controller\r\n", Port+1 );

			DebugOutput( "Port %u: Calling DukeInsert()\r\n", Port+1 );
			DukeInsert( Port );

			DebugOutput( "Port %u: Calling SimInsert()\r\n", Port+1 );
			// Pass information along to the simulator
			SimInsert( Port );

			DebugOutput( "Port %u: Calling ScriptInsert()\r\n", Port+1 );
			// Pass information along to the script
			ScriptFile->ScriptInsert( Port );
		}
		else
		{	
			DebugOutput( "Port %u: Did not find a controller\r\n", Port+1 );

			DebugOutput( "Port %u: Calling DukeRemove()\r\n", Port+1 );
			DukeRemove( Port );
			
			DebugOutput( "Port %u: Calling SimRemove()\r\n", Port+1 );
			// Pass information along to the simulator
			SimRemove( Port );
			
			DebugOutput( "Port %u: Don't need to call ScriptRemove()\r\n", Port+1 );
			// We don't need to pass anything to the script if there is
			// no controller connected to the port
		}
	}
}

/*************************************************************************************************

Routine Description:

	DetectChanges

	Detects device insertions and removals and updates list of connected controllers.  Calls 
	HandleChanges to deal with device changes.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void DetectChanges( void )
{
	DWORD Insertions, Removals;
    
	//DebugOutput( "Checking for controller insertions and removals\r\n" );

	// Were there any device changes since the last time we checked?
    if( XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &Insertions, &Removals ) )
    {
		DebugOutput( "Calling HandleChanges\r\n" );
		// Call handler to service the insertion and/or removal
        HandleChanges( Insertions, Removals );

		DebugOutput( "Updating ConnectedDukes\r\n" );
		// Update connected controllers
        ConnectedDukes &= ~Removals;
        ConnectedDukes |= Insertions;
    }
}

/*************************************************************************************************

Routine Description:

	HandleChanges

	Detects device insertions and removals.  Calls DukeInsert(), DukeRemove(), SimInsert(),
	SimRemove(), ScriptInsert(), and ScriptRemove() to create, plug, unplug, and delete the 
	appropriate Duke objects.

Arguments:

	DWORD Insertions	Bit mask of ports into which controllers have been inserted since the
						last time we checked.
	DWORD Removals		Bit mask of ports from which controllers have been removed since the
						last time we checked.

Return Value:

	none

*************************************************************************************************/

void HandleChanges( DWORD Insertions, DWORD Removals )
{
	DWORD Port;
	
	for( Port = 0; Port < NUM_PORTS; ++Port )
    {
		// Close removals
		if( ( 1 << Port & Removals ) && DukeStates[Port].DukeHandle )
        {
			DebugOutput( "Port %u: Controller removed\r\n", Port+1 );

			DebugOutput( "Port %u: Calling ScriptRemove()\r\n", Port+1 );
			// Pass information along to the script
			ScriptFile->ScriptRemove( Port );

			DebugOutput( "Port %u: Calling SimRemove()\r\n", Port+1 );
			// Pass information along to the simulator
			SimRemove( Port );

			DebugOutput( "Port %u: Calling DukeRemove()\r\n", Port+1 );
			DukeRemove( Port );
		}
		// Open insertions
        if( 1 << Port & Insertions )
        {
			DebugOutput( "Port %u: Controller inserted\r\n", Port+1 );

			DebugOutput( "Port %u: Calling DukeInsert()\r\n", Port+1 );
			DukeInsert( Port );

			DebugOutput( "Port %u: Calling SimInsert()\r\n", Port+1 );
			// Pass information along to the simulator
			SimInsert( Port );
			
			DebugOutput( "Port %u: Calling ScriptInsert()\r\n", Port+1 );
			// Pass information along to the script
			ScriptFile->ScriptInsert( Port );
		}
	}
}

/*************************************************************************************************

Routine Description:

	ReadGamepads

	Detects button presses and releases.  Passes controller state and timing information to the 
	simulator and script through SimSetState() and ScriptSetState().

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void ReadGamepads( void )
{
	DWORD Port;

	LARGE_INTEGER NewChange;
	LARGE_INTEGER ElapsedTime;
	
	//DebugOutput( "Checking for button presses and releases\r\n" );

	for( Port = 0; Port < NUM_PORTS; ++Port )
	{
		// Is there a controller connected to the port?
		if( ( 1 << Port & ConnectedDukes ) && DukeStates[Port].DukeHandle )
		{
			//DebugOutput( "Port %u: Found a controller\r\n", Port+1 );

			//DebugOutput( "Port %u: Getting current state\r\n", Port+1 );
			// Get current state
			XInputGetState( DukeStates[Port].DukeHandle, &CurrState );

			//DebugOutput( "Port %u: Comparing current state to last known state\r\n", Port+1 );
			// Is the current state different from the last known state?
			if( memcmp( (void*) &DukeStates[Port].LastState, (void*) &CurrState.Gamepad, 
					sizeof( CurrState.Gamepad ) - sizeof( CurrState.Gamepad.sThumbLX )
					- sizeof( CurrState.Gamepad.sThumbLY ) - sizeof( CurrState.Gamepad.sThumbRX )
					- sizeof( CurrState.Gamepad.sThumbRY ) ) != 0 ||
				abs ( CurrState.Gamepad.sThumbLX - DukeStates[Port].LastState.sThumbLX ) > 
					MaxDrift ||
				abs ( CurrState.Gamepad.sThumbLY - DukeStates[Port].LastState.sThumbLY ) > 
					MaxDrift ||
				abs ( CurrState.Gamepad.sThumbRX - DukeStates[Port].LastState.sThumbRX ) > 
					MaxDrift ||
				abs ( CurrState.Gamepad.sThumbRY - DukeStates[Port].LastState.sThumbRY ) >
					MaxDrift )

			 {
				//DebugOutput( "Port %u: Calculating elapsed time since last state change\r\n", Port+1 );
				// Get current time and calculate elapsed time since last change
				QueryPerformanceCounter( &NewChange );
				ElapsedTime.QuadPart = ( NewChange.QuadPart - DukeStates[Port].LastChange.QuadPart )
					/ Frequency.QuadPart;

				//DebugOutput( "Port %u: Checking if elapsed time is at least one millisecond\r\n", Port+1 );
				// Has at least one millisecond passed since the last state change?
				if( ElapsedTime.QuadPart >= 1 )
				{
					DebugOutput( "Port %u: State has changed\r\n", Port+1 );
				
					DebugOutput( "Port %u: Updating last state change time\r\n", Port+1 );
					// Update last state change time 
					DukeStates[Port].LastChange = NewChange;

					DebugOutput( "Port %u: Updating current state\r\n", Port+1 );
					// Update current state
					DukeStates[Port].LastState = CurrState.Gamepad;

					DebugOutput( "Port %u: Calling SimSetState()\r\n", Port+1 );
					// Pass information along to the simulator
					SimSetState( Port );
					
					DebugOutput( "Port %u: Calling ScriptSetState()\r\n", Port+1 );
					// Pass information along to the script
					ScriptFile->ScriptSetState( Port, ElapsedTime );
				}
			}
		}
	}
}

/*************************************************************************************************

Routine Description:

	DukeInsert

    Opens a handle and updates the appropriate data structures when a controller is inserted.

Arguments:

    DWORD Port			Array index to be used for inserting the Duke.

Return Value: 

	none

*************************************************************************************************/

void DukeInsert( DWORD Port )
{
	DebugOutput( "Port %u: Opening handle\r\n", Port+1 );
	// Open handle
	DukeStates[Port].DukeHandle = XInputOpen( XDEVICE_TYPE_GAMEPAD, Port, XDEVICE_NO_SLOT, NULL );

	if( DukeStates[Port].DukeHandle == NULL )
	{
		DebugOutput( "Port %u: XInputOpen() Error: %u\r\n", Port, GetLastError() );
		DebugOutput( "Port %u: Setting handle to invalid\r\n", Port+1 );
		// Set handle to invalid
		DukeStates[Port].DukeHandle = 0;
	}

	DebugOutput( "Port %u: Setting current state to default state\r\n", Port+1 );
	// Set current state to default state
	DukeStates[Port].LastState = BaseState;
					
	DebugOutput( "Port %u: Setting last state change time to current time\r\n", Port+1 );
	// Set last state change time to current time
	QueryPerformanceCounter( &DukeStates[Port].LastChange );
}

/*************************************************************************************************

Routine Description:

	DukeRemove

    Closes a handle and updates the appropriate data structures when a controller is removed.

Arguments:

    DWORD Port			Array index to be used for removing the Duke.

Return Value: 

	none

*************************************************************************************************/

void DukeRemove( DWORD Port )
{
	if( DukeStates[Port].DukeHandle )
	{
		DebugOutput( "Port %u: Closing handle\r\n", Port+1 );
		// Close handle
		XInputClose( DukeStates[Port].DukeHandle );
	}

	DebugOutput( "Port %u: Setting handle to invalid\r\n", Port+1 );
	// Set handle to invalid
	DukeStates[Port].DukeHandle = 0;

	DebugOutput( "Port %u: Setting current state to default state\r\n", Port+1 );
	// Set current state to default state
	DukeStates[Port].LastState = BaseState;

	DebugOutput( "Port %u: Setting last state change time to current time\r\n", Port+1 );
	// Set last state change time to current time
	QueryPerformanceCounter( &DukeStates[Port].LastChange );
}