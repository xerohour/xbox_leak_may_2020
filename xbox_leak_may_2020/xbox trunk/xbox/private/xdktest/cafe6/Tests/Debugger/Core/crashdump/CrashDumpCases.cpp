///////////////////////////////////////////////////////////////////////////////
//	CrashDumpCases.cpp
//											 
//	Created by: MichMa			Date: 9/30/97	
//
//	Description:								 
//		Implementation of the CCrashDumpCases class
		 
#include "stdafx.h"
#include "CrashDumpCases.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
IMPLEMENT_TEST(CCrashDumpCases, CDebugTestSet, "Crash Dump", -1, CCrashDumpSubsuite)
									   
												 
void CCrashDumpCases::PreRun(void)

{
	// call the base class
	CTest::PreRun();

}


void CCrashDumpCases::Run(void)

{
	if(!InitProject("crashdump\\crashdump"))
		{
		m_pLog->RecordInfo("ERROR: could not init crashdump project");
		return;
		}

	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_EXCEPTION));
	EXPECT_TRUE(stk.CurrentFunctionIs("dllfunc()"));
	EXPECT_TRUE(stk.FunctionIs("WinMain", 1));
	EXPECT_TRUE(cxx.ExpressionValueIs("zero", 0));
	EXPECT_TRUE(cxx.SetExpressionValue("zero", 1));
	EXPECT_TRUE(mem.MemoryDataIs("zero", 1));
	EXPECT_TRUE(mem.SetMemoryData("zero", 0));
	EXPECT_TRUE(cxx.ExpressionValueIs("zero", 0));
	EXPECT_TRUE(stk.NavigateStack(1));
	EXPECT_TRUE(cxx.ExpressionValueIs("nCmdShow", 1));
	EXPECT_TRUE(stk.NavigateStack(0));
	EXPECT_TRUE(cxx.ExpressionValueIs("zero", 0)); 
} 
	     
