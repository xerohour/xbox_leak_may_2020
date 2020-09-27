/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    iputil.c

Abstract:

    IP helper functions

Revision History:

    05/18/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Maximum number of multicast groups an interface can belong to
//
UINT cfgMaxIfMcastAddrs = 32;


VOID
IfSetIpAddr(
    IfInfo* ifp,
    IPADDR ipaddr,
    IPADDR addrmask
    )

/*++

Routine Description:

    Set / clear the IP address for an interface

Arguments:

    ifp - Points to the interface structure
    ipaddr - Specifies the IP address (0 means to clear the address)
    addrmask - Specifies the subnet mask

Return Value:

    NONE

--*/

{
    KIRQL irql = RaiseToDpc();

    if (ipaddr == 0)
        addrmask = 0;
    else if (addrmask == 0 || !XnetIsValidSubnetMask(addrmask))
        addrmask = XnetGetDefaultSubnetMask(ipaddr);

    ifp->ipaddr = ipaddr;
    ifp->addrmask = addrmask;
    ifp->subnetAddr = (ipaddr & addrmask);
    ifp->subnetBcastAddr = ifp->subnetAddr | ~addrmask;

    if (ipaddr) {
        ifp->flags |= IFFLAG_RUNNING;

        // Add a host route for this interface
        if (!IfLoopback(ifp)) {

            #ifdef DEVKIT

            // BUGBUG: Spew out our IP address in the debugger
            // (even on a free build).
            if (ifp->refcount == 1) {
                DbgPrint("\n*** Xbox IP address: %s /", IPADDRSTR(ipaddr));
                DbgPrint(" %s\n\n", IPADDRSTR(addrmask));
            }

            #endif

            IpAddRTE(ifp->ipaddr,
                   0xffffffff,
                   IPADDR_LOOPBACK,
                   LoopbackIfp,
                   RTEFLAG_HOST,
                   DEFAULT_RTE_METRIC);

            DnsNotifyDefaultServers(ifp);
        }

        // Add a local subnet network route for this interface
        IpAddRTE(ifp->subnetAddr,
               ifp->addrmask,
               ifp->ipaddr,
               ifp,
               RTEFLAG_LOCAL,
               DEFAULT_RTE_METRIC);
    } else {
        // Mark the interface as inactive and clear all
        // route entries that go thru this interface.
        // Leave the broadcast route alone.
        ifp->flags &= ~IFFLAG_RUNNING;
        IpRemoveInterfaceRTE(ifp, FALSE);
    }

    LowerFromDpc(irql);
}


DWORD
IpGetBestAddress(
    IPADDR* addr
    )

/*++

Routine Description:

    Get the best available IP address for the local host

Arguments:

    addr - Returns the best IP address

Return Value:

    Flags indicating how the IP address was obtained

--*/

{
    KIRQL irql = RaiseToDpc();
    IfInfo* ifp;
    DWORD flags = 0;

    *addr = 0;

    LOOP_THRU_INTERFACE_LIST(ifp)
        if (IfRunning(ifp)) {
            *addr = ifp->ipaddr;
            if (IfLoopback(ifp)) {
                flags = XNET_ADDR_LOOPBACK;
            } else if (IfDhcpEnabled(ifp)) {
                flags = DhcpGetActiveAddressType(ifp);
            }
            break;
        }
    END_INTERFACE_LIST_LOOP()

    LowerFromDpc(irql);
    return flags;
}


NTSTATUS
IfGetDefaultDnsServers(
    IfInfo* dnsifp,
    CHAR* domainName,
    UINT namelen,
    IPADDR* serverAddrs,
    UINT* serverCnt
    )

/*++

Routine Description:

    Get the default DNS server information
    associated with the specified interface

Arguments:

    dnsifp - Points to an interface structure
    domainName - Buffer for receiving the default domain name
    namelen - Size of the domain name buffer
    serverAddrs - Buffer for receiving the default server addresses
    serverCnt - Number of default servers
        On entry, it specifies the size of the server address buffer
        On return, it contains the actual number of default servers

Return Value:

    Status code

--*/

{
    KIRQL irql = RaiseToDpc();
    IfInfo* ifp = NULL;
    NTSTATUS status;

    LOOP_THRU_INTERFACE_LIST(ifp)
        if (ifp == dnsifp) break;
    END_INTERFACE_LIST_LOOP()

    if (ifp != dnsifp) {
        status = NETERR_PARAM;
    } else if (IfDhcpEnabled(ifp)) {
        status = DhcpGetDefaultDnsServers(ifp, domainName, namelen, serverAddrs, serverCnt);
    } else {
        UINT count = *serverCnt * sizeof(IPADDR);
        status = ifp->Ioctl(ifp, IFCTL_GET_DNSSERVERS, domainName, namelen, serverAddrs, &count);
        *serverCnt = count / sizeof(IPADDR);
    }

    LowerFromDpc(irql);
    return status;
}


NTSTATUS
IfInitMcastGroup(
    IfInfo* ifp
    )

/*++

Routine Description:

    Initialize the per-interface multicast information

Arguments:

    ifp - Points to the interface structure

Return Value:

    Status code

--*/

{
    // NOTE: if ifp->mcastData is not NULL, then we must be sharing
    // the interface with the debug monitor. Since the debug monitor
    // has done the necessary initialization, we don't need to do it
    // again here.

    if (!ifp->mcastData) {
        UINT size = offsetof(IfMcastData, mcastGroups) +
                    sizeof(IfMcastGroup) * cfgMaxIfMcastAddrs;
        ifp->mcastData = (IfMcastData*) SysAlloc0(size, PTAG_MCAST);
        if (!ifp->mcastData)
            return NETERR_MEMORY;

        // Assume the querier is v1 when we start up
        ifp->mcastData->v1QuerierTimer = IGMP_V1_QUERIER_TIMER;
        ifp->mcastData->hasV1Querier = TRUE;

        // Join the all-hosts group
        IfChangeMcastGroup(ifp, IPADDR_ALLHOSTS, TRUE);
    }

    // Make this interface the default multicast interface
    return IpSetMulticastInterface(ifp);
}


BOOL
IfFindMcastGroup(
    IfInfo* ifp,
    IPADDR addr
    )

/*++

Routine Description:

    Check if an interface belongs to the specified multicast group

Arguments:

    ifp - Points to the interface structure
    addr - Specifies the multicast group address

Return Value:

    TRUE if we belong to the multicast group
    FALSE otherwise

--*/

{
    IfMcastData* ifmcast = ifp->mcastData;

    RUNS_AT_DISPATCH_LEVEL

    if (ifmcast) {
        IfMcastGroup* mcastgrp = ifmcast->mcastGroups;
        UINT count = ifmcast->groupCount;
        while (count--) {
            if (mcastgrp->mcastaddr == addr) return TRUE;
            mcastgrp++;
        }
    }

    return FALSE;
}


NTSTATUS
IfChangeMcastGroup(
    IfInfo* ifp,
    IPADDR mcastaddr,
    BOOL add
    )

/*++

Routine Description:

    Join or leave a multicast on the specified interface

Arguments:

    ifp - Points to the interface structure
    mcastaddr - Specifies the multicast group address
    add - Whether to join or leave the group

Return Value:

    Status code

--*/

{
    IfMcastData* ifmcast;
    IfMcastGroup* mcastgrp;
    UINT count;
    KIRQL irql;
    NTSTATUS status = NETERR_OK;
    BOOL notify;

    // Make sure addr is indeed a multicast address
    // and the interface supports multicast
    if (!IS_MCAST_IPADDR(mcastaddr) || (ifmcast = ifp->mcastData) == NULL)
        return NETERR_PARAM;

    irql = RaiseToDpc();
    mcastgrp = ifmcast->mcastGroups;
    count = ifmcast->groupCount;
    while (count) {
        if (mcastgrp->mcastaddr == mcastaddr) break;
        count--, mcastgrp++;
    }

    notify = FALSE;
    if (add) {
        // Joining a gruop
        if (count) {
            // The specified group already exists;
            // just increment its reference count.
            mcastgrp->refcount++;
        } else if (ifmcast->groupCount == cfgMaxIfMcastAddrs) {
            // Too many groups already
            status = NETERR(WSAEADDRNOTAVAIL);
        } else {
            // Add a new group
            ifmcast->groupCount++;
            mcastgrp->refcount = 1;
            mcastgrp->mcastaddr = mcastaddr;
            mcastgrp->reportTimer = 0;
            mcastgrp->sentLastReport = FALSE;
            notify = TRUE;
        }
    } else {
        // Leaving a group
        if (count == 0) {
            // The specified address is not found
            status = NETERR_PARAM;
        } else {
            // Remove the specified multicast group address
            IgmpSendLeave(ifp, mcastgrp);
            while (--count) {
                *mcastgrp = *(mcastgrp+1);
                mcastgrp++;
            }
            ifmcast->groupCount--;
            notify = TRUE;
        }
    }

    // Notify the NIC and inform the IGMP module
    if (notify) {
        status = ifp->Ioctl(ifp, IFCTL_SET_MCAST_ADDRS, NULL, 0, NULL, NULL);
        if (add) {
            if (NT_SUCCESS(status)) {
                // Send out an IGMP report message
                // and resend another one a short while later
                IgmpSendReport(ifp, mcastgrp);
                mcastgrp->reportTimer = IGMP_JOIN_RETRY_TIMER;
            } else {
                // If we failed to join a group, back out
                ifmcast->groupCount--;
            }
        }
    }

    LowerFromDpc(irql);
    return status;
}

