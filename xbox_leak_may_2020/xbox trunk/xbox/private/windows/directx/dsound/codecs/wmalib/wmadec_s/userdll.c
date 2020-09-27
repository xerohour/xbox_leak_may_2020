/*M*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1999 Intel Corporation. All Rights Reserved.
//
//
//  Purpose: Generate custom DLL
//
*M*/

#include "../common/macros.h" // This will define WMA_TARGET_X86 (or not)
#include "msaudio.h"
#include "..\..\..\dsound\dsndver.h"

// The encoder requires the use of a multithreaded library w/ Intel FFT. Make sure we have it.
// Note that even if we do have it during compilation, we also must link to
// a multithreaded C runtime library at link time.
#if defined(ENCODER) && !defined(WMA_SERIALIZE) && !defined(_MT)
#error *** Encoder is multithreaded and therefore requires MT library when using Intel FFT ***
#endif  // defined(ENCODER) && !defined(WMA_SERIALIZE) && !defined(_MT)

// ###############################################################
// List of required CPUs.
//     PX=blend, M5=Pentium(R) with MMX(tm) support
//     P6=Pentium(R) Pro, M6=Pentium(R) Pro with MMX(tm) support
//     A6=Pentium(R) III

// Define the CPU's we want optimized. It costs ~100k per CPU included.
#define M6
#define A6

#if defined(WMA_TARGET_X86) && !defined(BUILD_INTEGER) && !defined(UNDER_CE)

static const char* SET_LIB_ERR = "Set PLib error";

//#define nsp_UsesAll

#ifndef _WINDOWS_
#   include <windows.h>
#endif


#define NSPAPI(type,name,arg)
#include "..\x86\nsp.h"

/// Describe Intel CPUs and libraries
typedef enum{CPU_PX=0, CPU_P6, CPU_M5, CPU_M6, CPU_A6, CPU_NOMORE} cpu_enum;
typedef enum{LIB_PX=0, LIB_P6, LIB_M5, LIB_M6, LIB_A6, LIB_NOMORE} lib_enum;

/// New cpu can use some libraries for old cpu
static const lib_enum libUsed[][LIB_NOMORE+1] = {
    { LIB_PX, LIB_NOMORE }, 
    { LIB_P6, LIB_PX, LIB_NOMORE },
    { LIB_M5, LIB_PX, LIB_NOMORE },
    { LIB_M6, LIB_M5, LIB_P6, LIB_PX, LIB_NOMORE },
    { LIB_A6, LIB_M6, LIB_M5, LIB_P6, LIB_PX, LIB_NOMORE }
};



#define __USERLIST__


#ifdef INTELFFT_FOR_CE
// For some reason, somebody has a #define __stdcall __cdecl somewhere.
// Get rid of this, because it's causing us linker errors.
#ifdef __stdcall
#undef __stdcall
#endif
#endif  // INTELFFT_FOR_CE

#ifdef PX
#   undef  NSPAPI
#   define NSPAPI(type,name,arg) extern type __stdcall px_##name arg;
#   define PX_NAME(name) (FARPROC)px_##name
#   include "..\x86\UserDll.h"
#else
#   define PX_NAME(name) NULL
#endif

#ifdef M5
#   undef  NSPAPI
#   define NSPAPI(type,name,arg) extern type __stdcall m5_##name arg;
#   define M5_NAME(name) (FARPROC)m5_##name
#   include "..\x86\UserDll.h"
#else
#   define M5_NAME(name) NULL
#endif

#ifdef P6
#   undef  NSPAPI
#   define NSPAPI(type,name,arg) extern type __stdcall p6_##name arg;
#   define P6_NAME(name) (FARPROC)p6_##name
#   include "..\x86\UserDll.h"
#else
#   define P6_NAME(name) NULL
#endif

#ifdef M6
#   undef  NSPAPI
#   define NSPAPI(type,name,arg) extern type __stdcall m6_##name arg;
#   define M6_NAME(name) (FARPROC)m6_##name
#   include "..\x86\UserDll.h"
#else
#   define M6_NAME(name) NULL
#endif

#ifdef A6
#   undef  NSPAPI
#   define NSPAPI(type,name,arg) extern type __stdcall a6_##name arg;
#   define A6_NAME(name) (FARPROC)a6_##name
#   include "..\x86\UserDll.h"
#else
#   define A6_NAME(name) NULL
#endif


#undef  NSPAPI
#define NSPAPI(type,name,arg) \
    static FARPROC d##name; \
__declspec(naked dllexport) void __stdcall name arg { __asm {jmp d##name } }
#include "..\x86\UserDll.h"


typedef struct _USER_Desc_t {
    FARPROC *WorkAddr;
    FARPROC FuncAddr[CPU_NOMORE];
} USER_Desc_t;

static USER_Desc_t AddressBook[] = {
#undef  NSPAPI
#define NSPAPI(type,name,arg) &d##name, \
    PX_NAME(name), P6_NAME(name), M5_NAME(name), M6_NAME(name), A6_NAME(name),
#include "..\x86\UserDll.h"
};
/// how large is the table of the functions
static const int sFuncCount  = sizeof( AddressBook ) / sizeof( AddressBook[0] );


#undef __USERLIST__

#if !defined( _USE_CPUINF32 )

/// try  define cpu yourself without cpuinf32.dll
/// because DLL generated must be used in the ring 0

/// ID flag in the EFLAGS register, bit 21
#define ID_FLAG   00200000h

/// MMX-extension, feature information after cpuid(1), EDX, bit 23
#define MMX_FLAG  00800000h

/// SSX-extension, feature information after cpuid(1), EDX, bit 25
/// Should be CR0.EM = 0 and CR4.OSFXSR = 1 (ring 0)
#define SSX_FLAG  02000000h

#define _read_stc _asm  _emit 00Fh _asm  _emit 031h
#define _cpuid    _asm  _emit 00Fh _asm  _emit 0A2h

typedef struct {
   int m_family; 
   int m_stepping;
   int m_model;
   int m_type;
   int m_feature;
   int m_tlb;
   int m_cache;
   int m_freq;
   int m_max_cpuid_input;
   int m_is_mmx;
   int m_is_ssx;
   int m_has_tsc;
} IntelCpu;

#pragma optimize("", off )

/// does cpu support cpuid instruction ?
static int has_cpuid() {
   _asm  pushfd
   _asm  pop   eax
   _asm  mov   ecx, eax
   _asm  xor   eax, ID_FLAG
   _asm  push  eax
   _asm  popfd
   _asm  pushfd
   _asm  pop   eax
   _asm  and   eax, ID_FLAG
   _asm  and   ecx, ID_FLAG
   _asm  cmp   eax, ecx
   _asm  jne   has_cpuid_lab
   return 0;
has_cpuid_lab:
   return 1;
}

/// cpu identification
static int pentium_ident( IntelCpu *cpu ) {

   _asm  mov   esi, cpu
   _asm  mov   eax, 0
   _asm  push  ebx
   _cpuid
   _asm  pop   ebx
   _asm  mov   edi, eax
   _asm  mov   [esi]IntelCpu.m_max_cpuid_input, eax
   _asm  cmp   edi, 1
   _asm  jae   above_eq_1
   return 0;
above_eq_1:
   _asm  mov   eax, 1
   _asm  push  ebx
   _cpuid
   _asm  pop   ebx
   _asm  mov   ecx, eax
   _asm  and   eax, 000Fh
   _asm  mov   [esi]IntelCpu.m_stepping, eax
   _asm  mov   eax, ecx
   _asm  shr   eax, 4
   _asm  and   eax, 000Fh
   _asm  mov   [esi]IntelCpu.m_model, eax
   _asm  mov   eax, ecx
   _asm  shr   eax, 8
   _asm  and   eax, 000Fh
   _asm  mov   [esi]IntelCpu.m_family, eax
   _asm  shr   eax, 11
   _asm  and   eax, 0003h
   _asm  mov   [esi]IntelCpu.m_type, eax
   _asm  mov   [esi]IntelCpu.m_feature, edx

   /// define IA mmx extension
   _asm  xor   eax, eax
   _asm  test  edx, MMX_FLAG
   _asm  setnz al
   _asm  mov   [esi]IntelCpu.m_is_mmx, eax

   /// define IA ssx extension
   _asm  xor   eax, eax
   _asm  test  edx, SSX_FLAG
   _asm  setnz al
   _asm  mov   [esi]IntelCpu.m_is_ssx, eax

   _asm  cmp   edi, 2
   _asm  jae   above_eq_2
   return 1;

above_eq_2:
   /// get extended info, for Pentium Pro and above
   _asm  mov   eax, 2
   _asm  push  ebx
   _cpuid
   _asm  pop   ebx
   _asm  mov   [esi]IntelCpu.m_tlb, eax
   _asm  mov   [esi]IntelCpu.m_cache, edx
   return 1;
}

static cpu_enum GetProcessorId(void) {

   IntelCpu cpu;
   if( !has_cpuid() || !pentium_ident( &cpu) ) return CPU_NOMORE;
   switch ( cpu.m_family ) {
   /// note that simple Pentium P5 is not supported
   case 5: return cpu.m_is_mmx ? CPU_M5 : CPU_PX;
   /// check ssx first, maybe it is mmx also
   case 6: return cpu.m_is_ssx ? CPU_A6 : cpu.m_is_mmx ? CPU_M6 : CPU_P6;
   }
   return CPU_PX;
}   

#else

/// Define target cpu with using cpuinf32.dll
/// Here is a solution for the case if cpu is unknown
/// Note. IF CPU IS UNKNOWN THEN PX LIBRARY WILL BE USED
///

__declspec(dllimport) unsigned short wincpuid(void);
__declspec(dllimport) unsigned short wincpuidext(void);
__declspec(dllimport) unsigned long  wincpufeatures(void);

static cpu_enum GetProcessorId(void)
{
    int cpuid = (int)wincpufeatures();
    int ismmx = cpuid & 0x00800000;        /// 0x00800000 - MMX(TM) technology
    int iskni = cpuid & 0x02000000;        /// 0x02000000 - SSX
    cpuid = (int)wincpuid();               /// 0x00000F00 - CPU
    
    // much more flexible in processor ID detection. Shinn
    switch (cpuid) {
    case 5: return ismmx ? CPU_M5 : CPU_PX;
        /// check ssx first, maybe it is mmx also
    case 6: return iskni ? CPU_A6 : ismmx ? CPU_M6 : CPU_P6;
    }
    return CPU_PX;
}
#endif

/// fill SPLib function address book in correspondence to the target cpu
static BOOL SetLib( lib_enum lib )
{    
   int i = 0;
   for ( i=0; i<sFuncCount; i++ ) 
      if( NULL == AddressBook[i].FuncAddr[lib] ) 
         return FALSE;
      else
        *(AddressBook[i].WorkAddr) = AddressBook[i].FuncAddr[lib];
   return TRUE;
}

static BOOL setCpuSpecificLib()
{
   //char buf[256] = "";
   cpu_enum cpu = GetProcessorId();
   if( sFuncCount > 0 && cpu >= CPU_PX && cpu < CPU_NOMORE ) {
      
      const lib_enum* libs = libUsed[ cpu ];
      while( *libs < LIB_NOMORE )
         if( SetLib( *libs++ ) ) return TRUE;     /// SUCCESS EXIT
   }
   /// if not found, then failed with error message
   //lstrcpy( buf, " No PLib matching to CPU was found during the Waterfall" );
   //MessageBeep( MB_ICONSTOP );
   //MessageBox( 0, buf, SET_LIB_ERR, MB_ICONSTOP | MB_OK );

   // Actually, for our purposes, it's OK to fail. Do so silently.

   return FALSE;
}

/*---- Warning!!! Please don't modify this section -----*/
/*---- Begin section  ----*/
void* __cdecl nspUndContext (void);
void* __cdecl nspInitContext (void* Context);
int   __cdecl nspSizeOfContext(void);
/*---- End of section ----*/

#ifndef TLS_OUT_OF_INDEXES
#define TLS_OUT_OF_INDEXES  (DWORD)0xFFFFFFFF
#endif  // !TLS_OUT_OF_INDEXES

static DWORD s_dwTlsIndex = TLS_OUT_OF_INDEXES;
static DWORD s_dwTlsIndexRefcount = 0;

void* __cdecl nspUndContext() {
    
    void *pContext = TlsGetValue( s_dwTlsIndex );
    if( NULL == pContext )  {
        pContext = (LPVOID)LocalAlloc( LPTR, nspSizeOfContext() );
        if( NULL == pContext )
        {
            assert(WMAB_FALSE);
            exit(-5);
        }
        
        nspInitContext (pContext);
        TlsSetValue( s_dwTlsIndex, pContext );
    }
    return pContext;
}


//BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason,
//                    LPVOID lpvReserved )
BOOL MyInitNSP(DWORD fdwReason)
{
    void * pContext = NULL;
    BOOL   fResult = TRUE;
    Int    iNewRefcount;

    switch (fdwReason)
    {
    //case DLL_PROCESS_ATTACH:
    case INTELFFT_INIT_PROCESS:
        
        // Bug #36310: It is possible for codec instances to overlap (create A, create B,
        // delete A, delete B), so ref-count s_dwTlsIndex so that we do not call TlsFree
        // during "delete A". We ignore the possibility of a race condition for TlsAlloc
        // and TlsGetValue because we assume:
        //   1) Race condition can only come out of multithreaded operation
        //   2) Multithreaded operation will use DMO
        //   3) Our FIRST call will be from DMO's DllMain's DLL_PROCESS_ATTACH
        iNewRefcount = InterlockedIncrement(&s_dwTlsIndexRefcount);
        assert(iNewRefcount >= 1);

        if (1 == iNewRefcount)
        {
            // Only if refcount went from 0 to 1, should we call TlsAlloc
            assert(TLS_OUT_OF_INDEXES == s_dwTlsIndex);
            s_dwTlsIndex = TlsAlloc();
            if(s_dwTlsIndex == TLS_OUT_OF_INDEXES)
            {
                fResult = FALSE;
                goto exit;
            }
            
            fResult = setCpuSpecificLib();
            if (FALSE == fResult)
                goto exit;
        }
        else if (TLS_OUT_OF_INDEXES == s_dwTlsIndex)
        {
            // If no TLS index, DLL_PROCESS_ATTACH initialization failed,
            // probably due to unsupported CPU.
            fResult = FALSE;
            goto exit;
        }
        
        /* No break: Initialize the index for first thread. */
        /* The attached process creates a new thread. */
        
    //case DLL_THREAD_ATTACH: 
    case INTELFFT_INIT_THREAD:
        
        if (s_dwTlsIndex != TLS_OUT_OF_INDEXES && NULL == TlsGetValue(s_dwTlsIndex))
        {
            pContext = (LPVOID)LocalAlloc(LPTR, nspSizeOfContext());
            if (pContext == NULL)
            {
                fResult = FALSE;
                goto exit;
            }

            nspInitContext (pContext);
            TlsSetValue( s_dwTlsIndex, pContext );
        }
        break;
        
    //case DLL_THREAD_DETACH:
    case INTELFFT_FREE_THREAD:
        
        if (s_dwTlsIndex != TLS_OUT_OF_INDEXES)
        {
            pContext = TlsGetValue( s_dwTlsIndex );
            if (pContext != NULL)
            {
                LocalFree( (HLOCAL) pContext );
                TlsSetValue( s_dwTlsIndex, NULL); // Avoid re-freeing this
            }
        }
        break;
        
    //case DLL_PROCESS_DETACH:
    case INTELFFT_FREE_PROCESS:
        
        if (s_dwTlsIndex != TLS_OUT_OF_INDEXES)
        {
            pContext = TlsGetValue(s_dwTlsIndex);
            if (pContext != NULL)
            {
                LocalFree((HLOCAL) pContext);
                TlsSetValue( s_dwTlsIndex, NULL); // Avoid re-freeing this
            }

            iNewRefcount = InterlockedDecrement(&s_dwTlsIndexRefcount);
            assert(iNewRefcount >= 0);

            // Only call TlsFree when refcount has dropped to zero
            if (0 == iNewRefcount)
            {
                TlsFree(s_dwTlsIndex);
                s_dwTlsIndex = TLS_OUT_OF_INDEXES;
            }
        }
        break;
        
    default:
        break;
    }

exit:
    if (FALSE == fResult && INTELFFT_INIT_PROCESS == fdwReason)
    {
        // Error occurred during attach/init, free everything. OTHERWISE if
        // setCpuSpecificLib() call failed, next time this process tries to create
        // a new codec, s_dwTlsIndex != TLS_OUT_OF_INDEXES and we will return TRUE
        // which will cause a crash (jump to NULL) when caller calls FFT.
        assert(NULL == pContext);

        iNewRefcount = InterlockedDecrement(&s_dwTlsIndexRefcount);

        // Only call TlsFree when refcount has dropped to zero
        if (0 == iNewRefcount && TLS_OUT_OF_INDEXES != s_dwTlsIndex)
        {
            TlsFree(s_dwTlsIndex);
            s_dwTlsIndex = TLS_OUT_OF_INDEXES;
        }
    }

    return fResult;
}


//***************************************************************************
// Function: DllInitCodec
//
// Purpose: This function receives DllMain notifications from the DMO's
//   DllMain, if we are being built as a DMO. This allows us to perform
//   per-thread initialization, as is required by the Intel FFT.
//
// Arguments:
//   ULONG ulReason - contains the DllMain notification, such as
//     DLL_PROCESS_ATTACH.
//***************************************************************************
void DllInitCodec(ULONG ulReason)
{
    DWORD   dwNewReason = -1;
    BOOL    fResult;

    // Tranlate original reason (such as DLL_PROCESS_ATTACH) to
    // new reason (such as INTELFFT_INIT_PROCESS).

    switch (ulReason)
    {
        case DLL_PROCESS_ATTACH:
            dwNewReason = INTELFFT_INIT_PROCESS;
            break;

        case DLL_THREAD_ATTACH:
            // DO NOTHING. This notification is given for every thread in the process,
            // while in reality only a few will end up using the codec. We check each
            // call into the codec and initialize Intel FFT there if it hasn't already
            // been done for that thread.
            goto exit;

        case DLL_THREAD_DETACH:
            dwNewReason = INTELFFT_FREE_THREAD;
            break;

        case DLL_PROCESS_DETACH:
            dwNewReason = INTELFFT_FREE_PROCESS;
            break;

        default:
            // We don't handle this code. Don't call MyInitNSP.
            goto exit;
    }

    assert(-1 != dwNewReason);
    fResult = MyInitNSP(dwNewReason);
    if (FALSE == fResult && DLL_PROCESS_ATTACH == ulReason)
    {
        // Special case: DllMain will not act on initialization failure,
        // which can occur if current CPU has no supported optimizations
        // (setCpuSpecificLib will fail). Therefore when DLL_PROCESS_DETACH
        // rolls around, we will decrement refcount below zero. More importantly,
        // our assumptions for race condition avoidance are violated. Therefore,
        // we need to artificially bump up the refcount. NOTE that with refcount
        // artificially bumped, refcount never goes down. That's fine, nothing
        // was allocated anyway.
        InterlockedIncrement(&s_dwTlsIndexRefcount);
    }

exit:
    return;
} // DllInitCodec

#endif  // WMA_TARGET_X86 && !defined(BUILD_INTEGER) && !defined(UNDER_CE)
