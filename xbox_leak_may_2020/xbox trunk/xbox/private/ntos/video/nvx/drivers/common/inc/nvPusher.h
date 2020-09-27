// **************************************************************************
//
//       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// **************************************************************************
//
//  Module: nvPusher.h
//      routines for creating, destroying, and interacting with a push buffer
//
// **************************************************************************
//
//  History:
//      Ben DeWaal              07Apr1999         original code
//      Craig Duttweiler        05Apr2000         cleanup, consolidation,
//                                                and C++'ification
//
// **************************************************************************

#ifndef _NVPUSHER_H
#define _NVPUSHER_H

#include "nvUniversal.h"
#include "nv32.h"

// Having lots of problems finding a version of assert.h in all builds... so we do this for now. -mlavoie
#ifdef _DEBUG
#define nvAssert(expr) if (expr) _asm {int 3};
#else
#define nvAssert(expr)
#endif

#if (NVARCH < 0x04)

typedef void CPushBuffer;   // Not avail on NV3, but let's compile go through easier

#else // (NVARCH >= 0x04)

//---------------------------------------------------------------------------

// pusher debugging switches (available in retail builds too)

//#define _PC_PRINTPUT                      // print kick-offs (slow)
//#define _PC_PRINT                         // print important pusher transactions
//#define _PC_CHECK                         // check for push buffer problems
//#define _PC_BREAK                         // break at specific class (define CHECK also)
//#define _PC_FLUSH_ON_ADJUST               // force flush on every call to nvPusherAdjust
//#define _PC_CAPTURE_METHOD_DATA           // captures method data and allows you to view it (good to check object state)
//#define _PC_CAPTURE                       // writes contents of the the push buffer to a file

// debug switch logic

// for now we enable push buffer validation for all debug builds
#if defined(DEBUG) || defined(_PC_CAPTURE_METHOD_DATA) || defined(_PC_CAPTURE)
#define _PC_CHECK
#endif

// don't allow inline functions if we have certain other debug bits enabled
#undef _NO_INLINE
#if defined(DEBUG) || defined(_PC_PRINTPUT) || defined(_PC_PRINT) || defined(_PC_CHECK) || defined(_PC_BREAK)
#define _NO_INLINE
#endif
#if defined(_PC_FLUSH_ON_ADJUST) || defined(_PC_CAPTURE_METHOD_DATA) || defined(_PC_CAPTURE)
#define _NO_INLINE
#endif

//---------------------------------------------------------------------------

// pusher constants and macros

#define NVPUSHER_THRESHOLD_SIZE     128     // words of data always available (4 triangles)
#define NVPUSHER_MINIMUM            1024    // bytes of data for option kick-off

#define NVPUSHER_JUMP(offset)       (0x20000000 | (offset))
#define NVPUSHER_NOINC(method)      (0x40000000 | (method))
#define NVPUSHER_CALL(offset)       (0x00000002 | (offset))
#define NVPUSHER_RETURN()           (0x00020000)

//---------------------------------------------------------------------------

#ifdef _PC_CAPTURE

extern HANDLE hPusherFileBinary;
extern HANDLE hPusherFileAscii;
extern BOOL   bPusherCapture;

#define NV_PC_CAPTURE_BEGIN(filename)                                               \
{                                                                                   \
    char fullname[64];                                                              \
    nvSprintf (fullname, "%s.bin", filename);                                       \
    hPusherFileBinary = CreateFile (fullname,GENERIC_WRITE,0,0,OPEN_ALWAYS,0,0);    \
    if (hPusherFileBinary == INVALID_HANDLE_VALUE) {                                \
        DPF ("NV_PC_CAPTURE_BEGIN: Binary file open failed");                       \
        nvAssert(0);                                                                  \
    }                                                                               \
    SetFilePointer (hPusherFileBinary, 0,0, FILE_END);                              \
    nvSprintf (fullname, "%s.txt", filename);                                       \
    hPusherFileAscii = CreateFile (fullname,GENERIC_WRITE,0,0,OPEN_ALWAYS,0,0);     \
    if (hPusherFileAscii == INVALID_HANDLE_VALUE) {                                 \
        DPF ("NV_PC_CAPTURE_BEGIN: Ascii file open failed");                        \
        nvAssert(0);                                                                  \
    }                                                                               \
    SetFilePointer (hPusherFileAscii, 0,0, FILE_END);                               \
    bPusherCapture = TRUE;                                                          \
}

#define NV_PC_CAPTURE_END                                                           \
{                                                                                   \
    if (bPusherCapture) {                                                           \
        getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);              \
        FlushFileBuffers (hPusherFileBinary);                                       \
        FlushFileBuffers(hPusherFileAscii);                                         \
        CloseHandle (hPusherFileBinary);                                            \
        CloseHandle (hPusherFileAscii);                                             \
        bPusherCapture = FALSE;                                                     \
    }                                                                               \
}

#endif

//---------------------------------------------------------------------------

// pusher aliases
// alias these to point to appropriate data for your particular implementation

// hw get
#define _pdwHWGet       ((volatile DWORD)(((Nv4ControlDma*)m_dwControlDMA)->Get))

// hw put
#define _pdwHWPut       ((volatile DWORD)(((Nv4ControlDma*)m_dwControlDMA)->Put))

// hw reference count
#define _pdwHWRef       ((volatile DWORD)(((Nv10ControlDma*)m_dwControlDMA)->Reference))

// true when HW is busy
#define _pbHWBusy       (*(volatile DWORD *)(pDriverData->NvBaseFlat + 0x400700))

// cache1 ownership: bits 0:4 give the channel that currently owns the cache
#define _ffCache1Push1  (*(volatile DWORD *)(pDriverData->NvBaseFlat + 0x003204))

// cache1 status: bit 4 is set when the cache is empty
#define _ffCache1Status (*(volatile DWORD *)(pDriverData->NvBaseFlat + 0x003214))

// true if we have KNI support
#define _hasKNI         (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)

// true if we have 3DNOW support
#define _has3DNOW       (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_3DNOW)

// true if we have SFENCE support
#define _hasSFENCE      (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_SFENCE)

//---------------------------------------------------------------------------

// pusher-related macros

// the if is to allow for a string of Function such as Flip(A->B), Clear(A), Blit(C), such that the Blit to C
// does not destroy the pending waitforflip on Clear(A).  We only wait on surfaces that are just becoming a
// target and not an arbitrary surface
#define nvPusherSignalWaitForFlip(fpVidMem, dwDDSFlags)          \
{                                                                \
    if (getDC()->flipPrimaryRecord.fpFlipFrom == (fpVidMem)) {   \
        getDC()->nvPusher.setSyncFlipFlag();                     \
        getDC()->nvPusher.setFlipSurface(fpVidMem);              \
        getDC()->nvPusher.setFlipSurfaceFlags(dwDDSFlags);       \
    }                                                            \
}

// i'd really rather see these eventually just be substituted, but i'll make that mess later
#define nvPushData(a,b)    getDC()->nvPusher.push(a,b)
#define nvPusherAdjust(a)  getDC()->nvPusher.adjust(a)
#define nvPusherStart(a)   getDC()->nvPusher.start(a)
#define nvPusherFlush(a)   getDC()->nvPusher.flush(a,0)     // 0 = FLUSH_WITH_DELAY

//---------------------------------------------------------------------------

#ifdef __cplusplus

// forward definitions

struct  _def_nv_d3d_context;
typedef _def_nv_d3d_context *PNVD3DCONTEXT;


//---------------------------------------------------------------------------


class CPushBuffer
{

private:

    // pusher flags
    const enum {
        PB_FLAG_VALID          = (1 << ( 0)),  // push buffer is allocated, context DMA is allocated, and channel is open
        PB_FLAG_CHANNEL_SYNC   = (1 << ( 1)),  // signal to sync other channels b4 put
        PB_FLAG_FLIP_SYNC      = (1 << ( 2))   // will wait for flip before writing put
    };

public:

    // flush modes
    const enum {
        FLUSH_WITH_DELAY       = 0,            // flush with a delays when polling    (detrimental to CPU performance)
        FLUSH_HEAVY_POLLING    = 1             // flush & poll hw as fast as possible (detrimental to HW performance)
    };

private:

    // members
    DWORD m_dwChannel;          // handle for channel used by this push buffer
    DWORD m_dwContextDMA;       // handle for context DMA
    DWORD m_dwContextDMASize;   // size of context dma allocated for this push buffer
    DWORD m_dwControlDMA;       // controlDMA structure for this channel
    DWORD m_dwPCIHeapHandle;    // pci heap handle (if any)
    DWORD m_dwPCIHeapBase;      // pci heap base (if any)
    DWORD m_dwDDGbl;            // the DDGbl with which allocated AGP memory is associated
    DWORD m_dwHeap;             // heap from which push buffer is allocated
    // bank
    DWORD m_dwBase;             // base of push buffer
    DWORD m_dwSize;             // size of push buffer
    DWORD m_dwPut;              // current put pointer
    DWORD m_dwPutLast;          // last put wrote to HW
    DWORD m_dwGet;              // cached get pointer
    DWORD m_dwThreshold;        // threshold value
    DWORD m_dwWrapCount;        // # of wrap arounds
    DWORD m_dwPad01[1];
    // bank
    DWORD m_dwFlipSurfaceAddr;  // what surface to wait on during getFrameStatus
    DWORD m_dwFlipSurfaceFlags; // DDS caps flags, identifying the type of surface
    DWORD m_dwFlags;            // pusher flags
    DWORD m_dwPad02[5];

public:

    inline DWORD getChannel           (void)            {   return (m_dwChannel);      }
    inline DWORD getContextDMA        (void)            {   return (m_dwContextDMA);   }

    inline DWORD getHeap              (void)            {   return (m_dwHeap);   }
    inline DWORD getBase              (void)            {   return (m_dwBase);   }
    inline DWORD getSize              (void)            {   return (m_dwSize);   }

    inline void  setPut               (DWORD dwPut)     {   m_dwPut = dwPut;    }
    inline DWORD getPut               (void)            {   return (m_dwPut);   }

    inline DWORD getThreshold         (void)            {   return (m_dwThreshold);   }
    inline DWORD getWrapCount         (void)            {   return (m_dwWrapCount);   }
    inline DWORD getRefCount          (void)            {   return (_pdwHWRef);       }

    inline void  setSyncFlipFlag      (void)            {   m_dwFlags |=  PB_FLAG_FLIP_SYNC;    }
    inline void  clearSyncFlipFlag    (void)            {   m_dwFlags &= ~PB_FLAG_FLIP_SYNC;    }
    inline void  setSyncChannelFlag   (void)            {   m_dwFlags |=  PB_FLAG_CHANNEL_SYNC; }
    inline void  clearSyncChannelFlag (void)            {   m_dwFlags &= ~PB_FLAG_CHANNEL_SYNC; }

    inline void  setFlipSurface       (DWORD dwAddr)    {   m_dwFlipSurfaceAddr = dwAddr;   }
    inline void  setFlipSurfaceFlags  (DWORD dwFlags)   {   m_dwFlipSurfaceFlags = dwFlags;   }

    inline BOOL  isValid              (void)            {   return ((m_dwFlags & PB_FLAG_VALID) ? TRUE : FALSE);   }
    inline void  invalidate           (void)            {   m_dwFlags &= ~PB_FLAG_VALID;   }

    // prototypes for functions in nvPusher.cpp

           BOOL  allocate             (DWORD dwChannel, LPDDRAWI_DIRECTDRAW_GBL pDDGbl);
           BOOL  free                 (void);

           void  start                (BOOL bRequired);                     // kick off
           void  flush                (BOOL bWaitForHWIdle, DWORD dwMode);  // kick off & wait for idle
           void  resetPut             (void);                               // set put to base & read get
           BOOL  isIdle               (BOOL bCheckHWAlso);                  // check if idle
           void  waitForOtherChannels (void);                               // idle other channels
           void  makeSpace            (DWORD dwCount);                      // makes space for large transfers (use inc instead of adjust)

    #ifdef _NO_INLINE

           void  push                 (DWORD dwOffset, DWORD dwData);
           BOOL  adjust               (DWORD dwCount);
           void  inc                  (DWORD dwCount);

    #else  // !_NO_INLINE

    FORCE_INLINE void  push           (DWORD dwOffset, DWORD dwData)
                {
                    ((DWORD*)m_dwPut)[dwOffset] = dwData;
                };

    FORCE_INLINE BOOL  adjust         (DWORD dwCount)
                {   m_dwPut += ((dwCount) << 2);
                    if (m_dwPut >= m_dwThreshold) {
                        return getSpace();
                    }
                    return FALSE;
                }
    FORCE_INLINE void  inc            (DWORD dwCount)
                {
                    m_dwPut += ((dwCount) << 2);
                }

    #endif  // !_NO_INLINE

    inline void setObject(NvU32 subChannel,NvU32 objId)
    {
        nvAssert((subChannel >= 0) && (subChannel<=7));
        nvAssert(objId != 0);
        push(0, (subChannel << 13) | 0x40000);
        push(1, objId);
        adjust(2);
    }
private:

    // prototypes for functions in nvPusher.cpp

    inline void    setChannel         (DWORD dwChannel)                 {   m_dwChannel    = dwChannel;
                                                                            m_dwContextDMA = dwChannel+1;   }

           void    setBase            (DWORD dwBase);   // set buffer base
           void    setSize            (DWORD dwSize);   // set buffer size

           BOOL    allocateMemory     (LPDDRAWI_DIRECTDRAW_GBL pDDGbl);
           BOOL    freeMemory         (void);
           BOOL    allocateContextDMA (void);
           BOOL    freeContextDMA     (void);
           BOOL    openChannel        (void);
           BOOL    closeChannel       (void);

           FLATPTR getfpVidMem        (void);           // FLATPTR of pusher base

    inline void    syncAndFlushWC     (void);
           void    kickOff            (void);

           BOOL    getSpace           (void);
           BOOL    calcThreshold      (void);           // read get
           void    wrapAround         (void);

    #ifdef _PC_CHECK
           BOOL    validate           (DWORD dwStart, DWORD dwEnd);
    #endif
    #ifdef _PC_CAPTURE_METHOD_DATA
           DWORD   parseMethod        (DWORD dwAddress);
           void    captureState       (DWORD dwSubChannel);
    #endif

public:

    // creation and destruction

    inline         CPushBuffer        (void)            {   memset (this, 0, sizeof(*this));  }
    inline        ~CPushBuffer        (void)            {}

    // friends

    friend void    nvCelsiusILCompile_beginEnd        (DWORD dwPrimType);
    friend DWORD   nvCelsiusILCompile_inline_prim     (PNVD3DCONTEXT pContext, DWORD dwFlags);
    friend DWORD   nvCelsiusILCompile_inline_tri_list (PNVD3DCONTEXT pContext, DWORD dwFlags);
    friend DWORD   nvCelsiusILCompile_super_tri_list  (PNVD3DCONTEXT pContext, DWORD dwFlags);
    friend DWORD   nvCelsiusILCompile_vb_prim         (PNVD3DCONTEXT pContext, DWORD dwFlags);

    //friend void    nvKelvinILCompile_beginEnd         (DWORD dwPrimType);
    friend DWORD   nvKelvinILCompile_inline_prim      (PNVD3DCONTEXT pContext, DWORD dwFlags);
    //friend DWORD   nvKelvinILCompile_inline_tri_list  (PNVD3DCONTEXT pContext, DWORD dwFlags);
    //friend DWORD   nvKelvinILCompile_super_tri_list   (PNVD3DCONTEXT pContext, DWORD dwFlags);
    friend DWORD   nvKelvinILCompile_vb_prim          (PNVD3DCONTEXT pContext, DWORD dwFlags);

};

#else  // !cplusplus

typedef struct _CPushBuffer
{
    // members
    DWORD m_dwChannel;          // handle for channel used by this push buffer
    DWORD m_dwContextDMA;       // handle for context DMA
    DWORD m_dwContextDMASize;   // size of context dma allocated for this push buffer
    DWORD m_dwControlDMA;       // controlDMA structure for this channel
    DWORD m_dwPCIHeapHandle;    // pci heap handle (if any)
    DWORD m_dwPCIHeapBase;      // pci heap base (if any)
    DWORD m_dwDDGbl;            // the DDGbl with which allocated AGP memory is associated
    DWORD m_dwHeap;             // heap from which push buffer is allocated
    // bank
    DWORD m_dwBase;             // base of push buffer
    DWORD m_dwSize;             // size of push buffer
    DWORD m_dwPut;              // current put pointer
    DWORD m_dwPutLast;          // last put wrote to HW
    DWORD m_dwGet;              // cached get pointer
    DWORD m_dwThreshold;        // threshold value
    DWORD m_dwWrapCount;        // # of wrap arounds
    DWORD m_dwPad01[1];
    // bank
    DWORD m_dwFlipSurfaceAddr;  // what surface to wait on during getFrameStatus
    DWORD m_dwFlipSurfaceFlags; // DDS caps flags, identifying the type of surface
    DWORD m_dwFlags;            // pusher flags
    DWORD m_dwPad02[5];
} CPushBuffer;

#endif  // !cplusplus

#endif  // (NVARCH >= 0x04)

#undef nvAssert

#endif  //!_NVPUSHER_H

