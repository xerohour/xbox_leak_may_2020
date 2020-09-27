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
#define ENUMDATAPATTERN_APPDESC		0x12345678
#define ENUMDATAPATTERN_QUERY		0x0711F00D
#define ENUMDATAPATTERN_RESPONSE	0xB001B00F

#define ENUM_HOSTS_TIMEOUT			60000
#define ENUM_SESSION_NAME			L"EnumSession"

#define ADDRESSES_EVENT_NAME		"HostAddresses"
#define ENUMSDONE_EVENT_NAME		"EnumsDone"
#define BUFRETURNED_EVENT_NAME		"BufReturned"

#define VALHOSTADDR_EVENT_NAME		"ValHostAddrs"
#define CSTOPPDONE_EVENT_NAME		"CS2PPDone"
#define PPTOCSDONE_EVENT_NAME		"PP2CSDone"
#define CSCAPPED_EVENT_NAME			"CSCapped"
#define CSMAXPLAYER_EVENT_NAME		"CSMaxPlayer"
#define PPCAPPED_EVENT_NAME			"PPCapped"
#define PPMAXPLAYER_EVENT_NAME		"PPMaxPlayer"
#define CSPW_EVENT_NAME				"CSPW"
#define CSPWDONE_EVENT_NAME			"CSPWDone"
#define PPPW_EVENT_NAME				"PPPW"
#define PPPWDONE_EVENT_NAME			"PPPWDone"
#define NONHOST_EVENT_NAME			"NonHost"
#define NONHOSTDONE_EVENT_NAME		"NonHostDone"

//==================================================================================
// Structures
//==================================================================================

typedef struct tagHOSTADDRESSESSYNCDATA
{
	DWORD				dwNumAddresses; // number of addresses the host has in the following buffer
	
	// Anything after this is part of the host's addresses blob.
} HOSTADDRESSESSYNCDATA, * PHOSTADDRESSESSYNCDATA;

typedef struct tagHOSTADDRESSESCSANDPEERSYNCDATA
{
	DWORD				dwNumAddressesCS; // number of addresses for c/s session the host has in the following buffer
	DWORD				dwNumAddressesPeer; // number of addresses for peer session the host has in the following buffer
	
	// Anything after this is part of the host's addresses blobs.
} HOSTADDRESSESCSANDPEERSYNCDATA, * PHOSTADDRESSESCSANDPEERSYNCDATA;

typedef struct tagENUMSSIMPLECONTEXT
{
	PDP_ENUMTESTPARAM	pEnumTestParam; // pointer to test parameters
	HANDLE				hLog; // handle to logging subsystem
	DWORD				dwNumMsgs; // number of messages received (queries on host, responses on clients)
	DWORD				dwAppDescDataSize; // size of application desc data expected
	PVOID				pvUserData; // pointer to user data payload to send with query/response
	DWORD				dwUserDataSize; // size of user data payload to send with query/response
	BOOL				fDontRespondWithData; // boolean that will be set when responses should not be responded to with data anymore
	LONG				lNumOutstandingResponseData; // number of outstanding responses that haven't returned the data yet
	BOOL				fReject; // whether the host should refuse to respond or not
	BOOL				fCancelled; // has the enum been cancelled yet
} ENUMSSIMPLECONTEXT, * PENUMSSIMPLECONTEXT;

typedef struct tagENUMSVALIDATECONTEXT
{
	PDP_ENUMTESTPARAM	pEnumTestParam; // pointer to test parameters
	HANDLE				hLog; // handle to logging subsystem
	DWORD				dwNumMsgs; // number of messages received (queries on host, responses on clients)
	DWORD				dwExpectedMaxPlayers; // expected number of max players in enum app descs
} ENUMSVALIDATECONTEXT, * PENUMSVALIDATECONTEXT;




//==================================================================================
// Prototypes
//==================================================================================

HRESULT EnumsSimpleDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT EnumsValidateDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
//HRESULT EnumsStartUpDownDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);


#undef DEBUG_SECTION
#define DEBUG_SECTION	"EnumsExec_SimpleAll()"
//==================================================================================
// EnumsExec_SimpleAll
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.4.1 - Simple peer-to-peer enumeration test
//				2.4.2 - Simple client/server enumeration test
//
// Arguments:
//	HANDLE				hLog				Handle to the logging subsystem
//	DP_ENUMTESTPARAM	*pEnumTestParam		Pointer to parameters for this test
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT EnumsExec_SimpleAll(HANDLE hLog, PDP_ENUMTESTPARAM pEnumTestParam)
{
	CTNSystemResult			sr;
	CTNTestResult			tr;
	ENUMSSIMPLECONTEXT		context;
	DPN_APPLICATION_DESC	dpnad;
	PWRAPDP8PEER			pDP8Peer = NULL;
	PWRAPDP8SERVER			pDP8Server = NULL;
	PWRAPDP8CLIENT			pDP8Client = NULL;
	PDIRECTPLAY8ADDRESS		pDP8AddressLocal = NULL;
	PDIRECTPLAY8ADDRESS		pDP8AddressRemote = NULL;
	HANDLE					hEnumDoneEvent = NULL;
	DPNHANDLE				dpnhEnumHosts = NULL;
	PHOSTADDRESSESSYNCDATA	pHostAddressesSyncData = NULL;
	PDIRECTPLAY8ADDRESS*	paDP8HostAddresses = NULL;
	LPVOID					pvSyncData = NULL;
	DWORD					dwSyncDataSize;
	DWORD					dwNumAddresses = 0;
	DWORD					dwWaitResult;
	DWORD					dwTemp;
	DWORD					dwURLsBufferSize = 0;
	DWORD					dwRemainingBufferSize;
	char*					pszURL;
	DWORD					dwSize;
	DWORD					dwDataType;
	WCHAR					wszIPCompareString[16]; // max IP string size + NULL termination

	ZeroMemory(&context, sizeof (ENUMSSIMPLECONTEXT));
	ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));

	BEGIN_TESTCASE
	{
		context.dwAppDescDataSize = pEnumTestParam->dwAppDescDataSize;
		context.dwUserDataSize = pEnumTestParam->dwUserEnumDataSize;
		context.pEnumTestParam = pEnumTestParam;
		context.hLog = hLog;

		if (context.dwUserDataSize > 0)
		{
			LOCALALLOC_OR_THROW( PVOID, context.pvUserData, context.dwUserDataSize);

			// Fill the memory with some pattern (different for queries vs.
			// responses).
			FillWithDWord(context.pvUserData, context.dwUserDataSize,
						((pEnumTestParam->fHostTest) ? ENUMDATAPATTERN_RESPONSE : ENUMDATAPATTERN_QUERY));
		} // end if (data should be sent)

		context.fReject = pEnumTestParam->fReject;


		if(pEnumTestParam->fClientServer)
		{

			if(pEnumTestParam->fHostTest)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Creating DirectPlay8Server object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				pDP8Server = new CWrapDP8Server(hLog);
				if (pDP8Server == NULL)
				{
					SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
				} // end if (couldn't allocate object)


				tr = pDP8Server->CoCreate();
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "CoCreating DP8Server object failed!", 0);
					THROW_TESTRESULT;
				} // end if (function failed)

			} // end if (tester 0)
			else
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Creating DirectPlay8Client object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				pDP8Client = new CWrapDP8Client(hLog);
				if (pDP8Client == NULL)
				{
					SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
				} // end if (couldn't allocate object)


				tr = pDP8Client->CoCreate();
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "CoCreating DP8Client object failed!", 0);
					THROW_TESTRESULT;
				} // end if (function failed)

			} // end else (not tester 0)
			
		} // end if (client/server)
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating DirectPlay8Peer object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			pDP8Peer = new CWrapDP8Peer(hLog);
			if (pDP8Peer == NULL)
			{
				SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
			} // end if (couldn't allocate object)


			tr = pDP8Peer->CoCreate();
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "CoCreating DP8Peer object failed!", 0);
				THROW_TESTRESULT;
			} // end if (function failed)

		} // end else (peer-to-peer)
		



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("DirectPlay8AddressCreate local address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8AddressLocal, NULL);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't DirectPlay8AddressCreate address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (couldn't CoCreate object)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting local address object's SP to TCP/IP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressLocal->SetSP(&CLSID_DP8SP_TCPIP);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't set SP for address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if (pEnumTestParam->fClientServer)
		{
			if (pEnumTestParam->fHostTest)
			{
				tr = pDP8Server->DP8S_Initialize(&context,
												EnumsSimpleDPNMessageHandler,
												0);
			} // end if pEnumTestParam->fHostTest
			else
			{
				tr = pDP8Client->DP8C_Initialize(&context,
												EnumsSimpleDPNMessageHandler,
												0);
			} // end else !pEnumTestParam->fHostTest
		} // end if (client/server)
		else
		{
			tr = pDP8Peer->DP8P_Initialize(&context,
											EnumsSimpleDPNMessageHandler,
											0);
		} // end else (peer-to-peer)

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't initialize!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)

		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		if (pEnumTestParam->fHostTest && pEnumTestParam->fClientServer)
			dpnad.dwFlags |= DPNSESSION_CLIENT_SERVER;
		dpnad.guidApplication = GUID_ENUMS_SIMPLE;
		dpnad.pwszSessionName = pEnumTestParam->fHostTest ? ENUM_SESSION_NAME : NULL;
		if (pEnumTestParam->fHostTest && pEnumTestParam->dwAppDescDataSize)
		{
			dpnad.dwApplicationReservedDataSize = pEnumTestParam->dwAppDescDataSize;
			LOCALALLOC_OR_THROW( PVOID, dpnad.pvApplicationReservedData, pEnumTestParam->dwAppDescDataSize);

			FillWithDWord(dpnad.pvApplicationReservedData, pEnumTestParam->dwAppDescDataSize, ENUMDATAPATTERN_APPDESC);
		} // end if (hosting test and there's app desc data)


		// Tester 0 should start hosting a session, tester 1 should wait until
		// the session is up.
		if(pEnumTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Hosting session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if (pEnumTestParam->fClientServer)
			{
				tr = pDP8Server->DP8S_Host(&dpnad, &pDP8AddressLocal, 1, NULL, NULL, NULL, 0);
			} // end if (client/server)
			else
			{
				tr = pDP8Peer->DP8P_Host(&dpnad, &pDP8AddressLocal, 1, NULL, NULL, NULL, 0);
			} // end else (peer-to-peer)

			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't start hosting session!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't host session)


			pDP8AddressLocal->Release();
			pDP8AddressLocal = NULL;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Getting host addresses");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if (pEnumTestParam->fClientServer)
			{
				tr = pDP8Server->DP8S_GetLocalHostAddresses(NULL, &dwNumAddresses, 0);
			} // end if (client/server)
			else
			{
				tr = pDP8Peer->DP8P_GetLocalHostAddresses(NULL, &dwNumAddresses, 0);
			} // end else (peer-to-peer)

			if (tr != DPNERR_BUFFERTOOSMALL)
			{
				DPTEST_FAIL(hLog, "Couldn't get number of host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't get host address)

			DPTEST_TRACE(hLog, "We are hosting on %u addresses.", 1, dwNumAddresses); 

			LOCALALLOC_OR_THROW( PDIRECTPLAY8ADDRESS*, paDP8HostAddresses, dwNumAddresses * sizeof (PDIRECTPLAY8ADDRESS));


			if (pEnumTestParam->fClientServer)
			{
				tr = pDP8Server->DP8S_GetLocalHostAddresses(paDP8HostAddresses, &dwNumAddresses, 0);
			} // end if (client/server)
			else
			{
				tr = pDP8Peer->DP8P_GetLocalHostAddresses(paDP8HostAddresses, &dwNumAddresses, 0);
			} // end else (peer-to-peer)

			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't get host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't get host address)


			// Get the total size of all the addresses.
			for(dwTemp = 0; dwTemp < dwNumAddresses; dwTemp++)
			{
				dwSize = 0;
				tr = paDP8HostAddresses[dwTemp]->GetURLA(NULL, &dwSize);
				if (tr != DPNERR_BUFFERTOOSMALL)
				{
					DPTEST_FAIL(hLog, "Couldn't get host address %u's URL size!", 1, dwTemp);
					THROW_TESTRESULT;
				} // end if (not buffer too small)
				
				dwURLsBufferSize += dwSize;
			} // end for (each address)

			// Allocate a buffer for all the URLs.
			LOCALALLOC_OR_THROW( PHOSTADDRESSESSYNCDATA,
								pHostAddressesSyncData,
								sizeof (HOSTADDRESSESSYNCDATA) + dwURLsBufferSize);

			pHostAddressesSyncData->dwNumAddresses = dwNumAddresses;
			pszURL = (char*) (pHostAddressesSyncData + 1);
			dwRemainingBufferSize = dwURLsBufferSize;


			// Actually fill in all the URLs and release the objects (since we're
			// done with them).
			for(dwTemp = 0; dwTemp < dwNumAddresses; dwTemp++)
			{
				dwSize = dwRemainingBufferSize;
				tr = paDP8HostAddresses[dwTemp]->GetURLA(pszURL,
														&dwSize);
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't get host address %u's URL!", 1, dwTemp);
					THROW_TESTRESULT;
				} // end if (failed)


				DPTEST_TRACE(hLog, "Address %u: \"%s\"", 2, dwTemp, pszURL);

				dwRemainingBufferSize -= dwSize;
				pszURL += dwSize;

				
				// We're done with this address object.
				paDP8HostAddresses[dwTemp]->Release();
				paDP8HostAddresses[dwTemp] = NULL;
			} // end for (each address)

			// We're done with the address array.
			MemFree(paDP8HostAddresses);
			paDP8HostAddresses = NULL;


			// Make sure we used all of the buffer.
			if (dwRemainingBufferSize != 0)
			{
				DPTEST_FAIL(hLog, "Didn't use all of or overran the allocated buffer (didn't use %i bytes)!",
					1, ((int) dwRemainingBufferSize));
				SETTHROW_TESTRESULT(E_FAIL);
			} // end if (didn't use all of the buffer)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Sending session addresses to enumerator");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, ADDRESSES_EVENT_NAME, (PBYTE) pHostAddressesSyncData,
				sizeof(HOSTADDRESSESSYNCDATA) + dwURLsBufferSize, NULL, 0))
			{
				DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, ADDRESSES_EVENT_NAME);
				SETTHROW_TESTRESULT(E_ABORT);
			}

		} // end if (tester 0)
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Receiving addresses of host");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			LOCALALLOC_OR_THROW( LPVOID, pvSyncData, MAX_SYNC_DATASIZE);

			dwSyncDataSize = MAX_SYNC_DATASIZE;
			if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, ADDRESSES_EVENT_NAME, NULL, 0, (PBYTE) pvSyncData, &dwSyncDataSize))
			{
				DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, ADDRESSES_EVENT_NAME);
				SETTHROW_TESTRESULT(E_ABORT);
			}

			if(dwSyncDataSize < (DWORD) (sizeof (HOSTADDRESSESSYNCDATA) + (strlen(DPNA_HEADER_A) + 1)))
			{
				DPTEST_FAIL(hLog, "Received sync data is below minimum amount (%u < %u)", 2,
					dwSyncDataSize, (sizeof (HOSTADDRESSESSYNCDATA) + (strlen(DPNA_HEADER_A) + 1)));
				SETTHROW_TESTRESULT(E_ABORT);
			}

			dwNumAddresses = ((PHOSTADDRESSESSYNCDATA) pvSyncData)->dwNumAddresses;

			// Start with the first address
			pszURL = (char*) (((PBYTE) pvSyncData) + sizeof (HOSTADDRESSESSYNCDATA));

			DPTEST_TRACE(hLog, "Tester 0 (host) has %u connectable addresses.", 1, dwNumAddresses);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("CoCreating remote (host's) address object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8AddressRemote, NULL);
			if (sr != S_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't DirectPlay8CreateAddress remote (host's) address object!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't CoCreate object)

			for(dwTemp = 0; dwTemp < dwNumAddresses; dwTemp++)
			{
				DPTEST_TRACE(hLog, "Host address %u: %s", 2, dwTemp, pszURL);


				DPTEST_TRACE(hLog, "Building remote address %u from string URL.", 1, dwTemp);


				tr = pDP8AddressRemote->BuildFromURLA(pszURL);
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't build host's address %u from URL (\"%s\")!",
						2, dwTemp, pszURL);
					THROW_TESTRESULT;
				} // end if (couldn't build from URL)



				DPTEST_TRACE(hLog, "Getting remote address %u's hostname component.", 1, dwTemp);

				dwSize = 16 * sizeof (WCHAR);

				tr = pDP8AddressRemote->GetComponentByName(DPNA_KEY_HOSTNAME,
															wszIPCompareString,
															&dwSize,
															&dwDataType);
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't get host's address %u hostname component!",
						1, dwTemp);
					THROW_TESTRESULT;
				} // end if (couldn't get component by name)

				// Make sure it's the right size for IP address.
				if (dwSize > (16 * sizeof (WCHAR)))
				{
					DPTEST_FAIL(hLog, "Size of hostname component is unexpected (%u > %u)!",
						2, dwSize, (16 * sizeof (WCHAR)));
					SETTHROW_TESTRESULT(E_UNEXPECTED);
				} // end if (didn't use all of the buffer)

				if (dwDataType != DPNA_DATATYPE_STRING)
				{
					DPTEST_FAIL(hLog, "Data type for hostname component is unexpected (%u != %u)!",
						2, dwDataType, DPNA_DATATYPE_STRING);
					SETTHROW_TESTRESULT(ERROR_NO_MATCH);
				} // end if (didn't use all of the buffer)


				// See if we found the IP address we want to use when
				// connecting.  If so, we're done.
				if (StringCmpAToU(hLog, wszIPCompareString, pEnumTestParam->szServerIPAddr, TRUE))
				{
					DPTEST_TRACE(hLog, "Found IP address \"%s\", using host's address %u (\"%s\").",
						3, pEnumTestParam->szServerIPAddr, dwTemp, pszURL);
					break;
				} // end if (found IP address)


				pszURL += strlen(pszURL) + 1;
			} // end for (each host address)

			// If we didn't find the address to use, we have to bail.
			if (dwTemp >= dwNumAddresses)
			{
				DPTEST_FAIL(hLog, "Couldn't find the correct host address to use!", 0);
				SETTHROW_TESTRESULT(ERROR_NOT_FOUND);
			} // end if (didn't find address)

			// We've determined what the host's real address should be.  However, we
			// don't use that for EnumHosts when broadcasting, so start from scratch.

			if(pEnumTestParam->fBroadcast)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Clearing remote (host's) address object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8AddressRemote->Clear();
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't clear remote (host's) address object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't clear)

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Setting remote (host's) address object's SP");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8AddressRemote->SetSP(&CLSID_DP8SP_TCPIP);

				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't set SP for remote (host's) address object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't set SP)

			} // end if (broadcasting)
			


			// At this point, the remote address object has the correct host
			// address we should use when enumerating.


			CREATEEVENT_OR_THROW(hEnumDoneEvent, NULL, FALSE, FALSE, NULL);

			if (pEnumTestParam->fClientServer)
			{
				tr = pDP8Client->DP8C_EnumHosts(&dpnad,
												pDP8AddressRemote,
												pDP8AddressLocal,
												context.pvUserData,
												context.dwUserDataSize,
												pEnumTestParam->dwEnumCount,
												pEnumTestParam->dwRetryInterval,
												pEnumTestParam->dwTimeOut,
												hEnumDoneEvent,
												&dpnhEnumHosts,
												0);
			} // end if (client/server)
			else
			{
				tr = pDP8Peer->DP8P_EnumHosts(&dpnad,
											pDP8AddressRemote,
											pDP8AddressLocal,
											context.pvUserData,
											context.dwUserDataSize,
											pEnumTestParam->dwEnumCount,
											pEnumTestParam->dwRetryInterval,
											pEnumTestParam->dwTimeOut,
											hEnumDoneEvent,
											&dpnhEnumHosts,
											0);
			} // end else (peer-to-peer)

			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog, "Couldn't start enumerating hosts!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't enumhosts)



			pDP8AddressLocal->Release();
			pDP8AddressLocal = NULL;

			pDP8AddressRemote->Release();
			pDP8AddressRemote = NULL;



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for enumeration to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			dwWaitResult = WaitForSingleObject(hEnumDoneEvent, ENUM_HOSTS_TIMEOUT);
			if((dwWaitResult != WAIT_OBJECT_0) && (dwWaitResult != WAIT_TIMEOUT))
			{
				DPTEST_FAIL(hLog, "Failure waiting for enumeration: %u", 1, dwWaitResult);
				THROW_SYSTEMRESULT;
			}
												
			sr = (HRESULT) S_OK;

			// If it timed out, it's time to cancel the enumeration.
			if(dwWaitResult == WAIT_TIMEOUT)
			{
				// Cancel the enumeration.  Note that we're doing it.
				// NOTE: There's a window where the enum could finish on its own
				// just before we go to cancel it.
				context.fCancelled = TRUE;

				if (pEnumTestParam->fClientServer)
				{
					tr = pDP8Client->DP8C_CancelAsyncOperation(dpnhEnumHosts, 0);
				} // end if (client/server)
				else
				{
					tr = pDP8Peer->DP8P_CancelAsyncOperation(dpnhEnumHosts, 0);
				} // end else (peer-to-peer)

				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't cancel enumeration!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't cancel enumeration)


				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for cancelled enumeration to complete");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				
				if(WaitForSingleObject(hEnumDoneEvent, ENUM_HOSTS_TIMEOUT) != WAIT_OBJECT_0)
				{
					DPTEST_FAIL(hLog, "Canceled enumeration didn't complete within %u seconds", 1, ENUM_HOSTS_TIMEOUT / 1000);
					THROW_SYSTEMRESULT;
				}
				
				sr = (HRESULT) S_OK;

			} // end if (wait timed out)
			



			CloseHandle(hEnumDoneEvent);
			hEnumDoneEvent = NULL;


			// Make sure we saw some responses come in if the host wasn't rejecting
			// our queries.
			if (! pEnumTestParam->fReject)
			{
				if (context.dwNumMsgs == 0)
				{
					DPTEST_FAIL(hLog, "Didn't receive any enum responses!", 0);
					SETTHROW_TESTRESULT(ERROR_NO_DATA);
				} // end if (no responses)
//#pragma TODO(vanceo, "Retrieve SP caps to determine 0 (default) num attempts")
//#pragma WAITFORDEVFIX(vanceo, ?, ?, "EnumCount still acts like RetryCount")
				/*
				else if ((pEnumTestParam->dwEnumCount > 0) &&
						(context.dwNumMsgs > pEnumTestParam->dwEnumCount))
				*/
				else if ((pEnumTestParam->dwEnumCount > 0) &&
						(context.dwNumMsgs > (pEnumTestParam->dwEnumCount + 1)))
				{
					DPTEST_FAIL(hLog, "Received too many enum responses (%u > %u)!",
						2, context.dwNumMsgs, pEnumTestParam->dwEnumCount);
					SETTHROW_TESTRESULT(E_FAIL);
				} // end else if (too many responses)
				else
				{
					DPTEST_TRACE(hLog, "Received %u enum responses.", 1, context.dwNumMsgs);
				} // end else (valid num responses)
			} // end if (not rejecting)
		} // end else (not tester 0)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for other tester to finish enumeration phase");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Implement this using Netsync
		if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, ENUMSDONE_EVENT_NAME, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, ENUMSDONE_EVENT_NAME);
			SETTHROW_TESTRESULT(E_ABORT);
		}

		// Make sure we saw some queries and all our buffers got returned.
		if(pEnumTestParam->fHostTest)
		{
			if (context.dwNumMsgs == 0)
			{
				DPTEST_FAIL(hLog, "Didn't receive any enum queries!", 0);
				SETTHROW_TESTRESULT(ERROR_NO_DATA);
			} // end if (no queries)
//#pragma TODO(vanceo, "Retrieve SP caps to determine 0 (default) num attempts")
//#pragma WAITFORDEVFIX(vanceo, ?, ?, "EnumCount still acts like RetryCount")
			/*
			else if ((pEnumTestParam->dwEnumCount > 0) &&
					(context.dwNumMsgs > pEnumTestParam->dwEnumCount))
			*/
			else if ((pEnumTestParam->dwEnumCount > 0) &&
					(context.dwNumMsgs > (pEnumTestParam->dwEnumCount + 1)))
			{
				DPTEST_FAIL(hLog, "Received too many enum queries (%u > %u)!",
					2, context.dwNumMsgs, pEnumTestParam->dwEnumCount);
				SETTHROW_TESTRESULT(E_FAIL);
			} // end else if (too many queries)
			else
			{
				DPTEST_TRACE(hLog, "Received %u enum queries.", 1, context.dwNumMsgs);
			} // end else (valid num queries)


			if(context.pvUserData != NULL)
			{
				DWORD dwCurrentRetry = 0;
				// This is somewhat murky, since we don't know how many enums are
				// coming in, so we don't know how many responses went out.
				//
				// We also don't know when the queries will stop trickling in, so
				// theoretically we could still be receiving the responses.  Thus,
				// even though the outstanding count could hit 0 now, it might pop
				// back up if another query came in.  To solve that, we're going to
				// signal the callback to not give any more data to DPlay if they do
				// come in, and we can just wait til the count hits zero.
				context.fDontRespondWithData = TRUE;

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for response buffers to be returned");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				// TODO - there needs to be a time limit established
				while ((context.lNumOutstandingResponseData != 0) && (dwCurrentRetry < 5))
				{
					DPTEST_FAIL(hLog, "Waiting for %i response buffers to be returned.",
						1, context.lNumOutstandingResponseData);

					Sleep(1000);
					++dwCurrentRetry;
				} // end while (still more buffers outstanding)

				if(dwCurrentRetry >= 5)
				{
					DPTEST_FAIL(hLog, "Outstanding response data wasn't returned within %u seconds", 1, dwCurrentRetry);
					SETTHROW_TESTRESULT(E_FAIL);
				}

			} // end if (sent data)
			
		} // end if (tester 0)
		




		if(context.pvUserData != NULL)
		{
			if(pEnumTestParam->fHostTest)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Notifiying other tester that buffers are returned");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			} // end if (tester 0)
			else
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for host's reply buffers to be returned");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			} // end else (not tester 0)
			
			
			if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, BUFRETURNED_EVENT_NAME, NULL, 0, NULL, 0))
			{
				DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, BUFRETURNED_EVENT_NAME);
				SETTHROW_TESTRESULT(E_ABORT);
			}
		
		} // end if (sent data)
		




		if(pEnumTestParam->fClientServer)
		{
			if(pEnumTestParam->fHostTest)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Closing object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Server->DP8S_Close(0);
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Closing failed!", 0);
					THROW_TESTRESULT;
				} // end if (close failed)

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Releasing DirectPlay8Server object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Server->Release();
				if (tr != S_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't release object)

				delete (pDP8Server);
				pDP8Server = NULL;

			} // end if (tester 0)
			else
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Closing object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Client->DP8C_Close(0);
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Closing failed!", 0);
					THROW_TESTRESULT;
				} // end if (close failed)

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Releasing DirectPlay8Client object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Client->Release();
				if (tr != S_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Client object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't release object)

				delete (pDP8Client);
				pDP8Client = NULL;
			
			} // end else (not tester 0)
			
		} // end if (client/server)
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Closing object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Peer->DP8P_Close(0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Closing failed!", 0);
				THROW_TESTRESULT;
			} // end if (close failed)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Peer object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Peer->Release();
			if (tr != S_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't release object)

			delete (pDP8Peer);
			pDP8Peer = NULL;
			
		} // end else (peer-to-peer)
		



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8Server != NULL)
	{
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)

	if (pDP8Client != NULL)
	{
		delete (pDP8Client);
		pDP8Client = NULL;
	} // end if (have client object)

	if (paDP8HostAddresses != NULL)
	{
		for(dwTemp = 0; dwTemp < dwNumAddresses; dwTemp++)
		{
			SAFE_RELEASE(paDP8HostAddresses[dwTemp]);
		} // end for (each address)

		MemFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	} // end if (have array of host addresses)

	SAFE_LOCALFREE(pvSyncData);
	SAFE_LOCALFREE(dpnad.pvApplicationReservedData);
	SAFE_LOCALFREE(context.pvUserData);
	SAFE_LOCALFREE(pHostAddressesSyncData);
	SAFE_RELEASE(pDP8AddressLocal);
	SAFE_RELEASE(pDP8AddressRemote);
	SAFE_CLOSEHANDLE(hEnumDoneEvent);

	
	return (sr);
} // EnumsExec_SimpleAll
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


#undef DEBUG_SECTION
#define DEBUG_SECTION	"EnumsExec_Validate()"
//==================================================================================
// EnumsExec_Validate
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.4.3 - Procedural validation enumeration types test
//
// Arguments:
//	HANDLE				hLog				Handle to the logging subsystem
//	DP_ENUMTESTPARAM	*pEnumTestParam		Pointer to parameters for this test
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
HRESULT EnumsExec_Validate(HANDLE hLog, DP_ENUMTESTPARAM *pEnumTestParam)
{
	CTNSystemResult						sr;
	CTNTestResult						tr;
	ENUMSVALIDATECONTEXT				context;
	DPN_APPLICATION_DESC				dpnadCS;
	DPN_APPLICATION_DESC				dpnadPeer;
	PVOID								pvSyncData = NULL;
	DWORD								dwSyncDataSize;
	PWRAPDP8PEER						pDP8PeerHost = NULL;
	PWRAPDP8PEER						pDP8PeerClient = NULL;
	PWRAPDP8SERVER						pDP8Server = NULL;
	PWRAPDP8CLIENT						pDP8Client = NULL;
	PDIRECTPLAY8ADDRESS					pDP8AddressLocal = NULL;
	PDIRECTPLAY8ADDRESS					pDP8AddressRemoteCS = NULL;
	PDIRECTPLAY8ADDRESS					pDP8AddressRemotePeer = NULL;
	HANDLE								hEnumDoneEvent = NULL;
	DPNHANDLE							dpnhEnumHosts = NULL;
	PHOSTADDRESSESCSANDPEERSYNCDATA		pHostAddressesCSAndPeerSyncData = NULL;
	PDIRECTPLAY8ADDRESS*				paDP8HostAddresses = NULL;
	DWORD								dwNumAddressesCS = 0;
	DWORD								dwNumAddressesPeer = 0;
	DWORD								dwTemp;
	DWORD								dwURLsBufferSize = 0;
	DWORD								dwRemainingBufferSize;
	char*								pszURL;
	DWORD								dwSize;
	DWORD								dwDataType;
	WCHAR								wszIPCompareString[16]; // max IP string size + NULL termination

	ZeroMemory(&context, sizeof (ENUMSVALIDATECONTEXT));

	BEGIN_TESTCASE
	{
		context.hLog = hLog;
		context.pEnumTestParam = pEnumTestParam;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Client = new CWrapDP8Client(hLog);
		if (pDP8Client == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)


		tr = pDP8Client->CoCreate();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "CoCreating DP8Client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (function failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Peer object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8PeerClient = new CWrapDP8Peer(hLog);
		if (pDP8PeerClient == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)


		tr = pDP8PeerClient->CoCreate();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "CoCreating DP8Peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (function failed)


		if(pEnumTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating DirectPlay8Server object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			pDP8Server = new CWrapDP8Server(hLog);
			if (pDP8Server == NULL)
			{
				SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
			} // end if (couldn't allocate object)


			tr = pDP8Server->CoCreate();
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "CoCreating DP8Server object failed!", 0);
				THROW_TESTRESULT;
			} // end if (function failed)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating secondary DirectPlay8Peer object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			pDP8PeerHost = new CWrapDP8Peer(hLog);
			if (pDP8PeerClient == NULL)
			{
				SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
			} // end if (couldn't allocate object)


			tr = pDP8PeerHost->CoCreate();
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "CoCreating DP8Peer object failed!", 0);
				THROW_TESTRESULT;
			} // end if (function failed)

		} // end if (tester 0)
		





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("CoCreating local address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8AddressLocal, NULL);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (couldn't CoCreate object)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting local address object's SP to TCP/IP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressLocal->SetSP(&CLSID_DP8SP_TCPIP);

		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't set SP for address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing objects");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_Initialize(&context,
										EnumsValidateDPNMessageHandler,
										0);
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't initialize client interface!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)


		tr = pDP8PeerClient->DP8P_Initialize(&context,
											EnumsValidateDPNMessageHandler,
											0);
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't initialize peer client interface!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)


		if (pEnumTestParam->fHostTest)
		{
			tr = pDP8Server->DP8S_Initialize(&context,
											EnumsValidateDPNMessageHandler,
											0);
			if (tr != S_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't initialize server interface!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't initialize)


			tr = pDP8PeerHost->DP8P_Initialize(&context,
												EnumsValidateDPNMessageHandler,
												0);
			if (tr != S_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't initialize peer host interface!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't initialize)
		} // end if (tester 0)



		ZeroMemory(&dpnadCS, sizeof (DPN_APPLICATION_DESC));
		dpnadCS.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpnadCS.dwFlags = 0;
		dpnadCS.dwFlags = (pEnumTestParam->fHostTest) ? DPNSESSION_CLIENT_SERVER : 0;
		//dpnadCS.guidInstance = GUID_NULL;
		dpnadCS.guidApplication = GUID_ENUMS_VALIDATE;
		//dpnadCS.dwMaxPlayers = 0;
		//dpnadCS.dwCurrentPlayers = 0;
		dpnadCS.pwszSessionName = (pEnumTestParam->fHostTest) ? ENUM_SESSION_NAME : NULL;
		//dpnadCS.pwszPassword = NULL;
		//dpnadCS.pvReservedData = NULL;
		//dpnadCS.dwReservedDataSize = 0;
		//dpnadCS.pvApplicationReservedData = NULL;
		//dpnad.CSpvApplicationReservedData = 0;



		ZeroMemory(&dpnadPeer, sizeof (DPN_APPLICATION_DESC));
		dpnadPeer.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpnadPeer.dwFlags = 0;
		dpnadPeer.dwFlags = 0;
		//dpnadPeer.guidInstance = GUID_NULL;
		dpnadPeer.guidApplication = GUID_ENUMS_VALIDATE;
		//dpnadPeer.dwMaxPlayers = 0;
		//dpnadPeer.dwCurrentPlayers = 0;
		dpnadPeer.pwszSessionName = (pEnumTestParam->fHostTest) ? ENUM_SESSION_NAME : NULL;
		//dpnadPeer.pwszPassword = NULL;
		//dpnadPeer.pvReservedData = NULL;
		//dpnadPeer.dwReservedDataSize = 0;
		//dpnadPeer.pvApplicationReservedData = NULL;
		//dpnadPeer.pvApplicationReservedData = 0;




		// Tester 0 should start hosting the sessions, tester 1 should wait until
		// they are up.
		if(pEnumTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Hosting session client/server session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Server->DP8S_Host(&dpnadCS, &pDP8AddressLocal, 1, NULL, NULL,
										NULL, 0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't start hosting client/server session!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't host session)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Hosting session peer-to-peer session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerHost->DP8P_Host(&dpnadPeer, &pDP8AddressLocal, 1, NULL, NULL,
										NULL, 0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't start hosting peer session!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't host session)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Getting number of host addresses for client/server");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Server->DP8S_GetLocalHostAddresses(NULL, &dwNumAddressesCS, 0);
			if (tr != DPNERR_BUFFERTOOSMALL)
			{
				DPTEST_FAIL(hLog, "Couldn't get number of client/server host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't get local host addresses)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Getting number of host addresses for peer-to-peer");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerHost->DP8P_GetLocalHostAddresses(NULL, &dwNumAddressesPeer, 0);
			if (tr != DPNERR_BUFFERTOOSMALL)
			{
				DPTEST_FAIL(hLog, "Couldn't get number of peer-to-peer host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't get local host addresses)


			DPTEST_TRACE(hLog, "We are hosting on %u C/S addresses and %u peer addresses.",
				2, dwNumAddressesCS, dwNumAddressesPeer); 


			LOCALALLOC_OR_THROW( PDIRECTPLAY8ADDRESS*, paDP8HostAddresses,
								(dwNumAddressesCS + dwNumAddressesPeer) * sizeof (PDIRECTPLAY8ADDRESS));



			tr = pDP8Server->DP8S_GetLocalHostAddresses(paDP8HostAddresses,
														&dwNumAddressesCS,
														0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't get c/s host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't local host addresses)


			tr = pDP8PeerHost->DP8P_GetLocalHostAddresses(paDP8HostAddresses + dwNumAddressesCS,
														&dwNumAddressesPeer,
														0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't get peer host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't local host addresses)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting local c/s client to local session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Client->DP8C_Connect(&dpnadCS,
										paDP8HostAddresses[0],
										pDP8AddressLocal,
										NULL,
										NULL,
										NULL,
										0,
										NULL,
										NULL,
										DPNCONNECT_SYNC);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't connect!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting local peer client to local session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerClient->DP8P_Connect(&dpnadPeer,
											paDP8HostAddresses[dwNumAddressesCS],
											pDP8AddressLocal,
											NULL,
											NULL,
											NULL,
											0,
											NULL,
											NULL,
											NULL,
											DPNCONNECT_SYNC);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't connect!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)


			pDP8AddressLocal->Release();
			pDP8AddressLocal = NULL;

			// Get the total size of all the addresses.
			for(dwTemp = 0; dwTemp < (dwNumAddressesCS + dwNumAddressesPeer); dwTemp++)
			{
				dwSize = 0;
				tr = paDP8HostAddresses[dwTemp]->GetURLA(NULL, &dwSize);
				if (tr != DPNERR_BUFFERTOOSMALL)
				{
					DPTEST_FAIL(hLog, "Couldn't get host address %u's URL size!", 1, dwTemp);
					THROW_TESTRESULT;
				} // end if (not buffer too small)
				
				dwURLsBufferSize += dwSize;
			} // end for (each address)

			// Allocate a buffer for all the URLs.
			LOCALALLOC_OR_THROW( PHOSTADDRESSESCSANDPEERSYNCDATA,
								pHostAddressesCSAndPeerSyncData,
								sizeof (HOSTADDRESSESCSANDPEERSYNCDATA) + dwURLsBufferSize);

			pHostAddressesCSAndPeerSyncData->dwNumAddressesCS = dwNumAddressesCS;
			pHostAddressesCSAndPeerSyncData->dwNumAddressesPeer = dwNumAddressesPeer;
			pszURL = (char*) (pHostAddressesCSAndPeerSyncData + 1);
			dwRemainingBufferSize = dwURLsBufferSize;


			// Actually fill in all the URLs and release the objects (since we're
			// done with them).
			for(dwTemp = 0; dwTemp < (dwNumAddressesCS + dwNumAddressesPeer); dwTemp++)
			{
				dwSize = dwRemainingBufferSize;
				tr = paDP8HostAddresses[dwTemp]->GetURLA(pszURL,
														&dwSize);
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't get host address %u's URL!", 1, dwTemp);
					THROW_TESTRESULT;
				} // end if (failed)


				DPTEST_TRACE(hLog, "Address %u: \"%s\"",
					2, dwTemp, pszURL);

				dwRemainingBufferSize -= dwSize;
				pszURL += dwSize;

				
				// We're done with this address object.
				paDP8HostAddresses[dwTemp]->Release();
				paDP8HostAddresses[dwTemp] = NULL;
			} // end for (each address)

			// We're done with the address array.
			MemFree(paDP8HostAddresses);
			paDP8HostAddresses = NULL;


			// Make sure we used all of the buffer.
			if (dwRemainingBufferSize != 0)
			{
				DPTEST_FAIL(hLog, "Didn't use all of or overran the allocated buffer (didn't use %i bytes)!",
					1, ((int) dwRemainingBufferSize));
				SETTHROW_TESTRESULT(E_FAIL);
			} // end if (didn't use all of the buffer)





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Sending session addresses to connector");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, VALHOSTADDR_EVENT_NAME,
				(BYTE *) pHostAddressesCSAndPeerSyncData, (sizeof(HOSTADDRESSESCSANDPEERSYNCDATA) + dwURLsBufferSize), NULL, 0))
			{
				DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, VALHOSTADDR_EVENT_NAME);
				SETTHROW_TESTRESULT(E_ABORT);
			}

			MemFree(pHostAddressesCSAndPeerSyncData);
			pHostAddressesCSAndPeerSyncData = NULL;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connector to finish connecting to wrong interface type");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end if (tester 0)
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Receiving addresses of host");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			LOCALALLOC_OR_THROW( LPVOID, pvSyncData, MAX_SYNC_DATASIZE);

			dwSyncDataSize = MAX_SYNC_DATASIZE;
			if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, VALHOSTADDR_EVENT_NAME,
				NULL, 0, (PBYTE) pvSyncData, &dwSyncDataSize))
			{
				DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, VALHOSTADDR_EVENT_NAME);
				SETTHROW_TESTRESULT(E_ABORT);
			}

			if(dwSyncDataSize < (DWORD) (sizeof (HOSTADDRESSESCSANDPEERSYNCDATA) + ((strlen(DPNA_HEADER_A) + 1) * 2)))
			{
				DPTEST_FAIL(hLog, "Received sync data is below minimum amount (%u < %u)", 2,
					dwSyncDataSize, (sizeof (HOSTADDRESSESCSANDPEERSYNCDATA) + ((strlen(DPNA_HEADER_A) + 1) * 2)));
				SETTHROW_TESTRESULT(E_ABORT);
			}

			dwNumAddressesCS = ((PHOSTADDRESSESCSANDPEERSYNCDATA) pvSyncData)->dwNumAddressesCS;
			dwNumAddressesPeer = ((PHOSTADDRESSESCSANDPEERSYNCDATA) pvSyncData)->dwNumAddressesPeer;

			// Start with the first address
			pszURL = (char*) (((PBYTE) pvSyncData) + sizeof (HOSTADDRESSESCSANDPEERSYNCDATA));

			DPTEST_TRACE(hLog, "Tester 0 (host) has %u c/s and %u peer connectable addresses.",
				2, dwNumAddressesCS, dwNumAddressesPeer);



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("CoCreating remote (host's) client/server address object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8AddressRemoteCS, NULL);
			if (sr != S_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't CoCreate remote (host's) client/server address object!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't CoCreate object)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("CoCreating remote (host's) peer-to-peer address object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8AddressRemotePeer, NULL);
			if (sr != S_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't CoCreate remote (host's) peer-to-peer address object!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't CoCreate object)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Finding appropriate c/s address");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			for(dwTemp = 0; dwTemp < dwNumAddressesCS; dwTemp++)
			{
				DPTEST_TRACE(hLog, "Host c/s address %u: %s", 2, dwTemp, pszURL);


				DPTEST_TRACE(hLog, "Building remote c/s address %u from string URL.", 1, dwTemp);

				tr = pDP8AddressRemoteCS->BuildFromURLA(pszURL);
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't build host's c/s address %u from URL (\"%s\")!",
						2, dwTemp, pszURL);
					THROW_TESTRESULT;
				} // end if (couldn't build from URL)



				DPTEST_TRACE(hLog, "Getting remote c/s address %u's hostname component.", 1, dwTemp);

				dwSize = 16 * sizeof (WCHAR);

				tr = pDP8AddressRemoteCS->GetComponentByName(DPNA_KEY_HOSTNAME,
															wszIPCompareString,
															&dwSize,
															&dwDataType);
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't get host's c/s address %u hostname component!",
						1, dwTemp);
					THROW_TESTRESULT;
				} // end if (couldn't get component by name)

				// Make sure it's the right size for IP address.
				if (dwSize > (16 * sizeof (WCHAR)))
				{
					DPTEST_FAIL(hLog, "Size of hostname component is unexpected (%u > %u)!",
						2, dwSize, (16 * sizeof (WCHAR)));
					SETTHROW_TESTRESULT(E_UNEXPECTED);
				} // end if (didn't use all of the buffer)

				if (dwDataType != DPNA_DATATYPE_STRING)
				{
					DPTEST_FAIL(hLog, "Data type for hostname component is unexpected (%u != %u)!",
						2, dwDataType, DPNA_DATATYPE_STRING);
					SETTHROW_TESTRESULT(ERROR_NO_MATCH);
				} // end if (didn't use all of the buffer)


				// See if we found the IP address we want to use when
				// connecting.  If so, we're done.
				if (StringCmpAToU(hLog, wszIPCompareString, pEnumTestParam->szServerIPAddr, TRUE))
				{
					DPTEST_TRACE(hLog, "Found IP address \"%s\", using host's c/s address %u (\"%s\").",
						3, pEnumTestParam->szServerIPAddr, dwTemp, pszURL);
					break;
				} // end if (found IP address)


				pszURL += strlen(pszURL) + 1;
			} // end for (each host address)

			// If we didn't find the address to use, we have to bail.
			if (dwTemp >= dwNumAddressesCS)
			{
				DPTEST_FAIL(hLog, "Couldn't find the correct host c/s address to use!", 0);
				SETTHROW_TESTRESULT(ERROR_NOT_FOUND);
			} // end if (didn't find address)



			// Move to the start of the peer addresses.
			while (dwTemp < dwNumAddressesCS)
			{
				pszURL += strlen(pszURL) + 1;
				dwTemp++;
			} // end for (each host address)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Finding appropriate peer address");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			for(dwTemp = 0; dwTemp < dwNumAddressesPeer; dwTemp++)
			{
				DPTEST_TRACE(hLog, "Host peer address %u: %s", 2, dwTemp, pszURL);


				DPTEST_TRACE(hLog, "Building remote peer address %u from string URL.", 1, dwTemp);

				tr = pDP8AddressRemotePeer->BuildFromURLA(pszURL);
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't build host's peer address %u from URL (\"%s\")!",
						2, dwTemp, pszURL);
					THROW_TESTRESULT;
				} // end if (couldn't build from URL)



				DPTEST_TRACE(hLog, "Getting remote peer address %u's hostname component.", 1, dwTemp);

				dwSize = 16 * sizeof (WCHAR);

				tr = pDP8AddressRemotePeer->GetComponentByName(DPNA_KEY_HOSTNAME,
																wszIPCompareString,
																&dwSize,
																&dwDataType);
				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Couldn't get host's peer address %u hostname component!",
						1, dwTemp);
					THROW_TESTRESULT;
				} // end if (couldn't get component by name)

				// Make sure it's the right size for IP address.
				if (dwSize > (16 * sizeof (WCHAR)))
				{
					DPTEST_FAIL(hLog, "Size of hostname component is unexpected (%u > %u)!",
						2, dwSize, (16 * sizeof (WCHAR)));
					SETTHROW_TESTRESULT(E_UNEXPECTED);
				} // end if (didn't use all of the buffer)

				if (dwDataType != DPNA_DATATYPE_STRING)
				{
					DPTEST_FAIL(hLog, "Data type for hostname component is unexpected (%u != %u)!",
						2, dwDataType, DPNA_DATATYPE_STRING);
					SETTHROW_TESTRESULT(ERROR_NO_MATCH);
				} // end if (didn't use all of the buffer)


				// See if we found the IP address we want to use when
				// connecting.  If so, we're done.
				if (StringCmpAToU(hLog, wszIPCompareString, pEnumTestParam->szServerIPAddr, TRUE))
				{
					DPTEST_TRACE(hLog, "Found IP address \"%s\", using host's peer address %u (\"%s\").",
						3, pEnumTestParam->szServerIPAddr, dwTemp, pszURL);
					break;
				} // end if (found IP address)


				pszURL += strlen(pszURL) + 1;
			} // end for (each host address)

			// If we didn't find the address to use, we have to bail.
			if (dwTemp >= dwNumAddressesPeer)
			{
				DPTEST_FAIL(hLog, "Couldn't find the correct host peer address to use!", 0);
				SETTHROW_TESTRESULT(ERROR_NOT_FOUND);
			} // end if (didn't find address)

			// At this point, the remote address objects have the correct host
			// addresses we should use when enumerating.

			CREATEEVENT_OR_THROW(hEnumDoneEvent, NULL, FALSE, FALSE, NULL);


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Enumerating peer-to-peer session using client/server interface");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Client->DP8C_EnumHosts(&dpnadPeer,
											pDP8AddressRemotePeer,
											pDP8AddressLocal,
											NULL,
											0,
											0, // default EnumCount
											0, // default RetryInterval
											0, // default TimeOut
											hEnumDoneEvent,
											&dpnhEnumHosts,
											0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog, "Couldn't start enumerating hosts!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't enumhosts)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for enumeration to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			if(WaitForSingleObject(hEnumDoneEvent, ENUM_HOSTS_TIMEOUT) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Enumeration didn't complete within %u seconds", 1, ENUM_HOSTS_TIMEOUT / 1000);
				THROW_SYSTEMRESULT;
			}
				
			sr = (HRESULT) S_OK;

		} // end else (not tester 0)
		


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for other tester to finish c/s to p-p enumeration phase");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, CSTOPPDONE_EVENT_NAME, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, CSTOPPDONE_EVENT_NAME);
			SETTHROW_TESTRESULT(E_ABORT);
		}

		// Make sure we saw some queries/responses come in.
		if (context.dwNumMsgs == 0)
		{
			DPTEST_FAIL(hLog, "Didn't receive any enum queries/responses!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (no queries/responses)
#pragma TODO(vanceo, "Retrieve SP caps to determine 0 (default) num attempts")

//		else if (context.dwNumMsgs > pInput->dwEnumCount)
//		{
//			DPTEST_FAIL(hLog, "Received too many enum queries/responses (%u > %u)!",
//				2, context.dwNumMsgs, pInput->dwEnumCount);
//			SETTHROW_TESTRESULT(E_FAIL);
//		} // end else if (too many queries/responses)
		else
		{
			DPTEST_TRACE(hLog, "Received %u enum queries/responses.", 1, context.dwNumMsgs);
		} // end else (valid num queries/responses)

		// Reset the count.
		context.dwNumMsgs = 0;

		if(!pEnumTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Enumerating client/server session using peer-to-peer interface");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerClient->DP8P_EnumHosts(&dpnadCS,
												pDP8AddressRemoteCS,
												pDP8AddressLocal,
												NULL,
												0,
												0, // default EnumCount
												0, // default RetryInterval
												0, // default TimeOut
												hEnumDoneEvent,
												&dpnhEnumHosts,
												0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog, "Couldn't start enumerating hosts!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't enumhosts)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for enumeration to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(hEnumDoneEvent, ENUM_HOSTS_TIMEOUT) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Enumeration didn't complete within %u seconds", 1, ENUM_HOSTS_TIMEOUT / 1000);
				THROW_SYSTEMRESULT;
			}
				
			sr = (HRESULT) S_OK;

		} // end else (not tester 0)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for other tester to finish p-p to c/s enumeration phase");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, PPTOCSDONE_EVENT_NAME, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, PPTOCSDONE_EVENT_NAME);
			SETTHROW_TESTRESULT(E_ABORT);
		}


		// Make sure we saw some queries/responses come in.
		if (context.dwNumMsgs == 0)
		{
			DPTEST_FAIL(hLog, "Didn't receive any enum queries/responses!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (no queries/responses)
#pragma TODO(vanceo, "Retrieve SP caps to determine 0 (default) num attempts")
//		else if (context.dwNumMsgs > pInput->dwEnumCount)
//		{
//			DPTEST_FAIL(hLog, "Received too many enum queries/responses (%u > %u)!",
//				2, context.dwNumMsgs, pInput->dwEnumCount);
//			SETTHROW_TESTRESULT(E_FAIL);
//		} // end else if (too many queries/responses)
		else
		{
			DPTEST_TRACE(hLog, "Received %u enum queries/responses.", 1, context.dwNumMsgs);
		} // end else (valid num queries/responses)

		// Reset the count.
		context.dwNumMsgs = 0;
		

		if(pEnumTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Capping the number of client/server clients");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadCS.dwMaxPlayers = 2;

			tr = pDP8Server->DP8S_SetApplicationDesc(&dpnadCS, 0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't set app desc!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't set app desc)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Notifying enumerator of c/s player limit change");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end if (tester 0)
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for host to limit number of c/s players");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end else (not tester 0)
		


		if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, CSCAPPED_EVENT_NAME, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, CSCAPPED_EVENT_NAME);
			SETTHROW_TESTRESULT(E_ABORT);
		}

		if(!pEnumTestParam->fHostTest)
		{
			context.dwExpectedMaxPlayers = 2;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Enumerating maxxed out client/server session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Client->DP8C_EnumHosts(&dpnadCS,
											pDP8AddressRemoteCS,
											pDP8AddressLocal,
											NULL,
											0,
											0, // default EnumCount
											0, // default RetryInterval
											0, // default TimeOut
											hEnumDoneEvent,
											&dpnhEnumHosts,
											0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog, "Couldn't start enumerating hosts!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't enumhosts)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for enumeration to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(hEnumDoneEvent, ENUM_HOSTS_TIMEOUT) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Enumeration didn't complete within %u seconds", 1, ENUM_HOSTS_TIMEOUT / 1000);
				THROW_SYSTEMRESULT;
			}

			sr = (HRESULT) S_OK;

		} // end else (not tester 0)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for other tester to finish c/s max players enumeration phase");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, CSMAXPLAYER_EVENT_NAME, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, CSMAXPLAYER_EVENT_NAME);
			SETTHROW_TESTRESULT(E_ABORT);
		}

		// Make sure we saw some queries/responses come in.
		if (context.dwNumMsgs == 0)
		{
			DPTEST_FAIL(hLog, "Didn't receive any enum queries/responses!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (no queries/responses)
#pragma TODO(vanceo, "Retrieve SP caps to determine 0 (default) num attempts")
//		else if (context.dwNumMsgs > pInput->dwEnumCount)
//		{
//			DPTEST_FAIL(hLog, "Received too many enum queries/responses (%u > %u)!",
//				2, context.dwNumMsgs, pInput->dwEnumCount);
//			SETTHROW_TESTRESULT(E_FAIL);
//		} // end else if (too many queries/responses)
		else
		{
			DPTEST_TRACE(hLog, "Received %u enum queries/responses.", 1, context.dwNumMsgs);
		} // end else (valid num queries/responses)

		// Reset the count.
		context.dwNumMsgs = 0;

		if(pEnumTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Capping the number of peer-to-peer players");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadPeer.dwMaxPlayers = 2;

			tr = pDP8PeerHost->DP8P_SetApplicationDesc(&dpnadPeer, 0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't set app desc!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't set app desc)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Notifying enumerator of p-p player limit change");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end if (tester 0)
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for host to limit number of p-p players");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end else (not tester 0)
		


		if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, PPCAPPED_EVENT_NAME, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, PPCAPPED_EVENT_NAME);
			SETTHROW_TESTRESULT(E_ABORT);
		}

		if(!pEnumTestParam->fHostTest)
		{
			//context.dwExpectedMaxPlayers = 2;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Enumerating maxxed out peer-to-peer session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerClient->DP8P_EnumHosts(&dpnadPeer,
												pDP8AddressRemotePeer,
												pDP8AddressLocal,
												NULL,
												0,
												0, // default EnumCount
												0, // default RetryInterval
												0, // default TimeOut
												hEnumDoneEvent,
												&dpnhEnumHosts,
												0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog, "Couldn't start enumerating hosts!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't enumhosts)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for enumeration to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(hEnumDoneEvent, ENUM_HOSTS_TIMEOUT) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Enumeration didn't complete within %u seconds", 1, ENUM_HOSTS_TIMEOUT / 1000);
				THROW_SYSTEMRESULT;
			}

			sr = (HRESULT) S_OK;

		} // end else (not tester 0)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for other tester to finish p-p max players enumeration phase");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, PPMAXPLAYER_EVENT_NAME, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, PPMAXPLAYER_EVENT_NAME);
			SETTHROW_TESTRESULT(E_ABORT);
		}

		// Make sure we saw some queries/responses come in.
		if (context.dwNumMsgs == 0)
		{
			DPTEST_FAIL(hLog, "Didn't receive any enum queries/responses!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (no queries/responses)
#pragma TODO(vanceo, "Retrieve SP caps to determine 0 (default) num attempts")
//		else if (context.dwNumMsgs > pInput->dwEnumCount)
//		{
//			DPTEST_FAIL(hLog, "Received too many enum queries/responses (%u > %u)!",
//				2, context.dwNumMsgs, pInput->dwEnumCount);
//			SETTHROW_TESTRESULT(E_FAIL);
//		} // end else if (too many queries/responses)
		else
		{
			DPTEST_TRACE(hLog, "Received %u enum queries/responses.", 1, context.dwNumMsgs);
		} // end else (valid num queries/responses)

		// Reset the count.
		context.dwNumMsgs = 0;


		if(pEnumTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Setting password on client/server session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadCS.dwFlags = DPNSESSION_CLIENT_SERVER | DPNSESSION_REQUIREPASSWORD;
			dpnadCS.dwMaxPlayers = 0;
			dpnadCS.pwszPassword = L"Some password, huh?";

			tr = pDP8Server->DP8S_SetApplicationDesc(&dpnadCS, 0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't set app desc!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't set app desc)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Notifying enumerator of c/s password change");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end if (tester 0)
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for host to set c/s password");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end else (not tester 0)
		


		if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, CSPW_EVENT_NAME, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, CSPW_EVENT_NAME);
			SETTHROW_TESTRESULT(E_ABORT);
		}

		if(!pEnumTestParam->fHostTest)
		{
			context.dwExpectedMaxPlayers = 0;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Enumerating password protected client/server session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Client->DP8C_EnumHosts(&dpnadCS,
											pDP8AddressRemoteCS,
											pDP8AddressLocal,
											NULL,
											0,
											0, // default EnumCount
											0, // default RetryInterval
											0, // default TimeOut
											hEnumDoneEvent,
											&dpnhEnumHosts,
											0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog, "Couldn't start enumerating hosts!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't enumhosts)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for enumeration to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(hEnumDoneEvent, ENUM_HOSTS_TIMEOUT) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Enumeration didn't complete within %u seconds", 1, ENUM_HOSTS_TIMEOUT / 1000);
				THROW_SYSTEMRESULT;
			}

			sr = (HRESULT) S_OK;

		} // end else (not tester 0)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for other tester to finish c/s password enumeration phase");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, CSPWDONE_EVENT_NAME, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, CSPWDONE_EVENT_NAME);
			SETTHROW_TESTRESULT(E_ABORT);
		}

		// Make sure we saw some queries/responses come in.
		if (context.dwNumMsgs == 0)
		{
			DPTEST_FAIL(hLog, "Didn't receive any enum queries/responses!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (no queries/responses)
#pragma TODO(vanceo, "Retrieve SP caps to determine 0 (default) num attempts")
//		else if (context.dwNumMsgs > pInput->dwEnumCount)
//		{
//			DPTEST_FAIL(hLog, "Received too many enum queries/responses (%u > %u)!",
//				2, context.dwNumMsgs, pInput->dwEnumCount);
//			SETTHROW_TESTRESULT(E_FAIL);
//		} // end else if (too many queries/responses)
		else
		{
			DPTEST_TRACE(hLog, "Received %u enum queries/responses.", 1, context.dwNumMsgs);
		} // end else (valid num queries/responses)

		// Reset the count.
		context.dwNumMsgs = 0;

		if(pEnumTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Setting password on peer-to-peer session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadPeer.dwFlags = DPNSESSION_REQUIREPASSWORD;
			dpnadPeer.dwMaxPlayers = 0;
			dpnadPeer.pwszPassword = L"Some password, huh?";

			tr = pDP8PeerHost->DP8P_SetApplicationDesc(&dpnadPeer, 0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't set app desc!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't set app desc)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Notifying enumerator of p-p password");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end if (tester 0)
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for host to set p-p password");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end else (not tester 0)
		


		if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, PPPW_EVENT_NAME, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, PPPW_EVENT_NAME);
			SETTHROW_TESTRESULT(E_ABORT);
		}

		if(!pEnumTestParam->fHostTest)
		{
			//context.dwExpectedMaxPlayers = 0;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Enumerating password protected peer-to-peer session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerClient->DP8P_EnumHosts(&dpnadPeer,
												pDP8AddressRemotePeer,
												pDP8AddressLocal,
												NULL,
												0,
												0, // default EnumCount
												0, // default RetryInterval
												0, // default TimeOut
												hEnumDoneEvent,
												&dpnhEnumHosts,
												0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog, "Couldn't start enumerating hosts!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't enumhosts)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for enumeration to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(hEnumDoneEvent, ENUM_HOSTS_TIMEOUT) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Enumeration didn't complete within %u seconds", 1, ENUM_HOSTS_TIMEOUT / 1000);
				THROW_SYSTEMRESULT;
			}

			sr = (HRESULT) S_OK;

		} // end if (not tester 0)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for other tester to finish p-p password enumeration phase");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, PPPWDONE_EVENT_NAME, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, PPPWDONE_EVENT_NAME);
			SETTHROW_TESTRESULT(E_ABORT);
		}



		// Make sure we saw some queries/responses come in.
		if (context.dwNumMsgs == 0)
		{
			DPTEST_FAIL(hLog, "Didn't receive any enum queries/responses!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (no queries/responses)
#pragma TODO(vanceo, "Retrieve SP caps to determine 0 (default) num attempts")
//		else if (context.dwNumMsgs > pInput->dwEnumCount)
//		{
//			DPTEST_FAIL(hLog, "Received too many enum queries/responses (%u > %u)!",
//				2, context.dwNumMsgs, pInput->dwEnumCount);
//			SETTHROW_TESTRESULT(E_FAIL);
//		} // end else if (too many queries/responses)
		else
		{
			DPTEST_TRACE(hLog, "Received %u enum queries/responses.", 1, context.dwNumMsgs);
		} // end else (valid num queries/responses)

		// Reset the count.
		context.dwNumMsgs = 0;

		if(pEnumTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Retrieving c/s client address");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Get real player ID")
			tr = pDP8Server->DP8S_GetClientAddress((DPNID) 0x00300003,
												&pDP8AddressRemoteCS,
												0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Getting c/s client address failed!", 0);
				THROW_TESTRESULT;
			} // end if (getting client address failed)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Retrieving peer client address");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Get real player ID")
			tr = pDP8PeerHost->DP8P_GetPeerAddress((DPNID) 0x00300003,
													&pDP8AddressRemotePeer,
													0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Getting peer client address failed!", 0);
				THROW_TESTRESULT;
			} // end if (getting peer address failed)


			dwSize = 0;
			dwURLsBufferSize = 0;

			tr = pDP8AddressRemoteCS->GetURLA(NULL, &dwSize);
			if (tr != DPNERR_BUFFERTOOSMALL)
			{
				DPTEST_FAIL(hLog, "Couldn't get c/s client address' URL size!", 0);
				THROW_TESTRESULT;
			} // end if (not buffer too small)
			
			dwURLsBufferSize += dwSize;

			dwSize = 0;
			tr = pDP8AddressRemotePeer->GetURLA(NULL, &dwSize);
			if (tr != DPNERR_BUFFERTOOSMALL)
			{
				DPTEST_FAIL(hLog, "Couldn't get peer client address' URL size!", 0);
				THROW_TESTRESULT;
			} // end if (not buffer too small)
			
			dwURLsBufferSize += dwSize;




			// Allocate a buffer for both the URLs.
			pszURL = NULL;
			LOCALALLOC_OR_THROW( char*, pszURL, dwURLsBufferSize);




			dwSize = dwURLsBufferSize;
			tr = pDP8AddressRemoteCS->GetURLA(pszURL, &dwSize);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't get c/s client address' URL!", 0);

				MemFree(pszURL);
				pszURL = NULL;

				THROW_TESTRESULT;
			} // end if (failed)


			DPTEST_TRACE(hLog, "C/S client address: \"%s\"", 1, pszURL);

			// We're done with this address object.
			pDP8AddressRemoteCS->Release();
			pDP8AddressRemoteCS = NULL;


			dwTemp = dwURLsBufferSize - dwSize;
			tr = pDP8AddressRemotePeer->GetURLA(pszURL + dwSize, &dwTemp);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't get peer client address' URL!", 0);

				MemFree(pszURL);
				pszURL = NULL;

				THROW_TESTRESULT;
			} // end if (failed)

			DPTEST_TRACE(hLog, "Peer client address: \"%s\"", 1, pszURL + dwSize);


			// We're done with this address object.
			pDP8AddressRemotePeer->Release();
			pDP8AddressRemotePeer = NULL;




			// Straggling enums can still mosey in.  Give them some time so that
			// they don't screw up our non-host enum count.

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Sleeping 10 seconds to allow for straggling enums");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			Sleep(10000);


			if (context.dwNumMsgs > 0)
			{
				DPTEST_TRACE(hLog, "Note: %u enum(s) took their sweet time arriving.",
					1, context.dwNumMsgs);

				context.dwNumMsgs = 0;
			} // end if (enums straggled in)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Ensuring other tester is ready for non-host enums");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, NONHOST_EVENT_NAME,
				(BYTE *) pszURL, dwURLsBufferSize, NULL, 0))
			{
				DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, NONHOST_EVENT_NAME);
				SETTHROW_TESTRESULT(E_ABORT);
			}

		} // end if (tester 0)
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Ensuring other tester is ready for non-host enums");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			LOCALALLOC_OR_THROW( char *, pszURL, MAX_SYNC_DATASIZE);

			dwSyncDataSize = MAX_SYNC_DATASIZE;
			if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, NONHOST_EVENT_NAME,
				NULL, 0, (PBYTE) pszURL, &dwSyncDataSize))
			{
				DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, NONHOST_EVENT_NAME);
				SETTHROW_TESTRESULT(E_ABORT);
			}

			if(dwSyncDataSize < (DWORD) ((strlen(DPNA_HEADER_A) + 1) * 2))
			{
				DPTEST_FAIL(hLog, "Received sync data is below minimum amount (%u < %u)", 2,
					dwSyncDataSize, ((strlen(DPNA_HEADER_A) + 1) * 2));
				SETTHROW_TESTRESULT(E_ABORT);
			}

			tr = pDP8AddressRemoteCS->BuildFromURLA(pszURL);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't build host's C/S address from URL (\"%s\")!",
					1, pszURL);
				THROW_TESTRESULT;
			} // end if (couldn't build from URL)

			pszURL += strlen(pszURL) + 1;


			tr = pDP8AddressRemotePeer->BuildFromURLA(pszURL);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't build host's peer address from URL (\"%s\")!",
					1, pszURL);
				THROW_TESTRESULT;
			} // end if (couldn't build from URL)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Enumerating c/s client");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Client->DP8C_EnumHosts(&dpnadCS,
											pDP8AddressRemoteCS,
											pDP8AddressLocal,
											NULL,
											0,
											0, // default EnumCount
											0, // default RetryInterval
											0, // default TimeOut
											hEnumDoneEvent,
											&dpnhEnumHosts,
											0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog, "Couldn't start enumerating hosts!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't enumhosts)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for enumeration to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(hEnumDoneEvent, ENUM_HOSTS_TIMEOUT) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Enumeration didn't complete within %u seconds", 1, ENUM_HOSTS_TIMEOUT / 1000);
				THROW_SYSTEMRESULT;
			}

			sr = (HRESULT) S_OK;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Enumerating non-host peer");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerClient->DP8P_EnumHosts(&dpnadPeer,
												pDP8AddressRemotePeer,
												pDP8AddressLocal,
												NULL,
												0,
												0, // default EnumCount
												0, // default RetryInterval
												0, // default TimeOut
												hEnumDoneEvent,
												&dpnhEnumHosts,
												0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPTEST_FAIL(hLog, "Couldn't start enumerating hosts!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't enumhosts)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for enumeration to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if(WaitForSingleObject(hEnumDoneEvent, ENUM_HOSTS_TIMEOUT) != WAIT_OBJECT_0)
			{
				DPTEST_FAIL(hLog, "Enumeration didn't complete within %u seconds", 1, ENUM_HOSTS_TIMEOUT / 1000);
				THROW_SYSTEMRESULT;
			}

			sr = (HRESULT) S_OK;

			CloseHandle(hEnumDoneEvent);
			hEnumDoneEvent = NULL;
		} // end if (not tester 0)
		






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for other tester to finish non-host enumeration phase");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if(!SyncWithOtherTesters(hLog, pEnumTestParam->hNetsyncObject, NONHOSTDONE_EVENT_NAME, NULL, 0, NULL, 0))
		{
			DPTEST_FAIL(hLog, "Couldn't sync with other testers on \"%s\" event", 1, NONHOSTDONE_EVENT_NAME);
			SETTHROW_TESTRESULT(E_ABORT);
		}


		// Make sure we didn't see any queries/responses come in.
		if (context.dwNumMsgs != 0)
		{
			DPTEST_FAIL(hLog, "Received %u enum queries/responses!", 1, context.dwNumMsgs);
			SETTHROW_TESTRESULT(ERROR_MORE_DATA);
		} // end if (got queries/responses)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing failed!", 0);
			THROW_TESTRESULT;
		} // end if (close failed)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Client object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Client);
		pDP8Client = NULL;


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing peer client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8PeerClient->DP8P_Close(0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Closing failed!", 0);
			THROW_TESTRESULT;
		} // end if (close failed)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Peer object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8PeerClient->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerClient);
		pDP8PeerClient = NULL;


		if(pEnumTestParam->fHostTest)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Closing server object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Server->DP8S_Close(0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Closing failed!", 0);
				THROW_TESTRESULT;
			} // end if (close failed)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Server object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Server->Release();
			if (tr != S_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Server object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't release object)

			delete (pDP8Server);
			pDP8Server = NULL;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Closing peer host object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerHost->DP8P_Close(0);
			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Closing failed!", 0);
				THROW_TESTRESULT;
			} // end if (close failed)

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Peer object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerHost->Release();
			if (tr != S_OK)
			{
				DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't release object)

			delete (pDP8PeerHost);
			pDP8PeerHost = NULL;

		} // end if (tester 0)
		




		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8PeerClient != NULL)
	{
		delete (pDP8PeerClient);
		pDP8PeerClient = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	if (pDP8Client != NULL)
	{
		delete (pDP8Client);
		pDP8Client = NULL;
	} // end if (have client object)

	if (pDP8Server != NULL)
	{
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)


	if (paDP8HostAddresses != NULL)
	{
		for(dwTemp = 0; dwTemp < (dwNumAddressesCS + dwNumAddressesPeer); dwTemp++)
		{
			SAFE_RELEASE(paDP8HostAddresses[dwTemp]);
		} // end for (each address)

		MemFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	} // end if (have array of host addresses)

	SAFE_LOCALFREE(pszURL);
	SAFE_LOCALFREE(pvSyncData);
	SAFE_LOCALFREE(pHostAddressesCSAndPeerSyncData);
	SAFE_RELEASE(pDP8AddressLocal);
	SAFE_RELEASE(pDP8AddressRemoteCS);
	SAFE_RELEASE(pDP8AddressRemotePeer);
	SAFE_CLOSEHANDLE(hEnumDoneEvent);

	
	return (sr);
} // EnumsExec_Validate
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"EnumsExec_StartUpDown()"
//==================================================================================
// EnumsExec_StartUpDown
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.4.4 - Peer-to-peer enumeration during startup and shutdown test
//				2.4.5 - Client/server enumeration during startup and shutdown test
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
HRESULT EnumsExec_StartUpDown(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult			sr;
	CTNTestResult			tr;
	DPN_APPLICATION_DESC	dpnad;
	PWRAPDP8PEER			pDP8Peer = NULL;
	PWRAPDP8SERVER			pDP8Server = NULL;
	PWRAPDP8CLIENT			pDP8Client = NULL;
	BOOL					fClientServer = FALSE;
	PDIRECTPLAY8ADDRESS		pDP8Address = NULL;
	HANDLE					hEnumDoneEvent = NULL;
	DPNHANDLE				dpnhEnumHosts = NULL;
	DWORD					dwTemp;

	
	
	BEGIN_TESTCASE
	{
		if(pTNecd->pExecutor->IsCase("2.4.2"))
		{
			fClientServer = TRUE;

			if(pTNecd->iTesterNum == 0)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Creating DirectPlay8Server wrapper object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				pDP8Server = new (CWrapDP8Server);
				if (pDP8Server == NULL)
				{
					SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
				} // end if (couldn't allocate object)
			} // end if (tester 0)
			else
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Creating DirectPlay8Client wrapper object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				pDP8Client = new (CWrapDP8Client);
				if (pDP8Client == NULL)
				{
					SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
				} // end if (couldn't allocate object)
			} // end else (not tester 0)
			
		} // end if (client/server)
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating DirectPlay8Peer wrapper object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			pDP8Peer = new (CWrapDP8Peer);
			if (pDP8Peer == NULL)
			{
				SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
			} // end if (couldn't allocate object)
		} // end else (peer-to-peer)
		



		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpnad.dwFlags = 0;
		if ((pTNecd->iTesterNum == 0) && (fClientServer))
			dpnad.dwFlags |= DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_ENUMS_STARTUPDOWN;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		dpnad.pwszSessionName = (pTNecd->iTesterNum == 0) ? ENUM_SESSION_NAME : NULL;
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.dwApplicationReservedDataSize = 0;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("CoCreating address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8Address, NULL);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (couldn't CoCreate object)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting address object's SP to TCP/IP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't set SP for address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)




#pragma TODO(vanceo, "Customize number of iterations")
		TESTSECTION_FOR(dwTemp = 0; dwTemp < 20; dwTemp++)
		{
			DPTEST_TRACE(hLog, "Beginning iteration %u.", 1, dwTemp);

			pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
									"Beginning iteration %u",
									1, dwTemp);



			// Leave a little time between attempts, while making sure the control
			// layer doesn't think we've deadlocked.
			TNSLEEP(100);



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("CoCreating object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if (fClientServer)
			{
				if (pTNecd->iTesterNum == 0)
				{
					tr = pDP8Server->CoCreate();
				} // end if (tester 0)
				else
				{
					tr = pDP8Client->CoCreate();
				} // end else (not tester 0)
			} // end if (client/server)
			else
			{
				tr = pDP8Peer->CoCreate();
			} // end else (peer-to-peer)

			if (tr != S_OK)
			{
				DPTEST_FAIL(hLog, "CoCreating object failed!", 0);
				THROW_TESTRESULT;
			} // end if (CoCreating failed)



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Initializing object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if (fClientServer)
			{
				if (pTNecd->iTesterNum == 0)
				{
					tr = pDP8Server->DP8S_Initialize(NULL,
													EnumsStartUpDownDPNMessageHandler,
													0);
				} // end if (tester 0)
				else
				{
					tr = pDP8Client->DP8C_Initialize(NULL,
													EnumsStartUpDownDPNMessageHandler,
													0);
				} // end else (not tester 0)
			} // end if (client/server)
			else
			{
				tr = pDP8Peer->DP8P_Initialize(NULL,
												EnumsStartUpDownDPNMessageHandler,
												0);
			} // end else (peer-to-peer)

			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Initializing object failed!", 0);
				THROW_TESTRESULT;
			} // end if (initializing failed)



			if(pTNecd->iTesterNum == 0)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Hosting session object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				
				if (fClientServer)
				{
					tr = pDP8Server->DP8S_Host(&dpnad, &pDP8Address, 1,
												NULL, NULL, NULL, 0);
				} // end if (client/server)
				else
				{
					tr = pDP8Peer->DP8P_Host(&dpnad, &pDP8Address, 1,
											NULL, NULL, NULL, 0);
				} // end else (peer-to-peer)

				if (tr != DPN_OK)
				{
					DPTEST_FAIL(hLog, "Initializing object failed!", 0);
					THROW_TESTRESULT;
				} // end if (initializing failed)
			} // end if (tester 0)
			else
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Starting enumeration");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				
#pragma TODO(vanceo, "Customize retry interval")
				if (fClientServer)
				{
					tr = pDP8Client->DP8C_EnumHosts(&dpnad, NULL, pDP8Address,
													NULL, 0,
													INFINITE, 50, 0,
													NULL, &dpnhEnumHosts, 0);
				} // end if (client/server)
				else
				{
					tr = pDP8Peer->DP8P_EnumHosts(&dpnad, NULL, pDP8Address,
													NULL, 0,
													INFINITE, 50, 0,
													NULL, &dpnhEnumHosts, 0);
				} // end else (peer-to-peer)

				if (tr != DPNSUCCESS_PENDING)
				{
					DPTEST_FAIL(hLog, "Starting enumeration failed!", 0);
					THROW_TESTRESULT;
				} // end if (enumhosts failed)
			} // end else (not tester 0)
			



#pragma TODO(vanceo, "Programatically vary sleep time; customize iterations")
			if(dwTemp == (20 / 2))
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Synchronizing at half-way point");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Halfway", NULL, 0, NULL, 0);
				HANDLE_SYNC_RESULT;
			} // end if (exactly half-way through)
			else
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Sleeping for 2 seconds");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				TNSLEEP(2000);
			} // end else (not exactly half-way through)
			




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Closing object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if (fClientServer)
			{
				if (pTNecd->iTesterNum == 0)
				{
					tr = pDP8Server->DP8S_Close(0);
				} // end if (tester 0)
				else
				{
					tr = pDP8Client->DP8C_Close(0);
				} // end else (not tester 0)
			} // end if (client/server)
			else
			{
				tr = pDP8Peer->DP8P_Close(0);
			} // end else (peer-to-peer)

			if (tr != DPN_OK)
			{
				DPTEST_FAIL(hLog, "Closing object failed!", 0);
				THROW_TESTRESULT;
			} // end if (closing failed)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if (fClientServer)
			{
				if (pTNecd->iTesterNum == 0)
				{
					tr = pDP8Server->Release();
				} // end if (tester 0)
				else
				{
					tr = pDP8Client->Release();
				} // end else (not tester 0)
			} // end if (client/server)
			else
			{
				tr = pDP8Peer->Release();
			} // end else (peer-to-peer)

			if (tr != S_OK)
			{
				DPTEST_FAIL(hLog, "Releasing object failed!", 0);
				THROW_TESTRESULT;
			} // end if (releasing failed)
		} // end for (each startup/shutdown iteration)
		TESTSECTION_ENDFOR




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for other tester to finish");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->SyncWithTesters("All done", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;




		if(fClientServer)
		{
			if(pTNecd->iTesterNum == 0)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Destroying DirectPlay8Server wrapper object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				delete (pDP8Server);
				pDP8Server = NULL;
			} // end if (tester 0)
			else
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Destroying DirectPlay8Client wrapper object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				delete (pDP8Client);
				pDP8Client = NULL;
			} // end else (not tester 0)
			
		} // end if (client/server)
		else
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Destroying DirectPlay8Peer wrapper object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			delete (pDP8Peer);
			pDP8Peer = NULL;
		} // end else (peer-to-peer)
		



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8Server != NULL)
	{
		delete (pDP8Server);
		pDP8Server = NULL;
	} // end if (have server object)

	if (pDP8Client != NULL)
	{
		delete (pDP8Client);
		pDP8Client = NULL;
	} // end if (have client object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(hEnumDoneEvent);

	
	return (sr);
} // EnumsExec_StartUpDown
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/






#undef DEBUG_SECTION
#define DEBUG_SECTION	"EnumsSimpleDPNMessageHandler()"
//==================================================================================
// EnumsSimpleDPNMessageHandler
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
HRESULT EnumsSimpleDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT					hr = DPN_OK;
	PENUMSSIMPLECONTEXT		pContext = (PENUMSSIMPLECONTEXT) pvContext;
	DWORD					dwTempNumQueries;
	LONG					lTempNumOutstanding;


	switch (dwMsgType)
	{
		case DPN_MSGID_ASYNC_OP_COMPLETE:
			PDPNMSG_ASYNC_OP_COMPLETE	pAsyncOpCompleteMsg;


			pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_ASYNC_OP_COMPLETE", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pAsyncOpCompleteMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    hAsyncOp = %u", 1, pAsyncOpCompleteMsg->hAsyncOp);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = %x", 1, pAsyncOpCompleteMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog, "    hResultCode = 0x%08x", 1, pAsyncOpCompleteMsg->hResultCode);

			if (pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting operation complete on host!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (tester 0)

			if (pContext->fCancelled)
			{
				if (pAsyncOpCompleteMsg->hResultCode != DPNERR_USERCANCEL)
				{
					DPTEST_TRACE(pContext->hLog, "Enumeration didn't complete with expected error USERCANCEL!  DEBUGBREAK()-ing.  0x%08x",
						1, pAsyncOpCompleteMsg->hResultCode);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (not user cancell error)
			} // end if (user cancelled)
			else
			{
				if (pAsyncOpCompleteMsg->hResultCode != DPN_OK)
				{
					DPTEST_TRACE(pContext->hLog, "Enumeration completed with a failure!  DEBUGBREAK()-ing.  0x%08x",
						1, pAsyncOpCompleteMsg->hResultCode);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (not ok)
			} // end else (not cancelled)

			if (! SetEvent(pAsyncOpCompleteMsg->pvUserContext))
			{
				hr = GetLastError();

				DPTEST_TRACE(pContext->hLog, "Couldn't set async op completion event (%x)!  DEBUGBREAK()-ing.  0x%08x",
					2, pAsyncOpCompleteMsg->pvUserContext, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;
			
			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

			if (!pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting destroy player on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)

#pragma TODO(vanceo, "Validate DPN_MSGID_CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;
			
			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

			if (!pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting destroy player on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)

#pragma TODO(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		case DPN_MSGID_ENUM_HOSTS_QUERY:
			PDPNMSG_ENUM_HOSTS_QUERY	pEnumQueryMsg;



			if (!pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting enum response on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)


			pEnumQueryMsg = (PDPNMSG_ENUM_HOSTS_QUERY) pvMsg;
			
			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_ENUM_HOSTS_QUERY", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pEnumQueryMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    pAddressSender = 0x%08x", 1, pEnumQueryMsg->pAddressSender);
			DPTEST_TRACE(pContext->hLog, "    pAddressDevice = 0x%08x", 1, pEnumQueryMsg->pAddressDevice);
			DPTEST_TRACE(pContext->hLog, "    pvReceivedData = %x", 1, pEnumQueryMsg->pvReceivedData);
			DPTEST_TRACE(pContext->hLog, "    dwReceivedDataSize = %u", 1, pEnumQueryMsg->dwReceivedDataSize);
			DPTEST_TRACE(pContext->hLog, "    dwMaxResponseDataSize = %u", 1, pEnumQueryMsg->dwMaxResponseDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvResponseData = %x", 1, pEnumQueryMsg->pvResponseData);
			DPTEST_TRACE(pContext->hLog, "    dwResponseDataSize = %u", 1, pEnumQueryMsg->dwResponseDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvResponseContext = %x", 1, pEnumQueryMsg->pvResponseContext);


			// Verify the incoming data.
			if (pEnumQueryMsg->pvReceivedData != NULL)
			{
				if (pContext->pvUserData == NULL)
				{
					DPTEST_TRACE(pContext->hLog, "%u bytes of query user data was received (%x) but none was expected!  DEBUGBREAK()-ing.",
						2, pEnumQueryMsg->dwReceivedDataSize,
						pEnumQueryMsg->pvReceivedData);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's not supposed to be data)

				if (pEnumQueryMsg->dwReceivedDataSize != pContext->dwUserDataSize)
				{
					DPTEST_TRACE(pContext->hLog, "Size of query user data received (%u bytes at %x) but not expected value (%u)!  DEBUGBREAK()-ing.",
						3, pEnumQueryMsg->dwReceivedDataSize,
						pEnumQueryMsg->pvReceivedData, pContext->dwUserDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data size isn't correct)

				if (! IsFilledWithDWord(pEnumQueryMsg->pvReceivedData, pEnumQueryMsg->dwReceivedDataSize, ENUMDATAPATTERN_QUERY))
				{
					DPTEST_TRACE(pContext->hLog, "Query user data received is corrupted (%x, %u bytes)!  DEBUGBREAK()-ing.",
						2, pEnumQueryMsg->pvReceivedData,
						pEnumQueryMsg->dwReceivedDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data isn't correct)
			} // end if (incoming data)
			else
			{
				if (pContext->pvUserData != NULL)
				{
					DPTEST_TRACE(pContext->hLog, "%u bytes of query user data was expected but none was received!  DEBUGBREAK()-ing.",
						1, pContext->dwUserDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's supposed to be data)

				if (pEnumQueryMsg->dwReceivedDataSize != 0)
				{
					DPTEST_TRACE(pContext->hLog, "Query received data pointer was NULL as expected, but size was %u!  DEBUGBREAK()-ing.",
						1, pEnumQueryMsg->dwReceivedDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (the size is not right)
			} // end else (no incoming data)


#pragma TODO(vanceo, "Make sure it's the other tester querying (address, don't respond if not him)")

			dwTempNumQueries = (DWORD) InterlockedIncrement((LPLONG) &(pContext->dwNumMsgs));

			// Respond with data, if there is any.
			if (pContext->pvUserData != NULL)
			{
				// Make sure we're allowed to respond with data.
				if (pContext->fDontRespondWithData)
				{
					DPTEST_TRACE(pContext->hLog, "Not responding to straggling enum query (%u) with data like normal.",
						1, dwTempNumQueries);
				} // end if (shouldn't respond with data)
				else
				{
					lTempNumOutstanding = InterlockedIncrement(&(pContext->lNumOutstandingResponseData));
					if (lTempNumOutstanding <= 0)
					{
						DPTEST_TRACE(pContext->hLog, "Number of outstanding response data buffers is bad!  DEBUGBREAK()-ing.", 0);
						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					} // end if (count went negative)

					pEnumQueryMsg->pvResponseData = pContext->pvUserData;
					pEnumQueryMsg->dwResponseDataSize = pContext->dwUserDataSize;
					pEnumQueryMsg->pvResponseContext = (PVOID) (DWORD_PTR) dwTempNumQueries;

					DPTEST_TRACE(pContext->hLog, "Setting query %u response data to %u bytes at %x (now %i outstanding responses).",
						4, dwTempNumQueries, pContext->dwUserDataSize,
						pContext->pvUserData, lTempNumOutstanding);
				} // end else (can respond with data)
			} // end if (there's some user data)

			// If we're refusing to respond, do so.
			if (pContext->fReject)
			{
				DPTEST_TRACE(pContext->hLog, "Refusing to respond to query.", 0);
				hr = E_FAIL;
			} // end if (rejecting enumeration)
		  break;

		case DPN_MSGID_ENUM_HOSTS_RESPONSE:
			PDPNMSG_ENUM_HOSTS_RESPONSE		pEnumHostsResponseMsg;



			if (pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting enum response on host!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (tester 0)


			pEnumHostsResponseMsg = (PDPNMSG_ENUM_HOSTS_RESPONSE) pvMsg;
			
			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_ENUM_HOSTS_RESPONSE", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pEnumHostsResponseMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    pAddressSender = 0x%08x", 1, pEnumHostsResponseMsg->pAddressSender);
			DPTEST_TRACE(pContext->hLog, "    pAddressDevice = 0x%08x", 1, pEnumHostsResponseMsg->pAddressDevice);
			DPTEST_TRACE(pContext->hLog, "    pApplicationDescription = %x", 1, pEnumHostsResponseMsg->pApplicationDescription);
			DPTEST_TRACE(pContext->hLog, "    pvResponseData = %x", 1, pEnumHostsResponseMsg->pvResponseData);
			DPTEST_TRACE(pContext->hLog, "    dwResponseDataSize = %u", 1, pEnumHostsResponseMsg->dwResponseDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = %x", 1, pEnumHostsResponseMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog, "    dwRoundTripLatencyMS = %u", 1, pEnumHostsResponseMsg->dwRoundTripLatencyMS);


			if (pContext->fReject)
			{
				DPTEST_TRACE(pContext->hLog, "Getting enum response even though host should have not replied!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (host supposed to reject)

#pragma TODO(vanceo, "Make sure it's the other tester responding (address)")

			if (pEnumHostsResponseMsg->pApplicationDescription->dwSize != sizeof (DPN_APPLICATION_DESC))
			{
				DPTEST_TRACE(pContext->hLog, "Size of application desc is wrong (%u != %u)!  DEBUGBREAK()-ing.",
					2, pEnumHostsResponseMsg->pApplicationDescription->dwSize,
					sizeof (DPN_APPLICATION_DESC));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc size is wrong)

#pragma TODO(vanceo, "Validate app desc dwFlags and guidInstance")

			if (memcmp(&(pEnumHostsResponseMsg->pApplicationDescription->guidApplication), &GUID_ENUMS_SIMPLE, sizeof (GUID)) != 0)
			{
				DPTEST_TRACE(pContext->hLog, "Application desc app guid is wrong (%g != %g)!  DEBUGBREAK()-ing.",
					2, &pEnumHostsResponseMsg->pApplicationDescription->guidApplication,
					&GUID_ENUMS_SIMPLE);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc size is wrong)

			if (pEnumHostsResponseMsg->pApplicationDescription->dwMaxPlayers != 0)
			{
				DPTEST_TRACE(pContext->hLog, "Application desc max players is wrong (%u != 0)!  DEBUGBREAK()-ing.",
					1, pEnumHostsResponseMsg->pApplicationDescription->dwMaxPlayers);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc max players is wrong)

			if (pEnumHostsResponseMsg->pApplicationDescription->dwCurrentPlayers != 1)
			{
				DPTEST_TRACE(pContext->hLog, "Application desc current players is wrong (%u != 1)!  DEBUGBREAK()-ing.",
					1, pEnumHostsResponseMsg->pApplicationDescription->dwCurrentPlayers);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc current players is wrong)

			if (wcscmp(pEnumHostsResponseMsg->pApplicationDescription->pwszSessionName, ENUM_SESSION_NAME) != 0)
			{
				DPTEST_TRACE(pContext->hLog, "Application desc session name is wrong (\"%S\" != \"%S\")!  DEBUGBREAK()-ing.",
					2, pEnumHostsResponseMsg->pApplicationDescription->pwszSessionName,
					ENUM_SESSION_NAME);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc session name is wrong)

			if (pEnumHostsResponseMsg->pApplicationDescription->pwszPassword != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Application desc password is not NULL (%S)!  DEBUGBREAK()-ing.",
					1, pEnumHostsResponseMsg->pApplicationDescription->pwszPassword);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc password is wrong)

			if ((pEnumHostsResponseMsg->pApplicationDescription->pvReservedData != NULL) ||
				(pEnumHostsResponseMsg->pApplicationDescription->dwReservedDataSize != 0))
			{
				DPTEST_TRACE(pContext->hLog, "Application desc reserved data is not NULL/0 (%x, %u)!  DEBUGBREAK()-ing.",
					2, pEnumHostsResponseMsg->pApplicationDescription->pvReservedData,
					pEnumHostsResponseMsg->pApplicationDescription->dwReservedDataSize);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc reserved data is wrong)


			// Verify the app desc data.
			if (pEnumHostsResponseMsg->pApplicationDescription->pvApplicationReservedData != NULL)
			{
				if (pContext->dwAppDescDataSize == 0)
				{
					DPTEST_TRACE(pContext->hLog, "%u bytes of application desc data was received (%x) but none was expected!  DEBUGBREAK()-ing.",
						2, pEnumHostsResponseMsg->pApplicationDescription->dwApplicationReservedDataSize,
						pEnumHostsResponseMsg->pApplicationDescription->pvApplicationReservedData);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's not supposed to be data)

				if (pEnumHostsResponseMsg->pApplicationDescription->dwApplicationReservedDataSize != pContext->dwAppDescDataSize)
				{
					DPTEST_TRACE(pContext->hLog, "Size of application desc data received (%u bytes at %x) but not expected value (%u)!  DEBUGBREAK()-ing.",
						3, pEnumHostsResponseMsg->pApplicationDescription->dwApplicationReservedDataSize,
						pEnumHostsResponseMsg->pApplicationDescription->pvApplicationReservedData,
						pContext->dwAppDescDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data size isn't correct)

				if (! IsFilledWithDWord(pEnumHostsResponseMsg->pApplicationDescription->pvApplicationReservedData, pEnumHostsResponseMsg->pApplicationDescription->dwApplicationReservedDataSize, ENUMDATAPATTERN_APPDESC))
				{
					DPTEST_TRACE(pContext->hLog, "Application desc data received is corrupted (%x, %u bytes)!  DEBUGBREAK()-ing.",
						2, pEnumHostsResponseMsg->pApplicationDescription->pvApplicationReservedData,
						pEnumHostsResponseMsg->pApplicationDescription->dwApplicationReservedDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data isn't correct)
			} // end if (appdesc data)
			else
			{
				if (pContext->dwAppDescDataSize != 0)
				{
					DPTEST_TRACE(pContext->hLog, "%u bytes of application desc data was expected but none was received!  DEBUGBREAK()-ing.",
						1, pContext->dwAppDescDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's supposed to be data)

				if (pEnumHostsResponseMsg->pApplicationDescription->dwApplicationReservedDataSize != 0)
				{
					DPTEST_TRACE(pContext->hLog, "Application desc received data pointer was NULL as expected, but size was %u!  DEBUGBREAK()-ing.",
						1, pEnumHostsResponseMsg->pApplicationDescription->dwApplicationReservedDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (the size is not right)
			} // end else (no appdesc data)


			// Verify the incoming data.
			if (pEnumHostsResponseMsg->pvResponseData != NULL)
			{
				if (pContext->pvUserData == NULL)
				{
					DPTEST_TRACE(pContext->hLog, "%u bytes of response user data was received (%x) but none was expected!  DEBUGBREAK()-ing.",
						2, pEnumHostsResponseMsg->dwResponseDataSize,
						pEnumHostsResponseMsg->pvResponseData);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's not supposed to be data)

				if (pEnumHostsResponseMsg->dwResponseDataSize != pContext->dwUserDataSize)
				{
					DPTEST_TRACE(pContext->hLog, "Size of response user data received (%u bytes at %x) but not expected value (%u)!  DEBUGBREAK()-ing.",
						3, pEnumHostsResponseMsg->dwResponseDataSize,
						pEnumHostsResponseMsg->pvResponseData,
						pContext->dwUserDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data size isn't correct)

				if (! IsFilledWithDWord(pEnumHostsResponseMsg->pvResponseData, pEnumHostsResponseMsg->dwResponseDataSize, ENUMDATAPATTERN_RESPONSE))
				{
					DPTEST_TRACE(pContext->hLog, "Response user data received is corrupted (%x, %u bytes)!  DEBUGBREAK()-ing.",
						2, pEnumHostsResponseMsg->pvResponseData,
						pEnumHostsResponseMsg->dwResponseDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data isn't correct)
			} // end if (incoming data)
			else
			{
				if (pContext->pvUserData != NULL)
				{
					DPTEST_TRACE(pContext->hLog, "%u bytes of response user data was expected but none was received!  DEBUGBREAK()-ing.",
						1, pContext->dwUserDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's supposed to be data)

				if (pEnumHostsResponseMsg->dwResponseDataSize != 0)
				{
					DPTEST_TRACE(pContext->hLog, "Response received data pointer was NULL as expected, but size was %u!  DEBUGBREAK()-ing.",
						1, pEnumHostsResponseMsg->dwResponseDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (the size is not right)
			} // end else (no incoming data)

			InterlockedIncrement((LPLONG) &(pContext->dwNumMsgs));
		  break;

		case DPN_MSGID_RETURN_BUFFER:
			PDPNMSG_RETURN_BUFFER	pReturnBufferMsg;



			if (!pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting return buffer on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)


			pReturnBufferMsg = (PDPNMSG_RETURN_BUFFER) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_RETURN_BUFFER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pReturnBufferMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    hResultCode = 0x%08x", 1, pReturnBufferMsg->hResultCode);
			DPTEST_TRACE(pContext->hLog, "    pvBuffer = %x", 1, pReturnBufferMsg->pvBuffer);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = %x", 1, pReturnBufferMsg->pvUserContext);


			if (pContext->pvUserData == NULL)
			{
				DPTEST_TRACE(pContext->hLog, "No response data was sent but returning buffer %x (context = %x)!?  DEBUGBREAK()-ing.",
					2, pReturnBufferMsg->pvBuffer,
					pReturnBufferMsg->pvUserContext);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not using data)

			if (pReturnBufferMsg->pvBuffer != pContext->pvUserData)
			{
				DPTEST_TRACE(pContext->hLog, "Buffer returned is not expected (%x != %x)!  DEBUGBREAK()-ing.",
					2, pReturnBufferMsg->pvBuffer, pContext->pvUserData);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (buffer is wrong)

			if (pReturnBufferMsg->pvUserContext == NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Context returned is NULL!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (context is NULL)

			lTempNumOutstanding = InterlockedDecrement(&(pContext->lNumOutstandingResponseData));
			if (lTempNumOutstanding < 0)
			{
				DPTEST_TRACE(pContext->hLog, "Number of outstanding response data buffers has gone negative!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (count went negative)
		  break;

		default:
			DPTEST_TRACE(pContext->hLog, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // EnumsSimpleDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"EnumsValidateDPNMessageHandler()"
//==================================================================================
// EnumsValidateDPNMessageHandler
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
HRESULT EnumsValidateDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT					hr = DPN_OK;
	PENUMSVALIDATECONTEXT	pContext = (PENUMSVALIDATECONTEXT) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_APPLICATION_DESC:
			if (!pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting application desc notification on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)
		  break;

		case DPN_MSGID_ASYNC_OP_COMPLETE:
			PDPNMSG_ASYNC_OP_COMPLETE	pAsyncOpCompleteMsg;


			pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE) pvMsg;

			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_ASYNC_OP_COMPLETE", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pAsyncOpCompleteMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    hAsyncOp = %u", 1, pAsyncOpCompleteMsg->hAsyncOp);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = %x", 1, pAsyncOpCompleteMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog, "    hResultCode = 0x%08x", 1, pAsyncOpCompleteMsg->hResultCode);


			if (pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting operation complete on host!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (tester 0)

			if (pAsyncOpCompleteMsg->hResultCode != DPN_OK)
			{
				DPTEST_TRACE(pContext->hLog, "Enumeration completed with a failure!  DEBUGBREAK()-ing.  0x%08x",
					1, pAsyncOpCompleteMsg->hResultCode);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not ok)

			if (! SetEvent(pAsyncOpCompleteMsg->pvUserContext))
			{
				hr = GetLastError();

				DPTEST_TRACE(pContext->hLog, "Couldn't set async op completion event (%x)!  DEBUGBREAK()-ing.  0x%08x",
					2, pAsyncOpCompleteMsg->pvUserContext, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;



			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;
			
			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog, "    hResultCode = 0x%08x", 1, pConnectCompleteMsg->hResultCode);
			DPTEST_TRACE(pContext->hLog, "    pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			if (!pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting connect completion on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (tester 0)

#pragma TODO(vanceo, "Validate hAsyncOp")

			// Make sure we got the right error result code.
			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				DPTEST_TRACE(pContext->hLog, "Connect failed!  DEBUGBREAK()-ing.  0x%08x",
					1, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong error code)


			// Verify the incoming data.
			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				DPTEST_TRACE(pContext->hLog, "%u bytes of reply data was received (%x) but none was expected!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwApplicationReplyDataSize,
					pConnectCompleteMsg->pvApplicationReplyData);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (incoming data)
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;
			
			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

			if (!pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting destroy player on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)

#pragma TODO(vanceo, "Validate DPN_MSGID_CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;
			
			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

			if (!pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting destroy player on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)

#pragma TODO(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		case DPN_MSGID_ENUM_HOSTS_QUERY:
			PDPNMSG_ENUM_HOSTS_QUERY	pEnumQueryMsg;



			if (!pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting enum response on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)


			pEnumQueryMsg = (PDPNMSG_ENUM_HOSTS_QUERY) pvMsg;
			
			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_ENUM_HOSTS_QUERY", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pEnumQueryMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    pAddressSender = 0x%08x", 1, pEnumQueryMsg->pAddressSender);
			DPTEST_TRACE(pContext->hLog, "    pAddressDevice = 0x%08x", 1, pEnumQueryMsg->pAddressDevice);
			DPTEST_TRACE(pContext->hLog, "    pvReceivedData = %x", 1, pEnumQueryMsg->pvReceivedData);
			DPTEST_TRACE(pContext->hLog, "    dwReceivedDataSize = %u", 1, pEnumQueryMsg->dwReceivedDataSize);
			DPTEST_TRACE(pContext->hLog, "    dwMaxResponseDataSize = %u", 1, pEnumQueryMsg->dwMaxResponseDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvResponseData = %x", 1, pEnumQueryMsg->pvResponseData);
			DPTEST_TRACE(pContext->hLog, "    dwResponseDataSize = %u", 1, pEnumQueryMsg->dwResponseDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvResponseContext = %x", 1, pEnumQueryMsg->pvResponseContext);


#pragma TODO(vanceo, "Detect queries on non-host interfaces of tester 0")

			// Verify the incoming data.
			if (pEnumQueryMsg->pvReceivedData != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "%u bytes of query user data was received (%x) but none was expected!  DEBUGBREAK()-ing.",
					2, pEnumQueryMsg->dwReceivedDataSize,
					pEnumQueryMsg->pvReceivedData);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (incoming data)


#pragma TODO(vanceo, "Make sure it's the other tester querying (address, don't respond if not him)")

			InterlockedIncrement((LPLONG) &(pContext->dwNumMsgs));
		  break;

		case DPN_MSGID_ENUM_HOSTS_RESPONSE:
			PDPNMSG_ENUM_HOSTS_RESPONSE		pEnumHostsResponseMsg;



			if (pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting enum response on host!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (tester 0)


			pEnumHostsResponseMsg = (PDPNMSG_ENUM_HOSTS_RESPONSE) pvMsg;
			
			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_ENUM_HOSTS_RESPONSE", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pEnumHostsResponseMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    pAddressSender = 0x%08x", 1, pEnumHostsResponseMsg->pAddressSender);
			DPTEST_TRACE(pContext->hLog, "    pAddressDevice = 0x%08x", 1, pEnumHostsResponseMsg->pAddressDevice);
			DPTEST_TRACE(pContext->hLog, "    pApplicationDescription = %x", 1, pEnumHostsResponseMsg->pApplicationDescription);
			DPTEST_TRACE(pContext->hLog, "    pvResponseData = %x", 1, pEnumHostsResponseMsg->pvResponseData);
			DPTEST_TRACE(pContext->hLog, "    dwResponseDataSize = %u", 1, pEnumHostsResponseMsg->dwResponseDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvUserContext = %x", 1, pEnumHostsResponseMsg->pvUserContext);
			DPTEST_TRACE(pContext->hLog, "    dwRoundTripLatencyMS = %u", 1, pEnumHostsResponseMsg->dwRoundTripLatencyMS);


#pragma TODO(vanceo, "Make sure it's the other tester responding (address)")

			if (pEnumHostsResponseMsg->pApplicationDescription->dwSize != sizeof (DPN_APPLICATION_DESC))
			{
				DPTEST_TRACE(pContext->hLog, "Size of application desc is wrong (%u != %u)!  DEBUGBREAK()-ing.",
					2, pEnumHostsResponseMsg->pApplicationDescription->dwSize,
					sizeof (DPN_APPLICATION_DESC));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc size is wrong)

#pragma TODO(vanceo, "Validate app desc dwFlags and guidInstance")

#pragma TODO(vanceo, "Get real GUID")
			if (memcmp(&(pEnumHostsResponseMsg->pApplicationDescription->guidApplication), &GUID_ENUMS_VALIDATE, sizeof (GUID)) != 0)
			{
				DPTEST_TRACE(pContext->hLog, "Application desc app guid is wrong (%g != %g)!  DEBUGBREAK()-ing.",
					2, &pEnumHostsResponseMsg->pApplicationDescription->guidApplication,
					&GUID_ENUMS_SIMPLE);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc size is wrong)

			if (pEnumHostsResponseMsg->pApplicationDescription->dwMaxPlayers != pContext->dwExpectedMaxPlayers)
			{
				DPTEST_TRACE(pContext->hLog, "Application desc max players is wrong (%u != %u)!  DEBUGBREAK()-ing.",
					2, pEnumHostsResponseMsg->pApplicationDescription->dwMaxPlayers,
					pContext->dwExpectedMaxPlayers);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc max players is wrong)

			if (pEnumHostsResponseMsg->pApplicationDescription->dwCurrentPlayers != 2)
			{
				DPTEST_TRACE(pContext->hLog, "Application desc current players is wrong (%u != 2)!  DEBUGBREAK()-ing.",
					1, pEnumHostsResponseMsg->pApplicationDescription->dwCurrentPlayers);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc current players is wrong)

			if (wcscmp(pEnumHostsResponseMsg->pApplicationDescription->pwszSessionName, ENUM_SESSION_NAME) != 0)
			{
				DPTEST_TRACE(pContext->hLog, "Application desc session name is wrong (\"%S\" != \"%S\")!  DEBUGBREAK()-ing.",
					2, pEnumHostsResponseMsg->pApplicationDescription->pwszSessionName,
					ENUM_SESSION_NAME);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc session name is wrong)

			if (pEnumHostsResponseMsg->pApplicationDescription->pwszPassword != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "Application desc password is not NULL (%S)!  DEBUGBREAK()-ing.",
					1, pEnumHostsResponseMsg->pApplicationDescription->pwszPassword);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc password is wrong)

			if ((pEnumHostsResponseMsg->pApplicationDescription->pvReservedData != NULL) ||
				(pEnumHostsResponseMsg->pApplicationDescription->dwReservedDataSize != 0))
			{
				DPTEST_TRACE(pContext->hLog, "Application desc reserved data is not NULL/0 (%x, %u)!  DEBUGBREAK()-ing.",
					2, pEnumHostsResponseMsg->pApplicationDescription->pvReservedData,
					pEnumHostsResponseMsg->pApplicationDescription->dwReservedDataSize);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (app desc reserved data is wrong)


			// Verify the app desc data.
			if (pEnumHostsResponseMsg->pApplicationDescription->pvApplicationReservedData != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "%u bytes of application desc data was received (%x) but none was expected!  DEBUGBREAK()-ing.",
					2, pEnumHostsResponseMsg->pApplicationDescription->dwApplicationReservedDataSize,
					pEnumHostsResponseMsg->pApplicationDescription->pvApplicationReservedData);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (appdesc data)


			// Verify the incoming data.
			if (pEnumHostsResponseMsg->pvResponseData != NULL)
			{
				DPTEST_TRACE(pContext->hLog, "%u bytes of response user data was received (%x) but none was expected!  DEBUGBREAK()-ing.",
					2, pEnumHostsResponseMsg->dwResponseDataSize,
					pEnumHostsResponseMsg->pvResponseData);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (incoming data)

			InterlockedIncrement((LPLONG) &(pContext->dwNumMsgs));
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;



			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;
			
			DPTEST_TRACE(pContext->hLog, "DPN_MSGID_INDICATE_CONNECT", 0);
			DPTEST_TRACE(pContext->hLog, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			DPTEST_TRACE(pContext->hLog, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			DPTEST_TRACE(pContext->hLog, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			DPTEST_TRACE(pContext->hLog, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			DPTEST_TRACE(pContext->hLog, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			DPTEST_TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			DPTEST_TRACE(pContext->hLog, "    pAddressPlayer = 0x%08x", 1, pIndicateConnectMsg->pAddressPlayer);
			DPTEST_TRACE(pContext->hLog, "    pAddressDevice = 0x%08x", 1, pIndicateConnectMsg->pAddressDevice);

			if (!pContext->pEnumTestParam->fHostTest)
			{
				DPTEST_TRACE(pContext->hLog, "Getting indicate connect on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)
		  break;

		default:
			DPTEST_TRACE(pContext->hLog, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // EnumsValidateDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"EnumsStartUpDownDPNMessageHandler()"
//==================================================================================
// EnumsStartUpDownDPNMessageHandler
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
HRESULT EnumsStartUpDownDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT		hr = DPN_OK;



	switch (dwMsgType)
	{
		case DPN_MSGID_ASYNC_OP_COMPLETE:
			PDPNMSG_ASYNC_OP_COMPLETE	pAsyncOpCompleteMsg;


			pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE) pvMsg;

			DPL(0, "DPN_MSGID_ASYNC_OP_COMPLETE", 0);
			DPL(0, "    dwSize = %u", 1, pAsyncOpCompleteMsg->dwSize);
			DPL(0, "    hAsyncOp = %u", 1, pAsyncOpCompleteMsg->hAsyncOp);
			DPL(0, "    pvUserContext = %x", 1, pAsyncOpCompleteMsg->pvUserContext);
			DPL(0, "    hResultCode = 0x%08x", 1, pAsyncOpCompleteMsg->hResultCode);

#pragma TODO(vanceo, "Validate DPN_MSGID_ASYNC_OP_COMPLETE")
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;
			
			DPL(0, "DPN_MSGID_CREATE_PLAYER", 0);
			DPL(0, "    dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			DPL(0, "    dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			DPL(0, "    pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate DPN_MSGID_CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;
			
			DPL(0, "DPN_MSGID_DESTROY_PLAYER", 0);
			DPL(0, "    dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			DPL(0, "    dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			DPL(0, "    pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		case DPN_MSGID_ENUM_HOSTS_QUERY:
			PDPNMSG_ENUM_HOSTS_QUERY	pEnumQueryMsg;


			pEnumQueryMsg = (PDPNMSG_ENUM_HOSTS_QUERY) pvMsg;
			
			DPL(6, "DPN_MSGID_ENUM_HOSTS_QUERY", 0);
			DPL(6, "    dwSize = %u", 1, pEnumQueryMsg->dwSize);
			DPL(6, "    pAddressSender = 0x%08x", 1, pEnumQueryMsg->pAddressSender);
			DPL(6, "    pAddressDevice = 0x%08x", 1, pEnumQueryMsg->pAddressDevice);
			DPL(6, "    pvReceivedData = %x", 1, pEnumQueryMsg->pvReceivedData);
			DPL(6, "    dwReceivedDataSize = %u", 1, pEnumQueryMsg->dwReceivedDataSize);
			DPL(6, "    dwMaxResponseDataSize = %u", 1, pEnumQueryMsg->dwMaxResponseDataSize);
			DPL(6, "    pvResponseData = %x", 1, pEnumQueryMsg->pvResponseData);
			DPL(6, "    dwResponseDataSize = %u", 1, pEnumQueryMsg->dwResponseDataSize);
			DPL(6, "    pvResponseContext = %x", 1, pEnumQueryMsg->pvResponseContext);

#pragma TODO(vanceo, "Validate DPN_MSGID_ENUM_HOSTS_QUERY")
		  break;

		case DPN_MSGID_ENUM_HOSTS_RESPONSE:
			PDPNMSG_ENUM_HOSTS_RESPONSE		pEnumHostsResponseMsg;


			pEnumHostsResponseMsg = (PDPNMSG_ENUM_HOSTS_RESPONSE) pvMsg;
			
			DPL(6, "DPN_MSGID_ENUM_HOSTS_RESPONSE", 0);
			DPL(6, "    dwSize = %u", 1, pEnumHostsResponseMsg->dwSize);
			DPL(6, "    pAddressSender = 0x%08x", 1, pEnumHostsResponseMsg->pAddressSender);
			DPL(6, "    pAddressDevice = 0x%08x", 1, pEnumHostsResponseMsg->pAddressDevice);
			DPL(6, "    pApplicationDescription = %x", 1, pEnumHostsResponseMsg->pApplicationDescription);
			DPL(6, "    pvResponseData = %x", 1, pEnumHostsResponseMsg->pvResponseData);
			DPL(6, "    dwResponseDataSize = %u", 1, pEnumHostsResponseMsg->dwResponseDataSize);
			DPL(6, "    pvUserContext = %x", 1, pEnumHostsResponseMsg->pvUserContext);
			DPL(6, "    dwRoundTripLatencyMS = %u", 1, pEnumHostsResponseMsg->dwRoundTripLatencyMS);

#pragma TODO(vanceo, "Validate DPN_MSGID_ENUM_HOSTS_RESPONSE")
		  break;

		default:
			DPL(0, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


//DONE:

	return (hr);
} // EnumsStartUpDownDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/

} // namespace DPlayCoreNamespace