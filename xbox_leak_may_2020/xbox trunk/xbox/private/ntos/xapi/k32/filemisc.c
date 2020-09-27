/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    filemisc.c

Abstract:

    Misc file operations for Win32

Author:

    Mark Lucovsky (markl) 26-Sep-1990

Revision History:

--*/

#include <basedll.h>
#include "..\..\fatx\fat.h"

BOOL
APIENTRY
SetFileAttributes(
    PCOSTR lpFileName,
    DWORD dwFileAttributes
    )

/*++

Routine Description:

    The attributes of a file can be set using SetFileAttributes.

    This API provides the same functionality as DOS (int 21h, function
    43H with AL=1), and provides a subset of OS/2's DosSetFileInfo.

Arguments:

    lpFileName - Supplies the file name of the file whose attributes are to
        be set.

    dwFileAttributes - Specifies the file attributes to be set for the
        file.  Any combination of flags is acceptable except that all
        other flags override the normal file attribute,
        FILE_ATTRIBUTE_NORMAL.

        FileAttributes Flags:

        FILE_ATTRIBUTE_NORMAL - A normal file should be created.

        FILE_ATTRIBUTE_READONLY - A read-only file should be created.

        FILE_ATTRIBUTE_HIDDEN - A hidden file should be created.

        FILE_ATTRIBUTE_SYSTEM - A system file should be created.

        FILE_ATTRIBUTE_ARCHIVE - The file should be marked so that it
            will be archived.

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
    FILE_BASIC_INFORMATION BasicInfo;

    RtlInitObjectString(&FileName, lpFileName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    //
    // Open the file.
    //

    Status = NtOpenFile(
                &Handle,
                (ACCESS_MASK)FILE_WRITE_ATTRIBUTES | SYNCHRONIZE,
                &Obja,
                &IoStatusBlock,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                );

    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
        }

    //
    // Set the attributes
    //

    RtlZeroMemory(&BasicInfo,sizeof(BasicInfo));
    BasicInfo.FileAttributes = (dwFileAttributes & FILE_ATTRIBUTE_VALID_SET_FLAGS) | FILE_ATTRIBUTE_NORMAL;

    Status = NtSetInformationFile(
                Handle,
                &IoStatusBlock,
                &BasicInfo,
                sizeof(BasicInfo),
                FileBasicInformation
                );

    NtClose(Handle);
    if ( NT_SUCCESS(Status) ) {
        return TRUE;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }
}

DWORD
APIENTRY
GetFileAttributes(
    PCOSTR lpFileName
    )

/*++

Routine Description:

    The attributes of a file can be obtained using GetFileAttributes.

    This API provides the same functionality as DOS (int 21h, function
    43H with AL=0), and provides a subset of OS/2's DosQueryFileInfo.

Arguments:

    lpFileName - Supplies the file name of the file whose attributes are to
        be set.

Return Value:

    Not -1 - Returns the attributes of the specified file.  Valid
        returned attributes are:

        FILE_ATTRIBUTE_NORMAL - The file is a normal file.

        FILE_ATTRIBUTE_READONLY - The file is marked read-only.

        FILE_ATTRIBUTE_HIDDEN - The file is marked as hidden.

        FILE_ATTRIBUTE_SYSTEM - The file is marked as a system file.

        FILE_ATTRIBUTE_ARCHIVE - The file is marked for archive.

        FILE_ATTRIBUTE_DIRECTORY - The file is marked as a directory.

        FILE_ATTRIBUTE_VOLUME_LABEL - The file is marked as a volume lable.

    0xffffffff - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    OBJECT_STRING FileName;
    FILE_NETWORK_OPEN_INFORMATION NetworkInfo;

    RtlInitObjectString(&FileName, lpFileName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    //
    // Query the information about the file using the path-based NT service.
    //

    Status = NtQueryFullAttributesFile( &Obja, &NetworkInfo );
    if ( NT_SUCCESS(Status) ) {
        return NetworkInfo.FileAttributes;
        }
    else {
        XapiSetLastNTError(Status);
        return (DWORD)-1;
        }
}

BOOL
APIENTRY
GetFileAttributesEx(
    PCOSTR lpFileName,
    GET_FILEEX_INFO_LEVELS fInfoLevelId,
    LPVOID lpFileInformation
    )

/*++

Routine Description:

    The main attributes of a file can be obtained using GetFileAttributesEx.

Arguments:

    lpFileName - Supplies the file name of the file whose attributes are to
        be set.

    fInfoLevelId - Supplies the info level indicating the information to be
        returned about the file.

    lpFileInformation - Supplies a buffer to receive the specified information
        about the file.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.


--*/

{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    OBJECT_STRING FileName;
    FILE_NETWORK_OPEN_INFORMATION NetworkInfo;
    LPWIN32_FILE_ATTRIBUTE_DATA AttributeData;

    //
    // Check the parameters.  Note that for now there is only one info level,
    // so there's no special code here to determine what to do.
    //

    RIP_ON_NOT_TRUE("GetFileAttributesEx()",
                    (fInfoLevelId < GetFileExMaxInfoLevel && fInfoLevelId >= GetFileExInfoStandard));

    RtlInitObjectString(&FileName, lpFileName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    //
    // Query the information about the file using the path-based NT service.
    //

    Status = NtQueryFullAttributesFile( &Obja, &NetworkInfo );

    if ( NT_SUCCESS(Status) ) {
        AttributeData = (LPWIN32_FILE_ATTRIBUTE_DATA)lpFileInformation;
        AttributeData->dwFileAttributes = NetworkInfo.FileAttributes;
        AttributeData->ftCreationTime = *(PFILETIME)&NetworkInfo.CreationTime;
        AttributeData->ftLastAccessTime = *(PFILETIME)&NetworkInfo.LastAccessTime;
        AttributeData->ftLastWriteTime = *(PFILETIME)&NetworkInfo.LastWriteTime;
        AttributeData->nFileSizeHigh = NetworkInfo.EndOfFile.HighPart;
        AttributeData->nFileSizeLow = (DWORD)NetworkInfo.EndOfFile.LowPart;
        return TRUE;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }
}

BOOL
APIENTRY
MoveFile(
    PCOSTR lpExistingFileName,
    PCOSTR lpNewFileName
    )
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    OBJECT_STRING FileName;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_RENAME_INFORMATION RenameInfo;

    RtlInitObjectString(&FileName, lpExistingFileName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    //
    // Open the existing file for delete access.
    //

    Status = NtOpenFile( &Handle,
                         DELETE | SYNCHRONIZE,
                         &Obja,
                         &IoStatusBlock,
                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                         FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                         );

    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
        }

    //
    // Rename the file.
    //

    RenameInfo.ReplaceIfExists = FALSE;
    RenameInfo.RootDirectory = ObDosDevicesDirectory();

    RtlInitObjectString(&RenameInfo.FileName, lpNewFileName);

    Status = NtSetInformationFile(
                 Handle,
                 &IoStatusBlock,
                 &RenameInfo,
                 sizeof(RenameInfo),
                 FileRenameInformation
                 );

    NtClose(Handle);
    if ( NT_SUCCESS(Status) ) {
        return TRUE;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }
}

BOOL
APIENTRY
MoveFileEx(
    PCOSTR lpExistingFileName,
    PCOSTR lpNewFileName,
    DWORD dwFlags
    )

{
    return MoveFileWithProgress(  lpExistingFileName,
                                  lpNewFileName,
                                  (LPPROGRESS_ROUTINE)NULL,
                                  NULL,
                                  dwFlags );
}

BOOL
APIENTRY
MoveFileWithProgress(
    PCOSTR lpExistingFileName,
    PCOSTR lpNewFileName,
    LPPROGRESS_ROUTINE lpProgressRoutine OPTIONAL,
    LPVOID lpData OPTIONAL,
    DWORD dwFlags
    )

/*++

Routine Description:

    An existing file can be renamed using MoveFileWithProgressW.

Arguments:

    lpExistingFileName - Supplies the name of an existing file that is to be
        renamed.

    lpNewFileName - Supplies the new name for the existing file.  The new
        name must reside in the same file system/drive as the existing
        file and must not already exist.

    lpProgressRoutine - Supplies a callback routine that is notified.

    lpData - Supplies context data passed to the progress routine.

    dwFlags - Supplies optional flag bits to control the behavior of the
        rename.  The following bits are currently defined:

        MOVEFILE_REPLACE_EXISTING - if the new file name exists, replace
            it by renaming the old file name on top of the new file name.

        MOVEFILE_COPY_ALLOWED - if the new file name is on a different
            volume than the old file name, and causes the rename operation
            to fail, then setting this flag allows the MoveFileEx API
            call to simulate the rename with a call to CopyFile followed
            by a call to DeleteFile to the delete the old file if the
            CopyFile was successful.

        MOVEFILE_WRITE_THROUGH - perform the rename operation in such a
            way that the file has actually been moved on the disk before
            the API returns to the caller.  Note that this flag causes a
            flush at the end of a copy operation (if one were allowed and
            necessary), and has no effect if the rename operation is
            delayed until the next reboot.

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
    FILE_RENAME_INFORMATION RenameInfo;
    BOOL b;

    RtlInitObjectString(&FileName, lpExistingFileName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    //
    // Open the existing file for delete access.
    //

    Status = NtOpenFile( &Handle,
                         DELETE | SYNCHRONIZE,
                         &Obja,
                         &IoStatusBlock,
                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                         FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                         );

    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
    }

    //
    // Rename the file.
    //

    RenameInfo.ReplaceIfExists = ((dwFlags & MOVEFILE_REPLACE_EXISTING) != 0);
    RenameInfo.RootDirectory = ObDosDevicesDirectory();

    RtlInitObjectString(&RenameInfo.FileName, lpNewFileName);

    Status = NtSetInformationFile(
                 Handle,
                 &IoStatusBlock,
                 &RenameInfo,
                 sizeof(RenameInfo),
                 FileRenameInformation
                 );

    NtClose(Handle);
    if ( NT_SUCCESS(Status) ) {
        return TRUE;
    }
    else {

        if ((Status == STATUS_NOT_SAME_DEVICE) &&
            ((dwFlags & MOVEFILE_COPY_ALLOWED) != 0)) {

            b = CopyFileEx(lpExistingFileName,
                           lpNewFileName,
                           lpProgressRoutine,
                           lpData,
                           NULL,
                           (RenameInfo.ReplaceIfExists ? 0 : COPY_FILE_FAIL_IF_EXISTS) |
                               COPY_FILE_OPEN_SOURCE_FOR_WRITE
                           );

            //
            // the copy worked... Delete the source of the rename
            // if it fails, try a set attributes and then a delete
            //

            if (b && !DeleteFile( lpExistingFileName ) ) {

                //
                // If the delete fails, we will return true, but possibly
                // leave the source dangling
                //

                SetFileAttributes(lpExistingFileName,FILE_ATTRIBUTE_NORMAL);
                DeleteFile( lpExistingFileName );
            }

            return b;
        }

        XapiSetLastNTError(Status);
        return FALSE;
    }
}

BOOL
APIENTRY
DeleteFile(
    PCOSTR lpFileName
    )

/*++

    Routine Description:

    An existing file can be deleted using DeleteFile.

    This API provides the same functionality as DOS (int 21h, function 41H)
    and OS/2's DosDelete.

Arguments:

    lpFileName - Supplies the file name of the file to be deleted.

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
    FILE_DISPOSITION_INFORMATION Disposition;

    RtlInitObjectString(&FileName, lpFileName);

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        ObDosDevicesDirectory(),
        NULL
        );

    //
    // Open the file for delete access.
    //

    Status = NtOpenFile(
                 &Handle,
                 (ACCESS_MASK)DELETE,
                 &Obja,
                 &IoStatusBlock,
                 FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                 FILE_NON_DIRECTORY_FILE | FILE_OPEN_FOR_BACKUP_INTENT
                 );

    if ( !NT_SUCCESS(Status) ) {
        XapiSetLastNTError(Status);
        return FALSE;
        }

    //
    // Delete the file
    //
#undef DeleteFile
    Disposition.DeleteFile = TRUE;

    Status = NtSetInformationFile(
                 Handle,
                 &IoStatusBlock,
                 &Disposition,
                 sizeof(Disposition),
                 FileDispositionInformation
                 );

    NtClose(Handle);
    if ( NT_SUCCESS(Status) ) {
        return TRUE;
        }
    else {
        XapiSetLastNTError(Status);
        return FALSE;
        }
}

BOOL
WINAPI
XSetFileCacheSize(
    SIZE_T dwCacheSize
    )
/*++

Routine Description:

    This routine changes the number of bytes allocated to the file system cache.

Arguments:

    dwCacheSize - Supplies the number of bytes that should be allocated to the
        file system cache.  The number of bytes is rounded up to a multiple of
        the page size.

Return Value:

    Status of operation.

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    NTSTATUS status;
    PFN_COUNT NumberOfCachePages;

    //
    // Carefully compute the number of cache pages if the cache size is near
    // MAXULONG.
    //

    NumberOfCachePages = dwCacheSize >> PAGE_SHIFT;

    if (BYTE_OFFSET(dwCacheSize) != 0) {
        NumberOfCachePages++;
    }

    status = FscSetCacheSize(NumberOfCachePages);

    if (NT_SUCCESS(status)) {
        return TRUE;
    } else {
        XapiSetLastNTError(status);
        return FALSE;
    }
}

SIZE_T
WINAPI
XGetFileCacheSize(
    VOID
    )
/*++

Routine Description:

    This routine returns the number of bytes allocated to the file system cache.

Arguments:

    None.

Return Value:

    Number of bytes currently allocated to the file system cache.

--*/
{
    return FscGetCacheSize() << PAGE_SHIFT;
}



NTSTATUS
XapiGetDirectoryDisplayBlocks(
        HANDLE                      DirHandle,
        DWORD                       *TotalBlocks,
        PFILE_DIRECTORY_INFORMATION DirectoryInfo
        )
/*++
  Routine Description:
    Helper function for XDisplayBlocksFromPath.  Does the recursion of directories.
--*/
{
    NTSTATUS          status;
    IO_STATUS_BLOCK   ioStatusBlock;
    BOOLEAN           fFindFirst = TRUE;
    DWORD             fileCount = 0;
    HANDLE            subdirHandle;
    OBJECT_STRING     subdirName;
    OBJECT_ATTRIBUTES objectAttributes;
    DWORD             directorySize;

    do
    {
       status = NtQueryDirectoryFile(
                        DirHandle,
                        NULL,
                        NULL,
                        NULL,
                        &ioStatusBlock,
                        DirectoryInfo,
                        sizeof(FILE_DIRECTORY_INFORMATION)+sizeof(OCHAR)*254,
                        FileDirectoryInformation,
                        NULL,
                        fFindFirst
                        );

        fFindFirst = FALSE;

        //
        // If we succesfully found a file or directory then we have work to do
        //
        if (NT_SUCCESS(status))
        {
            //
            //  The fileCount and fileNameSizes will go into 
            //  computing the blocks required by this directory.
            //

            fileCount++;
            
            //
            //  If this is a directory we will need to recurse it.
            //
            if(FILE_ATTRIBUTE_DIRECTORY&DirectoryInfo->FileAttributes)
            {
            
                //
                //  Null terminate filename
                //

                DirectoryInfo->FileName[DirectoryInfo->FileNameLength/sizeof(OCHAR)] = OBJECT_NULL;

                //
                //  Create an OBJECT_STRING for the relative path of the directory
                //

                RtlInitObjectString(&subdirName, DirectoryInfo->FileName);

                //
                //  Initialize Obja with Handle of the parent and the relative path.
                //
                InitializeObjectAttributes(
                    &objectAttributes,
                    &subdirName,
                    OBJ_CASE_INSENSITIVE,
                    DirHandle,
                    NULL
                    );

                //
                // Open the directory
                //

                status = NtOpenFile(
                            &subdirHandle,
                            FILE_LIST_DIRECTORY | SYNCHRONIZE,
                            &objectAttributes,
                            &ioStatusBlock,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                            );

                if(NT_SUCCESS(status))
                {
                    status = XapiGetDirectoryDisplayBlocks(subdirHandle, TotalBlocks, DirectoryInfo);
                    NtClose(subdirHandle);
                }
            } else
            {
                 if(DirectoryInfo->EndOfFile.HighPart)
                 {
                    *TotalBlocks = MAX_DISPLAY_BLOCKS;
                    break;
                 } else
                 {
                    DWORD fileBytes, fileBlocks;
                    fileBytes = DirectoryInfo->EndOfFile.LowPart;
                    fileBlocks = (fileBytes/0x4000) + ((fileBytes%0x4000) ? 1 : 0);
                    if(fileBlocks >= MAX_DISPLAY_BLOCKS)
                    {
                        fileBlocks = MAX_DISPLAY_BLOCKS;
                    } else
                    {
                        *TotalBlocks += fileBlocks;
                    }
                 }
            }
            if(*TotalBlocks >= MAX_DISPLAY_BLOCKS)
            {
                *TotalBlocks = MAX_DISPLAY_BLOCKS;
                break;
            }
        }
    } while(NT_SUCCESS(status));

    
    //
    //  How much space the directory itself occupies.
    //  CAVEAT: directory entries don't shrink so 
    //  this calculation could underestimate the size
    //  if many files\or subdirectoires have been deleted.
    //
    directorySize = fileCount*sizeof(DIRENT);
    *TotalBlocks += (directorySize/0x4000);
    *TotalBlocks += (directorySize%0x4000) ? 1 : 0;
    if(*TotalBlocks >= MAX_DISPLAY_BLOCKS)
    {
       *TotalBlocks = MAX_DISPLAY_BLOCKS;
    }

    //
    //  Don't fail if we just found an empty directory
    //  or got to the end of it.
    //

    if(
        (status == STATUS_NO_MORE_FILES) ||
        (status == STATUS_NO_SUCH_FILE)
    ){
        status = STATUS_SUCCESS;
    }

    return status;
}


DWORD
WINAPI
XGetDisplayBlocks(
    PCOSTR lpPathName
    )
/*++

 Routine Description:

    Given a path to a file or directory, recursively computes the number
    of display blocks (bytes on disk\16kbytes) taken up by that file or
    directory and all of its subdirectories and files.

 Arguments:

    On Success:
      

 Return Value:

    On Success: Number of Display Blocks used by the path.  If the total is greater than 50,000,
    then MAX_DISPLAY_BLOCKS is returned.

    On Failure: 0, call GetLastError() for extended error information.

--*/
{
    OBJECT_STRING                 objectName;
    OBJECT_ATTRIBUTES             objectAttributes;
    FILE_NETWORK_OPEN_INFORMATION networkInfo;
    HANDLE                        dirHandle;
    IO_STATUS_BLOCK               ioStatusBlock;
    NTSTATUS                      status;
    DWORD                         totalBlocks;


    //
    //  Open a handle to the path.
    //

    RtlInitObjectString(&objectName, lpPathName);
    InitializeObjectAttributes(&objectAttributes,
                               &objectName,
                               OBJ_CASE_INSENSITIVE,
                               ObDosDevicesDirectory(),
                               NULL
                               );

    status = NtQueryFullAttributesFile(&objectAttributes, &networkInfo);
    
    if(NT_SUCCESS(status))
    {
        if(networkInfo.FileAttributes&FILE_ATTRIBUTE_DIRECTORY)
        {
            totalBlocks = 0;
            
            status = NtOpenFile(
                &dirHandle,
                FILE_LIST_DIRECTORY | SYNCHRONIZE,
                &objectAttributes,
                &ioStatusBlock,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                );

             if(NT_SUCCESS(status))
             {
                struct {FILE_DIRECTORY_INFORMATION DirInfo;OCHAR Names[255];} buffer;
                PFILE_DIRECTORY_INFORMATION directoryInfo = &buffer.DirInfo;
                status = XapiGetDirectoryDisplayBlocks(dirHandle, &totalBlocks, directoryInfo);
                NtClose(dirHandle);
             }
        } else
        {
            if(networkInfo.EndOfFile.HighPart)
            {
                totalBlocks = MAX_DISPLAY_BLOCKS;
            } else
            {
                DWORD fileBytes;
                fileBytes = networkInfo.EndOfFile.LowPart;
                totalBlocks = (fileBytes/0x4000) + ((fileBytes%0x4000) ? 1 : 0);
                if(totalBlocks > MAX_DISPLAY_BLOCKS) totalBlocks = MAX_DISPLAY_BLOCKS;
            }
        }
    }

    //
    //  Set an error on failure.
    //
    if(!NT_SUCCESS(status))
    {
        XapiSetLastNTError(status);
        return 0;
    }
    
    return totalBlocks;
}
