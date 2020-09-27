 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/
#ifndef _COMPILE_CONTROL_H_
#define _COMPILE_CONTROL_H_

// Activate this block to get an idea of what #defs are set if it isn't clear
#if 0
#if defined(_WIN32)
#pragma message("Got _WIN32")
#endif
#if defined(WIN32)
#pragma message("Got WIN32")
#endif
#if defined(WINNT)
#pragma message("Got WINNT")
#else
#pragma message("No WINNT")
#endif
#if defined(_WIN32_WINNT)
#pragma message("Got WIN32 NT version")
#endif
#if (_WIN32_WINNT < 0x0500)
#pragma message("NT VER <500")
#else
#pragma message("NT VER >=500")
#endif
#endif
		
		
#if (defined(WINNT) && (_WIN32_WINNT < 0x0500))
#define IS_WINNT4 1
#else
#undef IS_WINNT4
#endif

#if (defined(WINNT) && (_WIN32_WINNT >= 0x0500))
#define IS_WINNT5 1
#else
#undef IS_WINNT5
#endif

#if (defined (WIN32) && !defined(WINNT))
#define IS_WIN9X 1
#else
#undef IS_WIN9X
#endif

#if 0
#if (IS_WIN9X)
#pragma message("Compiling for OS = WIN9X")
#elif IS_WINNT4
#pragma message("Compiling for OS = WINNT4")
#elif IS_WINNT5
#pragma message("Compiling for OS = WINNT5")
#else
#error Unrecognized OS!
#endif
#endif

#if !(IS_WIN9X || IS_WINNT4 || IS_WINNT5)
#error Unrecognized OS!
#endif

#endif _COMPILE_CONTROL_H_
