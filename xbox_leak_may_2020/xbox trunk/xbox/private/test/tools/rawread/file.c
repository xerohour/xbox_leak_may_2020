#ifdef WIN32
#include <windows.h>
#include "win32\winmisc.h"
#else // WIN32
#include <conio.h>
#include <dos.h>
#include <stdio.h>
#endif // WIN32

#include "defs.h"
#include "diskutil.h"
#include "image.h"
#include "file.h"
#include "text.h"
#include "debug.h"


#ifndef WIN32
#pragma optimize("", off)
#endif // WIN32

FILEHANDLE FileOpen(LPSTR lpszName, WORD wMode, WORD wAttrib, WORD wAction)
{
    FILEHANDLE              fh;    

#ifdef WIN32

    DWORD                   dwDesiredAccess;
    DWORD                   dwShareMode;
    DWORD                   dwCreationDistribution;
    DWORD                   dwFlagsAndAttributes;

    // Set up access mode
    dwDesiredAccess = GENERIC_READ;

    if(wMode & OPEN_ACCESS_WRITEONLY)
    {
        dwDesiredAccess = GENERIC_WRITE;
    }

    if(wMode & OPEN_ACCESS_READWRITE)
    {
        dwDesiredAccess |= GENERIC_WRITE;
    }

    // Set up share mode
    dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;

    if(wMode & OPEN_SHARE_DENYREADWRITE)
    {
        dwShareMode = 0;
    }

    if(wMode & OPEN_SHARE_DENYWRITE)
    {
        dwShareMode &= ~(FILE_SHARE_WRITE);
    }

    if(wMode & OPEN_SHARE_DENYREAD)
    {
        dwShareMode &= ~(FILE_SHARE_READ);
    }

    // Set up creation distribution
    dwCreationDistribution = OPEN_EXISTING;

    if(wAction & ACTION_FILE_TRUNCATE)
    {
        dwCreationDistribution = CREATE_ALWAYS;
    }

    if(wAction & ACTION_FILE_CREATE)
    {
        dwCreationDistribution = CREATE_NEW;
    }

    // Set up flags and attributes
    dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;

    if(wAttrib & ATTR_READONLY)
    {
        dwFlagsAndAttributes |= FILE_ATTRIBUTE_READONLY;
    }

    if(wAttrib & ATTR_HIDDEN)
    {
        dwFlagsAndAttributes |= FILE_ATTRIBUTE_HIDDEN;
    }

    if(wAttrib & ATTR_SYSTEM)
    {
        dwFlagsAndAttributes |= FILE_ATTRIBUTE_SYSTEM;
    }

    if(wAttrib & ATTR_ARCHIVE)
    {
        dwFlagsAndAttributes |= FILE_ATTRIBUTE_ARCHIVE;
    }

    if(wAttrib & OPEN_FLAGS_COMMIT)
    {
        dwFlagsAndAttributes |= FILE_FLAG_NO_BUFFERING;
    }

    // TODO: find out about EXTENDED_SIZE under Win32
    fh = CreateFile(lpszName, dwDesiredAccess, dwShareMode, NULL, dwCreationDistribution, dwFlagsAndAttributes, NULL);

    if(fh == INVALID_HANDLE_VALUE)
    {
        fh = INVALID_FILE_HANDLE;
    }

#else // WIN32

    // Set the EXTENDED_SIZE attribute, allowing a file to grow to over 2 gigs
    if(GetDosVersion() >= 0x70A)
    {
        wAttrib |= 0x1000;
    }

    _asm
    {
        mov     fh, 0FFFFh      ; assume failure
        
        push    ds
        push    si
        
        mov     bx, wMode       ; specifies the mode to open the file in
        mov     cx, wAttrib     ; file attributes
        mov     dx, wAction     ; action to take if file exists
        lds     si, lpszName    ; filename
        mov     ah, 6Ch         ; Extended File Open/Create
        int     21h
        
        pop     si
        pop     ds
        
        jc      failure
        
        mov     fh, ax          ; AX contains the file handle
    
    failure:
    
    }

#endif // WIN32

	if(INVALID_FILE_HANDLE == fh)
	{
		DPF("Failed to open %s (%X %X %X)", lpszName, wMode, wAttrib, wAction);
	}
	else
	{
		DPF("Opened %s (%X %X %X).  Handle = %X", lpszName, wMode, wAttrib, wAction, fh);
	}

    return fh;

}

#ifndef WIN32
#pragma optimize("", on)
#endif // WIN32


void FileClose(FILEHANDLE fh)
{

#ifdef WIN32

    CloseHandle(fh);

#else // WIN32

    _dos_close(fh);

#endif // WIN32

	DPF("Closed file handle %X", fh);
}


DWORD FileRead(FILEHANDLE fh, LPVOID lpvBuffer, DWORD dwBufSize)
{
    DWORD                   dwRead;

#ifdef WIN32

    if(!ReadFile(fh, lpvBuffer, dwBufSize, &dwRead, NULL))
    {
        return (DWORD)-1;
    }

#else // WIN32

    WORD                    wRead;

    if(dwBufSize > MAXWORD)
    {
        return (DWORD)-1;
    }

    if(_dos_read(fh, lpvBuffer, (WORD)dwBufSize, &wRead))
    {
        dwRead = (DWORD)-1;
    }
    else
    {
        dwRead = wRead;
    }

#endif // WIN32

    return dwRead;
}
 
 
DWORD FileWrite(FILEHANDLE fh, LPVOID lpvBuffer, DWORD dwBufSize)
{
    DWORD                   dwWritten;

#ifdef WIN32

    if(!WriteFile(fh, lpvBuffer, dwBufSize, &dwWritten, NULL))
    {
        return (DWORD)-1;
    }

#else // WIN32

    WORD                    wWritten;

    if(dwBufSize > MAXWORD)
    {
        return (DWORD)-1;
    }

    if(_dos_write(fh, lpvBuffer, (WORD)dwBufSize, &wWritten))
    {
        dwWritten = (DWORD)-1;
    }
    else
    {
        dwWritten = wWritten;
    }
            
#endif // WIN32

    return dwWritten;
}


void FileCommit(FILEHANDLE fh)
{

#ifdef WIN32
    
    FlushFileBuffers(fh);

#else // WIN32

    _dos_commit(fh);

#endif // WIN32

}


DWORD FileGetPos(FILEHANDLE fh)
{
    return FileSetPos(fh, 0, SEEK_CUR);
}


DWORD FileSetPos(FILEHANDLE fh, long lOffset, BYTE bMethod)
{
    long                    lNewOffset;

#ifdef WIN32

    lNewOffset = SetFilePointer(fh, lOffset, NULL, bMethod);

#else // WIN32

    lNewOffset = _dos_seek(fh, lOffset, bMethod);

#endif // WIN32

    return lNewOffset;
}

