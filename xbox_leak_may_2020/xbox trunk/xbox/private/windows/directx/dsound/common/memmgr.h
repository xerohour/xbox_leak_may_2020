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

#include "macros.h"

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
// Allocation tracking data
//

BEGIN_DEFINE_STRUCT()
    LIST_ENTRY              leListEntry;
    LPCSTR                  pszFile;
    ULONG                   nLine;
    LPCSTR                  pszClass;
    DSOUND_ALLOCATOR_TAG    nAllocatorTag;
    ULONG                   cbSize;
    LPVOID                  pvBaseAddress;
END_DEFINE_STRUCT(DSMEMTRACK);

#ifdef __cplusplus

//
// Memory manager
//

namespace DirectSound
{
    class CMemoryManager
    {
    public:

#ifdef BUILDING_DSOUND

        static DWORD &      m_dwPoolMemoryUsage;            // Pool memory usage, in bytes
        static DWORD &      m_dwPhysicalMemoryUsage;        // Physical memory usage, in bytes

#else // BUILDING_DSOUND

        static DWORD        m_dwPoolMemoryUsage;            // Pool memory usage, in bytes
        static DWORD        m_dwPhysicalMemoryUsage;        // Physical memory usage, in bytes

#endif // BUILDING_DSOUND

#ifdef TRACK_MEMORY_USAGE

    protected:
        static LIST_ENTRY   m_lstMemoryTracking;            // Allocation list

#endif // TRACK_MEMORY_USAGE
    
    public:
        // Pool memory
        static LPVOID PoolAlloc(DSOUND_POOL_TAG nTag, ULONG cbBuffer, BOOL fZeroInit);
        static void PoolFree(LPVOID pvBuffer);

        // Physically contiguous memory
        static LPVOID PhysicalAlloc(ULONG cbBuffer, ULONG cbAlignment, DWORD dwFlags, BOOL fZeroInit);
        static void PhysicalFree(LPVOID pvBuffer);

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

        // Unused memory recovery
        static LPVOID MemAlloc(DSOUND_POOL_TAG nTag, ULONG cbBuffer, BOOL fZeroInit);
        static void MemFree(LPVOID pvBuffer);

#endif // ENABLE_SLOP_MEMORY_RECOVERY

#ifdef TRACK_MEMORY_USAGE

        // Usage tracking
        static LPVOID TrackingPoolAlloc(LPCSTR pszFile, ULONG nLine, LPCSTR pszClass, DSOUND_POOL_TAG nTag, ULONG cbBuffer, BOOL fZeroInit);
        static void TrackingPoolFree(LPVOID pvBuffer);

        static LPVOID TrackingPhysicalAlloc(LPCSTR pszFile, ULONG nLine, LPCSTR pszClass, ULONG cbBuffer, ULONG cbAlignment, DWORD dwFlags, BOOL fZeroInit);
        static void TrackingPhysicalFree(LPVOID pvBuffer);

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

        static LPVOID TrackingMemAlloc(LPCSTR pszFile, ULONG nLine, LPCSTR pszClass, DSOUND_POOL_TAG nTag, ULONG cbBuffer, BOOL fZeroInit);
        static void TrackingMemFree(LPVOID pvBuffer);

#endif // ENABLE_SLOP_MEMORY_RECOVERY

#endif // TRACK_MEMORY_USAGE

        static void DumpMemoryUsage(BOOL fAssertNone);

#ifdef TRACK_MEMORY_USAGE

    private:
        // Tracking information
        static LPVOID TrackAlloc(LPVOID pvBaseAddress, ULONG cbTracking, LPCSTR pszFile, ULONG nLine, LPCSTR pszClass, DSOUND_ALLOCATOR_TAG nAllocatorTag, ULONG cbSize);
        static LPVOID ForgetAlloc(LPVOID pvBaseAddress, DSOUND_ALLOCATOR_TAG nAllocatorTag);

#endif // TRACK_MEMORY_USAGE

    };
}

#ifndef ENABLE_SLOP_MEMORY_RECOVERY

#define MemAlloc PoolAlloc
#define MemFree PoolFree

#ifdef TRACK_MEMORY_USAGE

#define TrackingMemAlloc TrackingPoolAlloc
#define TrackingMemFree TrackingPoolFree

#endif // TRACK_MEMORY_USAGE

#endif // ENABLE_SLOP_MEMORY_RECOVERY

#ifndef TRACK_MEMORY_USAGE

#define TrackingPoolAlloc(pszFile, nLine, pszClass, nTag, cbBuffer, fZeroInit) \
    PoolAlloc(nTag, cbBuffer, fZeroInit)

#define TrackingPoolFree(pvBuffer) \
    PoolFree(pvBuffer)

#define TrackingPhysicalAlloc(pszFile, nLine, pszClass, cbBuffer, cbAlignment, dwFlags, fZeroInit) \
    PhysicalAlloc(cbBuffer, cbAlignment, dwFlags, fZeroInit)

#define TrackingPhysicalFree(pvBuffer) \
    PhysicalFree(pvBuffer)

#define TrackingMemAlloc(pszFile, nLine, pszClass, nTag, cbBuffer, fZeroInit) \
    MemAlloc(nTag, cbBuffer, fZeroInit)

#define TrackingMemFree(pvBuffer) \
    MemFree(pvBuffer)

#endif // TRACK_MEMORY_USAGE

//
// Memory management macros
//

#define MEMALLOC(type, count) \
    ((type *)DirectSound::CMemoryManager::TrackingMemAlloc(__FILE__, __LINE__, #type, DSOUND_DATA_POOL_TAG, sizeof(type) * (count), TRUE))

#define MEMALLOC_NOINIT(type, count) \
    ((type *)DirectSound::CMemoryManager::TrackingMemAlloc(__FILE__, __LINE__, #type, DSOUND_DATA_POOL_TAG, sizeof(type) * (count), FALSE))

#define MEMFREE(p) \
    { \
        if(p) \
        { \
            DirectSound::CMemoryManager::TrackingMemFree(p); \
            (p) = NULL; \
        } \
    }

#define POOLALLOC(type, count) \
    ((type *)DirectSound::CMemoryManager::TrackingPoolAlloc(__FILE__, __LINE__, #type, DSOUND_DATA_POOL_TAG, sizeof(type) * (count), TRUE))

#define POOLALLOC_NOINIT(type, count) \
    ((type *)DirectSound::CMemoryManager::TrackingPoolAlloc(__FILE__, __LINE__, #type, DSOUND_DATA_POOL_TAG, sizeof(type) * (count), FALSE))

#define POOLFREE(p) \
    { \
        if(p) \
        { \
            DirectSound::CMemoryManager::TrackingPoolFree(p); \
            (p) = NULL; \
        } \
    }

#define PHYSALLOC(type, count, alignment, flags) \
    ((type *)DirectSound::CMemoryManager::TrackingPhysicalAlloc(__FILE__, __LINE__, #type, sizeof(type) * (count), alignment, flags, TRUE))

#define PHYSALLOC_NOINIT(type, count, alignment, flags) \
    ((type *)DirectSound::CMemoryManager::TrackingPhysicalAlloc(__FILE__, __LINE__, #type, sizeof(type) * (count), alignment, flags, FALSE))

#define PHYSFREE(p) \
    { \
        if(p) \
        { \
            DirectSound::CMemoryManager::TrackingPhysicalFree(p); \
            (p) = NULL; \
        } \
    }

//
// New and delete overrides
//

#ifndef DSOUND_NO_OVERRIDE_NEW_DELETE

static void *__cdecl operator new(size_t cbBuffer) 
{ 
    return DirectSound::CMemoryManager::TrackingMemAlloc("(none)", 0, "(unknown)", DSOUND_OBJECT_POOL_TAG, cbBuffer, TRUE); 
}

static void *__cdecl operator new[](size_t cbBuffer) 
{ 
    return DirectSound::CMemoryManager::TrackingMemAlloc("(none)", 0, "(unknown)", DSOUND_OBJECT_POOL_TAG, cbBuffer, TRUE);
}

#ifdef TRACK_MEMORY_USAGE

static void *__cdecl operator new(size_t cbBuffer, LPCSTR pszFile, ULONG nLine, LPCSTR pszClass)
{
    return DirectSound::CMemoryManager::TrackingMemAlloc(pszFile, nLine, pszClass, DSOUND_OBJECT_POOL_TAG, cbBuffer, TRUE); 
}

static void *__cdecl operator new[](size_t cbBuffer, LPCSTR pszFile, ULONG nLine, LPCSTR pszClass)
{
    return DirectSound::CMemoryManager::TrackingMemAlloc(pszFile, nLine, pszClass, DSOUND_OBJECT_POOL_TAG, cbBuffer, TRUE); 
}

#endif // TRACK_MEMORY_USAGE

static void __cdecl operator delete(void *pvBuffer) 
{ 
    DirectSound::CMemoryManager::TrackingMemFree(pvBuffer); 
}

static void __cdecl operator delete[](void *pvBuffer) 
{ 
    DirectSound::CMemoryManager::TrackingMemFree(pvBuffer); 
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
