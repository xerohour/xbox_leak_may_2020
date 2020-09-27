//
// (C) Copyright NVIDIA Corporation Inc., 1996-2000. All rights reserved.
// (C) Copyright SGS-THOMSON Microelectronics Inc., 1996. All rights reserved.
//
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: OS.C                                                              *
*   This is the OS interface module.  All operating system transactions     *
*   pass through these routines.  No other operating system specific code   *
*   or data should exist in the source.                                     *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)    09/07/93 - wrote it.                    *
*       Jeff Westerinen (jsw)       04/97 - modified for WinNT.             *
*                                                                           *
\***************************************************************************/

#include <ntddk.h>
#include <nvrm.h>
#include <os.h>
#include <nvos.h>
#include <nvhw.h>
#include <Nvcm.h>


// OS names
char nameVideoSink[]  = "CON:";
char nameImageVideo[] = "WND";
char nameSysMem[]     = "SYSMEM:";
char nameLVidMem[]    = "LOCALVIDMEM:";
char nameSVidMem[]    = "SYSVIDMEM:";

// registry names
char strDevNodeRM[]             = "\\Registry\\Machine\\SOFTWARE\\NVIDIA Corporation\\RIVA TNT\\System";
char strTilingOverride[]        = "TilingOverride";
char strMemoryOverride[]        = "MemoryOverride";
char strRTLOverride[]           = "RTLOverride";
char strPBUSOverride[]          = "PBUSOverride";
char strUnderscanXOverride[]    = "UnderscanX";
char strUnderscanYOverride[]    = "UnderscanY";
//char strDevNodeDisplayNumber[]  = "\\Registry\\Machine\\SOFTWARE\\NVIDIA Corporation\\RIVA TNT\\Display\\DeviceX";
char strDevNodeDisplayNumber[]  = "\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\nv4\\Device0";
char strNTSrvPackVersion[]      = "\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\";

// evil globals
KSEMAPHORE rmSemaphore, osSemaphore;
DISPLAYINFO osDisplayInfo;

// memory pools required so no memory is allocated during ISR
PDMALISTELEMENT_POOL_ELEMENT dmaListElementPool;

#if DBG

// memory allocation tracking data structs and globals
typedef struct _MEM_ALLOC_LOG_ENTRY
{
    U032 handle;
    VOID *address;
    U032 size;
    struct _MEM_ALLOC_LOG_ENTRY *next;
    struct _MEM_ALLOC_LOG_ENTRY *last;

} MEM_ALLOC_LOG_ENTRY;
MEM_ALLOC_LOG_ENTRY *memAllocLog = NULL;
MEM_ALLOC_LOG_ENTRY *memAllocLogTail = NULL;
U032 memAllocEntries = 0;
U032 memAllocTotal = 0;
U032 fourByteAllocs = 0;

// instance memory allocation tracking data structs and globals
typedef struct _INST_MEM_ALLOC_LOG_ENTRY
{
    U032 handle;
    U032 instance;
    U032 size;
    U032 alignment;
    struct _INST_MEM_ALLOC_LOG_ENTRY *next;
    struct _INST_MEM_ALLOC_LOG_ENTRY *last;

} INST_MEM_ALLOC_LOG_ENTRY;
INST_MEM_ALLOC_LOG_ENTRY *instMemAllocLog = NULL;
INST_MEM_ALLOC_LOG_ENTRY *instMemAllocLogTail = NULL;
U032 instMemAllocEntries = 0;
U032 instMemAllocTotal = 0;

U032 nvddHeapAllocationCount = 0;
U032 nvddHeapFreeCount = 0;

#endif // DBG    

//
// Some quick and dirty library functions.
// This is an OS function because some operating systems supply their
// own version of this function that they require you to use instead
// of the C library function.  And some OS code developers may decide to
// use the actual C library function instead of this code.  In this case,
// just replace the code within osStringCopy with a call to the C library
// function strcpy.
//
char *osStringCopy
(
    char *dst,
    const char *src
)
{
    char *dstptr = dst;

    while (*dstptr++ = *src++);
    return (dst);
}

S032 osStringCompare
(
    const char *s1,
    const char *s2
)
{
    return strcmp(s1, s2);
}

U032 osStringLength(const char * str)
{
    U032 i = 0;
    while (str[i++] != '\0');
    return i - 1;
}

unsigned char * osMemCopy(unsigned char * dst, const unsigned char * src, U032 length)
{
    U032 i;
    for (i = 0; i < length; i++)
    {
        dst[i] = src[i];
    }
    return dst;
}

//---------------------------------------------------------------------------
//
//  Operating System Memory functions.
//
//---------------------------------------------------------------------------

#ifndef VIDEO_PORT_IMPLEMENTATION
//#if (_WIN32_WINNT < 0x0500)

RM_STATUS  osCopyIn
(
	U008 *src, 
	U008 *target,
	U032 count
)
{
	osMemCopy(target, src, count);

	return(RM_OK);
}

RM_STATUS  osCopyOut
(
	U008 *src,
	U008 *target,
	U032 count
)
{
	osMemCopy(target, src, count);

	return(RM_OK);
}

RM_STATUS osAllocMem
(
    VOID **pAddress,
    U032   Size
)
{
    RM_STATUS status;
    U008     *pMem;
    U032      PageCount;

    status = RM_OK;
    
#if !DBG
    *pAddress = ExAllocatePoolWithTag(NonPagedPool, Size, NV_MEMORY_TAG);
    if (*pAddress != NULL)
    {
#else
    // allocate three extra dwords to hold the size and some debug tags
    Size += 3 * sizeof(U032);

    // round to the nearest dword size to prevent alignment fault on IA64
    Size = (Size + sizeof(U032) - 1) & ~(sizeof(U032) - 1);

    *pAddress = ExAllocatePoolWithTag(NonPagedPool, Size, NV_MEMORY_TAG);
    if (*pAddress != NULL)
    {
        // memory allocation tracking
        osLogMemAlloc(*pAddress, Size);

        // load the size into the first dword, the first tag into the second dword, 
        // and the second tag into the last, then adjust the address
        **(U032**)pAddress                                = Size;
        *(*(U032**)pAddress + 1)                          = NV_MARKER1;
        *(U032*)(*(U008**)pAddress + Size - sizeof(U032)) = NV_MARKER2;
        *(U032**)pAddress += 2;
        pMem = *(U008**)pAddress;
        Size -= 3 * sizeof(U032);
        while (Size--)
            *pMem++ = 0x69;
#endif // DBG
    }
    else
    {
        status = RM_ERR_NO_FREE_MEM;
    }
    return (status);
}
RM_STATUS osFreeMem
(
    VOID *pAddress
)
{
    RM_STATUS status;
    U032      Size;
    U032      PageCount;

#if DBG
    (U032*)pAddress -= 2;
    Size = *(U032*)pAddress;
    if (*((U032*)pAddress + 1) != NV_MARKER1)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVGFX: Invalid address to osFreeMem\n");
        DBG_BREAKPOINT();
    }
    if (*(U032*)((U008*)pAddress + Size - sizeof(U032)) != NV_MARKER2)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVGFX: Memory overrun in structure to osFreeMem\n");
        DBG_BREAKPOINT();
        return (RM_ERR_MEM_NOT_FREED);
    }
    *((U032*)pAddress + 1) = 'DAED';
    *(U032*)((U008*)pAddress + Size - sizeof(U032)) = 'DEAD';
    Size -= 3 * sizeof(U032);

    // memory allocation tracking
    osUnlogMemAlloc(pAddress);

#endif // DBG    
    ExFreePool(pAddress);

    status = RM_OK;
    return (status);
}

#endif // not Win2K

#if DBG

// add a memory allocation log entry
VOID osLogMemAlloc(VOID *address, U032 size)
{
    static U032 memAllocAttempts = 0;
    static U032 memAllocId = 0;

    MEM_ALLOC_LOG_ENTRY *newElement = ExAllocatePoolWithTag(NonPagedPool, sizeof(MEM_ALLOC_LOG_ENTRY), NV_MEMORY_TAG);
    if (newElement)
    {
        // stats
        memAllocEntries++;
        memAllocTotal += size;

        // find the magic 4-byte alloc
        if (size == 0x10)
        {
            fourByteAllocs++;
        }

        // add the new element to the head of the list
        newElement->handle = memAllocId++;
        newElement->address = address;
        newElement->size = size;
        newElement->next = memAllocLog;
        newElement->last = NULL;
        memAllocLog = newElement;

        // set the tail
        if (memAllocLogTail == NULL)
        {
            memAllocLogTail = newElement;
        }

        // fixup the back link of the next element
        if (newElement->next)
        {
            newElement->next->last = newElement;
        }
    }

    // more stats
    memAllocAttempts++;
}

// remove a memory allocation log entry
VOID osUnlogMemAlloc(VOID *address)
{
    static U032 memFreeAttempts = 0;
    MEM_ALLOC_LOG_ENTRY *element;

    for (element = memAllocLog; element; element = element->next)
    {
        if (element->address == address)
        {
            // stats
            memAllocEntries--;
            memAllocTotal -= element->size;

            // unlink the element from the list
            if (element->last)
            {
                element->last->next = element->next;
            }
            else
            {
                memAllocLog = element->next;
            }
            if (element->next)
            {
                element->next->last = element->last;
            }

            // adjust the tail
            if (element == memAllocLogTail)
            {
                memAllocLogTail = element->last;
            }

            // free the element
            ExFreePool(element);
            break;
        }
    }

    // more stats
    memFreeAttempts++;
}

// add a memory allocation log entry
VOID osLogInstMemAlloc(U032 instance, U032 size, U032 alignment)
{
    static U032 instMemAllocAttempts = 0;
    static U032 instMemAllocId = 0;

    INST_MEM_ALLOC_LOG_ENTRY *newElement = ExAllocatePoolWithTag(NonPagedPool, sizeof(INST_MEM_ALLOC_LOG_ENTRY), NV_MEMORY_TAG);
    if (newElement)
    {
        // stats
        instMemAllocEntries++;
        instMemAllocTotal += size;

        // add the new element to the head of the list
        newElement->handle = instMemAllocId++;
        newElement->instance = instance;
        newElement->size = size;
        newElement->alignment = alignment;
        newElement->next = instMemAllocLog;
        newElement->last = NULL;
        instMemAllocLog = newElement;

        // set the tail
        if (instMemAllocLogTail == NULL)
        {
            instMemAllocLogTail = newElement;
        }

        // fixup the back link of the next element
        if (newElement->next)
        {
            newElement->next->last = newElement;
        }
    }

    // more stats
    instMemAllocAttempts++;
}

// remove a memory allocation log entry
VOID osUnlogInstMemAlloc(U032 instance, U032 size)
{
    static U032 instMemFreeAttempts = 0;
    INST_MEM_ALLOC_LOG_ENTRY *element;

    for (element = instMemAllocLog; element; element = element->next)
    {
        if (element->instance == instance)
        {
            // stats
            instMemAllocEntries--;
            instMemAllocTotal -= size;

            // unlink the element from the list
            if (element->last)
            {
                element->last->next = element->next;
            }
            else
            {
                instMemAllocLog = element->next;
            }
            if (element->next)
            {
                element->next->last = element->last;
            }

            // adjust the tail
            if (element == instMemAllocLogTail)
            {
                instMemAllocLogTail = element->last;
            }

            // free the element
            ExFreePool(element);
            break;
        }
    }

    // more stats
    instMemFreeAttempts++;
}

#endif // DBG    

//
// osAllocPages - Allocate a number of aligned memory pages
//
//  **pAddress - handle to the linear address
//  PageCount  - number of pages to allocate
//  MemoryType - memory pool
//  Contiguous - contiguous pages?
//  Cache      - cacheing request
//
RM_STATUS osAllocPages
(
    PHWINFO pDev,
    VOID    **pAddress,
    U032    PageCount,
    U032    MemoryType,
    U032    Contiguous,
    U032    Cache,
    U032    ClientClass,
    VOID    **pMemData
)
{
    *pMemData = NULL;

    switch (MemoryType)
    {
        case NV_MEMORY_TYPE_AGP:
            return(osAllocAGPPages(pDev, pAddress, PageCount, ClientClass, pMemData));
            break;
        case NV_MEMORY_TYPE_SYSTEM:
            return(osAllocSystemPages(pDev, pAddress, PageCount, ClientClass, pMemData));
            break;
        default:
            return RM_ERROR;
    }
}

//
// osFreePages - Free a number of aligned memory pages
//
//  **pAddress - handle to the linear address
//  MemoryType - memory pool
//
RM_STATUS osFreePages
(
    PHWINFO pDev,
    VOID    **pAddress,
    U032    MemoryType,
    U032    PageCount,
    VOID    *pMemData,
    BOOL    IsWriteCombined          // all NT4 AGP allocs are write combined
)
{
    switch (MemoryType)
    {
        case NV_MEMORY_TYPE_AGP:
            return(osFreeAGPPages(pDev, pAddress, pMemData));
            break;
        case NV_MEMORY_TYPE_SYSTEM:
            return(osFreeSystemPages(pDev, pAddress, pMemData));
            break;
        default:
            return RM_ERROR;
    }
}

RM_STATUS osAllocDmaListElement
(
    PDMAUSEROBJECT *ppDmaListElement
)
{
    U032 i;

    // get the next available DMA list elementt
    for (i = 0; i < DMALISTELEMENT_POOL_SIZE; i++)
    {
        if (!dmaListElementPool[i].inUse)
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Allocating DMA list element", i);
            dmaListElementPool[i].inUse = TRUE;
            *ppDmaListElement = &dmaListElementPool[i].dmaListElement;
            return RM_OK;
        }
    }

    return RM_ERROR;

} // end of osAllocDmaListElement()

RM_STATUS osFreeDmaListElement
(
    PDMAUSEROBJECT pDmaListElement
)
{
    U032 i;

    // return the DMA list element to the pool
    for (i = 0; i < DMALISTELEMENT_POOL_SIZE; i++)
    {
        if (pDmaListElement == &dmaListElementPool[i].dmaListElement)
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Freeing DMA list element", i);
            dmaListElementPool[i].inUse = FALSE;
            return RM_OK;
        }
    }

    return RM_ERROR;

} // end of osFreeDmaListElement()

// video port service implementation
#ifndef VIDEO_PORT_IMPLEMENTATION
//#if (_WIN32_WINNT < 0x0500)

RM_STATUS osLockUserMem
(
    PHWINFO          pDev,
    U032             ChID,
    U032             Selector,
    VOID*            Offset,
    VOID*            DmaAddress,
    NV_ADDRESS_SPACE *DmaAddressSpace,
    U032             DmaLength,
    U032             PageCount,
    U032            *PageArray,
    VOID**           pLinAddr,
    VOID**           LockHandle
)
{
    NTSTATUS ntStatus;
    PMDL pMdl = 0;
    ULONG i;

    //
    // If we're asking to lock video memory, let's just build up the PTE's and assume
    // that the caller has real control over video memory (such as DirectDraw or the
    // display driver).
    //
    if (*DmaAddressSpace == ADDR_FBMEM)
    {

        *pLinAddr = Offset;   // 0-based selector!!

        //
        // For FB, the offset into the framebuffer has already been calculated by
        // osGetAddressInfo; don't assume 0, but use Offset as the first physaddr.
        //
        for (i = 0; i < PageCount; i++)
        {
            PageArray[i] = (U032)((NV_UINTPTR_T)Offset + (i * RM_PAGE_SIZE)) | 3;
        }
    }
    else if (*DmaAddressSpace == ADDR_AGPMEM)
    {
        // For AGP, load in the aperture address (we'll only need 1 pte loaded),
        // but since we've allocated PageCount worth, I guess we'll load it all

        for (i = 0; i < PageCount; i++)
        {
            PageArray[i] = (U032)((NV_UINTPTR_T)DmaAddress + (i * RM_PAGE_SIZE)) | 3;
        }
    }
    else
    {
        PHYSICAL_ADDRESS pagePhysicalAddr;
        U008* pageVirtualAddr;

        // lock the pages using an MDL created from the DMA buffer specs
        pMdl = IoAllocateMdl(
            DmaAddress,
            DmaLength,
            FALSE,
            FALSE,
            NULL
        );
        if (pMdl == NULL)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVGFX: Cannot allocate MDL\n");
            return RM_ERR_NO_FREE_MEM;
        }
        try
        {
            // the only way to know if locking fails is via an exception
            MmProbeAndLockPages(
                pMdl,
                KernelMode,
                IoModifyAccess
            );
        }
        except(EXCEPTION_EXECUTE_HANDLER)
        {
            ntStatus = GetExceptionCode();
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVGFX: MmProbeAndLockPages FAILED!\n");
            IoFreeMdl(pMdl);
            return RM_ERR_PAGE_TABLE_NOT_AVAIL;
        }

        // save the MDL pointer
        *LockHandle = pMdl;

        // map the buffer
#if (_WIN32_WINNT < 0x0500)
        *pLinAddr = MmGetSystemAddressForMdl(pMdl);
#else
        *pLinAddr = MmGetSystemAddressForMdlSafe(pMdl, NormalPagePriority);
#endif // (_WIN32_WINNT < 0x0500)
        if (*pLinAddr == NULL)
        {
            MmUnlockPages(pMdl);
            IoFreeMdl(pMdl);
            return RM_ERROR;
        }


        // load the PTE array with physical addresses of the page-aligned buffer
        // note: if memory refered by DmaAddress was paged out (therefore being
        //       invalid) at the time of the lock down call, it might be still
        //       invalid. Therefore MmGetPhysicalAddress(DmaAddress) returns NULL.
        //       (seen on a SMP system / low memory condition)
        //       The recently mapped pLinAddr alias is accessible though.
        pageVirtualAddr = (U008*)((NV_UINTPTR_T)(*pLinAddr) & ~RM_PAGE_MASK);

        for (i = 0; i < PageCount; i++)
        {
            pagePhysicalAddr = MmGetPhysicalAddress(pageVirtualAddr);
            if (pagePhysicalAddr.LowPart == 0)
            {
                MmUnmapLockedPages(*pLinAddr, pMdl);
                MmUnlockPages(pMdl);
                IoFreeMdl(pMdl);
                return RM_ERR_NO_FREE_MEM;
            }
            PageArray[i] = pagePhysicalAddr.LowPart | 3;
            pageVirtualAddr += RM_PAGE_SIZE;
        }
    }

    return (RM_OK);
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
    VOID*            LockHandle,
    U032             DirtyFlag
)
{
    RM_STATUS status = RM_OK;
    PMDL pMdl = (PMDL)LockHandle;

    //
    // If we're asking to unlock video memory, we're assuming the caller (probably
    // direct draw)  has complete control of video memory, and therefore knows what
    // is locked/unlocked.  No need to do anything.
    //
    if ((DmaAddressSpace != ADDR_FBMEM) && (DmaAddressSpace != ADDR_AGPMEM))
    {
        // otherwise, unmap locked pages, unlock pages, and free the MDL
        MmUnmapLockedPages((VOID*)LinAddr, pMdl);
        MmUnlockPages(pMdl);
        IoFreeMdl(pMdl);
    }
    return (status);
}

//#endif // (_WIN32_WINNT < 0x0500)
#endif // VIDEO_PORT_IMPLEMENTATION

RM_STATUS osMapFifo
(
    PHWINFO pDev,
    U032 clientClass,
    U032 ChannelID,
    VOID** pAddress
)
{
    RM_STATUS rmStatus = RM_OK;

    switch (clientClass)
    {
        case NV01_ROOT:
            *pAddress = (VOID*)((U008*)nvAddr + FIFO_USER_BASE(pDev) + (ChannelID << 16));
            break;

        case NV01_ROOT_USER:
            rmStatus = osMapPciMemoryUser(
                pDev,
                pDev->Mapping.PhysAddr + FIFO_USER_BASE(pDev) + (ChannelID << 16),
                CHANNEL_LENGTH,
                pAddress
            );
            break;

        default:
            rmStatus = RM_ERR_OPERATING_SYSTEM;
    }

    return rmStatus;
}

RM_STATUS osMapInstanceMemory
(
    PHWINFO pDev,
    U032 clientClass,
    U032 instanceOffset,
    U032 length,
    VOID** pAddress
)
{
    RM_STATUS rmStatus = RM_OK;

    switch (clientClass)
    {
        case NV01_ROOT:
            *pAddress = INSTANCE_MEMORY_LOGICAL_BASE(pDev) + instanceOffset;
            break;

        case NV01_ROOT_USER:
            rmStatus = osMapPciMemoryUser(
                pDev,
                INSTANCE_MEMORY_PHYSICAL_BASE(pDev) + instanceOffset,
                length,
                pAddress
            );
            break;

        default:
            rmStatus = RM_ERR_OPERATING_SYSTEM;
    }

    return rmStatus;
}

RM_STATUS osMapFrameBuffer
(
    PHWINFO pDev,
    U032 clientClass,
    U032 fbOffset,
    U032 length,
    VOID** pAddress
)
{
    RM_STATUS rmStatus = RM_OK;

    switch (clientClass)
    {
        case NV01_ROOT:
            *pAddress = (U008*)fbAddr + fbOffset;
            break;

        case NV01_ROOT_USER:
            rmStatus = osMapPciMemoryUser(
                pDev,
                pDev->Mapping.PhysFbAddr + fbOffset,
                length,
                pAddress
            );
            break;

        default:
            rmStatus = RM_ERR_OPERATING_SYSTEM;
    }

    return rmStatus;
}

// This procedure undoes the work of osMapFifo, osMapFrameBuffer and osMapInstanceMemory
RM_STATUS osUnmapMemory
(
    PHWINFO pDev,
    U032    clientClass,
    VOID*   address,
    U032    length
)
{
    RM_STATUS rmStatus = RM_OK;

    switch (clientClass)
    {
        case NV01_ROOT:
            break;

        case NV01_ROOT_USER:
            rmStatus = osUnmapPciMemoryUser(pDev, address);
            break;

        default:
            rmStatus = RM_ERR_OPERATING_SYSTEM;
    }

    return rmStatus;
}

RM_STATUS osSetFifoDmaParams
(
    PHWINFO pDev,
    U032   *pFetchTrigger,
    U032   *pFetchSize,
    U032   *pFetchRequests
)
{

    //
    // Default NT DMA channel fetch parameters
    // (optimized for NT display driver).
    //
    *pFetchTrigger = 128;           // 128 bytes
    *pFetchSize = 128;              // 128 bytes
    *pFetchRequests = 8;            // 8 outstanding read requests

    return RM_OK;
}

RM_STATUS osDeviceNameToDeviceClass(char *osName, U032 *pDevClass)
{
    // TO DO: add a validate pointer OS service
    // validate OS name parameter
    try
    {
        U008 testData = *(U008*)osName;
    }
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        NTSTATUS ntStatus = GetExceptionCode();
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVGFX: invalid OS name\n");
        return RM_ERROR;
    }

    switch (*pDevClass)
    {
        // explicit device classes need no processing
        case NV01_DEVICE_0:
        case NV01_DEVICE_1:
        case NV01_DEVICE_2:
        case NV01_DEVICE_3:
        case NV01_DEVICE_4:
        case NV01_DEVICE_5:
        case NV01_DEVICE_6:
        case NV01_DEVICE_7:
            break;

        // derive the device class from the OS name, for NT, the device reference
        case NV03_DEVICE_XX:
            switch (*(U032*)osName)
            {
                case 0:
                    *pDevClass = NV01_DEVICE_0;
                    break;

                case 1:
                    *pDevClass = NV01_DEVICE_1;
                    break;

                case 2:
                    *pDevClass = NV01_DEVICE_2;
                    break;

                case 3:
                    *pDevClass = NV01_DEVICE_3;
                    break;

                case 4:
                    *pDevClass = NV01_DEVICE_4;
                    break;

                case 5:
                    *pDevClass = NV01_DEVICE_5;
                    break;

                case 6:
                    *pDevClass = NV01_DEVICE_6;
                    break;

                case 7:
                    *pDevClass = NV01_DEVICE_7;
                    break;

                // invalid escape value
                default:
                    return RM_ERROR;
            }
            break;

        // invalid class
        default:
            return RM_ERROR;

    }

    return RM_OK;

}

//---------------------------------------------------------------------------
//
//  OS specific time of day.
//
//---------------------------------------------------------------------------

/*****************************************************************************
*
*   Name: osGetCurrentTime
*
*   Description:
*       This functions returns the number of nano seconds that have ellapsed
*   since January 1 at midnight, GMT, of the given base year.  The base year
*   is passed in year, and the elapsed time is returned in nSecElapsed.  The
*   rest of the parameters are filled with the current date and time.
*
*****************************************************************************/

RM_STATUS osGetCurrentTime
(
    U032 *year,
    U032 *month,
    U032 *day,
    U032 *hour,
    U032 *min,
    U032 *sec,
    U032 *msec
)
{
    TIME_FIELDS currentTime;
    LARGE_INTEGER timeIn100ns;

    // get the current elapsed time
    KeQuerySystemTime(&timeIn100ns);

    // set the current time fields
    RtlTimeToTimeFields(&timeIn100ns, &currentTime);
    *year   = currentTime.Year;
    *month  = currentTime.Month;
    *day    = currentTime.Day;
    *hour   = currentTime.Hour;
    *min    = currentTime.Minute;
    *sec    = currentTime.Second;
    *msec   = currentTime.Milliseconds;

    return RM_OK;
}

//---------------------------------------------------------------------------
//
//  Misc services.
//
//---------------------------------------------------------------------------


// Pre and Post modeset stuff, from the 9x stuff
// Os Specifix pre mode set actions.
VOID osPreModeSetEx(PHWINFO pDev, U032 Head)
{

    // Nothing to do here any more...all TV stuff is in the main
    // modeset path in dac/dacmode.c:dacSetModeMulti().

} // end of osPreModeSetEx()

// Os Specific post mode set actions.
VOID osPostModeSetEx(PHWINFO pDev, U032 Head)
{
    RM_STATUS status;
    U032      data32;

    // See if there are any registry overrides for the desktop positioning.
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_MONITOR) {
        U032 cbLen;
        NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_PARAMS position;

        cbLen = 4 * sizeof(U032);
        status = dacReadDesktopPositionFromRegistry(pDev, Head,
                                                    TRUE,  // isMonitor
                                                    (U008 *) &(position.HRetraceStart),
                                                    &cbLen);
        if (status == RM_OK) {
            // Program the new values.
            dacSetMonitorPosition(pDev, Head,
                                  position.HRetraceStart, position.HRetraceEnd,
                                  position.VRetraceStart, position.VRetraceEnd);
        }
    }

    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV)
        dacSetBiosDefaultTVType(pDev, pDev->Dac.TVStandard);

    // See if there are any registry overrides for the monitor timing.
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_MONITOR)
    {
        NV_CFGEX_CRTC_TIMING_PARAMS timings;

        //
        // First check if there are timings available in the registry for this mode
        //
        timings.Head = Head;
        timings.Reg = NV_CFGEX_CRTC_TIMING_REGISTRY;
        status = dacGetCRTCTiming(pDev, Head, &timings);

        if (status == RM_OK)
        {
            //
            // Program the new values
            //
            timings.Reg = NV_CFGEX_CRTC_TIMING_REGISTER;
            status = dacSetCRTCTiming(pDev, Head, &timings);
        }

    }

    // See if there are any registry overrides for the TV desktop positioning.
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV) {

       U032 cbLen;
       NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS Params;
 
       // Read the registry to get any updates to the default setting.
       // This should be done after dacAdjustCRTCForTV because it involves changes to CRTC.
       cbLen = sizeof(NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS);
       status = dacReadTVDesktopPositionFromRegistry(pDev, Head, (U008 *) &Params, &cbLen);
   
       // Make sure data is valid for the current encoder.
       if((status == RM_OK) && (Params.Encoder_ID == pDev->Dac.EncoderType)) {
           // Write the saved settings
           dacSetTVPosition(pDev, Head, (U032 *) &Params);
       }
       if (!pDev->Power.MobileOperation)
       {
           dacWriteTVStandardToRegistry(pDev, pDev->Dac.TVStandard);   // save TV standard in registry
       }
    }
    
    // Check/Set any PFB overrides
    if (osReadRegistryDword(pDev, strDevNodeRM, strMemoryOverride, &data32) == RM_OK)
    {
        nvHalFbLoadOverride(pDev, FB_LOAD_OVERRIDE_MEMORY, data32);
    }
    if (osReadRegistryDword(pDev, strDevNodeRM, strRTLOverride, &data32) == RM_OK)
    {
        nvHalFbLoadOverride(pDev, FB_LOAD_OVERRIDE_RTL, data32);
    }
    
} // end of osPostModeSetEx()





//
// First pass at implementing registry string read.
// Converts the UNICODE string from registry into a POS (plain old string)
// of max length '*cbLen' and returns it in buffer pointed to by '*Data'
//
// Currently just used by RM_FAILURE mechanism in rmfailure.c
//
// Note:
//    has a lot in common w/ osReadRegistryDword
//       could be commoned once it works :-)
//    missing pDev arg of osReadRegistryDword()
//    can not be called from RmInitRm(); it is apparently too early
//        if you do, will just return STATUS_NOT_FOUND
//
RM_STATUS osReadRegistryString
(
    char *regDevNode,
    char *regParmStr,
    char *Data,
    U032 *cbLen
)
{
    NTSTATUS ntStatus;
    RM_STATUS rmStatus;
    RTL_QUERY_REGISTRY_TABLE query[2];
    STRING sRegDevNode, sRegParmStr;
    UNICODE_STRING usRegDevNode, usRegParmStr;
    WCHAR buffer1[] = L"                                                                                ";
    WCHAR buffer2[] = L"                                                                                ";
    UNICODE_STRING dest_unicode_string;
    ANSI_STRING dest_ansi_string;

    // convert the parameters into unicode strings
    RtlInitAnsiString(&sRegDevNode, regDevNode);
    RtlInitUnicodeString(&usRegDevNode, buffer1);
    RtlAnsiStringToUnicodeString(&usRegDevNode, &sRegDevNode, FALSE);
    RtlInitAnsiString(&sRegParmStr, regParmStr);
    RtlInitUnicodeString(&usRegParmStr, buffer2);
    RtlAnsiStringToUnicodeString(&usRegParmStr, &sRegParmStr, FALSE);

    // create a trivial registry-query table (1 query + NULL terminator query)
    RtlZeroMemory(query, sizeof(query));
    query[0].Name = usRegParmStr.Buffer;
    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].DefaultType = REG_SZ;

    dest_unicode_string.Length = 0;
    dest_unicode_string.MaximumLength = (short) *cbLen;
    dest_unicode_string.Buffer = ExAllocatePoolWithTag(
                                        PagedPool,
                                        dest_unicode_string.MaximumLength, 
                                        NV_MEMORY_TAG
                                        );
    
    if (!dest_unicode_string.Buffer) 
    {
        return RM_ERROR;
    }

    query[0].DefaultData = dest_unicode_string.Buffer;
    query[0].DefaultLength = 0;
    query[0].EntryContext = &dest_unicode_string;

    // query the registry
    ntStatus = RtlQueryRegistryValues(
        RTL_REGISTRY_ABSOLUTE,
        usRegDevNode.Buffer,
        query,
        NULL,
        NULL	// environment unused
    );

    if((NT_SUCCESS(ntStatus)) && (dest_unicode_string.Length != 0))
    {
        dest_ansi_string.Length = 0;
        dest_ansi_string.MaximumLength = (unsigned short) *cbLen;
        // convert it into caller's buffer
        dest_ansi_string.Buffer = Data;
        RtlUnicodeStringToAnsiString(&dest_ansi_string,
                                 &dest_unicode_string,
                                 0 /* don't allocate dest */);
        ExFreePool(dest_unicode_string.Buffer);

        *cbLen = dest_ansi_string.Length;

        return RM_OK;
    }
    else
    {
        ExFreePool(dest_unicode_string.Buffer);
        return RM_ERROR;
    }

}


#if (_WIN32_WINNT < 0x0500)

RM_STATUS osDelayUs
(
    U032 MicroSeconds
    )
{
    // convert uSec to a relative (negative) interval of 100's of nSecs
    LARGE_INTEGER hundredsOfNanoSeconds = RtlConvertLongToLargeInteger(MicroSeconds * -10);

    // suspend for given relative time interval
    NTSTATUS ntStatus = KeDelayExecutionThread(
        KernelMode,
        FALSE,
        &hundredsOfNanoSeconds
    );

    return (NT_SUCCESS(ntStatus)) ? RM_OK : RM_ERROR;
}


#ifdef NOT_USING_VIDEOPORT_REGISTRY_CALLS

RM_STATUS osWriteRegistryBinary
(
    PHWINFO pDev,
    char *regDevNode,
    char *regParmStr,
    U008 *Data,
    U032 cbLen
)
{

	NTSTATUS ntStatus;
	RM_STATUS rmStatus;
	STRING sRegDevNode, sRegParmStr;
	UNICODE_STRING usRegDevNode, usRegParmStr;
	WCHAR buffer1[] = L"                                                                                ";
	WCHAR buffer2[] = L"                                                                                ";


	// convert the parameters into unicode strings
	RtlInitAnsiString(&sRegDevNode, regDevNode);
	RtlInitUnicodeString(&usRegDevNode, buffer1);
	RtlAnsiStringToUnicodeString(&usRegDevNode, &sRegDevNode, FALSE);
	RtlInitAnsiString(&sRegParmStr, regParmStr);
	RtlInitUnicodeString(&usRegParmStr, buffer2);
	RtlAnsiStringToUnicodeString(&usRegParmStr, &sRegParmStr, FALSE);

	// Write the registry value 
	ntStatus = RtlWriteRegistryValue(
		RTL_REGISTRY_ABSOLUTE,
		usRegDevNode.Buffer,
		usRegParmStr.Buffer,
		REG_BINARY,
		Data,
		cbLen
	);
    if (!NT_SUCCESS(ntStatus))
    	return RM_ERROR;

	return RM_OK;

}

//
// First pass at implementing registry binary read.
// Returns binary data
// of max length '*cbLen' and returns it in buffer pointed to by '*Data'
//
// 'Data' buffer must be 8 bytes longer than your real data as it is coerced
//     into a data structure used by registry.
//
// The union of a binary registry data record, that contains the size and type
// along with the data, and a pure UCHAR buffer is used here.
//
// Note:
//    has a lot in common w/ osReadRegistryDword
//       could be commoned once it works :-)
//    missing pDev arg of osReadRegistryDword()
//    can not be called from RmInitRm(); it is apparently too early
//        if you do, will just return STATUS_NOT_FOUND
//

RM_STATUS osReadRegistryBinary
(
    PHWINFO pDev,
    char *regDevNode,
    char *regParmStr,
    U008 *Data,
    U032 *cbLen
)
{

    NTSTATUS ntStatus;
    RM_STATUS rmStatus;
    RTL_QUERY_REGISTRY_TABLE query[2];
    STRING sRegDevNode, sRegParmStr;
    UNICODE_STRING usRegDevNode, usRegParmStr;
    WCHAR buffer1[] = L"                                                                                ";
    WCHAR buffer2[] = L"                                                                                ";
    RM_REG_BINARY_BUFFER localbuf;

    // is the buffer big enough to hold the data and the length/type fields?
    // TO DO: Is there a way so send a more helpful error code?
    if (*cbLen + 8 > sizeof(RM_REG_BINARY_BUFFER))
    {
        return RM_ERROR;
    }

    // convert the parameters into unicode strings
    RtlInitAnsiString(&sRegDevNode, regDevNode);
    RtlInitUnicodeString(&usRegDevNode, buffer1);
    RtlAnsiStringToUnicodeString(&usRegDevNode, &sRegDevNode, FALSE);
    RtlInitAnsiString(&sRegParmStr, regParmStr);
    RtlInitUnicodeString(&usRegParmStr, buffer2);
    RtlAnsiStringToUnicodeString(&usRegParmStr, &sRegParmStr, FALSE);

    // create a trivial, no callback, registry-query table (1 query + NULL terminator query)
    RtlZeroMemory(query, sizeof(query));
    query[0].Name = usRegParmStr.Buffer;
    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].EntryContext = localbuf.buffer;
    query[0].DefaultType = REG_NONE;
    query[0].DefaultData = localbuf.buffer;
    query[0].DefaultLength = sizeof(localbuf.buffer) - 8;   // Minus the size of the header that is prepended
    
    
    // - Prep the query
    // Zero the local buffer in case the return size is less than the expected size, as someone might get back
    // less than they expected, and at this point, we return them the size that they expected
    RtlZeroMemory(localbuf.buffer, sizeof(localbuf.buffer));
    localbuf.record.Length = sizeof(localbuf.buffer) - 8;   // Minus the size of the header that is prepended
    localbuf.record.type = REG_NONE;

    // query the registry
    ntStatus = RtlQueryRegistryValues(
        RTL_REGISTRY_ABSOLUTE,
        usRegDevNode.Buffer,
        query,
        NULL,
        NULL
    );
    if (!NT_SUCCESS(ntStatus)) {

        return RM_ERROR;
    }

    // Make sure that NT got the value type right...
    if( localbuf.record.type != REG_BINARY) {

       return RM_ERROR;
    }

	 // shift data out to Data using a copy that works on overlapping buffers
    // For some reason, the length value handed back from Win2K is incorrect, but correct under WinNT4, so use the users size
    RtlMoveMemory(Data, localbuf.record.buffer, *cbLen);

    return RM_OK;

}

RM_STATUS osWriteRegistryDword
(
    PHWINFO pDev,
    char *regDevNode,
    char *regParmStr,
    U032 Data
)
{

	NTSTATUS ntStatus;
	RM_STATUS rmStatus;
	STRING sRegDevNode, sRegParmStr;
	UNICODE_STRING usRegDevNode, usRegParmStr;
	WCHAR buffer1[] = L"                                                                                ";
	WCHAR buffer2[] = L"                                                                                ";


	// convert the parameters into unicode strings
	RtlInitAnsiString(&sRegDevNode, regDevNode);
	RtlInitUnicodeString(&usRegDevNode, buffer1);
	RtlAnsiStringToUnicodeString(&usRegDevNode, &sRegDevNode, FALSE);
	RtlInitAnsiString(&sRegParmStr, regParmStr);
	RtlInitUnicodeString(&usRegParmStr, buffer2);
	RtlAnsiStringToUnicodeString(&usRegParmStr, &sRegParmStr, FALSE);

	// Write the registry value 
	ntStatus = RtlWriteRegistryValue(
		RTL_REGISTRY_ABSOLUTE,
		usRegDevNode.Buffer,
		usRegParmStr.Buffer,
		REG_DWORD,
		&Data,
		sizeof(U032)
	);
    if (!NT_SUCCESS(ntStatus))
    	return RM_ERROR;

	return RM_OK;

}

RM_STATUS osReadRegistryDword
(
    PHWINFO pDev,
    char *regDevNode,
    char *regParmStr,
    U032 *Data
)
{

    NTSTATUS ntStatus;
    RM_STATUS rmStatus;
    RTL_QUERY_REGISTRY_TABLE query[2];
    STRING sRegDevNode, sRegParmStr;
    UNICODE_STRING usRegDevNode, usRegParmStr;
    WCHAR buffer1[] = L"                                                                                ";
    WCHAR buffer2[] = L"                                                                                ";
    U032 errorData = 0x69696969;

    // init Data to a known-bad value
    *Data = errorData;

    // convert the parameters into unicode strings
    RtlInitAnsiString(&sRegDevNode, regDevNode);
    RtlInitUnicodeString(&usRegDevNode, buffer1);
    RtlAnsiStringToUnicodeString(&usRegDevNode, &sRegDevNode, FALSE);
    RtlInitAnsiString(&sRegParmStr, regParmStr);
    RtlInitUnicodeString(&usRegParmStr, buffer2);
    RtlAnsiStringToUnicodeString(&usRegParmStr, &sRegParmStr, FALSE);

    // create a trivial registry-query table (1 query + NULL terminator query)
    RtlZeroMemory(query, sizeof(query));
    query[0].Name = usRegParmStr.Buffer;
    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].DefaultType = REG_DWORD;
    query[0].EntryContext = Data;

    // query the registry
    ntStatus = RtlQueryRegistryValues(
        RTL_REGISTRY_ABSOLUTE,
        usRegDevNode.Buffer,
        query,
        NULL,
        NULL
    );
    if (!NT_SUCCESS(ntStatus))
        return RM_ERROR;

    return (*Data != errorData) ? RM_OK : RM_ERROR;

}

#endif // NOT_USING_VIDEOPORT_REGISTRY_CALLS

RM_STATUS osIsr
(
    PHWINFO pDev
)
{
    BOOL serviced = FALSE;
    
    if (RmInterruptPending(pDev, &serviced))
    {
        // service any remaining interrupt and reenable ints
        //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVGFX: Servicing non-VBLANK interrupt...\n");
        mcService(pDev);
        RmEnableInterrupts(pDev);
        serviced = TRUE;
    }
    
    return (serviced) ? RM_OK : RM_ERROR;

} // end of osIsr()

#endif // (_WIN32_WINNT < 0x0500)


RM_STATUS
osMemSet(VOID *addr, U032 fill, U032 bytes)
{
    memset(addr, fill, bytes);
    return RM_OK;
}

// Win2K processor synchronization
#if (_WIN32_WINNT >= 0x0500)

RM_STATUS osInitSpinLock
(
    PHWINFO pDev
)
{
	PWINNTHWINFO pOsHwInfo = (PWINNTHWINFO) pDev->pOsHwInfo;

    RM_STATUS rmStatus = RM_OK;
    
    rmStatus = osAllocMem(&pOsHwInfo->pRmSpinLock, sizeof(KSPIN_LOCK));
    if (rmStatus == RM_OK)
    {
        KeInitializeSpinLock((PKSPIN_LOCK)(pOsHwInfo->pRmSpinLock));
    }
    
    return rmStatus;
    
} // end of osInitSpinLock()
    
RM_STATUS osEnterCriticalCode
(
    PHWINFO pDev 
)
{
	PWINNTHWINFO pOsHwInfo = (PWINNTHWINFO) pDev->pOsHwInfo;

    KeAcquireSpinLock
    (
        (PKSPIN_LOCK)(pOsHwInfo->pRmSpinLock),
        (PKIRQL)&pOsHwInfo->oldIrql
    );
    return RM_OK;
    
} // end of osAcquireSpinLock()

RM_STATUS osExitCriticalCode
(
    PHWINFO pDev
)
{
	PWINNTHWINFO pOsHwInfo = (PWINNTHWINFO) pDev->pOsHwInfo;

    KeReleaseSpinLock
    (
        (PKSPIN_LOCK)(pOsHwInfo->pRmSpinLock),
        (KIRQL)pOsHwInfo->oldIrql
    );
    return RM_OK;
    
} // end of osReleaseSpinLock()

#endif // (_WIN32_WINNT >= 0x0500)

#define rdtsc __asm _emit 0x0f __asm _emit 0x31 

// Return frequency value in MHz
U032 osGetCpuFrequency()
{
    U032 t1, t2;
    U032 tsc1, tsc2;

// TO DO: what to do about inline asm for IA-64?       
#ifndef IA64
    // read the tsc
    _asm {
        push    eax
        push    edx
        rdtsc
        mov     DWORD PTR [tsc1], eax
        pop     edx
        pop     eax
    }

    osDelay(100);

    // read the tsc again
    _asm {
        push    eax
        push    edx
        rdtsc
        mov     DWORD PTR [tsc2], eax
        pop     edx
        push    eax
    }
#else     
tsc1 = 0;
tsc2 = 0;
#endif // IA-64 cannot tolerate inline asm

    // multiply returned value by 10 since we timed 1/10th second
    // and divide the result to convert from Hz to MHz
    return (U032) ((tsc2-tsc1) * 10) / (1000000);
}

#define NT_HANDLE_PCI_DEVICE_SHIFT     8
#define NT_HANDLE_PCI_FUNC_SHIFT      16

//
//  PCI config space read/write routines.
//
U032
osPciReadDword(VOID *handle, U032 offset)
{
    U032 bus;
    U032 retval;
    U032 ihandle;
    PCI_SLOT_NUMBER slot;

    ihandle = (U032)((NV_UINTPTR_T)handle);

    bus  = ihandle & 0xFF;
    slot.u.AsULONG = 0x0;    // make sure all fields are initialized
    slot.u.bits.DeviceNumber   = (ihandle >> NT_HANDLE_PCI_DEVICE_SHIFT) & 0xFF;
    slot.u.bits.FunctionNumber = (ihandle >> NT_HANDLE_PCI_FUNC_SHIFT) & 0xFF;

    HalGetBusDataByOffset(PCIConfiguration,     // bus data type
                          bus,                  // bus number
                          slot.u.AsULONG,       // slot number
                          &retval,              // buffer
                          offset,               // config space offset
                          sizeof(retval));
    return retval;
}

U016
osPciReadWord(VOID *handle, U032 offset)
{
    U032 bus;
    U016 retval;
    U032 ihandle;
    PCI_SLOT_NUMBER slot;

    ihandle = (U032)((NV_UINTPTR_T)handle);

    bus  = ihandle & 0xFF;
    slot.u.AsULONG = 0x0;    // make sure all fields are initialized
    slot.u.bits.DeviceNumber   = (ihandle >> NT_HANDLE_PCI_DEVICE_SHIFT) & 0xFF;
    slot.u.bits.FunctionNumber = (ihandle >> NT_HANDLE_PCI_FUNC_SHIFT) & 0xFF;

    HalGetBusDataByOffset(PCIConfiguration,     // bus data type
                          bus,                  // bus number
                          slot.u.AsULONG,       // slot number
                          &retval,              // buffer
                          offset,               // config space offset
                          sizeof(retval));
    return retval;
}

U008
osPciReadByte(VOID *handle, U032 offset)
{
    U032 bus;
    U008 retval;
    U032 ihandle;
    PCI_SLOT_NUMBER slot;

    ihandle = (U032)((NV_UINTPTR_T)handle);

    bus  = ihandle & 0xFF;
    slot.u.AsULONG = 0x0;    // make sure all fields are initialized
    slot.u.bits.DeviceNumber   = (ihandle >> NT_HANDLE_PCI_DEVICE_SHIFT) & 0xFF;
    slot.u.bits.FunctionNumber = (ihandle >> NT_HANDLE_PCI_FUNC_SHIFT) & 0xFF;

    HalGetBusDataByOffset(PCIConfiguration,     // bus data type
                          bus,                  // bus number
                          slot.u.AsULONG,       // slot number
                          &retval,              // buffer
                          offset,               // config space offset
                          sizeof(retval));
    return retval;
}

VOID
osPciWriteDword(VOID *handle, U032 offset, U032 value)
{
    U032 bus;
    U032 ihandle;
    PCI_SLOT_NUMBER slot;

    ihandle = (U032)((NV_UINTPTR_T)handle);

    bus  = ihandle & 0xFF;
    slot.u.AsULONG = 0x0;    // make sure all fields are initialized
    slot.u.bits.DeviceNumber   = (ihandle >> NT_HANDLE_PCI_DEVICE_SHIFT) & 0xFF;
    slot.u.bits.FunctionNumber = (ihandle >> NT_HANDLE_PCI_FUNC_SHIFT) & 0xFF;

    HalSetBusDataByOffset(PCIConfiguration,     // bus data type
                          bus,                  // bus number
                          slot.u.AsULONG,       // slot number
                          &value,               // buffer
                          offset,               // config space offset
                          sizeof(value));
}

VOID
osPciWriteWord(VOID *handle, U032 offset, U016 value)
{
    U032 bus;
    U032 ihandle;
    PCI_SLOT_NUMBER slot;

    ihandle = (U032)((NV_UINTPTR_T)handle);

    bus  = ihandle & 0xFF;
    slot.u.AsULONG = 0x0;    // make sure all fields are initialized
    slot.u.bits.DeviceNumber   = (ihandle >> NT_HANDLE_PCI_DEVICE_SHIFT) & 0xFF;
    slot.u.bits.FunctionNumber = (ihandle >> NT_HANDLE_PCI_FUNC_SHIFT) & 0xFF;

    HalSetBusDataByOffset(PCIConfiguration,     // bus data type
                          bus,                  // bus number
                          slot.u.AsULONG,       // slot number
                          &value,               // buffer
                          offset,               // config space offset
                          sizeof(value));
}

VOID
osPciWriteByte(VOID *handle, U032 offset, U008 value)
{
    U032 bus;
    U032 ihandle;
    PCI_SLOT_NUMBER slot;

    ihandle = (U032)((NV_UINTPTR_T)handle);

    bus  = ihandle & 0xFF;
    slot.u.AsULONG = 0x0;    // make sure all fields are initialized
    slot.u.bits.DeviceNumber   = (ihandle >> NT_HANDLE_PCI_DEVICE_SHIFT) & 0xFF;
    slot.u.bits.FunctionNumber = (ihandle >> NT_HANDLE_PCI_FUNC_SHIFT) & 0xFF;

    HalSetBusDataByOffset(PCIConfiguration,     // bus data type
                          bus,                  // bus number
                          slot.u.AsULONG,       // slot number
                          &value,               // buffer
                          offset,               // config space offset
                          sizeof(value));
}

VOID *
osPciInitHandle(U008 bus, U008 device, U008 func, U016 *pVendorId, U016 *pDeviceId)
{
    U032 ihandle, vendevID, status;
    PCI_SLOT_NUMBER slot;

    slot.u.AsULONG = 0x0;    // make sure all fields are initialized
    slot.u.bits.DeviceNumber   = device;
    slot.u.bits.FunctionNumber = func;

    // get vendor/device id at offset 0
    status = HalGetBusDataByOffset(PCIConfiguration,     // bus data type
                                   bus,                  // bus number
                                   slot.u.AsULONG,       // slot number
                                   &vendevID,            // buffer
                                   0x0,                  // config space offset
                                   sizeof(vendevID));
    if (status == 0 || status == 2)
        return 0;

    // return these to the caller
    if (pVendorId)
        *pVendorId = (U016)(vendevID & 0xFFFF);
    if (pDeviceId)
        *pDeviceId = (U016)(vendevID >> 16);

    // return a non-zero handle out of bus/device/func
    ihandle = 0xFF000000 |
             (func << NT_HANDLE_PCI_FUNC_SHIFT) |
             (device << NT_HANDLE_PCI_DEVICE_SHIFT) | bus;
    return (VOID *)((NV_UINTPTR_T)ihandle);
}

RM_STATUS
osUpdateAGPLimit(PHWINFO pDev)
{
    // In winnt4/win2k, the AGP limit is always known
    return RM_OK;
}

RM_STATUS osBeginWatchCanvas
(
    U032 hwnd
)
{
    return RM_OK;
}

RM_STATUS osEndWatchCanvas
(
    U032 hwnd
)
{
    return RM_OK;
}

RM_STATUS osGetCurrentProcess(
    U032 *pPid
)
{
    // TO DO:  what are the semantics of this??
    *pPid = 0;
    return RM_OK;
}

// Validate the DMA address space name from the app
// A DMA object with no name is treated as System Memory space
// The DMA direction does not matter for system memory space
RM_STATUS osValidateDMAObject(
    char *OsName,
    U032 DmaObjectName,
    NV_ADDRESS_SPACE *AddressSpace
)
{
    U032 i;
    RM_STATUS status;

    if (OsName[0] == '\0')
    {
        *AddressSpace = ADDR_SYSMEM;
        return(RM_OK);
    }

    if (strcmp(OsName, nameSysMem) == 0)
    {
       *AddressSpace = ADDR_SYSMEM;
       return(RM_OK);
    }

    if (strcmp(OsName, nameLVidMem) == 0)
    {
       *AddressSpace = ADDR_FBMEM;
       return(RM_OK);
    }

    if (strcmp(OsName, nameSVidMem) == 0)
    {
       *AddressSpace = ADDR_AGPMEM;
       return(RM_OK);
    }

    *AddressSpace = ADDR_SYSMEM;

    return(RM_ERR_INVALID_OS_NAME);

}

RM_STATUS osValidateVideoSink
(
    char *OsName
)
{
    U032 i;

    for (i = 0; i < 4; i++)
        if (OsName[i] != nameVideoSink[i])
            return (RM_ERR_INVALID_OS_NAME);
    return (RM_OK);
}

RM_STATUS osValidateImageVideo
(
    char *OsName,
    U032 *CanvasID
)
{
    U032 i;

    *CanvasID = 0;
    for (i = 0; i < 3; i++)
        if (*OsName++ != nameImageVideo[i])
            return (RM_ERR_INVALID_OS_NAME);
    while (*OsName >= '0' && *OsName <= '9')
        *CanvasID = *CanvasID * 10 + *OsName++ - '0';
    return (*OsName == ':' ? RM_OK : RM_ERROR);
}

// these must be shared by the miniport and RM

RM_STATUS osNotifyAction
(
    POBJECT   Object,
    U032      Method,
    V032      Data,
    RM_STATUS Status,
    U032      Action
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVGFX: osNotifyAction() stub\n");
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVGFX:   Object =", Object);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVGFX:   Method =", Method);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVGFX:   Data   =", Data);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVGFX:   Status =", Status);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVGFX:   Action =", Action);

    return RM_OK;
}

RM_STATUS osError
(
    POBJECT   Object,
    U032      Method,
    V032      Data,
    RM_STATUS Error
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVGFX: osError()\n");
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVGFX:   Object =", Object);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVGFX:   Method =", Method);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVGFX:   Data   =", Data);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVGFX:   Error  =", Error);

    return (RM_OK);
}

VOID osStateFbEx
(
    PHWINFO pDev
)
{
    U032    filterLevel;

    // check for an override value -- legal values are 0 (off), 1, or 2
    if (osReadRegistryDword(pDev, strDevNodeRM, "FilterOverride", &filterLevel) == RM_OK)
    {
        //if (pDev->Framebuffer.MonitorType != NV_MONITOR_VGA)
        {
            //pDev->Framebuffer.FilterEnable = 2;
            pDev->Framebuffer.FilterEnable = filterLevel;
        }
    }
    else
    {
        // only enable flicker filter for TV w/bit depth > 8BPP
        if (GETMONITORTYPE(pDev, 0) != NV_MONITOR_VGA)
        {
            if (pDev->Dac.HalInfo.Depth > 8)
            {
                pDev->Framebuffer.FilterEnable = 2;
            }
            else
            {
                pDev->Framebuffer.FilterEnable = 0;
            }
        }
        else
        {
            pDev->Framebuffer.FilterEnable = 0;
        }
    }

} // end of osStateFbEx()

RM_STATUS osSetVideoMode
(
    PHWINFO pDev
)
{
    U032 data32;

    if (osReadRegistryDword(pDev, strDevNodeRM, strMemoryOverride, &data32) == RM_OK)
    {
        nvHalFbLoadOverride(pDev, FB_LOAD_OVERRIDE_MEMORY, data32);
    }
    if (osReadRegistryDword(pDev, strDevNodeRM, strRTLOverride, &data32) == RM_OK)
    {
        nvHalFbLoadOverride(pDev, FB_LOAD_OVERRIDE_MEMORY, data32);
    }

    return RM_OK;

} // end of osSetVideoMode()

#ifndef WIN2K_DDK_THAT_CONTAINS_APIS
//#if (_WIN32_WINNT < 0x0500)

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
                            KeSetEvent((PRKEVENT)NotifyEvent->Data, 0, FALSE);
                            break;
                        }

                        case NV01_EVENT_KERNEL_CALLBACK:
                        {
                            MINIPORT_CALLBACK callBackToMiniport = (MINIPORT_CALLBACK)NotifyEvent->Data;

                            // perform a direct callback to the miniport
                            if (callBackToMiniport)
                                callBackToMiniport(pDev->DBmpDeviceExtension);
                            break;
                        }
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

#endif // (_WIN32_WINNT < 0x0500)


//---------------------------------------------------------------------------
//
//  AGP support
//
//---------------------------------------------------------------------------

#if (_WIN32_WINNT >= 0x0500)

// AGP memory allocation tracking
typedef struct _AGP_MEM_INFO
{
    NvU64   aperAddr;
    VOID*   virtAddr;
    VOID*   physContext;
    VOID*   virtContext;
    U032    sysPageCount;
} AGP_MEM_INFO, *PAGP_MEM_INFO;

#if defined(IA64)

//
// All of the routines below are to make osGetAddressInfo to work
// with the 82460GX chipset.  Unlike other chipsets, the physical
// memory mapped on by the CPU are not the AGP aperture addresses.
// Thus a call to MmGetPhysicalAddress cannot be used to determine
// whether an virtual address is mapped to AGP memory.  The following
// routines maintain a simple mapping table to map the system physical
// address to the AGP aperture address to program the adapter with.

#define AGP_MAPPING_INCR	1    // keep it small at first to exercise code

typedef struct _AGP_MAPPING {
    PHYSICAL_ADDRESS sysAddr;
    PHYSICAL_ADDRESS aperAddr;
} AGP_MAPPING, *PAGP_MAPPING;

static PAGP_MAPPING osAGPMappingTab;
static U032 osAGPMappingSize = 0;
static U032 osAGPMappingUsed = 0;

static RM_STATUS
osAllocAGPMapping(PHYSICAL_ADDRESS sysAddr, PHYSICAL_ADDRESS aperAddr)
{
    RM_STATUS status;
    PAGP_MAPPING pTab;
    U032 newSize;

    if (osAGPMappingSize == osAGPMappingUsed) {
        newSize = sizeof(AGP_MAPPING) * (osAGPMappingSize + AGP_MAPPING_INCR);
        status = osAllocMem((VOID **)&pTab, newSize);
        if (status != RM_OK)
            return status;
        if (osAGPMappingSize) {
            osMemCopy((unsigned char *)pTab, (unsigned char *)osAGPMappingTab, newSize);
            osFreeMem(osAGPMappingTab);
        }
        osAGPMappingTab = pTab;
        osAGPMappingSize += AGP_MAPPING_INCR;
    }
#ifdef DEBUG
    for (pTab = osAGPMappingTab; pTab < &osAGPMappingTab[osAGPMappingUsed]; pTab++)    {
        RM_ASSERT(pTab->sysAddr.QuadPart != sysAddr.QuadPart);
        RM_ASSERT(pTab->aperAddr.QuadPart != aperAddr.QuadPart);
    }
#endif
    pTab = &osAGPMappingTab[osAGPMappingUsed++];
    pTab->sysAddr = sysAddr;
    pTab->aperAddr = aperAddr;
    return RM_OK;
}

static VOID
osFreeAGPMapping(PHYSICAL_ADDRESS aperAddr)
{
    PAGP_MAPPING pTab;

    for (pTab = osAGPMappingTab; pTab < &osAGPMappingTab[osAGPMappingUsed]; pTab++)    {
        if (pTab->aperAddr.QuadPart == aperAddr.QuadPart) {
            osAGPMappingUsed--;
            osMemCopy((unsigned char *)pTab, (unsigned char *)(pTab+1),
                      (U032)((char *)&osAGPMappingTab[osAGPMappingUsed] -
                             (char *)pTab));
            return;
        }
    }
    
}

static U032
osLookupAGPMapping(PHYSICAL_ADDRESS sysAddr)
{
    PAGP_MAPPING pTab;

    for (pTab = osAGPMappingTab; pTab < &osAGPMappingTab[osAGPMappingUsed]; pTab++)    {
        if (pTab->sysAddr.QuadPart == sysAddr.QuadPart) {
            return (pTab->aperAddr.LowPart);
        }
    }
    return sysAddr.LowPart;
}

#else

#define WIN2K_VIRT_OFFSET_PAGES    16

#define CPUID   __asm _emit 0x0F __asm _emit 0xA2
#define RDMSR   __asm _emit 0x0F __asm _emit 0x32

//
// win2k currently doesn't set up the correct page attribute table (PAT)
// index for write-combined user space mappings to AGP. The user mapping
// gets a WB index into the PAT and because of there are no non-system MTRR's,
// this defaults to uncached mapping.
//
// To work around this, we'll first READ the PAT and determine which index
// should be used for write-combining. Then, we'll read the PTE's for the
// length of the mapping and set the correct index.
//
// I don't believe (hopefully) MP will be an issue as I expect each CPU
// points to the same page table we're about to modify. At worst, this means
// we may be slow if we ran mostly on the wrong (uncached) processor.
//
static RM_STATUS
RmChangePageAttributeTable(PAGP_MEM_INFO pMemInfo)
{
    U032 modPass, PATvalue, PDTbase, WCindex;
    U032 PDEoffset, PTEoffset, physPage, lastPage;
    U032 *basepdb, *basepte, *pdbptr, *pteptr;
    U032 virtOffset, found_first_page = 0;
    PHYSICAL_ADDRESS pagePhysicalAddr;
    
    PATvalue = PDTbase = 0x0;

    __asm {
        ; save state
        push    eax
        push    ebx
        push    ecx
        push    edx

        mov     eax, 1
        CPUID

        // check for PAT support
        shr     edx, 16
        and     edx, 1  
        jz      Exit

        // get the (low 0:3 PAT) indices
        mov     ecx, 0x277
        RDMSR
        mov     PATvalue,eax

        // get the page descriptor base
        mov     ecx, cr3
        mov     PDTbase,ecx

        // flush caches making things consistent
        wbinvd
Exit:
        ; restore state
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }

    if (!PATvalue || !PDTbase)
        return RM_ERROR;

    // look for the WC type within the PAT indices
    for (WCindex = 0; WCindex < 4; WCindex++)
    {
        if ((PATvalue & 0xF) == 0x1)
            break;
        PATvalue >>= 8;
    }
    if (WCindex == 4)
        return RM_ERROR;    // didn't find a WC index

    // Map in the PDE page (assuming 4K ptes)
    pagePhysicalAddr.HighPart = 0;
    pagePhysicalAddr.LowPart  = (PDTbase & ~0xFFF);
    basepdb = MmMapIoSpace(pagePhysicalAddr, RM_PAGE_SIZE, FALSE);
    if (basepdb == NULL)
        return RM_ERROR;

    //
    // First, we need to find if there's a virtOffset that needs to be setup.
    // In win2k, it seems user mappings, in addition to not being write-combined,
    // don't start at the address returned from AgpCommitVirtual after the DX
    // AGP heap has been setup.
    //
    // This might be caused by a non-zero fpGARTLin from the GUID_UpdateNonLocalHeap
    // callback DDraw gets, where the driver is supposed to offset everything by this
    // amount. But, each user alloc seems to not be just fpGARTLin bytes, it's almost
    // fpGARTlin * number of outstanding user allocs.
    // 
    // Rather than guess, for now, just scan WIN2K_VIRT_OFFSET_PAGES pages for the
    // first page and then verify the rest of the mapping from there. This virtOffset
    // will then be added to the user mapping passed back.
    //
    physPage = (U032)pMemInfo->aperAddr;

    virtOffset = 0;
    while (!found_first_page && (virtOffset < WIN2K_VIRT_OFFSET_PAGES))
    {
        // 10bits of PDE offset, 10bits of PTE offset
        PDEoffset = (U032)((NV_UINTPTR_T)pMemInfo->virtAddr >> (RM_PAGE_SHIFT + 10));
        PTEoffset = (U032)(((NV_UINTPTR_T)pMemInfo->virtAddr >> RM_PAGE_SHIFT) & 0x003FF);

        pdbptr = basepdb + PDEoffset;
        if (*pdbptr & 0x80)
        {
            MmUnmapIoSpace(basepdb, RM_PAGE_SIZE);
            return RM_ERROR;    // this isn't a 4K PDE
        }

        // Map in a set of PTEs (assuming 4K ptes)
        pagePhysicalAddr.HighPart = 0;
        pagePhysicalAddr.LowPart  = (*pdbptr & ~0xFFF);
        basepte = MmMapIoSpace(pagePhysicalAddr, RM_PAGE_SIZE, FALSE);
        if (basepte == NULL)
        {
            MmUnmapIoSpace(basepdb, RM_PAGE_SIZE);
            return RM_ERROR;
        }

        pteptr = basepte + PTEoffset;

        // Look for the beginning of the mapping on this PTE page
        while ((PTEoffset <= 0x3FF) && (virtOffset < WIN2K_VIRT_OFFSET_PAGES))
        {
            if ((*pteptr & ~0xFFF) == physPage)
            {
                found_first_page = 1;
                break;
            }
            PTEoffset++; pteptr++; virtOffset++;
            (U008 *)pMemInfo->virtAddr += RM_PAGE_SIZE;
        }

        MmUnmapIoSpace(basepte, RM_PAGE_SIZE);
    }

    if (!found_first_page)
    {
        MmUnmapIoSpace(basepdb, RM_PAGE_SIZE);
        return RM_ERROR;    // can't find first page of our mapping
    }

    lastPage = (U032)pMemInfo->aperAddr + (pMemInfo->sysPageCount << RM_PAGE_SHIFT);

    //
    // Before updating any PTEs, make sure they have the expected physPage.
    // The second pass is the modifying pass where we update the PAT index.
    //
    for (modPass = 0; modPass < 2; modPass++)
    {
        // sets the beginning and ending phys page
        physPage = (U032)pMemInfo->aperAddr;

        // 10bits of PDE offset, 10bits of PTE offset
        PDEoffset = (U032)((NV_UINTPTR_T)pMemInfo->virtAddr >> (RM_PAGE_SHIFT + 10));
        PTEoffset = (U032)(((NV_UINTPTR_T)pMemInfo->virtAddr >> RM_PAGE_SHIFT) & 0x003FF);

        pdbptr = basepdb + PDEoffset;

        while (physPage < lastPage)
        {
            if (*pdbptr & 0x80)
            {
                MmUnmapIoSpace(basepdb, RM_PAGE_SIZE);
                return RM_ERROR;    // this isn't a 4K PDE
            }

            // Map in a set of PTEs (assuming 4K ptes)
            pagePhysicalAddr.HighPart = 0;
            pagePhysicalAddr.LowPart  = (*pdbptr & ~0xFFF);
            basepte = MmMapIoSpace(pagePhysicalAddr, RM_PAGE_SIZE, FALSE);
            if (basepte == NULL)
            {
                MmUnmapIoSpace(basepdb, RM_PAGE_SIZE);
                return RM_ERROR;
            }

            pteptr = basepte + PTEoffset;

            // Check (or modify) the PTEs on this page
            while ((PTEoffset <= 0x3FF) && (physPage < lastPage))
            {
                if ((*pteptr & ~0xFFF) != physPage)
                {
                    // mismatch on physical page in PTE
                    DBG_PRINT_STRING_PTR(DEBUGLEVEL_ERRORS, "NVRM: mismatch in AGP mapping ",
                                         pMemInfo->virtAddr);
                    MmUnmapIoSpace(basepte, RM_PAGE_SIZE);
                    MmUnmapIoSpace(basepdb, RM_PAGE_SIZE);
                    return RM_ERROR;
                }
                if (modPass)
                {
                    // mask off bits 4:3 and set the index
                    *pteptr = (*pteptr & ~0x18) | (WCindex << 3);
                }
                PTEoffset++; pteptr++;
                physPage += RM_PAGE_SIZE;
            }

            // Free the old page of PTEs
            MmUnmapIoSpace(basepte, RM_PAGE_SIZE);

            PTEoffset = 0x0;
            pdbptr++;
        }
    }

    MmUnmapIoSpace(basepdb, RM_PAGE_SIZE);

    return RM_OK;
}
#endif // !IA64

#include <videoagp.h>

RM_STATUS osAllocAGPPages
(
    PHWINFO pDev,
    VOID **pAddress,
    U032   pageCount,
    U032   ClientClass,
    VOID **pMemData
)
{
    PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;
    PWINNTHWINFO pOsHwInfo = (PWINNTHWINFO) pDev->pOsHwInfo;
    PAGP_MEM_INFO pMemInfo;
    HANDLE procHandle;
    RM_STATUS status;
    PHYSICAL_ADDRESS aperAddr;
#if defined(IA64)
    PHYSICAL_ADDRESS sysAddr;
#endif

    PVOID hwDeviceExtension = pDev->DBmpDeviceExtension;

    if (!hwDeviceExtension || !pOsHwInfo->AgpServices)
        return RM_ERR_NO_FREE_MEM;

    // we'll need to manage this allocation, a little
    status = osAllocMem((VOID **)&pMemInfo, sizeof(AGP_MEM_INFO));
    if (status != RM_OK)
        return RM_ERR_NO_FREE_MEM;

    // not all systems have PAGE_SIZE == RM_PAGE_SIZE;
    // round up to get an adequate number of system pages

    pMemInfo->sysPageCount = (pageCount + (PAGE_SIZE/RM_PAGE_SIZE) - 1) >> (PAGE_SHIFT - RM_PAGE_SHIFT);

    // reserve physical
    aperAddr = ((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpReservePhysical(hwDeviceExtension, pMemInfo->sysPageCount, TRUE, &pMemInfo->physContext);

    if ((aperAddr.LowPart == 0) && (aperAddr.HighPart == 0)) {
        osFreeMem(pMemInfo);
        return RM_ERR_NO_FREE_MEM;
    }

    pMemInfo->aperAddr = aperAddr.QuadPart;

    // commit physical
    if ( ((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpCommitPhysical(hwDeviceExtension,
																			   pMemInfo->physContext,
																			   pMemInfo->sysPageCount,
																			   0)
																			   == FALSE )
	{

		((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpReleasePhysical(hwDeviceExtension, pMemInfo->physContext);
        osFreeMem(pMemInfo);
        return RM_ERR_NO_FREE_MEM;
    }

    // set processs handle (hopefully triggers the correct mapping)
    procHandle = (ClientClass == NV01_ROOT_USER) ? NtCurrentProcess() : 0x0;

    // reserve virtual
	pMemInfo->virtAddr = ((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpReserveVirtual(hwDeviceExtension,
																							   procHandle,
																							   pMemInfo->physContext,
																							   &pMemInfo->virtContext);
    if (pMemInfo->virtAddr == NULL)
	{
		((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpFreePhysical(hwDeviceExtension, pMemInfo->physContext, pMemInfo->sysPageCount, 0);
		((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpReleasePhysical(hwDeviceExtension, pMemInfo->physContext);

        osFreeMem(pMemInfo);
        return RM_ERR_NO_FREE_MEM;
    }

    // commit virtual
    if ( ((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpCommitVirtual(hwDeviceExtension,
																			  pMemInfo->virtContext,
																			  pMemInfo->sysPageCount,
																			  0)
																			  == NULL)
	{
		((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpReleaseVirtual(hwDeviceExtension, pMemInfo->virtContext);
		((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpFreePhysical(hwDeviceExtension, pMemInfo->physContext, pMemInfo->sysPageCount, 0);
		((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpReleasePhysical(hwDeviceExtension, pMemInfo->physContext);

        osFreeMem(pMemInfo);
        return RM_ERR_NO_FREE_MEM;
    }

#if defined(IA64)
    sysAddr = MmGetPhysicalAddress(pMemInfo->virtAddr);
    if (osAllocAGPMapping(sysAddr, aperAddr) != RM_OK) {
        osFreeAGPPages(pDev, pAddress, pMemInfo); 
        return RM_ERR_NO_FREE_MEM;
    }
#else
    // apply a win2k kernel workaround for write-combined user mappings 
    if (ClientClass == NV01_ROOT_USER)
    {
        // Dios Mio, forgive me for what I'm about to do ...
        if (RmChangePageAttributeTable(pMemInfo) != RM_OK)
        {
            osFreeAGPPages(pDev, pAddress, pMemInfo); 
            return RM_ERR_NO_FREE_MEM;
        }
    }
#endif

    // verify the buffer is writable (may not be due to a chipset driver bug).
    // This has been seen on a Via 694x when returning from hibernation.
    *(volatile U032 *)pMemInfo->virtAddr = 0xBEEFCAFE;
    if (*(volatile U032 *)pMemInfo->virtAddr != 0xBEEFCAFE)
    {
        DBG_PRINT_STRING_PTR(DEBUGLEVEL_ERRORS,
                            "NVRM: allocated unwritable AGP buffer ", pMemInfo->virtAddr);
        osFreeAGPPages(pDev, pAddress, pMemInfo); 
        return RM_ERR_NO_FREE_MEM;
    }

    // Set our AGP parameters, if they need to be.
    // We don't expect this to ever be needed, since RmSetAgpServices should've
    // handled it already.
    if (!pRmInfo->AGP.AGPLinearStart) {
        pRmInfo->AGP.AGPLinearStart = (VOID_PTR)pMemInfo->virtAddr;

        if (!pRmInfo->AGP.AGPPhysStart && !pRmInfo->AGP.AGPLimit) {
            // should've already been filled in by NvGetAGPBaseLimit()
            pRmInfo->AGP.AGPPhysStart   = aperAddr.LowPart;
            pRmInfo->AGP.AGPLimit       = 0x4000000;        // 64MB
        }

#ifdef DEBUG
        // first allocation should be at the aperture base
        if (aperAddr.LowPart != pRmInfo->AGP.AGPPhysStart) {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: first AGP alloc != AGP base\n");
            DBG_BREAKPOINT();        
        }
#endif
    }

    *pAddress = pMemInfo->virtAddr;
    *pMemData = (VOID *)pMemInfo;

    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: setup AGP addr    ", *pAddress);

    return RM_OK;
}

RM_STATUS osFreeAGPPages
(
    PHWINFO pDev,
    VOID **pAddress,
    VOID  *pMemData
)
{
    PWINNTHWINFO pOsHwInfo = (PWINNTHWINFO) pDev->pOsHwInfo;
    PVOID hwDeviceExtension = pDev->DBmpDeviceExtension;
    PAGP_MEM_INFO pMemInfo = pMemData;
#if defined(IA64)
    PHYSICAL_ADDRESS aperAddr;
#endif

	((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpFreeVirtual(hwDeviceExtension, pMemInfo->virtContext, pMemInfo->sysPageCount, 0);
	((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpReleaseVirtual(hwDeviceExtension, pMemInfo->virtContext);
	((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpFreePhysical(hwDeviceExtension, pMemInfo->physContext, pMemInfo->sysPageCount, 0);
    ((PVIDEO_PORT_AGP_SERVICES)pOsHwInfo->AgpServices)->AgpReleasePhysical(hwDeviceExtension, pMemInfo->physContext);

#if defined(IA64)
    aperAddr.QuadPart = pMemInfo->aperAddr;
    osFreeAGPMapping(aperAddr);
#endif

    osFreeMem(pMemInfo);

    return RM_OK;
}

#else // WIN32_WINNT >= 0x0500

// NT4 versions just call into nvagp functions

#include <nvagp.h>

RM_STATUS osAllocAGPPages
(
    PHWINFO pDev,
    VOID **pAddress,
    U032   pageCount,
    U032   ClientClass,
    VOID **pMemData
)
{
    RM_STATUS rmStatus;

    rmStatus = NvAllocAGPPages(pDev, pAddress, pageCount, ClientClass, pMemData);
    if (rmStatus == RM_OK)
        rmStatus = NvMapAGPPages(pDev, pAddress, ClientClass, *pMemData);

    return rmStatus;
}

RM_STATUS osFreeAGPPages
(
    PHWINFO pDev,
    VOID **pAddress,
    VOID  *pMemData
)
{
    return NvFreeAGPPages(pDev, pAddress, pMemData);
}

#endif // WIN32_WINNT >= 0x0500

// Translate a Virtual to Physical addr (used to provide the AGP aperture offset)
RM_STATUS osVirtualToPhysicalAddr
(
    VOID* virtaddr,
    VOID** physaddr
)
{
    PHYSICAL_ADDRESS physAddress = MmGetPhysicalAddress(virtaddr);

    *physaddr = (VOID*)((NV_UINTPTR_T)(physAddress.LowPart));

    return RM_OK;
}

RM_STATUS osCalculatePteAdjust
(
    PHWINFO pDev,
    U032    AddressSpace,
    VOID*   Address,
    U032*   pPteAdjust
)
{
    *pPteAdjust = (U032)((SIZE_PTR)Address & RM_PAGE_MASK);

    return RM_OK;
}

// get address information from the linear address
RM_STATUS osGetAddressInfo
(
    PHWINFO pDev,
    U032    unusedParamChID,
    U032    unusedParamSelector,
    VOID**  pLinearAddress,
    U032    Limit,
    VOID**  pAddress,
    NV_ADDRESS_SPACE *pAddressSpace
)
{
	PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;

    PHYSICAL_ADDRESS pa;
    U032 physFbAddr;
    U032 physicalAddress;
    U032 ramSize;

    // Check to see if we have an address space conflict
    if ((*pAddressSpace != ADDR_UNKNOWN) && (*pAddressSpace != ADDR_SYSMEM))
    {
       return (RM_ERR_BAD_ADDRESS);
    }

    // Convert virtual address into physical address

    pa = MmGetPhysicalAddress(*pLinearAddress);

#ifdef IA64
    //
    // The virtual address of AGP memory returned to the user is mapped
    // to PCI pages on the 82460GX chipset.  Substitute the AGP aperture
    // address for this physical address in this case.

    physicalAddress = osLookupAGPMapping(pa);
#else
    physicalAddress = pa.LowPart;
#endif

    physFbAddr = pDev->Mapping.PhysFbAddr;
    ramSize = pDev->Framebuffer.HalInfo.RamSize;

    // address space type is frame buffer if the physical address lies within the FB range
    if (physicalAddress >= physFbAddr && physicalAddress < physFbAddr + ramSize)
    {
        if (physicalAddress + Limit >= physFbAddr + ramSize)
        {
            //
            // We're straddling fb mem...bomb.
            //
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: osGetAddressInfo: buffer crosses fbmem boundary ", physicalAddress);
            DBG_BREAKPOINT();
            return (RM_ERR_MULTIPLE_MEMORY_TYPES);
        }

        //
        // Subtract off the start of the framebuffer
        //
        *pLinearAddress = (VOID*)((NV_UINTPTR_T)(physicalAddress - physFbAddr));
        //
        // Turn this into a PNVM dma structure
        //
        *pAddressSpace = ADDR_FBMEM;
    }
    else if (pRmInfo->AGP.AGPLinearStart &&
             physicalAddress >= pRmInfo->AGP.AGPPhysStart &&
             physicalAddress < pRmInfo->AGP.AGPPhysStart + pRmInfo->AGP.AGPLimit)
    {
        if (physicalAddress + Limit > pRmInfo->AGP.AGPPhysStart + pRmInfo->AGP.AGPLimit)
        {
            //
            // We're straddling agp mem...bomb.
            //
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: osGetAddressInfo: buffer crosses agp aperture boundary ", physicalAddress);
            DBG_BREAKPOINT();
            return (RM_ERR_MULTIPLE_MEMORY_TYPES);
        }

        // For AGP, just return the Aperture address as the DescAddr, which'll get
        // loaded into the instance memory PTEs
        *pAddressSpace = ADDR_AGPMEM;
        *pLinearAddress = (VOID *)((NV_UINTPTR_T)physicalAddress);
    }

    // otherwise address space type is system    
    else
    {
        // The address space is always system memory
        *pAddressSpace = ADDR_SYSMEM;
    }

    // the resulting address IS the linear address
    *pAddress = *pLinearAddress;

    return (RM_OK);
}

#if !defined(_WIN64)
RM_STATUS osAllocSystemPages
(
    PHWINFO pDev,
    VOID **pAddress,
    U032   pageCount,
    U032   ClientClass,
    VOID **pMemData
)
{
    PSYS_MEM_INFO    pMemInfo;
    RM_STATUS        rmStatus;
    VOID             *pageAddr, *userAddr;

    // Since there will be some cleanup needed on the free side
    // create a struct that hooks into VOID * in CLI_MEMORY_INFO

    rmStatus = osAllocMem((VOID **)&pMemInfo, sizeof(SYS_MEM_INFO));
    if (rmStatus != RM_OK)
        return rmStatus;

    pMemInfo->sysAddr = ExAllocatePoolWithTag(NonPagedPool,
                                              (pageCount + 1) << RM_PAGE_SHIFT,
                                              NV_MEMORY_TAG);

    if (pMemInfo->sysAddr == NULL) {
        osFreeMem(pMemInfo);
        return RM_ERR_NO_FREE_MEM;
    }
    pageAddr = (VOID *)(((NV_UINTPTR_T)pMemInfo->sysAddr + RM_PAGE_MASK) & ~RM_PAGE_MASK);

    if ((pMemInfo->clientClass = ClientClass) == NV01_ROOT_USER) {
        userAddr = osMapUserSpace(pageAddr, &pMemInfo->pMdl,
                                  pageCount << RM_PAGE_SHIFT, MAP_WRITECOMBINE);
        if (userAddr == NULL) {
            ExFreePool(pMemInfo->sysAddr);
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
    PSYS_MEM_INFO    pMemInfo = (PSYS_MEM_INFO)pMemData;

    // unmap the memory from user space, if necessary
    if (pMemInfo->clientClass == NV01_ROOT_USER) {
        osUnmapUserSpace(*pAddress, pMemInfo->pMdl);
    }

    // free the memory using the original pointer
    ExFreePool(pMemInfo->sysAddr);

    // free our data struct that kept track of this mapping
    osFreeMem(pMemInfo);

    return RM_OK;
}
#endif !defined(_WIN64)


#if (_WIN32_WINNT < 0x0500)

// WINNT4 handles MmWriteCombined caching type, but it isn't in the DDK!

MEMORY_CACHING_TYPE modeTab[4] = { MmNonCached, MmCached,
                                   0x2, 0x2 };
#else
MEMORY_CACHING_TYPE modeTab[4] = { MmNonCached, MmCached,
                                   MmWriteCombined, MmWriteCombined };
#endif
/* remap IO memory to kernel space */
VOID *osMapKernelSpace(unsigned int start, unsigned int size_bytes, U008 mode)
{
    if (mode & MAP_USER)
        return NULL;
    else
        return osMapIOSpace(start, size_bytes, NULL, (U008)(mode));
}

VOID osUnmapKernelSpace(void *addr, unsigned int size)
{
   osUnmapIOSpace(addr, size, NULL, MAP_KERNEL);
}

/* remap memory to user space */
VOID *osMapUserSpace(VOID *kaddr, VOID **priv, unsigned int size_bytes, U008 mode)
{
    PMDL pMdl;
    VOID *addr;

    // create a mdl that describes the kernel mapping
    pMdl = IoAllocateMdl(kaddr, 
                         size_bytes,  // now in bytes
                         FALSE,
                         FALSE,
                         NULL);
    if (pMdl == NULL)
	{
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVGFX: Cannot allocate MDL\n");
        return NULL;
    }
  
  
    // fill in the physical page array of the MDL
    MmBuildMdlForNonPagedPool(pMdl);

    // create the user mapping to these pages
#if (_WIN32_WINNT < 0x0500)
    addr = MmMapLockedPages(pMdl, UserMode);
#else // Win2k doesn't want us using MmMapLockedPages, use MmMapLockedPagesSpecifyCache instead
    addr = MmMapLockedPagesSpecifyCache(pMdl, UserMode, modeTab[mode & 0x3],
                                        NULL, FALSE, HighPagePriority);
#endif
    if (addr == NULL)
    {
       IoFreeMdl(pMdl);
       pMdl = NULL;
       return NULL;
    }

	*priv = (VOID *) pMdl;
    return addr;
}

VOID osUnmapUserSpace(VOID *uaddr, VOID *priv)
{
    PMDL pMdl = (PMDL) priv;

    MmUnmapLockedPages(uaddr, pMdl);
    IoFreeMdl(pMdl);
	priv = (VOID *) NULL;
}

/* map I/O space to user or kernel space */
VOID *osMapIOSpace(U032 start, U032 size_bytes, VOID **priv, U008 mode)
{
    PHYSICAL_ADDRESS physaddr;
    PMDL pMdl;
    VOID *kaddr, *uaddr;

    physaddr.HighPart = 0;
    physaddr.LowPart  = start;

    kaddr = MmMapIoSpace(physaddr, size_bytes, modeTab[mode & 0x3]);

    if (kaddr == NULL)
        return NULL;

    if (mode & MAP_USER)
    {
        uaddr = osMapUserSpace(kaddr, priv, size_bytes, mode);

        if (uaddr == NULL)
        {
            MmUnmapIoSpace(kaddr, size_bytes);
            return NULL;
        }
        return uaddr;
    }
    else
    {
        return kaddr;
    }
}

VOID osUnmapIOSpace(VOID *addr, U032 size_bytes, void *priv, U008 mode)
{
    PMDL pMdl;
    VOID *kaddr;

    if (mode & MAP_USER)
    {
        pMdl = (PMDL) priv;
        kaddr = (char *)pMdl->StartVa + pMdl->ByteOffset;
        osUnmapUserSpace(addr, priv);
    }
    else
    {
        kaddr = addr;
    }
    MmUnmapIoSpace(kaddr, size_bytes);

}

RM_STATUS osSetMemRange(U032 start, U032 size_bytes, U008 mode)
{
    PHYSICAL_ADDRESS physaddr;
    VOID *kaddr;

    if (mode != MEM_WRITECOMBINE)
        return RM_ERROR;

    // Rely on side effect of this routine for now

    physaddr.HighPart = 0;
    physaddr.LowPart  = start;

    kaddr = MmMapIoSpace(physaddr, size_bytes, 0x2);

    if (kaddr == NULL)
        return RM_ERROR;

    MmUnmapIoSpace(kaddr, size_bytes);

    return RM_OK;
}

VOID *osAllocContigPages(U032 size)
{
    PHYSICAL_ADDRESS physaddr;
    physaddr.HighPart = 0;
    physaddr.LowPart  = -1;
    return (VOID *) MmAllocateContiguousMemory(size, physaddr);
}

VOID osFreeContigPages(VOID *addr)
{
    MmFreeContiguousMemory(addr);
}


U032 osGetKernPhysAddr(VOID *kern_virt_addr)
{
    PHYSICAL_ADDRESS physaddr;
    physaddr = MmGetPhysicalAddress(kern_virt_addr);
    return (physaddr.LowPart);
}

RM_STATUS osGetAGPPhysAddr(PHWINFO pDev, VOID *base, U032 index, U032 *paddr)
{
    VOID *vaddr = (VOID *) ((U008 *) base + (index << RM_PAGE_SHIFT));
    *paddr = osGetKernPhysAddr(vaddr);
    if (*paddr == (U032) 0x0) return RM_ERROR;
    return RM_OK;
}

RM_STATUS osAllocPool(VOID **addr, unsigned int size_bytes)
{
    *addr = ExAllocatePoolWithTag(NonPagedPool, size_bytes, NV_MEMORY_TAG);
    if (*addr == NULL) return RM_ERROR;
    return RM_OK;
}

RM_STATUS osFreePool(VOID *addr)
{
    ExFreePool(addr);
    return RM_OK;
}

//---------------------------------------------------------------------------
//
//  Debugging support.
//
//---------------------------------------------------------------------------

#if DBG

// The current debug display level 
//
// Best to leave it above setup and trace messages, but below warnings and error messages
//
int cur_debuglevel = DBG_LEVEL_DEFAULTS;    // all modules set to warning level

VOID osDbgBreakPoint
(
    void
)
{
    DbgBreakPoint();
}

VOID osDbgPrintf_va(
    int     module,
    int     debuglevel,
    const char *printf_format,
    va_list arglist)
{
    int     moduleValue = cur_debuglevel;
    char    nv_error_string[MAX_ERROR_STRING];
    char    *p = nv_error_string;
    int     chars_written;

    // get the module
    // put the module we want in the first 2 bits
    moduleValue = moduleValue >> (module * 2);

    // get the value of those two bits
    // mask off the last 30 bits
    moduleValue = moduleValue & 0x00000003;

    if (debuglevel >= moduleValue)
    {
        chars_written = nvDbgSprintf(p, printf_format, arglist);

        DbgPrint(p);
   }
}

#endif // DBG

#if (_WIN32_WINNT < 0x500) && defined(NTRM)
//
// Signals the nVidia service (NVSVC) via the "NVModeSwitchEvent" that a device switch hotkey has been pressed.
//
VOID RmSignalHotKeyEvent()
{
    SignalModeSwitchEvent();
}
#endif

// end of os.c
