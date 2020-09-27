//-----------------------------------------------------------------------------
// File: peerfoot.cpp
//
// Desc: Basic executable to measure static and dynamic DirectPlay memory footprint
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <dplay8.h>
#include <dpaddr.h>

// Define only one of the following
#define NONETWORKING
//#define XNETONLY
//#define DIRECTPLAY_BASE
//#define DIRECTPLAY_PEERBASE
//#define DIRECTPLAY_PEERGROUP
//#define DIRECTPLAY_PEERFULL
//#define DIRECTPLAY_CLIENTBASE
//#define DIRECTPLAY_CLIENTFULL
//#define DIRECTPLAY_SERVERBASE
//#define DIRECTPLAY_SERVERGROUP
//#define DIRECTPLAY_SERVERFULL
//#define DIRECTPLAY_ADDRESS
//#define DIRECTPLAY_ADDRESSIP
//#define DIRECTPLAY_ALLYOURBASE
//#define DIRECTPLAY_ALLYOURGROUP
//#define DIRECTPLAY_ALLYOURFULL


#if defined(NONETWORKING)
#elif defined(XNETONLY)
#define INCLUDE_XNET_IMPORTS
#elif defined(DIRECTPLAY_BASE)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#elif defined(DIRECTPLAY_PEERBASE)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_PEER_BASE
#elif defined(DIRECTPLAY_PEERGROUP)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_PEER_GROUPS
#elif defined(DIRECTPLAY_PEERFULL)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_PEER_FULL
#elif defined(DIRECTPLAY_CLIENTBASE)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_CLIENT_BASE
#elif defined(DIRECTPLAY_CLIENTFULL)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_CLIENT_FULL
#elif defined(DIRECTPLAY_SERVERBASE)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_SERVER_BASE
#elif defined(DIRECTPLAY_SERVERGROUP)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_SERVER_GROUPS
#elif defined(DIRECTPLAY_SERVERFULL)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_SERVER_FULL
#elif defined(DIRECTPLAY_ADDRESS)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_ADDRESS_FULL
#elif defined(DIRECTPLAY_ADDRESSIP)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_ADDRESSIP_FULL
#elif defined(DIRECTPLAY_ALLYOURBASE)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_PEER_BASE
#define INCLUDE_DPLAY_CLIENT_BASE
#define INCLUDE_DPLAY_SERVER_BASE
#define INCLUDE_DPLAY_ADDRESS_FULL
#define INCLUDE_DPLAY_ADDRESSIP_FULL
#elif defined(DIRECTPLAY_ALLYOURGROUP)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_PEER_GROUPS
#define INCLUDE_DPLAY_CLIENT_BASE
#define INCLUDE_DPLAY_SERVER_GROUPS
#define INCLUDE_DPLAY_ADDRESS_FULL
#define INCLUDE_DPLAY_ADDRESSIP_FULL
#elif defined(DIRECTPLAY_ALLYOURFULL)
#define INCLUDE_XNET_IMPORTS
#define INCLUDE_DPLAY_GENERAL_IMPORTS
#define INCLUDE_DPLAY_PEER_FULL
#define INCLUDE_DPLAY_CLIENT_FULL
#define INCLUDE_DPLAY_SERVER_FULL
#define INCLUDE_DPLAY_ADDRESS_FULL
#define INCLUDE_DPLAY_ADDRESSIP_FULL
#else // default to NONETWORKING
#endif

//-----------------------------------------------------------------------------
// Name: main()
// Desc: everyone knows what main does...
//-----------------------------------------------------------------------------
void __cdecl main()
{
	HRESULT				hr = DPN_OK;
	INT					n = 0;

#ifdef INCLUDE_XNET_IMPORTS
	XnetInitialize(NULL, TRUE);
#endif

#ifdef INCLUDE_DPLAY_GENERAL_IMPORTS
	hr = DPlayInitialize(1024 * 200);
	if(hr != DPN_OK)
		goto Exit;
#endif

#if (defined(INCLUDE_DPLAY_PEER_BASE) || defined(INCLUDE_DPLAY_PEER_GROUPS) || defined(INCLUDE_DPLAY_PEER_FULL))
	IDirectPlay8Peer *pPeer = NULL;
	hr = DirectPlay8Create(IID_IDirectPlay8Peer, (LPVOID *) &pPeer, NULL);
	if(hr != DPN_OK)
		goto Exit;

	// These functions are considered essential since almost every DirectPlay peer app will use them.
	IDirectPlay8Peer_AddRef(pPeer);
	IDirectPlay8Peer_Release(pPeer);
	IDirectPlay8Peer_Initialize(pPeer, NULL, NULL, 0);
	IDirectPlay8Peer_Connect(pPeer, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, 0);
	IDirectPlay8Peer_SendTo(pPeer, 0, NULL, 0, 0, NULL, NULL, 0);
	IDirectPlay8Peer_GetSendQueueInfo(pPeer, 0, NULL, NULL, 0);
	IDirectPlay8Peer_Host(pPeer, NULL, NULL, 0, NULL, NULL, NULL, 0);
	IDirectPlay8Peer_GetApplicationDesc(pPeer, NULL, NULL, 0);
	IDirectPlay8Peer_SetApplicationDesc(pPeer, NULL, 0);
	IDirectPlay8Peer_SetPeerInfo(pPeer, NULL, NULL, NULL, 0);
	IDirectPlay8Peer_GetPeerInfo(pPeer, NULL, NULL, NULL, 0);
	IDirectPlay8Peer_Close(pPeer, 0);
	IDirectPlay8Peer_EnumHosts(pPeer, NULL, NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, 0);
	IDirectPlay8Peer_DestroyPeer(pPeer, 0, NULL, 0, 0);
	IDirectPlay8Peer_ReturnBuffer(pPeer, NULL, 0);
	IDirectPlay8Peer_TerminateSession(pPeer, NULL, 0, 0);
	IDirectPlay8Peer_DoWork(pPeer, 0);

	// These functions are only included if groups are necessary
#if (defined(INCLUDE_DPLAY_PEER_GROUPS) || defined(INCLUDE_DPLAY_PEER_FULL))
	IDirectPlay8Peer_CreateGroup(pPeer, NULL, NULL, NULL, NULL, 0);
	IDirectPlay8Peer_DestroyGroup(pPeer, 0, NULL, NULL, 0);
	IDirectPlay8Peer_AddPlayerToGroup(pPeer, 0, 0, NULL, NULL, 0);
	IDirectPlay8Peer_RemovePlayerFromGroup(pPeer, 0, 0, NULL, NULL, 0);
	IDirectPlay8Peer_SetGroupInfo(pPeer, 0, NULL, NULL, NULL, 0);
	IDirectPlay8Peer_GetGroupInfo(pPeer, 0, NULL, NULL, 0);
	IDirectPlay8Peer_EnumPlayersAndGroups(pPeer, NULL, NULL, 0);
	IDirectPlay8Peer_EnumGroupMembers(pPeer, 0, NULL, NULL, 0);
	IDirectPlay8Peer_GetGroupContext(pPeer, 0, NULL, 0);
#endif
	// These functions are considered non-essential since many DirectPlay apps can be written not to use them.
#if defined(INCLUDE_DPLAY_PEER_FULL)
	IDirectPlay8Peer_EnumServiceProviders(pPeer, NULL, NULL, NULL, NULL, NULL, 0);
	IDirectPlay8Peer_CancelAsyncOperation(pPeer, NULL, 0);
	IDirectPlay8Peer_GetPeerAddress(pPeer, 0, NULL, 0);
	IDirectPlay8Peer_GetLocalHostAddresses(pPeer, NULL, NULL, 0);
	IDirectPlay8Peer_GetPlayerContext(pPeer, 0, NULL, 0);
	IDirectPlay8Peer_GetCaps(pPeer, NULL, 0);
	IDirectPlay8Peer_SetCaps(pPeer, NULL, 0);
	IDirectPlay8Peer_SetSPCaps(pPeer, NULL, NULL, 0);
	IDirectPlay8Peer_GetSPCaps(pPeer, NULL, NULL, 0);
	IDirectPlay8Peer_GetConnectionInfo(pPeer, 0, NULL,0);
	IDirectPlay8Peer_RegisterLobby(pPeer, NULL, NULL, 0);
#endif
#endif

#if (defined(INCLUDE_DPLAY_CLIENT_BASE) || defined(INCLUDE_DPLAY_CLIENT_GROUPS) || defined(INCLUDE_DPLAY_CLIENT_FULL))
	IDirectPlay8Client *pClient = NULL;
	hr = DirectPlay8Create(IID_IDirectPlay8Client, (LPVOID *) &pClient, NULL);
	if(hr != DPN_OK)
		goto Exit;

	// These functions are considered essential since almost every DirectPlay peer app will use them.
	IDirectPlay8Client_AddRef(pClient);
	IDirectPlay8Client_Release(pClient);
	IDirectPlay8Client_Initialize(pClient, NULL, NULL, 0);
	IDirectPlay8Client_EnumHosts(pClient, NULL, NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, 0);
	IDirectPlay8Client_Connect(pClient, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, 0);
	IDirectPlay8Client_Send(pClient, NULL, 0, 0, NULL, NULL, 0);
	IDirectPlay8Client_GetSendQueueInfo(pClient, NULL, NULL, 0);
	IDirectPlay8Client_GetApplicationDesc(pClient, NULL, NULL, 0);
	IDirectPlay8Client_SetClientInfo(pClient, NULL, NULL, NULL, 0);
	IDirectPlay8Client_GetServerInfo(pClient, NULL, NULL, 0);
	IDirectPlay8Client_Close(pClient, 0);
	IDirectPlay8Client_ReturnBuffer(pClient, NULL, 0);
	IDirectPlay8Client_DoWork(pClient, 0);

	// These functions are considered non-essential since many DirectPlay apps can be written not to use them.
#if defined(INCLUDE_DPLAY_CLIENT_FULL)
	IDirectPlay8Client_EnumServiceProviders(pClient, NULL, NULL, NULL, NULL, NULL, 0);
	IDirectPlay8Client_CancelAsyncOperation(pClient, NULL, 0);
	IDirectPlay8Client_GetServerAddress(pClient, NULL, 0);
	IDirectPlay8Client_GetCaps(pClient, NULL, 0);
	IDirectPlay8Client_SetCaps(pClient, NULL, 0);
	IDirectPlay8Client_SetSPCaps(pClient, NULL, NULL, 0);
	IDirectPlay8Client_GetSPCaps(pClient, NULL, NULL, 0);
	IDirectPlay8Client_GetConnectionInfo(pClient, NULL, 0);
	IDirectPlay8Client_RegisterLobby(pClient, NULL, NULL, 0);
#endif
#endif

#if (defined(INCLUDE_DPLAY_SERVER_BASE) || defined(INCLUDE_DPLAY_SERVER_GROUPS) || defined(INCLUDE_DPLAY_SERVER_FULL))
	IDirectPlay8Server *pServer = NULL;
	hr = DirectPlay8Create(IID_IDirectPlay8Server, (LPVOID *) &pServer, NULL);
	if(hr != DPN_OK)
		goto Exit;

	// These functions are considered essential since almost every DirectPlay peer app will use them.
	IDirectPlay8Server_AddRef(pServer);
	IDirectPlay8Server_Release(pServer);
	IDirectPlay8Server_Initialize(pServer, NULL, NULL, 0);
	IDirectPlay8Server_GetSendQueueInfo(pServer, 0, NULL, NULL, 0);
	IDirectPlay8Server_GetApplicationDesc(pServer, NULL, NULL, 0);
	IDirectPlay8Server_SetServerInfo(pServer, NULL, NULL, NULL, 0);
	IDirectPlay8Server_GetClientInfo(pServer, NULL, NULL, NULL, 0);
	IDirectPlay8Server_SetApplicationDesc(pServer, NULL, 0);
	IDirectPlay8Server_Host(pServer, NULL, NULL, 0, NULL, NULL, NULL, 0);
	IDirectPlay8Server_SendTo(pServer, 0, NULL, 0, 0, NULL, NULL, 0);
	IDirectPlay8Server_Close(pServer, 0);
	IDirectPlay8Server_DestroyClient(pServer, 0, NULL, 0, 0);
	IDirectPlay8Server_ReturnBuffer(pServer, NULL, 0);
	IDirectPlay8Server_DoWork(pServer, NULL);

		// These functions are only included if groups are necessary
#if (defined(INCLUDE_DPLAY_SERVER_GROUPS) || defined(INCLUDE_DPLAY_SERVER_FULL))
	IDirectPlay8Server_CreateGroup(pServer, NULL, NULL, NULL, NULL, 0);
	IDirectPlay8Server_DestroyGroup(pServer, 0, NULL, NULL, 0);
	IDirectPlay8Server_AddPlayerToGroup(pServer, 0, 0, NULL, NULL, 0);
	IDirectPlay8Server_RemovePlayerFromGroup(pServer, 0, 0, NULL, NULL, 0);
	IDirectPlay8Server_SetGroupInfo(pServer, 0, NULL, NULL, NULL, 0);
	IDirectPlay8Server_GetGroupInfo(pServer, 0, NULL, NULL, 0);
	IDirectPlay8Server_EnumPlayersAndGroups(pServer, NULL, NULL, 0);
	IDirectPlay8Server_EnumGroupMembers(pServer, 0, NULL, NULL, 0);
	IDirectPlay8Server_GetGroupContext(pServer, 0, NULL, 0);
#endif

		// These functions are considered non-essential since many DirectPlay apps can be written not to use them.
#if defined(INCLUDE_DPLAY_SERVER_FULL)
	IDirectPlay8Server_GetPlayerContext(pServer, 0, NULL, 0);
	IDirectPlay8Server_EnumServiceProviders(pServer, NULL, NULL, NULL, NULL, NULL, 0);
	IDirectPlay8Server_CancelAsyncOperation(pServer, NULL, 0);
	IDirectPlay8Server_GetClientAddress(pServer, 0, NULL, 0);
	IDirectPlay8Server_GetLocalHostAddresses(pServer, NULL, NULL, 0);
	IDirectPlay8Server_GetCaps(pServer, NULL, 0);
	IDirectPlay8Server_SetCaps(pServer, NULL, 0);
	IDirectPlay8Server_SetSPCaps(pServer, NULL, NULL, 0);
	IDirectPlay8Server_GetSPCaps(pServer, NULL, NULL, 0);
	IDirectPlay8Server_GetConnectionInfo(pServer, 0, NULL, 0);
	IDirectPlay8Server_RegisterLobby,(pServer, NULL, NULL, 0);
#endif
#endif

#if (defined(INCLUDE_DPLAY_ADDRESS_BASE) || defined(INCLUDE_DPLAY_ADDRESS_FULL) || defined(INCLUDE_DPLAY_ADDRESSIP_FULL))
	IDirectPlay8Address *pAddress = NULL;
	hr = DirectPlay8AddressCreate(NULL, NULL, NULL);

	if(hr != DPN_OK)
		goto Exit;
	
	IDirectPlay8Address_AddRef(pAddress);
	IDirectPlay8Address_Release(pAddress);

#if defined(INCLUDE_DPLAY_ADDRESS_FULL)
	// IDirectPlay8Address methods
	IDirectPlay8Address_BuildFromURLW(pAddress, NULL);
	IDirectPlay8Address_BuildFromURLA(pAddress, NULL);
	IDirectPlay8Address_Duplicate(pAddress, NULL);
	IDirectPlay8Address_SetEqual(pAddress, NULL);
	IDirectPlay8Address_IsEqual(pAddress, NULL);
	IDirectPlay8Address_Clear(pAddress);
	IDirectPlay8Address_GetURLW(pAddress, NULL, NULL);
	IDirectPlay8Address_GetURLA(pAddress, NULL, NULL);
	IDirectPlay8Address_GetSP(pAddress, NULL);
	IDirectPlay8Address_GetUserData(pAddress, NULL, NULL);
	IDirectPlay8Address_SetSP(pAddress, NULL);
	IDirectPlay8Address_SetUserData(pAddress, NULL, 0);
	IDirectPlay8Address_GetNumComponents(pAddress, NULL);
	IDirectPlay8Address_GetComponentByName(pAddress, NULL, NULL, NULL, NULL);
	IDirectPlay8Address_GetComponentByIndex(pAddress, 0, NULL, NULL, NULL, NULL, NULL);
	IDirectPlay8Address_AddComponent(pAddress, NULL, NULL, 0, 0);
	IDirectPlay8Address_GetDevice(pAddress, NULL);
	IDirectPlay8Address_SetDevice(pAddress, NULL);
#endif

#if defined(INCLUDE_DPLAY_ADDRESSIP_FULL)
	// IDirectPlay8AddressIP methods
	IDirectPlay8Address_BuildFromSockAddr(pAddress, NULL);
	IDirectPlay8Address_BuildAddress(pAddress, NULL, 0);
	IDirectPlay8Address_BuildLocalAddress(pAddress, NULL, 0);
	IDirectPlay8Address_GetSockAddress(pAddress, NULL, NULL);
	IDirectPlay8Address_GetLocalAddress(pAddress, NULL, NULL);
	IDirectPlay8Address_GetAddress(pAddress, NULL, NULL, NULL);
#endif
#endif

	while(TRUE)
	{
		// Loop to infinity
	}
	
	goto Exit;

Exit:

	__asm int 3;

#if (defined(INCLUDE_DPLAY_PEER_BASE) || defined(INCLUDE_DPLAY_PEER_GROUPS) || defined(INCLUDE_DPLAY_PEER_FULL))
	pPeer->Release();
	pPeer = NULL;
#endif

#if (defined(INCLUDE_DPLAY_CLIENT_BASE) || defined(INCLUDE_DPLAY_CLIENT_GROUPS) || defined(INCLUDE_DPLAY_CLIENT_FULL))
	pClient->Release();
	pClient = NULL;
#endif

#if (defined(INCLUDE_DPLAY_SERVER_BASE) || defined(INCLUDE_DPLAY_SERVER_GROUPS) || defined(INCLUDE_DPLAY_SERVER_FULL))
	pServer->Release();
	pServer = NULL;
#endif

#ifdef INCLUDE_DPLAY_GENERAL_IMPORTS
	DPlayCleanup();
#endif

#ifdef INCLUDE_XNET_IMPORTS
	XnetCleanup();
#endif
}


