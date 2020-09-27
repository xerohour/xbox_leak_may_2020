/*++

Copyright (c) 2000-2002  Microsoft Corporation

Module Name:

    idex.h

Abstract:

    This module contains the private data structures and procedure prototypes
    for the IDE port driver.

--*/

#ifndef _IDEX_
#define _IDEX_

#include <ntos.h>
#include <pci.h>
#include <ldr.h>
#include <scsi.h>
#include <ntddcdrm.h>
#include <ntddcdvd.h>
#include <ntdddisk.h>
#include <ntddscsi.h>
#include <idexchan.h>
#include <xcrypt.h>
#include <dvdx2.h>
#include <xdisk.h>
#include <smcdef.h>
#include <xconfig.h>
#include <segalpc.h>
#include <stdio.h>
#include <conio.h>
#include <limits.h>
#include <pshpack4.h>

//
// Enforce a hard limit of XDISK_FIXED_SECTOR_COUNT hard disk sectors when the
// following is defined.
//

#define IDEX_DISK_FIXED_SECTOR_COUNT

//
// DBG sensitive DbgPrint and DbgBreakPoint wrappers.
//

#if DBG
#define IdexDbgPrint(x)                         DbgPrint x
#define IdexDbgBreakPoint()                     DbgBreakPoint()
#else
#define IdexDbgPrint(x)
#define IdexDbgBreakPoint()
#endif

//
// Bit flag macros.
//

#define IdexIsFlagSet(flagset, flag)            (((flagset) & (flag)) != 0)
#define IdexIsFlagClear(flagset, flag)          (((flagset) & (flag)) == 0)

//
// Bug check module codes.
//

#define IDE_BUG_CHECK_CDROM                     (0x00010000)
#define IDE_BUG_CHECK_CHANNEL                   (0x00020000)
#define IDE_BUG_CHECK_DISK                      (0x00030000)
#define IDE_BUG_CHECK_DRIVER                    (0x00040000)
#define IDE_BUG_CHECK_MEDIA_BOARD               (0x00050000)

//
// Define the number of times that an operation will be retried after a device
// error is detected or after a timeout occurs.  For all of the above IRP codes,
// the retry count is placed in Argument4 of the IRP stack.
//

#define IDE_NO_RETRY_COUNT                      0
#define IDE_NORMAL_RETRY_COUNT                  4

//
// Define the timeouts in seconds for various IDE operations.
//

#define IDE_ATA_DEFAULT_TIMEOUT                 10
#define IDE_ATA_FLUSH_TIMEOUT                   30
#define IDE_ATAPI_DEFAULT_TIMEOUT               12

//
// Define the timer periods in milliseconds for various states of IRP handling.
//

#define IDE_SLOW_TIMER_PERIOD                   1000
#define IDE_FAST_TIMER_PERIOD                   100

//
// Macros to synchronize execution with the interrupt service routine.  The
// proper way to handle this is via KeSynchronizeExecution, but we can generate
// smaller code by raising and lowering the interrupt IRQL ourselves.
//

#define IdexRaiseIrqlToChannelDIRQL(oldirql) \
    KeRaiseIrql(IdexChannelObject.InterruptIrql, (oldirql))

#define IdexRaiseIrqlToChannelDIRQLFromDPCLevel() { \
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL); \
    KfRaiseIrql(IdexChannelObject.InterruptIrql); \
}

#define IdexLowerIrqlFromChannelDIRQL(oldirql) \
    KeLowerIrql(oldirql);

#define IdexAssertIrqlAtChannelDIRQL() \
    ASSERT(KeGetCurrentIrql() == IdexChannelObject.InterruptIrql)

//
// Macros to wrap the indirection of routines through the channel object.
//

#define IdexChannelStartPacket(device, irp) \
    IdexChannelObject.StartPacketRoutine(device, irp)

#define IdexChannelStartNextPacket() \
    IdexChannelObject.StartNextPacketRoutine()

//
// Disk device extension data.
//

typedef struct _IDE_DISK_EXTENSION {
    PDEVICE_OBJECT DeviceObject;
    PARTITION_INFORMATION PartitionInformation;
} IDE_DISK_EXTENSION, *PIDE_DISK_EXTENSION;

//
// Media board device extension data.
//

typedef struct _IDE_MEDIA_BOARD_EXTENSION {
    PDEVICE_OBJECT DeviceObject;
    PARTITION_INFORMATION PartitionInformation;
} IDE_MEDIA_BOARD_EXTENSION, *PIDE_MEDIA_BOARD_EXTENSION;

//
// Define the default device numbers for the various IDE devices.
//

#define IDE_DISK_DEVICE_NUMBER                  0
#define IDE_CDROM_DEVICE_NUMBER                 1
#define IDE_MEDIA_BOARD_DEVICE_NUMBER           1

//
// Define the PCI resource for the bus master interface.
//

#define IDE_PCI_BUS_MASTER_BASE                 XPCICFG_IDE_IO_REGISTER_BASE_4
#define IDE_PCI_BUS_MASTER_BMICP                (IDE_PCI_BUS_MASTER_BASE + 0x0)
#define IDE_PCI_BUS_MASTER_BMISP                (IDE_PCI_BUS_MASTER_BASE + 0x2)
#define IDE_PCI_BUS_MASTER_BMIDP                (IDE_PCI_BUS_MASTER_BASE + 0x4)
#define IDE_PCI_BUS_MASTER_BMICS                (IDE_PCI_BUS_MASTER_BASE + 0x8)
#define IDE_PCI_BUS_MASTER_BMISS                (IDE_PCI_BUS_MASTER_BASE + 0xA)
#define IDE_PCI_BUS_MASTER_BMIDS                (IDE_PCI_BUS_MASTER_BASE + 0xC)

//
// Define the legacy ISA resources for single IDE channel support.
//

#define IDE_CHANNEL_COMMAND_BASE                0x01F0
#define IDE_CHANNEL_CONTROL_BASE                0x03F4
#define IDE_CHANNEL_IRQ_RESOURCE                14

//
// Macros to compute the IDE registers for an IDE channel.  These macros allow
// us to expand to multiple channels in the future.
//

#define IDE_DATA_REGISTER                       (IDE_CHANNEL_COMMAND_BASE + 0)
#define IDE_ERROR_REGISTER                      (IDE_CHANNEL_COMMAND_BASE + 1)
#define IDE_SECTOR_COUNT_REGISTER               (IDE_CHANNEL_COMMAND_BASE + 2)
#define IDE_SECTOR_NUMBER_REGISTER              (IDE_CHANNEL_COMMAND_BASE + 3)
#define IDE_CYLINDER_LOW_REGISTER               (IDE_CHANNEL_COMMAND_BASE + 4)
#define IDE_CYLINDER_HIGH_REGISTER              (IDE_CHANNEL_COMMAND_BASE + 5)
#define IDE_DEVICE_SELECT_REGISTER              (IDE_CHANNEL_COMMAND_BASE + 6)
#define IDE_STATUS_REGISTER                     (IDE_CHANNEL_COMMAND_BASE + 7)
#define IDE_COMMAND_REGISTER                    (IDE_CHANNEL_COMMAND_BASE + 7)
#define IDE_ALTERNATE_STATUS_REGISTER           (IDE_CHANNEL_CONTROL_BASE + 2)
#define IDE_DEVICE_CONTROL_REGISTER             (IDE_CHANNEL_CONTROL_BASE + 2)

//
// Macros to read and write from the IDE registers.  These macros allow us to
// hook the reads and writes to the registers on an individual basis.
//

#define IdexReadDataPortUchar() \
    IdexReadPortUchar(IDE_DATA_REGISTER)

#define IdexWriteDataPortUchar(data) \
    IdexWritePortUchar(IDE_DATA_REGISTER, (data))

#define IdexReadDataPortUshort() \
    IdexReadPortUshort(IDE_DATA_REGISTER)

#define IdexReadDataPortBufferUshort(buffer, count) \
    IdexReadPortBufferUshort(IDE_DATA_REGISTER, (buffer), (count))

#define IdexWriteDataPortBufferUshort(buffer, count) \
    IdexWritePortBufferUshort(IDE_DATA_REGISTER, (buffer), (count))

#define IdexReadDataPortBufferUlong(buffer, count) \
    IdexReadPortBufferUlong(IDE_DATA_REGISTER, (buffer), (count))

#define IdexWriteDataPortBufferUlong(buffer, count) \
    IdexWritePortBufferUlong(IDE_DATA_REGISTER, (buffer), (count))

#define IdexReadErrorPort() \
    IdexReadPortUchar(IDE_ERROR_REGISTER)

#define IdexReadSectorCountPort() \
    IdexReadPortUchar(IDE_SECTOR_COUNT_REGISTER)

#define IdexWriteSectorCountPort(data) \
    IdexWritePortUchar(IDE_SECTOR_COUNT_REGISTER, (data))

#define IdexReadSectorNumberPort() \
    IdexReadPortUchar(IDE_SECTOR_NUMBER_REGISTER)

#define IdexWriteSectorNumberPort(data) \
    IdexWritePortUchar(IDE_SECTOR_NUMBER_REGISTER, (data))

#define IdexReadCylinderLowPort() \
    IdexReadPortUchar(IDE_CYLINDER_LOW_REGISTER)

#define IdexWriteCylinderLowPort(data) \
    IdexWritePortUchar(IDE_CYLINDER_LOW_REGISTER, (data))

#define IdexReadCylinderHighPort() \
    IdexReadPortUchar(IDE_CYLINDER_HIGH_REGISTER)

#define IdexWriteCylinderHighPort(data) \
    IdexWritePortUchar(IDE_CYLINDER_HIGH_REGISTER, (data))

#define IdexReadDeviceSelectPort() \
    IdexReadPortUchar(IDE_DEVICE_SELECT_REGISTER)

#define IdexWriteDeviceSelectPort(data) \
    IdexWritePortUchar(IDE_DEVICE_SELECT_REGISTER, (data))

#define IdexReadStatusPort() \
    IdexReadPortUchar(IDE_STATUS_REGISTER)

#define IdexWriteCommandPort(data) \
    IdexWritePortUchar(IDE_COMMAND_REGISTER, (data))

#define IdexReadAlternateStatusPort() \
    IdexReadPortUchar(IDE_ALTERNATE_STATUS_REGISTER)

#define IdexWriteDeviceControlPort(data) \
    IdexWritePortUchar(IDE_DEVICE_CONTROL_REGISTER, (data))

//
// Macros to access the IDE registers using the ATAPI register names.
//

#define IdexWriteFeaturesPort(data) \
    IdexWritePortUchar(IDE_ERROR_REGISTER, (data))

#define IdexReadInterruptReasonPort     IdexReadSectorCountPort
#define IdexReadByteCountLowPort        IdexReadCylinderLowPort
#define IdexWriteByteCountLowPort       IdexWriteCylinderLowPort
#define IdexReadByteCountHighPort       IdexReadCylinderHighPort
#define IdexWriteByteCountHighPort      IdexWriteCylinderHighPort

//
// Macros to access the IDE registers from a more abstract level.
//

#define IdexProgramTargetDevice(device) \
    IdexWriteDeviceSelectPort((UCHAR)(0xA0 | ((device) << 4)))

#define IdexProgramTargetDeviceWithData(device, data) \
    IdexWriteDeviceSelectPort((UCHAR)(0xA0 | ((device) << 4) | ((data) & 0xF)))

#define IdexProgramLBATransfer(device, sector, count) { \
    IdexWriteDeviceSelectPort((UCHAR)(0xE0 | ((device) << 4) | \
        (((sector) & 0xF000000) >> 24))); \
    IdexWriteSectorNumberPort((UCHAR)(((sector) & 0xFF) >> 0)); \
    IdexWriteCylinderLowPort((UCHAR)(((sector) & 0xFF00) >> 8)); \
    IdexWriteCylinderHighPort((UCHAR)(((sector) & 0xFF0000) >> 16)); \
    IdexWriteSectorCountPort((UCHAR)(count)); \
}

#define IdexWriteDataPortCdb(cdb) \
    IdexWriteDataPortBufferUlong((PULONG)(cdb), 3);

//
// Define the IDE status register flags.
//

#define IDE_STATUS_ERR                          ((UCHAR)0x01)
#define IDE_STATUS_DRQ                          ((UCHAR)0x08)
#define IDE_STATUS_DRDY                         ((UCHAR)0x40)
#define IDE_STATUS_BSY                          ((UCHAR)0x80)

//
// Define the IDE device control register flags.
//

#define IDE_DEVICE_CONTROL_NIEN                 ((UCHAR)0x02)
#define IDE_DEVICE_CONTROL_SRST                 ((UCHAR)0x04)

//
// Define the IDE command values.
//

#define IDE_COMMAND_DEVICE_RESET                ((UCHAR)0x08)
#define IDE_COMMAND_READ_SECTORS                ((UCHAR)0x20)
#define IDE_COMMAND_WRITE_SECTORS               ((UCHAR)0x30)
#define IDE_COMMAND_VERIFY                      ((UCHAR)0x40)
#define IDE_COMMAND_SET_DEVICE_PARAMETERS       ((UCHAR)0x91)
#define IDE_COMMAND_PACKET                      ((UCHAR)0xA0)
#define IDE_COMMAND_IDENTIFY_PACKET_DEVICE      ((UCHAR)0xA1)
#define IDE_COMMAND_READ_MULTIPLE               ((UCHAR)0xC4)
#define IDE_COMMAND_WRITE_MULTIPLE              ((UCHAR)0xC5)
#define IDE_COMMAND_SET_MULTIPLE_MODE           ((UCHAR)0xC6)
#define IDE_COMMAND_READ_DMA                    ((UCHAR)0xC8)
#define IDE_COMMAND_WRITE_DMA                   ((UCHAR)0xCA)
#define IDE_COMMAND_STANDBY_IMMEDIATE           ((UCHAR)0xE0)
#define IDE_COMMAND_FLUSH_CACHE                 ((UCHAR)0xE7)
#define IDE_COMMAND_IDENTIFY_DEVICE             ((UCHAR)0xEC)
#define IDE_COMMAND_SET_FEATURES                ((UCHAR)0xEF)
#define IDE_COMMAND_SECURITY_SET_PASSWORD       ((UCHAR)0xF1)
#define IDE_COMMAND_SECURITY_UNLOCK             ((UCHAR)0xF2)
#define IDE_COMMAND_SECURITY_DISABLE_PASSWORD   ((UCHAR)0xF6)

//
// Define the IDE feature values for an ATA device.
//

#define IDE_FEATURE_SET_TRANSFER_MODE           ((UCHAR)0x03)

//
// Define the IDE feature flags for an ATAPI device.
//

#define IDE_FEATURE_DMA                         ((UCHAR)0x01)
#define IDE_FEATURE_OVL                         ((UCHAR)0x02)

//
// Define the IDE interrupt reason flags for an ATAPI device.
//

#define IDE_INTERRUPT_REASON_CD                 ((UCHAR)0x01)
#define IDE_INTERRUPT_REASON_IO                 ((UCHAR)0x02)

//
// Define the data transfer values for an ATA device.
//

#define IDE_ATA_SECTOR_SHIFT                    9
#define IDE_ATA_SECTOR_SIZE                     (1 << IDE_ATA_SECTOR_SHIFT)
#define IDE_ATA_SECTOR_MASK                     (IDE_ATA_SECTOR_SIZE - 1)
#define IDE_ATA_MAXIMUM_TRANSFER_SECTORS        256
#define IDE_ATA_MAXIMUM_TRANSFER_BYTES          (IDE_ATA_MAXIMUM_TRANSFER_SECTORS * IDE_ATA_SECTOR_SIZE)
#define IDE_ATA_MAXIMUM_TRANSFER_PAGES          (IDE_ATA_MAXIMUM_TRANSFER_BYTES >> PAGE_SHIFT)

//
// Define the data transfer values for an ATAPI device.
//

#define IDE_ATAPI_CD_SECTOR_SHIFT               11
#define IDE_ATAPI_CD_SECTOR_SIZE                (1 << IDE_ATAPI_CD_SECTOR_SHIFT)
#define IDE_ATAPI_CD_SECTOR_MASK                (IDE_ATAPI_CD_SECTOR_SIZE - 1)
#define IDE_ATAPI_CD_MAXIMUM_TRANSFER_SECTORS   64
#define IDE_ATAPI_RAW_CD_SECTOR_SIZE            2352
#define IDE_ATAPI_MAXIMUM_TRANSFER_BYTES        (IDE_ATAPI_CD_MAXIMUM_TRANSFER_SECTORS * IDE_ATAPI_CD_SECTOR_SIZE)
#define IDE_ATAPI_MAXIMUM_TRANSFER_PAGES        (IDE_ATAPI_MAXIMUM_TRANSFER_BYTES >> PAGE_SHIFT)

//
// Define the transfer modes for the set transfer mode feature.
//

#define IDE_TRANSFER_MODE_PIO                   ((UCHAR)0x00)
#define IDE_TRANSFER_MODE_PIO_NO_IORDY          ((UCHAR)0x01)
#define IDE_TRANSFER_MODE_PIO_MODE_3            ((UCHAR)0x0B)
#define IDE_TRANSFER_MODE_PIO_MODE_4            ((UCHAR)0x0C)
#define IDE_TRANSFER_MODE_MWDMA_MODE_0          ((UCHAR)0x20)
#define IDE_TRANSFER_MODE_MWDMA_MODE_1          ((UCHAR)0x21)
#define IDE_TRANSFER_MODE_MWDMA_MODE_2          ((UCHAR)0x22)
#define IDE_TRANSFER_MODE_UDMA_MODE_0           ((UCHAR)0x40)
#define IDE_TRANSFER_MODE_UDMA_MODE_1           ((UCHAR)0x41)
#define IDE_TRANSFER_MODE_UDMA_MODE_2           ((UCHAR)0x42)
#define IDE_TRANSFER_MODE_UDMA_MODE_3           ((UCHAR)0x43)
#define IDE_TRANSFER_MODE_UDMA_MODE_4           ((UCHAR)0x44)
#define IDE_TRANSFER_MODE_UDMA_MODE_5           ((UCHAR)0x45)

//
// Define the number of bytes in a password buffer for an ATA device.
//

#define IDE_ATA_PASSWORD_LENGTH                 32

//
// Define the data buffer alignment requirements for a DMA bus master operation.
//

#define IDE_ALIGNMENT_REQUIREMENT               FILE_WORD_ALIGNMENT

//
// Macros to read and write from the PCI bus master interface.
//

#define IdexReadBusMasterCommandPort(channel) \
    IdexReadPortUchar(IDE_PCI_BUS_MASTER_BMICP)

#define IdexWriteBusMasterCommandPort(data) \
    IdexWritePortUchar(IDE_PCI_BUS_MASTER_BMICP, (data))

#define IdexReadBusMasterStatusPort() \
    IdexReadPortUchar(IDE_PCI_BUS_MASTER_BMISP)

#define IdexWriteBusMasterStatusPort(data) \
    IdexWritePortUchar(IDE_PCI_BUS_MASTER_BMISP, (data))

#define IdexReadBusMasterDescriptorTablePort() \
    IdexReadPortUlong(IDE_PCI_BUS_MASTER_BMIDP)

#define IdexWriteBusMasterDescriptorTablePort(data) \
    IdexWritePortUlong(IDE_PCI_BUS_MASTER_BMIDP, (data))

//
// Define the bus master interface command register flags.
//

#define IDE_BUS_MASTER_COMMAND_START            0x01
#define IDE_BUS_MASTER_COMMAND_READ             0x08

//
// Define the bus master interface status register flags.
//

#define IDE_BUS_MASTER_STATUS_ACTIVE            0x01
#define IDE_BUS_MASTER_STATUS_ERROR             0x02
#define IDE_BUS_MASTER_STATUS_INTERRUPT         0x04

//
// Structure that's passed to the bus master interface to indicate the location
// of the data buffer's pages.
//

typedef struct _IDE_PCI_PHYSICAL_REGION_DESCRIPTOR {
    ULONG PhysicalAddress;
    union {
        ULONG AsULong;
        struct {
            ULONG AsUShort : 16;
            ULONG Reserved : 15;
            ULONG EndOfTable : 1;
        } b;
    } ByteCount;
} IDE_PCI_PHYSICAL_REGION_DESCRIPTOR, *PIDE_PCI_PHYSICAL_REGION_DESCRIPTOR;

//
// Routines that act on the driver device.
//

NTSTATUS
IdexDriverIrpReturnSuccess(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

//
// Routines that act on the channel device.
//

BOOLEAN
FASTCALL
IdexChannelSpinWhileBusy(
    OUT PUCHAR IdeStatus
    );

BOOLEAN
FASTCALL
IdexChannelSpinWhileBusyAndNotDrq(
    OUT PUCHAR IdeStatus
    );

VOID
IdexChannelSetTimerPeriod(
    IN LONG Period
    );

VOID
FASTCALL
IdexChannelPrepareBufferTransfer(
    IN PUCHAR Buffer,
    IN ULONG ByteCount
    );

VOID
IdexChannelPrepareScatterGatherTransfer(
    IN PFILE_SEGMENT_ELEMENT SegmentArray,
    IN ULONG SegmentByteOffset,
    IN ULONG ByteCount
    );

VOID
IdexChannelRestartCurrentPacket(
    VOID
    );

VOID
IdexChannelAbortCurrentPacket(
    VOID
    );

VOID
FASTCALL
IdexChannelInvalidParameterRequest(
    IN PIRP Irp
    );

VOID
IdexChannelStartIdePassThrough(
    IN PIRP Irp,
    IN UCHAR TargetDevice,
    IN PIDE_RESET_DEVICE_ROUTINE ResetDeviceRoutine
    );

NTSTATUS
IdexChannelIdentifyDevice(
    IN UCHAR TargetDevice,
    IN UCHAR IdentifyCommand,
    OUT PIDE_IDENTIFY_DATA IdentifyData
    );

NTSTATUS
IdexChannelSetTransferMode(
    IN UCHAR TargetDevice,
    IN UCHAR TransferMode
    );

NTSTATUS
IdexChannelIssueImmediateCommand(
    IN UCHAR TargetDevice,
    IN UCHAR IdeCommand
    );

VOID
IdexChannelCreate(
    VOID
    );

//
// Routines that act on the disk device.
//

VOID
IdexDiskCreateQuick(
    VOID
    );

VOID
IdexDiskCreate(
    VOID
    );

//
// Routines that act on the CD-ROM device.
//

BOOLEAN
IdexCdRomPollResetComplete(
    VOID
    );

VOID
IdexCdRomCreateQuick(
    VOID
    );

VOID
IdexCdRomCreate(
    VOID
    );

//
// Routines that act on the media board device.
//

DECLSPEC_NORETURN
VOID
IdexMediaBoardFatalError(
    IN ULONG ErrorCode
    );

VOID
IdexMediaBoardCreateQuick(
    VOID
    );

VOID
IdexMediaBoardCreate(
    VOID
    );

//
// Routines that perform I/O port operations.
//

#pragma intrinsic(_inp,_inpw,_inpd,_outp,_outpw,_outpd)

#define IdexReadPortUchar(port)             ((UCHAR)_inp((USHORT)(port)))
#define IdexReadPortUshort(port)            ((USHORT)_inpw((USHORT)(port)))
#define IdexReadPortUlong(port)             ((ULONG)_inpd((USHORT)(port)))
#define IdexWritePortUchar(port, data)      (_outp((USHORT)(port),(UCHAR)(data)))
#define IdexWritePortUshort(port, data)     (_outpw((USHORT)(port),(USHORT)(data)))
#define IdexWritePortUlong(port, data)      (_outpd((USHORT)(port),(ULONG)(data)))

#define IdexReadPortBufferUshort(port, buffer, count) \
    READ_PORT_BUFFER_USHORT((PUSHORT)(port), buffer, count)
#define IdexWritePortBufferUshort(port, buffer, count) \
    WRITE_PORT_BUFFER_USHORT((PUSHORT)(port), buffer, count)
#define IdexReadPortBufferUlong(port, buffer, count) \
    READ_PORT_BUFFER_ULONG((PULONG)(port), buffer, count)
#define IdexWritePortBufferUlong(port, buffer, count) \
    WRITE_PORT_BUFFER_ULONG((PULONG)(port), buffer, count)

//
// Macros to swap the byte order of a USHORT or ULONG at compile time.
//

#define IdexConstantUshortByteSwap(ushort) \
    ((((USHORT)ushort) >> 8) + ((((USHORT)ushort) & 0x00FF) << 8))

#define IdexConstantUlongByteSwap(ulong) \
    ((((ULONG)ulong) >> 24) + ((((ULONG)ulong) & 0x00FF0000) >> 8) + \
    ((((ULONG)ulong) & 0x0000FF00) << 8) + ((((ULONG)ulong) & 0x000000FF) << 24))

//
// Miscellaneous routines.
//

DECLSPEC_NORETURN
VOID
IdexBugCheckWorker(
    IN ULONG FileAndLineCode,
    IN ULONG_PTR BugCheckParameter1
    );

#define IdexBugCheck(fileid, parameter1) \
    IdexBugCheckWorker((((ULONG)fileid) | __LINE__), ((ULONG_PTR)parameter1))

//
// External symbols.
//

extern DRIVER_OBJECT IdexDiskDriverObject;
extern OBJECT_TYPE IdexDiskDirectoryObjectType;
extern DRIVER_OBJECT IdexCdRomDriverObject;
extern DRIVER_OBJECT IdexMediaBoardDriverObject;

#include <poppack.h>

#endif  // IDEX
