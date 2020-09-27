/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mcpxcore.cpp
 *  Content:    MCP-X core object.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  04/24/01    dereks  Separated from APU object for kernel ROM bring-up.
 *
 ****************************************************************************/

#include "dsoundi.h"


/****************************************************************************
 *
 *  CMcpxCore
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::CMcpxCore"

const DWORD CMcpxCore::m_adwGPOutputBufferSizes[MCPX_GPOUTPUT_COUNT] =
{
    0x2000  // MCPX_GPOUTPUT_BOOT_SOUND
};

const DWORD CMcpxCore::m_adwEPOutputBufferSizes[MCPX_EPOUTPUT_COUNT] =
{
    0x2000, // MCPX_EPOUTPUT_AC97_ANALOG
    0x4000  // MCPX_EPOUTPUT_AC97_DIGITAL
};

MCPX_ALLOC_CONTEXT CMcpxCore::m_ctxMemory[MCPX_MEM_COUNT];

CMcpxCore::CMcpxCore
(
    CDirectSoundSettings *  pSettings
)
{
    DPF_ENTER();

    m_pSettings = ADDREF(pSettings);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CMcpxCore
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::~CMcpxCore"

CMcpxCore::~CMcpxCore
(
    void
)
{
    DWORD                   i;

    DPF_ENTER();

#ifdef MCPX_BOOT_LIB

    Reset();

#else // MCPX_BOOT_LIB

    //
    // We're assuming the derived object already called Reset
    //

#endif // MCPX_BOOT_LIB

    //
    // Free the DSP managers
    //

    DELETE(m_pGpDspManager);

#ifndef MCPX_BOOT_LIB

    DELETE(m_pEpDspManager);

#endif // MCPX_BOOT_LIB

    //
    // Free APU memory
    //

    for(i = 0; i < MCPX_MEM_COUNT; i++)
    {

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

        if(m_ctxMemory[i].fOwned)

#endif // ENABLE_SLOP_MEMORY_RECOVERY

        {
            PHYSFREE(m_ctxMemory[i].VirtualAddress);
        }

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

        else
        {
            m_ctxMemory[i].VirtualAddress = NULL;
        }

#endif // ENABLE_SLOP_MEMORY_RECOVERY

        m_ctxMemory[i].PhysicalAddress = 0;
        m_ctxMemory[i].Size = 0;
    }

    //
    // Release object references
    //

    RELEASE(m_pSettings);

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

    RELEASE(m_pSlopMemoryHeap);

#endif // ENABLE_SLOP_MEMORY_RECOVERY

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Intializes the object.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::Initialize"

HRESULT 
CMcpxCore::Initialize
(
    void
)
{
    HRESULT                 hr;

    DPF_ENTER();

    //
    // Make sure we're in a known good state
    //
    
    Reset();

    //
    // Allocate memory
    //

    hr = AllocateApuMemory();

    //
    // Set up the front-end and voice processor
    //

    if(SUCCEEDED(hr))
    {
        SetupFrontEndProcessor();
        SetupVoiceProcessor();
    }

    //
    // Set up the AC97
    //

    if(SUCCEEDED(hr))
    {
        hr = SetupAc97();
    }

    //
    // Set up the DSPs
    //

    if(SUCCEEDED(hr)) 
    {
        SetupDSPs();
    }    

    //
    // Set up the APU/ACI link
    //

    if(SUCCEEDED(hr))
    {
        SetSetupEngineState(MCPX_SE_STATE_DEFAULT);
    }

    //
    // Start the ACI channels running
    //

    if(SUCCEEDED(hr))
    {       
        m_Ac97.m_apChannels[AC97_CHANNELTYPE_DIGITAL]->Run();
        m_Ac97.m_apChannels[AC97_CHANNELTYPE_ANALOG]->Run();
    }

    DPF_LEAVE_HRESULT(hr);

	return hr;
}


/****************************************************************************
 *
 *  Reset
 *
 *  Description:
 *      Resets the APU state.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to only do a partial reset.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::Reset"

void
CMcpxCore::Reset
(
    void
)
{
    DPF_ENTER();

    //
    // Shut down the AC'97
    //

    m_Ac97.Terminate();

    //
    // Shut down the APU components
    //

    IdleVoiceProcessor(TRUE);
    SetInterruptState(FALSE);
    SetPrivLockState(TRUE);
    SetFrontEndState(MCPX_FE_STATE_HALTED);
    SetSetupEngineState(MCPX_SE_STATE_OFF);

    MCPX_REG_WRITE(NV_PAPU_TVL2D, 0xFFFF);
    MCPX_REG_WRITE(NV_PAPU_TVL3D, 0xFFFF);
    MCPX_REG_WRITE(NV_PAPU_TVLMP, 0xFFFF);

    MCPX_REG_WRITE(NV_PAPU_CVL2D, 0xFFFF);
    MCPX_REG_WRITE(NV_PAPU_CVL3D, 0xFFFF);
    MCPX_REG_WRITE(NV_PAPU_CVLMP, 0xFFFF);

    MCPX_REG_WRITE(NV_PAPU_NVL2D, 0xFFFF);
    MCPX_REG_WRITE(NV_PAPU_NVL3D, 0xFFFF);
    MCPX_REG_WRITE(NV_PAPU_NVLMP, 0xFFFF);

    MCPX_REG_WRITE(NV_PAPU_GPRST, 0);
    MCPX_REG_WRITE(NV_PAPU_EPRST, 1);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetupFrontEndProcessor
 *
 *  Description:
 *      Sets up the front-end processor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::SetupFrontEndProcessor"

void
CMcpxCore::SetupFrontEndProcessor
(
    void
)
{
    DWORD                   dwForce[2]      = { 0, 0 };
    DWORD                   dwIgnore[2]     = { 0, 0 };
    R_INTR                  rInterruptMask;
    R_FE_CONTROL            rFeControl;
    R_SE_CONTROL            rSeControl;
    DWORD                   i;

    DPF_ENTER();

    //
    // Setup interrupt masks
    //

    rInterruptMask.uValue = 0;
    rInterruptMask.General = NV_PAPU_IEN_GINTEN_DISABLED;

#ifdef MCPX_ENABLE_DELTA_WARNINGS

    rInterruptMask.DeltaWarning = NV_PAPU_IEN_DWINTEN_ENABLED;

#else // MCPX_ENABLE_DELTA_WARNINGS

    rInterruptMask.DeltaWarning = NV_PAPU_IEN_DWINTEN_DISABLED;

#endif // MCPX_ENABLE_DELTA_WARNINGS

    rInterruptMask.DeltaPanic = NV_PAPU_IEN_DPINTEN_ENABLED;
    rInterruptMask.FETrap = NV_PAPU_IEN_FETINTEN_ENABLED;
    rInterruptMask.FENotify = NV_PAPU_IEN_FENINTEN_DISABLED;
    rInterruptMask.FEVoice = NV_PAPU_IEN_FEVINTEN_ENABLED;
    rInterruptMask.FEMethodOverFlow = NV_PAPU_IEN_FEOINTEN_ENABLED;
    rInterruptMask.GPMailbox = NV_PAPU_IEN_GPMINTEN_DISABLED;
    rInterruptMask.GPNotify = NV_PAPU_IEN_GPNINTEN_DISABLED;
    rInterruptMask.EPMailbox = NV_PAPU_IEN_EPMINTEN_DISABLED;
    rInterruptMask.EPNotify = NV_PAPU_IEN_EPNINTEN_DISABLED;

    MCPX_REG_WRITE(NV_PAPU_IEN, rInterruptMask.uValue);

    //
    // Set default FE control bits
    //

    rFeControl.uValue = 0;
    rFeControl.TrapOnNotifier = NV_PAPU_FECTL_FENINT_ENABLED;
    rFeControl.Mode = NV_PAPU_FECTL_FEMETHMODE_HALTED;
    rFeControl.EnableLock = NV_PAPU_FECTL_FEMETH_PRIVLOCK_NOT_DISABLED;

    MCPX_REG_WRITE(NV_PAPU_FECTL, rFeControl.uValue);

    //
    // Set default SE control bits
    //
    
    rSeControl.uValue = 0;
    rSeControl.GSCUpdate = NV_PAPU_SECTL_XCNTMODE_OFF;

    MCPX_REG_WRITE(NV_PAPU_SECTL, rSeControl.uValue);

    //
    // We're overriding the internal SE2FE_IDLE_MESSAGE because a couple
    // of things don't work the way they should.  First, the hardware has
    // a race condition that can cause voice list corruption if a VoiceOn
    // method is received with an antecedent voice that has already gone
    // idle.  Second, VoiceRelease causes a PERSIST voice to be removed
    // from the processing list when the envelope completes.  By overriding
    // SE2FE_IDLE_VOICE in software, we can address both of these issues
    //

    dwForce[1] |= MCPX_MAKE_REG_VALUE(1, NV_PAPU_FETFORCE1_SE2FE_IDLE_VOICE);

    MCPX_REG_WRITE(NV_PAPU_FETFORCE0, dwForce[0]);
    MCPX_REG_WRITE(NV_PAPU_FETFORCE1, dwForce[1]);

    //
    // SET_VOICE_TAR_Hrtf has a bug in it that prevents us from setting 
    // a NULL HRTF filter handle (0xFFFF).  This is strictly a parameter
    // validation bug since NULL is a valid value.
    //
    // VoiceOn will cause a BAD_LIST_POINTER trap if the VP isn't idle
    // and we use the INHERIT list type.  We're working around this in
    // CMcpxVoiceClient::ActivateVoice.
    //

    dwIgnore[0] |= MCPX_MAKE_REG_VALUE(1, NV_PAPU_FETIGNORE0_SET_VOICE_TAR_HRTF);
    dwIgnore[0] |= MCPX_MAKE_REG_VALUE(1, NV_PAPU_FETIGNORE0_VOICE_ON);

    MCPX_REG_WRITE(NV_PAPU_FETIGNORE0, dwIgnore[0]);
    MCPX_REG_WRITE(NV_PAPU_FETIGNORE1, dwIgnore[1]);

    //
    // Initialize global counts so we can track how far ahead the output 
    // counter stays from the input
    //

    ResetGlobalCounters();
    
    //
    // Set up boundaries
    //

    MCPX_REG_WRITE(NV_PAPU_FEMAXV, MCPX_HW_MAX_VOICES - 1);
	MCPX_REG_WRITE(NV_PAPU_FEMAXTV, MCPX_HW_MAX_3D_VOICES - 1);
	MCPX_REG_WRITE(NV_PAPU_FEMAXHT, MCPX_HW_MAX_3D_VOICES * MCPX_HW_3DFILTERS_PER_VOICE - 1);
	MCPX_REG_WRITE(NV_PAPU_FEMAXSESSL, MCPX_HW_MAX_SSL_PRDS - 1);
	MCPX_REG_WRITE(NV_PAPU_FEMAXSESGE, MCPX_HW_MAX_BUFFER_PRDS - 1);
	MCPX_REG_WRITE(NV_PAPU_FEMAXMB, MCPX_HW_MAX_SUBMIX_BINS);

    MCPX_REG_WRITE(NV_PAPU_FESESSLCTXPA, 0);
    MCPX_REG_WRITE(NV_PAPU_FESESSLMAXOFF, MCPX_MAX_VALID_ADDRESS);

    MCPX_REG_WRITE(NV_PAPU_FESESGECTXPA, 0);
    MCPX_REG_WRITE(NV_PAPU_FESESGEMAXOFF, MCPX_MAX_VALID_ADDRESS);

    MCPX_REG_WRITE(NV_PAPU_FEGPSGECTXPA, 0);
    MCPX_REG_WRITE(NV_PAPU_FEGPSGEMAXOFF, MCPX_MAX_VALID_ADDRESS);

    //
    // Unlock the front end and setup engine
    //

    SetPrivLockState(FALSE);
    SetFrontEndState(MCPX_FE_STATE_FREE_RUNNING);
    SetFrontEndState(MCPX_FE_STATE_ISO);
    SetSetupEngineState(MCPX_SE_STATE_ISO);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  AllocateApuMemory
 *
 *  Description:
 *      Allocates system memory buffers ued by the APU.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::AllocateApuMemory"

HRESULT
CMcpxCore::AllocateApuMemory
(
    void
)
{
    BEGIN_DEFINE_STRUCT()
        DWORD               Size;
        DWORD               Alignment;
    END_DEFINE_STRUCT(MCPX_ALLOC_CTX);

    HRESULT                 hr                      = DS_OK;
    MCPX_ALLOC_CTX          Alloc[MCPX_MEM_COUNT];
    DWORD                   i;

    DPF_ENTER();

    //
    // Calculate allocation sizes and alignment.  We piggy-back the SGE heap
    // run markers on the magic write buffer since the magic write only needs
    // a single DWORD.  When we're building the boot sound, we allocate memory
    // for the I3DL2 reverb in the GP scratch.
    //
    // WARNING: to use some of the slop memory allocated due to alignement
    // restrictions, we're piggybacking some data structures on these
    // buffers.  Use extreme care when changing any of these sizes.
    //

    for(i = 0, Alloc[MCPX_MEM_GPOUTPUT].Size = 0; i < NUMELMS(m_adwGPOutputBufferSizes); i++)
    {
        ASSERT(!(m_adwGPOutputBufferSizes[i] % PAGE_SIZE));
        Alloc[MCPX_MEM_GPOUTPUT].Size += m_adwGPOutputBufferSizes[i];
    }

    Alloc[MCPX_MEM_GPOUTPUT].Alignment = PAGE_SIZE;

    for(i = 0, Alloc[MCPX_MEM_EPOUTPUT].Size = 0; i < NUMELMS(m_adwEPOutputBufferSizes); i++)
    {
        ASSERT(!(m_adwEPOutputBufferSizes[i] % PAGE_SIZE));
        Alloc[MCPX_MEM_EPOUTPUT].Size += m_adwEPOutputBufferSizes[i];
    }

    Alloc[MCPX_MEM_EPOUTPUT].Alignment = PAGE_SIZE;

    Alloc[MCPX_MEM_MAGICWRITE].Size = sizeof(DWORD);
    Alloc[MCPX_MEM_MAGICWRITE].Alignment = PAGE_SIZE;

    Alloc[MCPX_MEM_VOICE].Size = MCPX_HW_MAX_VOICES * NV_PAVS_SIZE;
    Alloc[MCPX_MEM_VOICE].Alignment = 0x8000;

    Alloc[MCPX_MEM_NOTIFIERS].Size = sizeof(MCPX_HW_NOTIFICATION) * MCPX_HW_MAX_NOTIFIERS;
    Alloc[MCPX_MEM_NOTIFIERS].Alignment = 0x4000;

    Alloc[MCPX_MEM_INPUTSGE].Size = MCPX_HW_MAX_BUFFER_PRDS * NV_PSGE_SIZE;
    Alloc[MCPX_MEM_INPUTSGE].Alignment = 0x4000;

    Alloc[MCPX_MEM_INPUTPRD].Size = MCPX_HW_MAX_SSL_PRDS * NV_PSGE_SIZE;
    Alloc[MCPX_MEM_INPUTPRD].Alignment = 0x4000;

    Alloc[MCPX_MEM_HRTFTARGET].Size = MCPX_HW_MAX_3D_VOICES * MCPX_HW_3DFILTERS_PER_VOICE * NV_PAHRTFT_SIZE;
    Alloc[MCPX_MEM_HRTFTARGET].Alignment = 0x4000;

    Alloc[MCPX_MEM_HRTFCURRENT].Size = MCPX_HW_MAX_3D_VOICES * NV_PAHRTFC_SIZE;
    Alloc[MCPX_MEM_HRTFCURRENT].Alignment = 0x4000;

    Alloc[MCPX_MEM_GPOUTPUTSGE].Size = Alloc[MCPX_MEM_GPOUTPUT].Size / PAGE_SIZE * NV_PSGE_SIZE;
    Alloc[MCPX_MEM_GPOUTPUTSGE].Alignment = 0x4000;

    Alloc[MCPX_MEM_EPOUTPUTSGE].Size = Alloc[MCPX_MEM_EPOUTPUT].Size / PAGE_SIZE * NV_PSGE_SIZE;
    Alloc[MCPX_MEM_EPOUTPUTSGE].Alignment = 0x4000;

    Alloc[MCPX_MEM_GPMULTIPASS].Size = MCPX_HW_MULTIPASS_NUM_PAGES * PAGE_SIZE;
    Alloc[MCPX_MEM_GPMULTIPASS].Alignment = 0x4000;

    Alloc[MCPX_MEM_GPSCRATCH].Size = ((MCPX_HW_MULTIPASS_NUM_PAGES + MCPX_HW_AC3_NUM_INPUT_PAGES) * PAGE_SIZE) + MCPX_GLOBAL_PROC_SCRATCH_SIZE;
    Alloc[MCPX_MEM_GPSCRATCH].Alignment = 0x4000;

#ifdef MCPX_BOOT_LIB
         
    Alloc[MCPX_MEM_GPSCRATCH].Size += 0x600000;

#endif // MCPX_BOOT_LIB

    Alloc[MCPX_MEM_GPSCRATCHSGE].Size = (MCPX_HW_MAX_FX_SCRATCH_PAGES + (Alloc[MCPX_MEM_GPSCRATCH].Size / PAGE_SIZE)) * NV_PSGE_SIZE;
    Alloc[MCPX_MEM_GPSCRATCHSGE].Alignment = 0x4000;

    Alloc[MCPX_MEM_EPSCRATCH].Size = ((CMcpxEPDspManager::AC3GetTotalScratchSize() / PAGE_SIZE) + 1 + MCPX_HW_AC3_NUM_INPUT_PAGES) * PAGE_SIZE;
    Alloc[MCPX_MEM_EPSCRATCH].Alignment = 0x4000;

    Alloc[MCPX_MEM_EPSCRATCHSGE].Size = Alloc[MCPX_MEM_EPSCRATCH].Size / PAGE_SIZE * NV_PSGE_SIZE;
    Alloc[MCPX_MEM_EPSCRATCHSGE].Alignment = 0x4000;

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

    //
    // Allocate the heap to track unused physical memory so we can reuse it
    // later
    //

    if(CMcpxSlopMemoryHeap::m_pSlopMemoryHeap)
    {
        m_pSlopMemoryHeap = ADDREF(CMcpxSlopMemoryHeap::m_pSlopMemoryHeap);
    }
    else
    {
        hr = HRFROMP(m_pSlopMemoryHeap = NEW(CMcpxSlopMemoryHeap));
    }

#endif // ENABLE_SLOP_MEMORY_RECOVERY

    //
    // Allocate APU memory
    //

    for(i = 0; (i < NUMELMS(Alloc)) && SUCCEEDED(hr); i++)
    {
        m_ctxMemory[i].Size = Alloc[i].Size;
        
        hr = AllocateContext(&m_ctxMemory[i], Alloc[i].Alignment, PAGE_READWRITE);

#ifdef ENABLE_SLOP_MEMORY_RECOVERY

        if(SUCCEEDED(hr))
        {
            m_ctxMemory[i].fOwned = !m_pSlopMemoryHeap->AddRun(m_ctxMemory[i].VirtualAddress, BLOCKALIGNPAD(m_ctxMemory[i].Size, PAGE_SIZE), m_ctxMemory[i].Size);
        }

#endif // ENABLE_SLOP_MEMORY_RECOVERY

    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetupVoiceProcessor
 *
 *  Description:
 *      Sets up the voice processor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::SetupVoiceProcessor"

void 
CMcpxCore::SetupVoiceProcessor
(
    void
)
{
    DWORD                   dwHrtfMixBins;
    DWORD                   i;

    DPF_ENTER();
    MCPX_CHECK_VOICE_FIFO_INIT(8);

    //
    // Set up the HW voice data structures to point to themselves
    //

    for(i = 0; i < MCPX_HW_MAX_VOICES; i++) 
    {
        MCPX_VOICE_STRUCT_WRITE(i, NV_PAVS_VOICE_TAR_PITCH_LINK, MCPX_MAKE_REG_VALUE(i, NV_PAVS_VOICE_TAR_PITCH_LINK_NEXT_VOICE_HANDLE));
    }

    //
    // Program the DMA PRD/SGE lists base addresses
    //

    MCPX_REG_WRITE(NV_PAPU_VPVADDR, m_ctxMemory[MCPX_MEM_VOICE].PhysicalAddress);
    MCPX_REG_WRITE(NV_PAPU_VPHTADDR, m_ctxMemory[MCPX_MEM_HRTFTARGET].PhysicalAddress);
    MCPX_REG_WRITE(NV_PAPU_VPHCADDR, m_ctxMemory[MCPX_MEM_HRTFCURRENT].PhysicalAddress);
    MCPX_REG_WRITE(NV_PAPU_VPSGEADDR, m_ctxMemory[MCPX_MEM_INPUTSGE].PhysicalAddress);
    MCPX_REG_WRITE(NV_PAPU_VPSSLADDR, m_ctxMemory[MCPX_MEM_INPUTPRD].PhysicalAddress);
    MCPX_REG_WRITE(NV_PAPU_FENADDR, m_ctxMemory[MCPX_MEM_NOTIFIERS].PhysicalAddress);

    //
    // Initialize all the global tracking parameters
    //

    MCPX_VOICE_WRITE(SetPitchTracking, MCPX_HW_DEFAULT_TRACKING);
    MCPX_VOICE_WRITE(SetFilterTracking, MCPX_HW_DEFAULT_TRACKING);
    MCPX_VOICE_WRITE(SetVolumeTracking, MCPX_HW_DEFAULT_TRACKING);
    MCPX_VOICE_WRITE(SetHRTFTracking, MCPX_HW_DEFAULT_TRACKING);
    MCPX_VOICE_WRITE(SetITDTracking, MCPX_HW_DEFAULT_TRACKING);

    //
    // Map the GP multipass page into the reserved SGE
    //

    MCPX_VOICE_WRITE(SetCurrentInBufSGE, MCPX_HW_MAX_BUFFER_PRDS - 1);
    MCPX_VOICE_WRITE(SetCurrentInBufSGEOffset, m_ctxMemory[MCPX_MEM_GPMULTIPASS].PhysicalAddress);

    //
    // Set 3D mixbins
    //

    dwHrtfMixBins = MCPX_MAKE_REG_VALUE(DirectSoundRequiredMixBins_3D.lpMixBinVolumePairs[0].dwMixBin, NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN0);
    dwHrtfMixBins |= MCPX_MAKE_REG_VALUE(DirectSoundRequiredMixBins_3D.lpMixBinVolumePairs[1].dwMixBin, NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN1);
    dwHrtfMixBins |= MCPX_MAKE_REG_VALUE(DirectSoundRequiredMixBins_3D.lpMixBinVolumePairs[2].dwMixBin, NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN2);
    dwHrtfMixBins |= MCPX_MAKE_REG_VALUE(DirectSoundRequiredMixBins_3D.lpMixBinVolumePairs[3].dwMixBin, NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN3);

    MCPX_VOICE_WRITE(SetHRTFSubmixes, dwHrtfMixBins);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetupGlobalProcessor
 *
 *  Description:
 *      Sets up the global processor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::SetupGlobalProcessor"

void 
CMcpxCore::SetupGlobalProcessor
(
    void
)                               
{                               
    DWORD                   adwSgeCounts[MCPX_GPOUTPUT_COUNT];
    DWORD                   dwTotalSgeCount;
    DWORD                   dwSgeOffset;
    DWORD                   dwAddress;
    R_GP_RESET              rReset;
    R_GPDMA_CONFIG          rGpDmaConfig;
    R_GP_CONTROL            rControl;
    DSP_CONTROL             DspControl;
    DWORD                   dwPos;
    DWORD                   i, z;
    MCP1_PRD *              pPrd;

    DPF_ENTER();
    
    //
    // Output buffer SGE table base address for output buffers and input buffers
    //

    MCPX_REG_WRITE(NV_PAPU_GPFADDR, m_ctxMemory[MCPX_MEM_GPOUTPUTSGE].PhysicalAddress);

    //
    // Initialize the get/put pointers
    //

    MCPX_REG_WRITE(NV_PAPU_GPGET, 0);
    MCPX_REG_WRITE(NV_PAPU_GPPUT, 0);

    //
    // Tell the hw how big the output SGE array needs to be
    //

    ASSERT(!(m_ctxMemory[MCPX_MEM_GPOUTPUT].PhysicalAddress % PAGE_SIZE));
    ASSERT(!(m_ctxMemory[MCPX_MEM_GPOUTPUT].Size % PAGE_SIZE));

    dwTotalSgeCount = m_ctxMemory[MCPX_MEM_GPOUTPUT].Size / PAGE_SIZE;

    ASSERT(!(dwTotalSgeCount & 0xFFFF0000));

	MCPX_REG_WRITE(NV_PAPU_GPFMAXSGE, dwTotalSgeCount - 1);
	MCPX_REG_WRITE(NV_PAPU_FEMAXGPSGE, dwTotalSgeCount - 1);

    MCPX_REG_WRITE(NV_PAPU_GPOFBASE0, 0);
    MCPX_REG_WRITE(NV_PAPU_GPOFEND0,  m_adwGPOutputBufferSizes[0]);

    //
    // Set up the output SGEs
    //

    pPrd = (MCP1_PRD *)m_ctxMemory[MCPX_MEM_GPOUTPUTSGE].VirtualAddress;

    for(i = 0, dwSgeOffset = 0; i < MCPX_GPOUTPUT_COUNT; i++)
    {
        for(z = 0; z < m_adwGPOutputBufferSizes[i] / PAGE_SIZE; z++)
        {
            dwAddress = m_ctxMemory[MCPX_MEM_GPOUTPUT].PhysicalAddress + ((dwSgeOffset + z) * PAGE_SIZE);
            
            MCPX_CHECK_VOICE_FIFO_INIT(2);
            MCPX_VOICE_WRITE(SetCurrentOutBufSGE, dwSgeOffset + z);
            MCPX_VOICE_WRITE(SetOutBufSGEOffset, dwAddress);

            pPrd[dwSgeOffset + z].uAddr = dwAddress;
            pPrd[dwSgeOffset + z].Control.uValue = 0;
            pPrd[dwSgeOffset + z].Control.Owner = NV_PSSL_PRD_CONTROL_OWN_HW;

        }
        
        MCPX_CHECK_VOICE_FIFO_INIT(2);
        MCPX_VOICE_WRITE(SetOutBuf[i].BA, dwSgeOffset * PAGE_SIZE);
        MCPX_VOICE_WRITE(SetOutBuf[i].Len, m_adwGPOutputBufferSizes[i]);

        dwSgeOffset += z;
    }

    //
    // Allocate the scratch space management code and GP DSP code management
    //

    if(!m_pGpDspManager)
    {
        if(!(m_pGpDspManager = NEW(CMcpxGPDspManager)))
        {
            ASSERTMSG("Failed to alloc DSP and scratch classes");
        }

        m_pGpDspManager->Initialize();
    }

    //
    // Take the GP peripherals out of reset, leave DSP core in reset state
    //

    rReset.uValue = 0;
    rReset.Global = NV_PAPU_GPRST_GPRST_DISABLED;
    rReset.DSP = NV_PAPU_GPRST_GPDSPRST_ENABLED;
    rReset.NMI = NV_PAPU_GPRST_GPNMI_DISABLED;
    rReset.Abort = NV_PAPU_GPRST_GPABORT_DISABLED;

    MCPX_REG_WRITE(NV_PAPU_GPRST, rReset.uValue);

    //
    // Enable the GP
    //

    rControl.Idle = NV_PAPU_GPIDRDY_GPSETIDLE_SET;
    rControl.IntrNotify = NV_PAPU_GPIDRDY_GPSETNINT_NOT_SET;

    MCPX_REG_WRITE(NV_PAPU_GPIDRDY, rControl.uValue);

    //
    // Clear the interrupt status
    //

    MCPX_REG_WRITE(NV_PAPU_GPISTS, 0xFF);

    //
    // Reset the FIFO positions
    //

    MCPX_REG_READ(NV_PAPU_GPOFBASE0, &dwPos);
    MCPX_REG_WRITE(NV_PAPU_GPOFCUR0, dwPos);

    MCPX_REG_READ(NV_PAPU_GPOFBASE1, &dwPos);
    MCPX_REG_WRITE(NV_PAPU_GPOFCUR1, dwPos);

    MCPX_REG_READ(NV_PAPU_GPOFBASE2, &dwPos);
    MCPX_REG_WRITE(NV_PAPU_GPOFCUR2, dwPos);

    MCPX_REG_READ(NV_PAPU_GPOFBASE3, &dwPos);
    MCPX_REG_WRITE(NV_PAPU_GPOFCUR3, dwPos);

    //
    // Now take DSP core out of reset as well
    //

    rReset.DSP = NV_PAPU_GPRST_GPDSPRST_DISABLED;

    MCPX_REG_WRITE(NV_PAPU_GPRST, rReset.uValue);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetupEncodeProcessor
 *
 *  Description:
 *      Sets up the encode processor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::SetupEncodeProcessor"

void 
CMcpxCore::SetupEncodeProcessor
(
    void
)
{
    DWORD                   dwTotalSgeCount;
    DWORD                   dwSgeOffset;
    DWORD                   dwAddress;
    R_GP_RESET              rReset;
    R_GP_CONTROL            rControl;
    MCP1_PRD *              pPrd;
    R_GPDMA_CONFIG          rConfig;
    DOLBY_CONFIG_TABLE *    pTable;
    BOOL                    fInitial;
    DWORD                   i, z;

    DPF_ENTER();

    //
    // take EP out of reset, but leave DSP core in reset
    //

    rReset.uValue = 0;
    rReset.Global = NV_PAPU_EPRST_EPRST_DISABLED;
    rReset.DSP = NV_PAPU_EPRST_EPDSPRST_ENABLED;
    rReset.NMI = NV_PAPU_EPRST_EPNMI_DISABLED;
    rReset.Abort = NV_PAPU_EPRST_EPABORT_DISABLED;

    MCPX_REG_WRITE(NV_PAPU_EPRST, rReset.uValue);

    //
    // setup the scratch space management and dsp download 
    //

    if(fInitial = !m_pEpDspManager)
    {
        if(!(m_pEpDspManager = NEW(CMcpxEPDspManager))) 
        {
            ASSERTMSG("Failed to alloc Dsp and scratch classes");
        }

        m_pEpDspManager->Initialize(m_pGpDspManager);
    }
    
    //
    // Output buffer SGE table base address for output buffers
    //

    MCPX_REG_WRITE(NV_PAPU_EPFADDR, m_ctxMemory[MCPX_MEM_EPOUTPUTSGE].PhysicalAddress);

    //
    // Initialize the get/put pointers
    //

    MCPX_REG_WRITE(NV_PAPU_EPGET, 0);
    MCPX_REG_WRITE(NV_PAPU_EPPUT, 0);

    //
    // Tell the hw how big the output SGE array needs to be
    //

    dwTotalSgeCount = m_ctxMemory[MCPX_MEM_EPOUTPUT].Size / PAGE_SIZE;

    ASSERT(!(dwTotalSgeCount & 0xFFFF0000));
	MCPX_REG_WRITE(NV_PAPU_EPFMAXSGE, dwTotalSgeCount - 1);

    //
    // Set up the actual PRDs (aka SGEs) in the hardware-owned PRD table
    //

    pPrd = (MCP1_PRD *)m_ctxMemory[MCPX_MEM_EPOUTPUTSGE].VirtualAddress;

    for(i = 0, dwSgeOffset = 0; i < MCPX_EPOUTPUT_COUNT; i++)
    {
        for(z = 0; z < m_adwEPOutputBufferSizes[i] / PAGE_SIZE; z++)
        {
            dwAddress = m_ctxMemory[MCPX_MEM_EPOUTPUT].PhysicalAddress + ((dwSgeOffset + z) * PAGE_SIZE);
            
            pPrd[dwSgeOffset + z].uAddr = dwAddress;
            pPrd[dwSgeOffset + z].Control.uValue = 0;
            pPrd[dwSgeOffset + z].Control.Owner = NV_PSSL_PRD_CONTROL_OWN_HW;
        }
        
        MCPX_REG_WRITE(NV_PAPU_EPOFBASE0 + (0x10 * i), dwSgeOffset * PAGE_SIZE);
        MCPX_REG_WRITE(NV_PAPU_EPOFEND0 + (0x10 * i), dwSgeOffset * PAGE_SIZE + m_adwEPOutputBufferSizes[i]); 

        dwSgeOffset += z;
    }

    if(fInitial)
    {
        //
        // update the DSP dolby code loader table with the output buffer offsets
        //

        m_pEpDspManager->AC3SetAnalogOutput(MCPX_EPOUTPUT_AC97_ANALOG, m_adwEPOutputBufferSizes[MCPX_EPOUTPUT_AC97_ANALOG]);

        //
        // Now the digital fifo. Its base offset is right after the analog FIFO
        // update the loader table
        //

        m_pEpDspManager->AC3SetDigitalOutput(MCPX_EPOUTPUT_AC97_DIGITAL, m_adwEPOutputBufferSizes[MCPX_EPOUTPUT_AC97_DIGITAL]);

        //
        // add the pages used for multipass buffer output from the GP, into the GP scratch
        // we do this here since the EpDspManager->Initialize() needs to run first and add 
        // its AC3 pages at the end of scratch first. It needs to happen in this order
        //

        m_pGpDspManager->SetMultipassBuffer(&m_ctxMemory[MCPX_MEM_GPMULTIPASS], MCPX_HW_MULTIPASS_NUM_PAGES);   

        //
        // based on the speaker config, setup the dolby DSP code to do the right thing
        //

        if(!DSSPEAKER_IS_SURROUND(m_pSettings->m_dwSpeakerConfig))
        {
            pTable = m_pEpDspManager->GetDolbyConfigTable();

            pTable->do_surround_encode = FALSE;
            pTable->do_game_encode = FALSE;

            if(DSSPEAKER_BASIC(m_pSettings->m_dwSpeakerConfig) == DSSPEAKER_MONO) 
            {
                pTable->do_downmix_encode = 1;
            } 
            else 
            {
                pTable->do_downmix_encode = 2;
            }
        }
    }

    //
    // setup cfg reg
    //

    MCPX_REG_WRITE(NV_PAPU_EPDCFG, 0);

    //
    // setup RDY reg
    //

    rControl.uValue = 0;
    rControl.Idle = NV_PAPU_EPIDRDY_EPSETIDLE_SET;
    rControl.IntrNotify = NV_PAPU_EPIDRDY_EPSETNINT_NOT_SET;

    MCPX_REG_WRITE(NV_PAPU_EPIDRDY, rControl.uValue);
    
    //
    // clear the EPISTS bit
    //

    MCPX_REG_WRITE(NV_PAPU_EPISTS, 0xFFFFFFFF);

    //
    // turn EP dsp core on..
    // BUGBUG nvidia writes a 0x1 as the last thing to turn on the EP (??)
    //

    rReset.DSP = NV_PAPU_EPRST_EPDSPRST_DISABLED;
    MCPX_REG_WRITE(NV_PAPU_EPRST,rReset.uValue);

    DPF_LEAVE_VOID();
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  SetupAc97
 *
 *  Description:
 *      Sets up the AC97.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::SetupAc97"

HRESULT
CMcpxCore::SetupAc97
(
    void
)
{
    HRESULT                 hr              = DS_OK;
    DWORD                   dwBufferAddress;
    LPCDWORD                adwBufferSizes;
    DWORD                   i;

    DPF_ENTER();

#ifndef MCPX_BOOT_LIB

    dwBufferAddress = m_ctxMemory[MCPX_MEM_EPOUTPUT].PhysicalAddress;
    adwBufferSizes = m_adwEPOutputBufferSizes;

#else

    dwBufferAddress = m_ctxMemory[MCPX_MEM_GPOUTPUT].PhysicalAddress;
    adwBufferSizes = m_adwGPOutputBufferSizes;

#endif // MCPX_BOOT_LIB

    //
    // Initialize the AC97
    //

    hr = m_Ac97.Initialize(AC97_OBJECTF_DIRECTISR);

    //
    // Create AC97 output channels.  If Dolby Digital isn't turned on in the
    // speaker config, both the analog and digital channels will read from
    // the analog buffer.
    //

    if(SUCCEEDED(hr))
    {
        hr = m_Ac97.CreateChannel(AC97_CHANNELTYPE_ANALOG);
    }

    if(SUCCEEDED(hr))
    {
        hr = m_Ac97.m_apChannels[AC97_CHANNELTYPE_ANALOG]->Initialize();
    }

    if(SUCCEEDED(hr))
    {
        m_Ac97.m_apChannels[AC97_CHANNELTYPE_ANALOG]->AttachPacket(dwBufferAddress, adwBufferSizes[AC97_CHANNELTYPE_ANALOG]);
    }

    if(SUCCEEDED(hr))
    {
        hr = m_Ac97.CreateChannel(AC97_CHANNELTYPE_DIGITAL);
    }

    if(SUCCEEDED(hr))
    {
        hr = m_Ac97.m_apChannels[AC97_CHANNELTYPE_DIGITAL]->Initialize();
    }

    if(SUCCEEDED(hr))
    {

#ifndef MCPX_BOOT_LIB

        if(DSSPEAKER_IS_AC3(m_pSettings->m_dwSpeakerConfig))
        {
            m_Ac97.m_apChannels[AC97_CHANNELTYPE_DIGITAL]->AttachPacket(dwBufferAddress + adwBufferSizes[AC97_CHANNELTYPE_ANALOG], adwBufferSizes[AC97_CHANNELTYPE_DIGITAL]);
            m_Ac97.m_apChannels[AC97_CHANNELTYPE_DIGITAL]->SetMode(DSAC97_MODE_ENCODED);
        }
        else

#endif // MCPX_BOOT_LIB

        {
            m_Ac97.m_apChannels[AC97_CHANNELTYPE_DIGITAL]->AttachPacket(dwBufferAddress, adwBufferSizes[AC97_CHANNELTYPE_ANALOG]);
            m_Ac97.m_apChannels[AC97_CHANNELTYPE_DIGITAL]->SetMode(DSAC97_MODE_PCM);
        }
    }

    DPF_LEAVE_HRESULT(hr);

	return hr;
}


/****************************************************************************
 *
 *  SetInterruptState
 *
 *  Description:
 *      Sets the APU interrupt state.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to enable interrupts, FALSE to disable.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::SetInterruptState"

void 
CMcpxCore::SetInterruptState
(
    BOOL                    fEnabled
)
{
    R_INTR                  rInterruptMask;

    DPF_ENTER();
    AutoIrql();

    MCPX_REG_READ(NV_PAPU_IEN, &rInterruptMask.uValue);

    rInterruptMask.General = fEnabled ? NV_PAPU_IEN_GINTEN_ENABLED : NV_PAPU_IEN_GINTEN_DISABLED;

    MCPX_REG_WRITE(NV_PAPU_IEN, rInterruptMask.uValue);

    // DPF_BLAB("Interrupts %s", fEnabled ? "enabled" : "disabled");

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetPrivLockState
 *
 *  Description:
 *      Sets the APU priv lock (?) state.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to lock, FALSE to unlock.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::SetPrivLockState"

void 
CMcpxCore::SetPrivLockState
(
    BOOL                    fLocked
)
{
    DWORD                   dwPrivLock;

    DPF_ENTER();
    AutoIrql();

    MCPX_REG_READ(NV_PAPU_FEPRIVLOCK, &dwPrivLock);

    dwPrivLock = MCPX_SET_REG_VALUE(dwPrivLock, fLocked ? NV_PAPU_FEPRIVLOCK_VALUE_LOCKED : NV_PAPU_FEPRIVLOCK_VALUE_UNLOCKED, NV_PAPU_FEPRIVLOCK_VALUE);

    MCPX_REG_WRITE(NV_PAPU_FEPRIVLOCK, dwPrivLock);

    // DPF_BLAB("Priv lock %s", fLocked ? "locked" : "unlocked");

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetFrontEndState
 *
 *  Description:
 *      Sets the front-end state.
 *
 *  Arguments:
 *      MCPX_FE_STATE [in]: state.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::SetFrontEndState"

void 
CMcpxCore::SetFrontEndState
(
    MCPX_FE_STATE           nState
)
{
    R_FE_CONTROL            rFeControl;

    DPF_ENTER();
    AutoIrql();

    MCPX_REG_READ(NV_PAPU_FECTL, &rFeControl.uValue);

    switch(nState) 
    {
        case MCPX_FE_STATE_HALTED:
            // DPF_BLAB("FE state set to HALTED");
            
            rFeControl.Mode = NV_PAPU_FECTL_FEMETHMODE_HALTED;

            break;

        case MCPX_FE_STATE_FREE_RUNNING:
            // DPF_BLAB("FE state set to FREE_RUNNING");
            
            rFeControl.Mode = NV_PAPU_FECTL_FEMETHMODE_FREE_RUNNING;

            break;

        case MCPX_FE_STATE_ISO:
            // DPF_BLAB("FE state set to ISO");
            
            rFeControl.WriteISO = NV_PAPU_FECTL_FEMWTYP_ISO;
            rFeControl.ReadISO = NV_PAPU_FECTL_FEMRTYP_ISO;
            rFeControl.NotifyISO = NV_PAPU_FECTL_FENTYP_ISO;
            rFeControl.PIOClass = NV_PAPU_FECTL_FEPIOCLASS_ISO;

            break;

        case MCPX_FE_STATE_NON_ISO:
            // DPF_BLAB("FE state set to NON_ISO");
            
            rFeControl.WriteISO = NV_PAPU_FECTL_FEMWTYP_NON_ISO;
            rFeControl.ReadISO = NV_PAPU_FECTL_FEMRTYP_NON_ISO;
            rFeControl.NotifyISO = NV_PAPU_FECTL_FENTYP_NON_ISO;
            rFeControl.PIOClass = NV_PAPU_FECTL_FEPIOCLASS_NON_ISO;

            break;

        default:
            ASSERTMSG("Invalid FE state");

            break;
    }

    MCPX_REG_WRITE(NV_PAPU_FECTL, rFeControl.uValue);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetSetupEngineState
 *
 *  Description:
 *      Sets the setup engine state.
 *
 *  Arguments:
 *      MCPX_SE_STATE [in]: state.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::SetSetupEngineState"

void 
CMcpxCore::SetSetupEngineState
(
    MCPX_SE_STATE           nState
)
{
    R_SE_CONTROL            rSeControl;

    DPF_ENTER();
    AutoIrql();

    MCPX_REG_READ(NV_PAPU_SECTL, &rSeControl.uValue);

    switch(nState)
    {
        case MCPX_SE_STATE_OFF:
            // DPF_BLAB("SE state set to OFF");
            
            rSeControl.GSCUpdate = NV_PAPU_SECTL_XCNTMODE_OFF;

            break;

        case MCPX_SE_STATE_AC_SYNC:
            // DPF_BLAB("SE state set to AC_SYNC");
            
            rSeControl.GSCUpdate = NV_PAPU_SECTL_XCNTMODE_AC_SYNC;

            break;

        case MCPX_SE_STATE_SW:
            // DPF_BLAB("SE state set to SW");
            
            rSeControl.GSCUpdate = NV_PAPU_SECTL_XCNTMODE_SW;

            break;

        case MCPX_SE_STATE_FREE_RUNNING:
            // DPF_BLAB("SE state set to FREE_RUNNING");
            
            rSeControl.GSCUpdate = NV_PAPU_SECTL_XCNTMODE_FREE_RUNNING;

            break;

        case MCPX_SE_STATE_ISO:
            // DPF_BLAB("SE state set to ISO");
            
            rSeControl.SampleReadISO = NV_PAPU_SECTL_SESRTYP_ISO;
            rSeControl.WriteISO = NV_PAPU_SECTL_SEPWTYP_ISO;
            rSeControl.ReadISO = NV_PAPU_SECTL_SEPRTYP_ISO;

            break;

        case MCPX_SE_STATE_NON_ISO:
            // DPF_BLAB("SE state set to NON_ISO");
            
            rSeControl.SampleReadISO = NV_PAPU_SECTL_SESRTYP_NON_ISO;
            rSeControl.WriteISO = NV_PAPU_SECTL_SEPWTYP_NON_ISO;
            rSeControl.ReadISO = NV_PAPU_SECTL_SEPRTYP_NON_ISO;

            break;

        default:
            ASSERTMSG("Invalid SE state");

            break;
    }

    MCPX_REG_WRITE(NV_PAPU_SECTL, rSeControl.uValue);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  GetPhysicalMemoryProperties
 *
 *  Description:
 *      Gets the physical address of a memory buffer, the byte offset into 
 *      the first page and the count of physically contiguous bytes in the
 *      buffer.
 *
 *  Arguments:
 *      LPVOID [in]: buffer virtual address.
 *      DWORD [in]: buffer size.
 *      LPDWORD [out]: page offset.
 *      LPDWORD [out]: contiguous length, in bytes.
 *
 *  Returns:  
 *      DWORD: buffer physical address.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::GetPhysicalMemoryProperties"

DWORD
CMcpxCore::GetPhysicalMemoryProperties
(
    LPVOID                  pvBuffer,
    DWORD                   dwBufferSize,
    LPDWORD                 pdwPageOffset,
    LPDWORD                 pdwContiguousLength
)
{
    DWORD                   dwBasePhysicalAddress;
    DWORD                   dwBasePageOffset;
    DWORD                   dwContiguousLength;
    DWORD                   dwPhysicalAddress;

    DPF_ENTER();

    //
    // Get the base physical address
    //

    dwBasePhysicalAddress = MmGetPhysicalAddress(pvBuffer);

    //
    // Get the base page offset
    //

    dwBasePageOffset = BYTE_OFFSET(pvBuffer);

    if(pdwPageOffset)
    {
        *pdwPageOffset = dwBasePageOffset;
    }

    //
    // Find the count of contiguous bytes in the remaining pages
    //

    if(pdwContiguousLength)
    {
        dwContiguousLength = PAGE_SIZE - dwBasePageOffset; 

        while(dwContiguousLength < dwBufferSize)
        {
            dwPhysicalAddress = MmGetPhysicalAddress((LPBYTE)pvBuffer + dwContiguousLength);
            ASSERT(!(dwPhysicalAddress & (PAGE_SIZE - 1)));

            if(dwBasePhysicalAddress + dwContiguousLength != dwPhysicalAddress)
            {
                break;
            }

            dwContiguousLength += PAGE_SIZE;
        }

        *pdwContiguousLength = min(dwContiguousLength, dwBufferSize);
    }

    DPF_LEAVE(dwBasePhysicalAddress);

    return dwBasePhysicalAddress;
}


/****************************************************************************
 *
 *  MapTransfer
 *
 *  Description:
 *      Replacement for IoMapTransfer.
 *
 *  Arguments:
 *      LPVOID * [in/out]: buffer virtual address.
 *      LPDWORD [in/out]: buffer size, in bytes.
 *      LPDWORD [out]: count of bytes mapped.
 *
 *  Returns:  
 *      DWORD: buffer physical address.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::MapTransfer"

DWORD
CMcpxCore::MapTransfer
(
    LPVOID *                ppvBuffer,
    LPDWORD                 pdwBufferSize,
    LPDWORD                 pdwBytesMapped
)
{
    DWORD                   dwPhysicalAddress;
    DWORD                   dwPageOffset;
    DWORD                   dwBytesMapped;

    DPF_ENTER();

    dwPhysicalAddress = GetPhysicalMemoryProperties(*ppvBuffer, *pdwBufferSize, &dwPageOffset, NULL);

    dwBytesMapped = min(PAGE_SIZE - dwPageOffset, *pdwBufferSize);

    *ppvBuffer = (LPBYTE)*ppvBuffer + dwBytesMapped;
    *pdwBufferSize -= dwBytesMapped;

    if(pdwBytesMapped)
    {
        *pdwBytesMapped = dwBytesMapped;
    }

    DPF_LEAVE(dwPhysicalAddress);

    return dwPhysicalAddress;
}


/****************************************************************************
 *
 *  AllocateContext
 *
 *  Description:
 *      Allocates physically contiguous memory.
 *
 *  Arguments:
 *      PMCPX_ALLOC_CONTEXT [in/out]: allocation context data.
 *      DWORD [in]: allocation size, in bytes.
 *      DWORD [in]: allocation alignment, in bytes.
 *      DWORD [in]: allocation flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::AllocateContext"

HRESULT
CMcpxCore::AllocateContext
(
    PMCPX_ALLOC_CONTEXT     pContext,
    DWORD                   dwAlignment,
    DWORD                   dwFlags
)
{
    HRESULT                 hr;
    
    DPF_ENTER();

    ASSERT(!pContext->VirtualAddress);
    ASSERT(!pContext->PhysicalAddress);
    ASSERT(pContext->Size);
    ASSERT(dwAlignment);

    if(SUCCEEDED(hr = HRFROMP(pContext->VirtualAddress = PHYSALLOC(BYTE, pContext->Size, dwAlignment, dwFlags))))
    {
        pContext->PhysicalAddress = MmGetPhysicalAddress(pContext->VirtualAddress);
        ASSERT(!(pContext->PhysicalAddress & (dwAlignment - 1)));
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  ResetGlobalCounters
 *
 *  Description:
 *      Resets the APU global counters.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::ResetGlobalCounters"

void
CMcpxCore::ResetGlobalCounters
(
    void
)
{
    static const DWORD      dwDelta = m_adwEPOutputBufferSizes[MCPX_EPOUTPUT_AC97_ANALOG] / 4;

    DPF_ENTER();

    MCPX_REG_WRITE(NV_PAPU_IGSCNT, dwDelta); 
    MCPX_REG_WRITE(NV_PAPU_XGSCNT, 0);
    MCPX_REG_WRITE(NV_PAPU_DGSCNT, dwDelta);
    MCPX_REG_WRITE(NV_PAPU_WGSCNT, dwDelta / 2);
    MCPX_REG_WRITE(NV_PAPU_RGSCNT, dwDelta - 1);
    MCPX_REG_WRITE(NV_PAPU_PGSCNT, 0);

    MCPX_REG_WRITE(NV_PAPU_EGSCNT, 6 * MCPX_HW_EP_STEP_SIZE);
    MCPX_REG_WRITE(NV_PAPU_DEGSCNT, MCPX_HW_EP_STEP_SIZE);
    MCPX_REG_WRITE(NV_PAPU_ECNTSTP, MCPX_HW_EP_STEP_SIZE);

    MCPX_REG_WRITE(NV_PAPU_EPOFCUR0, 1536);
    MCPX_REG_WRITE(NV_PAPU_EPOFCUR1, m_adwEPOutputBufferSizes[MCPX_EPOUTPUT_AC97_ANALOG] + 6144);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  IdleVoiceProcessor
 *
 *  Description:
 *      Puts the VP into an idle state, or restores it from one.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to idle, FALSE to run.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxCore::IdleVoiceProcessor"

BOOL
CMcpxCore::IdleVoiceProcessor
(
    BOOL                    fIdle
)
{
    DWORD                   dwTimeout   = MCPX_HW_FRAME_LENGTH_US;
    R_FE_CONTROL            rFeControl;
    R_SE_CONTROL            rSeControl;
    CMcpxNotifier           Notifier;

    DPF_ENTER();
    AutoIrql();

    if(fIdle)
    {
        //
        // Check the current FE and SE states.  If they're not running, we
        // don't need to idle the VP.
        //

        fIdle = FALSE;
        
        MCPX_REG_READ(NV_PAPU_FECTL, &rFeControl.uValue);
        
        if(NV_PAPU_FECTL_FEMETHMODE_FREE_RUNNING == rFeControl.Mode)
        {
            MCPX_REG_READ(NV_PAPU_SECTL, &rSeControl.uValue)
    
            if(NV_PAPU_SECTL_XCNTMODE_OFF != rSeControl.GSCUpdate)
            {
                fIdle = TRUE;
    
                //
                // Turn off the VP
                //
    
                MCPX_CHECK_VOICE_FIFO(1);
                MCPX_VOICE_WRITE(SetProcessorMode, NV1BA0_PIO_SET_MODE_PARAMETER_OFF);

                //
                // Wait for the VP to tell us it's idle
                //

                if(m_ctxMemory[MCPX_MEM_NOTIFIERS].VirtualAddress)
                {
                    Notifier.Initialize(0, 1);
    
                    MCPX_VOICE_WRITE(Synchronize, NV1BA0_PIO_SYNCHRONIZE_PARAMETER_WAIT_FOR_IDLE_WRITE_PE_NOTIFY);

                    while(!Notifier.GetStatus(0))
                    {
                        KeStallExecutionProcessor(1);

                        if(!dwTimeout--)
                        {
                            break;
                        }
                    }

                    Notifier.Reset();
                }
                else
                {
                    KeStallExecutionProcessor(dwTimeout);
                }
            }
        }
    }
    else
    {
        //
        // Turn the VP back on
        //
        
        MCPX_CHECK_VOICE_FIFO(1);
        MCPX_VOICE_WRITE(SetProcessorMode, NV1BA0_PIO_SET_MODE_PARAMETER_RUN);
    }        

    DPF_LEAVE(fIdle);

    return fIdle;
}


