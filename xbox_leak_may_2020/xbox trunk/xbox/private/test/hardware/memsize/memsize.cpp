/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    memsize.c

Abstract:

    Routines to get memory size from various APIs

Environment:

    Xbox

Revision History:

	Added console output via dxconio.lib

--*/
#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <xlogconio.h>

VOID
WINAPI
MemSizeStartTest(HANDLE LogHandle) {
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatus( &ms );

	xSetOwnerAlias(LogHandle, "a-emebac");
    xSetComponent(LogHandle, "Hardware", "memsize");
    xSetFunctionName(LogHandle, "GlobalMemoryStatus");
    xStartVariation(gbConsoleOut, LogHandle, "MEMSIZE: GlobalMemoryStatus");
    xLog( gbConsoleOut, LogHandle, XLL_PASS, "MEMSIZE: GlobalMemoryStatus returns total memory = %lu bytes", ms.dwTotalPhys );
    xEndVariation( LogHandle );
}


VOID
WINAPI
MemSizeEndTest(VOID) {
}

#if !defined(HARNESSLIB)
/*************************************************************************************
Function:	main (...)
Purpose:	XBE Entry point
Params:
Notes:		
*************************************************************************************/
int __cdecl main() {
	BOOL RunForever = TRUE;
	HANDLE	LogHandle;


	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);
	
	// xlog
	LogHandle = xCreateLog_W(L"t:\\memsize.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if (LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("MEMSIZE: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		return 0;
    }

	// Get test configuration from ini file
	if (GetProfileInt(TEXT("GeneralSettings"), TEXT("StressTestMode"), 0) ||
		GetProfileInt(TEXT("GeneralSettings"), TEXT("ParallelTestMode"), 0)) {
		gbConsoleOut = FALSE;
	} else {
		gbConsoleOut = TRUE;
	}

	// Start Test
	MemSizeStartTest(LogHandle);

	// End Test
	MemSizeEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("MEMSIZE: End - Waiting for reboot...\n");
	while (RunForever) {
	}

	// Do the de-initialize that the harness does
	// dxconio
	xReleaseConsole();

	return 0;
}
#endif
//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( memsize )
#pragma data_seg()

BEGIN_EXPORT_TABLE( memsize )
    EXPORT_TABLE_ENTRY( "StartTest", MemSizeStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", MemSizeEndTest )
END_EXPORT_TABLE( memsize )
