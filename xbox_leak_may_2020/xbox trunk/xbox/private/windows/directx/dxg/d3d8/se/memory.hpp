/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       memory.hpp
 *  Content:    Public interface to the routines that manage resource
 *              memory.
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#if DBG || PROFILE

extern "C" extern DWORD D3D__AllocsContiguous;
extern "C" extern DWORD D3D__AllocsNoncontiguous;

#endif

// Defines the upper limit of memory that we'll make available to D3D and
// the GPU
#define AGP_APERTURE_BYTES              (128*1024*1024)

// The high-nibble value used for the write-combined AGP mapping of the 
// frame buffer (a view which is coherent with tiling, unlike the normal
// faster mapping in the 0 through 64 MB physical address range):
#define AGP_FRAME_BUFFER_HIGH_NIBBLE    0xF0000000

// Asserts that a chunk of memory was allocated in contiguous memory.
#if DBG

BOOL IsContiguousMemory(void *pv);
BOOL IsWriteCombinedMemory(void *pv);

inline void AssertContiguous(void *pv)
{
    DWORD Type = (DWORD)pv & ~(MM_BYTES_IN_PHYSICAL_MAP - 1);

    if (Type != XMETAL_MEMORY_TYPE_CONTIGUOUS)
    {
        DXGRIP("AssertContiguous - the memory is not contiguous.");
    }
}

inline void AssertContiguousOrVideo(void *pv)
{
    XMETAL_DebugVerifyContiguous(pv);
}

inline void AssertWriteCombined(void *pv)
{
    if (!IsWriteCombinedMemory(pv))
    {
        DXGRIP("AssertWriteCombined - the memory is not in the write-combined address space.");
    }
}

inline void AssertPhysical(DWORD Offset)
{
    XMETAL_DebugVerifyPhysical(Offset);
}

inline void AssertContiguousOrPhysical(void *pv)
{
    DWORD Type = (DWORD)pv & ~(MM_BYTES_IN_PHYSICAL_MAP - 1);

    if (Type != XMETAL_MEMORY_TYPE_CONTIGUOUS && Type != 0)
    {
        DXGRIP("AssertContiguous - the memory is not contiguous or a physical address.");
    }
}

#else

inline void AssertContiguous(void *pv) {}
inline void AssertContiguousOrVideo(void *pv) {}
inline void AssertWriteCombined(void *pv) {}
inline void AssertPhysical(DWORD Offset) {}
inline void AssertContiguousOrPhysical(void *pv) {}

#endif

//------------------------------------------------------------------------------
// IsContiguousMemory
//
// Returns TRUE if the passed-in pointer is one of our contiguous memory
// allocations
//
inline BOOL IsContiguousMemory(
    void *pv)
{
    return ((DWORD)pv & ~(MM_BYTES_IN_PHYSICAL_MAP - 1)) == XMETAL_MEMORY_TYPE_CONTIGUOUS;
}


//------------------------------------------------------------------------------
// IsWriteCombinedMemory
//
// Returns TRUE if the passed-in pointer points to write combined memory.  Calls
// a kernel function to determine the protection on the memory.
//
inline BOOL IsWriteCombinedMemory(
    void *pv)
{
    ULONG Protection = MmQueryAddressProtect(pv);

    return (Protection & PAGE_WRITECOMBINE);
}


//------------------------------------------------------------------------------
// GetGPUAddress
//
// Maps from a virtual address to a GPU address.  The virtual address
// must be in cached or write-combined contiguous memory.  
//
inline DWORD GetGPUAddress(
    void *pv)
{
    return XMETAL_MapToPhysicalOffset(pv);
}

//------------------------------------------------------------------------------
// GetGPUAddressFromWC
//
// Maps a write combined virtual address to a GPU address.
//
inline DWORD GetGPUAddressFromWC(
    void *pv)
{
    AssertContiguous(pv);

    return (DWORD)pv & (MM_BYTES_IN_PHYSICAL_MAP - 1);
}

//------------------------------------------------------------------------------
// GetWriteCombinedAddress
//
// Maps any pointer to physical memory to a write-combined virtual address.
//
inline void *GetWriteCombinedAddress(
    DWORD Offset)
{
    return XMETAL_MapToContiguousAddress(Offset);
}
    
//------------------------------------------------------------------------------
// GetVideoAddress
//
// Maps any pointer to physical memory to a pointer into the video memory
// address space.
//
inline void *GetVideoAddress(
    DWORD Offset)
{
    return XMETAL_MapToVideoAddress(Offset);
}


//------------------------------------------------------------------------------
// GetPhysicalOffset
//
// Maps any pointer to a physical offset
//
inline DWORD GetPhysicalOffset(
    void *pv)
{
    return ((DWORD)pv) & 0xFFFFFFF;
}


//----------------------------------------------------------------------------
// Allocate a chunk of contiguous memory for textures and whatnot.
//
static __forceinline void *AllocateContiguousMemory(
    DWORD Size,
    DWORD Alignment
    )
{
    void *pv = MmAllocateContiguousMemoryEx(Size,
                                            0,
                                            AGP_APERTURE_BYTES - 1,
                                            Alignment,
                                            PAGE_READWRITE | PAGE_WRITECOMBINE);

#if DBG || PROFILE
    if (pv)
    {
        D3D__AllocsContiguous += MmQueryAllocationSize(pv);
    }
#endif

    return pv;
}

//----------------------------------------------------------------------------
// Free a chunk of contiguous memory.
//
static __forceinline void FreeContiguousMemory(
    void *pv
    )
{
#if DBG || PROFILE
    DWORD size = MmQueryAllocationSize(pv);

    ASSERT(D3D__AllocsContiguous >= size);

    D3D__AllocsContiguous -= size;
#endif

    MmFreeContiguousMemory(pv);
}

} // end namespace
