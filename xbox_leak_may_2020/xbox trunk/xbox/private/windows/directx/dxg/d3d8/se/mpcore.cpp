/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mpcore.cpp
 *  Content:    Hardware initialization
 *
 ***************************************************************************/

#include "precomp.hpp"

#include <pci.h>
#include <conio.h>

// Optimize this module for size (favor code space)

#pragma optimize("s", on)

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

const float F_XTAL_135 = 13.5f;
const float F_XTAL_166 = 16.6667f;


BOOL
CMiniport::InitHardware()
{

    ULONG Index;
    ULONG InterruptVector;
    KIRQL InterruptIrql;
    NTSTATUS status;
    PCI_SLOT_NUMBER PCISlotNumber;
    ULONG AGPCmd;

    KeInitializeDpc(&m_Dpc, CMiniport::Dpc, this);
    KeInitializeEvent(&m_BusyBlockEvent, NotificationEvent, TRUE);
    KeInitializeEvent(&m_VerticalBlankEvent, NotificationEvent, TRUE);

    //
    // Map the NV registers
    //

    if (!MapRegisters())
    {
        DXGRIP("MP: Failed to map NV registers");
        return FALSE;
    }


    if (!GetGeneralInfo())
    {
        DXGRIP("MP: Unable to get chip info");
        return FALSE;
    }

    //
    // Connect to the device's interrupt.
    //
    InterruptVector = HalGetInterruptVector(XPCICFG_GPU_IRQ, &InterruptIrql);

    KeInitializeInterrupt(&m_InterruptObject, CMiniport::Isr, this,
                          InterruptVector, InterruptIrql, LevelSensitive, TRUE);

    if (!KeConnectInterrupt(&m_InterruptObject)) {
        DXGRIP("MP: Unable to connect interrupt");
        return FALSE;
    }

    //
    // Register for notification when the system is shutting down so that we can
    // shutdown the hardware
    //
    m_ShutdownRegistration.NotificationRoutine = CMiniport::ShutdownNotification;
    m_ShutdownRegistration.Priority = 0;

    HalRegisterShutdownNotification(&m_ShutdownRegistration, TRUE);

    //
    // Do one time initialization
    //
    if (!InitEngines())
    {
        DXGRIP("MP: InitEngines failed");
        return FALSE;
    }

    //
    // Initialize the gamma ramps.
    //
    for (DWORD Ramp = 0; Ramp < 2; Ramp++)
    {
        D3DGAMMARAMP* GammaRamp = &(m_GammaRamp[Ramp]);

        for (DWORD i = 0; i < 256; i++)
        {
            GammaRamp->red[i] = (BYTE) i;
            GammaRamp->green[i] = (BYTE) i;
            GammaRamp->blue[i] = (BYTE) i;
        }
    }

    //
    // Set up the field GPIO pin.
    //

    _outp(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + 0xC0, 0x01);

    //
    // Setup the AGP read request queue.  The kernel initialization does not set
    // this to a optimal value.  We want to the GPU reads from AGP to be as 
    // fast as possible.  Read in the value set by the kernel, or-in the read
    // request, and write it back out.
    //

    PCISlotNumber.u.AsULONG = 0;
    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_GPU_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_GPU_FUNCTION_ID;

    HalReadPCISpace(1, PCISlotNumber.u.AsULONG, 0x4C, &AGPCmd, sizeof(AGPCmd));
    
    AGPCmd |= 0x1F000000;

    HalWritePCISpace(1, PCISlotNumber.u.AsULONG, 0x4C, &AGPCmd, sizeof(AGPCmd));


    //
    // Ready to handle interrupts
    //
    m_InterruptsEnabled = TRUE;

    //
    // Load engines
    //
    if (!LoadEngines())
    {
        DXGRIP("MP: LoadEngines failed");
        return FALSE;
    }

    //
    // Dump out various speed numbers
    //
    DumpClocks();


#if PROFILE
    PCI_SLOT_NUMBER SlotNumber;
    DWORD MPLLCoeff;
    DWORD m;
    DWORD n;
    float XTAL;
    DWORD cpuspeed;

    if (m_GenInfo.CrystalFreq == CRYSTAL_166) 
    {
        XTAL = F_XTAL_166;
    }
    else
    {
        XTAL = F_XTAL_135;
    }

    //
    // Read CR_CPU_MPLL_COEFF 
    //
    SlotNumber.u.AsULONG = 0;
    SlotNumber.u.bits.DeviceNumber = 0;
    SlotNumber.u.bits.FunctionNumber = 3;
    HalReadPCISpace(0, SlotNumber.u.AsULONG, 0x6c, &MPLLCoeff, sizeof(MPLLCoeff));

    m = MPLLCoeff & 0xFF;
    n = (MPLLCoeff >> 8) & 0xFF;

    if (m != 0)
    {
        cpuspeed = (DWORD)FloatToLong((XTAL / m) * n * 11 / 2);
        g_CpuCyclesPerMsec = cpuspeed * 1000;
    }
#endif


    return TRUE;
}


#if DBG

VOID
CMiniport::DumpClocks()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    PCI_SLOT_NUMBER SlotNumber;
    DWORD MPLLCoeff;
    DWORD m;
    DWORD n;
    DWORD p;
    DWORD fsbspeed;
    DWORD vcofreq;
    DWORD nvclk;
    DWORD nvpll;
    DWORD fsb_pdiv;
    DWORD mem_pdiv;
    DWORD mclk;
    DWORD cpuspeed;
    float XTAL;

    if (m_GenInfo.CrystalFreq == CRYSTAL_166) 
    {
        XTAL = F_XTAL_166;
    }
    else
    {
        XTAL = F_XTAL_135;
    }

    //
    // Read CR_CPU_MPLL_COEFF 
    //
    SlotNumber.u.AsULONG = 0;
    SlotNumber.u.bits.DeviceNumber = 0;
    SlotNumber.u.bits.FunctionNumber = 3;
    HalReadPCISpace(0, SlotNumber.u.AsULONG, 0x6c, &MPLLCoeff, sizeof(MPLLCoeff));

    m = MPLLCoeff & 0xFF;
    n = (MPLLCoeff >> 8) & 0xFF;

    if (m != 0)
    {
        //
        // Calculate cpu frequency
        //
        fsbspeed = (DWORD)FloatToLong((XTAL / m) * n);

        cpuspeed = (DWORD)FloatToLong((XTAL / m) * n * 11 / 2);
    
        //
        // Calculate nvclk
        //
        nvpll = REG_RD32(RegisterBase, NV_PRAMDAC_NVPLL_COEFF);
        m = nvpll & 0xFF;
        n = (nvpll >> 8)  & 0xFF;
        p = (nvpll >> 16) & 0xFF;
    
        nvclk = (m != 0) ? (DWORD)FloatToLong((n * XTAL / (1 << p) / m)) : 0;
    
        //
        // Calculate vco
        //
        m = MPLLCoeff & 0xFF;
        n = (MPLLCoeff >> 8) & 0xFF;
        fsb_pdiv = (MPLLCoeff >> 16) & 0xF;
        
        vcofreq = (DWORD)FloatToLong((XTAL / m) * (fsb_pdiv * 2 * n));
    
        //
        // Calculate mclk
        //
        mem_pdiv = (MPLLCoeff >> 20) & 0xF;
        mclk = (DWORD)(vcofreq / (2 * mem_pdiv));

        DbgPrint("Crystal=%s  FSB=%d  CPU=%d  NVCLK=%d  VCO=%d  MCLK=%d\n",
            (m_GenInfo.CrystalFreq == CRYSTAL_166) ? "16.6" : "13.5",
            fsbspeed, cpuspeed, nvclk, vcofreq, mclk);
    }
    else
    {
        DXGRIP("MP: Failed to display clock speeds because m=0");
    }
                
}
#endif


BOOL
CMiniport::MapRegisters()
{
    BYTE* RegisterBase;
    ULONG CurrentValue;

    //
    // Map GPU register space
    //

    m_RegisterBase = (PVOID)XPCICFG_GPU_MEMORY_REGISTER_BASE_0;
    RegisterBase = (BYTE*)XPCICFG_GPU_MEMORY_REGISTER_BASE_0;

    //
    // Enable bus mastering using CONFIG_PCI_NV1 register
    //

    CurrentValue = REG_RD32(RegisterBase, NV_PBUS_PCI_NV_1);
    CurrentValue |= (1 << DRF_SHIFT(NV_PBUS_PCI_NV_1_BUS_MASTER));
    REG_WR32(RegisterBase,  NV_PBUS_PCI_NV_1, CurrentValue);

    //
    // Disable VBLANK interrupt on startup
    //

    REG_WR32(RegisterBase, NV_PCRTC_INTR_EN_0, 0x00000000);

    //
    // Disable TIMER interrupt
    //

    REG_WR32(RegisterBase, NV_PTIMER_INTR_EN_0, 0x00000000);

    return TRUE;
}



BOOL
CMiniport::GetGeneralInfo()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    //
    // Get the chip ID, look at bits 16-31 and ignore revision number
    //

    m_GenInfo.ChipId = REG_RD32(RegisterBase, NV_PBUS_PCI_NV_0);
    m_GenInfo.ChipId >>= 16;
    m_GenInfo.ChipId &= 0xfffc;

    //
    // Get the mask revision.  There's a bug in nv20 where PMC_BOOT_0 (on A02 silicon)
    // isn't accurate.  So we get the maskrev from the pci config register
    //

    m_GenInfo.MaskRevision = REG_RD_DRF(RegisterBase, _PBUS, _PCI_NV_2, _REVISION_ID);

    //
    // Get video memory (aka frame buffer) size
    //

    m_GenInfo.VideoRamSize = REG_RD32(RegisterBase, NV_PFB_CSTATUS);

    //
    // Setup for crystal 16.6667 for DVT4 boards and later.  For correct operation
    // on DVT3 and earlier boxes this should be set to CRYSTAL_135.
    //
    m_GenInfo.CrystalFreq = CRYSTAL_166;

    return TRUE;
}



ULONG
CMiniport::ReserveInstMem(
    IN ULONG blocks
    )
{
    ASSERT(blocks <= (INSTANCE_MEM_MAXSIZE >> 4) - m_FreeInstAddr);

    ULONG instance = m_FreeInstAddr;
    m_FreeInstAddr += blocks;

    return instance;
}



BOOL
CMiniport::InitEngines()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    //
    // Disable ROM
    //
    
    REG_WR32(RegisterBase, NV_PBUS_PCI_NV_12, 0);

    //
    // Max out the chip's PCI latency timer
    //
    
    REG_WR32(RegisterBase, NV_PBUS_PCI_NV_3,
             DRF_NUM(_PBUS,_PCI_NV_3,_LATENCY_TIMER, NV_PBUS_PCI_NV_3_LATENCY_TIMER_248_CLOCKS));

    HalMcControlInit();

    m_GenInfo.ChipIntrEn0 = DRF_DEF(_PMC, _INTR_EN_0, _INTA, _HARDWARE);

    //
    // Calculate numerator and denomenator from DAC NVCLK parameters.
    //
    
    INT num = m_DacInfo.NVClk;
    INT den = 31250000;
    while ((num % 2 == 0) && (den % 2 == 0))
    {
        num /= 2;
        den /= 2;
    }
    while ((num % 5 == 0) && (den % 5 == 0))
    {
        num /= 5;
        den /= 5;
    }
    while ((num > 2 * DRF_MASK(NV_PTIMER_NUMERATOR_VALUE))
        || (den > 2 * DRF_MASK(NV_PTIMER_DENOMINATOR_VALUE)))
    {
        num /= 2;
        den /= 2;
    }
    num = (num + 1) / 2;
    den = (den + 1) / 2;

    REG_WR32(RegisterBase, NV_PTIMER_NUMERATOR,   num);
    REG_WR32(RegisterBase, NV_PTIMER_DENOMINATOR, den);
    REG_WR32(RegisterBase, NV_PTIMER_ALARM_0,     0xFFFFFFFF);

    HalFbControlInit();

    HalDacControlInit();

    //
    // Initialize the first gamma ramp to identity.  We only
    // do the one which corresponds to m_GammaCurrentIndex
    //
    InitGammaRamp(0);

    HalVideoControlInit();

    m_HalInfo.GrChID = NUM_FIFOS_NV20;
    HalGrControlInit();

    HalFifoControlInit();

    return TRUE;
}




BOOL
CMiniport::LoadEngines()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    //
    // Enable all devices and interrupts.
    //
    
    ULONG data = REG_RD32(RegisterBase, NV_PBUS_PCI_NV_19);     // read AGP COMMAND
    REG_WR32(RegisterBase, NV_PBUS_PCI_NV_19,
             (data & ~(DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_SBA_ENABLE, _ON) |
                       DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_AGP_ENABLE, _ON))));

    (VOID)(REG_RD32(RegisterBase, NV_PRAMIN_DATA032(0)));  // read inst mem to delay

    REG_WR32(RegisterBase, NV_PBUS_PCI_NV_19, data);      // restore

    //
    // Enable all engines
    //
    REG_WR32(RegisterBase, NV_PMC_ENABLE, 0xFFFFFFFF);
    EnableInterrupts(RegisterBase);

    HalDacLoad();


    LARGE_INTEGER SysTime;
    TIME_FIELDS TimeField;
    ULONG days;
    ULONG sec5;

    static const BYTE tmrMonthDays[12] =
    {
        31, 28, 31, 30,
        31, 30, 31, 31,
        30, 31, 30, 31
    };

    //
    // Calculate time in a format that the tmr expects (in 4.295 sec increments between 1970)
    //

    KeQuerySystemTime(&SysTime);
    RtlTimeToTimeFields(&SysTime, &TimeField);

    if (TimeField.Year > 1990)
        TimeField.Year -= 1990;
    days = TimeField.Year * 365 + (TimeField.Year + 1) / 4;

    if (!((TimeField.Year + 2) & 3) && (TimeField.Month > 2))
        days++;
    while (--TimeField.Month)
        days += tmrMonthDays[TimeField.Month];
    days += TimeField.Day;

    sec5 = days * 20117 + TimeField.Hour * 838 + (TimeField.Minute * 257698 + TimeField.Second * 4295) / 1000;

    // Add number of 4.295 sec increments between 1970 and 1990
    sec5 += 146951526;

    //
    // Since the internal timer is only accurate to about 5 sec/day,
    // that is about the accuracy of the upper 32 bits of the nsec timer.
    // To initialize, just calculate the current time to the nearest 4.295 sec.
    //
    REG_WR32(RegisterBase, NV_PTIMER_TIME_0, 0);
    REG_WR32(RegisterBase, NV_PTIMER_TIME_1, sec5);

    //
    // Disable data from fifo.
    //
    REG_WR32(RegisterBase, NV_PGRAPH_FIFO, 0x0);

    HalGrControlLoad();

    //
    // Reset and enabled the interrupts.
    //
    REG_WR32(RegisterBase, NV_PGRAPH_INTR, 0xFFFFFFFF);
    REG_WR32(RegisterBase, NV_PGRAPH_INTR_EN, 0xFFFFFFFF);

    HalFifoControlLoad();

    //
    // Enable all PFIFO interrupt conditions using value
    // setup by HAL (see FIFO_CONTROL_INIT code for each chip).
    //
    REG_WR32(RegisterBase, NV_PFIFO_INTR_0,     0xFFFFFFFF);
    REG_WR32(RegisterBase, NV_PFIFO_INTR_EN_0,  m_HalInfo.FifoIntrEn0);


    return TRUE;
}



VOID
CMiniport::InitGammaRamp(
    ULONG RampNo
    )
{
    D3DGAMMARAMP* GammaRamp = &(m_GammaRamp[RampNo]);
    for (DWORD i = 0; i < 256; i++)
    {
        GammaRamp->red[i] = (BYTE) i;
        GammaRamp->green[i] = (BYTE) i;
        GammaRamp->blue[i] = (BYTE) i;
    }
}


VOID
CMiniport::GetAddressInfo(
    PVOID pLinearAddress,
    PVOID* ppAddress,
    ULONG* pAddressSpace,
    BOOL IsAlias
    )
{
    AssertContiguousOrPhysical(pLinearAddress);

    if (IsAlias) 
    {
        //
        // Alias memory is treated as AGP memory for the GPU
        //
        *ppAddress = (VOID*)(GetPhysicalOffset(pLinearAddress) | 0x40000000);
        *pAddressSpace = ADDR_AGPMEM;
    }
    else if (!IsContiguousMemory(pLinearAddress))
    {
        //
        // 0-64 MB range is treated as video memory
        //
        *ppAddress = pLinearAddress;
        *pAddressSpace = ADDR_FBMEM;
    }
    else
    {
        //
        // System memory.
        //
        *ppAddress = (PVOID)GetGPUAddress(pLinearAddress);
        *pAddressSpace = ADDR_SYSMEM;
    }
}


BOOL
CMiniport::CreateCtxDmaObject(
    IN  ULONG Dma,
    IN  ULONG ClassNum,
    IN  PVOID Base,
    IN  ULONG Limit,
    OUT OBJECTINFO* Object
    )
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    ULONG RegValue = 0;
    PVOID Address = NULL;
    ULONG AddressSpace = 0;
    ULONG PteInfo;
    ULONG PteAdjust;

    //
    // Get address information
    //
    
#ifdef ALIASPUSHER
    BOOL IsAlias = (Dma == NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY);
#else
    BOOL IsAlias = FALSE;
#endif

    GetAddressInfo(Base, &Address, &AddressSpace, IsAlias);
    
    ASSERT((AddressSpace == ADDR_FBMEM || AddressSpace == ADDR_AGPMEM) || AddressSpace == ADDR_SYSMEM);
    PteInfo = (ULONG)(Address) | 3;

    //
    // Allocate instance memory.  VidMem or Agp take 1 inst mem unit (16 bytes)
    //
    ULONG Instance = ReserveInstMem(1);

    switch (ClassNum)
    {
        case NV01_CONTEXT_DMA_TO_MEMORY:
            RegValue |= SF_NUM(_DMA, _CLASS, NV1_CONTEXT_DMA_TO_MEMORY);
            break;

        case NV01_CONTEXT_DMA_FROM_MEMORY:
            RegValue |= SF_NUM(_DMA, _CLASS, NV1_CONTEXT_DMA_FROM_MEMORY);
            break;

        case NV01_CONTEXT_DMA_IN_MEMORY:
            RegValue |= SF_NUM(_DMA, _CLASS, NV1_CONTEXT_DMA_IN_MEMORY);
            break;
        default:
            NODEFAULT("Invalid class specified for DMA context\n");
    }

    PteAdjust = (ULONG)((ULONG)Address & 0x0FFF);

    RegValue |= SF_NUM(_DMA, _ADJUST, PteAdjust);

    RegValue |= SF_DEF(_DMA, _PAGE_ENTRY, _LINEAR);
    RegValue |= SF_DEF(_DMA, _PAGE_TABLE, _PRESENT);

    if (AddressSpace == ADDR_FBMEM)
    {
        // Video memory
        RegValue |= SF_DEF(_DMA, _TARGET_NODE, _NVM);
    }
    else if (AddressSpace == ADDR_AGPMEM)
    {
        // AGP memory
        RegValue |= SF_DEF(_DMA, _TARGET_NODE, _AGP);
    }
    else if (AddressSpace == ADDR_SYSMEM)
    {
        // PCI memory
        RegValue |= SF_DEF(_DMA, _TARGET_NODE, _PCI);
    }

    // BUGBUG: Verify if we still need to do this
    //
    // Workaround a bug in the HOST's semaphore context dma checks.
    // It only accepts the SetContextDmaSemaphore method if the
    // ctxdma is marked COHERENCY_CACHED (even though the underlying
    // memory is NODE_NVM).  Since no other engine checks the
    // coherency flags (yet), this should be OK for now.
    //
    RegValue |= SF_DEF(_DMA, _FLAGS_MAPPING_COHERENCY, _CACHED);

    //
    // Write to instance memory
    //
    INST_WR32(RegisterBase, Instance, 8, PteInfo);
    INST_WR32(RegisterBase, Instance, 12, PteInfo);

    INST_WR32(RegisterBase, Instance, SF_OFFSET(NV_DMA_CLASS), RegValue);
    INST_WR32(RegisterBase, Instance, SF_OFFSET(NV_DMA_LIMIT), Limit);

    //
    // Fill in the object fields
    //
    Object->Init();
    Object->Handle = Dma;
    Object->ClassNum = ClassNum;
    Object->Instance = Instance;
    Object->Engine = 0;//FIFO_ENGINE_TAG;

    return TRUE;
}

//-----------------------------------------------------------------------------
// Debug-only routine for modifying a DMA context object to be limited
// to the bounds of a specified surface.  Handy for catching cases where
// the GPU might draw out of bounds, which in a UMA system can be a
// rather nasty occurence.
//
// Derived from CreateCtxDmaObject
//
// Can modify only IN_MEMORY, video-memory contexts
//
// Returns the new base address of the memory context (all offsets fed to
// the GPU must be adjusted by this amount)

#if DBG && ENABLE_DMA_RANGE

DWORD
CMiniport::SetDmaRange(
    IN ULONG DmaContext,
    IN D3DSurface* pSurface // Can be NULL
    )
{
    ULONG RegValue = 0;
    PVOID Address = NULL;
    ULONG AddressSpace = 0;
    DWORD Limit;
    VOID* Base;
    DWORD Pitch;

    ULONG PteInfo;
    ULONG PteAdjust;
    ULONG Instance;

    CDevice* pDevice = g_pDevice;

    switch (DmaContext)
    {
    case D3D_COLOR_CONTEXT_DMA_IN_VIDEO_MEMORY:
        Instance = pDevice->m_ColorContextDmaInstance;
        break;

    case D3D_ZETA_CONTEXT_DMA_IN_VIDEO_MEMORY:
        Instance = pDevice->m_ZetaContextDmaInstance;
        break;

    case D3D_COPY_CONTEXT_DMA_IN_VIDEO_MEMORY:
        Instance = pDevice->m_CopyContextDmaInstance;
        break;

    default:
        NODEFAULT("Unexpected DMA context\n");
    }

    ULONG ClassNum = NV01_CONTEXT_DMA_IN_MEMORY; // Assumed
    BOOL IsAlias = FALSE; // Assumed

    if (pSurface == NULL)
    {
        Base = NULL;
        Limit = 0;
    }
    else
    {
        Pitch = PixelJar::GetPitch(pSurface);
        Base = (VOID*) pSurface->Data;
        Limit = PixelJar::GetHeight(pSurface) * Pitch - 1;

        ASSERT((DWORD) Base + Limit <= AGP_APERTURE_BYTES);
        ASSERT(Pitch >= PixelJar::GetWidth(pSurface) 
                      * PixelJar::GetBitsPerPixel(pSurface) / 8);
    }

    // We don't want to stick debug-only commands into the push-buffer when
    // recording a push-buffer, for a couple of reasons:
    //
    //   1.  The programmer may be recording a push-buffer using a debug
    //       build for later playback on a free build.
    //   2.  We'd rather not have to fix-up the DMA-object push-buffer
    //       commands.
    //
    if (pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER)
        return 0;

    // The corollary to the above is that as soon as we run a push-buffer
    // "resource", we revert back to always keeping the DMA context spanning
    // all of memory.  In other words, as soon as you call RunPushBuffer, 
    // you lose any DMA context protection, evermore.  We could be more 
    // sophisticated about it, but since this is a debug-only tool meant
    // mostly for validating the driver anyway, I think this is fine.
    //
    if (pDevice->m_StateFlags & STATE_RUNPUSHBUFFERWASCALLED)
    {
        Base = NULL;
        Limit = AGP_APERTURE_BYTES - 1;
    }

    GetAddressInfo(Base, &Address, &AddressSpace, IsAlias);

    PteInfo = (ULONG)(Address) | 3;

    switch (ClassNum)
    {
        case NV01_CONTEXT_DMA_TO_MEMORY:
            RegValue |= SF_NUM(_DMA, _CLASS, NV1_CONTEXT_DMA_TO_MEMORY);
            break;

        case NV01_CONTEXT_DMA_FROM_MEMORY:
            RegValue |= SF_NUM(_DMA, _CLASS, NV1_CONTEXT_DMA_FROM_MEMORY);
            break;

        case NV01_CONTEXT_DMA_IN_MEMORY:
            RegValue |= SF_NUM(_DMA, _CLASS, NV1_CONTEXT_DMA_IN_MEMORY);
            break;

        default:
            NODEFAULT("Invalid class specified for DMA context\n");
    }

    PteAdjust = (ULONG)((ULONG)Address & 0x0FFF);

    RegValue |= SF_NUM(_DMA, _ADJUST, PteAdjust);

    RegValue |= SF_DEF(_DMA, _PAGE_ENTRY, _LINEAR);
    RegValue |= SF_DEF(_DMA, _PAGE_TABLE, _PRESENT);

    if (AddressSpace == ADDR_FBMEM)
    {
        // Video memory
        RegValue |= SF_DEF(_DMA, _TARGET_NODE, _NVM);
    }
    else if (AddressSpace == ADDR_AGPMEM)
    {
        // AGP memory
        RegValue |= SF_DEF(_DMA, _TARGET_NODE, _AGP);
    }
    else if (AddressSpace == ADDR_SYSMEM)
    {
        // PCI memory
        RegValue |= SF_DEF(_DMA, _TARGET_NODE, _PCI);
    }

    // BUGBUG: Verify if we still need to do this
    //
    // Workaround a bug in the HOST's semaphore context dma checks.
    // It only accepts the SetContextDmaSemaphore method if the
    // ctxdma is marked COHERENCY_CACHED (even though the underlying
    // memory is NODE_NVM).  Since no other engine checks the
    // coherency flags (yet), this should be OK for now.
    //
    RegValue |= SF_DEF(_DMA, _FLAGS_MAPPING_COHERENCY, _CACHED);

    // Write to instance memory
    //
    // INST_WR32(RegisterBase, Instance, 8, PteInfo);
    // INST_WR32(RegisterBase, Instance, 12, PteInfo);
    // 
    // INST_WR32(RegisterBase, Instance, SF_OFFSET(NV_DMA_CLASS), RegValue);
    // INST_WR32(RegisterBase, Instance, SF_OFFSET(NV_DMA_LIMIT), Limit);
    //
    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_WAIT_FOR_IDLE, 0);
    pPush += 2;

    Push2(pPush, NV097_SET_ZSTENCIL_CLEAR_VALUE, 
          INST_OFFSET(Instance, 8), 
          PteInfo);
    Push1(pPush + 3, NV097_NO_OPERATION,
        NVX_WRITE_REGISTER_VALUE);
    pPush += 5;

    Push2(pPush, NV097_SET_ZSTENCIL_CLEAR_VALUE, 
          INST_OFFSET(Instance, 12),
          PteInfo);
    Push1(pPush + 3, NV097_NO_OPERATION,
        NVX_WRITE_REGISTER_VALUE);
    pPush += 5;

    Push2(pPush, NV097_SET_ZSTENCIL_CLEAR_VALUE, 
          INST_OFFSET(Instance, SF_OFFSET(NV_DMA_CLASS)), 
          RegValue);
    Push1(pPush + 3, NV097_NO_OPERATION,
        NVX_WRITE_REGISTER_VALUE);
    pPush += 5;

    Push2(pPush, NV097_SET_ZSTENCIL_CLEAR_VALUE, 
          INST_OFFSET(Instance, SF_OFFSET(NV_DMA_LIMIT)), 
          Limit);
    Push1(pPush + 3, NV097_NO_OPERATION,
        NVX_WRITE_REGISTER_VALUE);
    pPush += 5;

    // Now force a re-load of the DMA context with the new parameters.
    //
    switch (DmaContext)
    {
    case D3D_COLOR_CONTEXT_DMA_IN_VIDEO_MEMORY:
        Push1(pPush, 
              NV097_SET_CONTEXT_DMA_COLOR,
              D3D_COLOR_CONTEXT_DMA_IN_VIDEO_MEMORY);
        break;

    case D3D_ZETA_CONTEXT_DMA_IN_VIDEO_MEMORY:
        Push1(pPush, 
              NV097_SET_CONTEXT_DMA_ZETA,
              D3D_ZETA_CONTEXT_DMA_IN_VIDEO_MEMORY);
        break;

    case D3D_COPY_CONTEXT_DMA_IN_VIDEO_MEMORY:
        Push1(pPush,
              SUBCH_RECTCOPYSURFACES,
              NV062_SET_CONTEXT_DMA_IMAGE_DESTIN,
              D3D_COPY_CONTEXT_DMA_IN_VIDEO_MEMORY);
        break;

    default:
        NODEFAULT("Unexpected DMA context\n");
    }

    pDevice->EndPush(pPush + 2);

    return (DWORD) Base;
}

#endif


BOOL
CMiniport::InitDMAChannel(
    ULONG Class,
    OBJECTINFO* ErrorContext,
    OBJECTINFO* DataContext,
    ULONG Offset,
    PVOID* ppChannel
)
{

    // PERF: Are the following optimal

    ULONG FetchTrigger = 128;
    ULONG FetchSize = 128;
    ULONG FetchRequests = 8;

    //
    // Initialize channel stuff.  We only have one channel
    //
    m_HalInfo.FifoChID = 0;
    m_HalInfo.FifoMode = 0;
    m_HalInfo.FifoAllocCount = 0;
    m_HalInfo.FifoObjectCount = 0;
    m_HalInfo.FifoInstance = ReserveInstMem(NV20_GR_CONTEXT_SIZE >> 4);

    HalFifoAllocDMA(FetchTrigger, FetchSize, FetchRequests, DataContext);

    *ppChannel = (VOID*)((ULONG)m_RegisterBase + DEVICE_BASE(NV_USER) + (m_HalInfo.FifoChID << 16));

    return TRUE;
}



BOOL
CMiniport::BindToChannel(
    OBJECTINFO* Object
    )
{

    ULONG Entry;
    ULONG Hash;

    Hash = FIFO_HASH(Object->Handle, 0);

    //
    // Assume the entry is free, we don't maintain a list to search it
    // we need to ensure that IDs don't hash to the same function.  The IDs
    // are defined in hw.h sequentially.  If we create a new object, make
    // sure you define the id in hw.h and not use the one defined by nvidia
    //

    Entry = Hash;

    HalFifoHashAdd(Entry, Object->Handle, m_HalInfo.FifoChID, Object->Instance,
                    Object->Engine);

    return TRUE;
}



BOOL
CMiniport::CreateGrObject(
    ULONG Handle,
    ULONG ClassNum,
    OBJECTINFO* Object
    )
{
    ULONG InstSize;
    ULONG Instance;
    BOOL  Is3D = FALSE;

    //
    // Almost all graphic objects except for kelvin require 16 bytes of
    // instance memory.  You can look up the class in the following:
    //
    // NV20_ARCHITECTURE_CLASS_DESCRIPTORS in resman\kernel\hal\nv20\halnv20.c
    //
    // If you find a MKCLASSINSTMALLOC macro for the class then the specified
    // number of bytes are allocated, otherwise 16 bytes are allocated.
    //
    switch (ClassNum)
    {
        case NV20_KELVIN_PRIMITIVE:
            InstSize = 0x33 * 16;
            Is3D = TRUE;
            break;

        case NV04_CONTEXT_PATTERN:
        case NV04_CONTEXT_COLOR_KEY:
        case NV03_CONTEXT_ROP:
        case NV01_CONTEXT_BETA:
        case NV04_CONTEXT_BETA:
        case NV01_CONTEXT_CLIP_RECTANGLE:

        case NV10_CONTEXT_SURFACES_2D:
        case NV15_IMAGE_BLIT:
        case NV03_MEMORY_TO_MEMORY_FORMAT:
        case NV1_NULL:
            InstSize = 1 * 16;
            break;

        default:
            NODEFAULT("MP: CreateGrObject invalid class number\n");
    }

    //
    // Reserve instance memory
    //
    Instance = ReserveInstMem(InstSize >> 4);


    if (Is3D)
    {
        ASSERT(m_HalInfo.GrCurrentObjects3d[m_HalInfo.FifoChID] == 0);

        m_HalInfo.GrCurrentObjects3d[m_HalInfo.FifoChID] = Instance;

        //
        // Init the 3d object
        //
        HalGrInit3d();
    }

    //
    // Init object context.
    //
    HalGrInitObjectContext(Instance, ClassNum);


    //
    // Fill in the object fields
    //
    Object->Init();
    Object->Handle = Handle;
    Object->ClassNum = ClassNum;
    Object->Instance = Instance;
    Object->Engine = NV_RAMHT_ENGINE_GRAPHICS;

    BindToChannel(Object);

    return TRUE;
}


VOID
CMiniport::ShutdownEngines()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    //
    // Blank the screen if we don't have a copy of the bits on the screen.
    //
    if (AvGetSavedDataAddress() == NULL)
    {
        AvSendTVEncoderOption(RegisterBase, AV_OPTION_BLANK_SCREEN, TRUE, NULL);
    }

    for (INT TileRegion = 0; TileRegion < 8; TileRegion++)
    {
        DestroyTile(TileRegion, 1);
    }

    //
    // Disable dma timeslicing
    //
    REG_WR32(RegisterBase, NV_PFIFO_DMA_TIMESLICE, DRF_DEF(_PFIFO, _DMA_TIMESLICE, _TIMEOUT, _DISABLED));

    //
    // Empty FIFO and runout.
    //
    while ((REG_RD_DRF(RegisterBase, _PFIFO, _CACHE1_STATUS, _LOW_MARK) != NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY)
           || (REG_RD_DRF(RegisterBase, _PFIFO, _RUNOUT_STATUS, _LOW_MARK) != NV_PFIFO_RUNOUT_STATUS_LOW_MARK_EMPTY)
           || (REG_RD_DRF(RegisterBase, _PFIFO, _CACHE1_DMA_PUSH, _STATE) == NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY))
    {
        //
        // Make sure we process any fifo interrupts
        //
        ServiceFifoInterrupt();

        //
        // Check if GE needs servicing.
        //
        if (REG_RD32(RegisterBase, NV_PGRAPH_INTR))
            ServiceGrInterrupt();

        // Check if vblank needs servicing.
        if (REG_RD32(RegisterBase, NV_PMC_INTR_0) & DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING))
            VBlank();
    }

    //
    // Disable the dma pusher, if active
    //
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_DMA_PUSH, _ACCESS, _DISABLED);

    //
    // Wait til idle
    //
    while (REG_RD_DRF(RegisterBase, _PFIFO, _CACHE1_DMA_PUSH, _STATE) == NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY)
        ;

    //
    // Disable cache reassignment.
    //
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHES, _REASSIGN, _DISABLED);
    //
    // Disable pusher and puller access to cache0.
    //
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE0_PUSH0, _ACCESS, _DISABLED);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE0_PULL0, _ACCESS, _DISABLED);
    //
    // Disable pusher and puller access to cache1.
    //
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_PUSH0, _ACCESS, _DISABLED);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_PULL0, _ACCESS, _DISABLED);
    //
    // Force a context switch to PRAMIN.
    //
    HalFifoContextSwitch(NUM_FIFOS_NV20 - 1);

    //
    // Zero FIFO get/put.
    //
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_PUT, 0);
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_GET, 0);

    //
    // Flush all FIFOs.
    //
    ULONG FifoReassign = REG_RD32(RegisterBase, NV_PFIFO_CACHES);
    ULONG FifoPush = REG_RD32(RegisterBase, NV_PFIFO_CACHE1_PUSH0);
    ULONG FifoPull = REG_RD32(RegisterBase, NV_PFIFO_CACHE1_PULL0);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHES, _REASSIGN, _DISABLED);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_PUSH0, _ACCESS, _DISABLED);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_PULL0, _ACCESS, _DISABLED);

    //
    // Update the FIFO context and its dirty bits.  By moving the get pointer to the current
    // put value, we are flushing any pending push data that hasn't been fetched yet.
    //
    for (ULONG i = 0; i < NUM_FIFOS_NV20; i++)
    {
        if (m_HalInfo.FifoInUse)
        {
            REG_WR32(RegisterBase, (ULONG)(m_HalInfo.FifoContextAddr1 + (i * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_GET)),
                     REG_RD32(RegisterBase, (ULONG)(m_HalInfo.FifoContextAddr1 + (i * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_PUT))));

            //
            // Also, when setting the channels get pointer equal to its put, ensure the next
            // DWORD is interpreted as a command (and not data) by clearing RAMFC_DMA_METHOD.
            //
            REG_WR32(RegisterBase, (U032)(m_HalInfo.FifoContextAddr1 + (i * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_METHOD)), 0x0);
        }
    }

    //
    // Restore CACHE1 state.
    //
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_PULL0, FifoPull);
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_PUSH0, FifoPush);
    REG_WR32(RegisterBase, NV_PFIFO_CACHES, FifoReassign);

    //
    // Flush all pending
    //
    REG_WR32(RegisterBase, NV_PFIFO_DMA, 0);

    //
    // Disable all FIFO interrupts.
    //
    REG_WR32(RegisterBase, NV_PFIFO_INTR_EN_0, 0);

    //
    // Force an unload of the current channel.
    //
    HalGrLoadChannelContext(NUM_FIFOS_NV20);

    REG_WR32(RegisterBase, NV_PFB_CFG0, m_HalInfo.FbSave0);
    REG_WR32(RegisterBase, NV_PFB_CFG1, m_HalInfo.FbSave1);

    REG_WR32(RegisterBase, NV_PMC_ENABLE, m_HalInfo.McSave);
    REG_WR32(RegisterBase, NV_PMC_INTR_EN_0, m_HalInfo.McSaveIntrEn0);

    //
    // Make sure that the notification has been removed.
    //
    HalRegisterShutdownNotification(&m_ShutdownRegistration, FALSE);

    //
    // Disconnect the interrupt.
    //
    KeDisconnectInterrupt(&m_InterruptObject);
}


BOOL
CMiniport::CreateTile(
    ULONG TileRegion,
    ULONG Offset,
    ULONG Size,
    ULONG Pitch,
    ULONG MemType,
    ULONG ZTag,
    ULONG ZOffset
    )
{

    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG dmapush;


    ASSERT(!(Offset & 0x3fff) && !(Size & 0x3fff));

    // Idle graphics.  If we're not GR_IDLE during these writes to the PGRAPH registers,
    // these writes can be dropped leaving the PFB and PGRAPH regs inconsistent. The
    // idle/resume routines are called during FreeTiledRange/ProgramTiledRange.
    //
    TilingUpdateIdle(&dmapush);


    //
    // Note: due to a PFB_TLIMIT readback bug, only compare the upper bits
    // when checking for PFB_TLIMIT/PGRAPH_TLIMIT consistency.
    //
    do
    {
        // wait until graphics is idle
        HalGrIdle();

        REG_WR32(RegisterBase, NV_PFB_TLIMIT(TileRegion), Offset + Size - 1);
        REG_WR32(RegisterBase, NV_PFB_TSIZE(TileRegion), Pitch);
        REG_WR32(RegisterBase, NV_PGRAPH_TLIMIT(TileRegion), Offset + Size - 1);
        REG_WR32(RegisterBase, NV_PGRAPH_TSIZE(TileRegion), Pitch);

        //
        // Flip BANK0_SENSE on for depth buffers, but leave it off
        // for color and anything else.
        //

        if (MemType & D3DTILE_FLAGS_ZBUFFER)
        {
            REG_WR32(RegisterBase, NV_PGRAPH_TILE(TileRegion),
                     Offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID) |
                     DRF_DEF(_PFB, _TILE, _BANK0_SENSE, _1));
            REG_WR32(RegisterBase, NV_PFB_TILE(TileRegion),
                     Offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID) |
                     DRF_DEF(_PFB, _TILE, _BANK0_SENSE, _1));
            RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TILE_ADDRESS(TileRegion),
                     Offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID) |
                     DRF_DEF(_PFB, _TILE, _BANK0_SENSE, _1));
        }
        else
        {
            REG_WR32(RegisterBase, NV_PGRAPH_TILE(TileRegion), Offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID));
            REG_WR32(RegisterBase, NV_PFB_TILE(TileRegion), Offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID));
            RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TILE_ADDRESS(TileRegion), Offset | DRF_DEF(_PFB, _TILE, _REGION, _VALID));
        }

        RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TLIMIT_ADDRESS(TileRegion),
                     Offset + Size - 1);
        RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TPITCH_ADDRESS(TileRegion),
                     Pitch);
    } while ((REG_RD32(RegisterBase, NV_PFB_TILE(TileRegion)) != REG_RD32(RegisterBase, NV_PGRAPH_TILE(TileRegion))) ||
             (REG_RD32(RegisterBase, NV_PFB_TSIZE(TileRegion)) != REG_RD32(RegisterBase, NV_PGRAPH_TSIZE(TileRegion))) ||
             ((REG_RD32(RegisterBase, NV_PFB_TLIMIT(TileRegion)) & ~0x3FFF) != (REG_RD32(RegisterBase, NV_PGRAPH_TLIMIT(TileRegion)) & ~0x3FFF)));

    //
    // Handle Z compression.
    //
    if (MemType & D3DTILE_FLAGS_ZCOMPRESS)
    {
        ULONG zdata;

        // initialize zoffset into ztag pool.  Since there are 4 memory controllers,
        // we normalize the start tag here
        zdata = (ZTag / 4) | DRF_DEF(_PFB, _ZCOMP, _EN, _TRUE);

        // mode (16 or 32bit)
        if (MemType & D3DTILE_FLAGS_Z32BITS)
        {
            zdata |= DRF_DEF(_PFB, _ZCOMP, _MODE, _32);
        }

        do
        {
            // Wait until graphics is idle
            HalGrIdle();

            // Write to registers
            REG_WR32(RegisterBase, NV_PFB_ZCOMP(TileRegion), zdata);
            REG_WR32(RegisterBase, NV_PGRAPH_ZCOMP(TileRegion), zdata);
            RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                         NV_IGRAPH_TC_MINTFCDEBUG_ZCOMP_ADDRESS(TileRegion),zdata);

        } while (REG_RD32(RegisterBase, NV_PFB_ZCOMP(TileRegion)) !=
                 REG_RD32(RegisterBase, NV_PGRAPH_ZCOMP(TileRegion)));

        if (ZOffset != 0)
        {
            do
            {
                ULONG zoffset;

                zoffset = ZOffset | TileRegion | DRF_DEF(_PFB, _ZCOMP_OFFSET, _EN, _TRUE);

                // Wait until graphics is idle
                HalGrIdle();
    
                // Write to registers
                REG_WR32(RegisterBase, NV_PFB_ZCOMP_OFFSET, zoffset);
                REG_WR32(RegisterBase, NV_PGRAPH_ZCOMP_OFFSET, zoffset);
    
            } while (REG_RD32(RegisterBase, NV_PFB_ZCOMP_OFFSET) !=
                     REG_RD32(RegisterBase, NV_PGRAPH_ZCOMP_OFFSET));
        }
    }

    // Resume graphics, now that PFB/PGRAPH are consistent
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_PUSH, dmapush);

    return TRUE;
}


BOOL
CMiniport::DestroyTile(
    ULONG TileRegion,
    ULONG ZOffset)
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG dmapush;

    TilingUpdateIdle(&dmapush);

    do {
        // Now, wait until graphics is idle
        HalGrIdle();

        // invalidate the range
        REG_WR32(RegisterBase, NV_PFB_TILE(TileRegion), NV_PFB_TILE_REGION_INVALID);
        REG_WR32(RegisterBase, NV_PGRAPH_TILE(TileRegion), NV_PFB_TILE_REGION_INVALID);
        RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TILE_ADDRESS(TileRegion),
                     NV_PFB_TILE_REGION_INVALID);
    } while (REG_RD32(RegisterBase, NV_PFB_TILE(TileRegion)) != REG_RD32(RegisterBase, NV_PGRAPH_TILE(TileRegion)));

    REG_WR32(RegisterBase, NV_PFB_ZCOMP(TileRegion), NV_PFB_ZCOMP_EN_FALSE);
    REG_WR32(RegisterBase, NV_PGRAPH_ZCOMP(TileRegion), NV_PFB_ZCOMP_EN_FALSE);
    RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                 NV_IGRAPH_TC_MINTFCDEBUG_ZCOMP_ADDRESS(TileRegion),
                 NV_PFB_ZCOMP_EN_FALSE);

    if (ZOffset != 0)
    {
        REG_WR32(RegisterBase, NV_PFB_ZCOMP_OFFSET, NV_PFB_ZCOMP_OFFSET_EN_FALSE);
        REG_WR32(RegisterBase, NV_PGRAPH_ZCOMP_OFFSET, NV_PFB_ZCOMP_OFFSET_EN_FALSE);
    }

    // Resume graphics, now that PFB/PGRAPH are consistent
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_PUSH, dmapush);

    return TRUE;
}


VOID
CMiniport::TilingUpdateIdle(
    ULONG* dmapush
    )
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    while ((REG_RD_DRF(RegisterBase, _PFIFO, _CACHE1_STATUS, _LOW_MARK) != NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY)
           || (REG_RD_DRF(RegisterBase, _PFIFO, _RUNOUT_STATUS, _LOW_MARK) != NV_PFIFO_RUNOUT_STATUS_LOW_MARK_EMPTY)
           || (REG_RD_DRF(RegisterBase, _PFIFO, _CACHE1_DMA_PUSH, _STATE) == NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY))
    {
        // Make sure we process any fifo interrupts
        ServiceFifoInterrupt();

        // Check if GE needs servicing.
        if (REG_RD32(RegisterBase, NV_PGRAPH_INTR))
            ServiceGrInterrupt();

        // Check if vblank needs servicing.
        if (REG_RD32(RegisterBase, NV_PMC_INTR_0) & DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING))
            VBlank();
    }

    // Disable the dma pusher
    *dmapush = REG_RD32(RegisterBase, NV_PFIFO_CACHE1_DMA_PUSH);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_DMA_PUSH, _ACCESS, _DISABLED);

    // Wait until the DMA pusher is idle
    while (REG_RD_DRF(RegisterBase, _PFIFO, _CACHE1_DMA_PUSH, _STATE) == NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY)
        ;

}



VOID
CMiniport::TmrDelay(ULONG nsec)
{

    // NOTE: we only handle delays less than a second.
    // In this case, PTIMER_TIME_0 can wrap around at most once.
    // So doing 2's complement math on TIME_0 is enough.
    // What if this call is preempted by something else?
    // ASSERT(nsec < 0x40000000);

    ULONG start = REG_RD32(m_RegisterBase, NV_PTIMER_TIME_0);
    ULONG ellapsed;
    do
    {
        ellapsed = REG_RD32(m_RegisterBase, NV_PTIMER_TIME_0) - start;
    } while (ellapsed < nsec);
}

} // namespace D3D


