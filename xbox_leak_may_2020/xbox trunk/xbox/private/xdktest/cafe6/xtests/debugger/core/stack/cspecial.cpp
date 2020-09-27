///////////////////////////////////////////////////////////////////////////////
//	SK1CASES.CPP
//
//	Created by :			Date :
//		YefimS			01/06/94
//
//	Description :
//		Implementation of the Cstack2IDETest class
//

#include "stdafx.h"
#include "stacsub.h"
#include "cspecial.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(Cstack_specialIDETest, CDbgTestBase, "CallStack Special", -1, CstackSubSuite)

void Cstack_specialIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void Cstack_specialIDETest::Run()
{	 
	XSAFETY;
	if( SetProject("stack01\\stack01", PROJECT_XBE) )
	{	  
		XSAFETY;
		MainSequence();
		XSAFETY;
		Recursion();
		XSAFETY;
//		Options();
//		XSAFETY;
		AdvancedParam();
	}
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL Cstack_specialIDETest::MainSequence(   )
{
	EXPECT( dbg.Restart() );
	EXPECT( UIWB.VerifySubstringAtLine( "{" ) );		
	// Verify main and __crt are in stack
	CString strFunc = "main() line 57";
	LOG->RecordCompare( stk.CurrentFunctionIs(strFunc) == TRUE,
		"Top of stack on restart expected value: %s ", strFunc);
	//xbox strFunc = "mainCRTStartup";
	strFunc = "mainXapiStartup";
	stk.NavigateStack(1);
	Sleep(1500); // emmang@xbox
	LOG->RecordCompare( stk.FunctionIs(strFunc, 1) == TRUE,
		"CRT startup stack on restart expected value: %s ", strFunc);

	CheckParamList();
	VarParam();

	return (TRUE);
}

//I have no idea what I wanted to to here: all C++ cases are included in AdvancedParam()
BOOL Cstack_specialIDETest::CheckParamList(   )
{
	return (TRUE);
}

BOOL Cstack_specialIDETest::VarParam(   )
{
	bps.SetBreakpoint("average");
	dbg.Go("average");
	EXPECT_TRUE(stk.CurrentFunctionIs("average(int 1) line ") );
	bps.ClearAllBreakpoints();
	return (TRUE);
}

const int nFirstFactorialLine = 311;
//char szLastLn==nFirstFactorialLine+5;
//char szRetLn==nFirstFactorialLine+4;

BOOL Cstack_specialIDETest::Recursion(   )
{
 	LogTestHeader( "Recursion" );

	CString sLastLn;sLastLn.Format("%d",nFirstFactorialLine+5); //last line of factorial function
	CString sRetLn;sRetLn.Format("%d",nFirstFactorialLine+4);  //the return line of factorial function

	EXPECT( dbg.Restart() );
	// Open source
	CString strSource = "cstack.c";
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);   //Review yefims
	MST.WFndWnd(strSource, FW_FOCUS);

	EXPECT_TRUE( src.Find("factorial(9)") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "factorial(9)") );
	CString strFunc = "main() line 122";
	VerifyLocalsWndString("int", "i_main", "4");
//	MST.WFndWnd(strSource, FW_FOCUS);
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
/*	strFunc = "mainCRTStartup";
	WriteLog( stk.FunctionIs(strFunc, 1) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
*/
	WriteLog( ShortRecCycle(9) ? PASSED : FAILED, "Recursion 1: CallStack verification through recursion step" );
	WriteLog( ShortRecCycle(8) ? PASSED : FAILED, "Recursion 2: CallStack verification through recursion step" );

	MST.WFndWnd(strSource, FW_FOCUS);
	for(int cnt=0;cnt<6;cnt++)
		EXPECT( dbg.StepToCursor());

	WriteLog( ShortRecCycle(1) ? PASSED : FAILED, "Recursion 9: CallStack verification through recursion step" );
	WriteLog( ShortRecCycle(0) ? PASSED : FAILED, "Recursion 10: CallStack verification through recursion step" );

	EXPECT( dbg.StepOver() );

	WriteLog( StackIsCorrect(0, sLastLn) ? PASSED : FAILED, "Recursion 10: Stack correctness after StepOver");
	EXPECT( dbg.StepOut() );
	WriteLog( StackIsCorrect(1, sRetLn) ? PASSED : FAILED, "Recursion 9: Stack correctness after StepOut");
	EXPECT( dbg.StepOut() );

	for(cnt=0;cnt<6;cnt++)
		EXPECT( dbg.StepOut() );

	WriteLog( StackIsCorrect(8, sRetLn) ? PASSED : FAILED, "Recursion 2: Stack correctness after StepOut");
	EXPECT( dbg.StepOut() );
	WriteLog( StackIsCorrect(9, sRetLn) ? PASSED : FAILED, "Recursion 1: Stack correctness after StepOut");
	EXPECT( dbg.StepOut() );
//	EXPECT( EndOfStackIsCorrect(0) ); //use StackIsCorrect with no smbols from recursive function
	WriteLog( StackIsCorrect(10, "") ? PASSED : FAILED, "Recursion 0: Stack correctness after all is done");

	EXPECT( dbg.StopDebugging() );

	return (TRUE);
}
//callstack options (hex display, types/values only displey?)
BOOL Cstack_specialIDETest::Options(   )
{
	return (TRUE);
}

BOOL Cstack_specialIDETest::AdvancedParam(   )
{
	bps.SetBreakpoint("Cxx_Tests");
	dbg.Go("Cxx_Tests");
//pointers to gunctions on the stack
	EXPECT_TRUE( StepInUntilFrameAdded( "poniterstofunc" ));
	//TODO check the second param on the stack when the EE bug fixed
	EXPECT_TRUE(stk.CurrentFunctionIs("poniterstofunc(void (void *, unsigned long)*") );
	dbg.StepOut();
	EXPECT_TRUE( StepInUntilFrameAdded( "poniterstofunc" ));
	//NULL pointer to function
	//TODO: change <no type> when EE bug is fixed
	EXPECT_TRUE(stk.CurrentFunctionIs("int (void *, <no type>)* 0x00000000") );
	dbg.StepOut();
//overloaded functions
	EXPECT_TRUE( StepInUntilFrameAdded("overloaded_function(int 1)") );
	dbg.StepOut();
//TODO set ambiguous BP on second entry to "overloaded_function"?
	EXPECT_TRUE( StepInUntilFrameAdded("overloaded_function(long 1)") );
	dbg.StepOut();
//different calling conventions
	bps.SetBreakpoint("fastcall_function");
	dbg.Go("fastcall_function");	
	char const * const stk0[] =
	{
		"fastcall_function(float 3.00000) line", //__fastcall does not put all params on stack
		"stdcall_function(int 1, long 2, float 3.00000) line",
		"cdecl_function(int 1, long 2, float 3.00000) line",
		"Cxx_Tests()",
		NULL
	};
	// Verify information on stack above callback
	int index;
	EXPECT_SUBTEST( (index = stk.Index( "Cxx_Tests()" ) ) != -1 );
	EXPECT_SUBTEST( stk.Compare( stk0, FrameList::NoFilter, 0, index ) );

	bps.ClearAllBreakpoints();
	//xbox EXPECT_TRUE(dbg.Go( NULL, NULL, NULL, ASSUME_NORMAL_TERMINATION));
	EXPECT_TRUE(dbg.StopDebugging());
	return (TRUE);
}


BOOL Cstack_specialIDETest::ShortRecCycle(int nParam)
{
	CString strLine;
	strLine.Format("%d", nFirstFactorialLine+1);

	EXPECT_TRUE( dbg.StepInto(2) );
	if( !StackIsCorrect(nParam, strLine) )
		return FALSE;

	strLine.Format("%d", nParam);
	EXPECT_TRUE( LocalsWndStringIs("int", "i", strLine ));

	EXPECT_TRUE( dbg.StepInto() );
	EXPECT_TRUE( LocalsWndStringIs("int", "i", strLine ));

	if( nParam )
		strLine.Format("%d",nFirstFactorialLine+4);
	else
		strLine.Format("%d",nFirstFactorialLine+2);

	if( !StackIsCorrect(nParam, strLine) )
		return FALSE;

	return TRUE;
}

BOOL Cstack_specialIDETest::StackIsCorrect(int nParam, LPCSTR strLine)
{
#define MAX_STK_LINE 80
 	char buffer[10];
	BOOL ret = TRUE;	
	int level;
	char  stktable[11][MAX_STK_LINE]; //11 for the "main()" frame
	char const * stkrec[12]; //12 for the baseline which must be NULL
	for( level = 0; level < 10 - nParam; level++ )
	{
		strcpy(stktable[level], "factorial(int ");
		strcat(stktable[level], _itoa(nParam + level, buffer, 10));
		strcat(stktable[level], ") line ");
		if(level==0)
			strcat(stktable[level], strLine);
		else
		{
			sprintf(buffer, "%d + ",nFirstFactorialLine+4);
			strcat(stktable[level], buffer);
		}
		stkrec[level]=stktable[level];
	}
	strcpy(stktable[level], "main() "); //main shld be the last
	stkrec[level]=stktable[level];
//	strcpy(stktable[++level], "mainCRTStartup"); //platform specific
//	stkrec[level]=stktable[level];
	stkrec[level+1] =  NULL;
	EXPECT_SUBTEST( ret=stk.Compare( stkrec, FrameList::NoFilter, 0, level ) );

	return ret;
}



BOOL Cstack_specialIDETest::EndOfStackIsCorrect(int level)
{ //obsolete
	return TRUE;
}