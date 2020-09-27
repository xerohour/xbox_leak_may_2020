///////////////////////////////////////////////////////////////////////////////
//	COSOURCE.CPP
//
//	Created by :			Date :
//		DavidGa					1/13/94
//
//	Description :
//		Implementation of the COSource component object class
//

#include "stdafx.h"
#include "cosource.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\sym\vshell.h"
#include "guiv1.h"
#include "..\..\coclip.h"
#include "..\shl\uwbframe.h"
#include "Strings.h"
#include "ufindrep.h"
#include "..\shl\uioutput.h"
#include "..\shl\ucustdlg.h"
#include "..\..\UMSGBOX.H"  // GAC added for MSG_OK

#define new DEBUG_NEW

#undef THIS_FILE

static char BASED_CODE THIS_FILE[] = __FILE__;

#define MUST_BE_ACTIVE_V() \
	if( !m_editor.IsActive() && !m_editor.Activate() ) \
	{ \
		LOG->RecordInfo("Cannot activate %s editor", GetFileName()); \
		return; \
	}

#define MUST_BE_ACTIVE_B() \
	if( !m_editor.IsActive() && !m_editor.Activate() ) \
	{ \
		LOG->RecordInfo("Cannot activate %s editor", GetFileName()); \
		return FALSE; \
	}

COSource::COSource()
{
}

// BEGIN_HELP_COMMENT
// Function: int COSource::Create(LPCSTR szSaveAs /*=NULL*/)
// Description: Create a new editor window and file in the IDE.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szSaveAs A pointer to a string that contains the name to save the new file with. NULL indicates don't save (no name specified). (The default value is NULL.)
// END_HELP_COMMENT
int COSource::Create(LPCSTR szSaveAs /*=NULL*/)
{
	return COFile::Create(IDSS_NEW_SOURCE, szSaveAs);
}

// BEGIN_HELP_COMMENT
// Function: int COSource::Open(LPCSTR szFileName, LPCSTR szChDir /*=NULL*/, EOpenAs oa /*=OA_AUTO*/)
// Description: Open a text file in the IDE.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szFileName A pointer to a string that contains the name of the file to open.
// Param: szChDir A pointer to a string that contains the path to the file. NULL if szFileName includes a path or szFileName is in the current directory). (The default value is NULL.)
// Param: oa A value that indicates the editor to use when opening the file: OA_AUTO, OA_TEXT, OA_BINARY, OA_MAKEFILE. (The default value is OA_AUTO.)
// END_HELP_COMMENT
int COSource::Open(LPCSTR szFileName, LPCSTR szChDir /*=NULL*/, EOpenAs oa /*=OA_AUTO*/)
{
	return COFile::Open(szFileName, szChDir, oa);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COSource::GetReadOnly(void)
// Description: Determine the read only state of this file.
// Return: A Boolean value that indicates the read only state: TRUE if read only; FALSE otherwise.
// END_HELP_COMMENT
BOOL COSource::GetReadOnly(void)
{
	return FALSE;		// STUB: GetProperty?
}

// BEGIN_HELP_COMMENT
// Function: int COSource::GetCurrentLine(void)
// Description: Get the current line in the text editor.
// Return: An integer containing the current line.
// END_HELP_COMMENT
int COSource::GetCurrentLine(void)
{
	return UIWB.GetEditorCurPos(GECP_LINE, m_editor);
}

// BEGIN_HELP_COMMENT
// Function: int COSource::GetCurrentColumn(void)
// Description: Get the current column in the text editor.
// Return: An integer containing the current column.
// END_HELP_COMMENT
int COSource::GetCurrentColumn(void)
{
	return UIWB.GetEditorCurPos(GECP_COLUMN, m_editor);
}

// BEGIN_HELP_COMMENT
// Function: CString COSource::GetSelectedText(void)
// Description: Get the selected text in the editor.
// Return: A CString containing the selected text.
// END_HELP_COMMENT
CString COSource::GetSelectedText(void)
{
	if( !m_editor.IsActive() && !m_editor.Activate() )
	{
		LOG->RecordInfo("Cannot activate %s editor", GetFileName());
		return CString("");
	}

	UIWB.DoCommand(ID_EDIT_COPY, DC_ACCEL);		// REVIEW: is there a better way?
	COClipboard clip;
	return clip.GetText();
}

// BEGIN_HELP_COMMENT
// Function: void COSource::SelectText(int line1, int column1, int line2, int column2, BOOL bColumnSelect /*=SELECT_NORMAL*/)
// Description: Select text in the source editor.
// Return: none
// Param: line1 An integer that contains the starting line for the selection.
// Param: column1 An integer that contains the starting column for the selection.
// Param: line2 An integer that contains the ending line for the selection.
// Param: column2 An integer that contains the ending column for the selection.
// Param: bColumnSelect A integer that indicates the type of selection to perform: SELECT_NORMAL, SELECT_COLUMN, SELECT_BRIEF_COLUMN. (The default value is SELECT_NORMAL.)
// END_HELP_COMMENT
void COSource::SelectText(int line1, int column1, int line2, int column2, BOOL bColumnSelect /*=SELECT_NORMAL*/)
{
	int ThisLine = 0;
	MUST_BE_ACTIVE_V();

	SetCursor(line1, column1);		// goto line1, column1

	char acBuf[16];
	if( line2 <= 0 )		// <0 means last line of the file
		strcpy(acBuf, "^+{end}");
	else if( line2 > line1 )				// select to line2
		wsprintf(acBuf, "+{down %d}", line2 - line1);
	else if( line2 < line1 )
		wsprintf(acBuf, "+{up %d}", line1 - line2);

	// Enter column mode?
	if (bColumnSelect == SELECT_COLUMN)
		MST.DoKeys("^+{F8}"); 
	else if (bColumnSelect == SELECT_BRIEF_COLUMN)
		MST.DoKeys("%C");
		 
	if( line2 != line1 )
		MST.DoKeys( acBuf );

	if( column2 <= 0 )		// <0 means end of line
	{
		MST.DoKeys("+{end}");
		return;
	}
	else if( column2 > column1 )			// select to column2
		wsprintf(acBuf, "+{right %d}", column2 - column1);
	else if( column2 < column1 )
		wsprintf(acBuf, "+{left %d}", column1 - column2);
	if( column2 != column1 )
		MST.DoKeys( acBuf );

	while( GetCurrentColumn() > column2 )	// adjust for tabs
		MST.DoKeys("+{left}");
	ThisLine = GetCurrentLine();  // To stop runaway right keys
	while(( GetCurrentColumn() < column2 ) && ( GetCurrentLine() == ThisLine ))
		MST.DoKeys("+{right}");
	if ( GetCurrentLine() != ThisLine ) 
		LOG->RecordInfo("Column %d not available",column2);
}


// BEGIN_HELP_COMMENT
// Function: void COSource::SelectLines(int startline /* 0 */, int count /* 1 */)
// Description: Select one or more lines in the source editor.
// Return: none
// Param: startline An integer that contains the starting line of the block to select (0 represents the current line).
// Param: count An integer that contains the number of lines to select.
// END_HELP_COMMENT
void COSource::SelectLines(int startline /* 0 */, int count /* 1 */)

{
	// 0 represents the current line.
	if(startline > 0)
		GoToLine(startline);
	
	// hit the HOME key until we are in column 1 (with indentation it shouldn't take more than twice).
	while(GetCurrentColumn() != 1)
		MST.DoKeyshWnd(m_editor.HWnd(), "{HOME}");

	// select the line(s).
	for(int i = 0; i < count; i++)
		MST.DoKeyshWnd(m_editor.HWnd(), "+({DOWN})");
}


// BEGIN_HELP_COMMENT
// Function: void COSource::SetCursor(int line, int column)
// Description: Set the caret to the given line and column in the source editor.
// Return: none
// Param: line An integer containing the line to set the caret to.
// Param: column An integer containing the column to set the caret to.
// END_HELP_COMMENT
void COSource::SetCursor(int line, int column)
{
	int ThisLine = 0;

	MUST_BE_ACTIVE_V();

	if( line <= 0 )
        MST.DoKeys("^{end}{home}");    // beginning of last line in file
	else
		GoToLine(line);

	char acBuf[16] = "{end}";		// end of line
	if( column <= 0 )
	{
		MST.DoKeys(acBuf);
		return;
	}

	wsprintf(acBuf, "{right %d}", column - 1);	// or to a specific column
	MST.DoKeys(acBuf);

	while( GetCurrentColumn() > column )	// adjust for tabs
		MST.DoKeys("{left}");
	ThisLine = GetCurrentLine();  // To stop runaway right keys
	while(( GetCurrentColumn() < column ) && ( GetCurrentLine() == ThisLine ))
		MST.DoKeys("{right}");
	if ( GetCurrentLine() != ThisLine ) 
		LOG->RecordInfo("Column %d not available",column);
}

// BEGIN_HELP_COMMENT
// Function: void COSource::InsertText(LPCSTR szText, int line, int column, BOOL bLiteral /*=FALSE*/)
// Description: Insert the given text at the given line and column in the source editor.
// Return: none
// Param: szText A pointer to a string that contains the text to insert into the source editor.
// Param: line An integer that contains the line where the text should be inserted.
// Param: column An integer that contains the column where the text should be inserted.
// Param: bLiteral A Boolean value that indicates whether the string (szText) should be interpreted literally (TRUE) or following MS-Test's DoKeys intepretation. (The default value is FALSE.)
// END_HELP_COMMENT
void COSource::InsertText(LPCSTR szText, int line, int column, BOOL bLiteral /*=FALSE*/)
{
	MUST_BE_ACTIVE_V();

	SetCursor(line, column);
	MST.DoKeyshWnd(m_editor.HWnd(), szText, bLiteral);
}

// BEGIN_HELP_COMMENT
// Function: void COSource::TypeTextAtCursor(LPCSTR szText, BOOL bLiteral /*=FALSE*/)
// Description: Type the given text at the current caret location in the source editor.
// Return: none
// Param: szText A pointer to a string that contains the text to type at the caret location.
// Param: bLiteral A Boolean value that indicates whether the string (szText) should be interpreted literally (TRUE) or following MS-Test's DoKeys intepretation. (The default value is FALSE.)
// END_HELP_COMMENT
void COSource::TypeTextAtCursor(LPCSTR szText, BOOL bLiteral /*=FALSE*/)
{
	MUST_BE_ACTIVE_V();

	MST.DoKeyshWnd(m_editor.HWnd(), szText, bLiteral);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COSource::GoToLine(LPCSTR szLine)
// Description: Go to the given line in the source editor.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szLine A pointer to a string containing the line number to go to.
// Param: bCloseAfter A Boolean value that indicates if dialog should be closed after the command was implemented (TRUE).
// END_HELP_COMMENT
BOOL COSource::GoToLine(LPCSTR szLine, BOOL bCloseAfter /* TRUE */)
{
	UIWB.DoCommand(IDM_GOTO_LINE, DC_MNEMONIC);
	UIDialog gd(GetLocString(IDSS_GOTO_TITLE));
	if( !gd.WaitAttachActive(10000) )
	{
		LOG->RecordInfo("Could not open the Goto dialog");
		return FALSE;
	}

	MST.DoKeyshWnd(gd.HWnd(), szLine);
	Sleep(500);
	MST.DoKeyshWnd(gd.HWnd(), "{ENTER}");

	if(bCloseAfter)
	{
		gd.Close();
		gd.WaitUntilGone(1000);
//		UIWB.DoCommand(IDM_CURSOR_BEGINLINE, DC_MNEMONIC);
		MST.DoKeys("{ESC}");
	}

	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL COSource::GoToLine(int nLine)
// Description: Go to the given line in the source editor.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: nLine An integer containing the line number to go to.
// Param: bCloseAfter A Boolean value that indicates if dialog should be closed after the command was implemented (TRUE).
// END_HELP_COMMENT
BOOL COSource::GoToLine(int nLine, BOOL bCloseAfter /* TRUE */)
{
	char szLine[7];
	_itoa(nLine, szLine, 10);
	return GoToLine(szLine, bCloseAfter);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COSource::Find(LPCSTR szFind, BOOL bMatchWord, BOOL bMatchCase, BOOL bRegExpr, int fd)
// Description: Find the given string (szFind) in this source editor window.
// Return: TRUE if successful; FALSE otherwise.
// Param: szFind A pointer to a string containing the text to find in this window.
// Param: bMatchWord A Boolean value that indicates how to match words: TRUE to only find whole words; FALSE to find partial words. (Default value is FALSE.)
// Param: bMatchCase A Boolean value that indicates how to match case: TRUE to match the case exactly; FALSE to ignore case. (Default value is FALSE.)
// Param: bRegExpr A Boolean value that indicates whether the find string is a regular expression (TRUE) or not. (Default value is FALSE.)
// Param: fd A value that indicates the search direction: FD_DOWN to search toward the end of the file; FD_UP to search toward the top of the file. (Default value is FD_DOWN.)
// END_HELP_COMMENT
BOOL COSource::Find(LPCSTR szFind, BOOL bMatchWord /*= FALSE*/, BOOL bMatchCase /*= FALSE*/, BOOL bRegExpr /*= FALSE*/, int fd /*= FD_DOWN*/)
{
	UIFindDlg uiFind;

	//REVIEW: (michma)
	// requiring an active editor that UIEditor knows about excludes
	// files that are loaded via the debugger.  but debugger tests
	// need this Find function. recommendation is to enhance CODebug
	// and COSource classes to automatically maintain UIEditor objects
	// for any source files loaded while debugging.  temporarily we will
	// activate the editor if it exists , but we won't require a valid editor
	// to use this function.
	//MUST_BE_ACTIVE_B();
	m_editor.Activate();

	// open the find dialog
	uiFind.Activate();

	// set the find string
	uiFind.SetFindString(szFind);
	// set match word
	uiFind.SetMatchWord(bMatchWord);
	// set match case
	uiFind.SetMatchCase(bMatchCase);
	// set regular expression
	uiFind.SetRegularExpression(bRegExpr);
	// set find direction
	uiFind.SetDirection(fd == FD_UP ? UIFindDlg::FD_UP : UIFindDlg::FD_DOWN);

	// do the find
	uiFind.FindNext();

	// must look for "OK" button string rather than ctrl id because
	// while debugging there apparently is another control with the same id
	// which causes this function to incorrectly fail. (michma)
	if(MST.WFndWndWaitC(GetLocString(IDSS_OK), "Button", FW_PART | FW_ACTIVE, 3))
		{     
		if(MST.WFndWndC(GetLocString(IDSS_NOT_FOUND), "Static", FW_PART )) 
			LOG->RecordInfo("Could not find %s ",szFind);

		MST.DoKeys("{ESC}") ;  // get rid of OK message
		MST.DoKeys("{ESC}") ;  // get rid of menu
		return(FALSE);  // move on to next test
		}

	else
		return (TRUE);

}

// BEGIN_HELP_COMMENT
// Function: int COSource::Replace(LPCSTR szFind, LPCSTR szReplace, BOOL bMatchWord, BOOL bMatchCase, BOOL bRegExpr, COSource::ReplaceScope rs, BOOL bAll /*= FALSE*/)
// Description: Find (szFind) and replace (szReplace) in this source editor window.
// Return: An integer that specifies the number of occurences that were replaced by the operation.
// Param: szFind A pointer to a string containing the text to find in this window.
// Param: szReplace A pointer to a string containing the text to replace the found string within this window.
// Param: bMatchWord A Boolean value that indicates how to match words: TRUE to only find whole words; FALSE to find partial words. (Default value is FALSE.)
// Param: bMatchCase A Boolean value that indicates how to match case: TRUE to match the case exactly; FALSE to ignore case. (Default value is FALSE.)
// Param: bRegExpr A Boolean value that indicates whether the find string is a regular expression (TRUE) or not. (Default value is FALSE.)
// Param: rs A value that indicates the scope of the replace operation: COSource::RS_WHOLEFILE to search and replace across the entire file; COSource::RS_SELECTION to search and replace within the selection. (Default value is COSource::RS_WHOLEFILE.)
// Param: bAll A Boolean value that indicates whether to replace all occurences in the file/selection (TRUE) or just the first one. (Default value is FALSE.)
// END_HELP_COMMENT
int COSource::Replace(LPCSTR szFind, LPCSTR szReplace, BOOL bMatchWord /*= FALSE*/, BOOL bMatchCase /*= FALSE*/, BOOL bRegExpr /*= FALSE*/, COSource::ReplaceScope rs /*= RS_WHOLEFILE*/, BOOL bAll /*= FALSE*/)
{
	UIReplaceDlg uiReplace;

	MUST_BE_ACTIVE_B();

	// open the replace dialog
	uiReplace.Activate();

	// set the find string
	uiReplace.SetFindString(szFind);
	// set the replace string
	uiReplace.SetReplaceString(szReplace);
	// set match word
	uiReplace.SetMatchWord(bMatchWord);
	// set match case
	uiReplace.SetMatchCase(bMatchCase);
	// set regular expression
	uiReplace.SetRegularExpression(bRegExpr);
	// set replace scope
	uiReplace.SetReplaceScope(rs == RS_WHOLEFILE ? UIReplaceDlg::RS_WHOLEFILE : UIReplaceDlg::RS_SELECTION);

	// are replacing one occurence or all of them
	if (bAll) {
		// replace all
		int nReplaced = uiReplace.ReplaceAll();
		// close the dialog
		uiReplace.Close();
		return nReplaced;
	}
	else {
		// find the next occurance
		uiReplace.FindNext();
		// do the replace
		uiReplace.Replace();
		// close the dialog
		uiReplace.Close();
		return 1;
	}
}


// BEGIN_HELP_COMMENT
// Function: SelectEmulations(int EMULATE_EDITOR)
// Description: Select Editor to emulate from Tools.Options.Compatibility
// Return: TRUE if successful; FALSE otherwise.
// Param: EMULATE_EDITOR The editor to emulate from: EMULATE_VC3, EMULATE_VC2, EMULATE_BRIEF, EMULATE_EPSILON, EMULATE_CUSTOM 
// END_HELP_COMMENT
BOOL COSource::SelectEmulations(EmulationType eEM)
{

// REVIEW we need a UIDialog Class for this	
	UIOptionsTabDlg OptionsTools;
	OptionsTools.ShowPage("Compatibility");
	OptionsTools.ChangeCompatility(eEM) ;
	return TRUE;
}
