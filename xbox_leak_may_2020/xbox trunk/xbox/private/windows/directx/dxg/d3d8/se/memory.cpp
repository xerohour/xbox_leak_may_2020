/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       memory.cpp
 *  Content:    Video resource allocation routines.
 *
 ***************************************************************************/

#include "precomp.hpp"

#if DBG || PROFILE

// Counts of active allocations.
extern "C"
{
    DWORD D3D__AllocsContiguous;
    DWORD D3D__AllocsNoncontiguous;
}

#endif

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

// The following functions are not defined if building for startup animation 
// because the startup animation app provides its own implementation

#if (DBG || PROFILE) && !STARTUPANIMATION

//----------------------------------------------------------------------------
// Allocate non-contiguous memory, and automatically zero-initialize it.
//
void *MemAlloc(
    DWORD Size
    )
{
    void *pv = LocalAlloc(LMEM_ZEROINIT, Size);
    if (pv)
    {
        D3D__AllocsNoncontiguous += LocalSize(pv);
    }

    return pv;
}

//----------------------------------------------------------------------------
// Allocate non-contiguous memory, and don't zero-initialize it.
//
void *MemAllocNoZero(
    DWORD Size
    )
{
    void *pv = LocalAlloc(0, Size);
    if (pv)
    {
        D3D__AllocsNoncontiguous += LocalSize(pv);
    }

    return pv;
}

//----------------------------------------------------------------------------
// Free a non-contiguous memory allocation.
//
void MemFree(
    void *pv
    )
{
    if (pv)
    {
        DWORD size = LocalSize(pv);

        if (size == (DWORD)-1)
        {
            DXGRIP("MemFree - Invalid pointer %x", pv);
        }
        else
        {
            ASSERT(D3D__AllocsNoncontiguous >= size);

            D3D__AllocsNoncontiguous -= size;
        }
    }

    LocalFree(pv);
}

#endif 

//----------------------------------------------------------------------------
// Allocate a block of noncontiguous memory and return a pointer to it.
//
// NOTE: This is used only by XDash!  It uses Register() but wants D3D to
//       free the resource memory for it when all the reference counts go to 
//       zero.
//
extern "C"
void* WINAPI D3D_AllocNoncontiguousMemory(
    DWORD Size          // The size of the allocation in bytes
    )
{
    return MemAlloc(Size);
}

//----------------------------------------------------------------------------
// Frees memory allocated by the above method.
//
// NOTE: This is used only by XDash!  It uses Register() but wants D3D to
//       free the resource memory for it when all the reference counts go to 
//       zero.
//
extern "C"
void WINAPI D3D_FreeNoncontiguousMemory(
     void *pMemory       // The block of memory to free
    )
{
    MemFree(pMemory);
}

#if !STARTUPANIMATION

//----------------------------------------------------------------------------
// Allocate a block of contiguous memory and return a pointer to it.
//
extern "C"
void* WINAPI D3D_AllocContiguousMemory(
    DWORD Size,         // The size of the allocation in bytes
    DWORD Alignment     // The alignment of the allocation
    )
{
    return AllocateContiguousMemory(Size, Alignment);
}

//----------------------------------------------------------------------------
// Frees memory allocated by the above method.
//
extern "C"
void WINAPI D3D_FreeContiguousMemory(
     void *pMemory       // The block of memory to free
    )
{
    return FreeContiguousMemory(pMemory);
}

#endif STARTUPANIMATION

} // end of namespace

