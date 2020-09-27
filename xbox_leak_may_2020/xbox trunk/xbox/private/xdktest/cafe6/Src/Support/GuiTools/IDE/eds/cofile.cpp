///////////////////////////////////////////////////////////////////////////////
//	COFILE.CPP
//
//	Created by :			Date :
//		DavidGa					1/20/94
//
//	Description :
//		Implementation of the COFile component object class
//

#include "stdafx.h"
#include "cofile.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "guiv1.h"
#include "guitarg.h"
#include "..\..\udialog.h"
#include "..\..\testutil.h"
#include "..\shl\uiwbmsg.h"
#include "..\shl\uwbframe.h"
#include "..\shl\cowrkspc.h"
#include "..\shl\wbutil.h"
#include "..\shl\newfldlg.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

COFile::COFile()
{
}


// BEGIN_HELP_COMMENT
// Function: int COFile::AttachActive(void)
// Description: Attach an editor to the active file.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int COFile::AttachActive(void)
{
	return m_editor.AttachActive() ? ERROR_SUCCESS : ERROR_ERROR;
}

	
// BEGIN_HELP_COMMENT
// Function: int COFile::Create(UINT idsType, LPCSTR szSaveAs /*=NULL*/, LPCSTR szProject /* NULL */)
// Description: Create a new file and corresponding editor in the IDE.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: idsType The type of file and editor to create: IDSS_NEW_SOURCE, IDSS_NEW_PROJECT, IDSS_NEW_RC, IDSS_NEW_RCT, IDSS_NEW_BINARY, IDSS_NEW_BITMAP, IDSS_NEW_ICON.
// Param: szSaveAs A pointer to a string that contains the name to save the new file with. NULL indicates don't save (no name specified). (The default value is NULL.)
// Param: szProject A pointer to a string that contains the name of the project to add the file to. NULL indicates don't add it to project. (The default value is NULL.)
// END_HELP_COMMENT
int COFile::Create(UINT idsType, LPCSTR szSaveAs /*=NULL*/, LPCSTR szProject /* NULL */)
{

	HWND hwnd = UIWB.GetActiveEditor();

	// TODO (michma): we temporarily handle adding new files to projects in a separate block. this function shouldn't
	// be calling UIWB.CreateNewFile at all, but since it has been doing so for creating files without adding them to
	// projects, we'll leave it for now. i've added a block to handle adding new files to projects below.
	if(szProject)
	
	{
		UINewFileDlg uiNewFileDlg ;
		
		// TODO (michma): add error checking for each action.
		uiNewFileDlg.Display() ;
		uiNewFileDlg.NewFileTab() ;
		uiNewFileDlg.SelectFromList(GetLocString(idsType));
		uiNewFileDlg.AddToProject(TRUE);
		uiNewFileDlg.SetProject(szProject);
		uiNewFileDlg.SetName(szSaveAs, FILES);

		MST.WButtonClick(GetLabel(IDOK));
		uiNewFileDlg.WaitUntilGone(2000) ;
  		WaitForInputIdle(g_hTargetProc, 10000);
	}

	else
		UIWB.CreateNewFile(GetLocString(idsType)) ;
	
	int k = 0;

	do
	{
		m_editor = UIWB.GetActiveEditor();	// wait for a new editor
		if( (k++ > 4) && (m_editor == hwnd) )
		{
			// TODO(michma): we are failing to get a new editor in some cases, but not sure why.
			// one example: when the sys acceptance test creates a new html page and adds it to
			// an activex control project.
			break;
			m_editor.Detach();
			return ERROR_ERROR;
		}
		Sleep(1000);
	} while( m_editor == hwnd );

	if( !m_editor.IsValid() )
		return ERROR_ERROR;
		
	// if the file was added to a project, it has already been named, and the user can save it later.
	if(szProject)
		return ERROR_SUCCESS;

	// give the new file a name and save it.
	if(szSaveAs == NULL)
		return ERROR_SUCCESS;
	else
		return SaveAs(szSaveAs);
}


// BEGIN_HELP_COMMENT
// Function: int COFile::Open(LPCSTR szFileName, LPCSTR szChDir /*=NULL*/, EOpenAs oa /*=OA_AUTO*/)
// Description: Open a file and its corresponding editor in the IDE.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szFileName A pointer to a string that contains the name of the file to open.
// Param: szChDir A pointer to a string that contains the path to the file. NULL if szFileName includes a path or szFileName is in the current directory). (The default value is NULL.)
// Param: oa A value that indicates the editor to use when opening the file: OA_AUTO, OA_TEXT, OA_BINARY, OA_MAKEFILE. (The default value is OA_AUTO.)
// END_HELP_COMMENT
int COFile::Open(LPCSTR szFileName, LPCSTR szChDir /*=NULL*/, EOpenAs oa /*=OA_AUTO*/)
{
	const char* const THIS_FUNCTION = "COFile::Open";

	// REVIEW(michma, 10/26/97): is this necessary? i can see a reason for wanting to point
	// a COSource object at different files for different times, and no Detach function is provided.
	//ASSERT( !m_editor.IsValid() );		// new files go into new editors, not existing ones.

	UIFileOpenDlg ufod = UIWB.FileOpenDlg();
	if (!ufod.IsValid()) {
		LOG->RecordInfo("%s: unable to open the File Open dialog (UIFileOpenDlg::IsValid failed).", THIS_FUNCTION);
		return ERROR_ERROR;
	}

    if( szChDir != NULL )
        ufod.SetPath(szChDir);
    ufod.SetName(szFileName);
	ufod.SetEditor(oa);

	if (!ufod.OK(FALSE)) {
		LOG->RecordInfo("%s: unable to close the File Open dialog with the OK button (UIFileOpenDlg::OK failed).", THIS_FUNCTION);
		return ERROR_ERROR;
	}

	UIWBMessageBox mb;
	while( mb.WaitAttachActive(3000) != NULL )
	{
		if( mb.ButtonExists(MSG_NO) )
			// this will handle "do you want to save" msg box
			mb.ButtonClick(MSG_NO);
	}

	ufod.WaitUntilGone(1500);	// the only files that take a while are RC, which causes the whole system to wait while it compiles

	if (WaitForInputIdle(g_hTargetProc, 60000) == WAIT_TIMEOUT)
		LOG->RecordFailure("File took more than 60 seconds to load");

	
	// Get a pointer to the filename only; i.e. remove any path prefix.	
	const char *p = szFileName + strlen(szFileName) - 1;
	while (*(p-1) != '\\' && p != szFileName)
		p--;

	// If the new file pops up as a separate, MDI window....
	if (MST.WFndWnd(p, FW_ACTIVE | FW_PART | FW_CHILDOK | FW_NOCASE))
	{
		m_editor.AttachActive();
		m_strFileName = szFileName;

		if( m_editor.IsValid() )
			return ERROR_SUCCESS;
		else
			return ERROR_ERROR;
	}
	else
	{
		LOG->RecordInfo("Assuming %s is opened in Workspace window", szFileName);
		return ERROR_SUCCESS;
	}
}

// BEGIN_HELP_COMMENT
// Function: int COFile::Save(void)
// Description: Save the file.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int COFile::Save(void)
{
	const char* const THIS_FUNCTION = "COFile::Save";

	if (!m_editor.IsValid()) {
		LOG->RecordInfo("%s: the editor for the current file '%s' is not valid (UIEditor::IsValid failed).", THIS_FUNCTION, m_editor.GetTitle());
		return ERROR_ERROR;
	}
	if (!m_editor.Activate()) {
		LOG->RecordInfo("%s: cannot activate the editor for the current file '%s' (UIEditor::Activate failed).", THIS_FUNCTION, m_editor.GetTitle());
		return ERROR_ERROR;
	}

	UIWB.DoCommand(ID_FILE_SAVE, DC_MNEMONIC);
	// REVIEW: handle any problems
	// REVIEW: verify that 'dirty' marker is gone from titlebar?
	return ERROR_SUCCESS;
}

// BEGIN_HELP_COMMENT
// Function: int COFile::SaveAs(LPCSTR szFileName, BOOL bOverWrite /*=FALSE*/)
// Description: Save the file with the name given in szFileName.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szFileName A pointer to a string that contains the filename to use.
// Param: bOverWrite A Boolean value that indicates whether to overwrite an existing file (TRUE) or not. (The default value is FALSE.)
// END_HELP_COMMENT
int COFile::SaveAs(LPCSTR szFileName, BOOL bOverWrite /*=FALSE*/)
{
	const char* const THIS_FUNCTION = "COFile::SaveAs";

	if (!m_editor.IsValid()) {
		LOG->RecordInfo("%s: the editor for the current file '%s' is not valid (UIEditor::IsValid failed).", THIS_FUNCTION, m_editor.GetTitle());
		return ERROR_ERROR;
	}
	if (!m_editor.Activate()) {
		LOG->RecordInfo("%s: cannot activate the editor for the current file '%s' (UIEditor::Activate failed).", THIS_FUNCTION, m_editor.GetTitle());
		return ERROR_ERROR;
	}

	UIFileSaveAsDlg ufsad = UIWB.FileSaveAsDlg();
	if (!ufsad.IsValid()) {
		LOG->RecordInfo("%s: unable to open the File Save As dialog (UIFileSaveAsDlg::IsValid failed).", THIS_FUNCTION);
		return ERROR_ERROR;
	}

    ufsad.SetName(szFileName);

    ufsad.OK(FALSE);
	int k = 0;
	while( !ufsad.WaitUntilGone(0) )
	{
		UIWBMessageBox umb;
		if( umb.WaitAttachActive(1000) )
		{
			if( umb.ButtonExists( MSG_NO ) )		// assume yes/no options are "Overwrite?" questions
			{
				if( bOverWrite )
					umb.ButtonClick( MSG_YES );
				else {
					umb.ButtonClick( MSG_NO );
					ufsad.Cancel();
			}
			}
			else
				MST.DoKeys("~");		// answer any other messageboxes with the default	// REVIEW
		}
		else
		{
			if( !MST.WButtonFocus(GetLabel(IDOK)) )		// if the Save As dialog is still active, but
				return ERROR_ERROR;						// the OK button doesn't have focus, something went wrong
		}
	}
	
	m_strFileName = szFileName;
	return ERROR_SUCCESS;
}

// BEGIN_HELP_COMMENT
// Function: int COFile::Close(BOOL bSaveChanges /* = FALSE */)
// Description: Close the file and its associated editor.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: bSaveChanges A Boolean value that indicates whether to save changes (TRUE) or not before closing the file. (The default value is FALSE.)
// END_HELP_COMMENT
int COFile::Close(BOOL bSaveChanges /* = FALSE */)
{
	const char* const THIS_FUNCTION = "COFile::Close";

	if (!m_editor.IsValid()) {
		LOG->RecordInfo("%s: the editor for the current file '%s' is not valid (UIEditor::IsValid failed).", THIS_FUNCTION, m_editor.GetTitle());
		return ERROR_ERROR;
	}
	if (!m_editor.Activate()) {
		LOG->RecordInfo("%s: cannot activate the editor for the current file '%s' (UIEditor::Activate failed).", THIS_FUNCTION, m_editor.GetTitle());
		return ERROR_ERROR;
	}

	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);
	
	UIMessageBox uiSaveAlert;
	if (uiSaveAlert.WaitAttachActive(500))
	{
		if (bSaveChanges)
			uiSaveAlert.ButtonClick(MSG_YES);	
		else
			uiSaveAlert.ButtonClick(MSG_NO);	
	}
	
	// wait for up to a second for the editor to close
	for (int i = 0; i < 10; i++) {
		if (!m_editor.IsValid()) {
			break;
		}
		Sleep(100);
	}
	if (i >= 10) {
		LOG->RecordInfo("%s: after closing and waiting 1 second, the editor is still valid for the current file '%s' (UIEditor::IsValid succeeded).", THIS_FUNCTION, m_editor.GetTitle());
		return ERROR_ERROR;
	}
	else {
		return ERROR_SUCCESS;
	}
}

// BEGIN_HELP_COMMENT
// Function: LPCSTR COFile::GetFileName(void)
// Description: Get the filename of this file.
// Return: A pointer to a string that contains the filename of this file.
// END_HELP_COMMENT
LPCSTR COFile::GetFileName(void)
{
	return m_strFileName;	// STUB
}

// BEGIN_HELP_COMMENT
// Function: LPCSTR COFile::Cut(CString* pstr /*=NULL*/)
// Description: Perform a cut action in the editor associated with this file.
// Return: A pointer to a string that contains the text placed in the clipboard as a result of the cut.
// Param: pstr A pointer to a CString that will contain the text placed in the clipboard as a result of the cut operation. If this parameter is NULL, the return value will also be NULL. (The default value is NULL.)
// END_HELP_COMMENT
LPCSTR COFile::Cut(CString* pstr /*=NULL*/)
{
	UIWB.DoCommand(ID_EDIT_CUT, DC_ACCEL);		// should work anywhere
	if( pstr == NULL )
		return NULL;
	GetClipText(*pstr);		// if test wants string, they must provide the buffer
	return *pstr;
}

// BEGIN_HELP_COMMENT
// Function: LPCSTR COFile::Copy(CString* pstr /*=NULL*/)
// Description: Perform a copy operation in the editor associated with this file.
// Return: A pointer to a string that contains the text placed in the clipboard as a result of the cut.
// Param: pstr A pointer to a CString that will contain the text placed in the clipboard as a result of the cut operation. If this parameter is NULL, the return value will also be NULL. (The default value is NULL.)
// END_HELP_COMMENT
LPCSTR COFile::Copy(CString* pstr /*=NULL*/)
{
	UIWB.DoCommand(ID_EDIT_COPY, DC_ACCEL);		// should work anywhere
	if( pstr == NULL )
		return NULL;
	GetClipText(*pstr);
	return *pstr;
}

// BEGIN_HELP_COMMENT
// Function: void COFile::Paste(void)
// Description: Perform a paste operation in the editor associated with this file.
// Return: none
// END_HELP_COMMENT
void COFile::Paste(void)
{
	UIWB.DoCommand(ID_EDIT_PASTE, DC_ACCEL);		// should work anywhere
}

// BEGIN_HELP_COMMENT
// Function: void COFile::Delete(void)
// Description: Perform a delete operation in the editor associated with this file.
// Return: none
// END_HELP_COMMENT
void COFile::Delete(void)
{
	UIWB.DoCommand(ID_EDIT_CLEAR, DC_ACCEL);    // should work anywhere
}

// BEGIN_HELP_COMMENT
// Function: void COFile::Undo(void)
// Description: Perform an undo operation in the editor associated with this file.
// Return: none
// END_HELP_COMMENT
void COFile::Undo(void)
{
	UIWB.DoCommand(ID_EDIT_UNDO, DC_ACCEL);		// should work anywhere
}

// BEGIN_HELP_COMMENT
// Function: void COFile::Redo(void)
// Description: Peform a redo operation in the editor associated with this file.
// Return: none
// END_HELP_COMMENT
void COFile::Redo(void)
{
	UIWB.DoCommand(ID_EDIT_REDO, DC_ACCEL);		// should work anywhere
}

