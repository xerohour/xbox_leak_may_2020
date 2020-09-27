/*
 *
 * filedel.c
 *
 * XAPI a/w conversion
 *
 * Copyright (C) 2000 Microsoft Corporation
 *
 */

#include <ocharint.h>
#include <oscalls.h>

BOOL
__crtDeleteFile(
    IN LPCTSTR lpFileName
    )
{
	OCHAR szPathName[MAX_PATH];

	szPathName[0] = 0;
	tcstoocs(szPathName, lpFileName, OSZ_LEN(szPathName));
	szPathName[OSZ_LEN(szPathName) - 1] = 0;
	return DeleteFile(szPathName);
}
