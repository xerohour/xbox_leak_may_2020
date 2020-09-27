#ifdef NV4
/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4PROF.C                                                         *
*   NV4 Specific profiling routines.                                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 05/28/98 - NV4 development.             *
*                                                                           *
\***************************************************************************/
#include <math.h>
#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "nvddobj.h"
#include "d3dinc.h"
#include "nvd3dmac.h"
#include "nv3ddbg.h"
#include "nv4dinc.h"
#include "..\..\..\..\..\sdk\nvidia\inc\nvcm.h"

#ifdef NV_PROFILE

#if 0

/*
 * ----------
 * Locutus II
 * ----------
 */

/*
 * header files
 * ------------
 */
#include <stdio.h>
#include "..\..\..\..\..\apps\performance\locutus ii\dbase.h"

/*
 * constants
 * ---------
 */
#define L2_REG_VALUE            "Direct3D"

#define NVP_CAPMAX              65536
#define NVP_CAPMARK             65000

/*
 * locals
 * ------
 */
DWORD   nvpContextCount  = 0;
HANDLE  nvpFile          = 0;
DWORD   nvpCount         = 0;
DWORD   nvpEnableBits    = 0;
DWORD   nvpCyclesPerNanoSecond = 0;
__int64 nvpStartTime;
__int64 nvpTime[256];
BYTE    nvpData[NVP_CAPMAX];

L2ALPHABET nvpL2Alphabet[] =
{
    { NVP_T_CLOCK,      "t.clock",              L2_ALPHABETFLAG_TIME},
    { NVP_T_FLUSH,      "t.flush2disk",         L2_ALPHABETFLAG_TIME},
    { NVP_T_SCENE,      "t.scene",              L2_ALPHABETFLAG_TIME},
    { NVP_T_FLIP,       "t.wait4flip",          L2_ALPHABETFLAG_TIME},
    { NVP_T_DP2,        "t.dprim2",             L2_ALPHABETFLAG_TIME},
    { NVP_T_FREECOUNT,  "t.wait4pushbuffer",    L2_ALPHABETFLAG_TIME},
    { NVP_T_SCENE,      "t.scene",              L2_ALPHABETFLAG_TIME},
    { NVP_T_TSTAGE,     "t.wait4texstaging",    L2_ALPHABETFLAG_TIME},

    { NVP_E_PUT,        "e.put",                L2_ALPHABETFLAG_TIME},
    { NVP_E_BEGINSCENE, "e.beginscene",         L2_ALPHABETFLAG_TIME},

    { NVP_C_PENDING,    "c.pending",            L2_ALPHABETFLAG_COUNT},
};

/*
 * public functions
 * ----------------
 */

/*
 * nvpFlush
 *
 * commits data to disk
 */
void nvpFlush
(
    void
)
{
    /*
     * do we have pending data?
     */
    if (nvpCount) {
        DWORD dw;

        /*
         * time the HD access
         */
        NVP_START (NVP_T_FLUSH);

        /*
         * flush data
         */
        WriteFile(nvpFile,nvpData,nvpCount,&dw,0);
        FlushFileBuffers (nvpFile);
        nvpCount = 0;

        /*
         * log HD access time
         */
        NVP_STOP (NVP_T_FLUSH);
        nvpLogTime (NVP_T_FLUSH,nvpTime[NVP_T_FLUSH]);
    }
}

/*
 * nvpCreate
 *
 * creates a profiling session
 */
void nvpCreate
(
    void
)
{
    HKEY  hKey;
    char  szFilename[512];
    DWORD i,j;

    /*
     * setup defaults and read overrides from regisry
     */
    nvpCyclesPerNanoSecond = 1000 / global.dwProcessorSpeed; // works up to 1GHz
    nvpEnableBits          = 7;

    /*
     * open
     */
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L2_REG_KEY, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        DWORD dwType  = REG_DWORD;
        DWORD dwSize  = sizeof(DWORD);
        DWORD dwValue;

        /*
         * read enable bits
         */
        if (RegQueryValueEx(hKey,
                            L2_REG_VALUE,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            nvpEnableBits = dwValue;
        }

        /*
         * close
         */
        RegCloseKey (hKey);
    }

    /*
     * always force flush2disk and clock ticks on
     * always disable unused bits
     */
    nvpEnableBits &= NVP_EVERYTHING;
    nvpEnableBits |= 3;

    /*
     * create profiling file
     */
    sprintf (szFilename,"\\cap%02d.lc2",++nvpContextCount);
    nvpFile = CreateFile(szFilename,GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
    if (nvpFile == INVALID_HANDLE_VALUE)
    {
        OutputDebugString ("Profiler failed\n");
        __asm int 3;
    }
    nvpCount = 0;

    /*
     * write header
     */
    for (i = nvpEnableBits, j = 0; i;)
    {
        i &= i-1;
        j++;
    }
    {
        L2HEADER nvpL2Header =
        {
            L2_FILEID,
            j
        };
        memcpy (nvpData + nvpCount,&nvpL2Header,sizeof(nvpL2Header));
        nvpCount += sizeof(nvpL2Header);
    }

    /*
     * write alphabet entries
     */
    for (j = 0; j < 32; j++)
    {
        if (nvpEnableBits & (1 << j))
        {
            for (i = 0; i<sizeof(nvpL2Alphabet) / sizeof(nvpL2Alphabet[0]); i++)
            {
                if (j == nvpL2Alphabet[i].bID)
                {
                    memcpy (nvpData + nvpCount,nvpL2Alphabet + i,sizeof(nvpL2Alphabet[0]));
                    nvpCount += sizeof(nvpL2Alphabet[0]);
                    break;
                }
            }
        }
    }

    /*
     * log start time - used for events
     *  spoof my macros to give me negative abs time
     */
    nvpTime[0] = 0;
    NVP_START(0);
    nvpStartTime = nvpTime[0];
}

/*
 * nvpDestroy
 *
 * terminates profiling session
 */
void nvpDestroy
(
    void
)
{
    /*
     * write out pending data
     */
    nvpFlush();

    /*
     * close file
     */
    CloseHandle (nvpFile);

    /*
     * reset values
     */
    nvpFile = 0;
}

/*
 * nvpLogCount
 *
 * logs a count
 */
void nvpLogCount
(
    DWORD   id,
    __int64 count
)
{
    if (nvpEnableBits & (1 << id))
    {
        L2ENTRY *entry = (L2ENTRY*)(nvpData + nvpCount);
        entry->bID     = (BYTE)id;
        entry->qwValue = count;
        nvpCount += sizeof(L2ENTRY);
        if (nvpCount > NVP_CAPMARK) nvpFlush();
    }
}

/*
 * nvpLogTime
 *
 * logs a time in nanoseconds (argument is in clock ticks)
 */
void nvpLogTime
(
    DWORD   id,
    __int64 time
)
{
    if (nvpEnableBits & (1 << id))
    {
        L2ENTRY *entry = (L2ENTRY*)(nvpData + nvpCount);
        entry->bID     = (BYTE)id;
        entry->qwValue = time * nvpCyclesPerNanoSecond;
        nvpCount += sizeof(L2ENTRY);
        if (nvpCount > NVP_CAPMARK) nvpFlush();
    }
}

/*
 * nvpLogEvent
 *
 * logs the time an event took place
 */
void nvpLogEvent
(
    DWORD id
)
{
    if (nvpEnableBits & (1 << id))
    {
        nvpTime[255] = nvpStartTime;
        NVP_STOP(255);
        nvpLogTime (id,nvpTime[255]);
    }
}

#endif // 0

#if 1

/*
 * Locutus profiler section
 */
DWORD nvGetCPUSpeed (void);

//////////////////////////////////////////////////////////////////////////////
// constants
#define LOCUTUS_REG_KEY             "SOFTWARE\\NVIDIA Corporation\\Locutus"
#define LOCUTUS_REG_ENABLEBITS      "D3DEnableBits"
#define LOCUTUS_REG_TRIPERSECWINDOW "D3DTriPerSecWindow"

#define BTFT_1              0x42EC5201  // version 1 file header
#define BTRT_EOF            0xff

#define BTAA_EVENT          0x00000000  // event
#define BTAA_TIME           0x00000001  // time
#define BTAA_COUNT          0x00000002  // count

#define CAP_SIZE            (65536*8)
#define CAP_MARK            (CAP_SIZE - 32)

//////////////////////////////////////////////////////////////////////////////
// structures
#pragma pack(push,1)

typedef __int64 CBTTime;
typedef int     CBTCount;

typedef struct {
    char  m_szName[16];                 // human readable description
    DWORD m_dwRecordSize;               // size of record
    DWORD m_dwAttributes;               // Record description
} CBTAlphabet;

typedef struct {
    DWORD dwFileType;
    DWORD dwClockSpeed;
    DWORD dwAlphabetCount;
} CBTHeader;
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////////
// locals
DWORD   nvDeviceBase       = 0;
DWORD   nvpEnableBits      = 0x0000fffb;
DWORD   nvpTriPerSecWindow = 50;

__int64 nvpTime[256];           // stopwatch storage
#define nvp_time nvpTime        // alias
int     nvp_context_count = 1;  // we count contexi to generate unique file names
HANDLE  nvp_file          = 0;  // capture file
BYTE    nvp_buffer[CAP_SIZE];   // local capture buffer
DWORD   nvp_index;              // buffer index

CBTAlphabet nvp_alphabet[] = {                //            mask   default groups
    { "BeginScene",         0,  BTAA_EVENT }, // 0          0x00000001  *
    { "Put",                0,  BTAA_EVENT }, // 1          0x00000002  *
    { "TriDispatch",        0,  BTAA_EVENT }, // 2          0x00000004     abc
    { "SceneTime",          8,  BTAA_TIME  }, // 3          0x00000008  *
    { "FreeCountTime",      8,  BTAA_TIME  }, // 4          0x00000010  *
    { "FlipTime",           8,  BTAA_TIME  }, // 5          0x00000020  *
    { "TStageTime",         8,  BTAA_TIME  }, // 6          0x00000040  *
    { "DP2Time",            8,  BTAA_TIME  }, // 7          0x00000080  *
    { "HDFlushTime",        8,  BTAA_TIME  }, // 8          0x00000100  *
    { "TexWaitTime",        8,  BTAA_TIME  }, // 9          0x00000200  *
    { "TexSWBltTime",       8,  BTAA_TIME  }, // 10         0x00000400  *
    { "TexHWBltTime",       8,  BTAA_TIME  }, // 11         0x00000800  *
    { "InnerLoopTime",      8,  BTAA_TIME  }, // 12         0x00001000  *
    { "BuildLoopTime",      8,  BTAA_TIME  }, // 13         0x00002000  *
    { "PendingSize",        4,  BTAA_COUNT }, // 14         0x00004000  *
    { "BltArea",            4,  BTAA_COUNT }, // 15         0x00008000  *
    { "1pix/clk",           4,  BTAA_COUNT }, // 16         0x00010000    a
    { "2pix/clk",           4,  BTAA_COUNT }, // 17         0x00020000    a
    { "TriArea",            4,  BTAA_COUNT }, // 18         0x00040000      c
    { "PixPerSec",          4,  BTAA_COUNT }, // 19         0x00080000     b
    { "TriCount",           4,  BTAA_COUNT }, // 20         0x00100000     b
};

void nvpCreate (void) {
    HKEY  hKey;
    char  s[256];

    nvDeviceBase = (U032)NvDeviceBaseGet(NV_DEV_BASE,pDriverData->dwDeviceIDNum);

    wsprintf (s,"\\captr%03d.btd",nvp_context_count);
    nvp_file=CreateFile(s,GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
    if (nvp_file==INVALID_HANDLE_VALUE) {
        MessageBeep (0);
        __asm int 3;
    }
    nvp_context_count ++;

// place header
    nvp_index = 0;
    *(DWORD*)(nvp_buffer + nvp_index) = BTFT_1;
    nvp_index += 4;
    *(DWORD*)(nvp_buffer + nvp_index) = nvGetCPUSpeed();
    nvp_index += 4;
    *(DWORD*)(nvp_buffer + nvp_index) = sizeof(nvp_alphabet)/sizeof(nvp_alphabet[0]);
    nvp_index += 4;
    memcpy (nvp_buffer+nvp_index,nvp_alphabet,sizeof(nvp_alphabet));
    nvp_index += sizeof(nvp_alphabet);

// read registry for enabled bits
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, LOCUTUS_REG_KEY, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        DWORD dwType  = REG_DWORD;
        DWORD dwSize  = sizeof(DWORD);
        DWORD dwValue;

        /*
         * read enable bits
         */
        if (RegQueryValueEx(hKey,
                            LOCUTUS_REG_ENABLEBITS,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            nvpEnableBits = dwValue;
        }

        /*
         * read the tri per sec window (only used when tripersec is enabled)
         */
        if (RegQueryValueEx(hKey,
                            LOCUTUS_REG_TRIPERSECWINDOW,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            nvpTriPerSecWindow = dwValue;
        }

        /*
         * close
         */
        RegCloseKey (hKey);
    }

    /*
     * enable compulsory flags
     */
    if (nvpEnableBits & ((1 << NVP_C_1PIXCLK) | (1 << NVP_C_2PIXCLK)))
    {
        nvpEnableBits |= (1 << NVP_C_1PIXCLK) | (1 << NVP_C_2PIXCLK) | (1 << NVP_E_TRIDISP);
    }

    if (nvpEnableBits & (1 << NVP_C_TRIAREA))
    {
        nvpEnableBits |= (1 << NVP_E_TRIDISP);
    }

    if (nvpEnableBits & (1 << NVP_C_PIXPERSEC))
    {
        nvpEnableBits |= (1 << NVP_E_TRIDISP);
    }
}

void nvpDestroy (void) {
    nvp_buffer[nvp_index++] = BTRT_EOF;
    nvpFlush();
    CloseHandle (nvp_file);
}

void nvpFlush (void) {
    NVP_START (NVP_T_HDFLUSH);
    if (nvp_index) {
        DWORD dw;
        if (!WriteFile(nvp_file,nvp_buffer,nvp_index,&dw,0)) {
            DWORD dw=GetLastError();
            __asm {
                mov eax,[dw]
                int 3
            }
        }
        FlushFileBuffers (nvp_file);
        nvp_index = 0;
    }
    NVP_STOP (NVP_T_HDFLUSH);
    nvpLogTime (NVP_T_HDFLUSH,nvpTime[NVP_T_HDFLUSH]);
}

void nvpLogEvent (DWORD id) {
    if (nvpEnableBits & (1<<id))
    {
        nvp_buffer[nvp_index++] = (BYTE)id;
        if (nvp_index>CAP_MARK) nvpFlush();
    }
}

void nvpLogCount (DWORD id,int count) {
    if (nvpEnableBits & (1<<id))
    {
        BYTE *b = nvp_buffer + nvp_index;
        *((BYTE*)b)         = (BYTE)id;
        *((CBTCount*)(b+1)) = count;
        nvp_index += sizeof(BYTE)+sizeof(CBTCount);
        if (nvp_index>CAP_MARK) nvpFlush();
    }
}

void nvpLogTime (DWORD id,__int64 time) {
    if (nvpEnableBits & (1<<id))
    {
        BYTE *b = nvp_buffer + nvp_index;
        *((BYTE*)b)        = (BYTE)id;
        *((CBTTime*)(b+1)) = time;
        nvp_index += sizeof(BYTE)+sizeof(CBTTime);
        if (nvp_index>CAP_MARK) nvpFlush();
    }
}
#endif // 1

#endif // NV_PROFILE

//////////////////////////////////////////////////////////////////////////////
// Processor Speed
//
#define LATCH_VALUE(x,y)    if (fabs(((x)-(y))/(y)) < 0.05) return (DWORD)(y)

__int64 _clock_ticks;
DWORD nvGetCPUSpeed (void) {
    double      speed;
    DWORD       dwCaps;
    SYSTEM_INFO si;
#ifndef WINNT
    GetSystemInfo (&si);
    if (si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL) return 1; // unknown
    if (si.dwProcessorType != PROCESSOR_INTEL_PENTIUM) return 1; // not pentium
#endif // #ifndef WINNT
    __asm
    {
        mov eax,1
        // cpuid
        _emit 0x0f
        _emit 0xa2
        mov [dwCaps],edx
    }
    if (!(dwCaps & 0x00000010)) return 1; // does not have tsc


    SetPriorityClass  (GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
    SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
#ifndef WINNT
    GetTickCount(); // break in cache
#endif // #ifndef WINNT
    {
#ifndef WINNT
        DWORD start,stop;
        start = GetTickCount();
#else
        LONGLONG start,stop;
        EngQueryPerformanceCounter(&start);
#endif // #ifdef WINNT
        __asm {
            pushad

            // rdtsc
            _emit 0x0f
            _emit 0x31
            mov ebp,eax
            mov ebx,edx
#ifdef NV_PROFILE
            mov ecx,8000000h
#else
            mov ecx,1000000h
#endif
        here:
            loop here

            // rdtsc
            _emit 0x0f
            _emit 0x31
            sub eax,ebp
            sbb edx,ebx

            mov [dword ptr _clock_ticks  ],eax
            mov [dword ptr _clock_ticks+4],edx

            popad
        }
#ifndef WINNT
        stop = GetTickCount();
#else
        EngQueryPerformanceCounter(&stop);
#endif // #ifndef WINNT
        speed = ((double)_clock_ticks)/(1000.0*(double)(stop-start));
    }
    SetPriorityClass  (GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
    SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_NORMAL);

// normalize to known values
    LATCH_VALUE (speed, 90);
    LATCH_VALUE (speed,100);
    LATCH_VALUE (speed,166);
    LATCH_VALUE (speed,200);
    LATCH_VALUE (speed,233);
    LATCH_VALUE (speed,266);
    LATCH_VALUE (speed,300);
    LATCH_VALUE (speed,333);
    LATCH_VALUE (speed,400);
    LATCH_VALUE (speed,450);
    LATCH_VALUE (speed,500);
    LATCH_VALUE (speed,550);
    LATCH_VALUE (speed,600);
// did not latch - return unaltered (can be up to 10% off)
    return (DWORD)speed;
}

BOOL nvDetectMMX
(
    void
)
{
    DWORD       dwCaps;
    SYSTEM_INFO si;
#ifndef WINNT
    GetSystemInfo (&si);
    if (si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL) return FALSE; // unknown
    if (si.dwProcessorType != PROCESSOR_INTEL_PENTIUM) return FALSE; // not pentium
#endif // #ifndef WINNT
    __asm
    {
        mov eax,1
        // cpuid
        _emit 0x0f
        _emit 0xa2
        mov [dwCaps],edx
    }

    return (dwCaps & 0x00800000) ? TRUE : FALSE;
}

BOOL nvDetectKATMAI
(
    void
)
{
    DWORD       dwCaps;
//    DWORD       dwCR4;
    SYSTEM_INFO si;

#ifndef WINNT
    GetSystemInfo (&si);
    if (si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL) return FALSE; // unknown
    if (si.dwProcessorType != PROCESSOR_INTEL_PENTIUM) return FALSE; // not pentium
#endif // #ifndef WINNT

    __asm
    {
        mov eax,1
        // cpuid
        _emit 0x0f
        _emit 0xa2
        mov [dwCaps],edx

/*
        //mov eax,cr4
        _emit 0x0f
        _emit 0x20
        _emit 0xe0
        mov [dwCR4],eax
*/
    }

    return ((dwCaps & 0x02000000) /*&& (dwCR4 & 0x200)*/) ? TRUE : FALSE;
}

#endif // NV4