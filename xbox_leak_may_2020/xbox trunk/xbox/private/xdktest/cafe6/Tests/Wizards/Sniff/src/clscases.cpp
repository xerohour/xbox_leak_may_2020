///////////////////////////////////////////////////////////////////////////////
//      CLSCASES.CPP
//
//      Created by :                    Date :
//              DavidGa                                 10/27/93
//
//      Description :
//              Implementation of the CClsTestCases class
//

#include "stdafx.h"
#include "clscases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// flag to indicate to class wizard whether app wizard was successful or not
extern BOOL gbAppWiz;

//static char szAWTest[] = "AW_Test";

IMPLEMENT_TEST(CClsTestCases, CTest, "ClassWizard tests", -1, CSniffDriver)

void CClsTestCases::Run(void)
{
	// get the current working directory for this test
	::GetCurrentDirectory(MAX_PATH, m_strCWD.GetBuffer(MAX_PATH));
	m_strCWD.ReleaseBuffer();
	m_strCWD += '\\';
	
	strAWBase.LoadString(IDS_CLWBASEDIR) ;  //AW_base
	szDir.LoadString(IDS_CLWDIRNAME) ; //CW_Test
	szAWproj.LoadString(IDS_AWDIRNAME) ; //AW Test
	ProjName.LoadString(IDS_PROJNAME) ; //Edit Test
	KillAllFiles(szDir);//(m_strCWD + szDir); //.....\idesniff\wizards\CW_Test
	// don't know what to do for SECURITY_ATTRIBUTES
	CreateDirectory(m_strCWD + szDir, NULL);        

	if(gbAppWiz) 
		//copy ...\idesniff\wizards\AW Test\Edit Test to ...\idesniff\wizards\CW_Test
		CopyTree(m_strCWD + szAWproj+"\\"+ProjName, m_strCWD + szDir);
	else
	{
		m_pLog->RecordInfo("running from aw_base");
		//copy ...\idesniff\wizards\AW_base to ...\idesniff\wizards\CW_Test
		CopyTree(m_strCWD + strAWBase, m_strCWD + szDir);
		ProjName = "Edit" ;
	}
	szDir+="\\" ;

	OpenProject();
	AddClassesAndMessages() ;
	WizardOnProject();
	TabThroughPages();
}

///////////////////////////////////////////////////////////////////////////////
//      Test Cases

void CClsTestCases::OpenProject(void)
{
	COProject prj;

	// open the project
	if (prj.Open(m_strCWD + szDir + ProjName+ ".DSW") != ERROR_SUCCESS) {
		m_pLog->RecordFailure("Unable to open project (%s).", ProjName+ ".dsw");
		return;
	}

	CString str ;
    ::GetText(GetSubSuite()->GetIDE()->m_hWnd, str.GetBuffer(63),63);
    str.ReleaseBuffer() ;
	ProjName.MakeUpper() ;
	str.MakeUpper();
	if (str.Find(ProjName) == -1) {
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
		MST.DoKeys ("^({TAB})") ;  // walk through each tab, to at least verify they don't crash

		Sleep(500);
	}
	MST.DoKeys("{escape}");         // dismiss Class Wizard
	if (!UIWB.IsActive()) {
		m_pLog->RecordFailure("Dismissed Class Wizard, and returned focus to MSVC");
	}
	// REVIEW replace with a function in the shell.
	UIWB.SaveAll(3000) ;
	//MST.DoKeys("%f") ;
	//MST.DoKeys("l") ; 
	UIWB.CloseAllWindows();
	//MST.DoKeys("%w") ;
	//MST.DoKeys("l") ;

}

void CClsTestCases::AddClassesAndMessages(void)
{
 int i, j, k ;
 UIClassWizard  ClsWiz ;
 COProject cop ;

 
 COResScript coRes ;
 
  //MST.DoKeys("%(Fl)") ; 
  UIWB.SaveAll(3000) ;
  for (i = 0; i < 2; i++)
  {
	coRes.CreateResource(  IDSS_RT_DIALOG /*"Dialog" */);
	UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);
  }
  i = 0;
 
 //MST.DoKeys("%Wl") ;
 //MST.DoKeys("(^{F4 2})");
 //UIWB.CloseAllWindows();
 //MST.DoKeys("{ENTER}") ;
 Sleep(3000);
 ClsWiz.Display() ;
 if (ClsWiz.AddClass(1) == ERROR_SUCCESS)
 {
  // Add a couple new classes ;
  for (k = 2; k < 5 ; k++)
		ClsWiz.AddClass(k) ;
	  ClsWiz.EditCode() ;

   // Add several messages to the classes ;
	for (k = 1; k < 3; k++)
	   for (j = 1; j < 2; j++) 
			for (i = 1; i < 2 ; i++)
			ClsWiz.AddFunction(k,i,j) ;
	 ClsWiz.AddFunction(1,2,2) ; // Add one handler which is not virtual.
	 ClsWiz.EditCode() ;

	 // Add several messages using the wizardbar
	/* HWND hnd = MST.WGetActWnd(0) ;
	 CString strhnd ;
	 strhnd.Format("%d",hnd) ;
	
	 AfxMessageBox("Testing the active handle >>> " + strhnd) ;
	 hnd = GetParent(hnd) ;

     strhnd.Format("%d",hnd) ;
	 AfxMessageBox("Testing the active parent >>> " + strhnd) ;

	 UIWizardBar WizBar ;
	 for(i =1 ; k < 6; k++)
	 {
		WizBar.SelectClass(i) ;
		WizBar.HandleMessage(k) ;
	 } 
	 */
 }
 else
 {
	 m_pLog->RecordFailure("Couldn't add new class.");
	 DoKeys("{ESCAPE 2}");
 }
}
