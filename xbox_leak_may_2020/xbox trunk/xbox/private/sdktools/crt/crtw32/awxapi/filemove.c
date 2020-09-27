/*
 *
 * filemove.c
 *
 * XAPI a/w conversion
 *
 * Copyright (C) 2000 Microsoft Corporation
 *
 */

#include <ocharint.h>
#include <oscalls.h>

BOOL
__crtMoveFile(
    IN LPCTSTR lpExistingFileName,
    IN LPCTSTR lpNewFileName
    )
{
	OCHAR szEPathName[MAX_PATH];
	OCHAR szNPathName[MAX_PATH];

	szEPathName[0] = 0;
	tcstoocs(szEPathName, lpExistingFileName, OSZ_LEN(szEPathName));
	szEPathName[OSZ_LEN(szEPathName) - 1] = 0;
	szNPathName[0] = 0;
	tcstoocs(szNPathName, lpNewFileName, OSZ_LEN(szNPathName));
	szNPathName[OSZ_LEN(szNPathName) - 1] = 0;
	return MoveFile(szEPathName, szNPathName);
}
