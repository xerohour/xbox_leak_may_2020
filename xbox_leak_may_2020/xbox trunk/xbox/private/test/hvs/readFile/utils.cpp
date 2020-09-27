/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    utils.cpp

Abstract:

    Helper functions

Notes:

*****************************************************************************/

#include "readFile.h"

void CompareBuff(unsigned char *buff1, unsigned char *buff2, unsigned char *buff3, unsigned length)
    {
    for(unsigned i=0; i<length; i++)
        {
        if(buff1[i] != buff2[i])
            {
            LogPrint("    %08X: %02X %02X %02X\n", i, buff1[i], buff2[i], buff3[i]);
            }
        }
    }


bool ParseFiles(char *path, PARSE_FUNC funct)
    {
    bool exit = false;
    HANDLE h;
    DWORD err;
    char *pathEnd;
    
    pathEnd = path + strlen(path);
    strcpy(pathEnd, "\\*.*");

    h = FindFirstFile(path, &findData);
    if(h == INVALID_HANDLE_VALUE)
        {
        err = GetLastError();
        if(err != ERROR_NO_MORE_FILES && err != ERROR_FILE_NOT_FOUND && err!=0)
            {
            LogPrint("Find Error %u: %s\n", err, path);
            ++numFail;
            }
        *pathEnd = '\0';

        if(InputCheckExitCombo())
            {
            exit = true;
            }
        if(globalSettings.stopAfter.duration)
            {
            if((globalSettings.stopAfter.type == STOPAFTER_MINUTES && TICK2MIN(GetTickCount() - time) >= globalSettings.stopAfter.duration) ||
                (globalSettings.stopAfter.type == STOPAFTER_HOURS && TICK2HOUR(GetTickCount() - time) >= globalSettings.stopAfter.duration) )
                {
                exit = true;
                }
            }

        return exit;
        }

    do
        {
        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);
        XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &addDevice, &removeDevice);
        InputMUInsertions(addDevice, removeDevice);
        UpdateGraphics(DISPLAYMODE_RUN_TIME);

        strcpy(pathEnd+1, findData.cFileName);
        if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
            exit = ParseFiles(path, funct);
            if(exit == true)
                {
                SetLastError(0);
                break;
                }
            }
        else
            {
            exit = funct(path, findData);
            if(exit == true)
                {
                SetLastError(0);
                break;
                }
            }

        if(InputCheckDigitalButton(XINPUT_GAMEPAD_START)) GUI();
        if(InputCheckExitCombo())
            {
            exit = true;
            SetLastError(0);
            break;
            }
        if(globalSettings.stopAfter.duration)
            {
            if((globalSettings.stopAfter.type == STOPAFTER_MINUTES && TICK2MIN(GetTickCount() - time) >= globalSettings.stopAfter.duration) ||
                (globalSettings.stopAfter.type == STOPAFTER_HOURS && TICK2HOUR(GetTickCount() - time) >= globalSettings.stopAfter.duration) )
                {
                exit = true;
                SetLastError(0);
                break;
                }
            }
        } while(FindNextFile(h, &findData));

    err = GetLastError();
    if(err != ERROR_NO_MORE_FILES && err != ERROR_FILE_NOT_FOUND && err!=0)
        {
        LogPrint("Find Error %u: %s\n", err, path);
        ++numFail;
        }

    FindClose(h);
    *pathEnd = '\0';
    return exit;
    }


extern "C"
    {
	NTSYSCALLAPI
	NTSTATUS
	NTAPI
	IoCreateSymbolicLink(
		IN POBJECT_STRING SymbolicLinkName,
		IN POBJECT_STRING DeviceName
		);

	NTSYSCALLAPI
	NTSTATUS
	NTAPI
	IoDeleteSymbolicLink(
		IN POBJECT_STRING SymbolicLinkName
		);
    }


DWORD MapDrive( char cDriveLetter, char* pszPartition )
    {
    char pszDrive[20];
    ZeroMemory( pszDrive, 20 );

    sprintf( pszDrive, "\\??\\%c:", cDriveLetter );

    ANSI_STRING ostDrive, ostPath;

    RtlInitObjectString( &ostDrive, pszDrive );
    RtlInitObjectString( &ostPath, pszPartition );
    NTSTATUS status = IoCreateSymbolicLink( &ostDrive, &ostPath );

    return RtlNtStatusToDosError( status );
    }

DWORD UnMapDrive( char cDriveLetter )
    {
    char pszDrive[20];
    ZeroMemory( pszDrive, 20 );

    sprintf( pszDrive, "\\??\\%c:", cDriveLetter );

    ANSI_STRING ostDrive;

    RtlInitObjectString( &ostDrive, pszDrive );
    NTSTATUS status = IoDeleteSymbolicLink(&ostDrive);

    return RtlNtStatusToDosError( status );
    }
