/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    ifenet.h

Abstract:

    Ethernet and ARP implementation related declarations

Revision History:

    05/04/2000 davidx
        Created it.

--*/

#ifndef _IFENET_H
#define _IFENET_H

//
// ARP cache entry
//
typedef struct _ArpCacheEntry {
    IPADDR ipaddr;              // target IP address
    UINT timer;                 // timer (in seconds)
    PacketQueue waitq;          // outgoing packets blocked on this entry
    WORD state;                 // current state of the entry
    BYTE enetaddr[ENETADDRLEN]; // target Ethernet address
} ArpCacheEntry;

//
// Constants for ArpCacheEntry.flags
//
#define ARPST_FREE  0           // entry is free
#define ARPST_BUSY  1           // entry is being resolved
#define ARPST_OK    2           // entry is good
#define ARPST_BAD   3           // target is unreachable

#define IsArpEntryFree(_arpEntry) ((_arpEntry)->state == ARPST_FREE)
#define IsArpEntryBusy(_arpEntry) ((_arpEntry)->state == ARPST_BUSY)
#define IsArpEntryOk(_arpEntry) ((_arpEntry)->state == ARPST_OK)
#define IsArpEntryBad(_arpEntry) ((_arpEntry)->state == ARPST_BAD)

//
// Ethernet interface data structure
//
typedef struct _IfEnet {
    //
    // Information common to all network interfaces
    // !! must be the first field of the structure
    //
    IfInfo;
    DWORD magicCookie;          // magic cookie for debugging purpose

    //
    // NIC specific information
    //
    NicInfo;

    //
    // Used for DHCP/autonet: the tentative address that
    //  we need to check for conflict
    //
    IPADDR checkConflictAddr;

    //
    // ARP cache (the modulo value should be a prime number)
    //
    #define ARP_CACHE_SIZE  64
    #define ARP_HASH_RETRY  5
    #define ARP_HASH_MODULO (ARP_CACHE_SIZE-ARP_HASH_RETRY)

    IPADDR lastArpAddr;
    ArpCacheEntry* lastArpEntry;
    INT arpCacheCount;
    ArpCacheEntry arpCache[ARP_CACHE_SIZE];
} IfEnet;


//
// ARP configuration parameters
//
extern UINT cfgPositiveArpCacheTimeout;
extern UINT cfgNegativeArpCacheTimeout;
extern UINT cfgArpRequestRetries;

//
// Default ARP cache entry timeout (in seconds)
//  = 10 minutes for positive cache
//  = 1 minute for negative cache
//
#define POSITIVE_ARP_CACHE_TIMEOUT (10*60)
#define NEGATIVE_ARP_CACHE_TIMEOUT 60

//
// Number of times we try to retransmit an ARP request
//  (with 1 second delay)
//
#define ARP_REQUEST_RETRIES 2

//
// Resolve an IP address to an ARP cache entry
//  
#define RESOLVE_NONE            0
#define RESOLVE_CREATE_ENTRY    1
#define RESOLVE_SEND_REQUEST    2

ArpCacheEntry* ArpFindCacheEntry(IfEnet* ifp, IPADDR ipaddr, INT resolve);

// ARP timer function
VOID ArpTimerProc(IfEnet* ifp);

// Send out an ARP request or reply packet
NTSTATUS ArpSendPacket(IfEnet*, WORD, IPADDR, IPADDR, const BYTE*);

INLINE NTSTATUS ArpSendRequest(IfEnet* ifp, IPADDR tpa, IPADDR spa) {
    return ArpSendPacket(ifp, ARP_REQUEST, tpa, spa, NULL);
}

INLINE NTSTATUS ArpSendReply(IfEnet* ifp, IPADDR tpa, const BYTE* tha) {
    return ArpSendPacket(ifp, ARP_REPLY, tpa, ifp->ipaddr, tha);
}

// Process an incoming ARP packet
VOID ArpReceivePacket(IfEnet* ifp, Packet* pkt);

// Cleanup ARP related data
VOID ArpCleanup(IfEnet* ifp);

// Start output on the Ethernet interface
VOID EnetStartOutput(IfEnet* ifp);

// Generate an Ethernet multicast address from an IP multicast address
INLINE VOID EnetGenerateMcastAddr(IPADDR ipaddr, BYTE* hwaddr) {
    //
    // Refer to RFC1112 - section 6.4:
    // An IP host group address is mapped to an Ethernet multicast address
    // by placing the low-order 23-bits of the IP address into the low-order
    // 23 bits of the Ethernet multicast address 01-00-5E-00-00-00 (hex).
    //
    hwaddr[0] = 0x01;
    hwaddr[1] = 0x00;
    hwaddr[2] = 0x5e;

    // Note that IPADDR is in network byte order
    hwaddr[3] = (BYTE) ((ipaddr >> 8) & 0x7f);
    hwaddr[4] = (BYTE) (ipaddr >> 16);
    hwaddr[5] = (BYTE) (ipaddr >> 24);
}

//
// NIC functions
//
NTSTATUS NicInitialize(IfEnet* nic);
VOID NicReset(IfEnet* nic, BOOL disconnectIntr);
VOID NicCleanup(IfEnet* nic);
VOID NicTimerProc(IfEnet* nic);
VOID NicTransmitFrame(IfEnet* nic, Packet* pkt);
NTSTATUS NicSetMcastAddrs(IfEnet* nic, const BYTE* addrs, UINT count);
VOID NicWaitForXmitQEmpty(IfEnet* nic);
INLINE BOOL NicIsXmitQFull(IfEnet* nic) { return (nic->cmdqCount >= cfgXmitQLength); }

extern KINTERRUPT NicIntrObject;
INLINE VOID NicDisconnectInterrupt(IfEnet* nic) {
    if (NicIntrObject.Connected) {
        KeDisconnectInterrupt(&NicIntrObject);
        KeRemoveQueueDpc(&nic->dpc);
    }
}

VOID EnetReceiveFrame(IfEnet* ifp, Packet* pkt);
INLINE BOOL EnetIsSendQEmpty(IfEnet* ifp) { return PktQIsEmpty(&ifp->sendq); }

#endif // !_IFENET_H

