/*
	Copyright Microsoft 2001 - all rights reserved

  author:  sethmil

  Abstract:

	// CPowerCycle.h
	//
	// header file for class CPowerCycle
	//
	// class CPowerCycler uses an ActiveWire USB Board
	// to cycle power on the xbox
*/



#ifndef __CPOWERCYCLE_H__
#define __CPOWERCYCLE_H__


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <aw.h>

// constants
/*static const WORD  OUTPUT_PORT = 0x0003;
static const WORD  POWSW       = 0x0002;
static const WORD  ACPOWER     = 0x0001;
static const WORD  POWON	   = 0x0004;*/

#define OUTPUT_PORT 0x0003
#define POWSW       0x0002
#define ACPOWER     0x0001
#define POWON       0x0004	


enum 
{
	OK_XBOX_POWER = 0,
	XBOX_POWER_OFF_ERROR,
	XBOX_POWER_ON_ERROR
};


class CPowerCycle : protected CAWDevice
{
public:
	CPowerCycle( DWORD dwDevNum );
	~CPowerCycle();

	// turns box power on (AC Main)
	DWORD Unplug();

	// turns box power off (AC Main)
	DWORD PlugIn();

	// returns TRUE if the box is plugged in
	BOOL PluggedIn();

	// turns the box on
	DWORD BoxOn( const DWORD dwTimeOut = 10000 );

	// turns the box off
	DWORD BoxOff( const DWORD dwTimeOut = 10000 );

	// asserts POWSW for dwPulseWidth milliseconds
	DWORD CyclePower( const DWORD dwPulseWidth );

	// sets the POWSW signal to LOW
	DWORD SetPOWSWLow();

	// sets the POWSW signal to HIGH
	DWORD SetPOWSWHigh();

	// returns TRUE if POWSW is high
	BOOL POWSWIsHigh();

	// return TRUE if the box is on
	BOOL BoxIsOn();

	// returns TRUE if the box is off
	BOOL BoxIsOff();

	// returns TRUE if the AW device is open
	BOOL DeviceIsOpen();

	// Error handling
	DWORD GetLastError();
	char* ErrorMessage( const DWORD errcode );
	char* ErrorMessage();


protected:
	CPowerCycle();	// not to be used
};


#endif