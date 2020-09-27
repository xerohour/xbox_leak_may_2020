/*++

Copyright (c) 2000 Microsoft Corporation

    init.cpp

Abstract:

    XBox PPP initialization routines.

Revision History:

    07-11-00    vadimg      created

--*/

#include "precomp.h"

VOID (*gCpGetInfo[])(PPP_CP_INFO *pInfo) = {
    GetLcpInfo,
    GetPapInfo,
    GetIpcpInfo
};

/***************************************************************************\
* PppConnect
*
\***************************************************************************/

BOOL PppConnect(CHAR *pszUsername, CHAR *pszPassword, CHAR *pszDomain, CHAR *pszNumber)
{
    BOOL fSuccess = FALSE;
    HANDLE hThread = NULL;
    DWORD dwTid;
    ULONG i;
    PBYTE pFrame;
    
    if (strlen(pszUsername) > UNLEN) {
        return FALSE;
    }
    if (strlen(pszPassword) > PWLEN) {
        return FALSE;
    }
    if (strlen(pszDomain) > DNLEN) {
        return FALSE;
    }
    if (strlen(pszNumber) > PNLEN) {
        return FALSE;
    }

    pFrame = (PBYTE)Alloc(PPP_FRAME_MAX_LEN);
    if (pFrame != NULL) {
        gInfo.pFrame = (PPP_FRAME*)pFrame;
        gInfo.pFrame->Address = 0xFF;
        gInfo.pFrame->Control = 0x03;
    } else {
        goto Cleanup;
    }

    for (i = 0; i < NUM_CP; i++) {
        gCpGetInfo[i](&gCpTable[i]);
    }

    gQueue.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (gQueue.hEvent == NULL) {
        goto Cleanup;
    }

    if (!InitModem()) {
        goto Cleanup;
    }

    strcpy(gInfo.szUsername, pszUsername);
    strcpy(gInfo.szPassword, pszPassword);
    strcpy(gInfo.szDomain, pszDomain);
    strcpy(gInfo.szNumber, pszNumber);

    if (!DialModem()) {
        goto Cleanup;
    }

    gFraming.SendAccm = 0xFFFFFFFF;
    gFraming.RecvAccm = 0xFFFFFFFF;

    if (!FsmInit(CP_LCP)) {
        goto Cleanup;
    }

    hThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, &dwTid);
    if (hThread == NULL) {
        goto Cleanup;
    }
    CloseHandle(hThread);

    FsmOpen(CP_LCP);
    FsmUp(CP_LCP);

    fSuccess = TRUE;

Cleanup:

    if (!fSuccess) {

        ModemClose();

        if (gQueue.hEvent != NULL) {
            CloseHandle(gQueue.hEvent);
            gQueue.hEvent = NULL;
        }

        if (gInfo.pFrame != NULL) {
            Free((PBYTE)gInfo.pFrame);
            gInfo.pFrame = NULL;
        }
    }

    return fSuccess;
}

/***************************************************************************\
* DllMain
*
\***************************************************************************/

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID pvReserved)
{
    switch (dwReason) {
    case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}

