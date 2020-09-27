///////////////////////////////////////////////////////////////////////////////
//  UPROJWIZ.CPP
//
//  Created by :            Date :
//      Ivanl                 10/14/94
//
//  Description :
//      Implementation of the UIProjectWizard class
//
													  
#include "stdafx.h"
#include "uprojwiz.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\appwz.h"
#include "..\sym\vproj.h"
#include "..\shl\ucommdlg.h"
#include "..\shl\uwbframe.h"
#include "..\shl\wbutil.h"
#include "..\shl\uiwbmsg.h"
#include "uappwiz.h"
#include "guitarg.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void UIProjectWizard::OnUpdate(void)
{
	UIDialog::OnUpdate();
    m_nPage = GetPage();
}

WORD UIProjectWizard::GetCurrentPage()
{
	//Go through the array and find which control exists on the active page. 
	for(int i = 0; i < m_pgArr.GetSize(); i++)
	{
		WORD * ctlId ;
		ctlId = (WORD *)m_pgArr.GetAt(i) ;
		if (ControlOnPropPage(* ctlId))
			return *ctlId ;	 // Returns the ID which uniquely identifys the active page.
	} 
	return NULL ;
}


WORD UIProjectWizard::GoToPage(int ID) 
{
 	// Find the page we are on
 	WORD GoNext = 0;
	WORD wActive = GetCurrentPage() ;
	//if we are sitting at the control we want, just return
	if (ID == wActive)
		return wActive ;
	// Find the relationship between the wanted and active pages
 	for(int i =0; i < m_pgArr.GetSize(); i++)
	{	
		if (*((WORD *)(m_pgArr.GetAt(i))) == ID)
		{											     
			GoNext = 0 ;
			break ; 
		}
		if (*((WORD *)(m_pgArr.GetAt(i))) == wActive)
		{	
			GoNext = 1 ;
			break ; 
		}
	}
	// If the active page is before the one we want, move forward, else move backwards.
	if (GoNext) 
	{
		do
			NextPage() ;
		while(GetCurrentPage() != ID) ; 			
	} 
	else
	{
		do
			PrevPage() ;
		while(GetCurrentPage() != ID);			
	}
   return GetCurrentPage() ;
}

BOOL UIProjectWizard::IsValid() 
{
	if(!WaitForWndWithCtrl(VSHELL_IDC_PROJECTTYPES,1500))
		return FALSE ;
	return TRUE ;

} 

int UIProjectWizard::GetPage(HWND hwnd /*=NULL*/)
{

    if( hwnd == NULL )
        hwnd = WGetActWnd(0);
    if( !::IsWindow(hwnd) )
        return UIAW_INVALID;

	return -1 ; //Place holder.
 }

int UIProjectWizard::UpdatePage(void)
{
    AttachActive();
    return m_nPage;
}

void UIProjectWizard::ExpectPage(int nPage)
{
}

CString UIProjectWizard::ExpectedTitle(int nPage) const
{
  return CString(""); // Place holder  
}

int UIProjectWizard::NextPage(void)
{
	HWND hmsg;
    MST.WButtonClick(::GetLabel(IDOK));
	 // If prompted about an GUID already exists, or any other error message, say YES.
    if( (hmsg=WaitForWndWithCtrl(8,500)) || (hmsg=WaitForWndWithCtrl(6,500)) )
		MST.DoKeyshWnd(hmsg,"{ENTER}") ;

    return UpdatePage();
}
int UIProjectWizard::NextPage(int ctrlID)
{
	HWND hmsg;
    MST.WButtonClick(::GetLabel(IDOK));
	// This NextPage member function expects that a dialog will arise
	//with a control with the passed ID that needs pressing
    if (hmsg=WaitForWndWithCtrl(ctrlID,500)) 
		MST.DoKeyshWnd(hmsg,"{ENTER}") ;

    return UpdatePage();
}

int UIProjectWizard::PrevPage(void)
{
	if( GetCurrentPage() ==  APPWZ_IDC_OUTPUT)
	    MST.DoKeyshWnd(HWnd(),"{ESC}") ;
	else
		MST.WButtonClick(::GetLabel(APPWZ_IDC_BACK));
    return UpdatePage();
}

int UIProjectWizard::Finish(void)
{
	HWND hmsg;
    MST.WButtonClick(GetLocString(IDSS_PROJWIZ_FINISH));
	 // If prompted about an GUID already exists, or any other error message, say YES.
    if( (hmsg=WaitForWndWithCtrl(6,500)) || (hmsg=WaitForWndWithCtrl(8,500)) )
		MST.DoKeyshWnd(hmsg,"{ENTER}") ;

    return UpdatePage();
}
int UIProjectWizard::Finish(int ctrlID)
{
	HWND hmsg;
    MST.WButtonClick(GetLocString(IDSS_PROJWIZ_FINISH));
	// This Finish member function expects that a dialog will arise
	//with a control with the passed ID that needs pressing
    if (hmsg=WaitForWndWithCtrl(ctrlID,500))
		MST.DoKeyshWnd(hmsg,"{ENTER}") ;

    return UpdatePage();
}

HWND UIProjectWizard::Cancel(void)
{
	HWND hwnd = UIDialog::Cancel();
    UpdatePage();
	return hwnd;
}

HWND UIProjectWizard::Create(void)
{
   MST.WButtonClick(GetLabel(IDOK));

   	// Wait five seconds for a "Subdir doesn't exist.  Do you really want to create... blah blah blah" message box.
	if (WaitMsgBoxText(GetLocString(IDSS_CREATE_DIR_PROMPT), 5))
		MST.WButtonClick(GetLabel(MSG_YES));	// Go ahead and say it's ok to create it.
    DWORD dwProcessId ;

    GetWindowThreadProcessId(UIWB.HWnd(), &dwProcessId);

	// wait until we don't get any Save Changes? message boxes
	for (int i = 0; i < 20; i++) {
		// 8 is the Yes button
		if (WaitForSingleControl(dwProcessId, 8, 500) == WAIT_CONTROL_0) {
	//		if (bSaveChanges) {
				// click Yes
				MST.WButtonClick(GetLabel(GL_SCROLLBAR));
		//	}
		//	else {
				// click No
				MST.WButtonClick(GetLabel(GL_SLIDER));
		//	}
		}
		else {
			break;
		}
	}

	// too many Save Changes? message boxes
	if (i >= 20) {
		LOG->RecordInfo("%s: detected more than 20 Save Changes? message boxes when creating new project. Unable to start AppWizard.", "UIProjectWizard::Create");
		return NULL;
	}


   return MST.WGetActWnd(0); 

}

BOOL UIProjectWizard::ConfirmCreate(void)
{
	int WaitMore = 60000 ;
	ExpectPage( UIAW_CONFIRM );
    MST.WButtonClick(GetLabel(IDOK));

    if( GetPage() == UIAW_MESSAGEBOX )
        return FALSE;

	// Wait for the source control dialog up to 1 minute,
	
    WaitForInputIdle(g_hTargetProc, 60000);

	if(IsWindow(HWnd())) // If the AppWiz Dlg is still a window, we have a problem.
	    return FALSE ;

	// Get rid of the Source control dialog if it shows up. REVIEW(Ivanl) process source control later!
	MST.DoKeys("{ESC}") ;
 
/*
	(Let's handle this in the tests, shall we?  - BJoyce)	
	// This handles the case where newly created project has a name similar to that of an already registered project.
	if(UIWB.WaitOnWndWithCtrl(0x6,2000))
	{
		LOG->RecordInfo("Message box came up and clicked the default: Probably a project name has been reused.");
		 MST.DoKeys("{ENTER}") ;
	
	}
*/

/*
	(This was only a temporary situation during V5 development - BJoyce)   
	//This handles the case where newly created project has a name similar to that of an already registered project.
	if(UIWB.WaitOnWndWithCtrl(0x6, 2000))
	{
		LOG->RecordInfo("Message box came up and clicked the default: Probably a project name has been reused.");
		MST.DoKeys("{ENTER}");
	}
*/

    return TRUE;
}

void UIProjectWizard::SetProjType(PROJTYPE type)
{
//    ExpectPage( UIAW_NEWPROJ );
	SetProjType(GetLocString(type));
	Sleep (1000);    // BJoyce - 266MHZ machines are fast!

//    MST.WListItemClk(GetLabel(VPROJ_IDC_PROJ_TYPE), GetLocString(type));
}

void UIProjectWizard::SetName(LPCSTR szName)
{
  //  ExpectPage( IDC_PROJ_NAME );
	m_FlNewDlg.SetName(szName) ;
}

// toggle either the "Top level project" or "Subproject of:" 
// option buttons in the "Insert Project" dlg.
void UIProjectWizard::SetHierarchy(PROJ_HIERARCHY phProjHier)
{
	MST.WOptionSelect(GetLabel(VSHELL_IDC_ADDTOCURRENTWORKSPACE));
	if(phProjHier == TOP_LEVEL_PROJ)
		; //Just do nothing we this is default setting.
	else
		MST.WCheckCheck(GetLabel(VSHELL_IDC_DEPENDENCYOF));
}


// set the "Subproject of:" combo box in the "Insert Project" dlg.
void UIProjectWizard::SetTopLevelProj(LPCSTR szProjName)
	{
	// dependency checkbox must be checked for combo box (below) to be enabled.
	MST.WCheckCheck(GetLabel(VSHELL_IDC_DEPENDENCYOF));
	// combo box in question should be 1st in tab order. it has no label. 
	MST.WComboItemClk("@1", (CString)szProjName);
	}


void UIProjectWizard::SetLocation(LPCSTR szPath) // simpler, more up-to-date version of SetDir().
	{
	if(szPath)
		m_FlNewDlg.SetLocation(szPath) ;
	}


void UIProjectWizard::SetDir(LPCSTR szName)
{
 
	MST.WButtonClick(GetLabel(VSHELL_IDC_BROWSE));
    WaitForInputIdle(g_hTargetProc, 10000);
	UIDialog dlgBrowse;
	MST.WFndWndWait(GetLocString(IDSS_CHOOSE_DIR_TITLE), FW_CASE | FW_FOCUS | FW_HIDDENNOTOK, 5);
	dlgBrowse.WaitAttachActive(2000);
	MST.WEditSetText(NULL, szName);	// ID from ucommdlg.h

	// this first ok will close the dlg on nt4 and win95 if the specified dir was already current.
	// otherwise this ok will just change dirs.
	MST.WButtonClick(GetLabel(IDOK));
	WaitForInputIdle(g_hTargetProc, 10000);

	// if the ide had to change dirs, then this ok will close the dlg on nt4 and win95.
	if (dlgBrowse.IsActive())
	{
		MST.WButtonClick(GetLabel(IDOK));
		WaitForInputIdle(g_hTargetProc, 10000);
	}
	
	// todo(michma): a third ok is needed on win98 due to a bug where they change the
	// capitalization of the directory selected in the listbox on the 2nd ok. weird.
	if (dlgBrowse.IsActive())
	{
		MST.WButtonClick(GetLabel(IDOK));
		WaitForInputIdle(g_hTargetProc, 10000);
	}
}

void UIProjectWizard::SetProjType(CString Name)
{
//	MST.DoKeyshWnd(HWnd(),"^{TAB}") ; //review (chriskoz) bogus tab
	m_FlNewDlg.NewProjectTab() ;
	m_FlNewDlg.SelectFromList(Name) ;
}

// The intent of this function is to set a subdirectory off of the base directory that
// we're currently set at.  Most likely a SetDir() call preceded this call.
void UIProjectWizard::SetSubDir(LPCSTR szName)
{

/*
	(Let's not get too protective here, this stops logo testing. - BJoyce)
	// Make sure that there is no drive or root specifier on the subdirectory name.
	// This case also effectively filters UNC paths, which are also not allowed.
	if (szName[0] == '\\' || (strlen(szName) > 1 && szName[1] == ':'))
	{
		ASSERT(FALSE);		// Assert in debug.
		return;				// Just return in retail.
	}
*/

	CString Name(' ', MAX_PATH);
	MST.WEditText(GetLabel(VSHELL_IDC_LOCATIONFORPROJECT), Name);		// Get the current working directory.
	if (Name.ReverseFind('\\') != (Name.GetLength() -1))
		Name+=(CString ) "\\";	// append a trailing backslash only if it doesn't exist
	Name+=szName;	// append the subdirectory specifier.
	MST.WEditSetText(GetLabel(VSHELL_IDC_LOCATIONFORPROJECT), Name);	// Set the new directory.
}

void UIProjectWizard::SetPlatform(int iType) 
{
    ASSERT( iType <= MST.WListCount(GetLabel(VSHELL_IDC_PLATFORMS)) );
	MST.WListItemClk(GetLabel(VSHELL_IDC_PLATFORMS), iType, 8);		// make sure to click in checkbox
}


void UIProjectWizard::SetAddToWorkspace(BOOL AddToWorkspace) 
{
	// expected the Project pane of new dialog to be open
	if(AddToWorkspace)
		MST.WOptionClick(GetLabel(0x52e2));		// make sure to click in checkbox
	else
		MST.WOptionClick(GetLabel(0x52e1));		// make sure to click in checkbox
}

HWND UIProjectWizard::GetSubDialog(void)	// return the child dialog (the current page)
{
	HWND hwnd = GetWindow(HWnd(), GW_CHILD);
	hwnd = GetWindow(hwnd, GW_HWNDLAST);
	char acClass[32];
	::GetClassName(hwnd, acClass, 31);
	if( strcmp(acClass, "#32770") != 0 )	// REVIEW: there is only one sub-dialog, isn't there?
		return NULL;						// if not, we may need to find the Visible one.
	return hwnd;
}

LPCSTR UIProjectWizard::GetLabel(UINT id)
{
	HWND hwnd = GetSubDialog();
	LPCSTR sz = NULL;
	if( hwnd != NULL )
		sz = ::GetLabel(id, hwnd, TRUE);		// try to find it in the sub-dialog (if there is on)
	if( sz != NULL )
		return sz;
	else
		return ::GetLabel(id);			// but if it doesn't work, try in the main dialog (e.g. the Next/Back buttons)
}
