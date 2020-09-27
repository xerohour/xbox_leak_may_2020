///////////////////////////////////////////////////////////////////////////////
//	InsertInterfaceIntoATLCOMServerCase.CPP
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Implementation of the CInsertInterfaceIntoATLCOMServerCase class

#include "stdafx.h"
#include "InsertInterfaceIntoATLCOMServerCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInsertInterfaceIntoATLCOMServerCase, CSystemTestSet, "Insert Interface Into ATL COM Server", -1, CInsertInterfaceIntoATLCOMServerSubsuite)

void CInsertInterfaceIntoATLCOMServerCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CInsertInterfaceIntoATLCOMServerCase::Run(void)
{

	// need to do this because each subsuite has its own COProject object.
	prj.Attach();

	// select the ATL COM Server project so we can add a new object to it.
	prj.SetTarget((CString)m_strATLCOMServerProjName + " - " + 
					GetLocString(IDSS_PLATFORM_WIN32) + " " +
					GetLocString(IDSS_BUILD_DEBUG));

	LOG->RecordCompare(prj.InsertNewATLObject(m_strATLCOMServerInterface) == ERROR_SUCCESS, 
		"Insert new ATL object into '%s' project.", m_strATLCOMServerProjName);
}
