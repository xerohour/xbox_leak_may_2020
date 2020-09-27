/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       resource.hpp
 *  Content:    Resource helpers
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

//----------------------------------------------------------------------------
// Helper to get the data pointed to by a resource in the address space
// needed by the GPU. 
//
inline DWORD GetGPUDataFromResource(
    D3DResource *pResource
    )
{
    AssertPhysical(pResource->Data);

    return pResource->Data;
}

//----------------------------------------------------------------------------
// Check a surface to see if it's in use.
//
BOOL IsResourceSetInDevice(
    D3DResource *pResource
    );

//----------------------------------------------------------------------------
// Check a resource to see if it's in use.
//
__forceinline BOOL IsNonSurfaceResourceSetInDevice(
    D3DResource *pResource
    )
{
    ASSERT((pResource->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_SURFACE);

    return (pResource->Common & D3DCOMMON_INTREFCOUNT_MASK) != 0;
}

//----------------------------------------------------------------------------
// Destroys the contents of a resource when it has been completely released.
//
void DestroyResource(
    D3DResource *pResource
    );

//----------------------------------------------------------------------------
// Returns TRUE if the resource is a child surface (a surface pointing to
// a mipmap level or cubemap level), which always requires  special handling 
// of the parent for reference counts and surface times.
//
__forceinline BOOL IsChildSurface(
    D3DResource *pResource
    )
{
    return ((pResource->Common & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_SURFACE)
         && (((D3DSurface*) pResource)->Parent != NULL);
}

//----------------------------------------------------------------------------
// Add an internal reference to a non-surface resource.  This indicates that 
// the resource is currently being used in the device.
//
__forceinline void InternalAddRef(
    D3DResource *pResource
    )
{
    ASSERT((pResource->Common & D3DCOMMON_INTREFCOUNT_MASK) != D3DCOMMON_INTREFCOUNT_MASK);
    ASSERT(!IsChildSurface(pResource));

    pResource->Common += (1 << D3DCOMMON_INTREFCOUNT_SHIFT);
}

//----------------------------------------------------------------------------
// Release an internal reference for non-surfaces.
//
__forceinline void InternalRelease(
    D3DResource *pResource
    )
{
    ASSERT((pResource->Common & D3DCOMMON_INTREFCOUNT_MASK) != 0);
    ASSERT(!IsChildSurface(pResource));

    pResource->Common -= (1 << D3DCOMMON_INTREFCOUNT_SHIFT);

    // Destroy the resource if both the internal and external reference counts
    // are now zero.
    if ((pResource->Common & (D3DCOMMON_INTREFCOUNT_MASK | D3DCOMMON_REFCOUNT_MASK)) == 0)
    {
        DestroyResource(pResource);
    }
}

//----------------------------------------------------------------------------
// Add an internal reference to a surface resource.  This indicates that 
// the resource is currently being used in the device.
//
void InternalAddRefSurface(
    D3DResource *pResource
    );

//----------------------------------------------------------------------------
// Release an internal reference for surfaces.
//
void InternalReleaseSurface(
    D3DResource *pResource
    );
                       
//----------------------------------------------------------------------------
// Helper to get the data pointed to by a resource. This always returns
// a pointer to the data in the CPU's write-combined address space and will
// need to be manually mapped to whatever view you need.
//
BYTE *GetDataFromResource(
    D3DResource *pResource
    );

} // end namespace
