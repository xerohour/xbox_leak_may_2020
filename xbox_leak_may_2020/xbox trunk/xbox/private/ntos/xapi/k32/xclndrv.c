/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

   xclndrv.c

Abstract:

    API's that clean drives by reclaiming empty title directories
    and corrupt games.

Author:

    Mitchell S. Dernis (mitchd) 05-Jun-2001

Revision History:

--*/

#include "basedll.h"



#ifdef XAPILIBP

DWORD
WINAPI
XCleanMUFromRoot(
    IN OCHAR chDrive,
    PCOSTR pszPreserveDir OPTIONAL
    )
/*++
  Routine Description:
    From a root mounted MU drive, clean all the title directories that do not
    contain games.  Except do not cleanup the directory specified by pszPreserveDir
  Arguments:
    chDrive - drive letter of root mounted MU.
    pszPreserveDir OPTIONAL - title directory to preserve, even if it has no games.
--*/
{
    NTSTATUS status;
    OCHAR szDosDevice[25];
    
    //
    // Removing the 0x20 bit will make lower case characters uppercase
    //

    chDrive &= ~0x20;

    //
    //  Debug check to make sure that this is an MU it should be root mounted.
    //
#if DBG
    if (
         ((chDrive < MU_FIRST_DRIVE) || (chDrive > MU_LAST_DRIVE))
    )
    {
        RIP("XCleanDrive() invalid drive letter parameter");
    }
#endif // DBG
    
    //
    //  Create a string with the full dos name
    //
    soprintf(szDosDevice, OTEXT("\\??\\%c:\\"), chDrive);

    //
    // Call XapiNukeSubdirsWithoutSubdirs.  
    //
    return XapiNukeEmptySubdirs(szDosDevice, pszPreserveDir, TRUE);
}

DWORD 
WINAPI
XCleanDrive(
    IN OCHAR chDrive
)
/*++
  Routine Description:
    From a mounted title drive (T, or any mounted MU, but not a root mounted MU),
    find all the title areas on the drive and cleanup any directory that does not
    contain valid saved games.  Except do not cleanup the directory of the current
    title.

  Arguments:
    chDrive - drive letter to cleanup

  Comments:
    XCleanDrive and DWORD XCleanMUFromRoot are both tasked with finding the root of a volume of
    title directories, and then cleaning them up.  XapiNukeSubdirsWithoutSubdirs
    does the work.
--*/
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    OBJECT_STRING ObjectName;
    OCHAR szDosDevice[25];
    OCHAR Target[MAX_PATH+1];
    ULONG TargetLength;
    OCHAR oszPreserveDir[CCHMAX_HEX_DWORD];
    OBJECT_STRING ObjectTarget;
    HANDLE Handle;
    
    //
    // Removing the 0x20 bit will make lower case characters uppercase
    //

    chDrive &= ~0x20;

    //
    //  Debug check to make sure that this is an MU or the T: drive.
    //
#if DBG
    if ((chDrive < MU_FIRST_DRIVE) || (chDrive > MU_LAST_DRIVE))
    {
        RIP("XCleanDrive() invalid drive letter parameter");
    }
#endif // DBG
    
    //
    //  Open the symbolic link, at the real path
    //
    soprintf(szDosDevice, OTEXT("\\??\\%c:"), chDrive);

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
    ObjectTarget.MaximumLength = sizeof(Target)-1;

    status = NtQuerySymbolicLinkObject(Handle, &ObjectTarget, &TargetLength);

    NtClose(Handle);

    if (!NT_SUCCESS(status))
    {
        return RtlNtStatusToDosError(status);
    }

    //
    //  The title directory must be of length CCHMAX_HEX_DWORD.
    //
    if ((TargetLength < CCHMAX_HEX_DWORD) ||
        ('\\' != Target[TargetLength - (CCHMAX_HEX_DWORD)]))
    {
        return ERROR_INVALID_DRIVE;
    }

    //
    //  Break the Target into two strings.  The root path,
    //  and the current title library.
    //
    Target[TargetLength] = OBJECT_NULL;
    ocscpy(oszPreserveDir, &Target[TargetLength - (CCHMAX_HEX_DWORD)+1]);
    Target[TargetLength - (CCHMAX_HEX_DWORD)+1] = OBJECT_NULL; //remove subdir - maintain '\\'

    //
    // Call XapiNukeEmptySubdirs.  For all drives but T, we nuke the directory even if it contains  
    // files, as long as it has no subdirectories.
    //
    return XapiNukeEmptySubdirs(Target, oszPreserveDir, (HD_TDATA_DRIVE == chDrive) ? FALSE : TRUE );
}


DWORD 
XapiNukeEmptySubdirs(
    IN PCOSTR  pszDrivePath,
    IN PCOSTR  pszPreserveDir OPTIONAL,
    IN BOOLEAN fNukeFiles
    )
/*++
  Routine Description:
    
    Nukes all "empty} subdirectories under pszDrivePath, excluding the
    directory optionally specified by pszPreserveDir.

  Arguments:

    pszDrivePath   - path to directory that contains subdirectories.
    pszPreserveDir - optional relative path to single subdirectory
                     to preserve even if it does not have any subdirectories.
    fNukeFiles     - if true, it consider subdirectories empty even if they 
                     contain files, as long as they do not contain more
                     subdirectories.
  Comments:
    The motivation for having such a bizarre routine in XAPI is as a helper for cleaning
    up title directories.

    The primary clients are XCleanDrive and XCleanMUFromRoot.  The idea is
    to nuke empty title directories that do not contain useful user data.  There
    are two cases: U:\ (and all the MU's) and T:\.  In both cases, every time a
    game boots (and in the case of MU's mounts them) a subdirectory is created for
    that title.  For U:\ (and the MUs) each saved game is in a subdirectory.  Files
    that are not in a subdirectory are considered expandable.  In the T:\ case even
    individual files should be saved.  Thus fNukeFiles is passed as FALSE when
    cleaning the T:\ drive and TRUE for the other drives.

    pszPreserveDir is used to avoid deleting the title directory of the current title.
    It can be NULL.  The dashboard does this when cleaning up MU's.

--*/
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE hRoot;
    ULONG EndingIndex;
    OBJECT_STRING DirectoryName;
    IO_STATUS_BLOCK IoStatusBlock;
    BOOLEAN fFindFirstFile = TRUE;
    struct {FILE_DIRECTORY_INFORMATION DirInfo;OCHAR Names[255];} Buffer;
    PFILE_DIRECTORY_INFORMATION DirectoryInfo = &Buffer.DirInfo;
    

    RtlInitObjectString(&DirectoryName, pszDrivePath);

    InitializeObjectAttributes(
        &Obja,
        &DirectoryName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    //
    // Open the directory for list access.
    //

    Status = NtOpenFile(
                 &hRoot,
                 FILE_LIST_DIRECTORY | SYNCHRONIZE,
                 &Obja,
                 &IoStatusBlock,
                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                 FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                 );

    //
    // Loop over and find all the subdirectories
    //
    while(NT_SUCCESS(Status))
    {
        Status = NtQueryDirectoryFile(
                 hRoot,
                 NULL,
                 NULL,
                 NULL,
                 &IoStatusBlock,
                 DirectoryInfo,
                 sizeof(Buffer)-sizeof(OCHAR),
                 FileDirectoryInformation,
                 NULL,
                 fFindFirstFile
                 );
        fFindFirstFile = FALSE;
        //
        //  If we found a file, we need to determine if it is a subdirectory
        //  without subdirectories.
        //
        if(NT_SUCCESS(Status))
        {
           //
           //  Proceed only if it is a directory
           //
           if(DirectoryInfo->FileAttributes&FILE_ATTRIBUTE_DIRECTORY)
           {
               //
               //  Null terminate the name in place.  Note that we passed
               //  NtQueryDirectoryFile a small buffer length to save room for a NULL.
               //
               DirectoryInfo->FileName[DirectoryInfo->FileNameLength/sizeof(OCHAR)] = OBJECT_NULL;

               //
               //  Proceed only if we were not passed a directory to preserve, or
               //  if this is not that directory.
               //
               if(!pszPreserveDir || ocscmp(DirectoryInfo->FileName, pszPreserveDir))
               {
                   HANDLE   hSubDir;
                   NTSTATUS SubDirStatus;
                   BOOLEAN  fFindFirstSubDir = TRUE;
                   
                   //
                   // Open the subdirectory with FILE_LIST_DIRECTORY access
                   // to see if it has any subdirectories, and with DELETE and 
                   // FILE_WRITE_ATTRIBUTES access.
                   // 
                   // DELETE in case we need to delete it.  FILE_WRITE_ATTRIBUTES
                   // in case it turns out to have FILE_ATTRIBUTE_READONLY set
                   // when we go to DELETE it.
                   //
                   RtlInitObjectString(&DirectoryName, DirectoryInfo->FileName);
                   InitializeObjectAttributes(
                     &Obja,
                     &DirectoryName,
                     OBJ_CASE_INSENSITIVE,
                     hRoot,
                     NULL
                     );
                   SubDirStatus = NtOpenFile(
                         &hSubDir,
                         FILE_LIST_DIRECTORY | DELETE | FILE_WRITE_ATTRIBUTES |SYNCHRONIZE,
                         &Obja,
                         &IoStatusBlock,
                         FILE_SHARE_READ | FILE_SHARE_WRITE,
                         FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                         );

                   //
                   // Loop while successful, except we break if we 
                   // as soon as we know that we want to preserve
                   // this subdirectory.
                   //
                   while(NT_SUCCESS(SubDirStatus))
                   {
                      SubDirStatus = NtQueryDirectoryFile(
                                         hSubDir,
                                         NULL,
                                         NULL,
                                         NULL,
                                         &IoStatusBlock,
                                         DirectoryInfo,
                                         sizeof(Buffer)-sizeof(OCHAR),
                                         FileDirectoryInformation,
                                         NULL,
                                         fFindFirstSubDir
                                         );
                       fFindFirstSubDir = FALSE;
                       
                       if(NT_SUCCESS(SubDirStatus))
                       {
                           //
                           //  If the fNukeFiles is not set, or 
                           //  if this is a directory, then we break.
                           //  This will leave the loop with Status ==
                           //  STATUS_SUCCESS, which prevents this subdirectory 
                           //  from being nuked.
                           //
                           if(!fNukeFiles || DirectoryInfo->FileAttributes&FILE_ATTRIBUTE_DIRECTORY)
                           {
                             break;
                           }
                       }
                   }

                   //
                   //  If !fFindFirstSubDir, then we have an opened
                   //  hSubDir handle.
                   //
                   if(!fFindFirstSubDir)
                   {
                       //
                       //   If we got to the end of the loop and didn't
                       //   find a file (if fNukeFile not set) or a
                       //   subdirectory in the subdirectory, then
                       //   nuke it.
                       //
                       if(
                           (STATUS_NO_MORE_FILES == SubDirStatus) ||
                           (STATUS_NO_SUCH_FILE == SubDirStatus)
                        )
                       {
                           XapiNukeDirectoryFromHandle(hSubDir, DirectoryInfo);    
                       }
                       //
                       //   We are done with the subdirectory, if we
                       //   called XapiNukeDirectoryFromHandle on the
                       //   handle, this finishes the job.  Otherwise,
                       //   the directory should still be around.
                       //
                       NtClose(hSubDir);
                   }
               }
           }
        }
    }

    //
    //  !fFindFirstFile implies that we manage open the root directory.
    //  If so we know that STATUS_NO_SUCH_FILE refers to the attempt
    //  to find subdirectories, and not to opening the root.
    //  Doctor error codes, and close the root.
    //
    if(!fFindFirstFile)
    {
        //
        //  The normal "successful" thing is that Status is STATUS_NO_MORE_FILES or
        //  STATUS_NO_SUCH_FILE. The caller just wants to see STATUS_SUCCESS.
        //
        if(
            (Status == STATUS_NO_MORE_FILES) ||
            (Status == STATUS_NO_SUCH_FILE)
        )
        {
            Status = STATUS_SUCCESS;
        }
        NtClose(hRoot);
    }

    return RtlNtStatusToDosError(Status);
}

#endif //XAPILIBP

#ifndef XAPILIBP

#undef DeleteFile
NTSTATUS
XapiNukeDirectoryFromHandle(
    HANDLE                      hDirectory,
    PFILE_DIRECTORY_INFORMATION DirectoryInfo
    )
/*++
  Routine Description:
    Helper function for XapiNukeDirectory.  Basically, it does the
    recursion of subdirectories.  It is more convenient to recursive
    using a handle.  This avoids writing ugly string parsing code which
    I am sure already exists in the object manager.  Additionally, since
    it doesn't open and close handles it is probably more efficient too.

  Arguments:
    hDirectory    - handle to directory to recursively delete.  Must be opened
                    with (FILE_LIST_DIRECTORY|DELETE|SYNCHRONIZE) access.
    DirectoryInfo - pointer to a FILE_DIRECTORY_INFORMATION immediately followed
                    by 255 characters (i.e. size must be sizeof(FILE_DIRECTORY_INFORMATION)
                    +sizeof(OCHAR)*255.
--*/
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    IO_STATUS_BLOCK IoStatusBlock;
    OBJECT_STRING FileName;
    HANDLE hFile;
    BOOLEAN fFindFirst = TRUE;
    FILE_BASIC_INFORMATION BasicInfo;
    FILE_DISPOSITION_INFORMATION Disposition;

    do
    {
        Status = NtQueryDirectoryFile(
                    hDirectory,
                    NULL,
                    NULL,
                    NULL,
                    &IoStatusBlock,
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
        if (NT_SUCCESS(Status))
        {
            BOOLEAN fIsDirectory = (FILE_ATTRIBUTE_DIRECTORY&DirectoryInfo->FileAttributes) ? TRUE : FALSE;
            ACCESS_MASK accessMask = FILE_WRITE_ATTRIBUTES | DELETE | SYNCHRONIZE | (fIsDirectory ? FILE_LIST_DIRECTORY : 0);
            ULONG OpenOptions = FILE_OPEN_FOR_BACKUP_INTENT | (fIsDirectory ? FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE);
            
            //
            //  Null terminate filename
            //
            DirectoryInfo->FileName[DirectoryInfo->FileNameLength/sizeof(OCHAR)] = OBJECT_NULL;

            //
            //  Create an OBJECT_STRING for the relative path of the filename
            //
            RtlInitObjectString(&FileName, DirectoryInfo->FileName);

            //
            //  Initialize Obja with Handle of the parent and the relative path.
            //
            InitializeObjectAttributes(
                &Obja,
                &FileName,
                OBJ_CASE_INSENSITIVE,
                hDirectory,
                NULL
                );

            //
            // Open the file
            //
            Status = NtOpenFile(
                        &hFile,
                        accessMask,
                        &Obja,
                        &IoStatusBlock,
                        FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                        OpenOptions
                        );

            if(NT_SUCCESS(Status))
            {
                //
                //  If it is a directory call ourselves recursively.
                //
                if(fIsDirectory)
                {
                    XapiNukeDirectoryFromHandle(hFile, DirectoryInfo);
                } else
                //
                //  Otherwise, Set FileDispositionInformation.DeleteFile = TRUE
                //
                {
                    //
                    //  If it has FILE_ATTRIBUTE_READONLY clear that first.
                    //
                    if(DirectoryInfo->FileAttributes&FILE_ATTRIBUTE_READONLY)
                    {
                        RtlZeroMemory(&BasicInfo,sizeof(BasicInfo));
                        BasicInfo.FileAttributes = 
                            (DirectoryInfo->FileAttributes&FILE_ATTRIBUTE_VALID_SET_FLAGS)&
                            ~FILE_ATTRIBUTE_READONLY;
                        BasicInfo.FileAttributes |= FILE_ATTRIBUTE_NORMAL;
                            
                        Status = NtSetInformationFile(
                                    hFile,
                                    &IoStatusBlock,
                                    &BasicInfo,
                                    sizeof(BasicInfo),
                                    FileBasicInformation
                                    );
                    }

                    if(NT_SUCCESS(Status))
                    {
                        //
                        // Sets to delete on close
                        //
                        Disposition.DeleteFile = TRUE;
                        Status = NtSetInformationFile(
                                    hFile,
                                    &IoStatusBlock,
                                    &Disposition,
                                    sizeof(Disposition),
                                    FileDispositionInformation
                                    );
                    }
                }
                //
                //  Close the handle, if it was a file this will delete it.
                //  If it was a directory, this should also delete it,
                //  provided the recursion succeeded.
                //
                NtClose(hFile);
            }
        }
    }while(NT_SUCCESS(Status));

    //
    //  If the loop terminated normally (we ran out of files to
    //  delete, or there were none in the first place), delete
    //  this handle.
    //
    if(
        (STATUS_NO_MORE_FILES == Status) ||
        (STATUS_NO_SUCH_FILE == Status)
    )
    {
        //
        //  Don't know if it is READ_ONLY.  Just blast over it,
        //  it will be deleted in a second anyway.
        //
        RtlZeroMemory(&BasicInfo,sizeof(BasicInfo));
        BasicInfo.FileAttributes = 
            (DirectoryInfo->FileAttributes&FILE_ATTRIBUTE_VALID_SET_FLAGS)&
            ~FILE_ATTRIBUTE_READONLY;
            BasicInfo.FileAttributes |= FILE_ATTRIBUTE_NORMAL;
        //
        //  Don't checked the error, this could conceivably fail
        //  and the coming delete will work anyway.  We would be fully
        //  satisfied.
        //
        NtSetInformationFile(
            hDirectory,
            &IoStatusBlock,
            &BasicInfo,
            sizeof(BasicInfo),
            FileBasicInformation
            );
        

        //
        // Sets to delete on close, the caller will
        // finish off the delete by closing the handle
        // it passed in.
        //
        Disposition.DeleteFile = TRUE;
        Status = NtSetInformationFile(
                        hDirectory,
                        &IoStatusBlock,
                        &Disposition,
                        sizeof(Disposition),
                        FileDispositionInformation
                        );
    }
    return Status;
}


NTSTATUS
XapiNukeDirectory(
    PCOSTR pszPath
    )
/*++
  Routine Description:
   Deletes a directory and all of its contents.
--*/
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    IO_STATUS_BLOCK IoStatusBlock;
    OBJECT_STRING DirectoryName;
    HANDLE hDirectory;
    ASSERT(pszPath);

    //
    //  Open the file with delete permission
    //

    RtlInitObjectString(&DirectoryName, pszPath);
    InitializeObjectAttributes(
        &Obja,
        &DirectoryName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    Status = NtOpenFile(
            &hDirectory,
            FILE_LIST_DIRECTORY | DELETE | FILE_WRITE_ATTRIBUTES | SYNCHRONIZE,
            &Obja,
            &IoStatusBlock,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
            );

    if(NT_SUCCESS(Status))
    {
        struct {FILE_DIRECTORY_INFORMATION DirInfo;OCHAR Names[255];} Buffer;
        PFILE_DIRECTORY_INFORMATION DirectoryInfo = &Buffer.DirInfo;

        //
        //  Delegate to XapiNukeDirectoryFromHandle.
        //
        
        Status = XapiNukeDirectoryFromHandle(hDirectory, DirectoryInfo);

        //
        //  Close the handle, if XapiNukeDirectoryFromHandle was successful
        //  this will delete the file.
        //
        NtClose(hDirectory);
    }
    return Status;
}

#endif //XAPILIBP
