///////////////////////////////////////////////////////////////////////////////
//	cs_stres.CPP
//
//	Created by :			Date :
//		YefimS			01/06/94
//
//	Description :
//		Implementation of the Cstack2IDETest class
//

#include "stdafx.h"
#include "stacsub.h"
#include "cs_stres.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(Cstack_stressIDETest, CDbgTestBase, "CallStack Stress", -1, CstackSubSuite)

void Cstack_stressIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void Cstack_stressIDETest::Run()
{	 
	XSAFETY;
	if( SetProject("stack01\\stack01", PROJECT_XBE) )
	{	  
		XSAFETY;
		CheckLocals();
		XSAFETY;
		Recursion();
		XSAFETY;
	}
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL Cstack_stressIDETest::CheckLocals(   )
{
 	LogTestHeader( "CheckLocals" );

	CString strSource = "cstack.c";
	
	dbg.StepInto();

	// Open source
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);   //Review yefims

	EXPECT_TRUE( src.Find("VoidProcNoParam();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "VoidProcNoParam();") );
//	MST.WFndWnd(strSource, FW_FOCUS);
	EXPECT_TRUE( dbg.StepInto() );
	CString strFunc = "VoidProcNoParam()";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);

	EXPECT_TRUE( src.Find("CharProc1Param(SCHAR_MIN)") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "CharProc1Param(SCHAR_MIN)") );
	EXPECT_TRUE( dbg.StepInto(2) );
	strFunc = "CharProc1Param(char -128)";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
	VerifyLocalsWndString("char", "c", "-128");

	EXPECT_TRUE( src.Find("UIntProc1Param(UINT_MAX)") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "UIntProc1Param(UINT_MAX)") );
	EXPECT_TRUE( dbg.StepInto(2) );
	strFunc = "UIntProc1Param(unsigned int 4294967295)";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
	VerifyLocalsWndString("unsigned int", "ui", "4294967295");

	EXPECT_TRUE( src.Find("LongProc1Param") );
	EXPECT_TRUE( dbg.StepToCursor(0) );
	EXPECT( dbg.StepInto(2) );
	// Verify function is in stack
	strFunc = "LongProc1Param(long -2147483648)";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
	// Verify locals in Locals window							
	VerifyLocalsWndString("long", "l", "-2147483648");

	EXPECT_TRUE( src.Find("LDoubleProc1Param(ldblmin)") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "LDoubleProc1Param(ldblmin)") );
	EXPECT( dbg.StepInto(2) );
	strFunc = "LDoubleProc1Param(double 2.2250738585072e-308)";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
	VerifyLocalsWndString("double", "ld", "2.2250738585072e-308");

	EXPECT_TRUE( src.Find("PShortProc1Param") );
	EXPECT_TRUE( dbg.StepToCursor(0) );
	EXPECT( dbg.StepInto(2) );
	// Verify function is in stack
	strFunc = "PShortProc1Param(short *";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
	// Verify locals in Locals window							
	VerifyLocalsWndString("short *", "ps", "0x");

	EXPECT_TRUE( src.Find("average(1,2,3,4,5,6,7,8,9, -1)") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "average(1,2,3,4,5,6,7,8,9, -1)") );
	EXPECT( dbg.StepInto(2) );
	strFunc = "average(int 1)";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
	VerifyLocalsWndString("int", "first", "1");

	EXPECT_TRUE( src.Find("factorial(9)") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "factorial(9)") );
	EXPECT( dbg.StepInto(2) );
	strFunc = "factorial(int 9)";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
	VerifyLocalsWndString("int", "i", "9");

	EXPECT_TRUE( src.Find("MultiParam(CHAR_MIN,") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "MultiParam(CHAR_MIN,") );
	EXPECT( dbg.StepInto(2) );
	strFunc = "MultiParam(char -128, int 2147483647, short 32767, long 2147483647, double 1.7976931348623e+308, double 1.7976931348623e+308, float 3.40282e+038)";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);

	VerifyLocalsWndString("char", "c", "-128");
	VerifyLocalsWndString("int", "i", "2147483647", 2);
	VerifyLocalsWndString("short", "s", "32767", 3);
	VerifyLocalsWndString("long", "l", "2147483647", 4);
	VerifyLocalsWndString("double", "d", "1.7976931348623e+308", 5);
	VerifyLocalsWndString("double", "ld", "1.7976931348623e+308", 6);
	VerifyLocalsWndString("float", "f", "3.40282e+038", 7);

	return (TRUE);
}

const int nFirstFactorialLine = 311;
//char szLastLn==nFirstFactorialLine+5;
//char szRetLn==nFirstFactorialLine+4;

BOOL Cstack_stressIDETest::Recursion(   )
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
	//the copy of the sequence from cspecial case
//	CString strFunc = "main() line 122";
//	VerifyLocalsWndString("int", "i_main", "4");
//	MST.WFndWnd(strSource, FW_FOCUS);
//	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
/*	strFunc = "mainCRTStartup";
	WriteLog( stk.FunctionIs(strFunc, 1) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
*/
//CHRISKOZ	WriteLog( ShortRecCycle(9) ? PASSED : FAILED, "Recursion 1: CallStack verification through recursion step" );
//CHRISKOZ	WriteLog( ShortRecCycle(8) ? PASSED : FAILED, "Recursion 2: CallStack verification through recursion step" );
//CHRISKOZ	WriteLog( ShortRecCycle(7) ? PASSED : FAILED, "Recursion 3: CallStack verification through recursion step" );
//CHRISKOZ	WriteLog( ShortRecCycle(6) ? PASSED : FAILED, "Recursion 4: CallStack verification through recursion step" );
//CHRISKOZ	WriteLog( ShortRecCycle(5) ? PASSED : FAILED, "Recursion 5: CallStack verification through recursion step" );
//CHRISKOZ	WriteLog( ShortRecCycle(4) ? PASSED : FAILED, "Recursion 6: CallStack verification through recursion step" );
//CHRISKOZ	WriteLog( ShortRecCycle(3) ? PASSED : FAILED, "Recursion 7: CallStack verification through recursion step" );
//CHRISKOZ	WriteLog( ShortRecCycle(2) ? PASSED : FAILED, "Recursion 8: CallStack verification through recursion step" );

//CHRISKOZ	WriteLog( ShortRecCycle(1) ? PASSED : FAILED, "Recursion 9: CallStack verification through recursion step" );
//CHRISKOZ	WriteLog( ShortRecCycle(0) ? PASSED : FAILED, "Recursion 10: CallStack verification through recursion step" );

	RecCycle(9); LOG->RecordSuccess("Recursion 1: Stack correctness in recursive StepInto");
	RecCycle(8); LOG->RecordSuccess("Recursion 2: Stack correctness in recursive StepInto");
	RecCycle(7); LOG->RecordSuccess("Recursion 3: Stack correctness in recursive StepInto");
	RecCycle(6); LOG->RecordSuccess("Recursion 4: Stack correctness in recursive StepInto");
	RecCycle(5); LOG->RecordSuccess("Recursion 5: Stack correctness in recursive StepInto");
	RecCycle(4); LOG->RecordSuccess("Recursion 6: Stack correctness in recursive StepInto");
//CHRISKOZ enable when StepOver recurvisefunction does not work
//	RecCycle(3); 
//	RecCycle(2);
//	RecCycle(1);
//	RecCycle(0);

	EXPECT( dbg.StepOver() ); //CHRISKOZ, bug: StepOver the recursive function used not to work

//CHRISKOZ enable when StepOver recurvisefunction does not work
//	WriteLog( StackIsCorrect(0, sLastLn) ? PASSED : FAILED, "Recursion 10: Stack correctness after StepOver");
//	EXPECT( dbg.StepOut() );
//	WriteLog( StackIsCorrect(1, sRetLn) ? PASSED : FAILED, "Recursion 9: Stack correctness after StepOut");
//	EXPECT( dbg.StepOut() );
//	WriteLog( StackIsCorrect(2, sRetLn) ? PASSED : FAILED, "Recursion 8: Stack correctness after StepOut");
//	EXPECT( dbg.StepOut() );
//	WriteLog( StackIsCorrect(3, sRetLn) ? PASSED : FAILED, "Recursion 7: Stack correctness after StepOut");
//	EXPECT( dbg.StepOut() );
	WriteLog( StackIsCorrect(4, sLastLn) ? PASSED : FAILED, "Recursion 6: Stack correctness after StepOver");
	EXPECT( dbg.StepOut() );
	WriteLog( StackIsCorrect(5, sRetLn) ? PASSED : FAILED, "Recursion 5: Stack correctness after StepOut");
	EXPECT( dbg.StepOut() );
	WriteLog( StackIsCorrect(6, sRetLn) ? PASSED : FAILED, "Recursion 4: Stack correctness after StepOut");
	EXPECT( dbg.StepOut() );
	WriteLog( StackIsCorrect(7, sRetLn) ? PASSED : FAILED, "Recursion 3: Stack correctness after StepOut");
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



void Cstack_stressIDETest::RecCycle(int nParam)
{
	CString strLine;
	strLine.Format("%d", nFirstFactorialLine);

	EXPECT( dbg.StepInto() );
	StackIsCorrect(nParam, strLine);

	EXPECT( dbg.StepInto() );

	strLine.Format("%d", nParam);
	VerifyLocalsWndString("int", "i", strLine);	
	strLine.Format("%d", nFirstFactorialLine+1);	
	StackIsCorrect(nParam, strLine);

	EXPECT( dbg.StepInto() );
	strLine.Format("%d", nParam);
	VerifyLocalsWndString("int", "i", strLine);

	if( nParam )
		strLine.Format("%d", nFirstFactorialLine+4);
	else
		strLine.Format("%d", nFirstFactorialLine+2);

	StackIsCorrect(nParam, strLine);
}

BOOL Cstack_stressIDETest::ShortRecCycle(int nParam)
{
	CString strLine;
	strLine.Format("%d", nFirstFactorialLine+1);

	EXPECT_TRUE( dbg.StepInto(2) );
	if( !StackIsCorrect(nParam, strLine) )
		return FALSE;

	strLine.Format("%d", nParam);
	EXPECT_TRUE( LocalsWndStringIs("int", "i",  strLine));
//	if( !StackIsCorrect(nParam, nFirstFactorialLine+1) )
//		return FALSE;
	EXPECT_TRUE( dbg.StepInto() );
	EXPECT_TRUE( LocalsWndStringIs("int", "i", strLine));

	if( nParam )
		strLine.Format("%d", nFirstFactorialLine+4);
	else
		strLine.Format("%d", nFirstFactorialLine+2);

	if( !StackIsCorrect(nParam, strLine) )
		return FALSE;

	return TRUE;
}


BOOL Cstack_stressIDETest::StackIsCorrect(int nParam, LPCSTR strLine)
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
LOG->RecordInfo("Function at level %d: expected value: %s ", level, stkrec[level]);
	}
	strcpy(stktable[level], "main() "); //main shld be the last
	stkrec[level]=stktable[level];
LOG->RecordInfo("Function at level %d: expected value: %s ", level, stkrec[level]);
//	strcpy(stktable[++level], "mainCRTStartup"); //platform specific
//	stkrec[level]=stktable[level];
	stkrec[level+1] =  NULL;
	EXPECT_SUBTEST( ret=stk.Compare( stkrec, FrameList::NoFilter, 0, level ) );

	return ret;
}


BOOL Cstack_stressIDETest::EndOfStackIsCorrect(int level)
{ //obsolete
	return TRUE;
}

void Cstack_stressIDETest::VerifyStack(int nParam, LPCSTR strLine)
{ //obsolete
/* 	char buffer[4];

	// Verify stack
	CString strFunc = (CString)"factorial(int " + (CString)_itoa(nParam, buffer, 10) + ") line " + strLine;
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
	int level;
	for( level = 1; level < 10 - nParam; level++ )
	{
		strFunc = (CString)"factorial(int " + (CString)_itoa(nParam + level, buffer, 10) + ") line 'nFirstFactorialLine+4' + ";
		WriteLog( stk.FunctionIs(strFunc, level) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
	}		
	VerifyEndOfStack(level);
*/
}

void Cstack_stressIDETest::VerifyEndOfStack(int level)
{ //obsolete
}
