///////////////////////////////////////////////////////////////////////////////
//	CreateActiveXControlProjectCase.CPP
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Implementation of the CCreateActiveXControlProjectCase class

#include "stdafx.h"
#include "CreateActiveXControlProjectCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CCreateActiveXControlProjectCase, CSystemTestSet, "Create ActiveX Control Project", -1, CCreateActiveXControlProjectSubsuite)

void CCreateActiveXControlProjectCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CCreateActiveXControlProjectCase::Run(void)
{	 
	// set the name and location of the activex control project. we also want the project to be added to
	// the current workspace, and made a subproject of the main app project.
	CProjWizOptions *pCtrlWizOpt = new(CControlWizOptions);
	pCtrlWizOpt->m_strLocation = GetCWD() + m_strWorkspaceLoc;
	pCtrlWizOpt->m_strName = m_strActiveXControlProjName;	
	pCtrlWizOpt->m_pwWorkspace = ADD_TO_CURRENT_WORKSPACE;
	pCtrlWizOpt->m_phHierarchy = SUB_PROJ;
	pCtrlWizOpt->m_strParent = m_strAppProjName; 
	
	// create the activex control project.
	LOG->RecordCompare(prj.New(pCtrlWizOpt) == ERROR_SUCCESS, 
		"Create ActiveX control project named: %s", pCtrlWizOpt->m_strName);
}
