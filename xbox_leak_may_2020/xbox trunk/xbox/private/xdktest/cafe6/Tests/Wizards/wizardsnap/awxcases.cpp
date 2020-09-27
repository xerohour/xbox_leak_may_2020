///////////////////////////////////////////////////////////////////////////////
//	AWXCASES.CPP
//
//	Created by :			Date :
//		Ivanl				10/17/94
//
//	Description :
//		Implementation of the CAwxTestCases class
//
															   
#include "stdafx.h"
#include "awxcases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

extern CString  szDir;
extern char szDirRes[];
extern char* szRes ;
extern CString LangDllName ;
extern CString CurrDir ;
extern CString msg ;
extern CString ProjDir;
extern HWND hSubWnd ;
  
IMPLEMENT_TEST(CAwxTestCases, CTest, "AppWizard Extension Tests", -1, CSniffDriver)

void CAwxTestCases::Run(void)
{
	iteration = 1 ;
	LangDlls = 0 ;

  // Flush the strings first
	LangDllName.Empty() ;
	CurrDir.Empty() ;
	msg.Empty() ;
	ProjDir.Empty() ;

 
   	szDir = "AWX_Test";          
   	LangDllName =" " ;            
   	msg = "Building proj with " ; 
   	ProjDir = szDir ;             
   	
	COProject cop ; 
	cop.DeleteFromProjectDir(ProjDir) ;
	NewProject() ;
	ChooseZapper() ;
	ChooseCustom() ;
	ChooseApwSequence() ;
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

// Create extension project
void CAwxTestCases::NewProject(void)
{
   	m_extwiz = UIWB.AppWizard();
	if( !m_extwiz.IsValid() )
	{
		m_pLog->RecordFailure("Did not launch Custtom AppWizard from File/New/Project");
		EXPECT_EXEC(FALSE, "Need App Wiz to test anymore");	// no sense continuing
	}

	m_extwiz.SetName("Edit");
	m_extwiz.SetSubDir(szDir);
	m_extwiz.Create();				
}

void CAwxTestCases::ChooseZapper()
{
	m_extwiz.SetExtType(IDC_ZAP) ;
	m_extwiz.NextPage();
   	m_extwiz.PrevPage();
}

//Select the Custom Option
void CAwxTestCases::ChooseCustom()
{
	m_extwiz.SetExtType(IDC_CUSTOM) ;
	m_extwiz.SetNumPages(4) ;
	m_extwiz.NextPage();
	m_extwiz.Cancel();
}

	//Select the AppWizard based option
void CAwxTestCases::ChooseApwSequence()
{
	m_extwiz.SetExtType(IDC_SEQUENCE) ;
	m_extwiz.NextPage() ;
	m_extwiz.SetDllExeType(IDC_APWZ_SEQ) ;
	m_extwiz.NextPage();
	if(!(BOOL)m_extwiz.ConfirmCreate())
	{
 		m_pLog->RecordFailure(" Problem creating Extension project, or opening it in the IDE");
	}
}	 		

