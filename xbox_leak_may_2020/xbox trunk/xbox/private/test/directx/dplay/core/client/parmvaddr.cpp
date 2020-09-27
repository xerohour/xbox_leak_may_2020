//==================================================================================
// Includes
//==================================================================================
#include "dptest.h"
#include "macros.h"
#include "wrapbase.h"
#include "parmvalidation.h"

using namespace DPlayCoreNamespace;

namespace DPlayCoreNamespace {

//==================================================================================
// Global vars to gather return data from Threads.
//==================================================================================
struct	ThreadResult
{
	HRESULT			hrThreadResult;
	char			szTestDescript[256];
	char			szResultDescript[256];
};




//==================================================================================
// Defines
//==================================================================================

// Makes sure nobody touched the memory when they weren't supposed to.
#define DONT_TOUCH_MEMORY_PATTERN	0xBADAB00F

#define BUFFERPADDING_SIZE			64


//==================================================================================
// Defines
//==================================================================================
#define FAKE_TCPIPADDRESS	L"nonexistentmachine"
#define FAKE_TCPIPADDRESS_A	"nonexistentmachine"


//==================================================================================
// Constants and defines for URLs
//==================================================================================
// Note this GUID must match GUID_UNKNOWN (see guids.h)
#define ENCODED_UNKNOWN_GUID		L"%7BFDFEFF00-FBFC-F9FA-F8F7-F6F5F4F3F2F1%7D"
#define ENCODED_UNKNOWN_GUID_A		 "%7BFDFEFF00-FBFC-F9FA-F8F7-F6F5F4F3F2F1%7D"


#define ENCODED_TCPIP_GUID			L"%7BEBFE7BA0-628D-11D2-AE0F-006097B01411%7D"
#define ENCODED_TCPIP_GUID_A		 "%7BEBFE7BA0-628D-11D2-AE0F-006097B01411%7D"


// This is data that won't have to be encoded to be stored in an URL.
#define NO_ENCODE_DATA				L"noencodedata"
#define NO_ENCODE_DATA_A			 "noencodedata"

// This is data that will have to be encoded to be stored in an URL.
#define MUST_ENCODE_DATA			L"please en{ode\tdata\n"
#define MUST_ENCODE_DATA_A			 "please en{ode\tdata\n"

#define ENCODED_MUST_ENCODE_DATA	L"please%20en%7Bode%09data%0A"
#define ENCODED_MUST_ENCODE_DATA_A	 "please%20en%7Bode%09data%0A"


#define MISCELLANEOUS_KEY1			L"misckey1"
#define MISCELLANEOUS_KEY1_A		 "misckey1"
#define MISCELLANEOUS_KEY2			L"misckey2"
#define MISCELLANEOUS_KEY2_A		 "misckey2"

#define MISCELLANEOUS_VALUE1		L"miscvalue1"
#define MISCELLANEOUS_VALUE1_A		 "miscvalue1"
#define MISCELLANEOUS_VALUE2		L"miscvalue2"
#define MISCELLANEOUS_VALUE2_A		 "miscvalue2"



#define BINARYDATA_KEY				L"binkey"
#define BINARYDATA_KEY_A			 "binkey"

const BYTE		c_abBinaryData[16] = {0xEE, 0xBE, 0xEE, 0xBE,
										0x0F, 0xB0, 0x0F, 0xB0,
										0xAC, 0xAC, 0xAC, 0xAC,
										0xCE, 0xFA, 0x11, 0xD0};

#define ENCODED_BINARYDATA			L"%EE%BE%EE%BE%0F%B0%0F%B0%AC%AC%AC%AC%CE%FA%11%D0"
#define ENCODED_BINARYDATA_A		 "%EE%BE%EE%BE%0F%B0%0F%B0%AC%AC%AC%AC%CE%FA%11%D0"




//==================================================================================
// Prototypes
//==================================================================================
//HRESULT CreateInterface_Addr(HANDLE hLog);
//HRESULT ParmVExec_BQI(HANDLE hLog);
HRESULT ParmVExec_BBuildW(HANDLE hLog);
HRESULT ParmVExec_BBuildA(HANDLE hLog);
HRESULT ParmVExec_BDuplicate(HANDLE hLog);
HRESULT ParmVExec_BSetEqual(HANDLE hLog);
HRESULT ParmVExec_BIsEqual(HANDLE hLog);
HRESULT ParmVExec_BClear(HANDLE hLog);
HRESULT ParmVExec_BGetURLW(HANDLE hLog);
HRESULT ParmVExec_BGetURLA(HANDLE hLog);
HRESULT ParmVExec_BGetSP(HANDLE hLog);
HRESULT ParmVExec_BGetUserData(HANDLE hLog);
HRESULT ParmVExec_BSetSP(HANDLE hLog);
HRESULT ParmVExec_BSetUserData(HANDLE hLog);
HRESULT ParmVExec_BGetNumComponents(HANDLE hLog);
HRESULT ParmVExec_BGetByName(HANDLE hLog);
HRESULT ParmVExec_BGetByIndex(HANDLE hLog);
HRESULT ParmVExec_BAdd(HANDLE hLog);
HRESULT ParmVExec_BGetDevice(HANDLE hLog);
HRESULT ParmVExec_BSetDevice(HANDLE hLog);
//HRESULT ParmVExec_BLockUnlock(HANDLE hLog);
//HRESULT ParmVExec_BBuildDPADDR(HANDLE hLog);

//Seperate thread that is used to make sure coinit is not being called before DirectPlay8Create.
//HRESULT AddressingCreateThread(PVOID thrVoid);


/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmV_LoadTestTable()"
//==================================================================================
// ParmV_LoadTestTable
//----------------------------------------------------------------------------------
//
// Description: Loads all the possible tests into the table passed in:
//				2.1			Parameter validation tests
//				2.1.1		Base interface parameter validation tests
//				2.1.1.1		Base CreateDplay8Address parameter validation
//				2.1.1.2		Base QueryInterface parameter validation
//				2.1.1.3		Base BuildFromURLW parameter validation
//				2.1.1.4		Base BuildFromURLA parameter validation
//				2.1.1.5		Base Duplicate parameter validation
//				2.1.1.6		Base Set Equal parameter validation
//				2.1.1.7		Base Is Equal parameter validation
//				2.1.1.8		Base Clear parameter validation
//				2.1.1.9		Base GetURLW parameter validation
//				2.1.1.10	Base GetURLW parameter validation
//				2.1.1.11	Base GetSP parameter validation
//				2.1.1.12	Base GetUserData parameter validation
//				2.1.1.13	Base SetSP parameter validation
//				2.1.1.14	Base SetUserData parameter validation
//				2.1.1.15	Base GetNumComponents parameter validation
//				2.1.1.16	Base GetComponentByName parameter validation
//				2.1.1.17	Base GetComponentByIndex parameter validation
//				2.1.1.18	Base AddComponent parameter validation
//				2.1.1.19	Base GetDevice parameter validation
//				2.1.1.20	Base SetDevice parameter validation
//				2.1.1.21	Base Lock and UnLock parameter validation
//				2.1.1.22	Base Build From DPlay4 Addr parameter validation
//
// Arguments:
//	PTNLOADTESTTABLEDATA pTNlttd	Pointer to data to use when loading the tests.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ParmV_LoadTestTable(PTNLOADTESTTABLEDATA pTNlttd)
{
	HRESULT				hr = S_OK;
	PTNTESTTABLEGROUP	pParmVTests = NULL;
	PTNTESTTABLEGROUP	pSubGroup = NULL;
	TNTESTCASEPROCS		procs;



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1		Parameter validation tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	pParmVTests = pTNlttd->pBase->NewGroup("2.1", "Parameter validation tests");
	if (pParmVTests == NULL)
	{
		DPTEST_FAIL(hLog, "Couldn't create a new grouping!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't create a new grouping)



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.1.1	Base interface parameter validation tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	pSubGroup = pParmVTests->NewGroup("2.1.1", "Base interface parameter validation tests");
	if (pSubGroup == NULL)
	{
		DPTEST_FAIL(hLog, "Couldn't create a new subgrouping!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't create a new subgrouping)


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = CreateInterface_Addr;

	hr = pSubGroup->AddTest(
	"2.1.1.1",	"CreateDplay8Address Interface parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;

	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BQI;

	hr = pSubGroup->AddTest(
	"2.1.1.2",	"Base QueryInterface parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BBuildW;

	hr = pSubGroup->AddTest(
	"2.1.1.3",	"Base BuildFromURLW parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;

	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BBuildA;

	hr = pSubGroup->AddTest(
	"2.1.1.4",	"Base BuildFromURLW parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BDuplicate;

	hr = pSubGroup->AddTest(
	"2.1.1.5",	"Base Duplicate parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BSetEqual;

	hr = pSubGroup->AddTest(
	"2.1.1.6",	"Base Set Equal parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BIsEqual;

	hr = pSubGroup->AddTest(
	"2.1.1.7",	"Base Is Equal parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BClear;

	hr = pSubGroup->AddTest(
	"2.1.1.8",	"Base Clear parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BGetURLW;

	hr = pSubGroup->AddTest(
	"2.1.1.9",	"Base GetURLW parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BGetURLA;

	hr = pSubGroup->AddTest(
	"2.1.1.10",	"Base GetURLA parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;

	
	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BGetSP;

	hr = pSubGroup->AddTest(
	"2.1.1.11",	"Base GetSP parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BGetUserData;

	hr = pSubGroup->AddTest(
	"2.1.1.12",	"Base GetUserData parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BSetSP;

	hr = pSubGroup->AddTest(
	"2.1.1.13",	"Base SetSP parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BSetUserData;

	hr = pSubGroup->AddTest(
	"2.1.1.14",	"Base SetUserData parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BGetNumComponents;

	hr = pSubGroup->AddTest(
	"2.1.1.15",	"Base GetNumComponents parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BGetByName;

	hr = pSubGroup->AddTest(
	"2.1.1.16",	"Base GetComponentByName parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BGetByIndex;

	hr = pSubGroup->AddTest(
	"2.1.1.17",	"Base GetComponentByIndex parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BAdd;

	hr = pSubGroup->AddTest(
	"2.1.1.18",	"Base AddComponent parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BGetDevice;

	hr = pSubGroup->AddTest(
	"2.1.1.19",	"Base GetDevice parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BSetDevice;

	hr = pSubGroup->AddTest(
	"2.1.1.20",	"Base SetDevice parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;

	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BLockUnlock;

	hr = pSubGroup->AddTest(
	"2.1.1.21",	"Base Lock and Unlock parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;

	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_BBuildDPADDR;

	hr = pSubGroup->AddTest(
	"2.1.1.22",	"Base Lock and Unlock parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;

DONE:

	return (hr);
} // ParmVLoadTestTable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/

/* XBOX - Manually creating address objects, this function isn't required
//==================================================================================
// Create Address Interface
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.1 - CreateDirectPlay8Address Parameter Validation 
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: DPN_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT CreateInterface_Addr(HANDLE hLog)
{
	CTNSystemResult						sr;
	CTNTestResult						tr;
	
	HANDLE								hThreadHandle = NULL;
	DWORD								dwThreadID = NULL;
	
	ThreadResult						thrCreateApp;						

	BEGIN_TESTCASE
	{	
		//Initialize the thread result structure.
		thrCreateApp.hrThreadResult = DPN_OK;
		thrCreateApp.szTestDescript[0] = '\n';
		thrCreateApp.szResultDescript[0] = '\n';
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Create a new thread to ensure we are not calling CoInitialize!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//Need to Throw this on another thread to ensure LobbyCreate works without CoInitialize.
		hThreadHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)AddressingCreateThread,
										 (PVOID)&thrCreateApp, NULL, &dwThreadID);  

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Wait for the Application thread we just created!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		if(WaitForSingleObject(hThreadHandle, 60000) != WAIT_OBJECT_0)
		{
			DPTEST_FAIL(hLog, "Valid Create did not return within 60 seconds.", 0);
			THROW_TESTRESULT;
		}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Check the results from the thread that was just run!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION(thrCreateApp.szTestDescript);
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		if(thrCreateApp.hrThreadResult != DPN_OK)
		{
			//Print the results from the thread run.
			tr = thrCreateApp.hrThreadResult;
			DPTEST_FAIL(hLog, thrCreateApp.szResultDescript, 0);
			THROW_TESTRESULT;
		}
		
		//Dummy if that is put in to avoid compiler issue without calling Throw_SystemResult
		TESTSECTION_IF(0)
		{
			THROW_SYSTEMRESULT;
		}
		TESTSECTION_ENDIF

		FINAL_SUCCESS;
	}
	END_TESTCASE

//Cleanup:
		
	if(hThreadHandle)
	{
		CloseHandle(hThreadHandle);
	}

	return (sr);
}
*/

/* XBOX - Manually creating address objects, this function isn't required
//==================================================================================
// AddressingCreateThread
//----------------------------------------------------------------------------------
//
// Description: Addressing Thread used verify AddressCreate 
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: DPN_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT AddressingCreateThread(PVOID thrVoid)
{
	HRESULT								hr = DPN_OK;
	PDIRECTPLAY8ADDRESS					pDPAddr	= NULL;

	ThreadResult*						thrResult = (ThreadResult*)thrVoid;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	strcpy(thrResult->szTestDescript, "DirectPlay8AddressCreate of IUnknown Interface");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	hr = DirectPlay8AddressCreate(&IID_IUnknown,
									(PVOID*)&pDPAddr,
									NULL);

	if(hr != DPNERR_INVALIDPARAM)
	{
		strcpy(thrResult->szResultDescript, "Create of IUnknown did not return INVALIDPARAM.");
		thrResult->hrThreadResult = hr;
		goto Cleanup;
	} // end if (failed executing sub test case)

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	strcpy(thrResult->szTestDescript, "DirectPlay8AddressCreate of GUID_NULL Interface");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	hr = DirectPlay8AddressCreate(&GUID_NULL,
									(PVOID*)&pDPAddr,
									NULL);
	if(hr != DPNERR_INVALIDPARAM)
	{
		strcpy(thrResult->szResultDescript, "Create of NULL GUID did not return DPNERR_INVALIDPARAM.");
		thrResult->hrThreadResult = hr;
		goto Cleanup;
	} // end if (failed executing sub test case)

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	strcpy(thrResult->szTestDescript, "Valid DirectPlay8AddressCreate");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	hr = DirectPlay8AddressCreate(&IID_IDirectPlay8Address,
								  (PVOID*)&pDPAddr,
								  NULL);

	if(hr != DPN_OK)
	{
		strcpy(thrResult->szResultDescript, "Valid Create did not return DPN_OK.");
		thrResult->hrThreadResult = hr;
		goto Cleanup;
	} // end if (failed executing sub test case)

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	strcpy(thrResult->szTestDescript, "Build from Unicode URL just the header.");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	hr = pDPAddr->BuildFromURLW(DPNA_HEADER);
	if (hr != DPN_OK)
	{
		strcpy(thrResult->szResultDescript, "Building from Unicode URL using header failed.");
		thrResult->hrThreadResult = hr;
		goto Cleanup;
	} // end if (failed building from W URL)

	
	//We were successful ... set hr = DPN_OK
	strcpy(thrResult->szTestDescript, "Addressing App Create Tests finished successfully.");
	strcpy(thrResult->szResultDescript, "Addressing App Create Tests finished successfully.");
	thrResult->hrThreadResult = DPN_OK;

Cleanup:

	if(pDPAddr != NULL)
	{
		pDPAddr->Release();
		pDPAddr = NULL;
	}
	
	return hr;
}
*/



/* XBOX - QueryInterface not available
#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BQI()"
//==================================================================================
// ParmVExec_BQI
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.2 - Base QueryInterface parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BQI(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
	PWRAPDP8ADDRESS					pDP8Address = NULL;
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
	IUnknown*						pUnknown = NULL;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->ExecSubTestCase("1.1.1.1", NULL, 0,
												1, pTNecd->iTesterNum);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:CCIWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.1.1.1",
									"Creating DirectPlay8Address object failed!");

		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pvSubOutputData,
									dwSubOutputDataSize,
									sizeof (TOD_ABASICS_CCIWRAP));
		pDP8Address = ((PTOD_ABASICS_CCIWRAP) pvSubOutputData)->pDP8Address;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for IID_IDirectPlay8Address, NULL dest pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8Address, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Querying with IID_IDirectPlay8Address and NULL ptr didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for GUID_NULL, NULL dest pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(GUID_NULL, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Querying with GUID_NULL and NULL ptr didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for unknown GUID, NULL dest pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(GUID_UNKNOWN, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Querying with unknown GUID and NULL ptr didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for IID_IDirectPlay8Address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8Address,
											(PVOID*) &pUnknown);
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Querying for IID_IDirectPlay8Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)

		if (pUnknown == NULL)
		{
			DPTEST_FAIL(hLog, "Querying for IID_IDirectPlay8Address didn't return pointer!", 0);
			SETTHROW_TESTRESULT(E_NOINTERFACE);
		} // end if (didn't get pointer)

		pUnknown->Release();
		pUnknown = NULL;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for IID_IDirectPlay8AddressIP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressIP,
											(PVOID*) &pUnknown);
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Querying for IID_IDirectPlay8AddressIP failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)

		if (pUnknown == NULL)
		{
			DPTEST_FAIL(hLog, "Querying for IID_IDirectPlay8AddressIP didn't return pointer!", 0);
			SETTHROW_TESTRESULT(E_NOINTERFACE);
		} // end if (didn't get pointer)

		pUnknown->Release();
		pUnknown = NULL;

#pragma TODO(seanwh, "QI for these interfaces has not been implemented yet!")
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for IID_IDirectPlay8AddressIPX");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressIPX,
											(PVOID*) &pUnknown);
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Querying for IID_IDirectPlay8AddressIPX failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)

		if (pUnknown == NULL)
		{
			DPTEST_FAIL(hLog, "Querying for IID_IDirectPlay8AddressIPX didn't return pointer!", 0);
			SETTHROW_TESTRESULT(E_NOINTERFACE);
		} // end if (didn't get pointer)

		pUnknown->Release();
		pUnknown = NULL;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for IID_IDirectPlay8AddressModem");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressModem,
											(PVOID*) &pUnknown);
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Querying for IID_IDirectPlay8AddressModem failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)

		if (pUnknown == NULL)
		{
			DPTEST_FAIL(hLog, "Querying for IID_IDirectPlay8AddressModem didn't return pointer!", 0);
			SETTHROW_TESTRESULT(E_NOINTERFACE);
		} // end if (didn't get pointer)

		pUnknown->Release();
		pUnknown = NULL;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for IID_IDirectPlay8AddressSerial");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressSerial,
											(PVOID*) &pUnknown);
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Querying for IID_IDirectPlay8AddressSerial failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)

		if (pUnknown == NULL)
		{
			DPTEST_FAIL(hLog, "Querying for IID_IDirectPlay8AddressSerial didn't return pointer!", 0);
			SETTHROW_TESTRESULT(E_NOINTERFACE);
		} // end if (didn't get pointer)

		pUnknown->Release();
		pUnknown = NULL;


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for GUID_NULL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(GUID_NULL, (PVOID*) &pUnknown);
		if (tr != DPNERR_NOINTERFACE)
		{
			DPTEST_FAIL(hLog, "Querying for GUID_NULL didn't return expected error NOINTERFACE!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)

		if (pUnknown != NULL)
		{
			DPTEST_FAIL(hLog, "Querying for invalid interface returned an interface pointer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (got a pointer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for garbage GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(GUID_UNKNOWN, (PVOID*) &pUnknown);
		if (tr != DPNERR_NOINTERFACE)
		{
			DPTEST_FAIL(hLog, "Querying for unknown GUID didn't return expected error NOINTERFACE!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)

		if (pUnknown != NULL)
		{
			DPTEST_FAIL(hLog, "Querying for invalid interface returned an interface pointer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (got a pointer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)

		if (pDP8AddressInternal == NULL)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface didn't return pointer!", 0);
			SETTHROW_TESTRESULT(E_NOINTERFACE);
		} // end if (didn't get pointer)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for IID_IDirectPlay8Address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8Address,
											(PVOID*) &pUnknown);
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Querying for IID_IDirectPlay8Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)

		if (pUnknown == NULL)
		{
			DPTEST_FAIL(hLog, "Querying for IID_IDirectPlay8Address didn't return pointer!", 0);
			SETTHROW_TESTRESULT(E_NOINTERFACE);
		} // end if (didn't get pointer)

		pUnknown->Release();
		pUnknown = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_ABASICS_RELEASEWRAP));
		((PTID_ABASICS_RELEASEWRAP) pvSubInputData)->pDP8Address = pDP8Address;

		sr = pTNecd->pExecutor->ExecSubTestCase("1.1.2.1", pvSubInputData,
												sizeof (TID_ABASICS_RELEASEWRAP),
												1, pTNecd->iTesterNum);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:ReleaseWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.1.2.1",
									"Releasing DirectPlay8Address object failed!");

		pDP8Address = NULL;


		FINAL_SUCCESS;
	}
	END_TESTCASE


	SAFE_LOCALFREE(pvSubInputData);
	SAFE_RELEASE(pDP8AddressInternal);

	if (pUnknown != NULL)
	{
		pUnknown->Release();
		pUnknown = NULL;
	} // end if (have random interface)

	if (pDP8Address != NULL)
	{
		// Ignore error
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BQI
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BBuildW()"
//==================================================================================
// ParmVExec_BBuildW
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.3 - Base BuildFromURLW parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BBuildW(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/

	PDIRECTPLAY8ADDRESS				pNonwrappedDP8Address = NULL;

	WCHAR							wszTempString[256];
	DWORD							dwSizeofTempString = NULL;
	
	WCHAR*							pwszURL = NULL;
	DWORD							dwURLLen = NULL;
	
	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("DirectPlay8AddressCreate for another unwrapped interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pNonwrappedDP8Address, NULL);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "DirectPlay8AddressCreate for another unwrapped interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed creating address)

/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)
*/

#pragma BUGBUG(vanceo, "Figure out how to work around windbg still breaking on this")
		/*
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using invalid pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW((WCHAR*) 0x00000666);
		if (tr != DPNERR_INVALIDSTRING)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using invalid pointer didn't return expected error INVALIDSTRING!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)
		*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using empty string");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(L"");
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using invalid pointer didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using provider key name");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_KEY_PROVIDER);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using provider key name didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider key name");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider key name didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with no value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=");
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with no value didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with invalid value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=NOT_A_GUID");
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with invalid value didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with TCP/IP GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with TCP/IP GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID - double equal sign");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"==" ENCODED_UNKNOWN_GUID);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID - double equal sign didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using provider with unknown GUID & header");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID DPNA_HEADER);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using provider with unknown GUID & header didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID and semicolon");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";");
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID and semicolon didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, no ';' no-encode user data");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L"#" NO_ENCODE_DATA);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, no ';' no-encode user data failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, no-encode user data");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";#" NO_ENCODE_DATA);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, no-encode user data failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// This is invalid because there's extra crap not part of the guid

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID + extraneous junk");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L"extraneousjunk");
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID + extraneous junk didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// This is invalid because there's extra crap not part of the guid

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, extraneous junk + provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" L"extraneousjunk" ENCODED_UNKNOWN_GUID);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, extraneous junk + provider with unknown GUID didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)

		


		// This is invalid because there's extra crap not part of the guid and
		// the header vs value is off

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, no separator, misc key & value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, no separator, misc key & value didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, misc key & value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, misc key & value failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, provider with TCP/IP GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, provider with TCP/IP GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, misc key & value, provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1 L";" DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, misc key & value, provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, misc key & must encode value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MUST_ENCODE_DATA);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, misc key & must encode value didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, DPNA_VALUE_TCPIPPROVIDER & miscellaneous key and value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER 
											L"=" DPNA_VALUE_TCPIPPROVIDER
											L";" MISCELLANEOUS_KEY1 
											L"=" MISCELLANEOUS_VALUE1); 
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, DPNA_VALUE_TCPIPPROVIDER & miscellaneous key and value failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)
	

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting size of Unicode URL built using header, DPNA_VALUE_TCPIPPROVIDER & miscellaneous key and value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		wcscpy(wszTempString, 
			   DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1 L"\0");
		
		dwSizeofTempString = (wcslen(wszTempString)+1);

		dwURLLen = 0;

		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwSizeofTempString)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwSizeofTempString);
			DPTEST_FAIL(hLog, "Xbox bug 1879 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify URL that we created using DPNA_VALUE_TCPIPPROVIDER");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwSizeofTempString)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwSizeofTempString);
			DPTEST_FAIL(hLog, "Xbox bug 1879 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, wszTempString) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, wszTempString);
			DPTEST_FAIL(hLog, "Xbox bug 1879 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;


//XBox bug, repo code to verify that the PC version of DirectPlay is not the issue.
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Data in our passed in address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		wsprintfW(wszTempString, L"%sblahblah=sdfkjsadfhksjadhfkjhsdfjhsfsdhf", DPNA_HEADER);
		//wcscpy(pwszURL, L"%sblahblah=sdfkjsadfhksjadhfkjhsdfjhsfsdhf", DPNA_HEADER);
    
		tr = pNonwrappedDP8Address->BuildFromURLW(wszTempString);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building passed in Address from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Equal our address object to a wrapped object.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_SetEqual(pNonwrappedDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "SetEqual Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting size of Unicode URL built using header, followed by junk info and junk value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSizeofTempString = (wcslen(wszTempString)+1);

		dwURLLen = 0;

		tr = pDP8Address->DPA_GetURLW(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwSizeofTempString)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwSizeofTempString);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify URL that we created using DPNA_VALUE_TCPIPPROVIDER");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwSizeofTempString)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwSizeofTempString);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, wszTempString) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, wszTempString);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Data in our passed in address object to a smaller string");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		wsprintfW(wszTempString, L"%smark=meme", DPNA_HEADER );
		//wcscpy(pwszURL, L"%sblahblah=sdfkjsadfhksjadhfkjhsdfjhsfsdhf", DPNA_HEADER);
    
		tr = pNonwrappedDP8Address->BuildFromURLW(wszTempString);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building passed in Address from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Equal our address object to a wrapped object.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_SetEqual(pNonwrappedDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "SetEqual Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting size of Unicode URL built using header, followed by junk info and junk value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSizeofTempString = (wcslen(wszTempString)+1);

		dwURLLen = 0;

		tr = pDP8Address->DPA_GetURLW(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwSizeofTempString)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwSizeofTempString);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify URL that we created using DPNA_VALUE_TCPIPPROVIDER");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwSizeofTempString)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwSizeofTempString);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, wszTempString) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, wszTempString);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;
		
		
/* End of XBox bug case. */
		

/* XBOX - DPNA_VALUE_IPXPROVIDER doesn't exist on Xbox
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, DPNA_VALUE_IPXPROVIDER & miscellaneous key and value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER 
											L"=" DPNA_VALUE_IPXPROVIDER
											L";" MISCELLANEOUS_KEY1 
											L"=" MISCELLANEOUS_VALUE1); 
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, DPNA_VALUE_IPXPROVIDER & miscellaneous key and value failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)
*/

/* XBOX - DPNA_VALUE_MODEMPROVIDER doesn't exist on Xbox
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, DPNA_VALUE_MODEMPROVIDER & miscellaneous key and value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER 
											L"=" DPNA_VALUE_MODEMPROVIDER
											L";" MISCELLANEOUS_KEY1 
											L"=" MISCELLANEOUS_VALUE1); 
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, DPNA_VALUE_MODEMPROVIDER & miscellaneous key and value failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)
*/

/* XBOX - DPNA_VALUE_SERIALPROVIDER doesn't exist on Xbox
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, DPNA_VALUE_SERIALPROVIDER & miscellaneous key and value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER 
											L"=" DPNA_VALUE_SERIALPROVIDER
											L";" MISCELLANEOUS_KEY1 
											L"=" MISCELLANEOUS_VALUE1); 
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, DPNA_VALUE_SERIALPROVIDER & miscellaneous key and value failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, misc key & value while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, misc key & value while locked didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, misc key & value unlocked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, misc key & value unlocked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing non-wrapped interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		pNonwrappedDP8Address->Release();
		pNonwrappedDP8Address = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/
	SAFE_RELEASE(pNonwrappedDP8Address);

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BBuildW
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BBuildA()"
//==================================================================================
// ParmVExec_BBuildA
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.4 - Base BuildFromURLA parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BBuildA(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)
*/



#pragma BUGBUG(vanceo, "Figure out how to work around windbg still breaking on this")
		/*
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using invalid pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA((char*)0x00000666);
		if (tr != DPNERR_INVALIDSTRING)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using invalid pointer didn't return expected error INVALIDSTRING!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)
		*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using empty string");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA("");
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using invalid pointer didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using provider key name");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_KEY_PROVIDER_A);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using provider key name didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header & provider key name");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header & provider key name didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header & provider with no value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=");
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header & provider with no value didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header & provider with invalid value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=NOT_A_GUID");
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header & provider with invalid value didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header & provider with TCP/IP GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_TCPIP_GUID_A);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header & provider with TCP/IP GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header & provider with unknown GUID - double equal sign");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "==" ENCODED_UNKNOWN_GUID_A);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header & provider with unknown GUID - double equal sign didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using provider with unknown GUID & header");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A DPNA_HEADER_A);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using provider with unknown GUID & header didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header & provider with unknown GUID and semicolon");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A ";");
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header & provider with unknown GUID and semicolon didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header, provider with unknown GUID, no ';' no-encode user data");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A "#" NO_ENCODE_DATA_A);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header, provider with unknown GUID, no ';' no-encode user data failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header, provider with unknown GUID, no-encode user data");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A ";#" NO_ENCODE_DATA_A);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header, provider with unknown GUID, no-encode user data failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// This is invalid because there's extra crap not part of the guid

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header, provider with unknown GUID + extraneous junk");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A "extraneousjunk");
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header, provider with unknown GUID + extraneous junk didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// This is invalid because there's extra crap not part of the guid

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header, extraneous junk + provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" "extraneousjunk" ENCODED_UNKNOWN_GUID_A);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header, extraneous junk + provider with unknown GUID didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)

		


		// This is invalid because there's extra crap not part of the guid and
		// the header vs value is off

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header, provider with unknown GUID, no separator, misc key & value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A MISCELLANEOUS_KEY1_A "=" MISCELLANEOUS_VALUE1_A);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header, provider with unknown GUID, no separator, misc key & value didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header, provider with unknown GUID, misc key & value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A ";" MISCELLANEOUS_KEY1_A "=" MISCELLANEOUS_VALUE1_A);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header, provider with unknown GUID, misc key & value failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header, provider with unknown GUID, provider with TCP/IP GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A ";" DPNA_KEY_PROVIDER_A "=" ENCODED_TCPIP_GUID_A);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header, provider with unknown GUID, provider with TCP/IP GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header, misc key & value, provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A MISCELLANEOUS_KEY1_A "=" MISCELLANEOUS_VALUE1_A ";" DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header, misc key & value, provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header, misc key & must encode value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A MISCELLANEOUS_KEY1_A "=" MUST_ENCODE_DATA_A);
		if (tr != DPNERR_INVALIDURL)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header, misc key & must encode value didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)





/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header, provider with unknown GUID, misc key & value while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A ";" MISCELLANEOUS_KEY1_A "=" MISCELLANEOUS_VALUE1_A);
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header, provider with unknown GUID, misc key & value while locked didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header, provider with unknown GUID, misc key & value unlocked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A ";" MISCELLANEOUS_KEY1_A "=" MISCELLANEOUS_VALUE1_A);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header, provider with unknown GUID, misc key & value unlocked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BBuildA
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BDuplicate()"
//==================================================================================
// ParmVExec_BDuplicate
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.5 - Base Duplicate parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BDuplicate(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/
	PDIRECTPLAY8ADDRESS				pNonwrappedDP8Address = NULL;
	WCHAR*							pwszURL = NULL;
	DWORD							dwURLLen;
	DWORD							dwExpectedURLLen;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Duplicating with NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Duplicate(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Duplicating using NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)
*/


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Duplicating without object being set");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Duplicate(&pNonwrappedDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Duplicating without object being set failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Didn't get a valid object pointer returned!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)



		// We don't care about the copy, release it

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing duplicate");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedDP8Address->Release();
		pNonwrappedDP8Address = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Duplicating with object being set");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Duplicate(&pNonwrappedDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Duplicating with object being set failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Didn't get a valid object pointer returned!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)

		DPTEST_TRACE(hLog, "Got duplicate object at %x.", 1, pNonwrappedDP8Address);




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting duplicate object Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID) + 1;

		tr = pNonwrappedDP8Address->GetURLW(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting duplicate object Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pNonwrappedDP8Address->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Re-building original object from Unicode URL using header & misc key 1");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building original object from Unicode URL using header & misc key 1 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting duplicate object Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pNonwrappedDP8Address->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;




		// We're done with the copy, release it

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing duplicate");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedDP8Address->Release();
		pNonwrappedDP8Address = NULL;







/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Duplicating with object being set while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Duplicate(&pNonwrappedDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Duplicating with object being set while locked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Didn't get a valid object pointer returned!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)



		// We don't care about the copy, release it

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing duplicate");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedDP8Address->Release();
		pNonwrappedDP8Address = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/
	SAFE_RELEASE(pNonwrappedDP8Address);
	SAFE_LOCALFREE(pwszURL);

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BDuplicate
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BSetEqual()"
//==================================================================================
// ParmVExec_BSetEqual
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.6 - Base Set Equal parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BSetEqual(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/
	PDIRECTPLAY8ADDRESS				pNonwrappedDP8Address = NULL;
	WCHAR*							pwszURL = NULL;
	DWORD							dwURLLen;
	DWORD							dwExpectedURLLen;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("SetEqual with NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetEqual(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "SetEqual using NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("SetEqual to object without being set");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetEqual(pNonwrappedDP8Address);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "SetEqual without object being set didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address != NULL)
		{
			DPTEST_FAIL(hLog, "Data returned with invalid object pointer!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("DirectPlay8AddressCreate for another unwrapped interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pNonwrappedDP8Address, NULL);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "DirectPlay8AddressCreate for another unwrapped interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed creating address)


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Equal from an Empty Object to an Empty Object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_SetEqual(pNonwrappedDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Set Equal from an Empty Object to an Empty Object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Object pointer cleared when being set with Empty Address Object!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Data in our passed in address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pNonwrappedDP8Address->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building passed in Address from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Equal from pass in Object to an Empty Object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_SetEqual(pNonwrappedDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Set Equal from an Valid Object to an Empty Object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Object pointer cleared when being set with Empty Address Object!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting SetEqual main object Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID) + 1;

		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting duplicate object Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;


		/* Pass in a smaller URL in the SetEqual API. */

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Data in our passed in address object to just a header");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pNonwrappedDP8Address->BuildFromURLW(DPNA_HEADER);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building passed in Address from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Equal from pass in Object to an Empty Object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_SetEqual(pNonwrappedDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Set Equal from an Valid Object to an Empty Object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Object pointer cleared when being set with Empty Address Object!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting SetEqual main object Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = wcslen(DPNA_HEADER) + 1;

		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting duplicate object Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, DPNA_HEADER) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Re-building original object from Unicode URL using header & misc key 1");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pNonwrappedDP8Address->BuildFromURLW(DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building passed in object from Unicode URL using header & misc key 1 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		
		
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Equal Object and overwrite current Object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_SetEqual(pNonwrappedDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Set Equal from an Valid Object to overwrite Object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Object pointer cleared when being set with Empty Address Object!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting SetEqual main object Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = wcslen(DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1) + 1;

		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting duplicate object Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL,
				DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;


/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Duplicating with object being set while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetEqual(pNonwrappedDP8Address);
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "SetEqual with object being set while locked didn't return DPNERR_NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Passed in object was set to NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)



		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("SetEqual after object being set while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetEqual(pNonwrappedDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "SetEqual after object being locked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Passed in object was set to NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/

		// We don't care about the copy, release it

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing Nonwrapped object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedDP8Address->Release();
		pNonwrappedDP8Address = NULL;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/
	SAFE_RELEASE(pNonwrappedDP8Address);
	SAFE_LOCALFREE(pwszURL);

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BSetEqual
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BIsEqual()"
//==================================================================================
// ParmVExec_BIsEqual
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.7 - Base Is Equal parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BIsEqual(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/
	PDIRECTPLAY8ADDRESS				pNonwrappedDP8Address = NULL;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("IsEqual with NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_IsEqual(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "IsEqual using NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("IsEqual to object without being created");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_IsEqual(pNonwrappedDP8Address);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "IsEqual without object being set didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address != NULL)
		{
			DPTEST_FAIL(hLog, "Data returned with invalid object pointer!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("DirectPlay8AddressCreate for another unwrapped interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pNonwrappedDP8Address, NULL);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "DirectPlay8AddressCreate for another unwrapped interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed creating address)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Is Equal from an Empty Object to an Empty Object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_IsEqual(pNonwrappedDP8Address);
		if (tr != DPNSUCCESS_EQUAL)
		{
			DPTEST_FAIL(hLog, "Is Equal from an Empty Object to an Empty Object did not return DPNSUCCESS_EQUAL!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Object pointer cleared when being set with Empty Address Object!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Data in our passed in address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pNonwrappedDP8Address->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building passed in Address from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Is Equal from pass in Object to an Empty Object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_IsEqual(pNonwrappedDP8Address);
		if (tr != DPNSUCCESS_NOTEQUAL)
		{
			DPTEST_FAIL(hLog, "IsEqual from an Valid Object to an Empty Object didn't return DPNSUCCESS_NOTEQUAL!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Object pointer cleared when being set with Empty Address Object!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Data in our main address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building main Address from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Is Equal from pass in Object to an equal Set Object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_IsEqual(pNonwrappedDP8Address);
		if (tr != DPNSUCCESS_EQUAL)
		{
			DPTEST_FAIL(hLog, "IsEqual from an Valid Object to a Same set Object didn't return DPNSUCCESS_EQUAL!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Object pointer cleared when being set with Empty Address Object!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Re-building original object from Unicode URL using header & misc key 1");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building passed in object from Unicode URL using header & misc key 1 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Is Equal from pass in Object to an different Set Object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_IsEqual(pNonwrappedDP8Address);
		if (tr != DPNSUCCESS_NOTEQUAL)
		{
			DPTEST_FAIL(hLog, "IsEqual from an Valid Object to a Dirrerent set Object didn't return DPNSUCCESS_NOTEQUAL!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Object pointer cleared when being set with Empty Address Object!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)





/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("IsEqual with object being set while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_IsEqual(pNonwrappedDP8Address);
		if (tr != DPNSUCCESS_NOTEQUAL)
		{
			DPTEST_FAIL(hLog, "IsEqual with object being set while locked didn't return DPNSUCCESS_NOTEQUAL!", 0);
			THROW_TESTRESULT;
		} // end if (failed IsEqual)

		if (pNonwrappedDP8Address == NULL)
		{
			DPTEST_FAIL(hLog, "Passed in object was set to NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get an item back)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing duplicate");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedDP8Address->Release();
		pNonwrappedDP8Address = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/
	SAFE_RELEASE(pNonwrappedDP8Address);

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BIsEqual
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BClear()"
//==================================================================================
// ParmVExec_BClear
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.8 - Base Clear parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BClear(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Clearing without object being set");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Clear();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Clearing without object being set failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Clearing with object being set");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Clear();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Clearing with object being set failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing)




/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Clearing without object being set while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Clear();
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Clearing without object being set while locked didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)

	return (sr);
} // ParmVExec_BClear
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BGetURLW()"
//==================================================================================
// ParmVExec_BGetURLW
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.9 - Base GetURLW parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BGetURLW(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/
	WCHAR*							pwszURL = NULL;
	DWORD							dwURLLen;
	DWORD							dwExpectedURLLen;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)





/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting Unicode URL with NULL pointers");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_GetURLW(NULL, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting Unicode URL with NULL pointers didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = wcslen(DPNA_HEADER) + 1;

		tr = pDP8Address->DPA_GetURLW(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting non-set object Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		
		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting non-set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, DPNA_HEADER) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, DPNA_HEADER);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID) + 1;

		tr = pDP8Address->DPA_GetURLW(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;

		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, to verify size of buffer is reduced.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = wcslen(DPNA_HEADER) + 1;

		tr = pDP8Address->DPA_GetURLW(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting non-set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, DPNA_HEADER) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, DPNA_HEADER);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Clearing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Clear();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Clearing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W misc key with string type");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_AddComponent(MISCELLANEOUS_KEY1,
											MISCELLANEOUS_VALUE1,
											(wcslen(MISCELLANEOUS_VALUE1) + 1) * sizeof (WCHAR),
											DPNA_DATATYPE_STRING);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Adding component W with string type failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed adding component W)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Retrieving Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = wcslen(DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1) + 1;

		tr = pDP8Address->DPA_GetURLW(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Retrieving Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL,
				DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object Unicode URL - size while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID) + 1;

		tr = pDP8Address->DPA_GetURLW(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL - size while locked didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object Unicode URL while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL while locked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/
	SAFE_LOCALFREE(pwszURL);

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BGetURLW
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BGetURLA()"
//==================================================================================
// ParmVExec_BGetURLA
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.10 - Base GetURLA parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BGetURLA(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/
	CHAR*							pszURL = NULL;
	DWORD							dwURLLen;
	DWORD							dwExpectedURLLen;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)





/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting Ansi URL with NULL pointers");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_GetURLA(NULL, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting Ansi URL with NULL pointers didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = strlen(DPNA_HEADER_A) + 1;

		tr = pDP8Address->DPA_GetURLA(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting non-set object Ansi URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		
		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object Ansi URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(CHAR*, pszURL, ((dwURLLen * sizeof (CHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLA(pszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting non-set object Ansi URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (strcmp(pszURL, DPNA_HEADER_A) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pszURL, DPNA_HEADER_A);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pszURL);
		pszURL = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object Ansi URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = strlen(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A) + 1;

		tr = pDP8Address->DPA_GetURLA(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Ansi URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object Ansi URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(CHAR*, pszURL, ((dwURLLen * sizeof (CHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLA(pszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Ansi URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (strcmp(pszURL, DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pszURL,
				DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pszURL);
		pszURL = NULL;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Clearing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Clear();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Clearing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W misc key with string type");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_AddComponent(MISCELLANEOUS_KEY1,
											MISCELLANEOUS_VALUE1,
											(wcslen(MISCELLANEOUS_VALUE1) + 1) * sizeof (WCHAR),
											DPNA_DATATYPE_STRING);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Adding component W with string type failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed adding component W)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Retrieving Ansi URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = strlen(DPNA_HEADER_A MISCELLANEOUS_KEY1_A "=" MISCELLANEOUS_VALUE1_A) + 1;

		tr = pDP8Address->DPA_GetURLA(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Retrieving Ansi URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting Ansi URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(CHAR*, pszURL, ((dwURLLen * sizeof (CHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLA(pszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Ansi URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Ansi URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (strcmp(pszURL, DPNA_HEADER_A MISCELLANEOUS_KEY1_A "=" MISCELLANEOUS_VALUE1_A) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pszURL,
				DPNA_HEADER_A MISCELLANEOUS_KEY1_A "=" MISCELLANEOUS_VALUE1_A);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pszURL);
		pszURL = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object Unicode URL - size while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = strlen(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A) + 1;

		tr = pDP8Address->DPA_GetURLA(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Ansi URL - size while locked didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object Ansi URL while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(CHAR*, pszURL, ((dwURLLen * sizeof (CHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8Address->DPA_GetURLA(pszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Ansi URL while locked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the URL retrieved is expected.
		if (strcmp(pszURL, DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pszURL,
				DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pszURL);
		pszURL = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/
	SAFE_LOCALFREE(pszURL);

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BGetURLA
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BGetSP()"
//==================================================================================
// ParmVExec_BGetSP
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.11 - Base GetSP parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BGetSP(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/
	GUID							guidSP;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object SP with NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_GetSP(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			if(tr == DPNERR_UNSUPPORTED)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Getting non-set object SP with NULL pointer didn't return expected error INVALIDPOINTER!", 0);
				THROW_TESTRESULT;
			}
		} // end if (failed getting SP)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object SP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		guidSP = GUID_UNKNOWN;

		tr = pDP8Address->DPA_GetSP(&guidSP);
		if (tr != DPNERR_DOESNOTEXIST)
		{
			if(tr == DPNERR_UNSUPPORTED)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Getting non-set object SP didn't return expected error DOESNOTEXIST!", 0);
				THROW_TESTRESULT;
			}
		} // end if (failed getting SP)

		if (memcmp(&guidSP, &GUID_UNKNOWN, sizeof (GUID)) != 0)
		{
			DPTEST_FAIL(hLog, "SP GUID memory was stomped (%g != %g)!",
				2, &guidSP, &GUID_UNKNOWN);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (didn't get expected GUID)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object SP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&guidSP, sizeof (GUID));

		tr = pDP8Address->DPA_GetSP(&guidSP);
		if (tr != DPN_OK)
		{
			if(tr == DPNERR_UNSUPPORTED)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Getting set object SP failed!", 0);
				THROW_TESTRESULT;
			}
		} // end if (failed getting SP)

		if (memcmp(&guidSP, &GUID_UNKNOWN, sizeof (GUID)) != 0)
		{
			if(tr == DPNERR_UNSUPPORTED)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Didn't get expected SP GUID (%g != %g)!",
					2, &guidSP, &GUID_UNKNOWN);
				SETTHROW_TESTRESULT(ERROR_NO_MATCH);
			}
		} // end if (didn't get expected GUID)





/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object SP while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&guidSP, sizeof (GUID));

		tr = pDP8Address->DPA_GetSP(&guidSP);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object SP while locked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting SP)

		if (memcmp(&guidSP, &GUID_UNKNOWN, sizeof (GUID)) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected SP GUID (%g != %g)!",
				2, &guidSP, &GUID_UNKNOWN);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected GUID)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BGetSP
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BGetUserData()"
//==================================================================================
// ParmVExec_BGetUserData
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.12 - Base GetUserData parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BGetUserData(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/
	PVOID							pvUserData = NULL;
	DWORD							dwDataSize;
	DWORD							dwExpectedDataSize;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)





/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting user data with NULL pointers");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_GetUserData(NULL, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting user data with NULL pointers didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting user data)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object user data with NULL buffer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwDataSize = 0;

		tr = pDP8Address->DPA_GetUserData(NULL, &dwDataSize);
		if (tr != DPNERR_DOESNOTEXIST)
		{
			DPTEST_FAIL(hLog, "Getting non-set object user data didn't return DOESNOTEXIST!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting user data)

		// Should be empty.
		if (dwDataSize != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get 0 size for buffer!", 1, dwDataSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not 0 size)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object user data with NULL buffer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwDataSize = 0;

		tr = pDP8Address->DPA_GetUserData(NULL, &dwDataSize);
		if (tr != DPNERR_DOESNOTEXIST)
		{
			DPTEST_FAIL(hLog, "Getting set object user data with NULL buffer didn't return DOESNOTEXIST!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting user data)

		// Should be empty.
		if (dwDataSize != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get 0 size for buffer!", 1, dwDataSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not 0 size)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, no-encode user data");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";#" NO_ENCODE_DATA);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, no-encode user data failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object user data with NULL buffer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwDataSize = 0;
		dwExpectedDataSize = strlen(NO_ENCODE_DATA_A);

		tr = pDP8Address->DPA_GetUserData(NULL, &dwDataSize);

		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object user data with NULL buffer didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting user data)

		// Should be exactly enough to hold the special string.
		if (dwDataSize != dwExpectedDataSize)
		{
			DPTEST_FAIL(hLog, "Retrieved unexpected size for buffer (%u != %u)!",
				2, dwDataSize, dwExpectedDataSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected data size)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object user data");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvUserData, dwDataSize + BUFFERPADDING_SIZE);

		FillWithDWord(((LPBYTE) pvUserData) + dwDataSize, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);

		tr = pDP8Address->DPA_GetUserData(pvUserData, &dwDataSize);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object user data failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting user data)

		// Size shouldn't have changed.
		if (dwDataSize != dwExpectedDataSize)
		{
			DPTEST_FAIL(hLog, "Size for buffer was unexpectedly changed (%u != %u)!",
				2, dwDataSize, dwExpectedDataSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected data size)

		// Make sure we got the expected data back out.
		if (memcmp(pvUserData, NO_ENCODE_DATA_A, dwDataSize) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected user data (%u bytes of data at %x != \"%s\"!",
				3, dwDataSize, pvUserData, NO_ENCODE_DATA_A);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected user data)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(((LPBYTE) pvUserData) + dwDataSize, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pvUserData);
		pvUserData = NULL;




/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object user data with NULL buffer while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwDataSize = 0;
		dwExpectedDataSize = strlen(NO_ENCODE_DATA_A);

		tr = pDP8Address->DPA_GetUserData(NULL, &dwDataSize);

		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object user data with NULL buffer while locked didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting user data)

		// Should be exactly enough to hold the special string.
		if (dwDataSize != dwExpectedDataSize)
		{
			DPTEST_FAIL(hLog, "Retrieved unexpected size for buffer (%u != %u)!",
				2, dwDataSize, dwExpectedDataSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected data size)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object user data while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvUserData, dwDataSize + BUFFERPADDING_SIZE);

		FillWithDWord(((LPBYTE) pvUserData) + dwDataSize, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);

		tr = pDP8Address->DPA_GetUserData(pvUserData, &dwDataSize);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object user data while locked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting user data)

		// Size shouldn't have changed.
		if (dwDataSize != dwExpectedDataSize)
		{
			DPTEST_FAIL(hLog, "Size for buffer was unexpectedly changed (%u != %u)!",
				2, dwDataSize, dwExpectedDataSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected data size)

		// Make sure we got the expected data back out.
		if (memcmp(pvUserData, NO_ENCODE_DATA_A, dwDataSize) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected user data (%u bytes of data at %x != \"%s\"!",
				3, dwDataSize, pvUserData, NO_ENCODE_DATA_A);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected user data)

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(((LPBYTE) pvUserData) + dwDataSize, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pvUserData);
		pvUserData = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/
	SAFE_LOCALFREE(pvUserData);

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BGetUserData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BSetSP()"
//==================================================================================
// ParmVExec_BSetSP
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.13 - Base SetSP parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BSetSP(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting SP with NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetSP(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			if(tr == DPN_OK)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Setting object SP with NULL pointer didn't return expected error INVALIDPOINTER!", 0);
				THROW_TESTRESULT;
			}
		} // end if (failed setting SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting SP with GUID_UNKNOWN");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetSP(&GUID_UNKNOWN);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting SP with GUID_UNKNOWN failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting SP)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Clearing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Clear();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Clearing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing object)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting SP on built object with CLSID_DP8SP_TCPIP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetSP(&CLSID_DP8SP_TCPIP);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting SP on built object with CLSID_DP8SP_TCPIP failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting SP)





/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting SP on built object with CLSID_DP8SP_TCPIP while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetSP(&CLSID_DP8SP_TCPIP);
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Setting SP on built object with CLSID_DP8SP_TCPIP while locked didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting SP)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting SP on built object with CLSID_DP8SP_TCPIP unlocked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetSP(&CLSID_DP8SP_TCPIP);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting SP on built object with CLSID_DP8SP_TCPIP unlocked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting SP)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BSetSP
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BSetUserData()"
//==================================================================================
// ParmVExec_BSetUserData
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.14 - Base SetUserData parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BSetUserData(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data with NULL pointer and 0 size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetUserData(NULL, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting user data with NULL pointer and 0 size failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data with NULL pointer and non-zero size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetUserData(NULL, 666);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Setting user data with NULL pointer and non-zero size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)
*/



#pragma BUGBUG(vanceo, "Figure out how to work around windbg still breaking on this")
		/*
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data with invalid pointer and 0 size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetUserData((PVOID) 0x00000666, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting user data with invalid pointer and 0 size failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data with invalid pointer and non-zero size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetUserData((PVOID) 0x00000666, 666);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Setting user data with invalid pointer and non-zero size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)
		*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data with no-encode data and zero size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetUserData(NO_ENCODE_DATA_A, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting user data with no-encode data and zero size failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data with no-encode data");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetUserData(NO_ENCODE_DATA_A,
										strlen(NO_ENCODE_DATA_A));
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting user data with no-encode data failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data back to nothing");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetUserData(NULL, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting user data back to nothing failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)





/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data to nothing while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetUserData(NULL, 0);
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Setting user data back to nothing while locked didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data back to nothing unlocked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetUserData(NULL, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting user data back to nothing unlocked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BSetUserData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BGetNumComponents()"
//==================================================================================
// ParmVExec_BGetNumComponents
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.15 - Base GetNumComponents parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BGetNumComponents(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/
	DWORD							dwNumComponents;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting num components with NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_GetNumComponents(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting num components with NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting num components)
*/


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting num components for unset object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumComponents = 666;

		tr = pDP8Address->DPA_GetNumComponents(&dwNumComponents);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting num components for unset object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting num components)

		// Make sure we are told that there are no components.
		if (dwNumComponents != 0)
		{
			DPTEST_FAIL(hLog, "Num components retrieved isn't 0 (it's %u)!",
				1, dwNumComponents);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get 0 components)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting num components");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumComponents = 0;

		tr = pDP8Address->DPA_GetNumComponents(&dwNumComponents);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting num components for unset object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting num components)

		// Make sure we are told that there is 1 component.
		if (dwNumComponents != 1)
		{
			DPTEST_FAIL(hLog, "Num components retrieved isn't 1 (it's %u)!",
				1, dwNumComponents);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get 1 component)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, misc key & value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, misc key & value failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting num components");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumComponents = 0;

		tr = pDP8Address->DPA_GetNumComponents(&dwNumComponents);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting num components for unset object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting num components)

		// Make sure we are told that there are 2 components.
		if (dwNumComponents != 2)
		{
			DPTEST_FAIL(hLog, "Num components retrieved isn't 2 (it's %u)!",
				1, dwNumComponents);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get 2 components)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, misc key & value 1, misc key & value 2");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1 L";" MISCELLANEOUS_KEY2 L"=" MISCELLANEOUS_VALUE2);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, misc key & value 1, misc key & value 2 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting num components");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumComponents = 0;

		tr = pDP8Address->DPA_GetNumComponents(&dwNumComponents);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting num components for unset object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting num components)

		// Make sure we are told that there are 2 components.
		if (dwNumComponents != 2)
		{
			DPTEST_FAIL(hLog, "Num components retrieved isn't 2 (it's %u)!",
				1, dwNumComponents);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get 2 components)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, misc key & value 1, misc key & value 2");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1 L";" MISCELLANEOUS_KEY2 L"=" MISCELLANEOUS_VALUE2);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, misc key & value 1, misc key & value 2 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting num components");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumComponents = 0;

		tr = pDP8Address->DPA_GetNumComponents(&dwNumComponents);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting num components for unset object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting num components)

		// Make sure we are told that there are 3 components.
		if (dwNumComponents != 3)
		{
			DPTEST_FAIL(hLog, "Num components retrieved isn't 3 (it's %u)!",
				1, dwNumComponents);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get 3 components)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, misc key & value 1, misc key & value 1");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1 L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, misc key & value 1, misc key & value 1 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting num components");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumComponents = 0;

		tr = pDP8Address->DPA_GetNumComponents(&dwNumComponents);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting num components for unset object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting num components)

		// Make sure we are told that there are 2 components.
		if (dwNumComponents != 2)
		{
			DPTEST_FAIL(hLog, "Num components retrieved isn't 2 (it's %u)!",
				1, dwNumComponents);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get 2 components)





/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting num components while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumComponents = 0;

		tr = pDP8Address->DPA_GetNumComponents(&dwNumComponents);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting num components while locked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting num components)

		// Make sure we are told that there are 2 components.
		if (dwNumComponents != 2)
		{
			DPTEST_FAIL(hLog, "Num components retrieved isn't 2 (it's %u)!",
				1, dwNumComponents);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get 2 components)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BGetNumComponents
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BGetByName()"
//==================================================================================
// ParmVExec_BGetByName
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.16 - Base GetComponentByName parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BGetByName(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/
	DWORD							dwBufferSize;
	DWORD							dwExpectedBufferSize;
	DWORD							dwDataType;
	PVOID							pvBuffer = NULL;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component by Unicode name with NULL pointers");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_GetComponentByName(NULL, NULL, NULL, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting component by Unicode name with NULL pointers didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component by Unicode name with NULL name");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwBufferSize = 0;
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByName(NULL, NULL, &dwBufferSize, &dwDataType);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting component by Unicode name with NULL name didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if ((dwBufferSize != 0) || (dwDataType != 666))
		{
			DPTEST_FAIL(hLog, "Either buffer size (%u != 0) or buffer type (%u != 666) was set!",
				2, dwBufferSize, dwDataType);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component by Unicode name with NULL size pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByName(DPNA_KEY_PROVIDER, NULL,
												NULL, &dwDataType);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting component by Unicode name with NULL size pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwDataType != 666)
		{
			DPTEST_FAIL(hLog, "Buffer type was set (%u != 666)!", 1, dwDataType);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component by Unicode name with NULL type pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwBufferSize = 0;

		tr = pDP8Address->DPA_GetComponentByName(DPNA_KEY_PROVIDER, NULL,
												&dwBufferSize, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting component by Unicode name with NULL type pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwBufferSize != 0)
		{
			DPTEST_FAIL(hLog, "Buffer size was set (%u != 0)!", 1, dwBufferSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component by Unicode name with non-set object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwBufferSize = 0;
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByName(MISCELLANEOUS_KEY1, NULL,
												&dwBufferSize, &dwDataType);
		if (tr != DPNERR_DOESNOTEXIST)
		{
			DPTEST_FAIL(hLog, "Getting component by Unicode name with non-set object didn't return expected error DOESNOTEXIST!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if ((dwBufferSize != 0) || (dwDataType != 666))
		{
			DPTEST_FAIL(hLog, "Either buffer size (%u != 0) or buffer type (%u != 666) was set!",
				2, dwBufferSize, dwDataType);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, misc key & value 1");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, misc key & value 1 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component by non-existent Unicode name");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwBufferSize = 0;
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByName(MISCELLANEOUS_KEY2, NULL,
												&dwBufferSize, &dwDataType);
		if (tr != DPNERR_DOESNOTEXIST)
		{
			DPTEST_FAIL(hLog, "Getting component by non-existent Unicode name didn't return expected error DOESNOTEXIST!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if ((dwBufferSize != 0) || (dwDataType != 666))
		{
			DPTEST_FAIL(hLog, "Either buffer size (%u != 0) or buffer type (%u != 666) was set!",
				2, dwBufferSize, dwDataType);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component by Unicode name - provider - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwBufferSize = 0;
		dwExpectedBufferSize = sizeof (GUID);
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByName(DPNA_KEY_PROVIDER, NULL,
												&dwBufferSize, &dwDataType);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting component by Unicode name - provider - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwBufferSize != dwExpectedBufferSize)
		{
			DPTEST_FAIL(hLog, "Received unexpected buffer size (%u != %u)!",
				2, dwBufferSize, dwExpectedBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected size)

		if (dwDataType != DPNA_DATATYPE_GUID)
		{
			DPTEST_FAIL(hLog, "Recieved unexpected buffer type (%u != %u)!",
				2, dwDataType, DPNA_DATATYPE_GUID);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected data type)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component by Unicode name - provider");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvBuffer, dwBufferSize);

		tr = pDP8Address->DPA_GetComponentByName(DPNA_KEY_PROVIDER, pvBuffer,
												&dwBufferSize, &dwDataType);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting component by Unicode name - provider failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwBufferSize != dwExpectedBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size changed (%u != %u)!",
				2, dwBufferSize, dwExpectedBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected size)

		if (dwDataType != DPNA_DATATYPE_GUID)
		{
			DPTEST_FAIL(hLog, "Buffer type changed (%u != %u)!",
				2, dwDataType, DPNA_DATATYPE_GUID);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected data type)

		// Make sure the data is correct
		if (memcmp(pvBuffer, &GUID_UNKNOWN, sizeof (GUID)) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected GUID (%g != %g)!",
				2, pvBuffer, &GUID_UNKNOWN);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected GUID)

		SAFE_LOCALFREE(pvBuffer);
		pvBuffer = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component by Unicode name - misc key 1 - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwBufferSize = 0;
		dwExpectedBufferSize = (wcslen(MISCELLANEOUS_VALUE1) + 1) * sizeof (WCHAR);
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByName(MISCELLANEOUS_KEY1, NULL,
												&dwBufferSize, &dwDataType);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting component by Unicode name - misc key - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwBufferSize != dwExpectedBufferSize)
		{
			DPTEST_FAIL(hLog, "Received unexpected buffer size (%u != %u)!",
				2, dwBufferSize, dwExpectedBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected size)

		if (dwDataType != DPNA_DATATYPE_STRING)
		{
			DPTEST_FAIL(hLog, "Recieved unexpected buffer type (%u != %u)!",
				2, dwDataType, DPNA_DATATYPE_STRING);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected data type)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component by Unicode name - misc key 1");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvBuffer, dwBufferSize);

		tr = pDP8Address->DPA_GetComponentByName(MISCELLANEOUS_KEY1, pvBuffer,
												&dwBufferSize, &dwDataType);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting component by Unicode name - misc key failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwBufferSize != dwExpectedBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size changed (%u != %u)!",
				2, dwBufferSize, dwExpectedBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected size)

		if (dwDataType != DPNA_DATATYPE_STRING)
		{
			DPTEST_FAIL(hLog, "Buffer type changed (%u != %u)!",
				2, dwDataType, DPNA_DATATYPE_STRING);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected data type)

		// Make sure the data is correct
		if (wcscmp((WCHAR*) pvBuffer, MISCELLANEOUS_VALUE1) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected string (\"%S\" != \"%S\")!",
				2, pvBuffer, MISCELLANEOUS_VALUE1);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected GUID)

		SAFE_LOCALFREE(pvBuffer);
		pvBuffer = NULL;





/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component by Unicode name - misc key 1 while locked - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwBufferSize = 0;
		dwExpectedBufferSize = (wcslen(MISCELLANEOUS_VALUE1) + 1) * sizeof (WCHAR);
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByName(MISCELLANEOUS_KEY1, NULL,
												&dwBufferSize, &dwDataType);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting component by Unicode name - misc key 1 while locked - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwBufferSize != dwExpectedBufferSize)
		{
			DPTEST_FAIL(hLog, "Received unexpected buffer size (%u != %u)!",
				2, dwBufferSize, dwExpectedBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected size)

		if (dwDataType != DPNA_DATATYPE_STRING)
		{
			DPTEST_FAIL(hLog, "Recieved unexpected buffer type (%u != %u)!",
				2, dwDataType, DPNA_DATATYPE_STRING);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected data type)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component by Unicode name - misc key 1 while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvBuffer, dwBufferSize);

		tr = pDP8Address->DPA_GetComponentByName(MISCELLANEOUS_KEY1, pvBuffer,
												&dwBufferSize, &dwDataType);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting component by Unicode name - misc key 1 while locked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwBufferSize != dwExpectedBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size changed (%u != %u)!",
				2, dwBufferSize, dwExpectedBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected size)

		if (dwDataType != DPNA_DATATYPE_STRING)
		{
			DPTEST_FAIL(hLog, "Buffer type changed (%u != %u)!",
				2, dwDataType, DPNA_DATATYPE_STRING);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected data type)

		// Make sure the data is correct
		if (wcscmp((WCHAR*) pvBuffer, MISCELLANEOUS_VALUE1) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected string (\"%S\" != \"%S\")!",
				2, pvBuffer, MISCELLANEOUS_VALUE1);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected GUID)

		SAFE_LOCALFREE(pvBuffer);
		pvBuffer = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/
	SAFE_LOCALFREE(pvBuffer);

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BGetByName
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BGetByIndex()"
//==================================================================================
// ParmVExec_BGetByIndex
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.17 - Base GetComponentByIndex parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BGetByIndex(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/
	DWORD							dwNameLen;
	DWORD							dwExpectedNameLen;
	DWORD							dwBufferSize;
	DWORD							dwExpectedBufferSize;
	DWORD							dwDataType;
	WCHAR*							pwszName = NULL;
	PVOID							pvBuffer = NULL;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 0 with NULL pointers");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_GetComponentByIndex(0, NULL, NULL, NULL, NULL, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting component W 0 with NULL pointers didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 0 with NULL name len");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwBufferSize = 0;
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByIndex(0, NULL, NULL,
												NULL, &dwBufferSize,
												&dwDataType);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting component W 0 with NULL name len didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if ((dwBufferSize != 0) || (dwDataType != 666))
		{
			DPTEST_FAIL(hLog, "Either buffer size (%u != 0) or buffer type (%u != 666) was set!",
				2, dwBufferSize, dwDataType);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 0 with NULL buffer size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNameLen = 0;
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByIndex(0, NULL, &dwNameLen,
												NULL, NULL,
												&dwDataType);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting component W 0 with NULL buffer size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if ((dwNameLen != 0) || (dwDataType != 666))
		{
			DPTEST_FAIL(hLog, "Either name len (%u != 0) or buffer type (%u != 666) was set!",
				2, dwNameLen, dwDataType);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 0 with NULL data type");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNameLen = 0;
		dwBufferSize = 0;

		tr = pDP8Address->DPA_GetComponentByIndex(0, NULL, &dwNameLen,
												NULL, &dwBufferSize,
												NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting component W 0 with NULL buffer size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if ((dwNameLen != 0) || (dwBufferSize != 0))
		{
			DPTEST_FAIL(hLog, "Either name (%u != 0) or buffer size (%u != 0) was set!",
				2, dwNameLen, dwBufferSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 0 on non-set object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNameLen = 0;
		dwBufferSize = 0;
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByIndex(0, NULL, &dwNameLen,
												NULL, &dwBufferSize,
												&dwDataType);
		if (tr != DPNERR_DOESNOTEXIST)
		{
			DPTEST_FAIL(hLog, "Getting component W 0 on non-set object didn't return expected error DOESNOTEXIST!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if ((dwNameLen != 0) || (dwBufferSize != 0) || (dwDataType != 666))
		{
			DPTEST_FAIL(hLog, "Either name len (%u != 0) buffer size (%u != 0) or buffer type (%u != 666) was set!",
				3, dwNameLen, dwBufferSize, dwDataType);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 1 on non-set object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNameLen = 0;
		dwBufferSize = 0;
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByIndex(1, NULL, &dwNameLen,
												NULL, &dwBufferSize,
												&dwDataType);
		if (tr != DPNERR_DOESNOTEXIST)
		{
			DPTEST_FAIL(hLog, "Getting component W 1 on non-set object didn't return expected error DOESNOTEXIST!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if ((dwNameLen != 0) || (dwBufferSize != 0) || (dwDataType != 666))
		{
			DPTEST_FAIL(hLog, "Either name len (%u != 0) buffer size (%u != 0) or buffer type (%u != 666) was set!",
				3, dwNameLen, dwBufferSize, dwDataType);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W DPNA_INDEX_INVALID on non-set object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNameLen = 0;
		dwBufferSize = 0;
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByIndex(DPNA_INDEX_INVALID, NULL, &dwNameLen,
												NULL, &dwBufferSize,
												&dwDataType);
		if (tr != DPNERR_DOESNOTEXIST)
		{
			DPTEST_FAIL(hLog, "Getting component W DPNA_INDEX_INVALID on non-set object didn't return expected error DOESNOTEXIST!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if ((dwNameLen != 0) || (dwBufferSize != 0) || (dwDataType != 666))
		{
			DPTEST_FAIL(hLog, "Either name len (%u != 0) buffer size (%u != 0) or buffer type (%u != 666) was set!",
				3, dwNameLen, dwBufferSize, dwDataType);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header, provider with unknown GUID, misc key & value 1");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header, provider with unknown GUID, misc key & value 1 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 2");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNameLen = 0;
		dwBufferSize = 0;
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByIndex(2, NULL, &dwNameLen,
												NULL, &dwBufferSize,
												&dwDataType);
		if (tr != DPNERR_DOESNOTEXIST)
		{
			DPTEST_FAIL(hLog, "Getting component W 2 didn't return expected error DOESNOTEXIST!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if ((dwNameLen != 0) || (dwBufferSize != 0) || (dwDataType != 666))
		{
			DPTEST_FAIL(hLog, "Either name len (%u != 0) buffer size (%u != 0) or buffer type (%u != 666) was set!",
				3, dwNameLen, dwBufferSize, dwDataType);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W DPNA_INDEX_INVALID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNameLen = 0;
		dwBufferSize = 0;
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByIndex(DPNA_INDEX_INVALID, NULL, &dwNameLen,
												NULL, &dwBufferSize,
												&dwDataType);
		if (tr != DPNERR_DOESNOTEXIST)
		{
			DPTEST_FAIL(hLog, "Getting component W DPNA_INDEX_INVALID didn't return expected error DOESNOTEXIST!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if ((dwNameLen != 0) || (dwBufferSize != 0) || (dwDataType != 666))
		{
			DPTEST_FAIL(hLog, "Either name len (%u != 0) buffer size (%u != 0) or buffer type (%u != 666) was set!",
				3, dwNameLen, dwBufferSize, dwDataType);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (parameter value was altered)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 0 - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNameLen = 0;
		dwExpectedNameLen = wcslen(DPNA_KEY_PROVIDER) + 1;
		dwBufferSize = 0;
		dwExpectedBufferSize = sizeof (GUID);
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByIndex(0, NULL, &dwNameLen,
												NULL, &dwBufferSize,
												&dwDataType);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting component W 0 - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwNameLen != dwExpectedNameLen)
		{
			DPTEST_FAIL(hLog, "Received unexpected name length (%u != %u)!",
				2, dwNameLen, dwExpectedNameLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected name len)

		if (dwBufferSize != dwExpectedBufferSize)
		{
			DPTEST_FAIL(hLog, "Received unexpected buffer size (%u != %u)!",
				2, dwBufferSize, dwExpectedBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected size)

		if (dwDataType != DPNA_DATATYPE_GUID)
		{
			DPTEST_FAIL(hLog, "Recieved unexpected buffer type (%u != %u)!",
				2, dwDataType, DPNA_DATATYPE_GUID);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected data type)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 0");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszName, dwNameLen * sizeof (WCHAR));
		LOCALALLOC_OR_THROW(PVOID, pvBuffer, dwExpectedBufferSize);

		tr = pDP8Address->DPA_GetComponentByIndex(0, pwszName, &dwNameLen,
												pvBuffer, &dwBufferSize,
												&dwDataType);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting component W 0 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwNameLen != dwExpectedNameLen)
		{
			DPTEST_FAIL(hLog, "Name length changed (%u != %u)!",
				2, dwNameLen, dwExpectedNameLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected name len)

		if (dwBufferSize != dwExpectedBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size changed (%u != %u)!",
				2, dwBufferSize, dwExpectedBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected size)

		if (dwDataType != DPNA_DATATYPE_GUID)
		{
			DPTEST_FAIL(hLog, "Buffer type changed (%u != %u)!",
				2, dwDataType, DPNA_DATATYPE_GUID);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected data type)

		// Make sure the name is correct
		if (wcscmp(pwszName, DPNA_KEY_PROVIDER) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected name (\"%S\" != \"%S\")!",
				2, pwszName, DPNA_KEY_PROVIDER);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected name)

		// Make sure the data is correct
		if (memcmp(pvBuffer, &GUID_UNKNOWN, sizeof (GUID)) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected GUID (%g != %g)!",
				2, pvBuffer, &GUID_UNKNOWN);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected GUID)

		SAFE_LOCALFREE(pvBuffer);
		pvBuffer = NULL;
		SAFE_LOCALFREE(pwszName);
		pwszName = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 1 - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNameLen = 0;
		dwExpectedNameLen = wcslen(MISCELLANEOUS_KEY1) + 1;
		dwBufferSize = 0;
		dwExpectedBufferSize = (wcslen(MISCELLANEOUS_VALUE1) + 1) * sizeof (WCHAR);
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByIndex(1, NULL, &dwNameLen,
												NULL, &dwBufferSize,
												&dwDataType);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting component W 1 - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwNameLen != dwExpectedNameLen)
		{
			DPTEST_FAIL(hLog, "Received unexpected name length (%u != %u)!",
				2, dwNameLen, dwExpectedNameLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected name len)

		if (dwBufferSize != dwExpectedBufferSize)
		{
			DPTEST_FAIL(hLog, "Received unexpected buffer size (%u != %u)!",
				2, dwBufferSize, dwExpectedBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected size)

		if (dwDataType != DPNA_DATATYPE_STRING)
		{
			DPTEST_FAIL(hLog, "Recieved unexpected buffer type (%u != %u)!",
				2, dwDataType, DPNA_DATATYPE_STRING);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected data type)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 1");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszName, dwNameLen * sizeof (WCHAR));
		LOCALALLOC_OR_THROW(PVOID, pvBuffer, dwExpectedBufferSize);

		tr = pDP8Address->DPA_GetComponentByIndex(1, pwszName, &dwNameLen,
												pvBuffer, &dwBufferSize,
												&dwDataType);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting component W 1 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwNameLen != dwExpectedNameLen)
		{
			DPTEST_FAIL(hLog, "Name length changed (%u != %u)!",
				2, dwNameLen, dwExpectedNameLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected name len)

		if (dwBufferSize != dwExpectedBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size changed (%u != %u)!",
				2, dwBufferSize, dwExpectedBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected size)

		if (dwDataType != DPNA_DATATYPE_STRING)
		{
			DPTEST_FAIL(hLog, "Buffer type changed (%u != %u)!",
				2, dwDataType, DPNA_DATATYPE_STRING);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected data type)

		// Make sure the name is correct
		if (wcscmp(pwszName, MISCELLANEOUS_KEY1) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected name (\"%S\" != \"%S\")!",
				2, pwszName, MISCELLANEOUS_KEY1);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected name)

		// Make sure the data is correct
		if (wcscmp((WCHAR*) pvBuffer, MISCELLANEOUS_VALUE1) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected data (\"%S\" != \"%S\")!",
				2, pvBuffer, MISCELLANEOUS_VALUE1);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected GUID)

		SAFE_LOCALFREE(pvBuffer);
		pvBuffer = NULL;
		SAFE_LOCALFREE(pwszName);
		pwszName = NULL;





/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 1 while locked - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNameLen = 0;
		dwExpectedNameLen = wcslen(MISCELLANEOUS_KEY1) + 1;
		dwBufferSize = 0;
		dwExpectedBufferSize = (wcslen(MISCELLANEOUS_VALUE1) + 1) * sizeof (WCHAR);
		dwDataType = 666;

		tr = pDP8Address->DPA_GetComponentByIndex(1, NULL, &dwNameLen,
												NULL, &dwBufferSize,
												&dwDataType);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting component W 1 while locked - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwNameLen != dwExpectedNameLen)
		{
			DPTEST_FAIL(hLog, "Received unexpected name length (%u != %u)!",
				2, dwNameLen, dwExpectedNameLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected name len)

		if (dwBufferSize != dwExpectedBufferSize)
		{
			DPTEST_FAIL(hLog, "Received unexpected buffer size (%u != %u)!",
				2, dwBufferSize, dwExpectedBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected size)

		if (dwDataType != DPNA_DATATYPE_STRING)
		{
			DPTEST_FAIL(hLog, "Recieved unexpected buffer type (%u != %u)!",
				2, dwDataType, DPNA_DATATYPE_STRING);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected data type)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting component W 1 while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszName, dwNameLen * sizeof (WCHAR));
		LOCALALLOC_OR_THROW(PVOID, pvBuffer, dwExpectedBufferSize);

		tr = pDP8Address->DPA_GetComponentByIndex(1, pwszName, &dwNameLen,
												pvBuffer, &dwBufferSize,
												&dwDataType);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting component W 1 while locked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting component by name W)

		if (dwNameLen != dwExpectedNameLen)
		{
			DPTEST_FAIL(hLog, "Name length changed (%u != %u)!",
				2, dwNameLen, dwExpectedNameLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected name len)

		if (dwBufferSize != dwExpectedBufferSize)
		{
			DPTEST_FAIL(hLog, "Buffer size changed (%u != %u)!",
				2, dwBufferSize, dwExpectedBufferSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected size)

		if (dwDataType != DPNA_DATATYPE_STRING)
		{
			DPTEST_FAIL(hLog, "Buffer type changed (%u != %u)!",
				2, dwDataType, DPNA_DATATYPE_STRING);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected data type)

		// Make sure the name is correct
		if (wcscmp(pwszName, MISCELLANEOUS_KEY1) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected name (\"%S\" != \"%S\")!",
				2, pwszName, MISCELLANEOUS_KEY1);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected name)

		// Make sure the data is correct
		if (wcscmp((WCHAR*) pvBuffer, MISCELLANEOUS_VALUE1) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected data (\"%S\" != \"%S\")!",
				2, pvBuffer, MISCELLANEOUS_VALUE1);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected GUID)

		SAFE_LOCALFREE(pvBuffer);
		pvBuffer = NULL;
		SAFE_LOCALFREE(pwszName);
		pwszName = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/
	SAFE_LOCALFREE(pvBuffer);
	SAFE_LOCALFREE(pwszName);

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BGetByIndex
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BAdd()"
//==================================================================================
// ParmVExec_BAdd
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.18 - Base AddComponent parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BAdd(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/
	DWORD							dwURLLen;
	DWORD							dwExpectedURLLen;
	WCHAR*							pwszURL = NULL;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W with NULL pointers, 0 size, 0 type");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_AddComponent(NULL, NULL, 0, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Adding component W with NULL pointers, 0 size, 0 type didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed adding component W)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W with no data, 0 type");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_AddComponent(MISCELLANEOUS_KEY1, NULL, 0, 0);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Adding component W with no data, 0 type didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed adding component W)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W with 0 type");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_AddComponent(MISCELLANEOUS_KEY1,
											MISCELLANEOUS_VALUE1,
											(wcslen(MISCELLANEOUS_VALUE1) + 1) * sizeof (WCHAR),
											0);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Adding component W with 0 type didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (failed adding component W)
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W misc key with string type");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_AddComponent(MISCELLANEOUS_KEY1,
											MISCELLANEOUS_VALUE1,
											(wcslen(MISCELLANEOUS_VALUE1) + 1) * sizeof (WCHAR),
											DPNA_DATATYPE_STRING);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Adding component W with string type failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed adding component W)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W provider with GUID type");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_AddComponent(DPNA_KEY_PROVIDER,
											&GUID_UNKNOWN,
											sizeof (GUID),
											DPNA_DATATYPE_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Adding component W provider with GUID type failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed adding component W)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W provider with binary type");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_AddComponent(BINARYDATA_KEY,
											&c_abBinaryData,
											sizeof (c_abBinaryData),
											DPNA_DATATYPE_BINARY);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Adding component W provider with binary type failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed adding component W)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Retrieving Unicode URL size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1 L";" BINARYDATA_KEY L"=" ENCODED_BINARYDATA) + 1;

		tr = pDP8Address->DPA_GetURLW(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Retrieving Unicode URL size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "URL length is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected length)
		



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Retrieving Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, dwURLLen * sizeof (WCHAR));

		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Retrieving Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "URL length changed (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected length)
		

		// Make sure the URL we have is what we should have gotten.
		if (wcscmp(pwszURL, DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1 L";" BINARYDATA_KEY L"=" ENCODED_BINARYDATA) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected URL (\"%S\" != \"%S\")!",
				2, pwszURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1 L";" BINARYDATA_KEY L"=" ENCODED_BINARYDATA);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected URL)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W with string that must be encoded, same key name");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_AddComponent(MISCELLANEOUS_KEY1,
											MUST_ENCODE_DATA,
											(wcslen(MUST_ENCODE_DATA) + 1) * sizeof (WCHAR),
											DPNA_DATATYPE_STRING);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Adding component W with string that must be encoded, same key name failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed adding component W)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Retrieving Unicode URL size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" ENCODED_MUST_ENCODE_DATA L";" BINARYDATA_KEY L"=" ENCODED_BINARYDATA) + 1;

		tr = pDP8Address->DPA_GetURLW(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Retrieving Unicode URL size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "URL length is unexpected (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected length)
		



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Retrieving Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, dwURLLen * sizeof (WCHAR));

		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Retrieving Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "URL length changed (%u != %u)!",
				2, dwURLLen, dwExpectedURLLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected length)
		

		// Make sure the URL we have is what we should have gotten.
		if (wcscmp(pwszURL, DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" ENCODED_MUST_ENCODE_DATA L";" BINARYDATA_KEY L"=" ENCODED_BINARYDATA) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected URL (\"%S\" != \"%S\")!",
				2, pwszURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" MISCELLANEOUS_KEY1 L"=" ENCODED_MUST_ENCODE_DATA L";" BINARYDATA_KEY L"=" ENCODED_BINARYDATA);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected URL)

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;




/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W misc key with string type while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_AddComponent(MISCELLANEOUS_KEY1,
											MISCELLANEOUS_VALUE1,
											(wcslen(MISCELLANEOUS_VALUE1) + 1) * sizeof (WCHAR),
											DPNA_DATATYPE_STRING);
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Adding component W with string type while locked didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed adding component W)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W misc key with string type unlocked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_AddComponent(MISCELLANEOUS_KEY1,
											MISCELLANEOUS_VALUE1,
											(wcslen(MISCELLANEOUS_VALUE1) + 1) * sizeof (WCHAR),
											DPNA_DATATYPE_STRING);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Adding component W with string type unlocked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed adding component W)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/
	SAFE_LOCALFREE(pwszURL);

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BAdd
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BGetDevice()"
//==================================================================================
// ParmVExec_BGetDevice
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.19 - Base GetDevice parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BGetDevice(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/
	GUID							guidDevice;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object device with NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_GetDevice(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			if(tr == DPNERR_UNSUPPORTED)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Getting non-set object device with NULL pointer didn't return expected error INVALIDPOINTER!", 0);
				THROW_TESTRESULT;
			}
		} // end if (failed getting device)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object device");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		guidDevice = GUID_UNKNOWN;

		tr = pDP8Address->DPA_GetDevice(&guidDevice);
		if (tr != DPNERR_DOESNOTEXIST)
		{
			if(tr == DPNERR_UNSUPPORTED)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Getting non-set object device didn't return expected error DOESNOTEXIST!", 0);
				THROW_TESTRESULT;
			}
		} // end if (failed getting device)

		if (memcmp(&guidDevice, &GUID_UNKNOWN, sizeof (GUID)) != 0)
		{
			DPTEST_FAIL(hLog, "Device GUID memory was stomped (%g != %g)!",
				2, &guidDevice, &GUID_UNKNOWN);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (didn't get expected GUID)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & device with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_DEVICE L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & device with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object device");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&guidDevice, sizeof (GUID));

		tr = pDP8Address->DPA_GetDevice(&guidDevice);
		if (tr != DPN_OK)
		{
			if(tr == DPNERR_UNSUPPORTED)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Getting set object device failed!", 0);
				THROW_TESTRESULT;
			}
		} // end if (failed getting device)

		if (memcmp(&guidDevice, &GUID_UNKNOWN, sizeof (GUID)) != 0)
		{
			if(tr == DPNERR_UNSUPPORTED)
			{
				DPTEST_FAIL(hLog, "Xbox bug 1557 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Didn't get expected device GUID (%g != %g)!",
					2, &guidDevice, &GUID_UNKNOWN);
				SETTHROW_TESTRESULT(ERROR_NO_MATCH);
			}
		} // end if (didn't get expected GUID)





/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object device while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&guidDevice, sizeof (GUID));

		tr = pDP8Address->DPA_GetDevice(&guidDevice);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object device while locked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting device)

		if (memcmp(&guidDevice, &GUID_UNKNOWN, sizeof (GUID)) != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get expected device GUID (%g != %g)!",
				2, &guidDevice, &GUID_UNKNOWN);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (didn't get expected GUID)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BGetDevice
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BSetDevice()"
//==================================================================================
// ParmVExec_BSetDevice
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.20 - Base SetDevice parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BSetDevice(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESS					pDP8Address = NULL;
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address = new CWrapDP8Address(hLog);
		if (pDP8Address == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8Address->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting device with NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetDevice(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Setting object device with NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting device with GUID_UNKNOWN");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetDevice(&GUID_UNKNOWN);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting device with GUID_UNKNOWN failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Clearing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Clear();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Clearing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing object)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & device with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLW(DPNA_HEADER DPNA_KEY_DEVICE L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & device with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting device on built object with CLSID_DP8SP_TCPIP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetDevice(&CLSID_DP8SP_TCPIP);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting device on built object with CLSID_DP8SP_TCPIP failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)





/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting device on built object with CLSID_DP8SP_TCPIP while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetDevice(&CLSID_DP8SP_TCPIP);
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Setting device on built object with CLSID_DP8SP_TCPIP while locked didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting device on built object with CLSID_DP8SP_TCPIP unlocked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_SetDevice(&CLSID_DP8SP_TCPIP);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting device on built object with CLSID_DP8SP_TCPIP unlocked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)
*/



/* XBOX - IDirectPlay8AddressInternal doesn't exist
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Address);
		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal doesn't exist
	SAFE_RELEASE(pDP8AddressInternal);
*/

	if (pDP8Address != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
*/
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BSetDevice
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







/* XBOX - IDirectPlay8AddressInternal doesn't exist
#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BLockUnlock()"
//==================================================================================
// ParmVExec_BLockUnlock
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.21 - Base Lock and UnLock parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BLockUnlock(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
	PWRAPDP8ADDRESS					pDP8Address = NULL;
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->ExecSubTestCase("1.1.1.1", NULL, 0,
												1, pTNecd->iTesterNum);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:CCIWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.1.1.1",
									"Creating DirectPlay8Address object failed!");

		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pvSubOutputData,
									dwSubOutputDataSize,
									sizeof (TOD_ABASICS_CCIWRAP));
		pDP8Address = ((PTOD_ABASICS_CCIWRAP) pvSubOutputData)->pDP8Address;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking object that's not locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Unlocking address object didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Clearing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Clear();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Clearing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Clearing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Clear();
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Clearing object didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object again");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Clearing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Clear();
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Clearing object didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing object)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Clearing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Clear();
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Clearing object didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object again");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Clearing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_Clear();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Clearing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing object)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_ABASICS_RELEASEWRAP));
		((PTID_ABASICS_RELEASEWRAP) pvSubInputData)->pDP8Address = pDP8Address;

		sr = pTNecd->pExecutor->ExecSubTestCase("1.1.2.1", pvSubInputData,
												sizeof (TID_ABASICS_RELEASEWRAP),
												1, pTNecd->iTesterNum);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:ReleaseWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.1.2.1",
									"Releasing DirectPlay8Address object failed!");

		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	SAFE_LOCALFREE(pvSubInputData);
	SAFE_RELEASE(pDP8AddressInternal);

	if (pDP8Address != NULL)
	{
		// Ignore error
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BLockUnlock
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/





/* XBOX - BuildFromDPADDR not supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_BBuildFromDPADDR()"
//==================================================================================
// ParmVExec_BBuildFromDPADDR
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.22 - Base BuilfFromDPADDRESS parameter validation
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
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
HRESULT ParmVExec_BBuildDPADDR(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
	PWRAPDP8ADDRESS					pDP8Address = NULL;
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;

	LPDIRECTPLAYLOBBY2				lpDPLobby2 = NULL; // may be cast to a 2A version

	WCHAR							wszTempString[256];
	DWORD							dwSizeofTempString = NULL;
	LPVOID							lpDP4Address = NULL;
	DWORD							dwSizeofDP4Address = NULL;

	WCHAR*							pwszURL = NULL;
	DWORD							dwURLLen = NULL;

	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->ExecSubTestCase("1.1.1.1", NULL, 0,
												1, pTNecd->iTesterNum);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:CCIWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.1.1.1",
									"Creating DirectPlay8Address object failed!");

		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pvSubOutputData,
									dwSubOutputDataSize,
									sizeof (TOD_ABASICS_CCIWRAP));
		pDP8Address = ((PTOD_ABASICS_CCIWRAP) pvSubOutputData)->pDP8Address;


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("BuildDPAddress, NULL Addr, NULL Size.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_BuildFromDPADDRESS(NULL, NULL);
		if(tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Pass in NULL Addr and Size didn't return expected error DPNERR_INVALIDPARAM!",0);
			THROW_TESTRESULT;
		}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("BuildDPAddress, NULL Addr, value in size.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSizeofTempString = 16;

		tr = pDP8Address->DPA_BuildFromDPADDRESS(NULL, dwSizeofTempString);
		if(tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Pass in NULL Addr and valid Size didn't return expected error DPNERR_INVALIDPOINTER!",0);
			THROW_TESTRESULT;
		}

		
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("BuildDPAddress, Valid Address, NULL size.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		wcscpy(wszTempString,L"");
		dwSizeofTempString = wcslen(wszTempString) * sizeof(WCHAR);

		tr = pDP8Address->DPA_BuildFromDPADDRESS(wszTempString, NULL);
		if(tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Pass in Valid Addr and NULL(0) size didn't return expected INVALIDPARAM!",0);
			THROW_TESTRESULT;
		}


#pragma BUGBUG(vanceo, "Figure out how to work around windbg still breaking on this")
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from URL using invalid pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromDPADDRESS((void*) 0x00000666, NULL);
		if (tr != DPNERR_INVALIDSTRING)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using invalid pointer didn't return expected error INVALIDSTRING!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building using empty string");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		wcscpy(wszTempString,L"");
		dwSizeofTempString = (wcslen(wszTempString)+1) * sizeof(WCHAR);

		tr = pDP8Address->DPA_BuildFromDPADDRESS(wszTempString, dwSizeofTempString);
		if (tr != DPNERR_INVALIDADDRESSFORMAT)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using invalid pointer didn't return expected error INVALIDURL!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)

		

		
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Create a DirectPlay4Lobby object to get address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// We need a temporary lobby object, so create one.
		sr = CoCreateInstance(CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER,
								IID_IDirectPlayLobby2, (LPVOID*) (&lpDPLobby2));
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't create instance of IDirectPlayLobby2!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get size for a DirectPlay4Lobby Address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSizeofDP4Address = 0;    //Start with a zero size buffer.
		tr = lpDPLobby2->CreateAddress(CLSID_DP8SP_TCPIP,
									   DPAID_INet,
									   FAKE_TCPIPADDRESS_A,
									   strlen(FAKE_TCPIPADDRESS_A) + 1,
									   lpDP4Address,
									   &dwSizeofDP4Address);
		if (tr != DPERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Create DirectPlay4 Address didn't return DPERR_BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed executing sub test case)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Create a DirectPlay4Lobby Address with valid buffer.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(PVOID, lpDP4Address, dwSizeofDP4Address);
		
		tr = lpDPLobby2->CreateAddress(CLSID_DP8SP_TCPIP,
									   DPAID_INet,
									   FAKE_TCPIPADDRESS_A,
									   strlen(FAKE_TCPIPADDRESS_A) + 1,
									   lpDP4Address,
									   &dwSizeofDP4Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Create DirectPlay4 Address with valid buffer failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed executing sub test case)

		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building using DP4 Created Address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_BuildFromDPADDRESS(lpDP4Address, dwSizeofDP4Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building Address from valid DPlay4 Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)

		
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting DPlay8 Address  object Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		wcscpy(wszTempString, 
			   DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";" DPNA_KEY_HOSTNAME L"=" FAKE_TCPIPADDRESS L"\0");
		
		dwSizeofTempString = (wcslen(wszTempString)+1);

		dwURLLen = 0;

		tr = pDP8Address->DPA_GetURLW(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)
		
		if (dwURLLen != dwSizeofTempString)
		{
			DPTEST_FAIL(hLog, "URL Return size does not equal the expected size.", 0);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (failed getting Unicode URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting DPlay8 Address object Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, (dwURLLen * sizeof (WCHAR)));

		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, wszTempString) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, wszTempString);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		
		
		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;
		
		SAFE_LOCALFREE(lpDP4Address);
		lpDP4Address = NULL;
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get size for a DirectPlay4Lobby Address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSizeofDP4Address = 0;    //Start with a zero size buffer.
		tr = lpDPLobby2->CreateAddress(GUID_UNKNOWN,
									   DPAID_INet,
									   MISCELLANEOUS_VALUE1_A,
									   strlen(MISCELLANEOUS_VALUE1_A) + 1,
									   lpDP4Address,
									   &dwSizeofDP4Address);
		if (tr != DPERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Create DirectPlay4 Address didn't return DPERR_BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed executing sub test case)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Create a DirectPlay4Lobby Address with valid buffer.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(PVOID, lpDP4Address, dwSizeofDP4Address);
		
		tr = lpDPLobby2->CreateAddress(GUID_UNKNOWN,
									   DPAID_INet,
									   MISCELLANEOUS_VALUE1_A,
									   strlen(MISCELLANEOUS_VALUE1_A) + 1,
									   lpDP4Address,
									   &dwSizeofDP4Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Create DirectPlay4 Address with valid buffer failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed executing sub test case)


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building using DP4 Created Address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_BuildFromDPADDRESS(lpDP4Address, dwSizeofDP4Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building Address from valid DPlay4 Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)

		
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting DPlay8 Address  object Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		wcscpy(wszTempString, 
			   DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";" DPNA_KEY_HOSTNAME L"=" MISCELLANEOUS_VALUE1 L"\0");
		
		dwSizeofTempString = (wcslen(wszTempString)+1);

		dwURLLen = 0;

		tr = pDP8Address->DPA_GetURLW(NULL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)
		
		if (dwURLLen != dwSizeofTempString)
		{
			DPTEST_FAIL(hLog, "URL Return size does not equal the expected size.", 0);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (failed getting Unicode URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting DPlay8 Address object Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, (dwURLLen * sizeof (WCHAR)));

		tr = pDP8Address->DPA_GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, wszTempString) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, wszTempString);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		//Free the URL String
		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;

		//Free the DPlay4 Object
		SAFE_LOCALFREE(lpDP4Address);
		lpDP4Address = NULL;

	
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
											(PVOID*) &pDP8AddressInternal);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Locking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->Lock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Locking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed locking address object)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get size for a DirectPlay4Lobby Address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSizeofDP4Address = 0;    //Start with a zero size buffer.
		tr = lpDPLobby2->CreateAddress(GUID_UNKNOWN,
									   DPAID_INet,
									   MISCELLANEOUS_VALUE1_A,
									   strlen(MISCELLANEOUS_VALUE1_A) + 1,
									   lpDP4Address,
									   &dwSizeofDP4Address);
		if (tr != DPERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Create DirectPlay4 Address didn't return DPERR_BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed executing sub test case)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Create a DirectPlay4Lobby Address with valid buffer.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(PVOID, lpDP4Address, dwSizeofDP4Address);
		
		tr = lpDPLobby2->CreateAddress(GUID_UNKNOWN,
									   DPAID_INet,
									   MISCELLANEOUS_VALUE1_A,
									   strlen(MISCELLANEOUS_VALUE1_A) + 1,
									   lpDP4Address,
									   &dwSizeofDP4Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Create DirectPlay4 Address with valid buffer failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed executing sub test case)

		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building using DP4 Created Address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_BuildFromDPADDRESS(lpDP4Address, dwSizeofDP4Address);
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Building Locked Address from valid DPlay4 didn't return NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed building)

		
		SAFE_LOCALFREE(lpDP4Address);
		lpDP4Address = NULL;


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Unlocking address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressInternal->UnLock();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Unlocking address object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed unlocking address object)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get size for a DirectPlay4Lobby Address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSizeofDP4Address = 0;    //Start with a zero size buffer.
		tr = lpDPLobby2->CreateAddress(GUID_UNKNOWN,
									   DPAID_INet,
									   MISCELLANEOUS_VALUE1_A,
									   strlen(MISCELLANEOUS_VALUE1_A) + 1,
									   lpDP4Address,
									   &dwSizeofDP4Address);
		if (tr != DPERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Create DirectPlay4 Address didn't return DPERR_BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed executing sub test case)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Create a DirectPlay4Lobby Address with valid buffer.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(PVOID, lpDP4Address, dwSizeofDP4Address);
		
		tr = lpDPLobby2->CreateAddress(GUID_UNKNOWN,
									   DPAID_INet,
									   MISCELLANEOUS_VALUE1_A,
									   strlen(MISCELLANEOUS_VALUE1_A) + 1,
									   lpDP4Address,
									   &dwSizeofDP4Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Create DirectPlay4 Address with valid buffer failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed executing sub test case)

		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building using DP4 Created Address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8Address->DPA_BuildFromDPADDRESS(lpDP4Address, dwSizeofDP4Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building Locked Address from valid DPlay4 didn't return NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed building)

		
		SAFE_LOCALFREE(lpDP4Address);
		lpDP4Address = NULL;


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Release the Locked Internal address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay4 Lobby interface and release address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		lpDPLobby2->Release();
		lpDPLobby2 = NULL;
	
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_ABASICS_RELEASEWRAP));
		((PTID_ABASICS_RELEASEWRAP) pvSubInputData)->pDP8Address = pDP8Address;

		sr = pTNecd->pExecutor->ExecSubTestCase("1.1.2.1", pvSubInputData,
												sizeof (TID_ABASICS_RELEASEWRAP),
												1, pTNecd->iTesterNum);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:ReleaseWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.1.2.1",
									"Releasing DirectPlay8Address object failed!");

		pDP8Address = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	SAFE_LOCALFREE(pvSubInputData);
	SAFE_RELEASE(pDP8AddressInternal);
	SAFE_RELEASE(lpDPLobby2);


	if (pDP8Address != NULL)
	{
		// Ignore error
		g_pDP8AddressesList->RemoveFirstReference(pDP8Address);
		delete (pDP8Address);
		pDP8Address = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_BBuildFromDPADDR
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/

} // namespace DPlayCoreNamespace