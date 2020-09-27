/**************************************************************************************************************
*
*   Module:  osinit.c
*
*   Description:
*       This is the NT-specific, device-independent initialization code for the resource manager.
*
*
**************************************************************************************************************/

#include <nvhw.h>
#include <nvrm.h>
#include <nvos.h>
#include <nvarch.h>
#include <devinit.h>
#include <rmfail.h>

//
// Globals
//
WINNTHWINFO winNTHwInfo[MAX_INSTANCE];

// local prototypes
RM_STATUS   initDmaListElementPool();
VOID        initDisplayInfo(PDISPLAYINFO);


BOOL RmInitRm()
{
    // init client database
    if (initClientInfo() != RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Cannot initialize client database\n");
        return FALSE;
    }

    // init dma context-list element pool
    if (initDmaListElementPool() != RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Cannot initialize graphics patch pool\n");
        return FALSE;
    }

    // init state of display
    osDisplayInfo.HdrSize = sizeof(DISPLAYINFO);
    initDisplayInfo(&osDisplayInfo);

    return TRUE;
       
} // end of RmInitRm()


BOOL RmInitNvMapping
(
    VOID* deviceExtension,
    U032 PhysNvAddr,
    PHWREG nvRegisterBase,
    U032* pDeviceReference
)
{
    PHWINFO pDev;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: RmInitNvMapping:\n");

    // get the next available device instance
    if (!RM_SUCCESS(RmAllocDeviceInstance(pDeviceReference)))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Cannot initialize device database\n");
        return FALSE;
    }
    pDev = NvDBPtr_Table[*pDeviceReference];

    // setup the ptr to the OS Hw info
    pDev->pOsHwInfo = (VOID_PTR) &winNTHwInfo[*pDeviceReference];

    // set the device extension
    pDev->DBmpDeviceExtension = deviceExtension;

    // init configuration
    osStringCopy(pDev->Mapping.ID, "Device Mapping");
    pDev->Mapping.Size = sizeof(pDev->Mapping);

    // set physical and logical register base
    pDev->Mapping.PhysAddr = PhysNvAddr;
    nvAddr = nvRegisterBase;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: device instance          :", devInstance);
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: NV using physical address:", (VOID *)pDev->Mapping.PhysAddr);
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: NV using linear address  :", nvAddr);
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: ROM using linear address :", romAddr);
    
    // default the crystal frequency to 0
    pDev->Chip.HalInfo.CrystalFreq = 0;

    return TRUE;

} // end of RmInitNvMapping()

BOOL RmInitNvHal
(
    U032 deviceReference
)
{
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    // hook up the HAL
    if (initHal(pDev) != RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Cannot initialize device-dependent (HAL) state\n");
        return FALSE;
    }

    // set the chip revision
    nvHalMcControl(pDev, MC_CONTROL_SETREVISION);

    // set the bios addr now, PraminOffset is set in MC_CONTROL_SETREVISION
    biosAddr = (PHWREG)((U008 *)nvAddr + pDev->Pram.HalInfo.PraminOffset);

    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: BIOS using linear address:", biosAddr);

    return (TRUE);
} // end of RmInitNvHal()

// the FB address is only provided on the first POST
BOOL RmPostNvDevice
(
    U032 deviceReference,
    PHWREG externalBiosImage,
    PHWREG frameBufferBase
)
{
    RM_STATUS rmStatus = RM_OK;
    BOOL biosUsedToInit;
    
    // set the device context
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: POSTing device instance  :", devInstance);

    // set the FB logical address, if given
    if (frameBufferBase)
    {
        fbAddr = frameBufferBase;
    }
    
    // POST the device
    rmStatus = DevinitInitializeDevice(pDev, externalBiosImage, &biosUsedToInit);

    if (RM_SUCCESS(rmStatus))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: ...POST completed sucessfully\n");
        
        // shadow the VGA BIOS image in upper inst mem
        if (externalBiosImage)
        {
            DevinitShadowBios(pDev, externalBiosImage);
        }
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Cannot POST the device\n");
    }
    
    return (rmStatus == RM_OK);

} // end of RmPostNvDevice()

BOOL RmInitNvDevice
(
    U032 deviceReference,
    U032 PhysFbAddr,
    PHWREG frameBufferBase
)
{
    // set the device context
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: RmInitNvDevice:\n");

    // init the RM FAILURE facility if enabled
    RM_FAILURE_ENABLE();
    
    // init processor synchronization
    if (!RM_SUCCESS(osInitSpinLock(pDev)))
    {
        return FALSE;
    }

    // init "DMA adapter" for use in memory management routines
    if (!RM_SUCCESS(osInitDmaAdapter(pDev)))
    {
        return FALSE;
    }
        
#ifdef RM_STATS
    // init all data structures
    pDev->Statistics.MemAlloced = 0;
    pDev->Statistics.MemLocked = 0;
#endif

    // set physical and logical frame buffer
    pDev->Mapping.PhysFbAddr = PhysFbAddr;
    fbAddr = frameBufferBase;
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: device instance          :", devInstance);
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: FB using physical address:", (VOID *)pDev->Mapping.PhysFbAddr);
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: FB using linear address  :", fbAddr);

    // initialize all engines -- calls back initMapping()
    if (!RM_SUCCESS(stateNv(pDev, STATE_INIT)))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Cannot initialize the device\n");
        return FALSE;
    }

    // come up as VGA until display driver disables it, thus enabling high-res
    pDev->Vga.Enabled = TRUE;
    if (!RM_SUCCESS(stateNv(pDev, STATE_LOAD)))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Cannot load state into the device\n");
        return FALSE;
    }

    return TRUE;

} // end of RmInitNvDevice()

// init the global pool of DMA context list elements
RM_STATUS initDmaListElementPool()
{
    RM_STATUS status = RM_OK;
    U032 i;

    // allocate the pool
    status = osAllocMem((VOID **)&dmaListElementPool, sizeof(DMALISTELEMENT_POOL_ELEMENT) * DMALISTELEMENT_POOL_SIZE);
    if (RM_SUCCESS(status))
    {
        // initialize the pool
        for (i = 0; i < DMALISTELEMENT_POOL_SIZE; i++)
        {
            dmaListElementPool[i].inUse = FALSE;
        }
    }

    return status;

} // end of initDmaCtxtListElemPool()

VOID initDisplayInfo(PDISPLAYINFO pDi)
{
    // these are hard coded not unreasonable values for the time being
    pDi->XRes                   = 1024;
    pDi->YRes                   = 768;
    pDi->Bpp                    = 16;
    pDi->HdrSize                = 0x32;
    pDi->InfoFlags              = 0;
    pDi->DevNodeHandle          = 0;
    pDi->DriverName[0]          = 0;
    pDi->DPI                    = 0x60;
    pDi->Planes                 = 0x1;
    pDi->RefreshRateMax         = 0x4F;
    pDi->RefreshRateMin         = 0x32;
    pDi->LowHorz                = 0x1E;
    pDi->HighHorz               = 0x40;
    pDi->LowVert                = 0x32;
    pDi->HighVert               = 0x64;
    pDi->MonitorDevNodeHandle   = 0;
    pDi->HorzSyncPolarity       = 0x2B;
    pDi->VertSyncPolarity       = 0x2B;

} // end of RmInitDisplayInfo()

// map the NV device into the RM memory space
RM_STATUS initMapping
(
    PHWINFO pDev
)
{
    PWINNTHWINFO pOsHwInfo = (PWINNTHWINFO) pDev->pOsHwInfo;

    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: initMapping:\n");

    // attempt to determine the current CPU type
    RmInitCpuInfo(pDev);

    // attempt to determine the FSB and Memory speeds
    RmInitBusInfo(pDev);

    // reload mapped device addresses for HAL use
    if (initHalMappings(pDev) != RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Cannot initialize device mappings for HAL\n\r");
        return FALSE;
    }

    // satisfy MC init
    pDev->Mapping.IntPin = 0x0A;

    // Some PCI BIOSs leave the ROM mapped.  This causes problems if it overlays system RAM.  Just disable it.
    REG_WR32(NV_PBUS_PCI_NV_12, 0);
    
    // make sure our PCI latency timer is sufficient (max it out)
    REG_WR32(NV_PBUS_PCI_NV_3, DRF_NUM(_PBUS,_PCI_NV_3,_LATENCY_TIMER, NV_PBUS_PCI_NV_3_LATENCY_TIMER_248_CLOCKS));

#if (_WIN32_WINNT < 0x500)
    //
    // For NT4, we'll attempt to recognize the chipset and init AGP support
    // if this is an AGP card and there's no registry entry override set
    //
    {
        U032 data32;
        U032 stringlength=80;
        char string[80];
        extern char strDevNodeRM[];
        extern char strNTSrvPackVersion[];

        // For DELL's unattended install, they're running NT without *ANY* service packs!
        // In this case, do NOT implement AGP functionality, because NT without
        // service pack can't handle it.  We'll check and see if a service pack 
        // (greather than or equal to service pack 3) was installed.  Not sure
        // what C libraries we can include here, so to be safe, just compare the strings by hand.
    
        if (osReadRegistryString(strNTSrvPackVersion, "CSDVersion", &string[0], &stringlength) == RM_OK)
        {
            if ( ((string[0] == 'S') || (string[0] == 's'))  && 
                 ((string[1] == 'E') || (string[1] == 'e'))  &&                 
                 ((string[2] == 'R') || (string[2] == 'r'))  &&
                 ((string[3] == 'V') || (string[3] == 'v'))  && 
                 ((string[4] == 'I') || (string[4] == 'i'))  && 
                 ((string[5] == 'C') || (string[5] == 'c'))  && 
                 ((string[6] == 'E') || (string[6] == 'e'))  &&             
                 (string[13] >= '3') )             
            {

                if (osReadRegistryDword(pDev, strDevNodeRM, "NoNT4AGP", &data32) != RM_OK)
                {
                    // Allow an override to set the AGP rate
                    if (osReadRegistryDword(pDev, strDevNodeRM, "ReqAGPRate", &data32) == RM_OK)
                        pDev->Chip.ReqAGPRate = data32;

                    if (REG_RD_DRF(_PBUS, _PCI_NV_24, _NEXT_PTR) == NV_PBUS_PCI_NV_24_NEXT_PTR_AGP)
                        RmInitAGP(pDev);
                }

            }
        }
    }
#endif

#if 0
    // Apply any AGP chipset workarounds or overrides
    NvUpdateAGPConfig(pDev);
#endif

    // flag RM to be initialized
    pOsHwInfo->rmInitialized = TRUE;

    // TO DO: add AGP support in NT 5.0

    return RM_OK;

} // end of initMapping()

// OS-specific initializations

RM_STATUS initFbEx
(
    PHWINFO pDev
)
{
    S032  fbCalcMemSize(VOID);
    U032 calc_max_bw (VOID);
    U032 size;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: initFramebufferEx\n");
    //
    // Get screen resolution and framebuffer configuration from registry.
    //
    pDev->Dac.HalInfo.Depth = (osDisplayInfo.InfoFlags & REGISTRY_BPP_NOT_VALID) ? 8 : osDisplayInfo.Bpp;
    if ((osDisplayInfo.InfoFlags & (MONITOR_INFO_NOT_VALID | MONITOR_INFO_DISABLED_BY_USER))
     || (osDisplayInfo.RefreshRateMax == 0xFFFFFFFF)
     || (osDisplayInfo.RefreshRateMax == 0))
    {
        pDev->Framebuffer.HalInfo.RefreshRate   = 60;
        pDev->Framebuffer.HSyncPolarity = BUFFER_HSYNC_NEGATIVE;
        pDev->Framebuffer.VSyncPolarity = BUFFER_HSYNC_NEGATIVE;
    }
    else
    {
        pDev->Framebuffer.HalInfo.RefreshRate   = osDisplayInfo.RefreshRateMax;
        pDev->Framebuffer.HSyncPolarity = osDisplayInfo.HorzSyncPolarity;
        pDev->Framebuffer.VSyncPolarity = osDisplayInfo.VertSyncPolarity;
    }

    pDev->Framebuffer.HalInfo.HorizDisplayWidth = osDisplayInfo.XRes;
    pDev->Framebuffer.HalInfo.VertDisplayWidth  = osDisplayInfo.YRes;
    pDev->Framebuffer.HorizFrontPorch = 0;
    pDev->Framebuffer.HorizSyncWidth  = 0;
    pDev->Framebuffer.HorizBackPorch  = 0;
    pDev->Framebuffer.VertFrontPorch  = 0;
    pDev->Framebuffer.VertSyncWidth   = 0;
    pDev->Framebuffer.VertBackPorch   = 0;
    pDev->Framebuffer.CSync           = BUFFER_CSYNC_DISABLED;
    pDev->Dac.HalInfo.VClk            = 0;
    //
    // Calculate the framebuffer count based on the configuration with double buffering.
    //
    size = pDev->Dac.HalInfo.Depth / 8
         * pDev->Framebuffer.HalInfo.HorizDisplayWidth * pDev->Framebuffer.HalInfo.VertDisplayWidth
         * 2;
    if (size > pDev->Framebuffer.HalInfo.RamSize)
        size = 0;
    else
        size = pDev->Framebuffer.HalInfo.RamSize - size;
    pDev->Framebuffer.Count = 1;

    return (RM_OK);
}
RM_STATUS initGrEx
(
    PHWINFO pDev
)
{
    return (RM_OK);
}
RM_STATUS initDacEx
(
    PHWINFO pDev
)
{
    return (RM_OK);
}
RM_STATUS initFifoEx
(
    PHWINFO pDev
)
{
    pDev->Fifo.HalInfo.RetryCount = 1;

    return (RM_OK);
}
RM_STATUS initRModeEx
(
    PHWINFO pDev
)
{
    return(RM_OK);
}

// end of osinit.c
