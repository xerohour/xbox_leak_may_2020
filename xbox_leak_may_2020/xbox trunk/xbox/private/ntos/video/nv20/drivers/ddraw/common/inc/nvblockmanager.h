/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvBlockManager.cpp                                                *
*   block management/caching/renaming                                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Franck Diard/Michael Conrad  21Feb2001  rel20 development            *
*                                                                           *
\***************************************************************************/


#ifdef MCFD

#if (NVARCH >= 0x04)

#include "nvprecomp.h" 
#pragma hdrstop
#endif

class CMipLevelSet;
class CBlockListHeader;
class CMipLevel;
class CTexture;
class CBlockList;
class CBlockDescriptor;


enum
{
    RENAME_POLICY_NO_COPY   = 1,            // rename if allowed
    RENAME_POLICY_COPY   = 2,            // rename if allowed and also copy contents forward
};
/*
enum
{
    ALLOCATE_SIMPLE         = 0x00000001,
    ALLOCATE_TILED          = 0x00000002,
    ALLOCATE_AS_ZBUFFER_UNC = 0x00000010,   // uncompressed z-buffer
    ALLOCATE_AS_ZBUFFER_CMP = 0x00000020,   // compressed z-buffer
    ALLOCATE_AS_ZBUFFER     = 0x00000030    // any z format
};

enum
{
    FLAG_UPTODATE           = 0x00000100,   // surface data is up to date (can be used)
    FLAG_RENDERTARGET       = 0x00000200,   // surface is a render target
    FLAG_ZBUFFER            = 0x00000400,   // surface is a zbuffer
    FLAG_DONOTOWNMEMORY     = 0x00000800,   // memory management is done by somebody else (not self)
    FLAG_TILED              = 0x00001000,   // surface was allocated in tiled memory
    FLAG_SWIZZLED           = 0x00002000,   // surface is a swizzled (used by nvSetRenderTargetDX7)
    FLAG_available          = 0x00004000,
    FLAG_HASALPHA           = 0x00008000,   // surface format has alpha
    FLAG_ISDXT              = 0x00010000,   // surface format is DXT
#ifdef  STEREO_SUPPORT
    FLAG_STEREO             = 0x00020000,   // surface is stereo
#endif  //STEREO_SUPPORT

};
typedef enum
{
    LOCK_NONE               = 0,            // reserved
    LOCK_READ               = 1,            // READ
    LOCK_WRITE              = 2,            // WRITE
}BMLockPermission;

typedef enum
{
    HEAP_LOCATION_MASK      = 0x0000001f,   // surface location - preserve order
}BMMask;
typedef enum
{
    HEAP_VID                = 0x01,
    HEAP_AGP                = 0x02,
    HEAP_PCI                = 0x04,
    HEAP_SYS                = 0x08,

    HEAP_DYNSYS             = 0x18,
    HEAP_DYNSYSBIT          = 0x10,
    HEAP_ANYWHERE           = HEAP_VID | HEAP_AGP | HEAP_PCI | HEAP_SYS | HEAP_DYNSYS,
    HEAP_NOWHERE            = 0x0,
}DWORD;
 */

extern DWORD g_adwSurfaceContextDMA[9];

inline BOOL  BMhwCanRead         (DWORD dwFlags)         { return (dwFlags & CSimpleSurface::HEAP_LOCATION_MASK) != CSimpleSurface::HEAP_SYS; }
inline BOOL  BMhwCanWrite        (DWORD dwFlags)         { return (dwFlags & CSimpleSurface::HEAP_LOCATION_MASK) == CSimpleSurface::HEAP_VID; }
inline BOOL  BMisReadCached      (DWORD dwFlags)         { return (dwFlags & CSimpleSurface::HEAP_LOCATION_MASK) >= CSimpleSurface::HEAP_PCI; }
inline BOOL  BMisWriteCached     (DWORD dwFlags)         { return (dwFlags & CSimpleSurface::HEAP_LOCATION_MASK) >= CSimpleSurface::HEAP_PCI; }
inline BOOL  BMisWriteCombined   (DWORD dwFlags)         { return (dwFlags & CSimpleSurface::HEAP_LOCATION_MASK) <= CSimpleSurface::HEAP_AGP; }

#define MAXHEAD 4           // is there a limit here ?

BOOL GarbageCollect(DWORD Heap);

class CBlockManager
{
    friend CBlockList;
    friend CBlockDescriptor;

    //active blocks

	CBlockDescriptor * m_apVIDFirst[1];           // one heap per chip
	CBlockDescriptor * m_pAGPFirst;
	CBlockDescriptor * m_pPCIFirst;
	CBlockDescriptor * m_pSYSFirst;

    // cached block

	CBlockDescriptor * m_apVIDAllocCacheFirst[1];
	CBlockDescriptor * m_pAGPAllocCacheFirst;
	CBlockDescriptor * m_pPCIAllocCacheFirst;
	CBlockDescriptor * m_pSYSAllocCacheFirst;


    DWORD m_dwAllocatedVIDMemory;
    DWORD m_adwAllocatedVIDMemoryPerDevice[1];
    DWORD m_dwAllocatedAGPMemory;
    DWORD m_dwAllocatedPCIMemory;
    DWORD m_dwAllocatedSYSMemory;

    DWORD m_dwActiveBlocks;
    DWORD m_dwCachedBlocks;

public:

    CBlockManager();


    BOOL CBlockManager::GarbageCollect(DWORD Heaps);

    BOOL CreateBlock(CBlockList * BlockList,            // create, allocate and register a block, clone of SimpleSurface::create
        DWORD dwPitch,
        DWORD dwHeight,
        DWORD dwDepth,
        DWORD dwBPPGranted,
        DWORD dwAllowedHeaps,
        DWORD dwPreferredHeap,
        DWORD dwAllocFlags
    );

    BOOL AllocateBlock(
        CBlockDescriptor * ToFill,
        DWORD dwPitch,
        DWORD dwHeight,
        DWORD dwDepth,
        DWORD dwBPPGranted,
        DWORD dwAllowedHeaps,
        DWORD dwPreferredHeap,
        DWORD dwAllocFlags
    );

    BOOL OwnBlock(              CBlockList * BlockList,
                                DWORD dwAddress, DWORD dwPitch, DWORD dwLogHeight, 
                                DWORD dwHeapLocation, BOOL bOwnMemory );

    BOOL DisownBlock( CBlockList * BlockList);

    BOOL CreateBlock(CBlockDescriptor * ToClone);
    BOOL FreeBlock(CBlockList * BlockList, CBlockDescriptor * ToFree);
    BOOL FreeList(CBlockList * BlockList);
    BOOL Lock(CBlockList * BlockList, DWORD dwAccess, DWORD dwNewSize=-1);


private:
    BOOL AddToCachedBlockList(CBlockDescriptor * ToAdd);
    BOOL RegisterBlock(CBlockList * BlockList, CBlockDescriptor * ToAdd);
};

class CBlockDescriptor          // basically, a CSimpleSurface
{
    friend CBlockList;
    friend CBlockManager;

    DWORD   m_dwAddress;       // surface address [absolute]
    DWORD   m_dwOffset;        // surface offset [depends on dma context]
    DWORD   m_dwFlags;         // flags
    DWORD   m_dwHeap;
    DWORD   m_dwRetireDate;    // date that HW access retires
    DWORD   m_dwSize;         // size
    DWORD   m_dwAllocFlags;
    DWORD   m_dwPitch;
    DWORD   m_dwWidth;
    DWORD   m_dwHeight;        // height = 1 for vertex buffers, etc
    DWORD   m_dwDepth;         // depth = 1 for vertex buffers, etc

    CBlockList * m_pOwner; 

    CBlockDescriptor * m_pNextGlobal; // block manager use
    CBlockDescriptor * m_pPreviousGlobal; // block manager use        for debranching

	CBlockDescriptor * m_pNextRenamed; // renaming use
	CBlockDescriptor * m_pPreviousRenamed; // renaming use

public:
 


    inline BOOL  hwCanRead         (void)         const { return (BMhwCanRead(m_dwFlags)); }
    inline BOOL  hwCanWrite        (void)         const { return (BMhwCanWrite(m_dwFlags)); }
    inline BOOL  isReadCached      (void)         const { return (BMisReadCached(m_dwFlags)); }
    inline BOOL  isWriteCached     (void)         const { return (BMisWriteCached(m_dwFlags)); }
    inline BOOL  isWriteCombined   (void)         const { return (BMisWriteCombined(m_dwFlags)); }

    inline DWORD getAddress        (void)         const { return m_dwAddress; }
    inline DWORD getOffset         (void)         const { return m_dwOffset;  }
    inline DWORD getSize           (void)         const { return m_dwSize;  }

    inline DWORD getContextDMA     (void)         const { return g_adwSurfaceContextDMA[m_dwFlags & CSimpleSurface::HEAP_LOCATION_MASK]; }
    inline DWORD getHeapLocation   (void)         const { return m_dwFlags & CSimpleSurface::HEAP_LOCATION_MASK; }
    inline DWORD getFlags          (void)         const { return m_dwFlags; }

    BOOL IsBusy();

    CBlockDescriptor()
    {
        memset(this, 0, sizeof(CBlockDescriptor));
    }
};


class CBlockList
{
public:
    BOOL m_renaming;
    BOOL m_bOwnMemory;

	CBlockDescriptor * m_pActive;
	DWORD   m_dwNb;
	DWORD   m_dwMax;

    BOOL m_cpuLocked;
    BOOL m_gpuLocked;

    DWORD m_dwAllowedHeaps;
    DWORD m_dwPreferredHeap;
    DWORD m_dwBPPGranted;

    CBlockList(DWORD Max=6)
    {
        memset(this, 0, sizeof(CBlockList));
        m_dwMax=Max;
    }

    void  LockForGPU(DWORD dwAccess);
    void  UnlockForGPU(void);

    void  LockForCPU(DWORD dwAccess, DWORD dwRenamePolicy=RENAME_POLICY_NO_COPY);
    void  UnlockForCPU(void);

    BOOL Rename(DWORD dwRenamingCopyPolicy);

public:


    void  enableRenaming( ) {m_renaming=TRUE;};
    void  disableRenaming() {m_renaming=FALSE;};            // should cleanup


    inline DWORD modifyDDCaps (DWORD dwCaps) const
    {
        // clear all flags
        dwCaps &= ~(DDSCAPS_VIDEOMEMORY    |
                    DDSCAPS_LOCALVIDMEM    |
                    DDSCAPS_NONLOCALVIDMEM |
                    DDSCAPS_SYSTEMMEMORY);
        // put back the flags we want
        switch (m_pActive->m_dwFlags & CSimpleSurface::HEAP_LOCATION_MASK)
        {
            case CSimpleSurface::HEAP_VID:
                dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                break;
            case CSimpleSurface::HEAP_AGP:
            case CSimpleSurface::HEAP_PCI:
                dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM);
                break;
            // DDRAW just hates it if you specify the DDSCAPS_SYSTEMMEMORY flag. basically it will
            //  never call back to free surfaces. Ugghh.
            // believe it or not, it actually want the VidMem flag set!
            default:
                dwCaps |= (DDSCAPS_VIDEOMEMORY);
                break;
        }
        // return the new flags
        return (dwCaps);
    }
};



//////////////////////////// 

class CMipLevelSet
{
	void * m_data;  //pointer to the whole bitmap

	CBlockList m_blockList;   // list header of the blocks belonging to the renaming chain

	int m_pNbLevels;	   //number of items in the array
	CMipLevel * m_pLevels;     // the array  6 mip level -> new CMipLevel[6]
    BOOL m_bDirtyMipLevels;

	CTexture * m_pOwner;

	AllocateCreateVolume(DWORD dwWitdh, DWORD dwHeight, DWORD dwDepth, DWORD dwMip, DWORD SizeOfItem, DWORD dwFlags);
	AllocateCreate2DTexture(DWORD dwWitdh, DWORD dwHeight, DWORD dwMip, DWORD SizeOfItem, DWORD dwFlags);
	AllocateCreateCubeMap(DWORD dwWitdh, DWORD dwHeight, DWORD dwMip, DWORD SizeOfItem, DWORD dwFlags);

	UpdateMipMapChain(int MipLevelToExclude); //Update bitmap
	Lock(); // BM->Lock(m_blockList, rename_policy);
	UnLock();
};

class CMipLevel
{
	CMipLevelSet * m_pOwner;
	DWORD m_dwDataOffset;
	DWORD m_dwHeight;
	DWORD m_dwWidth;
	DWORD m_dwPitch;
	Lock(); // m_pOwner->Lock()
};



#endif