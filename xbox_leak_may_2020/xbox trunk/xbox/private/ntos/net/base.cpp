tag// ---------------------------------------------------------------------------------------
// base.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

// ---------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_XBDM_SERVER
#define XNET_TRACE_PREFIX   "XBDM"
#elif defined(XNET_FEATURE_ONLINE)
#define XNET_TRACE_PREFIX   "XONLINE"
#else
#define XNET_TRACE_PREFIX   "XNET"
#endif

DefineTag(Timer,    0);
DefineTag(poolWarn, TAG_ENABLE);
DefineTag(poolDump, 0);

// ---------------------------------------------------------------------------------------
// Crypto Constants
// ---------------------------------------------------------------------------------------

extern "C" const BYTE g_abOakleyGroup1Mod[CBDHG1] =
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xc9, 0x0f, 0xda, 0xa2, 0x21, 0x68, 0xc2, 0x34,
    0xc4, 0xc6, 0x62, 0x8b, 0x80, 0xdc, 0x1c, 0xd1,
    0x29, 0x02, 0x4e, 0x08, 0x8a, 0x67, 0xcc, 0x74,
    0x02, 0x0b, 0xbe, 0xa6, 0x3b, 0x13, 0x9b, 0x22,
    0x51, 0x4a, 0x08, 0x79, 0x8e, 0x34, 0x04, 0xdd,
    0xef, 0x95, 0x19, 0xb3, 0xcd, 0x3a, 0x43, 0x1b,
    0x30, 0x2b, 0x0a, 0x6d, 0xf2, 0x5f, 0x14, 0x37,
    0x4f, 0xe1, 0x35, 0x6d, 0x6d, 0x51, 0xc2, 0x45,
    0xe4, 0x85, 0xb5, 0x76, 0x62, 0x5e, 0x7e, 0xc6,
    0xf4, 0x4c, 0x42, 0xe9, 0xa6, 0x3a, 0x36, 0x20,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

extern "C" const BYTE g_abOakleyGroup1Base[CBDHG1] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02
};

// ---------------------------------------------------------------------------------------
// CXnBase (Params)
// ---------------------------------------------------------------------------------------

#undef  DEFINE_PARAM_
#define DEFINE_PARAM_(idx, name, def, min, max) typedef int _farf_##name[offsetof(XNetStartupParams, name) == idx];
#undef  DEFINE_PARAM
#define DEFINE_PARAM(name, def, min, max)
XNETPARAMDEFS()     // If this barfs, XNETPARAMDEFS is out of sync with XNetStartupParams
#undef  DEFINE_PARAM_
#define DEFINE_PARAM_(idx, name, def, min, max) def,
#undef  DEFINE_PARAM
#define DEFINE_PARAM(name, def, min, max) def,
const XNetParams CXnBase::s_XNetParamsDef = { XNETPARAMDEFS() };
#undef  DEFINE_PARAM_
#define DEFINE_PARAM_(idx, name, def, min, max) min,
#undef  DEFINE_PARAM
#define DEFINE_PARAM(name, def, min, max) min,
const XNetParams CXnBase::s_XNetParamsMin = { XNETPARAMDEFS() };
#undef  DEFINE_PARAM_
#define DEFINE_PARAM_(idx, name, def, min, max) max,
#undef  DEFINE_PARAM
#define DEFINE_PARAM(name, def, min, max)  max,
const XNetParams CXnBase::s_XNetParamsMax = { XNETPARAMDEFS() };
#undef DEFINE_PARAM

// ---------------------------------------------------------------------------------------
// CLeakTracker
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_ASSERT

#define LEAKS_PER_ALLOC     128

DefineTag(LeakWarn, TAG_ENABLE);

void * CXnBase::LeakAdd(CLeakInfo * pli, void * pv, UINT cb, ULONG tag)
{
    ICHECK(BASE, USER|UDPC|SDPC);

    if (pv && !pli->_fLeakDisable)
    {
        RaiseToDpc();

        CLeak * pLeak = NULL;

        if (pli->_cLeak == pli->_cLeakAlloc)
        {
            pLeak = (CLeak *)HalAlloc((pli->_cLeakAlloc + LEAKS_PER_ALLOC) * sizeof(CLeak), 0);

            if (pLeak == NULL)
            {
                if (pli->_pLeak)
                {
                    HalFree(pli->_pLeak);
                    pli->_pLeak = NULL;
                    pli->_cLeak = 0;
                }

                TraceSz(LeakWarn, "Leak tracker ran out of memory.  Tracking disabled.");
                pli->_fLeakDisable = TRUE;
                return(pv);
            }

            if (pli->_pLeak)
            {
                memcpy(pLeak, pli->_pLeak, pli->_cLeakAlloc * sizeof(CLeak));
                HalFree(pli->_pLeak);
            }

            pli->_pLeak = pLeak;
            pli->_cLeakAlloc += LEAKS_PER_ALLOC;
        }

        if (LeakFind(pli, pv, &pLeak))
        {
            AssertSz(FALSE, "Attempt to leak track the same memory more than once.  Tracking disabled.");
            HalFree(pli->_pLeak);
            pli->_pLeak = NULL;
            pli->_cLeak = 0;
            pli->_fLeakDisable = TRUE;
            return(pv);
        }

        UINT cbMov = (pli->_cLeak - (pLeak - pli->_pLeak)) * sizeof(CLeak);

        if (cbMov > 0)
        {
            memmove(pLeak + 1, pLeak, cbMov);
        }

        pLeak->_pv   = pv;
        pLeak->_cb   = cb;
        pLeak->_tag  = tag;
        pli->_cLeak += 1;
    }

    return(pv);
}

void * CXnBase::LeakDel(CLeakInfo * pli, void * pv)
{
    ICHECK(BASE, USER|UDPC|SDPC);

    if (pv && !pli->_fLeakDisable)
    {
        RaiseToDpc();

        CLeak * pLeak = NULL;

        if (LeakFind(pli, pv, &pLeak))
        {
            Assert(pLeak->_pv == pv);

            int cbMov = (pli->_cLeak - (pLeak - pli->_pLeak) - 1) * sizeof(CLeak);

            if (cbMov > 0)
            {
                memmove(pLeak, pLeak + 1, cbMov);
            }

            pli->_cLeak -= 1;
        }
        else
        {
            AssertSz(FALSE, "Leak tracker can't find memory block in its table");
        }
    }

    return(pv);
}

BOOL CXnBase::LeakFind(CLeakInfo * pli, void * pv, CLeak ** ppLeak)
{
    ICHECK(BASE, UDPC|SDPC);

    int     iLo     = 0;
    int     iHi     = (int)pli->_cLeak;
    int     iMid;
    CLeak * pLeak   = pli->_pLeak;
    BOOL    fFound  = FALSE;

    while (iLo < iHi)
    {
        iMid  = (iLo + iHi) >> 1;
        pLeak = pli->_pLeak + iMid;

        if (pLeak->_pv == pv)
        {
            iLo = iMid;
            fFound = TRUE;
            break;
        }
        else if (pLeak->_pv < pv)
            iLo = iMid + 1;
        else
            iHi = iMid;
    }

    *ppLeak = pli->_pLeak + iLo;

    return(fFound);
}

void CXnBase::LeakTerm(CLeakInfo * pli)
{
    if (pli->_cLeak > 0)
    {
        CLeak * pLeak   = pli->_pLeak;
        UINT cLeak      = pli->_cLeak;
        UINT cbLeak     = 0;

        for (; cLeak > 0; --cLeak, ++pLeak)
        {
            cbLeak += pLeak->_cb;
            TraceSz3(LeakWarn, "Leaked %5d bytes at %08lX - %s", pLeak->_cb, pLeak->_pv,
                     pli->_pfnLeakTag(pLeak->_tag));
        }

        TraceSz1(LeakWarn, "Leaked %5d bytes total", cbLeak);
    }

    if (pli->_pLeak)
    {
        HalFree(pli->_pLeak);
        pli->_pLeak = NULL;
    }
}

const char * XnLeakTagToString(ULONG tag)
{
    switch (tag)
    {
        #undef  XNETPTAG
        #define XNETPTAG(_name, _tag) case PTAG_##_name: return(#_name); break;
        XNETPTAGLIST()
    }

    return("?");
}

#endif

// ---------------------------------------------------------------------------------------
// CXnBase (Pool Allocator)
// ---------------------------------------------------------------------------------------

//
// Pool allocation block header: We intentionally let this have
// the same size as the system pool header. But we've restructured
// the fields in such a way that if you try to free a memory block
// that was allocated out of our private pool to the system pool,
// you'd get a bugcheck.
//

struct PoolEntry
{
    BYTE bOver;
        // if busy, the amount of overhead in the block
    BYTE bBusy;
        // Whether the block is allocated or free

    WORD tag;
        // We're only using 2 bytes for pool tag,
        // since all of our pool tags have the form NET*.

    WORD blockSize;
    WORD previousSize;
        // Size of this block and the previous block in 32-bit units

    LIST_ENTRY links;
        // A free pool entry has two additional pointer fields
        // for maintaining a doubly-linked list of free blocks.
};

#define POOL_BLOCK_SHIFT 5
#define POOL_BLOCK_SIZE (1 << POOL_BLOCK_SHIFT)
#define MAX_POOL_SIZE (POOL_BLOCK_SIZE * 0xffff)
#define POOL_HEADER_SIZE offsetof(PoolEntry, links)

#define MarkPoolEntryBusy(_entry) ((_entry)->bBusy = 1)
#define MarkPoolEntryFree(_entry) ((_entry)->bBusy = 0)
#define IsPoolEntryBusy(_entry) ((_entry)->bBusy != 0)
#define IsPoolEntryFree(_entry) ((_entry)->bBusy == 0)
#define TagPoolEntry(_entry, _tag) ((_entry)->tag = (WORD) ((_tag) >> 16))

#define GetPoolEntryHeader(_ptr) \
        ((PoolEntry*) ((BYTE*) (_ptr) - POOL_HEADER_SIZE))
#define GetPoolEntryNext(_entry) \
        ((PoolEntry*) ((BYTE*) (_entry) + ((UINT) (_entry)->blockSize << POOL_BLOCK_SHIFT)))
#define GetPoolEntryPrev(_entry) \
        ((PoolEntry*) ((BYTE*) (_entry) - ((UINT) (_entry)->previousSize << POOL_BLOCK_SHIFT)))

//
// Beginning and ending address of the entire pool
//

#define IsPoolEntryValid(_entry) ((_entry) >= _pvPoolStart && (_entry) < _pvPoolEnd)

#define InsertFreePoolEntry(_entry) { \
            LIST_ENTRY* _head; \
            _head = &_aleFree[ \
                        (_entry)->blockSize <= MAX_SMALL_BLOCKS ? \
                            (_entry)->blockSize - 1 : \
                            MAX_SMALL_BLOCKS]; \
            InsertHeadList(_head, &(_entry)->links); \
        }


void * CXnBase::PoolAlloc(size_t size, ULONG tag)
{
    ICHECK(BASE, USER|UDPC|SDPC);

#ifdef XNET_FEATURE_VMEM
    if (VMemIsEnabled())
    {
        return(LeakAdd(&_leakinfo, VMemAlloc(size), size, tag));
    }
#endif

    PoolEntry* entry;

    Assert(size > 0 && size <= MAX_POOL_SIZE - POOL_HEADER_SIZE);
    size_t size_orig = size;
    size = (size + (POOL_HEADER_SIZE + POOL_BLOCK_SIZE - 1)) >> POOL_BLOCK_SHIFT;

    RaiseToDpc();

    // Find a free pool block that's large enough for us.
    // Handle small block allocation with the quick lookup.
    if (size <= MAX_SMALL_BLOCKS)
    {
        UINT index;

        for (index=size-1; index <= MAX_SMALL_BLOCKS; index++)
        {
            if (!IsListEmpty(&_aleFree[index]))
            {
                entry = GetPoolEntryHeader(_aleFree[index].Flink);
                goto found;
            }
        }
    }
    else
    {
        LIST_ENTRY* head;
        LIST_ENTRY* list;

        head = &_aleFree[MAX_SMALL_BLOCKS];
        list = head->Flink;
        while (list != head)
        {
            entry = GetPoolEntryHeader(list);
            if ((WORD) size <= entry->blockSize)
                goto found;
            list = list->Flink;
        }
    }

    TraceSz1(poolWarn, "Pool allocation failed: %d blocks", size);
    TraceSz(poolWarn, "Make sure you promptly call recv() to receive incoming data.");
    TraceSz(poolWarn, "You may also want to consider setting a larger pool size.");

#if DBG
    if (Tag(poolDump))
        PoolDump();
#endif

    return(NULL);

found:
    // Take it out of the free list
    RemoveEntryList(&entry->links);

    // If we didn't use up the entire free block,
    // put the remaining portion back on the free list.
    if (entry->blockSize > size) {
        UINT leftover = entry->blockSize - size;
        PoolEntry* next;

        entry->blockSize = (WORD) size;

        next = GetPoolEntryNext(entry);
        next->blockSize = (WORD) leftover;
        next->previousSize = (WORD) size;
        next->tag = entry->tag;
        MarkPoolEntryFree(next);
        InsertFreePoolEntry(next);

        next = GetPoolEntryNext(next);
        Assert(IsPoolEntryValid(next));
        next->previousSize = (WORD) leftover;
    }

    MarkPoolEntryBusy(entry);
    TagPoolEntry(entry, tag);
    entry->bOver = (BYTE)((size << POOL_BLOCK_SHIFT) - size_orig);

#ifdef XNET_FEATURE_ASSERT
    LeakAdd(&_leakinfo, (BYTE *)entry + POOL_HEADER_SIZE, size_orig, tag);
#endif

    return (BYTE*) entry + POOL_HEADER_SIZE;
}

void * CXnBase::PoolAllocZ(size_t size, ULONG tag)
{
    ICHECK(BASE, USER|UDPC|SDPC);

    void * pv = PoolAlloc(size, tag);

    if (pv)
    {
        memset(pv, 0, size);
    }

    return(pv);
}

void CXnBase::PoolFree(void * ptr)
{
    ICHECK(BASE, USER|UDPC|SDPC);

    if (ptr == NULL)
    {
        return;
    }

#ifdef XNET_FEATURE_ASSERT
    LeakDel(&_leakinfo, ptr);
#endif

#ifdef XNET_FEATURE_VMEM
    if (VMemIsEnabled())
    {
        VMemFree(ptr);
        return;
    }
#endif

    PoolEntry* entry;
    PoolEntry* prev;
    PoolEntry* next;
    WORD blockSize;

    entry = GetPoolEntryHeader(ptr);
    Assert(IsPoolEntryValid(entry) && IsPoolEntryBusy(entry));

    RaiseToDpc();

    MarkPoolEntryFree(entry);
    blockSize = entry->blockSize;

    //
    // Check to see if the block before this one is free
    // If so, collapse the two free blocks together
    //
    prev = GetPoolEntryPrev(entry);
    Assert(IsPoolEntryValid(prev));
    if (IsPoolEntryFree(prev)) {
        RemoveEntryList(&prev->links);
        prev->blockSize = (WORD) (prev->blockSize + blockSize);
        entry = prev;
    }

    //
    // Check if the block after this one is free
    // If so, collapse the two free blocks together
    //
    next = GetPoolEntryNext(entry);
    Assert(IsPoolEntryValid(next) && next->previousSize == blockSize);
    if (IsPoolEntryFree(next)) {
        RemoveEntryList(&next->links);
        entry->blockSize = (WORD) (entry->blockSize + next->blockSize);
    }

    //
    // If we performed any collapsing, 
    // update the previousSize field of the next block.
    //
    if (entry->blockSize != blockSize) {
        next = GetPoolEntryNext(entry);
        Assert(IsPoolEntryValid(next));
        next->previousSize = entry->blockSize;
    }

    InsertFreePoolEntry(entry);
}

#if DBG

void CXnBase::PoolDump()
{
    ICHECK(BASE, USER|UDPC|SDPC);

    PoolEntry* entry;
    PoolEntry* prev;
    UINT cbFree = 0;
    UINT cbBusy = 0;
    UINT cbOver = 0;
    UINT cbPool = (BYTE *)_pvPoolEnd - (BYTE *)_pvPoolStart;

    TraceSz(poolWarn, "+\n-------------------------------------------------------------------------");
    TraceSz2(poolWarn, "Private Pool Dump (%08lX - %08lX)", _pvPoolStart, _pvPoolEnd);
    TraceSz(poolWarn, "");

    prev = NULL;
    entry = (PoolEntry *)_pvPoolStart;

    while (entry < _pvPoolEnd)
    {
        BOOL fIsFree = IsPoolEntryFree(entry);
        BOOL fIsOver = (entry == _pvPoolStart) || ((BYTE *)entry + (1 << POOL_BLOCK_SHIFT)) == (BYTE *)_pvPoolEnd;
        UINT cb      = (entry->blockSize << POOL_BLOCK_SHIFT);

        if (fIsFree)
        {
            cb -= POOL_HEADER_SIZE;
            cbFree += cb;
            cbOver += POOL_HEADER_SIZE;
            TraceSz3(poolWarn, "%08lX f %7d (+%2d) bytes",
                     (UINT_PTR)entry + POOL_HEADER_SIZE, cb, POOL_HEADER_SIZE);
        }
        else
        {
            cb -= entry->bOver;
            cbBusy += cb;
            cbOver += entry->bOver;

            TraceSz4(poolWarn, "%08lX   %7d (+%2d) bytes %s",
                     (UINT_PTR)entry + POOL_HEADER_SIZE, cb, entry->bOver,
                     XnLeakTagToString(('xTEN' & 0xFFFF) | (entry->tag << 16)));
        }

        if (prev)
        {
            Assert(prev->blockSize == entry->previousSize);
            prev = entry;
        }

        entry = GetPoolEntryNext(entry);
    }

    Assert(entry == _pvPoolEnd);

    TraceSz(poolWarn, "");
    TraceSz1(poolWarn, "%7d bytes used", cbBusy);
    TraceSz1(poolWarn, "%7d bytes free", cbFree);
    TraceSz1(poolWarn, "%7d bytes overhead", cbOver);
    TraceSz1(poolWarn, "%7d bytes entire pool", cbPool);
    TraceSz(poolWarn, "+-------------------------------------------------------------------------\n");

    Assert(cbBusy + cbFree + cbOver == cbPool);

    if (Tag(poolDump) >= TAG_BREAK)
    {
        DbgBreak();
    }
}

#endif // DBG

// ---------------------------------------------------------------------------------------
// CXnBase (Rand)
// ---------------------------------------------------------------------------------------

void CXnBase::RandInit(XNetInitParams * pxnip)
{
    TCHECK(USER);

    BYTE    abSha1Ctx[XC_SERVICE_SHA_CONTEXT_SIZE];
    BYTE    abRand[512];
    BYTE    abHash[XC_SERVICE_DIGEST_SIZE];
    UINT    cbRand = 0;

    // Gather system randomness into the abRand vector

    cbRand = HalRandGather(abRand, sizeof(abRand));
    Assert(cbRand <= sizeof(abRand));

    // Compute a SHA1 digest of all of the gathered random bits

    XcSHAInit(abSha1Ctx);
    XcSHAUpdate(abSha1Ctx, pxnip->abSeed, sizeof(pxnip->abSeed));
    XcSHAUpdate(abSha1Ctx, (BYTE *)&pxnip->liTime, sizeof(pxnip->liTime));
    XcSHAUpdate(abSha1Ctx, abRand, cbRand);
    XcSHAFinal(abSha1Ctx, abHash);

    // Initialize an RC4 machine with the given digest as a key

    XcRC4Key(_abRandRc4Struct, sizeof(abHash), abHash);
    
    // Throw away the first 256 bytes of the RC4 machine

    XcRC4Crypt(_abRandRc4Struct, 256, abRand);

    // Update the seed value with the next 20 bytes of the RC4 machine

    XcRC4Crypt(_abRandRc4Struct, sizeof(pxnip->abSeed), pxnip->abSeed);
}

void CXnBase::Rand(BYTE * pb, UINT cb)
{
    ICHECK(BASE, USER|UDPC|SDPC);
    RaiseToDpc();
    XcRC4Crypt(_abRandRc4Struct, cb, pb);
}

ULONG CXnBase::RandLong()
{
    ICHECK(BASE, USER|UDPC|SDPC);
    ULONG ul;
    Rand((BYTE *)&ul, sizeof(ULONG));
    return(ul);
}

// ---------------------------------------------------------------------------------------
// CXnBase (Packet)
// ---------------------------------------------------------------------------------------

CPacket * CXnBase::PacketAlloc(ULONG tag, UINT uiFlags, UINT cbPayload, UINT cbPkt, PFNPKTFREE pfn)
{
    ICHECK(BASE, USER|UDPC|SDPC);

    Assert(sizeof(PFNPKTFREE) == sizeof(void *));

    CPacket *   ppkt;
    UINT        cbPtr;
    UINT        cbPad = 0;
    UINT        cbHdr = 0;

    // We shouldn't be creating CPacket classes that aren't four-byte aligned

    if (cbPkt == 0)
        cbPkt = sizeof(CPacket);

    Assert(cbPkt == ROUNDUP4(cbPkt));

    // Add in the ethernet header

    cbPkt += ROUNDUP4(sizeof(CEnetHdr));

    // This offset tells us where the [Payload] begins.

    cbPtr = cbPkt;

    // Add in the size of all the headers in the packet based on its type.

    cbPkt += CPacket::_abPktTypeInfo[((uiFlags & PKTF_TYPE_MASK) >> PKTF_TYPE_SHIFT) * PKTI_MAX + PKTI_SIZE];

    if (uiFlags & PKTF_TYPE_ESP)
    {
        cbHdr  = (uiFlags & PKTF_TYPE_UDP) ? sizeof(CUdpHdr) : (uiFlags & PKTF_TYPE_TCP) ? sizeof(CTcpHdr) : 0;
        cbPkt -= cbHdr;
        cbPad  = cbPkt;

        if (uiFlags & PKTF_CRYPT)
        {
            cbPkt += ROUNDUP8(XC_SERVICE_DES_BLOCKLEN + cbHdr + cbPayload + offsetof(CEspTail, _abHash));
            cbPad  = cbPkt - cbPad - (XC_SERVICE_DES_BLOCKLEN + cbHdr + cbPayload + offsetof(CEspTail, _abHash));
        }
        else
        {
            cbPkt += ROUNDUP4(cbPayload + cbHdr + offsetof(CEspTail, _abHash));
            cbPad  = cbPkt - cbPad - (cbPayload + cbHdr + offsetof(CEspTail, _abHash));
        }

        cbPkt += sizeof(CEspTail) - offsetof(CEspTail, _abHash);
    }
    else
    {
        // Add in the size of the user-specified payload

        cbPkt += cbPayload;
    }

    // Allocate the packet and initialize it

    if (uiFlags & PKTF_POOLALLOC)
        ppkt = (CPacket *)PoolAlloc(cbPkt, tag);
    else 
        ppkt = (CPacket *)SysAlloc(cbPkt, tag);

    if (ppkt)
    {
        ppkt->Init(uiFlags, (BYTE *)ppkt + cbPtr, cbPkt - cbPtr, pfn ? pfn : (PFNPKTFREE)PacketFree);

        // If this is an [ESP] packet, we need to store the _bPadLen field now since
        // we just went through a lot of trouble to compute it.

        if (uiFlags & PKTF_TYPE_ESP)
        {
            CEspTail * pEspTail = ppkt->GetEspTail();
            
            pEspTail->_bPadLen = (BYTE)cbPad;

            // Fill the padding (if any) with the byte series 1, 2, 3, ...

            for (BYTE * pb = (BYTE *)pEspTail; cbPad > 0; )
            {
                *--pb = (BYTE)cbPad--;
            }
        }

        IFDBG(ppkt->SetAllocated();)

        ppkt->Validate();
    }
    else
    {
        TraceSz1(Warning, "Out of memory allocating a CPacket of size %ld bytes", cbPkt);
    }

    return(ppkt);
}

void CXnBase::PacketFree(CPacket * ppkt)
{
    ICHECK(BASE, USER|UDPC|SDPC);
    Assert(!ppkt->TestFlags(PKTF_RECV_LOOPBACK));

#if DBG
    Assert(ppkt->IsAllocated());
#endif

    if (ppkt->TestFlags(PKTF_POOLALLOC))
        PoolFree(ppkt);
    else
        SysFree(ppkt);
}

void * CPacket::GetHdr(int iHdr)
{
    Assert(IsIp());
    Assert(PKTF_IHL_SHIFT == 2);
    Assert(iHdr == PKTI_ESP || iHdr == PKTI_UDP_TCP);
    Assert(PKTI_UDP_TCP == 1);
    return(     (BYTE *)_pv
            +   (_wFlags & PKTF_IHL_MASK)
            +   _abPktTypeInfo[(GetType() >> PKTF_TYPE_SHIFT) * PKTI_MAX + iHdr]
            +   (((!!(_wFlags & PKTF_CRYPT)) & iHdr) << 3));
}

#if DBG

const char * const CPacket::_aszPktTypeName[] =
{
    "[ENET]^[IP][Payload]",
    "[ENET]^[IP][ESP][Payload][ESPT]",
    "[ENET]^[IP][UDP][Payload]",
    "[ENET]^[IP][ESP][UDP][Payload][ESPT]",
    "[ENET]^[IP][TCP][Payload]",
    "[ENET]^[IP][ESP][TCP][Payload][ESPT]",
    "[ENET]^[Payload]",
    "[ENET]^[IP][Payload]",
    "[ENET]^[IP][ESP][IV][Payload][ESPT]",
    "[ENET]^[IP][UDP][Payload]",
    "[ENET]^[IP][ESP][IV][UDP][Payload][ESPT]",
    "[ENET]^[IP][TCP][Payload]",
    "[ENET]^[IP][ESP][IV][TCP][Payload][ESPT]",
    "[ENET]^[Payload]",
};

void CPacket::DbgSetSpy()
{
    int iType = (int)((_wFlags & PKTF_TYPE_MASK) >> PKTF_TYPE_SHIFT);

    _pchSpy = _aszPktTypeName[iType + ((_wFlags & PKTF_CRYPT) ? 7 : 0)];

    memset(&_spy, 0, sizeof(CSpy));

    _spy._pEnetHdr = (CEnetHdr *)((BYTE *)_pv - sizeof(CEnetHdr));

    if ((_wFlags & PKTF_TYPE_MASK) != PKTF_TYPE_ENET)
    {
        _spy._pIpHdr = (CIpHdr *)_pv;

        if (_wFlags & PKTF_TYPE_ESP)
        {
            _spy._pEspHdr  = GetEspHdr();
            _spy._pEspTail = GetEspTail();
        }

        if (_wFlags & PKTF_TYPE_UDP)
        {
            _spy._pUdpHdr = GetUdpHdr();
        }

        if (_wFlags & PKTF_TYPE_TCP)
        {
            _spy._pTcpHdr = GetTcpHdr();
        }
    }
}

void CPacket::Validate()
{
    if (IsEsp())
    {
        CEspTail *  pEspTail = GetEspTail();
        UINT        cbPad    = pEspTail->_bPadLen;
        BYTE *      pbPad    = (BYTE *)pEspTail;

        // Verify that the padding is the series of bytes 1, 2, 3, ...

        while (cbPad > 0 && *--pbPad == (BYTE)cbPad)
            cbPad--;
    
        AssertSz1(cbPad == 0, "Packet padding has been overwritten (ppkt=%08lX)", this);
    }
}

#endif

const BYTE CPacket::_abPktTypeInfo[] =
{
    // [ENET]^[IP][Payload]

    0,                                                                      // [ESP]
    0,                                                                      // [UDP]/[TCP]
    sizeof(CIpHdr),                                                         // Size

    // [ENET]^[IP][ESP][Payload][ESPT]

    sizeof(CIpHdr),                                                         // [ESP]
    0,                                                                      // [UDP]/[TCP]
    sizeof(CIpHdr) + sizeof(CEspHdr),                                       // Size

    // [ENET]^[IP][UDP][Payload]

    0,                                                                      // [ESP]
    sizeof(CIpHdr),                                                         // [UDP]/[TCP]
    sizeof(CIpHdr) + sizeof(CUdpHdr),                                       // Size

    // [ENET]^[IP][ESP][UDP][Payload][ESPT]

    sizeof(CIpHdr),                                                         // [ESP]
    sizeof(CIpHdr) + sizeof(CEspHdr),                                       // [UDP]/[TCP]
    sizeof(CIpHdr) + sizeof(CEspHdr) + sizeof(CUdpHdr),                     // Size

    // [ENET]^[IP][TCP][Payload]

    0,                                                                      // [ESP]
    sizeof(CIpHdr),                                                         // [UDP]/[TCP]
    sizeof(CIpHdr) + sizeof(CTcpHdr),                                       // Size

    // [ENET]^[IP][ESP][TCP][Payload][ESPT]

    sizeof(CIpHdr),                                                         // [ESP]
    sizeof(CIpHdr) + sizeof(CEspHdr),                                       // [UDP]/[TCP]
    sizeof(CIpHdr) + sizeof(CEspHdr) + sizeof(CTcpHdr),                     // Size
    
    // [ENET]^[Payload]
    0,                                                                      // [ESP]
    0,                                                                      // [UDP]/[TCP]
    0                                                                       // Size

};

// ---------------------------------------------------------------------------------------
// CPacketQueue
// ---------------------------------------------------------------------------------------

void CPacketQueue::InsertHead(CPacket * ppkt)
{
    if ((ppkt->_ppktNext = _ppktHead) == NULL)
        _ppktTail = ppkt;
    _ppktHead = ppkt;
}

void CPacketQueue::InsertTail(CPacket * ppkt)
{
    if (_ppktTail)
        _ppktTail->_ppktNext = ppkt;
    else
        _ppktHead = ppkt;

    _ppktTail = ppkt;
    ppkt->_ppktNext = NULL;
}

void CPacketQueue::InsertHead(CPacketQueue * ppq)
{
    Assert(!ppq->IsEmpty());
    if (_ppktTail == NULL)
        _ppktTail = ppq->_ppktTail;
    ppq->_ppktTail->_ppktNext = _ppktHead;
    _ppktHead = ppq->_ppktHead;
    ppq->Init();
}

CPacket * CPacketQueue::RemoveHead()
{
    Assert(!IsEmpty());

    CPacket * ppkt = _ppktHead;

    if ((_ppktHead = ppkt->_ppktNext) == NULL)
        _ppktTail = NULL;
    else
        ppkt->_ppktNext = NULL;

    return(ppkt);
}

void CPacketQueue::Complete(CXnBase * pXnBase)
{
    while (!IsEmpty())
    {
        RemoveHead()->Complete(pXnBase);
    }
}

void CPacketQueue::Discard(CXnBase * pXnBase)
{
    while (!IsEmpty())
    {
        pXnBase->PacketFree(RemoveHead());
    }
}

void CPacketQueue::Dequeue(CPacket * ppktDequeue)
{
    CPacket **  pppkt    = &_ppktHead;
    CPacket *   ppktPrev = NULL;
    CPacket *   ppkt;

    for (; (ppkt = *pppkt) != NULL; ppktPrev = ppkt, pppkt = &ppkt->_ppktNext)
    {
        if (ppkt == ppktDequeue)
        {
            *pppkt = ppkt->_ppktNext;

            if (_ppktHead == NULL)
                _ppktTail = NULL;
            else if (_ppktTail == ppkt)
                _ppktTail = ppktPrev;
            return;
        }
    }

    AssertSz1(0, "CPacketQueue::Dequeue(%08lX) - Packet not found in queue", ppktDequeue);
}

UINT CPacketQueue::Count()
{
    CPacket * ppkt;
    UINT c;

    for (c = 0, ppkt = _ppktHead; ppkt; ppkt = ppkt->_ppktNext)
    {
        c += 1;
    }

    return(c);
}

// ---------------------------------------------------------------------------------------
// CIpAddr
// ---------------------------------------------------------------------------------------

CIpAddr CIpAddr::DefaultMask() const
{
    if (IsClassA()) return(IPADDR_CLASSA_NETMASK);
    if (IsClassB()) return(IPADDR_CLASSB_NETMASK);
    if (IsClassC()) return(IPADDR_CLASSC_NETMASK);
    if (IsClassD()) return(IPADDR_CLASSD_NETMASK);

    TraceSz1(Warning, "CIpAddr:DefaultMask(): Invalid host IP address: %s", Str());

    return(0);
}

BOOL CIpAddr::IsValidUnicast() const
{
    return(_dw != 0 && !IsSecure() && !IsBroadcast() && !IsMulticast() && !IsLoopback());
}

BOOL CIpAddr::IsValidAddr() const
{
    return(     ((BYTE)(_ab[0] - 1)) <  254     // 1 ... 254
            &&  _ab[1]               <= 254     // 0 ... 254
            &&  _ab[2]               <= 254     // 0 ... 254
            &&  ((BYTE)(_ab[3] - 1)) <  254);   // 1 ... 254
}

#if DBG || defined(XNET_FEATURE_ASSERT) || defined(XNET_FEATURE_TRACE)

char * CIpAddr::Str() const
{
    #define IPADDR_STR_BUFFS    32
    #define IPADDR_BUF_SIZE     16
    static char g_chBufIpAddr[IPADDR_STR_BUFFS * IPADDR_BUF_SIZE];
    static LONG g_lBufIndexIpAddr = 0;
    char * pch = &g_chBufIpAddr[(InterlockedIncrement(&g_lBufIndexIpAddr) % IPADDR_STR_BUFFS) * IPADDR_BUF_SIZE];
    XnInAddrToString(*(IN_ADDR *)&_dw, pch, IPADDR_BUF_SIZE);
    return(pch);
}

#endif

// ---------------------------------------------------------------------------------------
// HexStr
// ---------------------------------------------------------------------------------------

#if DBG || defined(XNET_FEATURE_ASSERT) || defined(XNET_FEATURE_TRACE)

char * HexStr(const void * pv, size_t cb)
{
    #define HEXSTR_BUFFS        8
    #define HEXSTR_CBMAX        128
    #define HEXSTR_BUF_SIZE     (HEXSTR_CBMAX * 2 + 1)
    static char g_chHexBuf[HEXSTR_BUFFS * HEXSTR_BUF_SIZE];
    static LONG g_lHexBufIndex = 0;
    char * pch = &g_chHexBuf[(InterlockedIncrement(&g_lHexBufIndex) % HEXSTR_BUFFS) * HEXSTR_BUF_SIZE];
    char * pchDst = pch;
    BYTE * pb = (BYTE *)pv;
    UINT   ui;

    if (cb > HEXSTR_CBMAX)
        cb = HEXSTR_CBMAX;

    for (; cb > 0; --cb, ++pb)
    {
        ui = (*pb) >> 4;
        *pchDst++ = ui < 10 ? '0' + ui : 'A' + (ui - 10);
        ui = (*pb) & 0x0F;
        *pchDst++ = ui < 10 ? '0' + ui : 'A' + (ui - 10);
    }

    *pchDst = 0;

    return(pch);
}

#endif

// ---------------------------------------------------------------------------------------
// XnInAddrToString
// ---------------------------------------------------------------------------------------

void XnInAddrToString(const IN_ADDR ina, char * pchBuf, INT cchBuf)
{
    char    ach[16];
    BYTE *  pbSrc = (BYTE *)&ina;
    BYTE *  pbEnd = pbSrc + 4;
    char *  pbDst = (cchBuf < sizeof(ach)) ? ach : pchBuf;
    UINT    b;

    while (pbSrc < pbEnd)
    {
        b = *pbSrc++;

        if (b >= 10)
        {
            if (b >= 100) { *pbDst++ = '0' + (b / 100); b %= 100; }
            *pbDst++ = '0' + (b / 10); b %= 10;
        }

        *pbDst++ = '0' + b;
        *pbDst++ = '.';
    }

    pbDst[-1] = 0;

    if (cchBuf < sizeof(ach) && cchBuf > 0)
    {
        memcpy(pchBuf, ach, cchBuf);
        pchBuf[cchBuf - 1] = 0;
    }
}

// ---------------------------------------------------------------------------------------
// XnAddrStr
// ---------------------------------------------------------------------------------------

#if DBG || defined(XNET_FEATURE_ASSERT) || defined(XNET_FEATURE_TRACE)

char * XnAddrStr(const XNADDR * pxnaddr)
{
    #define XNADDR_STR_BUFFS    4
    #define XNADDR_BUF_SIZE     128
    static char g_chXnAddrBuf[XNADDR_STR_BUFFS * XNADDR_BUF_SIZE];
    static LONG g_lXnAddrBufIndex = 0;
    char * pch = &g_chXnAddrBuf[(InterlockedIncrement(&g_lXnAddrBufIndex) % XNADDR_STR_BUFFS) * XNADDR_BUF_SIZE];

    memset(pch, 0, XNADDR_BUF_SIZE);

    _snprintf(pch, XNADDR_BUF_SIZE - strlen(pch) - 2, "{ %s ", ((CEnetAddr *)pxnaddr->abEnet)->Str());

    if (pxnaddr->ina.s_addr != 0)
    {
        _snprintf(&pch[strlen(pch)], XNADDR_BUF_SIZE - strlen(pch) - 2, "%s ",
                  CIpAddr(pxnaddr->ina.s_addr).Str());

        if (pxnaddr->inaOnline.s_addr != 0)
        {
            _snprintf(&pch[strlen(pch)], XNADDR_BUF_SIZE - strlen(pch) - 2, "NAT %s:%d SG %s ",
                      CIpAddr(pxnaddr->inaOnline.s_addr).Str(), NTOHS(pxnaddr->wPortOnline),
                      HexStr(pxnaddr->abOnline, sizeof(pxnaddr->abOnline)));
        }
    }

    _snprintf(&pch[strlen(pch)], XNADDR_BUF_SIZE - strlen(pch) - 2, "}");

    return(pch);
}

#endif

// ---------------------------------------------------------------------------------------
// CEnetAddr
// ---------------------------------------------------------------------------------------

#if DBG || defined(XNET_FEATURE_ASSERT) || defined(XNET_FEATURE_TRACE)

char * CEnetAddr::Str() const
{
    #define ENETADDR_STR_BUFFS    32
    #define ENETADDR_BUF_SIZE     18
    static char g_chBuf[ENETADDR_STR_BUFFS * ENETADDR_BUF_SIZE];
    static LONG g_lBufIndex = 0;
    char * pch = &g_chBuf[(InterlockedIncrement(&g_lBufIndex) % ENETADDR_STR_BUFFS) * ENETADDR_BUF_SIZE];
    _snprintf(pch, ENETADDR_BUF_SIZE, "%02X-%02X-%02X-%02X-%02X-%02X", _ab[0], _ab[1], _ab[2], _ab[3], _ab[4], _ab[5]);
    return(pch);
}

#endif

// ---------------------------------------------------------------------------------------
// CXnBase (Timer)
// ---------------------------------------------------------------------------------------

void CXnBase::TimerSet(CTimer * pt, DWORD dwTick)
{
    ICHECK(BASE, UDPC|SDPC);

    Assert(sizeof(PFNTIMER) == sizeof(void *));
    AssertSz(dwTick >= _dwTick, "Attempt to set timer in the past");

    TraceSz2(Timer, "TimerSet %08lX dwTick=%08lX", pt, dwTick);

    if (pt->_le.Flink != NULL)
    {
        TraceSz1(Timer, "TimerDequeue %08lX", pt);
        AssertListEntry(&_leTimers, &pt->_le);
        RemoveEntryList(&pt->_le);
        pt->_le.Flink = NULL;
    }

    AssertList(&_leTimers);

    pt->_dwTick = dwTick;

    if (dwTick < 0xFFFFFFFF)
    {
        if (IsListEmpty(&_leTimers) || dwTick <= ((CTimer *)_leTimers.Flink)->_dwTick)
        {
            TraceSz1(Timer, "TimerEnqueue %08lX (Head)", pt);
            InsertHeadList(&_leTimers, &pt->_le)
        }
        else
        {
            CTimer * ptPrev = (CTimer *)_leTimers.Blink;

            while (dwTick < ptPrev->_dwTick)
            {
                ptPrev = (CTimer *)ptPrev->_le.Blink;
                Assert(ptPrev != (CTimer *)&_leTimers);
            }

            pt->_le.Flink = ptPrev->_le.Flink;
            pt->_le.Blink = &ptPrev->_le;
            ptPrev->_le.Flink->Blink = &pt->_le;
            ptPrev->_le.Flink = &pt->_le;

            TraceSz2(Timer, "TimerEnqueue %08lX (after %08lX)", pt, ptPrev);
        }

        AssertListEntry(&_leTimers, &pt->_le);
    }
}

DWORD CXnBase::TimerSetRelative(CTimer * pt, DWORD dwTicksFromNow)
{
    ICHECK(BASE, UDPC|SDPC);

    DWORD dwTick = _dwTick + dwTicksFromNow;

    if (dwTick < pt->_dwTick)
    {
        TimerSet(pt, dwTick);
    }

    return(dwTick);
}

void CXnBase::TimerDpc(PKDPC pkdpc, void * pvContext, void * pvParam1, void * pvParam2)
{
    ((CXnBase *)pvContext)->TimerPush();
}

void CXnBase::TimerPush()
{
    ICHECK(BASE, SDPC);

    _dwTickKe = KeQueryTickCount();
    DWORD dwTick = ++_dwTick;

    while (1)
    {
        AssertList(&_leTimers);

        CTimer * pt = (CTimer *)_leTimers.Flink;

        if (pt == (CTimer *)&_leTimers)
            break;

        if (dwTick < pt->_dwTick)
            break;

        TraceSz2(Timer, "TimerPush %08lX dwTick=%08lX", pt, dwTick);

        RemoveEntryList(&pt->_le);

        pt->_le.Flink = NULL;
        pt->_dwTick = TIMER_INFINITE;

        (this->*(pt->_pfn))(pt);
    }

    SecRegProbe();

    NicTimer();
}

// ---------------------------------------------------------------------------------------
// CXnBase
// ---------------------------------------------------------------------------------------

NTSTATUS CXnBase::BaseInit(XNetInitParams * pxnip)
{
    TCHECK(USER);

    NTSTATUS status = HalInit(pxnip);
    if (!NT_SUCCESS(status))
        return(status);

    SetInitFlag(INITF_BASE);

    memcpy(&cfgSizeOfStruct, &s_XNetParamsDef.cfgSizeOfStruct, sizeof(XNetParams));

    if (pxnip->pxnp != NULL)
    {
        if (    pxnip->pxnp->cfgSizeOfStruct != sizeof(XNetStartupParams)
            &&  pxnip->pxnp->cfgSizeOfStruct != sizeof(XNetParams))
        {
            TraceSz(Warning, "Ignoring parameters because cfgSizeOfStruct is invalid");
        }
        else
        {
            BYTE * pbDef = (BYTE *)&s_XNetParamsDef.cfgSizeOfStruct;
            BYTE * pbMin = (BYTE *)&s_XNetParamsMin.cfgSizeOfStruct;
            BYTE * pbMax = (BYTE *)&s_XNetParamsMax.cfgSizeOfStruct;
            BYTE * pbSrc = (BYTE *)&pxnip->pxnp->cfgSizeOfStruct;
            BYTE * pbDst = (BYTE *)&cfgSizeOfStruct;
            int    c     = pxnip->pxnp->cfgSizeOfStruct;

            for (; --c >= 0; ++pbDef, ++pbMin, ++pbMax, ++pbSrc, ++pbDst)
            {
                if (*pbSrc == 0)
                    *pbDst = *pbDef;
                else if (*pbSrc < *pbMin)
                    *pbDst = *pbMin;
                else if (*pbSrc > *pbMax)
                    *pbDst = *pbMax;
                else
                    *pbDst = *pbSrc;
            }
        }
    }

#ifdef XNET_FEATURE_XBDM_SERVER
    cfgFlags |= XNET_STARTUP_BYPASS_SECURITY;
#endif

    RandInit(pxnip);

    InitializeListHead(&_leTimers);
    KeInitializeDpc(&_dpcTimer, TimerDpc, this);
    KeInitializeTimer(&_timerTimer);

    // Start the current timer tick at one day to allow timer routines to compute times
    // in the past and not have to worry about going negative.

    _dwTick = 24 * 60 * 60 * TICKS_PER_SECOND;
    _dwTickKe = KeQueryTickCount();

#ifdef XNET_FEATURE_ASSERT
    _leakinfo._pfnLeakTag = XnLeakTagToString;
#endif

    SetInitFlag(INITF_BASE_1);

    UINT        cbPool = cfgPrivatePoolSizeInPages * 4096;
    PoolEntry * ppe;
    UINT        n;

    ppe = (PoolEntry *)SysAlloc(cbPool, PTAG_Pool);

    if (ppe == NULL)
    {
        return(NETERR_MEMORY);
    }

    _pvPoolStart = ppe;
    _pvPoolEnd   = ((BYTE *)ppe + cbPool);

    SetInitFlag(INITF_BASE_2);

    // Initialize the free lists with a single big block

    for (n = 0; n <= MAX_SMALL_BLOCKS; n++)
    {
        InitializeListHead(&_aleFree[n]);
    }

    // NOTE: we reserved the very first and the very last
    // pool blocks. This saves us from a couple of extra checks
    // during alloc and free.

    MarkPoolEntryBusy(ppe);
    TagPoolEntry(ppe, PTAG_PoolStart);
    ppe->bOver = (1 << POOL_BLOCK_SHIFT);
    ppe->previousSize = 0;
    ppe->blockSize = 1;

    ppe = GetPoolEntryNext(ppe);
    n = (cbPool >> POOL_BLOCK_SHIFT) - 2;
    MarkPoolEntryFree(ppe);
    TagPoolEntry(ppe, 0);
    ppe->previousSize = 1;
    ppe->blockSize = (WORD)n;
    InsertFreePoolEntry(ppe);

    ppe = GetPoolEntryNext(ppe);
    MarkPoolEntryBusy(ppe);
    TagPoolEntry(ppe, PTAG_PoolEnd);
    ppe->bOver = (1 << POOL_BLOCK_SHIFT);
    ppe->previousSize = (WORD)n;
    ppe->blockSize = 1;

    return(NETERR_OK);
}

void CXnBase::BaseStart()
{
    ICHECK(BASE, USER);

    HalStart();

    Assert(!TestInitFlag(INITF_BASE_STOP));

    LARGE_INTEGER liDue;
    liDue.QuadPart = -10000000 / TICKS_PER_SECOND;
    KeSetTimerEx(&_timerTimer, liDue, 1000 / TICKS_PER_SECOND, &_dpcTimer);
}

void CXnBase::BaseStop()
{
    TCHECK(UDPC|SDPC);

    if (!TestInitFlag(INITF_BASE_STOP))
    {
        if (TestInitFlag(INITF_BASE_1))
        {
            KeCancelTimer(&_timerTimer);
            KeRemoveQueueDpc(&_dpcTimer);
        }

        SetInitFlag(INITF_BASE_STOP);
    }

    HalStop();
}

void CXnBase::BaseTerm()
{
    TCHECK(UDPC);

    BaseStop();

    Assert(IsListEmpty(&_leTimers));

    if (TestInitFlag(INITF_BASE_2))
    {
        SysFree(_pvPoolStart);
        _pvPoolStart = NULL;
        _pvPoolEnd = NULL;
    }

#ifdef XNET_FEATURE_ASSERT
    if (_leakinfo._cLeak > 0)
    {
        LeakTerm(&_leakinfo);
    }
#endif

    SetInitFlag(INITF_BASE_TERM);

    HalTerm();
}

// ---------------------------------------------------------------------------------------
// Assert
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_ASSERT

char * __cdecl DbgAssertFmt(char const * pszFmt, ...)
{
    #define ASSERT_ENTRY_SIZE   512
    #define ASSERT_ENTRY_COUNT  8
    static char s_achAssert[ASSERT_ENTRY_SIZE * ASSERT_ENTRY_COUNT];
    static LONG s_lAssert = 0;
    char * pchBuf = &s_achAssert[(InterlockedIncrement(&s_lAssert) % ASSERT_ENTRY_COUNT) * ASSERT_ENTRY_SIZE];
    pchBuf[0] = 0;
    va_list va;
    va_start(va, pszFmt);
    _vsnprintf(pchBuf, ASSERT_ENTRY_SIZE - 1, pszFmt, va);
    va_end(va);
    pchBuf[ASSERT_ENTRY_SIZE - 1] = 0;
    return(pchBuf);
}

#endif

// ---------------------------------------------------------------------------------------
// Trace
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_TRACE

void __cdecl DbgTrace(const char * pszTag, const char * pszFmt, ...)
{
    char ach[256];
    char * pchBuf = ach;
    int cchBuf = sizeof(ach);
    char * psz = (char *)pszFmt;
    va_list va;

    memset(ach, 0, sizeof(ach));

    if (*psz == '+')
        ++psz;
    else 
    {
        if (*psz == '\n')
        {
            *pchBuf++ = *psz++;
            cchBuf -= 1;
        }
#ifdef XNET_FEATURE_XBOX
        _snprintf(pchBuf, cchBuf - 2, "[" XNET_TRACE_PREFIX "] %s: ", pszTag);
#else
        _snprintf(pchBuf, cchBuf - 3, "[" XNET_TRACE_PREFIX ".%03X] %s: ", GetCurrentThreadId(), pszTag);
#endif
    }

    va_start(va, pszFmt);
    int cch = strlen(pchBuf);
    _vsnprintf(&pchBuf[cch], cchBuf - cch - 2, psz, va);
    va_end(va);

    cch = strlen(pchBuf);

    if (cch > 0 && *pszFmt && pszFmt[strlen(pszFmt) - 1] == '+')
        pchBuf[cch - 1] = 0;
    else
        strcpy(pchBuf + cch, "\n");

#ifdef XNET_FEATURE_XBDM_SERVER
    DbgPrintToKd(ach);
#else
    DbgPrint("%s", ach);
#endif
}

#endif

// ---------------------------------------------------------------------------------------
// DbgVerifyList
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_ASSERT

const char * DbgVerifyList(LIST_ENTRY * ple, LIST_ENTRY * pleRequire)
{
    LIST_ENTRY * plePrev;
    LIST_ENTRY * pleThis;
    BOOL fFound = FALSE;
    UINT cEnt = 0;

    if (ple->Flink == NULL || ple->Blink == NULL)
        return("List Flink is NULL");
    else if (ple->Blink == NULL)
        return("List Blink is NULL");

    if (ple->Flink == ple || ple->Blink == ple)
    {
        if (ple->Flink != ple->Blink)
        {
            return("List head corrupt");
        }

        return((!pleRequire) ? NULL : "Required entry not found");
    }

    plePrev = ple;
    pleThis = ple->Flink;

    while (1)
    {
        if (pleThis == NULL)
        {
            return("List entry has an Flink that points to NULL");
        }

        if (plePrev != pleThis->Blink)
        {
            return("List entry has a Blink that doesn't point to previous entry");
            return(FALSE);
        }

        if (pleThis == ple)
            break;

        if (pleThis == pleRequire)
        {
            if (fFound)
            {
                return("List has required entry twice.  Cycle detected.");
            }

            fFound = TRUE;
        }

        plePrev = pleThis;
        pleThis = plePrev->Flink;

        if (++cEnt > 100000)
        {
            return("List has cycle");
        }
    }

    return((!pleRequire || fFound) ? NULL : "Required entry not found");
}

// ---------------------------------------------------------------------------------------
// DataToString
// ---------------------------------------------------------------------------------------

#if defined(XNET_FEATURE_TRACE)  || defined(XNET_FEATURE_ASSERT)

char * DataToString(BYTE * pb, UINT cb)
{
    #define DTS_ENTRY_SIZE   32
    #define DTS_ENTRY_COUNT  8
    static char s_achDts[DTS_ENTRY_SIZE * DTS_ENTRY_COUNT];
    static LONG s_lDts = 0;
    char * pbBuf = &s_achDts[(InterlockedIncrement(&s_lDts) % DTS_ENTRY_COUNT) * DTS_ENTRY_SIZE];
    char * pbDst = pbBuf;

    if (cb > DTS_ENTRY_SIZE - 1)
        cb = DTS_ENTRY_SIZE - 1;

    for (; cb > 0; ++pbDst, ++pb, --cb)
    {
        if (*pb >= 31 && *pb < 127)
            *pbDst = *pb;
        else
            *pbDst = '.';
    }

    *pbDst = 0;

    return(pbBuf);
}

#endif

// ---------------------------------------------------------------------------------------
// Push/Pop Recv Tags
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_TRACE

void CXnBase::PushPktRecvTags(BOOL fBroadcast)
{
    ICHECK(BASE, UDPC|SDPC);

    Assert(_cbPushPop < sizeof(_abPushPop));

    _abPushPop[_cbPushPop++] = !!fBroadcast;

    if (fBroadcast && ++_cPushPopBroadcast == 1)
    {
        _pktRecvPushPop = Tag(pktRecv);
        _pktWarnPushPop = Tag(pktWarn);
        Tag(pktRecv)    = Tag(pktBroadcast);
        Tag(pktWarn)    = Tag(pktBroadcast);
    }
}

void CXnBase::PopPktRecvTags()
{
    ICHECK(BASE, UDPC|SDPC);

    Assert(_cbPushPop > 0);

    if (_abPushPop[--_cbPushPop])
    {
        Assert(_cPushPopBroadcast > 0);

        if (--_cPushPopBroadcast == 0)
        {
            Tag(pktRecv) = _pktRecvPushPop;
            Tag(pktWarn) = _pktWarnPushPop;
        }
    }
}

#endif

// ---------------------------------------------------------------------------------------

#endif
