/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    ohcd.h

Abstract:

    Structures used by the OpenHCI hardware dependent driver.
    These are constructs specific to this driver implementation.
    Standard Structures defined by Specification are in ohci.h
    
Environment:

    Designed for XBOX.

Notes:

Revision History:

    01-14-00 created by Mitchell Dernis (mitchd)

--*/
#ifndef __OCHD_H__
#define __OCHD_H__

//
//  Pull in the hardware spec stuff
//
#include "ohci.h"

//
//  forward declaration of pointer types, easy access later.
//
typedef struct _OHCD_TRANSFER_DESCRIPTOR        *POHCD_TRANSFER_DESCRIPTOR;
typedef struct _OHCD_SETUP_BUFFER               *POHCD_SETUP_BUFFER;
typedef struct _OHCD_ENDPOINT                   *POHCD_ENDPOINT;
typedef struct _OHCD_POOL                       *POHCD_POOL;
typedef struct _OHCD_INTERRUPT_SCHEDULE_NODE    *POHCD_INTERRUPT_SCHEDULE_NODE;
typedef struct _OHCD_ENDPOINT_SCHEDULE          *POHCD_ENDPOINT_SCHEDULE;
typedef struct _OHCD_DEVICE_EXTENSION           *POHCD_DEVICE_EXTENSION;


//
//  READ_REGISTER_ULONG and WRITE_REGISTER_ULONG are no longer
//  support in the Xbox Kernel.  Registers must be mapped to 
//  noncached memory.  Define macros here that simply do
//  a C assignement.
//
#define WRITE_REGISTER_ULONG(preg,value)  (*((volatile ULONG *)(preg))=value)
#define READ_REGISTER_ULONG(preg)  (*((volatile ULONG *)(preg)))

//
// struct OHCD_TRANSFER_DESCRIPTOR
//
//  This is the transfer descriptor as seen by the OHCD driver.
//  It is padded with 16 bytes of driver related fields.
//  Followed by the OHCI defined structure.
//
typedef struct _OHCD_TRANSFER_DESCRIPTOR
{
    //
    //  OHCI defined structure
    //
    OHCI_TRANSFER_DESCRIPTOR    HcTransferDescriptor;

    //
    //  16 bytes of Extra driver related fields
    //
    ULONG                       PhysicalAddress;        //Physical address of this descriptor
    union{
    POHCD_TRANSFER_DESCRIPTOR   NextFree;               //Used when the block is on the free list.
    POHCD_ENDPOINT              Endpoint;               //Endpoint which executed this transfer
    };
    PURB                        Urb;                    //Urb we are connected to
    UCHAR                       Flags;                  //Various flags
    UCHAR                       Bytes;                  //Various flags
    UCHAR                       Type;                   //Type of TD
    UCHAR                       HostControllerNumber;   //0xFF when not in use.
} OHCD_TRANSFER_DESCRIPTOR;

#define OHCD_TD_FLAG_CANCEL_PENDING 0x00000001  //TD has been marked for cancel
#define OHCD_TD_FLAG_LAST_TD        0x00000002  //Last TD of an URB

#define OHCD_TD_TYPE_DATA           0   //Setup TD of control transfer
#define OHCD_TD_TYPE_SETUP          1   //Setup TD of control transfer
#define OHCD_TD_TYPE_STATUS         2   //Status TD of control transfer
#define OHCD_TD_TYPE_DUMMY          3   //Dummy TD of control transfer


//
//  Flags used for the HcdUrbFlags found in the HCD area
//  of asynchronous transfer URBS.  These flags are used
//  for tracking the state of an URB so that it can be
//  cancelled.
//
#define OHCD_URB_FLAG_CANCELED      0x0001
#define OHCD_URB_FLAG_QUEUED        0x0002
#define OHCD_URB_FLAG_PROGRAMMED    0x0004
#define OHCD_URB_FLAG_COMPLETED     0x0008
//  Flags for isoch use are defined in isoch.h
//  This mask just claims the bits.
#define OHCD_URB_FLAG_ISOCH_USE     0x0F00


//
// struct OHCD_SETUP_STATUS_BUFFER
//
//  This structure is used for control transfers
//  to hold the setup packet data.  We define it as a type of
//  descriptor block so that we can leverage
//  the same code used for EDs and TDs
//  
typedef struct _OHCD_SETUP_BUFFER
{
    UCHAR                   Setup[8];       //Holds the data for a setup packet
    ULONG                   Pad1[2];        //Padding to keep the structure the correct size
                                            
    //
    //  Standard driver dependent part of block
    //
    ULONG                   PhysicalAddress;        //Physical address of this block
    ULONG                   Pad2[3];                //Pad to 32 bytes
} OHCD_SETUP_BUFFER;

//
//  Interrupt Node - see schedule.c for overview
//
typedef struct _OHCD_INTERRUPT_SCHEDULE_NODE
{
    BOOLEAN         RegisterHead;
    UCHAR           Pad;
    USHORT          Bandwidth;
    USHORT          BandwidthChildren;
    USHORT          BandwidthParents;
    POHCD_ENDPOINT  EndpointHead;
    POHCD_ENDPOINT  EndpointTail;
} OHCD_INTERRUPT_SCHEDULE_NODE;


//
//  Endpoint schedule - see schedule.c for overview
//
typedef struct _OHCD_ENDPOINT_SCHEDULE
{
    OHCD_INTERRUPT_SCHEDULE_NODE InterruptSchedule[64];
    POHCD_ENDPOINT               ControlHead;
    POHCD_ENDPOINT               BulkHead;
    USHORT                       BandwidthPeriodic;
    USHORT                       BandwidthTotal;                         
} OHCD_ENDPOINT_SCHEDULE;


//
//  Endpoint.   Due to the first member this structure must start on a 16-byte boundary.
//
typedef struct _OHCD_ENDPOINT
{
    //
    //  OHCI mandated portion of endpoint.  This structure
    //  must start on a paragraph(16-byte) boundary.
    //
    OHCI_ENDPOINT_DESCRIPTOR    HcEndpointDescriptor;
    //
    //  Fields for managing the schedule (especially for interrupt)
    //
    UCHAR                       Flags;                  //Endpoint flags
    UCHAR                       EndpointType;           //Type of endpoint
    UCHAR                       ScheduleIndex;          //Index in schedule - see definitions above
    UCHAR                       PollingInterval;        //Maximum polling interval (in ms)
    
    ULONG                       PhysicalAddress;        //Physical address of this structure.
    POHCD_ENDPOINT              Next;                   //Next endpoint in schedule

    ULONG                       PauseFrame;             //Used to verify that we have waited at least

    UCHAR                       PendingPauseCount;      //Number of reasons that we are paused.
    UCHAR                       Padding;                //Padding to keep up DWORD alignement.
    USHORT                      Bandwidth;              //Bandwidth required by this endpoint (interrupt and isoch only)
    
    USHORT                      TDInUseCount;           //Count of TDs in use.
    UCHAR                       QueuedUrbCount;         //Count of URBs that are queued waiting to be programmed.
    UCHAR                       ProgrammedUrbCount;     //Count URBs that are currently programmed.
    
    //
    //  Fields for managing URBs and keeping track of transfers
    //
    PURB                        PendingUrbHeadP;
    PURB                        PendingUrbTailP;
    
} OHCD_ENDPOINT;


#define OHCD_ENDPOINT_FLAG_CLOSING      0x10        //Flag used to indicate that endpoint is being
                                                    //closed, block cancels when endpoint is closing
#define OHCD_ENDPOINT_FLAG_PAUSING      0x20        //Flag used so that we don't try
                                                    //to pause the endpoint twice.
#define OHCD_ENDPOINT_FLAG_DELAY_PAUSE  0x40        //Indicates that the endpoint should not be considered
                                                    //paused until the next interrupt, set by the pause
                                                    //routine if a DPC was already queued.

typedef struct _OHCD_ROOT_HUB_OBJECT
{
    UCHAR                   NumberOfPorts;              //Number of ports present on the root hub.
    UCHAR                   DeviceDetectedBitmap;       //Bitmap of ports which have a detected device
    LARGE_INTEGER           PowerOnToGoodTime;          //PowerOnToGoodTime in relative 100ns intervals
    PFNHCD_RESET_COMPLETE   ResetComplete;              //Completion Routine.
    PVOID                   ResetContext;               //Completion Context.
    KTIMER                  ResetTimeoutTimer;          //Timer in case reset times out.
    KDPC                    ResetTimeoutDPC;            //DPC to be called if reset time-out occurs.
} OHCD_ROOT_HUB_OBJECT, *POHCD_ROOT_HUB_OBJECT;

VOID
HCD_ResetRootHubPort(
    IN PVOID                    HcdDeviceExtension,
    IN ULONG                    PortNumber,
    IN PFNHCD_RESET_COMPLETE    ResetCompleteProc,
    IN PVOID                    CompleteContext
    );


typedef struct _OHCD_DEVICE_EXTENSION
{
    
    POHCI_OPERATIONAL_REGISTERS OperationalRegisters;       //Port address of operational registers
    ULONG                       OperationalRegistersLength; //May vary depending on downstream port count
    POHCI_HCCA                  HCCA;                       //HCD accessable pointer to HCCA
    OHCD_ENDPOINT_SCHEDULE      Schedule;                   //Everything we need to know about the schedule
    ULONG                       FrameHighPart;              //The high part of a 32-bit frame counter.
                                                            //The hardware only keeps 16 bits.
    PURB                        ControlUrbHeadP;            //Head of URBs for control transfers
    PURB                        ControlUrbTailP;            //Tail of URBs for control transfers
    PURB                        BulkUrbHeadP;               //Head of URBs for bulk transfers
    PURB                        BulkUrbTailP;               //Tail of URBs for bulk transfers
    PURB                        PendingCancels;             //URBs that need to be canceled.
    PURB                        PendingAborts;              //URBs that need to be aborted.
    PURB                        PendingCloses;              //URBs representing pending closes.
    struct
    {  /* A context structure between Isr and Dpc */
      ULONG InterruptsSignaled;     //Bit field of interrupt conditions signaled
      ULONG Frame;                  //The frame number when the interrupt occured
    } IsrDpc_Context;
    KDPC                        IsrDpc;                     //DPC for the ISR
    ULONG                       HostControllerNumber;       //Host Controller Number - really only a byte
    OHCD_ROOT_HUB_OBJECT        RootHubObject;              //Everything the root hub needs to know
    HAL_SHUTDOWN_REGISTRATION   ShutdownRegistration;
} OHCD_DEVICE_EXTENSION;

//
//  WorkItem Payload for Hotplug items.
//
typedef struct _OHCD_PORT_INFO
{
    USHORT                  PortsConnectionChanged;     //Bitfield of ports whose connection status needs updating
    USHORT                  PortsConnected;             //Bitfield of ports that now show connected status
} OHCD_PORT_INFO, *POHCD_PORT_INFO;


//---------------------------------------------------------------------------------------------------------------
// Choose max controller based on the number of controllers
//---------------------------------------------------------------------------------------------------------------
#define HCD_MAX_HOST_CONTROLLERS 1
#if (USB_HOST_CONTROLLER_CONFIGURATION!=USB_SINGLE_HOST_CONTROLLER)
#undef  HCD_MAX_HOST_CONTROLLERS
#define HCD_MAX_HOST_CONTROLLERS 2
#endif

//------------------------------------------------------
//  Pull in Isochronous Support (or stubs)
//------------------------------------------------------
#include "isoch.h"

//------------------------------------------------------
//  Implemented in OHCD.C
//------------------------------------------------------
VOID
FASTCALL
OHCD_fPauseEndpoint(
    POHCD_DEVICE_EXTENSION DeviceExtension,
    POHCD_ENDPOINT  endpoint
    );
//--------------------------------------------------------
// Implemented in Schedule.c
//--------------------------------------------------------
VOID
FASTCALL
OHCD_ScheduleInitialize(
    IN POHCD_DEVICE_EXTENSION DeviceExtension
    );

USBD_STATUS
FASTCALL
OHCD_ScheduleAddEndpointPeriodic(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    IN POHCD_ENDPOINT Endpoint
    );

VOID
FASTCALL
OHCD_ScheduleRemoveEndpointPeriodic(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    IN POHCD_ENDPOINT Endpoint
    );

VOID
FASTCALL
OHCD_ScheduleAddEndpointControlOrBulk(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    IN POHCD_ENDPOINT Endpoint
    );

VOID
FASTCALL
OHCD_ScheduleRemoveEndpointControlOrBulk(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    IN POHCD_ENDPOINT Endpoint
    );

//------------------------------------------------------------
// Implemented in Isr.c
//------------------------------------------------------------

BOOLEAN
OHCD_InterruptService( 
    IN PKINTERRUPT Interrupt, 
    IN PVOID ServiceContext 
    );

VOID
OHCD_IsrDpc(
    PKDPC Dpc,
    PVOID DeviceExtensionPtr,
    PVOID Bogus1,
    PVOID Bogus2
    );

ULONG
FASTCALL
OHCD_Get32BitFrameNumber(
    IN POHCD_DEVICE_EXTENSION DeviceExtension
    );

//------------------------------------------------------------
// Implemented in transfer.c
//------------------------------------------------------------
USBD_STATUS
FASTCALL
OHCD_fQueueTransferRequest(
    POHCD_DEVICE_EXTENSION  DeviceExtension,
    PURB                    HcdUrb
    );

VOID
FASTCALL
OHCD_fProgramInterruptTransfer(
    POHCD_DEVICE_EXTENSION  DeviceExtension,
    POHCD_ENDPOINT          Endpoint
    );

VOID
FASTCALL
OHCD_fProgramBulkTransfer(
    POHCD_DEVICE_EXTENSION  DeviceExtension
    );

VOID
FASTCALL
OHCD_fProgramControlTransfer(
    POHCD_DEVICE_EXTENSION  DeviceExtension
    );

//------------------------------------------------------------
// Implemented in roothub.c
//------------------------------------------------------------
VOID
FASTCALL
OHCD_RootHubInitialize(
    POHCD_DEVICE_EXTENSION  DeviceExtension
    );
VOID
FASTCALL
OHCD_RootHubProcessInterrupt(
    POHCD_DEVICE_EXTENSION      DeviceExtension
);

VOID
FASTCALL
OHCD_RootHubProcessHotPlug(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN POHCD_PORT_INFO          PortInfo
);

//------------------------------------------------------
//  Implemented in pool.c
//------------------------------------------------------

//
//  struct OHCD_POOL - Global structure pool for
//                     all DMA managed structures
//
typedef struct _OHCD_POOL
{
    
    ULONG                       VirtualToPhysical;
    POHCI_HCCA                  Hcca[HCD_MAX_HOST_CONTROLLERS];
    POHCD_ENDPOINT              FreeEndpoints;
    POHCD_TRANSFER_DESCRIPTOR   FreeTDs;
    POHCD_TRANSFER_DESCRIPTOR   FirstTD;                   //First TD in pool
    POHCD_TRANSFER_DESCRIPTOR   LastTD;                    //Last TD in pool
    ULONG                       LostDoneHeadCount;         //Number of times the DoneHead was lost.
    ULONG                       LostMinimumDoneListLength; //minimum length of the DoneHead when
                                                           //it was lost
    USHORT                      ControlQuota;
    USHORT                      ControlQuotaRemaining;
    USHORT                      BulkQuota;
    USHORT                      BulkQuotaRemaining;
    OHCD_ISOCH_POOL
} OHCD_POOL;
extern OHCD_POOL OHCD_GlobalPool;
#define OHCD_INTERRUPT_TD_QUOTA  3 //Per endpoint quota

VOID
FASTCALL
OHCD_fPoolInit(
    IN PHCD_RESOURCE_REQUIREMENTS pResourceRequirements
    );

#ifdef SILVER
ULONG
FASTCALL
OHCD_fPoolFindLostDoneHead(
    POHCD_DEVICE_EXTENSION DeviceExtension
    );
#endif //SILVER

__inline
POHCI_HCCA
OHCD_PoolGetHcca(
    IN UCHAR HostControllerNumber
    )
{
    ASSERT(HCD_MAX_HOST_CONTROLLERS > HostControllerNumber);
    return OHCD_GlobalPool.Hcca[HostControllerNumber];
}

__inline
POHCD_ENDPOINT
OHCD_PoolAllocateEndpoint()
{
    POHCD_ENDPOINT retVal;
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    retVal = OHCD_GlobalPool.FreeEndpoints;
    if(retVal)
    {
        OHCD_GlobalPool.FreeEndpoints = retVal->Next;
    }
    ASSERT(retVal);
    return retVal;
}

__inline
VOID
OHCD_PoolFreeEndpoint(POHCD_ENDPOINT Endpoint)
{
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    Endpoint->Next = OHCD_GlobalPool.FreeEndpoints;
    OHCD_GlobalPool.FreeEndpoints = Endpoint;
}


__inline
POHCD_TRANSFER_DESCRIPTOR
OHCD_PoolAllocateTD(UCHAR HostControllerNumber)
{
    POHCD_TRANSFER_DESCRIPTOR retVal;
    retVal = OHCD_GlobalPool.FreeTDs;
    if(retVal)
    {
        OHCD_GlobalPool.FreeTDs = retVal->NextFree;
    }
    ASSERT(retVal);
    retVal->HostControllerNumber = HostControllerNumber;
    OHCD_CLEAR_TD_ISOCH_FORMAT_BIT(retVal);
    return retVal;
}

__inline
VOID
OHCD_PoolFreeTD(
    IN  POHCD_TRANSFER_DESCRIPTOR   TransferDescriptor
    )
{
    // We want the hardware NextTD to be an invalid
    // non-NULL address.  This aids in searching for
    // lost TD lists.
    TransferDescriptor->HcTransferDescriptor.NextTD = 0xFFFFFFFF;
    TransferDescriptor->HostControllerNumber  = 0xFF;
    TransferDescriptor->NextFree = OHCD_GlobalPool.FreeTDs;
    OHCD_GlobalPool.FreeTDs = TransferDescriptor;
}

__inline
POHCD_TRANSFER_DESCRIPTOR
OHCD_PoolTDFromPhysicalAddress(
    IN  ULONG           PhysicalAddress
    )
{
    return (POHCD_TRANSFER_DESCRIPTOR)(OHCD_GlobalPool.VirtualToPhysical + PhysicalAddress);
}

__inline
ULONG
OHCD_PoolGetPhysicalAddress(PVOID VirtualAddress)
{
    return ((ULONG)VirtualAddress) - OHCD_GlobalPool.VirtualToPhysical;
}

__inline
USHORT
OHCD_PoolGetControlQuota()
{ return OHCD_GlobalPool.ControlQuota; }

__inline
BOOLEAN
OHCD_PoolDebitControlTDQuota(
    IN USHORT Count
    )
{
    return
        (OHCD_GlobalPool.ControlQuotaRemaining < Count) ?
            FALSE :
            (OHCD_GlobalPool.ControlQuotaRemaining -= Count, TRUE);
}

__inline
VOID
OHCD_PoolCreditControlTDQuota(
    IN USHORT Count
    )
{
    OHCD_GlobalPool.ControlQuotaRemaining += Count;
}

__inline
USHORT
OHCD_PoolGetBulkQuota()
{ return OHCD_GlobalPool.BulkQuota; }

__inline
BOOLEAN
OHCD_PoolDebitBulkTDQuota(
    IN USHORT Count
    )
{
    return
        (OHCD_GlobalPool.BulkQuotaRemaining < Count) ?
            FALSE :
            (OHCD_GlobalPool.BulkQuotaRemaining -= Count, TRUE);
}

__inline
VOID
OHCD_PoolCreditBulkTDQuota(
    IN USHORT Count
    )
{
    OHCD_GlobalPool.BulkQuotaRemaining += Count;
}

#ifdef DVTSNOOPBUG
VOID OHCD_PoolCreateDoubleBufferPool();
VOID OHCD_PoolStartDoubleBufferTransfer(PURB Urb);
VOID OHCD_PoolEndDoubleBufferTransfer(PURB Urb);
#endif

//Isoch routines are conditionally compiled
#ifdef  OHCD_ISOCHRONOUS_SUPPORTED
__inline
ULONG_PTR
OHCD_IsochPoolAllocateEndpoint()
{
    ULONG_PTR retVal;
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    retVal = OHCD_GlobalPool.IsochFreeEndpoints;
    if(retVal)
    {
        OHCD_GlobalPool.IsochFreeEndpoints = *((PULONG_PTR)retVal);
    }
    ASSERT(retVal);
    return retVal;
}

__inline
VOID
OHCD_IsochPoolFreeEndpoint(ULONG_PTR IsochEndpoint)
{
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    *((PULONG_PTR)IsochEndpoint) = OHCD_GlobalPool.IsochFreeEndpoints;
    OHCD_GlobalPool.IsochFreeEndpoints = IsochEndpoint;
}

__inline
ULONG OHCD_IsochPoolGetMaxBuffers()
{
    return OHCD_GlobalPool.IsochMaxBuffers;
}

#endif //OHCD_ISOCHRONOUS_SUPPORTED


#endif __OCHD_H__
