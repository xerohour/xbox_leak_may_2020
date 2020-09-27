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

extern CCheckSum g_xsum;
extern bool g_fInPlaceholder, g_fInXbe, g_fInMedia;

class CMyFileStm : public CFileStmRand
{
    friend class CFile;
public:
    virtual BOOL FIsOpen(void)
    {
        return m_pfile && m_pfile->IsInited();
    }
    virtual DWORD CbRead(DWORD cb, PBYTE pbBuf)
    {
        int cbRead;
        if(!m_pfile)
            return 0;
        if(FAILED(m_pfile->Read(pbBuf, cb, &cbRead)))
            return 0;
        return cbRead;
    }
    virtual DWORD CbWrite(DWORD cb, const BYTE *pbBuf)
    {
        if(!m_pfile)
            return 0;
        return SUCCEEDED(m_pfile->Write((PBYTE)pbBuf, cb)) ? cb : 0;
    }
    
    virtual BOOL FSeek(LARGE_INTEGER li)
    {
        return SUCCEEDED(m_pfile->SeekTo(li));
    }
    virtual BOOL FSeek(LONG l)
    {
        LARGE_INTEGER li;
        li.QuadPart = l;
        return SUCCEEDED(m_pfile->SeekTo(li));
    }

    virtual BOOL FCopyToStm(CFileStmSeq *pstm, CCheckSum *) { return FALSE; }
    virtual ~CMyFileStm()
    {
        if(m_pfile)
            m_pfile->m_pstm = NULL;
    }

private:
    CMyFileStm(CFile *pfile) : m_pfile(pfile) {}
    CFile *m_pfile;
};


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// BATCH_SIZE       -- Number of bytes to copy at a time when copying a file.  Included for better
//                     performance.

#define BATCH_SIZE 32768


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFile::CFile
// Purpose:   Constructor for the CFile class.  Opens the specified file in preparation for READING
//            This will only be called on disk sources.
// Arguments: pdlg              -- The dialog that will handle informing the user of errors.
//            pstep             -- The step that created this file.
//            szFilename        -- name of the file to open
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CFile::CFile(CDlg *pdlg, CStep_Progress *pstep, char *szFilename) : m_pstm(NULL)
{
    // Validate parameters
    if (pdlg == NULL || szFilename == NULL)
    {
        SetInited(E_INVALIDARG);
        return;
    }

    // Track the dialog and step that this file was created in
    m_pdlg  = pdlg;
    m_pstep = pstep;

    // Track which media this file exists on.
    m_media = MEDIA_DISK;

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
// Function:  CFile::CFile
// Purpose:   Opens the specified file for read or write access.  This can be called on disk or
//            tape sources.
// Arguments: pdlg              -- The dialog that will handle informing the user of errors.
//            pstep             -- The step that created this file.
//            media             -- Type of media the file exists on (tape, disk)
//            szPath            -- Path the file exists in.  Ignored if media = tape
//            szFilename        -- name of the file to open (without path).
//            fInput            -- 'true' if open for read, 'false' if open for write
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CFile::CFile(CDlg *pdlg, CStep_Progress *pstep, eMedia media, char *szPath, char *szFilename,
             bool fInput) : m_pstm(NULL)
{
    // Validate parameters
    if (pdlg == NULL || szFilename == NULL)
    {
        SetInited(E_INVALIDARG);
        return;
    }

    // Track the dialog and step that this file was created in
    m_pdlg  = pdlg;
    m_pstep = pstep;

    // Track which media this file exists on.
    m_media = media;
    
    // Track the last action performed
    m_taskLast = TASK_OPEN;

    // Open the file
    if (media == MEDIA_DISK)
    {
        sprintf(m_szFilename, "%s\\%s", szPath, szFilename);
        if (fInput)
            m_hFile = CreateFile(m_szFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL, 0);
        else
            m_hFile = CreateFile(m_szFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL, 0);
        if (m_hFile == INVALID_HANDLE_VALUE)
        {
            // NOTE: Non-OLE compatible error code.  Acceptable here ONLY because I catch it elsewhere
            // (see note at top of this file).  Difficult to use OLE for errors here since it doesn't
            // have a lot of errors we need to catch (ie file not found).
            SetInited(GetLastError());
            return;
        }
    }
    else
    {
        DWORD dw1,dw2;
        BOOL fOpened;

        strcpy(m_szFilename, szFilename);
        SetCursor(LoadCursor(NULL, IDC_WAIT));
        if (fInput)
            fOpened = pstep->m_td.FReadFile(szFilename, &dw1,&dw2);
        else
            fOpened = pstep->m_td.FWriteFile(szFilename);
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        if (!fOpened)
        {
            // Generic error since CTapeDevice doesn't provide more detailed error information.
            SetInited(ERROR_ACCESS_DENIED);
            return;
        }
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
    if(m_pstm)
        delete m_pstm;
}

DWORD g_cbyTotalWritten = 0;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFile::Write
// Purpose:   Write the specified buffer of bytes out to our file
// Arguments: pby           -- Buffer containing bytes to write
//            cBytes        -- Number of bytes to write
// Return:    Error code, or S_OK if written successfully
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CFile::Write(BYTE *pby, UINT cBytes)
{
    DWORD dwWritten;

    if (!IsInited())
        return E_FAIL;

    // Track the last action performed
    m_taskLast = TASK_WRITE;

    if (m_media == MEDIA_DISK)
    {
        if (!::WriteFile(m_hFile, pby, cBytes, &dwWritten, 0) || dwWritten != cBytes)
        {
            // Failed to write the whole buffer.
            return(GetLastError());
        }
    }
    else
    {
        dwWritten = m_pstep->m_td.CbWrite(cBytes, pby);
        if (dwWritten == 0)
        {
            // An error occurred; we can't tell what the error is since CTapeDevice doesn't have
            // more robust error reporting.
            return E_FAIL;
        }
    }

    // Update the Submission checksum.  The Black XCheck checksum is calculated elsewhere
    if (!g_fInPlaceholder && !g_fInXbe && g_fInMedia)
        g_xsum.SumBytes(pby, cBytes);
    g_cbyTotalWritten += cBytes;

    return S_OK;
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

    assert (m_media == MEDIA_DISK);

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

    if (m_media == MEDIA_DISK)
    {
        if (!ReadFile(m_hFile, pby, cBytes, &dwRead, 0))
        {
            // Failed to read the whole buffer.
            if (pnRead)
                *pnRead = dwRead;
            return (GetLastError());
        }
    }
    else
    {
        dwRead = m_pstep->m_td.CbRead(cBytes, pby);
        if (dwRead == 0)
        {
            // An error occurred; we can't tell what the error is since CTapeDevice doesn't have
            // more robust error reporting.
            return E_FAIL;
        }
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
    assert (m_media == MEDIA_DISK);

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
// Function:  CFile::InformError
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CFile::InformError(HWND hwnd, HRESULT hrError, char *szFilename)
{
    char szCaption[MAX_PATH + 200];
    char szText[100];
    
    // UNDONE: Mixing OLE and non-ole error codes here.  It's okay because none of them conflict,
    // but this should be resolved if this class is ever extended to work with other apps.
    switch(hrError)
    {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
        // Failed to find the specified file
        sprintf(szText, "Failed to find the file '%s'", szFilename);
        sprintf(szCaption, "File not found");
        break;

    case ERROR_DISK_CORRUPT:
    case ERROR_ACCESS_DENIED:
    case ERROR_FILE_INVALID:
        sprintf(szText, "Cannot access the file '%s'.  Please verify that the file is not currently"\
                        " opened by another appliation", szFilename);
        sprintf(szCaption, "Cannot access file");
        break;
        
    case ERROR_DISK_FULL:
        sprintf(szText, "The drive '%c:' is full; xbPremaster cannot continue writing data.",
                szFilename[0]);
        sprintf(szCaption, "Not enough space on drive");
        break;

    default:
        // What was the last action performed?
        switch(m_taskLast)
        {
        case TASK_WRITE:
            sprintf(szText, "An error occurred while writing the file '%s'.  Please verify the " \
                            "output media is valid.", szFilename);
            sprintf(szCaption, "Error writing file");
            break;

        case TASK_READ:
            sprintf(szText, "An error occurred while reading the file '%s'.  Please verify the " \
                            "input media is valid.", szFilename);
            sprintf(szCaption, "Error reading file");
            break;

        case TASK_OPEN:
            sprintf(szText, "An error occurred while opening the file '%s'.", szFilename);
            sprintf(szCaption, "Error opening file");
            break;

        case TASK_CLOSE:
            sprintf(szText, "An error occurred while closing the file '%s'.", szFilename);
            sprintf(szCaption, "Error closing file");
            break;

        case TASK_SEEK:
            sprintf(szText, "An error occurred while seeking in the file '%s'.", szFilename);
            sprintf(szCaption, "Error seeking in file");
            break;

        default:
            sprintf(szText, "An error occurred accessing the file '%s'.  Please verify it is a " \
                            "valid filename", szFilename);
            sprintf(szCaption, "Error accessing file");
            break;
        }
        break;
        
    }

    // Display the error message to the user
    MessageBox(hwnd, szText, szCaption, MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
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
    if (m_media == MEDIA_DISK)
    {
        if (m_hFile)
        {
            CloseHandle(m_hFile);
            m_hFile = NULL;
        }
    }
    else
    {
        if (m_pstep->m_td.FIsOpen())
            if (!m_pstep->m_td.FCloseFile())
                return E_FAIL;
    }

    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFile::CopyFileBytes
// Purpose:   Copy the requested number of bytes from the current location of the source file
// Arguments: pfileSrc          -- File to copy from
//            nBytesLeft        -- Number of bytes to copy
//            pnWritten         -- Will contain the number of bytes written
//            phr               -- result of the copy (S_OK if cancel)
// Return:    'true' to continue; 'false' if error or user cancelled
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CFile::CopyFileBytes(CFile *pfileSrc, int nBytesLeft, int *pnWritten, HRESULT *phr)
{
    // Batch into blocks of BATCH_SIZE for perf
    BYTE rgbyBuffer[BATCH_SIZE];
    int  nRead, nToRead;
    
    // Start out with no bytes written.
    *pnWritten = 0;
    
    do
    {
        // Read in the next batch from the source file
        nToRead = min(nBytesLeft, BATCH_SIZE);
        *phr = pfileSrc->Read(rgbyBuffer, nToRead, &nRead);
        if (*phr != S_OK)
            return false;

        if(nRead != nToRead)
            memset(rgbyBuffer + nRead, 0, nToRead - nRead);
        
        // Check if the user clicked 'cancel'
        if (m_pstep->CheckCancelled())
            return false;

        // Write out the latest batch to this file
        *phr = Write(rgbyBuffer, nToRead);
        if (*phr != S_OK)
            return false;
            
        // Keep track of the total number of bytes read and written.
        nBytesLeft -= nToRead;
        *pnWritten += nToRead;
    }
    while (nBytesLeft);

    // If here, then the bytes were successfully copied.
    return true;
}

HRESULT CFile::CreateFileStm(CFileStmRand **ppstm)
{
    if(!m_pstm)
        m_pstm = new CMyFileStm(this);
    *ppstm = m_pstm;
    return m_pstm ? S_OK : E_FAIL;
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

