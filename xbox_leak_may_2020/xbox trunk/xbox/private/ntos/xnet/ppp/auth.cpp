/*++

Copyright (c) 2000 Microsoft Corporation

    auth.cpp

Abstract:

    Handle authentication.

Revision History:

    07-18-00    vadimg      created

--*/

#include "precomp.h"

/***************************************************************************\
* ApWork
*
\***************************************************************************/

VOID ApWork(DWORD CpIndex, PPP_CONFIG *pRecvConfig)
{
    DWORD dwRetCode;
    DWORD dwLength;
    PPP_PACKET *pSendPacket = GetSendPacket();
    PPP_CONFIG *pSendConfig = (PPP_CONFIG*)pSendPacket->Data;

    dwRetCode = gCpTable[CpIndex].CP_MakeMessage(pRecvConfig);

    if (dwRetCode != NO_ERROR) {
        gInfo.dwError = dwRetCode;
        return;
    }

    switch (gAp.Action) {
    case AP_SendWithTimeout2:
        gInfo.ConfigRetryCount++;

        // fall through

    case AP_Send:
    case AP_SendWithTimeout:
    case AP_SendAndDone:
        HostToWireFormat16((WORD)gCpTable[CpIndex].Protocol,
                (PBYTE)pSendPacket->Protocol);

        dwLength = WireToHostFormat16(pSendConfig->Length);

        SendFrame(dwLength + PPP_PACKET_HDR_LEN);

        gInfo.LastId = (BYTE)-1;
        break;

    case AP_Done:
        if (gInfo.dwError != NO_ERROR) {
            return;
        }

        gInfo.State = FSM_OPENED;
        FsmThisLayerUp(CpIndex);
        break;
    }
}

/***************************************************************************\
* ApStart
*
\***************************************************************************/

BOOL ApStart(DWORD CpIndex)
{
    if (!FsmInit(CpIndex)) {
        return FALSE;
    }

    ApWork(CpIndex, NULL);

    return TRUE;
}

/***************************************************************************\
* ApReset
*
\***************************************************************************/

VOID ApReset(VOID)
{
}

