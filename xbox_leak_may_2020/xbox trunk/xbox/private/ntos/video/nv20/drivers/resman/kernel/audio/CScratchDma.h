 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/************************Object to do DSP memory management*****************\
*                                                                           *
* Module: CScratchDma.h		                                                *
*   Object to handle pages/offsets etc for the scratch memory		        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CSCRATCHDMA_
#define _CSCRATCHDMA_

typedef struct _pageArray
{
    VOID *  pLin;
    U032    uPhys;
    U032    uBytesAvailBeg;     // bytes available on the beginning of the page
    U032    uBytesAvailEnd;     // bytes availbale at the end of the page
} PAGE_ARRAY, *PPAGE_ARRAY;

class CHalBase;

class CScratchDma
{
public:
    CScratchDma(CHalBase *pHal, U032 uRegOffset);
    ~CScratchDma();

public:
    RM_STATUS Initialize(MEM_DESC *pScratchTable);
    
    RM_STATUS Allocate(U032 uBytesReguested, U032 *pLinOffset);
    VOID Free(U032 uLinOffset, U032 uSize);

    VOID Write(U032 uLinOffset, VOID *pSource, U032 uSize);
    VOID Read(VOID *pDest, U032 uLinOffset, U032 uSize);

    VOID Put(U032 uLinOffset, U032 uVal);
    VOID Put(U032 uLinOffset, U032 uSize, U032 uVal);
    U032 Get(U032 uLinOffset);

    VOID *GetScratchTableLinAddr() { return m_ScratchTable.pLinAddr; }

    // only used to for AC-3 buffer sharing between GP & EP - use allocate
    // for all other purposes
    RM_STATUS AddPage(PMEM_DESC pMem, U032 uNumOfPages, U032 *pOffset);
    VOID RemovePage(PMEM_DESC pMem, U032 uNumOfPages);

    // debug only!
    VOID *GetPage(U032 uIndex) { return m_pPageTable[uIndex].pLin; }

    // these functions are to be used very very carefully
    // the address returned from here are not contiguous 
    // across the next page
    VOID *GetLinAddr(U032 uLinOffset);
    VOID *GetPhysAddr(U032 uLinOffset);

private:

    VOID RemovePage(U032 uIndex);
    VOID RemoveFromPageTable(U032 uIndex);

    U032 AddPage(U032 *pIndex, U032 uNum = 1);
    VOID AddToPageTable(U032 uIndex, VOID *pLinAddr, U032 uPhysAddr);

    U032 GetContiguousFreeIndex(U032 uNum);
    U032 GetPageIndex(VOID *pLin);

    U032 GetPageNumber(U032 uLinOffset) { return PAGENUM(uLinOffset); }
    U032 GetPageOffset(U032 uLinOffset) { return (uLinOffset & 0xFFF); }

    PAGE_ARRAY  *m_pPageTable;
    
    U016        m_uMaxPages;
    U016        m_uValidPages;
    MEM_DESC    m_ScratchTable;
    MEM_DESC    m_DummyPage;

    CHalBase    *m_pHal;
    U032        m_uRegOffsetValidPages;
};

#endif
