/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    interface.h

Abstract:

    Common declarations for all network interfaces

Revision History:

    05/04/2000 davidx
        Created it.

--*/

#ifndef _INTERFACE_H
#define _INTERFACE_H

//
// Common information for all network interfaces
//
typedef VOID (*IfDeleteProc)(IfInfo*);
typedef VOID (*IfStartOutputProc)(IfInfo*);
typedef VOID (*IfTimerProc)(IfInfo*);
typedef NTSTATUS (*IfIoctlProc)(IfInfo*, INT, VOID*, UINT, VOID*, UINT*);
typedef struct _IfMcastData IfMcastData;
typedef struct _DhcpInfo DhcpInfo;

typedef struct _IfInfo {
    //
    // Reference count
    //
    LONG refcount;

    //
    // Interface name
    //
    const CHAR* ifname;

    //
    // Interface functions:
    //  deletion
    //  start output
    //  timer
    //  ioctl
    //
    IfDeleteProc Delete;
    IfStartOutputProc StartOutput;
    IfTimerProc Timer;
    IfIoctlProc Ioctl;

    //
    // Output packet queue
    //
    PacketQueue sendq;

    //
    // Flags:
    //  low-order word is common to all interface types
    //  high-order word is interface type specific
    //
    INT flags;

    #define IFFLAG_UP               0x0001  // interface is initialized
    #define IFFLAG_RUNNING          0x0002  //  ... and has a valid IP address
    #define IFFLAG_DHCP_ENABLED     0x0100  // interface is enabled for DHCP
    #define IFFLAG_BCAST            0x0200  // ... supports broadcast
    #define IFFLAG_MCAST            0x0400  // ... supports multicast
    #define IFFLAG_CONNECTED_BOOT   0x0800  // ... is connected to at boot time

    //
    // Hardware address
    //  maximum hardware address length is 16 bytes
    //
    #define MAXHWADDRLEN    16
    #define IFTYPE_LOOPBACK 0
    #define IFTYPE_ETHERNET 1
    #define IFTYPE_DIALUP   2

    WORD iftype;
    WORD hwaddrlen;
    BYTE hwaddr[MAXHWADDRLEN];

    //
    // IP address and subnet mask
    //
    IPADDR ipaddr, addrmask;
    IPADDR subnetAddr, subnetBcastAddr;

    //
    // DHCP related data
    //
    DhcpInfo* dhcpInfo;

    //
    // Multicast groups that we belong to.
    // NOTE: This points to an array of IfMcast structures.
    // The last structure must have its mcastaddr field = 0.
    //
    IfMcastData* mcastData;

    //
    // Misc. configuration parameters
    //
    UINT mtu;               // MTU (maximum transfer unit)
    UINT framehdrlen;       // link layer frame header length
} IfInfo;

//
// Maximum link-layer header size (for all interfaces)
//  should be a multiple of 4
//
#define MAXLINKHDRLEN 16

//
// We only support a maximum of 3 interfaces
//
#define IFINDEX_LOOPBACK    0
#define IFINDEX_LAN         1
#define IFINDEX_DIALUP      2
#define IFINDEX_MAX         3

extern IfInfo* Interfaces[IFINDEX_MAX];

#define LoopbackIfp Interfaces[IFINDEX_LOOPBACK]
#define LanIfp      Interfaces[IFINDEX_LAN]
#define DialupIfp   Interfaces[IFINDEX_DIALUP]

//
// Loop through all interfaces - notice that we count
// down from the last interface to the first interface.
//
#define LOOP_THRU_INTERFACE_LIST(_ifp) { \
            INT _ifindex = IFINDEX_MAX; \
            while (_ifindex-- > 0) { \
                if ((_ifp = Interfaces[_ifindex]) == NULL) continue;

#define END_INTERFACE_LIST_LOOP() \
            } \
        }

// Find the interface with the specified IP address
INLINE IfInfo* IfFindInterface(IPADDR ipaddr) {
    IfInfo* ifp;

    RUNS_AT_DISPATCH_LEVEL

    LOOP_THRU_INTERFACE_LIST(ifp)
        if (ifp->ipaddr == ipaddr) return ifp;
    END_INTERFACE_LIST_LOOP()
    return NULL;
}

// Cache a reference to an interface object
#define CACHE_IFP_REFERENCE(_var, _ifp) ((_var) = (_ifp))

//
// Convenience macros for checking interface flags
//
#define IfUp(_ifp) ((_ifp)->flags & IFFLAG_UP)
#define IfRunning(_ifp) ((_ifp)->flags & IFFLAG_RUNNING)
#define IfDhcpEnabled(_ifp) ((_ifp)->flags & IFFLAG_DHCP_ENABLED)
#define IfBcastEnabled(_ifp) ((_ifp)->flags & IFFLAG_BCAST)
#define IfMcastEnabled(_ifp) ((_ifp)->flags & IFFLAG_MCAST)
#define IfLoopback(_ifp) ((_ifp)->iftype == IFTYPE_LOOPBACK)

//
// Check if the specified address is a broadcast address
// for a particular interface
//
#define IfBcastAddr(_ifp, _addr) \
        (IS_BCAST_IPADDR(_addr) || (_addr) == (_ifp)->subnetBcastAddr)

//
// Append a packet to the end of an interface's output queue.
//  NOTE: we assume we're at DISPATCH_LEVEL.
//
#define IfEnqueuePacket(_ifp, _pkt) \
        PktQInsertTail(&_ifp->sendq, _pkt)

#define IfDequeuePacket(_ifp) \
        PktQRemoveHead(&_ifp->sendq)

//
// Interface I/O control code
//
#define IFCTL_SET_MCAST_ADDRS       1
#define IFCTL_CHECK_ADDR_CONFLICT   2
#define IFCTL_GET_DNSSERVERS        3

//
// Multicast group membership information for each network interface
//
typedef struct _IfMcastGroup {
    LONG refcount;          // reference count for this mcast group
    IPADDR mcastaddr;       // mcast group address
    UINT reportTimer;       // when to send a report for this group
    BOOL sentLastReport;    // we were the last host to send report for this group
} IfMcastGroup;

typedef struct _IfMcastData {
    BOOL hasV1Querier;      // whether there is IGMPv1 querier
    UINT v1QuerierTimer;    // last time we heard IGMPv1 query (in seconds)

    // Variable number of multicast groups
    UINT groupCount;
    IfMcastGroup mcastGroups[1];
} IfMcastData;

// Join and leave multicast groups on an interface
NTSTATUS IfChangeMcastGroup(IfInfo* ifp, IPADDR addr, BOOL add);

#endif // !_INTERFACE_H

