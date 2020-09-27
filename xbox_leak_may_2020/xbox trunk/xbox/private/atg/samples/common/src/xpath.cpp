//-----------------------------------------------------------------------------
// File: xpath.cpp
//
// Desc: Routines to help us point to the right data files.
//
// Note: If you dont specify an out pointer in GetFileName, the routine
//       will use g_tstrXPathFileName[] to store the resulting file/path
//       info and return a pointer to it. The maximum number of these
//       that can exist at one time is defined by MAX_XPATHS.
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include <tchar.h>
#include "xpath.h"

#define MAX_XPATHS 4
DWORD g_dwXPathCurPathIdx = 0;
TCHAR g_tstrXPathBasePathT[MAX_PATH];
char g_tstrXPathBasePathA[MAX_PATH];
WORD g_tstrXPathFileName[MAX_XPATHS][MAX_PATH];

//-----------------------------------------------------------------------------
// Name: XPath_SetBasePath
// Desc: Set the base path for all our files
//-----------------------------------------------------------------------------
void XPath_SetBasePath(TCHAR *path)
{
	_tcsncpy(g_tstrXPathBasePathT, path, MAX_PATH);
	if(sizeof(TCHAR)==sizeof(WCHAR))
		WideCharToMultiByte(CP_ACP, 0, (WCHAR *)path, -1, g_tstrXPathBasePathA, MAX_PATH, NULL, NULL);
	else
		strncpy(g_tstrXPathBasePathA, (char *)path, MAX_PATH);
}

//-----------------------------------------------------------------------------
// Name: XPath_GetFileNameT
// Desc: TCHAR version of GetFileName
//-----------------------------------------------------------------------------
TCHAR *XPath_GetFileNameT(TCHAR *in, TCHAR *out)
{
	if(!out)
	{
		out = (TCHAR *)g_tstrXPathFileName[g_dwXPathCurPathIdx];
		g_dwXPathCurPathIdx = (g_dwXPathCurPathIdx+1)&(MAX_XPATHS-1);
	}

	_tcscpy(out, g_tstrXPathBasePathT);
	_tcscat(out, in);

	return out;
}

//-----------------------------------------------------------------------------
// Name: XPath_GetFileNameA
// Desc: char version of GetFileName
//-----------------------------------------------------------------------------
char *XPath_GetFileNameA(char *in, char *out)
{
	if(!out)
	{
		out = (char *)g_tstrXPathFileName[g_dwXPathCurPathIdx];
		g_dwXPathCurPathIdx = (g_dwXPathCurPathIdx+1)&(MAX_XPATHS-1);
	}

	strcpy(out, g_tstrXPathBasePathA);
	strcat(out, in);

	return out;
}