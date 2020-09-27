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

#include "Breakpoint.h"
#include "library/common/vddebug.h"

#define DEBUG_TIME_BREAKPOINT 0

////////////////////////////////////////////////////////////////////
//
//  ERSBreakpointControl Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

ERSBreakpointControl::ERSBreakpointControl(EventDispatcher * eventDispatcher) : breakpoints()
	, EventSender(pEventDispatcher)
	{
	nextID = 0;
	}

//
//  Destructor
//

ERSBreakpointControl::~ERSBreakpointControl(void)
	{
	ClearBreakpoints();
	}

//
//  Set Breakpoint
//

Error ERSBreakpointControl::SetBreakpoint(WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD & id)
	{
	ERSBreakpoint * bp = new ERSBreakpoint(title, ptt, time, flags, nextID);

	breakpoints.Push(bp);
	id = nextID;
	nextID++;
	if (nextID == ERS_BREAKPOINT_NONE)
		nextID++;
	GNRAISE_OK;
	}

//
//  Get Breakpoint (ID is ERS_BREAKPOINT_NONE if there is none)
//

Error ERSBreakpointControl::GetBreakpoint(DWORD id, ERSBreakpoint & breakpoint)
	{
	ERSBreakpoint * bp = (ERSBreakpoint*)breakpoints.First();

	breakpoint.id = ERS_BREAKPOINT_NONE;

	while (bp != NULL)
		{
		if (bp->id == id && !BP_ABANDONED(bp))
			{
			breakpoint = *bp;
			GNRAISE_OK;
			}
		else
			bp = (ERSBreakpoint*)bp->Succ();
		}

	GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Get Next Breakpoint (ID is ERS_BREAKPOINT_NONE if there is none)
//  Note that this works only for breakpoints which do not have the ERS_ENDOFPTT flag set
//

Error ERSBreakpointControl::GetNextBreakpoint(WORD currentTitle, DVDTime currentTime, ERSPlaybackDir dir,
															 ERSBreakpoint & breakpoint)
	{
	ERSBreakpoint * bp = (ERSBreakpoint*)breakpoints.First();
	BOOL found = FALSE;

	breakpoint.id = ERS_BREAKPOINT_NONE;
	breakpoint.title = 0xffff;
	breakpoint.time = DVDTime(99, 59, 59, currentTime.FrameRate() - 1, currentTime.FrameRate());

	while (bp != NULL)
		{
		if (!BP_ABANDONED(bp))
			{
			if (BP_DIR_OK(dir, bp->flags) && (bp->title > currentTitle || (bp->title == currentTitle && (bp->time > currentTime || BP_EOT(bp->flags)))))
				{
				if (bp->title < breakpoint.title || (bp->title == breakpoint.title && (bp->time < breakpoint.time || BP_EOT(breakpoint.flags))))
					{
					breakpoint = *bp;
					found = TRUE;
					}
				}
			}
		bp = (ERSBreakpoint*)bp->Succ();
		}

	if (found)
		GNRAISE_OK;
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Get Previous Breakpoint (ID is ERS_BREAKPOINT_NONE if there is none)
//  Note that this works only for breakpoints which do not have the ERS_ENDOFPTT flag set
//

Error ERSBreakpointControl::GetPrevBreakpoint(WORD currentTitle, DVDTime currentTime, ERSPlaybackDir dir,
															 ERSBreakpoint & breakpoint)
	{
	ERSBreakpoint * bp = (ERSBreakpoint*)breakpoints.First();
	BOOL found = FALSE;

	breakpoint.id = ERS_BREAKPOINT_NONE;
	breakpoint.title = 0;
	breakpoint.time = DVDTime(0, 0, 0, 0, currentTime.FrameRate());

	while (bp != NULL)
		{
		if (!BP_ABANDONED(bp))
			{
			if (BP_DIR_OK(dir, bp->flags) && (bp->title < currentTitle || (bp->title == currentTitle && bp->time < currentTime && !BP_EOT(bp->flags))))
				{
//				DP("Comparing %d-%02d:%02d:%02d and %d-%02d:%02d:%02d", bp->title, bp->time.Minutes(), bp->time.Seconds(), bp->time.Frames(), breakpoint.title, breakpoint.time.Minutes(), breakpoint.time.Seconds(), breakpoint.time.Frames());
				if (bp->title > breakpoint.title || (bp->title == breakpoint.title && (bp->time > breakpoint.time || BP_EOT(bp->flags))))
					{
					breakpoint = *bp;
					found = TRUE;
					}
				}
			}
		bp = (ERSBreakpoint*)bp->Succ();
		}

	if (found)
		GNRAISE_OK;
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Execute Breakpoint
//

Error ERSBreakpointControl::ExecuteBreakpoint(DWORD id)
	{
	ERSBreakpoint * bp = (ERSBreakpoint*)breakpoints.First();

	while (bp != NULL)
		{
		if (bp->id == id)
			{
#if DEBUG_TIME_BREAKPOINT
			DP("Executing breakpoint # %d", id);
#endif
			if (BP_TOSTILL(bp->flags))
				StartBreakpointStillCallback();
			SetBreakpointActive(bp, FALSE);
			SendEvent(DNE_BREAKPOINT_REACHED, id);
			if (BP_CLEAR(bp->flags) || BP_ABANDONED(bp))
				ClearBreakpoint(id);
			GNRAISE_OK;
			}
		else
			bp = (ERSBreakpoint*)bp->Succ();
		}

	GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Clear Breakpoint
//

Error ERSBreakpointControl::ClearBreakpoint(DWORD id)
	{
	ERSBreakpoint * bp = (ERSBreakpoint*)breakpoints.First();

	while (bp != NULL)
		{
		if (bp->id == id)
			{
			if (BP_UPCOMING(bp))
				{
				DPF("Abandoned ....\n");
				bp->state = EBS_ABANDONED;
				}
			else
				{
				SetBreakpointActive(bp->id, FALSE);
				breakpoints.Remove((ERSBreakpoint*)bp);
				delete bp;
				}
			GNRAISE_OK;
			}
		bp = (ERSBreakpoint*)bp->Succ();
		}

	GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Clear all breakpoints
//

Error ERSBreakpointControl::ClearBreakpoints(void)
	{
	ERSBreakpoint * bp;
	int i;
	int n = breakpoints.Num();

	for (i=0; i<n; i++)
		{
		bp = (ERSBreakpoint*)breakpoints.Pop();
		delete bp;
		}

	GNRAISE_OK;
	}

//
//  Test if breakpoint has been reached and return it if so (else ID is ERS_BREAKPOINT_NONE)
//

BOOL ERSBreakpointControl::BreakpointReached(WORD title, DVDTime lowerLimit, DVDTime upperLimit, ERSPlaybackDir dir, ERSBreakpoint & breakpoint)
	{
	ERSBreakpoint * bp = (ERSBreakpoint*)breakpoints.First();
	BOOL found = FALSE;

//	DPF("Checking between %d-%02d:%02d:%02d:%02d (%02d) and %d-%02d:%02d:%02d:%02d (%02d)\n", title, lowerLimit.Hours(), lowerLimit.Minutes(), lowerLimit.Seconds(), lowerLimit.Frames(), lowerLimit.FrameRate(), title, upperLimit.Hours(), upperLimit.Minutes(), upperLimit.Seconds(), upperLimit.Frames(), upperLimit.FrameRate());
	breakpoint.id = ERS_BREAKPOINT_NONE;
	while (bp != NULL)
		{
//		DPF("Checking BP %d (%02d-%02d:%02d:%02d:%02d (%02d))\n", bp->id, bp->title, bp->time.Hours(), bp->time.Minutes(), bp->time.Seconds(), bp->time.Frames(), bp->time.FrameRate());
//		if (!BP_ABANDONED(bp))
			{
			if (BP_ATTIME(bp->flags) && lowerLimit <= bp->time && upperLimit >= bp->time && title == bp->title &&
				 BP_DIR_OK(dir, bp->flags))
				{
				//
				//  Breakpoint found
				//

				if (BP_ACTIVE(bp))
					{
					breakpoint = *bp;
					found = TRUE;
					}
				else
					DP("Not active");
				}
			else
				{
				SetBreakpointActive(bp, TRUE);
				}
			}

		bp = (ERSBreakpoint*)bp->Succ();
		}

	return found;
	}

//
//  Test if a breakpoint is contained inside an interval (else ID is ERS_BREAKPOINT_NONE)
//  This function does not activate breakpoints as BreakpointReached does. It finds either the first
//  or last breakpoint in the interval
//

BOOL ERSBreakpointControl::BreakpointContained(WORD title, DVDTime lowerLimit, DVDTime upperLimit, BOOL lowest,
															  ERSPlaybackDir dir, ERSBreakpoint & breakpoint)
	{
	ERSBreakpoint * bp = (ERSBreakpoint*)breakpoints.First();
	BOOL found = FALSE;

//	DP("Contain Check between %d-%02d:%02d:%02d:%02d (%02d) and %d-%02d:%02d:%02d:%02d (%02d)", title, lowerLimit.Hours(), lowerLimit.Minutes(), lowerLimit.Seconds(), lowerLimit.Frames(), lowerLimit.FrameRate(), title, upperLimit.Hours(), upperLimit.Minutes(), upperLimit.Seconds(), upperLimit.Frames(), upperLimit.FrameRate());
	breakpoint.id = ERS_BREAKPOINT_NONE;
	while (bp != NULL)
		{
//		DP("Contain Check BP %d (%02d-%02d:%02d:%02d:%02d (%02d))", bp->id, bp->title, bp->time.Hours(), bp->time.Minutes(), bp->time.Seconds(), bp->time.Frames(), bp->time.FrameRate());
		if (!BP_ABANDONED(bp))
			{
			if (BP_ATTIME(bp->flags) && lowerLimit <= bp->time && upperLimit >= bp->time && title == bp->title &&
				 BP_DIR_OK(dir, bp->flags))
				{
				//
				//  Breakpoint found
				//

				if (BP_ACTIVE(bp))
					{
					if (!found || (lowest && bp->time < breakpoint.time) || (!lowest && bp->time > breakpoint.time))
						{
						breakpoint = *bp;
						found = TRUE;
						}
					}
				}
			}

		bp = (ERSBreakpoint*)bp->Succ();
		}

	return found;
	}

//
//  Test if end-of-title breakpoint has been reached and return it if so (else ID is ERS_BREAKPOINT_NONE)
//

BOOL ERSBreakpointControl::EndOfTitleBreakpointReached(WORD title, ERSPlaybackDir dir, ERSBreakpoint & breakpoint)
	{
	ERSBreakpoint * bp = (ERSBreakpoint*)breakpoints.First();
	BOOL found = FALSE;

	breakpoint.id = ERS_BREAKPOINT_NONE;
	while (bp != NULL)
		{
//		DP("Checking %d with %d %d:%d:%d:%d:%d", title, bp->title, bp->time.Hours(), bp->time.Minutes(), bp->time.Seconds(), bp->time.Frames(), bp->time.FrameRate());
		if (!BP_ABANDONED(bp) && BP_EOT(bp->flags) && title == bp->title && BP_DIR_OK(dir, bp->flags))
			{
			//
			//  Breakpoint found
			//

			if (BP_ACTIVE(bp))
				{
				breakpoint = *bp;
				found = TRUE;
				}
			}

		bp = (ERSBreakpoint*)bp->Succ();
		}

	return found;
	}

//
//  Test if end-of-part-of-title breakpoint was reached
//

BOOL ERSBreakpointControl::EndOfPTTBreakpointReached(WORD title, WORD ptt, ERSPlaybackDir dir, ERSBreakpoint & breakpoint)
	{
	ERSBreakpoint * bp = (ERSBreakpoint*)breakpoints.First();
	BOOL found = FALSE;

	breakpoint.id = ERS_BREAKPOINT_NONE;
	while (bp != NULL)
		{
		if (!BP_ABANDONED(bp) && BP_EOPTT(bp->flags) && title == bp->title && ptt == bp->ptt && BP_DIR_OK(dir, bp->flags))
			{
			if (BP_ACTIVE(bp))
				{
				breakpoint = *bp;
				found = TRUE;
				}
			}

		bp = (ERSBreakpoint*)bp->Succ();
		}

	return found;
	}

//
//  Check for Begin-Of-PTT breakpoint
//

BOOL ERSBreakpointControl::BeginOfPTTBreakpointReached(WORD title, WORD ptt, ERSPlaybackDir dir, ERSBreakpoint & breakpoint)
	{
	ERSBreakpoint * bp = (ERSBreakpoint*)breakpoints.First();
	BOOL found = FALSE;

	breakpoint.id = ERS_BREAKPOINT_NONE;
	while (bp != NULL)
		{
		if (!BP_ABANDONED(bp) && BP_BOPTT(bp->flags) && title == bp->title && ptt == bp->ptt && BP_DIR_OK(dir, bp->flags))
			{
			if (BP_ACTIVE(bp))
				{
				breakpoint = *bp;
				found = TRUE;
				}
			}

		bp = (ERSBreakpoint*)bp->Succ();
		}

	return found;
	}

//
//  Set breakpoint activation state (internal)
//

void ERSBreakpointControl::SetBreakpointActive(ERSBreakpoint * bp, BOOL active)
	{
	if (!BP_ABANDONED(bp))
		{
		bp->state = active ? EBS_ACTIVE : EBS_INACTIVE;
		SetBreakpointActiveCallback(*bp, active);
		}
	}

//
//  Set breakpoint activation state
//

Error ERSBreakpointControl::SetBreakpointActive(DWORD id, BOOL active)
	{
	ERSBreakpoint * bp = (ERSBreakpoint*)breakpoints.First();

	while (bp != NULL)
		{
		if (bp->id == id)
			{
			SetBreakpointActive(bp, active);
			GNRAISE_OK;
			}
		bp = (ERSBreakpoint*)bp->Succ();
		}

	GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Set breakpoint to upcoming
//

Error ERSBreakpointControl::SetBreakpointUpcoming(DWORD id)
	{
	ERSBreakpoint * bp = (ERSBreakpoint*)breakpoints.First();

	while (bp != NULL)
		{
		if (bp->id == id && !BP_ABANDONED(bp))
			{
			bp->state = EBS_UPCOMING;
			GNRAISE_OK;
			}
		bp = (ERSBreakpoint*)bp->Succ();
		}

	GNRAISE(GNR_OBJECT_NOT_FOUND);
	}
