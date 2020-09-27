///////////////////////////////////////////////////////////////////////////////
//  UITARGET.CPP
//
//  Created by :            Date :
//      Ivanl              2/7/94
//
//  Description :
//      Implementation of the UIProjectTarget class
//

#include "stdafx.h"
#include "uitarget.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\sym\vproj.h"
#include "guiv1.h"
#include "..\shl\uwbframe.h"
#include "..\shl\wbutil.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: int UIProjectTarget::New(LPCSTR Name, int Type, BOOL Debug)
// Description: Create a new target for the current project.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A pointer to a string that contains name of the new target.
// Param: Type An integer that contains the 1-based index into the list of target types for the new target.
// Param: Debug A Boolean value that indicates whether the new target is debug (true) or retail.
// END_HELP_COMMENT
int UIProjectTarget::New(LPCSTR Name, int Type, BOOL Debug)
{	
	int suc ;
	// If we already have this target, exit.
	if(!(suc = Select(Name, FALSE)))
	{	suc = TRUE ;
	   	CString idx = "@" ;
		char * nb = "                     " ;
		_itoa(Type,nb, 10) ;
		idx +=nb ; 
		MST.WButtonClick(GetLabel(VPROJ_IDC_NEW_TARGET)) ;
		if (UIWB.WaitOnWndWithCtrl(VPROJ_IDC_NEWTARGET_TARGETLIST,3000 ))
		{	
			MST.WEditSetText(GetLabel(VPROJ_IDC_NEWTARGET_NAME),Name) ; 
			if (Type <= MST.WComboCount(GetLabel(VPROJ_IDC_NEWTARGET_PLATFORM))) 		
			{	
				MST.WComboItemClk(GetLabel(VPROJ_IDC_NEWTARGET_PLATFORM),(const char *) idx); //Select type
			}
			if (Debug)
			{
				MST.WCheckCheck(GetLabel(VPROJ_IDC_NEWTARGET_DEBUG)) ; 
			}
			MST.WButtonClick(GetLabel(IDOK)) ;
		}
		else 
			suc = FALSE ; // If New dialog didn't show up
	}
	return suc ;
}

// BEGIN_HELP_COMMENT
// Function: int UIProjectTarget::New(LPCSTR Name, LPCSTR Target)
// Description: Create a new target for the current project.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Name A pointer to a string that contains name of the new target.
// Param: Target A pointer to a string that contains the name of the target type for the new target.
// END_HELP_COMMENT
int UIProjectTarget::New(LPCSTR Name, LPCSTR Target)
{
	int suc ;
	// If we already have this target, exit.
	if(!(suc = Select(Name, FALSE)))
	{	suc = TRUE ;
		MST.WButtonClick(GetLabel(VPROJ_IDC_NEW_TARGET)) ;
		if (!UIWB.WaitOnWndWithCtrl(VPROJ_IDC_NEWTARGET_TARGETLIST,3000 ))
		{	// if new target dialog didn't show up
			suc = FALSE ;
		}
		MST.WEditSetText(GetLabel(VPROJ_IDC_NEWTARGET_NAME),Name) ;
		MST.WCheckCheck(GetLabel(VPROJ_IDC_NEWTARGET_COPY)) ;
		MST.DoKeys("{TAB}") ; //Tab to the combobox
		if(MST.WComboItemExists(NULL,Target))
			MST.WComboItemClk(NULL,Target) ; //couldn't select type.
		else 
			LOG->RecordInfo("Could not find Targets combobox on the New Targets dialog") ;
			MST.WButtonClick(GetLabel(IDOK)) ;
	}
	return suc ;
}

// BEGIN_HELP_COMMENT
// Function: int UIProjectTarget::Delete(LPCSTR DelName) 
// Description: Delete a target from the current project.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: DelName A pointer to a string that contains the name of the target to delete.
// END_HELP_COMMENT
int UIProjectTarget::Delete(LPCSTR DelName) 
{
	BOOL suc ;
	// Select the target
	if ((suc =Select(DelName)))
		if (suc = MST.WButtonExists(GetLabel(VPROJ_IDC_DELETE_TARGET)))
		{ 
			MST.WButtonClick(GetLabel(VPROJ_IDC_DELETE_TARGET));
			suc = FALSE ;
			// Click YES on the confirmation message box
		 	if (UIWB.WaitOnWndWithCtrl(9,3000)) // 9 = YES
			{
				// MST.WButtonClick(GetLabel(9)) ;
				MST.DoKeys("{ENTER}") ; //Take default. 
				//Verify that it was deleted.
				if (!Select(DelName, FALSE))
					suc = TRUE ;
			}
		}
	return suc ;
}

// BEGIN_HELP_COMMENT
// Function: int UIProjectTarget::Rename(LPCSTR OldName, LPCSTR NewName)
// Description: Change the name of one of the target's in the current project.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: OldName A pointer to a string that contains the old name of the target.
// Param: NewName A pointer to a string that contains the new name for the target.
// END_HELP_COMMENT
int UIProjectTarget::Rename(LPCSTR OldName, LPCSTR NewName)
{ 	
  	BOOL suc ;
	// Select the target
	if ((suc =Select(OldName)))
	{	MST.WButtonClick(GetLabel(VPROJ_IDC_CHANGE_TARGET)) ;
		suc = FALSE ;
		// Wait on the Rename dialog box
		if (UIWB.WaitOnWndWithCtrl(VPROJ_IDC_STATIC_OLDTARGET,3000)) 
		{
			MST.WEditSetText(GetLabel(VPROJ_IDC_RENAME_NEWTARGET), NewName) ;
			MST.WButtonClick(GetLabel(IDOK));
			suc = TRUE ;
		}
		
	}
	return suc ;
}

// BEGIN_HELP_COMMENT
// Function: int UIProjectTarget::Select(LPCSTR SelName, BOOL LogWarning/* = TRUE */) 
// Description: Selects a target in the current project (substrings supported).
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: SelName A pointer to a string that contains the name of the target to select.
// Param: LogWarning A Boolean value that indicates whether to log information if the target isn't in the current project (TRUE) or not.
// END_HELP_COMMENT
int UIProjectTarget::Select(LPCSTR SelName, BOOL LogWarning/* = TRUE */) 

	{
	// REVIEW (michma): ms-test routines only allow initiating substring
	// searches of list box items and not terminating substring searches.
	// so in order to find platform-specific substrings (which terminate
	// a target list item), we need to search ourselves.
	CString target;

	// ms-test list box indexes start at 1, not 0.
	for(int i = 1; i <= MST.WListCount(GetLabel(VPROJ_IDC_TARGET_LIST)); i++)
	
		{
		// get the text of the next target.
		MST.WListItemText(GetLabel(VPROJ_IDC_TARGET_LIST), i, target);
		
		// if target contains SelName, click it.
		if(target.Find(SelName) != -1)
			{
			MST.WListItemClk(GetLabel(VPROJ_IDC_TARGET_LIST), i);
			return (MST.WListIndex(GetLabel(VPROJ_IDC_TARGET_LIST)) == i);
			}	
		}

	// SelName wasn't found in any of the targets.
	if(LogWarning) 
		LOG->RecordInfo("WARNING in UIProjectTarget::Select(): "
						"Couldn't Select target: %s", SelName);
  
   	return FALSE;
	}

// BEGIN_HELP_COMMENT
// Function: HWND UIProjectTarget::Show()
// Description: Display the Targets dialog for the current project.
// Return: The HWND of the Targets dialog, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UIProjectTarget::Show()
{
	UIWB.DoCommand(IDM_PROJECT_CONFIGURATIONS,DC_MNEMONIC) ;

	// wait up to 5 seconds for dialog to come up.
	if(!MST.WFndWndWait(GetLocString(IDSS_DLG_TITLE_DEF_PROJECT_CONFIG), FW_DEFAULT, 5))
		
		{
		LOG->RecordInfo("WARNING in UIProjectTarget::Show(): couldn't open "
						"%s Dialog", GetLocString(IDSS_DLG_TITLE_DEF_PROJECT_CONFIG));
		
		return NULL;
		}

	return WGetActWnd(0) ;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIProjectTarget::Close()
// Description: Close the Targets dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
HWND UIProjectTarget::Close()
{
	MST.WButtonClick(GetLabel(IDOK)); 
	int i = 0;

	// wait up to 5 seconds for the dialog to go away.
	while(MST.WFndWndWait("Default Project Configuration", FW_DEFAULT,0))
		
		{
		Sleep(500);
		i++;

		if(i > 9)
			
			{
			LOG->RecordInfo("WARNING in UIProjectTarget::Close(): couldn't close "
							"Default Project Configuration Dialog");
			
			return NULL;
			}
		}

	return (HWND) TRUE ; // REVIEW(Ivanl) return some diagnostic value
}

// BEGIN_HELP_COMMENT
// Function: int UIProjectTarget::SetTarget(LPCSTR Target) 
// Description: Set the current target in the project.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: Target A pointer to a string that contains the name of the target to select.
// END_HELP_COMMENT
int UIProjectTarget::SetTarget(LPCSTR Target) 
{
	int suc ;
	suc =Select(Target) ;
	return suc ;
}

// BEGIN_HELP_COMMENT
// Function: LPCSTR UIProjectTarget::GetTarget(int Index /* = 0*/) 
// Description: Get the name of the current target or a specified in the project.
// Return: A pointer to a string that contains the name of the selected target.
// Param: Index An integer that contains a 1-based index into the list of targets. This value is used to specify the target to retrieve the name for. 0 means the currently selected target. (Default value is 0.)
// END_HELP_COMMENT
LPCSTR UIProjectTarget::GetTarget(int Index /* = 0*/) 
{
	static CString str ;
	str.GetBuffer(128) ;
	str.ReleaseBuffer() ;
	if(!Index)
		Index =  MST.WListIndex(GetLabel(VPROJ_IDC_TARGET_LIST)) ;
	MST.WListItemText(GetLabel(VPROJ_IDC_TARGET_LIST),Index, str) ;
	str.ReleaseBuffer() ;
	return (const char *) str ;
}

