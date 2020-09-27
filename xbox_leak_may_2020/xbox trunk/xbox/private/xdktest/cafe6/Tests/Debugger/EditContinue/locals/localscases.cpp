///////////////////////////////////////////////////////////////////////////////
//	localscases.cpp
//											 
//	Created by:			Date:			
//		MichMa				10/22/97	
//
//	Description:								 
//		Implementation of CECLocalsCases		 

#include "stdafx.h"
#include "localscases.h"
#include "..\cleanup.h"
#include "assert.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
							   
IMPLEMENT_TEST(CECLocalsCases, CECTestSet, "Edit & Continue: Locals", -1, CECLocalsSubSuite)


void CECLocalsCases::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}


// tests for adding locals.
CArray<CAddTest, CAddTest> AddTests;
// tests for changing the assignment value of locals.
CArray<CValueTest, CValueTest> ValueTests;
// tests for changing the type of locals.
CArray<CTypeTest, CTypeTest> TypeTests;
// tests for removing locals.
CArray<CRemoveTest, CRemoveTest> RemoveTests;

// the functions that can be edited.
CArray<CFunc, CFunc> Funcs;
// used as the value for the int pointer tests.
CString strGlobalIntAddress;
// tracks how many tests have been performed.
int giTestsPerformed = 0;


void CECLocalsCases::Run()

{
	// setup some paths we will use.
	if(GetSystem() & SYSTEM_DBCS)
		m_strProjBase = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü ec locals app";
	else
		m_strProjBase = "ec locals app";

	m_strSrcFullPath = GetCWD() + "src\\ec locals app";
	m_strProjDirFullPath = m_strSrcFullPath + "\\" + m_strProjBase;

	// clean-up from the last run.
	OurKillFile(m_strProjDirFullPath + "\\Debug\\", "*.*");
	RemoveDirectory(m_strProjDirFullPath + "\\Debug");
	OurKillFile(m_strProjDirFullPath + "\\", "*.*");

	// set the options for the project we are building for this test.
	CProjWizOptions *pWin32AppWizOpt = new(CWin32AppWizOptions);
	pWin32AppWizOpt->m_strLocation = m_strSrcFullPath;
	pWin32AppWizOpt->m_strName = m_strProjBase;	
	
	// create the project.
	EXPECT_SUCCESS(proj.New(pWin32AppWizOpt));

	// create a new exe source file (to be edited) from the base exe source file.
	CopyFile(m_strSrcFullPath + "\\" + "base exe.cpp", m_strProjDirFullPath + "\\" + m_strProjBase + " exe.cpp", FALSE);
	// this appears to be a win32 timing problem. when running the test over the net, the server seems to keep the file 
	// locked even after CopyFile returns. an immediate call to SetFileAttributes will fail with a sharing violation.
	// so we wait a second here to make sure the server is totally finished processing the CopyFile request.
	Sleep(1000);
	// make the source file writable so we can edit it.
	SetFileAttributes(m_strProjDirFullPath + "\\" + m_strProjBase + " exe.cpp", FILE_ATTRIBUTE_NORMAL);
	
	// add source to the project.
	EXPECT_SUCCESS(proj.AddFiles(m_strProjBase + " exe.cpp"));
	// build the project.
	EXPECT_SUCCESS(proj.Build());
	// get the project into the intial debugging state.
	EXPECT_TRUE(dbg.StepInto());
	// get the initial address of the global int for any int pointer tests that happen before the next restart.
	EXPECT_TRUE(cxx.GetExpressionValue("&intGlobal", strGlobalIntAddress)); 
	
	// initialize info for the tests to perform and the functions to perform them in.
	InitTests();
	InitFuncs();

	// we only want to randomly do each test once.
	RandomNumberCheckList rnclTests(AddTests.GetSize() + 
									ValueTests.GetSize() + 
									TypeTests.GetSize() + 
									RemoveTests.GetSize());
	
	// the index of the next randomly selected test.
	int iTest;
	// flags whether or not a selected test was done.
	BOOL bTestDone;

	// the ranges of the different types of locals tests within the random number checklist.
	int iAddTestsStart = 0;
	int iAddTestsEnd = AddTests.GetSize() - 1;
	int iValueTestsStart = iAddTestsEnd + 1;
	int iValueTestsEnd = iValueTestsStart + ValueTests.GetSize() - 1;
	int iTypeTestsStart = iValueTestsEnd + 1;
	int iTypeTestsEnd = iTypeTestsStart + TypeTests.GetSize() - 1;
	int iRemoveTestsStart = iTypeTestsEnd + 1;
	int iRemoveTestsEnd = iRemoveTestsStart + RemoveTests.GetSize() - 1;

	// randomly do each test once.
	while((iTest = rnclTests.GetNext()) != -1)
	
	{
		// check if the test index selected is within the range of an Add test.
		if((iTest >= iAddTestsStart) && (iTest <= iAddTestsEnd) && (AddTests.GetSize() > 0))
			bTestDone = DoTest(AddTests[iTest - iAddTestsStart]);
		// check if the test index selected is within the range of a Value test.
		else if((iTest >= iValueTestsStart) && (iTest <= iValueTestsEnd) && (ValueTests.GetSize() > 0))
			bTestDone = DoTest(ValueTests[iTest - iValueTestsStart]);
		// check if the test index selected is within the range of a Type test.
		else if((iTest >= iTypeTestsStart) && (iTest <= iTypeTestsEnd) && (TypeTests.GetSize() > 0))
			bTestDone = DoTest(TypeTests[iTest - iTypeTestsStart]);
		// check if the test index selected is within the range of a Remove test.
		else if((iTest >= iRemoveTestsStart) && (iTest <= iRemoveTestsEnd) && (RemoveTests.GetSize() > 0))
			bTestDone = DoTest(RemoveTests[iTest - iRemoveTestsStart]);

		// if we were able to do the test, reset the debugger to the inital state.
		if(bTestDone)
			{EXPECT_TRUE(GetBackToStartOfWinMain());}
		// if we were unable to do the test at this time, make its index available again.
		else
			rnclTests.UndoLast();
	}

	// clean up.
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_SUCCESS(proj.Close());
}


void CECLocalsCases::InitFuncs(void)

{
	CFunc Func;
	Func.m_strFile = m_strProjBase + " exe.cpp";

	Func.m_strName = "WinMain";
	Func.m_strChild = "ExeFunc1";
	Funcs.Add(Func);

	Func.m_strName = "ExeFunc1";
	Func.m_strChild = "ExeFunc6";
	Funcs.Add(Func);

	Func.m_strName = "ExeFunc2";
	Func.m_strChild = "ExeFunc7";
	Funcs.Add(Func);

	Func.m_strName = "ExeFunc3";
	Func.m_strChild = "ExeFunc8";
	Funcs.Add(Func);

	Func.m_strName = "ExeFunc4";
	Func.m_strChild = "ExeFunc9";
	Funcs.Add(Func);

	Func.m_strName = "ExeFunc5";
	Func.m_strChild = "ExeFunc10";
	Funcs.Add(Func);

	Func.m_strChild = "";

	Func.m_strName = "ExeFunc6";
	Funcs.Add(Func);

	Func.m_strName = "ExeFunc7";
	Funcs.Add(Func);

	Func.m_strName = "ExeFunc8";
	Funcs.Add(Func);

	Func.m_strName = "ExeFunc9";
	Funcs.Add(Func);

	Func.m_strName = "ExeFunc10";
	Funcs.Add(Func);
}


void CECLocalsCases::InitTests(void)

{	
	//******************************* ADD TESTS ********************************

	CAddTest AddTest;
	AddTest.m_bTestDone = FALSE;
	AddTest.m_Local.m_bUsesCtorDtor = FALSE;

	// add an int variable.
	AddTest.m_Local.m_strType = "int";
	AddTest.m_Local.m_strName = "intLocal";
	AddTest.m_Local.m_strValue = "0";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	// add a second int variable since the value and type tests use int vars.
	// we want to make sure enough int vars are provided for those tests.
	AddTest.m_Local.m_strType = "int";
	AddTest.m_Local.m_strName = "intLocal2";
	AddTest.m_Local.m_strValue = "0";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	// add an enum variable.
	AddTest.m_Local.m_strType = "ENUM1";
	AddTest.m_Local.m_strName = "ENUM1Local";
	AddTest.m_Local.m_strValue = "ENUM1_VAL1";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 
	
	// add a derived class instance.
	// TODO(michma): add initialization and verification of member.
	AddTest.m_Local.m_strType = "CDerived";
	AddTest.m_Local.m_strName = "CDerivedLocal";
	AddTest.m_Local.m_strValue = "";
	AddTest.m_Local.m_ExprState = COLLAPSED;
	AddTest.m_Local.m_strVerifyValue = "{...}";
	AddTest.m_Local.m_bUsesCtorDtor = TRUE;
	AddTest.m_Local.m_strCodeLines.Add("CDerivedLocal.m_intCDerived = 0;");

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_strVerifyValue = "";
	AddTest.m_Local.m_bUsesCtorDtor = FALSE;
	AddTest.m_Local.m_strCodeLines.RemoveAll();

	// add a union variable.
	// TODO(michma): add initialization and verification of member.
	AddTest.m_Local.m_strType = "UNION1";
	AddTest.m_Local.m_strName = "UNION1Local";
	AddTest.m_Local.m_strValue = "";
	AddTest.m_Local.m_ExprState = COLLAPSED;
	AddTest.m_Local.m_strVerifyValue = "{...}";
	AddTest.m_Local.m_strCodeLines.Add("UNION1Local.m_intUNION1 = 0;");

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest);

	AddTest.m_Local.m_strVerifyValue = "";
	AddTest.m_Local.m_strCodeLines.RemoveAll();

	// TODO(michma): add a member of type double to a local union.

	// add an unsigned int variable.
	AddTest.m_Local.m_strType = "unsigned int";
	AddTest.m_Local.m_strName = "unsignedintLocal";
	AddTest.m_Local.m_strValue = "0";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest);

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest);

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest);

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest);

	// add an __int64 variable.
	AddTest.m_Local.m_strType = "__int64";
	AddTest.m_Local.m_strName = "int64Local";
	AddTest.m_Local.m_strValue = "0";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	// add a char variable.
	AddTest.m_Local.m_strType = "char";
	AddTest.m_Local.m_strName = "charLocal";
	AddTest.m_Local.m_strValue = "'a'";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;
	AddTest.m_Local.m_strVerifyValue = "97 'a'";

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_strVerifyValue = "";

	// add an unsigned char variable.
	AddTest.m_Local.m_strType = "unsigned char";
	AddTest.m_Local.m_strName = "unsignedcharLocal";
	AddTest.m_Local.m_strValue = "'a'";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;
	AddTest.m_Local.m_strVerifyValue = "97 'a'";

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_strVerifyValue = "";

	// add a short variable.
	AddTest.m_Local.m_strType = "short";
	AddTest.m_Local.m_strName = "shortLocal";
	AddTest.m_Local.m_strValue = "0";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	// add an unsigned short variable.
	AddTest.m_Local.m_strType = "unsigned short";
	AddTest.m_Local.m_strName = "unsignedshortLocal";
	AddTest.m_Local.m_strValue = "0";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest);
	
	// add a long variable.
	AddTest.m_Local.m_strType = "long";
	AddTest.m_Local.m_strName = "longLocal";
	AddTest.m_Local.m_strValue = "0";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	// add an unsigned long variable.
	AddTest.m_Local.m_strType = "unsigned long";
	AddTest.m_Local.m_strName = "unsignedlongLocal";
	AddTest.m_Local.m_strValue = "0";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest);

	// add a float variable.
	AddTest.m_Local.m_strType = "float";
	AddTest.m_Local.m_strName = "floatLocal";
	AddTest.m_Local.m_strValue = "0.0";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;
	AddTest.m_Local.m_strVerifyValue = "0.000000";

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest);

	AddTest.m_Local.m_strVerifyValue = "";

	// add a double variable.
	AddTest.m_Local.m_strType = "double";
	AddTest.m_Local.m_strName = "doubleLocal";
	AddTest.m_Local.m_strValue = "0.0";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;
	AddTest.m_Local.m_strVerifyValue = "0.00000000000000";

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest);

	AddTest.m_Local.m_strVerifyValue = "";

	// add a long double variable.
	AddTest.m_Local.m_strType = "long double";
	AddTest.m_Local.m_strName = "longdoubleLocal";
	AddTest.m_Local.m_strValue = "0.0";
	AddTest.m_Local.m_ExprState = NOT_EXPANDABLE;
	AddTest.m_Local.m_strVerifyType = "double";
	AddTest.m_Local.m_strVerifyValue = "0.00000000000000";

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest);

	AddTest.m_Local.m_strVerifyType = "";
	AddTest.m_Local.m_strVerifyValue = "";
	
	// add a base class instance.
	// TODO(michma): add initialization and verification of member.
	AddTest.m_Local.m_strType = "CBase";
	AddTest.m_Local.m_strName = "CBaseLocal";
	AddTest.m_Local.m_strValue = "";
	AddTest.m_Local.m_ExprState = COLLAPSED;
	AddTest.m_Local.m_strVerifyValue = "{...}";
	AddTest.m_Local.m_bUsesCtorDtor = TRUE;
	AddTest.m_Local.m_strCodeLines.Add("CBaseLocal.m_intCBase = 0;");

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest);

	AddTest.m_Local.m_strVerifyValue = "";
	AddTest.m_Local.m_bUsesCtorDtor = FALSE;
	AddTest.m_Local.m_strCodeLines.RemoveAll();

	// add a struct instance.
	// TODO(michma): add initialization and verification of member.
	AddTest.m_Local.m_strType = "STRUCT1";
	AddTest.m_Local.m_strName = "STRUCT1Local";
	AddTest.m_Local.m_strValue = "";
	AddTest.m_Local.m_ExprState = COLLAPSED;
	AddTest.m_Local.m_strVerifyValue = "{...}";
	AddTest.m_Local.m_strCodeLines.Add("STRUCT1Local.m_intSTRUCT1 = 0;");

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest);

	AddTest.m_Local.m_strVerifyValue = "";
	AddTest.m_Local.m_strCodeLines.RemoveAll();

	// add an int pointer.
	AddTest.m_Local.m_strType = "int *";
	AddTest.m_Local.m_strName = "pintLocal";
	AddTest.m_Local.m_strValue = "&intGlobal";
	AddTest.m_Local.m_ExprState = COLLAPSED;
	AddTest.m_Local.m_strVerifyValue = strGlobalIntAddress;

	AddTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	AddTests.Add(AddTest);
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 
	
	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	AddTests.Add(AddTest); 

	AddTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	AddTests.Add(AddTest);

	AddTest.m_Local.m_strVerifyValue = "";

	//******************************* VALUE TESTS ********************************

	CValueTest ValueTest;
	ValueTest.m_bTestDone = FALSE;
	ValueTest.m_bAddAssignment = FALSE;

	// change the value assignment of an int variable to its top boundary.
	ValueTest.m_Local.m_strType = "int";
	ValueTest.m_Local.m_strValue = "2147483647";
	
	ValueTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	ValueTests.Add(ValueTest);
	
	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	ValueTests.Add(ValueTest); 
	
	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	ValueTests.Add(ValueTest); 

	// add a value assignment to the declaration of an int variable.
	ValueTest.m_Local.m_strType = "int";
	ValueTest.m_Local.m_strValue = "1";
	ValueTest.m_bAddAssignment = TRUE;
	
	ValueTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	ValueTests.Add(ValueTest);

	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	ValueTests.Add(ValueTest); 
	
	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	ValueTests.Add(ValueTest); 

	// TODO(michma - 2/22/98): this case isn't possible, is it? you can't put the ip at a simple declaration.
	//ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	//ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	ValueTests.Add(ValueTest); 

	ValueTest.m_bAddAssignment = FALSE;

	// change the value assignment of an int variable to its low boundary.
	ValueTest.m_Local.m_strType = "int";
	ValueTest.m_Local.m_strValue = "-2147483648";
	
	ValueTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	ValueTests.Add(ValueTest);

	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	ValueTests.Add(ValueTest); 
	
	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	ValueTests.Add(ValueTest); 

	// remove the value assignment of an int variable.
	ValueTest.m_Local.m_strType = "int";
	ValueTest.m_Local.m_strValue = "";
	ValueTest.m_Local.m_strVerifyValue = "UNKNOWN";
	
	ValueTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	ValueTests.Add(ValueTest);

	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	ValueTests.Add(ValueTest); 
	
	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	ValueTests.Add(ValueTest); 

	// TODO(michma - 2/23/98): we can't undo this case, so disable it for now (see TODO above).
	//ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	//ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	ValueTests.Add(ValueTest);
	
	ValueTest.m_Local.m_strVerifyValue = "";

	// change the value assignment of an int variable to 0.
	ValueTest.m_Local.m_strType = "int";
	ValueTest.m_Local.m_strValue = "0";
	
	ValueTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	ValueTests.Add(ValueTest);
	
	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	ValueTests.Add(ValueTest); 
	
	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	ValueTests.Add(ValueTest); 

	ValueTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	ValueTests.Add(ValueTest); 

	//******************************* TYPE TESTS ********************************
/*
	CTypeTest TypeTest;
	TypeTest.m_bTestDone = FALSE;

	// change the type of an int variable to __int64.
	TypeTest.m_Local.m_strType = "int";
	TypeTest.m_strNewType = "__int64";
	
	TypeTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	TypeTests.Add(TypeTest);
	
	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	TypeTests.Add(TypeTest); 
	
	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	TypeTests.Add(TypeTest); 

	// change the type of a long double variable to a short.
	TypeTest.m_Local.m_strType = "long double";
	TypeTest.m_strNewType = "short";
	TypeTest.m_Local.m_strVerifyType = "";
	TypeTest.m_Local.m_strVerifyValue = "0";
	
	TypeTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	TypeTests.Add(TypeTest);
	
	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	TypeTests.Add(TypeTest); 
	
	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_strVerifyValue = "";

	// change the type of an int variable to float.
	TypeTest.m_Local.m_strType = "int";
	TypeTest.m_strNewType = "float";
	TypeTest.m_Local.m_strVerifyValue = "0.000000";
	
	TypeTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	TypeTests.Add(TypeTest);
	
	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	TypeTests.Add(TypeTest); 
	
	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_strVerifyValue = "";

	// change the type of an __int64 variable to char.
	TypeTest.m_Local.m_strType = "__int64";
	TypeTest.m_strNewType = "char";
	TypeTest.m_Local.m_strVerifyValue = "0 ''";
	
	TypeTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	TypeTests.Add(TypeTest);
	
	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	TypeTests.Add(TypeTest); 
	
	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	TypeTests.Add(TypeTest); 

	// change the type of an short variable to long double.
	TypeTest.m_Local.m_strType = "short";
	TypeTest.m_strNewType = "long double";
	TypeTest.m_Local.m_strVerifyType = "double";
	TypeTest.m_Local.m_strVerifyValue = "0.00000000000000";
	
	TypeTest.m_Local.m_ecEditType = EC_NO_STACK_FRAME;
	TypeTests.Add(TypeTest);
	
	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	TypeTests.Add(TypeTest); 
	
	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	TypeTests.Add(TypeTest); 

	TypeTest.m_Local.m_strVerifyValue = "";

	//******************************* REMOVE TESTS *****************************
	
	CRemoveTest RemoveTest;
	RemoveTest.m_bTestDone = FALSE;
	RemoveTest.m_bLeaveReferences = FALSE;

	// remove one or more locals, including references.
	RemoveTest.m_ecEditType = EC_NO_STACK_FRAME;
	RemoveTests.Add(RemoveTest);
	
	RemoveTest.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	RemoveTests.Add(RemoveTest); 
	
	RemoveTest.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	RemoveTests.Add(RemoveTest); 

	RemoveTest.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	RemoveTests.Add(RemoveTest); 

	RemoveTest.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	RemoveTests.Add(RemoveTest); 

	RemoveTest.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	RemoveTests.Add(RemoveTest); 

	// TODO(michma): this isn't finished yet.
	// remove a local and leave the references.
	RemoveTest.m_bLeaveReferences = TRUE;

	RemoveTest.m_ecEditType = EC_NO_STACK_FRAME;
	RemoveTests.Add(RemoveTest);
	
	RemoveTest.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	RemoveTests.Add(RemoveTest); 
	
	RemoveTest.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	RemoveTests.Add(RemoveTest); 

	RemoveTest.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	RemoveTests.Add(RemoveTest); 

	RemoveTest.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	RemoveTests.Add(RemoveTest); 

	RemoveTest.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	RemoveTests.Add(RemoveTest); 

	RemoveTest.m_bLeaveReferences = FALSE;
*/
}


//********************************** ADD TESTS *********************************

BOOL CECLocalsCases::DoTest(CAddTest &AddTest)

{
	// randomly select a function in which to perform the test. the function must not already have the local in it.
	CFunc &Func = Funcs[RandomlySelectFunctionToEdit(AddTest)];
	// this test will be performed.
	giTestsPerformed++;

	// log the test header.
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("TEST %d: Adding local \"%s\" to function \"%s\" using edit type \"%s\".", 
					   giTestsPerformed, AddTest.m_Local.m_strName, Func.m_strName, 
					   szecEditTypes[AddTest.m_Local.m_ecEditType]);
	
	// each type of edit requires different steps.
	switch(AddTest.m_Local.m_ecEditType)
	
	{
		// edited function does not have an active stack frame.
		case EC_NO_STACK_FRAME: 
		{
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// insert the local after the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			AddLocalToSource(AddTest.m_Local);
			// TODO(michma): step to cursor fails sometimes, so apply code changes separately first.
			ApplyCodeChanges();
			break;
		}

		// edited function is active on top of the stack. edit occurs after the ip.
		case EC_CURRENT_FRAME_AFTER_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// run to the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// insert the local after the first line of the function.
			AddLocalToSource(AddTest.m_Local);
			// TODO(michma): step to cursor fails sometimes, so apply code changes separately first.
			ApplyCodeChanges();
			break;
		}

		// edited function is active on top of the stack. edit occurs before the ip.
		case EC_CURRENT_FRAME_BEFORE_IP:

		{
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// run to the last line of the function.
			EXPECT_TRUE(src.Find("last line of " + Func.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// record the parent function for later comparison.
			CString strParentFunc = stk.GetFunction(1, TRUE);
			// insert the local after the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()", FALSE, FALSE, FALSE, FIND_UP));
			AddLocalToSource(AddTest.m_Local);

			// when adding locals that use constructors/destructors (classes), we get a message about the new local
			// not being available until the function is re-entered..
			if(AddTest.m_Local.m_bUsesCtorDtor)
			{
				// step out of the function (handle message about only disasm being available).
				EXPECT_TRUE(dbg.StepOut(1, NULL, NULL, NULL, NOWAIT));
				EXPECT_TRUE(dbg.HandleMsg(MSG_EC_DELAYED_DUE_TO_CTOR_DTOR));
				EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
			}
			// TODO(michma): sometimes get "execution point changed..." message doing step out.
			// TODO(michma): step out fails, so apply go changes first then step out.
			else
				EXPECT_TRUE(dbg.StepOut());

			// verify the debugger landed on the correct line and function.
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to " + Func.m_strName + "()"));
			EXPECT_TRUE(stk.CurrentFunctionIs(strParentFunc));
			// reset the ip to the call into the.edited function.
			EXPECT_TRUE(src.Find(Func.m_strName + "();", FALSE, FALSE, FALSE, FIND_UP));
			EXPECT_TRUE(dbg.SetNextStatement());
			break;
		}

		// edited function is active on top of the stack. edit occurs at the ip.
		case EC_CURRENT_FRAME_AT_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// run to the last line of the function.
			EXPECT_TRUE(src.Find("last line of " + Func.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// insert the local before the last line of the function.
			AddLocalToSource(AddTest.m_Local);
			// TODO(michma): step to cursor fails sometimes, so apply code changes separately first.
			ApplyCodeChanges();
			break;
		}

		// edited function is active, but not on top of the stack. edit occurs after the ip.
		case EC_PARENT_FRAME_AFTER_IP:
		{
			// get the child of the function to edit.
			CFunc &FuncChild = Funcs[GetFunctionIndexFromName(Func.m_strChild)];
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the child of the function to edit.
			EXPECT_SUCCESS(src.Open(FuncChild.m_strFile));
			// run to the first line of the child of the function to edit.
			EXPECT_TRUE(src.Find("first line of " + FuncChild.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// insert the local before the last line of the function.
			EXPECT_TRUE(src.Find("last line of " + Func.m_strName + "()"));
			AddLocalToSource(AddTest.m_Local);
			// step out of the child function.
			EXPECT_TRUE(dbg.StepOut());
			// verify we are back in the edited function on the correct line.
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to " + FuncChild.m_strName + "()"));
			EXPECT_TRUE(stk.CurrentFunctionIs(Func.m_strName));
			break;
		}

		// edited function is active, but not on top of the stack. edit occurs before the ip.
		case EC_PARENT_FRAME_BEFORE_IP:
		
		{
			// get the child of the function to edit.
			CFunc &FuncChild = Funcs[GetFunctionIndexFromName(Func.m_strChild)];
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the child of the function to edit.
			EXPECT_SUCCESS(src.Open(FuncChild.m_strFile));
			// run to the first line of the child of the function to edit.
			EXPECT_TRUE(src.Find("first line of " + FuncChild.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// insert the local after the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			AddLocalToSource(AddTest.m_Local);

			// when adding locals that use constructors/destructors (classes), e&c requires re-entrance to see changes.
			if(AddTest.m_Local.m_bUsesCtorDtor)
			{
				// record the parent (3rd frame from the top) of the edited function for later comparison.
				CString strParentFunc = stk.GetFunction(2, TRUE);
				// step out of the child function (handle message about only disasm being available).
				EXPECT_TRUE(dbg.StepOut(1, NULL, NULL, NULL, NOWAIT));
				EXPECT_TRUE(dbg.HandleMsg(MSG_EC_DELAYED_DUE_TO_CTOR_DTOR));
				EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
				// step out of the edited function so we can re-enter it to see the new local.
				EXPECT_TRUE(dbg.StepOut());
				EXPECT_TRUE(stk.CurrentFunctionIs(strParentFunc));
				// get back into source mode and reset the ip to the call into the.edited function.
				dbg.SetSteppingMode(SRC);
				EXPECT_TRUE(src.Find(Func.m_strName + "();", FALSE, FALSE, FALSE, FIND_UP));
				EXPECT_TRUE(dbg.SetNextStatement());
			}
			// adding locals that don't use constructors/destructors requires no re-entrance to see the locals.
			else
			{
				// step out of the child function.
				EXPECT_TRUE(dbg.StepOut());
				// verify the debugger is at the correct line and function.
				EXPECT_TRUE(dbg.CurrentLineIs("line after call to " + FuncChild.m_strName + "()"));
				EXPECT_TRUE(stk.CurrentFunctionIs(Func.m_strName));
			}

			break;
		}
	}

	// run to the last line of the function to verify locals.
	// TODO(michma): randomly select either step to cursor or bp.
	EXPECT_TRUE(src.Find("last line of " + Func.m_strName + "()"));
	EXPECT_TRUE(dbg.StepToCursor());
	// verify the debugger stopped at the correct line and function.
	EXPECT_TRUE(dbg.CurrentLineIs("last line of " + Func.m_strName + "()"));
	EXPECT_TRUE(stk.CurrentFunctionIs(Func.m_strName));
	// record that this local is now in this function (for verification later).
	Func.m_Locals.Add(AddTest.m_Local);

	// if the edit was made before the ip, then the value of the local just added is unknown for 
	// this first verification. so for locals that are actually initialized with a value, we need to substitute 
	// "UKNOWN" for their value this one time. the assigned value is stored in a temp variable for restoration later.
	// note that variables using contructors or destructors are a special case since we must re-enter the edited function
	// to see source, and they aren't initialized with values anyway.
	CString strTempValue;
	CString strTempVerifyValue;
	int iLastLocalAdded = Func.m_Locals.GetSize() - 1;

	if((Func.m_Locals[iLastLocalAdded].m_ecEditType == EC_CURRENT_FRAME_BEFORE_IP) || 
	   (Func.m_Locals[iLastLocalAdded].m_ecEditType == EC_PARENT_FRAME_BEFORE_IP))
	{
		if((Func.m_Locals[iLastLocalAdded].m_strVerifyValue != "") && 
		   (Func.m_Locals[iLastLocalAdded].m_strVerifyValue != "{...}"))
		{
			strTempVerifyValue = Func.m_Locals[iLastLocalAdded].m_strVerifyValue;
			Func.m_Locals[iLastLocalAdded].m_strVerifyValue = "UNKNOWN";
		}
		else if(Func.m_Locals[iLastLocalAdded].m_strValue != "")
		{
			strTempValue = Func.m_Locals[iLastLocalAdded].m_strValue;
			Func.m_Locals[iLastLocalAdded].m_strValue = "UNKNOWN";
		}
	}

	// verify the locals in the function.
	VerifyFunctionLocals(Func);
	
	// if we had to substitute a temporary value (see if block above), restore the original value.
	if(strTempVerifyValue != "")
		Func.m_Locals[iLastLocalAdded].m_strVerifyValue = strTempVerifyValue;
	else if(strTempValue != "")
		Func.m_Locals[iLastLocalAdded].m_strValue = strTempValue;

	// test was able to execute.
	AddTest.m_bTestDone = TRUE;
	return TRUE;
}


int CECLocalsCases::RandomlySelectFunctionToEdit(CAddTest &AddTest)

{
	// we only want to randomly check each function for suitability once.
	RandomNumberCheckList rnclFuncs(Funcs.GetSize());
	// the random number generated to index into the Funcs array.
	int iFunc;
		
	// randomly check each function for suitability once.
	while((iFunc = rnclFuncs.GetNext()) != -1)
	
	{
		// certain combinations of edit types and functions/callstacks are not compatible.
		if((!((AddTest.m_Local.m_ecEditType == EC_NO_STACK_FRAME) && 
			  (Funcs[iFunc].m_strName == "WinMain"))) &&

		   (!((AddTest.m_Local.m_ecEditType == EC_CURRENT_FRAME_BEFORE_IP) && 
			  (Funcs[iFunc].m_strName == "WinMain"))) &&

		   (!((AddTest.m_Local.m_ecEditType == EC_PARENT_FRAME_BEFORE_IP) && 
			  (Funcs[iFunc].m_strName == "WinMain"))) &&

		   (!((AddTest.m_Local.m_ecEditType == EC_PARENT_FRAME_AFTER_IP) &&
			  (Funcs[iFunc].m_strChild == ""))) &&

		   (!((AddTest.m_Local.m_ecEditType == EC_PARENT_FRAME_BEFORE_IP) &&
			  (Funcs[iFunc].m_strChild == ""))) &&

		   (!((AddTest.m_Local.m_ecEditType == EC_CURRENT_FRAME_AT_IP) &&
			  (Funcs[iFunc].m_strName == "WinMain") && 
			  (AddTest.m_Local.m_bUsesCtorDtor))))

		{
			// make sure a local of the specified name doesn't already exist in this function.
			for(int iLocal = 0; iLocal < Funcs[iFunc].m_Locals.GetSize(); iLocal++)
			{
				// if we find the local already in the function, start over by selecting a new function.
				if(Funcs[iFunc].m_Locals[iLocal].m_strName == AddTest.m_Local.m_strName)
					break;
			}

			// if the function doesn't already have the local in it, return its index.
			if(iLocal == Funcs[iFunc].m_Locals.GetSize())
				return iFunc;
		}
	}
	
	// we should always find a function to add the local to. Add tests are not dependent on other tests.
	ASSERT(0);
	return -1;
}


void CECLocalsCases::AddLocalToSource(CECLocal &Local)

{
	// set the cursor up according to the type of edit being performed.
	switch(Local.m_ecEditType)
	{
		case EC_NO_STACK_FRAME:
		case EC_CURRENT_FRAME_AFTER_IP:
		case EC_CURRENT_FRAME_BEFORE_IP:
		case EC_PARENT_FRAME_BEFORE_IP:
		{
			src.TypeTextAtCursor("{END}{ENTER}");
			break;
		}
		case EC_CURRENT_FRAME_AT_IP:
		{
			src.TypeTextAtCursor("{TAB}");
			break;
		}
		case EC_PARENT_FRAME_AFTER_IP:
		{
			src.TypeTextAtCursor("{HOME}{ENTER}{UP}{TAB}");
			break;
		}
	}

	// insert the type and name of the local.
	src.TypeTextAtCursor(Local.m_strType + " " + Local.m_strName);
	
	// append a value assignment if applicable.
	if(Local.m_strValue != "")
		src.TypeTextAtCursor(" = " + Local.m_strValue);
	
	// complete the statement and append a comment stating the edit type for later use.
	src.TypeTextAtCursor((CString)";{TAB}// " + szecEditTypes[Local.m_ecEditType]);

	// check if there are extra code lines to add.
	if(Local.m_strCodeLines.GetSize() > 0)
			
	{
		// add all the extra code lines.
		for(int iCodeLine = 0; iCodeLine < Local.m_strCodeLines.GetSize(); iCodeLine++)

		{
			src.TypeTextAtCursor("{ENTER}");

			// editing at the current ip requires a Tab before the first extra code line to align the
			// extra code lines with the initial line.
			if((Local.m_ecEditType == EC_CURRENT_FRAME_AT_IP) && (iCodeLine == 0))
				src.TypeTextAtCursor("{TAB}");
			
			src.TypeTextAtCursor(Local.m_strCodeLines[iCodeLine]);
		}
	}

	// editing at the current ip requires an Enter here to separate it from the next line.
	if(Local.m_ecEditType == EC_CURRENT_FRAME_AT_IP)
		src.TypeTextAtCursor("{ENTER}");
}


//******************************* VALUE TESTS ********************************

BOOL CECLocalsCases::DoTest(CValueTest &ValueTest)

{
	// randomly select an index for a function in which to perform the test.
	// the function must already have the local to change in it.
	int iLocalToChange;
	int iFunc = RandomlySelectFunctionToEdit(ValueTest, &iLocalToChange);

	// if a suitable function wasn't found, abort. the test may be tried again later.
	if(iFunc < 0)
		return FALSE;

	// this test will be performed.
	giTestsPerformed++;
	// get the CFunc object for the index chosen.
	CFunc &Func = Funcs[iFunc];
	// get the name of the local we are changing.
	ValueTest.m_Local.m_strName = Func.m_Locals[iLocalToChange].m_strName;

	// log the test header.
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("TEST %d: Changing value of local \"%s\" in function \"%s\" to value \"%s\" using edit type \"%s\".", 
					   giTestsPerformed, ValueTest.m_Local.m_strName, Func.m_strName, 
					   ValueTest.m_Local.m_strValue, szecEditTypes[ValueTest.m_Local.m_ecEditType]);
	
	// each type of edit requires different steps.
	switch(ValueTest.m_Local.m_ecEditType)
	
	{
		// edited function does not have an active stack frame.
		case EC_NO_STACK_FRAME: 
		{
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			// find the local assignment to change within the function.
			EXPECT_TRUE(src.Find(ValueTest.m_Local.m_strType + " " + ValueTest.m_Local.m_strName));
			// replace the assignment to one using the new value.
			ReplaceLocalAssignmentInSource(ValueTest.m_Local);
			// TODO(michma): step to cursor fails sometimes, so apply code changes separately first.
			ApplyCodeChanges();
			break;
		}

		// edited function is active on top of the stack. edit occurs after the ip.
		case EC_CURRENT_FRAME_AFTER_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// run to the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// find the local assignment to change within the function.
			EXPECT_TRUE(src.Find(ValueTest.m_Local.m_strType + " " + ValueTest.m_Local.m_strName));
			// replace the assignment to one using the new value.
			ReplaceLocalAssignmentInSource(ValueTest.m_Local);
			// TODO(michma): step to cursor fails sometimes, so apply code changes separately first.
			ApplyCodeChanges();
			break;
		}

		// edited function is active on top of the stack. edit occurs before the ip.
		case EC_CURRENT_FRAME_BEFORE_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// run to the last line of the function.
			EXPECT_TRUE(src.Find("last line of " + Func.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// record the parent function for later comparison.
			CString strParentFunc = stk.GetFunction(1, TRUE);
			// find the local assignment to change within the function.
			EXPECT_TRUE(src.Find(ValueTest.m_Local.m_strType + " " + ValueTest.m_Local.m_strName, FALSE, FALSE, FALSE, FIND_UP));
			// replace the assignment to one using the new value.
			ReplaceLocalAssignmentInSource(ValueTest.m_Local);
			// step out of the function and verify debugger lands in the right spot.
			// TODO(michma): sometimes get "execution point changed..." message doing step out.
			// TODO(michma): step out fails, so apply go changes first then step out.
			ApplyCodeChanges();	
			EXPECT_TRUE(dbg.StepOut());
			// verify the debugger is at the correct line and function.
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to " + Func.m_strName + "()"));
			EXPECT_TRUE(stk.CurrentFunctionIs(strParentFunc));
			// set the next statement to the call into the edited function.
			EXPECT_TRUE(src.Find(Func.m_strName + "();", FALSE, FALSE, FALSE, FIND_UP));
			EXPECT_TRUE(dbg.SetNextStatement());
			break;
		}

		// edited function is active on top of the stack. edit occurs at the ip.
		case EC_CURRENT_FRAME_AT_IP:
		
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			// find the local assignment to change within the function.
			EXPECT_TRUE(src.Find(ValueTest.m_Local.m_strType + " " + ValueTest.m_Local.m_strName));
			// run to the local assignment.
			EXPECT_TRUE(dbg.StepToCursor());
			// replace the assignment to one using the new value.
			ReplaceLocalAssignmentInSource(ValueTest.m_Local);
			// TODO(michma): step to cursor fails sometimes, so apply code changes separately first.
			ApplyCodeChanges();
		
			// if we are removing the assignment from a local, then the point of execution will be moved because
			// there will no longer be code at that line, and we'll get a message about that.
			if(ValueTest.m_Local.m_strValue == "")
				EXPECT_TRUE(dbg.HandleMsg(MSG_EC_POINT_OF_EXECUTION_MOVED));

			break;
		}

		// edited function is active, but not on top of the stack. edit occurs after the ip.
		case EC_PARENT_FRAME_AFTER_IP:
		{
			// get the child of the function to edit.
			CFunc &FuncChild = Funcs[GetFunctionIndexFromName(Func.m_strChild)];
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the child of the function to edit.
			EXPECT_SUCCESS(src.Open(FuncChild.m_strFile));
			// run to the first line of the child of the function to edit.
			EXPECT_TRUE(src.Find("first line of " + FuncChild.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			// find the local assignment to change within the function.
			EXPECT_TRUE(src.Find(ValueTest.m_Local.m_strType + " " + ValueTest.m_Local.m_strName));
			// replace the assignment to one using the new value.
			ReplaceLocalAssignmentInSource(ValueTest.m_Local);
			// step out of the child function.
			EXPECT_TRUE(dbg.StepOut());
			// verify we are back in the edited function.
			EXPECT_TRUE(stk.CurrentFunctionIs(Func.m_strName));
			break;
		}

		// edited function is active, but not on top of the stack. edit occurs before the ip.
		case EC_PARENT_FRAME_BEFORE_IP:
		{
			// get the child of the function to edit.
			CFunc &FuncChild = Funcs[GetFunctionIndexFromName(Func.m_strChild)];
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the child of the function to edit.
			EXPECT_SUCCESS(src.Open(FuncChild.m_strFile));
			// run to the first line of the child of the function to edit.
			EXPECT_TRUE(src.Find("first line of " + FuncChild.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			// find the local assignment to change within the function.
			EXPECT_TRUE(src.Find(ValueTest.m_Local.m_strType + " " + ValueTest.m_Local.m_strName));
			// replace the assignment to one using the new value.
			ReplaceLocalAssignmentInSource(ValueTest.m_Local);
			// step out of the child function and verify the debugger landed in correct spot. 
			EXPECT_TRUE(dbg.StepOut());
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to " + FuncChild.m_strName + "()"));
			EXPECT_TRUE(stk.CurrentFunctionIs(Func.m_strName));
			// record the parent function for later comparison.
			CString strParentFunc = stk.GetFunction(1, TRUE);
			// step out of the edited function and verify the debugger landed in the correct spot.
			EXPECT_TRUE(dbg.StepOut());
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to " + Func.m_strName + "()"));
			EXPECT_TRUE(stk.CurrentFunctionIs(strParentFunc));
			// set the next statement back to the call into the edited function.
			EXPECT_TRUE(src.Find(Func.m_strName + "();", FALSE, FALSE, FALSE, FIND_UP));
			EXPECT_TRUE(dbg.SetNextStatement());
			break;
		}
	}

	// run to the last line of the function to verify locals.
	// we need to call this function because in the case of EC_CURRENT_FRAME_AT_IP we may already be at the last line.
	EXPECT_TRUE(RunToLastLineOfFunc(Func.m_strName, ValueTest.m_Local.m_ecEditType));
	// verify the debugger stopped at the correct line and function.
	EXPECT_TRUE(dbg.CurrentLineIs("last line of " + Func.m_strName + "()"));
	EXPECT_TRUE(stk.CurrentFunctionIs(Func.m_strName));
	// record the value change in the local's entry within the function.
	Func.m_Locals[iLocalToChange].m_strValue = ValueTest.m_Local.m_strValue;
	Func.m_Locals[iLocalToChange].m_strVerifyValue = ValueTest.m_Local.m_strVerifyValue;
	// verify the locals in the function.
	VerifyFunctionLocals(Func);
	// the test was able to execute.
	ValueTest.m_bTestDone = TRUE;
	return TRUE;
}


int CECLocalsCases::RandomlySelectFunctionToEdit(CValueTest &ValueTest, int *piLocalToChange)

{
	// we only want to randomly check each function for suitability once.
	RandomNumberCheckList rnclFuncs(Funcs.GetSize());
	// the random number generated to index into the Funcs array.
	int iFunc;
		
	// randomly check each function for suitability once.
	while((iFunc = rnclFuncs.GetNext()) != -1)
	{
		// make sure an appropriate local already exists in this function.
		for(int iLocal = 0; iLocal < Funcs[iFunc].m_Locals.GetSize(); iLocal++)
		{
			// if the local meets all the criteria, return the function index and local index to change.
			if((Funcs[iFunc].m_Locals[iLocal].m_strType == ValueTest.m_Local.m_strType) &&
			   (Funcs[iFunc].m_Locals[iLocal].m_strValue != ValueTest.m_Local.m_strValue) &&
			   (Funcs[iFunc].m_Locals[iLocal].m_ecEditType == ValueTest.m_Local.m_ecEditType) &&
			   (!((Funcs[iFunc].m_Locals[iLocal].m_strValue == "") && !ValueTest.m_bAddAssignment)) &&
			   (!((Funcs[iFunc].m_Locals[iLocal].m_strValue != "") && ValueTest.m_bAddAssignment)))
			{	
				*piLocalToChange = iLocal;
				return iFunc;
			}
		}
	}

	// couldn't find a suitable function.
	return -1;
}


void CECLocalsCases::ReplaceLocalAssignmentInSource(CECLocal &Local)

{
	// select the local assignment to be replaced.
	src.TypeTextAtCursor("{HOME}+({END})");
	// insert the type and name of the local.
	src.TypeTextAtCursor(Local.m_strType + " " + Local.m_strName);
	
	// append a value assignment if there is one.
	if(Local.m_strValue != "")
		src.TypeTextAtCursor(" = " + Local.m_strValue);

	// finish the line, including a comment stating the edit type for later use.
	src.TypeTextAtCursor((CString)";{TAB}// " + szecEditTypes[Local.m_ecEditType]);
}


//******************************* TYPE TESTS ********************************

BOOL CECLocalsCases::DoTest(CTypeTest &TypeTest)

{
	// randomly select an index for a function in which to perform the test.
	// the function must already have the local to change in it.
	int iLocalToChange;
	int iFunc = RandomlySelectFunctionToEdit(TypeTest, &iLocalToChange);

	// if a suitable function wasn't found, abort. the test may be tried again later.
	if(iFunc < 0)
		return FALSE;

	// this test will be performed.
	giTestsPerformed++;
	// get the Func object for the index chosen.
	CFunc &Func = Funcs[iFunc];
	// get the name of the local we are changing.
	TypeTest.m_Local.m_strName = Func.m_Locals[iLocalToChange].m_strName;
	// get the value of the local we are changing.
	TypeTest.m_Local.m_strValue = Func.m_Locals[iLocalToChange].m_strValue;

	// log the test header.
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("TEST %d: Changing type of local \"%s\" in function \"%s\" to type \"%s\" using edit type \"%s\".", 
					   giTestsPerformed, TypeTest.m_Local.m_strName, Func.m_strName, 
					   TypeTest.m_strNewType, szecEditTypes[TypeTest.m_Local.m_ecEditType]);
	
	// each type of edit requires different steps.
	switch(TypeTest.m_Local.m_ecEditType)
	
	{
		// edited function does not have an active stack frame.
		case EC_NO_STACK_FRAME: 
		{
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			// find the local to change within the function.
			EXPECT_TRUE(src.Find(TypeTest.m_Local.m_strType + " " + TypeTest.m_Local.m_strName));
			// replace the type to one using the new type.
			ReplaceLocalTypeInSource(TypeTest);
			// TODO(michma): step to cursor fails sometimes, so apply code changes separately first.
			ApplyCodeChanges();
			break;
		}

		// edited function is active on top of the stack. edit occurs after the ip.
		case EC_CURRENT_FRAME_AFTER_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// run to the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// find the local to change within the function.
			EXPECT_TRUE(src.Find(TypeTest.m_Local.m_strType + " " + TypeTest.m_Local.m_strName));
			// replace the type to one using the new type.
			ReplaceLocalTypeInSource(TypeTest);
			// TODO(michma): step to cursor fails sometimes, so apply code changes separately first.
			ApplyCodeChanges();
			break;
		}

		// edited function is active on top of the stack. edit occurs before the ip.
		case EC_CURRENT_FRAME_BEFORE_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// run to the last line of the function.
			EXPECT_TRUE(src.Find("last line of " + Func.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// record the parent function for later comparison.
			CString strParentFunc = stk.GetFunction(1, TRUE);
			// find the local to change within the function.
			EXPECT_TRUE(src.Find(TypeTest.m_Local.m_strType + " " + TypeTest.m_Local.m_strName, FALSE, FALSE, FALSE, FIND_UP));
			// replace the type to one using the new type.
			ReplaceLocalTypeInSource(TypeTest);
			// step out of the function and verify debugger lands in the right spot.
			// TODO(michma): sometimes get "execution point changed..." message doing step out.
			// TODO(michma): step out fails, so apply go changes first then step out.
			ApplyCodeChanges();	
			EXPECT_TRUE(dbg.StepOut());
			// verify the debugger is at the correct line and function.
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to " + Func.m_strName + "()"));
			EXPECT_TRUE(stk.CurrentFunctionIs(strParentFunc));
			// set the next statement to the call into the edited function.
			EXPECT_TRUE(src.Find(Func.m_strName + "();", FALSE, FALSE, FALSE, FIND_UP));
			EXPECT_TRUE(dbg.SetNextStatement());
			break;
		}

		// edited function is active on top of the stack. edit occurs at the ip.
		case EC_CURRENT_FRAME_AT_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			// find the local to change within the function.
			EXPECT_TRUE(src.Find(TypeTest.m_Local.m_strType + " " + TypeTest.m_Local.m_strName));
			// run to the local assignment.
			EXPECT_TRUE(dbg.StepToCursor());
			// replace the type to one using the new type.
			ReplaceLocalTypeInSource(TypeTest);
			// TODO(michma): step to cursor fails sometimes, so apply code changes separately first.
			ApplyCodeChanges();
			break;
		}

		// edited function is active, but not on top of the stack. edit occurs after the ip.
		case EC_PARENT_FRAME_AFTER_IP:
		{
			// get the child of the function to edit.
			CFunc &FuncChild = Funcs[GetFunctionIndexFromName(Func.m_strChild)];
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the child of the function to edit.
			EXPECT_SUCCESS(src.Open(FuncChild.m_strFile));
			// run to the first line of the child of the function to edit.
			EXPECT_TRUE(src.Find("first line of " + FuncChild.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			// find the local to change within the function.
			EXPECT_TRUE(src.Find(TypeTest.m_Local.m_strType + " " + TypeTest.m_Local.m_strName));
			// replace the type to one using the new type.
			ReplaceLocalTypeInSource(TypeTest);
			// step out of the child function.
			EXPECT_TRUE(dbg.StepOut());
			// verify we are back in the edited function.
			EXPECT_TRUE(stk.CurrentFunctionIs(Func.m_strName));
			break;
		}

		// edited function is active, but not on top of the stack. edit occurs before the ip.
		case EC_PARENT_FRAME_BEFORE_IP:
		{
			// get the child of the function to edit.
			CFunc &FuncChild = Funcs[GetFunctionIndexFromName(Func.m_strChild)];
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the child of the function to edit.
			EXPECT_SUCCESS(src.Open(FuncChild.m_strFile));
			// run to the first line of the child of the function to edit.
			EXPECT_TRUE(src.Find("first line of " + FuncChild.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			// find the local to change within the function.
			EXPECT_TRUE(src.Find(TypeTest.m_Local.m_strType + " " + TypeTest.m_Local.m_strName));
			// replace the type to one using the new type.
			ReplaceLocalTypeInSource(TypeTest);
			// step out of the child function and verify the debugger landed in correct spot. 
			EXPECT_TRUE(dbg.StepOut());
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to " + FuncChild.m_strName + "()"));
			EXPECT_TRUE(stk.CurrentFunctionIs(Func.m_strName));
			// record the parent function for later comparison.
			CString strParentFunc = stk.GetFunction(1, TRUE);
			// step out of the edited function and verify the debugger landed in the correct spot.
			EXPECT_TRUE(dbg.StepOut());
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to " + Func.m_strName + "()"));
			EXPECT_TRUE(stk.CurrentFunctionIs(strParentFunc));
			// set the next statement back to the call into the edited function.
			EXPECT_TRUE(src.Find(Func.m_strName + "();", FALSE, FALSE, FALSE, FIND_UP));
			EXPECT_TRUE(dbg.SetNextStatement());
			break;
		}
	}

	// run to the last line of the function to verify locals.
	// we need to call this function because in the case of EC_CURRENT_FRAME_AT_IP we may already be at the last line.
	EXPECT_TRUE(RunToLastLineOfFunc(Func.m_strName, TypeTest.m_Local.m_ecEditType));
	// verify the debugger stopped at the correct line and function.
	EXPECT_TRUE(dbg.CurrentLineIs("last line of " + Func.m_strName + "()"));
	EXPECT_TRUE(stk.CurrentFunctionIs(Func.m_strName));
	// record the type and value info in the local's entry within the function.
	Func.m_Locals[iLocalToChange].m_strType = TypeTest.m_strNewType;
	Func.m_Locals[iLocalToChange].m_strVerifyType = TypeTest.m_Local.m_strVerifyType;
	Func.m_Locals[iLocalToChange].m_strVerifyValue = TypeTest.m_Local.m_strVerifyValue;
	// verify the information for all the locals in the function.
	VerifyFunctionLocals(Func);
	// the test was able to execute.
	TypeTest.m_bTestDone = TRUE;
	return TRUE;
}


int CECLocalsCases::RandomlySelectFunctionToEdit(CTypeTest &TypeTest, int *piLocalToChange)

{
	// we only want to randomly check each function for suitability once.
	RandomNumberCheckList rnclFuncs(Funcs.GetSize());
	// the random number generated to index into the Funcs array.
	int iFunc;
		
	// randomly check each function for suitability once.
	while((iFunc = rnclFuncs.GetNext()) != -1)
	{
		// make sure an appropriate local already exists in this function.
		for(int iLocal = 0; iLocal < Funcs[iFunc].m_Locals.GetSize(); iLocal++)
		{
			// if the local meets all the criteria, return the function index and local index to change.
			if((Funcs[iFunc].m_Locals[iLocal].m_strType == TypeTest.m_Local.m_strType) &&
			   (Funcs[iFunc].m_Locals[iLocal].m_ecEditType == TypeTest.m_Local.m_ecEditType))
			{	
			 	*piLocalToChange = iLocal;
				return iFunc;
			}
		}
	}

	// couldn't find a suitable function.
	return -1;
}


void CECLocalsCases::ReplaceLocalTypeInSource(CTypeTest &TypeTest)

{
	// select the local assignment to be replaced.
	src.TypeTextAtCursor("{HOME}+({END})");
	// insert the type and name of the local.
	src.TypeTextAtCursor(TypeTest.m_strNewType + " " + TypeTest.m_Local.m_strName);
	
	// append a value assignment if there is one.
	if(TypeTest.m_Local.m_strValue != "")
		src.TypeTextAtCursor(" = " + TypeTest.m_Local.m_strValue);

	// finish the line, including a comment stating the edit type for later use.
	src.TypeTextAtCursor((CString)";{TAB}// " + szecEditTypes[TypeTest.m_Local.m_ecEditType]);
}


//********************************** REMOVE TESTS *********************************

BOOL CECLocalsCases::DoTest(CRemoveTest &RemoveTest)

{
	// we don't want to remove any locals that may later be needed for value tests.
	// therefore we don't perform the remove test if there are any remaining value tests that have the same edit type.
	for(int iTest = 0; iTest < ValueTests.GetSize(); iTest++)
	{
		if((ValueTests[iTest].m_Local.m_ecEditType == RemoveTest.m_ecEditType) && !ValueTests[iTest].m_bTestDone)
			return FALSE;
	}
	
	// randomly select an index for a function in which to perform the test.
	// the function must already have at least one local of the matching edit type in it.
	int iFunc = RandomlySelectFunctionToEdit(RemoveTest);

	// if a suitable function wasn't found, abort. the test may be tried again later.
	if(iFunc < 0)
		return FALSE;

	// this test will be performed.
	giTestsPerformed++;
	// get the Func object for the index chosen.
	CFunc &Func = Funcs[iFunc];

	// log the test header.
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("TEST %d: Removing locals of edit type \"%s\" from function \"%s\". Leave references == %s.",
					   giTestsPerformed, szecEditTypes[RemoveTest.m_ecEditType], Func.m_strName, 
					   RemoveTest.m_bLeaveReferences ? "TRUE" : "FALSE");
	
	// each type of edit requires different steps.
	switch(RemoveTest.m_ecEditType)
	
	{
		// edited function does not have an active stack frame.
		case EC_NO_STACK_FRAME: 
		{
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			// remove the locals of the matching edit type.
			RemoveLocalsFromSource(Func, RemoveTest);
			// TODO(michma): step to cursor fails sometimes, so apply code changes separately first.
			ApplyCodeChanges();
			break;
		}

		// edited function is active on top of the stack. edit occurs after the ip.
		case EC_CURRENT_FRAME_AFTER_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// run to the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// remove the locals of the matching edit type.
			RemoveLocalsFromSource(Func, RemoveTest);
			// TODO(michma): step to cursor fails sometimes, so apply code changes separately first.
			ApplyCodeChanges();
			break;
		}

		// edited function is active on top of the stack. edit occurs before the ip.
		case EC_CURRENT_FRAME_BEFORE_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// run to the last line of the function.
			EXPECT_TRUE(src.Find("last line of " + Func.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// record the parent function for later comparison.
			CString strParentFunc = stk.GetFunction(1, TRUE);
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()", FALSE, FALSE, FALSE, FIND_UP));
			// remove the locals of the matching edit type.
			RemoveLocalsFromSource(Func, RemoveTest);
			// step out of the function and set the next statement back to the call into the function.
			// TODO(michma): sometimes get "execution point changed..." message doing step out.
			// TODO(michma): step out fails, so apply go changes first then step out.
			ApplyCodeChanges();	
			EXPECT_TRUE(dbg.StepOut());
			EXPECT_TRUE(src.Find(Func.m_strName + "();", FALSE, FALSE, FALSE, FIND_UP));
			EXPECT_TRUE(dbg.SetNextStatement());
			// verify the debugger is at the correct line and function.
			EXPECT_TRUE(dbg.CurrentLineIs(Func.m_strName + "();"));
			EXPECT_TRUE(stk.CurrentFunctionIs(strParentFunc));
			break;
		}

		// edited function is active on top of the stack. edit occurs at the ip.
		case EC_CURRENT_FRAME_AT_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			// find the declaration of the first local with the matching edit type.
			EXPECT_TRUE(src.Find("EC_CURRENT_FRAME_AT_IP"));
			// run to the first local declaration to remove.
			EXPECT_TRUE(dbg.StepToCursor());
			// remove the locals of the matching edit type.
			RemoveLocalsFromSource(Func, RemoveTest);
			// TODO(michma): step to cursor fails sometimes, so apply code changes separately first.
			ApplyCodeChanges();
			break;
		}

		// edited function is active, but not on top of the stack. edit occurs after the ip.
		case EC_PARENT_FRAME_AFTER_IP:
		{
			// get the child of the function to edit.
			CFunc &FuncChild = Funcs[GetFunctionIndexFromName(Func.m_strChild)];
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the child of the function to edit.
			EXPECT_SUCCESS(src.Open(FuncChild.m_strFile));
			// run to the first line of the child of the function to edit.
			EXPECT_TRUE(src.Find("first line of " + FuncChild.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			// remove the locals of the matching edit type.
			RemoveLocalsFromSource(Func, RemoveTest);
			// step out of the child function.
			EXPECT_TRUE(dbg.StepOut());
			// verify we are back in the edited function.
			EXPECT_TRUE(stk.CurrentFunctionIs(Func.m_strName));
			break;
		}

		// edited function is active, but not on top of the stack. edit occurs before the ip.
		case EC_PARENT_FRAME_BEFORE_IP:
		
		{
			// get the child of the function to edit.
			CFunc &FuncChild = Funcs[GetFunctionIndexFromName(Func.m_strChild)];
			// TODO(michma): randomly select either step to cursor or bp.
			// open the file containing the child of the function to edit.
			EXPECT_SUCCESS(src.Open(FuncChild.m_strFile));
			// run to the first line of the child of the function to edit.
			EXPECT_TRUE(src.Find("first line of " + FuncChild.m_strName + "()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// open the file containing the function to edit.
			EXPECT_SUCCESS(src.Open(Func.m_strFile));
			// find the first line of the function.
			EXPECT_TRUE(src.Find("first line of " + Func.m_strName + "()"));
			// remove the locals of the matching edit type.
			RemoveLocalsFromSource(Func, RemoveTest);
			
			// if we are removing any locals that have ctors/dtors, then we have to handle a special debugger message.
			BOOL bLocalUsingCtorDtorFound = FALSE;

			// loop through all the locals in the function, checking for one that uses a ctor/dtor.
			for(int iLocal = 0; iLocal < Func.m_Locals.GetSize(); iLocal++)
			{
				if(Func.m_Locals[iLocal].m_bUsesCtorDtor)
				{
					bLocalUsingCtorDtorFound = TRUE;
					break;
				}
			}

			if(bLocalUsingCtorDtorFound)
			{
				// step out of the child function (handle message about a local being removed).
				// we will end up in asm mode.
				EXPECT_TRUE(dbg.StepOut(1, NULL, NULL, NULL, NOWAIT));
				EXPECT_TRUE(dbg.HandleMsg(MSG_EC_LOCAL_VAR_REMOVED));
				EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
				// record the parent function for later comparison.
				CString strParentFunc = stk.GetFunction(1, TRUE);
				// step out of the edited function and reset source mode.
				EXPECT_TRUE(dbg.StepOut());
				EXPECT_TRUE(dbg.SetSteppingMode(SRC));
				// set the next statement back to the call into the edited function.
				EXPECT_TRUE(src.Find(Func.m_strName + "();", FALSE, FALSE, FALSE, FIND_UP));
				EXPECT_TRUE(dbg.SetNextStatement());
				// verify the debugger is at the correct line and function.
				EXPECT_TRUE(dbg.CurrentLineIs(Func.m_strName + "();"));
				EXPECT_TRUE(stk.CurrentFunctionIs(strParentFunc));	
			}
			else
				// if no locals using ctors/dtors were found, then we can step out normally.
				dbg.StepOut();

			break;
		}
	}

	// run to the last line of the function to verify locals.
	// we need to call this function because in the case of EC_CURRENT_FRAME_AT_IP we may already be at the last line.
	EXPECT_TRUE(RunToLastLineOfFunc(Func.m_strName, RemoveTest.m_ecEditType));
	// verify the debugger stopped at the correct line and function.
	EXPECT_TRUE(dbg.CurrentLineIs("last line of " + Func.m_strName + "()"));
	EXPECT_TRUE(stk.CurrentFunctionIs(Func.m_strName));

	// loop through all the locals in the function.
	for(int iLocal = 0; iLocal < Func.m_Locals.GetSize(); iLocal++)
	{
		// if the local's edit type matches, remove it (for later comparison).
		if(Func.m_Locals[iLocal].m_ecEditType == RemoveTest.m_ecEditType)
			Func.m_Locals.RemoveAt(iLocal);
	}

	// verify the locals in the function.
	VerifyFunctionLocals(Func);
	// the test was able to execute.
	RemoveTest.m_bTestDone = TRUE;
	return TRUE;
}


int CECLocalsCases::RandomlySelectFunctionToEdit(CRemoveTest &RemoveTest)

{
	// we only want to randomly check each function for suitability once.
	RandomNumberCheckList rnclFuncs(Funcs.GetSize());
	// the random number generated to index into the Funcs array.
	int iFunc;
		
	// randomly check each function for suitability once.
	while((iFunc = rnclFuncs.GetNext()) != -1)
	
	{
		// make sure at least one appropriate local already exists in this function.
		for(int iLocal = 0; iLocal < Funcs[iFunc].m_Locals.GetSize(); iLocal++)
		{
			// if the local meets the criteria, return the function index.
			if(Funcs[iFunc].m_Locals[iLocal].m_ecEditType == RemoveTest.m_ecEditType)
				return iFunc;
		}
	}

	// couldn't find a suitable function.
	return -1;
}


void CECLocalsCases::RemoveLocalsFromSource(CFunc &Func, CRemoveTest &RemoveTest)

{
	// count of locals in the function that have a matching edit type.
	int iLocalCount = 0;

	// loop through all the locals in the function.
	for(int iLocal = 0; iLocal < Func.m_Locals.GetSize(); iLocal++)
	{
		// if the local's edit type matches, count it.
		if(Func.m_Locals[iLocal].m_ecEditType == RemoveTest.m_ecEditType)
			iLocalCount++;	
	}	
			
	// remove each local that has a matching edit type.
	for(int i = 0; i < iLocalCount; i++)

	{
		// find the declaration of the next local with the matching edit type.
		EXPECT_TRUE(src.Find(szecEditTypes[RemoveTest.m_ecEditType]));
		// grab the line for examination.
		src.SelectLines();
		CString strLine = src.GetSelectedText();
		// figure out which local the line declares.
		int iLocal = FindLocalInLine(strLine, Func);
		// delete the local declaration line.
		src.TypeTextAtCursor("{DEL}");

		// remove the references to the local if applicable (i.e. the e&c is supposed to succeed).
		if(!RemoveTest.m_bLeaveReferences)
			RemoveLocalReferencesFromSource(Func.m_Locals[iLocal]);
	}
}


int CECLocalsCases::FindLocalInLine(CString &strLine, CFunc &Func)

{
	// loop through all the locals in the function.
	for(int iLocal = 0; iLocal < Func.m_Locals.GetSize(); iLocal++)
	{
		// if the name of the local is found within the line, return its index.
		if(strLine.Find(Func.m_Locals[iLocal].m_strName) != -1)
			return iLocal;
	}

	// should never get here. we've already verified that this line declares a local in this function.
	ASSERT(0);
	return -1;
}


void CECLocalsCases::RemoveLocalReferencesFromSource(CECLocal &Local)
{
	// loop through all the code lines that reference the local.
	for(int iCodeLines = 0; iCodeLines < Local.m_strCodeLines.GetSize(); iCodeLines++)
	{
		// find the next code line that references the local.
		EXPECT_TRUE(src.Find(Local.m_strCodeLines[iCodeLines]));
		// select the line and delete it.
		src.SelectLines();
		src.TypeTextAtCursor("{DEL}");
	}
}


//******************************* MISC FUNCTIONS *******************************

BOOL CECLocalsCases::GetBackToStartOfWinMain(void)

{
	/*
	if(!stk.CurrentFunctionIs("WinMain"))
		EXPECT_TRUE(stk.RunToFrame("WinMain"));
	
	EXPECT_TRUE(src.Find("first line of WinMain()"));
	EXPECT_TRUE(dbg.SetNextStatement());

	if(dbg.AtSymbol("WinMain"))
		return TRUE;
	else
		return FALSE;
	*/
	
	dbg.StopDebugging();
	proj.WaitUntilBuildDone(1);
	proj.RebuildAll();
	dbg.StepInto();
	// TODO(michma - 3/23/98): can't restart after adding float local and doing e&c (bug 37170).
	//dbg.Restart();
	cxx.GetExpressionValue("&intGlobal", strGlobalIntAddress);

	for(int iFunc = 0; iFunc < Funcs.GetSize(); iFunc++)
	{
		for(int iLocal = 0; iLocal < Funcs[iFunc].m_Locals.GetSize(); iLocal++)
		{
			if(Funcs[iFunc].m_Locals[iLocal].m_strType == "int *")
				Funcs[iFunc].m_Locals[iLocal].m_strVerifyValue = strGlobalIntAddress;
		}
	}

	for(int iTest = 0; iTest < AddTests.GetSize(); iTest++)
	{
		if(AddTests[iTest].m_Local.m_strType == "int *")
			AddTests[iTest].m_Local.m_strVerifyValue = strGlobalIntAddress;
	}

	return TRUE;
}


void CECLocalsCases::VerifyFunctionLocals(CFunc &Func)

{
	CString strTmpValue;	
	CString strTmpType;	

	// verify all the expected locals in the function.
	for(int iLocal = 0; iLocal < Func.m_Locals.GetSize(); iLocal++)
	
	{
		// log info about the local being verified.
		m_pLog->RecordInfo("verifying local \"%s\" is in function \"%s\".",
			Func.m_Locals[iLocal].m_strName, Func.m_strName);	
	
		// if the type to verify is different than the type assigned, then load it into the appropriate
		// field as required by COLocals::VerifyLocalInfo. the type assigned is preserved in a temp variable.
		if(Func.m_Locals[iLocal].m_strVerifyType != "")
		{
			strTmpType = Func.m_Locals[iLocal].m_strType;	
			Func.m_Locals[iLocal].m_strType = Func.m_Locals[iLocal].m_strVerifyType;
		}

		// if the value to verify is different than the value assigned, then load it into the appropriate
		// field as required by COLocals::VerifyLocalInfo. the value assigned is preserved in a temp variable.
		if(Func.m_Locals[iLocal].m_strVerifyValue != "")
		{
			strTmpValue = Func.m_Locals[iLocal].m_strValue;	
			Func.m_Locals[iLocal].m_strValue = Func.m_Locals[iLocal].m_strVerifyValue;
		}
			
		// verify the locals info.
		EXPECT_TRUE(locals.VerifyLocalInfo(Func.m_Locals[iLocal]));

		// if the type to verify was different than the type assigned, then it was loaded into the appropriate
		// field as required by COLocals::VerifyLocalInfo. restore the type assigned as preserved in the temp variable.
		if(Func.m_Locals[iLocal].m_strVerifyType != "")
			Func.m_Locals[iLocal].m_strType = strTmpType;	

		// if the value to verify was different than the value assigned, then it was loaded into the appropriate
		// field as required by COLocals::VerifyLocalInfo. restore the value assigned as preserved in the temp variable.
		if(Func.m_Locals[iLocal].m_strVerifyValue != "")
			Func.m_Locals[iLocal].m_strValue = strTmpValue;	
	}
}


int CECLocalsCases::GetFunctionIndexFromName(LPCSTR szName)

{
	// loop through all the functions checking for the name.
	for(int iFunc = 0; iFunc < Funcs.GetSize(); iFunc++)
	{
		// if the names match, we've found the index.
		if(Funcs[iFunc].m_strName == szName)
			return iFunc;
	}

	// function name was not found in the list (should never get here).
	assert(0);
	return -1;
}


// TODO(michma): this needs its own CODebug function.
void CECLocalsCases::ApplyCodeChanges(void)
{
	MST.DoKeys("%dy");
	proj.WaitUntilBuildDone();
}


// for the EC_CURRENT_FRAME_AT_IP case, sometimes we may already be at the
// current line, so we need to check first before trying to run to it. this function encapsulates that.
BOOL CECLocalsCases::RunToLastLineOfFunc(LPCSTR szFunc, EC_EDIT_TYPE ecEditType)

{
	if((ecEditType != EC_CURRENT_FRAME_AT_IP) || !dbg.CurrentLineIs((CString)"last line of " + szFunc + "()"))
	{
		if(!src.Find((CString)"last line of " + szFunc + "()"))
			return FALSE;
		if(!dbg.StepToCursor())
			return FALSE;
	}

	return TRUE;
}