//  MANIFEST.CPP
//
//  Created 13-Mar-2001 [JonT]

#include "unpacker.h"


//---------------------------------------------------------------------

 
CManifest::CManifest(
    CSettingsFile* psettings
    )
: m_lpData(NULL),
  m_fEOF(FALSE)
{
}

                                    
//  CManifest::SetFile
//      Initializes the manifest object by reading in the entire manifest and preparing
//      to parse each line.
                                    
BOOL
CManifest::SetFile(
    LPCTSTR lpFile
    )
{
    HANDLE handle;
    DWORD dwLen;
    DWORD dwActuallyRead;

    // Make sure we don't already have a manifest loaded
    if (m_lpData)
        free(m_lpData);

    // Try to open the file
    handle = CreateFile(lpFile, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (handle == INVALID_HANDLE_VALUE)
        return FALSE;

    // Read the entire file
    dwLen = GetFileSize(handle, NULL);
    if (dwLen == 0xffffffff)
        return FALSE;

    // Allocate a buffer big enough for the whole file
    m_lpData = (LPSTR)malloc(dwLen + 1);
    if (!m_lpData)
        return FALSE;

    // Read it completely and zero-terminate
    if (!ReadFile(handle, m_lpData, dwLen, &dwActuallyRead, NULL) ||
        dwActuallyRead != dwLen)
        return FALSE;
    *(m_lpData + dwLen) = 0;

    // Close file, we're done with it
    CloseHandle(handle);

    // Point the line pointer to the start of the data
    m_lpLine = m_lpData;

    return TRUE;
}


//  CManifest::ResetPointer
//      Sets the pointer to the start of the manifest
//      As with other parsing functions, returns TRUE on EOF.

BOOL
CManifest::ResetPointer()
{
    m_lpLine = m_lpData;

    // Skip whitespace including comment lines, etc.
    // Note that if we return TRUE here, the manifest is empty
    // and the installer is corrupt.
    return SkipWhitespace(m_lpLine);
}


//  CManifest::GetLine
//      Copies the current line of the manifest into the internal buffer.
//      This function does not move to the next line.
//      The buffer can be modified in place. GetLine can be called again
//      to copy in a fresh copy of the line.

LPSTR
CManifest::GetLine()
{
    LPSTR lpIn = m_lpLine;
    LPSTR lpOut = m_szLine;
    DWORD i;

    // Read from the buffer until we hit end of line or file
    for (i = 0 ; *lpIn != '\n' && *lpIn != '\r' && i < MAX_MANIFEST_LINE ; ++lpIn, ++i)
    {
        if (*lpIn == 0)
            break;
        else
            *lpOut++ = *lpIn;
    }
    *lpOut = 0;

    return m_szLine;
}


//  CManifest::SkipToNextLine
//      Moves the pointer to the start of the next line.
//      Returns TRUE at EOF.

BOOL
CManifest::SkipToNextLine()
{
    for (; *m_lpLine != '\n' && *m_lpLine != '\r' ; ++m_lpLine)
    {
        if (*m_lpLine == 0)
            return TRUE;
    }

    // Skip whitespace including comment lines, etc.
    return SkipWhitespace(m_lpLine);
}


