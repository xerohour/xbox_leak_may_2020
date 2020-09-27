///////////////////////////////////////////////////////////////////////////////
//  UITABDLG.CPP
//
//  Created by :            Date :
//      EnriqueP              1/27/94
//				  
//  Description :
//      Implementation of the UIOptionTabDialog class
//

#include "stdafx.h"
#include "mstwrap.h"
#include "guiv1.h"
#include "..\sym\cmdids.h"
#include "..\sym\vproj.h"
#include "uiwbmsg.h"
#include "..\sym\vshell.h"
#include "uwbframe.h"
#include "newfldlg.h"
#include "..\..\imewrap.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: HWND UINewFileDlg::Display() 
// Description: Bring up the New File tabbed dialog.
// Return: The HWND of the New File tabbed dialog.
// END_HELP_COMMENT
HWND UINewFileDlg::Display() 
{	
	AttachActive();		  // Check if is already up
	if (!IsValid())
	{
	 //MST.DoKeys("%fe") ;
	 UIWB.DoCommand(ID_FILE_NEW, DC_ACCEL);
//	 MST.WFndWnd("New",FW_CASE | FW_FOCUS | FW_HIDDENNOTOK);
	 WaitAttachActive(10000);
	}
	return WGetActWnd(0); 
}

// BEGIN_HELP_COMMENT
// Function: HWND UINewFileDlg::ChangeCompatility() 
// Description: Display the compatibility tab of the options dialog.
// Return: int
// END_HELP_COMMENT

int UINewFileDlg::SetName(CString Name, int DocType /* PROJECTS*/ )
{
	MST.WEditSetText(GetLabel(DocType),Name) ;
	return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: int UINewFileDlg::SetLocation(CString szLocation)
// Description: Sets the path for the new document/project being created.
// Return: ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szLocation a string containing the path
// END_HELP_COMMENT
			
int UINewFileDlg::SetLocation(CString szLocation, int DocType /* PROJECTS*/)
{
	switch (DocType)
	{
	case FILES:
		{
		 DocType = VSHELL_IDC_LOCATIONFORFILE;
		 break ;
		}

	case OTHER:
		{
		 DocType = VSHELL_IDC_LOCATIONFOROTHERFILE;
		 break ;
		}

	case PROJECTS:
		 {
		 DocType = VSHELL_IDC_LOCATIONFORPROJECT;
		 break ;
		 }

	case TEMPLATES:
		{
		 DocType = VSHELL_IDC_LOCATIONFORTEMPLATE;
		 break ;
		}

	case WORKSPACE:
	{
		 DocType = VSHELL_IDC_LOCATIONFORWORKSPACE;
		 break ;
	}
	
	default:
		LOG->RecordInfo("%d : Unknown document type passed to UINewFileDlg::SetLocation()", DocType);
	}
	MST.WEditSetText(GetLabel(DocType), szLocation) ;
	return ERROR_SUCCESS;

}

int UINewFileDlg::SelectFromList(CString ItemName)	 
{
//	MST.DoKeys("{DOWN}") ;
//	UINT ime_mode ;
//	if(IME.IsIMEInUse())					
//	{
//		ime_mode = IME.GetMode();
//		IME.SetMode();
//		IME.Open(FALSE);
//	}

	// we need the TRUE so that ItemName is interpreted literally.
	// this allows for correct selection of names with special
	// ms-test characters, like "MFC AppWizard (exe)", etc.
//	MST.DoKeys(ItemName, TRUE) ;
//	if(IME.IsIMEInUse())					
//	{
//		IME.Open();
//		IME.SetMode(ime_mode);
//	}
//
	MST.WViewSetFocus("@1");
	if(!MST.WViewItemExists("@1", ItemName))
		return FALSE;
	MST.WViewItemClk("@1", ItemName);

	return TRUE ;
}

int UINewFileDlg::SelectFromList(int Index)	 
{
	MST.DoKeys("{DOWN}") ;
//UINT ime_mode = IME.GetMode();
//IME.Open(FALSE);
	MST.WListItemClk("",Index) ;
//IME.Open();
//IME.SetMode(ime_mode);
	return TRUE ;
}


int UINewFileDlg::NewFileTab()
{
	ShowPage(FILES, 5) ;
	return ERROR_SUCCESS ;
}
int UINewFileDlg::NewProjectTab()
{
	ShowPage(PROJECTS, 5) ;
	return ERROR_SUCCESS ;
}

int UINewFileDlg::NewTemplateTab()
{
	ShowPage(TEMPLATES, 5) ;
	return ERROR_SUCCESS ;
}

int UINewFileDlg::NewWorkspaceTab(){
	ShowPage(WORKSPACE, 5) ;
	return ERROR_SUCCESS ;
}

int UINewFileDlg::NewOtherTab()
{
	ShowPage(OTHER, 5) ;
	return ERROR_SUCCESS ;
}

HWND UINewFileDlg::OK()
{
	MST.WButtonClick(GetLabel(IDOK));
	WaitUntilGone(1000);
	return MST.WGetActWnd(0);

}

void UINewFileDlg::SetProjPlatform(CString Platform)
{
	if(MST.WListItemExists(GetLabel(VSHELL_IDC_PLATFORMS ), GetLocString(IDSS_PLATFORM_WIN32)))
	{
		MST.WListItemClk(GetLabel(VSHELL_IDC_PLATFORMS ), GetLocString(IDSS_PLATFORM_WIN32));
		Sleep(500);
		MST.DoKeys(" ");
	}
}


int UINewFileDlg::AddToProject(BOOL bAdd)
{

	CString strAddToProjectCheckBox = GetLocString(IDSS_ADD_TO_PROJECT);
	
	if(!MST.WCheckEnabled(strAddToProjectCheckBox))
	{
		LOG->RecordInfo("ERROR! UINewFileDlg::AddToProject - "
						"Check box '%s' is not enabled", strAddToProjectCheckBox);
		return ERROR_ERROR;

	}

	if(bAdd)
		MST.WCheckCheck(strAddToProjectCheckBox);
	else
		MST.WCheckUnCheck(strAddToProjectCheckBox);

	return ERROR_SUCCESS;
}


int UINewFileDlg::SetProject(LPCSTR szProject)
{

	if(!MST.WComboEnabled("@1"))
	{
		LOG->RecordInfo("ERROR! UINewFileDlg::SetProject - The project combo box is not enabled");
		return ERROR_ERROR;
	}

	if(!MST.WComboItemExists("@1", szProject))
	{
		LOG->RecordInfo("ERROR! UINewFileDlg::SetProject - "
						"Cannot find project '%s' in the project combo box", szProject);
		return ERROR_ERROR;
	}

	MST.WComboItemClk("@1", szProject);
	return ERROR_SUCCESS;
}
