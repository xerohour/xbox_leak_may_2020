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

////////////////////////////////////////////////////////////////////
//
//  Breakpoint Handling
//
////////////////////////////////////////////////////////////////////

#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include "Library/General/Lists.h"
#include "DVDTime.h"
#include "EventSender.h"
#include "Library/Common/GNErrors.h"

////////////////////////////////////////////////////////////////////
//
//  Helper macros with boolean results
//
////////////////////////////////////////////////////////////////////

#define BP_ATTIME(flags)		(flags & ERS_ATTIME)
#define BP_EOT(flags)			(flags & ERS_ENDOFTITLE)
#define BP_EOPTT(flags)			(flags & ERS_ENDOFPTT)
#define BP_FORWARD(flags)		(flags & ERS_FORWARD)
#define BP_BACKWARD(flags)		(flags & ERS_BACKWARD)
#define BP_TOSTILL(flags)		(flags & ERS_TOSTILL)
#define BP_CLEAR(flags)			(flags & ERS_CLEAR)
#define BP_BOPTT(flags)			(flags & ERS_BEGINOFPTT)

#define BP_END(flags)			(flags & (ERS_ENDOFTITLE | ERS_ENDOFPTT))
#define BP_DIR_OK(dir, flags)	(dir & flags)

#define BP_VALID(bp)				((bp).id != ERS_BREAKPOINT_NONE && !BP_ABANDONED(&(bp)))
#define BP_ACTIVE(bp)			((bp)->state == EBS_ACTIVE || (bp)->state == EBS_UPCOMING)
#define BP_UPCOMING(bp)			((bp)->state == EBS_UPCOMING)
#define BP_ABANDONED(bp)		((bp)->state == EBS_ABANDONED)

enum ERSPlaybackDir
	{
	EPD_FORWARD		= ERS_FORWARD,
	EPD_BACKWARD	= ERS_BACKWARD,
	EPD_BOTH			= ERS_FORWARD| ERS_BACKWARD
	};

enum ERSBreakpointState
	{
	EBS_INACTIVE,		// Breakpoint will currently not trigger
	EBS_ACTIVE,			// Breakpoint will trigger
	EBS_UPCOMING,		// Breakpoint is between BreakpointReached() and ExecuteBreakpoint()
	EBS_ABANDONED		// Breakpoint was upcoming, but should be deleted. Will be deleted once it is executed
	};

//
//  Breakpoint Class
//

class ERSBreakpoint : public Node
	{
	public:
		DWORD		id;
		WORD		title;
		WORD		ptt;
		DVDTime	time;
		DWORD		flags;
		ERSBreakpointState state;

		ERSBreakpoint(WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD id) : Node(), title(title), ptt(ptt),
																						time(time), flags(flags), id(id), state(EBS_INACTIVE) {}
		ERSBreakpoint(void) : Node(), id(ERS_BREAKPOINT_NONE), title(0), ptt(0), time(DVDTime()), flags(ERS_NONE), state(EBS_INACTIVE) {}
		~ERSBreakpoint(void) {}

		ERSBreakpoint & operator= (const ERSBreakpoint & bp)
			{ id = bp.id; title = bp.title; ptt = bp.ptt, time = bp.time; flags = bp.flags; state = EBS_INACTIVE; return *this;}

		void Clear(void)
			{ id = ERS_BREAKPOINT_NONE; title = 0; ptt = 0; time = DVDTime(); flags = ERS_NONE; state = EBS_INACTIVE; }
	};

//
//  Breakpoint Control Class
//

class ERSBreakpointControl : virtual public EventSender
	{
	private:
		List	breakpoints;
		DWORD	nextID;

		void	SetBreakpointActive(ERSBreakpoint * bp, BOOL active);

	protected:
		//
		//  Callbacks which can be overloaded by derived classes
		//

		virtual	void	StartBreakpointStillCallback(void)									{}
		virtual	void	SetBreakpointActiveCallback(ERSBreakpoint bp, BOOL active)	{}

	public:
		ERSBreakpointControl(EventDispatcher* pEventDispatcher);
		~ERSBreakpointControl(void);

		virtual	Error	SetBreakpoint(WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD & id);
					Error	GetBreakpoint(DWORD id, ERSBreakpoint & bp);
					Error	GetNextBreakpoint(WORD currentTitle, DVDTime currentTime, ERSPlaybackDir dir, ERSBreakpoint & bp);
					Error	GetPrevBreakpoint(WORD currentTitle, DVDTime currentTime, ERSPlaybackDir dir, ERSBreakpoint & bp);
					Error	ExecuteBreakpoint(DWORD id);
					Error	SetBreakpointActive(DWORD id, BOOL active);
					Error SetBreakpointUpcoming(DWORD id);
		virtual	Error	ClearBreakpoint(DWORD id);
		virtual	Error	ClearBreakpoints(void);
					BOOL	BreakpointReached(WORD title, DVDTime lowerLimit, DVDTime upperLimit,
													ERSPlaybackDir dir, ERSBreakpoint & bp);
					BOOL	BreakpointContained(WORD title, DVDTime lowerLimit, DVDTime upperLimit, BOOL lowest,
													  ERSPlaybackDir dir, ERSBreakpoint & bp);
					BOOL	EndOfTitleBreakpointReached(WORD title, ERSPlaybackDir dir, ERSBreakpoint & bp);
					BOOL	EndOfPTTBreakpointReached(WORD title, WORD ptt, ERSPlaybackDir dir, ERSBreakpoint & bp);
					BOOL	BeginOfPTTBreakpointReached(WORD title, WORD ptt, ERSPlaybackDir dir, ERSBreakpoint & bp);
	};

#endif
