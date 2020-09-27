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

typedef struct _WIN32_FIND_DATAT {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    TCHAR cFileName[ MAX_PATH ];
    TCHAR cAlternateFileName[ 14 ];
} WIN32_FIND_DATAT, *PWIN32_FIND_DATAT, *LPWIN32_FIND_DATAT;

static void ConvertWFD(PWIN32_FIND_DATAT pwfdOut, PWIN32_FIND_DATA pwfdIn)
{
	memcpy(pwfdOut, pwfdIn, (int)&(((PWIN32_FIND_DATA)NULL)->cFileName[0]));
	pwfdOut->cFileName[0] = 0;
	ocstotcs(pwfdOut->cFileName, pwfdIn->cFileName,
		TSZ_LEN(pwfdOut->cFileName));
	pwfdOut->cFileName[TSZ_LEN(pwfdOut->cFileName)-1] = 0;
	pwfdOut->cAlternateFileName[0] = 0;
	ocstotcs(pwfdOut->cAlternateFileName, pwfdIn->cAlternateFileName,
		TSZ_LEN(pwfdOut->cAlternateFileName));
	pwfdOut->cAlternateFileName[TSZ_LEN(pwfdOut->cAlternateFileName)-1] = 0;
}

HANDLE
__crtFindFirstFile(
    IN LPCTSTR lpFileName,
    OUT LPWIN32_FIND_DATAT lpFindFileData
    )
{
	OCHAR szPathName[MAX_PATH];
	WIN32_FIND_DATA wfd;
	HANDLE hRet;

	szPathName[0] = 0;
	tcstoocs(szPathName, lpFileName, OSZ_LEN(szPathName));
	szPathName[OSZ_LEN(szPathName) - 1] = 0;
	hRet = FindFirstFile(szPathName, &wfd);
	if(hRet != INVALID_HANDLE_VALUE)
		ConvertWFD(lpFindFileData, &wfd);
	return hRet;
}

BOOL
__crtFindNextFile(
    IN HANDLE hFindFile,
    OUT LPWIN32_FIND_DATAT lpFindFileData
    )
{
	WIN32_FIND_DATA wfd;
	BOOL fRet;

	fRet = FindNextFile(hFindFile, &wfd);
	if(fRet)
		ConvertWFD(lpFindFileData, &wfd);
	return fRet;
}
