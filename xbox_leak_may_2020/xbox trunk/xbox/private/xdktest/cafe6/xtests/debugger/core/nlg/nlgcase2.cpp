///////////////////////////////////////////////////////////////////////////////
//	NLGcase2.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "NLGcase2.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(CNLG2IDETest, CDbgTestBase, "NonLocalGoto Special", -1, CNLGSubSuite)
												 
void CNLG2IDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CNLG2IDETest::Run()
{	 
	XSAFETY;
	SetProject("nlg\\nlg", PROJECT_XBE);
	XSAFETY;

	// EH
#ifdef DO_EH_CASES
	EH_StepIntoThrow( );
	XSAFETY;
/* bug # 9231postponed till after V50
	EH_StepIntoReThrow( );
	XSAFETY;
*/


//	EH_CatchInDLL( );  // plan to do it
//	XSAFETY;
//	EH_CatchFromDLL( );
//	XSAFETY;
//	EH_ReThrowInCtor( );  // may do it later
//	XSAFETY;
#endif

	if(1) //xbox if( GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
	// Setjmp/Longjmp
		LJMP_StepIntoLongJmp( );
		XSAFETY;
	
	// SEH
		SetProject("seh01\\seh01", PROJECT_XBE);
		XSAFETY;
/* bug # 9263  */
		SEH_StepIntoRaiseException( );  // too weird on Chicago: no CRT support
		XSAFETY;
		SEH_ExceptionContinueSearch( );
		XSAFETY;
	}
}
 
///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////


// EH

BOOL CNLG2IDETest::EH_StepIntoThrow( )
{
	LogTestHeader( "EH_StepIntoThrow" );
   	bps.ClearAllBreakpoints();
 	bps.SetBreakpoint("Case3");
	dbg.Go("Case3");
	int nSteps = 4;
	int i;

	EXPECT_TRUE( dbg.StepOver(nSteps, NULL, "throw t;") );
	EXPECT_TRUE(dbg.StepInto(1, NULL, "ThrowObject::ThrowObject( const ThrowObject& src) {"));
	EXPECT_TRUE(dbg.StepOut(1, NULL, "throw t;"));
	EXPECT_TRUE(dbg.StepInto(1, NULL, NULL, NULL, 0));

	if( WaitMsgBoxText("Directory does not exist", 10))
	{
		MST.WButtonClick( "OK" );
		while(MST.WFndWndC("Directory does not exist", "Static", FW_PART));
	}
	if (MST.WFndWnd("Browse for Folder", FW_NOCASE | FW_FOCUS | FW_PART)
		|| MST.WFndWnd("Find Source", FW_NOCASE | FW_FOCUS | FW_PART))
		MST.WButtonClick( "Cancel" );
	
	if ( !stk.CurrentFunctionIs( "eh01()" )	)
	{
		MST.DoKeys("{up}");
		EXPECT_TRUE( dbg.CurrentLineIs( "_CxxThrowException:" ) && stk.CurrentFunctionIs( "_CxxThrowException" ) );

		EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, NULL, 0) );

		if( GetSystem() & SYSTEM_WIN ) 
		{
			for (i = 0; i < 9; i++)
			{
				if( WaitMsgBoxText("Directory does not exist", 10))
				{
					MST.WButtonClick( "OK" );
					while(MST.WFndWndC("Directory does not exist", "Static", FW_PART));
				}
				if (MST.WFndWnd("Browse for Folder", FW_NOCASE | FW_FOCUS | FW_PART)
					|| MST.WFndWnd("Find Source", FW_NOCASE | FW_FOCUS | FW_PART))
				{
					MST.WButtonClick( "Cancel" );
					dbg.StepOut(1, NULL, NULL, NULL, 0);
				}
			}
		}
		dbg.SetSteppingMode(SRC); 
	}

	EXPECT_TRUE( dbg.CurrentLineIs( "catch( ... ){ // after Case3" ) );

	EXPECT_TRUE(dbg.VerifyNotification("First-Chance Exception in nlg.exe", 2, 20, TRUE));
	dbg.SetSteppingMode(SRC);

	dbg.StopDebugging();
	return TRUE;
}

BOOL CNLG2IDETest::EH_StepIntoReThrow( )
{
	LogTestHeader( "EH_StepIntoReThrow" );
   	bps.ClearAllBreakpoints();
 	bps.SetBreakpoint("Case7");
	dbg.Go("Case7");

	int nSteps = 6;
	EXPECT_TRUE( dbg.StepOver(nSteps, NULL, "throw t;") );

	nSteps = 2;
	EXPECT_TRUE( dbg.StepOver(nSteps, NULL, "throw;") );

	EXPECT_TRUE(dbg.StepInto(1, NULL, NULL, NULL, 0));

/*	if( GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		if( WaitMsgBoxText("Directory does not exist", 10))
		{
			MST.WButtonClick( "OK" );
			while(MST.WFndWndC("Directory does not exist", "Static", FW_PART));
		}
		if (MST.WFndWndWait("Browse for Folder", FW_NOCASE | FW_FOCUS | FW_PART, 1)
			|| MST.WFndWndWait("Find Source", FW_NOCASE | FW_FOCUS | FW_PART, 1))
				MST.WButtonClick( "Cancel" );
	
		MST.DoKeys("{up}");
		EXPECT_TRUE( dbg.CurrentLineIs( "_CxxThrowException:" ) );

		EXPECT_TRUE(dbg.StepOut(1, NULL, NULL, NULL, 0) );
		if( WaitMsgBoxText("Directory does not exist", 10))
		{
			MST.WButtonClick( "OK" );
			while(MST.WFndWndC("Directory does not exist", "Static", FW_PART));
		}
		if (MST.WFndWndWait("Browse for Folder", FW_NOCASE | FW_FOCUS | FW_PART, 1)
			|| MST.WFndWndWait("Find Source", FW_NOCASE | FW_FOCUS | FW_PART, 1))
				MST.WButtonClick( "Cancel" );

//		EXPECT_TRUE(dbg.StepOut(3, NULL, NULL, NULL, 0) && dbg.SetSteppingMode(SRC) );
	 	bps.SetBreakpoint("{,eh02.cpp,}.49");
		dbg.Go(NULL, NULL, NULL, 0);
		dbg.SetSteppingMode(SRC);
		MST.WFndWnd("eh02.cpp", FW_NOCASE | FW_FOCUS | FW_PART);
		MST.DoKeys("{up}");
	} 
*/	
	EXPECT_TRUE( dbg.CurrentLineIs( "catch( ThrowObject o ) {" ) );
 	
	EXPECT_TRUE(dbg.VerifyNotification("First-Chance Exception in nlg.exe", 8, 20, TRUE));

	dbg.StopDebugging();
	return TRUE;
}



BOOL CNLG2IDETest::EH_CatchInDLL( )
{
	return TRUE;
}

BOOL CNLG2IDETest::EH_CatchFromDLL( )
{
	return TRUE;
}


//BOOL CNLG2IDETest::EH_ReThrowInCtor( )
//{
//	LogTestHeader( "EH_ReThrowInCtor" );
//	dbg.Restart();
//   	bps.SetBreakpoint("{,ehutil.cpp,}.23");		// throw in Ctor
//	EXPECT_TRUE( dbg.Go(NULL, "throw m_i;") );
// 	EXPECT_TRUE( stk.CurrentFunctionIs( "ThrowObject::ThrowObject()" ) );
//	CString func = stk.GetFunction(1);
//	EXPECT_TRUE( func.Find( "Case8()" ) != -1 );
//
//	EXPECT_TRUE( dbg.Go(NULL, "m_i = i;") );
//	EXPECT_TRUE( stk.CurrentFunctionIs( "ThrowObject::ThrowObject(int 1)" ) );
//	func = stk.GetFunction(1);
//	EXPECT_TRUE( func.Find( "Case8()" ) != -1 );
//
//	EXPECT_TRUE(dbg.StepOver(1, NULL, "++counter; // Should execute"));
//	EXPECT_TRUE( stk.CurrentFunctionIs( "Case8() line 58" ) );
//
//	// F10 n times should get to line 68 in eh01.cpp
//   	EXPECT_TRUE(dbg.StepOver(2));
//	if( WaitMsgBoxText( "Browse for Folder", 10 ) )
//			MST.WButtonClick( "Cancel" );
//
//	// Verify we are in DAM 
// 	UIDAM dam;
//	EXPECT_TRUE( dam.IsActive() );
//	
//	EXPECT_TRUE( dbg.CurrentLineIs( "__NLG_Return" ) )
//   	func = stk.GetFunction(0);
//	EXPECT_TRUE( func.Find( "_CallSettingFrame" ) != -1 );
//   	func = stk.GetFunction(1);
//	EXPECT_TRUE( func.Find( "eh01()" ) != -1 );
//   	EXPECT_TRUE(bps.SetBreakpoint("{eh01}.64");
//	EXPECT_TRUE( dbg.Go(NULL, "catch( ... ) {") );
//	EXPECT_TRUE(dbg.StepOver(1, NULL, "++caseNum; // Should execute"));
//
////	EXPECT_TRUE( src.GetCurrentLine() == 68 );
//
//   	bps.ClearAllBreakpoints();
//
//	return TRUE;
//}


// SEH

BOOL CNLG2IDETest::SEH_ExceptionContinueSearch( )
{
 	LogTestHeader( "SEH_ExceptionContinueSearch" );
  	bps.ClearAllBreakpoints();
	bps.SetBreakpoint("SehCase5");
	dbg.Go("SehCase5");
	EXPECT_TRUE(dbg.StepOver(4, NULL, "IntDivideByZero();"));
	EXPECT_TRUE(dbg.StepOver(1, NULL, NULL, NULL, 0));
	EXPECT_TRUE( IsUnhandledException("Integer Divide by Zero") );
	EXPECT_TRUE( dbg.VerifyNotification("Integer Divide by Zero", 6, 20, TRUE) );
	dbg.SetSteppingMode(SRC);
	EXPECT_TRUE( dbg.CurrentLineIs( "k = i/j;" ) );
	MST.DoKeys("{down}");
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepOver(1, NULL, "++counter;"));

	// processor pack code-gen difference.
	if(1) //xbox - CMDLINE->GetBooleanValue("PP", FALSE))
		{EXPECT_TRUE(dbg.StepOver(1, NULL, "++counter; // should execute"));}
	else
		{EXPECT_TRUE(dbg.StepOver(2, NULL, "++counter; // should execute"));}

	dbg.StopDebugging();
	return TRUE;
}


BOOL CNLG2IDETest::SEH_StepIntoRaiseException( )
{
 	LogTestHeader( "SEH_StepIntoRaiseException" );
  	bps.ClearAllBreakpoints();
	bps.SetBreakpoint("SehCase3");
	dbg.Go("SehCase3");
	dbg.SetSteppingMode(SRC);
	EXPECT_TRUE(dbg.StepInto(4, NULL, "RaiseIntDivideByZero();"));
	EXPECT_TRUE(dbg.StepInto(1, "RaiseIntDivideByZero"));
	//xbox - EXPECT_TRUE(dbg.StepInto(3, NULL, "++counter; // Should continue here"));
	// emmang@xbox.com - we can't really step into the RaiseException() call;
	// on VC, the StepInto behaves just like a StepOver because it is a 
	// call to the kernel.  On Xbox, we can really step into it because
	// it is merely a lib call, but if we do we get a Find Source dlg box
	EXPECT_TRUE(dbg.StepOver(3, NULL, "++counter; // Should continue here"));


	// processor pack code-gen difference.
	if(1) //xbox - CMDLINE->GetBooleanValue("PP", FALSE))
		{EXPECT_TRUE(dbg.StepInto(1, NULL, "++counter; // should execute"));}
	else
		{EXPECT_TRUE(dbg.StepInto(2, NULL, "++counter; // should execute"));}

	bps.SetBreakpoint("SehCase4");
	dbg.Go("SehCase4");
	EXPECT_TRUE(dbg.StepInto(5, "IntDivideByZero"));
	EXPECT_TRUE(dbg.StepInto(4, NULL, "__except( ++counter, ExecuteHandler() ) {"));
	EXPECT_TRUE(dbg.StepInto(1, NULL, "++counter; // should execute"));
	bps.ClearAllBreakpoints();
	dbg.StopDebugging();
	return TRUE;
}


// Setjmp/Longjmp

BOOL CNLG2IDETest::LJMP_StepIntoLongJmp( )
{
 	LogTestHeader( "LJMP_StepIntoLongJmp" );
 	bps.ClearAllBreakpoints();
 	bps.SetBreakpoint("{,ljmp01.cpp,}.32");
	dbg.SetSteppingMode(SRC);

	// xbox additions
	EXPECT_TRUE(dbg.StepInto()); // begin debugging
	EXPECT_TRUE(dbg.StepOver()); // skip the opening bracket
	EXPECT_TRUE(dbg.SetNextStatement("ljmp01();"));

	EXPECT_TRUE( dbg.Go(NULL, "longjmp( mark, -1 );") );
	
	EXPECT_TRUE(dbg.StepInto(1, NULL, NULL, NULL, 0));

	if( WaitMsgBoxText("Directory does not exist", 10))
	{
		MST.WButtonClick( "OK" );
		while(MST.WFndWndC("Directory does not exist", "Static", FW_PART));
	}
	if (MST.WFndWndWait("Browse for Folder", FW_NOCASE | FW_FOCUS | FW_PART, 1)
		|| MST.WFndWndWait("Find Source", FW_NOCASE | FW_FOCUS | FW_PART, 1))
			MST.WButtonClick( "Cancel" );
	if ( stk.CurrentFunctionIs( "ljmp01()" ) )
	{
		EXPECT_TRUE( dbg.CurrentLineIs( "jmpret = setjmp( mark );" ) );
	}
	else
	{
		EXPECT_TRUE( stk.CurrentFunctionIs( "_longjmp()" ) );

		EXPECT_TRUE(dbg.StepOut(1, NULL, NULL, NULL, 0) && 
				dbg.SetSteppingMode(SRC) && 
				dbg.CurrentLineIs( "jmpret = setjmp( mark );" ) );
	}

	return TRUE;
}


BOOL CNLG2IDETest::IsUnhandledException(CString except_text)
{
	if( WaitMsgBoxText( except_text, 10 ) )
	{
		MST.WButtonClick( "OK" );
		return TRUE;
	}
	else
		return FALSE;
}







