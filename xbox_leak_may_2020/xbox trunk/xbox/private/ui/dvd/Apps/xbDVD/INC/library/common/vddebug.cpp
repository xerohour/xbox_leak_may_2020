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

//
// FILE:      library\common\vddebug.cpp
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   04.12.96
//
// PURPOSE:
//
// HISTORY:
//
#include <xtl.h>

#include "vddebug.h"

#if LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#endif



#if   LINUX

void DebugPrint(const TCHAR * szFormat, ...)
	{
	TCHAR buff[256];
	int fd;

  	sprintf(buff,szFormat,(&szFormat)[1], (&szFormat)[2], (&szFormat)[3], (&szFormat)[4]);
  	strcat(buff, "\n");
	/* FN: my hack to get debug output to something sort-of fast - the ttys */
	/* are rather low-overhead, me thinks.. */
	fd = open("/tmp/CINELOG", O_CREAT | O_RDWR | O_APPEND, S_IRWXU);
	write(fd, buff, strlen(buff));
	close(fd);
	}



#elif !VXD_VERSION

// Print debug string to DebugWin. Formatted output is possible.
void __cdecl DebugPrint(const TCHAR __far * szFormat, ...)
	{
	TCHAR buff[256];

  	wvsprintf(buff,szFormat,(char __far *)(&szFormat+1));
  	_fstrcat(buff, __TEXT("\r\n"));
  	::OutputDebugString(buff);
	}

void __cdecl MDebugPrint(const TCHAR * szFormat, ...)
	{
	TCHAR buff[256];

  	::wvsprintf(buff,szFormat,(char __far *)(&szFormat+1));
  	_fstrcat(buff, __TEXT("\r\n"));
  	::OutputDebugString(buff);
	}



#else

void _cdecl DebugPrint(char * szFormat, ...)
	{
	static char buff[256];

	_Sprintf(buff, szFormat, (&szFormat)[1], (&szFormat)[2], (&szFormat)[3], (&szFormat)[4] );
	Out_Debug_String(buff);
	}

#endif

