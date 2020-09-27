/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvProf.cpp                                                        *
*   NV4 specific profiling routines.                                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 05/28/98 - NV4 development.             *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#ifdef WINNT
#include "nvapi.h"
#endif

#include "nvaccess.h"

#include "..\..\..\common\src\nvPM.c"

#ifdef CAPTURE
#ifndef WINNT
#include "..\..\..\common\src\nvCapture.c"
#endif
#endif

//
// nv_ref.h - RM guys wil get me a better way to read this
//
#define NV_PBUS_PCI_NV_18                                0x00001848 /* RW-4R */
#define NV_PBUS_PCI_NV_18__ALIAS_1              NV_CONFIG_PCI_NV_18 /*       */
#define NV_CONFIG_PCI_NV_18                              0x00000048 /* RW-4R */
#define NV_CONFIG_PCI_NV_18__ALIAS_1              NV_PBUS_PCI_NV_18 /*       */

#define NV_PBUS_PCI_NV_18_AGP_STATUS_RQ                       31:24 /* C--VF */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RQ_32               0x0000001F /* C---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_SBA                        9:9 /* R--VF */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_SBA_NONE            0x00000000 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_SBA_CAPABLE         0x00000001 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_FW                         4:4 /* R--VF */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_FW_NONE             0x00000000 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_FW_CAPABLE          0x00000001 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RATE                       2:0 /* R--VF */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RATE_1X             0x00000001 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RATE_2X             0x00000002 /* ----V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RATE_1X_AND_2X      0x00000003 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RATE_4X             0x00000004 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RATE_1X_2X_4X       0x00000007 /* R---V */

#define NV_PBUS_PCI_NV_19                                0x0000184C /* RW-4R */
#define NV_PBUS_PCI_NV_19__ALIAS_1              NV_CONFIG_PCI_NV_19 /*       */
#define NV_CONFIG_PCI_NV_19                              0x0000004C /* RW-4R */
#define NV_CONFIG_PCI_NV_19__ALIAS_1              NV_PBUS_PCI_NV_19 /*       */

#define NV_PBUS_PCI_NV_19_AGP_COMMAND_RQ_DEPTH                28:24 /* RWIVF */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_RQ_DEPTH_0         0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_SBA_ENABLE                9:9 /* RWIVF */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_SBA_ENABLE_OFF     0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_SBA_ENABLE_ON      0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_AGP_ENABLE                8:8 /* RWIVF */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_AGP_ENABLE_OFF     0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_AGP_ENABLE_ON      0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_FW_ENABLE                 4:4 /* RWIVF */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_FW_ENABLE_OFF      0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_FW_ENABLE_ON       0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_DATA_RATE                 2:0 /* RWIVF */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_DATA_RATE_OFF      0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_DATA_RATE_1X       0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_DATA_RATE_2X       0x00000002 /* RW--V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_DATA_RATE_4X       0x00000004 /* RW--V */

//
//  NV4 PCI defines - These values should match those in NV4_REF.H
//
#define NV4_VENDOR_ID                   0x10DE
#define ST4_VENDOR_ID                   0x10DE
#define NV4_DEVICE_VGA_ID               0x0008
#define NV4_DEVICE_NV4_ID               0x0020
#define NV5_DEVICE_NV5_ID               0x0028
#define NV5ULTRA_DEVICE_NV5ULTRA_ID     0x0029
#define NV5VANTA_DEVICE_NV5VANTA_ID     0x002C
#define NV5MODEL64_DEVICE_NV5MODEL64_ID 0x002D
#define NV0A_DEVICE_NV0A_ID             0x00A0
#define NV10_DEVICE_NV10_ID             0x0100
#define NV10DDR_DEVICE_NV10DDR_ID       0x0101
#define NV10GL_DEVICE_NV10GL_ID         0x0103
#define NV11_DEVICE_NV11_ID             0x0110
#define NV11DDR_DEVICE_NV11DDR_ID       0x0111
#define NV11M_DEVICE_NV11M_ID           0x0112
#define NV11GL_DEVICE_NV11GL_ID         0x0113
#define NV15_DEVICE_NV15_ID             0x0150
#define NV15DDR_DEVICE_NV15DDR_ID       0x0151
#define NV15BR_DEVICE_NV15BR_ID         0x0152
#define NV15GL_DEVICE_NV15GL_ID         0x0153
#define NV20_DEVICE_NV20_ID             0x0200
#define NV20_DEVICE_NV20_1_ID           0x0201
#define NV20_DEVICE_NV20_2_ID           0x0202
#define NV20_DEVICE_NV20_3_ID           0x0203




#ifdef NV_PROFILE

/*
 * Locutus profiler section
 */

//////////////////////////////////////////////////////////////////////////////
// constants

#define BTFT_1              0x42EC5201  // version 1 file header
#define BTRT_EOF            0xff

#define BTAA_EVENT          0x00000000  // event
#define BTAA_TIME           0x00000001  // time
#define BTAA_COUNT          0x00000002  // count

#define CAP_SIZE            (65536*16)
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
#ifndef WINNT
DWORD   nvDeviceBase;           // nv base address
#endif
__int64 nvpTime[256];           // stopwatch storage
#define nvp_time nvpTime        // alias
int     nvp_context_count = 1;  // we count contexi to generate unique file names
HANDLE  nvp_file          = 0;  // capture file
BYTE    nvp_buffer[CAP_SIZE];   // local capture buffer
DWORD   nvp_index;              // buffer index

CBTAlphabet nvp_alphabet[] = {
    { "BeginScene",         0,  BTAA_EVENT }, // #define NVP_E_BEGINSCENE            0
    { "Put",                0,  BTAA_EVENT }, // #define NVP_E_PUT                   1

    { "HDFlush",            8,  BTAA_TIME  }, // #define NVP_T_HDFLUSH               2
    { "SceneTime",          8,  BTAA_TIME  }, // #define NVP_T_SCENE                 3
    { "DrawPrim2Time",      8,  BTAA_TIME  }, // #define NVP_T_DP2                   4
#ifdef NV_PROFILE_CPULOCK
    { "lockVBDP2Exit",      8,  BTAA_TIME  }, // #define NVP_T_VBDP2_LOCK            5 // dp2 exit cpu lock
    { "lockVB",             8,  BTAA_TIME  }, // #define NVP_T_VB_LOCK               6 // D3DLockExecuteBuffer
    { "lockVBInline",       8,  BTAA_TIME  }, // #define NVP_T_VB_INLINE             7 // inline VB lock
    { "lockTexULS",         8,  BTAA_TIME  }, // #define NVP_T_TEX_ULS               8 // update linear surface
    { "lockTexUSS",         8,  BTAA_TIME  }, // #define NVP_T_TEX_USS               9 // update swizzle surface
    { "lockTexBlt",         8,  BTAA_TIME  }, // #define NVP_T_TEX_BLT              10 // nvTextureBlt
    { "lockTexLock",        8,  BTAA_TIME  }, // #define NVP_T_TEX_LOCK             11 // nvTextureLock
    { "float0",             8,  BTAA_TIME  }, // #define NVP_T_FLOAT0               12
    { "float1",             8,  BTAA_TIME  }, // #define NVP_T_FLOAT1               13
#endif
#ifdef NV_PROFILE_PUSHER
    { "WrapAround",         8,  BTAA_TIME  }, // #define NVP_T_WRAPAROUND            5
    { "MakeSpace",          8,  BTAA_TIME  }, // #define NVP_T_MAKESPACE             6
    { "Adjust",             8,  BTAA_TIME  }, // #define NVP_T_ADJUST                7
    { "FlushTime",          8,  BTAA_TIME  }, // #define NVP_T_FLUSH                 8 // time spend waiting in nvPusherFlush
    { "PendingSize",        4,  BTAA_COUNT }, // #define NVP_C_PENDINGSIZE           9 // size of pending pushe buffer (at put time)
    { "Put",                4,  BTAA_COUNT }, // #define NVP_C_PUT                  10
    { "Get",                4,  BTAA_COUNT }, // #define NVP_C_GET                  11
    { "Velocity",           4,  BTAA_COUNT }, // #define NVP_C_VELOCITY             12
#endif
#ifdef NV_PROFILE_CALLSTACK
    { "SetCelsiusState",    8,  BTAA_TIME  }, // #define NVP_T_SETCELSIUSSTATE       5
    { "SetKelvinState",     8,  BTAA_TIME  }, // #define NVP_T_SETKELVINSTATE        6
    { "IdxDispatch",        8,  BTAA_TIME  }, // #define NVP_T_IDXDISPATCH           7
    { "OrdDispatch",        8,  BTAA_TIME  }, // #define NVP_T_ORDDISPATCH           8
#endif
#ifdef NV_PROFILE_DEFVB
    { "put",                4,  BTAA_COUNT }, // #define NVP_C_PUT                   5
    { "get",                4,  BTAA_COUNT }, // #define NVP_C_GET                   6
    { "freeCount",          4,  BTAA_COUNT }, // #define NVP_C_FREECOUNT             7
    { "Primitive",          0,  BTAA_EVENT }, // #define NVP_E_PRIM                  8
#endif
#ifdef NV_PROFILE_COPIES
    { "renAutoCopy",        8,  BTAA_TIME  }, // #define NVP_T_RENAME_AUTO_COPY      5
    { "lin2swz2",           8,  BTAA_TIME  }, // #define NVP_T_LIN2SWZ2              6
    { "renForward",         8,  BTAA_TIME  }, // #define NVP_T_RENAME_FWD            7
#endif
};

//    { "BeginScene",         0,  BTAA_EVENT }, // 0          0x00000001  *
//    { "CSimpleSurface::cpuLock",  8,  BTAA_TIME  }, // 1
//


bool PusherProfilingEnabled = false;

void nvpSetLogPusher(bool log)
{
    PusherProfilingEnabled = log;
}

bool nvpGetLogPusher()
{
    return PusherProfilingEnabled;
}

void nvpCreate (void) {
    char  s[256];

#ifndef WINNT
    nvDeviceBase = (U032)NvDeviceBaseGet(NV_DEV_BASE,pDriverData->dwDeviceIDNum);
#endif

    nvSprintf (s,"\\captr%03d.btd",nvp_context_count);
    nvp_file=NvCreateFile(s,GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
    if (nvp_file==INVALID_HANDLE_VALUE) {
#ifndef WINNT
        MessageBeep (0);
#endif
        __asm int 3;
    }
    nvp_context_count ++;

// place header
    nvp_index = 0;
    *(DWORD*)(nvp_buffer + nvp_index) = BTFT_1;
    nvp_index += 4;
    *(DWORD*)(nvp_buffer + nvp_index) = pDriverData->nvD3DPerfData.dwProcessorSpeed;
    nvp_index += 4;
    *(DWORD*)(nvp_buffer + nvp_index) = sizeof(nvp_alphabet)/sizeof(nvp_alphabet[0]);
    nvp_index += 4;
    memcpy (nvp_buffer+nvp_index,nvp_alphabet,sizeof(nvp_alphabet));
    nvp_index += sizeof(nvp_alphabet);

    nvpSetLogPusher(true);
}



void nvpDestroy (void) {
    nvp_buffer[nvp_index++] = BTRT_EOF;
    nvpFlush();
    NvCloseHandle (nvp_file);
    nvpSetLogPusher(false);
}

void nvpFlush (void) {
    NVP_START (NVP_T_HDFLUSH);
    if (nvp_index) {
        DWORD dw;
        if (!NvWriteFile(nvp_file,nvp_buffer,nvp_index,&dw,0)) {
#ifdef WINNT
            __asm {
                int 3
            }
#else
          DWORD dw=GetLastError();
           __asm {
              mov eax,[dw]
              int 3
          }
#endif
        }
        FlushFileBuffers (nvp_file);
        nvp_index = 0;
    }
    NVP_STOP (NVP_T_HDFLUSH);
    nvpLogTime (NVP_T_HDFLUSH,nvpTime[NVP_T_HDFLUSH]);
}

void nvpLogEvent (DWORD id) {
    nvp_buffer[nvp_index++] = (BYTE)id;
    if (nvp_index>CAP_MARK) nvpFlush();
}

void nvpLogCount (DWORD id,int count) {
    BYTE *b = nvp_buffer + nvp_index;
    *((BYTE*)b)         = (BYTE)id;
    *((CBTCount*)(b+1)) = count;
    nvp_index += sizeof(BYTE)+sizeof(CBTCount);
    if (nvp_index>CAP_MARK) nvpFlush();
}

void nvpLogTime (DWORD id,__int64 time) {
/*
    if (id == NVP_T_VBDP2_LOCK)
    {
        double t = double(time) / double(global.dwProcessorSpeed);
        if (t >= 1000.0)
        {
            __asm int 3;
        }
    }
*/

    BYTE *b = nvp_buffer + nvp_index;
    *((BYTE*)b)        = (BYTE)id;
    *((CBTTime*)(b+1)) = time;
    nvp_index += sizeof(BYTE)+sizeof(CBTTime);
    if (nvp_index>CAP_MARK) nvpFlush();
}

#endif // NV_PROFILE

#if 0
//////////////////////////////////////////////////////////////////////////////
// Processor Speed
//
#define LATCH_VALUE(x,y)    if (fabs(((x)-(y))/(y)) < 0.05) return (DWORD)(y)

__int64 _clock_ticks;
DWORD nvGetCPUSpeed (void) {
    double      speed;
    DWORD       dwCaps;
    SYSTEM_INFO si;

    GetSystemInfo (&si);
    if (si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL) return 1; // unknown
    if (si.dwProcessorType != PROCESSOR_INTEL_PENTIUM) return 1; // not pentium

    __asm
    {
        pushad
        mov eax,1
        // cpuid
        _emit 0x0f
        _emit 0xa2
        mov [dwCaps],edx
        popad
    }
    if (!(dwCaps & 0x00000010)) return 1; // does not have tsc


    SetPriorityClass  (GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
    SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    GetTickCount(); // break in cache
    {
        DWORD start,stop;

        start = GetTickCount();
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
        stop = GetTickCount();
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

    GetSystemInfo (&si);
    if (si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL) return FALSE; // unknown
    if (si.dwProcessorType != PROCESSOR_INTEL_PENTIUM) return FALSE; // not pentium

    __asm
    {
        pushad
        mov eax,1
        // cpuid
        _emit 0x0f
        _emit 0xa2
        mov [dwCaps],edx
        popad
    }

    return (dwCaps & 0x00800000) ? TRUE : FALSE;
}

BOOL nvDetectKATMAI
(
    void
)
{
    DWORD         dwCaps;
    SYSTEM_INFO   si;
    OSVERSIONINFO osvi;
    BOOL          bIsWindows98orLater;

    GetSystemInfo (&si);
    if (si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL) return FALSE; // unknown
    if (si.dwProcessorType != PROCESSOR_INTEL_PENTIUM) return FALSE; // not pentium

    __asm
    {
        pushad
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
        popad
    }

    /*
     * detect at least win 98
     */
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    GetVersionEx (&osvi);
    bIsWindows98orLater = (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
                       && ((osvi.dwMajorVersion > 4)
                        || ((osvi.dwMajorVersion == 4)
                         && (osvi.dwMinorVersion > 0)));

    return ((dwCaps & 0x02000000) && bIsWindows98orLater/*&& (dwCR4 & 0x200)*/) ? TRUE : FALSE;
}
#endif

#ifdef VTUNE_SUPPORT
/*****************************************************************************
 * support for vtuning inner loops
 *****************************************************************************/

#define DB(x)       __asm _emit ((x) & 255)
#define DW(x)       DB(x) DB((x) >> 8)
#define DD(x)       DW(x) DW((x) >> 16)

#define PAD1        __asm _emit 0x00
#define PAD8        PAD1    PAD1    PAD1    PAD1    PAD1    PAD1    PAD1    PAD1
#define PAD64       PAD8    PAD8    PAD8    PAD8    PAD8    PAD8    PAD8    PAD8
#define PAD4096     PAD64   PAD64   PAD64   PAD64   PAD64   PAD64   PAD64   PAD64
#define PAD32768    PAD4096 PAD4096 PAD4096 PAD4096 PAD4096 PAD4096 PAD4096 PAD4096

/*
 * memory object
 */
void __declspec(naked) nvVTuneMemoryImage
(
    void
)
{
    // signature
    DD(0x420352ec) DD(0xdeadbeef) DD(0x0badcafe) DD(0x0badbeef)
    DD(0x0badbeef) DD(0x0badcafe) DD(0xdeadbeef) DD(0x420352ec)
    // size
    DD(2*65536)
    // data goes here...
    PAD32768
    PAD32768
    PAD32768
    PAD32768
}

typedef struct
{
    DWORD dwAddr;       // +0
    DWORD dwSize;       // +4
} ring0params;

#define CR3Base     (0xffbfe000)
#define Alias       (0x100)
#define Addr        ((Alias << 22) | ((Alias + 1) << 12))

//
// NOTE: the logic here is broken - see r0SetPATIndices for the correct algorithm
void __declspec(naked) __cdecl ring0 (NvU32 a)
{
    __asm
    {
        jmp start

ord:  // or physical address (or [ebx],eax)
        push ebx
        push ecx
        push esi
        push edi

        //cli
        mov edi,ebx
        mov ebx,cr3
        mov esi,CR3Base + Alias * 4

        push dword ptr [esi]
        push dword ptr [esi + 4]

        mov ecx,ebx
        and ecx,~0x3ff
        or  ecx,7
        mov [esi],ecx

        mov ecx,edi
        and ecx,~0x3ff
        or  ecx,7
        mov [esi + 4],ecx
        mov cr3,ebx
        jmp n1
n1:

        mov ecx,edi
        and ecx,0x3ff
        add ecx,Addr
        or [ecx],eax

        pop dword ptr [esi + 4]
        pop dword ptr [esi]
        mov cr3,ebx
        jmp n2
n2:
        //sti

        pop edi
        pop esi
        pop ecx
        pop ebx
        ret

start:
        push ebp
        lea ebp,[esp + 8]
        pushad

        mov eax,[ebp + 0]   //rpm
        mov edx,[eax + 0]   // addr
        mov ecx,[eax + 4]   // size

nextpage:// get pde
         mov ebx,edx
         shr ebx,22
         mov eax,[CR3Base + ebx * 4]
         or  eax,2
         mov [CR3Base + ebx * 4],eax    // make writeable

         // get pte
         and eax,~0x3ff
         mov ebx,edx
         shr ebx,10
         and ebx,0x3fc
         or  ebx,eax
         mov eax,2
         call ord                       // make writeable

         // next
         sub ecx,4096
         add edx,4096
         cmp ecx,4096
        jge nextpage

        popad
        pop ebp
        xor eax,eax
        ret
    }
}

#undef Addr
#undef Alias
#undef CR3Base
void nvVTuneModifyAccessRights
(
    void
)
{
    DWORD dwBase;
    DWORD dwAddr;
    DWORD dwSize;
    DWORD i,j;

    /*
     * extract address & size of VTune block
     */
    dwBase = (DWORD)nvVTuneMemoryImage;
    dwSize = *(DWORD*)(dwBase + 32);
    dwAddr = dwBase + 36;

    /*
     * page in all pages (rely on a little luck and a lru paging meganism)
     */
    for (i = dwSize,j = dwAddr; !(i & 0x80000000); i -= 4096,j += 4096)
    {
        DWORD dwForceUse = *(volatile DWORD*)j;
    }

    /*
     * lock them down (dont want them paged out since the data is volatile)
     */
    if (!VirtualLock((void*)dwAddr,dwSize))
    {
        DPF ("*** FAILED to lock memory        ***");
        DPF ("*** GPF could happen at any time ***");
        __asm int 3;
    }

    /*
     * modify access rights to execute & r/w
     */
    {
        ring0params rpm =
        {
            dwAddr,
            dwSize
        };
        NvRmRing0Callback (0,0,(DWORD)ring0,(DWORD)&rpm.dwAddr,0);
    }
    /*
    if (!VirtualProtect((void*)dwAddr,dwSize,PAGE_EXECUTE_READWRITE,&i))
    {
        DPF ("*** FAILED to modify access rights ***");
        DPF ("*** GPF soon to follow...          ***");
        __asm int 3;
    }
    */
}

void nvVTuneFlushData
(
    void
)
{
    DWORD  dwBase;
    DWORD  dwAddr;
    DWORD  dwSize;
    HANDLE h;

    /*
     * extract address & size of VTune block
     */
    dwBase = (DWORD)nvVTuneMemoryImage;
    dwSize = *(DWORD*)(dwBase + 32);
    dwAddr = dwBase + 36;

    /*
     * write it to disk
     */
//     h = NvCreateFile("\\rawloops.bin",GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
//     NvWriteFile (h,(void*)dwAddr,dwSize,&dwSize,NULL);
//     NvCloseHandle (h);
}

#endif

//---------------------------------------------------------------------------

// search for a particular class in a list of classes
// return TRUE if found, FALSE if not
// (right now this is just a dumb linear search. it could be better...)

__inline BOOL nvClassListCheck
(
    NV_CFGEX_GET_SUPPORTED_CLASSES_PARAMS params,
    DWORD dwClass
)
{
    DWORD i=0;
    while (i < params.numClasses) {
        if (params.classBuffer[i] == dwClass) {
            return (TRUE);
        }
        i++;
    }
    return (FALSE);
}

//---------------------------------------------------------------------------

/*
 * nvDeterminePerformanceStrategy
 *
 * detects HW and use this info together with the CPU info to figure out the best
 *  performance strategy. pDriverData->dwDeviceIDNum must be initialized
 */
#pragma optimize("g",off)

#define INFINITY ~0
BOOL __stdcall nvDeterminePerformanceStrategy
(
#ifdef WINNT
    GLOBALDATA *pDriverData, PDEV* ppdev
#else
    GLOBALDATA *pDriverData
#endif
)
{
    DWORD dwRoot, dwDevID;
    BOOL  bOverride = FALSE;
    DWORD dwWidth;
    DWORD dwNVCLK;
    DWORD dwMCLK;
    DWORD dwGPUMemory;
    DWORD dwCpuClock;
    DWORD dwCpuType;
    DWORD dwFullspeedFW;
    DWORD dwFastWrite;
    DWORD dwBusCaps;
    DWORD dwStrapCaps;
    DWORD dwBase;
    DWORD dwBusType;
    DWORD dwPCIID;
    DWORD dwArchitecture;
    DWORD dwRevision;
    DWORD dwGraphicsCaps;

    pDriverData->nvD3DPerfData.dwNVClasses      = 0;
    pDriverData->nvD3DPerfData.dwCPUFeatureSet  = 0;
    pDriverData->nvD3DPerfData.dwProcessorSpeed = 0;

    //
    // determine amount of memory present
    //
    {
        MEMORYSTATUS ms;
        LPMEMORYSTATUS lpms = &ms;
        ms.dwLength = sizeof(ms);
        GlobalMemoryStatus (lpms);

        // roughly the number of MB of system memory on the machine
        pDriverData->nvD3DPerfData.dwSystemMemory = ms.dwTotalPhys >> 20;
        // round to the ceiling 8 MB
        pDriverData->nvD3DPerfData.dwSystemMemory = (pDriverData->nvD3DPerfData.dwSystemMemory + 7) & ~7;
    }

    /*
     * determine installed HW
     *
     * at this point ddraw has not been initialized yet so this code is
     *  opening a RM channel for itself to do the detection and then
     *  close it.
     */

#ifndef WINNT
    dwRoot  = pDriverData->dwRootHandle;
    dwDevID = pDriverData->dwDeviceHandle;
    NvRmOpen();
#else
    {
        DWORD res;
        /*
        * Under WINNT, must id the correct device in a  multimon
        * system.
        */

        union
        {
                ULONG osName;
                char devName[NV_DEVICE_NAME_LENGTH_MAX+1];
        } nameBuffer;
        nameBuffer.osName = ppdev->ulDeviceReference;
        #define name nameBuffer.devName

        dwDevID = 0xDEAD0001;

        NvRmOpen();

        res = NvRmAllocRoot (&dwRoot);
        if (res != NVOS01_STATUS_SUCCESS) return FALSE;

        res = NvRmAllocDevice (dwRoot, dwDevID, NV01_DEVICE_0, (BYTE*)name);
           if (res != NVOS06_STATUS_SUCCESS) return FALSE;
    }
#endif // WINNT

    // get general HW info

    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_DAC_INPUT_WIDTH,    &dwWidth);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_DAC_GRAPHICS_CLOCK, &dwNVCLK);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_DAC_MEMORY_CLOCK,   &dwMCLK);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_RAM_SIZE_MB,        &dwGPUMemory);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_PROCESSOR_SPEED,    &dwCpuClock);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_PROCESSOR_TYPE,     &dwCpuType);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_ADDRESS_NVADDR,     &dwBase);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_AGP_FULLSPEED_FW,   &dwFullspeedFW);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_AGP_FW_ENABLE,      &dwFastWrite);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_BUS_TYPE,           &dwBusType);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_PCI_ID,             &dwPCIID);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_ARCHITECTURE,       &dwArchitecture);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_REVISION,           &dwRevision);
    NvRmConfigGet (dwRoot, dwDevID, NV_CFG_GRAPHICS_CAPS,      &dwGraphicsCaps);

    pDriverData->nvD3DPerfData.dwHaveVolumeTextures = dwGraphicsCaps & NV_CFG_GRAPHICS_CAPS_3D_TEXTURES;
    pDriverData->nvD3DPerfData.dwHaveAALines        = dwGraphicsCaps & NV_CFG_GRAPHICS_CAPS_AA_LINES;
    pDriverData->nvD3DPerfData.dwHaveAnisotropic    = dwGraphicsCaps & NV_CFG_GRAPHICS_CAPS_ANISOTROPIC;

#ifdef WINNT
    {
        // Ask miniport if AGP should be enabled.
        DWORD ulReturn = 0;

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_SHOULD_AGP_BE_ENABLED,
                               NULL, 0, NULL, 0, &ulReturn))
        {
            dwBusType = 1; // Force it to be PCI if miniport thinks otherwise.
        }
    }

    // get the AGP heap size
    NvWin2KGetAgpLimit(ppdev, dwRoot, dwDevID);
#endif

    // get the list of classes this HW supports
    NV_CFGEX_GET_SUPPORTED_CLASSES_PARAMS params;
    DWORD dwSuccess;

    params.numClasses  = 0;
    params.classBuffer = NULL;

    // call to get the number of supported classes
    dwSuccess = NvRmConfigGetEx (dwRoot, dwDevID, NV_CFGEX_GET_SUPPORTED_CLASSES, &params, sizeof(params));
    nvAssert (dwSuccess == NVOS_CGE_STATUS_SUCCESS);

    // allocate and call again to actually get the class list itself
    params.classBuffer = (DWORD *) AllocIPM (params.numClasses * sizeof(DWORD));
    dwSuccess = NvRmConfigGetEx (dwRoot, dwDevID, NV_CFGEX_GET_SUPPORTED_CLASSES, &params, sizeof(params));
    nvAssert (dwSuccess == NVOS_CGE_STATUS_SUCCESS);

    // parse the class list
    if (nvClassListCheck (params, 0x0055))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0055_DX6TRI;
    if (nvClassListCheck (params, 0x0095))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0095_DX6TRI;

    if (nvClassListCheck (params, 0x0054))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0054_DX5TRI;
    if (nvClassListCheck (params, 0x0094))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0094_DX5TRI;

    if (nvClassListCheck (params, 0x0056))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0056_CELSIUS;
    if (nvClassListCheck (params, 0x0096))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0096_CELSIUS;
    if (nvClassListCheck (params, 0x1196))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_1196_CELSIUS;

    if (nvClassListCheck (params, 0x0097))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0097_KELVIN;

    if (nvClassListCheck (params, 0x0046))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0046_DAC;
    if (nvClassListCheck (params, 0x0049))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0049_DAC;
    if (nvClassListCheck (params, 0x0067))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0067_DAC;
    if (nvClassListCheck (params, 0x007C))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_007C_DAC;

    if (nvClassListCheck (params, 0x0042))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0042_CTXSURF2D;
    if (nvClassListCheck (params, 0x0062))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0062_CTXSURF2D;

    if (nvClassListCheck (params, 0x0060))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0060_IDXIMAGE;
    if (nvClassListCheck (params, 0x0064))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0064_IDXIMAGE;

    if (nvClassListCheck (params, 0x0077))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0077_SCALEDIMG;
    if (nvClassListCheck (params, 0x0063))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0063_SCALEDIMG;
    if (nvClassListCheck (params, 0x0089))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0089_SCALEDIMG;

    if (nvClassListCheck (params, 0x0038))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0038_DVDPICT;
    if (nvClassListCheck (params, 0x0088))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0088_DVDPICT;

    if (nvClassListCheck (params, 0x007A))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_007A_OVERLAY;

    if (nvClassListCheck (params, 0x0053))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0053_CTXSURF3D;
    if (nvClassListCheck (params, 0x0093))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0093_CTXSURF3D;

    if (nvClassListCheck (params, 0x004A))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_004A_GDIRECT;
    if (nvClassListCheck (params, 0x005E))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_005E_SOLIDRECT;

    if (nvClassListCheck (params, 0x0052))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0052_CTXSURFSWZ;
    if (nvClassListCheck (params, 0x009E))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_009E_CTXSURFSWZ;

    if (nvClassListCheck (params, 0x0004))  pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0004_TIMER;

    if (nvClassListCheck (params, 0x006C))  pDriverData->nvD3DPerfData.dwNVClasses1 |= NVCLASS1_006C_CHANNELDMA;
    if (nvClassListCheck (params, 0x006E))  pDriverData->nvD3DPerfData.dwNVClasses1 |= NVCLASS1_006E_CHANNELDMA;
    if (nvClassListCheck (params, 0x206E))  pDriverData->nvD3DPerfData.dwNVClasses1 |= NVCLASS1_206E_CHANNELDMA;

    // free the dynamically allocated list
    FreeIPM (params.classBuffer);

    dwNVCLK = (dwNVCLK + 500000) / 1000000;
    dwMCLK  = (dwMCLK  + 500000) / 1000000;

    // disconnect
#ifdef WINNT
    {
        DWORD res;
        res = NvRmFree (dwRoot, NV01_NULL_OBJECT, dwRoot);
        if (res != NVOS00_STATUS_SUCCESS) return FALSE;
    }
#endif
    NvRmClose();

    dwStrapCaps = *(DWORD*)(dwBase + NV_PBUS_PCI_NV_18);
    dwBusCaps   = *(DWORD*)(dwBase + NV_PBUS_PCI_NV_19);

    //
    // override
    //
#ifdef FORCE_NV10_AS_NV5
    pDriverData->nvD3DPerfData.dwNVClasses &= ~(NVCLASS_0056_CELSIUS |
                                                NVCLASS_0096_CELSIUS |
                                                NVCLASS_1196_CELSIUS);
#endif

#ifdef SPOOF_KELVIN
    pDriverData->nvD3DPerfData.dwNVClasses |= NVCLASS_0097_KELVIN;
#endif

    //
    // map cpu onto feature flags
    //
    pDriverData->nvD3DPerfData.dwProcessorSpeed = dwCpuClock;
    if (dwCpuType & NV_CPU_FUNC_MMX)     pDriverData->nvD3DPerfData.dwCPUFeatureSet |= FS_MMX;
    if (dwCpuType & NV_CPU_FUNC_SSE)     pDriverData->nvD3DPerfData.dwCPUFeatureSet |= FS_KATMAI;
    if (dwCpuType & NV_CPU_FUNC_SSE2)    pDriverData->nvD3DPerfData.dwCPUFeatureSet |= FS_WILLAMETTE;
    if (dwCpuType & NV_CPU_FUNC_3DNOW)   pDriverData->nvD3DPerfData.dwCPUFeatureSet |= FS_3DNOW;
    if (dwCpuType & NV_CPU_FUNC_SFENCE)  pDriverData->nvD3DPerfData.dwCPUFeatureSet |= FS_SFENCE;
    if ((dwCpuType & 0xff) == NV_CPU_K7) pDriverData->nvD3DPerfData.dwCPUFeatureSet |= FS_ATHLON;

    if ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI) ||
        (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE) ||
        (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON))
    {
        pDriverData->nvD3DPerfData.dwCPUFeatureSet |= FS_PREFETCH;
    }

    // See if this is a PCI bus interface
    if (dwBusType == 1)
    {
        pDriverData->nvD3DPerfData.dwCPUFeatureSet |= FS_PCIBUS;
    }

    //
    // map misc flags
    //
    // fast writes
    if (dwFastWrite && dwFullspeedFW)   pDriverData->nvD3DPerfData.dwCPUFeatureSet |= FS_FULLFASTWRITE;
    if (dwFastWrite && !dwFullspeedFW)  pDriverData->nvD3DPerfData.dwCPUFeatureSet |= FS_HALFFASTWRITE;
    // detect low back end bus
    if (dwWidth == 64)                  pDriverData->nvD3DPerfData.dwCPUFeatureSet |= FS_64BIT;

    //
    // determine strategy...
    //
    DWORD dwPSOr  =  0;
    DWORD dwPSAnd = ~0;
    {
        BOOL bFastMachine      = pDriverData->nvD3DPerfData.dwProcessorSpeed >= 390;
        BOOL bRealFastMachine  = pDriverData->nvD3DPerfData.dwProcessorSpeed >= 650; //650
        BOOL bSuperFastBackEnd = dwMCLK >= 300;
        // bFastBackEnd should only be FALSE on VANTA
        BOOL bFastBackEnd      = !(pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_64BIT)
                               || (dwMCLK > 200);
        BOOL bHalfFastWrites   = pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_HALFFASTWRITE;
        BOOL bFullFastWrites   = pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_FULLFASTWRITE;
        BOOL bHasAGP           = dwBusCaps & (1 << (0 ? NV_PBUS_PCI_NV_19_AGP_COMMAND_AGP_ENABLE));

        //
        // slow back end
        //
        if (!bFastBackEnd)
        {
            if (bFastMachine) pDriverData->nvD3DPerfData.dwPerformanceStrategy = PS_CONTROL_TRAFFIC_16 |
                                                                                 PS_CONTROL_TRAFFIC_32 |
                                                                                 PS_TEXTURE_RENAME     |
                                                                                 PS_TEXTURE_PREFER_AGP;
                         else pDriverData->nvD3DPerfData.dwPerformanceStrategy = PS_CONTROL_TRAFFIC_16 |
                                                                                 PS_CONTROL_TRAFFIC_32 |
                                                                                 PS_TEXTURE_USEHW;
        }
#if 0 // (NVARCH >= 0x020)
        else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)
        {
            // put stuff here
        }
#endif
        else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS)
        {
            // decide who does what
            pDriverData->nvD3DPerfData.dwPerformanceStrategy = PS_TEXTURE_RENAME | PS_TEXTURE_USEHW;
            // decide where to put stuff
            switch ((bFullFastWrites   ? 4 : 0)
                  | (bHalfFastWrites   ? 2 : 0)
                  | (bSuperFastBackEnd ? 1 : 0))
            {
                case 0: // !FW, !SF   - bx, SDR
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy &= ~PS_PUSH_VID;
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy |=  PS_VB_PREFER_AGP;
                        break;
                case 1: // !FW,  SF   - bx, DDR
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy &= ~PS_PUSH_VID;
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy |=  PS_VB_PREFER_AGP;
                        break;
                case 2: //  1/2 FW, !SF   - camino, SDR
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy &= ~PS_PUSH_VID;
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy |=  PS_VB_PREFER_AGP;
                        break;
                case 3: //  1/2 FW,  SF   - camino, DDR
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy &= ~PS_PUSH_VID;
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy |=  PS_VB_PREFER_AGP;
                        break;
                case 6: //  FW, !SF   - camino, SDR
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy &= ~PS_PUSH_VID;
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy |=  PS_VB_PREFER_AGP;
                        break;
                case 7: //  FW,  SF   - camino, DDR (we have not experimentd with this one yet - be conservative)
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy &= ~PS_PUSH_VID;
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy |=  PS_VB_PREFER_AGP;
                        break;
            }

            if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_PCIBUS)
            {
                // PCI bus interface -- don't prefer AGP!
                pDriverData->nvD3DPerfData.dwPerformanceStrategy &=  ~PS_VB_PREFER_AGP;
            }

            // should we use auto-palettes?
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS)
            {
                pDriverData->nvD3DPerfData.dwPerformanceStrategy |= PS_AUTOPALETTE;
            }

            //camino (i.e. FW's) do 32 bit perf strategy.
            if ((pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_1196_CELSIUS)
             || (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0096_CELSIUS)) {
                pDriverData->nvD3DPerfData.dwPerformanceStrategy |= PS_CONTROL_TRAFFIC_32;
                pDriverData->nvD3DPerfData.dwPerformanceStrategy |= PS_CONTROL_TRAFFIC_16;
            }

            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)
            {
                // No CT for Kelvin
                pDriverData->nvD3DPerfData.dwPerformanceStrategy &= ~(PS_CONTROL_TRAFFIC_16 | PS_CONTROL_TRAFFIC_32);
            }

#ifdef ALTERNATE_STENCIL_MODE
            if ((dwArchitecture == 0x10) && (dwRevision == 0xB1)) {
                // Only NV11 B01 supports this
                pDriverData->nvD3DPerfData.dwPerformanceStrategy &= ~(PS_CONTROL_TRAFFIC_16 | PS_CONTROL_TRAFFIC_32);
                pDriverData->nvD3DPerfData.dwPerformanceStrategy |= PS_ALT_STENCIL;
            }
#endif


            //more control traffic for mobile NV11
            if ((pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_CONTROL_TRAFFIC_16))
            {
                if (dwPCIID == (NV11M_DEVICE_NV11M_ID << 16 | NV4_VENDOR_ID))
                {
                    pDriverData->nvD3DPerfData.dwPerformanceStrategy |= PS_CT_11M;
                }
            }

            // should we do super triangles?
#ifdef SUPERTRI
           if (bRealFastMachine
              && ((pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
                    || (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_WILLAMETTE)
                    || (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)))
            {
                pDriverData->nvD3DPerfData.dwPerformanceStrategy |= (PS_SUPERTRI | PS_ST_XFORMCLIP | PS_ST_MODEL);

                // TODO: Add transformation and clip code for Athlon
                if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)
                    pDriverData->nvD3DPerfData.dwPerformanceStrategy &= ~PS_ST_XFORMCLIP;
            }
#endif
        }
        // ben BUGBUG
        // this is ugly. we infer nv4 vs. nv5 based on the DAC object. philosophically, this is illegal.
        else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0049_DAC)
        {
            if (bFastMachine) pDriverData->nvD3DPerfData.dwPerformanceStrategy = PS_CONTROL_TRAFFIC_32 | PS_TEXTURE_RENAME;
                         else pDriverData->nvD3DPerfData.dwPerformanceStrategy = PS_CONTROL_TRAFFIC_32 | PS_TEXTURE_USEHW;
        }
        else
        {
            if (bFastMachine) pDriverData->nvD3DPerfData.dwPerformanceStrategy = PS_CONTROL_TRAFFIC_16 |
                                                                                 PS_CONTROL_TRAFFIC_32 |
                                                                                 PS_TEXTURE_RENAME     |
                                                                                 PS_TEXTURE_PREFER_AGP;
                         else pDriverData->nvD3DPerfData.dwPerformanceStrategy = PS_CONTROL_TRAFFIC_16 |
                                                                                 PS_CONTROL_TRAFFIC_32 |
                                                                                 PS_TEXTURE_USEHW;
        }

#ifdef FORCE_CT
#if FORCE_CT
    pDriverData->nvD3DPerfData.dwPerformanceStrategy |= PS_CONTROL_TRAFFIC_32;
    pDriverData->nvD3DPerfData.dwPerformanceStrategy |= PS_CONTROL_TRAFFIC_16;
#else
    pDriverData->nvD3DPerfData.dwPerformanceStrategy &= ~PS_CONTROL_TRAFFIC_32;
    pDriverData->nvD3DPerfData.dwPerformanceStrategy &= ~PS_CONTROL_TRAFFIC_16;
#endif
#endif


        //
        // override from registry
        //
        {
            DWORD dwValue;

#ifdef WINNT
            PDEV *hKey = ppdev;
#else
            HKEY  hKey = OpenLocalD3DRegKey();
#endif

            if (hKey)
            {
                decryptRegistryKeys();
                if (nvReadRegistryDWORD(hKey,reg.szPerformanceStrategyOrString,&dwValue))
                {
                    dwPSOr    = dwValue;
                    bOverride = TRUE;
                }
                if (nvReadRegistryDWORD(hKey,reg.szPerformanceStrategyAndString,&dwValue))
                {
                    dwPSAnd    = dwValue;
                    bOverride = TRUE;
                }
#ifndef WINNT
                RegCloseKey(hKey);
#endif
                if (nvReadRegistryDWORD(hKey, "BUSCONTROL",&dwValue))
                {
                    if (((dwPCIID & 0xffff0000) == 0x00a00000) && (dwValue == 0xf0080001))
                    {
                        pDriverData->nvD3DPerfData.dwPerformanceStrategy |= PS_MUSH;
                        global.fdClockPerTri          = double(dwNVCLK) / 3.0;
                        global.fdClockPerPixel        = double(dwNVCLK) / 125.0;
                    }
                    bOverride = TRUE;
                }
#ifndef WINNT
                RegCloseKey(hKey);
#endif
            }
            pDriverData->nvD3DPerfData.dwPerformanceStrategy &= dwPSAnd;
            pDriverData->nvD3DPerfData.dwPerformanceStrategy |= dwPSOr;
        }
    }

    // always enable vertex buffer renaming
    pDriverData->nvD3DPerfData.dwPerformanceStrategy |=  PS_VB_RENAME;

/*
    pDriverData->nvD3DPerfData.dwPerformanceStrategy = (1 ? PS_TEXTURE_RENAME     : 0)
                                 | (0 ? PS_TEXTURE_USEHW      : 0)
                                 | (1 ? PS_TEXTURE_PREFER_AGP : 0)
                                 | (1 ? PS_CONTROL_TRAFFIC_16 : 0)
                                 | (1 ? PS_CONTROL_TRAFFIC_32 : 0);
//*/

#define SIZE_VIDMEM_PUSH_BUFFER         (1.0f /  32.0f)     // of total vid mem
#define SIZE_VIDMEM_STAGE_BUFFER        (2.0f /  32.0f)     // of total vid mem


#define SIZE_SYSMEM_PUSH_BUFFER         (2.5f / 32.0f)      // of total sys mem (buffer lives in PCI or AGP)
#define SIZE_SYSMEM_STAGE_BUFFER        (3.0f / 32.0f)      // of total sys mem (buffer lives in PCI or AGP)

    DWORD dwPBSize, dwSBSize;

    if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_PUSH_VID) // buffers in video memory
    {
        dwPBSize = DWORD(1024.0f * 1024.0f * dwGPUMemory * SIZE_VIDMEM_PUSH_BUFFER);
        dwSBSize = DWORD(1024.0f * 1024.0f * dwGPUMemory * SIZE_VIDMEM_STAGE_BUFFER);
    }
    else // buffers in AGP or PCI memory
    {
        // base AGP buffer sizes on aperture or memory size, depending on their relative sizes
        DWORD dwMem = pDriverData->nvD3DPerfData.dwSystemMemory;
        if (dwBusType == 4) { // limit by aperture size if system has AGP bus
            dwMem = min(dwMem, (pDriverData->regMaxAGPLimit + 1) / 1024 / 1024);
        }
        dwPBSize = DWORD(1024.0f * 1024.0f * dwMem * SIZE_SYSMEM_PUSH_BUFFER);
        dwSBSize = DWORD(1024.0f * 1024.0f * dwMem * SIZE_SYSMEM_STAGE_BUFFER);
    }

    dwPBSize = max(dwPBSize, 128 * 1024); // must be at least 128K

    dwPBSize = (dwPBSize + 4095) & ~4095; // 4K align

    if ((pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) ||
        (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN))
    {
        dwSBSize = max(dwSBSize, 128 * 1024); // must be at least 128K
        dwSBSize = (dwSBSize + 4095) & ~4095; // 4K align
    }
    else
    {
        dwSBSize = 0;
    }

    // limit maximum size of push buffer & staging buffer
    dwPBSize = min(dwPBSize, 4 * 1024 * 1024); // 4MB max
    dwSBSize = min(dwSBSize, 4 * 1024 * 1024); // 4MB max

    pDriverData->nvD3DPerfData.dwRecommendedStageBufferSize = dwSBSize;
    pDriverData->nvD3DPerfData.dwRecommendedPushBufferSize  = dwPBSize;

    //
    // determine maximum texture and recommended pci texture heap sizes
    //

    // give them two MB per 16 MB of system memory
    pDriverData->nvD3DPerfData.dwRecommendedPCITexHeapSize = (pDriverData->nvD3DPerfData.dwSystemMemory >> 4) << 1;
    // give them at least 2
    pDriverData->nvD3DPerfData.dwRecommendedPCITexHeapSize = max (pDriverData->nvD3DPerfData.dwRecommendedPCITexHeapSize, 2);

    // convert MB to bytes
    if(pDriverData->nvD3DPerfData.dwRecommendedPCITexHeapSize > 32) pDriverData->nvD3DPerfData.dwRecommendedPCITexHeapSize = 32;
    pDriverData->nvD3DPerfData.dwRecommendedPCITexHeapSize <<= 20;


    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            pDriverData->nvD3DPerfData.dwMaxTextureSize = 4096;
            pDriverData->nvD3DPerfData.dwMaxVolumeTextureSize = pDriverData->nvD3DPerfData.dwHaveVolumeTextures ? 512 : 0;
    } else {
            pDriverData->nvD3DPerfData.dwMaxTextureSize = 2048;
            pDriverData->nvD3DPerfData.dwMaxVolumeTextureSize = 0;
    }

    if (dwGPUMemory < 32) {
        // Reset dwMaxTextureSize for cards with less memory
        pDriverData->nvD3DPerfData.dwMaxTextureSize = (pDriverData->nvD3DPerfData.dwSystemMemory >= 48) ? 2048 : 1024;
    }

#define PRINT
#if defined(DEBUG) || defined(PRINT)
    {
        static BOOL bPrintedThisBefore = FALSE;

        if (!bPrintedThisBefore)
        {
            char szString[128];

            PF (" NV: classes=%08x %d/%d RAM=%dMB bus=%d", pDriverData->nvD3DPerfData.dwNVClasses,dwNVCLK,dwMCLK,dwGPUMemory,dwWidth);
            PF ("CPU: FS=%08x %dMHz", pDriverData->nvD3DPerfData.dwCPUFeatureSet, pDriverData->nvD3DPerfData.dwProcessorSpeed);

            {
                BOOL  bAGP   = dwBusCaps & (1 << (0 ? NV_PBUS_PCI_NV_19_AGP_COMMAND_AGP_ENABLE));
                DWORD dwRate = dwBusCaps ? (dwBusCaps & 7) : 0;
                BOOL  bSBA   = dwBusCaps & (1 << (0 ? NV_PBUS_PCI_NV_19_AGP_COMMAND_SBA_ENABLE));
                BOOL  bFW    = dwBusCaps & (1 << (0 ? NV_PBUS_PCI_NV_19_AGP_COMMAND_FW_ENABLE));

                PF (" BC: AGP=%dx, SBA=%d, HFW=%d, SSFW=%d, SFFW=%d",
                     bAGP ? dwRate : 0,
                     bSBA ? 1 : 0,
                     bFW ? 1 : 0,
                     (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_HALFFASTWRITE) ? 1 : 0,
                     (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_FULLFASTWRITE) ? 1 : 0);
            }
            {
                DWORD dwRate = dwStrapCaps ? (dwStrapCaps & 7) : 0;
                BOOL  bSBA   = dwStrapCaps & (1 << (0 ? NV_PBUS_PCI_NV_18_AGP_STATUS_SBA));
                BOOL  bFW    = dwStrapCaps & (1 << (0 ? NV_PBUS_PCI_NV_18_AGP_STATUS_FW));

                PF (" SC: AGP=%s%s%s%s, SBA=%d, FW=%d",
                     (dwRate & 4) ? "4" : "",
                     (dwRate & 2) ? "2" : "",
                     (dwRate & 1) ? "1" : "",
                     (dwRate & 7) ? "x" : "n/a",
                     bSBA ? 1 : 0,
                     bFW ? 1 : 0);
            }

            PF (" PS: %08x", pDriverData->nvD3DPerfData.dwPerformanceStrategy);

            nvStrCpy (szString, "     ");
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_TEXTURE_RENAME)     nvStrCat (szString, "texRn ");
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_TEXTURE_USEHW)      nvStrCat (szString, "texHW ");
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_TEXTURE_PREFER_AGP) nvStrCat (szString, "texAGP ");
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_CONTROL_TRAFFIC_16) nvStrCat (szString, "ct16 ");
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_CONTROL_TRAFFIC_32) nvStrCat (szString, "ct32 ");
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_VB_RENAME)          nvStrCat (szString, "vbRn ");
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_VB_PREFER_AGP)      nvStrCat (szString, "vbAGP ");
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_PUSH_VID)           nvStrCat (szString, "pushVID ");
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_SUPERTRI)           nvStrCat (szString, "supTri ");
            PF (szString);

            if (bOverride) {
                PF ("     * PS set from registry");
            }

            bPrintedThisBefore = TRUE;
        }
    }
#endif

    // calibrate delay loop
    DWORD dwCaps;
    __asm
    {
        push    eax
        push    ebx
        push    ecx
        push    edx

        mov eax,1
        cpuid          // assume cpuid capable
        mov [dwCaps],edx

        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }
    if (dwCaps & 0x10) // rdtsc capable
    {
        __int64 qwStart,qwStop;

        global.dwDelayCount = 10000;
        nvDelay();
        nvDelay();
        __asm
        {
            push    eax
            push    edx

            rdtsc
            mov DWORD PTR [qwStart + 0],eax
            mov DWORD PTR [qwStart + 4],edx

            pop     edx
            pop     eax
        }
        nvDelay();
        __asm
        {
            push    eax
            push    edx

            rdtsc
            mov DWORD PTR [qwStop + 0],eax
            mov DWORD PTR [qwStop + 4],edx

            pop     edx
            pop     eax

            // 3DMark 2000 leaves the floating point stack full of garbage
            // which causes the 3rd FILD below to fail resulting in a global.dwDelayCount of 0 and
            // ultimately causing the driver to hang
            // (perhaps they use MMX instructions)
            // clearing the MMX state here solves the problem
        }
        // this used to cause a fault on non-MMX machines.  Have to check for MMX before using
        // EMMS
        if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_MMX)
        {
            __asm
            {
                emms
            }
        }

        // set delay to 10us
        double time = double(qwStop - qwStart) / 10000.0;
        time /= (double) pDriverData->nvD3DPerfData.dwProcessorSpeed;
        global.dwDelayCount = (unsigned long) (10.0 / time);
    }
    else
    {
        // for non tsc machines we just assign a hard value for the delay loop
        global.dwDelayCount = pDriverData->nvD3DPerfData.dwProcessorSpeed * 4;
    }

    // Copy the spin count to the performance data struct
    pDriverData->nvD3DPerfData.dwSpinLoopCount = global.dwDelayCount;

    return TRUE;
}
#pragma optimize("g",on)
#endif  // NVARCH >= 0x04


void nvDetermineEarlyCopyStrategy(PNVD3DCONTEXT pContext)
{
    static DWORD magicValues[16][2] = {
        {0x3f800000, 0x44f27550},
        {0x40e00000, 0x45023056},
        {0x3ffa0000, 0x440ca46b},
        {0x3d3fbdf2, 0x42d094f4},
        {0x3f800000, 0x44c3ae90},
        {0x3e4ccccd, 0x421c1da7},
        {0x3e62eec8, 0x431ff939},
        {0x3dccccce, 0x433d085a},
        {0x3dcccccd, 0x4168c963},
        {0x00000000, 0x00000000},
        {0x41400000, 0x43fa0094},
        {0x41400000, 0x4415ffd4},
        {0x41400000, 0x442effe6},
        {0x3f100000, 0x43aeff84},
        {0x3e801000, 0x43af0117},
        {0x3f000100, 0x43aeff84}};

    static DWORD magicValues2[16][2] = {
        {0x3e62eec8, 0x431ff939},
        {0x3dccccce, 0x433d085a},
        {0x3dccccce, 0x433d085a},
        {0x3dcccccd, 0x4168c963},
        {0x3e4ccccd, 0x421c1da7},
        {0x3e4ccccd, 0x421c1da7},
        {0x3e62eec8, 0x431ff939},
        {0x3e4ccccd, 0x421c1da7},
        {0x3e62eec8, 0x431ff939},
        {0x3f800000, 0x44e24e63},
        {0x3f800000, 0x44c3ae90},
        {0x3dccccce, 0x433d085a},
        {0x3d3fbdf2, 0x42d094f4},
        {0x3ffa0000, 0x440ca46b},
        {0x3ffa0000, 0x440ca46b},
        {0x3f800000, 0x44e24e63}};

    static DWORD magicValues3[16][2] = {
        {0x40400000, 0x43fa0094},
        {0x40400000, 0x4415ffd4},
        {0x40400000, 0x442effe6},
        {0x3f000000, 0x43aeff84},
        {0x3e800000, 0x43af0117},
        {0x3e800000, 0x43af0117},
        {0x3e62eec8, 0x431ff939},
        {0x3dccccce, 0x433d085a},
        {0x3dccccce, 0x433d085a},
        {0x3dcccccd, 0x4168c963},
        {0x3e4ccccd, 0x421c1da7},
        {0x3e4ccccd, 0x421c1da7},
        {0x3e62eec8, 0x431ff939},
        {0x3e4ccccd, 0x421c1da7},
        {0x3e62eec8, 0x431ff939},
        {0x3f800000, 0x44e24e63}};

    DWORD i;
    if (pContext->dwDXAppVersion >= 0x0700) {
        for (i=0; i<15; i++) {
            if ((*(DWORD *)&pContext->surfaceViewport.dvWNear == magicValues[i][0]) &&
                (*(DWORD *)&pContext->surfaceViewport.dvWFar  == magicValues[i][1])) {
                pContext->dwEarlyCopyStrategy = i + 1;
                return;
            }

            if ((*(DWORD *)&pContext->surfaceViewport.dvWNear == magicValues3[i][0]) &&
                (*(DWORD *)&pContext->surfaceViewport.dvWFar  == magicValues3[i][1])) {
                pContext->dwEarlyCopyStrategy = 0x30 + i + 1;
                return;
            }
        }
    }
    else
    {
         for (i=0; i<15; i++) {
             if ((*(DWORD *)&pContext->surfaceViewport.dvWNear == magicValues2[i][0]) &&
                 (*(DWORD *)&pContext->surfaceViewport.dvWFar  == magicValues2[i][1])) {
                 pContext->dwEarlyCopyStrategy = 0x20 + i + 1;
                 return;
             }
         }
    }
    pContext->dwEarlyCopyStrategy = 0;

    //bHintVBRename = pContext->dwEarlyCopyStrategy >= 11;
}


#ifdef DEBUG_SURFACE_PLACEMENT
CSurfaceDebug               csd;
#endif //DEBUG_SURFACE_PLACEMENT

#ifdef PROFILE_INDEX

#define INDEX_FILE_NAME "\\idxprof.txt"

HANDLE  idx_file          = 0;  // index data file handle
DWORD   idx_buf_cnt       = 0;

void nvCreateIndexProfile()
{
    idx_file=NvCreateFile(INDEX_FILE_NAME,GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
    if (idx_file == INVALID_HANDLE_VALUE) {
        MessageBeep (0);
        __asm int 3;
    }
    idx_buf_cnt = 0;

}


void nvLogIndexData(DWORD primtype, DWORD count, WORD *idx)
{
    int i, j;
    DWORD bytes_written;

    static char outbuf[1024];
    switch (primtype)
    {
    case D3DDP2OP_INDEXEDTRIANGLELIST2:
        wsprintf(outbuf, "\n\n Buffer: %i, Primtype: Indexed Triangle List2, count: %i\n", idx_buf_cnt++, count);
        NvWriteFile(idx_file, outbuf, strlen(outbuf), &bytes_written, 0);
        for (i = 0; i < count; i++)
        {
            wsprintf(outbuf, "%04i: %04x %04x %04x\n", i, idx[0], idx[1], idx[2]);
            NvWriteFile(idx_file, outbuf, strlen(outbuf), &bytes_written, 0);
            idx+=3;
        }
        break;

    default:
        break;

    }


}

void nvDestroyIndexProfile()
{
    NvCloseHandle (idx_file);
}

#endif //PROFILE_INDEX
