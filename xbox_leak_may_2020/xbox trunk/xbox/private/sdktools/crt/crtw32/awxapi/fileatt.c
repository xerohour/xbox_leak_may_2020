/*
 *
 * fileatt.c
 *
 * XAPI a/w conversion
 *
 * Copyright (C) 2000 Microsoft Corporation
 *
 */

#include <ocharint.h>
#include <oscalls.h>

DWORD
__crtGetFileAttributes(
    IN LPCTSTR lpFileName
    )
{
	OCHAR szPathName[MAX_PATH];

	szPathName[0] = 0;
	tcstoocs(szPathName, lpFileName, OSZ_LEN(szPathName));
	szPathName[OSZ_LEN(szPathName) - 1] = 0;
	return GetFileAttributes(szPathName);
}

BOOL
__crtSetFileAttributes(
    IN LPCTSTR lpFileName,
    IN DWORD dwFileAttributes
    )
{
	OCHAR szPathName[MAX_PATH];

	szPathName[0] = 0;
	tcstoocs(szPathName, lpFileName, OSZ_LEN(szPathName));
	szPathName[OSZ_LEN(szPathName) - 1] = 0;
	return SetFileAttributes(szPathName, dwFileAttributes);
}
