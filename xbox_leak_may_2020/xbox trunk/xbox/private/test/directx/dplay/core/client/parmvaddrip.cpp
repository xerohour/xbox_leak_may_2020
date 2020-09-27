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

#define LOCALHOSTIP			L"127.0.0.1"
#define LOCALHOSTIP_A		"127.0.0.1"
#define LOCALHOSTIP_I		127.0.0.1

#define DPNSVR_PORT			L"6073"
#define DPNSVR_PORT_A		"6073"
#define DPNSVR_PORT_I		DPNA_DPNSVR_PORT   //6073

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
//HRESULT ParmVExec_IPQI(HANDLE hLog);			//2.2.1.1
//HRESULT ParmVExec_IPDuplicate(HANDLE hLog);	//2.2.1.2
//HRESULT ParmVExec_IPSetEqual(HANDLE hLog);	//2.2.1.3
//HRESULT ParmVExec_IPIsEqual(HANDLE hLog);	//2.2.1.4
//HRESULT ParmVExec_IPGetURLW(HANDLE hLog);	//2.2.1.5
//HRESULT ParmVExec_IPGetURLA(HANDLE hLog);	//2.2.1.6	
//HRESULT ParmVExec_IPGetUserData(HANDLE hLog);//2.2.1.7
//HRESULT ParmVExec_IPSetUserData(HANDLE hLog);//2.2.1.8
HRESULT ParmVExec_IPBuildSockAddr(HANDLE hLog);	//2.2.1.9
HRESULT ParmVExec_IPGetSockAddr(HANDLE hLog);//2.2.1.10
HRESULT ParmVExec_IPBuildAddress(HANDLE hLog);	//2.2.1.11
HRESULT ParmVExec_IPGetAddress(HANDLE hLog);	//2.2.1.12
HRESULT ParmVExec_IPBuildLocalAddress(HANDLE hLog);	//2.2.1.13
HRESULT ParmVExec_IPGetLocalAddress(HANDLE hLog);	//2.2.1.14




/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVIP_LoadTestTable()"
//==================================================================================
// ParmV_IPLoadTestTable
//----------------------------------------------------------------------------------
//
// Description: Loads all the possible tests into the table passed in:
//				2.2			Parameter validation tests
//				2.2.1		IP interface parameter validation tests
//				2.2.1.1		IP QueryInterface parameter validation
//				2.2.1.2		IP Duplicate parameter validation
//				2.2.1.3		IP Set Equal parameter validation
//				2.2.1.4		IP Is Equal parameter validation
//				2.2.1.5		IP GetURLW parameter validation
//				2.2.1.6		IP GetURLA parameter validation
//				2.2.1.7		IP GetUserData parameter validation
//				2.2.1.8		IP SetUserData parameter validation
//				2.2.1.9		IP Build Sock Address parameter validation
//				2.2.1.10	IP Get Sock Address parameter validation
//				2.2.1.11	IP Build Address parameter validation
//				2.2.1.12	IP Get Address parameter validation
//				2.2.1.13	IP Build Local Address parameter validation
//				2.2.1.14	IP Get Local Address parameter validation
//
// Arguments:
//	PTNLOADTESTTABLEDATA pTNlttd	Pointer to data to use when loading the tests.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ParmVIP_LoadTestTable(PTNLOADTESTTABLEDATA pTNlttd)
{
	HRESULT				hr = S_OK;
	PTNTESTTABLEGROUP	pParmVIPTests = NULL;
	PTNTESTTABLEGROUP	pSubGroup = NULL;
	TNTESTCASEPROCS		procs;



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.2		IP Parameter validation tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	pParmVIPTests = pTNlttd->pBase->NewGroup("2.2", "IP Parameter validation tests");
	if (pParmVIPTests == NULL)
	{
		DPTEST_FAIL(hLog, "Couldn't create a new grouping!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't create a new grouping)



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2.2.1	IP interface parameter validation tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	pSubGroup = pParmVIPTests->NewGroup("2.2.1", "IP interface parameter validation tests");
	if (pSubGroup == NULL)
	{
		DPTEST_FAIL(hLog, "Couldn't create a new subgrouping!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't create a new subgrouping)


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPQI;

	hr = pSubGroup->AddTest(
	"2.2.1.1",	"IP QueryInterface parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;

	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPDuplicate;

	hr = pSubGroup->AddTest(
	"2.2.1.2",	"IP Duplicate parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPSetEqual;

	hr = pSubGroup->AddTest(
	"2.2.1.3",	"IP SetEqual parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;

	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPIsEqual;

	hr = pSubGroup->AddTest(
	"2.2.1.4",	"IP IsEqual parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPGetURLW;

	hr = pSubGroup->AddTest(
	"2.2.1.5",	"IP Get URLW parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPGetURLA;

	hr = pSubGroup->AddTest(
	"2.2.1.6",	"IP GetURLA parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPGetUserData;

	hr = pSubGroup->AddTest(
	"2.2.1.7",	"IP Get User Data parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPSetUserData;

	hr = pSubGroup->AddTest(
	"2.2.1.8",	"IP Set User Data parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;

	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPBuildSockAddr;

	hr = pSubGroup->AddTest(
	"2.2.1.9",	"IP Build Sock Address parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPGetSockAddr;

	hr = pSubGroup->AddTest(
	"2.2.1.10",	"IP Get Sock Address parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;

	
	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPBuildAddress;

	hr = pSubGroup->AddTest(
	"2.2.1.11",	"IP Build Address parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPGetAddress;

	hr = pSubGroup->AddTest(
	"2.2.1.12",	"IP Get Address parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPBuildLocalAddress;

	hr = pSubGroup->AddTest(
	"2.2.1.13",	"IP Build Local Address parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


	ZeroMemory(&procs, sizeof (TNTESTCASEPROCS));
	procs.dwSize = sizeof (TNTESTCASEPROCS);
	procs.pfnExecCase = ParmVExec_IPGetLocalAddress;

	hr = pSubGroup->AddTest(
	"2.2.1.14",	"IP Get Local Address parameter validation",
			1, TNTCO_API | TNTCO_STRESS | TNTCO_SCENARIO | TNTCO_DONTSAVERESULTS,
			&procs, 0, NULL);
	if (hr != S_OK)
		goto DONE;


DONE:

	return (hr);
} // ParmV_IPLoadTestTable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/





/* XBOX - QueryInterface not available
#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPQI()"
//==================================================================================
// ParmVExec_IPQI
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.1 - IP QueryInterface parameter validation
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
HRESULT ParmVExec_IPQI(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
	IUnknown*						pUnknown = NULL;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.1.1", NULL, 0,
												1, pTNecd->iTesterNum);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:IP:CCIWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.1.1",
									"Creating DirectPlay8AddressIP object failed!");

		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pvSubOutputData,
									dwSubOutputDataSize,
									sizeof (TOD_ABASICS_IP_CCIWRAP));
		pDP8AddressIP = ((PTOD_ABASICS_IP_CCIWRAP) pvSubOutputData)->pDP8AddressIP;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for IID_IDirectPlay8Address, NULL dest pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Querying with IID_IDirectPlay8Address and NULL ptr didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for GUID_NULL, NULL dest pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(GUID_NULL, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Querying with GUID_NULL and NULL ptr didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for unknown GUID, NULL dest pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(GUID_UNKNOWN, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Querying with unknown GUID and NULL ptr didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)




		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for IID_IDirectPlay8Address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address,
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

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressIP,
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

		tr = pDP8AddressIP->DPA_QueryInterface(GUID_NULL, (PVOID*) &pUnknown);
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

		tr = pDP8AddressIP->DPA_QueryInterface(GUID_UNKNOWN, (PVOID*) &pUnknown);
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

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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
		TEST_SECTION("Querying for IID_IDirectPlay8AddressIP");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressIP,
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
		TEST_SECTION("Releasing DirectPlay8AddressIP object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_ABASICS_IP_RELEASEWRAP));
		((PTID_ABASICS_IP_RELEASEWRAP) pvSubInputData)->pDP8AddressIP = pDP8AddressIP;

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.2.1", pvSubInputData,
												sizeof (TID_ABASICS_IP_RELEASEWRAP),
												1, pTNecd->iTesterNum);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:IP:ReleaseWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.2.1",
									"Releasing DirectPlay8AddressIP object failed!");

		pDP8AddressIP = NULL;


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

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPQI
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/






/*

#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPDuplicate()"
//==================================================================================
// ParmVExec_IPDuplicate
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.2 - IP Duplicate parameter validation
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
HRESULT ParmVExec_IPDuplicate(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
	
	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
	PDIRECTPLAY8ADDRESS				pNonwrappedMainDP8Address = NULL;
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
	PDIRECTPLAY8ADDRESS				pNonwrappedDP8Address = NULL;
	WCHAR*							pwszURL = NULL;
	DWORD							dwURLLen;
	DWORD							dwExpectedURLLen;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8AddressIP object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.1.1", NULL, 0,
												1, pTNecd->iTesterNum);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:IP:CCIWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.1.1",
									"Creating DirectPlay8Address object failed!");

		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pvSubOutputData,
									dwSubOutputDataSize,
									sizeof (TOD_ABASICS_IP_CCIWRAP));
		pDP8AddressIP = ((PTOD_ABASICS_IP_CCIWRAP) pvSubOutputData)->pDP8AddressIP;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Duplicating with NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_Duplicate(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Duplicating using NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Duplicating without object being set");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_Duplicate(&pNonwrappedDP8Address);
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
		TEST_SECTION("QI for DP8Address Interface in order to build from URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address,
												(PVOID*) &pNonwrappedMainDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pNonwrappedMainDP8Address->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Duplicating with object being set");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_Duplicate(&pNonwrappedDP8Address);
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

		tr = pNonwrappedMainDP8Address->BuildFromURLW(DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
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







		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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

		tr = pDP8AddressIP->DPA_Duplicate(&pNonwrappedDP8Address);
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




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing Base Address interface. For the BuildFromURL functions");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedMainDP8Address->Release();
		pNonwrappedMainDP8Address = NULL;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8AddressIP object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_ABASICS_IP_RELEASEWRAP));
		((PTID_ABASICS_IP_RELEASEWRAP) pvSubInputData)->pDP8AddressIP = pDP8AddressIP;

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.2.1", pvSubInputData,
												sizeof (TID_ABASICS_IP_RELEASEWRAP),
												1, pTNecd->iTesterNum);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:ReleaseWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.2.1",
									"Releasing DirectPlay8Address object failed!");

		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	SAFE_LOCALFREE(pvSubInputData);
	SAFE_RELEASE(pDP8AddressInternal);
	SAFE_RELEASE(pNonwrappedDP8Address);
	SAFE_RELEASE(pNonwrappedMainDP8Address);
	SAFE_LOCALFREE(pwszURL);

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPDuplicate
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

*/



/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPSetEqual()"
//==================================================================================
// ParmVExec_BSetEqual
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.3 - IP Set Equal parameter validation
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
HRESULT ParmVExec_IPSetEqual(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;

	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
	PDIRECTPLAY8ADDRESS				pNonwrappedDP8Address = NULL;
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;

	WCHAR*							pwszURL = NULL;
	DWORD							dwURLLen;
	DWORD							dwExpectedURLLen;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8AddressIP object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.1.1", NULL, 0,
												1, pTNecd->iTesterNum);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:IP:CCIWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.1.1",
									"Creating DirectPlay8AddressIP object failed!");

		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pvSubOutputData,
									dwSubOutputDataSize,
									sizeof (TOD_ABASICS_CCIWRAP));
		pDP8AddressIP = ((PTOD_ABASICS_IP_CCIWRAP) pvSubOutputData)->pDP8AddressIP;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("SetEqual with NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_SetEqual(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "SetEqual using NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("SetEqual to object without being set");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_SetEqual(pNonwrappedDP8Address);
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


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("CoCreateInstance for another unwrapped interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address,
							(PVOID*) &pNonwrappedDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "CoCreateInstance for another unwrapped interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Equal from an Empty Object to an Empty Object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_SetEqual(pNonwrappedDP8Address);
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
		tr = pDP8AddressIP->DPA_SetEqual(pNonwrappedDP8Address);
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

		tr = pDP8AddressIP->DPA_GetURLW(NULL, &dwURLLen);
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


		tr = pDP8AddressIP->DPA_GetURLW(pwszURL, &dwURLLen);
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

		tr = pNonwrappedDP8Address->BuildFromURLW(DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building passed in object from Unicode URL using header & misc key 1 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting duplicate object Unicode URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8AddressIP->DPA_GetURLW(pwszURL, &dwURLLen);
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
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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

		tr = pDP8AddressIP->DPA_SetEqual(pNonwrappedDP8Address);
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

		tr = pDP8AddressIP->DPA_SetEqual(pNonwrappedDP8Address);
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




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;


		// We don't care about the copy, release it

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing Nonwrapped Base Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedDP8Address->Release();
		pNonwrappedDP8Address = NULL;


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_ABASICS_IP_RELEASEWRAP));
		((PTID_ABASICS_IP_RELEASEWRAP) pvSubInputData)->pDP8AddressIP = pDP8AddressIP;

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.2.1", pvSubInputData,
												sizeof (TID_ABASICS_IP_RELEASEWRAP),
												1, pTNecd->iTesterNum);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:ReleaseWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.2.1",
									"Releasing DirectPlay8Address object failed!");

		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	SAFE_LOCALFREE(pvSubInputData);
	SAFE_RELEASE(pDP8AddressInternal);
	SAFE_RELEASE(pNonwrappedDP8Address);
	SAFE_LOCALFREE(pwszURL);

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPSetEqual
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


*/
/*


#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPIsEqual()"
//==================================================================================
// ParmVExec_IPIsEqual
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.4 - IP Is Equal parameter validation
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
HRESULT ParmVExec_IPIsEqual(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;

	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
	PDIRECTPLAY8ADDRESS				pNonwrappedMainDP8Address = NULL;

	PDIRECTPLAY8ADDRESS				pNonwrappedDP8Address = NULL;
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.1.1", NULL, 0,
												1, pTNecd->iTesterNum);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:CCIWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.1.1",
									"Creating DirectPlay8Address object failed!");

		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pvSubOutputData,
									dwSubOutputDataSize,
									sizeof (TOD_ABASICS_CCIWRAP));
		pDP8AddressIP = ((PTOD_ABASICS_IP_CCIWRAP) pvSubOutputData)->pDP8AddressIP;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("IsEqual with NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_IsEqual(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "IsEqual using NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed duplicating)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("IsEqual to object without being created");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_IsEqual(pNonwrappedDP8Address);
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


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("CoCreateInstance for another unwrapped interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address,
							(PVOID*) &pNonwrappedDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "CoCreateInstance for another unwrapped interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Is Equal from an Empty Object to an Empty Object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_IsEqual(pNonwrappedDP8Address);
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
		tr = pDP8AddressIP->DPA_IsEqual(pNonwrappedDP8Address);
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
		TEST_SECTION("QI for DP8Address Interface in order to build from URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address,
												(PVOID*) &pNonwrappedMainDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Set Data in our main address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pNonwrappedMainDP8Address->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building main Address from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Is Equal from pass in Object to an equal Set Object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_IsEqual(pNonwrappedDP8Address);
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

		tr = pNonwrappedMainDP8Address->BuildFromURLW(DPNA_HEADER MISCELLANEOUS_KEY1 L"=" MISCELLANEOUS_VALUE1);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building passed in object from Unicode URL using header & misc key 1 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Is Equal from pass in Object to an different Set Object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_IsEqual(pNonwrappedDP8Address);
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





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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

		tr = pDP8AddressIP->DPA_IsEqual(pNonwrappedDP8Address);
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




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing duplicate");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedDP8Address->Release();
		pNonwrappedDP8Address = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing duplicate base address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedMainDP8Address->Release();
		pNonwrappedMainDP8Address = NULL;


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_ABASICS_IP_RELEASEWRAP));
		((PTID_ABASICS_IP_RELEASEWRAP) pvSubInputData)->pDP8AddressIP = pDP8AddressIP;

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.2.1", pvSubInputData,
												sizeof (TID_ABASICS_IP_RELEASEWRAP),
												1, pTNecd->iTesterNum);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:ReleaseWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.2.1",
									"Releasing DirectPlay8Address object failed!");

		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	SAFE_LOCALFREE(pvSubInputData);
	SAFE_RELEASE(pDP8AddressInternal);
	SAFE_RELEASE(pNonwrappedDP8Address);
	SAFE_RELEASE(pNonwrappedMainDP8Address);

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPIsEqual
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

*/
/*

#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPGetURLW()"
//==================================================================================
// ParmVExec_IPGetURLW
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.5 - IP GetURLW parameter validation
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
HRESULT ParmVExec_IPGetURLW(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;

	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
	PDIRECTPLAY8ADDRESS				pNonwrappedMainDP8Address = NULL;
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;

	WCHAR*							pwszURL = NULL;
	DWORD							dwURLLen;
	DWORD							dwExpectedURLLen;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.1.1", NULL, 0,
												1, pTNecd->iTesterNum);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:IP:CCIWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.1.1",
									"Creating DirectPlay8Address object failed!");

		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pvSubOutputData,
									dwSubOutputDataSize,
									sizeof (TOD_ABASICS_IP_CCIWRAP));
		pDP8AddressIP = ((PTOD_ABASICS_IP_CCIWRAP) pvSubOutputData)->pDP8AddressIP;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting Unicode URL with NULL pointers");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_GetURLW(NULL, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting Unicode URL with NULL pointers didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = wcslen(DPNA_HEADER) + 1;

		tr = pDP8AddressIP->DPA_GetURLW(NULL, &dwURLLen);
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


		tr = pDP8AddressIP->DPA_GetURLW(pwszURL, &dwURLLen);
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
		TEST_SECTION("Query Interface for the non-IP version of the Address to Build URL from!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address, 
												(PVOID*) &pNonwrappedMainDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pNonwrappedMainDP8Address->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
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

		tr = pDP8AddressIP->DPA_GetURLW(NULL, &dwURLLen);
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


		tr = pDP8AddressIP->DPA_GetURLW(pwszURL, &dwURLLen);
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
		TEST_SECTION("Clearing object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pNonwrappedMainDP8Address->Clear();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Clearing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W misc key with string type");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pNonwrappedMainDP8Address->AddComponent(MISCELLANEOUS_KEY1,
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

		tr = pDP8AddressIP->DPA_GetURLW(NULL, &dwURLLen);
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


		tr = pDP8AddressIP->DPA_GetURLW(pwszURL, &dwURLLen);
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

		tr = pNonwrappedMainDP8Address->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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

		tr = pDP8AddressIP->DPA_GetURLW(NULL, &dwURLLen);
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


		tr = pDP8AddressIP->DPA_GetURLW(pwszURL, &dwURLLen);
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




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing main Unwrapped Maininterface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedMainDP8Address->Release();
		pNonwrappedMainDP8Address = NULL;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_ABASICS_IP_RELEASEWRAP));
		((PTID_ABASICS_IP_RELEASEWRAP) pvSubInputData)->pDP8AddressIP = pDP8AddressIP;

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.2.1", pvSubInputData,
												sizeof (TID_ABASICS_IP_RELEASEWRAP),
												1, pTNecd->iTesterNum);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:IP:ReleaseWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.2.1",
									"Releasing DirectPlay8Address object failed!");

		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	SAFE_LOCALFREE(pvSubInputData);
	SAFE_RELEASE(pDP8AddressInternal);
	SAFE_RELEASE(pNonwrappedMainDP8Address);
	SAFE_LOCALFREE(pwszURL);

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPGetURLW  
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


*/
/*


#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPGetURLA()"
//==================================================================================
// ParmVExec_BGetURLA
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.6 - Base GetURLA parameter validation
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
HRESULT ParmVExec_IPGetURLA(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;

	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
	PDIRECTPLAY8ADDRESS				pNonwrappedMainDP8Address = NULL;
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;

	CHAR*							pszURL = NULL;
	DWORD							dwURLLen;
	DWORD							dwExpectedURLLen;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.1.1", NULL, 0,
												1, pTNecd->iTesterNum);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:IP:CCIWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.1.1",
									"Creating DirectPlay8Address object failed!");

		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pvSubOutputData,
									dwSubOutputDataSize,
									sizeof (TOD_ABASICS_IP_CCIWRAP));
		pDP8AddressIP = ((PTOD_ABASICS_IP_CCIWRAP) pvSubOutputData)->pDP8AddressIP;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting Ansi URL with NULL pointers");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_GetURLA(NULL, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting Ansi URL with NULL pointers didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object Unicode URL - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwURLLen = 0;
		dwExpectedURLLen = strlen(DPNA_HEADER_A) + 1;

		tr = pDP8AddressIP->DPA_GetURLA(NULL, &dwURLLen);
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


		tr = pDP8AddressIP->DPA_GetURLA(pszURL, &dwURLLen);
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
		TEST_SECTION("Query for non-ip non-wrapped address interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address,
											   (PVOID*)&pNonwrappedMainDP8Address);
		if(tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Not able to query for DP8Address Interface!",0);
			THROW_TESTRESULT;
		}


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Ansi URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pNonwrappedMainDP8Address->BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A);
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

		tr = pDP8AddressIP->DPA_GetURLA(NULL, &dwURLLen);
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


		tr = pDP8AddressIP->DPA_GetURLA(pszURL, &dwURLLen);
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

		tr = pNonwrappedMainDP8Address->Clear();
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Clearing object failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing)   



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Adding component W misc key with string type");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pNonwrappedMainDP8Address->AddComponent(MISCELLANEOUS_KEY1,
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

		tr = pDP8AddressIP->DPA_GetURLA(NULL, &dwURLLen);
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


		tr = pDP8AddressIP->DPA_GetURLA(pszURL, &dwURLLen);
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

		tr = pNonwrappedMainDP8Address->BuildFromURLA(DPNA_HEADER_A DPNA_KEY_PROVIDER_A "=" ENCODED_UNKNOWN_GUID_A);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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

		tr = pDP8AddressIP->DPA_GetURLA(NULL, &dwURLLen);
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


		tr = pDP8AddressIP->DPA_GetURLA(pszURL, &dwURLLen);
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




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedMainDP8Address->Release();
		pNonwrappedMainDP8Address = NULL;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_ABASICS_IP_RELEASEWRAP));
		((PTID_ABASICS_IP_RELEASEWRAP) pvSubInputData)->pDP8AddressIP = pDP8AddressIP;

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.2.1", pvSubInputData,
												sizeof (TID_ABASICS_IP_RELEASEWRAP),
												1, pTNecd->iTesterNum);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:ReleaseWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.2.1",
									"Releasing DirectPlay8Address object failed!");

		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	SAFE_LOCALFREE(pvSubInputData);
	SAFE_RELEASE(pDP8AddressInternal);
	SAFE_RELEASE(pNonwrappedMainDP8Address);
	SAFE_LOCALFREE(pszURL);

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPGetURLA
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



*/

/*



#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPGetUserData()"
//==================================================================================
// ParmVExec_IPGetUserData
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.7 - Base GetUserData parameter validation
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
HRESULT ParmVExec_IPGetUserData(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;

	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
	PDIRECTPLAY8ADDRESS				pNonwrappedMainDP8Address = NULL;
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;

	PVOID							pvUserData = NULL;
	DWORD							dwDataSize;
	DWORD							dwExpectedDataSize;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.1.1", NULL, 0,
												1, pTNecd->iTesterNum);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:IP:CCIWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.1.1",
									"Creating DirectPlay8Address object failed!");

		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pvSubOutputData,
									dwSubOutputDataSize,
									sizeof (TOD_ABASICS_IP_CCIWRAP));
		pDP8AddressIP = ((PTOD_ABASICS_IP_CCIWRAP) pvSubOutputData)->pDP8AddressIP;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting user data with NULL pointers");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_GetUserData(NULL, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting user data with NULL pointers didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting user data)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object user data with NULL buffer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwDataSize = 0;

		tr = pDP8AddressIP->DPA_GetUserData(NULL, &dwDataSize);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting non-set object user data with NULL buffer failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting user data)

		// Should be empty.
		if (dwDataSize != 0)
		{
			DPTEST_FAIL(hLog, "Didn't get 0 size for buffer!", 1, dwDataSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not 0 size)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Query for non-ip non-wrapped address interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address,
											   (PVOID*)&pNonwrappedMainDP8Address);
		if(tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Not able to query for DP8Address Interface!",0);
			THROW_TESTRESULT;
		}


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pNonwrappedMainDP8Address->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object user data with NULL buffer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwDataSize = 0;

		tr = pDP8AddressIP->DPA_GetUserData(NULL, &dwDataSize);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object user data with NULL buffer failed!", 0);
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

		tr = pNonwrappedMainDP8Address->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID L";#" NO_ENCODE_DATA);
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

		tr = pDP8AddressIP->DPA_GetUserData(NULL, &dwDataSize);

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

		tr = pDP8AddressIP->DPA_GetUserData(pvUserData, &dwDataSize);
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




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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

		tr = pDP8AddressIP->DPA_GetUserData(NULL, &dwDataSize);

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

		tr = pDP8AddressIP->DPA_GetUserData(pvUserData, &dwDataSize);
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




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing Nonwrapped Main DP8Address Pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedMainDP8Address->Release();
		pNonwrappedMainDP8Address = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_ABASICS_IP_RELEASEWRAP));
		((PTID_ABASICS_IP_RELEASEWRAP) pvSubInputData)->pDP8AddressIP = pDP8AddressIP;

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.2.1", pvSubInputData,
												sizeof (TID_ABASICS_IP_RELEASEWRAP),
												1, pTNecd->iTesterNum);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:IP:ReleaseWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.2.1",
									"Releasing DirectPlay8Address object failed!");

		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	SAFE_LOCALFREE(pvSubInputData);
	SAFE_RELEASE(pDP8AddressInternal);
	SAFE_RELEASE(pNonwrappedMainDP8Address);
	SAFE_LOCALFREE(pvUserData);

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPGetUserData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


*/

/*


#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPSetUserData()"
//==================================================================================
// ParmVExec_IPSetUserData
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.8 - IP SetUserData parameter validation
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
HRESULT ParmVExec_IPSetUserData(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.1.1", NULL, 0,
												1, pTNecd->iTesterNum);
		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:IP:CCIWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.1.1",
									"Creating DirectPlay8Address object failed!");

		// Otherwise get the object created.
		CHECKANDGET_SUBOUTPUTDATA(pSubResult,
									pvSubOutputData,
									dwSubOutputDataSize,
									sizeof (TOD_ABASICS_IP_CCIWRAP));
		pDP8AddressIP = ((PTOD_ABASICS_IP_CCIWRAP) pvSubOutputData)->pDP8AddressIP;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data with NULL pointer and 0 size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_SetUserData(NULL, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting user data with NULL pointer and 0 size failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data with NULL pointer and non-zero size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_SetUserData(NULL, 666);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Setting user data with NULL pointer and non-zero size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)




#pragma BUGBUG(vanceo, "Figure out how to work around windbg still breaking on this")
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data with invalid pointer and 0 size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_SetUserData((PVOID) 0x00000666, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting user data with invalid pointer and 0 size failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)  




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data with invalid pointer and non-zero size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_SetUserData((PVOID) 0x00000666, 666);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Setting user data with invalid pointer and non-zero size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)
		

		


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data with no-encode data and zero size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_SetUserData(NO_ENCODE_DATA_A, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting user data with no-encode data and zero size failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data with no-encode data");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_SetUserData(NO_ENCODE_DATA_A,
											strlen(NO_ENCODE_DATA_A));
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting user data with no-encode data failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting user data back to nothing");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_SetUserData(NULL, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting user data back to nothing failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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

		tr = pDP8AddressIP->DPA_SetUserData(NULL, 0);
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

		tr = pDP8AddressIP->DPA_SetUserData(NULL, 0);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting user data back to nothing unlocked failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting user data)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PVOID, pvSubInputData, sizeof (TID_ABASICS_IP_RELEASEWRAP));
		((PTID_ABASICS_IP_RELEASEWRAP) pvSubInputData)->pDP8AddressIP = pDP8AddressIP;

		sr = pTNecd->pExecutor->ExecSubTestCase("1.2.2.1", pvSubInputData,
												sizeof (TID_ABASICS_IP_RELEASEWRAP),
												1, pTNecd->iTesterNum);

		LocalFree(pvSubInputData);
		pvSubInputData = NULL;

		if (sr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't execute sub test case API:Basics:IP:ReleaseWrap!", 0);
			THROW_SYSTEMRESULT;
		} // end if (failed executing sub test case)

		GETSUBRESULT_AND_FAILIFFAILED(pSubResult, "1.2.2.1",
									"Releasing DirectPlay8Address object failed!");

		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	SAFE_LOCALFREE(pvSubInputData);
	SAFE_RELEASE(pDP8AddressInternal);

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPSetUserData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

*/

#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPBuildSockAddr()"
//==================================================================================
// ParmVExec_IPBuildSockAddr
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.9 -  IP BuildFromSockAddr parameter validation
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
HRESULT ParmVExec_IPBuildSockAddr(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
/* XBOX - Interfaces are merged on Xbox, don't need to query
	PDIRECTPLAY8ADDRESS				pNonwrappedMainDP8Address = NULL;
*/
/* XBOX - IDirectPlay8AddressInternal not supported
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/

	WCHAR*							pwszURL = NULL;
	DWORD							dwURLLen = NULL;

	SOCKADDR_IN						SockAddrIn;

	WCHAR*							pwszExpectedURL = NULL;

	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressIP = new CWrapDP8AddressIP(hLog);
		if (pDP8AddressIP == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8AddressIP->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Sock Address using NULL pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_BuildFromSockAddr(NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Building from Sock Address using NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)
*/



#pragma BUGBUG(vanceo, "Figure out how to work around windbg still breaking on this")
		/*
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Sock Address using invalid pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8Address->DPA_BuildFromURLA((char*)0x00000666);
		if (tr != DPNERR_INVALIDSTRING)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using invalid pointer didn't return expected error INVALIDSTRING!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)
		*/


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Build the sock address to test with.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//Set the port and the family type for the Sock Address
		//TCP and the Default Port for DPlay. 6073
		SockAddrIn.sin_family = AF_INET;
		SockAddrIn.sin_port = htons(DPNSVR_PORT_I);
		SockAddrIn.sin_addr.S_un.S_addr = inet_addr(LOCALHOSTIP_A);
		
		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL,
							(wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
								   DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP L";"
								   DPNA_KEY_PORT L"=" DPNSVR_PORT) + 1) * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
				DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP L";"
				DPNA_KEY_PORT L"=" DPNSVR_PORT);
		
			
			
		tr = pDP8AddressIP->DPA_BuildFromSockAddr((PSOCKADDR)&SockAddrIn);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Sock Addr failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from SockAddr)


/* XBOX - Interfaces are merged on Xbox, don't need to query
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for Regular DPlay8 interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address,
											(PVOID*) &pNonwrappedMainDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)
*/
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from SockAddr.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = NULL;

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting size of URL did not return BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if 


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from SockAddr.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, dwURLLen * sizeof(WCHAR));

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting URL with correct size failed!", 0);
			THROW_TESTRESULT;
		} // end if

		//Create the address string to compare it with.
		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, pwszExpectedURL) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, pwszExpectedURL);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		
		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;
		
		SAFE_LOCALFREE(pwszExpectedURL);
		pwszExpectedURL = NULL;

/* XBOX - AF_IPX not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Build the sock address with IPX family in address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		SockAddrIn.sin_family = AF_IPX;
		SockAddrIn.sin_port = htons(DPNSVR_PORT_I);
		SockAddrIn.sin_addr.S_un.S_addr = inet_addr(LOCALHOSTIP_A);
			
			
		tr = pDP8AddressIP->DPA_BuildFromSockAddr((PSOCKADDR)&SockAddrIn);
		if (tr != DPNERR_INVALIDPARAM)
		{
			DPTEST_FAIL(hLog, "Building from Sock Addr with IPX family did not return INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from SockAddr)
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Build the sock address with Port 0 in sock address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		SockAddrIn.sin_family = AF_INET;
		SockAddrIn.sin_port = 0;
		SockAddrIn.sin_addr.S_un.S_addr = inet_addr(LOCALHOSTIP_A);
			
		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL,
							(wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
								   DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP L";"
								   DPNA_KEY_PORT L"=" L"0") + 1) * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
				DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP L";"
				DPNA_KEY_PORT L"=" L"0");
			
		tr = pDP8AddressIP->DPA_BuildFromSockAddr((PSOCKADDR)&SockAddrIn);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Sock Addr with Port 0 failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from SockAddr)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from SockAddr.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = NULL;

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting size of URL did not return BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if 


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from SockAddr.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, dwURLLen * sizeof(WCHAR));

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting URL with correct size failed!", 0);
			THROW_TESTRESULT;
		} // end if

		//Create the address string to compare it with.
		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, pwszExpectedURL) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, pwszExpectedURL);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		
		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;
		
		SAFE_LOCALFREE(pwszExpectedURL);
		pwszExpectedURL = NULL;
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Build the sock address with NULL sock address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		SockAddrIn.sin_family = AF_INET;
		SockAddrIn.sin_port = htons(DPNSVR_PORT_I);
		SockAddrIn.sin_addr.S_un.S_addr = NULL;
			
		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL,
							(wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
								   DPNA_KEY_HOSTNAME L"=" L"0.0.0.0" L";"
								   DPNA_KEY_PORT L"=" DPNSVR_PORT) + 1) * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
				DPNA_KEY_HOSTNAME L"=" L"0.0.0.0" L";"
				DPNA_KEY_PORT L"=" DPNSVR_PORT);

		
		tr = pDP8AddressIP->DPA_BuildFromSockAddr((PSOCKADDR)&SockAddrIn);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Sock Addr with NULL Addr did not return INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from SockAddr)

		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from SockAddr.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = NULL;

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting size of URL did not return BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if 


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from SockAddr.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, dwURLLen * sizeof(WCHAR));

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting URL with correct size failed!", 0);
			THROW_TESTRESULT;
		} // end if

		//Create the address string to compare it with.
		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, pwszExpectedURL) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, pwszExpectedURL);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		
		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;
		
		SAFE_LOCALFREE(pwszExpectedURL);
		pwszExpectedURL = NULL;


		
/* XBOX - IDirectPlay8AddressInternal not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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
		TEST_SECTION("Building from Sock Addr while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		SockAddrIn.sin_family = AF_INET;
		SockAddrIn.sin_port = htons(DPNSVR_PORT_I);
		SockAddrIn.sin_addr.S_un.S_addr = inet_addr(LOCALHOSTIP_A);

		tr = pDP8AddressIP->DPA_BuildFromSockAddr((PSOCKADDR)&SockAddrIn);
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Building from Sock Addr while locked didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from SockAddr)



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
		TEST_SECTION("Building from Sock Addr using header, provider with unknown GUID, misc key & value while locked");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_BuildFromSockAddr((PSOCKADDR)&SockAddrIn);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Ansi URL using header, provider with unknown GUID, misc key & value while locked didn't return expected error NOTALLOWED!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)
*/




/* XBOX - IDirectPlay8AddressInternal not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/

/* XBOX - Interfaces are merged on Xbox, don't need to query
		pNonwrappedMainDP8Address->Release();
		pNonwrappedMainDP8Address = NULL;
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Free URL strings");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		SAFE_LOCALFREE(pwszExpectedURL);
		pwszExpectedURL = NULL;

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE

	SAFE_LOCALFREE(pwszExpectedURL);
	SAFE_LOCALFREE(pwszURL);

/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - Interfaces are merged on Xbox, don't need to query
	SAFE_RELEASE(pNonwrappedMainDP8Address);
*/
/* XBOX - IDirectPlay8AddressInternal not supported
	SAFE_RELEASE(pDP8AddressInternal);
*/

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
*/

		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPBuildSockAddr
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPGetSockAddr()"
//==================================================================================
// ParmVExec_IPGetSockAddr
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.10 -  IP GetSockAddress parameter validation
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
HRESULT ParmVExec_IPGetSockAddr(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
/* XBOX - Interfaces are merged on Xbox, don't need to query
	PDIRECTPLAY8ADDRESS				pNonwrappedMainDP8Address = NULL;
*/
/* XBOX - IDirectPlay8AddressInternal not supported
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/

	PSOCKADDR_IN					pSockAddrIn = NULL;
	DWORD							dwSockAddrLen = NULL;
	DWORD							dwExpectedSockAddrLen = NULL;

	SOCKADDR_IN						SockAddrIn;

	WCHAR*							pwszURL = NULL;
	DWORD							dwURLLen = NULL;
	DWORD							dwExpectedURLLen = NULL;

	WCHAR*							pwszExpectedURL = NULL;

	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressIP = new CWrapDP8AddressIP(hLog);
		if (pDP8AddressIP == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8AddressIP->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("NULL Sock pointer, NULL Sock Buffer Size Pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_GetSockAddress(NULL, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "NULL Pointers didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)
*/

/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Valid Sock Addr, NULL Buffer Size pointer.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSockAddrLen = NULL;
		pSockAddrIn = NULL;

		tr = pDP8AddressIP->DPA_GetSockAddress((PSOCKADDR)pSockAddrIn, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "NULL Buffer Size pointer didn't return INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if 
*/


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("NULL for SockAddr Pointer, 0 value Buffer Size, no Address Set.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSockAddrLen = NULL;
		pSockAddrIn = NULL;

		tr = pDP8AddressIP->DPA_GetSockAddress(NULL, &dwSockAddrLen);
		if (tr != DPNERR_INCOMPLETEADDRESS)
		{
			DPTEST_FAIL(hLog, "NULL Sock Pointer, 0 buffer, No Address Set didn't return INCOMPLETEADDRESS!", 0);
			THROW_TESTRESULT;
		} // end if 


/* XBOX - Interfaces are merged on Xbox, don't need to query
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for Nonwrapped DP8Address interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address,
											(PVOID*) &pNonwrappedMainDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Build the sock address with a URL.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//Set the port and the family type for the Sock Address
		//TCP and the Default Port for DPlay. 6073
		SockAddrIn.sin_family = AF_INET;
		SockAddrIn.sin_port = htons(DPNSVR_PORT_I);
		SockAddrIn.sin_addr.S_un.S_addr = inet_addr(LOCALHOSTIP_A);
		
		dwURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
								   DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP L";"
								   DPNA_KEY_PORT L"=" DPNSVR_PORT) + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
				DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP L";"
				DPNA_KEY_PORT L"=" DPNSVR_PORT);
		
			
			
		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(pwszExpectedURL);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Sock Addr from URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from SockAddr)


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("NULL for SockAddr Pointer, 100 value Buffer Size, Address Set.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSockAddrLen = 100;
		pSockAddrIn = NULL;

		tr = pDP8AddressIP->DPA_GetSockAddress(NULL, &dwSockAddrLen);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "NULL Sock Pointer, 100 buffer, Address Set didn't return INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if 
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("NULL for SockAddr Pointer, 0 value Buffer Size, Address Set.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSockAddrLen = NULL;
		pSockAddrIn = NULL;
		dwExpectedSockAddrLen = sizeof(SOCKADDR);

		tr = pDP8AddressIP->DPA_GetSockAddress(NULL, &dwSockAddrLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "NULL Sock Pointer, 0 buffer, Address Set didn't return BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if 

		if(dwSockAddrLen != dwExpectedSockAddrLen)
		{
			DPTEST_FAIL(hLog, "Did not receive expected Sock Address length of one Sock Address!",0);
			THROW_TESTRESULT;
		}


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("SockAddr Pointer, Valid Buffer Size, Address Set.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(PSOCKADDR_IN, pSockAddrIn, dwExpectedSockAddrLen + BUFFERPADDING_SIZE);

		FillWithDWord(pSockAddrIn + (dwSockAddrLen/sizeof(SOCKADDR)),
						BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN);

		tr = pDP8AddressIP->DPA_GetSockAddress((PSOCKADDR)pSockAddrIn, &dwSockAddrLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "SockAddr Pointer, Valid Buffer Size, Address Set failed!", 0);
			THROW_TESTRESULT;
		} // end if 

		//Make sure the SockAddr size is still the expected value
		if(dwSockAddrLen != dwExpectedSockAddrLen)
		{
			DPTEST_FAIL(hLog, "Did not receive expected Sock Address length of on Sock Address!",0);
			THROW_TESTRESULT;
		}

		//Make sure the SockAddr we got back is what we expected.
		if(memcmp(&SockAddrIn.sin_addr, &pSockAddrIn->sin_addr, sizeof(in_addr)))
		{
			DPTEST_FAIL(hLog, "Address in Sock Addresses Do No Match.",0);
			THROW_TESTRESULT;
		}
		
		//Make sure the SockAddr we got back is what we expected.
		if(SockAddrIn.sin_family != pSockAddrIn->sin_family)
		{
			DPTEST_FAIL(hLog, "Family in Sock Addresses Do No Match.",0);
			THROW_TESTRESULT;
		}

		//Make sure the SockAddr we got back is what we expected.
		if(SockAddrIn.sin_port != pSockAddrIn->sin_port)
		{
			DPTEST_FAIL(hLog, "Family in Sock Addresses Do No Match.",0);
			THROW_TESTRESULT;
		}

		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pSockAddrIn + (dwSockAddrLen/sizeof(SOCKADDR)), BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		
		SAFE_LOCALFREE(pSockAddrIn);
				

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Build the Invalid sock address with a URL.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//Free the URL Buffer so we can use it again.
		SAFE_LOCALFREE(pwszExpectedURL);

		dwURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID 
						  L";" DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP 
						  /*L";" DPNA_KEY_PORT L"=" DPNSVR_PORT*/) + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
				DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP
				/*L";" DPNA_KEY_PORT L"=" DPNSVR_PORT*/);
		
			
			
		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(pwszExpectedURL);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Sock Addr from URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from SockAddr)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("NULL for SockAddr Pointer, 0 value Buffer Size, Invalid Address Set.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSockAddrLen = NULL;
		pSockAddrIn = NULL;
		dwExpectedSockAddrLen = sizeof(SOCKADDR);

		tr = pDP8AddressIP->DPA_GetSockAddress(NULL, &dwSockAddrLen);
		if (tr != DPNERR_INCOMPLETEADDRESS)
		{
			DPTEST_FAIL(hLog, "Retrieving Invalid Sock Address, No Port, didn't return INCOMPLETEADDRESS!", 0);
			THROW_TESTRESULT;
		} // end if 


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Build the Invalid sock address No Host!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//Free the URL Buffer so we can use it again.
		SAFE_LOCALFREE(pwszExpectedURL);

		dwURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID 
						  /*L";" DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP*/ 
						  L";" DPNA_KEY_PORT L"=" DPNSVR_PORT) + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID
				/*L";"DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP*/
				L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);
		
			
			
		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(pwszExpectedURL);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Sock Addr from URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from SockAddr)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("NULL for SockAddr Pointer, 0 value Buffer Size, Invalid Address Set.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSockAddrLen = NULL;
		pSockAddrIn = NULL;
		dwExpectedSockAddrLen = sizeof(SOCKADDR);

		tr = pDP8AddressIP->DPA_GetSockAddress(NULL, &dwSockAddrLen);
		if (tr != DPNERR_INCOMPLETEADDRESS)
		{
			DPTEST_FAIL(hLog, "Retrieving Invalid Sock Address No Host didn't return INCOMPLETEADDRESS!", 0);
			THROW_TESTRESULT;
		} // end if 
		
		
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Build the Invalid sock address Unknown GUID  Provider!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//Free the URL Buffer so we can use it again.
		SAFE_LOCALFREE(pwszExpectedURL);

		dwURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID 
						  L";" DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP 
						  L";" DPNA_KEY_PORT L"=" DPNSVR_PORT) + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID
				L";"DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP
				L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);
		
			
			
		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(pwszExpectedURL);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Sock Addr from URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from SockAddr)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("NULL for SockAddr Pointer, 0 value Buffer Size, Unknown Guid Address Set.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSockAddrLen = NULL;
		pSockAddrIn = NULL;
		dwExpectedSockAddrLen = sizeof(SOCKADDR);


		tr = pDP8AddressIP->DPA_GetSockAddress(NULL, &dwSockAddrLen);
		if (tr != DPNERR_INVALIDADDRESSFORMAT)
		{
			if(tr == DPNERR_BUFFERTOOSMALL)
			{
				DPTEST_FAIL(hLog, "Xbox bug 2906 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Retrieving Invalid Sock Address No Host didn't return INVALIDADDRESSFORMAT!", 0);
				THROW_TESTRESULT;
			}
		} // end if 

		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Build the Invalid sock address Junk HostName!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//Free the URL Buffer so we can use it again.
		SAFE_LOCALFREE(pwszExpectedURL);

		dwURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID 
						  L";" DPNA_KEY_HOSTNAME L"=" MISCELLANEOUS_VALUE1 
						  L";" DPNA_KEY_PORT L"=" DPNSVR_PORT) + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID
				L";"DPNA_KEY_HOSTNAME L"=" MISCELLANEOUS_VALUE1
				L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);
		
			
			
		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(pwszExpectedURL);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Sock Addr from URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from SockAddr)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("NULL for SockAddr Pointer, 0 value Buffer Size, Unknown Guid Address Set.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSockAddrLen = NULL;
		pSockAddrIn = NULL;
		dwExpectedSockAddrLen = sizeof(SOCKADDR);

		tr = pDP8AddressIP->DPA_GetSockAddress(NULL, &dwSockAddrLen);
		if (tr != DPNERR_INVALIDHOSTADDRESS)
		{
			DPTEST_FAIL(hLog, "Retrieving Invalid Sock Address No Host didn't return INVALIDREMOTEADDRESS!", 0);
			THROW_TESTRESULT;
		} // end if 


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Build the Valid sock address!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//Set the port and the family type for the Sock Address
		//TCP and the Default Port for DPlay. 6073
		SockAddrIn.sin_family = AF_INET;
		SockAddrIn.sin_port = htons(DPNSVR_PORT_I);
		SockAddrIn.sin_addr.S_un.S_addr = inet_addr(LOCALHOSTIP_A);

		//Free the URL Buffer so we can use it again.
		SAFE_LOCALFREE(pwszExpectedURL);

		dwURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID 
						  L";" DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP 
						  L";" DPNA_KEY_PORT L"=" DPNSVR_PORT) + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID
				L";"DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP
				L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);
		
			
			
		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(pwszExpectedURL);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Sock Addr from URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from SockAddr)


	
/* XBOX - IDirectPlay8AddressInternal not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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
		TEST_SECTION("NULL for SockAddr Pointer, 0 value Buffer Size, Address Set.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSockAddrLen = NULL;
		pSockAddrIn = NULL;
		dwExpectedSockAddrLen = sizeof(SOCKADDR);

		tr = pDP8AddressIP->DPA_GetSockAddress(NULL, &dwSockAddrLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "NULL Sock Pointer, 0 buffer, Address Set didn't return BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if 

		if(dwSockAddrLen != dwExpectedSockAddrLen)
		{
			DPTEST_FAIL(hLog, "Did not receive expected Sock Address length of one Sock Address!",0);
			THROW_TESTRESULT;
		}


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("SockAddr Pointer, Valid Buffer Size, Address Set.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(PSOCKADDR_IN, pSockAddrIn, dwExpectedSockAddrLen + BUFFERPADDING_SIZE);

		FillWithDWord(pSockAddrIn + (dwSockAddrLen/sizeof(SOCKADDR)), BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);

		tr = pDP8AddressIP->DPA_GetSockAddress((PSOCKADDR)pSockAddrIn, &dwSockAddrLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "SockAddr Pointer, Valid Buffer Size, Address Set failed!", 0);
			THROW_TESTRESULT;
		} // end if 

		//Make sure the SockAddr size is still the expected value
		if(dwSockAddrLen != dwExpectedSockAddrLen)
		{
			DPTEST_FAIL(hLog, "Did not receive expected Sock Address length of on Sock Address!",0);
			THROW_TESTRESULT;
		}

		//Make sure the SockAddr we got back is what we expected.
		if(memcmp(&SockAddrIn.sin_addr, &pSockAddrIn->sin_addr, sizeof(in_addr)))
		{
			DPTEST_FAIL(hLog, "Address in Sock Addresses Do No Match.",0);
			THROW_TESTRESULT;
		}
		
		//Make sure the SockAddr we got back is what we expected.
		if(SockAddrIn.sin_family != pSockAddrIn->sin_family)
		{
			DPTEST_FAIL(hLog, "Family in Sock Addresses Do No Match.",0);
			THROW_TESTRESULT;
		}

		//Make sure the SockAddr we got back is what we expected.
		if(SockAddrIn.sin_port != pSockAddrIn->sin_port)
		{
			DPTEST_FAIL(hLog, "Family in Sock Addresses Do No Match.",0);
			THROW_TESTRESULT;
		}
		
		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pSockAddrIn + (dwSockAddrLen/sizeof(SOCKADDR)), BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		
		SAFE_LOCALFREE(pSockAddrIn);
				


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
		TEST_SECTION("NULL for SockAddr Pointer, 0 value Buffer Size, Address Set.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwSockAddrLen = NULL;
		pSockAddrIn = NULL;
		dwExpectedSockAddrLen = sizeof(SOCKADDR);

		tr = pDP8AddressIP->DPA_GetSockAddress(NULL, &dwSockAddrLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "NULL Sock Pointer, 0 buffer, Address Set didn't return BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if 

		if(dwSockAddrLen != dwExpectedSockAddrLen)
		{
			DPTEST_FAIL(hLog, "Did not receive expected Sock Address length of one Sock Address!",0);
			THROW_TESTRESULT;
		}


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("SockAddr Pointer, Valid Buffer Size, Address Set.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(PSOCKADDR_IN, pSockAddrIn, dwExpectedSockAddrLen + BUFFERPADDING_SIZE);

		FillWithDWord(pSockAddrIn + (dwSockAddrLen/sizeof(SOCKADDR)), BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);

		tr = pDP8AddressIP->DPA_GetSockAddress((PSOCKADDR)pSockAddrIn, &dwSockAddrLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "SockAddr Pointer, Valid Buffer Size, Address Set failed!", 0);
			THROW_TESTRESULT;
		} // end if 

		//Make sure the SockAddr size is still the expected value
		if(dwSockAddrLen != dwExpectedSockAddrLen)
		{
			DPTEST_FAIL(hLog, "Did not receive expected Sock Address length of on Sock Address!",0);
			THROW_TESTRESULT;
		}

		//Make sure the SockAddr we got back is what we expected.
		if(memcmp(&SockAddrIn.sin_addr, &pSockAddrIn->sin_addr, sizeof(in_addr)))
		{
			DPTEST_FAIL(hLog, "Address in Sock Addresses Do No Match.",0);
			THROW_TESTRESULT;
		}
		
		//Make sure the SockAddr we got back is what we expected.
		if(SockAddrIn.sin_family != pSockAddrIn->sin_family)
		{
			DPTEST_FAIL(hLog, "Family in Sock Addresses Do No Match.",0);
			THROW_TESTRESULT;
		}

		//Make sure the SockAddr we got back is what we expected.
		if(SockAddrIn.sin_port != pSockAddrIn->sin_port)
		{
			DPTEST_FAIL(hLog, "Family in Sock Addresses Do No Match.",0);
			THROW_TESTRESULT;
		}
		
		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pSockAddrIn + (dwSockAddrLen/sizeof(SOCKADDR)), BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		
/* XBOX - IDirectPlay8AddressInternal not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing helper interfaces");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		SAFE_RELEASE(pDP8AddressInternal);
*/
/* XBOX - Interfaces are merged on Xbox, don't need to query
		SAFE_RELEASE(pNonwrappedMainDP8Address);
*/


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Free our Sock Address Pointer interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		SAFE_LOCALFREE(pSockAddrIn);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Free URL strings");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		SAFE_LOCALFREE(pwszExpectedURL);
		SAFE_LOCALFREE(pwszURL);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE

	SAFE_LOCALFREE(pwszExpectedURL);
	SAFE_LOCALFREE(pwszURL);


	SAFE_LOCALFREE(pSockAddrIn);
/* XBOX - IDirectPlay8AddressInternal not supported
	SAFE_RELEASE(pDP8AddressInternal);
*/
/* XBOX - Interfaces are merged on Xbox, don't need to query
	SAFE_RELEASE(pNonwrappedMainDP8Address);
*/

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
*/
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPGetSockAddr
#undef DEBUG_SECTION
#define DEBUG_SECTION	""









#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPBuildAddress()"
//==================================================================================
// ParmVExec_IPBuildSockAddr
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.11 -  IP BuildAddress parameter validation
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
HRESULT ParmVExec_IPBuildAddress(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
/* XBOX - Interfaces are merged on Xbox, don't need to query
	PDIRECTPLAY8ADDRESS				pNonwrappedMainDP8Address = NULL;
*/
/* XBOX - IDirectPlay8AddressInternal not supported
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/

	WCHAR*							pwszURL = NULL;
	DWORD							dwURLLen = NULL;

	WCHAR*							pwszExpectedURL = NULL;

	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressIP = new CWrapDP8AddressIP(hLog);
		if (pDP8AddressIP == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8AddressIP->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Address using NULL Address Pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_BuildAddress(NULL, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Building ddress using NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Address using NULL Port");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID 
						  L";" DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP 
						  L";" DPNA_KEY_PORT L"=" L"0") + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
				DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP
				L";" DPNA_KEY_PORT L"=" L"0");
		
		tr = pDP8AddressIP->DPA_BuildAddress(LOCALHOSTIP, NULL);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building ddress using NULL Port failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)

/* XBOX - Interfaces are merged on Xbox, don't need to query
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for non-wrapped DP8 Address interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address,
											(PVOID*) &pNonwrappedMainDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from BuildAddress.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = NULL;

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting size of URL did not return BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if 


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from BuildAddress.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, dwURLLen * sizeof(WCHAR));

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting URL with correct size failed!", 0);
			THROW_TESTRESULT;
		} // end if

		//Create the address string to compare it with.
		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, pwszExpectedURL) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, pwszExpectedURL);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		
		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;
		
		SAFE_LOCALFREE(pwszExpectedURL);   //Leave for the next test case.
		pwszExpectedURL = NULL;


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Address using Miscellaneous Hostname");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID 
						  L";" DPNA_KEY_HOSTNAME L"=" MISCELLANEOUS_KEY1 
						  L";" DPNA_KEY_PORT L"=" L"0") + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
				DPNA_KEY_HOSTNAME L"=" MISCELLANEOUS_KEY1
				L";" DPNA_KEY_PORT L"=" L"0");

		
		tr = pDP8AddressIP->DPA_BuildAddress(MISCELLANEOUS_KEY1, NULL);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building address using Miscellaneous emote Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from BuildAddress.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = NULL;

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting size of URL did not return BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if 


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from Build Address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, dwURLLen * sizeof(WCHAR));

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting URL with correct size failed!", 0);
			THROW_TESTRESULT;
		} // end if

		//Create the address string to compare it with.
		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, pwszExpectedURL) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, pwszExpectedURL);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		
		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;
		
		SAFE_LOCALFREE(pwszExpectedURL);
		pwszExpectedURL = NULL;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Address using Default Port");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID 
						  L";" DPNA_KEY_HOSTNAME L"=" L"localhost" 
						  L";" DPNA_KEY_PORT L"=" DPNSVR_PORT) + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
				DPNA_KEY_HOSTNAME L"=" L"localhost"
				L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);
		
		tr = pDP8AddressIP->DPA_BuildAddress(L"localhost", DPNA_DPNSVR_PORT);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building ddress using Valid Port and Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from BuildAddress.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = NULL;

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting size of URL did not return BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if 


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from BuildAddress.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, dwURLLen * sizeof(WCHAR));

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting URL with correct size failed!", 0);
			THROW_TESTRESULT;
		} // end if

		//Create the address string to compare it with.
		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, pwszExpectedURL) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, pwszExpectedURL);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		
		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;
		
		SAFE_LOCALFREE(pwszExpectedURL);   //Leave for the next test case.
		pwszExpectedURL = NULL;



		
/* XBOX - IDirectPlay8AddressInternal not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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
		TEST_SECTION("Building from Address using Default Port");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
		tr = pDP8AddressIP->DPA_BuildAddress(L"localhost", DPNA_DPNSVR_PORT);
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Building locked address using Valid Port and Address didn't return NOTALLOWED!", 0);
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
		TEST_SECTION("Building from Address using Default Port");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID 
						  L";" DPNA_KEY_HOSTNAME L"=" L"localhost" 
						  L";" DPNA_KEY_PORT L"=" DPNSVR_PORT) + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
				DPNA_KEY_HOSTNAME L"=" L"localhost"
				L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);
		
		tr = pDP8AddressIP->DPA_BuildAddress(L"localhost", DPNA_DPNSVR_PORT);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building ddress using Valid Port and Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from A URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from BuildAddress.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = NULL;

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting size of URL did not return BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if 


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Get the URL from the Address Structure set from BuildAddress.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, dwURLLen * sizeof(WCHAR));

		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting URL with correct size failed!", 0);
			THROW_TESTRESULT;
		} // end if

		//Create the address string to compare it with.
		// Make sure the URL retrieved is expected.
		if (wcscmp(pwszURL, pwszExpectedURL) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!",
				2, pwszURL, pwszExpectedURL);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		
		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;
		
		SAFE_LOCALFREE(pwszExpectedURL);   //Leave for the next test case.
		pwszExpectedURL = NULL;





/* XBOX - IDirectPlay8AddressInternal not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/

/* XBOX - Interfaces are merged on Xbox, don't need to query
		pNonwrappedMainDP8Address->Release();
		pNonwrappedMainDP8Address = NULL;
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Free URL strings");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		SAFE_LOCALFREE(pwszExpectedURL);
		pwszExpectedURL = NULL;

		SAFE_LOCALFREE(pwszURL);
		pwszURL = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE

	SAFE_LOCALFREE(pwszExpectedURL);
	SAFE_LOCALFREE(pwszURL);

/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - Interfaces are merged on Xbox, don't need to query
	SAFE_RELEASE(pNonwrappedMainDP8Address);
*/
/* XBOX - IDirectPlay8AddressInternal not supported
	SAFE_RELEASE(pDP8AddressInternal);
*/

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
*/
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPBuildAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""








#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPGetAddress()"
//==================================================================================
// ParmVExec_IPGetAddress
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.12 - IP GetAddress parameter validation
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
HRESULT ParmVExec_IPGetAddress(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData = NULL;
	DWORD							dwSubOutputDataSize = NULL;
*/

	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
/* XBOX - Interfaces are merged on Xbox, don't need to query
	PDIRECTPLAY8ADDRESS				pNonwrappedMainDP8Address = NULL;
*/
/* XBOX - IDirectPlay8AddressInternal not supported
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/

	WCHAR*							pwszAddr = NULL;
	DWORD							dwAddrLen = NULL;
	DWORD							dwExpectedAddrLen = NULL;
	
	USHORT							usPort = NULL;

	USHORT							usExpectedPort = NULL;

	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressIP = new CWrapDP8AddressIP(hLog);
		if (pDP8AddressIP == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8AddressIP->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("GetAddress, NULL Address Pointer, Valid Length = 0, Val Port, no address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwAddrLen = 0;

		tr = pDP8AddressIP->DPA_GetAddress(NULL, &dwAddrLen, &usPort);
		if (tr != DPNERR_INCOMPLETEADDRESS)
		{
			DPTEST_FAIL(hLog, "Getting Address with NULL address pointer didn't return expected BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("GetAddress, valid Address Pointer, NULL Length pointer, Val Port, no address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwAddrLen = 0;

		tr = pDP8AddressIP->DPA_GetAddress(pwszAddr, NULL, &usPort);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting Address with NULL Length Pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)
*/


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("GetAddress, valid Address Pointer, valid Length = 0 , Port Pointer = NULL, no address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwAddrLen = 0;

		tr = pDP8AddressIP->DPA_GetAddress(pwszAddr, &dwAddrLen, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting Address with NULL Port Pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)
*/


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting non-set object Unicode Address - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwAddrLen = 0;
		dwExpectedAddrLen = 0;
		usPort = 0;
		pwszAddr = NULL;

		tr = pDP8AddressIP->DPA_GetAddress(pwszAddr, &dwAddrLen,&usPort);
		if (tr != DPNERR_INCOMPLETEADDRESS)
		{
			DPTEST_FAIL(hLog, "Getting non-set object Unicode Address - size didn't return expected error INCOMPLETEADDRESS!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		
		// Make sure the size has not changed.
		if (dwAddrLen != 0)
		{
			DPTEST_FAIL(hLog, "Addr Size changed unexpected (%u)!", 1, dwAddrLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		
		// Make sure the port has not changed.
		if (usPort != 0)
		{
			DPTEST_FAIL(hLog, "Port changed unexpected (%u)!", 1, usPort);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the size has not changed.
		if (pwszAddr != 0)
		{
			DPTEST_FAIL(hLog, "Addr Pointer changed unexpected (%x)!", 1, pwszAddr);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		

		
		
/* XBOX - Interfaces are merged on Xbox, don't need to query
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Query Interface for the non-IP version of the Address to Build URL from!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address, 
												(PVOID*) &pNonwrappedMainDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object without IP  - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwAddrLen = 0;
		dwExpectedAddrLen = NULL;
		usPort = NULL;
		pwszAddr = NULL;

		tr = pDP8AddressIP->DPA_GetAddress(NULL, &dwAddrLen, &usPort);
		if (tr != DPNERR_INVALIDADDRESSFORMAT)
		{
			if(tr == DPNERR_INCOMPLETEADDRESS)
			{
				DPTEST_FAIL(hLog, "Xbox bug 2906 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Getting set object without IP  - size didn't return expected error INVALIDREMOTEADDRESS!", 0);
				THROW_TESTRESULT;
			}
		} // end if (failed getting Unicode Address)


		// Make sure the size has not changed.
		if (dwAddrLen != 0)
		{
			DPTEST_FAIL(hLog, "Addr Size changed unexpected (%u)!", 1, dwAddrLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		
		// Make sure the port has not changed.
		if (usPort != 0)
		{
			DPTEST_FAIL(hLog, "Port changed unexpected (%u)!", 1, usPort);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the size has not changed.
		if (pwszAddr != 0)
		{
			DPTEST_FAIL(hLog, "Addr Pointer changed unexpected (%x)!", 1, pwszAddr);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with TCPIP GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID
													  L";" DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP
													  L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with TCPIP GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("GetAddress, NULL Address Pointer, valid Length = 100, valid Port valid remote address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwAddrLen = 100;
		usPort = 0;

		tr = pDP8AddressIP->DPA_GetAddress(NULL, &dwAddrLen, &usPort);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting Address with NULL Address Pointer and size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		if(usPort != 0)
		{
			DPTEST_FAIL(hLog, "Port has changed %u!", 1, usPort);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		}

		if(dwAddrLen != 100)
		{
			DPTEST_FAIL(hLog, "AddrLen has changed %u!", 1, dwAddrLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		}
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using only the header & provider with TCPIP GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID
													  /*L";" DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP*/
													  /*L";" DPNA_KEY_PORT L"=" DPNSVR_PORT*/);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with TCPIP GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("GetAddress, NULL Address Pointer, valid Length = 100, valid Port valid remote address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwAddrLen = 0;
		usPort = 0;

		tr = pDP8AddressIP->DPA_GetAddress(NULL, &dwAddrLen, &usPort);
		if (tr != DPNERR_INCOMPLETEADDRESS)
		{
			DPTEST_FAIL(hLog, "Getting Address with NULL Address Pointer and size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		if(usPort != 0)
		{
			DPTEST_FAIL(hLog, "Port has changed %u!", 1, usPort);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		}

		if(dwAddrLen != 0)
		{
			DPTEST_FAIL(hLog, "AddrLen has changed %u!", 1, dwAddrLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using only the header, TCPIP GUID, and host");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID
													  L";" DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP
													  /*L";" DPNA_KEY_PORT L"=" DPNSVR_PORT*/);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with TCPIP GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("GetAddress, NULL Address Pointer, length 0,  valid Port,  no port in remote address");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwAddrLen = 0;
		usPort = 0;

		tr = pDP8AddressIP->DPA_GetAddress(NULL, &dwAddrLen, &usPort);
		if (tr != DPNERR_INCOMPLETEADDRESS)
		{
			DPTEST_FAIL(hLog, "Getting Address with NULL Address Pointer and size with no port in address didn't return expected error INVALIDREMOTEADDRESS!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		if(usPort != 0)
		{
			DPTEST_FAIL(hLog, "Port has changed %u!", 1, usPort);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		}

		if(dwAddrLen != 0)
		{
			DPTEST_FAIL(hLog, "AddrLen has changed %u!", 1, dwAddrLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		}
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using only the header, TCPIP GUID, and host");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID
													  L";" DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP
													  L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with TCPIP GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object with IP  - size, pass in ULONG Port Param.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwAddrLen = 0;
		dwExpectedAddrLen = wcslen(LOCALHOSTIP) + 1;
		usExpectedPort = DPNSVR_PORT_I; 
		usPort = NULL;
		pwszAddr = NULL;

		tr = pDP8AddressIP->DPA_GetAddress(pwszAddr, &dwAddrLen, &usPort);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object with IP  - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode Address)


		// Make sure the size has not changed.
		if (dwAddrLen != dwExpectedAddrLen)
		{
			DPTEST_FAIL(hLog, "Addr Size changed unexpected (%u != %u)!", 2, dwAddrLen, dwExpectedAddrLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		
		// Make sure the port has not changed.
		if (usPort != 0)
		{
			DPTEST_FAIL(hLog, "Port changed unexpected (%u)!", 1, usPort);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the size has not changed.
		if (pwszAddr != 0)
		{
			DPTEST_FAIL(hLog, "Addr Pointer changed unexpected (%x)!", 1, pwszAddr);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object Address URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszAddr, ((dwAddrLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszAddr + dwAddrLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8AddressIP->DPA_GetAddress(pwszAddr, &dwAddrLen, &usPort);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwAddrLen != dwExpectedAddrLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!", 2, dwAddrLen, dwExpectedAddrLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the Address retrieved is expected.
		if (wcscmp(pwszAddr, LOCALHOSTIP) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!", 2, pwszAddr, LOCALHOSTIP);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the Ulong Port retrieved is expected.
		if (usPort != usExpectedPort)
		{
			DPTEST_FAIL(hLog, "Port retrieved is unexpected (%u != %u)!", 2, usPort, usExpectedPort);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		
		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszAddr + dwAddrLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszAddr);
		pwszAddr = NULL;


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID
													  L";" DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP
													  L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with unknown GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object with Unknown Guid Addr  - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwAddrLen = 0;
		dwExpectedAddrLen = wcslen(LOCALHOSTIP) + 1;
		usExpectedPort = DPNSVR_PORT_I; 
		usPort = NULL;
		pwszAddr = NULL;

		tr = pDP8AddressIP->DPA_GetAddress(pwszAddr, &dwAddrLen, &usPort);
		if (tr != DPNERR_INVALIDADDRESSFORMAT)
		{
			if(tr == DPNERR_BUFFERTOOSMALL)
			{
				DPTEST_FAIL(hLog, "Xbox bug 2906 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Getting set object with Unknown Guid  INVALIDREMOTEADDRESS!", 0);
				THROW_TESTRESULT;
			}
		} // end if (failed getting Unicode Address)


		// Make sure the size has not changed.
		if (dwAddrLen != 0)
		{
			if(tr == DPNERR_BUFFERTOOSMALL)
			{
				DPTEST_FAIL(hLog, "Xbox bug 2906 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Addr Size changed unexpected (%u != 0)!", 2, dwAddrLen);
				SETTHROW_TESTRESULT(ERROR_NO_MATCH);
			}
		} // end if (not expected size)
		
		// Make sure the port has not changed.
		if (usPort != 0)
		{
			DPTEST_FAIL(hLog, "Port changed unexpected (%u)!", 1, usPort);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the size has not changed.
		if (pwszAddr != 0)
		{
			DPTEST_FAIL(hLog, "Addr Pointer changed unexpected (%x)!", 1, pwszAddr);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with TCPIP GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID
													  L";" DPNA_KEY_HOSTNAME L"=" MISCELLANEOUS_VALUE1
													  L";" DPNA_KEY_PORT L"=" L"128"
													  L";#" NO_ENCODE_DATA);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider with TCPIP GUID failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)





/* XBOX - IDirectPlay8AddressInternal not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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
		TEST_SECTION("Getting set object with IP  - size");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwAddrLen = 0;
		dwExpectedAddrLen = wcslen(MISCELLANEOUS_VALUE1) + 1;
		usExpectedPort = 128; 
		usPort = NULL;
		pwszAddr = NULL;

		tr = pDP8AddressIP->DPA_GetAddress(pwszAddr, &dwAddrLen, &usPort);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "Getting set object with IP  - size didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode Address)


		// Make sure the size has not changed.
		if (dwAddrLen != dwExpectedAddrLen)
		{
			DPTEST_FAIL(hLog, "Addr Size changed unexpected (%u != %u)!", 2, dwAddrLen, dwExpectedAddrLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		
		// Make sure the port has not changed.
		if (usPort != 0)
		{
			DPTEST_FAIL(hLog, "Port changed unexpected (%u)!", 1, usPort);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the size has not changed.
		if (pwszAddr != 0)
		{
			DPTEST_FAIL(hLog, "Addr Pointer changed unexpected (%x)!", 1, pwszAddr);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting set object Address URL");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszAddr, ((dwAddrLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszAddr + dwAddrLen, BUFFERPADDING_SIZE,
					DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8AddressIP->DPA_GetAddress(pwszAddr, &dwAddrLen, &usPort);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwAddrLen != dwExpectedAddrLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!", 2, dwAddrLen, dwExpectedAddrLen);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the Address retrieved is expected.
		if (wcscmp(pwszAddr, MISCELLANEOUS_VALUE1) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!", 2, pwszAddr, MISCELLANEOUS_VALUE1);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		// Make sure the Port retrieved is expected.
		if (usPort != usExpectedPort)
		{
			DPTEST_FAIL(hLog, "Port retrieved is unexpected (%u != %u)!", 2, usPort, usExpectedPort);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)

		
		// Make sure the buffer wasn't overrun.
		if (! IsFilledWithDWord(pwszAddr + dwAddrLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN))
		{
			DPTEST_FAIL(hLog, "Data was written beyond end of buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (not still filled with pattern)

		SAFE_LOCALFREE(pwszAddr);
		pwszAddr = NULL;





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



/* XBOX - IDirectPlay8AddressInternal not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing main Unwrapped Maininterface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/* XBOX - Interfaces are merged on Xbox, don't need to query
		pNonwrappedMainDP8Address->Release();
		pNonwrappedMainDP8Address = NULL;
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal not supported
	SAFE_RELEASE(pDP8AddressInternal);
*/
/* XBOX - Interfaces are merged on Xbox, don't need to query
	SAFE_RELEASE(pNonwrappedMainDP8Address);
*/
	SAFE_LOCALFREE(pwszAddr);

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
*/
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPGetAddress  
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPBuildLocalAddress()"
//==================================================================================
// ParmVExec_IPBuildLocalAddress
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.13 - IP BuildLocalAddress parameter validation
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
HRESULT ParmVExec_IPBuildLocalAddress(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	
	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
/* XBOX - Interfaces are merged on Xbox, don't need to query
	PDIRECTPLAY8ADDRESS				pNonwrappedMainDP8Address = NULL;
*/
/* XBOX - IDirectPlay8AddressInternal not supported
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/

	WCHAR*							pwszURL = NULL;
	WCHAR*							pwszExpectedURL = NULL;
	DWORD							dwURLLen = NULL;
	DWORD							dwExpectedURLLen = NULL;
	
	USHORT							usPort = NULL;
	USHORT							usExpectedPort = NULL;
	
	GUID							guidDevice;

	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressIP = new CWrapDP8AddressIP(hLog);
		if (pDP8AddressIP == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8AddressIP->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Interfaces are merged on Xbox, don't need to query
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Query Interface for the non-IP version of the Address to Build URL from!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address, 
												(PVOID*) &pNonwrappedMainDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)
*/



/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting device with NULL GUID pointer");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_BuildLocalAddress(NULL, usPort);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Setting object device with NULL GUID pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting device with NULL Port value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		guidDevice = GUID_UNKNOWN;

		tr = pDP8AddressIP->DPA_BuildLocalAddress(&guidDevice, NULL);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting object device with NULL GUID value failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting device with NULL Port value and NULL GUID value");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		guidDevice = GUID_NULL;
		usPort = NULL;

		tr = pDP8AddressIP->DPA_BuildLocalAddress(&guidDevice, usPort);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting object device with NULL GUID value failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting device with GUID_UNKNOWN");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		guidDevice = GUID_UNKNOWN;
		usPort = DPNSVR_PORT_I;

		tr = pDP8AddressIP->DPA_BuildLocalAddress(&guidDevice, usPort);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting device with GUID_UNKNOWN failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify the GUID_UNKNOWN Device Address!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = 0;

		dwExpectedURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID 
									L";" DPNA_KEY_DEVICE L"=" ENCODED_UNKNOWN_GUID 
									L";" DPNA_KEY_PORT L"=" DPNSVR_PORT) + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwExpectedURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
				DPNA_KEY_DEVICE L"=" ENCODED_UNKNOWN_GUID
				L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);

			
		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "GetURLW with 0 Buffersize didn't return BUFFERTOOSMALL", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing object)

		
		if(dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Expected (%u) and returned (%u) lengths are not the same.", 2, dwExpectedURLLen, dwURLLen);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			THROW_TESTRESULT;
		}

		
		if(pwszURL != NULL)
		{
			DPTEST_FAIL(hLog, "pwszURL should not be set with BUFFERTOOSMALL (%x)", 1, pwszURL);
			THROW_TESTRESULT;
		}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify the GUID_UNKNOWN Device Address!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!", 2, dwURLLen, dwExpectedURLLen);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the Address retrieved is expected.
		if (wcscmp(pwszURL, pwszExpectedURL) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!", 2, pwszURL, pwszExpectedURL);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
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

		SAFE_LOCALFREE(pwszExpectedURL);
		pwszExpectedURL = NULL;


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting device with GUID_UNKNOWN");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		guidDevice = GUID_UNKNOWN;
		usPort = NULL;

		tr = pDP8AddressIP->DPA_BuildLocalAddress(&guidDevice, usPort);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting device with GUID_UNKNOWN failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify the GUID_UNKNOWN Device Address!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = 0;

		dwExpectedURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID 
									L";" DPNA_KEY_DEVICE L"=" ENCODED_UNKNOWN_GUID 
									L";" DPNA_KEY_PORT L"=" L"0") + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwExpectedURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
				DPNA_KEY_DEVICE L"=" ENCODED_UNKNOWN_GUID
				L";" DPNA_KEY_PORT L"=" L"0");

			
		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "GetURLW with 0 Buffersize didn't return BUFFERTOOSMALL", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing object)

		
		if(dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Expected (%u) and returned (%u) lengths are not the same.", 2, dwExpectedURLLen, dwURLLen);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			THROW_TESTRESULT;
		}

		
		if(pwszURL != NULL)
		{
			DPTEST_FAIL(hLog, "pwszURL should not be set with BUFFERTOOSMALL (%x)", 1, pwszURL);
			THROW_TESTRESULT;
		}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify the GUID_UNKNOWN Device Address!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!", 2, dwURLLen, dwExpectedURLLen);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the Address retrieved is expected.
		if (wcscmp(pwszURL, pwszExpectedURL) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!", 2, pwszURL, pwszExpectedURL);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
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

		SAFE_LOCALFREE(pwszExpectedURL);
		pwszExpectedURL = NULL;




/* XBOX - IDirectPlay8AddressInternal not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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
		guidDevice = GUID_UNKNOWN;
		usPort = DPNSVR_PORT_I;

		tr = pDP8AddressIP->DPA_BuildLocalAddress(&guidDevice, usPort);
		if (tr != DPNERR_NOTALLOWED)
		{
			DPTEST_FAIL(hLog, "Setting device on built object with GUID_UNKNOWN while locked didn't return expected error NOTALLOWED!", 0);
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
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Setting device with GUID_UNKNOWN");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		guidDevice = CLSID_DP8SP_TCPIP;
		usPort = DPNSVR_PORT_I;

		tr = pDP8AddressIP->DPA_BuildLocalAddress(&guidDevice, usPort);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Setting device with GUID_UNKNOWN failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify the GUID_UNKNOWN Device Address!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		dwURLLen = 0;

		dwExpectedURLLen = wcslen(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID 
									L";" DPNA_KEY_DEVICE L"=" ENCODED_TCPIP_GUID 
									L";" DPNA_KEY_PORT L"=" DPNSVR_PORT) + 1;

		LOCALALLOC_OR_THROW(WCHAR*, pwszExpectedURL, dwExpectedURLLen * sizeof(WCHAR)); 
		
		wcscpy(pwszExpectedURL,
				DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID L";"
				DPNA_KEY_DEVICE L"=" ENCODED_TCPIP_GUID
				L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);

			
		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPNERR_BUFFERTOOSMALL)
		{
			DPTEST_FAIL(hLog, "GetURLW with 0 Buffersize didn't return BUFFERTOOSMALL", 0);
			THROW_TESTRESULT;
		} // end if (failed clearing object)

		
		if(dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Expected (%u) and returned (%u) lengths are not the same.", 2, dwExpectedURLLen, dwURLLen);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			THROW_TESTRESULT;
		}

		
		if(pwszURL != NULL)
		{
			DPTEST_FAIL(hLog, "pwszURL should not be set with BUFFERTOOSMALL (%x)", 1, pwszURL);
			THROW_TESTRESULT;
		}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Verify the GUID_UNKNOWN Device Address!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(WCHAR*, pwszURL, ((dwURLLen * sizeof (WCHAR)) + BUFFERPADDING_SIZE));

		FillWithDWord(pwszURL + dwURLLen, BUFFERPADDING_SIZE, DONT_TOUCH_MEMORY_PATTERN);


		tr = pDP8AddressIP->m_pDP8AddressIP->GetURLW(pwszURL, &dwURLLen);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting set object Unicode Address failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed getting Unicode URL)

		// Make sure the size is still correct/expected.
		if (dwURLLen != dwExpectedURLLen)
		{
			DPTEST_FAIL(hLog, "Size retrieved is unexpected (%u != %u)!", 2, dwURLLen, dwExpectedURLLen);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
			fPassed = FALSE;
//			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (not expected size)
		

		// Make sure the Address retrieved is expected.
		if (wcscmp(pwszURL, pwszExpectedURL) != 0)
		{
			DPTEST_FAIL(hLog, "URL retrieved is unexpected (\"%S\" != \"%S\")!", 2, pwszURL, pwszExpectedURL);
			DPTEST_FAIL(hLog, "Xbox bug 2934 occurred!", 0);
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

		SAFE_LOCALFREE(pwszExpectedURL);
		pwszExpectedURL = NULL;






/* XBOX - IDirectPlay8AddressInternal not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/

/* XBOX - Interfaces are merged on Xbox, don't need to query
		pNonwrappedMainDP8Address->Release();
		pNonwrappedMainDP8Address = NULL;
*/


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


		
	SAFE_LOCALFREE(pwszURL);
	SAFE_LOCALFREE(pwszExpectedURL);

/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - IDirectPlay8AddressInternal not supported
	SAFE_RELEASE(pDP8AddressInternal);
*/
/* XBOX - Interfaces are merged on Xbox, don't need to query
	SAFE_RELEASE(pNonwrappedMainDP8Address);
*/

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
*/
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPBuildLocalDevice
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVExec_IPGetLocalAddress()"
//==================================================================================
// ParmVExec_IPBuildLocalAddress
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.2.1.14 - IP GetLocalAddress parameter validation
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
HRESULT ParmVExec_IPGetLocalAddress(HANDLE hLog)
{
	CTNSystemResult					sr;
	CTNTestResult					tr;
/* XBOX - No input and output data
	PTNRESULT						pSubResult;
	PVOID							pvSubInputData = NULL;
	PVOID							pvSubOutputData;
	DWORD							dwSubOutputDataSize;
*/
	
	PWRAPDP8ADDRESSIP				pDP8AddressIP = NULL;
/* XBOX - Interfaces are merged on Xbox, don't need to query
	PDIRECTPLAY8ADDRESS				pNonwrappedMainDP8Address = NULL;
*/
/* XBOX - IDirectPlay8AddressInternal not supported
	PDIRECTPLAY8ADDRESSINTERNAL		pDP8AddressInternal = NULL;
*/


	WCHAR*							pwszURL = NULL;
	DWORD							dwURLLen = NULL;
	
	USHORT							usPort = NULL;
	USHORT							usExpectedPort = NULL;
	
	GUID							guidDevice;

	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Creating DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressIP = new CWrapDP8AddressIP(hLog);
		if (pDP8AddressIP == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		tr = pDP8AddressIP->CoCreate();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting device with NULL GUID*.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		usPort = NULL;

		tr = pDP8AddressIP->DPA_GetLocalAddress(NULL, &usPort);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting NULL Address with NULL GUID* didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)
*/

/* XBOX - Now RIPs instead of returning an error
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting device with NULL Port*.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		usPort = NULL;

		tr = pDP8AddressIP->DPA_GetLocalAddress(&guidDevice, NULL);
		if (tr != DPNERR_INVALIDPOINTER)
		{
			DPTEST_FAIL(hLog, "Getting NULL Address with NULL Port* didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)
*/
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting device with NULL Address, valid params.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		usPort = NULL;

		tr = pDP8AddressIP->DPA_GetLocalAddress(&guidDevice, &usPort);
		if (tr != DPNERR_INCOMPLETEADDRESS)
		{
			DPTEST_FAIL(hLog, "Getting NULL Address with Valid Params didn't return expected error INCOMPLETEADDRESS!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)

		
/* XBOX - Interfaces are merged on Xbox, don't need to query
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Query Interface for the non-IP version of the Address to Build URL from!");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8Address, 
												(PVOID*) &pNonwrappedMainDP8Address);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Querying for internal interface failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed querying for interface)
*/

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID
													  /*L";" DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP*/
													  /*L";" DPNA_KEY_PORT L"=" DPNSVR_PORT*/);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting device with just header and provider in address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		usPort = NULL;

		tr = pDP8AddressIP->DPA_GetLocalAddress(&guidDevice, &usPort);
		if (tr != DPNERR_INCOMPLETEADDRESS)
		{
			DPTEST_FAIL(hLog, "No port address didn't return expected error INCOMPLETEADDRESS!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID
													  L";" DPNA_KEY_DEVICE L"=" ENCODED_UNKNOWN_GUID
													  /*L";" DPNA_KEY_PORT L"=" DPNSVR_PORT*/);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting device with header and provider in address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		usPort = NULL;

		tr = pDP8AddressIP->DPA_GetLocalAddress(&guidDevice, &usPort);
		if (tr != DPNERR_INCOMPLETEADDRESS)
		{
			DPTEST_FAIL(hLog, "Header and provider address didn't return expected error INCOMPLETEADDRESS!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & unknown provider");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_UNKNOWN_GUID
													  L";" DPNA_KEY_DEVICE L"=" ENCODED_UNKNOWN_GUID
													  L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting device with UNKNOWN provider GUID.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		usPort = NULL;

		tr = pDP8AddressIP->DPA_GetLocalAddress(&guidDevice, &usPort);
		if (tr != DPNERR_INVALIDADDRESSFORMAT)
		{
			if(tr == DPN_OK)
			{
				DPTEST_FAIL(hLog, "Xbox bug 2906 occurred!", 0);
				fPassed = FALSE;
			}
			else
			{
				DPTEST_FAIL(hLog, "Header and provider address didn't return expected error INVALIDADDRESSFORMAT!: 0x%08x", 1, tr);
				THROW_TESTRESULT;
			}
		} // end if (failed setting device)

		
		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID
													  L";" DPNA_KEY_DEVICE L"=" ENCODED_UNKNOWN_GUID
													  L";" DPNA_KEY_PORT L"=" DPNSVR_PORT);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting device with header, provider and port in address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		usPort = NULL;

		tr = pDP8AddressIP->DPA_GetLocalAddress(&guidDevice, &usPort);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Header, provider, and port failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)

		//Verify the Device Guid that was returned.
		if(guidDevice != GUID_UNKNOWN)
		{
			DPTEST_FAIL(hLog, "GUID retrieved is unexpected (%u != %u)!", 2,guidDevice, GUID_UNKNOWN);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (failed setting device)

		//Verify the Device Guid that was returned.
		if(usPort != DPNSVR_PORT_I)
		{
			DPTEST_FAIL(hLog, "Port retrieved is unexpected (%u != %u)!", 2,usPort, DPNSVR_PORT_I);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (failed setting device)


/* XBOX - IDirectPlay8AddressInternal not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Querying for internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->DPA_QueryInterface(IID_IDirectPlay8AddressInternal,
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
		TEST_SECTION("Getting device with header, provider and port in address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		usPort = NULL;

		tr = pDP8AddressIP->DPA_GetLocalAddress(&guidDevice, &usPort);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Header, provider, and port failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)

		//Verify the Device Guid that was returned.
		if(guidDevice != GUID_UNKNOWN)
		{
			DPTEST_FAIL(hLog, "GUID retrieved is unexpected (%u != %u)!", 2,guidDevice, GUID_UNKNOWN);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (failed setting device)

		//Verify the Device Guid that was returned.
		if(usPort != DPNSVR_PORT_I)
		{
			DPTEST_FAIL(hLog, "Port retrieved is unexpected (%u != %u)!", 2,usPort, DPNSVR_PORT_I);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
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
*/



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Building from Unicode URL using header & provider with unknown GUID");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->m_pDP8AddressIP->BuildFromURLW(DPNA_HEADER DPNA_KEY_PROVIDER L"=" ENCODED_TCPIP_GUID
													  L";" DPNA_KEY_HOSTNAME L"=" LOCALHOSTIP
													  L";" DPNA_KEY_DEVICE L"=" ENCODED_TCPIP_GUID
													  L";" DPNA_KEY_PORT L"=" L"0"
													  L";#" NO_ENCODE_DATA);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Building from Unicode URL using header & provider failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed building from W URL)


		
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Getting device with header, provider and port in address.");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		usPort = NULL;

		tr = pDP8AddressIP->DPA_GetLocalAddress(&guidDevice, &usPort);
		if (tr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Header, provider, and port failed!", 0);
			THROW_TESTRESULT;
		} // end if (failed setting device)

		//Verify the Device Guid that was returned.
		if(guidDevice != CLSID_DP8SP_TCPIP)
		{
			DPTEST_FAIL(hLog, "GUID retrieved is unexpected (%u != %u)!", 2,guidDevice, CLSID_DP8SP_TCPIP);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (failed setting device)

		//Verify the Device Guid that was returned.
		if(usPort != 0)
		{
			DPTEST_FAIL(hLog, "Port retrieved is unexpected (%u != %u)!", 2,usPort, 0);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (failed setting device)

/* XBOX - IDirectPlay8AddressInternal not supported
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8AddressInternal->Release();
		pDP8AddressInternal = NULL;
*/

/* XBOX - Interfaces are merged on Xbox, don't need to query
		TEST_SECTION("Releasing internal interface");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pNonwrappedMainDP8Address->Release();
		pNonwrappedMainDP8Address = NULL;
*/


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Releasing DirectPlay8Address object");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		tr = pDP8AddressIP->Release();
		if (tr != S_OK)
		{
			DPTEST_FAIL(hLog, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


		
	SAFE_LOCALFREE(pwszURL);

/* XBOX - No input and output data
	SAFE_LOCALFREE(pvSubInputData);
*/
/* XBOX - Interfaces are merged on Xbox, don't need to query
	SAFE_RELEASE(pNonwrappedMainDP8Address);
*/
/* XBOX - IDirectPlay8AddressInternal not supported
	SAFE_RELEASE(pDP8AddressInternal);
*/

	if (pDP8AddressIP != NULL)
	{
		// Ignore error
/* XBOX - No address list used
		g_pDP8AddressesList->RemoveFirstReference(pDP8AddressIP);
*/
		delete (pDP8AddressIP);
		pDP8AddressIP = NULL;
	} // end if (have server object)


	return (sr);
} // ParmVExec_IPGetLocalAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

} // namespace DPlayCoreNamespace