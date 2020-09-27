//-----------------------------------------------------------------------------
// File: peeraddressclose2.cpp
//
// Desc: Repro case for blocking close after peer address call
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <dplay8.h>
#include <dpaddr.h>

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define TEST_SECTION	OutputDebugString
#define CREATEEVENT_OR_THROW(handle, lpEventAttributes, fManualReset, fInitialState, pszName)\
										{\
											if (handle != NULL)\
											{\
												OutputDebugString("Handle is not NULL!\n");\
												__asm int 3\
											}\
											handle = CreateEvent(lpEventAttributes, fManualReset, fInitialState, pszName);\
											if (handle == NULL)\
											{\
												OutputDebugString("Couldn't create event!\n");\
												__asm int 3\
											}\
										}
#define IS_VALID_HANDLE(x)  			(x && (INVALID_HANDLE_VALUE != x))
#define SAFE_CLOSEHANDLE(h)				{\
											if (IS_VALID_HANDLE(h))\
											{\
												CloseHandle(h);\
												h = NULL;\
											}\
										}												

//-----------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------
typedef struct tagPARMVPGETPEERADDRESSCONTEXT
{
	BOOL					fHost; // whether this is the host context or not
	DPNID					dpnidHost; // ID of host player
	DPNID					dpnidClient; // ID of client player
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	HANDLE					hClientCreatedEvent; // event to set when CREATE_PLAYER for client arrives on host
} PARMVPGETPEERADDRESSCONTEXT, * PPARMVPGETPEERADDRESSCONTEXT;

typedef struct _DP_DOWORKLIST {
	DWORD					dwNumPeers;					// Number of peer objects present
	PDIRECTPLAY8PEER		*apDP8Peers;				// An array of pointers to peer objects
	DWORD					**apdwPeerCallbackTimes;	// An array of pointers to DWORDS that track the
														// amount of time spent in callbacks per peer
} DP_DOWORKLIST, *PDP_DOWORKLIST;

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
GUID g_guidApp = { 0x2ae835d, 0x9179, 0x485f, { 0x83, 0x43, 0x90, 0x1d, 0x32, 0x7c, 0xe7, 0x94 } };

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------
HRESULT ParmVPGetPeerAddressDPNMessageHandler(PVOID, DWORD, PVOID);
HRESULT ParmVCreatePeerHost(PFNDPNMESSAGEHANDLER, PVOID, PDPN_APPLICATION_DESC, PVOID,
							LPDIRECTPLAY8PEER*, PDIRECTPLAY8ADDRESS*);
BOOL PollUntilEventSignalled(PDP_DOWORKLIST, DWORD, HANDLE);
VOID ClearDoWorkList(DP_DOWORKLIST *);

void __cdecl main()
{
	LPDIRECTPLAY8PEER				pDP8Peer = NULL, pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS				pDP8HostAddress = NULL;
	PDIRECTPLAY8ADDRESS				pDP8HostAddressCompare = NULL;
	PDIRECTPLAY8ADDRESS				pDP8ReturnedAddress = NULL;
	PARMVPGETPEERADDRESSCONTEXT		hostcontext;
	PARMVPGETPEERADDRESSCONTEXT		nonhostcontext;
	DPN_APPLICATION_DESC			dpnad;
	DPN_APPLICATION_DESC			dpnadCompare;
	DP_DOWORKLIST					DoWorkList;
	DPNHANDLE						hAsyncOp;
	HRESULT							hr = DPN_OK;
	HANDLE							hAsyncOpComplete = NULL;
	
	ZeroMemory(&DoWorkList, sizeof(DP_DOWORKLIST));
	ZeroMemory(&hostcontext, sizeof (PARMVPGETPEERADDRESSCONTEXT));
	hostcontext.fHost = TRUE;
	
	ZeroMemory(&nonhostcontext, sizeof (PARMVPGETPEERADDRESSCONTEXT));
	
    if (XnetInitialize(NULL, TRUE) != NO_ERROR)
    {
        __asm int 3;
        goto Exit;
    }

    if (FAILED(DPlayInitialize(1024 * 200)))
    {
        __asm int 3;
        goto Exit;
    }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	TEST_SECTION("Creating DirectPlay8Peer object\n");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	hr = DirectPlay8Create(IID_IDirectPlay8Peer, (void **) &pDP8Peer, NULL);
	if( FAILED(hr) )
	{
		__asm int 3;
	}
	
	ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
	dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpnad.guidApplication = g_guidApp;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC));
	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	TEST_SECTION("Initializing peer object with nonhost context\n");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	hr = pDP8Peer->Initialize(&nonhostcontext, ParmVPGetPeerAddressDPNMessageHandler, 0);
	if (hr != DPN_OK)
	{
		OutputDebugString("Initializing peer object with context, 0 flags failed!\n");
		__asm int 3;
		goto Exit;
	}
	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	TEST_SECTION("Creating new peer host object\n");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	hr = ParmVCreatePeerHost(ParmVPGetPeerAddressDPNMessageHandler, &hostcontext, &dpnad, &(hostcontext.dpnidHost),
		&pDP8PeerHost, &pDP8HostAddress);
	if (hr != DPN_OK)
	{
		OutputDebugString("Creating peer host failed!\n");
		__asm int 3;
		goto Exit;
	}
	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	TEST_SECTION("Connecting peer\n");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	// Host should get the create player now.
	CREATEEVENT_OR_THROW(hostcontext.hClientCreatedEvent, NULL, FALSE, FALSE, NULL);
	CREATEEVENT_OR_THROW(hAsyncOpComplete, NULL, FALSE, FALSE, NULL);
	
	// Even though it's synchronous, we'll still get a completion.  The async op
	// handle will be NULL, though.
	nonhostcontext.fConnectCanComplete = TRUE;
	
	hr = pDP8Peer->Connect(&dpnad, pDP8HostAddress, NULL, NULL, NULL, NULL, 0,
		&(nonhostcontext.dpnidClient), hAsyncOpComplete, &hAsyncOp, 0);
	
	if((hr != DPNSUCCESS_PENDING) && (hr != DPN_OK))
	{
		OutputDebugString("Couldn't connect!\n");
		__asm int 3;
		goto Exit;
	} // end if (couldn't connect)
	
	if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
	{
		OutputDebugString("Application desc structure was modified!\n");
		__asm int 3;
		goto Exit;
	} // end if (app desc changed)
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	TEST_SECTION("Waiting for connect to complete on non-host\n");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
	DoWorkList.dwNumPeers = 2;
	DoWorkList.apDP8Peers = (PDIRECTPLAY8PEER *) LocalAlloc(LPTR, sizeof(LPVOID) * 2);
	DoWorkList.apDP8Peers[0] = pDP8Peer;
	DoWorkList.apDP8Peers[1] = pDP8PeerHost;
	
	if(!PollUntilEventSignalled(&DoWorkList, 60000, hAsyncOpComplete))
	{
		OutputDebugString("Polling for asynchronous operation failed!\n");
		__asm int 3;
		goto Exit;
	}
	
	ClearDoWorkList(&DoWorkList);
	
	// Prevent any weird connects from completing.
	nonhostcontext.fConnectCanComplete = FALSE;
	
	// Make sure we saw the connect completion indication.
	if (! nonhostcontext.fConnectCompleted)
	{
		OutputDebugString("Didn't get connect complete indication!\n");
		__asm int 3;
		goto Exit;
	} // end if (connect didn't complete)
	
	// Reset the context.
	nonhostcontext.fConnectCompleted = FALSE;
	
	// Make sure we got the create player messages.
	if (nonhostcontext.dpnidHost == 0)
	{
		OutputDebugString("Didn't get CREATE_PLAYER indication for host on client!\n");
		__asm int 3;
		goto Exit;
	} // end if (didn't get create player)
	
	if (nonhostcontext.dpnidClient == 0)
	{
		OutputDebugString("Didn't get CREATE_PLAYER indication for client on client!\n");
		__asm int 3;
		goto Exit;
	} // end if (didn't get create player)
	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	TEST_SECTION("Waiting for create player to be indicated on host\n");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
	DoWorkList.dwNumPeers = 2;
	DoWorkList.apDP8Peers = (PDIRECTPLAY8PEER *) LocalAlloc(LPTR, sizeof(LPVOID) * 2);
	DoWorkList.apDP8Peers[0] = pDP8Peer;
	DoWorkList.apDP8Peers[1] = pDP8PeerHost;
	
	if(!PollUntilEventSignalled(&DoWorkList, 60000, hostcontext.hClientCreatedEvent))
	{
		OutputDebugString("Polling for asynchronous operation failed!\n");
		__asm int 3;
		goto Exit;
	}
	
	ClearDoWorkList(&DoWorkList);
	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	TEST_SECTION("Getting client's peer address on host\n");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	hr = pDP8PeerHost->GetPeerAddress(hostcontext.dpnidClient, &pDP8ReturnedAddress, 0);
	if (hr != DPN_OK)
	{
		OutputDebugString("Getting client's peer address on host failed!\n");
		__asm int 3;
		goto Exit;
	} // end if (couldn't get peer address)
	
	if (pDP8ReturnedAddress == NULL)
	{
		OutputDebugString("No address was returned)!\n");
		__asm int 3;
		goto Exit;
	} // end if (no address returned)
	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	TEST_SECTION("Closing peer client object\n");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	hr = pDP8Peer->Close(0);
	if (hr != DPN_OK)
	{
		OutputDebugString("Closing peer client object failed!\n");
		__asm int 3;
		goto Exit;
	} // end if (couldn't close)
	
	// Make sure we got the destroy player messages.
	if (nonhostcontext.dpnidHost != 0)
	{
		OutputDebugString("Didn't get DESTROY_PLAYER indication for host on client!\n");
		__asm int 3;
		goto Exit;
	} // end if (didn't get destroy player)
	
	if (nonhostcontext.dpnidClient != 0)
	{
		OutputDebugString("Didn't get DESTROY_PLAYER indication for client on client!\n");
		__asm int 3;
		goto Exit;
	} // end if (didn't get destroy player)
	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	TEST_SECTION("Closing host object\n");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	hr = pDP8PeerHost->Close(0);
	if (hr != DPN_OK)
	{
		OutputDebugString("Closing host object failed!\n");
		__asm int 3;
		goto Exit;
	} // end if (couldn't close)
	
	// Make sure we got the destroy player messages.
	if (hostcontext.dpnidHost != 0)
	{
		OutputDebugString("Didn't get DESTROY_PLAYER indication for host on host!\n");
		__asm int 3;
		goto Exit;
	} // end if (didn't get destroy player)
	
	if (hostcontext.dpnidClient != 0)
	{
		OutputDebugString("Didn't get DESTROY_PLAYER indication for client on host!\n");
		__asm int 3;
		goto Exit;
	} // end if (didn't get destroy player)
	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	TEST_SECTION("Releasing hosting DirectPlay8Peer object\n");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	hr = pDP8PeerHost->Release();
	if (hr != S_OK)
	{
		OutputDebugString("Couldn't release hosting DirectPlay8Peer object!\n");
		__asm int 3;
		goto Exit;
	} // end if (couldn't release object)
	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	TEST_SECTION("Releasing DirectPlay8Peer object\n");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
	hr = pDP8Peer->Release();
	if (hr != S_OK)
	{
		OutputDebugString("Couldn't release DirectPlay8Peer object!\n");
		__asm int 3;
		goto Exit;
	} // end if (couldn't release object)
	
	
Exit:
	
	if (pDP8Peer != NULL)
	{
		pDP8Peer->Release();
		pDP8Peer = NULL;
	} // end if (have peer object)
	
	if (pDP8PeerHost != NULL)
	{
		pDP8PeerHost->Release();
		pDP8PeerHost = NULL;
	} // end if (have peer object)
	
	pDP8HostAddress ? pDP8HostAddress->Release() : 0;
	pDP8HostAddressCompare ? pDP8HostAddressCompare->Release() : 0;
	pDP8ReturnedAddress ? pDP8ReturnedAddress->Release() : 0;
	SAFE_CLOSEHANDLE(hostcontext.hClientCreatedEvent);
	ClearDoWorkList(&DoWorkList);
	SAFE_CLOSEHANDLE(hAsyncOpComplete);

	while(TRUE);

	return;
}

HRESULT ParmVPGetPeerAddressDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT							hr = DPN_OK;
	PPARMVPGETPEERADDRESSCONTEXT	pContext = (PPARMVPGETPEERADDRESSCONTEXT) pvContext;
	BOOL							fSetEvent = FALSE;


	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;

//			OutputDebugString("DPN_MSGID_CONNECT_COMPLETE\n");

			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			// Make sure we expect the completion.
			if (! pContext->fConnectCanComplete)
			{
				__asm int 3;
				hr = E_FAIL;
				goto DONE;
			} // end if (message unexpected)


			// Validate the completion.
			if (pConnectCompleteMsg->dwSize != sizeof (DPNMSG_CONNECT_COMPLETE))
			{
				__asm int 3;
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				__asm int 3;
				hr = pConnectCompleteMsg->hResultCode;
				goto DONE;
			} // end if (wrong result code)

			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				__asm int 3;
				hr = E_FAIL;
				goto DONE;
			} // end if (unexpected reply data)


			// Make sure this isn't a double completion.
			if (pContext->fConnectCompleted)
			{
				__asm int 3;
				hr = E_FAIL;
				goto DONE;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fConnectCompleted = TRUE;

			// XBOX - Handle async connect
			if(pConnectCompleteMsg->pvUserContext)
			{
				SetEvent((HANDLE) pConnectCompleteMsg->pvUserContext);
			}
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;

//			OutputDebugString("DPN_MSGID_CREATE_PLAYER\n");

			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			// Validate the indication.
			if (pCreatePlayerMsg->dwSize != sizeof (DPNMSG_CREATE_PLAYER))
			{
				__asm int 3;
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pCreatePlayerMsg->pvPlayerContext == NULL)
			{
				// Assume this is the remote player being created.  Store the
				// player context.
				if (pContext->fHost)
				{
					pCreatePlayerMsg->pvPlayerContext = &(pContext->dpnidClient);
					fSetEvent = TRUE;
				} // end if (host)
				else
				{
					pCreatePlayerMsg->pvPlayerContext = &(pContext->dpnidHost);
				} // end if (not host)
			} // end if (context is not set yet)


			// Make sure this isn't a duplicate message.
			if (*((DPNID*) pCreatePlayerMsg->pvPlayerContext) != 0)
			{
				__asm int 3;
				hr = E_FAIL;
				goto DONE;
			} // end if (already have ID)

			// Save the player ID.
			*((DPNID*) pCreatePlayerMsg->pvPlayerContext) = pCreatePlayerMsg->dpnidPlayer;


			// Now set the event, if we're supposed too.
			if (fSetEvent)
			{
				if (! SetEvent(pContext->hClientCreatedEvent))
				{
					hr = GetLastError();

					__asm int 3;

					if (hr == S_OK)
						hr = E_FAIL;

					goto DONE;
				} // end if (couldn't set event)
			} // end if (should set event)
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;

//			OutputDebugString("DPN_MSGID_DESTROY_PLAYER\n");

			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			// Validate the indication.
			if (pDestroyPlayerMsg->dwSize != sizeof (DPNMSG_DESTROY_PLAYER))
			{
				__asm int 3;
				hr = E_FAIL;
				goto DONE;
			} // end if (size incorrect)

			if (pDestroyPlayerMsg->pvPlayerContext == NULL)
			{
				__asm int 3;
				hr = E_FAIL;
				goto DONE;
			} // end if (context is wrong)

			if (pDestroyPlayerMsg->dwReason != DPNDESTROYPLAYERREASON_NORMAL)
			{
				__asm int 3;
				hr = E_FAIL;
				goto DONE;
			} // end if (reason is wrong)


			// Make sure this isn't a duplicate or bogus message.
			if ((*((DPNID*) pDestroyPlayerMsg->pvPlayerContext) == 0) ||
				(pDestroyPlayerMsg->dpnidPlayer != *((DPNID*) pDestroyPlayerMsg->pvPlayerContext)))
			{
				__asm int 3;
				hr = E_FAIL;
				goto DONE;
			} // end if (already have ID)

			// Clear the player ID.
			*((DPNID*) pDestroyPlayerMsg->pvPlayerContext) = 0;
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;

//			OutputDebugString("DPN_MSGID_INDICATE_CONNECT\n");

			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

		  break;

		default:

//			OutputDebugString("Unhandled message!\n");

			__asm int 3;
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

DONE:

	return (hr);
}

HRESULT ParmVCreatePeerHost(PFNDPNMESSAGEHANDLER pfn, PVOID pvContext, PDPN_APPLICATION_DESC pdpnad,
							PVOID pvPlayerContext, LPDIRECTPLAY8PEER* ppDP8PeerHost, PDIRECTPLAY8ADDRESS* ppDP8AddressHost)
{
	HRESULT					hr;
	PDIRECTPLAY8ADDRESS		pDP8AddressDevice = NULL;
	PDIRECTPLAY8ADDRESS*	paDP8HostAddresses = NULL;
	DWORD					dwNumAddresses = 0;



	if ((*ppDP8PeerHost) != NULL)
	{
		OutputDebugString("Already have peer object!");
		return (ERROR_INVALID_PARAMETER);
	} // end if (already have object)


	hr = DirectPlay8Create(IID_IDirectPlay8Peer, (void **) ppDP8PeerHost, NULL);
	if( FAILED(hr) )
	{
		__asm int 3;
	}

	hr = (*ppDP8PeerHost)->Initialize(pvContext, pfn, 0);
	if (hr != DPN_OK)
	{
		OutputDebugString("Initializing DP8Peer object failed!\n");
		goto ERROR_EXIT;
	} // end if (function failed)

	hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8AddressDevice, NULL);
	if (hr != S_OK)
	{
		OutputDebugString("Couldn't DirectPlay8AddressCreate host DirectPlay8Address object!\n");
		goto ERROR_EXIT;
	} // end if (DirectPlay8AddressCreate failed)

	hr = (*ppDP8PeerHost)->Host(pdpnad, &pDP8AddressDevice, 1, NULL, NULL, pvPlayerContext, 0);
	if (hr != DPN_OK)
	{
		OutputDebugString("Hosting failed!\n");
		goto ERROR_EXIT;
	} // end if (function failed)

	pDP8AddressDevice->Release();
	pDP8AddressDevice = NULL;

	// If the caller wants an address, give one to him.
	if (ppDP8AddressHost != NULL)
	{
		// Ignore error
		hr = (*ppDP8PeerHost)->GetLocalHostAddresses(NULL, &dwNumAddresses, 0);


		paDP8HostAddresses = (PDIRECTPLAY8ADDRESS*) LocalAlloc(LPTR, dwNumAddresses * sizeof (PDIRECTPLAY8ADDRESS));
		if (paDP8HostAddresses == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		} // end if (couldn't allocate memory)


		hr = (*ppDP8PeerHost)->GetLocalHostAddresses(paDP8HostAddresses, &dwNumAddresses, 0);
		if (hr != DPN_OK)
		{
			OutputDebugString("Getting local addresses failed!\n");
			goto ERROR_EXIT;
		} // end if (getting addresses failed)


		// Move the first address to the caller's pointer.
		(*ppDP8AddressHost) = paDP8HostAddresses[0];
		paDP8HostAddresses[0] = NULL;


		// Release all the other addresses.
		while (dwNumAddresses > 1)
		{
			dwNumAddresses--;
			paDP8HostAddresses[dwNumAddresses]->Release();
			paDP8HostAddresses[dwNumAddresses] = NULL;
		} // end while (more addresses)


		LocalFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	} // end if (caller wants address)

	return (DPN_OK);


ERROR_EXIT:


	if (paDP8HostAddresses != NULL)
	{
		LocalFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	} // end if (allocated address array)

	if (pDP8AddressDevice != NULL)
	{
		pDP8AddressDevice->Release();
		pDP8AddressDevice = NULL;
	} // end if (allocated address object)

	if ((*ppDP8PeerHost) != NULL)
	{
		(*ppDP8PeerHost)->Release();
		(*ppDP8PeerHost) = NULL;
	} // end if (have wrapper object)

	return (hr);
}

BOOL PollUntilEventSignalled(PDP_DOWORKLIST pDoWorkList, DWORD dwMaxTime, HANDLE hEvent)
{
	HRESULT	hr = S_OK;
	DWORD	dwWaitResult, dwTotalTime, dwStartTime, dwEndTime, dwCurrentObject;
	BOOL	fRet = TRUE, fRunDoWorkLoop = TRUE;

	// Need to get the start time to determine later if too much time has been
	// spent doing work and waiting
	if(dwMaxTime != INFINITE)
	{
		dwStartTime = GetTickCount();
		dwTotalTime = 0;
	}

	// Wait to see if the event has completed.  May not even require a DoWork call
	dwWaitResult = WaitForSingleObject(hEvent, 0);
	switch(dwWaitResult)
	{
	case WAIT_OBJECT_0:
		OutputDebugString("Event completed without needing to call DoWork!\n");
		fRunDoWorkLoop = FALSE;
		break;
	case WAIT_TIMEOUT:
		break;
	default:
		OutputDebugString("Error occurred while waiting on event handle\n");
		fRet = FALSE;
		goto Exit;
	}

	while(fRunDoWorkLoop)
	{
		// Cycle through peer objects and perform DoWork processing
		for(dwCurrentObject = 0; dwCurrentObject < pDoWorkList->dwNumPeers; ++dwCurrentObject)
		{
			// Do work on this object
			pDoWorkList->apDP8Peers[dwCurrentObject]->DoWork(0);

			if(hr != DPN_OK)
			{
				OutputDebugString("Couldn't do DirectPlay work on a peer object!\n");
				fRet = FALSE;
				goto Exit;
			}

			// Wait for the desired result
			dwWaitResult = WaitForSingleObject(hEvent, 0);

			// Event completed.  Exit the loop
			if(dwWaitResult == WAIT_OBJECT_0)
				goto Exit;

			// If there's an upper bound on the amount of time that can be spent waiting for is event
			// See if that amount of time has already passed
			if(dwMaxTime != INFINITE)
			{
				dwEndTime = GetTickCount();
				dwTotalTime = (dwEndTime >= dwStartTime) ? dwEndTime - dwStartTime : (0xFFFFFFFF - dwStartTime) + dwEndTime;

				// If time has expired... bail
				if(dwTotalTime > dwMaxTime)
				{
					OutputDebugString("Event wasn't signalled within allowable time!\n");
					fRet = FALSE;
					goto Exit;
				}
			}
		} // End of loop for doing work over peer objects

	}

Exit:

	return fRet;
}

VOID ClearDoWorkList(DP_DOWORKLIST *pDoWorkList)
{
	if(pDoWorkList->apDP8Peers)
	{
		LocalFree(pDoWorkList->apDP8Peers);
		pDoWorkList->apDP8Peers = NULL;
	}

	return;
}

