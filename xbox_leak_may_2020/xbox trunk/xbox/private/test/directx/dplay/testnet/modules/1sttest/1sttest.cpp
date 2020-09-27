//==================================================================================
// Includes
//==================================================================================
#include <windows.h>


#include "debugprint.h"
#include "cppobjhelp.h"
#include "linklist.h"
#include "linkedstr.h"

#include "tncontrl.h"


//#include "..\main.h"
#include "t_main.h"

#include "1sttest.h"






//==================================================================================
// Prototypes
//==================================================================================
HRESULT FirstTestExec_Basic(PTNEXECCASEDATA pTNecd);





#undef DEBUG_SECTION
#define DEBUG_SECTION	"FirstTestLoadTestTable()"
//==================================================================================
// FirstTestLoadTestTable
//----------------------------------------------------------------------------------
//
// Description: Loads all the possible tests into the table passed in:
//				1.1			First tests
//				1.1.1		Simple test A
//
// Arguments:
//	PTNTESTTABLEGROUP pTable	Group/table to fill with tests in this file.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FirstTestLoadTestTable(PTNTESTTABLEGROUP pTable)
{
	PTNTESTTABLEGROUP	pSubGroup;
	TNADDTESTDATA		tnatd;



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 1.1		First tests
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pTable, "1.1", "First tests", &pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 1.1.1	Simple test A
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "1.1.1";
	tnatd.pszName			= "Simple test A";
	tnatd.pszDescription	= "This is a template for a test";
	tnatd.pszInputDataHelp	= NULL;

	tnatd.iNumMachines		= 1;
	tnatd.dwOptionFlags		= TNTCO_API | TNTCO_TOPLEVELTEST;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= NULL;
	tnatd.pfnExecCase		= FirstTestExec_Basic;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);


	return (S_OK);
} // FirstTestLoadTestTable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"FirstTestExec_Basic()"
//==================================================================================
// FirstTestExec_Basic
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				1.1.1 - Simple test A
//
// Arguments:
//	PTNEXECCASEDATA lpTNecd	Pointer to structure with parameters for test case.
//
// Expected input data:
//	None.
//
// Output data:
//	None.
//
// Dynamic variables set:
//	None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT FirstTestExec_Basic(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult		sr;
	CTNTestResult		tr;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Function 1");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Call the first function you want to test
//		tr = FunctionToTest1();

		// For our example, we say if the call didn't return S_OK then the test is a
		// failure.  We will make note of the failure, and stop executing any more
		// of the test.  The END_TESTCASE macro will report this failure for us, we
		// just have to make sure we clean up after/outside of END_TESTCASE.
//		if (tr != S_OK)
//		{
//			DPL(0, "FunctionToTest1 didn't return S_OK!", 0);
//			THROW_TESTRESULT;
//		} // end if (we failed)

		tr = S_OK;

		// Otherwise, we can report that this function succeeded, but continue on.
		// Note that this is optional.  You may choose not to have any reporting
		// except the final exit sucess/failure report.

		sr = pTNecd->pExecutor->Report(tr, TRUE);
		if (sr != S_OK)
		{
			DPL(0, "Reporting FunctionToTest1 success failed!", 0);
			THROW_SYSTEMRESULT;
		} // end if (reporting failed)

		OutputDebugString(L"Function 1 complete!\r\n");


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Function 2");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


		// Call the second function you want to test
//		tr = FunctionToTest2(NULL);

		// For our example, we say that if the call didn't return the appropriate
		// error ERROR_INVALID_PARAMETER then the test is a failure.  We will stop
		// executing any more of the test and report this result.
//		if (tr != ERROR_INVALID_PARAMETER)
//		{
//			DPL(0, "FunctionToTest2 didn't return ERROR_INVALID_PARAMETER!", 0);
//			THROW_TESTRESULT;
//		} // end if (we failed)

		tr = S_OK;

		// Otherwise, we can report that this function succeeded, but continue on.
		// Note that this is optional.  You may choose not to have any reporting
		// except the final exit sucess/failure report.

		sr = pTNecd->pExecutor->Report(tr, TRUE);
		if (sr != S_OK)
		{
			DPL(0, "Reporting FunctionToTest2 results failed!", 0);
			THROW_SYSTEMRESULT;
		} // end if (reporting failed)

		OutputDebugString(L"Function 2 complete!\r\n");


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Function 3");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


		// Call the third function you want to test
//		tr = FunctionToTest3();

		// If the call didn't return S_OK then the test is a failure.  Since this is
		// the last test, we use this as our final result.
//		if (tr != S_OK)
//		{
//			DPL(0, "FunctionToTest3 didn't return S_OK!", 0);
//			THROW_TESTRESULT;
//		} // end if (we failed)

		tr = S_OK;

		// Otherwise, we can report that this function succeeded, but continue on.
		// Note that this is optional.  You may choose not to have any reporting
		// except the final exit sucess/failure report.

		sr = pTNecd->pExecutor->Report(tr, TRUE);
		if (sr != S_OK)
		{
			DPL(0, "Reporting FunctionToTest3 results failed!", 0);
			THROW_SYSTEMRESULT;
		} // end if (reporting failed)

		OutputDebugString(L"Function 3 complete!\r\n");

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		TEST_SECTION("Cleanup");
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Do cleanup.  We should check the error codes here, as opposed to below
		// where we don't want to mash a possibly already existing error.
		


		// Now report the fact the test case is done and successful.
		FINAL_SUCCESS;
	}
	END_TESTCASE


	// Do any cleanup that may be necessary.  Note that you should ignore error
	// codes or use a temp hresult and call the OVERWRITE_SR_IF_OK macro since you
	// may reach here with an error code already, and you don't want to stomp it.
	// Also do not use the THROW macros since we are outside the catch handler.

	return (sr);
} // FirstTestExec_Basic
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
