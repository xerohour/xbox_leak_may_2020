/*
 *
 * direct.c
 *
 * XAPI a/w conversion
 *
 * Copyright (C) 2000 Microsoft Corporation
 *
 */

#include <ocharint.h>
#include <oscalls.h>

BOOL
__crtCreateDirectory(
    IN LPCTSTR lpPathName,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes
    )
{
	OCHAR szPathName[MAX_PATH];

	szPathName[0] = 0;
	tcstoocs(szPathName, lpPathName, OSZ_LEN(szPathName));
	szPathName[OSZ_LEN(szPathName) - 1] = 0;
	return CreateDirectory(szPathName, lpSecurityAttributes);
}

BOOL
__crtRemoveDirectory(
    IN LPCTSTR lpPathName
    )
{
	OCHAR szPathName[MAX_PATH];

	szPathName[0] = 0;
	tcstoocs(szPathName, lpPathName, OSZ_LEN(szPathName));
	szPathName[OSZ_LEN(szPathName) - 1] = 0;
	return RemoveDirectory(szPathName);
}
