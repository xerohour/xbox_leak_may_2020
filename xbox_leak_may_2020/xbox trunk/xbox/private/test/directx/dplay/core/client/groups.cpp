//==================================================================================
// Includes
//==================================================================================
#include "dptest.h"
#include "macros.h"
#include "parmvalidation.h"

using namespace DPlayCoreNamespace;

namespace DPlayCoreNamespace {

//==================================================================================
// Structures
//==================================================================================
typedef struct tagGROUPSCONTEXT :public BASESESSIONCREATECONTEXT
{
	HANDLE				hLog;
	HANDLE				hGroupCreatedEvent; // event to set when the create group message comes in
	HANDLE				hPlayerAddedEvent; // event to set when the add player to group message comes in
	HANDLE				hReceiveEvent; // event to set when the message sent to group comes in
	DPNID				dpnidGroup; // ID of group created
} GROUPSCONTEXT, * PGROUPSCONTEXT;


//==================================================================================
// Prototypes
//==================================================================================
HRESULT GroupsDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);

#undef DEBUG_SECTION
#define DEBUG_SECTION	"GroupsExec_SimplePeer()"
//==================================================================================
// GroupsExec_SimplePeer
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.5.1.1 - Simple peer-to-peer group test
//
// Arguments:
//	HANDLE				hLog				Handle to logging subsystem
//	DP_GROUPTESTPARAM	*pGroupTestParam	Pointer to params for this test
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
HRESULT GroupsExec_SimplePeer(HANDLE hLog, DP_GROUPTESTPARAM *pGroupTestParam)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	HRESULT						temphr;
	DWORD						dwSubOutputDataSize;
	GROUPSCONTEXT				context;
	DPN_APPLICATION_DESC		dpnad;
	PWRAPDP8PEER				pDP8Peer = NULL;
	DPNID*						padpnidTesters = NULL;
	HANDLE						hCreateGroupCompleteEvent = NULL;
	DPNHANDLE					dpnhCreateGroup = NULL;
	HANDLE						hAddToGroupCompleteEvent = NULL;
	DPNHANDLE					dpnhAddToGroup = NULL;
	HANDLE						hSendCompleteEvent = NULL;
	DPNHANDLE					dpnhSend = NULL;
	DPN_GROUP_INFO				dpngi;
	DPN_BUFFER_DESC				dpnbd;
	int							i;



	ZeroMemory(&context, sizeof (GROUPSCONTEXT));
	context.hLog = hLog;

	BEGIN_TESTCASE
	{
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

		if(pGroupTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating session host");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			// When function returns we should have a valid peer and an array of tester DPNID's
			if(!CreateHostAndWaitForPlayers(hLog, &pDP8Peer, &context, &dpnad, GroupsDPNMessageHandler,
				&padpnidTesters, pGroupTestParam->dwNumMachines, 120000))
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
			if(!ConnectNonHostAndWaitForPlayers(hLog, &pDP8Peer, &context, pGroupTestParam->dwHostAddr, &dpnad, GroupsDPNMessageHandler,
				&padpnidTesters, pGroupTestParam->dwNumMachines, 120000))
			{
				DPTEST_FAIL(hLog, "Couldn't create host and receive correct number of players", 0);
				SETTHROW_TESTRESULT(E_ABORT);
			}
		}


		CREATEEVENT_OR_THROW(context.hGroupCreatedEvent, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(context.hPlayerAddedEvent, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(context.hReceiveEvent, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(hCreateGroupCompleteEvent, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(hAddToGroupCompleteEvent, NULL, FALSE, FALSE, NULL);
		CREATEEVENT_OR_THROW(hSendCompleteEvent, NULL, FALSE, FALSE, NULL);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for everyone to be ready for group creation");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!SyncWithOtherTesters(hLog, pGroupTestParam->hNetsyncObject, GROUP_BEGIN_EVENT, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, GROUP_BEGIN_EVENT);
			SETTHROW_TESTRESULT(E_ABORT);
		}
		

		// Do this twice, the first time the host will create the group and send,
		// the second time the joiner will.
		for(i = 0; i < 2; i++)
		{
			// If it's our turn to create the group, do so.
			if((pGroupTestParam->fHostTest && (i == 0)) || (!pGroupTestParam->fHostTest && (i == 1)))
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Creating group");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				ZeroMemory(&dpngi, sizeof (DPN_GROUP_INFO));
				dpngi.dwSize = sizeof (DPN_GROUP_INFO);
// BUGBUG - Group names not working
//				dpngi.dwInfoFlags = DPNINFO_NAME;
//				dpngi.pwszName = L"Group name";
				//dpngi.pvData = NULL;
				//dpngi.dwDataSize = 0;
				//dpngi.dwGroupFlags = 0;

				tr = pDP8Peer->DP8P_CreateGroup(&dpngi,
												NULL,
												hCreateGroupCompleteEvent,
												&dpnhCreateGroup,
												0);
				if (tr != (HRESULT) DPNSUCCESS_PENDING)
				{
					DPTEST_FAIL(hLog,  "Couldn't start creating group!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't create group)

				if (dpnhCreateGroup == NULL)
				{
					DPTEST_FAIL(hLog,  "CreateGroup async operation handle was not set!", 0);
					SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
				} // end if (async handle was not set)

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for group to be created");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				
				if(WaitForSingleObject(context.hGroupCreatedEvent, 30000) != WAIT_OBJECT_0)
				{
					DPTEST_FAIL(hLog, "Group wasn't created within 30 seconds", 0);
					THROW_SYSTEMRESULT;
				}

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for CreateGroup op to complete");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				if(WaitForSingleObject(hCreateGroupCompleteEvent, 30000) != WAIT_OBJECT_0)
				{
					DPTEST_FAIL(hLog, "Group creation operation didn't complete within 30 seconds", 0);
					THROW_SYSTEMRESULT;
				}

				dpnhCreateGroup = NULL;

				CloseHandle(hCreateGroupCompleteEvent);
				hCreateGroupCompleteEvent = NULL;

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Add other tester to group");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Peer->DP8P_AddPlayerToGroup(context.dpnidGroup,
													padpnidTesters[pGroupTestParam->fHostTest ? 1 : 0],
													hAddToGroupCompleteEvent,
													&dpnhAddToGroup,
													0);
				if (tr != (HRESULT) DPNSUCCESS_PENDING)
				{
					DPTEST_FAIL(hLog,  "Couldn't start adding player to group!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't add player to group)

				if (dpnhAddToGroup == NULL)
				{
					DPTEST_FAIL(hLog,  "AddPlayerToGroup async operation handle was not set!", 0);
					SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
				} // end if (async handle was not set)

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for player to be added group");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				if(WaitForSingleObject(context.hPlayerAddedEvent, 30000) != WAIT_OBJECT_0)
				{
					DPTEST_FAIL(hLog, "Player wasn't added within 30 seconds", 0);
					THROW_SYSTEMRESULT;
				}

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for AddPlayerToGroup op to complete");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				if(WaitForSingleObject(hAddToGroupCompleteEvent, 30000) != WAIT_OBJECT_0)
				{
					DPTEST_FAIL(hLog, "Player add operation didn't complete within 30 seconds", 0);
					THROW_SYSTEMRESULT;
				}

				dpnhAddToGroup = NULL;


				CloseHandle(hAddToGroupCompleteEvent);
				hAddToGroupCompleteEvent = NULL;

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Sending guaranteed message to group");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				ZeroMemory(&dpnbd, sizeof (DPN_BUFFER_DESC));
				dpnbd.pBufferData = (PBYTE) "My String";
				dpnbd.dwBufferSize = strlen("My String") + 1;

				tr = pDP8Peer->DP8P_SendTo(context.dpnidGroup,
											&dpnbd,
											1,
											0,
											hSendCompleteEvent,
											&dpnhSend,
											DPNSEND_NOCOPY | DPNSEND_GUARANTEED);
				if (tr != (HRESULT) DPNSUCCESS_PENDING)
				{
					DPTEST_FAIL(hLog,  "Couldn't start sending to group!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't send to)

				if (dpnhSend == NULL)
				{
					DPTEST_FAIL(hLog,  "Send async operation handle was not set!", 0);
					SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
				} // end if (async handle was not set)

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for send to complete");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				if(WaitForSingleObject(hSendCompleteEvent, 30000) != WAIT_OBJECT_0)
				{
					DPTEST_FAIL(hLog, "Send operation didn't complete within 30 seconds", 0);
					THROW_SYSTEMRESULT;
				}

				CloseHandle(hSendCompleteEvent);
				hSendCompleteEvent = NULL;

			} // end if (our turn to create group)
			else
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for group to be created");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				if(WaitForSingleObject(context.hGroupCreatedEvent, 30000) != WAIT_OBJECT_0)
				{
					DPTEST_FAIL(hLog, "Group wasn't created within 30 seconds", 0);
					THROW_SYSTEMRESULT;
				}

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for player to be added to group");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				if(WaitForSingleObject(context.hPlayerAddedEvent, 30000) != WAIT_OBJECT_0)
				{
					DPTEST_FAIL(hLog, "Player wasn't added within 30 seconds", 0);
					THROW_SYSTEMRESULT;
				}

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting to receive message");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				if(WaitForSingleObject(context.hReceiveEvent, 30000) != WAIT_OBJECT_0)
				{
					DPTEST_FAIL(hLog, "Message wasn't received within 30 seconds", 0);
					THROW_SYSTEMRESULT;
				}

				CloseHandle(context.hReceiveEvent);
				context.hReceiveEvent = NULL;

			} // end else (not our turn to create group)
		} // end for (each player times)


		CloseHandle(context.hGroupCreatedEvent);
		context.hGroupCreatedEvent = NULL;

		CloseHandle(context.hPlayerAddedEvent);
		context.hPlayerAddedEvent = NULL;

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
	SAFE_CLOSEHANDLE(hCreateGroupCompleteEvent);
	SAFE_CLOSEHANDLE(context.hGroupCreatedEvent);
	SAFE_CLOSEHANDLE(hAddToGroupCompleteEvent);
	SAFE_CLOSEHANDLE(context.hPlayerAddedEvent);
	SAFE_CLOSEHANDLE(hSendCompleteEvent);
	SAFE_CLOSEHANDLE(context.hReceiveEvent);

	return (sr);
} // GroupsExec_SimplePeer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"GroupsExec_SimpleCS()"
//==================================================================================
// GroupsExec_SimpleCS
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.5.2.1 - Simple client/server group test
//
// Arguments:
//	HANDLE				hLog				Handle to logging subsystem
//	DP_GROUPTESTPARAM	*pGroupTestParam	Pointer to params for this test
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
HRESULT GroupsExec_SimpleCS(HANDLE hLog, DP_GROUPTESTPARAM *pGroupTestParam)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	HRESULT							temphr;
	GROUPSCONTEXT					context;
	DPN_APPLICATION_DESC			dpnad;
	PWRAPDP8SERVER					pDP8Server = NULL;
	PWRAPDP8CLIENT					pDP8Client = NULL;
	DPNID*							padpnidTesters = NULL;
	HANDLE							hCreateGroupCompleteEvent = NULL;
	DPNHANDLE						dpnhCreateGroup = NULL;
	HANDLE							hAddToGroupCompleteEvent = NULL;
	DPNHANDLE						dpnhAddToGroup = NULL;
	HANDLE							hSendCompleteEvent = NULL;
	DPNHANDLE						dpnhSend = NULL;
	DPN_GROUP_INFO					dpngi;
	DPN_BUFFER_DESC					dpnbd;



	ZeroMemory(&context, sizeof (GROUPSCONTEXT));
	context.hLog = hLog;

	BEGIN_TESTCASE
	{
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


		if(pGroupTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating session server");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			// When function returns we should have a valid server and an array of tester DPNID's
			if(!CreateServerAndWaitForPlayers(hLog, &pDP8Server, &context, &dpnad, GroupsDPNMessageHandler,
				&padpnidTesters, pGroupTestParam->dwNumMachines, 120000))
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
			if(!ConnectClient(hLog, &pDP8Client, &context, pGroupTestParam->dwHostAddr, &dpnad,
				GroupsDPNMessageHandler, 120000))
			{
				DPTEST_FAIL(hLog, "Couldn't connect client", 0);
				SETTHROW_TESTRESULT(E_ABORT);
			}
		}


		if (pGroupTestParam->fHostTest)
		{
			CREATEEVENT_OR_THROW(context.hGroupCreatedEvent, NULL, FALSE, FALSE, NULL);
			CREATEEVENT_OR_THROW(context.hPlayerAddedEvent, NULL, FALSE, FALSE, NULL);
			CREATEEVENT_OR_THROW(hCreateGroupCompleteEvent, NULL, FALSE, FALSE, NULL);
			CREATEEVENT_OR_THROW(hAddToGroupCompleteEvent, NULL, FALSE, FALSE, NULL);
			CREATEEVENT_OR_THROW(hSendCompleteEvent, NULL, FALSE, FALSE, NULL);
		} // end if (tester 0)

		CREATEEVENT_OR_THROW(context.hReceiveEvent, NULL, FALSE, FALSE, NULL);





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for everyone to be ready for group creation");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!SyncWithOtherTesters(hLog, pGroupTestParam->hNetsyncObject, GROUP_BEGIN_EVENT, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, GROUP_BEGIN_EVENT);
			SETTHROW_TESTRESULT(E_ABORT);
		}
		

		// The server should create a group, add himself and the other tester to it,
		// and send to.  Both testers should receive the message.
		if(pGroupTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating group");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			ZeroMemory(&dpngi, sizeof (DPN_GROUP_INFO));
			dpngi.dwSize = sizeof (DPN_GROUP_INFO);
// BUGBUG - Group names not working
//			dpngi.dwInfoFlags = DPNINFO_NAME;
//			dpngi.pwszName = L"Group name";
			//dpngi.pvData = NULL;
			//dpngi.dwDataSize = 0;
			//dpngi.dwGroupFlags = 0;

			tr = pDP8Server->DP8S_CreateGroup(&dpngi,
											NULL,
											hCreateGroupCompleteEvent,
											&dpnhCreateGroup,
											0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog,  "Couldn't start creating group!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't create group)

			if (dpnhCreateGroup == NULL)
			{
				DPTEST_FAIL(hLog,  "CreateGroup async operation handle was not set!", 0);
				SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
			} // end if (async handle was not set)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for group to be created");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			if(WaitForSingleObject(context.hGroupCreatedEvent, 30000) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Group wasn't created within 30 seconds", 0);
				THROW_SYSTEMRESULT;
			}

			CloseHandle(context.hGroupCreatedEvent);
			context.hGroupCreatedEvent = NULL;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for CreateGroup op to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(hCreateGroupCompleteEvent, 30000) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Group creation operation didn't complete within 30 seconds", 0);
				THROW_SYSTEMRESULT;
			}

			dpnhCreateGroup = NULL;

			CloseHandle(hCreateGroupCompleteEvent);
			hCreateGroupCompleteEvent = NULL;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Add self to group");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Server->DP8S_AddPlayerToGroup(context.dpnidGroup,
												padpnidTesters[0],
												hAddToGroupCompleteEvent,
												&dpnhAddToGroup,
												0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog,  "Couldn't start adding self to group!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't add player to group)

			if (dpnhAddToGroup == NULL)
			{
				DPTEST_FAIL(hLog,  "AddPlayerToGroup async operation handle was not set!", 0);
				SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
			} // end if (async handle was not set)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for player to be added group");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(context.hPlayerAddedEvent, 30000) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Player wasn't added within 30 seconds", 0);
				THROW_SYSTEMRESULT;
			}

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for AddPlayerToGroup op to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(hAddToGroupCompleteEvent, 30000) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Player add operation didn't complete within 30 seconds", 0);
				THROW_SYSTEMRESULT;
			}

			dpnhAddToGroup = NULL;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Add other tester to group");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Server->DP8S_AddPlayerToGroup(context.dpnidGroup,
												padpnidTesters[1],
												hAddToGroupCompleteEvent,
												&dpnhAddToGroup,
												0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog,  "Couldn't start adding player to group!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't add player to group)

			if (dpnhAddToGroup == NULL)
			{
				DPTEST_FAIL(hLog,  "AddPlayerToGroup async operation handle was not set!", 0);
				SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
			} // end if (async handle was not set)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for player to be added group");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(context.hPlayerAddedEvent, 30000) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Player wasn't added within 30 seconds", 0);
				THROW_SYSTEMRESULT;
			}

			CloseHandle(context.hPlayerAddedEvent);
			context.hPlayerAddedEvent = NULL;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for AddPlayerToGroup op to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(hAddToGroupCompleteEvent, 30000) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Player add operation didn't complete within 30 seconds", 0);
				THROW_SYSTEMRESULT;
			}

			dpnhAddToGroup = NULL;


			CloseHandle(hAddToGroupCompleteEvent);
			hAddToGroupCompleteEvent = NULL;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Sending guaranteed message to group");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			ZeroMemory(&dpnbd, sizeof (DPN_BUFFER_DESC));
			dpnbd.pBufferData = (PBYTE) "My String";
			dpnbd.dwBufferSize = strlen("My String") + 1;

			tr = pDP8Server->DP8S_SendTo(context.dpnidGroup,
										&dpnbd,
										1,
										0,
										hSendCompleteEvent,
										&dpnhSend,
										DPNSEND_NOCOPY | DPNSEND_GUARANTEED);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog,  "Couldn't start sending to group!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't send to)

			if (dpnhSend == NULL)
			{
				DPTEST_FAIL(hLog,  "Send async operation handle was not set!", 0);
				SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
			} // end if (async handle was not set)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for send to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(hSendCompleteEvent, 30000) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Send operation didn't complete within 30 seconds", 0);
				THROW_SYSTEMRESULT;
			}

			CloseHandle(hSendCompleteEvent);
			hSendCompleteEvent = NULL;

		} // end if (server)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting to receive message");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
		if(WaitForSingleObject(context.hReceiveEvent, 30000) != WAIT_OBJECT_0)
		{
			DPTEST_FAIL(hLog, "Message wasn't received within 30 seconds", 0);
			THROW_SYSTEMRESULT;
		}

		CloseHandle(context.hReceiveEvent);
		context.hReceiveEvent = NULL;

		if (pGroupTestParam->fHostTest)
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
	SAFE_CLOSEHANDLE(hCreateGroupCompleteEvent);
	SAFE_CLOSEHANDLE(context.hGroupCreatedEvent);
	SAFE_CLOSEHANDLE(hAddToGroupCompleteEvent);
	SAFE_CLOSEHANDLE(context.hPlayerAddedEvent);
	SAFE_CLOSEHANDLE(hSendCompleteEvent);
	SAFE_CLOSEHANDLE(context.hReceiveEvent);

	return (sr);
} // GroupsExec_SimpleCS
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"GroupsDPNMessageHandler()"
//==================================================================================
// GroupsDPNMessageHandler
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
HRESULT GroupsDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT			hr = DPN_OK;
	PGROUPSCONTEXT	pContext = (PGROUPSCONTEXT) pvContext;
	DWORD			dwCurrentPlayers = 0;


	switch (dwMsgType)
	{
		case DPN_MSGID_ADD_PLAYER_TO_GROUP:
			PDPNMSG_ADD_PLAYER_TO_GROUP		pAddPlayerToGroupMsg;


			pAddPlayerToGroupMsg = (PDPNMSG_ADD_PLAYER_TO_GROUP) pvMsg;

#pragma BUGBUG(vanceo, "Validate")

			if (! SetEvent(pContext->hPlayerAddedEvent))
			{
				hr = GetLastError();

				DPTEST_TRACE(pContext->hLog,  "Couldn't set player added event (%x)!  DEBUGBREAK()-ing.  0x%08x",
					2, pContext->hPlayerAddedEvent, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_ASYNC_OP_COMPLETE:
			PDPNMSG_ASYNC_OP_COMPLETE	pAsyncOpCompleteMsg;


			pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE) pvMsg;

#pragma TODO(vanceo, "Verify result code is DPN_OK")

			if (! SetEvent(pAsyncOpCompleteMsg->pvUserContext))
			{
				hr = GetLastError();

				DPTEST_TRACE(pContext->hLog,  "Couldn't set async op completion event (%x)!  DEBUGBREAK()-ing.  0x%08x",
					2, pAsyncOpCompleteMsg->pvUserContext, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_CREATE_GROUP:
			PDPNMSG_CREATE_GROUP	pCreateGroupMsg;


			pCreateGroupMsg = (PDPNMSG_CREATE_GROUP) pvMsg;

#pragma BUGBUG(vanceo, "Validate")

			pContext->dpnidGroup = pCreateGroupMsg->dpnidGroup;

			if (! SetEvent(pContext->hGroupCreatedEvent))
			{
				hr = GetLastError();

				DPTEST_TRACE(pContext->hLog,  "Couldn't set group creation event (%x)!  DEBUGBREAK()-ing.  0x%08x",
					2, pContext->hGroupCreatedEvent, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_DESTROY_GROUP:
			PDPNMSG_DESTROY_GROUP	pDestroyGroupMsg;


			pDestroyGroupMsg = (PDPNMSG_DESTROY_GROUP) pvMsg;

#pragma BUGBUG(vanceo, "Validate")
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			// Safely increment the number of player create messages we've received
			dwCurrentPlayers = InterlockedIncrement((LPLONG) (&pContext->dwPlayersCreated));

			// If all the expected players have connected, then signal the main thread
			if(dwCurrentPlayers == pContext->dwPlayersExpected)
			{
				if(!SetEvent(pContext->hbsccAllPlayersCreated))
				{
					DPTEST_TRACE(pContext->hLog,  "SetEvent failed!  DEBUGBREAK()-ing.  0x%08x", 0);
					DEBUGBREAK();

					hr = E_FAIL;
					goto DONE;
				}
				DPTEST_TRACE(pContext->hLog,  "All %u expected players created", 1, dwCurrentPlayers);
			}
			break;

		case DPN_MSGID_DESTROY_PLAYER:
			// Indicate that some players have left
			pContext->fPlayersHaveLeft = TRUE;
			break;

		case DPN_MSGID_RECEIVE:
			PDPNMSG_RECEIVE		pReceiveMsg;


			pReceiveMsg = (PDPNMSG_RECEIVE) pvMsg;

			DPTEST_TRACE(pContext->hLog,  "DPN_MSGID_RECEIVE", 0);
			DPTEST_TRACE(pContext->hLog,  "    dwSize = %u", 1, pReceiveMsg->dwSize);
			DPTEST_TRACE(pContext->hLog,  "    dpnidSender = %u/%x", 2, pReceiveMsg->dpnidSender, pReceiveMsg->dpnidSender);
			DPTEST_TRACE(pContext->hLog,  "    pvPlayerContext = %x", 1, pReceiveMsg->pvPlayerContext);
			DPTEST_TRACE(pContext->hLog,  "    pReceiveData = %x", 1, pReceiveMsg->pReceiveData);
			DPTEST_TRACE(pContext->hLog,  "    dwReceiveDataSize = %u", 1, pReceiveMsg->dwReceiveDataSize);
			DPTEST_TRACE(pContext->hLog,  "    hBufferHandle = %x", 1, pReceiveMsg->hBufferHandle);

#pragma TODO(vanceo, "Verify message")

			if (! SetEvent(pContext->hReceiveEvent))
			{
				hr = GetLastError();

				DPTEST_TRACE(pContext->hLog,  "Couldn't set receive event (%x)!  DEBUGBREAK()-ing.  0x%08x",
					2, pContext->hReceiveEvent, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_REMOVE_PLAYER_FROM_GROUP:
#pragma TODO(vanceo, "Verify message")
		  break;

		case DPN_MSGID_SEND_COMPLETE:
			PDPNMSG_SEND_COMPLETE	pSendCompleteMsg;


			pSendCompleteMsg = (PDPNMSG_SEND_COMPLETE) pvMsg;

			DPTEST_TRACE(pContext->hLog,  "DPN_MSGID_SEND_COMPLETE", 0);
			DPTEST_TRACE(pContext->hLog,  "    dwSize = %u", 1, pSendCompleteMsg->dwSize);
			DPTEST_TRACE(pContext->hLog,  "    hAsyncOp = %x", 1, pSendCompleteMsg->hAsyncOp);
			DPTEST_TRACE(pContext->hLog,  "    pvUserContext = %u", 1, pSendCompleteMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog,  "    hResultCode = %x", 1, pSendCompleteMsg->hResultCode);
			DPTEST_TRACE(pContext->hLog,  "    dwSendTime = %u", 1, pSendCompleteMsg->dwSendTime);

#pragma TODO(vanceo, "Verify result code is DPN_OK")

			if (! SetEvent(pSendCompleteMsg->pvUserContext))
			{
				hr = GetLastError();

				DPTEST_TRACE(pContext->hLog,  "Couldn't set send completion event (%x)!  DEBUGBREAK()-ing.  0x%08x",
					2, pSendCompleteMsg->pvUserContext, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;
			} // end if (couldn't set event)
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
} // GroupsDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

} // namespace DPlayCoreNamespace