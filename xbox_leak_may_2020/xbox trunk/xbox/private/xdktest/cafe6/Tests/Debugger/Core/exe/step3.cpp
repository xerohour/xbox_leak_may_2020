///////////////////////////////////////////////////////////////////////////////
//	STEP3.CPP
//
//	Created by :			Date :
//		YefimS			01/06/94
//
//	Description :
//		Implementation of the CST1TestCases class
//

#include <process.h>
#include "stdafx.h"
#include "execase2.h"

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////


BOOL Cexe2IDETest::StepToCursorMixed(   )
{
 	LogTestHeader( "StepToCursorMixed" );

	CString strSource = "bp__ldld.c";

	// Open source
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);

	EXPECT_TRUE( src.Find("c10[counter]  = (char)counter;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "c10[counter]  = (char)counter;") );
	dbg.SetSteppingMode(ASM);
	CString strLine = "c10[counter]  = (char)counter;";
	MST.DoKeys("{up}");

	EXPECT_TRUE_COMMENT( dbg.CurrentLineIs( strLine ), "DAM: expected line 'c10[counter]  = (char)counter;' " );

	MST.DoKeys("{down 3}");
	dbg.StepToCursor();
	if( UIWB.GetPcodeSwitch() )
		strLine = "LdfL8";
	else
		strLine = "move.b";

    EXPECT_TRUE( dbg.CurrentLineIs( strLine ) );

	return (TRUE);
}


BOOL Cexe2IDETest::StepOutMixed(   )
{
 	LogTestHeader( "StepOutMixed" );

	CString strSource = "bp__ldld.c";

	// Open source
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);

	EXPECT_TRUE( src.Find("c10[counter]  = (char)counter;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "c10[counter]  = (char)counter;") );

	dbg.SetSteppingMode(ASM);
	CString strLine = "c10[counter]  = (char)counter;";
	MST.DoKeys("{up}");
    EXPECT_TRUE( dbg.CurrentLineIs( strLine ) );

	MST.DoKeys("{down 3}");
	dbg.StepToCursor();

	if( UIWB.GetPcodeSwitch() )
		strLine = "LdfL8";
	else
		strLine = "move.b";

    EXPECT_TRUE( dbg.CurrentLineIs( strLine ) );

	dbg.StepOut();
  	EXPECT_TRUE( stk.CurrentFunctionIs( "TestBP()" ) );

	if( UIWB.GetPcodeSwitch() )
		strLine = "Call";
	else
		strLine = "jsr"; 
	MST.DoKeys("{up}");

    EXPECT_TRUE( dbg.CurrentLineIs( strLine ) );

	strLine = "ld = CFncLDLD(ld);"; 
	MST.DoKeys("{up}");
	MST.DoKeys("{up}");
	if( !UIWB.GetPcodeSwitch() )
	{
		MST.DoKeys("{up}");
		MST.DoKeys("{up}");
		MST.DoKeys("{up}");
	}

    EXPECT_TRUE( dbg.CurrentLineIs( strLine ) );
	return (TRUE);
}



BOOL Cexe2IDETest::StepOverMixed(   )
{
 	LogTestHeader( "StepOverMixed" );

	CString strSource = "testbp.c";
	CString strLine;

	// Open source
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);

	dbg.StepOver();
	dbg.SetSteppingMode(ASM);

	if( UIWB.GetPcodeSwitch() )
		strLine = "Call";
	else
		strLine = "jsr"; 

	EXPECT_TRUE( dbg.StepOver(1, NULL, strLine) );

	dbg.StepOver();
	MST.DoKeys("{up}");
    EXPECT_TRUE( dbg.CurrentLineIs( strLine ) );
	return (TRUE);
}



BOOL Cexe2IDETest::StepIntoMixed(   )
{
 	LogTestHeader( "StepIntoMixed" );
	COSource		src;	// WinslowF - added to call GoToLine. It's not in UIWB any more.

	CString strSource = "testbp.c";

    dbg.Restart();
	dbg.SetSteppingMode(SRC);
	CString strLine = "{";
	if( UIWB.GetPcodeSwitch() )
		strLine = "int iBP = TestBP();";
    EXPECT_TRUE( dbg.CurrentLineIs( strLine ) );

	// Open source
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);
	EXPECT_TRUE( src.Find("f = CFncFF(f);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "f = CFncFF(f);") );
	dbg.SetSteppingMode(ASM);

	if( UIWB.GetPcodeSwitch() )
		strLine = "Call";
	else
		strLine = "jsr"; 

	EXPECT_TRUE( dbg.StepInto(1, NULL, strLine, "TestBP()") );

	dbg.StepInto(3);
  	EXPECT_TRUE( stk.CurrentFunctionIs( "CFncSSRecurse" ) );

	MST.DoKeys("{up}");
	if( UIWB.GetPcodeSwitch() )
		strLine = "Cuc = (uchar)NTimes;";
	else
		strLine = "{"; 

    EXPECT_TRUE( dbg.CurrentLineIs( strLine ) );

	if( UIWB.GetPcodeSwitch() )
		MST.DoKeys("{up 5}");
	
	strLine = "CFncSSRecurse(short NTimes)"; 
	MST.DoKeys("{up}");
    EXPECT_TRUE( dbg.CurrentLineIs( strLine ) );

	EXPECT(dbg.StopDebugging());
	return (TRUE);
}



