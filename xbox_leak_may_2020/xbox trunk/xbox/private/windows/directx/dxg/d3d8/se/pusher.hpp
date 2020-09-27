/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       pusher.hpp
 *  Content:    Pusher access constants and macros
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

//------------------------------------------------------------------------------
// Flags for our internal SetFence routine:

#define SETFENCE_SEGMENT 0x1

#define SETFENCE_NOKICKOFF 0x2    

//------------------------------------------------------------------------------
// Debug state

#define PUSH_STATE_NONE 0x0
#define PUSH_STATE_START 0x1
#define PUSH_STATE_BLOCKED 0x3
 
//------------------------------------------------------------------------------
// Pusher defines

// Default size of the push-buffer, in bytes:

#define DEFAULT_PUSH_BUFFER_SIZE (512 * 1024)

// Default segment size:

#define DEFAULT_PUSH_BUFFER_SEGMENT_SIZE (DEFAULT_PUSH_BUFFER_SIZE / 16)

// This is the number of most-recently-created fences that we keep track
// of (which includes both user-inserted fences, and fences used to mark
// segments).  Must be a power of two:

    // #define PUSHER_FENCE_COUNT 4 // Test purposes only

#define PUSHER_FENCE_COUNT 64

// This shift is how many bits are left for the 'time' component.  A 
// value of 1 means that there's 31 bits of time, implying 2 billion 
// unique time values can be assigned before a wrap occurs (or about 
// 134 million wraps of the push-buffer if app doesn't use InsertFence 
// much).
//
// The nice thing about our system is that even if an object lives long
// enough to have its time value re-issued because of a wrap, that's 
// not even fatal: in the prohibitively rare case, the worst it will do
// is cause an incorrect wait (which is guaranteed not to be endless).

    // #define PUSHER_TIME_SHIFT 16 // Test purposes only

#define PUSHER_TIME_SHIFT 1

// The low bit of any valid 'Time' is always set.  (If it's not set, then
// that means it belongs to an object that has never yet been used.)

#define PUSHER_TIME_VALID_FLAG 1

// How far away, in bytes, must the Get be from a fence before we'll 
// block the thread instead of spin?  The absolute minimum must
// be 1024 to avoid writing a notifier into a spot in the push
// buffer after it has already been cached on the GPU.

#define PUSHER_BLOCK_THRESHOLD (32*1024)

// Dwords of data always available in the push buffer:
 
#define PUSHER_THRESHOLD_SIZE 128     

// Dwords of space we actually reserve at the end of the push buffer to
// leave room for our extra 'goo'.  The '+ 1' accounts for a JUMP instruction
// we may have to insert in order to wrap:

#define PUSHER_THRESHOLD_SIZE_PLUS_OVERHEAD (PUSHER_THRESHOLD_SIZE + 1)
            
// After running a large push-buffer, we always insert a fence for better
// object time granularity:

#define PUSHER_RUN_FENCE_THRESHOLD 8*1024

// The parser FIFO reads ahead even before our software method
// gets executed.  With the current setup, it can read 1024 bytes
// ahead, and so we have to be sure we don't try to fix-up 
// anything within that range.  We pad with NV097_NO_OPERATIONs if 
// the offset of our first fix-up isn't far enough.  (Note that we
// can't use PUSHER_NOPs in this case.)
//
// 32 bytes are added to account for the DMA fetch buffer, which is in
// addition to the FIFO slots.
//
// See NV_PFIFO_CACHE1_DMA_FETCH for more details.

#define PUSHER_MINIMUM_RUN_OFFSET (1024 + 32)

// Hardware push buffer encoding commands.  These use '+' instead of '|'
// because that's more conducive to address macros that have adds (the
// constant adds can be collapsed, whereas an 'or' and an 'add' cannot):

#define PUSHER_JUMP(x) \
            (0x00000001 + (x))  
            
#define PUSHER_CALL(x) \
            (0x00000002 + (x))  

#define PUSHER_NOINC(method) \
            (0x40000000 + (method))

#define PUSHER_METHOD(subch, method, count) \
            (((count) << 18) + ((subch) << 13) + (method))
            
// NOTE: Never use PUSHER_NOP!  It breaks our assumptions with 
//       PUSHER_MINIMUM_RUN_OFFSET because it doesn't take any FIFO
//       read-ahead slots:
            
#define PUSHER_NOP 0            

//------------------------------------------------------------------------------
// FenceEncoding
//
// Describes the actual push-buffer encoding of a fence.  A fence can be
// in one of two 'modes'...either just hanging out in the push buffer
// waiting to be executed or in 'block' mode when we want to actually
// block on the fence.

struct FenceEncoding
{
    // Semaphore command: NV097_BACK_END_WRITE_SEMAPHORE_RELEASE or
    // NV097_TEXTURE_READ_SEMAPHORE_RELEASE:

    DWORD m_SemaphoreCommand;      

    // Time of this fence:

    DWORD m_Time;         

    union
    {
        // Normal case:

        struct
        {
            // Kelvin NOP command - actually NV097_SET_COLOR_CLEAR_VALUE:

            DWORD m_SetColorClearCommand1;    

            // Set to zero by the CPU and ignored by the GPU:

            DWORD m_SetColorClearArgument1;  

            // Kelvin NOP command - actually NV097_SET_COLOR_CLEAR_VALUE:

            DWORD m_SetColorClearCommand2;    

            // Set to zero by the CPU and ignored by the GPU:

            DWORD m_SetColorClearArgument2;
        };

        // Blocking case:

        struct
        {
            // Wait-for-idle command - NV097_WAIT_FOR_IDLE:

            DWORD m_WaitForIdleCommand;

            // Zero:

            DWORD m_WaitForIdleArgument;

            // Notify command - NV097_NO_OPERATION:

            DWORD m_NoOperationCommand; 

            // NVX_FENCE:

            DWORD m_FenceCommand;
        };
    };
};

//------------------------------------------------------------------------------
// Fence

struct Fence
{
    // Time associated with this fence:

    DWORD Time;

    // Points directly into the push-buffer to the fence encoding.
    //
    // NOTE: This points into write-combined memory, so don't read from it!

    FenceEncoding* pEncoding;

    // Total number of bytes of RunPushBuffer calls ever inserted, at the time
    // that the fence was created:

    DWORD RunTotal;
};

//------------------------------------------------------------------------------
// FlushWCCache
//
// Forces the write-combined cache to get flushed.  sfence should be all we
// need but it doesn't always work out that way.

#ifdef ALIASPUSHER
    
__forceinline VOID FlushWCCache() { _asm { sfence }; }

#else

VOID FlushWCCache();

#endif


//------------------------------------------------------------------------------
// Pusher prototypes:

#if DBG

void DbgRecordPushStart(DWORD Size);

#else

static __forceinline void DbgRecordPushStart(DWORD Size) { }

#endif


class CDevice;

void KickOffAndWaitForIdle();
VOID BlockOnTime(DWORD Time, BOOL MakeSpace);
DWORD SetFence(DWORD Flags);

// Resource prototypes:

BOOLEAN IsResourceBusy(IDirect3DResource8 *pResource);
VOID BlockOnResource(IDirect3DResource8 *pResource);
VOID BlockOnNonSurfaceResource(IDirect3DResource8 *pResource);

//------------------------------------------------------------------------------
// EncodeMethod 

FORCEINLINE DWORD EncodeMethod(
    SubChannel subch,
    DWORD method,
    DWORD count = 1)
{
    // Hardware encoding limitation:

    ASSERT(count < 2048);       

    return PUSHER_METHOD(subch, method, count);
}

//------------------------------------------------------------------------------
// EncodeMethod 

FORCEINLINE DWORD EncodeMethod(
    DWORD method,
    DWORD count = 1)
{
    return EncodeMethod(SUBCH_3D, method, count);
}

//------------------------------------------------------------------------------
// DumpMatrixTransposed

VOID DumpMatrixTransposed(
    PPUSH pPush,
    DWORD encodedMethod,
    CONST D3DMATRIX* pMatrix);

#if !DBG

//------------------------------------------------------------------------------
// Stuff that doesn't do anything on retail builds:

FORCEINLINE VOID PARSE_PUSH_BUFFER() {}
FORCEINLINE VOID PRIMITIVE_LAUNCH_STATE_TEST() {}
FORCEINLINE VOID PRIMITIVE_LAUNCH_STATE_TEST2() {}

//------------------------------------------------------------------------------
// PushedRaw - Call this to advance the push pointer after having copied
//             raw data into the pusher buffer

FORCEINLINE VOID PushedRaw(
    PPUSH pPushNext)
{
}

//------------------------------------------------------------------------------
// Push1 - Push one dword

FORCEINLINE VOID Push1(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    DWORD value)
{
    pPush[0] = PUSHER_METHOD(subch, method, 1);
    pPush[1] = value;
}

//------------------------------------------------------------------------------
// Push2 - Push two dwords

FORCEINLINE VOID Push2(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    DWORD a,
    DWORD b)
{
    pPush[0] = PUSHER_METHOD(subch, method, 2);
    pPush[1] = a;
    pPush[2] = b;
}

//------------------------------------------------------------------------------
// Push3 - Push three dwords

FORCEINLINE VOID Push3(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    DWORD a,
    DWORD b,
    DWORD c)
{
    pPush[0] = PUSHER_METHOD(subch, method, 3);
    pPush[1] = a;
    pPush[2] = b;
    pPush[3] = c;
}

//------------------------------------------------------------------------------
// Push4 - Push four dwords

FORCEINLINE VOID Push4(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    DWORD a,
    DWORD b,
    DWORD c,
    DWORD d)
{
    pPush[0] = PUSHER_METHOD(subch, method, 4);
    pPush[1] = a;
    pPush[2] = b;
    pPush[3] = c;
    pPush[4] = d;
}

//------------------------------------------------------------------------------
// Push1f - Push one float

FORCEINLINE VOID Push1f(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    FLOAT value)
{
    pPush[0] = PUSHER_METHOD(subch, method, 1);
    ((FLOAT*) pPush)[1] = value;
}

//------------------------------------------------------------------------------
// Push2f - Push two floats

FORCEINLINE VOID Push2f(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    FLOAT a,
    FLOAT b)
{
    pPush[0] = PUSHER_METHOD(subch, method, 2);
    ((FLOAT*) pPush)[1] = a;
    ((FLOAT*) pPush)[2] = b;
}

//------------------------------------------------------------------------------
// Push3f - Push three floats

FORCEINLINE VOID Push3f(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    FLOAT a,
    FLOAT b,
    FLOAT c)
{
    pPush[0] = PUSHER_METHOD(subch, method, 3);
    ((FLOAT*) pPush)[1] = a;
    ((FLOAT*) pPush)[2] = b;
    ((FLOAT*) pPush)[3] = c;
}

//------------------------------------------------------------------------------
// Push4f - Push four floats

FORCEINLINE VOID Push4f(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    FLOAT a,
    FLOAT b,
    FLOAT c,
    FLOAT d)
{
    pPush[0] = PUSHER_METHOD(subch, method, 4);
    ((FLOAT*) pPush)[1] = a;
    ((FLOAT*) pPush)[2] = b;
    ((FLOAT*) pPush)[3] = c;
    ((FLOAT*) pPush)[4] = d;
}

//------------------------------------------------------------------------------
// PushCount - Just push the method, with a specified count of data to follow

FORCEINLINE VOID PushCount(
    PPUSH pPush,
    SubChannel subch,
    DWORD method,
    DWORD count)
{
    pPush[0] = PUSHER_METHOD(subch, method, count);
}

//------------------------------------------------------------------------------
// PushMatrixTransposed

FORCEINLINE VOID PushMatrixTransposed(
    PPUSH pPush,
    DWORD method,
    CONST D3DMATRIX* pMatrix)
{
    DumpMatrixTransposed(pPush, PUSHER_METHOD(SUBCH_3D, method, 16), pMatrix);
}

//------------------------------------------------------------------------------
// PushInverseModelViewMatrix

FORCEINLINE VOID PushInverseModelViewMatrix(
    PPUSH pPush,
    DWORD method,
    CONST D3DMATRIX* pMatrix)
{
    *pPush = PUSHER_METHOD(SUBCH_3D, method, 12);
    memcpy((void*) (pPush + 1), pMatrix, 12 * sizeof(float));
}

//------------------------------------------------------------------------------
// PushMatrix
//
// WARNING: Use this method sparingly, as it causes code bloat

FORCEINLINE VOID PushMatrix(
    PPUSH pPush,
    DWORD method,
    FLOAT m11, FLOAT m12, FLOAT m13, FLOAT m14,
    FLOAT m21, FLOAT m22, FLOAT m23, FLOAT m24,
    FLOAT m31, FLOAT m32, FLOAT m33, FLOAT m34,
    FLOAT m41, FLOAT m42, FLOAT m43, FLOAT m44)
{
    *(pPush) = PUSHER_METHOD(SUBCH_3D, method, 16);
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
}

#else // This #else handles the debug case

//------------------------------------------------------------------------------
// For debug, we have out-of-line routines which do a bunch of work:

VOID PushedRaw(PPUSH pPushNext);
VOID Push1(PPUSH pPush, SubChannel subch, DWORD method, DWORD value);
VOID Push2(PPUSH pPush, SubChannel subch, DWORD method, DWORD a, DWORD b);
VOID Push3(PPUSH pPush, SubChannel subch, DWORD method, DWORD a, DWORD b, DWORD c);
VOID Push4(PPUSH pPush, SubChannel subch, DWORD method, DWORD a, DWORD b, DWORD c, DWORD d);
VOID Push1f(PPUSH pPush, SubChannel subch, DWORD method, FLOAT value);
VOID Push2f(PPUSH pPush, SubChannel subch, DWORD method, FLOAT a, FLOAT b);
VOID Push3f(PPUSH pPush, SubChannel subch, DWORD method, FLOAT a, FLOAT b, FLOAT c);
VOID Push4f(PPUSH pPush, SubChannel subch, DWORD method, FLOAT a, FLOAT b, FLOAT c, FLOAT d);
VOID PushCount(PPUSH pPush, SubChannel subch, DWORD method, DWORD count);
VOID PushMatrixTransposed(PPUSH pPush, DWORD method, CONST D3DMATRIX* pMatrix);
VOID PushInverseModelViewMatrix(PPUSH pPush, DWORD method, CONST D3DMATRIX* pMatrix);
VOID PushMatrix(PPUSH pPush, DWORD method, 
                FLOAT m11, FLOAT m12, FLOAT m13, FLOAT m14,
                FLOAT m21, FLOAT m22, FLOAT m23, FLOAT m24, 
                FLOAT m31, FLOAT m32, FLOAT m33, FLOAT m34,
                FLOAT m41, FLOAT m42, FLOAT m43, FLOAT m44);

//------------------------------------------------------------------------------
// Debug routine prototypes and globals

VOID PARSE_PUSH_BUFFER();
VOID PRIMITIVE_LAUNCH_STATE_TEST();
VOID PRIMITIVE_LAUNCH_STATE_TEST2();
VOID ShowDump(DWORD subch);

extern PPUSH g_ParsePut;

#endif

//------------------------------------------------------------------------------
// 3D-default push methods:

FORCEINLINE VOID Push1(
    PPUSH pPush,
    DWORD method,
    DWORD value)
{
    XMETAL_Push1(pPush, method, value);
}

FORCEINLINE VOID Push1f(
    PPUSH pPush,
    DWORD method,
    FLOAT value)
{
    XMETAL_Push1f(pPush, method, value);
}

FORCEINLINE VOID Push2(
    PPUSH pPush,
    DWORD method,
    DWORD a,
    DWORD b)
{
    XMETAL_Push2(pPush, method, a, b);
}

FORCEINLINE VOID Push3(
    PPUSH pPush,
    DWORD method,
    DWORD a,
    DWORD b,
    DWORD c)
{
    XMETAL_Push3(pPush, method, a, b, c);
}

FORCEINLINE VOID Push4(
    PPUSH pPush,
    DWORD method,
    DWORD a,
    DWORD b,
    DWORD c,
    DWORD d)
{
    XMETAL_Push4(pPush, method, a, b, c, d);
}

FORCEINLINE VOID Push2f(
    PPUSH pPush,
    DWORD method,
    FLOAT a,
    FLOAT b)
{
    XMETAL_Push2f(pPush, method, a, b);
}

FORCEINLINE VOID Push3f(
    PPUSH pPush,
    DWORD method,
    FLOAT a,
    FLOAT b,
    FLOAT c)
{
    XMETAL_Push3f(pPush, method, a, b, c);
}

FORCEINLINE VOID Push4f(
    PPUSH pPush,
    DWORD method,
    FLOAT a,
    FLOAT b,
    FLOAT c,
    FLOAT d)
{
    XMETAL_Push4f(pPush, method, a, b, c, d);
}

FORCEINLINE VOID PushCount(
    PPUSH pPush,
    DWORD method,
    DWORD count)
{
    XMETAL_PushCount(pPush, method, count);
}

} // end namespace
