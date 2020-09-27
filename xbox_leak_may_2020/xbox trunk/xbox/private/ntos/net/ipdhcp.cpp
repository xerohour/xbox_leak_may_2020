// ---------------------------------------------------------------------------------------
// ipdhcp.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

DefineTag(dhcp, 0);

#ifdef XNET_FEATURE_DHCP

//
// DHCP message format
// NOTE: default the options field to 64 bytes
// which is the size of BOOTP vendor-specified area
//

typedef struct _DhcpMessage {
    BYTE _op;                // message type
    BYTE _htype;             // hareware address type
    BYTE _hlen;              // hardware address length
    BYTE _hops;              // relay hops
    DWORD _xid;              // transaction ID
    WORD _secs;              // seconds since address acquisition process began
    WORD _flags;             // flags
    CIpAddr _ciaddr;          // client IP address
    CIpAddr _yiaddr;          // "your" (client) IP address
    CIpAddr _siaddr;          // server IP address
    CIpAddr _giaddr;          // relay agent IP address
    BYTE _chaddr[16];        // client hardware address
    BYTE _sname[64];         // optional server hostname
    BYTE _file[128];         // boot filename
    BYTE _options[64];       // optional parameters (variable length)
} DhcpMessage;

//
// Our default DHCP packet buffer is 576 bytes
//
#define BOOTP_MESSAGE_SIZE sizeof(DhcpMessage)
#define DEFAULT_DHCP_BUFSIZE 576
#define DHCPHDRLEN offsetof(DhcpMessage, _options)

//
// Minimum and maximum DHCP hardware address length
//
#define MIN_DHCP_HWADDRLEN 1
#define MAX_DHCP_HWADDRLEN 16

//
// BOOTP message type constants
//
#define BOOTREQUEST 1
#define BOOTREPLY   2

//
// Hardware type constants
//
#define HWTYPE_10MB_ETHERNET 1
#define HWTYPE_PPP           8

//
// Constants for DHCP_MESSAGE.flags
//
#define DHCP_BROADCAST 0x8000

//
// DHCP message type constants
//
#define DHCPDISCOVER    1
#define DHCPOFFER       2
#define DHCPREQUEST     3
#define DHCPDECLINE     4
#define DHCPACK         5
#define DHCPNAK         6
#define DHCPRELEASE     7
#define DHCPINFORM      8

//
// DHCP option tag constants
//

// Fixed-length options

#define DHCPOPT_PAD                     0
#define DHCPOPT_END                     255

// Standard options

#define DHCPOPT_SUBNET_MASK             1
#define DHCPOPT_TIME_OFFSET             2
#define DHCPOPT_ROUTERS                 3
#define DHCPOPT_TIME_SERVERS            4
#define DHCPOPT_IEN116_SERVERS          5
#define DHCPOPT_DNS_SERVERS             6
#define DHCPOPT_LOG_SERVERS             7
#define DHCPOPT_COOKIE_SERVERS          8
#define DHCPOPT_LPR_SERVERS             9
#define DHCPOPT_IMPRESS_SERVERS         10
#define DHCPOPT_RLP_SERVERS             11
#define DHCPOPT_HOST_NAME               12
#define DHCPOPT_BOOT_FILESIZE           13
#define DHCPOPT_DUMP_FILE               14
#define DHCPOPT_DOMAIN_NAME             15
#define DHCPOPT_SWAP_SERVER             16
#define DHCPOPT_ROOT_PATH               17
#define DHCPOPT_EXTENSIONS_PATH         18

// IP parameters (per host)

#define DHCPOPT_ENABLE_FORWARD          19
#define DHCPOPT_NONLOCAL_SRCROUTING     20
#define DHCPOPT_POLICY_FILTER           21
#define DHCPOPT_MAX_REASSEMBLY_SIZE     22
#define DHCPOPT_DEFAULT_IP_TTL          23
#define DHCPOPT_PMTU_AGING_TIMEOUT      24
#define DHCPOPT_PMTU_PLATEAU_TABLE      25

// IP parameters (per interface)

#define DHCPOPT_INTERFACE_MTU           26
#define DHCPOPT_ALL_SUBNETS_LOCAL       27
#define DHCPOPT_BROADCAST_ADDRESS       28
#define DHCPOPT_MASK_DISCOVERY          29
#define DHCPOPT_MASK_SUPPLIER           30
#define DHCPOPT_ROUTER_DISCOVERY        31
#define DHCPOPT_SOLICIT_ROUTER_ADDR     32
#define DHCPOPT_STATIC_ROUTES           33

// Link layer parameters

#define DHCPOPT_TRAILER_ENCAPS          34
#define DHCPOPT_ARP_CACHE_TIMEOUT       35
#define DHCPOPT_ETHERNET_ENCAPS         36

// TCP parameters

#define DHCPOPT_DEFAULT_TCP_TTL         37
#define DHCPOPT_KEEPALIVE_INTERVAL      38
#define DHCPOPT_KEEPALIVE_GARBAGE       39

// Application parameters

#define DHCPOPT_NIS_DOMAIN              40
#define DHCPOPT_NIS_SERVERS             41
#define DHCPOPT_NTP_SERVERS             42

// DHCP extensions

#define DHCPOPT_REQUESTED_CIpAddr        50
#define DHCPOPT_IPADDR_LEASE_TIME       51
#define DHCPOPT_FIELD_OVERLOAD          52
#define DHCPOPT_TFTP_SERVER_NAME        66
#define DHCPOPT_BOOT_FILENAME           67
#define DHCPOPT_DHCP_MESSAGE_TYPE       53
#define DHCPOPT_SERVERID                54
#define DHCPOPT_PARAM_REQUEST_LIST      55
#define DHCPOPT_MESSAGE                 56
#define DHCPOPT_MAX_DHCP_MESSAGESIZE    57
#define DHCPOPT_T1_INTERVAL             58
#define DHCPOPT_T2_INTERVAL             59
#define DHCPOPT_VENDOR_CLASSID          60
#define DHCPOPT_CLIENTID                61

// Vendor-specific information

#define DHCPOPT_VENDOR_INFO             43
#define DHCPOPT_NETBIOS_NAMESERVERS     44
#define DHCPOPT_NETBIOS_DATASERVERS     45
#define DHCPOPT_NETBIOS_NODETYPE        46
#define DHCPOPT_NETBIOS_SCOPE           47
#define DHCPOPT_X11_FONTSERVERS         48
#define DHCPOPT_X11_DISPLAYSERVERS      49
#define DHCPOPT_NIS2_DOMAIN             64
#define DHCPOPT_NIS2_SERVERS            65
#define DHCPOPT_MOBILEIP_HOME_AGENTS    68
#define DHCPOPT_SMTP_SERVERS            69
#define DHCPOPT_POP3_SERVERS            70
#define DHCPOPT_NNTP_SERVERS            71
#define DHCPOPT_WWW_SERVERS             72
#define DHCPOPT_FINGER_SERVERS          73
#define DHCPOPT_IRC_SERVERS             74
#define DHCPOPT_STREETTALK_SERVERS      75
#define DHCPOPT_STREETTALKDA_SERVERS    76

//
// DHCP magic cookie: 99, 130, 83, 99
//
//#define DHCPCOOKIELEN 4
//extern const BYTE DhcpMagicCookie[DHCPCOOKIELEN];

//
// DHCP time value (in seconds)
//
// NOTE: We use 32-bit unsigned interface to represent
// the number of seconds ellapsed since the start of 1/1/2000.
// This should last until year 2136 before overflowing.
//

#define DHCPTIME_INFINITE 0xffffffff

//
// Autonet address range: 169.254/16
//  don't use the first 256 and the last 256 addresses
//
#define AUTONET_ADDRMASK    0xffff0000
#define AUTONET_ADDRBASE    0xa9fe0100
#define AUTONET_ADDRRANGE   0x0000fcff

//
// States for an interface
//
enum {
    STATE_NONE,             // temporary wait state after startup
    STATE_STATIC_ADDR,      // statically assigned address
    STATE_INIT,             // sending discover
    STATE_REQUESTING,       // waiting for DHCPACK after selecting
    STATE_INIT_REBOOT,      // rebooting with valid lease
    STATE_BOUND,            // bound with valid address lease
    STATE_RENEWING,         // renewing after T1 expires
    STATE_REBINDING,        // rebinding after T2 expires
    STATE_DECLINING,        // temporary wait state after sending decline
    STATE_INIT_AUTOADDR,    // waiting for timer to kick off autonet
    STATE_SELECT_AUTOADDR   // checking autonet address conflicts
};

//
// Constants for the DhcpInfo.flags field:
//
#define FLAG_ACTIVE_DHCPADDR    0x00010000
#define FLAG_ACTIVE_AUTONETADDR 0x00020000
#define FLAG_ACTIVE_NOADDR      0x00040000
#define FLAG_ACTIVE_ADDRMASK    (FLAG_ACTIVE_DHCPADDR | FLAG_ACTIVE_AUTONETADDR | FLAG_ACTIVE_NOADDR)

BOOL CXnIp::ActiveDhcpAddr()
{
    return (_flags & FLAG_ACTIVE_DHCPADDR);
}

BOOL CXnIp::ActiveAutonetAddr()
{
    return (_flags & FLAG_ACTIVE_AUTONETADDR);
}

BOOL CXnIp::ActiveStaticAddr()
{
    return(_state == STATE_STATIC_ADDR);
}

BOOL CXnIp::ActiveNoAddr()
{
    return(_flags & FLAG_ACTIVE_NOADDR);
}

//
// Dump DHCP message in the debugger
//

#if DBG
VOID DhcpDumpMessage(DhcpMessage* msg, UINT msglen);
#else
#define DhcpDumpMessage(msg, msglen)
#endif

// ---------------------------------------------------------------------------------------

//
// First 4 bytes of the options field in a DHCP message
// must match the following magic cookie
//
const BYTE DhcpMagicCookie[] = { 99, 130, 83, 99 };

//
// Default address lease time requested:
//  let the server decide
//
//UINT cfgDefaultLeaseTime = 0;

//
// Minimum retransmission timeout while in
// RENEWING and REBINDING states: 60 seconds
//
//UINT cfgMinRenewTimeout = 60;

//
// Number of seconds to wait after reboot
// before we start sending out DHCPDISCOVER packets
//
// NOTE: RFC2131 recommends that we wait between 1 to 10 seconds.
// But we're only doing 1 seconds here for faster startup time.
//
//UINT cfgStartupWaitMin = 0;
//UINT cfgStartupWaitMax = 1;

//
// Number of times we'll attempt to resend
// DHCPDISCOVER and DHCPREQUEST packets
//
// NOTE: Non-standard behavior!!!
//  We're capping the timeout between retries to a maximum of 10 seconds.
//
//UINT cfgDhcpRetryCount = 3;
//UINT cfgDhcpRetryMaxTimeout = 10;

//
// Number of autonet addresses we'll attempt before giving up
//
//UINT cfgAutonetAddrRetries = 10;

//
// How frequently to look for DHCP server when in Autonet mode
//
//UINT cfgAutonetDhcpCheckInterval = 5*60;

//
// How many ARP requests to send
// when checking for address conflict
//
//UINT cfgConflictCheckRetries = 2;

// Check to see if we're forced to use autonet address
//  (without trying to find DHCP servers)
INLINE
BOOL CXnIp::DhcpForceAutonet(CIpAddr addr, CIpAddr mask)
{
    ICHECK(IP, USER|UDPC|SDPC);

    return (mask == HTONL(AUTONET_ADDRMASK)) && 
            (addr & mask) == (HTONL(AUTONET_ADDRBASE) & mask);
}

NTSTATUS CXnIp::DhcpInit()
{
    ICHECK(IP, USER);

    _state = STATE_NONE;
    _timerDhcp.Init((PFNTIMER)DhcpTimer);
    _dwXidNext = RandLong();

    return(NETERR_OK);
}

INT CXnIp::DhcpConfig(const XNetConfigParams * pxncp)
{
    ICHECK(IP, UDPC);

    //@@@ Cannot be reconfigured.  Need DHCP rewrite.

    _activeaddr = pxncp->ina.s_addr;

    if (_activeaddr != 0)
    {
        _activemask = pxncp->inaMask.s_addr;

        if (_activemask == 0 || !_activemask.IsValidMask())
            _activemask = _activeaddr.DefaultMask();

        _options._gatewayCount = 0;

        if (pxncp->inaGateway.s_addr != 0)
        {
            _options._gateways[_options._gatewayCount] = pxncp->inaGateway.s_addr;
            _options._gatewayCount += 1;
        }

        _options._dnsServerCount = 0;

        if (pxncp->inaDnsPrimary.s_addr != 0)
        {
            _options._dnsServers[_options._dnsServerCount] = pxncp->inaDnsPrimary.s_addr;
            _options._dnsServerCount += 1;
        }

        if (pxncp->inaDnsSecondary.s_addr != 0)
        {
            _options._dnsServers[_options._dnsServerCount] = pxncp->inaDnsSecondary.s_addr;
            _options._dnsServerCount += 1;
        }
    }

    if (!TestInitFlag(INITF_CONNECTED_BOOT))
    {
        TraceSz(Warning, "Ethernet cable not plugged in.  No IP address acquired.");
        _flags = (_flags & ~FLAG_ACTIVE_ADDRMASK) | FLAG_ACTIVE_NOADDR;
    }
#if defined(XNET_FEATURE_INSECURE) && !defined(XNET_FEATURE_SG)
    else if (!(cfgFlags & XNET_STARTUP_BYPASS_SECURITY))
    {
        // When operating in secure mode don't acquire an IP address so we behave
        // more like xnets.lib.

        _flags = (_flags & ~FLAG_ACTIVE_ADDRMASK) | FLAG_ACTIVE_NOADDR;
    }
#endif
    else if (_activeaddr != 0 && !DhcpForceAutonet(_activeaddr, _activemask))
    {
        // If we're using static address, inform the IP stack
        IpSetAddress(_activeaddr, _activemask);
        DhcpSetDefaultGateways();
        DhcpChangeState(STATE_STATIC_ADDR);
    }
    else if ((cfgFlags & XNET_STARTUP_BYPASS_DHCP) || _activeaddr != 0)
    {
        TraceSz(Warning, "Forcing autonet...");
        _activeaddr = _activemask = 0;
        DhcpChangeState(STATE_INIT_AUTOADDR);
    }
    else
    {
        // Obtain address via DHCP or Autonet.  Be quiet for a while after startup.
        DhcpTimerSetRelative(0, 0);
    }

    return(0);
}

void CXnIp::DhcpTerm()
{
    ICHECK(IP, UDPC);
    TimerSet(&_timerDhcp, TIMER_INFINITE);
}

void CXnIp::DhcpRecv(CPacket * ppkt, CUdpHdr * pUdpHdr, UINT msglen)
{
    ICHECK(IP, UDPC|SDPC);

    DhcpMessage * msg = (DhcpMessage *)(pUdpHdr + 1);
    CDhcpOptions * dhcpParams = NULL;
    BYTE* option;
    BYTE overload = 0;
    NTSTATUS status = NETERR_NOTIMPL;

    if (msglen < DHCPHDRLEN)
    {
        TraceSz1(pktRecv, "[DISCARD] DHCP packet is too short (%d)", msglen);
        return;
    }

    if (memcmp(msg->_chaddr, _ea._ab, sizeof(CEnetAddr)) != 0)
    {
        TraceSz(pktRecv, "[DISCARD] DHCP packet not meant for me");
        return;
    }

    if (    msg->_op != BOOTREPLY
        ||  msg->_htype != HWTYPE_10MB_ETHERNET
        ||  msg->_hlen != sizeof(CEnetAddr)
        ||  NTOHL(msg->_xid) != _xid)
    {
        TraceSz4(pktWarn, "[DISCARD] DHCP packet is invalid (%d,%d,%d,%d)",
                 msg->_op != BOOTREPLY,
                 msg->_htype != HWTYPE_10MB_ETHERNET,
                 msg->_hlen != sizeof(CEnetAddr),
                 NTOHL(msg->_xid) != _xid);
        return;
    }

    DhcpDumpMessage(msg, msglen);

    dhcpParams = (CDhcpOptions *)PoolAllocZ(sizeof(*dhcpParams), PTAG_CDhcpOptions);

    if (!dhcpParams)
    {
        TraceSz(pktWarn, "[DISCARD] Out of memory allocating CDhcpOptions");
        status = NETERR_MEMORY;
        goto exit;
    }

    //
    // Skip the DHCP magic cookie
    //
    option = msg->_options;
    msglen -= DHCPHDRLEN;
    if (    msglen < sizeof(DhcpMagicCookie)
        ||  memcmp(option, DhcpMagicCookie, sizeof(DhcpMagicCookie)) != 0)
    {
        TraceSz1(pktWarn, "[DISCARD] DHCP packet has invalid magic cookie (%d)", msglen);
        goto exit;
    }

    msglen -= sizeof(DhcpMagicCookie);
    option += sizeof(DhcpMagicCookie);

    //
    // Parse the regular options
    //
    status = DhcpParseOptionParams(dhcpParams, option, msglen, &overload);

    if (!NT_SUCCESS(status))
    {
        TraceSz(pktWarn, "[DISCARD] DHCP regular options are invalid");
        goto exit;
    }

    //
    // Parse overloaded options in the msg->_file field
    //
    if (overload & 1)
    {
        status = DhcpParseOptionParams(dhcpParams, msg->_file, sizeof(msg->_file), NULL);

        if (!NT_SUCCESS(status))
        {
            TraceSz(pktWarn, "[DISCARD] DHCP file options are invalid");
            goto exit;
        }
    }

    //
    // Parse overloaded options in the msg->_sname field
    //
    if (overload & 2)
    {
        status = DhcpParseOptionParams(dhcpParams, msg->_sname, sizeof(msg->_sname), NULL); 

        if (!NT_SUCCESS(status))
        {
            TraceSz(pktWarn, "[DISCARD] DHCP sname options are invalid");
            goto exit;
        }
    }

    //
    // The received packet must have a server identifier option
    //
    if (dhcpParams->_dhcpServer == 0)
    {
        TraceSz(pktWarn, "[DISCARD] DHCP packet lacks server identifier option");
        status = NETERR_PARAM;
        goto exit;
    }

    //
    // Now that we've parsed the option data,
    // use it appropriately
    //
    status = NETERR_NOTIMPL;

    switch (_state) {
    case STATE_INIT:
        // Expecting DHCPOFFER...
        if (dhcpParams->_recvMsgType == DHCPOFFER)
            status = DhcpProcessOffer(msg->_yiaddr, dhcpParams);
        break;

    case STATE_INIT_REBOOT:
    case STATE_REQUESTING:
    case STATE_RENEWING:
    case STATE_REBINDING:
        // Expecting DHCPACK or DHCPNAK...
        if (dhcpParams->_recvMsgType == DHCPACK)
            status = DhcpProcessAck(msg->_yiaddr, dhcpParams);
        else if (dhcpParams->_recvMsgType == DHCPNAK)
            status = DhcpProcessNak(dhcpParams->_dhcpServer);
        break;
    }

exit:

    if (!NT_SUCCESS(status) && status != NETERR_NOTIMPL)
    {
        TraceSz1(pktWarn, "[DISCARD] DhcpRecv failed: %08lX", status);
    }

    PoolFree(dhcpParams);
}

void CXnIp::DhcpNotifyAddressConflict()
{
    ICHECK(IP, UDPC|SDPC);

    if (_state == STATE_SELECT_AUTOADDR)
    {
        // the last autonet address we chose was no good
        // so we need to find another one
        DhcpSelectAutonetAddr();
    }
    else
    {
        TraceSz1(Warning, "!!! Address conflict: %s", _activeaddr.Str());
    }
}

//
// Number of seconds from start of 1601 to start of 2000
//
static const LONGLONG StartOf2000 = 0x2ee7dd480;

CXnIp::DHCPTIME CXnIp::DhcpTime()
{
    ICHECK(IP, UDPC|SDPC);

    LARGE_INTEGER currentTime;

    // Get the current UTC time
    //  = number of 100 nanoseconds since 1/1/1601
    KeQuerySystemTime(&currentTime);

    // Return the number of seconds since the start of 2000
    return (DHCPTIME) (currentTime.QuadPart / 10000000 - StartOf2000);
}


void CXnIp::DhcpTimerSetRelative(UINT minWait, UINT maxWait)
{
    ICHECK(IP, UDPC|SDPC);

    TimerSet(&_timerDhcp, TimerTick() + (minWait + RandScaled(maxWait - minWait)) * TICKS_PER_SECOND);
}

void CXnIp::DhcpTimerSetAbsolute(DHCPTIME dhcptime)
{
    ICHECK(IP, UDPC|SDPC);

    DWORD dwTick = TIMER_INFINITE;

    if (dhcptime != DHCPTIME_INFINITE)
    {
        DHCPTIME dhcptimeNow = DhcpTime();

        if (dhcptime > dhcptimeNow)
            dwTick = TimerTick() + (dhcptime - dhcptimeNow) * TICKS_PER_SECOND;
        else
            dwTick = TimerTick();
    }

    TimerSet(&_timerDhcp, dwTick);
}

void CXnIp::DhcpComputeTimeout()
{
    ICHECK(IP, UDPC|SDPC);

    UINT minWait, maxWait;

    switch (_state)
    {
    case STATE_INIT:
        if (ActiveAutonetAddr()) {
            //
            // Special case: we're currently using an Autonet address
            // and we're sending out periodic DHCPDISCOVER packets.
            //
            // DhcpSetIfTimerRelative(
            //     cfgAutonetDhcpCheckInterval,
            //     cfgAutonetDhcpCheckInterval);

            // NOTE: Non-standard behavior!!!
            //  We do not send out DHCPDISCOVER messages
            //  while we're using an active AutoNet address.
            DhcpTimerSetAbsolute(DHCPTIME_INFINITE);
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

        minWait = 2 << _retries;
        if (minWait > cfgDhcpRetryMaxTimeoutInSeconds)
            minWait = cfgDhcpRetryMaxTimeoutInSeconds;

        minWait -= 1;
        maxWait = minWait + 2;
        DhcpTimerSetRelative(minWait, maxWait);
        break;

    case STATE_BOUND:
        //
        // timeout when T1 expires
        //
        DhcpTimerSetAbsolute(_options._t1time);
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
        t2 = (_state == STATE_RENEWING) ? _options._t2time : _options._exptime;

        if (t1 < t2) {
            minWait = (t2-t1) / 2;
            if (minWait >= cfgDhcpRenewMinTimeoutInSeconds)
                t1 += minWait;
            else {
                t1 += cfgDhcpRenewMinTimeoutInSeconds;
                if (t1 > t2) t1 = t2;
            }
        }

        DhcpTimerSetAbsolute(t1);
        }
        break;

    case STATE_DECLINING:
        //
        // Wait 10 seconds after sending DHCP decline
        //
        DhcpTimerSetRelative(cfgDhcpDeclineWaitInSeconds, cfgDhcpDeclineWaitInSeconds);
        break;
    
    case STATE_SELECT_AUTOADDR:
        //
        // Wait ~2 seconds for address conflict detection
        //
        DhcpTimerSetRelative(cfgAutoIpRexmitTimeoutInSeconds, cfgAutoIpRexmitTimeoutInSeconds);
        break;

    default:
        // Should not happen - shut off the timer just in case
        Assert(FALSE);

    case STATE_NONE:
    case STATE_STATIC_ADDR:
        DhcpTimerSetAbsolute(DHCPTIME_INFINITE);
        break;
    }
}

void CXnIp::DhcpTimer(CTimer * pt)
{
    ICHECK(IP, UDPC|SDPC);

    switch (_state) {
    case STATE_NONE:
        //
        // We got here because we just finished the quiet
        // period after startup
        //
        // If we had a valid lease before, start in
        // INIT-REBOOT state; otherwise, start in INIT state
        //
        DhcpChangeState(_dhcpaddr ? STATE_INIT_REBOOT : STATE_INIT);
        break;
    
    case STATE_INIT:
        //
        // We're sending DHCPDISCOVER messages
        //
        if (_retries >= cfgDhcpRetryCount && !ActiveAutonetAddr())
        {
            //
            // Too many retries, give up and
            // start Autonet address acquisition process
            //
            TraceSz(dhcp, "Couldn't discover DHCP server, trying autonet...");
            DhcpChangeState(STATE_SELECT_AUTOADDR);
        }
        else
        {
            //
            // Send out another DHCPDISCOVER packet
            //
            TraceSz(dhcp, "Retransmit DHCPDISCOVER");
            DhcpSendDiscover();
        }
        break;

    case STATE_INIT_REBOOT:
    case STATE_REQUESTING:
        //
        // We're sending DHCPREQUEST
        //
        if (_retries >= cfgDhcpRetryCount) {
            //
            // Too many retries, go back to INIT state
            //
            TraceSz(dhcp, _state == STATE_INIT_REBOOT ?
                        "Failed to reuse a prior lease" :
                        "No ack for an offered address");

            DhcpChangeState(STATE_INIT);
        } else {
            //
            // Send out another DHCPREQUEST packet
            //
            TraceSz(dhcp, "Retransmit DHCPREQUEST");
            DhcpSendRequest();
        }
        break;
    
    case STATE_DECLINING:
        //
        // We just finished waiting after sending DHCPDECLINE
        //
        DhcpChangeState(STATE_INIT);
        break;
    
    case STATE_BOUND:
        //
        // T1 expired, start the renewing process
        //
        TraceSz(dhcp, "Switching to RENEWING state");
        DhcpChangeState(STATE_RENEWING);
        break;

    case STATE_RENEWING:
        //
        // We're trying to renew a valid address.
        // If T2 expired, start the rebinding process.
        //
        if (DhcpTime() >= _options._t2time) {
            TraceSz(dhcp, "Switching to REBINDING state");
            DhcpChangeState(STATE_REBINDING);
        } else {
            // Send out another DHCPREQUEST
            TraceSz(dhcp, "Retransmit DHCPREQUEST");
            DhcpSendRequest();
        }
        break;

    case STATE_REBINDING:
        //
        // We're trying to rebind a valid lease
        //  did our lease expire?
        //
        if (DhcpTime() >= _options._exptime) {
            // Inform IP stack to discard the active address
            TraceSz(dhcp, "Address lease expired - start over");
            DhcpResetInterface();

            // Too bad, go back to INIT state
            DhcpChangeState(STATE_INIT);
        } else {
            // Send out another DHCPREQUEST
            TraceSz(dhcp, "Retransmit DHCPREQUEST");
            DhcpSendRequest();
        }
        break;

    case STATE_INIT_AUTOADDR:
        DhcpChangeState(STATE_SELECT_AUTOADDR);
        break;

    case STATE_SELECT_AUTOADDR:
        if (++_retries > cfgAutoIpRetriesPerAttempt)
        {
            // We've successfully picked an autonet address.
            TraceSz1(dhcp, "Selected autonet address: %s", _autonetaddr.Str());

            _activeaddr = _autonetaddr;
            _activemask = HTONL(AUTONET_ADDRMASK);
            _flags |= FLAG_ACTIVE_AUTONETADDR;
            IpSetAddress(_activeaddr, _activemask);

            // Switch to INIT state to continue looking
            // for a DHCP server
            _initRetryCount = 0;
            DhcpChangeState(STATE_INIT);
        }
        else
        {
            // No response to our previous ARP request.
            // Try again just to be sure.
            EnetXmitArp(_autonetaddr);
            DhcpComputeTimeout();
        }
        break;

    default:
        TraceSz(dhcp, "Unexpected timeout");
        DhcpTimerSetAbsolute(DHCPTIME_INFINITE);
        break;
    }
}


void CXnIp::DhcpChangeState(INT state)
{
    ICHECK(IP, UDPC|SDPC);

    INT oldstate = _state;

    _state = state;
    _retries = 0;

    // Assign a new transaction ID for the next outgoing message.

    _xid = _dwXidNext++;

    switch (state) {
    case STATE_INIT_REBOOT:
    case STATE_REQUESTING:
    case STATE_RENEWING:
    case STATE_REBINDING:
        // Send out DHCPREQUEST
        DhcpSendRequest();
        break;

    case STATE_INIT:
        if (oldstate == STATE_SELECT_AUTOADDR) {
            // We just selected an autonet address.
            // Continue to look for a DHCP server.
            DhcpComputeTimeout();
        }
        else if (++_initRetryCount > cfgDhcpRetryCount) {
            // We went through the INIT state too many times
            // without getting a valid address lease. Just give up.
            // We don't try Autonet because in this case there
            // is a DHCP server but somehow we can't work with it.
            TraceSz(Warning, "Couldn't get a valid DHCP address after many tries");
            DhcpChangeState(STATE_NONE);
            _flags = (_flags & ~FLAG_ACTIVE_ADDRMASK) | FLAG_ACTIVE_NOADDR;
        } else {
            // Send out DHCPDISCOVER
            DhcpSendDiscover();
        }
        break;

    case STATE_BOUND:
        DhcpComputeTimeout();
        TraceSz1(dhcp, "Sleep %d seconds till renewal...", _options._t1time - DhcpTime());
        break;

    case STATE_INIT_AUTOADDR:
        DhcpTimerSetAbsolute(0);
        break;

    case STATE_SELECT_AUTOADDR:
        _initRetryCount = 0;
        DhcpSelectAutonetAddr();
        break;

    case STATE_NONE:
    case STATE_STATIC_ADDR:
        DhcpTimerSetAbsolute(DHCPTIME_INFINITE);
        break;
    }
}

void CXnIp::DhcpResetInterface()
{
    ICHECK(IP, UDPC|SDPC);

    IpSetAddress(0, 0);
    _flags &= ~FLAG_ACTIVE_ADDRMASK;
    if (_state != STATE_STATIC_ADDR)
        _activeaddr = 0;
}


NTSTATUS CXnIp::DhcpSendMessage(CPacket * ppkt, UINT msglen, BOOL broadcast)
{
    ICHECK(IP, UDPC|SDPC);

    ppkt->SetCb(ppkt->GetCb() - DEFAULT_DHCP_BUFSIZE + msglen);

    CUdpHdr * pUdpHdr = ppkt->GetUdpHdr();
    DhcpMessage * msg = (DhcpMessage *)(pUdpHdr + 1);

    // Debug trace
    DhcpDumpMessage(msg, msglen);

    // Set broadcast flag if necessary
    if (!_activeaddr || ActiveAutonetAddr())
        msg->_flags |= HTONS(DHCP_BROADCAST);

    pUdpHdr->_ipportSrc = DHCP_CLIENT_PORT;
    pUdpHdr->_ipportDst = DHCP_SERVER_PORT;
    pUdpHdr->_wLen = HTONS(msglen + sizeof(CUdpHdr));

    // Call the IP layer to send out the packet
    IpFillAndXmit(ppkt, broadcast ? IPADDR_BROADCAST : _options._dhcpServer, IPPROTOCOL_UDP);

    // Calculate the timeout value
    _retries++;
    DhcpComputeTimeout();

    return(NETERR_OK);
}

//
// Append the 'parameter request list' option
//  !!! we assume the data buffer is large enough.
//
BYTE * DhcpAppendParamReqList(BYTE * option)
{
    *option++ = DHCPOPT_PARAM_REQUEST_LIST;
    *option++ = 4;
    *option++ = DHCPOPT_SUBNET_MASK;
    *option++ = DHCPOPT_ROUTERS;
    *option++ = DHCPOPT_DNS_SERVERS;
    *option++ = DHCPOPT_DOMAIN_NAME;
    return(option);
}

//
// Append an option whose value is a DWORD
//
BYTE * DhcpAppendDWordOption(BYTE * option, INT tag, DWORD val)
{
    option[0] = (BYTE) tag;
    option[1] = sizeof(DWORD);
    option += 2;

    memcpy(option, &val, sizeof(DWORD));
    return option + sizeof(DWORD);
}


//
// Append the 'address lease time' option
//
BYTE * CXnIp::DhcpAppendLeaseTimeOption(BYTE * option)
{
    ICHECK(IP, UDPC|SDPC);

    DWORD dwLeaseTime = cfgDhcpDefaultLeaseTimeInDays * (24 * 60 * 60);

    if (dwLeaseTime == 0)
        return option;

    return DhcpAppendDWordOption(option, DHCPOPT_IPADDR_LEASE_TIME, HTONL(dwLeaseTime));
}


BYTE * CXnIp::DhcpFillMessageHeader(BYTE * buf, INT msgtype)
{
    ICHECK(IP, UDPC|SDPC);

    DhcpMessage* msg = (DhcpMessage*) buf;
    BYTE* option = msg->_options;
    BYTE hwtype;

    memset(buf, 0, DEFAULT_DHCP_BUFSIZE);
    msg->_op = BOOTREQUEST;
    msg->_htype = hwtype = HWTYPE_10MB_ETHERNET;
    msg->_hlen = sizeof(CEnetAddr);
    memcpy(msg->_chaddr, _ea._ab, sizeof(CEnetAddr));

    //
    // Fill in the transaction ID field
    // NOTE: We reuse the same XID for retransmissions.
    //
    msg->_xid = HTONL(_xid);

    // Number of seconds since we started the address
    // acquisition process.
    msg->_secs = (WORD) HTONS((WORD)_secsSinceStart);

    // Start with the magic cookie
    memcpy(option, DhcpMagicCookie, sizeof(DhcpMagicCookie));
    option += sizeof(DhcpMagicCookie);

    // Append the message type option
    option[0] = DHCPOPT_DHCP_MESSAGE_TYPE;
    option[1] = 1;
    option[2] = (BYTE) msgtype;
    option += 3;

    // Append the client identifier option
    option[0] = DHCPOPT_CLIENTID;
    option[1] = (BYTE) (sizeof(CEnetAddr)+1);
    option[2] = hwtype;
    option += 3;
    memcpy(option, _ea._ab, sizeof(CEnetAddr));
    option += sizeof(CEnetAddr);

    return option;
}


void CXnIp::DhcpComputeSecsSinceStart()
{
    ICHECK(IP, UDPC|SDPC);

    DHCPTIME now = DhcpTime();

    if (_retries == 0)
        _acqtime = now;

    _secsSinceStart = now - _acqtime;
}

//
// Common prolog and epilog for SendDhcpXXX functions
//
#define SEND_DHCP_MESSAGE_PROLOG() \
        DhcpMessage* msg; \
        BYTE* buf; \
        BYTE* option; \
        CPacket * pkt = PacketAlloc(PTAG_CDhcpPacket, PKTF_TYPE_UDP|PKTF_XMIT_INSECURE|PKTF_POOLALLOC, DEFAULT_DHCP_BUFSIZE); \
        if (!pkt) return NETERR_MEMORY; \
        buf = (BYTE *)pkt->GetUdpHdr() + sizeof(CUdpHdr); \
        msg = (DhcpMessage*) buf

#define SEND_DHCP_MESSAGE_RETURN(_bcast) \
        return DhcpSendMessage(pkt, option - buf, _bcast)


NTSTATUS CXnIp::DhcpSendDiscover()
{
    ICHECK(IP, UDPC|SDPC);

    SEND_DHCP_MESSAGE_PROLOG();

    Assert(_state == STATE_INIT);

    DhcpComputeSecsSinceStart();

    // Fill in common header fields
    option = DhcpFillMessageHeader(buf, DHCPDISCOVER);

    // Fill in the parameter request list
    option = DhcpAppendParamReqList(option);

    // Fill in requested ip address and lease time option
    if (_dhcpaddr)
        option = DhcpAppendDWordOption(option, DHCPOPT_REQUESTED_CIpAddr, _dhcpaddr);

    // Fill in the lease time option
    option = DhcpAppendLeaseTimeOption(option);
    *option++ = DHCPOPT_END;

    // Emit the message
    SEND_DHCP_MESSAGE_RETURN(TRUE);
}


NTSTATUS CXnIp::DhcpSendRequest()
{
    ICHECK(IP, UDPC|SDPC);

    INT state = _state;
    SEND_DHCP_MESSAGE_PROLOG();

    Assert(state == STATE_REQUESTING ||
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
        DhcpComputeSecsSinceStart();
    }

    // Fill in common header fields
    option = DhcpFillMessageHeader(buf, DHCPREQUEST);

    // Fill in the ciaddr field and the 'requested ip addr' option
    if (state == STATE_RENEWING || state == STATE_REBINDING) {
        msg->_ciaddr = _dhcpaddr;
    } else if (_dhcpaddr) {
        // state == STATE_INIT_REBOOT || state == STATE_REQUESTING
        option = DhcpAppendDWordOption(option, DHCPOPT_REQUESTED_CIpAddr, _dhcpaddr);
    }

    // Fill in the parameter request list
    option = DhcpAppendParamReqList(option);

    // Fill in the lease time option
    option = DhcpAppendLeaseTimeOption(option);

    // Fill in the server identifier option
    if (state == STATE_REQUESTING)
        option = DhcpAppendDWordOption(option, DHCPOPT_SERVERID, _options._dhcpServer);

    *option++ = DHCPOPT_END;

    // Emit the message:
    //  unicast in RENEWING state, broadcast otherwise
    SEND_DHCP_MESSAGE_RETURN(state != STATE_RENEWING);
}

BOOL CXnIp::DhcpValidateOffer(CIpAddr yiaddr, CDhcpOptions* param)
{
    ICHECK(IP, UDPC|SDPC);

    if (    !yiaddr.IsValidUnicast()
        ||  !param->_dhcpServer.IsValidUnicast()
        ||  param->_exptime < (DHCPTIME)(8*cfgDhcpRenewMinTimeoutInSeconds))
        return FALSE;

    if (param->_dhcpmask == 0)
        param->_dhcpmask = yiaddr.DefaultMask();

    if (param->_t1time == 0 ||
        param->_t2time == 0 ||
        param->_t1time >= param->_t2time ||
        param->_t2time - param->_t1time < cfgDhcpRenewMinTimeoutInSeconds ||
        param->_t2time >= param->_exptime ||
        param->_exptime - param->_t2time < cfgDhcpRenewMinTimeoutInSeconds) {
        param->_t1time = param->_exptime / 2;
        param->_t2time = param->_exptime * 7 / 8;
    }

    return TRUE;
}


NTSTATUS CXnIp::DhcpProcessOffer(CIpAddr yiaddr, CDhcpOptions* param)
{
    ICHECK(IP, UDPC|SDPC);

    Assert(_state == STATE_INIT);

    TraceSz3(dhcp, "Received DHCPOFFER %s from %s @ time %d", yiaddr.Str(), param->_dhcpServer.Str(), DhcpTime());

    // Simply sanity check of offered parameters
    if (!DhcpValidateOffer(yiaddr, param))
        return(NETERR_PARAM);

    _options._dhcpServer = param->_dhcpServer;
    _dhcpaddr = yiaddr;

    // Send DHCPREQUEST and ignore error
    DhcpChangeState(STATE_REQUESTING);

    return(NETERR_OK);
}


void CXnIp::DhcpAddOrRemoveGateways(BOOL fDelete)
{
    ICHECK(IP, UDPC|SDPC);

    CIpAddr ipa;
    UINT    i;

    for (i = 0; i < _options._gatewayCount; i++)
    {
        ipa = _options._gateways[i];

        TraceSz2(dhcp, "%s gateway: %s", fDelete ? "Remove" : "Add", ipa.Str());

        if (fDelete)
            RouteDelete(0, 0, ipa);
        else
            RouteAdd(0, 0, ipa, RTEF_DEFAULT, (WORD)(RTE_DEFAULT_METRIC + i));
    }
}

void CXnIp::DhcpSetDefaultGateways()
{
    ICHECK(IP, USER|UDPC|SDPC);

    RaiseToDpc();
    DhcpAddOrRemoveGateways(FALSE);
}


void CXnIp::DhcpUseOptionParams(CDhcpOptions* param)
{
    ICHECK(IP, UDPC|SDPC);

    BOOL resetGateways;

    //
    // Remember the originating server address and lease info
    //
    _options._dhcpServer = param->_dhcpServer;

    if (param->_exptime == DHCPTIME_INFINITE) {
        _options._t1time =
        _options._t2time =
        _options._exptime = DHCPTIME_INFINITE;
    } else {
        _options._t1time = _acqtime + param->_t1time;
        _options._t2time = _acqtime + param->_t2time;
        _options._exptime = _acqtime + param->_exptime;
    }

    //
    // Set gateways in the IP stack
    //
    if (param->_gatewayCount == 0 || _options._gatewayCount == 0) {
        resetGateways = TRUE;
    } else {
        UINT oldcnt = _options._gatewayCount;
        UINT newcnt = param->_gatewayCount;
        UINT i, j;

        for (i=0; i < newcnt; i++) {
            for (j=0; j < oldcnt; j++)
                if (param->_gateways[i] != _options._gateways[j]) break;
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
        DhcpAddOrRemoveGateways(TRUE);

        _options._gatewayCount = param->_gatewayCount;
        memcpy(_options._gateways,
                param->_gateways,
                param->_gatewayCount * sizeof(CIpAddr));

        DhcpAddOrRemoveGateways(FALSE);
    }

    _options._dnsServerCount = param->_dnsServerCount;
    memcpy(_options._dnsServers,
           param->_dnsServers,
           param->_dnsServerCount * sizeof(CIpAddr));

#if DBG
    UINT i;

    TraceSz2(dhcp, "%d gateway%s configured", _options._gatewayCount, _options._gatewayCount == 1 ? "" : "s");
    for (i = 0; i < _options._gatewayCount; ++i)
        TraceSz1(dhcp, "  %s", _options._gateways[i].Str());
    TraceSz2(dhcp, "%d DNS server%s configured", _options._dnsServerCount, _options._dnsServerCount == 1 ? "" : "s");
    for (i = 0; i < _options._dnsServerCount; ++i)
        TraceSz1(dhcp, "  %s", _options._dnsServers[i].Str());
#endif

}

NTSTATUS CXnIp::DhcpProcessAck(CIpAddr yiaddr, CDhcpOptions* param)
{
    ICHECK(IP, UDPC|SDPC);

    TraceSz2(dhcp, "Received DHCPACK %s from %s", yiaddr.Str(), param->_dhcpServer.Str());

    Assert(_state == STATE_INIT_REBOOT ||
           _state == STATE_REQUESTING ||
           _state == STATE_RENEWING ||
           _state == STATE_REBINDING);

    // Simply sanity check of offered parameters
    if (!DhcpValidateOffer(yiaddr, param))
        return NETERR_PARAM;

    // Note: We're not checking for address conflicts
    // and just assume the offered address is valid.

    // If we're currently using a different address, give it up
    if ((_activeaddr != 0) &&
        (_activeaddr != yiaddr ||
         _activemask != param->_dhcpmask) ||
         ActiveAutonetAddr())
    {
        TraceSz1(dhcp, "Giving up old IP address %s", _activeaddr.Str());
        DhcpResetInterface();
    }

    // If we got a new address, set it down in the IP stack
    if (_activeaddr == 0)
    {
        IpSetAddress(yiaddr, param->_dhcpmask);
        _activeaddr = _dhcpaddr = yiaddr;
        _activemask = _options._dhcpmask = param->_dhcpmask;
        _flags |= FLAG_ACTIVE_DHCPADDR;
    }

    TraceSz2(dhcp, "Accepted IP address: %s from %s", yiaddr.Str(), param->_dhcpServer.Str());
    TraceSz3(dhcp, "Lease time: %d / %d / %d", param->_t1time, param->_t2time, param->_exptime);

    //
    // Set other option parameters
    //
    _initRetryCount = 0;
    DhcpUseOptionParams(param);

    // We're now in bound state.
    // Set timer to expire at T1 time.
    DhcpChangeState(STATE_BOUND);
    return NETERR_OK;
}

NTSTATUS CXnIp::DhcpProcessNak(CIpAddr dhcpServer)
{
    ICHECK(IP, UDPC|SDPC);

    TraceSz1(dhcp, "Received DHCPNAK from %s", dhcpServer.Str());

    Assert(_state == STATE_INIT_REBOOT ||
           _state == STATE_REQUESTING ||
           _state == STATE_RENEWING ||
           _state == STATE_REBINDING);

    // Barf if we got DHCPNAK from an unexpected server
    if (_state != STATE_INIT_REBOOT && dhcpServer != _options._dhcpServer)
    {
        TraceSz1(Warning, "Random DHCPNAK from %s?", dhcpServer.Str());
    }

    // If we're using an address, give it up
    if (_activeaddr) {
        DhcpResetInterface();
    }

    //
    // Go to INIT state to start over again
    // Send DHCPDISCOVER and ignore error
    //
    DhcpChangeState(STATE_INIT);
    return NETERR_OK;
}


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
        if (_result == 0) _result = *((CIpAddr*) buf)

NTSTATUS CXnIp::DhcpParseOptionParams(CDhcpOptions* param, const BYTE* buf, UINT buflen, BYTE* overload)
{
    ICHECK(IP, UDPC|SDPC);

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
            if (param->_recvMsgType == 0)
                param->_recvMsgType = *buf;
            break;

        case DHCPOPT_SERVERID:
            EXTRACT_IPADDR_OPTION(param->_dhcpServer);
            break;

        case DHCPOPT_SUBNET_MASK:
            EXTRACT_IPADDR_OPTION(param->_dhcpmask);
            break;

        case DHCPOPT_ROUTERS:
        case DHCPOPT_DNS_SERVERS:
        {
            UINT* pcount;
            CIpAddr* parray;
            UINT n;

            if (len == 0 || len % sizeof(CIpAddr) != 0) goto exit;
            if (tag == DHCPOPT_ROUTERS)
            {
                pcount = &param->_gatewayCount;
                parray = param->_gateways;
                n = MAX_DEFAULT_GATEWAYS * sizeof(CIpAddr);
            }
            else
            {
                pcount = &param->_dnsServerCount;
                parray = param->_dnsServers;
                n = MAX_DEFAULT_DNSSERVERS * sizeof(CIpAddr);
            }
        
            if (n > len) n = len;
            if (*pcount == 0) {
                *pcount = n / sizeof(CIpAddr);
                memcpy(parray, buf, n);
            }
            break;
        }

        case DHCPOPT_IPADDR_LEASE_TIME:
            EXTRACT_DWORD_OPTION(param->_exptime);
            break;

        case DHCPOPT_T1_INTERVAL:
            EXTRACT_DWORD_OPTION(param->_t1time);
            break;

        case DHCPOPT_T2_INTERVAL:
            EXTRACT_DWORD_OPTION(param->_t2time);
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
    TraceSz(Warning, "Invalid option data");
    return NETERR_PARAM;
}

void CXnIp::DhcpSelectAutonetAddr()
{
    ICHECK(IP, UDPC|SDPC);

    CIpAddr addr;

    Assert(_state == STATE_SELECT_AUTOADDR);

    if (++_initRetryCount > cfgAutoIpMaxAttempts) {
        //
        // We tried too many autonet addresses without success.
        // Just give up.
        //
        _initRetryCount = 0;
        DhcpChangeState(STATE_NONE);

        // Signal that Xnet initialization was completed abnormally
        _flags = (_flags & ~FLAG_ACTIVE_ADDRMASK) | FLAG_ACTIVE_NOADDR;
        TraceSz(Warning, "Failed to pick an autonet address.");
        return;
    }

    // Generate a random autonet address
    addr = AUTONET_ADDRBASE + RandScaled(AUTONET_ADDRRANGE);
    _autonetaddr = addr = HTONL(addr);

    TraceSz1(dhcp, "Trying autonet address: %s", addr.Str());

    EnetXmitArp(addr);
    _retries = 0;
    DhcpComputeTimeout();
}

#if DBG

DefineTag(dhcpDump, 0);

//
// Whether to dump incoming and outgoing DHCP messages
//
VOID DhcpDumpByteArray(const BYTE* buf, UINT buflen)

/*++

Routine Description:

    Dump a byte array in hexdecimal format

Arguments:

    buf - Points to the data buffer
    buflen - Buffer length

Return Value:

    NONE

--*/

{
    for ( ; buflen--; buf++) {
        TraceSz1(dhcpDump, "+%02x+", *buf);
    }
}


VOID DhcpDumpOption(IN const BYTE* buf, IN UINT buflen, OUT BYTE* overload)

/*++

Routine Description:

    Dump DHCP options

Arguments:

    buf - Points to the option data buffer
    buflen - Data buffer length
    overload - Returns the option overload flags

Return Value:

    NONE

--*/

{
    static PCSTR dhcpMessageTypeStrs[] = {
        "***",
        "DHCPDISCOVER",
        "DHCPOFFER",
        "DHCPREQUEST",
        "DHCPDECLINE",
        "DHCPACK",
        "DHCPNAK",
        "DHCPRELEASE",
        "DHCPINFORM"
    };

    UINT tag, len, val;

    while (buflen && *buf != DHCPOPT_END) {
        // Special case for DHCPOPT_PAD - single byte
        if (*buf == DHCPOPT_PAD) {
            TraceSz(dhcpDump, "+    PAD");
            buflen--, buf++;
            continue;
        }

        // Check option length
        if (buflen < 2 || buflen-2 < (len = buf[1])) goto badopt;

        switch (tag = buf[0]) {
        case DHCPOPT_DHCP_MESSAGE_TYPE:
            if (len != 1) goto badopt;
            val = buf[2];
            if (val > 0 && val < dimensionof(dhcpMessageTypeStrs)) {
                TraceSz1(dhcpDump, "+    %s+", dhcpMessageTypeStrs[val]);
            } else {
                TraceSz1(dhcpDump, "+    Unknown DHCP message type: %d+", val);
            }
            break;

        case DHCPOPT_FIELD_OVERLOAD:
            if (len != 1) goto badopt;
            TraceSz1(dhcpDump, "+    OVERLOAD: %d+", buf[2]);
            if (overload)
                *overload = buf[2];
            else
                TraceSz(dhcpDump, "+ !!!+");
            break;

        case DHCPOPT_REQUESTED_CIpAddr:
        case DHCPOPT_SERVERID:
            if (len != 4) goto badopt;
            TraceSz5(dhcpDump, "+    %s %d.%d.%d.%d+",
                     (tag == DHCPOPT_SERVERID) ? "SERVERID" : "REQUEST IP ADDR",
                     buf[2], buf[3], buf[4], buf[5]);
            break;

        default:
            TraceSz1(dhcpDump, "+    %d - +", tag);
            DhcpDumpByteArray(buf+2, len);
            break;
        }

        TraceSz(dhcpDump, "+ ");
        buf += len+2;
        buflen -= len+2;
    }

    if (buflen == 0) {
        TraceSz(dhcpDump, "!!! Missing 'end' option");
    } else {
        do {
            buflen--, buf++;
        } while (buflen && *buf == 0);

        if (buflen != 0)
        {
            TraceSz(dhcpDump, "!!! Extra data after 'end' option");
        }
    }
    return;

badopt:
    TraceSz(dhcpDump, "!!! Bad DHCP option data");
}


void DhcpDumpMessage(DhcpMessage* msg, UINT msglen)

/*++

Routine Description:

    Dump the content of a DHCP message

Arguments:

    msg - Points to the DHCP message
    msglen - Message length

Return Value:

    NONE

--*/

{
    const BYTE* option;
    BYTE overload = 0;

    if (!Tag(dhcpDump))
        return;

    //
    // Sanity check
    //
    if (msglen < DHCPHDRLEN) {
        TraceSz1(dhcpDump, "!!! DHCP message too small: %d bytes", msglen);
        return;
    }

    //
    // Dump out fixed header information
    //
    TraceSz(dhcpDump, "DHCP message: ");
    if (msg->_op == BOOTREQUEST)
        TraceSz(dhcpDump, "BOOTREQUEST");
    else if (msg->_op == BOOTREPLY)
        TraceSz(dhcpDump, "BOOTREPLY");
    else
        TraceSz1(dhcpDump, "%d", msg->_op);

    TraceSz1(dhcpDump, "  htype: %d", msg->_htype);
    if (msg->_hlen > sizeof(msg->_chaddr)) {
        TraceSz1(dhcpDump, "!!! Invalid hardware address length: %d\n", msg->_hlen);
    } else if (msg->_hlen) {
        TraceSz(dhcpDump, "  chaddr: +");
        DhcpDumpByteArray(msg->_chaddr, msg->_hlen);
        TraceSz(dhcpDump, "+ ");
    }

    TraceSz1(dhcpDump, "  hops: %d", msg->_hops);
    TraceSz1(dhcpDump, "  xid: 0x%08x", NTOHL(msg->_xid));
    TraceSz1(dhcpDump, "  secs: %d", NTOHS(msg->_secs));
    TraceSz1(dhcpDump, "  flags: 0x%04x", NTOHS(msg->_flags));
    TraceSz1(dhcpDump, "  ciaddr: %s", msg->_ciaddr.Str());
    TraceSz1(dhcpDump, "  yiaddr: %s", msg->_yiaddr.Str());
    TraceSz1(dhcpDump, "  siaddr: %s", msg->_siaddr.Str());
    TraceSz1(dhcpDump, "  giaddr: %s", msg->_giaddr.Str());

    //
    // Dump options
    //
    option = msg->_options;
    msglen -= DHCPHDRLEN;

    if (msglen < sizeof(DhcpMagicCookie) ||
        memcmp(option, DhcpMagicCookie, sizeof(DhcpMagicCookie)) != 0) {
        TraceSz(dhcpDump, "!!! Invalid DHCP magic cookie");
        return;
    }
    msglen -= sizeof(DhcpMagicCookie);
    option += sizeof(DhcpMagicCookie);

    TraceSz(dhcpDump, "  options:");
    DhcpDumpOption(option, msglen, &overload);

    if (overload & 1) {
        TraceSz(dhcpDump, "  overload options (file):");
        DhcpDumpOption(msg->_file, sizeof(msg->_file), NULL);
    }

    if (overload & 2) {
        TraceSz(dhcpDump, "  overload options (sname):");
        DhcpDumpOption(msg->_sname, sizeof(msg->_sname), NULL);
    }
}

#endif // DBG

void CXnIp::IpRecvArp(CEnetAddr * pea)
{
    ICHECK(IP, UDPC|SDPC);
    DhcpNotifyAddressConflict();
}

#endif // XNET_FEATURE_DHCP
