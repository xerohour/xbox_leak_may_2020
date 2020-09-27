///////////////////////////////////////////////////////////////////////////////
//      CTRCASES.CPP
//
//      Created by :                    Date :
//              ivanl                                   3/17/95
//
//      Description :
//              Implementation of the CCtrlWizardCases class
//

#include "stdafx.h"
#include "ctrcases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

extern char* szRes ;
extern CString LangDllName ;
extern CString msg ;
extern HWND hSubWnd ;
  
IMPLEMENT_TEST(CCtrlWizardCases, CTest, "Control Wizard Test", -1, CSniffDriver)

void CCtrlWizardCases::Run(void)
{
	COWorkSpace ws;
	iteration = 1 ;
	LangDlls = 0 ;
  // Flush the strings first
	LangDllName.Empty() ;
	CurrDir.Empty() ;
	msg.Empty() ;
	ProjDir.Empty() ;
	LangDllName =" " ;            

	szDir.LoadString(IDS_CTRWIZDIR) ; 
	ProjName.LoadString(IDS_CTRWIZNAME) ;
	ProjDir = szDir ;             
	
	//prj.DeleteFromProjectDir(ProjDir) ;
	KillAllFiles(ProjDir);
	NewProject() ;
	SetProjOptions() ;
	SetCtrlOptions() ;
	GenerateProject() ;
	AddControlMethods() ;
	ws.CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
//      Test Cases

// Create extension project
void CCtrlWizardCases::NewProject(void)
{

// REVIEW(briancr): we can't support AppWizard at the UWBFrame level--it's in COProject
//      m_ctrlwiz = UIWB.AppWizard();
	prj.ActivateProjWnd(); // We need this up otherwise we can't tell if a project was created or not.
	m_ctrlwiz = prj.AppWizard(TRUE);
	if( !m_ctrlwiz.IsValid() )
	{
		m_pLog->RecordFailure("Did not launch Control AppWizard from File/New/Project");
		EXPECT_EXEC(FALSE, "Need Custom Appwizar to test anymore");     // no sense continuing
	}
	m_ctrlwiz.SetDir(m_strCWD) ;
	CreateDirectory(m_strCWD+szDir, NULL);
	m_ctrlwiz.SetSubDir(szDir);
	m_ctrlwiz.SetName(ProjName);
	m_ctrlwiz.Create();                             
}

void CCtrlWizardCases::SetProjOptions()
{
	m_ctrlwiz.SetCount(3) ;
	m_ctrlwiz.SetLicenceOpt(1);
	m_ctrlwiz.NextPage();
}

//Select the Custom Option
void CCtrlWizardCases::SetCtrlOptions()
{
	CString Name = "Control_" ;

	for(int i = 1; i < 4; i++)
	{
		CString cnt;
		cnt.Format("%d",i) ;
		m_ctrlwiz.SetActive(i) ;
		m_ctrlwiz.SubClassCtrl(i) ;
		m_ctrlwiz.SummaryInfo() ;

		m_ctrlwiz.SumDlg.SetShortName(Name+cnt);
		m_ctrlwiz.SumDlg.Close() ;
	}
}

void CCtrlWizardCases::GenerateProject()
{       
	m_ctrlwiz.NextPage() ;
	if(!m_ctrlwiz.ConfirmCreate())
	{
		m_pLog->RecordFailure(" Problem creating a control project, or opening it in the IDE");
	}
}
void CCtrlWizardCases::AddControlMethods() 
{
	UIClassWizard	ClsWiz ;
/*	ClsWiz.AddStockMethod(1) ;
	ClsWiz.AddCustomMethod(2) ;
    ClsWiz.AddUserMethod(1) ;
*/
}
