//******************************************************************************
//
// Copyright (c) 1992  Microsoft Corporation
//
// Module Name:
//
//     NVSYSMEM.C
//
// Abstract:
//
//     This is a few utility routines that map memory.
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997,1998  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

#include <ntddk.h>
#include <windef.h>
// jsw...
//#undef WIN32

#include "nvos.h"
#include "dspioctl.h"

// polymorphic type stubs for NT4
#if (_WIN32_WINNT < 0x0500)
#define SIZE_T ULONG
#define ULONG_PTR ULONG
#endif // _WIN32_WINNT < 0x0500

// TO DO: temporary constant until a better place is found (from nvgfx.h)
#define ADDR_SPACE_IO           0x00
#define ADDR_SPACE_MEM          0x01
#define MEMORY_MAPPED_IO        0x00

// Taken from video.h
//
// Debugging statements. This will remove all the debug information from the
// "free" version.
//

#if DBG
#define VideoDebugPrint(arg) VideoPortDebugPrint arg
#else
#define VideoDebugPrint(arg)
#endif

VOID
VideoPortDebugPrint(
    ULONG DebugPrintLevel,
    PCHAR DebugMessage,
    ...
    );

static VOID *mapToUserSpace(int addrSpace, PHYSICAL_ADDRESS addr, int bytes);

//******************************************************************************
//
// Function: NVLockdownMemory()
//
// Routine Description:
//
//     Create MDL and lock down the pages for a user supplied memory address.
//
//     Support function for IOCTL_VIDEO_LOCK_DOWN_MEMORY.
//
// Arguments:
//
//     pLockdownMemory - Pointer to description of user memory to be locked down.
//
// Return Value:
//
//     None.
//
//******************************************************************************

void
NvLockdownMemory(
NV_LOCK_DOWN_MEMORY *pLockdownMemory)
{
    MDL *pMdl;
    PHYSICAL_ADDRESS pa;
    NTSTATUS status;
    PVOID systemAddress;
    ULONG offset;

    pLockdownMemory->pMdl = (PVOID)NULL;

    pMdl = MmCreateMdl(NULL,
                       PAGE_ALIGN(pLockdownMemory->virtualAddress),
                       pLockdownMemory->byteLength);

    if (pMdl)
    {
        try 
        {
            if ( (pMdl->MdlFlags & MDL_PAGES_LOCKED)==0)   
                MmProbeAndLockPages(pMdl, IoModifyAccess);
        }
        except(EXCEPTION_EXECUTE_HANDLER)
        {
            status = GetExceptionCode();
            VideoDebugPrint((0, "Exception status = 0x%lx\n", (ULONG)status));
            ExFreePool (pMdl);
            pMdl = NULL;
        }

        pLockdownMemory->pMdl = (PVOID)pMdl;

        VideoDebugPrint((0, "pMdl = 0x%p\n", pMdl));
    }
}

//******************************************************************************
//
// Function: NVUnlockMemory()
//
// Routine Description:
//
//     Unlock memory and free associated MDL.
//
//     Support function for IOCTL_VIDEO_UNLOCK_MEMORY.
//
// Arguments:
//
//     pUnlockMemory - Pointer to description of user memory to be unlocked.
//
// Return Value:
//
//     None.
//
//******************************************************************************

void
NvUnlockMemory(
NV_UNLOCK_MEMORY *pUnlockMemory)
{
    NTSTATUS status;
    MDL *pMdl;

    pMdl = (MDL *)pUnlockMemory->pMdl;
    if (pMdl)
        {
        try 
            {
                MmUnlockPages(pMdl);
                ExFreePool(pMdl);
            }
            except(EXCEPTION_EXECUTE_HANDLER)
            {
                status = GetExceptionCode();
                VideoDebugPrint((0, "Exception status = 0x%lx\n", (ULONG)status));
            }
        }
}

//******************************************************************************
//
// Function: NVSystemMemoryShare()
//
// Routine Description:
//
//     Share a user process memory address with the display driver.  Lock down the
//     memory and then get a system address for it.  Fill in system memory structure
//     with new system memory address for later use by display driver.
//
//     Support function for IOCTL_VIDEO_SYSMEM_SHARE_MEMORY.
//
// Arguments:
//
//     pSysmemShareMemory - Pointer to description of user memory.
//
// Return Value:
//
//     None.
//
//******************************************************************************

void
NvSystemMemoryShare(
NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemory)
{
    MDL *pSystemMdl;
    PHYSICAL_ADDRESS pa;
    NTSTATUS status;
    PVOID systemAddress;
    ULONG_PTR offset;

    pSysmemShareMemory->pSystemMdl = (PVOID)NULL;
    pSysmemShareMemory->physicalAddress = 0;

    pSystemMdl = MmCreateMdl(NULL,
                             PAGE_ALIGN(pSysmemShareMemory->userVirtualAddress),
                             pSysmemShareMemory->byteLength);

    if (pSystemMdl)
    {
        try 
        {
            if ( (pSystemMdl->MdlFlags & MDL_PAGES_LOCKED)==0)   
                MmProbeAndLockPages(pSystemMdl, IoModifyAccess);
        }
        except(EXCEPTION_EXECUTE_HANDLER)
        {
            status = GetExceptionCode();
            VideoDebugPrint((0, "Exception status = 0x%lx\n", (ULONG)status));
        }

        pSysmemShareMemory->pSystemMdl = (PVOID)pSystemMdl;
        pa.QuadPart = MmGetPhysicalAddress((PVOID)(PAGE_ALIGN(pSysmemShareMemory->userVirtualAddress)));

        offset = (ULONG_PTR)pSysmemShareMemory->userVirtualAddress & (PAGE_SIZE - 1);

        systemAddress = MmGetSystemAddressForMdlSafe(pSystemMdl, NormalPagePriority);

        pSysmemShareMemory->ddVirtualAddress = (PUCHAR)systemAddress + offset;
        pSysmemShareMemory->physicalAddress = pa.LowPart;

        VideoDebugPrint((0, "pSystemMdl = 0x%p\n", pSystemMdl));
        VideoDebugPrint((0, "physicalAddress = 0x%p\n", pSysmemShareMemory->physicalAddress));
    }
}


//******************************************************************************
//
// Function: NVSystemMemoryUnShare()
//
// Routine Description:
//
//     Unmap a previous shared address.  Unlock the pages.
//
//     Support function for IOCTL_VIDEO_SYSMEM_UNSHARE_MEMORY.
//
// Arguments:
//
//     pSysmemShareMemory - Pointer to description of user memory.
//
// Return Value:
//
//     None.
//
//******************************************************************************

void
NvSystemMemoryUnShare(
NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemory)
{
    if (pSysmemShareMemory && pSysmemShareMemory->pSystemMdl)
    {
        MDL *pSystemMdl = (MDL *)pSysmemShareMemory->pSystemMdl;

        VideoDebugPrint((0, "pSystemMdl = 0x%p\n", pSystemMdl));
        VideoDebugPrint((0, "physicalAddress = 0x%p\n", pSysmemShareMemory->physicalAddress));

        MmUnlockPages(pSystemMdl);
        ExFreePool(pSystemMdl);

        pSysmemShareMemory->pSystemMdl = (PVOID)NULL;
    }
}

//******************************************************************************
//
// Function: NVMapIntoUserSpace()
//
// Routine Description:
//
//     Map a give memory address into user address space.  The incoming address
//     should be a kernel mode address.  
//
//     Support function for IOCTL_VIDEO_MAP_TO_USER.
//
// Arguments:
//
//     pSysmemShareMemory - Pointer to description of user memory.
//
// Return Value:
//
//     None.
//
//******************************************************************************

void
NvMapIntoUserSpace(
NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemory)
{
    PHYSICAL_ADDRESS pa;
    MDL *pSystemMdl;
    NTSTATUS status;

    pSystemMdl = pSysmemShareMemory->pSystemMdl;
    if (NULL == pSystemMdl)
    {
        pSysmemShareMemory->physicalAddress = 0;

        pSystemMdl = MmCreateMdl(NULL,
                                 PAGE_ALIGN(pSysmemShareMemory->ddVirtualAddress),
                                 pSysmemShareMemory->byteLength);
    }

    if (pSystemMdl != NULL)
    {
        try 
        {
            if ( (pSystemMdl->MdlFlags & MDL_PAGES_LOCKED)==0)   
                MmProbeAndLockPages(pSystemMdl, IoModifyAccess);
        }
        except(EXCEPTION_EXECUTE_HANDLER)
        {
            status = GetExceptionCode();
            VideoDebugPrint((0, "Exception status = 0x%lx\n", (ULONG)status));
        }

        //
        // Map display driver address into the address space of the current thread.
        //
        pa.QuadPart = MmGetPhysicalAddress((PVOID)(PAGE_ALIGN(pSysmemShareMemory->ddVirtualAddress)));
        pSysmemShareMemory->userVirtualAddress = mapToUserSpace(ADDR_SPACE_MEM, pa, pSysmemShareMemory->byteLength);
        pSysmemShareMemory->physicalAddress = pa.LowPart;

        pSysmemShareMemory->pSystemMdl = (PVOID)pSystemMdl;
    }
}

//******************************************************************************
//
// Function: NVUnmapFromUserSpace()
//
// Routine Description:
//
//     Unmap a previously mapped user address.
//
//     Support function for IOCTL_VIDEO_UNMAP_FROM_USER.
//
// Arguments:
//
//     pSysmemShareMemory - Pointer to description of user memory.
//
// Return Value:
//
//     None.
//
//******************************************************************************

void
NvUnmapFromUserSpace(
NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemory)
{
    NTSTATUS status;

    if (pSysmemShareMemory)
    {
        MDL *pSystemMdl = (MDL *)pSysmemShareMemory->pSystemMdl;

        VideoDebugPrint((0, "unmap pSystemMdl = 0x%p\n", pSystemMdl));
        VideoDebugPrint((0, "unmap va = 0x%p\n", pSysmemShareMemory->userVirtualAddress));

        try 
        {
            ZwUnmapViewOfSection((HANDLE)-1,          // current process handle
                                  (PVOID)pSysmemShareMemory->userVirtualAddress);
            if (pSystemMdl)
            {
                MmUnlockPages(pSystemMdl);
                ExFreePool(pSystemMdl);
            }
        }
        except(EXCEPTION_EXECUTE_HANDLER)
        {
            status = GetExceptionCode();
            VideoDebugPrint((0, "Exception status = 0x%lx\n", (ULONG)status));
        }

        pSysmemShareMemory->pSystemMdl = (PVOID)NULL;
    }
}

//******************************************************************************
//
// Function: NVMapPhysIntoUserSpace()
//
// Routine Description:
//
//     Map a physical address and length into a user address space.
//
//     Support function for IOCTL_VIDEO_MAP_TO_USER.
//
// Arguments:
//
//     pa - physical address
//     byteLength - bytes in mapping
//     lowPart - 32-bit return address
//
// Return Value:
//
//     None.
//
//******************************************************************************

void
NVMapPhysIntoUserSpace(
PHYSICAL_ADDRESS pa,
LONG byteLength,
PVOID *userVirtualAddress)
{
    //
    // Map physical address into the address space of the current thread.
    //
    *userVirtualAddress = mapToUserSpace(ADDR_SPACE_MEM, pa, byteLength);
}

//******************************************************************************
//
// Function: mapToUserSpace()
//
// Routine Description:
//
//     Map a give memory address into user address space.  The incoming address
//     should be a kernel mode address.
//
//     Support function for IOCTL_VIDEO_MAP_TO_USER.
//
// Arguments:
//
//     pSysmemShareMemory - Pointer to description of user memory.
//
// Return Value:
//
//     None.
//
//******************************************************************************

static VOID *
mapToUserSpace(
    int addrSpace, 
    PHYSICAL_ADDRESS addr, 
    int bytes)
{
    OBJECT_STRING      physicalMemoryUnicodeString;
    OBJECT_ATTRIBUTES  objectAttributes;
    HANDLE             physicalMemoryHandle  = NULL;
    void              *physicalMemorySection = NULL;
    void              *virtualAddress;
    PHYSICAL_ADDRESS   viewBase; 
    SIZE_T             viewSize;
    ULONG              length;
    NTSTATUS ntStatus;

    RtlInitObjectString(&physicalMemoryUnicodeString,
        OTEXT("\\Device\\PhysicalMemory"));
    InitializeObjectAttributes(&objectAttributes, 
        &physicalMemoryUnicodeString, 
        OBJ_CASE_INSENSITIVE, 
        (HANDLE) NULL, 
        (PSECURITY_DESCRIPTOR) NULL); 
 
    ntStatus = ZwOpenSection(&physicalMemoryHandle, 
        SECTION_ALL_ACCESS, &objectAttributes); 
 
    if (!NT_SUCCESS(ntStatus)) { 
        VideoDebugPrint((0, "NV: mapToUser\n"));
        return NULL;
    }

    ntStatus = ObReferenceObjectByHandle(physicalMemoryHandle, 
        (POBJECT_TYPE) NULL, &physicalMemorySection); 

    if (!NT_SUCCESS(ntStatus)) { 
        VideoDebugPrint((0, "NV: ObjREferenceObjectByHandle\n"));
        ZwClose(physicalMemoryHandle); 
        return NULL;
    } 

    length = bytes;
    // 
    // initialize view base that will receive the physical mapped 
    // address after the MapViewOfSection call. 
    // 
    viewBase = addr; 
    // 
    // Let ZwMapViewOfSection pick an address 
    // 
    virtualAddress = NULL;
    // 
    // Map the section 
    // 
    viewSize = length;
    ntStatus = ZwMapViewOfSection(
        physicalMemoryHandle, // section handle
        (HANDLE) -1,          // current process handle
        &virtualAddress,      // base address
        0L,                   // zero bits
        length,               // commit size
        &viewBase,            // section offset
        &viewSize,            // view size
        ViewUnmap,            // unmap for children processes
        0,                    // allocation type
        ((addrSpace == ADDR_SPACE_IO) ? (PAGE_READWRITE | PAGE_NOCACHE)
         : PAGE_READWRITE));  // page protection
 
    if (!NT_SUCCESS(ntStatus)) {
        VideoDebugPrint((0, "NV: ZwMapViewOfSection\n"));
        ZwClose(physicalMemoryHandle); 
        return NULL;
    }

    // 
    // Mapping the section above rounded the physical address down to the 
    // nearest 64 K boundary. Now return a virtual address that sits where 
    // we wnat by adding in the offset from the beginning of the section. 
    // 
    (PUCHAR)virtualAddress += addr.LowPart - viewBase.LowPart; 

    ZwClose(physicalMemoryHandle); 

    return virtualAddress;
} // end of mapToUser() 

