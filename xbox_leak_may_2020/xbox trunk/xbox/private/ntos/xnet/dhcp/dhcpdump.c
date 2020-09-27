/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    dhcpdump.c

Abstract:

    Dump DHCP message content to the debugger

Revision History:

    05/25/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Whether to dump incoming and outgoing DHCP messages
//
#if DBG

INT _dhcpDumpFlag = FALSE;

PRIVATE VOID
DhcpDumpByteArray(
    const BYTE* buf,
    UINT buflen
    )

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
        DbgPrint("%02x", *buf);
    }
}


PRIVATE VOID
DhcpDumpOption(
    IN const BYTE* buf,
    IN UINT buflen,
    OUT BYTE* overload
    )

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
            DbgPrint("    PAD\n");
            buflen--, buf++;
            continue;
        }

        // Check option length
        if (buflen < 2 || buflen-2 < (len = buf[1])) goto badopt;

        switch (tag = buf[0]) {
        case DHCPOPT_DHCP_MESSAGE_TYPE:
            if (len != 1) goto badopt;
            val = buf[2];
            if (val > 0 && val < ARRAYCOUNT(dhcpMessageTypeStrs)) {
                DbgPrint("    %s", dhcpMessageTypeStrs[val]);
            } else {
                DbgPrint("    Unknown DHCP message type: %d", val);
            }
            break;

        case DHCPOPT_FIELD_OVERLOAD:
            if (len != 1) goto badopt;
            DbgPrint("    OVERLOAD: %d", buf[2]);
            if (overload)
                *overload = buf[2];
            else
                DbgPrint(" !!!");
            break;

        case DHCPOPT_REQUESTED_IPADDR:
        case DHCPOPT_SERVERID:
            if (len != 4) goto badopt;
            DbgPrint("    %s %d.%d.%d.%d",
                (tag == DHCPOPT_SERVERID) ? "SERVERID" : "REQUEST IP ADDR",
                buf[2], buf[3], buf[4], buf[5]);
            break;

        default:
            DbgPrint("    %d - ", tag);
            DhcpDumpByteArray(buf+2, len);
            break;
        }

        DbgPrint("\n");
        buf += len+2;
        buflen -= len+2;
    }

    if (buflen == 0) {
        DbgPrint("!!! Missing 'end' option\n");
    } else {
        do {
            buflen--, buf++;
        } while (buflen && *buf == 0);

        if (buflen != 0)
            DbgPrint("!!! Extra data after 'end' option\n");
    }
    return;

badopt:
    DbgPrint("!!! Bad DHCP option data\n");
}


VOID
DhcpDumpMessage(
    const DhcpMessage* msg,
    UINT msglen
    )

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

    if (!_dhcpDumpFlag) return;

    //
    // Sanity check
    //
    if (msglen < DHCPHDRLEN) {
        DbgPrint("!!! DHCP message too small: %d bytes\n", msglen);
        return;
    }

    //
    // Dump out fixed header information
    //
    DbgPrint("DHCP message: ");
    if (msg->op == BOOTREQUEST)
        DbgPrint("BOOTREQUEST\n");
    else if (msg->op == BOOTREPLY)
        DbgPrint("BOOTREPLY\n");
    else
        DbgPrint("%d\n", msg->op);

    DbgPrint("  htype: %d\n", msg->htype);
    if (msg->hlen > sizeof(msg->chaddr)) {
        DbgPrint("!!! Invalid hardware address length: %d\n", msg->hlen);
    } else if (msg->hlen) {
        DbgPrint("  chaddr: ");
        DhcpDumpByteArray(msg->chaddr, msg->hlen);
        DbgPrint("\n");
    }

    DbgPrint("  hops: %d\n", msg->hops);
    DbgPrint("  xid: 0x%08x\n", NTOHL(msg->xid));
    DbgPrint("  secs: %d\n", NTOHS(msg->secs));
    DbgPrint("  flags: 0x%04x\n", NTOHS(msg->flags));
    DbgPrint("  ciaddr: %s\n", IPADDRSTR(msg->ciaddr));
    DbgPrint("  yiaddr: %s\n", IPADDRSTR(msg->yiaddr));
    DbgPrint("  siaddr: %s\n", IPADDRSTR(msg->siaddr));
    DbgPrint("  giaddr: %s\n", IPADDRSTR(msg->giaddr));

    //
    // Dump options
    //
    option = msg->options;
    msglen -= DHCPHDRLEN;

    if (msglen < DHCPCOOKIELEN ||
        !EqualMem(option, DhcpMagicCookie, DHCPCOOKIELEN)) {
        DbgPrint("!!! Invalid DHCP magic cookie\n");
        return;
    }
    msglen -= DHCPCOOKIELEN;
    option += DHCPCOOKIELEN;

    DbgPrint("  options:\n");
    DhcpDumpOption(option, msglen, &overload);

    if (overload & 1) {
        DbgPrint("  overload options (file):\n");
        DhcpDumpOption(msg->file, sizeof(msg->file), NULL);
    }

    if (overload & 2) {
        DbgPrint("  overload options (sname):\n");
        DhcpDumpOption(msg->sname, sizeof(msg->sname), NULL);
    }
}

#endif // DBG

