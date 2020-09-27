/******************************************************************************
*
*   Module: osvp.c
*
*   Description:
*       This module contains OS services provided to the RM kernel.  These
*       versions are implemented using the Video Port services.
*
*
*   THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
*   NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
*   IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
*
******************************************************************************/

#include <miniport.h>
#include <ntddvdeo.h>
#include <video.h>
#include <dderror.h>
#include <nvrm.h>
#include <os.h>
#include <nvos.h>

#define USE_CURRENT_CONTEXT -1

#ifdef VIDEO_PORT_IMPLEMENTATION
//#if (_WIN32_WINNT >= 0x0500)

// video port service implementation

#if (_WIN32_WINNT >= 0x0500)

RM_STATUS osAllocMem
(
    VOID **pAddress,
    U032   Size
)
{
    VP_STATUS vpStatus = NO_ERROR;
    U008* pMem;
    VOID* hwDeviceExtension = (VOID*)1;

#ifndef DBG

    vpStatus = VideoPortAllocateBuffer
    (
        hwDeviceExtension,
        Size + 4,
        pAddress
    );
    if (vpStatus == NO_ERROR)
    {
        //pDev.Statistics.MemAlloced += Size;
        **(U032 **)pAddress           = Size;
        *(U032 *)pAddress            += 4;
    }
        
#else

    Size += 12;
    vpStatus = VideoPortAllocateBuffer
    (
        hwDeviceExtension,
        Size,
        pAddress
    );
    if (vpStatus == NO_ERROR)
    {
        // memory allocation tracking
        osLogMemAlloc(*pAddress, Size);

        //pDev.Statistics.MemAlloced           += Size - 12;
        **(U032 **)pAddress                     = Size;
        *(U032 *)(*(U032 *)pAddress + 4)        = NV_MARKER1;
        *(U032 *)(*(U032 *)pAddress + Size - 4) = NV_MARKER2;
        *(U032 *)pAddress += 8;
        pMem = (U008 *)*pAddress;
        Size -= 12;
        while (Size--)
            *pMem++ = 0x69;
    }
            
#endif // DBG

    return (vpStatus == NO_ERROR) ? RM_OK : RM_ERR_NO_FREE_MEM;
    
}

RM_STATUS osFreeMem
(
    VOID *pAddress
)
{
    RM_STATUS status = RM_OK;
    U032      Size;
    VOID* hwDeviceExtension = (VOID*)1;
    
#ifndef DBG

    pAddress = (VOID *)((U032)pAddress - 4);
    Size     = *(U032 *)pAddress;
    
#else

    pAddress = (VOID *)((U032)pAddress - 8);
    Size     = *(U032 *)pAddress;
    if (*(U032 *)((U032)pAddress + 4) != NV_MARKER1)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVGFX: Invalid address to osFreeMem\n");
        DBG_BREAKPOINT();
    }
    if (*(U032 *)((U032)pAddress + Size - 4) != NV_MARKER2)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVGFX: Memory overrun in structure to osFreeMem\n");
        DBG_BREAKPOINT();
        return (RM_ERR_MEM_NOT_FREED);
    }
    *(U032 *)((U032)pAddress + 4)        = 'DAED';
    *(U032 *)((U032)pAddress + Size - 4) = 'DEAD';
    Size -= 12;

    // memory allocation tracking
    osUnlogMemAlloc(pAddress);

#endif // DBG  
  
    VideoPortReleaseBuffer(
        hwDeviceExtension,
        pAddress
    );

    //pDev.Statistics.MemAlloced -= Size;

    return status;
    
}

#endif // Win2K

typedef struct _MDL {
    struct _MDL *Next;
    CSHORT Size;
    CSHORT MdlFlags;
    struct _EPROCESS *Process;
    PVOID MappedSystemVa;
    PVOID StartVa;
    ULONG ByteCount;
    ULONG ByteOffset;
} MDL, *PMDL;

#define MDL_MAPPED_TO_SYSTEM_VA     0x0001
#define MDL_PAGES_LOCKED            0x0002
#define MDL_SOURCE_IS_NONPAGED_POOL 0x0004
#define MDL_ALLOCATED_FIXED_SIZE    0x0008
#define MDL_PARTIAL                 0x0010
#define MDL_PARTIAL_HAS_BEEN_MAPPED 0x0020
#define MDL_IO_PAGE_READ            0x0040
#define MDL_WRITE_OPERATION         0x0080
#define MDL_PARENT_MAPPED_SYSTEM_VA 0x0100
#define MDL_LOCK_HELD               0x0200
#define MDL_SCATTER_GATHER_VA       0x0400
#define MDL_IO_SPACE                0x0800
#define MDL_NETWORK_HEADER          0x1000
#define MDL_MAPPING_CAN_FAIL        0x2000
#define MDL_ALLOCATED_MUST_SUCCEED  0x4000
#define MDL_64_BIT_VA               0x8000

RM_STATUS osLockUserMem
(
    PHWINFO             pDev,
    U032                ChID,
    U032                Selector,
    U032                Offset,
    U032                DmaAddress,
    NV_ADDRESS_SPACE*   DmaAddressSpace,
    U032                DmaLength,
    U032                PageCount,
    U032*               PageArray,
    U032*               pLinAddr,
    U032*               pLockHandle
)
{
    RM_STATUS rmStatus;
    PMDL pMdl = 0;
    ULONG i;
    STATUS_BLOCK vrpStatus;
    VIDEO_REQUEST_PACKET vrp;
    PULONG mdlPageArray;

    // map the buffer
    *pLinAddr = DmaAddress;

    //
    // If we're asking to lock video memory, let's just build up the PTE's and assume
    // that the caller has real control over video memory (such as DirectDraw or the
    // display driver).
    //
    if (*DmaAddressSpace == ADDR_FBMEM)
    {
        //
        // This is hardcoded assuming zero = start of fb
        //
        for (i = 0; i < PageCount; i++)
        {
            PageArray[i] = (*pLinAddr + (i * RM_PAGE_SIZE)) | 3;
        }
    }
    else
    {
        // fill a VRP with the buffer information
        vrpStatus.Status = 0;    
        vrpStatus.Information = 0;    
        vrp.IoControlCode = 0;
        vrp.StatusBlock = &vrpStatus;    
        vrp.InputBuffer = (PVOID)DmaAddress;    
        vrp.InputBufferLength = DmaLength;
        vrp.OutputBuffer = NULL;    
        vrp.OutputBufferLength = 0;

        // lock the buffer's pages
        if (
            VideoPortLockPages(
                pDev->DBmpDeviceExtension,
                &vrp,
                NULL,
                NULL,
                VideoPortDmaInitOnly
            )
        )
        {
            // save the MDL pointer
            *pLockHandle = (U032)vrp.OutputBuffer;

            // load the PTE array with physical addresses of the page-aligned buffer
            pMdl = (PMDL)VideoPortGetMdl(
                pDev->DBmpDeviceExtension, 
                (PDMA)pLockHandle
            );
            if (pMdl != NULL)
            {
                // extract the physical address from the MDL into the PTE array
                mdlPageArray = (PULONG)(pMdl + 1);
                for (i = 0; i < PageCount; i++)
                {
                    PageArray[i] = mdlPageArray[i] | 3;
                }
            }
            else 
            {
                VideoPortUnlockPages(pDev->DBmpDeviceExtension, (PDMA)pLockHandle);
                rmStatus = RM_ERROR;
            }
        }
        else 
        {
            rmStatus = RM_ERROR;
        }
    }

    return rmStatus;    
}

RM_STATUS osUnlockUserMem
(
    PHWINFO          pDev,
    U032             ChID,
    VOID*            LinAddr,
    VOID*            DmaAddress,
    NV_ADDRESS_SPACE DmaAddressSpace,
    U032             DmaLength,
    U032             PageCount,
    U032             LockHandle,
    U032             DirtyFlag
)
{
    BOOLEAN status = TRUE;

    //
    // If we're asking to unlock video memory, we're assuming the caller (probably
    // direct draw)  has complete control of video memory, and therefore knows what
    // is locked/unlocked.  No need to do anything.
    //
    if (DmaAddressSpace != ADDR_FBMEM)
    {
        // otherwise, unmap locked pages, unlock pages, and free the MDL
        status = VideoPortUnlockPages(
            pDev->DBmpDeviceExtension,
            (PDMA)&LockHandle
        );
    }

    return (status) ? RM_OK : RM_ERROR;
}

//#endif // (_WIN32_WINNT >= 0x0500)
#endif // VIDEO_PORT_IMPLEMENTATION

RM_STATUS osMapPciMemoryUser( 
    PHWINFO pDev,
    U032 busAddress, 
    U032 length,
    VOID** pVirtualAddress
) 
{
    VP_STATUS vpStatus = NO_ERROR;
    U032 mapFlags;
    PHYSICAL_ADDRESS busPhysicalAddress; 

#if (_WIN32_WINNT >= 0x0500)
    mapFlags = VIDEO_MEMORY_SPACE_P6CACHE;
#else
    mapFlags = VIDEO_MEMORY_SPACE_USER_MODE;
#endif

    // construct an NT physical address from the requested bus-relative address
    busPhysicalAddress.HighPart = 0x00000000;
    busPhysicalAddress.LowPart = busAddress;

    // flag the video port to map into the current context
    *pVirtualAddress = (VOID*)USE_CURRENT_CONTEXT;

    // map the PCI memory into user mode
    vpStatus = VideoPortMapMemory(
        pDev->DBmpDeviceExtension,
        busPhysicalAddress,
        &length,
        &mapFlags,
        pVirtualAddress
    );
    
    return (vpStatus == NO_ERROR) ? RM_OK : RM_ERROR;

}

RM_STATUS osUnmapPciMemoryUser(
    PHWINFO pDev,
    VOID* virtualAddress
)
{
    VP_STATUS vpStatus = NO_ERROR;

    VideoPortUnmapMemory(
        pDev->DBmpDeviceExtension,
        virtualAddress,
        (VOID*)USE_CURRENT_CONTEXT
    );

    return (vpStatus == NO_ERROR) ? RM_OK : RM_ERROR;

}

#if defined(_WIN64)

#define MAX_DMA_LENGTH   (2*1024*1024)

RM_STATUS osInitDmaAdapter
(
    PHWINFO pDev
)
{
    PWINNTHWINFO pOsHwInfo = (PWINNTHWINFO) pDev->pOsHwInfo;

    RM_STATUS rmStatus = RM_OK;
    VP_DEVICE_DESCRIPTION deviceDescription = {
        TRUE,          // ScatterGather;
        TRUE,          // Dma32BitAddresses
        FALSE,         // Dma64BitAddresses
        MAX_DMA_LENGTH // MaximumLength
    };

    pOsHwInfo->dmaAdapter = VideoPortGetDmaAdapter(pDev->DBmpDeviceExtension,
                                                   &deviceDescription);
    if(pOsHwInfo->dmaAdapter == NULL)
        return RM_ERROR;
    else
        return RM_OK;
}

RM_STATUS osAllocSystemPages
(
    PHWINFO pDev,
    VOID **pAddress,
    U032   pageCount,
    U032   ClientClass,
    VOID **pMemData
)
{
    PWINNTHWINFO     pOsHwInfo = (PWINNTHWINFO) pDev->pOsHwInfo;
    PSYS_MEM_INFO    pMemInfo;
    PHYSICAL_ADDRESS logicalAddr;
    RM_STATUS        rmStatus;
    VOID             *pageAddr, *userAddr;

    // Since there will be some cleanup needed on the free side
    // create a struct that hooks into VOID * in CLI_MEMORY_INFO

    rmStatus = osAllocMem((VOID **)&pMemInfo, sizeof(SYS_MEM_INFO));
    if (rmStatus != RM_OK)
        return rmStatus;

    pMemInfo->sysAddr = VideoPortAllocateCommonBuffer(pDev->DBmpDeviceExtension,
                                                      pOsHwInfo->dmaAdapter,
                                                      (pageCount + 1) << RM_PAGE_SHIFT,
                                                      &logicalAddr,
                                                      TRUE,
                                                      &pMemInfo->length);
    if (pMemInfo->sysAddr == NULL) {
        osFreeMem(pMemInfo);
        return RM_ERR_NO_FREE_MEM;
    }
    pMemInfo->logicalAddr = logicalAddr.QuadPart;
    pageAddr = (VOID *)(((NV_UINTPTR_T)pMemInfo->sysAddr + RM_PAGE_MASK) & ~RM_PAGE_MASK);
    pMemInfo->physAddr = (pMemInfo->logicalAddr + RM_PAGE_MASK) & ~RM_PAGE_MASK;

    if ((pMemInfo->clientClass = ClientClass) == NV01_ROOT_USER) {
        userAddr = osMapUserSpace(pageAddr, &pMemInfo->pMdl,
                                  pageCount << RM_PAGE_SHIFT, MAP_CACHED);
        if (userAddr == NULL) {
            VideoPortReleaseCommonBuffer(pDev->DBmpDeviceExtension,
                                         pOsHwInfo->dmaAdapter,
                                         pMemInfo->length,
                                         logicalAddr,
                                         pMemInfo->sysAddr,
                                         TRUE);
            osFreeMem(pMemInfo);
            return RM_ERR_NO_FREE_MEM;
        } else {
           *pAddress = userAddr;
        }
    } else {
       *pAddress = pageAddr;
    }
    *pMemData = (VOID *)pMemInfo;
    return RM_OK;
}

RM_STATUS osFreeSystemPages
(
    PHWINFO pDev,
    VOID **pAddress,
    VOID *pMemData
)
{
    PWINNTHWINFO     pOsHwInfo = (PWINNTHWINFO) pDev->pOsHwInfo;
    PSYS_MEM_INFO    pMemInfo = (PSYS_MEM_INFO)pMemData;
    PHYSICAL_ADDRESS logicalAddr;

    // unmap the memory from user space, if necessary
    if (pMemInfo->clientClass == NV01_ROOT_USER) {
        osUnmapUserSpace(*pAddress, pMemInfo->pMdl);
    }

    // free the memory using the original pointer and actual allocated length
    logicalAddr.QuadPart = pMemInfo->logicalAddr;
    VideoPortReleaseCommonBuffer(pDev->DBmpDeviceExtension,
                                 pOsHwInfo->dmaAdapter, pMemInfo->length,
                                 logicalAddr, pMemInfo->sysAddr,
                                 TRUE);

    // free our data struct that kept track of this mapping
    osFreeMem(pMemInfo);

    return RM_OK;
}

#endif // _WIN64

VOID osWriteReg008(volatile U008* address, U008 data)
{
    VideoPortWriteRegisterUchar((PUCHAR)address, (UCHAR)data);
}

VOID osWriteReg016(volatile U016* address, U016 data)
{
    VideoPortWriteRegisterUshort((PUSHORT)address, (USHORT)data);
}

VOID osWriteReg032(volatile U032* address, U032 data)
{
    VideoPortWriteRegisterUlong((PULONG)address, (ULONG)data);
}

U008 osReadReg008(volatile U008* address)
{
    return VideoPortReadRegisterUchar((PUCHAR)address);
}

U016 osReadReg016(volatile U016* address)
{
    return VideoPortReadRegisterUshort((PUSHORT)address);
}

U032 osReadReg032(volatile U032* address)
{
    return VideoPortReadRegisterUlong((PULONG)address);
}

VP_STATUS osReadRegistryCallback
(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
)
{
    // return the extracted reg value
    *(U032*)Context = *(U032*)ValueData;
    
    return NO_ERROR;

} // end of osReadRegistryCallback()

VP_STATUS osReadRegistryBinaryCallback
(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
)
{
    ULONG i;

    // Grab the avil length from the contexts first element and make
    // sure that the value provided will fit in the buffer
    if (ValueLength > ((U032 *)Context)[0])
    {
        return ERROR_INSUFFICIENT_BUFFER;
    }

    // Let the VP do the value copy
    VideoPortMoveMemory(Context,ValueData,ValueLength);

    return NO_ERROR;

} // end of osReadRegistryBinaryCallback()

RM_STATUS osReadRegistryBinary
(
    PHWINFO pDev,
    char *regDevNode,
    char *regParmStr,
    U008 *Data,
    U032 *cbLen
)
{
    RM_STATUS rmStatus = RM_OK;
    VP_STATUS vpStatus;
    WCHAR parameterName[80];
    U032 i;
    // long word allign the following so we can stuff
    // a buffer length in the first element.  We need
    // to do this as the callback does not provide for
    // a buffer size to be passed in
    U032 localvaluebuffer[64];
 
    // Make sure that we have enough space for the callback data copy
    if (*cbLen > sizeof(localvaluebuffer))
    {
        return RM_ERROR;
    }

     // convert the parameter into a unicode string
    for (i = 0; regParmStr[i]; i++)
    {
        parameterName[i] = regParmStr[i];
    }
    parameterName[i] = (WCHAR)NULL;

    // Stuff the size in the first element
    localvaluebuffer[0] = *cbLen;

    // call the video port to get the parameter value -- calls back to osReadRegistryCallback()
    vpStatus = VideoPortGetRegistryParameters
    (
        pDev->DBmpDeviceExtension,
        parameterName,
        FALSE,
        osReadRegistryBinaryCallback,
        &localvaluebuffer
    );

    // If all is well, fetch the value data and copy it 
    if(vpStatus == NO_ERROR) {

      // Let the VP do the copy and set the size to
      // what the caller suggested
      VideoPortMoveMemory(Data,localvaluebuffer,*cbLen);
    }

    return (vpStatus == NO_ERROR) ? RM_OK : RM_ERROR;
}

RM_STATUS osWriteRegistryBinary
(
    PHWINFO pDev,
    char *regDevNode,
    char *regParmStr,
    U008 *Data,
    U032 cbLen
)
{
    VP_STATUS vpStatus;
    WCHAR parameterName[80];
    U032 i;

     // convert the parameter into a unicode string
    for (i = 0; regParmStr[i]; i++)
    {
        parameterName[i] = regParmStr[i];
    }
    parameterName[i] = (WCHAR)NULL;

    // call the video port to set the parameter value
    vpStatus = VideoPortSetRegistryParameters(
                  pDev->DBmpDeviceExtension,
                  parameterName,
                  Data,
                  cbLen
                  );

    return (vpStatus == NO_ERROR) ? RM_OK : RM_ERROR;
}

RM_STATUS osWriteRegistryDword
(
    PHWINFO pDev,
    char *regDevNode,
    char *regParmStr,
    U032 Data
)
{
    VP_STATUS vpStatus;
    WCHAR parameterName[80];
    U032 i;

     // convert the parameter into a unicode string
    for (i = 0; regParmStr[i]; i++)
    {
        parameterName[i] = regParmStr[i];
    }
    parameterName[i] = (WCHAR)NULL;

    // call the video port to set the parameter value
    vpStatus = VideoPortSetRegistryParameters(
                  pDev->DBmpDeviceExtension,
                  parameterName,
                  &Data,
                  sizeof(ULONG)
                  );

    return (vpStatus == NO_ERROR) ? RM_OK : RM_ERROR;
}

RM_STATUS osReadRegistryDword
(
    PHWINFO pDev,
    char* regDevNode,
    char* regParmStr,
    U032* pData
)
{
    RM_STATUS rmStatus = RM_OK;
    VP_STATUS vpStatus;
    WCHAR parameterName[80];
    U032 i;
    
    // convert the parameter into a unicode string
    for (i = 0; regParmStr[i]; i++)
    {
        parameterName[i] = regParmStr[i];
    }
    parameterName[i] = (WCHAR)NULL;

    // call the video port to get the parameter value -- calls back to osReadRegistryCallback()
    vpStatus = VideoPortGetRegistryParameters
    (
        pDev->DBmpDeviceExtension,
        parameterName,
        FALSE,
        osReadRegistryCallback,
        pData
    );

    return (vpStatus == NO_ERROR) ? RM_OK : RM_ERROR;
    
} // end of osReadRegistryDword()

RM_STATUS osCallVideoBIOS
(
    PHWINFO pDev, 
    U032 *pEAX, 
    U032 *pEBX, 
    U032 *pECX, 
    U032 *pEDX, 
    VOID *pBuffer
)
{
    VP_STATUS vpStatus = NO_ERROR;
    VIDEO_X86_BIOS_ARGUMENTS biosArguments;

    // load the register values and call int10 thru the Video Port
    VideoPortZeroMemory(&biosArguments, sizeof(VIDEO_X86_BIOS_ARGUMENTS));
    biosArguments.Eax = *pEAX;
    biosArguments.Ebx = *pEBX;
    biosArguments.Ecx = *pECX;
    biosArguments.Edx = *pEDX;
    vpStatus = VideoPortInt10(pDev->DBmpDeviceExtension, &biosArguments);
    *pEAX = biosArguments.Eax;
    *pEBX = biosArguments.Ebx;
    *pECX = biosArguments.Ecx;
    *pEDX = biosArguments.Edx;
    
    return (vpStatus == NO_ERROR) ? RM_OK : RM_ERROR;
    
} // end of osCallVideoBIOS()

#if (_WIN32_WINNT >= 0x0500)
RM_STATUS osDelayUs
(
    U032 MicroSeconds
)
{
    VP_STATUS vpStatus = NO_ERROR;

    VideoPortStallExecution(MicroSeconds);

    return (vpStatus == NO_ERROR) ? RM_OK : RM_ERROR;
    
}
#endif

//
// The purpose of this routine is to reset to base VGA display mode
// in preparation for full-screen DOS box, standby, or hibernation.
// 
VP_STATUS RMVideoResetDevice(
    U032 deviceReference
)
{
    U008 cr3B;
    BOOL scratchRestore = FALSE;
    U032 headDisable;
    VP_STATUS vpStatus = NO_ERROR;
    VIDEO_X86_BIOS_ARGUMENTS biosArguments;
    PHWINFO pDev = NvDBPtr_Table[deviceReference];

    /* 
     * Disable the secondary head (dacDisableBroadcast) if currently running in 
     * clone mode before calling the bios to do the mode set.  This will insure
     * that on chips where this feature is broken (nv11) that the secondary
     * head isn't left displaying garbage.  i.e. in a full screen dos box
     * on mobile the crt will display a red bar at the top while the internal
     * flat panel is displaying the dos screen.
     */
    if (pDev->Dac.CrtcInfo[0].PrimaryDevice)
        headDisable = 1;
    else
        headDisable = 0;

    scratchRestore = dacDisableBroadcast(pDev, headDisable, &cr3B);

    // load the register values and call int10 thru the Video Port
    VideoPortZeroMemory(&biosArguments, sizeof(VIDEO_X86_BIOS_ARGUMENTS));
    biosArguments.Eax = 0x0003;
    
    vpStatus = VideoPortInt10(pDev->DBmpDeviceExtension, &biosArguments);


    // Need to check to see if certain devices are enabled
    if(pDev->Power.MobileOperation)
    {
      // Call the BIOS to find out which displays were enabled
      VideoPortZeroMemory(&biosArguments, sizeof(VIDEO_X86_BIOS_ARGUMENTS));
      biosArguments.Eax = 0x4f14;
      biosArguments.Ebx = 0x0085;
      vpStatus = VideoPortInt10(pDev->DBmpDeviceExtension, &biosArguments);

      if(biosArguments.Ebx & 0x1)
      {
          // if the monitor was turned off by power management, make sure it is
          // turned back on.  Usually this happens via the call to do the int 10
          // modeset, but on toshiba, the bios doesn't do it.  So we'll do it here
          // just to make sure it's on.
          // Note:  This is the expected result if a modeset happens at a time when
          // just the monitor has been turned off via power management.
          if (pDev->Dac.DevicesEnabled & DAC_MONITOR_POWER_STATE_OFF)
          {
              mcPowerDisplayState(pDev, MC_POWER_LEVEL_5, 1);
              // delay needed because if it's not here, toshiba will not go completely
              // into standby.  everyone else doesn't care if the delay is here or not.
              // also, 200ms seemed to be the amount of delay needed.  200ms was close, but wasn't enough.
              tmrDelay(pDev, 1000000*300);	// 1ms*300

              pDev->Dac.DevicesEnabled &= ~DAC_MONITOR_POWER_STATE_OFF;
          }
          else
              pDev->Dac.DevicesEnabled |= DAC_PANEL_ENABLED;
      }
      // CRT is not currently used
//      if(biosArguments.Ebx & 0x2)
//          pDev->Dac.DevicesEnabled |= DAC_CRT_ENABLED;
      if(biosArguments.Ebx & 0x4)
          pDev->Dac.DevicesEnabled |= DAC_TV_ENABLED;
    }

    //
    // When just going into a dos box (i.e. display power state is on) we don't
    // want to restore the scratch bits because otherwise the secondary display will
    // get turned back on when win2k does a vga modeset behind our back.
    // However, when this reset is a result of a power management event (specifically standby)
    // and the display power state is off, then we need to restore the register before leaving
    // otherwise when we come out of standby, we won't always resume to the correct display
    // configuration.
    // Can this get any uglier??
    if ((scratchRestore)
     && (pDev->Power.DisplayState[headDisable] == MC_POWER_LEVEL_6))
        dacRestoreBroadcast(pDev, headDisable, cr3B);


    return (vpStatus);

} // end of RMVideoResetDevice()


RM_STATUS osDelay
(
    U032 MilliSeconds
)
{
    VP_STATUS vpStatus = NO_ERROR;

    VideoPortStallExecution(MilliSeconds * 1000);
    
    return (vpStatus == NO_ERROR) ? RM_OK : RM_ERROR;
    
}

#if (_WIN32_WINNT >= 0x0500)
RM_STATUS osIsr
(
    PHWINFO pDev
)
{
    BOOL serviced = FALSE;
    
    if (RmInterruptPending(pDev, &serviced))
    {
        // queue up a DPC for a normal priority interrupt
        //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Queueing interrupt on DPC queue...\n");
        serviced |= VideoPortQueueDpc(pDev->DBmpDeviceExtension, RmDpcForIsr, pDev);
    }
    
    return (serviced) ? RM_OK : RM_ERROR;

} // end of osIsr()

VOID RmDpcForIsr
(
    PVOID HwDeviceExtension,
    PVOID Context
)
{
    PHWINFO pDev = (PHWINFO)Context;
    
    // service queued up interrupts and reenable interrupts
    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Servicing interrupt at DPC level...\n");
    mcService(pDev);
    RmEnableInterrupts(pDev);

} // end of RmDpcForIsr()

#endif // (_WIN32_WINNT >= 0x0500)

#ifdef WIN2K_DDK_THAT_CONTAINS_APIS

RM_STATUS osNotifyEvent
(
    PHWINFO   pDev,
    POBJECT   Object,
    U032      Notifier,
    U032      Method,
    U032      Data,
    RM_STATUS Status,
    U032      Action
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 i, j;
    PEVENTNOTIFICATION NotifyEvent;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVGFX: osNotifyEvent()\n");
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVGFX:   Object =", Object);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVGFX:   Method =", Method);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVGFX:   Data   =", Data);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVGFX:   Status =", Status);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVGFX:   Action =", Action);

    // perform the type of action
    switch (Action)
    {
        case NV_OS_WRITE_THEN_AWAKEN:

            // walk this object's event list and find any matches for this specific notify
            for (NotifyEvent = Object->NotifyEvent; NotifyEvent; NotifyEvent = NotifyEvent->Next)
            {
                if (NotifyEvent->NotifyIndex == Notifier)
                {
                    // found a match -- notify the event
                    switch (NotifyEvent->NotifyType)
                    {
                        case NV01_EVENT_WIN32_EVENT:
                        {
                            // attempt to trigger the event
                            if (!VideoPortSetEvent(pDev->DBmpDeviceExtension, (PEVENT)NotifyEvent->Data.low))
                            {
                                // failed, so invalidate this event so we don't try again
                                NotifyEvent->NotifyType = 0;
                            }
                            break;
                        }

                        case NV01_EVENT_KERNEL_CALLBACK:
                        {
                            MINIPORT_CALLBACK callBackToMiniport = (MINIPORT_CALLBACK)(NotifyEvent->Data.low);

                            // perform a direct callback to the miniport
                            if (callBackToMiniport)
                                callBackToMiniport(pDev->DBmpDeviceExtension);
                            break;
                        }
                    
                        default:
                            break;    
                    }
                }
            }
            break;

        default:

            // any other actions are legacy channel-based notifies
            rmStatus = NVOS10_STATUS_ERROR_BAD_EVENT;
            break;
    }

    return rmStatus;

} // end of osNotifyEvent()

#endif // WIN2K_DDK_THAT_CONTAINS_APIS


// end of smp.c

