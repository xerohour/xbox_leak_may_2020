/*
	Copyright Microsoft 2001 - all rights reserved

  author:  sethmil

  Abstract:

	// AWClasses.cpp
	//
	// source file for class AWClasses.h
	//
	// contains a bunch of classes to use with the AW
	// board for testing xbox
	//
	// class CXBoxControlr uses an ActiveWire USB Board
	// to cycle power on the xbox


	// pin out
	//		IO/0 - output - controls AC power
	//		IO/1 - output - controls POWSW
	//		IO/2 - input  - POWON
*/

#include <windows.h>
#include "AWClasses.h"
#include <i2clib.h>
#include <stdio.h>




// default constructor
// protected constructor - does nothing
CXBoxControl::CXBoxControl() {}

// standard constructor
// this is the constructor that will
// be used to create the class object
CXBoxControl::CXBoxControl( DWORD dwDevNum )
{
	
	if( AW_OK != Open( dwDevNum ) )
	{
		return;
	}

	wEnable = ACPOWER | POWSW | EJTSW;
	if( AW_OK != EnablePort( wEnable ) )
	{
		Close();
		return;
	}
	iDeviceNum = dwDevNum;
	SetSignalHigh( ACPOWER );
	SetSignalHigh( POWSW );
	SetSignalHigh( EJTSW );
}

// destructor
CXBoxControl::~CXBoxControl()
{
	CAWDevice::~CAWDevice();
}


// turns box power off (AC Main)
DWORD CXBoxControl::Unplug()
{
	wEnable = ACPOWER;
	EnablePort( wEnable );
	return SetSignalLow( ACPOWER );
}


// turns box power on (AC Main)
DWORD CXBoxControl::PlugIn()
{
	DWORD result = AW_OK;
	wEnable = ACPOWER | POWSW | EJTSW;
	EnablePort( wEnable );
	result = result | SetSignalHigh( ACPOWER );
	result = result |SetSignalHigh( POWSW );
	result = result | SetSignalHigh( EJTSW );
	return result;
}


// returns TRUE if the box is plugged in
BOOL CXBoxControl::PluggedIn()
{
	return SignalIsHigh( ACPOWER );
}


// BoxIsOn
// returns TRUE if POWON is asserted on the DUT
BOOL CXBoxControl::BoxIsOn()
{
	return SignalIsHigh( POWON );
}

// BoxIsOff()
// returns TRUE if the box is off
BOOL CXBoxControl::BoxIsOff()
{
	return !BoxIsOn();
}


// DeviceIsOpen
// returns TRUE if the AW device is open
BOOL CXBoxControl::DeviceIsOpen()
{
	return CAWDevice::DeviceIsOpen();
}


// ErrorMessage()
// returns a string version of the last error message
char* CXBoxControl::ErrorMessage()
{
	return CAWDevice::ErrorMessage();
}

// ErrorMessage( DWORD )
// returns a string version of the error message
char* CXBoxControl::ErrorMessage( const DWORD errcode )
{
	return CAWDevice::ErrorMessage( errcode );
}

// GetLastError
// returns the last error code
DWORD CXBoxControl::GetLastError()
{
	return CAWDevice::GetLastError();
}



// sets POWSW to LOW
// if POWSW is already LOW, does nothing
// if POWSW does not go low, returns an error.
// POWSW is pin IO/1
DWORD CXBoxControl::SetPOWSWLow()
{
	return SetSignalLow( POWSW );
}


// sets POWSW to HIGH
// if POWSW is already HIGH, does nothing
// if POWSW does not go high, returns an error
DWORD CXBoxControl::SetPOWSWHigh()
{
	return SetSignalHigh( POWSW );
}


// returns TRUE if POWSW signal is high
//
BOOL CXBoxControl::POWSWIsHigh()
{
	return SignalIsHigh( POWSW );
}


// Cycles DUT power by pulsing
// the POWSW line
DWORD CXBoxControl::CyclePower( const DWORD dwPulseWidth )
{
	DWORD dwResult;

	// check to see if POWSW is HIGH
	dwResult = SetPOWSWHigh();
	if( AW_OK != dwResult )
	{
		SetLastError( dwResult );
		return dwResult;
	}
	Sleep( 100 );

	// now cycle power
	dwResult = SetPOWSWLow();
	if( AW_OK != dwResult )
	{
		SetLastError( dwResult );
		return dwResult;
	}
	Sleep( dwPulseWidth );
	dwResult = SetPOWSWHigh();
	if( AW_OK != dwResult )
	{
		SetLastError( dwResult );
		return dwResult;
	}

	return AW_OK;
}

	



// BoxOn
// turns the box on
// if the box is already on, does nothing
// if the box does not turn on,
// returns an error code
DWORD CXBoxControl::BoxOn( const DWORD dwTimeOut )
{
	DWORD dwResult;
	DWORD dwStartTime;

	// check to make sure the box is plugged in
	if( !PluggedIn() )
	{
		PlugIn();
		Sleep(200);
	}
	// check to make sure we're starting
	// with POWSW high
	if( !POWSWIsHigh() )
	{
		if(AW_OK != (dwResult = SetPOWSWHigh()))
		{
			return dwResult;
		}
		Sleep( 200 );
	}

	// check to see if power is on
	if( BoxIsOn() )
	{
		return AW_OK;
	}

	// if box is off, we must turn it on
	dwResult = CyclePower( 100 );
	if( AW_OK != dwResult )
	{
		SetLastError( dwResult );
		return dwResult;
	}

	dwStartTime = GetTickCount();

	// now wait for box to turn on
	while( !BoxIsOn() )
	{
		// check timeout
		if( (GetTickCount() - dwStartTime) > dwTimeOut )
		{
			SetLastError( AW_ERROR_TIMEOUT );
			return AW_ERROR_TIMEOUT;
		}
		Sleep(10);
	}

	return AW_OK;
}



		
// BoxOff
// turns the box off
// if the box is already off, does nothing
// if the box does not turn off within the timeout,
// returns an error code
DWORD CXBoxControl::BoxOff( const DWORD dwTimeOut )
{
	DWORD dwResult;
	DWORD dwStartTime;

	// check to make sure we're starting
	// with POWSW high
	if( !POWSWIsHigh() )
	{
		if(AW_OK != (dwResult = SetPOWSWHigh()))
		{
			return dwResult;
		}
		Sleep( 200 );
	}

	// check to see if power is off
	if( BoxIsOff() )
	{
		return AW_OK;
	}

	// if box is on, we must turn it off
	dwResult = CyclePower( 100 );
	if( AW_OK != dwResult )
	{
		SetLastError( dwResult );
		return dwResult;
	}

	dwStartTime = GetTickCount();

	// now wait for box to turn on
	while( BoxIsOn() )
	{
		// check timeout
		if( (GetTickCount() - dwStartTime) > dwTimeOut )
		{
			SetLastError( AW_ERROR_TIMEOUT );
			return AW_ERROR_TIMEOUT;
		}
		Sleep(10);
	}

	return AW_OK;
}


// returns TRUE if 3.3V standby is high
BOOL CXBoxControl::STBYIsHigh()
{
	return SignalIsHigh( STANDBY );
}


// pulses the EJTSW signal
DWORD CXBoxControl::EjectSwitch( const DWORD dwPulseWidth )
{
	// check to make sure EJTSW is high
	if( !SignalIsHigh( EJTSW ) )
	{
		SetSignalHigh( EJTSW );
		Sleep( 200 );
	}

	// now pulse the signal
	SetSignalLow( EJTSW );
	Sleep( dwPulseWidth );
	SetSignalHigh( EJTSW );

	return AW_OK;	

}

// closes the DVD - if the DVD is closed, does nothing
DWORD CXBoxControl::DVDClose()
{
	if( DVDIsClosed() )
	{
		return AW_OK;
	}

	EjectSwitch( 100 );

	if( DVDIsOpen() )
	{
		return AW_ERROR_HARDWARE;
	}

	return AW_OK;
}

// opens the DVD - if the DVD is open, does nothing
DWORD CXBoxControl::DVDOpen()
{
	if( DVDIsOpen() )
	{
		return AW_OK;
	}

	EjectSwitch( 100 );

	if( DVDIsClosed() )
	{
		return AW_ERROR_HARDWARE;
	}

	return AW_OK;
}

// returns TRUE if the DVD tray is open
BOOL CXBoxControl::DVDIsOpen()
{
	return DVDTrayOpen();
}

// returns TRUE if the DVD is in media detect state
BOOL CXBoxControl::DVDMediaDetect()
{
	WORD wData;
	InPort( &wData );

	// shift data so traystate bits are the 3 LSB's
	wData = wData >> 9;
	// mask out the traystate bits
	wData &= 0x0007;

	return ( wData == 0x0006 );	// Tray Open state
}

// return TRUE if the DVD is in NoMedia stata
BOOL CXBoxControl::DVDNoMedia()
{
	WORD wData;
	InPort( &wData );

	// shift data so traystate bits are the 3 LSB's
	wData = wData >> 9;
	// mask out the traystate bits
	wData &= 0x0007;

	return ( wData == 0x0004 );	// no media state
}

// returns TRUE if the DVD is in TRAYOPEN state
BOOL CXBoxControl::DVDTrayOpen()
{
	WORD wData;
	InPort( &wData );

	// shift data so traystate bits are the 3 LSB's
	wData = wData >> 9;
	// mask out the traystate bits
	wData &= 0x0007;

	return ( wData == 0x0001 );	// Tray Open state
}

// returns TRUE if the DVD is in CLOSED tray state
BOOL CXBoxControl::DVDTrayClosed()
{
	WORD wData;
	InPort( &wData );

	// shift data so traystate bits are the 3 LSB's
	wData = wData >> 9;
	// mask out the traystate bits
	wData &= 0x0007;

	return ( wData == 0x0000 );	// Tray Closed state
}

// returns TRUE if the DVD is closed
BOOL CXBoxControl::DVDIsClosed()
{
	WORD wData;
	InPort( &wData );

	// shift data so traystate bits are the 3 LSB's
	wData = wData >> 9;
	// mask out the traystate bits
	wData &= 0x0007;

	// tray closed, no media, and media detect states are
	// all valid
	if( (wData == 0x0000) || (wData == 0x0004) || (wData == 0x0006) )
	{
		return TRUE;
	}

	return FALSE;
}


// enables an output port
BOOL CXBoxControl::Enable( WORD wSignal )
{
	wEnable = wEnable | wSignal;
	return EnablePort( wEnable );
}


// disables an output prot
BOOL CXBoxControl::Disable( WORD wSignal )
{
	wEnable = wEnable & (~wSignal);
	return EnablePort( wEnable );
}




// protected functions
// ************************
// sets a signal high
DWORD CXBoxControl::SetSignalHigh( const WORD wSignal )	
{
	WORD wData;
	InPort( &wData );
	wData = wData | wSignal;
	if( AW_OK != OutPort( wData ) )
	{
		return GetLastError();
	}

	if( AW_OK != InPort( &wData ) )
	{
		return GetLastError();
	}

	if( !(wData & wSignal) )
	{
		SetLastError( AW_ERROR_HARDWARE );
		return GetLastError();
	}

	return AW_OK;

}
// sets a signal low
DWORD CXBoxControl::SetSignalLow( const WORD wSignal )
{
	WORD wData;
	InPort( &wData );
	wData = wData & (~wSignal);
	if( AW_OK != OutPort( wData ))
	{
		return GetLastError();
	}

	if( AW_OK != InPort( &wData ))
	{
		return GetLastError();
	}

	if( wData & wSignal )
	{
		SetLastError( AW_ERROR_HARDWARE );
		return GetLastError();
	}

	return AW_OK;
}		
// returns TRUE if a signal is high
BOOL  CXBoxControl::SignalIsHigh( const WORD wSignal )	
{
	WORD wData;
	InPort( &wData );
	return( wData & wSignal );
}	
// returns TRUE if a signal is low
BOOL  CXBoxControl::SignalIsLow( const WORD wSignal )
{
	return !SignalIsHigh( wSignal );
}	


//*********************************************************************************
// Class AV Pack
// controls AV Pack strappings
//
// Pin IO/4 = VMODE1
// PIN IO/5 = VMODE2
// PIN IO/6 = VMODE3
//////////////////////////////////////////////////////////////////

CAVPack::CAVPack( DWORD dwDevNum )
{
	dwErrCode = Open( dwDevNum );
	if( AW_OK != dwErrCode )
	{
		iDeviceNum = -1;
		return;
	}
	iDeviceNum = (int)dwDevNum;
	dwErrCode = OutPort( 0x0000 );
	if( AW_OK != dwErrCode )
		return;
	dwErrCode = EnablePort( 0x0000 );
	
}


CAVPack::~CAVPack()
{
	dwErrCode = EnablePort( 0x0000 );
	if( DeviceIsOpen() )
		Close( );
}


// Sets the AV Pack output
// AV is an enum that specifies the
// type of AV Pack
VOID CAVPack::SetAVPack( AVPack av )
{
	WORD wEnable = 0;
	
	// make sure outputs are all driven low
	
	switch( av )
	{
		case RFU:
			wEnable = VMODE3;
			break;
		case NOPACK:
			wEnable = 0x00;
			break;
		case POWEROFF:
			wEnable = VMODE2;
			break;
		case HDTV:
			wEnable = VMODE2 | VMODE3;
			break;
		case SDTV_ANALOG:
			wEnable = VMODE1;
			break;
		case VGA:
			wEnable = VMODE1 | VMODE3;
			break;
		case SDTV_DIGITAL:
			wEnable = VMODE1 | VMODE2;
			break;
		case SCART:
			wEnable = VMODE1 | VMODE2 | VMODE3;
			break;
		default:
			wEnable = 0x00;
			break;
	}

	EnablePort( wEnable );
	OutPort( 0x00 );	// bug bug
}



// Drives the AVPack lines
// the the desired mode
VOID CAVPack::DriveAVPack( AVPack av )
{
	WORD wData = 0x0;
	// make sure outputs are all driven low
	
	switch( av )
	{
		case RFU:
			wData = RFU << 4;
			break;
		case NOPACK:
			wData = NOPACK << 4;
			break;
		case POWEROFF:
			wData = POWEROFF << 4;
			break;
		case HDTV:
			wData = HDTV << 4;
			break;
		case SDTV_ANALOG:
			wData = SDTV_ANALOG << 4;
			break;
		case VGA:
			wData = VGA << 4;
			break;
		case SDTV_DIGITAL:
			wData = SDTV_DIGITAL << 4;
			break;
		case SCART:
			wData = SCART << 4;
			break;
		default:
			wData = 0x00;
			break;
	}

	OutPort( wData );
	EnablePort( VMODE1 | VMODE2 | VMODE3 );
}


// Power On
VOID CAVPack::PowerOn( )
{
	// Disable output port
	EnablePort( 0x00 );
	
	// now write the data
	OutPort( VMODE1 | VMODE2 | VMODE3 );
	EnablePort( VMODE1 | VMODE2 | VMODE3 );
}

// drive power off output
// Drives NOPACK for T1 ms
// Drives POWOFF for T2 ms
VOID CAVPack::PowerOff( )
{
	SetAVPack( POWEROFF );
}


DWORD CAVPack::GetLastError()
{
	return dwErrCode;
}

char* CAVPack::ErrorMessage()
{
	return CAWDevice::ErrorMessage( );
}




