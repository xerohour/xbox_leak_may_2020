///////////////////////////////////////////////////////////////////////////////
//	execase.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "execase2.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(Cexe2IDETest, CDebugTestSet, "Execution special", -1, CexeSubSuite)
												 
void Cexe2IDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void Cexe2IDETest::Run()
{	 
	XSAFETY;
	
	if( SetProject("testgo01\\testgo01"))
	{	  
		XSAFETY;
		GoExeDoesNotExist();
		XSAFETY;
		ExitWhileDebugging();
		XSAFETY;
	}
		
	if (SetProject("dllapp\\incdll", PROJECT_DLL) && SetProject("dllapp\\powdll", PROJECT_DLL)
		&& SetProject("dllapp\\dec2", PROJECT_DLL) && SetProject("dllapp\\dllappx"))
	{
		XSAFETY;
//		StartDebugging();		// WinslowF
			
		EXPECT_TRUE(DllStepTrace());
		XSAFETY;
		EXPECT_TRUE(DllStepOut());
		XSAFETY;
		EXPECT_TRUE(DllTraceOut());
		XSAFETY;
		EXPECT_TRUE(CannotFindDll());
		XSAFETY;
		EXPECT_TRUE(StepOverFuncInUnloadedDll());
		XSAFETY;
		EXPECT_TRUE(RestartFromWithinDll());
		XSAFETY;
		EXPECT_TRUE(GoFromWithinDll());
		XSAFETY;
	}
	//	StopDbgCloseProject();
		Sleep(1000);

	if( SetProject("cons01\\cons01"))
	{	  
		XSAFETY;
		StdInOutRedirection();
		XSAFETY;
	}
 
	if( SetProject("dbga\\dbga"))
	{	  
		XSAFETY;
		NoSystemCodeStepping();
		XSAFETY;
	}
	
}

