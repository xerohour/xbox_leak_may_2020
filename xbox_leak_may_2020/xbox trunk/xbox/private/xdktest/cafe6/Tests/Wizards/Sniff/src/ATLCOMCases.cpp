///////////////////////////////////////////////////////////////////////////////
//	ATLCOMCases.cpp
//
//	Created by :			Date :
//		MichMa					2/13/98
//
//	Description :
//		Implementation of the CATLCOMCases class

#include "stdafx.h"
#include "ATLCOMCases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CATLCOMCases, CTest, "ATL COM AppWizard", -1, CSniffDriver)


void CATLCOMCases::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}


void CATLCOMCases::Run(void)

{	 
	// the support layer objects we will need for this test.
	COProject prj;

	// the name of the atl com project we will create.
	CString strATLCOMProjName;

	// use a localized name.
	if(GetSystem() & SYSTEM_DBCS)
		strATLCOMProjName = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü ATL COM Project";
	else
		strATLCOMProjName = "ATL COM Project";
	
	// clean-up from a previous run if necessary.
	KillAllFiles(GetCWD() + strATLCOMProjName);

	// set the options for the atl com project (default is to create a top-level project in a new workspace).
	CProjWizOptions *pATLCOMWizOpt = new(CATLCOMWizOptions);
	pATLCOMWizOpt->m_strLocation = GetCWD();
	pATLCOMWizOpt->m_strName = strATLCOMProjName;	
	
	// create the atl com project.
	if(!(LOG->RecordCompare(prj.New(pATLCOMWizOpt) == ERROR_SUCCESS,
		"Create ATL COM project named: %s", pATLCOMWizOpt->m_strName)))
		return;

	// insert a new atl object into the project.
	if(!(LOG->RecordCompare(prj.InsertNewATLObject("ATLCOMInterface") == ERROR_SUCCESS, 
		"Insert new ATL object 'ATLCOMInterface' into '%s' project.", strATLCOMProjName)))
		return;

	// build the project.
	if(!(LOG->RecordCompare(prj.Build() == ERROR_SUCCESS, "Build '%s' project.", strATLCOMProjName)))
		return;
	
	// verify no errors or warnings were generated.
	int iErr, iWarn;
	if(!(LOG->RecordCompare(prj.VerifyBuild(TRUE, &iErr, &iWarn, TRUE) == ERROR_SUCCESS,
		"Verify build of '%s' project", strATLCOMProjName)))
		return;

	// close the project.
	if(!(LOG->RecordCompare(prj.Close() == ERROR_SUCCESS, "Close '%s' project.", strATLCOMProjName)))
		return;
}
