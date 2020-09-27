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

#include "hostmigrate.h"




//==================================================================================
// Defines
//==================================================================================
#define GROUPORDATAOP_NONE						-1

#define GROUPORDATAOP_SETPEERINFO				1
#define GROUPORDATAOP_CREATEGROUP				2
#define GROUPORDATAOP_SETGROUPINFO				3
#define GROUPORDATAOP_ADDPLAYERTOGROUP			4
#define GROUPORDATAOP_REMOVEPLAYERFROMGROUP		5
#define GROUPORDATAOP_DESTROYGROUP				6




//==================================================================================
// Structures
//==================================================================================
typedef struct tagHOSTMIGRATECONTEXT:public BLDDP8DPNHANDLERCONTEXTHEADER
{
	// See blddp8.h BLDDP8DPNHANDLERCONTEXTHEADER for first members of structure

	PTNEXECCASEDATA		pTNecd; // pointer to EXECCASEDATA passed in to test
	HANDLE				hPlayersGoneEvent; // event to set when all other players have been removed from session when expected
	BOOL				fSelfIndicate; // set to TRUE if expecting our own player to connect/disconnect, FALSE if not
	HANDLE				hHostMigratedEvent; // event to set when host migration message comes in
	DPNID*				padpnidTesters; // array of player IDs, in tester number order
	int					iCurrentHost; // tester number who was original host for this iteration
	BOOL				fDroppingPlayer; // whether we're a dropping player or not
	BOOL				fSecondInCommandCase; // whether this is a second-in-command migration case
	int					iGroupOrDataOperation; // particular group or data operation that will be performed, or -1 for none
	BOOL				fGroupOrDataOperatingPlayer; // whether we're the player performing the group or data operation or not
	PWRAPDP8PEER		pDP8Peer; // DPlay8 peer object in use
	BOOL*				pafTesterIndicated; // pointer to array to use to remember whether an indication has already arrived for this tester
	HANDLE				hRejoinedEvent; // event to set when tester has rejoined session after migration; round robin only
	int*				paiReliableMsgsReceivedFromTesters; // pointer to array to use when setting player contexts while rejoining; round robin only
	HANDLE				hGroupOrDataOperationEvent; // event to set when group or data operation is indicated
	DPNID				dpnidGroup; // ID of group created
	DPNID				dpnidPlayerAddedToGroup; // ID of player added to group
	volatile BOOL		fNoAsyncOpCompletion; // boolean indicating whether the group or data operation failed to get started and that no async op would be forthcoming
	BOOL				fPeerInfoSet; // whether the peer info data indication arrived or not
	BOOL				fGroupInfoSet; // whether the peer info data indication arrived or not
} HOSTMIGRATECONTEXT, * PHOSTMIGRATECONTEXT;





//==================================================================================
// Prototypes
//==================================================================================
HRESULT HostMigrateCanRun_All(PTNCANRUNDATA pTNcrd);
HRESULT HostMigrateGet_RoundRobin(PTNGETINPUTDATA pTNgid);
HRESULT HostMigrateWrite_RoundRobin(PTNWRITEDATA pTNwd);
HRESULT HostMigrateExec_All(PTNEXECCASEDATA pTNecd);


HRESULT HostMigrateDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);





#undef DEBUG_SECTION
#define DEBUG_SECTION	"HostMigrateLoadTestTable()"
//==================================================================================
// HostMigrateLoadTestTable
//----------------------------------------------------------------------------------
//
// Description: Loads all the possible tests into the table passed in:
//				2.3			Host migration tests
//				2.3.1		Normal host migration test cases
//				2.3.1.1		Simple 3 player normal host migration test
//				2.3.1.2		Round robin normal host migration test
//				2.3.1.3		Round robin normal host and second-in-command migration test
//				2.3.1.4		Round robin group and data operations during normal host migration test
//				2.3.1.5		Round robin group and data operations during normal host and second-in-command migration test
//				2.3.2		Dropped host migration test cases
//				2.3.2.1		Simple 3 player dropped host migration test
//				2.3.2.2		Round robin dropped host migration test
//				2.3.2.3		Round robin dropped host and second-in-command migration test
//				2.3.2.4		Round robin group and data operations during dropped host migration test
//				2.3.2.5		Round robin group and data operations during dropped host and second-in-command migration test
//
// Arguments:
//	PTNLOADTESTTABLEDATA pTNlttd	Pointer to data to use when loading the tests.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT HostMigrateLoadTestTable(PTNLOADTESTTABLEDATA pTNlttd)
{
	PTNTESTTABLEGROUP	pSubGroup;
	PTNTESTTABLEGROUP	pSubSubGroup;
	TNADDTESTDATA		tnatd;



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3		Host migration tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pTNlttd->pBase, "2.3", "Host migration tests", &pSubGroup);




	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3.1	Normal host migration test cases
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pSubGroup, "2.3.1", "Normal host migration test cases",
				&pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3.1.1	Simple 3 player normal host migration test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.3.1.1";
	tnatd.pszName			= "Simple 3 player normal host migration test";
	tnatd.pszDescription	= "Simple 3 player normal host migration test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 3;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= HostMigrateCanRun_All;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= HostMigrateExec_All;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3.1.2	Round robin normal host migration test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.3.1.2";
	tnatd.pszName			= "Round robin normal host migration test";
	tnatd.pszDescription	= "Round robin normal host migration test";
	tnatd.pszInputDataHelp	= "NumIterations = How many times to migrate host";

	tnatd.iNumMachines		= -3;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= HostMigrateCanRun_All;
	tnatd.pfnGetInputData	= HostMigrateGet_RoundRobin;
	tnatd.pfnExecCase		= HostMigrateExec_All;
	tnatd.pfnWriteData		= HostMigrateWrite_RoundRobin;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3.1.3	Round robin normal host and second-in-command migration test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.3.1.3";
	tnatd.pszName			= "Round robin normal host and second-in-command migration test";
	tnatd.pszDescription	= "Round robin normal host and second-in-command migration test";
	tnatd.pszInputDataHelp	= "NumIterations = How many times to migrate host";

	tnatd.iNumMachines		= -4;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= HostMigrateCanRun_All;
	tnatd.pfnGetInputData	= HostMigrateGet_RoundRobin;
	tnatd.pfnExecCase		= HostMigrateExec_All;
	tnatd.pfnWriteData		= HostMigrateWrite_RoundRobin;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3.1.4	Round robin group and data operations during normal host migration test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.3.1.4";
	tnatd.pszName			= "Round robin group and data operations during normal host migration test";
	tnatd.pszDescription	= "Round robin group and data operations during normal host migration test";
	tnatd.pszInputDataHelp	= "NumIterations = How many times to migrate host";

	tnatd.iNumMachines		= -3;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= HostMigrateCanRun_All;
	tnatd.pfnGetInputData	= HostMigrateGet_RoundRobin;
	tnatd.pfnExecCase		= HostMigrateExec_All;
	tnatd.pfnWriteData		= HostMigrateWrite_RoundRobin;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3.1.5	Round robin group and data operations during normal host and second-in-command migration test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.3.1.5";
	tnatd.pszName			= "Round robin group and data operations during normal host and second-in-command migration test";
	tnatd.pszDescription	= "Round robin group and data operations during normal host and second-in-command migration test";
	tnatd.pszInputDataHelp	= "NumIterations = How many times to migrate host";

	tnatd.iNumMachines		= -4;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= HostMigrateCanRun_All;
	tnatd.pfnGetInputData	= HostMigrateGet_RoundRobin;
	tnatd.pfnExecCase		= HostMigrateExec_All;
	tnatd.pfnWriteData		= HostMigrateWrite_RoundRobin;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);





	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3.2	Dropped host migration test cases
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pSubGroup, "2.3.2", "Dropped host migration test cases",
				&pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3.2.1	Simple 3 player dropped host migration test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.3.2.1";
	tnatd.pszName			= "Simple 3 player dropped host migration test";
	tnatd.pszDescription	= "Simple 3 player dropped host migration test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 3;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= HostMigrateCanRun_All;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= HostMigrateExec_All;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3.2.2	Round robin dropped host migration test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.3.2.2";
	tnatd.pszName			= "Round robin dropped host migration test";
	tnatd.pszDescription	= "Round robin dropped host migration test";
	tnatd.pszInputDataHelp	= "NumIterations = How many times to migrate host";

	tnatd.iNumMachines		= -3;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= HostMigrateCanRun_All;
	tnatd.pfnGetInputData	= HostMigrateGet_RoundRobin;
	tnatd.pfnExecCase		= HostMigrateExec_All;
	tnatd.pfnWriteData		= HostMigrateWrite_RoundRobin;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3.2.3	Round robin dropped host and second-in-command migration test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.3.2.3";
	tnatd.pszName			= "Round dropped normal host and second-in-command migration test";
	tnatd.pszDescription	= "Round dropped normal host and second-in-command migration test";
	tnatd.pszInputDataHelp	= "NumIterations = How many times to migrate host";

	tnatd.iNumMachines		= -4;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= HostMigrateCanRun_All;
	tnatd.pfnGetInputData	= HostMigrateGet_RoundRobin;
	tnatd.pfnExecCase		= HostMigrateExec_All;
	tnatd.pfnWriteData		= HostMigrateWrite_RoundRobin;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3.2.4	Round robin group and data operations during dropped host migration test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.3.2.4";
	tnatd.pszName			= "Round robin group and data operations during dropped host migration test";
	tnatd.pszDescription	= "Round robin group and data operations during dropped host migration test";
	tnatd.pszInputDataHelp	= "NumIterations = How many times to migrate host";

	tnatd.iNumMachines		= -3;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= HostMigrateCanRun_All;
	tnatd.pfnGetInputData	= HostMigrateGet_RoundRobin;
	tnatd.pfnExecCase		= HostMigrateExec_All;
	tnatd.pfnWriteData		= HostMigrateWrite_RoundRobin;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.3.1.5	Round robin group and data operations during dropped host and second-in-command migration test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.3.1.5";
	tnatd.pszName			= "Round robin group and data operations during dropped host and second-in-command migration test";
	tnatd.pszDescription	= "Round robin group and data operations during dropped host and second-in-command migration test";
	tnatd.pszInputDataHelp	= "NumIterations = How many times to migrate host";

	tnatd.iNumMachines		= -4;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= HostMigrateCanRun_All;
	tnatd.pfnGetInputData	= HostMigrateGet_RoundRobin;
	tnatd.pfnExecCase		= HostMigrateExec_All;
	tnatd.pfnWriteData		= HostMigrateWrite_RoundRobin;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);


	return (S_OK);
} // HostMigrateLoadTestTable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"HostMigrateCanRun_All()"
//==================================================================================
// HostMigrateCanRun_All
//----------------------------------------------------------------------------------
//
// Description: Callback that checks a possible tester list to make sure the test
//				can be run correctly for the following test case(s):
//				2.3.1.1 - Simple 3 player normal host migration test
//				2.3.1.2 - Round robin normal host migration test
//				2.3.1.3 - Round robin normal host and second-in-command migration test
//				2.3.1.4 - Round robin group and data operations during normal host migration test
//				2.3.1.5 - Round robin group and data operations during normal host and second-in-command migration test
//				2.3.2.1 - Simple 3 player dropped host migration test
//				2.3.2.2 - Round robin dropped host migration test
//				2.3.2.3 - Round robin dropped host and second-in-command migration test
//				2.3.2.4 - Round robin group and data operations during dropped host migration test
//				2.3.2.5 - Round robin group and data operations during dropped host and second-in-command migration test
//
// Arguments:
//	PTNCANRUNDATA pTNcrd	Pointer to parameter block with information on what
//							configuration to check.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT HostMigrateCanRun_All(PTNCANRUNDATA pTNcrd)
{
	HRESULT		hr;
	int			i;
	int			j;


	// Loop through all the testers to make sure they can connect to each other.
	for(i = 0; i < pTNcrd->iNumMachines; i++)
	{
		// Only the first host needs to have IMTest installed in the simple (i.e
		// only 1 time) drop test case.  All testers need it in the round robin
		// drop cases.
		if (((strcmp(pTNcrd->pTest->m_pszID, "2.3.2.1") == 0) && (i == 0)) ||
			(strcmp(pTNcrd->pTest->m_pszID, "2.3.2.2") == 0) ||
			(strcmp(pTNcrd->pTest->m_pszID, "2.3.2.3") == 0) ||
			(strcmp(pTNcrd->pTest->m_pszID, "2.3.2.4") == 0) ||
			(strcmp(pTNcrd->pTest->m_pszID, "2.3.2.5") == 0))
		{
			pTNcrd->fCanRun = pTNcrd->apTesters[i]->m_fFaultSimIMTestAvailable;

			if (! pTNcrd->fCanRun)
			{
				DPL(1, "Tester %i (%s) doesn't have IMTest fault simulator available.",
					2, i, pTNcrd->apTesters[i]->m_szComputerName);

				return (S_OK);
			} // end if (tester couldn't reach other tester)
		} // end if (first tester or round robin)


		for(j = 0; j < pTNcrd->iNumMachines; j++)
		{
			// Testers don't need to connect to themselves.
			if (i == j)
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
						j, pTNcrd->apTesters[j]->m_szComputerName);

					return (S_OK);
				} // end if (tester couldn't reach other tester)
			} // end else (not on same machine)
		} // end for (each tester)
	} // end for (each tester)


	return (S_OK);
} // HostMigrateCanRun_All
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"HostMigrateGet_RoundRobin()"
//==================================================================================
// HostMigrateGet_RoundRobin
//----------------------------------------------------------------------------------
//
// Description: Callback that retrieves the initial data for the test case(s):
//				2.3.1.2 - Round robin normal host migration test
//				2.3.1.3 - Round robin normal host and second-in-command migration test
//				2.3.1.4 - Round robin group and data operations during normal host migration test
//				2.3.1.5 - Round robin group and data operations during normal host and second-in-command migration test
//				2.3.2.2 - Round robin dropped host migration test
//				2.3.2.3 - Round robin dropped host and second-in-command migration test
//				2.3.2.4 - Round robin group and data operations during dropped host migration test
//				2.3.2.5 - Round robin group and data operations during dropped host and second-in-command migration test
//
// Arguments:
//	PTNGETINPUTDATA pTNgid	Pointer to parameter block with information on how and
//							where to place the input data.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT HostMigrateGet_RoundRobin(PTNGETINPUTDATA pTNgid)
{
	PTID_PHOSTMIGRATE_ROUNDROBIN	pParams = (PTID_PHOSTMIGRATE_ROUNDROBIN) pTNgid->pvData;
	PLSTRINGLIST					pStrings = NULL;
	char*							pszTemp;



	pTNgid->dwDataSize = sizeof (TID_PHOSTMIGRATE_ROUNDROBIN);

	if (pParams == NULL)
		return (ERROR_BUFFER_TOO_SMALL);


	// Start with default values
	pParams->iNumIterations = 5;
    

	// Get any input data strings that might exist.
	GETANYINPUTSTRINGS(pTNgid, pStrings);


	// Okay, so if we found strings, check if any of the options are specified.
	// If they are, override the default with the items' value.
	if (pStrings != NULL)
	{
		pszTemp = pStrings->GetValueOfKey("NumIterations");
		if (pszTemp != NULL)
			pParams->iNumIterations = StringToInt(pszTemp);
	} // end if (we found some items)


	return (S_OK);
} // HostMigrateGet_RoundRobin
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"HostMigrateWrite_RoundRobin()"
//==================================================================================
// HostMigrateWrite_RoundRobin
//----------------------------------------------------------------------------------
//
// Description: Callback that converts a buffer to strings for the test case(s):
//				2.3.1.2 - Round robin normal host migration test
//				2.3.1.3 - Round robin normal host and second-in-command migration test
//				2.3.1.4 - Round robin group and data operations during normal host migration test
//				2.3.1.5 - Round robin group and data operations during normal host and second-in-command migration test
//				2.3.2.2 - Round robin dropped host migration test
//				2.3.2.3 - Round robin dropped host and second-in-command migration test
//				2.3.2.4 - Round robin group and data operations during dropped host migration test
//				2.3.2.5 - Round robin group and data operations during dropped host and second-in-command migration test
//
// Arguments:
//	PTNWRITEDATA pTNwd	Parameter block with information and data to write.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT HostMigrateWrite_RoundRobin(PTNWRITEDATA pTNwd)
{
	PTID_PHOSTMIGRATE_ROUNDROBIN	pInputData = (PTID_PHOSTMIGRATE_ROUNDROBIN) pTNwd->pvData;

	
	switch (pTNwd->dwLocation)
	{
		case TNTSDL_REPORTASSIGN:
			REQUIRE_ASSIGNREPORT_DATA(sizeof (TID_PHOSTMIGRATE_ROUNDROBIN));


			WRITESPRINTFDATALINE("NumIterations= %i", 1, pInputData->iNumIterations);
		  break;

		default:
			DPL(0, "Got data for unexpected location %u!",
				1, pTNwd->dwLocation);
			return (ERROR_INVALID_PARAMETER);
		  break;
	} // end switch (on the source of the data buffer)

	
	return (S_OK);
} // HostMigrateWrite_RoundRobin
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"HostMigrateExec_All()"
//==================================================================================
// HostMigrateExec_All
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.3.1.1 - Simple 3 player normal host migration test
//				2.3.1.2 - Round robin normal host migration test
//				2.3.1.3 - Round robin normal host and second-in-command migration test
//				2.3.1.4 - Round robin group and data operations during normal host migration test
//				2.3.1.5 - Round robin group and data operations during normal host and second-in-command migration test
//				2.3.2.1 - Simple 3 player dropped host migration test
//				2.3.2.2 - Round robin dropped host migration test
//				2.3.2.3 - Round robin dropped host and second-in-command migration test
//				2.3.2.4 - Round robin group and data operations during dropped host migration test
//				2.3.2.5 - Round robin group and data operations during dropped host and second-in-command migration test
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
HRESULT HostMigrateExec_All(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	HRESULT							temphr;
	PTID_PHOSTMIGRATE_ROUNDROBIN	pRoundRobinInput = (PTID_PHOSTMIGRATE_ROUNDROBIN) pTNecd->pvInputData;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	DWORD							dwSubInputDataSize;
	PTOD_BBLDDP8_PEER_CREATE		pBldDP8PeerCreateOutput = NULL;
	DWORD							dwSubOutputDataSize;
	CTNSyncDataList					syncdata;
	PVOID							pvSyncData;
	HOSTMIGRATECONTEXT				context;
	DPN_APPLICATION_DESC			dpnad;
	DPN_PLAYER_INFO					dpnpi;
	DPN_GROUP_INFO					dpngi;
	PWRAPDP8PEER					pDP8Peer = NULL;
	PTNFAULTSIM						pFaultSim = NULL;
	int*							paiOtherTesters = NULL;
	int*							paiReliableMsgsSentToTesters = NULL;
	int								iCurrentIteration;
	DWORD							dwSyncDataSize;
	int*							paiReliableMsgsSentByTester;
	DPN_BUFFER_DESC					dpnbdUnreliable;
	DPN_BUFFER_DESC					dpnbdReliable;
	int								i;
	int								j;
	DWORD							dwStartTime;
	DPNHANDLE						dpnhSend;
	DPNHANDLE						dpnhConnect;
	DWORD							dwNumMsgs;
	PDPNID							padpnidDestroyTesters = NULL;
	PDIRECTPLAY8ADDRESS				pDP8AddressRemote = NULL;
	PDIRECTPLAY8ADDRESS				pDP8AddressLocal = NULL;
	HANDLE							hConnectCompleteEvent = NULL;
	int								iNextHost;
	DPNHANDLE						dpnhGroupOrDataOp;
#ifdef DEBUG
	char*							pszURL = NULL;
	DWORD							dwURLBufferSize = 0;
	DWORD							dwURLSize;
#endif // DEBUG




	ZeroMemory(&context, sizeof (HOSTMIGRATECONTEXT));

	ZeroMemory(&dpnpi, sizeof (DPN_PLAYER_INFO));
	dpnpi.dwSize = sizeof (DPN_PLAYER_INFO);

	ZeroMemory(&dpngi, sizeof (DPN_GROUP_INFO));
	dpngi.dwSize = sizeof (DPN_GROUP_INFO);


	BEGIN_TESTCASE
	{
		// Remember whether this is a second in command case or not.
		if ((pTNecd->pExecutor->IsCase("2.3.1.3")) ||
			(pTNecd->pExecutor->IsCase("2.3.1.5")) ||
			(pTNecd->pExecutor->IsCase("2.3.2.3")) ||
			(pTNecd->pExecutor->IsCase("2.3.2.5")))
		{
			context.fSecondInCommandCase = TRUE;

#ifdef DEBUG
			// We need at least 4 machines to run this test.
			if (pTNecd->iNumMachines < 4)
			{
				DPL(0, "Can't run host and second-in-command migration test case with fewer than 4 machines (%i)!  DEBUGBREAK()-ing.",
					1, pTNecd->iNumMachines);
				DEBUGBREAK();
			} // end if (fewer than 3 machines)
#endif // DEBUG
		} // end if (second in command case)
		else
		{
			//context.fSecondInCommandCase = FALSE;

#ifdef DEBUG
			// We need at least 3 machines to run this test.
			if (pTNecd->iNumMachines < 3)
			{
				DPL(0, "Can't run host migration test case with fewer than 3 machines (%i)!  DEBUGBREAK()-ing.",
					1, pTNecd->iNumMachines);
				DEBUGBREAK();
			} // end if (fewer than 3 machines)
#endif // DEBUG
		} // end else (not a second in command case)

		if ((! pTNecd->pExecutor->IsCase("2.3.1.1")) &&
			(! pTNecd->pExecutor->IsCase("2.3.2.1")))
		{
			REQUIRE_INPUT_DATA(sizeof (TID_PHOSTMIGRATE_ROUNDROBIN));
		} // end if (round robin case)


		// Remember whether this is a group operations case or not.
		if ((pTNecd->pExecutor->IsCase("2.3.1.4")) ||
			(pTNecd->pExecutor->IsCase("2.3.1.5")) ||
			(pTNecd->pExecutor->IsCase("2.3.2.4")) ||
			(pTNecd->pExecutor->IsCase("2.3.2.5")))
		{
			context.iGroupOrDataOperation = GROUPORDATAOP_SETPEERINFO;
		} // end if (group/data operations case)
		else
		{
			context.iGroupOrDataOperation = GROUPORDATAOP_NONE;
		} // end else (not a group/data operations case)




		// Allocate an array for all testers' player IDs.  The position of a tester
		// is always the same, regardless of the iteration.
		// We allocate this array even though build session will give us one
		// because we need to modify it, and we can't modify someone else's buffer.

		LOCALALLOC_OR_THROW(DPNID*, context.padpnidTesters,
							(pTNecd->iNumMachines * sizeof (DPNID)));


		// Allocate an array to hold the number of reliable messages sent to each
		// tester.  The position of a tester is always the same, regardless of the
		// iteration.

		LOCALALLOC_OR_THROW(int*, paiReliableMsgsSentToTesters,
							(pTNecd->iNumMachines * sizeof (int)));

		// Also allocate an array to hold the number of reliable messages received
		// from each tester.  The position of a tester is always the same,
		// regardless of the iteration.

		LOCALALLOC_OR_THROW(int*, context.paiReliableMsgsReceivedFromTesters,
							(pTNecd->iNumMachines * sizeof (int)));


		// Allocate an array for all the other testers' numbers.  The position of
		// testers will vary depending on the iteration.

		LOCALALLOC_OR_THROW(int*, paiOtherTesters,
							((pTNecd->iNumMachines - 1) * sizeof (int)));


		// Allocate an array for add player/delete player indications.  The position
		// of a tester is always the same, regardless of the iteration.

		LOCALALLOC_OR_THROW(BOOL*, context.pafTesterIndicated,
							(pTNecd->iNumMachines * sizeof (BOOL)));




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating peer-to-peer session");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		context.pfnDPNMessageHandler = HostMigrateDPNMessageHandler;
		context.pTNecd = pTNecd;

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_MIGRATE_HOST | DPNSESSION_NODPNSVR;
		//dpnad.guidInstance = GUID_NULL;
		if (pTNecd->pExecutor->IsCase("2.3.1.1"))
			dpnad.guidApplication = GUID_HOSTMIGRATE_SIMPLE3PNORMAL;
		else if (pTNecd->pExecutor->IsCase("2.3.1.2"))
			dpnad.guidApplication = GUID_HOSTMIGRATE_ROUNDROBINNORMAL;
		else if (pTNecd->pExecutor->IsCase("2.3.2.1"))
			dpnad.guidApplication = GUID_HOSTMIGRATE_SIMPLE3PDROP;
		else
			dpnad.guidApplication = GUID_HOSTMIGRATE_ROUNDROBINDROP;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		dpnad.pwszSessionName = L"Session";
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.pvApplicationReservedData = 0;


		dwSubInputDataSize = sizeof (TID_BBLDDP8_ALL_CREATE)
							+ (pTNecd->iNumMachines * sizeof (int));

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, dwSubInputDataSize);
#pragma TODO(vanceo, "Allow SP to be selected")
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->guidSP = CLSID_DP8SP_TCPIP;
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->pHandlerContext = &context;
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->pdpnad = &dpnad;

		// Set the testers' player contexts to all be pointers to that tester's
		// entry in the number of reliable messages received array.
		for(i = 0; i < pTNecd->iNumMachines; i++)
		{
			*((PVOID*) (((PBYTE) pvSubInputData) + sizeof (TID_BBLDDP8_ALL_CREATE) + (i * sizeof (int*)))) = &(context.paiReliableMsgsReceivedFromTesters[i]);
		} // end for (each tester)


		sr = pTNecd->pExecutor->ExecSubTestCase("3.1.1.1", pvSubInputData,
												dwSubInputDataSize, 0);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPL(0, "Couldn't execute sub test case BldSsn:BldPeer:Create!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.1.1",
									"Creating peer-to-peer session failed!");


		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pBldDP8PeerCreateOutput,
									dwSubOutputDataSize,
									(sizeof (TOD_BBLDDP8_PEER_CREATE) + (pTNecd->iNumMachines * sizeof (DPNID))));

		pDP8Peer = pBldDP8PeerCreateOutput->pDP8Peer;
		pDP8Peer->m_dwRefCount++; // we're using it during this function

		CopyMemory(context.padpnidTesters,
					(pBldDP8PeerCreateOutput + 1),
					(pTNecd->iNumMachines * sizeof (DPNID)));




#pragma TODO(vanceo, "Free output vars if possible")



		// Start at the very beginning...
		iCurrentIteration = 0;
		TESTSECTION_DO
		{
			DPL(0, "Starting migration iteration #%i.", 1, iCurrentIteration);
			// Ignore error
			pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
									"Starting migration iteration #%i",
									1, iCurrentIteration);


			// Who's the original host of this iteration?  For second in command
			// cases, the host moves by twos (since host and next in line drop).
			if (context.fSecondInCommandCase)
				context.iCurrentHost = (iCurrentIteration * 2) % pTNecd->iNumMachines;
			else
				context.iCurrentHost = iCurrentIteration % pTNecd->iNumMachines;

			if ((context.iCurrentHost == pTNecd->iTesterNum) ||
				((context.fSecondInCommandCase) && (pTNecd->iTesterNum == ((context.iCurrentHost + 1) % pTNecd->iNumMachines))))
			{
				DPL(1, "We're going to drop this iteration (tester num = %i, host for iteration = %i).",
					2, pTNecd->iTesterNum, context.iCurrentHost);

				context.fDroppingPlayer = TRUE;
			} // end if (dropping player)
			else
			{
				DPL(1, "Host for this iteration is tester %i, we're not going to drop (we're tester %i).",
					2, context.iCurrentHost, pTNecd->iTesterNum);

				context.fDroppingPlayer = FALSE;
			} // end else (not dropping player)

			if (context.iGroupOrDataOperation != GROUPORDATAOP_NONE)
			{
				if (pTNecd->iTesterNum == ((context.iCurrentHost + pTNecd->iNumMachines - 1) % pTNecd->iNumMachines))
				{
					DPL(1, "We're going to perform group/data operation %i this iteration (tester num = %i).",
						2, context.iGroupOrDataOperation, pTNecd->iTesterNum);

					context.fGroupOrDataOperatingPlayer = TRUE;
				} // end if (group op tester)
				else
				{
					DPL(1, "We're expecting tester %i to perform group/data operation %i this iteration (we're tester num %i).",
						3, ((context.iCurrentHost + pTNecd->iNumMachines - 1) % pTNecd->iNumMachines),
						context.iGroupOrDataOperation, pTNecd->iTesterNum);

					context.fGroupOrDataOperatingPlayer = FALSE;
				} // end else (group op tester)
			} // end if (group/data operations case)


			
			// Fill the other tester number array for the current iteration.
			// The first (0th) entry in the array will always be the current host
			// for the session before this iteration's host migration, unless of
			// course the tester IS the current host (testers won't have themselves
			// in the array).
			// The second entry in the array will always be the second in command,
			// tester unless, the tester IS the second in command (for same reason
			// as above), or the current host (because of the effects of above).
			//
			// For a four person test in the first (0) non second-in-command
			// iteration:
			//   Tester 0's other testers array = {1, 2, 3}
			//   Tester 1's other testers array = {0, 2, 3}
			//   Tester 2's other testers array = {0, 1, 3}
			//   Tester 3's other testers array = {0, 1, 2}
			//
			// For a four person test in the second (1) non second-in-command:
			// iteration:
			//   Tester 0's other testers array = {1, 2, 3}
			//   Tester 1's other testers array = {2, 3, 0}
			//   Tester 2's other testers array = {1, 3, 0}
			//   Tester 3's other testers array = {1, 2, 0}
			//
			// For a four person test in the third (2) non second-in-command:
			// iteration:
			//   Tester 0's other testers array = {2, 3, 1}
			//   Tester 1's other testers array = {2, 3, 0}
			//   Tester 2's other testers array = {3, 0, 1}
			//   Tester 3's other testers array = {2, 0, 1}

			j = 0;
			for(i = 0; i < pTNecd->iNumMachines; i++)
			{
				if (((i + context.iCurrentHost) % pTNecd->iNumMachines) != pTNecd->iTesterNum)
					paiOtherTesters[j++] = (i + context.iCurrentHost) % pTNecd->iNumMachines;
			} // end for (each other tester)





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Starting sends to all players");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			ZeroMemory(&dpnbdUnreliable, sizeof (DPN_BUFFER_DESC));
			dpnbdUnreliable.dwBufferSize = strlen("Unreliable send string") + 1;
			dpnbdUnreliable.pBufferData = (PBYTE) "Unreliable send string";

			ZeroMemory(&dpnbdReliable, sizeof (DPN_BUFFER_DESC));
			dpnbdReliable.dwBufferSize = strlen("Reliable send string") + 1;
			dpnbdReliable.pBufferData = (PBYTE) "Reliable send string";

			i = 0;
			dwStartTime = timeGetTime();

			do
			{
				// Move to the next person to send to.
				i++;


				// Use the API directly so we don't have tons of our wrapper spew.
				// We'll send a reliable message every (NumMachines + 1)th send.
				// Since we're sending to (NumMachines) different targets, we'll end
				// up sending that reliable message to a different person every time
				// (wrapping back around of course).
				// By sending guaranteed messages synchronously, we don't have to
				// wait for completions, DPlay will do it for us.  We really only
				// need to wait for the last completion, because order is preserved
				// for reliable messages, but since we're sending for a set amount
				// of time, we don't know which send will be the last one.  So, in
				// keeping with my laziness, we'll send them all synchronously.
				// They're also "complete on process" so I don't have to implement
				// any timeouts waiting for acked reliable messages (messages that
				// completed on the sending side) but have yet to be received at the
				// app level.
				// Everyone is in the session, and we send to ourselves, so we don't
				// have to worry about skipping over some slot in the array.

				if ((i % (pTNecd->iNumMachines + 1)) == 0)
					paiReliableMsgsSentToTesters[i % pTNecd->iNumMachines]++;

				tr = pDP8Peer->m_pDP8Peer->SendTo(context.padpnidTesters[i % pTNecd->iNumMachines],
													(((i % (pTNecd->iNumMachines + 1)) == 0) ? &dpnbdReliable : &dpnbdUnreliable),
													1,
													0,
													NULL,
													(((i % (pTNecd->iNumMachines + 1)) == 0) ? NULL : &dpnhSend),
													(((i % (pTNecd->iNumMachines + 1)) == 0) ? (DPNSEND_SYNC | DPNSEND_NOCOPY | DPNSEND_COMPLETEONPROCESS | DPNSEND_GUARANTEED) : DPNSEND_NOCOPY));

				// Allow OK for SYNC and PENDING for async sends.
				if ((tr != DPN_OK) && (tr != (HRESULT) DPNSUCCESS_PENDING))
				{
					DPL(0, "SendTo didn't return OK or expected PENDING success code!", 0);
					THROW_TESTRESULT;
				} // end if (didn't get pending)



				// Sleep for 200 ms.  We don't use WaitForEventOrCancel because that
				// puts spew out and we sort of want to send as fast as possible.
				Sleep(200);
			} // end do (while haven't exceeded time)
			while ((timeGetTime() - dwStartTime) < 10000);


			DPL(1, "Sent %i total messages in approximately 10 seconds.", 1, i);




#pragma TODO(vanceo, "Improve this waiting mechanism here and below")

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting until send queues empty");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			for(i = 0; i < pTNecd->iNumMachines; i++)
			{
				// Skip our tester number.
				if (i == pTNecd->iTesterNum)
					continue;


				tr = pDP8Peer->DP8P_GetSendQueueInfo(context.padpnidTesters[i],
													&dwNumMsgs,
													NULL,
													0);
				if (tr != DPN_OK)
				{
					DPL(0, "Getting send queue info for tester %i (player ID %u/%x) failed!",
						3, i, context.padpnidTesters[i], context.padpnidTesters[i]);
					THROW_TESTRESULT;
				} // end if (get send queue failed)

				if (dwNumMsgs > 0)
				{
					dwStartTime = timeGetTime();

					do
					{
						DPL(7, "Tester %i (player ID %u/%x)'s send queue has %u messages still in it.",
							4, i, context.padpnidTesters[i], context.padpnidTesters[i],
							dwNumMsgs);


						// Sleep for 100 ms.  We don't use WaitForEventOrCancel because
						// that has overhead that we really don't need.
						Sleep(100);


						// Use the API directly so we don't have tons of our wrapper
						// spew.
						tr = pDP8Peer->m_pDP8Peer->GetSendQueueInfo(context.padpnidTesters[i],
																	&dwNumMsgs,
																	NULL,
																	0);
						if (tr != DPN_OK)
						{
							DPL(0, "Getting send queue info for tester %i (player ID %u/%x) failed!",
								3, i, context.padpnidTesters[i],
								context.padpnidTesters[i]);
							THROW_TESTRESULT;
						} // end if (get send queue failed)
					} // end do (while still messages in queue)
					while (dwNumMsgs > 0);

					DPL(0, "WARNING: Waited about %u ms for tester %i (player ID %u/%x)'s send queue to clear.",
						4, timeGetTime() - dwStartTime, i,
						context.padpnidTesters[i], context.padpnidTesters[i]);
				} // end if (there are messages in queue)
			} // end for (each tester)





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Exchanging reliable message counts (pre-migration)");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			// Note that we send the entire array, including the entry for sending
			// to ourselves.  It's just easier to send extra data than deal with the
			// hassle of creating an array just for this sync.
			sr = pTNecd->pExecutor->SyncWithTesters("Exchange message counts pre",
													paiReliableMsgsSentToTesters,
													(pTNecd->iNumMachines * sizeof (int)),
													&syncdata,
													0);
			HANDLE_SYNC_RESULT;


			for(i = 0; i < pTNecd->iNumMachines; i++)
			{
#pragma TODO(vanceo, "Check the number of messages we get from ourselves, too")
				// Skip our tester number
				if (i == pTNecd->iTesterNum)
					continue;

				CHECKANDGET_SYNCDATA(syncdata, i, paiReliableMsgsSentByTester,
									dwSyncDataSize, (pTNecd->iNumMachines * sizeof (int)));


				if (context.paiReliableMsgsReceivedFromTesters[i] != paiReliableMsgsSentByTester[pTNecd->iTesterNum])
				{
					DPL(0, "We received %i reliable messages from tester %i (player %u/%x) but he says he sent %i to us.",
						5, context.paiReliableMsgsReceivedFromTesters[i],
						i,
						context.padpnidTesters[i],
						context.padpnidTesters[i],
						paiReliableMsgsSentByTester[pTNecd->iTesterNum]);

					pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
										"We received %i reliable messages from tester %i (player %u/%x) but he says he sent %i to us.",
										5, context.paiReliableMsgsReceivedFromTesters[i],
										i,
										context.padpnidTesters[i],
										context.padpnidTesters[i],
										paiReliableMsgsSentByTester[pTNecd->iTesterNum]);

					SETTHROW_TESTRESULT(ERROR_NO_MATCH);
				} // end if (didn't get expected amount)

				DPL(1, "Tester %i sent %i reliable messages to us.",
					2, i, context.paiReliableMsgsReceivedFromTesters[i]);
			} // end for (each tester)

			
			// Reset the reliable message counts for the post-migration sends.
			ZeroMemory(paiReliableMsgsSentToTesters,
						(pTNecd->iNumMachines * sizeof (int)));

			ZeroMemory(context.paiReliableMsgsReceivedFromTesters,
						(pTNecd->iNumMachines * sizeof (int)));




			TESTSECTION_IF(context.fDroppingPlayer)
			{
				TESTSECTION_IF((pRoundRobinInput != NULL) && ((iCurrentIteration + 1) < pRoundRobinInput->iNumIterations))
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Getting soon-to-be-host's address for reconnection later");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


					if (context.fSecondInCommandCase)
						iNextHost = (context.iCurrentHost + 2) % pTNecd->iNumMachines;
					else
						iNextHost = (context.iCurrentHost + 1) % pTNecd->iNumMachines;

					tr = pDP8Peer->DP8P_GetPeerAddress(context.padpnidTesters[iNextHost],
													&pDP8AddressRemote,
													0);
					if (tr != DPN_OK)
					{
						DPL(0, "Couldn't get next host (%i)'s address!",
							1, iNextHost);
						THROW_TESTRESULT;
					} // end if (couldn't get peer address)


#ifdef DEBUG
					do
					{
						dwURLSize = dwURLBufferSize;

						tr = pDP8AddressRemote->GetURLA(pszURL, &dwURLSize);
						if (tr == DPNERR_BUFFERTOOSMALL)
						{
							SAFE_LOCALFREE(pszURL);
							LOCALALLOC_OR_THROW(char*, pszURL, dwURLSize);
							dwURLBufferSize = dwURLSize;
						} // end if (buffer too small)
						else if (tr != DPN_OK)
						{
							DPL(0, "Couldn't get next host address's URL!", 0);
							THROW_TESTRESULT;
						} // end if (didn't get URL)
					} // end do (while buffer too small)
					while (tr == DPNERR_BUFFERTOOSMALL);

					DPL(1, "Will use address (\"%s\") to reconnect (to tester %i, ID %u/%x).",
						4, pszURL, iNextHost, context.padpnidTesters[iNextHost],
						context.padpnidTesters[iNextHost]);
#endif // DEBUG
				} // end if (more iterations)
				TESTSECTION_ENDIF
			} // end if (dropping player)
			TESTSECTION_ENDIF



			// Create an event to set when the appropriate DESTROY_PLAYERs are
			// received.
			CREATEEVENT_OR_THROW(context.hPlayersGoneEvent,
								NULL, FALSE, FALSE, NULL);

			// Testers who aren't the current host need to get ready for the
			// host to migrate when he leaves.
			if (! context.fDroppingPlayer)
			{
				CREATEEVENT_OR_THROW(context.hHostMigratedEvent,
									NULL, FALSE, FALSE, NULL);

				if (context.iGroupOrDataOperation != GROUPORDATAOP_NONE)
				{
					CREATEEVENT_OR_THROW(context.hGroupOrDataOperationEvent,
										NULL, FALSE, FALSE, NULL);
				} // end if (group/data operations case)
			} // end if (not current original host tester)

			ZeroMemory(context.pafTesterIndicated,
						(pTNecd->iNumMachines * sizeof (BOOL)));



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for everyone to be ready to migrate host");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->SyncWithTesters("Ready to migrate host", NULL, 0, NULL, 0);
			HANDLE_SYNC_RESULT;



			// The current host should close his session, and everyone remaining in
			// the session should see him leave, plus agree upon who became the new
			// host.
			TESTSECTION_IF(context.fDroppingPlayer)
			{
				TESTSECTION_IF((pTNecd->pExecutor->IsCase("2.3.2.1")) || (pTNecd->pExecutor->IsCase("2.3.2.2")))
				{
					// We do NOT expect our local player to be removed.
					context.fSelfIndicate = FALSE;



					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Creating IMTest fault simulator");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->CreateNewFaultSim(&pFaultSim,
															TN_FAULTSIM_IMTEST,
															NULL,
															0);
					if (sr != S_OK)
					{
						DPL(0, "Couldn't create IMTest fault simulator!", 0);
						THROW_SYSTEMRESULT;
					} // end if (couldn't create fault simulator)




					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Disconnecting sends and receives, will drop from session");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->FaultSimDisconnect(pFaultSim, TRUE);
					if (sr != S_OK)
					{
						DPL(0, "Couldn't disconnect sends!", 0);
						THROW_SYSTEMRESULT;
					} // end if (couldn't disconnect)

					sr = pTNecd->pExecutor->FaultSimDisconnect(pFaultSim, FALSE);
					if (sr != S_OK)
					{
						DPL(0, "Couldn't disconnect receives!", 0);
						THROW_SYSTEMRESULT;
					} // end if (couldn't disconnect)



#pragma TODO(vanceo, "Timeout (be able to set protocol caps: keepalives?)")
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Waiting for connections to all other players to drop");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hPlayersGoneEvent,
																1,
																NULL,
																-1,
																INFINITE,
																NULL);
					HANDLE_WAIT_RESULT;





					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Restoring send and receive traffic");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->FaultSimReconnect(pFaultSim, TRUE);
					if (sr != S_OK)
					{
						DPL(0, "Couldn't reconnect sends!", 0);
						THROW_SYSTEMRESULT;
					} // end if (couldn't reconnect)

					sr = pTNecd->pExecutor->FaultSimReconnect(pFaultSim, FALSE);
					if (sr != S_OK)
					{
						DPL(0, "Couldn't reconnect receives!", 0);
						THROW_SYSTEMRESULT;
					} // end if (couldn't reconnect)



					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Releasing IMTest fault simulator");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->ReleaseFaultSim(&pFaultSim);
					if (sr != S_OK)
					{
						DPL(0, "Couldn't release fault simulator %x!", 1, pFaultSim);
						THROW_SYSTEMRESULT;
					} // end if (couldn't release fault simulator)
				} // end if (drop host migration case)
				TESTSECTION_ENDIF



				// We expect our own player to be removed.
				context.fSelfIndicate = TRUE;


				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Disconnecting from session");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Peer->DP8P_Close(0);
				if (tr != DPN_OK)
				{
					DPL(0, "Closing failed!", 0);
					THROW_TESTRESULT;
				} // end if (close failed)


				// Make sure the group operations were unwound, if necessary.
				if (context.dpnidPlayerAddedToGroup != 0)
				{
					DPL(0, "Player add to group ID wasn't cleared!", 0);
					SETTHROW_TESTRESULT(ERROR_NO_DATA);
				} // end if (player ID wasn't cleared)

				if (context.dpnidGroup != 0)
				{
					DPL(0, "Group ID wasn't cleared!", 0);
					SETTHROW_TESTRESULT(ERROR_NO_DATA);
				} // end if (group ID wasn't cleared)

				// Make sure all the players are gone.
				for(i = 0; i < pTNecd->iNumMachines; i++)
				{
					if (! context.pafTesterIndicated[i])
					{
						DPL(0, "Tester %i was not removed from nametable!",
							1, i);
						SETTHROW_TESTRESULT(ERROR_NO_DATA);
					} // end if (tester not indicated)
				} // end for (each tester)

				if (context.iGroupOrDataOperation == GROUPORDATAOP_SETPEERINFO)
				{
					if (context.fPeerInfoSet)
					{
						DPL(0, "Noting (again) that the peer info update notification sneaked in sometime during shutdown.", 0);
						context.fPeerInfoSet = FALSE;
					} // end if (peer info indication arrived)
				} // end if (set peer info group/data operation)
				else if (context.iGroupOrDataOperation == GROUPORDATAOP_SETGROUPINFO)
				{
					if (context.fGroupInfoSet)
					{
						DPL(0, "Noting (again) that the group info update notification sneaked in sometime during shutdown.", 0);
						context.fGroupInfoSet = FALSE;
					} // end if (group info indication arrived)
				} // end if (set group info group/data operation)



				// Technically, we didn't even need this event for the normal leave
				// case.
				CloseHandle(context.hPlayersGoneEvent);
				context.hPlayersGoneEvent = NULL;



				// Clear this, to be paranoid.
				context.pDP8Peer = NULL;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Releasing DirectPlay8Peer object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Peer->Release();
				if (tr != S_OK)
				{
					DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't release object)
			} // end if (dropping tester)
			TESTSECTION_ELSE
			{
				// If this is a group operation case and we're supposed to perform
				// the group operation, do so.
				TESTSECTION_IF(context.fGroupOrDataOperatingPlayer)
				{
					TESTSECTION_SWITCH(context.iGroupOrDataOperation)
					{
						TESTSECTION_CASE(GROUPORDATAOP_SETPEERINFO)
						{
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -
							TEST_SECTION("Starting to set peer info");
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -

							tr = pDP8Peer->DP8P_SetPeerInfo(&dpnpi,
															NULL,
															&dpnhGroupOrDataOp,
															0);
							if (tr != (HRESULT) DPNSUCCESS_PENDING)
							{
								DPL(0, "Setting peer info didn't return expected PENDING success code!", 0);
								THROW_TESTRESULT;
							} // end if (couldn't set peer info)
						}
						TESTSECTION_ENDCASE

						TESTSECTION_CASE(GROUPORDATAOP_CREATEGROUP)
						{
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -
							TEST_SECTION("Starting to create a group");
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -

							tr = pDP8Peer->DP8P_CreateGroup(&dpngi,
															NULL,
															NULL,
															&dpnhGroupOrDataOp,
															0);
							if (tr != (HRESULT) DPNSUCCESS_PENDING)
							{
								DPL(0, "Creating group didn't return expected PENDING success code!", 0);
								THROW_TESTRESULT;
							} // end if (couldn't create group)
						}
						TESTSECTION_ENDCASE

						TESTSECTION_CASE(GROUPORDATAOP_SETGROUPINFO)
						{
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -
							TEST_SECTION("Starting to set group info");
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -

							tr = pDP8Peer->DP8P_SetGroupInfo(context.dpnidGroup,
															&dpngi,
															NULL,
															&dpnhGroupOrDataOp,
															0);
							if (tr != (HRESULT) DPNSUCCESS_PENDING)
							{
								DPL(0, "Setting group info didn't return expected PENDING success code!", 0);
								THROW_TESTRESULT;
							} // end if (couldn't set group info)
						}
						TESTSECTION_ENDCASE

						TESTSECTION_CASE(GROUPORDATAOP_ADDPLAYERTOGROUP)
						{
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -
							TEST_SECTION("Starting to add self to the group");
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -

							tr = pDP8Peer->DP8P_AddPlayerToGroup(context.dpnidGroup,
																context.padpnidTesters[pTNecd->iTesterNum],
																NULL,
																&dpnhGroupOrDataOp,
																0);
							if (tr != (HRESULT) DPNSUCCESS_PENDING)
							{
								DPL(0, "Adding player to group didn't return expected PENDING success code!", 0);
								THROW_TESTRESULT;
							} // end if (couldn't add player to group)
						}
						TESTSECTION_ENDCASE

						TESTSECTION_CASE(GROUPORDATAOP_REMOVEPLAYERFROMGROUP)
						{
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -
							TEST_SECTION("Starting to remove the player from the group");
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -

							// Since we're removing a different player that might be
							// racing to drop from the session, we need to handle
							// the INVALIDPLAYER and PLAYERNOTINGROUP error codes as
							// well.
							tr = pDP8Peer->DP8P_RemovePlayerFromGroup(context.dpnidGroup,
																	context.dpnidPlayerAddedToGroup,
																	NULL,
																	&dpnhGroupOrDataOp,
																	0);
							if (tr != (HRESULT) DPNSUCCESS_PENDING)
							{
								if ((tr != (HRESULT) DPNSUCCESS_PENDING) &&
									(tr != (HRESULT) DPNERR_INVALIDPLAYER) &&
									(tr != (HRESULT) DPNERR_PLAYERNOTINGROUP))
								{
									DPL(0, "Removing player from group didn't return expected PENDING, INVALIDPLAYER, or PLAYERNOTINGROUP!", 0);
									THROW_TESTRESULT;
								} // end if (couldn't remove player from group)

								// It failed to even startup up, so we won't get an
								// async op completion.  However, we still expect
								// the REMOVE_PLAYER_FROM_GROUP to be indicated due
								// to the nametable unwind.  It may have already
								// arrived by the time we notify the callback, so
								// check right after we set the alerting boolean.
								context.fNoAsyncOpCompletion = TRUE;
								if (context.dpnidPlayerAddedToGroup == 0)
								{
									DPL(0, "REMOVE_PLAYER_FROM_GROUP message appears to already have arrived, setting event.", 0);
									
									if (! SetEvent(context.hGroupOrDataOperationEvent))
									{
										sr = GetLastError();

										DPL(0, "Couldn't set group or data operation event (%x)!",
											1, context.hGroupOrDataOperationEvent);
										THROW_SYSTEMRESULT;
									} // end if (couldn't set event)
								} // end if (message already arrived)
							} // end if (couldn't remove player from group)
						}
						TESTSECTION_ENDCASE

						TESTSECTION_CASE(GROUPORDATAOP_DESTROYGROUP)
						{
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -
							TEST_SECTION("Starting to destroy the group");
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -

							tr = pDP8Peer->DP8P_DestroyGroup(context.dpnidGroup,
															NULL,
															&dpnhGroupOrDataOp,
															0);
							if (tr != (HRESULT) DPNSUCCESS_PENDING)
							{
								DPL(0, "Destroying group didn't return expected PENDING success code!", 0);
								THROW_TESTRESULT;
							} // end if (couldn't destroy group)
						}
						TESTSECTION_ENDCASE

#ifdef DEBUG
						TESTSECTION_DEFAULT
						{
							DPL(0, "Unexpected group/data operation %i!  DEBUGBREAK()-ing.",
								1, context.iGroupOrDataOperation);
							DEBUGBREAK();
						}
						TESTSECTION_ENDDEFAULT
#endif // DEBUG
					} // end if (creating a group)
					TESTSECTION_ENDSWITCH
				} // end if (group operations case)
				TESTSECTION_ENDIF


				TESTSECTION_IF(context.fSecondInCommandCase)
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Waiting for old host and second in command delete player notifications");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				} // end if (second in command case)
				TESTSECTION_ELSE
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Waiting for old host's delete player notification");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				} // end else (not second in command case)
				TESTSECTION_ENDIF

				sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hPlayersGoneEvent),
															1,
															NULL,
															-1,
															INFINITE,
															NULL);
				HANDLE_WAIT_RESULT;


				CloseHandle(context.hPlayersGoneEvent);
				context.hPlayersGoneEvent = NULL;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for host migration notification");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hHostMigratedEvent),
															1,
															NULL,
															-1,
															INFINITE,
															NULL);
				HANDLE_WAIT_RESULT;

				CloseHandle(context.hHostMigratedEvent);
				context.hHostMigratedEvent = NULL;



				TESTSECTION_IF(context.iGroupOrDataOperation != GROUPORDATAOP_NONE)
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Waiting for group or data operation notification");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hGroupOrDataOperationEvent),
																1,
																NULL,
																-1,
																INFINITE,
																NULL);
					HANDLE_WAIT_RESULT;


					// Make sure we got the indication correctly.  We really
					// shouldn't have our event set if we didn't, but we'll double
					// check to be sure.
					switch (context.iGroupOrDataOperation)
					{
						case GROUPORDATAOP_SETPEERINFO:
							if (! context.fPeerInfoSet)
							{
								DPL(0, "Peer info wasn't set!", 0);
								SETTHROW_TESTRESULT(ERROR_NO_DATA);
							} // end if (peer info wasn't set)

							// Reset the value.
							context.fPeerInfoSet = FALSE;
						  break;

						case GROUPORDATAOP_CREATEGROUP:
							if (context.dpnidGroup == 0)
							{
								DPL(0, "Group ID wasn't set!", 0);
								SETTHROW_TESTRESULT(ERROR_NO_DATA);
							} // end if (group ID wasn't set)
						  break;

						case GROUPORDATAOP_SETGROUPINFO:
							if (! context.fGroupInfoSet)
							{
								DPL(0, "Group info wasn't set!", 0);
								SETTHROW_TESTRESULT(ERROR_NO_DATA);
							} // end if (group info wasn't set)

							// Reset the value.
							context.fGroupInfoSet = FALSE;
						  break;

						case GROUPORDATAOP_ADDPLAYERTOGROUP:
							if (context.dpnidPlayerAddedToGroup == 0)
							{
								DPL(0, "Player add to group ID wasn't set!", 0);
								SETTHROW_TESTRESULT(ERROR_NO_DATA);
							} // end if (player ID wasn't set)
						  break;

						case GROUPORDATAOP_REMOVEPLAYERFROMGROUP:
							if (context.dpnidPlayerAddedToGroup != 0)
							{
								DPL(0, "Player add to group ID wasn't cleared!", 0);
								SETTHROW_TESTRESULT(ERROR_NO_DATA);
							} // end if (player ID wasn't cleared)
						  break;

						case GROUPORDATAOP_DESTROYGROUP:
							if (context.dpnidGroup != 0)
							{
								DPL(0, "Group ID wasn't cleared!", 0);
								SETTHROW_TESTRESULT(ERROR_NO_DATA);
							} // end if (group ID wasn't cleared)
						  break;
					} // end switch (on group operation)

					CloseHandle(context.hGroupOrDataOperationEvent);
					context.hGroupOrDataOperationEvent = NULL;


					// Reset this boolean in case it was set.
					context.fNoAsyncOpCompletion = FALSE;
				} // end if (group operations case)
				TESTSECTION_ENDIF



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Starting sends after host migration");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				i = 0;
				dwStartTime = timeGetTime();

				do
				{
					// Move to the next person to send to.
					i++;


					// Use the API directly so we don't have tons of our wrapper
					// spew.
					// We'll send a reliable message every (NumMachines)th send.
					// Since we're sending to (NumMachines - 1) different targets
					// now, we'll end up sending that reliable message to a
					// different person every time (wrapping back around of course).
					// By sending guaranteed messages synchronously, we don't have
					// to wait for completions, DPlay will do it for us.  We really
					// only need to wait for the last completion, because order is
					// preserved for reliable messages, but since we're sending for
					// a set amount of time, we don't know which send will be the
					// last one.  So, in keeping with my laziness, we'll send them
					// all synchronously.
					// They're also "complete on process" so I don't have to
					// implement any timeouts waiting for acked reliable messages
					// (messages that completed on the sending side) but have yet
					// to be received at the app level.
					// Not everyone is in the session, so we have to skip the
					// current original host's (and second-in-command's, if that type
					// of test) index in the array.

					if (context.fSecondInCommandCase)
					{
						if ((i % (pTNecd->iNumMachines - 1)) == 0)
							paiReliableMsgsSentToTesters[((i % (pTNecd->iNumMachines - 2)) + context.iCurrentHost + 2) % pTNecd->iNumMachines]++;

						tr = pDP8Peer->m_pDP8Peer->SendTo(context.padpnidTesters[((i % (pTNecd->iNumMachines - 2)) + context.iCurrentHost + 2) % pTNecd->iNumMachines],
															(((i % (pTNecd->iNumMachines - 1)) == 0) ? &dpnbdReliable : &dpnbdUnreliable),
															1,
															0,
															NULL,
															(((i % (pTNecd->iNumMachines - 1)) == 0) ? NULL : &dpnhSend),
															(((i % (pTNecd->iNumMachines - 1)) == 0) ? (DPNSEND_SYNC | DPNSEND_NOCOPY | DPNSEND_COMPLETEONPROCESS | DPNSEND_GUARANTEED) : DPNSEND_NOCOPY));
					} // end if (second in command case)
					else
					{
						if ((i % pTNecd->iNumMachines) == 0)
							paiReliableMsgsSentToTesters[((i % (pTNecd->iNumMachines - 1)) + context.iCurrentHost + 1) % pTNecd->iNumMachines]++;

						tr = pDP8Peer->m_pDP8Peer->SendTo(context.padpnidTesters[((i % (pTNecd->iNumMachines - 1)) + context.iCurrentHost + 1) % pTNecd->iNumMachines],
															(((i % pTNecd->iNumMachines) == 0) ? &dpnbdReliable : &dpnbdUnreliable),
															1,
															0,
															NULL,
															(((i % pTNecd->iNumMachines) == 0) ? NULL : &dpnhSend),
															(((i % pTNecd->iNumMachines) == 0) ? (DPNSEND_SYNC | DPNSEND_NOCOPY | DPNSEND_COMPLETEONPROCESS | DPNSEND_GUARANTEED) : DPNSEND_NOCOPY));
					} // end else (not second in command case)

					// Allow OK for SYNC and PENDING for async sends.
					if ((tr != DPN_OK) && (tr != (HRESULT) DPNSUCCESS_PENDING))
					{
						DPL(0, "SendTo didn't return OK or expected PENDING success code!", 0);
						THROW_TESTRESULT;
					} // end if (didn't get pending)



					// Sleep for 200 ms.  We don't use WaitForEventOrCancel because
					// that puts spew out and we sort of want to send as fast as
					// possible.
					Sleep(200);
				} // end do (while haven't exceeded time)
				while ((timeGetTime() - dwStartTime) < 10000);


				DPL(1, "Sent %i total messages in approximately 10 seconds.", 1, i);




				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting until send queues empty");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				for(i = 0; i < pTNecd->iNumMachines; i++)
				{
					// Skip our tester number and the people who left the session.
					if ((i == pTNecd->iTesterNum) || (i == context.iCurrentHost) ||
						((context.fSecondInCommandCase) && (i == ((context.iCurrentHost + 1) % pTNecd->iNumMachines))))
					{
						continue;
					} // end if (self or missing player)


					tr = pDP8Peer->DP8P_GetSendQueueInfo(context.padpnidTesters[i],
														&dwNumMsgs,
														NULL,
														0);
					if (tr != DPN_OK)
					{
						DPL(0, "Getting send queue info for tester %i (player ID %u/%x) failed!",
							3, i, context.padpnidTesters[i], context.padpnidTesters[i]);
						THROW_TESTRESULT;
					} // end if (get send queue failed)

					if (dwNumMsgs > 0)
					{
						dwStartTime = timeGetTime();

						do
						{
							DPL(7, "Tester %i (player ID %u/%x)'s send queue has %u messages still in it.",
								4, i, context.padpnidTesters[i], context.padpnidTesters[i],
								dwNumMsgs);


							// Sleep for 100 ms.  We don't use WaitForEventOrCancel because
							// that has overhead that we really don't need.
							Sleep(100);


							// Use the API directly so we don't have tons of our wrapper
							// spew.
							tr = pDP8Peer->m_pDP8Peer->GetSendQueueInfo(context.padpnidTesters[i],
																		&dwNumMsgs,
																		NULL,
																		0);
							if (tr != DPN_OK)
							{
								DPL(0, "Getting send queue info for tester %i (player ID %u/%x) failed!",
									3, i, context.padpnidTesters[i],
									context.padpnidTesters[i]);
								THROW_TESTRESULT;
							} // end if (get send queue failed)
						} // end do (while still messages in queue)
						while (dwNumMsgs > 0);

						DPL(0, "WARNING: Waited about %u ms for tester %i (player ID %u/%x)'s send queue to clear.",
							4, timeGetTime() - dwStartTime, i,
							context.padpnidTesters[i], context.padpnidTesters[i]);
					} // end if (there are messages in queue)
				} // end for (each tester)





				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Exchanging reliable message counts (post-migration)");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				// Note we actually send the count of messages we sent to the
				// missing player(s) (which should always be zero) AND the number
				// we sent to ourselves.  It's easier to just send 2-3 extra integers
				// than copy to another array just for this sync.
				// We synchronize only with the players that didn't drop out of the
				// session, though.
				if (context.fSecondInCommandCase)
				{
					sr = pTNecd->pExecutor->SyncWithTestersArray("Exchange message counts post",
																paiReliableMsgsSentToTesters,
																(pTNecd->iNumMachines * sizeof (int)),
																&syncdata,
																(pTNecd->iNumMachines - 3),
																(paiOtherTesters + 2));
				} // end if (second in command case)
				else
				{
					sr = pTNecd->pExecutor->SyncWithTestersArray("Exchange message counts post",
																paiReliableMsgsSentToTesters,
																(pTNecd->iNumMachines * sizeof (int)),
																&syncdata,
																(pTNecd->iNumMachines - 2),
																(paiOtherTesters + 1));
				} // end else (not second in command case)
				HANDLE_SYNC_RESULT;


				for(i = 0; i < pTNecd->iNumMachines; i++)
				{
#pragma TODO(vanceo, "Check the number of messages we get from ourselves, too")
					// Skip our tester number and the people who left the session.
					if ((i == pTNecd->iTesterNum) || (i == context.iCurrentHost) ||
						((context.fSecondInCommandCase) && (i == ((context.iCurrentHost + 1) % pTNecd->iNumMachines))))
					{
						continue;
					} // end if (self or missing player)


					CHECKANDGET_SYNCDATA(syncdata, i, paiReliableMsgsSentByTester,
										dwSyncDataSize, (pTNecd->iNumMachines * sizeof (int)));


					if (context.paiReliableMsgsReceivedFromTesters[i] != paiReliableMsgsSentByTester[pTNecd->iTesterNum])
					{
						DPL(0, "We received %i reliable messages from tester %i (player %u/%x) but he says he sent %i to us.",
							5, context.paiReliableMsgsReceivedFromTesters[i],
							i,
							context.padpnidTesters[i],
							context.padpnidTesters[i],
							paiReliableMsgsSentByTester[pTNecd->iTesterNum]);

						pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
											"We received %i reliable messages from tester %i (player %u/%x) but he says he sent %i to us.",
											5, context.paiReliableMsgsReceivedFromTesters[i],
											i,
											context.padpnidTesters[i],
											context.padpnidTesters[i],
											paiReliableMsgsSentByTester[pTNecd->iTesterNum]);

						SETTHROW_TESTRESULT(ERROR_NO_MATCH);
					} // end if (didn't get expected amount)

					DPL(1, "Tester %i sent %i reliable messages to us.",
						2, i, context.paiReliableMsgsReceivedFromTesters[i]);
				} // end for (each tester)
			} // end else (not dropping tester)
			TESTSECTION_ENDIF



			TESTSECTION_IF(pRoundRobinInput == NULL)
			{
				DPL(1, "Simple case, only one iteration required.", 0);
				TESTSECTION_BREAK;
			} // end if (not round robin case)
			TESTSECTION_ENDIF



			iCurrentIteration++;

			// If there aren't any more iterations, we can skip the rejoin phase.
			TESTSECTION_IF(iCurrentIteration >= pRoundRobinInput->iNumIterations)
			{
				DPL(1, "Final iteration completed.", 0);
				TESTSECTION_BREAK;
			} // end if (last iteration)
			TESTSECTION_ENDIF



			DPL(1, "Preparing for iteration %i.", 1, iCurrentIteration);
			pTNecd->pExecutor->Log(TNLF_PREFIX_TESTUNIQUEID,
									"Preparing for iteration %i.",
									1, iCurrentIteration);



			// Reset the player ID for the original host since he's no longer in
			// the session.  Same goes for second in command, if he's gone.
			context.padpnidTesters[context.iCurrentHost] = 0;
			if (context.fSecondInCommandCase)
				context.padpnidTesters[(context.iCurrentHost + 1) % pTNecd->iNumMachines] = 0;


			context.pDP8Peer = pDP8Peer; // this will be NULL for original host

			context.hRejoinedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (context.hRejoinedEvent == NULL)
			{
				sr = GetLastError();
				DPL(0, "Couldn't create rejoin event!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't create event)


			// Reset the reliable message counts for the next round of sends.
			ZeroMemory(paiReliableMsgsSentToTesters,
						(pTNecd->iNumMachines * sizeof (int)));

			ZeroMemory(context.paiReliableMsgsReceivedFromTesters,
						(pTNecd->iNumMachines * sizeof (int)));

			ZeroMemory(context.pafTesterIndicated,
						(pTNecd->iNumMachines * sizeof (BOOL)));



			// Now the players who left should rejoin the session.
			TESTSECTION_IF(context.fDroppingPlayer)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Recreating DirectPlay8Peer object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Peer->CoCreate();
				if (tr != S_OK)
				{
					DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't CoCreate)

				context.pDP8Peer = pDP8Peer; // update context pointer for this, too




				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Initializing object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				// We have to use the bldssn message handler because we use the
				// destroy session helper later, and that requires the appropriate
				// message handler.
				tr = pDP8Peer->DP8P_Initialize(&context,
												BldDP8PeerDPNMessageHandler,
												0);
				if (tr != S_OK)
				{
					DPL(0, "Couldn't initialize peer object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't initialize)



				hConnectCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				if (hConnectCompleteEvent == NULL)
				{
					sr = GetLastError();
					DPL(0, "Couldn't create connect completion event!", 0);
					THROW_SYSTEMRESULT;
				} // end if (couldn't create event)




				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("CoCreating local address object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = CoCreateInstance(CLSID_DirectPlay8Address, NULL,
									CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,
									(LPVOID*) &pDP8AddressLocal);
				if (sr != S_OK)
				{
					DPL(0, "Couldn't CoCreate address object!", 0);
					THROW_SYSTEMRESULT;
				} // end if (couldn't CoCreate object)



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Setting local address' SP");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8AddressLocal->SetSP(&CLSID_DP8SP_TCPIP);
				if (tr != DPN_OK)
				{
					DPL(0, "Couldn't set local address' SP!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't set SP)


				context.fSelfIndicate = TRUE;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Synchronizing with players still in session");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Ready for old host rejoin",
														NULL, 0,
														NULL,
														0);
				HANDLE_SYNC_RESULT;



				TESTSECTION_IF(context.iCurrentHost != pTNecd->iTesterNum)
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Waiting for old host to rejoin before us");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->SyncWithTesters("Ensure correct old host player ID",
															NULL, 0,
															&syncdata,
															0);
					HANDLE_SYNC_RESULT;

					CHECKANDGET_SYNCDATA(syncdata, context.iCurrentHost,
										pvSyncData, dwSubOutputDataSize, sizeof (DPNID));


					// Store the original host's new player ID.
					context.padpnidTesters[context.iCurrentHost] = *((DPNID*) pvSyncData);
				} // end if (not original host)
				TESTSECTION_ENDIF



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Reconnecting to session");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Peer->DP8P_Connect(&dpnad,
											pDP8AddressRemote,
											pDP8AddressLocal,
											NULL,
											NULL,
											NULL,
											0,
											NULL,
											hConnectCompleteEvent,
											&dpnhConnect,
											0);
				if (tr != (HRESULT) DPNSUCCESS_PENDING)
				{
					DPL(0, "Connect didn't return expected PENDING success code!", 0);
					THROW_TESTRESULT;
				} // end if (connect not pending)


				// We don't need the remote address anymore.
				pDP8AddressRemote->Release();
				pDP8AddressRemote = NULL;

				// We don't need the local address anymore.
				pDP8AddressLocal->Release();
				pDP8AddressLocal = NULL;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for all players in session to connect");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hRejoinedEvent),
															1,
															NULL,
															-1,
															INFINITE,
															NULL);
				HANDLE_WAIT_RESULT;



				context.fSelfIndicate = FALSE;




				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for connect completion notification");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->WaitForEventOrCancel(&hConnectCompleteEvent,
															1,
															NULL,
															-1,
															INFINITE,
															NULL);
				HANDLE_WAIT_RESULT;


				CloseHandle(hConnectCompleteEvent);
				hConnectCompleteEvent = NULL;

				// Make sure the current group status got rebuilt correctly.
				if (context.iGroupOrDataOperation != GROUPORDATAOP_NONE)
				{
					if ((context.iGroupOrDataOperation == GROUPORDATAOP_SETPEERINFO) ||
						(context.iGroupOrDataOperation == GROUPORDATAOP_DESTROYGROUP))
					{
						if (context.dpnidGroup != 0)
						{
							DPL(0, "Got CREATE_GROUP indication (ID = %u/%x)!",
								2, context.dpnidGroup, context.dpnidGroup);
							SETTHROW_TESTRESULT(E_FAIL);
						} // end if (group ID wasn't set)
					} // end if (destroy group operation)
					else
					{
						if (context.dpnidGroup == 0)
						{
							DPL(0, "Didn't get expected CREATE_GROUP indication!", 0);
							SETTHROW_TESTRESULT(ERROR_NO_DATA);
						} // end if (group ID wasn't set)
					} // end else (not destroy group operation)


					if (context.iGroupOrDataOperation == GROUPORDATAOP_ADDPLAYERTOGROUP)
					{
						if (context.dpnidPlayerAddedToGroup == 0)
						{
							DPL(0, "Didn't get expected ADD_PLAYER_TO_GROUP indication!", 0);
							SETTHROW_TESTRESULT(ERROR_NO_DATA);
						} // end if (player ID wasn't set)
					} // end if (add player to group operation)
					else
					{
						if (context.dpnidPlayerAddedToGroup != 0)
						{
							DPL(0, "Got ADD_PLAYER_TO_GROUP indication (player ID = %u/%x)!",
								2, context.dpnidPlayerAddedToGroup,
								context.dpnidPlayerAddedToGroup);
							SETTHROW_TESTRESULT(E_FAIL);
						} // end if (player ID wasn't set)
					} // end else (not add player to group operation)
				} // end if (group operations case)



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Ensure everyone has our player ID correct");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				if (context.iCurrentHost == pTNecd->iTesterNum)
				{
					sr = pTNecd->pExecutor->SyncWithTesters("Ensure correct old host player ID",
															&(context.padpnidTesters[pTNecd->iTesterNum]),
															sizeof (DPNID),
															NULL,
															0);
				} // end if (original host)
				else
				{
					sr = pTNecd->pExecutor->SyncWithTesters("Ensure correct old second-in-command player ID",
															&(context.padpnidTesters[pTNecd->iTesterNum]),
															sizeof (DPNID),
															NULL,
															0);
				} // end else (original second in command)
				HANDLE_SYNC_RESULT;



				TESTSECTION_IF((context.fSecondInCommandCase) && (context.iCurrentHost == pTNecd->iTesterNum))
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Waiting for old second-in-command to rejoin session");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hRejoinedEvent),
																1,
																NULL,
																-1,
																INFINITE,
																NULL);
					HANDLE_WAIT_RESULT;



					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Making sure new player ID for old second-in-command is correct");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->SyncWithTesters("Ensure correct old second-in-command player ID",
															NULL, 0,
															&syncdata,
															0);
					HANDLE_SYNC_RESULT;

					CHECKANDGET_SYNCDATA(syncdata, ((context.iCurrentHost + 1) % pTNecd->iNumMachines),
										pvSyncData, dwSubOutputDataSize, sizeof (DPNID));


					// Make sure he got the same ID we saw him get.
					if (context.padpnidTesters[(context.iCurrentHost + 1) % pTNecd->iNumMachines] != *((DPNID*) pvSyncData))
					{
						DPL(0, "The original second-in-command (tester %i) rejoining player ID %u/%x we received differs from the ID he claims he got (%u/%x)!",
							5, ((context.iCurrentHost + 1) % pTNecd->iNumMachines),
							context.padpnidTesters[(context.iCurrentHost + 1) % pTNecd->iNumMachines],
							context.padpnidTesters[(context.iCurrentHost + 1) % pTNecd->iNumMachines],
							*((DPNID*) pvSyncData),
							*((DPNID*) pvSyncData));
						SETTHROW_TESTRESULT(ERROR_NO_MATCH);
					} // end if (IDs don't match)
				} // end if (not original host)
				TESTSECTION_ENDIF
			} // end if (original host who left)
			TESTSECTION_ELSE
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Synchronizing with rejoining old host");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Ready for old host rejoin",
														NULL, 0,
														NULL,
														0);
				HANDLE_SYNC_RESULT;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for old host to rejoin session");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hRejoinedEvent),
															1,
															NULL,
															-1,
															INFINITE,
															NULL);
				HANDLE_WAIT_RESULT;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Making sure new player ID for old host is correct");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Ensure correct old host player ID",
														NULL, 0,
														&syncdata,
														0);
				HANDLE_SYNC_RESULT;

				CHECKANDGET_SYNCDATA(syncdata, context.iCurrentHost,
									pvSyncData, dwSubOutputDataSize, sizeof (DPNID));


				// Make sure he got the same ID we saw him get.
				if (context.padpnidTesters[context.iCurrentHost] != *((DPNID*) pvSyncData))
				{
					DPL(0, "The original host (tester %i) rejoining player ID %u/%x we received differs from the ID he claims he got (%u/%x)!",
						5, context.iCurrentHost,
						context.padpnidTesters[context.iCurrentHost],
						context.padpnidTesters[context.iCurrentHost],
						*((DPNID*) pvSyncData),
						*((DPNID*) pvSyncData));
					SETTHROW_TESTRESULT(ERROR_NO_MATCH);
				} // end if (IDs don't match)



				TESTSECTION_IF(context.fSecondInCommandCase)
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Waiting for old host to rejoin session");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hRejoinedEvent),
																1,
																NULL,
																-1,
																INFINITE,
																NULL);
					HANDLE_WAIT_RESULT;



					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Making sure new player ID for old second-in-command is correct");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->SyncWithTesters("Ensure correct old second-in-command player ID",
															NULL, 0,
															&syncdata,
															0);
					HANDLE_SYNC_RESULT;

					CHECKANDGET_SYNCDATA(syncdata, ((context.iCurrentHost + 1) % pTNecd->iNumMachines),
										pvSyncData, dwSubOutputDataSize, sizeof (DPNID));


					// Make sure he got the same ID we saw him get.
					if (context.padpnidTesters[(context.iCurrentHost + 1) % pTNecd->iNumMachines] != *((DPNID*) pvSyncData))
					{
						DPL(0, "The original second-in-command (tester %i) rejoining player ID %u/%x we received differs from the ID he claims he got (%u/%x)!",
							5, ((context.iCurrentHost + 1) % pTNecd->iNumMachines),
							context.padpnidTesters[(context.iCurrentHost + 1) % pTNecd->iNumMachines],
							context.padpnidTesters[(context.iCurrentHost + 1) % pTNecd->iNumMachines],
							*((DPNID*) pvSyncData),
							*((DPNID*) pvSyncData));
						SETTHROW_TESTRESULT(ERROR_NO_MATCH);
					} // end if (IDs don't match)
				} // end if (second in command case)
				TESTSECTION_ENDIF
			} // end else (not original host)
			TESTSECTION_ENDIF


			// Update to the group/data operation for the next iteration, if we're
			// doing that.
			if (context.iGroupOrDataOperation != GROUPORDATAOP_NONE)
			{
				context.iGroupOrDataOperation++;
				if (context.iGroupOrDataOperation > GROUPORDATAOP_DESTROYGROUP)
					context.iGroupOrDataOperation = GROUPORDATAOP_SETPEERINFO;
			} // end if (group /dataoperations)


			CloseHandle(context.hRejoinedEvent);
			context.hRejoinedEvent = NULL;
		} // end do (while still more iterations)
		TESTSECTION_DOWHILE(TRUE);




		TESTSECTION_IF(context.fDroppingPlayer)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Peer wrapper");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = g_pDP8PeersList->RemoveFirstReference(pDP8Peer);
			if (sr != S_OK)
			{
				DPL(0, "Couldn't remove DirectPlay8Peer wrapper object from list!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't remove object from list)


			pDP8Peer->m_dwRefCount--; // release our reference
			if (pDP8Peer->m_dwRefCount == 0)
			{
				DPL(7, "Deleting DirectPlay8Peer wrapper object %x.",
					1, pDP8Peer);
				delete (pDP8Peer);
			} // end if (can delete the object)
			else
			{
				DPL(7, "Can't delete DirectPlay8Peer wrapper object %x, it's refcount is %u.",
					2, pDP8Peer,
					pDP8Peer->m_dwRefCount);
			} // end else (can't delete the object)
			pDP8Peer = NULL;
		} // end if (dropping player)
		TESTSECTION_ELSE
		{
			// Cleanup people still in session after last iteration.


			// For second in command cases, this will allocate one extra slot.
			// Oh well.
			LOCALALLOC_OR_THROW(DPNID*, padpnidDestroyTesters,
								((pTNecd->iNumMachines - 1) * sizeof (DPNID)));

			// Clear this, to be paranoid.
			context.pDP8Peer = NULL;


			// Shuffle the array of testers' player IDs so that it doesn't wrap
			// around.  We need to do this because the destroy session helper
			// expects the array to be contiguous.
			// We can overwrite the other testers array because it won't be used
			// again.
			i = 0;
			j = context.iCurrentHost + 1;
			if (context.fSecondInCommandCase)
				j++; // we dropped second in command, too
			j = j % pTNecd->iNumMachines; // don't start past end of array

			do
			{
				paiOtherTesters[i] = j;
				padpnidDestroyTesters[i] = context.padpnidTesters[j];

				i++;
				j = (j + 1) % pTNecd->iNumMachines;

				// Stop when we wrap around (copied all the testers we need).
				if (j == context.iCurrentHost)
					break;
			} // end do (while more tester IDs to copy)
			while (TRUE);

			



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Destroying peer-to-peer session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			// This is a quick and dirty way to be able to leave the session with
			// the possible REMOVE_PLAYER_FROM_GROUP and DESTROY_GROUP messages
			// generated by nametable unwinding being marked as "expected". 
			context.fDroppingPlayer = TRUE;

			LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_BBLDDP8_PEER_DESTROY));
			((PTID_BBLDDP8_PEER_DESTROY) pvSubInputData)->pDP8Peer = pDP8Peer;
			((PTID_BBLDDP8_PEER_DESTROY) pvSubInputData)->pHandlerContext = &context;
			((PTID_BBLDDP8_PEER_DESTROY) pvSubInputData)->padpnidTesters = padpnidDestroyTesters;

			sr = pTNecd->pExecutor->ExecSubTestCaseArray("3.1.1.2", pvSubInputData,
														sizeof (TID_BBLDDP8_PEER_DESTROY),
														((context.fSecondInCommandCase) ? (pTNecd->iNumMachines - 2) : (pTNecd->iNumMachines - 1)),
														paiOtherTesters);
			LocalFree(pvSubInputData);
			pvSubInputData = NULL;

			LocalFree(padpnidDestroyTesters);
			padpnidDestroyTesters = NULL;

			if (sr != S_OK)
			{
				DPL(0, "Couldn't execute sub test case BldSsn:BldPeer:Destroy!", 0);
				THROW_SYSTEMRESULT;
			} // end if (failed executing sub test case)

			GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.1.2",
										"Destroying peer-to-peer session failed!");


			pDP8Peer->m_dwRefCount--;
			if (pDP8Peer->m_dwRefCount == 0)
			{
				DPL(7, "Deleting DirectPlay8Peer wrapper object %x.", 1, pDP8Peer);
				delete (pDP8Peer);
			} // end if (can delete the object)
			else
			{
				DPL(0, "WARNING: Can't delete DirectPlay8Peer wrapper object %x, it's refcount is %u!?",
					2, pDP8Peer, pDP8Peer->m_dwRefCount);
			} // end else (can't delete the object)
			pDP8Peer = NULL;


			// Make sure the group operations were unwound, if necessary.
			if (context.dpnidPlayerAddedToGroup != 0)
			{
				DPL(0, "Player add to group ID wasn't cleared!", 0);
				SETTHROW_TESTRESULT(ERROR_NO_DATA);
			} // end if (player ID wasn't cleared)

			if (context.dpnidGroup != 0)
			{
				DPL(0, "Group ID wasn't cleared!", 0);
				SETTHROW_TESTRESULT(ERROR_NO_DATA);
			} // end if (group ID wasn't cleared)
		} // end else (not current dropping tester)
		TESTSECTION_ENDIF




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Making sure everyone survived final iteration");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->SyncWithTesters("Done", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;




		FINAL_SUCCESS;
	}
	END_TESTCASE



	if (pFaultSim != NULL)
	{
		temphr = pTNecd->pExecutor->ReleaseFaultSim(&pFaultSim);
		if (temphr != S_OK)
		{
			DPL(0, "Couldn't release fault sim %x!  0x%08x", 2, pFaultSim, temphr);
			
			OVERWRITE_SR_IF_OK(temphr);

			pFaultSim = NULL;
		} // end if (couldn't remove handler)
	} // end if (have fault sim)

	if (pDP8Peer != NULL)
	{
		temphr = pDP8Peer->DP8P_Close(0);
		if (temphr != DPN_OK)
		{
			DPL(0, "Closing peer interface failed!  0x%08x", 1, temphr);
			OVERWRITE_SR_IF_OK(temphr);
		} // end if (closing peer interface failed)


		// Ignore error, it may not actually be on the list.
		g_pDP8PeersList->RemoveFirstReference(pDP8Peer);


		pDP8Peer->m_dwRefCount--;
		if (pDP8Peer->m_dwRefCount == 0)
		{
			DPL(7, "Deleting DirectPlay8Peer wrapper object %x.", 1, pDP8Peer);
			delete (pDP8Peer);
		} // end if (can delete the object)
		else
		{
			DPL(0, "WARNING: Can't delete DirectPlay8Peer wrapper object %x, it's refcount is %u!?",
				2, pDP8Peer, pDP8Peer->m_dwRefCount);
		} // end else (can't delete the object)
		pDP8Peer = NULL;
	} // end if (have peer object)

	SAFE_LOCALFREE(pszURL);
	SAFE_RELEASE(pDP8AddressRemote);
	SAFE_RELEASE(pDP8AddressLocal);
	SAFE_LOCALFREE(pvSubInputData);
	SAFE_LOCALFREE(paiOtherTesters);
	SAFE_LOCALFREE(paiReliableMsgsSentToTesters);
	SAFE_CLOSEHANDLE(hConnectCompleteEvent);
	SAFE_CLOSEHANDLE(context.hPlayersGoneEvent);
	SAFE_CLOSEHANDLE(context.hHostMigratedEvent);
	SAFE_CLOSEHANDLE(context.hRejoinedEvent);
	SAFE_LOCALFREE(context.pafTesterIndicated);
	SAFE_LOCALFREE(context.padpnidTesters);
	SAFE_LOCALFREE(context.paiReliableMsgsReceivedFromTesters);
	SAFE_CLOSEHANDLE(context.hGroupOrDataOperationEvent);

	return (sr);
} // HostMigrateExec_All
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"HostMigrateDPNMessageHandler()"
//==================================================================================
// HostMigrateDPNMessageHandler
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
HRESULT HostMigrateDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT					hr = DPN_OK;
	PHOSTMIGRATECONTEXT		pContext = (PHOSTMIGRATECONTEXT) pvContext;
	DWORD					dwSize = 0;
	PDPN_PLAYER_INFO		pdpnpi = NULL;
	int						iTesterNum;
	BOOL					fFound;


	switch (dwMsgType)
	{
		case DPN_MSGID_ADD_PLAYER_TO_GROUP:
			PDPNMSG_ADD_PLAYER_TO_GROUP		pAddPlayerToGroupMsg;


			pAddPlayerToGroupMsg = (PDPNMSG_ADD_PLAYER_TO_GROUP) pvMsg;


			// Make sure we're supposed to get this message.
			if (pContext->fDroppingPlayer)
			{
				if (pContext->hRejoinedEvent == NULL)
				{
					if (pContext->iGroupOrDataOperation == GROUPORDATAOP_ADDPLAYERTOGROUP)
					{
						DPL(0, "ADD_PLAYER_TO_GROUP sneaked in while we were leaving session, we had better get a REMOVE_PLAYER_FROM_GROUP shortly.", 0);
					} // end if (adding player to group)
					else
					{
						DPL(0, "Got unexpected DPN_MSGID_ADD_PLAYER_TO_GROUP (msg = %x, group/data op = %i) while dropping!  DEBUGBREAK()-ing!",
							2, pAddPlayerToGroupMsg, pContext->iGroupOrDataOperation);
						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					} // end else (some other group/data operation)
				} // end if (dropping)
				else
				{
					if (pContext->iGroupOrDataOperation == GROUPORDATAOP_ADDPLAYERTOGROUP)
					{
						DPL(1, "Previously dropped player got expected ADD_PLAYER_TO_GROUP during rejoin (group op = %i).",
							1, pContext->iGroupOrDataOperation);
					} // end if (adding player to group)
					else
					{
						DPL(0, "Got unexpected DPN_MSGID_ADD_PLAYER_TO_GROUP (msg = %x, group/data op = %i) while rejoining!  DEBUGBREAK()-ing!",
							2, pAddPlayerToGroupMsg, pContext->iGroupOrDataOperation);
						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					} // end else (some other group/data operation)
				} // end else (rejoining)
			} // end if (dropping player)
			else
			{
				if (pContext->iGroupOrDataOperation != GROUPORDATAOP_ADDPLAYERTOGROUP)
				{
					DPL(0, "Got unexpected DPN_MSGID_ADD_PLAYER_TO_GROUP (msg = %x, group op = %i)!  DEBUGBREAK()-ing!",
						2, pAddPlayerToGroupMsg, pContext->iGroupOrDataOperation);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (operation unexpected)
			} // end else (not dropping player)


			// Validate the message
			if (pAddPlayerToGroupMsg->dpnidGroup != pContext->dpnidGroup)
			{
				DPL(0, "Group ID is wrong (%u/%x != %u/%x)!  DEBUGBREAK()-ing!",
					4, pAddPlayerToGroupMsg->dpnidGroup,
					pAddPlayerToGroupMsg->dpnidGroup,
					pContext->dpnidGroup, pContext->dpnidGroup);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (group ID is wrong)

			iTesterNum = (pContext->iCurrentHost + pContext->pTNecd->iNumMachines - 1) % pContext->pTNecd->iNumMachines;
			if (pAddPlayerToGroupMsg->dpnidPlayer != pContext->padpnidTesters[iTesterNum])
			{
				DPL(0, "Player ID is wrong (%u/%x != %u/%x)!  DEBUGBREAK()-ing!",
					4, pAddPlayerToGroupMsg->dpnidPlayer,
					pAddPlayerToGroupMsg->dpnidPlayer,
					pContext->padpnidTesters[iTesterNum],
					pContext->padpnidTesters[iTesterNum]);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (player ID is wrong)

			// Check for duplicates.
			if (pContext->dpnidPlayerAddedToGroup != 0)
			{
				DPL(0, "Getting duplicate DPN_MSGID_ADD_PLAYER_TO_GROUP (already received ID %u/%x, getting %u/%x)!  DEBUGBREAK()-ing!",
					4, pContext->dpnidPlayerAddedToGroup,
					pContext->dpnidPlayerAddedToGroup,
					pAddPlayerToGroupMsg->dpnidPlayer,
					pAddPlayerToGroupMsg->dpnidPlayer);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already stored ID)

			pContext->dpnidPlayerAddedToGroup = pAddPlayerToGroupMsg->dpnidPlayer;



			// Notify the main thread, if necessary.
			if (pContext->fGroupOrDataOperatingPlayer)
			{
				DPL(1, "Not setting event after ADD_PLAYER_TO_GROUP, async op completion should trigger for group/data operating player (tester %i).",
					1, pContext->pTNecd->iTesterNum);
			} // end if (group/data operating player)
			else if (! pContext->fDroppingPlayer)
			{
				DPL(1, "Got expected ADD_PLAYER_TO_GROUP on tester %i, notifying main thread.",
					1, pContext->pTNecd->iTesterNum);

				if (! SetEvent(pContext->hGroupOrDataOperationEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set group or data operation event (%x)!  DEBUGBREAK()-ing.  0x%08x",
						2, pContext->hGroupOrDataOperationEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;
				} // end if (couldn't set event)
			} // end else if (not group/data operating or dropping player)
		  break;

		case DPN_MSGID_ASYNC_OP_COMPLETE:
			PDPNMSG_ASYNC_OP_COMPLETE	pAsyncOpCompleteMsg;


			pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE) pvMsg;


			if (! pContext->fGroupOrDataOperatingPlayer)
			{
				DPL(0, "Getting async op completion on unexpected tester (%i)!  DEBUGBREAK()-ing!",
					1, pContext->pTNecd->iTesterNum);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (we're not the instigator)
			
			if (pContext->iGroupOrDataOperation == GROUPORDATAOP_REMOVEPLAYERFROMGROUP)
			{
				if ((pAsyncOpCompleteMsg->hResultCode != DPN_OK) &&
					(pAsyncOpCompleteMsg->hResultCode != DPNERR_INVALIDPLAYER) &&
					(pAsyncOpCompleteMsg->hResultCode != DPNERR_PLAYERNOTINGROUP))
				{
					DPL(0, "Async op didn't complete with OK, INVALIDPLAYER, or PLAYERNOTINGROUP!  DEBUGBREAK()-ing.  0x%08x",
						1, hr);
					DEBUGBREAK();
					hr = pAsyncOpCompleteMsg->hResultCode;
					goto DONE;
				} // end if (result code is not OK)
			} // end if (remove player from group)
			else
			{
				if (pAsyncOpCompleteMsg->hResultCode != DPN_OK)
				{
					DPL(0, "Async op completed with a failure!  DEBUGBREAK()-ing.  0x%08x",
						1, hr);
					DEBUGBREAK();
					hr = pAsyncOpCompleteMsg->hResultCode;
					goto DONE;
				} // end if (result code is not OK)
			} // end else (not remove player from group)

			if (! SetEvent(pContext->hGroupOrDataOperationEvent))
			{
				hr = GetLastError();

				DPL(0, "Couldn't set group or data operation event (%x)!  DEBUGBREAK()-ing.  0x%08x",
					2, pContext->hGroupOrDataOperationEvent, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;
			

#pragma TODO(vanceo, "Just fail test")
			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				DPL(0, "Failed connecting!  DEBUGBREAK()-ing.  0x%08x", 1, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
			}


			// The connect context was a message handle, so set it.
			if (! SetEvent((HANDLE) pConnectCompleteMsg->pvUserContext))
			{
				hr = GetLastError();

				DPL(0, "Couldn't set ConnectCompletion event (%x)!  DEBUGBREAK()-ing.  0x%08x",
					2, pConnectCompleteMsg->pvUserContext, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				//goto DONE;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_CREATE_GROUP:
			PDPNMSG_CREATE_GROUP	pCreateGroupMsg;


			pCreateGroupMsg = (PDPNMSG_CREATE_GROUP) pvMsg;

			// Make sure we're supposed to get this message.
			if (pContext->fDroppingPlayer)
			{
				if (pContext->hRejoinedEvent == NULL)
				{
					if (pContext->iGroupOrDataOperation == GROUPORDATAOP_CREATEGROUP)
					{
						DPL(0, "CREATE_GROUP (ID %u/%x) sneaked in while we were leaving session, we had better get a DESTROY_GROUP shortly.",
							2, pCreateGroupMsg->dpnidGroup,
							pCreateGroupMsg->dpnidGroup);
					} // end if (adding player to group)
					else
					{
						DPL(0, "Got unexpected DPN_MSGID_CREATE_GROUP (msg = %x, group/data op = %i) while dropping!  DEBUGBREAK()-ing!",
							2, pCreateGroupMsg, pContext->iGroupOrDataOperation);
						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					} // end else (some other group/data operation)
				} // end if (dropping)
				else
				{
					if ((pContext->iGroupOrDataOperation != GROUPORDATAOP_SETPEERINFO) &&
						(pContext->iGroupOrDataOperation != GROUPORDATAOP_DESTROYGROUP))
					{
						DPL(1, "Dropping player got expected CREATE_GROUP (ID %u/%x) during rejoin (group/data op = %i).",
							3, pCreateGroupMsg->dpnidGroup,
							pCreateGroupMsg->dpnidGroup,
							pContext->iGroupOrDataOperation);
					} // end if (adding player to group)
					else
					{
						DPL(0, "Got unexpected DPN_MSGID_CREATE_GROUP (msg = %x, group/data op = %i) while rejoining!  DEBUGBREAK()-ing!",
							2, pCreateGroupMsg, pContext->iGroupOrDataOperation);
						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					} // end else (some other group operation)
				} // end else (rejoining)
			} // end if (dropping player)
			else
			{
				if (pContext->iGroupOrDataOperation != GROUPORDATAOP_CREATEGROUP)
				{
					DPL(0, "Got unexpected DPN_MSGID_CREATE_GROUP (msg = %x, group/data op = %i)!  DEBUGBREAK()-ing!",
						2, pCreateGroupMsg, pContext->iGroupOrDataOperation);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (operation unexpected)
			} // end else (not dropping player)


			// Check for duplicates.
			if (pContext->dpnidGroup != 0)
			{
				DPL(0, "Getting duplicate DPN_MSGID_CREATE_GROUP (already received ID %u/%x, getting %u/%x)!  DEBUGBREAK()-ing!",
					4, pContext->dpnidGroup,
					pContext->dpnidGroup,
					pCreateGroupMsg->dpnidGroup,
					pCreateGroupMsg->dpnidGroup);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already stored ID)

			pContext->dpnidGroup = pCreateGroupMsg->dpnidGroup;



			// Notify the main thread, if necessary.
			if (pContext->fGroupOrDataOperatingPlayer)
			{
				DPL(1, "Not setting event after CREATE_GROUP, async op completion should trigger for group/data operating player (tester %i).",
					1, pContext->pTNecd->iTesterNum);
			} // end if (group/data operating player)
			else if (! pContext->fDroppingPlayer)
			{
				DPL(1, "Got expected CREATE_GROUP on tester %i, notifying main thread.",
					1, pContext->pTNecd->iTesterNum);

				if (! SetEvent(pContext->hGroupOrDataOperationEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set group or data operation event (%x)!  DEBUGBREAK()-ing.  0x%08x",
						2, pContext->hGroupOrDataOperationEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;
				} // end if (couldn't set event)
			} // end else if (not group/data operating or dropping player)
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;
			
			// If we're one of the the players who dropped, but we're rejoining now,
			// make sure the nametable we're rebuilding is correct.
			if ((pContext->fDroppingPlayer) && (pContext->fSelfIndicate))
			{
				hr = pContext->pDP8Peer->DP8P_GetPeerInfo(pCreatePlayerMsg->dpnidPlayer,
															NULL,
															&dwSize,
															0);
				if (hr != DPNERR_BUFFERTOOSMALL)
				{
					DPL(0, "Getting peer info didn't return expected error BUFFERTOOSMALL!  DEBUGBREAK()-ing.  0x%08x",
						1, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					goto DONE;
				} // end if (didn't return expected error)

				pdpnpi = (PDPN_PLAYER_INFO) MemAlloc(dwSize);
				if (pdpnpi == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)
				pdpnpi->dwSize = sizeof (DPN_PLAYER_INFO);

				hr = pContext->pDP8Peer->DP8P_GetPeerInfo(pCreatePlayerMsg->dpnidPlayer,
															pdpnpi,
															&dwSize,
															0);
				if (hr != DPN_OK)
				{
					DPL(0, "Getting peer info failed!  DEBUGBREAK()-ing.  0x%08x", 1, hr);

					DEBUGBREAK();

					goto DONE;
				} // end if (didn't return expected error)

				if (pdpnpi->dwPlayerFlags & DPNPLAYER_LOCAL)
				{
					if (pContext->pafTesterIndicated[pContext->pTNecd->iTesterNum])
					{
						DPL(0, "Got duplicate CREATE_PLAYER for the local player (tester %i, prev player ID = %u/%x, new player ID = %u/%x)!  DEBUGBREAK()-ing.",
							5, pContext->pTNecd->iTesterNum,
							pContext->padpnidTesters[pContext->pTNecd->iTesterNum],
							pContext->padpnidTesters[pContext->pTNecd->iTesterNum],
							pCreatePlayerMsg->dpnidPlayer,
							pCreatePlayerMsg->dpnidPlayer);

						//Ignore error
						pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
														"Got duplicate CREATE_PLAYER for the local player!",
														0);

						DEBUGBREAK();

						hr = E_FAIL;
						goto DONE;
					} // end if (already found the player)

					iTesterNum = pContext->pTNecd->iTesterNum;

					DPL(1, "Local tester (%i, player ID %u/%x) has connected.",
						3, iTesterNum, pCreatePlayerMsg->dpnidPlayer,
						pCreatePlayerMsg->dpnidPlayer);
				} // end if (it's our local player)
				else
				{
					fFound = FALSE;

					// Lookup this ID in the list of IDs everyone who had previously
					// joined (except ourselves).
					for(iTesterNum = 0; iTesterNum <= pContext->pTNecd->iNumMachines; iTesterNum++)
					{
						// Skip our ID if this is it.
						if (iTesterNum == pContext->pTNecd->iTesterNum)
							continue;

						// If we found the tester ID
						if (pContext->padpnidTesters[iTesterNum] == pCreatePlayerMsg->dpnidPlayer)
						{
							// If that player already connected, something's wrong.
							if (pContext->pafTesterIndicated[iTesterNum])
							{
								DPL(0, "Got duplicate CREATE_PLAYER for tester %i (player ID %u/%x)!  DEBUGBREAK()-ing.",
									3, iTesterNum, pCreatePlayerMsg->dpnidPlayer,
									pCreatePlayerMsg->dpnidPlayer);

								//Ignore error
								pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
																"Got duplicate CREATE_PLAYER for tester %i!",
																1, iTesterNum);

								DEBUGBREAK();

								hr = E_FAIL;
								goto DONE;
							} // end if (player already connected)


							DPL(1, "Tester %i (player ID %u/%x) has connected.",
								3, iTesterNum, pCreatePlayerMsg->dpnidPlayer,
								pCreatePlayerMsg->dpnidPlayer);

							fFound = TRUE;
							break;
						} // end if (found)
					} // end for (each tester who's already joined)


					// If we didn't recognize that player ID, we're screwed.
					if (! fFound)
					{
						DPL(0, "Got CREATE_PLAYER for unexpected player ID %u/%x!  DEBUGBREAK()-ing.",
							2, pCreatePlayerMsg->dpnidPlayer,
							pCreatePlayerMsg->dpnidPlayer);

						//Ignore error
						pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
														"Got CREATE_PLAYER for unexpected player ID %u/%x!",
														2, pCreatePlayerMsg->dpnidPlayer,
														pCreatePlayerMsg->dpnidPlayer);

						DEBUGBREAK();

						hr = E_FAIL;
						goto DONE;
					} // end if (didn't find player ID)
				} // end else (it's some other player)


				// Store the player ID.
				pContext->padpnidTesters[iTesterNum] = pCreatePlayerMsg->dpnidPlayer;

				// Associate the context with this player.
				pCreatePlayerMsg->pvPlayerContext = &(pContext->paiReliableMsgsReceivedFromTesters[iTesterNum]);

				DPL(8, "Player context is now %x.", 1, pCreatePlayerMsg->pvPlayerContext);


				pContext->pafTesterIndicated[iTesterNum] = TRUE;


				// See if that's the last person we're waiting on to connect.
				for(iTesterNum = 0; iTesterNum < pContext->pTNecd->iNumMachines; iTesterNum++)
				{
					if (! pContext->pafTesterIndicated[iTesterNum])
					{
						// If we're the original host in a second-in-command case, and
						// we're just rejoining, then the original second-in-command
						// tester won't be in the session and therefore we shouldn't
						// wait for his CREATE_PLAYER.
						if ((pContext->fSecondInCommandCase) &&
							(pContext->pTNecd->iTesterNum == pContext->iCurrentHost) &&
							(pContext->fSelfIndicate) &&
							(iTesterNum == ((pContext->pTNecd->iTesterNum + 1) % pContext->pTNecd->iNumMachines)))
						{
							continue;
						} // end if (can skip second-in-command)

						DPL(4, "Still expecting tester %i to connect, not notifying main thread.",
							1, iTesterNum);
						// No sense in looking further, just return OK.
						goto DONE;
					} // end if (tester hasn't connected yet)
				} // end for (each tester who's already joined)
			} // end if (we're a rejoining player)
			else
			{
				// Start by assuming it's the rejoining original host.
				iTesterNum = pContext->iCurrentHost;

				if (pContext->padpnidTesters[pContext->iCurrentHost] != 0)
				{
					if (pContext->fSecondInCommandCase)
					{
						// Try looking at the rejoining original second-in-command.
						iTesterNum = (pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines;

						if (pContext->padpnidTesters[iTesterNum] != 0)
						{
							DPL(0, "Got duplicate CREATE_PLAYER (ID = %u/%x) for a rejoining player (original host tester %i player ID = %u/%x, original second-in-command tester %i player ID = %u/%x)!  DEBUGBREAK()-ing.",
								8, pCreatePlayerMsg->dpnidPlayer,
								pCreatePlayerMsg->dpnidPlayer,
								pContext->iCurrentHost,
								pContext->padpnidTesters[pContext->iCurrentHost],
								pContext->padpnidTesters[pContext->iCurrentHost],
								iTesterNum,
								pContext->padpnidTesters[iTesterNum],
								pContext->padpnidTesters[iTesterNum]);

							//Ignore error
							pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
															"Got duplicate CREATE_PLAYER for a rejoining player ID %u/%x!",
															2, pCreatePlayerMsg->dpnidPlayer,
															pCreatePlayerMsg->dpnidPlayer);

							DEBUGBREAK();

							hr = E_FAIL;
							goto DONE;
						} // end if (original second in command already joined)


						DPL(1, "Original second-in-command (tester %i)'s rejoin player ID = %u/%x.",
							3, iTesterNum, pCreatePlayerMsg->dpnidPlayer,
							pCreatePlayerMsg->dpnidPlayer);
					} // end if (second in command case)
					else
					{
						DPL(0, "Got duplicate CREATE_PLAYER for rejoining player (tester %i, prev player ID = %u/%x, new player ID = %u/%x)!  DEBUGBREAK()-ing.",
							5, pContext->iCurrentHost,
							pContext->padpnidTesters[pContext->iCurrentHost],
							pContext->padpnidTesters[pContext->iCurrentHost],
							pCreatePlayerMsg->dpnidPlayer,
							pCreatePlayerMsg->dpnidPlayer);

						//Ignore error
						pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
														"Got duplicate CREATE_PLAYER for rejoining player ID %u/%x!",
														2, pCreatePlayerMsg->dpnidPlayer,
														pCreatePlayerMsg->dpnidPlayer);

						DEBUGBREAK();

						hr = E_FAIL;
						goto DONE;
					} // end else (not second in command case)
				} // end if (already have the rejoining host's ID)


				// Store the player ID.
				pContext->padpnidTesters[iTesterNum] = pCreatePlayerMsg->dpnidPlayer;

				// Associate the context with this player.
				pCreatePlayerMsg->pvPlayerContext = &(pContext->paiReliableMsgsReceivedFromTesters[iTesterNum]);
	

				DPL(8, "Player context is now %x.", 1, pCreatePlayerMsg->pvPlayerContext);
			} // end else (we're not the joining player)



			// If we got here, it means all the testers that should have connected
			// (including ourselves, if necessary) did.  Notify the main thread.

			DPL(2, "All testers expected to connect actually have, notifying main thread.", 0);

			if (! SetEvent(pContext->hRejoinedEvent))
			{
				hr = GetLastError();

				DPL(0, "Couldn't set rejoined event (%x)!  DEBUGBREAK()-ing.  0x%08x",
					2, pContext->hRejoinedEvent, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				goto DONE;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_DESTROY_GROUP:
			PDPNMSG_DESTROY_GROUP	pDestroyGroupMsg;


			pDestroyGroupMsg = (PDPNMSG_DESTROY_GROUP) pvMsg;

			// Make sure we're supposed to get this message.
			if (pContext->fDroppingPlayer)
			{
				switch (pContext->iGroupOrDataOperation)
				{
					case GROUPORDATAOP_CREATEGROUP:
						DPL(0, "Got DESTROY_GROUP, assuming it's for an CREATE_GROUP that sneaked in.", 0);
					  break;

					case GROUPORDATAOP_SETGROUPINFO:
					case GROUPORDATAOP_ADDPLAYERTOGROUP:
					case GROUPORDATAOP_REMOVEPLAYERFROMGROUP:
						DPL(1, "Dropping player got DESTROY_GROUP (group/data op = %i).",
							1, pContext->iGroupOrDataOperation);
					  break;

					case GROUPORDATAOP_DESTROYGROUP:
						DPL(0, "Dropping player got DESTROY_GROUP that may or may not have sneaked in.", 0);
					  break;

					default:
						DPL(0, "Got unexpected DPN_MSGID_DESTROY_GROUP (msg = %x, group/data op = %i)!  DEBUGBREAK()-ing!",
							2, pDestroyGroupMsg, pContext->iGroupOrDataOperation);
						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					  break;
				} // end switch (on group/data operation)
			} // end if (dropping player)
			else
			{
				if (pContext->iGroupOrDataOperation != GROUPORDATAOP_DESTROYGROUP)
				{
					DPL(0, "Got unexpected DPN_MSGID_DESTROY_GROUP (msg = %x, group/data op = %i)!  DEBUGBREAK()-ing!",
						2, pDestroyGroupMsg, pContext->iGroupOrDataOperation);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (operation unexpected)
			} // end else (not dropping player)


			// Check for duplicates and invalid group IDs.
			if (pDestroyGroupMsg->dpnidGroup != pContext->dpnidGroup)
			{
				DPL(0, "Getting duplicate or wrong DPN_MSGID_DESTROY_GROUP (group ID %u/%x != %u/%x)!  DEBUGBREAK()-ing!",
					4 ,pDestroyGroupMsg->dpnidGroup,
					pDestroyGroupMsg->dpnidGroup,
					pContext->dpnidGroup,
					pContext->dpnidGroup);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (group ID is unexpected)

			pContext->dpnidGroup = 0;


			// Notify the main thread, if necessary.
			if (pContext->fGroupOrDataOperatingPlayer)
			{
				DPL(1, "Not setting event after DESTROY_GROUP, async op completion should trigger for group/data operating player (tester %i).",
					1, pContext->pTNecd->iTesterNum);
			} // end if (group/data operating player)
			else if (! pContext->fDroppingPlayer)
			{
				DPL(1, "Got expected DESTROY_GROUP on tester %i, notifying main thread.",
					1, pContext->pTNecd->iTesterNum);

				if (! SetEvent(pContext->hGroupOrDataOperationEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set group or data operation event (%x)!  DEBUGBREAK()-ing.  0x%08x",
						2, pContext->hGroupOrDataOperationEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;
				} // end if (couldn't set event)
			} // end else if (not group/data operating or dropping player)
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			if (pContext->hPlayersGoneEvent == NULL)
			{
				DPL(0, "WARNING: Ignoring DPN_MSGID_DESTROY_PLAYER message because no PlayersGone event, this had better be because of a test failure!", 0);
			} // end if (no players gone event)
			else
			{
				// If we're a dropping player, then we should be seeing all the
				// players be removed from the session.  If not, just the dropping
				// players should be going away.
				if (pContext->fDroppingPlayer)
				{
					fFound = FALSE;

					// Lookup this ID in the list of testers' player IDs.
					for(iTesterNum = 0; iTesterNum <= pContext->pTNecd->iNumMachines; iTesterNum++)
					{
						// If we're not waiting for our local player to disconnect,
						// and this is currently our tester number, skip it.
						if ((! pContext->fSelfIndicate) &&
							(iTesterNum == pContext->pTNecd->iTesterNum))
						{
							continue;
						} // end if (won't wait for self to disconnect and this is self)


						// If we found the tester ID...
						if (pContext->padpnidTesters[iTesterNum] == pDestroyPlayerMsg->dpnidPlayer)
						{
							// If that player already disconnected, something's
							// wrong.
							if (pContext->pafTesterIndicated[iTesterNum])
							{
								DPL(0, "Got duplicate DESTROY_PLAYER for tester %i (player ID %u/%x)!  DEBUGBREAK()-ing.",
									3, iTesterNum, pDestroyPlayerMsg->dpnidPlayer,
									pDestroyPlayerMsg->dpnidPlayer);

								//Ignore error
								pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
																"Got duplicate DESTROY_PLAYER for tester %i!",
																1, iTesterNum);

								DEBUGBREAK();

								hr = E_FAIL;
								goto DONE;
							} // end if (player already connected)

							// Mark him as disconnected now.
							pContext->pafTesterIndicated[iTesterNum] = TRUE;

							DPL(1, "Tester %i (player ID %u/%x) has disconnected.",
								3, iTesterNum, pDestroyPlayerMsg->dpnidPlayer,
								pDestroyPlayerMsg->dpnidPlayer);

							fFound = TRUE;
							break;
						} // end if (found)
					} // end for (each tester who's already joined)


					// If we didn't recognize that player ID, we're screwed.
					if (! fFound)
					{
						DPL(0, "Got DESTROY_PLAYER for unexpected player ID %u/%x!  DEBUGBREAK()-ing.",
							2, pDestroyPlayerMsg->dpnidPlayer,
							pDestroyPlayerMsg->dpnidPlayer);

						//Ignore error
						pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
														"Got DESTROY_PLAYER for unexpected player ID %u/%x!",
														2, pDestroyPlayerMsg->dpnidPlayer,
														pDestroyPlayerMsg->dpnidPlayer);

						DEBUGBREAK();

						hr = E_FAIL;
						goto DONE;
					} // end if (didn't find player ID)


					// See if that's the last person we're waiting on to disconnect.
					for(iTesterNum = 0; iTesterNum < pContext->pTNecd->iNumMachines; iTesterNum++)
					{
						// If we're not waiting for our local player to disconnect,
						// and this is currently our tester number, skip it.
						if ((! pContext->fSelfIndicate) &&
							(iTesterNum == pContext->pTNecd->iTesterNum))
						{
							continue;
						} // end if (won't wait for self to disconnect and this is self)


						if (! pContext->pafTesterIndicated[iTesterNum])
						{
							DPL(4, "Still expecting tester %i to disconnect, not notifying main thread.",
								1, iTesterNum);
							// No sense in looking further, just return OK.
							goto DONE;
						} // end if (tester hasn't connected yet)
					} // end for (each tester who's already joined)
				} // end if (current leaving host)
				else
				{
					// Make sure the leaving player is the old host.
					if (pDestroyPlayerMsg->dpnidPlayer != pContext->padpnidTesters[pContext->iCurrentHost])
					{
						if (pContext->fSecondInCommandCase)
						{
							if (pDestroyPlayerMsg->dpnidPlayer != pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines])
							{
								DPL(0, "Got DESTROY_PLAYER for unexpected player ID %u/%x (leaving host (tester %i) is ID %u/%x, second in command (tester %i) is ID %u/%x)!  DEBUGBREAK()-ing.",
									8, pDestroyPlayerMsg->dpnidPlayer,
									pDestroyPlayerMsg->dpnidPlayer,
									pContext->iCurrentHost,
									pContext->padpnidTesters[pContext->iCurrentHost],
									pContext->padpnidTesters[pContext->iCurrentHost],
									((pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines),
									pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines],
									pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines]);

								//Ignore error
								pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
																"Got DESTROY_PLAYER for unexpected player ID %u/%x!",
																2, pDestroyPlayerMsg->dpnidPlayer,
																pDestroyPlayerMsg->dpnidPlayer);

								DEBUGBREAK();

								hr = E_FAIL;
								goto DONE;
							} // end if (leaving player isn't second in command)
							else
							{
								if (pContext->pafTesterIndicated[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines])
								{
									DPL(0, "Got duplicate DESTROY_PLAYER for second-in-command tester %i, player ID %u/%x!  DEBUGBREAK()-ing.",
										3, ((pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines),
										pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines],
										pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines]);

									//Ignore error
									pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
																	"Got duplicate DESTROY_PLAYER for second-in-command tester (ID %u/%x)!",
																	2, pDestroyPlayerMsg->dpnidPlayer,
																	pDestroyPlayerMsg->dpnidPlayer);

									DEBUGBREAK();

									hr = E_FAIL;
									goto DONE;
								} // end if (tester already disconnected)

								// Mark second-in-command as disconnected now.
								pContext->pafTesterIndicated[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines] = TRUE;

								if (! pContext->pafTesterIndicated[pContext->iCurrentHost])
								{
									DPL(4, "Still expecting host (tester %i) to disconnect, not notifying main thread.",
										1, pContext->iCurrentHost);
									goto DONE;
								} // end if (tester hasn't connected yet)
							} // end else (leaving player is host)
						} // end if (second in command case)
						else
						{
							DPL(0, "Got DESTROY_PLAYER for unexpected player ID %u/%x (leaving host (tester %i) is ID %u/%x)!  DEBUGBREAK()-ing",
								5, pDestroyPlayerMsg->dpnidPlayer,
								pDestroyPlayerMsg->dpnidPlayer,
								pContext->iCurrentHost,
								pContext->padpnidTesters[pContext->iCurrentHost],
								pContext->padpnidTesters[pContext->iCurrentHost]);

							//Ignore error
							pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
															"Got DESTROY_PLAYER for unexpected player ID %u/%x!",
															2, pDestroyPlayerMsg->dpnidPlayer,
															pDestroyPlayerMsg->dpnidPlayer);

							DEBUGBREAK();

							hr = E_FAIL;
							goto DONE;
						} // end else (not second in command case)
					} // end if (leaving player isn't host)
					else
					{
						if (pContext->pafTesterIndicated[pContext->iCurrentHost])
						{
							DPL(0, "Got duplicate DESTROY_PLAYER for original host tester %i, player ID %u/%x!  DEBUGBREAK()-ing.",
								3, pContext->iCurrentHost,
								pContext->padpnidTesters[pContext->iCurrentHost],
								pContext->padpnidTesters[pContext->iCurrentHost]);

							//Ignore error
							pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
															"Got duplicate DESTROY_PLAYER for original host tester (ID %u/%x)!",
															2, pDestroyPlayerMsg->dpnidPlayer,
															pDestroyPlayerMsg->dpnidPlayer);

							DEBUGBREAK();

							hr = E_FAIL;
							goto DONE;
						} // end if (tester already disconnected)

						// Mark original host as disconnected now.
						pContext->pafTesterIndicated[pContext->iCurrentHost] = TRUE;

						if ((pContext->fSecondInCommandCase) &&
							(! pContext->pafTesterIndicated[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines]))
						{
							DPL(4, "Still expecting second in command (tester %i) to disconnect, not notifying main thread.",
								1, (pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines);
							goto DONE;
						} // end if (tester hasn't connected yet)
					} // end else (leaving player is host)
				} // end else (not current leaving host)



				DPL(2, "All testers expected to disconnect actually have, notifying main thread.", 0);

				if (! SetEvent(pContext->hPlayersGoneEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set PlayersGone event (%x)!  DEBUGBREAK()-ing.  0x%08x",
						2, pContext->hPlayersGoneEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;
				} // end if (couldn't set players gone event)
			} // end else (players gone event)
		  break;

		case DPN_MSGID_GROUP_INFO:
			PDPNMSG_GROUP_INFO	pGroupInfoMsg;


			pGroupInfoMsg = (PDPNMSG_GROUP_INFO) pvMsg;

			// Make sure we're supposed to get this message.
			if (pContext->fDroppingPlayer)
			{
				if (pContext->iGroupOrDataOperation == GROUPORDATAOP_SETGROUPINFO)
				{
					DPL(0, "GROUP_INFO sneaked in while we were leaving session.", 0);
				} // end if (setting peer info)
				else
				{
					DPL(0, "Got unexpected DPN_MSGID_GROUP_INFO (msg = %x, group/data op = %i)!  DEBUGBREAK()-ing!",
						2, pGroupInfoMsg, pContext->iGroupOrDataOperation);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end else (some other group/data operation)
			} // end if (dropping player)
			else
			{
				if (pContext->iGroupOrDataOperation != GROUPORDATAOP_SETGROUPINFO)
				{
					DPL(0, "Got unexpected DPN_MSGID_GROUP_INFO (msg = %x, group/data op = %i)!  DEBUGBREAK()-ing!",
						2, pGroupInfoMsg, pContext->iGroupOrDataOperation);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (operation unexpected)
			} // end else (not dropping player)


			// Validate the message
			if (pGroupInfoMsg->dwSize != sizeof (DPNMSG_GROUP_INFO))
			{
				DPL(0, "Group info indication structure size is wrong (%u != %u)!  DEBUGBREAK()-ing!",
					2, pGroupInfoMsg->dwSize, sizeof (DPNMSG_GROUP_INFO));
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (structure size wrong)

			if (pGroupInfoMsg->dpnidGroup != pContext->dpnidGroup)
			{
				DPL(0, "Group info ID is wrong (%u/%x != %u/%x)!  DEBUGBREAK()-ing!",
					2, pGroupInfoMsg->dpnidGroup, pContext->dpnidGroup);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (group ID is wrong)


			// Check for duplicates.
			if (pContext->fGroupInfoSet)
			{
				DPL(0, "Getting duplicate DPN_MSGID_GROUP_INFO (%x)!  DEBUGBREAK()-ing!",
					1, pGroupInfoMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already got info)

			pContext->fGroupInfoSet = TRUE;



			// Notify the main thread, if necessary.
			if (pContext->fGroupOrDataOperatingPlayer)
			{
				DPL(1, "Not setting event after GROUP_INFO, async op completion should trigger for group/data operating player (tester %i).",
					1, pContext->pTNecd->iTesterNum);
			} // end if (group/data operating player)
			else if (! pContext->fDroppingPlayer)
			{
				DPL(1, "Got expected GROUP_INFO on tester %i, notifying main thread.",
					1, pContext->pTNecd->iTesterNum);

				if (! SetEvent(pContext->hGroupOrDataOperationEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set group or data operation event (%x)!  DEBUGBREAK()-ing.  0x%08x",
						2, pContext->hGroupOrDataOperationEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;
				} // end if (couldn't set event)
			} // end else if (not group/data operating or dropping player)
		  break;

		case DPN_MSGID_HOST_MIGRATE:
			PDPNMSG_HOST_MIGRATE	pHostMigrateMsg;


			pHostMigrateMsg = (PDPNMSG_HOST_MIGRATE) pvMsg;

			if (pContext->hHostMigratedEvent == NULL)
			{
				DPL(0, "WARNING: Ignoring DPN_MSGID_HOST_MIGRATE message because no HostMigrated event, this had better be because of a test failure!", 0);
			} // end if (host migrated event)
			else
			{
				if (pContext->fSecondInCommandCase)
				{
					if (pHostMigrateMsg->dpnidNewHost != pContext->padpnidTesters[(pContext->iCurrentHost + 2) % pContext->pTNecd->iNumMachines])
					{
						if (pHostMigrateMsg->dpnidNewHost != pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines])
						{
							DPL(0, "Host didn't migrate to either expected machine (old second in command tester %i, player ID %u/%x, new host tester %i, player ID %u/%x), it migrated to player ID %u/%x!  DEBUGBREAK()-ing.",
								8, (pContext->iCurrentHost + 1),
								pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines],
								pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines],
								(pContext->iCurrentHost + 2),
								pContext->padpnidTesters[(pContext->iCurrentHost + 2) % pContext->pTNecd->iNumMachines],
								pContext->padpnidTesters[(pContext->iCurrentHost + 2) % pContext->pTNecd->iNumMachines],
								pHostMigrateMsg->dpnidNewHost,
								pHostMigrateMsg->dpnidNewHost);

							//Ignore error
							pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
															"Host didn't migrate to either expected machine!  DEBUGBREAK()-ing.",
															0);

							DEBUGBREAK();

							hr = E_FAIL;
							goto DONE;
						} // end if (host didn't migrate to second in command)
						else
						{
#pragma TODO(vanceo, "Make sure real comes after old second in command")

							DPL(0, "WARNING: Host migrated to old second in command (tester %i, player ID %u/%x) it had better not have already migrated to the real new host (tester %i, player ID %u/%x), but should do it next!",
								6, (pContext->iCurrentHost + 1),
								pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines],
								pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines],
								(pContext->iCurrentHost + 2),
								pContext->padpnidTesters[(pContext->iCurrentHost + 2) % pContext->pTNecd->iNumMachines],
								pContext->padpnidTesters[(pContext->iCurrentHost + 2) % pContext->pTNecd->iNumMachines]);

							// We're going to ignore it.
							goto DONE;
						} // end else (host did migrate to second in command)
					} // end if (host didn't migrate to expected machine)

#pragma TODO(vanceo, "Check for dupes")

					DPL(1, "Host migrated to tester %i (player ID %u/%x) as expected.",
						3, ((pContext->iCurrentHost + 2) % pContext->pTNecd->iNumMachines),
						pHostMigrateMsg->dpnidNewHost,
						pHostMigrateMsg->dpnidNewHost);
				} // end if (second in command case)
				else
				{
					if (pHostMigrateMsg->dpnidNewHost != pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines])
					{
						DPL(0, "Host didn't migrate to expected machine (tester %i, player ID %u/%x), it migrated to player ID %u/%x!  DEBUGBREAK()-ing.",
							5, (pContext->iCurrentHost + 1),
							pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines],
							pContext->padpnidTesters[(pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines],
							pHostMigrateMsg->dpnidNewHost,
							pHostMigrateMsg->dpnidNewHost);

						//Ignore error
						pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
														"Host didn't migrate to expected machine!  DEBUGBREAK()-ing.",
														0);

						DEBUGBREAK();

						hr = E_FAIL;
						goto DONE;
					} // end if (host didn't migrate to expected player)

#pragma TODO(vanceo, "Check for dupes")

					DPL(1, "Host migrated to tester %i (player ID %u/%x) as expected.",
						3, ((pContext->iCurrentHost + 1) % pContext->pTNecd->iNumMachines),
						pHostMigrateMsg->dpnidNewHost,
						pHostMigrateMsg->dpnidNewHost);
				} // end else (not second in command case)


				if (! SetEvent(pContext->hHostMigratedEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set HostMigrated event (%x)!  DEBUGBREAK()-ing.  0x%08x",
						2, pContext->hHostMigratedEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;
				} // end if (couldn't set host migration event)
			} // end else (host migrated event)
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;


			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			/*
			DPL(0, "DPN_MSGID_INDICATE_CONNECT", 0);
			DPL(0, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			DPL(0, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			DPL(0, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			DPL(0, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			DPL(0, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			DPL(0, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			DPL(0, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			DPL(0, "    pAddressPlayer = %a", 1, pIndicateConnectMsg->pAddressPlayer);
			DPL(0, "    pAddressDevice = %a", 1, pIndicateConnectMsg->pAddressDevice);
			*/

#pragma TODO(vanceo, "Validate this")
		  break;

		case DPN_MSGID_RECEIVE:
			PDPNMSG_RECEIVE		pReceiveMsg;


			pReceiveMsg = (PDPNMSG_RECEIVE) pvMsg;

			if (pReceiveMsg->dwReceiveDataSize < (strlen("Reliable send string") + 1))
			{
				DPL(0, "Received data (%x) that was too small (size = %u < %u)!  DEBUGBREAK()-ing",
					3, pReceiveMsg->pReceiveData,
					pReceiveMsg->dwReceiveDataSize,
					(strlen("Reliable send string") + 1));

				DEBUGBREAK();
			} // end if (too small data)
			else if (pReceiveMsg->dwReceiveDataSize > (strlen("Unreliable send string") + 1))
			{
				DPL(0, "Received data (%x) that was too large (size = %u)!  DEBUGBREAK()-ing",
					3, pReceiveMsg->pReceiveData,
					pReceiveMsg->dwReceiveDataSize,
					(strlen("Unreliable send string") + 1));

				DEBUGBREAK();
			} // end if (too large data)
			else
			{
				if (memcmp(pReceiveMsg->pReceiveData, "Unreliable send string", (strlen("Unreliable send string") + 1)) == 0)
				{
					DPL(9, "Received unreliable message from player %u/%x.",
						2, pReceiveMsg->dpnidSender,
						pReceiveMsg->dpnidSender);
				} // end if (unreliable message)
				else if (memcmp(pReceiveMsg->pReceiveData, "Reliable send string", (strlen("Reliable send string") + 1)) == 0)
				{
					DPL(9, "Received reliable message (received approximately %i already) from player %u/%x.",
						3, *((int*) pReceiveMsg->pvPlayerContext),
						pReceiveMsg->dpnidSender,
						pReceiveMsg->dpnidSender);

					// The player context is a pointer to the tester's entry in the
					// num reliable messages received array.  Update it.
					(*((int*) pReceiveMsg->pvPlayerContext))++;
				} // end else if (reliable message)
				else
				{
					DPL(0, "Received invalid data (at %x, size = %u)!  DEBUGBREAK()-ing",
						2, pReceiveMsg->pReceiveData,
						pReceiveMsg->dwReceiveDataSize);

					DEBUGBREAK();
				} // end else (incorrect message)
			} // end else (data is of valid size)
		  break;

		case DPN_MSGID_REMOVE_PLAYER_FROM_GROUP:
			PDPNMSG_REMOVE_PLAYER_FROM_GROUP	pRemovePlayerFromGroupMsg;


			pRemovePlayerFromGroupMsg = (PDPNMSG_REMOVE_PLAYER_FROM_GROUP) pvMsg;

			// Make sure we're supposed to get this message.
			if (pContext->fDroppingPlayer)
			{
				switch (pContext->iGroupOrDataOperation)
				{
					case GROUPORDATAOP_ADDPLAYERTOGROUP:
						DPL(0, "Got REMOVE_PLAYER_FROM_GROUP, assuming it's for an ADD_PLAYER_TO_GROUP that sneaked in or we're destroying the DPlay8 session.", 0);
					  break;

					case GROUPORDATAOP_REMOVEPLAYERFROMGROUP:
						DPL(0, "Dropping player got REMOVE_PLAYER_FROM_GROUP that may or may not have sneaked in.", 0);
					  break;

					default:
						DPL(0, "Got unexpected DPN_MSGID_REMOVE_PLAYER_FROM_GROUP (msg = %x, group/data op = %i)!  DEBUGBREAK()-ing!",
							2, pRemovePlayerFromGroupMsg, pContext->iGroupOrDataOperation);
						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					  break;
				} // end switch (on group/data operation)
			} // end if (dropping player)
			else
			{
				if (pContext->iGroupOrDataOperation != GROUPORDATAOP_REMOVEPLAYERFROMGROUP)
				{
					DPL(0, "Got unexpected DPN_MSGID_REMOVE_PLAYER_FROM_GROUP (msg = %x, group/data op = %i)!  DEBUGBREAK()-ing!",
						2, pRemovePlayerFromGroupMsg, pContext->iGroupOrDataOperation);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (operation unexpected)
			} // end else (not dropping player)


			// Validate the message
			if (pRemovePlayerFromGroupMsg->dpnidGroup != pContext->dpnidGroup)
			{
				DPL(0, "Group ID is wrong (%u/%x != %u/%x)!  DEBUGBREAK()-ing!",
					4, pRemovePlayerFromGroupMsg->dpnidGroup,
					pRemovePlayerFromGroupMsg->dpnidGroup,
					pContext->dpnidGroup, pContext->dpnidGroup);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (group ID is wrong)


			// Check for duplicates and invalid player IDs.
			if (pRemovePlayerFromGroupMsg->dpnidPlayer != pContext->dpnidPlayerAddedToGroup)
			{
				DPL(0, "Getting duplicate or wrong DPN_MSGID_REMOVE_PLAYER_FROM_GROUP (group ID %u/%x != %u/%x)!  DEBUGBREAK()-ing!",
					4 ,pRemovePlayerFromGroupMsg->dpnidPlayer,
					pRemovePlayerFromGroupMsg->dpnidPlayer,
					pContext->dpnidPlayerAddedToGroup,
					pContext->dpnidPlayerAddedToGroup);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (player ID is unexpected)

			pContext->dpnidPlayerAddedToGroup = 0;


			// Can't detect group ops that sneak in because it may just be the
			// nametable unwinding.
			/*
			if (pContext->fDroppingPlayer)
			{
				DPL(0, "REMOVE_PLAYER_FROM_GROUP sneaked in while we were leaving session.", 0);
			} // end if (dropping player)
			*/


			// Notify the main thread, if necessary.
			if (pContext->fGroupOrDataOperatingPlayer)
			{
				if (pContext->fNoAsyncOpCompletion)
				{
					DPL(0, "No async op completion expected on group/data operating player (tester %i), notifying main thread.",
						1, pContext->pTNecd->iTesterNum);

					if (! SetEvent(pContext->hGroupOrDataOperationEvent))
					{
						hr = GetLastError();

						DPL(0, "Couldn't set group or data operation event (%x)!  DEBUGBREAK()-ing.  0x%08x",
							2, pContext->hGroupOrDataOperationEvent, hr);

						DEBUGBREAK();

						if (hr == S_OK)
							hr = E_FAIL;
					} // end if (couldn't set event)
				} // end if (no async op completion)
				else
				{
					DPL(1, "Not setting event after REMOVE_PLAYER_FROM_GROUP, async op completion should trigger for group/data operating player (tester %i).",
						1, pContext->pTNecd->iTesterNum);
				} // end if (async op completion still expected)
			} // end if (group/data operating player)
			else if (! pContext->fDroppingPlayer)
			{
				DPL(1, "Got expected REMOVE_PLAYER_FROM_GROUP on tester %i, notifying main thread.",
					1, pContext->pTNecd->iTesterNum);

				if (! SetEvent(pContext->hGroupOrDataOperationEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set group or data operation event (%x)!  DEBUGBREAK()-ing.  0x%08x",
						2, pContext->hGroupOrDataOperationEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;
				} // end if (couldn't set event)
			} // end else if (not group/data operating or dropping player)
		  break;

		case DPN_MSGID_SEND_COMPLETE:
			DPL(9, "DPN_MSGID_SEND_COMPLETE", 0);
		  break;

		case DPN_MSGID_PEER_INFO:
			PDPNMSG_PEER_INFO	pPeerInfoMsg;


			pPeerInfoMsg = (PDPNMSG_PEER_INFO) pvMsg;

			// Make sure we're supposed to get this message.
			if (pContext->fDroppingPlayer)
			{
				if (pContext->iGroupOrDataOperation == GROUPORDATAOP_SETPEERINFO)
				{
					DPL(0, "PEER_INFO sneaked in while we were leaving session.", 0);
				} // end if (setting peer info)
				else
				{
					DPL(0, "Got unexpected DPN_MSGID_PEER_INFO (msg = %x, group/data op = %i)!  DEBUGBREAK()-ing!",
						2, pPeerInfoMsg, pContext->iGroupOrDataOperation);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end else (some other group/data operation)
			} // end if (dropping player)
			else
			{
				if (pContext->iGroupOrDataOperation != GROUPORDATAOP_SETPEERINFO)
				{
					DPL(0, "Got unexpected DPN_MSGID_PEER_INFO (msg = %x, group/data op = %i)!  DEBUGBREAK()-ing!",
						2, pPeerInfoMsg, pContext->iGroupOrDataOperation);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (operation unexpected)
			} // end else (not dropping player)


#pragma TODO(vanceo, "Validate the DPN_MSGID_PEER_INFO message")


			// Check for duplicates.
			if (pContext->fPeerInfoSet)
			{
				DPL(0, "Getting duplicate DPN_MSGID_PEER_INFO (%x)!  DEBUGBREAK()-ing!",
					1, pPeerInfoMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (already got info)

			pContext->fPeerInfoSet = TRUE;



			// Notify the main thread, if necessary.
			if (pContext->fGroupOrDataOperatingPlayer)
			{
				DPL(1, "Not setting event after PEER_INFO, async op completion should trigger for group/data operating player (tester %i).",
					1, pContext->pTNecd->iTesterNum);
			} // end if (group/data operating player)
			else if (! pContext->fDroppingPlayer)
			{
				DPL(1, "Got expected PEER_INFO on tester %i, notifying main thread.",
					1, pContext->pTNecd->iTesterNum);

				if (! SetEvent(pContext->hGroupOrDataOperationEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set group or data operation event (%x)!  DEBUGBREAK()-ing.  0x%08x",
						2, pContext->hGroupOrDataOperationEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;
				} // end if (couldn't set event)
			} // end else if (not group/data operating or dropping player)
		  break;

		default:
			DPL(0, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	SAFE_LOCALFREE(pdpnpi);

	return (hr);
} // HostMigrateDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
