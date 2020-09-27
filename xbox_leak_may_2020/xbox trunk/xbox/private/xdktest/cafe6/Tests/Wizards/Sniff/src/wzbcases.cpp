///////////////////////////////////////////////////////////////////////////////
//      wzbcases.CPP
//
//      Created by :                    Date :
//              Anita George         9/3/96		copied from clscases.cpp & modified
//
//      Description :
//              Implementation of the CWzbTestCases class
//

#include "stdafx.h"
#include "wzbcases.h"
#include "support.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// flag to indicate to class wizard whether app wizard was successful or not
extern BOOL gbAppWiz;

static char szAWTest[] = "AW_Test";

IMPLEMENT_TEST(CWzbTestCases, CTest, "WizardBar tests", -1, CSniffDriver)

void CWzbTestCases::Run(void)
{
	COWorkSpace ws;
	// get the current working directory for this test
	::GetCurrentDirectory(MAX_PATH, m_strCWD.GetBuffer(MAX_PATH));
	m_strCWD.ReleaseBuffer();
	m_strCWD += '\\';
	
	szWBproj.LoadString(IDS_WIZBDIRNAME) ;
	ProjName.LoadString(IDS_WIZBAR_PROJECT) ;
	KillAllFiles(m_strCWD + szWBproj + "\\" + ProjName + "\\");

	szDir = m_strCWD + szWBproj + "\\";

	//if new project created successfully
	if (CreateProject())
	{
		//if wizard bar gets displayed successfully
		if (ShowWizBar())
		{
			UIWizardBar WizBar;
			ClassName.LoadString(IDS_MAINFRAME_CLASS) ;
			CodeString.LoadString(IDS_CLASS_DEFINITION) ;
			WizBar.GotoClassDefinition(ClassName, NULL, CodeString);
			//AddMsgHandler()
		}

		//AddClassesAndMessages() ;
		//WizardOnProject();
		//TabThroughPages();
	}
	ws.CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
//      Test Cases

BOOL CWzbTestCases::CreateProject()
{
	COProject proj1;
	
	UIAppWizard uaw = proj1.AppWizard();
	m_prjwiz.SetProjType(GetLocString(UIAW_PT_APPWIZ));
	if( !uaw.IsValid() )
	{
		m_pLog->RecordFailure("Did not launch App Wizard from File/New/Project");
		EXPECT_EXEC(FALSE, "Can't continue with WizBar sniff");	// no sense continuing
	}

	uaw.SetDir(szDir);
	uaw.SetName(ProjName);
	uaw.Create();
	EXPECT( uaw.GetPage() == UIAW_APPTYPE );
	uaw.SetAppType(UIAW_APP_MDI);
	if (uaw.Finish() != UIAW_CONFIRM) 
	{
		m_pLog->Comment("Problem in confirmation dialog");
	}

	BOOL bCreated = uaw.ConfirmCreate();
	if( bCreated == NULL )
	{
		m_pLog->RecordFailure("Problem creating project in the IDE");
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


BOOL CWzbTestCases::ShowWizBar(void)
{
	UIWizardBar WizBar;
	//if can't show wizbar then error
	if (!WS.ShowToolbar(IDTB_CLASSVIEW_WIZBARU, TRUE) )
	{
		m_pLog->RecordFailure("Couldn't Show Wizard bar");
		return FALSE;
	}
	//if wizbar displayed, then set it up correctly
	else
	{
		WizBar.InitializeWizBar();
		m_pLog->RecordInfo("Done setting focus to combo");
	}
	return TRUE;
}