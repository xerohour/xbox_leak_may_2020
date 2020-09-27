/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    loopback.c

Abstract:

    Implement the loopback interface functions

Revision History:

    05/19/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Loopback interface structure
//
typedef struct _IfLoopback {
    // Info common to all interface types
    IfInfo;

    // DPC object
    KDPC dpc;
} IfLoopback;


PRIVATE VOID
LoopbackDpc(
    PKDPC dpc,
    IfLoopback* ifp,
    VOID* arg1,
    VOID* arg2
    )

/*++

Routine Description:

    Loopback interface DPC function:
      remove packets queued up in the sendq
      and pass it upstream for processing.

Arguments:

    dpc - Pointer to the DPC object
    ifp - Pointer to the loopback interface object
    arg1, arg2 - Unused arguments

Return Value:

    NONE

--*/

{
    Packet* pkt;
    IPADDR dstaddr;

    while (!PktQIsEmpty(&ifp->sendq)) {
        pkt = IfDequeuePacket(ifp);

        if (pkt->datalen < IPHDRLEN) {
            XnetCompletePacket(pkt, NETERR_PARAM);
            continue;
        }
        
        dstaddr = GETPKTDATA(pkt, IpHeader)->dstaddr;
        if (IfBcastAddr(ifp, dstaddr) || IS_MCAST_IPADDR(dstaddr))
            pkt->pktflags |= PKTFLAG_MCAST;
        else
            pkt->pktflags &= ~PKTFLAG_MCAST;

        pkt->recvifp = (IfInfo*) ifp;
        IpReceivePacket(pkt);
    }
}


PRIVATE VOID
LoopbackStartOutput(
    IfLoopback* ifp
    )

/*++

Routine Description:

    Send out any queued packets on the loopback interface

Arguments:

    ifp - Points to the interface structure

Return Value:

    NONE

--*/

{
    RUNS_AT_DISPATCH_LEVEL
    if (!PktQIsEmpty(&ifp->sendq)) {
        KeInsertQueueDpc(&ifp->dpc, NULL, NULL);
    }
}


NTSTATUS
IpQueueLoopbackPacket(
    Packet* pkt,
    BOOL copyflag
    )

/*++

Routine Description:

    Queue up a packet on the loopback interface

Arguments:

    pkt - Points to the packet to be queued up
    copyflag - Whether to queue up the packet directly
        or to queue up a copy of the packet

Return Value:

    Status code

--*/

{
    IfLoopback* ifp;

    ASSERT_DISPATCH_LEVEL();

    if (copyflag) {
        Packet* newpkt = XnetCopyPacket(pkt, 0);
        if (!newpkt) return NETERR_MEMORY;
        pkt = newpkt;
    }

    ifp = (IfLoopback*) LoopbackIfp;
    ASSERT(ifp != NULL);
    IfEnqueuePacket(ifp, pkt);
    KeInsertQueueDpc(&ifp->dpc, NULL, NULL);

    return NETERR_OK;
}


PRIVATE NTSTATUS
LoopbackIoctl(
    IfLoopback* ifp,
    INT ctlcode,
    VOID* inbuf,
    UINT inlen,
    VOID* outbuf,
    UINT* outlen
    )

/*++

Routine Description:

    Loopback interface I/O control function

Arguments:

    ifp - Points to the interface structure
    ctlcode - Control code
    inbuf - Points to the input buffer
    inlen - Size of the input buffer
    outbuf - Points to the output buffer
    outlen - On entry, this contains the size of the output buffer
        On return, this is the actually number of output bytes

Return Value:

    Status code

--*/

{
    // We don't support any interface IOCTLs
    return NETERR_NOTIMPL;
}

   
PRIVATE VOID
LoopbackDelete(
    IfLoopback* ifp
    )

/*++

Routine Description:

    Delete the loopback interface

Arguments:

    ifp - Points to the interface structure

Return Value:

    NONE

--*/

{
    RUNS_AT_DISPATCH_LEVEL

    ASSERT(ifp->refcount == 1);
    KeRemoveQueueDpc(&ifp->dpc);

    // Cleanup the send queue
    while (!PktQIsEmpty(&ifp->sendq)) {
        Packet* pkt = PktQRemoveHead(&ifp->sendq);
        XnetCompletePacket(pkt, NETERR_CANCELLED);
    }
    
    SysFree(ifp);
}


NTSTATUS
LoopbackInitialize(
    IfInfo** newifp
    )

/*++

Routine Description:

    Initialize the loopback interface

Arguments:

    newifp - Returns a pointer to the loopback interface

Return Value:

    Status code

--*/

{
    IfLoopback* ifp;

    ifp = (IfLoopback*) SysAlloc0(sizeof(IfLoopback), PTAG_LPBK);
    if (!ifp) return NETERR_MEMORY;

    ifp->refcount = 1;
    ifp->ifname = "Loopback";
    ifp->Delete = (IfDeleteProc) LoopbackDelete;
    ifp->StartOutput = (IfStartOutputProc) LoopbackStartOutput;
    ifp->Ioctl = (IfIoctlProc) LoopbackIoctl;

    // We pretend to have a hardware address
    // which is the same as the IP address.
    ifp->iftype = IFTYPE_LOOPBACK;
    ifp->hwaddrlen = IPADDRLEN;
    *((IPADDR*) ifp->hwaddr) = IPADDR_LOOPBACK;

    ifp->mtu = MAXIPLEN;
    ifp->framehdrlen = 0;

    KeInitializeDpc(&ifp->dpc, (PKDEFERRED_ROUTINE) LoopbackDpc, ifp);
    ifp->flags |= IFFLAG_UP;

    *newifp = (IfInfo*) ifp;
    IfSetIpAddr(*newifp, IPADDR_LOOPBACK, CLASSA_NETMASK);
    return NETERR_OK;
}

