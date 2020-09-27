/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    route.h

Abstract:

    Routing related declarations

Revision History:

    05/23/2000 davidx
        Created it.

--*/

#ifndef _ROUTE_H
#define _ROUTE_H

//
// Routing table entry
//
typedef struct _RouteTableEntry {
    union {                 // double-link pointers - must be the first field!
        LIST_ENTRY links;
        LIST_ENTRY;
    };
    LONG refcount;          // reference count on this entry
    INT flags;              // route flags
    INT metric;             // route metrics: smaller value = higher priority
    IPADDR dstaddr;         // destination address
    IPADDR addrmask;        // address mask
    IPADDR nexthop;         // next hop gateway address
    IfInfo* ifp;            // next hop interface
};

// Route table entry flags
#define RTEFLAG_ORPHAN      0x0001  // the route entry is orphaned
#define RTEFLAG_HOST        0x0002  // host route
#define RTEFLAG_DEFAULT     0x0004  // default gateway route
#define RTEFLAG_LOCAL       0x0008  // local route
#define RTEFLAG_BCAST       0x0010  // broadcast route
#define RTEFLAG_MCAST       0x0020  // multicast route

#define IsRteLocal(_rte)    ((_rte)->flags & RTEFLAG_LOCAL)
#define IsRteBcast(_rte)    ((_rte)->flags & RTEFLAG_BCAST)
#define IsRteOrphaned(_rte) ((_rte)->flags & RTEFLAG_ORPHAN)

// Increment or decrement route table entry reference count
#define RteAddRef(_rte) ((_rte)->refcount += 1)
#define RteDecRef(_rte) ((_rte)->refcount -= 1)

// Initialize and clean up the route table
NTSTATUS IpInitRouteTable();
VOID IpRemoveInterfaceRTE(IfInfo* ifp, BOOL deleteBcastRte);
INLINE VOID IpCleanupRouteTable() {
    IpRemoveInterfaceRTE(NULL, TRUE);
}

// Dump the route table in the debugger
#if DBG
VOID IpDumpRouteTable();
#endif

// Find a route table entry for the specified destination address
RTE* IpFindRTE(IPADDR dstaddr, IfInfo* ifp);

// Add or remove a route table entry
NTSTATUS
IpAddRTE(
    IPADDR dstaddr,
    IPADDR addrmask,
    IPADDR nexthop,
    IfInfo* ifp,
    INT flags,
    INT metric
    );

// Default route metric value
#define DEFAULT_RTE_METRIC 1

// Redirect the route for a destination host
VOID
IpRedirectHostRoute(
    IfInfo* ifp,
    IPADDR dstaddr,
    IPADDR oldgwaddr,
    IPADDR newgwaddr
    );

// Remove route table entries
VOID IpRemoveRTE(IPADDR dstaddr, IPADDR addrmask, IPADDR nexthop);

// Release the reference to a cached route entry
INLINE VOID IpReleaseCachedRTE(RTE* rte) {
    if (InterlockedDecrement(&rte->refcount) == 0) {
        ASSERT(IsRteOrphaned(rte));
        XnetFree(rte);
    }
}

// Add a default gateway route going through an interface
INLINE VOID IpAddDefaultGateway(IPADDR gwaddr, INT metric, IfInfo* ifp) {
    IpAddRTE(0, 0, gwaddr, ifp, RTEFLAG_DEFAULT, metric);
}

// Remove a default route going through an interface
INLINE VOID IpRemoveDefaultGateway(IPADDR gwaddr) {
    IpRemoveRTE(0, 0, gwaddr);
}

// Designate an interface for transmit outgoing broadcast/multicast packets.
NTSTATUS IpSetBroadcastInterface(IfInfo* ifp);
NTSTATUS IpSetMulticastInterface(IfInfo* ifp);

#endif // !_ROUTE_H

