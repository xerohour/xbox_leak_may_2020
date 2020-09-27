/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    memtest.c

Abstract:

    Routines to test memory

Environment:

    Xbox

Revision History:

Dev Notes:
	Data Checking:
		Write/Read/Verify 0x00, 0x55, 0xaa, 0xff

	Address Checking:
		Write address as value to each memory location.
		Read and verify contents.

	Walk ones:

	Walk zeros:

--*/

#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <xlogconio.h>

#define PAGE_SIZE 0x1000

//
// Prototypes and Typedefs from mm.h ke.h
//
extern "C" {
	ULONG_PTR
	WINAPI
	MmGetPhysicalAddress (
		IN PVOID BaseAddress
		);
}

struct MyInt128 {
	unsigned __int64 LowPart;
	unsigned __int64 HighPart;
};


//
// Function:	DataTest(...)
// Purpose:		Test memory data storage by writing and verifying the following data values
//				0x00, 0x55, 0xaa, 0xff
// Params:		
//				LogHandle	Handle for xLog
//				StartAddr	Pointer to starting address of memory to test
//				EndAddr		Pointer to ending address of memory to test
// Notes:		
//
BOOL
DataTest(HANDLE LogHandle, MyInt128 *StartAddr, MyInt128 *EndAddr) {
	MyInt128 *Addr, Value[4], R_Value;
	int i;
	BOOL bPass = TRUE;

	// Init Value Array
	Value[0].HighPart = 0x0000000000000000;
	Value[0].LowPart  = 0x0000000000000000;
	Value[1].HighPart = 0x5555555555555555;
	Value[1].LowPart  = 0x5555555555555555;
	Value[2].HighPart = 0xaaaaaaaaaaaaaaaa;
	Value[2].LowPart  = 0xaaaaaaaaaaaaaaaa;
	Value[3].HighPart = 0xffffffffffffffff;
	Value[3].LowPart  = 0xffffffffffffffff;

	Addr = StartAddr;
	do {
		// Check each value
		for(i = 0; i < 4; i++) {
			// Write memory location
			memcpy((void *)Addr, &Value[i], sizeof(MyInt128));

			// Read memory location
			memcpy(&R_Value, (void *)Addr, sizeof(MyInt128));

			// Test value
			if(memcmp(&R_Value, &Value[i], sizeof(MyInt128)) != 0) {
				bPass = FALSE;
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
						"DataTest FAILED at %p (physical = %p), Wrote: %#.16I64x%.16I64x, Read: %#.16I64x%.16I64x",
						Addr, MmGetPhysicalAddress(Addr), Value[i].HighPart, Value[i].LowPart, R_Value.HighPart, R_Value.LowPart);
				break;
			}
		}
		// Increment Address
		Addr++;

	} while((Addr <= EndAddr) & bPass);
	return bPass;
}
  

//
// Function:	AddressTest(...)
// Purpose:		Test memory addressing by writing and verifying the value of the address
//				to each location.

BOOL
AddressTest(HANDLE LogHandle, MyInt128 *StartAddr, MyInt128 *EndAddr) {
	MyInt128 *Addr, Value, R_Value;
	__int32 *ValueAddr;
	BOOL bPass = TRUE;

	// Write the address as the value
	for(Addr = StartAddr; Addr <= EndAddr; Addr++){
		// Fill Value with address
		ValueAddr = (__int32 *)&Value;
		*(ValueAddr) = (__int32)Addr;
		*(ValueAddr + 1) = (__int32)Addr;
		*(ValueAddr + 2) = (__int32)Addr;
		*(ValueAddr + 3) = (__int32)Addr;

		// Write Value
		memcpy((void *)Addr, &Value, sizeof(MyInt128));
	}

	// Read and Verify contents
	for(Addr = StartAddr; Addr <= EndAddr; Addr++){
		// Fill Value with address
		ValueAddr = (__int32 *)&Value;
		*(ValueAddr) = (__int32)Addr;
		*(ValueAddr + 1) = (__int32)Addr;
		*(ValueAddr + 2) = (__int32)Addr;
		*(ValueAddr + 3) = (__int32)Addr;

		// Read Value
		memcpy(&R_Value, (void *)Addr, sizeof(MyInt128));

		// Verify Value
		if(memcmp(&R_Value, &Value, sizeof(MyInt128)) != 0) {
			bPass = FALSE;
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
					"AddressTest FAILED at %p (physical = %p), Wrote: %#.16I64x%.16I64x, Read: %#.16I64x%.16I64x",
						Addr, MmGetPhysicalAddress(Addr), Value.HighPart, Value.LowPart, R_Value.HighPart, R_Value.LowPart);
			break;
		}
	}
	return bPass;
}


//
// Function:	WalkOnes(...)
// Purpose:		Test memory addressing by walking a "1" through a memory location
//				Only test 8 locations
//
BOOL
WalkOnes(HANDLE LogHandle, MyInt128 *StartAddr, MyInt128 *EndAddr) {
	MyInt128 *Addr, Value, R_Value;
	int i, j, iNumAddr;
	BOOL bPass = TRUE;

	// Number of MyInt128s
	iNumAddr = EndAddr - StartAddr + 1;
	iNumAddr /= 8;

	Value.HighPart = (unsigned __int64)0;
	Value.LowPart = (unsigned __int64)0;

	for(i = 0; i <= 8; i++) {
		// Set Address
		Addr = StartAddr + i*iNumAddr;
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "WalkOnes at %p (physical = %p)", Addr, MmGetPhysicalAddress(Addr));
		for(j = 0; j < 128; j++) {
			// Shift the "1"
			if(j == 0) {
				Value.HighPart = (unsigned __int64)0;
				Value.LowPart = (unsigned __int64)1;
			} else if((j > 0) & (j < 64)) {
				Value.HighPart = (unsigned __int64)0;
				Value.LowPart <<= (unsigned __int64)1;
			} else if(j == 64) {
				Value.HighPart = (unsigned __int64)1;
				Value.LowPart = (unsigned __int64)0;
			} else {			
				Value.HighPart <<= (unsigned __int64)1;
				Value.LowPart = (unsigned __int64)0;
			}

			// Write Value
			memcpy((void *)Addr, &Value, sizeof(MyInt128));

			// Read Value
			memcpy(&R_Value, (void *)Addr, sizeof(MyInt128));
			
			/*
			xLog(gbConsoleOut, LogHandle, XLL_INFO, 
				"WalkOnes at %p (physical = %p), Wrote: %#.16I64x%.16I64x, Read: %#.16I64x%.16I64x",
							Addr, MmGetPhysicalAddress(Addr), Value.HighPart, Value.LowPart, R_Value.HighPart, R_Value.LowPart);
			*/

			// Verify Value
			if(memcmp(&R_Value, &Value, sizeof(MyInt128)) != 0) {
				bPass = FALSE;
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
					"WalkOnes FAILED at %p (physical = %p), Wrote: %#.16I64x%.16I64x, Read: %#.16I64x%.16I64x",
							Addr, MmGetPhysicalAddress(Addr), Value.HighPart, Value.LowPart, R_Value.HighPart, R_Value.LowPart);
				break;
			}
		}
	}
	return bPass;
}


//
// Function:	WalkZeros()
// Purpose:		Test memory addressing by walking a "0" through a memory location
//				Only test 8 locations
//
BOOL
WalkZeros(HANDLE LogHandle, MyInt128 *StartAddr, MyInt128 *EndAddr) {
	MyInt128 *Addr, Value, R_Value;
	int i, j, iNumAddr;
	BOOL bPass = TRUE;

	// Number of MyInt128s
	iNumAddr = EndAddr - StartAddr + 1;
	iNumAddr /= 8;

	Value.HighPart = (unsigned __int64)0;
	Value.LowPart = (unsigned __int64)0;

	for(i = 0; i <= 8; i++) {
		// Set Address
		Addr = StartAddr + i*iNumAddr;
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "WalkZeros at %p (physical = %p)", Addr, MmGetPhysicalAddress(Addr));
		for(j = 0; j < 128; j++) {
			// Shift the "0"
			if(j == 0) {
				Value.HighPart = (unsigned __int64)~(0);
				Value.LowPart = (unsigned __int64)~(1);
			} else if((j > 0) & (j < 64)) {
				Value.HighPart = (unsigned __int64)~(0);
				Value.LowPart = (unsigned __int64)~(~(Value.LowPart)<<1);
			} else if(j == 64) {
				Value.HighPart = (unsigned __int64)~(1);
				Value.LowPart = (unsigned __int64)~(0);
			} else {			
				Value.HighPart = (unsigned __int64)~(~(Value.HighPart)<<1);
				Value.LowPart = (unsigned __int64)~(0);
			}

			// Write Value
			memcpy((void *)Addr, &Value, sizeof(MyInt128));

			// Read Value
			memcpy(&R_Value, (void *)Addr, sizeof(MyInt128));

			/*
			xLog(gbConsoleOut, LogHandle, XLL_INFO, 
				"WalkZeros at %p (physical = %p), Wrote: %#.16I64x%.16I64x, Read: %#.16I64x%.16I64x",
							Addr, MmGetPhysicalAddress(Addr), Value.HighPart, Value.LowPart, R_Value.HighPart, R_Value.LowPart);
			*/

			// Verify Value
			if(memcmp(&R_Value, &Value, sizeof(MyInt128)) != 0) {
				bPass = FALSE;
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
					"WalkZeros FAILED at %p (physical = %p), Wrote: %#.16I64x%.16I64x, Read: %#.16I64x%.16I64x",
							Addr, MmGetPhysicalAddress(Addr), Value.HighPart, Value.LowPart, R_Value.HighPart, R_Value.LowPart);
				break;
			}
		}
	}
	return bPass;
}


VOID
WINAPI
MemTestStartTest(HANDLE LogHandle) {
    SIZE_T TestableMemSize, PercentAvail;
    MEMORYSTATUS ms;
	MyInt128 *StartAddr, *EndAddr;
	BOOL DoDataTest, DoAddressTest, DoWalkOnes, DoWalkZeros, UseWriteCombine, bDone;
	ULONG PhysicalAddress, Alignment;
	DWORD Protect;
	int TestIterations, i;

	// Initialize
	i = 0;
	bDone = FALSE;

	DoDataTest = GetProfileInt(TEXT("memtest"), TEXT("DataTest"), 1);
	DoAddressTest = GetProfileInt(TEXT("memtest"), TEXT("AddressTest"), 1);
	DoWalkOnes = GetProfileInt(TEXT("memtest"), TEXT("WalkOnes"), 1);
	DoWalkZeros = GetProfileInt(TEXT("memtest"), TEXT("WalkZeros"), 1);
	TestIterations = GetProfileInt(TEXT("memtest"), TEXT("TestIterations"), 1);
	PercentAvail = GetProfileInt(TEXT("memtest"), TEXT("PercentAvail"), 80);
	UseWriteCombine = GetProfileInt(TEXT("memtest"), TEXT("UseWriteCombine"), 0);

	xSetOwnerAlias(LogHandle, "a-emebac");
    xSetComponent(LogHandle, "Hardware", "MemTest");
    xSetFunctionName(LogHandle, "MemTest");
	xStartVariation( gbConsoleOut, LogHandle, "Initialization" );

	// Query global memory status
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatus( &ms );

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"MEMTEST: GlobalMemoryStatus returned %u bytes of %u bytes available", 
		ms.dwAvailPhys, ms.dwTotalPhys);

	// Allocate specified % of available memory
	TestableMemSize = (SIZE_T)((double)ms.dwAvailPhys*(double)PercentAvail/100.0);

	// Round to neareast int128
	TestableMemSize -= TestableMemSize%(sizeof(MyInt128));

	// Start at any physical address
	PhysicalAddress = MAXULONG_PTR;

	// Default alignment (PAGE)
	Alignment = 0;

	// Memory Protection
	if(UseWriteCombine) {
		Protect = PAGE_READWRITE | PAGE_WRITECOMBINE;
	} else {
		Protect = PAGE_READWRITE | PAGE_NOCACHE;
	}

	// Reduce TestableMemSize a page at a time until memory can be allocated
    for (;;) {
		StartAddr = (MyInt128 *)XPhysicalAlloc(TestableMemSize, PhysicalAddress, Alignment, Protect);
        if ( StartAddr ) {
            break;
        }
        if ( TestableMemSize <= PAGE_SIZE ) {
            break;
        }
        TestableMemSize -= PAGE_SIZE;
    }

    if ( !StartAddr ) {
        xLog(gbConsoleOut, LogHandle, XLL_BLOCK, "MemTest: unable to allocate memory (size=%lu, error=%s)",
            TestableMemSize, WinErrorSymbolicName(GetLastError()));
        return;
    }

	EndAddr = StartAddr + TestableMemSize/sizeof(MyInt128) - sizeof(MyInt128);

	xLog(gbConsoleOut, LogHandle, XLL_INFO, "MemTest: %lu bytes allocated as %s", 
		TestableMemSize, UseWriteCombine ? "PAGE_WRITECOMBINE" : "PAGE_NOCACHE");
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "MemTest: Starting Address %p (physical %p)",
		StartAddr, MmGetPhysicalAddress(StartAddr));
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "MemTest: Ending Address %p (physical %p)",
		EndAddr, MmGetPhysicalAddress(EndAddr));

	xEndVariation(LogHandle);

	do {
		// Increment iteration counter
		i++;

		// DataTest
		if(DoDataTest) {
			xStartVariation(gbConsoleOut, LogHandle, "DataTest");
			if(DataTest(LogHandle, StartAddr, EndAddr)) {
				xLog(gbConsoleOut, LogHandle, XLL_PASS, "MemTest: DataTest: PASSED Iteration %d", i);
			} else {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "MemTest: DataTest: FAILED Iteration %d", i);
				break;
			}
			xEndVariation(LogHandle);
		}

		// AddressTest
		if(DoAddressTest) {
			xStartVariation(gbConsoleOut, LogHandle, "AddressTest");
			if(AddressTest(LogHandle, StartAddr, EndAddr)) {
				xLog(gbConsoleOut, LogHandle, XLL_PASS, "MemTest: AddressTest: PASSED Iteration %d", i);
			} else {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "MemTest: AddressTest: FAILED Iteration %d", i);
				break;
			}
			xEndVariation(LogHandle);
		}

		// WalkOnes
		if(DoWalkOnes) {
			xStartVariation(gbConsoleOut, LogHandle, "WalkOnes");
			if(WalkOnes(LogHandle, StartAddr, EndAddr)) {
				xLog(gbConsoleOut, LogHandle, XLL_PASS, "MemTest: WalkOnes: PASSED Iteration %d", i);
			} else {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "MemTest: WalkOnes: FAILED Iteration %d", i);
				break;
			}
			xEndVariation(LogHandle);
		}

		// WalkZeros
		if (DoWalkZeros) {
			xStartVariation(gbConsoleOut, LogHandle, "WalkZeros");
			if(WalkZeros(LogHandle, StartAddr, EndAddr)) {
				xLog(gbConsoleOut, LogHandle, XLL_PASS, "MemTest: WalkZeros: PASSED Iteration %d", i);
			} else {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "MemTest: WalkZeros: FAILED Iteration %d", i);
				break;
			}
			xEndVariation(LogHandle);
		}

		
		// Check for done
		if(TestIterations != -1) {
			if(i >= TestIterations) {
				bDone = TRUE;
			}
		}
	} while (!bDone);
    

	// Free memory
 	XPhysicalFree(StartAddr);
}


VOID
WINAPI
MemTestEndTest() {
}

#if !defined(HARNESSLIB)
/*************************************************************************************
Function:	main (...)
Purpose:	XBE Entry point
Params:
Notes:		
*************************************************************************************/
int __cdecl main() {
	HANDLE	LogHandle;
	
	// xlog
	LogHandle = xCreateLog_W(L"t:\\memtest.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if (LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("MEMTEST: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
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
	MemTestStartTest(LogHandle);

	// End Test
	MemTestEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("MemTest: End - Waiting for reboot...\n");
	Sleep(INFINITE);

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
DECLARE_EXPORT_DIRECTORY( memtest )
#pragma data_seg()

BEGIN_EXPORT_TABLE( memtest )
    EXPORT_TABLE_ENTRY( "StartTest", MemTestStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", MemTestEndTest )
END_EXPORT_TABLE( memtest )
