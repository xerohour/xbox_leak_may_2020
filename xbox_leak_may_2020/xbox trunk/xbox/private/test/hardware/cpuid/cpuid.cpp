/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    cpuid.cpp

Abstract:

    cpuid test 

by:

    Jason Gould (jgould) 5 sept 2000

Environment:

    XBox

Revision History:

	Changed to stand alone xbox app.
	Added screeen output using dxconio.lib

--*/

#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <xlogconio.h>

union Features
{
	DWORD reg;
	struct
	{
		DWORD FPU:1;
		DWORD VME:1;
		DWORD DE:1;
		DWORD PSE:1;
		DWORD TSC:1;
		DWORD MSR:1;
		DWORD PAE:1;
		DWORD MCE:1;
		DWORD CXS:1;
		DWORD APIC:1;
		DWORD RES1:2;
		DWORD MTRR:1;
		DWORD PGE:1;
		DWORD MCA:1;
		DWORD CMOV:1;
		DWORD WHOKNOWS:7;
		DWORD MMX:1;
		DWORD FXSR:1;
		DWORD XMM:1;
		DWORD WNI:1;	//bit 26
	} p_reg;
};


char* features[] = {
	"Floating-Point Unit",
	"Virtual 8086 Mode",
	"Debugging Extensions",
	"Page Size Extensions",
	"Time Stamp Counter",
	"Machine Specific Registers",
	"Physical Address Extension",
	"Machine Check Excption",
	"CMPXCHG8B Instruction",
	"APIC",
	"Reserved",
	"Fast System Call",
	"Memory Type Range Registers",
	"PTE Global Flag",
	"Machine Check ARchitecture",
	"Conditional Move and Compare Instructions",
	"Page Attribute Table",
	"36-byte Page Size Extension",
	"Processor Number",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"MMX Technology",
	"Fast FP/MMX/XMM Save/Restore",
	"XMM/Streaming SIMD Instructions"
};

char * proctypes[] = {
	"Origonal OEM Processor",
	"Intel Overdrive Processor",
	"Duel-Proc",
	"Reserved"
};

VOID
WINAPI
CpuIDStartTest(HANDLE LogHandle)
/*++

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

--*/ 
{
    // HANDLE atapt;
	DWORD a, d;
	int i;

    xSetComponent( LogHandle, "Harness", "cpuid" );
    xSetFunctionName(LogHandle, "StartTest" );

	_asm {
		xor eax, eax
		inc eax
		cpuid
		mov [a], eax
		mov [d], edx
	}

    xStartVariation(gbConsoleOut, LogHandle, "CPUID: ProcInfo");
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Processor Info: %08x", a);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Stepping ID: %d", a & 15);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Model: %d", ((a >> 4) & 15));
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Family: %d", ((a >> 8) & 15));
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Processor Type: %s", proctypes[(a >> 12) & 3]);

    xEndVariation(LogHandle);

    xStartVariation(gbConsoleOut, LogHandle, "CPUID: ProcFeatures");
	
	for(i = 0; i < sizeof(features) / sizeof(features[0]); i++) {
		if(d & (1 << i)) {
			xLog(gbConsoleOut, LogHandle, XLL_INFO, "%s Supported", features[i]);
		} else {
			xLog(gbConsoleOut, LogHandle, XLL_INFO, "%s NOT Supported", features[i]);
		}
	}

    xEndVariation(LogHandle);

    return;
}


VOID
WINAPI
CpuIDEndTest()
/*++

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

--*/ 
{
}

#if !defined(HARNESSLIB)
/*************************************************************************************
Function:	main (...)
Purpose:	XBE Entry point
Params:
Notes:		
*************************************************************************************/
int __cdecl main() {
	HANDLE LogHandle;
	BOOL RunForever = TRUE;

	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);
	
	// xlog
	LogHandle = xCreateLog_W(L"t:\\cpuid.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if (LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("CPUID: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		return 0;
    }
	xSetOwnerAlias(LogHandle, "a-emebac");

	// Get test configuration from ini file
	if (GetProfileInt(TEXT("GeneralSettings"), TEXT("StressTestMode"), 0) ||
		GetProfileInt(TEXT("GeneralSettings"), TEXT("ParallelTestMode"), 0)) {
		gbConsoleOut = FALSE;
	} else {
		gbConsoleOut = TRUE;
	}

	// Start Test
	CpuIDStartTest(LogHandle);

	// End Test
	CpuIDEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("CPUID: End - Waiting for reboot...\n");
	while (RunForever) {
	}

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
DECLARE_EXPORT_DIRECTORY( cpuid )
#pragma data_seg()

BEGIN_EXPORT_TABLE( cpuid )
    EXPORT_TABLE_ENTRY( "StartTest", CpuIDStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", CpuIDEndTest )
END_EXPORT_TABLE( cpuid )
