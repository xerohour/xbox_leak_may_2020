/*************************************************************************

Copyright (C) 1996 -- 1998  Microsoft Corporation

Module Name:

	CPUDetect.h

Abstract:

	Several CPU detection utilities. For non-X86, false is always returned.

Author:

    Raych, Wchen

Revision History:

*************************************************************************/

#ifndef __CPUDETECT_H_
#define __CPUDETECT_H_

#ifndef macintosh
Bool g_SupportMMX(void);
#endif
Bool g_SupportKNI(void);
Bool g_SupportSIMD_FP(void);
Bool g_SupportCMOV(void);

#endif // __CPUDETECT_H_
