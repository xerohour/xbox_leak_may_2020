///////////////////////////////////////////////////////////////////////////////
//  uogaldlg.cpp
//
//  Created by :            Date :
//      EnriqueP              10/11/94
//	Modified by:			Date:
//		Anita George			8/12/96
//		added functions for new File-Open-like Gallery UI
//				  
//  Description :
//      Implementation of the UIOGalleryDlg class	 
//

#include "stdafx.h"
#include "uogaldlg.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\shl\uwbframe.h"
#include "guiv1.h"
#include "guitarg.h"
//#include "..\..\umsgbox.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: HWND UIOGalleryDlg::Display() 
// Description: Bring up the Object Gallery dialog.
// Return: The HWND of the Object Gallery dialog, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UIOGalleryDlg::Display() 
{	
	AttachActive();		  // Check if is already up
	Sleep(5000);
	if (!IsValid())
	{
		//WaitForInputIdle(g_hTargetProc, 60000);
        LOG->RecordInfo("added sleep for timing problem-AnitaG");
		Sleep(5000);
		UIWB.DoCommand(IDM_GALLERY, DC_MNEMONIC);
		MST.WFndWndWait(ExpectedTitle(), FW_DIALOG, 60);
		//Sleep(20000);

		//UIDialog gallocdlg("Insert Component");	// REVIEW: Invalid Gallery dlg Use	GetLocString(IDSS_INVALID_GALLEERY)

		//gallocdlg.AttachActive();
		AttachActive();

		//if (!gallocdlg.IsValid())
		if (!IsValid())
		{
			LOG->RecordFailure("Couldn't bring up Gallery dialog.");
			return NULL;	// Error bringing up the gallery
		}
		else
		{
			Sleep(1000);
			return MST.WGetActWnd(0); 
		}

	}
	else
	{
		Sleep(1000);
		return MST.WGetActWnd(0); 
	}
}


// BEGIN_HELP_COMMENT
// Function: HWND UIOGalleryDlg::Close( void ) 
// Description: Closes the Object Gallery Main dialog and detaches the component list view.
// Return: The HWND of the window that remains after closing the Gallery.
// END_HELP_COMMENT
HWND UIOGalleryDlg::Close( void )
{
	m_OgletList.Detach();
	UIDialog::Close();
	WaitUntilGone(1000);
	Sleep(2000);

	return MST.WGetActWnd(0); 
}


// BEGIN_HELP_COMMENT
// Function: int UIOGalleryDlg::Import( CString& strOglet, BOOL bCopyToGallery /* = FALSE */)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int UIOGalleryDlg::Import( CString& strOglet, BOOL bCopyToGallery /* = FALSE */)
{
	UOGCustomDlg custDlg;
	UOGImportDlg impDlg;

	Display();
	custDlg.Display();
	impDlg.Display();
	impDlg.SetName(strOglet);
	impDlg.CopyToGallery(bCopyToGallery);
	int bResult = impDlg.Import();
	custDlg.Close();

	return bResult; 
}

// BEGIN_HELP_COMMENT
// Function: HWND UIOGalleryDlg::Insert( CString& strOglet) 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
HWND UIOGalleryDlg::Insert( CString& strOglet)
{
	Display();
	if (!SelectComponent(strOglet) )
	{
		LOG->Comment("Can't find oglet %s in the gallery", strOglet);
		return NULL;
	}
	if(!MST.WButtonEnabled(GetLabel(ID_OG_INSERT) ) )
	{
		LOG->Comment("Can't insert component because there is no project loaded");
		return NULL;
	}
	MST.WButtonClick(GetLabel(ID_OG_INSERT)); 
	return MST.WGetActWnd(0);
}

// BEGIN_HELP_COMMENT
// Function: HWND UIOGalleryDlg::Insert( UINT nOglet) 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
HWND UIOGalleryDlg::Insert( int nOglet)
{
	Display();
	if (SelectComponent(nOglet) == -1 )
	{
		LOG->Comment("Can't Insert component %d because it was not found", nOglet);
		return NULL;
	}
	
	if(!MST.WButtonEnabled(GetLabel(ID_OG_INSERT) ) )  
	{
		LOG->Comment("Can't insert component because there is no project loaded");
		return NULL;
	}

	MST.WButtonClick(GetLabel(ID_OG_INSERT)); 
	Sleep(1000);
	MST.DoKeys("{ENTER}");  //REVIEW: OK Apply dialog for OGX or OCX 
	Sleep(1000);

	return MST.WGetActWnd(0);
}

// BEGIN_HELP_COMMENT
// Function: int UIOGalleryDlg::SelectComponent( CString& strOglet )	
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int UIOGalleryDlg::SelectComponent( CString& strOglet )	
{
        short const LINE_LEN=4; // no of components in one line
	Display();
	// REVIEW:Give Listbox the focus
//	m_OgletList.Attach( GetDlgItem(ID_OG_LISTCTRL) );
	int nOglets = m_OgletList.GetItemCount();
	if (nOglets == 0)
	{
		LOG->Comment("This OG Page is empty");
		return 0;
	}
	
	
	int iOglet=0;
	while ( (strOglet != GetSelectedComponentText() ) && nOglets-- )
	{
		if((iOglet+1)%LINE_LEN!=0)
                     MST.DoKeys("{RIGHT}");	// Select next oglet with right arrow key
                else
                {  // the last oglet in the line, select the first in the next line
                    MST.DoKeys("{LEFT}");MST.DoKeys("{LEFT}");MST.DoKeys("{LEFT}"); 
                    MST.DoKeys("{DOWN}"); 
                }
                iOglet++; 
	}

	if (strOglet != GetSelectedComponentText() )
	{
		LOG->Comment(" The component %s can't be selected because it was not found", strOglet);
		return 0;
	}
	return 1;
}

// BEGIN_HELP_COMMENT
// Function: int UIOGalleryDlg::SelectComponent( UINT nOglet )	
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
int UIOGalleryDlg::SelectComponent( int nOglet )	
{
    //short const LINE_LEN=4; // no of components in one line

	Display();
	// REVIEW:Give Listbox the focus
//	m_OgletList.Attach( GetDlgItem(ID_OG_LISTCTRL) 
	int nOglets = m_OgletList.GetItemCount();
	if (nOglets == 0)
	{
		LOG->Comment("This OG Page is empty");
		return 0;
	}
	
	//find how many component appear in a line
	MST.DoKeys("{HOME}");
	MST.DoKeys("{RIGHT 10}");	//to make sure the last item in the line is selected
	int nLineIndex = m_OgletList.GetNextItem(-1, LVNI_SELECTED);
	int LINE_LEN = nLineIndex+1;
	MST.DoKeys("{HOME}");

	//set iOriginalOglet to equal the index of the selected item in the list view
	int iOriginalOglet = m_OgletList.GetNextItem(-1, LVNI_SELECTED);
	//if GetNextItem returns -1, a selected item was not found
	if (iOriginalOglet == -1)
	{
		LOG->Comment("No component is selected");
		return 0;
	}

	int iOglet = iOriginalOglet;
	while ( nOglet != iOglet )
	{
		if((iOglet+1)%LINE_LEN!=0)
                MST.DoKeys("{RIGHT}");	// Select next oglet with right arrow key
        else
                {  // the last oglet in the line, select the first in the next line
					for ( int i = 1; i < LINE_LEN; ++i )
					{
                    MST.DoKeys("{LEFT}");
					}
                    MST.DoKeys("{DOWN}"); 
                }
		iOglet = m_OgletList.GetNextItem(-1, LVNI_SELECTED);
		ASSERT(iOglet != -1);	// No Oglet is selected
		
		if (iOglet == iOriginalOglet)		// If we've looked at all of them
			break;
// If we reach the end and the component to select is not the last one then go to the begining.
		if ( (iOglet == (nOglets -1) ) && (nOglet != (nOglets-1) ) )		
		{
			MST.DoKeys("{HOME}");
			iOglet = m_OgletList.GetNextItem(-1, LVNI_SELECTED);
			ASSERT(iOglet != -1);	// No component is selected
		}
	}

	if (nOglet != iOglet )
	{
		LOG->Comment("The component %d can't be selected because it was not found", nOglet);
		return -1;
	}
	return iOglet;		// iOglet is zero based 
}


// BEGIN_HELP_COMMENT
// Function: LPTSTR UIOGalleryDlg::GetSelectedComponentText(void)
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
LPTSTR UIOGalleryDlg::GetSelectedComponentText(void)
{
	// Get selected item
	int iOglet = m_OgletList.GetNextItem(-1, LVNI_SELECTED);
	ASSERT(iOglet != -1);	// No component is selected

/*	LV_ITEM lvItem;
	lvItem.iItem = iOglet;
	lvItem.iSubItem = 0;
	lvItem.mask = LVIF_PARAM;

	if( !m_OgletList.GetItem(&lvItem) )
		return NULL;				// Something went wrong
	
*/	static char ogText[32];
	
	m_OgletList.GetItemText(iOglet, 0, ogText, 31); // This creates an access violation. Looks like address spaces problem

	
	return ogText;
}


// BEGIN_HELP_COMMENT
// Function: int UIOGalleryDlg::ActivateCategory( CString& strCategory )
// Description: Select a page in the Object Gallery dialog by name. (See SelectPage(UINT) to select a page by index.)
// Return: The HWND of the selected page, if successful; NULL otherwise.
// Param: szPageName A pointer to a string that contains the name of the page.
// END_HELP_COMMENT
int UIOGalleryDlg::ActivateCategory( CString& strCategory )	
{
	Display();

//     Check if Category is already selected
	CString strOriginalPage, strCurPage;
	strOriginalPage = GetActivePageText();
	if (strOriginalPage == strCategory)
		return 0; 

//	Start looking from the first page
	
	strCurPage = ActivateFirstPage();
	int cPage = 1;

	while (	( strCurPage != strCategory ) &&  ( strCurPage != ActivateNextPage() ) )
	{
		strCurPage = GetActivePageText();
		cPage++;
	}
	
	if (  strCurPage != strCategory)
	{
//		WriteWarning(" The Page %s can't be found", szPageName);
	
	// Restore original page
		ActivateCategory( strOriginalPage);					
		return -1;	
	}

	// Page was found, return page number.
	return cPage; 	
}

// BEGIN_HELP_COMMENT
// Function: LPTSTR UIOGalleryDlg::ActivateCategory( UINT nPage ) 
// Description: Select a page in the Object Gallery dialog by index. (See SelectPage(LPCSTR) to select a page by name.)
// Return: The HWND of the selected page, if successful; NULL otherwise.
// Param: nPage An integer that contains the 1-based index of the page to select.
// END_HELP_COMMENT
LPTSTR UIOGalleryDlg::ActivateCategory( UINT nPage )	
{
	Display();

	ActivateFirstPage();
	for ( unsigned int i = 1; i < nPage; i++)
		ActivateNextPage();

//	return the text of the Active Category
	return GetActivePageText(); 
}


// BEGIN_HELP_COMMENT
// Function: HWND UIOGalleryDlg::CreateCategory( CString& strCategory )
// Description: Close the Object Gallery Customize dialog.
// Return: The HWND of the Object Gallery dialog, if successful; the HWND of the Object Gallery Customize dialog otherwise.
// END_HELP_COMMENT
HWND UIOGalleryDlg::CreateCategory( CString& strCategory )
{
	UOGCustomDlg custDlg;

	Display();
	custDlg.Display();
	// custDlg.ActivateCategories();	REVIEW: TODO.  We assume the Categories Listbox has focus
	
	// Move to last row, the new entry row
	MST.DoKeys("{END}");		
	MST.DoKeys(strCategory);

	// Accept new name
	MST.DoKeys("{ENTER}");	
	custDlg.Close();

	return MST.WGetActWnd(0); 
}




// *********** Internal Functions

UINT UIOGalleryDlg::GetPageCount(void)
{
	ActivateFirstPage();

	CString strCurPage;
	strCurPage = GetActivePageText();
	unsigned int nPages = 1;	// There will always be at least 1 category

	while (	strCurPage != ActivateNextPage() )
	{
		strCurPage = GetActivePageText();
		nPages++;
	}

	ActivateFirstPage();

	return nPages;
}

LPTSTR UIOGalleryDlg::GetActivePageText(void)
{
	Display();
	
	HWND hTabs = GetTabHWnd();
	static char szTabText[32];
	GetWindowText( hTabs, szTabText,31 );
	
	return szTabText;
}	
	
LPTSTR UIOGalleryDlg::ActivateFirstPage(void)
{
	Display();

	HWND hTabs = GetTabHWnd();
	RECT rcTabs; 
	GetClientRect(hTabs, &rcTabs);

	// Click on view to give it focus	
	ClickMouse( VK_LBUTTON ,hTabs, rcTabs.left+50, rcTabs.top - 4 );	  
	// Click on first Tab	(left most)
	ClickMouse( VK_LBUTTON ,hTabs, rcTabs.left+50, rcTabs.top + 4 );
	
  	return GetActivePageText();
}

LPTSTR UIOGalleryDlg::ActivateNextPage( BOOL bRight /* TRUE */)
{
	Display();

	if ( !bRight )
		MST.DoKeys("^{PGUP}");
	else
		MST.DoKeys ("^{PGDN}") ;

	return GetActivePageText();
}	 

HWND UIOGalleryDlg::GetTabHWnd(void)
{
	Display();

	char szActWindowText[32];
	GetWindowText( WGetActWnd(0), szActWindowText,31 );

	HWND hSubDialog = ::GetDlgItem( WGetActWnd(0) , ID_OG_SUBDIALOG);
	ASSERT(hSubDialog);
	HWND hTab = ::GetDlgItem( hSubDialog , ID_OG_TABCTRL);
	ASSERT(hTab);

	return hTab;
}

////************************  Other OG functions


// BEGIN_HELP_COMMENT
// Function: BOOL UIOGalleryDlg::DeleteOGDatabase(CString& strPath) 
// Description: Delete the Object Gallery database file.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: strPath A CString that contains the name of the path to the IDE\Template directory
// END_HELP_COMMENT
BOOL UIOGalleryDlg::DeleteOGDatabase(CString& strPath) 
{	
	
	CString strOGPath = strPath; //"e:\\ide\\Template\\"
	KillFile(strOGPath.GetBuffer(strOGPath.GetLength()), "gallery.dat");
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIOGalleryDlg::RenameOGDatabase(CString& strPath, CString& strNewName, ROGD_VAL nRenameVal /* = ROGD_REPLACE */) 
// Description: Delete the Object Gallery database file.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: strPath A CString that contains the name of the path to the IDE\Template directory
// END_HELP_COMMENT
BOOL UIOGalleryDlg::RenameOGDatabase(CString& strPath, CString& strNewName, ROGD_VAL nRenameVal /* = ROGD_REPLACE */) 
{	
	
	CString strOGPath = strPath; //"e:\\ide\\template\\"
	ASSERT(!strOGPath.IsEmpty());

	strNewName = strOGPath + strNewName;
	strOGPath += "gallery.dat";

	TRY
	{
		if (nRenameVal != ROGD_RESTORE)
			CFile::Rename(strOGPath.GetBuffer(strOGPath.GetLength()), strNewName.GetBuffer(strNewName.GetLength()));
		else
			CFile::Rename( strNewName.GetBuffer(strNewName.GetLength()), strOGPath.GetBuffer(strOGPath.GetLength()));
	}
	CATCH( CFileException, e )
	{
		#ifdef _DEBUG
		if (nRenameVal != ROGD_RESTORE)
			afxDump << "File " << strOGPath << " not found, cause = "
				<< e->m_cause << "\n";
		else
			afxDump << "File " << strNewName << " not found, cause = "
				<< e->m_cause << "\n";
		#endif
	}
	END_CATCH

	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: LPTSTR UIOGalleryDlg::GetGalleryDir(CString& strPath) 
// Description: Delete the Object Gallery database file.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: strPath A CString that contains the name of the path to the IDE
// END_HELP_COMMENT
LPTSTR UIOGalleryDlg::GetGalleryDir(CString& strPath)
{
	static CString strOGPath = strPath;		//  = "e:\\ide\\bin\\"
	int iBin = strOGPath.ReverseFind('\\');

	ASSERT(iBin != -1);	// make sure is a valid path
	
	// truncate last '\\'
	strOGPath = strOGPath.Left(iBin);	
	// Look for installation directory
	iBin = strOGPath.ReverseFind('\\');
	strOGPath = strOGPath.Left((iBin+1));
	strOGPath += "Template\\";

	return strOGPath.GetBuffer(strOGPath.GetLength());
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIOGalleryDlg::VerifyFilesInDir(CStringList* pFileList, CString& strPath) 
// Description: The description for this function is not available. Please see the appropriate component owner for a description of this function.
// Return: The return value description for this function is not available.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
BOOL UIOGalleryDlg::VerifyFilesInDir(CStringList* pFileList, CString& strPath) 
{
	POSITION pos = pFileList->GetHeadPosition();
	BOOL bResult = TRUE;
	while (pos != NULL)
	{
		CString strFile; 
		if (!strPath.IsEmpty())
			strFile = strPath + "\\";	
		else
			strFile.Empty();

		// Get filename from list
		strFile += pFileList->GetNext(pos);
		
		if (GetFileAttributes( strFile ) == -1 )
		{
			bResult = FALSE;
			LOG->Comment("File %s could not be found", strFile);
		}
	}
	
	return bResult;
}


/**************************************************************/
//  UOGCustomDlg class  

HWND UOGCustomDlg::Display(void)
{
	AttachActive();		  // Check if is already up
	if (!IsValid())
	{
		MST.WButtonClick(GetLabel( ID_OG_CUSTOMIZE ));  
	 	AttachActive();
		if (!IsValid())
			return NULL;
	}

	return 	MST.WGetActWnd(0); 
}

/**************************************************************/
//  UOGImportDlg class  

HWND UOGImportDlg::Display(void)
{
	AttachActive();		  // Check if is already up
	if (!IsValid())
	{
		MST.WButtonClick(GetLabel( ID_OG_IMPORT ));  
	 	AttachActive();
		if (!IsValid())
			return NULL;
	}

	return 	MST.WGetActWnd(0); 
}

int UOGImportDlg::SetName(CString& strName)
{
	MST.WEditSetText(GetLabel(ID_OG_FILE_NAME),strName) ;
	return TRUE;
}	

int UOGImportDlg::CopyToGallery(BOOL bCopyToGallery /* = TRUE */)
{
	if (bCopyToGallery)
	{
		MST.WCheckCheck(GetLabel(ID_OG_COPY_TO_GALLERY)) ;
	}
	else
	{
		MST.WCheckUnCheck(GetLabel(ID_OG_COPY_TO_GALLERY)) ;
	}
	return TRUE;
}	

int UOGImportDlg::Import(void)
{
	if (!IsValid())
		Display();
	
	MST.WButtonClick(GetLabel( ID_OG_IMPORT_IMPORT ));  

	// Check to see if Import succeded, look if Customize dialog is active

	UOGCustomDlg dlgCust;
	dlgCust.WaitAttachActive(4000);
	if (!dlgCust.IsValid())
	{
		// Some alert popped up, Import failed, OK alert and exit Import dialog
		MST.DoKeys("{ENTER}");
		MST.DoKeys("{ESC}");
		return 0;
	}
	
	return 1;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIOGalleryDlg::FindFileInGallery(CString OgxName)
// Description: Looks at location in Gallery to verify if ogx exists or not.
// Return: Returns 1 if ogx appears in Gallery,0 if it doesn't, 
// 2 if a different ogx appears, and 3 if project folder not found, 4 if gallery doesn't display.
// Param: Name of the project. Name of the ogx.
// END_HELP_COMMENT
int UIOGalleryDlg::FindFileInGallery(CString ProjName, CString OgxName) 
{
	if (Display() != NULL)
	{
		UIFileDlg::SetName(ProjName);
		MST.DoKeys("{ENTER}");
		Sleep(3000);
		//if error message does not occur
		if (!WaitMsgBoxText(GetLocString(IDSS_NOFILE), 10000))
		{
			MST.DoKeys("{BACKSPACE}");
			//do shift tab to give focus to listview
			MST.DoKeys("+({TAB})");
			MST.DoKeys("{UP}");
			CString FileName = UIFileDlg::GetName();
			if (FileName != "")
			{
				if ((OgxName.Find(FileName)) != -1)
				{
					LOG->Comment("OGX found in gallery");
					return 1;
				}
				else
				{
					LOG->Comment("different OGX selected");
					return 2;
				}
			}
			else //if blank string then file doesn't exist
			{
				LOG->Comment("OGX not found in gallery");
				return 0;
			}
			//Close();
		}
		else //if error message appears
		{
			Sleep(3000);
			MST.WButtonClick(GetLocString(IDSS_OK));
			//Close();
			LOG->RecordInfo("Project folder not found in gallery.");
			return 3;
		}
	}
	else  //if gallery didn't display
	{
		return 4;
	}
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIOGalleryDlg::FindClassInClassView(CString ProjName,CString ClassName)
// Description: Selects class from classview.
// Return: BOOL TRUE if class is found, FALSE if class not found.
// Param: Name of the class you're looking for.
// END_HELP_COMMENT
BOOL UIOGalleryDlg::FindClassInClassView(CString ProjName, CString ClassName) 
{
	BOOL bResults = FALSE;
	UIWorkspaceWindow WorkSpaceWnd; //create project workspace window object
	//if project workspace activates properly
	if (WorkSpaceWnd.Activate())  
	{
		//if classview activates properly
		if (WorkSpaceWnd.ActivateClassPane())
		{
			int ClassSelected = WorkSpaceWnd.SelectClass(ClassName, ProjName);
			if (ClassSelected == 0)  //0 = successful
			{
				bResults = TRUE;
				LOG->Comment("Class selected properly");
			}
		}
		else
		{
			LOG->RecordFailure("Could not activate ClassView.");
		}
	}
	else
	{
		LOG->RecordFailure("Could not activate project workspace window.");
	}
	return bResults;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIOGalleryDlg::AddToGallery()
// Description: Creates ogx from selected class in classview by selecting Add to Gallery
// context menu item.  This function assumes a class is already selected in ClassView.
// Return: None
// Param: None
// END_HELP_COMMENT
void UIOGalleryDlg::AddToGallery() 
{
	MST.DoKeys("+({F10})");		// Bring up the popup context menu.
	CString strGotoMenuItem = GetLocString(IDSS_WRKSPC_ADDTOGALLERY);
	if (MST.WMenuExists(strGotoMenuItem))
	{
		MST.DoKeys("{DOWN 2}") ; //need to do this to get focus on context menu
		MST.WMenu(strGotoMenuItem);
		Sleep(2000);
	}
	else
	{
		LOG->RecordFailure("Add to Gallery menu item doesn't exist.");
	}
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIOGalleryDlg::AddToProject(CString FolderName, CString FileName)
// Description: Selects file from gallery and adds it to current project.
// Return: None
// Param: Name of the class to create ogx from.
// END_HELP_COMMENT
void UIOGalleryDlg::AddToProject(CString FolderName, CString FileName, CString FullFileName)
{
	if (FindFileInGallery(FolderName, FullFileName) == 1)  
	{
		
		//MST.WButtonClick(GetLocString(IDS_INSERTCOMP));
		DoKeys("{ENTER}");
		HWND DlgWnd = MST.WFndWndWaitC(FileName,"STATIC",FW_PART,60);
		//if confirmation dialog comes up
		if (DlgWnd != 0)
		{
			//LOG->RecordInfo("Clicking OK");
			MST.WButtonClick(GetLocString(IDSS_OK));
			//LOG->RecordInfo("WaitUntilGone");
			WaitUntilGone(4000);
			//LOG->RecordInfo("Clicking yes");
			MST.WButtonClick(GetLocString(IDSS_YES));
			//LOG->RecordInfo("WaitUntilGone");
			WaitUntilGone(4000);
		}
		else
		{
			LOG->RecordFailure("Insert confirmation dialog didn't display.");
		}
	}
	else
	{
		LOG->RecordFailure("Did not select file in Gallery properly.");
	}
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIOGalleryDlg::DeleteFileInGallery(CString FolderName, CString FullFileName, CString OgxFileName)
// Description: Deletes a file in the gallery.  Cleanup for gallery sniff.
// Return: BOOL
// Param: None
// END_HELP_COMMENT
BOOL UIOGalleryDlg::DeleteFileInGallery(CString FolderName, CString FullFileName, CString OgxFileName) 

{
	BOOL bResult;
	int FileFound = FindFileInGallery(FolderName, FullFileName);
	if (FileFound == 1)  
	{
		Sleep(5000);
		MST.DoKeys("{DELETE}");
		Sleep(5000);
		CString str;
		MST.WGetText(NULL, str);

		//verify delete confirmation window is up
		if (str.Find(GetLocString(IDSS_DELETE_TITLE)) != -1)
		{
			//verify correct file is going to be deleted
			if (MST.WFndWndWaitC(OgxFileName,"STATIC",FW_PART,60) != 0)
			{
				DoKeys("{ENTER}");
				//can't use yes and no text because the delete dlg is a system
				//dialog - will get localized when OS is localized but when VC
				//still English
				//MST.WButtonClick(GetLocString(IDSS_YES));
				WaitUntilGone(4000);
				bResult = TRUE;
			}
			else
			{
				DoKeys("{TAB}");
				DoKeys("{ENTER}");
				//MST.WButtonClick(GetLocString(IDSS_NO));
				WaitUntilGone(4000);
				LOG->RecordFailure("Almost deleted wrong file.");
				bResult = FALSE;
			}
		}
		else
		{
			LOG->RecordFailure("Delete confirmation dialog did not come up.");
			bResult = FALSE;
		}
	}
	else  //FindFileInGallery returns something different then 1
	{
		//0 means no file found, 3 means no project folder found
		if (FileFound == 0 || FileFound == 3)
		{
			bResult = TRUE;
		}
		else //FindFileInGallery returns 2 or 4
		{
			LOG->RecordFailure("Error trying to find OGX file to delete.");
			bResult = FALSE;
		}
	}
	Sleep(3000);
	Close();
	return bResult;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIOGalleryDlg::LookIn(LPCSTR szLocation)
// Description: Sets the "Look In" field of the gallery dlg. The gallery dlg must be open.
// Param: LPCSTR szLocation - a string specifying the folder to look in.
// Return: BOOL - TRUE if the location was successfully changed. FALSE if not.
// END_HELP_COMMENT
BOOL UIOGalleryDlg::LookIn(LPCSTR szLocation)
{
	
	// gallery dlg must be active.
	if(!IsActive())
	{
		LOG->Comment("ERROR! UIOGalleryDlg::LookIn - gallery dlg not active");
		return FALSE;
	}

	// calls the base class to set the path (UIFileDlg::SetPath).
	SetPath(szLocation);
	// get the handle of dlg that is active after waiting 1 second.
	Sleep(1000);
	HWND hwnd = MST.WGetActWnd(0);

	// if the handle doesn't match that for the gallery dlg, an unexpected dlg came up.
	if(hwnd != HWnd())
	{
		// check if the "file/folder does not exist" dlg came up.
		if(MST.WFndWndC(GetLocString(IDSS_OG_FILE_NOT_EXIST), "STATIC", FW_PART))
			LOG->Comment("ERROR! UIOGalleryDlg::LookIn - %s does not exist.", szLocation);
		else
			LOG->Comment("ERROR! UIOGalleryDlg::LookIn - unknown error");

		// hopefully this will cancel whatever dlg came up.
		MST.DoKeys("{ESC}");
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: int UIOGalleryDlg::SelectComponent(LPCSTR szComp)	
// Description: Selects a component from the gallery dialog. The component must be showing in the list.
// Param: LPCSTR szComp - The name of the component to select.
// Return: BOOL - Returns TRUE if the component was successfully selected, FALSE if not.
// END_HELP_COMMENT
BOOL UIOGalleryDlg::SelectComponent(LPCSTR szComp)	

{
	// gallery dlg must be active
	if(!IsActive())
	{
		LOG->Comment("ERROR! UIOGalleryDlg::SelectComponent - gallery dlg not active");
		return FALSE;
	}

	// make sure the component list box exists and is enabled.
	if(MST.WViewEnabled("@1")==FALSE)
	{
		LOG->Comment("ERROR! UIOGalleryDlg::SelectComponent - component list not enabled");
		return FALSE;
	}
//TODO: Remove temp test
	char lpszBuffer[128];
	::WViewGetClass(lpszBuffer);
	int count=MST.WViewCount("@1");
	LOG->Comment("Class ListView (%s) count: %d",lpszBuffer, count);
//End of temp test
	// give the component list focus.
	MST.WViewSetFocus     ("@1");
	MST.WViewItemClk("@1", szComp);
//TODO: Remove after having investigated why it does not work
	int result=MST.WViewSelItem("@1", szComp);
	LOG->Comment("Result:(%d) of WViewSelItem, Item:(%s)", result, szComp);
//End of investigation

	CString strCompCurrent;
	MST.DoKeys("{TAB}");			// tab to the edit field (can't read with WEditText)
	MST.DoKeys("^(c)");				// copy contents to the clipboard
	GetClipText(strCompCurrent);	// grab the clipboard
		
	if(strCompCurrent == szComp)	// see if we have a match
		return TRUE;

	LOG->Comment("ERROR! UIOGalleryDlg::SelectComponent - could not find component %s", szComp); 	 
	return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIOGalleryDlg::Insert(LPCSTR szComponent)	
// Description: Inserts the currently selected component from the gallery dlg and confirms the insertion.
// Param: LPCSTR szComponent - The name of the component being inserted.
// Return: BOOL - TRUE if the insert operation was sucessful (at which time the component-specific wizard dlg will be active, if one exists), FALSE if not.
// END_HELP_COMMENT
BOOL UIOGalleryDlg::Insert(LPCSTR szComponent)	

{
	// gallery dlg must be active.
	if(!IsActive())
	{
		LOG->Comment("ERROR! UIOGalleryDlg::Insert - gallery dlg not active");
		return FALSE;
	}

	// expected text of Insert button.
	CString strInsert = GetLocString(IDSS_OG_INSERT);

	// make sure the insert button exists and is enabled.
	if(!MST.WButtonEnabled(strInsert))
	{
		LOG->Comment("ERROR! UIOGalleryDlg::Insert - %s button doesn't exist", strInsert);
		return FALSE;
	}
	
	// click the insert button.
	MST.WButtonClick(strInsert);
	UIDialog confirmDlg(GetLocString(IDSS_WORKBENCH_TITLEBAR));
	confirmDlg.AttachActive();
	confirmDlg.IsValid();
	// the confirmation dlg should contain this text. 
//	CString strConfirm = GetLocString(IDSS_OG_INSERT_CONFIRM1) + szComponent + GetLocString(IDSS_OG_INSERT_CONFIRM2);
//	if(!MST.WFndWndWaitC(strConfirm, "STATIC", FW_PART, 3))
	//REVIEW (chriskoz): The check above is to strict. Does not make sense for ActiveX controls.
	//Also, correct localization of "IDSS_OG_INSERT_CONFIRM1 + szComponent + IDSS_OG_INSERT_CONFIRM2" will be impossible
	//We'd better drop this confirm check but it's still here below:
	HWND hexist; //handle of the static control in the confirm dialog
	LOG->Comment("ISValid retrned:'%d'",confirmDlg.IsValid());
	if(//!confirmDlg.IsValid() ||
		!MST.WFndWndWaitC(GetLocString(IDSS_OG_INSERT_CONFIRM1), "STATIC", FW_ACTIVE | FW_PART, 3) ||
		!(hexist=MST.WFndWndWaitC(GetLocString(IDSS_OG_INSERT_CONFIRM2), "STATIC", FW_ACTIVE | FW_PART, 3)) )
	{
		LOG->Comment("ERROR! UIOGalleryDlg::Insert - no confirmation dlg");
		return FALSE;
	}

	CString strOK = GetLocString(IDSS_OK);

	// make sure the ok button exists and is enabled.
	if(!MST.WButtonEnabled(strOK))
	{
		LOG->Comment("ERROR! UIOGalleryDlg::Insert - %s button doesn't exist", strOK);
		return FALSE;
	}

	CString strConfirm;
	MST.WGetText(hexist, strConfirm);
	// confirm.
	confirmDlg.OK();

 	// wait up to 3 seconds for the confirmation dlg to go away.
	if(!MST.WFndWndWaitC(strConfirm, "STATIC", FW_PART | FW_NOEXIST, 3) )
	{
		LOG->Comment("ERROR! UIOGalleryDlg::Insert - confirmation dlg didn't go away");
		// OK didn't work, try hitting ESC to make it go away.
		MST.DoKeys("{ESC}");
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: CString UIOGalleryDlg::Description(void)	
// Description: Gets the description of the currently selected component in the gallery dlg.
// Return: CString - The component description.
// END_HELP_COMMENT
CString UIOGalleryDlg::Description(void)	

{
	// gallery dlg must be active
	if(!IsActive())
	{
		LOG->Comment("ERROR! UIOGalleryDlg::Insert - gallery dlg not active");
		return (CString)"";
	}

	CString description;
	MST.WStaticText("@4", description);
	return description;
}


