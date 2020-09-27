#ifndef _STACKS_
	#define _STACKs_ 
#include "failure.h"

CHAR *StoreStack(CHAR **Lines, DWORD numLines);
VOID StripStack(CHAR **StackLines, DWORD *numLines);
// Tries to repair stack line at address. Returns new buffer if successful
BOOL GetRepairedStack(Cfailure *Fail, ULARGE_INTEGER Address);
BOOL DoesStackNeedManualReload(Cfailure * Fail, ULARGE_INTEGER* BadAddress);
// Given an address it does a !dlls -c and gets the dll name and base
BOOL GetDllAndBase(Cfailure * Fail, ULARGE_INTEGER Address, CHAR *DllName, ULARGE_INTEGER *Base);

#endif