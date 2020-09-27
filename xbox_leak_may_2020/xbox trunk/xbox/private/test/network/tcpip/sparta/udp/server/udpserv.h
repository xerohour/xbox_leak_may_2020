#ifndef __UDPSERV_H__
#define __UDPSERV_H__

//==================================================================================
// Includes
//==================================================================================

#include <netsync.h>
#include "udpmsg.h"
#include "spartaapi.h"

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
	PUDP_TESTREQ pCurrentRequest;
} SESSION_INFO, *PSESSION_INFO;

// UDP test function type
typedef BOOL (*PUDP_SERV_FUNCTION) (DWORD FromAddr, PSESSION_INFO pSessionInfo);

//==================================================================================
// Prototypes 
//==================================================================================

// Test case functions
BOOL UDPSourceMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPSourceMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPSourceZeroServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPDestMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPDestMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPDestZeroServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPLenBelowMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPLenAtMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPLenAboveMinServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPLenAboveTotalServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPLenBelowTotalServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPLenMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPCheckOnesServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);
BOOL UDPCheckZerosServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);

#endif
