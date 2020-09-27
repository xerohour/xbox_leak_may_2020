//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#include <mmsystem.h>

#include "..\..\..\tncommon\debugprint.h"
#include "..\..\..\tncommon\cppobjhelp.h"
#include "..\..\..\tncommon\linklist.h"
#include "..\..\..\tncommon\linkedstr.h"
#include "..\..\..\tncommon\strutils.h"
#include "..\..\..\tncommon\sprintf.h"

#include "..\..\..\tncontrl\tncontrl.h"


#include "..\main.h"

#include "..\bldssn\blddp8.h"

#include "multissn.h"




#pragma TODO(vanceo, "Make this input data")
#define NUM_SESSIONS_TO_HOST	2





//==================================================================================
// Prototypes
//==================================================================================
HRESULT MultiSsnCanRun_All(PTNCANRUNDATA pTNcrd);

HRESULT MultiSsnExec_SimplePeer(PTNEXECCASEDATA pTNecd);
HRESULT MultiSsnExec_SimpleCS(PTNEXECCASEDATA pTNecd);

HRESULT MultiSsnDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);






#undef DEBUG_SECTION
#define DEBUG_SECTION	"MultiSsnLoadTestTable()"
//==================================================================================
// MultiSsnLoadTestTable
//----------------------------------------------------------------------------------
//
// Description: Loads all the possible tests into the table passed in:
//				2.6			Multiple sessions tests
//				2.6.1		Simple peer-to-peer multiple sessions test
//				2.6.2		Simple client/server multiple sessions test
//
// Arguments:
//	PTNLOADTESTTABLEDATA pTNlttd	Pointer to data to use when loading the tests.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT MultiSsnLoadTestTable(PTNLOADTESTTABLEDATA pTNlttd)
{
	PTNTESTTABLEGROUP	pSubGroup;
	TNADDTESTDATA		tnatd;



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.6		Multiple sessions tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pTNlttd->pBase, "2.6", "Multiple sessions tests", &pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.6.1	Simple peer-to-peer multiple sessions test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.6.1";
	tnatd.pszName			= "Simple peer-to-peer multiple sessions test";
	tnatd.pszDescription	= "Simple peer-to-peer multiple sessions test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 2;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= MultiSsnCanRun_All;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= MultiSsnExec_SimplePeer;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.6.2	Simple client/server multiple sessions test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.6.2";
	tnatd.pszName			= "Simple client/server multiple sessions test";
	tnatd.pszDescription	= "Simple client/server multiple sessions test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 2;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= MultiSsnCanRun_All;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= MultiSsnExec_SimpleCS;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);


	return (S_OK);
} // MultiSsnLoadTestTable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"MultiSsnCanRun_All()"
//==================================================================================
// MultiSsnCanRun_All
//----------------------------------------------------------------------------------
//
// Description: Callback that checks a possible tester list to make sure the test
//				can be run correctly for the following test case(s):
//				2.6.1 - Simple peer-to-peer multiple sessions test
//				2.6.2 - Simple client/server multiple sessions test
//
// Arguments:
//	PTNCANRUNDATA pTNcrd	Pointer to parameter block with information on what
//							configuration to check.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT MultiSsnCanRun_All(PTNCANRUNDATA pTNcrd)
{
	HRESULT		hr;
	int			i;
	int			j;


	// Make sure all the subsequent testers can connect to the first.
	for(i = 0; i < pTNcrd->iNumMachines; i++)
	{
		for(j = 0; j < pTNcrd->iNumMachines; j++)
		{
			// Nobody needs to connect to himself, so skip the check.
			if (j == i)
				continue;

			if (pTNcrd->pMaster->AreOnSameMachine(pTNcrd->apTesters[i],
												pTNcrd->apTesters[j]))
			{
				DPL(1, "Tester %i is on same machine as tester %i (%s), assuming can reach via UDP.",
					3, i, j, pTNcrd->apTesters[i]->m_szComputerName);

				pTNcrd->fCanRun = TRUE;
			} // end if (are on same machine)
			else
			{
				hr = pTNcrd->pMaster->CanReachViaIP(pTNcrd->apTesters[i],
													pTNcrd->apTesters[j],
													DPLAY8_PORT,
													TNCR_IP_UDP,
													&(pTNcrd->fCanRun));
				if (hr != S_OK)
				{
					DPL(0, "Couldn't have tester %i (%s) try to reach tester %i (%s) via UDP!",
						4, i, pTNcrd->apTesters[i]->m_szComputerName,
						j, pTNcrd->apTesters[j]->m_szComputerName);

					return (hr);
				} // end if (couldn't test reachability)

				if (! pTNcrd->fCanRun)
				{
					DPL(1, "Tester %i (%s) can't reach tester %i (%s) via UDP.",
						4, i, pTNcrd->apTesters[i]->m_szComputerName,
						j, pTNcrd->apTesters[0]->m_szComputerName);

					return (S_OK);
				} // end if (current tester couldn't reach tester 0)
			} // end else (not on same machine)
		} // end for (each tester)
	} // end for (each tester)


	return (S_OK);
} // MultiSsnCanRun_All
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"MultiSsnExec_SimplePeer()"
//==================================================================================
// MultiSsnExec_SimplePeer
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.6.1 - Simple peer-to-peer multiple sessions test
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
HRESULT MultiSsnExec_SimplePeer(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	HRESULT							temphr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PTOD_BBLDDP8_PEER_CREATE		pBldDP8PeerCreateOutput = NULL;
	DWORD							dwSubOutputDataSize;
	BLDDP8DPNHANDLERCONTEXTHEADER	context;
	DPN_APPLICATION_DESC			dpnad;
	PVOID*							papvDP8Objects = NULL;
	DPNID**							papadpnidTesters = NULL;
	int*							paiTesters = NULL;
	int								i;
	int								j;



	BEGIN_TESTCASE
	{
		// Allocate the arrays to hold pointers to all the objects we'll generate.
		LOCALALLOC_OR_THROW(PVOID*, papvDP8Objects,
							NUM_SESSIONS_TO_HOST * pTNecd->iNumMachines * sizeof (PVOID));

		LOCALALLOC_OR_THROW(DPNID**, papadpnidTesters,
							NUM_SESSIONS_TO_HOST * pTNecd->iNumMachines * sizeof (DPNID*));

		// Allocate an array that will hold the tester numbers in varying orders
		// for each particular session.
		LOCALALLOC_OR_THROW(int*, paiTesters,
							pTNecd->iNumMachines * sizeof (int));




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating sessions");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		for(i = 0; i < NUM_SESSIONS_TO_HOST * pTNecd->iNumMachines; i++)
		{
			// Set the tester number order for this particular session.
			for(j = 0; j < pTNecd->iNumMachines; j++)
			{
				paiTesters[j] = (i + j) % pTNecd->iNumMachines;
			} // end for (each tester)


			DPL(0, "Creating peer-to-peer session %i (will %s).",
				2, i,
				(((i % pTNecd->iNumMachines) == pTNecd->iTesterNum) ? "host" : "join"));

			pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
									"Creating peer session %i (will %s).",
									2, i,
									(((i % pTNecd->iNumMachines) == pTNecd->iTesterNum) ? "host" : "join"));

			ZeroMemory(&context, sizeof (BLDDP8DPNHANDLERCONTEXTHEADER));
			context.pfnDPNMessageHandler = MultiSsnDPNMessageHandler;

			ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
			dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
			dpnad.dwFlags = 0;
			//dpnad.guidInstance = GUID_NULL;
			dpnad.guidApplication = GUID_MULTISSN_SIMPLE_PEER;
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

			sr = pTNecd->pExecutor->ExecSubTestCaseArray("3.1.1.1",
														pvSubInputData,
														sizeof (TID_BBLDDP8_ALL_CREATE),
														pTNecd->iNumMachines,
														paiTesters);

			LocalFree(pvSubInputData);
			pvSubInputData = NULL;

			if (sr != S_OK)
			{
				DPL(0, "Couldn't execute sub test case BldSsn:Create!", 0);
				THROW_SYSTEMRESULT;
			} // end if (failed executing sub test case)


			GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.1.1",
										"Creating peer-to-peer session failed!");

			// Otherwise get the object created.
			CHECKANDGET_SUBOUTPUTDATA(pSubResult,
										pBldDP8PeerCreateOutput,
										dwSubOutputDataSize,
										(sizeof (TOD_BBLDDP8_PEER_CREATE) + (pTNecd->iNumMachines * sizeof (DPNID))));

			// We're using it during this function, so add a ref.
			pBldDP8PeerCreateOutput->pDP8Peer->m_dwRefCount++;
			papvDP8Objects[i] = pBldDP8PeerCreateOutput->pDP8Peer;

			papadpnidTesters[i] = (DPNID*) (pBldDP8PeerCreateOutput + 1);
		} // end for (each session)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for everyone to be ready to close");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->SyncWithTesters("Ready to close", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Destroying sessions");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		for(i = 0; i < NUM_SESSIONS_TO_HOST * pTNecd->iNumMachines; i++)
		{
			// Set the tester number order for this particular session.
			for(j = 0; j < pTNecd->iNumMachines; j++)
			{
				paiTesters[j] = (i + j) % pTNecd->iNumMachines;
			} // end for (each tester)



			DPL(0, "Destroying peer-to-peer session %i (was %s).",
				2, i,
				(((i % pTNecd->iNumMachines) == pTNecd->iTesterNum) ? "host" : "joiner"));

			pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
									"Destroying peer-to-peer session %i (was %s).",
									2, i,
									(((i % pTNecd->iNumMachines) == pTNecd->iTesterNum) ? "host" : "joiner"));


			LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_BBLDDP8_PEER_DESTROY));
			((PTID_BBLDDP8_PEER_DESTROY) pvSubInputData)->pDP8Peer = (PWRAPDP8PEER) papvDP8Objects[i];
			((PTID_BBLDDP8_PEER_DESTROY) pvSubInputData)->pHandlerContext = &context;
			((PTID_BBLDDP8_PEER_DESTROY) pvSubInputData)->padpnidTesters = papadpnidTesters[i];

			sr = pTNecd->pExecutor->ExecSubTestCaseArray("3.1.1.2",
														pvSubInputData,
														sizeof (TID_BBLDDP8_PEER_DESTROY),
														pTNecd->iNumMachines,
														paiTesters);

			LocalFree(pvSubInputData);
			pvSubInputData = NULL;

			if (sr != S_OK)
			{
				DPL(0, "Couldn't execute sub test case BldSsn:BldPeer:Destroy!", 0);
				THROW_SYSTEMRESULT;
			} // end if (failed executing sub test case)

			GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.1.2",
										"Destroying peer-to-peer session failed!");



			((PWRAPDP8PEER) papvDP8Objects[i])->m_dwRefCount--;
			if (((PWRAPDP8PEER) papvDP8Objects[i])->m_dwRefCount == 0)
			{
				DPL(7, "Deleting peer wrapper object %i (%x).",
					2, i, papvDP8Objects[i]);
				delete ((PWRAPDP8PEER) papvDP8Objects[i]);
			} // end if (can delete the object)
			else
			{
				DPL(0, "WARNING: Can't delete peer wrapper object %i (%x), it's refcount is %u!?",
					3, i, papvDP8Objects[i],
					((PWRAPDP8PEER) papvDP8Objects[i])->m_dwRefCount);
			} // end else (can't delete the object)
			papvDP8Objects[i] = NULL;
		} // end for (each session)


		LocalFree(papvDP8Objects);
		papvDP8Objects = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE



	SAFE_LOCALFREE(pvSubInputData);
	SAFE_LOCALFREE(papadpnidTesters);
	SAFE_LOCALFREE(paiTesters);

	if (papvDP8Objects != NULL)
	{
		for(i = 0; i < NUM_SESSIONS_TO_HOST * pTNecd->iNumMachines; i++)
		{
			if (papvDP8Objects[i] != NULL)
			{
				temphr = ((PWRAPDP8PEER) papvDP8Objects[i])->DP8P_Close(0);
				if (temphr != DPN_OK)
				{
					DPL(0, "Closing peer interface %i failed!  0x%08x", 2, i, temphr);
					OVERWRITE_SR_IF_OK(temphr);
				} // end if (closing peer interface failed)


				// Ignore error, it may not actually be on the list.
				g_pDP8PeersList->RemoveFirstReference((PWRAPDP8PEER) papvDP8Objects[i]);


				((PWRAPDP8PEER) papvDP8Objects[i])->m_dwRefCount--;
				if (((PWRAPDP8PEER) papvDP8Objects[i])->m_dwRefCount == 0)
				{
					DPL(7, "Deleting peer wrapper object %i (%x).",
						2, i, papvDP8Objects[i]);
					delete ((PWRAPDP8PEER) papvDP8Objects[i]);
				} // end if (can delete the object)
				else
				{
					DPL(0, "WARNING: Can't delete peer wrapper object %i (%x), it's refcount is %u!?",
						3, i, papvDP8Objects[i],
						((PWRAPDP8PEER) papvDP8Objects[i])->m_dwRefCount);
				} // end else (can't delete the object)

				papvDP8Objects[i] = NULL;
			} // end if (have object)
		} // end for (each session)

		LocalFree(papvDP8Objects);
		papvDP8Objects = NULL;
	} // end if (there are sessions possibly)

	return (sr);
} // MultiSsnExec_SimplePeer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"MultiSsnExec_SimpleCS()"
//==================================================================================
// MultiSsnExec_SimpleCS
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.6.2 - Simple client/server multiple sessions test
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
HRESULT MultiSsnExec_SimpleCS(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	HRESULT							temphr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PTOD_BBLDDP8_CS_CREATE_0		pBldDP8CSCreateOutput0 = NULL;
	PTOD_BBLDDP8_CS_CREATE_NOT0		pBldDP8CSCreateOutputNot0 = NULL;
	DWORD							dwSubOutputDataSize;
	BLDDP8DPNHANDLERCONTEXTHEADER	context;
	DPN_APPLICATION_DESC			dpnad;
	PVOID*							papvDP8Objects = NULL;
	DPNID**							papadpnidTesters = NULL;
	int*							paiTesters = NULL;
	BOOL							fClientServer = FALSE;
	int								i;
	int								j;



	BEGIN_TESTCASE
	{
		// Allocate the arrays to hold pointers to all the objects we'll generate.
		LOCALALLOC_OR_THROW(PVOID*, papvDP8Objects,
							NUM_SESSIONS_TO_HOST * pTNecd->iNumMachines * sizeof (PVOID));

		LOCALALLOC_OR_THROW(DPNID**, papadpnidTesters,
							NUM_SESSIONS_TO_HOST * pTNecd->iNumMachines * sizeof (DPNID*));

		// Allocate an array that will hold the tester numbers in varying orders
		// for each particular session.
		LOCALALLOC_OR_THROW(int*, paiTesters,
							pTNecd->iNumMachines * sizeof (int));



		if (pTNecd->pExecutor->IsCase("2.6.2"))
			fClientServer = TRUE;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating sessions");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		for(i = 0; i < NUM_SESSIONS_TO_HOST * pTNecd->iNumMachines; i++)
		{
			// Set the tester number order for this particular session.
			for(j = 0; j < pTNecd->iNumMachines; j++)
			{
				paiTesters[j] = (i + j) % pTNecd->iNumMachines;
			} // end for (each tester)


			DPL(0, "Creating client/server session %i (will %s).",
				2, i,
				(((i % pTNecd->iNumMachines) == pTNecd->iTesterNum) ? "host" : "join"));

			pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
									"Creating client/server session %i (will %s).",
									2, i,
									(((i % pTNecd->iNumMachines) == pTNecd->iTesterNum) ? "host" : "join"));

			ZeroMemory(&context, sizeof (BLDDP8DPNHANDLERCONTEXTHEADER));
			context.pfnDPNMessageHandler = MultiSsnDPNMessageHandler;

			ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
			dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
			dpnad.dwFlags = DPNSESSION_CLIENT_SERVER;
			//dpnad.guidInstance = GUID_NULL;
			dpnad.guidApplication = GUID_MULTISSN_SIMPLE_CS;
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

			sr = pTNecd->pExecutor->ExecSubTestCaseArray("3.1.2.1",
														pvSubInputData,
														sizeof (TID_BBLDDP8_ALL_CREATE),
														pTNecd->iNumMachines,
														paiTesters);

			LocalFree(pvSubInputData);
			pvSubInputData = NULL;

			if (sr != S_OK)
			{
				DPL(0, "Couldn't execute sub test case BldSsn:Create!", 0);
				THROW_SYSTEMRESULT;
			} // end if (failed executing sub test case)

			GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.2.1",
										"Creating client/server session failed!");

			// Otherwise get the object created.
			CHECKANDGET_SUBOUTPUTDATA(pSubResult,
										pBldDP8CSCreateOutput0,
										dwSubOutputDataSize,
										(sizeof (TOD_BBLDDP8_PEER_CREATE) + (pTNecd->iNumMachines * sizeof (DPNID))));

			if ((i % pTNecd->iNumMachines) == pTNecd->iTesterNum)
			{
				// We're using it during this function, so add a ref.
				pBldDP8CSCreateOutput0->pDP8Server->m_dwRefCount++;
				papvDP8Objects[i] = pBldDP8CSCreateOutput0->pDP8Server;

				papadpnidTesters[i] = (DPNID*) (pBldDP8CSCreateOutput0 + 1);
			} // end if (server)
			else
			{
				pBldDP8CSCreateOutputNot0 = (PTOD_BBLDDP8_CS_CREATE_NOT0) pBldDP8CSCreateOutput0;

				// We're using it during this function, so add a ref.
				pBldDP8CSCreateOutputNot0->pDP8Client->m_dwRefCount++;
				papvDP8Objects[i] = pBldDP8CSCreateOutputNot0->pDP8Client;

				papadpnidTesters[i] = (DPNID*) (pBldDP8CSCreateOutputNot0 + 1);
			} // end else (client)
		} // end for (each session)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for everyone to be ready to close");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->SyncWithTesters("Ready to close", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Destroying sessions");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		for(i = 0; i < NUM_SESSIONS_TO_HOST * pTNecd->iNumMachines; i++)
		{
			// Set the tester number order for this particular session.
			for(j = 0; j < pTNecd->iNumMachines; j++)
			{
				paiTesters[j] = (i + j) % pTNecd->iNumMachines;
			} // end for (each tester)



			DPL(0, "Destroying client/server session %i (was %s).",
				2, i,
				(((i % pTNecd->iNumMachines) == pTNecd->iTesterNum) ? "host" : "joiner"));

			pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
									"Destroying client/server session %i (was %s).",
									2, i,
									(((i % pTNecd->iNumMachines) == pTNecd->iTesterNum) ? "host" : "joiner"));


			if ((i % pTNecd->iNumMachines) == pTNecd->iTesterNum)
			{
				LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_BBLDDP8_CS_DESTROY_0));
				((PTID_BBLDDP8_CS_DESTROY_0) pvSubInputData)->pDP8Server = (PWRAPDP8SERVER) papvDP8Objects[i];
				((PTID_BBLDDP8_CS_DESTROY_0) pvSubInputData)->pHandlerContext = &context;
				((PTID_BBLDDP8_CS_DESTROY_0) pvSubInputData)->padpnidTesters = papadpnidTesters[i];

				sr = pTNecd->pExecutor->ExecSubTestCaseArray("3.1.2.2",
															pvSubInputData,
															sizeof (TID_BBLDDP8_CS_DESTROY_0),
															pTNecd->iNumMachines,
															paiTesters);

				LocalFree(pvSubInputData);
				pvSubInputData = NULL;

				if (sr != S_OK)
				{
					DPL(0, "Couldn't execute sub test case BldSsn:BldCS:Destroy!", 0);
					THROW_SYSTEMRESULT;
				} // end if (failed executing sub test case)

				GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.2.2",
											"Destroying client/server session failed!");



				((PWRAPDP8SERVER) papvDP8Objects[i])->m_dwRefCount--;
				if (((PWRAPDP8SERVER) papvDP8Objects[i])->m_dwRefCount == 0)
				{
					DPL(7, "Deleting server wrapper object %i (%x).",
						2, i, papvDP8Objects[i]);
					delete ((PWRAPDP8SERVER) papvDP8Objects[i]);
				} // end if (can delete the object)
				else
				{
					DPL(0, "WARNING: Can't delete server wrapper object %i (%x), it's refcount is %u!?",
						3, i, papvDP8Objects[i],
						((PWRAPDP8SERVER) papvDP8Objects[i])->m_dwRefCount);
				} // end else (can't delete the object)
				papvDP8Objects[i] = NULL;
			} // end if (server)
			else
			{
				LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_BBLDDP8_CS_DESTROY_NOT0));
				((PTID_BBLDDP8_CS_DESTROY_NOT0) pvSubInputData)->pDP8Client = (PWRAPDP8CLIENT) papvDP8Objects[i];

				sr = pTNecd->pExecutor->ExecSubTestCaseArray("3.1.2.2",
															pvSubInputData,
															sizeof (TID_BBLDDP8_CS_DESTROY_NOT0),
															pTNecd->iNumMachines,
															paiTesters);

				LocalFree(pvSubInputData);
				pvSubInputData = NULL;

				if (sr != S_OK)
				{
					DPL(0, "Couldn't execute sub test case BldSsn:BldCS:Destroy!", 0);
					THROW_SYSTEMRESULT;
				} // end if (failed executing sub test case)

				GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.2.2",
											"Destroying client/server session failed!");



				((PWRAPDP8CLIENT) papvDP8Objects[i])->m_dwRefCount--;
				if (((PWRAPDP8CLIENT) papvDP8Objects[i])->m_dwRefCount == 0)
				{
					DPL(7, "Deleting client wrapper object %i (%x).",
						2, i, papvDP8Objects[i]);
					delete ((PWRAPDP8CLIENT) papvDP8Objects[i]);
				} // end if (can delete the object)
				else
				{
					DPL(0, "WARNING: Can't delete client wrapper object %i (%x), it's refcount is %u!?",
						3, i, papvDP8Objects[i],
						((PWRAPDP8CLIENT) papvDP8Objects[i])->m_dwRefCount);
				} // end else (can't delete the object)
				papvDP8Objects[i] = NULL;
			} // end else (client)
		} // end for (each session)


		LocalFree(papvDP8Objects);
		papvDP8Objects = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE



	SAFE_LOCALFREE(pvSubInputData);
	SAFE_LOCALFREE(papadpnidTesters);
	SAFE_LOCALFREE(paiTesters);

	if (papvDP8Objects != NULL)
	{
		for(i = 0; i < NUM_SESSIONS_TO_HOST * pTNecd->iNumMachines; i++)
		{
			if (papvDP8Objects[i] != NULL)
			{
				if ((i % pTNecd->iNumMachines) == pTNecd->iTesterNum)
				{
					temphr = ((PWRAPDP8SERVER) papvDP8Objects[i])->DP8S_Close(0);
					if (temphr != DPN_OK)
					{
						DPL(0, "Closing server interface %i failed!  0x%08x", 2, i, temphr);
						OVERWRITE_SR_IF_OK(temphr);
					} // end if (closing server interface failed)


					// Ignore error, it may not actually be on the list.
					g_pDP8ServersList->RemoveFirstReference((PWRAPDP8SERVER) papvDP8Objects[i]);


					((PWRAPDP8SERVER) papvDP8Objects[i])->m_dwRefCount--;
					if (((PWRAPDP8SERVER) papvDP8Objects[i])->m_dwRefCount == 0)
					{
						DPL(7, "Deleting server wrapper object %i (%x).",
							2, i, papvDP8Objects[i]);
						delete ((PWRAPDP8SERVER) papvDP8Objects[i]);
					} // end if (can delete the object)
					else
					{
						DPL(0, "WARNING: Can't delete server wrapper object %i (%x), it's refcount is %u!?",
							3, i, papvDP8Objects[i],
							((PWRAPDP8SERVER) papvDP8Objects[i])->m_dwRefCount);
					} // end else (can't delete the object)
				} // end if (server)
				else
				{
					temphr = ((PWRAPDP8CLIENT) papvDP8Objects[i])->DP8C_Close(0);
					if (temphr != DPN_OK)
					{
						DPL(0, "Closing client interface %i failed!  0x%08x", 2, i, temphr);
						OVERWRITE_SR_IF_OK(temphr);
					} // end if (closing client interface failed)


					// Ignore error, it may not actually be on the list.
					g_pDP8ClientsList->RemoveFirstReference((PWRAPDP8CLIENT) papvDP8Objects[i]);


					((PWRAPDP8CLIENT) papvDP8Objects[i])->m_dwRefCount--;
					if (((PWRAPDP8CLIENT) papvDP8Objects[i])->m_dwRefCount == 0)
					{
						DPL(7, "Deleting client wrapper object %i (%x).",
							2, i, papvDP8Objects[i]);
						delete ((PWRAPDP8CLIENT) papvDP8Objects[i]);
					} // end if (can delete the object)
					else
					{
						DPL(0, "WARNING: Can't delete client wrapper object %i (%x), it's refcount is %u!?",
							3, i, papvDP8Objects[i],
							((PWRAPDP8CLIENT) papvDP8Objects[i])->m_dwRefCount);
					} // end else (can't delete the object)
				} // end else (client)

				papvDP8Objects[i] = NULL;
			} // end if (have object)
		} // end for (each session)

		LocalFree(papvDP8Objects);
		papvDP8Objects = NULL;
	} // end if (there are sessions possibly)

	return (sr);
} // MultiSsnExec_SimpleCS
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"MultiSsnDPNMessageHandler()"
//==================================================================================
// MultiSsnDPNMessageHandler
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
HRESULT MultiSsnDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT							hr = DPN_OK;
	PBLDDP8DPNHANDLERCONTEXTHEADER	pContext = (PBLDDP8DPNHANDLERCONTEXTHEADER) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_DESTROY_PLAYER:
			DPL(0, "WARNING: Ignoring DPN_MSGID_DESTROY_PLAYER message, this had better be because of a test failure!", 0); 
		  break;

		default:
			DPL(0, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	return (hr);
} // MultiSsnDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
