///////////////////////////////////////////////////////////////////////////////
//      SYSCASES.CPP
//
//      Created by :                    Date :
//              RickKr                                  1/25/94
//		History:
//				BrianCr		12/12/94	Ported to CAFE v3
//
//      Description :
//              Implementation of the CSysTestCases class
//

#include "stdafx.h"
#include <io.h>
#include "syscases.h"
#include "sniff.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

///////////////////////////////////////////////////////////////////////////////
// CSysAppWizTest: SYS AppWizard test

IMPLEMENT_TEST(CSysAppWizTest, CTest, "Create AppWizard Project", 4, CSysSubSuite)

BOOL CSysAppWizTest::RunAsDependent(void)
{
	// name of the project file
	CString strProjFile = m_strCWD + PROJDIR + PROJNAME + ".DSP";

	// if the project file exists, there's no need to run this test as a dependency
	if (_access(strProjFile, 0) != -1) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

void CSysAppWizTest::Run(void)
{
	NewProject();
	SaveProject();
}

BOOL CSysAppWizTest::NewProject(void)
{
	BOOL bResult;
	
	// get the project from the subsuite
	COProject* pPrj = &(GetSubSuite()->m_prj);

	GetLog()->RecordInfo("Creating an AppWizard project named '%s'...", PROJNAME);

	// create a new AppWizard project
	APPWIZ_OPTIONS options;
	options.m_bUseMFCDLL = CMDLINE->GetBooleanValue("MFCDLL", TRUE); 

	// The following command line switch is mapped as follows:
	//	0 - APPWIZ_OPTIONS::AW_OLE_NONE (Default)
	//  1 - APPWIZ_OPTIONS::AW_OLE_CONTAINER
	//  2 - APPWIZ_OPTIONS::AW_OLE_MINISERVER (not supported)
	//  3 - APPWIZ_OPTIONS::AW_OLE_FULLSERVER
	//  4 - APPWIZ_OPTIONS::AW_OLE_CONTAINERSERVER
	options.m_OLECompound = (APPWIZ_OPTIONS::OLESupport)CMDLINE->GetIntValue("OLE", 4);
	ASSERT(options.m_OLECompound < 5);

	switch(options.m_OLECompound)
	{
	case APPWIZ_OPTIONS::AW_OLE_NONE:
		GetLog()->RecordInfo("No OLE options selected.");
		break;
	case APPWIZ_OPTIONS::AW_OLE_CONTAINER:
		GetLog()->RecordInfo("Creating an OLE container application.");
		break;
	case APPWIZ_OPTIONS::AW_OLE_MINISERVER:
		GetLog()->RecordFailure("Creating an OLE mini-server application is not supported by the System sniff.");
		break;
	case APPWIZ_OPTIONS::AW_OLE_FULLSERVER:
		GetLog()->RecordInfo("Creating an OLE full-server application.");
		break;
	case APPWIZ_OPTIONS::AW_OLE_CONTAINERSERVER:
		GetLog()->RecordInfo("Creating an OLE container/server application.");
		break;
	default:
		GetLog()->RecordInfo("Invalid OLE option specified: %d  Defaulting to no OLE support", options.m_OLECompound);
		options.m_OLECompound = APPWIZ_OPTIONS::AW_OLE_NONE;
		break;
	}

	bResult = GetLog()->RecordCompare(pPrj->NewAppWiz(PROJNAME, GetCWD(), GetUserTargetPlatforms(), &options) == ERROR_SUCCESS, "Creating an AppWizard project named '%s'.", PROJNAME);
	if (!bResult) {
		throw CTestException("CSysAppWizTest::NewProject: cannot create a new AppWizard project; unable to continue.", CTestException::causeOperationFail);
	}

	// turn on browse info file (bsc) generation (off by default).
	bResult = GetLog()->RecordCompare(pPrj->ToggleBSCGeneration(TOGGLE_ON) == ERROR_SUCCESS, "Turning on BSC Generation.", PROJNAME);
	if (!bResult) {
		throw CTestException("CSysAppWizTest::NewProject: cannot turn on BSC generation; unable to continue.", CTestException::causeOperationFail);
	}

	// change output file location to same as project file (avoid debug, macdbg, pmcdbg, etc).
	bResult = GetLog()->RecordCompare(pPrj->SetOutputFile((CString)PROJNAME + ".Exe") == ERROR_SUCCESS, "Resetting output file location.", PROJNAME);
	if (!bResult) {
		throw CTestException("CSysAppWizTest::NewProject: cannot reset output file location; unable to continue.", CTestException::causeOperationFail);
	}

	return bResult;
}

BOOL CSysAppWizTest::SaveProject(void)
{
	// get the project from the subsuite
	COProject* pPrj = &(GetSubSuite()->m_prj);

	CString strPrjName = pPrj->GetFullPath();

	GetLog()->RecordInfo("Saving the AppWizard project. Expecting the file '%s' to be written...", strPrjName);

	// save the project
	return GetLog()->RecordCompare(pPrj->Save(TRUE, strPrjName) == ERROR_SUCCESS, "Saving the AppWizardProject.");
}

///////////////////////////////////////////////////////////////////////////////
// CSysClassesTest: SYS Verify Class test

IMPLEMENT_TEST(CSysClassesTest, CTest, "Verify AppWizard Classes", -1, CSysSubSuite)

void CSysClassesTest::Run(void)
{
	GetLog()->RecordInfo("Verifying ClassView functionality for a small subset of classes");

	// First check the classes that appear in every case...
	CString strBaseLineFileName = m_strCWD + '\\' + "stdclass.bas";
	GetSubSuite()->m_prj.VerifyClasses(strBaseLineFileName);

	
	// Now verify additional OLE oriented classes

	// The following command line switch is mapped as follows:
	//	0 - APPWIZ_OPTIONS::AW_OLE_NONE (Default)
	//  1 - APPWIZ_OPTIONS::AW_OLE_CONTAINER
	//  2 - APPWIZ_OPTIONS::AW_OLE_MINISERVER (not supported)
	//  3 - APPWIZ_OPTIONS::AW_OLE_FULLSERVER
	//  4 - APPWIZ_OPTIONS::AW_OLE_CONTAINERSERVER
	APPWIZ_OPTIONS::OLESupport ole_choice = (APPWIZ_OPTIONS::OLESupport)CMDLINE->GetIntValue("OLE", 4);

	switch(ole_choice)
	{
	case APPWIZ_OPTIONS::AW_OLE_NONE:
		strBaseLineFileName = m_strCWD + '\\' + "no_ole.bas";
		break;
	case APPWIZ_OPTIONS::AW_OLE_CONTAINER:
		strBaseLineFileName = m_strCWD + '\\' + "olecont.bas";
		break;
	case APPWIZ_OPTIONS::AW_OLE_FULLSERVER:
		strBaseLineFileName = m_strCWD + '\\' + "oleserv.bas";
		break;
	case APPWIZ_OPTIONS::AW_OLE_CONTAINERSERVER:
		strBaseLineFileName = m_strCWD + '\\' + "olec-s.bas";
		break;
	default:
		GetLog()->RecordFailure("Unsupported OLE option detected in CSysClassesTest::Run");
		break;
	}

	GetSubSuite()->m_prj.VerifyClasses(strBaseLineFileName);
}


#ifdef __EDIT__NO__
///////////////////////////////////////////////////////////////////////////////
// CSysEditTest: SYS Edit & Continue test

IMPLEMENT_TEST(CSysEditTest, CTest, "Edit AppWiz project", 2, CSysSubSuite)

void CSysEditTest::Run(void)
{
	if(OpenSource())
	{
		int currentLine=-1;
		if((currentLine=SearchSource()) >0)
		{
			//assume the EXE is open as a project
			EditSourceAndContinue();
		}
	}
}	

BOOL CSysEditTest::OpenSource(void)
{
	CString strSourceFile = CString(PROJNAME) + ".CPP"; 
	return GetLog()->RecordCompare(m_src.Open(strSourceFile, m_strCWD + PROJDIR) == ERROR_SUCCESS, "Opening the source file '%s'.", (LPCSTR)strSourceFile);
}

//returns the current line in the source file if found, -1 otherwise
int CSysEditTest::SearchSource(void)
{
	CString strSearchText = GetLocString(IDSS_ADD_CONSTRUCT_CODE_HERE); 
	GetLog()->RecordCompare(m_src.Find(strSearchText), "Searching for text '%s' in the source file.", (LPCSTR)strSearchText);
		//return -1;
	//else
		return m_src.GetCurrentLine();
}

void CSysEditTest::EditSourceAndContinue(void)
{
	//	TODO : dverma : this test fails due to bug 66005
	if (GetSystem() == SYSTEM_NT_51)
		return;

	GetLog()->RecordInfo("Debugging & editing the source file.");
	// get the project from the subsuite
	COProject* pPrj = &(GetSubSuite()->m_prj);
	// get the debugger component from the subsuite
	CODebug* pDebugger = &(GetSubSuite()->m_dbg);
	// get the breakpoint component from the subsuite
	COBreakpoints* pBP = &(GetSubSuite()->m_bp);

	// this function will be inserted immediately before the constructor.
	CString strFuncName="func";
	CString strFunc = "void " + strFuncName + "(void){}";
	// a call to the function will be inserted within the constructor. 
	CString strFuncCall = strFuncName + "();";

	//m_src.SetCurrentLine(currentline-1);
	MST.DoKeys("{UP}"); //to set the BP at the previous line which is the entry point to the current constructor
	pBP->SetBreakpoint();

	int CurrentLine = m_src.GetCurrentLine();
	CString strline; strline.Format("@%d", CurrentLine);

	GetLog()->RecordCompare(pDebugger->Go()==TRUE && pDebugger->CurrentLineIs(strline)==TRUE,
		"Break at line: %s in EnC test", strline);

	// insert the function.
	m_src.InsertText(strFunc, CurrentLine - 2, 1, TRUE);
	m_src.TypeTextAtCursor("{ENTER}");
	// insert the function call.
	m_src.InsertText("{ENTER}", CurrentLine + 3, 0);
	m_src.TypeTextAtCursor(strFuncCall, TRUE);

	GetLog()->RecordCompare(pDebugger->StepOver(1, NULL, strFuncCall)==TRUE, "Start EnC steping");
	GetLog()->RecordCompare(pDebugger->StepInto(1, strFuncName, strFunc)==TRUE, "Step into new function");
	 //stepout to the last line of ctor (it was empty by default)
	GetLog()->RecordCompare(pDebugger->StepOut(1, NULL, /*line*/"}")==TRUE, "Step out of new function");
	
	pBP->ClearAllBreakpoints();
	pDebugger->StopDebugging();
	//verify the automatic relink of the changes in the project is correct
	pPrj->WaitUntilBuildDone(1); //1 min waiting for relink
	GetLog()->RecordCompare(pPrj->VerifyBuild(TRUE, NULL, NULL, TRUE) == ERROR_SUCCESS, "Verifying the EnC relink.");
}
#endif

///////////////////////////////////////////////////////////////////////////////
// CSysResTest: SYS resource test

IMPLEMENT_TEST(CSysResTest, CTest, "Add Resource to AppWizard Project", 2, CSysSubSuite)

void CSysResTest::Run(void)
{
	OpenResScript();
	AddDialog();
}

BOOL CSysResTest::OpenResScript(void)
{
	BOOL bResult;

	// get the resource script from the subsuite
	COResScript* pRes = &(GetSubSuite()->m_res);

	// set the name of the resource file
	CString strResFile = m_strCWD + PROJDIR + PROJNAME + ".RC";

	GetLog()->RecordInfo("Opening the resource file '%s' for this project...", (LPCSTR)strResFile);

	// open the resource file
	bResult = GetLog()->RecordCompare(pRes->Open(CString(PROJNAME) + ".RC", m_strCWD + PROJDIR) == ERROR_SUCCESS, "Opening the resource file '%s' for this project.", (LPCSTR)strResFile);
	if (!bResult) {
		throw CTestException("CSysResTest::OpenResScript: cannot open resource script; unable to continue.", CTestException::causeOperationFail);
	}
	return bResult;
}

BOOL CSysResTest::AddDialog(void)
{
	// get the resource script from the subsuite
	COResScript* pRes = &(GetSubSuite()->m_res);

	GetLog()->RecordInfo("Creating a dialog resource...");

	// create a dialog resource
	return GetLog()->RecordCompare(pRes->CreateResource(IDSS_RT_DIALOG) == ERROR_SUCCESS, "Creating a dialog resource.");
}

///////////////////////////////////////////////////////////////////////////////
// CSysBuildTest: SYS build test

IMPLEMENT_TEST(CSysBuildTest, CTest, "Build AppWizard project", 3, CSysSubSuite)

BOOL CSysBuildTest::RunAsDependent(void)
{
	// name of the EXE file
	CString strEXEFile = m_strCWD + PROJDIR + PROJNAME + ".EXE";

	// if the EXE file exists, there's no need to run this test as a dependency
	if (_access(strEXEFile, 0) != -1) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

void CSysBuildTest::Run(void)
{
	OpenProject();
	BuildProject();
}

BOOL CSysBuildTest::OpenProject(void)
{
	BOOL bResult;

	// get the project from the subsuite
	COProject* pPrj = &(GetSubSuite()->m_prj);

	// set the project's filename
	CString strProjFile = m_strCWD + PROJDIR + PROJNAME + ".DSP";

	// open the project
	GetLog()->RecordInfo("Opening the project file '%s'...", (LPCSTR)strProjFile);

	// if the project's already open, no need to open it again
	if (pPrj->IsOpen()) {
		GetLog()->RecordSuccess("The project '%s' is already open.", (LPCSTR)(pPrj->GetFullPath()));
		return TRUE;
	}
	else {
		// open the project
		bResult = GetLog()->RecordCompare((pPrj->Open(strProjFile, 0) == ERROR_SUCCESS), "Opening the project file '%s'.", (LPCSTR)strProjFile);
		if (!bResult) {
			throw CTestException("CSysBuildTest::OpenProject: cannot open project; unable to continue.", CTestException::causeOperationFail);
		}
		return bResult;
	}
}

BOOL CSysBuildTest::BuildProject(void)
{
	BOOL bResult;

	// get the project from the subsuite
	COProject* pPrj = &(GetSubSuite()->m_prj);

	COConnection* pConnec;

	GetLog()->RecordInfo("Setting the debug connection for the project...");

	// set the debug connection
	pConnec = GetSubSuite()->GetIDE()->GetConnectionInfo();
	GetLog()->RecordCompare(pConnec->SetAll() == ERROR_SUCCESS, "Setting the debug connection for the project.");

	GetLog()->RecordInfo("Setting the remote path for the project...");
	if (pConnec->GetUserConnections() != CONNECTION_LOCAL ) {
		// set the remote path
		GetLog()->RecordCompare(pPrj->SetRemotePath((CString)PROJNAME + ".Exe") == ERROR_SUCCESS, "Setting the remote path for the project.");
	}
	else {
		GetLog()->RecordSuccess("The project is being built locally; no need to set the remote path.");
	}

	GetLog()->RecordInfo("Building project...");

	// build the project
	GetLog()->RecordCompare(pPrj->Build( 60 ) == ERROR_SUCCESS, "Building project.");

	GetLog()->RecordInfo("Copying the EXE to the remote machine...");
	if ( (GetUserTargetPlatforms() == PLATFORM_WIN32S_X86) ||
		 ((GetUserTargetPlatforms() == PLATFORM_WIN32_X86) &&
		  (pConnec->GetUserConnections() != CONNECTION_LOCAL)) ) {

		// figure out the name of the EXE file
		char acFilename[_MAX_FNAME];
		_splitpath(pPrj->GetFullPath(), NULL, NULL, acFilename, NULL);

		CString strEXEFile = CString(pPrj->GetDir()) + acFilename + ".EXE";

		// copy the EXE
		GetLog()->RecordCompare(pPrj->UpdateRemoteFile(strEXEFile) == ERROR_SUCCESS, "Copying the EXE '%s' to the remote Win32 or Win32s machine.", strEXEFile);
	}
	else {
		GetLog()->RecordSuccess("The EXE is either local or copied by the build process; no need to copy to the remote machine.");
	}

	GetLog()->RecordInfo("Verifying the build...");

	// verify the build
	bResult = GetLog()->RecordCompare(pPrj->VerifyBuild(TRUE, NULL, NULL, TRUE) == ERROR_SUCCESS, "Verifying the build.");
	if (!bResult) {
		throw CTestException("CSysBuildTest::BuildProject: cannot build an AppWizard project; unable to continue.", CTestException::causeOperationFail);
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
// CSysDebugPrjTest: SYS debug test

IMPLEMENT_TEST(CSysDebugPrjTest, CTest, "Debug (and E&C) AppWizard Project", 9, CSysSubSuite)

void CSysDebugPrjTest::Run(void)
{
	OpenProject();
/* ENCCases() might go here
	but link step is broken right now
	so we pushed it down for a while		
end of ENC test*/
	SetBreakpoint();
	RunToBreakpoint();
	StepOut();
	GoToDefinition();
	// TODO (MICHMA): THIS IS THE IDE TIMING PROBLEM
	// StepToCursor();
	// StepOver();
	Restart();
	//VerifyENCRelink() shld go here if ENCCases() above?
	StepIntoMFC();
	StopDebugging();

	// e&c not supported on alpha.
    #if defined(_ALPHA_)
		ClearAllBreakpoints();
	#else
		ENCCases();//TODO move it above
		StopDebugging();
		VerifyENCRelink();//TODO move it above
		ClearAllBreakpoints();
		//must build browse info which was altered but not built by ENC
		GetSubSuite()->m_prj.Build( 60 );
		VerifyENCRelink();
	#endif
}

BOOL CSysDebugPrjTest::OpenProject(void)
{
	BOOL bResult;

	// get the project from the subsuite
	COProject* pPrj = &(GetSubSuite()->m_prj);

	// set the project's filename
	CString strProjFile = m_strCWD + PROJDIR + PROJNAME + ".DSP";

	// open the project
	GetLog()->RecordInfo("Opening the project file '%s'...", (LPCSTR)strProjFile);

	// if the project's already open, no need to open it again
	if (pPrj->IsOpen()) {
		GetLog()->RecordSuccess("The project '%s' is already open.", (LPCSTR)(pPrj->GetFullPath()));
		return TRUE;
	}
	else {
		// open the project
		bResult = GetLog()->RecordCompare((pPrj->Open(strProjFile, 0) == ERROR_SUCCESS), "Opening the project file '%s'.", (LPCSTR)strProjFile);
		if (!bResult) {
			throw CTestException("CSysDebugPrjTest::OpenProject: cannot open project; unable to continue.", CTestException::causeOperationFail);
		}
		return bResult;
	}
}

BOOL CSysDebugPrjTest::SetBreakpoint(void)
{
	BOOL bResult;

	// get the breakpoint component from the subsuite
	COBreakpoints* pBP = &(GetSubSuite()->m_bp);

	// store the breakpoint location
	CString strBPLoc = "CSysTestDoc::CSysTestDoc";

	GetLog()->RecordInfo("Setting a breakpoint on '%s'...", strBPLoc);

	// set the breakpoint
	bResult = GetLog()->RecordCompare(pBP->SetBreakpoint(strBPLoc) != NULL, "Setting a breakpoint on '%s'.", strBPLoc);
	if (!bResult) {
		throw CTestException("CSysDebugPrjTest::SetBreakpoint: cannot set the breakpoint; unable to continue.", CTestException::causeOperationFail);
	}
	return bResult;
}

BOOL CSysDebugPrjTest::RunToBreakpoint(void)
{
	BOOL bResult;

	// get the debugger component from the subsuite
	CODebug* pDebugger = &(GetSubSuite()->m_dbg);

	// store the expected break location
	CString strBreakLoc = "CSysTestDoc::CSysTestDoc";

	GetLog()->RecordInfo("Running until breakpoint at '%s' is hit...", strBreakLoc);

	// run
	bResult = GetLog()->RecordCompare(pDebugger->Go(NULL, NULL, strBreakLoc), "Running until breakpoint at '%s' is hit.", strBreakLoc);
	if (!bResult) {
		throw CTestException("CSysDebugPrjTest::RunToBreakpoint: cannot run to the breakpoint; unable to continue.", CTestException::causeOperationFail);
	}
	return bResult;
}

BOOL CSysDebugPrjTest::StepOut(void)
{
	// get the debugger component from the subsuite
	CODebug* pDebugger = &(GetSubSuite()->m_dbg);

	// store the expected break location
	CString strBreakLoc;
	
	// The following command line switch is mapped as follows:
	//	0 - APPWIZ_OPTIONS::AW_OLE_NONE (Default)
	//  1 - APPWIZ_OPTIONS::AW_OLE_CONTAINER
	//  2 - APPWIZ_OPTIONS::AW_OLE_MINISERVER
	//  3 - APPWIZ_OPTIONS::AW_OLE_FULLSERVER
	//  4 - APPWIZ_OPTIONS::AW_OLE_CONTAINERSERVER
	APPWIZ_OPTIONS::OLESupport ole_choice = (APPWIZ_OPTIONS::OLESupport)CMDLINE->GetIntValue("OLE", 4);

	switch(ole_choice)
	{
	case APPWIZ_OPTIONS::AW_OLE_NONE:
		strBreakLoc = "IMPLEMENT_DYNCREATE(CSysTestDoc, CDocument)";
		break;
	case APPWIZ_OPTIONS::AW_OLE_CONTAINER:
		strBreakLoc = "IMPLEMENT_DYNCREATE(CSysTestDoc, COleDocument)";
		break;
	case APPWIZ_OPTIONS::AW_OLE_FULLSERVER:
	case APPWIZ_OPTIONS::AW_OLE_CONTAINERSERVER:
		strBreakLoc = "IMPLEMENT_DYNCREATE(CSysTestDoc, COleServerDoc)";
		break;
	default:
		GetLog()->RecordFailure("Unsupported OLE option detected in CSysDebugPrjTest::StepOut");
		break;
	}

	if (!strBreakLoc.IsEmpty())
	{
		GetLog()->RecordInfo("Stepping out to '%s'.", strBreakLoc);

		// step out
		return GetLog()->RecordCompare(pDebugger->StepOut( 1, NULL, strBreakLoc), "Stepping out to '%s'.", strBreakLoc);
	}
	else
		return FALSE;
}

BOOL CSysDebugPrjTest::GoToDefinition(void)
{
	BOOL bResult;

	// get the browser from the subsuite
	COBrowse* pBrowser = &(GetSubSuite()->m_brz);

	// store the symbol to browse to
	CString strSymbol = "CSysTestView::OnDraw";

	GetLog()->RecordInfo("Going to the definition of '%s'.", strSymbol);

	// navigate
	bResult = GetLog()->RecordCompare(pBrowser->GotoNthDefinition(1, strSymbol), "Going to the definition of '%s'.", strSymbol);
	if (!bResult) {
		throw CTestException("CSysDebugPrjTest::GoToDefinition: cannot browse to definition; unable to continue.", CTestException::causeOperationFail);
	}
	return bResult;
}

BOOL CSysDebugPrjTest::StepToCursor(void)
{
	// get the debugger component from the subsuite
	CODebug* pDebugger = &(GetSubSuite()->m_dbg);

	GetLog()->RecordInfo("Stepping to the cursor location...");

	// TODO (michma): due to a timing problem in the ide we can't use run-to-cursor.
	// we've decided not to fix the timing problem so we need to figure out how to
	// properly slow-down the test at this point. set bp and go is a work-around.
	COBreakpoints bp;
	bp.SetBreakpoint();
	pDebugger->Go();
	return TRUE;

	// step to cursor
	//return GetLog()->RecordCompare(pDebugger->StepToCursor(), "Stepping to the cursor location.");
}

BOOL CSysDebugPrjTest::StepOver(void)
{
	// get the debugger component from the subsuite
	CODebug* pDebugger = &(GetSubSuite()->m_dbg);

	// store the expected step location
	CString strStepLine = "CSysTestDoc* pDoc = GetDocument();";

	GetLog()->RecordInfo("Stepping (over) until reaching line '%s'.", strStepLine);

	// step over until line
	if (!GetLog()->RecordCompare(pDebugger->StepOverUntilLine(strStepLine), "Stepping (over) until reaching line '%s'.", strStepLine)) {
		return FALSE;
	}

	// store the expected step location
	strStepLine = "ASSERT_VALID(pDoc);";

	GetLog()->RecordInfo("Stepping (over) one line to '%s'.", strStepLine);

	// step over
	return GetLog()->RecordCompare(pDebugger->StepOver(1, NULL, strStepLine), "Stepping (over) one line to '%s'.", strStepLine);
}

BOOL CSysDebugPrjTest::Restart(void)
{
	// get the debugger component from the subsuite
	CODebug* pDebugger = &(GetSubSuite()->m_dbg);

	GetLog()->RecordInfo("Restarting the debug session...");

	// restart
	return GetLog()->RecordCompare(pDebugger->Restart(), "Restarting the debug session.");
}

BOOL CSysDebugPrjTest::StepIntoMFC(void)
{
	// get the debugger component from the subsuite
	CODebug* pDebugger = &(GetSubSuite()->m_dbg);

	GetLog()->RecordInfo("Stepping into MFC...");

	// step into AfxWinMain and verify callstack.
	return GetLog()->RecordCompare(pDebugger->StepIntoUntilFunc("AfxWinMain"), "Stepping into MFC.");
}

BOOL CSysDebugPrjTest::StopDebugging(void)
{
	// get the debugger component from the subsuite
	CODebug* pDebugger = &(GetSubSuite()->m_dbg);

	GetLog()->RecordInfo("Stop debugging the project...");

	// stop debugging
	return GetLog()->RecordCompare(pDebugger->StopDebugging(ASSUME_NORMAL_TERMINATION), "Stop debugging the project.");
}

BOOL CSysDebugPrjTest::ClearAllBreakpoints(void)
{
	// get the breakpoint component from the subsuite
	COBreakpoints* pBP = &(GetSubSuite()->m_bp);

	GetLog()->RecordInfo("Clearing all breakpoints in the project...");

	// clear all breakpoints
	return GetLog()->RecordCompare(pBP->ClearAllBreakpoints(), "Clearing all breakpoints in the project.");
}

BOOL CSysDebugPrjTest::ENCCases(void)
{
	CString strSourceFile = CString(PROJNAME) + ".CPP"; 
	COSource m_src;

	UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC); //must close cause "OK" button in Resedit dialog hampers execution of src.Find()
	if(GetLog()->RecordCompare(m_src.Open(strSourceFile, m_strCWD + PROJDIR) == ERROR_SUCCESS, "Opening the source file '%s'.", (LPCSTR)strSourceFile))
	{
		CString strSearchText = GetLocString(IDSS_ADD_CONSTRUCT_CODE_HERE);
		if(GetLog()->RecordCompare(m_src.Find(strSearchText), "Searching for text '%s' in the source file.", (LPCSTR)strSearchText))
		{
			GetLog()->RecordInfo("Debugging & editing the source file.");
			// get the project from the subsuite
			COProject* pPrj = &(GetSubSuite()->m_prj);
			// get the debugger component from the subsuite
			CODebug* pDebugger = &(GetSubSuite()->m_dbg);
			// get the breakpoint component from the subsuite
			COBreakpoints* pBP = &(GetSubSuite()->m_bp);

			// this function will be inserted immediately before the constructor.
			CString strFuncName="func";
			CString strFunc = "void " + strFuncName + "(void){}";
			// a call to the function will be inserted within the constructor. 
			CString strFuncCall = strFuncName + "();";

			//m_src.SetCurrentLine(currentline-1);
			MST.DoKeyshWnd(UIWB.HWnd(),"{UP}"); //to set the BP at the previous line which is the entry point to the current constructor
			pBP->SetBreakpoint();

			int CurrentLine = m_src.GetCurrentLine();
			CString strline; strline.Format("@%d", CurrentLine);

			GetLog()->RecordCompare(pDebugger->Go()==TRUE && pDebugger->CurrentLineIs(strline)==TRUE,
				"Break at line: %s in EnC test", strline);

			// insert the function.
			m_src.InsertText(strFunc, CurrentLine - 2, 1, TRUE);
			m_src.TypeTextAtCursor("{ENTER}");
			// insert the function call.
			m_src.InsertText("{ENTER}", CurrentLine + 3, 0);
			m_src.TypeTextAtCursor(strFuncCall, TRUE);

			GetLog()->RecordCompare(pDebugger->StepOver(1, NULL, strFuncCall)==TRUE, "Start EnC steping");
			GetLog()->RecordCompare(pDebugger->StepInto(1, strFuncName, strFunc)==TRUE, "Step into new function");
			 //stepout to the last line of ctor (it was empty by default)
			GetLog()->RecordCompare(pDebugger->StepOut(1, NULL, /*line*/"}")==TRUE, "Step out of new function");
	
		}
		else 
			return FALSE;
	}
	else
		return FALSE;
	return TRUE;
}

void CSysDebugPrjTest::VerifyENCRelink(void)
{
	//verify the automatic relink of the changes in the project is correct
	(&GetSubSuite()->m_prj)/*pPrj*/->WaitUntilBuildDone(1); //1 min waiting for relink
	GetLog()->RecordCompare(
		(&GetSubSuite()->m_prj)/*pPrj*/->VerifyBuild(TRUE, NULL, NULL, TRUE) == ERROR_SUCCESS,
		"Verifying the EnC relink.");
}


///////////////////////////////////////////////////////////////////////////////
// CSysDebugExeTest: SYS EXE debug test

IMPLEMENT_TEST(CSysDebugExeTest, CTest, "Debug AppWizard Executable", -1, CSysSubSuite)


void CSysDebugExeTest::Run(void)

{
	// TODO(michma - 4/22/98): 
	// there's some weird timing/focus bug on nt5 where the Debug.Break doesn't get sent to the ide.
	if(GetSystem() & SYSTEM_NT_5)
		return;

	// the support layer objects we will need for this test.
	COProject prj;
	CODebug dbg;
	COBreakpoints bps;
	COStack stk;
	COApplication app;
	COConnection *pConnect = GetSubSuite()->GetIDE()->GetConnectionInfo();

	// we don't support this test over a remote connection yet.
	if(pConnect->GetUserConnections() != CONNECTION_LOCAL)
		return;

	// attach our COProject object to the currently open project. 
	if(!LOG->RecordCompare(prj.Attach() == ERROR_SUCCESS, "attaching a COProject object to project '%s'.", PROJNAME))
		return;

	// execute the project's application.
	if(!LOG->RecordCompare(prj.Execute() == ERROR_SUCCESS, "launching the project's application."))
		return;

	// wait up to 30 seconds for the application to come up. SysTes1 is the name of the default document.
	// we need to look for the document name because the other part of the app title is the same as that used
	// in the ide's title when the project is loaded.
	if(!LOG->RecordCompare(app.Attach("SysTes1", 30), "attaching the debugger to the application."))
		return;

	// attach the debugger to the application's process.
	if(!LOG->RecordCompare(dbg.AttachToProcess(PROJNAME), "attaching the debugger to the application."))
		return;
	
	// break into the process
	if(!LOG->RecordCompare(dbg.Break(), "breaking into the process."))
		return;

	// restart the process.
	if(!LOG->RecordCompare(dbg.Restart(), "restarting the process."))
		return;

	// set a breakpoint on the project's document constructor.
	if(!LOG->RecordCompare(bps.SetBreakpoint("CSysTestDoc::CSysTestDoc") != NULL, "setting a bp on doc constructor."))
		return;

	// run to the breakpoint.
	if(!LOG->RecordCompare(dbg.Go(), "running to the bp."))
		return;

	// verify that the debugger landed in the correct function.
	if(!LOG->RecordCompare(stk.CurrentFunctionIs("CSysTestDoc::CSysTestDoc"), 
		"verifying current function is CSysTestDoc::CSysTestDoc."))
		return;

	// stop the debugger.
	if(!LOG->RecordCompare(dbg.StopDebugging(), "stopping debugging."))
		return;

	// clear all breakpoints.
	if(!LOG->RecordCompare(bps.ClearAllBreakpoints(), "clearing all breakpoints."))
		return;

	// close the project (required if we are looping).
	if(!LOG->RecordCompare(prj.Close() == ERROR_SUCCESS, "closing the project"))
		return;
}
