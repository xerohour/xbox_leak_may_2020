/*
	Copyright Microsoft 2001 - all rights reserved

  author:  sethmil

  Abstract:

	// CPowerCycle.cpp
	//
	// source file for class CPowerCycle
	//
	// class CPowerCycler uses an ActiveWire USB Board
	// to cycle power on the xbox


	// pin out
	//		IO/0 - output - controls AC power
	//		IO/1 - output - controls POWSW
	//		IO/2 - input  - POWON
*/

#include <windows.h>
#include "CPowerCycle.h"




// default constructor
// protected constructor - does nothing
CPowerCycle::CPowerCycle() {}

// standard constructor
// this is the constructor that will
// be used to create the class object
CPowerCycle::CPowerCycle( DWORD dwDevNum )
{
	
	if( AW_OK != Open( dwDevNum ) )
	{
		return;
	}
	if( AW_OK != EnablePort( OUTPUT_PORT ) )
	{
		Close();
		return;
	}
	iDeviceNum = dwDevNum;
	SetPOWSWHigh();
}

// destructor
CPowerCycle::~CPowerCycle()
{
	CAWDevice::~CAWDevice();
}


// turns box power off (AC Main)
DWORD CPowerCycle::Unplug()
{
	WORD wData;
	InPort( &wData );
	wData = wData & (~ACPOWER) & (~POWSW);
	OutPort( wData );
	InPort( &wData );
	if( wData & POWON )
	{
		return AW_ERROR_HARDWARE;
	}
	return AW_OK;
}


// turns box power on (AC Main)
DWORD CPowerCycle::PlugIn()
{
	WORD wData;
	InPort( &wData );
	wData = wData | ACPOWER;
	OutPort( wData );
	InPort( &wData );
	if( !(wData & ACPOWER) )
	{
		return AW_ERROR_HARDWARE;
	}
	return AW_OK;
}


// returns TRUE if the box is plugged in
BOOL CPowerCycle::PluggedIn()
{
	WORD wData;
	InPort( &wData );
	return( wData & ACPOWER );
}


// BoxIsOn
// returns TRUE if POWON is asserted on the DUT
BOOL CPowerCycle::BoxIsOn()
{
	WORD wData;
	InPort( &wData );
	return ( wData & POWON );		// box is on if IO/2 is high
}

// BoxIsOff()
// returns TRUE if the box is off
BOOL CPowerCycle::BoxIsOff()
{
	return !BoxIsOn();
}


// DeviceIsOpen
// returns TRUE if the AW device is open
BOOL CPowerCycle::DeviceIsOpen()
{
	return CAWDevice::DeviceIsOpen();
}


// ErrorMessage()
// returns a string version of the last error message
char* CPowerCycle::ErrorMessage()
{
	return CAWDevice::ErrorMessage();
}

// ErrorMessage( DWORD )
// returns a string version of the error message
char* CPowerCycle::ErrorMessage( const DWORD errcode )
{
	return CAWDevice::ErrorMessage( errcode );
}

// GetLastError
// returns the last error code
DWORD CPowerCycle::GetLastError()
{
	return CAWDevice::GetLastError();
}



// sets POWSW to LOW
// if POWSW is already LOW, does nothing
// if POWSW does not go low, returns an error.
// POWSW is pin IO/1
DWORD CPowerCycle::SetPOWSWLow()
{
	WORD wData;
	InPort( &wData );
	wData = wData & (~POWSW);
	if( AW_OK != OutPort( wData ))
	{
		return GetLastError();
	}

	if( AW_OK != InPort( &wData ))
	{
		return GetLastError();
	}

	if( wData & POWSW )
	{
		SetLastError( AW_ERROR_HARDWARE );
		return GetLastError();
	}

	return AW_OK;
}


// sets POWSW to HIGH
// if POWSW is already HIGH, does nothing
// if POWSW does not go high, returns an error
DWORD CPowerCycle::SetPOWSWHigh()
{
	WORD wData;
	InPort( &wData );
	wData = wData | POWSW;
	if( AW_OK != OutPort( wData ) )
	{
		return GetLastError();
	}

	if( AW_OK != InPort( &wData ) )
	{
		return GetLastError();
	}

	if( !(wData & POWSW) )
	{
		SetLastError( AW_ERROR_HARDWARE );
		return GetLastError();
	}

	return AW_OK;
}


// returns TRUE if POWSW signal is high
//
BOOL CPowerCycle::POWSWIsHigh()
{
	WORD dwData;
	InPort( &dwData );
	return( dwData & POWSW );
}


// Cycles DUT power by pulsing
// the POWSW line
DWORD CPowerCycle::CyclePower( const DWORD dwPulseWidth )
{
	DWORD dwResult;

	// check to see if POWSW is HIGH
	if( !POWSWIsHigh() )
	{
		dwResult = SetPOWSWHigh();
		if( AW_OK != dwResult )
		{
			SetLastError( dwResult );
			return dwResult;
		}
		Sleep(200);
	}

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
DWORD CPowerCycle::BoxOn( const DWORD dwTimeOut )
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
DWORD CPowerCycle::BoxOff( const DWORD dwTimeOut )
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




