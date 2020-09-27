/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    dns.h

Abstract:

    DNS protocol related declarations

Revision History:

    07/12/2000 davidx
        Created it.

--*/

#ifndef _DNS_H
#define _DNS_H

//
// DNS name format:
//  1 byte - label length (N)
//  N bytes - label
//  ... more labels
//  0 - root label
//
// Compression:
//  Normally, the two high bits of the label length byte is 00.
//  But if the two high bits are 11, then the remaining 6 bits
//  and the next byte form an offset. The offset is relative to
//  the beginning of the DNS message. The domain name continues
//  with data from that offset.
//

//
// Resource record (RR) format
//  NAME - see above
//  2 bytes - type
//  2 bytes - class
//  4 bytes - TTL (time-to-live)
//  2 bytes - RDATA length
//  RDATA - variable length
//

//
// RDATA format for well-known types
//
// A
//  4 bytes - internet address
//
// PTR
//  PTRDNAME: <name> - pointer to some location in the domain name space
//
// CNAME
//  CNAME: <name> - canonical name for the record owner
//
// MX
//  2 bytes - preference value
//  EXCHANGE: <name> - domain name for the mail exchange host
//
// SOA
//  MNAME: <name> - name server that's the primary data source for this zone
//  RNAME: <name> - mailbox of the zone administrator
//  4 bytes - version number of the original copy of the zone
//  4 bytes - time interval before the zone should be refreshed
//  4 bytes - wait time before a failed refresh should be retried
//  4 bytes - max time interval before the zone is no longer authoritative
//  4 bytes - minimum TTL that should be used for any RR in this zone
//

//
// RR type constants
//
#define RRTYPE_A        1   // a host address
#define RRTYPE_NS       2   // an authoritative name server
#define RRTYPE_MD       3   // a mail destination (Obsolete - use MX)
#define RRTYPE_MF       4   // a mail forwarder (Obsolete - use MX)
#define RRTYPE_CNAME    5   // the canonical name for an alias
#define RRTYPE_SOA      6   // marks the start of a zone of authority
#define RRTYPE_MB       7   // a mailbox domain name (EXPERIMENTAL)
#define RRTYPE_MG       8   // a mail group member (EXPERIMENTAL)
#define RRTYPE_MR       9   // a mail rename domain name (EXPERIMENTAL)
#define RRTYPE_NULL     10  // a null RR (EXPERIMENTAL)
#define RRTYPE_WKS      11  // a well known service description
#define RRTYPE_PTR      12  // a domain name pointer
#define RRTYPE_HINFO    13  // host information
#define RRTYPE_MINFO    14  // mailbox or mail list information
#define RRTYPE_MX       15  // mail exchange
#define RRTYPE_TXT      16  // text strings

// Additional query type constants
#define QTYPE_AXFR      252 // a transfer of an entire zone
#define QTYPE_MAILB     253 // mailbox-related records (MB, MG or MR)
#define QTYPE_MAILA     254 // mail agent RRs (Obsolete - see MX)
#define QTYPE_ALL       255 // all records

//
// RR class constants
//
#define RRCLASS_IN      1   // the Internet
#define QCLASS_ANY      255 // any class

//
// Limits
//
#define DNS_MAXLABELLEN 63
#define DNS_MAXNAMELEN  255
#define DNS_MAXMSGLEN   512

//
// DNS message format
//  header section
//  question section
//  answer section
//  authority section
//  additional section
//

typedef struct _DnsHeader {
    WORD id;
    WORD code;
    WORD questions;
    WORD answers;
    WORD nservers;
    WORD extras;
} DnsHeader;

#define DNSHDRLEN sizeof(DnsHeader)

//
// Bit assignment for the DnsHeader.code field
//  bit 15: whether the message is a query or a response
//  bit 14-11: specifies the kind of query
//      0 - standard query
//      1 - inverse query
//      2 - server status request
//      others - reserved
//  bit 10: whether the responding name server is an authority
//      for the domain name in question
//  bit 9: whether this message was truncated
//  bit 8: whether recursion is desired by the requester
//  bit 7: whether recursion is available on the server
//  bit 6-4: reserved
//  bit 3-0: response code (see below)
//
#define DNSFLAG_QUERY       (0 << 15)
#define DNSFLAG_RESPONSE    BIT(15)
#define DNSFLAG_AA          BIT(10)
#define DNSFLAG_TC          BIT(9)
#define DNSFLAG_RD          BIT(8)
#define DNSFLAG_RA          BIT(7)

#define DNSGETOPCODE(x)     (((x) >> 11) & 0xf)
#define DNSOPCODE_STDQUERY  0

#define DNSGETRCODE(x)      ((x) & 0xf)
#define RCODE_OK            0
#define RCODE_FORMAT_ERROR  1
#define RCODE_SERVER_FAILED 2
#define RCODE_NAME_ERROR    3
#define RCODE_NOTIMPL       4
#define RCODE_REFUSED       5

//
// Data format for the query section
//  QNAME: <name> - the domain name in question
//  QTYPE: 2 bytes - query type
//  QCLASS: 2 bytes - query class
//

//
// DNS server port number
//
#define DNS_SERVER_PORT 53

#endif // !_DNS_H

