/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    gdformat.h

Abstract:

    This module defines the on-disk structure of the GDF file system.

--*/

#ifndef _GDFORMAT_
#define _GDFORMAT_

#include <pshpack1.h>

//
// Define the signature bytes that are at the head and tail of the volume
// descriptor sector.
//

#define GDF_VOLUME_DESCRIPTOR_SIGNATURE             "MICROSOFT*XBOX*MEDIA"
#define GDF_VOLUME_DESCRIPTOR_SIGNATURE_LENGTH      20

//
// Define the logical sector number of the volume descriptor sector.
//
// For ISO-9960 images, logical sector 16 is the start of the volume descriptor
// sequence.  For UDF images, logical sector 256 is a possible sector for the
// anchor volume descriptor pointer.  The following sector number is chosen
// such that its possible to coexist with other file systems for demo discs
// recorded on standard media.
//

#define GDF_VOLUME_DESCRIPTOR_SECTOR                32

//
// Define the structure of the GDF volume descriptor.
//
// The head and tail signature shall be set to GDF_VOLUME_DESCRIPTOR_SIGNATURE.
//
// The root directory starting sector is an absolute sector number relative to
// the start of the media.  The root directory file size is the number of bytes
// allocated to the root directory.
//
// The time stamp is the volume creation date stored as a the number of 100
// nanosecond intervals since January 1, 1601 UTC (see the documentation for
// FILETIME in the Xbox XDK or Platform SDK).  This time stamp is used as the
// time stamp for all files on the volume and is visible to the title through
// Win32 APIs such as FindFirstFile or GetFileAttributesEx.
//
// The reserved bytes shall be set to zero.
//

typedef struct _GDF_VOLUME_DESCRIPTOR {
    UCHAR HeadSignature[GDF_VOLUME_DESCRIPTOR_SIGNATURE_LENGTH];
    ULONG RootDirectoryFirstSector;
    ULONG RootDirectoryFileSize;
    LARGE_INTEGER TimeStamp;
    UCHAR Reserved[1992];
    UCHAR TailSignature[GDF_VOLUME_DESCRIPTOR_SIGNATURE_LENGTH];
} GDF_VOLUME_DESCRIPTOR, *PGDF_VOLUME_DESCRIPTOR;

//
// Define the structure of a GDF directory entry.
//
// Directory entries are stored as a binary tree with the directory entry at
// byte offset zero serving as the root node of the tree.  When searching for a
// file name in a directory, the file system will do a case insensitive
// comparison of the target name to the name stored in the directory entry.  If
// the names are equal, then the target directory entry has been found.  If the
// target name is less than the name stored in the current directory entry, then
// the left entry index is used to find the next directory entry.  If the target
// name is greater than the name stored in the current directory entry, then the
// right entry index is used to find the next directory entry.
//
// The left and right entry indexes are multiplied by four to obtain the byte
// offset of the next directory entry.  If the left or right entry indexes are
// zero, then there are no more directory entries that will satisfy the target
// name search, so the name search is stopped.  The byte offsets of the
// directory entries obtained by following the left or right entry indexes must
// be greater than the byte offset of the current directory entry; no entry
// indexes can point to a directory entry at a byte offset less than or equal to
// the current directory entry.
//
// Given that the left and right entry indexes are a 16-bit quantity and they
// are multiplied by four to obtain a byte offset, the maximum size for a GDF
// directory stream is 256K.
//
// No directory entry shall span a 2048 byte CD sector.  If a directory stream
// sector cannot be filled with directory entries, then all unused bytes shall
// be filled with GDF_DIRECTORY_STREAM_PADDING.
//
// The starting sector is an absolute sector number relative to the start of the
// media.  The file size is the number of bytes allocated to the file.  All GDF
// files are allocated as a single extent.  The file size shall be filled in for
// files and directories.
//
// The file attributes shall consist of the Win32 file attributes
// (FILE_ATTRIBUTE_*), but the only file attribute that the file system will pay
// attention to is FILE_ATTRIBUTE_DIRECTORY.
//
// The file name length is the number of bytes in the following variable length
// file name field.  The file name length shall not be zero.
//
// The file name shall be 1 to 255 characters from the first 256 entries of the
// 16-bit Unicode table.  Note that this is not equivalent to ASCII or to any of
// the Windows code pages.
//

typedef struct _GDF_DIRECTORY_ENTRY {
    USHORT LeftEntryIndex;
    USHORT RightEntryIndex;
    ULONG FirstSector;
    ULONG FileSize;
    UCHAR FileAttributes;
    UCHAR FileNameLength;
    UCHAR FileName[1];
} GDF_DIRECTORY_ENTRY, *PGDF_DIRECTORY_ENTRY;

//
// Define the byte used to fill the unused space between the last directory
// entry of a sector and the end of the sector.  When enumerating the contents
// of a directory, the file system will check for a "LeftEntryIndex ==
// RightEntryIndex == ((USHORT)-1)" in order to know that the rest of the
// directory stream sector is padding.
//

#define GDF_DIRECTORY_STREAM_PADDING                ((UCHAR)-1)

#include <poppack.h>

#endif  // GDFORMAT
