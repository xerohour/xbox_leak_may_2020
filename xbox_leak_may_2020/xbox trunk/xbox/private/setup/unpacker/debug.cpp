//  DEBUG.CPP
//
//  Created 13-Mar-2001 [JonT]

#include "unpacker.h"

void
_DebugPrint(
    LPCSTR lpszFormat,
    va_list vararg
    )
{
    char szOutput[4096];

    vsprintf(szOutput, lpszFormat, vararg);
    OutputDebugString(szOutput);
}

