// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      FormatHD.cpp
// Contents:  UNDONE-COMMENT
// Revisions: 22-Sep-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "RecovPCH.h"
#include <nt.h>

extern "C" BOOL WINAPI XapiFormatFATVolume(POBJECT_STRING pcVolume);
extern "C" UCHAR FASTCALL RtlFindFirstSetRightMember(IN ULONG Set);

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
#define CONSTANT_OBJECT_STRING(s)   { sizeof( s ) - sizeof( OCHAR ), sizeof( s ), s }

#define MAX_SECS_PER_VERIFY 256

COBJECT_STRING PrimaryHarddisk = CONSTANT_OBJECT_STRING(OTEXT("\\Device\\Harddisk0\\partition0"));
COBJECT_STRING HdPartition1 =    CONSTANT_OBJECT_STRING(OTEXT("\\Device\\Harddisk0\\partition1"));
COBJECT_STRING HdPartition2 =    CONSTANT_OBJECT_STRING(OTEXT("\\Device\\Harddisk0\\partition2"));
COBJECT_STRING HdPartition3 =    CONSTANT_OBJECT_STRING(OTEXT("\\Device\\Harddisk0\\partition3"));
COBJECT_STRING CDrive =          CONSTANT_OBJECT_STRING(OTEXT("\\??\\C:"));
COBJECT_STRING YDrive =          CONSTANT_OBJECT_STRING(OTEXT("\\??\\Y:"));
COBJECT_STRING ZDrive =          CONSTANT_OBJECT_STRING(OTEXT("\\??\\Z:"));

#define DASHBOARD_PARTITION_NUMBER 2

static const CHAR HdPartitionX[] = "\\Device\\Harddisk0\\partition%d";

extern TCHAR g_szResult[1000];

void DebugOutput(TCHAR *tszErr, ...);

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WriteFirstSectors
// Purpose:   Taken verbatim from ntos\recovery\recovery.cpp
// Arguments: 
// Return:    
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NTSTATUS WriteFirstSectors(IN HANDLE hDisk)
{
    LARGE_INTEGER WriteOffset;
    IO_STATUS_BLOCK ioStatus;
    NTSTATUS status;
    BYTE rgbSector[XBOX_HD_SECTOR_SIZE];
	BYTE RefurbSectorBytes[XBOX_HD_SECTOR_SIZE];
	XBOX_REFURB_INFO RefurbInfo;
    PXBOX_CONFIG_SECTOR ConfigSector = (PXBOX_CONFIG_SECTOR) rgbSector;
    int i;

    RtlZeroMemory(rgbSector, sizeof(rgbSector));

    // Zero out sectors 0-2 and 4-15 (including cache db sector):
    // Sector 3 (XBOX_REFURB_INFO_SECTOR_INDEX) contains the refurb
    // information - leave it alone
    for (i = XBOX_BOOT_SECTOR_INDEX; i < XBOX_CONFIG_SECTOR_INDEX +
         XBOX_NUM_CONFIG_SECTORS; i++)
    {
        if (i == XBOX_REFURB_INFO_SECTOR_INDEX)
            continue;

        WriteOffset.QuadPart = i * XBOX_HD_SECTOR_SIZE;

        status = NtWriteFile(hDisk, NULL, NULL, NULL, &ioStatus,
                             rgbSector, XBOX_HD_SECTOR_SIZE, &WriteOffset);
        if (FAILED(status))
            return status;
    }

    return STATUS_SUCCESS;
}
typedef
VOID
(CALLBACK *PFORMAT_PROGRESS_ROUTINE)(
    UINT uPercent
    );


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RecFormatFATVolume
// Purpose:   Taken from ntos\recovery\recovery.cpp
// Arguments: 
// Return:    
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL
WINAPI
RecFormatFATVolume(
    POBJECT_STRING VolumePath,
    BOOL fQuickFormat,
    PFORMAT_PROGRESS_ROUTINE pFPR = 0
    )
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    IO_STATUS_BLOCK IoStatusBlock;
    HANDLE VolumeHandle;
    DISK_GEOMETRY DiskGeometry;
    ULONG SectorShift;
    ULONG StructureAlignment;
    PARTITION_INFORMATION PartitionInformation;
    LARGE_INTEGER ByteOffset;

    if (pFPR)
    {
        pFPR(0);
    }

    InitializeObjectAttributes(&ObjectAttributes, VolumePath,
        OBJ_CASE_INSENSITIVE, NULL, NULL);

    if (!XapiFormatFATVolume(VolumePath))
    {
        return FALSE;
    }

    //
    // Open the volume for read/write access.
    //

    status = NtOpenFile(&VolumeHandle, SYNCHRONIZE | FILE_READ_DATA |
        FILE_WRITE_DATA, &ObjectAttributes, &IoStatusBlock, 0,
        FILE_SYNCHRONOUS_IO_ALERT | FILE_NO_INTERMEDIATE_BUFFERING);

    if (!NT_SUCCESS(status)) {
        // XapiSetLastNTError(status);
        return FALSE;
    }

    //
    // Obtain the drive geometry for the target device.
    //

    status = NtDeviceIoControlFile(VolumeHandle, NULL, NULL, NULL, &IoStatusBlock,
        IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &DiskGeometry,
        sizeof(DiskGeometry));

    if (!NT_SUCCESS(status)) {
        NtClose(VolumeHandle);
        // XapiSetLastNTError(status);
        return FALSE;
    }

    SectorShift = RtlFindFirstSetRightMember(DiskGeometry.BytesPerSector);

    //
    // Obtain the size of the partition for the target device.
    //

    status = NtDeviceIoControlFile(VolumeHandle, NULL, NULL, NULL, &IoStatusBlock,
        IOCTL_DISK_GET_PARTITION_INFO, NULL, 0, &PartitionInformation,
        sizeof(PartitionInformation));

    if (!NT_SUCCESS(status)) {
        NtClose(VolumeHandle);
        // XapiSetLastNTError(status);
        return FALSE;
    }

    //
    // Compute the alignment between the various file system structures.
    // Everything should be page aligned in order to maximum file system cache
    // efficiency.
    //

    StructureAlignment = ROUND_TO_PAGES(DiskGeometry.BytesPerSector);

    //
    // Now do the verify...
    //

    {
        DWORD dwAllSectors = (DWORD) PartitionInformation.PartitionLength.QuadPart >> SectorShift;
        DWORD dwSecsPerVerify = min((dwAllSectors / 20) + 1, MAX_SECS_PER_VERIFY);
        DWORD dwPercent = 0;

        // Do a full verify of the disk

        for (DWORD i = 0; i < dwAllSectors; i += dwSecsPerVerify)
        {
            VERIFY_INFORMATION vi;

            if ((i * 100) / dwAllSectors > dwPercent)
            {
                dwPercent = ((i * 100) / dwAllSectors);
                //XDBGTRC("RECOVERY", "Format is %ld%% complete", dwPercent);
                if (pFPR)
                {
                    pFPR(dwPercent);
                }
            }

            vi.StartingOffset.QuadPart = i << SectorShift;
            vi.Length = min(dwSecsPerVerify, dwAllSectors - i) << SectorShift;

            ASSERT(dwSecsPerVerify < (0xFFFFFFFF >> SectorShift));

            status = NtDeviceIoControlFile(
                        VolumeHandle,
                        NULL,
                        NULL,
                        NULL,
                        &IoStatusBlock,
                        IOCTL_DISK_VERIFY,
                        &vi,
                        sizeof(vi),
                        NULL,
                        0
                        );

            if (!NT_SUCCESS(status))
            {
                break;
            }

            if ((0 == i) && fQuickFormat)
            {
                //
                // NOTE: modifying loop counter, ugly, I know.  But this is a
                // convenient way to make sure that quickformat verifies the
                // beginning and the end of the disk..
                //
                i = max(i + dwSecsPerVerify, dwAllSectors - dwSecsPerVerify);
            }
        }

        NtClose(VolumeHandle);

        if (!NT_SUCCESS(status))
        {
            return FALSE;
        }
    }

    if (pFPR)
    {
        pFPR(100);
    }

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  FormatHD
// Purpose:   Formats the Xbox HD. Adapted from ntos\recovery\recovery.cpp.
//            NOTE: Doesn't touch Partition2 (Y -- the Dash partition).
//                  That must be done elsewhere.
// Arguments: None
// Return:    'true' if successful, 'false' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL FormatHD()
{
    IO_STATUS_BLOCK statusBlock;
    DISK_GEOMETRY diskGeometry;
    OBJECT_ATTRIBUTES oa;
    HANDLE hDisk;
    NTSTATUS Status;
    int i;

    // Open a handle to the entire hard disk (aka Partition0)
    InitializeObjectAttributes(&oa, (POBJECT_STRING) &PrimaryHarddisk,
                               OBJ_CASE_INSENSITIVE, NULL, NULL);

    Status = NtOpenFile(&hDisk, SYNCHRONIZE | GENERIC_ALL, &oa,
                        &statusBlock, 0, FILE_SYNCHRONOUS_IO_ALERT);
    if (!NT_SUCCESS(Status))
    {
        wsprintf(g_szResult, TEXT("Failed to open HD for format"));
        return FALSE;
    }

    Status = WriteFirstSectors(hDisk);

    if (!NT_SUCCESS(Status))
    {
        wsprintf(g_szResult, TEXT("WriteFirstSectors() failed (0x%08x)"),
                 Status);
        return FALSE;
    }

    NtClose(hDisk);

    for (DWORD nCurPartition = 1; nCurPartition <
         XDISK_FIRST_CACHE_PARTITION + *HalDiskCachePartitionCount;
         nCurPartition++)
    {
        if (nCurPartition != 2)
        {
            OCHAR szPartition[MAX_PATH];
            OBJECT_STRING PartitionString;

            DebugOutput(TEXT("Formatting partition %d\n"), nCurPartition);

            _snoprintf(szPartition, ARRAYSIZE(szPartition), HdPartitionX,
                       nCurPartition);

            RtlInitObjectString(&PartitionString, szPartition);

            if (!RecFormatFATVolume(&PartitionString, TRUE))
            {
                wsprintf(g_szResult, TEXT("Failed to format partition %d"),
                         nCurPartition);
                return FALSE;
            }
        }
    }

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WipeCacheDB
// Purpose:   Wipes the cache db sector.  Called after data has been copied
//            off of the utility drive.
// Arguments: None.
// Return:    'true' if successful, 'false' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL WipeCacheDB()
{
    IO_STATUS_BLOCK statusBlock;
    OBJECT_ATTRIBUTES oa;
    NTSTATUS Status;
    LARGE_INTEGER WriteOffset;
    HANDLE hDisk;
    BYTE rgbZeroes[XBOX_HD_SECTOR_SIZE];

    // Open a handle to the entire hard disk (aka Partition0)
    InitializeObjectAttributes(&oa, (POBJECT_STRING) &PrimaryHarddisk,
                               OBJ_CASE_INSENSITIVE, NULL, NULL);

    Status = NtOpenFile(&hDisk, SYNCHRONIZE | GENERIC_ALL, &oa,
                        &statusBlock, 0, FILE_SYNCHRONOUS_IO_ALERT);
    if (!NT_SUCCESS(Status))
    {
        wsprintf(g_szResult, TEXT("Failed to open HD for format"));
        return FALSE;
    }

    WriteOffset.QuadPart = XBOX_CACHE_DB_SECTOR_INDEX * XBOX_HD_SECTOR_SIZE;

    memset(rgbZeroes, 0, XBOX_HD_SECTOR_SIZE);

    Status = NtWriteFile(hDisk, NULL, NULL, NULL, &statusBlock, rgbZeroes,
                         XBOX_HD_SECTOR_SIZE, &WriteOffset);
    if (FAILED(Status))
        return Status;

    if (!NT_SUCCESS(Status))
    {
        wsprintf(g_szResult, TEXT("WipeCacheDB() failed (0x%08x)"),
                 Status);
        return FALSE;
    }

    NtClose(hDisk);

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  FormatYPartition
// Purpose:   Formats the Y partition (Partition 2; the dash partition)
// Arguments: None
// Return:    'true' if successful, 'false' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL FormatYPartition()
{
    OCHAR szPartition[MAX_PATH];
    OBJECT_STRING PartitionString;

    _snoprintf(szPartition, ARRAYSIZE(szPartition), HdPartitionX, 2);

    RtlInitObjectString(&PartitionString, szPartition);

    if (!RecFormatFATVolume(&PartitionString, TRUE))
    {
        wsprintf(g_szResult, TEXT("Failed to format partition 2"));
        return FALSE;
    }

    return TRUE;

}
