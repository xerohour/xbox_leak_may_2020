// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     cxboxstream.cpp
// Contents: UNDONE-WARN: Add file description
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
// Function:  CXboxStream::CXboxStream
// Purpose:   CXboxStream constructor
// Arguments: None.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CXboxStream::CXboxStream(char *szFile, bool fCut) : m_cRef(1)
{
    // Open the file, but don't read anything in yet.
    // Copy the file from the target xbox to a temporary file on this machine., and then open that
    // file.  Note: Be sure to delete the temp file when done!
    char szTempPath[MAX_PATH];

    m_hfile = NULL;
    
    // Create a temporary file name for the new file
    if (GetTempPath(MAX_PATH, szTempPath) == 0)
        return;
    
    if (GetTempFileName(szTempPath, "xbx", 0, m_szTempPathFileName) == 0)
        return;
    
    // Now that we have a valid name for our temporary file, copy the file from the Xbox
    // to the temporary file so that we can read from it as requested.
    if (DmReceiveFile(m_szTempPathFileName, szFile) != XBDM_NOERR)
        return;
    strcpy(m_szFile, szFile);
    m_fCut = fCut;

    // Open the file for subsequent reads
    m_hfile = CreateFile(m_szTempPathFileName, FILE_GENERIC_READ, 0, NULL, OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL, 0);
    if (m_hfile == INVALID_HANDLE_VALUE)
        m_hfile = NULL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::~CXboxStream
// Purpose:   CXbox destructor
// Arguments: None.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CXboxStream::~CXboxStream()
{
    // Close our file
    if (m_hfile)
        CloseHandle(m_hfile);

    // Need to delete the temp file we created
    DeleteFile(m_szTempPathFileName);

    if (0 && m_fCut)
    {
        DM_FILE_ATTRIBUTES dmfa;
        // Delete our source file
        xbfu.GetFileAttributes(m_szFile, &dmfa);
        xbfu.Del(m_szFile, false, NULL);
    }
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ IUNKNOWN METHODS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::QueryInterface
// Purpose:   Obtain a particular interface to this object.
// Arguments: riid          -- The interface to this object being queried for.
//            ppvObject     -- Buffer to fill with obtained interface.
// Return:    S_OK if interface obtained; E_NOINTERFACE otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::QueryInterface(REFIID riid, void **ppvObject)
{ 
    *ppvObject = NULL;

    if (riid == IID_IUnknown) 
        *ppvObject = this;
    else if (riid == IID_IStream) 
        *ppvObject = static_cast<IStream*>(this);
    else
        return E_NOINTERFACE;

    if (*ppvObject)
    {
        static_cast<LPUNKNOWN>(*ppvObject)->AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}   

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::AddRef
// Purpose:   Add a reference to this object
// Arguments: None
// Return:    New reference count
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP_(ULONG) CXboxStream::AddRef()
{
    return (ULONG)InterlockedIncrement(&m_cRef);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::Release
// Purpose:   Subtract a reference to this object.  If the new number of references is zero, then
//            delete the object.
// Arguments: None
// Return:    New reference count.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP_(ULONG) CXboxStream::Release()
{
    if (0 == InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return (ULONG)m_cRef;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ ISTREAM METHODS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::Read
// Purpose:   Read a string of bytes.
// Arguments: pv            -- Buffer into which the bytes are read.
//            cb            -- Number of bytes to read
//            pcbRead       -- Number of bytes actually read.
// Return:    S_OK if successful; error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
    // Read the bits from our source file.
    if (!ReadFile(m_hfile, pv, cb, pcbRead, NULL))
        return GetLastError();

    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::Read
// Purpose:   Read a string of bytes.
// Arguments: pv            -- Buffer into which the bytes are read.
//            cb            -- Number of bytes to read
//            pcbRead       -- Number of bytes actually read.
// Return:    S_OK if successful; error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::Write(VOID const *pv, ULONG cb, ULONG *pcbWritten)
{
    // Write the specified bits to our temporary destination file.
    WriteFile(m_hfile, pv, cb, pcbWritten, NULL);
    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::Seek
// Purpose:   Seeks to a particular position in the stream
// Arguments: dbMove        -- Number of bytes in which to move.
//            dwOrigin      -- Starting location of move.
//            pbNewPosition -- Filled with new file pointer.
// Return:    S_OK if successful; error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::Seek(LARGE_INTEGER dbMove, DWORD dwOrigin, ULARGE_INTEGER *pbNewPosition)
{
    // Seek to the specified position in our source and destination files.
    LARGE_INTEGER li = dbMove;
    li.LowPart = SetFilePointer(m_hfile, li.LowPart, &li.HighPart, dwOrigin);

    // Test for failure
    if (li.LowPart == 0xFFFFFFFF)
    {
        // Check if actually an error
        DWORD dwErr = GetLastError();
        if (dwErr != NO_ERROR)
            return dwErr;
    } 
    
    if (pbNewPosition)
        (*pbNewPosition).QuadPart = li.QuadPart;

    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::SetSize
// Purpose:   unimplemented
// Arguments: UNDONE-WARN: Fill in
// Return:    S_OK if successful; error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::SetSize(ULARGE_INTEGER cbNewSize)
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::CopyTo
// Purpose:   unimplemented
// Arguments: UNDONE-WARN: Fill in
// Return:    S_OK if successful; error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead,
                    ULARGE_INTEGER *pcbWritten)
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::Commit
// Purpose:   unimplemented
// Arguments: UNDONE-WARN: Fill in
// Return:    S_OK if successful; error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::Commit(DWORD grfCommitFlags)
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::Revert
// Purpose:   unimplemented
// Arguments: UNDONE-WARN: Fill in
// Return:    S_OK if successful; error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::Revert()
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::LockRegion
// Purpose:   unimplemented
// Arguments: UNDONE-WARN: Fill in
// Return:    S_OK if successful; error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::LockRegion(ULARGE_INTEGER bOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::UnlockRegion
// Purpose:   unimplemented
// Arguments: UNDONE-WARN: Fill in
// Return:    S_OK if successful; error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::UnlockRegion(ULARGE_INTEGER bOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::Stat
// Purpose:   unimplemented
// Arguments: UNDONE-WARN: Fill in
// Return:    S_OK if successful; error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::Clone
// Purpose:   unimplemented
// Arguments: UNDONE-WARN: Fill in
// Return:    S_OK if successful; error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::Clone(IStream **ppstm)
{
    return E_NOTIMPL;
}
