// Copyright (c) 1998-1999 Microsoft Corporation

#include <windows.h>
#include <winver.h>
#include <stdio.h>

#include "lsclsid.h"

static LPBYTE AllocFileVersionInfo(LPSTR pstrName);

static char szVerFormat[] = "%d.%d.%d.%d";

// File version, US English codepage
//
static char szFixedFileInfoTag[] = "\\";

#define USENG_STRINFO "\\StringFileInfo\\040904b0\\"

static char szFileVersionTag[] = USENG_STRINFO "FileVersion";

// FileVersionInfo
//
// Print just the file version of the given file or nothing if the version could not
// be determined.
//
char* FileVersionInfo(LPSTR pstrName)
{
	LPBYTE pbVerInfoBuff = AllocFileVersionInfo(pstrName);
	LPSTR  pstrFileVersion;
	unsigned cbFileVersion;
	VS_FIXEDFILEINFO *pVsffi;
	unsigned cbVsffi;
	
	static char szFileVersion[256];		// Returned!!!

	*szFileVersion = '\0';
	if (pbVerInfoBuff == NULL) {
		return szFileVersion;
	}
	
	// Try to get the file version string
	//
	if (VerQueryValue(pbVerInfoBuff, 
		szFileVersionTag, 
		(void**)&pstrFileVersion, 
		&cbFileVersion) != 0) {
		
		strcpy(szFileVersion, pstrFileVersion);

	// No file version string. Try to get the raw version info from the version
	// header
	//
	} else if (VerQueryValue(pbVerInfoBuff,
				szFixedFileInfoTag, 
				(void**)&pVsffi,
				&cbVsffi) !=0 && 
				cbVsffi >= sizeof(VS_FIXEDFILEINFO)) {

		wsprintf(szFileVersion,
				 szVerFormat, 
				 HIWORD(pVsffi->dwFileVersionMS),
				 LOWORD(pVsffi->dwFileVersionMS),
				 HIWORD(pVsffi->dwFileVersionLS),
				 LOWORD(pVsffi->dwFileVersionLS));
	} 	

	free(pbVerInfoBuff);

	return szFileVersion;
}

typedef struct {
	char *szResourcePath;
	char *szTag;
	char szStorage[256];
} VERSION_ITEM;


VERSION_ITEM versionItems[MAX_LONGVERINFO_LINES] =
{
	{ USENG_STRINFO "OriginalFilename",		"File Name" },
	{ szFileVersionTag,						"File Version" },
	{ USENG_STRINFO "ProductName",			"Product Name" },
	{ USENG_STRINFO "ProductVersion",		"Product Version" },
	{ USENG_STRINFO "Comments",				"Comments" },
	{ USENG_STRINFO "LegalCopyright",		"Legal Copyright" }
};


int LongFileVersionInfo(
	LPSTR pstrName, 
	char *ppTags[MAX_LONGVERINFO_LINES], 
	char *ppLines[MAX_LONGVERINFO_LINES], 
	unsigned *pMaxTagLen)
{
	LPBYTE pbVerInfoBuff = AllocFileVersionInfo(pstrName);
	VERSION_ITEM *pItem;
	int retLines = 0;
	int idx;
	unsigned cb;
	LPSTR pstr;
	unsigned maxTag = 0;

	if (pbVerInfoBuff == NULL) {
		return retLines;
	}

	for (idx = 0, pItem = versionItems; idx < MAX_LONGVERINFO_LINES; idx++, pItem++) {
		if (VerQueryValue(pbVerInfoBuff,
						  pItem->szResourcePath,
						  (void**)&pstr,
						  &cb)) {
			if (cb > sizeof(pItem->szStorage) - 1) {
				cb = sizeof(pItem->szStorage) - 1;
			}

			memcpy(pItem->szStorage, pstr, cb);
			pItem->szStorage[cb] = '\0';

			ppTags[retLines] = pItem->szTag;
			ppLines[retLines] = pItem->szStorage;
			retLines++;

			cb = strlen(pItem->szTag);
			if (cb > maxTag) {
				maxTag = cb;
			}
		}
	}

	*pMaxTagLen = maxTag;

	return retLines;
}

static LPBYTE AllocFileVersionInfo(LPSTR pstrName)
{
	DWORD dwNull;
	unsigned cb;
	LPBYTE pbVerInfoBuff;

	if ((cb = GetFileVersionInfoSize(pstrName, &dwNull)) == 0) {
		return NULL;
	}

	pbVerInfoBuff = new BYTE[cb];

	if (GetFileVersionInfo(pstrName, 0, cb, pbVerInfoBuff) == 0) {
		free(pbVerInfoBuff);
		return NULL;
	}

	return pbVerInfoBuff;
}

