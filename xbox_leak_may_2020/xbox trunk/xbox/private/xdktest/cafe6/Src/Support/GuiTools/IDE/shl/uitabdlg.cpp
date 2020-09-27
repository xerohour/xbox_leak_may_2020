///////////////////////////////////////////////////////////////////////////////
//  UITABDLG.CPP
//
//  Created by :            Date :
//      EnriqueP              2/7/94
//				  
//  Description :
//      Implementation of the UITabbedDialog class
//

#include "stdafx.h"
#include "uitabdlg.h"
#include "..\..\testutil.h"
#include "mstwrap.h"
#include "guiv1.h"
#include "..\sym\vproj.h"
#include "Strings.h"
#include "wbutil.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


//////////////////////////////////////////////////////////////////////////////////

/*
BOOL UITabbedDialog::IsValid(void) const
{
	return UIDialog::IsValid();
}
*/

// BEGIN_HELP_COMMENT
// Function: BOOL UITabbedDialog::VerifyClass(void) const
// Description: Verify that the window class of the tabbed dialog indicated by this object is correct.
// Return: TRUE if the window class is correct; FALSE otherwise.
// END_HELP_COMMENT
BOOL UITabbedDialog::VerifyClass(void) const
{
	char acClass[32];
	GetClassName(HWnd(), acClass, 31);
	return (strnicmp(acClass, "Afx:", 4) == 0) &&
			(GetCurrentPage() != NULL);
}

// BEGIN_HELP_COMMENT
// Function: INT UITabbedDialog::SetOption(INT iTab, UINT iCtrlId, UINT iNewValue) 
// Description: Set the control (iCtrlId) on tab (iTab) to the given value (iNewValue). This function is commonly called for objects of classes derived from UITabbedDialog.
// Return: ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iTab The tab containing the control to set.
// Param: iCtrlId The ID of the control to set.
// Param: iNewValue The value to set the control to.
// END_HELP_COMMENT
INT UITabbedDialog::SetOption(INT iTab, UINT iCtrlId, UINT iNewValue) 
{
	Display();
	if (!ShowPage( iTab, 8 ) )
		return ERROR_ERROR ;	// if it wasn't found ;

	HWND hCtl = ::GetDlgItem( GetCurrentPage(), iCtrlId) ;

	if (hCtl == NULL)	// If not found in Page look inside visible category
		hCtl = ::GetDlgItem( GetCurrentMiniPage(), iCtrlId) ;

	if (hCtl == NULL)
	{
		LOG->RecordFailure("Control was not found in this dialog");
		return ERROR_ERROR;
	}

	CString szLabel ;
	
    szLabel = GetLabel(iCtrlId) ;	   // We need the label, to call MSTest APIs.
 
	switch (GetDlgControlClass(hCtl))
	{
		case(GL_CHECKBOX): 
		{
			if(iNewValue)
		 		MST.WCheckCheck(szLabel) ;	
			else 
				MST.WCheckUnCheck(szLabel) ;
		}
		break ;
		case(GL_RADIOBUTTON):
			if(iNewValue)
		 		MST.WOptionClick(szLabel) ;	
		break ;
		case (GL_BUTTON):
		 	if(iNewValue)
				MST.WButtonClick(szLabel) ;
		break ;
		case(GL_COMBOBOX):
			MST.WComboItemClk(szLabel, iNewValue );
		break ;
		case (GL_LISTBOX):
		 	MST.WListItemClk(szLabel, iNewValue );
		break ;
		case (GL_EDIT):
			MST.WEditClick(szLabel) ;	
		break ;

		default:
			LOG->RecordFailure("SetOption() can't set this type of control");
			return ERROR_ERROR ; 
	}

	return ERROR_SUCCESS ; 		   

}



// BEGIN_HELP_COMMENT
// Function: int UITabbedDialog::SetOption(INT iTab, UINT iCtrlId, LPCSTR szNewValue)
// Description: Set the control (iCtrlId) on tab (iTab) to the given value (szNewValue). This function is commonly called for objects of classes derived from UITabbedDialog.
// Return: ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iTab The tab containing the control to set.
// Param: iCtrlId The ID of the control to set.
// Param: szNewValue A pointer to a string containing the value to set the control to.
// END_HELP_COMMENT
int UITabbedDialog::SetOption(INT iTab,UINT iCtrlId, LPCSTR szNewValue)
 {
	Display();
	if (!ShowPage(iTab,8) ) // if it wasn't found ;
		return ERROR_ERROR ;

	HWND hCtl = ::GetDlgItem( GetCurrentPage(), iCtrlId) ;

	if (hCtl == NULL)	// If not found in Page look inside visible category
		hCtl = ::GetDlgItem( GetCurrentMiniPage(), iCtrlId) ;

	if (hCtl == NULL)
	{
		LOG->RecordFailure("Control was not found in this dialog");
		return ERROR_ERROR;
	}

	CString szLabel, szNewText ;		  	
	int iCtrlClass ;
	szNewText = szNewValue ;
	
	szLabel = GetLabel(iCtrlId) ; 	 // We need the label, to call MSTest APIs.

	iCtrlClass = GetDlgControlClass(hCtl) ;
	// Determine the control class type
	switch (GetDlgControlClass(hCtl))
	{
		case(GL_COMBOBOX):
			MST.WComboItemDblClk(szLabel, szNewValue) ;	
		break ;
		case (GL_LISTBOX): 
			MST.WListItemDblClk(szLabel, szNewValue) ;	
		break ;
		case (GL_EDIT):
			MST.WEditSetText( szLabel, szNewText ) ;	
		break ;
  		default:
		return ERROR_ERROR ;
	}

	return ERROR_SUCCESS ; 
}


// BEGIN_HELP_COMMENT
// Function: int UITabbedDialog::GetOption(INT iTab, UINT iCtrlId)
// Description: Get the value of the control (iCtrlId) on tab (iTab). This function is commonly called for objects of classes derived from UITabbedDialog.
// Return: The value of the control, if successful; ERROR_ERROR otherwise.
// Param: iTab The tab containing the control to get.
// Param: iCtrlId The ID of the control to get.
// END_HELP_COMMENT
int UITabbedDialog::GetOption(INT iTab, UINT iCtrlId)
{
	Display();
	if (!ShowPage(iTab,8) ) // if tab wasn't found ;
		return ERROR_ERROR ;

	HWND hCtl = ::GetDlgItem( GetCurrentPage(), iCtrlId) ;

	if (hCtl == NULL)	// If not found in Page look inside visible category
		hCtl = ::GetDlgItem( GetCurrentMiniPage(), iCtrlId) ;

	if (hCtl == NULL)
	{
		LOG->RecordFailure("Control was not found in this dialog");
		return ERROR_ERROR;
	}

    CString szLabel ;
	szLabel = GetLabel(iCtrlId) ; 	   // We need the label, to call MSTest APIs.
	switch (GetDlgControlClass(hCtl))
	{
		case(GL_CHECKBOX): 
			 return MST.WCheckState(szLabel);  // -1 = doesn't exist, 0 = unchecked, 1 = checked, 2 = grayed
		
		case(GL_RADIOBUTTON):
			return MST.WOptionState(szLabel);  // -1 = doesn't exist, 0 = unselected, 1 = selected
		
		case(GL_COMBOBOX):
			return MST.WComboIndex(szLabel);  // returns selection index, returns 0 if there is no text selected in combo box

		case (GL_LISTBOX):
		 	return MST.WListIndex(szLabel);  // returns selection index, returns 0 if there is no text selected in combo box

		default:
			return ERROR_ERROR ; 
	}

}
	
// BEGIN_HELP_COMMENT
// Function: LPCSTR UITabbedDialog::GetOptionStr(INT iTab, UINT iCtrlId)
// Description: Get the value of the control (iCtrlId) on tab (iTab). This function is commonly called for objects of classes derived from UITabbedDialog.
// Return: A pointer to a string containing the value of the control, if successful; NULL otherwise.
// Param: iTab The tab containing the control to get.
// Param: iCtrlId The ID of the control to get.
// END_HELP_COMMENT
LPCSTR UITabbedDialog::GetOptionStr(INT iTab, UINT iCtrlId)
{
	Display();
	if (!ShowPage(iTab,8) ) // if tab wasn't found ;
		return NULL ;
	static char szContents[50];
	
	HWND hCtl = ::GetDlgItem( GetCurrentPage(), iCtrlId) ;

	if (hCtl == NULL)	// If not found in Page look inside visible category
		hCtl = ::GetDlgItem( GetCurrentMiniPage(), iCtrlId) ;

	if (hCtl == NULL)
	{
		LOG->RecordFailure("Control was not found in this dialog");
		return NULL;
	}

    CString szLabel, strContents;		
	szLabel = GetLabel(iCtrlId) ;			// We need the label, to call MSTest APIs.					 
	switch (GetDlgControlClass(hCtl))
	{
		case (GL_EDIT): 
			MST.WEditText(szLabel, strContents);
			break;
		case (GL_COMBOBOX):
			MST.WComboText(szLabel, strContents);
			break;
		case (GL_LISTBOX):
		 	MST.WListText(szLabel, strContents);
			break;		
		default:
			LOG->RecordInfo("GetOptionStr does not work with this type of control");
			return NULL;
	}

	lstrcpy(szContents, strContents.GetBuffer(50));
	strContents.ReleaseBuffer(-1);
	return szContents;
}
	
 
// BEGIN_HELP_COMMENT
// Function: HWND UITabbedDialog::ShowPage(int iCtrlID, int iTabCnt)
// Description: Activate the tab on a tabbed dialog that contains the control given by iCtrlID.
// Return: The HWND of the tab on the tabbed dialog, if successful; NULL otherwise.
// Param: iCtrlID The ID of a control on the desired tab.
// Param: iTabCnt The position number of the tab.
// END_HELP_COMMENT
HWND UITabbedDialog::ShowPage(int iCtrlID, int iTabCnt)
{
	Display();
	if (iCtrlID == TAB_CURRENT)
		return GetCurrentPage();			 

/*	HWND hActive = ControlOnPropPage(iCtrlID) ;
	if((hActive)&& (IsWindowVisible(hActive)))
	   		return GetCurrentPage() ;	 */

	if (IsControlInCurrentPage(iCtrlID))
			return GetCurrentPage();		 // Check if the page happens to be selected
		
	//Make sure that focus is not on some control inside the dialog. 
	//Always first activate the first page, then start the search.
	
	HWND hPage = GetCurrentPage();
	//RECT rcPage;
	//GetClientRect(hPage, &rcPage);
	// REVIEW (michma): need to click twice because as of 5095 clicking
	// only once leaves focus inside page and not on tabs so cursor-left/right
	// and ctrl-pgup/pgdn circulate through page controls and not tabs.
//	ClickMouse( VK_LBUTTON ,hPage, rcPage.left+50, rcPage.top - 15 );	  // Click on first Tab	(left most)
//	ClickMouse( VK_LBUTTON ,hPage, rcPage.left+50, rcPage.top - 15 );	  // Click on first Tab	(left most)
	
	hPage = GetCurrentPage();
	HWND hNextPage = NULL;
	while (	hPage != hNextPage)
	{
		if (IsControlInCurrentPage(iCtrlID))
			return GetCurrentPage();
		
		hNextPage = ActivateNextPage();
	}

	LOG->RecordInfo("UITabbedDialog::ShowPage(%d, %d) - The Page was not found", iCtrlID, iTabCnt);
	return NULL;
	
} 


// BEGIN_HELP_COMMENT
// Function: HWND UITabbedDialog::ShowPage(LPCSTR szTabCaption)
// Description: Activate the tab on a tabbed dialog that has the caption specified by szTabCaption.
// Return: The HWND of the tab on the tabbed dialog, if successful; NULL otherwise.
// Param: szTabCaption A pointer to a string that contains the caption of the tab.
// END_HELP_COMMENT
HWND UITabbedDialog::ShowPage(LPCSTR szTabCaption)
{
	Display();
	CString strCaption = szTabCaption;
	CString strCurCaption;
	if (strCaption.IsEmpty())
		return GetCurrentPage();
	
	HWND hStartPage = GetCurrentPage();
	HWND hPage = GetCurrentPage();
	HWND hNextPage = NULL;

	char szCaption[40];

	while (	hStartPage!= hNextPage)
	{
		hPage = GetCurrentPage();
		GetWindowText(hPage, szCaption, 40);
		strCurCaption = szCaption;
		if (strCurCaption == szTabCaption)
			return GetCurrentPage();
		hNextPage = ActivateNextPage();
	}

	LOG->RecordInfo("UITabbedDialog::ShowPage(\"%s\") - The Page was not found", szTabCaption);
	return NULL;
}

// BEGIN_HELP_COMMENT
// Function: HWND UITabbedDialog::GetCurrentPage() const
// Description: Get the currently displayed tab on a tabbed dialog.
// Return: The HWND of the current tab on the tabbed dialog, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UITabbedDialog::GetCurrentPage() const
{
	HWND hwnd = GetWindow(HWnd(), GW_CHILD);	// Get First window child
	char acClass[32];
	while (hwnd)								// Search among siblings to find dialog page
	{
		::GetClassName(hwnd, acClass, 31);
		if( strcmp(acClass, "#32770") == 0 )
		{
			if ( ::GetWindowLong(hwnd, GWL_STYLE) & WS_VISIBLE)
				return hwnd; 							// Return the page that is visible
		}
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);		
	}
								
	return NULL;  								// No Visible page was found  (who knows why)
}

HWND UITabbedDialog::GetCurrentMiniPage() const
{
	HWND hwnd = GetCurrentPage();	// Get First window child
	hwnd = 	GetWindow(hwnd, GW_CHILD);
	char acClass[32];
	while (hwnd)								// Search among siblings to find dialog page
	{
		::GetClassName(hwnd, acClass, 31);
		if( strcmp(acClass, "#32770") == 0 )
		{
			if ( ::GetWindowLong(hwnd, GWL_STYLE) & WS_VISIBLE)
				return hwnd; 							// Return the page that is visible
		}
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);		
	}
								
	return NULL;  								// No Visible page was found  (who knows why)
}


// BEGIN_HELP_COMMENT
// Function: BOOL UITabbedDialog::IsControlInCurrentPage(UINT iCtrlId) const
// Description: Determine if a control is on the currently displayed tab of a tabbed dialog.
// Return: TRUE if the control is on the current tab; FALSE otherwise.
// Param: iCtrlId The ID of the control to search for.
// END_HELP_COMMENT
BOOL UITabbedDialog::IsControlInCurrentPage(UINT iCtrlId) const
{
	HWND hCtrl = ::GetDlgItem(GetCurrentPage(), iCtrlId); // Look for control

	// REVIEW:  temporary hack!!!!! and with a little luck...
	// debug tab now has categories - like compiler/linker tab.
	// need to create a SetDebugOption()
	if ( (hCtrl == NULL) && (iCtrlId == VPROJ_IDC_REMOTE_TARGET) )
	{	// look inside visible category
		hCtrl = ::GetDlgItem(GetCurrentMiniPage(), iCtrlId);
	}

	if (hCtrl == NULL)
	{
		return FALSE;
	}
	if ( ::GetWindowLong(hCtrl, GWL_STYLE) & WS_VISIBLE)                                
		return TRUE; 							 
	else
	{
		//LOG->RecordInfo("The control is not visible but exists in the current Page");
		return FALSE;
	}		
}


// BEGIN_HELP_COMMENT
// Function: LPCSTR UITabbedDialog::GetLabel(UINT nCtrlId) const
// Description: Get the label for the control specified by nCtrlId. This function is overridden for tabbed dialogs because controls are often contained within subdialogs.
// Return: A pointer to a string containing the label of the control.
// Param: nCtrlId The ID of the control to retrieve the label for.
// END_HELP_COMMENT
LPCSTR UITabbedDialog::GetLabel(UINT nCtrlId) const
{
	HWND hwnd = GetCurrentPage();
	LPCSTR sz = NULL;
	if( hwnd != NULL )
		sz = ::GetLabel(nCtrlId, hwnd, TRUE );		// Find label for control in the current page
	
	if( sz != NULL )
		return sz;
	else
	{
		hwnd = GetCurrentMiniPage();
		if( hwnd != NULL )
			sz = ::GetLabel(nCtrlId, hwnd, TRUE );	// Find label for control in the current category
		if( sz != NULL )
			return sz;
		else
			return ::GetLabel(nCtrlId );		 	// Look in main dialog if it doesn't work
	}		
}


// BEGIN_HELP_COMMENT
// Function: HWND UITabbedDialog::ActivateNextPage( BOOL bRight /* TRUE */)
// Description: Activate the next or previous tab in a tabbed dialog.
// Return: The HWND of the tab on the tabbed dialog, if successful; NULL otherwise.
// Param: bRight TRUE to activate the tab to the right (next); FALSE to activate the tab to the left (previous). (Default is TRUE.)
// END_HELP_COMMENT
HWND UITabbedDialog::ActivateNextPage( BOOL bRight /* TRUE */)
{
	if ( !bRight )
		MST.DoKeys ("^{PGUP}") ;
	else
		MST.DoKeys("^{PGDN}");
	
	Sleep(50)  ;
	
	// if msdn isn't installed, then activating the help system page for the first time
	// brings up a vs help dlg that we need to dismiss.
	if(MST.WFndWndWait(GetLocString(IDSS_VS_HELP_TITLE), FW_DEFAULT, 1))
		MST.DoKeys("{ENTER}");

	return GetCurrentPage(); 
}	 


// BEGIN_HELP_COMMENT
// Function: HWND UITabbedDialog::ShowNextPage(int iCtrlID)
// Description: Display the next tab in a tabbed dialog that contains the control specified by iCtrlID.
// Return: The HWND of the tab on the tabbed dialog, if successful; NULL otherwise.
// Param: iCtrlID The ID of the control that identifies the tab.
// END_HELP_COMMENT
HWND UITabbedDialog::ShowNextPage(int iCtrlID)
{
	// We assume that the tab has the focus, otherwise the arrow key right won't do the right thing
	Display();
	if (iCtrlID == TAB_CURRENT)
		return GetCurrentPage();
			 
	ActivateNextPage();
		
	HWND hPage = GetCurrentPage();
	HWND hNextPage = NULL;
	while (	hPage != hNextPage)
	{
		if (IsControlInCurrentPage(iCtrlID))
			return GetCurrentPage();
		
		hNextPage = ActivateNextPage();
	}

	LOG->RecordInfo("UITabbedDialog::ShowPage(%d) - The Page was not found", iCtrlID);
	return NULL;
}    	  

// BEGIN_HELP_COMMENT
// Function: HWND UITabbedDialog::GetDlgItem(UINT id) const
// Description: Get the HWND of a window (control) on the currently active tab in a tabbed dialog. This function is overridden for tabbed dialogs because windows (controls) are often contained within subdialogs.
// Return: The HWND of the window (control).
// Param: id The ID of the window (control) to find.
// END_HELP_COMMENT
HWND UITabbedDialog::GetDlgItem(UINT id) const
{
	HWND hwnd = UIDialog::GetDlgItem(id);
	if( hwnd != NULL )
		return hwnd;
	else
		return ::GetDlgItem(GetCurrentPage(),id);
}
