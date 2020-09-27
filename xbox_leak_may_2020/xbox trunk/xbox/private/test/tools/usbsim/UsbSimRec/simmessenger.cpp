/*************************************************************************************************

  Copyright (c) 2001 Microsoft Corporation

  Module Name:

	simmessenger.cpp

  Abstract:

	Defines the USB Simulator Recorder functions which send input events to a USB Simulator.

  Author:

	Mabel Santos (t-msanto) 25-May-2001

  Revision History:

*************************************************************************************************/

#include "usbsimrec.h"

SIM_INFO SimDukes[NUM_PORTS];

/*************************************************************************************************

Routine Description:

	SimInit

	Finds connected simulators.  Attaches to a specific simulator and gets information from it.

Arguments:
	
	None

Return Value:

	None
	
*************************************************************************************************/

void SimInit( void )
{
	unsigned NumSimulators;
	SimPacketTyped<unsigned char [128]> SimData;

	DebugOutput( "Looking for simulators\r\n");
	// Are there any simulators connected?
	NumSimulators = UsbSim->FindSimulators();

	DebugOutput( "Finding simulator: %s\r\n", SimIp );
	// Point to the simulator specified in INI file
	UsbSim->ip[0] = inet_addr( SimIp );
	
	// Check if we can talk to the specified simulator
	memset(&SimData, 0, sizeof(SimData));
	UsbSim->SendCommand(0, SIM_CMD_STATUS, SIM_SUBCMD_STATUS_VERSION, SimData);
	SimData.data[SimData.header.dataSize] = '\0';
	
	// Keep looping until the specified simulator is found
	while( NumSimulators == 0 || SimData.header.dataSize == 0 || SimData.data[0] == '\0' )
	{
		DebugPrint( "Did NOT find simulator: %s. Trying again...\r\n", SimIp );
		Sleep( 500 );
		NumSimulators = UsbSim->FindSimulators();
		UsbSim->ip[0] = inet_addr( SimIp );
		memset(&SimData, 0, sizeof(SimData));
		UsbSim->SendCommand(0, SIM_CMD_STATUS, SIM_SUBCMD_STATUS_VERSION, SimData);
		SimData.data[SimData.header.dataSize] = '\0';
	}

	// Reaching here means we were able to get the simulator version
	DebugOutput( "Found simulator: %s\r\n", SimIp );
	
	// Display the version and MAC address of the simulator
    DebugOutput("Sim Version: %s\r\n", SimData.data);

    memset(&SimData, 0, sizeof(SimData));
    UsbSim->SendCommand(0, SIM_CMD_STATUS, SIM_SUBCMD_STATUS_MACADDR, SimData);
    DebugOutput("Sim MAC Addr: %02X %02X %02X %02X %02X %02X\r\n", SimData.data[0], SimData.data[1], SimData.data[2], SimData.data[3], SimData.data[4], SimData.data[5]);
  }

/*************************************************************************************************

Routine Description:

	SimInsert

	Creates a new Duke device.  Calls SimPlug() to plug in the Duke and get it enumerated.

Arguments:

	DWORD Port		Array index to be used for inserting the new Duke.

Return Value:

	none

*************************************************************************************************/

void SimInsert( DWORD Port )
{
	// Is there a controller plugged into the port?
	if( DukeStates[Port].DukeHandle )
	{
		DebugOutput( "Port %u: Creating new Duke device\r\n", Port+1 );
		// Create new Duke device
		SimDukes[Port].DukeDevice = new XIDDevice;
	
		DebugOutput( "Port %u: Setting XID report to default value\r\n", Port+1 );
		// Set XID report to default value
		SimDukes[Port].XidPacket = defaultXIDReport;

		DebugOutput( "Port %u: Calling SimPlug()\r\n", Port+1 );
		// Wait for Duke to get enumerated
		SimPlug( Port );
	}
}

/*************************************************************************************************

Routine Description:

	SimPlug

	Loops around, plugging in a new Duke and waiting for it to get enumerated.  If enumeration 
	does not succeed after 3 attempts, asks the user to unplug the controller and plug it back in.

Arguments:

	DWORD Port		Array index to be used for plugging in the new Duke.

Return Value:

	none

*************************************************************************************************/

void SimPlug( DWORD Port )
{
	DWORD Result;
	DWORD Retry;
	DWORD Timer;

	// Try a maximum of 3 times to get Duke enumerated
	for( Retry = 3; Retry > 0; Retry-- )
	{
		DebugOutput( "Port %u: Plugging in Duke\r\n", Port+1 );
		// Plug in the Duke
		Result = UsbSim->Plug( Port + 1, 0, SimDukes[Port].DukeDevice );
		_ASSERT(Result == USBSIM_ERROR_OK);
		
		// Wait for enumeration to happen
		for( Timer = 0; Timer < 3000; Timer += 250 )
		{
			Sleep( 250 );
			if( SimDukes[Port].DukeDevice->IsEnumerated() )
			{
				DebugOutput( "Port %u: Duke enumerated\r\n", Port+1 );
				return;
			}
		}

		// Reaching here means we need to try again
		DebugOutput( "Port %u: Duke NOT enumerated. Trying again...\r\n", Port+1 );
		
		DebugOutput( "Port %u: Unplugging Duke\r\n", Port+1 );
		// Unplug the Duke
		Result = UsbSim->Unplug( SimDukes[Port].DukeDevice );
		_ASSERT(Result == USBSIM_ERROR_OK);
	}

	// Reaching here means we must ask the user to unplug the Duke and plug it back in
	DebugPrint( "Port %u: Duke enumeration FAILED.\r\n", Port+1 );
	DebugPrint( "Please unplug the controller and plug it back in.\r\n", Port+1 );
	Sleep(5000);
}

/*************************************************************************************************

Routine Description:

	SimRemove

	Unplugs a Duke device and deletes it.

Arguments:

	DWORD Port		Array index to be used for removing the Duke.

Return Value:

	none

*************************************************************************************************/

void SimRemove( DWORD Port )
{
	DWORD Result;

	// Is there a controller plugged into the port?
	if( DukeStates[Port].DukeHandle )
    {
		DebugOutput( "Port %u: Unplugging Duke\r\n", Port+1 );
		// Unplug the Duke
		Result = UsbSim->Unplug( SimDukes[Port].DukeDevice );
		_ASSERT(Result == USBSIM_ERROR_OK);

		DebugOutput( "Port %u: Deleting Duke device\r\n", Port+1 );
		// Delete Duke device
		delete SimDukes[Port].DukeDevice;
	}

	DebugOutput( "Port %u: Setting Duke device pointer to null\r\n", Port+1 );
	// Set DukeDevice pointer to null
	SimDukes[Port].DukeDevice = NULL;

	DebugOutput( "Port %u: Setting XID report to default value\r\n", Port+1 );
	// Set XID report to default value
	SimDukes[Port].XidPacket = defaultXIDReport;
}

/*************************************************************************************************

Routine Description:

	SimSetState

	Uses the controller state obtained in ReadGamepads() to simulate button presses and releases.


Arguments:

	DWORD Port		Array index to be used for simulating Duke button presses and releases.

Return Value:

	none

*************************************************************************************************/

void SimSetState( DWORD Port )
{
	// Is there a controller plugged into the port?
	if( DukeStates[Port].DukeHandle )
	{
		DebugOutput( "Port %u: Copying current state of controller\r\n", Port+1 );
		// Copy current state of controller
		memcpy( (void*) &(SimDukes[Port].XidPacket.buttons), (void*) &CurrState.Gamepad,
			sizeof( CurrState.Gamepad ) );

		DebugOutput( "Port %u: Sending new controller state\r\n", Port+1 );
		// Send new controller state
		SimDukes[Port].XidPacket.size = sizeof(XIDInputReport);
		SimDukes[Port].DukeDevice->SetInputReport( &SimDukes[Port].XidPacket );
	}
}

