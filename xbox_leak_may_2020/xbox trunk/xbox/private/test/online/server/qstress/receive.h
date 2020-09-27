/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#ifndef __RECEIVE_H_
#define __RECEIVE_H_

#ifndef _WINDOWS_
#include <windows.h>
#endif

#include "channels.h"
#include "control.h"

//Transmit thread parameters structure
struct ReceiveThreadFuncParams{
	HANDLE hStop;  //Thread stop event
	HANDLE hStopped;  //Thread is stopped event
	HANDLE hReady;  //Thread is ready to execute event
	HANDLE hStart;  //Thread start event
	Control* pControl;  //Pointer to application control
	Channels* pChannels;  //Pointer to connection channels
};//endstruct

/////////////////////////////////////////////////////////////////////////////
// Thread that receives and processes incoming data
/////////////////////////////////////////////////////////////////////////////

DWORD WINAPI ReceiveThreadFunc(LPVOID pData);

#endif