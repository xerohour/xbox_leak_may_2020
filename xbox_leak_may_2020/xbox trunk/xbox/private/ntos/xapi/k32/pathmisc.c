/*++

Copyright (c) 1990-2002  Microsoft Corporation

Module Name:

    pathmisc.c

Abstract:

    Win32 miscellaneous path functions

--*/

#include "basedll.h"
#include <xboxp.h>
#include <xdisk.h>
#include <xconfig.h>
#include "xmeta.h"
#include "..\..\fatx\fat.h"

static const OBJECT_STRING ZDrive      = CONSTANT_OBJECT_STRING( OTEXT("\\??\\Z:") );
static const OCHAR CacheDriveFormat[]  = OTEXT("\\Device\\Harddisk0\\Partition%d\\");
static COBJECT_STRING WDrive           = CONSTANT_OBJECT_STRING( OTEXT("\\??\\W:") );
static COBJECT_STRING XDrive           = CONSTANT_OBJECT_STRING( OTEXT("\\??\\X:") );

#ifdef XAPILIBP

extern XAPI_MU_INFO XapiMuInfo;

#else  // XAPILIBP

XAPI_MU_INFO XapiMuInfo = {0};
#if DBG
BOOL g_fMountedUtilityDrive = FALSE;
#endif // DBG

#endif // XAPILIBP

//
//  Define the FAT32 X-Box cache db sector
//

typedef struct _XBOX_CACHE_DB_SECTOR {
    ULONG SectorBeginSignature;                     // offset = 0x000   0
    ULONG Version;                                  // offset = 0x004   4
    UCHAR Data[496];                                // offset = 0x008   8
    ULONG SectorEndSignature;                       // offset = 0x1fc 508
} XBOX_CACHE_DB_SECTOR, *PXBOX_CACHE_DB_SECTOR;

#define XBOX_HD_SECTOR_SIZE                    512

#define XBOX_CACHE_DB_DATA_SIZE                (sizeof(((PXBOX_CACHE_DB_SECTOR) 0)->Data))

#define XBOX_CACHE_DB_SECTOR_BEGIN_SIGNATURE   0x97315286
#define XBOX_CACHE_DB_SECTOR_END_SIGNATURE     0xAA550000
#define XBOX_CACHE_DB_CUR_VERSION              0x00000002
#define XBOX_CACHE_DB_MAX_ENTRY_COUNT          (XBOX_CACHE_DB_DATA_SIZE / sizeof(X_CACHE_DB_ENTRY))

#ifndef XAPILIBP

WINBASEAPI
BOOL
WINAPI
GetDiskFreeSpaceEx(
    PCOSTR lpDirectoryName,
    PULARGE_INTEGER lpFreeBytesAvailableToCaller,
    PULARGE_INTEGER lpTotalNumberOfBytes,
    PULARGE_INTEGER lpTotalNumberOfFreeBytes
    )
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    OBJECT_STRING FileName;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_FS_SIZE_INFORMATION NormalSizeInfo;

    ULARGE_INTEGER BytesPerAllocationUnit;
    ULARGE_INTEGER FreeBytesAvailableToCaller;
    ULARGE_INTEGER TotalNumberOfBytes;

    RIP_ON_NOT_TRUE("GetDiskFreeSpaceEx()", ARGUMENT_PRESENT(lpDirectoryName));

    RtlInitObjectString(&FileName, lpDirectoryName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    //
    // Open the file
    //

    Status = NtOpenFile(
                &Handle,
                (ACCESS_MASK)FILE_LIST_DIRECTORY | SYNCHRONIZE,
                &Obja,
                &IoStatusBlock,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE | FILE_OPEN_FOR_FREE_SPACE_QUERY
                );
    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        if ( GetLastError() == ERROR_FILE_NOT_FOUND ) {
            SetLastError(ERROR_PATH_NOT_FOUND);
            }
        return FALSE;
        }

    //
    // Determine the size parameters of the volume.
    //

    Status = NtQueryVolumeInformationFile(
                Handle,
                &IoStatusBlock,
                &NormalSizeInfo,
                sizeof(NormalSizeInfo),
                FileFsSizeInformation
                );
    NtClose(Handle);
    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
        }

    BytesPerAllocationUnit.QuadPart =
        NormalSizeInfo.BytesPerSector * NormalSizeInfo.SectorsPerAllocationUnit;

    FreeBytesAvailableToCaller.QuadPart =
        BytesPerAllocationUnit.QuadPart * NormalSizeInfo.AvailableAllocationUnits.QuadPart;

    TotalNumberOfBytes.QuadPart =
        BytesPerAllocationUnit.QuadPart * NormalSizeInfo.TotalAllocationUnits.QuadPart;

    if ( ARGUMENT_PRESENT(lpFreeBytesAvailableToCaller) ) {
        lpFreeBytesAvailableToCaller->QuadPart = FreeBytesAvailableToCaller.QuadPart;
        }
    if ( ARGUMENT_PRESENT(lpTotalNumberOfBytes) ) {
        lpTotalNumberOfBytes->QuadPart = TotalNumberOfBytes.QuadPart;
        }
    if ( ARGUMENT_PRESENT(lpTotalNumberOfFreeBytes) ) {
        lpTotalNumberOfFreeBytes->QuadPart = FreeBytesAvailableToCaller.QuadPart;
        }

    return TRUE;
}


BOOL
APIENTRY
GetVolumeInformation(
    PCOSTR lpRootPathName,
    POSTR lpVolumeNameBuffer,
    DWORD nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    POSTR lpFileSystemNameBuffer,
    DWORD nFileSystemNameSize
    )

/*++

Routine Description:

    This function returns information about the file system whose root
    directory is specified.

Arguments:

    lpRootPathName - An optional parameter, that if specified, supplies
        the root directory of the file system that information is to be
        returned about.  If this parameter is not specified, then the
        root of the current directory is used.

    lpVolumeNameBuffer - An optional parameter that if specified returns
        the name of the specified volume.

    nVolumeNameSize - Supplies the length of the volume name buffer.
        This parameter is ignored if the volume name buffer is not
        supplied.

    lpVolumeSerialNumber - An optional parameter that if specified
        points to a DWORD.  The DWORD contains the 32-bit of the volume
        serial number.

    lpMaximumComponentLength - An optional parameter that if specified
        returns the maximum length of a filename component supported by
        the specified file system.  A filename component is that portion
        of a filename between pathname seperators.

    lpFileSystemFlags - An optional parameter that if specified returns
        flags associated with the specified file system.

        lpFileSystemFlags Flags:

            FS_CASE_IS_PRESERVED - Indicates that the case of file names
                is preserved when the name is placed on disk.

            FS_CASE_SENSITIVE - Indicates that the file system supports
                case sensitive file name lookup.

            FS_UNICODE_STORED_ON_DISK - Indicates that the file system
                supports unicode in file names as they appear on disk.

    lpFileSystemNameBuffer - An optional parameter that if specified returns
        the name for the specified file system (e.g. FAT, HPFS...).

    nFileSystemNameSize - Supplies the length of the file system name
        buffer.  This parameter is ignored if the file system name
        buffer is not supplied.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    OBJECT_STRING FileName;
    IO_STATUS_BLOCK IoStatusBlock;
    PFILE_FS_ATTRIBUTE_INFORMATION AttributeInfo;
    PFILE_FS_VOLUME_INFORMATION VolumeInfo;
    ULONG AttributeInfoLength;
    ULONG VolumeInfoLength;
    BOOL rv;

    rv = FALSE;

    nVolumeNameSize *= 2;
    nFileSystemNameSize *= 2;

    RIP_ON_NOT_TRUE("GetVolumeInformation()", ARGUMENT_PRESENT(lpRootPathName));

    RtlInitObjectString(&FileName, lpRootPathName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    AttributeInfo = NULL;
    VolumeInfo = NULL;

    //
    // Open the file
    //

    Status = NtOpenFile(
                &Handle,
                (ACCESS_MASK)FILE_LIST_DIRECTORY | SYNCHRONIZE,
                &Obja,
                &IoStatusBlock,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE | FILE_OPEN_FOR_BACKUP_INTENT
                );
    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
        }

    if ( ARGUMENT_PRESENT(lpVolumeNameBuffer) ||
         ARGUMENT_PRESENT(lpVolumeSerialNumber) ) {
        if ( ARGUMENT_PRESENT(lpVolumeNameBuffer) ) {
            VolumeInfoLength = sizeof(*VolumeInfo)+nVolumeNameSize;
            }
        else {
            VolumeInfoLength = sizeof(*VolumeInfo)+MAX_PATH;
            }
        VolumeInfo = RtlAllocateHeap(XapiProcessHeap, 0, VolumeInfoLength);

        if ( !VolumeInfo ) {
            NtClose(Handle);
            XapiSetLastNTError(STATUS_NO_MEMORY);
            return FALSE;
            }
        }

    if ( ARGUMENT_PRESENT(lpFileSystemNameBuffer) ||
         ARGUMENT_PRESENT(lpMaximumComponentLength) ||
         ARGUMENT_PRESENT(lpFileSystemFlags) ) {
        if ( ARGUMENT_PRESENT(lpFileSystemNameBuffer) ) {
            AttributeInfoLength = sizeof(*AttributeInfo) + nFileSystemNameSize;
            }
        else {
            AttributeInfoLength = sizeof(*AttributeInfo) + MAX_PATH;
            }
        AttributeInfo = RtlAllocateHeap(XapiProcessHeap, 0, AttributeInfoLength);
        if ( !AttributeInfo ) {
            NtClose(Handle);
            if ( VolumeInfo ) {
                RtlFreeHeap(XapiProcessHeap, 0,VolumeInfo);
                }
            XapiSetLastNTError(STATUS_NO_MEMORY);
            return FALSE;
            }
        }

    try {
        if ( VolumeInfo ) {
            Status = NtQueryVolumeInformationFile(
                        Handle,
                        &IoStatusBlock,
                        VolumeInfo,
                        VolumeInfoLength,
                        FileFsVolumeInformation
                        );
            if ( !NT_SUCCESS(Status) ) {
                XapiSetLastNTError(Status);
                rv = FALSE;
                goto finally_exit;
                }
            }

        if ( AttributeInfo ) {
            Status = NtQueryVolumeInformationFile(
                        Handle,
                        &IoStatusBlock,
                        AttributeInfo,
                        AttributeInfoLength,
                        FileFsAttributeInformation
                        );
            if ( !NT_SUCCESS(Status) ) {
                XapiSetLastNTError(Status);
                rv = FALSE;
                goto finally_exit;
                }
            }
        try {

            if ( ARGUMENT_PRESENT(lpVolumeNameBuffer) ) {
                if ( VolumeInfo->VolumeLabelLength >= nVolumeNameSize ) {
                    SetLastError(ERROR_BAD_LENGTH);
                    rv = FALSE;
                    goto finally_exit;
                    }
                else {
                    RtlMoveMemory( lpVolumeNameBuffer,
                                   VolumeInfo->VolumeLabel,
                                   VolumeInfo->VolumeLabelLength );

                    *(lpVolumeNameBuffer + (VolumeInfo->VolumeLabelLength / sizeof(OCHAR))) = OBJECT_NULL;
                    }
                }

            if ( ARGUMENT_PRESENT(lpVolumeSerialNumber) ) {
                *lpVolumeSerialNumber = VolumeInfo->VolumeSerialNumber;
                }

            if ( ARGUMENT_PRESENT(lpFileSystemNameBuffer) ) {

                if ( AttributeInfo->FileSystemNameLength >= nFileSystemNameSize ) {
                    SetLastError(ERROR_BAD_LENGTH);
                    rv = FALSE;
                    goto finally_exit;
                    }
                else {
                    RtlMoveMemory( lpFileSystemNameBuffer,
                                   AttributeInfo->FileSystemName,
                                   AttributeInfo->FileSystemNameLength );

                    *(lpFileSystemNameBuffer + (AttributeInfo->FileSystemNameLength / sizeof(OCHAR))) = OBJECT_NULL;
                    }
                }

            if ( ARGUMENT_PRESENT(lpMaximumComponentLength) ) {
                *lpMaximumComponentLength = AttributeInfo->MaximumComponentNameLength;
                }

            if ( ARGUMENT_PRESENT(lpFileSystemFlags) ) {
                *lpFileSystemFlags = AttributeInfo->FileSystemAttributes;
                }
            }
        except (EXCEPTION_EXECUTE_HANDLER) {
            XapiSetLastNTError(STATUS_ACCESS_VIOLATION);
            return FALSE;
            }
        rv = TRUE;
finally_exit:;
        }
    finally {
        NtClose(Handle);
        if ( VolumeInfo ) {
            RtlFreeHeap(XapiProcessHeap, 0,VolumeInfo);
            }
        if ( AttributeInfo ) {
            RtlFreeHeap(XapiProcessHeap, 0,AttributeInfo);
            }
        }
    return rv;
}

NTSTATUS
XapiSelectCachePartition(
    IN BOOL fAlwaysFormat,
    OUT PULONG pnCachePartition,
    OUT PBOOL pfForceFormat
    )
{
    OBJECT_ATTRIBUTES oa;
    NTSTATUS          Status;
    IO_STATUS_BLOCK   statusBlock;
    HANDLE            hVolume;
    DWORD             dwTitleId = XeImageHeader()->Certificate->TitleID;
    ULONG             CachePartitionCount;
    ULONG             nCachePartition;

    ASSERT(pnCachePartition && pfForceFormat);

    InitializeObjectAttributes(&oa,
                               (POBJECT_STRING) &XapiHardDisk,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL);

    Status = NtOpenFile(&hVolume,
                        SYNCHRONIZE | GENERIC_READ | GENERIC_WRITE,
                        &oa,
                        &statusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        FILE_SYNCHRONOUS_IO_ALERT);

    if (NT_SUCCESS(Status))
    {
        UCHAR rgbSectorBuffer[XBOX_HD_SECTOR_SIZE];
        LARGE_INTEGER byteOffset;

        //
        // Read sector 4 (XBOX_CACHE_DB_SECTOR_INDEX)
        //

        byteOffset.QuadPart = XBOX_CACHE_DB_SECTOR_INDEX * XBOX_HD_SECTOR_SIZE;

        Status = NtReadFile(hVolume,
                            0,
                            NULL,
                            NULL,
                            &statusBlock,
                            rgbSectorBuffer,
                            sizeof(rgbSectorBuffer),
                            &byteOffset);

        if (NT_SUCCESS(Status))
        {
            PXBOX_CACHE_DB_SECTOR pCacheDBSec = (PXBOX_CACHE_DB_SECTOR) rgbSectorBuffer;
            PX_CACHE_DB_ENTRY pCacheDB = (PX_CACHE_DB_ENTRY) pCacheDBSec->Data;
            ULONG i;
            ULONG iPrevDBIndex = (*HalDiskCachePartitionCount - 1);
            ULONG iNewDBIndex;

            if ((XBOX_CACHE_DB_SECTOR_BEGIN_SIGNATURE != pCacheDBSec->SectorBeginSignature) ||
                (XBOX_CACHE_DB_SECTOR_END_SIGNATURE != pCacheDBSec->SectorEndSignature) ||
                (XBOX_CACHE_DB_CUR_VERSION != pCacheDBSec->Version))
            {
                RtlZeroMemory(rgbSectorBuffer, sizeof(rgbSectorBuffer));

                pCacheDBSec->SectorBeginSignature = XBOX_CACHE_DB_SECTOR_BEGIN_SIGNATURE;
                pCacheDBSec->Version = XBOX_CACHE_DB_CUR_VERSION;
                pCacheDBSec->SectorEndSignature = XBOX_CACHE_DB_SECTOR_END_SIGNATURE;
            }

            //
            // Assume that we're going to force the partition to be formatted unless
            // we find out otherwise
            //

            *pfForceFormat = TRUE;

            //
            // Obtain the number of cache partitions from the HAL.  The HAL
            // won't boot with a drive too small to contain one cache
            // partition, but we do need to limit the cache partition count
            // to the number that we can describe in the cache partition
            // database.
            //

            CachePartitionCount = *HalDiskCachePartitionCount;

            ASSERT(CachePartitionCount > 0);

            if (CachePartitionCount > XBOX_CACHE_DB_MAX_ENTRY_COUNT)
            {
                CachePartitionCount = XBOX_CACHE_DB_MAX_ENTRY_COUNT;
            }

            nCachePartition = 0;

            //
            // Search for a cache partition already allocated by this title
            //
            
            for (i = 0; i < CachePartitionCount; i++)
            {
                if ((dwTitleId == pCacheDB[i].dwTitleId) && pCacheDB[i].fUsed)
                {
                    nCachePartition = pCacheDB[i].nCacheIndex + XDISK_FIRST_CACHE_PARTITION;
                    iPrevDBIndex = i;
                    
                    //
                    // We found an existing cache partition, there is no longer a
                    // requirement that the partition be formatted.
                    //

                    *pfForceFormat = FALSE;
                    
                    break;
                }
            }

            //
            // If that search failed, search for a cache partition that is not in use
            // using ugly n-squared algorithm (fortunately, CachePartitionCount is small)
            //
            
            if (0 == nCachePartition)
            {
                UINT j;
                for (j = 0; j < CachePartitionCount; j++)
                {
                    for (i = 0; i < CachePartitionCount; i++)
                    {
                        if ((pCacheDB[i].fUsed) && (pCacheDB[i].nCacheIndex == j))
                        {
                            break;
                        }
                    }

                    //
                    // If we made it through the loop without a match, then this
                    // cache partition (index stored in the j variable) is available
                    //
                    
                    if (i == CachePartitionCount)
                    {
                        nCachePartition = j + XDISK_FIRST_CACHE_PARTITION;
                    }
                }
            }
                
            //
            // If that search failed, grab the oldest cache partition
            //
            // The Cache DB is stored in MRU order - the first entry was the most recently
            // used and the last entry was the least recently used
            //
            
            if (0 == nCachePartition)
            {
                nCachePartition = pCacheDB[CachePartitionCount - 1].nCacheIndex + XDISK_FIRST_CACHE_PARTITION;
            }

            //
            // If the value we've chosen is too large for some reason, pull it back and give
            // it a reasonable value
            //
            
            if (nCachePartition >= CachePartitionCount + XDISK_FIRST_CACHE_PARTITION)
            {
                nCachePartition = (CachePartitionCount - 1) + XDISK_FIRST_CACHE_PARTITION;
            }

            ASSERT(nCachePartition != 0);
            *pnCachePartition = nCachePartition;

            //
            // Normally, we bump this to the top of the cache db because that is how we
            // indicate that it was most recently used.  When fAlwaysFormat is set, we
            // always put it at the end of the list because we want it to be reclaimed
            // when the next title asks for a cache partition
            //
            
            iNewDBIndex = fAlwaysFormat ? (CachePartitionCount - 1) : 0;

            if (!fAlwaysFormat && (0 != iPrevDBIndex))
            {
                //
                // Modify the cache db - slide everything down and make room for this
                // entry at the top of the list
                //

                ASSERT(iPrevDBIndex < CachePartitionCount);
                
                RtlMoveMemory(&(pCacheDB[1]),
                              &(pCacheDB[0]),
                              iPrevDBIndex * sizeof(X_CACHE_DB_ENTRY));
            }
            
            //
            // Write this entry into the new index of the cache db
            //
            // Note that if this function was called with fAlwaysFormat set to TRUE,
            // the entry will be marked with fUsed == FALSE, so that it will be chosen
            // first the next time a title needs to allocate a new partition
            //

            pCacheDB[iNewDBIndex].dwTitleId = dwTitleId;
            pCacheDB[iNewDBIndex].nCacheIndex = (nCachePartition - XDISK_FIRST_CACHE_PARTITION);
            pCacheDB[iNewDBIndex].fUsed = (!fAlwaysFormat);

            //
            // Ignore status result
            //

            NtWriteFile(hVolume,
                        0,
                        NULL,
                        NULL,
                        &statusBlock,
                        rgbSectorBuffer,
                        sizeof(rgbSectorBuffer),
                        &byteOffset);
        }

        NtClose(hVolume);
    }

    return Status;
}


BOOL
WINAPI
XMountUtilityDrive(
    BOOL fFormatClean
    )
{
    BOOL fRet = TRUE;
    BOOL fForceFormat;
    ULONG nPartition;
    NTSTATUS Status;

#if DBG
    if (g_fMountedUtilityDrive)
    {
        RIP("XMountUtilityDrive(): Utility Drive has already been mounted");
    }
#endif // DBG

    Status = XapiSelectCachePartition(fFormatClean, &nPartition, &fForceFormat);

    if (NT_SUCCESS(Status))
    {
        OCHAR szCacheDrive[MAX_PATH];
        OBJECT_STRING VolString, DriveString;
        BOOL fDoFormat = (fFormatClean || fForceFormat);
        ULONG BytesPerCluster = XeUtilityDriveClusterSize();

        _snoprintf(szCacheDrive,
                   ARRAYSIZE(szCacheDrive),
                   CacheDriveFormat,
                   nPartition);

        RtlInitObjectString(&VolString, szCacheDrive);

        //
        // The DriveString should not end in a backslash, so init from the same
        // string, but subtract a character on the Length member.
        //

        RtlInitObjectString(&DriveString, szCacheDrive);
        DriveString.Length -= sizeof(OCHAR);

        if (fDoFormat)
        {
            fRet = XapiFormatFATVolumeEx(&DriveString, BytesPerCluster);
        }

        if (fRet)
        {
            Status = XapiValidateDiskPartitionEx(&VolString, BytesPerCluster);

            if (!NT_SUCCESS(Status) && !fDoFormat)
            {
                //
                // If the validate failed for some reason and we didn't just format
                // the partition, go ahead and format it now (make the system more
                // self-healing)
                //

                if (XapiFormatFATVolumeEx(&DriveString, BytesPerCluster))
                {
                    Status = XapiValidateDiskPartitionEx(&VolString, BytesPerCluster);
                }
            }

            if (NT_SUCCESS(Status))
            {
                // Give the cache partition a drive letter
                Status = IoCreateSymbolicLink((POBJECT_STRING) &ZDrive, &DriveString);
            }

            fRet = NT_SUCCESS(Status);

            if (!fRet)
            {
                XapiSetLastNTError(Status);
            }
        }
    }
    else
    {
        fRet = FALSE;
        XapiSetLastNTError(Status);
    }

#if DBG
    if (fRet)
    {
        g_fMountedUtilityDrive = TRUE;
    }
#endif // DBG

    return fRet;
}


BOOL
WINAPI
XFormatUtilityDrive(
    VOID
    )
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    CHAR Target[MAX_PATH];
    ULONG TargetLength;
    OBJECT_STRING ObjectTarget;
    HANDLE Handle;

#if DBG
    if (!g_fMountedUtilityDrive)
    {
        RIP("XFormatUtilityDrive(): Utility Drive has not been mounted");
    }
#endif // DBG

    InitializeObjectAttributes(&ObjectAttributes,
                               (POBJECT_STRING) &ZDrive,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL);

    status = NtOpenSymbolicLinkObject(&Handle, &ObjectAttributes);

    if (!NT_SUCCESS(status))
    {
        XapiSetLastNTError(status);
        return FALSE;
    }

    ObjectTarget.Buffer = Target;
    ObjectTarget.MaximumLength = sizeof(Target);

    status = NtQuerySymbolicLinkObject(Handle, &ObjectTarget, &TargetLength);

    NtClose(Handle);

    if (!NT_SUCCESS(status))
    {
        XapiSetLastNTError(status);
        return FALSE;
    }

    return XapiFormatFATVolumeEx(&ObjectTarget, XeUtilityDriveClusterSize());
}


DWORD
WINAPI
XMountAlternateTitle(
    IN PCOSTR lpRootPath,
    IN DWORD dwAltTitleId,
    OUT POCHAR pchDrive
    )
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    OBJECT_STRING ObjectName;
    OCHAR szDosDevice[MAX_PATH];
    OCHAR Target[MAX_PATH];
    ULONG TargetLength;
    OBJECT_STRING ObjectTarget;
    OCHAR AltTitleId[CCHMAX_HEX_DWORD];
    HANDLE Handle;
    OCHAR chDrive;
    BOOL fTData;
    PXBEIMAGE_CERTIFICATE Certificate = XeImageHeader()->Certificate;
    int i;

    RIP_ON_NOT_TRUE(XMountAlternateTitle, (lpRootPath != NULL));
    RIP_ON_NOT_TRUE(XMountAlternateTitle, (lpRootPath[0] != '\0'));
    RIP_ON_NOT_TRUE(XMountAlternateTitle, (lpRootPath[1] == ':'));
    RIP_ON_NOT_TRUE(XMountAlternateTitle, (lpRootPath[2] == '\\'));
    RIP_ON_NOT_TRUE(XMountAlternateTitle, (lpRootPath[3] == '\0'));
    RIP_ON_NOT_TRUE(XMountAlternateTitle, (pchDrive != NULL));

    //
    // Removing the 0x20 bit will make lower case characters uppercase
    //

    chDrive = lpRootPath[0] & (~0x20);
    fTData = (HD_TDATA_DRIVE == chDrive);

#if DBG
    if (((chDrive < MU_FIRST_DRIVE) || (chDrive > MU_LAST_DRIVE)) &&
        (HD_UDATA_DRIVE != chDrive) &&
        (!fTData))
    {
        RIP("XFindFirstSaveGame() invalid drive letter parameter");
    }
#endif // DBG

    for (i = 0; i < ARRAYSIZE(Certificate->AlternateTitleIDs); i++)
    {
        if (0 == Certificate->AlternateTitleIDs[i])
        {
            return ERROR_ACCESS_DENIED;
        }

        if (dwAltTitleId == Certificate->AlternateTitleIDs[i])
        {
            break;
        }
    }

    if (i >= sizeof(Certificate->AlternateTitleIDs))
    {
        return ERROR_ACCESS_DENIED;
    }

    soprintf(szDosDevice, OTEXT("\\??\\%c:"), lpRootPath[0]);

    RtlInitObjectString(&ObjectName, szDosDevice);

    InitializeObjectAttributes(&ObjectAttributes,
                               &ObjectName,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL);

    status = NtOpenSymbolicLinkObject(&Handle, &ObjectAttributes);

    if (!NT_SUCCESS(status))
    {
        return RtlNtStatusToDosError(status);
    }

    ObjectTarget.Buffer = Target;
    ObjectTarget.MaximumLength = sizeof(Target);

    status = NtQuerySymbolicLinkObject(Handle, &ObjectTarget, &TargetLength);

    NtClose(Handle);

    if (!NT_SUCCESS(status))
    {
        return RtlNtStatusToDosError(status);
    }

    if ((TargetLength < CCHMAX_HEX_DWORD) ||
        ('\\' != Target[TargetLength - (CCHMAX_HEX_DWORD)]))
    {
        return ERROR_INVALID_DRIVE;
    }

    //
    // Remove the existing title id from the end of the string so we can
    // reuse ObjectTarget below in XapiMapLetterToDirectory()
    //

    ObjectTarget.Length -= CCHMAX_HEX_DWORD;

    DwordToStringO(dwAltTitleId, AltTitleId);

    status = XapiMapLetterToDirectory(fTData ? &WDrive : &XDrive,
                                      (PCOBJECT_STRING) &ObjectTarget,
                                      AltTitleId,
                                      FALSE,
                                      NULL,
                                      FALSE);

    if (NT_SUCCESS(status))
    {
        *pchDrive = fTData ? HD_ALT_TDATA_DRIVE : HD_ALT_UDATA_DRIVE;

        if (!fTData && (HD_UDATA_DRIVE != chDrive))
        {
            //
            // Remember that we've mapped an alternate drive letter to this MU
            // drive so that we can unmount the alternate drive automatically
            // if the "real" MU drive is unmounted later using XUnmountMU()
            //

            ASSERT(OBJECT_NULL == XapiMuInfo.DriveWithAltDriveMapped);
            XapiMuInfo.DriveWithAltDriveMapped = chDrive;
        }
    }

    return RtlNtStatusToDosError(status);
}

DWORD
WINAPI
XUnmountAlternateTitle(
    IN OCHAR chDrive
    )
{
    NTSTATUS Status;
    OCHAR szDosDevice[MAX_PATH];
    OBJECT_STRING DosDevice;

    //
    // Removing the 0x20 bit will make lower case characters uppercase
    //

    chDrive &= (~0x20);

#if DBG
    switch (chDrive)
    {
        case HD_ALT_TDATA_DRIVE:
        case HD_ALT_UDATA_DRIVE:
            break;

        default:
            RIP("XUnmountAlternateTitle() - invalid chDrive parameter");
    }
#endif // DBG

    soprintf(szDosDevice, OTEXT("\\??\\%c:"), chDrive);

    RtlInitObjectString(&DosDevice, szDosDevice);

    //
    // BUGBUG: Do more than remove the symbolic link - we need to unmount
    // the filesystem here.
    //

    Status = IoDeleteSymbolicLink(&DosDevice);

    if ((HD_ALT_UDATA_DRIVE == chDrive) && NT_SUCCESS(Status))
    {
        XapiMuInfo.DriveWithAltDriveMapped = OBJECT_NULL;
    }

    return RtlNtStatusToDosError(Status);
}

DWORD
WINAPI
XGetDiskClusterSize(
    PCOSTR lpRootPathName
    )
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    OBJECT_STRING FileName;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_FS_SIZE_INFORMATION NormalSizeInfo;

    RIP_ON_NOT_TRUE("XGetDiskClusterSize()", ARGUMENT_PRESENT(lpRootPathName));

    RtlInitObjectString(&FileName, lpRootPathName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    //
    // Open the file
    //

    Status = NtOpenFile(
                &Handle,
                (ACCESS_MASK)FILE_LIST_DIRECTORY | SYNCHRONIZE,
                &Obja,
                &IoStatusBlock,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE | FILE_OPEN_FOR_FREE_SPACE_QUERY
                );
    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        if ( GetLastError() == ERROR_FILE_NOT_FOUND ) {
            SetLastError(ERROR_PATH_NOT_FOUND);
            }
        return 0;
        }

    //
    // Determine the size parameters of the volume.
    //

    Status = NtQueryVolumeInformationFile(
                Handle,
                &IoStatusBlock,
                &NormalSizeInfo,
                sizeof(NormalSizeInfo),
                FileFsSizeInformation
                );
    NtClose(Handle);
    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return 0;
        }

    ASSERT((0 != NormalSizeInfo.BytesPerSector) && (0 != NormalSizeInfo.SectorsPerAllocationUnit));

    return (ULONG) (NormalSizeInfo.BytesPerSector * NormalSizeInfo.SectorsPerAllocationUnit);
}

DWORD
WINAPI
XGetDiskSectorSize(
    PCOSTR lpRootPathName
    )
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    OBJECT_STRING FileName;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_FS_SIZE_INFORMATION NormalSizeInfo;

    RIP_ON_NOT_TRUE("XGetDiskSectorSize()", ARGUMENT_PRESENT(lpRootPathName));

    RtlInitObjectString(&FileName, lpRootPathName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    //
    // Open the file
    //

    Status = NtOpenFile(
                &Handle,
                (ACCESS_MASK)FILE_LIST_DIRECTORY | SYNCHRONIZE,
                &Obja,
                &IoStatusBlock,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE | FILE_OPEN_FOR_FREE_SPACE_QUERY
                );
    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        if ( GetLastError() == ERROR_FILE_NOT_FOUND ) {
            SetLastError(ERROR_PATH_NOT_FOUND);
            }
        return 0;
        }

    //
    // Determine the size parameters of the volume.
    //

    Status = NtQueryVolumeInformationFile(
                Handle,
                &IoStatusBlock,
                &NormalSizeInfo,
                sizeof(NormalSizeInfo),
                FileFsSizeInformation
                );
    NtClose(Handle);
    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return 0;
        }

    ASSERT(0 != NormalSizeInfo.BytesPerSector);

    return NormalSizeInfo.BytesPerSector;
}


DWORD
WINAPI
XMUNameFromDriveLetter(
    IN CHAR chDrive,
    OUT LPWSTR lpName,
    IN UINT cchName
    )
{
    NTSTATUS Status;
    OCHAR szDosDevice[8];
    OBJECT_STRING DosDevice;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    IO_STATUS_BLOCK IoStatusBlock;
    FSCTL_VOLUME_METADATA VolumeMetadata;
    WCHAR VolumeName[FAT_VOLUME_NAME_LENGTH];

    //
    // Fail if the device is not already mounted.
    //
    if (!MU_IS_MOUNTED(chDrive))
    {
        XDBGERR("XAPI", "XMUNameFromDriveLetter() MU %c: is not mounted", chDrive);
        return ERROR_INVALID_DRIVE;
    }

    //
    // Open a handle to the volume or directory of the drive.
    //
    soprintf(szDosDevice, OTEXT("\\??\\%c:"), chDrive);
    RtlInitObjectString(&DosDevice, szDosDevice);

    InitializeObjectAttributes(
        &Obja,
        (POBJECT_STRING) &DosDevice,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    Status = NtOpenFile(&Handle,
                        SYNCHRONIZE | GENERIC_READ,
                        &Obja,
                        &IoStatusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        FILE_SYNCHRONOUS_IO_ALERT);

    if (NT_SUCCESS(Status))
    {
        VolumeMetadata.ByteOffset = FIELD_OFFSET(FAT_VOLUME_METADATA, VolumeName);
        VolumeMetadata.TransferLength = sizeof(VolumeName);
        VolumeMetadata.TransferBuffer = VolumeName;

        Status = NtFsControlFile(Handle,
                                 NULL,
                                 NULL,
                                 NULL,
                                 &IoStatusBlock,
                                 FSCTL_READ_VOLUME_METADATA,
                                 &VolumeMetadata,
                                 sizeof(VolumeMetadata),
                                 NULL,
                                 0);

        if (NT_SUCCESS(Status))
        {
            lstrcpynW(lpName, VolumeName, min(cchName, FAT_VOLUME_NAME_LENGTH));
        }

        NtClose(Handle);
    }

    return RtlNtStatusToDosError(Status);
}

#endif // ! XAPILIBP

#ifdef XAPILIBP

VOID
XapiDeleteCachePartition(
    IN DWORD dwTitleId
    )
{
    OBJECT_ATTRIBUTES oa;
    NTSTATUS          Status;
    IO_STATUS_BLOCK   statusBlock;
    HANDLE            hVolume;
    ULONG             CachePartitionCount;

    InitializeObjectAttributes(&oa,
                               (POBJECT_STRING) &XapiHardDisk,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL);

    Status = NtOpenFile(&hVolume,
                        SYNCHRONIZE | GENERIC_READ | GENERIC_WRITE,
                        &oa,
                        &statusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        FILE_SYNCHRONOUS_IO_ALERT);

    if (NT_SUCCESS(Status))
    {
        UCHAR rgbSectorBuffer[XBOX_HD_SECTOR_SIZE];
        LARGE_INTEGER byteOffset;

        //
        // Read sector 4 (XBOX_CACHE_DB_SECTOR_INDEX)
        //

        byteOffset.QuadPart = XBOX_CACHE_DB_SECTOR_INDEX * XBOX_HD_SECTOR_SIZE;

        Status = NtReadFile(hVolume,
                            0,
                            NULL,
                            NULL,
                            &statusBlock,
                            rgbSectorBuffer,
                            sizeof(rgbSectorBuffer),
                            &byteOffset);

        if (NT_SUCCESS(Status))
        {
            PXBOX_CACHE_DB_SECTOR pCacheDBSec = (PXBOX_CACHE_DB_SECTOR) rgbSectorBuffer;
            PX_CACHE_DB_ENTRY pCacheDB = (PX_CACHE_DB_ENTRY) pCacheDBSec->Data;
            ULONG i;

            if ((XBOX_CACHE_DB_SECTOR_BEGIN_SIGNATURE != pCacheDBSec->SectorBeginSignature) ||
                (XBOX_CACHE_DB_SECTOR_END_SIGNATURE != pCacheDBSec->SectorEndSignature) ||
                (XBOX_CACHE_DB_CUR_VERSION != pCacheDBSec->Version))
            {
                NtClose(hVolume);

                return;
            }

            //
            // Obtain the number of cache partitions from the HAL.  The HAL
            // won't boot with a drive too small to contain one cache
            // partition, but we do need to limit the cache partition count
            // to the number that we can describe in the cache partition
            // database.
            //

            CachePartitionCount = *HalDiskCachePartitionCount;

            ASSERT(CachePartitionCount > 0);

            if (CachePartitionCount > XBOX_CACHE_DB_MAX_ENTRY_COUNT)
            {
                CachePartitionCount = XBOX_CACHE_DB_MAX_ENTRY_COUNT;
            }

            //
            // Search the cache partition database for a matching title ID
            // If found, clear the title ID so we can write it back to the sector
            //

            for (i = 0; i < CachePartitionCount; i++)
            {
                if (dwTitleId == pCacheDB[i].dwTitleId)
                {
                    pCacheDB[i].dwTitleId = 0;
                    pCacheDB[i].fUsed = FALSE;

                    break;
                }
            }

            //
            // If we picked this partition because it is the oldest and not because
            // we matched a TitleId, then we must format the cache partition before
            // giving it to the title
            //

            //
            // Write back into the cache db
            //

            //
            // Ignore status result
            //

            Status = NtWriteFile(hVolume,
                                 0,
                                 NULL,
                                 NULL,
                                 &statusBlock,
                                 rgbSectorBuffer,
                                 sizeof(rgbSectorBuffer),
                                 &byteOffset);
        }

        NtClose(hVolume);
    }
}


NTSTATUS
XapiGetCachePartitions(
    IN PX_CACHE_DB_ENTRY pCacheEntriesBuffer,
    IN UINT cbBufferSize,
    OUT PDWORD pdwNumCacheEntries )
{
    OBJECT_ATTRIBUTES oa;
    NTSTATUS          Status;
    IO_STATUS_BLOCK   statusBlock;
    HANDLE            hVolume;
    ULONG             CachePartitionCount;

    ASSERT(pdwNumCacheEntries);

    //
    // Set the number of entries written to 0, in case of failure
    //

    *pdwNumCacheEntries = 0;

    InitializeObjectAttributes(&oa,
                               (POBJECT_STRING) &XapiHardDisk,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL);

    Status = NtOpenFile(&hVolume,
                        SYNCHRONIZE | GENERIC_READ | GENERIC_WRITE,
                        &oa,
                        &statusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        FILE_SYNCHRONOUS_IO_ALERT);

    if (NT_SUCCESS(Status))
    {
        UCHAR rgbSectorBuffer[XBOX_HD_SECTOR_SIZE];
        LARGE_INTEGER byteOffset;

        //
        // Read sector 4 (XBOX_CACHE_DB_SECTOR_INDEX)
        //

        byteOffset.QuadPart = XBOX_CACHE_DB_SECTOR_INDEX * XBOX_HD_SECTOR_SIZE;

        Status = NtReadFile(hVolume,
                            0,
                            NULL,
                            NULL,
                            &statusBlock,
                            rgbSectorBuffer,
                            sizeof(rgbSectorBuffer),
                            &byteOffset);

        if (NT_SUCCESS(Status))
        {
            PXBOX_CACHE_DB_SECTOR pCacheDBSec = (PXBOX_CACHE_DB_SECTOR) rgbSectorBuffer;
            PX_CACHE_DB_ENTRY pCacheDB = (PX_CACHE_DB_ENTRY) pCacheDBSec->Data;
            ULONG i;

            if ((XBOX_CACHE_DB_SECTOR_BEGIN_SIGNATURE != pCacheDBSec->SectorBeginSignature) ||
                (XBOX_CACHE_DB_SECTOR_END_SIGNATURE != pCacheDBSec->SectorEndSignature) ||
                (XBOX_CACHE_DB_CUR_VERSION != pCacheDBSec->Version))
            {
                RtlZeroMemory(rgbSectorBuffer, sizeof(rgbSectorBuffer));
            }

            //
            // Obtain the number of cache partitions from the HAL.  The HAL
            // won't boot with a drive too small to contain one cache
            // partition, but we do need to limit the cache partition count
            // to the number that we can describe in the cache partition
            // database.
            //

            CachePartitionCount = *HalDiskCachePartitionCount;

            ASSERT(CachePartitionCount > 0);

            if (CachePartitionCount > XBOX_CACHE_DB_MAX_ENTRY_COUNT)
            {
                CachePartitionCount = XBOX_CACHE_DB_MAX_ENTRY_COUNT;
            }

            //
            // Search the cache partition database for a matching title ID
            //

            for (i = 0; i < CachePartitionCount; i++)
            {
                if( 0 != pCacheDB[i].dwTitleId )
                {
                    if( ( pCacheEntriesBuffer != NULL ) && ( ( sizeof( X_CACHE_DB_ENTRY ) * (*pdwNumCacheEntries + 1) ) <= cbBufferSize ) )
                    {
                        RtlCopyMemory(&(pCacheEntriesBuffer[*pdwNumCacheEntries]),
                                      &(pCacheDB[i]),
                                      sizeof(X_CACHE_DB_ENTRY));
                    }

                    *pdwNumCacheEntries += 1;
                }
            }
        }

        NtClose(hVolume);
    }

    return Status;
}


DWORD
WINAPI
XMUWriteNameToDriveLetter(
    IN CHAR chDrive,
    IN LPCWSTR lpName
    )
{
    NTSTATUS Status;
    OCHAR szDosDevice[8];
    OBJECT_STRING DosDevice;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    IO_STATUS_BLOCK IoStatusBlock;
    FSCTL_VOLUME_METADATA VolumeMetadata;
    WCHAR VolumeName[FAT_VOLUME_NAME_LENGTH];

    //
    //  Fail if the device is not already mounted.
    //
    if (!MU_IS_MOUNTED(chDrive))
    {
        XDBGERR("XAPI", "XMUNameFromDriveLetter() MU %c: is not mounted", chDrive);
        return ERROR_INVALID_DRIVE;
    }

    ASSERT(wcslen(lpName) < MAX_MUNAME);

    //
    // Open a handle to the volume or directory of the drive.
    //
    soprintf(szDosDevice, OTEXT("\\??\\%c:"), chDrive);
    RtlInitObjectString(&DosDevice, szDosDevice);

    InitializeObjectAttributes(
        &Obja,
        (POBJECT_STRING) &DosDevice,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    Status = NtOpenFile(&Handle,
                        SYNCHRONIZE | GENERIC_WRITE,
                        &Obja,
                        &IoStatusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        FILE_SYNCHRONOUS_IO_ALERT);

    if (NT_SUCCESS(Status))
    {
        lstrcpynW(VolumeName, lpName, FAT_VOLUME_NAME_LENGTH);

        VolumeMetadata.ByteOffset = FIELD_OFFSET(FAT_VOLUME_METADATA, VolumeName);
        VolumeMetadata.TransferLength = sizeof(VolumeName);
        VolumeMetadata.TransferBuffer = VolumeName;

        Status = NtFsControlFile(Handle,
                                 NULL,
                                 NULL,
                                 NULL,
                                 &IoStatusBlock,
                                 FSCTL_WRITE_VOLUME_METADATA,
                                 &VolumeMetadata,
                                 sizeof(VolumeMetadata),
                                 NULL,
                                 0);

        NtClose(Handle);
    }

    return RtlNtStatusToDosError(Status);
}

#endif // XAPILIBP
