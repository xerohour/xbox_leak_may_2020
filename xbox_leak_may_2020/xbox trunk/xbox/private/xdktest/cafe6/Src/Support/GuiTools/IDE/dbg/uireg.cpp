///////////////////////////////////////////////////////////////////////////////
//  UIREG.CPP
//
//  Created by :            Date :
//      WayneBr                 1/14/94
//
//  Description :
//      Implementation of the UIRegisters class
//

#include "stdafx.h"
#include "uireg.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\shl\uwbframe.h"
#include "..\shl\wbutil.h"
#include "Strings.h"
#include "..\sym\reg.h"
#include "..\SRC\cosource.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIRegisters::IsValid(void) const
// Description: Determine if the registers window is a valid window.
// Return: A Boolean that indicates whether the registers window is valid (TRUE) or not.
// END_HELP_COMMENT
BOOL UIRegisters::IsValid(void) const
	{
	return UIWindow::IsValid();
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIRegisters::AttachActive(void)
// Description: Attach to the registers window if it's the active window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIRegisters::AttachActive(void)
	{
	HWND hwnd;

    if(hwnd = MST.WFndWndWait(GetLocString(IDSS_REG_WND_TITLE), FW_FULL, 2))
		return Attach(hwnd);
	else
		return Attach(FindFirstChild(UIWB.HWnd(), IDW_CPU_WIN));
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIRegisters::Find(void)
// Description: Find the registers window and attach to it
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIRegisters::Find(void)
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
// Function: BOOL UIRegisters::IsActive(void)
// Description: Determine if the registers window is active.
// Return: A Boolean value that indicates whether the registers window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL UIRegisters::IsActive(void)
	{
	HWND hwnd = MST.WGetFocus();
	return (hwnd == HWnd()) || IsChild(HWnd(), hwnd);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIRegisters::Activate(void)
// Description: Bring up the registers window.
// Return: A Boolean value that indicates whether the registers window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL UIRegisters::Activate(void)
{
	UIWB.DoCommand(IDM_VIEW_REGS, DC_MNEMONIC);
	return AttachActive();
}

// BEGIN_HELP_COMMENT
// Function: void UIRegisters::ToggleFloatingPoint(void)
// Description: Toggles the Floating Point menu item in the Registers window context menu 
// Return: A Boolean value that indicates whether the Floating Point menu item was toggled successfully (TRUE) or not.
// END_HELP_COMMENT
BOOL UIRegisters::ToggleFloatingPoint(void)
{
	
	// Verify that window is active
	if(!IsActive())
		return FALSE;

	// Bring up the popup context menu
	MST.DoKeys("+({F10})");

	//Go to the desired menu item
	MST.DoKeys("{DOWN}");

	// Toggle it
	MST.DoKeys("{ENTER}");

	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: int UIRegisters::GetRegister(int reg)
// Description: Get the value of one of the registers in the registers window.
// Return: An integer that contains the value of the specified register.
// Param: reg An integer that specifies the 1-based index into the set of registers.
// END_HELP_COMMENT
int  UIRegisters::GetRegister(int reg)
	{
	ASSERT(reg <= MaxReg);
	char buffer[10];
	CString count=itoa(reg,buffer,10);
	CString Keys = "^{HOME}{TAB "+count+"}+{END}^(c)";
	MST.DoKeys(Keys);
	CString cstr;
	GetClipText(cstr);
	return (atoi(cstr));
	};

// BEGIN_HELP_COMMENT
// Function: CString UIRegisters::GetRegister(CString csRegister, int *pnValue)
// Description: Get the value of one of the registers.
// Return: A CString that contains the value of the specified register.
// Param : CString csRegister a string that specifies the register
// Param int* pnValue A pointer to integer that contains the value of the specified register.
// END_HELP_COMMENT
CString UIRegisters::GetRegister(CString csRegister, int *pnValue)

{
	CString csReturnedValue;
	CString csRegisterInfo;
	char szRegisterInfo[4096];
	char *ptr;

	// Verify that window is active
	if(!IsActive())
		return CString("");

	// Find the register
	CString Keys = "^{END}^(+{HOME})^(c)";
	MST.DoKeys(Keys);

	// TODO(dklem - 8/2/99): without this we get the text that was previously in the clipboard.
	Sleep(500);

	// Get text from Clipboard
	GetClipText(csRegisterInfo);
	lstrcpyn(szRegisterInfo, LPCTSTR(csRegisterInfo), sizeof(szRegisterInfo));

	// Now remove the selection
	MST.DoKeys("^{HOME}");	

	// need to distinguish between EI and EIP.
	if(csRegister == "EI")
		csRegister = "EI=";
	// need to distinguish between ES and ESI.
	else if(csRegister == "ES")
		csRegister = "ES =";
	//xbox need to work around flag AC because it might be found inside a hex value
	else if (csRegister == "AC")
		csRegister = "AC=";

	if(NULL == (ptr = strstr(szRegisterInfo, LPCTSTR(csRegister))))
	{
		LOG->RecordInfo( (LPCSTR) "Can not find register %s in Registry window", LPCTSTR(csRegister));
		return CString("");
	}

	// Find the register in the returned string
	ptr = strstr(ptr, "=");
	while(*(++ptr) == ' ');

	// Find the next space
	for(int ii = 0; *(ptr + ii) != '\0'; ii++)
	{
		if(*(ptr + ii) == ' ' || *(ptr + ii) == '\r')
		{
			*(ptr + ii) = '\0';
			break;
		}
	}

	// Fill the CString return value
	csReturnedValue = ptr;

	// Set the int return value
	if(pnValue)
	{
		char *stop;
		*pnValue = strtol(ptr, &stop, 16);
	}

	return csReturnedValue;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIRegisters::SetRegister(int reg, LPCSTR value)
// Description: Set the value of the specified register in the registers window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: reg An integer that specifies the 1-based index into the set of registers.
// Param: value A pointer to a string that contains the value to set the specified register to.
// END_HELP_COMMENT
BOOL UIRegisters::SetRegister(int reg, LPCSTR value)
	{
	ASSERT(reg <= MaxReg);
	char buffer[10];
	CString count=itoa(reg,buffer,10);
	CString Keys = "^{HOME}{TAB "+count+"}";
	MST.DoKeys(Keys);
	// call func that excapes the dokeys controls chars use TRUE flag
	MST.DoKeys(value,TRUE);
	return TRUE;
	};


// BEGIN_HELP_COMMENT
// Function: BOOL IUIRegisters::SetRegister(CString csRegister, CString szValue, int nValue, BOOL bValueIsCString = TRUE)
// Description: Set the value of the specified register.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param : CString csRegister CString that specifies the register to be set
// Param : CString szValue A CString that contains the value to set the specified register to
// Param : int nValue An int that contains the value to set the specified register to
// Param : bValueIsCString A BOOL that indicats if value is represented in the form of CString (TRUE) or int
// END_HELP_COMMENT
BOOL UIRegisters::SetRegister(CString csRegister, CString szValue, int nValue, BOOL bValueIsCString /* TRUE */)
{
	CString csRegisterInfo;
	char szRegisterInfo[4096];
	char szRegister[128];
	char *ptr;

	int ii;
	int nLine = 0;
	int nColumn = 0;
	int nNewLinePos = 0;

	// Verify that window is active
	if(!IsActive())
		return FALSE;

	// Find the register
	// MST.DoKeys("^{HOME}");	
	// MST.DoKeys("^+{END}");	
	// MST.DoKeys("^C");	

	CString Keys = "^{END}^(+{HOME})^(c)";
	MST.DoKeys(Keys);

	// TODO(dklem - 8/2/99): without this we get the text that was previously in the clipboard.
	Sleep(500);

	// Get text from Clipboard
	GetClipText(csRegisterInfo);
	lstrcpyn(szRegisterInfo, LPCTSTR(csRegisterInfo), sizeof(szRegisterInfo));

	// Now remove the selection
	MST.DoKeys("^{HOME}");	

	// need to distinguish between EI and EIP.
	if(csRegister == "EI")
		csRegister = "EI=";
	// need to distinguish between ES and ESI.
	else if(csRegister == "ES")
		csRegister = "ES =";

	if(NULL == (ptr = strstr(szRegisterInfo, LPCTSTR(csRegister))))
	{
		LOG->RecordInfo( (LPCSTR) "Can not find register %s in Registry window", LPCTSTR(csRegister));
		return FALSE;
	}

	// Now we need to find the line that containd the string
	for(ii = 0; *(szRegisterInfo + ii) != '\0' && ((szRegisterInfo + ii) < ptr); ii++)
	{
		if(*(szRegisterInfo + ii) == '\n')
		{
			nLine++;
			nNewLinePos = ii;
		}
	}

	// Now we need to find the column (or offset from the begining of the line in other words)
	for(ii = nNewLinePos; *(szRegisterInfo + ii) != '\0' && ((szRegisterInfo + ii) < ptr); ii++)
	{
		if(*(szRegisterInfo + ii) == '=')
			nColumn++;
	}

	// We counted how many '='s were before the register we are looking for.  So the correct column will be the next '='
	nColumn++;

	// Now locate the caret at the correct position
	MST.DoKeys("^{HOME}");	

	// Down
	for(ii = 0; ii < nLine; ii++)
		MST.DoKeys("{DOWN}");	
	// Right
	for(ii = 0; ii < nColumn; ii++)
		MST.DoKeys("{TAB}");	

	// Type string
	if(bValueIsCString)
		MST.DoKeys(szValue, TRUE);
	else
	{
		sprintf(szRegister, "%x", nValue);
		MST.DoKeys(szRegister);	
	}

	return TRUE;
}

// BEGIN_HELP_COMMENT
// added by dverma 3/21/2000
// Function: BOOL UIRegisters::RegisterExists(CString csRegister)
// Description: Checks whether the given register is displayed in the registers window
// Return: A Boolean that indicates whether the given register was displayed or not
// Param : CString that specifies the register to checked
// END_HELP_COMMENT
BOOL UIRegisters::RegisterDisplayed(CString csRegister)
{
	CString csRegisterInfo;
	char szRegisterInfo[4096];

	// Verify that window is active
	if(!IsActive())
	{
		LOG->RecordInfo( (LPCSTR) "UIReg failure : Register Window Not Active.");
		return FALSE;
	}

	CString Keys = "^{END}^(+{HOME})^(c)";
	MST.DoKeys(Keys);

	// TODO(dklem - 8/2/99): without this we get the text that was previously in the clipboard.
	Sleep(500);

	// Get text from Clipboard
	GetClipText(csRegisterInfo);
	lstrcpyn(szRegisterInfo, LPCTSTR(csRegisterInfo), sizeof(szRegisterInfo));

	// Now remove the selection
	MST.DoKeys("^{HOME}");	

	// need to distinguish between EI and EIP.
	if(csRegister == "EI")
		csRegister = "EI=";
	// need to distinguish between ES and ESI.
	else if(csRegister == "ES")
		csRegister = "ES =";

	//	need to check for MM strings as they may be wrongly detected as part of XMM
	if (csRegister.GetLength() > 2)
	{
		if ((csRegister[0] == 'M') && (csRegister[1] == 'M'))
			csRegister = " " + csRegister;
	}

	if(NULL == strstr(szRegisterInfo, LPCTSTR(csRegister)))
		return FALSE;
	else 
		return TRUE;
}