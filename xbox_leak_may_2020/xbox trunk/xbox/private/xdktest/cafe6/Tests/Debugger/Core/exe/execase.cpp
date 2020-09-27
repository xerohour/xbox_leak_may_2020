///////////////////////////////////////////////////////////////////////////////
//	execase.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "execase.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(CexeIDETest, CDebugTestSet, "Execution base", -1, CexeSubSuite)
												 
void CexeIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CexeIDETest::Run()
{	 
	XSAFETY;
	
	if( SetProject("testgo01\\testgo01"))
	{	  
		XSAFETY;
		GoAndVerify();
		XSAFETY;
		RestartAndGo();
		XSAFETY;
		RestartStopDebugging();
		XSAFETY;
		StepOverFuncToBreakAtLocBP();
		XSAFETY;
		StepOverFuncToBreakOnExprTrueBP();
		XSAFETY;
		StepOverFuncToBreakOnExprChangesBP();
		XSAFETY;
	}
		
	if( SetProject("testbp01\\testbp01") )
	{	  
		XSAFETY;
		StepOutSource();
		XSAFETY;
		StepOverSource();
		XSAFETY;
		StepIntoSource();
		XSAFETY;
		StepToCursorSource();
		XSAFETY;
	}
		
	// tests against example x

	if ( SetProject("dbg\\dbg") )
	{
		XSAFETY;
//		StartDebugging();		// WinslowF

		EXPECT(StepModel());
		XSAFETY;
		EXPECT(BreakStepTrace());
		XSAFETY;
	}
	StopDbgCloseProject();
	Sleep(1000);
	
/* bug # 1347 is postponed for after V50
	if( SetProject("dbga\\dbga") )
	{	  
		XSAFETY;
		TraceIntoWndProc();
		XSAFETY;
	}
*/
}





