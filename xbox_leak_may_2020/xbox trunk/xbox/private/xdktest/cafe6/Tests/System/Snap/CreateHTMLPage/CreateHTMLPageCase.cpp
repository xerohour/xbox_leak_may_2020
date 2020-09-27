///////////////////////////////////////////////////////////////////////////////
//	CreateHTMLPageCase.CPP
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Implementation of the CCreateHTMLPageCase class

#include "stdafx.h"
#include "CreateHTMLPageCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CCreateHTMLPageCase, CSystemTestSet, "Create HTML Page", -1, CCreateHTMLPageSubsuite)

void CCreateHTMLPageCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CCreateHTMLPageCase::Run(void)
{
	LOG->RecordCompare(html.Create(m_strHTMLPageName, m_strActiveXControlProjName) == ERROR_SUCCESS, 
		"Create HTML page named: %s", m_strHTMLPageName);
}
