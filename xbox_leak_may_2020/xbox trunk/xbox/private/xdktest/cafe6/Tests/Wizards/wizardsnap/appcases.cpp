///////////////////////////////////////////////////////////////////////////////
//	APPCASES.CPP
//
//	Created by :			Date :
//		DavidGa					10/27/93 
// 
//	Description :
//		Implementation of the CAppTestCases class
//

#include "stdafx.h"
#include "appcases.h"

#undef THIS_FILE 
static char BASED_CODE THIS_FILE[] = __FILE__;

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

// flag to indicate to class wizard whether app wizard was successful or not
extern BOOL gbAppWiz;

CString  szDir = "AW_Test";
char szDirRes[] = "AW_Test\\res";
char* szRes = szDirRes + 8;
CString LangDllName = "                                            " ;
CString CurrDir('\0',256) ;
CString msg = "Building proj with " ;
CString ProjDir = szDir ;
HWND hSubWnd ;

IMPLEMENT_TEST(CAppTestCases, CTest, "AppWizard tests", -1, CSniffDriver)

void CAppTestCases::Run(void)
{
	int nFails = 0;

	iteration = 1 ;
	LangDlls = 0 ;
	ProjDir = szDir = "AW_Test";
	
    do
	{
		COProject cop ; 
		cop.DeleteFromProjectDir(ProjDir) ;
	    NewProject();
		MDIApp();
		if (1 == iteration)
		{  // Flush the stings first.
			CurrDir.Empty() ;
			LangDllName.Empty() ;

			GetCurrentDirectory(256,CurrDir.GetBuffer(256));
			CurrDir.ReleaseBuffer() ;
			CurrDir+="\\" ;
			CurrDir+= szDir ;
			ProjDir = CurrDir ;
			szDir = ProjDir ;
			Classes();
			ChangeAppType();
			NoAbout();
		}
		JumpConfirm();
		CreateProject();
		iteration++ ;
		cop.Build(15) ;
		if (!VERIFY_TEST_SUCCESS(cop.VerifyBuild())) {
			m_pLog->RecordFailure(msg + LangDllName + "  resources.");
			nFails++;
		}
	}
	while(LangDlls +1 > iteration) ;
	VerifyFiles();

	// indicate appwizard was successful
	gbAppWiz = (nFails == 0);
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

void CAppTestCases::NewProject(void)
{
	m_uaw = UIWB.AppWizard();
	if( !m_uaw.IsValid() )
	{
		m_pLog->RecordFailure("Did not launch App Wizard from File/New/Project");
		EXPECT_EXEC(FALSE, "Need App Wiz to test anymore");	// no sense continuing
	}
	m_uaw.SetName("Edit");
	m_uaw.SetSubDir(szDir);
	m_uaw.Create();
	if (m_uaw.GetPage() != UIAW_APPTYPE) {
		m_pLog->RecordFailure("Invoke App Wizard from New Project dialog");
	}
}

void CAppTestCases::MDIApp(void)
{

    // Get the number of lang DLL
// 	LangDlls = ((LangDlls =MST.WListCount(m_uaw.GetLabel(APPWZ_IDC_RSC_LANG))) >1) ? LangDlls + 1: LangDlls ;
 	LangDlls = ((LangDlls =MST.WListCount(m_uaw.GetLabel(APPWZ_IDC_RSC_LANG))) >1) ? 2: LangDlls ;

   	CListBox langLst ;
	CString tmpName('\0',40) ;

	EXPECT( m_uaw.GetPage() == UIAW_APPTYPE );
	m_uaw.SetAppType(UIAW_APP_MDI);

	MST.WListItemClk(m_uaw.GetLabel(APPWZ_IDC_RSC_LANG),iteration) ;
	langLst.Attach(GetDlgItem(ControlOnPropPage(APPWZ_IDC_RSC_LANG),APPWZ_IDC_RSC_LANG)) ; //MST.WFndWndWaitC( m_uaw.GetLabel(APPWZ_IDC_RSC_LANG), "", FW_CASE, 5));

	// Turn on the checkboxes for all language DLLs items except one.
	for (int i = 0; i < LangDlls; i++)
	{
	 	if(iteration == i+1)
	 		langLst.SetItemData(i,0) ;	
		else
		{
			MST.WListItemText(m_uaw.GetLabel(APPWZ_IDC_RSC_LANG),i+1,tmpName) ;
			LangDllName = LangDllName + ", "+ tmpName ;
			langLst.SetItemData(i,1) ;
		}
	}
	if(LangDlls == 1) // If there is only one language DLL, select it.
	{
		langLst.SetItemData(iteration -1, 1) ;
		MST.WListItemText(m_uaw.GetLabel(APPWZ_IDC_RSC_LANG),iteration,tmpName) ;
	}
	langLst.SetCurSel(iteration -1) ;
	langLst.Detach() ;

	if (1 == iteration)
	{
		EXPECT_EXEC( m_uaw.NextPage() == UIAW_DBOPTIONS, "Page after App Type was not Database Options" );
		m_uaw.SetDBOptions(UIAW_DB_HEADER);
	
		EXPECT_EXEC( m_uaw.NextPage() == UIAW_OLEOPTIONS, "Page after Database Options was not OLE Options" );
		m_uaw.SetOLEOptions(UIAW_OLE_MINI_SERVER);
		m_uaw.SetOLEOptions(UIAW_OLE_YES_AUTO);
	
		m_uaw.GoToPage(APPWZ_IDC_DB_RADIO); 
		m_uaw.GoToPage(APPWZ_IDC_RSC_LANG);
		m_uaw.GoToPage(APPWZ_IDC_PONOTDLL);	
	
		m_uaw.GoToPage(APPWZ_IDC_OUTPUT);
		m_uaw.GoToPage(APPWZ_IDC_NO_INPLACE) ; 
		m_uaw.GoToPage(APPWZ_IDCD_POPRINT) ;
		m_uaw.GoToPage(APPWZ_IDC_RSC_LANG);
	 	m_uaw.GoToPage(APPWZ_IDC_CLASS_LIST); 
 
		if (m_uaw.GetPage() != UIAW_CLASSES)
		{
			m_pLog->RecordFailure("App Wizard MDI pages work" );
		}
    }
}

void CAppTestCases::Classes()
{
	EXPECT( m_uaw.GetPage() == UIAW_CLASSES );

    if (m_uaw.OleInstalled) {
		if (MST.WListCount(NULL) != 6) {
			m_pLog->RecordFailure("All Classes accounted for" );	// 7 if use Recordset
		}
	}
	else {
		if (MST.WListCount(NULL) != 4) {
			m_pLog->RecordFailure( "All Classes accounted for" );	//
		}
	}
		 	
	CString strView;
	MST.WListItemText("", 4, strView);
	if (strView != "CMyEditView") {
		m_pLog->RecordFailure( "Catches illegal names" );
	}

	m_uaw.SelectClass("CMainFrame");
	CString strBase = m_uaw.GetBaseClass();
/*
	if (strBase != "CFrameWnd") {
		m_pLog->RecordFailure( "OLE Mini-Server forced SDI");	// Sanchovy bug #1066
	}
*/
}

void CAppTestCases::ChangeAppType()
{
	/*m_uaw.PrevPage();		// Proj Options
	m_uaw.PrevPage();		// Features
	m_uaw.PrevPage();		// OLE Options
	m_uaw.PrevPage();		// DB Options */
	m_uaw.GoToPage(APPWZ_IDC_DB_RADIO); // DB Options
	EXPECT_EXEC( m_uaw.PrevPage() == UIAW_APPTYPE, "Couldn't get back to App Type page");

	m_uaw.SetAppType(UIAW_APP_FORM);
	if (m_uaw.NextPage() != UIAW_DLGOPTIONS) {
		m_pLog->RecordFailure("Form-based app uses different steps");
	}
}

void CAppTestCases::NoAbout()
{
	EXPECT( m_uaw.GetPage() == UIAW_DLGOPTIONS );
//	m_uaw.SetDlgOptions(UIAW_DLG_NOABOUT);
}

void CAppTestCases::JumpConfirm(void)
{
	if (m_uaw.Finish() != UIAW_CONFIRM) {
		m_pLog->RecordFailure("Finish jumped to confirmation dialog");
	}
}

void CAppTestCases::CreateProject(void)
{
	HWND hwnd = m_uaw.ConfirmCreate();
	if( hwnd == NULL )
		m_pLog->RecordFailure("Problem creating EDIT project, or opening it in the IDE");
	else
	{
		//char _acTitle[64];
		CString acTitle ;
		::GetText(hwnd, acTitle.GetBuffer(63),63);
		acTitle.ReleaseBuffer() ;
		acTitle.MakeUpper() ;
		if( acTitle.Find("EDIT")!= -1 )
			m_pLog->RecordInfo("Created EDIT project, and opened it in the IDE");
		else
			m_pLog->RecordFailure("Problem creating EDIT project, or opening it in the IDE.  Found '%s'", acTitle);
	}
}

void CAppTestCases::VerifyFiles(void)
{
	UIWB.CloseEditor();
}
