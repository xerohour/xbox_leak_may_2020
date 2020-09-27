/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    pool.c

Abstract:
    
    Implementation of transfer descriptor pool manager.


    OHCI implements a multi-plexed DMA scheme.  Rather than having a number of fixed registers for
    programming DMA.  The host controller and the host controller driver share a common data structure.
    This structure must be locked down in physical memory at all times.

    The structure is a linked list of queues.  The head of each queue is referred to as an Endpoint Descriptor
    (Not to be confused to the endpoint descriptor in the USB specification, which is a different entity
    entirely.)  Each endpoint descriptor points to the head and tail of singly linked list of Transfer Descriptors.
    Each TD describes the user buffer for transfering a single packet of data across the USB wire.  The host controller
    walks through this schedule and pulls of transfer descriptors one at a time.  It performs the transfer (or records
    an error).  Then it places the TD on a DoneHead and moves on to the next one.

    Each of these TDs is 16 bytes long as defined by hardware.  However, the software needs additional fields so
    each block is expanded to 32 bytes.
    
    Isochronous Endpoints to not use a TD pool, but instead come as part of the Isochronous part of the URB extension.

    The code in this module allocates two pages of memory.  The first part of the first page is returned for use as the HCCA area.
    The rest is broken into 32 byte blocks.

    01-12-01  Originally, this code allocate memory for the TD's and the HCCA area.  There was a separate
    TD pool for each host controller.  Now there will be one TD pool for both host controllers, plus endpoints
    will also come out of here.  We can do this, because we are moving to an upfront iniitialization of everything.
    We also allocate isochronous endpoints from here.

Environment:

    Designed for XBOX.

Notes:

Revision History:

    01-17-00 created by Mitchell Dernis (mitchd)
    01-12-01 changed name to pool.c (mitchd)

--*/

//
//  Pull in OS headers
//
#include <ntos.h>

//
//  Setup the debug information for this file (see ..\inc\debug.h)
//
#define MODULE_POOL_TAG          'DCHO'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("OHCD");

//
//  Pull in usb headers
//

#include <hcdi.h>       //Interface between USBD and HCD
#include "ohcd.h"       //Private OHCD stuff

OHCD_POOL OHCD_GlobalPool;

#pragma code_seg(".XPPCINIT")


//------------------------------------------
//  Initialize Pool
//------------------------------------------
VOID
FASTCALL
OHCD_fPoolInit(
    IN PHCD_RESOURCE_REQUIREMENTS pResourceRequirements
    )
{
    ULONG                       poolSize;
    ULONG                       pageCount;
    ULONG_PTR                   poolMemory;
    ULONG_PTR                   poolEnd;
    ULONG                       index;
    ULONG                       isochEndpointSize;
    ULONG                       combinedEndpointSize;
    ULONG                       transferDescriptorsCount;
    POHCD_TRANSFER_DESCRIPTOR   transferDescriptor;

    USB_DBG_ENTRY_PRINT(("Entering OHCD_InitPool."));

    //
    //  Figure out how much memory is needed.
    //
    isochEndpointSize = OHCD_ISOCH_ENDPOINT_SIZE(pResourceRequirements->IsochMaxBuffers);
    combinedEndpointSize = isochEndpointSize+sizeof(OHCD_ENDPOINT);
    ASSERT(0==isochEndpointSize%16);
    ASSERT(0==(combinedEndpointSize)%32);
    ASSERT(0==sizeof(OHCI_HCCA)%32);
    ASSERT(32==sizeof(OHCD_TRANSFER_DESCRIPTOR));

    if(pResourceRequirements->EndpointCount < pResourceRequirements->IsochEndpointCount)
    {
        pResourceRequirements->EndpointCount = pResourceRequirements->IsochEndpointCount;
    }

    poolSize = sizeof(OHCI_HCCA)*HCD_MAX_HOST_CONTROLLERS +
               pResourceRequirements->IsochEndpointCount*isochEndpointSize +
               pResourceRequirements->EndpointCount * sizeof(OHCD_ENDPOINT) +
               pResourceRequirements->TDCount * sizeof(OHCD_TRANSFER_DESCRIPTOR);

    USB_DBG_TRACE_PRINT(("Required Pool Size = %d bytes.", poolSize));
    //
    //  Figure out how many pages are needed.
    //  (divide by PAGE_SIZE, but make sure that it rounds up, not down)
    //
    pageCount = (poolSize + PAGE_SIZE - 1)/PAGE_SIZE;

    //
    //  Alignment issues may require 16 bytes of dead space
    //  at the end of each page. Make sure we have it just in case.
    if(pageCount*PAGE_SIZE < (pageCount*16+poolSize))
    {
        pageCount++;
    }

    USB_DBG_TRACE_PRINT(("Low-level XPP uses %d pages.", pageCount));
    USB_DBG_TRACE_PRINT(("Max EndpointCount = %d.", pResourceRequirements->EndpointCount));
    USB_DBG_TRACE_PRINT(("Max IsochEndpointCount = %d.", pResourceRequirements->IsochEndpointCount));
    
    //
    //  Allocate PageCount pages of memory.
    //
    poolSize = pageCount*PAGE_SIZE;
#ifdef DVTSNOOPBUG
    OHCD_PoolCreateDoubleBufferPool();
    poolMemory = (ULONG_PTR)MmAllocateContiguousMemoryEx(poolSize, 0, MAXULONG_PTR, 0, PAGE_READWRITE|PAGE_NOCACHE);
    __asm wbinvd; //flush cache
#else
    poolMemory = (ULONG_PTR)MmAllocateContiguousMemory(poolSize);
#endif //DVTSNOOPBUG
   
    RIP_ON_NOT_TRUE_WITH_MESSAGE(poolMemory, "XInitDevices failed due to insufficent memory.  There is no graceful recovery.");
    
    //
    //  Lock it all down.
    //
    MmLockUnlockBufferPages((PVOID)poolMemory, poolSize, FALSE);

    //
    //  Record the difference between the virtual and physical addresses.
    //  (use this for quick conversion.)
    OHCD_GlobalPool.VirtualToPhysical = poolMemory - MmGetPhysicalAddress((PVOID)poolMemory);

    //
    //  Initialize page
    //
    RtlZeroMemory((PVOID)poolMemory, poolSize);

    //**
    //**  Carve up the pool into HCCA's, isoch endpoints, endpoints, and TDs.
    //**  
    poolEnd = poolMemory + poolSize;
    
    //
    // HCCA
    //
    for(index=0; index < HCD_MAX_HOST_CONTROLLERS; index++)
    {
        OHCD_GlobalPool.Hcca[index] = (POHCI_HCCA)poolMemory;
        poolMemory += sizeof(OHCI_HCCA);
        ASSERT(poolEnd > poolMemory);
    }

    //
    //  OHCD_ISOCH_ENDPOINT
    //  (allocate an OHCD_ENDPOINT with each OHCD_ISOCH_ENDPOINT
    //   to keep the alignment requirements.)
    //
    
    index = 0;  //we want index initialized, outside the conditionally compiled
                //isochronous code.
    OHCD_GlobalPool.FreeEndpoints = NULL;
#ifdef OHCD_ISOCHRONOUS_SUPPORTED
    OHCD_GlobalPool.IsochMaxBuffers = pResourceRequirements->IsochMaxBuffers;
    OHCD_GlobalPool.IsochFreeEndpoints = 0;
    for(; index < pResourceRequirements->IsochEndpointCount; index++)
    {
        // An isoch endpoint is 48 bytes long +
        // an integer multiple of 64 bytes, yet the hardware requires 32 byte alignment.
        // So these cannot be densly packet.  To achieve better packing we intersperse
        // regular endpoints which are 48 bytes long, but require only 16 byte alignment.
    
            
        // allocate an isoch endpoint            
        *((PULONG_PTR)(poolMemory)) = OHCD_GlobalPool.IsochFreeEndpoints;
        OHCD_GlobalPool.IsochFreeEndpoints = (ULONG_PTR)(poolMemory);
        poolMemory += isochEndpointSize;
        ASSERT(poolEnd > poolMemory);
        // allocate an endpoint
        ((POHCD_ENDPOINT)(poolMemory))->Next = OHCD_GlobalPool.FreeEndpoints;
        OHCD_GlobalPool.FreeEndpoints = (POHCD_ENDPOINT)(poolMemory);
        poolMemory += sizeof(OHCD_ENDPOINT);
        ASSERT(poolEnd > poolMemory);
    }
#endif //OHCD_ISOCHRONOUS_SUPPORTED
    
    //
    // OHCD_ENDPOINT - don't reset index, since it already 
    //                 records how many were allocated with
    //                 OHCD_ISOCH_ENDPOINTs.
    //
    for(; index < pResourceRequirements->EndpointCount; index++)
    {
        // allocate an endpoint
        ((POHCD_ENDPOINT)(poolMemory))->Next = OHCD_GlobalPool.FreeEndpoints;
        OHCD_GlobalPool.FreeEndpoints = (POHCD_ENDPOINT)(poolMemory);
        poolMemory += sizeof(OHCD_ENDPOINT);
        ASSERT(poolEnd > poolMemory);
    }

    //
    //  OHCD_TRANSFER_DESCRIPTOR - we can always use extras, so
    //                             allocate until the end of the pool.
    //
    transferDescriptorsCount = 0;
    OHCD_GlobalPool.FreeTDs = NULL;
    OHCD_GlobalPool.FirstTD = (POHCD_TRANSFER_DESCRIPTOR)poolMemory;
    while((poolMemory + sizeof(OHCD_TRANSFER_DESCRIPTOR)) <= poolEnd)
    {
        transferDescriptor = (POHCD_TRANSFER_DESCRIPTOR)poolMemory;
        transferDescriptor->PhysicalAddress = OHCD_PoolGetPhysicalAddress(transferDescriptor);
        OHCD_PoolFreeTD(transferDescriptor);
        poolMemory += sizeof(OHCD_TRANSFER_DESCRIPTOR);
        transferDescriptorsCount++;
    }
    OHCD_GlobalPool.LastTD = transferDescriptor;
    OHCD_GlobalPool.LostDoneHeadCount = 0;
    OHCD_GlobalPool.LostMinimumDoneListLength = 1000; //init to a large value.
    
    //
    //  If this assert fails we under-allocated.  Hitting this is a pretty serious
    //  bug in the pool initialization code.
    //
    ASSERT(transferDescriptorsCount >= pResourceRequirements->TDCount);

    //
    //  Record the quotas in the pool
    //
    OHCD_GlobalPool.ControlQuota = pResourceRequirements->ControlTDQuota;
    OHCD_GlobalPool.BulkQuota = pResourceRequirements->BulkTDQuota;

    //
    //  At this point, the memory should be carved up.
    //  Figure out how many extra TD's we got out of it
    //  and apply it to the TD quotas.  
    //
    if(transferDescriptorsCount > pResourceRequirements->TDCount)
    {
       UCHAR extraTDs = transferDescriptorsCount - pResourceRequirements->TDCount;
       // If there is a bulk quota, apply half of the extra TD's to it.
       if(OHCD_GlobalPool.BulkQuota)
       {
           OHCD_GlobalPool.BulkQuota += extraTDs/2;
           extraTDs -= extraTDs/2;
       }
       // Apply the remaining extra TD's tp the control TD quota
       OHCD_GlobalPool.ControlQuota += extraTDs;
       pResourceRequirements->TDCount = transferDescriptorsCount;
    }
    
    USB_DBG_TRACE_PRINT(("TDCount = %d.", pResourceRequirements->TDCount));
    USB_DBG_TRACE_PRINT(("ControlTDQuota = %d.", OHCD_GlobalPool.ControlQuota));
    USB_DBG_TRACE_PRINT(("BulkTDQuota = %d.", OHCD_GlobalPool.BulkQuota));
    OHCD_GlobalPool.ControlQuotaRemaining = OHCD_GlobalPool.ControlQuota;
    OHCD_GlobalPool.BulkQuotaRemaining = OHCD_GlobalPool.BulkQuota;

    USB_DBG_EXIT_PRINT(("Exiting OHCD_TDPoolInit."));
}

#ifdef SILVER

ULONG
FASTCALL
OHCD_fPoolFindLostDoneHead(
    POHCD_DEVICE_EXTENSION DeviceExtension
    )
/*++
    Routine Description:
        Compaq's original OpenHCI design (code named Hydra) had a serious bug
    where it would sometimes lose the done head.  This flaw was subsequently
    propogated to a number of chips based off that design.  nVidia claims that
    this bug was in the original MCPX design for xbox, but that it was fixed
    even before the first silicon.  In the meantime, we need to support it on
    the pre-DVT development kits using the Opti Chipset, some of which seem
    to suffer from the flaw.

        The code walks the TD pool and figures out which one is the head
    of our missing done list.  The implementation is taken from the Windows 2000
    code base.  However, this code base has one little catch.  A single global
    pool is used for both host controllers  (The first version of MCPX for XBox
    has two controllers.  In fact, launch titles will only use one, but we need
    to support two for the development kits - even after going to DVT - and in 
    the future we may decide to support the second host controller for shipping
    games - who knows).  The solution is to stamp the TD's with the host controller
    number, and erase the stamp when freeing the TD's (see above).  This is
    equivalent to the InUse flag in the Win2k implementation, except we have
    more than one bit of actual information in it.

    BUGBUG: WARNING!!!: THIS CODE FAILS AT THIS TIME IF THE LOST LIST CONTAINS ANY
    ISOCHRONOUS TDS, AS THESE TDS ARE NOT IN THE POOL!!!  (This was entered as 
    bug #4463.  The bug was resolved as "won't fix", since this code does not execute
    on MCPX, the bug is extremely rare and it is not critical to support isoch on the
    pre-DVT boxes.)

    The algorithm for searching is as follows:

    1) Find the tails of the lists.  There are two lists: HcDoneHead points to the
    head of a private list that the HC is building (the hardware presumably doesn't
    lose this one) and Hcca.DoneHead (the one that is lost).  The tail TD of both of
    these lists look the same: the NextTD member is NULL, and the HostControllerNumber
    is the same as the HostControllerNumber in the DeviceExtension.  We saw to both
    of these conditions in OHCD_PoolFreeTD by initializing these 0xFFFFFFFF and 0xFF,
    and by setting HostControllerNumber in OHCD_PoolAllocateTD.  So NextTD might get
    changed to a valid pointer (but not NULL), when an URB is programmed, but only
    the HostController changes NexTD to NULL.

    2) a snap shot of HcDoneHead is taken.

    3) After finding the tails, the lists are built up by repeatedly walking the pool
    looking for TD's that point to the heads of the two lists.  If one is found it
    is the new head.

    4) Each time a new head is found (including before the first iteration),
    it is compared to the snap shot of HcDoneHead taken in step 2).  If it
    matches HcDoneHead, we can throw away that list and just keep building the
    other list, which must be the lost one.

    5) The first time through the whole pool that we find no changes, we can
    be sure that we got the whole missing list.
    
--*/
{
    UCHAR HostControllerNumber = (UCHAR)DeviceExtension->HostControllerNumber;
    POHCD_TRANSFER_DESCRIPTOR   Td;
    ULONG                       TdList1 = 0;
    ULONG                       TdList1Length = 0;
    ULONG                       TdList2 = 0;
    ULONG                       TdList2Length = 0;
    ULONG                       HcDoneHead = 0;
    BOOLEAN                     updated;

    USB_DBG_WARN_PRINT(("Lost DoneHead %d time(s).", ++OHCD_GlobalPool.LostDoneHeadCount));

    //
    // Scan the TD pool looking for TDs with a NULL NextTD pointer.
    // A TD should only have NULL NextTD pointer if it is the tail of
    // a done TD list.  There might be two such lists:  the list of TDs
    // that were completed the last time the HC should have updated the
    // HCCA->HccaDoneHead, and the list of TDs that have completed since
    // then.
    //

    for (Td = OHCD_GlobalPool.FirstTD; Td <= OHCD_GlobalPool.LastTD; Td++)
    {
        if (Td->HostControllerNumber == HostControllerNumber)
        {
            if (0 == Td->HcTransferDescriptor.NextTD)
            {
                // This TD has a NULL NextTD pointer.  Save it as the
                // tail of either TdList1 or TdList2.
                //
                if (0 == TdList1)
                {
                    TdList1 = Td->PhysicalAddress;
                    TdList1Length++;
                }
                else
                {
                    // We expect to find at most two TDs with NULL
                    // NextTD pointers.
                    //
                    ASSERT(0 == TdList2);

                    TdList2 = Td->PhysicalAddress;
                    TdList2Length++;
                }
            }
        }
    }


    if (0 == TdList1)
    {
        USB_DBG_WARN_PRINT(("Lost DoneHead not found."));
        return 0;
    }

    if (TdList2 != 0)
    {
        // There are two lists of completed TDs.  One list should be
        // pointed to by HCCA->HccaDoneHead, and the other list should be
        // pointed to by HC->HcDoneHead.  Read HC->HcDoneHead so we can
        // determine which list is pointed to (or should have been pointed
        // to) by HCCA->HccaDoneHead and which list is pointed to by
        // HC->HcDoneHead.
        //
        HcDoneHead = READ_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcDoneHead);

        // If HC->HcDoneHead is NULL, then something is does not match our
        // expectations.
        //
        ASSERT(HcDoneHead != 0);
    }

    do
    {
        updated = FALSE;

        if (HcDoneHead)
        {
            if (HcDoneHead == TdList1)
            {
                // TdList1 is pointed to by HC->HcDoneHead.  Toss TdList1
                // and keep TdList2
                //
                TdList1 = TdList2;
                TdList1Length = TdList2Length;
                TdList2 = 0;
            }
            else if (HcDoneHead == TdList2)
            {
                // TdList2 is pointed to by HC->HcDoneHead.  Toss TdList2
                // and keep TdList1
                //
                TdList2 = 0;
            }
        }

        //
        // Scan the TD pool looking for TDs with NextTD pointers that
        // point to the head of either TdList1 or TdList2.  If such a TD
        // is found, it becomes the new head of the appropriate list, and
        // loop around at least one more time.  If no such TD is found, then
        // the current heads of the lists must be the true heads and we can
        // quit looping.
        //

        for (Td = OHCD_GlobalPool.FirstTD; Td <= OHCD_GlobalPool.LastTD; Td++)
        {
            if(
                (Td->HostControllerNumber == HostControllerNumber) &&
                (Td->HcTransferDescriptor.NextTD != 0)
            )
            {
               if (Td->HcTransferDescriptor.NextTD == TdList1)
               {
                    TdList1 = Td->PhysicalAddress;
                    TdList1Length++;
                    updated = TRUE;
               }
               else if (Td->HcTransferDescriptor.NextTD == TdList2)
               {
                    TdList2 = Td->PhysicalAddress;
                    TdList2Length++;
                    updated = TRUE;
                }
            }
        }
    } while (updated);

    //For logging purposes keep the minimum list length
    if(TdList1Length < OHCD_GlobalPool.LostMinimumDoneListLength)
    {
        OHCD_GlobalPool.LostMinimumDoneListLength = TdList1Length;
        USB_DBG_WARN_PRINT(("New LostMinimumDoneListLength %d .", OHCD_GlobalPool.LostMinimumDoneListLength));
    }

    ASSERT(TdList1 != 0);
    ASSERT(TdList2 == 0);

    return TdList1;
}

#endif //SILVER

/**
*** Another type of pool type workaround.  This workaround is for a broken a NV2A on the first
*** silicon revision.  It should be fixed on the next revision, but unfortunately we need a
*** workaround to get through the next several weeks.
***
*** All transfers need to be executed to\from uncached memory.  A big sweeping fix is to double
*** buffer all transfers.  This code manages that.
***
*** During initialization of the pool, OHCD_PoolCreateDoubleBufferPool is called.  It allocates
*** a pool and creates two free lists, one for large buffer and one for small buffers.
***
*** OHCD_PoolStartDoubleBufferTransfer is called whenever an URB is about to be programmed.
*** It pops a buffer off the correctfree list and assigns it to the URB.  On a write, it also copies
*** the data over.
***
*** OHCD_PoolEndDoubleBufferTransfer is called whenever a transfer completes.  On a read, it first
*** copies over the bytes read to the original buffer.  Then it places the double buffer back on
*** the correct free list and restores the original buffer in the URB.
***
*** It is asserted that POOL_DOUBLE_BUFFER_SIZE and POOL_DOUBLE_BUFFER_COUNT are large enough.
***
**/  

#ifdef DVTSNOOPBUG

//These values should allocate 3 4k pages.
#define POOL_LARGE_DOUBLE_BUFFER_SIZE  1024
#define POOL_LARGE_DOUBLE_BUFFER_COUNT 9
#define POOL_SMALL_DOUBLE_BUFFER_SIZE  64
#define POOL_SMALL_DOUBLE_BUFFER_COUNT 27

typedef struct _OHCD_POOL_UNCACHED_BUFFER *POHCD_POOL_UNCACHED_BUFFER;
typedef struct _OHCD_POOL_UNCACHED_BUFFER
{
  PVOID                         UserBuffer;
  union
  {
    POHCD_POOL_UNCACHED_BUFFER  *PoolHead;
    POHCD_POOL_UNCACHED_BUFFER  NextFree;
  };
  CHAR                          Buffer[0];
} OHCD_POOL_UNCACHED_BUFFER;

POHCD_POOL_UNCACHED_BUFFER OHCD_PoolSmallDoubleBufferFreeList = NULL;
POHCD_POOL_UNCACHED_BUFFER OHCD_PoolLargeDoubleBufferFreeList = NULL;

VOID OHCD_PoolCreateDoubleBufferPool()
{
    ULONG     poolSize;
    ULONG_PTR poolMemory;
    POHCD_POOL_UNCACHED_BUFFER bufferPtr;
    ULONG largeBufferSize = sizeof(OHCD_POOL_UNCACHED_BUFFER)+POOL_LARGE_DOUBLE_BUFFER_SIZE;
    ULONG smallBufferSize = sizeof(OHCD_POOL_UNCACHED_BUFFER)+POOL_SMALL_DOUBLE_BUFFER_SIZE;
    int i;

    //Allocate the memory
    poolSize = largeBufferSize*POOL_LARGE_DOUBLE_BUFFER_COUNT+
               smallBufferSize*POOL_SMALL_DOUBLE_BUFFER_COUNT;
    //round up to a full page
    if(poolSize%PAGE_SIZE) poolSize += (PAGE_SIZE-(poolSize%PAGE_SIZE));
    poolMemory = (ULONG_PTR)MmAllocateContiguousMemoryEx(
                                poolSize, 0, MAXULONG_PTR, 0, PAGE_READWRITE|PAGE_NOCACHE);    
    ASSERT(poolMemory);

    // Create the large buffer free list
    bufferPtr = (POHCD_POOL_UNCACHED_BUFFER)poolMemory;
    for(i=0; i < POOL_LARGE_DOUBLE_BUFFER_COUNT; i++)
    {
        bufferPtr->NextFree = OHCD_PoolLargeDoubleBufferFreeList;
        OHCD_PoolLargeDoubleBufferFreeList = bufferPtr;
        bufferPtr = (POHCD_POOL_UNCACHED_BUFFER)(((ULONG_PTR)bufferPtr)+largeBufferSize);
    }

    // Create the small buffer free list
    for(i=0; i < POOL_SMALL_DOUBLE_BUFFER_COUNT; i++)
    {
        bufferPtr->NextFree = OHCD_PoolSmallDoubleBufferFreeList;
        OHCD_PoolSmallDoubleBufferFreeList = bufferPtr;
        bufferPtr = (POHCD_POOL_UNCACHED_BUFFER)(((ULONG_PTR)bufferPtr)+smallBufferSize);
    }
}

VOID OHCD_PoolStartDoubleBufferTransfer(PURB Urb)
{
    POHCD_POOL_UNCACHED_BUFFER uncachedBuffer;
    ULONG                      transferLength = Urb->CommonTransfer.TransferBufferLength;
    
    ASSERT(0 != transferLength);
    ASSERT(0 != Urb->CommonTransfer.TransferBuffer);
    ASSERT(POOL_LARGE_DOUBLE_BUFFER_SIZE >= transferLength);

    //Grab a buffer from a free list
    if(transferLength > POOL_SMALL_DOUBLE_BUFFER_SIZE)
    {
        uncachedBuffer = OHCD_PoolLargeDoubleBufferFreeList;
        ASSERT(uncachedBuffer); //increase POOL_DOUBLE_BUFFER_COUNT if this is hit
        OHCD_PoolLargeDoubleBufferFreeList = uncachedBuffer->NextFree;
        uncachedBuffer->PoolHead = &OHCD_PoolLargeDoubleBufferFreeList;
    } else
    {
        uncachedBuffer = OHCD_PoolSmallDoubleBufferFreeList;
        ASSERT(uncachedBuffer); //increase POOL_DOUBLE_BUFFER_COUNT if this is hit
        OHCD_PoolSmallDoubleBufferFreeList = uncachedBuffer->NextFree;
        uncachedBuffer->PoolHead = &OHCD_PoolSmallDoubleBufferFreeList;
    }
    
    //swap the original buffer for the uncached buffer.
    uncachedBuffer->UserBuffer = Urb->CommonTransfer.TransferBuffer;
    Urb->CommonTransfer.TransferBuffer = uncachedBuffer->Buffer;

    //if it is a write copy the data to the uncached buffer
    if(USB_TRANSFER_DIRECTION_OUT == Urb->CommonTransfer.TransferDirection)
    {
       RtlCopyMemory(uncachedBuffer->Buffer, uncachedBuffer->UserBuffer, transferLength);
    }

}

VOID OHCD_PoolEndDoubleBufferTransfer(PURB Urb)
{
    
    POHCD_POOL_UNCACHED_BUFFER uncachedBuffer = CONTAINING_RECORD(
                                                  Urb->CommonTransfer.TransferBuffer,
                                                  OHCD_POOL_UNCACHED_BUFFER,
                                                  Buffer
                                                  );
    POHCD_POOL_UNCACHED_BUFFER *poolHead = uncachedBuffer->PoolHead;

    //if it was a read, copy the data to the user buffer
    if(USB_TRANSFER_DIRECTION_IN == Urb->CommonTransfer.TransferDirection)
    {
        ULONG transferLength = Urb->CommonTransfer.TransferBufferLength;
        ASSERT(uncachedBuffer);
        ASSERT(uncachedBuffer->UserBuffer);
        ASSERT(transferLength <= Urb->CommonTransfer.Hca.HcdOriginalLength);
        if(transferLength)
        {
            //
            //  The wierdness that follows is to due to the fact that one may
            //  DMA into PAGE_READONLY memory, but not write to under CPU control.
            //  The workaround here is to detect PAGE_READONLY memory, switch it
            //  to PAGE_READWRITE, perform the copy and then switch it back.
            //
            PVOID userBuffer = uncachedBuffer->UserBuffer;
            ULONG oldProtect = MmQueryAddressProtect(userBuffer);
            if(PAGE_READONLY == ((PAGE_READONLY | PAGE_READWRITE)&oldProtect))
            {
                ULONG newProtect = oldProtect;
                newProtect &= ~PAGE_READONLY; //clear readonly
                newProtect |= PAGE_READWRITE; //set readwrite
	            MmSetAddressProtect(userBuffer, transferLength, newProtect);
	            RtlCopyMemory(
                 userBuffer,
                 uncachedBuffer->Buffer,
                 transferLength
                 );   
	            MmSetAddressProtect(userBuffer, transferLength, oldProtect);
            } else
            {
                RtlCopyMemory(
                 userBuffer,
                 uncachedBuffer->Buffer,
                 transferLength
                 );   
            }
        }
    }

    //swap the original buffer for the uncached buffer.
    Urb->CommonTransfer.TransferBuffer = uncachedBuffer->UserBuffer;

    //return the uncached buffer to the free list
    uncachedBuffer->NextFree = *poolHead;
    *poolHead = uncachedBuffer;
}

#endif //DVTSNOOPBUG
