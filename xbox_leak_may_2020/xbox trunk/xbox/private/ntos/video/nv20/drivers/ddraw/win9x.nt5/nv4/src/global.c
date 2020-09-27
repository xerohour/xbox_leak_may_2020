#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "nvddobj.h"
#include "d3dinc.h"

#ifdef NV_TEX2
#ifdef NV_FASTLOOPS
#include "..\x86\x86.h"
void ILCCompile_move (DWORD,DWORD,DWORD);
#endif
#endif

#ifdef NV_FASTLOOPS
void ILCWarmCache (void);
#endif

//#pragma data_seg("GLOBAL")
#pragma pack(push,1) // we control alignment
global_struc global={{0}};
#pragma pack(pop)

void init_globals (void) {
    memset (&global,0,sizeof(global));

#ifdef NV_FASTLOOPS
    global.fFogC1     = 2.0f;
    global.fFogC2     = -1.44269504f * (float)0x800000;

    global.fCTC1      = 0.5f;

    global.dwILCData  = NULL;
    global.dwILCCount = 0;
    global.dwILCMax   = 0;

#else  //!NV_FASTLOOPS
    dwVertexSN  = 1;
#endif //!NV_FASTLOOPS

#ifndef WINNT
// setup processor speed
    global.dwProcessorSpeed = nvGetCPUSpeed();

// detect MMX
    {
        BOOL isMMX = nvDetectMMX();
        global.dwCPUFeatureSet |= isMMX ? FS_MMX : 0;
    }

// detect KATMAI
    {
        BOOL isKATMAI = nvDetectKATMAI();
        global.dwCPUFeatureSet |= isKATMAI ? FS_KATMAI : 0;
    }
#endif // #ifndef WINNT

// generate some commonly used inner loops
#ifdef NV_FASTLOOPS
    ILCWarmCache();
#endif //NV_FASTLOOPS

// setup automipmap table
    {
        extern BYTE nvAutoMipPush[256];
        int i;

        for (i=0; i<256; i++)
        {
            float x = ((float)i - 127.5f) / 127.5f;
            if (x > 0)
            {
                x = 2.0f*x - x*x;
            }
            else
            {
                x = 2.0f*x + x*x;
            }
            nvAutoMipPush[i] = (BYTE)((x * 127.5f)+127.5f);
        }
    }

    /*
     * texture sync
     */
#ifdef NV_TEX2
    global.dwHWTextureDate = 10;
    global.dwSWTextureDate = 10;
#if 0
    {
        extern DWORD dbgLevel;
        dbgLevel = 0xffffffff;
    }
#endif
#if 0
    {
        extern DWORD dbgPreviewTexture;
        extern DWORD dbgPreviewTexturePitch;

        dbgPreviewTexture      = 0xffffffff;
        dbgPreviewTexturePitch = 640*2;
    }
#endif

#endif // NV_TEX2
    //*********************************************************
    // Set default surface alignment for AGP push buf heap. There
    // are no alignment restrictions, but we need to pass the
    // SURFACEALIGMENT struct to the ddraw surface allocation
    // routines.
    //*********************************************************
    AgpHeapAlignment.Linear.dwStartAlignment = 4;
    AgpHeapAlignment.Linear.dwPitchAlignment = 0;
}

//////////////////////////////////////////////////////////////////////////////
// InterProcessMemory code
//
static HANDLE hIPM=0;

#ifndef OLDSTUFF
void* AllocIPM (unsigned size) {
    if (!hIPM) hIPM=HEAPCREATE (size);
    return HEAPALLOC (hIPM,size);
}

void FreeIPM (void* ptr) {
    HEAPFREE (hIPM,ptr);
}

void DestroyIPM (void) {
    if (hIPM) HEAPDESTROY (hIPM);
}

void* ReallocIPM (void* ptr,unsigned size) {
    if (!hIPM) hIPM=HEAPCREATE (size);
    if (!ptr) return HEAPALLOC (hIPM,size);
    HEAPREALLOC (hIPM,ptr,size);
    return ptr;
}
#else
void* AllocIPM (unsigned size) {
    if (!hIPM) hIPM=HeapCreate (HEAP_SHARED,size,0);
    return HeapAlloc (hIPM,0,size);
}

void FreeIPM (void* ptr) {
    HeapFree (hIPM,0,ptr);
}

void DestroyIPM (void) {
    if (hIPM) HeapDestroy (hIPM);
}

void* ReallocIPM (void* ptr,unsigned size) {
    if (!hIPM) hIPM=HeapCreate (HEAP_SHARED,size,0);
    if (!ptr) return HeapAlloc (hIPM,0,size);
    return HeapReAlloc (hIPM,0,ptr,size);
}
#endif
