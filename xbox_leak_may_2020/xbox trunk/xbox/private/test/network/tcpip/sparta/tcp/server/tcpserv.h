#ifndef __TCPSERV_H__
#define __TCPSERV_H__

//==================================================================================
// Includes
//==================================================================================

#include <netsync.h>
#include "tcpmsg.h"
#include "spartaapi.h"

//==================================================================================
// Defines
//==================================================================================

#define	MSS_DEFAULT	1460

// This might need to be changed from time to time if another host is added
// to the subnet with the same address or if the test lab moves to another subnet
#define LOCAL_SPOOF_IP_BASE_STRING TEXT("157.56.11.")

// This is the base host that we will spoof as
// We will loop around at 254, giving us support for 4 simultaneous hosts
#define LOCAL_SPOOF_IP_BASE_HOST 250

//==================================================================================
// Typedefs
//==================================================================================

// Session info structure
typedef struct _SESSION_INFO
{
	CInterfaceList *pInterfaceList;
	CInterface *pInterface;
	CAutoArp *pAutoArp;
    CPattern *pProtocolPattern;
    CPattern *pDestAddrPattern;
    CPattern *pSrcIpPattern;
    CPattern *pDestIpPattern;
    CPattern *pProtocolIpPattern;
	HANDLE hNetsyncObject;
	BYTE byClientCount;
	DWORD *ClientAddrs;
	WORD LowPort;
	WORD HighPort;
	CHAR szServerIPAddr[16];
	CHAR szClientIPAddr[16];
	PTCP_TESTREQ pCurrentRequest;
} SESSION_INFO, *PSESSION_INFO;

// TCP test function type
typedef BOOL (*PTCP_SERV_FUNCTION) (DWORD FromAddr, PSESSION_INFO pSessionInfo);

//==================================================================================
// Prototypes 
//==================================================================================

// Test case functions
BOOL TCPSeqNumMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPSeqNumMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPDataOffBelowMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPDataOffAboveAvailServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
#if 0
BOOL TCPReservedSetServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPWindowMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPWindowMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPChecksumBadServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPUrgentAfterEndServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPUrgentAtEndServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPUrgentBeforeEndServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPUrgentMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPUrgentNoFlagServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPOptCutoffServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPOptAtEndServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPOptBeforeEndServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPOptNoOptsServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPOptMaxOptsServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPOptMaxOptLenServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPOptEndOfOptsServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPOptMssZeroServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPOptMssOneServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPOptMssMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPConnSynAndNoAckServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPConnExtraSynServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPConnAckWrongSynServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPConnSynAckWrongSynServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPConnAckNoConnServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPConnSynAckListenerServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL TCPShutFinAndNoAckServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
#endif

#endif
