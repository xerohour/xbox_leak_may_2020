/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    memval.cpp

Abstract:

    Read and write the specified value to the specified address

Environment:

    Xbox

Revision History:

--*/

#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <xlogconio.h>

//
// 128 bit value
struct MEMBUS {
	DWORD DWord0;
	DWORD DWord1;
	DWORD DWord2;
	DWORD DWord3;
};

VOID
WINAPI
MemValStartTest(HANDLE LogHandle) {
	MEMBUS *Val, RVal, WAVal, WBVal;
	ULONG_PTR Address;
	BOOL bARO, bAWO, bBRO, bBWO, bUseWriteCombine, bDone, bPass;
	DWORD Protect;
	int TestIterations, i;

	// Initialize
	i = 0;
	bDone = FALSE;
	bPass = TRUE;

	// Get physical address from testini.ini, default MAXULONG_PTR causes system to determine address
	Address = (ULONG_PTR)GetProfileInt(TEXT("memval"), TEXT("Address"), MAXULONG_PTR);

	// Get A value from testini.ini
	WAVal.DWord0 = (DWORD)GetProfileInt(TEXT("memval"), TEXT("AVal0"), 0x00000000);
	WAVal.DWord1 = (DWORD)GetProfileInt(TEXT("memval"), TEXT("AVal1"), 0x55555555);
	WAVal.DWord2 = (DWORD)GetProfileInt(TEXT("memval"), TEXT("AVal2"), 0xaaaaaaaa);
	WAVal.DWord3 = (DWORD)GetProfileInt(TEXT("memval"), TEXT("AVal3"), 0xffffffff);
	bARO = GetProfileInt(TEXT("memval"), TEXT("ARO"), 1);
	bAWO = GetProfileInt(TEXT("memval"), TEXT("AWO"), 1);

	// Get B value from testini.ini
	WBVal.DWord0 = (DWORD)GetProfileInt(TEXT("memval"), TEXT("BVal0"), 0xffffffff);
	WBVal.DWord1 = (DWORD)GetProfileInt(TEXT("memval"), TEXT("BVal1"), 0xaaaaaaaa);
	WBVal.DWord2 = (DWORD)GetProfileInt(TEXT("memval"), TEXT("BVal2"), 0x55555555);
	WBVal.DWord3 = (DWORD)GetProfileInt(TEXT("memval"), TEXT("BVal3"), 0x00000000);
	bBRO = GetProfileInt(TEXT("memval"), TEXT("BRO"), 1);
	bBWO = GetProfileInt(TEXT("memval"), TEXT("BWO"), 1);

	// Get number of iterations from testini.ini
	TestIterations = GetProfileInt(TEXT("memval"), TEXT("TestIterations"), 1);

	// Memory Protection
	bUseWriteCombine = GetProfileInt(TEXT("memval"), TEXT("UseWriteCombine"), 0);
	if(bUseWriteCombine) {
		Protect = PAGE_READWRITE | PAGE_WRITECOMBINE;
	} else {
		Protect = PAGE_READWRITE | PAGE_NOCACHE;
	}

	// Set xLog info
    xSetOwnerAlias(LogHandle, "a-emebac");
	xSetComponent(LogHandle, "Hardware", "MemVal");
	xSetFunctionName(LogHandle, "MemVal");

	// Allocate 128 bits at address
	Val = (MEMBUS *)XPhysicalAlloc(sizeof(WAVal), Address, 0,  Protect);
    if(Val == NULL) {
        xLog(gbConsoleOut, LogHandle, XLL_FAIL, "MemVal: XPhysicalAlloc() %u bytes at %p FAILED: %s",
				sizeof(WAVal), Address, WinErrorSymbolicName(GetLastError()));
        return;
    }

	// Log memory allocation
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "MemVal: Virtural Address: %p, Physical Address: %p", Val, Address);

	// Log Value A
	if(bARO && bAWO) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Read/Write Value A: %#.8x%.8x%.8x%.8x",
			WAVal.DWord3, WAVal.DWord2, WAVal.DWord1, WAVal.DWord0);
	} else if(bARO && !bAWO) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Read Only Value A: %#.8x%.8x%.8x%.8x",
			WAVal.DWord3, WAVal.DWord2, WAVal.DWord1, WAVal.DWord0);
	} else if(!bARO && bAWO) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Write Only Value A: %#.8x%.8x%.8x%.8x",
			WAVal.DWord3, WAVal.DWord2, WAVal.DWord1, WAVal.DWord0);
	} else {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Value A Not Used");
	}

	// Log Value B
	if(bBRO && bBWO) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Read/Write Value B: %#.8x%.8x%.8x%.8x",
			WBVal.DWord3, WBVal.DWord2, WBVal.DWord1, WBVal.DWord0);
	} else if(bBRO && !bBWO) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Read Only Value B: %#.8x%.8x%.8x%.8x",
			WBVal.DWord3, WBVal.DWord2, WBVal.DWord1, WBVal.DWord0);
	} else if(!bBRO && bBWO) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Write Only Value B: %#.8x%.8x%.8x%.8x",
			WBVal.DWord3, WBVal.DWord2, WBVal.DWord1, WBVal.DWord0);
	} else {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Value B Not Used");
	}

	// Do first write to memory
	if(!bARO && !bAWO && (bBRO || bBWO)) {
		// First write should be B value
		memcpy(Val, &WBVal, sizeof(WBVal));
	} else {
		// First write should be A value
		memcpy(Val, &WAVal, sizeof(WAVal));
	}

	do {
		// Increment iteration counter
		i++;

		// Read A value
		if(bARO) {
			memcpy(&RVal, Val, sizeof(WAVal));

			// Compare value
			if(memcmp(&RVal, &WAVal, sizeof(WAVal)) != 0) {
				bPass = FALSE;
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "MemVal: Iteration %d: Read Value A (%#.8x%.8x%.8x%.8x) != Write Value A (%#.8x%.8x%.8x%.8x)",
					i, RVal.DWord3, RVal.DWord2, RVal.DWord1, RVal.DWord0, 
					WAVal.DWord3, WAVal.DWord2, WAVal.DWord1, WAVal.DWord0);
				break;
			}
		}

		// Write B value
		if(bBWO) {
			memcpy(Val, &WBVal, sizeof(WBVal));
		}

		// Read B value
		if(bBRO) {
			memcpy(&RVal, Val, sizeof(WBVal));

			// Compare value
			if(memcmp(&RVal, &WBVal, sizeof(WBVal)) != 0) {
				bPass = FALSE;
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "MemVal: Iteration %d: Read Value B (%#.8x%.8x%.8x%.8x) != Write Value B (%#.8x%.8x%.8x%.8x)",
					i, RVal.DWord3, RVal.DWord2, RVal.DWord1, RVal.DWord0, 
					WBVal.DWord3, WBVal.DWord2, WBVal.DWord1, WBVal.DWord0);
				break;
			}
		}

		// Write A value
		if(bAWO) {
			memcpy(Val, &WAVal, sizeof(WAVal));
		}

		// Check for done
		if(TestIterations != -1) {
			if(i >= TestIterations) {
				bDone = TRUE;
			}
		}
	} while(!bDone);

	// Pass/Fail
	if(bPass) {
		xLog(gbConsoleOut, LogHandle, XLL_PASS, "MemVal: PASSED %i Iterations", i);
	} else {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "MemVal: FAILED %i Iteration", i);
	}

	// Free memory
	XPhysicalFree(Val);
}


VOID
WINAPI
MemValEndTest(VOID) {
}

#if !defined(HARNESSLIB)
/*************************************************************************************
Function:	main (...)
Purpose:	XBE Entry point
Params:
Notes:		
*************************************************************************************/
int __cdecl main() {
	HANDLE	LogHandle = NULL;

	// Do the initialization that the harness does
	// xlog
	LogHandle = xCreateLog_W(L"t:\\memval.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("MEMVAL: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		return 0;
    }

	// Get test configuration from ini file
	if (GetProfileInt(TEXT("GeneralSettings"), TEXT("StressTestMode"), 0) ||
		GetProfileInt(TEXT("GeneralSettings"), TEXT("ParallelTestMode"), 0)) {
		gbConsoleOut = FALSE;
	} else {
		xCreateConsole(NULL);
		xSetBackgroundImage(NULL);
		gbConsoleOut = TRUE;
	}

	// Start Test
	MemValStartTest(LogHandle);

	// End Test
	MemValEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("MEMVAL: End - Waiting for reboot...\n");
	Sleep(INFINITE);

	// Do the de-initialize that the harness does
	// dxconio
	xReleaseConsole();

	// xlog
	xCloseLog(LogHandle);

	return 0;
}
#endif

//
// Export function pointers of StartTest and EndTest
//
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( memval )
#pragma data_seg()

BEGIN_EXPORT_TABLE( memval )
    EXPORT_TABLE_ENTRY( "StartTest", MemValStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", MemValEndTest )
END_EXPORT_TABLE( memval )

