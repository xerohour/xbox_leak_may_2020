/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    arp.c

Abstract:

    Handles the ARP protocol

Revision History:

    05/04/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Global ARP configuration info:
//  ARP cache entry timeout value (in seconds)
//  # of ARP cache entries per-interface
//  # of retries for ARP requests
//
UINT cfgPositiveArpCacheTimeout = POSITIVE_ARP_CACHE_TIMEOUT;
UINT cfgNegativeArpCacheTimeout = NEGATIVE_ARP_CACHE_TIMEOUT;
UINT cfgArpRequestRetries = ARP_REQUEST_RETRIES;


NTSTATUS
ArpSendPacket(
    IfEnet* ifp,
    WORD op,
    IPADDR tpa,
    IPADDR spa,
    const BYTE* tha
    )

/*++

Routine Description:

    Send out an ARP packet

Arguments:

    ifp - Points to the interface structure
    op - opcode: ARP_REQUEST or ARP_REPLY
    tpa - Specifies the target IP address
    spa - Specifies the source IP address
    tha - Sppecifies the target hardware address
        NULL when opcode is ARP_REQUEST

Return Value:

    Status code

--*/

{
    Packet* pkt;
    EnetFrameHeader* enethdr;
    ArpPacket* arppkt;
    UINT size = ENETHDRLEN + ARPPKTLEN;

    // Allocate memory for the packet
    pkt = XnetAllocPacket(size, PKTFLAG_NETPOOL);
    if (!pkt) return NETERR_MEMORY;
    pkt->datalen = size;

    // Fill out Ethernet header
    //  assume pkt->data is DWORD-aligned initially
    enethdr = GETPKTDATA(pkt, EnetFrameHeader);
    CopyMem(enethdr->dstaddr, tha ? tha : EnetBroadcastAddr, ENETADDRLEN);
    CopyMem(enethdr->srcaddr, ifp->hwaddr, ENETADDRLEN);
    enethdr->etherType = HTONS(ENETTYPE_ARP);
    
    // Fill out the ARP request packet
    arppkt = (ArpPacket*) (enethdr+1);
    arppkt->hrd = HTONS(ARP_HWTYPE_ENET);
    arppkt->pro = HTONS(ENETTYPE_IP);
    arppkt->hln = ENETADDRLEN;
    arppkt->pln = IPADDRLEN;
    arppkt->op = HTONS(op);
    CopyMem(arppkt->sha, ifp->hwaddr, ENETADDRLEN);
    arppkt->spa = spa;
    arppkt->tpa = tpa;
    if (tha) {
        CopyMem(arppkt->tha, tha, ENETADDRLEN);
    } else {
        ZeroMem(arppkt->tha, ENETADDRLEN);
    }

    NicTransmitFrame(ifp, pkt);
    return NETERR_OK;
}


ArpCacheEntry*
ArpFindCacheEntry(
    IfEnet* ifp,
    IPADDR ipaddr,
    INT resolve
    )

/*++

Routine Description:

    Resolve the specified target IP address to an ARP cache entry.
    If no existing entry is found, then:
    - if resolve is RESOLVE_NONE, we'll simple return NULL;
    - if resolve is RESOLVE_CREATE_ENTRY, we'll try to make a new entry
      for the specified address and return it to the caller;
    - if resolve is RESOLVE_SEND_REQUEST, we'll create the entry
      as well as send out a ARP request packet.

Arguments:

    ifp - Points to the interface structure
    ipaddr - Specifies the target address
    resolve - What to do if no existing entry is found

Return Value:

    ARP cache entry for the specified IP address
    NULL if there is an error

--*/

// Not a terribly fancy hash function
// but it should serve our purpose.
#define ARPHASH(_addr) ((_addr) % ARP_HASH_MODULO)

{
    ArpCacheEntry* arpEntry;
    ArpCacheEntry* retryEnd;
    ArpCacheEntry* hashEntry;
    UINT hash;

    RUNS_AT_DISPATCH_LEVEL
    ASSERT(XnetIsValidUnicastAddr(ipaddr));

    // Fast check
    if (ipaddr == ifp->lastArpAddr)
        return ifp->lastArpEntry;

    // Get the hash bucket for the specified address
    hash = ARPHASH(ipaddr);
    arpEntry = &ifp->arpCache[hash];

    // Found the target address in the cache via a direct hash hit
    if (arpEntry->ipaddr == ipaddr) {
found:
        ifp->lastArpAddr = ipaddr;
        return (ifp->lastArpEntry = arpEntry);
    }

    // No direct hash hit, try linear search
    hashEntry = arpEntry;
    retryEnd = arpEntry + ARP_HASH_RETRY;
    while (++arpEntry < retryEnd) {
        if (arpEntry->ipaddr == ipaddr) goto found;
    }

    if (resolve == RESOLVE_NONE)
        return NULL;

    //
    // The target IP address is not in the cache:
    //  send out an ARP request if specified;
    //  and make a new cache entry for the target
    //
    
    // Check to see if the hack bucket is free
    arpEntry = hashEntry;
    if (!IsArpEntryFree(arpEntry)) {
        while (++arpEntry < retryEnd) {
            if (IsArpEntryFree(arpEntry)) break;
        }
        
        // Couldn't find a free entry
        //  fall back and try to find a non-busy entry
        if (arpEntry == retryEnd) {
            arpEntry = hashEntry;
            while (++arpEntry < retryEnd) {
                if (!IsArpEntryBusy(arpEntry)) break;
            }

            if (arpEntry == retryEnd) {
                // Too bad: couldn't find either a free or non-busy entry
                //  emit a warning and give up
                WARNING_("ARP cache collision error");
                return NULL;
            }

            // This entry is currently used for another address:
            // we'll just bump it off here.
            VERBOSE_("Bumped ARP entry for %s", IPADDRSTR(arpEntry->ipaddr));
            ifp->arpCacheCount--;
        }
    }

    TRACE_("Add ARP entry for %s", IPADDRSTR(ipaddr));
    arpEntry->ipaddr = ipaddr;
    ASSERT(PktQIsEmpty(&arpEntry->waitq));

    if (resolve == RESOLVE_SEND_REQUEST) {
        arpEntry->state = ARPST_BUSY;
        arpEntry->timer = cfgArpRequestRetries;
        ArpSendRequest(ifp, ipaddr, ifp->ipaddr);
    } else {
        // NOTE: we mark the entry free here
        // It'll be immediately marked as OK in ArpReceivePacket
        // after this function returns.
        arpEntry->state = ARPST_FREE;
    }

    ifp->arpCacheCount++;
    goto found;
}


VOID
ArpTimerProc(
    IfEnet* ifp
    )

/*++

Routine Description:

    ARP timer routine

Arguments:

    ifp - Points to the interface structure

Return Value:

    NONE

--*/

{
    ArpCacheEntry* arpEntry;
    INT index;

    // Quick check: do nothing if all entries are free
    if (ifp->arpCacheCount == 0) return;
    ASSERT(ifp->arpCacheCount > 0);

    // Loop through all cache entries
    for (index=0; index < ARP_CACHE_SIZE; index++) {
        arpEntry = &ifp->arpCache[index];
        if (arpEntry->state == ARPST_FREE) continue;

        switch (arpEntry->state) {
        case ARPST_BUSY:
            //
            // The entry is currently being resolved
            //
            if (--arpEntry->timer == 0) {
                //
                // And the entry has expired:
                //  mark the entry as bad and complete any waiting packet
                //  with error status
                //
                arpEntry->state = ARPST_BAD;
                arpEntry->timer = cfgNegativeArpCacheTimeout;

                while (!PktQIsEmpty(&arpEntry->waitq)) {
                    Packet* pkt = PktQRemoveHead(&arpEntry->waitq);
                    COMPLETEPACKET(pkt, NETERR_UNREACHABLE);
                }
            } else {
                //
                // Timeout while waiting for ARP reply:
                //  retransmit the ARP request frame
                //
                ArpSendRequest(ifp, arpEntry->ipaddr, ifp->ipaddr);
            }
            break;
        
        case ARPST_OK:
        case ARPST_BAD:
            if (--arpEntry->timer == 0) {
                //
                // If the entry has expired, remove it from the cache
                //
                TRACE_("ARP entry for %s timed out", IPADDRSTR(arpEntry->ipaddr));

                // Reset the last ARP lookup info if applicable.
                if (arpEntry == ifp->lastArpEntry) {
                    ifp->lastArpAddr = 0;
                    ifp->lastArpEntry = NULL;
                }

                ZeroMem(arpEntry, sizeof(*arpEntry));
                ifp->arpCacheCount--;
            }
            break;
        }
    }
}


VOID
ArpReceivePacket(
    IfEnet* ifp,
    Packet* pkt
    )

/*++

Routine Description:

    Process an incoming ARP packet

Arguments:

    ifp - Points to the interface structure
    pkt - The received ARP packet

Return Value:

    NONE

--*/

{
    ArpPacket* arppkt = GETPKTDATA(pkt, ArpPacket);
    WORD op;
    IPADDR spa, tpa;
    ArpCacheEntry* arpEntry;
    INT resolve;
    BYTE sha[ENETADDRLEN];

    ASSERT(pkt->datalen >= ARPPKTLEN);

    op = NTOHS(arppkt->op);
    spa = arppkt->spa;
    tpa = arppkt->tpa;

    // Simple sanity check
    if (arppkt->hrd != HTONS(ARP_HWTYPE_ENET) &&
        arppkt->hrd != HTONS(ARP_HWTYPE_802) ||
        arppkt->pro != HTONS(ENETTYPE_IP) ||
        arppkt->hln != ENETADDRLEN ||
        arppkt->pln != IPADDRLEN ||
        op != ARP_REQUEST && op != ARP_REPLY ||
        !XnetIsValidUnicastAddr(spa) ||
        !XnetIsValidUnicastAddr(tpa)) {
        goto exit;
    }

    // Check to see if we have an existing entry for the sender
    // in our ARP cache. If we're the target and there is no
    // existing entry, then we'll create a new entry.
    // This assumes that communication will likely be bidirectional.

    if (spa == ifp->ipaddr || spa == ifp->checkConflictAddr) {
        if (IfDhcpEnabled(ifp)) {
            ifp->checkConflictAddr = 0;
            DhcpNotifyAddressConflict((IfInfo*) ifp);
        }

        // In case the other case is checking for address conflict,
        // send out a reply to indicate we already have the address.
        if (tpa == ifp->ipaddr && op == ARP_REQUEST) {
            ArpSendReply(ifp, spa, arppkt->sha);
        }
        goto exit;
    } else if (!IfRunning(ifp)) {
        goto exit;
    }

    CopyMem(sha, arppkt->sha, ENETADDRLEN);
    resolve = (tpa == ifp->ipaddr) ? RESOLVE_CREATE_ENTRY : RESOLVE_NONE;
    
    // We should return the incoming packet structure
    // back to the Enet driver as soon as possible.
    COMPLETEPACKET(pkt, NETERR_OK);
    arpEntry = ArpFindCacheEntry(ifp, spa, resolve);

    if (arpEntry) {
        arpEntry->state = ARPST_OK;
        arpEntry->timer = cfgPositiveArpCacheTimeout;
        CopyMem(arpEntry->enetaddr, sha, ENETADDRLEN);

        // If there were packets waiting on this entry,
        // now that we have the address resolve, it's
        // time to start the transmission.
        //
        // NOTE: We insert the waiting packets at the head of
        // the interface's send queue. We're not calling
        // EnetTransmitPacket directly because we want to
        // limit the depth of the command queue (which is
        // done inside EnetStartOutput).

        if (!PktQIsEmpty(&arpEntry->waitq)) {
            PktQJoinBefore(&ifp->sendq, &arpEntry->waitq);
            PktQInit(&arpEntry->waitq);
            EnetStartOutput(ifp);
        }
    }
    
    // If we're the target and the packet is an ARP request,
    // then send out an ARP reply. Notice that we allocate a
    // new Packet here instead reusing the incoming packet.

    if (tpa == ifp->ipaddr && op == ARP_REQUEST) {
        ArpSendReply(ifp, spa, sha);
    }
    return;

exit:
    // Return the packet to the caller
    COMPLETEPACKET(pkt, NETERR_DISCARDED);
}


VOID
ArpCleanup(
    IfEnet* ifp
    )

/*++

Routine Description:

    Clean up ARP related data

Arguments:

    ifp - Points to the interface structure

Return Value:

    NONE

--*/

{
    ArpCacheEntry* arpEntry;
    Packet* pkt;
    INT index;

    // Loop thru all cache entries
    for (index=0; index < ARP_CACHE_SIZE; index++) {
        arpEntry = &ifp->arpCache[index];
        arpEntry->state = ARPST_BAD;

        // Complete any packets waiting on the entry
        while (!PktQIsEmpty(&arpEntry->waitq)) {
            pkt = PktQRemoveHead(&arpEntry->waitq);
            COMPLETEPACKET(pkt, NETERR_CANCELLED);
        }
    }

    ifp->lastArpAddr = 0;
    ifp->lastArpEntry = NULL;
    ifp->arpCacheCount = 0;
    ZeroMem(&ifp->arpCache, sizeof(ifp->arpCache));
}

