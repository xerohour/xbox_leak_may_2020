///////////////////////////////////////////////////////////////////////////////
//	UMAINFRM.CPP
//
//	Created by :			Date :
//		DavidGa					9/20/93
//
//	Description :
//		Declaration of the UIWindow class
//

#include "stdafx.h"
#include "umainfrm.h"
#include "guiv1.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "wbutil.h"
#include "..\..\imewrap.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void UIMainFrm::OnUpdate(void)
{
	m_pCmdTable = FALSE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIMainFrm::IsValid(void) const
// Description: Determine if the IDE's main frame window is a valid window.
// Return: TRUE if the main frame is valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIMainFrm::IsValid(void) const
{
	return UIWindow::IsValid();
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIMainFrm::DoCommand(UINT id, DC_METHOD method)
// Description: Perform a command in the IDE. Most of the commands handled by this function are menu commands, which can be performed either by sending a message, selecting the menu items, using the menu mnemonics, or using the keyboard accelerator.
// Return: TRUE if the command was performed; FALSE otherwise.
// Param: id The ID of the command to perform. See cmdarrray.cpp for the set of valid commands.
// Param: method The method by which to perform the action: DC_MESSAGE, DC_MENU, DC_MNEMONIC, DC_ACCEL.
// END_HELP_COMMENT
BOOL UIMainFrm::DoCommand( UINT id, DC_METHOD method)
{
	const char* const THIS_FUNCTION = "UIMainFrm::DoCommand";
	UINT ime_mode;

	// be sure the command table is valid
	ASSERT(m_pCmdTable != NULL);

	// search for the command in the command table
	for (int index = 0; m_pCmdTable[index].idCommand != 0; index++) {
		// is this the command?
		if (m_pCmdTable[index].idCommand == id) {
			CMD_STRUCT* pCmd = &(m_pCmdTable[index]);
			// perform the command using whatever method was specified
			switch (method) {
				// perform the command by sending a message
				case DC_MESSAGE: {
					WaitStepInstructions("PostMessage WM_COMMAND, wParam == 0x%X", id);
					return !PostMessage(HWnd(), WM_COMMAND, pCmd->idCommand, 0L);
					break;
				}
				// perform the command by selecting the menus
				case DC_MENU: {  // ERROR ERROR ERROR ERROR DC_MENU doesn't work it's converted to DC_MNEMONIC
					// be sure at least one menu string exists for this command
                                      ASSERT(pCmd->idMenu[0] != 0);

					// build a menu string for DoMenu (a series of null-separated menu names
					char szMenu[1024];
					char* pc = szMenu;
					for (int i = 0; i < 3; i++) {
						if (pCmd->idMenu[i] != 0) {
							CString str = GetLocString(pCmd->idMenu[i]);
							strcpy(pc, str);
							pc += str.GetLength() + 1;
						}
					}
					*pc = '\0';

					return !DoMenu(szMenu);
                                        break;
				}
				// perform the command by using the mnemonics
				case DC_MNEMONIC:{
					// in order to access the menu items in any other emulation modes other than VC,
					// we need to do Alt by itself and then the remaining keystrokes to access the menu item.
					// Doing Alt by itself doesn't work in all cases. Sometimes the Alt by itself is swallowed by
					// the IDE, so we'll do Alt+space+esc to activate the menu bar.
					// is the first character an Alt?

					if(IME.IsIMEInUse())					
					
					{
						// REVIEW (michma - 6/3/98)
						// sometimes calling the ime here de-activates the active window (for example, when using the
						// Edit.Goto dialog). so we preserve the active handle here for possible re-activation later.
						HWND hWndActiveBefore = MST.WGetActWnd(0);

						ime_mode = IME.GetMode();
						IME.SetMode();
						IME.Open(FALSE);

						HWND hWndActiveAfter = MST.WGetActWnd(0);

						if(hWndActiveAfter != hWndActiveBefore)
							MST.WSetActWnd(hWndActiveBefore);
					}

					if ((pCmd->szMnemonic[0] == '%')|| (pCmd->szMnemonic[0] == NULL)) {
					// build a menu string for DoMenu (a series of null-separated menu names
					char szMenu[1024];
					char* pc = szMenu;
					CString Menu ;
					for (int i = 0; i < 3; i++) {
						if (pCmd->idMenu[i] != 0) {
							CString str = GetLocString(pCmd->idMenu[i]);
							strcpy(pc, str);
							pc += str.GetLength() + 1;
							Menu += str ;
						}
					
					}
					*pc = '\0';
					//Find the mnemonics from the string and use them.
					//CString Menu = pc ;
					
					CString HotKeys, Alt = "%";
					HotKeys = Alt+ ExtractHotKey(Menu) ;
					Menu = szMenu ;
					// do the Alt
					CString FileMenu = GetLocString(IDS_FILEMENU) ;
					FileMenu = Alt+ ExtractHotKey(FileMenu) + "{esc}" ;

					MST.DoKeyshWnd(HWnd(), FileMenu.GetBuffer(FileMenu.GetLength())) ; // "%f{esc}");
					// Perform the desired command.
					MST.DoKeyshWnd(HWnd(), HotKeys.GetBuffer(HotKeys.GetLength()));
					}
					else {  // If the string contained an accelerator sequence instead.
						MST.DoKeyshWnd(HWnd(), pCmd->szMnemonic);	// REVIEW(davidga): could use DoKeyshWnd()
					}

					if(IME.IsIMEInUse())					
					
					{
						// REVIEW (michma - 6/3/98)
						// sometimes calling the ime here de-activates the active window (for example, when using the
						// Edit.Goto dialog). so we preserve the active handle here for possible re-activation later.
						HWND hWndActiveBefore = MST.WGetActWnd(0);
						
						IME.Open();
						IME.SetMode(ime_mode);

						HWND hWndActiveAfter = MST.WGetActWnd(0);

						if(hWndActiveAfter != hWndActiveBefore)
							MST.WSetActWnd(hWndActiveBefore);
					}

					return TRUE;
					}
					
				// perform the command by using the accelerator
				case DC_ACCEL: {
					// be sure an accelerator is defined for this command
					ASSERT(pCmd->szAccel != NULL);
					MST.DoKeys(pCmd->szAccel);
					return TRUE;
					break;


				}
				default: {
					ASSERT(FALSE);	// Did we forget to add a new case?
					return 0;
					break;
				}
			}
		}
	}
	LOG->RecordInfo("%s: command %d not found in the command table.", THIS_FUNCTION, id);
	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIMainFrm::IsCommandEnabled(UINT id)
// Description: Determine if the specified command is enabled in the IDE.
// Return: TRUE if the command is enabled; FALSE otherwise.
// Param: id The ID of the command to check. See cmdarrray.cpp for the set of valid commands.
// END_HELP_COMMENT
BOOL UIMainFrm::IsCommandEnabled(UINT id)
{
	const char* const THIS_FUNCTION = "UIMainFrm::IsCommandEnabled";

	// be sure the command table is valid
	ASSERT(m_pCmdTable != NULL);

	// search for the command in the command table
	for (int index = 0; m_pCmdTable[index].idCommand != 0; index++) {
		// is this the command?
		if (m_pCmdTable[index].idCommand == id) {
			CMD_STRUCT* pCmd = &(m_pCmdTable[index]);
			// be sure at least one menu string exists for this command
			ASSERT(pCmd->idMenu[0] != 0);

			for (int i = 0; i < 3; i++) {
				if (pCmd->idMenu[i] != 0) {
					Sleep(400);
					// if the menu item's not enabled, close all menus and return fail
					BOOL bEnabled = MST.WMenuEnabled(GetLocString(pCmd->idMenu[i]));
					if (!bEnabled) {
						MST.WMenuEnd();
						return FALSE;
					}
					// if there are more menu items to check, select this one
					if (i < 2 && pCmd->idMenu[i+1]) {
						MST.WMenu(GetLocString(pCmd->idMenu[i]));
					}
					// otherwise the menu item's enabled
					else {
						MST.WMenuEnd();
						return TRUE;
					}
				}
			}
		}
	}

	LOG->RecordInfo("%s: command %d not found in command table.", THIS_FUNCTION, id);
	return FALSE;
}

