//==================================================================================
// Includes
//==================================================================================
#include "dptest.h"
#include "macros.h"
#include "parmvalidation.h"

using namespace DPlayCoreNamespace;

namespace DPlayCoreNamespace {

//==================================================================================
// Defines
//==================================================================================
#define THROTTLECHECK_CHECKPOINTPERIOD		1000
#define THROTTLECHECK_NUMHITSTHRESHOLD		25

#define QUEUEEMPTY_CHECKPOINTPERIOD			4000


#define SENDFILLPATTERN						0x0711F00D
#define PENDINGSENDS_CS_DESTROYPLAYER_ID	(~SENDFILLPATTERN)





//==================================================================================
// Structures
//==================================================================================
typedef struct tagSENDSSIMPLECONTEXT : public BASESESSIONCREATECONTEXT
{
	HANDLE				hLog;  // handle to event logging subsystem
	BOOL				fCancelling; // whether we're cancelling sends or not
	bool*				pabSendsCompleted; // array for completions of each message
	DWORD				dwNumSendsNotCompleted; // number of sends not yet completed
	DWORD				dwReceives; // number of packets received
	HANDLE				hAllSendsCompletedEvent; // event to set when all sends have completed
} SENDSSIMPLECONTEXT, * PSENDSSIMPLECONTEXT;

typedef struct tagSENDSPENDINGCONTEXT : public BASESESSIONCREATECONTEXT
{
	HANDLE				hLog; // handle to event logging subsystem
	DPNID*				padpnidTesters; // array of player IDs, in tester number order
	BOOL				fClosingPlayer; // whether we're a leaving/dropping player or not
	//BOOL				fSelfIndicate; // TRUE if we expect our player ID to be destroyed, too
	int					iNumClosers; // number of closing players
	volatile BOOL		fPlayersLeftOrDropped; // TRUE if all the expected players have left or dropped actually have
	volatile BOOL		fGotTerminateSession; // TRUE if we've gotten the terminate session
	BOOL*				pafTesterIndicated; // pointer to array to use to remember whether an indication has already arrived for the testers
	CRITICAL_SECTION	csStatsLock; // lock protecting the statistics
	DWORD				dwMsgSize; // expected message size for incoming messages
	DWORD				dwNumRelevantSends; // number of sends to/from leaving/dropping player that have been submitted
	DWORD				dwNumIrrelevantSends; // number of sends not to/from leaving/dropping player that have been submitted
	DWORD				dwNumRelevantSendsCompletedSuccess; // number of sends to/from leaving/dropping player that have completed successfully
	DWORD				dwNumRelevantSendsCompletedFailure; // number of sends to/from leaving/dropping player that have completed with a failure
	DWORD				dwNumIrrelevantSendsCompleted; // number of sends not to/from leaving/dropping player that have completed
} SENDSPENDINGCONTEXT, * PSENDSPENDINGCONTEXT;


//==================================================================================
// Prototypes
//==================================================================================
HRESULT SendsSimpleDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
//HRESULT SendsPendingDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);


#undef DEBUG_SECTION
#define DEBUG_SECTION	"SendsExec_SimplePeer()"
//==================================================================================
// SendsExec_SimplePeer
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.1 - Simple peer-to-peer sends test
//
// Arguments:
//	HANDLE				hLog			Handle to logging subsystem
//	DP_SENDTESTPARAM	*pSendTestParam	Pointer to params for this test
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT SendsExec_SimplePeer(HANDLE hLog, DP_SENDTESTPARAM *pSendTestParam)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	HRESULT						temphr;
	SENDSSIMPLECONTEXT			context;
	DPN_APPLICATION_DESC		dpnad;
	PWRAPDP8PEER				pDP8Peer = NULL;
	DPN_BUFFER_DESC				dpnbd;
	DPNID						*padpnidTesters = NULL;
//	BOOL						fEatingCPU = FALSE;
	DWORD						dwTemp;
	DWORD						dwCurrentTarget;
	DWORD						dwNextCancel;
	DWORD						dwStartTime;
	DWORD						dwMessagesOutstanding = 0;
	DWORD						dwSuccessfulCancels = 0;
	DPNHANDLE					dpnhAsyncHandle;

	ZeroMemory(&context, sizeof (SENDSSIMPLECONTEXT));
	ZeroMemory(&dpnbd, sizeof (DPN_BUFFER_DESC));

	context.hLog = hLog;

	BEGIN_TESTCASE
	{
		// Setup completion items if we're not sending synchronously
		if (! (pSendTestParam->dwFlags & DPNSEND_SYNC))
		{
			if (pSendTestParam->dwCancelInterval != 0)
				context.fCancelling = TRUE;

			context.dwNumSendsNotCompleted = pSendTestParam->dwNumMsgs;

			LOCALALLOC_OR_THROW(bool*, context.pabSendsCompleted,
								(pSendTestParam->dwNumMsgs * sizeof (bool)));

			context.hAllSendsCompletedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (context.hAllSendsCompletedEvent == NULL)
			{
				sr = GetLastError();
				DPTEST_FAIL(hLog,  "Couldn't create all sends completed event!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't create event)
		} // end if (asynchronous sends)




		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = 0;
		//dpnad.guidInstance = GUID_NULL;
#pragma BUGBUG(vanceo, "Real GUID")
		dpnad.guidApplication = GUID_UNKNOWN;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		dpnad.pwszSessionName = L"Session";
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.pvApplicationReservedData = 0;


		if(pSendTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating session host");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			// When function returns we should have a valid peer and an array of tester DPNID's
			if(!CreateHostAndWaitForPlayers(hLog, &pDP8Peer, &context, &dpnad, SendsSimpleDPNMessageHandler,
				&padpnidTesters, pSendTestParam->dwNumMachines, 120000))
			{
				DPTEST_FAIL(hLog, "Couldn't create host and receive correct number of players", 0);
				SETTHROW_TESTRESULT(E_ABORT);
			}
		}
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to session host");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			// When function returns we should have a valid peer and an array of tester DPNID's
			if(!ConnectNonHostAndWaitForPlayers(hLog, &pDP8Peer, &context, pSendTestParam->dwHostAddr, &dpnad, SendsSimpleDPNMessageHandler,
				&padpnidTesters, pSendTestParam->dwNumMachines, 120000))
			{
				DPTEST_FAIL(hLog, "Couldn't create host and receive correct number of players", 0);
				SETTHROW_TESTRESULT(E_ABORT);
			}
		}

		dpnbd.dwBufferSize = pSendTestParam->dwMsgSize;
		LOCALALLOC_OR_THROW(PBYTE, dpnbd.pBufferData, dpnbd.dwBufferSize);

		FillWithDWord(dpnbd.pBufferData, dpnbd.dwBufferSize, SENDFILLPATTERN);

// BUGBUG - may add CPU eating later
//		if (pSendTestParam->dwPercentEatCPU != 0)
//		{
//			sr = StartEatingCPUTime(0, pSendTestParam->dwPercentEatCPU);
//			if (sr != S_OK)
//			{
//				DPTEST_FAIL(hLog, "Couldn't start eating CPU time!", 0);
//				THROW_SYSTEMRESULT;
//			} // end if (failed starting to eat CPU time)
//
//			fEatingCPU = TRUE;
//		} // end if (should eat CPU time)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for everyone to be ready to begin");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!SyncWithOtherTesters(hLog, pSendTestParam->hNetsyncObject, SEND_BEGIN_EVENT, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, SEND_BEGIN_EVENT);
			SETTHROW_TESTRESULT(E_ABORT);
		}
		

		DPTEST_TRACE(hLog,  "Sending %u %u byte messages every %u ms.",
			3, pSendTestParam->dwNumMsgs, pSendTestParam->dwMsgSize, pSendTestParam->dwInterval);

		dwCurrentTarget = 0;

		// Remember when we're next cancelling sends.
		dwNextCancel = pSendTestParam->dwCancelInterval;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Begin sends");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwStartTime = GetTickCount();

		for(dwTemp = 0; dwTemp < pSendTestParam->dwNumMsgs; dwTemp++)
		{
			// Find the next person to send to (this will not actually be used if
			// sending to All Players group).
			dwCurrentTarget++;
			if (dwCurrentTarget >= pSendTestParam->dwNumMachines)
				dwCurrentTarget = 0;

#pragma TODO(vanceo, "Handle NOLOOPBACK yet sending to self")


			if (! (pSendTestParam->dwFlags & DPNSEND_SYNC))
				InterlockedIncrement((LPLONG) (&dwMessagesOutstanding));

			// Use the API directly so we don't have tons of our wrapper spew.
			tr = pDP8Peer->m_pDP8Peer->SendTo(((pSendTestParam->fSendToAllPlayersGroup) ? DPNID_ALL_PLAYERS_GROUP : padpnidTesters[dwCurrentTarget]),
												&dpnbd,
												1,
												pSendTestParam->dwTimeout,
												(PVOID) ((DWORD_PTR) dwTemp),
												&dpnhAsyncHandle,
												pSendTestParam->dwFlags);
			if (pSendTestParam->dwFlags & DPNSEND_SYNC)
			{
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog,  "SendTo failed!", 0);
					THROW_TESTRESULT;
				} // end if (sendto failed)
			} // end if (synchronous send)
			else
			{
				if (tr != (HRESULT) DPNSUCCESS_PENDING)
				{
					DPTEST_FAIL(hLog,  "SendTo didn't return expected PENDING success code!", 0);
					THROW_TESTRESULT;
				} // end if (didn't get pending error)

			
				// If we're supposed to cancel things, and it's time to do so, try.
				if (pSendTestParam->dwCancelInterval != 0)
				{
					dwNextCancel--;
					if (dwNextCancel == 0)
					{
						// Reset the cancel interval counter.
						dwNextCancel = pSendTestParam->dwCancelInterval;

						tr = pDP8Peer->DP8P_CancelAsyncOperation(((pSendTestParam->dwCancelFlags == 0) ? dpnhAsyncHandle : NULL),
																pSendTestParam->dwCancelFlags);
						if (tr != DPN_OK)
						{
							if (pSendTestParam->dwCancelFlags == 0)
							{
								if ((tr != DPNERR_CANNOTCANCEL) && (tr != DPNERR_INVALIDHANDLE))
								{
									DPTEST_FAIL(hLog,  "Cancelling send %x failed!",
										1, dpnhAsyncHandle);
									THROW_TESTRESULT;
								} // end if (not expected cannotcancel or invalidhandler errors)
								

								DPTEST_TRACE(hLog,  "Couldn't cancel async send %x, assuming it is completing/completed.  0x%08x",
									2, dpnhAsyncHandle, (HRESULT) tr);
							} // end if (cancelling individual send)
							else
							{
								if (tr != DPNERR_CANNOTCANCEL)
								{
									DPTEST_FAIL(hLog,  "CancelAsyncOperation (flags %x) failed!",
										1, pSendTestParam->dwCancelFlags);
									THROW_TESTRESULT;
								} // end if (not expected cannotcancel error)
								

								DPTEST_TRACE(hLog,  "Couldn't cancel async operations with flags %x, assuming the sends are completing/completed.  0x%08x",
									2, pSendTestParam->dwCancelFlags, (HRESULT) tr);
							} // end else (cancelling mutiple operations)
						} // end if (cancel failed)
						else
						{
							dwSuccessfulCancels++;

							if (pSendTestParam->dwCancelFlags == 0)
							{
								DPTEST_TRACE(hLog, "Cancelling async send %x succeeded.",
									1, dpnhAsyncHandle);
							} // end if (cancelling individual send)
							else
							{
								DPTEST_TRACE(hLog, "CancelAsyncOperation (flags %x) succeeded.",
									1, pSendTestParam->dwCancelFlags);
							} // end else (cancelling mutiple operations)
						} // end else (cancel succeeded)
					} // end if (time to cancel)
				} // end if (cancelling sends)
			} // end else (asynchronous send)


			// Sleep for the requested interval.  We don't use TNSLEEP because that
			// puts spew out and we sort of want to send as fast as possible.
			Sleep(pSendTestParam->dwInterval);
		} // end for (each send)
		
		
		DPTEST_TRACE(hLog, "Spent %u ms sending %u %u byte messages every %u ms, %u successful cancel attempts.",
			5, (GetTickCount() - dwStartTime), pSendTestParam->dwNumMsgs, pSendTestParam->dwMsgSize,
			pSendTestParam->dwInterval, dwSuccessfulCancels);


		if(! (pSendTestParam->dwFlags & DPNSEND_SYNC))
		{
			dwStartTime = GetTickCount();

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for all asynchronous sends to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			if(WaitForSingleObject(context.hAllSendsCompletedEvent, 120000) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Completion of all sends wasn't indicated within 2 minutes", 0);
				
				THROW_SYSTEMRESULT;
			}

			DPTEST_TRACE(hLog, "Spent %u ms waiting for all sends to complete.",
				1, (GetTickCount() - dwStartTime));

		} // end if (asynchronous sends)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for everyone to be ready to close");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!SyncWithOtherTesters(hLog, pSendTestParam->hNetsyncObject, SEND_END_EVENT, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, SEND_END_EVENT);
			SETTHROW_TESTRESULT(E_ABORT);
		}

		DPTEST_TRACE(hLog, "Received %u messages during test.", 1, context.dwReceives);

// BUGBUG - may add CPU eating later
//		if (fEatingCPU)
//		{
//			sr = StopEatingCPUTime();
//			if (sr != S_OK)
//			{
//				DPTEST_FAIL(hLog,  "Couldn't stop eating CPU time!", 0);
//				THROW_SYSTEMRESULT;
//			} // end if (couldn't stop eating CPU time)
//
//			fEatingCPU = FALSE;
//		} // end if (ate CPU time)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing peer object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Peer->DP8P_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing peer object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Peer->Release();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Releaseing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		DPTEST_TRACE(hLog, "Releasing peer wrapper", 0);
		delete (pDP8Peer);
		pDP8Peer = NULL;

		SAFE_LOCALFREE(padpnidTesters);

		FINAL_SUCCESS;
	}
	END_TESTCASE


//	// If we're still eating CPU time, be sure to stop it.
//	if (fEatingCPU)
//	{
//		temphr = StopEatingCPUTime();
//		if (temphr != S_OK)
//		{
//			DPTEST_FAIL(hLog,  "Couldn't stop eating CPU time!", 0);
//			OVERWRITE_SR_IF_OK(temphr);
//		} // end if (couldn't stop eating CPU time)
//
//		fEatingCPU = FALSE;
//	} // end if (ate CPU time)

	if (pDP8Peer != NULL)
	{
		temphr = pDP8Peer->DP8P_Close(0);
		if (temphr != DPN_OK)
		{
			DPTEST_FAIL(hLog,  "Closing peer interface failed!  0x%08x", 1, temphr);
			OVERWRITE_SR_IF_OK(temphr);
		} // end if (closing peer interface failed)

		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	SAFE_LOCALFREE(padpnidTesters);
	SAFE_CLOSEHANDLE(context.hbsccAllPlayersCreated);
	SAFE_CLOSEHANDLE(context.hAllSendsCompletedEvent);
	SAFE_LOCALFREE(context.pabSendsCompleted);
	SAFE_LOCALFREE(dpnbd.pBufferData);

	return (sr);
} // SendsExec_SimplePeer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"SendsExec_SimpleCS()"
//==================================================================================
// SendsExec_SimpleCS
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.2 - Simple client/server sends test
//
// Arguments:
//	HANDLE				hLog			Handle to logging subsystem
//	DP_SENDTESTPARAM	*pSendTestParam	Pointer to params for this test
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT SendsExec_SimpleCS(HANDLE hLog, DP_SENDTESTPARAM *pSendTestParam)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	HRESULT							temphr;
	SENDSSIMPLECONTEXT				context;
	DPN_APPLICATION_DESC			dpnad;
	PWRAPDP8SERVER					pDP8Server = NULL;
	PWRAPDP8CLIENT					pDP8Client = NULL;
	DPNID*							padpnidTesters = NULL;
	DPN_BUFFER_DESC					dpnbd;
//	BOOL							fEatingCPU = FALSE;
	DWORD							dwTemp;
	DWORD							dwCurrentTarget;
	DWORD							dwNextCancel;
	DWORD							dwStartTime;
	DWORD							dwMessagesOutstanding = 0;
	DWORD							dwSuccessfulCancels = 0;
	DPNHANDLE						dpnhAsyncHandle;


	ZeroMemory(&context, sizeof (SENDSSIMPLECONTEXT));
	ZeroMemory(&dpnbd, sizeof (DPN_BUFFER_DESC));

	context.hLog = hLog;

	BEGIN_TESTCASE
	{

		// Setup completion items if we're not sending synchronously
		if (! (pSendTestParam->dwFlags & DPNSEND_SYNC))
		{
			if (pSendTestParam->dwCancelInterval != 0)
				context.fCancelling = TRUE;

			context.dwNumSendsNotCompleted = pSendTestParam->dwNumMsgs;

			LOCALALLOC_OR_THROW(bool*, context.pabSendsCompleted,
								(pSendTestParam->dwNumMsgs * sizeof (bool)));

			context.hAllSendsCompletedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (context.hAllSendsCompletedEvent == NULL)
			{
				sr = GetLastError();
				DPTEST_FAIL(hLog,  "Couldn't create all sends completed event!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't create event)
		} // end if (asynchronous sends)

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
#pragma BUGBUG(vanceo, "Real GUID")
		dpnad.guidApplication = GUID_UNKNOWN;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		dpnad.pwszSessionName = L"Session";
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.pvApplicationReservedData = 0;


		if(pSendTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating session server");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			// When function returns we should have a valid server and an array of tester DPNID's
			if(!CreateServerAndWaitForPlayers(hLog, &pDP8Server, &context, &dpnad, SendsSimpleDPNMessageHandler,
				&padpnidTesters, pSendTestParam->dwNumMachines, 120000))
			{
				DPTEST_FAIL(hLog, "Couldn't create server and receive correct number of players", 0);
				SETTHROW_TESTRESULT(E_ABORT);
			}
		}
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to session server");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			// When function returns we should have a valid client
			if(!ConnectClient(hLog, &pDP8Client, &context, pSendTestParam->dwHostAddr, &dpnad,
				SendsSimpleDPNMessageHandler, 120000))
			{
				DPTEST_FAIL(hLog, "Couldn't connect client", 0);
				SETTHROW_TESTRESULT(E_ABORT);
			}
		}

		dpnbd.dwBufferSize = pSendTestParam->dwMsgSize;
		LOCALALLOC_OR_THROW(PBYTE, dpnbd.pBufferData, dpnbd.dwBufferSize);

		FillWithDWord(dpnbd.pBufferData, dpnbd.dwBufferSize, SENDFILLPATTERN);

// BUGBUG - may add CPU eating later
//		if (pSendTestParam->dwPercentEatCPU != 0)
//		{
//			sr = StartEatingCPUTime(0, pSendTestParam->dwPercentEatCPU);
//			if (sr != S_OK)
//			{
//				DPTEST_FAIL(hLog,  "Couldn't start eating CPU time!", 0);
//				THROW_SYSTEMRESULT;
//			} // end if (failed starting to eat CPU time)
//
//			fEatingCPU = TRUE;
//		} // end if (should eat CPU time)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for everyone to be ready to begin");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!SyncWithOtherTesters(hLog, pSendTestParam->hNetsyncObject, SEND_BEGIN_EVENT, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, SEND_BEGIN_EVENT);
			SETTHROW_TESTRESULT(E_ABORT);
		}
		
		DPTEST_TRACE(hLog,  "Sending %u %u byte messages every %u ms.",
			3, pSendTestParam->dwNumMsgs, pSendTestParam->dwMsgSize, pSendTestParam->dwInterval);

		dwCurrentTarget = 0;

		// Remember when we're next cancelling sends.
		dwNextCancel = pSendTestParam->dwCancelInterval;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Begin sends");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwStartTime = GetTickCount();

		for(dwTemp = 0; dwTemp < pSendTestParam->dwNumMsgs; dwTemp++)
		{

			// Find the next person to send to (this will not actually be used if
			// not tester 0 or sending to All Players group).
			dwCurrentTarget++;
			if (dwCurrentTarget >= pSendTestParam->dwNumMachines)
				dwCurrentTarget = 0;

#pragma TODO(vanceo, "Handle NOLOOPBACK yet sending to self")


			if (! (pSendTestParam->dwFlags & DPNSEND_SYNC))
				InterlockedIncrement((LPLONG) (&dwMessagesOutstanding));

			// Use the API directly so we don't have tons of our wrapper spew.
			if (pSendTestParam->fHostTest == 0)
			{
				tr = pDP8Server->m_pDP8Server->SendTo(((pSendTestParam->fSendToAllPlayersGroup) ? DPNID_ALL_PLAYERS_GROUP : padpnidTesters[dwCurrentTarget]),
													&dpnbd,
													1,
													pSendTestParam->dwTimeout,
													(PVOID) ((DWORD_PTR) dwTemp),
													&dpnhAsyncHandle,
													pSendTestParam->dwFlags);
			} // end if (tester 0)
			else
			{
				tr = pDP8Client->m_pDP8Client->Send(&dpnbd,
													1,
													pSendTestParam->dwTimeout,
													(PVOID) ((DWORD_PTR) dwTemp),
													&dpnhAsyncHandle,
													pSendTestParam->dwFlags);
			} // end else (not tester 0)

			if (pSendTestParam->dwFlags & DPNSEND_SYNC)
			{
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog,  "SendTo failed!", 0);
					THROW_TESTRESULT;
				} // end if (sendto failed)
			} // end if (synchronous send)
			else
			{
				if (tr != (HRESULT) DPNSUCCESS_PENDING)
				{
					DPTEST_FAIL(hLog,  "SendTo didn't return expected PENDING success code!", 0);
					THROW_TESTRESULT;
				} // end if (didn't get pending error)

			
				// If we're supposed to cancel things, and it's time to do so, try.
				if (pSendTestParam->dwCancelInterval != 0)
				{
					dwNextCancel--;
					if (dwNextCancel == 0)
					{
						// Reset the cancel interval counter.
						dwNextCancel = pSendTestParam->dwCancelInterval;

						if (pSendTestParam->fHostTest)
						{
							tr = pDP8Server->DP8S_CancelAsyncOperation(((pSendTestParam->dwCancelFlags == 0) ? dpnhAsyncHandle : NULL),
																		pSendTestParam->dwCancelFlags);
						} // end if (tester 0)
						else
						{
							tr = pDP8Client->DP8C_CancelAsyncOperation(((pSendTestParam->dwCancelFlags == 0) ? dpnhAsyncHandle : NULL),
																		pSendTestParam->dwCancelFlags);
						} // end else (not tester 0)

						if (tr != DPN_OK)
						{
							if (pSendTestParam->dwCancelFlags == 0)
							{
								if ((tr != DPNERR_CANNOTCANCEL) && (tr != DPNERR_INVALIDHANDLE))
								{
									DPTEST_FAIL(hLog,  "Cancelling send %x failed!",
										1, dpnhAsyncHandle);
									THROW_TESTRESULT;
								} // end if (not expected cannotcancel error)
								

								DPTEST_TRACE(hLog,  "Couldn't cancel async send %x, assuming it is completing/completed.  0x%08x",
									2, dpnhAsyncHandle, (HRESULT) tr);
							} // end if (cancelling individual send)
							else
							{
								if (tr != DPNERR_CANNOTCANCEL)
								{
									DPTEST_FAIL(hLog,  "CancelAsyncOperation (flags %x) failed!",
										1, pSendTestParam->dwCancelFlags);
									THROW_TESTRESULT;
								} // end if (not expected cannotcancel error)
								

								DPTEST_TRACE(hLog,  "Couldn't cancel async operations with flags %x, assuming the sends are completing/completed.  0x%08x",
									2, pSendTestParam->dwCancelFlags, (HRESULT) tr);
							} // end else (cancelling mutiple operations)
						} // end if (cancel failed)
						else
						{
							dwSuccessfulCancels++;

							if (pSendTestParam->dwCancelFlags == 0)
							{
								DPTEST_TRACE(hLog, "Cancelling async send %x succeeded.",
									1, dpnhAsyncHandle);
							} // end if (cancelling individual send)
							else
							{
								DPTEST_TRACE(hLog, "CancelAsyncOperation (flags %x) succeeded.",
									1, pSendTestParam->dwCancelFlags);
							} // end else (cancelling mutiple operations)
						} // end else (cancel succeeded)
					} // end if (time to cancel)
				} // end if (cancelling sends)
			} // end else (asynchronous send)
			
			
			// Sleep for the requested interval.  We don't use TNSLEEP because that
			// puts spew out and we sort of want to send as fast as possible.
			Sleep(pSendTestParam->dwInterval);
		} // end for (each send)
		
		
		DPTEST_TRACE(hLog, "Spent %u ms sending %u %u byte messages every %u ms, %u successful cancel attempts.",
			5, (GetTickCount() - dwStartTime), pSendTestParam->dwNumMsgs, pSendTestParam->dwMsgSize,
			pSendTestParam->dwInterval, dwSuccessfulCancels);
	
		if(! (pSendTestParam->dwFlags & DPNSEND_SYNC))
		{
			dwStartTime = GetTickCount();
			
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for all asynchronous sends to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			if(WaitForSingleObject(context.hAllSendsCompletedEvent, 120000) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Completion of all sends wasn't indicated within 2 minutes", 0);
				
				THROW_SYSTEMRESULT;
			}
			
			DPTEST_TRACE(hLog, "Spent %u ms waiting for all sends to complete.",
				1, (GetTickCount() - dwStartTime));

		} // end if (asynchronous sends)
				
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for everyone to be ready to close");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
		if(!SyncWithOtherTesters(hLog, pSendTestParam->hNetsyncObject, SEND_END_EVENT, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, SEND_BEGIN_EVENT);
			SETTHROW_TESTRESULT(E_ABORT);
		}
		
		DPTEST_TRACE(hLog, "Received %u messages during test.", 1, context.dwReceives);

// BUGBUG - may add CPU eating later
//		if (fEatingCPU)
//		{
//			sr = StopEatingCPUTime();
//			if (sr != S_OK)
//			{
//				DPTEST_FAIL(hLog,  "Couldn't stop eating CPU time!", 0);
//				THROW_SYSTEMRESULT;
//			} // end if (couldn't stop eating CPU time)
//
//			fEatingCPU = FALSE;
//		} // end if (ate CPU time)
		
		if (pSendTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Closing server object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			tr = pDP8Server->DP8S_Close(0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Closing server object failed!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't close)
			
		
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing server object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			tr = pDP8Server->Release();
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Releaseing server object failed!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't close)
			
		
			DPTEST_TRACE(hLog, "Releasing server wrapper", 0);
			delete (pDP8Server);
			pDP8Server = NULL;
		} // end if (tester 0)
		else
		{
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
			TEST_SECTION("Releasing client object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			tr = pDP8Client->Release();
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Releaseing client object failed!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't close)
			
		
			DPTEST_TRACE(hLog, "Releasing client wrapper", 0);
			delete (pDP8Client);
			pDP8Client = NULL;
		} // end else (not tester 0)

		FINAL_SUCCESS;
	}
	END_TESTCASE

// BUGBUG - may add CPU eating later
//	// If we're still eating CPU time, be sure to stop it.
//	if (fEatingCPU)
//	{
//		temphr = StopEatingCPUTime();
//		if (temphr != S_OK)
//		{
//			DPTEST_FAIL(hLog,  "Couldn't stop eating CPU time!", 0);
//			OVERWRITE_SR_IF_OK(temphr);
//		} // end if (couldn't stop eating CPU time)
//
//		fEatingCPU = FALSE;
//	} // end if (ate CPU time)

	if (pDP8Server != NULL)
	{
		temphr = pDP8Server->DP8S_Close(0);
		if (temphr != DPN_OK)
		{
			DPTEST_FAIL(hLog,  "Closing server interface failed!  0x%08x", 1, temphr);
			OVERWRITE_SR_IF_OK(temphr);
		} // end if (closing server interface failed)

		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)

	if (pDP8Client != NULL)
	{
		temphr = pDP8Client->DP8C_Close(0);
		if (temphr != DPN_OK)
		{
			DPTEST_FAIL(hLog,  "Closing client interface failed!  0x%08x", 1, temphr);
			OVERWRITE_SR_IF_OK(temphr);
		} // end if (closing client interface failed)

		delete (pDP8Client);
		pDP8Client = NULL;
	} // end if (have client object)

	SAFE_LOCALFREE(padpnidTesters);
	SAFE_CLOSEHANDLE(context.hbsccAllPlayersCreated);
	SAFE_CLOSEHANDLE(context.hAllSendsCompletedEvent);
	SAFE_LOCALFREE(context.pabSendsCompleted);
	SAFE_LOCALFREE(dpnbd.pBufferData);

	return (sr);
} // SendsExec_SimpleCS
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



/* BUGBUG - No support for software disconnects
#undef DEBUG_SECTION
#define DEBUG_SECTION	"SendsExec_PendingPeer()"
//==================================================================================
// SendsExec_PendingPeer
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.2.1 - Pending peer-to-peer sends while leaving test
//				2.2.2.2 - Pending peer-to-peer sends while dropping test
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT SendsExec_PendingPeer(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	HRESULT						temphr;
	PTID_PSENDS_PENDING			pInput = (PTID_PSENDS_PENDING) pTNecd->pvInputData;
	PTNRESULT					pSubResult;
	PVOID						pvSubInputData = NULL;
	PTOD_BBLDDP8_PEER_CREATE	pBldDP8PeerCreateOutput = NULL;
	DWORD						dwSubOutputDataSize;
	SENDSPENDINGCONTEXT			context;
	DPN_APPLICATION_DESC		dpnad;
	PWRAPDP8PEER				pDP8Peer = NULL;
	PTNFAULTSIM					pFaultSim = NULL;
	DPN_BUFFER_DESC				dpnbd;
	int							dwCurrentTarget;
	int							i;
	DWORD						dwLeaveDropTime = 0;
	BOOL						fThrottle;
	BOOL						fRelevantSend;
	DPNHANDLE					dpnhAsyncHandle;
	DWORD						dwTotalThrottleHits = 0;
	DWORD						dwLastCheckpointTime;
	DWORD						dwValueAtLastCheckpoint;
	DWORD						dwSleepInterval = 0;
	int*						paiDestroyTesters = NULL;
	DPNID*						padpnidDestroyTesters = NULL;



	ZeroMemory(&context, sizeof (SENDSPENDINGCONTEXT));
	context.pTNecd = pTNecd;

	InitializeCriticalSection(&(context.csStatsLock));


	ZeroMemory(&dpnbd, sizeof (DPN_BUFFER_DESC));


	BEGIN_TESTCASE
	{
		REQUIRE_INPUT_DATA(sizeof (TID_PSENDS_PENDING));


#ifdef DEBUG
		// We had better not be send synchronously
		if (pInput->dwFlags & DPNSEND_SYNC)
		{
			DPTEST_FAIL(hLog,  "Can't specify synchronous as a send flag for this test case (flags = %x)!",
				1, pInput->dwFlags);
			THROW_SYSTEMRESULT;
		} // end if (synchronous sends)

		if (pInput->iNumClosers > (pTNecd->dwNumMachines - 2))
		{
			DPTEST_FAIL(hLog,  "Can't specify %i closers when only %i machines are running the test (need host and one client to remain in session)!",
				2, pInput->iNumClosers, pTNecd->dwNumMachines);
			THROW_SYSTEMRESULT;
		} // end if (too many closers)
#endif // DEBUG


		// Determine how many leaving/dropping players there will be.
		context.iNumClosers = pInput->iNumClosers;
		if (pInput->iNumClosers <= 0)
			context.iNumClosers = pTNecd->dwNumMachines / 2;

		DPTEST_TRACE(hLog, "This test will have %i peers leave/drop while sends are pending.",
			1, context.iNumClosers);




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating peer-to-peer session");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		context.pfnDPNMessageHandler = SendsPendingDPNMessageHandler;

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = 0; // no host migration
		//dpnad.guidInstance = GUID_NULL;
#pragma BUGBUG(vanceo, "Real GUID")
		dpnad.guidApplication = GUID_UNKNOWN;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		dpnad.pwszSessionName = L"Session";
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.pvApplicationReservedData = 0;


		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_BBLDDP8_ALL_CREATE));
#pragma TODO(vanceo, "Allow SP to be selected")
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->guidSP = CLSID_DP8SP_TCPIP;
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->pHandlerContext = &context;
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->pdpnad = &dpnad;

		sr = pTNecd->pExecutor->ExecSubTestCase("3.1.1.1",
												pvSubInputData,
												sizeof (TID_BBLDDP8_ALL_CREATE),
												0);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog,  "Couldn't execute sub test case BldSsn:BldPeer:Create!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.1.1",
									"Creating peer-to-peer session failed!");

		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pBldDP8PeerCreateOutput,
									dwSubOutputDataSize,
									(sizeof (TOD_BBLDDP8_PEER_CREATE) + (pTNecd->dwNumMachines * sizeof (DPNID))));

		pDP8Peer = pBldDP8PeerCreateOutput->pDP8Peer;
		pDP8Peer->m_dwRefCount++; // we're using it during this function

		context.padpnidTesters = (DPNID*) (pBldDP8PeerCreateOutput + 1);




		context.dwMsgSize = pInput->dwMsgSize;
		dpnbd.dwBufferSize = pInput->dwMsgSize;
		LOCALALLOC_OR_THROW(PBYTE, dpnbd.pBufferData, dpnbd.dwBufferSize);

		FillWithDWord(dpnbd.pBufferData, dpnbd.dwBufferSize, SENDFILLPATTERN);


		LOCALALLOC_OR_THROW(BOOL*, context.pafTesterIndicated,
							(pTNecd->dwNumMachines * sizeof (BOOL)));


		TESTSECTION_IF((pTNecd->iTesterNum != 0) && (pTNecd->iTesterNum <= context.iNumClosers))
		{
			context.fClosingPlayer = TRUE;

			// Set the fault simulator up now so that we have less to do after the
			// queue is filled.  This gives us a better chance of keeping the queue
			// full while we drop from the session.

			TESTSECTION_IF(pTNecd->pExecutor->IsCase("2.2.2.2"))
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Creating IMTest fault simulator for future use");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->CreateNewFaultSim(&pFaultSim,
														TN_FAULTSIM_IMTEST,
														NULL,
														0);
				if (sr != S_OK)
				{
					DPTEST_FAIL(hLog,  "Couldn't create IMTest fault simulator!", 0);
					THROW_SYSTEMRESULT;
				} // end if (couldn't create fault simulator)
			} // end if (drop test case)
			TESTSECTION_ENDIF
		} // end if (leaving/dropping tester)
		TESTSECTION_ENDIF



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for all testers to be ready to fill queue");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		sr = pTNecd->pExecutor->SyncWithTesters("Ready to send", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;




		DPTEST_TRACE(hLog,  "Trying to fill queue with %u byte messages.",
			1, pInput->dwMsgSize);
		//Ignore error
		pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
								"Trying to fill queue with %u byte messages.",
								1, pInput->dwMsgSize);

		dwCurrentTarget = pTNecd->iTesterNum;



		dwValueAtLastCheckpoint = 0;
		dwLastCheckpointTime = timeGetTime();

		// Figure out when to leave session if we're the leaving/dropping player.
		if (context.fClosingPlayer)
			dwLeaveDropTime = dwLastCheckpointTime + pInput->dwQueueTime;


		TESTSECTION_DO
		{
			// Keep the control layer from thinking we're deadlocked.
			pTNecd->pExecutor->NoteTestIsStillRunning();


			if (pInput->fSendToAllPlayersGroup)
			{
				fRelevantSend = TRUE;
			} // end if (sending to all players)
			else
			{
				// Find the next person to send to.
				dwCurrentTarget++;
				if (dwCurrentTarget >= pTNecd->dwNumMachines)
					dwCurrentTarget = 0;

#pragma TODO(vanceo, "Handle NOLOOPBACK yet sending to self")


				if ((context.fClosingPlayer) ||
					((dwCurrentTarget != 0) && (dwCurrentTarget <= context.iNumClosers)))
				{
					fRelevantSend = TRUE;
				} // end if (leaving player or sending to leaving player)
				else
				{
					fRelevantSend = FALSE;
				} // end else (not leaving player or sending to him)
			} // end else (not sending to all players)


			// Check to see if we hit the throttle limit frequently.  If we did,
			// let's sleep a little between each send. 
			if (timeGetTime() > (dwLastCheckpointTime + THROTTLECHECK_CHECKPOINTPERIOD))
			{
				if ((dwValueAtLastCheckpoint - dwTotalThrottleHits) > THROTTLECHECK_NUMHITSTHRESHOLD)
				{
					dwSleepInterval += 10;
					DPTEST_TRACE(hLog, "Increasing sleep interval to %u.", 1, dwSleepInterval);
				} // end if (hit the throttle limit frequently)

				dwValueAtLastCheckpoint = dwTotalThrottleHits;
				dwLastCheckpointTime =  timeGetTime();
			} // end if (time to check throttle again)

			// Sleep some if necessary.
			if (dwSleepInterval != 0)
			{
				Sleep(dwSleepInterval);
			} // end if (should sleep)


			fThrottle = FALSE;

			// Lock the stats while we verify/update them.
			EnterCriticalSection(&(context.csStatsLock));

			if (fRelevantSend)
			{
				if ((context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure)) > pInput->dwMaxQueueSize)
					fThrottle = TRUE;
				else
					context.dwNumRelevantSends++;
			} // end if (relevant send)
			else
			{
				if ((context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted) > pInput->dwMaxQueueSize)
					fThrottle = TRUE;
				else
					context.dwNumIrrelevantSends++;
			} // end else (irrelevant send)

			// Drop the stats lock.
			LeaveCriticalSection(&(context.csStatsLock));


			
			if (fThrottle)
			{
				dwTotalThrottleHits++;
			} // end if (throttle)
			else
			{
				// Use the API directly so we don't have tons of our wrapper spew.
				tr = pDP8Peer->m_pDP8Peer->SendTo(((pInput->fSendToAllPlayersGroup) ? DPNID_ALL_PLAYERS_GROUP : context.padpnidTesters[dwCurrentTarget]),
													&dpnbd,
													1,
													pInput->dwTimeout,
													(PVOID) ((INT_PTR) fRelevantSend),
													&dpnhAsyncHandle,
													pInput->dwFlags);
				if (tr != (HRESULT) DPNSUCCESS_PENDING)
				{
					// INVALIDPLAYER is allowed because we may still be calling Send
					// after we've returned from the DESTROYPLAYER indication.
					// NOCONNECTION is allowed if we're the leaving player and we
					// called Close(), because that's what it's spec'd to do.

#pragma TODO(vanceo, "Be more specific about allowing the INVALIDPLAYER and NOCONNECTION error codes")

					if ((tr != DPNERR_CONNECTIONLOST) && (tr != DPNERR_INVALIDPLAYER) && (tr != DPNERR_NOCONNECTION))
					{
						DPTEST_FAIL(hLog,  "SendTo didn't return expected PENDING, CONNECTIONLOST, INVALIDPLAYER, or NOCONNECTION!  DEBUGBREAK()-ing.  0x%08x",
							1, (HRESULT) tr);
						DEBUGBREAK();
						THROW_TESTRESULT;
					} // end if (didn't returned expected error)

					if (! fRelevantSend)
					{
						DPTEST_FAIL(hLog,  "SendTo to irrelevant tester %i (player ID %u/%u) failed!",
							3, dwCurrentTarget, context.padpnidTesters[dwCurrentTarget],
							context.padpnidTesters[dwCurrentTarget]);
						THROW_TESTRESULT;
					} // end if (not relevant send)


					//DPTEST_TRACE(hLog, "SendTo %x failed.", 1, dpnhAsyncHandle);


					// Lock the stats while we update them.
					EnterCriticalSection(&(context.csStatsLock));

					context.dwNumRelevantSendsCompletedFailure++;

					if ((int) (context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure)) < 0)
					{
						DPTEST_FAIL(hLog,  "Immediate relevant failure is a duplicate ((%u - (%u + %u)) < 0)!  DEBUGBREAK()-ing.",
							3, context.dwNumRelevantSends,
							context.dwNumRelevantSendsCompletedSuccess,
							context.dwNumRelevantSendsCompletedFailure);
						// Ignore error
						pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
												"Immediate relevant failure is a duplicate ((%u - (%u + %u)) < 0)!  DEBUGBREAK()-ing.",
												3, context.dwNumRelevantSends,
												context.dwNumRelevantSendsCompletedSuccess,
												context.dwNumRelevantSendsCompletedFailure);
						DEBUGBREAK();
					} // end if (got too many completions)

					// Drop the stats lock.
					LeaveCriticalSection(&(context.csStatsLock));
				} // end if (didn't get pending)
			} // end else (not throttled)

			
			// If we're the dropping player, we haven't done so yet, and it's time,
			// do so.
			TESTSECTION_IF((dwLeaveDropTime != 0) && ((int) (dwLeaveDropTime - timeGetTime()) < 0))
			{
#ifdef DEBUG
				char	szTemp[32];


				// Lock the stats while we work with them.
				EnterCriticalSection(&(context.csStatsLock));

				wsprintf(szTemp, "%i messages out of %u sent",
						(int) (context.dwNumRelevantSends - context.dwNumRelevantSendsCompletedSuccess),
						context.dwNumRelevantSends);

				if (context.dwNumIrrelevantSends != 0)
				{
					DPTEST_FAIL(hLog,  "Somehow %u irrelevant sends were submitted!  DEBUGBREAK()-ing.",
						1, context.dwNumIrrelevantSends);
					DEBUGBREAK();
				} // end if (any irrelevant sends submitted)

				if (context.dwNumIrrelevantSendsCompleted != 0)
				{
					DPTEST_FAIL(hLog,  "Somehow %u irrelevant sends were completed!  DEBUGBREAK()-ing.",
						1, context.dwNumIrrelevantSendsCompleted);
					DEBUGBREAK();
				} // end if (any irrelevant sends submitted)

				if (context.dwNumRelevantSendsCompletedFailure != 0)
				{
					DPTEST_FAIL(hLog,  "Somehow %u sends failed!  DEBUGBREAK()-ing.",
						1, context.dwNumRelevantSendsCompletedFailure);
					DEBUGBREAK();
				} // end if (any failed sends already)

				// Drop the stats lock.
				LeaveCriticalSection(&(context.csStatsLock));


				DPTEST_TRACE(hLog, "Approximately %s are still outstanding after %u ms of queueing.",
					2, szTemp, pInput->dwQueueTime);
#endif // DEBUG


				TESTSECTION_IF(pTNecd->pExecutor->IsCase("2.2.2.2"))
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Disconnecting sends and receives, will drop from session");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->FaultSimDisconnect(pFaultSim, TRUE);
					if (sr != S_OK)
					{
						DPTEST_FAIL(hLog,  "Couldn't disconnect sends!", 0);
						THROW_SYSTEMRESULT;
					} // end if (couldn't disconnect)

					sr = pTNecd->pExecutor->FaultSimDisconnect(pFaultSim, FALSE);
					if (sr != S_OK)
					{
						DPTEST_FAIL(hLog,  "Couldn't disconnect receives!", 0);
						THROW_SYSTEMRESULT;
					} // end if (couldn't disconnect)


					// Prevent us from trying to leave/drop again, because we're
					// going to keep looping (and queueing sends).
					dwLeaveDropTime = 0;
				} // end if (drop test case)
				TESTSECTION_ELSE
				{
					// Leave the session right now.

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Closing DPlay");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					tr = pDP8Peer->DP8P_Close(0);
					if (tr != DPN_OK)
					{
						DPTEST_FAIL(hLog,  "Failed closing DPlay8Peer object!", 0);
						THROW_TESTRESULT;
					} // end if (failed closing)


					// Make sure the nametable unwound correctly.
					for(i = 0; i < pTNecd->dwNumMachines; i++)
					{
						if (! context.pafTesterIndicated[i])
						{
							DPTEST_FAIL(hLog,  "Didn't get DESTROY_PLAYER indication for tester %i!",
								1, i);
							SETTHROW_TESTRESULT(ERROR_NO_DATA);
						} // end if (tester not indicated)
					} // end for (each tester)

					// Stop looping.
					TESTSECTION_BREAK;
				} // end else (not drop test case)
				TESTSECTION_ENDIF
			} // end if (time to close)
			TESTSECTION_ELSEIF(context.fPlayersLeftOrDropped)
			{
				// The above else-if condition is for when:
				//
				// a) we're the leaving player in the drop case and we have started
				//    the disconnection.  We need to keep trying to send until we
				//    get booted from the session.
				// b) we're not a player that leaves/drops from the session.  We
				//    need to keep trying to send until we see that player go away.
				//
				// Either way, context.fPlayerLeftOrDropped will be set to TRUE when
				// the condition is met.  Once that happens, we can bail out of the
				// loop to do the final cleanup stuff.
				TESTSECTION_BREAK;
			} // end else (dropping player after disconnecting or not leaving)
			TESTSECTION_ENDIF
		} // end do (while time hasn't been exceeded yet)
		TESTSECTION_DOWHILE(TRUE);
		


		DPTEST_TRACE(hLog, "Hit the queue throttle limit of %u a total of %u times.",
			2, pInput->dwMaxQueueSize, dwTotalThrottleHits);



		// Note that we're not taking the lock to check the stats since
		// a) we're only reading the values
		// b) the values should stop changing shortly
		// c) we're polling the values quite regularly.
		dwLastCheckpointTime = timeGetTime();
		dwValueAtLastCheckpoint = context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure);


		// Wait for all outstanding sends to be completed.
		while ((context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure)) != 0)
		{
			// See if it's time to monitor the completion rate again.
			if ((int) (timeGetTime() - (dwLastCheckpointTime + QUEUEEMPTY_CHECKPOINTPERIOD)) > 0)
			{
				// Make sure the completions are still coming in.  If we ever hit
				// the steady state of no-completions, something's bad.
				if ((dwValueAtLastCheckpoint - (context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure))) == 0)
				{
					DPTEST_FAIL(hLog,  "No relevant completions came in over the last %u ms, approximately %i remain!",
						2, QUEUEEMPTY_CHECKPOINTPERIOD,
						(int) (context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure)));
					SETTHROW_TESTRESULT(ERROR_MORE_DATA);
				} // end if (no completions came in)
				
				dwLastCheckpointTime = timeGetTime();
				dwValueAtLastCheckpoint = context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure);
			} // end if (time for next checkpoint)


			DPTEST_TRACE(hLog, "Waiting for approximately %i relevant sends to complete.",
				1, (int) (context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure)));
			

			// Use TNSLEEP to keep the control layer alert so it doesn't think
			// we're deadlocked.
			TNSLEEP(500);
		} // end if (sends still outstanding)


		// Print the stats for reference.  No lock necessary since
		// the count should no longer be changing.
		DPTEST_TRACE(hLog,  "%u relevant sends were submitted, %u succeeded and %u failed.",
			3, context.dwNumRelevantSends,
			context.dwNumRelevantSendsCompletedSuccess,
			context.dwNumRelevantSendsCompletedFailure);


		TESTSECTION_IF(context.fClosingPlayer)
		{
			// If this is a drop case, we still should close DPlay.
			TESTSECTION_IF(pTNecd->pExecutor->IsCase("2.2.2.2"))
			{
				// Make sure we got the TERMINATE_SESSION indication.
				if (! context.fGotTerminateSession)
				{
					DPTEST_FAIL(hLog,  "Didn't get terminate session indication!", 0);
					SETTHROW_TESTRESULT(ERROR_NO_DATA);
				} // end if (didn't get terminate session)


				// Make sure the nametable unwound correctly (how else would we
				// get here?).
				for(i = 0; i < pTNecd->dwNumMachines; i++)
				{
					if (! context.pafTesterIndicated[i])
					{
						DPTEST_FAIL(hLog,  "Didn't get DESTROY_PLAYER indication for tester %i!",
							1, i);
						SETTHROW_TESTRESULT(ERROR_NO_DATA);
					} // end if (tester not indicated)
				} // end for (each tester)



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Restoring send and receive traffic");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->FaultSimReconnect(pFaultSim, TRUE);
				if (sr != S_OK)
				{
					DPTEST_FAIL(hLog,  "Couldn't reconnect sends!", 0);
					THROW_SYSTEMRESULT;
				} // end if (couldn't reconnect)

				sr = pTNecd->pExecutor->FaultSimReconnect(pFaultSim, FALSE);
				if (sr != S_OK)
				{
					DPTEST_FAIL(hLog,  "Couldn't reconnect receives!", 0);
					THROW_SYSTEMRESULT;
				} // end if (couldn't reconnect)



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Releasing IMTest fault simulator");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->ReleaseFaultSim(&pFaultSim);
				if (sr != S_OK)
				{
					DPTEST_FAIL(hLog,  "Couldn't release fault simulator %x!", 1, pFaultSim);
					THROW_SYSTEMRESULT;
				} // end if (couldn't release fault simulator)



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Closing DPlay");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Peer->DP8P_Close(0);
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog,  "Failed closing DPlay8Peer object!", 0);
					THROW_TESTRESULT;
				} // end if (failed closing)
			} // end if (drop case)
			TESTSECTION_ENDIF




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for other testers to finish");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			sr = pTNecd->pExecutor->SyncWithTesters("Ready to destroy session",
													NULL, 0, NULL, 0);
			HANDLE_SYNC_RESULT;




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Peer wrapper");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Peer->Release();
			if (tr != S_OK)
			{
				DPTEST_FAIL(hLog,  "Couldn't release DirectPlay8Peer object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't release object)


			sr = g_pDP8PeersList->RemoveFirstReference(pDP8Peer);
			if (sr != S_OK)
			{
				DPTEST_FAIL(hLog,  "Couldn't remove DirectPlay8Peer wrapper object from list!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't remove object from list)


			pDP8Peer->m_dwRefCount--; // release our reference
			if (pDP8Peer->m_dwRefCount == 0)
			{
				DPTEST_TRACE(hLog, "Deleting DirectPlay8Peer wrapper object %x.",
					1, pDP8Peer);
				delete (pDP8Peer);
			} // end if (can delete the object)
			else
			{
				DPTEST_TRACE(hLog, "Can't delete DirectPlay8Peer wrapper object %x, it's refcount is %u.",
					2, pDP8Peer,
					pDP8Peer->m_dwRefCount);
			} // end else (can't delete the object)
			pDP8Peer = NULL;
		} // end if (closing player)
		TESTSECTION_ELSE
		{
			// Note that we're not taking the lock to check the stats since
			// a) we're only reading the values
			// b) the values should stop changing shortly
			// c) we're polling the values quite regularly.
			dwLastCheckpointTime = timeGetTime();
			dwValueAtLastCheckpoint = context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted;


			// Wait for all outstanding sends to be completed.
			while ((context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted) != 0)
			{
				// See if it's time to monitor the completion rate again.
				if ((int) (timeGetTime() - (dwLastCheckpointTime + QUEUEEMPTY_CHECKPOINTPERIOD)) > 0)
				{
					// Make sure the completions are still coming in.  If we ever
					// hit the steady state of no-completions, something's bad.
					if ((dwValueAtLastCheckpoint - (context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted)) == 0)
					{
						DPTEST_FAIL(hLog,  "No irrelevant completions came in over the last %u ms, approximately %i remain!",
							2, QUEUEEMPTY_CHECKPOINTPERIOD,
							(int) (context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted));
						SETTHROW_TESTRESULT(ERROR_MORE_DATA);
					} // end if (no completions came in)
					
					dwLastCheckpointTime = timeGetTime();
					dwValueAtLastCheckpoint = context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted;
				} // end if (time for next checkpoint)


				DPTEST_TRACE(hLog, "Waiting for approximately %i irrelevant sends to complete.",
					1, (int) (context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted));
				

				// Use TNSLEEP to keep the control layer alert so it doesn't think
				// we're deadlocked.
				TNSLEEP(500);
			} // end if (sends still outstanding)




			LOCALALLOC_OR_THROW(int*, paiDestroyTesters,
								((pTNecd->dwNumMachines - context.iNumClosers) * sizeof (int)));

			LOCALALLOC_OR_THROW(DPNID*, padpnidDestroyTesters,
								((pTNecd->dwNumMachines - context.iNumClosers) * sizeof (DPNID)));

			// Store hosting tester.
			//paiDestroyTesters[0] = 0;
			padpnidDestroyTesters[0] = context.padpnidTesters[0];

			// Store non-host testers still in session.
			for(i = 1; i < (pTNecd->dwNumMachines - context.iNumClosers); i++)
			{
				paiDestroyTesters[i] = context.iNumClosers + i;
				padpnidDestroyTesters[i] = context.padpnidTesters[context.iNumClosers + i];
			} // end for (each non-host tester still in session)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for other testers to finish");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			sr = pTNecd->pExecutor->SyncWithTesters("Ready to destroy session",
													NULL, 0, NULL, 0);
			HANDLE_SYNC_RESULT;



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Destroying peer-to-peer session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_BBLDDP8_PEER_DESTROY));
			((PTID_BBLDDP8_PEER_DESTROY) pvSubInputData)->pDP8Peer = pDP8Peer;
			((PTID_BBLDDP8_PEER_DESTROY) pvSubInputData)->pHandlerContext = &context;
			((PTID_BBLDDP8_PEER_DESTROY) pvSubInputData)->padpnidTesters = padpnidDestroyTesters;

			sr = pTNecd->pExecutor->ExecSubTestCaseArray("3.1.1.2", pvSubInputData,
														sizeof (TID_BBLDDP8_PEER_DESTROY),
														(pTNecd->dwNumMachines - context.iNumClosers),
														paiDestroyTesters);

			LocalFree(pvSubInputData);
			pvSubInputData = NULL;

			LocalFree(padpnidDestroyTesters);
			padpnidDestroyTesters = NULL;

			LocalFree(paiDestroyTesters);
			paiDestroyTesters = NULL;

			if (sr != S_OK)
			{
				DPTEST_FAIL(hLog,  "Couldn't execute sub test case BldSsn:BldPeer:Destroy!", 0);
				THROW_SYSTEMRESULT;
			} // end if (failed executing sub test case)

			GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.1.2",
										"Destroying peer-to-peer session failed!");



			pDP8Peer->m_dwRefCount--;
			if (pDP8Peer->m_dwRefCount == 0)
			{
				DPTEST_TRACE(hLog, "Deleting peer wrapper object %x.", 1, pDP8Peer);
				delete (pDP8Peer);
			} // end if (can delete the object)
			else
			{
				DPTEST_TRACE(hLog,  "WARNING: Can't delete peer wrapper object %x, it's refcount is %u!?",
					2, pDP8Peer, pDP8Peer->m_dwRefCount);
			} // end else (can't delete the object)
			pDP8Peer = NULL;
		} // end else (not closing player)
		TESTSECTION_ENDIF



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pFaultSim != NULL)
	{
		temphr = pTNecd->pExecutor->ReleaseFaultSim(&pFaultSim);
		if (temphr != S_OK)
		{
			DPTEST_FAIL(hLog,  "Couldn't release fault sim %x!  0x%08x", 2, pFaultSim, temphr);
			
			OVERWRITE_SR_IF_OK(temphr);

			pFaultSim = NULL;
		} // end if (couldn't remove handler)
	} // end if (have fault sim)

	if (pDP8Peer != NULL)
	{
		temphr = pDP8Peer->DP8P_Close(0);
		if (temphr != DPN_OK)
		{
			DPTEST_FAIL(hLog,  "Closing peer interface failed!  0x%08x", 1, temphr);
			OVERWRITE_SR_IF_OK(temphr);
		} // end if (closing peer interface failed)


		// Ignore error, it may not actually be on the list.
		g_pDP8PeersList->RemoveFirstReference(pDP8Peer);


		pDP8Peer->m_dwRefCount--;
		if (pDP8Peer->m_dwRefCount == 0)
		{
			DPTEST_TRACE(hLog, "Deleting DirectPlay8Peer wrapper object %x.", 1, pDP8Peer);
			delete (pDP8Peer);
		} // end if (can delete the object)
		else
		{
			DPTEST_TRACE(hLog,  "WARNING: Can't delete DirectPlay8Peer wrapper object %x, it's refcount is %u!?",
				2, pDP8Peer, pDP8Peer->m_dwRefCount);
		} // end else (can't delete the object)
		pDP8Peer = NULL;
	} // end if (have peer object)

	SAFE_LOCALFREE(padpnidDestroyTesters);
	SAFE_LOCALFREE(paiDestroyTesters);
	SAFE_LOCALFREE(context.pafTesterIndicated);
	SAFE_LOCALFREE(dpnbd.pBufferData);
	SAFE_LOCALFREE(pvSubInputData);

	DeleteCriticalSection(&(context.csStatsLock));

	return (sr);
} // SendsExec_PendingPeer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/




/* BUGBUG - No support for software disconnects
#undef DEBUG_SECTION
#define DEBUG_SECTION	"SendsExec_PendingCS()"
//==================================================================================
// SendsExec_PendingCS
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.2.3 - Pending client/server sends while leaving test
//				2.2.2.4 - Pending client/server sends while dropping test
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT SendsExec_PendingCS(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	HRESULT							temphr;
	PTID_PSENDS_PENDING				pInput = (PTID_PSENDS_PENDING) pTNecd->pvInputData;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PTOD_BBLDDP8_CS_CREATE_0		pBldDP8CSCreateOutput0 = NULL;
	PTOD_BBLDDP8_CS_CREATE_NOT0		pBldDP8CSCreateOutputNot0 = NULL;
	DWORD							dwSubInputDataSize;
	DWORD							dwSubOutputDataSize;
	SENDSPENDINGCONTEXT				context;
	DPN_APPLICATION_DESC			dpnad;
	PWRAPDP8SERVER					pDP8Server = NULL;
	PWRAPDP8CLIENT					pDP8Client = NULL;
	PTNFAULTSIM						pFaultSim = NULL;
	DPN_BUFFER_DESC					dpnbd;
	int								dwCurrentTarget;
	int								i;
	DWORD							dwLeaveDropTime = 0;
	BOOL							fThrottle;
	BOOL							fRelevantSend;
	DPNHANDLE						dpnhAsyncHandle;
	DWORD							dwTotalThrottleHits = 0;
	DWORD							dwLastCheckpointTime;
	DWORD							dwValueAtLastCheckpoint;
	DWORD							dwSleepInterval = 0;
	int*							paiDestroyTesters = NULL;
	DPNID*							padpnidDestroyTesters = NULL;



	ZeroMemory(&context, sizeof (SENDSPENDINGCONTEXT));
	context.pTNecd = pTNecd;

	InitializeCriticalSection(&(context.csStatsLock));


	ZeroMemory(&dpnbd, sizeof (DPN_BUFFER_DESC));


	BEGIN_TESTCASE
	{
		REQUIRE_INPUT_DATA(sizeof (TID_PSENDS_PENDING));


#ifdef DEBUG
		// We had better not be send synchronously
		if (pInput->dwFlags & DPNSEND_SYNC)
		{
			DPTEST_FAIL(hLog,  "Can't specify synchronous as a send flag for this test case (flags = %x)!",
				1, pInput->dwFlags);
			THROW_SYSTEMRESULT;
		} // end if (synchronous sends)

		if (pInput->iNumClosers > (pTNecd->dwNumMachines - 2))
		{
			DPTEST_FAIL(hLog,  "Can't specify %i closers when only %i machines are running the test (need host and one client to remain in session)!",
				2, pInput->iNumClosers, pTNecd->dwNumMachines);
			THROW_SYSTEMRESULT;
		} // end if (too many closers)
#endif // DEBUG


		// Determine how many leaving/dropping players there will be.
		context.iNumClosers = pInput->iNumClosers;
		if (pInput->iNumClosers <= 0)
			context.iNumClosers = pTNecd->dwNumMachines / 2;

		DPTEST_TRACE(hLog, "This test will have %i peers leave/drop while sends are pending.",
			1, context.iNumClosers);




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating client/server session");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		//if (pTNecd->iTesterNum == 0)
			context.pfnDPNMessageHandler = SendsPendingDPNMessageHandler;
		//else
		//	context.pfnDPNMessageHandler = SendsPendingDPNMessageHandler;

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
#pragma BUGBUG(vanceo, "Real GUID")
		dpnad.guidApplication = GUID_UNKNOWN;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		dpnad.pwszSessionName = L"Session";
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.pvApplicationReservedData = 0;


		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_BBLDDP8_ALL_CREATE));
#pragma TODO(vanceo, "Allow SP to be selected")
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->guidSP = CLSID_DP8SP_TCPIP;
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->pHandlerContext = &context;
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->pdpnad = &dpnad;

		sr = pTNecd->pExecutor->ExecSubTestCase("3.1.2.1",
												pvSubInputData,
												sizeof (TID_BBLDDP8_ALL_CREATE),
												0);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog,  "Couldn't execute sub test case BldSsn:BldCS:Create!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.2.1",
									"Creating client/server session failed!");

		// Otherwise get the object created.
		if (pTNecd->iTesterNum == 0)
		{
			CHECKANDGET_SUBOUTPUTDATA(pSubResult,
										pBldDP8CSCreateOutput0,
										dwSubOutputDataSize,
										(sizeof (TOD_BBLDDP8_CS_CREATE_0) + (pTNecd->dwNumMachines * sizeof (DPNID))));

			pDP8Server = pBldDP8CSCreateOutput0->pDP8Server;
			pDP8Server->m_dwRefCount++; // we're using it during this function
			context.padpnidTesters = (DPNID*) (pBldDP8CSCreateOutput0 + 1);
		} // end if (tester 0)
		else
		{
			CHECKANDGET_SUBOUTPUTDATA(pSubResult,
										pBldDP8CSCreateOutputNot0,
										dwSubOutputDataSize,
										(sizeof (TOD_BBLDDP8_CS_CREATE_NOT0) + (pTNecd->dwNumMachines * sizeof (DPNID))));

			pDP8Client = pBldDP8CSCreateOutputNot0->pDP8Client;
			pDP8Client->m_dwRefCount++; // we're using it during this function
			context.padpnidTesters = (DPNID*) (pBldDP8CSCreateOutputNot0 + 1);
		} // end else (not tester 0)



		context.dwMsgSize = pInput->dwMsgSize;
		dpnbd.dwBufferSize = pInput->dwMsgSize;
		LOCALALLOC_OR_THROW(PBYTE, dpnbd.pBufferData, dpnbd.dwBufferSize);

		FillWithDWord(dpnbd.pBufferData, dpnbd.dwBufferSize, SENDFILLPATTERN);


		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			LOCALALLOC_OR_THROW(BOOL*, context.pafTesterIndicated,
								(pTNecd->dwNumMachines * sizeof (BOOL)));
		} // end if (tester 0)
		TESTSECTION_ELSE
		{
			TESTSECTION_IF(pTNecd->iTesterNum <= context.iNumClosers)
			{
				context.fClosingPlayer = TRUE;

				// Set junk up now so that we have less to do after the queue is filled.
				// This gives us a better chance of keeping the queue full while we drop
				// from the session.

				TESTSECTION_IF(pTNecd->pExecutor->IsCase("2.2.2.2"))
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Creating IMTest fault simulator for future use");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->CreateNewFaultSim(&pFaultSim,
															TN_FAULTSIM_IMTEST,
															NULL,
															0);
					if (sr != S_OK)
					{
						DPTEST_FAIL(hLog,  "Couldn't create IMTest fault simulator!", 0);
						THROW_SYSTEMRESULT;
					} // end if (couldn't create fault simulator)
				} // end if (drop test case)
				TESTSECTION_ENDIF
			} // end if (leaving/dropping tester)
			TESTSECTION_ENDIF
		} // end else (not tester 0)
		TESTSECTION_ENDIF



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for all testers to be ready to fill queue");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		sr = pTNecd->pExecutor->SyncWithTesters("Ready to send", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;




		DPTEST_TRACE(hLog,  "Trying to fill queue with %u byte messages.",
			1, pInput->dwMsgSize);
		//Ignore error
		pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
								"Trying to fill queue with %u byte messages.",
								1, pInput->dwMsgSize);

		dwCurrentTarget = pTNecd->iTesterNum;



		dwValueAtLastCheckpoint = 0;
		dwLastCheckpointTime = timeGetTime();

		// Figure out when to leave session if we're a leaving/dropping player.
		if (context.fClosingPlayer)
			dwLeaveDropTime = dwLastCheckpointTime + pInput->dwQueueTime;


		TESTSECTION_DO
		{
			// Keep the control layer from thinking we're deadlocked.
			pTNecd->pExecutor->NoteTestIsStillRunning();


			if (pTNecd->iTesterNum == 0)
			{
				if (pInput->fSendToAllPlayersGroup)
				{
					fRelevantSend = TRUE;
				} // end if (sending to all players)
				else
				{
					// Find the next person to send to.
					dwCurrentTarget++;
					if (dwCurrentTarget >= pTNecd->dwNumMachines)
					{
						dwCurrentTarget = 0;

#pragma TODO(vanceo, "Handle NOLOOPBACK yet sending to self")
					} // end if (sending to self)
					else
					{
						if (dwCurrentTarget <= context.iNumClosers)
						{
							fRelevantSend = TRUE;
						} // end if (leaving player or sending to leaving player)
						else
						{
							fRelevantSend = FALSE;
						} // end else (not leaving player or sending to him)
					} // end else (not sending to self)
				} // end else (not sending to all players)
			} // end if (tester 0)
			else
			{
				dwCurrentTarget = 0; // we always send to the server

				if (context.fClosingPlayer)
				{
					fRelevantSend = TRUE;
				} // end if (leaving player)
				else
				{
					fRelevantSend = FALSE;
				} // end else (not leaving player)
			} // end else (not tester 0)


			// Check to see if we hit the throttle limit frequently.  If we did,
			// let's sleep a little between each send. 
			if (timeGetTime() > (dwLastCheckpointTime + THROTTLECHECK_CHECKPOINTPERIOD))
			{
				if ((dwValueAtLastCheckpoint - dwTotalThrottleHits) > THROTTLECHECK_NUMHITSTHRESHOLD)
				{
					dwSleepInterval += 10;
					DPTEST_TRACE(hLog, "Increasing sleep interval to %u.", 1, dwSleepInterval);
				} // end if (hit the throttle limit frequently)

				dwValueAtLastCheckpoint = dwTotalThrottleHits;
				dwLastCheckpointTime =  timeGetTime();
			} // end if (time to check throttle again)

			// Sleep some if necessary.
			if (dwSleepInterval != 0)
			{
				Sleep(dwSleepInterval);
			} // end if (should sleep)


			fThrottle = FALSE;

			// Lock the stats while we verify/update them.
			EnterCriticalSection(&(context.csStatsLock));

			if (fRelevantSend)
			{
				if ((context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure)) > pInput->dwMaxQueueSize)
					fThrottle = TRUE;
				else
					context.dwNumRelevantSends++;
			} // end if (relevant send)
			else
			{
				if ((context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted) > pInput->dwMaxQueueSize)
					fThrottle = TRUE;
				else
					context.dwNumIrrelevantSends++;
			} // end else (irrelevant send)

			// Drop the stats lock.
			LeaveCriticalSection(&(context.csStatsLock));


			
			if (fThrottle)
			{
				dwTotalThrottleHits++;
			} // end if (throttle)
			else
			{
				// Use the API directly so we don't have tons of our wrapper spew.
				if (pTNecd->iTesterNum == 0)
				{
					tr = pDP8Server->m_pDP8Server->SendTo(((pInput->fSendToAllPlayersGroup) ? DPNID_ALL_PLAYERS_GROUP : context.padpnidTesters[dwCurrentTarget]),
														&dpnbd,
														1,
														pInput->dwTimeout,
														(PVOID) ((INT_PTR) fRelevantSend),
														&dpnhAsyncHandle,
														pInput->dwFlags);
				} // end if (tester 0)
				else
				{
					tr = pDP8Client->m_pDP8Client->Send(&dpnbd,
														1,
														pInput->dwTimeout,
														(PVOID) ((INT_PTR) fRelevantSend),
														&dpnhAsyncHandle,
														pInput->dwFlags);
				} // end else (not tester 0)

				if (tr != (HRESULT) DPNSUCCESS_PENDING)
				{
					// INVALIDPLAYER is allowed because we may still be calling Send
					// after we've returned from the DESTROYPLAYER indication.
					// NOCONNECTION is allowed if we're the leaving player and we
					// called Close(), because that's what it's spec'd to do.

#pragma TODO(vanceo, "Be more specific about allowing the INVALIDPLAYER and NOCONNECTION error codes")

					if ((tr != DPNERR_CONNECTIONLOST) && (tr != DPNERR_INVALIDPLAYER) && (tr != DPNERR_NOCONNECTION))
					{
						DPTEST_FAIL(hLog,  "Send(To) didn't return expected PENDING, CONNECTIONLOST, INVALIDPLAYER, or NOCONNECTION!  DEBUGBREAK()-ing.  0x%08x",
							1, (HRESULT) tr);
						DEBUGBREAK();
						THROW_TESTRESULT;
					} // end if (didn't returned expected error)

					if (! fRelevantSend)
					{
						DPTEST_FAIL(hLog,  "SendTo to irrelevant tester %i (player ID %u/%u) failed!",
							3, dwCurrentTarget, context.padpnidTesters[dwCurrentTarget],
							context.padpnidTesters[dwCurrentTarget]);
						THROW_TESTRESULT;
					} // end if (not relevant send)


					//DPTEST_TRACE(hLog, "Send(To) %x failed.", 1, dpnhAsyncHandle);


					// Lock the stats while we update them.
					EnterCriticalSection(&(context.csStatsLock));

					context.dwNumRelevantSendsCompletedFailure++;

					if ((int) (context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure)) < 0)
					{
						DPTEST_FAIL(hLog,  "Immediate relevant failure is a duplicate ((%u - (%u + %u)) < 0)!  DEBUGBREAK()-ing.",
							3, context.dwNumRelevantSends,
							context.dwNumRelevantSendsCompletedSuccess,
							context.dwNumRelevantSendsCompletedFailure);
						// Ignore error
						pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
												"Immediate relevant failure is a duplicate ((%u - (%u + %u)) < 0)!  DEBUGBREAK()-ing.",
												3, context.dwNumRelevantSends,
												context.dwNumRelevantSendsCompletedSuccess,
												context.dwNumRelevantSendsCompletedFailure);
						DEBUGBREAK();
					} // end if (got too many completions)

					// Drop the stats lock.
					LeaveCriticalSection(&(context.csStatsLock));
				} // end if (didn't get pending)
			} // end else (not throttled)

			
			// If we're the dropping player, we haven't done so yet, and it's time,
			// do so.
			TESTSECTION_IF((dwLeaveDropTime != 0) && ((int) (dwLeaveDropTime - timeGetTime()) < 0))
			{
#ifdef DEBUG
				char	szTemp[32];


				// Lock the stats while we work with them.
				EnterCriticalSection(&(context.csStatsLock));

				wsprintf(szTemp, "%i messages out of %u sent",
						(int) (context.dwNumRelevantSends - context.dwNumRelevantSendsCompletedSuccess),
						context.dwNumRelevantSends);

				if (context.dwNumIrrelevantSends != 0)
				{
					DPTEST_FAIL(hLog,  "Somehow %u irrelevant sends were submitted!  DEBUGBREAK()-ing.",
						1, context.dwNumIrrelevantSends);
					DEBUGBREAK();
				} // end if (any irrelevant sends submitted)

				if (context.dwNumIrrelevantSendsCompleted != 0)
				{
					DPTEST_FAIL(hLog,  "Somehow %u irrelevant sends were completed!  DEBUGBREAK()-ing.",
						1, context.dwNumIrrelevantSendsCompleted);
					DEBUGBREAK();
				} // end if (any irrelevant sends submitted)

				if (context.dwNumRelevantSendsCompletedFailure != 0)
				{
					DPTEST_FAIL(hLog,  "Somehow %u sends failed!  DEBUGBREAK()-ing.",
						1, context.dwNumRelevantSendsCompletedFailure);
					DEBUGBREAK();
				} // end if (any failed sends already)

				// Drop the stats lock.
				LeaveCriticalSection(&(context.csStatsLock));


				DPTEST_TRACE(hLog, "Approximately %s are still outstanding after %u ms of queueing.",
					2, szTemp, pInput->dwQueueTime);
#endif // DEBUG


				TESTSECTION_IF(pTNecd->pExecutor->IsCase("2.2.2.2"))
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Disconnecting sends and receives, will drop from session");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->FaultSimDisconnect(pFaultSim, TRUE);
					if (sr != S_OK)
					{
						DPTEST_FAIL(hLog,  "Couldn't disconnect sends!", 0);
						THROW_SYSTEMRESULT;
					} // end if (couldn't disconnect)

					sr = pTNecd->pExecutor->FaultSimDisconnect(pFaultSim, FALSE);
					if (sr != S_OK)
					{
						DPTEST_FAIL(hLog,  "Couldn't disconnect receives!", 0);
						THROW_SYSTEMRESULT;
					} // end if (couldn't disconnect)


					// Prevent us from trying to leave/drop again, because we're
					// going to keep looping (and queueing sends).
					dwLeaveDropTime = 0;
				} // end if (drop test case)
				TESTSECTION_ELSE
				{
					// Leave the session right now.

					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Closing DPlay");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					tr = pDP8Client->DP8C_Close(0);
					if (tr != DPN_OK)
					{
						DPTEST_FAIL(hLog,  "Failed closing DPlay8Client object!", 0);
						THROW_TESTRESULT;
					} // end if (failed closing)


					// Stop looping.
					TESTSECTION_BREAK;
				} // end else (not drop test case)
				TESTSECTION_ENDIF
			} // end if (time to close)
			TESTSECTION_ELSEIF(context.fPlayersLeftOrDropped)
			{
				// The above else-if condition is for when:
				//
				// a) we're the leaving player in the drop case and we have started
				//    the disconnection.  We need to keep trying to send until we
				//    get booted from the session.
				// b) we're not a player that leaves/drops from the session.  We
				//    need to keep trying to send until we see that player go away.
				//    Unfortunately if we're another client, the only way to tell
				//    that the dropping client is gone is to have the server send us
				//    a specially constructed message via DPlay that the other
				//    client has now gone away... ugly.
				//
				// Either way, context.fPlayerLeftOrDropped will be set to TRUE when
				// the condition is met.  Once that happens, we can bail out of the
				// loop to do the final cleanup stuff.
				//
				// A server needs to send that special message to everyone.
				if (pTNecd->iTesterNum == 0)
				{
					DPN_BUFFER_DESC		dpnbdTemp;
					DWORD				dwMsg;


					dwMsg = PENDINGSENDS_CS_DESTROYPLAYER_ID; 

					dpnbdTemp.dwBufferSize = sizeof (DWORD);
					dpnbdTemp.pBufferData = (PBYTE) (&dwMsg);


					// Lock the stats while we update them.
					EnterCriticalSection(&(context.csStatsLock));

					context.dwNumIrrelevantSends++;

					// Drop the stats lock.
					LeaveCriticalSection(&(context.csStatsLock));


					tr = pDP8Server->DP8S_SendTo(DPNID_ALL_PLAYERS_GROUP,
												&dpnbdTemp,
												1,
												0,
												FALSE,
												&dpnhAsyncHandle,
												DPNSEND_GUARANTEED);
					if (tr != (HRESULT) DPNSUCCESS_PENDING)
					{
						DPTEST_FAIL(hLog,  "SendTo didn't return expected PENDING success code!  DEBUGBREAK()-ing.  0x%08x",
							1, (HRESULT) tr);
						DEBUGBREAK();
						THROW_TESTRESULT;
					} // end if (not pending)
				} // end if (tester 0)
				TESTSECTION_BREAK;
			} // end else (dropping player after disconnecting or not leaving)
			TESTSECTION_ENDIF
		} // end do (while time hasn't been exceeded yet)
		TESTSECTION_DOWHILE(TRUE);
		


		DPTEST_TRACE(hLog, "Hit the queue throttle limit of %u a total of %u times.",
			2, pInput->dwMaxQueueSize, dwTotalThrottleHits);



		// Note that we're not taking the lock to check the stats since
		// a) we're only reading the values
		// b) the values should stop changing shortly
		// c) we're polling the values quite regularly.
		dwLastCheckpointTime = timeGetTime();
		dwValueAtLastCheckpoint = context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure);


		// Wait for all outstanding sends to be completed.
		while ((context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure)) != 0)
		{
			// See if it's time to monitor the completion rate again.
			if ((int) (timeGetTime() - (dwLastCheckpointTime + QUEUEEMPTY_CHECKPOINTPERIOD)) > 0)
			{
				// Make sure the completions are still coming in.  If we ever hit
				// the steady state of no-completions, something's bad.
				if ((dwValueAtLastCheckpoint - (context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure))) == 0)
				{
					DPTEST_FAIL(hLog,  "No relevant completions came in over the last %u ms, approximately %i remain!",
						2, QUEUEEMPTY_CHECKPOINTPERIOD,
						(int) (context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure)));
					SETTHROW_TESTRESULT(ERROR_MORE_DATA);
				} // end if (no completions came in)
				
				dwLastCheckpointTime = timeGetTime();
				dwValueAtLastCheckpoint = context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure);
			} // end if (time for next checkpoint)


			DPTEST_TRACE(hLog, "Waiting for approximately %i relevant sends to complete.",
				1, (int) (context.dwNumRelevantSends - (context.dwNumRelevantSendsCompletedSuccess + context.dwNumRelevantSendsCompletedFailure)));
			

			// Use TNSLEEP to keep the control layer alert so it doesn't think
			// we're deadlocked.
			TNSLEEP(500);
		} // end if (sends still outstanding)


		// Print the stats for reference.  No lock necessary since
		// the count should no longer be changing.
		DPTEST_TRACE(hLog,  "%u relevant sends were submitted, %u succeeded and %u failed.",
			3, context.dwNumRelevantSends,
			context.dwNumRelevantSendsCompletedSuccess,
			context.dwNumRelevantSendsCompletedFailure);


		TESTSECTION_IF(context.fClosingPlayer)
		{
			// If this is a drop case, we still need to close DPlay.
			TESTSECTION_IF(pTNecd->pExecutor->IsCase("2.2.2.2"))
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Restoring send and receive traffic");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->FaultSimReconnect(pFaultSim, TRUE);
				if (sr != S_OK)
				{
					DPTEST_FAIL(hLog,  "Couldn't reconnect sends!", 0);
					THROW_SYSTEMRESULT;
				} // end if (couldn't reconnect)

				sr = pTNecd->pExecutor->FaultSimReconnect(pFaultSim, FALSE);
				if (sr != S_OK)
				{
					DPTEST_FAIL(hLog,  "Couldn't reconnect receives!", 0);
					THROW_SYSTEMRESULT;
				} // end if (couldn't reconnect)



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Releasing IMTest fault simulator");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->ReleaseFaultSim(&pFaultSim);
				if (sr != S_OK)
				{
					DPTEST_FAIL(hLog,  "Couldn't release fault simulator %x!", 1, pFaultSim);
					THROW_SYSTEMRESULT;
				} // end if (couldn't release fault simulator)


				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Closing DPlay");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Client->DP8C_Close(0);
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog,  "Failed closing DPlay8Client object!", 0);
					THROW_TESTRESULT;
				} // end if (failed closing)
			} // end if (drop case)
			TESTSECTION_ENDIF




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for other testers to finish");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			sr = pTNecd->pExecutor->SyncWithTesters("Ready to destroy session",
													NULL, 0, NULL, 0);
			HANDLE_SYNC_RESULT;




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Client object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Client->Release();
			if (tr != S_OK)
			{
				DPTEST_FAIL(hLog,  "Couldn't release DirectPlay8Client object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't release object)


			sr = g_pDP8ClientsList->RemoveFirstReference(pDP8Client);
			if (sr != S_OK)
			{
				DPTEST_FAIL(hLog,  "Couldn't remove DirectPlay8Client wrapper object from list!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't remove object from list)


			pDP8Client->m_dwRefCount--; // release our reference
			if (pDP8Client->m_dwRefCount == 0)
			{
				DPTEST_TRACE(hLog, "Deleting DirectPlay8Client wrapper object %x.",
					1, pDP8Client);
				delete (pDP8Client);
			} // end if (can delete the object)
			else
			{
				DPTEST_TRACE(hLog, "Can't delete DirectPlay8Client wrapper object %x, it's refcount is %u.",
					2, pDP8Client,
					pDP8Client->m_dwRefCount);
			} // end else (can't delete the object)
			pDP8Client = NULL;
		} // end if (tester 1)
		TESTSECTION_ELSE
		{
			// Note that we're not taking the lock to check the stats since
			// a) we're only reading the values
			// b) the values should stop changing shortly
			// c) we're polling the values quite regularly.
			dwLastCheckpointTime = timeGetTime();
			dwValueAtLastCheckpoint = context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted;


			// Wait for all outstanding sends to be completed.
			while ((context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted) != 0)
			{
				// See if it's time to monitor the completion rate again.
				if ((int) (timeGetTime() - (dwLastCheckpointTime + QUEUEEMPTY_CHECKPOINTPERIOD)) > 0)
				{
					// Make sure the completions are still coming in.  If we ever
					// hit the steady state of no-completions, something's bad.
					if ((dwValueAtLastCheckpoint - (context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted)) == 0)
					{
						DPTEST_FAIL(hLog,  "No irrelevant completions came in over the last %u ms, approximately %i remain!",
							2, QUEUEEMPTY_CHECKPOINTPERIOD,
							(int) (context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted));
						SETTHROW_TESTRESULT(ERROR_MORE_DATA);
					} // end if (no completions came in)
					
					dwLastCheckpointTime = timeGetTime();
					dwValueAtLastCheckpoint = context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted;
				} // end if (time for next checkpoint)


				DPTEST_TRACE(hLog, "Waiting for approximately %i irrelevant sends to complete.",
					1, (int) (context.dwNumIrrelevantSends - context.dwNumIrrelevantSendsCompleted));
				

				// Use TNSLEEP to keep the control layer alert so it doesn't think
				// we're deadlocked.
				TNSLEEP(500);
			} // end if (sends still outstanding)




			LOCALALLOC_OR_THROW(int*, paiDestroyTesters,
								((pTNecd->dwNumMachines - context.iNumClosers) * sizeof (int)));

			// Only tester 0 will actually use this array.
			LOCALALLOC_OR_THROW(DPNID*, padpnidDestroyTesters,
								((pTNecd->dwNumMachines - context.iNumClosers) * sizeof (DPNID)));

			// Store hosting tester.
			//paiDestroyTesters[0] = 0;
			padpnidDestroyTesters[0] = context.padpnidTesters[0];

			// Store non-host testers still in session.
			for(i = 1; i < (pTNecd->dwNumMachines - context.iNumClosers); i++)
			{
				paiDestroyTesters[i] = context.iNumClosers + i;
				padpnidDestroyTesters[i] = context.padpnidTesters[context.iNumClosers + i];
			} // end for (each non-host tester still in session)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for other testers to finish");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			sr = pTNecd->pExecutor->SyncWithTesters("Ready to destroy session",
													NULL, 0, NULL, 0);
			HANDLE_SYNC_RESULT;



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Destroying client/server session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if (pTNecd->iTesterNum == 0)
			{
				dwSubInputDataSize = sizeof (TID_BBLDDP8_CS_DESTROY_0);
				LOCALALLOC_OR_THROW(PVOID, pvSubInputData, dwSubInputDataSize);
				((PTID_BBLDDP8_CS_DESTROY_0) pvSubInputData)->pDP8Server = pDP8Server;
				((PTID_BBLDDP8_CS_DESTROY_0) pvSubInputData)->pHandlerContext = &context;
				((PTID_BBLDDP8_CS_DESTROY_0) pvSubInputData)->padpnidTesters = padpnidDestroyTesters;
			} // end if (tester 0)
			else
			{
				dwSubInputDataSize = sizeof (TID_BBLDDP8_CS_DESTROY_NOT0);
				LOCALALLOC_OR_THROW(PVOID, pvSubInputData, dwSubInputDataSize);
				((PTID_BBLDDP8_CS_DESTROY_NOT0) pvSubInputData)->pDP8Client = pDP8Client;
			} // end else (not tester 0)

			sr = pTNecd->pExecutor->ExecSubTestCaseArray("3.1.2.2",
														pvSubInputData,
														dwSubInputDataSize,
														(pTNecd->dwNumMachines - context.iNumClosers),
														paiDestroyTesters);

			LocalFree(pvSubInputData);
			pvSubInputData = NULL;

			SAFE_LOCALFREE(padpnidDestroyTesters);

			LocalFree(paiDestroyTesters);
			paiDestroyTesters = NULL;

			if (sr != S_OK)
			{
				DPTEST_FAIL(hLog,  "Couldn't execute sub test case BldSsn:BldCS:Destroy!", 0);
				THROW_SYSTEMRESULT;
			} // end if (failed executing sub test case)

			GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.2.2",
										"Destroying client/server session failed!");



			if (pTNecd->iTesterNum == 0)
			{
				pDP8Server->m_dwRefCount--;
				if (pDP8Server->m_dwRefCount == 0)
				{
					DPTEST_TRACE(hLog, "Deleting server wrapper object %x.", 1, pDP8Server);
					delete (pDP8Server);
				} // end if (can delete the object)
				else
				{
					DPTEST_TRACE(hLog,  "WARNING: Can't delete server wrapper object %x, it's refcount is %u!?",
						2, pDP8Server, pDP8Server->m_dwRefCount);
				} // end else (can't delete the object)
				pDP8Server = NULL;
			} // end if (tester 0)
			else
			{
				pDP8Client->m_dwRefCount--;
				if (pDP8Client->m_dwRefCount == 0)
				{
					DPTEST_TRACE(hLog, "Deleting client wrapper object %x.", 1, pDP8Client);
					delete (pDP8Client);
				} // end if (can delete the object)
				else
				{
					DPTEST_TRACE(hLog,  "WARNING: Can't delete client wrapper object %x, it's refcount is %u!?",
						2, pDP8Client, pDP8Client->m_dwRefCount);
				} // end else (can't delete the object)
				pDP8Client = NULL;
			} // end else (not tester 0)
		} // end else (not tester 1)
		TESTSECTION_ENDIF



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pFaultSim != NULL)
	{
		temphr = pTNecd->pExecutor->ReleaseFaultSim(&pFaultSim);
		if (temphr != S_OK)
		{
			DPTEST_FAIL(hLog,  "Couldn't release fault sim %x!  0x%08x", 2, pFaultSim, temphr);
			
			OVERWRITE_SR_IF_OK(temphr);

			pFaultSim = NULL;
		} // end if (couldn't remove handler)
	} // end if (have fault sim)

	if (pDP8Server != NULL)
	{
		temphr = pDP8Server->DP8S_Close(0);
		if (temphr != DPN_OK)
		{
			DPTEST_FAIL(hLog,  "Closing DirectPlay8Server interface failed!  0x%08x", 1, temphr);
			OVERWRITE_SR_IF_OK(temphr);
		} // end if (closing server interface failed)


		// Ignore error, it may not actually be on the list.
		g_pDP8ServersList->RemoveFirstReference(pDP8Server);


		pDP8Server->m_dwRefCount--;
		if (pDP8Server->m_dwRefCount == 0)
		{
			DPTEST_TRACE(hLog, "Deleting DirectPlay8Server wrapper object %x.", 1, pDP8Server);
			delete (pDP8Server);
		} // end if (can delete the object)
		else
		{
			DPTEST_TRACE(hLog,  "WARNING: Can't delete DirectPlay8Server wrapper object %x, it's refcount is %u!?",
				2, pDP8Server, pDP8Server->m_dwRefCount);
		} // end else (can't delete the object)
		pDP8Server = NULL;
	} // end if (have server object)

	if (pDP8Client != NULL)
	{
		temphr = pDP8Client->DP8C_Close(0);
		if (temphr != DPN_OK)
		{
			DPTEST_FAIL(hLog,  "Closing DirectPlay8Client interface failed!  0x%08x", 1, temphr);
			OVERWRITE_SR_IF_OK(temphr);
		} // end if (closing client interface failed)


		// Ignore error, it may not actually be on the list.
		g_pDP8ClientsList->RemoveFirstReference(pDP8Client);


		pDP8Client->m_dwRefCount--;
		if (pDP8Client->m_dwRefCount == 0)
		{
			DPTEST_TRACE(hLog, "Deleting DirectPlay8Client wrapper object %x.", 1, pDP8Client);
			delete (pDP8Client);
		} // end if (can delete the object)
		else
		{
			DPTEST_TRACE(hLog,  "WARNING: Can't delete DirectPlay8Client wrapper object %x, it's refcount is %u!?",
				2, pDP8Client, pDP8Client->m_dwRefCount);
		} // end else (can't delete the object)
		pDP8Client = NULL;
	} // end if (have client object)

	SAFE_LOCALFREE(padpnidDestroyTesters);
	SAFE_LOCALFREE(paiDestroyTesters);
	SAFE_LOCALFREE(context.pafTesterIndicated);
	SAFE_LOCALFREE(dpnbd.pBufferData);
	SAFE_LOCALFREE(pvSubInputData);

	DeleteCriticalSection(&(context.csStatsLock));

	return (sr);
} // SendsExec_PendingCS
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/




#undef DEBUG_SECTION
#define DEBUG_SECTION	"SendsSimpleDPNMessageHandler()"
//==================================================================================
// SendsSimpleDPNMessageHandler
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
HRESULT SendsSimpleDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT					hr = DPN_OK;
	PSENDSSIMPLECONTEXT		pContext = (PSENDSSIMPLECONTEXT) pvContext;
	DWORD					dwMsgNum, dwTemp;


	switch (dwMsgType)
	{
		case DPN_MSGID_RECEIVE:
			dwTemp = InterlockedIncrement((LPLONG) (&pContext->dwReceives));

#pragma TODO(vanceo, "Validate and count receives")
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			// Safely increment the number of player create messages we've received
			dwTemp = InterlockedIncrement((LPLONG) (&pContext->dwPlayersCreated));

			// If all the expected players have connected, then signal the main thread
			if(dwTemp == pContext->dwPlayersExpected)
			{
				if(!SetEvent(pContext->hbsccAllPlayersCreated))
				{
					DPTEST_TRACE(pContext->hLog,  "SetEvent failed!  DEBUGBREAK()-ing.  0x%08x", 0);
					DEBUGBREAK();

					hr = E_FAIL;
					goto DONE;
				}
				DPTEST_TRACE(pContext->hLog,  "All %u expected players created", 1, dwTemp);
			}
			break;

		case DPN_MSGID_DESTROY_PLAYER:
			// Indicate that some players have left
			pContext->fPlayersHaveLeft = TRUE;
			break;

		case DPN_MSGID_ENUM_HOSTS_QUERY:
			DPTEST_TRACE(pContext->hLog,  "DPN_MSGID_ENUM_HOSTS_QUERY", 0);

#pragma TODO(vanceo, "Verify that we're tester 0, and then tell DNet to not respond")
		  break;

		case DPN_MSGID_SEND_COMPLETE:
			PDPNMSG_SEND_COMPLETE	pSendCompleteMsg;


			pSendCompleteMsg = (PDPNMSG_SEND_COMPLETE) pvMsg;

			dwMsgNum = (DWORD) ((DWORD_PTR) (pSendCompleteMsg->pvUserContext));


			if (pSendCompleteMsg->hResultCode != DPN_OK)
			{
				if ((! (pContext->fCancelling)) ||
					(pSendCompleteMsg->hResultCode != DPNERR_USERCANCEL))
				{
					DPTEST_TRACE(pContext->hLog,  "Send %u failed!  DEBUGBREAK()-ing.  0x%08x",
						2, dwMsgNum, pSendCompleteMsg->hResultCode);
					DEBUGBREAK();

					hr = E_FAIL;
					goto DONE;
				} // end if (not cancelling)

				// This is just a cancelled send, we're okay.
				DPTEST_TRACE(pContext->hLog, "Send %u was cancelled.  0x%08x",
					2, dwMsgNum, pSendCompleteMsg->hResultCode);
			} // end if (send failed)


			if (pContext->pabSendsCompleted[dwMsgNum])
			{
				DPTEST_TRACE(pContext->hLog,  "Getting duplicate send completion for send number %u!  DEBUGBREAK()-ing.",
					1, dwMsgNum);

				DEBUGBREAK();

				hr = E_FAIL;
				goto DONE;
			} // end if (send already completed)

			dwMsgNum = InterlockedDecrement((LPLONG) (&pContext->dwNumSendsNotCompleted));
			if (dwMsgNum == 0)
			{
				DPTEST_TRACE(pContext->hLog,  "All sends have completed notifying main thread.", 0);
				
				if (! SetEvent(pContext->hAllSendsCompletedEvent))
				{
					hr = GetLastError();

					DPTEST_TRACE(pContext->hLog,  "Couldn't set all sends completed event %x!  0x%08x",
						2, pContext->hAllSendsCompletedEvent, hr);
					
					if (hr == S_OK)
						hr = E_FAIL;

					//goto DONE;
				} // end if (couldn't set event)

				goto DONE;
			} // end if (last send)


			// Every 5 messages or so print out this update.
			if ((dwMsgNum % 25) == 0)
			{
				DPTEST_TRACE(pContext->hLog, "Still %u messages remaining to be completed, not notifying main thread.",
					1, dwMsgNum);
				goto DONE;
			} // end if (not last message)
		  break;

		case DPN_MSGID_TERMINATE_SESSION:
			PDPNMSG_TERMINATE_SESSION	pTerminateSessionMsg;


			pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION) pvMsg;

			DPTEST_TRACE(pContext->hLog,  "WARNING: Ignoring unexpected TERMINATE_SESSION, this had better be because of a test failure!  0x%08x",
				1, pTerminateSessionMsg->hResultCode);
		  break;

		default:
//			DPTEST_TRACE(pContext->hLog,  "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);
			DPTEST_TRACE(pContext->hLog,  "Unexpected message type %x!  Do we need to handle this?", 1, dwMsgType);
//			DEBUGBREAK();

//			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // SendsSimpleDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




/* BUGBUG - no support for software disconnects
#undef DEBUG_SECTION
#define DEBUG_SECTION	"SendsPendingDPNMessageHandler()"
//==================================================================================
// SendsPendingDPNMessageHandler
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
HRESULT SendsPendingDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT					hr = DPN_OK;
	PSENDSPENDINGCONTEXT	pContext = (PSENDSPENDINGCONTEXT) pvContext;
	BOOL					fFound = FALSE;
	int						iNeedDestroyPlayer = -1;
	int						i;
	int						iLastTesterToCheck;


	switch (dwMsgType)
	{
		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER		pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			// Make sure it's an expected destroy player message.  Loop through
			// everyone if we're the closing player, or all the closing players if
			// we're remaining in the session.
			if (pContext->fClosingPlayer)
				iLastTesterToCheck = pContext->pTNecd->dwNumMachines - 1;
			else
				iLastTesterToCheck = pContext->iNumClosers;

			for(i = ((pContext->fClosingPlayer) ? 0 : 1); i <= iLastTesterToCheck; i++)
			{
				if (pDestroyPlayerMsg->dpnidPlayer == pContext->padpnidTesters[i])
				{
					if (pContext->pafTesterIndicated[i])
					{
						DPTEST_TRACE(pContext->hLog,  "Got duplicate DESTROY_PLAYER for tester %i (player ID %u/%x)!  DEBUGBREAK()-ing.",
							3, i, pDestroyPlayerMsg->dpnidPlayer,
							pDestroyPlayerMsg->dpnidPlayer);

						//Ignore error
						pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
														"Got duplicate DESTROY_PLAYER for tester %i (player ID %u/%x)!  DEBUGBREAK()-ing.",
														3, i, pDestroyPlayerMsg->dpnidPlayer,
														pDestroyPlayerMsg->dpnidPlayer);

						DEBUGBREAK();

						hr = E_FAIL;
						goto DONE;
					} // end if (got duplicate indication)

					// Mark the player as gone now.
					pContext->pafTesterIndicated[i] = TRUE;
					fFound = TRUE;

					DPTEST_TRACE(pContext->hLog,  "Got DESTROY_PLAYER for expected ID %u/%x (tester %i), approximately %i relevant messages out of %u sent are still outstanding.",
						5, pDestroyPlayerMsg->dpnidPlayer,
						pDestroyPlayerMsg->dpnidPlayer,
						i,
						(int) (pContext->dwNumRelevantSends - (pContext->dwNumRelevantSendsCompletedSuccess + pContext->dwNumRelevantSendsCompletedFailure)),
						pContext->dwNumRelevantSends);
				} // end if (found destroy player target)
				else
				{
					// Keep track of all the indications so far so we can tell
					// if that's the last one necessary.
					if (! pContext->pafTesterIndicated[i])
						iNeedDestroyPlayer = i;
				} // end else (didn't find destroy player target)
			} // end for (each closing player)

			if (! fFound)
			{
				DPTEST_TRACE(pContext->hLog,  "WARNING: Ignoring unexpected DESTROY_PLAYER for ID %u/%x (expecting %u/%x), this had better be because of a test failure!",
					4, pDestroyPlayerMsg->dpnidPlayer,
					pDestroyPlayerMsg->dpnidPlayer,
					pContext->padpnidTesters[1],
					pContext->padpnidTesters[1]);
				goto DONE;
			} // end if (destroy player message wrong)

			if (iNeedDestroyPlayer > 0)
			{
				DPTEST_TRACE(pContext->hLog, "Still expecting tester %i (player ID %u/%x) to be destroyed, not notifying main thread.",
					2, iNeedDestroyPlayer, pDestroyPlayerMsg->dpnidPlayer,
					pDestroyPlayerMsg->dpnidPlayer);
				goto DONE;
			} // end if (got duplicate)


			DPTEST_TRACE(pContext->hLog,  "Got all expected DESTROY_PLAYERs, approximately %i relevant messages out of %u sent are still outstanding.",
				2, (int) (pContext->dwNumRelevantSends - (pContext->dwNumRelevantSendsCompletedSuccess + pContext->dwNumRelevantSendsCompletedFailure)),
				pContext->dwNumRelevantSends);

			pContext->fPlayersLeftOrDropped = TRUE;
		  break;

		case DPN_MSGID_RECEIVE:
			PDPNMSG_RECEIVE		pReceiveMsg;


			pReceiveMsg = (PDPNMSG_RECEIVE) pvMsg;

			//pReceiveMsg->dwSize;
			//pReceiveMsg->dpnidSender;
			//pReceiveMsg->pvPlayerContext;
			//pReceiveMsg->hBufferHandle;

			pReceiveMsg->pReceiveData;

			// Make sure the size is right.
			if (pReceiveMsg->dwReceiveDataSize != pContext->dwMsgSize)
			{
				// Check to see if it's the special client/server destroy player
				// alert message.
				if ((pReceiveMsg->dwReceiveDataSize != sizeof (DWORD)) ||
					(*((DWORD*) pReceiveMsg->pReceiveData) != PENDINGSENDS_CS_DESTROYPLAYER_ID))
				{
					DPTEST_FAIL(pContext->hLog,  "Received incorrectly sized message from ID %u/%x (ptr = %x, size = %u, expected size = %u)!  DEBUGBREAK()-ing.",
						5, pReceiveMsg->dpnidSender, pReceiveMsg->dpnidSender,
						pReceiveMsg->pReceiveData, pReceiveMsg->dwReceiveDataSize,
						pContext->dwMsgSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (not special message)


				// If it is the special message, act on it.

				DPTEST_TRACE(pContext->hLog,  "Server indicated leaving client(s) have left.", 0);
				pContext->fPlayersLeftOrDropped = TRUE;

				goto DONE;
			} // end if (size is not expected)

			// The size is right, make sure the data is too.
			if (! IsFilledWithDWord(pReceiveMsg->pReceiveData, pReceiveMsg->dwReceiveDataSize, SENDFILLPATTERN))
			{
				DPTEST_FAIL(pContext->hLog,  "Received invalid message from ID %u/%x (ptr = %x, size = %u)!  DEBUGBREAK()-ing.",
					4, pReceiveMsg->dpnidSender, pReceiveMsg->dpnidSender,
					pReceiveMsg->pReceiveData, pReceiveMsg->dwReceiveDataSize);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not filled with DWORD)

			// If we got here, it's an expected message.  Cool.
		  break;

		case DPN_MSGID_SEND_COMPLETE:
			PDPNMSG_SEND_COMPLETE	pSendCompleteMsg;


			pSendCompleteMsg = (PDPNMSG_SEND_COMPLETE) pvMsg;


			// Lock the stats while we update them.
			EnterCriticalSection(&(pContext->csStatsLock));

			// The context was a boolean describing whether it's a relevant send or not.
			if (((BOOL) (INT_PTR) pSendCompleteMsg->pvUserContext))
			{
				if (pSendCompleteMsg->hResultCode != DPN_OK)
				{
//					DPTEST_TRACE(pContext->hLog, "Completion %x failure.  0x%08x",
//						2, pSendCompleteMsg->hAsyncOp, pSendCompleteMsg->hResultCode);

					pContext->dwNumRelevantSendsCompletedFailure++;
				} // end if (send failed)
				else
				{
					pContext->dwNumRelevantSendsCompletedSuccess++;
				} // end else (send succeeded)

				if ((int) (pContext->dwNumRelevantSends - (pContext->dwNumRelevantSendsCompletedSuccess + pContext->dwNumRelevantSendsCompletedFailure)) < 0)
				{
					DPTEST_TRACE(pContext->hLog,  "Got extra relevant completion ((%u - (%u + %u)) < 0)!  DEBUGBREAK()-ing.",
						3, pContext->dwNumRelevantSends,
						pContext->dwNumRelevantSendsCompletedSuccess,
						pContext->dwNumRelevantSendsCompletedFailure);
					// Ignore error
					pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
													"Got extra relevant completion ((%u - (%u + %u)) < 0)!  DEBUGBREAK()-ing.",
													3, pContext->dwNumRelevantSends,
													pContext->dwNumRelevantSendsCompletedSuccess,
													pContext->dwNumRelevantSendsCompletedFailure);
					DEBUGBREAK();
				} // end if (got too many completions)
			} // end if (relevant message)
			else
			{
				if (pSendCompleteMsg->hResultCode != DPN_OK)
				{
					DPTEST_TRACE(pContext->hLog,  "Irrelevant send failed!  DEBUGBREAK()-ing.  0x%08x",
						1, pSendCompleteMsg->hResultCode);
					// Ignore error
					pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
													"Send failed!  DEBUGBREAK()-ing.  0x%08x",
													1, pSendCompleteMsg->hResultCode);
					DEBUGBREAK();
				} // end if (send failed)
				else
				{
					pContext->dwNumIrrelevantSendsCompleted++;

					if ((int) (pContext->dwNumIrrelevantSends - pContext->dwNumIrrelevantSendsCompleted) < 0)
					{
						DPTEST_TRACE(pContext->hLog,  "Got extra irrelevant completion (%u - %u) < 0)!  DEBUGBREAK()-ing.",
							2, pContext->dwNumIrrelevantSends,
							pContext->dwNumIrrelevantSendsCompleted);
						// Ignore error
						pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
														"Got extra irrelevant completion (%u - %u) < 0)!  DEBUGBREAK()-ing.",
														2, pContext->dwNumIrrelevantSends,
														pContext->dwNumIrrelevantSendsCompleted);
						DEBUGBREAK();
					} // end if (got too many completions)
				} // end else (send succeeded)
			} // end else (not relevant message)

			// Drop the stats lock.
			LeaveCriticalSection(&(pContext->csStatsLock));
		  break;

		case DPN_MSGID_TERMINATE_SESSION:
			PDPNMSG_TERMINATE_SESSION	pTerminateSessionMsg;


			pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION) pvMsg;

			if (! pContext->fClosingPlayer)
			{
				DPTEST_TRACE(pContext->hLog,  "WARNING: Ignoring unexpected TERMINATE_SESSION, this had better be because of a test failure!  0x%08x",
					1, pTerminateSessionMsg->hResultCode);
				goto DONE;
			} // end if (not dropping tester)
			else
			{
				if (pContext->fGotTerminateSession)
				{
					DPTEST_FAIL(pContext->hLog,  "Got duplicate TERMINATE_SESSION message!  DEBUGBREAK()-ing.", 0);

					//Ignore error
					pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
													"Got duplicate TERMINATE_SESSION message!  DEBUGBREAK()-ing.",
													0);

					DEBUGBREAK();

					hr = E_FAIL;
					goto DONE;
				} // end if (got duplicate)

				pContext->fGotTerminateSession = TRUE;


				// Client/server clients can only key off of TERMINATE_SESSION
				// messages (they won't get any DESTROY_PLAYERs).
				if ((pContext->pTNecd->pExecutor->IsCase("2.2.2.3")) ||
					(pContext->pTNecd->pExecutor->IsCase("2.2.2.4")))
				{
					pContext->fPlayersLeftOrDropped = TRUE;
				} // end if (client/server)
			} // end else (dropping tester)
		  break;

		default:
//			DPTEST_FAIL(pContext->hLog,  "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);
			DPTEST_TRACE(pContext->hLog,  "Unexpected message type %x!  Do we need to handle this?", 1, dwMsgType);
//			DEBUGBREAK();

//			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // SendsPendingDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/

} // namespace DPlayCoreNamespace