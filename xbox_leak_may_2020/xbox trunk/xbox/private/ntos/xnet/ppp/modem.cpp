/*++

Copyright (c) 2000 Microsoft Corporation

    init.cpp

Abstract:

    XBox PPP initialization routines.

Revision History:

    07-11-00    vadimg      created

--*/

#include "precomp.h"

/***************************************************************************\
* ModemCommand
*
\***************************************************************************/

BOOL ModemCommand(const CHAR *pszCommand, const CHAR *pszResponse)
{
    char buffer[MODEM_BUFFER_SIZE];
    ULONG cb = strlen(pszCommand);

    if (!ModemWrite((PUCHAR)pszCommand, &cb)) {
        return FALSE;
    }

    DbgPrint("WRITE: %s\n", pszCommand);

    if (WaitForSingleObject(gQueue.hEvent, READ_TIMEOUT) != WAIT_OBJECT_0) {
        return FALSE;
    }

    cb = MODEM_BUFFER_SIZE;
    if (!ModemRead((PUCHAR)buffer, &cb) || cb == 0) {
        return FALSE;
    }
    buffer[cb] = '\0';

    DbgPrint("READ: %s\n", buffer);

    if (str_search(buffer, pszResponse) == NULL) {
        return FALSE;
    }

    DbgPrint("RESPONSE: %s\n", pszResponse);
    return TRUE;
}

/***************************************************************************\
* InitModem
*
\***************************************************************************/

BOOL InitModem(VOID)
{
    if (!ModemOpen(gQueue.hEvent)) {
        return FALSE;
    }

    if (!ModemSetLineCoding(57600, 0, 0, 8)) {
        return FALSE;
    }

    if (!ModemCommand("AT &F E0 &C1 &D2 V1 S0=0\r", "OK")) {
        return FALSE;
    }

    if (!ModemCommand("ATS7=60S30=0L0M1\\N3%C1&K3N1X4\r", "OK")) {
        return FALSE;
    }

    return TRUE;
}

/***************************************************************************\
* DialModem
*
\***************************************************************************/

BOOL DialModem(VOID)
{
    char buffer[MODEM_BUFFER_SIZE];
    char *p = buffer;

    p = str_add(p, "ATDT");
    p = str_add(p, gInfo.szNumber);
    str_add(p, "\r");

    return ModemCommand(buffer, "CONNECT");
}

/***************************************************************************\
* HangupModem
*
\***************************************************************************/

BOOL HangupModem(VOID)
{
    return ModemCommand("ATH0\r", "OK");
}

