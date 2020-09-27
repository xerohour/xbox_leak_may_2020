///////////////////////////////////////////////////////////////////////////////
//  CODAM.CPP
//
//  Created by :            Date :
//      WayneBr                 1/14/94
//
//  Description :
//      Implementation of the CODAM class
//

#include "stdafx.h"
#include "codam.h"
#include "uidebug.h"

#include "..\SYM\cmdids.h"
#include "..\SHL\uwbframe.h"
#include "..\..\coclip.h"
#include <testxcpt.h>
#include <guiv1.h>
#include <mstwrap.h>
#include "..\SHL\uioptdlg.h"
#include "codebug.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// none of these close the DAM window 				

// BEGIN_HELP_COMMENT
// Function: int CODAM::GotoSymbol(LPCSTR symbol)
// Description: Navigate to a specified symbol in the disassembly code.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A pointer to a string that contains the symbol to navigate to.
// Param: bCloseAfter A Boolean value that indicates if dialog should be closed after the command was implemented (TRUE).
// END_HELP_COMMENT
BOOL  CODAM::GotoSymbol(LPCSTR symbol, BOOL bCloseAfter /* TRUE */)

{
	const char* const THIS_FUNCTION = "CODAM::GotoSymbol()";

	if(!uidam.Activate())
	{
		LOG->RecordInfo("ERROR in %s: could not activate the asm window.", THIS_FUNCTION);
		return FALSE;
	}

	return uidam.GotoSymbol(symbol, bCloseAfter);
}


// BEGIN_HELP_COMMENT
// Function: int CODAM::GotoAddress(int address)
// Description: Navigate to an address in the disassembly code.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: address An integer that contains the address to navigate to.
// Param: bCloseAfter A Boolean value that indicates if dialog should be closed after the command was implemented (TRUE).
// END_HELP_COMMENT
BOOL  CODAM::GotoAddress(int address, BOOL bCloseAfter /* TRUE */)

{
	const char* const THIS_FUNCTION = "CODAM::GotoAddress()";

	if(!uidam.Activate())
	{
		LOG->RecordInfo("ERROR in %s: could not activate the asm window.", THIS_FUNCTION);
		return FALSE;
	}

	return uidam.GotoAddress(address, bCloseAfter);
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODAM::InstructionIs(LPCSTR inst)
// Description: Determine whether the instruction at the current location in the diassasembly code matches the given instruction.
// Return: A Boolean value that indicates whether the instruction at the current location in the disassembly code matches the given instruction (TRUE) or not.
// Param: inst A pointer to a string that contains the instruction to compare to.
// END_HELP_COMMENT
BOOL  CODAM::InstructionIs(LPCSTR inst)
	
{
	const char* const THIS_FUNCTION = "CODAM::InstructionIs()";

	if(!uidam.Activate())
	{
		LOG->RecordInfo("ERROR in %s: could not activate the asm window.", THIS_FUNCTION);
		return FALSE;
	}

	CString temp = uidam.GetInstruction();

	if(temp.Find(inst) == -1)
		LOG->RecordInfo("ERROR in %s: Instruction is %s, expected %s", THIS_FUNCTION, temp, inst);

	return (temp.Find(inst) != -1);
}


// BEGIN_HELP_COMMENT
// Function: CString CODAM::GetInstruction()
// Description: Get the instruction at the current location in the disassembly code.
// Return: A CString that contains the instruction.
// END_HELP_COMMENT
CString CODAM::GetInstruction()

{
	const char* const THIS_FUNCTION = "CODAM::GetInstruction()";

	if(!uidam.Activate())
	{
		LOG->RecordInfo("ERROR in %s: could not activate the asm window.", THIS_FUNCTION);
		return (CString)"";
	}

	return uidam.GetInstruction();
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODAM::Enable()
// Description: Enable access to disassembly in the debugger.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL CODAM::Enable()
{
	return uidam.Activate();
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODAM::Disable()
// Description: Disable access to disassembly in the debugger.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL CODAM::Disable()

{
	const char* const THIS_FUNCTION = "CODAM::Disable()";

	if(!uidam.ChooseContextMenuItem(CLOSE_HIDE))
	{
		LOG->RecordInfo("ERROR in %s: could not select CLOSE/HIDE menu item in asm context menu.", THIS_FUNCTION);
		return FALSE;
	}

	if(uidam.IsActive())
	{
		LOG->RecordInfo("ERROR in %s: asm window is still active after trying to close it.", THIS_FUNCTION);
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: CString CODAM::GetLines(int startline /* 0 */, int count /* 1 */)
// Description: Returns one or more lines in the disassembly listing.
// Return: none
// Param: startline An integer that contains offset of the starting line of the block to get (ex: 0 represents the current line, 1 the line below, -1 the line above, etc.).
// Param: count An integer that contains the number of lines to get.
// END_HELP_COMMENT
CString CODAM::GetLines(int startline /* 0 */, int count /* 1 */)

{
	int i;

	// 0 represents the current line.
	if(startline > 0)
	{
		for(i = 0; i < startline; i++)
			MST.DoKeys("{DOWN}");
	}
	else if(startline < 0)
	{
		for(i = 0; i > startline; i--)
			MST.DoKeys("{UP}");
	}
	
	// select the line(s).
	for(i = 0; i < count; i++)
		MST.DoKeys("+({DOWN})");

	// copy the lines to the clipboard and return them.
	UIWB.DoCommand(ID_EDIT_COPY, DC_ACCEL);
	COClipboard clip;
	return clip.GetText();
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODAM::ToggleSourceDisplay(BOOL bToggle)
// Description: Toggles the source display feature of the asm window on and off.
// Return: TRUE is successful, FALSE if not.
// Param: bToggle: a BOOL value specifying to turn source display on (TRUE) or off (FALSE).
// END_HELP_COMMENT
BOOL CODAM::ToggleSourceDisplay(BOOL bToggle)

{
	const char* const THIS_FUNC = "CODebug::GoToDisassembly()";
	BOOL bReturn = TRUE;
	UIOptionsTabDlg uiopt;

	// bring up the options dlg.
	if(uiopt.Display())
	
	{
		// select the debug page.
		if(uiopt.ShowPage(TAB_DEBUG, 6))
		
		{
			// set the source display checkbox accordingly.
			if(bToggle)
				MST.WCheckCheck("@13");
			else
 				MST.WCheckUnCheck("@13");

			// check that the source display checkbox was checked\unchecked properly.
			BOOL bCheckState = MST.WCheckState("@13");

			if((bCheckState  && !bToggle) || (!bCheckState  && bToggle))
			{
				LOG->RecordInfo("ERROR in %s: source display checkbox state is %d instead of expected %d.", 
					THIS_FUNC, bCheckState, bToggle);
				bReturn = FALSE;
			}
		
		}
		
		else
		{
			LOG->RecordInfo("ERROR in %s: could not select the debug page of the options dlg.", THIS_FUNC);
			bReturn = FALSE;
		}

		// close the options dlg.
		if(uiopt.OK())
		{
			LOG->RecordInfo("ERROR in %s: could not close the options dlg.", THIS_FUNC);
			bReturn = FALSE;
		}
	}
		
	else
	{
		LOG->RecordInfo("ERROR in %s: could not bring up the options dlg.", THIS_FUNC);
		bReturn = FALSE;
	}
	
	return bReturn;
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODAM::InstructionContains(int offset, CString inst)
// Description: Determine whether the instruction at the location set by offset in the diassasembly code contains the given instruction.
// Return: A Boolean value that indicates whether the instruction at the current location in the disassembly code matches the given instruction (TRUE) or not.
// Param: offset An integer that contains the offset (in lines) from the current cursor position to start retrieving instructions from. This value is negative to move up in the window (down in memory). (Default value is 0.)
// Param: inst A string that contains the instruction to compare to.
// Param: bLocateCaretAtLeftTop A Boolean value that indicates if the caret should be moved to the left top corner of the DAM window before offset
// END_HELP_COMMENT
BOOL CODAM::InstructionContains(int offset, CString ExpectedInst, BOOL bLocateCaretAtLeftTop)
{
	CString csDamInstruction = uidam.GetInstruction(offset, 1, bLocateCaretAtLeftTop);

	// Remove Tabs and spaces. 
	ExpectedInst.TrimRight("\t ");
	ExpectedInst.TrimLeft("\t ");

	if(-1 == csDamInstruction.Find(ExpectedInst))
	{
		LOG->RecordSuccess("Expected Instruction %s", ExpectedInst);
		LOG->RecordSuccess("Actual   Instruction %s", csDamInstruction);
		LOG->RecordSuccess("Offset               %d", offset);
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODAM::SwitchBetweenASMAndSRC(BOOL bUsingContextMenu)
// Description: Switches from Source window to DAM and back
// Return: A Boolean value which is currenly always TRUE
// Param: bUsingContextMenu A Boolean value that indicates if the context menu should be used to switch.  If FALSE, hot key is used.
// END_HELP_COMMENT
BOOL CODAM::SwitchBetweenASMAndSRC(BOOL bUsingContextMenu)
{
 return uidam.SwitchBetweenASMAndSRC(bUsingContextMenu);
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODAM::IsActive(void)
// Description: Determine if the disassembly window is active.
// Return: A Boolean value that indicates whether the disassembly window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL CODAM::IsActive(void)
{
	return uidam.IsActive();
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODAM::VerifyCurrentInstruction(LPCSTR szInstruction)
// Description: Determine whether the instruction at the current location in the diassasembly code matches the given instruction.
// Description: The given instruction should be in the form "<instruction> <operands>", with one space between the instruction and operands.
// Return: A Boolean value that indicates whether the instruction at the current location in the disassembly code matches the given instruction (TRUE) or not.
// Param: szInstruction - A pointer to a string that contains the instruction to compare to.
// END_HELP_COMMENT
BOOL  CODAM::VerifyCurrentInstruction(LPCSTR szInstruction)
	
{
	const char* const THIS_FUNCTION = "CODAM::VerifyCurrentInstruction()";

	// activate the disassembly window.
	if(!uidam.Activate())
	{
		LOG->RecordInfo("ERROR in %s: could not activate the disassembly window.", THIS_FUNCTION);
		return FALSE;
	}

	// see if the expected instruction has an operand.
	CString cstrExpected = szInstruction;
	int intSpace = cstrExpected.Find(" ");

	// if the instruction has an operand, then we need to insert the correct amount of spaces between them so it matches what the debugger will display.
	if(intSpace != -1)
	
	{
		// the instruction is everything before the space.
		CString cstrInstruction = cstrExpected.Left(intSpace);
		// the operand is everything after the space.
		CString cstrOperand = cstrExpected.Mid(intSpace + 1);

		// the length from the start of the instruction to the start of the operand is always 12 characters.
		// in between the end of the instruction and the start of the operand is a variable number of spaces.
		cstrExpected = cstrInstruction;
		int intNumSpaces = 12 - cstrInstruction.GetLength();
		
		// add the appropriate number of spaces.
		for(int i = 0; i < intNumSpaces; i++)
			cstrExpected = cstrExpected + " ";
		
		// append the operand.
		cstrExpected = cstrExpected + cstrOperand;
	}

	// get the actual instruction from the disassembly window.
	CString cstrActual = uidam.GetInstruction();

	// check if the expected instruction can be found in the actual instruction.
	if(cstrActual.Find(cstrExpected) == -1)
	{
		LOG->RecordInfo("ERROR in %s: Cannot find expected instruction \"%s\" in actual instruction \"%s\"", THIS_FUNCTION, cstrExpected, cstrActual);
		return FALSE;
	}

	return TRUE;
}

