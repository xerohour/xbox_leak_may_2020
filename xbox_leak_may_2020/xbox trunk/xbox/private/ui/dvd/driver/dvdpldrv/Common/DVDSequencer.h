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

///////////////////////////////////////////////////////////////////////////////
//
// DVD Sequencer Module
//
// Description: Declares DVDSequencer. DVDSequencer is an abstract
//			class for all Sequencer classes.
//
//		DVDSequencer <-- DVDProgramChainSequencer <-- DVDTitleSequencer ...
//
//
///////////////////////////////////////////////////////////////////////////////


#ifndef DVDSEQUENCER_H
#define DVDSEQUENCER_H


#include "Breakpoint.h"


//
// Forward Declaration because of conflicts
// (DVDSequencer.h including DVDStreamServer.h
//	and DVDStreamServer.h including DVDSequencer.h)
//

class DVDCell;
class DVDVOBU;


////////////////////////////////////////////////////////////////////
//
//  DVDSequencer Class
//
////////////////////////////////////////////////////////////////////

class DVDSequencer : virtual public ERSBreakpointControl
	{
	friend class DVDVideoStreamServer;
	protected:
		virtual WORD CurrentAngle(void) = 0;
		virtual void AngleChanged(void) = 0;

		virtual BOOL BreakpointReached(DVDTime lowerLimit, DVDTime upperLimit, ERSPlaybackDir dir, ERSBreakpoint & bp) = 0;
		virtual BOOL BreakpointContained(int start, int length, BOOL lowest, ERSPlaybackDir dir, ERSBreakpoint & bp) = 0;
		virtual BOOL EndOfCellBreakpointReached(DVDCell * cell, ERSPlaybackDir dir, ERSBreakpoint & bp) = 0;
		virtual BOOL BeginOfCellBreakpointReached(DVDCell * cell, ERSPlaybackDir dir, ERSBreakpoint & bp) = 0;

		virtual Error VOBUStarted(DVDVOBU * vobu) {GNRAISE_OK;}
		virtual Error VOBUCompleted(DVDVOBU * vobu) {GNRAISE_OK;}

      virtual Error CellStarted(DVDCell * cell) {GNRAISE_OK;}
      virtual Error CellCompleted(DVDCell * cell) {GNRAISE_OK;}
		virtual Error CellCorrupt(DVDCell * cell) {GNRAISE_OK;}

		virtual Error PlaybackTerminated(Error err) {GNRAISE_OK;}

	public:
		DVDSequencer(EventDispatcher* pEventDispatcher)
			: EventSender(pEventDispatcher)
			, ERSBreakpointControl(pEventDispatcher)
			{}
		virtual ~DVDSequencer(void) {}
	};


#endif //DVDSEQUENCER_H
