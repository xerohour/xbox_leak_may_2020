/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    dnslib.h

Abstract:

    DNS client implementation related declarations

Revision History:

    07/11/2000 davidx
        Created it.

--*/

#ifndef _DNSLIB_H
#define _DNSLIB_H

//
// Internal DNS domain name representation
//  We represent the DNS domain name as an array of bytes.
//  1st byte is the total length of the name (not including the first two bytes)
//  2nd byte is the number of labels.
//  followed by the normal domain name data (as in RFC1035).
//  e.g.
//      19
//      3
//      3 www
//      9 microsoft
//      3 com
//      0
//
typedef BYTE DnsName;
DnsName* CreateDnsNameFromString(const CHAR* buf);
INLINE VOID FreeDnsName(DnsName* dnsname) {
    SysFree(dnsname);
}

#define SetDnsNameLen(_dnsname, _len)       ((_dnsname)[0] = (BYTE) (_len))
#define SetDnsNameLabels(_dnsname, _labels) ((_dnsname)[1] = (BYTE) (_labels))
#define GetDnsNameLen(_dnsname)             (_dnsname)[0]
#define GetDnsNameLabels(_dnsname)          (_dnsname)[1]
#define GetDnsNameData(_dnsname)            ((_dnsname) + 2)

#define SkipDnsNameLabel(_p) ((_p) + 1 + *(_p))

INLINE const BYTE* SkipDnsNameLabels(const BYTE* p, UINT n) {
    while (n--) {
        p = SkipDnsNameLabel(p);
    }
    return p;
}

// Make a copy of a DNS name
INLINE DnsName* CopyDnsName(const DnsName* dnsname) {
    UINT len = (UINT) GetDnsNameLen(dnsname) + 2;
    DnsName* newname = (DnsName*) SysAlloc(len, PTAG_DNS);
    if (newname) {
        CopyMem(newname, dnsname, len);
    }
    return newname;
}


//
// Cached DNS server entry
//
typedef struct _DnsServerCacheEntry {
    LIST_ENTRY links;
        // doubly-linked list pointers
        // must be the first field!

    UINT expires;
        // expiration time (in 0.5 seconds)

    IPADDR serverAddr;
        // DNS server address

    DnsName* domain;
        // name of the domain this server can handle

    UINT priority;
        // server precedence
        // lower value is more preferred
} DnsServerCacheEntry;


//
// Cached DNS lookup entry
//  we artifically limit the maximum number of aliases to 4
//
#define MAXALIASCNT 4

typedef struct _DnsCacheEntry {
    LIST_ENTRY links;
        // doubly-linked list pointers
        // must be the first field!
    
    LONG refcount;
        // reference count

    UINT expires;
        // expiration time (in 0.5 seconds)

    NTSTATUS status;
        // entry status

    WORD type;
        // entry type:
        //  RRTYPE_A
        //  RRTYPE_PTR

    BYTE namecnt;
        // number of names

    BYTE addrcnt;
        // number of addresses

    IPADDR addrs[MAXALIASCNT];
        // list of addresses

    DnsName* names[MAXALIASCNT];
        // list of names

    KEVENT event;
        // wait event object
} DnsCacheEntry;

//
// Map relative TTL value in seconds to
// absolute expiration time in ticks
//
#define DnsCacheExpires(_ttl) \
        (TcpTickCount + (_ttl) * SLOW_TCP_TIMER_FREQ)


//
// Data structure for storing parsed information
// from a DNS response message
//
typedef struct _DnsRR DnsRR;
typedef struct _DnsRR {
    DnsRR* next;            // points to the next RR
    DnsName* rrname;        // record name
    WORD type;              // record type
    WORD unused;
    UINT ttl;               // TTL
    union {
        IPADDR addr;        // IP address for RRTYPE_A record
        DnsName* dataname;  // data for other records:
                            //  RRTYPE_NS
                            //  RRTYPE_CNAME
                            //  RRTYPE_SOA
                            //  RRTYPE_PTR
    };
};

typedef struct _DnsResp {
    BYTE* start;            // points to where the DNS response message starts
    BYTE* end;              // points to after the DNS message
    DnsRR* answers;         // information in the answers section
    DnsRR* nservers;        // ... the nservers section
    DnsRR* extras;          // ... the extras section

    // temporary name buffer
    BYTE namebuf[DNS_MAXNAMELEN+1];
} DnsResp;

#endif // !_DNSLIB_H

