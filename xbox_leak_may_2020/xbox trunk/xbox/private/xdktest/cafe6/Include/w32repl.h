///////////////////////////////////////////////////////////////////////////////
// File: W32Repl.h
//
// Description: This file creates strategic replacements for certain Win32 
//              API functions.  This header is a forced #include for ALL
//              CAFE source files.
///////////////////////////////////////////////////////////////////////////////

#ifndef _INC_W32REPL_H
#define _INC_W32REPL_H

#include <afxwin.h>
#include "targxprt.h"

extern TARGET_DATA UINT g_nSleepModifier;
extern TARGET_DATA BOOL g_bInfiniteIdleWaits;

TARGET_API DWORD CAFE_WaitForInputIdle(HANDLE hProcess, DWORD dwTimeout);
TARGET_API VOID CAFE_Sleep(DWORD cMilliseconds);

#define Sleep(X) CAFE_Sleep((X))
#define WaitForInputIdle(X, Y)  CAFE_WaitForInputIdle((X), (Y))

#endif // _INC_W32REPL_H
