// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFile.cpp
// Contents:  Specialized file class for xbPremaster.
// Revisions: 2-Jun-2001: Created (jeffsim)
// Notes:     * This is a very specialized file class for the xbPremaster app.  It is not
//              recommended that this class be used for other applications.
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFile::CFile
// Purpose:   Constructor for the CFile class.  Opens the specified file in preparation for READING
//            This will only be called on disk sources.
// Arguments: szFilename        -- name of the file to open
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CFile::CFile(char *szFilename)
{
    // Validate parameters
    if (szFilename == NULL)
    {
        SetInited(E_INVALIDARG);
        return;
    }

    // Track the name of our file.
    strcpy(m_szFilename, szFilename);
    
    // Track the last action performed
    m_taskLast = TASK_OPEN;

    // Open the file for READ-ONLY access.  The file must exist or we fail.
    m_hFile = CreateFile(szFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (m_hFile == INVALID_HANDLE_VALUE)
    {
        // NOTE: Non-OLE compatible error code.  Acceptable here ONLY because I catch it elsewhere
        // (see note at top of this file).  Difficult to use OLE for errors here since it doesn't
        // have a lot of errors we need to catch (ie file not found).
        SetInited(GetLastError());
        return;
    }

    SetInited(S_OK);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFile::~CFile
// Purpose:   CFile destructor.  Close the handle to our file and cleanup after ourselves.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CFile::~CFile()
{
    Close();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFile::SeekTo
// Purpose:   Seeks to the specified position in the file (from the beginning of the file)
//            This is only called on disk sources.
// Arguments: liDist        -- Number of bytes to seek forward
// Return:    Error code, or S_OK if successful.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CFile::SeekTo(LARGE_INTEGER liDist)
{
    if (!IsInited())
        return E_FAIL;
    
    // Track the last action performed
    m_taskLast = TASK_SEEK;

    if (SetFilePointer(m_hFile, liDist.LowPart, &liDist.HighPart, FILE_BEGIN) == 0xFFFFFFFF)
    {
        // Since 0xffffffff could be a valid low value, we need to check getlasterror as well
        DWORD dw = GetLastError();
        if (dw != NO_ERROR)
            return dw;

        // Otherwise, it was a valid value
    }

    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFile::Read
// Purpose:   Reads the specified number of bytes from our file.
// Arguments: pby           -- Buffer containing bytes to read
//            cBytes        -- Number of bytes to read
//            pnRead        -- Will contain the actual number of bytes read.  Can be 'NULL'.
// Return:    Error code, or S_OK if successful
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CFile::Read(BYTE *pby, UINT cBytes, int *pnRead)
{
    DWORD dwRead;

    if (!IsInited())
        return GetInitCode();

    // Track the last action performed
    m_taskLast = TASK_READ;

    if (!ReadFile(m_hFile, pby, cBytes, &dwRead, 0))
    {
        // Failed to read the whole buffer.
        if (pnRead)
            *pnRead = dwRead;
        return (GetLastError());
    }
    if (pnRead)
        *pnRead = dwRead;

    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFile::GetSize
// Purpose:   Gets the size of our file
//            Only called on disk sources
// Arguments: puliSize       -- Variable to contain the size of our file
// Return:    Error code, or S_OK if successful
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CFile::GetSize(ULARGE_INTEGER *puliSize)
{
    // Track the last action performed
    m_taskLast = TASK_GETSIZE;

    // Get the size of the file
    puliSize->LowPart = GetFileSize(m_hFile, &puliSize->HighPart);
    if (puliSize->LowPart != INVALID_FILE_SIZE)
        return S_OK;
    else
        return GetLastError();
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFile::Close
// Purpose:   Closes our file
// Arguments: None
// Return:    Error code, or S_OK if successful
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CFile::Close()
{
    // Track the last action performed
    m_taskLast = TASK_CLOSE;

    // Close the handle to our file (if it was successfully opened in the first place).
    if (m_hFile)
    {
        CloseHandle(m_hFile);
        m_hFile = NULL;
    }

    return S_OK;
}

HRESULT CFile::ReadBYTE(BYTE *pbyData, int *pnRead)
{
    return Read(pbyData, 1, pnRead);
}

HRESULT CFile::ReadWORD(WORD *pwData, int *pnRead)
{
    return Read((BYTE*)pwData, 2, pnRead);
}

HRESULT CFile::ReadDWORD(DWORD *pdwData, int *pnRead)
{
    return Read((BYTE*)pdwData, 4, pnRead);
}

HRESULT CFile::ReadString(char *sz, int *pnRead)
{
    DWORD dwStrlen;
    int   nRead;
    HRESULT hr = ReadDWORD(&dwStrlen, &nRead);
    if (FAILED(hr))
        return hr;
    
    hr = Read((BYTE*)sz, dwStrlen, &nRead);
    if (FAILED(hr))
        return hr;
    if (nRead != (int)dwStrlen)
        return E_FAIL;
    
    sz[dwStrlen] = '\0';
    if (pnRead)
        *pnRead = nRead + 4;
    return S_OK;
}

