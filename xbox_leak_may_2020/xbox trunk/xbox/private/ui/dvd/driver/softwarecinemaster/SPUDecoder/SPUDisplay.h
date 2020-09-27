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

#ifndef SPUDISPLAY_H
#define SPUDISPLAY_H

#include "..\video\VideoDisplay.h"
#include "..\video\FrameStore.h"
#include "..\common\TimedFibers.h"

enum XSPUButtonState
	{
	XSBS_DISABLED,
	XSBS_SELECTED,
	XSBS_ACTIVATED
	};


#define NUM_OSD_BITMAPS		16

class SPUDisplay : private TimedFiber
	{
	protected:
		GenericPictureDisplay	*	display;
		TimedFiberMutex				lock;
		TimedFiberEvent				event, queueEvent;

		volatile bool					terminate;
		volatile bool					pause;
		bool								streaming;

		SPUColorEntry					palette[16];
		DWORD								newSelectedColors, newActiveColors;
		DWORD								normalColors, selectedColors, activeColors;
		int								buttonX, buttonY, buttonW, buttonH;
		XSPUButtonState				buttonState;
		bool								buttonChanged;
		bool								osdActive;
		SPUDisplayBufferFactory	*	highQualityOsdFactory, * lowQualityOsdFactory;
		TimingClient					timingClient;
		bool								enable;

		struct OSDPendingQueue
			{
			int						startTime, stopTime;
			bool						posted, forced;
			DWORD						colors;
			SPUDisplayBuffer	*	map;
			XSPUButtonState		buttonState;
			} pendingQueue[NUM_OSD_BITMAPS];

		struct OSDAvailQueue
			{
			SPUDisplayBuffer	*	map;
			} availQueue[NUM_OSD_BITMAPS];

		volatile int					osdFirst, osdLast, osdAvail;

		void FiberRoutine(void);
	public:
		SPUDisplay(GenericPictureDisplay * display, SPUDisplayBufferFactory * highQualityOsdFactory, SPUDisplayBufferFactory * lowQualityOsdFactory);

		virtual ~SPUDisplay(void);

		void BeginStreaming(bool highQuality);

		void StartStreaming(int playbackSpeed);

		void StopStreaming(void);

		void EndStreaming(bool fullReset);


			// Hack for MS Navigator, if data discontinuity for
			// video arrives after the new subpicture data.
		void VideoDisplayBeginStreaming(void);

		void SetButtonState(XSPUButtonState state);

		void SetButtonPosition(int x, int y, int w, int h);

		void SetButtonColors(DWORD selected, DWORD active);

		void SetPalette(int entry, int y, int u, int v);

		void PostOSDBitmap(SPUDisplayBuffer * map, int startTime, DWORD colors, bool forced);

		void PostOSDStopTime(int stopTime);

		SPUDisplayBuffer * GetOSDBitmap(void);

		void EnableSPU(bool enable);
	};


#endif
