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
#define DEBUG_SECTION	"DirectPlayExec_SimpleServer()"
//==================================================================================
// DirectPlayExec_SimpleServer
//----------------------------------------------------------------------------------
//
// Description: Implements the DirectPlay simple server test
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
HRESULT DirectPlayExec_SimpleServer(HANDLE hLog, PDP_HOSTINFO pHostInfo, WORD LowPort)
{
	CTNSystemResult			sr;
	CTNTestResult			tr;
	PWRAPDP8SERVER			pDP8Server = NULL;
	PDIRECTPLAY8ADDRESS		pDP8DeviceAddress = NULL;
	DPN_APPLICATION_DESC	dpnad, *pdpnad = NULL;
	DPN_APPLICATION_DESC	dpnadCompare;
	DPN_BUFFER_DESC			dpnbd;
	DPN_BUFFER_DESC			dpnbdCompare;
	DPN_PLAYER_INFO			dpnpi, *pdpnpiClient = NULL;
	DPN_PLAYER_INFO			dpnpiCompare;
	DPN_CONNECTION_INFO		dpnci;
	DPN_SP_CAPS				dpnspc;
	DPN_CAPS				dpnc;
	DP_DOWORKLIST			DoWorkList;
	SIMPLESERVERCONTEXT		SimpleServerContext;
	DWORD					dwPort, i, dwNumIDs, dwChecksum, dwAppDescSize, dwNumMsgs, dwNumBytes, dwClientDataSize;
	PPLAYERCONTEXT			pLocalPlayerContext = NULL, pTempContext = NULL;
	DPNID					SendTarget, *pCurrentPlayers = NULL;
	MESSAGEDATA				MessageData, *pClientInfo = NULL;
	DPNHANDLE				dpnhSendTo;
	PVOID					pCurrentPlayerContext = NULL;

	ZeroMemory(&SimpleServerContext, sizeof (SIMPLESERVERCONTEXT));
	InitializeCriticalSection(&(SimpleServerContext.Lock));

	ZeroMemory(&DoWorkList, sizeof(DP_DOWORKLIST));

	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Server = new CWrapDP8Server(hLog);
		if (pDP8Server == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Server->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("CoCreating first DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8DeviceAddress, NULL);

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate first DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing with context, 0 flags");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		SimpleServerContext.hLog = hLog;
		SimpleServerContext.fVerbose = FALSE;
		SimpleServerContext.LocalPlayerID = 0;
		CREATEEVENT_OR_THROW(SimpleServerContext.hIndicateConnect, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(SimpleServerContext.hIndicateRemotePlayer, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(SimpleServerContext.hReceiveData, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(SimpleServerContext.hAsyncOpCompleted, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(SimpleServerContext.hClientInfoChanged, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(SimpleServerContext.hAppDescChanged, NULL, FALSE, FALSE, NULL);

		tr = pDP8Server->DP8S_Initialize(&SimpleServerContext, SimpleServerMessageHandler, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Initializing with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("DoWork on newly initialized server");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->m_pDP8Server->DoWork(0);
		if((tr != S_OK) && (tr != S_FALSE))
		{
			DPTEST_FAIL(hLog, "DoWork on server just after initialization returned 0x%08x", 1, tr);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting port for hosted session");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
//		dwPort = (DWORD) LowPort;
		dwPort = 2302;

		if(pDP8DeviceAddress->AddComponent(DPNA_KEY_PORT, &dwPort, sizeof(dwPort), DPNA_DATATYPE_DWORD) != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't set port for host", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting initial server information");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Copy the player buffer
		strncpy(MessageData.szData, SERVER_DATA1, MAX_MESSAGEDATA_DATA);
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
		dpnpi.pwszName = SERVER_PLAYER_NAME1;

		CopyMemory(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO));

		SimpleServerContext.fAsyncOpCanComplete = TRUE;
		SimpleServerContext.fAsyncOpCompleted = FALSE;
		SimpleServerContext.hrAsyncOpExpectedResult = S_OK;
		SimpleServerContext.dpnhAsyncOp = (DPNHANDLE) 0x666;

		tr = pDP8Server->DP8S_SetServerInfo(&dpnpi, NULL, &(SimpleServerContext.dpnhAsyncOp), 0);

		// This function can return synchronously even when called
		// as an async function, so be prepared for that
		if(tr == S_OK)
		{
			DPTEST_TRACE(hLog, "SetServerInfo completed synchronously", 0);
			SetEvent(SimpleServerContext.hAsyncOpCompleted);
			SimpleServerContext.fAsyncOpCompleted = TRUE;
		}
		// If this operation is completing asynchronously we should have a handle
		else if (tr == DPNSUCCESS_PENDING)
		{
			if (SimpleServerContext.dpnhAsyncOp == (DPNHANDLE) 0x666)
			{
				DPTEST_FAIL(hLog, "No async operation handle was returned!", 0);
				SETTHROW_TESTRESULT(ERROR_NO_DATA);
			} // end if (handle was not set)
			
			if (SimpleServerContext.dpnhAsyncOp == NULL)
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
		TEST_SECTION("Waiting for initial set server info operation to complete");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Set up DoWork time parameters
		memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
		DoWorkList.fTrackDoWorkTimes = TRUE;
		DoWorkList.dwNumServers = 1;
		DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;
		DoWorkList.apdwServerCallbackTimes = (DWORD **) MemAlloc(sizeof(LPVOID) * 1);
		DoWorkList.apdwServerCallbackTimes[0] = &(SimpleServerContext.dwCallbackTime);

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleServerContext.hAsyncOpCompleted))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Stop allowing asynchronous operation completions
		SimpleServerContext.fAsyncOpCanComplete = FALSE;

		// If we didn't get an asynchronous operation completion, then we're in trouble
		if(!SimpleServerContext.fAsyncOpCompleted)
		{
			DPTEST_FAIL(hLog, "Didn't receive asynchronous completion for set server info!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (async operation didn't complete)
	
		SimpleServerContext.fAsyncOpCompleted = FALSE;
		SimpleServerContext.dpnhAsyncOp = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Hosting with 1 device");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.guidApplication = GUID_PARMV_PEER_HOST;
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC));

		// Allocate a local player context.  Callback will deallocate on destroy player message
		pLocalPlayerContext = NULL;
		LOCALALLOC_OR_THROW(PPLAYERCONTEXT, pLocalPlayerContext, sizeof(PLAYERCONTEXT));

		// Allow incoming connections
		SimpleServerContext.fCanAcceptConnect = TRUE;
		SimpleServerContext.fAcceptedConnect = FALSE;
		
		// Allow remote player creations
		SimpleServerContext.fCanAcceptRemotePlayer = TRUE;
		SimpleServerContext.fAcceptedRemotePlayer = FALSE;

		tr = pDP8Server->DP8S_Host(&dpnad, &pDP8DeviceAddress, 1, NULL, NULL, pLocalPlayerContext, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Hosting with 1 device failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpnadCompare, &dpnad, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPTEST_FAIL(hLog, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpnadCompare, &dpnad);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for incoming connection to be indicated");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleServerContext.hIndicateConnect))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		// Stop allowing connections
		SimpleServerContext.fCanAcceptConnect = FALSE;

		// If we didn't get a connection, then we're in trouble
		if(!SimpleServerContext.fAcceptedConnect)
		{
			DPTEST_FAIL(hLog, "No connection indicated!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (no connection accepted)
	
		SimpleServerContext.fAcceptedConnect = FALSE;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for remote player to be created");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleServerContext.hIndicateRemotePlayer))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		// Stop allowing remote player creations
		SimpleServerContext.fCanAcceptRemotePlayer = FALSE;

		// If we didn't get a remote player creation, then we're in trouble
		if(!SimpleServerContext.fAcceptedRemotePlayer)
		{
			DPTEST_FAIL(hLog, "No remote player creations indicated!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (no remote player accepted)
	
		SimpleServerContext.fAcceptedRemotePlayer = FALSE;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Enumerate players");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwNumIDs = 0;

		tr = pDP8Server->DP8S_EnumPlayersAndGroups(NULL, &dwNumIDs, DPNENUM_PLAYERS);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Enuming players with NULL buffer didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 2)
		{
			DPTEST_FAIL(hLog, "Number of IDs was not expected (%u != 2)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)

		LOCALALLOC_OR_THROW(PDPNID, pCurrentPlayers, dwNumIDs * sizeof(DPNID));

		tr = pDP8Server->DP8S_EnumPlayersAndGroups(pCurrentPlayers, &dwNumIDs, DPNENUM_PLAYERS);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Enuming players with valid buffer didn't return success!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 2)
		{
			DPTEST_FAIL(hLog, "Number of IDs was not expected (%u != 2)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify player contexts");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Validate each player in the enumeration
		for(i = 0; i < dwNumIDs; ++i)
		{
			for(pTempContext = SimpleServerContext.pPlayerList; pTempContext; pTempContext = pTempContext->pNext)
			{
				if(pTempContext->PlayerID == pCurrentPlayers[i])
					break;
			}

			if(!pTempContext)
			{
				DPTEST_FAIL(hLog, "Enumerated player 0x%08x wasn't found in our local list", 0, pCurrentPlayers[i]);
				THROW_TESTRESULT;
			}

			tr = pDP8Server->DP8S_GetPlayerContext(pCurrentPlayers[i], &pCurrentPlayerContext, 0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't get player context for player 0x%08x",
					1, pCurrentPlayers[i]);
				THROW_TESTRESULT;
			} // end if (couldn't enum players and groups)

			if(pTempContext != pCurrentPlayerContext)
			{
				DPTEST_FAIL(hLog, "Context returned for player 0x%08x was incorrect (0x%08x != 0x%08x)",
					3, pCurrentPlayers[i], pTempContext, pCurrentPlayerContext);
				THROW_TESTRESULT;
			}

			DPTEST_TRACE(hLog, "Successfully retreived player context for player 0x%08x",
				1, pCurrentPlayers[i]);
		}

		DPTEST_TRACE(hLog, "Found all %u players in our local list", 0, dwNumIDs);


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Pick a send target");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		EnterCriticalSection(&(SimpleServerContext.Lock));

		// Find the first non-local player and set them as the send target
		SendTarget = 0;
		for(pTempContext = SimpleServerContext.pPlayerList; pTempContext && !SendTarget; pTempContext = pTempContext->pNext)
		{
			if(pTempContext->PlayerID != SimpleServerContext.LocalPlayerID)
				SendTarget = pTempContext->PlayerID;
		}
		LeaveCriticalSection(&(SimpleServerContext.Lock));

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Check send queue before sending");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumMsgs = dwNumBytes = 0x666;

		tr = pDP8Server->DP8S_GetSendQueueInfo(SendTarget, &dwNumMsgs, &dwNumBytes, 0);

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

		tr = pDP8Server->DP8S_GetConnectionInfo(SendTarget, &dpnci, 0);

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

		tr = pDP8Server->DP8S_GetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspc, 0);

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

		tr = pDP8Server->DP8S_SetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspc, 0);

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

		tr = pDP8Server->DP8S_GetCaps(&dpnc, 0);

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

		tr = pDP8Server->DP8S_SetCaps(&dpnc, 0);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't set DirectPlay caps!", 0);
			THROW_TESTRESULT;
		}
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Send a message to the newly connected player");
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

		SimpleServerContext.fCanReceive = TRUE;
		SimpleServerContext.fReceived = FALSE;

		tr = pDP8Server->DP8S_SendTo(SendTarget, &dpnbd, 1, 0, NULL, &dpnhSendTo, 0);

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
		TEST_SECTION("Waiting for message from remote player");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleServerContext.hReceiveData))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		// Stop allowing incoming messages
		SimpleServerContext.fCanReceive = FALSE;

		// If we didn't get an incoming message, then we're in trouble
		if(!SimpleServerContext.fReceived)
		{
			DPTEST_FAIL(hLog, "No messages received!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (no message received)
	
		SimpleServerContext.fCanReceive = FALSE;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify client info");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwClientDataSize = 0;
		pdpnpiClient = NULL;

		tr = pDP8Server->DP8S_GetClientInfo(SendTarget, NULL, &dwClientDataSize, 0);

		// This function always returns synchronously
		if(tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting client info with NULL buffer on server didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		LOCALALLOC_OR_THROW(PDPN_PLAYER_INFO, pdpnpiClient, dwClientDataSize);
		pdpnpiClient->dwSize = sizeof(DPN_PLAYER_INFO);

		tr = pDP8Server->DP8S_GetClientInfo(SendTarget, pdpnpiClient, &dwClientDataSize, 0);

		// This function always returns synchronously
		if(tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Getting client info with valid buffer on server didn't return S_OK!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		//
		// Now validate the info received
		//

		// First validate that the dwInfoFlags parameter is correctly set
		if(pdpnpiClient->dwInfoFlags ^ (DPNINFO_NAME | DPNINFO_DATA))
		{
			DPTEST_FAIL(hLog, "Info flags didn't have both DPNINFO_NAME and DPNINFO_DATA set!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Next validate the player name
		if(wcscmp(pdpnpiClient->pwszName, CLIENT_PLAYER_NAME1))
		{
			DPTEST_FAIL(hLog, "Unexpected client player name! (\"%S\" != \"%S\")",
				2, pdpnpiClient->pwszName, CLIENT_PLAYER_NAME1);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Make sure we received enough data
		if(pdpnpiClient->dwDataSize < sizeof(MESSAGEDATA))
		{
			DPTEST_FAIL(hLog, "Received client data is too small! (\"%u\" < \"%u\")",
				2, pdpnpiClient->dwDataSize, sizeof(MESSAGEDATA));
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Verify the player data
		pClientInfo = NULL;
		if(!(pClientInfo = (PMESSAGEDATA) pdpnpiClient->pvData))
		{
			DPTEST_FAIL(hLog, "No player data was provided.", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Verify the type of data
		if(pClientInfo->dwMessageType != PLAYER_DATA)
		{
			DPTEST_FAIL(hLog, "Received player info data of invalid type!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		dwChecksum = 0;

		// Verify the data checksum
		for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
			dwChecksum += pClientInfo->szData[i];
		
		// Compare it to the checksum listed in the info structure
		if(dwChecksum != pClientInfo->dwChecksum)
		{
			DPTEST_FAIL(hLog, "Received client info data failed checksum!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Verify the player name is what we expect
		if(strcmp(pClientInfo->szData, CLIENT_DATA1))
		{
			DPTEST_FAIL(hLog, "Unexpected client player data! (\"%s\" != \"%s\")",
				2, pClientInfo->szData, CLIENT_DATA1);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		SAFE_LOCALFREE(pdpnpiClient);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Change server information");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Copy the player buffer
		strncpy(MessageData.szData, SERVER_DATA2, MAX_MESSAGEDATA_DATA);
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
		dpnpi.pwszName = SERVER_PLAYER_NAME2;

		CopyMemory(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO));

		// Be ready for asychronous completion
		SimpleServerContext.fAsyncOpCanComplete = TRUE;
		SimpleServerContext.fAsyncOpCompleted = FALSE;
		SimpleServerContext.hrAsyncOpExpectedResult = S_OK;
		SimpleServerContext.dpnhAsyncOp = (DPNHANDLE) 0x666;

		// Be ready for client changing info in response to server info change
		SimpleServerContext.fClientInfoCanChange = TRUE;
		SimpleServerContext.fClientInfoChanged = FALSE;

		tr = pDP8Server->DP8S_SetServerInfo(&dpnpi, NULL, &(SimpleServerContext.dpnhAsyncOp), 0);

		// This function can return synchronously even when called
		// as an async function, so be prepared for that
		if(tr == S_OK)
		{
			DPTEST_TRACE(hLog, "SetServerInfo completed synchronously", 0);
			SetEvent(SimpleServerContext.hAsyncOpCompleted);
			SimpleServerContext.fAsyncOpCompleted = TRUE;
		}
		// If this operation is completing asynchronously we should have a handle
		else if (tr == DPNSUCCESS_PENDING)
		{
			if (SimpleServerContext.dpnhAsyncOp == (DPNHANDLE) 0x666)
			{
				DPTEST_FAIL(hLog, "No async operation handle was returned!", 0);
				SETTHROW_TESTRESULT(ERROR_NO_DATA);
			} // end if (handle was not set)
			
			if (SimpleServerContext.dpnhAsyncOp == NULL)
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
		TEST_SECTION("Waiting for set server info operation to complete");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleServerContext.hAsyncOpCompleted))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		// Stop allowing async operations to complete
		SimpleServerContext.fAsyncOpCanComplete = FALSE;

		// If we didn't get an asynchronous operation completion, then we're in trouble
		if(!SimpleServerContext.fAsyncOpCompleted)
		{
			DPTEST_FAIL(hLog, "Didn't receive asynchronous completion for set server info!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (async operation didn't complete)
	
		SimpleServerContext.fAsyncOpCompleted = FALSE;
		SimpleServerContext.dpnhAsyncOp = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for client to change information");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleServerContext.hClientInfoChanged))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		// Stop allowing client info changes
		SimpleServerContext.fClientInfoCanChange = FALSE;

		// If we didn't get a client info change, then we're in trouble
		if(!SimpleServerContext.fClientInfoChanged)
		{
			DPTEST_FAIL(hLog, "Client info didn't change!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (client info didn't change)
	
		SimpleServerContext.fClientInfoChanged = FALSE;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify new client info");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwClientDataSize = 0;
		pdpnpiClient = NULL;

		tr = pDP8Server->DP8S_GetClientInfo(SendTarget, NULL, &dwClientDataSize, 0);

		// This function always returns synchronously
		if(tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting client info with NULL buffer on client didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		LOCALALLOC_OR_THROW(PDPN_PLAYER_INFO, pdpnpiClient, dwClientDataSize);
		pdpnpiClient->dwSize = sizeof(DPN_PLAYER_INFO);

		tr = pDP8Server->DP8S_GetClientInfo(SendTarget, pdpnpiClient, &dwClientDataSize, 0);

		// This function always returns synchronously
		if(tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Getting client info with NULL buffer on client didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get client info)

		//
		// Now validate the info received
		//

		// First validate that the dwInfoFlags parameter is correctly set
		if(pdpnpiClient->dwInfoFlags ^ (DPNINFO_NAME | DPNINFO_DATA))
		{
			DPTEST_FAIL(hLog, "Info flags didn't have both DPNINFO_NAME and DPNINFO_DATA set!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Next validate the player name
		if(wcscmp(pdpnpiClient->pwszName, CLIENT_PLAYER_NAME2))
		{
			DPTEST_FAIL(hLog, "Unexpected client player name! (\"%S\" != \"%S\")",
				2, pdpnpiClient->pwszName, CLIENT_PLAYER_NAME2);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Make sure we received enough data
		if(pdpnpiClient->dwDataSize < sizeof(MESSAGEDATA))
		{
			DPTEST_FAIL(hLog, "Received client data is too small! (\"%u\" < \"%u\")",
				2, pdpnpiClient->dwDataSize, sizeof(MESSAGEDATA));
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Verify the player data
		pClientInfo = NULL;
		if(!(pClientInfo = (PMESSAGEDATA) pdpnpiClient->pvData))
		{
			DPTEST_FAIL(hLog, "No player data was provided.", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Verify the type of data
		if(pClientInfo->dwMessageType != PLAYER_DATA)
		{
			DPTEST_FAIL(hLog, "Received player info data of invalid type!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		dwChecksum = 0;

		// Verify the data checksum
		for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
			dwChecksum += pClientInfo->szData[i];
		
		// Compare it to the checksum listed in the info structure
		if(dwChecksum != pClientInfo->dwChecksum)
		{
			DPTEST_FAIL(hLog, "Received client info data failed checksum!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		// Verify the player name is what we expect
		if(strcmp(pClientInfo->szData, CLIENT_DATA2))
		{
			DPTEST_FAIL(hLog, "Unexpected client player data! (\"%s\" != \"%s\")",
				2, pClientInfo->szData, CLIENT_DATA2);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		SAFE_LOCALFREE(pdpnpiClient);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify current application description");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
		dwAppDescSize = 0;
		pdpnad = NULL;

		tr = pDP8Server->DP8S_GetApplicationDesc(NULL, &dwAppDescSize, 0);

		// This function always returns synchronously
		if(tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting app desc with NULL buffer on server didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		LOCALALLOC_OR_THROW(PDPN_APPLICATION_DESC, pdpnad, dwAppDescSize);
		pdpnad->dwSize = sizeof(DPN_APPLICATION_DESC);

		tr = pDP8Server->DP8S_GetApplicationDesc(pdpnad, &dwAppDescSize, 0);

		// This function always returns synchronously
		if(tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Getting app desc with valid buffer on server didn't return S_OK!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		if(dpnadCompare.guidApplication != pdpnad->guidApplication)
		{
			DPTEST_FAIL(hLog, "Retreived app desc structure has different app GUID!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		if(pdpnad->pwszSessionName)
		{
			DPTEST_FAIL(hLog, "Retreived app desc structure has session name!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

		if(pdpnad->guidInstance == GUID_NULL)
		{
			DPTEST_FAIL(hLog, "Retreived app desc structure has NULL instance GUID", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}

// BUGBUG - Later, add more data to the original application descriptor...
// then validate that data here

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Change current application description");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Copy the player buffer
		strncpy(MessageData.szData, SERVER_APP_DATA2, MAX_MESSAGEDATA_DATA);
		MessageData.szData[MAX_MESSAGEDATA_DATA - 1] = 0;
		MessageData.dwMessageType = APP_DATA;
		MessageData.dwChecksum = 0;

		// Calculate the checksum
		for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
			MessageData.dwChecksum += MessageData.szData[i];

		// Set up new app desc structure
		pdpnad->dwSize = sizeof(DPN_APPLICATION_DESC);
		pdpnad->dwFlags = DPNSESSION_CLIENT_SERVER | DPNSESSION_REQUIREPASSWORD;
		pdpnad->dwMaxPlayers = 3;
		pdpnad->pwszSessionName = SERVER_SESSION_NAME2;
		pdpnad->pwszPassword = SERVER_PASSWORD2;
		pdpnad->pvApplicationReservedData = &MessageData;
		pdpnad->dwApplicationReservedDataSize = sizeof(MESSAGEDATA);

		// Be ready for incoming app desc change message
		SimpleServerContext.fAppDescCanChange = TRUE;
		SimpleServerContext.fAppDescChanged = FALSE;

		// Be ready for the client to send a message in response to the app change 
		SimpleServerContext.fCanReceive = TRUE;
		SimpleServerContext.fReceived = FALSE;

		tr = pDP8Server->DP8S_SetApplicationDesc(pdpnad, 0);

		// This function always returns synchronously
		if(tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Setting app desc with valid buffer on server didn't return S_OK!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get app desc)

		SAFE_LOCALFREE(pdpnad);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for app desc change indication");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleServerContext.hAppDescChanged))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		// Stop allowing app desc changes
		SimpleServerContext.fAppDescCanChange = FALSE;

		// If we didn't get an app desc change, then we're in trouble
		if(!SimpleServerContext.fAppDescChanged)
		{
			DPTEST_FAIL(hLog, "App desc didn't change!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (app desc didn't change)
	
		SimpleServerContext.fAppDescChanged = FALSE;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for second message from remote player");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, SimpleServerContext.hReceiveData))
		{
			DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		}
		
		// Stop allowing incoming messages
		SimpleServerContext.fCanReceive = FALSE;

		// If we didn't get an incoming message, then we're in trouble
		if(!SimpleServerContext.fReceived)
		{
			DPTEST_FAIL(hLog, "No messages received!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (no message received)
	
		SimpleServerContext.fCanReceive = FALSE;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Check connection info again");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		ZeroMemory(&dpnci, sizeof(DPN_CONNECTION_INFO));
		dpnci.dwSize = sizeof(DPN_CONNECTION_INFO);

		tr = pDP8Server->DP8S_GetConnectionInfo(SendTarget, &dpnci, 0);

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
		TEST_SECTION("Destroy connected client");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Copy the player buffer
		strncpy(MessageData.szData, DESTROY_MESSAGE, MAX_MESSAGEDATA_DATA);
		MessageData.szData[MAX_MESSAGEDATA_DATA - 1] = 0;
		MessageData.dwMessageType = TERMINATE_DATA;
		MessageData.dwChecksum = 0;

		// Calculate the checksum
		for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
			MessageData.dwChecksum += MessageData.szData[i];

		tr = pDP8Server->DP8S_DestroyClient(SendTarget, &MessageData, sizeof(MESSAGEDATA), 0);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't destroy client!", 0);
			THROW_TESTRESULT;
		}

		//
		// Wait for 5 seconds
		//

		Sleep(5000);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->DP8S_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Server object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Server->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		DPTEST_TRACE(hLog, "Releasing server wrapper", 0);
		delete (pDP8Server);
		pDP8Server = NULL;

		FINAL_SUCCESS;
	}
	END_TESTCASE

	if (pDP8Server != NULL)
	{
		DPTEST_TRACE(hLog, "Releasing server wrapper", 0);
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)

	DPTEST_TRACE(hLog, "Cleaning up test variables", 0);
	SAFE_RELEASE(pDP8DeviceAddress);
	SAFE_LOCALFREE(pCurrentPlayers);
	SAFE_LOCALFREE(pdpnpiClient);
	SAFE_LOCALFREE(pdpnad);
	ClearDoWorkList(&DoWorkList);
	SAFE_CLOSEHANDLE(SimpleServerContext.hIndicateConnect);
	SAFE_CLOSEHANDLE(SimpleServerContext.hIndicateRemotePlayer);
	SAFE_CLOSEHANDLE(SimpleServerContext.hReceiveData);
	SAFE_CLOSEHANDLE(SimpleServerContext.hAsyncOpCompleted);
	SAFE_CLOSEHANDLE(SimpleServerContext.hClientInfoChanged);
	SAFE_CLOSEHANDLE(SimpleServerContext.hAppDescChanged);
	DeleteCriticalSection(&(SimpleServerContext.Lock));

	return (sr);
} // DirectPlayExec_SimpleServer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#undef DEBUG_SECTION
#define DEBUG_SECTION	"SimpleServerMessageHandler()"
//==================================================================================
// SimpleServerMessageHandler
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
HRESULT SimpleServerMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT					hr = DPN_OK;
	PSIMPLESERVERCONTEXT	pContext = (PSIMPLESERVERCONTEXT) pvContext;
	BOOL					fLocalPlayer = TRUE;
	DWORD					dwStartTime = 0, dwEndTime = 0;

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
				DPTEST_TRACE(pContext->hLog, "     pvPlayerContext = 0x%08x", 1, pCreatePlayerMsg->pvPlayerContext);
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

			// Release the player context
			MemFree(pPlayerContext);

		break;
		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT pIndicateConnectMsg;
			PMESSAGEDATA pConnectData, pConnectReplyData;
			CHAR *pszTempURL;
			HRESULT hrTemp;
			DWORD dwTempURLSize, i;
			DWORD dwChecksum;

			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			
			if(pContext->fVerbose)
			{
				DPTEST_TRACE(pContext->hLog, "DPN_MSGID_INDICATE_CONNECT", 0);
				DPTEST_TRACE(pContext->hLog, "     dwSize = %u", 1, pIndicateConnectMsg->dwSize);
				DPTEST_TRACE(pContext->hLog, "     pvUserConnectData = 0x%08x", 1, pIndicateConnectMsg->pvUserConnectData);
				DPTEST_TRACE(pContext->hLog, "     dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			}
			
			// Parse any connect data if present
			if(!(pConnectData = (PMESSAGEDATA) pIndicateConnectMsg->pvUserConnectData))
			{
					DPTEST_TRACE(pContext->hLog, "No incoming connection data was provided.  Reject connection.", 0);
					hr = E_FAIL;
					break;
			}
			else
			{
				
				dwChecksum = 0;
				
				// Make sure we received enough data
				if(pIndicateConnectMsg->dwUserConnectDataSize < sizeof(MESSAGEDATA))
				{
					DPTEST_TRACE(pContext->hLog, "Received connect data was too small!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					hr = E_FAIL;
				}
				// If the message is of valid size
				else
				{
					if(pConnectData->dwMessageType != CONNECT_DATA)
					{
						DPTEST_TRACE(pContext->hLog, "Received connect data of invalid type!  DEBUGBREAK()-ing.", 0);
						DEBUGBREAK();
						hr = E_FAIL;
					}
					
					// Calculate the checksum from the message
					for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
						dwChecksum += pConnectData->szData[i];
					
					// Compare it to the checksum listed in the message
					if(dwChecksum != pConnectData->dwChecksum)
					{
						DPTEST_TRACE(pContext->hLog, "Received connect data failed checksum!  DEBUGBREAK()-ing.", 0);
						DEBUGBREAK();
						hr = E_FAIL;
					}
					
					pContext->fVerbose ? DPTEST_TRACE(pContext->hLog, "Connect data was : \"%s\"", 0, pConnectData->szData) : 0;
					
					pIndicateConnectMsg->dwReplyDataSize = sizeof(MESSAGEDATA);
					pConnectReplyData = (PMESSAGEDATA) MemAlloc(pIndicateConnectMsg->dwReplyDataSize);
				
					// Copy the player buffer
					strncpy(pConnectReplyData->szData, CONNECT_REPLY_MESSAGE, MAX_MESSAGEDATA_DATA);
					pConnectReplyData->szData[MAX_MESSAGEDATA_DATA - 1] = 0;
					pConnectReplyData->dwMessageType = CONNECT_REPLY_DATA;
					pConnectReplyData->dwChecksum = 0;
					
					// Calculate the checksum
					for(i = 0; i < MAX_MESSAGEDATA_DATA; ++i)
						pConnectReplyData->dwChecksum += pConnectReplyData->szData[i];
					
					pIndicateConnectMsg->pvReplyData = pConnectReplyData;
					
				}
			}

			// BUGBUG - set some reply data and verify that it is returned later with the provided context
//			    PVOID     pvReplyContext;
			
			if(pContext->fVerbose)
			{
				
				// BUGBUG - try setting player context value indirectly through this value
				DPTEST_TRACE(pContext->hLog, "     pvPlayerContext = 0x%08x", 1, pIndicateConnectMsg->pvPlayerContext);
				
				//
				// Get the DPlay URL of the incoming connection address and display it
				//
				
				// Find out how big the URL is
				dwTempURLSize = 0;
				hrTemp = pIndicateConnectMsg->pAddressPlayer->GetURLA(NULL, &dwTempURLSize);
				if(hrTemp != DPNERR_BUFFERTOOSMALL)
				{
					DPTEST_TRACE(pContext->hLog, "Couldn't query size of incoming connection address!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					hr = E_FAIL;
				}
				
				// Allocate a buffer for it
				pszTempURL = (CHAR *) MemAlloc(dwTempURLSize);
				if(!pszTempURL)
				{
					DPTEST_TRACE(pContext->hLog, "Couldn't create URL buffer!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					hr = E_FAIL;
				}
				
				// Retrieve the URL
				hrTemp = pIndicateConnectMsg->pAddressPlayer->GetURLA(pszTempURL, &dwTempURLSize);
				if(hrTemp != DPN_OK)
				{
					DPTEST_TRACE(pContext->hLog, "Couldn't retrieve incoming connection address!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					hr = E_FAIL;
				}
				
				// Log the URL
				DPTEST_TRACE(pContext->hLog, "     pAddressPlayer = %s", 1, pszTempURL);
				
				// Release the URL
				MemFree(pszTempURL);
				
				//
				// Get the DPlay URL of the incoming connection address and display it
				//
				
				// Find out how big the URL is
				dwTempURLSize = 0;
				hrTemp = pIndicateConnectMsg->pAddressDevice->GetURLA(NULL, &dwTempURLSize);
				if(hrTemp != DPNERR_BUFFERTOOSMALL)
				{
					DPTEST_TRACE(pContext->hLog, "Couldn't query size of incoming connection address!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					hr = E_FAIL;
				}
				
				// Allocate a buffer for it
				pszTempURL = (CHAR *) MemAlloc(dwTempURLSize);
				if(!pszTempURL)
				{
					DPTEST_TRACE(pContext->hLog, "Couldn't create URL buffer!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					hr = E_FAIL;
				}
				
				// Retrieve the URL
				hrTemp = pIndicateConnectMsg->pAddressDevice->GetURLA(pszTempURL, &dwTempURLSize);
				if(hrTemp != DPN_OK)
				{
					DPTEST_TRACE(pContext->hLog, "Couldn't retrieve incoming connection address!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					hr = E_FAIL;
				}
				
				// Log the URL
				DPTEST_TRACE(pContext->hLog, "     pAddressDevice = %s", 1, pszTempURL);
				
				// Release the URL
				MemFree(pszTempURL);
			}
			
			if(!pContext->fCanAcceptConnect)
			{
				DPTEST_TRACE(pContext->hLog, "Unexpected connection indication!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			}

			pContext->fAcceptedConnect = TRUE;
			
			// Signal the main thread that a connection was indicated
			if (! SetEvent(pContext->hIndicateConnect))
			{
				hr = GetLastError();
				DPTEST_TRACE(pContext->hLog, "Couldn't set connection indication event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hIndicateConnect);
				
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
			
			// Signal the main thread that an asynchronous operation completed
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
		case DPN_MSGID_CLIENT_INFO:
			PDPNMSG_CLIENT_INFO pClientInfoMsg;

			pClientInfoMsg = (PDPNMSG_CLIENT_INFO) pvMsg;
			
			if(pContext->fVerbose)
			{
				DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CLIENT_INFO", 0);
				DPTEST_TRACE(pContext->hLog, "     dwSize = %u", 1, pClientInfoMsg->dwSize);
				DPTEST_TRACE(pContext->hLog, "     dpnidClient = %u/%x", 2, pClientInfoMsg->dpnidClient, pClientInfoMsg->dpnidClient);
				// BUGBUG - verify that the player context matches the player context that was set when
				// this player connected
				DPTEST_TRACE(pContext->hLog, "     pvPlayerContext = 0x%08x", 1, pClientInfoMsg->pvPlayerContext);
			}
			
			if(!pContext->fClientInfoCanChange)
			{
				DPTEST_TRACE(pContext->hLog, "Unexpected client info change!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			}

			pContext->fClientInfoChanged = TRUE;
			
			// Signal the main thread that the client info changed
			if (! SetEvent(pContext->hClientInfoChanged))
			{
				hr = GetLastError();
				DPTEST_TRACE(pContext->hLog, "Couldn't set client info change indication event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hClientInfoChanged);
				
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
} // SimpleServerMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

} // namespace DPlayCoreNamespace
