// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFile.h
// Contents:  Handles File input/output.
// Revisions: 31-May-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ CLASSES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Forward Class declarations
class CStep_Progress;

typedef enum eTask {TASK_OPEN, TASK_WRITE, TASK_READ, TASK_SEEK, TASK_GETSIZE, TASK_CLOSE};


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Class:   CFile
// Purpose: Handles File input/output.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CFile : public CInitedObject
{
public:

    // ++++ CONSTRUCTION-RELATED FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // CFile            -- This constructor is only called for read files
    CFile(CDlg *pdlg, CStep_Progress *pstep, char *szFilename);
    
    // CFile            -- This constructor could be called for read or write files
    CFile(CDlg *pdlg, CStep_Progress *pstep, eMedia media, char *szPath, char *szFilename, bool fInput);

    // ~CFile           -- CFile destructor
    virtual ~CFile();

    // ++++ INPUT/OUTPUT-RELATED FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    // Read             -- Reads a set of bytes from the file
    HRESULT Read(BYTE *pby, UINT cBytes, int *pnRead = NULL);

    HRESULT ReadBYTE(BYTE *pbyData, int *pnRead = NULL);
    HRESULT ReadWORD(WORD *pwData, int *pnRead = NULL);
    HRESULT ReadDWORD(DWORD *pdwData, int *pnRead = NULL);
    HRESULT ReadString(char *sz, int *pnRead = NULL);

    // Write            -- Writes a set of bytes to the file
    HRESULT Write(BYTE *pby, UINT cBytes);

    // CopyFileBytes    -- Copies a set of bytes from another file to this file.
    bool CopyFileBytes(CFile *pfileSrc, int nBytesLeft, int *pnWritten, HRESULT *phr);


    // ++++ MEMBER-ACCESS FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // GetSize          -- Returns the size of the file
    HRESULT GetSize(ULARGE_INTEGER *puliSize);

    // GetFilename      -- Returns the name of the file
    char *GetFilename() {return m_szFilename; }


    // ++++ MISCELLANEOUS FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // InformError      -- Informs the user that an error has occurred
    void InformError(HWND hwnd, HRESULT hrError, char *szFilename);

    // Close            -- Closes the file
    HRESULT Close();

    // SeekTo           -- Sets the file's current file pointer to the specified location.
    HRESULT SeekTo(LARGE_INTEGER liDist);
    
    HRESULT CreateFileStm(CFileStmRand **ppstm);

    
private:

    // m_hFile          -- Handle to the Win32 file object that we encapsulate
    HANDLE m_hFile;

    // m_pstep          -- The step that this file was created in.
    CStep_Progress *m_pstep;

    // m_pdlg           -- The dialog that this file was created in.
    CDlg *m_pdlg;
    
    // m_media          -- The media the file exists on.
    eMedia m_media;

    // m_taskLast      -- The last task performed on this file
    eTask m_taskLast;

    class CMyFileStm *m_pstm;
    
protected:

    // m_szFilename     -- name of this file.
    char m_szFilename[MAX_PATH];

    friend class CMyFileStm;
};
