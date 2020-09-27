/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    mempat.cpp

Abstract:

    Memory pattern test based on MATS. Designed to catch coupling faults by stepping up and down
	memory while alternating reads and writes.

Environment:

    Xbox

Revision History:

--*/

#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <xlogconio.h>

#define PAGE_SIZE 0x00001000

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


// Globals
BOOL	gbFailed = FALSE;


//
// Function:	WUp128(...)
// Purpose:		Step up through memory writing a 128 bit value
// Params:		
//				LogHandle	Handle for xLog
//				StartAddr	Starting address of memory to test
//				EndAddr		Ending address of memory to test, adjusted for data width
//				W_Value		Value to write
// Notes:		
//
BOOL
WUp128(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr,
	  unsigned __int64 W_Value1, unsigned __int64 W_Value2) {

	volatile MyInt128 *loc;
	MyInt128 W_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, "WUp128:   Write Value: %#.16I64x%.16I64x", W_Value1, W_Value2);

	W_Value.LowPart = W_Value2;
	W_Value.HighPart = W_Value1;

	
	for(loc = (MyInt128 *)StartAddr; loc <= (MyInt128 *)EndAddr; loc++) {
				memcpy((void *)loc, &W_Value, sizeof(W_Value));
		}
	return TRUE;
}

//
// Function:	RWUp128(...)
// Purpose:		Step up through memory reading an expected value and writing a 128 bit value
//
BOOL
RWUp128(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int64 Exp_Value1, unsigned __int64 Exp_Value2, 
		unsigned __int64 W_Value1, unsigned __int64 W_Value2) {

	volatile MyInt128 *loc;
	MyInt128 Exp_Value, R_Value, W_Value;

	Exp_Value.LowPart = Exp_Value2;
	Exp_Value.HighPart = Exp_Value1;
	W_Value.LowPart = W_Value2;
	W_Value.HighPart = W_Value1;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RWUp128:   Expected Value: %#.16I64x%.16I64x, Write Value: %#.16I64x%.16I64x", 
		Exp_Value.HighPart, Exp_Value.LowPart, W_Value.HighPart, W_Value.LowPart);

	for(loc = (MyInt128 *)StartAddr; loc <= (MyInt128 *)EndAddr; loc++) {
		// Read expected value
		memcpy(&R_Value,  (void *)loc, sizeof(R_Value));
		if(memcmp(&Exp_Value, &R_Value, sizeof(R_Value)) != 0) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RWUp128:   FAILED at %p (physical = %p), Read: %#.16I64x%.16I64x, Expected: %#.16I64x%.16I64x",
						loc, MmGetPhysicalAddress((void *)loc), 
						R_Value.HighPart, R_Value.LowPart, Exp_Value.HighPart, Exp_Value.LowPart);
			return FALSE;
		}

		// Write new value	
		memcpy((void *)loc, &W_Value, sizeof(W_Value));
	}
	return TRUE;
}	

//
// Function:	RWDown128(...)
// Purpose:		Step down through memory reading an expected value and writing a 128 bit value
//
BOOL
RWDown128(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int64 Exp_Value1, unsigned __int64 Exp_Value2, 
		unsigned __int64 W_Value1, unsigned __int64 W_Value2) {

	volatile MyInt128 *loc;
	MyInt128 Exp_Value, R_Value, W_Value;

	Exp_Value.LowPart = Exp_Value2;
	Exp_Value.HighPart = Exp_Value1;
	W_Value.LowPart = W_Value2;
	W_Value.HighPart = W_Value1;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RWDown128: Expected Value: %#.16I64x%.16I64x, Write Value: %#.16I64x%.16I64x", 
		Exp_Value.HighPart, Exp_Value.LowPart, W_Value.HighPart, W_Value.LowPart);

	for(loc = (MyInt128 *)EndAddr; loc >= (MyInt128 *)StartAddr; loc--) {
		// Read expected value
		memcpy(&R_Value, (void *)loc, sizeof(R_Value));
		if(memcmp(&Exp_Value, &R_Value, sizeof(R_Value)) != 0) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RWDown128: FAILED at %p (physical = %p), Read: %#.16I64x%.16I64x, Expected: %#.16I64x%.16I64x",
						loc, MmGetPhysicalAddress((void *)loc), 
						R_Value.HighPart, R_Value.LowPart, Exp_Value.HighPart, Exp_Value.LowPart);
			return FALSE;
		}

		// Write new value	
		memcpy((void *)loc, &W_Value, sizeof(W_Value));
	}
	return TRUE;
}	
	
//
// Function:	RDown128(...)
// Purpose:		Step down through memory reading an expected value
//
BOOL
RDown128(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int64 Exp_Value1, unsigned __int64 Exp_Value2) {

	volatile MyInt128 *loc;
	MyInt128 Exp_Value, R_Value;

	Exp_Value.LowPart = Exp_Value2;
	Exp_Value.HighPart = Exp_Value1;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RDown128:  Expected Value: %#.16I64x%.16I64x", Exp_Value.HighPart, Exp_Value.LowPart);

	for(loc = (MyInt128 *)EndAddr; loc >= (MyInt128 *)StartAddr; loc--) {
		// Read expected value
		memcpy(&R_Value, (void *)loc, sizeof(R_Value));
		if(memcmp(&Exp_Value, &R_Value, sizeof(R_Value)) != 0) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RDown128:  FAILED at %p (physical = %p), Read: %#.16I64x%.16I64x, Expected: %#.16I64x%.16I64x",
						loc, MmGetPhysicalAddress((void *)loc), R_Value.HighPart, R_Value.LowPart, 
						Exp_Value.HighPart, Exp_Value.LowPart);
			return FALSE;
		}
	}
	return TRUE;
}	

//
// Function:	WUp64(...)
// Purpose:		Step up through memory writing a 64 bit value
// Params:		
//				LogHandle	Handle for xLog
//				StartAddr	Starting address of memory to test
//				EndAddr		Ending address of memory to test, adjusted for data width
//				W_Value		Value to write
// Notes:		
//
BOOL
WUp64(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr,
	  unsigned __int64 W_Value) {

	volatile unsigned __int64 *loc;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, "WUp64:    Write Value: %#.16I64x", W_Value);
	
	for(loc = (unsigned __int64 *)StartAddr; loc <= (unsigned __int64 *)EndAddr; loc++) {
				*loc = W_Value;
		}
	return TRUE;
}

//
// Function:	RWUp64(...)
// Purpose:		Step up through memory reading an expected value and writing a 64 bit value
//
BOOL
RWUp64(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int64 Exp_Value, unsigned __int64 W_Value) {

	volatile unsigned __int64 *loc;
	unsigned __int64 R_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RWUp64:   Expected Value: %#.16I64x, Write Value: %#.16I64x", Exp_Value, W_Value);

	for(loc = (unsigned __int64 *)StartAddr; loc <= (unsigned __int64 *)EndAddr; loc++) {
		// Read expected value
		R_Value = *loc;
		if(Exp_Value != R_Value) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RWUp64:   FAILED at %p (physical = %p), Read: %#.16I64x, Expected: %#.16I64x",
						loc, MmGetPhysicalAddress((void *)loc), R_Value, Exp_Value);
			return FALSE;
		}

		// Write new value	
		*loc = W_Value;
	}
	return TRUE;
}	

//
// Function:	RWDown64(...)
// Purpose:		Step down through memory reading an expected value and writing a 64 bit value
//
BOOL
RWDown64(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int64 Exp_Value, unsigned __int64 W_Value) {

	volatile unsigned __int64 *loc;
	unsigned __int64 R_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RWDown64: Expected Value: %#.16I64x, Write Value: %#.16I64x", Exp_Value, W_Value);

	for(loc = (unsigned __int64 *)EndAddr; loc >= (unsigned __int64 *)StartAddr; loc--) {
		// Read expected value
		R_Value = *loc;
		if(Exp_Value != R_Value) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RWDown64: FAILED at %p (physical = %p), Read: %#.16I64x, Expected: %#.16I64x",
						loc, MmGetPhysicalAddress((void *)loc), R_Value, Exp_Value);
			return FALSE;
		}

		// Write new value	
		*loc = W_Value;
	}
	return TRUE;
}	
	
//
// Function:	RDown64(...)
// Purpose:		Step down through memory reading an expected value
//
BOOL
RDown64(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int64 Exp_Value) {

	volatile unsigned __int64 *loc;
	unsigned __int64 R_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RDown64:  Expected Value: %#.16I64x", Exp_Value);

	for(loc = (unsigned __int64 *)EndAddr; loc >= (unsigned __int64 *)StartAddr; loc--) {
		// Read expected value
		R_Value = *loc;
		if(Exp_Value != R_Value) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RDown64:  FAILED at %p (physical = %p), Read: %#.16I64x, Expected: %#.16I64x",
				loc, MmGetPhysicalAddress((void *)loc), R_Value, Exp_Value);
			return FALSE;
		}
	}
	return TRUE;
}	

//
// Function:	WUp32(...)
// Purpose:		Step up through memory writing a 32 bit value
// Params:		
//				LogHandle	Handle for xLog
//				StartAddr	Starting address of memory to test
//				EndAddr		Ending address of memory to test, adjusted for data width
//				W_Value		Value to write
// Notes:		
//
BOOL
WUp32(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr,
	  unsigned __int32 W_Value) {

	volatile unsigned __int32 *loc;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, "WUp32:    Write Value: %#.8x", W_Value);
	
	for(loc = (unsigned __int32 *)StartAddr; loc <= (unsigned __int32 *)EndAddr; loc++) {
				*loc = W_Value;
		}
	return TRUE;
}

//
// Function:	RWUp32(...)
// Purpose:		Step up through memory reading an expected value and writing a 32 bit value
//
BOOL
RWUp32(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int32 Exp_Value, unsigned __int32 W_Value) {

	volatile unsigned __int32 *loc;
	unsigned __int32 R_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RWUp32:   Expected Value: %#.8x, Write Value: %#.8x", Exp_Value, W_Value);

	for(loc = (unsigned __int32 *)StartAddr; loc <= (unsigned __int32 *)EndAddr; loc++) {
		// Read expected value
		R_Value = *loc;
		if(Exp_Value != R_Value) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RWUp32:   FAILED at %p (physical = %p), Read: %#.8x, Expected: %#.8x",
						loc, MmGetPhysicalAddress((void *)loc), R_Value, Exp_Value);
			return FALSE;
		}

		// Write new value	
		*loc = W_Value;
	}
	return TRUE;
}	

//
// Function:	RWDown32(...)
// Purpose:		Step down through memory reading an expected value and writing a 32 bit value
//
BOOL
RWDown32(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int32 Exp_Value, unsigned __int32 W_Value) {

	volatile unsigned __int32 *loc;
	unsigned __int32 R_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RWDown32: Expected Value: %#.8x, Write Value: %#.8x", Exp_Value, W_Value);

	for(loc = (unsigned __int32 *)EndAddr; loc >= (unsigned __int32 *)StartAddr; loc--) {
		// Read expected value
		R_Value = *loc;
		if(Exp_Value != R_Value) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RWDown32: FAILED at %p (physical = %p), Read: %#.8x, Expected: %#.8x",
						loc, MmGetPhysicalAddress((void *)loc), R_Value, Exp_Value);
			return FALSE;
		}

		// Write new value	
		*loc = W_Value;
	}
	return TRUE;
}	
	
//
// Function:	RDown32(...)
// Purpose:		Step down through memory reading an expected value
//
BOOL
RDown32(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int32 Exp_Value) {

	volatile unsigned __int32 *loc;
	unsigned __int32 R_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RDown32:  Expected Value: %#.8x", Exp_Value);

	for(loc = (unsigned __int32 *)EndAddr; loc >= (unsigned __int32 *)StartAddr; loc--) {
		// Read expected value
		R_Value = *loc;
		if(Exp_Value != R_Value) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RDown32:  FAILED at %p (physical = %p), Read: %#.8x, Expected: %#.8x",
				loc, MmGetPhysicalAddress((void *)loc), R_Value, Exp_Value);
			return FALSE;
		}
	}
	return TRUE;
}	


//
// Function:	WUp16(...)
// Purpose:		Step up through memory writing a 16 bit value
// Params:		
//				LogHandle	Handle for xLog
//				StartAddr	Starting address of memory to test
//				EndAddr		Ending address of memory to test, adjusted for data width
//				W_Value		Value to write
// Notes:		
//
BOOL
WUp16(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr,
	  unsigned __int16 W_Value) {

	volatile unsigned __int16 *loc;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, "WUp16:    Write Value: %#.4x", W_Value);
	
	for(loc = (unsigned __int16 *)StartAddr; loc <= (unsigned __int16 *)EndAddr; loc++) {
				*loc = W_Value;
		}
	return TRUE;
}

//
// Function:	RWUp16(...)
// Purpose:		Step up through memory reading an expected value and writing a 16 bit value
//
BOOL
RWUp16(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int16 Exp_Value, unsigned __int16 W_Value) {

	volatile unsigned __int16 *loc;
	unsigned __int16 R_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RWUp16:   Expected Value: %#.4x, Write Value: %#.4x", Exp_Value, W_Value);

	for(loc = (unsigned __int16 *)StartAddr; loc <= (unsigned __int16 *)EndAddr; loc++) {
		// Read expected value
		R_Value = *loc;
		if(Exp_Value != R_Value) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RWUp16:   FAILED at %p (physical = %p), Read: %#.4x, Expected: %#.4x",
						loc, MmGetPhysicalAddress((void *)loc), R_Value, Exp_Value);
			return FALSE;
		}

		// Write new value	
		*loc = W_Value;
	}
	return TRUE;
}	

//
// Function:	RWDown16(...)
// Purpose:		Step down through memory reading an expected value and writing a 16 bit value
//
BOOL
RWDown16(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int16 Exp_Value, unsigned __int16 W_Value) {

	volatile unsigned __int16 *loc;
	unsigned __int16 R_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RWDown16: Expected Value: %#.4x, Write Value: %#.4x", Exp_Value, W_Value);

	for(loc = (unsigned __int16 *)EndAddr; loc >= (unsigned __int16 *)StartAddr; loc--) {
		// Read expected value
		R_Value = *loc;
		if(Exp_Value != R_Value) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RWDown16: FAILED at %p (physical = %p), Read: %#.4x, Expected: %#.4x",
						loc, MmGetPhysicalAddress((void *)loc), R_Value, Exp_Value);
			return FALSE;
		}

		// Write new value	
		*loc = W_Value;
	}
	return TRUE;
}	
	
//
// Function:	RDown16(...)
// Purpose:		Step down through memory reading an expected value
//
BOOL
RDown16(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int16 Exp_Value) {

	volatile unsigned __int16 *loc;
	unsigned __int16 R_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RDown16:  Expected Value: %#.4x", Exp_Value);

	for(loc = (unsigned __int16 *)EndAddr; loc >= (unsigned __int16 *)StartAddr; loc--) {
		// Read expected value
		R_Value = *loc;
		if(Exp_Value != R_Value) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RDown16:  FAILED at %p (physical = %p), Read: %#.4x, Expected: %#.4x",
				loc, MmGetPhysicalAddress((void *)loc), R_Value, Exp_Value);
			return FALSE;
		}
	}
	return TRUE;
}	

//
// Function:	WUp8(...)
// Purpose:		Step up through memory writing a 8 bit value
// Params:		
//				LogHandle	Handle for xLog
//				StartAddr	Starting address of memory to test
//				EndAddr		Ending address of memory to test, adjusted for data width
//				W_Value		Value to write
// Notes:		
//
BOOL
WUp8(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr,
	  unsigned __int8 W_Value) {

	volatile unsigned __int8 *loc;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, "WUp8:     Write Value: %#.2x", W_Value);
	
	for(loc = (unsigned __int8 *)StartAddr; loc <= (unsigned __int8 *)EndAddr; loc++) {
				*loc = W_Value;
		}
	return TRUE;
}

//
// Function:	RWUp8(...)
// Purpose:		Step up through memory reading an expected value and writing a 8 bit value
//
BOOL
RWUp8(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int8 Exp_Value, unsigned __int8 W_Value) {

	volatile unsigned __int8 *loc;
	unsigned __int8 R_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RWUp8:     Expected Value: %#.2x, Write Value: %#.2x", Exp_Value, W_Value);

	for(loc = (unsigned __int8 *)StartAddr; loc <= (unsigned __int8 *)EndAddr; loc++) {
		// Read expected value
		R_Value = *loc;
		if(Exp_Value != R_Value) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RWUp8:     FAILED at %p (physical = %p), Read: %#.2x, Expected: %#.2x",
						loc, MmGetPhysicalAddress((void *)loc), R_Value, Exp_Value);
			return FALSE;
		}

		// Write new value	
		*loc = W_Value;
	}
	return TRUE;
}	

//
// Function:	RWDown8(...)
// Purpose:		Step down through memory reading an expected value and writing a 8 bit value
//
BOOL
RWDown8(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int8 Exp_Value, unsigned __int8 W_Value) {

	volatile unsigned __int8 *loc;
	unsigned __int8 R_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RWDown8:  Expected Value: %#.2x, Write Value: %#.2x", Exp_Value, W_Value);

	for(loc = (unsigned __int8 *)EndAddr; loc >= (unsigned __int8 *)StartAddr; loc--) {
		// Read expected value
		R_Value = *loc;
		if(Exp_Value != R_Value) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RWDown8:  FAILED at %p (physical = %p), Read: %#.2x, Expected: %#.2x",
						loc, MmGetPhysicalAddress((void *)loc), R_Value, Exp_Value);
			return FALSE;
		}

		// Write new value	
		*loc = W_Value;
	}
	return TRUE;
}	
	
//
// Function:	RDown8(...)
// Purpose:		Step down through memory reading an expected value
//
BOOL
RDown8(HANDLE LogHandle, unsigned __int8 *StartAddr, unsigned __int8 *EndAddr, 
		unsigned __int8 Exp_Value) {

	volatile unsigned __int8 *loc;
	unsigned __int8 R_Value;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"RDown8:   Expected Value: %#.2x", Exp_Value);

	for(loc = (unsigned __int8 *)EndAddr; loc >= (unsigned __int8 *)StartAddr; loc--) {
		// Read expected value
		R_Value = *loc;
		if(Exp_Value != R_Value) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"RDown8:   FAILED at %p (physical = %p), Read: %#.2x, Expected: %#.2x",
				loc, MmGetPhysicalAddress((void *)loc), R_Value, Exp_Value);
			return FALSE;
		}
	}
	return TRUE;
}	


VOID
WINAPI
MemPatStartTest(HANDLE LogHandle) {
    unsigned __int8 *StartAddr, *EndAddr;
	SIZE_T TestableMemSize, PercentAvail;
	ULONG PhysicalAddress, Alignment;
	MEMORYSTATUS ms;
	BOOL Width128, Width64, Width32, Width16, Width8, UseWriteCombine, bDone, bPass;
	DWORD Protect;
	int TestIterations, i;

	// Initialize
	i = 0;
	bDone = FALSE;

	// Get test configuration from ini file
	Width128 = GetProfileInt(TEXT("mempat"), TEXT("Width128"), 1);
	Width64 = GetProfileInt(TEXT("mempat"), TEXT("Width64"), 1);
	Width32 = GetProfileInt(TEXT("mempat"), TEXT("Width32"), 1);
	Width16 = GetProfileInt(TEXT("mempat"), TEXT("Width16"), 1);
	Width8 = GetProfileInt(TEXT("mempat"), TEXT("Width8"), 1);
	TestIterations = GetProfileInt(TEXT("mempat"), TEXT("TestIterations"), 1);
	PercentAvail = GetProfileInt(TEXT("mempat"), TEXT("PercentAvail"), 80);
	UseWriteCombine = GetProfileInt(TEXT("mempat"), TEXT("UseWriteCombine"), 0);

    xSetOwnerAlias(LogHandle, "a-emebac");
	xSetComponent(LogHandle, "Hardware", "MemTest");
    xSetFunctionName(LogHandle, "MemTest");
	xStartVariation( gbConsoleOut, LogHandle, "Initialization" );

	// Query global memory status
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatus( &ms );
	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"MEMPAT: GlobalMemoryStatus returned %u bytes of %u bytes available", 
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
		StartAddr = (unsigned __int8 *)XPhysicalAlloc(TestableMemSize, PhysicalAddress, Alignment, Protect);
  
        if ( StartAddr ) {
            break;
        }
        if ( TestableMemSize <= PAGE_SIZE ) {
            break;
        }
        TestableMemSize -= PAGE_SIZE;
    }

    if ( !StartAddr ) {
        xLog(gbConsoleOut, LogHandle, XLL_BLOCK, "MEMPAT: unable to allocate memory (size=%lu, error=%s)",
            TestableMemSize, WinErrorSymbolicName(GetLastError()));
        return;
    }

	xLog(gbConsoleOut, LogHandle, XLL_INFO, "MemPat: %lu bytes allocated as %s", 
		TestableMemSize, UseWriteCombine ? "PAGE_WRITECOMBINE" : "PAGE_NOCACHE");
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "MemPat: Starting Address %p (physical %p)",
		StartAddr, MmGetPhysicalAddress(StartAddr));
	xEndVariation(LogHandle);

	do {
		// Increment iteration counter
		i++;

		// 128 bit data
		if (Width128) {
			bPass = TRUE;
			EndAddr = StartAddr + TestableMemSize - sizeof(MyInt128);
			xStartVariation(gbConsoleOut, LogHandle, "128 Bit Data");
			xLog(gbConsoleOut, LogHandle, XLL_INFO, "128 Bit Data: Iteration %d: StartAddr = %p, EndAddr = %p", i, StartAddr, EndAddr);

			bPass &= WUp128	 (LogHandle, StartAddr, EndAddr, 0x0000000000000000, 0x0000000000000000);
			bPass &= RWDown128(LogHandle, StartAddr, EndAddr, 0x0000000000000000, 0x0000000000000000, 0xffffffffffffffff, 0xffffffffffffffff);
			bPass &= RWUp128	 (LogHandle, StartAddr, EndAddr, 0xffffffffffffffff, 0xffffffffffffffff, 0x5555555555555555, 0x5555555555555555);
			bPass &= RWDown128(LogHandle, StartAddr, EndAddr, 0x5555555555555555, 0x5555555555555555, 0xaaaaaaaaaaaaaaaa, 0xaaaaaaaaaaaaaaaa);
			bPass &= RWUp128	 (LogHandle, StartAddr, EndAddr, 0xaaaaaaaaaaaaaaaa, 0xaaaaaaaaaaaaaaaa, 0x5555555555555555, 0x5555555555555555);
			bPass &= RWDown128(LogHandle, StartAddr, EndAddr, 0x5555555555555555, 0x5555555555555555, 0x3333333333333333, 0x3333333333333333);
			bPass &= RWUp128	 (LogHandle, StartAddr, EndAddr, 0x3333333333333333, 0x3333333333333333, 0xcccccccccccccccc, 0xcccccccccccccccc);
			bPass &= RWDown128(LogHandle, StartAddr, EndAddr, 0xcccccccccccccccc, 0xcccccccccccccccc, 0x3333333333333333, 0x3333333333333333);
			bPass &= RWUp128	 (LogHandle, StartAddr, EndAddr, 0x3333333333333333, 0x3333333333333333, 0x0f0f0f0f0f0f0f0f, 0x0f0f0f0f0f0f0f0f);
			bPass &= RWDown128(LogHandle, StartAddr, EndAddr, 0x0f0f0f0f0f0f0f0f, 0x0f0f0f0f0f0f0f0f, 0xf0f0f0f0f0f0f0f0, 0xf0f0f0f0f0f0f0f0);
			bPass &= RWUp128	 (LogHandle, StartAddr, EndAddr, 0xf0f0f0f0f0f0f0f0, 0xf0f0f0f0f0f0f0f0, 0x0f0f0f0f0f0f0f0f, 0x0f0f0f0f0f0f0f0f);
			bPass &= RWDown128(LogHandle, StartAddr, EndAddr, 0x0f0f0f0f0f0f0f0f, 0x0f0f0f0f0f0f0f0f, 0x00ff00ff00ff00ff, 0x00ff00ff00ff00ff);
			bPass &= RWUp128	 (LogHandle, StartAddr, EndAddr, 0x00ff00ff00ff00ff, 0x00ff00ff00ff00ff, 0xff00ff00ff00ff00, 0xff00ff00ff00ff00);
			bPass &= RWDown128(LogHandle, StartAddr, EndAddr, 0xff00ff00ff00ff00, 0xff00ff00ff00ff00, 0x00ff00ff00ff00ff, 0x00ff00ff00ff00ff);
			bPass &= RWUp128	 (LogHandle, StartAddr, EndAddr, 0x00ff00ff00ff00ff, 0x00ff00ff00ff00ff, 0x0000ffff0000ffff, 0x0000ffff0000ffff);
			bPass &= RWDown128(LogHandle, StartAddr, EndAddr, 0x0000ffff0000ffff, 0x0000ffff0000ffff, 0xffff0000ffff0000, 0xffff0000ffff0000);
			bPass &= RWUp128	 (LogHandle, StartAddr, EndAddr, 0xffff0000ffff0000, 0xffff0000ffff0000, 0x0000ffff0000ffff, 0x0000ffff0000ffff);
			bPass &= RWDown128(LogHandle, StartAddr, EndAddr, 0x0000ffff0000ffff, 0x0000ffff0000ffff, 0x00000000ffffffff, 0x00000000ffffffff);
			bPass &= RWUp128	 (LogHandle, StartAddr, EndAddr, 0x00000000ffffffff, 0x00000000ffffffff, 0xffffffff00000000, 0xffffffff00000000);
			bPass &= RWDown128(LogHandle, StartAddr, EndAddr, 0xffffffff00000000, 0xffffffff00000000, 0x0000000000000000, 0xffffffffffffffff);
			bPass &= RWUp128  (LogHandle, StartAddr, EndAddr, 0x0000000000000000, 0xffffffffffffffff, 0xffffffffffffffff, 0x0000000000000000);
			bPass &= RDown128 (LogHandle, StartAddr, EndAddr, 0xffffffffffffffff, 0x0000000000000000);

			// Pass Fail
			if(bPass) {
				xLog(gbConsoleOut, LogHandle, XLL_PASS, "128 Bit Data: PASSED Iteration %d", i);
			} else {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "128 Bit Data: FAILED Iteration %d", i);
				break;
			}
			xEndVariation(LogHandle);
		}

		// 64 bit data
		if (Width64) {
			bPass = TRUE;
			EndAddr = StartAddr + TestableMemSize - sizeof(unsigned __int64);
			xStartVariation(gbConsoleOut, LogHandle, "64 Bit Data");
			xLog(gbConsoleOut, LogHandle, XLL_INFO, "64 Bit Data: Iteration %d: StartAddr = %p, EndAddr = %p", i, StartAddr, EndAddr);
			bPass &= WUp64	(LogHandle, StartAddr, EndAddr, 0x0000000000000000);
			bPass &= RWDown64(LogHandle, StartAddr, EndAddr, 0x0000000000000000, 0xffffffffffffffff);
			bPass &= RWUp64	(LogHandle, StartAddr, EndAddr, 0xffffffffffffffff, 0x5555555555555555);
			bPass &= RWDown64(LogHandle, StartAddr, EndAddr, 0x5555555555555555, 0xaaaaaaaaaaaaaaaa);
			bPass &= RWUp64	(LogHandle, StartAddr, EndAddr, 0xaaaaaaaaaaaaaaaa, 0x5555555555555555);
			bPass &= RWDown64(LogHandle, StartAddr, EndAddr, 0x5555555555555555, 0x3333333333333333);
			bPass &= RWUp64	(LogHandle, StartAddr, EndAddr, 0x3333333333333333, 0xcccccccccccccccc);
			bPass &= RWDown64(LogHandle, StartAddr, EndAddr, 0xcccccccccccccccc, 0x3333333333333333);
			bPass &= RWUp64	(LogHandle, StartAddr, EndAddr, 0x3333333333333333, 0x0f0f0f0f0f0f0f0f);
			bPass &= RWDown64(LogHandle, StartAddr, EndAddr, 0x0f0f0f0f0f0f0f0f, 0xf0f0f0f0f0f0f0f0);
			bPass &= RWUp64	(LogHandle, StartAddr, EndAddr, 0xf0f0f0f0f0f0f0f0, 0x0f0f0f0f0f0f0f0f);
			bPass &= RWDown64(LogHandle, StartAddr, EndAddr, 0x0f0f0f0f0f0f0f0f, 0x00ff00ff00ff00ff);
			bPass &= RWUp64	(LogHandle, StartAddr, EndAddr, 0x00ff00ff00ff00ff, 0xff00ff00ff00ff00);
			bPass &= RWDown64(LogHandle, StartAddr, EndAddr, 0xff00ff00ff00ff00, 0x00ff00ff00ff00ff);
			bPass &= RWUp64	(LogHandle, StartAddr, EndAddr, 0x00ff00ff00ff00ff, 0x0000ffff0000ffff);
			bPass &= RWDown64(LogHandle, StartAddr, EndAddr, 0x0000ffff0000ffff, 0xffff0000ffff0000);
			bPass &= RWUp64	(LogHandle, StartAddr, EndAddr, 0xffff0000ffff0000, 0x0000ffff0000ffff);
			bPass &= RWDown64(LogHandle, StartAddr, EndAddr, 0x0000ffff0000ffff, 0x00000000ffffffff);
			bPass &= RWUp64	(LogHandle, StartAddr, EndAddr, 0x00000000ffffffff, 0xffffffff00000000);
			bPass &= RDown64 (LogHandle, StartAddr, EndAddr, 0xffffffff00000000);

			// Pass Fail
			if(bPass) {
				xLog(gbConsoleOut, LogHandle, XLL_PASS, "64 Bit Data: PASSED Iteration %d", i);
			} else {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "64 Bit Data: FAILED Iteration %d", i);
				break;
			}
			xEndVariation(LogHandle);
		}

		// 32 bit data
		if (Width32) {
			bPass = TRUE;
			EndAddr = StartAddr + TestableMemSize - sizeof(unsigned __int32);
			xStartVariation(gbConsoleOut, LogHandle, "32 Bit Data");
			xLog(gbConsoleOut, LogHandle, XLL_INFO, "32 Bit Data: Iteration %d: StartAddr = %p, EndAddr = %p", i, StartAddr, EndAddr);
			bPass &= WUp32	(LogHandle, StartAddr, EndAddr, 0x00000000);
			bPass &= RWDown32(LogHandle, StartAddr, EndAddr, 0x00000000, 0xffffffff);
			bPass &= RWUp32	(LogHandle, StartAddr, EndAddr, 0xffffffff, 0x55555555);
			bPass &= RWDown32(LogHandle, StartAddr, EndAddr, 0x55555555, 0xaaaaaaaa);
			bPass &= RWUp32	(LogHandle, StartAddr, EndAddr, 0xaaaaaaaa, 0x55555555);
			bPass &= RWDown32(LogHandle, StartAddr, EndAddr, 0x55555555, 0x33333333);
			bPass &= RWUp32	(LogHandle, StartAddr, EndAddr, 0x33333333, 0xcccccccc);
			bPass &= RWDown32(LogHandle, StartAddr, EndAddr, 0xcccccccc, 0x33333333);
			bPass &= RWUp32	(LogHandle, StartAddr, EndAddr, 0x33333333, 0x0f0f0f0f);
			bPass &= RWDown32(LogHandle, StartAddr, EndAddr, 0x0f0f0f0f, 0xf0f0f0f0);
			bPass &= RWUp32	(LogHandle, StartAddr, EndAddr, 0xf0f0f0f0, 0x0f0f0f0f);
			bPass &= RWDown32(LogHandle, StartAddr, EndAddr, 0x0f0f0f0f, 0x00ff00ff);
			bPass &= RWUp32	(LogHandle, StartAddr, EndAddr, 0x00ff00ff, 0xff00ff00);
			bPass &= RWDown32(LogHandle, StartAddr, EndAddr, 0xff00ff00, 0x00ff00ff);
			bPass &= RWUp32	(LogHandle, StartAddr, EndAddr, 0x00ff00ff, 0x0000ffff);
			bPass &= RWDown32(LogHandle, StartAddr, EndAddr, 0x0000ffff, 0xffff0000);
			bPass &= RWUp32	(LogHandle, StartAddr, EndAddr, 0xffff0000, 0x0000ffff);
			bPass &= RDown32	(LogHandle, StartAddr, EndAddr, 0x0000ffff);

			// Pass Fail
			if(bPass) {
				xLog(gbConsoleOut, LogHandle, XLL_PASS, "32 Bit Data: PASSED Iteration %d", i);
			} else {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "32 Bit Data: FAILED Iteration %d", i);
				break;
			}
			xEndVariation(LogHandle);
		}

		// 16 bit data
		if (Width16) {
			bPass = TRUE;
			EndAddr = StartAddr + TestableMemSize - sizeof(unsigned __int16);
			xStartVariation(gbConsoleOut, LogHandle, "16 Bit Data");
			xLog(gbConsoleOut, LogHandle, XLL_INFO, "16 Bit Data: Iteration %d: StartAddr = %p, EndAddr = %p", i, StartAddr, EndAddr);
			bPass &= WUp16	(LogHandle, StartAddr, EndAddr, 0x0000);
			bPass &= RWDown16(LogHandle, StartAddr, EndAddr, 0x0000, 0xffff);
			bPass &= RWUp16	(LogHandle, StartAddr, EndAddr, 0xffff, 0x5555);
			bPass &= RWDown16(LogHandle, StartAddr, EndAddr, 0x5555, 0xaaaa);
			bPass &= RWUp16	(LogHandle, StartAddr, EndAddr, 0xaaaa, 0x5555);
			bPass &= RWDown16(LogHandle, StartAddr, EndAddr, 0x5555, 0x3333);
			bPass &= RWUp16	(LogHandle, StartAddr, EndAddr, 0x3333, 0xcccc);
			bPass &= RWDown16(LogHandle, StartAddr, EndAddr, 0xcccc, 0x3333);
			bPass &= RWUp16	(LogHandle, StartAddr, EndAddr, 0x3333, 0x0f0f);
			bPass &= RWDown16(LogHandle, StartAddr, EndAddr, 0x0f0f, 0xf0f0);
			bPass &= RWUp16	(LogHandle, StartAddr, EndAddr, 0xf0f0, 0x0f0f);
			bPass &= RWDown16(LogHandle, StartAddr, EndAddr, 0x0f0f, 0x00ff);
			bPass &= RWUp16	(LogHandle, StartAddr, EndAddr, 0x00ff, 0xff00);
			bPass &= RDown16	(LogHandle, StartAddr, EndAddr, 0xff00);

			// Pass Fail
			if(bPass) {
				xLog(gbConsoleOut, LogHandle, XLL_PASS, "16 Bit Data: PASSED Iteration %d", i);
			} else {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "16 Bit Data: FAILED Iteration %d", i);
				break;
			}
			xEndVariation(LogHandle);
		}

		// 8 bit data
		if (Width8) {
			bPass = TRUE;
			EndAddr = StartAddr + TestableMemSize - sizeof(unsigned __int8);
			xStartVariation(gbConsoleOut, LogHandle, "8 Bit Data");
			xLog(gbConsoleOut, LogHandle, XLL_INFO, "8 Bit Data: Iteration %d: StartAddr = %p, EndAddr = %p", i, StartAddr, EndAddr);
			bPass &= WUp8	(LogHandle, StartAddr, EndAddr, 0x00);
			bPass &= RWDown8	(LogHandle, StartAddr, EndAddr, 0x00, 0xff);
			bPass &= RWUp8	(LogHandle, StartAddr, EndAddr, 0xff, 0x55);
			bPass &= RWDown8	(LogHandle, StartAddr, EndAddr, 0x55, 0xaa);
			bPass &= RWUp8	(LogHandle, StartAddr, EndAddr, 0xaa, 0x55);
			bPass &= RWDown8	(LogHandle, StartAddr, EndAddr, 0x55, 0x33);
			bPass &= RWUp8	(LogHandle, StartAddr, EndAddr, 0x33, 0xcc);
			bPass &= RWDown8	(LogHandle, StartAddr, EndAddr, 0xcc, 0x33);
			bPass &= RWUp8	(LogHandle, StartAddr, EndAddr, 0x33, 0x0f);
			bPass &= RWDown8	(LogHandle, StartAddr, EndAddr, 0x0f, 0xf0);
			bPass &= RWUp8	(LogHandle, StartAddr, EndAddr, 0xf0, 0x0f);
			bPass &= RDown8	(LogHandle, StartAddr, EndAddr, 0x0f);

			// Pass Fail
			if(bPass) {
				xLog(gbConsoleOut, LogHandle, XLL_PASS, "8 Bit Data: PASSED Iteration %d", i);
			} else {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "8 Bit Data: FAILED Iteration %d", i);
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
MemPatEndTest(VOID) {
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
	LogHandle = xCreateLog_W(L"t:\\mempat.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("MEMPAT: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
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
	MemPatStartTest(LogHandle);

	// End Test
	MemPatEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("MEMPAT: End - Waiting for reboot...\n");
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
DECLARE_EXPORT_DIRECTORY( mempat )
#pragma data_seg()

BEGIN_EXPORT_TABLE( mempat )
    EXPORT_TABLE_ENTRY( "StartTest", MemPatStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", MemPatEndTest )
END_EXPORT_TABLE( mempat )

