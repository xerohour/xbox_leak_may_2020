/************************Object to do DSP memory management*****************\
*                                                                           *
* History:                                                                  *
* 12/21/2000 georgioc(MSFT) - redid page table management, integrated with 
* xbox apu driver
*                                                                           *
\***************************************************************************/

#include "dsoundi.h"
#include "dspdma.h"

#define INVALID_INDEX_VALUE         (~0)


CMcpxDspScratchDma::CMcpxDspScratchDma(BOOL fGpScratch)
{
    m_dwMaxPages = 0;
    m_fGpScratch = fGpScratch;
    memset(&m_ctxFxScratch,0,sizeof(m_ctxFxScratch));

}

CMcpxDspScratchDma::~CMcpxDspScratchDma()
{
    if (m_ctxFxScratch.VirtualAddress && m_ctxFxScratch.Size) {

        MmLockUnlockBufferPages(m_ctxFxScratch.VirtualAddress,m_ctxFxScratch.Size,TRUE);
        POOLFREE(m_ctxFxScratch.VirtualAddress);

    }
}

VOID
CMcpxDspScratchDma::Initialize(DWORD dwReservedPages)
{

    ULONG index = (m_fGpScratch == TRUE) ? (index = MCPX_MEM_GPSCRATCH) : (MCPX_MEM_EPSCRATCH);

    //
    // keep a pointer to the proper alloc contexts
    //

    m_pDmaBufferContext = &CMcpxCore::m_ctxMemory[index];
    m_pSgeTableContext = &CMcpxCore::m_ctxMemory[index+1];

    //
    // the caller passes us an array of ALLOC_CONTEXTs
    // first one is the allocation for the scratch memory itself
    // the second is the allocation for the SGE table describing the scratch space
    //

	m_dwMaxPages = (USHORT) (m_pSgeTableContext->Size / NV_PSGE_SIZE);
    m_dwReservedPages = dwReservedPages;

    //
    // initialize the hw SGE table and our internal page table to point to 
    // DMA allocated memory
    //

    MCP1_PRD *pPrd = (MCP1_PRD *)m_pSgeTableContext->VirtualAddress;

    ULONG bytesRemaining = m_pDmaBufferContext->Size;
    ULONG bytesMapped = 0;
    PVOID currentVa = m_pDmaBufferContext->VirtualAddress;

    //
    // note that all pages are 4k aligned and since the dma buffer is 4k length
    // aligned, all sizes are 4k as well..
    //

    for (ULONG i = 0; i < m_dwMaxPages-dwReservedPages; i++)
    {
        pPrd[i].uAddr = CMcpxCore::MapTransfer(&currentVa,
                                               &bytesRemaining,
                                               &bytesMapped);

        pPrd[i].Control.uValue = 0;
        pPrd[i].Control.Owner = NV_PSSL_PRD_CONTROL_OWN_HW;       

    }

    //
    // Set base address for GP/EP scratch
    //

    //
    // tell the hw that all pages are valid , except the reserved ones
    //
    
    if (m_fGpScratch) {

        MCPX_REG_WRITE(NV_PAPU_GPSADDR, m_pSgeTableContext->PhysicalAddress);    
        MCPX_REG_WRITE(NV_PAPU_GPSMAXSGE, m_dwMaxPages-m_dwReservedPages);

    } else {

        MCPX_REG_WRITE(NV_PAPU_EPSADDR, m_pSgeTableContext->PhysicalAddress);    
        MCPX_REG_WRITE(NV_PAPU_EPSMAXSGE, m_dwMaxPages);
    }

    return;
}

VOID
CMcpxDspScratchDma::AddPages(
    PMCPX_ALLOC_CONTEXT pContextArray,
    DWORD dwCount,
    PULONG pOffset
    )
{
    DWORD dwPageIndex = m_dwMaxPages - m_dwReservedPages;
    ULONG i;

    ASSERT(dwCount <= m_dwReservedPages);

    m_dwReservedPages -= dwCount;

    MCP1_PRD *pPrd = (MCP1_PRD *)CMcpxCore::m_ctxMemory[MCPX_MEM_GPSCRATCHSGE].VirtualAddress;

    for (i=0;i<dwCount;i++) {

        //
        // update the hw SGE table
        //

        pPrd[dwPageIndex+i].uAddr = pContextArray[i].PhysicalAddress;

        pPrd[dwPageIndex+i].Control.uValue = 0;
        pPrd[dwPageIndex+i].Control.Owner = NV_PSSL_PRD_CONTROL_OWN_HW;       

    }

    //
    // tell the hw that all pages are valid , including the reserved ones
    //
    
    MCPX_REG_WRITE(NV_PAPU_GPSMAXSGE, m_dwMaxPages-m_dwReservedPages);

    //
    // give them the linear offset into our DMA buffer window
    //

    *pOffset = dwPageIndex*PAGE_SIZE;
    return;

}


VOID
CMcpxDspScratchDma::Copy(ULONG uLinOffset, VOID *pSource, ULONG uSize)
{
    //
    // scratch mem is contigious
    //

    PUCHAR pDest = (PUCHAR)m_pDmaBufferContext->VirtualAddress+uLinOffset;
    memcpy(pDest, pSource, uSize);
}

HRESULT
CMcpxDspScratchDma::AdjustFxScratch(DWORD dwSize)
{

    MCPX_ALLOC_CONTEXT ctx;
    ULONG offset,i;
    DWORD dwPageIndex = m_dwMaxPages-m_dwReservedPages;
    MCP1_PRD *pPrd = (MCP1_PRD *)CMcpxCore::m_ctxMemory[MCPX_MEM_GPSCRATCHSGE].VirtualAddress;
    DWORD dwCount = dwSize/PAGE_SIZE;

    if (dwCount > m_dwReservedPages) {

        DPF_ERROR("Effects Image scratch space usage exceed max (%d) pages",
                  MCPX_HW_MAX_FX_SCRATCH_PAGES);

        return DSERR_OUTOFMEMORY;
    }

    ctx.Size = 0;

    if (m_ctxFxScratch.Size < dwSize) {

        //
        // there is already scratch allocated for FX delay lines
        // If its larger than what we need dont do anything.
        // if its less, free whats there and replace it with a new allocation
        //
    
        memcpy(&ctx,&m_ctxFxScratch,sizeof(m_ctxFxScratch));        

        //
        // allocate a new one
        //

        m_ctxFxScratch.Size = dwSize;
        m_ctxFxScratch.VirtualAddress = POOLALLOC(BYTE, m_ctxFxScratch.Size);
        if (m_ctxFxScratch.VirtualAddress == NULL) {

            return E_OUTOFMEMORY;

        }

        MmLockUnlockBufferPages(m_ctxFxScratch.VirtualAddress,dwSize,FALSE);

    } else {

        return DS_OK;
    }


    for (i=0;i<dwCount;i++) {

        //
        // update the hw SGE table
        //

        pPrd[dwPageIndex+i].uAddr = MmGetPhysicalAddress(i*PAGE_SIZE+(PUCHAR)m_ctxFxScratch.VirtualAddress);
        pPrd[dwPageIndex+i].Control.uValue = 0;
        pPrd[dwPageIndex+i].Control.Owner = NV_PSSL_PRD_CONTROL_OWN_HW;       

    }

    //
    // tell the hw, more pages are now valid...
    //

    MCPX_REG_WRITE(NV_PAPU_GPSMAXSGE, m_dwMaxPages-m_dwReservedPages+dwCount);

    //
    // free old allocation
    //

    if (ctx.Size) {

        MmLockUnlockBufferPages(ctx.VirtualAddress,ctx.Size,TRUE);
        POOLFREE(ctx.VirtualAddress);

    }

    return DS_OK;
}


