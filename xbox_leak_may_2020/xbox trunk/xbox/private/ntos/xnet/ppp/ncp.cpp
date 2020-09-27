/*++

Copyright (c) 2000 Microsoft Corporation

    ncp.cpp

Abstract:

    Network control protocol and IPCP implementation.

Revision History:

    07-27-00    vadimg      created

--*/

#include "precomp.h"

/***************************************************************************\
* NcpStart
*
\***************************************************************************/

BOOL NcpStart(DWORD CpIndex)
{
    if (!FsmInit(CpIndex)) {
        return FALSE;
    }

    FsmOpen(CpIndex);
    FsmUp(CpIndex);

    return TRUE;
}

/***************************************************************************\
* NcpReset
*
\***************************************************************************/

VOID NcpReset(VOID)
{
    gIp.IpAddrRemote = 0;
    gIp.IpAddrLocal = 0;
    gIp.IpAddrDns = 0;
}

/***************************************************************************\
* AddIpAddressOption
*
\***************************************************************************/

VOID AddIpAddressOption(BYTE *pBuf, BYTE bOption, IPADDR ipaddr)
{
    *pBuf++ = bOption;
    *pBuf++ = IPADDRESSOPTIONLEN;
    *((IPADDR*)pBuf) = ipaddr;
}

/***************************************************************************\
* RejectCheck
*
\***************************************************************************/

DWORD RejectCheck(PPP_CONFIG* pRecvBuf, PPP_CONFIG* pSendBuf, BOOL *pfRej)
{
    PPP_OPTION *pROption = (PPP_OPTION*)pRecvBuf->Data;
    PPP_OPTION *pSOption = (PPP_OPTION*)pSendBuf->Data;
    WORD cbPacket = WireToHostFormat16(pRecvBuf->Length);
    WORD cbLeft = cbPacket - PPP_CONFIG_HDR_LEN;

    *pfRej = FALSE;

    while (cbLeft > 0) {

        if (cbLeft < pROption->Length) {
            return ERROR_PPP_INVALID_PACKET;
        }

        if (pROption->Type == IPCP_IpCompression) {

            *pfRej = TRUE;
            CopyMemory((VOID*)pSOption, (VOID*)pROption, pROption->Length);
            pSOption = (PPP_OPTION*)((BYTE*)pSOption + pROption->Length);

        } else {

            IPADDR ipaddr;

            BOOL fBad = (pROption->Type != IPCP_IpAddress || 
                    pROption->Length != IPADDRESSOPTIONLEN);

            if (!fBad) {
                CopyMemory(&ipaddr, pROption->Data, sizeof(IPADDR));
            }

            if (fBad || ipaddr == 0) {

                *pfRej = TRUE;

                CopyMemory((VOID*)pSOption, (VOID*)pROption, pROption->Length);
                pSOption = (PPP_OPTION*)((BYTE*)pSOption + pROption->Length);

            } else {
                gIp.IpAddrRemote = ipaddr;
            }
        }

        if (pROption->Length && pROption->Length < cbLeft) {
            cbLeft -= pROption->Length;
        } else {
            cbLeft = 0;
        }

        pROption = (PPP_OPTION*)((BYTE*)pROption + pROption->Length);
    }

    if (*pfRej) {
        pSendBuf->Code = CONFIG_REJ;
        HostToWireFormat16((WORD)((BYTE*)pSOption - (BYTE*)pSendBuf), pSendBuf->Length);
    }

    return NO_ERROR;
}

/***************************************************************************\
* IpcpMakeConfigRequest
*
\***************************************************************************/

DWORD IpcpMakeConfigRequest(PPP_CONFIG *pSendConfig, DWORD cbSendConfig)
{
    BYTE *pb = pSendConfig->Data;
    WORD cbPacket = PPP_CONFIG_HDR_LEN;

    AddIpAddressOption(pb, IPCP_IpAddress, gIp.IpAddrLocal);
    cbPacket += IPADDRESSOPTIONLEN;
    pb += IPADDRESSOPTIONLEN;

    AddIpAddressOption(pb, IPCP_DnsIpAddress, gIp.IpAddrDns);
    cbPacket += IPADDRESSOPTIONLEN;
    pb += IPADDRESSOPTIONLEN;

    HostToWireFormat16(cbPacket, pSendConfig->Length);

    return NO_ERROR;
}

/***************************************************************************\
* IpcpMakeConfigResult
*
\***************************************************************************/

DWORD IpcpMakeConfigResult(PPP_CONFIG *pRecvBuf, PPP_CONFIG *pSendBuf,
        DWORD cbSendBuf, BOOL fRejectNaks)
{
    DWORD dwErr;
    BOOL fRej;
    WORD cbPacket;

    dwErr = RejectCheck(pRecvBuf, pSendBuf, &fRej);
    if (dwErr != 0) {
        return dwErr;
    }

    if (fRej) {
        return NO_ERROR;
    }

    cbPacket = WireToHostFormat16(pRecvBuf->Length);
    CopyMemory(pSendBuf, pRecvBuf, cbPacket);
    pSendBuf->Code = CONFIG_ACK;

    return 0;
}

/***************************************************************************\
* IpcpConfigAckReceived
*
\***************************************************************************/

DWORD IpcpConfigAckReceived(PPP_CONFIG *pRecvBuf)
{
    return NO_ERROR;
}

/***************************************************************************\
* IpcpConfigRejReceived
*
\***************************************************************************/

DWORD IpcpConfigRejReceived(PPP_CONFIG *pRecvBuf)
{
    return NO_ERROR;
}

/***************************************************************************\
* IpcpConfigNakReceived
*
\***************************************************************************/

DWORD IpcpConfigNakReceived(PPP_CONFIG* pRecvBuf)
{
    PPP_OPTION *pROption = (PPP_OPTION*)pRecvBuf->Data;
    WORD cbPacket = WireToHostFormat16(pRecvBuf->Length);
    WORD cbLeft = cbPacket - PPP_CONFIG_HDR_LEN;
    IPADDR ipaddr;

    while (cbLeft > 0) {

        if (cbLeft < pROption->Length) {
            return ERROR_PPP_INVALID_PACKET;
        }

        switch (pROption->Type) {
        case IPCP_IpAddress:
            if (pROption->Length != IPADDRESSOPTIONLEN) {
                return ERROR_PPP_INVALID_PACKET;
            }

            CopyMemory(&ipaddr, pROption->Data, sizeof(IPADDR));

            if (ipaddr == 0) {
                return ERROR_PPP_INVALID_PACKET;
            }

            gIp.IpAddrLocal = ipaddr;
            break;

        case IPCP_DnsIpAddress:
            if (pROption->Length != IPADDRESSOPTIONLEN) {
                return ERROR_PPP_INVALID_PACKET;
            }

            CopyMemory(&gIp.IpAddrDns, pROption->Data, sizeof(IPADDR));
            break;
        }

        if (pROption->Length && pROption->Length < cbLeft) {
            cbLeft -= pROption->Length;
        } else {
            cbLeft = 0;
        }

        pROption = (PPP_OPTION*)((BYTE*)pROption + pROption->Length);
    }

    return NO_ERROR;
}

/***************************************************************************\
* IpcpBegin
*
\***************************************************************************/

DWORD IpcpBegin(VOID)
{
    return NO_ERROR;
}

/***************************************************************************\
* IpcpEnd
*
\***************************************************************************/

DWORD IpcpEnd(VOID)
{
    return NO_ERROR;
}

/***************************************************************************\
* GetIpcpInfo
*
\***************************************************************************/

VOID GetIpcpInfo(PPP_CP_INFO *pInfo)
{
    ZeroMemory(pInfo, sizeof(PPP_CP_INFO));
    pInfo->Protocol = PPP_IPCP_PROTOCOL;
    pInfo->Recognize = MAX_IPCP_CODE;
    pInfo->CP_Begin = IpcpBegin;
    pInfo->CP_End = IpcpEnd;
    pInfo->CP_MakeConfigRequest = IpcpMakeConfigRequest;
    pInfo->CP_MakeConfigResult = IpcpMakeConfigResult;
    pInfo->CP_ConfigAckReceived = IpcpConfigAckReceived;
    pInfo->CP_ConfigRejReceived = IpcpConfigRejReceived;
    pInfo->CP_ConfigNakReceived = IpcpConfigNakReceived;
}

