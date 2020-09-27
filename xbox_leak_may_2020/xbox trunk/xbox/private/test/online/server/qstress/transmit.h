/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#ifndef __TRANSMIT_H_
#define __TRANSMIT_H_

#ifndef _WINDOWS_
#include <windows.h>
#endif

#include "channels.h"
#include "control.h"
#include "config.h"

//Transmit thread parameters structure
struct TransmitThreadFuncParams{
	HANDLE hStop;  //Thread stop event
	HANDLE hStopped;  //Thread is stopped event
	HANDLE hReady;  //Thread is ready event
	HANDLE hStart;  //Thread start event
	Control* pControl;  //Pointer to application control
	Channels* pChannels;  //Pointer to connection channels
	Configuration* pConfig;  //Pointer to application configuration
};//endstruct

/////////////////////////////////////////////////////////////////////////////
// Transmit thread that performs actions against XNQ
/////////////////////////////////////////////////////////////////////////////

DWORD WINAPI TransmitThreadFunc(LPVOID pData);

#endif