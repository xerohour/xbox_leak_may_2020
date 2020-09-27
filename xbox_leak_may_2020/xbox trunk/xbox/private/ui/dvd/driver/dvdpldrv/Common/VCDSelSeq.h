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

#ifndef VCDSELSEQ_H
#define VCDSELSEQ_H

#include "VCDPlaySeq.h"

#define VCD_MAX_BUTTON_NUMBER 103

class VCDSelectionListSequencer : public VCDPlayListSequencer
	{
	protected:
		VCDPSD				*	psd;

		VCDPlayList			*	playList;
		VCDSelectionList	*	selectionList;
		VCDActionList		*	actionList;

		WORD pendingSelection;
		WORD currentListOffset;

		// VCDPlayListSequencer functions.
		virtual Error PlayListCompleted(Error error = GNR_OK);
		virtual void PlayItemCompleted(void);

		Error StartSelectionList(VCDSelectionList * list, DWORD flags = DDPSPF_NONE);
		virtual Error StartPlayList(VCDPlayList * list, DWORD flags = DDPSPF_NONE);

		BOOL IsValidOffset (WORD offset);

		//
		//  Freezing stuff
		//

		struct VCDSLSFreezeState
			{
			WORD currentListOffset;

			VCDPlayListSequencer::VCDPLSState vcdPLSState;
			};

		Error Freeze(VCDSLSFreezeState * buffer);
		Error Defrost(VCDSLSFreezeState * buffer, DWORD flags);

	public:
		VCDSelectionListSequencer(WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher);

		Error Initialize(VCDFileSystem * vcfs, VCDInfo * info, VCDEntries * entries,
							  VCDTracks * tracks, VCDSearch * search, VCDPSD * psd);

		Error StartActionList(WORD offset, DWORD flags = DDPSPF_NONE);
		Error AbortActionList(void);

		Error GoTopList(void);
		Error GoPreviousList(void);
		Error GoNextList(void);
		Error GoReturnList(void);
		Error GoDefaultList(void);
		Error GoSelectionList(WORD selection);
		Error GoSelectionListAt(int x, int y);

		Error TitlePlay(WORD title, DWORD flags = DDPSPF_NONE);
		Error TimePlay(WORD title, DVDTime time, BOOL toPause = FALSE);

		Error PreviousFunction(void);
		Error NextFunction(void);

		BOOL	IsButtonAt(int x, int y);
		BOOL	HasPositionalButtons(void);

		BOOL	InPlayList(void) {return playList != NULL;}
		BOOL	InSelectionList(void) {return selectionList != NULL;}

		WORD	CurrentList(void) {return currentListOffset;}

		Error GetAvailStreams(BYTE & audio, DWORD & subPicture);
		WORD	GetCurrentAudioStream(void);
		Error AudioStreamChange(WORD stream);

		Error GetCurrentButtonState(WORD & minButton, WORD & numButtons, WORD & currentButton);
		void	GetButtonInformation(WORD & numberOfButtons, WORD & userButtonOffset, DVDButtonInformation * info);
	};

#endif
