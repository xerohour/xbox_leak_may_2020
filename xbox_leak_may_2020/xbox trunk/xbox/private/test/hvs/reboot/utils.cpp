/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    utils.cpp

Abstract:

    Helper functions

Notes:

*****************************************************************************/

#include "reboot.h"

DWORD MapDrive(char cDriveLetter, char* pszPartition)
    {
    char pszDrive[20];
    ZeroMemory(pszDrive, 20);
 
    sprintf(pszDrive, "\\??\\%c:", cDriveLetter);
 
    ANSI_STRING ostDrive, ostPath;
 
    RtlInitObjectString(&ostDrive, pszDrive);
    RtlInitObjectString(&ostPath, pszPartition);
    NTSTATUS status = IoCreateSymbolicLink(&ostDrive, &ostPath);
 
    return RtlNtStatusToDosError(status);
    }

DWORD UnMapDrive(char cDriveLetter)
    {
    char pszDrive[20];
    ZeroMemory(pszDrive, 20);
 
    sprintf(pszDrive, "\\??\\%c:", cDriveLetter);
 
    ANSI_STRING ostDrive;
 
    RtlInitObjectString(&ostDrive, pszDrive);
    NTSTATUS status = IoDeleteSymbolicLink(&ostDrive);
 
    return RtlNtStatusToDosError(status);
    }

DWORD GetMapDrive(char cDriveLetter, char *partition)
    {
    OBJECT_STRING ObjectName;
    ULONG TargetLength = MAX_PATH;
    OBJECT_STRING ObjectTarget;
    char pszDrive[20];
    HANDLE handle;

    ZeroMemory(pszDrive, 20);
    sprintf(pszDrive, "\\??\\%c:", cDriveLetter);

    OBJECT_ATTRIBUTES ObjectAttributes;
    RtlInitObjectString(&ObjectName, pszDrive);

    InitializeObjectAttributes(&ObjectAttributes,
                               &ObjectName,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL);

    NTSTATUS status = NtOpenSymbolicLinkObject(&handle, &ObjectAttributes);
    if(!NT_SUCCESS(status))
        {
        return RtlNtStatusToDosError(status);
        }

    ObjectTarget.Buffer = partition;
    ObjectTarget.MaximumLength = MAX_PATH;

    status = NtQuerySymbolicLinkObject(handle, &ObjectTarget, &TargetLength);

    NtClose(handle);

    strncpy(partition, ObjectTarget.Buffer, ObjectTarget.Length);
    partition[ObjectTarget.Length] = '\0';

    return RtlNtStatusToDosError(status);
    }


/*

Routine Description:

    Checks to see if a file or directory exists

Arguments:

    char *filename - the name of the file or dir to search for

Return Value:

    true if it exists, false if it doesnt

*/
bool FileExists(const char *filename)
    {
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    bool retval = false;

    hFind = FindFirstFile(filename, &FindFileData);

    if(hFind != INVALID_HANDLE_VALUE)
        {
        retval = true;
        }
    FindClose(hFind);

    return retval;
    }
