/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    dhcplib.h

Abstract:

    DHCP client implementation for XBox

Revision History:

    04/21/2000 davidx
        Created it.

--*/

#ifndef _DHCPLIB_H
#define _DHCPLIB_H

//
// Information we extract from DHCP options
//
typedef struct _DhcpOptionParam {
    INT recvMsgType;        // type message received
    IPADDR dhcpmask;        // subnet mask
    IPADDR dhcpServer;      // dhcp server address
    DHCPTIME t1time;        // when to enter renewing state
    DHCPTIME t2time;        // when to enter rebinding state
    DHCPTIME exptime;       // lease expiration time
    UINT gatewayCount;      // number of gateways
    IPADDR gateways[MAX_DEFAULT_GATEWAYS];
    UINT dnsServerCount;    // number of DNS servers
    IPADDR dnsServers[MAX_DEFAULT_DNSSERVERS];
    CHAR domainName[256];   // domain name
} DhcpOptionParam;


//
// Information we maintain for each interface
//
typedef struct _DhcpInfo {
    IfInfo* ifp;            // Pointer back to the interface
    KEVENT addrEvent;       // address acquisition event
    INT state;              // current state of the interface
    INT flags;              // misc. flags (see constants below)
    IPADDR activeaddr;      // active IP address and mask
    IPADDR activemask;      //
    IPADDR dhcpaddr;        // last DHCP address
    IPADDR autonetaddr;     // last autonet address
    DHCPTIME acqtime;       // when lease acquisition started
    DWORD xid;              // XID for the next outgoing message
    UINT retries;           // number of retries for the current message
    UINT initRetryCount;    // how many retries into INIT state
    UINT secsSinceStart;    // secs since the address-req process started
    UINT timer;             // timer (unit = second)
    union {                 // DHCP option paramters
        // We're using this odd-looking syntax construct so that
        // we can reference the fields in DhcpOptionParam directly.
        DhcpOptionParam;
        DhcpOptionParam optionParam;
    };
} DhcpInfo;


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
    STATE_SELECT_AUTOADDR   // checking autonet address conflicts
};

//
// Constants for the DhcpInfo.flags field:
//
#define FLAG_RELEASE_ON_REBOOT  0x0001
#define FLAG_SEND_DHCPINFORM    0x0002
#define FLAG_CREATED_BY_DEBUGGER 0x0004

#define FLAG_ACTIVE_DHCPADDR    0x00010000
#define FLAG_ACTIVE_AUTONETADDR 0x00020000
#define FLAG_ACTIVE_ADDRMASK    (FLAG_ACTIVE_DHCPADDR | FLAG_ACTIVE_AUTONETADDR)

INLINE BOOL ActiveDhcpAddr(DhcpInfo* dhcp) {
    return (dhcp->flags & FLAG_ACTIVE_DHCPADDR);
}

INLINE BOOL ActiveAutonetAddr(DhcpInfo* dhcp) {
    return (dhcp->flags & FLAG_ACTIVE_AUTONETADDR);
}

//
// Dump DHCP message in the debugger
//

#if DBG
VOID DhcpDumpMessage(const DhcpMessage* msg, UINT msglen);
#else
#define DhcpDumpMessage(msg, msglen)
#endif

//
// Map interface hardware type to DHCP hardware type
//
INLINE BYTE DhcpMapHwtype(WORD iftype) {
    return (BYTE) ((iftype == IFTYPE_ETHERNET) ?
                    HWTYPE_10MB_ETHERNET :
                    HWTYPE_PPP);
}

//
// Check whether there is a conflict with an IP address
//
INLINE VOID DhcpCheckAddressConflict(IfInfo* ifp, IPADDR ipaddr) {
    ifp->Ioctl(ifp, IFCTL_CHECK_ADDR_CONFLICT, &ipaddr, sizeof(ipaddr), NULL, NULL);
}

//
// Signal address acquisition event
//
INLINE VOID DhcpSignalAddrEvent(DhcpInfo* dhcp) {
    KeSetEvent(&dhcp->addrEvent, IO_NO_INCREMENT, FALSE);
}

#endif // !_DHCPLIB_H

