///////////////////////////////////////////////////////////////////////////////
//  UAPPWIZ.CPP
//
//  Created by :            Date :
//      DavidGa                 10/27/93
//
//  Description :
//      Implementation of the UIAppWizard class
//
													  
#include "stdafx.h"
#include "uappwiz.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "guiv1.h"
#include "..\shl\uiwbmsg.h"
#include "..\shl\wbutil.h"
#include "..\sym\vshell.h"


#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void UIAppWizard::OnUpdate(void) 
{
	UIProjectWizard::OnUpdate() ;
	Initialize() ;
}

void UIAppWizard::Initialize()
{
	pg1 =APPWZ_IDC_RSC_LANG, pg2 = UIAW_DB_NONE, pg3 = APPWZ_IDC_NO_INPLACE ; 
    pg4 =APPWZ_IDC_PONOTDLL , pg5 =APPWZ_IDCD_POPRINT , pg6 =APPWZ_IDC_CLASS_LIST, confirm  = APPWZ_IDC_OUTPUT ;
	
	m_CurrPage.SetAt(pg1, "Comments/External/MFC libs options") ;
	m_CurrPage.SetAt(pg2, "OLE options") ;
	m_CurrPage.SetAt(pg3, "Database options") ;
	m_CurrPage.SetAt(pg4, "Project type") ;
	m_CurrPage.SetAt(pg5, "Project features") ;
	m_CurrPage.SetAt(pg6, "Classes list") ;
	m_CurrPage.SetAt(confirm, "Confirm dialog") ;

	m_pgArr.Add(&pg1) ;
	m_pgArr.Add(&pg2) ;
	m_pgArr.Add(&pg3) ;
	m_pgArr.Add(&pg4) ;
	m_pgArr.Add(&pg5) ;
	m_pgArr.Add(&pg6) ;
	m_pgArr.Add(&confirm) ;
}

BOOL UIAppWizard::IsValid(void)
{
	if(!WaitForWndWithCtrl(VSHELL_IDC_PROJECTTYPES,1500))
		return FALSE ;
	return TRUE ;

} 
// BEGIN_HELP_COMMENT
// Function: int UIAppWizard::GetPage(HWND hwnd /*=NULL*/)
// Description: Get the current page in the AppWizard sequence.
// Return: A value that indicates the current AppWizard page: UIAW_INVALID, UIAW_NEWPROJ, UIAW_APPTYPE, UIAW_DLGOPTIONS, UIAW_DLGFEATURES, UIAW_DLGCLASSES, UIAW_DBOPTIONS, UIAW_OLEOPTIONS, UIAW_FEATURES, UIAW_PROJOPTIONS, UIAW_CLASSES, UIAW_CONFIRM, UIAW_PROGRESS, UIAW_DBSOURCES, UIAW_MESSAGEBOX.
// Param: hwnd The HWND of the AppWizard dialog. NULL means use the current dialog. (Default value is NULL.)
// END_HELP_COMMENT
int UIAppWizard::GetPage(HWND hwnd /*=NULL*/)
{

    if( hwnd == NULL )
	hwnd = WGetActWnd(0);
    if( !::IsWindow(hwnd) )
	return UIAW_INVALID;
    char acBuf[128];
    ::GetText(hwnd, acBuf, 127);
//    LOG->Comment("GetPage called when '%s' was active", acBuf);

	CString strTitleAppWiz = GetLocString(IDSS_AW_TITLE_APPWIZ);

    if( stricmp(acBuf, GetLocString(IDSS_AW_TITLE_NEWPROJ)) == 0 )
	return UIAW_NEWPROJ;     // New Project page
    else if( strnicmp(acBuf, strTitleAppWiz, strTitleAppWiz.GetLength()) == 0 )
    {
		static CString strStep = GetLocString(IDSS_AW_TITLE_STEP);
		static CString strOf = GetLocString(IDSS_AW_TITLE_OF);
	char* pch = strstr(acBuf, strStep);
	if( pch == NULL )
		{
			UIWBMessageBox mb;      // UIAWMessageBox?
			if( mb.AttachActive() && mb.ButtonExists(MSG_OK) )
		    return UIAW_MESSAGEBOX;  // Some error dialog with an OK button
	    return UIAW_INVALID;     // if it's not a messagebox, I dunno what it is
		}

	EXPECT( strlen(pch) >= (size_t)strStep.GetLength()+1 );
		pch += strStep.GetLength();
	int nPage = *pch - '0';     // Step #
	int nOfPages = 0;


    CString Title = acBuf ;
	CString NumberOfPages ;
	if ((nPage = Title.FindOneOf("123456789")) == -1)
		return UIAW_INVALID ;
	 else
	 {
		Title = Title.Right(Title.GetLength() - nPage) ;
		NumberOfPages = Title ;
		Title = Title.Left(1) ;
		nPage = *(Title.GetBuffer(1)) - '0' ;
	 }

	 // Number of pages.
	  if ((nOfPages = NumberOfPages.FindOneOf("123456789")) != -1)
	  {
	  	NumberOfPages = NumberOfPages.Right(Title.GetLength() - nOfPages) ;
		NumberOfPages = NumberOfPages.Left(1) ;
		nOfPages = *(NumberOfPages.GetBuffer(1)) - '0' ;
	  }


	if( nPage == 1 )
	    return UIAW_APPTYPE;
	if( nOfPages == 4 )
	{
			switch( nPage )
			{
		    case 2:
			return UIAW_DLGOPTIONS;
		    case 3:
			return UIAW_DLGCLASSES;
				case 4:
					return UIAW_DLGFEATURES;
			}
	}
	else if( nOfPages == 6 )
	{
			switch( nPage )
			{
		    case 2:
			return UIAW_DBOPTIONS;
		    case 3:
				    return UIAW_OLEOPTIONS;
			    case 4:
					return UIAW_FEATURES;
				case 5:
			return UIAW_PROJOPTIONS;
		    case 6:
			return UIAW_CLASSES;
			}
	}
    }
	else if( stricmp(acBuf, GetLocString(IDSS_AW_TITLE_CONFIRM)) == 0 )
		return UIAW_CONFIRM;
    else if( stricmp(acBuf, GetLocString(IDSS_AW_TITLE_DBSOURCE)) == 0 )
	return UIAW_DBSOURCES;
    else if( stricmp(acBuf, GetLocString(IDSS_AW_TITLE_PROGRESS)) == 0 )
	return UIAW_PROGRESS;

    return UIAW_INVALID;
}


// BEGIN_HELP_COMMENT
// Function: void UIAppWizard::ExpectPage(int nPage)
// Description: Expect that the given page is currently active in the AppWizard sequence. An exception is thrown if nPage doesn't match the current page.
// Return: none
// Param: nPage A value that contains the expected AppWizard page: UIAW_INVALID, UIAW_NEWPROJ, UIAW_APPTYPE, UIAW_DLGOPTIONS, UIAW_DLGFEATURES, UIAW_DLGCLASSES, UIAW_DBOPTIONS, UIAW_OLEOPTIONS, UIAW_FEATURES, UIAW_PROJOPTIONS, UIAW_CLASSES, UIAW_CONFIRM, UIAW_PROGRESS, UIAW_DBSOURCES, UIAW_MESSAGEBOX.
// END_HELP_COMMENT
void UIAppWizard::ExpectPage(int nPage)
{
	EXPECT( GetPage() == nPage );           // expect we're where we want to be
	EXPECT( nPage == m_nPage );                     // expect that m_nPage is also in sync
}

// BEGIN_HELP_COMMENT
// Function: LPCSTR UIAppWizard::ExpectedTitle(int nPage) const
// Description: Get the title of the given AppWizard page.
// Return: A pointer to a string that contains the title of the given AppWizard page.
// Param: nPage A value that contains the AppWizard page to get the title for: UIAW_NEWPROJ, UIAW_APPTYPE, UIAW_DLGOPTIONS, UIAW_DLGFEATURES, UIAW_DLGCLASSES, UIAW_DBOPTIONS, UIAW_OLEOPTIONS, UIAW_FEATURES, UIAW_PROJOPTIONS, UIAW_CLASSES, UIAW_CONFIRM, UIAW_PROGRESS, UIAW_DBSOURCES.
// END_HELP_COMMENT
CString UIAppWizard::ExpectedTitle(int nPage) const
{
    if( nPage < 0 )
		nPage = m_nPage;
    static CString strTitleAppWiz;
	strTitleAppWiz = GetLocString(IDSS_AW_TITLE_APPWIZ);
    strTitleAppWiz += GetLocString(IDSS_AW_TITLE_STEP);
	CString strOf4 = GetLocString(IDSS_AW_TITLE_OF);
	CString strOf6 = strOf4 + "6";
	strOf4 += "4";

    switch( nPage )
    {
	case UIAW_NEWPROJ:
			strTitleAppWiz = GetLocString(IDSS_AW_TITLE_NEWPROJ);
	    break;
	case UIAW_APPTYPE:
			strTitleAppWiz += "1";
			break;
	case UIAW_DLGOPTIONS:
			strTitleAppWiz += "2";
			strTitleAppWiz += strOf4;
			break;
	case UIAW_DLGFEATURES:
			strTitleAppWiz += "3";
			strTitleAppWiz += strOf4;
			break;
	case UIAW_DLGCLASSES:
			strTitleAppWiz += "4";
			strTitleAppWiz += strOf4;
			break;
	case UIAW_DBOPTIONS:
			strTitleAppWiz += "2";
			strTitleAppWiz += strOf6;
			break;
	case UIAW_OLEOPTIONS:
			strTitleAppWiz += "3";
			strTitleAppWiz += strOf6;
			break;
	case UIAW_FEATURES:
			strTitleAppWiz += "4";
			strTitleAppWiz += strOf6;
			break;
	case UIAW_PROJOPTIONS:
			strTitleAppWiz += "5";
			strTitleAppWiz += strOf6;
			break;
	case UIAW_CLASSES:
			strTitleAppWiz += "6";
			strTitleAppWiz += strOf6;
			break;
	case UIAW_CONFIRM:
	    strTitleAppWiz = GetLocString(IDSS_AW_TITLE_CONFIRM);
			break;
	case UIAW_DBSOURCES:
	    strTitleAppWiz = GetLocString(IDSS_AW_TITLE_DBSOURCE);
			break;
	case UIAW_PROGRESS:
	    strTitleAppWiz = GetLocString(IDSS_AW_TITLE_PROGRESS);
			break;
	default:        // if asked before we calculate m_nPage
	    strTitleAppWiz = GetLocString(IDSS_AW_TITLE_NEWPROJ);
    }
	return strTitleAppWiz;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIAppWizard::Create(void)
// Description: Click the Create button on the New Project dialog.
// Return: The HWND of the dialog displayed after clicking the Create button (either the Add Files dialog or the first page of the AppWizard sequence).
// END_HELP_COMMENT
HWND UIAppWizard::Create(void)
{
	const char* const THIS_FUNCTION = "UIAppWizard::Create";

/*	if (GetPage() != UIAW_NEWPROJ) {
		LOG->RecordInfo("%s: Current AppWizard dialog is not the New Project dialog (expected %d; found %d). Cannot create AppWizard project.", THIS_FUNCTION, UIAW_NEWPROJ, GetPage());
		return NULL;
	}

    int index = MST.WListIndex(GetLabel(VPROJ_IDC_PROJ_TYPE));
    // determine if the project to be created is an AppWizard project or not
	// get the currently selected item in the project type combo box
	CString strProjType;
	MST.WListText(GetLabel(VPROJ_IDC_PROJ_TYPE), strProjType);
	BOOL bAppWiz = FALSE;
	if (strProjType == GetLocString(IDSS_PROJTYPE_APPWIZ_APP) || strProjType == GetLocString(IDSS_PROJTYPE_APPWIZ_DLL)) {
		bAppWiz = TRUE;
	}
  */
     MST.WButtonClick(GetLabel(IDOK));

   	// Wait five seconds for a "Subdir doesn't exist.  Do you really want to create... blah blah blah" message box.
	if (WaitMsgBoxText(GetLocString(IDSS_CREATE_DIR_PROMPT), 5))
		MST.WButtonClick(GetLabel(MSG_YES));	// Go ahead and say it's ok to create it.

	Sleep(2000) ;
	HWND hwnd;
	if (hwnd = MST.WFndWndWait(ExpectedTitle(UIAW_APPTYPE), FW_DEFAULT, 5))
	{
		LOG->Comment("Found '%s'", ExpectedTitle(UIAW_APPTYPE));
		MST.WSetActWnd(hwnd) ;

		RECT rcPage;
		GetClientRect(hwnd, &rcPage);
		ClickMouse( VK_LBUTTON ,hwnd, rcPage.left+50, rcPage.top - 15 );	  // Click on first Tab	(left most)
		ClickMouse( VK_LBUTTON ,hwnd, rcPage.left+50, rcPage.top - 15 );	  // Click on first Tab	(left most)

	}
    int nPage = UpdatePage();
    if( nPage == UIAW_MESSAGEBOX )
	return NULL;     // an error dialog came up instead
	 // if( bAppWiz ) 
    {
	LOG->Comment("Now looking for '%s'", ExpectedTitle(UIAW_APPTYPE));
	MST.WFndWndWait(ExpectedTitle(UIAW_APPTYPE), FW_DEFAULT, 5);
	return NULL;            // ought to be page 1, but we might have changed dirs
    }

	return NULL; // No longer invoke add files dialog after creating new project workspace.
}


// BEGIN_HELP_COMMENT
// Function: void UIAppWizard::SetAppType(UINT id)
// Description: Set the type of AppWizard application (SDI, MDI, or dialog based).
// Return: none
// Param: id The ID of the control to click on that specifies the type of AppWizard application.
// END_HELP_COMMENT
void UIAppWizard::SetAppType(UINT id)
{
	ExpectPage( UIAW_APPTYPE );
	MST.WOptionClick(GetLabel(id));
}

// BEGIN_HELP_COMMENT
// Function: void UIAppWizard::SetDBOptions(UINT id)
// Description: Set the database option for the AppWizard application (none, header files, db view, or db view and file support).
// Return: none
// Param: id An integer containing the ID of the control to click on that specifies the database option.
// END_HELP_COMMENT
void UIAppWizard::SetDBOptions(UINT id)
{
	ExpectPage( UIAW_DBOPTIONS );
	MST.WOptionClick(GetLabel(id));
}

// BEGIN_HELP_COMMENT
// Function: void UIAppWizard::SetOLEOptions(UINT id)
// Description: Set the OLE option for the AppWizard application (none, container, mini-server, full-server, container and server, support for OLE compound files, or support for OLE automation).
// Return: none
// Param: id An integer containing the ID of the control to click on that specifies the OLE option.
// END_HELP_COMMENT
void UIAppWizard::SetOLEOptions(UINT id)
{
	ExpectPage( UIAW_OLEOPTIONS );
	//Determine if the OLE options are enabled.
	if (MST.WOptionEnabled(::GetLabel(id)))
		OleInstalled = TRUE ;
    else OleInstalled =FALSE ;
	MST.WOptionClick(GetLabel(id));
}

// BEGIN_HELP_COMMENT
// Function: void UIAppWizard::SetOLEOptions2(UINT id)
// Description: Set the OLE option for the AppWizard application (none, container, mini-server, full-server, container and server, support for OLE compound files, or support for OLE automation).
// Return: none
// Param: id An integer containing the ID of the control to click on that specifies the OLE option.
// END_HELP_COMMENT
void UIAppWizard::SetOLEOptions2(UINT id)
{
	ExpectPage( UIAW_OLEOPTIONS );
	//Determine if the OLE options are enabled.
	if (MST.WCheckEnabled(::GetLabel(id)))
	{
		MST.WCheckClick(GetLabel(id));
	}
}

// BEGIN_HELP_COMMENT
// Function: void UIAppWizard::SetProjOptions(UINT id)
// Description: Set project options for the AppWizard application (dockable toolbar, status bar, printing, help, 3d controls, MAPI, or WinSockets).
// Return: none
// Param: id An integer that contains the ID of the control to click on that specifies the project option.
// END_HELP_COMMENT
void UIAppWizard::SetProjOptions(UINT id)
{
	ExpectPage( UIAW_PROJOPTIONS );
	MST.WOptionClick(GetLabel(id));
}

// BEGIN_HELP_COMMENT
// Function: void UIAppWizard::SetFeatures(UINT id)
// Description: Set features options for the AppWizard application (dockable toolbar, status bar, printing, help, 3d controls, MAPI, or WinSockets).
// Return: none
// Param: id An integer that contains the ID of the control to click on that specifies the project option.
// END_HELP_COMMENT
void UIAppWizard::SetFeatures(UINT id)
{
	ExpectPage( UIAW_FEATURES );
	if (MST.WCheckEnabled(::GetLabel(id)))
	{
		MST.WCheckClick(GetLabel(id));
	}
}


// BEGIN_HELP_COMMENT
// Function: void UIAppWizard::SelectClass(int n)
// Description: Select a class in the AppWizard class page by index.
// Return: none
// Param: n An integer containing the 1-based index of the class to select into the list of classes.
// END_HELP_COMMENT
void UIAppWizard::SelectClass(int n)
{
    MST.WListItemClk(GetLabel(APPWZ_IDC_CLASS_LIST), n);
}

// BEGIN_HELP_COMMENT
// Function: void UIAppWizard::SelectClass(LPCSTR sz)
// Description: Select a class in the AppWizard class page by name.
// Return: none
// Param: sz A pointer to a string containing the name of the class to select in the list of classes.
// END_HELP_COMMENT
void UIAppWizard::SelectClass(LPCSTR sz)
{
    MST.WListItemClk(GetLabel(APPWZ_IDC_CLASS_LIST), sz);
}

// BEGIN_HELP_COMMENT
// Function: CString UIAppWizard::GetClassName(void)
// Description: Get the name of the selected class from the AppWizard class page.
// Return: A CString containing the name of the selected class.
// END_HELP_COMMENT
CString UIAppWizard::GetClassName(void)         // AACK! This is the same name as a Windows API
{
    CString str;
    MST.WEditText(GetLabel(APPWZ_IDC_CLASS_NAME), str);
    return str;
}

// BEGIN_HELP_COMMENT
// Function: CString UIAppWizard::GetBaseClass(void)
// Description: Get the name of the selected class's base class from the AppWizard class page.
// Return: A CString containing the name of the base class.
// END_HELP_COMMENT
CString UIAppWizard::GetBaseClass(void)
{
    CString str;
    MST.WEditText(GetLabel(APPWZ_IDC_BASE_CLASS), str);
    return str;
}

// BEGIN_HELP_COMMENT
// Function: CString UIAppWizard::GetSourceFile(void)
// Description: Get the name of the source file for the selected class from the AppWizard class page.
// Return: A CString that contains the name of the source file.
// END_HELP_COMMENT
CString UIAppWizard::GetSourceFile(void)
{
    CString str;
    MST.WEditText(GetLabel(APPWZ_IDC_IFILE), str);
    return str;
}

// BEGIN_HELP_COMMENT
// Function: CString UIAppWizard::GetHeaderFile(void)
// Description: Get the name of the header file for the selected class from the AppWizard class page.
// Return: A CString that contains the name of the header file.
// END_HELP_COMMENT
CString UIAppWizard::GetHeaderFile(void)
{
    CString str;
    MST.WEditText(GetLabel(APPWZ_IDC_HFILE), str);
    return str;
}

