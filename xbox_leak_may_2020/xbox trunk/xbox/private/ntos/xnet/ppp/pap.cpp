/*++

Copyright (c) 2000 Microsoft Corporation

    pap.cpp

Abstract:

    Password authentication protocol.

Revision History:

    07-26-00    vadimg      created

--*/

#include "precomp.h"

/***************************************************************************\
* PapBegin
*
\***************************************************************************/

DWORD PapBegin(VOID)
{
    gAp.State = PS_Initial;
    return NO_ERROR;
}

/***************************************************************************\
* PapEnd
*
\***************************************************************************/

DWORD PapEnd(VOID)
{
    return NO_ERROR;
}

/***************************************************************************\
* PapMakeRequestMessage
*
\***************************************************************************/

VOID PapMakeRequestMessage(PPP_CONFIG *pSendBuf)
{
    BYTE *pcbPeerId;
    BYTE *pcbPassword;
    WORD wLength;
    CHAR szAccount[DNLEN + 1 + UNLEN + 1];
    char *p = szAccount;

    if (gInfo.szDomain[0] != '\0') {
        p = str_add(p, gInfo.szDomain);
        p = str_add(p, "\\");
    }
    str_add(p, gInfo.szUsername);

    pcbPeerId = pSendBuf->Data;
    *pcbPeerId = (BYTE)strlen(szAccount);
    strcpy((char*)(pcbPeerId + 1), szAccount);

    pcbPassword = pcbPeerId + 1 + *pcbPeerId;
    *pcbPassword = (BYTE)strlen(gInfo.szPassword);
    strcpy((char*)(pcbPassword + 1), gInfo.szPassword);

    pSendBuf->Code = (BYTE)PAPCODE_Req;
    pSendBuf->Id = GetId();

    wLength = (WORD)(PPP_CONFIG_HDR_LEN + 1 + *pcbPeerId + 1 + *pcbPassword);
    HostToWireFormat16(wLength, pSendBuf->Length);
}

/***************************************************************************\
* PapMakeMessage
*
\***************************************************************************/

DWORD PapMakeMessage(PPP_CONFIG *pRecvBuf)
{
    PPP_PACKET *pSendPacket = GetSendPacket();
    PPP_CONFIG *pSendConfig = (PPP_CONFIG*)pSendPacket->Data;

    if (pRecvBuf == NULL && gAp.State != PS_Initial) {
        gAp.State = PS_Initial;
    }

    switch (gAp.State) {
    case PS_Initial:
        PapMakeRequestMessage(pSendConfig);
        gAp.Action = AP_SendWithTimeout;
        gAp.State = PS_RequestSent;
        break;

    case PS_RequestSent:
        if (pRecvBuf->Code == PAPCODE_Ack) {
            gInfo.dwError = 0;
            gAp.Action = AP_Done;
            gAp.State = PS_Done;
        } else if (pRecvBuf->Code == PAPCODE_Nak) {
            gInfo.dwError = ERROR_AUTHENTICATION_FAILURE;
            gAp.Action = AP_Done;
            gAp.State = PS_Done;
        } else {
            gAp.Action = AP_NoAction;
        }
    }

    return NO_ERROR;
}

/***************************************************************************\
* GetPapInfo
*
\***************************************************************************/

VOID GetPapInfo(PPP_CP_INFO *pInfo)
{
    ZeroMemory(pInfo, sizeof(PPP_CP_INFO));
    pInfo->Protocol = PPP_PAP_PROTOCOL;
    pInfo->Recognize = MAX_PAP_CODE;
    pInfo->CP_Begin = PapBegin;
    pInfo->CP_End = PapEnd;
    pInfo->CP_MakeMessage = PapMakeMessage;
}    

