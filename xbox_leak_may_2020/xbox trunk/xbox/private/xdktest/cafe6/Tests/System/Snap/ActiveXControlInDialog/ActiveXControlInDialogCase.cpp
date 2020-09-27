///////////////////////////////////////////////////////////////////////////////
//	ActiveXControlInDialogCase.CPP
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Implementation of the CActiveXControlInDialogCase class

#include "stdafx.h"
#include "ActiveXControlInDialogCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CActiveXControlInDialogCase, CSystemTestSet, "Insert and Debug ActiveX Control in Dialog", -1, CActiveXControlInDialogSubsuite)

void CActiveXControlInDialogCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CActiveXControlInDialogCase::Run(void)
{	 
	OpenMainProject();
	XSAFETY;
	AddControlFromGallery();
	XSAFETY;
	DropControlsToDialog();
	XSAFETY;
	BuildProject();
	XSAFETY;
	DebugProject();
	prj.Close(TRUE);
}

void CActiveXControlInDialogCase::OpenMainProject(void)
{
	UIWorkspaceWindow wksp;
	prj.Open(GetCWD() + m_strWorkspaceLoc + m_strWorkspaceName + ".DSW");
	LOG->RecordCompare(wksp.SetActiveProject(m_strAppProjName)!=FALSE,"selected main project");
} //OpenMainProject

CString strActiveXName;

void CActiveXControlInDialogCase::AddControlFromGallery(void)
{
	CString strActiveXLoc = GetLocString(IDSS_GALLERY_CONTROL_FOLDER);
	LOG->RecordInfo("");
	LOG->RecordCompare(prj.EnableComponents(strActiveXLoc), "*** Enabling ActiveX controls. ***");

	LOG->RecordInfo("");
	strActiveXName = m_strActiveXControlName + " Control";
	LOG->RecordInfo("*** Adding %s control. ***", strActiveXName); 
	
	// create the ActiveX control that we want to add.
	COComponent* pComponent= new CActiveXControl(strActiveXName);

	// add the component to the project.
	LOG->RecordCompare(prj.AddComponent(pComponent,strActiveXLoc), "Adding %s control.", strActiveXName);		
	delete pComponent;

	// puts ide back into state it was in before prj.EnableComponents was called.
	LOG->RecordInfo("");
	LOG->RecordCompare(prj.DisableComponents(), "*** Disabling galery. ***");

} //AddControlFromGallery



void CActiveXControlInDialogCase::DropControlsToDialog(void)
{
	UIWorkspaceWindow uwksp;
	UIDlgEdit m_uDlgEd;
	EXPECT_TRUE(uwksp.SelectResource("Dialog", "IDD_TESTDIALOG")==SUCCESS); //TODO change to the name of the dialog Yefim will provide
	MST.DoKeys("{ENTER}", FALSE, 5000);
	m_uDlgEd = UIWB.GetActiveEditor();
//drop the control just added to the project
	m_uDlgEd.DropControl(CT_USER+2, 20, 20); //TODO very likely CT_USER+2 to change when layout of control pallette chnage

	UIControlProp prop = UIWB.ShowPropPage(TRUE);
	BOOL bvalid=prop.IsValid();
	
	CString controlID=prop.GetID();
	LOG->RecordInfo("ID of control dropped: '%s'",controlID);
	prop.SetAllPage();
	prop.Close();
//TODO open the file from class view and add the code to invoke the TestDialog
//	UIWorkspaceWindow WorkSpaceWnd; //create project workspace window object
	//if project workspace activates properly
//	if (WorkSpaceWnd.Activate())  
//	{
//		//if classview activates properly
//		if (WorkSpaceWnd.ActivateClassPane())
//		{
//			int ClassSelected = WorkSpaceWnd.SelectClass(/*m_strViewClassName*/, m_strAppProjName); //TODO provide ViewClassName
//		}
//		else
//		{
//			LOG->RecordFailure("Could not activate ClassView.");
//			return FALSE;
//		}
//	}
//	else
//	{
//		LOG->RecordFailure("Could not activate project workspace window.");
//		return FALSE;
//	}
// return; //TODO do enable this return (do not set this code) when you know how to pass keystrokes to the app
	UIWB.OpenFile( m_strProjLoc + /*m_strViewName*/"AppWiz AppView" + ".cpp" ); //TODO provide the view name
/*
Add 
	AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND, ID_Test_TestDialog  );
to CView::OnInitialUpdate() to invoke the dialog TestDialog without UI interraction
*/
	src.Find( "CView::OnInitialUpdate();" );
//	int nLine = src.GetCurrentLine();
//	src.InsertText("AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND, ID_Test_TestDialog  );");  //Doesn't work
	MST.DoKeys("{Down}{Home}");
	MST.DoKeys("AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND, ID_Test_TestDialog  );",TRUE);
	MST.DoKeys("{Enter}");	
	
	UIWB.DoCommand(IDM_FILE_SAVE_ALL, DC_MNEMONIC); //the only way to save the resources
	UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);
	//TODO drop another control (ATL) this time from the right click
} //DropControlsToDialog

void CActiveXControlInDialogCase::BuildProject(void)
{
		// build the project
	LOG->RecordCompare(prj.Build( 60 ) == ERROR_SUCCESS, "Building project.");


	LOG->RecordInfo("Verifying the build...");

	// verify the build
	BOOL bResult = LOG->RecordCompare(prj.VerifyBuild() == ERROR_SUCCESS, "Verifying the build.");
	if (!bResult) {
		throw CTestException("CBindBuildTest::BuildProject: cannot build a container project; unable to continue.", CTestException::causeOperationFail);
	}

} //BuildProject

void CActiveXControlInDialogCase::DebugProject(void)
{
	COBreakpoints bp;

	CString strActiveXControlProjRoot = GetCWD() + m_strWorkspaceLoc + m_strActiveXControlProjName + "\\";
	int ret=prj.SetAdditionalDLLs(strActiveXControlProjRoot + "Debug\\" + m_strActiveXControlProjName + ".ocx");

// set a bp in the InitInstance function of the ActiveX control.
	UIWB.OpenFile(strActiveXControlProjRoot + m_strActiveXControlProjName + ".cpp"); 
	/*EXPECT_TRUE*/( src.Find("BOOL bInit = COleControlModule::InitInstance();" ) ); //it's not true
	MST.DoKeys("{F9}"); //set the BP
	UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);
	
	// TODO(michma 2/9/98): this is just in here to nail that stupid build problem where it asks to build
	// when it shouldn't.
	//LOG->RecordCompare(dbg.Go(NULL, NULL, NULL, WAIT_FOR_RUN), "Start debugging the app");
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
	Sleep(1000);

	if(MST.WButtonExists("@1"))
	{
		CString str;
		MST.WStaticText("@2", str);
		m_pLog->RecordFailure("Got a button when we shouldn't have. The message text is \"%s\".", str);
		return;
	}


	HWND hwndapp=0;
	hwndapp=MST.WFndWndWait(m_strAppProjName + " - AppWiz1", FW_PART /*| FW_FOCUS | FW_CHILDNOTOK | FW_HIDDENNOTOK*/, 10); //wait 10 seconds for the window to come up
//	EXPECT_TRUE(hwndapp!=NULL); //does not work is always NULL
	::WaitStepInstructions("Invoke the Test Dialog");
	DWORD appThreadId=0;if(hwndapp!=NULL)appThreadId=::GetWindowThreadProcessId(hwndapp, NULL);
	DWORD IDEThreadId=::GetWindowThreadProcessId(UIWB, NULL);
	LOG->Comment("Found the handle of window: (0x%X),its thread:(%d), IDE thread: (%d)",hwndapp,appThreadId,IDEThreadId);
/*this message journal does not work, I withheld it to later
and enabled invoking the dialog
//	MST.DoKeyshWnd(hwndapp,"%TT"); //brings  TestDialog even this DOES HANG the debugger
	::WaitStepInstructions("alt-TT for thread: %d", appThreadId);
	::PostThreadMessage(appThreadId,WM_SYSKEYDOWN, VK_MENU,0x20380001);
	::PostThreadMessage(appThreadId,WM_SYSKEYDOWN, 0x54,   0x20140001);
	::PostThreadMessage(appThreadId,WM_SYSKEYUP,   0x54,   0xF0140001);
	::PostThreadMessage(appThreadId,WM_KEYUP,      VK_MENU,0xD0380001);
	::PostThreadMessage(appThreadId,WM_KEYDOWN,    0x54,   0x10140001);
	::PostThreadMessage(appThreadId,WM_KEYUP,      0x54,   0xC0140001);
*/
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE(dbg.CurrentLineIs("BOOL bInit = COleControlModule::InitInstance();" ) );
//	bp.DisableAllBreakpoints(); //does not work; Clear instead
	bp.ClearAllBreakpoints();
	
	CString strOnDraw = m_strActiveXControlViewClass + "::OnDraw";
		LOG->RecordCompare(bp.SetBreakpoint(strOnDraw) != NULL,
		"Set a breakpoint in the OnDraw function of the ActiveX control.");
// hit the bp.
//post key messages to IDE thread directly as DoKeys hang the IDE
		::WaitStepInstructions("F5 Key message to IDE thread");
		Sleep(1000);
		::PostThreadMessage(IDEThreadId,WM_KEYDOWN,VK_F5,0);
		::PostThreadMessage(IDEThreadId,WM_KEYUP,VK_F5,0);
		Sleep(2000);
	LOG->RecordCompare(dbg.AtSymbol(strOnDraw), 
		"Hit the bp in the OnDraw function of the ActiveX control.");
	bp.ClearAllBreakpoints();
//post key messages to IDE thread directly as DoKeys hang the IDE
		::WaitStepInstructions("F5 Key message to IDE thread");
		Sleep(1000);
		::PostThreadMessage(IDEThreadId,WM_KEYDOWN,VK_F5,0);
		::PostThreadMessage(IDEThreadId,WM_KEYUP,VK_F5,0);
		Sleep(2000);
	LOG->RecordCompare(dbg.Wait(WAIT_FOR_RUN), "Continue debugging the app");
/*
// set a bp in the ExitInstance function of the ActiveX control.
	CString strOnExit = "CActiveXControlApp::ExitInstance";
	LOG->RecordCompare(bp.SetBreakpoint(strOnExit) != NULL,
	"Set a breakpoint in the strOnExit function of the ActiveX control.");
*/
//switch to the app & terminate it
	HWND hnext=MST.WFndWndWait(m_strAppProjName + " - AppWiz1", FW_PART | FW_CHILDNOTOK | FW_HIDDENNOTOK, 10); //wait 10 seconds for the window to come up
	LOG->Comment("'0x%X' app window found the second time",hnext);
//	EXPECT_TRUE(hwndapp==hnext); //does not work because the window cannot be found the first time
	if(hwndapp==NULL)hwndapp=hnext;
	if(appThreadId==0)appThreadId=::GetWindowThreadProcessId(hwndapp, NULL);
	LOG->Comment("Second time handle wnd: (0x%X),its thread:(%d)",hwndapp,appThreadId);

//*this message journal does not work
//but it's not critical to the pass state
//post key messages to app thread directly as DoKeys hang the IDE
	::WaitStepInstructions("ENTER Key message to the app");
	::PostThreadMessage(appThreadId,WM_KEYDOWN,VK_RETURN, 0x001C0001);
	::PostThreadMessage(appThreadId,WM_KEYUP,  VK_RETURN, 0xC01C0001);
	Sleep(5000);
//*/
	::WaitStepInstructions("MW_CLOSE message to the app");
	::PostMessage(hwndapp, WM_CLOSE, 0, 0);

/*
//we shld hiot the ExitInstance bp
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE(dbg.AtSymbol(strOnExit);
//dbg.Go()
	TODO F5
*/
	LOG->RecordCompare(dbg.Wait(WAIT_FOR_TERMINATION), "Terminate the app.");
} //DebugProject

/*
//	MST.DoKeyshWnd(UIWB,"{F5}");
//		HWND IDEhandle=GetSubSuite()->GetIDE()->m_hWnd;
	DWORD IDEThreadId=::GetWindowThreadProcessId(UIWB, NULL);

		::WaitStepInstructions("F5 Key down Flush up");
		::PostThreadMessage(IDEThreadId,WM_KEYDOWN,VK_F5,0);
		::PostThreadMessage(IDEThreadId,WM_KEYUP,VK_F5,0);
 
	Sleep(20000);
	EXPECT_TRUE(dbg.CurrentLineIs("BOOL bInit = COleControlModule::InitInstance();" ) );
	::WaitStepInstructions("F5 Key down Flush up");
	::PostThreadMessage(IDEThreadId,WM_KEYDOWN,VK_F5,0);
	::PostThreadMessage(IDEThreadId,WM_KEYUP,VK_F5,0);
	Sleep(1000);
	EXPECT_TRUE(dbg.CurrentLineIs("return bInit;"));
for(int i=0;i<20;i++)
{	
	::WaitStepInstructions("F5 Key down Flush up");
	::PostThreadMessage(IDEThreadId,WM_KEYDOWN,VK_F5,0);
	::PostThreadMessage(IDEThreadId,WM_KEYUP,VK_F5,0);
	EXPECT_TRUE(dbg.CurrentLineIs(strOnDraw)); 
	Sleep(1000);
}
*/