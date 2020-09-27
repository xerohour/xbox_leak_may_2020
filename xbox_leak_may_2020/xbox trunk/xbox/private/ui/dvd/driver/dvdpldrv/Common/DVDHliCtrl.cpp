////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "DVDHliCtrl.h"
#include "Library/Common/vddebug.h"
#include "Library/lowlevel/timer.h"

//
//  Constructor
//

DVDHighlightControl::DVDHighlightControl(void)
	{
	buttonSelected = FALSE;
	highlightStarted = FALSE;
	buttonActivated = FALSE;
	presentationMode = DPM_4BY3;
	highlightActive = FALSE;
	numButtons = 0;
	}

//
//  Initialize
//

Error DVDHighlightControl::Init(DVDSPUDisplay * display, DVDNavigationProcessor * navpu)
	{
	this->display = display;
	this->navpu = navpu;
	GNRAISE_OK;
	}

//
//  Select a button
//

Error DVDHighlightControl::DoSelectButton(BOOL autoActivate)
	{
	ButtonInfo * b = buttons + selectedButton - 1;

	if (autoActivate && b->autoAction)
		return DoActivateButton();
	else
		{
		buttonSelected = TRUE;

		GNREASSERT(display->SetButtonColors(colors[b->colorGroup - 1].select, colors[b->colorGroup - 1].active));
		GNREASSERT(display->SetButtonPosition(buttonGroup, selectedButton, b->x, b->y, b->w, b->h));
		GNREASSERT(display->SetButtonSelect());

		GNRAISE_OK;
		}
	}

//
//  Activate button
//

Error DVDHighlightControl::DoActivateButton(void)
	{
	Error err;
	ButtonInfo * b = buttons + selectedButton - 1;
	int pc = 1;
	DVDNavigationProcessor::NavCommandDone done;
	WORD pml;

	buttonActivated = TRUE;

	GNREASSERT(display->SetButtonColors(colors[b->colorGroup - 1].select, colors[b->colorGroup - 1].active));
	GNREASSERT(display->SetButtonPosition(buttonGroup, selectedButton, b->x, b->y, b->w, b->h));
	GNREASSERT(display->SetButtonActive());

	Timer.WaitMilliSecs(300);

	err = navpu->InterpretCommand(b->com, pc, done, pml);

	if (err == GNR_NO_RESUME_INFORMATION)
		Exit(err);

	GNRAISE(err);
	}

//
//  Select button 'num' and activate it if necessary
//

Error DVDHighlightControl::SelectButton(WORD num, BOOL autoActivate)
	{
	if (num != selectedButton)
		{
		selectedButton = num;

		return DoSelectButton(autoActivate);
		}
	else
		GNRAISE_OK;
	}

//
//  Activate button 'num'
//

Error DVDHighlightControl::ActivateButton(WORD num)
	{
	selectedButton = num;

	return DoActivateButton();
	}

//
//  Complete highlight
//

Error DVDHighlightControl::CompleteHighlight(void)
	{
	if (buttonSelected && !buttonActivated && forcedActivateButton)
		{
		if (forcedActivateButton != 63)
			GNREASSERT(SelectButton(forcedActivateButton));

		GNREASSERT(DoActivateButton());
		}

	GNRAISE_OK;
	}

//
//  Test on valid highlight information for current display mode
//

static inline BOOL IsValidHLI(WORD flags, DisplayPresentationMode mode)
	{
	if      (mode == DPM_4BY3)        return flags == 0;
	else if (mode == DPM_16BY9)       return (flags & 1) != 0;
	else if (mode == DPM_LETTERBOXED) return (flags & 2) != 0;
	else if (mode == DPM_PANSCAN)     return (flags & 4) != 0;
	else return TRUE;
	}

Error DVDHighlightControl::CompleteHighlight(DVDVOBU * vobu)
	{
	int at;
	WORD w;
	DWORD startTime, stopTime, terminateTime, currentTime;

	//
	// Start with Highlight Status
	//

	at = 96;

	w = vobu->GetPCIWordSeq(at);

	startTime = vobu->GetPCIDWordSeq(at);
	stopTime = vobu->GetPCIDWordSeq(at);
	terminateTime = vobu->GetPCIDWordSeq(at);

	currentTime = vobu->GetPCIDWord(16);

	//DP("Complete Current %d Start %d Stop %d Terminate %d", currentTime / 90, startTime / 90, stopTime / 90, terminateTime / 90);

	if ((w & 3) != 0)
		{
		if (terminateTime != 0xffffffff && currentTime >= terminateTime)
			{
			CompleteHighlight();
			}
		}

	GNRAISE_OK;
	}
//
//  Start highlighting
//

Error DVDHighlightControl::StartHighlight(DVDVOBU * vobu)
	{
	int at, i;
	WORD w;
	DWORD d;
	WORD	buttonGroups, group;
	DWORD startTime, stopTime, terminateTime, currentTime;

	//
	// Start with Highlight Status
	//

	at = 96;

	w = vobu->GetPCIWordSeq(at);

	startTime = vobu->GetPCIDWordSeq(at);
	stopTime = vobu->GetPCIDWordSeq(at);
	terminateTime = vobu->GetPCIDWordSeq(at);

	currentTime = vobu->GetPCIDWord(12);

	//DP("Current %d Start %d Stop %d Terminate %d", currentTime / 90, startTime / 90, stopTime / 90, terminateTime / 90);

	if ((w & 3) == 2 && buttonSelected)
		{
		if (terminateTime != 0xffffffff && currentTime >= terminateTime)
			{
			CompleteHighlight();
			}

		highlightStarted = TRUE;
		GNRAISE_OK;  // HLI equal to previous VOBU
		}

	if (w & 3)  // if HLI exists
		{
		buttonSelected = FALSE;
		buttonActivated = FALSE;

		//
		// Read button mode => buttonGroups
		//

		w = vobu->GetPCIWordSeq(at);
		buttonGroups = XTBFW(12, 2, w);

		if (buttonGroups)
			{
			group = 0;
			while (group < buttonGroups && !IsValidHLI((WORD)XTBF(4 * (2 - group), 3, w), presentationMode)) group++;

			if (group >= buttonGroups)
				{
				group = 0;
				}

			buttonGroup = group + 1;

			userButtonOffset = vobu->GetPCIByteSeq(at);
			numButtons = vobu->GetPCIByteSeq(at);
			numUserButtons = vobu->GetPCIByteSeq(at);

			at+=1;  // Skip reserved

			//
			//  Read forcedly selected and activated buttons
			//

			w = vobu->GetPCIByteSeq(at);
			if (w) selectedButton = w;
			forcedActivateButton = vobu->GetPCIByteSeq(at);

			//
			// Read colors
			//

			for (i=0; i<3; i++)
				{
				colors[i].select = vobu->GetPCIDWordSeq(at);
				colors[i].active = vobu->GetPCIDWordSeq(at);
				}

			at += 18 * 36 / buttonGroups * group;

			//
			// Read button information
			//

			for (i=0; i<numButtons; i++)
				{
				ButtonInfo * b = buttons + i;

				d = vobu->GetPCIDWordSeq(at);
				b->colorGroup = (BYTE)XTBF(30, 2, d);
				b->x = (WORD)XTBF(20, 10, d);
				b->w = (WORD)XTBF( 8, 10, d) - b->x + 1;
				d = (d << 16) | vobu->GetPCIWordSeq(at);
				b->autoAction = XTBF(22, 2, d);
				b->y = (WORD)XTBF(12, 10, d);
				b->h = (WORD)XTBF( 0, 10, d) - b->y + 1;

//				DP("(%d, %d) - (%d, %d)", b->x, b->y, b->w, b->h);

				b->upper = vobu->GetPCIByteSeq(at) & 0x3f;
				b->lower = vobu->GetPCIByteSeq(at) & 0x3f;
				b->left = vobu->GetPCIByteSeq(at) & 0x3f;
				b->right = vobu->GetPCIByteSeq(at) & 0x3f;

				vobu->GetPCIBytesSeq(&(b->com), at, 8);
				}

			if (!selectedButton) selectedButton = 1;
			else if (selectedButton > numButtons) selectedButton = numButtons;

			GNREASSERT(DoSelectButton(FALSE));
			highlightActive = TRUE;
			}
		else
			GNREASSERT(CancelHighlight());
		}
	else
		{
		if (highlightStarted)
			{
			highlightStarted = FALSE;
			GNREASSERT(CompleteHighlight());
			}
		GNREASSERT(CancelHighlight());
		}

	GNRAISE_OK;
	}

//
//  Deselect and deactivate button
//

Error DVDHighlightControl::CancelHighlight(void)
	{
	buttonSelected = FALSE;
	buttonActivated = FALSE;
	highlightActive = FALSE;
	numButtons = 0;

	return display->SetButtonOff();
	}

//
//  Select upper button
//

Error DVDHighlightControl::ButtonUp(void)
	{
	if (buttonSelected)
		return SelectButton(buttons[selectedButton - 1].upper);
	else
		GNRAISE(GNR_BUTTON_NOT_FOUND);
	}

//
//  Select lower button
//

Error DVDHighlightControl::ButtonDown(void)
	{
	if (buttonSelected)
		{
		return SelectButton(buttons[selectedButton - 1].lower);
		}
	else
		GNRAISE(GNR_BUTTON_NOT_FOUND);
	}

//
//  Select left button
//

Error DVDHighlightControl::ButtonLeft(void)
	{
	if (buttonSelected)
		return SelectButton(buttons[selectedButton - 1].left);
	else
		GNRAISE(GNR_BUTTON_NOT_FOUND);
	}

//
//  Select right button
//

Error DVDHighlightControl::ButtonRight(void)
	{
	if (buttonSelected)
		return SelectButton(buttons[selectedButton - 1].right);
	else
		GNRAISE(GNR_BUTTON_NOT_FOUND);
	}

//
// ???
//

Error DVDHighlightControl::ButtonSet(WORD num)
	{
	if (buttonSelected && !buttonActivated)
		{
		return SelectButton(num);
		}
	else
		selectedButton = num;

	GNRAISE_OK;
	}

//
//  Select and activate button 'num'
//

Error DVDHighlightControl::ButtonSelectAndActivate(WORD num)
	{
	if (buttonSelected)
		{
		if (num - userButtonOffset >= 1 && num - userButtonOffset <= numUserButtons)
			return ActivateButton(num - userButtonOffset);
		else
			GNRAISE(GNR_RANGE_VIOLATION);
		}
	else
		GNRAISE_OK;
	}

//
//  Select and activate button at position (x, y)
//

Error DVDHighlightControl::ButtonSelectAtAndActivate(WORD x, WORD y)
	{
	WORD num;

//	DP("H (%d, %d)", x, y);

	if (buttonSelected)
		{
		for(num = 1; num <= numButtons; num++)
			{
			ButtonInfo * b = buttons + num - 1;

			if (x >= b->x && x < b->x + b->w && y >= b->y && y < b->y + b->h)
				return ActivateButton(num);
			}

		GNRAISE(GNR_RANGE_VIOLATION);
		}

	GNRAISE_OK;
	}

//
//  Select button at position (x, y)
//

Error DVDHighlightControl::ButtonSelectAt(WORD x, WORD y)
	{
	WORD num;

//	DP("H (%d, %d)", x, y);

	if (buttonSelected)
		{
		for(num = 1; num <= numButtons; num++)
			{
			ButtonInfo * b = buttons + num - 1;

			//
			// prevents selection of buttons in "Die Apothekerin" that causes
			// the menue to be non responsive to keyboard movement
			//
			if (x >= b->x && x < b->x + b->w && y >= b->y && y < b->y + b->h)
				{
				if (b->left == num && b->right == num && b->upper == num && b->lower == num)
					GNRAISE_OK;
				else
					return SelectButton(num, FALSE);
				}
			}

		GNRAISE(GNR_RANGE_VIOLATION);
		}

	GNRAISE_OK;
	}

//
//  Activate currently selected button
//

Error DVDHighlightControl::ButtonActivate(void)
	{
	if (buttonSelected)
		return DoActivateButton();
	else
		GNRAISE_OK;
	}

//
//  Check if there is a button at position (x, y)
//

BOOL DVDHighlightControl::IsButtonAt(WORD x, WORD y)
	{
	int num;

	if (buttonSelected)
		{
		for(num = 1; num <= numButtons; num++)
			{
			ButtonInfo * b = buttons + num - 1;

			if (x >= b->x && x < b->x + b->w && y >= b->y && y < b->y + b->h)
				return TRUE;
			}
		}

	return FALSE;
	}

//
//  Returns of we currently have positional buttons
//

BOOL DVDHighlightControl::HasPositionalButtons(void)
	{
	return buttonSelected != 0;
	}

//
//  Get the state of the currently selected button
//

Error DVDHighlightControl::GetCurrentButtonState(WORD & minButton, WORD & numButtons, WORD & currentButton)
	{
	if (buttonSelected)
		{
		minButton = 1;
		numButtons = numButtons;
		currentButton = selectedButton;
		}
	else
		{
		minButton = 0;
		numButtons = 0;
		currentButton = 0xffff;
		}

	GNRAISE_OK;
	}

//
//  Save current state
//

Error DVDHighlightControl::Freeze(DVDHCFreezeState & state)
	{
	//
	// Copy data into buffer
	//

	state.selectedButton = selectedButton;
	state.highlightActive = highlightActive;

	GNRAISE_OK;
	}

//
//  Return to previously saved state
//

Error DVDHighlightControl::Defrost(DVDHCFreezeState & state)
	{
	//
	// Restore data
	//

	selectedButton = state.selectedButton;
	highlightActive = state.highlightActive;

	GNRAISE_OK;
	}
