//==================================================================================
// Includes
//==================================================================================
#include "dptest.h"
#include "macros.h"
#include "parmvalidation.h"
#include "simpletests.h"

using namespace DPlayCoreNamespace;

namespace DPlayCoreNamespace {

#undef DEBUG_SECTION
#define DEBUG_SECTION	"DirectPlayExec_SimpleClient()"

//==================================================================================
// DirectPlayExec_SimpleClient
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//
// Arguments:
//     HANDLE hLog                     Handle to the logging subsystem
//     PDP_HOSTINFO pHostInfo          Pointer to info on which machine is host
//                                     and which are clients
//     WORD LowPort                    Port that session will be hosted on 
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT DirectPlayExec_SimpleClient(HANDLE hLog, PDP_HOSTINFO pHostInfo, WORD LowPort)
{
	CTNSystemResult			sr;
	CTNTestResult			tr;
	PWRAPDP8CLIENT			pDP8Client = NULL;
	PDIRECTPLAY8ADDRESS		pDP8AddressHost = NULL;
	PDIRECTPLAY8ADDRESS		pDP8AddressDevice = NULL;
	DPN_APPLICATION_DESC	dpnad, *pdpnad = NULL;
	DPN_APPLICATION_DESC	dpnadCompare;
	DPN_BUFFER_DESC			dpnbd;
	DPN_BUFFER_DESC			dpnbdCompare;
	DPN_PLAYER_INFO			dpnpi, *pdpnpiServer = NULL;
	DPN_PLAYER_INFO			dpnpiCompare;
	DPN_CONNECTION_INFO		dpnci;
	DPN_SP_CAPS				dpnspc;
	DPN_CAPS				dpnc;
	DP_DOWORKLIST			DoWorkList;
	SIMPLECLIENTCONTEXT		SimpleClientContext;
	DWORD					i, dwNumIDs	,dwChecksum, dwAppDescSize, dwNumMsgs, dwNumBytes, dwServerDataSize;
	DPNHANDLE				dpnhSendTo;
	DPNID					SendTarget, *pCurrentPlayers = NULL;
	MESSAGEDATA				MessageData, *pServerData = NULL, *pAppReservedData = NULL;
	SOCKADDR_IN				ServerAddress;

	ZeroMemory(&SimpleClientContext, sizeof(SIMPLECLIENTCONTEXT));
	InitializeCriticalSection(&(SimpleClientContext.Lock));

	ZeroMemory(&DoWorkList, sizeof(DP_DOWORKLIST));

	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Client = new CWrapDP8Client(hLog);
		if (pDP8Client == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Client->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("CoCreating host DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8AddressHost, NULL);

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate host DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("CoCreating device DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8AddressDevice, NULL);

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate device DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing with context, 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		SimpleClientContext.hLog = hLog;
		SimpleClientContext.fVerbose = FALSE;
		SimpleClientContext.LocalPlayerID = 0;
		CREATEEVENT_OR_THROW(SimpleClientContext.hConnectComplete, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(SimpleClientContext.hIndicateRemotePlayer, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(SimpleClientContext.hReceiveData, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(SimpleClientContext.hAsyncOpCompleted, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(SimpleClientContext.hServerInfoChanged, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(SimpleClientContext.hAppDescChanged, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(SimpleClientContext.hSessionTerminated, NULL, FALSE, FALSE, NULL);

		tr = pDP8Client->DP8C_Initialize(&SimpleClientContext, SimpleClientMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("DoWork on newly initialized client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->m_pDP8Client->DoWork(0);
		if((tr != S_OK) && (tr != S_FALSE))
		{
			DPTEST_FAIL(hLog, "DoWork on client just after initialization returned 0x%08x", 1, tr);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting remote address and port to host machine");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ServerAddress.sin_family = AF_INET;
//		ServerAddress.sin_port = htons(LowPort);
		ServerAddress.sin_port = htons(2302);
		ServerAddress.sin_addr.S_un.S_addr = pHostInfo->dwHostAddr;

		// BUGBUG - hardcode address of host machine
		tr = pDP8AddressHost->BuildFromSockAddr((SOCKADDR *) &ServerAddress);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't set port for non-host.", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting initial client information");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Copy the player buffer
		strncpy(MessageData.szData, CLIENT_DATA1, MAX_MESSAGEDATA_DATA);
		MessageData.szData[MAX_MESSAGEDATA_DATA - 1] = 0;
		MessageData.dwMessageType = PLAYER_DATA;
		MessageData.dwChecksum = 0;

		// Calculate the checksum
		for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
			MessageData.dwChecksum += MessageData.szData[i];

		// Set up the player info structure
		ZeroMemory(&dpnpi, sizeof(DPN_PLAYER_INFO));
		dpnpi.dwSize = sizeof(DPN_PLAYER_INFO);
		dpnpi.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
		dpnpi.dwPlayerFlags = 0;
		dpnpi.dwDataSize = sizeof(MESSAGEDATA);
		dpnpi.pvData = &MessageData;
		dpnpi.pwszName = CLIENT_PLAYER_NAME1;

		CopyMemory(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO));

		SimpleClientContext.fAsyncOpCanComplete = TRUE;
		SimpleClientContext.fAsyncOpCompleted = FALSE;
		SimpleClientContext.hrAsyncOpExpectedResult = S_OK;
		SimpleClientContext.dpnhAsyncOp = (DPNHANDLE) 0x666;

		tr = pDP8Client->DP8C_SetClientInfo(&dpnpi, NULL, &(SimpleClientContext.dpnhAsyncOp), 0);

		// This function can return synchronously even when called
		// as an async function, so be prepared for that
		if(tr == S_OK)
		{
			DPTEST_TRACE(hLog, "SetClientInfo completed synchronously", 0);
			SetEvent(SimpleClientContext.hAsyncOpCompleted);
			SimpleClientContext.fAsyncOpCompleted = TRUE;
		}
		// If this operation is completing asynchronously we should have a handle
		else if (tr == DPNSUCCESS_PENDING)
		{
			if (SimpleClientContext.dpnhAsyncOp == (DPNHANDLE) 0x666)
			{
				DPTEST_FAIL(hLog, "No async operation handle was returned!", 0);
				SETTHROW_TESTRESULT(ERROR_NO_DATA);
			} // end if (handle was not set)
			
			if (SimpleClientContext.dpnhAsyncOp == NULL)
			{
				DPTEST_FAIL(hLog, "Async operation handle returned is NULL!", 0);
				SETTHROW_TESTRESULT(E_FAIL);
			} // end if (handle was set to NULL)
		}
		// Otherwise, we got an unexpected error
		else
		{
			DPTEST_FAIL(hLog, "Couldn't send message!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't send message to self)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer desc changed)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for initial set client info operation to complete");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Set up DoWork time parameters
		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = TRUE;
		DoWorkList.dwNumClients = 1;
		DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Clients[0] = pDP8Client->m_pDP8Client;
		DoWorkList.apdwClientCallbackTimes = (DWORD **) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apdwClientCallbackTimes[0] = &(SimpleClientContext.dwCallbackTime);

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleClientContext.hAsyncOpCompleted))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Stop allowing asynchronous operation completions
		SimpleClientContext.fAsyncOpCanComplete = FALSE;

		// If we didn't get an asynchronous operation completion, then we're in trouble
		if(!SimpleClientContext.fAsyncOpCompleted)
		{
			DPTEST_FAIL(hLog, "Didn't receive asynchronous completion for set client info!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (async operation didn't complete)
	
		SimpleClientContext.fAsyncOpCompleted = FALSE;
		SimpleClientContext.dpnhAsyncOp = NULL;

/* BUGBUG - remove until connect data and connect reply data are working
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Starting async connect to host w/o connect data");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.guidApplication = GUID_PARMV_PEER_HOST;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC));

		// Allow connection completions but only for host rejection
		SimpleClientContext.fConnectCanComplete = TRUE;
		SimpleClientContext.fConnectCompleted = FALSE;
		SimpleClientContext.hrExpectedConnectResult = DPNERR_HOSTREJECTEDCONNECTION;
		
		// The host should reject this connection so we shouldn't
		// get remote player creations or incoming messages
		SimpleClientContext.fCanAcceptRemotePlayer = FALSE;
		SimpleClientContext.fAcceptedRemotePlayer = FALSE;
		SimpleClientContext.fCanReceive = TRUE;
		SimpleClientContext.fReceived = FALSE;

		SimpleClientContext.dpnhConnect = 0x666;

		tr = pDP8Client->DP8C_Connect(&dpnad,
									pDP8AddressHost,
									pDP8AddressDevice,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									&(SimpleClientContext.dpnhConnect),
									0);

		if (tr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPTEST_FAIL(hLog, "Couldn't start async connect!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

		if (SimpleClientContext.dpnhConnect == (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (SimpleClientContext.dpnhConnect == NULL)
		{
			DPTEST_FAIL(hLog, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for connection completion to be indicated");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleClientContext.hConnectComplete))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Stop allowing connections
		SimpleClientContext.fConnectCanComplete = FALSE;

		// If the connection didn't complete, then we're in trouble
		if(!SimpleClientContext.fConnectCompleted)
		{
			DPTEST_FAIL(hLog, "No connection indicated!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (no connection completed)
	
		SimpleClientContext.fConnectCompleted = FALSE;

*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Starting async connect to host machine");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.guidApplication = GUID_PARMV_PEER_HOST;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC));

		// Copy the player buffer
		strncpy(MessageData.szData, CONNECT_MESSAGE, MAX_MESSAGEDATA_DATA);
		MessageData.szData[MAX_MESSAGEDATA_DATA - 1] = 0;
		MessageData.dwMessageType = CONNECT_DATA;
		MessageData.dwChecksum = 0;

		// Calculate the checksum
		for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
			MessageData.dwChecksum += MessageData.szData[i];

		// Allow successful connection completions
		SimpleClientContext.fConnectCanComplete = TRUE;
		SimpleClientContext.fConnectCompleted = FALSE;
		SimpleClientContext.hrExpectedConnectResult = S_OK;
		
		// Since this is a client, we don't get player indications
		// so we should give an error if we receive one
		SimpleClientContext.fCanAcceptRemotePlayer = FALSE;
		SimpleClientContext.fAcceptedRemotePlayer = FALSE;

		// Allow incoming messages
		SimpleClientContext.fCanReceive = TRUE;
		SimpleClientContext.fReceived = FALSE;

		SimpleClientContext.dpnhConnect = 0x666;

		tr = pDP8Client->DP8C_Connect(&dpnad,
									pDP8AddressHost,
									pDP8AddressDevice,
									NULL,
									NULL,
									&MessageData,
									sizeof(MESSAGEDATA),
									NULL,
									&(SimpleClientContext.dpnhConnect),
									0);


		if (tr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPTEST_FAIL(hLog, "Couldn't start async connect!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

		if (SimpleClientContext.dpnhConnect == (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (SimpleClientContext.dpnhConnect == NULL)
		{
			DPTEST_FAIL(hLog, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for connection completion to be indicated");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleClientContext.hConnectComplete))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Stop allowing connections
		SimpleClientContext.fConnectCanComplete = FALSE;

		// If the connection didn't complete, then we're in trouble
		if(!SimpleClientContext.fConnectCompleted)
		{
			DPTEST_FAIL(hLog, "No connection indicated!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (no connection completed)
	
		SimpleClientContext.fConnectCompleted = FALSE;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify server info");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwServerDataSize = 0;
		pdpnpiServer = NULL;

		tr = pDP8Client->DP8C_GetServerInfo(NULL, &dwServerDataSize, 0);

		// This function always returns synchronously
		if(tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting server info with NULL buffer on client didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server info)

		LOCALALLOC_OR_THROW(PDPN_PLAYER_INFO, pdpnpiServer, dwServerDataSize);
		pdpnpiServer->dwSize = sizeof(DPN_PLAYER_INFO);

		tr = pDP8Client->DP8C_GetServerInfo(pdpnpiServer, &dwServerDataSize, 0);

		// This function always returns synchronously
		if(tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Getting server info with valid buffer on client didn't return S_OK!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server info)

		//
		// Now validate the info received
		//

		// First validate that the dwInfoFlags parameter is correctly set
		if(pdpnpiServer->dwInfoFlags ^ (DPNINFO_NAME | DPNINFO_DATA))
		{
			DPTEST_FAIL(hLog, "Info flags didn't have both DPNINFO_NAME and DPNINFO_DATA set!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Next validate the player name
		if(wcscmp(pdpnpiServer->pwszName, SERVER_PLAYER_NAME1))
		{
			DPTEST_FAIL(hLog, "Unexpected server player name! (\"%S\" != \"%S\")",
				2, pdpnpiServer->pwszName, SERVER_PLAYER_NAME1);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Make sure we received enough data
		if(pdpnpiServer->dwDataSize < sizeof(MESSAGEDATA))
		{
			DPTEST_FAIL(hLog, "Received server data is too small! (\"%u\" < \"%u\")",
				2, pdpnpiServer->dwDataSize, sizeof(MESSAGEDATA));
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Verify the player data
		pServerData = NULL;
		if(!(pServerData = (PMESSAGEDATA) pdpnpiServer->pvData))
		{
			DPTEST_FAIL(hLog, "No player data was provided.", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Verify the type of data
		if(pServerData->dwMessageType != PLAYER_DATA)
		{
			DPTEST_FAIL(hLog, "Received player info data of invalid type!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		dwChecksum = 0;

		// Verify the data checksum
		for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
			dwChecksum += pServerData->szData[i];
		
		// Compare it to the checksum listed in the info structure
		if(dwChecksum != pServerData->dwChecksum)
		{
			DPTEST_FAIL(hLog, "Received server info data failed checksum!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Verify the player name is what we expect
		if(strcmp(pServerData->szData, SERVER_DATA1))
		{
			DPTEST_FAIL(hLog, "Unexpected server player data! (\"%s\" != \"%s\")",
				2, pServerData->szData, SERVER_DATA1);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		SAFE_LOCALFREE(pdpnpiServer);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for message from remote player");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleClientContext.hReceiveData))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Stop allowing incoming messages
		SimpleClientContext.fCanReceive = FALSE;

		// If we didn't get an incoming message, then we're in trouble
		if(!SimpleClientContext.fReceived)
		{
			DPTEST_FAIL(hLog, "No messages received!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (no message received)
	
		SimpleClientContext.fCanReceive = FALSE;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Check send queue before sending");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumMsgs = dwNumBytes = 0x666;

		tr = pDP8Client->DP8C_GetSendQueueInfo(&dwNumMsgs, &dwNumBytes, 0);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't get send queue info!", 0);
			THROW_TESTRESULT;
		}

		// Verify that no data is in the send queue
		if(dwNumMsgs || dwNumBytes)
		{
			DPTEST_FAIL(hLog, "Send queue shows backlog, but no data has been sent (%u/%u)",
				2, dwNumMsgs, dwNumBytes);
			THROW_TESTRESULT;
		}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Check connection info with new player");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		ZeroMemory(&dpnci, sizeof(DPN_CONNECTION_INFO));
		dpnci.dwSize = sizeof(DPN_CONNECTION_INFO);

		tr = pDP8Client->DP8C_GetConnectionInfo(&dpnci, 0);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't get connection info!", 0);
			THROW_TESTRESULT;
		}

		// Output the connection statistics
		DPTEST_TRACE(hLog, "Connection statistics:", 0);
		DPTEST_TRACE(hLog, "     Latency: %u", 1, dpnci.dwRoundTripLatencyMS);
		DPTEST_TRACE(hLog, "     Throughput: %u", 1, dpnci.dwThroughputBPS);
		DPTEST_TRACE(hLog, "     Peak Throughput: %u", 1, dpnci.dwPeakThroughputBPS);
		DPTEST_TRACE(hLog, "     Guaranteed sends : %u pkts/%u bytes",
			2, dpnci.dwPacketsSentGuaranteed, dpnci.dwBytesSentGuaranteed);
		DPTEST_TRACE(hLog, "     Non-guaranteed sends: %u pkts/%u bytes",
			2, dpnci.dwPacketsSentNonGuaranteed, dpnci.dwBytesSentNonGuaranteed);
		DPTEST_TRACE(hLog, "     Retried sends : %u pkts/%u bytes",
			2, dpnci.dwPacketsRetried, dpnci.dwBytesRetried);
		DPTEST_TRACE(hLog, "     Dropped sends : %u pkts/%u bytes",
			2, dpnci.dwPacketsDropped, dpnci.dwBytesDropped);
		DPTEST_TRACE(hLog, "     High-priority sends : %u xmited/%u timed-out",
			2, dpnci.dwMessagesTransmittedHighPriority, dpnci.dwMessagesTimedOutHighPriority);
		DPTEST_TRACE(hLog, "     Normal-priority sends : %u xmited/%u timed-out",
			2, dpnci.dwMessagesTransmittedNormalPriority, dpnci.dwMessagesTimedOutNormalPriority);
		DPTEST_TRACE(hLog, "     Low-priority sends: %u xmited/%u timed-out",
			2, dpnci.dwMessagesTransmittedLowPriority, dpnci.dwMessagesTimedOutLowPriority);
		DPTEST_TRACE(hLog, "     Messages received: %u",
			1, dpnci.dwMessagesReceived);
		DPTEST_TRACE(hLog, "     Guaranteed recvs: %u pkts/%u bytes",
			2, dpnci.dwPacketsReceivedGuaranteed, dpnci.dwBytesReceivedGuaranteed);
		DPTEST_TRACE(hLog, "     Non-guaranteed recvs: %u pkts/%u bytes",
			2, dpnci.dwPacketsReceivedNonGuaranteed, dpnci.dwBytesReceivedGuaranteed);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Check the capabilities of the service provider");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		ZeroMemory(&dpnspc, sizeof(DPN_SP_CAPS));
		dpnspc.dwSize = sizeof(DPN_SP_CAPS);

		tr = pDP8Client->DP8C_GetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspc, 0);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't get service provider caps!", 0);
			THROW_TESTRESULT;
		}
		
		// Output the service provider capabilities
		DPTEST_TRACE(hLog, "Service provider caps:", 0);
		DPTEST_TRACE(hLog, "     Flags: 0x%08x", 1, dpnspc.dwFlags);
		DPTEST_TRACE(hLog, "     Number of threads: %u", 1, dpnspc.dwNumThreads);
		DPTEST_TRACE(hLog, "     Enum count: %u", 1, dpnspc.dwDefaultEnumCount);
		DPTEST_TRACE(hLog, "     Enum retry interval: %u", 1, dpnspc.dwDefaultEnumRetryInterval);
		DPTEST_TRACE(hLog, "     Enum timeout: %u", 1, dpnspc.dwDefaultEnumTimeout);
		DPTEST_TRACE(hLog, "     Max enum payload: %u", 1, dpnspc.dwMaxEnumPayloadSize);
		DPTEST_TRACE(hLog, "     Buffers per thread: %u", 1, dpnspc.dwBuffersPerThread);
		DPTEST_TRACE(hLog, "     System buffer size: %u", 1, dpnspc.dwSystemBufferSize);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set the capabilities of the service provider with indentical info");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_SetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspc, 0);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't set service provider caps!", 0);
			THROW_TESTRESULT;
		}
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Check the capabilities of the DirectPlay");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		ZeroMemory(&dpnc, sizeof(DPN_CAPS));
		dpnc.dwSize = sizeof(DPN_CAPS);

		tr = pDP8Client->DP8C_GetCaps(&dpnc, 0);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't get DirectPlay caps!", 0);
			THROW_TESTRESULT;
		}
		
		// Output the service provider capabilities
		DPTEST_TRACE(hLog, "DirectPlay caps:", 0);
		DPTEST_TRACE(hLog, "     Flags: 0x%08x", 1, dpnc.dwFlags);
		DPTEST_TRACE(hLog, "     Connect timeout: %u", 1, dpnc.dwConnectTimeout);
		DPTEST_TRACE(hLog, "     Connect retries: %u", 1, dpnc.dwConnectRetries);
		DPTEST_TRACE(hLog, "     Keepalive timeout: %u", 1, dpnc.dwTimeoutUntilKeepAlive);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set the capabilities of DirectPlay with indentical info");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_SetCaps(&dpnc, 0);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't set DirectPlay caps!", 0);
			THROW_TESTRESULT;
		}
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Send a message to the host player");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Copy the player buffer
		strncpy(MessageData.szData, SEND_MESSAGE, MAX_MESSAGEDATA_DATA);
		MessageData.szData[MAX_MESSAGEDATA_DATA - 1] = 0;
		MessageData.dwMessageType = SEND_DATA;
		MessageData.dwChecksum = 0;

		// Calculate the checksum
		for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
			MessageData.dwChecksum += MessageData.szData[i];

		// Package up the buffer
		dpnbd.dwBufferSize = sizeof(MESSAGEDATA);
		dpnbd.pBufferData = (BYTE *) &MessageData;

		// Init these variables so we can see if they were modified
		dpnhSendTo = 0x666;
		CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));

		// Once we send data to the server, it will change its player info
		// so be ready for that
		SimpleClientContext.fServerInfoCanChange = TRUE;
		SimpleClientContext.fServerInfoChanged = FALSE;

		tr = pDP8Client->DP8C_Send(&dpnbd, 1, 0, NULL, &dpnhSendTo, 0);

		if (tr != DPNSUCCESS_PENDING)
		{
			DPTEST_FAIL(hLog, "Couldn't send message!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't send message to self)

		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Buffer desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnbdCompare, &dpnbd);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer desc changed)

		if (dpnhSendTo == (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhSendTo == NULL)
		{
			DPTEST_FAIL(hLog, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for server to change information");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleClientContext.hServerInfoChanged))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		// Stop allowing server info changes
		SimpleClientContext.fServerInfoCanChange = FALSE;

		// If we didn't get a server info change, then we're in trouble
		if(!SimpleClientContext.fServerInfoChanged)
		{
			DPTEST_FAIL(hLog, "Server info didn't change!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (server info didn't change)
	
		SimpleClientContext.fServerInfoChanged = FALSE;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify new server info");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwServerDataSize = 0;
		pdpnpiServer = NULL;

		tr = pDP8Client->DP8C_GetServerInfo(NULL, &dwServerDataSize, 0);

		// This function always returns synchronously
		if(tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting server info with NULL buffer on client didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server info)

		LOCALALLOC_OR_THROW(PDPN_PLAYER_INFO, pdpnpiServer, dwServerDataSize);
		pdpnpiServer->dwSize = sizeof(DPN_PLAYER_INFO);

		tr = pDP8Client->DP8C_GetServerInfo(pdpnpiServer, &dwServerDataSize, 0);

		// This function always returns synchronously
		if(tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Getting server info with valid buffer on client didn't return S_OK!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get server info)

		//
		// Now validate the info received
		//

		// First validate that the dwInfoFlags parameter is correctly set
		if(pdpnpiServer->dwInfoFlags ^ (DPNINFO_NAME | DPNINFO_DATA))
		{
			DPTEST_FAIL(hLog, "Info flags didn't have both DPNINFO_NAME and DPNINFO_DATA set!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Next validate the player name
		if(wcscmp(pdpnpiServer->pwszName, SERVER_PLAYER_NAME2))
		{
			DPTEST_FAIL(hLog, "Unexpected server player name! (\"%S\" != \"%S\")",
				2, pdpnpiServer->pwszName, SERVER_PLAYER_NAME2);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Make sure we received enough data
		if(pdpnpiServer->dwDataSize < sizeof(MESSAGEDATA))
		{
			DPTEST_FAIL(hLog, "Received server data is too small! (\"%u\" < \"%u\")",
				2, pdpnpiServer->dwDataSize, sizeof(MESSAGEDATA));
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Verify the player data
		pServerData = NULL;
		if(!(pServerData = (PMESSAGEDATA) pdpnpiServer->pvData))
		{
			DPTEST_FAIL(hLog, "No player data was provided.", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Verify the type of data
		if(pServerData->dwMessageType != PLAYER_DATA)
		{
			DPTEST_FAIL(hLog, "Received player info data of invalid type!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		dwChecksum = 0;

		// Verify the data checksum
		for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
			dwChecksum += pServerData->szData[i];
		
		// Compare it to the checksum listed in the info structure
		if(dwChecksum != pServerData->dwChecksum)
		{
			DPTEST_FAIL(hLog, "Received server info data failed checksum!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Verify the player name is what we expect
		if(strcmp(pServerData->szData, SERVER_DATA2))
		{
			DPTEST_FAIL(hLog, "Unexpected server player data! (\"%s\" != \"%s\")",
				2, pServerData->szData, SERVER_DATA2);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		SAFE_LOCALFREE(pdpnpiServer);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Change client information");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Copy the player buffer
		strncpy(MessageData.szData, CLIENT_DATA2, MAX_MESSAGEDATA_DATA);
		MessageData.szData[MAX_MESSAGEDATA_DATA - 1] = 0;
		MessageData.dwMessageType = PLAYER_DATA;
		MessageData.dwChecksum = 0;

		// Calculate the checksum
		for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
			MessageData.dwChecksum += MessageData.szData[i];

		// Set up the player info structure
		ZeroMemory(&dpnpi, sizeof(DPN_PLAYER_INFO));
		dpnpi.dwSize = sizeof(DPN_PLAYER_INFO);
		dpnpi.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
		dpnpi.dwPlayerFlags = 0;
		dpnpi.dwDataSize = sizeof(MESSAGEDATA);
		dpnpi.pvData = &MessageData;
		dpnpi.pwszName = CLIENT_PLAYER_NAME2;

		CopyMemory(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO));

		SimpleClientContext.fAsyncOpCanComplete = TRUE;
		SimpleClientContext.fAsyncOpCompleted = FALSE;
		SimpleClientContext.hrAsyncOpExpectedResult = S_OK;
		SimpleClientContext.dpnhAsyncOp = (DPNHANDLE) 0x666;

		// Be ready for incoming app desc change message
		// Because server will change it after client changes player info
		SimpleClientContext.fAppDescCanChange = TRUE;
		SimpleClientContext.fAppDescChanged = FALSE;

		tr = pDP8Client->DP8C_SetClientInfo(&dpnpi, NULL, &(SimpleClientContext.dpnhAsyncOp), 0);

		// This function can return synchronously even when called
		// as an async function, so be prepared for that
		if(tr == S_OK)
		{
			DPTEST_TRACE(hLog, "SetClientInfo completed synchronously", 0);
			SetEvent(SimpleClientContext.hAsyncOpCompleted);
			SimpleClientContext.fAsyncOpCompleted = TRUE;
		}
		// If this operation is completing asynchronously we should have a handle
		else if (tr == DPNSUCCESS_PENDING)
		{
			if (SimpleClientContext.dpnhAsyncOp == (DPNHANDLE) 0x666)
			{
				DPTEST_FAIL(hLog, "No async operation handle was returned!", 0);
				SETTHROW_TESTRESULT(ERROR_NO_DATA);
			} // end if (handle was not set)
			
			if (SimpleClientContext.dpnhAsyncOp == NULL)
			{
				DPTEST_FAIL(hLog, "Async operation handle returned is NULL!", 0);
				SETTHROW_TESTRESULT(E_FAIL);
			} // end if (handle was set to NULL)
		}
		// Otherwise, we got an unexpected error
		else
		{
			DPTEST_FAIL(hLog, "Couldn't send message!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't send message to self)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPTEST_FAIL(hLog, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer desc changed)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for set client info operation to complete");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleClientContext.hAsyncOpCompleted))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		// Stop allowing asynchronous operation completions
		SimpleClientContext.fAsyncOpCanComplete = FALSE;

		// If we didn't get an asynchronous operation completion, then we're in trouble
		if(!SimpleClientContext.fAsyncOpCompleted)
		{
			DPTEST_FAIL(hLog, "Didn't receive asynchronous completion for set client info!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (async operation didn't complete)
	
		SimpleClientContext.fAsyncOpCompleted = FALSE;
		SimpleClientContext.dpnhAsyncOp = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for app desc change indication");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleClientContext.hAppDescChanged))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		// Stop allowing app desc changes
		SimpleClientContext.fAppDescCanChange = FALSE;

		// If we didn't get an app desc change, then we're in trouble
		if(!SimpleClientContext.fAppDescChanged)
		{
			DPTEST_FAIL(hLog, "App desc didn't change!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (app desc didn't change)
	
		SimpleClientContext.fAppDescChanged = FALSE;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify current application description");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
		dwAppDescSize = 0;
		pdpnad = NULL;

		tr = pDP8Client->DP8C_GetApplicationDesc(NULL, &dwAppDescSize, 0);

		// This function always returns synchronously
		if(tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting app desc with NULL buffer on client didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		LOCALALLOC_OR_THROW(PDPN_APPLICATION_DESC, pdpnad, dwAppDescSize);
		pdpnad->dwSize = sizeof(DPN_APPLICATION_DESC);

		tr = pDP8Client->DP8C_GetApplicationDesc(pdpnad, &dwAppDescSize, 0);

		// This function always returns synchronously
		if(tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Getting app desc with valid buffer on client didn't return S_OK!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		//
		// Verify the data in the new app desc
		//

		if(pdpnad->dwMaxPlayers != 3)
		{
			DPTEST_FAIL(hLog, "New app desc has unexpected max player limit", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		if(!pdpnad->pwszSessionName)
		{
			DPTEST_FAIL(hLog, "Session name field wasn't set!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		if(wcscmp(pdpnad->pwszSessionName, SERVER_SESSION_NAME2))
		{
			DPTEST_FAIL(hLog, "New app desc has unexpected session name: %S",
				1, pdpnad->pwszSessionName);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		if(pdpnad->pwszPassword)
			DPTEST_TRACE(hLog, "Password field was set as %S", 1, pdpnad->pwszPassword);
		else
			DPTEST_TRACE(hLog, "Password field wasn't set", 0);

		// Make sure we received enough data
		if(pdpnad->dwApplicationReservedDataSize < sizeof(MESSAGEDATA))
		{
			DPTEST_FAIL(hLog, "Received application data is too small! (\"%u\" < \"%u\")",
				2, pdpnad->dwApplicationReservedDataSize, sizeof(MESSAGEDATA));
			SETTHROW_TESTRESULT(E_FAIL);
		}

		if(!(pAppReservedData = (PMESSAGEDATA) pdpnad->pvApplicationReservedData))
		{
			DPTEST_FAIL(hLog, "No reserved data was provided with app desc", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		if(pAppReservedData->dwMessageType != APP_DATA)
		{
			DPTEST_FAIL(hLog, "Received app desc data of invalid type!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		dwChecksum = 0;

		// Calculate the checksum from the message
		for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
			dwChecksum += pAppReservedData->szData[i];
		
		// Compare it to the checksum listed in the message
		if(dwChecksum != pAppReservedData->dwChecksum)
		{
			DPTEST_FAIL(hLog, "Received app desc data failed checksum!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		if(strcmp(pAppReservedData->szData, SERVER_APP_DATA2))
		{
			DPTEST_FAIL(hLog, "Received app desc data has unexpected value", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		SAFE_LOCALFREE(pdpnad);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Check connection info again");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		ZeroMemory(&dpnci, sizeof(DPN_CONNECTION_INFO));
		dpnci.dwSize = sizeof(DPN_CONNECTION_INFO);

		tr = pDP8Client->DP8C_GetConnectionInfo(&dpnci, 0);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't get connection info!", 0);
			THROW_TESTRESULT;
		}

		// Output the connection statistics
		DPTEST_TRACE(hLog, "Connection statistics:", 0);
		DPTEST_TRACE(hLog, "     Latency: %u", 1, dpnci.dwRoundTripLatencyMS);
		DPTEST_TRACE(hLog, "     Throughput: %u", 1, dpnci.dwThroughputBPS);
		DPTEST_TRACE(hLog, "     Peak Throughput: %u", 1, dpnci.dwPeakThroughputBPS);
		DPTEST_TRACE(hLog, "     Guaranteed sends : %u pkts/%u bytes",
			2, dpnci.dwPacketsSentGuaranteed, dpnci.dwBytesSentGuaranteed);
		DPTEST_TRACE(hLog, "     Non-guaranteed sends: %u pkts/%u bytes",
			2, dpnci.dwPacketsSentNonGuaranteed, dpnci.dwBytesSentNonGuaranteed);
		DPTEST_TRACE(hLog, "     Retried sends : %u pkts/%u bytes",
			2, dpnci.dwPacketsRetried, dpnci.dwBytesRetried);
		DPTEST_TRACE(hLog, "     Dropped sends : %u pkts/%u bytes",
			2, dpnci.dwPacketsDropped, dpnci.dwBytesDropped);
		DPTEST_TRACE(hLog, "     High-priority sends : %u xmited/%u timed-out",
			2, dpnci.dwMessagesTransmittedHighPriority, dpnci.dwMessagesTimedOutHighPriority);
		DPTEST_TRACE(hLog, "     Normal-priority sends : %u xmited/%u timed-out",
			2, dpnci.dwMessagesTransmittedNormalPriority, dpnci.dwMessagesTimedOutNormalPriority);
		DPTEST_TRACE(hLog, "     Low-priority sends: %u xmited/%u timed-out",
			2, dpnci.dwMessagesTransmittedLowPriority, dpnci.dwMessagesTimedOutLowPriority);
		DPTEST_TRACE(hLog, "     Messages received: %u",
			1, dpnci.dwMessagesReceived);
		DPTEST_TRACE(hLog, "     Guaranteed recvs: %u pkts/%u bytes",
			2, dpnci.dwPacketsReceivedGuaranteed, dpnci.dwBytesReceivedGuaranteed);
		DPTEST_TRACE(hLog, "     Non-guaranteed recvs: %u pkts/%u bytes",
			2, dpnci.dwPacketsReceivedNonGuaranteed, dpnci.dwBytesReceivedGuaranteed);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Send second message to the host player");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Copy the player buffer
		strncpy(MessageData.szData, SEND_MESSAGE, MAX_MESSAGEDATA_DATA);
		MessageData.szData[MAX_MESSAGEDATA_DATA - 1] = 0;
		MessageData.dwMessageType = SEND_DATA;
		MessageData.dwChecksum = 0;

		// Calculate the checksum
		for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
			MessageData.dwChecksum += MessageData.szData[i];

		// Package up the buffer
		dpnbd.dwBufferSize = sizeof(MESSAGEDATA);
		dpnbd.pBufferData = (BYTE *) &MessageData;

		// Init these variables so we can see if they were modified
		dpnhSendTo = 0x666;
		CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));

		// Be ready for session termination because the server will do a DestroyClient
		// on us when it receives this message
		SimpleClientContext.fSessionCanTerminate = TRUE;
		SimpleClientContext.fSessionTerminated = FALSE;

		tr = pDP8Client->DP8C_Send(&dpnbd, 1, 0, NULL, &dpnhSendTo, 0);

		if (tr != DPNSUCCESS_PENDING)
		{
			DPTEST_FAIL(hLog, "Couldn't send message!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't send message to self)

		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Buffer desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnbdCompare, &dpnbd);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer desc changed)

		if (dpnhSendTo == (DPNHANDLE) 0x666)
		{
			DPTEST_FAIL(hLog, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhSendTo == NULL)
		{
			DPTEST_FAIL(hLog, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for session to be terminated");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleClientContext.hSessionTerminated))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		// Stop allowing session termination messages
		SimpleClientContext.fSessionCanTerminate = FALSE;

		// If we didn't get a session termination message, then we're in trouble
		if(!SimpleClientContext.fSessionTerminated)
		{
			DPTEST_FAIL(hLog, "Session didn't terminate!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (session didn't terminate)
	
		SimpleClientContext.fSessionTerminated = FALSE;

		//
		// Wait for 5 seconds
		//

		Sleep(5000);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		DPTEST_TRACE(hLog, "Releasing client wrapper", 0);
		delete (pDP8Client);
		pDP8Client = NULL;


		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Client != NULL)
	{
		DPTEST_TRACE(hLog, "Releasing client wrapper", 0);
		delete (pDP8Client);
		pDP8Client = NULL;
	} // end if (have client object)

	DPTEST_TRACE(hLog, "Cleaning up test variables", 0);
	SAFE_RELEASE(pDP8AddressHost);
	SAFE_RELEASE(pDP8AddressDevice);
	SAFE_LOCALFREE(pCurrentPlayers);
	SAFE_LOCALFREE(pdpnpiServer);
	SAFE_LOCALFREE(pdpnad);
	ClearDoWorkList(&DoWorkList);
	SAFE_CLOSEHANDLE(SimpleClientContext.hConnectComplete);
	SAFE_CLOSEHANDLE(SimpleClientContext.hIndicateRemotePlayer);
	SAFE_CLOSEHANDLE(SimpleClientContext.hReceiveData);
	SAFE_CLOSEHANDLE(SimpleClientContext.hAsyncOpCompleted);
	SAFE_CLOSEHANDLE(SimpleClientContext.hServerInfoChanged);
	SAFE_CLOSEHANDLE(SimpleClientContext.hAppDescChanged);
	SAFE_CLOSEHANDLE(SimpleClientContext.hSessionTerminated);
	DeleteCriticalSection(&(SimpleClientContext.Lock));


	return (sr);
} // DirectPlayExec_SimpleClient
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#undef DEBUG_SECTION
#define DEBUG_SECTION	"SimpleClientMessageHandler()"
//==================================================================================
// SimpleClientMessageHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT SimpleClientMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT				hr = DPN_OK;
	PSIMPLECLIENTCONTEXT	pContext = (PSIMPLECLIENTCONTEXT) pvContext;
	BOOL				fLocalPlayer = TRUE;
	DWORD				dwStartTime = 0, dwEndTime = 0;

	dwStartTime = GetTickCount();

	switch (dwMsgType)
	{
		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;
			PPLAYERCONTEXT			pNewPlayerContext;

			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;
			
			if(pContext->fVerbose)
			{
				DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
				DPTEST_TRACE(pContext->hLog, "     dwSize = %u", 1, pCreatePlayerMsg->dwSize);
				DPTEST_TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
				DPTEST_TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);
			}

			// If this is a local player, then take the buffer that we allocated when we hosted
			// or connected
			if(pCreatePlayerMsg->pvPlayerContext)
			{
				fLocalPlayer = TRUE;

				pContext->LocalPlayerID = pCreatePlayerMsg->dpnidPlayer;
				pNewPlayerContext = ((PPLAYERCONTEXT) pCreatePlayerMsg->pvPlayerContext);
			}
			// Otherwise this is a remote player.  Create a new buffer.
			else
			{
				fLocalPlayer = FALSE;

				pNewPlayerContext = (PPLAYERCONTEXT) MemAlloc(sizeof(PLAYERCONTEXT));
				if(!pNewPlayerContext)
				{
					DPTEST_TRACE(pContext->hLog, "Not enough memory to allocate player context!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					hr = E_FAIL;
				}

				if(!pContext->fCanAcceptRemotePlayer)
				{
					DPTEST_TRACE(pContext->hLog, "Unexpected remote player creation!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					break;
				}

				pCreatePlayerMsg->pvPlayerContext = pNewPlayerContext;
			}

			// Initialize the player information
			pNewPlayerContext->PlayerID = pCreatePlayerMsg->dpnidPlayer;
			pNewPlayerContext->TotalBytesReceived = 0;
			pNewPlayerContext->TotalMessagesReceived = 0;

			// BUGBUG - figure out why this player will be destroyed.  Need cases for the following
			// DPNDESTROYPLAYERREASON_NORMAL  - The player is being deleted for normal reasons. 
			// DPNDESTROYPLAYERREASON_CONNECTIONLOST - The connection was lost. 
			// DPNDESTROYPLAYERREASON_SESSIONTERMINATED - The session was terminated. 
			// DPNDESTROYPLAYERREASON_HOSTDESTROYEDPLAYER - The host called DestroyPeer
			pNewPlayerContext->dwExpectedDestroyReason = 0;

			// Add info on this player to our global player list
			EnterCriticalSection(&(pContext->Lock));

			pNewPlayerContext->pPrev = NULL;
			if(pContext->pPlayerList)
				pContext->pPlayerList->pPrev = pNewPlayerContext->pNext;
			pNewPlayerContext->pNext = pContext->pPlayerList;
			pContext->pPlayerList = pNewPlayerContext;

			++pContext->dwPlayersConnected;

			LeaveCriticalSection(&(pContext->Lock));

			if(!fLocalPlayer)
			{
				pContext->fAcceptedRemotePlayer = TRUE;
				
				// Signal the main thread that a player was created
				if (! SetEvent(pContext->hIndicateRemotePlayer))
				{
					hr = GetLastError();
					DPTEST_TRACE(pContext->hLog, "Couldn't set remote player indication event (%x)!  DEBUGBREAK()-ing.",
						1, pContext->hIndicateRemotePlayer);
					
					DEBUGBREAK();
					
					if (hr == S_OK)
						hr = E_FAIL;
					
					break;
				} // end if (couldn't set event)
				
			}

		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;
			PPLAYERCONTEXT			pPlayerContext;
			CHAR					szReason[64];

			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;
			
			if(pContext->fVerbose)
			{
				DPTEST_TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
				DPTEST_TRACE(pContext->hLog, "     dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
				DPTEST_TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
				DPTEST_TRACE(pContext->hLog, "     pvPlayerContext = 0x%08x", 1, pDestroyPlayerMsg->pvPlayerContext);
				
				switch(pDestroyPlayerMsg->dwReason)
				{
				case DPNDESTROYPLAYERREASON_NORMAL:
					strcpy(szReason, "NORMAL");
					break;
				case DPNDESTROYPLAYERREASON_CONNECTIONLOST:
					strcpy(szReason, "CONNECTIONLOST");
					break;
				case DPNDESTROYPLAYERREASON_SESSIONTERMINATED:
					strcpy(szReason, "SESSIONTERMINATED");
					break;
				case DPNDESTROYPLAYERREASON_HOSTDESTROYEDPLAYER:
					strcpy(szReason, "HOSTDESTROYEDPLAYER");
					break;
				default:
					DPTEST_TRACE(pContext->hLog, "Unknown player destroy reason! DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					hr = E_FAIL;
					break;
				}
				
				DPTEST_TRACE(pContext->hLog, "     dwReason = %s", 1, szReason);
			}
			
			pPlayerContext = (PPLAYERCONTEXT) pDestroyPlayerMsg->pvPlayerContext;

			if(!pPlayerContext)
			{
				DPTEST_TRACE(pContext->hLog, "No player context received!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			}

			// If we knew how the player was supposed to be destroyed
			if(pPlayerContext->dwExpectedDestroyReason &&
				(pPlayerContext->dwExpectedDestroyReason != pDestroyPlayerMsg->dwReason))
			{
				// Then verify that the way they were destroyed
				DPTEST_TRACE(pContext->hLog, "Player destroyed for unexpected reason! DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
			}

			// Remove this player from our global player list
			EnterCriticalSection(&(pContext->Lock));

			if(pPlayerContext->pNext)
				pPlayerContext->pNext->pPrev = pPlayerContext->pPrev;
			if(pPlayerContext->pPrev)
				pPlayerContext->pPrev->pNext = pPlayerContext->pNext;
			else
				pContext->pPlayerList = pPlayerContext->pNext;

			--pContext->dwPlayersConnected;

			LeaveCriticalSection(&(pContext->Lock));

			// Clear the context value for the local player
			if(pDestroyPlayerMsg->dpnidPlayer == pContext->LocalPlayerID)
				pContext->LocalPlayerID = 0;
			// Only free the context buffer if this isn't a local player
			// otherwise, the main app thread will do it
			else
				MemFree(pPlayerContext);

		break;
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE pConnectCompleteMsg;
			PMESSAGEDATA pReplyData;
			DWORD dwChecksum;

			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;
			
			if(pContext->fVerbose)
			{
				DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
				DPTEST_TRACE(pContext->hLog, "     dwSize = %u", 1, pConnectCompleteMsg->dwSize);
				DPTEST_TRACE(pContext->hLog, "     hAsyncOp = 0x%08x", 1, pConnectCompleteMsg->hAsyncOp);
				// BUGBUG - compare this with what was passed in to connect
				DPTEST_TRACE(pContext->hLog, "     pvUserContext = 0x%08x", 1, pConnectCompleteMsg->pvUserContext);
				DPTEST_TRACE(pContext->hLog, "     hResultCode = 0x%08x", 1, pConnectCompleteMsg->hResultCode);
				DPTEST_TRACE(pContext->hLog, "     pvApplicationReplyData = 0x%08x", 1, pConnectCompleteMsg->pvApplicationReplyData);
				DPTEST_TRACE(pContext->hLog, "     dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);
			}

			if(pConnectCompleteMsg->hAsyncOp != pContext->dpnhConnect)
			{
				DPTEST_TRACE(pContext->hLog, "Connection completed with different async handle!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
			}

			// Parse any reply data if present
			if(pReplyData = (PMESSAGEDATA) pConnectCompleteMsg->pvApplicationReplyData)
			{
				DWORD dwChecksum = 0;

				// Make sure we received enough data
				if(pConnectCompleteMsg->dwApplicationReplyDataSize < sizeof(MESSAGEDATA))
				{
					DPTEST_FAIL(pContext->hLog, "Received application reply data is too small! (\"%u\" < \"%u\")",
						2, pConnectCompleteMsg->dwApplicationReplyDataSize, sizeof(MESSAGEDATA));
					DEBUGBREAK();
					hr = E_FAIL;
				}
				// If the message is of valid size
				else
				{
					if(pReplyData->dwMessageType != CONNECT_REPLY_DATA)
					{
						DPTEST_TRACE(pContext->hLog, "Received connect data of invalid type!  DEBUGBREAK()-ing.", 0);
						DEBUGBREAK();
						hr = E_FAIL;
					}

					// Calculate the checksum from the message
					for(DWORD i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
						dwChecksum += pReplyData->szData[i];

					// Compare it to the checksum listed in the message
					if(dwChecksum != pReplyData->dwChecksum)
					{
						DPTEST_TRACE(pContext->hLog, "Received connect data failed checksum!  DEBUGBREAK()-ing.", 0);
						DEBUGBREAK();
						hr = E_FAIL;
					}

					pContext->fVerbose ?
						DPTEST_TRACE(pContext->hLog, "Connect reply data was : \"%s\"", 0, pReplyData->szData) : 0;
				}

			}

			if(!pContext->fConnectCanComplete)
			{
				DPTEST_TRACE(pContext->hLog, "Unexpected connection indication!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			}

			// Check to see if we got the expected result value
			if(pContext->hrExpectedConnectResult != pConnectCompleteMsg->hResultCode)
			{
				DPTEST_TRACE(pContext->hLog, "Received unexpected connection result (0x%08x != 0x%08x)  DEBUGBREAK()-ing.", 2,
					pContext->hrExpectedConnectResult, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
				hr = E_FAIL;
			}
			else
				pContext->fConnectCompleted = TRUE;

			
			// Signal the main thread that connection completed
			if (! SetEvent(pContext->hConnectComplete))
			{
				hr = GetLastError();
				DPTEST_TRACE(pContext->hLog, "Couldn't set connection indication event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hConnectComplete);
				
				DEBUGBREAK();
				
				if (hr == S_OK)
					hr = E_FAIL;
				
				break;
			} // end if (couldn't set event)

			break;

		case DPN_MSGID_RECEIVE:
			PDPNMSG_RECEIVE pReceiveMsg;

			pReceiveMsg = (PDPNMSG_RECEIVE) pvMsg;
			
			if(pContext->fVerbose)
			{
				DPTEST_TRACE(pContext->hLog, "DPN_MSGID_RECEIVE", 0);
				DPTEST_TRACE(pContext->hLog, "     dwSize = %u", 1, pReceiveMsg->dwSize);
				DPTEST_TRACE(pContext->hLog, "     dpnidSender = %u/%x", 2, pReceiveMsg->dpnidSender, pReceiveMsg->dpnidSender);
				// BUGBUG - verify that the player context matches the player context that was set when
				// this player connected
				DPTEST_TRACE(pContext->hLog, "     pvPlayerContext = 0x%08x", 1, pReceiveMsg->pvPlayerContext);
				// BUGBUG - create a common message format that contains the sender information
				// so that we can verify that this data is accurate
				DPTEST_TRACE(pContext->hLog, "     pReceiveData = 0x%08x", 1, pReceiveMsg->pReceiveData);
				DPTEST_TRACE(pContext->hLog, "     dwReceiveDataSize = %u", 1, pReceiveMsg->dwReceiveDataSize);
				// BUGBUG - try holding onto this buffer and returning it on subsequent calls to receive
				// Use the buffer handle to return it later.  Maybe store the last buffer handle received
				// in the players context.
				DPTEST_TRACE(pContext->hLog, "     hBufferHandle = 0x%08x", 1, pReceiveMsg->hBufferHandle);
			}

			if(!pContext->fCanReceive)
			{
				DPTEST_TRACE(pContext->hLog, "Unexpected incoming message!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			}

			pContext->fReceived = TRUE;
			
			// Signal the main thread that data was received
			if (! SetEvent(pContext->hReceiveData))
			{
				hr = GetLastError();
				DPTEST_TRACE(pContext->hLog, "Couldn't set incoming message indication event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hReceiveData);
				
				DEBUGBREAK();
				
				if (hr == S_OK)
					hr = E_FAIL;
				
				break;
			} // end if (couldn't set event)

			break;
		case DPN_MSGID_SEND_COMPLETE:
			PDPNMSG_SEND_COMPLETE pSendCompleteMsg;

			pSendCompleteMsg = (PDPNMSG_SEND_COMPLETE) pvMsg;
			
			if(pContext->fVerbose)
			{
				DPTEST_TRACE(pContext->hLog, "DPN_MSGID_SEND_COMPLETE", 0);
				DPTEST_TRACE(pContext->hLog, "     dwSize = %u", 1, pSendCompleteMsg->dwSize);
				// BUGBUG - check with the context and verify that this is the handle of a send
				// that we have outstanding
				DPTEST_TRACE(pContext->hLog, "     hAsyncOp = 0x%08x", 1, pSendCompleteMsg->hAsyncOp);
				// BUGBUG - again check with the context and verify that this is the context that we
				// supplied with a call to send
				DPTEST_TRACE(pContext->hLog, "     pvUserContext = 0x%08x", 1, pSendCompleteMsg->pvUserContext);
				// BUGBUG - are there ways to get this value to be a failure?
				DPTEST_TRACE(pContext->hLog, "     hResultCode = 0x%08x", 1, pSendCompleteMsg->hResultCode);
				// BUGBUG - do lots of sends and see if this increases, what about guaranteed?
				DPTEST_TRACE(pContext->hLog, "     dwSendTime = %u ms", 1, pSendCompleteMsg->dwSendTime);
			}

			break;
		case DPN_MSGID_APPLICATION_DESC:
			if(pContext->fVerbose)
			{
				DPTEST_TRACE(pContext->hLog, "DPN_MSGID_APPLICATION_DESC", 0);
			}

			if(!pContext->fAppDescCanChange)
			{
				DPTEST_TRACE(pContext->hLog, "Unexpected app desc change!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			}

			pContext->fAppDescChanged = TRUE;
			
			// Signal the main thread that the app desc changed
			if (! SetEvent(pContext->hAppDescChanged))
			{
				hr = GetLastError();
				DPTEST_TRACE(pContext->hLog, "Couldn't set app desc change indication event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hAppDescChanged);
				
				DEBUGBREAK();
				
				if (hr == S_OK)
					hr = E_FAIL;
				
				break;
			} // end if (couldn't set event)

			break;
		case DPN_MSGID_ASYNC_OP_COMPLETE:
			PDPNMSG_ASYNC_OP_COMPLETE pAsyncOpCompleteMsg;

			pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE) pvMsg;
			
			if(pContext->fVerbose)
			{
				DPTEST_TRACE(pContext->hLog, "DPN_MSGID_ASYNC_OP_COMPLETE", 0);
				DPTEST_TRACE(pContext->hLog, "     dwSize = %u", 1, pAsyncOpCompleteMsg->dwSize);
				// BUGBUG - check with the context and verify that this is the handle of an operation
				// that we have outstanding
				DPTEST_TRACE(pContext->hLog, "     hAsyncOp = %u", 1, pAsyncOpCompleteMsg->hAsyncOp);
				// BUGBUG - are there ways to get this value to be a failure?
				DPTEST_TRACE(pContext->hLog, "     hResultCode = 0x%08x", 1, pAsyncOpCompleteMsg->hResultCode);
			}

			if(!pContext->fAsyncOpCanComplete)
			{
				DPTEST_TRACE(pContext->hLog, "Unexpected asynchronous operation completion!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			}

			if(pContext->dpnhAsyncOp != pAsyncOpCompleteMsg->hAsyncOp)
			{
				DPTEST_TRACE(pContext->hLog, "Asynchronous operation completion has unexpected handle (0x%08x != 0x%08x)!  DEBUGBREAK()-ing.", 2,
					pContext->dpnhAsyncOp, pAsyncOpCompleteMsg->hAsyncOp);
				DEBUGBREAK();
				break;
			}

			if(pContext->hrAsyncOpExpectedResult != pAsyncOpCompleteMsg->hResultCode)
			{
				DPTEST_TRACE(pContext->hLog, "Asynchronous operation completed with unexpected result (0x%08x != 0x%08x)!  DEBUGBREAK()-ing.", 2,
					pContext->hrAsyncOpExpectedResult, pAsyncOpCompleteMsg->hResultCode);
				DEBUGBREAK();
				break;
			}

			pContext->fAsyncOpCompleted = TRUE;
			
			// Signal the main thread that an async op completed
			if (! SetEvent(pContext->hAsyncOpCompleted))
			{
				hr = GetLastError();
				DPTEST_TRACE(pContext->hLog, "Couldn't set incoming message indication event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hAsyncOpCompleted);
				
				DEBUGBREAK();
				
				if (hr == S_OK)
					hr = E_FAIL;
				
				break;
			} // end if (couldn't set event)

			break;
		case DPN_MSGID_SERVER_INFO:
			PDPNMSG_SERVER_INFO pServerInfoMsg;

			pServerInfoMsg = (PDPNMSG_SERVER_INFO) pvMsg;
			
			if(pContext->fVerbose)
			{
				DPTEST_TRACE(pContext->hLog, "DPN_MSGID_SERVER_INFO", 0);
				DPTEST_TRACE(pContext->hLog, "     dwSize = %u", 1, pServerInfoMsg->dwSize);
				// BUGBUG - Not sure why DPlay does this...  The whole concept of a client
				// is supposed to be free of the notion of DPNID's and player contexts
				// So why are we provided one here?
				DPTEST_TRACE(pContext->hLog, "     dpnidServer = %u/%x", 2, pServerInfoMsg->dpnidServer, pServerInfoMsg->dpnidServer);
				DPTEST_TRACE(pContext->hLog, "     pvPlayerContext = 0x%08x", 1, pServerInfoMsg->pvPlayerContext);
			}

			if(!pContext->fServerInfoCanChange)
			{
				DPTEST_TRACE(pContext->hLog, "Unexpected server info change!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			}

			pContext->fServerInfoChanged = TRUE;
			
			// Signal the main thread that the server info changed
			if (! SetEvent(pContext->hServerInfoChanged))
			{
				hr = GetLastError();
				DPTEST_TRACE(pContext->hLog, "Couldn't set server info change indication event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hServerInfoChanged);
				
				DEBUGBREAK();
				
				if (hr == S_OK)
					hr = E_FAIL;
				
				break;
			} // end if (couldn't set event)

			break;

		case DPN_MSGID_TERMINATE_SESSION:
			PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;

			pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION) pvMsg;
			
			if(pContext->fVerbose)
			{
				DPTEST_TRACE(pContext->hLog, "DPN_MSGID_TERMINATE_SESSION", 0);
				DPTEST_TRACE(pContext->hLog, "     dwSize = %u", 1, pTerminateSessionMsg->dwSize);
				// BUGBUG - check the context and verify the difference between the host calling\
				// close and the host calling terminate session.
				DPTEST_TRACE(pContext->hLog, "     hResultCode = 0x%08x", 1, pTerminateSessionMsg->hResultCode);
				// BUGBUG - check with the context and verify whether or not data should have been provided
				DPTEST_TRACE(pContext->hLog, "     pvTerminateData = 0x%08x", 1, pTerminateSessionMsg->pvTerminateData);
				DPTEST_TRACE(pContext->hLog, "     dwTerminateDataSize = %u", 1, pTerminateSessionMsg->dwTerminateDataSize);
			}

			if(!pContext->fSessionCanTerminate)
			{
				DPTEST_TRACE(pContext->hLog, "Unexpected session termination!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			}

			pContext->fSessionTerminated = TRUE;
			
			// Signal the main thread that the session terminated
			if (! SetEvent(pContext->hSessionTerminated))
			{
				hr = GetLastError();
				DPTEST_TRACE(pContext->hLog, "Couldn't set session termination event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hAppDescChanged);
				
				DEBUGBREAK();
				
				if (hr == S_OK)
					hr = E_FAIL;
				
				break;
			} // end if (couldn't set event)

			break;

		case DPN_MSGID_RETURN_BUFFER:
			PDPNMSG_RETURN_BUFFER pReturnBufferMsg;

			pReturnBufferMsg = (PDPNMSG_RETURN_BUFFER) pvMsg;
			
			if(pContext->fVerbose)
			{
				DPTEST_TRACE(pContext->hLog, "DPN_MSGID_RETURN_BUFFER", 0);
				DPTEST_TRACE(pContext->hLog, "     dwSize = %u", 1, pReturnBufferMsg->dwSize);
				// BUGBUG - in enum cases, this could be an error, otherwise, should be success
				DPTEST_TRACE(pContext->hLog, "     hResultCode = 0x%08x", 1, pReturnBufferMsg->hResultCode);
				DPTEST_TRACE(pContext->hLog, "     pvBuffer = 0x%08x", 1, pReturnBufferMsg->pvBuffer);
				// BUGBUG - this should probably only be set in concurrent enumeration cases
				DPTEST_TRACE(pContext->hLog, "     pvUserContext = 0x%08x", 1, pReturnBufferMsg->pvUserContext);
			}

			if(!pReturnBufferMsg->pvBuffer)
			{
				DPTEST_TRACE(pContext->hLog, "Return buffer message with invalid buffer!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
			}

			MemFree(pReturnBufferMsg->pvBuffer);

			break;
		default:
			DPTEST_TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	dwEndTime = GetTickCount();

	pContext->dwCallbackTime += (dwEndTime >= dwStartTime) ? dwEndTime - dwStartTime : (0xFFFFFFFF - dwStartTime) + dwEndTime;

	return (hr);
} // SimpleClientMessageHandler
#undef DEBUG_SECTION

} // namespace DPlayCoreNamespace