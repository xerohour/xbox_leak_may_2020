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

#include <xtl.h>
#include "PerformanceMonitor.h"

#pragma data_seg(".sdata")

PerformanceMonitor	perfmon = {0};
char						perfmonMsgBuffer[PERFMON_MSGBUFFER_SIZE] = {0};

#pragma data_seg()

void PMMSG(char * msg)
	{
	int i = perfmon.msgBufferWritePos;

//	OutputDebugString(msg);
	while (*msg)
		{
		perfmon.xMsgBuffer[i] = *msg++;
		i = (i + 1) & PERFMON_MSGBUFFER_MASK;
		}

	perfmon.xMsgBuffer[i] = 0;
	perfmon.msgBufferWritePos = i;
	}

void __cdecl PMMSGX(char * format, ...)
	{
	char buffer[256];

  	wvsprintf(buffer,format,(char *)(&format+1));
	PMMSG(buffer);
	}
