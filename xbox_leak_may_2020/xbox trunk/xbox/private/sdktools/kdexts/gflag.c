/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    gflag.c

Abstract:

    WinDbg Extension Api

Author:

    Ramon J San Andres (ramonsa) 5-Nov-1993

Environment:

    User Mode.

Revision History:

--*/

#include "precomp.h"
#pragma hdrstop

VOID
GFlagExtension(
    IN PVOID pNtGlobalFlag,
    IN ULONG ValidBits,
    IN PCSTR lpArgumentString
    );


DECLARE_API( gflag )

/*++

Routine Description:

    see syntax in ..\ntsdexts\gflagext.c

Arguments:

Return Value:

    None

--*/

{
    dprintf( "NtGlobalFlag at %x\n", GetExpression("nt!NtGlobalFlag") );

    GFlagExtension( (PVOID)GetExpression("nt!NtGlobalFlag"), FLG_VALID_BITS, args );
}

#include "..\\ntsdexts\\gflagext.c"
