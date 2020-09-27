/*++

Copyright (c) 2000 Microsoft Corporation

    smaction.cpp

Abstract:

    Handle actions that occur during state transitions.

Revision History:

    07-14-00    vadimg      created

--*/

#include "precomp.h"

/***************************************************************************\
* FsmSendConfigReq
*
\***************************************************************************/

BOOL FsmSendConfigReq(DWORD CpIndex)
{
    DWORD dwRetCode;
    PPP_PACKET *pSendPacket = GetSendPacket();
    PPP_CONFIG *pSendConfig = (PPP_CONFIG*)pSendPacket->Data;
    DWORD dwLength;

    dwRetCode = (gCpTable[CpIndex].CP_MakeConfigRequest)(pSendConfig,
            LCP_DEFAULT_MRU - PPP_PACKET_HDR_LEN);

    if (dwRetCode != NO_ERROR) {
        gInfo.dwError = dwRetCode;
        FsmClose(CpIndex);
        return FALSE;
    }

    HostToWireFormat16((WORD)gCpTable[CpIndex].Protocol,
            (PBYTE)pSendPacket->Protocol);

    pSendConfig->Code = CONFIG_REQ;

    //
    // If we are resending a configure request because of a timeout, we do not
    // use the id of the previous configure request, instead we get a new Id.
    // Id we do not, then the wrong Config-Req's and Config-Acks may be matched
    // up and we start getting crossed connections.
    //

    pSendConfig->Id = GetId();

    dwLength = WireToHostFormat16(pSendConfig->Length);

    SendFrame(dwLength + PPP_PACKET_HDR_LEN);

    gInfo.LastId = pSendConfig->Id;

    SetTimeout();

    return TRUE;
}

/***************************************************************************\
* FsmSendTermReq
*
\***************************************************************************/

BOOL FsmSendTermReq(DWORD CpIndex)
{
    PPP_PACKET *pSendPacket = GetSendPacket();
    PPP_CONFIG *pSendConfig = (PPP_CONFIG*)pSendPacket->Data;

    HostToWireFormat16((WORD)gCpTable[CpIndex].Protocol,
                    (PBYTE)pSendPacket->Protocol);

    pSendConfig->Code = TERM_REQ;
    pSendConfig->Id = GetId();

    HostToWireFormat16((WORD)((PPP_CONFIG_HDR_LEN)+(sizeof(DWORD)*3)),
            (PBYTE)pSendConfig->Length);

    HostToWireFormat32(gLcp.Local.Work.MagicNumber, (PBYTE)pSendConfig->Data);
    HostToWireFormat32(777, (PBYTE)(pSendConfig->Data+4));
    HostToWireFormat32(gInfo.dwError, (PBYTE)(pSendConfig->Data+8));

    SendFrame(PPP_PACKET_HDR_LEN + PPP_CONFIG_HDR_LEN + sizeof(DWORD) * 3);

    gInfo.LastId = pSendConfig->Id;
    return TRUE;
}

/***************************************************************************\
* FsmSendTermAck
*
\***************************************************************************/

BOOL FsmSendTermAck(DWORD CpIndex, PPP_CONFIG *pRecvConfig)
{
    PPP_PACKET *pSendPacket = GetSendPacket();
    PPP_CONFIG *pSendConfig = (PPP_CONFIG*)pSendPacket->Data;
    DWORD dwLength;

    dwLength = PPP_PACKET_HDR_LEN + WireToHostFormat16(pRecvConfig->Length);
    if (dwLength > LCP_DEFAULT_MRU) {
        dwLength = LCP_DEFAULT_MRU;
    }

    HostToWireFormat16((WORD)gCpTable[CpIndex].Protocol,
            (PBYTE)pSendPacket->Protocol);

    pSendConfig->Code = TERM_ACK;
    pSendConfig->Id = pRecvConfig->Id;

    HostToWireFormat16((WORD)(dwLength - PPP_PACKET_HDR_LEN),
            (PBYTE)pSendConfig->Length);

    CopyMemory(pSendConfig->Data, pRecvConfig->Data,
            dwLength - PPP_CONFIG_HDR_LEN - PPP_PACKET_HDR_LEN);

    SendFrame(dwLength);

    return TRUE;
}

/***************************************************************************\
* FsmSendConfigResult
*
\***************************************************************************/

BOOL FsmSendConfigResult(DWORD CpIndex, PPP_CONFIG *pRecvConfig, BOOL *pfAcked)
{
    PPP_PACKET *pSendPacket = GetSendPacket();
    PPP_CONFIG *pSendConfig = (PPP_CONFIG*)pSendPacket->Data;
    DWORD dwLength;
    DWORD dwRetCode;

    *pfAcked = FALSE;

    ZeroMemory(pSendConfig, 30);

    pSendConfig->Id = pRecvConfig->Id;

    dwRetCode = (gCpTable[CpIndex].CP_MakeConfigResult)(pRecvConfig, pSendConfig,
            LCP_DEFAULT_MRU - PPP_PACKET_HDR_LEN, (gInfo.NakRetryCount == 0));

    if (dwRetCode == ERROR_PPP_INVALID_PACKET) {

        return FALSE;

    } else if (dwRetCode != NO_ERROR) {

        gInfo.dwError = dwRetCode;
        FsmClose(CpIndex);
        return FALSE;
    }

    switch (pSendConfig->Code) {
    case CONFIG_ACK:
        *pfAcked = TRUE;
        break;

    case CONFIG_NAK:
        if (gInfo.NakRetryCount > 0) {
            gInfo.NakRetryCount--;
        } else {

            gInfo.dwError = ERROR_PPP_NOT_CONVERGING;
            FsmClose(CP_LCP);
            return FALSE;
        }
        break;

    case CONFIG_REJ:
        if (gInfo.RejRetryCount > 0) {
            gInfo.RejRetryCount--;
        } else {
    
            gInfo.dwError = ERROR_PPP_NOT_CONVERGING;
            FsmClose(CP_LCP);
            return FALSE;
        }
        break;
    }

    HostToWireFormat16((WORD)gCpTable[CpIndex].Protocol,
            (PBYTE)pSendPacket->Protocol);

    pSendConfig->Id = pRecvConfig->Id;
    dwLength = WireToHostFormat16(pSendConfig->Length);

    SendFrame(dwLength + PPP_PACKET_HDR_LEN);

    return TRUE;
}

/***************************************************************************\
* FsmSendEchoReply
*
\***************************************************************************/

BOOL FsmSendEchoReply(DWORD CpIndex, PPP_CONFIG *pRecvConfig)
{
    PPP_PACKET *pSendPacket = GetSendPacket();
    PPP_CONFIG *pSendConfig = (PPP_CONFIG*)pSendPacket->Data;
    DWORD dwLength;

    dwLength =  PPP_PACKET_HDR_LEN + WireToHostFormat16(pRecvConfig->Length);
    if (dwLength > LCP_DEFAULT_MRU) {
        dwLength = LCP_DEFAULT_MRU;
    }

    if (dwLength < PPP_PACKET_HDR_LEN + PPP_CONFIG_HDR_LEN + 4) {
        return FALSE;
    }

    HostToWireFormat16((WORD)gCpTable[CpIndex].Protocol,
            (PBYTE)pSendPacket->Protocol);

    pSendConfig->Code = ECHO_REPLY;
    pSendConfig->Id = pRecvConfig->Id;

    HostToWireFormat16((WORD)(dwLength - PPP_PACKET_HDR_LEN),
            (PBYTE)pSendConfig->Length);

    HostToWireFormat32(gLcp.Local.Work.MagicNumber, (PBYTE)pSendConfig->Data);

    CopyMemory(pSendConfig->Data + 4, pRecvConfig->Data + 4,
            dwLength - PPP_CONFIG_HDR_LEN - PPP_PACKET_HDR_LEN - 4);

    SendFrame(dwLength);

    return TRUE;
}

/***************************************************************************\
* FsmSendCodeReject
*
\***************************************************************************/

BOOL FsmSendCodeReject(DWORD CpIndex, PPP_CONFIG *pRecvConfig)
{
    PPP_PACKET *pSendPacket = GetSendPacket();
    PPP_CONFIG *pSendConfig = (PPP_CONFIG*)pSendPacket->Data;
    DWORD dwLength;
        
    dwLength = PPP_PACKET_HDR_LEN + PPP_CONFIG_HDR_LEN +
            WireToHostFormat16(pRecvConfig->Length);

    if (dwLength > LCP_DEFAULT_MRU) {
        dwLength = LCP_DEFAULT_MRU;
    }

    HostToWireFormat16((WORD)gCpTable[CpIndex].Protocol,
            (PBYTE)pSendPacket->Protocol);

    pSendConfig->Code = CODE_REJ;
    pSendConfig->Id = GetId();

    HostToWireFormat16((WORD)(dwLength - PPP_PACKET_HDR_LEN),
            (PBYTE)pSendConfig->Length);

    CopyMemory( pSendConfig->Data,
                pRecvConfig,
                dwLength - PPP_CONFIG_HDR_LEN - PPP_PACKET_HDR_LEN );

    SendFrame(dwLength);

    return TRUE;
}

/***************************************************************************\
* FsmSendProtocolRej
*
\***************************************************************************/

BOOL FsmSendProtocolRej(PPP_PACKET *pPacket, DWORD dwPacketLength)
{
    PPP_PACKET *pSendPacket = GetSendPacket();
    PPP_CONFIG *pSendConfig = (PPP_CONFIG*)pSendPacket->Data;
    PPP_CONFIG *pRecvConfig = (PPP_CONFIG*)pPacket->Data;
    DWORD dwLength;

    // If LCP isn't in the opened state we can't send a protocol reject packet
    if (!IsLcpOpened()) {
        return ERROR_UNKNOWN;
    }

    dwLength = PPP_PACKET_HDR_LEN + PPP_CONFIG_HDR_LEN + dwPacketLength;

    if (dwLength > LCP_DEFAULT_MRU) {
        dwLength = LCP_DEFAULT_MRU;
    }

    HostToWireFormat16((WORD)gCpTable[CP_LCP].Protocol,
            (PBYTE)pSendPacket->Protocol);

    pSendConfig->Code = PROT_REJ;
    pSendConfig->Id = GetId();

    HostToWireFormat16((WORD)(dwLength - PPP_PACKET_HDR_LEN),
            (PBYTE)pSendConfig->Length);

    CopyMemory(pSendConfig->Data, pPacket,
                dwLength - PPP_CONFIG_HDR_LEN - PPP_PACKET_HDR_LEN);

    SendFrame(dwLength);

    return TRUE;
}

/***************************************************************************\
* FsmInit
*
\***************************************************************************/

BOOL FsmInit(DWORD CpIndex)
{
    DWORD dwRetCode;

    gInfo.dwError = NO_ERROR;
    gInfo.State = FSM_INITIAL;

    if (!FsmReset(CpIndex)) {
        return FALSE;
    }

    dwRetCode = gCpTable[CpIndex].CP_Begin();
    if (dwRetCode != NO_ERROR) {
        gInfo.dwError = dwRetCode;
        return FALSE;
    }

    return TRUE;
}

/***************************************************************************\
* FsmReset
*
\***************************************************************************/

BOOL FsmReset(DWORD CpIndex)
{
    InitRestartCounters();

    gInfo.NakRetryCount = PPP_MAX_FAILURE;
    gInfo.RejRetryCount = PPP_MAX_FAILURE;

    gInfo.LastId = 0;
    return TRUE;
}

/***************************************************************************\
* FsmThisLayerUp
*
\***************************************************************************/

BOOL FsmThisLayerUp(DWORD CpIndex)
{
    DWORD dwRetCode;

    DbgPrint("LcpThisLayerUp %4X\n", gCpTable[CpIndex].Protocol);

    if (gCpTable[CpIndex].CP_ThisLayerUp != NULL) {

        dwRetCode = gCpTable[CpIndex].CP_ThisLayerUp();
        if (dwRetCode != NO_ERROR) {
            gInfo.dwError = dwRetCode;
            FsmClose(CpIndex);
            return FALSE;
        }
    }

    switch (gInfo.Phase) {
    case PPP_LCP:
        gInfo.Phase = PPP_AP;
        CpIndex = GetCpIndexFromProtocol(gLcp.Remote.Work.AP);
        ApStart(CpIndex);
        break;

    case PPP_AP:
        gInfo.Phase = PPP_NCP;
        CpIndex = CP_IPCP;
        NcpStart(CpIndex);
        break;

    case PPP_NCP:
        InitNet();
        break;
    }

    return TRUE;
}

/***************************************************************************\
* FsmThisLayerDown
*
\***************************************************************************/

BOOL FsmThisLayerDown(DWORD CpIndex)
{
    DWORD dwRetCode;

    DbgPrint("LcpThisLayerDown %4X\n", gCpTable[CpIndex].Protocol);

    if (gCpTable[CpIndex].CP_ThisLayerDown != NULL) {
        dwRetCode = gCpTable[CpIndex].CP_ThisLayerDown();
        if (dwRetCode != NO_ERROR) {
            gInfo.dwError = dwRetCode;
            FsmClose(CpIndex);
            return FALSE;
        }
    }

    if (CpIndex == CP_LCP) {
        // bring down all NCPs
        // bring down AP
    }
    return TRUE;
}

/***************************************************************************\
* FsmThisLayerStarted
*
\***************************************************************************/

BOOL FsmThisLayerStarted(DWORD CpIndex)
{
    DWORD dwRetCode;

    if (gCpTable[CpIndex].CP_ThisLayerStarted != NULL) {
        dwRetCode = gCpTable[CpIndex].CP_ThisLayerStarted();
        if (dwRetCode != NO_ERROR) {
            gInfo.dwError = dwRetCode;
            FsmClose(CpIndex);
            return FALSE;
        }
    }
    return TRUE;
}

/***************************************************************************\
* FsmThisLayerFinished
*
\***************************************************************************/

BOOL FsmThisLayerFinished(DWORD CpIndex)
{
    DWORD dwRetCode;

    if (gCpTable[CpIndex].CP_ThisLayerFinished != NULL) {
        dwRetCode = gCpTable[CpIndex].CP_ThisLayerFinished();
        if (dwRetCode != NO_ERROR) {
            gInfo.dwError = dwRetCode;
            return FALSE;
        }
    }
    return TRUE;
}

