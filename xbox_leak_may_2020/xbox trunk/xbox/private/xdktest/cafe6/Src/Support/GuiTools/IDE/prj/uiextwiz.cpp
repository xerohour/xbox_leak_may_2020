///////////////////////////////////////////////////////////////////////////////
//  uiextwiz.CPP
//
//  Created by :            Date :
//      Ivanl              10/14/94
//				  
//  Description :
//      Implementation of the UIExtWizard class
//

#include "stdafx.h"
#include "uiextwiz.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\appwz.h"
#include "guiv1.h"
#include "..\shl\uiwbmsg.h"
#include "..\shl\wbutil.h"
#include "uappwiz.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIExtWizard::Create()
// Description: Start the Custom AppWizard sequence from the New Project dialog. The New Project dialog must be displayed before calling this function.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
HWND UIExtWizard::Create() 
{	
	ExpectPage( UIAW_NEWPROJ );

	SetProjType(GetLocString(IDSS_PROJTYPE_CUST_APPWIZ));
    MST.WButtonClick(GetLabel(IDOK));

   	// Wait five seconds for a "Subdir doesn't exist.  Do you really want to create... blah blah blah" message box.
	if (WaitMsgBoxText(GetLocString(IDSS_CREATE_DIR_PROMPT), 5))
		MST.WButtonClick(GetLabel(MSG_YES));	// Go ahead and say it's ok to create it.
	Sleep(2000) ;
	HWND hwnd;
	if (hwnd = MST.WFndWndWait(GetLocString(IDSS_PROJTYPE_CUST_APPWIZ), FW_ALL|FW_NOCASE|FW_PART|FW_FOCUS, 5))
	{
		LOG->Comment("Found '%s'", GetLocString(IDSS_PROJTYPE_CUST_APPWIZ));
		MST.WSetActWnd(hwnd) ;

		RECT rcPage;
		GetClientRect(hwnd, &rcPage);
		ClickMouse( VK_LBUTTON ,hwnd, rcPage.left+50, rcPage.top - 15 );	  // Click on first Tab	(left most)
		ClickMouse( VK_LBUTTON ,hwnd, rcPage.left+50, rcPage.top - 15 );	  // Click on first Tab	(left most)

	}
	else 
  		LOG->Comment("Didn't find %s' dialog.", GetLocString(IDSS_PROJTYPE_CUST_APPWIZ));
  return MST.WGetActWnd(0) ;   
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIExtWizard::SetExtType(int Type)
// Description: Set the type of Custom AppWizard to create: from an existing project, standard AppWizard, or custom.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Type An integer that contains the ID of the radio button that indicates the type of custom AppWizard to create.
// END_HELP_COMMENT
BOOL UIExtWizard::SetExtType(int Type) 
{
	MST.WOptionClick(GetLabel(Type));
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIExtWizard::SetNumPages(int Pages)
// Description: Set the number of steps (pages) that the custom AppWizard will contain.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Pages An integer containing the number of steps (pages).
// END_HELP_COMMENT
BOOL UIExtWizard::SetNumPages(int Pages) 
{					
	CString szPg ;
	szPg.Format("%d",Pages) ;
    MST.WEditSetText(GetLabel(IDC_PAGES), szPg);
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIExtWizard::SetBasePrjName(CString PrjName)
// Description: Set the name of the project that the custom AppWizard will be based from. This is only applicable if the custom AppWizard is being created from an existing project.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: PrjName A CString that contains the path and filename of the project to base this custom AppWizard from.
// END_HELP_COMMENT
BOOL UIExtWizard::SetBasePrjName(CString PrjName) 
{
	MST.WEditText(GetLabel(IDC_PAGES), PrjName);
	return TRUE ;

}

// BEGIN_HELP_COMMENT
// Function: BOOL UIExtWizard::SetDllExeType(int Type)
// Description: Set whether this custom AppWizard is for an EXE or a DLL. This is only applicable if the custom AppWizard is being based on a standard AppWizard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Type An integer that contains the ID of the radio button that indicates whether to create an EXE Custom AppWizard or a DLL Custom AppWizard.
// END_HELP_COMMENT
BOOL UIExtWizard::SetDllExeType(int Type) 
{
	MST.WOptionClick(GetLabel(Type));
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIExtWizard::SetLangs(int index)
// Description: Set the language supported by this custom AppWizard by index. This is only applicable if the custom AppWizard is baesed on a standard AppWizard. (See SetLangs(CString) to set the language by name.)
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: index An integer that contains the 1-based index into the list of languages to set for this custom AppWizard.
// END_HELP_COMMENT
BOOL UIExtWizard::SetLangs(int index) 
{
	CListBox langLst ;
	langLst.Attach(::GetDlgItem(ControlOnPropPage(IDC_LANGS),IDC_LANGS)) ; 
	langLst.SetItemData(index,1) ;	
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIExtWizard::SetLangs(CString LangStr)
// Description: Set the language supported by this custom AppWizard by name. This is only applicable if the custom AppWizard is baesed on a standard AppWizard. (See SetLangs(int) to set the language by index.) This function is NYI.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: LangStr A CString that contains the name of the language to set for this custom AppWizard.
// END_HELP_COMMENT
BOOL UIExtWizard::SetLangs(CString LangStr) 
{
	return TRUE ;
}
