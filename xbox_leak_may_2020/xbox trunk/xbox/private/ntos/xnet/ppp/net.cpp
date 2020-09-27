/*++

Copyright (c) 2000 Microsoft Corporation

    net.cpp

Abstract:

    Network stack integration code.

Revision History:

    07-27-00    vadimg      created

--*/

#include "precomp.h"

/***************************************************************************\
* PppDeleteProc
*
\***************************************************************************/

VOID PppDeleteProc(IfInfo *ifp)
{
    while (!PktQIsEmpty(&ifp->sendq)) {
        Packet* pkt = PktQRemoveHead(&ifp->sendq);
        XnetCompletePacket(pkt, NETERR_CANCELLED);
    }
    SysFree(ifp);
}

/***************************************************************************\
* PppStartOutputProc
*
\***************************************************************************/

VOID PppStartOutputProc(IfInfo *ifp)
{
    Packet *pkt;
    PPP_PACKET *pSendPacket = GetSendPacket();

    while (!PktQIsEmpty(&ifp->sendq)) {
        pkt = IfDequeuePacket(ifp);

        pSendPacket->Protocol[0] = 0x00;
        pSendPacket->Protocol[1] = 0x21;

        CopyMemory(pSendPacket->Data, pkt->data, pkt->datalen);

        SendFrame(pkt->datalen + 2);

        XnetCompletePacket(pkt, NO_ERROR);
    }
}

/***************************************************************************\
* PppIoctlProc
*
* Arguments:
*
*   ifp - Points to the interface structure
*   ctlcode - Control code
*   inbuf - Points to the input buffer
*   inlen - Size of the input buffer
*   outbuf - Points to the output buffer
*   outlen - On entry, this contains the size of the output buffer
*       On return, this is the actually number of output bytes
*
\***************************************************************************/

NTSTATUS
PppIoctlProc(
    IfInfo* ifp,
    INT ctlcode,
    VOID* inbuf,
    UINT inlen,
    VOID* outbuf,
    UINT* outlen
    )
{
    NTSTATUS status;

    switch (ctlcode) {
    case IFCTL_GET_DNSSERVERS:
        if (!inbuf || !inlen || !outbuf || !outlen || *outlen < sizeof(IPADDR)) {
            status = NETERR_PARAM;
        } else {
            //
            // NOTE: we're using the inbuf to return the default domain name string.
            // outbuf is used to return the default DNS server addresses.
            //
            *((CHAR*) inbuf) = 0;
            *outlen = sizeof(IPADDR);
            *((IPADDR*) outbuf) = gIp.IpAddrDns;
            status = NETERR_OK;
        }
        break;

    default:
        status = NETERR_NOTIMPL;
        break;
    }

    return status;
}

/***************************************************************************\
* InitNet
*
\***************************************************************************/

BOOL InitNet(VOID)
{
    DbgPrint("PPP: setting IP and DNS\n");

    gIp.ifp = (IfInfo*) SysAlloc0(sizeof(IfInfo), 'PPP0');
    if (gIp.ifp == NULL) {
        return FALSE;
    }

    gIp.ifp->refcount = 1;
    gIp.ifp->ifname = "Dialup";
    gIp.ifp->flags = IFFLAG_UP;
    gIp.ifp->Delete = PppDeleteProc;
    gIp.ifp->StartOutput = PppStartOutputProc;
    gIp.ifp->Ioctl = PppIoctlProc;

    IfSetIpAddr(gIp.ifp, gIp.IpAddrLocal, 0);

    return TRUE;
}

