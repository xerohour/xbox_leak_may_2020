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
#include "nvPM.h"

/*
#undef NV_PRAMDAC_TEST_CONTROL
#undef NV_PRAMDAC_CHECKSUM
#include "..\..\resman\kernel\inc\nv_ref.h"
#include "..\..\resman\kernel\inc\nv10\nv10_ref.h"
*/
#include "..\..\..\common\nv10\inc\nv10ppm.h"
#include <stdio.h>

/*
 * build specific aliases (aliaii?)
 */
#ifdef NVDD32

#define pmAllocMemory       AllocIPM
#define pmFreeMemory        FreeIPM

#define writeMethod140()                                                    \
    nvglSetObject(NV_DD_SPARE, D3D_RENDER_SOLID_RECTANGLE);                 \
    nvPushData (0,0x0004e140);                                              \
    nvPushData (1,0x00000001);                                              \
    nvPusherAdjust (2);                                                     \
    pDriverData->dDrawSpareSubchannelObject = D3D_RENDER_SOLID_RECTANGLE;

#define flush()  getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);

#elif defined(IS_OPENGL)

#define pmAllocMemory       malloc
#define pmFreeMemory        free

#define writeMethod140()                    \
{                                           \
    __GL_SETUP();                           \
    __GLNV_SETUP();                         \
    NV_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();  \
    nvCurrent[0].u = 0x0004e140;            \
    nvCurrent[1].u = 0x00000001;            \
    nvCurrent += 2;                         \
    NV_GET_SKID_SPACE_IF_NEEDED();          \
}

#define flush()                             \
{                                           \
    __GL_SETUP();                           \
    if (gc) (*gc->procs.finish)(gc);        \
}

#define DPF(_a, _b, _c, _d)

// define TRACE_QUAKE in nvTrace.h
extern int InQuake;
int reset = 1;

#else

#define pmAllocMemory       malloc
#define pmFreeMemory        free

#define writeMethod140()                    _error_
#define flush()                             _error_

#endif

/*
 * macros
 */
#define rdtsc  __asm _emit 0x0f __asm _emit 0x31

/*
 * globals
 */
DWORD g_dwPMTrigger      = PM_REG_PMTRIGGER_DISABLED;
DWORD g_dwPMTriggerCount = 0;
DWORD g_dwPMTriggerValue = 0;

HANDLE         hPMFile              = 0;
PM_PGM_HEADER *pbPMProgramSetup     = NULL;
PM_PGM_HEADER *pbPMProgramStart     = NULL;
PM_PGM_HEADER *pbPMProgramSample    = NULL;
BYTE          *pbPMEventBuffer      = NULL;
DWORD          dwPMEventBufferIndex = 0;
DWORD          dwPMHWLinearBase     = 0;
DWORD          dwPMHWArch           = 0;
DWORD          dwPMFirstTrigger     = 1;
DWORD          dwPMFlags            = 0;

/*
 * pmFlush
 *
 * commit capture buffer to disk
 */
void pmFlush
(
    void
)
{
#ifdef WINNT
#else
    /*
     * commit to disk
     */
    if (dwPMEventBufferIndex)
    {
        DWORD dw;
        WriteFile (hPMFile,pbPMEventBuffer,dwPMEventBufferIndex * sizeof(PM_LOG_EVENT),&dw,NULL);
        FlushFileBuffers (hPMFile);

        /*
         * reset
         */
        dwPMEventBufferIndex = 0;
    }
#endif
}

/*
 * pmLogEvent
 *
 * writes a single event into the log
 */
void pmLogEvent
(
    PM_LOG_EVENT *pEvent
)
{
#ifdef WINNT
#else
    /*
     * store it
     */
    ((PM_LOG_EVENT*)pbPMEventBuffer)[dwPMEventBufferIndex++] = *pEvent;

    /*
     * flush?
     */
    if (dwPMEventBufferIndex >= PM_LOG_EVENT_BUFFER_SIZE)
    {
        pmFlush();
    }
#endif
}

/*
 * pmExecuteProgram
 *
 * executes a program
 */
void pmExecuteProgram
(
    PM_PGM_HEADER *pPgm
)
{
#ifdef WINNT
#else
    /*
     * for all entries
     */
    DWORD dwIndex = 0;
    DWORD dwCount = pPgm->dwEntryCount;
    for (; dwCount; dwIndex++, dwCount--)
    {
        PM_PGM_ENTRY *pEntry = pPgm->Entry + dwIndex;

        if (pEntry->dwFlags & PM_PGM_FLAG_READ)
        {
            PM_LOG_EVENT event;

            /*
             * read operation
             */
            switch (pEntry->dwFlags & PM_PGM_MASK_SIZE)
            {
                case PM_PGM_VAL_SIZE_8:
                {
                    event.dwValueHi = 0;
                    event.dwValueLo = (DWORD)*(BYTE*)(dwPMHWLinearBase + pEntry->dwOffset);
                    DPF ("---pmread8(%08x) -> %02x%08x",pEntry->dwOffset,event.dwValueHi,event.dwValueLo);
                    break;
                }
                case PM_PGM_VAL_SIZE_16:
                {
                    event.dwValueHi = 0;
                    event.dwValueLo = (DWORD)*(WORD*)(dwPMHWLinearBase + pEntry->dwOffset);
                    DPF ("---pmread16(%08x) -> %02x%08x",pEntry->dwOffset,event.dwValueHi,event.dwValueLo);
                    break;
                }
                case PM_PGM_VAL_SIZE_32:
                {
                    event.dwValueHi = 0;
                    event.dwValueLo = (DWORD)*(DWORD*)(dwPMHWLinearBase + pEntry->dwOffset);
                    DPF ("---pmread32(%08x) -> %02x%08x",pEntry->dwOffset,event.dwValueHi,event.dwValueLo);
                    break;
                }
                case PM_PGM_VAL_SIZE_40:
                {
                    DWORD hi;
                    event.dwValueHi = (DWORD)*(BYTE*)(dwPMHWLinearBase + pEntry->dwOffset + 4);
                    do
                    {
                        hi = event.dwValueHi;
                        event.dwValueLo = (DWORD)*(DWORD*)(dwPMHWLinearBase + pEntry->dwOffset);
                        event.dwValueHi = (DWORD)*(BYTE*) (dwPMHWLinearBase + pEntry->dwOffset + 4);
                    } while (hi != event.dwValueHi);
                    DPF ("---pmread40(%08x) -> %02x%08x",pEntry->dwOffset,event.dwValueHi,event.dwValueLo);
                    break;
                }
            }

            /*
             * log it
             */
            pmLogEvent (&event);
        }
        else
        {
            /*
             * write operation
             */
            switch (pEntry->dwFlags & PM_PGM_MASK_SIZE)
            {
                case PM_PGM_VAL_SIZE_8:
                {
                    DPF ("---pmwrite8(%08x) -> %02x%08x",pEntry->dwOffset,pEntry->dwValueHi,pEntry->dwValueLo);
                    *(BYTE*)(dwPMHWLinearBase + pEntry->dwOffset) = (BYTE)pEntry->dwValueLo;
                    break;
                }
                case PM_PGM_VAL_SIZE_16:
                {
                    DPF ("---pmwrite16(%08x) -> %02x%08x",pEntry->dwOffset,pEntry->dwValueHi,pEntry->dwValueLo);
                    *(WORD*)(dwPMHWLinearBase + pEntry->dwOffset) = (WORD)pEntry->dwValueLo;
                    break;
                }
                case PM_PGM_VAL_SIZE_32:
                {
                    DPF ("---pmwrite32(%08x) -> %02x%08x",pEntry->dwOffset,pEntry->dwValueHi,pEntry->dwValueLo);
                    *(DWORD*)(dwPMHWLinearBase + pEntry->dwOffset) = (DWORD)pEntry->dwValueLo;
                    break;
                }
                case PM_PGM_VAL_SIZE_40:
                {
                    DPF ("---pmwrite40(%08x) -> %02x%08x",pEntry->dwOffset,pEntry->dwValueHi,pEntry->dwValueLo);
                    *(BYTE*) (dwPMHWLinearBase + pEntry->dwOffset + 4) = (BYTE) pEntry->dwValueHi;
                    *(DWORD*)(dwPMHWLinearBase + pEntry->dwOffset)     = (DWORD)pEntry->dwValueLo;
                    break;
                }
            }
        }
    }
#endif
}

/*
 * pmConnect
 *
 * connect to PM host (Locutus II)
 */
BOOL pmConnect
(
#if defined(IS_OPENGL)
    HDC hEscapeDC,
#else
    DWORD dwHeadNumber,
#endif
    DWORD dwHWLinearBase,
    DWORD dwArch
)
{
#ifdef WINNT
    return FALSE;
#else
    /*
     * open registry
     */
    DWORD dwSize  = sizeof(DWORD);
    DWORD dwType  = REG_DWORD;
    DWORD dwValue;
    HKEY  hKey;
    HKEY  hPrimaryKey;
    char  szLocalRegPath[128];
    char  szD3DSubKey[256];
    char  szLogFileName[256] = "\\lc2.tmp";
    PM_LOG_HEADER hdr;

    /*
     * save constants
     */
    dwPMHWArch       = dwArch;
    dwPMHWLinearBase = dwHWLinearBase;

    /*
     * get the registry path for this particular device
     * (of the many devices possible in a multi-mon configuration)
     */
#if defined(IS_OPENGL)
    ExtEscape (hEscapeDC,NV_ESC_GET_LOCAL_REGISTRY_PATH,0,NULL,128,szLocalRegPath);
#else
    MyExtEscape (dwHeadNumber,NV_ESC_GET_LOCAL_REGISTRY_PATH,0,NULL,128,szLocalRegPath);
#endif

    /*
     * the first DWORD of the value returned is the primary key (e.g. HKEY_LOCAL_MACHINE)
     */
    hPrimaryKey = (HKEY)(*(DWORD*)szLocalRegPath);

    /*
     * the NV subkey for this device starts after the first four bytes.
     * concatenate this with driver's subtree's name
     */
    sprintf (szD3DSubKey,"%s\\%s",szLocalRegPath + 4,PM_REGISTRY_SUBKEY);

    /*
     * finally open the key
     */
    if (RegOpenKeyEx(hPrimaryKey,szD3DSubKey,0,KEY_ALL_ACCESS,&hKey) == ERROR_SUCCESS)
    {
        /*
         * read master switch
         */
        DWORD dwConnectCount;
        if (!RegQueryValueEx(hKey,
                             PM_REG_CONNECTCOUNT,
                             NULL,
                             &dwType,
                             (LPBYTE)&dwConnectCount,
                             &dwSize) == ERROR_SUCCESS)
        {
            dwConnectCount = 0;
        }

        //
        // read type of trigger
        //
        if (RegQueryValueEx(hKey,
                            PM_REG_PMTRIGGER,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue > PM_REG_PMTRIGGER_MAX) dwValue = PM_REG_PMTRIGGER_DISABLED;
            g_dwPMTrigger = dwValue;
        }

        /*
         * if enabled, read other information
         */
        if (dwConnectCount && (g_dwPMTrigger != PM_REG_PMTRIGGER_DISABLED))
        {
            /*
             * decrement master switch
             */
            dwConnectCount--;
            RegSetValueEx(hKey,
                          PM_REG_CONNECTCOUNT,
                          (unsigned long)NULL,
                          dwType,
                          (LPBYTE)&dwConnectCount,
                          dwSize);

            /*
             * read log file name
             */
            dwType = REG_SZ;
            dwSize = sizeof(szLocalRegPath);
            if (RegQueryValueEx(hKey,
                                PM_REG_LOGFILENAME,
                                NULL,
                                &dwType,
                                (LPBYTE)szLogFileName,
                                &dwSize) != ERROR_SUCCESS)
            {
                pmDisconnect();
                g_dwPMTrigger = 0;
                return FALSE;
            }

            /*
             * read PM trigger count
             */
            dwType = REG_DWORD;
            dwSize = sizeof(DWORD);
            if (RegQueryValueEx(hKey,
                                PM_REG_TRIGGERCOUNT,
                                NULL,
                                &dwType,
                                (LPBYTE)&g_dwPMTriggerCount,
                                &dwSize) != ERROR_SUCCESS)
            {
                pmDisconnect();
                g_dwPMTrigger = 0;
                return FALSE;
            }
            g_dwPMTriggerValue = g_dwPMTriggerCount;

            /*
             * read PM flags
             */
            // no error checking on this for backwards compatability
            dwType = REG_DWORD;
            dwSize = sizeof(DWORD);
            RegQueryValueEx(hKey,
                                PM_REG_FLAGS,
                                NULL,
                                &dwType,
                                (LPBYTE)&dwPMFlags,
                                &dwSize);

            /*
             * read programs
             */
            dwType = REG_BINARY;
            if (RegQueryValueEx(hKey,
                                PM_REG_PROGRAM_SETUP,
                                NULL,
                                &dwType,
                                NULL,
                                &dwSize) == ERROR_SUCCESS)
            {
                pbPMProgramSetup = (PM_PGM_HEADER*)pmAllocMemory(dwSize);
                if (RegQueryValueEx(hKey,
                                    PM_REG_PROGRAM_SETUP,
                                    NULL,
                                    &dwType,
                                    (BYTE*)pbPMProgramSetup,
                                    &dwSize) != ERROR_SUCCESS)
                {
                    pmDisconnect();
                    g_dwPMTrigger = 0;
                    return FALSE;
                }
            }
            else
            {
                pmDisconnect();
                g_dwPMTrigger = 0;
                return FALSE;
            }

            if (RegQueryValueEx(hKey,
                                PM_REG_PROGRAM_START,
                                NULL,
                                &dwType,
                                NULL,
                                &dwSize) == ERROR_SUCCESS)
            {
                pbPMProgramStart = (PM_PGM_HEADER*)pmAllocMemory(dwSize);
                if (RegQueryValueEx(hKey,
                                    PM_REG_PROGRAM_START,
                                    NULL,
                                    &dwType,
                                    (BYTE*)pbPMProgramStart,
                                    &dwSize) != ERROR_SUCCESS)
                {
                    pmDisconnect();
                    g_dwPMTrigger = 0;
                    return FALSE;
                }
            }
            else
            {
                pmDisconnect();
                g_dwPMTrigger = 0;
                return FALSE;
            }

            if (RegQueryValueEx(hKey,
                                PM_REG_PROGRAM_SAMPLE,
                                NULL,
                                &dwType,
                                NULL,
                                &dwSize) == ERROR_SUCCESS)
            {
                pbPMProgramSample = (PM_PGM_HEADER*)pmAllocMemory(dwSize);
                if (RegQueryValueEx(hKey,
                                    PM_REG_PROGRAM_SAMPLE,
                                    NULL,
                                    &dwType,
                                    (BYTE*)pbPMProgramSample,
                                    &dwSize) != ERROR_SUCCESS)
                {
                    pmDisconnect();
                    g_dwPMTrigger = 0;
                    return FALSE;
                }
            }
            else
            {
                pmDisconnect();
                g_dwPMTrigger = 0;
                return FALSE;
            }
        }
        else
        {
            pmDisconnect();
            g_dwPMTrigger = 0;
            return FALSE;
        }

        RegCloseKey(hKey);
    }
    else
    {
        pmDisconnect();
        g_dwPMTrigger = 0;
        return FALSE;
    }

    /*
     * enable PM
     */
    switch (dwPMHWArch)
    {
        case 0x10:        
        case 0x20:
        {
            //
            // enable PM_TRIGGER bit for nv10/nv20
            //
            #define NV_PGRAPH_DEBUG_3                                0x0040008C /* RW-4R */
            #define NV_PGRAPH_DEBUG_3_PM_TRIGGER_ENABLED             0x00000001 /* RW--V */
            #define NV_PGRAPH_DEBUG_3_PM_TRIGGER                          15:15 /* RWIVF */

            DWORD dwAddr = dwPMHWLinearBase + NV_PGRAPH_DEBUG_3;
            DWORD dwBits = *(DWORD*)dwAddr;
            dwBits |= NV_PGRAPH_DEBUG_3_PM_TRIGGER_ENABLED << (0 ? NV_PGRAPH_DEBUG_3_PM_TRIGGER);
            *(DWORD*)dwAddr = dwBits;
            break;
        }                
        default: // HW not supported
        {
            pmDisconnect();
            return FALSE;
        }
    }

    /*
     * open log file
     */
    hPMFile = CreateFile(szLogFileName,GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
    if (hPMFile == INVALID_HANDLE_VALUE)
    {
        hPMFile = NULL;
        pmDisconnect();
        return FALSE;
    }

    /*
     * write header
     */
    strcpy (hdr.szID,PM_LOG_ID);
    hdr.dwVersion = PM_LOG_VERSION;
    WriteFile (hPMFile,&hdr,sizeof(hdr) - sizeof(hdr.Event),&dwValue,NULL);

    /*
     * create event buffer
     */
    pbPMEventBuffer = (BYTE*)pmAllocMemory(sizeof(PM_LOG_EVENT) * PM_LOG_EVENT_BUFFER_SIZE);
    if (!pbPMEventBuffer)
    {
        pmDisconnect();
        return FALSE;
    }
    dwPMEventBufferIndex = 0;

    /*
     * setup PM
     */
    pmExecuteProgram (pbPMProgramSetup);

    /*
     * start 1st experiment
     */
#ifdef NVDD32
    pmExecuteProgram (pbPMProgramStart);
    writeMethod140();
    dwPMFirstTrigger = 0;
#endif

    /*
     * done
     */
    return TRUE;
#endif
}

/*
 * pmDisconnect
 *
 * disconnect from PM host
 */
BOOL pmDisconnect
(
    void
)
{
#ifdef WINNT
    return FALSE;
#else
    /*
     * general clean up
     */
    g_dwPMTrigger      = PM_REG_PMTRIGGER_DISABLED;
    g_dwPMTriggerCount = 0;
    g_dwPMTriggerValue = 0;

    /*
     * close log file
     */
    if (hPMFile)
    {
        pmFlush();
        CloseHandle (hPMFile);
        hPMFile = NULL;
    }

    /*
     * free programs
     */
    if (pbPMProgramSetup)
    {
        pmFreeMemory (pbPMProgramSetup);
        pbPMProgramSetup = NULL;
    }
    if (pbPMProgramStart)
    {
        pmFreeMemory (pbPMProgramStart);
        pbPMProgramStart = NULL;
    }
    if (pbPMProgramSample)
    {
        pmFreeMemory (pbPMProgramSample);
        pbPMProgramSample = NULL;
    }

    /*
     * free event buffer
     */
    if (pbPMEventBuffer)
    {
        pmFreeMemory (pbPMEventBuffer);
        pbPMEventBuffer = NULL;
    }

    /*
     * done
     */
    return TRUE;
#endif
}

/*
 * pmSignal
 *
 * signals the end of an experiment
 *  wait for HW flush to capture data.
 *  logs results to log file.
 */
BOOL pmSignal
(
    void
)
{
#ifdef WINNT
    return FALSE;
#else
    PM_LOG_EVENT event;
    BOOL bTimedOut;

    /*
     * Return if this call to pmSignal() is just starting the trigger
     */
    if (dwPMFirstTrigger) {
        pmExecuteProgram (pbPMProgramStart);
        writeMethod140();
        dwPMFirstTrigger = 0;
        return TRUE;
    }

    /*
     * signal end-of-experiment (fe2pm_nop)
     */
    writeMethod140();
    flush();

    /*
     * wait for PM idle
     */
    bTimedOut = FALSE;
    
#if defined(IS_OPENGL)
    if (!(dwPMFlags & PM_FLAGS_IN_QUAKE3) || InQuake) 
    {
        if (reset && (dwPMFlags & PM_FLAGS_IN_QUAKE3))
        {
            reset = 0;
            pmExecuteProgram (pbPMProgramStart);
        }
        else
        {
#endif
            if (!(dwPMFlags & PM_FLAGS_NO_WAIT))
            {
                switch (dwPMHWArch)
                {
                    case 0x10:
                    case 0x20:
                    {
                        DWORD dwAddr    = dwPMHWLinearBase + NV_PPM_CONTROL;
                        DWORD dwBits;
                        DWORD dwTimeout = GetTickCount() + 2000; // 2s timeout window
                        do
                        {
                            bTimedOut   = GetTickCount() >= dwTimeout;
                            dwBits      = *(volatile DWORD*)dwAddr;
                            dwBits    >>= 0 ? NV_PPM_CONTROL_NV_STATE;
                            dwBits     &= (1 << ((1 ? NV_PPM_CONTROL_NV_STATE) - (0 ? NV_PPM_CONTROL_NV_STATE) + 1)) - 1;
                        } while ((dwBits != NV_PPM_CONTROL_STATE_IDLE)
                              && !bTimedOut);
                    }
                }
            }

            /*
             * if PM timed out, we do not log anything but we do reset the PM engine
             */

            if (!bTimedOut)
            {
                /*
                 * log current time
                 */
                __asm
                {
                    rdtsc
                    mov [event.dwValueLo],eax
                    mov [event.dwValueHi],edx
                }
                pmLogEvent (&event);

                /*
                 * sample PM and log events
                 */
                pmExecuteProgram (pbPMProgramSample);
            }
            else
            {
                /*
                 * reset engine by reprogramming state
                 */
                pmExecuteProgram (pbPMProgramSetup);
            }
#if defined(IS_OPENGL)
        }
    }
    else if (dwPMFlags & PM_FLAGS_IN_QUAKE3)
    {
        reset = 1;        
    }
#endif

    /*
     * start next experiment
     */
    if (!(dwPMFlags & PM_FLAGS_NO_RESET))
        pmExecuteProgram (pbPMProgramStart);

    writeMethod140();

    /*
     * done
     */
    return TRUE;
#endif
}
