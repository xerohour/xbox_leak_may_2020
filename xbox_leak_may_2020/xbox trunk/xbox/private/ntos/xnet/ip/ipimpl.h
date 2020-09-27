/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    ipimpl.h

Abstract:

    Declarations private to the IP implementation

Revision History:

    05/22/2000 davidx
        Created it.

--*/

#ifndef _IPIMPL_H
#define _IPIMPL_H

// IP datagram reassembly related functions
VOID IpCleanupReassemblyPkts(IfInfo* ifp);
VOID IpReassemblyTimerProc();

// Handle the reception of an ICMP message
VOID IcmpReceivePacket(Packet* pkt);

// Reflect option data in the IP header
IPADDR IpReflectIpOptions(IfInfo* ifp, IpHeader* iphdr, IPADDR dstaddr);

//
// Returns the number of milliseconds since midnight GMT
//
INLINE UINT IpGetMsecsSinceMidnightGMT() {
    LARGE_INTEGER time;
    TIME_FIELDS timeFields;

    KeQuerySystemTime(&time);
    RtlTimeToTimeFields(&time, &timeFields);

    return timeFields.Milliseconds +
           (timeFields.Hour * 3600 +
            timeFields.Minute * 60 +
            timeFields.Second) * 1000;
}

// Find if an interface belongs to a multicast group
BOOL IfFindMcastGroup(IfInfo* ifp, IPADDR addr);
NTSTATUS IfInitMcastGroup(IfInfo* ifp);

// Initialize the loopback interface
NTSTATUS LoopbackInitialize(IfInfo** newifp);

// Process a received IGMP message
VOID IgmpReceivePacket(Packet* pkt);
VOID IgmpTimerProc(IfInfo* ifp);
VOID IgmpSendMessage(IfInfo* ifp, IPADDR dstaddr, BYTE type, IPADDR groupaddr);

INLINE VOID IgmpSendReport(IfInfo* ifp, IfMcastGroup* mcastgrp) {
    if (mcastgrp->mcastaddr != IPADDR_ALLHOSTS) {
        mcastgrp->sentLastReport = TRUE;
        IgmpSendMessage(
            ifp,
            mcastgrp->mcastaddr, 
            (BYTE) (ifp->mcastData->hasV1Querier ?
                    IGMPTYPE_MEMBER_REPORT_1 :
                    IGMPTYPE_MEMBER_REPORT_2),
            mcastgrp->mcastaddr);
    }
}

INLINE VOID IgmpSendLeave(IfInfo* ifp, IfMcastGroup* mcastgrp) {
    if (mcastgrp->mcastaddr != IPADDR_ALLHOSTS &&
        !ifp->mcastData->hasV1Querier &&
        mcastgrp->sentLastReport) {
        IgmpSendMessage(
            ifp,
            IPADDR_ALLROUTERS,
            IGMPTYPE_LEAVE_GROUP,
            mcastgrp->mcastaddr);
    }
}

#endif // !_IPIMPL_H

