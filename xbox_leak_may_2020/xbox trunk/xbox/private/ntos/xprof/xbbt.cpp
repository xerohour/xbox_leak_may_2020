/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xbbt.cpp

Abstract:

    BBT instrumented binary runtime support functions

--*/

#ifdef _XBOX_ENABLE_PROFILING

#include "ntos.h"
#include "xprofp.h"
#include "xbbt.h"

#ifdef _XBOX_ENABLE_BBT

IRTCLIENTINFO* IrtClientInfo;   // Client-init information
LONG IrtSweepingFlag;           // Whether we're currently sweep data to disk
KTIMER IrtTimer;                // Background timer
KDPC IrtTimerDpc;               //  and associated DPC

#define IsIrtClientInited() (IrtClientInfo != NULL)
#define IrtAssert(cond) do { if (!(cond)) KeBugCheckEx(0, __LINE__, 'IRTS', 0, 0); } while(0)


//
// Allocate space for an additional TLE structure
//
TLE* PtleCreate(IDFHDR* pidfhdr, TLH* ptlh, DWORD iInterval)
{
    DWORD ibtleNew = InterlockedExchangeAdd( (PLONG)&pidfhdr->cb, sizeof(TLE) );
   
    IrtAssert(ibtleNew + sizeof(TLE) <= pidfhdr->cbMax);

    // Fill in the TLE
    TLE* ptle = (TLE*) ((BYTE*) pidfhdr + ibtleNew);
    ptle->ibtleNext = ptlh->ibtleFirst;
    ptle->tob.iInterval = (iInterval & 0xFFFFFF80);

    // Link it into the list
    ptlh->ibtleFirst = ibtleNew;

    return ptle;
}


//
// Sweep TOB data
//
#define PtlhFromIfun(pidfhdr, ifun) \
        ((TLH*) ((BYTE*) pidfhdr + pidfhdr->ibrgtlhCode) + ifun)

#define COPYTOBBITS() { \
        ptlh = PtlhFromIfun(pidfhdr, i); \
        ptlh->qwTOBFanin++; \
        ibtle = ptlh->ibtleFirst; \
        if (ibtle != 0) { \
            ptle = (TLE*) ((BYTE*) pidfhdr + ibtle); \
            dwTimeSlot = iIntervalCur - ptle->tob.iInterval; \
            if (dwTimeSlot < cdwTob * cbitDw) { \
                ptle->tob.rgdwBit[dwTimeSlot>>5] |= 1 << (dwTimeSlot & 0x1F); \
                continue; \
            } \
        } \
        ptle = PtleCreate(pidfhdr, ptlh, iIntervalCur); \
        dwTimeSlot = iIntervalCur - ptle->tob.iInterval; \
        ptle->tob.rgdwBit[dwTimeSlot>>5] |= 1 << (dwTimeSlot & 0x1F); \
        }

VOID
IrtCopyCurrentTOBBits(IRTCLIENTINFO* pci)
{
    // Figure out the current interval index
    IDFHDR* pidfhdr = pci->pidfhdr;
    const IRTP* pirtp = pci->pirtp;
    DWORD iIntervalCur = pci->tobSweepTime / pirtp->cmsTimer;
    pci->tobSweepTime %= pirtp->cmsTimer;

    pci->intervalCount += iIntervalCur;
    pidfhdr->iIntervalCur = iIntervalCur = pci->intervalCount - 1;

    BYTE* rgTobBytes = pci->rgTobCounts;
    DWORD* rgCounts = pci->rgCounts;
    DWORDLONG* pqwDest = (DWORDLONG*) ((BYTE*) pidfhdr + pidfhdr->ibrgqwCount);
    DWORD i, n, ibtle, dwTimeSlot;
    TLH* ptlh;
    TLE* ptle;
    
    n = pidfhdr->ctlhCode;
    if (pidfhdr->tov == tovCTO) {
        // Make tobs from the counts

        // for each TOB
        for (i=0; i < n; i++) {
            // tob bytes are intialized to 1 and set to 0 when hit.
            if (rgCounts[i]) {
                // this byte was hit in the last interval - reset it for the next interval
                // Save counts too!
                pqwDest[i] += rgCounts[i];
                rgCounts[i] = 0;

                COPYTOBBITS();
            }
        }
    } else {
        IrtAssert((pidfhdr->tov & tov4Mask) == 0);

        // for each TOB
        for (i=0; i < n; i++) {
            // tob bytes are intialized to 1 and set to 0 when hit.
            if (rgTobBytes[i] == 0) {
                //this byte was hit in the last interval - reset it for the next interval
                rgTobBytes[i] = 1;

                COPYTOBBITS();
            }
        }
    }
}


//
// Sweep static edge counts
//
VOID IrtCopyCounts(IRTCLIENTINFO* pci)
{
    DWORD* srcCounts = pci->rgCounts;
    IDFHDR* pidfhdr = pci->pidfhdr;
    DWORDLONG* dstCounts = (DWORDLONG*) ((BYTE*) pidfhdr + pidfhdr->ibrgqwCount);
    DWORD i, n;

    n = pidfhdr->cqwCount;
    for (i=0; i < n; i++) {
        *dstCounts++ += *srcCounts;
        *srcCounts++ = 0;
    }

    if (pci->rgSeqNums) {
        n = pidfhdr->ctlhCode;
        for (i=0; i < n; i++) {
            TLH* ptlh = PtlhFromIfun(pidfhdr, i);
            ptlh->iSequenceInit = pci->rgSeqNums[i+1];
        }

        pidfhdr->iSequence = pci->rgSeqNums[0];
    }

    pci->copyCountsTime = 0;
}


VOID
IrtTimerProc(
    PKDPC dpc,
    VOID* context,
    VOID* param1,
    VOID* param2
    )

/*++

Routine Description:

    IRT runtime background sweeping timer procedure

Arguments:

    See DDK documentation

Return Value:

    NONE

Notes:

    This function runs at dispatch level.

--*/

{
    IRTCLIENTINFO* pci = IrtClientInfo;
    IrtAssert(IsIrtClientInited());

    // Check if we need to do a sweep yet. Note that if we're currently
    // in the process of writing IDF data to disk, then skip this round.
    pci->currentTime += IRT_SWEEP_INTERVAL;
    pci->tobSweepTime += IRT_SWEEP_INTERVAL;
    pci->copyCountsTime += IRT_SWEEP_INTERVAL;

    if (IrtSweepingFlag) return;

    if (pci->tobSweepTime >= pci->pirtp->cmsTimer) {
        IrtCopyCurrentTOBBits(pci);
    }

    // Call IrtCopyCounts once every 4 seconds
    if (pci->copyCountsTime >= 4000) {
        IrtCopyCounts(pci);
    }
}


//
// Initialize IDF header information
//
VOID IrtInitializeIdfHeader(IRTCLIENTINFO* pci)
{
    DWORD cb = CbRoundToPage(sizeof(*pci));
    IDFHDR* pidfhdr = (IDFHDR*) ((BYTE*) pci + cb);
    pci->pidfhdr = pidfhdr;

    DWORD cbMax = IRT_BUFFER_SIZE - cb;
    memset(pidfhdr, 0, cbMax);

    const IRTP* pirtp = pci->pirtp;
    pidfhdr->dwSignature = dwIdfSignature;
    pidfhdr->dwVersion = dwIdfVerCurrent;
    pidfhdr->dwId = pirtp->dwId;
    pidfhdr->dwGeneration = pirtp->dwGeneration;
    pidfhdr->tov = pirtp->tov;
    pidfhdr->fSweepable = pirtp->fSweepable ? 1 : 0;

    // Calculate the minimum size for the shared memory region

    cb = CbRound(sizeof(IDFHDR), sizeof(DWORDLONG));
    if (pirtp->szIdfKeyPath) {
        pidfhdr->ibszIdfKeyPath = cb;
        cb += strlen(pirtp->szIdfKeyPath) + 1;
    }
    cb = CbRound(cb, sizeof(DWORDLONG));

    pidfhdr->cqwCount = pirtp->cbrc;
    pidfhdr->ibrgqwCount = cb;
    cb += pidfhdr->cqwCount * sizeof(DWORDLONG);

    // Save space for indirect branches
    pidfhdr->ciblh = pirtp->cibd ? pirtp->cibd : pirtp->cibs;

    pidfhdr->ibrgiblh = cb;
    cb += pidfhdr->ciblh * sizeof(IBLH);
    cb = CbRound(cb, sizeof(DWORDLONG));

    IrtAssert(pirtp->cmsTimer != 0);

    if (pirtp->cfun != 0) {
        pidfhdr->ctlhCode = pirtp->cfun;
        pidfhdr->ibrgtlhCode = cb;
        cb += pidfhdr->ctlhCode * sizeof(TLH);
        cb = CbRound(cb, sizeof(DWORDLONG));
    }

    if (pirtp->crsc != 0) {
        pidfhdr->ctlhResource = pirtp->crsc;
        pidfhdr->ibrgtlhResource = cb;
        cb += pidfhdr->ctlhResource * sizeof(TLH);
        cb = CbRound(cb, sizeof(DWORDLONG));
    }

    pidfhdr->cb = cb;

    // Calculate the minimum size for the shared memory region

    DWORD cbMin = cb + sizeof(THD);
    cbMin += pidfhdr->ciblh * sizeof(IBLE);

    if (pirtp->cfun != 0)
        cbMin += pidfhdr->ctlhCode * sizeof(TLE);

    if (pirtp->crsc != 0)
        cbMin += pidfhdr->ctlhResource * sizeof(TLE);

    IrtAssert(cbMin <= cbMax);

    pidfhdr->cbMax = cbMax;
    pidfhdr->cbAllocated = cbMax;
    pidfhdr->cmsTimer = pirtp->cmsTimer;
    pidfhdr->cpsActive = 1;
    pidfhdr->ifunEndBoot = pirtp->ifunEndBoot;
    pidfhdr->iSequenceEndBoot = (DWORD) -1;
    pidfhdr->iIntervalEndBoot = (DWORD) -1;
    pidfhdr->fFastRT = (pci->rgCounts != NULL);
    pidfhdr->iIntervalStart = pci->intervalCount;
    pidfhdr->cmsStartTime = pci->currentTime;
    KeQuerySystemTime((LARGE_INTEGER*) &pidfhdr->qwStartTime);

    if (pirtp->szIdfKeyPath) {
        strcpy((char*) pidfhdr + pidfhdr->ibszIdfKeyPath, pirtp->szIdfKeyPath);
    }
}


DWORD IrtClientNewIndFast(const IRTP* pirtp, DWORD rvaDest, long iIndSrc)
{
   if (IrtSweepingFlag) return 0;

   IRTCLIENTINFO* pci = IrtClientInfo;
   IDFHDR* pidfhdr = pci->pidfhdr;
   IBLH* rgiblh = (IBLH*) ((BYTE*) pidfhdr + pidfhdr->ibrgiblh);
   IBLH* piblh = rgiblh + iIndSrc;
   DWORD* pibible = &piblh->ibibleFirst;

   // Allocate a new ible
   DWORD ibible = InterlockedExchangeAdd((PLONG)&pidfhdr->cb, sizeof(IBLE));
   IrtAssert(ibible + sizeof(IBLE) <= pidfhdr->cbMax);

   IBLE *pible = (IBLE *) ((BYTE *) pidfhdr + ibible);

   // Fill in the IBLE

   pible->blkidSrc = rvaDest;
   pible->qwCount = 0;

   // Atomically add to the front of the list
   DWORD ibibleFirst;
   do
   {
      ibibleFirst = piblh->ibibleFirst;
      pible->ibibleNext = ibibleFirst;
   }
   while (ibibleFirst != InterlockedCompareExchange((PLONG)&piblh->ibibleFirst, ibible, ibibleFirst ));
   
   InterlockedIncrement((PLONG)&pidfhdr->cible);

   return ibible;
}


//
// This function is called when a BBT instrumented binary is run.
// We assume this function is only called once.
//
extern "C" VOID
IrtClientInitFast(
    const IRTP* pirtp,
    DWORD* rgCounts,
    BYTE* rgTobCounts,
    FASTRTPARAM* pfrtp,
    DWORD* rgSeqNums,
    FASTINDDATA* pfid,
    VOID* unused
    )
{
    if (IsIrtClientInited()) return;

    // Allocate the necessary memory buffers
    IrtSweepingFlag = 0;

    IrtClientInfo = (IRTCLIENTINFO*) MmAllocateSystemMemory(IRT_BUFFER_SIZE, PAGE_READWRITE);
    IrtAssert(IrtClientInfo != NULL);

    // Remember the client init parameters
    memset(IrtClientInfo, 0, sizeof(IRTCLIENTINFO));
    IrtClientInfo->pirtp = pirtp;
    IrtClientInfo->rgCounts = rgCounts;
    IrtClientInfo->rgTobCounts = rgTobCounts;
    IrtClientInfo->rgSeqNums = rgSeqNums;

    // Initialize IDF header information
    IrtInitializeIdfHeader(IrtClientInfo);

    if (pfid) {
       IBLH* rgiblh = (IBLH*) ((BYTE*) IrtClientInfo->pidfhdr + IrtClientInfo->pidfhdr->ibrgiblh);
       pfid->rgiblh = (DWORD_PTR) rgiblh;
       pfid->negaddrbase = -(LONGLONG) pirtp->pvImageBase;
       pfid->pidfhdr = (DWORD_PTR) IrtClientInfo->pidfhdr;
       pfid->pfnInd = (DWORD_PTR) IrtClientNewIndFast;
    }

    // Start the background timer
    LARGE_INTEGER dueTime;
    dueTime.QuadPart = -10000*IRT_SWEEP_INTERVAL;

    KeInitializeDpc(&IrtTimerDpc, IrtTimerProc, NULL);
    KeInitializeTimer(&IrtTimer);
    KeSetTimerEx(&IrtTimer, dueTime, IRT_SWEEP_INTERVAL, &IrtTimerDpc);
}


//
// Sweep the BBT data into a disk file
//
NTSTATUS IrtSweep(HANDLE file)
{
    NTSTATUS status = STATUS_INVALID_PARAMETER;

    //
    // Make sure the client has been initialized
    // and we're not sweeping at the moment
    //
    if (!IsIrtClientInited() ||
        InterlockedCompareExchange(&IrtSweepingFlag, 1, 0) != 0) {
        status = STATUS_INVALID_PARAMETER;
    } else {
        // Sweep static edge counts
        IrtCopyCounts(IrtClientInfo);

        // Write the in-memory IDF data out to the disk

        IO_STATUS_BLOCK iostatusBlock;
        FILE_END_OF_FILE_INFORMATION endOfFile;
        FILE_ALLOCATION_INFORMATION allocation;
        IDFHDR* pidfhdr = IrtClientInfo->pidfhdr;
        DWORD count = pidfhdr->cb;

        status = NtWriteFile(
                   file,
                   NULL,
                   NULL,
                   NULL,
                   &iostatusBlock,
                   pidfhdr,
                   (count + 511) & ~511,
                   NULL);

        // Set file size

        if (NT_SUCCESS(status)) {
            endOfFile.EndOfFile.QuadPart = count;
            status = NtSetInformationFile(
                        file,
                        &iostatusBlock,
                        &endOfFile,
                        sizeof(endOfFile),
                        FileEndOfFileInformation);
        }

        if (NT_SUCCESS(status)) {
            allocation.AllocationSize.QuadPart = count;
            status = NtSetInformationFile(
                        file,
                        &iostatusBlock,
                        &allocation,
                        sizeof(allocation),
                        FileAllocationInformation);
        }

        // Clean up the in-memory data
        IrtInitializeIdfHeader(IrtClientInfo);

        IrtSweepingFlag = 0;
    }

    return status;
}

#else // !_XBOX_ENABLE_BBT

//
// When BBTBUILD is not enabled, just stub out these two functions
//
extern "C" VOID
IrtClientInitFast(
    const IRTP* pirtp,
    DWORD* rgCounts,
    BYTE* rgTobCounts,
    FASTRTPARAM* pfrtp,
    DWORD* rgSeqNums,
    VOID* unused
    ) {}

NTSTATUS IrtSweep(HANDLE file) { return STATUS_NOT_IMPLEMENTED; }

#endif // !_XBOX_ENABLE_BBT

#endif // _XBOX_ENABLE_PROFILING

