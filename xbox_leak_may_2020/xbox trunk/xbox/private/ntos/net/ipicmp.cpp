// ---------------------------------------------------------------------------------------
// ipicmp.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

// ---------------------------------------------------------------------------------------
// CIcmpHdr
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_ICMP

#include <pshpack1.h>

class CIcmpHdr
{

    // Definitions -----------------------------------------------------------------------

    #define ICMPTYPE_ECHO_REPLY                 0
    #define ICMPTYPE_DESTINATION_UNREACHABLE    3
    #define   ICMPCODE_NET_UNREACHABLE          0
    #define   ICMPCODE_HOST_UNREACHABLE         1
    #define   ICMPCODE_PROTOCOL_UNREACHABLE     2
    #define   ICMPCODE_PORT_UNREACHABLE         3
    #define   ICMPCODE_CANT_FRAGMENT_WITH_DF    4
    #define   ICMPCODE_SRCROUTE_FAILED          5
    #define   ICMPCODE_DEST_NET_UNKNOWN         6
    #define   ICMPCODE_DEST_HOST_UNKNOWN        7
    #define   ICMPCODE_SRC_HOST_ISOLATED        8
    #define   ICMPCODE_DEST_NET_PROHIBITED      9
    #define   ICMPCODE_DEST_HOST_PROHIBITED     10
    #define   ICMPCODE_NET_UNREACHABLE_TOS      11
    #define   ICMPCODE_HOST_UNREACHABLE_TOS     12
    #define ICMPTYPE_SOURCE_QUENCH              4
    #define ICMPTYPE_REDIRECT                   5
    #define   ICMPCODE_REDIRECT_NET             0
    #define   ICMPCODE_REDIRECT_HOST            1
    #define   ICMPCODE_REDIRECT_NET_TOS         2
    #define   ICMPCODE_REDIRECT_HOST_TOS        3
    #define ICMPTYPE_ECHO_REQUEST               8
    #define ICMPTYPE_ROUTER_ADVERTISEMENT       9
    #define ICMPTYPE_ROUTER_SOLICIATION         10
    #define ICMPTYPE_TIME_EXCEEDED              11
    #define   ICMPCODE_TTL_EXCEEDED             0
    #define   ICMPCODE_REASSEMBLY_TIMEOUT       1
    #define ICMPTYPE_PARAMETER_PROBLEM          12
    #define   ICMPCODE_PTR_INDICATES_ERROR      0
    #define   ICMPCODE_MISSING_REQD_OPTION      1
    #define ICMPTYPE_TIMESTAMP_REQUEST          13
    #define ICMPTYPE_TIMESTAMP_REPLY            14
    #define ICMPTYPE_INFORMATION_REQUEST        15
    #define ICMPTYPE_INFORMATION_REPLY          16
    #define ICMPTYPE_ADDRESSMASK_REQUEST        17
    #define ICMPTYPE_ADDRESSMASK_REPLY          18

    // Trace Support ---------------------------------------------------------------------

public:

    char * Str()
    {
        switch (_bType)
        {
            case ICMPTYPE_ECHO_REPLY:
                return("Echo Reply");
                break;
            case ICMPTYPE_DESTINATION_UNREACHABLE:
            {
                switch (_bCode)
                {
                    case ICMPCODE_NET_UNREACHABLE:
                        return("Net Unreachable");
                    case ICMPCODE_HOST_UNREACHABLE:
                        return("Host Unreachable");
                    case ICMPCODE_PROTOCOL_UNREACHABLE:
                        return("Protocol Unreachable");
                    case ICMPCODE_PORT_UNREACHABLE:
                        return("Port Unreachable");
                    case ICMPCODE_CANT_FRAGMENT_WITH_DF:
                        return("Can't Fragment With DF");
                    case ICMPCODE_SRCROUTE_FAILED:
                        return("Source Route Failed");
                    case ICMPCODE_DEST_NET_UNKNOWN:
                        return("Destination Net Unknown");
                    case ICMPCODE_DEST_HOST_UNKNOWN:
                        return("Destination Host Unknown");
                    case ICMPCODE_SRC_HOST_ISOLATED:
                        return("Source Host Isolated");
                    case ICMPCODE_DEST_NET_PROHIBITED:
                        return("Destination Net Prohibited");
                    case ICMPCODE_NET_UNREACHABLE_TOS:
                        return("Network Unreachable For Tos");
                    case ICMPCODE_HOST_UNREACHABLE_TOS:
                        return("Host Unreachable For Tos");
                    default:
                        return("Destination Unreachable");
                }
            }
            case ICMPTYPE_SOURCE_QUENCH:
                return("Source Quench");
                break;
            case ICMPTYPE_REDIRECT:
            {
                switch (_bCode)
                {
                    case ICMPCODE_REDIRECT_NET:
                        return("Redirect Datagrams For Net");
                    case ICMPCODE_REDIRECT_HOST:
                        return("Redirect Datagrams For Host");
                    case ICMPCODE_REDIRECT_NET_TOS:
                        return("Redirect Datagrams For Net And Tos");
                    case ICMPCODE_REDIRECT_HOST_TOS:
                        return("Redirect Datagrams For Host And Tos");
                    default:
                        return("Redirect");
                }
            }
            case ICMPTYPE_ECHO_REQUEST:
                return("Echo Request");
                break;
            case ICMPTYPE_ROUTER_ADVERTISEMENT:
                return("Router Advertisement");
                break;
            case ICMPTYPE_TIME_EXCEEDED:
            {
                switch (_bCode)
                {
                    case ICMPCODE_TTL_EXCEEDED:
                        return("Ttl Exceeded");
                    case ICMPCODE_REASSEMBLY_TIMEOUT:
                        return("Reassembly Timeout");
                    default:
                        return("Time Exceeded");
                }
            }
            case ICMPTYPE_PARAMETER_PROBLEM:
            {
                switch (_bCode)
                {
                    case ICMPCODE_MISSING_REQD_OPTION:
                        return("Missing Required Option");
                    default:
                        return("Parameter Problem");
                }
            }
            case ICMPTYPE_TIMESTAMP_REQUEST:
                return("Timestamp Request");
            case ICMPTYPE_TIMESTAMP_REPLY:
                return("Timestamp Reply");
            case ICMPTYPE_INFORMATION_REQUEST:
                return("Information Request");
            case ICMPTYPE_INFORMATION_REPLY:
                return("Information Reply");
            case ICMPTYPE_ADDRESSMASK_REQUEST:
                return("Address Mask Request");
            case ICMPTYPE_ADDRESSMASK_REPLY:
                return("Address Mask Reply");
            default:
                return("Unknown");
        }
    }

    // Data ------------------------------------------------------------------------------

public:

    BYTE            _bType;         // One of ICMPTYPE_* above
    BYTE            _bCode;         // One of ICMPCODE_* above
    WORD            _wChecksum;     // Checksum
    DWORD           _dwData;        // Data (depends on _bType)

};

#include <poppack.h>

// ---------------------------------------------------------------------------------------
// CXnIp::IcmpRecv
// ---------------------------------------------------------------------------------------

void CXnIp::IcmpRecv(CPacket* ppkt, CIpHdr * pIpHdr, void * pvData, UINT cbData)
{
    ICHECK(IP, UDPC|SDPC);

    if (cbData < sizeof(CIcmpHdr))
    {
        TraceSz1(pktWarn, "[DISCARD] ICMP header doesn't fit in the packet (%d bytes)", cbData);
        return;
    }

    if (tcpipxsum(0, pvData, cbData) != 0xFFFF)
    {
        TraceSz(pktWarn, "[DISCARD] ICMP header checksum failed");
        return;
    }

    CIcmpHdr * pIcmpHdr = (CIcmpHdr *)pvData;

    TraceSz4(pktRecv, "[ICMP <%d/%d> %s][cb=%d]",
             pIcmpHdr->_bType, pIcmpHdr->_bCode, pIcmpHdr->Str(),
             cbData - sizeof(CIcmpHdr));

    if (ppkt->TestFlags(PKTF_RECV_BROADCAST|PKTF_RECV_LOOPBACK))
    {
        TraceSz1(pktWarn, "[DISCARD] ICMP packet received via %s",
                 ppkt->TestFlags(PKTF_RECV_BROADCAST) ? "broadcast" : "loopback");
        return;
    }

#ifdef XNET_FEATURE_ROUTE

    if (    pIcmpHdr->_bType == ICMPTYPE_REDIRECT
        &&  (   pIcmpHdr->_bCode == ICMPCODE_REDIRECT_NET
             || pIcmpHdr->_bCode == ICMPCODE_REDIRECT_HOST
             || pIcmpHdr->_bCode == ICMPCODE_REDIRECT_NET_TOS
             || pIcmpHdr->_bCode == ICMPCODE_REDIRECT_HOST_TOS))
    {
        pvData  = (BYTE *)pvData + sizeof(CIcmpHdr);
        cbData -= sizeof(CIcmpHdr);

        if (cbData < sizeof(CIpHdr))
        {
            TraceSz(pktWarn, "[DISCARD] ICMP embedded IP header doesn't fit in the packet");
            return;
        }

        CIpHdr * pIpHdrIcmp = (CIpHdr *)pvData;
        UINT cbIpHdrIcmp = pIpHdrIcmp->VerifyHdrLen();

        if (cbIpHdrIcmp == 0)
        {
            TraceSz(pktWarn, "[DISCARD] ICMP embedded IP header version is incorrect");
            return;
        }

        if (cbData < (cbIpHdrIcmp + 8))
        {
            TraceSz(pktWarn, "[DISCARD] ICMP embedded IP header and 64-bits of data doesn't fit in the packet");
            return;
        }

        // The embedded IP header is supposed to have originated from this host, and
        // we would never have added IP options, so we can check that here.

        if (cbIpHdrIcmp != sizeof(CIpHdr))
        {
            TraceSz(pktWarn, "[DISCARD] ICMP embedded IP header has options");
            return;
        }

        TraceSz3(pktRecv, "[REDIRECT] Rerouting %s from gateway %s to gateway %s",
                 pIpHdrIcmp->_ipaDst.Str(), pIpHdr->_ipaSrc.Str(), CIpAddr(pIcmpHdr->_dwData).Str());

        RouteRedirect(pIpHdrIcmp->_ipaDst, pIpHdr->_ipaSrc, CIpAddr(pIcmpHdr->_dwData));
        return;
    }

#endif

    if (pIcmpHdr->_bType == ICMPTYPE_ECHO_REQUEST)
    {
        if (!pIpHdr->_ipaSrc.IsValidUnicast())
        {
            TraceSz1(pktWarn, "[DISCARD] ICMP echo from bad source address %s",
                     pIpHdr->_ipaSrc.Str());
            return;
        }

        if (pIpHdr->GetOptLen() > 0)
        {
            TraceSz(pktWarn, "[DISCARD] ICMP echo packet with IP header options");
            return;
        }

        // Don't allow Ping'er to make us consume gratuitous amounts of pool memory

        if (cbData > sizeof(CIcmpHdr) + 64)
        {
            TraceSz1(pktWarn, "[DISCARD] ICMP echo request with too much optional data (%d)",
                     cbData - sizeof(CIcmpHdr));
            return;
        }

        CPacket * ppktReply = PacketAlloc(PTAG_CIcmpEchoPacket,
                                          PKTF_TYPE_IP|PKTF_XMIT_INSECURE|PKTF_POOLALLOC,
                                          cbData);

        if (ppktReply == NULL)
        {
            TraceSz(pktWarn, "[DISCARD] Out of memory allocating ICMP echo reply packet");
            return;
        }

        CIpHdr * pIpHdrReply = ppktReply->GetIpHdr();

        memcpy(pIpHdrReply, pIpHdr, sizeof(CIpHdr) + cbData);

        CIcmpHdr * pIcmpHdrReply = (CIcmpHdr *)(pIpHdrReply + 1);

        pIcmpHdrReply->_bType     = ICMPTYPE_ECHO_REPLY;
        pIcmpHdrReply->_wChecksum = 0;
        pIcmpHdrReply->_wChecksum = (WORD)~tcpipxsum(0, pIcmpHdrReply, cbData);

        TraceSz(pktRecv, "[REPLY] Replying to ICMP echo request");

        IpFillAndXmit(ppktReply, pIpHdr->_ipaSrc, IPPROTOCOL_ICMP);
        return;
    }

    TraceSz3(pktWarn, "[DISCARD] No support ICMP <%d/%d> %s",
             pIcmpHdr->_bType, pIcmpHdr->_bCode, pIcmpHdr->Str());
}

#endif
