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

#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

#define PERFMON_MSGBUFFER_SIZE	4096
#define PERFMON_MSGBUFFER_MASK	(PERFMON_MSGBUFFER_SIZE - 1)

	#define PMON_READ_KEY (0x55AA1234)


struct PerformanceMonitor
	{
	int		systemTime;
	int		threadTime[16];
	int		framesDropped;
	int		framesDecoded;
	int		framesDisplayed;
	int		framesDegraded;
	int		framesWeave, framesBob;
	int		avgFrameSkew;
	int		maxFrameSkew;
	int		avgFrameDecodingTime[3];
	int		maxFrameDecodingTime[3];
	int		msgBufferWritePos;

	char		*	msgBuffer;

	int		videoBitBufferSize;
	int		videoBitBufferFill;
	int		audioBitBufferSize;
	int		audioBitBufferFill;
	int		audioSampleBufferSize;
	int		audioSampleBufferFill;
	__int64	videoBitsProcessed;

	char		xMsgBuffer[PERFMON_MSGBUFFER_SIZE];
	};

extern PerformanceMonitor	perfmon;
extern char						perfmonMsgBuffer[PERFMON_MSGBUFFER_SIZE];

void PMMSG(char * msg);

void __cdecl PMMSGX(char * format, ...);

#endif
