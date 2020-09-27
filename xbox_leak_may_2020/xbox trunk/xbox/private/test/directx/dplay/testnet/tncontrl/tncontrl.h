#ifndef __TNCONTROL__
#define __TNCONTROL__
//#pragma message("Defining __TNCONTROL__")



#ifndef UNLEN
#include <LMCONS.H>
#endif // ! UNLEN



//==================================================================================
// Debugging help
//==================================================================================
// This is the version of the DLL
extern char		g_szTNCONTRLVersion[];





//==================================================================================
// Defines
//==================================================================================
#define CURRENT_TNCONTROL_API_VERSION	37



//BUGBUG um, do something better than this
#ifndef ERROR_BUFFER_TOO_SMALL
#define ERROR_BUFFER_TOO_SMALL			603
#endif //! ERROR_BUFFER_TOO_SMALL


#define MAX_COMPORT						255



//----------------------------------------------------------------------------------
// These sizes are how big the buffer should be in characters.  The actual maximum
// number of characters you can put in the buffers are these sizes - 1, to account
// for the zero termination.
//----------------------------------------------------------------------------------
#ifdef _XBOX // MAX_COMPUTERNAME_LENGTH not defined in Xbox environment
#define MAX_COMPUTERNAME_LENGTH		15
#endif

#define MAX_COMPUTERNAME_SIZE		(MAX_COMPUTERNAME_LENGTH + 1)
#define MAX_USERNAME_SIZE			(UNLEN + 1)



//----------------------------------------------------------------------------------
// Hardcoded control methods available
//----------------------------------------------------------------------------------

#define TN_CTRLMETHOD_TCPIP_OPTIMAL		1
//
// Communicates using WinSock TCP/IP.  Uses WinSock 2 functionality if on a machine
// that has it, reverts to WinSock 1 (inefficient) if not.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// dwMethodFlags			unused
// lpszMethodSessionInfo	can refer to a specific master machine (IP address or
//							name) to join (slave only).


#define TN_CTRLMETHOD_TCPIP_WINSOCK1	2
//
// Communicates using WinSock TCP/IP.  Forces WinSock 1 (inefficient) functionality
// even if on a machine that has WinSock 2.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// dwMethodFlags			unused
// lpszMethodSessionInfo	can refer to a specific master machine (IP address or
//							name) to join (slave only).


#define TN_CTRLMETHOD_TCPIP_WINSOCK2_ONETOONE	3
//
// Communicates using WinSock 2 functionality on TCP/IP (if available).  Each
// connection is assigned its own event, which is slightly more efficient than the
// regular WinSock 2 behavior (TN_CTRLMETHOD_TCPIP_OPTIMAL), but if the incoming
// traffic rate is high, earlier connections can starve later connections.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// dwMethodFlags			unused
// lpszMethodSessionInfo	can refer to a specific master machine (IP address or
//							name) to join (slave only).




//----------------------------------------------------------------------------------
// Control modes
//----------------------------------------------------------------------------------
#define TNMODE_API				1 // execute a sequence of tests, possibly more than once 
#define TNMODE_STRESS			2 // randomly call tests
#define TNMODE_POKE				3 // use the UI to run tests
#define TNMODE_PICKY			4 // master manually selects API tests to run
#define TNMODE_DOCUMENTATION	5 // execute a special documentation build of the module



//----------------------------------------------------------------------------------
// These are the possible test case options to use when adding tests.
// The first two sections contain options that must be specified.
//----------------------------------------------------------------------------------

// One or more of these four modes must be specified.

// Test can be executed in API mode.
#define TNTCO_API				0x0010

// Test can be executed in stress mode.
#define TNTCO_STRESS			0x0020

// Test can be executed in poke mode.
#define TNTCO_POKE				0x0040

// Test can be executed in picky mode.
#define TNTCO_PICKY				0x0080


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// One or more of these two options must be specified.

// Test can be executed as a top level test (directly within a script, for example).
#define TNTCO_TOPLEVELTEST		0x0100

// Test can be executed from within a scenario test.
#define TNTCO_SUBTEST			0x0200

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Test may call other tests.
#define TNTCO_SCENARIO			0x0400

// Don't save the results, output data, or output variables from this test or any
// subtest called by this test.  Any results accumulated will be freed upon the
// completion of this test case.
#define TNTCO_DONTSAVERESULTS	0x0800

// Can other tests be run while this one continues?
#define TNTCO_ONGOING			0x1000

// Will the test use the binary management SwapBinaryTo() function?
#define TNTCO_SWAPSBINARIES		0x2000




 

//----------------------------------------------------------------------------------
// These are the possible permutation modes that a test can be run.  This only
// affects tests in API mode.
//----------------------------------------------------------------------------------
// The test is run only once.  The first slave to join the session is tester 0, the
// second is tester 1, etc.
#define TNTPM_ONCE							1

// The test is run only enough times so that each slave in the session has
// participated at least once.
#define TNTPM_ALLSLAVES						2

// The test is run only enough times so that each slave in the session has executed
// it as tester number 0 at least once and as a tester number other than 0 at least
// once.
#define TNTPM_ALLSLAVESASTESTERS0ANDNON0	3

// The test is run only enough times so that each slave in the session has executed
// it in each tester position at least once.
#define TNTPM_ALLSLAVESASALLTESTERS			4

// The test is run in all the possible configurations using all the testers in the
// session.
#define TNTPM_ALLPOSSIBLE					5




//----------------------------------------------------------------------------------
// These are the possible flags to pass to Log().
//----------------------------------------------------------------------------------
// The string is critical, and will be highlighted when displayed.
#define TNLF_CRITICAL					0x01

// The string is important.
#define TNLF_IMPORTANT					0x02

// The string is not very important.
#define TNLF_VERBOSE					0x04

// Prepend the current test's unique ID before the line.
#define TNLF_PREFIX_TESTUNIQUEID		0x08

// Prepend the current test case's name before the line.
#define TNLF_PREFIX_CASENAME			0x10

// Prepend the current test case and instance ID before the line.
#define TNLF_PREFIX_CASEANDINSTANCEID	0x20



//----------------------------------------------------------------------------------
// These are the possible types of strings that may be generated for logging, as
// reported to the shell.
//----------------------------------------------------------------------------------
#define TNLST_CRITICAL						1 // A critical log string
#define TNLST_CONTROLLAYER_INFO				2 // A control layer information message, not related to any test
#define TNLST_CONTROLLAYER_TESTSUCCESS		3 // A successful test completion string
#define TNLST_CONTROLLAYER_TESTFAILURE		4 // A failed test completion string
#define TNLST_CONTROLLAYER_TESTWARNING		5 // A test warning string
#define TNLST_MODULE_IMPORTANT				6 // An important message from the module
#define TNLST_MODULE_NORMAL					7 // A normal message from the module
#define TNLST_MODULE_VERBOSE				8 // A verbose information message from the module





//----------------------------------------------------------------------------------
// Generic error codes.
//----------------------------------------------------------------------------------
#define TNERR_CONNECTIONDROPPED		0x86661001
#define TNERR_LOSTTESTER			0x86661002



//----------------------------------------------------------------------------------
// These are the possible return values for SyncWithTesters() and
// SyncWithTestersArray().
//----------------------------------------------------------------------------------

// Everything's okay, we're in sync with the other testers
#define TNSR_INSYNC					S_OK

// The user is aborting this test.
#define TNSR_USERCANCEL				0x86661101

// Another tester failed and left the test, probably invalidating any results this
// tester would come up with.  Should generally be treated as a USERCANCEL.
#define TNSR_LOSTTESTER				0x86661103



//----------------------------------------------------------------------------------
// These are the expected return values for WaitForEventOrCancel().
//----------------------------------------------------------------------------------

// The event(s) was/were triggered.
#define TNWR_GOTEVENT			S_OK

// The user is aborting this test.
#define TNWR_USERCANCEL			0x86661111

// The time limit elapsed before the event(s) was/were triggered.
#define TNWR_TIMEOUT			0x86661112

// Another tester failed and left the test, probably invalidating any results this
// tester would come up with.  Should generally be treated as a test failure.
#define TNWR_LOSTTESTER			0x86661113



//----------------------------------------------------------------------------------
// These are the expected return values for WaitForLeechConnection().
//----------------------------------------------------------------------------------

// The leech connected correctly.
#define TNCWR_CONNECTED			S_OK

// The user is aborting this test.
#define TNCWR_USERCANCEL		0x86661121

// The time limit elapsed before the leech connected.
#define TNCWR_TIMEOUT			0x86661122

// Another tester failed and left the test, probably invalidating any results this
// tester would come up with.  Should generally be treated as a test failure.
#define TNCWR_LOSTTESTER		0x86661123




//----------------------------------------------------------------------------------
// Hardcoded fault simulators available
//----------------------------------------------------------------------------------

#define TN_FAULTSIM_IMTEST		1
//
// TCP/IP fault simulator.  This is a driver wrapper which is only available on
// Windows 2000 machines.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// init data	none.





//----------------------------------------------------------------------------------
// Report limiting
//----------------------------------------------------------------------------------
// Only this many reports are allowed to accumulate in the list before new entries
// begin pushing old entries out.
#define DEFAULT_MAX_NUM_REPORTS		5000






//----------------------------------------------------------------------------------
// These are bit flags used to pass into report printing family of functions that
// determine what gets printed in the file header.
//----------------------------------------------------------------------------------

// Print information on the control session
#define TNRH_SESSIONINFO		0x0001
// Print the list of machines in the control session
#define TNRH_MACHINELIST		0x0002
// Print the list of tests currently being run.
#define TNRH_ACTIVETESTS		0x0004
// Print the list of tests remaining to be run.
#define TNRH_REMAININGTESTS		0x0008
// Print the list of tests that had to be skipped.
#define TNRH_SKIPPEDTESTS		0x0010




//----------------------------------------------------------------------------------
// These are body styles used to pass into the report printing family of functions.
// They are mutually exclusive (you can only pass one in).
//----------------------------------------------------------------------------------

// In order they were logged.
#define TNRBS_PLAIN									1
// In order they were logged.  Prints more related information with each
// report.
#define TNRBS_PLAIN_VERBOSE							2

/*
// In order they were logged, organized by machine.
#define TNRBS_GROUPBYMACHINE						3 
// In order they were logged, organized by machine.  Prints more related
// information with each report.
#define TNRBS_GROUPBYMACHINE_VERBOSE				4 

// In order they were logged, organized by test.
#define TNRBS_GROUPBYTEST							5
// In order they were logged, organized by test.  Prints more related
// information with each report.
#define TNRBS_GROUPBYTEST_VERBOSE					6
*/



//----------------------------------------------------------------------------------
// These are bit flags used to pass into report printing family of functions that
// determine what reports get printed.
//----------------------------------------------------------------------------------

// Report each completed test indicating success
#define TNREPORT_TESTSUCCESSES									0x0001
// Report each completed test indicating failure
#define TNREPORT_TESTFAILURES									0x0002

// Report each item not a completed test indicating success
#define TNREPORT_DURINGTESTSUCCESSES							0x0004
// Report each item not a completed test indicating failure
#define TNREPORT_DURINGTESTFAILURES								0x0008
// Report each item not a completed test indicating a warning
#define TNREPORT_DURINGTESTWARNINGS								0x0010

// Report each test assignment to a machine where the test has not completed yet
#define TNREPORT_TESTASSIGNMENTSTESTNOTCOMPLETE					0x0020
// Report each test assignment to a machine where the test eventually completed successfully
#define TNREPORT_TESTASSIGNMENTSTESTSUCCEEDED					0x0040
// Report each test assignment to a machine where the test eventually completed as a failure
#define TNREPORT_TESTASSIGNMENTSTESTFAILED						0x0080
// Report each test assignment to a machine where the test eventually had a warning
#define TNREPORT_TESTASSIGNMENTSTESTWARNED						0x0100

// Report test assignment completions for tests that have not completed yet
#define TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTNOTCOMPLETE		0x0200
// Report test assignment completions for tests that eventually completed successfully
#define TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTSUCCEEDED			0x0400
// Report test assignment completions for tests that eventually completed as a failure
#define TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTFAILED			0x0800
// Report test assignment completions for tests that eventually had a warning
#define TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTWARNED			0x1000

// Report every item not associated with a test (not a success, failure, test assignment, etc.)
#define TNREPORT_NONTESTREPORTS									0x2000


// Report every completed test in the log
#define TNREPORT_ALLCOMPLETEDTESTS								(TNREPORT_TESTSUCCESSES | TNREPORT_TESTFAILURES)
// Report every during-test item in the log
#define TNREPORT_ALLDURINGTESTITEMS								(TNREPORT_DURINGTESTSUCCESSES | TNREPORT_DURINGTESTFAILURES | TNREPORT_DURINGTESTWARNINGS)
// Report each test assignment, whether the test completed or not
#define TNREPORT_ALLSINGLETESTASSIGNMENTS						(TNREPORT_TESTASSIGNMENTSTESTNOTCOMPLETE | TNREPORT_TESTASSIGNMENTSTESTSUCCEEDED | TNREPORT_TESTASSIGNMENTSTESTFAILED | TNREPORT_TESTASSIGNMENTSTESTWARNED)
// Report all test assignment completions, whether the test completed or not
#define TNREPORT_ALLTESTASSIGNMENTCOMPLETIONS					(TNREPORT_TESTASSIGNMENTSTESTNOTCOMPLETE | TNREPORT_TESTASSIGNMENTSTESTSUCCEEDED | TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTFAILED | TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTWARNED)

// Report every item associated with a test (sucess, failures, test assignments, etc.)
#define TNREPORT_ALLTESTREPORTS									(TNREPORT_ALLCOMPLETEDTESTS | TNREPORT_ALLDURINGTESTITEMS | TNREPORT_ALLSINGLETESTASSIGNMENTS | TNREPORT_ALLTESTASSIGNMENTCOMPLETIONS)

// Reports for subtests should be printed as well
#define TNREPORT_SUBTESTS										0x4000

// Report every item in the log
#define TNREPORT_ALL											(TNREPORT_NONTESTREPORTS | TNREPORT_ALLTESTREPORTS | TNREPORT_SUBTESTS)




//----------------------------------------------------------------------------------
// These are method flags passed into CanReachViaIP to check whether the given
// communication is possible.
//----------------------------------------------------------------------------------
// Will UDP broadcasts reach the intended machine?
#define TNCR_IP_UDPBROADCAST	0x01

// Will directed UDP messages reach the intended machine?
#define TNCR_IP_UDP				0x02

// Will directed TCP/IP connections reach the intended machine?
#define TNCR_IP_TCP				0x04




//----------------------------------------------------------------------------------
// These are dialog types that PromptUser can display.
//----------------------------------------------------------------------------------

// Displays a question dialog box that simply waits for the user to click OK.
#define TNPUDT_OK				1
// There is no additional data.
// piResponse will always contain TNPUR_OK.


// Displays a question dialog box that asks the user to select yes or no.
#define TNPUDT_YESNO			2
// There is no additional data.
// piResponse will contain TNPUR_YES or TNPUR_NO depending on button pressed.


// Displays a dialog box where the user must select a choice from a combo box before
// OK is enabled. 
#define TNPUDT_SELECTCOMBO		3
// The additional data is an array of char*s, which are the strings to be displayed
// in a combo box for the user to select. 
// piResponse will contain the zero-based index of the selected choice.




//----------------------------------------------------------------------------------
// These are response types that PromptUser may return depending on dialog type.
//----------------------------------------------------------------------------------
#define TNPUR_OK		1
#define TNPUR_YES		2
#define TNPUR_NO		3




//==================================================================================
// Include the common objects used by classes
//==================================================================================
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"


//==================================================================================
// Include the class type definitions
//==================================================================================
#include "classes.h"




//==================================================================================
// Test execution result classes
//==================================================================================
class DLLEXPORT CTNSystemResult
{
	private:
		HRESULT		m_hr;


	public:
		CTNSystemResult(void):
		  m_hr(S_OK)
		{
		};

		CTNSystemResult(HRESULT hr):
		  m_hr(hr)
		{
		};

		~CTNSystemResult(void)
		{
		};


		inline void operator =(HRESULT hr)
		{
			this->m_hr = hr;
		};

		inline BOOL operator ==(HRESULT hr)
		{
			if (this->m_hr == hr)
				return (TRUE);
			return (FALSE);
		};

		inline BOOL operator !=(HRESULT hr)
		{
			if (this->m_hr != hr)
				return (TRUE);
			return (FALSE);
		};

		inline operator HRESULT(void)
		{
			return (this->m_hr);
		};
};

class DLLEXPORT CTNTestResult
{
	private:
		HRESULT		m_hr;


	public:
		CTNTestResult(void):
		  m_hr(S_OK)
		{
		};

		CTNTestResult(HRESULT hr):
		  m_hr(hr)
		{
		};

		~CTNTestResult(void)
		{
		};


		inline void operator =(HRESULT hr)
		{
			this->m_hr = hr;
		};

		inline BOOL operator ==(HRESULT hr)
		{
			if (this->m_hr == hr)
				return (TRUE);
			return (FALSE);
		};

		inline BOOL operator !=(HRESULT hr)
		{
			if (this->m_hr != hr)
				return (TRUE);
			return (FALSE);
		};

		inline operator HRESULT(void)
		{
			return (this->m_hr);
		};
};



//==================================================================================
// Structs that must be defined before classes can be fleshed out
//==================================================================================
typedef struct tagTNCTRLMACHINEID
{
	DWORD					dwTime;
} TNCTRLMACHINEID, * PTNCTRLMACHINEID;


typedef struct tagTNMODULEID
{
	// Short name of the application, must be same when joining sessions.
	char					szBriefName[32];

	// Major version, also must match.
	DWORD					dwMajorVersion;

	// Minor version, also must match
	DWORD					dwMinorVersion1;

	// Secondary minor version, is not checked for compatability
	DWORD					dwMinorVersion2;

	// Build version, is not checked for compatability
	DWORD					dwBuildVersion;
} TNMODULEID, * PTNMODULEID;


typedef struct tagTNSUCCESSFILTERITEM
{
	// Slave for this tester number.
	PTNMACHINEINFO			pSlave;

	// Result code slave returned in final report.
	HRESULT					hresult;

	// Whether the slave indicated a success or failure in final report.
	BOOL					fSuccess;

	// Output data for this slave, if any.
	PVOID					pvOutputData;

	// Size of output data, if any.
	DWORD					dwOutputDataSize;

	// Pointer to list of variables, if any.
	PTNOUTPUTVARSLIST		pVars;
} TNSUCCESSFILTERITEM, * PTNSUCCESSFILTERITEM;


//BUGBUG implement graphing
typedef struct tagTNGRAPHITEM
{
	DWORD					dwBlah;
} TNGRAPHITEM, * PTNGRAPHITEM;


typedef struct tagTNEXECCASEDATA
{
	// Size of this structure.
	DWORD					dwSize;

	// Pointer to executor interface object to use for reporting results,
	// synchronizing, and all the other major functions.
	PTNEXECUTOR				pExecutor;

	// Position in tester list in which to execute test.
	int						iTesterNum;

	// Total number of testers who are running this test.
	int						iNumMachines;

	// Pointer to initial input data to use, or NULL if none exists.
	PVOID					pvInputData;

	// Size of initial input data to use
	DWORD					dwInputDataSize;

	// Pointer to list to pass to subtests to retrieve their results.
	PTNRESULTSCHAIN			pSubResults;

	// Pointer to result object control layer will use when generating automatic
	// report.  Test will set this on its way out of the function.
	PTNRESULT				pFinalResult;

	// Pointer to local machine info object.
	PTNMACHINEINFO			pMachineInfo;
} TNEXECCASEDATA, * PTNEXECCASEDATA;


typedef struct tagTNCANRUNDATA
{
	// Size of this structure.
	DWORD					dwSize;

#ifndef _XBOX // no master supported
	// Pointer to master object to use.
	PTNMASTER				pMaster;
#endif // ! XBOX

	// Test case to check.
	PTNTESTTABLECASE		pTest;

	// Number of items in the following tester list.
	int						iNumMachines;

	// Array of slaves (in tester list order).  This is the configuration to
	// accept or reject.
	PTNMACHINEINFO*			apTesters;

	// Pointer to strings that will be passed to GetInputData.
	PTNSTOREDDATALIST		pStringData;

	// Boolean to set to TRUE if the configuration is acceptable.
	BOOL					fCanRun;
} TNCANRUNDATA, * PTNCANRUNDATA;


typedef struct tagTNGETINPUTDATA
{
	// Size of this structure.
	DWORD					dwSize;

#ifndef _XBOX // no master supported
	// Pointer to master object making call to use.
	PTNMASTER				pMaster;
#endif // ! XBOX

	// Pointer to machine that needs the input data.
	PTNMACHINEINFO			pMachine;

	// Machine's position in tester list.
	int						iTesterNum;

	// Test case to get data for.
	PTNTESTTABLECASE		pTest;

	// Pointer to strings to parse.
	PTNSTOREDDATALIST		pStringData;

	// Pointer to buffer to fill with data (or NULL if just retrieving data size).
	PVOID					pvData;


	// Size of buffer above.  If pvData is NULL, the callback should fill in the
	// required buffer size.
	DWORD					dwDataSize;
} TNGETINPUTDATA, * PTNGETINPUTDATA;


typedef struct tagTNWRITEDATA
{
	// Size of this structure.
	DWORD					dwSize;

	// Location of the data to write.
	DWORD					dwLocation;

	// Pointer to buffer with data to convert.
	PVOID					pvData;

	// Size of buffer to convert.
	DWORD					dwDataSize;

	// Pointer to associated machine.
	PTNMACHINEINFO			pMachine;

	// Machine's position in tester list.
	int						iTesterNum;

	// Pointer to test case with data associated.
	PTNTESTTABLECASE		pTest;

	// Pointer to list to store strings that are generated when converting the data.
	// These will be what gets written.
	PTNSTOREDDATA			pWriteStoredData;
} TNWRITEDATA, * PTNWRITEDATA;


typedef struct tagTNFILTERSUCCESSDATA
{
	// Size of this structure.
	DWORD					dwSize;

	// Number of items in the following item list.
	int						iNumMachines;

	// Array of final results (in tester list order) for each of the testers running
	// the test. 
	PTNSUCCESSFILTERITEM	aTesterResults;

	// Result code for filtered success report.  This will initially be S_OK, but
	// the module can set it to anything it wishes.
	HRESULT					hresult;

	// Whether the test case was successful after filtering.  This will initially be
	// TRUE if all testers' final reports were successes, or initially FALSE if at
	// least one testers' final report was a failure.  The module can change a TRUE
	// to FALSE if it deems the test a failure even though all the slaves think it
	// is a success.
	BOOL					fSuccess;

	// Pointer to buffer with data to use in filtered success report, if allocated.
	PVOID					pvData;

	// Size of filtered success report data buffer, or place to store size required.
	DWORD					dwDataSize;
} TNFILTERSUCCESSDATA, * PTNFILTERSUCCESSDATA;


typedef struct tagTNPOKETESTCOMPLETEDDATA
{
	// Size of this structure.
	DWORD					dwSize;

	// Pointer to test case which was executed.
	PTNTESTTABLECASE		pTest;

	// Whether the test was successful or not.
	BOOL					fSuccess;

	// Result code the test returned.
	HRESULT					hresult;

	// Pointer to output data generated by test case, if any.
	PVOID					pvData;

	// Size of output data generated by test case, if any.
	DWORD					dwDataSize;

	// User specified context passed in to ExecPokeTest function.
	PVOID					pvUserContext;
} TNPOKETESTCOMPLETEDDATA, * PTNPOKETESTCOMPLETEDDATA;





//==================================================================================
// Include the callback functions
//==================================================================================
#include "procs.h"


typedef struct tagTNTESTCASEPROCS
{
	// Size of this structure.
	DWORD							dwSize;

	// Callback to use to determine if a given machine can run the test.
	PTNCANRUNPROC					pfnCanRun;

	// Callback to use to retrieve initial data to pass into the tester's ExecCase
	// function, if the data hasn't already been retrieved by reading a script or
	// being passed in by a calling function (for sub-tests).
	PTNGETINPUTDATAPROC				pfnGetInputData;

	// Callback to use to execute the test case.
	PTNEXECCASEPROC					pfnExecCase;

	// Callback for writing initial and/or report data to a sequence of strings.
	PTNWRITEDATAPROC				pfnWriteData;

	// Callback that allows a master to verify whether success by all testers is
	// actually a successful test case.
	PTNFILTERSUCCESSPROC			pfnFilterSuccess;
} TNTESTCASEPROCS, * PTNTESTCASEPROCS;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct tagTNADDTESTDATA
{
	// Size of this structure.
	DWORD							dwSize;

	// Unique ID string for this test case in the form of "n.n.n...".  This string
	// pointer must be constant, that is, the string memory must remain valid for
	// the life of the application.
	// This string is required.
	char*							pszCaseID;

	// Name string for this test case.  This string pointer must be constant, that
	// is, the string memory must remain valid for the life of the application.
	// This string is required.
	char*							pszName;

	// Description string for this test case.  This string pointer must be constant,
	// that is, the string memory must remain valid for the life of the application.
	// This string is required.
	char*							pszDescription;

	// Input data informational string for this test case.  It is used as a comment
	// to explain what can be passed in for input data strings when selecting tests
	// in picky mode.  The pointer must be constant, that is, the string memory must
	// remain valid for the life of the application.
	// This string is required if a GetInputData callback is specified and must not
	// be specified otherwise.
	char*							pszInputDataHelp;

	// Number of machines required for this test.  You can also define tests which
	// allow a variable number of tests by using a negative number, where the
	// absolute value = the minimum number of testers required.
	// For example, 2 = two machines required, -3 = at least three machines
	// required, but more can be specified.
	int								iNumMachines;

	// Option flags (TNTCO_xxx).
	DWORD							dwOptionFlags;


	// Callback executed by the master that is used to determine if a given set of
	// machines (permutation) can run the test.
	PTNCANRUNPROC					pfnCanRun;

	// Callback executed by the master that is used to retrieve input data for top
	// level tests that will be passed into a tester's ExecCase function.  This is
	// not used for sub-tests, since the input data is specified by the slave when
	// calling the ExecSubTestCase function.
	PTNGETINPUTDATAPROC				pfnGetInputData;

	// Callback executed by the slave(s) which performs the test case.
	PTNEXECCASEPROC					pfnExecCase;

	// Callback executed by the master that is used for writing binary input and/or
	// report data to a sequence of strings for use in reports.
	PTNWRITEDATAPROC				pfnWriteData;

	// Callback that allows a master to verify whether success by all testers is
	// actually a successful test case.
	PTNFILTERSUCCESSPROC			pfnFilterSuccess;


	// Optional array of graph items.
	PTNGRAPHITEM					paGraphs;

	// Number of graph items in preceding array.
	DWORD							dwNumGraphs;
} TNADDTESTDATA, * PTNADDTESTDATA;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct tagTNJOINCONTROLSESSIONDATA
{
	// Size of this structure.
	DWORD							dwSize;

	// The control layer API version the caller intends to use.
	DWORD							dwAPIVersion;

	// The mode this session will be run in (TNMODE_xxx).
	DWORD							dwMode;

	// Pointer to structure that uniquely identifies the calling application and
	// version of that app.
	PTNMODULEID						pModuleID;

	// Pointer to root testnet path.  Must end in a backslash.
	char*							pszTestNetRootPath;

	// Pointer to string that holds additional user specified filtering string used
	// to select a specific master/session.  Optional.
	char*							pszSessionFilter;

	// ID of desired control method (TN_CTRLMETHOD_TCPIP, etc.).  Required.
	DWORD							dwControlMethodID;

	// Additional flags for the control method to to use when joining a
	// master/session.  Control method specific.
	DWORD							dwMethodFlags;

	// Pointer to additional information for the control method to use when
	// creating this slave/joining a session.  Control method specific.
	PVOID							pvMethodData;

	// Size of above additional information.  Control method specific.
	DWORD							dwMethodDataSize;

	// Pointer to control method recognizable string identifying the address
	// of the meta-master to use, if desired.  Optional.  Control method specific.
	// Either this or the pszMasterAddress may be specified, but not both.
	char*							pszMetaMasterAddress;

	// Pointer to control method recognizable string identifying the address
	// of the master to use, if desired.  Optional.  Control method specific.
	// Either this or the pszMetaMasterAddress may be specified, but not both.
	char*							pszMasterAddress;

	// Handle to an manual-reset event that the app can set when it wants to abort
	// or cancel the session.  Optional.
	HANDLE							hUserCancelEvent;

	// How long (in minutes) this slave should run tests before discontinuing, zero
	// to run indefinitely.
	DWORD							dwTimelimit;

	// Array of procedures that will be called to retrieve the total test list.
	// At least one entry required.
	PTNLOADTESTTABLEPROC*			apfnLoadTestTable;

	// Number of procedures in preceding array.  At least one is required.
	DWORD							dwNumLoadTestTables;

	// Procedure that will be called to log a string.  Optional.
	PTNLOGSTRINGPROC				pfnLogString;

	// Procedure that will be called just before joining a session, to allow the
	// slave a chance to load startup data or halt the joining process if it needs
	// to.  Optional.
	PTNDOSTARTUPPROC				pfnDoStartup;

	// Procedure that will be called just before testing begins, to allow the slave
	// a chance to create any objects and do preparation for testing if it needs to.
	// Optional.
	PTNINITIALIZETESTINGPROC		pfnInitializeTesting;

	// Procedure that will be called after testing has completed, to allow the slave
	// to free any resources allocated during testing. Optional.
	PTNCLEANUPTESTINGPROC			pfnCleanupTesting;

	// Procedure that will be called to retrieve the window for APIs that require
	// it.  Optional.
	PTNGETTESTINGWINDOWPROC			pfnGetTestingWindow;

	// Procedure that will be called to retrieve the list of binaries that the
	// module wants to make note of, and/or may swap during a test.  Optional.
	PTNADDIMPORTANTBINARIESPROC		pfnAddImportantBinaries;

	// Procedure that will be called when the module requests a binary set changed.
	// Optional.
	PTNCHANGEBINARYSETPROC			pfnChangeBinarySet;

	// Handle to event to set when joining the control session succeeds/fails, or
	// when the test stats need to be updated.  Optional.
	HANDLE							hCompletionOrUpdateEvent;

	// Pointer to string that holds the path to the directory to use when saving and
	// restoring binary sets.  Optional.
	char*							pszSavedBinsDirPath;

	// Pointer to list of strings describing cases to DebugBreak() on if they fail.
	// Optional.
	PLSTRINGLIST					pFailureBreakRules;

	// Pointer to string that holds the path to the directory to use when dumping
	// the debug log after a test failure.  Optional.
	char*							pszFailureMemDumpDirPath;

	// Indicates whether the client should do database logging.
	BOOL							fLogToDB;

	// Whether CoInitialize (or CoInitializeEx, if available) should be called to
	// setup the testthread.
	BOOL							fInitializeCOMInTestThread;

	// Whether to prompt user for action if test thread has problems shutting down
	// or not.
	BOOL							fPromptTestThreadDieTimeout;

	// Pointer to string that holds the path to the file to use when printing
	// ErrToSrc data.  Optional.
	char*							pszErrToSrcFilepath;

	// Pointer to string that holds a string to search for in ErrToSrc paths.
	// Optional, but pszErrToSrcPathReplace must also be specified.
	char*							pszErrToSrcPathSearch;

	// Pointer to string that holds a string with data to replace with in ErrToSrc
	// paths.  Optional, but pszErrToSrcPathSearch must also be specified.
	char*							pszErrToSrcPathReplace;

	// Pointer to string that holds the path to the file to use when printing
	// documentation data.  Optional.
	char*							pszDocFilepath;

	// Whether to retrieve net stat information on test failure and mem log dump
	// requests or not.
	BOOL							fGetNetStatInfoOnFailure;
} TNJOINCONTROLSESSIONDATA, * PTNJOINCONTROLSESSIONDATA;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct tagTNCREATECONTROLSESSIONDATA
{
	// Size of this structure.
	DWORD							dwSize;

	// The control layer API version the caller intends to use.
	DWORD							dwAPIVersion;

	// The mode this session will be run in (TNMODE_xxx).
	DWORD							dwMode;

	// Pointer to structure that uniquely identifies the calling application and
	// version of that app.
	PTNMODULEID						pModuleID;

	// Pointer to root testnet path.  Must end in a backslash.
	char*							pszTestNetRootPath;

	// Pointer to string that holds additional user specified filtering string used
	// to identify this master/session.  Optional.
	char*							pszSessionFilter;

	// ID of desired control method (TN_CTRLMETHOD_TCPIP, etc.).  Required.
	DWORD							dwControlMethodID;

	// Additional flags for the control method to to use when creating this
	// master/session.  Control method specific.
	DWORD							dwMethodFlags;

	// Pointer to additional information for the control method to use when
	// creating this master/session.  Control method specific.
	PVOID							pvMethodData;

	// Size of above additional information.  Control method specific.
	DWORD							dwMethodDataSize;

	// Pointer to control method recognizable string identifying the address
	// of the meta-master to use, if desired.  Optional.  Control method specific.
	char*							pszMetaMasterAddress;

	// Handle to an manual-reset event that the app can set when it wants to abort
	// or cancel the session.  Optional.
	HANDLE							hUserCancelEvent;

	// Total number of machines expected to join this session (testing will not
	// start until this number is reached).  Set to 0 if it should be a free join
	// session.
	int								iNumSlaves;

	// Number of ticks that the session will remain open after the first joiner
	// to allow additional slaves to join.  Zero if the session should never close.
	// Ignored if iNumTesters (above) is not 0.  Zero is not allowed in API mode.
	DWORD							dwAllowAdditionalJoinersTime;

	// Number of times to repeat the testlist before completing, zero to endlessly
	// loop.
	DWORD							dwReps;

	// How long (in minutes) each slave should run tests before discontinuing, zero
	// to run indefinitely.
	DWORD							dwTimelimit;

	// Array of procedures that will be called to retrieve the total test list.
	// At least one entry required.
	PTNLOADTESTTABLEPROC*			apfnLoadTestTable;

	// Number of procedures in preceding array.  At least one is required.
	DWORD							dwNumLoadTestTables;

	// Pointer to filepath to read the list of tests to run.  Required if not in
	// poke mode.
	char*							pszTestlistFilepath;

	// Procedure that will be called to log a string.  Optional.
	PTNLOGSTRINGPROC				pfnLogString;

	// Procedure that will be called to retrieve startup data for new joiners.
	// Optional.
	PTNGETSTARTUPDATAPROC			pfnGetStartupData;

	// Procedure that will be called to load startup data from a sequence of
	// strings.  Optional.
	PTNLOADSTARTUPDATAPROC			pfnLoadStartupData;

	// Procedure that will be called to write startup data to a sequence of
	// strings.  Optional.
	PTNWRITESTARTUPDATAPROC			pfnWriteStartupData;

	// Procedure that will be called when a UserQuery is received.  Optional.
	PTNHANDLEUSERQUERYPROC			pfnHandleUserQueryProc;

	// Handle to event to set when creating the control session succeeds/fails,
	// or when the test stats need to be updated.  Optional.
	HANDLE							hCompletionOrUpdateEvent;

	// Pointer to list of strings describing cases to tell involved testers to
	// DEBUGBREAK() on if they fail.  Optional.
	PLSTRINGLIST					pFailureBreakRules;

	// Boolean to set to whether we want to ping the slaves or not.  If we do and
	// they don't respond in the set amount of time, they are killed.
	BOOL							fPingSlaves;

	// Path to directory to output a file with all reported events remaining in
	// the log when closing the session.  Optional.
	char*							pszClosingReportDirPath;

	// If specified this points to a directory to have the fellow testers of a slave
	// reporting a failure dump their memory logs to.  If the pointer is not NULL
	// but the string is empty ("") then the slave's default dump dir is used.
	// Optional.
	char*							pszSiblingFailuresMemDumpDirPath;

	// If this path is specified, a list of all the tests that had to be skipped
	// will be printed to the given file.  Optional.
	char*							pszSkippedTestsPath;

	// TRUE if reports generated should be in the BVT standard format or FALSE if
	// not.
	BOOL							fReportInBVTFormat;
} TNCREATECONTROLSESSIONDATA, * PTNCREATECONTROLSESSIONDATA;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct tagTNSTARTUPMETAMASTERDATA
{
	// Size of this structure.
	DWORD						dwSize;

	// The control layer API version the caller intends to use.
	DWORD						dwAPIVersion;

	// ID of desired control method (TN_CTRLMETHOD_TCPIP, etc.).  Required.
	DWORD						dwControlMethodID;

	// Additional flags for the control method to to use when creating this
	// master/session.  Control method specific.
	DWORD						dwMethodFlags;

	// Pointer to additional information for the control method to use when
	// creating this master/session.  Control method specific.
	PVOID						pvMethodData;

	// Size of above additional information.  Control method specific.
	DWORD						dwMethodDataSize;

	// Procedure that will be called to log a string.  Optional.
	PTNLOGSTRINGPROC			pfnLogString;

	// Handle to event to set when meta-master info is updated.  Optional.
	HANDLE						hCompletionOrUpdateEvent;
} TNSTARTUPMETAMASTERDATA, * PTNSTARTUPMETAMASTERDATA;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct tagTNPERFORMLEECHTESTINGDATA
{
	// Size of this structure.
	DWORD							dwSize;

	// The control layer API version the caller intends to use.
	DWORD							dwAPIVersion;

	// Pointer to structure that uniquely identifies the calling application and
	// version of that app.
	PTNMODULEID						pModuleID;

	// Pointer to root testnet path.  Must end in a backslash.
	char*							pszTestNetRootPath;

	// Handle to an manual-reset event that the app can set when it wants to abort
	// or cancel the session.  Optional.
	HANDLE							hUserCancelEvent;

	// Pointer to data to send to hosting slave when connecting.  Optional.
	PVOID							pvSendConnectData;

	// Size of data to send to hosting slave when connecting.  Optional.
	DWORD							dwSendConnectDataSize;

	// Array of procedures that will be called to retrieve the total test list.
	// At least one entry required.
	PTNLOADTESTTABLEPROC*			apfnLoadTestTable;

	// Number of procedures in preceding array.  At least one is required.
	DWORD							dwNumLoadTestTables;

	// Procedure that will be called just before testing begins, to allow the leech
	// a chance to create any objects and do preparation for testing if it needs to.
	// Optional.
	PTNINITIALIZETESTINGPROC		pfnInitializeTesting;

	// Procedure that will be called after testing has completed, to allow the leech
	// to free any resources allocated during testing. Optional.
	PTNCLEANUPTESTINGPROC			pfnCleanupTesting;

	// Procedure that will be called to retrieve the window for APIs that require
	// it.  This window will also receive a WM_CLOSE message when testing is done
	// or aborted.  Optional.
	PTNGETTESTINGWINDOWPROC			pfnGetTestingWindow;

	// ID used to establish connection to slave.
	char* 							pszAttachmentID;

	// Whether the ID used is static or the dynamic one given by the slave.
	BOOL 							fStaticAttachmentID;

	// Whether CoInitialize (or CoInitializeEx, if available) should be called to
	// setup the testthread.
	BOOL							fInitializeCOMInTestThread;
} TNPERFORMLEECHTESTINGDATA, * PTNPERFORMLEECHTESTINGDATA;






//==================================================================================
// Standard headers for the classes
//==================================================================================
#ifdef _XBOX
#include <winsockx.h>
#else
#include <winsock.h>
#endif
#include <time.h>


//==================================================================================
// Other headers for the classes
//==================================================================================
#ifndef _XBOX
#include <tmagent.h>
#endif // ! XBOX

#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\excptns.h"

#include "..\tncommon\fileutils.h"

#include "..\tncommon\crc32.h"




//==================================================================================
// Flesh out the classes
//==================================================================================
#include "macros.h" // not really necessary for classes, but we'll include it here now anyway


#include "msgs.h"

#include "teststats.h"

#include "tableitem.h"
#include "tablecase.h"
#include "tablegroup.h"

#include "syncdata.h"
#include "storedata.h"
#include "sendq.h"
#include "periodicq.h"
#include "jobq.h"
#include "reachcheck.h"

#include "control.h"
#include "ipaddrs.h"
#include "binmgmt.h"
#include "tapidevs.h"
#include "comports.h"
#include "sndcards.h"
#include "info.h"
#include "testlist.h"
#include "vars.h"
#include "rsltchain.h"
#include "ipcobj.h"
#include "leech.h"
#include "faultsim.h"
#include "executor.h"
#include "testfromfiles.h"
#include "testinst.h"
#include "excptns.h"
#include "slave.h"

#include "slavelist.h"
#include "reportlist.h"
#include "master.h"

#include "masterlist.h"
#include "meta.h"




#else //__TNCONTROL__
//#pragma message("__TNCONTROL__ already included!")
#endif //__TNCONTROL__

