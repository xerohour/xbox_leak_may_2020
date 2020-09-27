/*++

Copyright (c) 1997-1999 Microsoft Corporation

Module Name:

    parttype.h

Abstract:

    Header file for routines used to determine the correct
    partition type to use for a partition.

Author:

    Ted Miller (tedm) 5 Feb 1997

Revision History:

--*/

#if _MSC_VER > 1000
#pragma once
#endif


//
// Define classes of partitions meaningful to partition type APIs.
//
typedef enum {
    GenPartitionClassExtended,      // container partition (type 5 or f)
    GenPartitionClassFat12Or16,     // fat (types 1,4,6,e)
    GenPartitionClassFat32,         // fat32 (types b,c)
    GenPartitionClassNonFat,        // type 7
    GenPartitionClassMax
} GenPartitionClass;

//
// Flags for partition type APIs.
//
#define GENPARTTYPE_DISALLOW_XINT13     0x00000002
#define GENPARTTYPE_FORCE_XINT13        0x00000004


#ifdef __cplusplus
extern "C" {
#endif

//
// Routines.
//
NTSTATUS
GeneratePartitionType(
    IN  LPCWSTR NtDevicePath,               OPTIONAL
    IN  HANDLE DeviceHandle,                OPTIONAL
    IN  ULONGLONG StartSector,
    IN  ULONGLONG SectorCount,
    IN  GenPartitionClass PartitionClass,
    IN  ULONG Flags,
    IN  ULONG Reserved,
    OUT PUCHAR Type
    );

NTSTATUS
RegeneratePartitionType(
    IN  LPCWSTR NtPartitionPath,            OPTIONAL
    IN  HANDLE PartitionHandle,             OPTIONAL
    IN  GenPartitionClass PartitionClass,
    IN  ULONG Flags,
    IN  ULONG Reserved,
    OUT PUCHAR Type
    );

#ifdef __cplusplus
}
#endif
