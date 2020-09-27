/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    utils.cpp

Abstract:

    Helper functions

Notes:

*****************************************************************************/

#include "diskWrite.h"


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
