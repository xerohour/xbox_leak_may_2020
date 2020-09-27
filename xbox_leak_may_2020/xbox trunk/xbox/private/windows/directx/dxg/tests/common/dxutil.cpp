//-----------------------------------------------------------------------------
// File: DXUtil.cpp
//
// Desc: Shortcut macros and functions for using DX objects
//
//
// Copyright (c) 1997-2000 Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------

#define STRICT

#ifdef XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include <stdio.h> 
#include <stdarg.h>
#include <D3D8.h>
#include <d3dx8core.h>
#include "DXUtil.h"

FLOAT DXUtil_Timer::m_fSecsPerTick = 0.0f;
FLOAT DXUtil_Timer::m_fBaseTime    = 0.0f;
FLOAT DXUtil_Timer::m_fStopTime    = 0.0f;

//-----------------------------------------------------------------------------
// Helper to display error messages.  This just shoves them out through the
// debug console.
//-----------------------------------------------------------------------------

void DisplayError(const WCHAR *szCall, HRESULT hr)
{
    WCHAR szErr[512];
	unsigned cch;

	if (hr)
	{
		cch = _snwprintf(szErr, sizeof(szErr), L"FATAL: %s failed 0x%08lX: ", szCall, hr);

		D3DXGetErrorString(hr, &szErr[cch], sizeof(szErr));
	}
	else
	{
		wcscpy(szErr, szCall);
	}

    wcscat(szErr, L"\n");

    OutputDebugString(szErr);

    _asm int 3;
}

//-----------------------------------------------------------------------------
// Name: DXUtil_FindMediaFile()
// Desc: Returns a valid path to a DXSDK media file
//-----------------------------------------------------------------------------
void DXUtil_FindMediaFile( CHAR* strPath, CHAR* strFilename )
{
    HANDLE file;

    if ( NULL==strFilename || NULL==strPath )
	{
		DisplayError(L"FATAL: Bad arguments to DXUtil_FindMediaFile", 0);
	}

    // Check if the file exists in the current directory
    strcpy( strPath, strFilename );

    file = CreateFileA( strPath, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, 0, NULL );
    if( INVALID_HANDLE_VALUE != file )
    {
        CloseHandle( file );
        return;
    }

    // Check if the file exists in the media directory
#ifdef PCBUILD
    sprintf( strPath, "c:\\Media\\%s", strFilename );
#else
    sprintf( strPath, "T:\\Media\\%s", strFilename );
#endif 

    file = CreateFileA(strPath, GENERIC_READ, FILE_SHARE_READ, NULL, 
                       OPEN_EXISTING, 0, NULL );
    if( INVALID_HANDLE_VALUE != file )
    {
        CloseHandle( file );
		return;
    }

	DisplayError(L"FATAL: Unable to open a file.", 0);
}

#ifdef PCBUILD

extern "C" ULONG DebugPrint(const char *Format, ...)
{
    char sz[2048];

    va_list args;
    va_start(args, Format);

    _vsnprintf(sz, 2048, Format, args);
    OutputDebugStringA(sz);

    va_end(args);

    return 0;
}


#endif PCBUILD
