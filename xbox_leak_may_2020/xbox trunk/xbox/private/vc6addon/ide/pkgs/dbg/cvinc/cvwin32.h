//-----------------------------------------------------------------------------
//	cvwin32.h
//
//  Copyright (C) 1993, Microsoft Corporation
//
//  Purpose:
//		api for the 4 functions in cvwin32.c
//
//  Functions/Methods present:
//
//  Revision History:
//
//	[]		05-Mar-1993 Dans	Created
//
//-----------------------------------------------------------------------------

#if !(defined (_cvwin32_h))
#define _cvwin32_h 1

typedef enum ICS {	// index to critical section
	icsBm,			// handle-based memory allocation routines
	icsWmalloc,		// pointer-based memory allocation routines
	icsMax
} ICS;

typedef void *	PCS;

void _fastcall		CVInitCritSection(ICS);
void _fastcall		CVLeaveCritSection(ICS);
void _fastcall		CVEnterCritSection(ICS);
void _fastcall		CVDeleteCritSection(ICS);
PCS  _fastcall		PcsAllocInit();
void _fastcall		FreePcs(PCS);

// These routines are called thousands of times when stepping,
// and show up in the profile reports
void inline 	AcquireLockPcs ( PCS pcs ) {
        EnterCriticalSection ( (CRITICAL_SECTION *)pcs );
}

void inline		ReleaseLockPcs ( PCS pcs ) {
        LeaveCriticalSection ((CRITICAL_SECTION *)pcs );
}

// void _fastcall          AcquireLockPcs(PCS);
// void _fastcall          ReleaseLockPcs(PCS);

#endif
