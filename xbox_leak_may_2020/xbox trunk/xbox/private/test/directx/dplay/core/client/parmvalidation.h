#ifndef __DNETRSLT_PROCEDURAL_PARMV__
#define __DNETRSLT_PROCEDURAL_PARMV__

#include "wrappeer.h"

namespace DPlayCoreNamespace {


//==================================================================================
// Defines
//==================================================================================

// Makes sure nobody touched the memory when they weren't supposed to.
#define DONT_TOUCH_MEMORY_PATTERN	0xBADAB00F

#define BUFFERPADDING_SIZE			64


#define NONLOCAL_CONTEXT			((PVOID) 0xD00FD00D)







//==================================================================================
// External Prototypes
//==================================================================================
HRESULT ParmVCompareAppDesc(HANDLE hLog,
							PDPN_APPLICATION_DESC pdpnadCompare,
							PDPN_APPLICATION_DESC pdpnadExpected);

HRESULT ParmVCompareGroupInfo(HANDLE hLog,
							  PDPN_GROUP_INFO pdpngiCompare,
							  PDPN_GROUP_INFO pdpngiExpected);

HRESULT ParmVComparePlayerInfo(HANDLE hLog,
							   PDPN_PLAYER_INFO pdpnpiCompare,
							   PDPN_PLAYER_INFO pdpnpiExpected);

HRESULT ParmVCreatePeerHost(HANDLE hLog,
							PFNDPNMESSAGEHANDLER pfn,
							PVOID pvContext,
							PDPN_APPLICATION_DESC pdpnad,
							PVOID pvPlayerContext,
							PWRAPDP8PEER* ppDP8PeerHost,
							PDIRECTPLAY8ADDRESS* ppDP8AddressHost);

HRESULT ParmVCreateAndConnectClient(HANDLE hLog,
									PFNDPNMESSAGEHANDLER pfn,
									PVOID pvContext,
									PWRAPDP8SERVER pDP8Server,
									PDPN_APPLICATION_DESC pdpnad,
									HANDLE hClientCreatedEvent,
									PWRAPDP8CLIENT* ppDP8Client);

HRESULT ParmVCreateServer(HANDLE hLog,
						PFNDPNMESSAGEHANDLER pfn,
						PVOID pvContext,
						PDPN_APPLICATION_DESC pdpnad,
						DPNID* pdpnidPlayerContext,
						PWRAPDP8SERVER* ppDP8Server,
						PDIRECTPLAY8ADDRESS* ppDP8AddressHost);

HRESULT ParmVNoMessagesDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT ParmVNewPlayerDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);

BOOL StringCmpAToU(HANDLE hLog, WCHAR* wszUnicodeString, char* szANSIString, BOOL fMatchCase);

VOID ClearDoWorkList(DP_DOWORKLIST *pDoWorkList);

} // namespace DPlayCoreNamespace

#endif //__DNETRSLT_PROCEDURAL_PARMV__
