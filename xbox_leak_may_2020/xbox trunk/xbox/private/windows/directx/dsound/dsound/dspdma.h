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

#ifndef _CSCRATCHDMA_
#define _CSCRATCHDMA_

namespace DirectSound
{
    class CMcpxDspScratchDma
    {
    public:
        CMcpxDspScratchDma(BOOL fGpScratch);
        CMcpxDspScratchDma::~CMcpxDspScratchDma();


    public:
        VOID Initialize(DWORD dwReservedPages);
        VOID Copy(ULONG uLinOffset, VOID *pSource, ULONG uSize);
        VOID Put(ULONG uLinOffset, ULONG dwValue)
        {
            *((PULONG)m_pDmaBufferContext->VirtualAddress+uLinOffset/sizeof(DWORD)) = dwValue;
        }
    
        VOID
        AddPages(
            PMCPX_ALLOC_CONTEXT pContextArray,
            DWORD dwCount,
            PULONG pOffset
            );

        VOID *GetScratchTableLinAddr() { return m_pSgeTableContext->VirtualAddress; }
        VOID *GetScratchSpaceLinAddr() { return m_pDmaBufferContext->VirtualAddress; }
        VOID *GetFxScratchSpaceLinAddr() { return m_ctxFxScratch.VirtualAddress; }

        ULONG GetScratchSpacePhysAddr() { return m_pDmaBufferContext->PhysicalAddress; }
        ULONG GetScratchSpaceSize() { return m_pDmaBufferContext->Size; }
        HRESULT AdjustFxScratch(DWORD dwSize);


    private:
    
        DWORD       m_dwMaxPages;
        DWORD       m_dwReservedPages;
        BOOL        m_fGpScratch;

        MCPX_ALLOC_CONTEXT *m_pSgeTableContext;
        MCPX_ALLOC_CONTEXT *m_pDmaBufferContext;
        MCPX_ALLOC_CONTEXT m_ctxFxScratch;

        ULONG        m_uRegOffsetValidPages;
    };
}

//
// scratch queue
//

typedef union _MCPX_DSP_MBOX
{
    struct
    {
        UCHAR    write;
        UCHAR    read;
    } Ptr;

    USHORT uVal;
} MCPX_DSP_MBOX, *PMCPX_DSP_MBOX;

namespace DirectSound
{
    class CMcpxDspScratchQ
    {
    public:

        CMcpxDspScratchQ(ULONG uBaseOffset, 
                         UCHAR uMaxEntries, 
                         CMcpxCore *pApu, 
                         CMcpxDspScratchDma *pDma);

        UCHAR Start();
        VOID Stop();

        BOOL Read(ULONG *puMethod, ULONG *puData);
        BOOL Write(ULONG uMethod, ULONG uData1, ULONG uData2);

        ULONG GetSize()  { return (3*m_uWrapIndex); }
        ULONG GetOffset() { return m_uBaseOffset; }
    
    private:
        UCHAR       GetAvailableSlots();
        VOID        Incr(UCHAR *puIndex);
    
        ULONG           m_uBaseOffset;
        ULONG           m_uRegOffset;
    
        MCPX_DSP_MBOX   m_MailBox;
        CMcpxCore        *m_pApu;
        CMcpxDspScratchDma     *m_pScratchDma;
        ULONG            m_uStartFlag;

        UCHAR           m_uWrapIndex;
    };
}

#endif
