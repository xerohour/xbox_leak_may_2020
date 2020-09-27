/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: IPConstants.h                                                               *
* Description: This is the implementation of the IPHeader functions                  *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       deepakp      4/25/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_IPCONSTANTS_H__
#define __SPARTA_IPCONSTANTS_H__

#define ETHERNET_802_3_HEADER_LENGTH 14

#define LONGEST_POSSIBLE_IP_HEADER         60
#define IP_HEADER_LENGTH_WITOUT_OPTIONS    20
#define IPV4_ADDRESS_LENGTH                 4
#define IPV4_SOURCE_ADDRESS_OFFSET         12
#define IPV4_DESTINATION_ADDRESS_OFFSET    16

#define IPV6_ADDRESS_BYTES                 16      //IPV6_ADDRESS_LENGTH defined in ip6.h (in bits)
#define IPV6_SOURCE_ADDRESS_OFFSET          8
#define IPV6_DESTINATION_ADDRESS_OFFSET    24

typedef enum
{
    OFF = 0,
    ON,
} AUTO_CAL_TYPES;

// The type field of the various ip options as stored in the
// type field of the IPoptions in the IP Packet
#define IP_OPTIONS_END_OF_OPTIONLIST                    0
#define IP_OPTIONS_NO_OPERATION                                 1
#define IP_OPTIONS_RECORD_ROUTE                                 7
#define IP_OPTIONS_TIME_STAMP                                   68
#define IP_OPTIONS_BASIC_SECURITY                               130
#define IP_OPTIONS_LOOSE_SOURCE_RECORD_ROUTE    131
#define IP_OPTIONS_EXTENDED_SECURITY                    133
#define IP_OPTIONS_STREAM_IDENTIFIER                    136
#define IP_OPTIONS_STRICT_SOURCE_RECORD_ROUTE   137
#define IP_OPTIONS_UNKNOWN_TYPE                                 255


#define IPOPT_TS_TSONLY         0
#define IPOPT_TS_TSANDADDR      1
#define IPOPT_TS_PRESPEC        3

#define IP_OPTIONS_MINIMUM_LENGTH       1
#define IP_OPTIONS_MAXIMUM_LENGTH       40

#define IP_OPTIONS_MINIMUM_OFFSET       4

#define IPOPT_OPTVAL    0
#define IPOPT_OPTLEN    1
#define IPOPT_OPTOFFSET 2
#define IPOPT_OPT_FLAG_OFFSET   3

typedef unsigned long IPAddr;     // An IP address.

typedef unsigned char UCHAR;
typedef unsigned short USHORT;

#define MAX_SIZE_IP_STRING      16  // Null terminated
#define MAX_SIZE_IPV6_STRING    40

#define BROADCAST_ADDRESS "255.255.255.255"



///////////////////////////////////////////////////////////////
// ICMPv4 constants:      types & codes
///////////////////////////////////////////////////////////////

#define  ICMPv4_ECHO_REPLY_TYPE           0        // echo reply

#define  ICMPv4_DEST_UNR_TYPE             3        // destination unreachable:
// codes for DEST_UNR
#define  ICMPv4_NET_UNR_CODE              0        // net unreachable
#define  ICMPv4_HOST_UNR_CODE             1        // host unreachable
#define  ICMPv4_PROT_UNR_CODE             2        // protocol unreachable
#define  ICMPv4_PORT_UNR_CODE             3        // port unreachable
#define  ICMPv4_FRAG_DF_CODE              4        // fragmentation needed + DF
#define  ICMPv4_SR_FAIL_CODE              5        // source route failed
#define  ICMPv4_DST_NET_UNKNOWN_CODE      6        // dest network unknown
#define  ICMPv4_DST_HOST_UNKNOWN_CODE     7        // dest host unknown
#define  ICMPv4_SRC_HOST_ISOLATED_CODE    8        // source host isolated
#define  ICMPv4_NET_PROHIBITED_CODE       9        // communication with dest network administratively prohibited
#define  ICMPv4_HOST_PROHIBITED_CODE     10        // communication with dest host administratively prohibited
#define  ICMPv4_NET_UNR_FOR_TOS_CODE     11        // network unreachable for type of service
#define  ICMPv4_HOST_UNR_FOR_TOS_CODE    12        // host unreachable for type of service
// The following three codes not used by Microsoft
#define  ICMPv4_COMM_PROHIBITED_FILTER_CODE  13    // Communication prohibited filtering
#define  ICMPv4_HOST_PRECEDENCE_VOILATION_CODE 14  // host precedence voilation
#define  ICMPv4_PRECEDENCE_CUTOFF_CODE   15        // Precedence cutoff in effect


#define  ICMPv4_SRC_QUENCH_TYPE           4        // source quench

#define  ICMPv4_REDIRECT_TYPE             5        // redirect message
// codes for redirect
#define  ICMPv4_NET_RE_CODE               0        // redirect for network
#define  ICMPv4_HOST_RE_CODE              1        // redirect for host
#define  ICMPv4_TOSN_RE_CODE              2        // redirect for TOS/network
#define  ICMPv4_TOSH_RE_CODE              3        // redirect for TOS/host

#define  ICMPv4_ECHO_REQUEST_TYPE         8        // echo request
#define  ICMPv4_ROUTER_ADVERTISEMENT_TYPE 9        // router advertisement
#define  ICMPv4_ROUTER_SOLICITATION_TYPE 10        // router solicitation


#define  ICMPv4_TIME_EXCEEDED_TYPE       11        // time exceeded
// codes for time exceeded
#define  ICMPv4_TTL_X_CODE                0        // time-to-live exceeded
#define  ICMPv4_FRAG_X_CODE               1        // frag reassem time excluded

#define  ICMPv4_PARAMETER_TYPE           12        // parameter problem
#define  ICMPv4_PARAM_POINTER_CODE        0        // pointer indicates error
#define  ICMPv4_PARAM_OPTION_CODE         1        // required option missing

#define  ICMPv4_TIME_STAMP_TYPE          13        // timestamp request
#define  ICMPv4_STAMP_REPLY_TYPE         14        // timestamp reply

#define  ICMPv4_INFO_REQ_TYPE            15        // information request
#define  ICMPv4_INFO_REPLY_TYPE          16        // information reply

#define  ICMPv4_MASK_REQ_TYPE            17        // address mask request
#define  ICMPv4_MASK_REPLY_TYPE          18        // address mask reply

#define  ICMPv4_ILLEGAL_TYPE            100        // an arbitrary illegal value


// ICMP header length estimates

#define  ICMPv4_MINLEN         8
#define  ICMPv4_TSLEN         20
#define  ICMPv4_MASKLEN       12
#define  ICMPv4_ADVLENMIN     36

//  ICMPv6 constants are defined in tcpip\tpipv6\inc\icmp6.h

#define PROTOCOL_ICMP6        58


///////////////////////////////////////////////////////////
// IGMP constants
///////////////////////////////////////////////////////////


//
#define IGMPV1_MEMBERSHIP_QUERY_TYPE 0x1
#define IGMPV1_MEMBERSHIP_REPORT_TYPE 0x2
#define IGMP_DVMRP_TYPE 0x3

#define IGMPV2_MEMBERSHIP_QUERY_TYPE 0x11
#define IGMPV2_MEMBERSHIP_REPORT_TYPE 0x16
#define IGMP_LEAVE_GROUP_TYPE 0x17


#endif //__SPARTA_IPCONSTANTS_H__
