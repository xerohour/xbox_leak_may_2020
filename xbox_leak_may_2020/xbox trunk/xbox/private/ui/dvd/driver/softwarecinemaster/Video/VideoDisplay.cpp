//depot/xbox-aug01-final/private/ui/dvd/driver/softwarecinemaster/Video/VideoDisplay.cpp#2 - edit change 18236 (text)
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


#include "VideoDisplay.h"
#include "..\common\PerformanceMonitor.h"
#include "library\common\vddebug.h"

#define DUMP_FLIP_TIMES			0
//
//  Fiber Routine
//
#if DUMP_FLIP_TIMES

struct
	{
	int	time, streamTime;
	int	numTimes;
	int	times[16];
	int	avail;
	} dispinfo[40000];
int dispcnt;

#endif

void GenericPictureDisplay::FiberRoutine(void)
	{
	int time, delay, displayTime;

	secondField = FALSE;

	for(;;)
		{
		isPaused = TRUE;
		event.WaitForever();
		if (terminate)
			{
			queueEvent.SetEvent();

			return;
			}
		else if ((!pause || step || !noOfDisplayFrames) && numDisplayBuffers)
			{
			isPaused = FALSE;

			lock.Enter();

			while ((!(pause && (completed || !pendingQueue[0] || !display || display->frame && display->frame->IsFullFrame())) || step || !noOfDisplayFrames) && !terminate)
				{
				switch (numDisplayBuffers)
					{
					case 2:
						if (!pendingQueue[1])
							{
							lock.Leave();
							event.WaitForever();
							lock.Enter();
							}
						else
							{
							time = CurrentTime();

							if (step || time >= timeQueue[1])
								{
								if (pendingQueue[1] != display)
									{
									nextAvail = display;
									display = pendingQueue[1];
									display->DisplayFrame(display->mode);
									noOfDisplayFrames++;

									}


								pendingQueue[1] = NULL;
								step = FALSE;

								queueEvent.SetEvent();
								}
							else
								{
								if (timeQueue[1] - time > 100)
									{
									lock.Leave();
									event.Wait(100);
									lock.Enter();
									if (pendingQueue[1])
										{
										if (pendingQueue[1] != display)
											{
											nextAvail = display;
											display = pendingQueue[1];
											display->DisplayFrame(display->mode);
											noOfDisplayFrames++;

											}

										pendingQueue[1] = NULL;

										queueEvent.SetEvent();
										}
									}
								else
									{
									lock.Leave();
#if HALF_PLAYBACK_SPEED
									delay = 2 * (timeQueue[1] - time);
#else
									delay = timeQueue[1] - time;
#endif
									YieldFiber(delay, TRUE);

									lock.Enter();
									}
								}
							}
						break;
					case 3:
					case 4:
					case 5:
					case 6:
						if (nextAvail && nextAvail->DisplayCompleted())
							{
							if (nextAvail->referenceCount)
								{
								nextAvail->state = PDBS_REFERENCED;
								nextAvail = NULL;
								}
							else
								{
								nextAvail->state = PDBS_IDLE;
								availQueue[firstAvail] = nextAvail;
								firstAvail = (firstAvail + 1) & 15;
								nextAvail = NULL;
								queueEvent.SetEvent();
								}
							}

						if (!pendingQueue[0])
							{
							secondField = FALSE;
							lock.Leave();

							if (nextAvail && firstAvail == lastAvail)
								{
								event.Wait(nextAvail->DisplayCompletionDelay());
								}
							else if (nextAvail)
								{
								event.Wait(100);
								}
							else
								{
								event.WaitForever();
								}
							lock.Enter();
							}
						else
							{
							time = CurrentTime();

							if (step || !noOfDisplayFrames || time + 1 >= timeQueue[0])
								{
#if DUMP_FLIP_TIMES
								dispinfo[dispcnt].time = GetInternalTime();
								dispinfo[dispcnt].streamTime = time;
								int i = 0;
								while (i < 4 && pendingQueue[i])
									{
									dispinfo[dispcnt].times[i] = timeQueue[i];
									i++;
									}
								dispinfo[dispcnt].numTimes = i;
								dispinfo[dispcnt].avail = (firstAvail - lastAvail) & 15;
								dispcnt++;
#endif

								if (!secondField || displayMode == PS_FRAME_PICTURE)
									{
									if (nextAvail)
										{
										if (nextAvail->referenceCount)
											{
											nextAvail->state = PDBS_REFERENCED;
											nextAvail = NULL;
											}
										else
											{
											nextAvail->state = PDBS_IDLE;
											availQueue[firstAvail] = nextAvail;
											firstAvail = (firstAvail + 1) & 15;
											nextAvail = NULL;
											queueEvent.SetEvent();
											}
										}
									nextAvail = display;
									if (nextAvail) nextAvail->state = PDBS_DISPLAYCOMPLETING;
									}

								if (!secondField || display->fmode == PS_FRAME_PICTURE)
									{
									while (pendingQueue[1] && (time > timeQueue[1] + 2 * pendingQueue[1]->fieldTime && !step || step && secondField))
										{
										OutputDebugString("Drop 1\n");
										if (!secondField || displayMode == PS_FRAME_PICTURE)
											{
											if (pendingQueue[0]->referenceCount)
												{
												pendingQueue[0]->state = PDBS_REFERENCED;
												}
											else
												{
												pendingQueue[0]->state = PDBS_IDLE;
												availQueue[firstAvail] = pendingQueue[0];
												firstAvail = (firstAvail + 1) & 15;
												queueEvent.SetEvent();
												}
											}
										else
											{
											if (nextAvail)
												{
												if (nextAvail->referenceCount)
													{
													nextAvail->state = PDBS_REFERENCED;
													nextAvail = NULL;
													}
												else
													{
													nextAvail->state = PDBS_IDLE;
													availQueue[firstAvail] = nextAvail;
													firstAvail = (firstAvail + 1) & 15;
													nextAvail = NULL;
													queueEvent.SetEvent();
													}
												}
											nextAvail = display;
											if (nextAvail) nextAvail->state = PDBS_DISPLAYCOMPLETING;
											}

										pendingQueue[0] = pendingQueue[1];
										timeQueue[0] = timeQueue[1];
										pendingQueue[1] = pendingQueue[2];
										timeQueue[1] = timeQueue[2];
										pendingQueue[2] = pendingQueue[3];
										timeQueue[2] = timeQueue[3];
										pendingQueue[3] = NULL;

										secondField = FALSE;
										}
									}

								displayTime = timeQueue[0];
								display = pendingQueue[0];
								displayMode = display->mode;
								if (secondField || display->mode == PS_FRAME_PICTURE || display->fmode == PS_FRAME_PICTURE && displayTime + display->fieldTime <= time || step)
									{
									pendingQueue[0] = pendingQueue[1];
									timeQueue[0] = timeQueue[1];
									pendingQueue[1] = pendingQueue[2];
									timeQueue[1] = timeQueue[2];
									pendingQueue[2] = pendingQueue[3];
									timeQueue[2] = timeQueue[3];
									pendingQueue[3] = NULL;

									secondField = FALSE;
									if      (displayMode == PS_TOP_FIELD)    displayMode = PS_BOTTOM_FIELD;
									else if (displayMode == PS_BOTTOM_FIELD) displayMode = PS_TOP_FIELD;
									}
								else
									{
									timeQueue[0] += display->fieldTime;
									secondField = TRUE;
									}

								display->state = PDBS_DISPLAYING;
								noOfDisplayFrames++;
								lock.Leave();
								display->DisplayFrame(displayMode);
								lock.Enter();
								queueEvent.SetEvent();
								if (step)
									{
									UpdateTimer(displayTime);
									}
								step = FALSE;
								}
							else
								{
								lock.Leave();

								if (nextAvail && firstAvail == lastAvail)
									{
									int t1 = timeQueue[0] - time;
									int t2 = nextAvail->DisplayCompletionDelay();

									event.Wait(min(t1, t2), TRUE);
									}
								else
									{
									event.Wait(timeQueue[0] - time, TRUE);
									}

								lock.Enter();
								}
							}
						break;
					default:
						lock.Leave();
						event.WaitForever();
						lock.Enter();
						break;
					}
				}

			lock.Leave();
			if (terminate)
				{
				queueEvent.SetEvent();

				return;
				}
			}
		}
	}

//
//  Constructor
//

#pragma warning(disable : 4355)
GenericPictureDisplay::GenericPictureDisplay(void) :
	event(FALSE, TRUE), queueEvent(FALSE, TRUE), TimedFiber(6), timingClient(this)
	{
	numDisplayBuffers = 0;
	display = NULL; nextAvail = NULL;
	macrovisionEncoder = NULL;
	streaming = FALSE;
	preferedDeinterlaceMode = DEIF_DEINTERLACE_WEAVE | DEIF_DEINTERLACE_BOB;
	spuDeferData.map = NULL;
	presentationMode = PDPM_FULLSIZE;
	errorMessenger = NULL;
	}
#pragma warning(default : 4355)

//
//  Destructor
//

GenericPictureDisplay::~GenericPictureDisplay(void)
	{
	if (streaming) EndStreaming(TRUE);
	}

//
//  Init Display
//

void GenericPictureDisplay::InitDisplay(int width, int height, int minStretch)
	{
	int i, j;

	if (!numDisplayBuffers)
		{
		AllocateDisplayBuffers();
		ClearDisplayBuffers();
		}

	if (numDisplayBuffers == 2 || scanning)
		{
		display = displayBuffers[0];
		nextAvail = displayBuffers[1];
		pendingQueue[1] = NULL;
		}
	else if (numDisplayBuffers >= 3)
		{
		if (!display)
			{
			display = displayBuffers[numDisplayBuffers - 1];
			display->mode = PS_FRAME_PICTURE;
			}

		for(i=0; i<numDisplayBuffers; i++)
			{
			displayBuffers[i]->referenceCount = 0;
			}

		i = 0;
		for(j=0; j<numDisplayBuffers-1; j++)
			{
			if (displayBuffers[i] == display) i++;

			availQueue[j] = displayBuffers[i];
			availQueue[j]->state = PDBS_IDLE;
			i++;
			}
		display->state = PDBS_DISPLAYING;

		pendingQueue[0] = NULL;
		pendingQueue[1] = NULL;
		pendingQueue[2] = NULL;
		pendingQueue[3] = NULL;
		firstAvail = numDisplayBuffers - 1;
		lastAvail = 0;
		display->DisplayFrame(display->mode);
		}
	else
		{
		display = NULL;
		nextAvail = 0;
		}
	}

//
//  Hide Display
//

void GenericPictureDisplay::HideDisplay(void)
	{
	display = NULL;
	if (numDisplayBuffers) FreeDisplayBuffers();
	}

//
//  Obtain IP Frame Buffer Reference
//

void GenericPictureDisplay::ObtainIPFrameBufferReference(PictureDisplayBuffer * frame)
	{
	lock.Enter();
	frame->referenceCount++;
	lock.Leave();
	}

//
//  Release IP Frame Buffer Reference
//

void GenericPictureDisplay::ReleaseIPFrameBufferReference(PictureDisplayBuffer * frame)
	{
	lock.Enter();
	frame->referenceCount--;

	if (!frame->referenceCount && frame->state == PDBS_REFERENCED)
		{
		frame->state = PDBS_IDLE;
		availQueue[firstAvail] = frame;
		firstAvail = (firstAvail + 1) & 15;
		queueEvent.SetEvent();
		}

	lock.Leave();
	}

//
//  Invalidate decoding buffer
//

void GenericPictureDisplay::InvalidateDecodingBuffer(void)
	{
	int i;

	lock.Enter();

	for(i=0; i<numDisplayBuffers; i++)
		displayBuffers[i]->InvalidateBuffer();

	lock.Leave();
	}

//
//  Begin streaming
//

void GenericPictureDisplay::BeginStreaming(bool scanning)
	{
	int i;

	lock.Enter();

	this->scanning = scanning;

	terminate = FALSE;
	pause = TRUE;
	step = FALSE;
	if (!numDisplayBuffers) display = NULL;
	nextAvail = NULL;

	for(i=0; i<NUMOSDPREVIEW; i++)
		osdMap[i] = NULL;

	spuDeferData.map = NULL;
	initialDisplayTime = 0;

	if (!scanning)
		{
		SetFiberPriority(4);
		StartFiber();
		}

	ResetTimer();

	streaming = TRUE;
	completed = FALSE;
	noOfDisplayFrames = 0;

	lock.Leave();
	}

//
//  End streaming
//

void GenericPictureDisplay::EndStreaming(bool fullReset)
	{
	lock.Enter();

	int i;

	if (streaming)
		{
		terminate = TRUE;
		queueEvent.SetEvent();
		if (!scanning)
			{
			event.SetEvent();
			lock.Leave();
			CompleteFiber();
			lock.Enter();
			}
		for(i=0; i<NUMOSDPREVIEW; i++)
			osdMap[i] = NULL;
		spuDeferData.map = NULL;
		pendingQueue[0] = NULL;
		pendingQueue[1] = NULL;
		pendingQueue[2] = NULL;
		pendingQueue[3] = NULL;
		streaming = FALSE;
		}

	lock.Leave();
	}

//
//  Start streaming
//

void GenericPictureDisplay::StartStreaming(int playbackSpeed)
	{
	pause = FALSE;
	if (!scanning)
		{
		EndFreezeDisplay();

		SetPlaybackSpeed(playbackSpeed);

		StartTimer();
		event.SetEvent();
		queueEvent.SetEvent();
		}
	}

//
//  Stop streaming
//

void GenericPictureDisplay::StopStreaming(void)
	{
	int timeOut;

	if (!scanning)
		{
		lock.Enter();

		pause = TRUE;
		event.SetEvent();

		timeOut = 10;

		//
		// Timeout condition in case, no data is sent at all...
		//
		while (!isPaused && (noOfDisplayFrames > 0 || timeOut > 0))
			{
			lock.Leave();
			YieldTimedFiber(20);
			timeOut--;
			lock.Enter();
			}

		StopTimer();

		BeginFreezeDisplay();

		lock.Leave();
		}
	else
		pause = TRUE;
	}

//
//  Done streaming
//

void GenericPictureDisplay::DoneStreaming(void)
	{
	int timeout;

	lock.Enter();

	timeout = GetInternalTime() + 200;

	while (!pause && !terminate && !(display && (numDisplayBuffers == 2 ?
			 (nextAvail != 0) : ((firstAvail - lastAvail & 15) == numDisplayBuffers - 1))) &&
			 GetInternalTime() < timeout)
		{
		lock.Leave();
		event.SetEvent();
		YieldTimedFiber(1);
		lock.Enter();
		}

	completed = TRUE;

	if (!terminate && display)
		{
		if (spuDeferData.map)
			{
			spuDeferData.map->PerformButtonChange();
			PrepareOSDBitmap(spuDeferData.map);

			osdMap[spuDeferData.mid] = spuDeferData.map;
			osdStartTime[spuDeferData.mid] = spuDeferData.startTime;
			osdStopTime[spuDeferData.mid] = spuDeferData.stopTime;

			if (osdStopTime[spuDeferData.mid] < CurrentTime()) osdMap[spuDeferData.mid] = NULL;

			spuDeferData.map = NULL;
			}

		SPUDisplayBuffer * map = FindOSDBitmap(0x7fffffff);

		if (map)
			display->UpdateOSDBitmap(map);
		}

	lock.Leave();
	}

//
//  Find OSD Bitmap
//

SPUDisplayBuffer * GenericPictureDisplay::FindOSDBitmap(int time)
	{
	int i;

	for(i=0; i<NUMOSDPREVIEW; i++)
		{
		if (osdMap[i] && (time == 0x7fffffff || time >= osdStartTime[i] && time <= osdStopTime[i]))
			return osdMap[i];
		}

	return NULL;
	}

//
//  Advance frame
//

void GenericPictureDisplay::AdvanceFrame(void)
	{
	if (pause)
		{
		step = TRUE;
		event.SetEvent();
		}
	}

//
//  Post IP Frame Buffer
//

void GenericPictureDisplay::PostIPFrameBuffer(PictureDisplayBuffer * frame, int displayTime)
	{
	lock.Enter();

	completed = FALSE;
	if (!initialDisplayTime) initialDisplayTime = displayTime;

	frame->DoneDecoding();

	if (numDisplayBuffers == 2 || scanning)
		{
		while (!terminate && !scanning && pendingQueue[1])
			{
			lock.Leave();
			queueEvent.WaitForever();
			lock.Enter();
			}
		if (!terminate)
			{
			if (!scanning && displayTime > CurrentTime())
				{
				pendingQueue[1] = frame;
				timeQueue[1] = displayTime;
				event.SetEvent();
				}
			else
				{
				nextAvail = display;
				display = frame;
				noOfDisplayFrames++;
				display->DisplayFrame(display->mode);
				}
			}
		}
	else
		{
		if (frame->IsFrameComplete())
			{
			frame->state = PDBS_PENDING;

			while (!terminate && pendingQueue[3])
				{
				lock.Leave();
				queueEvent.WaitForever();
				lock.Enter();
				}

			if (!terminate)
				{
				if (!pendingQueue[0])
					{
					if (displayTime < CurrentTime())
						{
						pendingQueue[0] = frame;
						timeQueue[0] = displayTime;
						event.SetEvent();
						}
					else
						{
						pendingQueue[0] = frame;
						timeQueue[0] = displayTime;
						event.SetEvent();
						}
					}
				else if (!pendingQueue[1])
					{
					pendingQueue[1] = frame;
					timeQueue[1] = displayTime;
					}
				else if (!pendingQueue[2])
					{
					pendingQueue[2] = frame;
					timeQueue[2] = displayTime;
					}
				else
					{
					pendingQueue[3] = frame;
					timeQueue[3] = displayTime;
					}
				}
			}
		else
			{
			if (frame->referenceCount)
				{
				frame->state = PDBS_REFERENCED;
				}
			else
				{
				frame->state = PDBS_IDLE;
				availQueue[firstAvail] = frame;
				firstAvail = (firstAvail + 1) & 15;
				}
			}
		}

	lock.Leave();
	}

//
//  Get IP Frame Buffer
//

PictureDisplayBuffer * GenericPictureDisplay::GetIPFrameBuffer(int approxDisplayTime)
	{
	PictureDisplayBuffer * buffer;

	int startTime = GetInternalTime();

	lock.Enter();

	if (spuDeferData.map)
		{
		spuDeferData.map->PerformButtonChange();
		PrepareOSDBitmap(spuDeferData.map);

		osdMap[spuDeferData.mid] = spuDeferData.map;
		osdStartTime[spuDeferData.mid] = spuDeferData.startTime;
		osdStopTime[spuDeferData.mid] = spuDeferData.stopTime;

		if (osdStopTime[spuDeferData.mid] < CurrentTime()) osdMap[spuDeferData.mid] = NULL;

		spuDeferData.map = NULL;
		}

	if (numDisplayBuffers == 2 || scanning)
		{
		while (!scanning && !terminate && !nextAvail)
			{
			lock.Leave();
			queueEvent.WaitForever();
			lock.Enter();
			}

		if (!terminate && nextAvail)
			{
			buffer = nextAvail;
			nextAvail = NULL;
			while (!terminate && !buffer->DisplayCompleted())
				{
				lock.Leave();
				YieldTimedFiber(1);
				lock.Enter();
				}

			if (terminate)
				{
				buffer = NULL;
				}
			}
		else
			buffer = NULL;
		}
	else if (numDisplayBuffers >= 3)
		{
		while (!terminate && (firstAvail == lastAvail ||
			     (pause && pendingQueue[0])))
			{
			event.SetEvent();
			lock.Leave();
			queueEvent.WaitForever();
			lock.Enter();
			}
		if (!terminate)
			{
			buffer = availQueue[lastAvail];
			lastAvail = (lastAvail + 1) & 15;

			while (!terminate &&!(buffer->DisplayCompleted()))
				YieldTimedFiber(buffer->DisplayCompletionDelay());
			}
		else
			buffer = NULL;
		}
	else
		buffer = NULL;

	if (buffer)
		{
		buffer->BeginDecoding(FindOSDBitmap(approxDisplayTime));
		buffer->state = PDBS_DECODING;
		}

	lock.Leave();

	return buffer;
	}

//
//  Post B Frame Buffer
//

void GenericPictureDisplay::PostBFrameBuffer(PictureDisplayBuffer * frame, int displayTime)
	{
	lock.Enter();

	completed = FALSE;
	frame->DoneDecoding();

	if (numDisplayBuffers != 2)
		{
		if (scanning)
			{
			if (!terminate)
				{
				nextAvail = display;
				display = frame;
				noOfDisplayFrames++;
				display->DisplayFrame(display->mode);
				}
			}
		else
			{
			if (frame->IsFrameComplete())
				{
				frame->state = PDBS_PENDING;

				while (!terminate && pendingQueue[3])
					{
					lock.Leave();
					queueEvent.WaitForever();
					lock.Enter();
					}

				if (!terminate)
					{
					if (!pendingQueue[0])
						{
						if (displayTime < CurrentTime())
							{
							pendingQueue[0] = frame;
							timeQueue[0] = displayTime;
							event.SetEvent();
							}
						else
							{
							pendingQueue[0] = frame;
							timeQueue[0] = displayTime;
							event.SetEvent();
							}
						}
					else if (!pendingQueue[1])
						{
						pendingQueue[1] = frame;
						timeQueue[1] = displayTime;
						}
					else if (!pendingQueue[2])
						{
						pendingQueue[2] = frame;
						timeQueue[2] = displayTime;
						}
					else
						{
						pendingQueue[3] = frame;
						timeQueue[3] = displayTime;
						}
					}
				}
			else
				{
				if (frame->referenceCount)
					{
					frame->state = PDBS_REFERENCED;
					}
				else
					{
					frame->state = PDBS_IDLE;
					availQueue[firstAvail] = frame;
					firstAvail = (firstAvail + 1) & 15;
					}
				}
			}
		}

	lock.Leave();
	}

//
//  Get B Frame Buffer
//

PictureDisplayBuffer * GenericPictureDisplay::GetBFrameBuffer(int displayTime)
	{
	PictureDisplayBuffer * buffer;

	int startTime = GetInternalTime();

	lock.Enter();

	if (scanning)
		{
		if (!terminate && nextAvail)
			{
			buffer = nextAvail;
			nextAvail = NULL;
			while (!terminate && !buffer->DisplayCompleted())
				{
				lock.Leave();
				YieldTimedFiber(1);
				lock.Enter();
				}

			if (terminate)
				{
				buffer = NULL;
				}
			}
		else
			buffer = NULL;
		}
	else if (numDisplayBuffers == 2)
		{
		if (terminate)
			{
			buffer = NULL;
			}
		else if (displayTime <= CurrentTime())
			{
			buffer = display;
			}
		else
			{
			while (!terminate && pendingQueue[1])
				{
				lock.Leave();
				queueEvent.WaitForever();
				lock.Enter();
				}

			if (terminate)
				buffer = NULL;
			else if (displayTime <= CurrentTime())
				{
				buffer = display;
				}
			else
				{
				pendingQueue[1] = display;
				timeQueue[1] = displayTime;
				event.SetEvent();
				while (!terminate && pendingQueue[1])
					{
					lock.Leave();
					queueEvent.WaitForever();
					lock.Enter();
					}
				if (terminate)
					buffer = NULL;
				else
					buffer = display;
				}
			}
		}
	else if (numDisplayBuffers >= 3)
		{
		while (!terminate && firstAvail == lastAvail)
			{
			event.SetEvent();
			lock.Leave();
			queueEvent.WaitForever();
			lock.Enter();
			}

		if (!terminate)
			{
			buffer = availQueue[lastAvail];
			lastAvail = (lastAvail + 1 ) & 15;

			while (!terminate &&!(buffer->DisplayCompleted()))
				YieldTimedFiber(buffer->DisplayCompletionDelay());
			}
		else
			buffer = NULL;
		}
	else
		buffer = NULL;

	if (buffer)
		{
		buffer->BeginDecoding(FindOSDBitmap(displayTime));
		buffer->state = PDBS_DECODING;
		}

	lock.Leave();

	return buffer;
	}

//
//  Final Frame Is Displayed
//

bool GenericPictureDisplay::FinalFrameIsDisplayed(void)
	{
	return (display && (numDisplayBuffers == 2 ?
		    (nextAvail != 0) : ((firstAvail - lastAvail & 15) + (nextAvail != 0) == numDisplayBuffers - 1)));
	}

//
//  Update OSD Bitmap
//

void GenericPictureDisplay::UpdateOSDBitmap(SPUDisplayBuffer * map)
	{
	if (display && (numDisplayBuffers == 2 ?
		(nextAvail != 0) : ((firstAvail - lastAvail & 15) + (nextAvail != 0) == numDisplayBuffers - 1)))
		{
		display->UpdateOSDBitmap(map);
		}
	}

//
//  Defer Update OSD Bitmap
//

bool GenericPictureDisplay::DeferUpdateOSDBitmap(SPUDisplayBuffer * map, int mid, int startTime, int stopTime)
	{
	if (!terminate && streaming && !pause && !completed)
		{
		spuDeferData.map = map;
		spuDeferData.mid = mid;
		spuDeferData.startTime = startTime;
		spuDeferData.stopTime = stopTime;

		return TRUE;
		}
	else
		return FALSE;
	}

//
//  Post OSD Bitmap
//

void GenericPictureDisplay::PostOSDBitmap(SPUDisplayBuffer * map, int startTime, int stopTime)
	{
	int	time, mid;
	int i;

	lock.Enter();

	if (!terminate)
		{
		time = CurrentTime();

		if (spuDeferData.map)
			{
			if (spuDeferData.map == map)
				{
				spuDeferData.startTime = startTime;
				spuDeferData.stopTime = stopTime;

				lock.Leave();

				return;
				}

			spuDeferData.map->PerformButtonChange();
			PrepareOSDBitmap(spuDeferData.map);

			osdMap[spuDeferData.mid] = spuDeferData.map;
			osdStartTime[spuDeferData.mid] = spuDeferData.startTime;
			osdStopTime[spuDeferData.mid] = spuDeferData.stopTime;

			if (osdStopTime[spuDeferData.mid] < CurrentTime()) osdMap[spuDeferData.mid] = NULL;

			spuDeferData.map = NULL;
			}

		for(i=0; i<NUMOSDPREVIEW; i++)
			{
			if (osdStopTime[i] < time) osdMap[i] = NULL;
			}

		for(mid = 0; mid<NUMOSDPREVIEW; mid++)
			{
			if (map == osdMap[mid]) break;
			}

		if (mid == NUMOSDPREVIEW)
			{
			for(mid = 0; mid<NUMOSDPREVIEW; mid++)
				{
				if (!osdMap[mid]) break;
				}
			}

		if (mid == NUMOSDPREVIEW)
			{
			mid = 0;
			for(i=1; i<NUMOSDPREVIEW; i++)
				{
				if (osdStartTime[i] < osdStartTime[mid]) mid = i;
				}
			}

		if (map != osdMap[mid] || !DeferUpdateOSDBitmap(map, mid, startTime, stopTime))
			{
			osdMap[mid] = map;
			osdStartTime[mid] = startTime;
			osdStopTime[mid] = stopTime;

			if (osdStopTime[mid] < time) osdMap[mid] = NULL;

			if (osdMap[mid])
				{
				osdMap[mid]->PerformButtonChange();
				PrepareOSDBitmap(osdMap[mid]);

				if (startTime <= time && stopTime >= time)
					UpdateOSDBitmap(osdMap[mid]);
				}
			}
		}

	lock.Leave();
	}

//
//  Invalidate OSD Bitmaps
//

void GenericPictureDisplay::InvalidateOSDBitmaps(void)
	{
	int i;

	lock.Enter();

	spuDeferData.map = NULL;

	for(i=0; i<NUMOSDPREVIEW; i++)
		osdMap[i] = NULL;

	lock.Leave();
	}

//
//  Set Video Start Time
//

void GenericPictureDisplay::SetVideoStartTiming(int startTime)
	{
	if (startTime > CurrentTime())
		{
		UpdateTimer(startTime);
		event.SetEvent();
		}
	}

void GenericPictureDisplay::SetDisplayPresentationMode(PictureDisplayPresentationMode presentationMode)
	{
	DP("Presentationmode %d", presentationMode);
//	if (presentationMode == PDPM_PANSCAN) presentationMode = PDPM_LETTERBOXED;
	this->presentationMode = presentationMode;
	}
