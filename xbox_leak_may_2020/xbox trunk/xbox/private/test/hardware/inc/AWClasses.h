/*
	Copyright Microsoft 2001 - all rights reserved

  author:  sethmil

  Abstract:

	// AWClasses.h
	//
	// header file for classes used with the AW board
	//
	// class CXBoxControlr uses an ActiveWire USB Board
	// to cycle power on the xbox
*/


#ifndef __AWCLASSES_H__
#define __AWCLASSES_H__


#define WIN32_LEAN_AND_MEAN
#ifdef _XBOX
	#include <xtl.h>
#else
	#include <windows.h>
#endif
#include <aw.h>
#include <i2clib.h>

// constants
/*static const WORD  OUTPUT_PORT = 0x0003;
static const WORD  POWSW       = 0x0002;
static const WORD  ACPOWER     = 0x0001;
static const WORD  POWON	   = 0x0004;*/

//#define OUTPUT_PORT 0x0003
#define ACPOWER     0x0001		// output IO/0 - AC Power Control
#define POWSW       0x0002		// output IO/1 - POWSW control		
#define POWON       0x0004		// input IO/2 - POWON signal
#define STANDBY		0x0008		// input IO/3 - 3.3V stby voltage
#define VMODE1		0x0010		// input IO/4 - VMODE1
#define VMODE2		0x0020		// input IO/5 - VMODE2
#define VMODE3		0x0040		// input IO/6 - VMODE3
#define SYSRESET	0x0080		// input - sysreset

#define EJTSW		0x0100		// output DVD Eject
#define TRAYSTATE0	0x0200		// input TRAYState0
#define TRAYSTATE1  0x0400		// input traysate1
#define TRAYSTATE2  0x0800		// input traysate2
#define POWOK		0x1000		// output - POWOK
#define DVDEJECT	0x2000		// input - DVDEject



enum 
{
	OK_XBOX_POWER = 0,
	XBOX_POWER_OFF_ERROR,
	XBOX_POWER_ON_ERROR
};





// AV Pack definitions
static const enum AVPack
{
	NOPACK		= 0x7,
	RFU			= 0x3,
	POWEROFF	= 0x5,
	HDTV		= 0x1,
	SDTV_ANALOG = 0x6,
	VGA			= 0x2,
	SDTV_DIGITAL= 0x4,
	SCART		= 0x0
};



//********************************************************************************************
// Class definitions
//


// class CXBoxControlr uses an ActiveWire USB Board
// to cycle power on the xbox

class CXBoxControl : protected CAWDevice
{
public:
	CXBoxControl( DWORD dwDevNum );
	~CXBoxControl();

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

	// pulses the EJTSW signal
	DWORD EjectSwitch( const DWORD dwPulseWidth );

	// closes the DVD - if the DVD is closed, does nothing
	DWORD DVDClose();

	// opens the DVD - if the DVD is open, does nothing
	DWORD DVDOpen();

	// returns TRUE if the DVD tray is open
	BOOL DVDIsOpen();

	// returns TRUE if the DVD is closed, or in media detect, or in nomedia
	BOOL DVDIsClosed();

	// returns TRUE if the DVD is in media detect state
	BOOL DVDMediaDetect();

	// return TRUE if the DVD is in NoMedia stata
	BOOL DVDNoMedia();

	// returns TRUE if the DVD is in TRAYOPEN state
	BOOL DVDTrayOpen();

	// returns TRUE if the DVD is in CLOSED tray state
	BOOL DVDTrayClosed();

	// returns TRUE if POWSW is high
	BOOL POWSWIsHigh();

	// return TRUE if the box is on
	BOOL BoxIsOn();

	// returns TRUE if the box is off
	BOOL BoxIsOff();

	// returns TRUE if the AW device is open
	BOOL DeviceIsOpen();

	// returns TRUE if 3.3V standby is high
	BOOL STBYIsHigh();

	// enables an output port
	BOOL Enable( WORD wSignal );

	// disables an output prot
	BOOL Disable( WORD wSignal );

	// Error handling
	DWORD GetLastError();
	char* ErrorMessage( const DWORD errcode );
	char* ErrorMessage();


protected:
	CXBoxControl();	// not to be used
	DWORD SetSignalHigh( const WORD wSignal );	// sets a signal high
	DWORD SetSignalLow( const WORD wSignal );		// sets a signal low
	BOOL  SignalIsHigh( const WORD wSignal );		// returns TRUE if a signal is high
	BOOL  SignalIsLow( const WORD wSignal );		// returns TRUE if a signal is low
	
	WORD	wEnable;
};


/////////////////////////////////////////////////////////////////
// Class AV Pack
// controls AV Pack strappings
//
// Pin IO/4 = VMODE1
// PIN IO/5 = VMODE2
// PIN IO/6 = VMODE3
//////////////////////////////////////////////////////////////////

class CAVPack : protected CAWDevice
{
public:
	CAVPack( DWORD dwDevNum );
	~CAVPack( );
	
	// Sets the AV Pack output
	// AV is an enum that specifies the
	// type of AV Pack
	VOID SetAVPack( AVPack av );

	// Drives the AVPack lines
	// the the desired mode
	VOID DriveAVPack( AVPack av );
	
	// Drives the PowerOn signal
	VOID PowerOn( );

	// sets AVPack to POWEROFF
	VOID PowerOff( );


	DWORD GetLastError();
	char* ErrorMessage();

};




#endif