/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    fat.h

Abstract:

    This module defines the on-disk structure of the FAT file system.

--*/

#ifndef _FAT_
#define _FAT_

#include <pshpack1.h>

//
// Define the length of the volume name stored in the FAT volume metadata block.
// This includes one character for storing a null terminator.  This should be
// equal to MAX_MUNAME (from xbox.h).
//

#define FAT_VOLUME_NAME_LENGTH          32

//
// Defines the length of region reserved for online related data.
//

#define FAT_ONLINE_DATA_LENGTH          2048

//
// Define the structure of the FAT volume metadata block.  The metadata block
// is at physical byte offset zero of the volume.  The metadata block is either
// 4096 bytes or one device sector in length, whichever is greater.
//

typedef struct _FAT_VOLUME_METADATA {
    ULONG Signature;
    ULONG SerialNumber;
    ULONG SectorsPerCluster;
    ULONG RootDirFirstCluster;
    WCHAR VolumeName[FAT_VOLUME_NAME_LENGTH];
    UCHAR OnlineData[FAT_ONLINE_DATA_LENGTH];
    // Unused space in the block is filled with 0xFF bytes.
} FAT_VOLUME_METADATA, *PFAT_VOLUME_METADATA;

//
// Define the volume signature value contained in the volume metadata block.
//

#define FAT_VOLUME_SIGNATURE            'XTAF'

//
// The first entry of the FAT is reserved in order to catch media errors that
// cause part of the FAT to be zeroed out.
//

#define FAT_RESERVED_FAT_ENTRIES        1

//
// The following constants the are the valid FAT index values.
//

#define FAT_CLUSTER_AVAILABLE           (ULONG)0x00000000
#define FAT_CLUSTER_RESERVED            (ULONG)0xfffffff0
#define FAT_CLUSTER_BAD                 (ULONG)0xfffffff7
#define FAT_CLUSTER_MEDIA               (ULONG)0xfffffff8
#define FAT_CLUSTER_LAST                (ULONG)0xffffffff

#define FAT_CLUSTER16_AVAILABLE         (USHORT)0x0000
#define FAT_CLUSTER16_RESERVED          (USHORT)0xfff0
#define FAT_CLUSTER16_BAD               (USHORT)0xfff7
#define FAT_CLUSTER16_MEDIA             (USHORT)0xfff8
#define FAT_CLUSTER16_LAST              (USHORT)0xffff

#define FAT_CLUSTER_NULL                (ULONG)0

//
// FAT files have the following time/date structures.
//

typedef union _FAT_TIME_STAMP {
    struct {
        USHORT DoubleSeconds : 5;
        USHORT Minute        : 6;
        USHORT Hour          : 5;
        USHORT Day           : 5;
        USHORT Month         : 4;
        USHORT Year          : 7; // Relative to 2000
    };
    ULONG AsULONG;
} FAT_TIME_STAMP;
typedef FAT_TIME_STAMP *PFAT_TIME_STAMP;

//
// FAT files have the following name length.
//

#define FAT_FILE_NAME_LENGTH            42

//
// FAT paths have the following name limit.
//

#define FAT_PATH_NAME_LIMIT             250

//
// The directory entry record exists for every file/directory on the
// disk except for the root directory.
//

typedef struct _DIRENT {
    UCHAR FileNameLength;                           // offset 0x00
    UCHAR FileAttributes;                           // offset 0x01
    UCHAR FileName[FAT_FILE_NAME_LENGTH];           // offset 0x02
    ULONG FirstCluster;                             // offset 0x2C
    ULONG FileSize;                                 // offset 0x30
    FAT_TIME_STAMP CreationTime;                    // offset 0x34
    FAT_TIME_STAMP LastWriteTime;                   // offset 0x38
    FAT_TIME_STAMP LastAccessTime;                  // offset 0x3C
} DIRENT, *PDIRENT;                                 // sizeof = 64

//
// The first byte of a dirent describes the dirent.
//

#define FAT_DIRENT_NEVER_USED           0x00
#define FAT_DIRENT_DELETED              0xE5
#define FAT_DIRENT_NEVER_USED2          0xFF

//
// FAT directories have the following file size restriction.  The restriction
// helps to avoid circular FAT corruption (directories don't track the file
// size in their directory entry) and to ensure that titles don't create a
// directory structure that's slow to search.
//

#define FAT_MAXIMUM_DIRECTORY_FILE_SIZE (4096 * sizeof(DIRENT))

#include <poppack.h>

#endif // _FAT_
