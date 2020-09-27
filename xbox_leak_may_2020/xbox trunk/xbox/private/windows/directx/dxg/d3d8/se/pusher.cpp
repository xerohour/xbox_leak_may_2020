/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       pusher.cpp
 *  Content:    Handles access to the hardware's push buffer
 *
 ***************************************************************************/

#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#if DBG

// 'g_pPushNext' is used to make sure we don't leave any gaps in the
// push buffer when using Push() macros.

PPUSH g_pPushNext;

// 'g_PushIndex' is used to make sure we don't overwrite the end of
// our guaranteed buffer size when using Push() macros.

DWORD g_PushIndex;

// Number of dwords we reserved at either 'StartPush' or 'StartBuffer'
// time:

DWORD g_ReservedSize;

// 'g_PushState' is a debug-only global variable for making sure that
// 'StartPush/EndPush' is correctly paired by the calling code.

DWORD g_PushState;

#endif

#if DBG

//------------------------------------------------------------------------------
// Debug helper to record a 'StartPush'.

void DbgRecordPushStart(
    DWORD Size)
{
    if (g_PushState == PUSH_STATE_START)
    {
        DXGRIP("Pusher - Nested calls to the push-buffer allocator, potentially "
             "caused by calling drawing methods from multiple threads or from "
             "within a DPC.");
    }

    else if (g_PushState == PUSH_STATE_BLOCKED)
    {
        DXGRIP("Pusher - The push buffer allocator was called while a thread was "
             "blocked on a resource, potentially caused by calling a drawing "
             "from another thread or a DPC.");
    }

    g_PushState = PUSH_STATE_START;
    g_PushIndex = 0;
    g_pPushNext = g_pDevice->m_Pusher.m_pPut;
    g_ReservedSize = Size;
}

#endif DBG

//------------------------------------------------------------------------------
// BusyLoop
//
// Short time-waster.  Used mainly when we're looping on the 'HwGet()'
// register, as its bad hardware-pipeline-wise for us to be continually
// querying.
//
// Note that for longer delays it might be preferable to give up our time-
// slice for a while.

VOID BusyLoop()
{
    volatile DWORD i = 400;

    do {} while (--i != 0);
}

//------------------------------------------------------------------------------
// Set up the push buffer.  Separated from dxgcreate to keep it from getting
// too nasty.

HRESULT CDevice::InitializePushBuffer()
{
    m_pPushBase = (DWORD *)AllocateContiguousMemory(m_PushBufferSize, 0);
    if (m_pPushBase == NULL)
    {
        WARNING("FATAL: Unable to allocate push buffer, Init failed.");
        return E_OUTOFMEMORY;
    }

    m_pPushLimit = m_pPushBase + (m_PushBufferSize / sizeof(DWORD));

    m_Pusher.m_pPut = m_pPushBase;
    m_Pusher.m_pThreshold = m_pPushBase 
                          + (m_PushSegmentSize / sizeof(DWORD))
                          - PUSHER_THRESHOLD_SIZE_PLUS_OVERHEAD;

    // GPU time never equals CPU time, and neither are ever zero:

    m_CpuTime = (2 << PUSHER_TIME_SHIFT) | PUSHER_TIME_VALID_FLAG;
    *m_pGpuTime = (1 << PUSHER_TIME_SHIFT) | PUSHER_TIME_VALID_FLAG;
    m_LastRunPushBufferTime = (1 << PUSHER_TIME_SHIFT) | PUSHER_TIME_VALID_FLAG;

    // There must be enough bits in the Time variable to not overflow if
    // the entire push-buffer were filled with Fence commands (because
    // times would be re-issued while objects with those times are still
    // pending, which is bad):

    ASSERT(m_PushBufferSize / sizeof(FenceEncoding)
           < (1 << (32 - PUSHER_TIME_SHIFT)));

    // Initialize the segments:

    DWORD segments = (m_PushBufferSize - 1) / m_PushSegmentSize;
    DWORD power = 0;

    do {
        power++;
        segments >>= 1;

    } while (segments != 0);

    segments = (1 << power);

    ASSERT(segments >= m_PushBufferSize / m_PushSegmentSize);
    ASSERT(m_PusherSegment == NULL);

    m_PusherSegmentMask = (segments - 1);
    m_PusherSegment = (Fence*) MemAlloc(segments * sizeof(Fence));
    if (m_PusherSegment == NULL)
    {
        WARNING("FATAL: Unable to allocate segment array, Init failed.");
        return E_OUTOFMEMORY;
    }

#if DBG
    g_ParsePut = m_pPushBase;
#endif

    return S_OK;
}

//------------------------------------------------------------------------------
// UninitializePushBuffer

VOID CDevice::UninitializePushBuffer()
{
    if (m_pPushBase)
    {
        FreeContiguousMemory((void*) m_pPushBase);
    }

    if (m_PusherSegment)
    {
        MemFree(m_PusherSegment);
    }
}

//------------------------------------------------------------------------------
// SetFence
//
// Do the actual fence-putting into the push buffer.  Can't be called
// while recording a push-buffer.
//
// NOTE: Every SetFence MUST be accompanied by a KickOff!  The reason is
//       that our multi-threaded support lets a resource be released by
//       a non-drawing thread so long as that thread spins on IsBusy()
//       before doing the release.  A resource may be tied to any 
//       SetFence, so in order to not starve the thread we always do a
//       KickOff after every SetFence.

DWORD SetFence(
    DWORD Flags)
{
    CDevice *pDevice = g_pDevice;

    if (DBG_CHECK(pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER))
    {
        DXGRIP("D3DDevice_InsertFence - can't InsertFence while recording a "
               "push-buffer.");
    }

    PPUSH pPush = pDevice->StartPush();

    FenceEncoding* pEncoding = (FenceEncoding *) pPush;

    DWORD time = pDevice->m_CpuTime;

    ASSERT(time & PUSHER_TIME_VALID_FLAG);

    // Fill in the fence:

    pEncoding->m_SemaphoreCommand
        = EncodeMethod(SUBCH_3D, NV097_BACK_END_WRITE_SEMAPHORE_RELEASE);
            
    pEncoding->m_Time = time;

    // We should use a Kelvin NOP here except that it will throw an
    // interrupt if the data is not zero.  SET_COLOR_CLEAR_VALUE takes the 
    // same amount of time to execute as a Kelvin NOP and allows any 
    // value.  It's safe to use because we always manually reset it
    // before we do any clears.

    pEncoding->m_SetColorClearCommand1 
            = PUSHER_METHOD(SUBCH_3D, NV097_SET_COLOR_CLEAR_VALUE, 1);
    pEncoding->m_SetColorClearArgument1 = 0;

    pEncoding->m_SetColorClearCommand2 
            = PUSHER_METHOD(SUBCH_3D, NV097_SET_COLOR_CLEAR_VALUE, 1);
    pEncoding->m_SetColorClearArgument2 = 0;

    PushedRaw(pPush + 6);
    pDevice->EndPush(pPush + 6);

    DWORD runTotal = pDevice->m_PusherPutRunTotal;
    DWORD fence = (time >> PUSHER_TIME_SHIFT) & (PUSHER_FENCE_COUNT - 1);

    pDevice->m_PusherFence[fence].Time = time;
    pDevice->m_PusherFence[fence].pEncoding = pEncoding;
    pDevice->m_PusherFence[fence].RunTotal = runTotal;

    // For 'segment' fences, which are to be kept at fairly regular
    // intervals, we also remember the fence in our segment array:

    if (Flags & SETFENCE_SEGMENT)
    {
        DWORD segment 
            = (pDevice->m_PusherLastSegment + 1) & pDevice->m_PusherSegmentMask;

        pDevice->m_PusherLastSegment = segment;

        pDevice->m_PusherSegment[segment].Time = time;
        pDevice->m_PusherSegment[segment].pEncoding = pEncoding;
        pDevice->m_PusherSegment[segment].RunTotal = runTotal;
    }

    // Open the next time.  We make sure we never have a CPU time of zero
    // by always adding by 2 and having the LSB set to one:

    pDevice->m_CpuTime += (1 << PUSHER_TIME_SHIFT);

    // The NOKICKOFF flag can only be set if the caller guarantees they will 
    // do their own KickOff:

    if (!(Flags & SETFENCE_NOKICKOFF))
    {
        pDevice->KickOff();
    }

    return time;
}

//------------------------------------------------------------------------------
// ComputeGap
//
// Computes the number of push-buffer bytes between a specified fence and
// where the GPU is currently reading.  If the GPU has already passed the
// fence, it returns zero.
//
// By its nature, this routine returns a value equal to or less than the
// true gap (the GPU is operating in real time to reduce the gap anyways).

DWORD FASTCALL ComputeGap(
    CDevice* pDevice,
    Fence* pFence)
{
    // Compute the push-buffer difference, accounting for wrapping of
    // the push-buffer:

    DWORD get = (DWORD) pDevice->GpuGet();
    if ((DWORD*) get <= pDevice->m_Pusher.m_pPut)
        get += pDevice->m_PusherLastSize;

    DWORD fence = (DWORD) pFence->pEncoding;
    if ((DWORD*) fence <= pDevice->m_Pusher.m_pPut)
        fence += pDevice->m_PusherLastSize;

    // The distance to the fence can be negative by the time we get here
    // because the GPU is constantly advancing in real-time:

    INT distance = fence - get;
    if (distance < 0)
        return 0;

    // Add in the length of all RunPushBuffer calls between the GPU and the
    // fence that haven't yet been executed by the GPU:

    INT run = pFence->RunTotal - pDevice->m_Miniport.m_PusherGetRunTotal;
    if (run < 0)
        run = 0;

    return run + distance;
}

//------------------------------------------------------------------------------
// FindFence
//
// Finds the fence of the same age (or newer if there isn't an exact match)
// as the specified 'Time'.
//
// Note that IsTimePending() should have already been done on 'Time'.

static Fence* FindFence(
    DWORD TargetTime)
{
    Fence* pFence;

    CDevice* pDevice = g_pDevice;

    ASSERT(TargetTime & PUSHER_TIME_VALID_FLAG);
    ASSERT(TargetTime != pDevice->m_CpuTime);

    // 'targetAge' is how many fences ago the particular fence we're looking
    // for was written:

    DWORD targetAge = pDevice->Age(TargetTime);

    // We keep the most recent PUSHER_FENCE_COUNT fences in the m_PusherFence
    // array:

    DWORD iFence = (DWORD) -1;
    DWORD iSegment;

    if (targetAge <= (PUSHER_FENCE_COUNT << PUSHER_TIME_SHIFT))
    {
        iFence = (TargetTime >> PUSHER_TIME_SHIFT) & (PUSHER_FENCE_COUNT - 1);
    }
    else
    {
        // D'oh, the specified time is older than anything in our fence array
        // (this can happen if a lot of InsertFences were done by the caller
        // recently).
        //
        // For this purpose, we use our segment array, which keeps fences over
        // a longer life time, at regular intervals.
        //
        // Start from the oldest and increase until we find something newer:
    
        iSegment = pDevice->m_PusherLastSegment;

        // We might not have a fence in the segment array that is the
        // same age or newer than the request.  If so, use the oldest
        // fence from the fence array.

        if (pDevice->Age(pDevice->m_PusherSegment[iSegment].Time) > targetAge)
        {
            iFence = (pDevice->m_CpuTime >> PUSHER_TIME_SHIFT) 
                   & (PUSHER_FENCE_COUNT - 1);
        }
        else
        {
            // We now know that there's a segment fence which is newer than the
            // target time, so our loop will terminate:

            do {
                iSegment = (iSegment + 1) & pDevice->m_PusherSegmentMask;

            } while (pDevice->Age(pDevice->m_PusherSegment[iSegment].Time) 
                        > targetAge);
        }
    }

    // Make sure the fence isn't stale by going through each of the newer
    // fences and ensuring that their push-buffer address is really
    // newer:

    if (iFence != (DWORD) -1)
    {
        pFence = &pDevice->m_PusherFence[iFence];
    
        DWORD fence = (DWORD) pFence->pEncoding;
        if ((DWORD*) fence <= pDevice->m_Pusher.m_pPut)
            fence += pDevice->m_PusherLastSize;
    
        DWORD iOldest = (pDevice->m_CpuTime >> PUSHER_TIME_SHIFT) 
                      & (PUSHER_FENCE_COUNT - 1);

        while (TRUE)
        {
            iFence = (iFence + 1) & (PUSHER_FENCE_COUNT - 1);
            if (iFence == iOldest)
                break;
    
            DWORD newerFence = (DWORD) pDevice->m_PusherFence[iFence].pEncoding;
            if ((DWORD*) newerFence <= pDevice->m_Pusher.m_pPut)
                newerFence += pDevice->m_PusherLastSize;
    
            if (newerFence < fence)
                return NULL;
        }
    }
    else
    {
        pFence = &pDevice->m_PusherSegment[iSegment];
    
        DWORD segment = (DWORD) pFence->pEncoding;
        if ((DWORD*) segment <= pDevice->m_Pusher.m_pPut)
            segment += pDevice->m_PusherLastSize;
    
        DWORD iOldest = (pDevice->m_PusherLastSegment + 1) 
                      & pDevice->m_PusherSegmentMask;
        while (TRUE)
        {
            iSegment = (iSegment + 1) & pDevice->m_PusherSegmentMask;
            if (iSegment == iOldest)
                break;
    
            DWORD newerSegment = (DWORD) pDevice->m_PusherSegment[iSegment].pEncoding;
            if ((DWORD*) newerSegment <= pDevice->m_Pusher.m_pPut)
                newerSegment += pDevice->m_PusherLastSize;
    
            if (newerSegment < segment)
                return NULL;
        }
    }

    ASSERT(pFence->pEncoding != NULL);
    ASSERT(pFence->Time & PUSHER_TIME_VALID_FLAG);

    // Make sure the returned fence is the same as (or newer than) the 
    // requested time:

    ASSERT(pDevice->Age(pFence->Time) <= pDevice->Age(TargetTime));

    return pFence;
}

//------------------------------------------------------------------------------
// BlockOnTime
//
// Wait until the GPU gets to the specified time.  
//
// If the GPU is very close to the requested time, simply spin the CPU until 
// the GPU gets there; if the GPU is far from the requested time, insert a 
// software-method instruction at the requested point in the push-buffer, and 
// yield our thread until the interrupt is hit, at which time our thread will
// be resumed.

VOID BlockOnTime(
    DWORD Time,
    BOOL MakeSpace)
{
    DWORD status;
    LARGE_INTEGER timeOut;
    LARGE_INTEGER* pTimeOut = NULL;

    CDevice* pDevice = g_pDevice;

    ASSERT(Time & PUSHER_TIME_VALID_FLAG);
    ASSERT((pDevice->GpuTime() != pDevice->m_CpuTime) &&
           (pDevice->GpuTime() & PUSHER_TIME_VALID_FLAG) &&
           (pDevice->m_CpuTime & PUSHER_TIME_VALID_FLAG));

    // No need to block if the GPU is already past the specified time
    // value:

    if (!pDevice->IsTimePending(Time))
        return;

    // If the time is current, then we need to insert a marker:

    if (Time == pDevice->m_CpuTime)
        SetFence(0);

    DWORD spinTime = Time;

    Fence* targetFence = FindFence(Time);
    if (targetFence == NULL)
        return;

#if DBG

    if (g_PushState == PUSH_STATE_START)
    {
        DXGRIP("Pusher - Attempting to block while pushing data to the push "
               "buffer, possibly because you are calling blocking/drawing "
               "methods from multiple threads or a DPC.");
    }
    else if (g_PushState == PUSH_STATE_BLOCKED)
    {
        DXGRIP("Pusher - Attempting to block on a fence while a thread is "
               "already blocked, possibly because you are calling blocking "
               "methods from multiple threads or a DPC.");
    }

    g_PushState = PUSH_STATE_BLOCKED;

    timeOut.QuadPart = (LONGLONG) D3D__DeadlockTimeOutVal * (LONGLONG)-10000;
    pTimeOut = &timeOut;

#endif DBG

    // Because of the GPU's read-ahead-cache, it will effectively ignore
    // any modifications we make that are less than 1KB ahead of where
    // the hardware is currently 'getting':

    DWORD gap = ComputeGap(pDevice, targetFence);

    if (gap >= PUSHER_BLOCK_THRESHOLD)
    {
        // Ensure that the fence is not stale.  Yes, these are reads from 
        // write-combined memory.  (Hi Mike.)

        ASSERT(targetFence->pEncoding->m_SemaphoreCommand 
            == EncodeMethod(SUBCH_3D, NV097_BACK_END_WRITE_SEMAPHORE_RELEASE));
        ASSERT(targetFence->Time == targetFence->pEncoding->m_Time);

        FenceEncoding* pEncoding = targetFence->pEncoding;
    
        KeClearEvent(&pDevice->m_Miniport.m_BusyBlockEvent);

        // Here we're going to modify an already-submitted portion of
        // the push-buffer in order to insert an interrupt command that 
        // will wake up the CPU and release our thread.  
        //
        // NOTE: Because the GPU is operating asynchronously to the CPU,
        //       there's a chance that the CPU may get paused between
        //       any instruction in this code - meaning that the GPU
        //       may get to where we're modifying before we've completed
        //       all of our push-buffer modifications!  Consequently, we
        //       have to make sure that we're robust if this happens (we
        //       can't crash the GPU with any partial writes, and we
        //       have to account for the possibility of the GPU getting 
        //       past the inserted interrupt command before we finish 
        //       writing it.)
        //
        // First, ensure that the back-end is flushed before the NVX_FENCE
        // command is processed (otherwise the kernel event could be 
        // set before the back-end finishes).  An alternative to this
        // would be to spin the CPU after the thread wakes up.
        //
        // We don't need to do this for MakeSpace...

        if (!MakeSpace)
        {
            pEncoding->m_WaitForIdleCommand 
                = PUSHER_METHOD(SUBCH_3D, NV097_WAIT_FOR_IDLE, 1);
    
            // Note that we technically don't have to write this zero,
            // since we wrote it as zero when we first created the fence.
            // I'm doing this here to keep the writes contiguous...
    
            pEncoding->m_WaitForIdleArgument
                = 0;
        }

        // Insert the command that wakes up the CPU and sets the kernel
        // event:
    
        pEncoding->m_NoOperationCommand 
            = PUSHER_METHOD(SUBCH_3D, NV097_NO_OPERATION, 1);
        pEncoding->m_FenceCommand 
            = NVX_FENCE;

        // Make sure that our modifications get written to the push-buffer:
    
        FlushWCCache();

        DWORD newGap = ComputeGap(pDevice, targetFence);
        ASSERT(newGap <= gap);
        if (newGap < PUSHER_BLOCK_THRESHOLD)
        {
            // Argh, the gap closed enough that we're not sure if we beat
            // the GPU's read-ahead.  It would be safe to let it go, but we
            // NOP out the wait-for-idle to save some GPU time in case we
            // truly did beat the read-ahead:

            pEncoding->m_WaitForIdleCommand 
                = PUSHER_METHOD(SUBCH_3D, NV097_NO_OPERATION, 1);
            pEncoding->m_WaitForIdleArgument
                = 0;
            pEncoding->m_NoOperationCommand 
                = PUSHER_METHOD(SUBCH_3D, NV097_NO_OPERATION, 1);
            pEncoding->m_FenceCommand 
                = 0;

            FlushWCCache();

            spinTime = targetFence->Time;
            goto Spin;
        }

        // Block on the event:

        while (TRUE)
        {
            status = KeWaitForSingleObject(&pDevice->m_Miniport.m_BusyBlockEvent,
                                           UserRequest,
                                           UserMode,
                                           FALSE,
                                           pTimeOut);
            if (status == STATUS_SUCCESS)
                break;

            DXGRIP("Possible deadlock");
        }
    }
    else
    {

Spin:

        // Spin if the GPU is close to the fence we're blocking on.  We do
        // this check again just in case the GPU has wandered across this
        // threshold while writing the notification above.

        COUNT_PERFCYCLES(PERF_CPUSPINDURINGWAIT_TIME);

        INITDEADLOCKCHECK();

        // Note that since 'm_pGpuTime' is in cached memory, we don't bother
        // to invoke BusyLoop here:

        while (pDevice->IsTimePending(spinTime))
        {
            if (DBG_CHECK(TRUE))
            {
                // Make sure that our gap logic works properly, and that the
                // gap never increases:

                DWORD newerGap = ComputeGap(pDevice, targetFence);
                // ASSERT(newerGap <= gap);
                gap = newerGap;
            }

            DODEADLOCKCHECK();
        }
    }

    // Assert that we did our job properly:

    ASSERT(MakeSpace || !pDevice->IsTimePending(Time));

#if DBG

    g_PushState = PUSH_STATE_NONE;

#endif DBG
}

//------------------------------------------------------------------------------
// MakeRequestedSpace
//
// This does the following:
//
//   - Kicks off what is in the push buffer so far
//   - Ensures that there is enough space to write the allocation
//   - Writes a marker indicating that we've started a new section
//
// RequestedSpace is the size (in bytes) that is requested, but it's okay
// to return a smaller allocation so long as it's as big as MinimumSpace.

PPUSH MakeRequestedSpace(
    DWORD MinimumSpace, 
    DWORD RequestedSpace) 
{
    CDevice* pDevice = g_pDevice;

    ASSERT(MinimumSpace <= RequestedSpace);

    // For best performance, 'RequestedSpace' should equal the standard
    // kick-off size.  But we allow larger requests.

    ASSERT(RequestedSpace >= pDevice->m_PushSegmentSize);

    // No single request can be larger than half the push-buffer size
    // (because we assume that when we've waited for half the push-buffer
    // to empty that there will always be enough room for the 
    // MakeRequestedSpace request).

    ASSERT(RequestedSpace <= pDevice->m_PushBufferSize / 2);

    if (pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER)
    {
        // Uh oh, we're recording and we just overran the end of the caller's
        // buffer.  Mark the fact that this push-buffer is now messed up
        // (and at the same time remember how much memory will be needed
        // by the final result):

        pDevice->m_PushBufferRecordWrapSize
            += (DWORD) pDevice->m_Pusher.m_pPut 
             - pDevice->m_pPushBufferRecordResource->Data;

        // Since we've just declared the push-buffer officially screwed,
        // give us some place to write the new data by resetting the
        // pointer back to the beginning of the buffer:

        pDevice->m_Pusher.m_pPut 
            = (PPUSH) pDevice->m_pPushBufferRecordResource->Data;

        return pDevice->m_Pusher.m_pPut;
    }

    PPUSH pStart = pDevice->m_Pusher.m_pPut;
    PPUSH pEnd = (PPUSH) ((DWORD) pStart + RequestedSpace);

    DWORD gpuTime = pDevice->GpuTime();
    PPUSH pGet = pDevice->GpuGet();

    // Check to see if we're 'close' to the buffer end:

    if ((DWORD) pEnd + (DEFAULT_PUSH_BUFFER_SEGMENT_SIZE / 2)
                                        >= (DWORD) pDevice->m_pPushLimit)
    {
        if ((DWORD) pStart + MinimumSpace <= (DWORD) pDevice->m_pPushLimit)
        {
            // We're close to the end of the buffer, and there's enough
            // room for the minimum requested amount, so just give
            // the rest of it away:

            pEnd = pDevice->m_pPushLimit;
        }
        else
        {
            // Darn, there's not enough room to satisfy the minimum amount
            // we need.  We have to wrap to the beginning of the push-buffer.

            COUNT_PERF(PERF_PUSHBUFFER_JUMPTOBEGINNING);

            // Remember how far we got:

            pDevice->m_PusherLastSize = (DWORD) pDevice->m_Pusher.m_pPut 
                                      - (DWORD) pDevice->m_pPushBase;

            // Insert a jump at the current location.  This is always
            // legal because we reserve an extra DWORD at the end of
            // each segment.

            *pDevice->m_Pusher.m_pPut 
                = PUSHER_JUMP(GetGPUAddressFromWC((void*) pDevice->m_pPushBase));

            ASSERT(pDevice->m_Pusher.m_pPut < pDevice->m_pPushLimit);

            // If the requested size is bigger than the normal segment size,
            // we may get a case where the GPU is still reading the end of
            // the push-buffer from the previous pass.  Simply treat this
            // as if the push-buffer has advanced to reading the start, and
            // our logic below will correctly handle this case:

            if (pGet > pDevice->m_Pusher.m_pPut)
            {
                pGet = pDevice->m_pPushBase + 1;
            }

            // This apparently demented check is to handle the wrap case 
            // where the GPU is a full push-buffer behind, and is just now 
            // processing the JUMP to the start of the push-buffer, at the
            // same time we've just completed another whole pass of the
            // push-buffer.  This special case exists because the 
            // "((pStart < pGet) && (pGet <= pEnd))" check below should
            // really have pGet = pPushLimit if pGet == pPushBase.

            if (pGet == pDevice->m_pPushBase)
            {
                pGet = pDevice->m_pPushBase + 1;
            }

        #if PROFILE

            // Record the number of bytes at the end we just skipped:

            g_PerfCounters.m_PushBufferEndDwordsSkipped 
                += pDevice->m_pPushLimit - pDevice->m_Pusher.m_pPut;

        #endif

            // Reset back to the beginning:

            pStart = pDevice->m_pPushBase;
            pEnd = (PPUSH) ((DWORD) pStart + RequestedSpace);

            pDevice->m_Pusher.m_pPut = pStart;
        }
    }

    // Check to see if the GPU is still reading from the range we want to
    // use.
    //
    // Note that the GPU is completely caught-up if pStart == pGet:

    if ((pStart < pGet) && (pGet <= pEnd))
    {
        COUNT_PERFCYCLES(PERF_PUSHBUFFER_WAITS);
        COUNT_PERFEVENT(D3DPERFEvent_PushBufferWait, FALSE);

        PPUSH pSegment;
        DWORD blockTime;

        // Yep, the GPU's backed up and there's no room in the push-buffer 
        // for the new request.  We want to wait on a fence that is
        // approximately half way around the push-buffer (or newer).

        PPUSH pTarget 
            = (PPUSH) ((DWORD) pStart + (pDevice->m_PushBufferSize / 2));

        // Scan through the segment list.
        //
        // Note that we have to actually scan through the list - we can't
        // simply jump to an index half way around, because unfortunately
        // segments aren't guaranteed to be at regular points, because
        // of the Begin/End fence limitation imposed by the hardware.  (We
        // could do a binary search, though.)
        //
        // Proceed from the newest to the oldest, looking for the last 
        // segment newer than our target segment.  We have to have something
        // newer than half way around to ensure that there's enough room to
        // succeed our call:

        // [andrewso] If we get here then we know that the CPU has caught up
        // to the GPU.  The goal of this loop is to fine the first safe
        // time at which we can block to ensure that half of the push buffer
        // has been emptied before we continue.  But there can be a case where
        // the GPU slows down as the CPU is pushing a huge-o-mungo begin/end
        // dataset into the push buffer in which case there may be stale
        // entries in the segment table.  We do not want to block on these
        // stale entries!
        //
        // The solution is to block on the very next fence after the target
        // that has not been crossed by the GPU when this loop first starts.
        // If it has been crossed then we can't be sure if it's a stale 
        // entry of a valid one...we need to play it safe.
        
        DWORD i = pDevice->m_PusherLastSegment;

        do {
            blockTime = pDevice->m_PusherSegment[i].Time;

            // Look at the next, older segment:

            i = (i - 1) & pDevice->m_PusherSegmentMask;

            pSegment = (PPUSH) pDevice->m_PusherSegment[i].pEncoding;
            
            // Any segment with a value less than the current 'put' location 
            // is actually newer than any segment with a value more than the
            // current 'put'.  So to make the time comparison valid, bias
            // any values less than 'pStart':
    
            if (pSegment < pStart)
            {
                pSegment = (PPUSH) ((DWORD) pSegment + pDevice->m_PusherLastSize);
            }

            // Keep looping while the current segment is newer than our target
            // (larger values are newer), or we run off the end of the list:

        } while ((pSegment > pTarget) && (i != pDevice->m_PusherLastSegment) && pDevice->m_PusherSegment[i].Time > gpuTime);

        // If the caller is doing space requests (via RunPushBuffer or 
        // BeginPush) that are close to the size of the total push-buffer, 
        // we may be unable to find a single fence newer than the requested 
        // position on which to wait.  We simply break in that case:

        if (DBG_CHECK(pSegment < pStart))
        {
            DPF_ERR("Space request is too big - can't find a fence to block on.\n"
                    "Consider expanding push-buffer using SetPushBufferSize");
        }

        BlockOnTime(blockTime, TRUE);

        // Make sure our logic worked, and the GPU is no longer in the range
        // where we want to write:

        ASSERT((pGet = pDevice->GpuGet(), !((pStart < pGet) && (pGet <= pEnd))));
    }

    // Set the new threshold, which is the start of the next marker, minus
    // the threshold value plus a little.  This guarantees enough room at
    // the end of a segment for a jump and marker, if needed, and also
    // makes sure that a single allocation can't take us up to or past the
    // end of the segment.

    pDevice->m_Pusher.m_pThreshold = pEnd - PUSHER_THRESHOLD_SIZE_PLUS_OVERHEAD;

#if DBG

    // Quick, let's verify that we can actually use the whole segment by
    // scribbling over everything:

    PPUSH pTmp = pDevice->m_Pusher.m_pThreshold 
                + PUSHER_THRESHOLD_SIZE_PLUS_OVERHEAD - 1;
    DWORD c = 0;

    for (; pTmp >= pDevice->m_Pusher.m_pPut; pTmp--)
    {
        *pTmp = 0x0000BEEF | (c++ << 16);
    }

#endif

    // Mark the new segment if we can:

    if (pDevice->m_StateFlags & STATE_BEGINENDBRACKET)
    {
        pDevice->m_StateFlags |= STATE_SEGMENTFENCEPENDING;
    }
    else
    {
        SetFence(SETFENCE_SEGMENT);
    }

    // Finally, kick off the finished segment:

    pDevice->KickOff();

    return pDevice->m_Pusher.m_pPut;
}

//------------------------------------------------------------------------------
// MakeSpace
//
// This method is called anytime a push buffer allocation would cross
// a threshold boundary.

PPUSH MakeSpace()
{
    // You might think that such a small routine as this should be inlined.
    // But there are a zillion calls to MakeSpace, and pushing the default
    // segment size inline would increase our code size significantly.
    // Calls to MakeSpace are reasonably infrequent, and so do not have to
    // be the ultimate in performance.

    return MakeRequestedSpace(g_pDevice->m_PushSegmentSize / 2, 
                              g_pDevice->m_PushSegmentSize);
}

//------------------------------------------------------------------------------
// XMETAL_StartPushCount

extern "C"
PPUSH WINAPI XMETAL_StartPushCount(
    XMETAL_PushBuffer *pPusher,
    DWORD count)
{
    PPUSH pStart;

    // We currently always pass in 'g_pDevice' as a parameter since the
    // caller almost always has it enregistered already.

    CDevice* pDevice = (CDevice*) pPusher;

    if (DBG_CHECK(TRUE))
    {
        if (pDevice != g_pDevice)
        {
            DXGRIP("XMETAL_StartPushCount - invalid pusher pointer.");
        }

        if (pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER)
        {
            if ((count + 1) * sizeof(DWORD)
                >= pDevice->m_pPushBufferRecordResource->AllocationSize)
            {
                DXGRIP("XMETAL_StartPush - an individual push buffer request "
                     "exceeds the total BeginPushBuffer buffer size.  \n"
                     "We need %li bytes, so we're about to overwrite the end "
                     "of the buffer...",
                     (count + 1) * sizeof(DWORD));
            }
        }
    }

    // We will usually take the early out here:

    if (pDevice->m_Pusher.m_pPut + count
            < pDevice->m_Pusher.m_pThreshold + PUSHER_THRESHOLD_SIZE)
    {
        pStart = pDevice->m_Pusher.m_pPut;
    }
    else
    {
        // Assume the default values first:

        DWORD maximumSpace = pDevice->m_PushSegmentSize;
        DWORD minimumSpace = pDevice->m_PushSegmentSize / 2;

        DWORD neededSpace = (count + PUSHER_THRESHOLD_SIZE + 1) * sizeof(DWORD);
        if (neededSpace > minimumSpace)
        {
            minimumSpace = neededSpace;
        }
        if (neededSpace > maximumSpace)
        {
            maximumSpace = neededSpace;
        }

        pStart = MakeRequestedSpace(minimumSpace, maximumSpace);

        ASSERT(pDevice->m_Pusher.m_pPut + count
                < pDevice->m_Pusher.m_pThreshold + PUSHER_THRESHOLD_SIZE);
    }

    DbgRecordPushStart(count);

    return pStart;
}

//------------------------------------------------------------------------------
// KickOff
//
// Update hardware's 'put' pointer to let it start chewing on the recent
// push buffer data:

void CDevice::KickOff()
{
    if (!(m_StateFlags & STATE_NULLHARDWARE))
    {
        COUNT_PERFEVENT(D3DPERFEvent_Kickoff, FALSE);
    
        // We still let KickOffs go through even while recording push-
        // buffers:
    
        PPUSH pPut = (m_StateFlags & STATE_RECORDPUSHBUFFER)
                    ? m_pPushBufferRecordSavedPut
                    : m_Pusher.m_pPut;
    
        // You can't call 'KickOff' while you've got an active PushStart or
        // BufferStart!
    
        ASSERT(g_PushState != PUSH_STATE_START);
        ASSERT((m_Pusher.m_pPut < m_pPushLimit) ||
               (m_StateFlags & STATE_RECORDPUSHBUFFER ));
    
        FlushWCCache();
    
        // Let the hardware know:
    
        HwPut(pPut);
    
        // Remember the push-buffer location:
    
        m_pKickOff = pPut;

        if (D3D__NullHardware)
        {
            // Wait for the hardware to go idle, then commandeer the 
            // hardware's get/put structure to point to normal system
            // memory (so that we can overwrite the 'get' and 'put'
            // registers):

            while (D3DDevice_IsBusy())
                ;

            m_StateFlags |= STATE_NULLHARDWARE;
        }
    }
    else
    {
        // Pretend that we sent the push-buffer to the hardware and it has
        // instantly processed everything:

        m_pControlDma = (Nv20ControlDma*) 
            ((BYTE*) g_NullHardwareGetPut - offsetof(Nv20ControlDma, Put));

        DWORD getPut = GetGPUAddress((void*) m_Pusher.m_pPut);

        m_pControlDma->Put = getPut;
        m_pControlDma->Get = getPut;

        *m_pGpuTime = m_CpuTime - (1 << PUSHER_TIME_SHIFT);

        m_Miniport.m_VBlankFlipCount = m_SwapCount;

        m_Miniport.m_PusherGetRunTotal = m_PusherPutRunTotal;
    }
}

//------------------------------------------------------------------------------
// KickOffAndWaitForIdle
//
// Waits until the GPU has emptied the push buffer and finished all processing.

void KickOffAndWaitForIdle()
{
    CDevice* pDevice = g_pDevice;

    BlockOnTime(pDevice->m_CpuTime, FALSE);

    ASSERT(pDevice->m_Miniport.m_PusherGetRunTotal == pDevice->m_PusherPutRunTotal);

    // The status register sometimes slightly lags the semaphore write-back, so 
    // unfortunately we can't always do this assertion:
    //
    //      ASSERT(!D3DDevice_IsBusy());
}

//------------------------------------------------------------------------------
// Block until a resource is no longer used by the GPU.
//
// Unlike the faster BlockOnNonSurfaceResource, this works for any type of 
// resource.

VOID BlockOnResource(
    D3DResource *pResource)
{
    COUNT_PERFCYCLES(PERF_OBJECTLOCK_WAITS);
    COUNT_PERFEVENT(D3DPERFEvent_ObjectLockWait, FALSE);

    // Handle the case where a resource Release occurs after the device is
    // released.

    if (g_pDevice == NULL)
        return;

    D3DResource *pParent = PixelJar::GetParent(pResource);

    if (pParent)
    {
        if (IsResourceSetInDevice(pResource))
        {
            KickOffAndWaitForIdle();
            return;
        }

        pResource = pParent;
    }

    DWORD ResourceTime = pResource->Lock;

    if (DBG_CHECK((ResourceTime != 0) && !(ResourceTime & PUSHER_TIME_VALID_FLAG)))
    {
        DXGRIP("Resource has an invalid 'Lock' field.  Uninitialized?");
    }

    // If the object is currently selected then wait for the GPU to idle.

    if (IsResourceSetInDevice(pResource))
    {
        KickOffAndWaitForIdle();
    }
    else
    {
        // If the time is zero, the resource hasn't been used yet (note that 
        // we permit titles to hit this case even while a different thread 
        // is busy rendering):

        if (ResourceTime != 0)
        {
            BlockOnTime(ResourceTime, FALSE);
        }
    }
}

//------------------------------------------------------------------------------
// Faster version of 'BlockOnResource', for non-surfaces only.

VOID BlockOnNonSurfaceResource(
    D3DResource *pResource)
{
    ASSERT((pResource->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_SURFACE);

    COUNT_PERFCYCLES(PERF_OBJECTLOCK_WAITS);
    COUNT_PERFEVENT(D3DPERFEvent_ObjectLockWait, FALSE);

    // Handle the case where a resource Release occurs after the device is
    // released.

    if (g_pDevice == NULL)
        return;

    DWORD ResourceTime = pResource->Lock;

    if (DBG_CHECK((ResourceTime != 0) && !(ResourceTime & PUSHER_TIME_VALID_FLAG)))
    {
        DXGRIP("Resource has an invalid 'Lock' field.  Uninitialized?");
    }

    // If the object is currently selected then wait for the GPU to idle.

    if (IsNonSurfaceResourceSetInDevice(pResource))
    {
        KickOffAndWaitForIdle();
    }
    else
    {
        // If the time is zero, the resource hasn't been used yet (note that 
        // we permit titles to hit this case even while a different thread 
        // is busy rendering):

        if (ResourceTime != 0)
        {
            BlockOnTime(ResourceTime, FALSE);
        }
    }
}

//------------------------------------------------------------------------------

#ifndef ALIASPUSHER

VOID FlushWCCache()
{
    BYTE* RegisterBase = (BYTE*)g_pDevice->m_Miniport.m_RegisterBase;

    // Flush the write-combine cache:

    _asm sfence;

    // Wait for the ol' chip to sync.
    
    FLD_WR_DRF_DEF(RegisterBase, _PFB, _WBC, _FLUSH, _PENDING);

    while (REG_RD_DRF(RegisterBase, _PFB, _WBC, _FLUSH))
        ;
}

#endif

//------------------------------------------------------------------------------
// Return whether a fence has been crossed yet or not.

extern "C"
BOOL WINAPI D3DDevice_IsFencePending(
    DWORD Fence)
{
    COUNT_API(API_D3DDEVICE_ISFENCEPENDING);

    CDevice *pDevice = g_pDevice;

    return pDevice->IsTimePending(Fence);
}

//------------------------------------------------------------------------------
// Insert a callback fence into the push buffer.

extern "C"
VOID WINAPI D3DDevice_InsertCallback(
    D3DCALLBACKTYPE Type,
    D3DCALLBACK pCallback,
    DWORD Context)
{
    COUNT_API(API_D3DDEVICE_INSERTCALLBACK);

    if (DBG_CHECK(TRUE))
    {
        if (!pCallback)
        {
            DXGRIP("D3DDevice_InsertCallback - Callback pointer.");
        }

        if (Type > D3DCALLBACK_WRITE)
        {
            DXGRIP("D3DDevice_InsertCallback - Invalid callback type.");
        }
    }

    CDevice *pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    // The callback context is stored in COLOR_CLEAR_VALUE.

    Push2(pPush, NV097_SET_ZSTENCIL_CLEAR_VALUE, 

          // NVX_READ_CALLBACK Data stored in NV097_SET_ZSTENCIL_CLEAR_VALUE

          (DWORD) pCallback, 

          // NVX_READ_CALLBACK Context stored in NV097_SET_COLOR_CLEAR_VALUE

          Context);

    // Push the software method with the callback value.

    if (Type == D3DCALLBACK_READ)
    {
        Push1(pPush + 3, NV097_NO_OPERATION, NVX_READ_CALLBACK);

        pDevice->EndPush(pPush + 5);
    }
    else
    {
        Push1(pPush + 3, NV097_WAIT_FOR_IDLE, 0);

        Push1(pPush + 5, NV097_NO_OPERATION, NVX_WRITE_CALLBACK);

        pDevice->EndPush(pPush + 7);
    }
}

//------------------------------------------------------------------------------
// External API for taking control of the push-buffer

#if DBG
    PPUSH g_pEndPush;
#endif

extern "C"
VOID WINAPI D3DDevice_BeginPush(
    DWORD Count,
    DWORD **ppPush)
{
    COUNT_API(API_D3DDEVICE_BEGINPUSH);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (ppPush == NULL)
        {
            DPF_ERR("NULL ppPush parameter");
        }
    }

    pDevice->SetStateVB(0);

    if (DBG_CHECK(TRUE))
    {
        // warn if vertex shader inputs remapped via TEXCOORDINDEX
        BYTE* pSlotMapping 
          = &g_SlotMapping[pDevice->m_pVertexShader->Flags & VERTEXSHADER_PROGRAM];

        for(UINT i=SLOT_TEXTURE0; i<=SLOT_TEXTURE3; i++)
        {
            if((pDevice->m_pVertexShader->Slot[pSlotMapping[i]].SizeAndType != 
                SIZEANDTYPE_DISABLED) && (pSlotMapping[i] != i))
            {
                char buf[256];
                sprintf(buf, "Texture stage %d is mapped to texture coordinate %d via "
                             "TEXCOORDINDEX.  BeginPush requires no remapping.\n",
                             i, pSlotMapping[i]);
                DPF_ERR(buf);
            }
        }
    }



    // Do a "+ 1" to save space for 0x0badbeef:

    PPUSH pPush = pDevice->StartPush(Count + 1);

#if DBG
    if (g_pEndPush != NULL)
    {
        DPF_ERR("Previous BeginPush never had an EndPush");
    }
    g_pEndPush = pPush + Count;
    *g_pEndPush = 0x0badbeef;
#endif

    *ppPush = (DWORD*) pPush;
}

//------------------------------------------------------------------------------
// Restore control of the push-buffer to D3D

extern "C"
VOID WINAPI D3DDevice_EndPush(
    DWORD *p)
{
    COUNT_API(API_D3DDEVICE_ENDPUSH);

    PPUSH pPush = (PPUSH) p;

    CDevice* pDevice = g_pDevice;

#if DBG
    if (g_pEndPush == NULL)
    {
        DPF_ERR("EndPush without having first done a BeginPush");
    }
    if (*g_pEndPush != 0x0badbeef)
    {
        DPF_ERR("End of buffer overwritten (more data written than specified to BeginPush)");
    }
    if ((pPush < pDevice->m_Pusher.m_pPut) || (pPush > g_pEndPush))
    {
        DPF_ERR("pPush parameter not within BeginPush range");
    }
    g_pEndPush = NULL;
#endif

    PushedRaw(pPush);

    pDevice->EndPush(pPush);
}

//------------------------------------------------------------------------------
// DumpMatrixTransposed

VOID DumpMatrixTransposed(
    PPUSH pPush,
    DWORD encodedMethod,
    CONST D3DMATRIX* pMatrix)
{
    *(pPush) = encodedMethod;

    *(pPush + 1) = *((DWORD*) &pMatrix->_11);
    *(pPush + 2) = *((DWORD*) &pMatrix->_21);
    *(pPush + 3) = *((DWORD*) &pMatrix->_31);
    *(pPush + 4) = *((DWORD*) &pMatrix->_41);
    *(pPush + 5) = *((DWORD*) &pMatrix->_12);
    *(pPush + 6) = *((DWORD*) &pMatrix->_22);
    *(pPush + 7) = *((DWORD*) &pMatrix->_32);
    *(pPush + 8) = *((DWORD*) &pMatrix->_42);
    *(pPush + 9) = *((DWORD*) &pMatrix->_13);
    *(pPush + 10) = *((DWORD*) &pMatrix->_23);
    *(pPush + 11) = *((DWORD*) &pMatrix->_33);
    *(pPush + 12) = *((DWORD*) &pMatrix->_43);
    *(pPush + 13) = *((DWORD*) &pMatrix->_14);
    *(pPush + 14) = *((DWORD*) &pMatrix->_24);
    *(pPush + 15) = *((DWORD*) &pMatrix->_34);
    *(pPush + 16) = *((DWORD*) &pMatrix->_44);
}

////////////////////////////////////////////////////////////////////////////////

#if DBG

//------------------------------------------------------------------------------
// CDevice::EndPush - debug

extern "C"
void WINAPI XMETAL_EndPush(
    XMETAL_PushBuffer *pPusher,
    PPUSH pPush)
{
    if (DBG_CHECK((void *)pPusher != (void *)g_pDevice))
    {
        DXGRIP("XMETAL_EndPush - invalid pusher pointer.");
    }

    CDevice *pDevice = g_pDevice;

    // Make sure we were paired with a StartPush call:

    if (g_PushState != PUSH_STATE_START)
    {
        DXGRIP("Pusher - End push is not paired with a start push, probably due to an earlier error.");
    }

    g_PushState = PUSH_STATE_NONE;

    // Make sure we ended where expected:

    ASSERTMSG(pPush == pPusher->m_pPut + g_PushIndex,
              "Bad EndPush: Expected %lx, got %lx.  g_PushIndex: %li",
                    pPusher->m_pPut + g_PushIndex, pPush, g_PushIndex);
    ASSERT((pPusher->m_pPut < pDevice->m_pPushLimit) ||
           (pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER ));

    // This is the only work the retail version of this routine does:

    pPusher->m_pPut = pPush;

    // Do a single step.

    if (D3D__SingleStepPusher)
    {           
        // We can't do a KickOffAndWaitForIdle here anymore because
        // that now inserts a semaphore and a bunch of other Kelvin
        // goop that can't be done while in a Begin/End bracket.

        pDevice->KickOff();

        // Wait for the pusher to empty.

        INITDEADLOCKCHECK();

        while (D3DDevice_IsBusy())
        {
            BusyLoop();

            DODEADLOCKCHECK();
        }
    }

    // Parse the push buffer on debug builds to record the current register
    // state:

    PARSE_PUSH_BUFFER();
}

//------------------------------------------------------------------------------
// PushedRaw - Call this to advance the push pointer after having copied
//             raw data into the pusher buffer

VOID PushedRaw(
    PPUSH pPushNext)
{
    g_PushIndex += (pPushNext - g_pPushNext);
    g_pPushNext = pPushNext;

    // Make sure we're in the middle of a Start/End bracket:

    ASSERT(g_PushState == PUSH_STATE_START);

    // Make sure we didn't write past the end of our guaranteed space:

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// Push1 - debug

VOID Push1(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    DWORD value)
{
    // Make sure we didn't leave a gap in the push buffer:

    ASSERT(pPush == g_pPushNext);

    // Make sure we're in the middle of a Start/End bracket:

    ASSERT(g_PushState == PUSH_STATE_START);

    pPush[0] = PUSHER_METHOD(subch, method, 1);
    pPush[1] = value;

    // Account for the two dwords just written:

    g_PushIndex += 2;
    g_pPushNext += 2;

    // Make sure we didn't write past the end of our guaranteed space:

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// Push2 - debug

VOID Push2(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    DWORD a,
    DWORD b)
{
    ASSERT(pPush == g_pPushNext);
    ASSERT(g_PushState == PUSH_STATE_START);

    pPush[0] = PUSHER_METHOD(subch, method, 2);
    pPush[1] = a;
    pPush[2] = b;

    g_PushIndex += 3;
    g_pPushNext += 3;

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// Push3 - debug

VOID Push3(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    DWORD a,
    DWORD b,
    DWORD c)
{
    ASSERT(pPush == g_pPushNext);
    ASSERT(g_PushState == PUSH_STATE_START);

    pPush[0] = PUSHER_METHOD(subch, method, 3);
    pPush[1] = a;
    pPush[2] = b;
    pPush[3] = c;

    g_PushIndex += 4;
    g_pPushNext += 4;

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// Push4 - debug

VOID Push4(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    DWORD a,
    DWORD b,
    DWORD c,
    DWORD d)
{
    ASSERT(pPush == g_pPushNext);
    ASSERT(g_PushState == PUSH_STATE_START);

    pPush[0] = PUSHER_METHOD(subch, method, 4);
    pPush[1] = a;
    pPush[2] = b;
    pPush[3] = c;
    pPush[4] = d;

    g_PushIndex += 5;
    g_pPushNext += 5;

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// Push1f - debug

VOID Push1f(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    FLOAT value)
{
    ASSERT(pPush == g_pPushNext);
    ASSERT(g_PushState == PUSH_STATE_START);

    pPush[0] = PUSHER_METHOD(subch, method, 1);
    ((FLOAT*) pPush)[1] = value;

    g_PushIndex += 2;
    g_pPushNext += 2;

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// Push2f - debug

VOID Push2f(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    FLOAT a,
    FLOAT b)
{
    ASSERT(pPush == g_pPushNext);
    ASSERT(g_PushState == PUSH_STATE_START);

    pPush[0] = PUSHER_METHOD(subch, method, 2);
    ((FLOAT*) pPush)[1] = a;
    ((FLOAT*) pPush)[2] = b;

    g_PushIndex += 3;
    g_pPushNext += 3;

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// Push3f - debug

VOID Push3f(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    FLOAT a,
    FLOAT b,
    FLOAT c)
{
    ASSERT(pPush == g_pPushNext);
    ASSERT(g_PushState == PUSH_STATE_START);

    pPush[0] = PUSHER_METHOD(subch, method, 3);
    ((FLOAT*) pPush)[1] = a;
    ((FLOAT*) pPush)[2] = b;
    ((FLOAT*) pPush)[3] = c;

    g_PushIndex += 4;
    g_pPushNext += 4;

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// Push4f - debug

VOID Push4f(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    FLOAT a,
    FLOAT b,
    FLOAT c,
    FLOAT d)
{
    ASSERT(pPush == g_pPushNext);
    ASSERT(g_PushState == PUSH_STATE_START);

    pPush[0] = PUSHER_METHOD(subch, method, 4);
    ((FLOAT*) pPush)[1] = a;
    ((FLOAT*) pPush)[2] = b;
    ((FLOAT*) pPush)[3] = c;
    ((FLOAT*) pPush)[4] = d;

    g_PushIndex += 5;
    g_pPushNext += 5;

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// PushCount - debug

VOID PushCount(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    DWORD count)
{
    ASSERT(pPush == g_pPushNext);
    ASSERT(g_PushState == PUSH_STATE_START);

    pPush[0] = PUSHER_METHOD(subch, method, count);

    g_PushIndex += count + 1;
    g_pPushNext += count + 1;

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// PushMatrixTransposed - debug

VOID PushMatrixTransposed(
    PPUSH pPush,
    DWORD method,
    CONST D3DMATRIX* pMatrix)
{
    ASSERT(pPush == g_pPushNext);
    ASSERT(g_PushState == PUSH_STATE_START);

    DumpMatrixTransposed(pPush, PUSHER_METHOD(SUBCH_3D, method, 16), pMatrix);

    g_PushIndex += 17;
    g_pPushNext += 17;

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// PushInverseModelViewMatrix - debug

VOID PushInverseModelViewMatrix(
    PPUSH pPush,
    DWORD method,
    CONST D3DMATRIX* pMatrix)
{
    ASSERT(pPush == g_pPushNext);
    ASSERT(g_PushState == PUSH_STATE_START);

    *pPush = PUSHER_METHOD(SUBCH_3D, method, 12);
    memcpy((void*) (pPush + 1), pMatrix, 12 * sizeof(float));

    g_PushIndex += 13;
    g_pPushNext += 13;

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// PushMatrix - debug

VOID PushMatrix(
    PPUSH pPush,
    DWORD method,
    FLOAT m11, FLOAT m12, FLOAT m13, FLOAT m14,
    FLOAT m21, FLOAT m22, FLOAT m23, FLOAT m24,
    FLOAT m31, FLOAT m32, FLOAT m33, FLOAT m34,
    FLOAT m41, FLOAT m42, FLOAT m43, FLOAT m44)
{
    ASSERT(pPush == g_pPushNext);
    ASSERT(g_PushState == PUSH_STATE_START);

    *pPush = PUSHER_METHOD(SUBCH_3D, method, 16);
    *((FLOAT*) pPush + 1) = m11;
    *((FLOAT*) pPush + 2) = m12;
    *((FLOAT*) pPush + 3) = m13;
    *((FLOAT*) pPush + 4) = m14;
    *((FLOAT*) pPush + 5) = m21;
    *((FLOAT*) pPush + 6) = m22;
    *((FLOAT*) pPush + 7) = m23;
    *((FLOAT*) pPush + 8) = m24;
    *((FLOAT*) pPush + 9) = m31;
    *((FLOAT*) pPush + 10) = m32;
    *((FLOAT*) pPush + 11) = m33;
    *((FLOAT*) pPush + 12) = m34;
    *((FLOAT*) pPush + 13) = m41;
    *((FLOAT*) pPush + 14) = m42;
    *((FLOAT*) pPush + 15) = m43;
    *((FLOAT*) pPush + 16) = m44;

    g_PushIndex += 17;
    g_pPushNext += 17;

    ASSERT(g_PushIndex <= g_ReservedSize);
}

//------------------------------------------------------------------------------
// The debug XMETAL pusher routines just defer to the subchanneled routines.

extern "C"
VOID WINAPI XMETAL_PushedRaw(PPUSH pPushNext)
{
    PushedRaw(pPushNext);
}

extern "C"
VOID WINAPI XMETAL_Push1(PPUSH pPush, DWORD method, DWORD value)
{
    Push1(pPush, SUBCH_3D, method, value);
}

extern "C"
VOID WINAPI XMETAL_Push2(PPUSH pPush, DWORD method, DWORD a, DWORD b)
{
    Push2(pPush, SUBCH_3D, method, a, b);
}

extern "C"
VOID WINAPI XMETAL_Push3(PPUSH pPush, DWORD method, DWORD a, DWORD b, DWORD c)
{
    Push3(pPush, SUBCH_3D, method, a, b, c);
}

extern "C"
VOID WINAPI XMETAL_Push4(PPUSH pPush, DWORD method, DWORD a, DWORD b, DWORD c, DWORD d)
{
    Push4(pPush, SUBCH_3D, method, a, b, c, d);
}

extern "C"
VOID WINAPI XMETAL_Push1f(PPUSH pPush, DWORD method, FLOAT value)
{
    Push1f(pPush, SUBCH_3D, method, value);
}

extern "C"
VOID WINAPI XMETAL_Push2f(PPUSH pPush, DWORD method, FLOAT a, FLOAT b)
{
    Push2f(pPush, SUBCH_3D, method, a, b);
}

extern "C"
VOID WINAPI XMETAL_Push3f(PPUSH pPush, DWORD method, FLOAT a, FLOAT b, FLOAT c)
{
    Push3f(pPush, SUBCH_3D, method, a, b, c);
}

extern "C"
VOID WINAPI XMETAL_Push4f(PPUSH pPush, DWORD method, FLOAT a, FLOAT b, FLOAT c, FLOAT d)
{
    Push4f(pPush, SUBCH_3D, method, a, b, c, d);
}

extern "C"
VOID WINAPI XMETAL_PushCount(PPUSH pPush, DWORD method, DWORD count)
{
    PushCount(pPush, SUBCH_3D, method, count);
}

#endif // DBG

} // end of namespace
