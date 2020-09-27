/*
 *
 * filecrea.c
 *
 * XAPI a/w conversion
 *
 * Copyright (C) 2000 Microsoft Corporation
 *
 */

#include <ocharint.h>
#include <oscalls.h>

HANDLE
__crtCreateFile(
    IN LPCTSTR lpFileName,
    IN DWORD dwDesiredAccess,
    IN DWORD dwShareMode,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    IN DWORD dwCreationDisposition,
    IN DWORD dwFlagsAndAttributes,
    IN HANDLE hTemplateFile
    )
{
	OCHAR szPathName[MAX_PATH];

	szPathName[0] = 0;
	tcstoocs(szPathName, lpFileName, OSZ_LEN(szPathName));
	szPathName[OSZ_LEN(szPathName) - 1] = 0;
	return CreateFile(
		szPathName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile
		);
}
