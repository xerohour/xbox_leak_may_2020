//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\..\..\tncommon\debugprint.h"
#include "..\..\..\tncommon\cppobjhelp.h"
#include "..\..\..\tncommon\linklist.h"
#include "..\..\..\tncommon\linkedstr.h"
#include "..\..\..\tncommon\strutils.h"
#include "..\..\..\tncommon\sprintf.h"

#include "..\..\..\tncontrl\tncontrl.h"


#include "..\main.h"

#include "..\bldssn\blddp8.h"

#include "connex.h"



//==================================================================================
// Defines
//==================================================================================
#define DATAPATTERN_CONNECT		0x0711F00D
#define DATAPATTERN_REPLY		0xB001B00F




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



typedef struct tagCONNEXLEAVEWHILEJOINCONTEXT:public BLDDP8DPNHANDLERCONTEXTHEADER
{
	// See blddp8.h BLDDP8DPNHANDLERCONTEXTHEADER for first members of structure

	PTNEXECCASEDATA		pTNecd; // pointer to EXECCASEDATA passed in to test
	DPNID*				padpnidTesters; // array of player IDs, in tester number order
	BOOL				fEnsureDisconnectBeforeJoin; // ensure the dropping player can't see joining player
	BOOL				fSelfIndicate; // set to TRUE if expecting our own player to connect/disconnect, FALSE if not
	int					iCurrentDroppingPlayer; // tester number of player who is dropping this iteration
	int					iCurrentJoiningPlayer; // tester number of player who is joining this iteration
	BOOL*				pafTesterIndicated; // pointer to array to use to remember whether an indication has already arrived for the tester
	PWRAPDP8PEER		pDP8Peer; // DPlay8 peer object in use
	HANDLE				hDroppedAndJoinedEvent; // event to set when players expected to connect/disconnect actually have
	HANDLE				hDestroyPlayerForRacingLeave; // event to set when dropping player who sneaked into nametable drops as expected
} CONNEXLEAVEWHILEJOINCONTEXT, * PCONNEXLEAVEWHILEJOINCONTEXT;

typedef struct tagCONNEXMANYHOSTCONTEXT
{
	PTNEXECCASEDATA		pTNecd; // pointer to EXECCASEDATA passed in to test
	DWORD				dwUserDataSize; // size of user connect data and reply data to use
	PVOID				pvReplyData; // pointer to reply data buffer to use
	BOOL				fReject; // whether to reject the attempts or not
	int					iNumConnectsRemaining; // how many connect attempts remaing
	HANDLE				hConnectsDoneEvent; // event to set when all connects are indicated (and their reply buffers are returned)
} CONNEXMANYHOSTCONTEXT, * PCONNEXMANYHOSTCONTEXT;

typedef struct tagCONNEXMANYCONNECTORCONTEXT
{
	PTNEXECCASEDATA		pTNecd; // pointer to EXECCASEDATA passed in to test
	HANDLE*				pahConnectCompleteEvents; // connect completion events
	DPNHANDLE*			padpnhConnects; // DPlay8 handles to connect calls
	DWORD				dwReplyDataSize; // size of expected reply data
	BOOL				fReject; // whether to reject the attempts or not
} CONNEXMANYCONNECTORCONTEXT, * PCONNEXMANYCONNECTORCONTEXT;

typedef struct tagCONNEXSERVERLOSSCONTEXT:public BLDDP8DPNHANDLERCONTEXTHEADER
{
	// See blddp8.h BLDDP8DPNHANDLERCONTEXTHEADER for first members of structure

	PTNEXECCASEDATA		pTNecd; // pointer to EXECCASEDATA passed in to test
	BOOL				fSelfIndicate; // whether the server expects to see the local player drop or not
	DPNID*				padpnidTesters; // array of player IDs, in tester number order
	BOOL*				pafTesterIndicated; // pointer to array to use to remember whether a DESTROY_PLAYER has already arrived for the tester
	HANDLE				hDisconnectEvent; // event to set when server disconnection is complete
} CONNEXSERVERLOSSCONTEXT, * PCONNEXSERVERLOSSCONTEXT;


typedef struct tagCONNEXREJECTCONTEXT
{
	PTNEXECCASEDATA		pTNecd; // pointer to EXECCASEDATA passed in to test
	PVOID				pvConnectData; // data to send with connection/reply
	DWORD				dwConnectDataSize; // size of data to send with connection/reply
	HANDLE				hConnectEvent; // event for player to set when connection is indicated/completes
	HANDLE				hReplyBufferReturnedEvent; // event for host player to set when reply buffer (if any) is returned
} CONNEXREJECTCONTEXT, * PCONNEXREJECTCONTEXT;


typedef struct tagCONNEXNOTALLOWEDCONTEXT
{
	PTNEXECCASEDATA		pTNecd; // pointer to EXECCASEDATA passed in to test
	HRESULT				hrExpectedResult; // expected result code for connect attempt
	HANDLE				hConnectCompletedEvent; // event for player to set when connection attempt completes
	BOOL				fConnectsAllowed; // whether connects are allowed or not
} CONNEXNOTALLOWEDCONTEXT, * PCONNEXNOTALLOWEDCONTEXT;





//==================================================================================
// Prototypes
//==================================================================================
HRESULT ConnexCanRun_NonServerLoss(PTNCANRUNDATA pTNcrd);

HRESULT ConnexGet_LeaveWhileJoinAll(PTNGETINPUTDATA pTNgid);
HRESULT ConnexWrite_LeaveWhileJoinAll(PTNWRITEDATA pTNwd);
HRESULT ConnexExec_LeaveWhileJoinPeer(PTNEXECCASEDATA pTNecd);
HRESULT ConnexExec_LeaveWhileJoinCS(PTNEXECCASEDATA pTNecd);

HRESULT ConnexGet_ManyAll(PTNGETINPUTDATA pTNgid);
HRESULT ConnexWrite_ManyAll(PTNWRITEDATA pTNwd);
HRESULT ConnexExec_ManyAll(PTNEXECCASEDATA pTNecd);

HRESULT ConnexCanRun_ServerLoss(PTNCANRUNDATA pTNcrd);
HRESULT ConnexExec_ServerLoss(PTNEXECCASEDATA pTNecd);

HRESULT ConnexExec_Reject(PTNEXECCASEDATA pTNecd);

HRESULT ConnexExec_NotAllowed(PTNEXECCASEDATA pTNecd);



HRESULT ConnexLeaveWhileJoinPeerDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT ConnexLeaveWhileJoinServerDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT ConnexLeaveWhileJoinClientDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);

HRESULT ConnexManyHostDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT ConnexManyConnectorDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);

HRESULT ConnexServerLossServerDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT ConnexServerLossClientDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);

HRESULT ConnexRejectDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);

HRESULT ConnexNotAllowedDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexLoadTestTable()"
//==================================================================================
// ConnexLoadTestTable
//----------------------------------------------------------------------------------
//
// Description: Loads all the possible tests into the table passed in:
//				2.7			Connection/disconnection tests
//				2.7.1		Leave while joining tests
//				2.7.1.1		Simple normal leave while joining peer test
//				2.7.1.2		Simple normal leave while joining peer test
//				2.7.1.3		Round robin normal leave while joining client/server test
//				2.7.1.4		Round robin normal leave while joining client/server test
//				2.7.2		Drop while joining tests
//				2.7.2.1		Simple drop while joining peer test
//				2.7.2.2		Simple drop while joining peer test
//				2.7.2.3		Round robin drop while joining client/server test
//				2.7.2.4		Round robin drop while joining client/server test
//				2.7.3		Many connect attempts tests
//				2.7.3.1		Many peer connect attempts test
//				2.7.3.2		Many client/server connect attempts test
//				2.7.4		Server loss tests
//				2.7.4.1		Server leave test
//				2.7.4.2		Server drop test
//				2.7.5		Simple reject connection tests
//				2.7.5.1		Simple peer reject connections test
//				2.7.5.2		Simple client/server reject connections test
//				2.7.6		Not allowed connection tests
//				2.7.6.1		All not-allowed connection types test
//
// Arguments:
//	PTNLOADTESTTABLEDATA pTNlttd	Pointer to data to use when loading the tests.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ConnexLoadTestTable(PTNLOADTESTTABLEDATA pTNlttd)
{
	PTNTESTTABLEGROUP	pSubGroup;
	PTNTESTTABLEGROUP	pSubSubGroup;
	TNADDTESTDATA		tnatd;



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7		Connection/disconnection tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pTNlttd->pBase, "2.7", "Connection/disconnection tests", &pSubGroup);




	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.1	Leave while joining tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pSubGroup, "2.7.1", "Leave while joining tests", &pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.1.1	Simple normal leave while joining peer test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.1.1";
	tnatd.pszName			= "Simple normal leave while joining peer test";
	tnatd.pszDescription	= "Simple normal leave while joining peer test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 4;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ConnexExec_LeaveWhileJoinPeer;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.1.2	Simple normal leave while joining client/server test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.1.2";
	tnatd.pszName			= "Simple normal leave while joining client/server test";
	tnatd.pszDescription	= "Simple normal leave while joining client/server test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 4;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ConnexExec_LeaveWhileJoinCS;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);





	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.1.3	Round robin normal leave while joining peer test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.1.3";
	tnatd.pszName			= "Round robin normal leave while joining peer test";
	tnatd.pszDescription	= "Round robin normal leave while joining peer test";
	tnatd.pszInputDataHelp	= "NumIterations = How many times to join and leave";

	tnatd.iNumMachines		= -4;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= ConnexGet_LeaveWhileJoinAll;
	tnatd.pfnExecCase		= ConnexExec_LeaveWhileJoinPeer;
	tnatd.pfnWriteData		= ConnexWrite_LeaveWhileJoinAll;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.1.4	Round robin normal leave while joining client/server test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.1.4";
	tnatd.pszName			= "Round robin normal leave while joining client/server test";
	tnatd.pszDescription	= "Round robin normal leave while joining client/server test";
	tnatd.pszInputDataHelp	= "NumIterations = How many times to join and leave";

	tnatd.iNumMachines		= -4;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= ConnexGet_LeaveWhileJoinAll;
	tnatd.pfnExecCase		= ConnexExec_LeaveWhileJoinCS;
	tnatd.pfnWriteData		= ConnexWrite_LeaveWhileJoinAll;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);





	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.2	Drop while joining tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pSubGroup, "2.7.2", "Drop while joining tests", &pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.2.1	Simple drop while joining peer test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.2.1";
	tnatd.pszName			= "Simple drop while joining peer test";
	tnatd.pszDescription	= "Simple drop while joining peer test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 4;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ConnexExec_LeaveWhileJoinPeer;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.2.2	Simple drop while joining client/server test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.2.2";
	tnatd.pszName			= "Simple drop while joining client/server test";
	tnatd.pszDescription	= "Simple drop while joining client/server test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 4;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ConnexExec_LeaveWhileJoinCS;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.2.3	Round robin drop while joining peer test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.2.3";
	tnatd.pszName			= "Round robin drop while joining peer test";
	tnatd.pszDescription	= "Round robin drop while joining peer test";
	tnatd.pszInputDataHelp	= "NumIterations = How many times to join and drop";

	tnatd.iNumMachines		= -4;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= ConnexGet_LeaveWhileJoinAll;
	tnatd.pfnExecCase		= ConnexExec_LeaveWhileJoinPeer;
	tnatd.pfnWriteData		= ConnexWrite_LeaveWhileJoinAll;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.2.4	Round robin drop while joining client/server test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.2.4";
	tnatd.pszName			= "Round robin drop while joining client/server test";
	tnatd.pszDescription	= "Round robin drop while joining client/server test";
	tnatd.pszInputDataHelp	= "NumIterations = How many times to join and drop";

	tnatd.iNumMachines		= -4;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= ConnexGet_LeaveWhileJoinAll;
	tnatd.pfnExecCase		= ConnexExec_LeaveWhileJoinCS;
	tnatd.pfnWriteData		= ConnexWrite_LeaveWhileJoinAll;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);





	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.3	Many connect attempts tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pSubGroup, "2.7.3", "Many connect attempts tests", &pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.3.1	Many peer connect attempts test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.3.1";
	tnatd.pszName			= "Many peer connect attempts test";
	tnatd.pszDescription	= "Testers try multiple times (optionally using multiple objects) to connect to the host.";
	tnatd.pszInputDataHelp	= "NumIterations = How many times slaves try to connect\n"
								"NumObjects = Number of individual interfaces each client uses (connects may overlap if > 1)\n"
								"Overlap = TRUE if connect attempts can overlap, FALSE if attempts are sequential\n"
								"UserDataSize = Size of user connect data and reply data\n"
								"Reject = TRUE to have reject connections, FALSE for to accept them";

	tnatd.iNumMachines		= -2;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= ConnexGet_ManyAll;
	tnatd.pfnExecCase		= ConnexExec_ManyAll;
	tnatd.pfnWriteData		= ConnexWrite_ManyAll;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.3.2	Many client/server connect attempts test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.3.2";
	tnatd.pszName			= "Many client/server connect attempts test";
	tnatd.pszDescription	= "Testers try multiple times (optionally using multiple objects) to connect to the host.";
	tnatd.pszInputDataHelp	= "NumIterations = How many times slaves try to connect\n"
								"NumObjects = Number of individual interfaces each client uses (connects may overlap if > 1)\n"
								"Overlap = TRUE if connect attempts can overlap, FALSE if attempts are sequential\n"
								"UserDataSize = Size of user connect data and reply data\n"
								"Reject = TRUE to have reject connections, FALSE for to accept them";

	tnatd.iNumMachines		= -2;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= ConnexGet_ManyAll;
	tnatd.pfnExecCase		= ConnexExec_ManyAll;
	tnatd.pfnWriteData		= ConnexWrite_ManyAll;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);





	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.4	Server loss tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pSubGroup, "2.7.4", "Server loss tests", &pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.4.1	Server leave test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.4.1";
	tnatd.pszName			= "Server leave test";
	tnatd.pszDescription	= "Server leave test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= -2;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_ServerLoss;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ConnexExec_ServerLoss;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.4.2	Server drop test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.4.2";
	tnatd.pszName			= "Server drop test";
	tnatd.pszDescription	= "Server drop test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= -2;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_ServerLoss;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ConnexExec_ServerLoss;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);





	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.5	Simple reject connection tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pSubGroup, "2.7.5", "Simple reject connection tests", &pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.5.1	Simple peer reject connections test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.5.1";
	tnatd.pszName			= "Simple peer reject connections test";
	tnatd.pszDescription	= "Simple peer reject connections test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 2;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ConnexExec_Reject;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.5.2	Simple client/server reject connections test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.5.2";
	tnatd.pszName			= "Simple client/server reject connections test";
	tnatd.pszDescription	= "Simple client/server reject connections test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 2;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ConnexExec_Reject;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);





	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.6	Not allowed connection tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pSubGroup, "2.7.6", "Not allowed connection tests", &pSubSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.7.6.1	All not-allowed connection types test
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "2.7.6.1";
	tnatd.pszName			= "All not-allowed connection types test";
	tnatd.pszDescription	= "All not-allowed connection types test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 2;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_STRESS | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST
								| TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS;

	tnatd.pfnCanRun			= ConnexCanRun_NonServerLoss;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= ConnexExec_NotAllowed;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubSubGroup);



	return (S_OK);
} // ConnexLoadTestTable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexCanRun_NonServerLoss()"
//==================================================================================
// ConnexCanRun_NonServerLoss
//----------------------------------------------------------------------------------
//
// Description: Callback that checks a possible tester list to make sure the test
//				can be run correctly for the following test case(s):
//				2.7.1.1 - Simple normal leave while joining peer test
//				2.7.1.2 - Simple normal leave while joining client/server test
//				2.7.1.3 - Round robin normal leave while joining peer test
//				2.7.1.4 - Round robin normal leave while joining client/server test
//				2.7.2.1 - Simple drop while joining peer test
//				2.7.2.2 - Simple drop while joining client/server test
//				2.7.2.3 - Round robin drop while joining peer test
//				2.7.2.4 - Round robin drop while joining client/server test
//				2.7.3.1 - Many peer connect attempts test
//				2.7.5.1 - Simple peer reject connections test
//				2.7.5.2 - Simple client/server reject connections test
//				2.7.6.1 - All not-allowed connection types test
//
// Arguments:
//	PTNCANRUNDATA pTNcrd	Pointer to parameter block with information on what
//							configuration to check.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ConnexCanRun_NonServerLoss(PTNCANRUNDATA pTNcrd)
{
	HRESULT		hr;
	int			i;


	// Make sure all subsequent testers can connect to the first.
	for(i = 1; i < pTNcrd->iNumMachines; i++)
	{
		// Only the second tester needs to have IMTest installed in the simple (i.e
		// only 1 time) drop test cases.  All non-host testers need it in the round
		// robin drop cases.
		if (((strcmp(pTNcrd->pTest->m_pszID, "2.7.2.1") == 0) && (i == 1)) ||
			((strcmp(pTNcrd->pTest->m_pszID, "2.7.2.2") == 0) && (i == 1)) ||
			(strcmp(pTNcrd->pTest->m_pszID, "2.7.2.3") == 0) ||
			(strcmp(pTNcrd->pTest->m_pszID, "2.7.2.4") == 0))
		{
			pTNcrd->fCanRun = pTNcrd->apTesters[i]->m_fFaultSimIMTestAvailable;

			if (! pTNcrd->fCanRun)
			{
				DPL(1, "Tester %i (%s) doesn't have IMTest fault simulator available.",
					2, i, pTNcrd->apTesters[i]->m_szComputerName);

				return (S_OK);
			} // end if (tester doesn't have IMTest)
		} // end if (drop test)


		if (pTNcrd->pMaster->AreOnSameMachine(pTNcrd->apTesters[i],
											pTNcrd->apTesters[0]))
		{
			DPL(1, "Tester 1 is on same machine as tester 0 (%s), assuming can reach via UDP.",
				1, pTNcrd->apTesters[0]->m_szComputerName);

			pTNcrd->fCanRun = TRUE;
		} // end if (are on same machine)
		else
		{
			hr = pTNcrd->pMaster->CanReachViaIP(pTNcrd->apTesters[i],
												pTNcrd->apTesters[0],
												DPLAY8_PORT,
												TNCR_IP_UDP,
												&(pTNcrd->fCanRun));
			if (hr != S_OK)
			{
				DPL(0, "Couldn't have tester %i (%s) try to reach tester 0 (%s) via UDP!",
					3, i, pTNcrd->apTesters[i]->m_szComputerName,
					pTNcrd->apTesters[0]->m_szComputerName);

				return (hr);
			} // end if (couldn't test reachability)

			if (! pTNcrd->fCanRun)
			{
				DPL(1, "Tester %i (%s) can't reach tester 0 (%s) via UDP.",
					3, i, pTNcrd->apTesters[i]->m_szComputerName,
					pTNcrd->apTesters[0]->m_szComputerName);

				return (S_OK);
			} // end if (tester couldn't reach tester 0)
		} // end else (not on same machine)
	} // end for (each tester after 0)

	return (S_OK);
} // ConnexCanRun_NonServerLoss
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexGet_LeaveWhileJoinAll()"
//==================================================================================
// ConnexGet_LeaveWhileJoinAll
//----------------------------------------------------------------------------------
//
// Description: Callback that retrieves the initial data for the test case(s):
//				2.7.1.1 - Simple normal leave while joining peer test
//				2.7.1.2 - Simple normal leave while joining client/server test
//				2.7.1.3 - Round robin normal leave while joining peer test
//				2.7.1.4 - Round robin normal leave while joining client/server test
//				2.7.2.1 - Simple drop while joining peer test
//				2.7.2.2 - Simple drop while joining client/server test
//				2.7.2.3 - Round robin drop while joining peer test
//				2.7.2.4 - Round robin drop while joining client/server test
//
// Arguments:
//	PTNGETINPUTDATA pTNgid	Pointer to parameter block with information on how and
//							where to place the input data.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ConnexGet_LeaveWhileJoinAll(PTNGETINPUTDATA pTNgid)
{
	PTID_PCONNEX_LEAVEWHILEJOIN		pParams = (PTID_PCONNEX_LEAVEWHILEJOIN) pTNgid->pvData;
	PLSTRINGLIST					pStrings = NULL;
	char*							pszTemp;



	pTNgid->dwDataSize = sizeof (TID_PCONNEX_LEAVEWHILEJOIN);

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
} // ConnexGet_LeaveWhileJoinAll
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexWrite_LeaveWhileJoinAll()"
//==================================================================================
// ConnexWrite_LeaveWhileJoinAll
//----------------------------------------------------------------------------------
//
// Description: Callback that converts a buffer to strings for the test case(s):
//				2.7.1.1 - Simple normal leave while joining peer test
//				2.7.1.2 - Simple normal leave while joining client/server test
//				2.7.1.3 - Round robin normal leave while joining peer test
//				2.7.1.4 - Round robin normal leave while joining client/server test
//				2.7.2.1 - Simple drop while joining peer test
//				2.7.2.2 - Simple drop while joining client/server test
//				2.7.2.3 - Round robin drop while joining peer test
//				2.7.2.4 - Round robin drop while joining client/server test
//
// Arguments:
//	PTNWRITEDATA pTNwd	Parameter block with information and data to write.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ConnexWrite_LeaveWhileJoinAll(PTNWRITEDATA pTNwd)
{
	PTID_PCONNEX_LEAVEWHILEJOIN		pInputData = (PTID_PCONNEX_LEAVEWHILEJOIN) pTNwd->pvData;

	
	switch (pTNwd->dwLocation)
	{
		case TNTSDL_REPORTASSIGN:
			REQUIRE_ASSIGNREPORT_DATA(sizeof (TID_PCONNEX_LEAVEWHILEJOIN));


			WRITESPRINTFDATALINE("NumIterations= %i", 1, pInputData->iNumIterations);
		  break;

		default:
			DPL(0, "Got data for unexpected location %u!",
				1, pTNwd->dwLocation);
			return (ERROR_INVALID_PARAMETER);
		  break;
	} // end switch (on the source of the data buffer)

	
	return (S_OK);
} // ConnexWrite_LeaveWhileJoinAll
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexExec_LeaveWhileJoinPeer()"
//==================================================================================
// ConnexExec_LeaveWhileJoinPeer
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.7.1.1 - Simple normal leave while joining peer test
//				2.7.1.3 - Round robin normal leave while joining peer test
//				2.7.2.1 - Simple drop while joining peer test
//				2.7.2.3 - Round robin drop while joining peer test
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
HRESULT ConnexExec_LeaveWhileJoinPeer(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	HRESULT							temphr;
	PTID_PCONNEX_LEAVEWHILEJOIN		pInput = (PTID_PCONNEX_LEAVEWHILEJOIN) pTNecd->pvInputData;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	DWORD							dwSubInputDataSize;
	PWRAPDP8PEER					pDP8Peer = NULL;
	PTOD_BBLDDP8_PEER_CREATE		pBldDP8PeerCreateOutput = NULL;
	DWORD							dwSubOutputDataSize;
	CONNEXLEAVEWHILEJOINCONTEXT		context;
	DPN_APPLICATION_DESC			dpnad;
	PDIRECTPLAY8ADDRESS				pDP8AddressRemote = NULL;
	PDIRECTPLAY8ADDRESS				pDP8AddressLocal = NULL;
	PTNFAULTSIM						pFaultSim = NULL;
	HANDLE							hConnectCompleteEvent = NULL;
	DPNHANDLE						dpnhConnect;
	CTNSyncDataList					syncdata;
	PVOID							pvSyncData;
	int*							paiDestroyTesters = NULL;
	DPNID*							padpnidDestroyTesters = NULL;
	int								iNumIterations;
	int								iCurrentIteration;
	int								i;
	int								j;
#ifdef DEBUG
	char*							pszURL = NULL;
	DWORD							dwURLBufferSize = 0;
	DWORD							dwURLSize;
#endif // DEBUG



	ZeroMemory(&context, sizeof (CONNEXLEAVEWHILEJOINCONTEXT));
	context.pTNecd = pTNecd;


	BEGIN_TESTCASE
	{
		if ((pTNecd->pExecutor->IsCase("2.7.1.1")) || (pTNecd->pExecutor->IsCase("2.7.2.1")))
		{
			iNumIterations = 1;
		} // end if (simple case)
		else
		{
			REQUIRE_INPUT_DATA(sizeof (TID_PCONNEX_LEAVEWHILEJOIN));

			iNumIterations = pInput->iNumIterations;
		} // end else (round robin case)

		DPL(1, "Will perform %i iterations.", 1, iNumIterations);



		// Allocate an array for all testers' player IDs.  The position of a tester
		// is always the same, regardless of the iteration.
		// We allocate this array even though build session will give us one
		// because we need to modify it, and we can't modify someone else's buffer.

		LOCALALLOC_OR_THROW(DPNID*, context.padpnidTesters,
							(pTNecd->iNumMachines * sizeof (DPNID)));

		// Allocate an array for add player/delete player indications.  The position
		// of a tester is always the same, regardless of the iteration.

		LOCALALLOC_OR_THROW(BOOL*, context.pafTesterIndicated,
							(pTNecd->iNumMachines * sizeof (BOOL)));



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating peer-to-peer session");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		context.pfnDPNMessageHandler = ConnexLeaveWhileJoinPeerDPNMessageHandler;
		context.pTNecd = pTNecd;

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_MIGRATE_HOST | DPNSESSION_NODPNSVR;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_CONNEX_LEAVEWHILEJOIN_PEER;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		dpnad.pwszSessionName = L"Session";
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.pvApplicationReservedData = 0;



		dwSubInputDataSize = sizeof (TID_BBLDDP8_ALL_CREATE);
							//+ (pTNecd->iNumMachines * sizeof (int));

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, dwSubInputDataSize);
#pragma TODO(vanceo, "Allow SP to be selected")
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->guidSP = CLSID_DP8SP_TCPIP;
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->pHandlerContext = &context;
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->pdpnad = &dpnad;

		/*
		for(i = 0; i < pTNecd->iNumMachines; i++)
		{
			*((PVOID*) (((PBYTE) pvSubInputData) + sizeof (TID_BBLDDP8_ALL_CREATE) + (i * sizeof (int*)))) = &(context.paiReliableMsgsReceivedFromTesters[i]);
		} // end for (each tester)
		*/


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

		CopyMemory(context.padpnidTesters, (pBldDP8PeerCreateOutput + 1),
					(pTNecd->iNumMachines * sizeof (DPNID)));


		// Everybody who might be rejoining the session at some point needs to
		// retrieve the address of the host so they can rejoin easily.
		TESTSECTION_IF(pTNecd->iTesterNum != 0)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Getting host's session address");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Peer->DP8P_GetPeerAddress(context.padpnidTesters[0],
											&pDP8AddressRemote,
											0);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't get host's address!", 0);
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

			DPL(1, "Will use address (\"%s\") to reconnect.", 1, pszURL);
#endif // DEBUG
		} // end if (not host)
		TESTSECTION_ENDIF



		// If we're the tester who's supposed to join first, we need to drop out of
		// the session we built (we only joined the first time so we could get the
		// host's address.

		context.hDroppedAndJoinedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (context.hDroppedAndJoinedEvent == NULL)
		{
			sr = GetLastError();
			DPL(0, "Couldn't create dropped and joined event!", 0);
			THROW_SYSTEMRESULT;
		} // end if (couldn't create event)


		context.iCurrentDroppingPlayer = pTNecd->iNumMachines - 1;
		context.iCurrentJoiningPlayer = -1;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for all testers to be ready for tester to temporarily leave");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->SyncWithTesters("Ready for temp leave", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;




		TESTSECTION_IF(pTNecd->iTesterNum == (pTNecd->iNumMachines - 1))
		{
			// We expect our own player to be removed.
			context.fSelfIndicate = TRUE;


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Leaving session temporarily");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Peer->DP8P_Close(0);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't close interface!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't close)



			// Make sure everyone got removed from nametable.
			for(i = 0; i < pTNecd->iNumMachines; i++)
			{
				if (! context.pafTesterIndicated[i])
				{
					DPL(0, "Nametable didn't unwind during Close(), tester %i (player ID %u/%x) hasn't been removed!",
						3, i, context.padpnidTesters[i], context.padpnidTesters[i]);
					SETTHROW_TESTRESULT(ERROR_IO_INCOMPLETE);
				} // end if (tester hasn't been removed)
			} // end for (each tester)


			context.fSelfIndicate = FALSE;



#pragma TODO(vanceo, "Make release optional")

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Peer object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Peer->Release();
			if (tr != S_OK)
			{
				DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't release)
		} // end if (first joiner)
		TESTSECTION_ELSE
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for player to temporarily drop");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hDroppedAndJoinedEvent),
														1,
														NULL,
														-1,
														INFINITE,
														NULL);
			HANDLE_WAIT_RESULT;
		} // end else (not first joiner)
		TESTSECTION_ENDIF


		// Forget the dropping player's ID
		context.padpnidTesters[pTNecd->iNumMachines - 1] = 0;

		CloseHandle(context.hDroppedAndJoinedEvent);
		context.hDroppedAndJoinedEvent = NULL;



		
		// Start at the very beginning...
		iCurrentIteration = 0;
		TESTSECTION_DO
		{
			// Remember who's dropping and who's joining.
			context.iCurrentDroppingPlayer = (iCurrentIteration % (pTNecd->iNumMachines - 1)) + 1;
			context.iCurrentJoiningPlayer = context.iCurrentDroppingPlayer + (pTNecd->iNumMachines - 2);
			if (context.iCurrentJoiningPlayer >= pTNecd->iNumMachines)
				context.iCurrentJoiningPlayer = (context.iCurrentJoiningPlayer - pTNecd->iNumMachines) + 1;
			
			// Note that tester 0 should never be a dropping or joining player.


			DPL(1, "Iteration %i: dropping player = %i, joining player = %i, (we're tester %i so normal = %B)",
				5, iCurrentIteration, context.iCurrentDroppingPlayer,
				context.iCurrentJoiningPlayer, pTNecd->iTesterNum,
				(((pTNecd->iTesterNum == context.iCurrentDroppingPlayer) || (pTNecd->iTesterNum == context.iCurrentJoiningPlayer)) ? FALSE : TRUE));

			// Ignore error
			pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
									"Starting iteration #%i",
									1, (iCurrentIteration + 1));


			TESTSECTION_IF(pTNecd->iTesterNum == context.iCurrentDroppingPlayer)
			{
				// Get ready for DESTROY_PLAYER indications for everyone.
				ZeroMemory(context.pafTesterIndicated,
							(pTNecd->iNumMachines * sizeof (BOOL)));

				// NOTE: If we're not ensuring that the joining player can never
				// send any traffic to us, then we race with him to leave the
				// session before he joins it.  It's therefore possible to get the
				// CREATE_PLAYER for him.  If we do, however, we need to get a
				// DESTROY_PLAYER for him when he finally does leave the session.
				// Mark him as indicated now, so that if the CREATE_PLAYER doesn't
				// arrive, we're not waiting forever for the DESTROY_PLAYER that
				// will never show up.  But if the CREATE_PLAYER does come in, then
				// we do need to wait for the DESTROY_PLAYER.  That means, we force
				// CREATE_PLAYER to have the opposite behavior of marking this guy
				// as _not_ indicated when the indication comes in.  That way the
				// matching DESTROY_PLAYER resets it to the way we want.  Certainly
				// not foolproof, but it should catch any realistically possible bug
				// in DPlay.
				context.pafTesterIndicated[context.iCurrentJoiningPlayer] = TRUE;


				context.hDroppedAndJoinedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				if (context.hDroppedAndJoinedEvent == NULL)
				{
					sr = GetLastError();
					DPL(0, "Couldn't create dropped and joined event!", 0);
					THROW_SYSTEMRESULT;
				} // end if (couldn't create event)



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Synchronizing with rejoining player and those still in session");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Ready for player leave & join",
														NULL, 0,
														NULL,
														0);
				HANDLE_SYNC_RESULT;



				TESTSECTION_IF((pTNecd->pExecutor->IsCase("2.7.2.1")) || (pTNecd->pExecutor->IsCase("2.7.2.3")))
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
					TEST_SECTION("Disconnecting sends and received, will drop from session");
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

					sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hDroppedAndJoinedEvent,
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



				// Clear this, to be paranoid.
				context.pDP8Peer = NULL;



				TESTSECTION_IF((pTNecd->pExecutor->IsCase("2.7.2.1")) || (pTNecd->pExecutor->IsCase("2.7.2.3")))
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Ensuring local host player has been removed");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				} // end if (drop case)
				TESTSECTION_ELSE
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Ensuring all players have been removed");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				} // end else (normal leave case)
				TESTSECTION_ENDIF


				for(i = 0; i < pTNecd->iNumMachines; i++)
				{
					if (! context.pafTesterIndicated[i])
					{
						DPL(0, "Nametable didn't unwind during Close(), tester %i (player ID %u/%x) hasn't been removed (current joining tester = %i)!",
							4, i, context.padpnidTesters[i], context.padpnidTesters[i],
							context.iCurrentJoiningPlayer);
						SETTHROW_TESTRESULT(ERROR_IO_INCOMPLETE);
					} // end if (tester hasn't been removed)
				} // end for (each tester)



#pragma TODO(vanceo, "Make release optional")

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Releasing DirectPlay8Peer object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Peer->Release();
				if (tr != S_OK)
				{
					DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't release)




				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Retrieving player ID for rejoiner");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Ensure correct rejoiner player ID",
														NULL, 0,
														&syncdata,
														0);
				HANDLE_SYNC_RESULT;

				CHECKANDGET_SYNCDATA(syncdata, context.iCurrentJoiningPlayer,
									pvSyncData, dwSubOutputDataSize, sizeof (DPNID));


				// Store the player ID.
				context.padpnidTesters[context.iCurrentJoiningPlayer] = *((DPNID*) pvSyncData);
			} // end if (we're the dropping player)
			TESTSECTION_ELSEIF(pTNecd->iTesterNum == context.iCurrentJoiningPlayer)
			{
#pragma TODO(vanceo, "Make release optional")

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

				context.hDroppedAndJoinedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				if (context.hDroppedAndJoinedEvent == NULL)
				{
					sr = GetLastError();
					DPL(0, "Couldn't create dropped and joined event!", 0);
					THROW_SYSTEMRESULT;
				} // end if (couldn't create event)

				// Create an event for the DESTROY_PLAYER race in case it happens,
				// see notes below.
				context.hDestroyPlayerForRacingLeave = CreateEvent(NULL, FALSE, FALSE, NULL);
				if (context.hDestroyPlayerForRacingLeave == NULL)
				{
					sr = GetLastError();
					DPL(0, "Couldn't create destroy player for racing leave event!", 0);
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



				// Get ready for everyone to reconnect.
				ZeroMemory(context.pafTesterIndicated,
							(pTNecd->iNumMachines * sizeof (BOOL)));


				// We're going to join, so we expect our player to appear.
				context.fSelfIndicate = TRUE;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Synchronizing with dropping player and those still in session");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Ready for player leave & join",
														NULL, 0,
														NULL,
														0);
				HANDLE_SYNC_RESULT;



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


				// We don't need the local address anymore.
				pDP8AddressLocal->Release();
				pDP8AddressLocal = NULL;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for all players in session to connect");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hDroppedAndJoinedEvent),
															1,
															NULL,
															-1,
															INFINITE,
															NULL);
				HANDLE_WAIT_RESULT;


				CloseHandle(context.hDroppedAndJoinedEvent);
				context.hDroppedAndJoinedEvent = NULL;


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



				// NOTE: If we're not ensuring that the dropping player can never
				// see any traffic from us, then we race with him to join the
				// session before he leaves it.  It's therefore possible to get the
				// CREATE_PLAYER for him.  If we do, however, we need to get a
				// DESTROY_PLAYER for him when he finally does leave the session.
				// Wait for it to come in if that's the case.
				if (context.pafTesterIndicated[context.iCurrentDroppingPlayer])
				{
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
					TEST_SECTION("Dropping player made it into the name table, waiting for DESTROY_PLAYER");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hDestroyPlayerForRacingLeave),
																1,
																NULL,
																-1,
																INFINITE,
																NULL);
					HANDLE_WAIT_RESULT;
				} // end if (dropping player made it in)

				CloseHandle(context.hDestroyPlayerForRacingLeave);
				context.hDestroyPlayerForRacingLeave = NULL;




				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Ensure everyone has our player ID correct");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Ensure correct rejoiner player ID",
														&(context.padpnidTesters[pTNecd->iTesterNum]),
														sizeof (DPNID),
														NULL,
														0);
				HANDLE_SYNC_RESULT;
			} // end else if (we're the joining player)
			TESTSECTION_ELSE
			{
				context.hDroppedAndJoinedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				if (context.hDroppedAndJoinedEvent == NULL)
				{
					sr = GetLastError();
					DPL(0, "Couldn't create rejoin event event!", 0);
					THROW_SYSTEMRESULT;
				} // end if (couldn't create event)


				// Get ready for players to drop and join.
				context.pafTesterIndicated[context.iCurrentDroppingPlayer] = FALSE;
				context.pafTesterIndicated[context.iCurrentJoiningPlayer] = FALSE;
				context.padpnidTesters[context.iCurrentJoiningPlayer] = 0;


				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Synchronizing with leaving and joining players");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Ready for player leave & join",
														NULL, 0,
														NULL,
														0);
				HANDLE_SYNC_RESULT;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for players to drop and rejoin as appropriate");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hDroppedAndJoinedEvent),
															1,
															NULL,
															-1,
															INFINITE,
															NULL);
				HANDLE_WAIT_RESULT;


				CloseHandle(context.hDroppedAndJoinedEvent);
				context.hDroppedAndJoinedEvent = NULL;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Making sure new player ID for rejoiner is correct");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Ensure correct rejoiner player ID",
														NULL, 0,
														&syncdata,
														0);
				HANDLE_SYNC_RESULT;

				CHECKANDGET_SYNCDATA(syncdata, context.iCurrentJoiningPlayer,
									pvSyncData, dwSubOutputDataSize, sizeof (DPNID));

				// Make sure he got the same ID we saw him get.
				if (context.padpnidTesters[context.iCurrentJoiningPlayer] != *((DPNID*) pvSyncData))
				{
					DPL(0, "The original host (tester %i) rejoining player ID %u/%x we received differs from the ID he claims he got (%u/%x)!",
						5, context.iCurrentJoiningPlayer,
						context.padpnidTesters[context.iCurrentJoiningPlayer],
						context.padpnidTesters[context.iCurrentJoiningPlayer],
						*((DPNID*) pvSyncData),
						*((DPNID*) pvSyncData));
					SETTHROW_TESTRESULT(ERROR_NO_MATCH);
				} // end if (IDs don't match)
			} // end else (we're a normal player)
			TESTSECTION_ENDIF




			iCurrentIteration++;

			// If there aren't any more iterations, we can skip the rejoin phase.
			TESTSECTION_IF(iCurrentIteration >= iNumIterations)
			{
				DPL(1, "Final iteration completed.", 0);
				TESTSECTION_BREAK;
			} // end if (last iteration)
			TESTSECTION_ENDIF
		} // end do (while still more iterations to go)
		TESTSECTION_DOWHILE(TRUE);



		// Cleanup people still in session after last iteration.
		TESTSECTION_IF(pTNecd->iTesterNum == context.iCurrentDroppingPlayer)
		{
			sr = g_pDP8PeersList->RemoveFirstReference(pDP8Peer);
			if (sr != S_OK)
			{
				DPL(0, "Couldn't remove DirectPlay8Peer wrapper object from list!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't remove object from list)

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
		} // end if (current dropping tester)
		TESTSECTION_ELSE
		{
			paiDestroyTesters = NULL;
			padpnidDestroyTesters = NULL;

			LOCALALLOC_OR_THROW(int*, paiDestroyTesters,
								((pTNecd->iNumMachines - 1) * sizeof (int)));

			LOCALALLOC_OR_THROW(DPNID*, padpnidDestroyTesters,
								((pTNecd->iNumMachines - 1) * sizeof (DPNID)));

			// Clear this, to be paranoid.
			context.pDP8Peer = NULL;


			// Build arrays of tester numbers and player IDs that exclude the
			// missing player.
			j = 0;
			for(i = 0; i < pTNecd->iNumMachines; i++)
			{
				// Skip the player not in the session.
				if (i == context.iCurrentDroppingPlayer)
					continue;

				paiDestroyTesters[j] = i;
				padpnidDestroyTesters[j] = context.padpnidTesters[i];

				j++;
			} // end for (each tester)



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Destroying peer-to-peer session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_BBLDDP8_PEER_DESTROY));
			((PTID_BBLDDP8_PEER_DESTROY) pvSubInputData)->pDP8Peer = pDP8Peer;
			((PTID_BBLDDP8_PEER_DESTROY) pvSubInputData)->pHandlerContext = &context;
			((PTID_BBLDDP8_PEER_DESTROY) pvSubInputData)->padpnidTesters = padpnidDestroyTesters;

			sr = pTNecd->pExecutor->ExecSubTestCaseArray("3.1.1.2", pvSubInputData,
														sizeof (TID_BBLDDP8_PEER_DESTROY),
														(pTNecd->iNumMachines - 1),
														paiDestroyTesters);
			LocalFree(pvSubInputData);
			pvSubInputData = NULL;

			LocalFree(padpnidDestroyTesters);
			padpnidDestroyTesters = NULL;

			LocalFree(paiDestroyTesters);
			paiDestroyTesters = NULL;

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

	SAFE_LOCALFREE(pszURL);
	SAFE_LOCALFREE(pvSubInputData);
	SAFE_CLOSEHANDLE(hConnectCompleteEvent);
	SAFE_CLOSEHANDLE(context.hDroppedAndJoinedEvent);
	SAFE_CLOSEHANDLE(context.hDroppedAndJoinedEvent);
	SAFE_RELEASE(pDP8AddressLocal);
	SAFE_RELEASE(pDP8AddressRemote);
	SAFE_LOCALFREE(context.pafTesterIndicated);
	SAFE_LOCALFREE(context.padpnidTesters);

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

	return (sr);
} // ConnexExec_LeaveWhileJoinPeer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexExec_LeaveWhileJoinCS()"
//==================================================================================
// ConnexExec_LeaveWhileJoinCS
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.7.1.2 - Simple normal leave while joining client/server test
//				2.7.1.4 - Round robin normal leave while joining client/server test
//				2.7.2.2 - Simple drop while joining client/server test
//				2.7.2.4 - Round robin drop while joining client/server test
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
HRESULT ConnexExec_LeaveWhileJoinCS(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	HRESULT							temphr;
	PTID_PCONNEX_LEAVEWHILEJOIN		pInput = (PTID_PCONNEX_LEAVEWHILEJOIN) pTNecd->pvInputData;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	DWORD							dwSubInputDataSize;
	PWRAPDP8SERVER					pDP8Server = NULL;
	PWRAPDP8CLIENT					pDP8Client= NULL;
	PTOD_BBLDDP8_CS_CREATE_0		pBldDP8CSCreateOutput0 = NULL;
	PTOD_BBLDDP8_CS_CREATE_NOT0		pBldDP8CSCreateOutputNot0 = NULL;
	DWORD							dwSubOutputDataSize;
	CONNEXLEAVEWHILEJOINCONTEXT		context;
	DPN_APPLICATION_DESC			dpnad;
	PDIRECTPLAY8ADDRESS				pDP8AddressRemote = NULL;
	PDIRECTPLAY8ADDRESS				pDP8AddressLocal = NULL;
	PTNFAULTSIM						pFaultSim = NULL;
	HANDLE							hConnectCompleteEvent = NULL;
	DPNHANDLE						dpnhConnect;
	int*							paiDestroyTesters = NULL;
	DPNID*							padpnidDestroyTesters = NULL;
	int								iNumIterations;
	int								iCurrentIteration;
	int								i;
	int								j;
#ifdef DEBUG
	char*							pszURL = NULL;
	DWORD							dwURLBufferSize = 0;
	DWORD							dwURLSize;
#endif // DEBUG



	ZeroMemory(&context, sizeof (CONNEXLEAVEWHILEJOINCONTEXT));
	context.pTNecd = pTNecd;


	BEGIN_TESTCASE
	{
		if ((pTNecd->pExecutor->IsCase("2.7.1.2")) || (pTNecd->pExecutor->IsCase("2.7.2.2")))
		{
			iNumIterations = 1;
		} // end if (simple case)
		else
		{
			REQUIRE_INPUT_DATA(sizeof (TID_PCONNEX_LEAVEWHILEJOIN));

			iNumIterations = pInput->iNumIterations;
		} // end else (round robin case)

		DPL(1, "Will perform %i iterations.", 1, iNumIterations);


		if (pTNecd->iTesterNum == 0)
		{
			// Servers need to allocate an array for all testers' player IDs.  The
			// position of a tester is always the same, regardless of the iteration.
			// We allocate this array even though build session will give us one
			// because we need to modify it, and we can't modify someone else's
			// buffer.

			LOCALALLOC_OR_THROW(DPNID*, context.padpnidTesters,
								(pTNecd->iNumMachines * sizeof (DPNID)));

			// Servers need to allocate an array for add player/delete player
			// indications.  The position of a tester is always the same, regardless
			// of the iteration.

			LOCALALLOC_OR_THROW(BOOL*, context.pafTesterIndicated,
								(pTNecd->iNumMachines * sizeof (BOOL)));
		} // end if (tester 0)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating client/server session");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if (pTNecd->iTesterNum == 0)
			context.pfnDPNMessageHandler = ConnexLeaveWhileJoinServerDPNMessageHandler;
		else
			context.pfnDPNMessageHandler = ConnexLeaveWhileJoinClientDPNMessageHandler;

		context.pTNecd = pTNecd;

		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_CLIENT_SERVER | DPNSESSION_NODPNSVR;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = GUID_CONNEX_LEAVEWHILEJOIN_CS;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		dpnad.pwszSessionName = L"Session";
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.pvApplicationReservedData = 0;



		dwSubInputDataSize = sizeof (TID_BBLDDP8_ALL_CREATE);
							//+ (pTNecd->iNumMachines * sizeof (int));

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, dwSubInputDataSize);
#pragma TODO(vanceo, "Allow SP to be selected")
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->guidSP = CLSID_DP8SP_TCPIP;
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->pHandlerContext = &context;
		((PTID_BBLDDP8_ALL_CREATE) pvSubInputData)->pdpnad = &dpnad;

		/*
		for(i = 0; i < pTNecd->iNumMachines; i++)
		{
			*((PVOID*) (((PBYTE) pvSubInputData) + sizeof (TID_BBLDDP8_ALL_CREATE) + (i * sizeof (int*)))) = &(context.paiReliableMsgsReceivedFromTesters[i]);
		} // end for (each tester)
		*/


		sr = pTNecd->pExecutor->ExecSubTestCase("3.1.2.1", pvSubInputData,
												dwSubInputDataSize, 0);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPL(0, "Couldn't execute sub test case BldSsn:BldCS:Create!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.2.1",
									"Creating client/server session failed!");


		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			CHECKANDGET_SUBOUTPUTDATA(pSubResult,
										pBldDP8CSCreateOutput0,
										dwSubOutputDataSize,
										(sizeof (TOD_BBLDDP8_CS_CREATE_0) + (pTNecd->iNumMachines * sizeof (DPNID))));

			pDP8Server = pBldDP8CSCreateOutput0->pDP8Server;
			pDP8Server->m_dwRefCount++; // we're using it during this function

			CopyMemory(context.padpnidTesters, (pBldDP8CSCreateOutput0 + 1),
						(pTNecd->iNumMachines * sizeof (DPNID)));
		} // end if (tester 0)
		TESTSECTION_ELSE
		{
			CHECKANDGET_SUBOUTPUTDATA(pSubResult,
										pBldDP8CSCreateOutputNot0,
										dwSubOutputDataSize,
										(sizeof (TOD_BBLDDP8_CS_CREATE_NOT0) + (pTNecd->iNumMachines * sizeof (DPNID))));

			pDP8Client = pBldDP8CSCreateOutputNot0->pDP8Client;
			pDP8Client->m_dwRefCount++; // we're using it during this function



			// Everybody who might be rejoining the session at some point needs to
			// retrieve the address of the host so they can rejoin easily.

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Getting host's session address");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Client->DP8C_GetServerAddress(&pDP8AddressRemote, 0);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't get server address!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't get server address)


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

			DPL(1, "Will use address (\"%s\") to reconnect.", 1, pszURL);
#endif // DEBUG
		} // end else (not tester 0)
		TESTSECTION_ENDIF





		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			// The tester who's supposed to join first needs to drop out of
			// the session we built.

			context.hDroppedAndJoinedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (context.hDroppedAndJoinedEvent == NULL)
			{
				sr = GetLastError();
				DPL(0, "Couldn't create dropped and joined event!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't create event)


			context.iCurrentDroppingPlayer = pTNecd->iNumMachines - 1;
			context.iCurrentJoiningPlayer = -1;


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for testers to be ready to temporarily leave");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

			sr = pTNecd->pExecutor->SyncWithTesters("Ready for temp leave", NULL, 0, NULL,
													1, (pTNecd->iNumMachines - 1));
			HANDLE_SYNC_RESULT;



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for player to temporarily drop");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hDroppedAndJoinedEvent),
														1,
														NULL,
														-1,
														INFINITE,
														NULL);
			HANDLE_WAIT_RESULT;


			// Forget the dropping player's ID
			context.padpnidTesters[pTNecd->iNumMachines - 1] = 0;

			CloseHandle(context.hDroppedAndJoinedEvent);
			context.hDroppedAndJoinedEvent = NULL;
		} // end if (tester 0)
		TESTSECTION_ELSEIF(pTNecd->iTesterNum == (pTNecd->iNumMachines - 1))
		{
			// If we're the tester who's supposed to join first, we need to drop out of
			// the session we built (we only joined the first time so we could get the
			// host's address.

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for host to be ready for our temporary leave");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

			sr = pTNecd->pExecutor->SyncWithTesters("Ready for temp leave", NULL, 0, NULL,
													1, 0);
			HANDLE_SYNC_RESULT;



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Leaving session temporarily");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Client->DP8C_Close(0);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't close interface!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't close)



#pragma TODO(vanceo, "Make release optional")

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Client object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Client->Release();
			if (tr != S_OK)
			{
				DPL(0, "Couldn't release DirectPlay8Client object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't release)
		} // end else if (first joiner)
		TESTSECTION_ENDIF




		
		// Start at the very beginning...
		iCurrentIteration = 0;
		TESTSECTION_DO
		{
			// Remember who's dropping and who's joining.
			context.iCurrentDroppingPlayer = (iCurrentIteration % (pTNecd->iNumMachines - 1)) + 1;
			context.iCurrentJoiningPlayer = context.iCurrentDroppingPlayer + (pTNecd->iNumMachines - 2);
			if (context.iCurrentJoiningPlayer >= pTNecd->iNumMachines)
				context.iCurrentJoiningPlayer = (context.iCurrentJoiningPlayer - pTNecd->iNumMachines) + 1;
			
			// Note that tester 0 should never be a dropping or joining player.


			DPL(1, "Iteration %i: dropping player = %i, joining player = %i, (we're tester %i so normal = %B)",
				5, iCurrentIteration, context.iCurrentDroppingPlayer,
				context.iCurrentJoiningPlayer, pTNecd->iTesterNum,
				(((pTNecd->iTesterNum == context.iCurrentDroppingPlayer) || (pTNecd->iTesterNum == context.iCurrentJoiningPlayer)) ? FALSE : TRUE));

			// Ignore error
			pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
									"Starting iteration #%i",
									1, (iCurrentIteration + 1));


			TESTSECTION_IF(pTNecd->iTesterNum == 0)
			{
				context.hDroppedAndJoinedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				if (context.hDroppedAndJoinedEvent == NULL)
				{
					sr = GetLastError();
					DPL(0, "Couldn't create rejoin event event!", 0);
					THROW_SYSTEMRESULT;
				} // end if (couldn't create event)


				// Get ready for players to drop and join.
				context.pafTesterIndicated[context.iCurrentDroppingPlayer] = FALSE;
				context.pafTesterIndicated[context.iCurrentJoiningPlayer] = FALSE;
				context.padpnidTesters[context.iCurrentJoiningPlayer] = 0;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Synchronizing with dropping and joining players");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Ready for player leave & join",
														NULL, 0,
														NULL,
														2, context.iCurrentDroppingPlayer,
														context.iCurrentJoiningPlayer);
				HANDLE_SYNC_RESULT;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for players to drop and rejoin as appropriate");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hDroppedAndJoinedEvent),
															1,
															NULL,
															-1,
															INFINITE,
															NULL);
				HANDLE_WAIT_RESULT;


				CloseHandle(context.hDroppedAndJoinedEvent);
				context.hDroppedAndJoinedEvent = NULL;
			} // end if (tester 0)
			TESTSECTION_ELSEIF(pTNecd->iTesterNum == context.iCurrentDroppingPlayer)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Synchronizing with host before dropping");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Ready for player leave & join",
														NULL, 0,
														NULL,
														1, 0);
				HANDLE_SYNC_RESULT;



				TESTSECTION_IF((pTNecd->pExecutor->IsCase("2.7.2.2")) || (pTNecd->pExecutor->IsCase("2.7.2.4")))
				{
					context.hDroppedAndJoinedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
					if (context.hDroppedAndJoinedEvent == NULL)
					{
						sr = GetLastError();
						DPL(0, "Couldn't create rejoin event event!", 0);
						THROW_SYSTEMRESULT;
					} // end if (couldn't create event)



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
					TEST_SECTION("Disconnecting sends and received, will drop from session");
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
					TEST_SECTION("Waiting for connection to server to drop");
					// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

					sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hDroppedAndJoinedEvent,
																1,
																NULL,
																-1,
																INFINITE,
																NULL);
					HANDLE_WAIT_RESULT;



					CloseHandle(context.hDroppedAndJoinedEvent);
					context.hDroppedAndJoinedEvent = NULL;



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




				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Disconnecting from session");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Client->DP8C_Close(0);
				if (tr != DPN_OK)
				{
					DPL(0, "Closing failed!", 0);
					THROW_TESTRESULT;
				} // end if (close failed)




#pragma TODO(vanceo, "Make release optional")

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Releasing DirectPlay8Client object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Client->Release();
				if (tr != S_OK)
				{
					DPL(0, "Couldn't release DirectPlay8Client object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't release)
			} // end if (we're the dropping player)
			TESTSECTION_ELSEIF(pTNecd->iTesterNum == context.iCurrentJoiningPlayer)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Recreating DirectPlay8Client object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Client->CoCreate();
				if (tr != S_OK)
				{
					DPL(0, "Couldn't CoCreate DirectPlay8Client object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't CoCreate)




				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Initializing object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				// We have to use the bldssn message handler because we use the
				// destroy session helper later, and that requires the appropriate
				// message handler.
				tr = pDP8Client->DP8C_Initialize(&context,
												BldDP8ClientDPNMessageHandler,
												0);
				if (tr != S_OK)
				{
					DPL(0, "Couldn't initialize client object!", 0);
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



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Synchronizing with host before rejoining");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->SyncWithTesters("Ready for player leave & join",
														NULL, 0,
														NULL,
														1, 0);
				HANDLE_SYNC_RESULT;



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Reconnecting to session");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Client->DP8C_Connect(&dpnad,
											pDP8AddressRemote,
											pDP8AddressLocal,
											NULL,
											NULL,
											NULL,
											0,
											hConnectCompleteEvent,
											&dpnhConnect,
											0);
				if (tr != (HRESULT) DPNSUCCESS_PENDING)
				{
					DPL(0, "Connect didn't return expected PENDING success code!", 0);
					THROW_TESTRESULT;
				} // end if (connect not pending)


				// We don't need the local address anymore.
				pDP8AddressLocal->Release();
				pDP8AddressLocal = NULL;




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

			} // end else if (we're the joining player)
			TESTSECTION_ELSE
			{
				// We've got nothing to do.

				DPL(1, "Nothing to do for tester %i during iteration %i (dropping player = %i, joining player = %i)",
					4, pTNecd->iTesterNum, iCurrentIteration,
					context.iCurrentDroppingPlayer, context.iCurrentJoiningPlayer);

				// Ignore error
				pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
										"Nothing to do for tester %i during iteration %i",
										2, pTNecd->iTesterNum,
										(iCurrentIteration + 1));
			} // end else (we're a normal player)
			TESTSECTION_ENDIF



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for everyone to complete iteration");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->SyncWithTesters("Iteration complete", NULL, 0, NULL, 0);
			HANDLE_SYNC_RESULT;



			iCurrentIteration++;

			// If there aren't any more iterations, we can skip the rejoin phase.
			TESTSECTION_IF(iCurrentIteration >= iNumIterations)
			{
				DPL(1, "Final iteration completed.", 0);
				TESTSECTION_BREAK;
			} // end if (last iteration)
			TESTSECTION_ENDIF
		} // end do (while still more iterations to go)
		TESTSECTION_DOWHILE(TRUE);



		// Cleanup people still in session after last iteration.
		TESTSECTION_IF(pTNecd->iTesterNum == context.iCurrentDroppingPlayer)
		{
			sr = g_pDP8ClientsList->RemoveFirstReference(pDP8Client);
			if (sr != S_OK)
			{
				DPL(0, "Couldn't remove DirectPlay8Client wrapper object from list!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't remove object from list)

			pDP8Client->m_dwRefCount--;
			if (pDP8Client->m_dwRefCount == 0)
			{
				DPL(7, "Deleting DirectPlay8Client wrapper object %x.",
					1, pDP8Client);
				delete (pDP8Client);
			} // end if (can delete the object)
			else
			{
				DPL(0, "WARNING: Can't delete DirectPlay8Client wrapper object %x, it's refcount is %u!?",
					2, pDP8Client, pDP8Client->m_dwRefCount);
			} // end else (can't delete the object)
			pDP8Client = NULL;
		} // end if (current dropping tester)
		TESTSECTION_ELSE
		{
			paiDestroyTesters = NULL;
			padpnidDestroyTesters = NULL;

			LOCALALLOC_OR_THROW(int*, paiDestroyTesters,
								((pTNecd->iNumMachines - 1) * sizeof (int)));

			if (pTNecd->iTesterNum == 0)
			{
				LOCALALLOC_OR_THROW(DPNID*, padpnidDestroyTesters,
									((pTNecd->iNumMachines - 1) * sizeof (DPNID)));
			} // end if (tester 0)


			// Build arrays of tester numbers and player IDs that exclude the
			// missing player.
			j = 0;
			for(i = 0; i < pTNecd->iNumMachines; i++)
			{
				// Skip the player not in the session.
				if (i == context.iCurrentDroppingPlayer)
					continue;

				paiDestroyTesters[j] = i;

				if (pTNecd->iTesterNum == 0)
					padpnidDestroyTesters[j] = context.padpnidTesters[i];

				j++;
			} // end for (each tester)



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
														(pTNecd->iNumMachines - 1),
														paiDestroyTesters);

			LocalFree(pvSubInputData);
			pvSubInputData = NULL;

			SAFE_LOCALFREE(padpnidDestroyTesters);

			LocalFree(paiDestroyTesters);
			paiDestroyTesters = NULL;

			if (sr != S_OK)
			{
				DPL(0, "Couldn't execute sub test case BldSsn:BldCS:Destroy!", 0);
				THROW_SYSTEMRESULT;
			} // end if (failed executing sub test case)

			GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "3.1.2.2",
										"Destroying client/server session failed!");

			if (pTNecd->iTesterNum == 0)
			{
				pDP8Server->m_dwRefCount--;
				if (pDP8Server->m_dwRefCount == 0)
				{
					DPL(7, "Deleting server wrapper object %x.", 1, pDP8Server);
					delete (pDP8Server);
				} // end if (can delete the object)
				else
				{
					DPL(0, "WARNING: Can't delete server wrapper object %x, it's refcount is %u!?",
						2, pDP8Server, pDP8Server->m_dwRefCount);
				} // end else (can't delete the object)
				pDP8Server = NULL;
			} // end if (tester 0)
			else
			{
				pDP8Client->m_dwRefCount--;
				if (pDP8Client->m_dwRefCount == 0)
				{
					DPL(7, "Deleting client wrapper object %x.", 1, pDP8Client);
					delete (pDP8Client);
				} // end if (can delete the object)
				else
				{
					DPL(0, "WARNING: Can't delete client wrapper object %x, it's refcount is %u!?",
						2, pDP8Client, pDP8Client->m_dwRefCount);
				} // end else (can't delete the object)
				pDP8Client = NULL;
			} // end else (not tester 0)
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

	SAFE_LOCALFREE(pszURL);
	SAFE_LOCALFREE(pvSubInputData);
	SAFE_CLOSEHANDLE(hConnectCompleteEvent);
	SAFE_CLOSEHANDLE(context.hDroppedAndJoinedEvent);
	SAFE_CLOSEHANDLE(context.hDroppedAndJoinedEvent);
	SAFE_RELEASE(pDP8AddressLocal);
	SAFE_RELEASE(pDP8AddressRemote);
	SAFE_LOCALFREE(context.pafTesterIndicated);
	SAFE_LOCALFREE(context.padpnidTesters);

	if (pDP8Server != NULL)
	{
		temphr = pDP8Server->DP8S_Close(0);
		if (temphr != DPN_OK)
		{
			DPL(0, "Closing server interface failed!  0x%08x", 1, temphr);
			OVERWRITE_SR_IF_OK(temphr);
		} // end if (closing server interface failed)


		// Ignore error, it may not actually be on the list.
		g_pDP8ServersList->RemoveFirstReference(pDP8Server);


		pDP8Server->m_dwRefCount--;
		if (pDP8Server->m_dwRefCount == 0)
		{
			DPL(7, "Deleting server wrapper object %x.", 1, pDP8Server);
			delete (pDP8Server);
		} // end if (can delete the object)
		else
		{
			DPL(0, "WARNING: Can't delete server wrapper object %x, it's refcount is %u!?",
				2, pDP8Server, pDP8Server->m_dwRefCount);
		} // end else (can't delete the object)
		pDP8Server = NULL;
	} // end if (have server object)

	if (pDP8Client != NULL)
	{
		temphr = pDP8Client->DP8C_Close(0);
		if (temphr != DPN_OK)
		{
			DPL(0, "Closing client interface failed!  0x%08x", 1, temphr);
			OVERWRITE_SR_IF_OK(temphr);
		} // end if (closing client interface failed)


		// Ignore error, it may not actually be on the list.
		g_pDP8ClientsList->RemoveFirstReference(pDP8Client);


		pDP8Client->m_dwRefCount--;
		if (pDP8Client->m_dwRefCount == 0)
		{
			DPL(7, "Deleting client wrapper object %x.", 1, pDP8Client);
			delete (pDP8Client);
		} // end if (can delete the object)
		else
		{
			DPL(0, "WARNING: Can't delete client wrapper object %x, it's refcount is %u!?",
				2, pDP8Client, pDP8Client->m_dwRefCount);
		} // end else (can't delete the object)
		pDP8Client = NULL;
	} // end if (have client object)

	return (sr);
} // ConnexExec_LeaveWhileJoinCS
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexGet_ManyAll()"
//==================================================================================
// ConnexGet_ManyAll
//----------------------------------------------------------------------------------
//
// Description: Callback that retrieves the initial data for the test case(s):
//				2.7.3.1 - Many peer connect attempts test
//				2.7.3.2 - Many client/server connect attempts test
//
// Arguments:
//	PTNGETINPUTDATA pTNgid	Pointer to parameter block with information on how and
//							where to place the input data.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ConnexGet_ManyAll(PTNGETINPUTDATA pTNgid)
{
	PTID_PCONNEX_MANY	pParams = (PTID_PCONNEX_MANY) pTNgid->pvData;
	PLSTRINGLIST		pStrings = NULL;
	char*				pszTemp;



	pTNgid->dwDataSize = sizeof (TID_PCONNEX_MANY);

	if (pParams == NULL)
		return (ERROR_BUFFER_TOO_SMALL);


	// Start with default values
	pParams->iNumIterations = 5;
	pParams->iNumObjects = 1;
	pParams->fOverlap = FALSE;
	pParams->fDontReuseObjects = FALSE;
	pParams->dwUserDataSize = 0;
	pParams->fReject = FALSE;
    

	// Get any input data strings that might exist.
	GETANYINPUTSTRINGS(pTNgid, pStrings);


	// Okay, so if we found strings, check if any of the options are specified.
	// If they are, override the default with the items' value.
	if (pStrings != NULL)
	{
		pszTemp = pStrings->GetValueOfKey("NumIterations");
		if (pszTemp != NULL)
			pParams->iNumIterations = StringToInt(pszTemp);

		pszTemp = pStrings->GetValueOfKey("NumObjects");
		if (pszTemp != NULL)
			pParams->iNumObjects = StringToInt(pszTemp);

		pParams->fOverlap = pStrings->IsKeyTrue("Overlap");
		pParams->fDontReuseObjects = pStrings->IsKeyTrue("DontReuseObjects");

		pszTemp = pStrings->GetValueOfKey("UserDataSize");
		if (pszTemp != NULL)
			pParams->dwUserDataSize = StringToDWord(pszTemp);

		pParams->fReject = pStrings->IsKeyTrue("Reject");
	} // end if (we found some items)


	return (S_OK);
} // ConnexGet_ManyAll
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexWrite_ManyAll()"
//==================================================================================
// ConnexWrite_ManyAll
//----------------------------------------------------------------------------------
//
// Description: Callback that converts a buffer to strings for the test case(s):
//				2.7.3.1 - Many peer connect attempts test
//				2.7.3.2 - Many client/server connect attempts test
//
// Arguments:
//	PTNWRITEDATA pTNwd	Parameter block with information and data to write.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ConnexWrite_ManyAll(PTNWRITEDATA pTNwd)
{
	PTID_PCONNEX_MANY	pInputData = (PTID_PCONNEX_MANY) pTNwd->pvData;

	
	switch (pTNwd->dwLocation)
	{
		case TNTSDL_REPORTASSIGN:
			REQUIRE_ASSIGNREPORT_DATA(sizeof (TID_PCONNEX_MANY));


			WRITESPRINTFDATALINE("NumIterations= %i", 1, pInputData->iNumIterations);
			WRITESPRINTFDATALINE("NumObjects= %i", 1, pInputData->iNumObjects);
			WRITESPRINTFDATALINE("Overlap= %B", 1, pInputData->fOverlap);
			WRITESPRINTFDATALINE("DontReuseObjects= %B", 1, pInputData->fDontReuseObjects);
			WRITESPRINTFDATALINE("UserDataSize= %u", 1, pInputData->dwUserDataSize);
			WRITESPRINTFDATALINE("Reject= %B", 1, pInputData->fReject);
		  break;

		default:
			DPL(0, "Got data for unexpected location %u!",
				1, pTNwd->dwLocation);
			return (ERROR_INVALID_PARAMETER);
		  break;
	} // end switch (on the source of the data buffer)

	
	return (S_OK);
} // ConnexWrite_ManyAll
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexExec_ManyAll()"
//==================================================================================
// ConnexExec_ManyAll
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.7.3.1 - Many peer connect attempts test
//				2.7.3.2 - Many client/server connect attempts test
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
HRESULT ConnexExec_ManyAll(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult				sr;
	CTNTestResult				tr;
	//HRESULT						temphr;
	PTID_PCONNEX_MANY			pInput = (PTID_PCONNEX_MANY) pTNecd->pvInputData;
	PVOID*						papvInterfaces = NULL;
	int							iNumInterfaces = 0;
	BOOL						fClientServer = FALSE;
	DPN_APPLICATION_DESC		dpnad;
	PDIRECTPLAY8ADDRESS			pDP8AddressRemote = NULL;
	PDIRECTPLAY8ADDRESS			pDP8AddressLocal = NULL;
	CTNSyncDataList				syncdata;
	PVOID						pvSyncData;
	DWORD						dwSyncDataSize;
	CONNEXMANYHOSTCONTEXT		hostcontext;
	CONNEXMANYCONNECTORCONTEXT	connectorcontext;
	PVOID						pvUserConnectData = NULL;
	int							i;
	int							iCurrentIteration;
	int							iCurrentConnectingTester;
	int							iCurrentConnectingInterface;
	PHOSTADDRESSESSYNCDATA		pHostAddressesSyncData = NULL;
	PDIRECTPLAY8ADDRESS*		paDP8HostAddresses = NULL;
	DWORD						dwNumAddresses = 0;
	DWORD						dwTemp;
	DWORD						dwURLsBufferSize = 0;
	DWORD						dwRemainingBufferSize;
	char*						pszURL;
	DWORD						dwSize;
	DWORD						dwDataType;
	char						szIPString[16]; // max IP string size + NULL termination
	WCHAR						wszIPCompareString[16]; // max IP string size + NULL termination



	ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));

	ZeroMemory(&hostcontext, sizeof (CONNEXMANYHOSTCONTEXT));
	hostcontext.pTNecd = pTNecd;

	ZeroMemory(&connectorcontext, sizeof (CONNEXMANYCONNECTORCONTEXT));
	connectorcontext.pTNecd = pTNecd;


	BEGIN_TESTCASE
	{
		REQUIRE_INPUT_DATA(sizeof (TID_PCONNEX_MANY));


		if (pTNecd->pExecutor->IsCase("2.7.3.2"))
			fClientServer = TRUE;



		if (pTNecd->iTesterNum == 0)
		{
			// Pre-allocate the user data buffer we're going to when replying.
			hostcontext.dwUserDataSize = pInput->dwUserDataSize;
			if (pInput->dwUserDataSize > 0)
			{
				LOCALALLOC_OR_THROW(PVOID, hostcontext.pvReplyData,
									pInput->dwUserDataSize);

				FillWithDWord(hostcontext.pvReplyData, pInput->dwUserDataSize,
							DATAPATTERN_REPLY);
			} // end if (there's supposed to be user data)

			hostcontext.fReject = pInput->fReject;


			CREATEEVENT_OR_THROW(hostcontext.hConnectsDoneEvent,
								NULL, FALSE, FALSE, NULL);

			hostcontext.iNumConnectsRemaining = pInput->iNumIterations;

			
			iNumInterfaces = 1;
		} // end if (tester 0)
		else
		{
			// Pre-allocate the user data buffer we're going to send.
			if (pInput->dwUserDataSize > 0)
			{
				LOCALALLOC_OR_THROW(PVOID, pvUserConnectData,
									pInput->dwUserDataSize);

				FillWithDWord(pvUserConnectData, pInput->dwUserDataSize,
							DATAPATTERN_CONNECT);
			} // end if (there's supposed to be user data)

			connectorcontext.dwReplyDataSize = pInput->dwUserDataSize;

			connectorcontext.fReject = pInput->fReject;


			
			iNumInterfaces = pInput->iNumObjects;

			LOCALALLOC_OR_THROW(HANDLE*, connectorcontext.pahConnectCompleteEvents,
								(iNumInterfaces * sizeof (HANDLE)));

			LOCALALLOC_OR_THROW(DPNHANDLE*, connectorcontext.padpnhConnects,
								(iNumInterfaces * sizeof (DPNHANDLE)));
		} // end else (not tester 0)

		LOCALALLOC_OR_THROW(PVOID*, papvInterfaces,
							sizeof (PVOID) * iNumInterfaces);



		TESTSECTION_FOR(i = 0; i < iNumInterfaces; i++)
		{
			TESTSECTION_IF(fClientServer)
			{
				TESTSECTION_IF(pTNecd->iTesterNum == 0)
				{
					DPL(0, "Creating DirectPlay8Server object %i", 1, i);
					// Ignore error
					pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
											"Creating DirectPlay8Server object %i",
											1, i);


					papvInterfaces[i] = new (CWrapDP8Server);
					if (papvInterfaces[i] == NULL)
					{
						SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
					} // end if (couldn't allocate object)


					tr = ((PWRAPDP8SERVER) papvInterfaces[i])->CoCreate();
					if (tr != DPN_OK)
					{
						DPL(0, "CoCreating DP8Server object failed!", 0);
						THROW_TESTRESULT;
					} // end if (function failed)



					DPL(0, "Initializing DirectPlay8Server object %i", 1, i);
					// Ignore error
					pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
											"Initializing DirectPlay8Server object %i",
											1, i);

					tr = ((PWRAPDP8SERVER) papvInterfaces[i])->DP8S_Initialize(&hostcontext,
																				ConnexManyHostDPNMessageHandler,
																				0);
					if (tr != S_OK)
					{
						DPL(0, "Couldn't initialize server object!", 0);
						THROW_TESTRESULT;
					} // end if (couldn't initialize)
				} // end if (tester 0)
				TESTSECTION_ELSE
				{
					DPL(0, "Creating DirectPlay8Client object %i", 1, i);
					// Ignore error
					pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
											"Creating DirectPlay8Client object %i",
											1, i);


					papvInterfaces[i] = new (CWrapDP8Client);
					if (papvInterfaces[i] == NULL)
					{
						SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
					} // end if (couldn't allocate object)


					tr = ((PWRAPDP8CLIENT) papvInterfaces[i])->CoCreate();
					if (tr != DPN_OK)
					{
						DPL(0, "CoCreating DP8Client object failed!", 0);
						THROW_TESTRESULT;
					} // end if (function failed)




					DPL(0, "Initializing DirectPlay8Client object %i", 1, i);
					// Ignore error
					pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
											"Initializing DirectPlay8Client object %i",
											1, i);

					tr = ((PWRAPDP8CLIENT) papvInterfaces[i])->DP8C_Initialize(&connectorcontext,
																				ConnexManyConnectorDPNMessageHandler,
																				0);
					if (tr != S_OK)
					{
						DPL(0, "Couldn't initialize client object!", 0);
						THROW_TESTRESULT;
					} // end if (couldn't initialize)
				} // end else (not tester 0)
				TESTSECTION_ENDIF
			} // end if (client/server)
			TESTSECTION_ELSE
			{
				DPL(0, "Creating DirectPlay8Peer object %i", 1, i);
				// Ignore error
				pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
										"Creating DirectPlay8Peer object %i",
										1, i);



				papvInterfaces[i] = new (CWrapDP8Peer);
				if (papvInterfaces[i] == NULL)
				{
					SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
				} // end if (couldn't allocate object)


				tr = ((PWRAPDP8PEER) papvInterfaces[i])->CoCreate();
				if (tr != DPN_OK)
				{
					DPL(0, "CoCreating DP8Peer object failed!", 0);
					THROW_TESTRESULT;
				} // end if (function failed)




				DPL(0, "Initializing DirectPlay8Peer object %i", 1, i);
				// Ignore error
				pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
										"Initializing DirectPlay8Peer object %i",
										1, i);

				if (pTNecd->iTesterNum == 0)
				{
					tr = ((PWRAPDP8PEER) papvInterfaces[i])->DP8P_Initialize(&hostcontext,
																			ConnexManyHostDPNMessageHandler,
																			0);
				} // end if (tester 0)
				else
				{
					tr = ((PWRAPDP8PEER) papvInterfaces[i])->DP8P_Initialize(&connectorcontext,
																			ConnexManyConnectorDPNMessageHandler,
																			0);
				} // end else (not tester 0)

				if (tr != S_OK)
				{
					DPL(0, "Couldn't initialize peer object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't initialize)
			} // end else (peer-to-peer)
			TESTSECTION_ENDIF
		} // end for (each interface to create)
		TESTSECTION_ENDFOR



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("CoCreating local address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) &pDP8AddressLocal);
		if (sr != S_OK)
		{
			DPL(0, "Couldn't CoCreate address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (couldn't CoCreate object)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting local address object's SP to TCP/IP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressLocal->SetSP(&CLSID_DP8SP_TCPIP);
		if (tr != S_OK)
		{
			DPL(0, "Couldn't set SP for address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)




		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpnad.dwFlags = 0;
		if ((pTNecd->iTesterNum == 0) && (fClientServer))
			dpnad.dwFlags |= DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
		dpnad.guidApplication = (fClientServer) ? GUID_CONNEX_MANY_CS : GUID_CONNEX_MANY_PEER;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		dpnad.pwszSessionName = (pTNecd->iTesterNum == 0) ? L"Session" : NULL;
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.pvApplicationReservedData = 0;



		// Tester 0 should start hosting a session, tester 1 should wait until
		// the session is up.
		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Hosting session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if (fClientServer)
			{
				tr = ((PWRAPDP8SERVER) papvInterfaces[0])->DP8S_Host(&dpnad,
																	&pDP8AddressLocal,
																	1,
																	NULL,
																	NULL,
																	NULL,
																	0);
			} // end if (client/server)
			else
			{
				tr = ((PWRAPDP8PEER) papvInterfaces[0])->DP8P_Host(&dpnad,
																	&pDP8AddressLocal,
																	1,
																	NULL,
																	NULL,
																	NULL,
																	0);
			} // end else (peer-to-peer)

			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't start hosting session!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't host session)


			pDP8AddressLocal->Release();
			pDP8AddressLocal = NULL;




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Getting local host addresses");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if (fClientServer)
			{
				tr = ((PWRAPDP8SERVER) papvInterfaces[0])->DP8S_GetLocalHostAddresses(NULL,
																					&dwNumAddresses,
																					0);
			} // end if (client/server)
			else
			{
				tr = ((PWRAPDP8PEER) papvInterfaces[0])->DP8P_GetLocalHostAddresses(NULL,
																					&dwNumAddresses,
																					0);
			} // end else (peer-to-peer)

			if (tr != DPNERR_BUFFERTOOSMALL)
			{
				DPL(0, "Couldn't get number of host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't get host address)



			DPL(1, "We are hosting on %u addresses.", 1, dwNumAddresses); 


			LOCALALLOC_OR_THROW(PDIRECTPLAY8ADDRESS*, paDP8HostAddresses,
								dwNumAddresses * sizeof (PDIRECTPLAY8ADDRESS));


			if (fClientServer)
			{
				tr = ((PWRAPDP8SERVER) papvInterfaces[0])->DP8S_GetLocalHostAddresses(paDP8HostAddresses,
																					&dwNumAddresses,
																					0);
			} // end if (client/server)
			else
			{
				tr = ((PWRAPDP8PEER) papvInterfaces[0])->DP8P_GetLocalHostAddresses(paDP8HostAddresses,
																					&dwNumAddresses,
																					0);
			} // end else (peer-to-peer)

			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't get host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't get host address)

			// Get the total size of all the addresses.
			for(dwTemp = 0; dwTemp < dwNumAddresses; dwTemp++)
			{
				dwSize = 0;
				tr = paDP8HostAddresses[dwTemp]->GetURLA(NULL, &dwSize);
				if (tr != DPNERR_BUFFERTOOSMALL)
				{
					DPL(0, "Couldn't get host address %u's URL size!", 1, dwTemp);
					THROW_TESTRESULT;
				} // end if (not buffer too small)
				
				dwURLsBufferSize += dwSize;
			} // end for (each address)

			// Allocate a buffer for all the URLs.
			LOCALALLOC_OR_THROW(PHOSTADDRESSESSYNCDATA,
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
					DPL(0, "Couldn't get host address %u's URL!", 1, dwTemp);
					THROW_TESTRESULT;
				} // end if (failed)


				DPL(1, "Address %u: \"%s\"",
					2, dwTemp, pszURL);

				dwRemainingBufferSize -= dwSize;
				pszURL += dwSize;

				
				// We're done with this address object.
				paDP8HostAddresses[dwTemp]->Release();
				paDP8HostAddresses[dwTemp] = NULL;
			} // end for (each address)

			// We're done with the address array.
			LocalFree(paDP8HostAddresses);
			paDP8HostAddresses = NULL;


			// Make sure we used all of the buffer.
			if (dwRemainingBufferSize != 0)
			{
				DPL(0, "Didn't use all of or overran the allocated buffer (didn't use %i bytes)!",
					1, ((int) dwRemainingBufferSize));
				SETTHROW_TESTRESULT(E_FAIL);
			} // end if (didn't use all of the buffer)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Sending addresses to everyone");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->SyncWithTesters("Host addresses",
													pHostAddressesSyncData,
													(sizeof (HOSTADDRESSESSYNCDATA) + dwURLsBufferSize),
													NULL,
													0);
			HANDLE_SYNC_RESULT;


			LocalFree(pHostAddressesSyncData);
			pHostAddressesSyncData = NULL;



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Beginning multiple connection sequence");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// Technically it already began once the sync completed.  Picky picky
			// picky...


			TESTSECTION_IF(! pInput->fOverlap)
			{
				// We don't need to synchronize on the last iteration, it will be caught by
				// the final sync.
				TESTSECTION_FOR(iCurrentIteration = 0; iCurrentIteration < (pInput->iNumIterations - 1); iCurrentIteration++)
				{
					iCurrentConnectingTester = (iCurrentIteration % (pTNecd->iNumMachines - 1)) + 1;
					iCurrentConnectingInterface = (iCurrentIteration / (pTNecd->iNumMachines - 1)) % pInput->iNumObjects;
					

					DPL(1, "Waiting for tester %i to perform connection iteration #%i (using interface %i).",
						3, iCurrentConnectingTester, iCurrentIteration,
						iCurrentConnectingInterface);

					// Ignore error
					pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
											"Waiting for tester %i to perform connection iteration #%i",
											2, iCurrentConnectingTester,
											iCurrentIteration);


					// Synchronize with everyone.
					sr = pTNecd->pExecutor->SyncWithTesters("Connection iteration complete",
															NULL, 0, NULL,
															0);
					HANDLE_SYNC_RESULT;
				} // end for (each iteration)
				TESTSECTION_ENDFOR
			} // end if (not overlapped connections)
			TESTSECTION_ENDIF
		} // end if (tester 0)
		TESTSECTION_ELSE
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Receiving addresses of host");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->SyncWithTesters("Host addresses",
													NULL,
													0,
													&syncdata,
													0);
			HANDLE_SYNC_RESULT;

			CHECKANDGET_MINIMUM_SYNCDATA(syncdata, 0, pvSyncData, dwSyncDataSize,
										(sizeof (HOSTADDRESSESSYNCDATA) + (strlen(DPNA_HEADER_A) + 1)));


			dwNumAddresses = ((PHOSTADDRESSESSYNCDATA) pvSyncData)->dwNumAddresses;

			// Start with the first address
			pszURL = (char*) (((PBYTE) pvSyncData) + sizeof (HOSTADDRESSESSYNCDATA));



			DPL(1, "Tester 0 (host) has %u connectable addresses.",
				1, dwNumAddresses);



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("CoCreating remote (host's) address object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
								IID_IDirectPlay8Address, (LPVOID*) &pDP8AddressRemote);
			if (sr != S_OK)
			{
				DPL(0, "Couldn't CoCreate remote (host's) address object!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't CoCreate object)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Finding appropriate address");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma TODO(vanceo, "Support other SPs")
			// Figure out what IP we should use to connect to the host.
			sr = pTNecd->pExecutor->GetTestersIPForPort(0, DPLAY8_PORT, szIPString);
			if (sr != S_OK)
			{
				DPL(0, "Couldn't get tester 0's IP for port %u!",
					1, DPLAY8_PORT);
				THROW_SYSTEMRESULT;
			} // end if (couldn't get tester 0's IP)


			for(dwTemp = 0; dwTemp < dwNumAddresses; dwTemp++)
			{
				DPL(1, "Host address %u: %s", 2, dwTemp, pszURL);


				DPL(0, "Building remote address %u from string URL.", 1, dwTemp);
				// Ignore error
				pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
										"Building remote address %u from string URL.",
										1, dwTemp);


				tr = pDP8AddressRemote->BuildFromURLA(pszURL);
				if (tr != DPN_OK)
				{
					DPL(0, "Couldn't build host's address %u from URL (\"%s\")!",
						2, dwTemp, pszURL);
					THROW_TESTRESULT;
				} // end if (couldn't build from URL)



				DPL(0, "Getting remote address %u's hostname component.", 1, dwTemp);
				// Ignore error
				pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
										"Getting remote address %u's hostname component.",
										1, dwTemp);

				dwSize = 16 * sizeof (WCHAR);

				tr = pDP8AddressRemote->GetComponentByName(DPNA_KEY_HOSTNAME,
															wszIPCompareString,
															&dwSize,
															&dwDataType);
				if (tr != DPN_OK)
				{
					DPL(0, "Couldn't get host's address %u hostname component!",
						1, dwTemp);
					THROW_TESTRESULT;
				} // end if (couldn't get component by name)

				// Make sure it's the right size for IP address.
				if (dwSize > (16 * sizeof (WCHAR)))
				{
					DPL(0, "Size of hostname component is unexpected (%u > %u)!",
						2, dwSize, (16 * sizeof (WCHAR)));
					SETTHROW_TESTRESULT(E_UNEXPECTED);
				} // end if (didn't use all of the buffer)

				if (dwDataType != DPNA_DATATYPE_STRING)
				{
					DPL(0, "Data type for hostname component is unexpected (%u != %u)!",
						2, dwDataType, DPNA_DATATYPE_STRING);
					SETTHROW_TESTRESULT(ERROR_NO_MATCH);
				} // end if (didn't use all of the buffer)


				// See if we found the IP address we want to use when
				// connecting.  If so, we're done.
				if (StringCmpAToU(wszIPCompareString, szIPString, TRUE))
				{
					DPL(1, "Found IP address \"%s\", using host's address %u (\"%s\").",
						3, szIPString, dwTemp, pszURL);
					break;
				} // end if (found IP address)


				pszURL += strlen(pszURL) + 1;
			} // end for (each host address)

			// If we didn't find the address to use, we have to bail.
			if (dwTemp >= dwNumAddresses)
			{
				DPL(0, "Couldn't find the correct host address to use!", 0);
				SETTHROW_TESTRESULT(ERROR_NOT_FOUND);
			} // end if (didn't find address)


			// At this point, the remote address object has the correct host
			// address we should use when connecting.



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Beginning multiple connection sequence");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			TESTSECTION_FOR(iCurrentIteration = 0; iCurrentIteration < pInput->iNumIterations; iCurrentIteration++)
			{
				iCurrentConnectingTester = (iCurrentIteration % (pTNecd->iNumMachines - 1)) + 1;
				

				TESTSECTION_IF(iCurrentConnectingTester == pTNecd->iTesterNum)
				{
					iCurrentConnectingInterface = (iCurrentIteration / (pTNecd->iNumMachines - 1)) % pInput->iNumObjects;

					DPL(0, "Starting connection iteration #%i, connecting with interface %i.",
						2, iCurrentIteration, iCurrentConnectingInterface);
					// Ignore error
					pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
											"Starting connection iteration #%i",
											1, iCurrentIteration);



					TESTSECTION_IF(connectorcontext.pahConnectCompleteEvents[iCurrentConnectingInterface] == NULL)
					{
						// This is the first time through the loop, or we're not
						// overlapping connects, so create the event for this time.
						CREATEEVENT_OR_THROW(connectorcontext.pahConnectCompleteEvents[iCurrentConnectingInterface],
											NULL, FALSE, FALSE, NULL);
					} // end if (don't have event)
					TESTSECTION_ELSE
					{
						// This is not the first time through the loop with
						// overlapping connects.  We must wait for the previous
						// connect to complete.

						DPL(0, "Waiting for object %i connect to %s.",
							2, iCurrentConnectingInterface,
							((pInput->fReject) ? "be rejected" : "complete successfully"));
						// Ignore error
						pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
												"Waiting for object %i connect to %s",
												2, iCurrentConnectingInterface,
												((pInput->fReject) ? "be rejected" : "complete successfully"));

						sr = pTNecd->pExecutor->WaitForEventOrCancel(&(connectorcontext.pahConnectCompleteEvents[iCurrentConnectingInterface]),
																	1,
																	NULL,
																	-1,
																	INFINITE,
																	NULL);
						HANDLE_WAIT_RESULT;


						// The connect completed, so close the object and prepare to
						// try again.

						DPL(0, "Closing object %i", 1, iCurrentConnectingInterface);
						// Ignore error
						pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
												"Closing object %i",
												1, iCurrentConnectingInterface);

						if (fClientServer)
						{
							tr = ((PWRAPDP8CLIENT) papvInterfaces[iCurrentConnectingInterface])->DP8C_Close(0);
						} // end if (client/server)
						else
						{
							tr = ((PWRAPDP8PEER) papvInterfaces[iCurrentConnectingInterface])->DP8P_Close(0);
						} // end else (peer-to-peer)

						if (tr != DPN_OK)
						{
							DPL(0, "Failed closing object %i!",
								1, iCurrentConnectingInterface);
							THROW_TESTRESULT;
						} // end if (couldn't close)


						if (pInput->fDontReuseObjects)
						{
							DPL(0, "Releasing object %i", 1, iCurrentConnectingInterface);
							// Ignore error
							pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
													"Releasing object %i",
													1, iCurrentConnectingInterface);
							if (fClientServer)
							{
								tr = ((PWRAPDP8CLIENT) papvInterfaces[iCurrentConnectingInterface])->Release();
							} // end if (client/server)
							else
							{
								tr = ((PWRAPDP8PEER) papvInterfaces[iCurrentConnectingInterface])->Release();
							} // end else (peer-to-peer)

							if (tr != DPN_OK)
							{
								DPL(0, "Failed releasing object %i!",
									1, iCurrentConnectingInterface);
								THROW_TESTRESULT;
							} // end if (couldn't release)


							DPL(0, "Recreating object %i", 1, iCurrentConnectingInterface);
							// Ignore error
							pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
													"Recreating object %i",
													1, iCurrentConnectingInterface);
							if (fClientServer)
							{
								tr = ((PWRAPDP8CLIENT) papvInterfaces[iCurrentConnectingInterface])->CoCreate();
							} // end if (client/server)
							else
							{
								tr = ((PWRAPDP8PEER) papvInterfaces[iCurrentConnectingInterface])->CoCreate();
							} // end else (peer-to-peer)

							if (tr != DPN_OK)
							{
								DPL(0, "Failed recreating object %i!",
									1, iCurrentConnectingInterface);
								THROW_TESTRESULT;
							} // end if (couldn't cocreate)
						} // end if (reusing objects)



						// Reinitialize the object.

						DPL(0, "Re-initializing object %i",
							1, iCurrentConnectingInterface);
						// Ignore error
						pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
												"Re-initializing object %i",
												1, iCurrentConnectingInterface);

						if (fClientServer)
						{
							tr = ((PWRAPDP8CLIENT) papvInterfaces[iCurrentConnectingInterface])->DP8C_Initialize(&connectorcontext,
																												ConnexManyConnectorDPNMessageHandler,
																												0);
						} // end if (client/server)
						else
						{
							tr = ((PWRAPDP8PEER) papvInterfaces[iCurrentConnectingInterface])->DP8P_Initialize(&connectorcontext,
																												ConnexManyConnectorDPNMessageHandler,
																												0);
						} // end else (peer-to-peer)

						if (tr != DPN_OK)
						{
							DPL(0, "Failed re-initializing object %i!",
								1, iCurrentConnectingInterface);
							THROW_TESTRESULT;
						} // end if (couldn't initialize)
					} // end else (do have event)
					TESTSECTION_ENDIF



					DPL(0, "Connecting object %i", 1, iCurrentConnectingInterface);
					// Ignore error
					pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
											"Connecting object %i",
											1, iCurrentConnectingInterface);
					
					// Use the interface index as the context.
					if (fClientServer)
					{
						tr = ((PWRAPDP8CLIENT) papvInterfaces[iCurrentConnectingInterface])->DP8C_Connect(&dpnad,
																										pDP8AddressRemote,
																										pDP8AddressLocal,
																										NULL,
																										NULL,
																										pvUserConnectData,
																										pInput->dwUserDataSize,
																										(PVOID) ((INT_PTR) iCurrentConnectingInterface),
																										&(connectorcontext.padpnhConnects[i % pInput->iNumObjects]),
																										0);
					} // end if (client/server)
					else
					{
						tr = ((PWRAPDP8PEER) papvInterfaces[iCurrentConnectingInterface])->DP8P_Connect(&dpnad,
																										pDP8AddressRemote,
																										pDP8AddressLocal,
																										NULL,
																										NULL,
																										pvUserConnectData,
																										pInput->dwUserDataSize,
																										NULL,
																										(PVOID) ((INT_PTR) iCurrentConnectingInterface),
																										&(connectorcontext.padpnhConnects[i % pInput->iNumObjects]),
																										0);
					} // end else (peer-to-peer)

					if (tr != (HRESULT) DPNSUCCESS_PENDING)
					{
						DPL(0, "Couldn't start connecting object %i!",
							1, iCurrentConnectingInterface);
						THROW_TESTRESULT;
					} // end if (couldn't connect)


					TESTSECTION_IF(! pInput->fOverlap)
					{
						DPL(0, "Waiting for object %i connect to %s.",
							2, iCurrentConnectingInterface,
							((pInput->fReject) ? "be rejected" : "complete successfully"));
						// Ignore error
						pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
												"Waiting for object %i connect to %s",
												2, iCurrentConnectingInterface,
												((pInput->fReject) ? "be rejected" : "complete successfully"));

						sr = pTNecd->pExecutor->WaitForEventOrCancel(&(connectorcontext.pahConnectCompleteEvents[iCurrentConnectingInterface]),
																	1,
																	NULL,
																	-1,
																	INFINITE,
																	NULL);
						HANDLE_WAIT_RESULT;


						CloseHandle(connectorcontext.pahConnectCompleteEvents[iCurrentConnectingInterface]);
						connectorcontext.pahConnectCompleteEvents[iCurrentConnectingInterface] = NULL;


						// The connect completed, so close the object and prepare to
						// try again.

						DPL(0, "Closing object %i",
							1, (i % pInput->iNumObjects));
						// Ignore error
						pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
												"Closing object %i",
												1, iCurrentConnectingInterface);

						if (fClientServer)
						{
							tr = ((PWRAPDP8CLIENT) papvInterfaces[iCurrentConnectingInterface])->DP8C_Close(0);
						} // end if (client/server)
						else
						{
							tr = ((PWRAPDP8PEER) papvInterfaces[iCurrentConnectingInterface])->DP8P_Close(0);
						} // end else (peer-to-peer)

						if (tr != DPN_OK)
						{
							DPL(0, "Failed closing object %i!",
								1, iCurrentConnectingInterface);
							THROW_TESTRESULT;
						} // end if (couldn't connect)



						// Reinitialize the object if that wasn't the last iteration
						// this interface will be doing.
						TESTSECTION_IF(iCurrentIteration < (pInput->iNumIterations - ((pTNecd->iNumMachines - 1) * pInput->iNumObjects)))
						{
							if (pInput->fDontReuseObjects)
							{
								DPL(0, "Releasing object %i", 1, iCurrentConnectingInterface);
								// Ignore error
								pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
														"Releasing object %i",
														1, iCurrentConnectingInterface);
								if (fClientServer)
								{
									tr = ((PWRAPDP8CLIENT) papvInterfaces[iCurrentConnectingInterface])->Release();
								} // end if (client/server)
								else
								{
									tr = ((PWRAPDP8PEER) papvInterfaces[iCurrentConnectingInterface])->Release();
								} // end else (peer-to-peer)

								if (tr != DPN_OK)
								{
									DPL(0, "Failed releasing object %i!",
										1, iCurrentConnectingInterface);
									THROW_TESTRESULT;
								} // end if (couldn't release)


								DPL(0, "Recreating object %i", 1, iCurrentConnectingInterface);
								// Ignore error
								pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
														"Recreating object %i",
														1, iCurrentConnectingInterface);
								if (fClientServer)
								{
									tr = ((PWRAPDP8CLIENT) papvInterfaces[iCurrentConnectingInterface])->CoCreate();
								} // end if (client/server)
								else
								{
									tr = ((PWRAPDP8PEER) papvInterfaces[iCurrentConnectingInterface])->CoCreate();
								} // end else (peer-to-peer)

								if (tr != DPN_OK)
								{
									DPL(0, "Failed recreating object %i!",
										1, iCurrentConnectingInterface);
									THROW_TESTRESULT;
								} // end if (couldn't cocreate)
							} // end if (reusing objects)


							DPL(0, "Re-initializing object %i",
								1, iCurrentConnectingInterface);
							// Ignore error
							pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
													"Re-initializing object %i",
													1, iCurrentConnectingInterface);

							if (fClientServer)
							{
								tr = ((PWRAPDP8CLIENT) papvInterfaces[iCurrentConnectingInterface])->DP8C_Initialize(&connectorcontext,
																													ConnexManyConnectorDPNMessageHandler,
																													0);
							} // end if (client/server)
							else
							{
								tr = ((PWRAPDP8PEER) papvInterfaces[iCurrentConnectingInterface])->DP8P_Initialize(&connectorcontext,
																													ConnexManyConnectorDPNMessageHandler,
																													0);
							} // end else (peer-to-peer)

							if (tr != DPN_OK)
							{
								DPL(0, "Failed re-initializing object %i!",
									1, iCurrentConnectingInterface);
								THROW_TESTRESULT;
							} // end if (couldn't connect)
						} // end if (not last iteration for object)
						TESTSECTION_ENDIF


						// Synchronize with everyone if this isn't the absolute last
						// iteration
						TESTSECTION_IF(iCurrentIteration < (pInput->iNumIterations - 1))
						{
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -
							TEST_SECTION("Letting other testers know iteration is complete");
							// - - - - - - - - - - - - - - - - - - - - - - - - - - -


							// Synchronize with everyone.
							sr = pTNecd->pExecutor->SyncWithTesters("Connection iteration complete",
																	NULL, 0, NULL,
																	0);
							HANDLE_SYNC_RESULT;
						} // end if (not absolute last iteration)
						TESTSECTION_ENDIF
					} // end if (not overlapping connects)
					TESTSECTION_ENDIF
				} // end if (it's current client's turn to connect)
				TESTSECTION_ELSE
				{
					TESTSECTION_IF(pInput->fOverlap)
					{
						DPL(1, "Not involved with connection iteration #%i (tester %i is the connector).",
							2, iCurrentIteration, iCurrentConnectingTester);

						// Ignore error
						pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
												"Not involved with connection iteration #%i",
												1, iCurrentIteration);
					} // end if (overlapping connects)
					TESTSECTION_ELSE
					{
						TESTSECTION_IF(iCurrentIteration < (pInput->iNumIterations - 1))
						{
							DPL(1, "Waiting for tester %i to perform connection iteration #%i.",
								2, iCurrentConnectingTester, iCurrentIteration);

							// Ignore error
							pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
													"Waiting for tester %i to perform connection iteration #%i",
													2, iCurrentConnectingTester,
													iCurrentIteration);


							// Synchronize with everyone.
							sr = pTNecd->pExecutor->SyncWithTesters("Connection iteration complete",
																	NULL, 0, NULL,
																	0);
							HANDLE_SYNC_RESULT;
						} // end if (not last iteration for object)
						TESTSECTION_ENDIF
					} // end else (not overlapping connects)
					TESTSECTION_ENDIF
				} // end else (it's not current client's turn to connect)
				TESTSECTION_ENDIF
			} // end for (each iteration)
			TESTSECTION_ENDFOR



			pDP8AddressLocal->Release();
			pDP8AddressLocal = NULL;

			pDP8AddressRemote->Release();
			pDP8AddressRemote = NULL;




			// If we overlapped connect attempts on the interfaces, wait for them
			// to complete now, and then close the interfaces.
			TESTSECTION_IF(pInput->fOverlap)
			{
				TESTSECTION_FOR(i = 0; i < iNumInterfaces; i++)
				{
					DPL(0, "Waiting for object %i connect to %s.",
						2, i,
						((pInput->fReject) ? "be rejected" : "complete successfully"));
					// Ignore error
					pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
											"Waiting for object %i connect to %s",
											2, i,
											((pInput->fReject) ? "be rejected" : "complete successfully"));

					sr = pTNecd->pExecutor->WaitForEventOrCancel(&(connectorcontext.pahConnectCompleteEvents[i]),
																1,
																NULL,
																-1,
																INFINITE,
																NULL);

					HANDLE_WAIT_RESULT;

					CloseHandle(connectorcontext.pahConnectCompleteEvents[i]);
					connectorcontext.pahConnectCompleteEvents[i] = NULL;


					// The connect completed, so close the object and prepare to
					// try again.

					DPL(0, "Closing object %i", 1, i);
					// Ignore error
					pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
											"Closing object %i",
											1, i);

					if (fClientServer)
					{
						tr = ((PWRAPDP8CLIENT) papvInterfaces[i])->DP8C_Close(0);
					} // end if (client/server)
					else
					{
						tr = ((PWRAPDP8PEER) papvInterfaces[i])->DP8P_Close(0);
					} // end else (peer-to-peer)

					if (tr != DPN_OK)
					{
						DPL(0, "Failed closing object %i!", 1, i);
						THROW_TESTRESULT;
					} // end if (couldn't connect)
				} // end for (each interface)
				TESTSECTION_ENDFOR
			} // end if (overlapping connects)
			TESTSECTION_ENDIF
		} // end else (not tester 0)
		TESTSECTION_ENDIF



		// Tester 0 needs to wait for his determination of connect indications and
		// their reply buffers being returned.
		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			TESTSECTION_IF(pInput->dwUserDataSize > 0)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for all connect reply buffers to be returned");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			} // end if (user data)
			TESTSECTION_ELSE
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Ensuring all connects were indicated");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			} // end else (no user data)
			TESTSECTION_ENDIF


			pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hConnectsDoneEvent),
													1, NULL, -1, INFINITE, NULL);
		} // end if (tester 0)
		TESTSECTION_ENDIF




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Making sure all testers have finished the connect(s) sequence");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->SyncWithTesters("All connects complete", NULL, 0,
												NULL, 0);
		HANDLE_SYNC_RESULT;



		// Tester 0 needs to close his interface
		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Closing hosted session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if (fClientServer)
			{
				tr = ((PWRAPDP8SERVER) papvInterfaces[0])->DP8S_Close(0);
			} // end if (client/server)
			else
			{
				tr = ((PWRAPDP8PEER) papvInterfaces[0])->DP8P_Close(0);
			} // end else (peer-to-peer)

			if (tr != DPN_OK)
			{
				DPL(0, "Failed closing hosting object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)
		} // end if (tester 0)
		TESTSECTION_ENDIF



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing interfaces");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Everybody needs to release there interfaces.
		TESTSECTION_FOR(i = 0; i < iNumInterfaces; i++)
		{
			if (fClientServer)
			{
				if (pTNecd->iTesterNum == 0)
				{
					DPL(0, "Releasing DirectPlay8Server object %i",
						1, i);
					// Ignore error
					pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
											"Releasing DirectPlay8Server object %i",
											1, i);

					tr = ((PWRAPDP8SERVER) papvInterfaces[i])->Release();
					if (tr != S_OK)
					{
						DPL(0, "Couldn't release DirectPlay8Server object!", 0);
						THROW_TESTRESULT;
					} // end if (couldn't release object)

					delete ((PWRAPDP8SERVER) papvInterfaces[i]);
				} // end if (tester 0)
				else
				{
					DPL(0, "Releasing DirectPlay8Client object %i",
						1, i);
					// Ignore error
					pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
											"Releasing DirectPlay8Client object %i",
											1, i);

					tr = ((PWRAPDP8CLIENT) papvInterfaces[i])->Release();
					if (tr != S_OK)
					{
						DPL(0, "Couldn't release DirectPlay8Client object!", 0);
						THROW_TESTRESULT;
					} // end if (couldn't release object)

					delete ((PWRAPDP8CLIENT) papvInterfaces[i]);
				} // end else (not tester 0)
			} // end if (client/server)
			else
			{
				DPL(0, "Releasing DirectPlay8Peer object %i",
					1, i);
				// Ignore error
				pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
										"Releasing DirectPlay8Peer object %i",
										1, i);

				tr = ((PWRAPDP8PEER) papvInterfaces[i])->Release();
				if (tr != S_OK)
				{
					DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't release object)

				delete ((PWRAPDP8PEER) papvInterfaces[i]);
			} // end else (peer-to-peer)
			papvInterfaces[i] = NULL;
		} // end for (each interface)
		TESTSECTION_ENDFOR

		LocalFree(papvInterfaces);
		papvInterfaces = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (papvInterfaces != NULL)
	{
		for(i = 0; i < iNumInterfaces; i++)
		{
			if (papvInterfaces[i] != NULL)
			{
				if (fClientServer)
				{
					if (pTNecd->iTesterNum == 0)
					{
						delete ((PWRAPDP8SERVER) papvInterfaces[i]);
					} // end if (tester 0)
					else
					{
						delete ((PWRAPDP8CLIENT) papvInterfaces[i]);
					} // end else (not tester 0)
				} // end if (client/server)
				else
				{
					delete ((PWRAPDP8PEER) papvInterfaces[i]);
				} // end else (peer-to-peer)
				papvInterfaces[i] = NULL;
			} // end if (actually have interface)
		} // end for (each interface)

		LocalFree(papvInterfaces);
		papvInterfaces = NULL;
	} // end if (have interface array)

	if (paDP8HostAddresses != NULL)
	{
		for(dwTemp = 0; dwTemp < dwNumAddresses; dwTemp++)
		{
			SAFE_RELEASE(paDP8HostAddresses[dwTemp]);
		} // end for (each address)

		LocalFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	} // end if (have array of host addresses)


	if (connectorcontext.pahConnectCompleteEvents != NULL)
	{
		for(i = 0; i < iNumInterfaces; i++)
		{
			SAFE_CLOSEHANDLE(connectorcontext.pahConnectCompleteEvents[i]);
		} // end for (each address)

		LocalFree(connectorcontext.pahConnectCompleteEvents);
		connectorcontext.pahConnectCompleteEvents = NULL;
	} // end if (have array of connect complete events)

	SAFE_LOCALFREE(connectorcontext.padpnhConnects);
	SAFE_LOCALFREE(pHostAddressesSyncData);
	SAFE_RELEASE(pDP8AddressLocal);
	SAFE_RELEASE(pDP8AddressRemote);
	SAFE_LOCALFREE(pvUserConnectData);
	SAFE_LOCALFREE(hostcontext.pvReplyData);

	return (sr);
} // ConnexExec_ManyAll
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexCanRun_ServerLoss()"
//==================================================================================
// ConnexCanRun_ServerLoss
//----------------------------------------------------------------------------------
//
// Description: Callback that checks a possible tester list to make sure the test
//				can be run correctly for the following test case(s):
//				2.7.4.1 - Server leave test
//				2.7.4.2 - Server drop test
//
// Arguments:
//	PTNCANRUNDATA pTNcrd	Pointer to parameter block with information on what
//							configuration to check.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ConnexCanRun_ServerLoss(PTNCANRUNDATA pTNcrd)
{
	HRESULT		hr;
	int			i;


	// The host tester needs to have IMTest installed in the drop case.
	if (strcmp(pTNcrd->pTest->m_pszID, "2.7.4.2") == 0)
	{
		pTNcrd->fCanRun = pTNcrd->apTesters[0]->m_fFaultSimIMTestAvailable;

		if (! pTNcrd->fCanRun)
		{
			DPL(1, "Tester 0 (%s) doesn't have IMTest fault simulator available.",
				1, pTNcrd->apTesters[0]->m_szComputerName);

			return (S_OK);
		} // end if (tester doesn't have IMTest)
	} // end if (drop test)


	// Make sure all subsequent testers can connect to the first.
	for(i = 1; i < pTNcrd->iNumMachines; i++)
	{
		if (pTNcrd->pMaster->AreOnSameMachine(pTNcrd->apTesters[i],
											pTNcrd->apTesters[0]))
		{
			DPL(1, "Tester 1 is on same machine as tester 0 (%s), assuming can reach via UDP.",
				1, pTNcrd->apTesters[0]->m_szComputerName);

			pTNcrd->fCanRun = TRUE;
		} // end if (are on same machine)
		else
		{
			hr = pTNcrd->pMaster->CanReachViaIP(pTNcrd->apTesters[i],
												pTNcrd->apTesters[0],
												DPLAY8_PORT,
												TNCR_IP_UDP,
												&(pTNcrd->fCanRun));
			if (hr != S_OK)
			{
				DPL(0, "Couldn't have tester %i (%s) try to reach tester 0 (%s) via UDP!",
					3, i, pTNcrd->apTesters[i]->m_szComputerName,
					pTNcrd->apTesters[0]->m_szComputerName);

				return (hr);
			} // end if (couldn't test reachability)

			if (! pTNcrd->fCanRun)
			{
				DPL(1, "Tester %i (%s) can't reach tester 0 (%s) via UDP.",
					3, i, pTNcrd->apTesters[i]->m_szComputerName,
					pTNcrd->apTesters[0]->m_szComputerName);

				return (S_OK);
			} // end if (tester couldn't reach tester 0)
		} // end else (not on same machine)
	} // end for (each tester after 0)

	return (S_OK);
} // ConnexCanRun_ServerLoss
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexExec_ServerLoss()"
//==================================================================================
// ConnexExec_ServerLoss
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.7.4.1 - Server leave test
//				2.7.4.2 - Server drop test
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
HRESULT ConnexExec_ServerLoss(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	HRESULT							temphr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PTOD_BBLDDP8_CS_CREATE_0		pBldDP8CSCreateOutput0 = NULL;
	PTOD_BBLDDP8_CS_CREATE_NOT0		pBldDP8CSCreateOutputNot0 = NULL;
	DWORD							dwSubOutputDataSize;
	CONNEXSERVERLOSSCONTEXT			context;
	DPN_APPLICATION_DESC			dpnad;
	PWRAPDP8SERVER					pDP8Server = NULL;
	PWRAPDP8CLIENT					pDP8Client = NULL;
	PTNFAULTSIM						pFaultSim = NULL;
	int								i;



	ZeroMemory(&context, sizeof (CONNEXSERVERLOSSCONTEXT));
	context.pTNecd = pTNecd;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating client/server session");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if (pTNecd->iTesterNum == 0)
			context.pfnDPNMessageHandler = ConnexServerLossServerDPNMessageHandler;
		else
			context.pfnDPNMessageHandler = ConnexServerLossClientDPNMessageHandler;

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
			DPL(0, "Couldn't execute sub test case BldSsn:BldCS:Create!", 0);
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
										(sizeof (TOD_BBLDDP8_CS_CREATE_0) + (pTNecd->iNumMachines * sizeof (DPNID))));

			pDP8Server = pBldDP8CSCreateOutput0->pDP8Server;
			pDP8Server->m_dwRefCount++; // we're using it during this function
			context.padpnidTesters = (DPNID*) (pBldDP8CSCreateOutput0 + 1);
		} // end if (tester 0)
		else
		{
			CHECKANDGET_SUBOUTPUTDATA(pSubResult,
										pBldDP8CSCreateOutputNot0,
										dwSubOutputDataSize,
										(sizeof (TOD_BBLDDP8_CS_CREATE_NOT0) + (pTNecd->iNumMachines * sizeof (DPNID))));

			pDP8Client = pBldDP8CSCreateOutputNot0->pDP8Client;
			pDP8Client->m_dwRefCount++; // we're using it during this function
			context.padpnidTesters = (DPNID*) (pBldDP8CSCreateOutputNot0 + 1);
		} // end else (not tester 0)



		if (pTNecd->iTesterNum == 0)
		{
			// Servers need to allocate an array for delete player indications.
			LOCALALLOC_OR_THROW(BOOL*, context.pafTesterIndicated,
								(pTNecd->iNumMachines * sizeof (BOOL)));
		} // end if (tester 0)


		context.hDisconnectEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (context.hDisconnectEvent == NULL)
		{
			sr = GetLastError();
			DPL(0, "Couldn't create disconnect event!", 0);
			THROW_SYSTEMRESULT;
		} // end if (couldn't create event)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for everyone to be ready for server disconnection");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->SyncWithTesters("Ready for disconnect", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;



		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			TESTSECTION_IF(pTNecd->pExecutor->IsCase("2.7.4.2"))
			{
				// We do NOT expect our local player to be removed.
				context.fSelfIndicate = FALSE;


				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Creating IMTest fault simulator");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->CreateNewFaultSim(&pFaultSim,
														TN_FAULTSIM_IMTEST,
														NULL,
														0);
				if (sr != S_OK)
				{
					DPL(0, "Couldn't create IMTest fault simulator!", 0);
					THROW_SYSTEMRESULT;
				} // end if (couldn't create fault simulator)



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Disconnecting sends and receives, will drop from session");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for connections to all other players to drop");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hDisconnectEvent,
															1,
															NULL,
															-1,
															INFINITE,
															NULL);
				HANDLE_WAIT_RESULT;





				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Restoring send and receive traffic");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Releasing IMTest fault simulator");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->ReleaseFaultSim(&pFaultSim);
				if (sr != S_OK)
				{
					DPL(0, "Couldn't release fault simulator %x!", 1, pFaultSim);
					THROW_SYSTEMRESULT;
				} // end if (couldn't release fault simulator)
			} // end if (drop case)
			TESTSECTION_ENDIF



			// We expect our own player to be removed.
			context.fSelfIndicate = TRUE;


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Disconnecting from session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Server->DP8S_Close(0);
			if (tr != DPN_OK)
			{
				DPL(0, "Closing failed!", 0);
				THROW_TESTRESULT;
			} // end if (close failed)


			// Make sure the nametable has been unwound properly.
			for(i = 0; i < pTNecd->iNumMachines; i++)
			{
				if (! context.pafTesterIndicated[i])
				{
					DPL(0, "Haven't received DELETE_PLAYER for tester %i!", 1, i);
					SETTHROW_TESTRESULT(E_FAIL);
				} // end if (tester hasn't been deleted)
			} // end for (each tester)
		} // end if (tester 0)
		TESTSECTION_ELSE
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connection to server to drop");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hDisconnectEvent,
														1,
														NULL,
														-1,
														INFINITE,
														NULL);
			HANDLE_WAIT_RESULT;
		} // end else (not tester 0)
		TESTSECTION_ENDIF



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Making sure everyone survived server disconnection");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->SyncWithTesters("Done", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;



		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Server object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Server->Release();
			if (tr != S_OK)
			{
				DPL(0, "Couldn't release DirectPlay8Server object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't release object)


			sr = g_pDP8ServersList->RemoveFirstReference(pDP8Server);
			if (sr != S_OK)
			{
				DPL(0, "Couldn't remove DirectPlay8Server wrapper object from list!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't remove object from list)


			pDP8Server->m_dwRefCount--; // release our reference
			if (pDP8Server->m_dwRefCount == 0)
			{
				DPL(7, "Deleting DirectPlay8Server wrapper object %x.",
					1, pDP8Server);
				delete (pDP8Server);
			} // end if (can delete the object)
			else
			{
				DPL(7, "Can't delete DirectPlay8Server wrapper object %x, it's refcount is %u.",
					2, pDP8Server,
					pDP8Server->m_dwRefCount);
			} // end else (can't delete the object)
			pDP8Server = NULL;
		} // end if (tester 0)
		TESTSECTION_ELSE
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Closing object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Client->DP8C_Close(0);
			if (tr != DPN_OK)
			{
				DPL(0, "Closing failed!", 0);
				THROW_TESTRESULT;
			} // end if (close failed)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Client object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Client->Release();
			if (tr != S_OK)
			{
				DPL(0, "Couldn't release DirectPlay8Client object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't release object)


			sr = g_pDP8ClientsList->RemoveFirstReference(pDP8Client);
			if (sr != S_OK)
			{
				DPL(0, "Couldn't remove DirectPlay8Client wrapper object from list!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't remove object from list)


			pDP8Client->m_dwRefCount--; // release our reference
			if (pDP8Client->m_dwRefCount == 0)
			{
				DPL(7, "Deleting DirectPlay8Client wrapper object %x.",
					1, pDP8Client);
				delete (pDP8Client);
			} // end if (can delete the object)
			else
			{
				DPL(7, "Can't delete DirectPlay8Client wrapper object %x, it's refcount is %u.",
					2, pDP8Client,
					pDP8Client->m_dwRefCount);
			} // end else (can't delete the object)
			pDP8Client = NULL;
		} // end else (not tester 0)
		TESTSECTION_ENDIF




		FINAL_SUCCESS;
	}
	END_TESTCASE



	if (pDP8Server != NULL)
	{
		temphr = pDP8Server->DP8S_Close(0);
		if (temphr != DPN_OK)
		{
			DPL(0, "Closing server interface failed!  0x%08x", 1, temphr);
			OVERWRITE_SR_IF_OK(temphr);
		} // end if (closing server interface failed)


		// Ignore error, it may not actually be on the list.
		g_pDP8ServersList->RemoveFirstReference(pDP8Server);


		pDP8Server->m_dwRefCount--;
		if (pDP8Server->m_dwRefCount == 0)
		{
			DPL(7, "Deleting wrapper object %x.", 1, pDP8Server);
			delete (pDP8Server);
		} // end if (can delete the object)
		else
		{
			DPL(0, "WARNING: Can't delete wrapper object %x, it's refcount is %u!?",
				2, pDP8Server, pDP8Server->m_dwRefCount);
		} // end else (can't delete the object)
		pDP8Server = NULL;
	} // end if (have server object)

	if (pDP8Client != NULL)
	{
		temphr = pDP8Client->DP8C_Close(0);
		if (temphr != DPN_OK)
		{
			DPL(0, "Closing client interface failed!  0x%08x", 1, temphr);
			OVERWRITE_SR_IF_OK(temphr);
		} // end if (closing client interface failed)


		// Ignore error, it may not actually be on the list.
		g_pDP8ClientsList->RemoveFirstReference(pDP8Client);


		pDP8Client->m_dwRefCount--;
		if (pDP8Client->m_dwRefCount == 0)
		{
			DPL(7, "Deleting DirectPlay8Client wrapper object %x.", 1, pDP8Client);
			delete (pDP8Client);
		} // end if (can delete the object)
		else
		{
			DPL(0, "WARNING: Can't delete DirectPlay8Client wrapper object %x, it's refcount is %u!?",
				2, pDP8Client, pDP8Client->m_dwRefCount);
		} // end else (can't delete the object)
		pDP8Client = NULL;
	} // end if (have client object)

	SAFE_CLOSEHANDLE(context.hDisconnectEvent);
	SAFE_LOCALFREE(context.pafTesterIndicated);

	return (sr);
} // ConnexExec_ServerLoss
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexExec_Reject()"
//==================================================================================
// ConnexExec_Reject
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.7.5.1 - Simple peer reject connections test
//				2.7.5.2 - Simple client/server reject connections test
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
HRESULT ConnexExec_Reject(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult			sr;
	CTNTestResult			tr;
	CONNEXREJECTCONTEXT		context;
	DPN_APPLICATION_DESC	dpnad;
	CTNSyncDataList			syncdata;
	PVOID					pvSyncData;
	DWORD					dwSyncDataSize;
	BOOL					fClientServer = FALSE;
	PWRAPDP8PEER			pDP8Peer = NULL;
	PWRAPDP8SERVER			pDP8Server = NULL;
	PWRAPDP8CLIENT			pDP8Client = NULL;
	PDIRECTPLAY8ADDRESS		pDP8AddressLocal = NULL;
	PDIRECTPLAY8ADDRESS		pDP8AddressRemote = NULL;
	DPNHANDLE				dpnhConnect = NULL;
	PHOSTADDRESSESSYNCDATA	pHostAddressesSyncData = NULL;
	PDIRECTPLAY8ADDRESS*	paDP8HostAddresses = NULL;
	DWORD					dwNumAddresses = 0;
	DWORD					dwTemp;
	DWORD					dwURLsBufferSize = 0;
	DWORD					dwRemainingBufferSize;
	char*					pszURL;
	DWORD					dwSize;
	DWORD					dwDataType;
	char					szIPString[16]; // max IP string size + NULL termination
	WCHAR					wszIPCompareString[16]; // max IP string size + NULL termination



	ZeroMemory(&context, sizeof (CONNEXREJECTCONTEXT));
	context.pTNecd = pTNecd;



	BEGIN_TESTCASE
	{
		TESTSECTION_IF(pTNecd->pExecutor->IsCase("2.7.5.2"))
		{
			fClientServer = TRUE;

			TESTSECTION_IF(pTNecd->iTesterNum == 0)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Creating DirectPlay8Server object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				pDP8Server = new (CWrapDP8Server);
				if (pDP8Server == NULL)
				{
					SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
				} // end if (couldn't allocate object)


				tr = pDP8Server->CoCreate();
				if (tr != DPN_OK)
				{
					DPL(0, "CoCreating DP8Server object failed!", 0);
					THROW_TESTRESULT;
				} // end if (function failed)
			} // end if (tester 0)
			TESTSECTION_ELSE
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Creating DirectPlay8Client object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				pDP8Client = new (CWrapDP8Client);
				if (pDP8Client == NULL)
				{
					SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
				} // end if (couldn't allocate object)


				tr = pDP8Client->CoCreate();
				if (tr != DPN_OK)
				{
					DPL(0, "CoCreating DP8Client object failed!", 0);
					THROW_TESTRESULT;
				} // end if (function failed)
			} // end else (not tester 0)
			TESTSECTION_ENDIF
		} // end if (client/server)
		TESTSECTION_ELSE
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating DirectPlay8Peer object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			pDP8Peer = new (CWrapDP8Peer);
			if (pDP8Peer == NULL)
			{
				SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
			} // end if (couldn't allocate object)


			tr = pDP8Peer->CoCreate();
			if (tr != DPN_OK)
			{
				DPL(0, "CoCreating DP8Peer object failed!", 0);
				THROW_TESTRESULT;
			} // end if (function failed)
		} // end else (peer-to-peer)
		TESTSECTION_ENDIF



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("CoCreating local address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) &pDP8AddressLocal);
		if (sr != S_OK)
		{
			DPL(0, "Couldn't CoCreate address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (couldn't CoCreate object)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting local address object's SP to TCP/IP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressLocal->SetSP(&CLSID_DP8SP_TCPIP);
		if (tr != S_OK)
		{
			DPL(0, "Couldn't set SP for address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		if (fClientServer)
		{
			if (pTNecd->iTesterNum == 0)
			{
				tr = pDP8Server->DP8S_Initialize(&context,
												ConnexRejectDPNMessageHandler,
												0);
			} // end if (tester 0)
			else
			{
				tr = pDP8Client->DP8C_Initialize(&context,
												ConnexRejectDPNMessageHandler,
												0);
			} // end else (not tester 0)
		} // end if (client/server)
		else
		{
			tr = pDP8Peer->DP8P_Initialize(&context,
											ConnexRejectDPNMessageHandler,
											0);
		} // end else (peer-to-peer)

		if (tr != S_OK)
		{
			DPL(0, "Couldn't initialize!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)



		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpnad.dwFlags = 0;
		if ((pTNecd->iTesterNum == 0) && (fClientServer))
			dpnad.dwFlags |= DPNSESSION_CLIENT_SERVER;
		//dpnad.guidInstance = GUID_NULL;
#pragma TODO(vanceo, "Get real GUID")
		dpnad.guidApplication = GUID_ENUMS_SIMPLE;
		//dpnad.dwMaxPlayers = 0;
		//dpnad.dwCurrentPlayers = 0;
		dpnad.pwszSessionName = (pTNecd->iTesterNum == 0) ? L"Session" : NULL;
		//dpnad.pwszPassword = NULL;
		//dpnad.pvReservedData = NULL;
		//dpnad.dwReservedDataSize = 0;
		//dpnad.pvApplicationReservedData = NULL;
		//dpnad.pvApplicationReservedData = 0;



		/*
		context.dwConnectDataSize = pInput->dwConnectDataSize;
		if (context.dwConnectDataSize > 0)
		{
			LOCALALLOC_OR_THROW(PVOID, context.pvConnectData, context.dwConnectDataSize);

			// Fill the memory with some pattern (different for connects vs.
			// responses).
			FillWithDWord(context.pvConnectData, context.dwConnectDataSize,
						((pTNecd->iTesterNum == 0) ? DATAPATTERN_REPLY : DATAPATTERN_CONNECT));
		} // end if (data should be sent)
		*/

		CREATEEVENT_OR_THROW(context.hConnectEvent,
							NULL, FALSE, FALSE, NULL);



		// Tester 0 should start hosting a session, tester 1 should wait until
		// the session is up.
		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Hosting session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if (fClientServer)
			{
				tr = pDP8Server->DP8S_Host(&dpnad, &pDP8AddressLocal, 1, NULL, NULL,
											NULL, 0);
			} // end if (client/server)
			else
			{
				tr = pDP8Peer->DP8P_Host(&dpnad, &pDP8AddressLocal, 1, NULL, NULL,
										NULL, 0);
			} // end else (peer-to-peer)

			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't start hosting session!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't host session)


			pDP8AddressLocal->Release();
			pDP8AddressLocal = NULL;




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Getting host addresses");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			if (fClientServer)
			{
				tr = pDP8Server->DP8S_GetLocalHostAddresses(NULL, &dwNumAddresses, 0);
			} // end if (client/server)
			else
			{
				tr = pDP8Peer->DP8P_GetLocalHostAddresses(NULL, &dwNumAddresses, 0);
			} // end else (peer-to-peer)

			if (tr != DPNERR_BUFFERTOOSMALL)
			{
				DPL(0, "Couldn't get number of host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't get host address)



			DPL(1, "We are hosting on %u addresses.", 1, dwNumAddresses); 


			LOCALALLOC_OR_THROW(PDIRECTPLAY8ADDRESS*, paDP8HostAddresses,
								dwNumAddresses * sizeof (PDIRECTPLAY8ADDRESS));


			if (fClientServer)
			{
				tr = pDP8Server->DP8S_GetLocalHostAddresses(paDP8HostAddresses,
															&dwNumAddresses,
															0);
			} // end if (client/server)
			else
			{
				tr = pDP8Peer->DP8P_GetLocalHostAddresses(paDP8HostAddresses,
														&dwNumAddresses,
														0);
			} // end else (peer-to-peer)

			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't get host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't get host address)


			// Get the total size of all the addresses.
			for(dwTemp = 0; dwTemp < dwNumAddresses; dwTemp++)
			{
				dwSize = 0;
				tr = paDP8HostAddresses[dwTemp]->GetURLA(NULL, &dwSize);
				if (tr != DPNERR_BUFFERTOOSMALL)
				{
					DPL(0, "Couldn't get host address %u's URL size!", 1, dwTemp);
					THROW_TESTRESULT;
				} // end if (not buffer too small)
				
				dwURLsBufferSize += dwSize;
			} // end for (each address)

			// Allocate a buffer for all the URLs.
			LOCALALLOC_OR_THROW(PHOSTADDRESSESSYNCDATA,
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
					DPL(0, "Couldn't get host address %u's URL!", 1, dwTemp);
					THROW_TESTRESULT;
				} // end if (failed)


				DPL(1, "Address %u: \"%s\"",
					2, dwTemp, pszURL);

				dwRemainingBufferSize -= dwSize;
				pszURL += dwSize;

				
				// We're done with this address object.
				paDP8HostAddresses[dwTemp]->Release();
				paDP8HostAddresses[dwTemp] = NULL;
			} // end for (each address)

			// We're done with the address array.
			LocalFree(paDP8HostAddresses);
			paDP8HostAddresses = NULL;


			// Make sure we used all of the buffer.
			if (dwRemainingBufferSize != 0)
			{
				DPL(0, "Didn't use all of or overran the allocated buffer (didn't use %i bytes)!",
					1, ((int) dwRemainingBufferSize));
				SETTHROW_TESTRESULT(E_FAIL);
			} // end if (didn't use all of the buffer)



			CREATEEVENT_OR_THROW(context.hReplyBufferReturnedEvent,
								NULL, FALSE, FALSE, NULL);



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Sending session addresses to connector");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->SyncWithTesters("Host addresses",
													pHostAddressesSyncData,
													(sizeof (HOSTADDRESSESSYNCDATA) + dwURLsBufferSize),
													NULL,
													0);
			HANDLE_SYNC_RESULT;


			LocalFree(pHostAddressesSyncData);
			pHostAddressesSyncData = NULL;




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connection indication");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;




			// Make sure the reply data was returned, if we had any.
			if (context.pvConnectData != NULL)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Waiting for reply buffer to be returned");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hReplyBufferReturnedEvent,
															1, NULL, -1, INFINITE,
															NULL);
				HANDLE_WAIT_RESULT;



				CloseHandle(context.hReplyBufferReturnedEvent);
				context.hReplyBufferReturnedEvent = NULL;
			} // end if (there's connect data)
		} // end if (tester 0)
		TESTSECTION_ELSE
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Receiving addresses of host");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->SyncWithTesters("Host addresses",
													NULL,
													0,
													&syncdata,
													0);
			HANDLE_SYNC_RESULT;

			CHECKANDGET_MINIMUM_SYNCDATA(syncdata, 0, pvSyncData, dwSyncDataSize,
										(sizeof (HOSTADDRESSESSYNCDATA) + (strlen(DPNA_HEADER_A) + 1)));


			dwNumAddresses = ((PHOSTADDRESSESSYNCDATA) pvSyncData)->dwNumAddresses;

			// Start with the first address
			pszURL = (char*) (((PBYTE) pvSyncData) + sizeof (HOSTADDRESSESSYNCDATA));



			DPL(1, "Tester 0 (host) has %u connectable addresses.",
				1, dwNumAddresses);



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("CoCreating remote (host's) address object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
								IID_IDirectPlay8Address, (LPVOID*) &pDP8AddressRemote);
			if (sr != S_OK)
			{
				DPL(0, "Couldn't CoCreate remote (host's) address object!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't CoCreate object)



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Finding appropriate address");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8AddressRemote->SetSP(&CLSID_DP8SP_TCPIP);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't set SP for remote (host's) address object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't set SP)


#pragma TODO(vanceo, "Support other SPs")
			// Figure out what IP we should use to connect to the host.
			sr = pTNecd->pExecutor->GetTestersIPForPort(0, DPLAY8_PORT, szIPString);
			if (sr != S_OK)
			{
				DPL(0, "Couldn't get tester 0's IP for port %u!",
					1, DPLAY8_PORT);
				THROW_SYSTEMRESULT;
			} // end if (couldn't get tester 0's IP)


			for(dwTemp = 0; dwTemp < dwNumAddresses; dwTemp++)
			{
				DPL(1, "Host address %u: %s", 2, dwTemp, pszURL);


				DPL(0, "Building remote address %u from string URL.", 1, dwTemp);
				// Ignore error
				pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
										"Building remote address %u from string URL.",
										1, dwTemp);


				tr = pDP8AddressRemote->BuildFromURLA(pszURL);
				if (tr != DPN_OK)
				{
					DPL(0, "Couldn't build host's address %u from URL (\"%s\")!",
						2, dwTemp, pszURL);
					THROW_TESTRESULT;
				} // end if (couldn't build from URL)



				DPL(0, "Getting remote address %u's hostname component.", 1, dwTemp);
				// Ignore error
				pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
										"Getting remote address %u's hostname component.",
										1, dwTemp);

				dwSize = 16 * sizeof (WCHAR);

				tr = pDP8AddressRemote->GetComponentByName(DPNA_KEY_HOSTNAME,
															wszIPCompareString,
															&dwSize,
															&dwDataType);
				if (tr != DPN_OK)
				{
					DPL(0, "Couldn't get host's address %u hostname component!",
						1, dwTemp);
					THROW_TESTRESULT;
				} // end if (couldn't get component by name)

				// Make sure it's the right size for IP address.
				if (dwSize > (16 * sizeof (WCHAR)))
				{
					DPL(0, "Size of hostname component is unexpected (%u > %u)!",
						2, dwSize, (16 * sizeof (WCHAR)));
					SETTHROW_TESTRESULT(E_UNEXPECTED);
				} // end if (didn't use all of the buffer)

				if (dwDataType != DPNA_DATATYPE_STRING)
				{
					DPL(0, "Data type for hostname component is unexpected (%u != %u)!",
						2, dwDataType, DPNA_DATATYPE_STRING);
					SETTHROW_TESTRESULT(ERROR_NO_MATCH);
				} // end if (didn't use all of the buffer)


				// See if we found the IP address we want to use when
				// connecting.  If so, we're done.
				if (StringCmpAToU(wszIPCompareString, szIPString, TRUE))
				{
					DPL(1, "Found IP address \"%s\", using host's address %u (\"%s\").",
						3, szIPString, dwTemp, pszURL);
					break;
				} // end if (found IP address)


				pszURL += strlen(pszURL) + 1;
			} // end for (each host address)

			// If we didn't find the address to use, we have to bail.
			if (dwTemp >= dwNumAddresses)
			{
				DPL(0, "Couldn't find the correct host address to use!", 0);
				SETTHROW_TESTRESULT(ERROR_NOT_FOUND);
			} // end if (didn't find address)


			// At this point, the remote address object has the correct host
			// address we should use when connecting.

			if (fClientServer)
			{
				tr = pDP8Client->DP8C_Connect(&dpnad,
											pDP8AddressRemote,
											pDP8AddressLocal,
											NULL,
											NULL,
											context.pvConnectData,
											context.dwConnectDataSize,
											context.hConnectEvent,
											&dpnhConnect,
											0);
			} // end if (client/server)
			else
			{
				tr = pDP8Peer->DP8P_Connect(&dpnad,
											pDP8AddressRemote,
											pDP8AddressLocal,
											NULL,
											NULL,
											context.pvConnectData,
											context.dwConnectDataSize,
											NULL,
											context.hConnectEvent,
											&dpnhConnect,
											0);
			} // end else (peer-to-peer)

			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)



			pDP8AddressLocal->Release();
			pDP8AddressLocal = NULL;

			pDP8AddressRemote->Release();
			pDP8AddressRemote = NULL;



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connection to complete");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;
		} // end else (not tester 0)
		TESTSECTION_ENDIF


		CloseHandle(context.hConnectEvent);
		context.hConnectEvent = NULL;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Waiting for other tester to complete connect sequence");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->SyncWithTesters("Connect done", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;



	
		TESTSECTION_IF(fClientServer)
		{
			TESTSECTION_IF(pTNecd->iTesterNum == 0)
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Closing object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Server->DP8S_Close(0);
				if (tr != DPN_OK)
				{
					DPL(0, "Closing failed!", 0);
					THROW_TESTRESULT;
				} // end if (close failed)



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Releasing DirectPlay8Server object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Server->Release();
				if (tr != S_OK)
				{
					DPL(0, "Couldn't release DirectPlay8Server object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't release object)

				delete (pDP8Server);
				pDP8Server = NULL;
			} // end if (tester 0)
			TESTSECTION_ELSE
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Closing object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Client->DP8C_Close(0);
				if (tr != DPN_OK)
				{
					DPL(0, "Closing failed!", 0);
					THROW_TESTRESULT;
				} // end if (close failed)



				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				TEST_SECTION("Releasing DirectPlay8Client object");
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

				tr = pDP8Client->Release();
				if (tr != S_OK)
				{
					DPL(0, "Couldn't release DirectPlay8Client object!", 0);
					THROW_TESTRESULT;
				} // end if (couldn't release object)

				delete (pDP8Client);
				pDP8Client = NULL;
			} // end else (not tester 0)
			TESTSECTION_ENDIF
		} // end if (client/server)
		TESTSECTION_ELSE
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Closing object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Peer->DP8P_Close(0);
			if (tr != DPN_OK)
			{
				DPL(0, "Closing failed!", 0);
				THROW_TESTRESULT;
			} // end if (close failed)



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Peer object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Peer->Release();
			if (tr != S_OK)
			{
				DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't release object)

			delete (pDP8Peer);
			pDP8Peer = NULL;
		} // end else (peer-to-peer)
		TESTSECTION_ENDIF



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

		LocalFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	} // end if (have array of host addresses)

	SAFE_LOCALFREE(context.pvConnectData);
	SAFE_LOCALFREE(pHostAddressesSyncData);
	SAFE_RELEASE(pDP8AddressLocal);
	SAFE_RELEASE(pDP8AddressRemote);
	SAFE_CLOSEHANDLE(context.hConnectEvent);
	SAFE_CLOSEHANDLE(context.hReplyBufferReturnedEvent);


	return (sr);
} // ConnexExec_Reject
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexExec_NotAllowed()"
//==================================================================================
// ConnexExec_NotAllowed
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.7.6.1 - All not-allowed connection types test
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
HRESULT ConnexExec_NotAllowed(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult						sr;
	CTNTestResult						tr;
	CONNEXNOTALLOWEDCONTEXT				context;
	DPN_APPLICATION_DESC				dpnadCS;
	DPN_APPLICATION_DESC				dpnadPeer;
	CTNSyncDataList						syncdata;
	PVOID								pvSyncData;
	DWORD								dwSyncDataSize;
	PWRAPDP8PEER						pDP8PeerHost = NULL;
	PWRAPDP8PEER						pDP8PeerClient = NULL;
	PWRAPDP8SERVER						pDP8Server = NULL;
	PWRAPDP8CLIENT						pDP8Client = NULL;
	PDIRECTPLAY8ADDRESS					pDP8AddressLocal = NULL;
	PDIRECTPLAY8ADDRESS					pDP8AddressRemoteCS = NULL;
	PDIRECTPLAY8ADDRESS					pDP8AddressRemotePeer = NULL;
	DPNHANDLE							dpnhConnect = NULL;
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
	char								szIPString[16]; // max IP string size + NULL termination
	WCHAR								wszIPCompareString[16]; // max IP string size + NULL termination



	ZeroMemory(&context, sizeof (CONNEXNOTALLOWEDCONTEXT));
	context.pTNecd = pTNecd;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Client = new (CWrapDP8Client);
		if (pDP8Client == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)


		tr = pDP8Client->CoCreate();
		if (tr != DPN_OK)
		{
			DPL(0, "CoCreating DP8Client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (function failed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Peer object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8PeerClient = new (CWrapDP8Peer);
		if (pDP8PeerClient == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)


		tr = pDP8PeerClient->CoCreate();
		if (tr != DPN_OK)
		{
			DPL(0, "CoCreating DP8Peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (function failed)



		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating DirectPlay8Server object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			pDP8Server = new (CWrapDP8Server);
			if (pDP8Server == NULL)
			{
				SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
			} // end if (couldn't allocate object)


			tr = pDP8Server->CoCreate();
			if (tr != DPN_OK)
			{
				DPL(0, "CoCreating DP8Server object failed!", 0);
				THROW_TESTRESULT;
			} // end if (function failed)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Creating secondary DirectPlay8Peer object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			pDP8PeerHost = new (CWrapDP8Peer);
			if (pDP8PeerClient == NULL)
			{
				SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
			} // end if (couldn't allocate object)


			tr = pDP8PeerHost->CoCreate();
			if (tr != DPN_OK)
			{
				DPL(0, "CoCreating DP8Peer object failed!", 0);
				THROW_TESTRESULT;
			} // end if (function failed)
		} // end if (tester 0)
		TESTSECTION_ENDIF





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("CoCreating local address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) &pDP8AddressLocal);
		if (sr != S_OK)
		{
			DPL(0, "Couldn't CoCreate address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (couldn't CoCreate object)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting local address object's SP to TCP/IP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressLocal->SetSP(&CLSID_DP8SP_TCPIP);
		if (tr != S_OK)
		{
			DPL(0, "Couldn't set SP for address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Initializing objects");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_Initialize(&context,
										ConnexNotAllowedDPNMessageHandler,
										0);
		if (tr != S_OK)
		{
			DPL(0, "Couldn't initialize client interface!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)


		tr = pDP8PeerClient->DP8P_Initialize(&context,
											ConnexNotAllowedDPNMessageHandler,
											0);
		if (tr != S_OK)
		{
			DPL(0, "Couldn't initialize peer client interface!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)


		if (pTNecd->iTesterNum == 0)
		{
			tr = pDP8Server->DP8S_Initialize(&context,
											ConnexNotAllowedDPNMessageHandler,
											0);
			if (tr != S_OK)
			{
				DPL(0, "Couldn't initialize server interface!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't initialize)


			tr = pDP8PeerHost->DP8P_Initialize(&context,
												ConnexNotAllowedDPNMessageHandler,
												0);
			if (tr != S_OK)
			{
				DPL(0, "Couldn't initialize peer host interface!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't initialize)
		} // end if (tester 0)





		ZeroMemory(&dpnadCS, sizeof (DPN_APPLICATION_DESC));
		dpnadCS.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpnadCS.dwFlags = 0;
		dpnadCS.dwFlags = DPNSESSION_CLIENT_SERVER;
		//dpnadCS.guidInstance = GUID_NULL;
#pragma TODO(vanceo, "Get real GUID")
		dpnadCS.guidApplication = GUID_ENUMS_SIMPLE;
		//dpnadCS.dwMaxPlayers = 0;
		//dpnadCS.dwCurrentPlayers = 0;
		dpnadCS.pwszSessionName = (pTNecd->iTesterNum == 0) ? L"Session" : NULL;
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
#pragma TODO(vanceo, "Get real GUID")
		dpnadPeer.guidApplication = GUID_ENUMS_SIMPLE;
		//dpnadPeer.dwMaxPlayers = 0;
		//dpnadPeer.dwCurrentPlayers = 0;
		dpnadPeer.pwszSessionName = (pTNecd->iTesterNum == 0) ? L"Session" : NULL;
		//dpnadPeer.pwszPassword = NULL;
		//dpnadPeer.pvReservedData = NULL;
		//dpnadPeer.dwReservedDataSize = 0;
		//dpnadPeer.pvApplicationReservedData = NULL;
		//dpnadPeer.pvApplicationReservedData = 0;




		// Tester 0 should start hosting the sessions, tester 1 should wait until
		// they are up.
		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Hosting session client/server session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Server->DP8S_Host(&dpnadCS, &pDP8AddressLocal, 1, NULL, NULL,
										NULL, 0);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't start hosting client/server session!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't host session)


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Hosting session peer-to-peer session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerHost->DP8P_Host(&dpnadPeer, &pDP8AddressLocal, 1, NULL, NULL,
										NULL, 0);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't start hosting peer session!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't host session)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Getting number of host addresses for client/server");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Server->DP8S_GetLocalHostAddresses(NULL, &dwNumAddressesCS, 0);
			if (tr != DPNERR_BUFFERTOOSMALL)
			{
				DPL(0, "Couldn't get number of client/server host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't get local host addresses)



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Getting number of host addresses for peer-to-peer");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerHost->DP8P_GetLocalHostAddresses(NULL, &dwNumAddressesPeer, 0);
			if (tr != DPNERR_BUFFERTOOSMALL)
			{
				DPL(0, "Couldn't get number of peer-to-peer host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't get local host addresses)




			DPL(1, "We are hosting on %u C/S addresses and %u peer addresses.",
				2, dwNumAddressesCS, dwNumAddressesPeer); 


			LOCALALLOC_OR_THROW(PDIRECTPLAY8ADDRESS*, paDP8HostAddresses,
								(dwNumAddressesCS + dwNumAddressesPeer) * sizeof (PDIRECTPLAY8ADDRESS));



			tr = pDP8Server->DP8S_GetLocalHostAddresses(paDP8HostAddresses,
														&dwNumAddressesCS,
														0);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't get c/s host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't local host addresses)


			tr = pDP8PeerHost->DP8P_GetLocalHostAddresses(paDP8HostAddresses + dwNumAddressesCS,
														&dwNumAddressesPeer,
														0);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't get peer host addresses!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't local host addresses)



			// Connect the client objects.
			context.fConnectsAllowed = TRUE;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting local c/s client to local session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			context.hrExpectedResult = DPN_OK;

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
				DPL(0, "Couldn't connect!", 0);
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
				DPL(0, "Couldn't connect!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)


			pDP8AddressLocal->Release();
			pDP8AddressLocal = NULL;


			// No more connects should ever be allowed or expected.
			context.fConnectsAllowed = FALSE;



			// Get the total size of all the addresses.
			for(dwTemp = 0; dwTemp < (dwNumAddressesCS + dwNumAddressesPeer); dwTemp++)
			{
				dwSize = 0;
				tr = paDP8HostAddresses[dwTemp]->GetURLA(NULL, &dwSize);
				if (tr != DPNERR_BUFFERTOOSMALL)
				{
					DPL(0, "Couldn't get host address %u's URL size!", 1, dwTemp);
					THROW_TESTRESULT;
				} // end if (not buffer too small)
				
				dwURLsBufferSize += dwSize;
			} // end for (each address)

			// Allocate a buffer for all the URLs.
			LOCALALLOC_OR_THROW(PHOSTADDRESSESCSANDPEERSYNCDATA,
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
					DPL(0, "Couldn't get host address %u's URL!", 1, dwTemp);
					THROW_TESTRESULT;
				} // end if (failed)


				DPL(1, "Address %u: \"%s\"",
					2, dwTemp, pszURL);

				dwRemainingBufferSize -= dwSize;
				pszURL += dwSize;

				
				// We're done with this address object.
				paDP8HostAddresses[dwTemp]->Release();
				paDP8HostAddresses[dwTemp] = NULL;
			} // end for (each address)

			// We're done with the address array.
			LocalFree(paDP8HostAddresses);
			paDP8HostAddresses = NULL;


			// Make sure we used all of the buffer.
			if (dwRemainingBufferSize != 0)
			{
				DPL(0, "Didn't use all of or overran the allocated buffer (didn't use %i bytes)!",
					1, ((int) dwRemainingBufferSize));
				SETTHROW_TESTRESULT(E_FAIL);
			} // end if (didn't use all of the buffer)





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Sending session addresses to connector");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->SyncWithTesters("Host addresses",
													pHostAddressesCSAndPeerSyncData,
													(sizeof (HOSTADDRESSESCSANDPEERSYNCDATA) + dwURLsBufferSize),
													NULL,
													0);
			HANDLE_SYNC_RESULT;


			LocalFree(pHostAddressesCSAndPeerSyncData);
			pHostAddressesCSAndPeerSyncData = NULL;



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connector to finish connecting to wrong interface type");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end if (tester 0)
		TESTSECTION_ELSE
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Receiving addresses of host");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->SyncWithTesters("Host addresses",
													NULL,
													0,
													&syncdata,
													0);
			HANDLE_SYNC_RESULT;

			CHECKANDGET_MINIMUM_SYNCDATA(syncdata, 0, pvSyncData, dwSyncDataSize,
										(sizeof (HOSTADDRESSESCSANDPEERSYNCDATA) + ((strlen(DPNA_HEADER_A) + 1) * 2)));


			dwNumAddressesCS = ((PHOSTADDRESSESCSANDPEERSYNCDATA) pvSyncData)->dwNumAddressesCS;
			dwNumAddressesPeer = ((PHOSTADDRESSESCSANDPEERSYNCDATA) pvSyncData)->dwNumAddressesPeer;

			// Start with the first address
			pszURL = (char*) (((PBYTE) pvSyncData) + sizeof (HOSTADDRESSESCSANDPEERSYNCDATA));



			DPL(1, "Tester 0 (host) has %u c/s and %u peer connectable addresses.",
				2, dwNumAddressesCS, dwNumAddressesPeer);



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("CoCreating remote (host's) client/server address object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
								IID_IDirectPlay8Address, (LPVOID*) &pDP8AddressRemoteCS);
			if (sr != S_OK)
			{
				DPL(0, "Couldn't CoCreate remote (host's) client/server address object!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't CoCreate object)



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("CoCreating remote (host's) peer-to-peer address object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
								IID_IDirectPlay8Address, (LPVOID*) &pDP8AddressRemotePeer);
			if (sr != S_OK)
			{
				DPL(0, "Couldn't CoCreate remote (host's) peer-to-peer address object!", 0);
				THROW_SYSTEMRESULT;
			} // end if (couldn't CoCreate object)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Finding appropriate c/s address");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma TODO(vanceo, "Support other SPs")
			// Figure out what IP we should use to connect to the host.
			sr = pTNecd->pExecutor->GetTestersIPForPort(0, DPLAY8_PORT, szIPString);
			if (sr != S_OK)
			{
				DPL(0, "Couldn't get tester 0's IP for port %u!",
					1, DPLAY8_PORT);
				THROW_SYSTEMRESULT;
			} // end if (couldn't get tester 0's IP)


			for(dwTemp = 0; dwTemp < dwNumAddressesCS; dwTemp++)
			{
				DPL(1, "Host c/s address %u: %s", 2, dwTemp, pszURL);


				DPL(0, "Building remote c/s address %u from string URL.", 1, dwTemp);
				// Ignore error
				pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
										"Building remote c/s address %u from string URL.",
										1, dwTemp);


				tr = pDP8AddressRemoteCS->BuildFromURLA(pszURL);
				if (tr != DPN_OK)
				{
					DPL(0, "Couldn't build host's c/s address %u from URL (\"%s\")!",
						2, dwTemp, pszURL);
					THROW_TESTRESULT;
				} // end if (couldn't build from URL)



				DPL(0, "Getting remote c/s address %u's hostname component.", 1, dwTemp);
				// Ignore error
				pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
										"Getting remote c/s address %u's hostname component.",
										1, dwTemp);

				dwSize = 16 * sizeof (WCHAR);

				tr = pDP8AddressRemoteCS->GetComponentByName(DPNA_KEY_HOSTNAME,
															wszIPCompareString,
															&dwSize,
															&dwDataType);
				if (tr != DPN_OK)
				{
					DPL(0, "Couldn't get host's c/s address %u hostname component!",
						1, dwTemp);
					THROW_TESTRESULT;
				} // end if (couldn't get component by name)

				// Make sure it's the right size for IP address.
				if (dwSize > (16 * sizeof (WCHAR)))
				{
					DPL(0, "Size of hostname component is unexpected (%u > %u)!",
						2, dwSize, (16 * sizeof (WCHAR)));
					SETTHROW_TESTRESULT(E_UNEXPECTED);
				} // end if (didn't use all of the buffer)

				if (dwDataType != DPNA_DATATYPE_STRING)
				{
					DPL(0, "Data type for hostname component is unexpected (%u != %u)!",
						2, dwDataType, DPNA_DATATYPE_STRING);
					SETTHROW_TESTRESULT(ERROR_NO_MATCH);
				} // end if (didn't use all of the buffer)


				// See if we found the IP address we want to use when
				// connecting.  If so, we're done.
				if (StringCmpAToU(wszIPCompareString, szIPString, TRUE))
				{
					DPL(1, "Found IP address \"%s\", using host's c/s address %u (\"%s\").",
						3, szIPString, dwTemp, pszURL);
					break;
				} // end if (found IP address)


				pszURL += strlen(pszURL) + 1;
			} // end for (each host address)

			// If we didn't find the address to use, we have to bail.
			if (dwTemp >= dwNumAddressesCS)
			{
				DPL(0, "Couldn't find the correct host c/s address to use!", 0);
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
				DPL(1, "Host peer address %u: %s", 2, dwTemp, pszURL);


				DPL(0, "Building remote peer address %u from string URL.", 1, dwTemp);
				// Ignore error
				pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
										"Building remote peer address %u from string URL.",
										1, dwTemp);


				tr = pDP8AddressRemotePeer->BuildFromURLA(pszURL);
				if (tr != DPN_OK)
				{
					DPL(0, "Couldn't build host's peer address %u from URL (\"%s\")!",
						2, dwTemp, pszURL);
					THROW_TESTRESULT;
				} // end if (couldn't build from URL)



				DPL(0, "Getting remote peer address %u's hostname component.", 1, dwTemp);
				// Ignore error
				pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
										"Getting remote peer address %u's hostname component.",
										1, dwTemp);

				dwSize = 16 * sizeof (WCHAR);

				tr = pDP8AddressRemotePeer->GetComponentByName(DPNA_KEY_HOSTNAME,
																wszIPCompareString,
																&dwSize,
																&dwDataType);
				if (tr != DPN_OK)
				{
					DPL(0, "Couldn't get host's peer address %u hostname component!",
						1, dwTemp);
					THROW_TESTRESULT;
				} // end if (couldn't get component by name)

				// Make sure it's the right size for IP address.
				if (dwSize > (16 * sizeof (WCHAR)))
				{
					DPL(0, "Size of hostname component is unexpected (%u > %u)!",
						2, dwSize, (16 * sizeof (WCHAR)));
					SETTHROW_TESTRESULT(E_UNEXPECTED);
				} // end if (didn't use all of the buffer)

				if (dwDataType != DPNA_DATATYPE_STRING)
				{
					DPL(0, "Data type for hostname component is unexpected (%u != %u)!",
						2, dwDataType, DPNA_DATATYPE_STRING);
					SETTHROW_TESTRESULT(ERROR_NO_MATCH);
				} // end if (didn't use all of the buffer)


				// See if we found the IP address we want to use when
				// connecting.  If so, we're done.
				if (StringCmpAToU(wszIPCompareString, szIPString, TRUE))
				{
					DPL(1, "Found IP address \"%s\", using host's peer address %u (\"%s\").",
						3, szIPString, dwTemp, pszURL);
					break;
				} // end if (found IP address)


				pszURL += strlen(pszURL) + 1;
			} // end for (each host address)

			// If we didn't find the address to use, we have to bail.
			if (dwTemp >= dwNumAddressesPeer)
			{
				DPL(0, "Couldn't find the correct host peer address to use!", 0);
				SETTHROW_TESTRESULT(ERROR_NOT_FOUND);
			} // end if (didn't find address)



			CREATEEVENT_OR_THROW(context.hConnectCompletedEvent,
								NULL, FALSE, FALSE, NULL);



			// At this point, the remote address objects have the correct host
			// addresses we should use when connecting.


			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to peer-to-peer session using client/server interface");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			context.hrExpectedResult = DPNERR_INVALIDINTERFACE;

			tr = pDP8Client->DP8C_Connect(&dpnadPeer,
										pDP8AddressRemotePeer,
										pDP8AddressLocal,
										NULL,
										NULL,
										NULL,
										0,
										context.hConnectCompletedEvent,
										&dpnhConnect,
										0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with INVALIDINTERFACE");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to client/server session using peer-to-peer interface");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerClient->DP8P_Connect(&dpnadCS,
											pDP8AddressRemoteCS,
											pDP8AddressLocal,
											NULL,
											NULL,
											NULL,
											0,
											NULL,
											context.hConnectCompletedEvent,
											&dpnhConnect,
											0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with INVALIDINTERFACE");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to client/server session with wrong instance GUID");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadCS.guidInstance = GUID_UNKNOWN;

			context.hrExpectedResult = DPNERR_INVALIDINSTANCE;

			tr = pDP8Client->DP8C_Connect(&dpnadCS,
										pDP8AddressRemoteCS,
										pDP8AddressLocal,
										NULL,
										NULL,
										NULL,
										0,
										context.hConnectCompletedEvent,
										&dpnhConnect,
										0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with INVALIDINSTANCE");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to peer-to-peer session with wrong instance GUID");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadPeer.guidInstance = GUID_UNKNOWN;

			tr = pDP8PeerClient->DP8P_Connect(&dpnadPeer,
											pDP8AddressRemotePeer,
											pDP8AddressLocal,
											NULL,
											NULL,
											NULL,
											0,
											NULL,
											context.hConnectCompletedEvent,
											&dpnhConnect,
											0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with INVALIDINSTANCE");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to client/server session with wrong application GUID");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadCS.guidInstance = GUID_NULL;
			dpnadCS.guidApplication = GUID_UNKNOWN;

			context.hrExpectedResult = DPNERR_INVALIDAPPLICATION;

			tr = pDP8Client->DP8C_Connect(&dpnadCS,
										pDP8AddressRemoteCS,
										pDP8AddressLocal,
										NULL,
										NULL,
										NULL,
										0,
										context.hConnectCompletedEvent,
										&dpnhConnect,
										0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with INVALIDAPPLICATION");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to peer-to-peer session with wrong application GUID");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadPeer.guidInstance = GUID_NULL;
			dpnadPeer.guidApplication = GUID_UNKNOWN;

			tr = pDP8PeerClient->DP8P_Connect(&dpnadPeer,
											pDP8AddressRemotePeer,
											pDP8AddressLocal,
											NULL,
											NULL,
											NULL,
											0,
											NULL,
											context.hConnectCompletedEvent,
											&dpnhConnect,
											0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with INVALIDAPPLICATION");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Notifying host that we're ready to test max players");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end else (not tester 0)
		TESTSECTION_ENDIF


		sr = pTNecd->pExecutor->SyncWithTesters("Interfaces and GUIDs done", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;



		// Tester 0 should cap the number of players allowed in the sessions, tester
		// 1 should wait until that's ready.
		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Limiting number of players allowed in client/server session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadCS.dwMaxPlayers = 2;

			tr = pDP8Server->DP8S_SetApplicationDesc(&dpnadCS, 0);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't set application desc!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't set application desc)



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Limiting number of players allowed in peer-to-peer session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadPeer.dwMaxPlayers = 2;

			tr = pDP8PeerHost->DP8P_SetApplicationDesc(&dpnadPeer, 0);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't set application desc!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't set application desc)



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Telling connector to start trying to join maxxed out sessions");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end if (tester 0)
		TESTSECTION_ELSE
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for session player limits to be set");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end else (not tester 0)
		TESTSECTION_ENDIF


		sr = pTNecd->pExecutor->SyncWithTesters("Player limits set", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;



		// Tester 0 should wait until tester 1 has tried connecting to both sessions.
		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for other tester to try connecting to maxxed out sessions");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end if (tester 0)
		TESTSECTION_ELSE
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to maxxed out client/server session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadCS.guidApplication = GUID_NULL;

			context.hrExpectedResult = DPNERR_SESSIONFULL;

			tr = pDP8Client->DP8C_Connect(&dpnadCS,
										pDP8AddressRemoteCS,
										pDP8AddressLocal,
										NULL,
										NULL,
										NULL,
										0,
										context.hConnectCompletedEvent,
										&dpnhConnect,
										0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with SESSIONFULL");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;







			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to maxxed out peer-to-peer session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadPeer.guidApplication = GUID_NULL;

			tr = pDP8PeerClient->DP8P_Connect(&dpnadPeer,
											pDP8AddressRemotePeer,
											pDP8AddressLocal,
											NULL,
											NULL,
											NULL,
											0,
											NULL,
											context.hConnectCompletedEvent,
											&dpnhConnect,
											0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with SESSIONFULL");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Notifying host that we're ready to test invalid password");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end else (not tester 0)
		TESTSECTION_ENDIF


		sr = pTNecd->pExecutor->SyncWithTesters("Player limit done", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;



		// Tester 0 should set a password for the sessions, tester 1 should wait
		// until that's ready.
		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Removing player limit and setting password for client/server session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadCS.dwFlags = DPNSESSION_CLIENT_SERVER | DPNSESSION_REQUIREPASSWORD;
			dpnadCS.dwMaxPlayers = 0;
			dpnadCS.pwszPassword = L"I bet you can't get this password!";

			tr = pDP8Server->DP8S_SetApplicationDesc(&dpnadCS, 0);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't set application desc!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't set application desc)



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Removing player limit and setting password for peer-to-peer session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadPeer.dwFlags = DPNSESSION_REQUIREPASSWORD;
			dpnadPeer.dwMaxPlayers = 0;
			dpnadPeer.pwszPassword = L"I bet you can't get this password!";

			tr = pDP8PeerHost->DP8P_SetApplicationDesc(&dpnadPeer, 0);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't set application desc!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't set application desc)



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Telling connector to start trying to join password protected sessions");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end if (tester 0)
		TESTSECTION_ELSE
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for password to be set");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end else (not tester 0)
		TESTSECTION_ENDIF


		sr = pTNecd->pExecutor->SyncWithTesters("Password set", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;



		// Tester 0 should wait until tester 1 has tried connecting to both sessions.
		TESTSECTION_IF(pTNecd->iTesterNum == 0)
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
				DPL(0, "Getting c/s client address failed!", 0);
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
				DPL(0, "Getting peer client address failed!", 0);
				THROW_TESTRESULT;
			} // end if (getting peer address failed)




			dwSize = 0;
			dwURLsBufferSize = 0;

			tr = pDP8AddressRemoteCS->GetURLA(NULL, &dwSize);
			if (tr != DPNERR_BUFFERTOOSMALL)
			{
				DPL(0, "Couldn't get c/s client address' URL size!", 0);
				THROW_TESTRESULT;
			} // end if (not buffer too small)
			
			dwURLsBufferSize += dwSize;

			dwSize = 0;
			tr = pDP8AddressRemotePeer->GetURLA(NULL, &dwSize);
			if (tr != DPNERR_BUFFERTOOSMALL)
			{
				DPL(0, "Couldn't get peer client address' URL size!", 0);
				THROW_TESTRESULT;
			} // end if (not buffer too small)
			
			dwURLsBufferSize += dwSize;




			// Allocate a buffer for both the URLs.
			pszURL = NULL;
			LOCALALLOC_OR_THROW(char*, pszURL, dwURLsBufferSize);




			dwSize = dwURLsBufferSize;
			tr = pDP8AddressRemoteCS->GetURLA(pszURL, &dwSize);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't get c/s client address' URL!", 0);

				LocalFree(pszURL);
				pszURL = NULL;

				THROW_TESTRESULT;
			} // end if (failed)


			DPL(1, "C/S client address: \"%s\"", 1, pszURL);

			// We're done with this address object.
			pDP8AddressRemoteCS->Release();
			pDP8AddressRemoteCS = NULL;


			dwTemp = dwURLsBufferSize - dwSize;
			tr = pDP8AddressRemotePeer->GetURLA(pszURL + dwSize, &dwTemp);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't get peer client address' URL!", 0);

				LocalFree(pszURL);
				pszURL = NULL;

				THROW_TESTRESULT;
			} // end if (failed)

			DPL(1, "Peer client address: \"%s\"", 1, pszURL + dwSize);


			// We're done with this address object.
			pDP8AddressRemotePeer->Release();
			pDP8AddressRemotePeer = NULL;




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for other tester finish connecting to password protected sessions and sending clients' addresses");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->SyncWithTesters("Password set & non-host URLs",
													pszURL, dwURLsBufferSize,
													NULL, 0);
			HANDLE_SYNC_RESULT;




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for other tester to finish connecting to non-hosts");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end if (tester 0)
		TESTSECTION_ELSE
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to password protected client/server session without a password");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			context.hrExpectedResult = DPNERR_INVALIDPASSWORD;

			tr = pDP8Client->DP8C_Connect(&dpnadCS,
										pDP8AddressRemoteCS,
										pDP8AddressLocal,
										NULL,
										NULL,
										NULL,
										0,
										context.hConnectCompletedEvent,
										&dpnhConnect,
										0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with INVALIDPASSWORD");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to password protected peer-to-peer session without a password");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerClient->DP8P_Connect(&dpnadPeer,
											pDP8AddressRemotePeer,
											pDP8AddressLocal,
											NULL,
											NULL,
											NULL,
											0,
											NULL,
											context.hConnectCompletedEvent,
											&dpnhConnect,
											0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with INVALIDPASSWORD");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to password protected client/server session with wrong password");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadCS.pwszPassword = L"This is most definitely not the password.";

			tr = pDP8Client->DP8C_Connect(&dpnadCS,
										pDP8AddressRemoteCS,
										pDP8AddressLocal,
										NULL,
										NULL,
										NULL,
										0,
										context.hConnectCompletedEvent,
										&dpnhConnect,
										0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with INVALIDPASSWORD");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;





			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to password protected peer-to-peer session with wrong password");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			dpnadPeer.pwszPassword = L"This is most definitely not the password.";

			tr = pDP8PeerClient->DP8P_Connect(&dpnadPeer,
											pDP8AddressRemotePeer,
											pDP8AddressLocal,
											NULL,
											NULL,
											NULL,
											0,
											NULL,
											context.hConnectCompletedEvent,
											&dpnhConnect,
											0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with INVALIDPASSWORD");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Retrieving non-host addresses");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->SyncWithTesters("Password set & non-host URLs",
													NULL, 0, &syncdata, 0);
			HANDLE_SYNC_RESULT;



			CHECKANDGET_MINIMUM_SYNCDATA(syncdata, 0, pszURL, dwSyncDataSize,
										((strlen(DPNA_HEADER_A) + 1) * 2));



			tr = pDP8AddressRemoteCS->BuildFromURLA(pszURL);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't build host's C/S address from URL (\"%s\")!",
					1, pszURL);
				THROW_TESTRESULT;
			} // end if (couldn't build from URL)

			pszURL += strlen(pszURL) + 1;


			tr = pDP8AddressRemotePeer->BuildFromURLA(pszURL);
			if (tr != DPN_OK)
			{
				DPL(0, "Couldn't build host's peer address from URL (\"%s\")!",
					1, pszURL);
				THROW_TESTRESULT;
			} // end if (couldn't build from URL)






			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to client in client/server session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			context.hrExpectedResult = DPNERR_NORESPONSE;

			tr = pDP8Client->DP8C_Connect(&dpnadCS,
										pDP8AddressRemoteCS,
										pDP8AddressLocal,
										NULL,
										NULL,
										NULL,
										0,
										context.hConnectCompletedEvent,
										&dpnhConnect,
										0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with NORESPONSE");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Connecting to peer client in peer-to-peer session");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			context.hrExpectedResult = DPNERR_NOTHOST;

			tr = pDP8PeerClient->DP8P_Connect(&dpnadPeer,
											pDP8AddressRemotePeer,
											pDP8AddressLocal,
											NULL,
											NULL,
											NULL,
											0,
											NULL,
											context.hConnectCompletedEvent,
											&dpnhConnect,
											0);
			if (tr != (HRESULT) DPNSUCCESS_PENDING)
			{
				DPL(0, "Couldn't start connecting!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't connect)




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Waiting for connect to complete with NOTHOST");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			sr = pTNecd->pExecutor->WaitForEventOrCancel(&context.hConnectCompletedEvent, 1,
														NULL, -1, INFINITE, NULL);
			HANDLE_WAIT_RESULT;




			CloseHandle(context.hConnectCompletedEvent);
			context.hConnectCompletedEvent = NULL;




			pDP8AddressLocal->Release();
			pDP8AddressLocal = NULL;

			pDP8AddressRemoteCS->Release();
			pDP8AddressRemoteCS = NULL;

			pDP8AddressRemotePeer->Release();
			pDP8AddressRemotePeer = NULL;




			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Notifying host that we're done");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		} // end else (not tester 0)
		TESTSECTION_ENDIF



		sr = pTNecd->pExecutor->SyncWithTesters("All connects done", NULL, 0, NULL, 0);
		HANDLE_SYNC_RESULT;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Closing client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->DP8C_Close(0);
		if (tr != DPN_OK)
		{
			DPL(0, "Closing failed!", 0);
			THROW_TESTRESULT;
		} // end if (close failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Client object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Client->Release();
		if (tr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Client object!", 0);
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
			DPL(0, "Closing failed!", 0);
			THROW_TESTRESULT;
		} // end if (close failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Peer object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8PeerClient->Release();
		if (tr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerClient);
		pDP8PeerClient = NULL;




		TESTSECTION_IF(pTNecd->iTesterNum == 0)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Closing server object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Server->DP8S_Close(0);
			if (tr != DPN_OK)
			{
				DPL(0, "Closing failed!", 0);
				THROW_TESTRESULT;
			} // end if (close failed)



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Server object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8Server->Release();
			if (tr != S_OK)
			{
				DPL(0, "Couldn't release DirectPlay8Server object!", 0);
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
				DPL(0, "Closing failed!", 0);
				THROW_TESTRESULT;
			} // end if (close failed)



			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			TEST_SECTION("Releasing DirectPlay8Peer object");
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			tr = pDP8PeerHost->Release();
			if (tr != S_OK)
			{
				DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
				THROW_TESTRESULT;
			} // end if (couldn't release object)

			delete (pDP8PeerHost);
			pDP8PeerHost = NULL;
		} // end if (tester 0)
		TESTSECTION_ENDIF




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

		LocalFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	} // end if (have array of host addresses)

	SAFE_LOCALFREE(pHostAddressesCSAndPeerSyncData);
	SAFE_RELEASE(pDP8AddressLocal);
	SAFE_RELEASE(pDP8AddressRemoteCS);
	SAFE_RELEASE(pDP8AddressRemotePeer);
	SAFE_CLOSEHANDLE(context.hConnectCompletedEvent);


	return (sr);
} // ConnexExec_NotAllowed
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexLeaveWhileJoinPeerDPNMessageHandler()"
//==================================================================================
// ConnexLeaveWhileJoinPeerDPNMessageHandler
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
HRESULT ConnexLeaveWhileJoinPeerDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT							hr = DPN_OK;
	PCONNEXLEAVEWHILEJOINCONTEXT	pContext = (PCONNEXLEAVEWHILEJOINCONTEXT) pvContext;
	DWORD							dwSize = 0;
	PDPN_PLAYER_INFO				pdpnpi = NULL;
	int								iTesterNum;
	BOOL							fFound;


	switch (dwMsgType)
	{
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

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;
			
			// If we're rejoining, make sure the nametable we're rebuilding is
			// correct.
			if ((pContext->pTNecd->iTesterNum == pContext->iCurrentJoiningPlayer) &&
				(pContext->fSelfIndicate))
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

				pdpnpi = (PDPN_PLAYER_INFO) MemAlloc(LPTR, dwSize);
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

					pContext->padpnidTesters[pContext->pTNecd->iTesterNum] = pCreatePlayerMsg->dpnidPlayer;


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


					// If we ensured that the dropping player couldn't connect, but here
					// he is, we've got a problem.
					if (iTesterNum == pContext->iCurrentDroppingPlayer)
					{
						if (pContext->fEnsureDisconnectBeforeJoin)
						{
							DPL(0, "Got CREATE_PLAYER for dropping player (tester %i, player ID %u/%x) even he should have been disconnected before we joined!  DEBUGBREAK()-ing.",
								3, iTesterNum, pCreatePlayerMsg->dpnidPlayer,
								pCreatePlayerMsg->dpnidPlayer);
							//Ignore error
							pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
															"Got CREATE_PLAYER for dropping player (tester %i, player ID %u/%x) even he should have been disconnected before we joined!",
															3, iTesterNum,
															pCreatePlayerMsg->dpnidPlayer,
															pCreatePlayerMsg->dpnidPlayer);

							DEBUGBREAK();

							hr = E_FAIL;
							goto DONE;
						} // end if (ensured disconnect)
						else
						{
							DPL(0, "Got CREATE_PLAYER for dropping player (tester %i, player ID %u/%x), we'd better get a DESTROY_PLAYER soon.",
								3, iTesterNum, pCreatePlayerMsg->dpnidPlayer,
								pCreatePlayerMsg->dpnidPlayer);
							//Ignore error
							pContext->pTNecd->pExecutor->Log(TNLF_IMPORTANT | TNLF_PREFIX_TESTUNIQUEID,
															"Got CREATE_PLAYER for dropping player (tester %i, player ID %u/%x), we'd better get a DESTROY_PLAYER soon.",
															3, iTesterNum,
															pCreatePlayerMsg->dpnidPlayer,
															pCreatePlayerMsg->dpnidPlayer);
						} // end else (didn't ensure disconnect)
					} // end if (dropping player
				} // end else (it's some other player)


				// Associate the context with this player.
				//pCreatePlayerMsg->pvPlayerContext = &(pContext->paiReliableMsgsReceivedFromTesters[iTesterNum]);

				//DPL(8, "Player context is now %x.", 1, pCreatePlayerMsg->pvPlayerContext);


				pContext->pafTesterIndicated[iTesterNum] = TRUE;


				// See if that's the last person we're waiting on to connect.
				for(iTesterNum = 0; iTesterNum < pContext->pTNecd->iNumMachines; iTesterNum++)
				{
					// Don't worry about the dropping tester.
					if (iTesterNum == pContext->iCurrentDroppingPlayer)
						continue;


					if (! pContext->pafTesterIndicated[iTesterNum])
					{
						DPL(4, "Still expecting tester %i to connect, not notifying main thread.",
							1, iTesterNum);
						// No sense in looking further, just return OK.
						goto DONE;
					} // end if (tester hasn't connected yet)
				} // end for (each tester who's already joined)


				// If we got here, it means all the testers that should have connected
				// (including ourselves) did.  Notify the main thread.

				DPL(2, "All testers expected to connect actually have, notifying main thread.", 0);

				if (! SetEvent(pContext->hDroppedAndJoinedEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set dropped and joined event (%x)!  DEBUGBREAK()-ing.  0x%08x",
						2, pContext->hDroppedAndJoinedEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					goto DONE;
				} // end if (couldn't set event)
			} // end if (we're the rejoining player)
			else
			{
				if (pContext->padpnidTesters[pContext->iCurrentJoiningPlayer] != 0)
				{
					DPL(0, "Got duplicate CREATE_PLAYER for rejoining player (tester %i, prev player ID = %u/%x, new player ID = %u/%x)!  DEBUGBREAK()-ing.",
						5, pContext->iCurrentJoiningPlayer,
						pContext->padpnidTesters[pContext->iCurrentJoiningPlayer],
						pContext->padpnidTesters[pContext->iCurrentJoiningPlayer],
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
				} // end if (already have the rejoining player's ID)


				DPL(1, "Joining tester %i has entered the session with player ID %u/%x.",
					3, pContext->iCurrentJoiningPlayer, pCreatePlayerMsg->dpnidPlayer,
					pCreatePlayerMsg->dpnidPlayer);


				// Associate the context with this player.
				//pCreatePlayerMsg->pvPlayerContext = &(pContext->paiReliableMsgsReceivedFromTesters[pContext->iCurrentJoiningPlayer]);
	
				//DPL(8, "Player context is now %x.", 1, pCreatePlayerMsg->pvPlayerContext);


				// Store the player ID.
				pContext->padpnidTesters[pContext->iCurrentJoiningPlayer] = pCreatePlayerMsg->dpnidPlayer;

				if (pContext->pTNecd->iTesterNum == pContext->iCurrentDroppingPlayer)
				{
					DPL(0, "Joining tester (%i) sneaked in with player ID %u/%x, we had better get a corresponding DELETE_PLAYER shortly!",
						3, pContext->iCurrentDroppingPlayer, pCreatePlayerMsg->dpnidPlayer,
						pCreatePlayerMsg->dpnidPlayer);

					// Note we special case mark him as _not_ indicated.  This is
					// not protected by locks or anything, so I think it may be
					// possible for this to go wrong.
					pContext->pafTesterIndicated[pContext->iCurrentJoiningPlayer] = FALSE;

					// And we're done here, so return DPN_OK.
					goto DONE;
				} // end if (we're dropping player)


				// If we're here, we're not a dropping or joining player, and the
				// joining player just joined.  Mark him as indicated, and see if
				// the dropping player has dropped.
				pContext->pafTesterIndicated[pContext->iCurrentJoiningPlayer] = TRUE;
				

				// If the dropping player hasn't left yet, we need to wait.
				if (! pContext->pafTesterIndicated[pContext->iCurrentDroppingPlayer])
				{
					DPL(2, "Dropping tester %i (player ID %u/%x) has not left yet, not notifying main thread.",
						3, pContext->iCurrentDroppingPlayer,
						pContext->padpnidTesters[pContext->iCurrentDroppingPlayer],
						pContext->padpnidTesters[pContext->iCurrentDroppingPlayer]);

					// Just return DPN_OK.
					goto DONE;
				} // end if (dropping player hasn't yet)


				// If we got here, the dropping player has gone already, so notify
				// the main thread.

				DPL(2, "Testers expected to leave and join have done so, notifying main thread.", 0);

				if (! SetEvent(pContext->hDroppedAndJoinedEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set dropped and joined event (%x)!  DEBUGBREAK()-ing.  0x%08x",
						2, pContext->hDroppedAndJoinedEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					goto DONE;
				} // end if (couldn't set event)
			} // end else (we're not the joining player)
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			if (pContext->hDroppedAndJoinedEvent == NULL)
			{
				DPL(0, "WARNING: Ignoring DPN_MSGID_DESTROY_PLAYER message because no DroppedAndJoined event, this had better be because of a test failure!", 0);
			} // end if (no dropped and joined event)
			else
			{
				// If we're a dropping player, then we should be seeing all the
				// players be removed from the session.  If not, just the dropping
				// players should be going away.
				if (pContext->pTNecd->iTesterNum == pContext->iCurrentDroppingPlayer)
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
							if (iTesterNum == pContext->iCurrentJoiningPlayer)
							{
								DPL(0, "Joining tester (%i) sneaked in with player ID %u/%x, this is the DELETE_PLAYER.",
									3, iTesterNum, pDestroyPlayerMsg->dpnidPlayer,
									pDestroyPlayerMsg->dpnidPlayer);
								
								// If that player wasn't indicated, something's
								// wrong.
								if (! pContext->pafTesterIndicated[iTesterNum])
								{
									DPL(0, "Got unexpected DESTROY_PLAYER for rejoining tester %i (player ID %u/%x)!  DEBUGBREAK()-ing.",
										3, iTesterNum, pDestroyPlayerMsg->dpnidPlayer,
										pDestroyPlayerMsg->dpnidPlayer);

									//Ignore error
									pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
																	"Got unexpected DESTROY_PLAYER for rejoining tester %i!",
																	1, iTesterNum);

									DEBUGBREAK();

									hr = E_FAIL;
									goto DONE;
								} // end if (player already connected)

								// Mark him as disconnected now.
								pContext->pafTesterIndicated[iTesterNum] = FALSE;
							} // end if (it's the joining player)
							else
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
							} // end else (not the joining player)

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
				} // end if (current leaving player)
				else
				{
					// Make sure the leaving player is the right one.
					if (pDestroyPlayerMsg->dpnidPlayer != pContext->padpnidTesters[pContext->iCurrentDroppingPlayer])
					{
						DPL(0, "Got DESTROY_PLAYER for unexpected player ID %u/%x (leaving player (tester %i) is ID %u/%x)!  DEBUGBREAK()-ing",
							5, pDestroyPlayerMsg->dpnidPlayer,
							pDestroyPlayerMsg->dpnidPlayer,
							pContext->iCurrentDroppingPlayer,
							pContext->padpnidTesters[pContext->iCurrentDroppingPlayer],
							pContext->padpnidTesters[pContext->iCurrentDroppingPlayer]);

						//Ignore error
						pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
														"Got DESTROY_PLAYER for unexpected player ID %u/%x!",
														2, pDestroyPlayerMsg->dpnidPlayer,
														pDestroyPlayerMsg->dpnidPlayer);

						DEBUGBREAK();

						hr = E_FAIL;
						goto DONE;
					} // end if (leaving player isn't expected)

					
					if (pContext->pafTesterIndicated[pContext->iCurrentDroppingPlayer])
					{
						DPL(0, "Got duplicate DESTROY_PLAYER for leaving tester %i, player ID %u/%x!  DEBUGBREAK()-ing.",
							3, pContext->iCurrentDroppingPlayer,
							pContext->padpnidTesters[pContext->iCurrentDroppingPlayer],
							pContext->padpnidTesters[pContext->iCurrentDroppingPlayer]);

						//Ignore error
						pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
														"Got duplicate DESTROY_PLAYER for leaving tester (ID %u/%x)!",
														2, pDestroyPlayerMsg->dpnidPlayer,
														pDestroyPlayerMsg->dpnidPlayer);

						DEBUGBREAK();

						hr = E_FAIL;
						goto DONE;
					} // end if (tester already disconnected)


					DPL(1, "Dropping tester %i (player ID %u/%x) has left the session.",
						3, pContext->iCurrentDroppingPlayer, pDestroyPlayerMsg->dpnidPlayer,
						pDestroyPlayerMsg->dpnidPlayer);


					// Mark dropping player as disconnected now.
					pContext->pafTesterIndicated[pContext->iCurrentDroppingPlayer] = TRUE;


					// If the joining player hasn't done so, just return DPN_OK;
					if (! pContext->pafTesterIndicated[pContext->iCurrentJoiningPlayer])
					{
						DPL(2, "Joining tester %i has not done so yet, not notifying main thread.",
							1, pContext->iCurrentJoiningPlayer);
						goto DONE;
					} // end if (joining player hasn't done so)
				} // end else (not current leaving player)



				// If we got here, then we're good to go.
				DPL(2, "All testers expected to disconnect actually have, notifying main thread.", 0);

				if (! SetEvent(pContext->hDroppedAndJoinedEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set dropped and joined event (%x)!  DEBUGBREAK()-ing.  0x%08x",
						2, pContext->hDroppedAndJoinedEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;
				} // end if (couldn't set dropped and joined event)
			} // end else (dropped and joined event)
		  break;

		case DPN_MSGID_HOST_MIGRATE:
			DPL(0, "WARNING: Ignoring HOST_MIGRATE, this had better be because of a test failure!", 0);
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
			DPL(0, "    pAddressPlayer = 0x%08x", 1, pIndicateConnectMsg->pAddressPlayer);
			DPL(0, "    pAddressDevice = 0x%08x", 1, pIndicateConnectMsg->pAddressDevice);
			*/

#pragma TODO(vanceo, "Validate this")
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
} // ConnexLeaveWhileJoinPeerDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexLeaveWhileJoinServerDPNMessageHandler()"
//==================================================================================
// ConnexLeaveWhileJoinServerDPNMessageHandler
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
HRESULT ConnexLeaveWhileJoinServerDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT							hr = DPN_OK;
	PCONNEXLEAVEWHILEJOINCONTEXT	pContext = (PCONNEXLEAVEWHILEJOINCONTEXT) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;
			
			if (pContext->padpnidTesters[pContext->iCurrentJoiningPlayer] != 0)
			{
				DPL(0, "Got duplicate CREATE_PLAYER for rejoining player (tester %i, prev player ID = %u/%x, new player ID = %u/%x)!  DEBUGBREAK()-ing.",
					5, pContext->iCurrentJoiningPlayer,
					pContext->padpnidTesters[pContext->iCurrentJoiningPlayer],
					pContext->padpnidTesters[pContext->iCurrentJoiningPlayer],
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
			} // end if (already have the rejoining player's ID)


			DPL(1, "Joining tester %i has entered the session with player ID %u/%x.",
				3, pContext->iCurrentJoiningPlayer, pCreatePlayerMsg->dpnidPlayer,
				pCreatePlayerMsg->dpnidPlayer);


			// Associate the context with this player.
			//pCreatePlayerMsg->pvPlayerContext = &(pContext->paiReliableMsgsReceivedFromTesters[pContext->iCurrentJoiningPlayer]);

			//DPL(8, "Player context is now %x.", 1, pCreatePlayerMsg->pvPlayerContext);


			// Store the player ID.
			pContext->padpnidTesters[pContext->iCurrentJoiningPlayer] = pCreatePlayerMsg->dpnidPlayer;

			pContext->pafTesterIndicated[pContext->iCurrentJoiningPlayer] = TRUE;


			// If the dropping player hasn't left yet, we need to wait.
			if (! pContext->pafTesterIndicated[pContext->iCurrentDroppingPlayer])
			{
				DPL(2, "Dropping tester %i (player ID %u/%x) has not left yet, not notifying main thread.",
					3, pContext->iCurrentDroppingPlayer,
					pContext->padpnidTesters[pContext->iCurrentDroppingPlayer],
					pContext->padpnidTesters[pContext->iCurrentDroppingPlayer]);

				// Just return DPN_OK.
				goto DONE;
			} // end if (dropping player hasn't yet)


			// If we got here, the dropping player has gone already, so notify
			// the main thread.

			DPL(2, "Testers expected to leave and join have done so, notifying main thread.", 0);

			if (! SetEvent(pContext->hDroppedAndJoinedEvent))
			{
				hr = GetLastError();

				DPL(0, "Couldn't set dropped and joined event (%x)!  DEBUGBREAK()-ing.  0x%08x",
					2, pContext->hDroppedAndJoinedEvent, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				goto DONE;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			if (pContext->hDroppedAndJoinedEvent == NULL)
			{
				DPL(0, "WARNING: Ignoring DPN_MSGID_DESTROY_PLAYER message because no DroppedAndJoined event, this had better be because of a test failure!", 0);
			} // end if (no dropped and joined event)
			else
			{
				// Make sure the leaving player is the right one.
				if (pDestroyPlayerMsg->dpnidPlayer != pContext->padpnidTesters[pContext->iCurrentDroppingPlayer])
				{
					DPL(0, "Got DESTROY_PLAYER for unexpected player ID %u/%x (leaving player (tester %i) is ID %u/%x)!  DEBUGBREAK()-ing",
						5, pDestroyPlayerMsg->dpnidPlayer,
						pDestroyPlayerMsg->dpnidPlayer,
						pContext->iCurrentDroppingPlayer,
						pContext->padpnidTesters[pContext->iCurrentDroppingPlayer],
						pContext->padpnidTesters[pContext->iCurrentDroppingPlayer]);

					//Ignore error
					pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
													"Got DESTROY_PLAYER for unexpected player ID %u/%x!",
													2, pDestroyPlayerMsg->dpnidPlayer,
													pDestroyPlayerMsg->dpnidPlayer);

					DEBUGBREAK();

					hr = E_FAIL;
					goto DONE;
				} // end if (leaving player isn't expected)

				
				if (pContext->pafTesterIndicated[pContext->iCurrentDroppingPlayer])
				{
					DPL(0, "Got duplicate DESTROY_PLAYER for leaving tester %i, player ID %u/%x!  DEBUGBREAK()-ing.",
						3, pContext->iCurrentDroppingPlayer,
						pContext->padpnidTesters[pContext->iCurrentDroppingPlayer],
						pContext->padpnidTesters[pContext->iCurrentDroppingPlayer]);

					//Ignore error
					pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
													"Got duplicate DESTROY_PLAYER for leaving tester (ID %u/%x)!",
													2, pDestroyPlayerMsg->dpnidPlayer,
													pDestroyPlayerMsg->dpnidPlayer);

					DEBUGBREAK();

					hr = E_FAIL;
					goto DONE;
				} // end if (tester already disconnected)


				DPL(1, "Dropping tester %i (player ID %u/%x) has left the session.",
					3, pContext->iCurrentDroppingPlayer, pDestroyPlayerMsg->dpnidPlayer,
					pDestroyPlayerMsg->dpnidPlayer);


				// Mark dropping player as disconnected now.
				pContext->pafTesterIndicated[pContext->iCurrentDroppingPlayer] = TRUE;


				// If the joining player hasn't done so, just return DPN_OK;
				if (! pContext->pafTesterIndicated[pContext->iCurrentJoiningPlayer])
				{
					DPL(2, "Joining tester %i has not done so yet, not notifying main thread.",
						1, pContext->iCurrentJoiningPlayer);
					goto DONE;
				} // end if (joining player hasn't done so)



				// If we got here, then we're good to go.
				DPL(2, "Testers expected to joing and disconnect actually have, notifying main thread.", 0);

				if (! SetEvent(pContext->hDroppedAndJoinedEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set dropped and joined event (%x)!  DEBUGBREAK()-ing.  0x%08x",
						2, pContext->hDroppedAndJoinedEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;
				} // end if (couldn't set dropped and joined event)
			} // end else (dropped and joined event)
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
			DPL(0, "    pAddressPlayer = 0x%08x", 1, pIndicateConnectMsg->pAddressPlayer);
			DPL(0, "    pAddressDevice = 0x%08x", 1, pIndicateConnectMsg->pAddressDevice);
			*/

#pragma TODO(vanceo, "Validate this")
		  break;

		default:
			DPL(0, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // ConnexLeaveWhileJoinServerDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexLeaveWhileJoinClientDPNMessageHandler()"
//==================================================================================
// ConnexLeaveWhileJoinClientDPNMessageHandler
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
HRESULT ConnexLeaveWhileJoinClientDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT							hr = DPN_OK;
	PCONNEXLEAVEWHILEJOINCONTEXT	pContext = (PCONNEXLEAVEWHILEJOINCONTEXT) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				DPL(0, "WARNING: Connect completion failed, this had better be because of a test failure!  0x%08x",
					1, pConnectCompleteMsg->hResultCode);
#pragma TODO(vanceo, "How do we notify main thread?")
			} // end if (connect completion failed)

			if (! SetEvent((HANDLE) pConnectCompleteMsg->pvUserContext))
			{
				hr = GetLastError();

				DPL(0, "Couldn't set connect complete event %x!  0x%08x",
					2, pConnectCompleteMsg->pvUserContext, hr);

				if (hr == DPN_OK)
					hr = E_FAIL;

				//goto DONE;
			} // end if (couldn't set completion event)
		  break;

		case DPN_MSGID_TERMINATE_SESSION:
			if (pContext->hDroppedAndJoinedEvent == NULL)
			{
				DPL(0, "WARNING: Ignoring TERMINATE_SESSION message because no DroppedAndJoined event, this had better be because of a test failure!", 0);
			} // end if (no dropped and joined event)
			else
			{
				if (! SetEvent(pContext->hDroppedAndJoinedEvent))
				{
					hr = GetLastError();

					DPL(0, "Couldn't set DroppedAndJoined event %x!  0x%08x",
						2, pContext->hDroppedAndJoinedEvent, hr);

					if (hr == DPN_OK)
						hr = E_FAIL;

					//goto DONE;
				} // end if (couldn't set dropped and joined event)
			} // end else (dropped and joined event)
		  break;

		default:
			DPL(0, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	return (hr);
} // ConnexLeaveWhileJoinClientDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexManyHostDPNMessageHandler()"
//==================================================================================
// ConnexManyHostDPNMessageHandler
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
HRESULT ConnexManyHostDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT					hr = DPN_OK;
	PCONNEXMANYHOSTCONTEXT	pContext = (PCONNEXMANYHOSTCONTEXT) pvContext;
	int						iTempCount;


	switch (dwMsgType)
	{
		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;
			
			DPL(0, "DPN_MSGID_CREATE_PLAYER", 0);
			DPL(0, "    dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			DPL(0, "    dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			DPL(0, "    pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

#pragma BUGBUG(vanceo, "Validate DPN_MSGID_CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER		pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;
			
			DPL(0, "DPN_MSGID_DESTROY_PLAYER", 0);
			DPL(0, "    dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			DPL(0, "    dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			DPL(0, "    pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

#pragma BUGBUG(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;


			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			DPL(0, "DPN_MSGID_INDICATE_CONNECT", 0);
			DPL(0, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			DPL(0, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			DPL(0, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			DPL(0, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			DPL(0, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			DPL(0, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			DPL(0, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			DPL(0, "    pAddressPlayer = 0x%08x", 1, pIndicateConnectMsg->pAddressPlayer);
			DPL(0, "    pAddressDevice = 0x%08x", 1, pIndicateConnectMsg->pAddressDevice);


			// Verify the incoming data.
			if (pIndicateConnectMsg->pvUserConnectData != NULL)
			{
				if (pContext->pvReplyData == NULL)
				{
					DPL(0, "%u bytes of user connect data was received (%x) but none was expected!  DEBUGBREAK()-ing.",
						2, pIndicateConnectMsg->dwUserConnectDataSize,
						pIndicateConnectMsg->pvUserConnectData);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's not supposed to be data)

				if (pIndicateConnectMsg->dwUserConnectDataSize != pContext->dwUserDataSize)
				{
					DPL(0, "Size of user connect data received (%u bytes at %x) but not expected value (%u)!  DEBUGBREAK()-ing.",
						3, pIndicateConnectMsg->dwUserConnectDataSize,
						pIndicateConnectMsg->pvUserConnectData, pContext->dwUserDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data size isn't correct)


				if (! IsFilledWithDWord(pIndicateConnectMsg->pvUserConnectData, pIndicateConnectMsg->dwUserConnectDataSize, DATAPATTERN_CONNECT))
				{
					DPL(0, "User connect data received is corrupted (%x, %u bytes)!  DEBUGBREAK()-ing.",
						2, pIndicateConnectMsg->pvUserConnectData,
						pIndicateConnectMsg->dwUserConnectDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data isn't correct)
			} // end if (incoming data)
			else
			{
				if (pContext->pvReplyData != NULL)
				{
					DPL(0, "%u bytes of user connect data was expected but none was received!  DEBUGBREAK()-ing.",
						1, pContext->dwUserDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's supposed to be data)

				if (pIndicateConnectMsg->dwUserConnectDataSize != 0)
				{
					DPL(0, "User connect data pointer was NULL as expected, but size was %u!  DEBUGBREAK()-ing.",
						1, pIndicateConnectMsg->dwUserConnectDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (the size is not right)
			} // end else (no incoming data)


			// Make sure the context is NULL.
			if (pIndicateConnectMsg->pvPlayerContext != NULL)
			{
				DPL(0, "Initial value for context is not NULL (%x)!  DEBUGBREAK()-ing.",
					1, pIndicateConnectMsg->pvPlayerContext);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (context is not NULL)

#pragma TODO(vanceo, "Make sure it's the other tester (address)")

			// Respond with data, if there is any.
			if (pContext->pvReplyData != NULL)
			{
				pIndicateConnectMsg->pvReplyData = pContext->pvReplyData;
				pIndicateConnectMsg->dwReplyDataSize = pContext->dwUserDataSize;
				pIndicateConnectMsg->pvReplyContext = NULL;

				DPL(0, "Replying to connect with %u bytes of data at %x.",
					2, pContext->dwUserDataSize, pContext->pvReplyData);
			} // end if (there's some user data)
			else
			{
				// Otherwise see if that's the last connect necessary.

				iTempCount = InterlockedDecrement((LPLONG) (&pContext->iNumConnectsRemaining));
				if (iTempCount < 0)
				{
					DPL(0, "Number of connects remaining is bad!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (count went negative)
				
				// Still more connects?
				if (iTempCount > 0)
				{
					DPL(2, "Still approximately %i more connects remaining, not notifying main thread.",
						1, iTempCount);
				} // end if (still more connects)
				else
				{
					// If we got here, that was the last connect expected, so notify
					// the main thread.

					DPL(1, "Last connect indicated, notifying main thread.", 0);

					if (! SetEvent(pContext->hConnectsDoneEvent))
					{
						hr = GetLastError();
						DPL(0, "Couldn't set connects done event (%x)!  0x%08x",
							2, pContext->hConnectsDoneEvent, hr);

						DEBUGBREAK();

						if (hr == S_OK)
							hr = E_FAIL;

						goto DONE;
					} // end if (couldn't set event)
				} // end else (no more connects)
			} // end else (no data)


			// If we're rejecting connection attempts, do so.
			if (pContext->fReject)
			{
				DPL(1, "Rejecting connection attempt.", 0);
				hr = E_FAIL;
			} // end if (rejecting connection attempt)
		  break;

		case DPN_MSGID_RETURN_BUFFER:
			PDPNMSG_RETURN_BUFFER	pReturnBufferMsg;



			pReturnBufferMsg = (PDPNMSG_RETURN_BUFFER) pvMsg;

			DPL(0, "DPN_MSGID_RETURN_BUFFER", 0);
			DPL(0, "    dwSize = %u", 1, pReturnBufferMsg->dwSize);
			DPL(0, "    hResultCode = 0x%08x", 1, pReturnBufferMsg->hResultCode);
			DPL(0, "    pvBuffer = %x", 1, pReturnBufferMsg->pvBuffer);
			DPL(0, "    pvUserContext = %x", 1, pReturnBufferMsg->pvUserContext);


			if (pContext->pvReplyData == NULL)
			{
				DPL(0, "No reply data was sent but returning buffer %x (context = %x)!?  DEBUGBREAK()-ing.",
					2, pReturnBufferMsg->pvBuffer,
					pReturnBufferMsg->pvUserContext);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not using data)

			if (pReturnBufferMsg->pvBuffer != pContext->pvReplyData)
			{
				DPL(0, "Buffer returned is not expected (%x != %x)!  DEBUGBREAK()-ing.",
					2, pReturnBufferMsg->pvBuffer, pContext->pvReplyData);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (buffer is wrong)

			if (pReturnBufferMsg->pvUserContext != NULL)
			{
				DPL(0, "Reply context returned is wrong (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pReturnBufferMsg->pvUserContext);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (context is NULL)


			iTempCount = InterlockedDecrement((LPLONG) (&pContext->iNumConnectsRemaining));
			if (iTempCount < 0)
			{
				DPL(0, "Number of connects remaining is bad!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (count went negative)
			
			// Still more connects?
			if (iTempCount > 0)
			{
				DPL(2, "Still approximately %i more connects remaining, not notifying main thread.",
					1, iTempCount);
				goto DONE;
			} // end if (still more connects)


			// If we got here, that was the buffer for the last expected connect
			// attempt, so notify the main thread.

			DPL(1, "Last connect reply buffer returned, notifying main thread.", 0);

			if (! SetEvent(pContext->hConnectsDoneEvent))
			{
				hr = GetLastError();
				DPL(0, "Couldn't set connects done event (%x)!  0x%08x",
					2, pContext->hConnectsDoneEvent, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				//goto DONE;
			} // end if (couldn't set event)
		  break;

		default:
			DPL(0, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // ConnexManyHostDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexManyConnectorDPNMessageHandler()"
//==================================================================================
// ConnexManyConnectorDPNMessageHandler
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
HRESULT ConnexManyConnectorDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT							hr = DPN_OK;
	PCONNEXMANYCONNECTORCONTEXT		pContext = (PCONNEXMANYCONNECTORCONTEXT) pvContext;
	int								iInterface;


	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			DPL(0, "DPN_MSGID_CONNECT_COMPLETE", 0);
			DPL(0, "    dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			DPL(0, "    hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			DPL(0, "    pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			DPL(0, "    hResultCode = 0x%08x", 1, pConnectCompleteMsg->hResultCode);
			DPL(0, "    pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			DPL(0, "    dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);

			iInterface = (int) ((INT_PTR) pConnectCompleteMsg->pvUserContext);

			if (pContext->fReject)
			{
				if (pConnectCompleteMsg->hResultCode != DPNERR_HOSTREJECTEDCONNECTION)
				{
					DPL(0, "Connect didn't complete with expected HOSTREJECTEDCONNECTION failure!  DEBUGBREAK()-ing.  0x%08x",
						1, pConnectCompleteMsg->hResultCode);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (connect completion failed)
			} // end if (connect attempts should be rejected)
			else
			{
				if (pConnectCompleteMsg->hResultCode != DPN_OK)
				{
					DPL(0, "Connect completion failed, this had better be because of a test failure!  DEBUGBREAK()-ing.  0x%08x",
						1, pConnectCompleteMsg->hResultCode);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (connect completion failed)
			} // end else (connect attempts should succeed)


			// Verify the incoming data.
			if (pConnectCompleteMsg->pvApplicationReplyData != NULL)
			{
				if (pContext->dwReplyDataSize == 0)
				{
					DPL(0, "%u bytes of reply data was received (%x) but none was expected!  DEBUGBREAK()-ing.",
						2, pConnectCompleteMsg->dwApplicationReplyDataSize,
						pConnectCompleteMsg->pvApplicationReplyData);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's not supposed to be data)

				if (pConnectCompleteMsg->dwApplicationReplyDataSize != pContext->dwReplyDataSize)
				{
					DPL(0, "Size of reply data received (%u bytes at %x) but not expected value (%u)!  DEBUGBREAK()-ing.",
						3, pConnectCompleteMsg->dwApplicationReplyDataSize,
						pConnectCompleteMsg->pvApplicationReplyData,
						pContext->dwReplyDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data size isn't correct)


				if (! IsFilledWithDWord(pConnectCompleteMsg->pvApplicationReplyData, pConnectCompleteMsg->dwApplicationReplyDataSize, DATAPATTERN_REPLY))
				{
					DPL(0, "Reply data received is corrupted (%x, %u bytes)!  DEBUGBREAK()-ing.",
						2, pConnectCompleteMsg->pvApplicationReplyData,
						pConnectCompleteMsg->dwApplicationReplyDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data isn't correct)
			} // end if (incoming data)
			else
			{
				if (pContext->dwReplyDataSize != 0)
				{
					DPL(0, "%u bytes of reply data was expected but none was received!  DEBUGBREAK()-ing.",
						1, pContext->dwReplyDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's supposed to be data)

				if (pConnectCompleteMsg->dwApplicationReplyDataSize != 0)
				{
					DPL(0, "Reply received data pointer was NULL as expected, but size was %u!  DEBUGBREAK()-ing.",
						1, pConnectCompleteMsg->dwApplicationReplyDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (the size is not right)
			} // end else (no incoming data)


			if (! SetEvent(pContext->pahConnectCompleteEvents[iInterface]))
			{
				hr = GetLastError();

				DPL(0, "Couldn't set connect complete %i event %x!  0x%08x",
					3, iInterface, pContext->pahConnectCompleteEvents[iInterface], hr);

				if (hr == DPN_OK)
					hr = E_FAIL;

				//goto DONE;
			} // end if (couldn't set completion event)
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;
			
			DPL(0, "DPN_MSGID_CREATE_PLAYER", 0);
			DPL(0, "    dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			DPL(0, "    dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			DPL(0, "    pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

#pragma BUGBUG(vanceo, "Validate DPN_MSGID_CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;
			
			DPL(0, "DPN_MSGID_DESTROY_PLAYER", 0);
			DPL(0, "    dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			DPL(0, "    dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			DPL(0, "    pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

#pragma BUGBUG(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		default:
			DPL(0, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // ConnexManyConnectorDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexServerLossServerDPNMessageHandler()"
//==================================================================================
// ConnexServerLossServerDPNMessageHandler
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
HRESULT ConnexServerLossServerDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT						hr = DPN_OK;
	PCONNEXSERVERLOSSCONTEXT	pContext = (PCONNEXSERVERLOSSCONTEXT) pvContext;
	int							iFirstTesterNotIndicated = -1;
	BOOL						fFoundPlayer = FALSE;
	int							i;


	switch (dwMsgType)
	{
		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER		pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;
			
			for(i = 0; i < pContext->pTNecd->iNumMachines; i++)
			{
				if (pContext->padpnidTesters[i] == pDestroyPlayerMsg->dpnidPlayer)
				{
					fFoundPlayer = TRUE;

					if (pContext->pafTesterIndicated[i])
					{
						DPL(0, "Got duplicate DESTROY_PLAYER for tester %i (player ID %u/%x)!  DEBUGBREAK()-ing!",
							3, i, pDestroyPlayerMsg->dpnidPlayer,
							pDestroyPlayerMsg->dpnidPlayer);
						// Ignore error
						pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
														"Got duplicate DESTROY_PLAYER for tester %i (player ID %u/%x)!  DEBUGBREAK()-ing!",
														3, i, pDestroyPlayerMsg->dpnidPlayer,
														pDestroyPlayerMsg->dpnidPlayer);

						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					} // end if (tester was already indicated)

					if ((i == 0) && (! pContext->fSelfIndicate))
					{
						DPL(0, "Got unexpected DESTROY_PLAYER for local player (ID %u/%x)!  DEBUGBREAK()-ing!",
							2, pDestroyPlayerMsg->dpnidPlayer,
							pDestroyPlayerMsg->dpnidPlayer);
						// Ignore error
						pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
														"Got unexpected DESTROY_PLAYER for local player (ID %u/%x)!  DEBUGBREAK()-ing!",
														2, pDestroyPlayerMsg->dpnidPlayer,
														pDestroyPlayerMsg->dpnidPlayer);

						DEBUGBREAK();
						hr = E_FAIL;
						goto DONE;
					} // end if (local player but shouldn't get it)

					pContext->pafTesterIndicated[i] = TRUE;
				} // end if (found player)


				// Make sure everyone so far has been indicated (except local
				// player if not expecting that).
				if ((i != 0) || (pContext->fSelfIndicate))
				{
					if (! pContext->pafTesterIndicated[i])
						iFirstTesterNotIndicated = i;
				} // end if (not local player or self indicating)


				// If we found the player we wanted, but not all the players have
				// been indicated, then we're done here.
				if ((fFoundPlayer) && (iFirstTesterNotIndicated >= 0))
				{
					DPL(2, "Found tester corresponding to DELETE_PLAYER, but tester %i still hasn't disconnected, not notifying main thread.",
						1, iFirstTesterNotIndicated);
					goto DONE;
				} // end if (found tester, but not all indicated)
			} // end for (each tester)

			if (! fFoundPlayer)
			{
				DPL(0, "Got DESTROY_PLAYER for unknown player ID %u/%x!  DEBUGBREAK()-ing!",
					2, pDestroyPlayerMsg->dpnidPlayer,
					pDestroyPlayerMsg->dpnidPlayer);
				// Ignore error
				pContext->pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
												"Got DESTROY_PLAYER for unknown player ID %u/%x!  DEBUGBREAK()-ing!",
												2, pDestroyPlayerMsg->dpnidPlayer,
												pDestroyPlayerMsg->dpnidPlayer);

				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (local player but shouldn't get it)


			// If we made it here, we found the tester matching this DELETE_PLAYER
			// message, and now all testers expected to be deleted actually have.
			DPL(1, "All testers expected to be deleted have been, notifying main thread.", 0);

			if (! SetEvent(pContext->hDisconnectEvent))
			{
				hr = GetLastError();
				DPL(0, "Couldn't set disconnect event (%x)!  0x%08x",
					2, pContext->hDisconnectEvent, hr);
				if (hr == S_OK)
					hr = E_FAIL;
				goto DONE;
			} // end if (couldn't set disconnect event)
		  break;

		default:
			DPL(0, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // ConnexServerLossServerDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexServerLossClientDPNMessageHandler()"
//==================================================================================
// ConnexServerLossClientDPNMessageHandler
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
HRESULT ConnexServerLossClientDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT						hr = DPN_OK;
	PCONNEXSERVERLOSSCONTEXT	pContext = (PCONNEXSERVERLOSSCONTEXT) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_TERMINATE_SESSION:
			PDPNMSG_TERMINATE_SESSION	pTerminateSessionMsg;


			pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION) pvMsg;

#pragma BUGBUG(vanceo, "Validate DPN_MSGID_TERMINATE_SESSION")

			DPL(1, "Got expected terminate session message, notifying main thread.", 0);

			if (! SetEvent(pContext->hDisconnectEvent))
			{
				hr = GetLastError();
				DPL(0, "Couldn't set disconnect event (%x)!  0x%08x",
					2, pContext->hDisconnectEvent, hr);
				if (hr == S_OK)
					hr = E_FAIL;
				//goto DONE;
			} // end if (couldn't set disconnect event)
		  break;

		default:
			DPL(0, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	return (hr);
} // ConnexServerLossClientDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexRejectDPNMessageHandler()"
//==================================================================================
// ConnexRejectDPNMessageHandler
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
HRESULT ConnexRejectDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT					hr = DPN_OK;
	PCONNEXREJECTCONTEXT	pContext = (PCONNEXREJECTCONTEXT) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;



			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;
			
			DPL(0, "DPN_MSGID_CONNECT_COMPLETE", 0);
			DPL(0, "    dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			DPL(0, "    hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			DPL(0, "    pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			DPL(0, "    hResultCode = 0x%08x", 1, pConnectCompleteMsg->hResultCode);
			DPL(0, "    pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			DPL(0, "    dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			if (pContext->pTNecd->iTesterNum == 0)
			{
				DPL(0, "Getting connect completion on host!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (tester 0)

#pragma TODO(vanceo, "Validate hAsyncOp")

			if (pConnectCompleteMsg->pvUserContext != pContext->hConnectEvent)
			{
				DPL(0, "Connect completion context is wrong (%x != %x)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvUserContext, pContext->hConnectEvent);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (context wrong)


			// Make sure we got the right error result code.
			if (pConnectCompleteMsg->hResultCode != DPNERR_HOSTREJECTEDCONNECTION)
			{
				DPL(0, "Connect completed with wrong error result code (not HOSTREJECTEDCONNECTION)!  DEBUGBREAK()-ing.  0x%08x",
					1, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong error code)


			// Verify the incoming data.
			if (pConnectCompleteMsg->pvApplicationReplyData != NULL)
			{
				if (pContext->pvConnectData == NULL)
				{
					DPL(0, "%u bytes of reply data was received (%x) but none was expected!  DEBUGBREAK()-ing.",
						2, pConnectCompleteMsg->dwApplicationReplyDataSize,
						pConnectCompleteMsg->pvApplicationReplyData);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's not supposed to be data)

				if (pConnectCompleteMsg->dwApplicationReplyDataSize != pContext->dwConnectDataSize)
				{
					DPL(0, "Size of reply data received (%u bytes at %x) but not expected value (%u)!  DEBUGBREAK()-ing.",
						3, pConnectCompleteMsg->dwApplicationReplyDataSize,
						pConnectCompleteMsg->pvApplicationReplyData,
						pContext->dwConnectDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data size isn't correct)

				if (! IsFilledWithDWord(pConnectCompleteMsg->pvApplicationReplyData, pConnectCompleteMsg->dwApplicationReplyDataSize, DATAPATTERN_REPLY))
				{
					DPL(0, "Reply data received is corrupted (%x, %u bytes)!  DEBUGBREAK()-ing.",
						2, pConnectCompleteMsg->pvApplicationReplyData,
						pConnectCompleteMsg->dwApplicationReplyDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data isn't correct)
			} // end if (incoming data)
			else
			{
				if (pContext->pvConnectData != NULL)
				{
					DPL(0, "%u bytes of reply data was expected but none was received!  DEBUGBREAK()-ing.",
						1, pContext->dwConnectDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's supposed to be data)

				if (pConnectCompleteMsg->dwApplicationReplyDataSize != 0)
				{
					DPL(0, "Reply received data pointer was NULL as expected, but size was %u!  DEBUGBREAK()-ing.",
						1, pConnectCompleteMsg->dwApplicationReplyDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (the size is not right)
			} // end else (no incoming data)


			// Alert the main thread.
			if (! SetEvent(pContext->hConnectEvent))
			{
				hr = GetLastError();
				DPL(0, "Couldn't set connect completion event (%x)!  0x%08x",
					2, pContext->hConnectEvent, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				goto DONE;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;
			
			DPL(0, "DPN_MSGID_CREATE_PLAYER", 0);
			DPL(0, "    dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			DPL(0, "    dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			DPL(0, "    pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

			if (pContext->pTNecd->iTesterNum != 0)
			{
				DPL(0, "Getting destroy player on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)

#pragma TODO(vanceo, "Validate DPN_MSGID_CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;
			
			DPL(0, "DPN_MSGID_DESTROY_PLAYER", 0);
			DPL(0, "    dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			DPL(0, "    dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			DPL(0, "    pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

			if (pContext->pTNecd->iTesterNum != 0)
			{
				DPL(0, "Getting destroy player on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)

#pragma TODO(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;



			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;
			
			DPL(0, "DPN_MSGID_INDICATE_CONNECT", 0);
			DPL(0, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			DPL(0, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			DPL(0, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			DPL(0, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			DPL(0, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			DPL(0, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			DPL(0, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			DPL(0, "    pAddressPlayer = 0x%08x", 1, pIndicateConnectMsg->pAddressPlayer);
			DPL(0, "    pAddressDevice = 0x%08x", 1, pIndicateConnectMsg->pAddressDevice);

			if (pContext->pTNecd->iTesterNum != 0)
			{
				DPL(0, "Getting indicate connect on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)

			// Verify the incoming data.
			if (pIndicateConnectMsg->pvUserConnectData != NULL)
			{
				if (pContext->pvConnectData == NULL)
				{
					DPL(0, "%u bytes of user connect data was received (%x) but none was expected!  DEBUGBREAK()-ing.",
						2, pIndicateConnectMsg->dwUserConnectDataSize,
						pIndicateConnectMsg->pvUserConnectData);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's not supposed to be data)

				if (pIndicateConnectMsg->dwUserConnectDataSize != pContext->dwConnectDataSize)
				{
					DPL(0, "Size of user connect data received (%u bytes at %x) but not expected value (%u)!  DEBUGBREAK()-ing.",
						3, pIndicateConnectMsg->dwUserConnectDataSize,
						pIndicateConnectMsg->pvUserConnectData, pContext->dwConnectDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data size isn't correct)

				if (! IsFilledWithDWord(pIndicateConnectMsg->pvUserConnectData, pIndicateConnectMsg->dwUserConnectDataSize, DATAPATTERN_CONNECT))
				{
					DPL(0, "User connect data received is corrupted (%x, %u bytes)!  DEBUGBREAK()-ing.",
						2, pIndicateConnectMsg->pvUserConnectData,
						pIndicateConnectMsg->dwUserConnectDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (data isn't correct)
			} // end if (incoming data)
			else
			{
				if (pContext->pvConnectData != NULL)
				{
					DPL(0, "%u bytes of user connect data was expected but none was received!  DEBUGBREAK()-ing.",
						1, pContext->dwConnectDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (there's supposed to be data)

				if (pIndicateConnectMsg->dwUserConnectDataSize != 0)
				{
					DPL(0, "User connect data pointer was NULL as expected, but size was %u!  DEBUGBREAK()-ing.",
						1, pIndicateConnectMsg->dwUserConnectDataSize);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (the size is not right)
			} // end else (no incoming data)


#pragma TODO(vanceo, "Make sure it's the other tester querying (address)")


			// Respond with data, if there is any.
			if (pContext->pvConnectData != NULL)
			{
				pIndicateConnectMsg->pvReplyData = pContext->pvConnectData;
				pIndicateConnectMsg->dwReplyDataSize = pContext->dwConnectDataSize;
				pIndicateConnectMsg->pvReplyContext = pContext->hReplyBufferReturnedEvent;

				DPL(0, "Rejecting connect with %u bytes of data at %x.",
					2, pContext->dwConnectDataSize, pContext->pvConnectData);
			} // end if (there's some user data)



			// Alert the main thread.
			if (! SetEvent(pContext->hConnectEvent))
			{
				hr = GetLastError();
				DPL(0, "Couldn't set connect indication event (%x)!  0x%08x",
					2, pContext->hConnectEvent, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				goto DONE;
			} // end if (couldn't set event)


			// Return a failure to reject the connection.
			hr = E_FAIL;
		  break;

		case DPN_MSGID_RETURN_BUFFER:
			PDPNMSG_RETURN_BUFFER	pReturnBufferMsg;



			pReturnBufferMsg = (PDPNMSG_RETURN_BUFFER) pvMsg;

			DPL(0, "DPN_MSGID_RETURN_BUFFER", 0);
			DPL(0, "    dwSize = %u", 1, pReturnBufferMsg->dwSize);
			DPL(0, "    hResultCode = 0x%08x", 1, pReturnBufferMsg->hResultCode);
			DPL(0, "    pvBuffer = %x", 1, pReturnBufferMsg->pvBuffer);
			DPL(0, "    pvUserContext = %x", 1, pReturnBufferMsg->pvUserContext);


			if (pContext->pTNecd->iTesterNum != 0)
			{
				DPL(0, "Getting return buffer on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)


			if (pContext->pvConnectData == NULL)
			{
				DPL(0, "No response data was sent but returning buffer %x (context = %x)!?  DEBUGBREAK()-ing.",
					2, pReturnBufferMsg->pvBuffer,
					pReturnBufferMsg->pvUserContext);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not using data)

			if (pReturnBufferMsg->pvBuffer != pContext->pvConnectData)
			{
				DPL(0, "Buffer returned is not expected (%x != %x)!  DEBUGBREAK()-ing.",
					2, pReturnBufferMsg->pvBuffer, pContext->pvConnectData);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (buffer is wrong)

			if (pReturnBufferMsg->pvUserContext != pContext->hReplyBufferReturnedEvent)
			{
				DPL(0, "Context returned is wrong (%x != %x)!  DEBUGBREAK()-ing.",
					2, pReturnBufferMsg->pvUserContext, pContext->hReplyBufferReturnedEvent);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (context is NULL)


			// Alert the main thread.
			if (! SetEvent(pContext->hReplyBufferReturnedEvent))
			{
				hr = GetLastError();
				DPL(0, "Couldn't set reply buffer returned event (%x)!  0x%08x",
					2, pContext->hReplyBufferReturnedEvent, hr);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				goto DONE;
			} // end if (couldn't set event)
		  break;

		default:
			DPL(0, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // ConnexRejectDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ConnexNotAllowedDPNMessageHandler()"
//==================================================================================
// ConnexNotAllowedDPNMessageHandler
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
HRESULT ConnexNotAllowedDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT						hr = DPN_OK;
	PCONNEXNOTALLOWEDCONTEXT	pContext = (PCONNEXNOTALLOWEDCONTEXT) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_APPLICATION_DESC:
			if (pContext->pTNecd->iTesterNum != 0)
			{
				DPL(0, "Getting application desc notification on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)
		  break;

		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;



			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;
			
			DPL(0, "DPN_MSGID_CONNECT_COMPLETE", 0);
			DPL(0, "    dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			DPL(0, "    hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			DPL(0, "    pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			DPL(0, "    hResultCode = 0x%08x", 1, pConnectCompleteMsg->hResultCode);
			DPL(0, "    pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			DPL(0, "    dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			if ((pContext->pTNecd->iTesterNum == 0) && (! pContext->fConnectsAllowed))
			{
				DPL(0, "Getting connect completion on host when they're not expected!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (tester 0)

#pragma TODO(vanceo, "Validate hAsyncOp")

			if (pConnectCompleteMsg->pvUserContext != pContext->hConnectCompletedEvent)
			{
				DPL(0, "Connect completion context is wrong (%x != %x)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvUserContext, pContext->hConnectCompletedEvent);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (context wrong)


			// Make sure we got the right error result code.
			if (pConnectCompleteMsg->hResultCode != pContext->hrExpectedResult)
			{
				DPL(0, "Connect completed with wrong error result code (%x != %x)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->hResultCode,
					pContext->hrExpectedResult);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (wrong error code)


			// Verify the incoming data.
			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				DPL(0, "%u bytes of reply data was received (%x) but none was expected!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwApplicationReplyDataSize,
					pConnectCompleteMsg->pvApplicationReplyData);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (incoming data)


			// Alert the main thread, if necessary.
			if (pContext->hConnectCompletedEvent != NULL)
			{
				if (! SetEvent(pContext->hConnectCompletedEvent))
				{
					hr = GetLastError();
					DPL(0, "Couldn't set connect completion event (%x)!  0x%08x",
						2, pContext->hConnectCompletedEvent, hr);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					goto DONE;
				} // end if (couldn't set event)
			} // end if (there's an event)
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;
			
			DPL(0, "DPN_MSGID_CREATE_PLAYER", 0);
			DPL(0, "    dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			DPL(0, "    dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			DPL(0, "    pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

			if (pContext->pTNecd->iTesterNum != 0)
			{
				DPL(0, "Getting destroy player on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)

#pragma TODO(vanceo, "Validate DPN_MSGID_CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;
			
			DPL(0, "DPN_MSGID_DESTROY_PLAYER", 0);
			DPL(0, "    dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			DPL(0, "    dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			DPL(0, "    pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

			if (pContext->pTNecd->iTesterNum != 0)
			{
				DPL(0, "Getting destroy player on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)

#pragma TODO(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;



			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;
			
			DPL(0, "DPN_MSGID_INDICATE_CONNECT", 0);
			DPL(0, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			DPL(0, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			DPL(0, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			DPL(0, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			DPL(0, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			DPL(0, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			DPL(0, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			DPL(0, "    pAddressPlayer = 0x%08x", 1, pIndicateConnectMsg->pAddressPlayer);
			DPL(0, "    pAddressDevice = 0x%08x", 1, pIndicateConnectMsg->pAddressDevice);

			if (pContext->pTNecd->iTesterNum != 0)
			{
				DPL(0, "Getting indicate connect on non-tester 0!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (not tester 0)

			if (! pContext->fConnectsAllowed)
			{
				DPL(0, "Getting unexpected DPN_MSGID_INDICATE_CONNECT (msg = %x)!  DEBUGBREAK()-ing.",
					1, pIndicateConnectMsg);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (connects are not allowed)
		  break;

		default:
			DPL(0, "Unexpected message type %x!  DEBUGBREAK()-ing.", 1, dwMsgType);

			DEBUGBREAK();

			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)


DONE:

	return (hr);
} // ConnexNotAllowedDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

