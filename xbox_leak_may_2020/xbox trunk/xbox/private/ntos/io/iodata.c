/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    iodata.c

Abstract:

    This module contains the global read/write data for the I/O system.

Author:

    Darryl E. Havens (darrylh) April 27, 1989

Revision History:


--*/

#include "iop.h"

//
// The following are the global pointers for the Object Type Descriptors that
// are created when each of the I/O specific object types are created.
//

DECLSPEC_RDATA OBJECT_TYPE IoCompletionObjectType = {
    ExAllocatePoolWithTag,
    ExFreePool,
    NULL,
    IopDeleteIoCompletion,
    NULL,
    &ObpDefaultObject,
    'pmoC'
};

DECLSPEC_RDATA OBJECT_TYPE IoDeviceObjectType = {
    ExAllocatePoolWithTag,
    ExFreePool,
    NULL,
    NULL,
    IoParseDevice,
    &ObpDefaultObject,
    'iveD'
};

DECLSPEC_RDATA OBJECT_TYPE IoFileObjectType = {
    ExAllocatePoolWithTag,
    ExFreePool,
    IopCloseFile,
    IopDeleteFile,
    IopParseFile,
    (PVOID)FIELD_OFFSET(FILE_OBJECT, Event.Header),
    'eliF'
};

//
// The following array specifies the minimum length of the FileInformation
// buffer for an NtQueryInformationFile service.
//
// WARNING:  This array depends on the order of the values in the
//           FileInformationClass enumerated type.  Note that the
//           enumerated type is one-based and the array is zero-based.
//

const UCHAR IopQueryOperationLength[] =
          {
            0,
            0,                                         //  1 FileDirectoryInformation
            0,                                         //  2 FileFullDirectoryInformation
            0,                                         //  3 FileBothDirectoryInformation
            sizeof( FILE_BASIC_INFORMATION ),          //  4 FileBasicInformation
            sizeof( FILE_STANDARD_INFORMATION ),       //  5 FileStandardInformation
            sizeof( FILE_INTERNAL_INFORMATION ),       //  6 FileInternalInformation
            sizeof( FILE_EA_INFORMATION ),             //  7 FileEaInformation
            0,                                         //  8 FileAccessInformation
            sizeof( FILE_NAME_INFORMATION ),           //  9 FileNameInformation
            0,                                         // 10 FileRenameInformation
            0,                                         // 11 FileLinkInformation
            0,                                         // 12 FileNamesInformation
            0,                                         // 13 FileDispositionInformation
            sizeof( FILE_POSITION_INFORMATION ),       // 14 FilePositionInformation
            0,                                         // 15 FileFullEaInformation
            sizeof( FILE_MODE_INFORMATION ),           // 16 FileModeInformation
            sizeof( FILE_ALIGNMENT_INFORMATION ),      // 17 FileAlignmentInformation
            sizeof( FILE_ALL_INFORMATION ),            // 18 FileAllInformation
            0,                                         // 19 FileAllocationInformation
            0,                                         // 20 FileEndOfFileInformation
            sizeof( FILE_NAME_INFORMATION ),           // 21 FileAlternateNameInformation
            sizeof( FILE_STREAM_INFORMATION ),         // 22 FileStreamInformation
            0,                                         // 23 FilePipeInformation
            0,                                         // 24 FilePipeLocalInformation
            0,                                         // 25 FilePipeRemoteInformation
            0,                                         // 26 FileMailslotQueryInformation
            0,                                         // 27 FileMailslotSetInformation
            0,                                         // 28 FileCompressionInformation
            0,                                         // 29 FileObjectIdInformation
            0,                                         // 30 FileCompletionInformation
            0,                                         // 31 FileMoveClusterInformation
            0,                                         // 32 FileQuotaInformation
            0,                                         // 33 FileReparsePointInformation
            sizeof( FILE_NETWORK_OPEN_INFORMATION),    // 34 FileNetworkOpenInformation
            sizeof( FILE_ATTRIBUTE_TAG_INFORMATION),   // 35 FileAttributeTagInformation
            0,                                         // 36 FileTrackingInformation
            0xff                                       //    FileMaximumInformation
          };

//
// The following array specifies the minimum length of the FileInformation
// buffer for an NtSetInformationFile service.
//
// WARNING:  This array depends on the order of the values in the
//           FileInformationClass enumerated type.  Note that the
//           enumerated type is one-based and the array is zero-based.
//

const UCHAR IopSetOperationLength[] =
          {
            0,
            0,                                       //  1 FileDirectoryInformation
            0,                                       //  2 FileFullDirectoryInformation
            0,                                       //  3 FileBothDirectoryInformation
            sizeof( FILE_BASIC_INFORMATION ),        //  4 FileBasicInformation
            0,                                       //  5 FileStandardInformation
            0,                                       //  6 FileInternalInformation
            0,                                       //  7 FileEaInformation
            0,                                       //  8 FileAccessInformation
            0,                                       //  9 FileNameInformation
            sizeof( FILE_RENAME_INFORMATION ),       // 10 FileRenameInformation
            sizeof( FILE_LINK_INFORMATION ),         // 11 FileLinkInformation
            0,                                       // 12 FileNamesInformation
            sizeof( FILE_DISPOSITION_INFORMATION ),  // 13 FileDispositionInformation
            sizeof( FILE_POSITION_INFORMATION ),     // 14 FilePositionInformation
            0,                                       // 15 FileFullEaInformation
            sizeof( FILE_MODE_INFORMATION ),         // 16 FileModeInformation
            0,                                       // 17 FileAlignmentInformation
            0,                                       // 18 FileAllInformation
            sizeof( FILE_ALLOCATION_INFORMATION ),   // 19 FileAllocationInformation
            sizeof( FILE_END_OF_FILE_INFORMATION ),  // 20 FileEndOfFileInformation
            0,                                       // 21 FileAlternateNameInformation
            0,                                       // 22 FileStreamInformation
            0,                                       // 23 FilePipeInformation
            0,                                       // 24 FilePipeLocalInformation
            0,                                       // 25 FilePipeRemoteInformation
            0,                                       // 26 FileMailslotQueryInformation
            0,                                       // 27 FileMailslotSetInformation
            0,                                       // 28 FileCompressionInformation
            0,                                       // 29 FileObjectIdInformation
            sizeof( FILE_COMPLETION_INFORMATION ),   // 30 FileCompletionInformation
            sizeof( FILE_MOVE_CLUSTER_INFORMATION ), // 31 FileMoveClusterInformation
            0,                                       // 32 FileQuotaInformation
            0,                                       // 33 FileReparsePointInformation
            0,                                       // 34 FileNetworkOpenInformation
            0,                                       // 35 FileAttributeTagInformation
            0,                                       // 36 FileTrackingInformation
            0xff                                     //    FileMaximumInformation
          };

//
// The following array specifies the required access mask for the caller to
// access information in an NtQueryXxxFile service.
//
// WARNING:  This array depends on the order of the values in the
//           FileInformationClass enumerated type.  Note that the
//           enumerated type is one-based and the array is zero-based.
//

const ULONG IopQueryOperationAccess[] =
         {
            0,
            0,                    //  1 FileDirectoryInformation
            0,                    //  2 FileFullDirectoryInformation
            0,                    //  3 FileBothDirectoryInformation
            FILE_READ_ATTRIBUTES, //  4 FileBasicInformation
            0,                    //  5 FileStandardInformation
            0,                    //  6 FileInternalInformation
            0,                    //  7 FileEaInformation
            0,                    //  8 FileAccessInformation
            0,                    //  9 FileNameInformation
            0,                    // 10 FileRenameInformation
            0,                    // 11 FileLinkInformation
            0,                    // 12 FileNamesInformation
            0,                    // 13 FileDispositionInformation
            0,                    // 14 FilePositionInformation
            FILE_READ_EA,         // 15 FileFullEaInformation
            0,                    // 16 FileModeInformation
            0,                    // 17 FileAlignmentInformation
            FILE_READ_ATTRIBUTES, // 18 FileAllInformation
            0,                    // 19 FileAllocationInformation
            0,                    // 20 FileEndOfFileInformation
            0,                    // 21 FileAlternateNameInformation
            0,                    // 22 FileStreamInformation
            FILE_READ_ATTRIBUTES, // 23 FilePipeInformation
            FILE_READ_ATTRIBUTES, // 24 FilePipeLocalInformation
            FILE_READ_ATTRIBUTES, // 25 FilePipeRemoteInformation
            0,                    // 26 FileMailslotQueryInformation
            0,                    // 27 FileMailslotSetInformation
            0,                    // 28 FileCompressionInformation
            0,                    // 29 FileObjectIdInformation
            0,                    // 30 FileCompletionInformation
            0,                    // 31 FileMoveClusterInformation
            0,                    // 32 FileQuotaInformation
            0,                    // 33 FileReparsePointInformation
            FILE_READ_ATTRIBUTES, // 34 FileNetworkOpenInformation
            FILE_READ_ATTRIBUTES, // 35 FileAttributeTagInformation
            0,                    // 36 FileTrackingInformation
            0xffffffff            //    FileMaximumInformation
          };

//
// The following array specifies the required access mask for the caller to
// access information in an NtSetXxxFile service.
//
// WARNING:  This array depends on the order of the values in the
//           FILE_INFORMATION_CLASS enumerated type.  Note that the
//           enumerated type is one-based and the array is zero-based.
//

const ULONG IopSetOperationAccess[] =
         {
            0,
            0,                     //  1 FileDirectoryInformation
            0,                     //  2 FileFullDirectoryInformation
            0,                     //  3 FileBothDirectoryInformation
            FILE_WRITE_ATTRIBUTES, //  4 FileBasicInformation
            0,                     //  5 FileStandardInformation
            0,                     //  6 FileInternalInformation
            0,                     //  7 FileEaInformation
            0,                     //  8 FileAccessInformation
            0,                     //  9 FileNameInformation
            DELETE,                // 10 FileRenameInformation
            0,                     // 11 FileLinkInformation
            0,                     // 12 FileNamesInformation
            DELETE,                // 13 FileDispositionInformation
            0,                     // 14 FilePositionInformation
            FILE_WRITE_EA,         // 15 FileFullEaInformation
            0,                     // 16 FileModeInformation
            0,                     // 17 FileAlignmentInformation
            0,                     // 18 FileAllInformation
            FILE_WRITE_DATA,       // 19 FileAllocationInformation
            FILE_WRITE_DATA,       // 20 FileEndOfFileInformation
            0,                     // 21 FileAlternateNameInformation
            0,                     // 22 FileStreamInformation
            FILE_WRITE_ATTRIBUTES, // 23 FilePipeInformation
            0,                     // 24 FilePipeLocalInformation
            FILE_WRITE_ATTRIBUTES, // 25 FilePipeRemoteInformation
            0,                     // 26 FileMailslotQueryInformation
            0,                     // 27 FileMailslotSetInformation
            0,                     // 28 FileCompressionInformation
            0,                     // 29 FileObjectIdInformation
            0,                     // 30 FileCompletionInformation
            FILE_WRITE_DATA,       // 31 FileMoveClusterInformation
            0,                     // 32 FileQuotaInformation
            0,                     // 33 FileReparsePointInformation
            0,                     // 34 FileNetworkOpenInformation
            0,                     // 35 FileAttributeTagInformation
            FILE_WRITE_DATA,       // 36 FileTrackingInformation
            0xffffffff             //    FileMaximumInformation
          };

//
// The following array specifies the minimum length of the FsInformation
// buffer for an NtQueryVolumeInformation service.
//
// WARNING:  This array depends on the order of the values in the
//           FS_INFORMATION_CLASS enumerated type.  Note that the
//           enumerated type is one-based and the array is zero-based.
//

const UCHAR IopQueryFsOperationLength[] =
          {
            0,
            sizeof( FILE_FS_VOLUME_INFORMATION ),    // 1 FileFsVolumeInformation
            0,                                       // 2 FileFsLabelInformation
            sizeof( FILE_FS_SIZE_INFORMATION ),      // 3 FileFsSizeInformation
            sizeof( FILE_FS_DEVICE_INFORMATION ),    // 4 FileFsDeviceInformation
            sizeof( FILE_FS_ATTRIBUTE_INFORMATION ), // 5 FileFsAttributeInformation
            sizeof( FILE_FS_CONTROL_INFORMATION ),   // 6 FileFsControlInformation
            sizeof( FILE_FS_FULL_SIZE_INFORMATION ), // 7 FileFsFullSizeInformation
            sizeof( FILE_FS_OBJECTID_INFORMATION ),  // 8 FileFsObjectIdInformation
            0xff                                     //   FileFsMaximumInformation
          };

//
// The following array specifies the required access mask for the caller to
// access information in an NtQueryVolumeInformation service.
//
// WARNING:  This array depends on the order of the values in the
//           FS_INFORMATION_CLASS enumerated type.  Note that the
//           enumerated type is one-based and the array is zero-based.
//

const ULONG IopQueryFsOperationAccess[] =
         {
            0,
            0,              // 1 FileFsVolumeInformation [any access to file or volume]
            0,              // 2 FileFsLabelInformation [query is invalid]
            0,              // 3 FileFsSizeInformation [any access to file or volume]
            0,              // 4 FileFsDeviceInformation [any access to file or volume]
            0,              // 5 FileFsAttributeInformation [any access to file or vol]
            FILE_READ_DATA, // 6 FileFsControlInformation [vol read access]
            0,              // 7 FileFsFullSizeInformation [any access to file or volume]
            0,              // 8 FileFsObjectIdInformation [any access to file or volume]
            0xffffffff      //   FileFsMaximumInformation
          };

DECLSPEC_RDATA GENERIC_MAPPING IopFileMapping = {
    STANDARD_RIGHTS_READ |
        FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA | SYNCHRONIZE,
    STANDARD_RIGHTS_WRITE |
        FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | FILE_APPEND_DATA | SYNCHRONIZE,
    STANDARD_RIGHTS_EXECUTE |
        SYNCHRONIZE | FILE_READ_ATTRIBUTES | FILE_EXECUTE,
    FILE_ALL_ACCESS
};

ULONG IoPendingMustCompletePackets;
