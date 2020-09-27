///////////////////////////////////////////////////////////////////////////////
//	scn02.CPP
//
//	Created by :			Date :
//		YefimS					5/19/97
//
//	Description :
//		Implementation of the CSystemScenario01Test class

#include "stdafx.h"
#include "scn02.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CEditBuildTest, CSystemTestSet, "AddCodeEditBuild", -1, CDebugAppWizAppSubSuite)

void CEditBuildTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CEditBuildTest

void CEditBuildTest::Run(void)
{	 
//	m_strAppProjName = APPNAME; // "appwizapp" //ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strAppProjName + "\\";

	XSAFETY;
	EXPECT_TRUE( AddMemberVar() );
	XSAFETY;
	EXPECT_TRUE( Build() );
	XSAFETY;
	EXPECT_TRUE( EditCode() );
	XSAFETY;
	EXPECT_TRUE( ReBuild() );
	XSAFETY;

}


BOOL CEditBuildTest::AddMemberVar( void )
{
	LogTestHeader("Add a member variable");

	if (!VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strAppProjName)))
	{
		m_pLog->RecordFailure("Could not open the project");
		return FALSE;
	}

	// Select the appnameApp class
	UIWorkspaceWindow WorkSpaceWnd; //create project workspace window object
	//if project workspace activates properly
	if (WorkSpaceWnd.Activate())  
	{
		//if classview activates properly
		if (WorkSpaceWnd.ActivateClassPane())
		{
//			int ClassSelected = WorkSpaceWnd.SelectClass((CString)"C" + m_strAppProjName + "App", m_strAppProjName);
			int ClassSelected = WorkSpaceWnd.SelectClass(m_strAppClassName, m_strAppProjName);
			if (ClassSelected == 0)  //0 = successful
			{
				LOG->Comment("Class selected properly");
			}
		}
		else
		{
			LOG->RecordFailure("Could not activate ClassView.");
			return FALSE;
		}
	}
	else
	{
		LOG->RecordFailure("Could not activate project workspace window.");
		return FALSE;
	}

	// Next code should be replaced by some support layer call when it is created
	// open the Add Member Variable Dialog and type in the Type and Name
	MST.DoKeys( "+{F10}" );
	MST.DoKeys( "{Down 3}{Enter}" );
	MST.DoKeys( "int{Tab}");
	MST.DoKeys( "m_IntVar{Enter}" );
	// end of replacable code

	// Add assignment code
	// Open appwizapp.cpp
	UIWB.OpenFile( m_strProjLoc + m_strAppProjName + ".cpp" );
// Add m_IntVar = 5 + 6;
	src.Find( "LoadStdProfileSettings" );
//	int nLine = src.GetCurrentLine();
//	src.InsertText("m_IntVar = 5 + 6;", nLine+1, 1);  //Doesn't work
	MST.DoKeys("{Down}{Home}");
	MST.DoKeys("    m_IntVar = 5 {+} 6;{Enter}", FALSE);

	return TRUE;
}

BOOL CEditBuildTest::Build( void )
{
	LogTestHeader("Build the project");

//	This is a workaround for a known CAFE problem: another workaround is to do Build in a test from a different class (this is used by sys and vproj sniffs).
	LogSubTestHeader("Open Project");
	if (!VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strAppProjName)))
	{
		m_pLog->RecordFailure("Could not open the project");
		return FALSE;
	}
// end of workaround

	LogSubTestHeader("BuildProjectAndVerify");
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.VerifyBuild()));

	LogSubTestHeader("SaveProject");
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Save()));

	return TRUE;
}

BOOL CEditBuildTest::EditCode( void )
{
	LogTestHeader("Edit code");

	// Edit assignment code	and add more
	UIWB.OpenFile( m_strProjLoc + m_strAppProjName + ".cpp" );
	src.Find( "m_IntVar = 5 + 6;" );
	MST.DoKeys("{Down}{Home}");
	MST.DoKeys("    for{(} int ii = 0; ii < 20; ii{+}{+}{)}{Enter}", FALSE);
	MST.DoKeys("{{}{Enter}", FALSE);
	MST.DoKeys("m_IntVar = ii;{Enter}", FALSE);
	MST.DoKeys("{}}{Enter}", FALSE);

	return TRUE;
}

BOOL CEditBuildTest::ReBuild( void )
{
	LogTestHeader("Rebuild the project");

//	This is a workaround for a known CAFE problem: another workaround is to do Build in a test from a different class (this is used by sys and vproj sniffs).
	LogSubTestHeader("Open Project");
	if (!VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strAppProjName)))
	{
		m_pLog->RecordFailure("Could not open the project");
		return FALSE;
	}
// end of workaround

	LogSubTestHeader("BuildProjectAndVerify");
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild)));
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.VerifyBuild()));

	LogSubTestHeader("SaveProject");
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Save()));

	return TRUE;
}



IMPLEMENT_TEST(CDebugTest, CSystemTestSet, "Debug", -1, CDebugAppWizAppSubSuite)

void CDebugTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CDebugTest

void CDebugTest::Run(void)
{
	// ToDo: strings should be replaced by member variables 
//	m_strAppProjName = APPNAME; // "appwizapp" //ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + m_strAppsRoot + "\\" + m_strAppProjName + "\\";

	XSAFETY;
	EXPECT_TRUE( BreakStep() );
	XSAFETY;
	EXPECT_TRUE( LocalsAndWatch() );
	XSAFETY;
	EXPECT_TRUE( CallStack() );
	XSAFETY;
	EXPECT_TRUE( EditAndGo() );
	XSAFETY;
	EXPECT_TRUE( Memory() );
	XSAFETY;
	EXPECT_TRUE( DAM() );
	XSAFETY;
	EXPECT_TRUE( Threads() );
	XSAFETY;

}


BOOL CDebugTest::BreakStep( void )
{
	LogTestHeader("Set a conditional BP, execute to break and step");

	bp.ClearAllBreakpoints();
	UIWB.OpenFile( m_strProjLoc + m_strAppProjName + ".cpp" );
	EXPECT_TRUE( src.Find( "m_IntVar = ii;" ) );

	// TODO(michma 2/9/98): this is just in here to nail that stupid build problem where it asks to build
	// when it shouldn't.
	//EXPECT_TRUE( dbg.StepToCursor(0, NULL, "m_IntVar = ii;") );
	EXPECT_TRUE(dbg.StepToCursor(NULL, NULL, NULL, NULL, NOWAIT));
	Sleep(1000);

	if(MST.WButtonExists("@1"))
	{
		CString str;
		MST.WStaticText("@2", str);
		m_pLog->RecordFailure("Got a button when we shouldn't have. The message text is \"%s\".", str);
		return FALSE;
	}

	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
	EXPECT_TRUE(dbg.CurrentLineIs("m_IntVar = ii;"));

	EXPECT_TRUE( bp.SetBreakOnExpr((CString)"{" + m_strAppClassName + "::InitInstance," + m_strProjLoc + m_strAppProjName + ".cpp," + m_strProjLoc + m_strAppProjName + ".exe}(m_IntVar==6)", COBP_TYPE_IF_EXP_TRUE) != NULL );
	dbg.Go(NULL,NULL,NULL,0);
	if( WaitMsgBoxText("m_IntVar==6", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Break ... when 'm_IntVar==6'\" was found as expected" );
		MST.WButtonClick( "OK" ); 
		EXPECT_TRUE_COMMENT( TRUE, "BreakAtLocationWithConditionExprTrue" );	 	
	}
	else
	{
		WriteLog( FAILED, "The MsgBox \"Break ... when 'm_IntVar==6'\" was not found" );
		EXPECT_TRUE( FALSE );	 	
	}
	EXPECT_TRUE( dbg.CurrentLineIs( "}" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_IntVar==6",1) );

	// do some stepping
	EXPECT_TRUE( dbg.StepInto(1, NULL, "for(") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, "m_IntVar = ii;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "m_IntVar = ii;") );
//	EXPECT_TRUE( cxx.ExpressionValueIs("m_IntVar==7",1) );
	EXPECT_TRUE( dbg.StepInto(1, NULL, "}") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_IntVar==8",1) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "for(") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "m_IntVar = ii;") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "}") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_IntVar==9",1) );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "}") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_IntVar==10",1) );

	return TRUE;
}

BOOL CDebugTest::LocalsAndWatch( void )
{
	LogTestHeader("Watch a local and modify");

	EXPR_INFO watch_info_actual[10], watch_info_expected[10];

	// Put the var in the Watch window and verify the value
	uiwatch.Activate();
	EXPECT_SUCCESS(uiwatch.SetName("m_IntVar"));
	EXPECT_SUCCESS(uiwatch.GetAllFields(watch_info_actual, 1, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "int", "m_IntVar", "10");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 1));

	// Change value of the local m_IntVar
	EXPECT_SUCCESS(uiwatch.SetValue("5", 1));
	RestoreFocusToSrcWnd();
	EXPECT_TRUE( dbg.StepOver(1, NULL, "for(") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_IntVar==5",1) );

	LogSubTestHeader("VerifyLocalVarUpdate");
	EXPECT_TRUE(dbg.StepOver(2));
	uivar.Activate();

	if(!uivar.SetPane(PANE_AUTO) == ERROR_SUCCESS)
	{
		m_pLog->RecordInfo("ERROR: could not activate Auto pane");
		return FALSE;
	}
	
	EXPECT_SUCCESS(uivar.GetAllFields(watch_info_actual, 2, ROW_ALL));
	FillExprInfo(watch_info_expected[0], NOT_EXPANDABLE, "int", "m_IntVar", "11");
	EXPECT_TRUE(ExprInfoIs(watch_info_actual, watch_info_expected, 1));
	RestoreFocusToSrcWnd();

	return TRUE;
}

BOOL CDebugTest::CallStack( void )
{
	LogTestHeader("CallStack: Navigate");

	CString fnName;
	CString szTitle;
	UIEditor editor;

	bp.ClearAllBreakpoints();

	LogSubTestHeader( "Navigate to AfxWinMain" );
	EXPECT_TRUE( stk.NavigateStack( "AfxWinMain" )>0 );
	MST.WFndWndWait("Winmain.cpp", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	szTitle.MakeLower();
	EXPECT_TRUE( szTitle.Find("Winmain.cpp") );

 	LogTestHeader( "Navigate to WinMain" );
	EXPECT_TRUE( stk.NavigateStack( "WinMain" )>0 );
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	szTitle.MakeLower();
	EXPECT_TRUE( szTitle.Find("Appmodul.cpp") );

 	LogTestHeader( "Navigate back to InitInstance" );
	EXPECT_TRUE( stk.NavigateStack( "InitInstance" )>0 );
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	szTitle.MakeLower();
	EXPECT_TRUE( szTitle.Find(m_strAppProjName + ".cpp") );
	EXPECT_TRUE( editor.Close() );

	return TRUE;
}

BOOL CDebugTest::EditAndGo( void )
{
	LogTestHeader("Edit code, build, execute the modified code");

	dbg.StopDebugging();

	LogSubTestHeader("EditCode");
	UIWB.OpenFile( m_strProjLoc + m_strAppProjName + ".cpp" );
	src.Find( "ii < 20" );
	MST.DoKeys("ii < 5", FALSE);
	src.Find( "m_IntVar = ii;" );
	MST.DoKeys("m_IntVar {+}= ii;{Enter}", FALSE);
	MST.DoKeys("m_IntVar *= 2;", FALSE);

	LogSubTestHeader("BuildProjectAndVerify");
//	This is a workaround for a known CAFE problem: another workaround is to do Build in a test from a different class (this is used by sys and vproj sniffs).
	LogSubTestHeader("Open Project");
	if (!VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strAppProjName)))
	{
		m_pLog->RecordFailure("Could not open the project");
		return FALSE;
	}
// end of workaround
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.VerifyBuild()));

	LogSubTestHeader("Execute");
	UIWB.OpenFile( m_strProjLoc + m_strAppProjName + ".cpp" );
	src.Find( "m_IntVar += ii;" );

	// TODO(michma 2/9/98): this is just in here to nail that stupid build problem where it asks to build
	// when it shouldn't.
	//EXPECT_TRUE( dbg.StepToCursor(0, NULL, "m_IntVar += ii;") );
	EXPECT_TRUE(dbg.StepToCursor(NULL, NULL, NULL, NULL, NOWAIT));
	Sleep(1000);

	if(MST.WButtonExists("@1"))
	{
		CString str;
		MST.WStaticText("@2", str);
		m_pLog->RecordFailure("Got a button when we shouldn't have. The message text is \"%s\".", str);
		return FALSE;
	}

	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
	EXPECT_TRUE(dbg.CurrentLineIs("m_IntVar += ii;"));
	
	EXPECT_TRUE( dbg.StepInto(2, NULL, "}") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_IntVar==22",1) );
	EXPECT_TRUE( dbg.StepOver(8, NULL, "}") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_IntVar==96",1) );

	dbg.Restart();
	UIWB.OpenFile( m_strProjLoc + m_strAppProjName + ".cpp" );
	src.Find( "m_IntVar += ii;" );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "m_IntVar += ii;") );
	EXPECT_TRUE( dbg.StepInto(18, NULL, "}") );
	EXPECT_TRUE( cxx.ExpressionValueIs("m_IntVar==404",1) );
	EXPECT_TRUE( dbg.StepOver(2) );

	return TRUE;
}

BOOL CDebugTest::Memory( void )
{
	LogTestHeader("Memory window: go to symbol and modify");

	bp.ClearAllBreakpoints();
	dbg.Restart();
	UIWB.OpenFile( m_strProjLoc + m_strAppProjName + ".cpp" );
	src.Find( "m_IntVar += ii;" );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "m_IntVar += ii;") );
	EXPECT_TRUE( dbg.StepInto(2, NULL, "}") );
	UIWB.SetMemoryFormat(MEM_FORMAT_BYTE);                                                // verify endian (white-box test)
	uimem = UIDebug::ShowDockWindow(IDW_MEMORY_WIN);
	uimem.GoToAddress("m_IntVar");
	CString Bytes = uimem.GetCurrentData(4);
	EXPECT_TRUE(Bytes == "16 00 00 00 ");

	return TRUE;
}

BOOL CDebugTest::DAM( void )
{
	LogTestHeader("Disassembly: do some stepping and toggle DAM/SRC");

	dbg.Restart();
	UIWB.OpenFile( m_strProjLoc + m_strAppProjName + ".cpp" );
	src.Find( "m_IntVar += ii;" );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "m_IntVar += ii;") );

	dbg.SetSteppingMode(ASM);

	EXPECT_TRUE( dbg.StepInto(4) );
	EXPECT_TRUE( dbg.StepOver(2) );

	//trace 
	while ( !cxx.ExpressionValueIs("m_IntVar==96",1))
	{
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE ( cxx.ExpressionValueIs("m_IntVar==96",1) );

	dbg.SetSteppingMode(SRC);

	src.Find( "m_IntVar *= 2;" );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "m_IntVar *= 2;") );

	dbg.SetSteppingMode(ASM);
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "AfxWinMain") );
	dbg.StopDebugging();

	return TRUE;
}

BOOL CDebugTest::Threads( void )
{
	LogTestHeader("Threads dialog check out");

	return TRUE;
}




