///////////////////////////////////////////////////////////////////////////////
//	UTSTAPWZ.CPP
//
//	Created by :			Date :
//		Ivanl				12/29/94
//
//	Description :
//		Implementation of the UITestAppWizard class. 
//		This class corresponds to the Testing AppWizard, a custom appwizard used for snap tests.
													  
#include "stdafx.h"
#include "utstapwz.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\appwz.h"
#include "guiv1.h"
#include "..\shl\uiwbmsg.h"
#include "..\shl\wbutil.h"
 
#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void UITestAppWizard::OnUpdate()
{
	UIProjectWizard::OnUpdate() ;
	Initialize() ;
//	m_pgArr.RemoveAll() ;
} 

void UITestAppWizard::Initialize()
{

  	pg0 =IDC_ITERATION, pg1 =APPWZ_IDC_RSC_LANG, pg2 = UIAW_DB_NONE, pg3 = APPWZ_IDC_NO_INPLACE ; 
    pg4 =APPWZ_IDC_PONOTDLL , pg5 =APPWZ_IDCD_POPRINT , pg6 =APPWZ_IDC_CLASS_LIST, confirm  = APPWZ_IDC_OUTPUT ;
	
	m_CurrPage.SetAt(pg0, "Custom dialog for the test appwizard") ;
	m_CurrPage.SetAt(pg1, "Comments/External/MFC libs options") ;
	m_CurrPage.SetAt(pg2, "OLE options") ;
	m_CurrPage.SetAt(pg3, "Database options") ;
	m_CurrPage.SetAt(pg4, "Project type") ;
	m_CurrPage.SetAt(pg5, "Project features") ;
	m_CurrPage.SetAt(pg6, "Classes list") ;
	m_CurrPage.SetAt(confirm, "Confirm dialog") ;

	m_pgArr.Add(&pg0) ;
	m_pgArr.Add(&pg1) ;
	m_pgArr.Add(&pg2) ;
	m_pgArr.Add(&pg3) ;
	m_pgArr.Add(&pg4) ;
	m_pgArr.Add(&pg5) ;
	m_pgArr.Add(&pg6) ;
	m_pgArr.Add(&confirm) ;
}

void UITestAppWizard::SetIteration(CString iteration)
{
	MST.WEditSetText(GetLabel(IDC_ITERATION),iteration) ;
};

CString UITestAppWizard::GetIteration()
{
	CString iteration ;
	MST.WListText(GetLabel(IDC_ITERATION),iteration) ;
	return iteration ;
} ;

HWND UITestAppWizard::Create() 
{	
	ExpectPage( UIAW_NEWPROJ );

    //BOOL bAppWiz = MST.WComboItemClk(GetLabel(APPWZ_IDC_PTCOMBO), "extwiz.dll");
	SetProjType("Testing AppWizard");
    MST.WButtonClick(GetLabel(IDOK));

   	// Wait five seconds for a "Subdir doesn't exist.  Do you really want to create... blah blah blah" message box.
	if (WaitMsgBoxText(GetLocString(IDSS_CREATE_DIR_PROMPT), 5))
		MST.WButtonClick(GetLabel(MSG_YES));	// Go ahead and say it's ok to create it.

	// Wait around until the Wizard dialog is displayed.
	if (WaitForWndWithCtrl(IDC_ITERATION, 2000))
		LOG->RecordInfo("Custom AppWizard successfully loaded") ;
	else 
		LOG->RecordFailure("Could not load Custom AppWizard") ;	
  return MST.WGetActWnd(0) ;   
}

