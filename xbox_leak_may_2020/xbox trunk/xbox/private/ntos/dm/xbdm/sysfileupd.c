/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    sysfileupd.c

Abstract:

    This module implements routines to update system files on XDK.

--*/

#include "dmp.h"
#undef DeleteFile

NTSTATUS
ReplaceFile(
    IN OBJECT_STRING* NewFileName,
    IN HANDLE ExistingHandle
    )
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES ObjA;
    HANDLE Handle;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_BASIC_INFORMATION BasicInfo;
    FILE_RENAME_INFORMATION RenameInfo;

    // Clear read-only file attribute as needed
    InitializeObjectAttributes(&ObjA, NewFileName, OBJ_CASE_INSENSITIVE, 0, 0);

    Status = NtOpenFile(&Handle, (ACCESS_MASK)FILE_WRITE_ATTRIBUTES | SYNCHRONIZE,
                &ObjA, &IoStatusBlock, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT);

    if (NT_SUCCESS(Status)) {
        RtlZeroMemory(&BasicInfo,sizeof(BasicInfo));
        BasicInfo.FileAttributes = FILE_ATTRIBUTE_NORMAL;
        NtSetInformationFile(Handle, &IoStatusBlock, &BasicInfo, sizeof(BasicInfo),
            FileBasicInformation);
        NtClose(Handle);
    }

    // Rename the file
    RenameInfo.ReplaceIfExists = TRUE;
    RenameInfo.RootDirectory = 0;
    RenameInfo.FileName.Length = NewFileName->Length;
    RenameInfo.FileName.MaximumLength = NewFileName->MaximumLength;
    RenameInfo.FileName.Buffer = NewFileName->Buffer;

    Status = NtSetInformationFile(ExistingHandle, &IoStatusBlock, &RenameInfo,
        sizeof(RenameInfo), FileRenameInformation);

    return Status;
}

HRESULT HrReceiveSystemFile(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;
    FILE_DISPOSITION_INFORMATION fdi;
    IO_STATUS_BLOCK IoStatusBlock;
    NTSTATUS Status;

    if (pdmcc->DataSize) {
        pccs->sysupd.BytesReceived += pdmcc->DataSize;
        ASSERT(pccs->sysupd.BytesReceived <= DISK_BUF_SIZE);
        pdmcc->BytesRemaining -= pdmcc->DataSize;
    } else if (pdmcc->BytesRemaining) {
        DmFreePool(pccs->sysupd.FileBuffer);
        DmFreePool(pccs->sysupd.FileName.Buffer);
        fdi.DeleteFile = TRUE;
        NtSetInformationFile(pccs->sysupd.FileHandle, &IoStatusBlock, &fdi,
            sizeof(fdi), FileDispositionInformation);
        _snprintf(szResponse, cchResponse, "Unexpected data size");
        szResponse[cchResponse - 1] = 0;
        return E_UNEXPECTED;
    }

    // See if we're ready for a disk write
    if (!pdmcc->BytesRemaining || pccs->sysupd.BytesReceived == DISK_BUF_SIZE) {

        pccs->sysupd.Crc = Crc32(pccs->sysupd.Crc, pccs->sysupd.FileBuffer,
            pccs->sysupd.BytesReceived);

        Status = NtWriteFile(pccs->sysupd.FileHandle, NULL, NULL,
            NULL, &IoStatusBlock, pccs->sysupd.FileBuffer,
            pccs->sysupd.BytesReceived, NULL);

        if (!NT_SUCCESS(Status)) {
cleanup:
            fdi.DeleteFile = TRUE;
            NtSetInformationFile(pccs->sysupd.FileHandle, &IoStatusBlock,
                &fdi, sizeof(fdi), FileDispositionInformation);
            NtClose(pccs->sysupd.FileHandle);
            pccs->sysupd.FileHandle = NULL;
            _snprintf(szResponse, cchResponse, "File write error");
            szResponse[cchResponse - 1] = 0;
            return HrFromStatus(Status, XBDM_CANNOTCREATE);
        } else if (!pdmcc->BytesRemaining) {
            if (pccs->sysupd.Crc != pccs->sysupd.ExpectedCrc) {
                _snprintf(szResponse, cchResponse, "Invalid CRC");
                szResponse[cchResponse - 1] = 0;
                goto cleanup;
            }
            Status = ReplaceFile(&pccs->sysupd.FileName, pccs->sysupd.FileHandle);
            if (!NT_SUCCESS(Status)) {
                _snprintf(szResponse, cchResponse, "Replace file failed (0x%X)", Status);
                szResponse[cchResponse - 1] = 0;
                goto cleanup;
            }
            NtClose(pccs->sysupd.FileHandle);
        }

        pccs->sysupd.BytesReceived = 0;

        if (!pdmcc->BytesRemaining) {
            DmFreePool(pccs->sysupd.FileBuffer);
            pccs->sysupd.FileBuffer = NULL;
        }
    }

    // Make sure the buffer pointer is set up
    pdmcc->Buffer = pccs->sysupd.FileBuffer + pccs->sysupd.BytesReceived;
    pdmcc->BufferSize = DISK_BUF_SIZE - pccs->sysupd.BytesReceived;
    return XBDM_NOERR;
}

VOID
CreateDirectoryFromPath(
    IN CONST OBJECT_STRING* Path
    )
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES ObjA;
    HANDLE Handle;
    OBJECT_STRING FileName;
    IO_STATUS_BLOCK IoStatusBlock;
    WORD Length;

    FileName.Buffer = Path->Buffer;
    Length = FileName.Length = Path->Length;
    FileName.MaximumLength = Path->MaximumLength;
    InitializeObjectAttributes(&ObjA, &FileName, OBJ_CASE_INSENSITIVE, 0, 0);

    // REVIEW: there should be a better way to do this
    for (FileName.Length=1; FileName.Length<Length; FileName.Length++) {
        if (FileName.Buffer[FileName.Length] != '\\') {
            continue;
        }

        Status = NtCreateFile(&Handle, FILE_LIST_DIRECTORY | SYNCHRONIZE,
            &ObjA, &IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL,
            FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_CREATE,
            FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | \
            FILE_OPEN_FOR_BACKUP_INTENT);

        if (NT_SUCCESS(Status)) {
            NtClose(Handle);
        }
    }
}

HRESULT HrUpdateSystemFile(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    CCS* pccs;
    SIZE_T ImageSize;
    NTSTATUS Status;
    OBJECT_ATTRIBUTES ObjA;
    OBJECT_STRING ObjS;
    OCHAR buffer[512];
    LARGE_INTEGER FileSize;
    OCHAR* psz;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_END_OF_FILE_INFORMATION feof;
    FILE_DISPOSITION_INFORMATION fdi;
    HRESULT hr;

    if (!pdmcc) {
        return E_FAIL;
    }

    pccs = (CCS*)pdmcc->CustomData;

    // Get file size and CRC32 of entire file
    if (!FGetNamedDwParam(sz, "size", &pdmcc->BytesRemaining, szResp) || \
        !FGetNamedDwParam(sz, "crc", &pccs->sysupd.ExpectedCrc, szResp)) {
        _snprintf(szResp, cchResp, "Invalid parameter(s)");
        szResp[cchResp - 1] = 0;
        return E_FAIL;
    }

    // Allocate buffer for file name and file buffer
    pccs->sysupd.FileBuffer = DmAllocatePoolWithTag(
        DISK_BUF_SIZE + sizeof(buffer), 'ufsx');
    if (!pccs->sysupd.FileBuffer) {
        _snprintf(szResp, cchResp, "Not enough memory");
        szResp[cchResp - 1] = 0;
        return E_OUTOFMEMORY;
    }

    pccs->sysupd.FileName.Length = 0;
    pccs->sysupd.FileName.MaximumLength = sizeof(buffer);
    pccs->sysupd.FileName.Buffer = (PCHAR)\
        ((ULONG_PTR)pccs->sysupd.FileBuffer + DISK_BUF_SIZE);

    if (!FGetSzParam(sz, "name", pccs->sysupd.FileName.Buffer,
            pccs->sysupd.FileName.MaximumLength/sizeof(OCHAR))) {
        _snprintf(szResp, cchResp, "missing or invalid file name");
        szResp[cchResp - 1] = 0;
        hr = E_FAIL;
        goto cleanup;
    }

    pccs->sysupd.FileName.Length = strlen(pccs->sysupd.FileName.Buffer) * \
        sizeof(OCHAR);
    ASSERT(pccs->sysupd.FileName.Length < sizeof(buffer));

    // Create temporary file which later will be renamed to the real target
    strcpy(buffer, pccs->sysupd.FileName.Buffer);
    psz = strrchr(buffer, '\\');

    if (!psz || sizeof(buffer) - ((ULONG_PTR)psz - (ULONG_PTR)buffer + 1) < \
        sizeof("\\temp1234.tmp")) {
        _snprintf(szResp, cchResp, "Invalid file name");
        szResp[cchResp - 1] = 0;
        hr = E_INVALIDARG;
        goto cleanup;
    }

    sprintf(psz, "\\temp%04x.tmp", NtGetTickCount() & 0xFFFF);
    RtlInitObjectString(&ObjS, buffer);
    InitializeObjectAttributes(&ObjA, &ObjS, OBJ_CASE_INSENSITIVE, 0, 0);
    CreateDirectoryFromPath(&ObjS);

    FileSize.HighPart = 0;
    FileSize.LowPart = pdmcc->BytesRemaining;

    Status = NtCreateFile(&pccs->sysupd.FileHandle, GENERIC_WRITE | DELETE | SYNCHRONIZE,
        &ObjA, &IoStatusBlock, &FileSize, FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_CREATE,
        FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT);

    if (NT_SUCCESS(Status)) {

        // Commit the size of the file
        feof.EndOfFile = FileSize;

        Status = NtSetInformationFile(pccs->sysupd.FileHandle, &IoStatusBlock, &feof,
            sizeof(feof), FileEndOfFileInformation);

        if (NT_SUCCESS(Status)) {
            pccs->sysupd.BytesReceived = 0;
            pccs->sysupd.Crc = ~0UL;
            pdmcc->Buffer = pccs->sysupd.FileBuffer;
            pdmcc->BufferSize = DISK_BUF_SIZE;
            pdmcc->HandlingFunction = HrReceiveSystemFile;
            return XBDM_READYFORBIN;
        } else {
            // Mark the file for deletion and close the file
            fdi.DeleteFile = TRUE;
            NtSetInformationFile(&pccs->sysupd.FileHandle, &IoStatusBlock, &fdi, sizeof(fdi),
                FileDispositionInformation);
            NtClose(pccs->sysupd.FileHandle);
            pccs->sysupd.FileHandle = NULL;
            _snprintf(szResp, cchResp, "Disk full");
            szResp[cchResp - 1] = 0;
        }
    }

    hr = HrFromStatus(Status, XBDM_CANNOTCREATE);

cleanup:
    DmFreePool(pccs->sysupd.FileBuffer);
    return hr;
}

