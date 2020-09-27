////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
//  VCD Selection List Sequencer
//
////////////////////////////////////////////////////////////////////

#include "Library/Common/vddebug.h"
#include "VCDSelSeq.h"

////////////////////////////////////////////////////////////////////
//
//  Macro Definitions
//
////////////////////////////////////////////////////////////////////

#define CHECK_UOPS(command) { DWORD oldUOPs;												\
										DWORD newUOPs;												\
										GNREASSERT(GetUOPs(oldUOPs));							\
										command;														\
										GNREASSERT(GetUOPs(newUOPs));							\
										if (oldUOPs != newUOPs)									\
											SendEvent(DNE_VALID_UOP_CHANGE, newUOPs);		\
										}

////////////////////////////////////////////////////////////////////
//
//  Function Definitions
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

VCDSelectionListSequencer::VCDSelectionListSequencer(WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher)
	: VCDPlayListSequencer(server, units, pEventDispatcher)
	, EventSender(pEventDispatcher)
	, ERSBreakpointControl(pEventDispatcher)
	{
	playList = NULL;
	selectionList = NULL;
	actionList = NULL;
	pendingSelection = 0;
	}

//
//  Initialize Sequencer
//

Error VCDSelectionListSequencer::Initialize(VCDFileSystem * vcdfs, VCDInfo * info, VCDEntries * entries,
														  VCDTracks * tracks, VCDSearch * search, VCDPSD * psd)
	{
	this->psd = psd;

	GNREASSERT(VCDPlayListSequencer::Initialize(vcdfs, info, entries, tracks, search));

	GNRAISE_OK;
	}

//
//  Play List Completed
//

Error VCDSelectionListSequencer::PlayListCompleted(Error error)
	{
	Error err;

	if (error == GNR_ITEM_NOT_FOUND)
		{
		err = AbortActionList();
		GNRAISE(err);
		}
	if (playList)
		{
		err = GoNextList();
		if (IS_ERROR(err))
			{
			AbortActionList();
			GNRAISE(err);
			}
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);

	GNRAISE_OK;
	}

//
//  Calculate Random Number
//

static WORD Rnd(WORD max)
	{
	static DWORD seed = ::timeGetTime();
	seed = 1664525L * seed + 1013904223L;
	return (WORD)(seed % max);
	}

//
//  Play Item Completed
//

void VCDSelectionListSequencer::PlayItemCompleted(void)
	{
	WORD w;

	if (selectionList)
		{
		if (pendingSelection)
			{
			w = selectionList->SelectionOffset(pendingSelection);
			pendingSelection = 0;
			}
		else
			{
			w = selectionList->TimeOutListOffset();
			if (w == 0xffff)
				{
				w =  (WORD)(selectionList->SelectionOffset((WORD)(selectionList->BaseOfSelectionNumber() + Rnd((WORD)(selectionList->NumberOfSelections())))));
				}
			}

		if (IsValidOffset(w))
			{
			AbortActionList();
			StartActionList(w);
			}
		else
			{
			PlayListCompleted(GNR_ITEM_NOT_FOUND);
			EnterStopState();
			}
		}
	else
		EnterStopState();
	}

//
//  Start A Selection List
//

Error VCDSelectionListSequencer::StartSelectionList(VCDSelectionList * list, DWORD flags)
	{
	CHECK_UOPS(selectionList = list);
	actionList = list;

	pendingSelection = 0;

	GNREASSERT(VCDPlayListSequencer::StartPlayItem(list->PlayItemNumber(), list->TimeOutTime(), list->LoopCount() - 1,
																  0, (flags & DDPSPF_TOPAUSE) != 0));

	GNRAISE_OK;
	}

//
//  Start A Play List
//

Error VCDSelectionListSequencer::StartPlayList(VCDPlayList * list, DWORD flags)
	{
	CHECK_UOPS(playList = list; actionList = list);

	GNREASSERT(VCDPlayListSequencer::StartPlayList(list, flags));

	GNRAISE_OK;
	}

//
//  Check If Offset Is Valid
//

BOOL VCDSelectionListSequencer::IsValidOffset (WORD offset)
	{
	if (psd == NULL)
		return TRUE;

	if (offset == 0xffff)
		return FALSE;

	offset *= (WORD)(info->OffsetMultiplier());

	return (psd->IsPlayList(offset)  ||  psd->IsSelectionList(offset));
	}

//
//  Start Action List
//

Error VCDSelectionListSequencer::StartActionList(WORD offset, DWORD flags)
	{
	if (psd)
		{
		if (!actionList)
			{
			currentListOffset = offset;

			offset *= (WORD)(info->OffsetMultiplier());

			if (psd->IsPlayList(offset))
				{
				GNREASSERT(StartPlayList(psd->GetPlayList(offset), flags));
				}
			else if (psd->IsSelectionList(offset))
				{
				GNREASSERT(StartSelectionList(psd->GetSelectionList(offset), flags));
				}
			else
				{
				}
			}
		else
			GNRAISE(GNR_OBJECT_IN_USE);
		}

	GNRAISE_OK;
	}

//
//  Abort Action List
//

Error VCDSelectionListSequencer::AbortActionList(void)
	{
	//
	//  Stop current playback
	//

	GNREASSERT(AbortPlayList());

	//
	//  Invalidate lists
	//

	if (actionList)
		{
		delete actionList;

		actionList = NULL;
		CHECK_UOPS(playList = NULL; selectionList = NULL);
		}

	GNRAISE_OK;
	}

//
//  Go To Top Of List
//

Error VCDSelectionListSequencer::GoTopList(void)
	{
	if (actionList)
		{
		GNREASSERT(AbortActionList());
		GNREASSERTMAP(StartActionList(currentListOffset), GNR_OBJECT_NOT_FOUND);
		GNRAISE_OK;
		}

	GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Go To Previous List
//

Error VCDSelectionListSequencer::GoPreviousList(void)
	{
	WORD w;

	if (actionList)
		{
		w = actionList->PreviousListOffset();
		if (IsValidOffset(w))
			{
			GNREASSERT(AbortActionList());
			GNREASSERTMAP(StartActionList(w), GNR_OBJECT_NOT_FOUND);
			GNRAISE_OK;
			}
		}

	GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Go To Next List
//

Error VCDSelectionListSequencer::GoNextList(void)
	{
	WORD w;

	if (actionList)
		{
		w = actionList->NextListOffset();
		if (IsValidOffset(w))
			{
			GNREASSERT(AbortActionList());
			GNREASSERTMAP(StartActionList(w), GNR_OBJECT_NOT_FOUND);
			GNRAISE_OK;
			}
		}

	GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Go To Return List
//

Error VCDSelectionListSequencer::GoReturnList(void)
	{
	WORD w;

	if (actionList)
		{
		w = actionList->ReturnListOffset();
		if (IsValidOffset(w))
			{
			GNREASSERT(AbortActionList());
			GNREASSERTMAP(StartActionList(w), GNR_OBJECT_NOT_FOUND);
			}
		}

	GNRAISE_OK;
	}

//
//  Go To Default List
//

Error VCDSelectionListSequencer::GoDefaultList(void)
	{
	WORD w;

	if (selectionList)
		{
		w = selectionList->DefaultListOffset();
		if (IsValidOffset(w))
			{
			GNREASSERT(AbortActionList());
			GNREASSERTMAP(StartActionList(w), GNR_OBJECT_NOT_FOUND);
			GNRAISE_OK;
			}
		}

	GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Go To A Selection
//

Error VCDSelectionListSequencer::GoSelectionList(WORD selection)
	{
	WORD w;

	if (selectionList)
		{
		if (selection >= selectionList->BaseOfSelectionNumber() &&
			 selection <  selectionList->BaseOfSelectionNumber() + selectionList->NumberOfSelections())
			{
			w = selectionList->SelectionOffset(selection);
			if (IsValidOffset(w))
				{
				if (selectionList->JumpTiming())
					{
					pendingSelection = selection;
					GNREASSERT(CompletePlayItem());
					}
				else
					{
					GNREASSERT(AbortActionList());
					GNREASSERT(StartActionList(w));
					}
				}
			}
		}

	GNRAISE_OK;
	}

//
//  Execute Previous Function
//

Error VCDSelectionListSequencer::PreviousFunction(void)
	{
	Error err;

	if (selectionList)
		return GoPreviousList();
	else
		{
		err = GoPrevItem();
		if (err == GNR_OBJECT_NOT_FOUND)
			return GoPreviousList();
		else
			GNRAISE(err);
		}
	}

//
//  Execute Next Function
//

Error VCDSelectionListSequencer::NextFunction(void)
	{
	Error err;

	if (selectionList)
		return GoNextList();
	else
		{
		err = GoNextItem();
		if (err == GNR_OBJECT_NOT_FOUND)
			return GoNextList();
		else
			GNRAISE(err);
		}
	}

//
//  Check A Selection
//

static BOOL CheckSelection(DWORD pos, int x, int y)
	{
	return (x >= (int)XTBF(24, 8, pos) && x <= (int)XTBF(8, 8, pos) &&
		     y >= (int)XTBF(16, 8, pos) && y <= (int)XTBF(0, 8, pos));
	}

//
//  Return Button Information
//

void VCDSelectionListSequencer::GetButtonInformation(WORD & numberOfButtons, WORD & userButtonOffset,
																	  DVDButtonInformation * info)
	{
	DWORD	position;
	int	i, x, y, w, h;

	//
	//  Test on (extended) selection list
	//

	numberOfButtons = 0;
	if (selectionList != NULL)
		{
		userButtonOffset = selectionList->BaseOfSelectionNumber();

		//
		// Get standard buttons
		//

		if (position = selectionList->PreviousListSelectionArea() != 0)
			{
			x = (int)XTBF(position, 24, 8);
			y = (int)XTBF(position, 16, 8);
			w = (int)XTBF(position, 8, 8) - x + 1;
			h = (int)XTBF(position, 0, 8) - y + 1;
			player->TranslateButtonPosition(x, y);
			player->TranslateButtonPosition(w, h);
			info[numberOfButtons].x = (WORD)x;
			info[numberOfButtons].y = (WORD)y;
			info[numberOfButtons].w = (WORD)w;
			info[numberOfButtons].h = (WORD)h;
			numberOfButtons++;
			}

		if (position = selectionList->NextListSelectionArea() != 0)
			{
			x = (int)XTBF(position, 24, 8);
			y = (int)XTBF(position, 16, 8);
			w = (int)XTBF(position, 8, 8) - x + 1;
			h = (int)XTBF(position, 0, 8) - y + 1;
			player->TranslateButtonPosition(x, y);
			player->TranslateButtonPosition(w, h);
			info[numberOfButtons].x = (WORD)x;
			info[numberOfButtons].y = (WORD)y;
			info[numberOfButtons].w = (WORD)w;
			info[numberOfButtons].h = (WORD)h;
			numberOfButtons++;
			}

		if (position = selectionList->ReturnListSelectionArea() != 0)
			{
			x = (int)XTBF(position, 24, 8);
			y = (int)XTBF(position, 16, 8);
			w = (int)XTBF(position, 8, 8) - x + 1;
			h = (int)XTBF(position, 0, 8) - y + 1;
			player->TranslateButtonPosition(x, y);
			player->TranslateButtonPosition(w, h);
			info[numberOfButtons].x = (WORD)x;
			info[numberOfButtons].y = (WORD)y;
			info[numberOfButtons].w = (WORD)w;
			info[numberOfButtons].h = (WORD)h;
			numberOfButtons++;
			}

		if (position = selectionList->DefaultListSelectionArea() != 0)
			{
			x = (int)XTBF(position, 24, 8);
			y = (int)XTBF(position, 16, 8);
			w = (int)XTBF(position, 8, 8) - x + 1;
			h = (int)XTBF(position, 0, 8) - y + 1;
			player->TranslateButtonPosition(x, y);
			player->TranslateButtonPosition(w, h);
			info[numberOfButtons].x = (WORD)x;
			info[numberOfButtons].y = (WORD)y;
			info[numberOfButtons].w = (WORD)w;
			info[numberOfButtons].h = (WORD)h;
			numberOfButtons++;
			}

		//
		// Get all other buttons
		//

		for (i=0; i<selectionList->NumberOfSelections(); i++)
			{
			if (position = selectionList->SelectionArea(i + userButtonOffset) != 0)
				{
				x = (int)XTBF(position, 24, 8);
				y = (int)XTBF(position, 16, 8);
				w = (int)XTBF(position, 8, 8) - x + 1;
				h = (int)XTBF(position, 0, 8) - y + 1;
				player->TranslateButtonPosition(x, y);
				player->TranslateButtonPosition(w, h);
				info[numberOfButtons].x = (WORD)x;
				info[numberOfButtons].y = (WORD)y;
				info[numberOfButtons].w = (WORD)w;
				info[numberOfButtons].h = (WORD)h;
				}
			else
				{
				info[numberOfButtons].x = 0;
				info[numberOfButtons].y = 0;
				info[numberOfButtons].w = 0;
				info[numberOfButtons].h = 0;
				}
			numberOfButtons++;
			}

		//
		// Clear unused data
		//

		for (i=0; i<numberOfButtons; i++)
			info[i].autoAction = info[i].upperButton = info[i].lowerButton = info[i].leftButton = info[i].rightButton = 0;
		}
	}

//
//  Check If There Are Positional Buttons
//

BOOL VCDSelectionListSequencer::HasPositionalButtons(void)
	{
	return selectionList != NULL;
	}

//
//  Check If There Is A Button At A Certain Position
//

BOOL VCDSelectionListSequencer::IsButtonAt(int x, int y)
	{
	int sel;

	if (selectionList)
		{
		GNREASSERT(player->TranslateButtonPosition(x, y));

		if      (CheckSelection(selectionList->PreviousListSelectionArea(), x, y))
			return TRUE;
		else if (CheckSelection(selectionList->NextListSelectionArea(), x, y))
			return TRUE;
		else if (CheckSelection(selectionList->ReturnListSelectionArea(), x, y))
			return TRUE;
		else if (CheckSelection(selectionList->DefaultListSelectionArea(), x, y))
			return TRUE;
		else
			{
			sel = selectionList->BaseOfSelectionNumber();

			while (sel < selectionList->BaseOfSelectionNumber() + selectionList->NumberOfSelections())
				{
				if (CheckSelection(selectionList->SelectionArea(sel), x, y))
					{
					return TRUE;
					}
				sel++;
				}
			}
		}

	return FALSE;
	}

//
//  Go To Selection Given By Mouse Position
//

Error VCDSelectionListSequencer::GoSelectionListAt(int x, int y)
	{
	int sel;

	if (selectionList)
		{
		GNREASSERT(player->TranslateButtonPosition(x, y));

		if      (CheckSelection(selectionList->PreviousListSelectionArea(), x, y))
			GNREASSERT(GoPreviousList());
		else if (CheckSelection(selectionList->NextListSelectionArea(), x, y))
			GNREASSERT(GoNextList());
		else if (CheckSelection(selectionList->ReturnListSelectionArea(), x, y))
			GNREASSERT(GoReturnList());
		else if (CheckSelection(selectionList->DefaultListSelectionArea(), x, y))
			GNREASSERT(GoDefaultList());
		else
			{
			sel = selectionList->BaseOfSelectionNumber();

			while (sel < selectionList->BaseOfSelectionNumber() + selectionList->NumberOfSelections())
				{
				if (CheckSelection(selectionList->SelectionArea(sel), x, y))
					{
					return GoSelectionList((WORD)sel);
					}
				sel++;
				}
			}
		}

	GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Title Play
//

Error VCDSelectionListSequencer::TitlePlay(WORD title, DWORD flags)
	{
	AbortActionList();
	GNREASSERT(StartTrackAt(title + 1, 0, (flags & DDPSPF_TOPAUSE) != 0));

	GNRAISE_OK;
	}

//
//  Time Play
//

Error VCDSelectionListSequencer::TimePlay(WORD title, DVDTime time, BOOL toPause)
	{
	AbortActionList();
	return StartTrackAt(title + 1, time.Millisecs() * 3 / 40, toPause);
	}

//
//  Get available streams
//

Error VCDSelectionListSequencer::GetAvailStreams(BYTE & audio, DWORD & subPicture)
	{
	return VCDPlayListSequencer::GetAvailStreams(audio, subPicture);
	}

//
//  Get current audio stream
//

WORD VCDSelectionListSequencer::GetCurrentAudioStream(void)
	{
	return VCDPlayListSequencer::GetCurrentAudioStream();
	}

//
//  Audio stream change
//

Error VCDSelectionListSequencer::AudioStreamChange(WORD stream)
	{
	return VCDPlayListSequencer::AudioStreamChange(stream);
	}

//
//  Get State Of Buttons
//

Error VCDSelectionListSequencer::GetCurrentButtonState(WORD & minButton, WORD & numButtons, WORD & currentButton)
	{
	if (selectionList)
		{
		minButton = selectionList->BaseOfSelectionNumber();
		numButtons = (WORD)(selectionList->NumberOfSelections());
		}
	else
		{
		minButton = 0;
		numButtons = 0;
		}

	currentButton = 0xffff;

	GNRAISE_OK;
	}

//
//  Freeze current player state
//

Error VCDSelectionListSequencer::Freeze(VCDSLSFreezeState * buffer)
	{
	//
	//  Save lower level state
	//

	GNREASSERT(VCDPlayListSequencer::Freeze(&(buffer->vcdPLSState)));

	//
	//  Save own state
	//

	buffer->currentListOffset = currentListOffset;

	GNRAISE_OK;
	}

//
//  Restore previously saved player state
//

Error VCDSelectionListSequencer::Defrost(VCDSLSFreezeState * buffer, DWORD flags)
	{
	WORD offset;

	if (psd)
		{
		//
		//  Stop Current Playback
		//

		GNREASSERT(AbortActionList());

		//
		//  Restore own state
		//

		offset = buffer->currentListOffset * info->OffsetMultiplier();

		if (psd->IsPlayList(offset))
			{
			playList = psd->GetPlayList(offset);
			selectionList = NULL;
			actionList = playList;
			GNREASSERT(VCDPlayListSequencer::Defrost(&(buffer->vcdPLSState), playList, flags));
			}
		else if (psd->IsSelectionList(offset))
			{
			playList = NULL;
			StartSelectionList(psd->GetSelectionList(offset));
			}
		else if (psd->IsEndList(offset))
			{
			}
		else
			GNRAISE(GNR_OBJECT_INVALID);
		}
	else
		{
		GNREASSERT(VCDPlayListSequencer::Defrost(&(buffer->vcdPLSState), NULL, flags));
		}

	GNRAISE_OK;
	}
