///////////////////////////////////////////////////////////////////////////////
//	InsertActiveXControlIntoHTMLPageCase.CPP
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Implementation of the CInsertActiveXControlIntoHTMLPageCase class

#include "stdafx.h"
#include "InsertActiveXControlIntoHTMLPageCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInsertActiveXControlIntoHTMLPageCase, CSystemTestSet, "Insert ActiveX Control Into HTML Page", -1, CInsertActiveXControlIntoHTMLPageSubsuite)

void CInsertActiveXControlIntoHTMLPageCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CInsertActiveXControlIntoHTMLPageCase::Run(void)
{	 	
	// TODO(michma): need to activate html page and navigate to correct line.
	
	// need to do this so the html object will have a valid editor associated with it,
	// which is required when we save it later.
	html.AttachActive();
	
	// insert the activex control into the html page.
	LOG->RecordCompare(html.InsertActiveXControl(m_strActiveXControlName) == ERROR_SUCCESS, 
		"Insert ActiveX control '%s' into HTML page '%s'", m_strActiveXControlName,  m_strHTMLPageName);

	// close the file for the control that was inserted. this is required for the html to be written to the html file.
	COFile fileCtrl;
	fileCtrl.AttachActive();
	fileCtrl.Close();
	
	// save the change.
	html.Save();
}
