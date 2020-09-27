///////////////////////////////////////////////////////////////////////////////
//	CreateATLCOMServerProjectCase.CPP
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Implementation of the CCreateATLCOMServerProjectCase class

#include "stdafx.h"
#include "CreateATLCOMServerProjectCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CCreateATLCOMServerProjectCase, CSystemTestSet, "Create ATL COM Server Project", -1, CCreateATLCOMServerProjectSubsuite)

void CCreateATLCOMServerProjectCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CCreateATLCOMServerProjectCase::Run(void)
{	 
	// set the name and location of the atl com server project. we also want the project to be added to
	// the current workspace, and made a subproject of the main app project.
	CProjWizOptions *pATLCOMWizOpt = new(CATLCOMWizOptions);
	pATLCOMWizOpt->m_strLocation = GetCWD() + m_strWorkspaceLoc;
	pATLCOMWizOpt->m_strName = m_strATLCOMServerProjName;	
	pATLCOMWizOpt->m_pwWorkspace = ADD_TO_CURRENT_WORKSPACE;
	pATLCOMWizOpt->m_phHierarchy = SUB_PROJ;
	pATLCOMWizOpt->m_strParent = m_strAppProjName; 
	
	// create the activex control project.
	LOG->RecordCompare(prj.New(pATLCOMWizOpt) == ERROR_SUCCESS, 
		"Create ATL COM Server project named: %s", pATLCOMWizOpt->m_strName);
}
