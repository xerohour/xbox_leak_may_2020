/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    xdisk.h

Abstract:

    This module defines the disk configuration for an XBOX hard disk.

--*/

#ifndef _XDISK_
#define _XDISK_

//
// Define the size of a sector on an XBOX hard disk.
//

#define XDISK_SECTOR_SIZE                   512

//
// Define the number of unpartitioned sectors reserved at the start of the disk.
// These sectors are used for various purposes such as storing data that should
// not be visible from the file system.
//

#define XDISK_UNPARTITIONED_SECTORS         1024
#define XDISK_UNPARTITIONED_SIZE            (XDISK_UNPARTITION_SECTORS * XDISK_SECTOR_SIZE)

//
// Define the number of sectors in the shell partition (partition 2).
//

#define XDISK_SHELL_PARTITION_SECTORS       1024000
#define XDISK_SHELL_PARTITION_SIZE          (XDISK_SHELL_PARTITION_SECTORS * XDISK_SECTOR_SIZE)

//
// Define the number of sectors in each cache partition (partitions 3 to N).
//

#define XDISK_CACHE_PARTITION_SECTORS       1536000
#define XDISK_CACHE_PARTITION_SIZE          (XDISK_CACHE_PARTITION_SECTORS * XDISK_SECTOR_SIZE)

//
// Define the formula used to compute the number of cache partitions given the
// number of user addressable sectors for the disk.
//

#define XDISK_NUMBER_OF_CACHE_PARTITIONS(UserAddressableSectors) \
    ((((UserAddressableSectors) / 200 * 73) - \
    (XDISK_UNPARTITIONED_SECTORS + XDISK_SHELL_PARTITION_SECTORS)) / \
    XDISK_CACHE_PARTITION_SECTORS)

//
// Define the static partition numbers.
//

#define XDISK_DATA_PARTITION                1
#define XDISK_SHELL_PARTITION               2
#define XDISK_FIRST_CACHE_PARTITION         3

//
// Define the static number of sectors allowed for an XBOX hard disk.
//

#define XDISK_FIXED_SECTOR_COUNT            15633072

#endif  // XDISK
