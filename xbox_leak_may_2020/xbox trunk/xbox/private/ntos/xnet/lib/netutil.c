/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    netutil.c

Abstract:

    Misc. utility functions used by the net code

Revision History:

    05/17/2000 davidx
        Created it.

--*/

#include "precomp.h"


Packet*
XnetAllocPacket(
    UINT size,
    UINT pktflags
    )

/*++

Routine Description:

    Allocate memory for holding a network packet

Arguments:

    size - Specifies the desired packet size (not including header)
    pktflags - Allocation flags: PKTFLAG_NETPOOL or 0

Return Value:

    Pointer to the allocated packet structure
    NULL if we ran out of memory

--*/

{
    Packet* pkt;

    size += PKTHDRLEN;

    #ifndef DVTSNOOPBUG

    if (pktflags & PKTFLAG_NETPOOL)
        pkt = (Packet*) XnetAlloc(size, PTAG_PKT);
    else 
        pkt = (Packet*) SysAlloc(size, PTAG_PKT);

    #else

    // BUGBUG: temporary hack to workaround nv2a snooping bug
    pktflags = PKTFLAG_UNCACHED;
    pkt = (Packet*) XnetUncachedAllocProc(size, PTAG_PKT);

    #endif

    if (pkt) {
        pkt->nextpkt = NULL;
        pkt->pktflags = (WORD) (pktflags | defaultPacketAllocFlag);
        pkt->data = pkt->buf;
        pkt->datalen = 0;
        pkt->completionCallback = NULL;
    } else {
        WARNING_("Couldn't allocate packet: out of memory");
    }

    return pkt;
}


Packet*
XnetCopyPacket(
    Packet* pkt,
    UINT extraHdr
    )

/*++

Routine Description:

    Make a copy of the specified packet

Arguments:

    pkt - Specifies the packet to be copied
    extraHdr - Extra space to reserve at the beginning of the copied packet

Return Value:

    Return a pointer to the copied packet
    NULL if out of memory

--*/

{
    Packet* newpkt;

    ASSERT(pkt != NULL);

    // Allocate memory space for the new packet
    newpkt = XnetAllocPacket(extraHdr + pkt->datalen, 0);
    if (!newpkt) return NULL;
    newpkt->data += extraHdr;

    // Copy data into the new packet
    if (pkt->datalen) {
        newpkt->datalen = pkt->datalen;
        CopyMem(newpkt->data, pkt->data, pkt->datalen);
    }

    return newpkt;
}


CHAR*
IpAddrToString(
    IPADDR ipaddr,
    CHAR* buf,
    INT buflen
    )

/*++

Routine Description:

    Convert an IP address to an ASCII character string

Arguments:

    ipaddr - Specifies the IP address in question
    buf - Output buffer
    buflen - Size of the output buffer

Return Value:

    Points to the converted IP address string

--*/

{
    // The longest string is xxx.xxx.xxx.xxx
    CHAR tmpbuf[16];
    INT i, j, q, r, d;
    CHAR* s;
    CHAR* p;

    // If the caller's buffer is large enough, then we'll
    // use it directly. Otherwise, we'll use a temporary
    // stack buffer first and then copy the result into
    // the caller's buffer.
    p = s = (buflen >= sizeof(tmpbuf)) ? buf : tmpbuf;

    for (i=0; i < 4; i++) {
        r = ipaddr & 0xff;
        ipaddr = ipaddr >> 8;   // assume little-endian here
        d = 100;
        for (j=0; j < 3; j++) {
            q = r / d;
            r = r % d;
            d /= 10;
            if (q || j == 2 || (p != s && p[-1] != '.'))
                *p++ = (CHAR) (q + '0');
        }
        *p++ = '.';
    }
    p[-1] = 0;

    if (s != buf) {
        CopyMem(buf, s, min(buflen, p-s));
    }
    return buf;
}


BOOL
IpAddrFromString(
    const CHAR* str,
    IPADDR* ipaddr
    )

/*++

Routine Description:

    Convert the string representation of IP address to its binary form.
    Following formats are recognized:
        a.b.c.d     8-8-8-8
        a.b.c       8-8-16
        a.b         8-24
        a           32
    Each field can be in decimal, octal, or hex format.

Arguments:

    str - Points to the IP address string to be converted
    ipaddr - Returns the binary representation of the IP address
        (in network byte order)

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    ULONG fields[4], addr;
    UINT fieldcnt = 0;
    const UCHAR* p = (const UCHAR*) str;

    // NOTE: We don't handle overflow conditions.

    while (TRUE) {
        // skip leading spaces
        while (*p == ' ') p++;
        if (fieldcnt >= 4 || *p == 0) break;
    
        addr = 0;
        if (*p == '0' && (p[1] == 'x' || p[1] == 'X')) {
            // hex number
            const UCHAR* q = (p += 2);

            while (TRUE) {
                if (*p >= '0' && *p <= '9')
                    addr = (addr << 4) + (*p - '0');
                else if (*p >= 'a' && *p <= 'f')
                    addr = (addr << 4) + 10 + (*p - 'a');
                else if (*p >= 'A' && *p <= 'F')
                    addr = (addr << 4) + 10 + (*p - 'A');
                else
                    break;
                p++;
            }

            if (q == p) return FALSE;

        } else if (*p == '0') {
            // octal number
            do {
                addr = (addr << 3) + (*p - '0');
                p++;
            } while (*p >= '0' && *p <= '7');
        } else if (*p >= '1' && *p <= '9') {
            // decimal number
            do {
                addr = addr*10 + (*p - '0');
                p++;
            } while (*p >= '0' && *p <= '9');
        } else {
            // invalid character
            break;
        }

        // skip trailing spaces and . separator
        while (*p == ' ') p++;
        if (*p == '.') p++;

        fields[fieldcnt++] = addr;
    }

    if (*p) return FALSE;

    switch (fieldcnt) {
    case 1:
        addr = fields[0];
        break;
    case 2:
        addr = ((fields[0] & 0xff) << 24) |
               (fields[1] & 0xffffff);
        break;
    case 3:
        addr = ((fields[0] & 0xff) << 24) |
               ((fields[1] & 0xff) << 16) |
               (fields[2] & 0xffff);
        break;
    case 4:
        addr = ((fields[0] & 0xff) << 24) |
               ((fields[1] & 0xff) << 16) |
               ((fields[2] & 0xff) <<  8) |
               (fields[3] & 0xff);
        break;

    default:
        addr = 0;
        break;
    }

    *ipaddr = HTONL(addr);
    return TRUE;
}


IPADDR
XnetGetDefaultSubnetMask(
    IPADDR ipaddr
    )

/*++

Routine Description:

    Return the default subnet mask for a given IP address

Arguments:

    ipaddr - Specifies the IP address

Return Value:

    Default subnet mask for the specified address

--*/

{
    IPADDR mask;

    if (IS_CLASSA_IPADDR(ipaddr))
        mask = CLASSA_NETMASK;
    else if (IS_CLASSB_IPADDR(ipaddr))
        mask = CLASSB_NETMASK;
    else if (IS_CLASSC_IPADDR(ipaddr))
        mask = CLASSC_NETMASK;
    else if (IS_CLASSD_IPADDR(ipaddr))
        mask = CLASSD_NETMASK;
    else {
        WARNING_("Invalid host IP address: %s", IPADDRSTR(ipaddr));
        mask = 0;
    }

    return mask;
}


//
// Pseudo-random number generator seed
//
ULONG XnetRandSeed;

VOID
XnetInitRandSeed()

/*++

Routine Description:

    Initialize the pseudo random number generator seed

Arguments:

    NONE

Return Value:

    NONE

Note:

    Our initial random number seed is based on processor's timestamp count
    and the system real-time clock. This will be later modified by the
    Ethernet hardware ID.

--*/

{
    LARGE_INTEGER currentTime;

    __asm {
        __emit  0x0f                // rdtsc
        __emit  0x31
        mov     XnetRandSeed, eax
    }

    KeQuerySystemTime(&currentTime);
    XnetRandSeed = (XnetRandSeed ^ currentTime.LowPart) & 0x7fffffff;
}

