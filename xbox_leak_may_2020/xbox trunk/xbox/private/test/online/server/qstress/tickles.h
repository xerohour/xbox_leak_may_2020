/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#ifndef __TICKLES_H_
#define __TICKLES_H_

#ifndef _WINDOWS_
#include <windows.h>
#endif

#include "xqprotocol.h"
#include "control.h"

//Tickle thread parameters structure
struct TickleThreadFuncParams{
	HANDLE hStop;  //Thread stop event
	HANDLE hStopped;  //Thread is stopped event
	HANDLE hReady;  //Thread is ready event
	HANDLE hStart;  //Thread start event
	Control* pControl;  //Pointer to application control
};//endstruct

/////////////////////////////////////////////////////////////////////////////
// Thread function that receives and processes all tickle notifications
/////////////////////////////////////////////////////////////////////////////

DWORD WINAPI TickleThreadFunc(LPVOID pData);

#endif