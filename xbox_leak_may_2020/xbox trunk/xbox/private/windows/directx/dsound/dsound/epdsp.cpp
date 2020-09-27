
#include "dsoundi.h"
#include "dspdma.h"
#include "GPDsp.h"
#include "EPDsp.h"

#pragma const_seg("DOLBY")

const ULONG AC3SuperExec[] = 
{
#include <AC3SuperExec.h>
};

const ULONG AC3Loader[] =
{
#include <AC3Loader.h>
};

const ULONG AC3Dolby1[] =
{
#include <AC3Dolby1.h>
};

const ULONG AC3Dolby2[] =
{
#include <AC3Dolby2.h>
};

const ULONG AC3Dolby3[] =
{
#include <AC3Dolby3.h>
};

const ULONG AC3Dolby4[] =
{
#include <AC3Dolby4.h>
};

const ULONG AC3Dolby5[] =
{
#include <AC3Dolby5.h>
};

const ULONG AC3Dolby6[] =
{
#include <AC3Dolby6.h>
};

const DOLBY_CONFIG_TABLE AC3ConfigTable =
{
    24,         // table size
    1,          // perform dolby surround encode flag
    1,          // perform dolby game encode flag
    0,          // perform downmix
    0,          // ac3 pcm sampling rate code
    7,          // ac3 dolby input channel config code (acmod)
    1,          // lfe channel present flag
    31,         // ac3 dialnorm value
    9,          // ac3 channel bandwidth code
    0,          // channel lpf enabled flag
    1,          // lfe channel lpf enabled flag
    1,          // dc hpf enabled flag
    0,          // dynrng exists flag
    0,          // ac3 dynrng code
    1,          // compr exists flag
    0xEF,       // ac3 compr code
    0,          // dolby surround gain enabled flag
    0x65AC8C,   // dolby surround encoder gain value
    2,          // dolby surround mode. Only used for 2/0 Mode
    0,          // reserved 1
    0,          // reserved 2
    0,          // reserved 3
    0,          // reserved 4
    0           // reserved 5
};


#pragma const_seg("DSOUND_RD")


ULONG
CMcpxEPDspManager::AC3GetTotalScratchSize()
{
    // in bytes
    return (AC3_DOLBYBASE * sizeof(ULONG) + 
            sizeof(AC3Dolby1) + 
            sizeof(AC3Dolby2) + 
            sizeof(AC3Dolby3) + 
            sizeof(AC3Dolby4) +
            sizeof(AC3Dolby5) +
            sizeof(AC3Dolby6) +
            sizeof(DOLBY_CONFIG_TABLE) + 
            AC3_HEAPLENGTH * sizeof(ULONG));

}

VOID
CMcpxEPDspManager::AC3GetSuperExec(PVOID *ppData, ULONG *pSize, ULONG *pBase)
{
    *ppData = (PVOID)AC3SuperExec;
    *pSize = sizeof(AC3SuperExec)/sizeof(DWORD);
    *pBase = AC3_SUPREXECBASE;
}

VOID
CMcpxEPDspManager::AC3GetLoader(PVOID *ppData, ULONG *pSize, ULONG *pBase)
{
    *ppData = (PVOID)AC3Loader;
    *pSize = sizeof(AC3Loader)/sizeof(DWORD);
    *pBase = AC3_LOADERBASE;
}

PVOID 
CMcpxEPDspManager::AC3GetInitialConfigTable()
{
    return (PVOID)&AC3ConfigTable;
}

VOID
CMcpxEPDspManager::AC3GetProgram(ULONG uIndex, PVOID *ppData, ULONG *pSize)
{
    switch(uIndex)
    {
    case 0:
        *ppData = (PVOID)AC3Dolby1;
        *pSize = sizeof(AC3Dolby1)/sizeof(DWORD);
        break;
    case 1:
        *ppData = (PVOID)AC3Dolby2;
        *pSize = sizeof(AC3Dolby2)/sizeof(DWORD);
        break;
    case 2:
        *ppData = (PVOID)AC3Dolby3;
        *pSize = sizeof(AC3Dolby3)/sizeof(DWORD);
        break;
    case 3:
        *ppData = (PVOID)AC3Dolby4;
        *pSize = sizeof(AC3Dolby4)/sizeof(DWORD);
        break;

    case 4:
        *ppData = (PVOID)AC3Dolby5;
        *pSize = sizeof(AC3Dolby5)/sizeof(DWORD);
        break;

    case 5:
        *ppData = (PVOID)AC3Dolby6;
        *pSize = sizeof(AC3Dolby6)/sizeof(DWORD);
        break;

    default:
        *ppData = NULL;
        *pSize = 0;
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// dma manager code
//
///////////////////////////////////////////////////////////////////////////////


CMcpxEPDspManager::CMcpxEPDspManager()
{
    m_pGlobalProc = NULL;
    m_pScratchDma = NULL;
    m_pConfigTable = NULL;
}

CMcpxEPDspManager::~CMcpxEPDspManager()
{
    DELETE(m_pScratchDma);
}

VOID   
CMcpxEPDspManager::Initialize(CMcpxGPDspManager *pGlobalProc)
{
    ASSERT(CMcpxCore::m_ctxMemory[MCPX_MEM_EPSCRATCH].VirtualAddress);
    ASSERT(CMcpxCore::m_ctxMemory[MCPX_MEM_EPSCRATCH].PhysicalAddress);
    ASSERT(CMcpxCore::m_ctxMemory[MCPX_MEM_EPSCRATCH].Size);

    ASSERT(CMcpxCore::m_ctxMemory[MCPX_MEM_EPSCRATCHSGE].VirtualAddress);
    ASSERT(CMcpxCore::m_ctxMemory[MCPX_MEM_EPSCRATCHSGE].PhysicalAddress);
    ASSERT(CMcpxCore::m_ctxMemory[MCPX_MEM_EPSCRATCHSGE].Size);

    m_pGlobalProc = pGlobalProc;

    //
    // allocate the scratch Dma. This must be done AFTER allocating the contexts above
    //

    m_pScratchDma = NEW(CMcpxDspScratchDma(FALSE));
    ASSERT(m_pScratchDma);
    
    //
    // initialize the scratch dma object. We dont need do reserve any pages
    //

    m_pScratchDma->Initialize(0);
    
#ifdef DEBUG
    //
    // check if somebody called DirectSoundLoadEncoder
    // if they did, use their code+data instead of re-creating it here..
    //

    if ((CDirectSound::m_pvEncoderImageData != NULL) &&
        (CDirectSound::m_dwEncoderImageSize != 0)) {

        m_pScratchDma->Copy(0, CDirectSound::m_pvEncoderImageData, CDirectSound::m_dwEncoderImageSize);
           
    } else 
#endif 
    {
        //
        // copy the dolby DSP code to the DMA memory pages
        //

        PVOID pCode;
        ULONG uSize;
        ULONG uBase;
    
        //
        // superexec dsp code
        //

        AC3GetSuperExec(&pCode, &uSize, &uBase);
        m_pScratchDma->Copy(uBase*sizeof(ULONG), (VOID *)pCode, uSize*sizeof(ULONG));

        //
        // loader code, copy to dma scratch space
        //

        AC3GetLoader(&pCode, &uSize, &uBase);
        m_pScratchDma->Copy(uBase*sizeof(ULONG), (VOID *)pCode, uSize*sizeof(ULONG));

        //
        // loader table
        //

        DOLBY_LOADER_TABLE LoaderTable;
    
        LoaderTable.tableSize = sizeof(DOLBY_LOADER_TABLE) / sizeof(ULONG);
        LoaderTable.maxProgs = AC3GetMaxPrograms();

        //
        // now copy the four Dolby encoder programs into system memory so the DSP can
        // DMA them when it needs them.  Keep track of the current offset into the
        // buffer, and update the loader table with the addresses as we go along.
        // NOTE: offset and Sizes are in DWORDS!!!!!
        //

        ULONG uCurrentOffset = AC3GetProgramBase();
    
        ULONG uCnt;
        for (uCnt = 0; uCnt < AC3GetMaxPrograms(); uCnt++)
        {
            AC3GetProgram(uCnt, &pCode, &uSize);
            m_pScratchDma->Copy(uCurrentOffset*sizeof(ULONG), (VOID *)pCode, uSize*sizeof(ULONG));

            //
            // update the loader table
            //

            LoaderTable.prog[uCnt].ptr = uCurrentOffset;
            LoaderTable.prog[uCnt].size = uSize ;
            uCurrentOffset += uSize;
        }

        //
        // now the config table
        //

        ULONG *pConfig = (ULONG *)AC3GetInitialConfigTable();

        //
        // Note: the AC3 config table is actually a DOLBY_CONFIG_TABLE,
        // not a ULONG...  The size is contained in the first word of the
        // table, so we will treat it here as a ULONG array.
        //

        uSize = pConfig[0];

        LoaderTable.config_ptr = uCurrentOffset;
        LoaderTable.config_size = uSize;

        m_pScratchDma->Copy(uCurrentOffset*sizeof(ULONG), (VOID *)pConfig, uSize * sizeof(ULONG));

        //
        // save the config table offset into the scratch space.
        //

        m_pConfigTable = uCurrentOffset*sizeof(ULONG) +  (PUCHAR)m_pScratchDma->GetScratchSpaceLinAddr();

        uCurrentOffset += uSize;

        //Init pingpong buffer offset
        LoaderTable.pingpong_offset = 0;
        LoaderTable.reserved1 = 0;
        LoaderTable.reserved2 = 0;
        LoaderTable.reserved3 = 0;
    
        //
        // now for the heap
        //

        LoaderTable.heap_ptr = uCurrentOffset;
        LoaderTable.heap_size = AC3GetHeapSize();
    
        //
        // copy the table to the offset
        //

        m_pScratchDma->Copy(AC3GetLoaderTableBase()*sizeof(ULONG), (VOID *)&LoaderTable, sizeof(DOLBY_LOADER_TABLE));
    }

    //
    // map the same number of pages into the GP scratch.
    // The GP outputs dolby data to SCRATCH space, not to the normal FIFO output buffers
    // THe EP then reads from the GP scratch buffer...
    //

    AC3StartGpInput(CMcpxCore::m_ctxMemory[MCPX_MEM_EPSCRATCH].Size - (MCPX_HW_AC3_NUM_INPUT_PAGES * PAGE_SIZE));

    return;
}


DOLBY_CONFIG_TABLE*
CMcpxEPDspManager::GetDolbyConfigTable()
{
    return (DOLBY_CONFIG_TABLE*) m_pConfigTable;
}

VOID
CMcpxEPDspManager::AC3StartGpInput(ULONG uLinOffset)
{
    //
    // use the last n pages for ac-3 input from the GP
    //

    MCPX_ALLOC_CONTEXT mem[MCPX_HW_AC3_NUM_INPUT_PAGES];
    ULONG uNumOfPagesForInput = MCPX_HW_AC3_NUM_INPUT_PAGES;
    
    ASSERT(m_pGlobalProc);

    ULONG uOffset = AC3GetLoaderTableBase()*sizeof(ULONG);

#ifdef DEBUG
    //
    // check if somebody called DirectSoundLoadEncoder
    // if they did, use their code+data instead of re-creating it here..
    //

    if ((CDirectSound::m_pvEncoderImageData == NULL) &&
        (CDirectSound::m_dwEncoderImageSize == 0)) 
#endif
    {
        //
        // because we ping pong, here we tell the DSP code to address half the total buffer
        //

        m_pScratchDma->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(pcm_ptr), uLinOffset/sizeof(ULONG));
        m_pScratchDma->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(pcm_size), AC3_FRAME_SIZE * AC3_NUM_CHANNELS);
    }

    // program the same pages in the GP
    for (ULONG uCnt = 0; uCnt < uNumOfPagesForInput; uCnt++)
    {
        mem[uCnt].VirtualAddress = (PUCHAR) m_pScratchDma->GetScratchSpaceLinAddr() + uLinOffset;
        mem[uCnt].PhysicalAddress = m_pScratchDma->GetScratchSpacePhysAddr() +uLinOffset ;
        mem[uCnt].Size = PAGE_SIZE;
        uLinOffset += mem[uCnt].Size;
    }
    
    m_pGlobalProc->AC3SetOutputBuffer(mem, uNumOfPagesForInput);
    return;
}


VOID
CMcpxEPDspManager::AC3SetAnalogOutput(ULONG uAnalogFifoIndex, ULONG uSize)
{

    ULONG uOffset = AC3GetLoaderTableBase()*sizeof(ULONG);

    ASSERT(m_pGlobalProc);

#ifdef DEBUG
    //
    // check if somebody called DirectSoundLoadEncoder
    // if they did, use their code+data instead of re-creating it here..
    //

    if ((CDirectSound::m_pvEncoderImageData == NULL) &&
        (CDirectSound::m_dwEncoderImageSize == 0)) 
#endif
    {
        m_pScratchDma->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(ltrt_ptr), uAnalogFifoIndex);
        m_pScratchDma->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(ltrt_size), uSize/sizeof(ULONG));
    }

    return;
}

VOID
CMcpxEPDspManager::AC3SetDigitalOutput(ULONG uDigitalFifoIndex, ULONG uSize)
{

    ULONG uOffset = AC3GetLoaderTableBase() * sizeof(ULONG);
    ASSERT(m_pGlobalProc);

#ifdef DEBUG
    //
    // check if somebody called DirectSoundLoadEncoder
    // if they did, use their code+data instead of re-creating it here..
    //

    if ((CDirectSound::m_pvEncoderImageData == NULL) &&
        (CDirectSound::m_dwEncoderImageSize == 0)) 
#endif
    {
        m_pScratchDma->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(ac3_ptr), uDigitalFifoIndex);
        m_pScratchDma->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(ac3_zero_fill), 1);
        m_pScratchDma->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(ac3_preamble), 1);
        m_pScratchDma->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(ac3_size), uSize/sizeof(ULONG));
    }

    return;
}


