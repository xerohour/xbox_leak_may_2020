///////////////////////////////////////////////////////////////////////////////
//	UIMEM.CPP
//
//	Created by :			Date :
//		MichMa					1/14/94
//
//	Description :
//		Implementation of the UIMemory class
//

#include "stdafx.h"
#include "uimem.h"
#include "..\SYM\cmdids.h"
#include "..\SYM\reg.h"
#include "..\..\testutil.h"
#include "..\SHL\uwbframe.h"
#include "..\SHL\wbutil.h"
#include "..\SHL\uioptdlg.h"
#include <guiv1.h>
#include <mstwrap.h>

#include "..\..\udialog.h"
extern TARGET_DATA HANDLE g_hTargetProc;

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

int FormatWidth[14] = {1, 1, 3, 8, 6, 7, 13, 10, 12, 15, 24, 22, 18, 22};


// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::AttachActive(void)
// Description: Attach to the locals window if it's the active window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIMemory::AttachActive(void)
	{
	HWND hwnd;

    if(hwnd = MST.WFndWndWait(GetLocString(IDSS_MEM_WND_TITLE), FW_FULL, 2))
		return Attach(hwnd);
	else
		return Attach(FindFirstChild(UIWB.HWnd(), IDW_MEMORY_WIN));
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::Find(void)
// Description: Find the locals window and attach to it
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIMemory::Find(void)
	{
	int pkgId ;
	switch(GetID())
	{
	case 0:
		pkgId = 0 ;
		break ;
	case IDDW_PROJECT:
		pkgId = PACKAGE_VPROJ ;
		break;
	case IDW_LOCALS_WIN: 
		pkgId = PACKAGE_VCPP ;
		break ;
	default:
		break;
	}
	return Attach(UIWB.GetToolbar(GetID(),pkgId));
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::IsValid(void) const
// Description: Determine if the locals window is a valid window.
// Return: A Boolean that indicates whether the locals window is valid (TRUE) or not.
// END_HELP_COMMENT
BOOL UIMemory::IsValid(void) const
	{
	return UIWindow::IsValid();
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::IsActive(void)
// Description: Determine if the locals window is active.
// Return: A Boolean value that indicates whether the locals window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL UIMemory::IsActive(void)
	{
	HWND hwnd = MST.WGetFocus();
	return (hwnd == HWnd()) || IsChild(HWnd(), hwnd);
	}


// BEGIN_HELP_COMMENT
// Function: CString UIMemory::GetCurrentAddress(void)
// Description: Get the starting address of the first caret line in the memory window.
// Return: A CString that contains the address.
// END_HELP_COMMENT
CString UIMemory::GetCurrentAddress(void)
	{
	Activate();

	if(!IsActive()) return CString("");

	MST.DoKeys("{Enter}");					// Jump to memory window from the Address edit box.  If we already there, this command does nothing
	// In 60 ^{HOME} causes a jump to 00000000
	//MST.DoKeys("^{HOME}");					// Go to the first caret line in the memory window

	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and
	CString address;						// copy first 8 characters to clipboard
	GetClipText(address);
	return address;
	}


// BEGIN_HELP_COMMENT
// Function: CString UIMemory::GetCurrentData(int count /* 1 */, int start /* 0 */)
// Description: Get data from the current caret line in the memory window.
// Return: A CString that contains the data.
// Param: count An integer that contains the number of data elements to include when getting the data. (Default value is 1.)
// Param: start An integer that contains the number of data elements to skip from the beginning of the line. (Default value is 0.)
// END_HELP_COMMENT
CString UIMemory::GetCurrentData(int count /* 1 */, int start /* 0 */)

	{												 //TODO: handle getting chars from multiple lines
	if(!IsActive())
		{
		LOG->RecordInfo("ERROR in UIMemory::GetCurrentData(): Memory window not active");
		return CString("");
		}

	MST.DoKeys("{HOME}");		// set cursor at beginning of line
	int i;
	
	for(i = -1; i < start; i++)		// set cursor at beginning of start value
		MST.DoKeys("{TAB}");
			
	for(i = 0; i < ((FormatWidth[GetMemoryFormat()]) * count); i++)	// select count worth
		MST.DoKeys("+({RIGHT})");												// of chars for the			
							 													// current format width
	MST.DoKeys("^(c)");		// copy selection to clipboard
	CString data;		
	GetClipText(data);
	return data;
	}


// BEGIN_HELP_COMMENT
// Function: CString UIMemory::GetCurrentChars(int count /* 1 */, int start /* 0 */)
// Description: Get character data from the current caret line in the memory window.
// Return: A CString that contains the characters.
// Param: count An integer that contains the number of characters to include when getting the data. (Default value is 1.)
// Param: start An integer that contains the number of Characters to skip from the beginning of the line. (Default value is 0.)
// END_HELP_COMMENT
CString UIMemory::GetCurrentChars(int count /* 1 */, int start /* 0 */)
	
	{														//TODO: handle getting chars from multiple lines
	if(!IsActive()) return CString("");
	MST.DoKeys("{HOME}");		// set cursor at beginning of line
	int i;

	for(i = -1; i < (GetMemoryWidth() + start); i++)		// set cursor at start char
		MST.DoKeys("{TAB}");

	for(i = 0; i < count; i++)
		MST.DoKeys("+({RIGHT})");		// select count worth of chars			

	MST.DoKeys("^(c)");		// copy selection to clipboard
	CString chars;
	GetClipText(chars);
	return chars;
	}

	
// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::SetCurrentData(LPCSTR data, int start /* 0 */)
// Description: Set data in the current caret line in the memory window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: data A pointer to a string that contains the data to enter into the memory window.
// Param: start An integer that contains the number of data elements to skip from the beginning of the line before entering the data. (Default value is 0.)
// END_HELP_COMMENT
BOOL UIMemory::SetCurrentData(LPCSTR data, int start /* 0 */)		// TODO: handle data overlapping to next line
	
	{
	int i;

	if(!IsActive()) return FALSE;
	MST.DoKeys("{HOME}");		// set cursor at beginning of line
										
	for(i = -1; i < start; i++)		// set cursor at beginning of start value
		MST.DoKeys("{TAB}");

	// Put 0s to fit format
	while(data[i] == ' ')
		memset((void*)&data[i++], (int) '0',  1);
	
	MST.DoKeys(data);		// type new data (must be legal for current format)						


	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::SetCurrentChars(LPCSTR chars, int start /* 0 */)
// Description: Set character data in the current caret line in the memory window. (See also SetCurrentChars(char, int).)
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: chars A pointer to a string that contains the characters to enter into the memory window.
// Param: start An integer that contains the number of characters to skip from the beginning of the line before entering the data. (Default value is 0.)
// END_HELP_COMMENT
BOOL UIMemory::SetCurrentChars(LPCSTR chars, int start /* 0 */)		// TODO: handle chars overlapping to next line
	
	{
	if(!IsActive()) return FALSE;
	MST.DoKeys("{HOME}");		// set cursor at beginning of line
										
	for(int i = -1; i < (GetMemoryWidth() + start); i++)		// set cursor at start char
		MST.DoKeys("{TAB}");

	MST.DoKeys(chars);		// type new chars (must be legal for current format)						
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::SetCurrentChars(char chr, int start /* 0 */)
// Description: Set character data in the current caret line in the memory window. (See also SetCurrentChars(LPCSTR, int).)
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: chr A char that contains the character to enter into the memory window.
// Param: start An integer that contains the number of characters to skip from the beginning of the line before entering the data. (Default value is 0.)
// END_HELP_COMMENT
BOOL UIMemory::SetCurrentChars(char chr, int start /* 0 */)
	
	{
	if(!IsActive()) return FALSE;
	MST.DoKeys("{HOME}");		// set cursor at beginning of line
										
	for(int i = -1; i < (GetMemoryWidth() + start); i++)		// set cursor at start char
		MST.DoKeys("{TAB}");

	CString cstr = chr;
	MST.DoKeys(cstr);		// type new char (must be legal for current format)						
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: int UIMemory::GetFormatWidth(int format /* 0 */)
// Description: Get the width of a single data element in the specified memory window format.
// Return: An integer that contains the width of a single data element in the specified memory window format.
// Param: format A value that specifies the memory window format to get the width for: MEM_FORMAT_ASCII, MEM_FORMAT_WCHAR, MEM_FORMAT_BYTE, MEM_FORMAT_SHORT, MEM_FORMAT_SHORT_HEX, MEM_FORMAT_SHORT_UNSIGNED, MEM_FORMAT_LONG, MEM_FORMAT_LONG_HEX, MEM_FORMAT_LONG_UNSIGNED, MEM_FORMAT_REAL, MEM_FORMAT_REAL_LONG. 0 means get the width for the current format. (Default value is 0.)
// END_HELP_COMMENT
int UIMemory::GetFormatWidth(int format /* 0 */)
	{
	if(format)
		return FormatWidth[format];
	else
		return FormatWidth[GetMemoryFormat()];
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::Activate(void)
// Description: Bring up the memory window.
// Return: A Boolean value that indicates whether the memory window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL UIMemory::Activate(void)
	{
	UIWB.DoCommand(IDM_VIEW_MEM, DC_MNEMONIC);
	return AttachActive();
	}


// BEGIN_HELP_COMMENT
// Function: int UIMemory::GoToAddress(LPCSTR symbol)
// Description: Go to the specified address or symbol in the memory window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: symbol A pointer to a string that contains the address or symbol to go to in the memory window.
// END_HELP_COMMENT
int UIMemory::GoToAddress(LPCSTR symbol)
	
	{
	// Activate();  TODO: (dklem 11/25/98) remove this string if all snaps and sniffs pass

	if(!IsActive())
		{
		LOG->RecordInfo("ERROR in UIMemory::GoToAddress(): Memory window not active");
		return ERROR_ERROR;
		}

	CString cstr;
	if(MST.WEditExists(GetLocString(IDSS_MEMORY_ADDRESS_EDIT)))
	{
		MST.WEditSetFocus(GetLocString(IDSS_MEMORY_ADDRESS_EDIT));
		MST.WEditSetText("", symbol);
		MST.WEditText(GetLocString(IDSS_MEMORY_ADDRESS_EDIT), cstr);
		if(cstr != symbol)
		
			{
			LOG->RecordInfo("ERROR in UIMemory::GoToAddress(): Address field could not be set -"
						"expected text = %s, actual text = %s\n", symbol, cstr);
		
			return ERROR_ERROR;
			}

	}
	else
	{
		// Preserve the focus
		//Save Focus
		HWND hwndSaveFocus = UIWB.HWnd();
		//Set Focus to the Memory window
		UIWB.SetHWnd(MST.WGetFocus());
		::WaitForInputIdle(g_hTargetProc, 10000);
		UIWB.DoCommand(IDM_GOTO_LINE, DC_MNEMONIC);
		// Wait for the dialog
		UIDialog gd(GetLocString(IDSS_GOTO_TITLE));
		if( !gd.WaitAttachActive(10000) )
		{
			LOG->RecordInfo("Could not open the Goto dialog");
			return FALSE;
		}
		// Set desired address
		MST.WEditSetText("@1", symbol);
		// Restore Focus
		UIWB.SetHWnd(hwndSaveFocus);
	}
	
	// michma - 9/21/99: we're seeing timing problems where it appears that the ENTER key isn't taking affect, and
	// the cursor is left in the address field instead of being put into the data field. these sleeps might help.
	LOG->RecordInfo("doing 1000 ms sleep around {ENTER} to avoid timing problem.");
	Sleep(1000);
	MST.DoKeys("{ENTER}");
	Sleep(1000);

	// We need to close the GoTo dialog.  If it is not opened one extra ECS wouldn't hurt.  Would it ? dklem 11/11/98
	// Yes it hurts, so we need to find out if the GoTo dialog exists
	HWND hWndGoTo = MST.WFndWndWait("Go To", FW_NOCASE | FW_PART | FW_FOCUS, 1);  // Let's wait 1 second
	if(hWndGoTo)
	{
		// Hit Enter one more time if the address is invalid
		HWND hWndInvalidAddress = MST.WFndWndWait("Microsoft Visual C++", FW_NOCASE | FW_PART | FW_FOCUS, 1);  // Let's wait 1 second
		if(hWndInvalidAddress)
			MST.DoKeys("{ENTER}");

		// Close the dialog
		SetActiveWindow(hWndGoTo);
		MST.DoKeys("{ESC}");
	}
		
	UIWB.DoCommand(IDM_VIEW_MEM, DC_ACCEL);
	// Since the address of the symbol is always located in the left upper corner, we want to put hte cursor to this corner
	// For 60 this will set address to 0000000
	// MST.DoKeys("^{HOME}");
	return ERROR_SUCCESS;
	}

// BEGIN_HELP_COMMENT
// Function: void UIMemory::SetMemoryFormat(MEM_FORMAT format)	
// Description: Set the format of the memory window. 
// Return: No
// Param: format A MEM_FORMAT value specifying the format of the memory window: MEM_FORMAT_ASCII, MEM_FORMAT_WCHAR, 
//	MEM_FORMAT_BYTE, MEM_FORMAT_SHORT, MEM_FORMAT_SHORT_HEX, MEM_FORMAT_SHORT_UNSIGNED, MEM_FORMAT_LONG, MEM_FORMAT_LONG_HEX, 
//	MEM_FORMAT_LONG_UNSIGNED, MEM_FORMAT_REAL, MEM_FORMAT_REAL_LONG, MEM_FORMAT_INT64, MEM_FORMAT_INT64_HEX, MEM_FORMAT_INT64_UNSIGNED
// END_HELP_COMMENT
BOOL UIMemory::SetMemoryFormat(MEM_FORMAT format)	
	{

	// Bring up the popup context menu
	MST.DoKeys("+({F10})");
	//Go to the "More Formats" menu item
	MST.DoKeys("{DOWN 4}");

	// Choose More Formats
	MST.DoKeys("{RIGHT}");

	// Do one more Right to highLight the first item in the Submenu
	MST.DoKeys("{RIGHT}");

	// Go to the desirable Memory Format
	switch(format)
	{
	case MEM_FORMAT_ASCII:
		{
		//MST.DoKeys("{DOWN 0}");
		break;
		}
	case MEM_FORMAT_WCHAR:
		{
		MST.DoKeys("{DOWN 1}");
		break;
		}
	case MEM_FORMAT_BYTE:
		{
		MST.DoKeys("{DOWN 2}");
		break;
		}
	case MEM_FORMAT_SHORT:
		{
		MST.DoKeys("{DOWN 3}");
		break;
		}
	case MEM_FORMAT_SHORT_HEX:
		{
		MST.DoKeys("{DOWN 4}");
		break;
		}
	case MEM_FORMAT_SHORT_UNSIGNED:
		{
		MST.DoKeys("{DOWN 5}");
		break;
		}
	case MEM_FORMAT_LONG:
		{
		MST.DoKeys("{DOWN 6}");
		break;
		}
	case MEM_FORMAT_LONG_HEX:
		{
		MST.DoKeys("{DOWN 7}");
		break;
		}
	case MEM_FORMAT_LONG_UNSIGNED:
		{
		MST.DoKeys("{DOWN 8}");
		break;
		}
	case MEM_FORMAT_REAL:
		{
		MST.DoKeys("{DOWN 9}");
		break;
		}
	case MEM_FORMAT_REAL_LONG:
		{
		MST.DoKeys("{DOWN 10}");
		break;
		}
	case MEM_FORMAT_INT64:
		{
		MST.DoKeys("{DOWN 11}");
		break;
		}
	case MEM_FORMAT_INT64_HEX:
		{
		MST.DoKeys("{DOWN 12}");
		break;
		}
	case MEM_FORMAT_INT64_UNSIGNED:
		{
		MST.DoKeys("{DOWN 13}");
		break;
		}
	}
	
	// And choose it
	MST.DoKeys("{ENTER}");

	// Remember memory format
	m_MemoryFormat = format;

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: MEM_FORMAT UIMemory::GetMemoryFormat(void)
// Description: Get the format of the memory window. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A MEM_FORMAT value containing the format of the memory window: MEM_FORMAT_ASCII, MEM_FORMAT_WCHAR, 
//	MEM_FORMAT_BYTE, MEM_FORMAT_SHORT, MEM_FORMAT_SHORT_HEX, MEM_FORMAT_SHORT_UNSIGNED, MEM_FORMAT_LONG, MEM_FORMAT_LONG_HEX, 
//	MEM_FORMAT_LONG_UNSIGNED, MEM_FORMAT_REAL, MEM_FORMAT_REAL_LONG, MEM_FORMAT_INT64, MEM_FORMAT_INT64_HEX, MEM_FORMAT_INT64_UNSIGNED
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
	MEM_FORMAT UIMemory::GetMemoryFormat(void)		//must only set format with SetMemoryFormat!
	{
	return m_MemoryFormat;
	}


// BEGIN_HELP_COMMENT
// Function: int UIMemory::GetMemoryWidth(void)
// Description: Get the width of the memory window. NOTE: NIY
// Return: An integer containing the width of the memory window.
// END_HELP_COMMENT
int UIMemory::GetMemoryWidth(void)			//must only set width with SetMemoryWidth!			
{
	return 0;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::EnableToolBar(BOOL bEnable)
// Description: Enables or disables the edit control in memory window
// Param: bEnable If the parameter is TRUE, edit control will be enabled. If FALSE - disabled.
// Return: A Boolean value that indicates whether the toggle operatin was sucessfull (TRUE) or not.
// END_HELP_COMMENT

BOOL UIMemory::EnableToolBar(BOOL bEnable)
{
	if((  MST.WEditExists(GetLocString(IDSS_MEMORY_ADDRESS_EDIT)) && bEnable) || 
		(!MST.WEditExists(GetLocString(IDSS_MEMORY_ADDRESS_EDIT)) && !bEnable))
		return TRUE;

	// Bring up the popup context menu
	MST.DoKeys("+({F10})");
	//Go to the "Toggle Toolbar" menu item
	MST.DoKeys("{DOWN 4}");

	// Toggle it
	MST.DoKeys("{ENTER}");

	// Check that the edit control exists
	if((  MST.WEditExists(GetLocString(IDSS_MEMORY_ADDRESS_EDIT)) && bEnable) || 
		(!MST.WEditExists(GetLocString(IDSS_MEMORY_ADDRESS_EDIT)) && !bEnable))
		return TRUE;
	else 
		return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: CString UIMemory::GetColumns(void)
// Description: Gets number of columns in memory window
// Param: No Params
// Return: CString Number of columns
// END_HELP_COMMENT
CString UIMemory::GetColumns(void)
{	
	CString Columns;
	MST.WComboText("@1", Columns);
	return Columns;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::SetColumns(CString Columns)
// Description: Sets number of columns in memory window
// Param: Columns A number of columns or "<Auto>"
// Return: A Boolean value that indicates whether the set column operatin was sucessfull (TRUE) or not.
// END_HELP_COMMENT
BOOL UIMemory::SetColumns(CString Columns)
{
	if(!IsActive())
		{
		LOG->RecordInfo("ERROR in UIMemory::GoToAddress(): Memory window not active");
		return ERROR_ERROR;
		}

	MST.WComboItemClk("@1", Columns);
	if(Columns == GetColumns())
		return TRUE;

	LOG->RecordInfo("ERROR in UIMemory::SetColumns(): Number of columns was not set correctly");
	return ERROR_ERROR;
}

// BEGIN_HELP_COMMENT
// Function: CString UIMemory::GetCurrentSymbol(void)
// Description: Gets the address or symbol which appears in the Address edit box in the memory window
// Return: CString that contains the address or symbol which appears in the Address edit box in the memory window
// Param: No Params
// END_HELP_COMMENT
CString UIMemory::GetCurrentSymbol(void)
{
	CString symbol;
	Activate();

	if(!IsActive())
	{
		LOG->RecordInfo("ERROR in UIMemory::GetCurrentSymbol(): Memory window not active");
		return (CString)"";
	}

	if(!MST.WEditExists(GetLocString(IDSS_MEMORY_ADDRESS_EDIT)))
	{
		LOG->RecordInfo("ERROR in UIMemory::GetCurrentSymbol(): The Address edit box does not exist");
		return (CString)"";
	}

	MST.WEditSetFocus(GetLocString(IDSS_MEMORY_ADDRESS_EDIT));
	MST.WEditText(GetLocString(IDSS_MEMORY_ADDRESS_EDIT), symbol);

	return symbol;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::ToogleReEvaluation(void)
// Description: Toggles Re-evaluation in memory window
// Param: No Params
// Return: A Boolean value that indicates whether the toggle operatin was sucessfull (TRUE) or not.
// END_HELP_COMMENT

BOOL UIMemory::ToogleReEvaluation(void)
{

	UIOptionsTabDlg uiopt;
	uiopt.Display();
	uiopt.ShowPage(TAB_DEBUG, 6);

	// Check the status of the "Hexadecimal display" check box
	int stat = MST.WCheckState("@9");
	if(0 == stat)
		MST.WCheckCheck("@9");
	else
		MST. WCheckUnCheck("@9");
	uiopt.OK();
	return TRUE;

}



// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::CycleMemoryFormat(int count)
// Description: Cycles though memory formats
// Param: count A number of "Next Format" changes that will be done 
// Return: A Boolean value that indicates whether the cycle operatin was sucessfull (TRUE) or not..
// END_HELP_COMMENT

BOOL UIMemory::CycleMemoryFormat(int count)
{

	for(int ii = 0; ii < count; ii++)
	{
		// Do cycle using hot keys
		MST.DoKeys("+%({F11})");
	
		// Change m_MemoryFormat
		switch(m_MemoryFormat)
		{
		case MEM_FORMAT_ASCII:
			m_MemoryFormat = MEM_FORMAT_INT64_UNSIGNED;
			break;
		case MEM_FORMAT_WCHAR:
			m_MemoryFormat = MEM_FORMAT_ASCII;		
			break;
		case MEM_FORMAT_BYTE:
			m_MemoryFormat = MEM_FORMAT_WCHAR;		
			break;
		case MEM_FORMAT_SHORT:
			m_MemoryFormat = MEM_FORMAT_BYTE;		
			break;
		case MEM_FORMAT_SHORT_HEX:
			m_MemoryFormat = MEM_FORMAT_SHORT;		
			break;
		case MEM_FORMAT_SHORT_UNSIGNED:
			m_MemoryFormat = MEM_FORMAT_SHORT_HEX;		
			break;
		case MEM_FORMAT_LONG:
			m_MemoryFormat = MEM_FORMAT_SHORT_UNSIGNED;		
			break;
		case MEM_FORMAT_LONG_HEX:
			m_MemoryFormat = MEM_FORMAT_LONG;		
			break;
		case MEM_FORMAT_LONG_UNSIGNED:
			m_MemoryFormat = MEM_FORMAT_LONG_HEX;		
			break;
		case MEM_FORMAT_REAL:
			m_MemoryFormat = MEM_FORMAT_LONG_UNSIGNED;		
			break;
		case MEM_FORMAT_REAL_LONG:
			m_MemoryFormat = MEM_FORMAT_REAL;		
			break;
		case MEM_FORMAT_INT64:
			m_MemoryFormat = MEM_FORMAT_REAL_LONG;		
			break;
		case MEM_FORMAT_INT64_HEX:
			m_MemoryFormat = MEM_FORMAT_INT64;		
			break;
		case MEM_FORMAT_INT64_UNSIGNED:
			m_MemoryFormat = MEM_FORMAT_INT64_HEX;		
			break;
		}
			
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::ToggleDockingView()
// Description: Toggles the Docking View for memory window
// Return: A Boolean value that indicates whether the toggle operatin was sucessfull (TRUE) or not.
// END_HELP_COMMENT

BOOL UIMemory::ToggleDockingView()
{
	Activate();

	if(!IsActive())
		{
		LOG->RecordInfo("ERROR in UIMemory::ToggleDockingView(): Memory window not active");
		return FALSE;
		}
	// Bring up the popup context menu
	MST.DoKeys("+({F10})");
	//Go to the "Docking view" menu item
	MST.DoKeys("{DOWN 5}");

	// Toggle it
	MST.DoKeys("{ENTER}");

	return TRUE;
}




// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::EnableDockingView(BOOL bEnable)
// Description: Enables or disables the Docking View for memory window
// Param: bEnable If the parameter is TRUE, Docking View will be enabled. If FALSE - disabled.
// Return: A Boolean value that indicates whether the toggle operatin was sucessfull (TRUE) or not.
// END_HELP_COMMENT

BOOL UIMemory::EnableDockingView(BOOL bEnable)
{
	Activate();

	if(!IsActive())
		{
		LOG->RecordInfo("ERROR in UIMemory::EnableDockingView(): Memory window not active");
		return FALSE;
		}

	if((IsDockingViewEnabled() && bEnable) || (!IsDockingViewEnabled() && !bEnable))
		return TRUE;

	// Bring up the popup context menu
	MST.DoKeys("+({F10})");
	//Go to the "Docking view" menu item
	MST.DoKeys("{DOWN 5}");

	// Toggle it
	MST.DoKeys("{ENTER}");

	// Check that the window was enabled/disabled
	if((IsDockingViewEnabled() && bEnable) || (!IsDockingViewEnabled() && !bEnable))
		return TRUE;
	else 
		return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIMemory::IsDockingViewEnabled(BOOL bEnable)
// Description: Check if Docking View is enabled for memory window
// Param: No Params
// Return: A Boolean value that indicates whether Docking View is enabled (TRUE) or not.
// END_HELP_COMMENT

BOOL UIMemory::IsDockingViewEnabled(void)
{
	BOOL bResult, bInitiallyMaximized;
	CString csTitle;

	Activate();

	if(!IsActive())
		{
		LOG->RecordInfo("ERROR in UIMemory::IsDockingViewEnabled(): Memory window not active");
		return FALSE;
		}

	HWND hwnd = HWnd();

	// Compiler gives the error that WIsMaximized is not a member of MSTest, so we use standard Windows function
	//if(MST.WIsMaximized(hwnd))
	if(IsZoomed(hwnd))
		bInitiallyMaximized = TRUE;
	else 
	{
		bInitiallyMaximized = FALSE;
		// MSTest.WMaxWnd doesn't work here for some reason, so we use standard Windows function
		// MST.WMaxWnd(hwnd);
		ShowWindow(hwnd, SW_MAXIMIZE);
	}

	MST.WGetText(UIWB.HWnd(), csTitle);
	if(-1 == csTitle.Find("["+ GetLocString(IDSS_MEM_WND_TITLE)+"]"))
		bResult = TRUE;
	else
		bResult = FALSE;


	if(!bInitiallyMaximized)
		// Compiler gives the error that WResWnd is not a member of MSTest, so we use standard Windows function
		//MST.WResWnd(hwnd);
		ShowWindow(hwnd, SW_RESTORE);

	return bResult;

}
