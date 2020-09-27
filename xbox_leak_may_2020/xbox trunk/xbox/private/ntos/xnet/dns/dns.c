/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    dns.c

Abstract:

    DNS client implementation

Revision History:

    07/12/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Access to global DNS client data is protected by a mutex
//
RTL_CRITICAL_SECTION DnsCritSect;

#define DnsLock()   RtlEnterCriticalSection(&DnsCritSect)
#define DnsUnlock() RtlLeaveCriticalSection(&DnsCritSect)

//
// Default suffixes we'll try to append to the user specified
// name when doing a DNS lookup.
// 
#define MAX_DEFAULT_SUFFIXES 2
DnsName* DnsDefaultSuffixes[MAX_DEFAULT_SUFFIXES];
UINT DnsDefaultSuffixCount;

const DnsName* DnsRootDomainName = "\1\0";

//
// Cached list of DNS servers
//
LIST_ENTRY DnsServerCache;
UINT DnsServerCacheSize;
UINT cfgMaxDnsServerCacheSize = 16;

// Loop through all entries of the DNS server cache

#define DnsServerCacheFirst() ((DnsServerCacheEntry*) DnsServerCache.Flink)
#define DnsServerCacheNull() ((DnsServerCacheEntry*) &DnsServerCache)
#define DnsServerCacheFlink(_dnssrv) ((DnsServerCacheEntry*) (_dnssrv)->links.Flink)

#define LOOP_THRU_DNSSERVER_CACHE(_dnssrv) { \
            DnsServerCacheEntry* _next; \
            _dnssrv = DnsServerCacheFirst(); \
            for (; _dnssrv != DnsServerCacheNull(); _dnssrv = _next) { \
                _next = DnsServerCacheFlink(_dnssrv);

#define END_DNSSERVER_CACHE_LOOP \
            } \
        }

//
// Cached result of DNS lookups
// NOTE: we're using a simple linked list structure here.
// If the number of cache entries gets big, we may need
// something more elaborated for better perf.
//
LIST_ENTRY DnsCache;
UINT DnsCacheSize;
UINT cfgMaxDnsCacheSize = 64;
UINT cfgMaxLookupRetries = 4;
UINT cfgMinLookupTimeout = 5;
UINT cfgMaxDnsCacheTtl = 2*60*60;

// Loop through all entries of the DNS lookup cache

#define DnsCacheFirst() ((DnsCacheEntry*) DnsCache.Flink)
#define DnsCacheNull() ((DnsCacheEntry*) &DnsCache)
#define DnsCacheFlink(_dnsent) ((DnsCacheEntry*) (_dnsent)->links.Flink)

#define LOOP_THRU_DNS_CACHE(_dnsent) { \
            DnsCacheEntry* _next; \
            _dnsent = DnsCacheFirst(); \
            for (; _dnsent != DnsCacheNull(); _dnsent = _next) { \
                _next = DnsCacheFlink(_dnsent);

#define END_DNS_CACHE_LOOP \
            } \
        }

//
// Pending request to set default DNS domain and servers
//
IfInfo* DnsSetDefaultIfp;

//
// Next DNS query message ID
//
WORD DnsQueryMsgId;

//
// Forward function declarations
//
PRIVATE NTSTATUS DnsLookupProc(DnsName*, IPADDR, DnsCacheEntry**);


NTSTATUS
DnsInitialize()

/*++

Routine Description:

    Initialize the DNS client module

Arguments:

    NONE

Return Value:

    Status code

--*/

{
    RtlInitializeCriticalSection(&DnsCritSect);

    ZeroMem(DnsDefaultSuffixes, sizeof(DnsDefaultSuffixes));
    DnsDefaultSuffixCount = 0;

    InitializeListHead(&DnsServerCache);
    InitializeListHead(&DnsCache);
    DnsServerCacheSize = DnsCacheSize = 0;

    DnsQueryMsgId = (WORD) XnetRand();
    return NETERR_OK;
}


//
// Clean up the default search suffixes
//
INLINE VOID DnsCleanupDefaultSuffixes() {
    while (DnsDefaultSuffixCount) {
        DnsDefaultSuffixCount--;
        FreeDnsName(DnsDefaultSuffixes[DnsDefaultSuffixCount]);
        DnsDefaultSuffixes[DnsDefaultSuffixCount] = NULL;
    }
}

//
// Remove an entry from the DNS lookup cache
//
INLINE VOID FreeDnsCacheEntry(DnsCacheEntry* dnsent) {
    UINT index;
    for (index=0; index < dnsent->namecnt; index++) {
        FreeDnsName(dnsent->names[index]);
    }
    SysFree(dnsent);
}

INLINE VOID RemoveDnsCacheEntry(DnsCacheEntry* dnsent) {
    RemoveEntryList(&dnsent->links);
    DnsCacheSize--;

    ASSERT(dnsent->refcount != 0);
    if (dnsent->refcount > 1) {
        dnsent->refcount--;
        if (dnsent->status == NETERR_PENDING) {
            dnsent->status = NETERR_NETDOWN;
            SetKernelEvent(&dnsent->event);
        }
    } else {
        FreeDnsCacheEntry(dnsent);
    }
}

INLINE VOID DnsCacheEntryRelease(DnsCacheEntry* dnsent) {
    if (InterlockedDecrement(&dnsent->refcount) == 0) {
        FreeDnsCacheEntry(dnsent);
    }
}


//
// Remove an entry from the DNS server cache
//
INLINE VOID RemoveDnsServerCacheEntry(DnsServerCacheEntry* dnssrv) {
    RemoveEntryList(&dnssrv->links);
    SysFree(dnssrv);
    DnsServerCacheSize--;
}


VOID
DnsCleanup()

/*++

Routine Description:

    Clean up all DNS client data

Arguments:

    NONE

Return Value:

    NONE

--*/

{
    // If DnsInitialize hasn't been called, do nothing
    if (IsListNull(&DnsServerCache)) return;

    DnsLock();

    // Clean up the default search suffixes
    DnsCleanupDefaultSuffixes();

    // Clean up the cached DNS servers
    while (!IsListEmpty(&DnsServerCache)) {
        DnsServerCacheEntry* dnssrv = DnsServerCacheFirst();
        RemoveDnsServerCacheEntry(dnssrv);
    }

    // Clean up the cached DNS lookup results
    while (!IsListEmpty(&DnsCache)) {
        DnsCacheEntry* dnsent = DnsCacheFirst();
        RemoveDnsCacheEntry(dnsent);
    }

    DnsUnlock();

    RtlDeleteCriticalSection(&DnsCritSect);
    DnsSetDefaultIfp = NULL;
}


DnsName*
CreateDnsNameFromString(
    const CHAR* str
    )

/*++

Routine Description:

    Create a DNS name structure from a character string

Arguments:

    str - Points to the domain name character string 

Return Value:

    Pointer to the new DNS name structure
    NULL if there is an error

--*/

{
    DnsName* dnsname;
    BYTE* data;
    BYTE* p;
    BYTE* q;
    UINT labels = 0;
    UINT len = strlen(str);

    // Validate length
    if (len == 0 || len >= DNS_MAXNAMELEN-1) return NULL;

    dnsname = (DnsName*) SysAlloc(len+4, PTAG_DNS);
    if (!dnsname) return NULL;

    data = GetDnsNameData(dnsname);
    p = data+1;
    CopyMem(p, str, len+1);

    // parse the '.' separated labels
    do {
        for (q=p; *q && *q != '.'; q++)
            NULL;
        
        // zero-length label is an error unless it's at the end
        // (in that case, the extra . is ignored).

        if (q == p) {
            if (*q == 0 && labels > 0) {
                *--q = 0;
                break;
            }
            SysFree(dnsname);
            return NULL;
        }

        if (q-p > DNS_MAXLABELLEN) {
            SysFree(dnsname);
            return NULL;
        }
        p[-1] = (BYTE) (q-p);
        labels++;
        p = q+1;
    } while (*q);

    SetDnsNameLen(dnsname, q-data+1);
    SetDnsNameLabels(dnsname, labels);
    return dnsname;
}


PRIVATE DnsName*
CreateDnsNameFromLabels(
    const BYTE* data
    )

/*++

Routine Description:

    Create a DNS name structure from a sequence of labels

Arguments:

    data - Points to the label data

Return Value:

    Pointer to the newly created DNS name
    NULL if there is an error

Note:

    We assume the label data is already validated.

--*/

{
    UINT len = 1;
    UINT labels = 0;
    const BYTE* p = data;
    DnsName* dnsname;

    while (*p) {
        labels++;
        len += 1 + *p;
        p = SkipDnsNameLabel(p);
    }
    
    dnsname = (DnsName*) SysAlloc(len+2, PTAG_DNS);
    if (dnsname) {
        SetDnsNameLen(dnsname, len);
        SetDnsNameLabels(dnsname, labels);
        CopyMem(GetDnsNameData(dnsname), data, len);
    }

    return dnsname;
}


PRIVATE DnsName*
CreateDnsNameFromAddr(
    IPADDR addr
    )

/*++

Routine Description:

    Create a DNS name from an IP address:
        x.x.x.x.IN-ADDR.ARPA

Arguments:

    addr - Specifies the IP address

Return Value:

    Pointer to the newly created DNS name
    NULL if there is an error

--*/

{
    CHAR buf[32];

    // Reverse the byte order
    addr = HTONL(addr);
    IpAddrToString(addr, buf, sizeof(buf));
    strcat(buf, ".IN-ADDR.ARPA");

    return CreateDnsNameFromString(buf);
}


PRIVATE UINT
MatchDnsNames(
    const DnsName* name1,
    const DnsName* name2
    )

/*++

Routine Description:

    Compare two DNS names and how closely they match

Arguments:

    name1, name2 - Specifies the two DNS names to be compared

Return Value:

    Number of labels that are common between the two DNS names

--*/

#define IsEqualDnsNameLabel(_p1, _p2) \
        (*(_p1) == *(_p2) && _strnicmp((_p1), (_p2), *(_p1)) == 0)

{
    UINT labels, l1, l2;
    const BYTE* data1;
    const BYTE* data2;

    l1 = GetDnsNameLabels(name1);
    l2 = GetDnsNameLabels(name2);
    labels = min(l1, l2);
    data1 = GetDnsNameData(name1);
    data2 = GetDnsNameData(name2);

    data1 = SkipDnsNameLabels(data1, l1-labels);
    data2 = SkipDnsNameLabels(data2, l2-labels);

    l1 = labels;
    while (labels--) {
        if (!IsEqualDnsNameLabel(data1, data2))
            l1 = labels;
        data1 = SkipDnsNameLabel(data1);
        data2 = SkipDnsNameLabel(data2);
    }

    return l1;
}


PRIVATE BOOL
IsEqualDnsNames(
    const DnsName* name1,
    const DnsName* name2
    )

/*++

Routine Description:

    Check if two DNS names are the same (case insensitive)

Arguments:

    name1, name2 - Specifies the two DNS names to be compared

Return Value:

    TRUE if two names are the same, FALSE otherwise

--*/

{
    BYTE labels = GetDnsNameLabels(name1);
    const BYTE* data1;
    const BYTE* data2;

    // must have the same number of labels
    if (labels != GetDnsNameLabels(name2)) return FALSE;

    data1 = GetDnsNameData(name1);
    data2 = GetDnsNameData(name2);
    while (labels--) {
        // every label must be equal
        if (!IsEqualDnsNameLabel(data1, data2))
            return FALSE;
        data1 = SkipDnsNameLabel(data1);
        data2 = SkipDnsNameLabel(data2);
    }
    return TRUE;
}


PRIVATE VOID
DnsNameToString(
    const DnsName* dnsname,
    CHAR* buf,
    UINT buflen
    )

/*++

Routine Description:

    Convert a DNS name string to a null-terminated ASCII string

Arguments:

    dnsname - Points to the DNS name structure
    buf - Points to the buffer for storing the ASCII string
    buflen - Size of the buffer

Return Value:

    NONE

--*/

{
    const BYTE* data = GetDnsNameData(dnsname);
    UINT len;

    // If the output buffer is not big enough,
    // just return an empty string.
    if (buflen < GetDnsNameLen(dnsname) ||
        GetDnsNameLen(dnsname) <= 1) {
        *buf = 0;
        return;
    }

    while ((len = *data++) != 0) {
        CopyMem(buf, data, len);
        data += len;
        buf += len;
        *buf++ = '.';
    }

    buf[-1] = 0;
}


INLINE CHAR* DNSNAMESTR(const DnsName* dnsname) {
    static CHAR buf[256];
    DnsNameToString(dnsname, buf, sizeof(buf));
    return buf;
}

DnsName*
ConcatDnsNames(
    const DnsName* name1,
    const DnsName* name2
    )

/*++

Routine Description:

    Concatenate two DNS names together to form a new name

Arguments:

    name1 - Specifies the name to put in the front
    name2 - Specifies the name to put at the end

Return Value:

    Points to the new concatenated name
    NULL if there is an error

--*/

{
    DnsName* dnsname;
    BYTE* data;
    UINT l1, l2, labels;

    l1 = GetDnsNameLen(name1);
    l2 = GetDnsNameLen(name2);
    labels = (UINT) GetDnsNameLabels(name1) +
             (UINT) GetDnsNameLabels(name2);

    // Make sure the resulting DNS is not too long
    if (l1+l2-1 > DNS_MAXNAMELEN)
        return NULL;

    dnsname = (DnsName*) SysAlloc(l1+l2+1, PTAG_DNS);
    if (dnsname) {
        data = GetDnsNameData(dnsname);
        CopyMem(data, GetDnsNameData(name1), l1);
        CopyMem(data+l1-1, GetDnsNameData(name2), l2);
        SetDnsNameLen(dnsname, l1+l2-1);
        SetDnsNameLabels(dnsname, labels);
    }

    return dnsname;
}


VOID
DnsNotifyDefaultServers(
    IfInfo* ifp
    )

/*++

Routine Description:

    This is called by the lower level modules to notify us
    that the default DNS server information has changed.

Arguments:

    ifp - Points to the interface in question

Return Value:

    NONE

Note:

    This function is called by the DHCP module
    (potentially at DISPATCH_LEVEL).

--*/

{
    // We don't immediately set the default servers.
    // Rather, we'll queue up a request which will be
    // executed upon the next lookup operation.
    //
    // If there is another pending request,
    // it'll be replaced by the new request.

    CACHE_IFP_REFERENCE(DnsSetDefaultIfp, ifp);
}


PRIVATE NTSTATUS
AddDnsServerCacheEntry(
    const DnsName* domain,
    IPADDR serverAddr,
    UINT expires,
    UINT priority
    )

/*++

Routine Description:

    Add an entry to the DNS server cache

Arguments:

    domain - Specifies the domain for which the server is responsible
    serverAddr - Specifies the server's IP address
    expires - When will this entry be no longer valid
    priority - The server precedence value (smaller is better)

Return Value:

    Status code

--*/

{
    DnsServerCacheEntry* newsrv;
    DnsServerCacheEntry* dnssrv;
    DnsServerCacheEntry* oldie;
    UINT len;

    TRACE_("Adding DNS server: %s %s %d",
           DNSNAMESTR(domain),
           IPADDRSTR(serverAddr),
           expires);

    if (!XnetIsValidUnicastAddr(serverAddr))
        return NETERR_PARAM;

    len = GetDnsNameLen(domain) + 2;
    newsrv = (DnsServerCacheEntry*) SysAlloc(sizeof(*newsrv) + len, PTAG_DNS);
    if (!newsrv) return NETERR_MEMORY;

    newsrv->serverAddr = serverAddr;
    newsrv->expires = expires;
    newsrv->priority = priority;
    newsrv->domain = (DnsName*) (newsrv+1);
    CopyMem(newsrv->domain, domain, len);

    DnsLock();
    expires = (UINT) -1;
    oldie = DnsServerCacheNull();
    LOOP_THRU_DNSSERVER_CACHE(dnssrv)

        // If the specified domain/server combination is
        // already in the cache, just return success.
        if ((dnssrv->serverAddr == serverAddr) &&
            IsEqualDnsNames(dnssrv->domain, domain)) {
            dnssrv->expires = max(newsrv->expires, dnssrv->expires);
            dnssrv->priority = min(newsrv->priority, dnssrv->priority);
            DnsUnlock();
            SysFree(newsrv);
            return NETERR_OK;
        }

        // Remember the entry that'll expire the earliest.
        if (dnssrv->expires < expires) {
            oldie = dnssrv;
            expires = dnssrv->expires;
        }

    END_DNSSERVER_CACHE_LOOP

    if (DnsServerCacheSize >= cfgMaxDnsServerCacheSize) {
        // If there are too many entries in the DNS server cache,
        // remove the one with the smallest TTL. If no such
        // entry exists, return error.

        if (oldie == DnsServerCacheNull()) {
            DnsUnlock();
            SysFree(newsrv);
            WARNING_("DNS server cache is full.");
            return NETERR_MEMORY;
        }

        RemoveDnsServerCacheEntry(oldie);
    }

    InsertHeadList(&DnsServerCache, &newsrv->links);
    DnsServerCacheSize++;
    DnsUnlock();

    return NETERR_OK;
}


PRIVATE VOID
DnsExecutePendingSetDefaultReq()

/*++

Routine Description:

    Execute pending request to set default DNS domain and servers

Arguments:

    NONE

Return Value:

    NONE

--*/

{
    DnsName* domainName;
    CHAR namestr[DNS_MAXNAMELEN+1];
    IPADDR addrs[MAX_DEFAULT_DNSSERVERS];
    UINT index, addrcnt;
    IfInfo* ifp;
    NTSTATUS status;

    ifp = DnsSetDefaultIfp;
    DnsSetDefaultIfp = NULL;
    if (!ifp) return;

    addrcnt = MAX_DEFAULT_DNSSERVERS;
    status = IfGetDefaultDnsServers(ifp, namestr, ARRAYCOUNT(namestr), addrs, &addrcnt);
    if (!NT_SUCCESS(status)) return;

    // Cleanup the DNS cache first
    DnsCleanup();
    if (addrcnt == 0) return;

    domainName = namestr[0] ?
                    CreateDnsNameFromString(namestr) :
                    CopyDnsName(DnsRootDomainName);

    if (!domainName) return;

    DnsLock();
    DnsCleanupDefaultSuffixes();

    DnsDefaultSuffixCount = 1;
    DnsDefaultSuffixes[0] = domainName;
    TRACE_("Default DNS search suffix: %s", DNSNAMESTR(domainName));

    // If the domain name has 3 or more labels, we'll add
    // the parent domain to the default search suffix list as well
    if (GetDnsNameLabels(domainName) >= 3) {
        BYTE* data;
        DnsName* dnsname;

        data = GetDnsNameData(domainName);
        data = SkipDnsNameLabel(data);
        dnsname = CreateDnsNameFromLabels(data);
        if (dnsname) {
            DnsDefaultSuffixCount++;
            DnsDefaultSuffixes[1] = dnsname;
            TRACE_("Default DNS search suffix: %s", DNSNAMESTR(dnsname));
        }
    }

    // Add default DNS servers to the cache
    for (index=0; index < addrcnt; index++) {
        AddDnsServerCacheEntry(domainName, *addrs, (UINT) -1, index);
    }

    DnsUnlock();
}


PRIVATE DnsCacheEntry*
CreateDnsCacheEntry(
    DnsName* dnsname,
    IPADDR addr
    )

/*++

Routine Description:

    Create a new entry in the DNS cache

Arguments:

    dnsname - Points to the name for the new entry

Return Value:

    Pointer to the newly created DNS cache entry
    NULL if there is an error

--*/

{
    DnsCacheEntry* dnsent;

    // Create a new entry and inserted it into the list
    dnsent = (DnsCacheEntry*) SysAlloc0(sizeof(*dnsent), PTAG_DNS);
    if (!dnsent) return NULL;

    if (dnsname) {
        dnsname = CopyDnsName(dnsname);
        if (!dnsname) {
            SysFree(dnsent);
            return NULL;
        }

        dnsent->namecnt = 1;
        dnsent->names[0] = dnsname;
    } else {
        dnsent->addrcnt = 1;
        dnsent->addrs[0] = addr;
    }

    // Refcount is initialized to 2 here:
    //  1 for keeping it in the cache
    //  1 for the caller 
    dnsent->refcount = 2;
    dnsent->status = NETERR_PENDING;
    dnsent->type = (WORD) (dnsname ? RRTYPE_A : RRTYPE_PTR);
    KeInitializeEvent(&dnsent->event, NotificationEvent, FALSE);

    InsertHeadList(&DnsCache, &dnsent->links);
    DnsCacheSize++;
    return dnsent;
}


PRIVATE IPADDR
FindBestDnsServer(
    DnsName* qname
    )

/*++

Routine Description:

    Find the best DNS server to use for the specified query

Arguments:

    qname - Specifies the query name

Return Value:

    IP address of the DNS server to use
    0 if no server is found

--*/

{
    UINT match, bestMatch, priority;
    IPADDR serverAddr;
    DnsServerCacheEntry* dnssrv;
    DnsServerCacheEntry* found;

    found = NULL;
    priority = (UINT) -1;
    bestMatch = 0;
    DnsLock();

    LOOP_THRU_DNSSERVER_CACHE(dnssrv)
        
        // Get rid of expired cache entries
        if (dnssrv->expires <= TcpTickCount) {
            RemoveDnsServerCacheEntry(dnssrv);
            continue;
        }

        match = MatchDnsNames(qname, dnssrv->domain);
        if (match > bestMatch ||
            match == bestMatch && dnssrv->priority < priority) {
            found = dnssrv;
            priority = dnssrv->priority;
            bestMatch = match;
        }

    END_DNSSERVER_CACHE_LOOP

    if (found) {
        // Move the server to the end of the search list
        // so that another server with the same priority
        // will get taken for the next lookup.
        RemoveEntryList(&found->links);
        InsertTailList(&DnsServerCache, &found->links);
        serverAddr = found->serverAddr;
    } else
        serverAddr = 0;

    DnsUnlock();
    return serverAddr;
}


PRIVATE VOID
MarkBadDnsServer(
    IPADDR serverAddr
    )

/*++

Routine Description:

    This function is called when we timed out while waiting
    for a response from the specified DNS server.

Arguments:

    serverAddr - Specifies the DNS server's IP address

Return Value:

    NONE

--*/

{
    DnsServerCacheEntry* dnssrv;
    DnsServerCacheEntry* prev;

    DnsLock();

    // NOTE: We can't use the LOOP_THRU_DNSSERVER_CACHE macro here
    // because we must go through the list in reverse order

    dnssrv = (DnsServerCacheEntry*) DnsServerCache.Blink;
    while (dnssrv != DnsServerCacheNull()) {
        prev = (DnsServerCacheEntry*) dnssrv->links.Blink;

        // Lower the server's precedence and
        // also move it to the end of the search list.
        if (dnssrv->serverAddr == serverAddr) {
            dnssrv->priority++;
            if (dnssrv->priority == -1)
                dnssrv->priority = 0;

            RemoveEntryList(&dnssrv->links);
            InsertTailList(&DnsServerCache, &dnssrv->links);
        }
        dnssrv = prev;
    }

    DnsUnlock();
}


PRIVATE INT
ComposeDnsQuery(
    BYTE* buf,
    DnsName* qname,
    WORD qtype
    )

/*++

Routine Description:

    Compose a DNS query message

Arguments:

    buf - Points to the data buffer
    qname - Query name
    qtype - Query type: RRTYPE_A or RRTYPE_PTR

Return Value:

    The length of the composed DNS query message

Note:
    
    We assume the data buffer size is at least DNS_MAXMSGLEN bytes.

--*/

{
    DnsHeader* dnshdr = (DnsHeader*) buf;
    WORD code;
    BYTE* p;
    UINT len;

    DnsQueryMsgId++;
    dnshdr->id = HTONS(DnsQueryMsgId);
    dnshdr->questions = HTONS(1);
    dnshdr->answers = dnshdr->nservers = dnshdr->extras = HTONS(0);

    // We always request recursive mode. But if the server
    // doesn't support, we'll fall back to use iterative mode.
    code = DNSFLAG_QUERY | DNSFLAG_RD | DNSOPCODE_STDQUERY;
    dnshdr->code = HTONS(code);

    p = buf + DNSHDRLEN;
    len = GetDnsNameLen(qname);
    CopyMem(p, GetDnsNameData(qname), len);
    p += len;

    *((WORD*) p) = HTONS(qtype);
    p += 2;

    *((WORD*) p) = HTONS(RRCLASS_IN);
    p += 2;

    return (p - buf);
}


//
// Extract a 16-bit value from the received message.
// NOTE: we're assuming little-endian machines here.
//
#define DNS_EXTRACT_WORD(_p) ((WORD) \
        (((WORD) (_p)[0] << 8) | \
         ((WORD) (_p)[1]     )))

// Extract a 32-bit value from the received message.
#define DNS_EXTRACT_DWORD(_p) \
        (((DWORD) (_p)[0] << 24) | \
         ((DWORD) (_p)[1] << 16) | \
         ((DWORD) (_p)[2] <<  8) | \
         ((DWORD) (_p)[3]      ))


//
// Free information about a resource record section
//
PRIVATE VOID FreeDnsRRList(DnsRR* rrlist) {
    while (rrlist) {
        DnsRR* rr = rrlist;
        rrlist = rrlist->next;

        FreeDnsName(rr->rrname);
        if (rr->type != RRTYPE_A) {
            FreeDnsName(rr->dataname);
        }
        SysFree(rr);
    }
}

//
// Free the information we parsed out of a DNS response
// Note: The DnsResp structure itself is not freed. 
//
PRIVATE VOID FreeDnsResp(DnsResp* dnsresp) {
    FreeDnsRRList(dnsresp->answers);
    FreeDnsRRList(dnsresp->nservers);
    FreeDnsRRList(dnsresp->extras);
}


PRIVATE BYTE*
DnsParseName(
    DnsResp* dnsresp,
    BYTE* p,
    DnsName** result
    )

/*++

Routine Description:

    Parse a name field out of a DNS response message
    (handles message compression too)

Arguments:

    dnsresp - Points to the information about the DNS response message
    p - Where the name field starts
    result - Returns a pointer to the parsed DNS name
        or NULL if there is an error

Return Value:

    Points to the first byte after the DNS name
    NULL if there is an error

--*/

{
    UINT totallen = 0;
    UINT ptrcnt = 0;
    BYTE* end = dnsresp->end;
    BYTE* retval = NULL;
    BYTE* namebuf = dnsresp->namebuf;

    *result = NULL;
    while (TRUE) {
        UINT b1, b2;

        if (p == end) return NULL;
        b1 = *p++;

        // Stop if we see the last label
        if (b1 == 0) {
            if (retval == NULL) retval = p;
            namebuf[totallen++] = 0;
            break;
        }

        b2 = b1 >> 6;
        if (b2 == 0) {
            // Regular label
            if (totallen + b1 + 1 >= DNS_MAXNAMELEN) return NULL;
            namebuf[totallen++] = (BYTE) b1;
            CopyMem(namebuf+totallen, p, b1);
            p += b1;
            totallen += b1;
        } else {
            // Name compression
            //  we artificially limit the number of name pointers to 8
            //  to avoid infinite recursion.
            INT offset;

            if (b2 == 1 || b2 == 2 || p == end || ++ptrcnt > 8)
                return NULL;
            
            offset = ((b1 & 0x3f) << 8) | *p++;
            if (offset >= end - dnsresp->start) return NULL;

            if (retval == NULL) retval = p;
            p = dnsresp->start + offset;
        }
    }

    *result = CreateDnsNameFromLabels(namebuf);
    return *result ? retval : NULL;
}


PRIVATE BYTE*
DnsParseRRSection(
    DnsResp* dnsresp,
    BYTE* p,
    DnsRR** sect,
    WORD count
    )

/*++

Routine Description:

    Parse a resource record section of a DNS response message

Arguments:

    dnsresp - Points to the information about the DNS response message
    p - Where the resource record section starts
    sect - Returns parsed information about the RR section
    count - Number of RRs as specified in the DNS message header

Return Value:

    Points to the first byte after the RR section

--*/

{
    BYTE* end = dnsresp->end;
    DnsRR* rrlist;
    DnsRR* rr;
    DnsName* dnsname;

    ASSERT(*sect == NULL);
    rrlist = NULL;
    count = NTOHS(count);

    while (count-- && p < end) {
        WORD type, cls, rdlen;
        DWORD ttl;
        BYTE* rdata;

        // record name
        p = DnsParseName(dnsresp, p, &dnsname);
        if (!p) break;

        // record type and class, ttl, and data length
        if (end-p < 10) goto badrr2;
        type = DNS_EXTRACT_WORD(p); p += 2;
        cls = DNS_EXTRACT_WORD(p); p += 2;
        ttl = DNS_EXTRACT_DWORD(p); p += 4;
        rdlen = DNS_EXTRACT_WORD(p); p += 2;
        if (end-p < rdlen) goto badrr2;
        rdata = p;
        p += rdlen;

        if (cls != RRCLASS_IN ||
            type != RRTYPE_A &&
            type != RRTYPE_NS &&
            type != RRTYPE_CNAME &&
            type != RRTYPE_SOA &&
            type != RRTYPE_PTR) {
            FreeDnsName(dnsname);
            continue;
        }

        rr = (DnsRR*) SysAlloc(sizeof(DnsRR), PTAG_DNS);
        if (!rr) goto badrr2;
        
        // record data
        switch (rr->type = type) {
        case RRTYPE_A:
            if (rdlen < IPADDRLEN) goto badrr1;
            CopyMem(&rr->addr, rdata, IPADDRLEN);
            break;

        case RRTYPE_NS:
        case RRTYPE_CNAME:
        case RRTYPE_PTR:
            if (!DnsParseName(dnsresp, rdata, &rr->dataname))
                goto badrr1;
            break;

        case RRTYPE_SOA: {
            DnsName* mxadmin;

            rdata = DnsParseName(dnsresp, rdata, &rr->dataname);
            if (rdata == NULL) goto badrr1;

            rdata = DnsParseName(dnsresp, rdata, &mxadmin);
            if (rdata == NULL) goto badrr1;

            FreeDnsName(mxadmin);
            if (end-rdata < 20) goto badrr1;

            // The TTL field isn't meaningful for SOA records.
            // Instead, we use the MINIMUM field as the TTL value
            // for negative caching.
            rdata += 16;
            ttl = DNS_EXTRACT_DWORD(rdata);
            }
            break;
        }

        rr->rrname = dnsname;
        rr->next = rrlist;
        rr->ttl = min(ttl, cfgMaxDnsCacheTtl);
        rrlist = rr;
    }

exit:
    // Reverse the resource record list
    // NOTE: even in case of error, a partial resource record list
    // is still returned to the caller.

    rr = rrlist;
    rrlist = NULL;
    while (rr) {
        DnsRR* next = rr->next;
        rr->next = rrlist;
        rrlist = rr;
        rr = next;
    }

    *sect = rrlist;
    return p;

badrr1:
    SysFree(rr);
badrr2:
    FreeDnsName(dnsname);
    p = NULL;
    goto exit;
}


//
// Check if a name is already associated with a DNS cache entry
//
INLINE BOOL DnsEntHasName(DnsName** names, UINT namecnt, DnsName* dnsname) {
    while (namecnt--) {
        if (IsEqualDnsNames(*names, dnsname)) return TRUE;
        names++;
    }
    return FALSE;
}


//
// Check if an address is already associated with a DNS cache entry
//
INLINE BOOL DnsEntHasAddr(IPADDR* addrs, UINT addrcnt, IPADDR addr) {
    while (addrcnt--) {
        if (*addrs++ == addr) return TRUE;
    }
    return FALSE;
}


//
// Add a name to a DNS cache entry
//
PRIVATE BOOL DnsEntAddName(DnsCacheEntry* dnsent, DnsName* dnsname) {
    if (DnsEntHasName(dnsent->names, dnsent->namecnt, dnsname) ||
        dnsent->namecnt >= MAXALIASCNT)
        return FALSE;

    dnsent->names[dnsent->namecnt++] = dnsname;
    return TRUE;
}


//
// Process CNAME records in a DNS response message
//
PRIVATE VOID DnsProcessCNames(DnsCacheEntry* dnsent, DnsRR* rrlist) {
    DnsName* dnsname;
    DnsRR* rr;
    UINT high, index = 0;

    while (index < dnsent->namecnt) {
        dnsname = dnsent->names[index++];
        for (rr=rrlist; rr; rr=rr->next) {
            if (rr->type == RRTYPE_CNAME && IsEqualDnsNames(rr->rrname, dnsname)) {
                DnsEntAddName(dnsent, rr->dataname);
            }
        }
    }

    // Reverse the name list
    ASSERT(dnsent->namecnt);
    index = 0;
    high = dnsent->namecnt - 1;
    while (index < high) {
        dnsname = dnsent->names[index];
        dnsent->names[index++] = dnsent->names[high];
        dnsent->names[high--] = dnsname;
    }
}


//
// Find the first DNS resource record of the specified
// name/type combination in a RR section
//
INLINE DnsRR* DnsFindRR(DnsRR* rrlist, DnsName* name, WORD type) {
    while (rrlist) {
        if ((rrlist->type == type) && 
            (name == NULL || IsEqualDnsNames(name, rrlist->rrname)))
            break;
        rrlist = rrlist->next;
    }
    return rrlist;
}


PRIVATE VOID
DnsProcessPositiveResp(
    DnsCacheEntry* dnsent,
    DnsResp* dnsresp,
    DnsName* qname
    )

/*++

Routine Description:

    Process a positive answer from the DNS server

Arguments:

    dnsent - Points to the DNS cache entry corresponding to the DNS query
    dnsresp - Information about the DNS response message
    qname - Specifies the original DNS query name

Return Value:

    NONE

--*/

{
    DnsRR* rr = dnsresp->answers;
    UINT ttl = (UINT) -1;

    if (dnsent->type == RRTYPE_PTR) {
        // Process all PTR records first
        while (rr) {
            if (rr->type == RRTYPE_PTR && IsEqualDnsNames(rr->rrname, qname)) {
                if (DnsEntAddName(dnsent, rr->dataname)) {
                    rr->dataname = NULL;
                    ttl = min(ttl, rr->ttl);
                }
            }
            rr = rr->next;
        }

        // Then process all CNAME records
        DnsProcessCNames(dnsent, dnsresp->answers);
    } else {
        // Process all CNAME records first
        DnsProcessCNames(dnsent, rr);

        // Then process all A records
        while (rr) {
            if (rr->type == RRTYPE_A &&
                DnsEntHasName(dnsent->names, dnsent->namecnt, rr->rrname) &&
                !DnsEntHasAddr(dnsent->addrs, dnsent->addrcnt, rr->addr) &&
                dnsent->addrcnt < MAXALIASCNT) {
                dnsent->addrs[dnsent->addrcnt++] = rr->addr;
                ttl = min(ttl, rr->ttl);
            }
            rr = rr->next;
        }
    }

    dnsent->expires = DnsCacheExpires(ttl);
}


PRIVATE VOID
DnsProcessNegativeResp(
    DnsCacheEntry* dnsent,
    DnsResp* dnsresp
    )

/*++

Routine Description:

    Process negative response to a DNS query

Arguments:

    dnsent - Points to the DNS cache entry corresponding to the query
    dnsresp - Information about the DNS response message

Return Value:

    NONE

--*/

{
    DnsRR* rr;

    // If this was a name-to-address query,
    // process the CNAME records.
    if (dnsent->type == RRTYPE_A) {
        DnsProcessCNames(dnsent, dnsresp->answers);
    }

    // Figure out the negative cache TTL
    rr = DnsFindRR(dnsresp->nservers, NULL, RRTYPE_SOA);
    dnsent->expires = DnsCacheExpires(rr ? rr->ttl : 0);
}


PRIVATE NTSTATUS
DnsProcessNServers(
    DnsCacheEntry* dnsent,
    DnsResp* dnsresp,
    BOOL referral
    )

/*++

Routine Description:

    Process the name server information inside the nservers and extras
    sections of a DNS response message

Arguments:

    dnsent - Points to the DNS cache entry corresponding to a DNS query
    dnsresp - Information about the DNS response message
    referral - Whether the response is a referral

Return Value:

    Status code

--*/

{
    DnsRR* rrNS;
    DnsRR* rrA;
    IPADDR serverAddr;
    NTSTATUS status = NETERR_OK;

    for (rrNS = dnsresp->nservers; rrNS; rrNS = rrNS->next) {
        if (rrNS->type != RRTYPE_NS) continue;
        rrA = DnsFindRR(dnsresp->extras, rrNS->dataname, RRTYPE_A);

        if (rrA) {
            serverAddr = rrA->addr;
        } else {
            serverAddr = 0;

            if (referral) {
                DnsCacheEntry* found;
                status = DnsLookupProc(rrNS->dataname, 0, &found);
                if (NT_SUCCESS(status)) {
                    status = found->status;
                    serverAddr = found->addrs[0];
                    DnsCacheEntryRelease(found);

                    // If anther thread is actively resolving
                    // the address of our referred server,
                    // return WSATRY_AGAIN error rather than wait here
                    // to avoid potential deadlock.
                    if (status == NETERR_PENDING) {
                        status = NETERR(WSATRY_AGAIN);
                    }
                }
            }
        }

        if (serverAddr) {
            status = AddDnsServerCacheEntry(
                            rrNS->rrname,
                            serverAddr,
                            DnsCacheExpires(rrNS->ttl),
                            0);
        }

        // If this function was called because of a referral response,
        // we'll return after seeing the first NS records.
        if (referral) return status;
    }

    return referral ? NETERR_PARAM : NETERR_OK;
}


PRIVATE NTSTATUS
DnsProcessResponse(
    DnsCacheEntry* dnsent,
    DnsResp* dnsresp,
    DnsName* qname,
    DnsName** tempname
    )

/*++

Routine Description:

    Process a received DNS query response

Arguments:

    dnsent - Points to the DNS cache entry
    dnsresp - Points to the DNS response information
    qname - Specifies the name that's being queried
    tempname - Returns a pointer to the new query name
        if the response is a referral and
        the query type involved was RRTYPE_PTR

Return Value:

    Status code:
        NETERR_OK - the response was a positive answer to our query
        NETERR(WSAHOST_NOT_FOUND)
        NETERR(WSANO_DATA) - the response was a negative answer
        DNSRESP_REFERRAL - the response was a referral
        other error code - there was an error interpreting the response

--*/

#define DNSRESP_REFERRAL 1

{
    NTSTATUS status;
    DnsHeader* dnshdr;
    WORD code, type, cls;
    BYTE* p;
    DnsName* dnsname;

    // Validate header information
    // NOTE: We don't compare DNS message ID here.
    if (dnsresp->end - dnsresp->start < DNSHDRLEN)
        return NETERR_PARAM;

    dnshdr = (DnsHeader*) dnsresp->start;
    code = NTOHS(dnshdr->code);
    if (!(code & DNSFLAG_RESPONSE) ||
        DNSGETOPCODE(code) != DNSOPCODE_STDQUERY ||
        dnshdr->questions != HTONS(1)) {
        return NETERR_PARAM;
    }

    // Interpret the RCODE
    code = (WORD) DNSGETRCODE(code);
    switch (code) {
    case RCODE_OK:
    case RCODE_NAME_ERROR:
        // continue processing
        break;

    case RCODE_SERVER_FAILED:
        return NETERR(WSATRY_AGAIN);

    case RCODE_FORMAT_ERROR:
    default:
        return NETERR_PARAM;
    }

    // Parse the questions section

    p = dnsresp->start + DNSHDRLEN;
    p = DnsParseName(dnsresp, p, &dnsname);
    if (!dnsname || !IsEqualDnsNames(dnsname, qname)) {
        FreeDnsName(dnsname);
        return NETERR_PARAM;
    }
    FreeDnsName(dnsname);

    if (dnsresp->end - p < 4) return NETERR_PARAM;
    type = DNS_EXTRACT_WORD(p); p += 2;
    cls = DNS_EXTRACT_WORD(p); p += 2;
    if (type != dnsent->type || cls != RRCLASS_IN)
        return NETERR_PARAM;

    // Parse the resource record sections:
    //  We only require the answers section to be complete.
    //  Errors in the other two sections are ignored.
    p = DnsParseRRSection(dnsresp, p, &dnsresp->answers, dnshdr->answers);
    if (!p) return NETERR_PARAM;

    p = DnsParseRRSection(dnsresp, p, &dnsresp->nservers, dnshdr->nservers);
    if (p) {
        DnsParseRRSection(dnsresp, p, &dnsresp->extras, dnshdr->extras);
    }

    // Process the information in the answers section
    if (code == RCODE_OK) {
        if (DnsFindRR(dnsresp->answers, qname, dnsent->type)) {
            // Positive answer
            DnsProcessPositiveResp(dnsent, dnsresp, qname);
            status = NETERR_OK;
        } else {
            // We differentiate the "no data" case from the "referral" case
            // by the presence of SOA record or the absence of NS record
            // in the nservers section.
            if (DnsFindRR(dnsresp->nservers, NULL, RRTYPE_SOA) ||
                !DnsFindRR(dnsresp->nservers, NULL, RRTYPE_NS)) {
                TRACE_("No data for host: %s", DNSNAMESTR(qname));
                DnsProcessNegativeResp(dnsent, dnsresp);
                status = NETERR(WSANO_DATA);
            } else {
                TRACE_("Referral for host: %s", DNSNAMESTR(qname));
                if (dnsent->type == RRTYPE_A) {
                    DnsProcessCNames(dnsent, dnsresp->answers); 
                    TRACE_("  %s", DNSNAMESTR(dnsent->names[0]));
                } else {
                    DnsRR* rr = DnsFindRR(dnsresp->answers, qname, RRTYPE_CNAME);
                    if (rr) {
                        TRACE_("  %s", DNSNAMESTR(rr->dataname));
                        FreeDnsName(*tempname);
                        *tempname = rr->dataname;
                        rr->dataname = NULL;
                    }
                }
                status = DNSRESP_REFERRAL;
            }
        }
    } else {
        // Name error
        TRACE_("Host not found: %s", DNSNAMESTR(qname));
        DnsProcessNegativeResp(dnsent, dnsresp);
        status = NETERR(WSAHOST_NOT_FOUND);
    }

    // Process the information in the nservers and extras sections
    if (status == DNSRESP_REFERRAL) {
        NTSTATUS status1 = DnsProcessNServers(dnsent, dnsresp, TRUE);
        if (!NT_SUCCESS(status1)) status = status1;
    } else {
        DnsProcessNServers(dnsent, dnsresp, FALSE);
    }

    return status;
}


PRIVATE NTSTATUS
ResolveDnsCacheEntry(
    DnsCacheEntry* dnsent
    )

/*++

Routine Description:

    Resolve a DNS cache entry

Arguments:

    dnsent - Points to the DNS cache entry to be resolved

Return Value:

    Status code

--*/

{
    SOCKET s = INVALID_SOCKET;
    struct sockaddr_in sockname;
    INT fromlen, datalen;
    NTSTATUS status;
    UINT retries, timeout;
    CHAR* databuf;
    DnsName* qname;
    DnsName* tempname = NULL;
    IPADDR serverAddr;
    UINT cnames = 0;
    DnsResp* dnsresp;

    ASSERT(dnsent->status == NETERR_PENDING);
    dnsent->expires = (UINT) -1;

    // allocate temporary buffer for send and receive
    // create a UDP socket and bind it
    databuf = (BYTE*) SysAlloc(DNS_MAXMSGLEN, PTAG_DNS);
    dnsresp = (DnsResp*) SysAlloc(sizeof(DnsResp), PTAG_DNS);
    if (!databuf || !dnsresp) {
        status = NETERR_MEMORY; goto exit;
    }

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == INVALID_SOCKET) goto errexit;

    ZeroMem(&sockname, sizeof(sockname));
    sockname.sin_family = AF_INET;
    sockname.sin_port = 0;
    sockname.sin_addr.s_addr = INADDR_ANY;

    status = bind(s, (struct sockaddr*) &sockname, sizeof(sockname));
    if (status != NO_ERROR) goto errexit;

    // Generate IN-ADDR.ARPA query name if we're
    // doing an address-to-name lookup
    if (dnsent->type == RRTYPE_PTR) {
        tempname = CreateDnsNameFromAddr(dnsent->addrs[0]);
        if (!tempname) {
            status = NETERR_MEMORY; goto exit;
        }
    }

newlookup:
    // Give up if there are too many levels of CNAMEs
    if (cnames++ >= MAXALIASCNT) {
        status = NETERR(WSAHOST_NOT_FOUND);
        goto exit;
    }

    timeout = cfgMinLookupTimeout * 1000;
    retries = cfgMaxLookupRetries;
    while (TRUE) {
        if (retries-- == 0) {
            status = NETERR(WSATRY_AGAIN);
            break;
        }

        // Look for an appropriate DNS server
        qname = (dnsent->type == RRTYPE_A) ? dnsent->names[0] : tempname;
        TRACE_("Query DNS name: %s", DNSNAMESTR(qname));

        serverAddr = FindBestDnsServer(qname);
        if (serverAddr == 0) {
            status = NETERR(WSAENETDOWN);
            break;
        }
        // Send out a message
        datalen = ComposeDnsQuery(databuf, qname, dnsent->type);

        sockname.sin_port = HTONS(DNS_SERVER_PORT);
        sockname.sin_addr.s_addr = serverAddr;
        TRACE_("  DNS server: %s", inet_ntoa(sockname.sin_addr));

        fromlen = sizeof(sockname);
        datalen = sendto(s, databuf, datalen, 0, (struct sockaddr*) &sockname, fromlen);
        if (datalen == SOCKET_ERROR) continue;

        // wait for reply
        status = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (CHAR*) &timeout, sizeof(timeout));
        if (status != NO_ERROR) goto errexit;
        timeout += timeout;

        datalen = recvfrom(s, databuf, DNS_MAXMSGLEN, 0, (struct sockaddr*) &sockname, &fromlen);
        if (datalen == SOCKET_ERROR) {
            if (WSAGetLastError() != WSAETIMEDOUT) goto errexit;
            MarkBadDnsServer(serverAddr);
            continue;
        }
        TRACE_("Response recieved from server: %s", inet_ntoa(sockname.sin_addr));

        // Process the received message
        dnsresp->start = databuf;
        dnsresp->end = databuf + datalen;
        dnsresp->answers = dnsresp->nservers = dnsresp->extras = NULL;
        status = DnsProcessResponse(dnsent, dnsresp, qname, &tempname);
        FreeDnsResp(dnsresp);

        switch (status) {
        case DNSRESP_REFERRAL:
            // Iterative lookup
            goto newlookup;
        
        case NETERR_OK:
        case NETERR(WSAHOST_NOT_FOUND):
        case NETERR(WSANO_DATA):
            // stop searching and return appropriate error code
            goto exit;

        default:
            // continue on with the next server
            VERBOSE_("Error processing response: 0x%x", status);
            break;
        }
    }

exit:
    if (s != INVALID_SOCKET) {
        closesocket(s);
    }
    SysFree(databuf);
    SysFree(dnsresp);
    FreeDnsName(tempname);

    // In case of temporary server error,
    // cache the negative response for a few seconds
    if (dnsent->expires == -1) {
        dnsent->expires = DnsCacheExpires(cfgMinLookupTimeout);
    }
    dnsent->status = status;
    SetKernelEvent(&dnsent->event);
    return status;

errexit:
    status = WSAGetLastError();
    status = (status == NO_ERROR) ? NETERR_SYSCALL : NETERR(status);
    goto exit;
}


PRIVATE NTSTATUS
DnsLookupProc(
    DnsName* dnsname,
    IPADDR addr,
    DnsCacheEntry** result
    )

/*++

Routine Description:

    DNS lookup function - called to:
    - resolve a host name to IP addresses,
    - resolve an IP address to host names

Arguments:

    dnsname - Specifies the host name (NULL if doing lookup by address)
    addr - Specifies the IP address
    result - Return the result of the lookup

Return Value:

    Status code

--*/

{
    DnsCacheEntry* dnsent;
    DnsCacheEntry* found;
    DnsCacheEntry* oldie;
    UINT expires;

    *result = NULL;
    found = NULL;
    expires = (UINT) -1;
    oldie = DnsCacheNull();

    DnsLock();
    LOOP_THRU_DNS_CACHE(dnsent)
        
        // If we have an entry that's already expired,
        // remove it from the list now.
        if (dnsent->expires <= TcpTickCount) {
            RemoveDnsCacheEntry(dnsent);
            continue;
        }

        // Check if the specified name or address matches
        // the names or addresses associated with this DNS cache entry
        if (dnsname && DnsEntHasName(dnsent->names, dnsent->namecnt, dnsname) ||
            !dnsname && DnsEntHasAddr(dnsent->addrs, dnsent->addrcnt, addr)) {
            dnsent->refcount++;
            found = dnsent;
            break;
        }

        // Remember the entry that has the earliest
        // expiration time.

        if (dnsent->expires < expires) {
            oldie = dnsent;
            expires = dnsent->expires;
        }

    END_DNS_CACHE_LOOP

    if (!found) {
        // If the name is not already in the cache,
        // we'll create a new DNS cache entry.

        if (DnsCacheSize >= cfgMaxDnsCacheSize) {
            if (oldie == DnsCacheNull()) {
                DnsUnlock();
                WARNING_("DNS cache is full.");
                return NETERR_MEMORY;
            }
            RemoveDnsCacheEntry(oldie);
        }

        found = CreateDnsCacheEntry(dnsname, addr);
        DnsUnlock();
        if (!found) return NETERR_MEMORY;

        // Resolve the new entry now
        ResolveDnsCacheEntry(found);
    } else {
        DnsUnlock();
    }

    *result = found;
    return NETERR_OK;
}


PRIVATE NTSTATUS
ReturnDnsEntryInfo(
    DnsCacheEntry* dnsent,
    DnsLookupCallback callback,
    VOID* param
    )

/*++

Routine Description:

    Return information for a DNS lookup

Arguments:

    dnsent - Points to the cached DNS entry
    callback - Callback function for allocating output buffers
    param - Parameter passed to the callback function

Return Value:

    Status code

--*/

{
    UINT size;
    BYTE index, namecnt, addrcnt, namelen;
    struct hostent* hostentbuf;
    BYTE *p;

    // Check if some other thread is actively resolving this entry.
    // If so, we'll just wait until the other thread complete its work.
    if (dnsent->status == NETERR_PENDING) {
        WaitKernelEventObject(&dnsent->event, 0);
    }

    // If the result was unsuccessful, return error
    if (!NT_SUCCESS(dnsent->status))
        return dnsent->status;

    namecnt = dnsent->namecnt;
    addrcnt = dnsent->addrcnt;
    ASSERT(namecnt && addrcnt);

    // Figure out the total buffer size we need to
    // output the information in the DNS entry
    size = sizeof(struct hostent) +
           sizeof(CHAR*) * namecnt +
           sizeof(CHAR*) * (addrcnt + 1) +
           sizeof(struct in_addr) * addrcnt;

    for (index=0; index < namecnt; index++) {
        size += GetDnsNameLen(dnsent->names[index]);
    }

    // Allocate memory buffer
    hostentbuf = callback(size, param);
    if (!hostentbuf) return NETERR_MEMORY;

    hostentbuf->h_addrtype = AF_INET;
    hostentbuf->h_length = IPADDRLEN;

    // 1. null-terminated array of alias name pointers
    // comes after the hostent structure itself

    p = (BYTE*) (hostentbuf+1);
    hostentbuf->h_aliases = (CHAR**) p;
    p += sizeof(CHAR*) * namecnt;

    // 2. then comes the null-terminate array of pointers to
    // the list of addresses.

    hostentbuf->h_addr_list = (CHAR**) p;
    p += sizeof(CHAR*) * (addrcnt + 1);

    // 3. then comes the array of addresses themselves

    for (index=0; index < addrcnt; index++) {
        hostentbuf->h_addr_list[index] = (CHAR*) p;
        ((struct in_addr*) p)->s_addr = dnsent->addrs[index];
        p += sizeof(struct in_addr);
    }
    hostentbuf->h_addr_list[addrcnt] = NULL;

    // 4. then comes the primary DNS name

    hostentbuf->h_name = (CHAR*) p;
    namelen = GetDnsNameLen(dnsent->names[0]);
    DnsNameToString(dnsent->names[0], p, namelen);
    p += namelen;

    // 5. and finally the list of name aliases

    for (index=1; index < namecnt; index++) {
        hostentbuf->h_aliases[index-1] = (CHAR*) p;
        namelen = GetDnsNameLen(dnsent->names[index]);
        DnsNameToString(dnsent->names[index], p, namelen);
        p += namelen;
    }
    hostentbuf->h_aliases[namecnt-1] = NULL;

    return NETERR_OK;
}


NTSTATUS
DnsLookupByName(
    const CHAR* name,
    DnsLookupCallback callback,
    VOID* param
    )

/*++

Routine Description:

    Map a hostname to its corresponding IP addresses

Arguments:

    name - Specifies the host name to be looked up
    callback - Callback function for allocating output buffers
    param - Parameter passed to the callback function

Return Value:

    Status code

--*/

{
    DnsCacheEntry* found;
    DnsName* dnsname;
    DnsName* names[MAX_DEFAULT_SUFFIXES+1];
    UINT index, namecnt;
    NTSTATUS status = NETERR_OK;

    // If there is a pending request to set default
    // DNS domain and servers, execute it now.
    if (DnsSetDefaultIfp) {
        DnsExecutePendingSetDefaultReq();
    }

    dnsname = CreateDnsNameFromString(name);
    if (!dnsname) return NETERR_MEMORY;

    // Compose a list of names to search for
    // by appending the default search suffixes.

    namecnt = 0;
    DnsLock();

    // If the specified name has more than 1 labels,
    // then search it first.
    if (GetDnsNameLabels(dnsname) > 1)
        names[namecnt++] = dnsname;

    // If the specified name ends in '.'
    // then we don't try the default search suffixes
    if (name[strlen(name)-1] != '.') {
        for (index=0; index < DnsDefaultSuffixCount; index++) {
            names[namecnt] = ConcatDnsNames(dnsname, DnsDefaultSuffixes[index]);
            if (names[namecnt]) 
                namecnt++;
        }
    }
    
    // If the specified name has just 1 label, then
    // search it last.
    if (GetDnsNameLabels(dnsname) <= 1)
        names[namecnt++] = dnsname;

    DnsUnlock();

    // Try all the name combinations and stop until one succeeds
    for (index=0; index < namecnt; index++) {
        NTSTATUS statusTemp;

        statusTemp = DnsLookupProc(names[index], 0, &found);
        if (NT_SUCCESS(statusTemp)) {
            statusTemp = ReturnDnsEntryInfo(found, callback, param);
            DnsCacheEntryRelease(found);

            if (NT_SUCCESS(statusTemp)) {
                status = statusTemp;
                break;
            }
        }

        // Remember the error code if this was the original name
        if (names[index] == dnsname)
            status = statusTemp;
    }

    for (index=0; index < namecnt; index++) {
        FreeDnsName(names[index]);
    }
    return status;
}


NTSTATUS
DnsLookupByAddr(
    IPADDR addr,
    DnsLookupCallback callback,
    VOID* param
    )

/*++

Routine Description:

    Map an IP address to its corresponding hostnames

Arguments:

    addr - Specifies the IP address to be looked up
    callback - Callback function for allocating output buffers
    param - Parameter passed to the callback function

Return Value:

    Status code

--*/

{
    DnsCacheEntry* found;
    NTSTATUS status;

    // If there is a pending request to set default
    // DNS domain and servers, execute it now.
    if (DnsSetDefaultIfp) {
        DnsExecutePendingSetDefaultReq();
    }

    status = DnsLookupProc(NULL, addr, &found);
    if (NT_SUCCESS(status)) {
        status = ReturnDnsEntryInfo(found, callback, param);
        DnsCacheEntryRelease(found);
    }

    return status;
}

