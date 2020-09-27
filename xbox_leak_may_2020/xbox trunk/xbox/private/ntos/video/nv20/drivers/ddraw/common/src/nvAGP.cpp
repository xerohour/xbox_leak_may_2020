/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvAGP.cpp                                                         *
*   AGP Allocation routines.                                                *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 01/12/99 - per context agp heap man.    *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)


//#define SANITY              // do sanity checks
//#define ANALSANITYCHECK     // do a real good job of sanity checking
//#define STATS               // print heap statistics
//#define TIME_ALLOC          // time AGPAlloc calls

#ifdef ANALSANITYCHECK
#define SANITY
#endif

//****************************************************************************
// constants
//****************************************************************************
#define REALSIZE(size)          (4 + ALIGN + GAPSIZE * 2 + (size))
#define MINCHUNKSIZE            (4 * REALSIZE(256 * 256 * 2))       // fit 4 256x256x16 textures
#define ALIGN                   (NV_TEXTURE_OFFSET_ALIGN + 1)
#define GROWSIZE                (128 * sizeof(AGPHEADER))
#define SLACK                   4

#ifdef DEBUG
#define GAPSIZE                 (ALIGN * 4) // must be aligned
#else
#define GAPSIZE                 0
#endif

//
// flags
//
#define AGPFLAG_ALLOCATED       0x00000001
#define AGPFLAG_DISCONTINUITY   0x00000002
#define AGPFLAG_UNUSED          0x00000004

//
// macros
//
#define ISALLOC(x)          ((x) & AGPFLAG_ALLOCATED)
#define ISFREE(x)           (!ISALLOC(x))
#define ISDISCONTINUOUS(x)  ((x) & AGPFLAG_DISCONTINUITY)
#define ISCONTINUOUS(x)     (!ISDISCONTINUOUS(x))
#define ISAVAILABLE(x)      ((x) & AGPFLAG_UNUSED)
#define ISUSED(x)           (!ISAVAILABLE(x))

#ifdef NV_AGP
#define LOWERGAPSTART(p)    (((p)->dwMemory - GAPSIZE))                    // lower no-mans-land
#define UPPERGAPSTART(p)    (((p)->dwMemory + (p)->dwSize))               // upper no-mans-land
#else
#define LOWERGAPSTART(p)    (AGPMEM_ADDR((p)->dwMemory - GAPSIZE))         // lower no-mans-land
#define UPPERGAPSTART(p)    (AGPMEM_ADDR((p)->dwMemory + (p)->dwSize))     // upper no-mans-land
#endif

#define HEADERPTR(dw)       (((dw) == ~0)  ? (AGPHEADER*)NULL : (AGPHEADER*)(getDC()->dwAGPHeapList + (dw)))
#define HEADERADDR(p)       (((p) == NULL) ? ~0 : ((DWORD)p - getDC()->dwAGPHeapList))

#define HASHPTR(dw)         (((dw) == ~0)  ? (AGPHASHENTRY*)NULL : (AGPHASHENTRY*)(getDC()->dwAGPHashTable + (dw)))
#define HASHADDR(p)         (((p) == NULL) ? ~0 : ((DWORD)p - getDC()->dwAGPHashTable))

#define AGPALLOCFLAG_USER   1 // return a user mode address for this allocation


//****************************************************************************
// structures
//****************************************************************************

#ifdef NV_AGP

typedef struct _AGPMAPPING
{
    NV_CFGEX_UNMAP_USER_SPACE_PARAMS addr;
    DWORD dwProcessId;
    _AGPMAPPING *pNext;
} AGPMAPPING, *PAGPMAPPING;

static DWORD      m_dwRMID;
static DWORD      m_dwRootHandle     = 0;
static DWORD      m_dwDeviceHandle   = AGP_DEVICE_HANDLE;
static AGPMAPPING m_pPendingMappings = { { 0, 0 }, 0, 0 };

#endif // NV_AGP

typedef struct
{
    DWORD  dwFlags;         // block status

    DWORD  dwRealMemory;    // actual memory block base
    DWORD  dwRealSize;      // actual memory block size (amount allocated)

    DWORD  dwMemory;        // address returned
    DWORD  dwSize;          // size requested by caller
#ifdef DEBUG
    DWORD  dwSerial;        // allocation serial number (DEBUG only)
#endif // DEBUG
#ifdef NV_AGP
    DWORD  dwRMID;          // block ID
    NV_CFGEX_UNMAP_USER_SPACE_PARAMS addr; // user mode address and memory descriptor list
    DWORD  dwProcessId;     // process ID of creator (when needed)
#else // !NV_AGP
    DWORD  _pad[2];
#endif // !NV_AGP

} AGPHEADER;

typedef struct
{
    DWORD dwAddress;        // actual user address (NULL implies a free entry)
    DWORD dwAGPListOffset;  // offset to surface descriptor (AGPHEADER)
    DWORD dwNext;           // offset to next hash table entry
    DWORD dwPrev;           // offset to prev hash table entry
} AGPHASHENTRY;

//****************************************************************************
// helpers
//****************************************************************************

#ifdef NV_AGP
inline DWORD nvMapUserSpace(AGPHEADER *pCurr)
{
    NV_CFGEX_MAP_USER_SPACE_PARAMS addr;

    addr.kAddr = (void *)pCurr->dwMemory;
    addr.mSize = pCurr->dwSize;
    addr.ppMDL = &pCurr->addr.pMDL;

    DWORD dwRes = NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_MAP_USER_SPACE, &addr, sizeof(addr));

    pCurr->addr.uAddr  = addr.uAddr;
    pCurr->dwProcessId = GetCurrentProcessId();
    DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "Mapped AGP address %08x, user space = %08x (pMDL = %08x, PID = %d)", pCurr->dwMemory, pCurr->addr.uAddr, pCurr->addr.pMDL, pCurr->dwProcessId);

    return dwRes;
}

//-------------------------------------------------------------------------

void nvPurgeAGPMappings(DWORD dwProcessId)
{
    AGPMAPPING *pPrev = &m_pPendingMappings;
    AGPMAPPING *pNext;
    DWORD dwFreed = 0, dwTotal = 0;

    for (AGPMAPPING *pMapping = m_pPendingMappings.pNext; pMapping; pMapping = pNext)
    {
        pNext = pMapping->pNext;
        if (pMapping->dwProcessId == dwProcessId) {
            NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_UNMAP_USER_SPACE, &pMapping->addr, sizeof(pMapping->addr));
            pPrev->pNext = pNext;
            dwFreed++;
            delete pMapping;
        }
        dwTotal++;
    }

    if (dwFreed) {
        DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "Cleaned out %d pending mappings for process %d (%d mappings remaining)", dwFreed, dwProcessId, dwTotal - dwFreed);
    }
}

//-------------------------------------------------------------------------

inline void nvUnmapUserSpace(AGPHEADER *pCurr)
{

    DWORD dwProcessId = GetCurrentProcessId();

    // purge any pending mappings for this process
    if (m_pPendingMappings.pNext) {
        nvPurgeAGPMappings(dwProcessId);
    }

    if (pCurr->dwProcessId == dwProcessId) {
        // unmap
        DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "   Unmapping AGP address %08x, user space = %08x (pMDL = %08x)", pCurr->dwMemory, pCurr->addr.uAddr, pCurr->addr.pMDL);
        NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_UNMAP_USER_SPACE, &pCurr->addr, sizeof(pCurr->addr));
    } else {
        // wrong process ID, add this mapping to pending list
        DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "   Pending mapping added for process %d", pCurr->dwProcessId);
        AGPMAPPING *pNewMapping = new AGPMAPPING;
        if (pNewMapping) {
            pNewMapping->addr.uAddr  = pCurr->addr.uAddr;
            pNewMapping->addr.pMDL   = pCurr->addr.pMDL;
            pNewMapping->dwProcessId = pCurr->dwProcessId;
            pNewMapping->pNext       = m_pPendingMappings.pNext;
            m_pPendingMappings.pNext = pNewMapping;
        }
    }
}
#endif // NV_AGP

//-------------------------------------------------------------------------

#if 0
void dbgValidateAGPMemory(DWORD dwMem, DWORD dwSize)
{
    DWORD dwLimit = ((dwSize + 4095) & ~4095) - 1;

    NV_CFGEX_MEM_INFO_PARAMS info;
    DWORD dwRes = NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_MEM_INFO, &info, sizeof(info));
    DPF("PAT = %08x:%08x", info.uPATHigh, info.uPATLow);
    for(int i = 0; i < 8; i++)
    {
        if (info.uMTRRphysMaskLow[i] & 0x800)
        {
            DPF("MTRRphysBase%d = %08x:%08x, MTRRphysMask%d = %08x:%08x",
                i, info.uMTRRphysBaseHigh[i], info.uMTRRphysBaseLow[i],
                i, info.uMTRRphysMaskHigh[i], info.uMTRRphysMaskLow[i]);
        }
    }
    DWORD _CR3;
    _asm {
        mov eax, cr3;
        mov [_CR3], eax
    }

    for(DWORD dwOffset = 0; dwOffset < dwLimit; dwOffset += 4 * 1024)
    {
        DWORD dwAddress   = dwMem + dwOffset;
        DWORD dwDirectory = (dwAddress >> 22);
        DWORD dwTable     = (dwAddress >> 12) & 0x3FF;
        DWORD dwPTE       = 0x80000000 | _CR3 | (dwDirectory << 2);

        DWORD dwL1 = *(DWORD*)dwPTE;
        DWORD dwL2 = 0x80000000 + (dwL1 & 0xFFFFF000) + (dwTable << 2);
        DWORD dwL3 = *(DWORD*)dwL2;
        DWORD dwCache1 = ((dwL1 >> 3) & 0x03) | ((dwL1 >> 5) & 0x04);
        DWORD dwCache2 = ((dwL3 >> 3) & 0x03) | ((dwL3 >> 5) & 0x04);
        if ((dwCache1 != 0) || (dwCache2 != 1))
        {
            DPF("cache problem @ addr %08x (%08x) %03x %03x", dwAddress, dwL3 & 0xFFFFF000, dwCache1, dwCache2);
            _asm int 3;
        }
    }
}
#endif

//-------------------------------------------------------------------------

DWORD AllocAGPHeap
(
    DWORD dwSize
)
{
    dbgTracePush ("AllocAGPHeap");

    DWORD pAGPMem;

    dwSize += NV_TEXTURE_PAD;

    // get dd global
    LPDDRAWI_DIRECTDRAW_GBL pDrv = getDC()->getDDGlobal();

#ifdef NV_AGP
    DWORD dwLimit = ((dwSize + 4095) & ~4095) - 1;
    ULONG status = NvRmAllocMemory (m_dwRootHandle, 
                                    m_dwDeviceHandle,
                                    m_dwRMID,
                                    NV01_MEMORY_SYSTEM,
                                    (DRF_DEF(OS02, _FLAGS, _LOCATION, _AGP) |
                                    DRF_DEF(OS02, _FLAGS, _COHERENCY, _WRITE_COMBINE)),
                                    (PVOID *) &pAGPMem,
                                    (unsigned long *) &dwLimit);

    if (status) {
        DPF("AllocAGPHeap failed, out of memory");
        return ERR_DXALLOC_FAILED;
    }
    NV_CFGEX_GET_AGP_OFFSET_PARAMS addr;
    addr.linaddr = (void*)pAGPMem;
    NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_GET_AGP_OFFSET, &addr, sizeof(addr));
    if (addr.offset + dwLimit > pDriverData->regMaxAGPLimit) {
        // memory was allocated, but lies outside the context dma range, free it and return an error
        NvRmFree (m_dwRootHandle, m_dwDeviceHandle, m_dwRMID);
        DPF("AllocAGPHeap failed, out of range");
        return ERR_DXALLOC_FAILED;
    }
#else
    pAGPMem = (DWORD)DDHAL32_VidMemAlloc(pDrv, AGP_HEAP, dwSize, 1);
#endif

    DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "Allocated AGP block at %08x (size = %08x)", pAGPMem, dwSize);

    dbgTracePop();
    return pAGPMem;
}

//-------------------------------------------------------------------------

BOOL FreeAGPHeap
(
    AGPHEADER *pCurr
)
{
    dbgTracePush("FreeAgpHeap");

    // get dd global
    LPDDRAWI_DIRECTDRAW_GBL pDrv = getDC()->getDDGlobal();

#ifdef NV_AGP
    // there should be no user mapping on a free block
    nvAssert(pCurr->addr.uAddr == 0);

    if (NvRmFree (m_dwRootHandle, m_dwDeviceHandle, pCurr->dwRMID)) {
        DPF("FreeAGPHeap failed");
        nvAssert (0);
        return FALSE;
    }
#else
    DDHAL32_VidMemFree(pDrv, AGP_HEAP, pCurr->dwRealMemory);
#endif

    DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "Freed AGP block at %08x (size = %08x)", pCurr->dwRealMemory, pCurr->dwRealSize);
    
    dbgTracePop();
    return TRUE;
}

//
// nvAGPAddBackPointer
//
// adds an address to the list of back pointers
//
BOOL nvAGPAddBackPointer
(
    DWORD      dwAddress,
    AGPHEADER *pHeader
)
{
    nvAssert(dwAddress);

    // get hash table index
    DWORD  dwIndex    = ((dwAddress >> 16) ^ (dwAddress >> 8)) & 255;
    DWORD *pHashTable = (DWORD*)getDC()->dwAGPHashTableHead;

    // if have no hash table list yet, get one
    if (!pHashTable)
    {
        getDC()->dwAGPHashTableHead = (DWORD)AllocIPM(4 * 256);
        pHashTable = (DWORD*)getDC()->dwAGPHashTableHead;
        assert (pHashTable);
        memset (pHashTable,~0,4 * 256);
    }

    for (AGPHASHENTRY *pEntry = HASHPTR(0);;)
    {
        // find a free hash table entry
        AGPHASHENTRY *pMax = (AGPHASHENTRY*)(getDC()->dwAGPHashTable + getDC()->dwAGPHashTableMax);
        for (; pEntry < pMax; pEntry++)
        {
            if (!pEntry->dwAddress)
            {
                // add to list (this becomes the new first entry)
                pEntry->dwAddress        = dwAddress;
                pEntry->dwAGPListOffset  = HEADERADDR(pHeader);
                pEntry->dwNext           = pHashTable[dwIndex];
                pEntry->dwPrev           = HASHADDR(NULL);
                AGPHASHENTRY *pEntryOld  = HASHPTR(pHashTable[dwIndex]);
                pHashTable[dwIndex]      = HASHADDR(pEntry);
                if (pEntryOld)
                {
                    pEntryOld->dwPrev    = pHashTable[dwIndex];
                }
                // done
                return TRUE;
            }
        }

        // out of space, make more space
        DWORD dwOldMax = getDC()->dwAGPHashTableMax;
        getDC()->dwAGPHashTableMax += sizeof(AGPHASHENTRY) * 256;
        void *pNewMem;
        if (!ReallocIPM((void*)getDC()->dwAGPHashTable, getDC()->dwAGPHashTableMax, &pNewMem))
        {
            DPF("nvAGPAddBackPointer: ReallocIPM failed");
            return FALSE;
        }
        getDC()->dwAGPHashTable = (DWORD)pNewMem;

        // clear out new part
        memset ((void*)(getDC()->dwAGPHashTable + dwOldMax),0,sizeof(AGPHASHENTRY) * 256);

        // loop back so we can find an entry. we know where one is, so we help a little
        pEntry = (AGPHASHENTRY*)(getDC()->dwAGPHashTable + dwOldMax);
    }

    dbgD3DError();
    return TRUE;
}

//
// nvAGPRemoveBackPointer
//
// removes a back pointer from the hash table
//
void nvAGPRemoveBackPointer
(
    AGPHASHENTRY *pEntry
)
{
    AGPHASHENTRY *pNext = HASHPTR(pEntry->dwNext);
    AGPHASHENTRY *pPrev = HASHPTR(pEntry->dwPrev);
    nvAssert(pEntry->dwAddress);
    if (pNext) pNext->dwPrev = pEntry->dwPrev;
    if (pPrev) pPrev->dwNext = pEntry->dwNext;
    else
    {
        DWORD  dwAddress    = pEntry->dwAddress;
        DWORD  dwIndex      = ((dwAddress >> 16) ^ (dwAddress >> 8)) & 255;
        DWORD *pHashTable   = (DWORD*)getDC()->dwAGPHashTableHead;
        pHashTable[dwIndex] = HASHADDR(pNext);
    }

#ifdef DEBUG
    memset (pEntry,0,sizeof(*pEntry));
#else
    pEntry->dwAddress = 0;
#endif
}

//
// nvAGPFindBackPointer
//
// finds the AGPHEADER that matches the given address
//
AGPHASHENTRY* nvAGPFindBackPointer
(
    DWORD dwAddress
)
{
    // valid hash table?
    if (!getDC()->dwAGPHashTable)
    {
        return NULL;
    }

    // get hash table index
    DWORD  dwIndex    = ((dwAddress >> 16) ^ (dwAddress >> 8)) & 255;
    DWORD *pHashTable = (DWORD*)getDC()->dwAGPHashTableHead;

    // find the address
    AGPHASHENTRY *pEntry = HASHPTR(pHashTable[dwIndex]);
    while (pEntry)
    {
#ifdef DEBUG
        if (!pEntry->dwAddress)
        {
            DPF("nvAGPFindBackPointer: AGP heap insane - empty table entry");
            __asm int 3;
            return NULL;
        }
#endif // DEBUG

        // test
        if (pEntry->dwAddress == dwAddress)
        {
            // got it
            return pEntry;
        }

        // next
        pEntry = HASHPTR(pEntry->dwNext);
    }

    // not found
    return NULL;
}


// debug stuff
#ifdef DEBUG

DWORD dwSerial      = 0;
DWORD dwSerialBreak = 0;

//
// nvAGPDebugTestSetup
//
// fills the no-mans-land around a surface with a known value so we can track
// cases where stuff overwrites the assigned boundaries or something else
// corrupts our memory
//
void nvAGPDebugTestSetup
(
    AGPHEADER *pHeader
)
{
    void *pLowerGapStart = (void*)LOWERGAPSTART(pHeader);
    void *pUpperGapStart = (void*)UPPERGAPSTART(pHeader);

    memset (pLowerGapStart, 0xe7, GAPSIZE);
    memset (pUpperGapStart, 0xe7, GAPSIZE);
}

//
// nvAGPDebugTest
//
// checks that the preset no-mans-land is intact. fail if not.
//
void nvAGPDebugTest
(
    AGPHEADER *pHeader
)
{
    DWORD dwLowerGapStart = LOWERGAPSTART(pHeader);
    DWORD dwUpperGapStart = UPPERGAPSTART(pHeader);
    DWORD dw,cnt;

    for (cnt = GAPSIZE,dw = dwLowerGapStart; cnt; dw++, cnt--)
    {
        if ((*(BYTE*)dw) != 0xe7)
        {
            DPF("AGPHeap underfill : Gap %08x : Fail %08x",dwLowerGapStart,dw);
            DPF("                    pHeader  = %08x",pHeader);
            DPF("                    S#       = %08x",pHeader->dwSerial);
            dbgD3DError();
        }
    }

    for (cnt = GAPSIZE,dw = dwUpperGapStart; cnt; dw++, cnt--)
    {
        if ((*(BYTE*)dw) != 0xe7)
        {
            DPF("AGPHeap overfill : Gap %08x : Fail %08x",dwUpperGapStart,dw);
            DPF("                   pHeader  = %08x",pHeader);
            DPF("                   S#       = %08x",pHeader->dwSerial);
            dbgD3DError();
        }
    }
}

//
// nvAGPHeapDump
//
// shows all memory block
//
void nvAGPHeapDump
(
    void
)
{
    DWORD i, j;

    for (i = getDC()->dwAGPHeapListMax, j = 0; i; j += sizeof(AGPHEADER), i -= sizeof(AGPHEADER))
    {
        char       s1[256];
        char       s2[256];
        AGPHEADER *pCurr = HEADERPTR(j);

        nvSprintf (s1,"%08x:",j);
        if (pCurr->dwFlags & AGPFLAG_DISCONTINUITY)
        {
            nvStrCat (s1," dis");
        }
        if (pCurr->dwFlags & AGPFLAG_UNUSED)
        {
            nvStrCat (s1," empty");
        }
        else
        {
            nvSprintf (s2," rm=0x%08x rs=%d", pCurr->dwRealMemory, pCurr->dwRealSize);
            nvStrCat (s1,s2);

            if (pCurr->dwFlags & AGPFLAG_ALLOCATED)
            {
                nvSprintf (s2," m=0x%08x s=%d #=0x%x", pCurr->dwMemory, pCurr->dwSize, pCurr->dwSerial);
                nvStrCat (s1,s2);
            }
            else
            {
                nvStrCat (s1," free");
            }
        }

        DPF("%s.",s1);
    }
}

//
// nvAGPHeapSanity
//
// checks that the agp heap is intact and setup properly
//
void nvAGPHeapSanity
(
    void
)
{
    DWORD i,j;

    for (i = getDC()->dwAGPHeapListMax, j = 0; i; j += sizeof(AGPHEADER),i -= sizeof(AGPHEADER))
    {
        AGPHEADER *pCurr = HEADERPTR(j);

        // find next
        DWORD      k      = i      - sizeof(AGPHEADER);
        AGPHEADER *pNext = pCurr + 1;
        while (k && ISAVAILABLE(pNext->dwFlags))
        {
            pNext ++;
            k      -= sizeof(AGPHEADER);
        }
        if (!k) pNext = NULL;

        // test back pointer
        if (ISALLOC(pCurr->dwFlags))
        {
            DWORD         dwMemory = pCurr->dwMemory;
            AGPHASHENTRY *pEntry  = nvAGPFindBackPointer(dwMemory);
            DWORD         dwTemp   = pEntry->dwAGPListOffset;

            if (dwTemp != j)
            {
                DPF("nvAGPHeapSanity: back pointer invalid");
                DPF("                 pCurr    = %08x",pCurr);
                DPF("                 dwMemory = %08x",dwMemory);
                DPF("                 dwTemp   = %08x",dwTemp);
                DPF("                 j        = %08x",j);
                DPF("                 S#       = %08x",pCurr->dwSerial);
                DPF("Hit F5 to dump heap blocks...");
                dbgD3DError();
                nvAGPHeapDump();
                __asm int 3;
            }
        }

        // test size
        if ((pNext)
         && ISUSED(pCurr->dwFlags)
         && ISUSED(pNext->dwFlags)
         && ISCONTINUOUS(pNext->dwFlags))
        {
            // real size & memory check out?
            if (pCurr->dwRealMemory + pCurr->dwRealSize != pNext->dwRealMemory)
            {
                DPF("nvAGPHeapSanity: real memory addr & size does not check out");
                DPF("                 pCurr    = %08x",pCurr);
                DPF("                 j        = %08x",j);
                DPF("                 S#       = %08x",pCurr->dwSerial);
                DPF("Hit F5 to dump heap blocks...");
                dbgD3DError();
                nvAGPHeapDump();
                __asm int 3;
            }
        }

        // is memory contained properly and aligned strict enough?
        if (ISALLOC(pCurr->dwFlags))
        {
            DWORD dwRealEnd = pCurr->dwRealMemory + pCurr->dwRealSize;
            DWORD dwEnd     = pCurr->dwMemory     + pCurr->dwSize;

            if ((pCurr->dwMemory < pCurr->dwRealMemory)
             || (dwEnd > dwRealEnd)
             || (pCurr->dwMemory & (ALIGN - 1)))
            {
                DPF("nvAGPHeapSanity: memory outsize of valid range Or not aligned");
                DPF("                 pCurr    = %08x",pCurr);
                DPF("                 dwMemory = %08x",pCurr->dwMemory);
                DPF("                 j        = %08x",j);
                DPF("                 S#       = %08x",pCurr->dwSerial);
                DPF("Hit F5 to dump heap blocks...");
                dbgD3DError();
                nvAGPHeapDump();
                __asm int 3;
            }

#ifdef ANALSANITYCHECK
            // test for over/underrun
            nvAGPDebugTest(pCurr);
#endif
        }
    }
}
#endif // DEBUG

#ifdef STATS
//
// nvAGPShowStats
//
// prints usage statistics
//
void nvAGPShowStats
(
    void
)
{
    DWORD dwTotalAlloc = 0;
    DWORD dwTotalUsed  = 0;
    DWORD i,j;

    for (i = getDC()->dwAGPHeapListMax, j = 0; i; j += sizeof(AGPHEADER),i -= sizeof(AGPHEADER))
    {
        AGPHEADER *pCurr = HEADERPTR(j);

        if (ISUSED(pCurr->dwFlags))
        {
            dwTotalAlloc += pCurr->dwRealSize;

            if (ISALLOC(pCurr->dwFlags))
            {
                dwTotalUsed += pCurr->dwRealSize;
            }
        }

    }

    DPF ("AGPHeap: %d/%d (%d%%) used", dwTotalUsed,dwTotalAlloc,(dwTotalUsed * 100) / dwTotalAlloc);
}
#endif //STATS

//
// nvAGPListInsertFree
//
// creates a free entry right after dwStart
//  returns TRUE on success
//
BOOL nvAGPListInsertFree
(
    DWORD dwStart,
    DWORD *pdwNew
)
{
    // do we want a specific entry or just one at the end of the list?
    // dwStart == ~0 to request eol
    //
    if (dwStart == ~0)
    {
        DWORD      dwMax;
        AGPHEADER *pCurr;

        // get a free header at end of list
        dwMax = getDC()->dwAGPHeapListMax;
        if (dwMax && (ISAVAILABLE((HEADERPTR(dwMax) - 1)->dwFlags))) // at least one available
        {
            // find first used block from the end of the list
            *pdwNew = dwMax;
            pCurr = HEADERPTR(*pdwNew);
            while (*pdwNew && ISAVAILABLE((pCurr - 1)->dwFlags))
            {
                pCurr --;
                *pdwNew -= sizeof(AGPHEADER);
            }

            // allow some slack space if we can - this introduce bubbles in the
            // list to make insertion faster
            if ((dwMax - *pdwNew) > SLACK * sizeof(AGPHEADER))
            {
                *pdwNew += SLACK * sizeof(AGPHEADER);
                pCurr += SLACK;
            }

            // return it
            return TRUE;
        }
        else
        {
            AGPHEADER *pHeader;
            *pdwNew = dwMax; // start of new block
            DWORD i;

            // alloc more space
            dwMax += GROWSIZE;
            void *pNewMem;
            if (!ReallocIPM((void*)getDC()->dwAGPHeapList, dwMax, &pNewMem))
            {
                DPF("nvAGPListInsertFree, ReallocIPM failed");
                return FALSE;
            }
            getDC()->dwAGPHeapList     = (DWORD)pNewMem;
            getDC()->dwAGPHeapListMax  = dwMax;

            // fill with empty headers
            for (pHeader = HEADERPTR(*pdwNew), i = *pdwNew; i < dwMax; pHeader++, i += sizeof(AGPHEADER))
            {
#ifdef DEBUG
                memset (pHeader,0x77,sizeof(*pHeader));
#endif
                pHeader->dwFlags = AGPFLAG_UNUSED;
            }

            // return appropriate entry (with some slack space)
            *pdwNew += SLACK * sizeof(AGPHEADER);
            return TRUE;
        }
    }
    else
    {
        DWORD      dwCount;
        DWORD      dwMax;
        AGPHEADER *pHeader;
        AGPHEADER *pFree;

        // find first free entry after start
        dwCount = 0;
        *pdwNew = dwStart;
        dwMax   = getDC()->dwAGPHeapListMax;
        pHeader   = HEADERPTR(dwStart);

        while ((*pdwNew < dwMax) && !ISAVAILABLE(pHeader->dwFlags))
        {
            dwCount ++;
            pHeader   ++;
            *pdwNew += sizeof(AGPHEADER);
        }

        // if we have not found one add it to the end
        if (*pdwNew >= dwMax)
        {
            // since we get here we know there are no free entries left
            // the following call will return (grow list) a free entry
            // that we just need to move into place
            //
            if (!nvAGPListInsertFree(~0, pdwNew))
            {
                DPF("nvAGPListInsertFree: nvAGPListInsertFree failed");
                return FALSE;
            }
            pHeader   = HEADERPTR(*pdwNew);
            dwCount = (*pdwNew - dwStart) / sizeof(AGPHEADER);
            dwMax   = getDC()->dwAGPHeapListMax;
        }

        // if the free pointer is not adjacent to start,
        // move the header list to make it so
        // also decrement dwCount so we know how many items to move
        if (--dwCount)
        {
            DWORD i;

            *pdwNew = dwStart + sizeof(AGPHEADER);
            pFree  = HEADERPTR(*pdwNew);
            memmove (pFree + 1,pFree,dwCount * sizeof(AGPHEADER));

            // update the back pointers
            pFree ++;
            for (i = dwCount; i; i--, pFree++)
            {
                if (ISALLOC(pFree->dwFlags)) // is in use
                {
                    AGPHASHENTRY *pEntry = nvAGPFindBackPointer(pFree->dwMemory);
                    if (pEntry)
                    {
                        pEntry->dwAGPListOffset += sizeof(AGPHEADER);
                    }
                }
            }
        }

        // done
        return TRUE;
    }
}

/*****************************************************************************
 * exports
 *****************************************************************************/

// CreateAGPHeap
// top-level routine for creation of the texture heap in AGP memory

BOOL nvAGPCreateHeap(void)
{
    dbgTracePush ("CreateAGPHeap");

    DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "nvAGPCreateHeap");

    // shouldn't create the heap more than once
    nvAssert(!pDriverData->nvD3DTexHeapData.dwHandle);
    
    // should only do this if AGP is available
    nvAssert(pDriverData->GARTLinearBase);

    // create context DMA
    if (D3DCreateTextureContexts (pDriverData->GARTLinearBase, pDriverData->regMaxAGPLimit)) {
        dbgTracePop();
        return FALSE;
    }

    // set the handle to indicate heap has been created
    pDriverData->nvD3DTexHeapData.dwHandle = 1;

    nvAGPResetHeap();

#ifdef NV_AGP
    char devName[128];
    DWORD res = NvRmAllocRoot(&m_dwRootHandle);
//    DWORD res = NvAllocRoot(pDriverData->ppdev->hDriver, NV01_ROOT_USER, &m_dwRootHandle);
    nvAssert(res == NVOS01_STATUS_SUCCESS);
    *((DWORD*)devName) = 0;
    res = NvRmAllocDevice (m_dwRootHandle, m_dwDeviceHandle, NV01_DEVICE_0, (BYTE*)devName);
    nvAssert(res == NVOS06_STATUS_SUCCESS);
#endif

    dbgTracePop();
    return TRUE;
}

BOOL nvAGPDestroyHeap(void)
{
    dbgTracePush ("DestroyTextureHeap");

    DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "nvAGPDestroyHeap");

    D3DDestroyTextureContexts (pDriverData);

    // Can't destroy a heap if there isn't one.
    nvAssert (getDC()->nvD3DTexHeapData.dwHandle);

    if (!(getDC()->nvD3DTexHeapData.dwHandle)) { 
        return TRUE;
    }

    // should only do this if AGP is available
    nvAssert(pDriverData->GARTLinearBase);

    getDC()->nvD3DTexHeapData.dwHandle = 0;

    dbgTracePop();
    return TRUE;
}

//
// nvAGPCombine
//
// walks all of the local agp heap and merge adjacent free blocks
//
void nvAGPCombine
(
    void
)
{
    DWORD      dwMax;
    DWORD      dwCurr;
    AGPHEADER *pCurr;

    // for all blocks
    dwMax = getDC()->dwAGPHeapListMax;
    for (dwCurr = 0,pCurr = HEADERPTR(0); dwCurr < dwMax;)
    {
        if (ISUSED(pCurr->dwFlags))
        {
            // get next block
            DWORD      dwNext = dwCurr + sizeof(AGPHEADER);
            AGPHEADER *pNext = pCurr + 1;

            while ((dwNext < dwMax) && ISAVAILABLE(pNext->dwFlags))
            {
                dwNext += sizeof(AGPHEADER);
                pNext ++;
            }
            if (dwNext >= dwMax) break;

            // test if both are free and we do not cross a discontinuous boundary
            if (ISFREE(pCurr->dwFlags)
             && ISFREE(pNext->dwFlags)
             && ISCONTINUOUS(pNext->dwFlags))
            {
                // consolidate two blocks
                pCurr->dwRealSize += pNext->dwRealSize;
                pCurr->dwMemory    = 0;
                pCurr->dwSize      = 0;

#ifdef NV_AGP
                // there should be no user mapping on a free block
                nvAssert(pCurr->addr.uAddr == 0);
#endif

                // remove "next" entry from active list
                pNext->dwFlags = AGPFLAG_UNUSED;

                // do not increment dwCurr
                continue;
            }
            else
            // test if we have a dummy block - i.e. no real size
            if (ISFREE(pCurr->dwFlags)
             && (pCurr->dwRealSize == 0))
            {
                // remove redundant entry from active list
                pCurr->dwFlags = AGPFLAG_UNUSED;
            }
        }

        // next
        pCurr ++;
        dwCurr += sizeof(AGPHEADER);
    }

#ifdef SANITY
    nvAGPHeapSanity();
#endif
}

//
// nvAGPReclaim
//
// try to free up unsued memory. usually called after a nvAGPCombine for max effect
//
void nvAGPReclaim
(
    void
)
{
    DWORD      dwMax;
    DWORD      dwCurr;
    AGPHEADER *pCurr;

    // for all blocks
    dwMax = getDC()->dwAGPHeapListMax;
    for (dwCurr = 0,pCurr = HEADERPTR(0); dwCurr < dwMax;)
    {
        if (ISUSED(pCurr->dwFlags))
        {
            // get next block
            DWORD      dwNext = dwCurr + sizeof(AGPHEADER);
            AGPHEADER *pNext = pCurr + 1;
            while ((dwNext < dwMax) && ISAVAILABLE(pNext->dwFlags))
            {
                dwNext += sizeof(AGPHEADER);
                pNext ++;
            }
            if (dwNext >= dwMax) pNext = NULL;

            // see if the whole allocated block is available for freeing
            //  - current block must be free and a discontinuous boundary
            //  - next must either not exist or must be a discontinuous boundary
            if (((pNext && ISDISCONTINUOUS(pNext->dwFlags))
              || (!pNext))
             && ISFREE(pCurr->dwFlags)
             && ISDISCONTINUOUS(pCurr->dwFlags))
            {
                // free AGP memory
                FreeAGPHeap (pCurr);

                // remove block from active list
                pCurr->dwFlags = AGPFLAG_UNUSED;
            }
        }

        // next
        pCurr ++;
        dwCurr += sizeof(AGPHEADER);
    }

#ifdef SANITY
    nvAGPHeapSanity();
#endif
}

//
// nvAGPResetHeap
//
// cleans up the current agp heap if possible, and reset as empty
//
void nvAGPResetHeap
(
    void
)
{
    // combine and reclaim as much memory as possible
    nvAGPCombine();
    nvAGPReclaim();

    // hash table
    if (getDC()->dwAGPHashTable)
    {
        FreeIPM ((void*)getDC()->dwAGPHashTable);
    }
    if (getDC()->dwAGPHashTableHead)
    {
        FreeIPM ((void*)getDC()->dwAGPHashTableHead);
    }
    getDC()->dwAGPHashTableHead = 0;
    getDC()->dwAGPHashTable     = 0;
    getDC()->dwAGPHashTableMax  = 0;

    // zero out
    if (getDC()->dwAGPHeapList)
    {
        FreeIPM ((void*)getDC()->dwAGPHeapList);
    }
    getDC()->dwAGPHeapList    = 0;
    getDC()->dwAGPHeapListMax = 0;

    //  mark the heap as valid
    getDC()->bAGPHeapValid = TRUE;
#ifdef NV_AGP
    m_dwRMID = AGP_MEMORY_OBJECT_LO;
    if (m_dwRootHandle) {
        DWORD res = NvRmFree (m_dwRootHandle, NV01_NULL_OBJECT, m_dwRootHandle);
        m_dwRootHandle = 0;
        nvAssert(res == NVOS00_STATUS_SUCCESS);
    }
#endif
}

//
// nvAGPInvalidate
//
// marks the heap as invalid. useful when MS rips the heap out from under us
// with no warning whatsoever. calling this function will prevent all heap
// access until it's been reallocated and reset
//
void nvAGPInvalidate
(
    void
)
{
    getDC()->bAGPHeapValid = FALSE;
}

#ifdef NV_AGP
//
// nvAGPGetUserAddr
//
// return a user space mapping for the supplied AGP address
//
DWORD nvAGPGetUserAddr(DWORD dwAddr)
{
    AGPHASHENTRY *pEntry = nvAGPFindBackPointer(dwAddr);

    if (!pEntry)
    {
        DPF("nvGetUserModeAddr: Surface is not in our memory context");
        nvAssert(0);
        return NULL;
    }

    AGPHEADER *pCurr = HEADERPTR(pEntry->dwAGPListOffset);
    nvAssert(pCurr);

    // check if we already have a user space mapping
    if (pCurr->addr.uAddr) {
        DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "User mapping for %08x already exists at %08x", pCurr->dwMemory, pCurr->addr.uAddr);
        return (DWORD)pCurr->addr.uAddr;
    }

    // create a user space mapping
    nvMapUserSpace(pCurr);

    return (DWORD)pCurr->addr.uAddr;
}

#endif

//
// nvAGPAlloc
//
// allocates agp memory for textures
//
void* nvAGPAlloc
(
    DWORD dwSize
)
{
    DWORD dwRealSize;
    DWORD i;

    nvAssert (getDC()->bAGPHeapValid);

    // calculate how much memory we really need to allocate
    dwRealSize = REALSIZE(dwSize);

    // try to allocate the memory:
    //  pass 1 - see if we can find a large enough block out of existing set
    //  pass 2 - try to combine adjacent free blocks and try pass 1 again
    //  pass 3 - allocate a large enouch chunk and retry pass 1 - this is guaranteed to work
    for (i = 0; i < 3; i++)
    {
        DWORD      dwMax;
        DWORD      dwCurr;
        AGPHEADER *pCurr;

        // find free space
        dwMax = getDC()->dwAGPHeapListMax;
        for (dwCurr = 0,pCurr = HEADERPTR(0); dwCurr < dwMax; pCurr ++,dwCurr += sizeof(AGPHEADER))
        {
            if (ISUSED(pCurr->dwFlags)
             && ISFREE(pCurr->dwFlags))
            {
                // check if aligned block matches exactly (implies that this alloc matches a previous alloc & free)
                if (pCurr->dwSize == dwSize)
                {
                    // update flags
                    pCurr->dwFlags |= AGPFLAG_ALLOCATED;

                    // debug preps
#ifdef DEBUG
                    nvAGPDebugTestSetup (pCurr);
                    pCurr->dwSerial = ++dwSerial;
                    if (dwSerial == dwSerialBreak) __asm int 3;
#endif //DEBUG
                    // add back pointer to hash table
                    nvAGPAddBackPointer (pCurr->dwMemory,pCurr);

                    // debug checks
#ifdef SANITY
                    nvAGPHeapSanity();
#endif
#ifdef STATS
                    nvAGPShowStats();
#endif

#ifdef NV_AGP
                    // no user address should exist yet
                    nvAssert(pCurr->addr.uAddr == 0);
                    DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "Allocating AGP memory @ %08x", pCurr->dwMemory);
#endif

                    // done
                    return (void*)pCurr->dwMemory;
                }

                // check if we can fit a block that has to be split
                if (pCurr->dwRealSize >= dwRealSize)
                {
                    DWORD      dwSplit;
                    AGPHEADER *pSplit;

                    // get a free header
                    // reinit some variables since the header list may move in memory
                    if (!nvAGPListInsertFree(dwCurr, &dwSplit))
                    {
                        DPF("nvAGPAlloc: nvAGPListInsertFree failed");
                        nvAssert(0);
                        return (void*)ERR_DXALLOC_FAILED;
                    }
                    pSplit = HEADERPTR(dwSplit);
                    pCurr  = HEADERPTR(dwCurr);
                    dwMax  = getDC()->dwAGPHeapListMax;

#ifdef DEBUG
                    pCurr->dwSerial = ++dwSerial;
                    if (dwSerial  == dwSerialBreak) __asm int 3;
#endif //DEBUG

                    // update split block data
                    pSplit->dwFlags      = 0; // this block is always continuous and always free
                    pSplit->dwRealSize   = pCurr->dwRealSize   - dwRealSize;
                    pSplit->dwRealMemory = pCurr->dwRealMemory + dwRealSize;
                    pSplit->dwSize       = 0;
                    pSplit->dwMemory     = 0;
#ifdef NV_AGP
                    pSplit->addr.uAddr   = 0;
#endif

                    // update current block data
                    pCurr->dwRealSize  = dwRealSize;
                    pCurr->dwFlags    |= AGPFLAG_ALLOCATED;
                    pCurr->dwMemory    = (pCurr->dwRealMemory + GAPSIZE + ALIGN - 1) & ~(ALIGN - 1);
                    if ((pCurr->dwMemory - pCurr->dwRealMemory) <= GAPSIZE) pCurr->dwMemory += ALIGN;
                    pCurr->dwSize      = dwSize;

                    // debug preps
#ifdef DEBUG
                    nvAGPDebugTestSetup (pCurr);
#endif //DEBUG

                    // add back pointer to hash table
                    nvAGPAddBackPointer (pCurr->dwMemory,pCurr);

                    // debug checks
#ifdef SANITY
                    nvAGPHeapSanity();
#endif
#ifdef STATS
                    nvAGPShowStats();
#endif

#ifdef NV_AGP
                    // no user address should exist yet
                    nvAssert(pCurr->addr.uAddr == 0);
                    DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "Allocating AGP memory @ %08x", pCurr->dwMemory);
#endif

                    // done
                    return (void*)pCurr->dwMemory;
                }
            }
        }

        // nothing found
        switch (i)
        {
            case 0:
            {
                // combine free blocks now and try again
                nvAGPCombine();
                break;
            }
            case 1:
            {
                DWORD      dwCurr;
                AGPHEADER *pCurr;
                DWORD      dwNewMemory;
                DWORD      dwNewSize;

                // use this opportunity to free up unused blocks
                //  this can help / speed up the following alloc
                nvAGPReclaim();

                // allocate a new chunk to fit at least this or MINCHUNKSIZE
                dwNewSize   = max(MINCHUNKSIZE,dwRealSize);
#ifdef TIME_ALLOC
                {
                DWORD a = GetTickCount();
                DWORD b;
#endif
                dwNewMemory = AllocAGPHeap(dwNewSize);
#ifdef TIME_ALLOC
                b = GetTickCount();
                DPF ("%d ms", b-a);
                }
#endif
                if (dwNewMemory == ERR_DXALLOC_FAILED)
                {
                    return (void*)ERR_DXALLOC_FAILED;
                }

                // get a free header at end of list
                if (!nvAGPListInsertFree(~0, &dwCurr))
                {
                    DPF("nvAGPAlloc: nvAGPListInsertFree failed");
                    nvAssert(0);
                    return (void*)ERR_DXALLOC_FAILED;
                }
                pCurr = HEADERPTR(dwCurr);

                // populate
#ifdef NV_AGP
                pCurr->dwRMID       = m_dwRMID;
                pCurr->addr.uAddr   = 0;
#endif
                pCurr->dwFlags      = AGPFLAG_DISCONTINUITY;
                pCurr->dwRealMemory = dwNewMemory;
                pCurr->dwRealSize   = dwNewSize;
                pCurr->dwMemory     = 0;
                pCurr->dwSize       = 0;

#ifdef NV_AGP
                // prepare for next alloc
                m_dwRMID++;
#endif

                // free space finder is guaranteed to find enough space for this request
                break;
            }
        }
    }

    // fail utterly
    //  - this cannot really happen but if it does things are pretty insane already
    DPF("nvAGPAlloc: Reality in AGP-Heap land has failed");
    dbgD3DError();
    return (void*)ERR_DXALLOC_FAILED;
}

//
// nvAGPFree
//
// frees agp memory for textures
//
void nvAGPFree
(
    void *pMem
)
{
    if (!getDC()->bAGPHeapValid)
    {
        return;
    }

    // sanity test
    if (IsBadReadPtr(pMem,4))
    {
        // this texture was allocated with our special heap but the context has
        // been destroyed in the mean time so we cannot free thus we ignore it
        DPF("nvAGPFree: invalid request");
        dbgD3DError();
        return;
    }

    // get back pointer
    AGPHASHENTRY *pEntry = nvAGPFindBackPointer((DWORD)pMem);
    if (!pEntry)
    {
        DPF("nvAGPFree: Surface is not in our memory context. May be OK if mode");
        DPF("           switch just occurred. Ignoring.");
        nvAssert(0);
        return;
    }

    AGPHEADER *pCurr = HEADERPTR(pEntry->dwAGPListOffset);
    if (ISALLOC(pCurr->dwFlags))
    {
        DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "Freeing AGP memory @ %08x", pMem);
        // free memory
#ifdef DEBUG
        nvAGPDebugTest (pCurr);
#endif
        pCurr->dwFlags &= ~AGPFLAG_ALLOCATED;

#ifdef NV_AGP
        // delete the user space mapping
        if (pCurr->addr.uAddr) {
            nvUnmapUserSpace(pCurr);
            pCurr->addr.uAddr = 0;
        }
#endif

#ifdef DEBUG
#ifdef NV_AGP
        memset ((void*)pCurr->dwRealMemory, 0xDD, pCurr->dwRealSize);
#else
        memset ((void*)AGPMEM_ADDR(pCurr->dwRealMemory), 0xdd, pCurr->dwRealSize);
#endif
#endif

        // remove back pointer
        nvAGPRemoveBackPointer (pEntry);
    }
    else
    {
        DPF("nvAGPFree: Freeing an already free texture!");
        dbgD3DError();
        return;
    }

#ifdef SANITY
    nvAGPHeapSanity();
#endif
#ifdef STATS
    nvAGPShowStats();
#endif
}

//
// nvAGPGetMemory
//
// returns the total amount of memory we have allocated and how much of it
// is free
//
void nvAGPGetMemory
(
    DWORD *pdwTotal,
    DWORD *pdwFree
)
{
    DWORD      dwTotal = 0;
    DWORD      dwFree  = 0;
    DWORD      dwCurr;
    DWORD      dwMax;
    AGPHEADER *pCurr;

    if (getDC()->bAGPHeapValid) {
        
        // try to compact memory
        nvAGPCombine();
        
        // for all blocks
        dwMax = getDC()->dwAGPHeapListMax;
        for (dwCurr = 0, pCurr = HEADERPTR(0); dwCurr < dwMax;)
        {
            if (ISUSED(pCurr->dwFlags))
            {
                dwTotal += pCurr->dwRealSize;
                if (ISFREE(pCurr->dwFlags)) dwFree += pCurr->dwRealSize;
            }
            
            // next
            pCurr ++;
            dwCurr += sizeof(AGPHEADER);
        }
    }

    // done
    *pdwTotal = dwTotal;
    *pdwFree  = dwFree;
}

#endif  // NVARCH >= 0x04
