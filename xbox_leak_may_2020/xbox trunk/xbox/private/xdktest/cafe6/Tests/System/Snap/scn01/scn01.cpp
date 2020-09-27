///////////////////////////////////////////////////////////////////////////////
//	scn01.CPP
//
//	Created by :			Date :
//		YefimS					5/19/97
//
//	Description :
//		Implementation of the CSystemScenario01Test class

#include "stdafx.h"
#include "scn01.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CCreateAppWizAppTest, CSystemTestSet, "CreateAppWizApp", -1, CCreateBuildDebugAppWizAppSubSuite)

void CCreateAppWizAppTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CCreateAppWizAppTest

void CCreateAppWizAppTest::Run(void)
{	 
	XSAFETY;
	EXPECT_TRUE( CreateAppWizApp() );
	XSAFETY;

}


BOOL CCreateAppWizAppTest::CreateAppWizApp( void )
{
	LogTestHeader("Create AppWiz App with OLE and Context sensitive Help");

	m_strProjLoc  = m_strCWD + m_strAppsRoot + "\\" + m_strWorkspaceName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strAppProjName + "\\";

	RemoveTestFiles( m_strProjLoc );

/*	if (!VERIFY_TEST_SUCCESS( prj.NewAppWiz( m_strAppProjName, m_strProjLoc, PLATFORMS)))
	{
 		m_pLog->RecordFailure("Could not create an Application project");
		return FALSE;
	}
*/

	BOOL bResult;
	
	// get the project from the subsuite
	COProject* pPrj = &(GetSubSuite()->m_prj);

	// create a new AppWizard project
	APPWIZ_OPTIONS options;
//	options.m_bUseMFCDLL = CMDLINE->GetBooleanValue("MFCDLL", TRUE); 
	options.m_bUseMFCDLL = TRUE; 

	// The following command line switch is mapped as follows:
	//	0 - APPWIZ_OPTIONS::AW_OLE_NONE (Default)
	//  1 - APPWIZ_OPTIONS::AW_OLE_CONTAINER
	//  2 - APPWIZ_OPTIONS::AW_OLE_MINISERVER (not supported)
	//  3 - APPWIZ_OPTIONS::AW_OLE_FULLSERVER
	//  4 - APPWIZ_OPTIONS::AW_OLE_CONTAINERSERVER
	options.m_OLECompound = (APPWIZ_OPTIONS::OLESupport)4;
//	options.m_OLECompound = (APPWIZ_OPTIONS::OLESupport)CMDLINE->GetIntValue("OLE", 4);
//	ASSERT(options.m_OLECompound < 5);

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

	options.m_bUseMFCDLL = TRUE; 
	options.m_OLEAuto = 1; 
	options.m_Help = 1; 
	
	::CreateDirectory(m_strCWD + m_strAppsRoot, NULL); //create just in case it does not exist

	bResult = GetLog()->RecordCompare(pPrj->NewAppWiz(m_strAppProjName, m_strCWD + m_strAppsRoot + "\\", PLATFORMS, &options) == ERROR_SUCCESS, "Creating an AppWizard project named '%s'.", m_strAppProjName);
	if (!bResult) {
		throw CTestException("CCreateAppWizAppTest::CreateAppWizApp: cannot create a new AppWizard project; unable to continue.", CTestException::causeOperationFail);
	}

	// turn on browse info file (bsc) generation (off by default).
	bResult = GetLog()->RecordCompare(pPrj->ToggleBSCGeneration(TOGGLE_ON) == ERROR_SUCCESS, "Turning on BSC Generation.", m_strAppProjName);
	if (!bResult) {
		throw CTestException("CCreateAppWizAppTest::CreateAppWizApp: cannot turn on BSC generation; unable to continue.", CTestException::causeOperationFail);
	}

	// change output file location to same as project file (avoid debug, macdbg, pmcdbg, etc).
	bResult = GetLog()->RecordCompare(pPrj->SetOutputFile((CString)m_strAppProjName + ".Exe") == ERROR_SUCCESS, "Resetting output file location.", m_strAppProjName);
	if (!bResult) {
		throw CTestException("CCreateAppWizAppTest::CreateAppWizApp: cannot reset output file location; unable to continue.", CTestException::causeOperationFail);
	}

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
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("SaveProject");
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Save()));

	return TRUE;
}



IMPLEMENT_TEST(CAddDialogTest, CSystemTestSet, "AddDialog", -1, CCreateBuildDebugAppWizAppSubSuite)

void CAddDialogTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CAddDialogTest

void CAddDialogTest::Run(void)
{	// ToDo: strings should be replaced by member variables 
	XSAFETY;
	EXPECT_TRUE( AddMenu("Test", "TestDialog") );
	XSAFETY;
	EXPECT_TRUE( CreateMenuItemMsgHandler("Test", "TestDialog") );
	XSAFETY;
	EXPECT_TRUE( AddDialog("TestDialog", "IDD_TESTDIALOG") );
	XSAFETY;
	EXPECT_TRUE( CreateTestDialogClass("TestDialog") );
	XSAFETY;
	EXPECT_TRUE( ModifyCodeForDialog("TestDialog") );
	XSAFETY;
	EXPECT_TRUE( Build() );
	XSAFETY;

}


BOOL CAddDialogTest::AddDialog( LPCSTR szDialogCaption, LPCSTR szDialogID, LPCSTR szDefaultDialogID /* IDD_DIALOG1 */)
{
	LogTestHeader("Create a Dialog resource, specify the Caption and ID");

	// create a dialog resource
	if (!VERIFY_TEST_SUCCESS( res.CreateResource(IDSS_RT_DIALOG) ))
	{
		m_pLog->RecordFailure("Could not create a dialog resource.");
		return FALSE;
	}
	LogSubTestHeader("SUCCESS: res.CreateResource(IDSS_RT_DIALOG)");

	UIResEditor resEditor;

	if (!VERIFY_TEST_SUCCESS( res.OpenResource(IDSS_RT_DIALOG, szDefaultDialogID, resEditor)))
	{
		m_pLog->RecordFailure("Could not open the dialog resource");
		return FALSE;
	}
	LogSubTestHeader("SUCCESS: res.OpenResource(IDSS_RT_DIALOG, szDefaultDialogID, resEditor)");

	if (!VERIFY_TEST_SUCCESS( res.ChangeDialogCaption( szDialogCaption, resEditor)))
	{
		m_pLog->RecordFailure("Could not change the dialog caption");
		return FALSE;
	}
	LogSubTestHeader("SUCCESS: res.ChangeDialogCaption( szDialogCaption, resEditor)");

	if (!VERIFY_TEST_SUCCESS( res.ChangeDialogID( szDialogID, szDefaultDialogID)))
	{
		m_pLog->RecordFailure("Could not change the dialog caption");
		return FALSE;
	}
	LogSubTestHeader("SUCCESS: res.ChangeDialogID( szDialogID, szDefaultDialogID)");

	UIWorkspaceWindow wnd;
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( res.OpenResource(IDSS_RT_DIALOG, szDialogID, resEditor)));
	EXPECT_TRUE( UIWB.SaveFileAs(m_strAppProjName + ".rc") );
	resEditor.Close();

	return TRUE;
}

BOOL CAddDialogTest::AddMenu( LPCSTR szMenuName, LPCSTR szMenuItem )
{
	LogTestHeader("Add a Menu and a menu item");

//	m_strProjName = APPNAME; // "appwizapp" //ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + m_strAppsRoot + "\\" + m_strAppProjName + "\\";
//	m_strMenuName = MENUNAME;
//	m_strMenuItem = MENUITEM;

	CString strMenuRes = (CString)"IDR_APPWIZTYPE" ;   //ToDo: will replace by m_strAppResourceID
	UIResEditor resEditor;

	if (!VERIFY_TEST_SUCCESS( res.OpenResource(IDSS_RT_MENU, strMenuRes, resEditor)))
	{
		m_pLog->RecordFailure("Could not open the menu resource");
		return FALSE;
	}

	if (!VERIFY_TEST_SUCCESS( res.AddMenu( szMenuName, szMenuItem, resEditor)))
	{
		m_pLog->RecordFailure("Could not add menu");
		return FALSE;
	}

	EXPECT_TRUE( UIWB.SaveFileAs(m_strAppProjName + ".rc") );
	resEditor.Close();

	return TRUE;
}

BOOL CAddDialogTest::CreateMenuItemMsgHandler( LPCSTR szMenuName, LPCSTR szMenuItem )
{
	LogTestHeader("CreateMenuItemMsgHandler");

// Open appwizapp.cpp
	UIWB.OpenFile( m_strProjLoc + m_strAppProjName + ".cpp" );
// Open res editor: it should be opened and frontmost in order to get all IDs in ClassWizard dialog
	CString strMenuRes = (CString)"IDR_APPWIZTYPE" ;   //ToDo: will replace by m_strAppResourceID
	UIResEditor resEditor;

	if (!VERIFY_TEST_SUCCESS( res.OpenResource(IDSS_RT_MENU, strMenuRes, resEditor)))
	{
		m_pLog->RecordFailure("Could not open the menu resource");
		return FALSE;
	}

// Add function for ID_TEST_TESTDIALOG, command
	if (!VERIFY_TEST_SUCCESS( uiclswiz.AddFunction(2,1,"ID_Test_TestDialog")))
	{
		m_pLog->RecordFailure("Could not add a function to class");
		return FALSE;
	}

// Click EditCode
	if (!VERIFY_TEST_SUCCESS( uiclswiz.EditCode()))
	{
		m_pLog->RecordFailure("Could not edit code");
		return FALSE;
	}

	return TRUE;
}

BOOL CAddDialogTest::CreateTestDialogClass( LPCSTR szDialogName )
{
	LogTestHeader("CreateTestDialogClass");

// Create CTestDialog Class	derived from CDialog
	if (!VERIFY_TEST_SUCCESS( uiclswiz.AddClass2
		("CDialog", szDialogName)))
	{
		m_pLog->RecordFailure("Could not add a dialog class");
		return FALSE;
	}
	UIWB.OpenFile( m_strProjLoc + szDialogName + ".cpp" );

	return TRUE;
}

BOOL CAddDialogTest::ModifyCodeForDialog( LPCSTR szDialogName )
{
	LogTestHeader("ModifyCodeForDialog");

// Open appwizapp.cpp
	UIWB.OpenFile( m_strProjLoc + m_strAppProjName + ".cpp" );
// Add #include "testdialog.h"
	src.Find( m_strAppProjName + "View.h" );
	int nLine = src.GetCurrentLine();
//	src.InsertText("#include \"testdialog.h\"", nLine+1, 1);  //Doesn't work
//	src.SetCursor(nLine+1, 1);	//Doesn't work
	MST.DoKeys("{Down}{Home}");
	MST.DoKeys("#include \"testdialog.h\"{Enter}", FALSE);

// Add code to OnTestDialog in appwizapp.cpp
	src.Find( "::OnTestTestDialog" );
	nLine = src.GetCurrentLine();
//	src.InsertText("TestDialog testDlg;\\n     testDlg.DoModal();\\n", nLine+3, 5);	  //Doesn't work
	MST.DoKeys("{Down 3}{Home}");
	MST.DoKeys("    TestDialog testDlg;{Enter}testDlg.DoModal{(}{)};{Enter}", FALSE);

	return TRUE;
}


BOOL CAddDialogTest::Build( void )
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
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("SaveProject");
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Save()));

	return TRUE;
}


