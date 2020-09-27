/*************************************************************************************************

  Copyright (c) 2001 Microsoft Corporation

  Module Name:

	usbsimrec.h

  Abstract:

	Defines the USB Simulator Recorder data structures and function prototypes.

  Author:

	Mabel Santos (t-msanto) 25-May-2001

  Revision History:

*************************************************************************************************/


#ifndef _USBSIMREC_
#define _USBSIMREC_

#include <xtl.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
#include <iostream>
#include <fstream.h>
#include <assert.h>


#include "..\inc\usbsimulator.h"
#include "..\..\..\inc\profilep.h"

#define NUM_PORTS XGetPortCount()  // Number of controller ports on an Xbox


// INI FILE STUFF

// Flag to indicate debug mode
extern DWORD DebugOn;

// Threshold for thumbstick drift
extern LONG MaxDrift;

// Type of script file to generate
extern DWORD ScriptType;

// Buffer length for character strings
extern const DWORD BuffLen;

// IP address of USB simulator to attach to
extern char *SimIp;

// Name of script file
extern char *FileName;  

void StringTrim( char *String );

void DebugOutput( char *ptszFormat, ... );


// DUKELISTENER STUFF

// Connected controllers
extern DWORD ConnectedDukes;

// A controller's handle, last known state and last state change time
struct STATE_INFO {
	HANDLE DukeHandle;
	XINPUT_GAMEPAD LastState;
	LARGE_INTEGER LastChange;
};

// States of different controllers
extern STATE_INFO DukeStates[NUM_PORTS];

// Variable for retrieving the performance counter's frequency
// Used for calculating timing information
extern LARGE_INTEGER Frequency;

// Variable for querying a controller's current state
extern XINPUT_STATE CurrState;

// Default controller state
extern XINPUT_GAMEPAD BaseState;

// Array for initializing controllers
extern XDEVICE_PREALLOC_TYPE Dukes[];

void InitGamepads( void );

void DetectChanges( void );

void HandleChanges( DWORD Insertions, DWORD Removals );

void ReadGamepads( void );

void DukeInsert( DWORD Port );

void DukeRemove( DWORD Port );


// SIMMESSENGER STUFF

// A XIDDevice object and input report for simulating controllers
struct SIM_INFO {
	XIDDevice *DukeDevice;
	XIDInputReport XidPacket;
};

// Duke objects for different controllers
extern SIM_INFO SimDukes[NUM_PORTS];

// USB Simulator
extern USBSimulator *UsbSim;

void SimInit( void );

void SimInsert( DWORD Port );

void SimPlug( DWORD Port );

void SimRemove( DWORD Port );

void SimSetState( DWORD Port );


// SCRIPTWRITER STUFF

// Script abstract superclass
class Script
{
public:

	// Output filestream
	ofstream OutFile;

	Script(){}

	virtual ~Script(){}

	virtual void ScriptInsert( DWORD Port ) = 0;

	virtual void ScriptRemove( DWORD Port ) = 0;

	virtual void ScriptSetState( DWORD Port, const LARGE_INTEGER& ElapsedTime ) = 0;
};


// JScript subclass
class JScript : public Script
{
public:

	JScript( void );

	~JScript(){}

	void ScriptInsert( DWORD Port );

	void ScriptRemove( DWORD Port );

	void ScriptSetState( DWORD Port, const LARGE_INTEGER& ElapsedTime );
};

// Script File
extern Script *ScriptFile;

#endif