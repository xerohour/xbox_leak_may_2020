/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    dhcp.h

Abstract:

    DHCP and Autonet protocol related declarations

Revision History:

    04/21/2000 davidx
        Created it.

Notes:

    Please refer to RFC2131, RFC2132, and RFC951. Also, the internet draft
    http://search.ietf.org/internet-drafts/draft-ietf-dhc-ipv4-autoconfig-05.txt.

--*/

#ifndef _DHCP_H
#define _DHCP_H

//
// UDP port numbers used by DHCP
//
#define DHCP_SERVER_PORT HTONS(67)
#define DHCP_CLIENT_PORT HTONS(68)

//
// DHCP message format
// NOTE: default the options field to 64 bytes
// which is the size of BOOTP vendor-specified area
//

typedef struct _DhcpMessage {
    BYTE op;                // message type
    BYTE htype;             // hareware address type
    BYTE hlen;              // hardware address length
    BYTE hops;              // relay hops
    DWORD xid;              // transaction ID
    WORD secs;              // seconds since address acquisition process began
    WORD flags;             // flags
    IPADDR ciaddr;          // client IP address
    IPADDR yiaddr;          // "your" (client) IP address
    IPADDR siaddr;          // server IP address
    IPADDR giaddr;          // relay agent IP address
    BYTE chaddr[16];        // client hardware address
    BYTE sname[64];         // optional server hostname
    BYTE file[128];         // boot filename
    BYTE options[64];       // optional parameters (variable length)
} DhcpMessage;

//
// Our default DHCP packet buffer is 576 bytes
//
#define BOOTP_MESSAGE_SIZE sizeof(DhcpMessage)
#define DEFAULT_DHCP_BUFSIZE 576
#define DHCPHDRLEN offsetof(DhcpMessage, options)

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

#define DHCPOPT_REQUESTED_IPADDR        50
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
#define DHCPCOOKIELEN 4
extern const BYTE DhcpMagicCookie[DHCPCOOKIELEN];

//
// DHCP time value (in seconds)
//
// NOTE: We use 32-bit unsigned interface to represent
// the number of seconds ellapsed since the start of 1/1/2000.
// This should last until year 2136 before overflowing.
//
typedef DWORD DHCPTIME;

#define DHCPTIME_INFINITE 0xffffffff

//
// Autonet address range: 169.254/16
//  don't use the first 256 and the last 256 addresses
//
#define AUTONET_ADDRMASK    0xffff0000
#define AUTONET_ADDRBASE    0xa9fe0100
#define AUTONET_ADDRRANGE   0x0000fcff

#endif // !_DHCP_H

