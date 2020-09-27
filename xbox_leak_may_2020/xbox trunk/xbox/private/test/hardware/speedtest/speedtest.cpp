/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    speedtest.c

Abstract:

    Routines to test CPU clock speed

Environment:

    Xbox

Revision History:

--*/
#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <xlogconio.h>

#define MAX_TRIES           200     // Maximum number of samplings
#define HIPERF_TICKS        50000
#define RDTSC               __asm _emit 0x0F __asm _emit 0x31	
#define GetCounter(pdwRet)  RDTSC __asm mov pdwRet, eax

DWORD
GetCPUSpeed(VOID) {
    LARGE_INTEGER liBegin, liEnd, liFreq;   // High Resolution Performance Counter frequency
    int iTries = 0;
    DWORD dwCycles, dwStamp0;               // Time Stamp Variable for beginning and end 
    DWORD dwStamp1, dwFreq = 0, dwFreq2 = 1, dwFreq3 = 2, dwTicks;
    int iPriority;
    HANDLE hThread = GetCurrentThread();

    // Must have a high resolution counter.
    if ( !QueryPerformanceFrequency(&liFreq) ) {
        return 0;
    }

    // Loop until all three frequencies match or we exeed MAX_TRIES.
    for ( iTries=0; (dwFreq != dwFreq2 || dwFreq != dwFreq3) && iTries < MAX_TRIES; iTries++ ) {           

        // Shift frequencies back to make room for new frequency.
        dwFreq3 = dwFreq2;  
        dwFreq2 = dwFreq;   

        iPriority = GetThreadPriority(hThread);

        if ( iPriority != THREAD_PRIORITY_ERROR_RETURN ) {
            SetThreadPriority( hThread, THREAD_PRIORITY_TIME_CRITICAL );
        }

        // Reset the initial time.
        QueryPerformanceCounter( &liBegin );
        GetCounter( dwStamp0 ); 

        do {
            // Loop until the hiperf timer has ticked HIPERF_TICKS ticks 
            // This allows for elapsed time for sampling.
            QueryPerformanceCounter( &liEnd );
            GetCounter( dwStamp1 );
        } while ( (DWORD)liEnd.LowPart - (DWORD)liBegin.LowPart < HIPERF_TICKS );

        // Reset priority
        if ( iPriority != THREAD_PRIORITY_ERROR_RETURN ) {
            SetThreadPriority( hThread, iPriority );
        }

        // Number of internal clock cycles is difference between 
        //   two time stamp readings.
        dwCycles = dwStamp1 - dwStamp0; 

        // Number of external ticks is difference between two
        //   hi-res counter reads.
        dwTicks = (DWORD)liEnd.LowPart - (DWORD)liBegin.LowPart;  
    
        dwFreq = \
            (DWORD) ((((float) dwCycles * (float) liFreq.LowPart) / 
                (float) dwTicks) / 100000.0f);
    } 

    // dwFreq is currently in this form: 4338 (433.8 MHz)
    // Take any fraction up to the next round number.
    dwFreq = (dwFreq + (dwFreq % 10)) / 10;

    return dwFreq;
}


VOID
WINAPI
SpeedTestStartTest(HANDLE LogHandle) {
	xSetOwnerAlias(LogHandle, "a-emebac");
    xSetComponent( LogHandle, "Hardware", "SpeedTest" );
    xSetFunctionName( LogHandle, "SpeedTest" );
	xStartVariation(gbConsoleOut, LogHandle, "SPEEDTEST");
    xLog(gbConsoleOut, LogHandle, XLL_PASS, "SPEEDTEST: CPU Speed = %d MHz", GetCPUSpeed());
	xEndVariation(LogHandle);
}


VOID
WINAPI
SpeedTestEndTest(VOID) {
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
	LogHandle = xCreateLog_W(L"t:\\speedtest.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if (LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("SPEEDTEST: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
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
	SpeedTestStartTest(LogHandle);

	// End Test
	SpeedTestEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("SPEEDTEST: End - Waiting for reboot...\n");
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
DECLARE_EXPORT_DIRECTORY( speedtest )
#pragma data_seg()

BEGIN_EXPORT_TABLE( speedtest )
    EXPORT_TABLE_ENTRY( "StartTest", SpeedTestStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", SpeedTestEndTest )
END_EXPORT_TABLE( speedtest )
