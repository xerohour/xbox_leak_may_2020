 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
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
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
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
#ifndef _nvPM_h
#define _nvPM_h

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * constants
 */

// registry related
#define PM_REGISTRY_SUBKEY          "PerformanceMonitor"

#define PM_REG_LOGFILENAME          "logFileName"
#define PM_REG_CONNECTCOUNT         "connectCount"
#define PM_REG_PMTRIGGER            "trigger"
#define PM_REG_TRIGGERCOUNT         "triggerCount"
#define PM_REG_FLAGS                "flags"
#define PM_REG_PROGRAM_SETUP        "programSetup"
#define PM_REG_PROGRAM_START        "programStart"
#define PM_REG_PROGRAM_SAMPLE       "programSample"

#define PM_REG_PMTRIGGER_DISABLED   0                   // disabled
#define PM_REG_PMTRIGGER_ENDSCENE   1                   // emit trigger at end of frame
#define PM_REG_PMTRIGGER_1MS        2                   // emit trigger at 1ms intervals (not exact)
#define PM_REG_PMTRIGGER_RESV00     3
#define PM_REG_PMTRIGGER_CUSTOM0    4
#define PM_REG_PMTRIGGER_CUSTOM1    5
#define PM_REG_PMTRIGGER_CUSTOM2    6
#define PM_REG_PMTRIGGER_CUSTOM3    7
#define PM_REG_PMTRIGGER_MAX        7

#define PM_LOG_ID                   "events \0"
#define PM_PGM_ID                   "program\0"

#define PM_LOG_VERSION              0x00000001
#define PM_PGM_VERSION              0x00000001

#define PM_PGM_MASK_SIZE            0x0000000f
#define PM_PGM_VAL_SIZE_8               0x00000000
#define PM_PGM_VAL_SIZE_16              0x00000001
#define PM_PGM_VAL_SIZE_32              0x00000002
#define PM_PGM_VAL_SIZE_40              0x00000003
#define PM_PGM_FLAG_READ            0x80000000

#define PM_LOG_EVENT_BUFFER_SIZE    (65536 / 8)

// flags bits
#define PM_FLAGS_NO_WAIT 0x1
#define PM_FLAGS_NO_RESET 0x2
#define PM_FLAGS_IN_QUAKE3 0x4

/*
 * macros
 */
#define PM_SIGNAL(match)            if (g_dwPMTrigger == (match)) { if (!(--g_dwPMTriggerValue)) { pmSignal(); g_dwPMTriggerValue = g_dwPMTriggerCount; } }

/*
 * structures
 */
#pragma pack(push,1)

//////
// log event
typedef struct
{
    DWORD dwValueLo;
    DWORD dwValueHi;
} PM_LOG_EVENT;

// log header
typedef struct
{
    char         szID[8];
    DWORD        dwVersion;
    PM_LOG_EVENT Event[1];
} PM_LOG_HEADER;

//////
// program entry
typedef struct
{
    DWORD dwFlags;
    DWORD dwOffset;
    DWORD dwValueLo;
    DWORD dwValueHi;
} PM_PGM_ENTRY;

// program header
typedef struct
{
    char         szID[8];
    DWORD        dwVersion;
    DWORD        dwEntryCount;
    PM_PGM_ENTRY Entry[1];
} PM_PGM_HEADER;

#pragma pack(pop)

/*
 * exported globals
 */
extern DWORD g_dwPMTrigger;
extern DWORD g_dwPMTriggerCount;
extern DWORD g_dwPMTriggerValue;

/*
 * public functions
 */
#if defined(IS_OPENGL)
BOOL pmConnect    (HDC hEscapeDC,DWORD dwHWLinearBase,DWORD dwArch);    // arch: nv4 -> 0x04, nv10 -> 0x10
#else
BOOL pmConnect    (DWORD dwHeadNumber,DWORD dwHWLinearBase,DWORD dwArch);    // arch: nv4 -> 0x04, nv10 -> 0x10
#endif
BOOL pmDisconnect (void);
BOOL pmSignal     (void);

#ifdef __cplusplus
}
#endif

#endif // _nvPM_h
