#ifndef __DPSERV_H__
#define __DPSERV_H__

//==================================================================================
// Includes
//==================================================================================

#include <netsync.h>
#include "dpmsg.h"

#define LOST_HOST_TIME  (180000)

//==================================================================================
// Typedefs
//==================================================================================
typedef struct _DPSYNC_LIST_ENTRY {
	DWORD dwMinMachinesRequired;
	DWORD dwCurrentMachines;
	CHAR szSyncDataName[16];
	DWORD dwSyncDataSize;
	PBYTE pbSyncData;
	BOOL *pWaitStatus;
	struct _DPSYNC_LIST_ENTRY		*pNext;
} DPSYNC_LIST_ENTRY, *PDPSYNC_LIST_ENTRY;

// Session info structure
typedef struct _DPCORE_SESSION_INFO
{
	PDPSYNC_LIST_ENTRY pSyncListHead;
	HANDLE hNetsyncObject;
	DP_HOSTINFO *pHostInfo;
	BYTE byClientCount;
	DWORD *ClientAddrs;
	BOOL *pTestStatus;
	CHAR szServerIPAddr[16];
	WORD LowPort;
	WORD HighPort;
} DPCORE_SESSION_INFO, *PDPCORE_SESSION_INFO;

// Session info structure
typedef struct _DPSTRESS_SESSION_INFO
{
	HANDLE hNetsyncObject;
	DP_HOSTINFO *pHostInfo;
	BYTE byClientCount;
	DWORD *ClientAddrs;
	DWORD dwLastHostPing;
	WORD LowPort;
	WORD HighPort;
} DPSTRESS_SESSION_INFO, *PDPSTRESS_SESSION_INFO;

// DP test function type
typedef BOOL (*PDP_SERV_FUNCTION) (DWORD FromAddr, PDPCORE_SESSION_INFO pSessionInfo);

//==================================================================================
// Prototypes 
//==================================================================================

// Test case functions
//BOOL DPSourceMaxServ (DWORD FromAddr, PSESSION_INFO pSessionInfo);

#endif
