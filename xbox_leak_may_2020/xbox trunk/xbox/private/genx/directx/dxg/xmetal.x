/*==========================================================================;
 *
 *  Copyright (C) 2000 - 2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xmetal.h
 *  Content:    Xbox low-level graphics helpers
 *
 ****************************************************************************/

#ifndef _XMETAL_H_
#define _XMETAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 *
 * Debug helpers
 *
 ****************************************************************************/

#if DBG

    ULONG _cdecl DbgPrint(PCH Format, ...);

    #define ERROR(msg)            \
        {                       \
            DbgPrint msg;       \
            DbgPrint("\n");     \
            _asm { int 3 };     \
        }

#else !DBG

    #define ERROR(x) {}

#endif !DBG

/****************************************************************************
 *
 * Memory
 *
 ****************************************************************************/

#ifdef FASTPUSHER

#define XMETAL_MEMORY_TYPE_VIDEO           0x80000000
#define XMETAL_MEMORY_TYPE_CONTIGUOUS      0x80000000

#else

#define XMETAL_MEMORY_TYPE_VIDEO           0xF0000000
#define XMETAL_MEMORY_TYPE_CONTIGUOUS      0xF0000000

#endif

#define XMETAL_MAX_PHYSICAL_OFFSET         0x08000000

#if DBG

// Verify that the memory address is a virtual address to a write
// combined mapping of contiguous memory.
//
static __forceinline void XMETAL_DebugVerifyContiguous(
    void *pv
    )
{
    DWORD Type = (DWORD)pv & 0xF0000000;

    if (Type != XMETAL_MEMORY_TYPE_CONTIGUOUS && 
        Type != XMETAL_MEMORY_TYPE_VIDEO)
    {
        ERROR(("DebugVerifyContiguous - the address is not a write combined mapping to contiguous memory."));
    }

    if (((DWORD)pv & 0x0FFFFFFF) > XMETAL_MAX_PHYSICAL_OFFSET)
    {
        ERROR(("DebugVerifyContiguous - the address is not a write combined mapping to contiguous memory."));
    }
}

// Verify that the offset is a valid physical memory offset and can be
// accessed by the graphics processor.
//
static __forceinline void XMETAL_DebugVerifyPhysical(
    DWORD Offset
    )
{
    if (Offset > XMETAL_MAX_PHYSICAL_OFFSET)
    {
        ERROR(("DebugVerifyPhysical - Not a valid physical memory offset."));
    }
}

#else !DBG

static __forceinline void XMETAL_DebugVerifyContiguous(void *pv) { }
static __forceinline void XMETAL_DebugVerifyPhysical(DWORD offset) { }

#endif !DBG

// Map from a write-combined pointer to a physical memory offset.
static __forceinline DWORD XMETAL_MapToPhysicalOffset(
    void *pv
    )
{
    XMETAL_DebugVerifyContiguous(pv);

    return ((DWORD)pv) & (XMETAL_MAX_PHYSICAL_OFFSET - 1);
}

// Map from a physical memory offset to a contiguous memory
// address.
//
static __forceinline void *XMETAL_MapToContiguousAddress(
    DWORD Offset
    )
{
    XMETAL_DebugVerifyPhysical(Offset);

    return (void *)(XMETAL_MEMORY_TYPE_CONTIGUOUS | Offset);
}

// Map from a physical memory offset to a video memory address.  This method
// is temporary and will change to return the exact same value at the
// contiguous address on the final hardware.
//
static __forceinline void *XMETAL_MapToVideoAddress(
    DWORD Offset
    )
{
    XMETAL_DebugVerifyPhysical(Offset);

    return (void *)(XMETAL_MEMORY_TYPE_VIDEO | Offset);
}

/****************************************************************************
 *
 * Push Buffer
 *
 ****************************************************************************/

//
// DOC: Push buffer usage.
//
// All graphics processor commands are queued up in the push buffer.  This 
// buffer is created by Direct3D_CreateDevice at is set to the size specified
// by Direct3D_SetPushBufferSize.  The push buffer is a circular queue.
//
// All commands are pushed as a 4-byte method descriptor which contains the
// command value and a count, followed by that number of 4 byte values.
// Consecutively-numbered commands can be combined simply by providing
// extra values on the end of the first command.
//
// Space in the push buffer can be reserved by calling XMETAL_StartPush,
// which returns a pointer where the commands should be stored.  The space
// is actually committed by calling XMETAL_EndPush.  Not all of the
// reserved space needs to be used and, in fact, will not be used most of
// the time.
//
// The XMETAL_Push* methods write information into the push buffer.  They
// must be used as all of the push buffer methods perform debug checks
// (in the debug build only) to ensure that the reserved space is not
// overrun, that there are no holes, etc.
//
// Multiple commands can be written into the push buffer at a time but the
// push buffer pointer must be manually incremented for each command.
//
// Example:
//
// void PushStuff()
// {
//     XMETAL_PushBuffer *pPusher = g_pPushBuffer;
//
//     PPUSH pPush = XMETAL_StartPush(pPusher);  /* reserves 128 DWORDS */
//
//     XMETAL_Push1(pPush,     CMD_1, 0);         /* method, value */
//     XMETAL_Push2(pPush + 2, CMD_3, 1, 2);      /* method, value, value */
//
//     XMETAL_PushCount(pPush + 5, CMD_10, 3);    /* method, 3 values */
//
//     *(pPush + 6) = 0;    // Value for CMD_10
//     *(pPush + 7) = 1;    // Value for CMD_11
//     *(pPush + 8) = 2;    // Value for CMD_12
//
//     XMETAL_EndPush(pPusher, pPush + 9);        /* commits 9 dwords */
// }
//

//
/*
 * Push buffer allocator
 */

// All writes to the push-buffer should be using a 'volatile' type, 
// otherwise the compiler may be allowed to do the writes out-of-order,
// which is bad for write-combining performance.
//
typedef volatile DWORD* PPUSH;

// Describes the available space in the push buffer.  This structure should
// never be manipulated directly; space should be allocated through the
// StartPush/EndPush methods.
//
typedef struct _XMETAL_PushBuffer 
{
    PPUSH m_pPut;
    PPUSH m_pThreshold;
} XMETAL_PushBuffer;

// The global push buffer pointer.  This pointer is set when 
// Direct3D_CreateDevice is called and will not change unless the device is 
// destroyed.
//
extern XMETAL_PushBuffer *g_pPushBuffer;

// Reserve a specific number of DWORDs in the push buffer and return a pointer
// to the beginning of the reserved block.  This method is not as efficient
// as XMETAL_StartPush and should only be called if more than 128 DWORDs are
// required.
//
// This method cannot fail but may block until there is enough space
// in the push buffer to process the reservation.
//
DWORD * WINAPI XMETAL_StartPushCount(XMETAL_PushBuffer *pPushBuffer, DWORD count);

// Commit the space actually used in the push buffer.  Pass in the pointer
// in the push buffer immediately after the last pushed command.  It is not
// necessary to use all of the space reserved in the push buffer.
//
#if DBG

void WINAPI XMETAL_EndPush(XMETAL_PushBuffer *pPushBuffer, PPUSH pPush);

#else !DBG

static __forceinline void XMETAL_EndPush(XMETAL_PushBuffer *pPushBuffer, PPUSH pPush)
{
    pPushBuffer->m_pPut = pPush;
}

#endif !DBG

/*
 * Method pushing helpers.  This uses '+' instead of '|' because it's more
 * friendly to macros that include add operations.
 */

#define XMETAL_PUSHER_METHOD(method, count) \
            (((count) << 18) + (method))
         
#if !DBG

//----------------------------------------------------------------------------
// PushedRaw - Call this to advance the push pointer after having copied
//             raw data into the pusher buffer
//
static __forceinline VOID XMETAL_PushedRaw(
    PPUSH pPushNext)
{
}

//----------------------------------------------------------------------------
// Push1 - Push one dword
//
static __forceinline VOID XMETAL_Push1(
    PPUSH pPush,
    DWORD method,
    DWORD value)
{
    pPush[0] = XMETAL_PUSHER_METHOD(method, 1);
    pPush[1] = value;
}

//----------------------------------------------------------------------------
// Push2 - Push two dwords
//
static __forceinline VOID XMETAL_Push2(
    PPUSH pPush,
    DWORD method,
    DWORD a,
    DWORD b)
{
    pPush[0] = XMETAL_PUSHER_METHOD(method, 2);
    pPush[1] = a;
    pPush[2] = b;
}

//----------------------------------------------------------------------------
// Push3 - Push three dwords
//
static __forceinline VOID XMETAL_Push3(
    PPUSH pPush,
    DWORD method,
    DWORD a,
    DWORD b,
    DWORD c)
{
    pPush[0] = XMETAL_PUSHER_METHOD(method, 3);
    pPush[1] = a;
    pPush[2] = b;
    pPush[3] = c;
}

//----------------------------------------------------------------------------
// Push4 - Push four dwords
//
static __forceinline VOID XMETAL_Push4(
    PPUSH pPush,
    DWORD method,
    DWORD a,
    DWORD b,
    DWORD c,
    DWORD d)
{
    pPush[0] = XMETAL_PUSHER_METHOD(method, 4);
    pPush[1] = a;
    pPush[2] = b;
    pPush[3] = c;
    pPush[4] = d;
}

//----------------------------------------------------------------------------
// Push1f - Push one float
//
static __forceinline VOID XMETAL_Push1f(
    PPUSH pPush,
    DWORD method,
    FLOAT value)
{
    pPush[0] = XMETAL_PUSHER_METHOD(method, 1);
    ((FLOAT*) pPush)[1] = value;
}

//----------------------------------------------------------------------------
// Push2f - Push two floats
//
static __forceinline VOID XMETAL_Push2f(
    PPUSH pPush,
    DWORD method,
    FLOAT a,
    FLOAT b)
{
    pPush[0] = XMETAL_PUSHER_METHOD(method, 2);
    ((FLOAT*) pPush)[1] = a;
    ((FLOAT*) pPush)[2] = b;
}

//----------------------------------------------------------------------------
// Push3f - Push three floats
//
static __forceinline VOID XMETAL_Push3f(
    PPUSH pPush,
    DWORD method,
    FLOAT a,
    FLOAT b,
    FLOAT c)
{
    pPush[0] = XMETAL_PUSHER_METHOD(method, 3);
    ((FLOAT*) pPush)[1] = a;
    ((FLOAT*) pPush)[2] = b;
    ((FLOAT*) pPush)[3] = c;
}

//----------------------------------------------------------------------------
// Push4f - Push four floats
//
static __forceinline VOID XMETAL_Push4f(
    PPUSH pPush,
    DWORD method,
    FLOAT a,
    FLOAT b,
    FLOAT c,
    FLOAT d)
{
    pPush[0] = XMETAL_PUSHER_METHOD(method, 4);
    ((FLOAT*) pPush)[1] = a;
    ((FLOAT*) pPush)[2] = b;
    ((FLOAT*) pPush)[3] = c;
    ((FLOAT*) pPush)[4] = d;
}

//----------------------------------------------------------------------------
// PushCount - Just push the method, with a specified count of data to follow
//
static __forceinline VOID XMETAL_PushCount(
    PPUSH pPush,
    DWORD method,
    DWORD count)
{
    pPush[0] = XMETAL_PUSHER_METHOD(method, count);
}

#else DBG

//----------------------------------------------------------------------------
// For debug, we have out-of-line routines which do a bunch of extra checks:
//
VOID WINAPI XMETAL_PushedRaw(PPUSH pPushNext);
VOID WINAPI XMETAL_Push1(PPUSH pPush, DWORD method, DWORD value);
VOID WINAPI XMETAL_Push2(PPUSH pPush, DWORD method, DWORD a, DWORD b);
VOID WINAPI XMETAL_Push3(PPUSH pPush, DWORD method, DWORD a, DWORD b, DWORD c);
VOID WINAPI XMETAL_Push4(PPUSH pPush, DWORD method, DWORD a, DWORD b, DWORD c, DWORD d);
VOID WINAPI XMETAL_Push1f(PPUSH pPush, DWORD method, FLOAT value);
VOID WINAPI XMETAL_Push2f(PPUSH pPush, DWORD method, FLOAT a, FLOAT b);
VOID WINAPI XMETAL_Push3f(PPUSH pPush, DWORD method, FLOAT a, FLOAT b, FLOAT c);
VOID WINAPI XMETAL_Push4f(PPUSH pPush, DWORD method, FLOAT a, FLOAT b, FLOAT c, FLOAT d);
VOID WINAPI XMETAL_PushCount(PPUSH pPush, DWORD method, DWORD count);

#endif DBG

#ifdef __cplusplus
}
#endif __cplusplus

#endif _XMETAL_H_
