/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    getxbyy.c

Abstract:

    Implementation of the following Winsock APIs:
        gethostbyaddr
        gethostbyname
        gethostname
        getservbyname
        getservbyport
        getprotobynumber
        getprotobyname
        XnetGetIpAddress

Revision History:

    03/30/2000 davidx
        Created it.

--*/

#include "precomp.h"

PRIVATE INT
SockGetLocalHostName(
    OUT CHAR* buf,
    IN UINT bufsize
    )

/*++

Routine Description:

    Return the name of the local host

Arguments:

    buf - Output buffer
    bufsize - Output buffer size

Return Value:

    Winsock error code

Note:

    For now, our local hostname will just be our Ethernet
    address in hexdecimal format.

--*/

{
    static const CHAR hexDigits[] = "0123456789abcdef";
    IfInfo* ifp = LanIfp;
    BYTE* hwaddr;
    UINT count;

    if (!ifp) return WSAENETDOWN;
    hwaddr = ifp->hwaddr;
    count = ifp->hwaddrlen;
    if (bufsize < 2 + count * 2) return WSAENOBUFS;

    *buf++ = '_';
    while (count--) {
        *buf++ = hexDigits[*hwaddr >> 4];
        *buf++ = hexDigits[*hwaddr & 15];
        hwaddr++;
    }

    *buf = 0;
    return NO_ERROR;
}


PRIVATE BOOL
SockIsLocalHostName(
    IN const CHAR* name,
    OUT CHAR* buf,
    IN UINT bufsize
    )

/*++

Routine Description:

    Check if the specified hostname matches the local hostname

Arguments:

    name - Specified the hostname in question
    buf - Output buffer for returning the local hostname
    bufsize - Output buffer size

Return Value:

    TRUE if the specified hostname is the same as the local hostname
    FALSE otherwise

--*/

{
    return (name[0] == '_' &&
            SockGetLocalHostName(buf, bufsize) == NO_ERROR &&
            strcmp(name, buf) == 0);
}


//
// hostent structure with 1 host name and 1 host address
//
typedef struct _HostEnt1 {
    struct hostent;
    CHAR* alias_array[1];
    CHAR* addr_array[2];
    IPADDR ipaddr;
    CHAR namebuf[1];
} HostEnt1;

PRIVATE INT
SockReturnLocalHostent(
    WinsockThreadbuf* hostentbuf,
    CHAR* localhost
    )

/*++

Routine Description:

    Return the local host address

Arguments:

    hostentbuf - Points to the per-thread temporary data buffer
    localhost - Local hostname

Return Value:

    Winsock error code

--*/

{
    UINT namesize, size;
    HostEnt1* host;

    //
    // Get the best available IP address
    //
    IPADDR ipaddr;
    if (IpGetBestAddress(&ipaddr) == XNET_ADDR_NONE)
        return WSAENETDOWN;

    //
    // Assemble the return information into a HOSTENT structure
    //
    namesize = SizeofSTR(localhost);
    size = offsetof(HostEnt1, namebuf) + namesize;
    if (!SockAllocThreadbuf(hostentbuf, size))
        return WSAENOBUFS;

    host = (HostEnt1*) hostentbuf->data;
    host->h_name = host->namebuf;
    host->h_aliases = host->alias_array;
    host->h_addrtype = AF_INET;
    host->h_length = IPADDRLEN;
    host->h_addr_list = host->addr_array;
    host->alias_array[0] = NULL;
    host->addr_array[0] = (CHAR*) &host->ipaddr;
    host->addr_array[1] = NULL;
    host->ipaddr = ipaddr;
    CopyMem(host->namebuf, localhost, namesize);

    return NO_ERROR;
}


//
// Callback function passed to DnsLookupBy___.  It's called to
// allocate memory buffer to store the hostent information.
//
PRIVATE struct hostent*
SockAllocHostentCallback(
    UINT size,
    VOID* param
    )
{
    return (struct hostent*) SockAllocThreadbuf((WinsockThreadbuf*) param, size);
}


struct hostent* WSAAPI
gethostbyaddr(
    IN const char* addr,
    IN int len,
    IN int type
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    WinsockThreadbuf* hostentbuf;
    IPADDR ipaddr;
    WinsockApiProlog_(gethostbyaddr, NULL);

    WinsockApiParamCheck_(
        addr != NULL &&
        len >= (INT) sizeof(struct in_addr) &&
        type == AF_INET);

    ipaddr = ((struct in_addr*) addr)->s_addr;
    hostentbuf = &tlsData->hostentbuf;
    err = DnsLookupByAddr(ipaddr, SockAllocHostentCallback, hostentbuf);
    MapNtStatusToWinsockError_(err);

    WinsockApiCheckError_(NULL);
    return (struct hostent*) hostentbuf->data;
}


struct hostent* WSAAPI
gethostbyname(
    IN const char* name
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    WinsockThreadbuf* hostentbuf;
    CHAR localhost[32];
    WinsockApiProlog_(gethostbyname, NULL);

    WinsockApiParamCheck_(name != NULL);

    hostentbuf = &tlsData->hostentbuf;
    if (SockIsLocalHostName(name, localhost, 32)) {
        err = SockReturnLocalHostent(hostentbuf, localhost);
    } else {
        err = DnsLookupByName(name, SockAllocHostentCallback, hostentbuf);
        MapNtStatusToWinsockError_(err);
    }

    WinsockApiCheckError_(NULL);
    return (struct hostent*) hostentbuf->data;
}


int WSAAPI
gethostname(
    OUT char* name,
    IN int namelen
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    WinsockApiProlog_(gethostname, SOCKET_ERROR);
    WinsockApiParamCheck_(name != NULL);

    if (namelen <= 0) {
        err = WSAEFAULT;
    } else {
        err = SockGetLocalHostName(name, namelen);
        if (err == WSAENOBUFS)
            err = WSAEFAULT;
    }

    WinsockApiCheckError_(SOCKET_ERROR);
    return NO_ERROR;
}



//
// !!! _XBOX_NOTE
//  Hard-coded services database
//

typedef struct {
    WORD port;
    WORD type;
    const CHAR* names;
} ServiceEntry;

#define SVCTYPE_TCP    1
#define SVCTYPE_UDP    2
#define SVCTYPE_ALL   (SVCTYPE_TCP|SVCTYPE_UDP)
#define SVCENTRYTCP(_port, _names) { HTONS(_port), SVCTYPE_TCP, _names }
#define SVCENTRYUDP(_port, _names) { HTONS(_port), SVCTYPE_UDP, _names }
#define SVCENTRYALL(_port, _names) { HTONS(_port), SVCTYPE_ALL, _names }
    
PRIVATE const ServiceEntry Services[] = {
    SVCENTRYTCP(20, "ftp-data\0"),
    SVCENTRYTCP(21, "ftp\0"),
    SVCENTRYTCP(23, "telnet\0"),
    SVCENTRYTCP(25, "smtp\0mail\0"),
    SVCENTRYALL(53, "domain\0"),
    SVCENTRYUDP(67, "bootps\0dhcps\0"),
    SVCENTRYUDP(68, "bootpc\0dhcpc\0"),
    SVCENTRYUDP(69, "tftp\0"),
    SVCENTRYTCP(70, "gopher\0"),
    SVCENTRYTCP(80, "http\0www\0www-http\0"),
    SVCENTRYTCP(110, "pop3\0"),
    SVCENTRYTCP(143, "imap\0imap4\0"),
    SVCENTRYUDP(161, "snmp\0"),
    SVCENTRYTCP(194, "irc\0"),
    SVCENTRYALL(443, "https\0MCom\0"),
    { 0, }
};


//
// Structure for storing the SERVENT information
// we return from the getservbyXxx APIs.
//

struct myservent {
    struct servent;
    CHAR* aliases[3];
};

PRIVATE struct servent*
SockComposeServentBuf(
    const ServiceEntry* service,
    WORD type
    )

/*++

Routine Description:

    Generate a SERVENT structure based on the specified service database entry

Arguments:

    service - Pointer to the service database entry
    type - Determine whether to use TCP or UDP protocol

Return Value:

    Points to the resulting SERVENT structure
    NULL if there is an error

--*/

{
    struct myservent* buf;
    INT bufsize, namelen, protolen, alias1len, alias2len;
    const CHAR* names = service->names;
    const CHAR* proto = (type & SVCTYPE_TCP) ? "tcp" : "udp";

    WinsockApiProlog_(getservby_, NULL);

    // Figure out how large a buffer we need

    protolen = SizeofSTR(proto);
    namelen = SizeofSTR(names);
    names += namelen;
    alias1len = *names ? SizeofSTR(names) : 0;
    names += alias1len;
    alias2len = *names ? SizeofSTR(names) : 0;

    bufsize = ROUNDUP8(sizeof(*buf) + protolen + namelen + alias1len + alias2len);

    // Allocate per-thread temporary buffer
    buf = (struct myservent*) SockAllocThreadbuf(&tlsData->serventbuf, bufsize);
    if (buf == NULL) {
        WinsockApiReturnError_(WSAENOBUFS, NULL);
    }

    // Fill out the SERVENT structure
    buf->s_port = service->port;
    buf->s_proto = (CHAR*) (buf+1);
    buf->s_name = buf->s_proto + protolen;
    buf->s_aliases = buf->aliases;
    buf->aliases[0] = buf->aliases[1] = buf->aliases[2] = NULL;

    if (alias1len) {
        buf->aliases[0] = buf->s_name + namelen;
        if (alias2len) buf->aliases[1] = buf->aliases[0] + alias1len;
    }

    CopyMem(buf->s_proto, proto, protolen);
    CopyMem(buf->s_name, service->names, namelen+alias1len+alias2len);

    return (struct servent*) buf;
}


//
// Map protocol name string to our internal service type flag
//
INLINE WORD
SockParseProtocolType(
    const CHAR* proto
    )
{
    return
     (WORD) ((proto == NULL) ? SVCTYPE_ALL :
             (_stricmp(proto, "tcp") == 0) ? SVCTYPE_TCP :
             (_stricmp(proto, "udp") == 0) ? SVCTYPE_UDP : 0);
}

struct servent* WSAAPI
getservbyport(
    IN int port,
    IN const char* proto
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    const ServiceEntry* service = Services;
    WORD type;

    WinsockApiPrologLight_(getservbyport);

    type = SockParseProtocolType(proto);
    while (service->type) {
        if ((port == service->port) && (type & service->type)) {
            return SockComposeServentBuf(service, type);
        }
        service++;
    }

    WARNING_("getservbyport: not found");
    SetLastError(WSANO_DATA);
    return NULL;
}


struct servent* WSAAPI
getservbyname(
    IN const char* name,
    IN const char* proto
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    const ServiceEntry* service = Services;
    WORD type;

    WinsockApiPrologLight_(getservbyname);
    WinsockApiParamCheck_(name != NULL);

    type = SockParseProtocolType(proto);
    while (service->type) {
        if ((type & service->type) && _stricmp(name, service->names) == 0) {
            return SockComposeServentBuf(service, type);
        }
        service++;
    }

    WARNING_("getservbyname: not found");
    SetLastError(WSANO_DATA);
    return NULL;
}



//
// !!! _XBOX_NOTE:
//  Hard-coded protocol database
//

typedef struct {
    const CHAR* name;
    const CHAR* alias;
    INT protocol;
} ProtocolEntry;

PRIVATE const ProtocolEntry Protocols[] = {
    { "ip", "IP", 0 },
    { "icmp", "ICMP", IPPROTOCOL_ICMP },
    { "igmp", "IGMP", IPPROTOCOL_IGMP },
    { "tcp", "TCP", IPPROTOCOL_TCP },
    { "udp", "UDP", IPPROTOCOL_UDP },
    { NULL, }
};

//
// Structure for storing the PROTOENT information
// we return from the getprotobyX APIs.
//

struct myprotoent {
    struct protoent;
    CHAR* aliases[2];
};

PRIVATE struct protoent*
SockComposeProtoentBuf(
    const ProtocolEntry* proto
    )

/*++

Routine Description:

    Generate a PROTOENT structure based on the specified protocol database entry

Arguments:

    proto - Pointer to the protocol database entry

Return Value:

    Points to the resulting PROTOENT structure
    NULL if there is an error

--*/

{
    struct myprotoent* buf;
    INT bufsize, namelen, aliaslen;

    WinsockApiProlog_(getprotoby_, NULL);

    // Figure out how big a buffer we need
    namelen = SizeofSTR(proto->name);
    aliaslen = SizeofSTR(proto->alias);
    bufsize = ROUNDUP8(sizeof(*buf) + namelen + aliaslen);

    // Allocate per-thread temporary buffer
    buf = (struct myprotoent*) SockAllocThreadbuf(&tlsData->protoentbuf, bufsize); 
    if (!buf) {
        WinsockApiReturnError_(WSAENOBUFS, NULL);
    }

    buf->p_proto = (short) proto->protocol;
    buf->p_name = (CHAR*) (buf+1);
    buf->p_aliases = buf->aliases;
    buf->aliases[0] = buf->p_name + namelen;
    buf->aliases[1] = NULL;

    CopyMem(buf->p_name, proto->name, namelen);
    CopyMem(buf->aliases[0], proto->alias, aliaslen);

    return (struct protoent*) buf;
}


struct protoent* WSAAPI
getprotobynumber(
    IN int number
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    const ProtocolEntry* proto = Protocols;
    WinsockApiPrologLight_(getprotobynumber);

    while (proto->name) {
        if (proto->protocol == number)
            return SockComposeProtoentBuf(proto);
        proto++;
    }

    WARNING_("getprotobynumber: not found");
    SetLastError(WSANO_DATA);
    return NULL;
}


struct protoent* WSAAPI
getprotobyname(
    IN const char* name
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    const ProtocolEntry* proto = Protocols;

    WinsockApiPrologLight_(getprotobyname);
    WinsockApiParamCheck_(name != NULL);

    while (proto->name) {
        if (_stricmp(proto->name, name) == 0)
            return SockComposeProtoentBuf(proto);
        proto++;
    }

    WARNING_("getprotobyname: not found");
    SetLastError(WSANO_DATA);
    return NULL;
}


DWORD WSAAPI
XnetGetIpAddress(
    OUT struct in_addr* addr
    )

/*++

Routine Description:

    Get the active IP address that's currently in use

Arguments:

    addr - Return the active IP address

Return Value:

    Flags indicating how the address was obtained

--*/

{
    IPADDR ipaddr;
    DWORD flags;
    WinsockApiPrologLight_(XnetGetIpAddress);
    WinsockApiParamCheck_(addr != NULL);

    flags = IpGetBestAddress(&ipaddr);
    addr->s_addr = ipaddr;
    return flags;
}

