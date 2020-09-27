/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       pushres.cpp
 *  Content:    Contains all functionality for handling push-buffer resources.
 *
 ***************************************************************************/
 
#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

// We reserve 4 bytes of the push-buffer buffer for putting the JUMP
// instruction to jump back to the main push-buffer:

#define PUSHBUFFER_RETURN_RESERVE 4

// We use this flag with the Common field of a fix-up object on debug
// builds to mark when we're a Begin/End bracket:

#define D3DFIXUP_COMMON_IN_BEGIN_END 0x80000000

//------------------------------------------------------------------------------
// RunPushBufferParameterCheck

VOID RunPushBufferParameterCheck(
    D3DPushBuffer* pPushBuffer,
    D3DFixup* pFixup) // Can be NULL
{
    if (pPushBuffer->Size == 0)
    {
        DPF_ERR("Zero push-buffer 'Size' (maybe failed recording?)");
    }
    if (pPushBuffer->Size & 3)
    {
        DPF_ERR("Push-buffer 'Size' not a dword multiple");
    }
    if (pFixup != NULL)
    {
        CHECK(pFixup, "D3DDevice_RunPushBuffer");
        if (pFixup->Common & D3DFIXUP_COMMON_IN_BEGIN_END)
        {
            DPF_ERR("Can't RunPushBuffer with a Fixup object that is "
                    "in the middle of a begin/end bracket");
        }
        if (pFixup->Next > pFixup->Size)
        {
            DPF_ERR("The last fix-up overflowed the Fixup object");
        }
        DWORD* pFix = (DWORD*) (pFixup->Data + pFixup->Run);
        if (*pFix == 0xffffffff)
        {
            // Our 'firstModify' calculation depends on this:

            DPF_ERR("Can't have empty fix-up buffers");
        }

        DWORD previousOffset = 0;
        while (*pFix != 0xffffffff)
        {
            DWORD size = *(pFix);
            DWORD offset = *(pFix + 1);

            if (size >= pPushBuffer->Size)
            {
                DPF_ERR("Size in 'Fixup' larger than push-buffer size");
            }
            if (offset & 3)
            {
                DPF_ERR("Non-dword-aligned offset in 'Fixup'");
            }
            if (size & 3)
            {
                DPF_ERR("Non-dword-multiple size in 'Fixup'");
            }
            if (offset >= pPushBuffer->Size)
            {
                DPF_ERR("Offset in 'Fixup' larger than push buffer size");
            }
            if (offset < previousOffset)
            {
                DPF_ERR("Not in order of increasing offsets");
            }

            previousOffset = offset;
            pFix = (DWORD*) ((BYTE*) pFix + size + 8);
        }
    }

    if (!(pPushBuffer->Common & D3DPUSHBUFFER_RUN_USING_CPU_COPY))
    {
        AssertContiguous((VOID*) pPushBuffer->Data);
        AssertWriteCombined((VOID*) pPushBuffer->Data);
    }
    else
    {
        if (pFixup != NULL)
        {
            DPF_ERR("D3DPUSHBUFFER_RUN_USING_CPU_COPY push-buffers can't "
                    "take a fixup object (apply the fixups directly)");
        }
    }
}

//------------------------------------------------------------------------------
// D3DDevice_RunPushBuffer

extern "C"
VOID WINAPI D3DDevice_RunPushBuffer(
    D3DPushBuffer* pPushBuffer,
    D3DFixup* pFixup) // Can be NULL
{
#if PROFILE

    COUNT_API(API_D3DDEVICE_RUNPUSHBUFFER);
    ___D3DPERFEVENT _PerfEvent(D3DPERFEvent_RunPushBuffer, TRUE);

    // Store size of the pushbuffer instead of the space taken in pushbuffer
    if(_PerfEvent.m_pD3DPerfEvent)
        _PerfEvent.m_pD3DPerfEvent->Data = pPushBuffer->Size;

#endif

    // Note: we can get the size of this push buffer, but if other push-buffers
    // are called from this one we don't account for those.

    COUNT_ADD_PERF(PERF_RUNPUSHBUFFER_BYTES, pPushBuffer->Size);
    CHECK(pPushBuffer, "D3DDevice_RunPushBuffer");

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        RunPushBufferParameterCheck(pPushBuffer, pFixup);
    }

#if DBG

    // We disable the debug-only SetDmaRange logic on the first call to
    // RunPushBuffer, because it's too hard to deal with SetRenderTarget
    // fix-ups when there's a context DMA involved.  Calling SetRenderTarget
    // with STATE_RUNPUSHBUFFERWASCALLED set will do this for us:

    if (!(pDevice->m_StateFlags & STATE_RUNPUSHBUFFERWASCALLED) &&
        !(pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER))
    {
        pDevice->m_StateFlags |= STATE_RUNPUSHBUFFERWASCALLED;

        D3DDevice_SetRenderTarget(pDevice->m_pRenderTarget, pDevice->m_pZBuffer);
    }

#endif

    // Make sure that the push-buffer properly inherits any lazy state:

    pDevice->SetStateVB(0);

    // Take radically different code paths depending on whether the memory
    // is write-combined and contiguous, or not:

    if (!(pPushBuffer->Common & D3DPUSHBUFFER_RUN_USING_CPU_COPY))
    {
        DWORD pushbufferSize = pPushBuffer->Size - PUSHBUFFER_RETURN_RESERVE;
        BYTE* pStart = (BYTE*) pPushBuffer->Data;

        // Record the reference to the push-buffer so that it's not deleted
        // or modified before the GPU gets to it:

        pDevice->RecordResourceReadPush(pPushBuffer);

        DWORD* pFixupData = NULL;
        DWORD firstModify = pushbufferSize;
        if (pFixup != NULL)
        {
            pDevice->RecordResourceReadPush(pFixup);

            pFixupData = (DWORD*) (pFixup->Data + pFixup->Run);

            // First DWORD is 'size', second DWORD is 'offset':

            firstModify = pFixupData[1];
        }

        if (!(pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER) && 
            !(pDevice->IsTimePending(pDevice->m_LastRunPushBufferTime)))
        {
            CMiniport::PUSHBUFFERFIXUPINFO fixupInfo;

            // Yahoo, no push-buffer resource is currently in use by the GPU.
            // That means we can apply the fix-ups directly and save us
            // the cost of an interrupt:

            PPUSH pPush = pDevice->StartPush();

            *pPush++ = PUSHER_JUMP(GetGPUAddressFromWC(pStart));

            fixupInfo.pFixupData = pFixupData;
            fixupInfo.pStart = pStart;
            fixupInfo.ReturnOffset = pushbufferSize;
            fixupInfo.ReturnAddress = (DWORD*) pPush;

            pDevice->m_Miniport.FixupPushBuffer(&fixupInfo, NVX_PUSH_BUFFER_RUN);

            PushedRaw(pPush);
            pDevice->EndPush(pPush);
        }
        else
        {
            DWORD method;
            DWORD jump;

            // We use PUSHER_CALL instead of PUSHER_JUMP in some cases in
            // order to have GpuGet() work properly.  A PUSHER_CALL has the
            // nice property that the hardware's FIFO getter will 
            // automatically update the NV_PFIFO_CACHE1_DMA_SUBROUTINE
            // register with the push-buffer address from which it was
            // called.  We use that register to determine where in the main
            // push-buffer we are if the GPU is currently running a called 
            // push-buffer.
            //
            // We can't use the pusher 'return' instruction because that's
            // broken on the NV2A.  So FixupPushBuffer will handle patching
            // the return address instead.
            //
            // We don't use PUSHER_CALL for nested push-buffers, because we
            // only ever want NV_PFIFO_CACHE1_DMA_SUBROUTINE to point to
            // an address in the main push-buffer.  Similarly, we avoid
            // NVX_PUSH_BUFFER_RUN in the case of a nested push-buffer so 
            // that the pusher code doesn't get confused about 
            // m_PusherGetRunTotal.

            if (pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER)
            {
                method = NVX_PUSH_BUFFER_FIXUP;
                jump = PUSHER_JUMP(GetGPUAddressFromWC(pStart));
            }
            else
            {
                method = NVX_PUSH_BUFFER_RUN;                    
                jump = PUSHER_CALL(GetGPUAddressFromWC(pStart)); 
            }

            // Darn, a push-buffer is already in use by the GPU.  That
            // means we'll have to do more work and post an interrupt that
            // will handle the fix-up:

            if (DBG_CHECK(firstModify <= PUSHER_MINIMUM_RUN_OFFSET / 2))
            {
                DPF_ERR("Push-buffer size too small or fixup too close to start\n"
                        "(regenerate push-buffer data and fixups?)");
            }
    
            PPUSH pPush = pDevice->StartPush(); 
    
            // NVX_PUSH_BUFFER_FIXUP_POINTER Data stored in 
            // NV097_SET_ZSTENCIL_CLEAR_VALUE
    
            Push1(pPush, NV097_SET_ZSTENCIL_CLEAR_VALUE, 
    
                  // Store PushBufferFixupInfo after the pushbuffer jump
    
                  (DWORD)(pPush + 5));
    
            Push1(pPush + 2, NV097_NO_OPERATION, method);
    
            *(pPush + 4) = jump;

            volatile CMiniport::PUSHBUFFERFIXUPINFO* pFixupInfo
                = (CMiniport::PUSHBUFFERFIXUPINFO*) (pPush + 5);

            pPush = (PPUSH) (pFixupInfo + 1);

            pFixupInfo->pFixupData = pFixupData;
            pFixupInfo->pStart = pStart;
            pFixupInfo->ReturnOffset = pushbufferSize;
            pFixupInfo->ReturnAddress = (DWORD*) pPush;
    
            PushedRaw(pPush);
            pDevice->EndPush(pPush);
        }
        
        if (!(pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER))
        {
            // Also remember the time so that we can tell if a RunPushBuffer
            // command is pending with the GPU:

            pDevice->m_LastRunPushBufferTime = pDevice->m_CpuTime;

            // Let the pusher code know the number of bytes called so that it
            // can be smarter about fences around RunPushBuffer calls:

            pDevice->m_PusherPutRunTotal += pushbufferSize;

            // For better resource time granularity with 'large' push-buffers,
            // we insert a fence:
    
            if (pushbufferSize > PUSHER_RUN_FENCE_THRESHOLD)
            {
                SetFence(0);
            }
        }
    }
    else
    {
        DWORD size = pPushBuffer->Size - PUSHBUFFER_RETURN_RESERVE;

        if (DBG_CHECK(TRUE))
        {
            if (size >= pDevice->m_PushBufferSize / 2)
            {
                DPF_ERR("D3DPUSHBUFFER_RUN_USING_CPU_COPY push-bufffers can't "
                        "be bigger than \nhalf of the main push-buffer size as "
                        "set by SetPushBufferSize\n");
            }
        }

        DWORD dwCount = size >> 2;

        PPUSH pPush = pDevice->StartPush(dwCount);

        FastCopyToWC(pPush, (DWORD*) pPushBuffer->Data, dwCount);

        PushedRaw(pPush + dwCount);

        pDevice->EndPush(pPush + dwCount);

        // Note that we don't call RecordResourceReadPush for the copy case.
    }
}

//------------------------------------------------------------------------------
// D3DDevice_CreatePushBuffer

extern "C"
HRESULT WINAPI D3DDevice_CreatePushBuffer(
    UINT Size,
    BOOL RunUsingCpuCopy,
    D3DPushBuffer** ppPushBuffer)
{
    COUNT_API(API_D3DDEVICE_CREATEPUSHBUFFER);

    if (DBG_CHECK(TRUE))
    {
        if (Size & 3)
        {
            DPF_ERR("Buffer allocation size must be a multiple of 4");
        }
        if (RunUsingCpuCopy > TRUE)
        {
            DPF_ERR("Invalid value for RunUsingCpuCopy");
        }
    }

    DWORD memAllocSize = (RunUsingCpuCopy) 
                       ? sizeof(D3DPushBuffer) + Size 
                       : sizeof(D3DPushBuffer);

    D3DPushBuffer* pPushBuffer = (D3DPushBuffer*) MemAllocNoZero(memAllocSize);
    if (!pPushBuffer)
    {
        return E_OUTOFMEMORY;
    }

    ZeroMemory(pPushBuffer, sizeof(D3DPushBuffer));

    pPushBuffer->Common = /* initial refcount */ 1
                        | D3DCOMMON_TYPE_PUSHBUFFER
                        | D3DCOMMON_D3DCREATED;

    if (RunUsingCpuCopy)
    {
        pPushBuffer->Common |= D3DPUSHBUFFER_RUN_USING_CPU_COPY;
        pPushBuffer->Data = (DWORD) (pPushBuffer + 1);
    }
    else
    {
        VOID *pData = AllocateContiguousMemory(Size, D3DPUSHBUFFER_ALIGNMENT);
        if (!pData)
        {
            MemFree(pPushBuffer);
            return E_OUTOFMEMORY;
        }

        // 'Data' for a push-buffer is always the virtual address:

        pPushBuffer->Data = (DWORD) pData;
    }

    pPushBuffer->AllocationSize = Size;

    *ppPushBuffer = pPushBuffer;

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DDevice_BeginPushBuffer

extern "C"
VOID WINAPI D3DDevice_BeginPushBuffer(
    D3DPushBuffer* pPushBuffer)
{
    COUNT_API(API_D3DDEVICE_BEGINPUSHBUFFER);
    CHECK(pPushBuffer, "D3DDevice_BeginPushBuffer");

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if ((pPushBuffer->Data == 0) || (pPushBuffer->Data & 3))
        {
            DPF_ERR("Buffer pointer can't be NULL and must be dword aligned");
        }
        if (pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER)
        {
            DPF_ERR("Already recording a push-buffer, BeginPushBuffer fails");
        }
        if (pPushBuffer->AllocationSize & 3)
        {
            DPF_ERR("Buffer allocation size must be a multiple of 4");
        }
        if (pPushBuffer->AllocationSize <= PUSHER_THRESHOLD_SIZE * sizeof(DWORD))
        {
            DPF_ERR("Buffer size (AllocationSize) must be more than 512 bytes "
                    "to use BeginPushBuffer");
        }
        if (pDevice->m_IndexBase != 0)
        {
            DPF_ERR("Can't call BeginPushBuffer when BaseVertexIndex as set by "
                    "SetIndices is non-zero");
        }
    }

    // Set any lazy state now so that no state set before BeginPushBuffer
    // gets recorded into the push-buffer:

    pDevice->SetStateVB(0);

    // Make sure we don't overwrite the previous contents if they're still
    // being used:

    BlockOnNonSurfaceResource(pPushBuffer);

    // Remember our inputs:

    pDevice->m_PushBufferRecordWrapSize = 0;
    pDevice->m_pPushBufferRecordResource = pPushBuffer;

    InternalAddRef(pPushBuffer);

    // Now hijack the push-buffer, remembering to save our current state
    // first:

    pDevice->m_pPushBufferRecordSavedPut = pDevice->m_Pusher.m_pPut;
    pDevice->m_pPushBufferRecordSavedThreshold = pDevice->m_Pusher.m_pThreshold;

    // Leave some room at the end for the 'JUMP' instruction:

    pDevice->m_Pusher.m_pPut = (DWORD*) pPushBuffer->Data;
    pDevice->m_Pusher.m_pThreshold 
        = (DWORD*) (pPushBuffer->Data 
                  + pPushBuffer->AllocationSize 
                  - PUSHBUFFER_RETURN_RESERVE)
        - PUSHER_THRESHOLD_SIZE;

    // Remember that we're recording:

    pDevice->m_StateFlags |= STATE_RECORDPUSHBUFFER;

    ASSERT(!(pDevice->m_StateFlags & STATE_RECORDPUSHBUFFERPADDED));
}

//------------------------------------------------------------------------------
// D3DDevice_EndPushBuffer

extern "C"
HRESULT WINAPI D3DDevice_EndPushBuffer()
{
    DWORD unused;

    COUNT_API(API_D3DDEVICE_ENDPUSHBUFFER);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (!(pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER))
        {
            DPF_ERR("Can't call EndPushBuffer without first calling "
                    "BeginPushBuffer!");
        }
    }

    // Make sure we record any pending lazy state changes: 

    pDevice->SetStateVB(0);

    // GetPushBufferOffset has the side effect of padding the push-buffer
    // to the necessary minimum size for us:

    D3DDevice_GetPushBufferOffset(&unused);

    // Initialize the push-buffer structure:

    pDevice->m_pPushBufferRecordResource->Size 
        = pDevice->m_PushBufferRecordWrapSize 
        + (DWORD) pDevice->m_Pusher.m_pPut 
        - pDevice->m_pPushBufferRecordResource->Data
        + PUSHBUFFER_RETURN_RESERVE;

    ASSERT((pDevice->m_pPushBufferRecordResource->Size 
                <= pDevice->m_pPushBufferRecordResource->AllocationSize) ||
           (pDevice->m_PushBufferRecordWrapSize != 0));

    // Stop recording and restore our saved push-buffer state:

    pDevice->m_Pusher.m_pPut = pDevice->m_pPushBufferRecordSavedPut;
    pDevice->m_Pusher.m_pThreshold = pDevice->m_pPushBufferRecordSavedThreshold;

    pDevice->m_StateFlags &= ~(STATE_RECORDPUSHBUFFER | 
                               STATE_RECORDPUSHBUFFERPADDED);

    InternalRelease(pDevice->m_pPushBufferRecordResource);

    // Note that we may have failed even if the buffer was big enough
    // to hold the final result, so we HAVE to check the wrap count:

    if (pDevice->m_PushBufferRecordWrapSize != 0) 
    {
        pDevice->m_pPushBufferRecordResource->Size = 0;
        return D3DERR_BUFFERTOOSMALL;
    }

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DDevice_GetPushBufferOffset

extern "C"
VOID WINAPI D3DDevice_GetPushBufferOffset(
    DWORD* pOffset)
{
    COUNT_API(API_D3DDEVICE_GETPUSHBUFFEROFFSET);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (!(pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER))
        {
            DPF_ERR("GetPushBufferOffset can only be called while recording "
                    "a push-buffer");
        }
    }

    DWORD offset = (DWORD) pDevice->m_Pusher.m_pPut 
                 - pDevice->m_pPushBufferRecordResource->Data;

    // We may need to pad the push-buffer if we haven't done it already,
    // and it's not a RUN_USING_CPU_COPY one:

    INT nops = (PUSHER_MINIMUM_RUN_OFFSET - (INT) offset) / 8;

    if ((nops > 0) &&
        !(pDevice->m_StateFlags & STATE_RECORDPUSHBUFFERPADDED) &&
        !(pDevice->m_pPushBufferRecordResource->Common & 
            D3DPUSHBUFFER_RUN_USING_CPU_COPY))
    {
        pDevice->m_StateFlags |= STATE_RECORDPUSHBUFFERPADDED;

        // The GPU read-ahead is 1K, so we can't have any fixups
        // within 1K of the start of the push-buffer.   So we
        // always pad with NV097_NO_OPERATIONs to the 1K mark.  But 
        // the GPU caches the read-ahead in (offset, value) pairs, so
        // we can get away with only 512 bytes by using the NOINC
        // operation.
        //
        // (Note that we have to use NV097_NO_OPERATION and not
        // PUSHER_NOP in this case.)

        PPUSH pPush = pDevice->StartPush(nops + 1);

        PushCount(pPush, PUSHER_NOINC(NV097_NO_OPERATION), nops);
        ZeroMemory((void*) (pPush + 1), nops * sizeof(DWORD));

        pDevice->EndPush(pPush + nops + 1);

        offset = (DWORD) pDevice->m_Pusher.m_pPut 
               - pDevice->m_pPushBufferRecordResource->Data;
    }

    *pOffset = offset;
}

//------------------------------------------------------------------------------
// Fix-up helper functions

#if !DBG

    FORCEINLINE VOID EndFixup(BOOL IsRaw = FALSE)
    {
    }
    FORCEINLINE VOID Fixup1(DWORD* pFixup, DWORD method, DWORD a)
    {
        method = PUSHER_METHOD(SUBCH_3D, method, 1);
        pFixup[0] = method;
        pFixup[1] = a;
    }
    FORCEINLINE VOID Fixup2(DWORD* pFixup, DWORD method, DWORD a, DWORD b)
    {
        method = PUSHER_METHOD(SUBCH_3D, method, 2);
        pFixup[0] = method;
        pFixup[1] = a;
        pFixup[2] = b;
    }
    FORCEINLINE VOID FixupCount(DWORD* pFixup, DWORD method, DWORD count)
    {
        method = PUSHER_METHOD(SUBCH_3D, method, count);
        pFixup[0] = method;
    }
    FORCEINLINE VOID FixupCount(DWORD* pFixup, SubChannel subch, DWORD method, DWORD count)
    {
        method = PUSHER_METHOD(subch, method, count);
        pFixup[0] = method;
    }
    FORCEINLINE VOID FixupMatrixTransposed(
        DWORD* pFixup, 
        DWORD method, 
        CONST D3DMATRIX* pMatrix)
    {
        DumpMatrixTransposed(pFixup, 
                             PUSHER_METHOD(SUBCH_3D, method, 16),
                             pMatrix);
    }
    FORCEINLINE DWORD ReadBuffer(DWORD* pFixup) 
    {
        return 0xffffffff;
    }

#else

    // Are we in a BeginFixup/EndFixup bracket?

    BOOL g_FixupBracket;

    // Are we in a StartFixup/EndFixup bracket within a fix-up call?

    BOOL g_FixupActive;

    // Current count of dwords we've written within this fix-up call:

    DWORD g_FixupCount;

    // The next place we expect to write within this fix-up call:

    DWORD* g_pNextFixup;

    // Add this value to 'pFixup' (which is where we're currently writing
    // fix-up data) to get a pointer to the corresponding location in the
    // push-buffer, for verifying signatures:

    DWORD g_FixupToBuffer;

    // The size they tell us at StartFixup time has to match watch they
    // stick in the buffer:

    DWORD g_OriginalCount;

    //--------------------------------------------------------------------------
    // ReadBuffer - reads the corresponding address in the original push-buffer

    DWORD ReadBuffer(DWORD* pFixup) 
    {
        ASSERT(g_FixupActive);
        return *(pFixup + g_FixupToBuffer);
    }

    //--------------------------------------------------------------------------
    // EndFixup - analagous to EndPush

    VOID EndFixup(BOOL isRaw = FALSE)
    {
        ASSERT(g_FixupActive);
        ASSERT((isRaw) || (g_FixupCount == g_OriginalCount));

        g_FixupActive = FALSE;
    }

    //--------------------------------------------------------------------------
    // Fixup1 - analagous to Push1

    VOID Fixup1(DWORD* pFixup, DWORD method, DWORD a)
    {
        ASSERT(g_FixupActive);
        ASSERT(pFixup == g_pNextFixup);

        method = PUSHER_METHOD(SUBCH_3D, method, 1);
        if (ReadBuffer(pFixup) != method)
        {
            DPF_ERR("The fix-up signature doesn't match existing data\n"
                    "(A bad offset was specified, or the specified parameters \n"
                    "don't match the signatures of those recorded)");
        }

        g_FixupCount += 2;
        g_pNextFixup += 2;

        pFixup[0] = method;
        pFixup[1] = a;
    }

    //--------------------------------------------------------------------------
    // Fixup2 - analagous to Push2

    VOID Fixup2(DWORD* pFixup, DWORD method, DWORD a, DWORD b)
    {
        ASSERT(g_FixupActive);
        ASSERT(pFixup == g_pNextFixup);

        method = PUSHER_METHOD(SUBCH_3D, method, 2);
        if (ReadBuffer(pFixup) != method)
        {
            DPF_ERR("The fix-up signature doesn't match existing data\n"
                    "(A bad offset was specified, or the specified parameters \n"
                    "don't match the signatures of those recorded)");
        }

        g_FixupCount += 3;
        g_pNextFixup += 3;

        pFixup[0] = method;
        pFixup[1] = a;
        pFixup[2] = b;
    }

    //--------------------------------------------------------------------------
    // FixupCount - analagous to PushCount

    VOID FixupCount(DWORD* pFixup, SubChannel subch, DWORD method, DWORD count)
    {
        ASSERT(g_FixupActive);
        ASSERT(pFixup == g_pNextFixup);

        method = PUSHER_METHOD(subch, method, count);
        if (ReadBuffer(pFixup) != method)
        {
            DPF_ERR("The fix-up signature doesn't match existing data\n"
                    "(A bad offset was specified, or the specified parameters \n"
                    "don't match the signatures of those recorded)");
        }

        g_FixupCount += (count + 1);
        g_pNextFixup += (count + 1);

        pFixup[0] = method;
    }

    FORCEINLINE VOID FixupCount(DWORD* pFixup, DWORD method, DWORD count)
    {
        FixupCount(pFixup, SUBCH_3D, method, count);
    }

    //--------------------------------------------------------------------------
    // FixupMatrixTransposed - analagous to PushMatrixTransposed

    VOID FixupMatrixTransposed(
        DWORD* pFixup, 
        DWORD method, 
        CONST D3DMATRIX* pMatrix)
    {
        ASSERT(g_FixupActive);
        ASSERT(pFixup == g_pNextFixup);

        method = PUSHER_METHOD(SUBCH_3D, method, 16);
        if (ReadBuffer(pFixup) != method)
        {
            DPF_ERR("The fix-up signature doesn't match existing data\n"
                    "(A bad offset was specified, or the specified parameters \n"
                    "don't match the signatures of those recorded)");
        }

        DumpMatrixTransposed(pFixup, method, pMatrix);

        g_FixupCount += 17;
        g_pNextFixup += 17;
    }

#endif            

//------------------------------------------------------------------------------
// g_pFixup
//
// While in a BeginFixup/EndFixup bracket, points to the fix-up object (if any).

D3DFixup* g_pFixup;

//------------------------------------------------------------------------------
// AddToFixup - for early-outs to account for space that would have been used
//    by subsequent StartFixup calls in the same routine

VOID AddToFixup(
    DWORD Count)
{
    D3DFixup* pFixup = g_pFixup;
    if (pFixup != NULL)
    {
        DWORD size = Count * sizeof(DWORD);

        // Reserve 2 dwords of structure overhead for 'size' and 'offset':

        pFixup->Next += (8 + size);
    }
}

//------------------------------------------------------------------------------
// StartFixup - analagous to StartPush
//
// Handles the logic of setting up to write the fix-up data either to a
// fix-up array, or directly to the push-buffer data.
//
// NOTE: Once this function returns, you have to write ALL 'Count' DWORDS
//       to the buffer (you can't have any gaps in your writes, due to the
//       nature of the fix-up buffer).

DWORD* StartFixup(
    D3DPushBuffer* pPushBuffer,
    UINT Offset,
    DWORD Count)
{
    DWORD* pSrc = (DWORD*) ((BYTE*) pPushBuffer->Data + Offset);
    DWORD* pDst = pSrc;

    if (DBG_CHECK(TRUE))
    {
        if (DBG_CHECK(!g_FixupBracket))
        {
            DPF_ERR("Fix-up routine called without first calling BeginFixup");
        }
        if (pPushBuffer->Size == 0)
        {
            DPF_ERR("Zero push-buffer 'Size' (maybe failed recording?)");
        }
        if (pPushBuffer->Size & 3)
        {
            DPF_ERR("Push-buffer 'Size' not a dword multiple");
        }
        if (Offset >= pPushBuffer->Size)
        {
            DPF_ERR("Specified offset is larger than push-buffer");
        }
    }

    D3DFixup* pFixup = g_pFixup;
    if (pFixup != NULL)
    {
        DWORD* pFix = (DWORD*) (pFixup->Data + pFixup->Next);
        DWORD size = Count * sizeof(DWORD);

        // Reserve 2 dwords of structure overhead for 'size' and 'offset':

        pFixup->Next += (8 + size);
        if (pFixup->Next > pFixup->Size)
            return NULL;

        pFix[0] = size;
        pFix[1] = Offset;

        pDst = &pFix[2];
    }

#if DBG

    // Note that this assumes fix-ups aren't multi-threaded, but I think
    // this is okay, particularly since it's only on debug builds (meaning
    // this won't cause mysterious free builds crashes).

    ASSERT(!g_FixupActive);
    g_pNextFixup = pDst;
    g_FixupToBuffer = (pSrc - pDst);
    g_FixupActive = TRUE;
    g_FixupCount = 0;
    g_OriginalCount = Count;

#endif

    return pDst;
}

//------------------------------------------------------------------------------
// D3DPushBuffer_RunPushBuffer
//
// Note that this can't handle D3DPUSHBUFFER_RUN_USING_CPU_COPY cases for
// either the source or the destination.

extern "C"
VOID WINAPI D3DPushBuffer_RunPushBuffer(
    D3DPushBuffer* pPushBuffer,
    UINT Offset,
    D3DPushBuffer* pDestinationPushBuffer,
    D3DFixup* pDestinationFixup)
{
    COUNT_API(API_D3DPUSHBUFFER_RUNPUSHBUFFER);
    CHECK(pPushBuffer, "D3DPushBuffer_RunPushBuffer");

    if (DBG_CHECK(TRUE))
    {
        RunPushBufferParameterCheck(pDestinationPushBuffer, pDestinationFixup);

        if ((pPushBuffer->Common & D3DPUSHBUFFER_RUN_USING_CPU_COPY) ||
            (pDestinationPushBuffer->Common & D3DPUSHBUFFER_RUN_USING_CPU_COPY))
        {
            DPF_ERR("Can't handle D3DPUSHBUFFER_RUN_USING_CPU_COPY for either "
                    "source or \ndestination push-buffer types");
        }
    }

    DWORD infoDwords = sizeof(CMiniport::PUSHBUFFERFIXUPINFO) / sizeof(DWORD);
    DWORD* pFixup = StartFixup(pPushBuffer, Offset, 5 + infoDwords);
    if (!pFixup)
        return;

    CMiniport::PUSHBUFFERFIXUPINFO PushBufferFixupInfo;

    PushBufferFixupInfo.pStart = (BYTE *)pDestinationPushBuffer->Data;
    PushBufferFixupInfo.ReturnOffset =
        pDestinationPushBuffer->Size - PUSHBUFFER_RETURN_RESERVE;

    DWORD firstModify = PushBufferFixupInfo.ReturnOffset;
    DWORD* pFix = NULL;

    if (pDestinationFixup != 0)
    {
        pFix = (DWORD*) (pDestinationFixup->Data + pDestinationFixup->Run);
        firstModify = *pFix;
    }

    PushBufferFixupInfo.pFixupData = pFix;
    PushBufferFixupInfo.ReturnAddress = 
        (DWORD*) (pPushBuffer->Data + Offset + (5 + infoDwords)*sizeof(DWORD));

    // NVX_PUSH_BUFFER_FIXUP_POINTER Data stored in NV097_SET_ZSTENCIL_CLEAR_VALUE

    Fixup1(pFixup, 
           NV097_SET_ZSTENCIL_CLEAR_VALUE, 
           pPushBuffer->Data + Offset + 5*sizeof(DWORD));

    Fixup1(pFixup + 2,
           NV097_NO_OPERATION,
           NVX_PUSH_BUFFER_FIXUP);

    pFixup += 4;

    // Make sure the JUMP instruction was there:

    if (DBG_CHECK((ReadBuffer(pFixup) & 3) != 1))
    {
        DPF_ERR("Signature mismatch - Offset or push-buffer size may "
                "different from what was recorded.");
    }

    *pFixup = PUSHER_JUMP(GetGPUAddressFromWC(PushBufferFixupInfo.pStart));
    *(CMiniport::PUSHBUFFERFIXUPINFO *)(pFixup + 1) = PushBufferFixupInfo;

    EndFixup(TRUE);
}

//------------------------------------------------------------------------------
// D3DPushBuffer_SetModelView

extern "C"
VOID WINAPI D3DPushBuffer_SetModelView(
    D3DPushBuffer* pPushBuffer,
    UINT Offset,
    CONST D3DMATRIX* pModelView,
    CONST D3DMATRIX* pInverseModelView,
    CONST D3DMATRIX* pComposite)
{
    COUNT_API(API_D3DPUSHBUFFER_SETMODELVIEW);
    CHECK(pPushBuffer, "D3DPushBuffer_SetModelView");

    DWORD fixupCount = (pInverseModelView) ? (17*2 + 13) : (17*2);
    DWORD* pFixup = StartFixup(pPushBuffer, Offset, fixupCount);
    if (!pFixup)
        return;

    FixupMatrixTransposed(pFixup, NV097_SET_MODEL_VIEW_MATRIX0(0), pModelView);

    FixupMatrixTransposed(pFixup + 17, NV097_SET_COMPOSITE_MATRIX(0), pComposite);

    if (pInverseModelView)
    {
        FixupCount(pFixup + 34, NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0), 12);

        memcpy(pFixup + 35, pInverseModelView, 12 * sizeof(DWORD));
    }

    EndFixup();
}

//------------------------------------------------------------------------------
// D3DPushBuffer_SetVertexBlendModelView

extern "C"
VOID WINAPI D3DPushBuffer_SetVertexBlendModelView(
    D3DPushBuffer* pPushBuffer,
    UINT Offset,
    UINT Count,
    CONST D3DMATRIX* pModelViews,
    CONST D3DMATRIX* pInverseModelViews,
    CONST D3DMATRIX* pProjectionViewport)
{
    DWORD i;

    COUNT_API(API_D3DPUSHBUFFER_SETVERTEXBLENDMODELVIEW);
    CHECK(pPushBuffer, "D3DPushBuffer_SetVertexBlendModelView");

    DWORD fixupCount = 17 + 17 * Count;
    if (pInverseModelViews)
        fixupCount += 13 * Count;

    DWORD* pFixup = StartFixup(pPushBuffer, Offset, fixupCount);
    if (!pFixup)
        return;

    for (i = 0; i < Count; i++)
    {
        DWORD stride = NV097_SET_MODEL_VIEW_MATRIX1(0) 
                     - NV097_SET_MODEL_VIEW_MATRIX0(0);

        FixupMatrixTransposed(pFixup, 
                              NV097_SET_MODEL_VIEW_MATRIX0(0) + (i * stride),
                              &pModelViews[i]);

        pFixup += 17;
    }

    if (pInverseModelViews)
    {
        for (i = 0; i < Count; i++)
        {
            DWORD stride = NV097_SET_INVERSE_MODEL_VIEW_MATRIX1(0) 
                         - NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0);

            FixupCount(pFixup, 
                       NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0) + (i * stride), 
                       12);

            memcpy(pFixup + 1, &pInverseModelViews[i], 12 * sizeof(DWORD));

            pFixup += 13;
        }
    }

    FixupMatrixTransposed(pFixup,
                          NV097_SET_COMPOSITE_MATRIX(0),
                          pProjectionViewport);

    pFixup += 17;

    EndFixup();
}

//------------------------------------------------------------------------------
// D3DPushBuffer_SetVertexShaderInputDirect

extern "C"
void WINAPI D3DPushBuffer_SetVertexShaderInputDirect(
    D3DPushBuffer* pPushBuffer,
    UINT Offset,
    D3DVERTEXATTRIBUTEFORMAT *pVAF,
    UINT StreamCount,
    CONST D3DSTREAM_INPUT *pStreamInputs)
{
    // This API call is counted as an API_D3DPUSHBUFFER_SETVERTEXSHADERINPUT
    
    if (DBG_CHECK(TRUE))
    {
        if(pVAF == NULL)
        {
            DPF_ERR("NULL D3DVERTEXATTRIBUTEFORMAT is invalid in SetVertexShaderInputDirect");
        }
#if DBG
        CheckVertexAttributeFormatStruct(pVAF);
#endif    
    }

    memcpy(&(g_DirectVertexShader.Slot[0]), pVAF, sizeof(VertexShaderSlot)*16);
    D3DPushBuffer_SetVertexShaderInput(pPushBuffer, Offset, (DWORD)(&g_DirectVertexShader) | D3DFVF_RESERVED0, StreamCount, pStreamInputs);
}

//------------------------------------------------------------------------------
// D3DPushBuffer_SetVertexShaderInput

extern "C"
VOID WINAPI D3DPushBuffer_SetVertexShaderInput(
    D3DPushBuffer* pPushBuffer,
    UINT Offset,
    DWORD Handle,
    UINT StreamCount,
    CONST D3DSTREAM_INPUT *pStreamInputs)
{
    DWORD i;

    COUNT_API(API_D3DPUSHBUFFER_SETVERTEXSHADERINPUT);
    CHECK(pPushBuffer, "D3DPushBuffer_SetVertexInput");

    VertexShader *pVertexShader = (VertexShader*) (Handle - D3DFVF_RESERVED0);

    if (DBG_CHECK(TRUE))
    {
        if (!IsVertexProgram(Handle))
        {
            DPF_ERR("Must be a vertex shader program, not an FVF");
        }
    #if DBG
        if (pVertexShader->Signature != 'Vshd')
        {
            DPF_ERR("Invalid vertex shader object (already deleted?)");
        }
    #endif
        if ((StreamCount == 0) || (StreamCount > 15))
        {
            DPF_ERR("Stream count can't be 0 or more than 15");
        }
        for (i = 0; i < StreamCount; i++)
        {
            if (pStreamInputs[i].VertexBuffer == NULL)
            {
                DPF_ERR("Can't specify NULL VertexBuffer");
            }
            if (pStreamInputs[i].Stride > 255)
            {
                DPF_ERR("Can't have a stride more than 255");
            }
        }
    }

    DWORD* pFixup = StartFixup(pPushBuffer, Offset, 34);
    if (!pFixup)
        return;

    FixupCount(pFixup, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0), 16);
    FixupCount(pFixup + 17, NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0), 16);

    for (i = 0; i < 16; i++)
    {
        VertexShaderSlot *pSlot = &pVertexShader->Slot[i];

        if (DBG_CHECK(pSlot->StreamIndex >= StreamCount))
        {
            DPF_ERR("Vertex Shader references more streams than given to "
                    "SetVertexShaderInput");
        }

        CONST D3DSTREAM_INPUT* pStreamInput = &pStreamInputs[pSlot->StreamIndex];

        DWORD offset = pStreamInput->VertexBuffer->Data
                     + pStreamInput->Offset
                     + pSlot->Offset;

        pFixup[1 + i] = offset;

        DWORD format = (pStreamInput->Stride << 8) + pSlot->SizeAndType;

        pFixup[18 + i] = format;
    }

    EndFixup();
}

//------------------------------------------------------------------------------
// D3DPushBuffer_SetRenderTarget
//
// NOTE: The new render target must be the same format and dimension as the old.

extern "C"
VOID WINAPI D3DPushBuffer_SetRenderTarget(
    D3DPushBuffer* pPushBuffer,
    UINT Offset,
    D3DSurface* pRenderTarget,
    D3DSurface* pZBuffer)
{
    COUNT_API(API_D3DPUSHBUFFER_SETRENDERTARGET);
    CHECK(pPushBuffer, "D3DPushBuffer_SetRenderTarget");

    DWORD* pFixup = StartFixup(pPushBuffer, Offset, 48);
    if (!pFixup)
        return;

    DWORD pitch = PixelJar::GetPitch(pRenderTarget);
    DWORD Zpitch = (pZBuffer) ? PixelJar::GetPitch(pZBuffer) : pitch;

    if (DBG_CHECK(TRUE))
    {
        if (ReadBuffer(pFixup + 3) != 
            (DRF_NUM(097, _SET_SURFACE_PITCH, _COLOR, pitch)
           | DRF_NUM(097, _SET_SURFACE_PITCH, _ZETA, Zpitch)))
        {
            DPF_ERR("Pitch mismatch");
        }
        if (ReadBuffer(pFixup + 3 + 24) != 
            (DRF_NUM(097, _SET_SURFACE_PITCH, _COLOR, pitch)
           | DRF_NUM(097, _SET_SURFACE_PITCH, _ZETA, Zpitch)))
        {
            DPF_ERR("Pitch mismatch");
        }
        if ((ReadBuffer(pFixup + 3 + 16) == 0) && (pZBuffer != NULL))
        {
            DPF_ERR("Z-buffer now but wasn't before");
        }
        if ((ReadBuffer(pFixup + 3 + 16) != 0) && (pZBuffer == NULL))
        {
            DPF_ERR("No z-buffer now but was one before");
        }
    }

    DWORD colorOffset = pRenderTarget->Data;
    DWORD Zoffset = (pZBuffer != NULL) ? pZBuffer->Data : 0;

    for (DWORD iterations = 0; iterations < 2; iterations++)
    {
        Fixup1(pFixup, NV097_NO_OPERATION, 0);
        Fixup1(pFixup + 2, NV097_SET_SURFACE_PITCH,
                     DRF_NUM(097, _SET_SURFACE_PITCH, _COLOR, pitch)
                   | DRF_NUM(097, _SET_SURFACE_PITCH, _ZETA, Zpitch));
        Fixup1(pFixup + 4, NV097_NO_OPERATION, 0);
        Fixup1(pFixup + 6, NV097_WAIT_FOR_IDLE, 0);
        pFixup += 8;

        Fixup1(pFixup, NV097_NO_OPERATION, 0);
        Fixup1(pFixup + 2, NV097_SET_SURFACE_COLOR_OFFSET, colorOffset);
        Fixup1(pFixup + 4, NV097_NO_OPERATION, 0);
        Fixup1(pFixup + 6, NV097_WAIT_FOR_IDLE, 0);
        pFixup += 8;

        Fixup1(pFixup, NV097_NO_OPERATION, 0);
        Fixup1(pFixup + 2, NV097_SET_SURFACE_ZETA_OFFSET, Zoffset);
        Fixup1(pFixup + 4, NV097_NO_OPERATION, 0);
        Fixup1(pFixup + 6, NV097_WAIT_FOR_IDLE, 0);
        pFixup += 8;
    }

    EndFixup();
}

//------------------------------------------------------------------------------
// D3DPushBuffer_SetTexture

extern "C"
VOID WINAPI D3DPushBuffer_SetTexture(
    D3DPushBuffer* pPushBuffer,
    UINT Offset,
    DWORD Stage,
    D3DBaseTexture *pTexture)
{
    COUNT_API(API_D3DPUSHBUFFER_SETTEXTURE);
    CHECK(pPushBuffer, "D3DPushBuffer_SetTexture");

    if (DBG_CHECK(TRUE))
    {
        if ((Offset & 3) || (Offset >= pPushBuffer->Size))
        {
            DPF_ERR("Offset must be multiple of 4 and less than push buffer size");
        }
        if (pTexture == NULL)
        {
            DPF_ERR("Can't take a NULL texture");
        }
    }

    DWORD fixupCount = (pTexture->Size != 0) ? 7 : 3;
    DWORD* pFixup = StartFixup(pPushBuffer, Offset, fixupCount);
    if (!pFixup)
        return;

    if (DBG_CHECK((pTexture->Format ^ ReadBuffer(pFixup + 2)) &
            (DRF_NUM(097, _SET_TEXTURE_FORMAT, _CUBEMAP_ENABLE, ~0) | 
             DRF_NUM(097, _SET_TEXTURE_FORMAT, _DIMENSIONALITY, ~0))))
    {
        DPF_ERR("New texture type doesn't match old");
    }

    Fixup2(pFixup,  
           NV097_SET_TEXTURE_OFFSET(Stage), 
           pTexture->Data, 
           pTexture->Format);

    if (pTexture->Size != 0)
    {
        DWORD width = PixelJar::GetLinearWidth(pTexture);
        DWORD height = PixelJar::GetLinearHeight(pTexture);
        DWORD pitch = PixelJar::GetLinearPitch(pTexture);
        
        Fixup1(pFixup + 3, 
               NV097_SET_TEXTURE_CONTROL1(Stage),
               DRF_NUMFAST(097, _SET_TEXTURE_CONTROL1, _IMAGE_PITCH, pitch));
        
        Fixup1(pFixup + 5, 
               NV097_SET_TEXTURE_IMAGE_RECT(Stage),
               DRF_NUMFAST(097, _SET_TEXTURE_IMAGE_RECT, _WIDTH, width)
               | DRF_NUMFAST(097, _SET_TEXTURE_IMAGE_RECT, _HEIGHT, height));
    }

    EndFixup();
}

//------------------------------------------------------------------------------
// D3DPushBuffer_SetPalette

extern "C"
VOID WINAPI D3DPushBuffer_SetPalette(
    D3DPushBuffer* pPushBuffer,
    UINT Offset,
    DWORD Stage,
    D3DPalette *pPalette)
{
    COUNT_API(API_D3DPUSHBUFFER_SETPALETTE);
    CHECK(pPushBuffer, "D3DPushBuffer_SetPalette");

    if (DBG_CHECK(TRUE))
    {
        if ((Offset & 3) || (Offset >= pPushBuffer->Size))
        {
            DPF_ERR("Offset must be multiple of 4 and less than push buffer size");
        }
    }

    DWORD* pFixup = StartFixup(pPushBuffer, Offset, 2);
    if (!pFixup)
        return;

    DWORD contextAndLength
        = (pPalette->Common >> D3DPALETTE_COMMON_PALETTESET_SHIFT)
        & D3DPALETTE_COMMON_PALETTESET_MASK;

    ASSERT((contextAndLength & 2) == 0);
    ASSERT(contextAndLength <=
           (DRF_DEF(097, _SET_TEXTURE_PALETTE, _LENGTH, _32)
          | DRF_DEF(097, _SET_TEXTURE_PALETTE, _CONTEXT_DMA, _B)));
    ASSERT((pPalette->Data & 63) == 0);

    Fixup1(pFixup, 
           NV097_SET_TEXTURE_PALETTE(Stage),
           pPalette->Data | contextAndLength);

    EndFixup();
}

//------------------------------------------------------------------------------
// D3DPushBuffer_EndVisibilityTest

extern "C"
HRESULT WINAPI D3DPushBuffer_EndVisibilityTest(
    D3DPushBuffer* pPushBuffer,
    UINT Offset,
    DWORD Index)
{
    COUNT_API(API_D3DPUSHBUFFER_ENDVISIBILITYTEST);
    CHECK(pPushBuffer, "D3DPushBuffer_EndVisibilityTest");

    BYTE* virtualAddress = GetVisibilityAddress(Index);
    if (virtualAddress == NULL)
        return E_OUTOFMEMORY;

    DWORD* pFixup = StartFixup(pPushBuffer, Offset, 3);
    if (!pFixup)
        return S_OK;

    Fixup2(pFixup, NV097_SET_ZPASS_PIXEL_COUNT_ENABLE,

          // NV097_SET_ZPASS_PIXEL_COUNT_ENABLE:

          FALSE,

          // NV097_GET_REPORT:
          
          DRF_DEF(097, _GET_REPORT, _TYPE, _ZPASS_PIXEL_CNT)
        | DRF_NUMFAST(097, _GET_REPORT, _OFFSET, GetGPUAddress(virtualAddress)));

    EndFixup();

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DPushBuffer_SetVertexShaderConstant

extern "C"
VOID WINAPI D3DPushBuffer_SetVertexShaderConstant(
    D3DPushBuffer* pPushBuffer,
    UINT Offset,
    INT Register, // Not used
    CONST VOID* pConstantData,
    DWORD ConstantCount)
{
    COUNT_API(API_D3DPUSHBUFFER_SETVERTEXSHADERCONSTANT);
    CHECK(pPushBuffer, "D3DPushBuffer_SetVertexShaderConstant");

    CDevice* pDevice = g_pDevice;

    // Convert to number of DWORDs:

    DWORD totalCount = 4 * ConstantCount;
    DWORD batchCount = (totalCount + 31) / 32;
    CONST DWORD* pData = (CONST DWORD*) pConstantData;

    // Skip the NV097_SET_TRANSFORM_CONSTANT_LOAD load:

    Offset += 8;

    DWORD* pFixup = StartFixup(pPushBuffer, Offset, totalCount + batchCount);
    if (!pFixup)
        return;

    do {
        DWORD thisCount = min(totalCount, 32);

        totalCount -= thisCount;

        FixupCount(pFixup, NV097_SET_TRANSFORM_CONSTANT(0), thisCount);

        memcpy(&pFixup[1], pData, thisCount * sizeof(DWORD));

        pFixup += (thisCount + 1);
        pData += thisCount;

    } while (--batchCount != 0);

    ASSERT(totalCount == 0);

    EndFixup();
}

//------------------------------------------------------------------------------
// D3DPushBuffer_Jump

extern "C"
VOID WINAPI D3DPushBuffer_Jump(
    D3DPushBuffer* pPushBuffer,
    UINT Offset,
    UINT DestinationOffset)
{
    COUNT_API(API_D3DPUSHBUFFER_JUMP);
    CHECK(pPushBuffer, "D3DPushBuffer_SetJump");

    if (DBG_CHECK(TRUE))
    {
        if (pPushBuffer->Common & D3DPUSHBUFFER_RUN_USING_CPU_COPY)
        {
            DPF_ERR("Can't do a SetJump when D3DPUSHBUFFER_RUN_USING_CPU_COPY");
        }
    }

    DWORD* pFixup = StartFixup(pPushBuffer, Offset, 1);
    if (!pFixup)
        return;

    if (DBG_CHECK(TRUE))
    {
        DWORD oldInstruction = ReadBuffer(pFixup);

        if ((oldInstruction != PUSHER_METHOD(SUBCH_3D, NV097_NO_OPERATION, 1)) &&
            ((oldInstruction & 3) != 1))
        {
            DPF_ERR("Offset signature is neither a NOP nor a JUMP instruction");
        }
    }

    if (Offset == DestinationOffset)
    {
        *pFixup = PUSHER_METHOD(SUBCH_3D, NV097_NO_OPERATION, 1);
    }
    else
    {
        *pFixup = PUSHER_JUMP(GetGPUAddressFromWC((VOID*) pPushBuffer->Data) +
                             DestinationOffset);
    }

    EndFixup(TRUE);

    if (!g_pFixup)
    {
        // This is very likely to be called from a DPC to implement a 
        // conditional jump, so we'll do the WC flush now.  
    
        FlushWCCache();
    }
}

//------------------------------------------------------------------------------
// g_ResourceMethods

D3DCONST DWORD g_ResourceMethods[] =
{
    PUSHER_METHOD(SUBCH_3D, NV097_SET_SURFACE_COLOR_OFFSET, 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_SURFACE_ZETA_OFFSET, 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(1), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(2), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(3), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(4), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(5), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(6), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(7), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(8), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(9), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(10), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(11), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(12), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(13), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(14), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_VERTEX_DATA_ARRAY_OFFSET(15), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_TEXTURE_OFFSET(0), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_TEXTURE_OFFSET(1), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_TEXTURE_OFFSET(2), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_TEXTURE_OFFSET(3), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_TEXTURE_PALETTE(0), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_TEXTURE_PALETTE(1), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_TEXTURE_PALETTE(2), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_TEXTURE_PALETTE(3), 0),
    PUSHER_METHOD(SUBCH_3D, NV097_SET_SEMAPHORE_OFFSET, 0),
    PUSHER_METHOD(SUBCH_3D, NV097_GET_REPORT, 0),
};

#define STAMP_VALUE 0xfffffffd // Must have a 'JUMP' encoding

//------------------------------------------------------------------------------
// StampCheckValue

__forceinline VOID StampCheckValue(
    PPUSH pPush,
    BOOL Stamp,
    DWORD Offset)
{
    if (Stamp)
    {
        *pPush = STAMP_VALUE;
    }
    else if (*pPush == STAMP_VALUE)
    {
        DXGRIP("D3DPushBuffer_Verify: Unresolved resource reference "
               "near offset 0x%lx", Offset);
    }
}

//------------------------------------------------------------------------------
// VerifyInstruction

VOID VerifyInstruction(
    DWORD EncodedMethod, // NOTE: This includes the subchannel
    PPUSH pPush,
    DWORD Offset,
    BOOL Stamp)
{
    DWORD subch = EncodedMethod >> 13;

    if (subch != SUBCH_3D)
    {
        DPF_ERR("D3DPushBuffer_Verify: Unexpected sub-channel");
    }
    else if(EncodedMethod == PUSHER_METHOD(SUBCH_3D, NV097_SET_ZSTENCIL_CLEAR_VALUE, 0))
    {
        // NVX_PUSH_BUFFER_FIXUP_POINTER is the SET_ZSTENCIL_CLEAR_VALUE
        // followed by the data dword then a noop with the NVX_* method value.

        if(pPush[1] == PUSHER_METHOD(SUBCH_3D, NV097_NO_OPERATION, 1) &&
            ((pPush[2] == NVX_PUSH_BUFFER_FIXUP) || (pPush[2] == NVX_PUSH_BUFFER_RUN)))
        {
            StampCheckValue(pPush, Stamp, Offset);
        }

    }
    else
    {
        // This is obviously not the fastest search in the world, but what
        // the heck, this is debug only.

        for (DWORD i = 0; 
             i < sizeof(g_ResourceMethods) / sizeof(g_ResourceMethods[0]);
             i++)
        {
            if (EncodedMethod == g_ResourceMethods[i])
            {
                StampCheckValue(pPush, Stamp, Offset);
            }
        }
    }
}

//------------------------------------------------------------------------------
// D3DPushBuffer_Verify

extern "C"
VOID WINAPI D3DPushBuffer_Verify(
    D3DPushBuffer* pPushBuffer,
    BOOL StampResources)
{
    COUNT_API(API_D3DPUSHBUFFER_VERIFY);

    CHECK(pPushBuffer, "D3DPushBuffer_Verify");

#if DBG

    if (DBG_CHECK(TRUE))
    {
        if (StampResources > TRUE)
        {
            DPF_ERR("Invalid value for StampResources");
        }
    }

    PPUSH pPush = (DWORD*) (pPushBuffer->Data);
    PPUSH pPushEnd = (DWORD*) (pPushBuffer->Data + pPushBuffer->Size 
                                - PUSHBUFFER_RETURN_RESERVE);

    while (pPush < pPushEnd)
    {
        DWORD push = *pPush++;
        DWORD instruction = push >> 29;
        DWORD count = (push & 0x1ffc0000) >> 18;
        DWORD encodedMethod = push & 0xffff; // Note we include sub-channel

        ASSERT((push == 0) || (count > 0));
        ASSERT(instruction != 1);

        // In order to be able to stamp 'jumps', the stamp value must look
        // like a valid jump:

        ASSERT((STAMP_VALUE & 3) == 1); 

        if ((push & 3) == 1) // Jump case
        {
            DWORD i;
            DWORD jumpAndInfo = 1 + sizeof(CMiniport::PUSHBUFFERFIXUPINFO) 
                                  / sizeof(DWORD);

            if (StampResources)
            {
                for (i = 0; i < jumpAndInfo; i++)
                {
                    *(pPush - 1) = STAMP_VALUE;
                    pPush++;
                }
            }
            else 
            {
                for (i = 0; i < jumpAndInfo; i++)
                {
                    if (*(pPush - 1) == STAMP_VALUE)
                    {
                        DXGRIP("D3DPushBuffer_Verify: Unresolved Jump reference near "
                               "offset 0x%lx", (DWORD) pPush - pPushBuffer->Data);
                    }
                    pPush++;
                }
            }

            // We wanted to stamp 'jumpAndInfo' dwords, but only advance
            // 'pPush' by 'jumpAndInfo - 1' dwords:

            pPush--;
        }
        else if (instruction == 2) // No-increment case
        {
            VerifyInstruction(encodedMethod, 
                              pPush + count - 1,
                              (DWORD) pPush - pPushBuffer->Data,
                              StampResources);

            pPush += count;
        }
        else if (instruction == 0) // Increment case
        {
            while (count != 0)
            {
                VerifyInstruction(encodedMethod, 
                                  pPush, 
                                  (DWORD) pPush - pPushBuffer->Data,
                                  StampResources);

                pPush++;
                count--;
                encodedMethod += 4;
            }
        }
        else
        {
            RIP(("D3DPushBuffer_Verify: Unexpected push-buffer encoding (corrupted push-buffer?)"));
        }
    }

#endif

}

//------------------------------------------------------------------------------
// D3DDevice_CreateFixup

extern "C"
HRESULT WINAPI D3DDevice_CreateFixup(
    UINT Size,
    D3DFixup** ppFixup)
{
    COUNT_API(API_D3DDEVICE_CREATEFIXUP);

    D3DFixup* pFixup = (D3DFixup*) MemAllocNoZero(sizeof(D3DFixup) + Size);
    if (!pFixup)
    {
        return E_OUTOFMEMORY;
    }

    ZeroMemory(pFixup, sizeof(D3DFixup));

    pFixup->Common = /* initial refcount */ 1
                        | D3DCOMMON_TYPE_FIXUP
                        | D3DCOMMON_D3DCREATED;

    pFixup->Data = (DWORD) (pFixup + 1);
    pFixup->Size = Size;

    *ppFixup = pFixup;

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DFixup_Reset

extern "C"
VOID WINAPI D3DFixup_Reset(
    D3DFixup* pFixup)
{
    COUNT_API(API_D3DFIXUP_RESET);

    CHECK(pFixup, "D3DFixup_Reset");

    if (DBG_CHECK(pFixup->Common & D3DFIXUP_COMMON_IN_BEGIN_END))
    {
        DPF_ERR("Can't Reset while in a Begin/End bracket");
    }

    BlockOnNonSurfaceResource(pFixup);

    pFixup->Next = 0;
    pFixup->Run = 0;
}

//------------------------------------------------------------------------------
// D3DPushBuffer_BeginFixup

extern "C"
VOID WINAPI D3DPushBuffer_BeginFixup(
    D3DPushBuffer* pPushBuffer,
    D3DFixup* pFixup,
    BOOL NoWait)
{
    COUNT_API(API_D3DPUSHBUFFER_BEGINFIXUP);

    CHECK(pPushBuffer, "D3DPushBuffer_BeginFixup");

    if (DBG_CHECK(TRUE))
    {
        if (NoWait > TRUE)
        {
            DPF_ERR("Invalid value for NoWait");
        }
        if (g_pFixup != NULL)
        {
            DPF_ERR("Can't have more than one BeginFixup/EndFixup active at "
                    "once");
        }
        if (pFixup != NULL)
        {
            CHECK(pFixup, "D3DPushBuffer_BeginFixup");

            if (pFixup->Common & D3DFIXUP_COMMON_IN_BEGIN_END)
            {
                DPF_ERR("Fixup object already in a BeginFixup/EndFixup bracket");
            }
        
            pFixup->Common |= D3DFIXUP_COMMON_IN_BEGIN_END;
        }

    #if DBG
        if (g_FixupBracket)
        {
            DPF_ERR("BeginFixup called twice with no intervening EndFixup");
        }
        g_FixupBracket = TRUE;
    #endif
    
    }

    // Stash a copy of the fix-up object for use by the push-buffer modify
    // routines.  If we were nice we would store this in the push-buffer
    // object and wouldn't have it as a global (which prevents multiple 
    // threads from using the modify methods at once),  But the BlockOnResource
    // below requires single-threaded semantics (since it can do a kick-off),
    // and our push-buffer modify routine debug code also has a bunch of 
    // globals.  But I don't think anyone will really mind if they can call
    // this from only one thread at a time.

    g_pFixup = pFixup;

    // Ready the fix-up object so that the next RunPushBuffer call knows
    // where to start reading:

    if (pFixup != NULL)
    {
        pFixup->Run = pFixup->Next;
    }

    // When directly modifying the contents of a WC push-buffer, make sure 
    // it's not in use:

    if ((pFixup == NULL) &&
        !(pPushBuffer->Common & D3DPUSHBUFFER_RUN_USING_CPU_COPY) && 
        !(NoWait))
    {
        BlockOnNonSurfaceResource(pPushBuffer);
    }
}

//------------------------------------------------------------------------------
// D3DPushBuffer_EndFixup

extern "C"
HRESULT WINAPI D3DPushBuffer_EndFixup(
    D3DPushBuffer* pPushBuffer)
{
    COUNT_API(API_D3DPUSHBUFFER_ENDFIXUP);

    CHECK(pPushBuffer, "D3DPushBuffer_EndFixup");

    if (DBG_CHECK(TRUE))
    {
    #if DBG
        if (!g_FixupBracket)
        {
            DPF_ERR("EndFixup called without first calling BeginFixup");
        }
        g_FixupBracket = FALSE;
    #endif
    }

    D3DFixup* pFixup = g_pFixup;

    // We only have to do anything if we were recording into a fix-up
    // object:

    if (pFixup)
    {
        g_pFixup = NULL;
    
        if (DBG_CHECK(TRUE))
        {
            if (!(pFixup->Common & D3DFIXUP_COMMON_IN_BEGIN_END))
            {
                DPF_ERR("End without a matching Begin");
            }
        
            pFixup->Common &= ~D3DFIXUP_COMMON_IN_BEGIN_END;
        }
    
        DWORD* pData = (DWORD*) (pFixup->Data + pFixup->Next);
    
        pFixup->Next += 4;
        if (pFixup->Next > pFixup->Size)
            return D3DERR_BUFFERTOOSMALL;
    
        *pData = 0xffffffff;
    }

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DFixup_GetSize

extern "C"
VOID WINAPI D3DFixup_GetSize(
    D3DFixup* pFixup,
    DWORD *pSize)
{
    COUNT_API(API_D3DFIXUP_GETSIZE);

    CHECK(pFixup, "D3DFixup_GetSize");

    *pSize = pFixup->Next - pFixup->Run;
}

//------------------------------------------------------------------------------
// D3DFixup_GetSpace

extern "C"
VOID WINAPI D3DFixup_GetSpace(
    D3DFixup* pFixup,
    DWORD *pSpace)
{
    COUNT_API(API_D3DFIXUP_GETSPACE);

    CHECK(pFixup, "D3DFixup_GetSpace");

    INT space = (pFixup->Size - pFixup->Next);

    *pSpace = (space > 0) ? space : 0;
}

} // end of namespace
