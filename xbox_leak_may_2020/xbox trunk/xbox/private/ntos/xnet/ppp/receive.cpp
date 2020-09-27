/*++

Copyright (c) 2000 Microsoft Corporation

    receive.cpp

Abstract:

    Handle received packets.

Revision History:

    07-18-00    vadimg      created

--*/

#include "precomp.h"

VOID (*gReceive[])(DWORD CpIndex, PPP_CONFIG *pRecvConfig) = {
        NULL,
        ReceiveConfigReq,
        ReceiveConfigAck,
        ReceiveConfigNakRej,
        ReceiveConfigNakRej,
        ReceiveTermReq,
        ReceiveTermAck,
        ReceiveCodeRej,
        NULL,
        ReceiveEchoReq,
        ReceiveEchoReply,
        ReceiveDiscardReq
};

/***************************************************************************\
* ReceiveConfigReq
*
\***************************************************************************/

VOID ReceiveConfigReq(DWORD CpIndex, PPP_CONFIG *pRecvConfig)
{
    PPP_PACKET *pSendPacket = GetSendPacket();
    PPP_CONFIG *pSendConfig = (PPP_CONFIG*)pSendPacket->Data;
    BOOL fAcked;

    switch (gInfo.State) {
    case FSM_OPENED:            
        if (!FsmThisLayerDown(CpIndex)) {
            return;
        }
        if (!FsmSendConfigReq(CpIndex)) {
            return;
        }
        if (!FsmSendConfigResult(CpIndex, pRecvConfig, &fAcked)) {
            return;
        }

        gInfo.State = fAcked ? FSM_ACK_SENT : FSM_REQ_SENT;
        break;

    case FSM_STOPPED:

        InitRestartCounters();

        if (!FsmSendConfigReq(CpIndex)) {
            return;
        }

        // fall through

    case FSM_REQ_SENT:
    case FSM_ACK_SENT:          
        if (!FsmSendConfigResult(CpIndex, pRecvConfig, &fAcked)) {
            return;
        }

        gInfo.State = fAcked ? FSM_ACK_SENT : FSM_REQ_SENT;
        break;

    case FSM_ACK_RCVD:
        if (!FsmSendConfigResult(CpIndex, pRecvConfig, &fAcked)) {
            return;
        }

        if (fAcked) {
            gInfo.State = FSM_OPENED;
            FsmThisLayerUp(CpIndex);
        }
        break;

    case FSM_CLOSED:
        FsmSendTermAck(CpIndex, pRecvConfig);
        break;

    case FSM_CLOSING:
    case FSM_STOPPING:
        break;
    }
}

/***************************************************************************\
* ReceiveConfigAck
*
\***************************************************************************/

VOID ReceiveConfigAck(DWORD CpIndex, PPP_CONFIG *pRecvConfig)
{
    // The Id of the Ack HAS to match the Id of the last request sent
    // If it is different, then we should silently discard it.
    if (pRecvConfig->Id != gInfo.LastId) {
        return;
    }

    ClearTimeout();

    switch (gInfo.State) {
    case FSM_REQ_SENT:
        if (!FsmConfigResultReceived(CpIndex, pRecvConfig)) {
            return;
        }

        InitRestartCounters();
        gInfo.State = FSM_ACK_RCVD;
        break;

    case FSM_ACK_SENT:
        if (!FsmConfigResultReceived(CpIndex, pRecvConfig)) {
            return;
        }

        InitRestartCounters();
        gInfo.State = FSM_OPENED;
        FsmThisLayerUp(CpIndex);
        break;

    case FSM_OPENED:            
        if (!FsmThisLayerDown(CpIndex)) {
            return;
        }

        // fall through

    case FSM_ACK_RCVD:  
        if (!FsmSendConfigReq(CpIndex)) {
            return;
        }

        gInfo.State = FSM_REQ_SENT;
        break;

    case FSM_CLOSED:
    case FSM_STOPPED:
        // Out of Sync; kill the remote 
        FsmSendTermAck(CpIndex, pRecvConfig);
        break;

    case FSM_CLOSING:
    case FSM_STOPPING:
        break;
    }
}

/***************************************************************************\
* ReceiveConfigNakRej
*
\***************************************************************************/

VOID ReceiveConfigNakRej(DWORD CpIndex, PPP_CONFIG *pRecvConfig)
{
    // The Id of the Nak/Rej HAS to match the Id of the last request sent
    // If it is different, then we should silently discard it.
    if (pRecvConfig->Id != gInfo.LastId) {
        return;
    }

    ClearTimeout();

    switch (gInfo.State) {
    case FSM_REQ_SENT:
    case FSM_ACK_SENT:
        if (!FsmConfigResultReceived(CpIndex, pRecvConfig)) {
            return;
        }

        InitRestartCounters();

        if (!FsmSendConfigReq(CpIndex)) {
            return;
        }
        break;

    case FSM_OPENED:            
        if (!FsmThisLayerDown(CpIndex)) {
            return;
        }

        // fall through

    case FSM_ACK_RCVD:          
        if (!FsmSendConfigReq(CpIndex)) {
            return;
        }

        gInfo.State = FSM_REQ_SENT;
        break;

    case FSM_CLOSED:
    case FSM_STOPPED:
        // Out of Sync; kill the remote 
        FsmSendTermAck(CpIndex, pRecvConfig);
        break;

    case FSM_CLOSING:
    case FSM_STOPPING:
        break;
    }
}

/***************************************************************************\
* ReceiveTermReq
*
\***************************************************************************/

VOID ReceiveTermReq(DWORD CpIndex, PPP_CONFIG *pConfig)
{
    switch (gInfo.State) {
    case FSM_OPENED:
        if (!FsmThisLayerDown(CpIndex)) {
            return;
        }

        FsmSendTermAck(CpIndex, pConfig);
        gInfo.State = FSM_STOPPING;
        break;

    case FSM_ACK_RCVD:
    case FSM_ACK_SENT:
    case FSM_REQ_SENT:
        FsmSendTermAck(CpIndex, pConfig);
        gInfo.State = FSM_REQ_SENT;
        break;

    case FSM_CLOSED:
    case FSM_CLOSING:
    case FSM_STOPPED:
    case FSM_STOPPING:
        FsmSendTermAck(CpIndex, pConfig);
        break;
    }

    FsmThisLayerFinished(CpIndex);
}

/***************************************************************************\
* ReceiveTermAck
*
\***************************************************************************/

VOID ReceiveTermAck(DWORD CpIndex, PPP_CONFIG *pRecvConfig)
{
    // The Id of the Term Ack HAS to match the Id of the last request sent
    // If it is different, then we should silently discard it.
    if (pRecvConfig->Id != gInfo.LastId) {
        return;
    }

    switch (gInfo.State) {
    case FSM_OPENED:
        if (!FsmThisLayerDown(CpIndex)) {
            return;
        }

        if (!FsmSendConfigReq(CpIndex)) {
            return;
        }

        gInfo.State = FSM_REQ_SENT;
        break;

    case FSM_ACK_RCVD:
        gInfo.State = FSM_REQ_SENT;
        break;

    case FSM_CLOSING:
    case FSM_STOPPING:
        if (!FsmThisLayerFinished(CpIndex)) {
            return;
        }

        gInfo.State = (gInfo.State == FSM_CLOSING) ? FSM_CLOSED : FSM_STOPPED;
        break;

    case FSM_REQ_SENT:
    case FSM_ACK_SENT:
    case FSM_CLOSED:
    case FSM_STOPPED:
        break;
    }
}

/***************************************************************************\
* ReceiveUnknownCode 
*
\***************************************************************************/

VOID ReceiveUnknownCode(DWORD CpIndex, PPP_CONFIG *pConfig)
{
    switch (gInfo.State) {
    case FSM_STOPPED:
    case FSM_STOPPING:
    case FSM_OPENED:
    case FSM_ACK_SENT:
    case FSM_ACK_RCVD:
    case FSM_REQ_SENT:
    case FSM_CLOSING:
    case FSM_CLOSED:
        FsmSendCodeReject(CpIndex, pConfig);
        break;
    }
}

/***************************************************************************\
* ReceiveDiscardReq
*
\***************************************************************************/

VOID ReceiveDiscardReq(DWORD CpIndex, PPP_CONFIG *pConfig)
{
}

/***************************************************************************\
* ReceiveEchoReq
*
\***************************************************************************/

VOID ReceiveEchoReq(DWORD CpIndex, PPP_CONFIG *pConfig)
{
    // Silently discard this packet if LCP is not in an opened state
    if (!IsLcpOpened()) {
        return;
    }

    switch (gInfo.State) {
    case FSM_STOPPED:
    case FSM_STOPPING:
    case FSM_ACK_SENT:
    case FSM_ACK_RCVD:
    case FSM_REQ_SENT:
    case FSM_CLOSING:
    case FSM_CLOSED:
    case FSM_STARTING:
    case FSM_INITIAL:
        break;

    case FSM_OPENED:
        FsmSendEchoReply(CpIndex, pConfig);
        break;
    }
}

/***************************************************************************\
* ReceiveEchoReply
*
\***************************************************************************/

VOID ReceiveEchoReply(DWORD CpIndex, PPP_CONFIG *pConfig)
{
}

/***************************************************************************\
* ReceiveCodeRej
*
\***************************************************************************/

VOID ReceiveCodeRej(DWORD CpIndex, PPP_CONFIG *pConfig)
{
    pConfig = (PPP_CONFIG*)pConfig->Data;

    // First check to see if these codes may be rejected without 
    // affecting implementation. Permitted code rejects
    if (CpIndex == CP_LCP) {
        switch (pConfig->Code) {
        case CONFIG_REQ:
        case CONFIG_ACK:
        case CONFIG_NAK:
        case CONFIG_REJ:
        case TERM_REQ:
        case TERM_ACK:
        case CODE_REJ:
        case PROT_REJ:
        case ECHO_REQ:
        case ECHO_REPLY:
        case DISCARD_REQ:
            // Unpermitted code rejects.
            break;

        default:
            // Permitted code rejects, we can still work.
            switch (gInfo.State) {
            case FSM_ACK_RCVD:
                gInfo.State = FSM_REQ_SENT;
                break;
            }

            return;
        }
    }
    // Actually the remote side did not reject the protocol, it rejected
    // the code. But for all practical purposes we cannot talk with
    // the corresponding CP on the remote side. This is actually an
    // implementation error in the remote side.
    gInfo.dwError = ERROR_PPP_NOT_CONVERGING;

    switch (gInfo.State) {
    case FSM_CLOSING:
        if (!FsmThisLayerFinished(CpIndex)) {
            return;
        }
        gInfo.State = FSM_CLOSED;
        break;

    case FSM_REQ_SENT:
    case FSM_ACK_RCVD:
    case FSM_ACK_SENT:
    case FSM_STOPPING:
        if (!FsmThisLayerFinished(CpIndex)) {
            return;
        }
        gInfo.State = FSM_STOPPED;
        break;

    case FSM_OPENED:
        if (!FsmThisLayerDown(CpIndex)) {
            return;
        }

        InitRestartCounters();
        FsmSendTermReq(CpIndex);
        gInfo.State = FSM_STOPPING;
        break;

    case FSM_CLOSED:
    case FSM_STOPPED:
        break;
    }
}

/***************************************************************************\
* ReceiveProtocolRej
*
\***************************************************************************/

VOID ReceiveProtocolRej(PPP_PACKET *pPacket)
{
    PPP_CONFIG *pRecvConfig = (PPP_CONFIG*)pPacket->Data;
    DWORD dwProtocol  = WireToHostFormat16(pRecvConfig->Data);
    DWORD CpIndex;

    CpIndex = GetCpIndexFromProtocol(dwProtocol);

    if (CpIndex == (DWORD)-1) {
        return;
    }

    if (CpIndex == CP_LCP) {
        gInfo.dwError = ERROR_PPP_NOT_CONVERGING;
        return;
    }

    // If LCP is not in the opened state we silently discard this packet 
    if (!IsLcpOpened()) {
        return;
    }

    gInfo.dwError = ERROR_PPP_CP_REJECTED;

    switch (gInfo.State) {
    case FSM_CLOSING:
        if (!FsmThisLayerFinished(CpIndex)) {
            return;
        }

        gInfo.State = FSM_CLOSED;
        break;

    case FSM_REQ_SENT:
    case FSM_ACK_RCVD:
    case FSM_ACK_SENT:
    case FSM_STOPPING:
        if (!FsmThisLayerFinished(CpIndex)) {
            return;
        }

        gInfo.State = FSM_STOPPED;
        break;

    case FSM_OPENED:
        if (!FsmThisLayerDown(CpIndex)) {
            return;
        }

        InitRestartCounters();
        FsmSendTermReq(CpIndex);
        gInfo.State = FSM_STOPPING;
        break;

    case FSM_CLOSED:
    case FSM_STOPPED:
        break;
    }
}

/***************************************************************************\
* FsmConfigResultReceived
*
\***************************************************************************/

BOOL FsmConfigResultReceived(DWORD CpIndex, PPP_CONFIG *pRecvConfig)
{
    DWORD dwRetCode; 

    switch (pRecvConfig->Code) {
    case CONFIG_NAK:
        dwRetCode = gCpTable[CpIndex].CP_ConfigNakReceived(pRecvConfig);
        break;

    case CONFIG_ACK:
        dwRetCode = gCpTable[CpIndex].CP_ConfigAckReceived(pRecvConfig);
        break;

    case CONFIG_REJ:
        dwRetCode = gCpTable[CpIndex].CP_ConfigRejReceived(pRecvConfig);
        break;

    default:
        return FALSE;
    }

    if (dwRetCode != NO_ERROR) {
        if (dwRetCode == ERROR_PPP_INVALID_PACKET) {
            return TRUE;
        } else {
            FsmClose(CpIndex);
            return FALSE;
        }
    }

    return TRUE;
}

/***************************************************************************\
* FsmReceive
*
\***************************************************************************/

VOID FsmReceive(PPP_PACKET *pPacket, DWORD dwPacketLength)
{
    DWORD dwProtocol;
    DWORD CpIndex;
    PPP_CONFIG *pRecvConfig;
    DWORD dwLength;
    BOOL fAuth = FALSE;
    
    // Validate length of packet
    if (dwPacketLength < (PPP_PACKET_HDR_LEN + PPP_CONFIG_HDR_LEN)) {
        return;
    }

    dwProtocol = WireToHostFormat16(pPacket->Protocol);
    CpIndex = GetCpIndexFromProtocol(dwProtocol);
    if (CpIndex == (DWORD)-1) {
        DbgPrint("Unknown protocol in PPP packet\n");
        return;
    }

    switch (gInfo.Phase) {
    case PPP_AP:
        if (CpIndex == GetCpIndexFromProtocol(gLcp.Remote.Work.AP)) {
            fAuth = TRUE;
            break;
        }

        // fall through

    case PPP_LCP:
        if (CpIndex == CP_LCP) {
            break;
        }

        return;
    }

    pRecvConfig = (PPP_CONFIG*)pPacket->Data;

    dwLength = WireToHostFormat16(pRecvConfig->Length);

    if (dwLength > (dwPacketLength - PPP_PACKET_HDR_LEN) || 
            (dwLength < PPP_CONFIG_HDR_LEN)) {
        return;
    }

    // Not in ProcessPacket table since parameters to this are different.
    if (CpIndex == CP_LCP && pRecvConfig->Code == PROT_REJ) {
        ReceiveProtocolRej(pPacket);
        return;
    }

    // Make sure that the protocol can handle the config code sent.
    if (pRecvConfig->Code == 0 || 
            pRecvConfig->Code > gCpTable[CpIndex].Recognize) {
        ReceiveUnknownCode(CpIndex, pRecvConfig);
        return;
    }

    if (fAuth) {
        ApWork(CpIndex, pRecvConfig);
    } else {
        gReceive[pRecvConfig->Code](CpIndex, pRecvConfig);
    }
}

