/*++

Copyright (c) 1992-1999  Microsoft Corporation

Module Name:

    ntddnetd.h

Abstract:

    Header file for Netcard Detection DLLs

Author:

    Sean Selitrennikoff (SeanSe) December 1992

Revision History:

--*/

#ifndef _NTDDNETD_
#define _NTDDNETD_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// Routines for Detection DLLs.
//


NTSTATUS
__stdcall
NDetCheckPortUsage(
    IN  INTERFACE_TYPE InterfaceType,
    IN  ULONG BusNumber,
    IN  ULONG Port,
    IN  ULONG Length
    );

NTSTATUS
__stdcall
NDetReadPortUchar(
    IN  INTERFACE_TYPE InterfaceType,
    IN  ULONG BusNumber,
    IN  ULONG Port,
    OUT PUCHAR Value
    );

NTSTATUS
__stdcall
NDetReadPortUshort(
    IN  INTERFACE_TYPE InterfaceType,
    IN  ULONG BusNumber,
    IN  ULONG Port,
    OUT PUSHORT Value
    );

NTSTATUS
__stdcall
NDetReadPortUlong(
    IN  INTERFACE_TYPE InterfaceType,
    IN  ULONG BusNumber,
    IN  ULONG Port,
    OUT PULONG Value
    );

NTSTATUS
__stdcall
NDetWritePortUchar(
    IN  INTERFACE_TYPE InterfaceType,
    IN  ULONG BusNumber,
    IN  ULONG Port,
    IN  UCHAR Value
    );

NTSTATUS
__stdcall
NDetWritePortUshort(
    IN  INTERFACE_TYPE InterfaceType,
    IN  ULONG BusNumber,
    IN  ULONG Port,
    IN  USHORT Value
    );

NTSTATUS
__stdcall
NDetWritePortUlong(
    IN  INTERFACE_TYPE InterfaceType,
    IN  ULONG BusNumber,
    IN  ULONG Port,
    IN  ULONG Value
    );

NTSTATUS
__stdcall
NDetCheckMemoryUsage(
    IN  INTERFACE_TYPE InterfaceType,
    IN  ULONG BusNumber,
    IN  ULONG BaseAddress,
    IN  ULONG Length
    );

NTSTATUS
__stdcall
NDetReadMappedMemory(
    IN  INTERFACE_TYPE InterfaceType,
    IN  ULONG BusNumber,
    IN  ULONG BaseAddress,
    IN  ULONG Length,
    OUT PVOID Data
    );

NTSTATUS
__stdcall
NDetWriteMappedMemory(
    IN  INTERFACE_TYPE InterfaceType,
    IN  ULONG BusNumber,
    IN  ULONG BaseAddress,
    IN  ULONG Length,
    IN  PVOID Data
    );

NTSTATUS
__stdcall
NDetSetInterruptTrap(
    IN  INTERFACE_TYPE InterfaceType,
    IN  ULONG BusNumber,
    OUT PHANDLE TrapHandle,
    IN  UCHAR InterruptList[],
    IN  ULONG InterruptListLength
    );

NTSTATUS
__stdcall
NDetQueryInterruptTrap(
    IN  HANDLE TrapHandle,
    OUT UCHAR InterruptList[],
    IN  ULONG InterruptListLength
    );

NTSTATUS
__stdcall
NDetRemoveInterruptTrap(
    IN  HANDLE TrapHandle
    );

NTSTATUS
__stdcall
NDetReadEisaSlotInformation(
    IN  ULONG   BusNumber,
    IN  ULONG   SlotNumber,
    IN  ULONG   Length,
    OUT PVOID   Data
    );

NTSTATUS
__stdcall
NDetReadMcaSlotInformation(
    IN  ULONG   BusNumber,
    IN  ULONG   SlotNumber,
    IN  ULONG   Length,
    OUT PVOID   Data
    );

typedef struct _NETDTECT_DMA_STATUS {

    ULONG   CurrentCount;

} NETDTECT_DMA_STATUS, *PNETDTECT_DMA_STATUS;

NTSTATUS
__stdcall
NDetAllocateDmaChannel(
    IN INTERFACE_TYPE InterfaceType,
    IN ULONG BusNumber,
    IN ULONG DmaChannel,
    OUT PHANDLE DmaHandle
    );

NTSTATUS
__stdcall
NDetBeginDmaTransfer(
    IN HANDLE DmaHandle,
    IN ULONG TransferLength,
    IN BOOLEAN WriteToDevice,
    IN UCHAR FillValue OPTIONAL
    );

NTSTATUS
__stdcall
NDetEndDmaTransfer(
    IN HANDLE DmaHandle
    );

NTSTATUS
__stdcall
NDetQueryDmaChannel(
    IN HANDLE DmaHandle,
    OUT PNETDTECT_DMA_STATUS DmaStatus
    );

NTSTATUS
__stdcall
NDetFreeDmaChannel(
    IN HANDLE DmaHandle
    );


//
// Resource information for Detection DLLs
//

#define NETDTECT_IRQ_RESOURCE    1
#define NETDTECT_MEMORY_RESOURCE 2
#define NETDTECT_PORT_RESOURCE   3
#define NETDTECT_DMA_RESOURCE    4

#define NETDTECT_IRQ_RESOURCE_LEVEL_SENSITIVE CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE
#define NETDTECT_IRQ_RESOURCE_LATCHED CM_RESOURCE_INTERRUPT_LATCHED

typedef struct _NETDTECT_RESOURCE {

    INTERFACE_TYPE InterfaceType;
    ULONG BusNumber;
    ULONG Type;
    ULONG Value;
    ULONG Length;
    ULONG Flags;

} NETDTECT_RESOURCE, *PNETDTECT_RESOURCE;

NTSTATUS
__stdcall
NDetTemporaryClaimResource(
    IN  PNETDTECT_RESOURCE Resource
    );

NTSTATUS
__stdcall
NDetFreeTemporaryResources(
    );

NTSTATUS
__stdcall
NDetFreeSpecificTemporaryResource(
    IN  PNETDTECT_RESOURCE  Resource
    );

#ifdef __cplusplus
}
#endif

#endif  // _NTDDNETD_

