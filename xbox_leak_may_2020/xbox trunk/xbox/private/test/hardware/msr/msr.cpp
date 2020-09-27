/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    msr.c

Abstract:

    msr test

by:

    Jason Gould (jgould) 5 sept 2000

Environment:

    XBox

Revision History:

--*/

#include <xtl.h>
#include <stdio.h>
#include <xlog.h>
#include <xtestlib.h>
#include <dxconio.h>
#include <xlogconio.h>

VOID
WINAPI
MsrStartTest(HANDLE LogHandle)
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
 //   HANDLE atapt;

    int num;
    BOOL write;
    __int64 outval;
    int low;
    int high;
    int dobool;
    __int64 inval;
    int i;

    char buf[128];

    num = GetProfileInt(TEXT("msr"), TEXT("MsrNumber"), 0x2a);
    write = GetProfileInt(TEXT("msr"), TEXT("MsrWrite"), 0);

    *(((int*)&outval)    ) = GetProfileInt(TEXT("msr"), TEXT("MsrWriteValLow"), 0);   //low byte
    *(((int*)&outval) + 1) = GetProfileInt(TEXT("msr"), TEXT("MsrWriteValHigh"), 0);  //high byte

    low = GetProfileInt(TEXT("msr"), TEXT("MsrRangeLow"), 22);
    high = GetProfileInt(TEXT("msr"), TEXT("MsrRangeHigh"), 25);
    dobool = GetProfileInt(TEXT("msr"), TEXT("MsrBool"), 1);

	xSetOwnerAlias(LogHandle, "a-emebac");
    xSetComponent( LogHandle, "msr", "msr" );
    xSetFunctionName(LogHandle, "StartTest" );

    xStartVariation( gbConsoleOut, LogHandle, "ProcInfo" );

    if(write) {
	    xStartVariation(gbConsoleOut, LogHandle, "MSR: Write");

        __try{
            __asm {
                mov ecx, [num];
                rdmsr;
                mov dword ptr [inval], eax;
                mov dword ptr [inval+4], edx;
            }
        } __except (1) {
            xLog(gbConsoleOut, LogHandle, XLL_INFO, "MSR: could not read msr 0x%x", num);
            goto done;
        }

        inval  &= ~(((1i64 >> (high + 1 - low)) - 1) << low);   //clear bits we will change
        outval &= (1i64 >> (high + 1 - low)) - 1;               //remove "noise" from the outval
        outval <<= low;                                     //shift outval to fit in the hole in inval
        inval |= outval;                                    //merge

        __try{
            __asm {
                mov ecx, [num];
                mov eax, dword ptr [inval];
                mov edx, dword ptr [inval+4];
                wrmsr;
            }
        } __except (1) {
            xLog(gbConsoleOut, LogHandle, XLL_INFO, "MSR: could not write to msr 0x%x", num);
            goto done;
        }
		xEndVariation( LogHandle );
    }

	xStartVariation( gbConsoleOut, LogHandle, "MSR: Read");
    __try{
        __asm {
            mov ecx, [num];
            rdmsr;
            mov dword ptr [inval], eax;
            mov dword ptr [inval+4], edx;
        }
    } __except (1) {
        xLog(gbConsoleOut, LogHandle, XLL_INFO, "MSR: could not read msr 0x%x", num);
        goto done;
    }

    xLog(gbConsoleOut, LogHandle, XLL_INFO, "MSR: read of MSR 0x%x succeeded", num);

    if(dobool) {
        sprintf(buf, "Whole MSR: ");
        for(i = 63; i >= 0; i--) {
            strcat(buf, (inval & (1i64 << i)) ? "1" : "0");
        }
        xLog(gbConsoleOut, LogHandle, XLL_INFO, buf);

        if(low != 0 || high != 63) {
            sprintf(buf, "Partial MSR:");
            for(i = high; i >= low; i--) {
                strcat(buf, (inval & (1i64 << i)) ? "1" : "0");
            }
            xLog(gbConsoleOut, LogHandle, XLL_INFO, buf);
        }

    } else {
        xLog(gbConsoleOut, LogHandle, XLL_INFO, "Whole MSR: %016I64x", inval);

        if(low != 0 || high != 63) {
            xLog(gbConsoleOut, LogHandle, XLL_INFO, "Partial MSR: 0x%I64x", (inval >> low) & ((1 << (high + 1 - low)) - 1));
        }
    }

done:
    xEndVariation( LogHandle );
    return;
}


VOID
WINAPI
MsrEndTest(VOID)

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
	BOOL RunForever = TRUE;
	HANDLE	LogHandle;


	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);
	
	// xlog
	LogHandle = xCreateLog_W(L"t:\\msr.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if (LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("MSR: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
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
	MsrStartTest(LogHandle);

	// End Test
	MsrEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("MSR: End - Waiting for reboot...\n");
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
DECLARE_EXPORT_DIRECTORY( msr )
#pragma data_seg()

BEGIN_EXPORT_TABLE( msr )
    EXPORT_TABLE_ENTRY( "StartTest", MsrStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", MsrEndTest )
END_EXPORT_TABLE( msr )
