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


#include "SPUDisplay.h"
#include "library\common\vddebug.h"
#include "..\common\TimedFibers.h"

#define DUMP_SPU_EVENTS		0

//
//  Fiber Routine
//

void SPUDisplay::FiberRoutine(void)
	{
	int time, stop, osdNext, start, num;

	lock.Enter();

	while (!terminate)
		{
		if (osdFirst != osdLast)
			{
			time = display->CurrentTime();
			start = pendingQueue[osdFirst].startTime;
			stop = pendingQueue[osdFirst].stopTime;

			osdNext = (osdFirst + 1) % NUM_OSD_BITMAPS;
			if (stop < time)
				{
				display->PostOSDBitmap(pendingQueue[osdFirst].map, pendingQueue[osdFirst].startTime, pendingQueue[osdFirst].stopTime);
				availQueue[osdAvail].map = pendingQueue[osdFirst].map;
				availQueue[osdAvail].map->ReleaseBuffer();
				osdAvail++;
				osdFirst = osdNext;
				queueEvent.SetEvent();
				}
			else
				{
				if (!pendingQueue[osdFirst].posted || buttonChanged || buttonState != pendingQueue[osdFirst].buttonState)
					{
					pendingQueue[osdFirst].map->SetColorTable(palette);
					if (buttonState != pendingQueue[osdFirst].buttonState)
						{
						switch (buttonState)
							{
							case XSBS_DISABLED:
#if DUMP_SPU_EVENTS
								OutputDebugString("Setting button to disabled\n");
#endif
								pendingQueue[osdFirst].map->SetButton(buttonX, buttonY, buttonW, buttonH, pendingQueue[osdFirst].colors);
								break;
							case XSBS_SELECTED:
#if DUMP_SPU_EVENTS
								OutputDebugString("Setting button to selected\n");
#endif
								pendingQueue[osdFirst].map->SetButton(buttonX, buttonY, buttonW, buttonH, selectedColors);
								break;
							case XSBS_ACTIVATED:
#if DUMP_SPU_EVENTS
								OutputDebugString("Setting button to activated\n");
#endif
								pendingQueue[osdFirst].map->SetButton(buttonX, buttonY, buttonW, buttonH, activeColors);
								break;
							}
						}

					pendingQueue[osdFirst].posted = TRUE;
					buttonChanged = FALSE;
					}

				if (enable || pendingQueue[osdFirst].forced)
					{
					pendingQueue[osdFirst].map->CalculateNonTransparentRegion();
					display->PostOSDBitmap(pendingQueue[osdFirst].map, pendingQueue[osdFirst].startTime, pendingQueue[osdFirst].stopTime);
					}

				num = display->NumOSDBitmapPreview() - 1;
				while (num && osdNext != osdLast)
					{
					if (!pendingQueue[osdNext].posted)
						{
						pendingQueue[osdNext].map->SetColorTable(palette);
						pendingQueue[osdNext].posted = TRUE;
						}
					if (enable || pendingQueue[osdNext].forced)
						{
						pendingQueue[osdNext].map->CalculateNonTransparentRegion();
						display->PostOSDBitmap(pendingQueue[osdNext].map, pendingQueue[osdNext].startTime, pendingQueue[osdNext].stopTime);
						}

					osdNext = (osdNext + 1) % NUM_OSD_BITMAPS;
					num--;
					}

				if (pause)
					{
					lock.Leave();
					event.WaitForever();
					lock.Enter();
					}
				else
					{
					lock.Leave();
					if (start > time)
						{
						timingClient.Wait(&event, start);
						}
					else 	if (stop != 0x7fffffff)
						{
						timingClient.Wait(&event, stop);
						}
					else
						{
						event.WaitForever();
						}
					lock.Enter();
					}
				}
			}
		else
			{
			//
			// Nothing to do, wait for better times
			//

			lock.Leave();
//			DP("SPUWaitForeverBegin");
			event.WaitForever();
//			DP("SPUWaitForeverEnd");
			lock.Enter();
			}
		}

	queueEvent.SetEvent();
	lock.Leave();
	}

//
//  Constructor
//

SPUDisplay::SPUDisplay(GenericPictureDisplay * display, SPUDisplayBufferFactory * highQualityOsdFactory, SPUDisplayBufferFactory * lowQualityOsdFactory)
	: event(FALSE, TRUE), queueEvent(FALSE, TRUE), TimedFiber(7), timingClient(display)
	{
	this->display = display;
	this->highQualityOsdFactory = highQualityOsdFactory;
	this->lowQualityOsdFactory = lowQualityOsdFactory;

	osdAvail = 0;
	streaming = FALSE;
	}

//
//  Begin Streaming
//

void SPUDisplay::BeginStreaming(bool highQuality)
	{
	int i;
	SPUDisplayBufferFactory	*	osdFactory = highQuality ? highQualityOsdFactory : lowQualityOsdFactory;

	terminate = FALSE;
	pause = TRUE;

	osdFirst = osdLast = 0;
	osdActive = FALSE;
	buttonChanged = FALSE;
	buttonState = XSBS_DISABLED;
	buttonX = buttonY = buttonW = buttonH = 0;
	newSelectedColors = selectedColors = 0x00000000;
	newActiveColors = activeColors = 0x00000000;

	for(i=0; i<display->NumOSDBitmapPreview() + 1; i++)
		{
		availQueue[i].map = osdFactory->CreateSPUDisplayBuffer();
		}
	osdAvail = display->NumOSDBitmapPreview() + 1;

	streaming = TRUE;

	SetFiberPriority(1);
	StartFiber();
	}

//
//  Start Streaming
//

void SPUDisplay::StartStreaming(int playbackSpeed)
	{
	pause = FALSE;
	event.SetEvent();
	}

//
//  Stop Streaming
//

void SPUDisplay::StopStreaming(void)
	{
	pause = TRUE;
	event.SetEvent();
	}

//
//  End Streaming
//

void SPUDisplay::EndStreaming(bool fullReset)
	{
	if (streaming)
		{
		terminate = TRUE;
		event.SetEvent();
		CompleteFiber();

		display->InvalidateOSDBitmaps();

		lock.Enter();

		while (osdAvail)
			{
			osdAvail--;
			delete availQueue[osdAvail].map;
			}

		while (osdFirst != osdLast)
			{
			delete pendingQueue[osdFirst].map;
			osdFirst = (osdFirst + 1) % NUM_OSD_BITMAPS;
			}

		streaming = FALSE;
		lock.Leave();
		}
	}

void SPUDisplay::VideoDisplayBeginStreaming(void)
	{
	int i;

	lock.Enter();

	if (osdFirst != osdLast)
		{
		i = osdFirst;
		while (i != osdLast)
			{
			pendingQueue[i].posted = false;
			i = (i + 1) % NUM_OSD_BITMAPS;
			}

		event.SetEvent();
		}

	lock.Leave();
	}

//
//  Destructor
//

SPUDisplay::~SPUDisplay(void)
	{
	if (streaming) EndStreaming(TRUE);
	}

//
//  Set Button State
//

void SPUDisplay::SetButtonState(XSPUButtonState state)
	{
	lock.Enter();

#if DUMP_SPU_EVENTS
		{
		char buffer[100];
		wsprintf(buffer, "Set button %d was %d\n", state, buttonState);
		OutputDebugString(buffer);
		}
#endif

	if (buttonState != state)
		{
		buttonState = state;
		buttonChanged = TRUE;
		}

	if (buttonChanged)
		{
		event.SetEvent();
		}


	lock.Leave();
	}

//
//  Set Button Position
//

void SPUDisplay::SetButtonPosition(int x, int y, int w, int h)
	{
	lock.Enter();

	if (x != buttonX || y != buttonY || w != buttonW || h != buttonH)
		{
		buttonChanged = TRUE;

		buttonX = x;
		buttonY = y;
		buttonW = w;
		buttonH = h;

		selectedColors = newSelectedColors;
		activeColors = newActiveColors;

		if (buttonState != XSBS_DISABLED)
			{
			event.SetEvent();
			}
		}

	lock.Leave();
	}

//
//  Set Button Colors
//

void SPUDisplay::SetButtonColors(DWORD selected, DWORD active)
	{
	this->newSelectedColors = selected;
	this->newActiveColors = active;
	}

//
//  Set Palette
//

void SPUDisplay::SetPalette(int entry, int y, int u, int v)
	{
	palette[entry].y = (BYTE)y;
	palette[entry].u = (BYTE)u;
	palette[entry].v = (BYTE)v;
	}

//
//  Post OSD Bitmap
//

void SPUDisplay::PostOSDBitmap(SPUDisplayBuffer * map, int startTime, DWORD colors, bool forced)
	{
	int osdNext, osdCur;

	lock.Enter();

	osdNext = (osdLast + 1) % NUM_OSD_BITMAPS;
	while (!terminate && osdNext == osdFirst)
		{
		lock.Leave();
		queueEvent.WaitForever();
		lock.Enter();
		osdNext = (osdLast + 1) % NUM_OSD_BITMAPS;
		}

	if (!terminate)
		{
		if (osdLast != osdFirst)
			{
			osdCur = (osdLast + NUM_OSD_BITMAPS - 1) % NUM_OSD_BITMAPS;
			if (pendingQueue[osdCur].stopTime > startTime)
				pendingQueue[osdCur].stopTime = startTime;
			}

		pendingQueue[osdLast].map = map;
		pendingQueue[osdLast].colors = colors;
		pendingQueue[osdLast].startTime = startTime;
		pendingQueue[osdLast].stopTime = 0x7fffffff;
		pendingQueue[osdLast].posted = FALSE;
		pendingQueue[osdLast].forced = forced;
		pendingQueue[osdLast].buttonState = XSBS_DISABLED;
		osdLast = osdNext;
		event.SetEvent();
		}
	else
		delete map;

	lock.Leave();
	}

//
//  Post OSD Stop Time
//

void SPUDisplay::PostOSDStopTime(int stopTime)
	{
	int osdCur;

	lock.Enter();

	if (!terminate && osdLast != osdFirst)
		{
		osdCur = (osdLast + NUM_OSD_BITMAPS - 1) % NUM_OSD_BITMAPS;
		pendingQueue[osdCur].stopTime = stopTime;
		event.SetEvent();
		}

	lock.Leave();
	}

//
//  Get OSD Bitmap
//

SPUDisplayBuffer * SPUDisplay::GetOSDBitmap(void)
	{
	SPUDisplayBuffer * map;

	lock.Enter();

	while (!terminate && osdAvail == 0)
		{
		lock.Leave();
		queueEvent.WaitForever();
		lock.Enter();
		}

	if (!terminate)
		{
		osdAvail--;
		map = availQueue[osdAvail].map;
		}
	else
		map = NULL;

	lock.Leave();

	return map;
	}

//
//  Enable SPU
//

void SPUDisplay::EnableSPU(bool enable)
	{
	int i;

	lock.Enter();

	if (this->enable != enable)
		{
		this->enable = enable;

		if (streaming)
			{
			if (!enable)
				{
				i = osdFirst;

				while (i != osdLast)
					{
					if (!pendingQueue[i].forced)
						display->PostOSDBitmap(pendingQueue[i].map, 0, 0);
					i = (i + 1) % NUM_OSD_BITMAPS;
					}
				}
			else
				{
				event.SetEvent();
				}
			}
		}

	lock.Leave();
	}
