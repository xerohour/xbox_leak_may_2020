/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1995-1999  Microsoft Corporation

Module Name:

    ntkxapi.h

Abstract:

    This module is the header file for all the executive system services
    that are exported by the "ke" directory.

Author:

    David N. Cutler (davec) 1-Apr-1995

Environment:

    Any mode.

Revision History:

--*/

#ifndef _NTKXAPI_
#define _NTKXAPI_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif


//
// Channel Specific Access Rights.
//

#define CHANNEL_READ_MESSAGE 0x1
#define CHANNEL_WRITE_MESSAGE 0x2
#define CHANNEL_QUERY_INFORMATION 0x4
#define CHANNEL_SET_INFORMATION 0x8

#define CHANNEL_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0xf)

//
// Channel message structure.
//
#ifdef _MAC
#pragma warning( disable : 4121)
#endif

typedef struct _CHANNEL_MESSAGE {
    PVOID Text;
    ULONG Length;
    PVOID Context;
    PVOID Base;
    union {
        BOOLEAN Close;
        LONGLONG Align;
    };
#ifdef _MAC
#pragma warning( default : 4121 )
#endif


} CHANNEL_MESSAGE, *PCHANNEL_MESSAGE;

//
// Channel object function defintions.
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateChannel (
    OUT PHANDLE ChannelHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenChannel (
    OUT PHANDLE ChannelHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtListenChannel (
    IN HANDLE ChannelHandle,
    OUT PCHANNEL_MESSAGE *Message
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSendWaitReplyChannel (
    IN HANDLE ChannelHandle,
    IN PVOID Text,
    IN ULONG Length,
    OUT PCHANNEL_MESSAGE *Message
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtReplyWaitSendChannel (
    IN PVOID Text,
    IN ULONG Length,
    OUT PCHANNEL_MESSAGE *Message
    );

//NTSYSAPI
//NTSTATUS
//NTAPI
//NtImpersonateChannel (
//    VOID
//    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetContextChannel (
    IN PVOID Context
    );

#ifdef __cplusplus
}
#endif

#endif  // _NTKXAPI_
