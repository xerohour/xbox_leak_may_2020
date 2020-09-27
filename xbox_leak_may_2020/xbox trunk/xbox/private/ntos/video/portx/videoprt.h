/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    videoprt.h

Abstract:

    This module contains the private data structures and procedure prototypes
    for the video port driver.

--*/

#ifndef _VIDEOPRT_
#define _VIDEOPRT_

#include <ntos.h>
#include <pci.h>
#include <ntddvdeo.h>
#include <video.h>
#include <dderror.h>
#include <ntagp.h>
#include <stdio.h>
#include "agp440.h"

//
// DBG sensitive DbgPrint wrapper.
//

#if DBG
#define VideoDbgPrint(x)                        DbgPrint x
#else
#define VideoDbgPrint(x)
#endif

//
// Bit flag macros.
//

#define VideoIsFlagSet(flagset, flag)           (((flagset) & (flag)) != 0)
#define VideoIsFlagClear(flagset, flag)         (((flagset) & (flag)) == 0)
#define VideoIsBitSet(flagset, bit)             VideoIsFlagSet((flagset), (1 << (bit)))
#define VideoIsBitClear(flagset, bit)           VideoIsFlagClear((flagset), (1 << (bit)))

//
// Structure to track the size of memory mappings.
//

typedef struct _VIDEO_MAPPED_ADDRESS {
    PVOID MappedAddress;
    ULONG NumberOfUchars;
} VIDEO_MAPPED_ADDRESS, *PVIDEO_MAPPED_ADDRESS;

#define NUMBER_OF_VIDEO_MAPPED_ADDRESSES        8

//
// Video device extension data.
//

typedef struct _VIDEO_DEVICE_EXTENSION {
    PVIDEO_HW_FIND_ADAPTER HwFindAdapter;
    PVIDEO_HW_INITIALIZE HwInitialize;
    PVIDEO_HW_INTERRUPT HwInterrupt;
    PVIDEO_HW_START_IO HwStartIO;
    PVIDEO_HW_RESET_HW HwResetHw;
    PVIDEO_HW_TIMER HwTimer;
    ULONG BaseAddresses[PCI_TYPE0_ADDRESSES];
    VIDEO_MAPPED_ADDRESS MappedAddresses[NUMBER_OF_VIDEO_MAPPED_ADDRESSES];
    PKINTERRUPT InterruptObject;
    KDPC Dpc;
    BOOLEAN DeviceOpened;
    BOOLEAN InterruptsEnabled;
    KMUTEX SyncMutex;
    AGP_BUS_INTERFACE_STANDARD AgpInterface;
    FAST_MUTEX AgpInterfaceLock;
    AGP440_EXTENSION Agp440Extension;
} VIDEO_DEVICE_EXTENSION, *PVIDEO_DEVICE_EXTENSION;

//
// Macros to convert between the VIDEO_DEVICE_EXTENSION structure and the
// miniport's hardware extension.
//

#define VIDEO_GET_VIDEO_DEVICE_EXTENSION(hwe) \
    (((PVIDEO_DEVICE_EXTENSION)(hwe)) - 1)

#define VIDEO_GET_HW_DEVICE_EXTENSION(vde) \
    ((PVOID)(((PVIDEO_DEVICE_EXTENSION)(vde)) + 1))

//
// Define the slot configuration for the video adapter attached to an AGP440.
//

#define AGP440_TARGET_DEVICE_NUMBER             0
#define AGP440_TARGET_FUNCTION_NUMBER           0

//
// These macros are used to protect threads which will enter the
// miniport.  We need to guarantee that only one thread enters
// the miniport at a time.
//

#define ACQUIRE_DEVICE_LOCK(DeviceExtension)           \
    KeWaitForSingleObject(&DeviceExtension->SyncMutex, \
                          Executive,                   \
                          KernelMode,                  \
                          FALSE,                       \
                          (PTIME)NULL);

#define RELEASE_DEVICE_LOCK(DeviceExtension)           \
    KeReleaseMutex(&DeviceExtension->SyncMutex,        \
                   FALSE);

#endif // VIDEOPRT
