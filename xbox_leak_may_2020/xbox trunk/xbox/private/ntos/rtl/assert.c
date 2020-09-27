/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    assert.c

Abstract:

    This module implements the RtlAssert function that is referenced by the
    debugging version of the ASSERT macro defined in NTDEF.H

Author:

    Steve Wood (stevewo) 03-Oct-1989

Revision History:

--*/

#include <nt.h>
#include <ntrtl.h>
#include <zwapi.h>
#include "stdio.h"

NTSTATUS
PsTerminateSystemThread(
    IN NTSTATUS ExitStatus
    );

NTSTATUS
DebugService(
	ULONG ServiceClass,
	PVOID Arg1,
	PVOID Arg2
	);

//
// RtlAssert is not called unless the caller is compiled with DBG non-zero
// therefore it does no harm to always have this routin in the kernel.  
// This allows checked drivers to be thrown on the system and have their
// asserts be meaningful.
//

#define RTL_ASSERT_ALWAYS_ENABLED 1

VOID
RtlAssert(
    IN PVOID FailedAssertion,
    IN PVOID FileName,
    IN ULONG LineNumber,
    IN PCHAR Message OPTIONAL
    )
{
#if DBG || RTL_ASSERT_ALWAYS_ENABLED
    char Response;
    CONTEXT Context;
	STRING String;
	UCHAR Buffer[512];

#ifndef BLDR_KERNEL_RUNTIME
    RtlCaptureContext( &Context );
#endif

	sprintf(Buffer, "\n*** Assertion failed: %s%s\n***   Source File: %s, line %ld\n\n",
                  Message ? Message : "",
                  FailedAssertion,
                  FileName,
                  LineNumber
				  );
    RtlInitAnsiString( &String, Buffer);

    while (TRUE) {
		Response = (CHAR)DebugService(BREAKPOINT_RTLASSERT, &String, 0);
        switch (Response) {
            case 'B':
            case 'b':
                DbgPrint( "Execute '!cxr %p' to dump context\n", &Context);
                DbgBreakPoint();
                break;

            case 'I':
            case 'i':
                return;

            case 'T':
            case 't':
                PsTerminateSystemThread( STATUS_UNSUCCESSFUL );
                break;
            }
        }

    DbgBreakPoint();
#endif
}

VOID
RtlRip(
    IN PVOID ApiName,
    IN PVOID Expression,
    IN PVOID Message
    )
{
#if DBG || RTL_ASSERT_ALWAYS_ENABLED
    UCHAR Buffer[512];
    char Colon;
    char OpenParen;
    char CloseParen;

    if(NULL == Message)
        Message = Expression != NULL ? "failed" : "unexpected failure";
    Colon = ApiName != NULL ? ':' : ' ';
    if(NULL != Expression) {
        OpenParen = '(';
        CloseParen = ')';
    } else
        OpenParen = CloseParen = ' ';

    _snprintf(Buffer, sizeof Buffer, "%s%c %c%s%c %s", ApiName ? ApiName : "",
        Colon, OpenParen, Expression ? Expression : "", CloseParen, Message);
    Buffer[sizeof Buffer - 1] = 0;
    DebugService(BREAKPOINT_RIP, Buffer, 0);
#endif
}
