///////////////////////////////////////////////////////////////////////////////
//	bpcase.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "bpcase.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(CbpIDETest, CDebugTestSet, "Breakpoints base", -1, CbpSubSuite)
												 
void CbpIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CbpIDETest::Run()
{
	XSAFETY;
  
	if( SetProject("testbp01\\testbp01") )
	{	  
		XSAFETY;
		BreakAtLine();
		XSAFETY;
		Disable_Enable();
		XSAFETY;
	}
 
	// breakpoints against example x
	// TODO: use coproject to see if it is already open.
	if ( SetProject("dbg\\dbg") )
	{
		XSAFETY;
		StartDebugging();

		EXPECT_TRUE(BPTable());
		XSAFETY;
		EXPECT_TRUE(LocationBPWithExp());
		XSAFETY;
		if(GetUserTargetPlatforms() != PLATFORM_MAC_68K && 
			GetUserTargetPlatforms() != PLATFORM_MAC_PPC)
		{
			EXPECT_TRUE(MessageBP());
			XSAFETY;
		}
		EXPECT_TRUE(BreakStepTrace());
		XSAFETY;
	}
	//	StopDbgCloseProject();
	//	Sleep(1000);
 	
	if( SetProject("testgo01\\testgo01") )
	{	  
		XSAFETY;
		BreakOnExpressionStaticVars();
		XSAFETY;
		BreakOnCompoundExpression();
		XSAFETY;
	}

	if( SetProject("dbga\\dbga") )
	{	  
		XSAFETY;
		BreakAtVirtualFunc();
		XSAFETY;
	}
		
}
