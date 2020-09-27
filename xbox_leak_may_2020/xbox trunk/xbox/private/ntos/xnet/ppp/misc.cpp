/*++

Copyright (c) 2000 Microsoft Corporation

    misc.cpp

Abstract:

    Misc support functions.

Revision History:

    07-17-00    vadimg      created

--*/

#include "precomp.h"

/***************************************************************************\
* InitRestartCounters
*
\***************************************************************************/

VOID InitRestartCounters(VOID)
{
    gInfo.ConfigRetryCount = PPP_MAX_FAILURE;
    gInfo.TermRetryCount   = PPP_MAX_FAILURE;
}

/***************************************************************************\
* GetCpIndexFromProtocol
*
\***************************************************************************/

DWORD GetCpIndexFromProtocol(DWORD dwProtocol)
{
    DWORD i;

    for (i = 0; i < NUM_CP; i++) {
        if (gCpTable[i].Protocol == dwProtocol) {
            return i;
        }
    }

    return (DWORD)-1;
}

/***************************************************************************\
* ReverseString
*
\***************************************************************************/

VOID ReverseString(CHAR *psz)
{
    CHAR *pchBegin, *pchEnd, ch;

    pchBegin = psz;
    pchEnd = psz + strlen(psz) - 1;

    while (pchBegin < pchEnd) {

        ch = *pchBegin;
        *pchBegin = *pchEnd;
        *pchEnd = ch;

        ++pchBegin;
        --pchEnd;
    }
}

/***************************************************************************\
* EncodePw
*
\***************************************************************************/

#define PASSWORDMAGIC 0xA5

VOID EncodePw(CHAR* pszPassword)
{
    CHAR *pch;

    ReverseString(pszPassword);

    for (pch = pszPassword; *pch != '\0'; ++pch) {
        if (*pch != (CHAR)PASSWORDMAGIC) {
            *pch ^= PASSWORDMAGIC;
        }
    }
}

/***************************************************************************\
* DecodePw
*
\***************************************************************************/

VOID DecodePw(CHAR* pszPassword)
{
    EncodePw(pszPassword);
}

