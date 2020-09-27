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


// FILE:      library\lowlevel\timer.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   17.03.95
//
// PURPOSE: The timer class.
//
// HISTORY:

#ifndef TIMER_H
#define TIMER_H

#include "..\common\prelude.h"

class TimerClass {
	private:
		static int timerClassInited;
		static int nanoDelay;
	public:
		TimerClass (void);
		virtual void WaitMicroSecs (DWORD micros);
		virtual void WaitMilliSecs (DWORD millis);
		virtual DWORD GetMilliTicks (void);
		virtual DWORD GetMicroTicks (void);
	};


#if VXD_VERSION

class VxDTimerClass : public TimerClass
	{
	private:
		DWORD	vxdTimerHandle;

	public:
		VxDTimerClass(DWORD vxdTimerHandle) {this->vxdTimerHandle = vxdTimerHandle;}

		void WaitMilliSecs (DWORD millis);
	};

#define Timer (*globalTimer)
extern TimerClass * globalTimer;

#elif NT_KERNEL

#define Timer (*globalTimer)
extern TimerClass * globalTimer;

#else

extern TimerClass Timer;

#endif // VXD_VERSION



#endif
