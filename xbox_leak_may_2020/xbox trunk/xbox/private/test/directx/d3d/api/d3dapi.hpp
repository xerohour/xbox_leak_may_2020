#ifndef __d3dapi_h_
#define __d3dapi_h_

#pragma warning(disable: 4509;disable:4731)

#include <xtl.h>

#include "d3dapimmstats.h"

#include <stdio.h>

#include <tchar.h>
#include <d3dx8.h>
#include <xlog.h>
#include <xtestlib.h>
#include "swizzler.h"

#include <time.h>

#define LINKME(n) namespace d3dapi { int dummy##n() { return n; } }
#define C_(n) d3dapi::dummy##n()
#define D_(n) namespace d3dapi { int dummy##n(); }



//****************************************************************************
// A few remainder macros shamelessly stolen from dsound.
//****************************************************************************
#define QUOTE0(a)               #a
#define QUOTE1(a)               QUOTE0(a)

//
// #pragma Reminders
//

#define MESSAGE(a)              message(__FILE__ "(" QUOTE1(__LINE__) "): " a)
#define TODO(a)                 MESSAGE("TODO: " a)
#define BUGBUG(a)               MESSAGE("BUGBUG: " a)
#define HACKHACK(a)             MESSAGE("HACKHACK: " a)

#ifndef min
  #define min(a, b) (((a) < (b)) ? (a) : (b))
  #define max(a, b) (((a) > (b)) ? (a) : (b))
  #define min3(a, b, c) min(a, min(b, c))
  #define max3(a, b, c) max(a, max(b, c))
#endif //def min

#define ASSERT(x) {if(!(x)) { xLog(d3dapi::hlog,XLL_INFO, "ASSERT: " #x); }}

//***************************************************************************

#define BEGINTEST(func)                                 \
    namespace d3dapi {                                  \
     DWORD func();                                      \
     TESTFUNC p##func = func;                           \
     DWORD func() {                                     \
      char funcname[] = #func;                          \
      xSetFunctionName(hlog, funcname);                 \
      OutputDebugString(L"D3dapi: Entering " L###func L"\n");   \
      DWORD status = XLL_PASS;                          \
      MM_STATISTICS         mmStats, mmStats2;          \
      PS_STATISTICS         psStats, psStats2;          \
      ZeroMemory(&mmStats, sizeof(mmStats));            \
      mmStats.Length = sizeof(mmStats);                 \
      ZeroMemory(&psStats, sizeof(psStats));            \
      psStats.Length = sizeof(psStats);                 \
      ZeroMemory(&mmStats2, sizeof(mmStats2));          \
      mmStats2.Length = sizeof(mmStats2);               \
      ZeroMemory(&psStats2, sizeof(psStats2));          \
      psStats2.Length = sizeof(psStats2);               \

//    __try {
    
#define ENDTEST()                                       \
      return status;                                    \
    }}  
/*
#define ENDTEST()                                       \
      } _except(D3DAPIExceptionHandler(GetExceptionInformation())) {    \
       xLog(hlog, XLL_EXCEPTION, "Exception in %s", funcname);  \
       xEndVariation(hlog);                             \
       status = XLL_FAIL;                               \
        _asm {int 3}                                        \
      }                                                 \
      return status;                                    \
    }}  
*/

#define STARTLEAKCHECK()                                \
    MmQueryStatistics(&mmStats);                        \
    PsQueryStatistics(&psStats);                        

#define CHECKLEAKS()                                    \
    MmQueryStatistics(&mmStats2);                       \
    PsQueryStatistics(&psStats2);                       \
    d3dapi::CompareStats(&mmStats,  &mmStats2, &psStats, &psStats2);            


//#define RESETRESULT()                                 \
//  status = XLL_PASS;

#define WASBADRESULT()                                  \
    (status != XLL_PASS)

#define WASGOODRESULT()                                 \
    (status == XLL_PASS)

#define CHECKRESULT(x)                                  \
    if(!(x)) status = XLL_FAIL; else status = XLL_PASS;\
    xLog(hlog, status, __FILE__ "(" QUOTE1(__LINE__) "): " #x)
                
#define CHECKRESULTNOLOG(x)                             \
    (status = ((x) ? XLL_PASS : XLL_FAIL))

#define BLOCKRESULT(x)                                  \
    if(!(x)) status = XLL_BLOCK; else status = XLL_PASS;\
    xLog(hlog, status, __FILE__ "(" QUOTE1(__LINE__) "): " #x)

#define GETRESULT()                                     \
    (status)

#define COUNTOF(x) (sizeof(x) / sizeof(x[0]))
#define LASTOF(x) (COUNTOF(x) - 1)
#define countof(a) COUNTOF(a)
#define MIN(a,b) min(a,b)
#define MAX(a,b) max(a,b)
#define ROUNDUP(a,b) (((a) + ((b)-1)) & ~((b)-1)) //if b == power of 2
#define ROUNDDOWN(a,b) ((a) & !((b)-1))           //if b == power of 2

typedef DWORD (*TESTFUNC)();

void CallAllDummyFunctions();

namespace d3dapi {
    extern HANDLE hlog;

    extern HINSTANCE                g_hInstance;
    extern LPDIRECT3D8              g_pd3d8;
    extern LPDIRECT3DDEVICE8        g_pd3dd8;
//  extern LPDIRECT3DSWAPCHAIN8     g_pd3dsw8;
    extern LPDIRECT3DTEXTURE8       g_pd3dt8;
    extern LPDIRECT3DCUBETEXTURE8   g_pd3dtc8;
    extern LPDIRECT3DVOLUMETEXTURE8 g_pd3dtv8;
    extern LPDIRECT3DVERTEXBUFFER8  g_pd3dr8;
    extern LPDIRECT3DINDEXBUFFER8   g_pd3di8;
    extern LPDIRECT3DTEXTURE8       g_pd3dtSurf8;
    extern LPDIRECT3DSURFACE8       g_pd3ds8;
    extern LPDIRECT3DCUBETEXTURE8   g_pd3dtcSurf8;
    extern LPDIRECT3DSURFACE8       g_pd3dsc8;
    extern LPDIRECT3DVOLUMETEXTURE8 g_pd3dtvVol8;
    extern LPDIRECT3DVOLUME8        g_pd3dv8;

    ULONG rnd();
    VOID srnd(UINT seed);
    DWORD CompareStats(MM_STATISTICS*, MM_STATISTICS*, PS_STATISTICS*, PS_STATISTICS*);
    void RegisterDeinitFunc(TESTFUNC deinitproc);
    void RegisterDoAgain(TESTFUNC* newfunc);

    int D3DAPIExceptionHandler(EXCEPTION_POINTERS* p);

    class SWIZCOORD {
    public:
        DWORD u;
        DWORD v;
        DWORD s;
        SWIZNUM c;
        DWORD i;
        void Init(DWORD newu, DWORD newv, DWORD news, SWIZZLER* ps) {
            u = newu; v = newv; s = news; 
            c = ps->Convert(u, UCOORD) | ps->Convert(v, VCOORD) | ps->Convert(s, SCOORD);
            i = u + v * ps->m_size[UCOORD] + s * ps->m_size[UCOORD] * ps->m_size[VCOORD];
        }
    };

    inline DWORD Log2(DWORD Value)
    {
        ASSERT((Value & (Value - 1)) == 0);

        __asm {
            mov eax,[Value]   
            bsf ecx,eax 
            mov [Value],ecx
        }
        return Value;
    }
}

namespace D3D {
    DWORD BitsPerPixelOfD3DFORMAT(DWORD Format);
}

#pragma warning(disable:4035)
__inline __int64 GetTsc(){_asm {push ebx} _asm {cpuid} _asm{rdtsc} _asm {pop ebx}}
__inline __int64 GetSecCount(){ return GetTsc() / 733000000; }
__inline __int64 GetMsCount(){ return GetTsc() / 733000; }
__inline __int64 GetUsCount(){ return GetTsc() / 733; }
#pragma warning(default:4035)


#endif //def __d3dapi_h_
