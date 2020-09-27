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

/******************* Operating System Interface Routines *******************\
*                                                                           *
* ModuleI: INIT.C                                                           *
*   This is the OS initialization module.  One time initialization code     *
*   is present here so it can be unloaded after the OS initialization phase *
*   is complete.                                                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include "basedef.h"
#include "vmmtypes.h"
#include "vmm.h"
#include "vnvrmd.h"
#include "vpicd.h"
#include "vdd.h"
//#include "minivdd.h"
//#include "vpaged.h"
#include <nv_ref.h>
#include <nv10_ref.h>
#include <nvrm.h>
#include "nvrmwin.h"
#include "nvhw.h"
#include <state.h>
#include <class.h>
#include <os.h>
#include "oswin.h"
#include "vmm2.h"
#include "files.h"
#include "nvmisc.h"
#include "nvReg.h"
#include <nv10_hal.h>

#ifndef PC98
// PCI function ID for PC AT
#define PCI_FUNCTION_ID 0xB1
#else
// PCI function ID for PC98
#define PCI_FUNCTION_ID 0xCC
#endif // PC98

#define READ_CONFIG_BYTE        0x08
#define READ_CONFIG_DWORD       0x0A
#define WRITE_CONFIG_BYTE       0x0B
#define WRITE_CONFIG_DWORD      0x0D

//
// Globals
//
WIN9XHWINFO	win9xHwInfo[MAX_INSTANCE];

//
// Externs.
//
extern DISPLAYINFO osDisplayInfo;
extern VOID RmInitCpuInfo(PHWINFO);
extern VOID RmInitBusInfo(PHWINFO);

// This should already exist somewhere, right??
#define RM_GLOBAL_BASE_PATH    "SOFTWARE\\NVIDIA Corporation\\Global\\System"

//---------------------------------------------------------------------------
//
//  Initialization routines.
//
//---------------------------------------------------------------------------
RM_STATUS initStack_frame
(
    PHWINFO pDev
)
{
	PWIN9XHWINFO pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;
    RM_STATUS status;
    U032      i;
    
    //
    // Allocate it.
    //
    status = osAllocMem((VOID **)&pOsHwInfo->NvIsrStackFrameBase, STACK_SIZE);
    if (status)
        return (status);
    //
    // Lock it.
    //
    vmmLinPageLock(PAGE(pOsHwInfo->NvIsrStackFrameBase), NPAGES(((U032)pOsHwInfo->NvIsrStackFrameBase & PAGEMASK) + STACK_SIZE), 0);
    //
    // Insert fence guard.
    //
    pOsHwInfo->NvIsrStackFrame = pOsHwInfo->NvIsrStackFrameBase;
    for (i = 0; i < STACK_SIZE / 4; i++)
        *(pOsHwInfo->NvIsrStackFrame)++ = 'MRVN';
    (pOsHwInfo->NvIsrStackFrame)--;
    
    return (RM_OK);
}

//
// Initialize the Resource Manager semaphores.
//
RM_STATUS initSemaphore
(
    VOID
)
{
    //
    // Create semaphores.
    //
    rmSemaphore = vmmCreateSemaphore(0);
    osSemaphore = vmmCreateSemaphore(0);
    // Multi-Mon support semaphore.
    mmSemaphore = vmmCreateSemaphore(0);
    //
    // Prime the semaphores.
    //
    vmmSignalSemaphore(rmSemaphore);
    vmmSignalSemaphore(osSemaphore);
    vmmSignalSemaphore(mmSemaphore);
    return (RM_OK);
}
//
// Initialize the callback structure free list.
//
RM_STATUS initCallbackFreeList
(
    VOID
)
{
    RM_STATUS status;
    U032      i;
    
    osCallbackList = NULL;
    status = osAllocMem((VOID **)&osCallbackTable, sizeof(CALLBACK) * NUM_CALLBACKS);
    if (status)
        return (status);
    osCallbackFreeList = osCallbackTable;    
    for (i = 0; i < NUM_CALLBACKS - 1; i++)
    {
        osCallbackFreeList[i].Next = &(osCallbackFreeList[i + 1]);
    }
    osCallbackFreeList[i].Next = NULL;
    return (RM_OK);
}

//
// Currently not used, but a useful routine to determine if the chipset driver has been
// loaded. If the DeviceID/VendorID is in the registry under the GARTMiniports key, we'll
// assume the driver has been loaded (registry entry is setup in the chipset INF).
//
static BOOL IsChipsetDriverLoaded( PHWINFO pDev, U016 vendorid, U016 deviceid)
{
    U032 data, csid = (deviceid << 16) | vendorid;
    UCHAR *strGARTMP = "System\\CurrentControlSet\\Services\\VxD\\PCI\\GARTMiniports\\DDDDVVVV";
    UCHAR *pDV = &strGARTMP[56];      // ptr to where DeviceID and VenderID are concatenated
    S032 i;

    // convert each nibble of the Device/Vendor IDs into chars and concatenate
    for (i = 7; i >= 0; i--) {
        U008 ch = (U008) ((csid >> (i * 4)) & 0xF);

        if (ch >= 0x0 && ch <= 0x9)
            *pDV++ = '0' + ch;
        else
            *pDV++ = 'A' + (ch - 0xA);
    }

    if (osReadRegistryDword(pDev, strGARTMP, "Instance", &data) != RM_OK)
        return FALSE;
    else
        return TRUE;
}

//
// Initialize the channel allocation database.
//
RM_STATUS initChannelInfo
(
    VOID
)
{
    return (RM_OK);
}    

// This is a helper proc to detect an ALI1541 chipset.
// Returns: TRUE if ALI1541 is detected,
//          FALSE otherwise.
//
static BOOL IsALI1541ChipSet(CLIENT_STRUCT *pcrs)
{
    pcrs->CBRS.Client_AH  = PCI_FUNCTION_ID; // PCI_FUNCTION_ID
    pcrs->CBRS.Client_AL  = 0x02;            // FIND_PCI_DEVICE
    pcrs->CWRS.Client_CX = 0x1541;          // Device ID
    pcrs->CWRS.Client_DX = 0x10B9;          // Vendor ID
    pcrs->CWRS.Client_SI = 0;               // Index
    PCI(pcrs);
    
    return (pcrs->CBRS.Client_AH == 0);
}

// This is a helper proc to detect an Crush11 North Bridge chipset.
// Returns: TRUE if Crush11 is detected,
//          FALSE otherwise.
//
static BOOL IsCrush11ChipSet(CLIENT_STRUCT *pcrs)
{
    pcrs->CBRS.Client_AH  = PCI_FUNCTION_ID; // PCI_FUNCTION_ID
    pcrs->CBRS.Client_AL  = 0x02;            // FIND_PCI_DEVICE
    pcrs->CWRS.Client_CX = 0x01B7;          // Device ID
    pcrs->CWRS.Client_DX = 0x10DE;          // Vendor ID
    pcrs->CWRS.Client_SI = 0;               // Index
    PCI(pcrs);
    
    return (pcrs->CBRS.Client_AH == 0);
}

#define FB_TEST_VALUE     0xA5A5A5A5

//
// Initialize the Device Information structure.
//
RM_STATUS initMapping
(
    PHWINFO pDev
)
{
	PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;
	PWIN9XHWINFO pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;

    U032 data;
    CLIENT_STRUCT  crs;
    struct VPICD_IRQ_Descriptor picIRQDesc;
    U032 fb_test, status = 0;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: initMapping\n\r");
    //
    // Initialize structure size.
    //
    pDev->Mapping.Size = sizeof(pDev->Mapping);
    osStringCopy(pDev->Mapping.ID, "Device Mapping");
    pDev->Mapping.IntPin  = 0x01;      // Int #A

    nvAddr = (PHWREG)vmmMapPhysToLinear(pDev->Mapping.PhysAddr, 0x01000000, 0);    
    fbAddr = (PHWREG)vmmMapPhysToLinear(pDev->Mapping.PhysFbAddr, pDev->Mapping.fbLength, 0);

    //
    // Attempt to determine the current CPU type
    //
    RmInitCpuInfo(pDev);

    //
    // Before going any further, let's do a quick verification that this memory mapping is correct.
    //
    fb_test = fbAddr->Reg032[0];
    fbAddr->Reg032[0] = FB_TEST_VALUE;                  // write fb or NV_CONFIG_PCI_NV_0 (READ ONLY)
    if((data = fbAddr->Reg032[0]) != FB_TEST_VALUE)     // if we read back, its fb
    {
        // is it possible the device manager gave us physical addresses in the "wrong" order?
        // try swapping them
        data = (U032)nvAddr;
        nvAddr = fbAddr;
        fbAddr = (PHWREG)data;

        fbAddr->Reg032[0] = FB_TEST_VALUE;
        if((data = fbAddr->Reg032[0]) != FB_TEST_VALUE)
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: NV not found at physical address ", pDev->Mapping.PhysAddr);
            return(RM_ERROR);    
        }
    }       
    else    // its framebuffer, restore the data (important if we are secondary display and have a message)
        fbAddr->Reg032[0] = fb_test;

    if (!pDev->Mapping.IntLine)   // we got this IRQ from config mgr. this should never be 0.
    {
        // if BIOS assigned and IRQ,
        // read reg 3C and look for IRQ number       
        pDev->Mapping.IntLine = REG_RD32(NV_PBUS_PCI_NV_15) & 0XFF;
        if (!pDev->Mapping.IntLine)
        {
            //vmmFatalErrorHandler((DWORD)"NVVDD: Unable to connect to IRQ.", 0);
            return (RM_ERROR);
        }
    }

    // 
    // Determine the chip type and initialize the chip-specific data
    // in the pDev structure.
    //

    // The architecture bits changed after NV3, so check the device ID to rule out NV3
    if (REG_RD_DRF(_PBUS, _PCI_NV_0, _DEVICE_ID_CHIP) == 3)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: NV3 found, incorrect driver!\n\r");
        return (RM_ERROR);
    }

    //
    // Hook up data/code from the ROM.
    //
    initHal(pDev);

    // We need to know which chip this is early during boot.
    nvHalMcControl(pDev, MC_CONTROL_SETREVISION);

    //
    // I hate to do this here, but the fbstate isn't called until after we map the
    // chip (of course), but I need to figure out right now how big is our framebuffer.
    //
    // Configure validated framebuffer resolution.  Note that the BIOS will be setting these
    // bits during post, and he uses the extension bit to denote an 8meg framebuffer.  So
    // check that first...
    //
    status = nvHalFbControl(pDev, FB_CONTROL_INIT);

    if (status != RM_OK) //Was FB config set up correctly? Did we find false memory configs?
    {
        return RM_ERROR;
    }

    biosAddr = (PHWREG)((U008*)nvAddr + pDev->Pram.HalInfo.PraminOffset);

    //
    // Enable PPro write-combining using MTRR
    //
    // Framebuffer only, all fb space, fb cache type 2
    //
    // IKOS workaround for NV5/NV10 emulation.
    // If the FB is allowed to be write-combined, bad pixels may be read in during
    // these bursts, so during emulation, don't allow the FB to be write-combined.
    // 
#ifndef IKOS
    if (mtrrGetVersion() && !IsCrush11ChipSet(&crs))
        mtrrSetPhysicalCacheTypeRange(pDev->Mapping.PhysFbAddr, 0, pDev->Framebuffer.HalInfo.RamSize, 2);
#endif
    
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: NV using physical address ", pDev->Mapping.PhysAddr);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: NV using linear   address ", (int)nvAddr);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: FB using physical address ", pDev->Mapping.PhysFbAddr);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: FB using linear   address ", (int)fbAddr);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: NV using IRQ line ", pDev->Mapping.IntLine);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: NV using IRQ pin  ", pDev->Mapping.IntPin);
    //
    // Map the the ROM address based on whether this is a motherboard or add in.
    //
    //
    // Add in NV board map ROM through NV Space.
    //
    romAddr = (PHWREG)((DWORD)nvAddr + DEVICE_BASE(NV_PROM));

    //
    // Some PCI BIOSs leave the ROM mapped.  This causes problems if it overlays
    // system RAM.  Just disable it.
    //
    REG_WR32(NV_PBUS_PCI_NV_12, 0);

    //
    // Make sure our PCI latency timer is sufficient (max it out)
    //
    REG_WR32(NV_PBUS_PCI_NV_3, DRF_NUM(_PBUS,_PCI_NV_3,_LATENCY_TIMER, NV_PBUS_PCI_NV_3_LATENCY_TIMER_248_CLOCKS));

    if (IsALI1541ChipSet(&crs))
    {
        // This chipset has a bug which causes it to read stale data.
        // An IO needs to be done to ensure all caches are flushed before
        // the Put is updated. 
        // This happens on both PCI and AGP.
        pDev->Mapping.doIOToFlushCache = 1;
        
        //
        // This chipset has another bug, confirmed by ALI, where it cannot
        // accept fast DEVSEL# at 100MHz bus speeds.  We cannot adjust our
        // DEVSEL# speeds, but we can delay our TRDY by one clock cycle, which
        // should have the same effect.
        //
        // Note that we (NV) have seen no improvement in any of the ALI1541
        // issues with this change, but ALI is adament there is a bug in the 1541
        // having to do with this logic.  Better safe than sorry.
        //
        // This bit is present in NV4, NV5, and NV10.
        //
        FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _PCIS_WRITE, _1_CYCLE);
        
        //
        // Force the drivers to not use AGP pushing for their command streams.  There is still
        // some unexplained cache coherency issues with memory when lots of AGP traffic is
        // present.  The problems are much less when running only PCI66 (or even PCI33) traffic.
        //
        // Only enable this flag if it doesn't already exist -- that way we have a method to
        // override the default behavior if necessary.  Since you tend not to change your motherboard
        // the first install of our drivers will enable the flag.
        //
        if (osReadRegistryDword(pDev, strDevNodeRM, NV_REG_SYS_SUPER7_COMPATIBILITY, &data) != RM_OK)
        {
            // Write to both local/global keys, since the drivers check in different paths
            osWriteRegistryDword(pDev, strDevNodeRM,        NV_REG_SYS_SUPER7_COMPATIBILITY, 1);
            osWriteRegistryDword(pDev, RM_GLOBAL_BASE_PATH, NV_REG_SYS_SUPER7_COMPATIBILITY, 1);
        }
    }
    else
    {
        // Also reset a stale NV_REG_SYS_SUPER7_COMPATIBILITY registry setting
        if (osReadRegistryDword(pDev, strDevNodeRM, NV_REG_SYS_SUPER7_COMPATIBILITY, &data) == RM_OK)
        {
            // Write to both local/global keys, since the drivers check in different paths
            osWriteRegistryDword(pDev, strDevNodeRM,        NV_REG_SYS_SUPER7_COMPATIBILITY, 0);
            osWriteRegistryDword(pDev, RM_GLOBAL_BASE_PATH, NV_REG_SYS_SUPER7_COMPATIBILITY, 0);
        }
    }
        
    //
    // Attempt to determine the FSB and Memory speeds
    //
    RmInitBusInfo(pDev);

    //
    // Backdoor AGP support.  If this is an AGP card and there is an Intel 440LX/BX in the
    // system, let's detect the current GART settings.  This allows us to use AGP outside
    // of DDraw in Win9X (specifically Win95)
    //
    // Are we AGP?
    //
    pRmInfo->AGP.AGPPhysStart = 0;
    pRmInfo->AGP.AGPLimit = 0;
    pDev->Chip.ReqAGPRate = 0x2;                  // default to AGP 2x    
    
    //
    // Instead of reading the strap, walk the PCI config space.  See if there is an AGP cap
    // after the power mgmt cap.
    //
    //if ((REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_BUS_TYPE) == NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE_AGP))
    if (REG_RD_DRF(_PBUS, _PCI_NV_24, _NEXT_PTR) == NV_PBUS_PCI_NV_24_NEXT_PTR_AGP)
    {
        U032 numRanges = 0;

        // Apply any AGP chipset workarounds or overrides
        NvUpdateAGPConfig(pDev);

        // Set the AGP limit to the WC part of the Aperture
        osUpdateAGPLimit(pDev);

    }  // AGP

#ifdef PC98
    //
    // POST the chip if we're running on a system with no VGA BIOS
    //
    NvPost();
    // set a mode to get VGA running, or else we will hang on DDC
    VBESetMode(0x101, NULL);
#endif // PC98
    
    //
    // Install hardware interrupt routine through PIC device.
    //
    picIRQDesc.VID_IRQ_Number       = (U016) pDev->Mapping.IntLine;
    picIRQDesc.VID_Options          = VPICD_OPT_CAN_SHARE;
    picIRQDesc.VID_Hw_Int_Proc      = (DWORD) osInterrupt;
    picIRQDesc.VID_IRET_Time_Out    = 500;
    picIRQDesc.VID_Virt_Int_Proc    = 0;
    picIRQDesc.VID_EOI_Proc         = 0;
    picIRQDesc.VID_Mask_Change_Proc = 0;
    picIRQDesc.VID_IRET_Proc        = 0;

    //
    // Wait for PMC state change to LOAD before re-enabling the interrupt line.
    //
    CLI
    pDev->Mapping.hpicIRQ = (U032) VPICD_Virtualize_IRQ(&picIRQDesc);
    STI
    VPICD_Physically_Mask(pDev->Mapping.hpicIRQ);
    VPICD_Physically_Unmask(pDev->Mapping.hpicIRQ);
//    picPhysicallyMask(hpicIRQ);
//    picPhysicallyUnmask(hpicIRQ);

    // Check if "DoIOToFlushCache" override exists.
    if (osReadRegistryDword(pDev, strDevNodeRM, "DoIOToFlushCache", &data) == RM_OK)
    {
        pDev->Mapping.doIOToFlushCache = 1;
    } 

    // write the name of the RM VXD in the registry
    if (osWriteRegistryBinary(pDev, strDevNodeRM, strRmCoreName, strRmCoreVxd, osStringLength(strRmCoreVxd)+1) != RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: WriteRegistry of RmCoreName failed\n\r");
    } 

    return (RM_OK);
}

RM_STATUS initFbEx
(
    PHWINFO pDev
)
{
    S032  fbCalcMemSize(VOID);
    U032 calc_max_bw (VOID);
    U032 size;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: initFramebufferEx\n\r");
    //
    // Get screen resolution and framebuffer configuration from registry.
    //
    pDev->Dac.HalInfo.Depth = (osDisplayInfo.InfoFlags & REGISTRY_BPP_NOT_VALID) ? 8 : osDisplayInfo.Bpp;

//
//7/12(DDD) commenting out the ifdef below to fix for reboot keeping at 60Hz bug
// RM guys please fix approriately and remove my comments
// I'm placing these here so somebody will look at this
//
//#ifdef NV3_HW                    
    //
    // BUG WORKAROUND: If the user has set a specific refresh rate via the Win95 OSR2+ control
    // panel, the resulting refresh rate will always return MONITOR_INFO_NOT_VALID.  But the
    // refresh value is correct!  Huh...
    //
    // So until we can find the VDD (?) bug, if the refresh value is valid (>60,<250) for our
    // known refresh settings, let it through.
    //
    if ((osDisplayInfo.InfoFlags & MONITOR_INFO_NOT_VALID) &&
        (osDisplayInfo.InfoFlags & REFRESH_RATE_MAX_ONLY) &&
        (osDisplayInfo.RefreshRateMax >= 60) &&
        (osDisplayInfo.RefreshRateMax <= 250))
            osDisplayInfo.InfoFlags &= ~MONITOR_INFO_NOT_VALID;
//#endif // NV3_HW                    
    if ((osDisplayInfo.InfoFlags & (MONITOR_INFO_NOT_VALID | MONITOR_INFO_DISABLED_BY_USER))
     || (osDisplayInfo.RefreshRateMax == 0xFFFFFFFF)
     || (osDisplayInfo.RefreshRateMax == 0))
    {
        pDev->Framebuffer.HalInfo.RefreshRate  = 60;
        pDev->Framebuffer.HSyncPolarity = BUFFER_HSYNC_NEGATIVE;
        pDev->Framebuffer.VSyncPolarity = BUFFER_HSYNC_NEGATIVE;
    }
    else
    {
        pDev->Framebuffer.HalInfo.RefreshRate  = osDisplayInfo.RefreshRateMax;
        pDev->Framebuffer.HSyncPolarity = osDisplayInfo.HorzSyncPolarity;
        pDev->Framebuffer.VSyncPolarity = osDisplayInfo.VertSyncPolarity;
    }
    // This is because we might have shut down with > 800x600, and then rebooted on TV,
    // in which case the registry will have a resolution we cannot support. 
    // The display driver should call in NvConfigSet to set new resolution. 
    // We should never see HorizDisplayWidth == 0, but just in case, we'll check and set to registry values if it is.
    if (pDev->Framebuffer.HalInfo.HorizDisplayWidth == 0) // display driver should have called NvSetConfig to set these
    {
        pDev->Framebuffer.HalInfo.HorizDisplayWidth = osDisplayInfo.XRes;
        pDev->Framebuffer.HalInfo.VertDisplayWidth  = osDisplayInfo.YRes;
    }
    pDev->Framebuffer.HorizFrontPorch = 0;
    pDev->Framebuffer.HorizSyncWidth  = 0;
    pDev->Framebuffer.HorizBackPorch  = 0;
    pDev->Framebuffer.VertFrontPorch  = 0;
    pDev->Framebuffer.VertSyncWidth   = 0;
    pDev->Framebuffer.VertBackPorch   = 0;
    pDev->Framebuffer.CSync           = BUFFER_CSYNC_DISABLED;
    pDev->Dac.HalInfo.VClk            = 0;
    //
    // Check for valid TV mode
    //
#if 0   // taking this out -- display driver is now responsible
    if (pDev->Framebuffer.MonitorType != MONITOR_TYPE_VGA)
        if (pDev->Framebuffer.HalInfo.HorizDisplayWidth > 800)
        {
            pDev->Framebuffer.HalInfo.HorizDisplayWidth = 640;
            pDev->Framebuffer.HalInfo.VertDisplayWidth  = 480;
        } 
#endif
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

    //
    // Only enable single arch buffering
    //
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
    // Setup the timing information for the initial mode set.
//    osSetupForNewStyleModeSet(pDev);
    return (RM_OK);
}
RM_STATUS initFifoEx
(
    PHWINFO pDev
)
{
//    pDev->Fifo.RetryCount = vmmGetProfileDecimalInt(pDev->Fifo.RetryCount, strNv, "FifoRetryCount");
    return (RM_OK);
}
