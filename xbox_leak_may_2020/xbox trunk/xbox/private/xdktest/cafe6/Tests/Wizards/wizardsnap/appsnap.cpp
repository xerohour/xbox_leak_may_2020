///////////////////////////////////////////////////////////////////////////////
//	APPSNAP.CPP
//
//	Created by :			Date :
//		Ivanl				12/29/94
//
//	Description :
//		Implementation of the CTestAppWizardCases class
//

#include "stdafx.h"
#include "appsnap.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

static CString  szDir = "AWtest";
static char szDirRes[] = "AW_Test\\res";
static char* szRes = szDirRes + 8;
static CString LangDllName =" " ;
static CString InitDir('\0',256) ;
static CString msg = "Building proj with " ;
static CString ProjDir = szDir ;
static CString num ;

static HWND hSubWnd ;

IMPLEMENT_TEST(CTestAppWizardCases, CTest, "AppWizard snap", -1, CSniffDriver)

void CTestAppWizardCases::Run(void)
{
	GetCurrentDirectory(256,InitDir.GetBuffer(256));
	InitDir.ReleaseBuffer() ;
	InitDir+="\\" ;

	iteration = 1 ;
	LangDlls = 0 ;
	LOG->Comment("in appsnap.cpp");
	ProjDir = szDir = "AW_Test";

    do
	{
		iteration++ ;
		num.Format("%d",iteration);

		//ProjDir = szDir +num ;

		COProject cop ; 
		cop.DeleteFromProjectDir(InitDir+ProjDir) ;
		m_pLog->Comment("before NewProject");
	    NewProject();
		m_pLog->Comment("before CreateProject");
		//CreateProject() ;
		cop.Attach();
		/*
		MDIApp();
		if (1 == iteration)
		{
			GetCurrentDirectory(256,InitDir.GetBuffer(256));
			InitDir.ReleaseBuffer() ;
			InitDir+="\\" ;
			InitDir+= szDir ;
			ProjDir = InitDir ;
			szDir = ProjDir ;
			Classes();
			ChangeAppType();
			NoAbout();
		}
		JumpConfirm();
		CreateProject();
		iteration++ ;
		cop.Build(15) ;
	    WriteLog(VERIFY_TEST_SUCCESS(cop.VerifyBuild()),msg + LangDllName + "  resources." ) ;
		LangDllName = " " ;
	}
	while(LangDlls +1 > iteration) ; */
	}
	while(iteration <3) ;

	VerifyFiles();

	return ;
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

void CTestAppWizardCases::NewProject(void)
{
	COProject prj;
	m_utstaw = prj.AppWizard();
	//9/29/97 - the following line was commented out to remove a build error.
	//actually we need to define UIAW_PT_TEST_APPWIZ in order to get the
	//appwizard snap to work.  we aren't currently concerned with this.
	//when we are, we need to add this to a string table.
	//m_utstaw.SetProjType(GetLocString(UIAW_PT_TEST_APPWIZ));
	//m_utstaw.SetProjType(GetLocString(UIAW_PT_APPWIZ));
	//if( !m_utstaw.IsValid() )
	//{
	//	m_pLog->RecordFailure("Did not launch App Wizard from File/New/Project");
	//	EXPECT_EXEC(FALSE, "Need App Wiz to test anymore");	// no sense continuing
	//}
	m_pLog->Comment("in NewProject,before SetDir");
	m_utstaw.SetDir(m_strCWD) ;
	//if (m_strCWD.Find(szDir) == -1)
	//{
	//	CreateDirectory(m_strCWD+szDir, NULL);
	//	m_utstaw.SetSubDir(szDir);
	//}

	m_pLog->Comment("in NewProject,before SetName");
	m_utstaw.SetName(ProjDir);
	//m_pLog->Comment("in NewProject,before SetSubDir");
	//m_utstaw.SetSubDir(ProjDir);
	m_pLog->Comment("in NewProject,before Create");
	m_utstaw.Create();
	m_pLog->Comment("in NewProject,after Create");
	//if (m_utstaw.GetPage() != UIAW_APPTYPE) 
	//{
	//	m_pLog->RecordFailure("Invoke App Wizard from New Project dialog");
	//}

	m_pLog->Comment("in NewProject,before SetIteration");
	//m_utstaw.SetIteration(num) ;
	m_pLog->Comment("in NewProject,before nextpage");
	m_utstaw.NextPage() ;
	m_pLog->Comment("in NewProject,before gotopage");
	m_utstaw.GoToPage(APPWZ_IDC_OUTPUT);
	m_pLog->Comment("in NewProject,before finish");
	m_utstaw.Finish() ;
	m_pLog->Comment("in NewProject,at end of it");
  	//m_utstaw.ConfirmCreate();

					
}

void CTestAppWizardCases::MDIApp(void)
{

    // Get the number of lang DLL
 /*	LangDlls = ((LangDlls =MST.WListCount(m_uaw.GetLabel(APPWZ_IDC_RSC_LANG))) >1) ? LangDlls + 1: LangDlls ;
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

	
	m_uaw.NextPage();		// UIAW_FEATURES
	m_uaw.NextPage();		// UIAW_PROJOPTIONS
	m_uaw.NextPage();		// UIAW_CLASSES
	
	WriteLog( m_uaw.GetPage() == UIAW_CLASSES, "App Wizard MDI pages work" );
	} */
}

void CTestAppWizardCases::Classes()
{
/*	EXPECT( m_uaw.GetPage() == UIAW_CLASSES );

    if (m_uaw.OleInstalled)
		WriteLog( WListCount(NULL) == 6, "All Classes accounted for" );	// 7 if use Recordset
	else
		WriteLog( WListCount(NULL) == 4, "All Classes accounted for" );	//
		 	
	CString strView;
	MST.WListItemText("", 4, strView);
	WriteLog( strView == "CMyEditView", "Catches illegal names" );

	m_uaw.SelectClass("CMainFrame");
	CString strBase = m_uaw.GetBaseClass();
//	WriteLog( strBase == "CFrameWnd", "OLE Mini-Server forced SDI");	// Sanchovy bug #1066
*/
}

void CTestAppWizardCases::ChangeAppType()
{
/*	m_uaw.PrevPage();		// Proj Options
	m_uaw.PrevPage();		// Features
	m_uaw.PrevPage();		// OLE Options
	m_uaw.PrevPage();		// DB Options
	EXPECT_EXEC( m_uaw.PrevPage() == UIAW_APPTYPE, "Couldn't get back to App Type page");

	m_uaw.SetAppType(UIAW_APP_FORM);
	WriteLog(m_uaw.NextPage() == UIAW_DLGOPTIONS, "Form-based app uses different steps"); 
	*/
}

void CTestAppWizardCases::NoAbout()
{
//	EXPECT( m_uaw.GetPage() == UIAW_DLGOPTIONS );
//	m_uaw.SetDlgOptions(UIAW_DLG_NOABOUT);
}

void CTestAppWizardCases::JumpConfirm(void)
{
//	WriteLog(m_uaw.Finish() == UIAW_CONFIRM, "Finish jumped to confirmation dialog");
}

void CTestAppWizardCases::CreateProject(void)
{
	if( m_utstaw.ConfirmCreate() == FALSE )
		m_pLog->RecordFailure("Problem creating EDIT project, or opening it in the IDE");
	else
	{
		//char _acTitle[64];
		CString acTitle ;
		::GetText(GetSubSuite()->GetIDE()->m_hWnd, acTitle.GetBuffer(63),63);
		acTitle.ReleaseBuffer() ;
		acTitle.MakeUpper() ;
		ProjDir.MakeUpper() ;
		if( acTitle.Find(ProjDir)!= -1 )
			m_pLog->RecordInfo("Created EDIT project, and opened it in the IDE");
		else
			m_pLog->RecordFailure("Problem creating EDIT project, or opening it in the IDE.  Found '%s'", acTitle + " Expeted  " + ProjDir);
	}
}

void CTestAppWizardCases::VerifyFiles(void)
{
	UIEditor ued = UIWB.GetActiveEditor();

	if( !ued.IsValid() )
		ued.AttachActive();
	else if( !ued.IsActive() )
		UIWB.SetActiveEditor(ued);
	ued.Close();
}
