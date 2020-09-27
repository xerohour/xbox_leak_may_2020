// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
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
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
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
// ********************************* Direct 3D ******************************
//
//  Module: nvPusher.cpp
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

// #define LOG_GETPUT

#if (NVARCH >= 0x04)

#include "nvprecomp.h"
#pragma hdrstop

#ifdef _PC_CAPTURE
HANDLE hPusherFileBinary;
HANDLE hPusherFileAscii;
BOOL   bPusherCapture = FALSE;
#endif

#if IS_WINNT4
/*
 * Map System Memory Allocation to display driver functions.  Ignore heap-create/destroy.
 */
#define HeapCreate(nop, nop1, nop2)       (!NULL)
#define HeapDestroy(heap)                 (TRUE)
#define HeapAlloc(nop, nop1, size)            EngAllocMem(FL_ZERO_MEMORY, (size), ALLOC_TAG)
#define HeapFree(nop, nop1, ptr)              { \
    if (ptr != NULL)            \
    {                           \
    EngFreeMem((ptr));          \
    }                           \
}
#endif

//---------------------------------------------------------------------------

#ifdef LOG_GETPUT
// Store put/get/time

#define MAX_INDEX 25000

DWORD dwGet[MAX_INDEX];
DWORD dwPut[MAX_INDEX];
LARGE_INTEGER dwTime[MAX_INDEX];
DWORD dwIndex = 0;

void SaveData(void)
{
    static int FileNum = 0;
    char buff[128];

    nvSprintf (buff, "GPData%02d.txt", FileNum);
    FileNum++;
    HANDLE hFile = NvCreateFile(buff, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD dwWritten;
    LARGE_INTEGER dwFreq;
    QueryPerformanceFrequency(&dwFreq);
    nvSprintf (buff, "%9s %8s %8s Freq = %u\n", "Time", "Get", "Put", dwFreq.LowPart);
    NvWriteFile (hFile, buff, nvStrLen(buff), &dwWritten, NULL);
    for(DWORD i = 0; i < dwIndex; i++) {
        nvSprintf (buff, "%9u %08u %08u\n", dwTime[i].LowPart, dwGet[i], dwPut[i]);
        WriteFile (hFile, buff, nvStrLen(buff), &dwWritten, NULL);
    }
    NvCloseHandle(hFile);
    dwIndex = 0;
    _asm int 3
}

#define STORE_GETPUT()                                                                              \
{                                                                                                   \
    DWORD dwHWGet = _pdwHWGet;                                                                      \
    dwGet[dwIndex] = dwHWGet;                                                                       \
    dwPut[dwIndex] = m_dwPutLast - m_dwBase;                                                        \
    QueryPerformanceCounter((LARGE_INTEGER*)&dwTime[dwIndex]);                                      \
    dwIndex++;                                                                                      \
    if (dwIndex == MAX_INDEX) {                                                                     \
        SaveData();                                                                                 \
    }                                                                                               \
}

#endif // LOG_GETPUT

//---------------------------------------------------------------------------

// pusher debug helpers

#ifdef _PC_BREAK
DWORD dwPutOffsetBreak = ~0;
#endif //_PC_BREAK
#ifdef _PC_CHECK
DWORD dwPusherCount = ~0;
#endif //_PC_CHECK

//---------------------------------------------------------------------------
// public functions
//---------------------------------------------------------------------------

// CPushBuffer::allocate

BOOL CPushBuffer::allocate
(
    DWORD dwChannel
)
{
    setChannel (dwChannel);

    if (allocateMemory()     &&
        allocateContextDMA() &&
        openChannel())
    {
        m_dwFlags |= PB_FLAG_VALID;
        resetPut();
        return (TRUE);
    }

    return (FALSE);
}

//---------------------------------------------------------------------------

// CPushBuffer::free
// returns TRUE on success, FALSE on failure

BOOL CPushBuffer::free
(
    void
)
{
#if IS_WIN9X
    nvAssert (!global.b16BitCode);
#endif

    closeChannel();
    freeContextDMA();
    freeMemory();

    // mark buffer as being invalid
    m_dwFlags &= ~PB_FLAG_VALID;

    return (TRUE);
}

//---------------------------------------------------------------------------

// CPushBuffer::start
// kicks off the current push buffer. if bRequired is FALSE then this
// routine will only kick it off when there is NVPUSHER_MINIMUM bytes
// in the buffer

void CPushBuffer::start
(
    BOOL bRequired
)
{
#ifdef _PC_CHECK
#if IS_WIN9X
    if (!global.b16BitCode)
#endif
    {
        if (!isValid()) {
            DPF ("caught attempt to start an invalid push buffer.");
            __asm int 3;
            return;
        }
        if (dwPusherCount != ~0) {
            DPF ("adjust() or inc() not called, data lost.");
            DPF ("F10 to return to caller.");
            __asm int 3;
            return;
        }
    }
#endif //_PC_CHECK

    // return if nothing has been buffered
    if (m_dwPutLast == m_dwPut) {
        return;
    }

    // handle optional case
    if (!bRequired) {
        if ((m_dwPut - m_dwPutLast) < NVPUSHER_MINIMUM) {
            return;
        }
    }

    // handle proper kick-off
    kickOff();
}

//---------------------------------------------------------------------------

// CPushBuffer::flush
// kicks off the push buffer and spin until it is consumed. If bWaitForHWIdle
// is specified, we also wait for the HW to go idle afterwards

void CPushBuffer::flush
(
    BOOL  bWaitForHWIdle,
    DWORD dwMode
)
{
    // kick off buffer
    start (TRUE);

    //
    // capture driver
    //
#ifdef CAPTURE
    if (!global.b16BitCode)
    {
        if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
            CAPTURE_SYNC3 sync;
            sync.dwJmpCommand         = CAPTURE_JMP_COMMAND;
            sync.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
            sync.dwExtensionID        = CAPTURE_XID_SYNC3;
            sync.dwType               = bWaitForHWIdle ? CAPTURE_SYNC3_TYPE_CHIPIDLE : CAPTURE_SYNC3_TYPE_ZEROCOMMANDS;
            sync.dwCtxDMAHandle       = 0;
            sync.dwOffset             = 0;
            sync.dwValue              = 0;
            captureLog (&sync,sizeof(sync));
        }
    }
#endif

    // wait for idle
            #ifdef NV_PROFILE_PUSHER
            if (!global.b16BitCode) {
                NVP_START(NVP_T_FLUSH);
            }
            #endif

    switch (dwMode) {

        case FLUSH_WITH_DELAY:
            while (!isIdle(bWaitForHWIdle)) {
                nvSpin(_systemInfo.dwSpinLoopCount);
            }
            break;

        case FLUSH_HEAVY_POLLING:
            while (!isIdle(bWaitForHWIdle));
            break;

        default:
            DPF ("unknown flush mode");
            dbgError("NV Error!");
            break;

    }  // switch

            #ifdef NV_PROFILE_PUSHER
            if (!global.b16BitCode) {
                NVP_STOP(NVP_T_FLUSH);
                nvpLogTime (NVP_T_FLUSH,nvpTime[NVP_T_FLUSH]);
            }
            #endif
}

//---------------------------------------------------------------------------

// CPushBuffer::resetPut
// called to initialize a new, empty push buffer.
// sets up the put pointer, get pointer, and threshold

void CPushBuffer::resetPut
(
    void
)
{
#ifdef _PC_CHECK
    if (!isValid()) {
        DPF ("invalid push buffer in resetPut");
        __asm int 3;
        return;
    }
    if (dwPusherCount != ~0) {
        DPF ("Pusher not kicked off, data lost.");
        DPF ("F10 to return to caller.");
        __asm int 3;
        return;
    }
#endif //_PC_CHECK

    // initialize to base temporarily to make CPushBuffer::calcThreshold happy
    m_dwPut = m_dwPutLast = m_dwBase;
    calcThreshold();
    // set put to get since technically, there is no strict guarantee
    // that the get pointer starts out at the beginning of a new buffer
    m_dwPut = m_dwPutLast = m_dwGet;

#ifdef _PC_PRINT
    DPF ("CPushBuffer::resetPut: base = %08x",m_dwBase);
    DPF ("                       put  = %08x",m_dwPut);
    DPF ("                       get  = %08x",m_dwGet);
#endif
}

//---------------------------------------------------------------------------

// CPushBuffer::isIdle
// returns TRUE if the push buffer is idle

BOOL CPushBuffer::isIdle
(
    BOOL bCheckHWAlso
)
{
    // pDriverData, dwFullScreenDOSStatus and the flag value are not available
    // on NT4.
#if !IS_WINNT4
    // degenerate case: monitor shutdown could occur with dma pending
    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        return TRUE;
    }
#endif

    // is push buffer consumed?
    // STORE_GETPUT();
    m_dwGet = m_dwBase + _pdwHWGet;
    if (m_dwGet != m_dwPutLast) {
        return FALSE;
    }

    // is HW idle?
    if (bCheckHWAlso) {

        // check for stuff pending in the cache
        // (DCT clipping test fails on Win2K dual CPU without this test)
        if (((_ffCache1Push1  & 0x1f) == _dmaPusherChannelIndex) &&
            ((_ffCache1Status & 0x10) == 0)) {
            // we own the cache and it's got stuff in it
            return FALSE;
        }

        // check the graphics engine itself
        if (_pbHWBusy) {
            return FALSE;
        }

    }

    // idle
    return TRUE;
}

//---------------------------------------------------------------------------

// CPushBuffer::waitForOtherChannels
// waits for other channels to go idle before returning

void CPushBuffer::waitForOtherChannels
(
    void
)
{
    volatile DWORD *npNvBase;
    DWORD *pDW;

#ifdef _PC_CHECK
#if IS_WIN9X
    if (!global.b16BitCode)
#endif
    {
        if (!_pDriverData) {
            DPF ("pDriverData not yet initialized in CPushBuffer::waitForOtherChannels.");
            __asm int 3;
            return;
        }
    }
#endif //_PC_CHECK
#ifdef CAPTURE
    if (!global.b16BitCode) {
        if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
            CAPTURE_SYNC3 sync;
            sync.dwJmpCommand         = CAPTURE_JMP_COMMAND;
            sync.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
            sync.dwExtensionID        = CAPTURE_XID_SYNC3;
            sync.dwType               = CAPTURE_SYNC3_TYPE_CHANNELS;
            sync.dwCtxDMAHandle       = 0;
            sync.dwOffset             = 0;
            sync.dwValue              = 0;
            captureLog (&sync,sizeof(sync));
        }
    }
#endif

    m_dwFlags &= ~PB_FLAG_CHANNEL_SYNC;

    npNvBase = _nvBaseFlat;
    pDW      = _pCurrentChannelID;

    if (*pDW != _dmaPusherChannelIndex)
    {
        *pDW = _dmaPusherChannelIndex;
        while ((npNvBase[(NV_PFIFO_DMA >> 2)] & (~(1<<_dmaPusherChannelIndex))) != 0);
        if ((npNvBase[NV_PFIFO_CACHE1_PUSH1 >> 2] & NV_PFIFO_CACHE1_PUSH1_MODE_DMA_BIT)
         && ((npNvBase[NV_PFIFO_CACHE1_PUSH1 >> 2] & 0x1F) != (WORD)_dmaPusherChannelIndex))
        {
            while ((npNvBase[NV_PFIFO_CACHE1_DMA_PUSH >> 2] & NV_PFIFO_CACHE1_DMA_PUSH_BUFFER_EMPTY_BIT) == 0);
        }
    }
}

//---------------------------------------------------------------------------

// CPushBuffer::makeSpace
// makes space for at least dwCount words in the push buffer

void CPushBuffer::makeSpace
(
    DWORD dwCount
)
{
    BOOL  bFirst = TRUE;
    DWORD dwMax  = m_dwBase                     // start
                 + m_dwSize                     // + base         -> end
                 - NVPUSHER_THRESHOLD_SIZE * 4  // - assumed      -> largest allowed end (with zero space)
                 - dwCount * 4;                 // - dwCount * 4  -> largest allowed end (with dwCount words)

#ifdef _PC_CHECK
    if (!isValid()) {
        DPF ("caught attempt to make space in an invalid push buffer.");
        __asm int 3;
        return;
    }
    if (dwPusherCount != ~0) {
        DPF ("Pusher not kicked off, data lost.");
        DPF ("F10 to return to caller.");
        __asm int 3;
        return;
    }
#endif //_PC_CHECK

            #ifdef NV_PROFILE_PUSHER
            NVP_START(NVP_T_MAKESPACE);
            #endif
    // see if this size can fit between put and end of buffer
    if (m_dwPut >= dwMax) {
        // wrap around
        wrapAround();
#ifdef _PC_CHECK
        dwPusherCount = ~0; // reset
#endif //_PC_CHECK
        calcThreshold();
        bFirst = FALSE;
    }

    // wait until threshold is far enough away
    dwCount *= 4; // words -> bytes
    while (m_dwThreshold < (m_dwPut + dwCount)) {
        if (bFirst) {
            bFirst = FALSE;
        }
        else {
            nvSpin(_systemInfo.dwSpinLoopCount);
        }
        calcThreshold();
    }
            #ifdef NV_PROFILE_PUSHER
            NVP_STOP(NVP_T_MAKESPACE);
            nvpLogTime (NVP_T_MAKESPACE,nvpTime[NVP_T_MAKESPACE]);
            #endif
}

//---------------------------------------------------------------------------

#ifdef _NO_INLINE

// CPushBuffer::push
// push dwData at put + offset (in words)

void CPushBuffer::push
(
    DWORD dwOffset,
    DWORD dwData
)
{
#ifdef _PC_CHECK
    if (!isValid()) {
        DPF ("caught attempt to push data to an invalid push buffer.");
        __asm int 3;
        return;
    }
    if (dwOffset) {
        dwPusherCount ++;
        if (dwPusherCount != dwOffset) {
            DPF ("CPushBuffer::push: gap in offset sequence.");
            DPF ("ZAP or F5 to continue.");
            __asm int 3;
        }
    }
    else {
        dwPusherCount = 0;
        //memset ((void*)m_dwPut,0x55,NVPUSHER_THRESHOLD_SIZE * 4);
    }
#endif
#ifdef _PC_BREAK
    if ((m_dwPut - m_dwBase + dwOffset * 4) == dwPutOffsetBreak) {
        DPF ("CPushBuffer::push: desired put reached.");
        DPF ("ZAP or F5 to continue.");
        __asm int 3;
    }
#endif //_PC_BREAK

    ((DWORD*)m_dwPut)[dwOffset] = dwData;
}

//---------------------------------------------------------------------------

// CPushBuffer::adjust
// advance put by dwCount words

BOOL CPushBuffer::adjust
(
    DWORD dwCount
)
{
    BOOL bKickedOff;

#ifdef _PC_CHECK
    if (!isValid()) {
        DPF ("caught attempt to adjust an invalid push buffer.");
        __asm int 3;
        return (FALSE);
    }

    if (dwCount) {

        // check for push / adjust mismatch
        if (dwCount != (dwPusherCount + 1)) {
            dwPusherCount = ~0; // reset
            DPF ("CPushBuffer::adjust: incorrect amount adjusted.");
            DPF ("                     current object address: %08x",m_dwPut);
            DPF ("F10 to return to caller.");
            __asm int 3;
            return (FALSE);
        }

        else {
            // check for proper object size
            //  complete objects must be pushed since this may kick-off unexpectedly
            DWORD dwAddr = m_dwPut;
            DWORD dwCnt  = dwCount;

            while (dwCnt) {
                // get push buffer data
                DWORD dwValue = *(DWORD*)dwAddr;

                // process JMP command
                if (dwValue & 0x20000000) {
                    // extract jmp dest
                    DWORD dwDest = dwAddr & 0x1fffffff;
                    // test valid
                    //  - for now we assume only zero to be valid since we do not
                    //    jump anywhere else. Later we can make the test simply check
                    //    that dwDest lies within limits
                    if (dwDest > 0) {// (dwDest < m_dwSize)
                        dwPusherCount = ~0; // reset
                        DPF ("CPushBuffer::adjust: JUMP command out of range.");
                        DPF ("                     JUMP target    : %08x", dwDest);
                        DPF ("                     object address : %08x", dwAddr);
                        DPF ("F10 to return to caller.");
                        __asm int 3;
                        return (FALSE);
                    }
                    // apply jump
                    dwCnt  -= 1;
                    dwAddr  = m_dwBase + dwDest;
                }

                else if (dwValue & 0x00000002) {
                    // call command. just skip over it
                    // (maybe later we can be fancy by sanity checking the function)
                    dwCnt  -= 1;
                    dwAddr += 4;
                }

                else {
                    DWORD dwAdd = 1 + ((dwValue >> 18) & 0x7f);  // data count + 1 for the method itself
                    dwCnt -= dwAdd;
                    if (dwCnt > dwCount) { // includes going negative
                        dwPusherCount = ~0; // reset
                        DPF ("CPushBuffer::adjust: partial or incorrect object pushed.");
                        DPF ("                     object address: %08x", dwAddr);
                        DPF ("F10 to return to caller.");
                        __asm int 3;
                        return (FALSE);
                    }
                    dwAddr += dwAdd * 4;
                }
            }
        }
    }  // if (dwCount)...

    dwPusherCount = ~0; // reset
#endif

    // increment put
    inc (dwCount);

    // spin until we have enough space in the push buffer
    bKickedOff = getSpace();

#ifdef _PC_FLUSH_ON_ADJUST
    flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);
#endif // _PC_FLUSH_ON_ADJUST

    return (bKickedOff);
}

//---------------------------------------------------------------------------

// CPushBuffer::inc
// increments put pointer

void CPushBuffer::inc
(
    DWORD dwCount
)
{
#ifdef _PC_CHECK
    if (!isValid()) {
        DPF ("caught attempt to increment an invalid push buffer.");
        __asm int 3;
        return;
    }
    DWORD dwDest = m_dwPut + (dwCount << 2);
    /*
    -- DO NOT REMOVE -- tests for put running past get but slows down too much
    DWORD dwGet = m_dwBase + _pdwHWGet;
    if (dwGet > m_dwPut)
    {
        if (dwDest >= dwGet)
        {
            DPF ("CPushBuffer::inc: increment past end of push buffer (current get).");
            DPF ("                  current put address: %08x",m_dwPut);
            DPF ("F10 to return to caller.");
            __asm int 3;
            return;
        }
    }
    */
    if (dwDest >= m_dwBase + m_dwSize) {
        DPF ("CPushBuffer::inc: increment past end of push buffer (top).");
        DPF ("                  current put address: %08x", m_dwPut);
        DPF ("F10 to return to caller.");
        __asm int 3;
        return;
    }
    dwPusherCount = ~0; // reset
#endif //_PC_CHECK
    m_dwPut += (dwCount << 2);
}

#endif  // _NO_INLINE

//---------------------------------------------------------------------------
// private functions
//---------------------------------------------------------------------------

// CPushBuffer::setBase
// set the pusher base.
// put & get are discarded, leaving the push buffer in an invalid state;
// resetPut must be called subsequently to re-initialize things

void CPushBuffer::setBase
(
    DWORD dwBase
)
{
#ifdef _PC_CHECK
    if (dwPusherCount != ~0) {
        DPF ("Pusher not kicked off, data lost.");
        DPF ("F10 to return to caller.");
        __asm int 3;
        return;
    }
#endif //_PC_CHECK

    m_dwBase      = dwBase;
    m_dwGet       = 0;
    m_dwPut       = 0;
    m_dwPutLast   = 0;
    m_dwThreshold = 0;

#ifdef _PC_PRINT
    DPF ("CPushBuffer::setBase: base = %08x", dwBase);
#endif
}

//---------------------------------------------------------------------------

// CPushBuffer::setSize
// set the pusher size.
// put & get are discarded, leaving the push buffer in an invalid state;
// resetPut must be called subsequently to re-initialize things

void CPushBuffer::setSize
(
    DWORD dwSize
)
{
#ifdef _PC_CHECK
    if (!_pDriverData) {
        DPF ("pDriverData not yet initialized in CPushBuffer::setSize.");
        __asm int 3;
        return;
    }
    if (dwPusherCount != ~0) {
        DPF ("Pusher not kicked off, data lost.");
        DPF ("F10 to return to caller.");
        __asm int 3;
        return;
    }
#endif //_PC_CHECK

    m_dwSize      = dwSize;
    m_dwGet       = 0;
    m_dwPut       = 0;
    m_dwPutLast   = 0;
    m_dwThreshold = 0;

#ifdef _PC_PRINT
    DPF ("CPushBuffer::setSize: size = %08x",dwSize);
#endif
}

//---------------------------------------------------------------------------

// CPushBuffer::allocateMemory

BOOL CPushBuffer::allocateMemory
(
    void
)
{
    // check to see if the push buffer was already allocated (probably via updateNonLocalHeap)
    if (!m_dwBase) {
        // Ensure size of pushbuffer is initialized properly. Previously done in
        // DriverInit, but under NT DriverInit is only called once when GLOBALDATA
        // is created. Push buffer size is being reset on every mode change.
        setSize (_systemInfo.dwRecommendedPushBufferSize);

        if ((_pDriverData->regPBLocale == NV_REG_SYS_PB_LOCATION_VIDEO) ||
            (_systemInfo.dwPerformanceStrategy & PS_PUSH_VID)) {

            // allocate push buffer in video memory
            DWORD dwStatus, dwVidMemBase;
            NVHEAP_ALLOC (dwStatus, dwVidMemBase, m_dwSize, TYPE_IMAGE);
            nvAssert (dwStatus == 0);
            setBase (VIDMEM_ADDR(dwVidMemBase));

            m_dwHeap &= ~(CSimpleSurface::HEAP_AGP | CSimpleSurface::HEAP_PCI);
            m_dwHeap |=  CSimpleSurface::HEAP_VID;

        }

#if (!IS_WINNT4)	// AGP pushbuffer is not (yet) available on WinNT4

        else if (((_pDriverData->regPBLocale == NV_REG_SYS_PB_LOCATION_AGP) ||
                  (_pDriverData->regPBLocale == NV_REG_SYS_PB_LOCATION_DEFAULT))
                 &&
                 (_pDriverData->GARTLinearBase != NULL)) {

            // allocate push buffer from AGP memory
#ifdef NV_AGP			
            DWORD dwAgpAddr = (DWORD)nvAGPAlloc(m_dwSize);
            if (dwAgpAddr == ERR_DXALLOC_FAILED) {
                DPF ("CPushBuffer::allocateMemory: warning - failed to allocate required");
                DPF ("                             amount of AGP mem for pushbuffer!");
            }
            else {
                setBase (dwAgpAddr);
            }
#else
            DWORD dwAgpOffset = (DWORD)nvAGPAlloc(m_dwSize);
            if (dwAgpOffset == ERR_DXALLOC_FAILED) {
                DPF ("CPushBuffer::allocateMemory: warning - failed to allocate required");
                DPF ("                             amount of AGP mem for pushbuffer!");
            }
            else {
                setBase (AGPMEM_ADDR(dwAgpOffset));
            }

            if (!m_dwBase) {
                // mark AGP as invalid so that further allocations go to PCI memory
#ifdef WINNT
                NvWin2KUnmapAgpHeap(_pDriverData);
#endif // !WINNT
                _pDriverData->GARTLinearBase = 0;
            }
#endif

            m_dwHeap &= ~(CSimpleSurface::HEAP_PCI | CSimpleSurface::HEAP_VID);
            m_dwHeap |=  CSimpleSurface::HEAP_AGP;
        }
#endif // NT4

        if (!m_dwBase) {
            // we still haven't allocated a push buffer. this is either because we failed to do so
            // or because we didn't want it in video / AGP. either way, try now to put it in PCI

            if (m_dwPCIHeapHandle == 0) {

                // not allocated for PCI yet
                m_dwPCIHeapHandle = (DWORD) HeapCreate (HEAP_SHARED, m_dwSize, 0);
                m_dwPCIHeapBase   = (DWORD) HeapAlloc  ((HANDLE)m_dwPCIHeapHandle, HEAP_ZERO_MEMORY, m_dwSize);

                if (m_dwPCIHeapBase) {
                    // align things properly. both the base and limit need to be aligned to make nv10 happy.
                    DWORD dwAlignedBase  = (m_dwPCIHeapBase + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN;
                    DWORD dwAlignedLimit = (m_dwPCIHeapBase + m_dwSize) & ~NV_TEXTURE_OFFSET_ALIGN;
                    DWORD dwAlignedSize  = dwAlignedLimit - dwAlignedBase;
                    setBase (dwAlignedBase);
                    setSize (dwAlignedSize);
                    m_dwHeap &= ~(CSimpleSurface::HEAP_AGP | CSimpleSurface::HEAP_VID);
                    m_dwHeap |=  CSimpleSurface::HEAP_PCI;
                } else {
                    // may see system alloc failure in win2k + 1G RAM
                    HeapDestroy((HANDLE)m_dwPCIHeapHandle);
                    m_dwPCIHeapHandle = 0;

                    // last resort, allocate push buffer in video memory
                    DWORD dwStatus, dwVidMemBase;
                    NVHEAP_ALLOC (dwStatus, dwVidMemBase, m_dwSize, TYPE_IMAGE);
                    nvAssert (dwStatus == 0);
                    setBase (VIDMEM_ADDR(dwVidMemBase));

                    m_dwHeap &= ~(CSimpleSurface::HEAP_AGP | CSimpleSurface::HEAP_PCI);
                    m_dwHeap |=  CSimpleSurface::HEAP_VID;
                }
            }
            else {
                DPF ("what the heck does this mean??");
                dbgError("NV Error!");
            }

        }

    }  // !m_dwBase

    return (m_dwBase ? TRUE : FALSE);
}

//---------------------------------------------------------------------------

// CPushBuffer::freeMemory

BOOL CPushBuffer::freeMemory
(
    void
)
{
    if (m_dwBase) {

#if (!IS_WINNT4)	// AGP pushbuffer is not (yet) available on WinNT4

        if (_pDriverData->GARTLinearBase && (m_dwHeap & CSimpleSurface::HEAP_AGP)) {
            nvAGPFree((void*)getfpVidMem());
        }
        else 
#endif // NT4
		if (m_dwHeap & CSimpleSurface::HEAP_VID)
		{
            NVHEAP_FREE (getfpVidMem());
        }
        else {
            // DMA push buffer in system mem
            HeapFree    ((HANDLE) m_dwPCIHeapHandle, 0, (PVOID) m_dwPCIHeapBase);
            HeapDestroy ((void*) m_dwPCIHeapHandle);
            m_dwPCIHeapHandle = m_dwPCIHeapBase = 0;
        }

        setBase (0);

    }

    return (TRUE);
}

//---------------------------------------------------------------------------

// CPushBuffer::allocateContextDMA

BOOL CPushBuffer::allocateContextDMA
(
    void
)
{
    NvU32 status;

    if (!m_dwContextDMASize) {
        status = NvRmAllocContextDma (_dwRootHandle,
                                 m_dwContextDMA,
                                 NV01_CONTEXT_DMA,
                                 DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_ONLY) |
                                 DRF_DEF(OS03, _FLAGS, _LOCKED, _ALWAYS) |
                                 DRF_DEF(OS03, _FLAGS, _COHERENCY, _CACHED),
                                 (PVOID) m_dwBase,
                                 m_dwSize - 1);
        if (status!=0)
        {
            DPF("Failed... Status = 0x%08x",status);
            return(FALSE);
        }
        m_dwContextDMASize = m_dwSize;
    }
    return (TRUE);
}

//---------------------------------------------------------------------------

// CPushBuffer::freeContextDMA

BOOL CPushBuffer::freeContextDMA
(
    void
)
{
    if (m_dwContextDMASize) {
        NvRmFree (_dwRootHandle, _dwRootHandle, m_dwContextDMA);
        m_dwContextDMASize = 0;
    }
    return (TRUE);
}

//---------------------------------------------------------------------------

// CPushBuffer::openChannel

BOOL CPushBuffer::openChannel
(
    void
)
{
    if (!m_dwControlDMA) {
        DWORD dwChannelFlavor = (_systemInfo.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA) ?
                                    NV20_CHANNEL_DMA :
                                    (_systemInfo.dwNVClasses1 & NVCLASS1_006E_CHANNELDMA) ?
                                        NV10_CHANNEL_DMA :
                                        NV04_CHANNEL_DMA;
        if (NvRmAllocChannelDma (_dwRootHandle, _hDevice,
                                 m_dwChannel, dwChannelFlavor, 0,
                                 m_dwContextDMA, 0,
                                 (PVOID*)&m_dwControlDMA)) {
            return(FALSE);
        }
    }
    return (TRUE);
}

//---------------------------------------------------------------------------

// CPushBuffer::closeChannel

BOOL CPushBuffer::closeChannel
(
    void
)
{
    if (m_dwControlDMA) {
        NvRmFree (_dwRootHandle, _hDevice, m_dwChannel);
        m_dwControlDMA = 0;
    }
    return (TRUE);
}

//---------------------------------------------------------------------------

// CPushBuffer::getfpVidMem
// returns the FLATPTR base of the push buffer

FLATPTR CPushBuffer::getfpVidMem
(
    void
)
{
#ifdef WINNT
    if (!m_dwBase) { return (NULL); }
#ifndef NV_AGP
    if (m_dwHeap & CSimpleSurface::HEAP_AGP) {
        return (m_dwBase - _pDriverData->GARTLinearBase);
    }
#endif
    else if (m_dwHeap & CSimpleSurface::HEAP_VID) {
#if IS_WINNT4
        return (m_dwBase - (ULONG) m_ppdev->pjFrameBufbase);
#else
        return (m_dwBase - (ULONG) ppdev->pjFrameBufbase);
#endif
    }
    else
#endif // WINNT
    return (m_dwBase);
}


//---------------------------------------------------------------------------

// CPushBuffer::syncAndFlushWC
// do a WC flush

inline void CPushBuffer::syncAndFlushWC (void)
{
    waitForOtherChannels();

    // wait for flip to complete if the flag is set
    if (m_dwFlags & PB_FLAG_FLIP_SYNC) {
#if IS_WINNT4
        // Flip sync is not implemented on NT4 -- need to port getFrameStatus...
        nvAssert(FALSE);
#else
        while (getFlipStatus (m_dwFlipSurfaceAddr, m_dwFlipSurfaceFlags) != DD_OK) {
            nvSpin(_systemInfo.dwSpinLoopCount);
        }
        m_dwFlags &= ~PB_FLAG_FLIP_SYNC;
#endif
    }

    // flush WC (Write Combine - not Water Closet)
    if (_hasSFENCE) {
        __asm _emit 0x0f
        __asm _emit 0xae
        __asm _emit 0xf8 // sfence
    }
    else {
        __asm mov edx,0x3d0
        __asm xor eax,eax
        __asm out dx,al
    }
}

//---------------------------------------------------------------------------

// CPushBuffer::kickOff
// writes put

void CPushBuffer::kickOff
(
    void
)
{
    DWORD dwPutOffset;

#ifdef _PC_PRINTPUT
#if IS_WIN9X
    if (!global.b16BitCode)
#endif
        {
        DPF ("CPushBuffer::kickOff: put = %08x (rel %08x)", m_dwBase + dwPutOffset, dwPutOffset);
    }
#endif

#ifdef _PC_CHECK
#if IS_WIN9X
    if (!global.b16BitCode)
#endif
        {
        if (!validate (m_dwPutLast, m_dwPut)) {
            DPF ("CPushBuffer::kickOff: warning - put was not written due to parse error.");
            return;
        }
    }
#endif

#if 0
    {
        static HANDLE h = 0;

        static DWORD totl = 0;
        static DWORD last = 0;
        if (!last) last = GetTickCount();
        DWORD now = GetTickCount();

        totl += (m_dwPut > m_dwPutLast)
                ? (m_dwPut - m_dwPutLast)
                : 0;

        //if ((now - last) > 50)
        {
            DWORD x = now - last;
            DWORD y = totl;
            totl = 0;

            last = now;

            if (!h) {
                h = NvCreateFile("c:\\data.txt",GENERIC_WRITE,0,0,OPEN_ALWAYS,0,0);
                if (h == INVALID_HANDLE_VALUE) {
                    h = 0;
                }
                else {
                    SetFilePointer (h,0,NULL,FILE_END);
                }
            }

            if (h) {
                char sz[256];
                wsprintf (sz,"%d\t%d\r\n",x,y);
                NvWriteFile (h,sz,strlen(sz),&x,NULL);
            }

            static DWORD t = 0;
            if (!t) t = GetTickCount();
            DWORD n = GetTickCount();

            if ((n - t) > 1000) {
                t = n;
                NvCloseHandle (h);
                h = 0;
            }
        }
    }
#endif // 0

    // write put & update last put address
    syncAndFlushWC();

#ifdef CAPTURE
    if (!global.b16BitCode) {
        if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
            if (m_dwPutLast < m_dwPut) {
                captureLog ((void*)m_dwPutLast, m_dwPut - m_dwPutLast);
            }
        }
    }
#endif

    dwPutOffset = m_dwPut - m_dwBase;
    m_dwPutLast = m_dwPut;

//    STORE_GETPUT();

            #ifdef NV_PROFILE_PUSHER
            if (!global.b16BitCode) {
                DWORD dwGetOffset = _pdwHWGet;
                DWORD dwPending;
                if (dwGetOffset > dwPutOffset) {
                    dwPending = m_dwSize - (dwGetOffset - dwPutOffset);
                }
                else {
                    dwPending = dwPutOffset - dwGetOffset;
                }
                // static int   first = 1;
                // static DWORD dwLast;
                // if (first)
                // {
                    // first = 0;
                // }
                // else
                // {
                    // NVP_STOP(NVP_C_VELOCITY);
                // }
                // DWORD dwNow = dwGetOffset;
                // double vel  = double(int(dwNow) - int(dwLast)) * 1000.0 * double(global.dwProcessorSpeed) / double(nvpTime[NVP_C_VELOCITY]);
                // if (vel < 0) vel = 0;
                // if (vel > 10000) vel = 10000;
                // nvpLogCount (NVP_C_VELOCITY,DWORD(vel));

                 if (nvpGetLogPusher())
                 {
                    nvpLogCount (NVP_C_PENDINGSIZE,dwPending);
                    
                 }
                // nvpLogCount (NVP_C_PUT,dwPutOffset);
                // nvpLogCount (NVP_C_GET,dwGetOffset);
                // dwLast = dwNow;
                // NVP_START(NVP_C_VELOCITY);
            }
            #endif  // NV_PROFILE_PUSHER

    _pdwHWPut = dwPutOffset;

            #ifdef NV_PROFILE
            if (!global.b16BitCode) {
            #ifdef NV_PROFILE_PUSHER
                if (nvpGetLogPusher())
            #endif
                nvpLogEvent (NVP_E_PUT);
            }
            #endif  // NV_PROFILE

//    memset((void*)m_dwPutLast, 0xFA, (NVPUSHER_THRESHOLD_SIZE - 1)*4);
}

//---------------------------------------------------------------------------

// CPushBuffer::getSpace
// spins until we have space in push buffer

BOOL CPushBuffer::getSpace
(
    void
)
{
    #ifdef NV_PROFILE_PUSHER
    NVP_START(NVP_T_ADJUST);
    #endif

    BOOL bFirst     = TRUE;
    BOOL bKickedOff = FALSE;

    while (m_dwPut >= m_dwThreshold) {
        if (bFirst) {
            bFirst = FALSE;
        }
        else {
            nvSpin(_systemInfo.dwSpinLoopCount);
        }
        bKickedOff |= calcThreshold();
    }

    #ifdef NV_PROFILE_PUSHER
    NVP_STOP(NVP_T_ADJUST);
    nvpLogTime (NVP_T_ADJUST,nvpTime[NVP_T_ADJUST]);
    #endif

    return (bKickedOff);
}

//---------------------------------------------------------------------------

// CPushBuffer::calcThreshold
// recalculate pusher threshold

BOOL CPushBuffer::calcThreshold
(
    void
)
{
    BOOL  bKickedOff;
    DWORD dwMax;

#ifdef _PC_CHECK
    if (!isValid()) {
        DPF ("invalid push buffer in CPushBuffer::calcThreshold");
        __asm int 3;
        return (FALSE);
    }
#endif //_PC_CHECK

    dwMax = m_dwBase + m_dwSize - (4*NVPUSHER_THRESHOLD_SIZE);

    // have we crossed the largest possible threshold?
    if (m_dwPut >= dwMax) {
        // wrap push buffer around
        wrapAround();
#ifdef _PC_CHECK
        dwPusherCount = ~0; // reset
#endif //_PC_CHECK
        bKickedOff    = TRUE;
    }

    // read get pointer
//    STORE_GETPUT();
    m_dwGet = m_dwBase + _pdwHWGet;

    // figure out who is chasing whom
    if (m_dwPut >= m_dwGet) {
        // get is chasing put -- .....G*******P....T.
        m_dwThreshold = dwMax;
    }
    else {
        // put is chasing get -- *******P...T.G*****.
        // note that the threshold can actually be less that the push buffer base. this is okay.
        m_dwThreshold = m_dwGet - (4*NVPUSHER_THRESHOLD_SIZE);
    }

    return bKickedOff;
}

//---------------------------------------------------------------------------

// CPushBuffer::wrapAround
// wraps pushbuffer around. This is the only place where we can get put
// to chase get.

void CPushBuffer::wrapAround
(
    void
)
{
            #ifdef NV_PROFILE_PUSHER
            NVP_START(NVP_T_WRAPAROUND);
            #endif

    // in the case where get already points to the push buffer base
    // we have to first get it going before we can write put to the
    // base again. This is because writing put to the same value
    // as get will do nothing instead of running through the whole
    // push buffer like we want it to.
    if (m_dwGet == m_dwBase)
    {
        // check get again
        // STORE_GETPUT();
        m_dwGet = m_dwBase + _pdwHWGet;
        if (m_dwGet == m_dwBase) {
            // kick off buffer
            kickOff();
            // wait until get has progressed at least one word
            while (m_dwGet == m_dwBase) {
                // STORE_GETPUT();
                m_dwGet = m_dwBase + _pdwHWGet;
            }
        }
    }

    // wait for get to wrap around

    // the following line used to be:
    // while ((m_dwGet > m_dwPutLast) && (m_dwGet != m_dwBase)) {
    // but i'm almost certain the second check is meaningless. dwGet can't possibly
    // be both ==base and >putlast. this would imply (putlast < base)  -- CRD 16jun00
    while (m_dwGet > m_dwPutLast) {
        nvSpin(_systemInfo.dwSpinLoopCount);
        // STORE_GETPUT();
        m_dwGet = m_dwBase + _pdwHWGet;
    }

#ifdef CAPTURE
    // if capturing, write out the tail end of the push buffer now,
    // since we won't do it after issuing the jump and resetting put
    kickOff();
#endif

    // write the jump and start puller
    push (0, NVPUSHER_JUMP(0));
    m_dwPut = m_dwBase;
    m_dwWrapCount ++;
    kickOff();

            #ifdef NV_PROFILE_PUSHER
            NVP_STOP(NVP_T_WRAPAROUND);
            nvpLogTime (NVP_T_WRAPAROUND,nvpTime[NVP_T_WRAPAROUND]);
            #endif
}

//---------------------------------------------------------------------------

#ifdef _PC_CHECK

// CPushBuffer::validate
// walks a push buffer and check that we have no parser or other
// common pusher errors. This is a second check, since CPushBuffer::adjust
// does a simple pass already.

BOOL CPushBuffer::validate
(
    DWORD dwStart,
    DWORD dwEnd
)
{
    DWORD dwLastAddr     = 0;
    DWORD dwLastJumpAddr = 0;
    DWORD dwAddr         = dwStart;
    DWORD dwCount        = m_dwSize; // allow 4 times the number of commands before we time out

#ifdef _PC_CAPTURE
    if (bPusherCapture) {
        DWORD dwJunk;
        char  sz[256];

        for (DWORD dwAddr = dwStart; dwAddr != dwEnd;) {
            DWORD dwMethod = *(DWORD*)dwAddr;
            dwAddr += 4;

            // save method
            NvWriteFile (hPusherFileBinary, (LPBYTE)(&dwMethod), 4, &dwJunk, 0);
            nvSprintf (sz, "%08x: method %08x ----\r\n", dwAddr, dwMethod);
            NvWriteFile (hPusherFileAscii, sz, nvStrLen(sz), &dwJunk, 0);

            if (dwMethod & 0x20000000) {
                // jmp
                dwAddr = m_dwBase + (dwMethod & 0x1fffffff);
            }
            else {
                for (DWORD dwCount = ((dwMethod & 0x1fff0000) >> 18); dwCount; dwCount--) {
                    DWORD dwData = *(DWORD*)dwAddr;
                    dwAddr += 4;
                    // save data
                    NvWriteFile (hPusherFileBinary, (LPBYTE)(&dwData), 4, &dwJunk, 0);
                    nvSprintf (sz, "%08x:   data %08x\r\n", dwAddr, dwData);
                    NvWriteFile (hPusherFileAscii, sz, nvStrLen(sz), &dwJunk, 0);
                }
            }
        }
    }
#endif

    while (dwCount && (dwAddr != dwEnd)) {

        DWORD dwCommand;

        // check if we are still in range
        if ((dwAddr < m_dwBase) || (dwAddr >= m_dwBase + m_dwSize)) {
            DPF ("CPushBuffer::validate: Pointer out of push buffer range.");
            DPF ("                       Bad address  : %08x", dwAddr);
            DPF ("                       Last address : %08x", dwLastAddr);
            DPF ("F10 to return to caller.");
            __asm int 3;
            return (FALSE);
        }

        // get command
        dwLastAddr = dwAddr;
        dwCommand = *(DWORD*)dwAddr;

        // process
        if (dwCommand & 0x00000001) {
            PF("dwCommand = %08x", dwCommand);
            PF("Invalid low-order bit");
            _asm int 3;
        }
        else if (dwCommand & 0x00000002) {
            // call command
            dwAddr += 4;
        }
        else if (dwCommand & 0x20000000) {
            // jump instruction
            dwLastJumpAddr = dwAddr;
            dwAddr = m_dwBase + (dwCommand & 0x1fffffff);
        }
        else {
#ifdef _PC_CAPTURE_METHOD_DATA
            // capture data
            parseMethod (dwAddr);
#endif
            // skip past this method header and its data
            dwAddr += 4 * (1 + ((dwCommand & ~0xe0000000) >> 18));
        }

        dwCount--;

    }

    // if we timed out then we assume the push buffer has some kind of infine loop
    //  that does not cause the pointer to go out of range but will hang
    //  or cause other failures
    if (!dwCount) {
        DPF ("CPushBuffer::validate: Validation timed out.");
        DPF ("                       Current address   : %08x", dwAddr);
        DPF ("                       Last address      : %08x", dwLastAddr);
        DPF ("                       Last JUMP address : %08x", dwLastJumpAddr);
        DPF ("F10 to return to caller.");
        __asm int 3;
        return FALSE;
    }

    return TRUE;
}

#endif //_PC_CHECK

//---------------------------------------------------------------------------

#ifdef _PC_CAPTURE_METHOD_DATA

static DWORD g_adwMax[8] = {0,0,0,0,0,0,0,0};
static DWORD g_adwState[8][0x2000 / 4]; // a whole bunch of method info

#define _SUBCHANNEL(x)          (((x) >> 13) & 7)
#define _SIZE(x)                (((x) >> 18) & 0x7ff)
#define _METHOD(x)              (((x) >>  2) & 0x7ff)
#define _NOINC(x)               ((x) & 0x40000000)
#define _JMP(x)                 ((x) & 0x20000000)

// CPushBuffer::parseMethod

DWORD CPushBuffer::parseMethod (DWORD dwAddress)
{
    DWORD dwMethod     = *(DWORD*)dwAddress;
    DWORD dwSubChannel = _SUBCHANNEL(dwMethod);
    DWORD dwAdd        = _NOINC(dwMethod) ? 0 : 1;
    DWORD dwCount      = _SIZE(dwMethod);
          dwMethod     = _METHOD(dwMethod);

    dwAddress += 4;

    for (; dwCount; dwCount--) {
        DWORD dwData = *(DWORD*)dwAddress;

        // handle new object
        if ((dwMethod == 0)
            && (g_adwState[dwSubChannel][0] != dwData)) {
            memset (g_adwState[dwSubChannel],0xdd,sizeof(g_adwState[dwSubChannel]));
            g_adwMax[dwSubChannel] = 0;
        }

        // store state
        g_adwState[dwSubChannel][dwMethod] = dwData;
        if (dwMethod > g_adwMax[dwSubChannel]) {
            g_adwMax[dwSubChannel] = dwMethod;
        }

        // next
        dwMethod  += dwAdd;
        dwAddress += 4;
    }

    return dwAddress;
}

//---------------------------------------------------------------------------

// CPushBuffer::captureState

void CPushBuffer::captureState (DWORD dwSubChannel)
{
    char sz[512];

    nvSprintf (sz,"\\state_%d.txt",dwSubChannel);
    HANDLE f = NvCreateFile(sz, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD  dw;

    for (DWORD i = 0; i < g_adwMax[dwSubChannel]; i++) {
        nvSprintf (sz,"%08x: %08x\r\n",i * 4,g_adwState[dwSubChannel][i]);
        NvWriteFile (f,sz,nvStrLen(sz),&dw,0);
    }
    FlushFileBuffers (f);
    NvCloseHandle (f);
}

#endif  // !_PC_CAPTURE_METHOD_DATA

#endif  // (NVARCH >= 0x04)

