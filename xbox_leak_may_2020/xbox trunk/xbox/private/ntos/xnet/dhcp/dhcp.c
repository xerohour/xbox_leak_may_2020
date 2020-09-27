/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    protocol.c

Abstract:

    DHCP client protocol handling code
    as well as Autonet implementation

Revision History:

    04/21/2000 davidx
        Created it.

    05/25/2000 davidx
        Ported over to the new net tree.

--*/

#include "precomp.h"


//
// First 4 bytes of the options field in a DHCP message
// must match the following magic cookie
//
const BYTE DhcpMagicCookie[DHCPCOOKIELEN] = { 99, 130, 83, 99 };

//
// Default address lease time requested:
//  let the server decide
//
UINT cfgDefaultLeaseTime = 0;

//
// Minimum retransmission timeout while in
// RENEWING and REBINDING states: 60 seconds
//
UINT cfgMinRenewTimeout = 60;

//
// Number of seconds to wait after reboot
// before we start sending out DHCPDISCOVER packets
//
// NOTE: RFC2131 recommends that we wait between 1 to 10 seconds.
// But we're only doing 1 seconds here for faster startup time.
//
UINT cfgStartupWaitMin = 0;
UINT cfgStartupWaitMax = 1;

//
// Number of times we'll attempt to resend
// DHCPDISCOVER and DHCPREQUEST packets
//
// NOTE: Non-standard behavior!!!
//  We're capping the timeout between retries to a maximum of 10 seconds.
//
UINT cfgDhcpRetryCount = 3;
UINT cfgDhcpRetryMaxTimeout = 10;

//
// Number of autonet addresses we'll attempt before giving up
//
UINT cfgAutonetAddrRetries = 10;

//
// How frequently to look for DHCP server when in Autonet mode
//
UINT cfgAutonetDhcpCheckInterval = 5*60;

//
// How many ARP requests to send
// when checking for address conflict
//
UINT cfgConflictCheckRetries = 2;

//
// Global transaction ID
//
PRIVATE DWORD DhcpGlobalXid = 0;

//
// Forward function declarations
//
PRIVATE VOID DhcpChangeState(DhcpInfo*, INT);
PRIVATE VOID DhcpSetIfTimerRelative(DhcpInfo*, UINT, UINT);
PRIVATE VOID DhcpSetIfTimerAbsolute(DhcpInfo*, DHCPTIME);
PRIVATE VOID DhcpHandleTimeout(DhcpInfo*);
PRIVATE VOID DhcpResetInterface(DhcpInfo*);
PRIVATE VOID DhcpSelectAutonetAddr(DhcpInfo*);

PRIVATE NTSTATUS DhcpSendDiscover(DhcpInfo*);
PRIVATE NTSTATUS DhcpSendRequest(DhcpInfo*);
PRIVATE NTSTATUS DhcpSendInform(DhcpInfo*);
PRIVATE NTSTATUS DhcpSendDecline(DhcpInfo*, IPADDR, IPADDR);
PRIVATE NTSTATUS DhcpSendRelease(DhcpInfo*);

PRIVATE VOID DhcpUseOptionParams(DhcpInfo*, DhcpOptionParam*);
PRIVATE NTSTATUS DhcpParseOptionParams(DhcpOptionParam*, const BYTE*, UINT, BYTE*);
PRIVATE NTSTATUS DhcpProcessOffer(DhcpInfo*, IPADDR, DhcpOptionParam*);
PRIVATE NTSTATUS DhcpProcessAck(DhcpInfo*, IPADDR, DhcpOptionParam*);
PRIVATE NTSTATUS DhcpProcessNak(DhcpInfo*, IPADDR);
PRIVATE VOID DhcpLoadConfigInfo(DhcpInfo*);

// Check to see if we're forced to use autonet address
//  (without trying to find DHCP servers)
INLINE BOOL DhcpForceAutonet(IPADDR addr, IPADDR mask) {
    return (mask == HTONL(AUTONET_ADDRMASK)) && 
           (addr & mask) == (HTONL(AUTONET_ADDRBASE) & mask);
}



NTSTATUS
DhcpInitialize(
    IfInfo* ifp
    )

/*++

Routine Description:

    Initialize the DHCP related data structure for an interface

Arguments:

    ifp - Points to the interface structure

Return Value:

    Status code

--*/

{
    DhcpInfo* dhcp;

    if ((dhcp = ifp->dhcpInfo) != NULL) {
        dhcp->flags |= FLAG_CREATED_BY_DEBUGGER;
        return NETERR_OK;
    }

    dhcp = (DhcpInfo*) SysAlloc0(sizeof(DhcpInfo), PTAG_DHCP);
    if (!dhcp) return NETERR_MEMORY;

    KeInitializeEvent(&dhcp->addrEvent, NotificationEvent, FALSE);
    dhcp->ifp = ifp;
    dhcp->state = STATE_NONE;
    dhcp->flags = FLAG_SEND_DHCPINFORM;
    dhcp->timer = 0xffffffff;

    ifp->dhcpInfo = dhcp;

    // Load persistent DHCP configuration parameters
    DhcpLoadConfigInfo(dhcp);

    if (dhcp->activeaddr != 0 &&
        !DhcpForceAutonet(dhcp->activeaddr, dhcp->activemask)) {

        //
        // If we're using static address, inform the IP stack
        // BUGBUG - also need to set up the default DNS server?
        //
        IfSetIpAddr(ifp, dhcp->activeaddr, dhcp->activemask);
        DhcpSignalAddrEvent(dhcp);
        DhcpSetDefaultGateways(ifp);
        DhcpChangeState(dhcp, STATE_STATIC_ADDR);

    } else if (!(ifp->flags & IFFLAG_CONNECTED_BOOT) ||
               (cfgXnetConfigFlags & XNET_INITFLAG_FORCE_AUTONET) ||
               dhcp->activeaddr != 0) {

        //
        // Skip the DHCP address discovery process and
        // go straight to autonet mode if:
        //  1. The net cable is disconnected at boot time
        //  2. XnetInitialize was called with force-autonet mode
        //  3. User has configed static IP address = 169.254.x.x
        //
        WARNING_("Forcing autonet...");
        dhcp->activeaddr = dhcp->activemask = 0;
        DhcpChangeState(dhcp, STATE_SELECT_AUTOADDR);

    } else {

        //
        // Obtain address via DHCP or Autonet
        // Be quiet for a while after startup
        //
        DhcpSetIfTimerRelative(dhcp, cfgStartupWaitMin, cfgStartupWaitMax);
    }

    return NETERR_OK;
}


VOID
DhcpCleanup(
    IfInfo* ifp
    )

/*++

Routine Description:

    Clean up DHCP related data for an interface

Arguments:

    ifp - Points to the interface structure

Return Value:

    NONE

--*/

{
    DhcpInfo* dhcp = ifp->dhcpInfo;

    RUNS_AT_DISPATCH_LEVEL
    if (!dhcp) return;
    if (dhcp->flags & FLAG_CREATED_BY_DEBUGGER) {
        dhcp->flags &= ~FLAG_CREATED_BY_DEBUGGER;
        return;
    }
    ifp->dhcpInfo = NULL;

    // Release the DHCP address if necessary
    if (ActiveDhcpAddr(dhcp) && (dhcp->flags & FLAG_RELEASE_ON_REBOOT)) {
        DhcpSendRelease(dhcp);
    }

    // Free the memory for the interface structure
    SysFree(dhcp);
}


VOID
DhcpTimerProc(
    IfInfo* ifp
    )

/*++

Routine Description:

    DHCP timer routine for an interface

Arguments:

    ifp - Points to the interface structure

Return Value:

    NONE

--*/

{
    DhcpInfo* dhcp = ifp->dhcpInfo;

    // Do nothing if the timer hasn't expired
    if (dhcp == NULL ||
        dhcp->timer == 0xffffffff ||
        dhcp->timer-- > 1)
        return;

    DhcpHandleTimeout(dhcp);
}


VOID
DhcpReceivePacket(
    IfInfo* ifp,
    Packet* pkt
    )

/*++

Routine Description:

    Handle a UDP packet received on an interface
    that's destined for the DHCP client port

Arguments:

    ifp - Points to the interface structure
    pkt - Points to the received packet

Return Value:

    NONE

Note:

    The caller retains ownership of the received packet.
    So we don't call CompletePacket on it here.

--*/

{
    DhcpInfo* dhcp = ifp->dhcpInfo;
    DhcpMessage* msg;
    UINT msglen;
    DhcpOptionParam* dhcpParams = NULL;
    BYTE* option;
    BYTE overload = 0;
    NTSTATUS status = NETERR_DISCARDED;

    RUNS_AT_DISPATCH_LEVEL
    if (!dhcp) return;

    msg = GETPKTDATA(pkt, DhcpMessage);
    msglen = pkt->datalen;

    DhcpDumpMessage(msg, msglen);

    //
    // Basic sanity checks: was the packet really meant for us?
    //
    if (msglen < DHCPHDRLEN ||
        msg->op != BOOTREPLY ||
        msg->htype != DhcpMapHwtype(ifp->iftype) ||
        msg->hlen != ifp->hwaddrlen ||
        !EqualMem(msg->chaddr, ifp->hwaddr, ifp->hwaddrlen) ||
        NTOHL(msg->xid) != dhcp->xid) {
        return;
    }

    dhcpParams = (DhcpOptionParam*) XnetAlloc0(sizeof(*dhcpParams), PTAG_DHCP);
    if (!dhcpParams) {
        status = NETERR_MEMORY;
        goto exit;
    }

    //
    // Skip the DHCP magic cookie
    //
    option = msg->options;
    msglen -= DHCPHDRLEN;
    if (msglen < DHCPCOOKIELEN ||
        !EqualMem(option, DhcpMagicCookie, DHCPCOOKIELEN)) {
        goto exit;
    }
    msglen -= DHCPCOOKIELEN;
    option += DHCPCOOKIELEN;

    //
    // Parse the regular options
    //
    status = DhcpParseOptionParams(dhcpParams, option, msglen, &overload);
    if (!NT_SUCCESS(status)) goto exit;

    //
    // Parse overloaded options in the msg->file field
    //
    if (overload & 1) {
        status = DhcpParseOptionParams(dhcpParams, msg->file, sizeof(msg->file), NULL);
        if (!NT_SUCCESS(status)) goto exit;
    }

    //
    // Parse overloaded options in the msg->sname field
    //
    if (overload & 2) {
        status = DhcpParseOptionParams(dhcpParams, msg->sname, sizeof(msg->sname), NULL); 
        if (!NT_SUCCESS(status)) goto exit;
    }

    //
    // The received packet must have a server identifier option
    //
    if (dhcpParams->dhcpServer == 0) {
        status = NETERR_PARAM;
        goto exit;
    }

    //
    // Now that we've parsed the option data,
    // use it appropriately
    //
    status = NETERR_DISCARDED;

    switch (dhcp->state) {
    case STATE_INIT:
        // Expecting DHCPOFFER...
        if (dhcpParams->recvMsgType == DHCPOFFER)
            status = DhcpProcessOffer(dhcp, msg->yiaddr, dhcpParams);
        break;

    case STATE_INIT_REBOOT:
    case STATE_REQUESTING:
    case STATE_RENEWING:
    case STATE_REBINDING:
        // Expecting DHCPACK or DHCPNAK...
        if (dhcpParams->recvMsgType == DHCPACK)
            status = DhcpProcessAck(dhcp, msg->yiaddr, dhcpParams);
        else if (dhcpParams->recvMsgType == DHCPNAK)
            status = DhcpProcessNak(dhcp, dhcpParams->dhcpServer);
        break;

    case STATE_STATIC_ADDR:
        // Expecting DHCPACK...
        if (dhcpParams->recvMsgType == DHCPACK) {
            // Use the option parameters from the server
            DhcpUseOptionParams(dhcp, dhcpParams);
            status = NETERR_OK;
        }
        break;
    }

exit:

    if (!NT_SUCCESS(status) && status != NETERR_DISCARDED) {
        WARNING_("ProcessDhcpPacket failed: 0x%x", status);
    }

    XnetFree(dhcpParams);
}


VOID
DhcpNotifyAddressConflict(
    IfInfo* ifp
    )

/*++

Routine Description:

    This is called when the interface layer detects
    an address conflict (e.g. through ARP)

Arguments:

    ifp - Points to the interface structure

Return Value:

    NONE

--*/

{
    DhcpInfo* dhcp = ifp->dhcpInfo;

    RUNS_AT_DISPATCH_LEVEL
    if (!dhcp) return;

    if (dhcp->state == STATE_SELECT_AUTOADDR) {
        // the last autonet address we chose was no good
        // so we need to find another one
        DhcpSelectAutonetAddr(dhcp);
    } else {
        WARNING_("!!! Address conflict: %s", IPADDRSTR(dhcp->activeaddr));
    }
}


//
// Number of seconds from start of 1601 to start of 2000
//
static const LONGLONG StartOf2000 = 0x2ee7dd480;

PRIVATE DHCPTIME
DhcpTime()

/*++

Routine Description:

    Return the number of seconds ellapsed since 00:00:00
    January 1, 2000, coordinated universal time.

Arguments:

    NONE

Return Value:

    See above.

--*/

{
    LARGE_INTEGER currentTime;

    // Get the current UTC time
    //  = number of 100 nanoseconds since 1/1/1601
    KeQuerySystemTime(&currentTime);

    // Return the number of seconds since the start of 2000
    return (DHCPTIME) (currentTime.QuadPart / 10000000 - StartOf2000);
}


PRIVATE VOID
DhcpSetIfTimerRelative(
    DhcpInfo* dhcp,
    UINT minWait,
    UINT maxWait
    )

/*++

Routine Description:

    Set the timer for an interface:
        randomized and relative to current time

Arguments:

    dhcp - Points to the DHCP structure
    minWait, maxWait - minimum and maximum wait time, in seconds

Return Value:

    NONE

--*/

{
    // Compute the randomized wait time
    minWait += XnetRandScaled(maxWait - minWait);

    if ((dhcp->timer = minWait) == 0) {
        DhcpHandleTimeout(dhcp);
    }
}


PRIVATE VOID
DhcpSetIfTimerAbsolute(
    DhcpInfo* dhcp,
    DHCPTIME timer
    )

/*++

Routine Description:

    Set the timer for an interface: absolute time

Arguments:

    dhcp - Points to the DHCP structure
    timer - When the timer should expire
        number of seconds since the start of 1/1/2000

Return Value:

    NONE

--*/

{
    DHCPTIME now;

    if (timer == DHCPTIME_INFINITE) {
        // Infinite wait
        dhcp->timer = 0xffffffff;
        return;
    }

    now = DhcpTime();
    if (timer > now) {
        dhcp->timer = timer - now;
    } else {
        dhcp->timer = 0;
        DhcpHandleTimeout(dhcp);
    }
}


PRIVATE VOID
DhcpComputeTimeout(
    DhcpInfo* dhcp
    )

/*++

Routine Description:

    Compute the next timeout interval after sending out a packet

Arguments:

    dhcp - Points to the DHCP structure

Return Value:

    Status code

--*/

{
    UINT minWait, maxWait;

    switch (dhcp->state) {
    case STATE_INIT:
        if (ActiveAutonetAddr(dhcp)) {
            //
            // Special case: we're currently using an Autonet address
            // and we're sending out periodic DHCPDISCOVER packets.
            //
            // DhcpSetIfTimerRelative(
            //     dhcp,
            //     cfgAutonetDhcpCheckInterval,
            //     cfgAutonetDhcpCheckInterval);

            // NOTE: Non-standard behavior!!!
            //  We do not send out DHCPDISCOVER messages
            //  while we're using an active AutoNet address.
            DhcpSetIfTimerAbsolute(dhcp, DHCPTIME_INFINITE);
            break;
        }

        // Fall through

    case STATE_REQUESTING:
    case STATE_INIT_REBOOT:
        //
        // timeout = 2**retry x 2 +/- 1, i.e.
        //  4 +/- 1
        //  8 +/- 1
        //  16 +/- 1
        //  32 +/- 1
        //

        minWait = 2 << dhcp->retries;
        if (minWait > cfgDhcpRetryMaxTimeout)
            minWait = cfgDhcpRetryMaxTimeout;

        minWait -= 1;
        maxWait = minWait + 2;
        DhcpSetIfTimerRelative(dhcp, minWait, maxWait);
        break;

    case STATE_BOUND:
        //
        // timeout when T1 expires
        //
        DhcpSetIfTimerAbsolute(dhcp, dhcp->t1time);
        break;

    case STATE_RENEWING:
    case STATE_REBINDING: {
        //
        // calculate retransmission timeout for
        // RENEWING and REBINDING states:
        //  1. half the time from now to T2 (renew) or expiration (rebind)
        //  2. make sure it's at least 60 seconds
        //

        DHCPTIME t1, t2;
        t1 = DhcpTime();
        t2 = (dhcp->state == STATE_RENEWING) ? dhcp->t2time : dhcp->exptime;

        if (t1 < t2) {
            minWait = (t2-t1) / 2;
            if (minWait >= cfgMinRenewTimeout)
                t1 += minWait;
            else {
                t1 += cfgMinRenewTimeout;
                if (t1 > t2) t1 = t2;
            }
        }

        DhcpSetIfTimerAbsolute(dhcp, t1);
        }
        break;

    case STATE_DECLINING:
        //
        // Wait 10 seconds after sending DHCP decline
        //
        DhcpSetIfTimerRelative(dhcp, 10, 10);
        break;
    
    case STATE_SELECT_AUTOADDR:
        //
        // Wait ~2 seconds for address conflict detection
        //
        DhcpSetIfTimerRelative(dhcp, 2, 2);
        break;

    default:
        // Should not happen - shut off the timer just in case
        ASSERT(FALSE);

    case STATE_STATIC_ADDR:
    case STATE_NONE:
        DhcpSetIfTimerAbsolute(dhcp, DHCPTIME_INFINITE);
        break;
    }
}


PRIVATE VOID
DhcpHandleTimeout(
    DhcpInfo* dhcp
    )

/*++

Routine Description:

    Handle a timeout event for an interface

Arguments:

    dhcp - Points to the DHCP structure

Return Value:

    NONE

--*/

{
    switch (dhcp->state) {
    case STATE_NONE:
        //
        // We got here because we just finished the quiet
        // period after startup
        //
        // If we had a valid lease before, start in
        // INIT-REBOOT state; otherwise, start in INIT state
        //
        DhcpChangeState(dhcp, dhcp->dhcpaddr ? STATE_INIT_REBOOT : STATE_INIT);
        break;
    
    case STATE_INIT:
        //
        // We're sending DHCPDISCOVER messages
        //
        if (dhcp->retries >= cfgDhcpRetryCount && !ActiveAutonetAddr(dhcp)) {
            //
            // Too many retries, give up and
            // start Autonet address acquisition process
            //
            VERBOSE_("Couldn't discover DHCP server, trying autonet...");
            DhcpChangeState(dhcp, STATE_SELECT_AUTOADDR);
        } else {
            //
            // Send out another DHCPDISCOVER packet
            //
            VERBOSE_("Retransmit DHCPDISCOVER");
            DhcpSendDiscover(dhcp);
        }
        break;

    case STATE_INIT_REBOOT:
    case STATE_REQUESTING:
        //
        // We're sending DHCPREQUEST
        //
        if (dhcp->retries >= cfgDhcpRetryCount) {
            //
            // Too many retries, go back to INIT state
            //
            VERBOSE_(dhcp->state == STATE_INIT_REBOOT ?
                        "Failed to reuse a prior lease" :
                        "No ack for an offered address");

            DhcpChangeState(dhcp, STATE_INIT);
        } else {
            //
            // Send out another DHCPREQUEST packet
            //
            VERBOSE_("Retransmit DHCPREQUEST");
            DhcpSendRequest(dhcp);
        }
        break;
    
    case STATE_DECLINING:
        //
        // We just finished waiting after sending DHCPDECLINE
        //
        DhcpChangeState(dhcp, STATE_INIT);
        break;
    
    case STATE_BOUND:
        //
        // T1 expired, start the renewing process
        //
        VERBOSE_("Switching to RENEWING state");
        DhcpChangeState(dhcp, STATE_RENEWING);
        break;

    case STATE_RENEWING:
        //
        // We're trying to renew a valid address.
        // If T2 expired, start the rebinding process.
        //
        if (DhcpTime() >= dhcp->t2time) {
            VERBOSE_("Switching to REBINDING state");
            DhcpChangeState(dhcp, STATE_REBINDING);
        } else {
            // Send out another DHCPREQUEST
            VERBOSE_("Retransmit DHCPREQUEST");
            DhcpSendRequest(dhcp);
        }
        break;

    case STATE_REBINDING:
        //
        // We're trying to rebind a valid lease
        //  did our lease expire?
        //
        if (DhcpTime() >= dhcp->exptime) {
            // Inform IP stack to discard the active address
            VERBOSE_("Address lease expired - start over");
            DhcpResetInterface(dhcp);

            // Too bad, go back to INIT state
            DhcpChangeState(dhcp, STATE_INIT);
        } else {
            // Send out another DHCPREQUEST
            VERBOSE_("Retransmit DHCPREQUEST");
            DhcpSendRequest(dhcp);
        }
        break;

    case STATE_SELECT_AUTOADDR:
        if (++dhcp->retries >= cfgConflictCheckRetries) {
            // We've successfully picked an autonet address.
            VERBOSE_("Selected autonet address: %s", IPADDRSTR(dhcp->autonetaddr));

            dhcp->activeaddr = dhcp->autonetaddr;
            dhcp->activemask = HTONL(AUTONET_ADDRMASK);
            dhcp->flags |= FLAG_ACTIVE_AUTONETADDR;
            IfSetIpAddr(dhcp->ifp, dhcp->activeaddr, dhcp->activemask);
            DhcpSignalAddrEvent(dhcp);

            // Switch to INIT state to continue looking
            // for a DHCP server
            dhcp->initRetryCount = 0;
            DhcpChangeState(dhcp, STATE_INIT);
        } else {
            // No response to our previous ARP request.
            // Try again just to be sure.
            DhcpCheckAddressConflict(dhcp->ifp, dhcp->autonetaddr);
            DhcpComputeTimeout(dhcp);
        }
        break;

    default:
        VERBOSE_("Unexpected timeout");
        DhcpSetIfTimerAbsolute(dhcp, DHCPTIME_INFINITE);
        break;
    }
}


PRIVATE VOID
DhcpChangeState(
    DhcpInfo* dhcp,
    INT state
    )

/*++

Routine Description:

    Change the state of an interface

Arguments:

    dhcp - Points to the DHCP structure
    state - Specifies the new state for the interface

Return Value:

    NONE

--*/

{
    INT oldstate = dhcp->state;

    dhcp->state = state;
    dhcp->retries = 0;

    // Assign a new transaction ID for the next outgoing message.

    if (DhcpGlobalXid == 0) DhcpGlobalXid = XnetRand();
    dhcp->xid = DhcpGlobalXid++;

    switch (state) {
    case STATE_STATIC_ADDR:
        // Should we send DHCPINFORM?
        if (dhcp->flags & FLAG_SEND_DHCPINFORM) {
            DhcpSendInform(dhcp);
        }
        break;

    case STATE_INIT_REBOOT:
    case STATE_REQUESTING:
    case STATE_RENEWING:
    case STATE_REBINDING:
        // Send out DHCPREQUEST
        DhcpSendRequest(dhcp);
        break;

    case STATE_INIT:
        if (oldstate == STATE_SELECT_AUTOADDR) {
            // We just selected an autonet address.
            // Continue to look for a DHCP server.
            DhcpComputeTimeout(dhcp);
        } else if (++dhcp->initRetryCount > cfgDhcpRetryCount) {
            // We went through the INIT state too many times
            // without getting a valid address lease. Just give up.
            // We don't try Autonet because in this case there
            // is a DHCP server but somehow we can't work with it.
            WARNING_("Couldn't get a valid DHCP address after many tries");
            DhcpChangeState(dhcp, STATE_NONE);

            // Signal that Xnet initialization was completed abnormally
            DhcpSignalAddrEvent(dhcp);
        } else {
            // Send out DHCPDISCOVER
            DhcpSendDiscover(dhcp);
        }
        break;

    case STATE_BOUND:
        DhcpComputeTimeout(dhcp);
        VERBOSE_("Sleep %d seconds till renewal...", dhcp->t1time - DhcpTime());
        break;

    case STATE_SELECT_AUTOADDR:
        dhcp->initRetryCount = 0;
        DhcpSelectAutonetAddr(dhcp);
        break;

    case STATE_NONE:
        DhcpSetIfTimerAbsolute(dhcp, DHCPTIME_INFINITE);
        break;
    }
}


PRIVATE VOID
DhcpResetInterface(
    DhcpInfo* dhcp
    )

/*++

Routine Description:

    Reset an interface to have no address

Arguments:

    dhcp - Points to the DHCP structure

Return Value:

    Status code

--*/

{
    IfSetIpAddr(dhcp->ifp, 0, 0);
    dhcp->flags &= ~FLAG_ACTIVE_ADDRMASK;
    if (dhcp->state != STATE_STATIC_ADDR)
        dhcp->activeaddr = 0;

    // Since we don't have an active address anymore,
    // we need to notify the DNS module of the change.
    DnsNotifyDefaultServers(dhcp->ifp);
}


PRIVATE NTSTATUS
DhcpSendMessage(
    DhcpInfo* dhcp,
    Packet* pkt,
    BOOL broadcast
    )

/*++

Routine Description:

    Send out a DHCP message on the specified interface

Arguments:

    dhcp - Points to the DHCP structure
    pkt - Points to the message to be sent
        !!! we assume the message buffer is at least 300 bytes
        and unused bytes are zero-filled.
    broadcast - Whether to use broadcast or unicast

Return Value:

    Status code

--*/

{
    IpAddrPair addrpair;
    DhcpMessage* msg;
    UINT msglen;

    addrpair.dstaddr = broadcast ? IPADDR_BCAST : dhcp->dhcpServer;
    addrpair.srcaddr = dhcp->activeaddr;
    addrpair.dstport = DHCP_SERVER_PORT;
    addrpair.srcport = DHCP_CLIENT_PORT;

    msg = GETPKTDATA(pkt, DhcpMessage);
    msglen = pkt->datalen;

    // Debug trace
    DhcpDumpMessage(msg, msglen);

    // Set broadcast flag if necessary
    if (!dhcp->activeaddr || ActiveAutonetAddr(dhcp))
        msg->flags |= HTONS(DHCP_BROADCAST);

    // Call UDP directly to send out the packet
    UdpSendPacketInternal(pkt, &addrpair, dhcp->ifp);

    // Calculate the timeout value
    dhcp->retries++;
    DhcpComputeTimeout(dhcp);

    return NETERR_OK;
}


//
// Append the 'parameter request list' option
//  !!! we assume the data buffer is large enough.
//
PRIVATE BYTE*
DhcpAppendParamReqList(
    BYTE* option
    )
{
    //
    // our default list of option parameters
    //
    static const BYTE defaultParamReqList[] = {
        DHCPOPT_SUBNET_MASK,
        DHCPOPT_ROUTERS,
        DHCPOPT_DNS_SERVERS,
        DHCPOPT_DOMAIN_NAME
    };

    UINT count = sizeof(defaultParamReqList);

    option[0] = DHCPOPT_PARAM_REQUEST_LIST;
    option[1] = (BYTE) count;
    option += 2;

    CopyMem(option, defaultParamReqList, count);
    return option + count;
}

//
// Append an option whose value is a DWORD
//
PRIVATE BYTE*
DhcpAppendDWordOption(
    BYTE* option,
    INT tag,
    DWORD val
    )
{
    option[0] = (BYTE) tag;
    option[1] = sizeof(DWORD);
    option += 2;

    CopyMem(option, &val, sizeof(DWORD));
    return option + sizeof(DWORD);
}


//
// Append the 'address lease time' option
//
PRIVATE BYTE*
DhcpAppendLeaseTimeOption(
    BYTE* option
    )
{
    if (cfgDefaultLeaseTime == 0) return option;

    return DhcpAppendDWordOption(   
                option,
                DHCPOPT_IPADDR_LEASE_TIME,
                HTONL(cfgDefaultLeaseTime));
}


PRIVATE BYTE*
DhcpFillMessageHeader(
    DhcpInfo* dhcp,
    BYTE* buf,
    INT msgtype
    )

/*++

Routine Description:

    Fill in the common header information for
    all outgoing DHCP messages

Arguments:

    dhcp - Points to the DHCP structure
    buf - Points to the message buffer
        !!! must be at least DEFAULT_DHCP_BUFSIZE bytes
    msgtype - DHCP message type

Return Value:

    Points to the first byte after the common options:
        magic cookie
        DHCP message type
        client identifier

--*/

{
    DhcpMessage* msg = (DhcpMessage*) buf;
    BYTE* option = msg->options;
    IfInfo* ifp = dhcp->ifp;
    BYTE hwtype;

    ZeroMem(buf, DEFAULT_DHCP_BUFSIZE);
    msg->op = BOOTREQUEST;
    msg->htype = hwtype = DhcpMapHwtype(ifp->iftype);
    msg->hlen = (BYTE) ifp->hwaddrlen;
    CopyMem(msg->chaddr, ifp->hwaddr, ifp->hwaddrlen);

    //
    // Fill in the transaction ID field
    // NOTE: We reuse the same XID for retransmissions.
    //
    msg->xid = HTONL(dhcp->xid);

    // Number of seconds since we started the address
    // acquisition process.
    msg->secs = (WORD) HTONS(dhcp->secsSinceStart);

    // Start with the magic cookie
    CopyMem(option, DhcpMagicCookie, DHCPCOOKIELEN);
    option += DHCPCOOKIELEN;

    // Append the message type option
    option[0] = DHCPOPT_DHCP_MESSAGE_TYPE;
    option[1] = 1;
    option[2] = (BYTE) msgtype;
    option += 3;

    // Append the client identifier option
    option[0] = DHCPOPT_CLIENTID;
    option[1] = (BYTE) (ifp->hwaddrlen+1);
    option[2] = hwtype;
    option += 3;
    CopyMem(option, ifp->hwaddr, ifp->hwaddrlen);
    option += ifp->hwaddrlen;

    return option;
}


PRIVATE VOID
DhcpComputeSecsSinceStart(
    DhcpInfo* dhcp
    )

/*++

Routine Description:

    Compute the number of seconds since the current
    address acqusition process started.

Arguments:

    dhcp - Points to the DHCP structure

Return Value:

    NONE

--*/

{
    DHCPTIME now = DhcpTime();

    if (dhcp->retries == 0) dhcp->acqtime = now;
    dhcp->secsSinceStart = now - dhcp->acqtime;
}


//
// Common prolog and epilog for SendDhcpXXX functions
//
#define SEND_DHCP_MESSAGE_PROLOG() \
        DhcpMessage* msg; \
        BYTE* buf; \
        BYTE* option; \
        Packet* pkt = XnetAllocIpPacket(UDPHDRLEN, DEFAULT_DHCP_BUFSIZE); \
        if (!pkt) return NETERR_MEMORY; \
        buf = pkt->data; \
        msg = (DhcpMessage*) buf

#define SEND_DHCP_MESSAGE_RETURN(_bcast) \
        pkt->datalen = option - buf; \
        return DhcpSendMessage(dhcp, pkt, _bcast)


PRIVATE NTSTATUS
DhcpSendDiscover(
    DhcpInfo* dhcp
    )

/*++

Routine Description:

    Broadcast a DHCPDISCOVER message

Arguments:

    dhcp - Points to the DHCP structure

Return Value:

    Status code

--*/

{
    SEND_DHCP_MESSAGE_PROLOG();

    ASSERT(dhcp->state == STATE_INIT);

    DhcpComputeSecsSinceStart(dhcp);

    // Fill in common header fields
    option = DhcpFillMessageHeader(dhcp, buf, DHCPDISCOVER);

    // Fill in the parameter request list
    option = DhcpAppendParamReqList(option);

    // Fill in requested ip address and lease time option
    if (dhcp->dhcpaddr)
        option = DhcpAppendDWordOption(option, DHCPOPT_REQUESTED_IPADDR, dhcp->dhcpaddr);

    // Fill in the lease time option
    option = DhcpAppendLeaseTimeOption(option);
    *option++ = DHCPOPT_END;

    // Emit the message
    SEND_DHCP_MESSAGE_RETURN(TRUE);
}


PRIVATE NTSTATUS
DhcpSendRequest(
    DhcpInfo* dhcp
    )

/*++

Routine Description:

    Send (broadcast/unicast) a DHCPREQUEST message

Arguments:

    dhcp - Points to the DHCP structure

Return Value:

    Status code

--*/

{
    INT state = dhcp->state;
    SEND_DHCP_MESSAGE_PROLOG();

    ASSERT(state == STATE_REQUESTING ||
           state == STATE_RENEWING ||
           state == STATE_REBINDING ||
           state == STATE_INIT_REBOOT);

    //
    // If we're in REQUESTING state, then don't update
    // the secsSinceStart field. This is so that the
    // secs in DHCPREQUEST message will be the same as
    // what's in the original DHCPDISCOVER message.
    //
    if (state != STATE_REQUESTING) {
        DhcpComputeSecsSinceStart(dhcp);
    }

    // Fill in common header fields
    option = DhcpFillMessageHeader(dhcp, buf, DHCPREQUEST);

    // Fill in the ciaddr field and the 'requested ip addr' option
    if (state == STATE_RENEWING || state == STATE_REBINDING) {
        msg->ciaddr = dhcp->dhcpaddr;
    } else if (dhcp->dhcpaddr) {
        // state == STATE_INIT_REBOOT || state == STATE_REQUESTING
        option = DhcpAppendDWordOption(option, DHCPOPT_REQUESTED_IPADDR, dhcp->dhcpaddr);
    }

    // Fill in the parameter request list
    option = DhcpAppendParamReqList(option);

    // Fill in the lease time option
    option = DhcpAppendLeaseTimeOption(option);

    // Fill in the server identifier option
    if (state == STATE_REQUESTING)
        option = DhcpAppendDWordOption(option, DHCPOPT_SERVERID, dhcp->dhcpServer);

    *option++ = DHCPOPT_END;

    // Emit the message:
    //  unicast in RENEWING state, broadcast otherwise
    SEND_DHCP_MESSAGE_RETURN(state != STATE_RENEWING);
}


PRIVATE NTSTATUS
DhcpSendInform(
    DhcpInfo* dhcp
    )

/*++

Routine Description:

    Broadcast a DHCPINFORM message

Arguments:

    dhcp - Points to the DHCP structure

Return Value:

    Status code

--*/

{
    SEND_DHCP_MESSAGE_PROLOG();

    ASSERT(dhcp->state == STATE_STATIC_ADDR);

    DhcpComputeSecsSinceStart(dhcp);

    // Fill in common header fields
    option = DhcpFillMessageHeader(dhcp, buf, DHCPINFORM);

    // Fill in ciaddr field
    msg->ciaddr = dhcp->activeaddr;

    // Fill in the parameter request list
    option = DhcpAppendParamReqList(option);
    *option++ = DHCPOPT_END;

    // Emit the message
    SEND_DHCP_MESSAGE_RETURN(TRUE);
}


#ifdef _ENABLE_SEND_DECLINE

PRIVATE NTSTATUS
DhcpSendDecline(
    DhcpInfo* dhcp,
    IPADDR dhcpServer,
    IPADDR ipaddr
    )

/*++

Routine Description:

    Broadcast a DHCPDECLINE message

Arguments:

    dhcp - Points to the DHCP structure
    dhcpServer - The server that offered the IP address
    ipaddr - The IP address to decline

Return Value:

    Status code

--*/

{
    SEND_DHCP_MESSAGE_PROLOG();

    ASSERT(dhcp->state == STATE_INIT_REBOOT ||
           dhcp->state == STATE_REQUESTING);

    DhcpChangeState(dhcp, STATE_DECLINING);
    dhcp->secsSinceStart = 0;

    // Fill in common header fields
    option = DhcpFillMessageHeader(dhcp, buf, DHCPDECLINE);

    // Fill in 'requested ip addr' and 'server identifier' options
    option = DhcpAppendDWordOption(option, DHCPOPT_REQUESTED_IPADDR, ipaddr);
    option = DhcpAppendDWordOption(option, DHCPOPT_SERVERID, dhcpServer);
    *option++ = DHCPOPT_END;

    // Emit the message
    SEND_DHCP_MESSAGE_RETURN(TRUE);
}

#endif // _ENABLE_SEND_DECLINE


PRIVATE NTSTATUS
DhcpSendRelease(
    DhcpInfo* dhcp
    )

/*++

Routine Description:

    Unicast a DHCPRELEASE message to the server

Arguments:

    dhcp - Points to the DHCP structure

Return Value:

    Status code

--*/

{
    SEND_DHCP_MESSAGE_PROLOG();

    ASSERT(dhcp->state == STATE_BOUND ||
           dhcp->state == STATE_RENEWING ||
           dhcp->state == STATE_REBINDING);

    DhcpChangeState(dhcp, STATE_NONE);
    dhcp->secsSinceStart = 0;

    // Fill in common header fields
    option = DhcpFillMessageHeader(dhcp, buf, DHCPRELEASE);

    // Fill in 'server identifier' option
    option = DhcpAppendDWordOption(option, DHCPOPT_SERVERID, dhcp->dhcpServer);
    *option++ = DHCPOPT_END;

    // Fill in ciaddr field
    msg->ciaddr = dhcp->dhcpaddr;

    // Emit the message
    SEND_DHCP_MESSAGE_RETURN(FALSE);
}


PRIVATE BOOL
DhcpValidateOffer(
    IPADDR yiaddr,
    DhcpOptionParam* param
    )

/*++

Routine Description:

    Perform simple sanity check of offered DHCP parameters

Arguments:

    yiaddr - Offered IP address
    param - Other offered parameters

Return Value:

    TRUE if ok, FALSE otherwise

--*/

{
    if (!XnetIsValidUnicastAddr(yiaddr) ||
        !XnetIsValidUnicastAddr(param->dhcpServer) ||
        param->exptime < 8*cfgMinRenewTimeout)
        return FALSE;

    if (param->dhcpmask == 0)
        param->dhcpmask = XnetGetDefaultSubnetMask(yiaddr);

    if (param->t1time == 0 ||
        param->t2time == 0 ||
        param->t1time >= param->t2time ||
        param->t2time - param->t1time < cfgMinRenewTimeout ||
        param->t2time >= param->exptime ||
        param->exptime - param->t2time < cfgMinRenewTimeout) {
        param->t1time = param->exptime / 2;
        param->t2time = param->exptime * 7 / 8;
    }

    return TRUE;
}


PRIVATE NTSTATUS
DhcpProcessOffer(
    DhcpInfo* dhcp,
    IPADDR yiaddr,
    DhcpOptionParam* param
    )

/*++

Routine Description:

    Process a received DHCPOFFER packet

Arguments:

    dhcp - Points to the DHCP structure
    yiaddr - Offered address
    param - Option parameters

Return Value:

    Status code

--*/

{
    ASSERT(dhcp->state == STATE_INIT);

    VERBOSE_("Received DHCPOFFER %s", IPADDRSTR(yiaddr));
    VERBOSE_(" from %s", IPADDRSTR(param->dhcpServer));
    VERBOSE_(" @ time %d", DhcpTime());

    // Simply sanity check of offered parameters
    if (!DhcpValidateOffer(yiaddr, param))
        return NETERR_PARAM;

    dhcp->dhcpServer = param->dhcpServer;
    dhcp->dhcpaddr = yiaddr;

    // Send DHCPREQUEST and ignore error
    DhcpChangeState(dhcp, STATE_REQUESTING);
    return NETERR_OK;
}


PRIVATE VOID
DhcpAddOrRemoveGateways(
    DhcpInfo* dhcp,
    BOOL doDelete
    )

/*++

Routine Description:

    Set or delete default gateways

Arguments:

    dhcp - Points to the DHCP structure
    doDelete - Whether to set or delete gateways

Return Value:

    NONE

--*/

#define DEFAULT_METRIC 1

{
    UINT i;
    IPADDR addr;

    if (doDelete) {
        for (i=0; i < dhcp->gatewayCount; i++) {
            addr = dhcp->gateways[i];
            TRACE_("Remove gateway: %s", IPADDRSTR(addr));
            IpRemoveDefaultGateway(addr);
        }
    } else {
        for (i=0; i < dhcp->gatewayCount; i++) {
            addr = dhcp->gateways[i];
            TRACE_("Add gateway: %s", IPADDRSTR(addr));
            IpAddDefaultGateway(addr, DEFAULT_METRIC+i, dhcp->ifp);
        }
    }
}


VOID
DhcpSetDefaultGateways(
    IfInfo* ifp
    )
{
    KIRQL irql = RaiseToDpc();
    DhcpInfo* dhcp = ifp->dhcpInfo;

    if (dhcp) {
        DhcpAddOrRemoveGateways(dhcp, FALSE);
    }

    LowerFromDpc(irql);
}


PRIVATE VOID
DhcpUseOptionParams(
    DhcpInfo* dhcp,
    DhcpOptionParam* param
    )

/*++

Routine Description:

    Make use of the option parameters received from the server

Arguments:

    dhcp - Points to the DHCP structure
    param - Optional parameter values

Return Value:

    Status code

--*/

{
    BOOL resetGateways;

    //
    // Remember the originating server address and lease info
    //
    dhcp->dhcpServer = param->dhcpServer;

    if (param->exptime == DHCPTIME_INFINITE) {
        dhcp->t1time =
        dhcp->t2time =
        dhcp->exptime = DHCPTIME_INFINITE;
    } else {
        dhcp->t1time = dhcp->acqtime + param->t1time;
        dhcp->t2time = dhcp->acqtime + param->t2time;
        dhcp->exptime = dhcp->acqtime + param->exptime;
    }

    //
    // Set gateways in the IP stack
    //
    if (param->gatewayCount == 0 || dhcp->gatewayCount == 0) {
        resetGateways = TRUE;
    } else {
        UINT oldcnt = dhcp->gatewayCount;
        UINT newcnt = param->gatewayCount;
        UINT i, j;

        for (i=0; i < newcnt; i++) {
            for (j=0; j < oldcnt; j++)
                if (param->gateways[i] != dhcp->gateways[j]) break;
            if (j < oldcnt) break;
        }
        resetGateways = (i < newcnt);
    }

    if (resetGateways) {
        //
        // Reset gateways if anything has changed
        //  first delete existing gateways
        //  then set new gateways
        //
        // NOTE: should we ping the new gateways here?
        //
        DhcpAddOrRemoveGateways(dhcp, TRUE);

        dhcp->gatewayCount = param->gatewayCount;
        CopyMem(dhcp->gateways,
                param->gateways,
                param->gatewayCount * sizeof(IPADDR));

        DhcpAddOrRemoveGateways(dhcp, FALSE);
    }

    dhcp->dnsServerCount = param->dnsServerCount;
    CopyMem(dhcp->dnsServers,
            param->dnsServers,
            param->dnsServerCount * sizeof(IPADDR));

    strcpy(dhcp->domainName, param->domainName);
}


PRIVATE NTSTATUS
DhcpProcessAck(
    DhcpInfo* dhcp,
    IPADDR yiaddr,
    DhcpOptionParam* param
    )

/*++

Routine Description:

    Process a received DHCPACK packet

Arguments:

    dhcp - Points to the DHCP structure
    yiaddr - Offered address
    param - Option parameters

Return Value:

    Status code

--*/

{
    VERBOSE_("Received DHCPACK %s", IPADDRSTR(yiaddr));
    VERBOSE_("  from %s", IPADDRSTR(param->dhcpServer));

    ASSERT(dhcp->state == STATE_INIT_REBOOT ||
           dhcp->state == STATE_REQUESTING ||
           dhcp->state == STATE_RENEWING ||
           dhcp->state == STATE_REBINDING);

    // Simply sanity check of offered parameters
    if (!DhcpValidateOffer(yiaddr, param))
        return NETERR_PARAM;

    // Note: We're not checking for address conflicts
    // and just assume the offered address is valid.

    // If we're currently using a different address, give it up
    if ((dhcp->activeaddr != 0) &&
        (dhcp->activeaddr != yiaddr ||
         dhcp->activemask != param->dhcpmask) ||
         ActiveAutonetAddr(dhcp))
    {
        VERBOSE_("Giving up old IP address %s", IPADDRSTR(dhcp->activeaddr));
        DhcpResetInterface(dhcp);
    }

    // If we got a new address, set it down in the IP stack
    if (dhcp->activeaddr == 0) {
        IfSetIpAddr(dhcp->ifp, yiaddr, param->dhcpmask);
        DhcpSignalAddrEvent(dhcp);
        dhcp->activeaddr = dhcp->dhcpaddr = yiaddr;
        dhcp->activemask = dhcp->dhcpmask = param->dhcpmask;
        dhcp->flags |= FLAG_ACTIVE_DHCPADDR;
    }

    VERBOSE_("Accepted IP address: %s", IPADDRSTR(yiaddr));
    VERBOSE_("Lease time: %d / %d / %d", param->t1time, param->t2time, param->exptime);

    //
    // Set other option parameters
    //
    dhcp->initRetryCount = 0;
    DhcpUseOptionParams(dhcp, param);

    // We're now in bound state.
    // Set timer to expire at T1 time.
    DhcpChangeState(dhcp, STATE_BOUND);
    return NETERR_OK;
}


PRIVATE NTSTATUS
DhcpProcessNak(
    DhcpInfo* dhcp,
    IPADDR dhcpServer
    )

/*++

Routine Description:

    Process a received DHCPNAK packet

Arguments:

    dhcp - Points to the DHCP structure
    dhcpServer - The originating server address

Return Value:

    Status code

--*/

{
    VERBOSE_("Received DHCPNAK from %s", IPADDRSTR(dhcpServer));

    ASSERT(dhcp->state == STATE_INIT_REBOOT ||
           dhcp->state == STATE_REQUESTING ||
           dhcp->state == STATE_RENEWING ||
           dhcp->state == STATE_REBINDING);

    // Barf if we got DHCPNAK from an unexpected server
    if (dhcp->state != STATE_INIT_REBOOT && dhcpServer != dhcp->dhcpServer) {
        WARNING_("Random DHCPNAK from %s?", IPADDRSTR(dhcpServer));
    }

    // If we're using an address, give it up
    if (dhcp->activeaddr) {
        DhcpResetInterface(dhcp);
    }

    //
    // Go to INIT state to start over again
    // Send DHCPDISCOVER and ignore error
    //
    DhcpChangeState(dhcp, STATE_INIT);
    return NETERR_OK;
}


PRIVATE NTSTATUS
DhcpParseOptionParams(
    DhcpOptionParam* param,
    const BYTE* buf,
    UINT buflen,
    BYTE* overload
    )

/*++

Routine Description:

    Parse option parameters in a received DHCP packet

Arguments:

    param - Where to store the resulting information
    buf - Points to the option data buffer
    buflen - Buffer length
    overload - Returns the overload option value

Return Value:

    Status code

--*/

// NOTE: we're assuming little-endian machine here.
#define EXTRACT_DWORD_OPTION(_result) \
        if (len != 4) goto exit; \
        if (_result == 0) { \
            _result = (((DWORD) buf[0] << 24) | \
                       ((DWORD) buf[1] << 16) | \
                       ((DWORD) buf[2] <<  8) | \
                       ((DWORD) buf[3]      )); \
        }

#define EXTRACT_IPADDR_OPTION(_result) \
        if (len != 4) goto exit; \
        if (_result == 0) _result = *((IPADDR*) buf)

{
    while (buflen) {
        UINT tag, len;

        // Stop after seeing the 'end' option
        if ((tag = buf[0]) == DHCPOPT_END) break;

        // Skip the 'pad' option
        if (tag == DHCPOPT_PAD) {
            buf++; buflen--;
            continue;
        }

        // Is the option length field valid?
        if (buflen < 2 || buflen-2 < (len = buf[1])) goto exit;
        buf += 2; buflen -= 2;
    
        // Interpret option data
        switch (tag) {
        case DHCPOPT_DHCP_MESSAGE_TYPE:
            if (len != 1) goto exit;
            if (param->recvMsgType == 0)
                param->recvMsgType = *buf;
            break;

        case DHCPOPT_SERVERID:
            EXTRACT_IPADDR_OPTION(param->dhcpServer);
            break;

        case DHCPOPT_SUBNET_MASK:
            EXTRACT_IPADDR_OPTION(param->dhcpmask);
            break;

        case DHCPOPT_ROUTERS:
        case DHCPOPT_DNS_SERVERS: {
            UINT* pcount;
            IPADDR* parray;
            UINT n;

            if (len == 0 || len % sizeof(IPADDR) != 0) goto exit;
            if (tag == DHCPOPT_ROUTERS) {
                pcount = &param->gatewayCount;
                parray = param->gateways;
                n = MAX_DEFAULT_GATEWAYS * sizeof(IPADDR);
            } else {
                pcount = &param->dnsServerCount;
                parray = param->dnsServers;
                n = MAX_DEFAULT_DNSSERVERS * sizeof(IPADDR);
            }
        
            if (n > len) n = len;
            if (*pcount == 0) {
                *pcount = n / sizeof(IPADDR);
                CopyMem(parray, buf, n);
            }
            }
            break;

        case DHCPOPT_DOMAIN_NAME:
            if (len < 1 || len >= sizeof(param->domainName)) goto exit;
            if (param->domainName[0] == 0) {
                CopyMem(param->domainName, buf, len);
                param->domainName[len] = 0;
            }
            break;

        case DHCPOPT_IPADDR_LEASE_TIME:
            EXTRACT_DWORD_OPTION(param->exptime);
            break;

        case DHCPOPT_T1_INTERVAL:
            EXTRACT_DWORD_OPTION(param->t1time);
            break;

        case DHCPOPT_T2_INTERVAL:
            EXTRACT_DWORD_OPTION(param->t2time);
            break;

        case DHCPOPT_FIELD_OVERLOAD:
            if (len != 1) goto exit;
            if (overload && *overload == 0) *overload = *buf;
            break;
        }

        buf += len; buflen -= len;
    }

    return NETERR_OK;

exit:
    WARNING_("Invalid option data");
    return NETERR_PARAM;
}


PRIVATE VOID
DhcpSelectAutonetAddr(
    DhcpInfo* dhcp
    )

/*++

Routine Description:

    Attempt to select an autonet address

Arguments:

    dhcp - Points to the DHCP structure

Return Value:

    NONE

--*/

{
    IPADDR addr;

    ASSERT(dhcp->state == STATE_SELECT_AUTOADDR);

    if (++dhcp->initRetryCount > cfgAutonetAddrRetries) {
        //
        // We tried too many autonet addresses without success.
        // Just give up.
        //
        dhcp->initRetryCount = 0;
        DhcpChangeState(dhcp, STATE_NONE);

        // Signal that Xnet initialization was completed abnormally
        WARNING_("Failed to pick an autonet address.");
        DhcpSignalAddrEvent(dhcp);
        return;
    }

    // Generate a random autonet address
    addr = AUTONET_ADDRBASE + XnetRandScaled(AUTONET_ADDRRANGE);
    dhcp->autonetaddr = addr = HTONL(addr);

    VERBOSE_("Trying autonet address: %s", IPADDRSTR(addr));

    if (dhcp->ifp->flags & IFFLAG_CONNECTED_BOOT) {
        DhcpCheckAddressConflict(dhcp->ifp, addr);
        dhcp->retries = 0;
        DhcpComputeTimeout(dhcp);
    } else {
        //
        // NOTE: if net cable isn't connected at boot time,
        // there is no point trying to check for address conflicts.
        //
        dhcp->retries = cfgConflictCheckRetries;
        DhcpHandleTimeout(dhcp);
    }
}


NTSTATUS
DhcpGetDefaultDnsServers(
    IfInfo* ifp,
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

    ifp - Points to an interface structure
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
    DhcpInfo* dhcp = ifp->dhcpInfo;
    UINT count;

    RUNS_AT_DISPATCH_LEVEL
    if (!dhcp) return NETERR_PARAM;

    //
    // Return the default domain name
    //
    count = dhcp->domainName ? strlen(dhcp->domainName) : 0;
    if (namelen <= count) return NETERR_MEMORY;
    if (count) {
        CopyMem(domainName, dhcp->domainName, count);
    }
    domainName[count] = 0;

    //
    // Return the default DNS server addresses
    //
    if (dhcp->dnsServerCount) {
        count = min(*serverCnt, dhcp->dnsServerCount);
        *serverCnt = count;
        CopyMem(serverAddrs, dhcp->dnsServers, count*sizeof(IPADDR));
    } else {
        *serverCnt = 0;
    }

    return NETERR_OK;
}


PRIVATE IPADDR
DhcpReadConfigIpAddr(
    ULONG index
    )

/*++

Routine Description:

    Wrapper function to read an IP address value
    from the xbox configuration sector

Arguments:

    index - Specifies the address of interest

Return Value:

    IP address value, 0 if there is an error

--*/

{
    ULONG type, size, addr;
    INT err = XQueryValue(index, &type, (VOID*) &addr, IPADDRLEN, &size);

    // If the query call failed, return 0 address.
    if (err != NO_ERROR || type != REG_DWORD || size != IPADDRLEN) {
        VERBOSE_("XQueryValue(%d) failed: %d, %d, %d", index, err, type, size);
        return 0;
    }

    return addr;
}


PRIVATE VOID
DhcpLoadConfigInfo(
    DhcpInfo* dhcp
    )

/*++

Routine Description:

    Load persistent DHCP configuration parameters

Arguments:

    dhcp - Points to the DHCP structure

Return Value:

    NONE

--*/

{
    //
    // Read the static IP address value.
    // Return right away if none is set.
    //
    dhcp->activeaddr = DhcpReadConfigIpAddr(XC_ONLINE_IP_ADDRESS);
    if (dhcp->activeaddr == 0) return;

    //
    // Read the subnet mask value.
    // If none is set, generate a default mask.
    //
    dhcp->activemask = DhcpReadConfigIpAddr(XC_ONLINE_SUBNET_ADDRESS);
    if (dhcp->activemask == 0) {
        dhcp->activemask = XnetGetDefaultSubnetMask(dhcp->activeaddr);
    }

    //
    // Read the default gateway address.
    //
    dhcp->gateways[0] = DhcpReadConfigIpAddr(XC_ONLINE_DEFAULT_GATEWAY_ADDRESS);
    if (dhcp->gateways[0] != 0)
        dhcp->gatewayCount = 1;
}


DWORD
DhcpGetActiveAddressType(
    IfInfo* ifp
    )

/*++

Routine Description:

    Determine how we obtained our currently active IP address

Arguments:

    ifp - Points to the interface structure

Return Value:

    Flags (see winsockx.h)

--*/

{
    DhcpInfo* dhcp;
    DWORD flags = 0;

    RUNS_AT_DISPATCH_LEVEL

    if ((dhcp = ifp->dhcpInfo) != NULL) {
        if (ActiveDhcpAddr(dhcp)) {
            flags = XNET_ADDR_DHCP;
        } else if (ActiveAutonetAddr(dhcp)) {
            flags = XNET_ADDR_AUTOIP;
        } else if (dhcp->state == STATE_STATIC_ADDR) {
            flags = XNET_ADDR_STATIC;
        }

        if (dhcp->gatewayCount)
            flags |= XNET_HAS_GATEWAY;
    }

    return flags;
}


NTSTATUS
DhcpWaitForAddress(
    IfInfo* ifp
    )

/*++

Routine Description:

    Wait for address acquisition process to complete

Arguments:

    ifp - Points to the interface structure

Return Value:

    Status code

--*/

{
    DhcpInfo* dhcp = ifp->dhcpInfo;
    NTSTATUS status;

    if (dhcp) {
        status = WaitKernelEventObject(&dhcp->addrEvent, 0);

        // Check if the acquisition process failed
        if (status == NETERR_OK && dhcp->state == STATE_NONE)
            status = NETERR_NETDOWN;
    } else
        status = NETERR(WSASYSNOTREADY);
    
    return status;
}

