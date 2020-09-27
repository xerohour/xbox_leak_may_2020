/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    route.c

Abstract:

    Simple IP datagram routing functionality

Revision History:

    05/22/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Route table (actually a list)
//
LIST_ENTRY IpRouteList;
IPADDR IpLastRteAddr;
RTE* IpLastRte;

#define INVALIDATELASTRTE() (IpLastRteAddr = 0, IpLastRte = NULL)

//
// Convenience macros for loop through all route table entries
//
#define LOOP_THRU_ROUTE_LIST(_rte) { \
            RTE* _next; \
            _rte = (RTE*) IpRouteList.Flink; \
            for (; _rte != (RTE*) &IpRouteList; _rte = _next) { \
                _next = (RTE*) _rte->Flink;

#define END_ROUTE_LIST_LOOP \
            } \
        }


NTSTATUS
IpInitRouteTable()

/*++

Routine Description:

    Initialize the routing table

Arguments:

    NONE

Return Value:

    Status code

--*/

{
    INVALIDATELASTRTE();
    InitializeListHead(&IpRouteList);
    return NETERR_OK;
}


RTE*
IpFindRTE(
    IPADDR dstaddr,
    IfInfo* ifp
    )

/*++

Routine Description:

    Find a route table entry for the specified destination address

Arguments:

    dstaddr - Specifies the destination address
    ifp - Optional parameter, when present it indicates the caller
        is only interested in routes thru that particular interface.

Return Value:

    Pointer to the route table entry for the destination
    NULL if no route is found

NOTE:

    This is a very simple implementation. We're assuming that our
    route table will be very small (most likely a handful of entries +
    a default gateway).

--*/

{
    RTE* rte;
    RTE* found;

    RUNS_AT_DISPATCH_LEVEL
    if (dstaddr == 0) return NULL;

    // Fast check
    if (dstaddr == IpLastRteAddr && (!ifp || !IpLastRte || ifp == IpLastRte->ifp))
        return IpLastRte;

    found = NULL;
    LOOP_THRU_ROUTE_LIST(rte)

        if ((dstaddr & rte->addrmask) == rte->dstaddr &&
            (!ifp || ifp == rte->ifp)) {
            // Found a matching entry:
            //  since the route list is sorted by mask length,
            //  we can stop searching right away
            found = rte;
            break;
        }

    END_ROUTE_LIST_LOOP

    // Update the last route lookup information
    IpLastRteAddr = dstaddr;
    return (IpLastRte = found);
}


NTSTATUS
IpAddRTE(
    IPADDR dstaddr,
    IPADDR addrmask,
    IPADDR nexthop,
    IfInfo* ifp,
    INT flags,
    INT metric
    )

/*++

Routine Description:

    Add an entry to the route table

Arguments:

    dstaddr, addrmask - Specifies the destination address and mask
    nexthop - Specifies the next hop gateway address
    ifp - Points to the output interface for the route
    flags - Route flags
    metric - Route metrics

Return Value:

    Status code

--*/

{
    RTE* newrte;
    RTE* rte;
    KIRQL irql;

    // Validate input parameters:
    //  - address mask must be of the form 111...000
    //      be careful about the byte order
    //  - next hop address cannot be a broadcast address

    if (!XnetIsValidSubnetMask(addrmask) ||
        nexthop == 0 ||
        IfBcastAddr(ifp, nexthop) && !(flags & RTEFLAG_BCAST)) {
        WARNING_("Bad route to %s:", IPADDRSTR(dstaddr));
        WARNING_("  mask = %s", IPADDRSTR(addrmask));
        WARNING_("  nexthop = %s", IPADDRSTR(nexthop));
        return NETERR_PARAM;
    }

    // Make sure loopback address never goes outside
    ASSERT(!IS_LOOPBACK_IPADDR(dstaddr) ||
           IfLoopback(ifp) &&
           addrmask == CLASSA_NETMASK &&
           nexthop == IPADDR_LOOPBACK);

    dstaddr &= addrmask;
    irql = RaiseToDpc();

    // Find out if the specified route is already in the table

    LOOP_THRU_ROUTE_LIST(rte)

        if (rte->dstaddr == dstaddr &&
            rte->addrmask == addrmask && 
            (addrmask != 0 || rte->nexthop == nexthop)) {
            // Update the existing route information
            newrte = rte;
            RemoveEntryList(&rte->links)
            goto insert_route;
        }

    END_ROUTE_LIST_LOOP

    // Add a new route table entry
    newrte = XnetAlloc0(sizeof(RTE), PTAG_RTE);
    if (!newrte) {
        LowerFromDpc(irql);
        return NETERR_MEMORY;
    }
    newrte->refcount = 1;

insert_route:

    newrte->flags = flags;
    newrte->metric = metric;
    newrte->dstaddr = dstaddr;
    newrte->addrmask = addrmask;
    newrte->nexthop = nexthop;
    CACHE_IFP_REFERENCE(newrte->ifp, ifp);
    INVALIDATELASTRTE();

    // Insert the new route into the table
    //  sorted by the mask length and route metric

    LOOP_THRU_ROUTE_LIST(rte)

        if (addrmask > rte->addrmask || 
            addrmask == rte->addrmask && metric < rte->metric) {
            break;
        }

    END_ROUTE_LIST_LOOP

    newrte->Flink = (LIST_ENTRY*) rte;
    newrte->Blink = rte->Blink;
    rte->Blink->Flink = (LIST_ENTRY*) newrte;
    rte->Blink = (LIST_ENTRY*) newrte;

    LowerFromDpc(irql);
    return NETERR_OK;
}


PRIVATE VOID
IpDeleteRouteEntry(
    RTE* rte
    )

/*++

Routine Description:

    Delete the specified route entry

Arguments:

    rte - Points to the route entry to be deleted

Return Value:

    NONE

--*/

{
    RUNS_AT_DISPATCH_LEVEL

    INVALIDATELASTRTE();

    RemoveEntryList(&rte->links)
    if (RteDecRef(rte) == 0) {
        // no more reference to this entry, we can delete it
        XnetFree(rte);
    } else {
        // there is still cached reference to this entry,
        // mark it as orphaned and it will be deleted
        // when the last reference goes away.
        rte->flags |= RTEFLAG_ORPHAN;
        rte->ifp = NULL;
    }
}


VOID
IpRemoveInterfaceRTE(
    IfInfo* ifp,
    BOOL deleteBcastRte
    )

/*++

Routine Description:

    Remove all routes going through the specified interface

Arguments:

    ifp - Points to the interface structure
        if NULL, then all routes are removed
    deleteBcastRte - Whether to delete the broadcast routes

Return Value:

    NONE

--*/

{
    RTE* rte;

    RUNS_AT_DISPATCH_LEVEL
    if (IsListNull(&IpRouteList)) return;

    // go through the entire route table and look for
    // entries whose interface matches the specified one

    LOOP_THRU_ROUTE_LIST(rte)

        if (ifp && rte->ifp != ifp) continue;
        if (!deleteBcastRte && IsRteBcast(rte)) continue;
        IpDeleteRouteEntry(rte);

    END_ROUTE_LIST_LOOP
}


VOID
IpRemoveRTE(
    IPADDR dstaddr,
    IPADDR addrmask,
    IPADDR nexthop
    )

/*++

Routine Description:

    Delete a route going to the specified destination
    and through the specified gateway

Arguments:

    dstaddr, addrmask - Specifies the route destination
    nexthop - Specifies the next hop gateway for the route

Return Value:

    NONE

--*/

{
    RTE* rte;
    KIRQL irql = RaiseToDpc();

    LOOP_THRU_ROUTE_LIST(rte)

        if (rte->dstaddr == dstaddr &&
            rte->addrmask == addrmask &&
            rte->nexthop == nexthop) {
            IpDeleteRouteEntry(rte);
            break;
        }

    END_ROUTE_LIST_LOOP

    LowerFromDpc(irql);
}


NTSTATUS
IpSetBroadcastInterface(
    IfInfo* ifp
    )

/*++

Routine Description:

    Designate an interface for transmitting
    outgoing broadcast datagrams

Arguments:

    ifp - Specifies the new broadcast interface

Return Value:

    Status code

--*/

{
    // Remove the route entry for the previous broadcast interface
    IpRemoveRTE(IPADDR_BCAST, 0xffffffff, IPADDR_BCAST);
    if (ifp == NULL) return NETERR_OK;

    // Create an entry for the new broadcast interface
    return IpAddRTE(IPADDR_BCAST,
                  0xffffffff,
                  IPADDR_BCAST,
                  ifp,
                  RTEFLAG_BCAST|RTEFLAG_LOCAL,
                  DEFAULT_RTE_METRIC);
}


NTSTATUS
IpSetMulticastInterface(
    IfInfo* ifp
    )

/*++

Routine Description:

    Designate an interface for transmitting
    outgoing broadcast datagrams

Arguments:

    ifp - Specifies the new broadcast interface

Return Value:

    Status code

--*/

{
    // Remove the route entry for the previous broadcast interface
    IpRemoveRTE(CLASSD_NETID, CLASSD_NETMASK, CLASSD_NETID);
    if (ifp == NULL) return NETERR_OK;

    // Create an entry for the new broadcast interface
    return IpAddRTE(CLASSD_NETID,
                  CLASSD_NETMASK,
                  CLASSD_NETID,
                  ifp,
                  RTEFLAG_MCAST|RTEFLAG_LOCAL,
                  DEFAULT_RTE_METRIC);
}


VOID
IpRedirectHostRoute(
    IfInfo* ifp,
    IPADDR dstaddr,
    IPADDR oldgwaddr,
    IPADDR newgwaddr
    )

/*++

Routine Description:

    Redirect the route to a destination host

Arguments:

    ifp - Specifies the interface that received the redirect message
    dstaddr - Destination host address
    oldgwaddr - Specifies the current gateway to the destination host
    newgwaddr - Specifies the new gateway address

Return Value:

    NONE

--*/

{
    RTE* rte;

    RUNS_AT_DISPATCH_LEVEL

    // The redirected destination must be a unicast address
    if (!XnetIsValidUnicastAddr(dstaddr)) return;

    // Validate the new gateway address:
    //  must be a unicast address on the same subnet
    //  through which the redirect message arrived
    if (!XnetIsValidUnicastAddr(newgwaddr) ||
        ifp->subnetAddr != (newgwaddr & ifp->addrmask))
        return;

    // Find the current route to the destination
    // and see if the sender of the redirect message
    // is the current next-hop gateway
    rte = IpFindRTE(dstaddr, NULL);
    if (!rte || rte->nexthop != oldgwaddr) return;

    // Add a new host route
    IpAddRTE(dstaddr,
           0xffffffff,
           newgwaddr,
           ifp,
           RTEFLAG_HOST,
           DEFAULT_RTE_METRIC);
}


#if DBG

VOID
IpDumpRouteTable()

/*++

Routine Description:

    Dump the content of the route table in the debugger

Arguments:

    NONE

Return Value:

    NONE

--*/

{
    KIRQL irql;
    RTE* rte;

    DbgPrint("\n*** Route table:\n");
    if (IsListNull(&IpRouteList)) return;

    irql = RaiseToDpc();
    LOOP_THRU_ROUTE_LIST(rte)

        DbgPrint("%s/", IPADDRSTR(rte->dstaddr));
        DbgPrint("%s/", IPADDRSTR(rte->addrmask));
        DbgPrint("%s ", IPADDRSTR(rte->nexthop));
        DbgPrint("%02x %d %d %s\n",
                 rte->flags,
                 rte->metric,
                 rte->refcount,
                 rte->ifp->ifname);

    END_ROUTE_LIST_LOOP
    LowerFromDpc(irql);
}

#endif // DBG
