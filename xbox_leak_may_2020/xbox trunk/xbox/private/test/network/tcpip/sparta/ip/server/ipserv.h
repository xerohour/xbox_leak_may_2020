#ifndef __IPSERV_H__
#define __IPSERV_H__

//==================================================================================
// Includes
//==================================================================================

#include <netsync.h>
#include "ipmsg.h"
#include "spartaapi.h"

//==================================================================================
// Defines
//==================================================================================

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

// Net attack values

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

//==================================================================================
// Typedefs
//==================================================================================

// Session info structure
typedef struct _SESSION_INFO
{
	CInterfaceList *pInterfaceList;
	CInterface *pInterface;
	HANDLE hNetsyncObject;
	BYTE byClientCount;
	DWORD *ClientAddrs;
	WORD LowPort;
	WORD HighPort;
	CHAR szServerIPAddr[16];
	PIP_TESTREQ pCurrentRequest;
} SESSION_INFO, *PSESSION_INFO;

// IP test function type
typedef BOOL (*PIP_SERV_FUNCTION) (DWORD FromAddr, PSESSION_INFO pSessionInfo);

//==================================================================================
// Prototypes 
//==================================================================================

// Test case functions
BOOL IPVersionSixServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPVersionBadServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPHeaderLenBelowMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPHeaderLenMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPHeaderLenAboveTotalLenServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPHeaderLenAboveAvailServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPTosNormalServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPTosNetControlServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPTosLowDelayServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPTosHighThroughputServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPTosHighReliabilityServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPTosReservedServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPLengthBelowMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPLengthAtMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPLengthAboveMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPLengthAboveTotalServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPLengthBelowTotalServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPLengthMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPIDMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPIDMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFlagReservedSetServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFlagDontFragAndMoreFragsServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPTtlMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPTtlMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPProtocolUnsupportedServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPProtocolICMPTooSmallServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPProtocolIGMPTooSmallServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPProtocolTCPTooSmallServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPProtocolUDPTooSmallServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPProtocolUnsupportedTooSmallServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPChecksumBadServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPOptionsAboveAvailServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPOptionsExactSizeServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPOptionsEndOfOpsLastServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPOptionsEndOfOpsFirstServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragFullReverseServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragMidReverseServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragLastInSecondServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragMixedSizeServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragOneHundredServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragMultipleLastFragmentServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragOverlappingFragmentsServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragMaxDatagramSizeServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragMaxReassemblySizeServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragAboveMaxReassemblySizeServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragMaxSimulReassemblyServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragAboveMaxSimulReassemblyServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPFragOversizedFragmentsServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPAttackTeardropServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPAttackNewTeardropServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPAttackImpTeardropServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPAttackSynDropServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPAttackBonkServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPAttackZeroLenOptServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL IPAttackNesteaServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
#endif
