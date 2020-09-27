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
#ifndef _nvRefCount_h
#define _nvRefCount_h

#ifndef __cplusplus
#error This file requires a C++ compiler
#endif

/*****************************************************************************
 * class CReferenceCount
 *
 * interface to shared reference count logic. also abstracts functionality to
 * work with pre-refcount HW.
 * NOTE: all functions work only in the context of a proper pDriverData
 */
class CReferenceCount
{
public:
    CReferenceCount::CReferenceCount() {
      m_dwRefCount            = 0xffffffff;
      m_dwHWRefCount          = 0xffffffff;
      m_dwNV4SurfaceMemory    = 0xffffffff;
      m_dwNV4SurfaceOffset    = 0xffffffff;
      m_dwLastPut             = 0xffffffff;
      m_dwLastWrap            = 0xffffffff;
      m_dwTimeOutValue        = 0xffffffff;
      m_dwTimeOutReload       = 0xffffffff;
    };
        // flags
public:
    const enum {
        // flags used to specify waiting protocol
        WAIT_COOPERATIVE = 0x01,
        WAIT_PREEMPTIVE  = 0x02,

        INC_LAZY         = 0x03,
        INC_IMMEDIATE    = 0x04
    };

    // members
private:
    DWORD       m_dwRefCount;             // the last reference count written to the HW
    DWORD       m_dwHWRefCount;           // the last reference count read from the HW
                                          // (conservative estimate, might be slightly out of date)
    DWORD       m_dwNV4SurfaceMemory;     // nv4 notifier emulation address
    DWORD       m_dwNV4SurfaceOffset;     // nv4 notifier emulation offset
    DWORD       m_dwLastPut;              // last put we wrote a notifier
    DWORD       m_dwLastWrap;             // last wrap count we wrote a notifier
    DWORD       m_dwTimeOutValue;         // # of writes to go before writing
    DWORD       m_dwTimeOutReload;        // # to reset timeout to

#if ((NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES2))
    CSemaphore *m_pSem;
#endif

    // methods
private:
    void nv4WriteCount (void);

    // write a reference count instruction
    inline void writeRefCount (void)
    {
        // write count
#if ((NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES2))
        if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
            (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
            (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
        {
            m_pSem->release (m_dwRefCount, &(getDC()->nvPusher), NV_DD_KELVIN, CSemaphore::SEMAPHORE_RELEASE_KELVIN_BACKEND);
        }
        else
#endif
#if (NVARCH >= 0x020)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            // kelvin acts like celsius for now
            getDC()->nvPusher.push   (0, dDrawSubchannelOffset(NV_DD_KELVIN) + NV06E_SET_REFERENCE | 0x40000);
            getDC()->nvPusher.push   (1, m_dwRefCount);
            getDC()->nvPusher.adjust (2);
        }
        else
#endif
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
            getDC()->nvPusher.push   (0, dDrawSubchannelOffset(NV_DD_CELSIUS) + NV06E_SET_REFERENCE | 0x40000);
            getDC()->nvPusher.push   (1, m_dwRefCount);
            getDC()->nvPusher.adjust (2);
        }
        else {
            nv4WriteCount();
        }
        // cache current pusher values
        m_dwLastPut  = getDC()->nvPusher.getPut();
        m_dwLastWrap = getDC()->nvPusher.getWrapCount();
    }

public:
    // conditionally increment the ref count, write it, and return it
    // INC_LAZY:      we will only write the refcount if we actually pushed
    //                something since we called this or forceInc last.
    //                also - we do not write every time, but batch some up
    // INC_IMMEDIATE: force ref count to be incremented and written
    inline DWORD inc (DWORD dwIncMode)
    {
        switch (dwIncMode) {

            case INC_LAZY:

                if ((getDC()->nvPusher.getPut()       == m_dwLastPut) &&
                    (getDC()->nvPusher.getWrapCount() == m_dwLastWrap)) {
                    return m_dwRefCount;
                }

                if (m_dwTimeOutValue) {
                    m_dwTimeOutValue--;
                    // return fake retirement date. this date won't actually get
                    // written out (or occur) until a sync is forced via wait()
                    return (m_dwRefCount+1);
                }

                // reload
                m_dwTimeOutValue = m_dwTimeOutReload;

                // fall through: write next number

            case INC_IMMEDIATE:

                // increment but never allow it to be zero. zero has special meaning
                m_dwRefCount++;
                if (m_dwRefCount > 0xFFFFFFF0) {
                    // refcount is about to roll over, flush now
                    getDC()->nvPusher.start (TRUE);
                    while (read() != (m_dwRefCount-1)) {
                        nvDelay();
                    }
                    m_dwRefCount = 1;
                    DPF ("refcount wrapped. this may be buggy!");
                    nvAssert (0);
                }

                // write and return new count
                writeRefCount();
                return (m_dwRefCount);

                break;

            default:
                break;

        }  // switch

        DPF ("CReferenceCount::inc() failure");
        dbgD3DError();
        return (0);
    }

    // gets the current HW ref count
    inline DWORD read (void)
    {
#if ((NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES2))
        if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
            (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
            (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
        {
            m_dwHWRefCount = m_pSem->read();
        }
        else
#endif
        if ((pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) ||
            (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS))
        {
            if (getDC()->nvPusher.isValid()) {
                m_dwHWRefCount = getDC()->nvPusher.getRefCount();
            }
            else {
                // force retirement
                m_dwHWRefCount = m_dwRefCount;
            }
        }
        else
        {
            if (m_dwNV4SurfaceOffset) {
                m_dwHWRefCount = *(DWORD*)(VIDMEM_ADDR((m_dwNV4SurfaceOffset + pDriverData->BaseAddress)));
            }
            else {
                // force retirement
                m_dwHWRefCount = m_dwRefCount;
            }
        }
        return m_dwHWRefCount;
    }

    // gets the shadowed HW ref count
    inline DWORD readShadow (void) const
    {
        return (m_dwHWRefCount);
    }

    // wait until hardware has reached the specified reference count or until we've timed out.
    // returns true if it successfully waits for the HW to catch up, FALSE if it times out
    inline BOOL wait (DWORD dwWaitMode, DWORD dwRetireDate, DWORD dwMaxDelay)
    {
#ifdef CAPTURE
        if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
            CAPTURE_SYNC3 sync;
            sync.dwJmpCommand         = CAPTURE_JMP_COMMAND;
            sync.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
            sync.dwExtensionID        = CAPTURE_XID_SYNC3;
            sync.dwValue              = dwRetireDate;
#if ((NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES2))
            if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
                (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
                (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
            {
                sync.dwType           = CAPTURE_SYNC3_TYPE_SEMAPHORE;
                sync.dwCtxDMAHandle   = CSemaphoreManager::SEMAPHORE_CONTEXT_DMA_IN_MEMORY;
                sync.dwOffset         = m_pSem->m_dwOffset;
            }
            else
#endif
            if ((pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) ||
                (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS))
            {
                sync.dwType           = CAPTURE_SYNC3_TYPE_REFCOUNT;
                sync.dwCtxDMAHandle   = 0;
                sync.dwOffset         = 0;
            }
            captureLog (&sync,sizeof(sync));
        }
#endif  // CAPTURE

        if (m_dwHWRefCount >= dwRetireDate) {
            return (TRUE);
        }

        // make sure we actually call forceInc() before starting a sync loop
        if (dwRetireDate > m_dwRefCount) {
            // There are 2 situations where we expect this to happen:
            // 1) dwRetireDate contains a fake value that hasn't been sent
            //    to the HW yet. We know it is m_dwRefCount+1
            // 2) nvReset occured before surface has retired. That means m_dwRefCount=0.
            // In any of these cases the next line solves the problem.
            m_dwRefCount = dwRetireDate;
            writeRefCount();
        }

        getDC()->nvPusher.start (TRUE);

        if (!dwMaxDelay) {
            // don't delay at all. just re-read the value from the HW check it
            return ((read() >= dwRetireDate) ? TRUE : FALSE);
        }

        while (dwMaxDelay) {
            switch (dwWaitMode) {
                case WAIT_COOPERATIVE:
                    NV_SLEEP;
                    break;
                case WAIT_PREEMPTIVE:
                    nvDelay();
                    break;
                default:
                    DPF ("unknown wait flags");
                    dbgD3DError();
            }
            if (read() >= dwRetireDate) {
                return (TRUE);
            }
            dwMaxDelay--;
        }

        return (FALSE);
    }

    // wait as long as required for hardware to reach the specified reference count
    inline BOOL wait (DWORD dwWaitMode, DWORD dwRetireDate)
    {
        BOOL bDone = FALSE;
        while (!bDone) {
            bDone = wait (dwWaitMode, dwRetireDate, 1000);
        }
        return (TRUE);
    }

    // wait as long as required for hardware to reach the current reference count
    inline BOOL wait (DWORD dwWaitMode)
    {
        return (wait (dwWaitMode, m_dwRefCount));
    }

public:
    BOOL create  (DWORD dwTimeOut);
    void destroy (void);
};

/*****************************************************************************
 * class CReferenceCountMap
 *
 * object that maps refcount values on to custom values.
 */
template <int cdwLogMax> class CReferenceCountMap
{
    /*
     * members
     */
protected:
// bank
    BOOL        m_bConservative;
    DWORD       m_dwHead;
    DWORD       m_dwTail;
    DWORD       m_dwLast;
    DWORD       m_dwLast2;
    DWORD       m_dwLastPut;
#if ((NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES2))
    CSemaphore *m_pSem;
    DWORD       _pad[1];
#else
    DWORD       _pad[2];
#endif

// bank
    DWORD       m_adwRefCount[1 << cdwLogMax];
// bank
    DWORD       m_adwNumber[1 << cdwLogMax];

    /*
     * methods
     */
public:
           void  put       (DWORD dwNumber);
           DWORD get       (void);
    inline DWORD getCached (void)               const { return m_dwLast; }
           BOOL  isBusy    (void);

    /*
     * construction
     */
public:
           void  create    (DWORD dwInitialNumber, BOOL bConservative);
           void  destroy   (void);
};

#endif _nvRefCount_h

