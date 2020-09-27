/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		in_core.h
 *  Content:	Instrumentation for voice core.
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 02/17/2000	rodtoll	Created it
 ***************************************************************************/
#ifndef __IN_CORE_H
#define __IN_CORE_H

#ifdef DBG
#include <stdio.h>
#define NUM_CORE_SECTIONS		 14

// Defines for modules 
#define RRI_DEBUGOUTPUT_LEVEL									0x0001
#define RECORD_SWITCH_DEBUG_LEVEL								0x0002
#define PLAYBACK_SWITCH_DEBUG_LEVEL								0x0004
#define PWI_DEBUGOUTPUT_LEVEL									0x0008
#define DPVF_SEND_DEBUG_LEVEL		                            0x0020

#define DPVF_PLAYERMANAGE_DEBUG_LEVEL							0x0040
#define DPVF_GLITCH_DEBUG_LEVEL									0x0080
#define DPVF_CLIENT_SEQNUM_DEBUG_LEVEL							0x0100
#define DPVF_HOSTMIGRATE_DEBUG_LEVEL                             0x0200
#define DPVF_COMPRESSION_DEBUG_LEVEL								0x0400
#define DPVF_BUFFERDESC_DEBUG_LEVEL								0x0800
#define DPVF_SOUNDTARGET_DEBUG_LEVEL								0x1000



#ifndef DPVF_ERRORLEVEL
#define DPVF_ERRORLEVEL                                          0x2000
#endif
#ifndef DPVF_ENTRYLEVEL
#define DPVF_ENTRYLEVEL                                          0x4000
#endif
#ifndef DPVF_INFOLEVEL
#define DPVF_INFOLEVEL                                           0x8000  // share same level
#endif
#ifndef DPVF_APIPARAM
#define DPVF_APIPARAM                                            0x8000
#endif
#ifndef DPVF_STRUCTUREDUMP
#define DPVF_STRUCTUREDUMP                                       0x8000
#endif
#ifndef DPVF_WARNINGLEVEL
#define DPVF_WARNINGLEVEL                                       0x10000
#endif
#ifndef DPVF_RECORDLEVEL
#define DPVF_RECORDLEVEL                                        0x20000
#endif

#ifndef DPVF_SPAMLEVEL
#define DPVF_SPAMLEVEL                                          0x40000
#endif

#ifndef DPVF_QUEUELEVEL
#define DPVF_QUEUELEVEL                                         0x80000
#endif

#define DPVL_ERRORS_ONLY                                         (DPVF_ERRORLEVEL | DPVF_WARNINGLEVEL)
#define DPVL_INFO                                                (DPVF_ERRORLEVEL | DPVF_WARNINGLEVEL | \
                                                                 DPVF_INFOLEVEL)

#define DPVL_SPAM                                                (DPVL_INFO | DPVF_ENTRYLEVEL | DPVF_INFOLEVEL) 


typedef struct _DVDEBUGSTACK {

    UCHAR ModName[256];
    PVOID This;

} DVDEBUGSTACK;

typedef struct _DVDEBUGDATA {

    DVDEBUGSTACK Stack[20];
    ULONG StackLevel;

} DVDEBUGDATA;

extern ULONG g_DVDebugLevel;
extern DVDEBUGDATA g_DVDbgData;
extern UCHAR g_ModName[256];
extern PVOID g_This;

#endif

#if DBG
extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}

#ifndef DPF_FNAME
#define DPF_FNAME DPF_MODNAME
#endif

void
DPVF
(
    DWORD                   dwLevel,
    LPCSTR                  pszFormat,
    ...
);

#define DPVF_ENTER()  {\
    if (g_DVDbgData.StackLevel > 3) {\
        g_DVDbgData.StackLevel = 0;\
    }\
    memcpy(g_DVDbgData.Stack[0].ModName,g_ModName,sizeof(g_ModName));\
    memcpy(g_ModName,DPF_MODNAME,sizeof(g_ModName));g_This = this;\
    }


#define DPVF_EXIT()  {\
    memcpy(g_ModName,g_DVDbgData.Stack[0].ModName,sizeof(g_ModName));\
    }


#else // DEBUG

#pragma warning(disable:4002)

#define DPVF()
#define DPVF_ENTER()
#define DPVF_EXIT()
#endif // DEBUG


#endif // _IN_CORE_
