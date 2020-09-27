// AttackConstants.h : header file containing constants for SPARTA net attack cases
//

#ifndef _ATTACKCONSTANTS_H_
#define _ATTACKCONSTANTS_H_

typedef BOOL (*PATTACK_FUNCTION) (CInterface *pInterface, CHAR *szDestMac);

// ----------------------------------------
// ESP attack constants
// ----------------------------------------
#define ESPATTACK_MAXTEST 14

#define LANEXCHANGE_SIZE 164
#define TITLEENCRYPT_SIZE 740
#define TCP_PROTOCOL_VALUE 6
#define UDP_PROTOCOL_VALUE 17
#define UNKNOWN_PROTOCOL_VALUE 255
#define CBDHLEN 96
#define ESP_ITERATIONS 100

// ----------------------------------------
// TCP attack constants
// ----------------------------------------
#define TCPATTACK_MAXTEST 4

#define MSS_DEFAULT 1418
#define TCPHEADER_SIZE 20
#define TCPPORT_DEST 3074
#define TCPPORT_SOURCE 3074

// ----------------------------------------
// UDP attack constants
// ----------------------------------------
#define UDPATTACK_MAXTEST 14

#define PROTOCOL_UDP 17
#define UDPHEADER_SIZE 8
#define UDPPORT_DEST 3074
#define UDPPORT_SOURCE 3074

// ----------------------------------------
// IP attack constants
// ----------------------------------------
#define IPATTACK_MAXTEST 55

#define IPADDR_DEST "0.0.0.1"
#define IPADDR_SOURCE "0.0.0.1"
#define IPADDR_LOOPBACK "127.0.0.1"
#define	IPHEADER_SIZE		20
#define IPVERSION_SIX 6
#define IPVERSION_BAD 15
#define IPVERSION_DEFAULT 4
#define IPHEADER_DEFAULT 20
#define IPHEADER_BELOWMIN 4
#define IPHEADER_MAX 60
#define IPTOS_NORMAL 0
#define IPTOS_NETCONTROL 224
#define IPTOS_LOWDELAY 16
#define IPTOS_HIGHTHRUPUT 8
#define IPTOS_HIGHRELI 4
#define IPTOS_RESERVED 1
#define IPTOS_DEFAULT IPTOS_NORMAL
#define IPLENGTH_DEFAULT UDPHEADER_SIZE + 1
#define IPLENGTH_BELOWMIN 10
#define IPLENGTH_ATMIN 20
#define IPLENGTH_MAX 1500
#define IPID_MIN 0
#define IPID_MAX MAXWORD
#define IPID_DEFAULT 100
#define IPFLAG_RESERVED 4
#define IPFLAG_DONTFRAG 2
#define IPFLAG_MOREFRAGS 1
#define IPTTL_MIN 0
#define IPTTL_MAX 255
#define IPTTL_DEFAULT 64
#define IPPROTOCOL_UNSUPPORTED 255
#define IPPROTOCOL_ICMP 1
#define IPPROTOCOL_IGMP 2
#define IPPROTOCOL_TCP 6
#define IPPROTOCOL_UDP 17
#define OVERSIZED_FRAG_COUNT 99
#define DEFAULT_FRAG_COUNT 4
#define TEARDROP_SIZE 28
#define TEARDROP_XVALUE 3
#define NEWTEAR_SIZE 20
#define NEWTEAR_XVALUE 3
#define SYNDROP_SIZE 20
#define SYNDROP_XVALUE 3
#define BONK_SIZE 20
#define BONK_XVALUE 3
#define SIM_REASM_MAX 4
#define MAX_SIMUL_PORT_OFFSET 5
#define ABOVE_MAX_SIMUL_PORT_OFFSET 5

#endif // _ATTACKCONSTANTS_H_
