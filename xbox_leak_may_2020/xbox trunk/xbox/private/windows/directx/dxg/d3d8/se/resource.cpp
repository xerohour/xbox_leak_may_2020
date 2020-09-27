/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       resource.cpp
 *  Content:    Implementation of the D3DResource class.
 *
 ***************************************************************************/

#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

//------------------------------------------------------------------------------
// Global private data list

struct PrivateDataNode
{
    PrivateDataNode    *pNext;
    void               *pObject;
    GUID                guid;
    DWORD               size;
    DWORD               flags;

    union
    {
        IUnknown       *pUnknown;
        BYTE            Data[1];
    };
};

static PrivateDataNode *g_pPrivateData;

//------------------------------------------------------------------------------
// Helper that finds a private data node for a specific object/guid in the 
// private data list.

PrivateDataNode* FindPrivateData
(
    void *pObject, 
    REFGUID refguid
)
{
    PrivateDataNode *pNode = g_pPrivateData;

    while (pNode != NULL)
    {
        if (pNode->pObject == pObject && pNode->guid == refguid)
        {
            return pNode;
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
// Helper to free all of the private data associated with an object that
// is about to go away.

void CleanPrivateData
(
    void *p
)
{
    PrivateDataNode *pNode = g_pPrivateData;
    PrivateDataNode **ppPrev = &g_pPrivateData;

    while (pNode != NULL)
    {
        if (pNode->pObject == p)
        {
            *ppPrev = pNode->pNext;

            if (pNode->flags & D3DSPD_IUNKNOWN)
            {
                pNode->pUnknown->Release();
            }

            MemFree(pNode);

            pNode = *ppPrev;
        }
        else
        {
            ppPrev = &pNode->pNext;
            pNode = pNode->pNext;
        }
    }
}

//----------------------------------------------------------------------------
// Helper to get the data pointed to by a resource. This always returns
// a pointer to the data in the CPU's write-combined address space.
//
BYTE *GetDataFromResource
(
    D3DResource *pResource
)
{    
    if (pResource->Common & D3DCOMMON_VIDEOMEMORY)
    {
        return (BYTE *)GetVideoAddress(pResource->Data);
    }
    else
    {
        return (BYTE *)GetWriteCombinedAddress(pResource->Data);
    }
}

//----------------------------------------------------------------------------
// Destroys the contents of a resource when it has been completely released.
//
void DestroyResource(
    D3DResource *pResource
    )
{
    if (DBG_CHECK(!(pResource->Common & D3DCOMMON_D3DCREATED)))
    {
        DXGRIP("DestroyResource - The reference count of a non-d3d created resource went to zero.");
    }

    DWORD Type = pResource->Common & D3DCOMMON_TYPE_MASK;

    // Make sure this object isn't being used by the GPU.  We will always
    // block, it is the caller's responsibility to check to see if this
    // object may be freed before releasing this if they do not want
    // to block.
    //
    // There is no need to do this for a surface that does not
    // own its memory.
    //
    if (Type != D3DCOMMON_TYPE_SURFACE || pResource->Common & D3DSURFACE_OWNSMEMORY)
    {
        BlockOnResource(pResource);
    }
    
    // If anybody actually uses the private data then we should
    // use a bit on the object to see if any private data has been 
    // set on it or not.  Otherwise we'll walk the whole private data
    // list each time an object is destroyed.
    //
    CleanPrivateData(pResource);

    // Get rid of our data.  The data for an index buffer is allocated
    // directly with the header.
    //
    if (Type == D3DCOMMON_TYPE_SURFACE)
    {
        if (pResource->Common & D3DSURFACE_OWNSMEMORY)
        {
            FreeContiguousMemory(GetWriteCombinedAddress(pResource->Data));
        }
    }

    else if (Type == D3DCOMMON_TYPE_PUSHBUFFER) 
    {
        if (!(pResource->Common & D3DPUSHBUFFER_RUN_USING_CPU_COPY))
        {
            // A push-buffers always has a virtual address for 'Data':
            FreeContiguousMemory((void*) pResource->Data);
        }
    }
    else if ((Type != D3DCOMMON_TYPE_INDEXBUFFER) &&
             (Type != D3DCOMMON_TYPE_FIXUP))
    {
        FreeContiguousMemory(GetWriteCombinedAddress(pResource->Data));
    }

    // Destroy the header.
    MemFree(pResource);
}

//----------------------------------------------------------------------------
// Check a resource to see if it's in use.
//
// Unlike the faster IsNonSurfaceResourceSetInDevice, this handles any type
// of resource.
//
BOOL IsResourceSetInDevice(
    D3DResource *pResource
    )
{
    CHECK(pResource, "IsResourceSetInDevice");

    if (pResource->Common & D3DCOMMON_INTREFCOUNT_MASK)
    {
        return TRUE;
    }

    if ((pResource->Common & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_SURFACE)
    {
        D3DBaseTexture *pParent = PixelJar::GetParent((D3DPixelContainer *)pResource);

        if (pParent && (pParent->Common & D3DCOMMON_INTREFCOUNT_MASK))
        {
            return TRUE;
        }
    }

    return FALSE;
}

//----------------------------------------------------------------------------
// Add an internal reference to a resource.  This indicates that the
// resource is currently being used in the device.
//
void InternalAddRefSurface(
    D3DResource *pResource
    )
{
    ASSERT((pResource->Common & D3DCOMMON_INTREFCOUNT_MASK) != D3DCOMMON_INTREFCOUNT_MASK);
    ASSERT((pResource->Common & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_SURFACE);

    // If the resource being referenced is a surface then we also need
    // to increment the refcount on its parent.  We only do this once.
    //
    if (!(pResource->Common & D3DCOMMON_INTREFCOUNT_MASK))
    {
        D3DBaseTexture *pParent = PixelJar::GetParent((D3DPixelContainer *)pResource);
    
        if (pParent)
        {
            InternalAddRef(pParent);
        }
    }

    pResource->Common += (1 << D3DCOMMON_INTREFCOUNT_SHIFT);
}

//----------------------------------------------------------------------------
// Release an internal reference for surfaces.
//
void InternalReleaseSurface(
    D3DResource *pResource
    )
{
    ASSERT((pResource->Common & D3DCOMMON_INTREFCOUNT_MASK) != 0);
    ASSERT((pResource->Common & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_SURFACE);

    if ((pResource->Common & D3DCOMMON_INTREFCOUNT_MASK) == (1 << D3DCOMMON_INTREFCOUNT_SHIFT))
    {
        // If this is the last internal release from a surface then we need
        // to release its internal reference on the parent.
        //
        D3DBaseTexture *pParent = PixelJar::GetParent((D3DPixelContainer *)pResource);

        if (pParent)
        {
            InternalRelease(pParent);
        }

        // Only destroy this if the external refcount is zero.
        if ((pResource->Common & D3DCOMMON_REFCOUNT_MASK) == 0)
        {
            DestroyResource(pResource);

            return;
        }
    }

    pResource->Common -= (1 << D3DCOMMON_INTREFCOUNT_SHIFT);
}
                       
//------------------------------------------------------------------------------
// D3DResource_AddRef
//
extern "C"
ULONG WINAPI D3DResource_AddRef
(
    D3DResource *pResource
) 
{
    COUNT_API(API_D3DRESOURCE_ADDREF);
    CHECK(pResource, "D3DResource_AddRef");

    ASSERT(D3DCOMMON_REFCOUNT_MASK == 0x0000FFFF);

    // If the resource being referenced is a surface then we also need
    // to increment the refcount on its parent.  We only do this once.
    //
    if ((pResource->Common & D3DCOMMON_REFCOUNT_MASK) == 0)
    {
        if ((pResource->Common & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_SURFACE)
        {
            D3DBaseTexture *pParent = PixelJar::GetParent((D3DPixelContainer *)pResource);
    
            if (pParent)
            {
                pParent->AddRef();
            }
        }
    }

    ASSERTMSG((pResource->Common & D3DCOMMON_REFCOUNT_MASK) != D3DCOMMON_REFCOUNT_MASK,
              "D3DResource_AddRef overflow - was a Release() forgotten somewhere?");

    return ++pResource->Common & D3DCOMMON_REFCOUNT_MASK;
}

//------------------------------------------------------------------------------
// D3DResource_Release
//
// If the GPU is currently using this object when the last call to release
// is made then this call will block until the GPU is done with this object.
// The caller will have to manually check this if they do not want this call
// to block.
//
extern "C"
ULONG WINAPI D3DResource_Release
(
    D3DResource *pResource
) 
{
    COUNT_API(API_D3DRESOURCE_RELEASE);
    CHECK(pResource, "D3DResource_Release");

    ASSERT(D3DCOMMON_REFCOUNT_MASK == 0x0000FFFF);

    if ((pResource->Common & D3DCOMMON_REFCOUNT_MASK) == 1)
    {
        // If this is the last external release from a surface then we need
        // to release its external reference on the parent.
        //
        if ((pResource->Common & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_SURFACE)
        {
            D3DBaseTexture *pParent = PixelJar::GetParent((D3DPixelContainer *)pResource);

            if (pParent)
            {
                pParent->Release();
            }
        }

        // Only destroy this if the internal refcount is zero.
        if ((pResource->Common & D3DCOMMON_INTREFCOUNT_MASK) == 0)
        {
            DestroyResource(pResource);

            return 0;
        }
    }

    ASSERTMSG((pResource->Common & D3DCOMMON_REFCOUNT_MASK) != 0,
              "D3DResource_Release underflow - was an AddRef() forgotten somewhere?");

    return --pResource->Common & D3DCOMMON_REFCOUNT_MASK;
}

//------------------------------------------------------------------------------
// D3DResource_GetType

extern "C"
D3DRESOURCETYPE WINAPI D3DResource_GetType
(
    D3DResource *pResource
) 
{
    COUNT_API(API_D3DRESOURCE_GETTYPE);
    CHECK(pResource, "D3DResource_GetType");

    DWORD Type = pResource->Common & D3DCOMMON_TYPE_MASK;

    switch(Type)
    {
    case D3DCOMMON_TYPE_VERTEXBUFFER:
        return D3DRTYPE_VERTEXBUFFER;

    case D3DCOMMON_TYPE_INDEXBUFFER:
        return D3DRTYPE_INDEXBUFFER;

    case D3DCOMMON_TYPE_PUSHBUFFER:
        return D3DRTYPE_PUSHBUFFER;

    case D3DCOMMON_TYPE_FIXUP:
        return D3DRTYPE_FIXUP;

    case D3DCOMMON_TYPE_PALETTE:
        return D3DRTYPE_PALETTE;

    case D3DCOMMON_TYPE_TEXTURE:
        {
            DWORD Format = ((D3DBaseTexture *)pResource)->Format;

            if (Format & D3DFORMAT_CUBEMAP)
            {
                return D3DRTYPE_CUBETEXTURE;
            }
            else if ((Format & D3DFORMAT_DIMENSION_MASK) > (2 << D3DFORMAT_DIMENSION_SHIFT))
            {
                return D3DRTYPE_VOLUMETEXTURE;  
            }
            else
            {
                return D3DRTYPE_TEXTURE;
            }
        }

    case D3DCOMMON_TYPE_SURFACE:
        {
            DWORD Format = ((D3DPixelContainer *)pResource)->Format;

            if ((Format & D3DFORMAT_DIMENSION_MASK) > (2 << D3DFORMAT_DIMENSION_SHIFT))
            {
                return D3DRTYPE_VOLUME;
            }
            else
            {
                return D3DRTYPE_SURFACE;
            }
        }

    default:
        NODEFAULT("D3DResource_GetType - unknown type\n");
        
        return (D3DRESOURCETYPE)0;
    }
}

//------------------------------------------------------------------------------
// D3DResource_IsBusy
//
// Check to see if a resource is currently being used by the GPU.
//
extern "C"
BOOL WINAPI D3DResource_IsBusy
(
    D3DResource *pResource
)
{
    COUNT_API(API_D3DRESOURCE_ISBUSY);
    CHECK(pResource, "D3DResource_IsBusy");

    CDevice *pDevice = g_pDevice;
    D3DResource *pParent = PixelJar::GetParent(pResource);

    if (pParent)
    {
        if (IsResourceSetInDevice(pResource))
        {
            return TRUE;
        }

        pResource = pParent;
    }

    if (IsResourceSetInDevice(pResource))
    {
        return TRUE;
    }

    BOOL Result = (pResource->Lock != 0) 
                && (pDevice->IsTimePending(pResource->Lock));

    // If this resource is not busy then reset the lock.  This is not needed
    // to protect from time rollover (our push-buffer logic handles this 
    // neatly), but rather because we document this behavior in our docs 
    // under "Using Resources From Multiple Threads": 
    //
    //      The following methods can be called on a resource as long as 
    //      the Lock field of the resource is set to zero and the resource 
    //      is not set on IDirect3DDevice8. The Lock field is automatically 
    //      set to zero if IDirect3DResource8::IsBusy is called and returns 
    //      FALSE.
    //
    //      ...

    if (!Result)
    {
        pResource->Lock = 0;
    }

    return Result;
}

//------------------------------------------------------------------------------
// D3DResource_GetDevice

extern "C"
void WINAPI D3DResource_GetDevice
(
    D3DResource *pResource,
    D3DDevice  **ppDevice
)
{
    COUNT_API(API_D3DRESOURCE_GETDEVICE);
    CHECK(pResource, "D3DResource_GetDevice");

    *ppDevice = g_pDevice;
    g_pDevice->AddRef();
}

//------------------------------------------------------------------------------
// D3DResource_Register

extern "C"
VOID WINAPI D3DResource_Register(
    D3DResource *pResource,
    void *pBase
    )
{
    COUNT_API(API_D3DRESOURCE_REGISTER);

    DWORD type = pResource->Common & D3DCOMMON_TYPE_MASK;
    void *pMemory = (BYTE*) pBase + pResource->Data;

    if (DBG_CHECK(!pResource))
    {
        DXGRIP("D3DResource_Register - NULL this pointer.");
    }

    if (DBG_CHECK(pResource->Common & D3DCOMMON_D3DCREATED))
    {
        DXGRIP("D3DResource_Register - Do not register a D3D created resource.");
    }

    if (DBG_CHECK(type == D3DCOMMON_TYPE_INDEXBUFFER))
    {
        DXGRIP("D3DResource_Register - Do not register index buffers.");
    }

    if (DBG_CHECK(type == D3DCOMMON_TYPE_FIXUP))
    {
        DXGRIP("D3DResource_Register - Do not register fixups.");
    }

    if (DBG_CHECK(!IsContiguousMemory(pMemory)))
    {
        DXGRIP("D3DResource_Register - The object must be in physically contiguous memory.");
    }

    if (DBG_CHECK(IsWriteCombinedMemory(pResource)))
    {
        DXGRIP("D3DResource_Register - D3DResource structure should not be in write-combined memory.");
    }

    if (DBG_CHECK(pResource->Lock != 0))
    {
        DXGRIP("D3DResource_Register - Lock field should be zero.");
    }

    // Push-buffers always set 'Data' to the virtual address.
    //
    if (type == D3DCOMMON_TYPE_PUSHBUFFER)
    {
        pResource->Data = (DWORD) pMemory;
    }
    else
    {
        pResource->Data = GetGPUAddress(pMemory);
    }

    // This will check for the proper alignment which is why we do it after
    // we set the data pointer.
    //
    CHECK(pResource, "D3DResource_Register");

#if DBG

    if (type == D3DCOMMON_TYPE_TEXTURE || type == D3DCOMMON_TYPE_SURFACE)
    {
        D3DPixelContainer *pPixels = (D3DPixelContainer*)pResource;

        if (pPixels->Size != 0)
        {
            DWORD Width, Height, Depth, Pitch, Slice;
            D3DFORMAT Format;
        
            PixelJar::GetSize(pPixels, 0, &Width, &Height, &Depth, &Pitch, &Slice);
            Format = PixelJar::GetFormat(pPixels);
        
            if (Pitch < CalcPitch(Width, BitsPerPixelOfD3DFORMAT(Format)))
            {
                if (Pitch != 64)
                {
                    DXGRIP("pResource->Size has a bad pitch value");
                }
                else
                {
                    DXGRIP("pResource->Size has a bad pitch value of 64 bytes.\n"
                           "(Could it be that a surface is being loaded that "
                           "wasn't created with the \nnew D3DSIZE_PITCH_MASK "
                           "encoding?)");
                }
            }
        }

        if ((pPixels->Format & D3DFORMAT_DMACHANNEL_MASK) == 0)
        {
            DXGRIP("pResource->Format must have D3DFORMAT_DMACHANNEL_A or _B set\n");
        }
    }

#endif

}

//------------------------------------------------------------------------------
// D3DResource_BlockUntilNotBusy

extern "C"
VOID WINAPI D3DResource_BlockUntilNotBusy(
    D3DResource *pResource
    )
{
    COUNT_API(API_D3DRESOURCE_BLOCKUNTILNOTBUSY);
    CHECK(pResource, "D3DResource_BlockUntilNotBusy");

    // We can get rid of the extra call here by making our
    // internal methods call this directly and moving the BlockOnResource
    // code in here, but then the API counter would also count the internal
    // blocks.  Not good.
    //   
    BlockOnResource(pResource);
}

#if DBG

LONG ExceptionFilter()
{
    WARNING("Possible attempt to use D3DSPD_IUNKNOWN with an object");
    WARNING("not derived from the IUnknown interface");
    return EXCEPTION_CONTINUE_SEARCH;
}

#endif // DBG

//------------------------------------------------------------------------------
// D3DResource_SetPrivateData

extern "C"
HRESULT WINAPI D3DResource_SetPrivateData
(
    D3DResource *pResource, 
    REFGUID refguid, 
    CONST void *pData, 
    DWORD SizeOfData, 
    DWORD Flags
)
{
    COUNT_API(API_D3DRESOURCE_SETPRIVATEDATA);
    CHECK(pResource, "D3DResource_SetPrivateData");

    PrivateDataNode *pNode;
    DWORD cbExtraData;

    if (DBG_CHECK(TRUE))
    {
        if (Flags & ~D3DSPD_IUNKNOWN)
        {
            DXGRIP("Invalid flags to SetPrivateData");
        }

        if (Flags & D3DSPD_IUNKNOWN)
        {
            if (SizeOfData != sizeof(LPVOID))
            {
                DXGRIP("Invalid SizeOfData for IUnknown to SetPrivateData");
            }
        }
    }

    // Find the node in our list if any and free it
    pNode = FindPrivateData(pResource, refguid);
    if (pNode != NULL)
    {
        D3DResource_FreePrivateData(pResource, refguid);
    }

    // No need to allocate more data to store the IUknown pointer
    if (Flags & D3DSPD_IUNKNOWN)
    {
        cbExtraData = 0;
    }
    else
    {
        cbExtraData = SizeOfData;
    }

    // Allocate the node and initialize the fields
    pNode = (PrivateDataNode*)MemAlloc(sizeof(PrivateDataNode) + SizeOfData);
    if (pNode == NULL)
    {
        return E_OUTOFMEMORY;
    }

    pNode->size = SizeOfData;
    pNode->guid = refguid;
    pNode->flags = Flags;
    pNode->pObject = pResource;

    // Copy the data portion over
    if (Flags & D3DSPD_IUNKNOWN)
    {
        pNode->pUnknown = (IUnknown*)pData;

    // If the AddRef has problems, the ExceptionFilter will notify the caller
    // the most likely cause of the problem is that the object is not derived
    // from IUnknown. Xbox D3D objects are not derived from IUnknown.

#if DBG
    __try
    {
#endif // DBG
        pNode->pUnknown->AddRef();
#if DBG
    }
    __except (ExceptionFilter())
    {
    }
#endif // DBG
    }
    else
    {
        memcpy(pNode->Data, pData, SizeOfData);
    }

    // Link it into the list
    pNode->pNext = g_pPrivateData;
    g_pPrivateData = pNode;

    return D3D_OK;
}   

//------------------------------------------------------------------------------
// D3DResource_GetPrivateData

extern "C"
HRESULT WINAPI D3DResource_GetPrivateData
(
    D3DResource *pResource, 
    REFGUID refguid, 
    void *pData, 
    DWORD *pSizeOfData
)
{
    COUNT_API(API_D3DRESOURCE_GETPRIVATEDATA);
    CHECK(pResource, "D3DResource_GetPrivateData");

    PrivateDataNode *pNode;

    // Find the node in our list
    pNode = FindPrivateData(pResource, refguid);
    if (pNode == NULL)
    {
        return D3DERR_NOTFOUND;
    }

    // Is the user just asking for the size?
    if (pData == NULL)
    {
        *pSizeOfData = pNode->size;
        return D3D_OK;
    }

    // Check if we were given a large enough buffer
    if (*pSizeOfData < pNode->size)
    {
        // If the buffer is insufficient, return
        // the necessary size in the out parameter
        *pSizeOfData = pNode->size;

        // An error is returned since pvBuffer != NULL and
        // no data was actually returned.
        return D3DERR_MOREDATA;
    }

    // There is enough room; so just overwrite with the right size
    *pSizeOfData = pNode->size;

    if (pNode->flags & D3DSPD_IUNKNOWN)
    {
        // Add-Ref the returned object
        *(IUnknown**)pData = pNode->pUnknown;
        pNode->pUnknown->AddRef();
        return D3D_OK;
    }
    else
    {
        memcpy(pData, pNode->Data, pNode->size);
    }

    return D3D_OK;
}

//------------------------------------------------------------------------------
// D3DResource_FreePrivateData

extern "C"
void WINAPI D3DResource_FreePrivateData
(
    D3DResource *pResource, 
    REFGUID refguid
)
{
    COUNT_API(API_D3DRESOURCE_FREEPRIVATEDATA);
    CHECK(pResource, "D3DResource_FreePrivateData");

    PrivateDataNode *pNode;
    PrivateDataNode **ppPrev;

    pNode = g_pPrivateData;
    ppPrev = &g_pPrivateData;

    while (pNode != NULL)
    {
        if (pNode->pObject == pResource && pNode->guid == refguid)
        {
            *ppPrev = pNode->pNext;

            if (pNode->flags & D3DSPD_IUNKNOWN)
            {
                pNode->pUnknown->Release();
            }

            MemFree(pNode);
            return;
        }

        ppPrev = &pNode->pNext;
        pNode = pNode->pNext;
    }
}

} // end of namespace
