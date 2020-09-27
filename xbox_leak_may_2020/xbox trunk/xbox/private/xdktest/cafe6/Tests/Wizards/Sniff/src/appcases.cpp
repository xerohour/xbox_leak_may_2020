///////////////////////////////////////////////////////////////////////////////
//      APPCASES.CPP
//
//      Created by :                    Date :
//              DavidGa                                 10/27/93 
// 
//      Description :
//              Implementation of the CAppTestCases class
//
 
#include "stdafx.h"
#include "appcases.h"
//#include "..\..\support\guitools\testutil.h"

#define new DEBUG_NEW

#undef THIS_FILE  
static char BASED_CODE THIS_FILE[] = __FILE__;

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

// flag to indicate to class wizard whether app wizard was successful or not
extern BOOL gbAppWiz;

CString msg = "Building proj with " ;
CString LangDllName = "                                            " ;

IMPLEMENT_TEST(CAppTestCases, CTest, "AppWizard Tests", -1, CSniffDriver)

void CAppTestCases::Run(void)
{
	COWorkSpace ws;
	int nFails = 0;
	szDirRes.LoadString(IDS_RESDIRNAME) ; //
	szDir.LoadString(IDS_AWDIRNAME) ;
	ProjDir = szDir ;
	ProjName.LoadString(IDS_PROJNAME) ;
	//HWND hSubWnd ;
 
	iteration = 1 ;
	LangDlls = 0 ;

    //do  //loop for >1 languange dll was causing problems - should eventually fix this instead
	//of just removing it - AG.
	//{
//		COProject cop ; 
//		cop.DeleteFromProjectDir(ProjDir) ;
		//DeleteFromProjectDir uses wrong dir when ran more than once
		//m_prj.DeleteFromProjectDir(ProjDir) ;
		KillAllFiles(ProjDir);
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
			//szDir = ProjDir ;
			Classes();
			ChangeAppType();
			NoAbout();
		}
	
		JumpConfirm();
		CreateProject();
		iteration++ ;
		if (nFails) // If we have had a failure aready, just exist don't build
			return ;
		m_prj.Build(15) ;
		if (!VERIFY_TEST_SUCCESS(m_prj.VerifyBuild())) {
			m_pLog->RecordFailure(msg + LangDllName + "  resources.");
			nFails++;
		}
		
		// Clean up for the next iteration.
		{
			CString num;
			num.Format("%d",iteration) ;
			ProjName +=num ;
			//m_prj.DeleteFromProjectDir(ProjName); //clean up from before
			
		}

	//}
	//while(LangDlls +1 > iteration) ;
	VerifyFiles();

	// indicate appwizard was successful
	gbAppWiz = (nFails == 0);
	ws.CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
//      Test Cases

void CAppTestCases::NewProject(void)
{
//	COProject prj;

// REVIEW(briancr): we can't support AppWizard at the UWBFrame level--it's in COProject
//      m_uaw = UIWB.AppWizard();
//	m_uaw = prj.AppWizard();
	m_uaw = m_prj.AppWizard();
	m_prjwiz.SetProjType(GetLocString(UIAW_PT_APPWIZ));
	if( !m_uaw.IsValid() )
	{
		m_pLog->RecordFailure("Did not launch App Wizard from File/New/Project");
		EXPECT_EXEC(FALSE, "Need App Wiz to test anymore");     // no sense continuing
	}
	m_uaw.SetDir(m_strCWD) ;
	if (m_strCWD.Find(szDir) == -1)
	{
		CreateDirectory(m_strCWD+szDir, NULL);
		m_uaw.SetSubDir(szDir);
	}
	m_uaw.SetName(ProjName);
	m_uaw.Create();
	if (m_uaw.GetPage() != UIAW_APPTYPE) {
		m_pLog->RecordFailure("Invoke App Wizard from New Project dialog");
	}
}

void CAppTestCases::MDIApp(void)
{

    // Get the number of lang DLL
//      LangDlls = ((LangDlls =MST.WListCount(m_uaw.GetLabel(APPWZ_IDC_RSC_LANG))) >1) ? LangDlls + 1: LangDlls ;
	LangDlls = ((LangDlls =MST.WComboCount(m_uaw.GetLabel(APPWZ_IDC_RSC_LANG))) >1) ? 2: LangDlls ;

//      CListBox langLst ;
	CString tmpName('\0',40) ;

	EXPECT( m_uaw.GetPage() == UIAW_APPTYPE );
	m_uaw.SetAppType(UIAW_APP_MDI);

	//MST.WComboItemClk(m_uaw.GetLabel(APPWZ_IDC_RSC_LANG),iteration) ;
	//langLst.Attach(GetDlgItem(ControlOnPropPage(APPWZ_IDC_RSC_LANG),APPWZ_IDC_RSC_LANG)) ; //MST.WFndWndWaitC( m_uaw.GetLabel(APPWZ_IDC_RSC_LANG), "", FW_CASE, 5));

	// Turn on the checkboxes for all language DLLs items except one.
	// NO MULTI-LANGUAGE SUPPORT FOR V3
/*      for (int i = 0; i < LangDlls; i++) 
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
	langLst.Detach() ; */

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
			m_pLog->RecordFailure("All Classes accounted for" );    // 7 if use Recordset
		}
	}
	else {
		if (MST.WListCount(NULL) != 4) {
			m_pLog->RecordFailure( "All Classes accounted for" );   //
		}
	}
			
	CString strView;
	MST.WListItemText("", 1, strView);
	CString ClassName;
	ClassName.LoadString(IDS_CLASSNAME) ;
	m_pLog->RecordInfo("Classname is %s",ClassName);
	m_pLog->RecordInfo("strView is %s",strView);
	if (strView != ClassName) {
		m_pLog->RecordFailure( "View class has illegal name." );
	}

	ClassName.LoadString(IDS_MAINFRAME_CLASS) ;
	m_uaw.SelectClass(ClassName);
	CString strBase = m_uaw.GetBaseClass();
}

void CAppTestCases::ChangeAppType()
{
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
//      m_uaw.SetDlgOptions(UIAW_DLG_NOABOUT);
}

void CAppTestCases::JumpConfirm(void)
{
	if (m_uaw.Finish() != UIAW_CONFIRM) {
		m_pLog->RecordFailure("Finish jumped to confirmation dialog");
	}
}

void CAppTestCases::CreateProject(void)
{
	BOOL bCreated = m_uaw.ConfirmCreate();
	if( bCreated == FALSE )
		m_pLog->RecordFailure("Problem creating EDIT project, or opening it in the IDE");
	else
	{
		CString acTitle ;
		::GetText(GetSubSuite()->GetIDE()->m_hWnd, acTitle.GetBuffer(63),63);
		acTitle.ReleaseBuffer() ;
		acTitle.MakeUpper() ;
		ProjName.MakeUpper() ;
		if( acTitle.Find(ProjName)!= -1 ) {
			m_prj.Attach();
			m_pLog->RecordInfo("Created EDIT project, and opened it in the IDE");
		}
		else
			m_pLog->RecordFailure("Problem creating EDIT project, or opening it in the IDE.  Found '%s'", acTitle);
	}
}

void CAppTestCases::VerifyFiles(void)
{
// REVIEW(briancr): we can't support CloseEditor at the UIWB level, so I've
// moved the code here for now
	UIEditor ued = UIWB.GetActiveEditor();

	if( !ued.IsValid() )
		ued.AttachActive();
	else if( !ued.IsActive() )
		UIWB.SetActiveEditor(ued);
	ued.Close();
	m_pLog->RecordInfo("closing project");
   m_prj.Close() ; // This creates the MDP file wich is needed for the next test.
}
