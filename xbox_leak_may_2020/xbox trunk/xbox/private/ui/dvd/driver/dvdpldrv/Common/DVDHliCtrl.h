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

//////////////////////////////////////////////////////////////////////
//
//  Sub Picture and Highlight Module
//
//////////////////////////////////////////////////////////////////////

#ifndef DVDHLICL_H
#define DVDHLICL_H

#include "DVDStreamServer.h"
#include "DVDNavpu.h"

//////////////////////////////////////////////////////////////////////
//
//  Sub Picture Unit
//
//////////////////////////////////////////////////////////////////////

class DVDSPUDisplay
	{
	public:
		virtual Error SetButtonColors(DWORD select, DWORD active) = 0;
		virtual Error SetButtonPosition(WORD group, WORD button, WORD x, WORD y, WORD w, WORD h) = 0;
		virtual Error SetButtonOff(void) = 0;
		virtual Error SetButtonSelect(void) = 0;
		virtual Error SetButtonActive(void) = 0;
	};

//////////////////////////////////////////////////////////////////////
//
//  Highlight Control
//
//////////////////////////////////////////////////////////////////////

class DVDHighlightControl
	{
	protected:
		BOOL	buttonSelected;
		BOOL	highlightStarted;
		BOOL	buttonActivated;
		BOOL	highlightActive;

		WORD 	userButtonOffset;
		WORD  numButtons;
		WORD	numUserButtons;
		WORD	selectedButton;
		WORD	forcedActivateButton;
		WORD	buttonGroup;				// Current button group

		DisplayPresentationMode	presentationMode;

		struct ButtonColors
			{
			DWORD select, active;
			} colors[3];

		struct ButtonInfo
			{
			BYTE						colorGroup;
			WORD						x, y, w, h;
			BOOL						autoAction;
			BYTE						upper, lower, left, right;
			DVDNavigationCommand com;
			} buttons[36];

		DVDSPUDisplay 				*	display;
		DVDNavigationProcessor	*	navpu;

		Error DoSelectButton(BOOL autoActivate = TRUE);
		virtual Error DoActivateButton(void);
		Error SelectButton(WORD num, BOOL autoActivate = TRUE);
		Error ActivateButton(WORD num);

		//
		//  Freezing stuff
		//

		class DVDHCFreezeState
			{
			public:
				WORD	selectedButton;
				BOOL	highlightActive;
			};

		Error Freeze (DVDHCFreezeState & state);
		Error Defrost(DVDHCFreezeState & state);

		//
		//  Inquiry methods
		//

		void GetButtonInfo(int button, WORD & x, WORD & y, WORD & w, WORD & h,
												 BYTE & upper, BYTE & lower, BYTE & left, BYTE & right, BOOL autoAction)
			{
			x = buttons[button].x; y = buttons[button].y; w = buttons[button].w; h = buttons[button].h;
			upper = buttons[button].upper; lower = buttons[button].lower; left = buttons[button].left; right = buttons[button].right;
			autoAction = buttons[button].autoAction;
			}

		void GetButtonGeneralInfo(WORD & numberOfButtons, WORD & selectedButton, WORD & forcedlyActivatedButton,
										  WORD & userButtonOffset, WORD & numberOfUserButtons)
			{
			numberOfButtons = numButtons; selectedButton = this->selectedButton; forcedlyActivatedButton = forcedActivateButton;
			userButtonOffset = this->userButtonOffset; numberOfUserButtons = numUserButtons;
			}

	public:
		DVDHighlightControl(void);

		Error Init(DVDSPUDisplay * display, DVDNavigationProcessor * navpu);
		virtual Error SetPresentationMode(DisplayPresentationMode mode) {presentationMode = mode; GNRAISE_OK;}

		Error StartHighlight(DVDVOBU * vobu);
		Error CompleteHighlight(DVDVOBU * vobu);
		Error CompleteHighlight(void);

		Error CancelHighlight(void);

		Error ButtonUp(void);
		Error ButtonDown(void);
		Error ButtonLeft(void);
		Error ButtonRight(void);
		Error ButtonSet(WORD num);
		Error ButtonSelectAt(WORD x, WORD y);

		Error ButtonSelectAndActivate(WORD num);
		Error ButtonSelectAtAndActivate(WORD x, WORD y);

		Error ButtonActivate(void);

		BOOL HasPositionalButtons(void);
		BOOL IsButtonAt(WORD x, WORD y);
		BOOL HighlightActive(void) { return highlightActive; }
		BOOL ButtonActivated(void) { return buttonActivated; }

		Error GetCurrentButtonState(WORD & minButton, WORD & numButtons, WORD & currentButton);

		virtual Error Exit(Error err = GNR_OK) = 0;
	};

#endif
