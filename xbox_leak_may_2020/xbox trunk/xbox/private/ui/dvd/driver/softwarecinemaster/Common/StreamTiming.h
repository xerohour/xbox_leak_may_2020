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

#ifndef STREAMTIMING_H
#define STREAMTIMING_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"
#include "TimedFibers.h"

class TimingSlave;

class TimingClient
	{
	friend class TimingSlave;
	private:
		TimingClient	*	succ, * pred;
		TimingSlave		*	timer;
		TimedFiberEvent * event;
	public:
		TimingClient(TimingSlave * timer)
			{
			this->timer = timer;
			}

		void Wait(TimedFiberEvent * event, int targetTime, bool precise = FALSE);
	};

class TimingSlave
	{
	private:
		int					previousTime;
		int					startTime;
		int					pauseTime;
		int					playbackSpeed;
		bool					paused;
		bool					initial;
		TimingClient	*	firstClient;
	public:
		void ResetTimer(void)
			{
			paused = TRUE;
			initial = TRUE;
			startTime = 0; pauseTime = 0;
			previousTime = 0;
#if HALF_PLAYBACK_SPEED
			playbackSpeed = 0x00100;
#else
			playbackSpeed = 0x10000;
#endif
			}

		TimingSlave(void)
			{
			ResetTimer();
			firstClient = NULL;
			}

		void SetPlaybackSpeed(int speed)
			{
#if HALF_PLAYBACK_SPEED
			speed /= 256;
#endif
			if (!paused)
				startTime = ScaleLong(GetInternalTime(), 0x10000, speed) - CurrentTime();

			this->playbackSpeed = speed;
			}

		int PlaybackSpeed(void)
			{
			return playbackSpeed;
			}

		void StartTimer(void)
			{
			if (paused)
				{
				startTime = ScaleLong(GetInternalTime(), 0x10000, playbackSpeed) - pauseTime;

				paused = FALSE;
				}
			}

		void StopTimer(void)
			{
			if (!paused)
				{
				pauseTime = CurrentTime();

				paused = TRUE;
				}
			}

		int CurrentTime(void)
			{
			if (paused)
				return pauseTime;
			else
				return ScaleLong(GetInternalTime(), 0x10000, playbackSpeed) - startTime;
			}

		void ClientWait(TimingClient * client, TimedFiberEvent * event, int targetTime, bool precise)
			{
			int time;

			time = targetTime - CurrentTime();

			if (time > 0)
				{
				time = ScaleLong(time, playbackSpeed, 0x10000);

				client->event = event;

				client->succ = firstClient;
				client->pred = NULL;
				if (firstClient) firstClient->pred = client;
				firstClient = client;

				event->Wait(time, precise);

				if (client->pred)
					client->pred->succ = client->succ;
				else
					firstClient = client->succ;
				if (client->succ)
					client->succ->pred = client->pred;
				}
			}

		void UpdateTimer(int time)
			{
			TimingClient	*	client;
			int newStartTime;

			if (paused)
				pauseTime = time;
			else
				{
				newStartTime = ScaleLong(GetInternalTime(), 0x10000, playbackSpeed) - time;

				if (initial || newStartTime > startTime + 500 || newStartTime < startTime - 500)
					{
					startTime = newStartTime;

					client = firstClient;
					while (client)
						{
						client->event->SetEvent();
						client = client->succ;
						}
					}
				else
					startTime = (int)((((__int64)newStartTime) + (__int64)startTime * 31 + 16) / 32);
				}

			initial = FALSE;

			previousTime = time;
			}

		void ForceUpdateTimer(int time)
			{
			TimingClient	*	client;

			if (paused)
				pauseTime = time;
			else
				{
				startTime = ScaleLong(GetInternalTime(), 0x10000, playbackSpeed) - time;

				client = firstClient;
				while (client)
					{
					client->event->SetEvent();
					client = client->succ;
					}
				}
			initial = FALSE;

			previousTime = time;
			}
	};


inline void TimingClient::Wait(TimedFiberEvent * event, int targetTime, bool precise)
	{
	timer->ClientWait(this, event, targetTime, precise);
	}

#endif
