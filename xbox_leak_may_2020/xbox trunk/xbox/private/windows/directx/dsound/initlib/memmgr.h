/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       memmgr.h
 *  Content:    DirectSound memory manager.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  12/21/00    dereks  Created.
 *
 ****************************************************************************/

#ifndef __MEMMGR_H__
#define __MEMMGR_H__

#define ENABLE_SLOP_MEMORY_RECOVERY

#if defined(DEBUG) && !defined(TRACK_MEMORY_USAGE) && !defined(MCPX_BOOT_LIB)
#define TRACK_MEMORY_USAGE
#endif // defined(DEBUG) && !defined(TRACK_MEMORY_USAGE) && !defined(MCPX_BOOT_LIB)

BEGIN_DEFINE_ENUM()
    DSOUND_OBJECT_POOL_TAG  = 'boSD',
    DSOUND_DATA_POOL_TAG    = 'adSD'
END_DEFINE_ENUM(DSOUND_POOL_TAG);

BEGIN_DEFINE_ENUM()
    DSOUND_ALLOCATOR_POOL   = 'loop',
    DSOUND_ALLOCATOR_PHYS   = 'syhp',
    DSOUND_ALLOCATOR_SLOP   = 'pols'
END_DEFINE_ENUM(DSOUND_ALLOCATOR_TAG);

//
// Memory management functions
//

#ifdef TRACK_MEMORY_USAGE

EXTERN_C LPVOID DirectSoundTestTrackingPoolAlloc(LPCSTR pszFile, ULONG nLine, LPCSTR pszClass, DSOUND_POOL_TAG nTag, ULONG cbBuffer, BOOL fZeroInit);
EXTERN_C void DirectSoundTestTrackingPoolFree(LPVOID pvBuffer);

EXTERN_C LPVOID DirectSoundTestTrackingPhysicalAlloc(LPCSTR pszFile, ULONG nLine, LPCSTR pszClass, ULONG cbBuffer, ULONG cbAlignment, DWORD dwFlags, BOOL fZeroInit);
EXTERN_C void DirectSoundTestTrackingPhysicalFree(LPVOID pvBuffer);

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

EXTERN_C LPVOID DirectSoundTestTrackingMemAlloc(LPCSTR pszFile, ULONG nLine, LPCSTR pszClass, DSOUND_POOL_TAG nTag, ULONG cbBuffer, BOOL fZeroInit);
EXTERN_C void DirectSoundTestTrackingMemFree(LPVOID pvBuffer);

#else // ENABLE_SLOP_MEMORY_RECOVERY

#define DirectSoundTestTrackingMemAlloc DirectSoundTestTrackingPoolAlloc
#define DirectSoundTestTrackingMemFree DirectSoundTestTrackingPoolFree

#endif // ENABLE_SLOP_MEMORY_RECOVERY

#else // TRACK_MEMORY_USAGE

EXTERN_C LPVOID DirectSoundTestPoolAlloc(DSOUND_POOL_TAG nTag, ULONG cbBuffer, BOOL fZeroInit);
EXTERN_C void DirectSoundTestPoolFree(LPVOID pvBuffer);

EXTERN_C LPVOID DirectSoundTestPhysicalAlloc(ULONG cbBuffer, ULONG cbAlignment, DWORD dwFlags, BOOL fZeroInit);
EXTERN_C void DirectSoundTestPhysicalFree(LPVOID pvBuffer);

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

EXTERN_C LPVOID DirectSoundTestMemAlloc(DSOUND_POOL_TAG nTag, ULONG cbBuffer, BOOL fZeroInit);
EXTERN_C void DirectSoundTestMemFree(LPVOID pvBuffer);

#else // ENABLE_SLOP_MEMORY_RECOVERY

#define DirectSoundTestMemAlloc DirectSoundTestPoolAlloc
#define DirectSoundTestMemFree DirectSoundTestPoolFree

#endif // ENABLE_SLOP_MEMORY_RECOVERY

#define DirectSoundTestTrackingPoolAlloc(pszFile, nLine, pszClass, nTag, cbBuffer, fZeroInit) \
    DirectSoundTestPoolAlloc(nTag, cbBuffer, fZeroInit)

#define DirectSoundTestTrackingPoolFree(pvBuffer) \
    DirectSoundTestPoolFree(pvBuffer)

#define DirectSoundTestTrackingPhysicalAlloc(pszFile, nLine, pszClass, cbBuffer, cbAlignment, dwFlags, fZeroInit) \
    DirectSoundTestPhysicalAlloc(cbBuffer, cbAlignment, dwFlags, fZeroInit)

#define DirectSoundTestTrackingPhysicalFree(pvBuffer) \
    DirectSoundTestPhysicalFree(pvBuffer)

#define DirectSoundTestTrackingMemAlloc(pszFile, nLine, pszClass, nTag, cbBuffer, fZeroInit) \
    DirectSoundTestMemAlloc(nTag, cbBuffer, fZeroInit)

#define DirectSoundTestTrackingMemFree(pvBuffer) \
    DirectSoundTestMemFree(pvBuffer)

#endif // TRACK_MEMORY_USAGE

//
// Memory management macros
//

#define MEMALLOC(type, count) \
    ((type *)DirectSoundTestTrackingMemAlloc(__FILE__, __LINE__, #type, DSOUND_DATA_POOL_TAG, sizeof(type) * (count), TRUE))

#define MEMALLOC_NOINIT(type, count) \
    ((type *)DirectSoundTestTrackingMemAlloc(__FILE__, __LINE__, #type, DSOUND_DATA_POOL_TAG, sizeof(type) * (count), FALSE))

#define MEMFREE(p) \
    { \
        if(p) \
        { \
            DirectSoundTestTrackingMemFree(p); \
            (p) = NULL; \
        } \
    }

#define POOLALLOC(type, count) \
    ((type *)DirectSoundTestTrackingPoolAlloc(__FILE__, __LINE__, #type, DSOUND_DATA_POOL_TAG, sizeof(type) * (count), TRUE))

#define POOLALLOC_NOINIT(type, count) \
    ((type *)DirectSoundTestTrackingPoolAlloc(__FILE__, __LINE__, #type, DSOUND_DATA_POOL_TAG, sizeof(type) * (count), FALSE))

#define POOLFREE(p) \
    { \
        if(p) \
        { \
            DirectSoundTestTrackingPoolFree(p); \
            (p) = NULL; \
        } \
    }

#define PHYSALLOC(type, count, alignment, flags) \
    ((type *)DirectSoundTestTrackingPhysicalAlloc(__FILE__, __LINE__, #type, sizeof(type) * (count), alignment, flags, TRUE))

#define PHYSALLOC_NOINIT(type, count, alignment, flags) \
    ((type *)DirectSoundTestTrackingPhysicalAlloc(__FILE__, __LINE__, #type, sizeof(type) * (count), alignment, flags, FALSE))

#define PHYSFREE(p) \
    { \
        if(p) \
        { \
            DirectSoundTestTrackingPhysicalFree(p); \
            (p) = NULL; \
        } \
    }

//
// New and delete overrides
//

#ifdef __cplusplus

#ifndef DSOUND_NO_OVERRIDE_NEW_DELETE

static void *__cdecl operator new(size_t cbBuffer) 
{ 
    return DirectSoundTestTrackingMemAlloc("(none)", 0, "(unknown)", DSOUND_OBJECT_POOL_TAG, cbBuffer, TRUE); 
}

static void *__cdecl operator new[](size_t cbBuffer) 
{ 
    return DirectSoundTestTrackingMemAlloc("(none)", 0, "(unknown)", DSOUND_OBJECT_POOL_TAG, cbBuffer, TRUE);
}

#ifdef TRACK_MEMORY_USAGE

static void *__cdecl operator new(size_t cbBuffer, LPCSTR pszFile, ULONG nLine, LPCSTR pszClass)
{
    return DirectSoundTestTrackingMemAlloc(pszFile, nLine, pszClass, DSOUND_OBJECT_POOL_TAG, cbBuffer, TRUE); 
}

static void *__cdecl operator new[](size_t cbBuffer, LPCSTR pszFile, ULONG nLine, LPCSTR pszClass)
{
    return DirectSoundTestTrackingMemAlloc(pszFile, nLine, pszClass, DSOUND_OBJECT_POOL_TAG, cbBuffer, TRUE); 
}

#endif // TRACK_MEMORY_USAGE

static void __cdecl operator delete(void *pvBuffer) 
{ 
    DirectSoundTestTrackingMemFree(pvBuffer); 
}

static void __cdecl operator delete[](void *pvBuffer) 
{ 
    DirectSoundTestTrackingMemFree(pvBuffer); 
}

#endif // DSOUND_NO_OVERRIDE_NEW_DELETE

#ifdef TRACK_MEMORY_USAGE

#define NEW(type) \
    new(__FILE__, __LINE__, #type) type

#define NEW_A(type, count) \
    new(__FILE__, __LINE__, #type) type [count]

#else // TRACK_MEMORY_USAGE

#define NEW(type) \
    new type

#define NEW_A(type, count) \
    new type [count]

#endif // TRACK_MEMORY_USAGE

#undef DELETE
#define DELETE(p) \
    { \
        if(p) \
        { \
            delete (p); \
            (p) = NULL; \
        } \
    }

#define DELETE_A(p) \
    { \
        if(p) \
        { \
            delete [] (p); \
            (p) = NULL; \
        } \
    }

#endif // __cplusplus

#endif // __MEMMGR_H__
