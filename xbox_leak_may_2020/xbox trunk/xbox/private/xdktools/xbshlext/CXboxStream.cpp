// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     CXboxStream.cpp
// Contents: Implementation of CXboxStream.  CXboxStream handles streaming data of a single file
//           across the wire.
// Revision History:
//           04-26-2001 : Created (jeffsim)
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
// Arguments: pixbconn          -- A connection to the xbox that we are streaming from.
//            tszRootFolder     -- The name of the folder that the object exists in.  This will be
//                                 of the form "XE:\Samples\BumpLens".
//            tszFile           -- The file we'll be streaming.  Note that it may exist in a subfolder
//                                 of the root folder.  An example would be "Media\Textures\Lake.bmp"
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CXboxStream::CXboxStream() : m_cRef(1)
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::~CXboxStream
// Purpose:   CXbox destructor
// Arguments: None.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CXboxStream::~CXboxStream()
{
    //
    // Close the file if we managed to open it.
    //
    if(m_hFile)
    {
        CloseHandle(m_hFile);
    }
    
    //
    //  Delete the file, if we were asked to.
    //
    if(m_fDeleteOnFinalRelease)
    {
        DeleteFileA(m_szFilename);
    }
}


HRESULT CXboxStream::Initialize(LPCSTR pszFilename, bool fDeleteOnFinalRelease)
{
    HRESULT hr = S_OK;
    m_fDeleteOnFinalRelease = fDeleteOnFinalRelease;
    strcpy(m_szFilename, pszFilename);

    m_hFile = CreateFileA(
                m_szFilename,
                FILE_GENERIC_READ,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                0);

    if(INVALID_HANDLE_VALUE==m_hFile)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    return hr;
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
    // Verify that we initialized properly
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
    // Verify that we initialized properly
    if (0 == InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return (ULONG)m_cRef;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ IMPLEMENTED ISTREAM METHODS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
    if (!ReadFile(m_hFile, pv, cb, pcbRead, NULL))
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
    WriteFile(m_hFile, pv, cb, pcbWritten, NULL);
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
    li.LowPart = SetFilePointer(m_hFile, li.LowPart, &li.HighPart, dwOrigin);

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
// ++++ UNIMPLEMENTED ISTREAM METHODS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::SetSize
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::SetSize(ULARGE_INTEGER cbNewSize)
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::CopyTo
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead,
                                 ULARGE_INTEGER *pcbWritten)
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::Commit
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::Commit(DWORD grfCommitFlags)
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::Revert
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::Revert()
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::LockRegion
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::LockRegion(ULARGE_INTEGER bOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::UnlockRegion
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::UnlockRegion(ULARGE_INTEGER bOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::Stat
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
    return E_NOTIMPL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXboxStream::Clone
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CXboxStream::Clone(IStream **ppstm)
{
    return E_NOTIMPL;
}
