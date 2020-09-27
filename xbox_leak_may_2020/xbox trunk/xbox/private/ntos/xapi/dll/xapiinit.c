/*++

Copyright (c) 1999-2002  Microsoft Corporation

Module Name:

    init.c

Abstract:

    Initialization code for the win32 api equivalents in kernel mode for xbox

--*/

#include "dllp.h"
#include <xboxverp.h>

#define DASHBOARD_TITLE_ID   0xFFFE0000

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))

#include "xmeta.h"

COBJECT_STRING DDrive      = CONSTANT_OBJECT_STRING( OTEXT("\\??\\D:") );
COBJECT_STRING CdDevice    = CONSTANT_OBJECT_STRING( OTEXT("\\Device\\CdRom0") );
COBJECT_STRING MainVol     = CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\partition1\\") );
COBJECT_STRING TDrive      = CONSTANT_OBJECT_STRING( OTEXT("\\??\\T:") );
COBJECT_STRING TitleData   = CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\partition1\\TDATA") );
COBJECT_STRING UDrive      = CONSTANT_OBJECT_STRING( OTEXT("\\??\\U:") );
COBJECT_STRING UserData    = CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\partition1\\UDATA") );

NTSTATUS
XapiValidateDiskPartition(
    POBJECT_STRING PartitionName
    )
{
    return XapiValidateDiskPartitionEx(PartitionName, 16384);
}

NTSTATUS
XapiValidateDiskPartitionEx(
    POBJECT_STRING PartitionName,
    ULONG BytesPerCluster
    )
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_FS_SIZE_INFORMATION SizeInfo;

    InitializeObjectAttributes(
        &Obja,
        PartitionName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    //
    // Open the partition
    //

    Status = NtOpenFile(
                &Handle,
                (ACCESS_MASK)FILE_LIST_DIRECTORY | SYNCHRONIZE,
                &Obja,
                &IoStatusBlock,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE | FILE_OPEN_FOR_FREE_SPACE_QUERY
                );

    if (NT_SUCCESS(Status))
    {
        //
        // Determine the size parameters of the volume.
        //

        Status = NtQueryVolumeInformationFile(
                    Handle,
                    &IoStatusBlock,
                    &SizeInfo,
                    sizeof(SizeInfo),
                    FileFsSizeInformation
                    );

        NtClose(Handle);

        if (NT_SUCCESS(Status))
        {
            if (SizeInfo.BytesPerSector * SizeInfo.SectorsPerAllocationUnit !=
                BytesPerCluster)
            {
                Status = STATUS_UNRECOGNIZED_VOLUME;
            }
        }
    }

    return Status;
}

NTSTATUS
XapiCopySectionToFile(
    HANDLE SectionHandle,
    PSTR pszPathBuffer,
    UINT cchPathBuffer,
    PCSTR pcszFileName
    )
{
    OBJECT_STRING MetaFilePathString;
    OBJECT_ATTRIBUTES Obja;
    IO_STATUS_BLOCK IoStatusBlock;
    NTSTATUS Status;
    HANDLE MetaHandle;
    int nPathLen;

    ASSERT((INVALID_HANDLE_VALUE != SectionHandle) && pszPathBuffer && pcszFileName);

    nPathLen = ocslen(pszPathBuffer);

    lstrcpynO(&(pszPathBuffer[nPathLen]),
              pcszFileName,
              cchPathBuffer - nPathLen);

    RtlInitObjectString(&MetaFilePathString, pszPathBuffer);

    InitializeObjectAttributes(
        &Obja,
        &MetaFilePathString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    Status = NtCreateFile(&MetaHandle,
                          SYNCHRONIZE | GENERIC_WRITE,
                          &Obja,
                          &IoStatusBlock,
                          NULL,
                          FILE_ATTRIBUTE_SYSTEM,
                          FILE_SHARE_READ,
                          FILE_OPEN_IF,
                          FILE_WRITE_THROUGH | FILE_SYNCHRONOUS_IO_NONALERT
                          );

    pszPathBuffer[nPathLen] = OBJECT_NULL;

    if (NT_SUCCESS(Status))
    {
        FILE_NETWORK_OPEN_INFORMATION NetworkInfo;

        Status = NtQueryInformationFile(
                    MetaHandle,
                    &IoStatusBlock,
                    &NetworkInfo,
                    sizeof(NetworkInfo),
                    FileNetworkOpenInformation
                    );

        if (NT_SUCCESS(Status))
        {
            //
            // Only write if this is an empty file (first creation or first time
            // after an invalid, failed creation)
            //

            if (0 == NetworkInfo.EndOfFile.QuadPart)
            {
                PBYTE pbSection;

                //
                // Attempt to load the title info section
                //

                pbSection = XLoadSectionByHandle(SectionHandle);

                if (pbSection)
                {
                    Status = NtWriteFile(MetaHandle,
                                         NULL,
                                         NULL,
                                         NULL,
                                         &IoStatusBlock,
                                         pbSection,
                                         XGetSectionSize(SectionHandle),
                                         NULL
                                         );

                    if (!NT_SUCCESS(Status))
                    {
                        XDBGWRN("XAPI", "XapiCopySectionToFile() failed to write to file");
                    }

                    //
                    // Unload the title info section (we're done with it)
                    //

                    XFreeSectionByHandle(SectionHandle);
                }
            }
        }
        else
        {
            XDBGERR("XAPI", "XapiCopySectionToFile() failed to get meta file size");
        }

        NtClose(MetaHandle);
    }
    else if (STATUS_OBJECT_NAME_COLLISION == Status)
    {
        //
        // If the meta data image file already exists, we should continue
        // as if everything is fine (no need to rewrite this data)
        //

        Status = STATUS_SUCCESS;
    }

    return Status;
}


NTSTATUS
XapiMapLetterToDirectory(
    PCOBJECT_STRING pcDriveString,
    PCOBJECT_STRING pcPathString,
    PCOSTR pcszTitleId,
    BOOL fCreateDirectory,
    LPCWSTR pcszTitleName,
    BOOL fUpdateTimestamp
    )
{
    NTSTATUS Status;
    OCHAR szPathTemp[MAX_PATH];
    OBJECT_ATTRIBUTES Obja;
    IO_STATUS_BLOCK IoStatusBlock;
    OBJECT_STRING PerTitlePathString;
    HANDLE DirHandle;

    ASSERT(pcDriveString && pcPathString && pcszTitleId);

    InitializeObjectAttributes(
        &Obja,
        (POBJECT_STRING) pcPathString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );


    //
    //  The parent of the subirectory may not already exist
    //  so we open it first with 'FILE_OPEN_IF' to create it,
    //  just in case.  This mainly occurs when running
    //  recovery.
    //

    Status = NtCreateFile(
                &DirHandle,
                FILE_LIST_DIRECTORY | SYNCHRONIZE,
                &Obja,
                &IoStatusBlock,
                NULL,
                FILE_ATTRIBUTE_NORMAL,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                FILE_OPEN_IF,
                FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                );

    if (NT_SUCCESS(Status))
    {
        NtClose(DirHandle);
    }
    else if (STATUS_NOT_A_DIRECTORY == Status)
    {
        //
        //  XMountAlternateTitle when called on an MU passes a volume (not the
        //  volume's root directory) for pcDriveString.  So the above call will
        //  fail with STATUS_NOT_A_DIRECTORY.  This is OK though, so we change
        //  to success and continue.  Note that if we remove FILE_DIRECTORY_FILE
        //  from the above NtCreateFile, then recovery creates a file named UDATA
        //  instead of a directory and promptly crashes.
        //
        Status = STATUS_SUCCESS;
    }
    else
    {
        XDBGWRN("XAPI",
                "XapiMapLetterToDirectory(): NtCreateFile() \"%Z\" failed with 0x%08x",
                pcPathString,
                Status);
    }

    if (NT_SUCCESS(Status))
    {
        ASSERT(ocslen(pcszTitleId) < CCHMAX_HEX_DWORD);
        ASSERT(pcPathString->Length < (sizeof(szPathTemp) - (CCHMAX_HEX_DWORD * sizeof(OCHAR))));
        ocscpy(szPathTemp, pcPathString->Buffer);
        //
        //  Usually, pcDriveString usually does not have a '\\' on the end, the exception
        //  is on mounting MU's.  So here we check for '\\' and only append if necessary.
        //
        if(szPathTemp[(pcPathString->Length / sizeof(OCHAR))-1] != OTEXT('\\'))
        {
            szPathTemp[pcPathString->Length / sizeof(OCHAR)] = OTEXT('\\');
        } else
        {
            pcPathString->Length--;
        }
        ocscpy(&(szPathTemp[(pcPathString->Length + sizeof(OCHAR)) / sizeof(OCHAR)]), pcszTitleId);

        RtlInitObjectString(&PerTitlePathString, szPathTemp);

        InitializeObjectAttributes(
            &Obja,
            &PerTitlePathString,
            OBJ_CASE_INSENSITIVE,
            NULL,
            NULL
            );

        Status = NtCreateFile(
                    &DirHandle,
                    FILE_LIST_DIRECTORY | SYNCHRONIZE | FILE_GENERIC_WRITE,
                    &Obja,
                    &IoStatusBlock,
                    NULL,
                    FILE_ATTRIBUTE_NORMAL,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    fCreateDirectory ? FILE_OPEN_IF : FILE_OPEN,
                    FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                    );

        if (NT_SUCCESS(Status))
        {
            if (fUpdateTimestamp)
            {
                FILE_BASIC_INFORMATION BasicInfo;

                //
                // Zero all the time values we can set.
                //

                RtlZeroMemory(&BasicInfo, sizeof(BasicInfo));

                //
                // Set the last write times
                //

                KeQuerySystemTime(&BasicInfo.LastWriteTime);

                NtSetInformationFile(
                            DirHandle,
                            &IoStatusBlock,
                            &BasicInfo,
                            sizeof(BasicInfo),
                            FileBasicInformation
                            );
            }

            //
            // Only attempt to write meta data if non-null titlename and pubname strings were passed in
            // (these are not passed in if we're mounting another title's drive)
            //

            if (pcszTitleName)
            {
                HANDLE TitleInfoSection;
                HANDLE TitleImageSection;
                HANDLE SaveImageSection;

                TitleInfoSection = XGetSectionHandle("$$XTINFO");
                TitleImageSection = XGetSectionHandle("$$XTIMAGE");
                SaveImageSection = XGetSectionHandle("$$XSIMAGE");

                if ((INVALID_HANDLE_VALUE != TitleInfoSection) ||
                    (L'\0' != *pcszTitleName))
                {
                    //
                    // Create TITLEMETA.XBX file underneath the save game directory and
                    // write metadata information there
                    //

                    OBJECT_STRING MetaFilePathString;
                    HANDLE MetaHandle;
                    int nPathLen = ocslen(szPathTemp);

                    lstrcpynO(&(szPathTemp[nPathLen]),
                              g_cszTitleMetaFileName,
                              ARRAYSIZE(szPathTemp) - nPathLen);

                    RtlInitObjectString(&MetaFilePathString, szPathTemp);

                    InitializeObjectAttributes(
                        &Obja,
                        &MetaFilePathString,
                        OBJ_CASE_INSENSITIVE,
                        NULL,
                        NULL
                        );

                    Status = NtCreateFile(&MetaHandle,
                                          SYNCHRONIZE | GENERIC_WRITE,
                                          &Obja,
                                          &IoStatusBlock,
                                          NULL,
                                          FILE_ATTRIBUTE_SYSTEM,
                                          FILE_SHARE_READ,
                                          FILE_OPEN_IF,
                                          FILE_WRITE_THROUGH | FILE_SYNCHRONOUS_IO_NONALERT
                                          );

                    szPathTemp[nPathLen] = OBJECT_NULL;

                    if (NT_SUCCESS(Status))
                    {
                        FILE_NETWORK_OPEN_INFORMATION NetworkInfo;

                        Status = NtQueryInformationFile(
                                    MetaHandle,
                                    &IoStatusBlock,
                                    &NetworkInfo,
                                    sizeof(NetworkInfo),
                                    FileNetworkOpenInformation
                                    );

                        if (NT_SUCCESS(Status))
                        {
                            //
                            // Only write if this is an empty file (first creation or first time
                            // after an invalid, failed creation)
                            //

                            if (0 == NetworkInfo.EndOfFile.QuadPart)
                            {
                                PVOID pvTitleInfo = NULL;

                                //
                                // Attempt to load the title info section
                                //

                                if (INVALID_HANDLE_VALUE != TitleInfoSection)
                                {
                                    pvTitleInfo = XLoadSectionByHandle(TitleInfoSection);
                                }

                                if (pvTitleInfo)
                                {
                                    //
                                    // Write TITLEMETA.XBX as a copy of the title info section
                                    //

                                    Status = NtWriteFile(MetaHandle,
                                                         NULL,
                                                         NULL,
                                                         NULL,
                                                         &IoStatusBlock,
                                                         pvTitleInfo,
                                                         XGetSectionSize(TitleInfoSection),
                                                         NULL
                                                         );

                                    if (!NT_SUCCESS(Status))
                                    {
                                        XDBGWRN("XAPI", "XapiMapLetterToDirectory() failed to write title info file");
                                    }

                                    XFreeSectionByHandle(TitleInfoSection);
                                }
                                else
                                {
                                    //
                                    // Write TITLEMETA.XBX with the title name value from the cert
                                    //

                                    //
                                    // szBuffer size is: 1 signature WCHAR plus one line of metadata
                                    //

                                    WCHAR szBuffer[1 + MAX_METADATA_LINE];
                                    DWORD dwSize;

                                    dwSize = _snwprintf(szBuffer,
                                                        ARRAYSIZE(szBuffer),
                                                        L"%lc%ls%lc%ls%ls",
                                                        g_chUnicodeSignature,
                                                        g_cszTitleNameTag,
                                                        g_chEqual,
                                                        pcszTitleName,
                                                        g_cszCRLF);
                                    dwSize *= sizeof(WCHAR);

                                    Status = NtWriteFile(MetaHandle,
                                                         NULL,
                                                         NULL,
                                                         NULL,
                                                         &IoStatusBlock,
                                                         szBuffer,
                                                         dwSize,
                                                         NULL
                                                         );

                                    if (!NT_SUCCESS(Status))
                                    {
                                        XDBGWRN("XAPI", "XapiMapLetterToDirectory() failed to write title info file");
                                    }
                                }
                            }
                        }
                        else
                        {
                            XDBGERR("XAPI", "XapiMapLetterToDirectory() failed to get meta file size");
                        }

                        NtClose(MetaHandle);
                    }
                    else if (STATUS_OBJECT_NAME_COLLISION == Status)
                    {
                        //
                        // If the meta data file already exists, we should continue
                        // as if everything is fine (no need to rewrite this data)
                        //

                        Status = STATUS_SUCCESS;
                    }
                }

                if (NT_SUCCESS(Status) && (INVALID_HANDLE_VALUE != TitleImageSection))
                {
                    Status = XapiCopySectionToFile(TitleImageSection,
                                                   szPathTemp,
                                                   ARRAYSIZE(szPathTemp),
                                                   g_cszTitleImageFileName);
                }

                if (NT_SUCCESS(Status) && (INVALID_HANDLE_VALUE != SaveImageSection))
                {
                    Status = XapiCopySectionToFile(SaveImageSection,
                                                   szPathTemp,
                                                   ARRAYSIZE(szPathTemp),
                                                   g_cszSaveImageFileName);
                }
            }

            NtClose(DirHandle);
        }
        else if (fCreateDirectory)
        {
            XDBGWRN("XAPI",
                    "XapiMapLetterToDirectory(): NtCreateFile() \"%s\" failed with 0x%08x",
                    szPathTemp,
                    Status);
        }
    }

    if (NT_SUCCESS(Status))
    {
        Status = IoCreateSymbolicLink((POBJECT_STRING) pcDriveString, &PerTitlePathString);

        if (!NT_SUCCESS(Status))
        {
            XDBGERR("XAPI",
                    "XapiMapLetterToDirectory(): IoCreateSymbolicLink() failed \"%Z\"->\"%Z\"",
                    pcDriveString,
                    pcPathString);
        }
    }

    return Status;
}

NTSTATUS
XapiSetupPerTitleDriveLetters(
    DWORD dwTitleId,
    LPCWSTR pcszTitleName
    )
{
    NTSTATUS Status;
    OCHAR szTitleId[CCHMAX_HEX_DWORD];

    DwordToStringO(dwTitleId, szTitleId);

    Status = XapiMapLetterToDirectory(&TDrive,
                                      &TitleData,
                                      szTitleId,
                                      TRUE,
                                      NULL,
                                      FALSE);

    if (NT_SUCCESS(Status))
    {
        Status = XapiMapLetterToDirectory(&UDrive,
                                          &UserData,
                                          szTitleId,
                                          TRUE,
                                          pcszTitleName,
                                          FALSE);
    }

    return Status;
}

VOID
XapiBootToDash(
    DWORD dwReason,
    DWORD dwParameter1,
    DWORD dwParameter2
    )
{
    if (DASHBOARD_TITLE_ID != XeImageHeader()->Certificate->TitleID)
    {
        LD_LAUNCH_DASHBOARD LaunchDash;

        ZeroMemory(&LaunchDash, sizeof(LD_LAUNCH_DASHBOARD));

        LaunchDash.dwReason = dwReason;
        LaunchDash.dwContext = 0;
        LaunchDash.dwParameter1 = dwParameter1;
        LaunchDash.dwParameter2 = dwParameter2;

        XLaunchNewImage(NULL, (PLAUNCH_DATA) &LaunchDash);
    }
    else
    {
        //
        // Display Universal Error Message
        //

        HalReturnToFirmware(HalFatalErrorRebootRoutine);
    }
}

//
// Macros to swap the byte order of a USHORT or ULONG at compile time.
//

#define XapiConstantUshortByteSwap(ushort) \
    ((((USHORT)ushort) >> 8) + ((((USHORT)ushort) & 0x00FF) << 8))

#define XapiConstantUlongByteSwap(ulong) \
    ((((ULONG)ulong) >> 24) + ((((ULONG)ulong) & 0x00FF0000) >> 8) + \
    ((((ULONG)ulong) & 0x0000FF00) << 8) + ((((ULONG)ulong) & 0x000000FF) << 24))


NTSTATUS
XapiVerifyMediaInDrive(
    VOID
    )
/*++

Routine Description:

    Verifies that the DVD drive has in fact authenticated X2 media.

Arguments:

    None.

Return Value:

    None.

--*/

{
    NTSTATUS status;
    SCSI_PASS_THROUGH_DIRECT PassThrough;
    PCDB Cdb = (PCDB)&PassThrough.Cdb;
    DVDX2_AUTHENTICATION Authentication;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    IO_STATUS_BLOCK IoStatusBlock;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    InitializeObjectAttributes(
        &Obja,
        (POBJECT_STRING) &CdDevice,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    //
    // Open the CD/DVD device
    //

    status = NtOpenFile(
                &Handle,
                (ACCESS_MASK) GENERIC_READ | SYNCHRONIZE,
                &Obja,
                &IoStatusBlock,
                FILE_SHARE_READ,
                FILE_SYNCHRONOUS_IO_NONALERT
                );

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Prepare the SCSI pass through structure.
    //

    RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));

    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);

    //
    // Request the authentication page from the DVD-X2 drive.
    //

    PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
    PassThrough.DataBuffer = &Authentication;
    PassThrough.DataTransferLength = sizeof(DVDX2_AUTHENTICATION);

    RtlZeroMemory(Cdb, sizeof(CDB));

    Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SENSE10;
    Cdb->MODE_SENSE10.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
    *((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) =
        (USHORT)XapiConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION));

    RtlZeroMemory(&Authentication, sizeof(DVDX2_AUTHENTICATION));

    status = NtDeviceIoControlFile(Handle,
                                   NULL,
                                   NULL,
                                   NULL,
                                   &IoStatusBlock,
                                   IOCTL_SCSI_PASS_THROUGH_DIRECT,
                                   &PassThrough,
                                   sizeof(SCSI_PASS_THROUGH_DIRECT),
                                   NULL,
                                   0);

    NtClose(Handle);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Check if the DVD-X2 drive thinks that this is a valid CDF header.
    //

    if (Authentication.AuthenticationPage.CDFValid != DVDX2_CDF_VALID) {
        return STATUS_UNRECOGNIZED_MEDIA;
    }

    //
    // Check if the DVD-X2 drive already thinks that we're authenticated.
    //

    if ((Authentication.AuthenticationPage.PartitionArea != 0) &&
        (Authentication.AuthenticationPage.Authentication != 0)) {
        return STATUS_SUCCESS;
    }

    //
    // The DVD-X2 drive does not think that we're authenticated.
    //

    return STATUS_UNSUCCESSFUL;
}


VOID
XapiInitProcess(
    VOID
    )
/*++

Routine Description:

    Initializes the Xapi process, loads the title and starts its first thread.

Arguments:

    None.

Return Value:

    None.

--*/
{
    HANDLE ThreadHandle;
    /* ULONG_PTR EntryPoint; */
    ULONG ProcessHeapFlags;
    RTL_HEAP_PARAMETERS HeapParameters;
    HANDLE hthread;
    USHORT CharIndex;
    DWORD dwReason = XLD_LAUNCH_DASHBOARD_MAIN_MENU;
    DWORD dwParameter1;
    DWORD dwParameter2 = 0;
    NTSTATUS status;

    // Initialize the auto-power-down feature.
    XapiInitAutoPowerDown();

    RtlZeroMemory( &HeapParameters, sizeof( HeapParameters ) );
    ProcessHeapFlags = HEAP_GROWABLE | HEAP_CLASS_0;
    HeapParameters.Length = sizeof( HeapParameters );

    // Note: there are some more parameters in the image that normal Win32 apps use to
    // modify ProcessHeapFlags and HeapParameters. Here, we just use the heap reserve/commit
    // sizes specified
    XapiProcessHeap = RtlCreateHeap(ProcessHeapFlags,
                                    NULL,
                                    XeImageHeader()->SizeOfHeapReserve,
                                    XeImageHeader()->SizeOfHeapCommit,
                                    NULL,
                                    &HeapParameters);

    if (XapiProcessHeap == NULL)
    {
        dwReason = XLD_LAUNCH_DASHBOARD_ERROR;
        dwParameter1 = XLD_ERROR_INVALID_XBE;
        goto handle_error;
    }

    if ((XeImageHeader()->Certificate->AllowedMediaTypes & XBEIMAGE_MEDIA_TYPE_MEDIA_MASK)
        == XBEIMAGE_MEDIA_TYPE_DVD_X2)
    {
        //
        // This title is only allowed to run on X2 media - double check that the drive
        // has mounted X2 media:
        //

        if (!NT_SUCCESS(XapiVerifyMediaInDrive()))
        {
            //
            // Don't display a dash error message - we want to avoid showing an
            // inapplicable errorduring race conditions where valid media was ejected
            // while this code was verifying the media
            //

            XapiBootToDash(XLD_LAUNCH_DASHBOARD_MAIN_MENU, 0, 0);
            return;
        }
    }

    if (!(XeImageHeader()->InitFlags & XINIT_NO_SETUP_HARD_DISK))
    {
        // Get the hard disk ready
        if (!NT_SUCCESS(XapiValidateDiskPartition((POBJECT_STRING) &MainVol)))
        {
            dwReason = XLD_LAUNCH_DASHBOARD_ERROR;
            dwParameter1 = XLD_ERROR_INVALID_HARD_DISK;
            goto handle_error;
        }
    }

    // Give the DVD/CD drive a drive letter
    if (!NT_SUCCESS(status = IoCreateSymbolicLink((POBJECT_STRING) &DDrive,
                                                  (POBJECT_STRING) &CdDevice)))
    {
        XDBGWRN("XAPI",
                "The D: drive is not assigned to the CD/DVD device - error 0x%08x",
                status);

        //
        // Devkits will tend to already have the D: drive assigned, so we'll ignore
        // this error, but reboot to the dash in all other cases...
        //

        if (STATUS_OBJECT_NAME_COLLISION != status)
        {
            dwReason = XLD_LAUNCH_DASHBOARD_ERROR;
            dwParameter1 = XLD_ERROR_INVALID_XBE;
            goto handle_error;
        }
    }

    if ((!(XeImageHeader()->InitFlags & XINIT_NO_SETUP_HARD_DISK)) &&
        (!(XeImageHeader()->InitFlags & XINIT_DONT_MODIFY_HARD_DISK)))
    {
        // After we've initialized everything, start the process
        if (!NT_SUCCESS(status = XapiSetupPerTitleDriveLetters(
                XeImageHeader()->Certificate->TitleID,
                XeImageHeader()->Certificate->TitleName)))
        {
            XDBGWRN("XAPI", "Could not set up per-title drive letters");

            if (STATUS_DISK_FULL == status)
            {
                dwReason = XLD_LAUNCH_DASHBOARD_MEMORY;
                dwParameter1 = (DWORD) 'T'; // drive letter
                dwParameter2 = 16; // blocks needed
            }
            else
            {
                dwReason = XLD_LAUNCH_DASHBOARD_ERROR;
                dwParameter1 = XLD_ERROR_INVALID_HARD_DISK;
            }
            goto handle_error;
        }
    }

    if ((!(XeImageHeader()->InitFlags & XINIT_NO_SETUP_HARD_DISK)) &&
        (XeImageHeader()->InitFlags & XINIT_MOUNT_UTILITY_DRIVE))
    {
        if (!XMountUtilityDrive(XeImageHeader()->InitFlags & XINIT_FORMAT_UTILITY_DRIVE))
        {
            XapiBootToDash(XLD_LAUNCH_DASHBOARD_ERROR,
                           XLD_ERROR_INVALID_HARD_DISK,
                           0);
        }
    }

    //
    // Check to see if parental control settings should prevent this title from
    // running on this box
    //

    if (XeImageHeader()->Certificate->GameRatings < XGetParentalControlSetting())
    {
        XDBGWRN("XAPI", "The box's parental control setting prohibits this title from starting");
        XDBGWRN("XAPI", "Go to the Xbox dashboard to change the box's parental control setting");

        dwReason = XLD_LAUNCH_DASHBOARD_ERROR;
        dwParameter1 = XLD_ERROR_XBE_PARENTAL_CONTROL;
        dwParameter2 = XeImageHeader()->Certificate->GameRatings;
        goto handle_error;
    }

    XDBGTRC("XAPI", "InitProcess: SizeOfStack=%d", XeImageHeader()->SizeOfStackCommit);

handle_error:
    if (XLD_LAUNCH_DASHBOARD_MAIN_MENU != dwReason)
    {
        XapiBootToDash(dwReason,
                       dwParameter1,
                       dwParameter2);
    }
}
