/*++

Copyright (c) 1997-2000  Microsoft Corporation

Module Name:

    utils.cpp

Abstract:

    This program contains routines to stress object manager

Author:

    Silviu Calinoiu (silviuc) 18-Mar-1997

Environment:

    Xbox

Revision History:

    04-May-2000     schanbai

        Ported to Xbox

--*/


#include "pch.h"


VOID
__cdecl 
_DbgMessage(
    LPCSTR Format,
    ...
    )
{
    va_list Params;
    //CHAR Buffer [4096];

    va_start (Params, Format);
    //sprintf (Buffer, TEXT("OBSTRESS: "));
    //vsprintf (Buffer + _tcslen(Buffer), Format, Params);
    va_end(Params);
    //OutputDebugStringA( Buffer );
}
