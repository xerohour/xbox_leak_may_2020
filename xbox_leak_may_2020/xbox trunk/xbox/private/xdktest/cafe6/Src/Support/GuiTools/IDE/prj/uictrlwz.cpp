///////////////////////////////////////////////////////////////////////////////
//  uiextwiz.CPP
//
//  Created by :            Date :
//      Ivanl              3/16/95
//				  
//  Description :
//      Implementation of the UICtrlWizard class
//

#include "stdafx.h"
#include "uictrlwz.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\appwz.h"
#include "guiv1.h"
#include "uappwiz.h"
#include "..\shl\uiwbmsg.h"
#include "..\shl\wbutil.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: void UICtrlWizard::OnUpdate(void)
// Description: Initializes members of UICtrlWizard object (called by constructor).
// Return: none.
// END_HELP_COMMENT
void UICtrlWizard::OnUpdate(void)
{
	// set the page titles for later use.
	m_strPageTitles[0] = GetLocString(IDSS_PROJTYPE_OLE_CONTROLWIZ);
    m_strPageTitles[0] += GetLocString(IDSS_AW_TITLE_STEP);
	m_strPageTitles[0] += GetLocString(IDSS_AW_TITLE_1);
	m_strPageTitles[0] += GetLocString(IDSS_AW_TITLE_OF);
	m_strPageTitles[0] += GetLocString(IDSS_AW_TITLE_2);
	m_strPageTitles[1] = GetLocString(IDSS_PROJTYPE_OLE_CONTROLWIZ);
    m_strPageTitles[1] += GetLocString(IDSS_AW_TITLE_STEP);
	m_strPageTitles[1] += GetLocString(IDSS_AW_TITLE_2);
	m_strPageTitles[1] += GetLocString(IDSS_AW_TITLE_OF);
	m_strPageTitles[1] += GetLocString(IDSS_AW_TITLE_2);
}


// BEGIN_HELP_COMMENT
// Function: HWND UICtrlWizard::Create()
// Description: Start the ActiveX ControlWizard sequence from the New Project dialog. The New Project dialog must be displayed before calling this function.
// Return: The HWND of the first page of the ActiveX ControlWizard, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UICtrlWizard::Create() 
{	
	ExpectPage( UIAW_NEWPROJ );

	SetProjType(GetLocString(IDSS_PROJTYPE_OLE_CONTROLWIZ));
    MST.WButtonClick(GetLabel(IDOK));

   	// Wait five seconds for a "Subdir doesn't exist.  Do you really want to create... blah blah blah" message box.
	if (WaitMsgBoxText(GetLocString(IDSS_CREATE_DIR_PROMPT), 5))
		MST.WButtonClick(GetLabel(MSG_YES));	// Go ahead and say it's ok to create it.

	// Wait around until the Wizard dialog is displayed.
	
	Sleep(2000) ;
	HWND hwnd;
	if (hwnd = MST.WFndWndWait(GetLocString(IDSS_PROJTYPE_OLE_CONTROLWIZ), FW_ALL|FW_NOCASE|FW_PART|FW_FOCUS, 5))
	{
		LOG->Comment("Found '%s'", GetLocString(IDSS_PROJTYPE_OLE_CONTROLWIZ));
		MST.WSetActWnd(hwnd) ;

		RECT rcPage;
		GetClientRect(hwnd, &rcPage);
		ClickMouse( VK_LBUTTON ,hwnd, rcPage.left+50, rcPage.top - 15 );	  // Click on first Tab	(left most)
		ClickMouse( VK_LBUTTON ,hwnd, rcPage.left+50, rcPage.top - 15 );	  // Click on first Tab	(left most)

	}
	if (WaitForWndWithCtrl(IDC_NUMCONTROLS, 2000))
		LOG->RecordInfo("ControlWizard successfully loaded") ;
	else 
		LOG->RecordFailure("Could not load ControlWizard") ;	
  return MST.WGetActWnd(0) ;   
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlWizard::SetCount(int count)
// Description: Set the number of controls in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: count An integer that contains the number of controls for this project.
// END_HELP_COMMENT
BOOL UICtrlWizard::SetCount(int count) 
{
	CString strCnt;
	strCnt.Format("%d",count) ;
	MST.WEditSetText(GetLabel(IDC_NUMCONTROLS),strCnt);
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlWizard::SetLicenceOpt(int iLic)
// Description: Set whether or not to include a runtime license with the controls in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: iLic A Boolean value that indicates whether to include a runtime license (TRUE) or not.
// END_HELP_COMMENT
BOOL UICtrlWizard::SetLicenceOpt(int iLic) 
{
	if(iLic)
		MST.WOptionClick(GetLabel(IDC_LICENSE));
	else
		MST.WOptionClick(GetLabel(IDC_NOLICENSE));
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlWizard::SetCommentOpt(int icomm)
// Description: Set whether to include source file comments or not in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: icomm A Boolean value that indicates whether to include source file comments (TRUE) or not.
// END_HELP_COMMENT
BOOL UICtrlWizard::SetCommentOpt(int icomm) 
{
	if(icomm)
		MST.WOptionClick(GetLabel(IDC_COMMENTS));
	else
		MST.WOptionClick(GetLabel(IDC_NOCOMMENTS));
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlWizard::SetHelpOpt(int icomm)
// Description: Set whether to include help file support or not in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: icomm A Boolean value that indicates whether to include help file support (TRUE) or not.
// END_HELP_COMMENT
BOOL UICtrlWizard::SetHelpOpt(int icomm) 
{
	if(icomm)
		MST.WOptionClick(GetLabel(IDC_HELPFILES));
	else
		MST.WOptionClick(GetLabel(IDC_NOHELPFILES));
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlWizard::SetActive(int index)
// Description: Set the active control in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: index An integer that contains the 1-based index into the list of controls to be generated by ControlWizard.
// END_HELP_COMMENT
BOOL UICtrlWizard::SetActive(int index) 
{	
	CString Name = "												" ;
	MST.WComboItemText(GetLabel(IDC_CTLCOMBO), index, Name) ;
	MST.WComboSetText(GetLabel(IDC_CTLCOMBO), Name) ;
	m_ActiveCtrl = index ;
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: CString UICtrlWizard::GetActive()
// Description: Get the currently selected control in ControlWizard.
// Return: A CString that contains the name of the currently selected control in ControlWizard.
// END_HELP_COMMENT
CString UICtrlWizard::GetActive() 
{	
	CString Name = "												" ;
	MST.WComboItemText(GetLabel(IDC_CTLCOMBO), m_ActiveCtrl, Name) ;
	return Name ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlWizard::SummaryInfo()
// Description: Display the summary information for the currently selected control in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UICtrlWizard::SummaryInfo() 
{
	SumDlg.Display() ;
	return TRUE ;

}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlWizard::ActiveIfVisible(int active)
// Description: Set the "Activates when visible" property of the currently selected control.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: active A Boolean value that indicates whether to check (TRUE) or uncheck the "Activates when visible" property.
// END_HELP_COMMENT
BOOL UICtrlWizard::ActiveIfVisible(int active)  
{
	if(active)
		MST.WCheckCheck(GetLabel(IDC_ACTIVATEWHENVISIBLE));
	else
		MST.WCheckUnCheck(GetLabel(IDC_ACTIVATEWHENVISIBLE));
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlWizard::Invisible(int opt)
// Description: Set the "Invisible at runtime" property of the currently selected control.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: opt A Boolean value that indicates whether to check (TRUE) or uncheck the "Invisible at runtime" property.
// END_HELP_COMMENT
BOOL UICtrlWizard::Invisible(int opt)  
{
	if(opt)
		MST.WCheckCheck(GetLabel(IDC_INVISIBLEATRUNTIME));
	else
		MST.WCheckUnCheck(GetLabel(IDC_INVISIBLEATRUNTIME));
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlWizard::InInsertDlg(int opt)
// Description: Set the "Available in 'Insert Object' dialog" property of the currently selected control.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: opt A Boolean value that indicates whether to check (TRUE) or uncheck the "Available in 'Insert Object' dialog" property.
// END_HELP_COMMENT
BOOL UICtrlWizard::InInsertDlg(int opt)  
{
	if(opt)
		MST.WCheckCheck(GetLabel(IDC_INSERTABLE));
	else
		MST.WCheckUnCheck(GetLabel(IDC_INSERTABLE));
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlWizard::SimpleFrame(int opt)
// Description: Set the "Supports simple frame" property of the currently selected control.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: opt A Boolean value that indicates whether to check (TRUE) or uncheck the "Supports simple frame" property.
// END_HELP_COMMENT
BOOL UICtrlWizard::SimpleFrame(int opt)  
{
	if(opt)
		MST.WCheckCheck(GetLabel(IDC_SIMPLEFRAME));
	else
		MST.WCheckUnCheck(GetLabel(IDC_SIMPLEFRAME));
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlWizard::SubClassCtrl(int index)
// Description: Select the window class by index that the currently selected control should subclass from in ControlWizard. (See SubClassCtrl(CString) to select the window class by name.)
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: index An integer that contains the 1-based index into the list of window classes to subclass from.
// END_HELP_COMMENT
BOOL UICtrlWizard::SubClassCtrl(int index) 
{
	CString Name = "												" ;
	MST.WComboItemText(GetLabel(IDC_BASECLASS), index, Name) ;
	MST.WComboSetText(GetLabel(IDC_BASECLASS), Name) ;
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlWizard::SubClassCtrl(CString strcls)
// Description: Select the window class by name that the currently selected control should subclass from in ControlWizard. (See SubClassCtrl(int) to select the window class by index.)
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: strcls A CString that contains the name of the window class to subclass from.
// END_HELP_COMMENT
BOOL UICtrlWizard::SubClassCtrl(CString strcls)
{
	MST.WComboSetText(GetLabel(IDC_BASECLASS), strcls) ;
	return TRUE ;
}

// Summary info dialog implementation functions.

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlSummaryDlg::SetShortName(CString Name)
// Description: Set the short name of the control in the Summary Info dialog in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A CString that contains the name.
// END_HELP_COMMENT
int UICtrlSummaryDlg::SetShortName(CString Name) 
{
	MST.WEditSetText(GetLabel(IDC_CTLNAME), Name);	
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlSummaryDlg::SetCrlClsName(CString Name)
// Description: Set the class name of the control in the Summary Info dialog in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A CString that contains the name.
// END_HELP_COMMENT
int UICtrlSummaryDlg::SetCrlClsName(CString Name) 
{
	MST.WEditSetText(GetLabel(IDC_CTLCLASS), Name);
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlSummaryDlg::SetCtrlHName(CString Name)
// Description: Set the header filename of the control in the Summary Info dialog in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A CString that contains the name.
// END_HELP_COMMENT
int UICtrlSummaryDlg::SetCtrlHName(CString Name)
{
	MST.WEditSetText(GetLabel(IDC_CTLIFILE), Name);
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlSummaryDlg::SetCtrlCppName(CString Name)
// Description: Set the source filename of the control in the Summary Info dialog in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A CString that contains the name.
// END_HELP_COMMENT
int UICtrlSummaryDlg::SetCtrlCppName(CString Name)
{
	MST.WEditSetText(GetLabel(IDC_CTLIFILE), Name);
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlSummaryDlg::SetCtrlUsrName(CString Name)
// Description: Set the user type name of the control in the Summary Info dialog in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A CString that contains the name.
// END_HELP_COMMENT
int UICtrlSummaryDlg::SetCtrlUsrName(CString Name)
{
	MST.WEditSetText(GetLabel(IDC_CTLTYPENAME), Name);
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlSummaryDlg::SetCtrlIDName(CString Name)
// Description: Set the programmatic ID name of the control in the Summary Info dialog in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A CString that contains the name.
// END_HELP_COMMENT
int UICtrlSummaryDlg::SetCtrlIDName(CString Name)
{
	MST.WEditSetText(GetLabel(IDC_CTLPROGID), Name);
	return TRUE ;
}

// Prop page functions.	

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlSummaryDlg::SetPpgClsName(CString Name)
// Description: Set the class name of the property page in the Summary Info dialog in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A CString that contains the name.
// END_HELP_COMMENT
int UICtrlSummaryDlg::SetPpgClsName(CString Name)
{
	MST.WEditSetText(GetLabel(IDC_PPGCLASS), Name);
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlSummaryDlg::SetPpgHName(CString Name)
// Description: Set the header filename of the property page in the Summary Info dialog in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A CString that contains the name.
// END_HELP_COMMENT
int UICtrlSummaryDlg::SetPpgHName(CString Name)
{
	MST.WEditSetText(GetLabel(IDC_PPGHFILE), Name);
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlSummaryDlg::SetPpgCppName(CString Name)
// Description: Set the source filename of the property page in the Summary Info dialog in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A CString that contains the name.
// END_HELP_COMMENT
int UICtrlSummaryDlg::SetPpgCppName(CString Name)
{
	MST.WEditSetText(GetLabel(IDC_PPGIFILE), Name);
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlSummaryDlg::SetPpgUsrName(CString Name)
// Description: Set the user type name of the property page in the Summary Info dialog in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A CString that contains the name.
// END_HELP_COMMENT
int UICtrlSummaryDlg::SetPpgUsrName(CString Name)
{
	MST.WEditSetText(GetLabel(IDC_PPGTYPENAME), Name);
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlSummaryDlg::SetPpgIDName(CString Name)
// Description: Set the programmatic ID name of the property page in the Summary Info dialog in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A CString that contains the name.
// END_HELP_COMMENT
int UICtrlSummaryDlg::SetPpgIDName(CString Name) 
{
	MST.WEditSetText(GetLabel(IDC_PPGPROGID), Name);
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICtrlSummaryDlg::Close(int how /*= 1*/)
// Description: Close the Summary Info dialog in ControlWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: how A Boolean value that indicates whether to click on the OK button (TRUE) or the Cancel button when closing the Summary Info dialog.
// END_HELP_COMMENT
int UICtrlSummaryDlg::Close(int how /* =1 */) 
{
	if(how)
		MST.WButtonClick(GetLabel(IDOK));
	else 
		MST.WButtonClick(GetLabel(IDCANCEL));
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: HWND UICtrlSummaryDlg::Display(void)
// Description: Display the Summary Info dialog in ControlWizard. The page with the summary info button must be active in ControlWizard.
// Return: The HWND of the Summary Info dialog, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UICtrlSummaryDlg::Display(void)  
{
	AttachActive();           // Check if is already up
	if (!IsValid())
	{
		MST.WButtonClick(GetLabel(IDC_NAMES)) ;	 
		WaitForWndWithCtrl(IDC_PPGPROGID,1500) ;
		AttachActive();
	}
	return WGetActWnd(0); 
}


// BEGIN_HELP_COMMENT
// Function: CString UICtrlWizard::ExpectedTitle(int page)
// Description: Returns the title of the specified control wizard page.
// Return: A CString containing the title of the specified control wizard page.
// Param: page An int specifying which page you want the title for.
// END_HELP_COMMENT
CString UICtrlWizard::ExpectedTitle(int page)
{
	// wizard only supports 2 pages.
	ASSERT((page >= 1) && (page <= 2));
	// return the title of the specified page.
	return m_strPageTitles[page - 1];
}


