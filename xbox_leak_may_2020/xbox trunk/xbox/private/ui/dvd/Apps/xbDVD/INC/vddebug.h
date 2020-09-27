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
// FILE:      library\common\vddebug.h
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   04.12.96
//
// PURPOSE:
//
// HISTORY:
//
#ifndef _VDDEBUG_
#define _VDDEBUG_

#include "library/common/gnerrors.h"


#if LINUX
#include <stdio.h>
#include "library/common/prelude.h"
#define _export    /* FN: should be corrected in prelude.h.. */
#endif

void __cdecl MDebugPrint(const TCHAR * szFormat, ...);
#define RDP MDebugPrint

#if _DEBUG
	#ifdef _PROPTEST
		extern Error Print(const TCHAR __far * szFormat, ...);
		#define DP Print
	#elif NT_KERNEL
		#define DP DbgPrint("\n"), DbgPrint
	#elif VXD_VERSION
		void _cdecl DebugPrint(char * szFormat, ...);
		#define DP DebugPrint
	#elif LINUX
		void DebugPrint (const TCHAR * szFormat, ...);	// standard prototype for debug output
		#define DP DebugPrint
		#define DPF printf
		void DebugPrintRecord (const TCHAR * szFormat, ...);	// standard prototype for debug output
		#define DPR DebugPrintRecord
		void InitializeDebugRecording (void);
		void GetDebugRecordingParameters (BYTE * & array, int & size);
	#elif _DOS
		#define DP printf   // note: output should really go to stderr instead of stdout
#else
		void FAR __cdecl DebugPrint(const TCHAR __far * szFormat, ...);	// standard prototype for debug output
		#define DP DebugPrint
	#endif
#else
	#ifdef _PROPTEST
		extern Error Print(const TCHAR __far * szFormat, ...);
		#define DP Print
	#elif NT_KERNEL
		inline void __cdecl DebugPrintEmpty(const char __far * szFormat, ...) {}		// empty function (optimized to nothing)
		#define DP while(0) DebugPrintEmpty
	#elif LINUX
		inline void DebugPrintEmpty(const TCHAR * szFormat, ...) {}		// empty function (optimized to nothing)
		#define DP while(0) DebugPrintEmpty
		#define DPF while(0) DebugPrintEmpty
		#define DPR while(0) DebugPrintEmpty
		inline void InitializeDebugRecording (void) {}
		inline void GetDebugRecordingParameters (BYTE * & array, int & size) {array=NULL; size=0;}
#else
		inline void __cdecl DebugPrintEmpty(const TCHAR __far * szFormat, ...) {}		// empty function (optimized to nothing)
		#define DP while(0) DebugPrintEmpty
	#endif
#endif

#ifdef _DEBUG

#if NT_KERNEL
#define BREAKPOINT		DbgBreakPoint();
#else
#define BREAKPOINT		__asm int 3
#endif

#else

#define BREAKPOINT

#endif

#endif
