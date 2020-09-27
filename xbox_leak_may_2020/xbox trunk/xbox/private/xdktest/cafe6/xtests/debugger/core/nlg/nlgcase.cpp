///////////////////////////////////////////////////////////////////////////////
//	NLGcase.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "NLGcase.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(CNLGIDETest, CDbgTestBase, "NonLocalGoto Base", -1, CNLGSubSuite)

#define MSCPP_EXCEPTION "Microsoft C++ Exception"
	
CString GetLocExceptString(LPCSTR strExe)
{
	CString strTemp;
	if (GetLang() == CAFE_LANG_JAPANESE)
	{
		strTemp.Format("—áŠOˆ— (‰‰ñ) ‚Í %s", strExe);
	}
	else
		strTemp.Format("First-chance exception in %s", strExe);

	return strTemp;
}

void CNLGIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CNLGIDETest::Run()
{	 
	XSAFETY;
	SetProject("nlg\\nlg", PROJECT_XBE);
	XSAFETY;

	if (1) // bug in xbox CRT means skip exception handling for now
	{
		// EH
		EH_StepOverThrow( );
		XSAFETY;
		EH_TryNoThrow( );
		XSAFETY;
		EH_StepOverTry_StepOverCatch( );
		XSAFETY;

		//if( GetSystem() != SYSTEM_CHICAGO )  // bug #10127
		/* bug # 9231  postponed till after V50
		EH_StepOverReThrow( );
		XSAFETY;
		*/

		EH_BreakInCatch( );
		XSAFETY;
		EH_StepOverThrowFunc( );
		XSAFETY;
		EH_StepOutThrowFunc( );
		XSAFETY;
		EH_StepToCursorOverThrowIntoCatch( );
		XSAFETY;
		EH_BreakInCtor( );
		XSAFETY;
		EH_BreakInDtor( );
		XSAFETY;
	}

	if (1)
	{
		// Setjmp/Longjmp
		LJMP_StepOverLongJmp( );
		XSAFETY;
		LJMP_StepOverFuncWithLongJmp( );
		XSAFETY;
		LJMP_StepOutFuncWithLongJmp( );
		XSAFETY;
	}
	
        
	// SEH
	if (1)
	{
		SetProject("seh01\\seh01", PROJECT_XBE);
		XSAFETY;
		SEH_TryNoException( );
		XSAFETY;
		SEH_ExceptionContinueExecution( );
		XSAFETY;
		SEH_ExceptionExecuteHandler( );
		XSAFETY;

		SEH_TryFinallyNoException( );
		XSAFETY;

		SEH_TryFinallyException( ); 
		XSAFETY;
 
		/* bug # 9258 postponed till after V50
		SEH_BreakInFilter( );
		XSAFETY;
		*/
		SEH_StepOver__try( );
		XSAFETY;
		SEH_BreakInExcept( );
		XSAFETY;
		SEH_BreakInFinally( );
		XSAFETY;
		SEH_StepToCursorOver__tryIntoExcept( );
		XSAFETY;
		SEH_StepToCursorOver__tryIntoFinally( );
		XSAFETY;
	}

}
 
///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////


// EH

BOOL CNLGIDETest::EH_StepOverThrow( )
{
	LogTestHeader( "EH_StepOverThrow" );
   	bps.ClearAllBreakpoints();
 	bps.SetBreakpoint("Case1");
	dbg.Go("Case1");
	dbg.SetSteppingMode(SRC);
	MST.DoKeys("{down 3}");

	EXPECT_TRUE( dbg.StepToCursor(0,NULL,"throw t;")	 )	;

	EXPECT_TRUE(dbg.StepOver(1, NULL, "catch( ... ) // after Case1"));

	if(1) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		EXPECT_TRUE(dbg.VerifyNotification(GetLocExceptString("nlg.xbe"), 1, 20, TRUE) &&
					dbg.VerifyNotification(MSCPP_EXCEPTION, 1, 20, TRUE) );
	}
	dbg.StopDebugging();
	return TRUE;
}


BOOL CNLGIDETest::EH_TryNoThrow( )
{
 	LogTestHeader( "EH_TryNoThrow" );
  	bps.ClearAllBreakpoints();
	bps.SetBreakpoint("Case2");
	dbg.Go("Case2");
	int nSteps;

	/*xbox
	switch(GetUserTargetPlatforms())
	{
		case PLATFORM_WIN32_X86:
			// processor pack code-gen difference.
			if(CMDLINE->GetBooleanValue("PP", FALSE))
				nSteps = 3;
			else
				nSteps = 4;
			break;
        case PLATFORM_WIN32_ALPHA:
            nSteps = 4;
            break;
	}
	*/
	nSteps = 4; //xbox - we always use the processor pack (changed 3/27/2001)

	EXPECT_TRUE( dbg.StepOver(nSteps, NULL, "++counter; // Should execute") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "}") );
	
	return TRUE;
}

BOOL CNLGIDETest::EH_StepOverTry_StepOverCatch( )
{
 	LogTestHeader( "EH_StepOverTry_StepOverCatch" );
  	bps.ClearAllBreakpoints();
   	bps.SetBreakpoint("Case4");
	dbg.Go("Case4");
	dbg.SetSteppingMode(SRC);
	EXPECT_TRUE(dbg.StepOver(1, NULL, "try { ++counter; ThrowObject t(4); throw t; }"));

	EXPECT_TRUE(dbg.StepOver(1, NULL, "catch( ThrowObject c ) { ++counter; /* Should execute */ }"));
	if(1) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		EXPECT_TRUE( dbg.VerifyNotification(MSCPP_EXCEPTION, 3, 20, TRUE) );
		dbg.SetSteppingMode(SRC);
		MST.WFndWnd("eh01.cpp", FW_NOCASE | FW_FOCUS | FW_PART);
	}

	// changed 1 to 2 (emmang@xbox)
	EXPECT_TRUE(dbg.StepOver(2, NULL, "++counter; // Should execute"));

	dbg.StopDebugging();
  	return TRUE;
}


BOOL CNLGIDETest::EH_StepOverReThrow( )
{
 	LogTestHeader( "EH_StepOverReThrow" );
  	bps.ClearAllBreakpoints();
 	bps.SetBreakpoint("Case6");
	dbg.Go("Case6");
	dbg.SetSteppingMode(SRC);
	int nSteps = 6;
	EXPECT_TRUE( dbg.StepOver(nSteps, NULL, "throw t;") );
	EXPECT_TRUE(dbg.StepOver(3, NULL, "throw c;"));

/*	if( GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{ 
		EXPECT_TRUE(dbg.StepOver(1, NULL, NULL, NULL, 0));
	
		if( WaitMsgBoxText("Directory does not exist", 10))
		{
			MST.WButtonClick( "OK" );
			while(MST.WFndWndC("Directory does not exist", "Static", FW_PART));
		}
		if (MST.WFndWnd("Browse for Folder", FW_NOCASE | FW_FOCUS | FW_PART)
			|| MST.WFndWnd("Find Source", FW_NOCASE | FW_FOCUS | FW_PART))
				MST.WButtonClick( "Cancel" );
	
		EXPECT_TRUE( stk.CurrentFunctionIs( "CallCatchBlock" ) );
		
		EXPECT_TRUE(dbg.StepOut(1, NULL, NULL, NULL, 0) );
		if( WaitMsgBoxText("Directory does not exist", 10))
		{
			MST.WButtonClick( "OK" );
			while(MST.WFndWndC("Directory does not exist", "Static", FW_PART));
		}
		if (MST.WFndWnd("Browse for Folder", FW_NOCASE | FW_FOCUS | FW_PART)
			|| MST.WFndWnd("Find Source", FW_NOCASE | FW_FOCUS | FW_PART))
				MST.WButtonClick( "Cancel" );
		int i;
		for(i = 0; i< 5; i++)
		{ 
			if (MST.WFndWndWait("Browse for Folder", FW_NOCASE | FW_FOCUS | FW_PART, 1)
				|| MST.WFndWndWait("Find Source", FW_NOCASE | FW_FOCUS | FW_PART, 1))
					MST.WButtonClick( "Cancel" );
		}

		EXPECT_TRUE(dbg.StepOut(1, NULL, NULL, NULL, 0) );

		for(i = 0; i< 5; i++)	// WinslowF - to handle multiple dlg boxes on NT
		{ 
			if (MST.WFndWndWait("Browse for Folder", FW_NOCASE | FW_FOCUS | FW_PART, 1)
				|| MST.WFndWndWait("Find Source", FW_NOCASE | FW_FOCUS | FW_PART, 1))
					MST.WButtonClick( "Cancel" );
		}

/* Doesn't work for Win95
		EXPECT_TRUE(dbg.StepOut(3, NULL, NULL, NULL, 0) && 
				dbg.SetSteppingMode(SRC) && 
				dbg.CurrentLineIs( "catch( ThrowObject o ) {" ) );
*/
/*	 	bps.SetBreakpoint("{,eh01.cpp,}@157");
		dbg.Go(NULL, NULL, NULL, 0);
		dbg.SetSteppingMode(SRC);
		MST.DoKeys("{up}");
		EXPECT_TRUE( dbg.CurrentLineIs( "catch( ThrowObject o ) {" ) );
	}
	else
	{
*/		EXPECT_TRUE(dbg.StepOver(1, NULL, "catch( ThrowObject o ) {"));
//	}

	if(1) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		EXPECT_TRUE(dbg.VerifyNotification(GetLocExceptString("nlg.xbe"), 5, 20, TRUE));
		EXPECT_TRUE(dbg.VerifyNotification(MSCPP_EXCEPTION, 6, 20, TRUE) );
		dbg.SetSteppingMode(SRC);
		MST.WFndWnd("eh01.cpp", FW_NOCASE | FW_FOCUS | FW_PART);
	}

//	dbg.StopDebugging();
 	bps.SetBreakpoint("Case7");
	dbg.Go("Case7");

	nSteps = 6;
	EXPECT_TRUE( dbg.StepOver(nSteps, NULL, "throw t;") );

	nSteps = 2;
	EXPECT_TRUE( dbg.StepOver(nSteps, NULL, "throw;") );
	if( GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		EXPECT_TRUE(dbg.VerifyNotification(GetLocExceptString("nlg.xbe"), 7, 20, TRUE) );
		dbg.SetSteppingMode(SRC);
		MST.WFndWnd("eh01.cpp", FW_NOCASE | FW_FOCUS | FW_PART);
	}

	EXPECT_TRUE(dbg.StepOver(1, NULL, "catch( ThrowObject o ) {"));
	if( GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		EXPECT_TRUE(dbg.VerifyNotification(MSCPP_EXCEPTION, 8, 20, TRUE) );
		dbg.SetSteppingMode(SRC);
	}

	dbg.StopDebugging();
	return TRUE;
}

BOOL CNLGIDETest::EH_BreakInCatch( )
{
 	LogTestHeader( "EH_BreakInCatch" );
	dbg.Restart();
  	bps.ClearAllBreakpoints();
   	bps.SetBreakpoint("{,eh01.cpp,}@20");
   	bps.SetBreakpoint("{,eh01.cpp,}@31");
   	bps.SetBreakpoint("{,eh01.cpp,}@132");
	dbg.SetSteppingMode(SRC);
	EXPECT_TRUE( dbg.Go(NULL, "Case2();") );
	EXPECT_TRUE( dbg.Go(NULL, "Case4();") );
	EXPECT_TRUE( dbg.Go(NULL, "++counter; // Should execute") );

   	bps.ClearAllBreakpoints();

	return TRUE;
}


BOOL CNLGIDETest::EH_StepOverThrowFunc( )
{
 	LogTestHeader( "EH_StepOverThrowFunc" );
    bps.ClearAllBreakpoints();
	dbg.StopDebugging();
  
   	bps.SetBreakpoint("{,eh01.cpp,}@14");	 
	EXPECT_TRUE( dbg.Go(NULL, "Case1();") );

// can't break at catch(){} on MIPS (v-katsuf) 
	EXPECT_TRUE(dbg.StepOver(1, NULL, "catch( ... ) // after Case1"));

   	bps.SetBreakpoint("{,eh01.cpp,}@25");	 
	dbg.SetSteppingMode(SRC);  
	EXPECT_TRUE( dbg.Go(NULL, "Case3();") );

// can't break at catch(){} on MIPS (v-katsuf) 
	EXPECT_TRUE(dbg.StepOver(1, NULL, "catch( ... ){ // after Case3"));

 	bps.SetBreakpoint("{,eh01.cpp,}@31");	 
	dbg.SetSteppingMode(SRC);  
	EXPECT_TRUE( dbg.Go(NULL, "Case4();") );
	EXPECT_TRUE(dbg.StepOver(1, NULL, "++caseNum; // should execute"));

	if(1) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		EXPECT_TRUE(dbg.VerifyNotification(GetLocExceptString("nlg.xbe"), 3, 20, TRUE) );
	}

   	bps.SetBreakpoint("{,eh01.cpp,}@42");	 
	dbg.SetSteppingMode(SRC);  
	EXPECT_TRUE( dbg.Go(NULL, "Case5();") );
	EXPECT_TRUE(dbg.StepOver(1, NULL, "++caseNum; // should execute"));

   	bps.SetBreakpoint("{,eh01.cpp,}@51");	 
	dbg.SetSteppingMode(SRC);  
	EXPECT_TRUE( dbg.Go(NULL, "Case6();") );
	EXPECT_TRUE(dbg.StepOver(1, NULL, "++caseNum; // should execute"));

   	bps.SetBreakpoint("{,eh01.cpp,}@60");	 // Case7 is in different segment
	dbg.SetSteppingMode(SRC);  
	EXPECT_TRUE( dbg.Go(NULL, "Case7();") );
	EXPECT_TRUE(dbg.StepOver(1, NULL, "++caseNum; // should execute"));
	if(1) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
	EXPECT_TRUE(dbg.VerifyNotification(GetLocExceptString("nlg.xbe"), 7, 20, TRUE) &&
				dbg.VerifyNotification(GetLocExceptString("nlg.xbe"), 9, 20, TRUE) );
	}

    bps.ClearAllBreakpoints();
	dbg.StopDebugging();

	return TRUE;
}

BOOL CNLGIDETest::EH_StepOutThrowFunc( )
{
 	LogTestHeader( "EH_StepOutThrowFunc" );
  	bps.ClearAllBreakpoints();
 	dbg.Restart();
	dbg.SetSteppingMode(SRC);
	bps.SetBreakpoint("Case1");
	dbg.Go("Case1");
	EXPECT_TRUE(dbg.StepOut(1, NULL, "catch( ... ) // after Case1"));
	if(1) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		EXPECT_TRUE(dbg.VerifyNotification(GetLocExceptString("nlg.xbe"), 1, 20, TRUE) );
	}

	bps.SetBreakpoint("Case3");
	dbg.SetSteppingMode(SRC);  
	dbg.Go("Case3");
	EXPECT_TRUE(dbg.StepOut(1, NULL, "catch( ... ){ // after Case3"));

	bps.SetBreakpoint("Case5");
	dbg.SetSteppingMode(SRC);  
	dbg.Go("Case5");
if (1) //xbox GetUserTargetPlatforms() != PLATFORM_WIN32_ALPHA )			//v-katsuf
//	EXPECT_TRUE(dbg.StepOut(1, NULL, "catch( ThrowObject o )")); // This is by design now.
	EXPECT_TRUE(dbg.StepOut(1, NULL, "++caseNum; // should execute")); 

	if(1) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		EXPECT_TRUE(dbg.VerifyNotification(GetLocExceptString("nlg.xbe"), 4, 20, TRUE) );
		dbg.SetSteppingMode(SRC);  
		MST.WFndWnd("eh01.cpp", FW_NOCASE | FW_FOCUS | FW_PART);
	}
	
	bps.SetBreakpoint("Case6");
	dbg.Go("Case6");
	if(1) //xbox GetUserTargetPlatforms() != PLATFORM_WIN32_ALPHA) // bug #8481
	{
//		EXPECT_TRUE(dbg.StepOut(1, NULL, "catch( ThrowObject c )")); // This is by design now.
	}
	EXPECT_TRUE(dbg.StepOut(1, NULL, "++caseNum; // should execute")); 
	if(1) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		EXPECT_TRUE(dbg.VerifyNotification(GetLocExceptString("nlg.xbe"), 5, 20, TRUE) );
//		dbg.SetSteppingMode(SRC);  
//		MST.WFndWnd("eh01.cpp", FW_NOCASE | FW_FOCUS | FW_PART);
	}

// To do : now there is a bug in CRT

	bps.ClearAllBreakpoints();
	dbg.StopDebugging();
	return TRUE;
}

BOOL CNLGIDETest::EH_StepToCursorOverThrowIntoCatch( )
{
	LogTestHeader( "EH_StepToCursorOverThrowIntoCatch" );
	bps.ClearAllBreakpoints();
	dbg.StepInto(1, NULL, NULL, NULL, 0); 
	dbg.SetSteppingMode(SRC);  

	CString strSource = "eh01.cpp";
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);   //Review yefims
//	file.Open( m_strProjectDir + "\\" + strSource);   
	
	EXPECT_TRUE( dbg.StepToCursor(19, NULL, "++caseNum;") );

	EXPECT_TRUE( dbg.StepToCursor(31, NULL, "Case4();") );
	if(1) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		EXPECT_TRUE(dbg.VerifyNotification(GetLocExceptString("nlg.xbe"), 2, 20, TRUE) );
		dbg.SetSteppingMode(SRC);  
		MST.WFndWnd(strSource, FW_NOCASE | FW_FOCUS | FW_PART);
	}

	EXPECT_TRUE( dbg.StepToCursor(108, NULL, "catch( ThrowObject c ) { ++counter;") );
	EXPECT_TRUE( dbg.StepToCursor(132, NULL, "++counter; // Should execute") );
	EXPECT_TRUE( dbg.StepToCursor(150, NULL, "throw c;") );

	EXPECT_TRUE( dbg.StepToCursor(157, NULL, "++counter; // Should execute") );
	if(1) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		EXPECT_TRUE(dbg.VerifyNotification(GetLocExceptString("nlg.xbe"), 6, 20, TRUE) );
		dbg.SetSteppingMode(SRC);  
		MST.WFndWnd(strSource, FW_NOCASE | FW_FOCUS | FW_PART);
	}

	strSource = "eh02.cpp";
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);   //Review yefims
	EXPECT_TRUE( dbg.StepToCursor(49, NULL, "++counter; // Should execute") );
	if(1) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
		EXPECT_TRUE(dbg.VerifyNotification(GetLocExceptString("nlg.xbe"), 8, 20, TRUE) );
		dbg.SetSteppingMode(SRC);  
	}

	dbg.StopDebugging();

	return TRUE;
}

BOOL CNLGIDETest::EH_BreakInCtor( )
{
 	LogTestHeader( "EH_BreakInCtor" );
    bps.ClearAllBreakpoints();
	dbg.Restart();
   	bps.SetBreakpoint("{,ehutil.cpp,}@10");		// Ctor
	dbg.SetSteppingMode(SRC);

	EXPECT_TRUE( dbg.Go(NULL, "m_i = i;") );
	EXPECT_TRUE( stk.CurrentFunctionIs( "ThrowObject::ThrowObject" ) );
	CString func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "Case1()" ) != -1 );
    bps.ClearAllBreakpoints();

   	bps.SetBreakpoint("{,ehutil.cpp,}@16");		// Copy Ctor
	EXPECT_TRUE( dbg.Go(NULL, "m_i = src.m_i;") );
	EXPECT_TRUE( stk.CurrentFunctionIs( "ThrowObject::ThrowObject" ) );
	func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "Case1()" ) != -1 );

	EXPECT_TRUE( dbg.Go(NULL, "m_i = src.m_i;") );
	func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "Case3()" ) != -1 );

	EXPECT_TRUE( dbg.Go(NULL, "m_i = src.m_i;") );
	func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "Case4()" ) != -1 );
	EXPECT_TRUE( dbg.StepOver(1, NULL, NULL, NULL, 0) );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 4) && cxx.ExpressionValueIs("m_iCopy", 1));

   	EXPECT_TRUE( dbg.Go(NULL, "m_i = src.m_i;") );
	func = stk.GetFunction(1);
	if(0) //FrameUnwindToState is for Win32
	{
		EXPECT_TRUE( func.Find( "FrameUnwindToState" ) != -1 );	
	}
	else
	{
		EXPECT_TRUE( func.Find( "BuildCatchObject" ) != -1 );
	}
	EXPECT_TRUE( dbg.StepOver(1, NULL, NULL, NULL, 0) );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 4) && cxx.ExpressionValueIs("m_iCopy", 2));

   	EXPECT_TRUE( dbg.Go(NULL, "m_i = src.m_i;") );
	func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "Case5()" ) != -1 );
   
   	EXPECT_TRUE( dbg.Go(NULL, "m_i = src.m_i;") );
	func = stk.GetFunction(1);
	if(0) //FrameUnwindToState is for Win32
	{
		EXPECT_TRUE( func.Find( "FrameUnwindToState" ) != -1 );	//v-katsuf
	}
	else
	{
		EXPECT_TRUE( func.Find( "BuildCatchObject" ) != -1 );
	}
	EXPECT_TRUE( dbg.StepOver(1, NULL, NULL, NULL, 0) );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 5) && cxx.ExpressionValueIs("m_iCopy", 2));

   	EXPECT_TRUE( dbg.Go(NULL, "m_i = src.m_i;") );
	func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "Case6()" ) != -1 );
   
   	EXPECT_TRUE( dbg.Go(NULL, "m_i = src.m_i;") );
	func = stk.GetFunction(1);
	if(0) //FrameUnwindToState is for Win32
	{
		EXPECT_TRUE( func.Find( "FrameUnwindToState" ) != -1 );	//v-katsuf
	}
	else
	{
		EXPECT_TRUE( func.Find( "BuildCatchObject" ) != -1 );
	}
	EXPECT_TRUE( dbg.StepOver(1, NULL, NULL, NULL, 0) );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 2));

   	EXPECT_TRUE( dbg.Go(NULL, "m_i = src.m_i;") );
	func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "Case6()" ) != -1 );
   
   	EXPECT_TRUE( dbg.Go(NULL, "m_i = src.m_i;") );
	func = stk.GetFunction(1);
	if(0) //FrameUnwindToState is for Win32
	{
		EXPECT_TRUE( func.Find( "FrameUnwindToState" ) != -1 );	//v-katsuf
	}
	else
	{
		EXPECT_TRUE( func.Find( "BuildCatchObject" ) != -1 );
	}
	EXPECT_TRUE( dbg.StepOver(1, NULL, NULL, NULL, 0) );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 4));

   	EXPECT_TRUE( dbg.Go(NULL, "m_i = src.m_i;") );
	func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "Case7()" ) != -1 );
   
   	EXPECT_TRUE( dbg.Go(NULL, "m_i = src.m_i;") );
	func = stk.GetFunction(1);
	if(0) //FrameUnwindToState is for Win32
	{
		EXPECT_TRUE( func.Find( "FrameUnwindToState" ) != -1 );	//v-katsuf
	}
	else
	{
		EXPECT_TRUE( func.Find( "BuildCatchObject" ) != -1 );
	}
	EXPECT_TRUE( dbg.StepOver(1, NULL, NULL, NULL, 0) );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 7) && cxx.ExpressionValueIs("m_iCopy", 2));

	bps.ClearAllBreakpoints();

	return TRUE;
}

BOOL CNLGIDETest::EH_BreakInDtor( )
{
	LogTestHeader( "EH_BreakInDtor" );
    bps.ClearAllBreakpoints();
	dbg.Restart();
   	bps.SetBreakpoint("{,ehutil.cpp,}@30");		// Dtor
	dbg.SetSteppingMode(SRC);

	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( stk.CurrentFunctionIs( "ThrowObject::~ThrowObject" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 1) && cxx.ExpressionValueIs("m_iCopy", 0));

	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 3) && cxx.ExpressionValueIs("m_iCopy", 0));

	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 4) && cxx.ExpressionValueIs("m_iCopy", 0));

	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 4) && cxx.ExpressionValueIs("m_iCopy", 2));

 	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( stk.CurrentFunctionIs( "ThrowObject::~ThrowObject" ) );
 	CString func;
//	func = stk.GetFunction(1);
	func = stk.GetAllFunctions();
	if(0) //xbox
	{
		EXPECT_TRUE( func.Find( "FrameUnwindToState" ) != -1 );	
	}
	else
	{
		EXPECT_TRUE( func.Find( "__DestructExceptionObject" ) != -1 );
	}
	func = stk.GetAllFunctions();
/*	 code gen was changed ?
	EXPECT_TRUE( func.Find( "eh01() line 32" ) != -1 );
*/
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 4) && cxx.ExpressionValueIs("m_iCopy", 1));

 	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
// 	func = stk.GetFunction(1);
	func = stk.GetAllFunctions();
	if(0) //xbox
	{
		EXPECT_TRUE( func.Find( "FrameUnwindToState" ) != -1 );	//v-katsuf
	}
	else
	{
		EXPECT_TRUE( func.Find( "__DestructExceptionObject" ) != -1 );
	}
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 3) && cxx.ExpressionValueIs("m_iCopy", 1));

	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 1) && cxx.ExpressionValueIs("m_iCopy", 1));

	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 5) && cxx.ExpressionValueIs("m_iCopy", 0));

	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 5) && cxx.ExpressionValueIs("m_iCopy", 2));

 	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 5) && cxx.ExpressionValueIs("m_iCopy", 1));

	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 0));

	if(1) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_X86 )
	{
	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 1));
	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 2));
	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 4));
	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 3));
	}
	else if(0) //xbox GetUserTargetPlatforms() == PLATFORM_WIN32_ALPHA)	
	{
	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 2));
	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 1));
	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 4));
	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 3));
	}
	else
	{
	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 2));
	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 4));
	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 3));
	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 6) && cxx.ExpressionValueIs("m_iCopy", 1));
	}

 	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 7) && cxx.ExpressionValueIs("m_iCopy", 0));
 	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 7) && cxx.ExpressionValueIs("m_iCopy", 2));
 	EXPECT_TRUE( dbg.Go(NULL, "++dtorCounter;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_i", 7) && cxx.ExpressionValueIs("m_iCopy", 1));

    bps.ClearAllBreakpoints();

	return TRUE;
}



// SEH
BOOL CNLGIDETest::SEH_TryNoException( )

{
	LogTestHeader( "SEH_TryNoException" );
   	bps.ClearAllBreakpoints();
 	dbg.Restart();
	bps.SetBreakpoint("SehCase1");
	dbg.SetSteppingMode(SRC);
	dbg.Go("SehCase1");
	EXPECT_TRUE(dbg.StepOver(4, NULL, "RaiseNoException();"));

	// processor pack code-gen difference.
	if(1) //xbox - always true, skip the check CMDLINE->GetBooleanValue("PP", FALSE))
		{EXPECT_TRUE(dbg.StepOver(1, NULL, "++counter; // should execute"));}
	else
		{EXPECT_TRUE(dbg.StepOver(2, NULL, "++counter; // should execute"));}

	EXPECT_TRUE(dbg.StepOver(1, NULL, "}"));
	dbg.StopDebugging();
	return TRUE;
}

BOOL CNLGIDETest::SEH_ExceptionContinueExecution( )

{
 	LogTestHeader( "SEH_ExceptionContinueExecution" );
  	bps.ClearAllBreakpoints();
	bps.SetBreakpoint("SehCase3");
	dbg.Go("SehCase3");
	EXPECT_TRUE(dbg.StepOver(4, NULL, "RaiseIntDivideByZero();"));
	EXPECT_TRUE(dbg.StepOver(1, NULL, "++counter; // Should continue here"));
	EXPECT_TRUE(dbg.VerifyNotification("Integer Divide by Zero", 1, 20, TRUE) );
	dbg.SetSteppingMode(SRC);

	// processor pack code-gen difference.
	if(1) //xbox - always true, skip the check CMDLINE->GetBooleanValue("PP", FALSE))
		{EXPECT_TRUE(dbg.StepOver(1, NULL, "++counter; // should execute"));}
	else
		{EXPECT_TRUE(dbg.StepOver(2, NULL, "++counter; // should execute"));}

	dbg.StopDebugging();
	return TRUE;
}

BOOL CNLGIDETest::SEH_ExceptionExecuteHandler( )
{
 	LogTestHeader( "SEH_ExceptionExecuteHandler" );
  	bps.ClearAllBreakpoints();
	bps.SetBreakpoint("SehCase4");
	dbg.Go("SehCase4");
	EXPECT_TRUE(dbg.StepOver(4, NULL, "IntDivideByZero();"));
	EXPECT_TRUE(dbg.StepOver(1, NULL, "__except( ++counter, ExecuteHandler() ) {"));
	EXPECT_TRUE(dbg.VerifyNotification(GetLocExceptString("seh01.xbe"), 2, 20, TRUE) );
	dbg.SetSteppingMode(SRC);
	EXPECT_TRUE(dbg.StepOver(1, NULL, "++counter; // should execute"));	
	dbg.StopDebugging();
	return TRUE;
}

BOOL CNLGIDETest::SEH_TryFinallyNoException( )

{
	LogTestHeader( "SEH_TryFinallyNoException" );
	bps.ClearAllBreakpoints();
	dbg.StopDebugging();
	bps.SetBreakpoint("SehCase2");
	dbg.SetSteppingMode(SRC);
	dbg.Go("SehCase2");
	EXPECT_TRUE(dbg.StepOver(4, NULL, "RaiseNoException();"));

//	EXPECT_TRUE(dbg.StepOver(2, NULL, "++counter; // should execute")); //bug 9256 postponed

	// processor pack code-gen difference.
	if(1) //xbox - always true, skip the check CMDLINE->GetBooleanValue("PP", FALSE))

	{
 		EXPECT_TRUE(dbg.StepOver(1, NULL, "}"));
		EXPECT_TRUE( stk.CurrentFunctionIs( "SehCase2() line 54" ) );
	}
	else
	{
		dbg.StepOver();
		EXPECT_TRUE(dbg.StepInto(1, NULL, "++counter; // should execute"));
		EXPECT_TRUE( stk.CurrentFunctionIs( "SehCase2() line 52" ) );
 		EXPECT_TRUE(dbg.StepOver(2, NULL, "}"));
	}

	dbg.StopDebugging();
	return TRUE;
}

BOOL CNLGIDETest::SEH_TryFinallyException( )
{
	LogTestHeader( "SEH_TryFinallyException" );
   	bps.ClearAllBreakpoints();
	bps.SetBreakpoint("SehCase6");
	dbg.SetSteppingMode(SRC);
	EXPECT_TRUE( dbg.Go("SehCase6") );
	EXPECT_TRUE(dbg.StepOver(6, NULL, "IntDivideByZero();"));
	EXPECT_TRUE(dbg.StepOver(1, NULL, "++counter; // should execute"));
	EXPECT_TRUE( dbg.VerifyNotification("Integer Divide by Zero", 1, 20, TRUE) );
	dbg.SetSteppingMode(SRC);
	//Warning:  the following line has been changed from a two step to a single stepover
	//It is uncertain if the test was always bad or code generation changed handling in some way
	//So, if you see a failure where "__except( ++counter, ExecuteHandler() is expected but 
	//the cursor is one line short, then a code gen change has occured.
	EXPECT_TRUE(dbg.StepOver(1, NULL, "__except( ++counter, ExecuteHandler() ) {"));
	EXPECT_TRUE(dbg.StepOver(1, NULL, "++counter; // should execute"));
	dbg.StopDebugging();
	return TRUE;
}

BOOL CNLGIDETest::SEH_BreakInFilter( )
{
	LogTestHeader( "SEH_BreakInFilter" );
 	dbg.Restart();
	dbg.SetSteppingMode(SRC);
  	bps.ClearAllBreakpoints();
	EXPECT_TRUE( bps.SetBreakpoint("ContinueExecution") );
	EXPECT_TRUE( bps.SetBreakpoint("ExecuteHandler") );
	EXPECT_TRUE( bps.SetBreakpoint("ContinueSearch") );

	EXPECT_TRUE( dbg.Go("ContinueExecution") );
	EXPECT_TRUE( stk.CurrentFunctionIs( "ContinueExecution()" ) );
 	CString func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "SehCase3()" ) != -1 );
	EXPECT_TRUE(dbg.StepOut(1, NULL, "__except( ++counter, ContinueExecution() ) {"));

	EXPECT_TRUE( dbg.Go("ExecuteHandler") );
	EXPECT_TRUE( stk.CurrentFunctionIs( "ExecuteHandler()" ) );
 	func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "SehCase4()" ) != -1 );
	EXPECT_TRUE(dbg.StepOut(1, NULL, "__except( ++counter, ExecuteHandler() ) {"));

	EXPECT_TRUE( dbg.Go("ExecuteHandler") );
	EXPECT_TRUE( stk.CurrentFunctionIs( "ExecuteHandler()" ) );
 	func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "SehCase6()" ) != -1 );
	EXPECT_TRUE(dbg.StepOver(4, NULL, "__except( ++counter, ExecuteHandler() ) {"));

	EXPECT_TRUE( dbg.Go("ExecuteHandler") );
	EXPECT_TRUE( stk.CurrentFunctionIs( "ExecuteHandler()" ) );
 	func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "SehCase8()" ) != -1 );
	EXPECT_TRUE(dbg.StepOut(1, NULL, "__except( ++counter, ExecuteHandler() ) {"));

	EXPECT_TRUE( dbg.Go("ContinueExecution") );
	EXPECT_TRUE( stk.CurrentFunctionIs( "ContinueExecution()" ) );
 	func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "SehCase8()" ) != -1 );
	EXPECT_TRUE(dbg.StepOut(1, NULL, "__except( ++counter, ContinueExecution() ) {"));

	EXPECT_TRUE( dbg.Go("ContinueSearch") );
	EXPECT_TRUE( stk.CurrentFunctionIs( "ContinueSearch()" ) );
 	func = stk.GetFunction(1);
	EXPECT_TRUE( func.Find( "SehCase5()" ) != -1 );
	EXPECT_TRUE(dbg.StepOut(1, NULL, "__except( ++counter, ContinueSearch() ) {"));

	dbg.StopDebugging();
  	bps.ClearAllBreakpoints();
	return TRUE;
}

BOOL CNLGIDETest::SEH_StepOver__try( )
{
	LogTestHeader( "SEH_StepOver__try" );
	CString strSource = "seh01.cpp";
 	dbg.Restart();
	bps.ClearAllBreakpoints();
	bps.SetBreakpoint("SehCase8");
	dbg.SetSteppingMode(SRC);
	MST.WFndWnd(strSource, FW_NOCASE | FW_FOCUS | FW_PART);
	EXPECT_TRUE( dbg.Go("SehCase8") );
	EXPECT_TRUE(dbg.StepOver(4, NULL, "__try { ++counter; RaiseIntDivideByZero();"));
	EXPECT_TRUE(dbg.StepOver(1, NULL, "__finally { ++counter; /* should execute */ }"));
	EXPECT_TRUE( dbg.VerifyNotification("Integer Divide by Zero", 2, 20, TRUE) );
	dbg.SetSteppingMode(SRC);
//	MST.WFndWnd(strSource, FW_NOCASE | FW_FOCUS | FW_PART);
	EXPECT_TRUE(dbg.StepOver(1, NULL, "__except( ++counter, ExecuteHandler() ) {"));
	EXPECT_TRUE(dbg.StepOver(1, NULL, "++counter; // should execute"));

	// processor pack code-gen difference.
	if(1) //xbox - CMDLINE->GetBooleanValue("PP", FALSE))
		{EXPECT_TRUE(dbg.StepOver(1, NULL, "__try { ++counter; RaiseIntDivideByZero(); ++counter;"));}
	else
		{EXPECT_TRUE(dbg.StepOver(2, NULL, "__try { ++counter; RaiseIntDivideByZero(); ++counter;"));}

	EXPECT_TRUE(dbg.StepOver(1, NULL, "++counter; // should execute"));
	EXPECT_TRUE( dbg.VerifyNotification("Integer Divide by Zero", 3, 20, TRUE) );

	dbg.StopDebugging();
	return TRUE;
}

BOOL CNLGIDETest::SEH_BreakInExcept( )
{
	LogTestHeader( "SEH_BreakInExcept" );
 	dbg.Restart();
	dbg.SetSteppingMode(SRC);
  	bps.ClearAllBreakpoints();

	EXPECT_TRUE( bps.SetBreakpoint("{,seh01.cpp,}@91") );
	EXPECT_TRUE( bps.SetBreakpoint("{,seh01.cpp,}@136") );

	EXPECT_TRUE(dbg.Go(NULL, "++counter; // should execute")  &&
		stk.CurrentFunctionIs( "SehCase4()" ) );
	EXPECT_TRUE(dbg.Go(NULL, "++counter; // should execute")  &&
		stk.CurrentFunctionIs( "SehCase6()" ) );

	bps.ClearAllBreakpoints();

	dbg.StopDebugging();
	return TRUE;
}

BOOL CNLGIDETest::SEH_BreakInFinally( )
{
	LogTestHeader( "SEH_BreakInFinally" );
 	dbg.Restart();
	dbg.SetSteppingMode(SRC);
  	bps.ClearAllBreakpoints();

	EXPECT_TRUE( bps.SetBreakpoint("{,seh01.cpp,}@52") );
	EXPECT_TRUE( bps.SetBreakpoint("{,seh01.cpp,}@130") );

	EXPECT_TRUE(dbg.Go(NULL, "++counter; // should execute") &&
		stk.CurrentFunctionIs( "SehCase2()" ) );
	EXPECT_TRUE(dbg.Go(NULL, "++counter; // should execute") &&
		stk.CurrentFunctionIs( "SehCase6()" ) );

	bps.ClearAllBreakpoints();
	dbg.StopDebugging();
	return TRUE;
}

BOOL CNLGIDETest::SEH_StepToCursorOver__tryIntoExcept( )
{
	LogTestHeader( "SEH_StepToCursorOver__tryIntoExcept" );
 	dbg.Restart();
	dbg.SetSteppingMode(SRC);
  	bps.ClearAllBreakpoints();

	CString strSource = "seh01.cpp";
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);   //Review yefims
//	file.Open( m_strProjectDir + "\\" + strSource);   

	EXPECT_TRUE(dbg.StepToCursor(84, NULL, "__try {"));
	EXPECT_TRUE(dbg.StepToCursor(91, NULL, "++counter; // should execute"));
	EXPECT_TRUE(dbg.StepToCursor(121, NULL, "__try {"));
	EXPECT_TRUE(dbg.StepToCursor(136, NULL, "++counter; // should execute"));

	dbg.StopDebugging();

	return TRUE;
}


BOOL CNLGIDETest::SEH_StepToCursorOver__tryIntoFinally( )
{
	LogTestHeader( "SEH_StepToCursorOver__tryIntoFinally" );
 	dbg.Restart();
	dbg.SetSteppingMode(SRC);
  	bps.ClearAllBreakpoints();

	CString strSource = "seh01.cpp";
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);   //Review yefims
//	file.Open( m_strProjectDir + "\\" + strSource);   

	EXPECT_TRUE(dbg.StepToCursor(47, NULL, "__try {"));
	EXPECT_TRUE(dbg.StepToCursor(52, NULL, "++counter; // should execute"));
	EXPECT_TRUE(dbg.StepToCursor(123, NULL, "__try {"));
	EXPECT_TRUE(dbg.StepToCursor(130, NULL, "++counter; // should execute"));

	dbg.StopDebugging();

	return TRUE;
}


// Setjmp/Longjmp
BOOL CNLGIDETest::LJMP_StepOverLongJmp( )
{
	LogTestHeader( "LJMP_StepOverLongJmp" );
 	bps.ClearAllBreakpoints();
 	bps.SetBreakpoint("ljmp01");
	dbg.SetSteppingMode(SRC);

	// xbox additions
	EXPECT_TRUE(dbg.StepOver()); // skip the opening bracket
	EXPECT_TRUE(dbg.SetNextStatement("ljmp01();"));

	dbg.Go("ljmp01");
	MST.DoKeys("{down 15}");
	EXPECT_TRUE( dbg.StepToCursor(0,NULL,"longjmp( mark, -1 );")	 )	;
	EXPECT_TRUE(dbg.StepOver(1, NULL, "jmpret = setjmp( mark );"));
	return TRUE;
}

BOOL CNLGIDETest::LJMP_StepOverFuncWithLongJmp( )
{
 	LogTestHeader( "LJMP_StepOverFuncWithLongJmp" );
 	bps.ClearAllBreakpoints();
 	bps.SetBreakpoint("{,ljmp01.cpp,}@55");
	dbg.SetSteppingMode(SRC);

	dbg.Go(NULL, "ljmp02( mark, i );");
	EXPECT_TRUE(dbg.StepOver(1, NULL, "jmpret = setjmp( mark );"));
	return TRUE;
}

BOOL CNLGIDETest::LJMP_StepOutFuncWithLongJmp( )
{
	LogTestHeader( "LJMP_StepOutFuncWithLongJmp" );
  	bps.ClearAllBreakpoints();
 	bps.SetBreakpoint("ljmp02");
	dbg.SetSteppingMode(SRC);

	dbg.Go("ljmp02");
	EXPECT_TRUE(dbg.StepOut(1, NULL, "jmpret = setjmp( mark );"));

	dbg.StopDebugging();
	return TRUE;
}


BOOL CNLGIDETest::IsUnhandledException(CString except_text)
{
	if( WaitMsgBoxText( except_text, 10 ) )
	{
		MST.WButtonClick( "OK" );
		return TRUE;
	}
	else
		return FALSE;
}







