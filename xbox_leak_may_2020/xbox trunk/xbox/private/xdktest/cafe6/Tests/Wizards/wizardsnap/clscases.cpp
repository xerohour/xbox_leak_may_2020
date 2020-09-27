///////////////////////////////////////////////////////////////////////////////
//	CLSCASES.CPP
//
//	Created by :			Date :
//		DavidGa					10/27/93
//
//	Description :
//		Implementation of the CClsTestCases class
//

#include "stdafx.h"
#include "clscases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// flag to indicate to class wizard whether app wizard was successful or not
extern BOOL gbAppWiz;

static char szDir[] = "CW_Test";
static char szDirRes[] = "CW_Test\\res";
static char szAWBase[] = "AW_Base";
static char szAWTest[] = "AW_Test";

IMPLEMENT_TEST(CClsTestCases, CTest, "ClassWizard Tests", -1, CSniffDriver)

void CClsTestCases::Run(void)
{
	// get the current working directory for this test
	::GetCurrentDirectory(MAX_PATH, m_strCWD.GetBuffer(MAX_PATH));
	m_strCWD.ReleaseBuffer();
	m_strCWD += '\\';

	KillAllFiles(m_strCWD + szDirRes);
	KillAllFiles(m_strCWD + szDir);
	CreateDirectory(m_strCWD + szDir, NULL);	// don't know what to do for SECURITY_ATTRIBUTES

	if(gbAppWiz)
		CopyTree(m_strCWD + szAWTest, m_strCWD + szDir);
	else
		CopyTree(m_strCWD + szAWBase, m_strCWD + szDir);

	OpenProject();
	AddClassesAndMessages() ;
	WizardOnProject();
	TabThroughPages();
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

void CClsTestCases::OpenProject(void)
{
	COProject prj;

	// open the project
	if (prj.Open(m_strCWD + szDir + "\\EDIT.MAK") != ERROR_SUCCESS) {
		m_pLog->RecordFailure("Unable to open project (%s).", (LPCTSTR)"edit.mak");
		return;
	}

	UIEditor uprj;
	uprj.AttachActive(); 
	CString str = uprj.GetTitle();
	str.MakeUpper();
	if (str.Find("EDIT") == -1) {
		m_pLog->RecordFailure("Opened project window '%s'", (LPCSTR)str );
	}
}

void CClsTestCases::WizardOnProject(void)
{
	if( UIWB.ClassWizard() == NULL )
		EXPECT_EXEC(FALSE, "Could not launch Class Wizard from menu.  Is it available?");
	else
		m_pLog->RecordInfo("Launched Class Wizard");
}

void CClsTestCases::TabThroughPages(void)
{
	for( int n = 0; n < 6; n++)
	{
		MST.DoKeys("{right}");	// walk through each tab, to at least verify they don't crash
		Sleep(500);
	}
	MST.DoKeys("{escape}");		// dismiss Class Wizard
	if (!UIWB.IsActive()) {
		m_pLog->RecordFailure("Dismissed Class Wizard, and returned focus to MSVC");
	}
}

void CClsTestCases::AddClassesAndMessages(void)
{
 int i, j, k ;
 UIClassWizard	ClsWiz ;
 COProject cop ;

 
 COResScript coRes ;
 
  MST.DoKeys("%(Fl)") ; 
 	for (i = 0; i < 2; i++)
	{
		coRes.CreateResource( /* IDSS_RT_DIALOG */ "Dialog" );
	}
	i = 0;

 MST.DoKeys("%(wl)") ;
 MST.DoKeys("{ENTER}") ;
 ClsWiz.Display() ;
  // Add a couple new classes ;
  for (k = 1; k < 5 ; k++)
 		ClsWiz.AddClass(k) ;
	  ClsWiz.EditCode() ;

   // Add several messages to the classes ;
   	for (k = 1; k < 3; k++)
	   for (j = 1; j < 2; j++) 
	  	  	for (i = 1; i < 2 ; i++)
		 	ClsWiz.AddFunction(k,i,j) ;
	 ClsWiz.EditCode() ;
}
