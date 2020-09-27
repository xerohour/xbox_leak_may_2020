/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    igmp.c

Abstract:

    Handle IGMP protocol version 2

Revision History:

    05/31/2000 davidx
        Created it.

--*/

#include "precomp.h"


VOID
IgmpSendMessage(
    IfInfo* ifp,
    IPADDR dstaddr,
    BYTE type,
    IPADDR groupaddr
    )

/*++

Routine Description:

    Send out an IGMP report or leave message

Arguments:

    ifp - Points to the interface structure
    dstaddr - Specifies the message's destination address
    type - Message type
    groupaddr - Address of the multicast group in question

Return Value:

    NONE

--*/

{
    Packet* pkt;
    IgmpMessage* igmpmsg;
    IpHeader* iphdr;

    pkt = XnetAllocIpPacket(0, IGMPHDRLEN);
    if (!pkt) return;

    igmpmsg = GETPKTDATA(pkt, IgmpMessage);
    igmpmsg->type = type;
    igmpmsg->maxresptime = 0;
    igmpmsg->groupaddr = groupaddr;
    COMPUTE_CHECKSUM(igmpmsg->checksum, igmpmsg, IGMPHDRLEN);

    iphdr = GETPKTIPHDR(pkt);
    pkt->data -= IPHDRLEN;
    pkt->datalen += IPHDRLEN;
    pkt->nexthop = dstaddr;

    FILL_IPHEADER(
        iphdr,
        IPHDRLEN,
        0,
        pkt->datalen,
        0,
        1,
        IPPROTOCOL_IGMP,
        ifp->ipaddr,
        dstaddr);

    // Queue up the packet for transmission
    IfEnqueuePacket(ifp, pkt);
    ifp->StartOutput(ifp);
}


PRIVATE VOID
IgmpSetQueryReportTimer(
    IfInfo* ifp,
    IfMcastGroup* mcastgrp,
    UINT maxresptime
    )

/*++

Routine Description:

    Set the query report timer on a multicast group we belong to.

Arguments:

    ifp - Points to the interface structure
    mcastgrp - Points to the multicast group in question
    maxresptime - Specifies the maximum response time in 1/10 of a second

Return Value:

    NONE

--*/

{
    // If existing timer value is less than the new max response time,
    // then we just leave the existing timer alone.
    if (mcastgrp->reportTimer && mcastgrp->reportTimer*10 <= maxresptime)
        return;

    // Randomize the response timer
    maxresptime = XnetRandScaled(maxresptime) / 10;

    if ((mcastgrp->reportTimer = maxresptime) == 0) {
        IgmpSendReport(ifp, mcastgrp);
    }
}


PRIVATE VOID
IgmpProcessMessage(
    IfInfo* ifp,
    IgmpMessage* igmpmsg,
    IPADDR dstaddr
    )

/*++

Routine Description:

    Process a received IGMP message

Arguments:

    ifp - Points to the interface structure
    igmpmsg - Points to the received IGMP message
    dstaddr - Destination group the message is addressed to

Return Value:

    NONE

Note:
    
    This function will only be called when the interface
    belongs to the message's destination address group.

--*/

#define LOCATE_MCASTGROUP() \
        while (count--) { \
            if (mcastgrp->mcastaddr == dstaddr) break; \
            mcastgrp++; \
        }

{
    IfMcastData* ifmcast = ifp->mcastData;
    IfMcastGroup* mcastgrp = ifmcast->mcastGroups;
    UINT count = ifmcast->groupCount;
    IPADDR grpaddr = igmpmsg->groupaddr;

    switch (igmpmsg->type) {
    case IGMPTYPE_MEMBER_QUERY:
        if (grpaddr == 0) {
            UINT maxresptime;

            // General query must be directed to all-hosts address
            if (dstaddr != IPADDR_ALLHOSTS) return;

            if (igmpmsg->maxresptime == 0) {
                ifmcast->hasV1Querier = TRUE;
                ifmcast->v1QuerierTimer = IGMP_V1_QUERIER_TIMER;
                maxresptime = IGMP_V1_QUERY_RESPONSE*10;
            } else
                maxresptime = igmpmsg->maxresptime;

            while (count--) {
                IgmpSetQueryReportTimer(ifp, mcastgrp, maxresptime);
                mcastgrp++;
            }
        } else {
            // Group-specific query must directed to that group address
            if (dstaddr != grpaddr) return;

            LOCATE_MCASTGROUP();
            IgmpSetQueryReportTimer(ifp, mcastgrp, igmpmsg->maxresptime);
        }
        break;

    case IGMPTYPE_MEMBER_REPORT_1:
    case IGMPTYPE_MEMBER_REPORT_2:
        // Membership report must be sent to the specific group
        if (dstaddr != grpaddr) return;

        LOCATE_MCASTGROUP();
        mcastgrp->reportTimer = 0;
        mcastgrp->sentLastReport = FALSE;
        break;
    }
}


VOID
IgmpReceivePacket(
    Packet* pkt
    )

/*++

Routine Description:

    Receive  an IGMP packet

Arguments:

    pkt - Points to the received IGMP message

Return Value:

    NONE

--*/

{
    RUNS_AT_DISPATCH_LEVEL

    // Verify packet length and checksum
    if (pkt->datalen < IGMPHDRLEN ||
        tcpipxsum(0, pkt->data, pkt->datalen) != 0xffff) {
        XnetCompletePacket(pkt, NETERR_DISCARDED);
        return;
    }

    // We're only interested in packets addressed
    // to the all-host multicast group here.
    if (pkt->recvifp->mcastData) {
        IgmpProcessMessage(
            pkt->recvifp,
            GETPKTDATA(pkt, IgmpMessage),
            GETPKTIPHDR(pkt)->dstaddr);
    }

    // Pass the packet upstream for further processing
    RawReceivePacket(pkt);
}


VOID
IgmpTimerProc(
    IfInfo* ifp
    )

/*++

Routine Description:

    IGMP timer procedure

Arguments:

    ifp - Points to the interface structure

Return Value:

    NONE

--*/

{
    IfMcastData* ifmcast = ifp->mcastData;
    IfMcastGroup* mcastgrp;
    UINT count;

    ASSERT(ifmcast != NULL);

    if (ifmcast->v1QuerierTimer && --(ifmcast->v1QuerierTimer) == 0)
        ifmcast->hasV1Querier = FALSE;

    mcastgrp = ifmcast->mcastGroups;
    count = ifmcast->groupCount;
    while (count--) {
        if (mcastgrp->reportTimer && --(mcastgrp->reportTimer) == 0) {
            IgmpSendReport(ifp, mcastgrp);
        }
        mcastgrp++;
    }
}

