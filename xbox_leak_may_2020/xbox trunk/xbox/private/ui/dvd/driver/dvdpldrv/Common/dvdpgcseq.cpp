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
//  DVD Program Chain Sequencer Implementation
//
//////////////////////////////////////////////////////////////////////

#include "DVDPGCSeq.h"
#include "Library\Common\vddebug.h"
#include "Library\lowlevel\timer.h"

//////////////////////////////////////////////////////////////////////
//
//  Navigation Timer Class
//
//////////////////////////////////////////////////////////////////////

DVDNavigationTimer::DVDNavigationTimer(WinPortServer * server, DVDNavigationControl * navControl)
	: WinPort(server)
	{
	this->navControl = navControl;
	active = FALSE;
	paused = FALSE;
	started = FALSE;
	}

DVDNavigationTimer::~DVDNavigationTimer(void)
	{
	}

Error DVDNavigationTimer::InitNavigationTimer(DWORD time, WORD pgcn)
	{
	CancelTimer();
	FlushMessages();

	active = TRUE;

	this->pgcn = pgcn;
	this->time = time;

	if (started)
		{
		ScheduleTimer(1000, time);

		time = Timer.GetMilliTicks() + time;
		}

	GNRAISE_OK;
	}

void DVDNavigationTimer::Message (WPARAM wParam, LPARAM lParam)
	{
	if (active)
		{
		active = FALSE;
		started = FALSE;
		navControl->GoProgramChain(pgcn);
		}
	}

Error DVDNavigationTimer::StartNavigationTimer(void)
	{
	if (!started && active && !paused)
		{
		started = TRUE;

		ScheduleTimer(1000, time);

		time = Timer.GetMilliTicks() + time;

		GNRAISE_OK;
		}
	else
		{
		started = TRUE;

		GNRAISE_OK;
		}
	}

Error DVDNavigationTimer::StopNavigationTimer(void)
	{
	if (started && active && !paused)
		{
		time = time - Timer.GetMilliTicks();

		CancelTimer();
		FlushMessages();
		}

	started = FALSE;

	GNRAISE_OK;
	}

Error DVDNavigationTimer::ResetNavigationTimer(void)
	{
	this->active = FALSE;
	this->paused = FALSE;
	CancelTimer();
	FlushMessages();

	GNRAISE_OK;
	}

Error DVDNavigationTimer::QueryNavigationTimer(DWORD & time, BOOL & active)
	{
	active = this->active;

	if (this->active)
		{
		if (paused || !started)
			time = this->time;
		else
			time = this->time - Timer.GetMilliTicks();
		}
	else
		time = 0;

	GNRAISE_OK;
	}

Error DVDNavigationTimer::PauseNavigationTimer(void)
	{
	if (active && !paused)
		{
		paused = true;
		CancelTimer();
		FlushMessages();

		if (started)
			time = time - Timer.GetMilliTicks();
		}

	GNRAISE_OK;
	}

Error DVDNavigationTimer::ResumeNavigationTimer(void)
	{
	if (active && paused)
		{
		paused = false;

		if (started)
			{
			ScheduleTimer(1000, time);

			time = time + Timer.GetMilliTicks();
			}
		}

	GNRAISE_OK;
	}

////////////////////////////////////////////////////////////////////
//
//  Program Chain Sequencer
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

#pragma warning(disable : 4355)
DVDPGCSequencer::DVDPGCSequencer (WinPortServer * server, UnitSet units, EventDispatcher * eventDispatcher)
	: DVDSequencer(eventDispatcher)
	, DVDNavigationControl()
	, DVDHighlightControl()
	, EventSender(eventDispatcher)
	, ERSBreakpointControl(eventDispatcher)
	, navigationTimer(server, this)
	{
	SetState(pgcs_idle);
	shuffleHistory = NULL;
	playbackShuffleHistory = NULL;
	cellTimes = NULL;
	angle = 1;
	playingAngle = 1;
	requestedPresentationMode = DPM_16BY9;
	scanSpeed = 1;
	pmlHandling = PMH_COMPATIBLE;
	breakpointStillState = pgcs_idle;
	isMultiAngleScene = FALSE;
	streaming = FALSE;
	navpu = NULL;
	state = pgcs_idle;
	audioStreamSelectionPreferences = 0;

	resumeNavtimerActive = FALSE;
	resumeNavtimerTime = 0;

	//
	//  Create stream server
	//
	streamServer = new DVDVideoStreamServer(this, server, units, pEventDispatcher);
	}
#pragma warning(default : 4355)

//
//  Destructor
//

DVDPGCSequencer::~DVDPGCSequencer(void)
	{
	delete[] shuffleHistory;
	delete[] playbackShuffleHistory;
	delete[] cellTimes;

	if (navpu)
		{
		delete navpu;
		navpu = NULL;
		}

	if (streamServer)
		{
		delete streamServer;
		streamServer = NULL;
		}
	}

//
//  Init
//

Error DVDPGCSequencer::Init(DVDFileSystem * dvdfs)
	{
	if (!streamServer)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	GNREASSERT(streamServer->Init(dvdfs));
	DVDPGCSequencer::player = (DVDPlayer*)streamServer->GetStreamPlayer();

	//
	//  Create navigation processor
	//

	navpu = new DVDNavigationProcessor();
	if (!navpu)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	navpu->SetNavigationControl(this);

	return DVDHighlightControl::Init(player, navpu);
	}

//
//  Playback terminated
//

Error DVDPGCSequencer::PlaybackTerminated(Error err)
	{
	Exit(err);

	GNRAISE_OK;
	}

//
//  VOBU Started
//

Error DVDPGCSequencer::VOBUStarted(DVDVOBU * vobu)
	{
	DWORD duration;

	playingVOBU = vobu->firstBlock;
	if (vobu->final)
		nextVOBU = playingVOBU;
	else
		nextVOBU = vobu->NextVOBU();

	if (GetState() != pgcs_aborting)
		{
		cellTime = vobu->GetTimeOffset();

		vobuUOPs = vobu->GetPCIDWord(0x08) & 0x01fdfff8;

		if (!vobu->singleFrame)
			GNREASSERT(StartHighlight(vobu));

		duration = vobu->GetPCIDWord(16) - vobu->GetPCIDWord(12);

		if (duration)
			{
//			DP("ILVU Pos %8d BitRate %8d ILVU Size %8d", vobu->GetDSIDWord(4), ScaleDWord(vobu->numBlocks * DVD_BLOCK_SIZE * 8, duration, 90000), vobu->GetDSIDWord(32+2));
			CurrentBitrateSet(ScaleDWord(vobu->numBlocks * DVD_BLOCK_SIZE * 8, duration, 90000));
			}
		else
			CurrentBitrateSet(0);
		}

	GNRAISE_OK;
	}

//
//  VOBU Completed
//

Error DVDPGCSequencer::VOBUCompleted(DVDVOBU * vobu)
	{
	BOOL				bpReached;
	BOOL				pauseBreakpoint = FALSE;
	Error				err = GNR_OK;

	if (GetState() != pgcs_aborting)
		{
		//
		//  Test breakpoint location
		//

		bpReached = BP_VALID(vobu->breakpoint);
		if (bpReached)
			{
			DP("Breakpoint reached");
			breakpoint = vobu->breakpoint;
			if (BP_TOSTILL(breakpoint.flags))
				{
				breakpointStillState = pgcs_vobuStill;
				pauseBreakpoint = TRUE;

				//
				//  Compute proper VOBU for restarting with StillOff() after reaching breakpoint
				//  While scanning forward we stop a VOBU to late which prevents to much jumping at still off (For AB repeat)
				//

				if (nextVOBU == playingVOBU)
					{
					if (IsPlayingForward())
						{
						//
						//  On last VOBU of cell we go to next cell or next pgc
						//

						if (playingCell < finalCell)
							{
							restartVOBU = 0;
							restartCell = vobu->cell->num + 1;
							}
						else
							breakpointStillState = pgcs_pgcStill;
						}
					else
						{
						restartVOBU = playingVOBU;
						restartCell = playingCell;
						}
					}
				else
					{
					if (GetState() == pgcs_scanForward)
						restartVOBU = playingVOBU;
					else
						restartVOBU = nextVOBU;
					restartCell = vobu->cell->num;
					}

				cellTime =  vobu->GetTimeOffset() + DVDTime(vobu->GetPCIDWord(16) - vobu->GetPCIDWord(12), 90, breakpoint.time.FrameRate());
				}
			}

		//
		//  Complete VOBU
		//

		if (vobu->singleFrame)
			{
			err = streamServer->ReturnVOBU(vobu);
			}
		else
   		{
			if (vobu->numBlocks == 1)
				vobuCellCompletionDelay++;

			if (!vobuCellCompletionDelay)
				GNREASSERT(CompleteHighlight(vobu));

			if (!pauseBreakpoint)
				{
				if (vobu->still)
					{
					SetState(pgcs_vobuStill);

					err = StartStillPhase(255);
					}
				}

			err = streamServer->ReturnVOBU(vobu);
			}

		if (bpReached)
			ExecuteBreakpoint(breakpoint.id);
		}

	GNRAISE(err);
	}

//
//  Generate random number
//

static WORD Rnd(WORD max)
	{
	static DWORD seed = Timer.GetMilliTicks();

	seed = 1664525L * seed + 1013904223L;
	return (WORD)(seed % max) + 1;
	}

//
//  Test if cell is still cell
//

BOOL DVDPGCSequencer::IsStillCell(void)
	{
	if (GetState() == pgcs_scanBackward || GetState() == pgcs_cellPlaybackReverse)
		{
		if (currentCell < finalCell)
			return (currentPG == 1 && (playbackMode == 0 || loopCnt == 1));
		}
	else
		{
		if (currentCell > finalCell)
			return (currentPG == numPG && (playbackMode == 0 || loopCnt == 1));
		}

	return FALSE;
	}

//
//	Test if cell is final cell of program
//

Error DVDPGCSequencer::IsFinalCell(WORD program, WORD cell, BOOL & isFinalCell)
	{
	WORD finalCell;
	DVDCPBI cpbi;
	WORD firstAngleCell;
	WORD lastAngleCell;
	WORD angleCell;

	GNREASSERT(pgci->GetCellInformation(cell, cpbi));

	//
	//	Search angle block for first and last Cell
	//

	if (cpbi.BlockType() == CBT_ANGLE_BLOCK)
		{
		angleCell = cell;
		while (cpbi.BlockMode() != CBM_FIRST_CELL_IN_BLOCK)
			{
			angleCell--;
			GNREASSERT(pgci->GetCellInformation(angleCell, cpbi));
			}
		firstAngleCell = angleCell;
		angleCell = cell;
		while (cpbi.BlockMode() != CBM_LAST_CELL_IN_BLOCK)
			{
			angleCell++;
			GNREASSERT(pgci->GetCellInformation(angleCell, cpbi));
			}
		lastAngleCell = angleCell;

		if (cell >= firstAngleCell && cell <= lastAngleCell)
			isFinalCell = TRUE;
		else
			isFinalCell = FALSE;
		}
	else
		{
		GNREASSERT(pgci->GetLastCellOfProgram(program, finalCell));

		isFinalCell = (cell == finalCell);
		}

	GNRAISE_OK;
	}

//
//  Go to next cell in playback direction
//

Error DVDPGCSequencer::AdvanceCell(void)
	{
	DVDNavigationCommand	* cmds;
	DVDCPBI cpbi;
	int cmd;

	GNREASSERT(pgci->GetCellInformation(currentCell, cpbi));

	//
	//  Search angle block for first/last Cell
	//

	if (cpbi.BlockType() == CBT_ANGLE_BLOCK)
		{
		if (GetState() == pgcs_scanBackward || GetState() == pgcs_cellPlaybackReverse)
			{
			while (cpbi.BlockMode() != CBM_FIRST_CELL_IN_BLOCK)
				{
				currentCell--;
				GNREASSERT(pgci->GetCellInformation(currentCell, cpbi));
				}
			}
		else
			{
			while (cpbi.BlockMode() != CBM_LAST_CELL_IN_BLOCK)
				{
				currentCell++;
				GNREASSERT(pgci->GetCellInformation(currentCell, cpbi));
				}
			}
		}

	//
	//  Now go back/foward one cell
	//

	if (GetState() == pgcs_scanBackward || GetState() == pgcs_cellPlaybackReverse)
		currentCell--;
	else
		{
		cmd = cpbi.CommandNumber();
		if (cmd)
			{
			pgci->GetCellCommands(cmds);
			if (cmds)
				{
				if (navpu->IsBasicCellLinkCommand(cmds[cmd-1]))
					{
					navpu->GetBasicCellLinkTarget(cmds[cmd-1], currentCell);
					}
				else
					currentCell++;
				}
			}
		else
			currentCell++;
		}

	GNRAISE_OK;
	}

//
//  Compute successor cell
//

Error DVDPGCSequencer::SuccCell(DVDGenericPGCI * pgci, WORD angle, WORD & cell, DVDCPBI & cpbi)
	{
	DVDNavigationCommand	*	cmds;
	int cmd;

	//
	//  Go to last cell of angle block
	//

	GNREASSERT(pgci->GetCellInformation(cell, cpbi));
	if (cpbi.BlockType() == CBT_ANGLE_BLOCK)
		{
		while (cpbi.BlockMode() != CBM_LAST_CELL_IN_BLOCK)
			{
			cell++;
			GNREASSERT(pgci->GetCellInformation(cell, cpbi));
			}
		}

	//
	//  Now go foward one cell, with respect to cell commands
	//

	cmd = cpbi.CommandNumber();
	if (cmd)
		{
		pgci->GetCellCommands(cmds);
		if (cmds)
			{
			if (navpu->IsBasicCellLinkCommand(cmds[cmd-1]))
				{
				navpu->GetBasicCellLinkTarget(cmds[cmd-1], cell);
				}
			else
				cell++;
			}
		}
	else
		cell++;

	//
	//  Now find proper cell within angle block
	//

	GNREASSERT(pgci->GetCellInformation(cell, cpbi));

	if (cpbi.BlockType() == CBT_ANGLE_BLOCK)
		{
		angle--;
		while (angle && cpbi.BlockMode() != CBM_LAST_CELL_IN_BLOCK)
			{
			angle--;
			cell++;
			GNREASSERT(pgci->GetCellInformation(cell, cpbi));
			}
		}

	GNRAISE_OK;
	}

//
//  Compute predecessor cell
//

Error DVDPGCSequencer::PredCell(DVDGenericPGCI * pgci, WORD angle, WORD & cell, DVDCPBI & cpbi)
	{
	GNREASSERT(pgci->GetCellInformation(cell, cpbi));
	if (cpbi.BlockType() == CBT_ANGLE_BLOCK)
		{
		while (cpbi.BlockMode() != CBM_FIRST_CELL_IN_BLOCK)
			{
			cell--;
			GNREASSERT(pgci->GetCellInformation(cell, cpbi));
			}
		}
	cell--;

	//
	//  Find proper cell within angle block
	//

	GNREASSERT(pgci->GetCellInformation(cell, cpbi));

	if (cpbi.BlockType() == CBT_ANGLE_BLOCK)
		{
		//
		//  Go to first cell
		//

		while (cpbi.BlockMode() != CBM_FIRST_CELL_IN_BLOCK)
			{
			cell--;
			GNREASSERT(pgci->GetCellInformation(cell, cpbi));
			}

		//
		//  From there search for proper angle
		//

		angle--;
		while (angle && cpbi.BlockMode() != CBM_LAST_CELL_IN_BLOCK)
			{
			angle--;
			cell++;
			GNREASSERT(pgci->GetCellInformation(cell, cpbi));
			}
		}

	GNRAISE_OK;
	}

//
//  Start Cell Playback
//

Error DVDPGCSequencer::StartCell(WORD cell, DWORD vobu)
	{
	BOOL still = FALSE;
	WORD num;
	WORD next;
	DVDCPBI cpbi;
	WORD program;
	WORD finalCell;
	BOOL isFinalCell = FALSE;

	//
	//  Find proper cell for angle
	//

	DP("Starting Cell %d VOBU %d, Angle %d", cell, vobu, angle);
	GNREASSERT(pgci->GetCellInformation(cell, cpbi));

	if (cpbi.BlockType() == CBT_ANGLE_BLOCK)
		{
		while (cpbi.BlockMode() != CBM_FIRST_CELL_IN_BLOCK)
			{
			cell--;
			GNREASSERT(pgci->GetCellInformation(cell, cpbi));
			}
		num = 1;
		while (cpbi.BlockMode() != CBM_LAST_CELL_IN_BLOCK && num < angle)
			{
			num++;
			cell++;
			GNREASSERT(pgci->GetCellInformation(cell, cpbi));
			}
		}

	currentCell = cell;
	playingAngle = angle;

	if (!streaming)
		{
		DP("Initial playing cell %d to %d", playingCell, cell);
		playingCell = cell;
		streaming = TRUE;
		if (!vobu) cellTime = 0;
		}

	GNREASSERT(AdvanceCell());

	//
	//  Now start cell
	//

	still = IsStillCell();
	playbackDone = FALSE;

	next = cell;

	GNREASSERT(pgci->GetProgramOfCell(cell, program));

	switch (GetState())
		{
		case pgcs_scanForward:
			SuccCell(pgci, angle, next, cpbi);
			still |= cpbi.AccessRestricted();
			GNREASSERT(IsFinalCell(program, cell, isFinalCell));
			GNREASSERT(streamServer->TransferCellScan(pgci, cell, still, isFinalCell, cellTimes[cell - 1], TRUE, 0, scanSpeed, vobu));
			break;

		case pgcs_scanBackward:
			PredCell(pgci, angle, next, cpbi);
			still |= cpbi.AccessRestricted();
			GNREASSERT(pgci->GetFirstCellOfProgram(program, finalCell));
			GNREASSERT(streamServer->TransferCellScan(pgci, cell, still, cell == finalCell, cellTimes[cell - 1], FALSE, 0, scanSpeed, vobu));
			break;

		case pgcs_cellPlaybackReverse:
			PredCell(pgci, angle, next, cpbi);
			still |= cpbi.AccessRestricted();
			GNREASSERT(pgci->GetFirstCellOfProgram(program, finalCell));
			GNREASSERT(streamServer->TransferCellReverse(pgci, cell, still, cell == finalCell, cellTimes[cell - 1], vobu));
			break;

		case pgcs_cellTrickPlay:
			break;	// Intended do-nothing

		default:
			GNREASSERT(IsFinalCell(program, cell, isFinalCell));
			GNREASSERT(streamServer->TransferCell(pgci, cell, still, isFinalCell, cellTimes[cell - 1], vobu, angle));
			break;
		}

	//
	//  Send Title Change Event
	//

	GNRAISE(CheckTitleChange());
	}

//
//  Start cell playback at certain time (always pgcs_cellPlayback)
//  NOTE: Here we do not care about "CellAccessRestricted" flag since this
//  has to be checked before calling this function
//

Error DVDPGCSequencer::StartCellAt(WORD cell, DVDTime time)
	{
	BOOL still = FALSE;
	WORD num;
	DVDCPBI cpbi;
	WORD program;
	WORD lastCell;

//	DP("Starting Cell %d with angle %d", cell, angle);
	GNREASSERT(pgci->GetCellInformation(cell, cpbi));

	if (cpbi.BlockType() == CBT_ANGLE_BLOCK)
		{
		//
		//  Find proper angle
		//

		while (cpbi.BlockMode() != CBM_FIRST_CELL_IN_BLOCK)
			{
			cell--;
			GNREASSERT(pgci->GetCellInformation(cell, cpbi));
			}
		num = 1;
		while (cpbi.BlockMode() != CBM_LAST_CELL_IN_BLOCK && num < angle)
			{
			num++;
			cell++;
			GNREASSERT(pgci->GetCellInformation(cell, cpbi));
			}
		currentCell = cell;
		}

	if (!streaming)
		{
		DP("Initial playing cell at %d to %d", playingCell, cell);
		playingCell = cell;
		streaming = TRUE;
		}
	playingAngle = angle;

	GNREASSERT(AdvanceCell());

	still = IsStillCell();

	GNREASSERT(pgci->GetProgramOfCell(cell, program));
	GNREASSERT(pgci->GetLastCellOfProgram(program, lastCell));

	GNREASSERT(streamServer->TransferCellAt(pgci, cell, still, cell == lastCell, cellTimes[cell - 1], time));

	GNRAISE(CheckTitleChange());
	}

//
//  Start Program Playback
//
//  GNR_OK
//

Error DVDPGCSequencer::StartProgram(void)
	{
	WORD firstCell;
	WORD numCells;
	playbackDone = FALSE;
	programStarted = TRUE;

	GNREASSERT(pgci->GetFirstCellOfProgram(currentPG, firstCell));
	GNREASSERT(pgci->GetNumberOfCellsInProgram(currentPG, numCells));

	if (GetState() == pgcs_scanBackward || GetState() == pgcs_cellPlaybackReverse)
		{
		currentCell = firstCell + numCells - 1;		// Start with the last one
		finalCell = firstCell;
		}
	else
		{
		currentCell = firstCell;
		finalCell = currentCell + numCells - 1;
		}

	playingPG = currentPG;
	StartCell(currentCell);

	GNRAISE_OK;
	}

//
//  Send next cell
//

Error DVDPGCSequencer::SendNextCell(void)
	{
	if (!playbackDone)
		{
		if (GetState() != pgcs_cellTrickPlay)
			{
			if (GetState() == pgcs_scanBackward || GetState() == pgcs_cellPlaybackReverse)
				{
				if (currentCell >= finalCell)
					GNRAISE(StartCell(currentCell));
				else
					GNRAISE(SendNextProgram());
				}
			else
				{
				if (currentCell <= finalCell)
					GNRAISE(StartCell(currentCell));
				else
					GNRAISE(SendNextProgram());
				}
			}
		else
			GNRAISE_OK;
		}
	else
		GNRAISE_OK;
	}

//
//  Has next program
//

Error DVDPGCSequencer::HasNextProgram(BOOL & hasNextProg)
	{
	WORD prog;

	if (!playbackDone)
		{
		if (playbackMode == 0)
			{
			GNREASSERT(pgci->GetProgramOfCell(playingCell, prog));

			if (GetState() == pgcs_scanBackward || GetState() == pgcs_cellPlaybackReverse)
				{
				hasNextProg = prog > 1;
				}
			else
				{
				hasNextProg = prog < numPG;
				}
			}
		else if (playbackMode & 0x80)
			{
			hasNextProg = loopCnt > 1;
			}
		else
			{
			hasNextProg = loopCnt > 1;
			}
		}
	else
		hasNextProg = FALSE;

	GNRAISE_OK;
	}

//
//  Send next program
//

Error DVDPGCSequencer::SendNextProgram(void)
	{
	int i;

	if (!playbackDone)
		{
		if (playbackMode == 0)  // Normal playback
			{
			if (IsPlayingForward())
				{
				currentPG++;
				if (currentPG <= numPG)
					StartProgram();
				else
					playbackDone = TRUE;
				}
			else
				{
				currentPG--;
				if (currentPG >= 1)
					StartProgram();
				else
					playbackDone = TRUE;
				}
			}
		else if (playbackMode & 0x80)  // Shuffle mode
			{
			loopCnt--;
			if (loopCnt)
				{
				BOOL allDone = TRUE;

				for(i=0; i<numPG; i++)
					allDone &= shuffleHistory[i];

				if (allDone)
					{
					for(i=0; i<numPG; i++)
						shuffleHistory[i] = FALSE;
					}

				do {
					currentPG = Rnd(numPG);
					} while (shuffleHistory[currentPG-1]);
				shuffleHistory[currentPG-1] = TRUE;

				StartProgram();
				}
			else
				playbackDone = TRUE;
			}
		else  // Random playback
			{
			loopCnt--;
			if (loopCnt)
				{
				currentPG = Rnd(numPG);
				StartProgram();
				}
			else
				playbackDone = TRUE;
			}
		}

	GNRAISE_OK;
	}

//
//  Cell started
//

Error DVDPGCSequencer::CellStarted(DVDCell * cell)
	{
	int i;
	BOOL first = programStarted;

//	DP("Cell Started: Curr %d Play %d Start %d", currentCell, playingCell, cell->num);
	vobuCellCompletionDelay = 0;
	playingCell = cell->num;
	playingLoopCnt = loopCnt;
	if (shuffleHistory && playbackShuffleHistory)
		{
		for (i=0; i<numPG; i++)
			playbackShuffleHistory[i] = shuffleHistory[i];
		}
	isMultiAngleScene = cell->IsMultiAngleCell();
	programStarted = FALSE;

	if (GetState() != pgcs_aborting)
		{
		//
		//  Check if we reached breakpoint
		//

		if (first && BP_VALID(cell->breakpoint))
			{
			if (BP_TOSTILL(cell->breakpoint.flags))
				{
				breakpointStillState = pgcs_vobuStill;
				stillCell = cell;
				restartCell = cell->num;
				restartVOBU = 0;
				}

			ExecuteBreakpoint(cell->breakpoint.id);
			GNRAISE_OK;
			}

		GNREASSERT(SendNextCell());
		}

	GNRAISE_OK;
	}

//
//  Cell completed
//

Error DVDPGCSequencer::CellCorrupt(DVDCell * cell)
	{
	DP("Advancing To Next Cell+");
	if (GoNextCell() == GNR_RANGE_VIOLATION)
		GoNextProgram();

	DP("Advancing To Next Cell-");

	GNRAISE_OK;
	}

Error DVDPGCSequencer::CellCompleted(DVDCell * cell)
	{
	WORD next = cell->num;
	DVDCPBI cpbi;

//	DP("Cell Completed: %d", cell->num);
	if (GetState() != pgcs_aborting)
		{
		//
		//  Test if we reached breakpoint
		//

		if (BP_VALID(cell->breakpoint))
			{
			if (BP_TOSTILL(cell->breakpoint.flags))
				breakpointStillState = pgcs_cellStill;

			ExecuteBreakpoint(cell->breakpoint.id);
			GNRAISE_OK;
			}

		//
		//  Test Access Restricted flag of next cell during normal playback
		//

		SuccCell(cell->pgci, angle, next, cpbi);
		if (GetState() == pgcs_cellPlayback)
			{
			if (cpbi.AccessRestricted())
				{
				if (player->IsPaused())		// This handles the case when we do single step over cell boundary
					Resume();
				else
					{
					if (player->GetPlaybackSpeed() != 1000)
						player->SetPlaybackSpeed(1000);
					}
				}
			}

		//
		//  Perform still actions (if any)
		//

		if (cell->still)
			{
			//
			//  Test if we reached beginning of PGC or Access Restricted cell during reverse playback
			//

			if (GetState() == pgcs_scanBackward || GetState() == pgcs_cellPlaybackReverse)
				{
				PredCell(cell->pgci, angle, next, cpbi);
				if (cpbi.AccessRestricted())
					{
					DP("Restricted cell access %d to %d", playingCell, cell->num);
					playingCell = cell->num;
					}

				if (GetState() == pgcs_cellPlaybackReverse)
					player->SetPlaybackSpeed(1000);

				StopScan(FALSE);
				}
			else
				{
				WORD timeOut = cell->info.StillTime();

				//
				//  Test Cell Access Restricted flag during forward scan
				//

				if (GetState() == pgcs_scanForward)
					{
					if (cpbi.AccessRestricted())
						{
						StopScan(FALSE);
						GNRAISE_OK;
						}
					}

				//
				//  Perform still actions
				//

				if (cell->info.ContinuousPresentation()) timeOut = 255;

	//				DP("timeOut %d", timeOut);

				if (timeOut < 255)
					{
					timeOut += vobuCellCompletionDelay / 2;
					}

				stillCell = cell;

				SetState(pgcs_cellStill);

				GNREASSERT(StartStillPhase(timeOut));
				}
			}
		else
			{
			GNREASSERT(streamServer->ReturnCell(cell));
			GNREASSERT(CompleteHighlight());
//			GNREASSERT(SendNextCell()); //URG!!
			}
		}

	GNRAISE_OK;
	}


//
//  Start Still Phase
//

Error DVDPGCSequencer::StartStillPhase(WORD delay)
	{
	if (delay > 0)
		{
		SetScanSpeed(1);
		player->SetPlaybackSpeed(1000);
		}
	GNRAISE_OK;
	}

//
//  Still phase completed
//

Error DVDPGCSequencer::StillPhaseCompleted(void)
	{
	int cmd;
	DVDNavigationCommand	*	cmds;
	DVDNavigationProcessor::NavCommandDone dummyDone;
	WORD dummyPML;

	//
	//  Handle breakpoint stills
	//

	if (breakpointStillState == pgcs_vobuStill)
		{
		CancelStillPhase();
		streamServer->AbortTransferAccurate();

//		DP("Still phase completed %d to %d", playingCell, restartCell);
		currentCell = playingCell = restartCell;

		GNREASSERT(pgci->GetProgramOfCell(playingCell, currentPG));
		GNREASSERT(pgci->GetLastCellOfProgram(currentPG, finalCell));

		state = pgcs_cellPlayback;					// Fake for AdvanceCell, but we don't want event here
		AdvanceCell();
		state = pgcs_vobuStill;						// No need for SetState() here, we don't want a change event twice
		externalState = pgcs_vobuStill;

		StartCell(restartCell, restartVOBU);

		player->Resume();
		}
	else if (breakpointStillState == pgcs_pgcStill)
		{
		state = pgcs_pgcStill;
		externalState = pgcs_pgcStill;
		breakpointStillState = pgcs_idle;
		player->Resume();
		}

	//
	//  Handle all other stills
	//

	switch (GetState())
		{
		case pgcs_vobuStill:
//			DP("StillPhaseCompleted: vobu still");
			GNREASSERT(streamServer->CompleteStill());
			if (breakpointStillState != pgcs_idle)
				{
				state = pgcs_cellPlayback;
				externalState = pgcs_cellPlayback;
				breakpointStillState = pgcs_idle;
				}
			break;
		case pgcs_cellStill:
//			DP("StillPhaseCompleted: cell still");
			cmd = stillCell->info.CommandNumber();
			GNREASSERT(streamServer->ReturnCell(stillCell));
			stillCell = NULL;

			//
			//  Perform forced activation of button
			//

			SetState(pgcs_cellCommand);
			GNREASSERT(CompleteHighlight());
			if (GetState() != pgcs_cellCommand)
         	GNRAISE_OK;

			//
			//  Execute cell commands
			//

			if (cmd)
				{
				pgci->GetCellCommands(cmds);
				if (cmds)
					{
					if (!(navpu->IsBasicCellLinkCommand(cmds[cmd-1])))
						{
						//  Here we don't need to check for SetTMPPML command since it is not allowed here
						navpu->InterpretCommand(cmds[cmd-1], cmd, dummyDone, dummyPML);
						if (GetState() != pgcs_cellCommand)
							GNRAISE_OK;
						}
					}
				}

			SetState(pgcs_cellPlayback);
			if (player->IsPaused())		// This handles the case when we do single step over cell boundary
				externalState = pgcs_paused;

//				SendNextCell();
			if (!playbackDone)
				streamServer->CompleteStill();
			else
				{
				BYTE timeOut;

				GNREASSERT(pgci->GetStillTimeValue(timeOut));

				if (EndOfPGCBreakpointReached(EPD_FORWARD, breakpoint))
					{
					if (BP_TOSTILL(breakpoint.flags))
						breakpointStillState = pgcs_pgcStill;
					ExecuteBreakpoint(breakpoint.id);
					}
				else
					breakpoint.flags = 0;

				if (!BP_TOSTILL(breakpoint.flags))
					{
					SetState(pgcs_pgcStill);
					GNREASSERT(StartStillPhase(timeOut));
					}
				}
			break;
		case pgcs_pgcStill:
//			DP("StillPhaseCompleted: pgc still");
			GNREASSERT(streamServer->CompleteStill());
			GNREASSERT(streamServer->AbortTransfer());
			GNREASSERT(CompletePGCPlayback());
			break;
		}

	GNRAISE_OK;
	}


//
//  Return current playing program
//

Error DVDPGCSequencer::GetCurrentPlayingProgram(WORD & pg)
	{
	switch (GetState())
		{
		case pgcs_idle:
			pg = 1;
			break;
		case pgcs_preCommand:
			pg = currentPG;
			break;
		default:
			return pgci->GetProgramOfCell(playingCell, pg);
		}
	GNRAISE_OK;
	}

//
//  Complete PGC playback
//
//  GNR_OK
//  GNR_PARENTAL_LEVEL_TOO_LOW
//  GNR_UNEXPECTED_NAVIGATION_ERROR
//  GNR_ILLEGAL_NAVIGATION_COMMAND
//

Error DVDPGCSequencer::CompletePGCPlayback(void)
	{
	int numCmd, pc;
	DVDNavigationCommand * cmd;
	DVDNavigationProcessor::NavCommandDone done;

	//
	//  Only execute first part if we didn't wait for PML change
	//

	if (state != pgcs_awaitingPMLChange)
		{
		GNREASSERT(CancelHighlight());

		SetState(pgcs_postCommand);

		pc = 1;
		GNREASSERT(pgci->GetPostCommands(numCmd, cmd));	// Always returns GNR_OK
		}
	else
		{
		//
		// Check if PML has been set (this point is never reached if PML handling is PMH_COMPATIBLE)
		//

		if (pmlHandling == PMH_DEFER_CHANGE)
			GNRAISE(GNR_PARENTAL_LEVEL_TOO_LOW);

		SetState(pgcs_postCommand);
		pc = awaitPMLChangePC;
		GNREASSERT(pgci->GetPostCommands(numCmd, cmd));	// numCmd is at least 1 (because of SetTMPPML), always returns GNR_OK
		}

	//
	//  Execute post commands and send "playback completed"
	//

	if (numCmd)
		{
		//
		// Execute post commands and wait in case of SetTMPPML request
		//

		GNREASSERT(navpu->InterpretCommandList(cmd, pc, numCmd, done, requestedPML));
		if (done == DVDNavigationProcessor::NCD_SETTMPPML_DEFERED)
			{
			SetAwaitPMLChangeState(PDL_COMPLETEPGC, pc);
			GNRAISE_OK;
			}
		}

	if (GetState() == pgcs_postCommand)
		{
		SetState(pgcs_idle);
		PGCCompleted(pgci);
		}

	GNRAISE_OK;
	}

//
//  Do activate button
//

Error DVDPGCSequencer::DoActivateButton(void)
	{
	GNREASSERT(DVDHighlightControl::DoActivateButton());
	if (buttonActivated && GetState() == pgcs_cellStill)
		{
		GNREASSERT(CancelStillPhase());
		GNREASSERT(StillPhaseCompleted());
		SendEvent(DNE_STILL_OFF, 0);
		}

	GNRAISE_OK;
	}

Error DVDPGCSequencer::SetAudioStreamSelectionPreferences(DWORD flags)
	{
	audioStreamSelectionPreferences = flags;

	GNRAISE_OK;
	}

//
//  Select audio stream
//
Error DVDPGCSequencer::SelectAudioStream(WORD & strm)
	{
	WORD num;						// Number of audio streams
	WORD attr;
	WORD ctrl;						// Audio stream control information
	WORD language, extension;  // Language and extension
	WORD appInfo;					// Application Information
	WORD reqlang, reqextend;	// Requested language and extension
	WORD potentialStreams, i;
	WORD tattr, tlanguage, textension, tappInfo;

	//
	//  Get current audio stream and check if it is available
	//  (Note: This selection algo is described in DVD Spec Chapter 3.3.9)
	//

	GNREASSERT(GetSPRM(1, strm));
	GNREASSERT(pgci->GetAudioStreamControl(strm, ctrl));

	if (!XTBF(15, ctrl))
		{
		//
		//  Stream is not available
		//

		potentialStreams = 0;

		GNREASSERT(vobs->GetNumberOfAudioStreams(num));
		GNREASSERT(GetSPRM(16, reqlang));
		GNREASSERT(GetSPRM(17, reqextend));

		//
		//  Find all stream which match exactly the requested one
		//

		for (strm=0; strm<num; strm++)
			{
			GNREASSERT(vobs->GetAudioStreamAttribute(strm, attr, language, extension, appInfo));
			if (language == reqlang && extension == reqextend)
				{
				GNREASSERT(pgci->GetAudioStreamControl(strm, ctrl));
				if (XTBF(15, ctrl))
					potentialStreams |= MKFLAG(strm);
				}
			}

		if (!potentialStreams)
			{
			//
			//  Find a stream which has the requested language
			//

			for (strm=0; strm<num; strm++)
				{
				GNREASSERT(vobs->GetAudioStreamAttribute(strm, attr, language, extension, appInfo));
				if (language == reqlang)
					{
					GNREASSERT(pgci->GetAudioStreamControl(strm, ctrl));
					if (XTBF(15, ctrl))
						potentialStreams |= MKFLAG(strm);
					}
				}
			}

		if (!potentialStreams)
			{
			//
			//  Find a stream which has the requested language extension
			//

			for (strm=0; strm<num; strm++)
				{
				GNREASSERT(vobs->GetAudioStreamAttribute(strm, attr, language, extension, appInfo));
				if (extension == reqextend)
					{
					GNREASSERT(pgci->GetAudioStreamControl(strm, ctrl));
					if (XTBF(15, ctrl))
						potentialStreams |= MKFLAG(strm);
					}
				}
			}

		if (!potentialStreams)
			{
			//
			//  Find a stream which is just available
			//

			for (strm=0; strm<num; strm++)
				{
				GNREASSERT(pgci->GetAudioStreamControl(strm, ctrl));
				if (XTBF(15, ctrl))
					potentialStreams |= MKFLAG(strm);
				}
			}

		if (potentialStreams)
			{
			strm = 0;
			while (!XTBF(strm, potentialStreams)) strm++;

			if (audioStreamSelectionPreferences)
				{
				GNREASSERT(vobs->GetAudioStreamAttribute(strm, attr, language, extension, appInfo));

				i = strm + 1;
				while (i < 8)
					{
					GNREASSERT(vobs->GetAudioStreamAttribute(i, tattr, tlanguage, textension, tappInfo));

					if ((DDPASSP_DTS & audioStreamSelectionPreferences) && XTBF(13, 3, attr) != 6 && XTBF(13, 3, tattr) == 6)
						{
						strm = i;
						GNREASSERT(vobs->GetAudioStreamAttribute(strm, attr, language, extension, appInfo));
						}
					else if ((DDPASSP_DTS & audioStreamSelectionPreferences) && XTBF(13, 3, attr) == 6 && XTBF(13, 3, tattr) != 6)
						{
						}
					else if ((DDPASSP_NO_DTS & audioStreamSelectionPreferences) && XTBF(13, 3, attr) == 6 && XTBF(13, 3, tattr) != 6)
						{
						strm = i;
						GNREASSERT(vobs->GetAudioStreamAttribute(strm, attr, language, extension, appInfo));
						}
					else if ((DDPASSP_NO_DTS & audioStreamSelectionPreferences) && XTBF(13, 3, attr) != 6 && XTBF(13, 3, tattr) == 6)
						{
						}
					else if (DDPASSP_MAXCHANNELS & audioStreamSelectionPreferences && XTBF(0, 3, attr) < XTBF(0, 3, tattr))
						{
						strm = i;
						GNREASSERT(vobs->GetAudioStreamAttribute(strm, attr, language, extension, appInfo));
						}
					}
				}

			return SetSPRM(1, strm);
			}
		else
			{
			//
			//  No proper stream found
			//

			strm = 15;

			return SetSPRM(1, strm);
			}
		}
	else
		GNRAISE_OK;  // Stream was available
	}

//
//  Select SPU stream
//

Error DVDPGCSequencer::SelectSPUStream(WORD & strm, BOOL & enable)
	{
	WORD num, full;
	WORD attr, language, reqlang, extension, reqextend;
	DWORD ctrl;

	GNREASSERT(GetSPRM(2, full));
	strm = full & 0x3f;
	full &= 0x40;
	enable = full != 0;

	if (strm == 63)
		GNRAISE_OK;

	GNREASSERT(pgci->GetSubPictureStreamControl(strm, ctrl));

	if (!XTBF(31, ctrl))
		{
		GNREASSERT(vobs->GetNumberOfSubPictureStreams(num));
		GNREASSERT(GetSPRM(18, reqlang));
		GNREASSERT(GetSPRM(19, reqextend));

		for (strm=0; strm<num; strm++)
			{
			GNREASSERT(vobs->GetSubPictureStreamAttribute(strm, attr, language, extension));
			if (language == reqlang && extension == reqextend)
				{
				GNREASSERT(pgci->GetSubPictureStreamControl(strm, ctrl));
				if (XTBF(31, ctrl))
					return SetSPRM(2, strm | full);
				}
			}

		for (strm=0; strm<num; strm++)
			{
			GNREASSERT(vobs->GetSubPictureStreamAttribute(strm, attr, language, extension));
			if (language == reqlang)
				{
				GNREASSERT(pgci->GetSubPictureStreamControl(strm, ctrl));
				if (XTBF(31, ctrl))
					return SetSPRM(2, strm | full);
				}
			}

		for (strm=0; strm<num; strm++)
			{
			GNREASSERT(vobs->GetSubPictureStreamAttribute(strm, attr, language, extension));
			if (extension == reqextend)
				{
				GNREASSERT(pgci->GetSubPictureStreamControl(strm, ctrl));
				if (XTBF(31, ctrl))
					return SetSPRM(2, strm | full);
				}
			}

		for (strm=0; strm<num; strm++)
			{
			GNREASSERT(pgci->GetSubPictureStreamControl(strm, ctrl));
			if (XTBF(31, ctrl))
				return SetSPRM(2, strm | full);
			}

		strm = 62;

		return SetSPRM(2, strm | full);
		}
	else
		GNRAISE_OK;
	}

//
//  Select display presentation mode
//

Error DVDPGCSequencer::SelectDisplayPresentationMode(DisplayPresentationMode & mode)
	{
	WORD attrib;

	GNREASSERT(vobs->GetVideoAttributes(attrib));

	if (XTBF(10, 2, attrib) == 0)
		{
		// 4 by 3
		mode = DPM_4BY3;
		}
	else
		{
		if (requestedPresentationMode == DPM_16BY9 || XTBF(8, 2, attrib) == 3)
			{
			// 16 by 9 presentation
			mode = DPM_16BY9;
			}
		else if (XTBF(8, 2, attrib) == 1)
			{
			mode = DPM_PANSCAN;
			}
		else if (XTBF(8, 2, attrib) == 2)
			{
			mode = DPM_LETTERBOXED;
			}
		else
			{
			if (requestedPresentationMode == DPM_PANSCAN)
				{
				mode = DPM_PANSCAN;
				}
			else
				{
				mode = DPM_LETTERBOXED;
				}
			}
		}

	GNRAISE_OK;
	}

//
//  Initialize array of cell times
//
//  GNR_OK
//

Error DVDPGCSequencer::BuildCellTimes(DVDGenericPGCI * pgci, DVDTime * & times, WORD cellNum)
	{
	int i;
	DVDTime	timeTotal;
	BOOL initial = TRUE;

	//
	//  Set up time array
	//

	if (times)
		delete[] times;
	times = new DVDTime[cellNum + 1];

	for(i=1; i<=cellNum; i++)
		{
		DVDCPBI cpbi;

		GNREASSERT(pgci->GetCellInformation(i, cpbi));

		//
		// get frame rate from first time stamp, if at least one is available,
		// otherwise just stick with default, it will not matter anyway.
		//
		if (initial)
			{
			timeTotal = DVDTime(0, 0, 0, 0, cpbi.PresentationTime().FrameRate());
			initial = FALSE;
			}

		times[i-1] = timeTotal;

		if (cpbi.BlockType() != CBT_ANGLE_BLOCK || cpbi.BlockMode() == CBM_LAST_CELL_IN_BLOCK)
			{
			timeTotal += cpbi.PresentationTime();
			}
		}
	times[cellNum] = timeTotal;

	GNRAISE_OK;
	}

//
//  Set Stream Parameters
//

Error DVDPGCSequencer::SetStreamParameters(void)
	{
	WORD audioStream, audioStreamID, audioStreamCtrl;
	WORD spuStream, spuStreamID;
   DWORD spuStreamCtrl;
	DisplayPresentationMode presentationMode;
	WORD videoAttr;
	WORD audioAttr, language, extension;
	WORD appInfo;
	WORD subPictureAttr;
	BOOL spuEnable;
	DWORD palette;
	int i;

	SelectAudioStream(audioStream);
	SelectSPUStream(spuStream, spuEnable);
	SelectDisplayPresentationMode(presentationMode);

	//
	// Set up audio stream
	//

	GNREASSERT(pgci->GetAudioStreamControl(audioStream, audioStreamCtrl));
	GNREASSERT(pgci->GetSubPictureStreamControl(spuStream, spuStreamCtrl));

	audioStreamID = (WORD)XTBF(8, 3, audioStreamCtrl);
	if (!XTBF(15, audioStreamCtrl)) audioStreamID = 0x0f;

	//
	// Set up presentation mode
	//

	switch (presentationMode)
		{
		case DPM_4BY3:
			spuStreamID = (WORD)XTBF(24, 5, spuStreamCtrl);
			break;
		case DPM_16BY9:
			spuStreamID = (WORD)XTBF(16, 5, spuStreamCtrl);
			break;
		case DPM_LETTERBOXED:
			spuStreamID = (WORD)XTBF( 8, 5, spuStreamCtrl);
			break;
		case DPM_PANSCAN:
			spuStreamID = (WORD)XTBF( 0, 5, spuStreamCtrl);
			break;
		}

	//
	//  Set up sub pictures
	//

	if (spuEnable) spuStreamID |= 0x40;

	if (numPG)
		{
		GNREASSERT(vobs->GetVideoAttributes(videoAttr));
		GNREASSERT(vobs->GetAudioStreamAttribute(audioStream, audioAttr, language, extension, appInfo));
		GNREASSERT(vobs->GetSubPictureStreamAttribute(spuStream, subPictureAttr, language, extension));

		GNREASSERT(SetPresentationMode(presentationMode));
		GNREASSERT(player->SetStreamAttributes(videoAttr,
			                                    audioStreamID, audioAttr,
			                                    spuStreamID, subPictureAttr,
			                                    presentationMode));

		for(i=0; i<16; i++)
			{
			GNREASSERT(pgci->GetSubPicturePalette(i, palette));
			player->SetSubPicturePalette(i, palette);
			}
		}

	if (currentPresentationMode != presentationMode)
		SendEvent(DNE_DISPLAY_MODE_CHANGE, presentationMode);
	currentPresentationMode = presentationMode;

	GNRAISE_OK;
	}

//
//  Set audio stream parameters
//

Error DVDPGCSequencer::SetAudioStreamParameters(void)
	{
	WORD audioStream, audioStreamID, audioStreamCtrl;
	WORD audioAttr, language, extension;
	WORD appInfo;

	//
	//  Get audio stream id
	//

	SelectAudioStream(audioStream);
	GNREASSERT(pgci->GetAudioStreamControl(audioStream, audioStreamCtrl));
	audioStreamID = (WORD)XTBF(8, 3, audioStreamCtrl);
	if (!XTBF(15, audioStreamCtrl))
		audioStreamID = 0x0f;

	//
	//  Set audio attributes

	GNREASSERT(vobs->GetAudioStreamAttribute(audioStream, audioAttr, language, extension, appInfo));
	GNREASSERT(player->SetAudioStreamAttributes(audioStreamID, audioAttr));
	GNRAISE_OK;
	}

//
//  Set SPU stream parameters
//

Error DVDPGCSequencer::SetSPUStreamParameters(void)
	{
	WORD spuStream, spuStreamID;
   DWORD spuStreamCtrl;
	WORD language, extension;
	WORD subPictureAttr;
	BOOL spuEnable;

	SelectSPUStream(spuStream, spuEnable);

	GNREASSERT(pgci->GetSubPictureStreamControl(spuStream, spuStreamCtrl));

	//
	//  Set up sub pictures
	//
	switch (currentPresentationMode)
		{
		case DPM_4BY3:
			spuStreamID = (WORD)XTBF(24, 5, spuStreamCtrl);
			break;
		case DPM_16BY9:
			spuStreamID = (WORD)XTBF(16, 5, spuStreamCtrl);
			break;
		case DPM_LETTERBOXED:
			spuStreamID = (WORD)XTBF( 8, 5, spuStreamCtrl);
			break;
		case DPM_PANSCAN:
			spuStreamID = (WORD)XTBF( 0, 5, spuStreamCtrl);
			break;
		}

	if (spuEnable) spuStreamID |= 0x40;

	if (numPG)
		{
		GNREASSERT(vobs->GetSubPictureStreamAttribute(spuStream, subPictureAttr, language, extension));
		GNREASSERT(player->SetSubPictureStreamAttributes(spuStreamID, subPictureAttr));
		}

	GNRAISE_OK;
	}

//
//  Start Program Chain Playback
//
//  GNR_OK
//  GNR_OBJECT_IN_USE - The playback is already running
//  GNR_INVALID_NAV_INFO
//  GNR_PARENTAL_LEVEL_TOO_LOW
//  GNR_ILLEGAL_NAVIGATION_COMMAND
//  GNR_UNEXPECTED_NAVIGATION_ERROR
//

Error DVDPGCSequencer::StartPGCPlayback(DVDOBS * vobs, DVDGenericPGCI * pgci, WORD program, BOOL skipPreCommands, BOOL toPause)
	{
	int numCmd, pc, i;
	DVDNavigationCommand * cmd;
	DVDNavigationProcessor::NavCommandDone done;

	if (GetState() != pgcs_idle)
		GNRAISE(GNR_OBJECT_IN_USE);

	this->vobs = vobs;
	this->pgci = pgci;

	GNREASSERT(pgci->GetNumberOfPrograms(numPG));
	GNREASSERT(pgci->GetNumberOfCells(numCell));
	GNREASSERT(pgci->GetUserOperations(pgciUOPs));
	GNREASSERT(pgci->GetPlaybackMode(playbackMode));
	playingCell = 1;

	DP("Starting params numPG %d numCell %d pgciUOPs %d playbackMode %d", numPG, numCell, pgciUOPs, playbackMode);

	if (numPG)
		{
		GNREASSERT(BuildCellTimes(pgci, cellTimes, numCell));	// Always returns GNR_OK

		if (playbackMode == 0)
			{
			currentPG = program;
			loopCnt = 0;
			}
		else if (playbackMode & 0x80)  // Shuffle mode
			{
			loopCnt = playbackMode & 0x7f;
			if (shuffleHistory)
				delete[] shuffleHistory;
			if (playbackShuffleHistory)
				delete[] playbackShuffleHistory;
			shuffleHistory = new BOOL[numPG];
			playbackShuffleHistory = new BOOL[numPG];
			for(i=0; i<numPG; i++)
				shuffleHistory[i] = FALSE;
			currentPG = Rnd(numPG);
			shuffleHistory[currentPG-1] = TRUE;
			for(i=0; i<numPG; i++)
				playbackShuffleHistory[i] = shuffleHistory[i];
			}
		else
			{
			loopCnt = playbackMode & 0x7f;
			currentPG = Rnd(numPG);
			}
		loopCnt++;
		}
	playingLoopCnt = loopCnt;

	GNREASSERT(streamServer->SetOBS(vobs));	// Always returns GNR_OK

	//
	//  Execute Pre Commands
	//

	if (!skipPreCommands)
		{
		SetState(pgcs_preCommand);

		pc = 1;
		GNREASSERT(pgci->GetPreCommands(numCmd, cmd));	// Always returns GNR_OK
		if (numCmd)
			{
			//
			// Execute pre commands and wait in case of SetTMPPML request
			//

//			GNREASSERTMAP(navpu->InterpretCommandList(cmd, pc, numCmd, done, requestedPML), GNR_INVALID_NAV_INFO);
			GNREASSERT(navpu->InterpretCommandList(cmd, pc, numCmd, done, requestedPML));
			if (done == DVDNavigationProcessor::NCD_SETTMPPML_DEFERED)
				{
				SetAwaitPMLChangeState(PDL_STARTPGC, pc);
				GNRAISE_OK;
				}
			}

		if (GetState() != pgcs_preCommand)
			{
			if (toPause) Pause();
			GNRAISE_OK;
			}
		}

	//
	//  Start playback
	//

	GNREASSERT(SetStreamParameters());

	if (numPG)
		{
	   SetState(pgcs_cellPlayback);
		SetScanSpeed(1);
		player->SetPlaybackSpeed(1000);
		breakpointStillState = pgcs_idle;

		playbackDone = FALSE;

		if (toPause) Pause();

		GNREASSERT(StartProgram());
		GNREASSERT(navigationTimer.StartNavigationTimer());
		}
	else
		{
		if (toPause) Pause();
		GNREASSERT(CompletePGCPlayback());
		}

	GNRAISE_OK;
	}

//
//  Start PGC Playback With Parental Level Change
//

Error DVDPGCSequencer::StartPGCPlaybackWithParentalLevel(DVDGenericPGCI * pgci)
	{
	int numCmd, pc;
	DVDNavigationCommand * cmd;
	DVDNavigationProcessor::NavCommandDone done;

	if (GetState() != pgcs_awaitingPMLChange)
		GNRAISE(GNR_UNEXPECTED_NAVIGATION_ERROR);

	//
	// Check if PML has been set (this point is never reached if PML handling is PMH_COMPATIBLE)
	//

	if (pmlHandling == PMH_DEFER_CHANGE)
		GNRAISE(GNR_PARENTAL_LEVEL_TOO_LOW);

	SetState(pgcs_preCommand);
	pc = awaitPMLChangePC;
	GNREASSERT(pgci->GetPreCommands(numCmd, cmd));	// PC stays as it was, numCmd is at least 1 (because of SetTMPPML)

	//
	// Execute pre commands and wait in case of SetTMPPML request
	//

	GNREASSERT(navpu->InterpretCommandList(cmd, pc, numCmd, done, requestedPML));
	if (done == DVDNavigationProcessor::NCD_SETTMPPML_DEFERED)
		{
		SetAwaitPMLChangeState(PDL_STARTPGC, pc);
		GNRAISE_OK;
		}

	if (GetState() != pgcs_preCommand)
		GNRAISE_OK;

	//
	//  Start playback
	//

	GNREASSERT(SetStreamParameters());

	if (numPG)
		{
	   SetState(pgcs_cellPlayback);
		SetScanSpeed(1);
		player->SetPlaybackSpeed(1000);
		breakpointStillState = pgcs_idle;

		playbackDone = FALSE;

//		if (toPause) Pause();	// We don't need this here

		GNREASSERT(StartProgram());
		GNREASSERT(navigationTimer.StartNavigationTimer());
		}
	else
		{
//		if (toPause) Pause();	// We don't need this here
		GNREASSERT(CompletePGCPlayback());
		}

	GNRAISE_OK;
	}

//
//  Start Program Chain Playback
//

Error DVDPGCSequencer::StartPGCPlaybackWithTime(DVDOBS * vobs, DVDGenericPGCI * pgci, DVDTime time, BOOL skipPreCommands, BOOL toPause)
	{
	int numCmd, pc;
	DVDNavigationCommand * cmd;
	DVDNavigationProcessor::NavCommandDone done;

//	skipPreCommands = TRUE;

	if (GetState() != pgcs_idle)
		GNRAISE(GNR_OBJECT_IN_USE);
	else
		{
		this->vobs = vobs;
		this->pgci = pgci;

		GNREASSERT(pgci->GetNumberOfPrograms(numPG));
		GNREASSERT(pgci->GetNumberOfCells(numCell));
		GNREASSERT(pgci->GetPlaybackMode(playbackMode));
		GNREASSERT(pgci->GetUserOperations(pgciUOPs));
		playingCell = 1;

		if (playbackMode)
			GNRAISE(GNR_OBJECT_NOT_FOUND);

		if (numPG)
			{
			GNREASSERT(BuildCellTimes(pgci, cellTimes, numCell));
			}

		GNREASSERT(streamServer->SetOBS(vobs));

		//
		//  Execute PreCommands
		//

		if (!skipPreCommands)
			{
			SetState(pgcs_preCommand);
			pc = 1;
			GNREASSERT(pgci->GetPreCommands(numCmd, cmd));
			if (numCmd)
				GNREASSERT(navpu->InterpretCommandList(cmd, pc, numCmd, done, requestedPML));

			if (GetState() != pgcs_preCommand)
				GNRAISE_OK;
			}

		//
		//  Start Playback
		//

		GNREASSERT(SetStreamParameters());

		if (numPG)
			{
	      SetState(pgcs_cellPlayback);
			SetScanSpeed(1);
			player->SetPlaybackSpeed(1000);
			breakpointStillState = pgcs_idle;

			playbackDone = FALSE;

			currentCell = 1;
			while (currentCell < numCell && cellTimes[currentCell] < time)
				currentCell++;

			GNREASSERT(pgci->GetProgramOfCell(currentCell, currentPG));
			GNREASSERT(pgci->GetLastCellOfProgram(currentPG, finalCell));

			if (toPause) Pause();

			GNREASSERT(StartCellAt(currentCell, time - cellTimes[currentCell-1]));
			GNREASSERT(navigationTimer.StartNavigationTimer());
			}
		else
			{
			if (toPause) Pause();
			GNREASSERT(CompletePGCPlayback());
			}
		}

	GNRAISE_OK;
	}

//
//  Test access restriction of cell by time
//

Error DVDPGCSequencer::AccessRestricted(DVDGenericPGCI * pgci, DVDTime time, BOOL & restricted)
	{
	DVDCPBI cpbi;
	WORD cell;
	WORD numCells;
	DVDTime * times = NULL;

	cell = 1;
	GNREASSERT(pgci->GetNumberOfCells(numCells));
	GNREASSERT(BuildCellTimes(pgci, times, numCells));

	while (cell < numCells && times[cell] < time)
		cell++;

	delete[] times;

	if (cell <= numCells)
		{
		GNREASSERT(pgci->GetCellInformation(cell, cpbi));
		restricted = cpbi.AccessRestricted();
		}
	else
		restricted = FALSE;

	GNRAISE_OK;
	}

//
//  Get User Operations
//

Error DVDPGCSequencer::GetUOPs(DWORD & uops)
	{
	DVDCPBI cpbi;

	if (GetState() == pgcs_idle)
		{
		uops = 0;
		}
	else
		{
		uops = vobuUOPs | pgciUOPs | MKBF(17, !HighlightActive() || ButtonActivated()) |
				 MKBF(18, !(GetState() == pgcs_vobuStill || GetState() == pgcs_cellStill || GetState() == pgcs_pgcStill));

		//
		//  If we reached a breakpoint we have to enable "still off"
		//

		if (breakpointStillState != pgcs_idle)
			uops &= ~UOP_STILL_OFF;

		//
		//  If cell has Access restricted flag set we have to disable scanning
		//

		GNREASSERT(pgci->GetCellInformation(playingCell, cpbi));
		if (cpbi.AccessRestricted())
			uops |= UOP_FORWARD_SCAN | UOP_BACKWARD_SCAN;
		}

	GNRAISE_OK;
	}

//
//  Check if UOP is permitted
//

Error DVDPGCSequencer::UOPPermitted(int uop, BOOL & permitted)
	{
	DWORD uops;

	GNREASSERT(GetUOPs(uops));

	permitted = !XTBF(uop, uops);
	GNRAISE_OK;
	}

//
//  Get Current Location in stream
//

Error DVDPGCSequencer::GetCurrentLocation(DVDLocation & loc)
	{
	DVDTime playbackTime;

	if (GetState() == pgcs_cellPlayback)
		{
		GNREASSERT(pgci->GetProgramOfCell(playingCell, loc.program));
		loc.cell     = playingCell;
		loc.cellTime = cellTime;
		if (IS_ERROR(streamServer->GetPlaybackTime(playbackTime)))
			{
			playbackTime = cellTime + cellTimes[playingCell - 1];
			}
		loc.pgcTime = playbackTime;
		}
	else if (GetState() != pgcs_idle)
		{
		GNREASSERT(pgci->GetProgramOfCell(playingCell, loc.program));
		loc.cell     = playingCell;
		loc.cellTime = cellTime;
		loc.pgcTime = cellTime + cellTimes[playingCell - 1];
		}
	else
		{
		loc.program = 0;
		loc.cell = 0;
		loc.cellTime = 0;
		loc.pgcTime = 0;
		}

	GNRAISE_OK;
	}

//
//  Get Current Duration (of cell)
//

Error DVDPGCSequencer::GetCurrentDuration(DVDLocation & loc)
	{
	if (GetState() != pgcs_idle)
		{
		loc.program  = numPG;
		loc.cell		 = numCell;
		loc.cellTime = cellTimes[playingCell] - cellTimes[playingCell-1];
		GNREASSERT(pgci->GetPresentationTime(loc.pgcTime));
		}
	else
		{
		loc.program = 0;
		loc.cell = 0;
		loc.cellTime = 0;
		loc.pgcTime = 0;
		}

	GNRAISE_OK;
	}

//
//  Stop Program Chain Playback
//

Error DVDPGCSequencer::StopPGCPlayback(void)
	{
	switch (GetState())
		{
		case pgcs_idle:
		case pgcs_preCommand:
		case pgcs_postCommand:
			break;
		case pgcs_cellStill:
			GNREASSERT(streamServer->ReturnCell(stillCell));
			stillCell = NULL;
		case pgcs_pgcStill:
//			GNREASSERT(CompleteStill());
//			GNREASSERT(CancelStillPhase());
//			break;
		case pgcs_cellCommand:
		case pgcs_vobuStill:
			GNREASSERT(CancelStillPhase());
//			GNREASSERT(CompleteStill());
		case pgcs_scanForward:
		case pgcs_scanBackward:
		case pgcs_cellPlayback:
		case pgcs_cellPlaybackReverse:
		case pgcs_cellTrickPlay:
			GNREASSERT(streamServer->AbortTransfer());
			GNREASSERT(player->Resume());
			break;
		}

	GNREASSERT(CancelHighlight());
	GNREASSERT(navigationTimer.StopNavigationTimer());

	pgci = NULL;
	SetState(pgcs_idle);
	streamServer->StopPlayback();
	isMultiAngleScene = FALSE;
	streaming = FALSE;

	if (EventHasHandler(DNE_PARENTAL_LEVEL_CHANGE))
		pmlHandling = PMH_DEFER_CHANGE;
	else
		pmlHandling = PMH_COMPATIBLE;

	player->SetAnalogCopyProtectionMode(0);

	GNRAISE_OK;
	}

//
//  Prepare navigation accurate
//

Error DVDPGCSequencer::PrepareNavigationAccurate(void)
	{
	switch(GetState())
		{
		case pgcs_idle:
			GNRAISE(GNR_OBJECT_NOT_FOUND);
			break;
		case pgcs_preCommand:
		case pgcs_postCommand:
			break;
		case pgcs_cellStill:
			GNREASSERT(streamServer->ReturnCell(stillCell));
			stillCell = NULL;
		case pgcs_vobuStill:
		case pgcs_pgcStill:
			GNREASSERT(CancelStillPhase());
		case pgcs_cellCommand:
			GNREASSERT(streamServer->CompleteStill());
		case pgcs_scanForward:
		case pgcs_scanBackward:
		case pgcs_cellPlayback:
		case pgcs_cellPlaybackReverse:
		case pgcs_cellTrickPlay:
			SetState(pgcs_aborting);
			GNREASSERT(streamServer->AbortTransferAccurate());
			GNREASSERT(player->Resume());
			break;
		}

	SetState(pgcs_navigating);
	breakpointStillState = pgcs_idle;
	programStarted = FALSE;
	streaming = FALSE;

	GNRAISE_OK;
	}

//
//  Prepare Navigation Change
//

Error DVDPGCSequencer::PrepareNavigation(void)
	{
	switch(GetState())
		{
		case pgcs_idle:
			GNRAISE(GNR_OBJECT_NOT_FOUND);
			break;
		case pgcs_preCommand:
		case pgcs_postCommand:
			break;
		case pgcs_cellStill:
			GNREASSERT(streamServer->ReturnCell(stillCell));
			stillCell = NULL;
		case pgcs_vobuStill:
		case pgcs_pgcStill:
			GNREASSERT(CancelStillPhase());
		case pgcs_cellCommand:
//			GNREASSERT(CompleteStill());
		case pgcs_scanForward:
		case pgcs_scanBackward:
		case pgcs_cellPlayback:
		case pgcs_cellPlaybackReverse:
		case pgcs_cellTrickPlay:
			GNREASSERT(streamServer->AbortTransfer());
			GNREASSERT(player->Resume());
			break;
		}

	SetState(pgcs_navigating);
	breakpointStillState = pgcs_idle;
	programStarted = FALSE;
	streaming = FALSE;

	GNRAISE_OK;
	}

//
//  Start Forward Scan
//

Error DVDPGCSequencer::StartForwardScan(WORD speed)
	{
	if (GetState() == pgcs_cellPlayback || GetState() == pgcs_scanForward ||
		 GetState() == pgcs_scanBackward || GetState() == pgcs_cellPlaybackReverse)
		{
		GNREASSERT(PrepareNavigationAccurate());

		SetScanSpeed(speed);
		player->SetPlaybackSpeed(1000);
		SetState(pgcs_scanForward);

		loopCnt = playingLoopCnt;
		GNREASSERT(pgci->GetProgramOfCell(playingCell, currentPG));
		GNREASSERT(pgci->GetLastCellOfProgram(currentPG, finalCell));

		playbackDone = FALSE;

		GNRAISE(StartCell(playingCell, nextVOBU));
		}
	else
		GNRAISE(GNR_OPERATION_PROHIBITED);
	}

//
//  Start Backward Scan
//

Error DVDPGCSequencer::StartBackwardScan(WORD speed)
	{
	if (GetState() == pgcs_cellPlayback || GetState() == pgcs_scanForward ||
		 GetState() == pgcs_scanBackward || GetState() == pgcs_cellPlaybackReverse )
		{
		GNREASSERT(PrepareNavigationAccurate());

		SetScanSpeed(speed);
		player->SetPlaybackSpeed(1000);
		SetState(pgcs_scanBackward);

		loopCnt = playingLoopCnt;
		GNREASSERT(pgci->GetProgramOfCell(playingCell, currentPG));
		GNREASSERT(pgci->GetFirstCellOfProgram(currentPG, finalCell));

		playbackDone = FALSE;

		GNRAISE(StartCell(playingCell, playingVOBU));
		}
	else
		GNRAISE(GNR_OPERATION_PROHIBITED);
	}

//
//  Start Reverse Playback
//

Error DVDPGCSequencer::StartReversePlayback(void)
	{
	if (GetState() == pgcs_cellPlayback || GetState() == pgcs_scanForward ||
		 GetState() == pgcs_scanBackward)
		{
		GNREASSERT(PrepareNavigationAccurate());

		SetScanSpeed(1);
		SetState(pgcs_cellPlaybackReverse);

		GNREASSERT(pgci->GetProgramOfCell(playingCell, currentPG));
		GNREASSERT(pgci->GetFirstCellOfProgram(currentPG, finalCell));

		currentCell = playingCell;
		playbackDone = FALSE;

		GNRAISE(StartCell(playingCell, playingVOBU));
		}

	GNRAISE_OK;
	}

//
//  Start trick playback
//  NOTE: Here we do not care about "Cell Access Restricted" flag since this is
//  prohibited if Fwd/BwdScan is prohibited by UOPs (set if CAR flag is set).
//

Error DVDPGCSequencer::StartTrickPlayback(void)
	{
	BOOL still;

	if (GetState() == pgcs_cellPlayback)
		{
		GNREASSERT(PrepareNavigationAccurate());

		SetState(pgcs_cellTrickPlay);

		GNREASSERT(pgci->GetProgramOfCell(playingCell, currentPG));
		GNREASSERT(pgci->GetFirstCellOfProgram(currentPG, finalCell));

		currentCell = playingCell;

//		GNREASSERT(AdvanceCell());

		still = IsStillCell();

		GNREASSERT(streamServer->TransferCellTrickplay(pgci, playingCell, still, FALSE, cellTimes[playingCell - 1], playingVOBU));
		}

	GNRAISE_OK;
	}

//
//  Stop Scanning, continue with normal playback
//  NOTE: Here wo do not care about "Cell Access Restricted" flag since
//  scan of is exactly what we would want in this case.
//

Error DVDPGCSequencer::StopScan(BOOL toPause)
	{
	BOOL still;

	if (GetState() == pgcs_scanForward || GetState() == pgcs_scanBackward || GetState() == pgcs_cellPlaybackReverse || GetState() == pgcs_cellTrickPlay)
		{
		GNREASSERT(PrepareNavigationAccurate());

		SetState(pgcs_cellPlayback);

		GNREASSERT(pgci->GetProgramOfCell(playingCell, currentPG));
		GNREASSERT(pgci->GetLastCellOfProgram(currentPG, finalCell));

		loopCnt = playingLoopCnt;
		currentCell = playingCell;
		SetScanSpeed(1);

		playbackDone = FALSE;
		GNREASSERT(AdvanceCell());

		still = IsStillCell();

		if (toPause) Pause();

		streaming = TRUE;

		GNREASSERT(streamServer->TransferCell(pgci, playingCell, still, playingCell == finalCell, cellTimes[playingCell - 1], playingVOBU, angle));
		}

	GNRAISE_OK;
	}

//
//  Perform audio stream change
//

Error DVDPGCSequencer::PerformAudioStreamChange(void)
	{
	if (GetState() == pgcs_cellPlayback)
		{
		GNREASSERT(SetAudioStreamParameters());
		}

	GNRAISE_OK;
	}

//
//  Perform SPU stream change
//

Error DVDPGCSequencer::PerformSPUStreamChange(void)
	{
	if (GetState() == pgcs_scanForward || GetState() == pgcs_scanBackward ||
		 GetState() == pgcs_cellPlaybackReverse || GetState() == pgcs_cellTrickPlay ||
	    GetState() == pgcs_cellPlayback)
		{
		GNREASSERT(SetSPUStreamParameters());
		}

	GNRAISE_OK;
	}

//
//  Perform stream change
//

Error DVDPGCSequencer::PerformStreamChange(void)
	{
	if (GetState() == pgcs_cellPlayback)
		{
		GNREASSERT(PrepareNavigationAccurate());

		GNREASSERT(SetStreamParameters());

		SetState(pgcs_cellPlayback);

		GNREASSERT(pgci->GetProgramOfCell(playingCell, currentPG));
		GNREASSERT(pgci->GetLastCellOfProgram(currentPG, finalCell));

		GNRAISE(StartCell(playingCell, playingVOBU));
		}

	GNRAISE_OK;
	}

//
//  Perform angle change
//

Error DVDPGCSequencer::AngleChange(void)
	{
	AngleChangeType type;
	Error err = GNR_OK;

	if (GetState() == pgcs_cellPlayback)
		{

		GNREASSERT(streamServer->GetAngleChangeType(type));

		switch (type)
			{
			case ACT_NONE:
				break;
			case ACT_DEFERRED:
				break;
			case ACT_SEAMLESS:
				DP("Seamless angle change");
				err = streamServer->SeamlessAngleChange(angle);
				break;
			case ACT_NON_SEAMLESS:
				DP("Non seamless angle change");

				GNREASSERT(PrepareNavigationAccurate());

				SetState(pgcs_cellPlayback);

				GNREASSERT(pgci->GetProgramOfCell(playingCell, currentPG));
				GNREASSERT(pgci->GetLastCellOfProgram(currentPG, finalCell));

				GNREASSERT(StartCell(playingCell, playingVOBU));
				SendEvent(DNE_ANGLE_CHANGE, angle);
				break;
			default:
				BREAKPOINT;
			};
		}
	else
		DP("Wrong state during angle change");

	GNRAISE(err);
	}

//
//  Get System Parameter
//

Error DVDPGCSequencer::GetSPRM(int index, WORD & sprm)
	{
	BOOL active;
	DWORD dw;

	switch(index)
		{
		case 3:
			sprm = angle;
			break;
		case 9:
			navigationTimer.QueryNavigationTimer(dw, active);
			if (!active)
				sprm = 0;
			else
				sprm = (WORD)(dw / 1000);
			break;
		case 8:
			sprm = (WORD)(MKBF(10, 6, selectedButton));
			break;
		case 14:
			switch(requestedPresentationMode)
				{
				case DPM_4BY3:
				case DPM_LETTERBOXED:
				case DPM_PANSCAN:
					sprm = 0x0000;
					break;
				case DPM_16BY9:
					sprm = 0x0c00;
					break;
				}

			switch (currentPresentationMode)
				{
				case DPM_4BY3:
					sprm |= 0x0000;
					break;
				case DPM_LETTERBOXED:
					sprm |= 0x0200;
					break;
				case DPM_PANSCAN:
					sprm |= 0x0100;
					break;
				case DPM_16BY9:
					sprm |= 0x0000;
					break;
				}
			break;
		default:
			GNRAISE(DVDNavigationControl::GetSPRM(index, sprm));
		}

	GNRAISE_OK;
	}

//
//  Set System Parameter
//

Error DVDPGCSequencer::SetSPRM(int index, WORD val)
	{
	WORD sprm;

	switch(index)
		{
		case 3:
//			DP("Set angle %d to %d", angle, val);
			if (angle != val)
				{
				angle = val;
				GNRAISE(DVDPGCSequencer::AngleChange());
				}
			break;

		case 1: // Audio Stream
			GNREASSERT(DVDNavigationControl::GetSPRM(1, sprm));
			if (sprm != val)
				{
				GNREASSERT(DVDNavigationControl::SetSPRM(1, val));
				GNREASSERT(PerformAudioStreamChange());
				SendEvent(DNE_AUDIO_STREAM_CHANGE, val);
				}
			break;

		case 2: // SPU Stream
			GNREASSERT(DVDNavigationControl::GetSPRM(2, sprm));
			if (sprm != val)
				{
				GNREASSERT(DVDNavigationControl::SetSPRM(2, val));
				GNREASSERT(PerformSPUStreamChange());
				SendEvent(DNE_SUBPICTURE_STREAM_CHANGE, val);
				}
			break;

		case 8:
			GNRAISE(ButtonSet(val));

		case 9:
			GNREASSERT(GetSPRM(10, sprm));
			if (val)
				navigationTimer.InitNavigationTimer((DWORD)val*1000, sprm);
			else
				navigationTimer.ResetNavigationTimer();
			break;

		default:
			GNRAISE(DVDNavigationControl::SetSPRM(index, val));
		}

	GNRAISE_OK;
	}

//
//  Go to a program
//

Error DVDPGCSequencer::GoProgram(WORD pgn)
	{
	if (!playbackMode && pgn >=1 && pgn <= numPG)
		{
		if (GetState() == pgcs_preCommand)
			currentPG = pgn;
		else
			{
			GNREASSERT(PrepareNavigation());
			currentPG = pgn;
		   SetState(pgcs_cellPlayback);
			GNREASSERT(StartProgram());
			}
		}
	else
		GNRAISE(GNR_RANGE_VIOLATION);

	GNRAISE_OK;
	}

//
//  Go to a cell
//

Error DVDPGCSequencer::GoCell(WORD cell)
	{
	WORD numCells;

	GNREASSERT(pgci->GetNumberOfCells(numCells));
	if (!playbackMode && cell >=1 && cell <= numCells)
		{
		if (GetState() == pgcs_preCommand)
			{
			GNREASSERT(SetStreamParameters());

			SetScanSpeed(1);
			player->SetPlaybackSpeed(1000);
			playbackDone = FALSE;
			}
		else
			GNREASSERT(PrepareNavigation());

		GNREASSERT(pgci->GetProgramOfCell(cell, currentPG));
		GNREASSERT(pgci->GetLastCellOfProgram(currentPG, finalCell));
		currentCell = cell;
	   SetState(pgcs_cellPlayback);
		GNRAISE(StartCell(currentCell));
		}
	else
		GNRAISE(GNR_RANGE_VIOLATION);
	}

//
//  Go to previous program
//

Error DVDPGCSequencer::GoPrevProgram(void)
	{
	GNREASSERT(pgci->GetProgramOfCell(playingCell, playingPG));

	if (!playbackMode && playingPG > 1)
		{
		GNREASSERT(PrepareNavigation());
		currentPG = playingPG - 1;
	   SetState(pgcs_cellPlayback);
		GNREASSERT(StartProgram());
		}
	else
		GNRAISE(GNR_RANGE_VIOLATION);

	GNRAISE_OK;
	}

//
// Go to next program
//

Error DVDPGCSequencer::GoNextProgram(void)
	{
	int i;
	BOOL hasNext;
	WORD nextPGC;

	loopCnt = playingLoopCnt;
	if (shuffleHistory && playbackShuffleHistory)
		{
		for(i=0; i<numPG; i++)
			shuffleHistory[i] = playbackShuffleHistory[i];
		}
	GNREASSERT(HasNextProgram(hasNext));
	if (!hasNext)
		{
		GNREASSERT(pgci->GetNextProgramChain(nextPGC));
		if (!nextPGC)
			GNRAISE_OK;
		}

	//
	//  There is a next program, so let's go there
	//

	GNREASSERT(PrepareNavigation());
	GNREASSERT(pgci->GetProgramOfCell(playingCell, currentPG));
	SetState(pgcs_cellPlayback);
	GNREASSERT(SendNextProgram());
	if (playbackDone)
		GNREASSERT(CompletePGCPlayback());

	GNRAISE_OK;
	}

//
//  Go to top program
//

Error DVDPGCSequencer::GoTopProgram(void)
	{
	GNREASSERT(PrepareNavigation());

	GNREASSERT(pgci->GetProgramOfCell(playingCell, currentPG));

   SetState(pgcs_cellPlayback);
	GNREASSERT(StartProgram());

	GNRAISE_OK;
	}

//
//  Go to previous cell
//

Error DVDPGCSequencer::GoPrevCell(void)
	{
	WORD prog;
	WORD first;

	GNREASSERT(pgci->GetProgramOfCell(playingCell, prog));
	GNREASSERT(pgci->GetFirstCellOfProgram(prog, first));

	if (playingCell > first)
		{
		GNREASSERT(PrepareNavigation());
		loopCnt = playingLoopCnt;
		GNREASSERT(pgci->GetProgramOfCell(playingCell, currentPG));
		currentCell = playingCell - 1;
	   SetState(pgcs_cellPlayback);
		GNRAISE(StartCell(currentCell));
		}
	else
		GNRAISE(GNR_RANGE_VIOLATION);
	}

//
//  Go to next cell
//

Error DVDPGCSequencer::GoNextCell(void)
	{
	DP("GoNextCell Playiung %d Final %d", playingCell, finalCell);
	if (playingCell <= finalCell)
		{
		DP("1");
		GNREASSERT(PrepareNavigation());
		DP("2");
		GNREASSERT(pgci->GetProgramOfCell(playingCell, currentPG));
		DP("3");
	   SetState(pgcs_cellPlayback);
		currentCell = playingCell + 1;
		GNRAISE(StartCell(currentCell));
		DP("4");
		}
	else
		GNRAISE(GNR_RANGE_VIOLATION);

	GNRAISE_OK;
	}

//
//  Go to top cell
//

Error DVDPGCSequencer::GoTopCell(void)
	{
	GNREASSERT(PrepareNavigation());
	currentCell = playingCell;
   SetState(pgcs_cellPlayback);
	GNRAISE(StartCell(currentCell));
	}

//
//  Go to top program chain
//

Error DVDPGCSequencer::GoTopProgramChain(void)
	{
	int numCmd, pc, i;
	DVDNavigationCommand * cmd;
	DVDNavigationProcessor::NavCommandDone done;

	//
	//  Only execute the first part if we didn't wait for PML change
	//

	if (GetState() != pgcs_awaitingPMLChange)
		{
		GNREASSERT(PrepareNavigation());

		if (numPG)
			{
			if (playbackMode == 0)
				{
				currentPG = 1;
				loopCnt = 0;
				}
			else if (playbackMode & 0x80)
				{
				loopCnt = playbackMode & 0x7f;
				for(i=0; i<numPG; i++)
					shuffleHistory[i] = FALSE;
				currentPG = Rnd(numPG);
				shuffleHistory[currentPG-1] = FALSE;
				}
			else
				{
				loopCnt = playbackMode & 0x7f;
				currentPG = Rnd(numPG);
				}
			loopCnt++;
			playingLoopCnt = loopCnt;
			}

		SetState(pgcs_preCommand);

		pc = 1;
		GNREASSERT(pgci->GetPreCommands(numCmd, cmd));
		}
	else
		{
		//
		// Check if PML has been set (this point is never reached if PML handling is PMH_COMPATIBLE)
		//

		if (pmlHandling == PMH_DEFER_CHANGE)
			GNRAISE(GNR_OPERATION_PROHIBITED);

		SetState(pgcs_preCommand);
		pc = awaitPMLChangePC;
		GNREASSERT(pgci->GetPreCommands(numCmd, cmd));
		}

	//
	//  Execute pre commands
	//

	if (numCmd)
		{
		//
		// Execute pre commands and wait in case of SetTMPPML request
		//

		GNREASSERT(navpu->InterpretCommandList(cmd, pc, numCmd, done, requestedPML));
		if (done == DVDNavigationProcessor::NCD_SETTMPPML_DEFERED)
			{
			SetAwaitPMLChangeState(PDL_GOTOPPGC, pc);
			GNRAISE_OK;
			}
		}

	if (GetState() != pgcs_preCommand)
		GNRAISE_OK;

	if (numPG)
		{
      SetState(pgcs_cellPlayback);

		GNREASSERT(StartProgram());
		}
	else
		GNREASSERT(CompletePGCPlayback());

	GNRAISE_OK;
	}

//
//  Go Tail Program Chain
//

Error DVDPGCSequencer::GoTailProgramChain(void)
	{
	GNREASSERT(PrepareNavigation());
	GNREASSERT(CompletePGCPlayback());

	GNRAISE_OK;
	}

//
//  Set Display Mode
//

Error DVDPGCSequencer::SetDisplayMode(DisplayPresentationMode mode)
	{
	if (requestedPresentationMode != mode)
		{
		requestedPresentationMode = mode;

		GNREASSERT(PerformStreamChange());
		SendEvent(DNE_DISPLAY_MODE_CHANGE, mode);
		}

	GNRAISE_OK;
	}

//
//  Resume playback
//

Error DVDPGCSequencer::ResumeFromSystemSpace(DVDOBS * vobs, DVDGenericPGCI * pgci)
	{
	BOOL still = FALSE;
	WORD pgcn;

	if (GetState() != pgcs_idle) GNREASSERT(PrepareNavigation());

	this->vobs = vobs;
	this->pgci = pgci;

	GNREASSERT(pgci->GetNumberOfPrograms(numPG));
	GNREASSERT(pgci->GetNumberOfCells(numCell));
	GNREASSERT(pgci->GetPlaybackMode(playbackMode));
	GNREASSERT(pgci->GetUserOperations(pgciUOPs));

	GNREASSERT(BuildCellTimes(pgci, cellTimes, numCell));

	GNREASSERT(streamServer->SetOBS(vobs));

	GNREASSERT(SetStreamParameters());

	playingLoopCnt = loopCnt = resumeLoopCnt;

   SetState(pgcs_cellPlayback);

	GNREASSERT(pgci->GetProgramOfCell(resumeCell, currentPG));
	playingCell = currentCell = resumeCell;
	GNREASSERT(pgci->GetLastCellOfProgram(currentPG, finalCell));
	playbackDone = FALSE;

	GNREASSERT(StartCell(playingCell, resumeVOBU));

	if (resumeNavtimerActive)
		{
		GetSPRM(10, pgcn);
		navigationTimer.InitNavigationTimer(resumeNavtimerTime, pgcn);
		navigationTimer.StartNavigationTimer();
		}

	GNRAISE_OK;
	}

//
//  Save information for resume
//

Error DVDPGCSequencer::SaveResumeInformation(void)
	{
	resumeVOBU = playingVOBU;
	resumeCell = playingCell;

	resumeLoopCnt = playingLoopCnt;
	if (shuffleHistory && playbackShuffleHistory)
		{
		for(int i=0; i<numPG; i++)
			shuffleHistory[i] = playbackShuffleHistory[i];
		}
	navigationTimer.QueryNavigationTimer(resumeNavtimerTime, resumeNavtimerActive);
	navigationTimer.ResetNavigationTimer(); // a little ugly here...

	GNRAISE_OK;
	}

//
//  Freeze current state
//

Error DVDPGCSequencer::Freeze(DVDPGCSFreezeState & state)
	{
	int i;

	//
	//  Freeze state of base classes
	//

	DVDHighlightControl::Freeze(state.hcFreezeState);
	navpu->Freeze(state.npFreezeState);

	//
	// Freeze miscellaneous
	//

	state.cellTime = cellTime;
	state.angle = angle;

	//
	// Freeze shuffle history
	//

	if (shuffleHistory)
		{
		for (i=0; i < 4; i++)
			state.shuffleHistory[i] = 0;

		state.shuffleHistoryExists = TRUE;

		for (i=0; i<numPG; i++)
			state.shuffleHistory[i >> 5] |= MKBF(i & 0x1f, playbackShuffleHistory[i]);
		}
	else
		state.shuffleHistoryExists = FALSE;

	//
	// Freeze cell data
	//

	state.playingCell = playingCell;
	state.loopCnt = loopCnt;

	//
	// Freeze navtimer information
	//
	navigationTimer.QueryNavigationTimer(state.navtimerTime, state.navtimerActive);

	//
	//  Save resume information
	//

   state.resumeCell = resumeCell;
   state.resumeVOBU = resumeVOBU;
   state.resumeLoopCnt = resumeLoopCnt;
	state.resumeNavtimerActive = resumeNavtimerActive;
	state.resumeNavtimerTime = resumeNavtimerTime;

	GNRAISE_OK;
	}

//
//  Return to previously saved state
//

Error DVDPGCSequencer::Defrost(DVDPGCSFreezeState & state, DVDOBS * vobs, DVDGenericPGCI * pgci, DVDDomain domain, DWORD flags)
	{
	BOOL still = FALSE;
	int i;

	//
	// Restore state of other classes
	//

	DVDHighlightControl::Defrost(state.hcFreezeState);
	GNREASSERT(navpu->Defrost(state.npFreezeState));
	streamServer->Defrost();

	//
	// Reread data
	//

	this->vobs = vobs;
	this->pgci = pgci;

	GNREASSERT(pgci->GetNumberOfPrograms(numPG));
	GNREASSERT(pgci->GetNumberOfCells(numCell));
	GNREASSERT(pgci->GetPlaybackMode(playbackMode));
	GNREASSERT(pgci->GetUserOperations(pgciUOPs));

	//
	// Set up cell times
	//

	cellTime = state.cellTime;
	GNREASSERT(BuildCellTimes(pgci, cellTimes, numCell));

	GNREASSERT(SetStreamParameters());
	SetState(pgcs_cellPlayback);
	SetScanSpeed(1);
	player->SetPlaybackSpeed(1000);
	breakpointStillState = pgcs_idle;

	//
	// Restore cell data
	//

	playingCell = state.playingCell;
	GNREASSERT(pgci->GetProgramOfCell(playingCell, playingPG));
	currentCell = playingCell;
	currentPG = playingPG;
	GNREASSERT(pgci->GetLastCellOfProgram(playingPG, finalCell));

	//
	// Restore miscellaneous
	//

	playingLoopCnt = loopCnt = state.loopCnt;
	angle = state.angle;
	playingAngle = angle;
	SetState(pgcs_cellPlayback);  // Always start normal playback

	//
	// Restore resume data
	//

   resumeCell = state.resumeCell;
   resumeVOBU = state.resumeVOBU;
   resumeLoopCnt = state.resumeLoopCnt;
	resumeNavtimerActive = state.resumeNavtimerActive;
	resumeNavtimerTime = state.resumeNavtimerTime;

	//
	// Restore shuffle history
	//

	if (state.shuffleHistoryExists)
		{
		if (shuffleHistory)
			delete[] shuffleHistory;
		if (playbackShuffleHistory)
			delete[] playbackShuffleHistory;
		shuffleHistory = new BOOL[numPG];
		playbackShuffleHistory = new BOOL[numPG];

		for (i=0; i<numPG; i++)
			playbackShuffleHistory[i] = shuffleHistory[i] = XTBF(i & 0x1f, state.shuffleHistory[i >> 5]);
		}

	playbackDone = FALSE;

	//
	// Misc Setup
	//

	GNREASSERT(streamServer->SetOBS(vobs));
	GNREASSERT(SetStreamParameters());

	if (HighlightActive())
		StartCell(playingCell);
	else
		StartCellAt(playingCell, cellTime);

	//
	// Defrost navtimer information
	//

	if (state.navtimerActive)
		{
		WORD pgcn;

		GetSPRM(10, pgcn);

		navigationTimer.InitNavigationTimer(resumeNavtimerTime, pgcn);
		navigationTimer.StartNavigationTimer();
		}

	if (flags & DDPSPF_TOPAUSE)
		Pause();

	GNRAISE_OK;
	}

//
//  Set State
//

void DVDPGCSequencer::SetState(DVDPGCSequencer::State newState)
	{
	if (newState != state)
		{
		state = newState;
		externalState = newState;
		if (!IsInternalState(newState))
			StateChanged();
		}
	}

//
//  Test if state is internal only
//

BOOL DVDPGCSequencer::IsInternalState(DVDPGCSequencer::State state)
	{
	switch (state)
		{
		case pgcs_cellPlayback:
		case pgcs_vobuStill:
		case pgcs_cellStill:
		case pgcs_pgcStill:
		case pgcs_scanForward:
		case pgcs_scanBackward:
		case pgcs_cellPlaybackReverse:
		case pgcs_cellTrickPlay:
		case pgcs_paused:
			return FALSE;
		default:
			return TRUE;
		}
	}

//
//  Pause Playback
//

Error DVDPGCSequencer::Pause(void)
	{
	Error err = player->Pause();
	if (!IS_ERROR(err))
		{
		navigationTimer.PauseNavigationTimer();
		externalState = pgcs_paused;
		StateChanged();
		}
	return err;
	}

//
//  Resume Playback
//

Error DVDPGCSequencer::Resume(void)
	{
	Error err = player->Resume();
	if (!IS_ERROR(err))
		{
		navigationTimer.ResumeNavigationTimer();
		externalState = GetState();
		StateChanged();
		}
	return err;
	}

//
//  Select Parental Level (to be called from the app only)
//

Error DVDPGCSequencer::ParentalLevelSelect(WORD level)
	{
	WORD pml;

	DP("Parental Level Select %d", level);
	if (level >= 1 && level <= 8 || level == 15)
		{
		if (GetState() != pgcs_awaitingPMLChange)
			{
			SetSPRM(13, level);
			GNRAISE_OK;
			}
		else
			{
			//
			//  Configure navigation processor to accept/reject PML change
			//

			GNREASSERT(GetSPRM(13, pml));
			if (level != pml)
				pmlHandling = PMH_ALLOW_CHANGE;
			else
				pmlHandling = PMH_FORBID_CHANGE;

			//
			//  Restart defered functions
			//

			switch (pmlDeferLocation)
				{
				case PDL_STARTPGC:
					GNRAISE(StartPGCPlaybackWithParentalLevel(pgci));
				case PDL_COMPLETEPGC:
					GNRAISE(CompletePGCPlayback());
				case PDL_GOTOPPGC:
					GNRAISE(GoTopProgramChain());
				default:
					GNRAISE(GNR_UNEXPECTED_NAVIGATION_ERROR);
				}
			}
		}
	else
		GNRAISE(GNR_RANGE_VIOLATION);
	}

//
//  Request PML Change
//

DVDNavigationControl::PMLHandling DVDPGCSequencer::PMLChangeAllowed(void)
	{
	WORD pml;

	switch (pmlHandling)
		{
		case PMH_COMPATIBLE:
			GetSPRM(13, pml);
			if (pml == 15)
				return PMH_ALLOW_CHANGE;
			else
				return PMH_FORBID_CHANGE;

		case PMH_ALLOW_CHANGE:
			pmlHandling = PMH_DEFER_CHANGE;
			return PMH_ALLOW_CHANGE;

		case PMH_FORBID_CHANGE:
			pmlHandling = PMH_DEFER_CHANGE;
			return PMH_FORBID_CHANGE;

		case PMH_DEFER_CHANGE:
			return PMH_DEFER_CHANGE;

		default:
			return PMH_FORBID_CHANGE;
		}
	}

//
//  Wait for parental level change
//

void DVDPGCSequencer::SetAwaitPMLChangeState(PMLDeferLocation deferLoc, int pc)
	{
	//
	//  Send PGCSequencer to a pause state and inform application of SetTMPPML request
	//

	SetState(pgcs_awaitingPMLChange);
	pmlDeferLocation = deferLoc;
	awaitPMLChangePC = pc;
	SendEvent(DNE_PARENTAL_LEVEL_CHANGE, requestedPML);
	}

//
//  Register Event Handler
//

Error DVDPGCSequencer::SetEventHandler(DWORD event, DNEEventHandler handler, void * userData)
	{
	Error err;

	if (!IS_ERROR(err = EventSender::SetEventHandler(event, handler, userData)))
		{
		if (EventHasHandler(DNE_PARENTAL_LEVEL_CHANGE))
			pmlHandling = PMH_DEFER_CHANGE;
		else
			pmlHandling = PMH_COMPATIBLE;
		}

	GNRAISE(err);
	}

//
//  Set Scan Speed
//

Error DVDPGCSequencer::SetScanSpeed(WORD speed)
	{
	if (speed != scanSpeed)
		SendEvent(DNE_SCAN_SPEED_CHANGE, speed);
	scanSpeed = speed;
	GNRAISE_OK;
	}

//
//  Start breakpoint pause
//

void DVDPGCSequencer::StartBreakpointStillCallback()
	{
	//
	//  Go to still state
	//

//	if (GetState() == pgcs_scanForward || GetState() == pgcs_scanBackward)
//		StopScan(TRUE);
//	else
		player->Pause();
	player->SetPlaybackSpeed(1000);
	SetScanSpeed(1);
	StartStillPhase(255);
	SetState(breakpointStillState);
	}

//
//  Test if we reached End-Of-Cell breakpoint
//

BOOL DVDPGCSequencer::EndOfCellBreakpointReached(DVDCell * cell, ERSPlaybackDir dir, ERSBreakpoint & bp)
	{
	WORD num;

	if (IS_ERROR(pgci->GetProgramOfCell(cell->num, num)))
		return FALSE;

	if (dir == EPD_BACKWARD)
		num--;

	return EndOfPGBreakpointReached(num, dir, bp);
	}

//
//  Test if we reached Begin-Of-Cell breakpoint
//

BOOL DVDPGCSequencer::BeginOfCellBreakpointReached(DVDCell * cell, ERSPlaybackDir dir, ERSBreakpoint & bp)
	{
	WORD num;

	if (IS_ERROR(pgci->GetProgramOfCell(cell->num, num)))
		return FALSE;

//	if (dir == EPD_BACKWARD)
//		num--;

	return BeginOfPGBreakpointReached(num, dir, bp);
	}

//
//  Test if we are playing forward
//

BOOL DVDPGCSequencer::IsPlayingForward(void)
	{
	return (GetState() != pgcs_cellPlaybackReverse && GetState() != pgcs_scanBackward);
	}

//
//  Notification of angle change
//

void DVDPGCSequencer::AngleChanged(void)
	{
	playingAngle = angle;
	}

