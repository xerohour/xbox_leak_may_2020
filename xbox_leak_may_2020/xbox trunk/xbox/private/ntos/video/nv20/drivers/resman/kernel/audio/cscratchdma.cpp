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
* Module: CScratchMemory.cpp		                                            *
*   Object to handle pages/offsets etc for the scratch memory		        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
extern "C"
{
#include <nvarm.h>
#include <aos.h>
#include <AudioHw.h>
#include <nv_papu.h>
};

#include "CHalBase.h"
#include "CScratchDma.h"

#define INVALID_INDEX_VALUE         (~0)

CScratchDma::CScratchDma(CHalBase *pHal, U032 uRegValidPages)
{
    m_uValidPages = 0;
    m_uMaxPages = 0;
    m_pPageTable = NULL;
    m_pHal = pHal;
    m_uRegOffsetValidPages = uRegValidPages;
}

CScratchDma::~CScratchDma()
{
    if (m_pPageTable)
    {
        // free all the memory
        for (U032 uCnt = 0; uCnt < m_uValidPages; uCnt++)
            aosFreeMem(m_pPageTable[uCnt].pLin);

        aosFreeMem(m_pPageTable);
        aosFreeMem(m_DummyPage.pLinAddr);
        m_pPageTable = NULL;
    }
}

RM_STATUS
CScratchDma::Initialize(MEM_DESC *pScratchTable)
{
	m_uMaxPages = (U016) (pScratchTable->Size / NV_PSGE_SIZE);
    aosMemCopy((VOID *)&m_ScratchTable, (VOID *)pScratchTable, sizeof(MEM_DESC));

    // allocate memory for the page table
    RM_STATUS rmStatus = aosAllocateMem(m_uMaxPages * sizeof(PAGE_ARRAY),
                                        ALIGN_DONT_CARE,
                                        (VOID **)&m_pPageTable,
                                        NULL);
    
    // allocate a dummy page, and point all unused PRDs to it
    m_DummyPage.Size = PAGE_SIZE;
    rmStatus = aosAllocateMem(m_DummyPage.Size,
                            ALIGN_4K,
                            &m_DummyPage.pLinAddr,
                            &m_DummyPage.pPhysAddr);
    
    aosZeroMemory(m_DummyPage.pLinAddr, m_DummyPage.Size);

    // make all the PRDs point to the dummy page...
    MCP1_PRD *pPrd = (MCP1_PRD *)m_ScratchTable.pLinAddr;

    for (U032 uCnt = 0; uCnt < m_uMaxPages; uCnt++)
    {
        pPrd[uCnt].uAddr = (U032)m_DummyPage.pPhysAddr;
        pPrd[uCnt].Control.uValue = 0;
        pPrd[uCnt].Control.Owner = NV_PSSL_PRD_CONTROL_OWN_HW;
    }

    if (rmStatus == RM_OK)
        aosZeroMemory(m_pPageTable, m_uMaxPages * sizeof(PAGE_ARRAY));
    
    m_pHal->RegWrite(m_uRegOffsetValidPages, m_uValidPages);

    return rmStatus;
}

RM_STATUS 
CScratchDma::Allocate(U032 uBytesReguested, U032 *pLinOffset)
{
    
    U032 uBytesAvail;

    // can the requirement be satisfied by the exisiting pages?
    for (U016 uCnt = 0; uCnt < m_uValidPages - 1; uCnt++)
    {
        uBytesAvail = 0;

        if (m_pPageTable[uCnt].pLin)
            uBytesAvail += m_pPageTable[uCnt].uBytesAvailEnd;
            
        if (m_pPageTable[uCnt + 1].pLin)
            uBytesAvail += m_pPageTable[uCnt].uBytesAvailBeg;

        if (uBytesReguested <= uBytesAvail)
        {
            if (m_pPageTable[uCnt].uBytesAvailEnd)
            {
                *pLinOffset = (PAGE_SIZE * uCnt) + m_pPageTable[uCnt].uBytesAvailEnd;
                if (m_pPageTable[uCnt].uBytesAvailEnd < uBytesReguested)
                {
                    m_pPageTable[uCnt + 1].uBytesAvailBeg = uBytesReguested - m_pPageTable[uCnt].uBytesAvailEnd;
                    m_pPageTable[uCnt].uBytesAvailEnd = 0;
                }
                else
                    m_pPageTable[uCnt].uBytesAvailEnd -= uBytesReguested;
            }
            else
            {
                *pLinOffset = PAGE_SIZE * (uCnt+1);
                m_pPageTable[uCnt + 1].uBytesAvailBeg -= uBytesReguested;
            }

            break;
        }
    }

    // did we satisfy the requirement
    if (uCnt < m_uValidPages - 1)
        return RM_OK;

    // elaborate the logic later to include end avail on pages other
    // than the last.. etc etc etc

    U016 uPagesNeeded = (U016)(((uBytesReguested+PAGE_SIZE-1)/PAGE_SIZE));
    U032 uPageIndex;

    RM_STATUS rmStatus = AddPage(&uPageIndex, uPagesNeeded);
    if (rmStatus != RM_OK)
        return rmStatus;
    
    m_uValidPages = uPagesNeeded;

    if (uPageIndex)
    {
        // get the 411 on the previous page and update it's offset
        *pLinOffset = (PAGE_SIZE * (uPageIndex-1) ) + m_pPageTable[uPageIndex-1].uBytesAvailEnd;
        uBytesReguested -= m_pPageTable[uPageIndex-1].uBytesAvailEnd;
        m_pPageTable[uPageIndex-1].uBytesAvailEnd = 0;
    }
    else
        *pLinOffset = PAGE_SIZE * uPageIndex;

    // spin thru' the pages updating the offsets on each page
    while (uBytesReguested)
    {
        m_pPageTable[uPageIndex].uBytesAvailBeg = 0;
        m_pPageTable[uPageIndex].uBytesAvailEnd = (PAGE_SIZE < uBytesReguested)?0:(PAGE_SIZE - uBytesReguested);
        uBytesReguested -= (PAGE_SIZE - m_pPageTable[uPageIndex].uBytesAvailEnd);
        uPageIndex++;
    }

    return rmStatus;
}

VOID
CScratchDma::Free(U032 uOffset, U032 uSize)
{
    U032 uPageIndex = GetPageNumber(uOffset);
    U032 uPageOffset = GetPageOffset(uOffset);
    
    if (uPageOffset)
    {
        m_pPageTable[uPageIndex].uBytesAvailEnd = PAGE_SIZE - uPageOffset;
        uSize -= m_pPageTable[uPageIndex].uBytesAvailEnd;
        uPageIndex++;
    }

    // how many pages are spanned?
    while(uSize)
    {
        if (PAGE_SIZE <= m_pPageTable[uPageIndex].uBytesAvailEnd + uSize)
        {   
            uSize -= (PAGE_SIZE - m_pPageTable[uPageIndex].uBytesAvailEnd);
            RemovePage(uPageIndex);
        }
        else
        {
            // this is the last page
            m_pPageTable[uPageIndex].uBytesAvailBeg = uSize;
            uSize = 0;
            if ( (m_pPageTable[uPageIndex].uBytesAvailBeg + m_pPageTable[uPageIndex].uBytesAvailBeg) == PAGE_SIZE)
                RemovePage(uPageIndex);
        }
    }
}

VOID
CScratchDma::RemovePage(U032 uIndex)
{
    aosFreeMem(m_pPageTable[uIndex].pLin);
    RemoveFromPageTable(uIndex);
}

VOID
CScratchDma::RemoveFromPageTable(U032 uIndex)
{
    m_pPageTable[uIndex].pLin = NULL;
    m_pPageTable[uIndex].uPhys = 0;

    MCP1_PRD *pPrd = (MCP1_PRD *)m_ScratchTable.pLinAddr;
    pPrd[uIndex].uAddr = (U032)m_DummyPage.pPhysAddr;
    pPrd[uIndex].Control.uValue = 0;
    pPrd[uIndex].Control.Owner = NV_PSSL_PRD_CONTROL_OWN_NOT_HW;

    // do we need to update max valid pages?
    if (uIndex == (U032)(m_uValidPages - 1) )
    {
        // last page
        m_uValidPages--;
        m_pHal->RegWrite(m_uRegOffsetValidPages, m_uValidPages);
    }

    return;
}

U032
CScratchDma::GetContiguousFreeIndex(U032 uNum)
{
    bool bFlag;

    for (U032 uCnt = 0; uCnt < m_uMaxPages; uCnt++)
    {
        if (!m_pPageTable[uCnt].pLin)
        {
            bFlag = TRUE;

            // make sure we have contiguous indices free
            for (U032 uCnt1 = uCnt + 1; uCnt1 < (uCnt + uNum); uCnt1++)
            {
                if (m_pPageTable[uCnt].pLin)
                {
                    bFlag = FALSE;
                    break;
                }
            }

            if (bFlag == TRUE)
                return uCnt;
        }
    }

    return INVALID_INDEX_VALUE;
}

VOID
CScratchDma::AddToPageTable(U032 uIndex, VOID *pLinAddr, U032 uPhysAddr)
{
    m_pPageTable[uIndex].pLin = pLinAddr;
    m_pPageTable[uIndex].uPhys = uPhysAddr;

    // assume the pages are fully utilized
    m_pPageTable[uIndex].uBytesAvailBeg = 0;
    m_pPageTable[uIndex].uBytesAvailEnd = 0;
    
    // update the PRDs
    MCP1_PRD *pPrd = (MCP1_PRD *)m_ScratchTable.pLinAddr;
    pPrd[uIndex].uAddr = uPhysAddr;
    pPrd[uIndex].Control.uValue = 0;
    pPrd[uIndex].Control.Owner = NV_PSSL_PRD_CONTROL_OWN_HW;
    
    if (uIndex > (U032)m_uValidPages)
    {
        m_uValidPages = (U016)uIndex;
        m_pHal->RegWrite(m_uRegOffsetValidPages, m_uValidPages);
    }
}

RM_STATUS
CScratchDma::AddPage(U032 *pIndex, U032 uNum)
{
    RM_STATUS rmStatus = RM_ERROR;
    U032 uPagesAdded = 0;
    
    VOID *pLin;
    U032 uPhys;
    
    U032 uIndex = GetContiguousFreeIndex(uNum);

    if (uIndex == INVALID_INDEX_VALUE)
        return RM_ERROR;
    
    *pIndex = uIndex;

    while (uPagesAdded < uNum)
    {
        rmStatus =  aosAllocateMem( PAGE_SIZE,
						            ALIGN_4K,
						            &pLin,
						            (VOID **)&uPhys);

        if (rmStatus != RM_OK)
            break;

        AddToPageTable(uIndex, pLin, uPhys);

        uPagesAdded++;
        uIndex++;
    }
    
    return rmStatus;
}

RM_STATUS
CScratchDma::AddPage(PMEM_DESC pMem, U032 uNumOfPages, U032 *pOffset)
{
    RM_STATUS rmStatus = RM_OK;

    U032 uIndex = GetContiguousFreeIndex(uNumOfPages);
    if (uIndex == INVALID_INDEX_VALUE)
        return RM_ERROR;
    
    *pOffset = PAGE_SIZE * uIndex;

    // memory already allocated...
    for (U032 uCnt = 0; uCnt < uNumOfPages; uCnt++)
    {
        AddToPageTable(uIndex, pMem[uCnt].pLinAddr, (U032)pMem[uCnt].pPhysAddr);
        uIndex++;
    }

    return RM_OK;
}

U032
CScratchDma::GetPageIndex(VOID *pLin)
{
    for (U032 uCnt = 0; uCnt < m_uValidPages; uCnt++)
    {
        if (m_pPageTable[uCnt].pLin == pLin)
            return uCnt;
    }

    return INVALID_INDEX_VALUE;
}

VOID
CScratchDma::RemovePage(PMEM_DESC pMem, U032 uNumOfPages)
{
    U032 uIndex;

    for (U032 uCnt = 0; uCnt < uNumOfPages; uCnt++)
    {
        uIndex = GetPageIndex(pMem[uCnt].pLinAddr);
        if (uIndex != INVALID_INDEX_VALUE)
            RemoveFromPageTable(uIndex);
    }
}


VOID
CScratchDma::Write(U032 uLinOffset, VOID *pSource, U032 uSize)
{
    while(uSize)
    {
        U032 uDestSize = PAGE_SIZE - GetPageOffset(uLinOffset);
        U032 uCopySize = MIN(uDestSize, uSize);

#if defined DEBUG
        if (GetPageNumber(uLinOffset) > m_uValidPages)
        {
            aosDbgPrintString(DEBUGLEVEL_ERRORS, "CScratchDma::Write - incorrect offset\n");
            aosDbgBreakPoint();
        }
#endif
        // destination ptr
        VOID *pDest = GetLinAddr(uLinOffset);
        aosMemCopy(pDest, pSource, uCopySize);

        uSize -= uCopySize;
        pSource = (VOID *)((U032)pSource + uCopySize);
        uLinOffset += uCopySize;
    }
}

VOID
CScratchDma::Read(VOID *pDest, U032 uLinOffset, U032 uSize)
{
    while (uSize)
    {
        U032 uSrcSize = PAGE_SIZE - GetPageOffset(uLinOffset);
        U032 uCopySize = MIN(uSrcSize, uSize);

#if defined DEBUG
        if (GetPageNumber(uLinOffset) > m_uValidPages)
        {
            aosDbgPrintString(DEBUGLEVEL_ERRORS, "CScratchDma::Read - incorrect offset\n");
            aosDbgBreakPoint();
        }
#endif

        VOID *pSrc = GetLinAddr(uLinOffset);
        aosMemCopy(pDest, pSrc, uCopySize);

        uSize -= uCopySize;
        pDest = (VOID *)((U032)pDest + uCopySize);
        uLinOffset += uCopySize;
    }
}

U032
CScratchDma::Get(U032 uLinOffset)
{
#if defined DEBUG
        if (GetPageNumber(uLinOffset) > m_uValidPages)
        {
            aosDbgPrintString(DEBUGLEVEL_ERRORS, "CScratchDma::Get - incorrect offset\n");
            aosDbgBreakPoint();
        }
#endif
    return *( (U032 *) GetLinAddr(uLinOffset) );
}

VOID
CScratchDma::Put(U032 uLinOffset, U032 uSize, U032 uVal)
{
    for (U032 uCnt = 0; uCnt < uSize; uCnt++)
        Put(uLinOffset + uCnt, uVal);
}

VOID
CScratchDma::Put(U032 uLinOffset, U032 uData)
{
#if defined DEBUG
        if (GetPageNumber(uLinOffset) > m_uValidPages)
        {
            aosDbgPrintString(DEBUGLEVEL_ERRORS, "CScratchDma::Put - incorrect offset\n");
            aosDbgBreakPoint();
        }
#endif
    *( (U032 *) GetLinAddr(uLinOffset) ) = uData;
}

VOID *
CScratchDma::GetLinAddr(U032 uLinOffset)
{
    return (VOID *)((U032)m_pPageTable[GetPageNumber(uLinOffset)].pLin + GetPageOffset(uLinOffset));
}

VOID *
CScratchDma::GetPhysAddr(U032 uLinOffset)
{
    return (VOID *)(m_pPageTable[GetPageNumber(uLinOffset)].uPhys + GetPageOffset(uLinOffset));
}