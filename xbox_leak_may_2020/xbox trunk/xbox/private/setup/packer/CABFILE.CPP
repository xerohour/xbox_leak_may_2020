//  CABFILE.CPP
//
//  Created 12-Mar-2001 [JonT]

#include "packer.h"

//---------------------------------------------------------------------
// FCI callback prototypes

INT_PTR DIAMONDAPI callback_file_open(char FAR *pszFile, int oflag, int pmode, int FAR *err, void FAR *pv);
unsigned int DIAMONDAPI callback_file_read(INT_PTR hf, void FAR *memory, UINT cb, int FAR *err, void FAR *pv);
unsigned int DIAMONDAPI callback_file_write(INT_PTR hf, void FAR *memory, UINT cb, int FAR *err, void FAR *pv);
int DIAMONDAPI callback_file_close(INT_PTR hf, int FAR *err, void FAR *pv);
long DIAMONDAPI callback_file_seek(INT_PTR hf, long dist, int seektype, int FAR *err, void FAR *pv);
int DIAMONDAPI callback_file_placed(PCCAB pccab, char *pszFile, long  cbFile, BOOL  fContinuation, void FAR *pv);
void HUGE * DIAMONDAPI callback_file_alloc(ULONG cb);
void DIAMONDAPI callback_file_free(void HUGE *memory);
int DIAMONDAPI callback_file_delete(char FAR *pszFile, int FAR *err, void FAR *pv);
BOOL DIAMONDAPI callback_file_template(char *pszTempName, int   cbTempName, void FAR *pv);
BOOL DIAMONDAPI callback_file_next_cab(PCCAB  pccab, ULONG  cbPrevCab, void FAR *pv);
long DIAMONDAPI callback_file_status(UINT   typeStatus, ULONG  cb1, ULONG  cb2, void FAR *pv);
INT_PTR DIAMONDAPI callback_file_openinfo(char   *pszName, USHORT *pdate, USHORT *ptime, USHORT *pattribs, int FAR *err, void FAR *pv);

//---------------------------------------------------------------------
// FCI callback routines

INT_PTR DIAMONDAPI
callback_file_open(
    char FAR *pszFile,
    int oflag,
    int pmode,
    int FAR *err,
    void FAR *pv
        )
{
    int result;

    result = _open(pszFile, oflag, pmode);

    if (result == -1)
        *err = errno;

    return result;
}

unsigned int DIAMONDAPI
callback_file_read(
    INT_PTR hf,
    void FAR *memory,
    UINT cb,
    int FAR *err,
    void FAR *pv
        )
{
    unsigned int result;

    result = (unsigned int) _read((HFILE)hf, memory, cb);

    if (result != cb)
        *err = errno;

    return result;
}

unsigned int DIAMONDAPI
callback_file_write(
    INT_PTR hf,
    void FAR *memory,
    UINT cb,
    int FAR *err,
    void FAR *pv
        )
{
    unsigned int result;

    result = (unsigned int) _write((HFILE)hf, memory, cb);

    if (result != cb)
        *err = errno;

    return result;
}


int DIAMONDAPI
callback_file_close(
    INT_PTR hf,
    int FAR *err,
    void FAR *pv
        )
{
    int result;

    result = _close((HFILE)hf);

    if (result != 0)
        *err = errno;

    return result;
}


long DIAMONDAPI
callback_file_seek(
    INT_PTR hf,
    long dist,
    int seektype,
    int FAR *err,
    void FAR *pv
        )
{
    long result;

    result = _lseek((HFILE)hf, dist, seektype);

    if (result == -1)
        *err = errno;

    return result;
}

int DIAMONDAPI
callback_file_placed(
    PCCAB pccab,
    char *pszFile,
    long  cbFile,
    BOOL  fContinuation,
    void FAR *pv
    )
{
    return 0;
}

void HUGE * DIAMONDAPI
callback_file_alloc(ULONG cb)
{
    return malloc(cb);
}

void DIAMONDAPI
callback_file_free(void HUGE *memory)
{
    free(memory);
}

int DIAMONDAPI
callback_file_delete(
    char FAR *pszFile,
    int FAR *err,
    void FAR *pv
    )
{
    int result;

    result = remove(pszFile);

    if (result != 0)
        *err = errno;

    return result;
}


BOOL DIAMONDAPI
callback_file_template(
    char *pszTempName,
    int   cbTempName,
    void FAR *pv
    )
{
    char    *psz;

    psz = _tempnam("","xx");            // Get a name
    if ((psz != NULL) && (strlen(psz) < (unsigned)cbTempName))
    {
        strcpy(pszTempName,psz);        // Copy to caller's buffer
        free(psz);                      // Free temporary name buffer
        return TRUE;                    // Success
    }
    //** Failed
    if (psz)
    {
        free(psz);
    }

    return FALSE;
}

BOOL DIAMONDAPI
callback_file_next_cab(
    PCCAB pccab,
    ULONG cbPrevCab,
    void FAR *pv
    )
{
    return TRUE;
}

long DIAMONDAPI
callback_file_status(
    UINT typeStatus,
    ULONG cb1,
    ULONG cb2,
    void FAR *pv
    )
{
    return 0;
}

INT_PTR DIAMONDAPI
callback_file_openinfo(
    char *pszName,
    USHORT *pdate,
    USHORT *ptime,
    USHORT *pattribs,
    int FAR *err,
    void FAR *pv
    )
{
    BY_HANDLE_FILE_INFORMATION finfo;
    FILETIME filetime;
    HANDLE handle;
    DWORD attrs;
    int hf;

    // Need a Win32 type handle to get file date/time
    // using the Win32 APIs, even though the handle we
    // will be returning is of the type compatible with
    // _open
    handle = CreateFileA(pszName, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);
    if (handle == INVALID_HANDLE_VALUE)
        return -1;

    if (GetFileInformationByHandle(handle, &finfo) == FALSE)
    {
        CloseHandle(handle);
        return -1;
    }

    FileTimeToLocalFileTime(&finfo.ftLastWriteTime, &filetime);
    FileTimeToDosDateTime(&filetime, pdate, ptime);
    attrs = GetFileAttributesA(pszName);
    if (attrs == 0xFFFFFFFF)
    {
        // failure
        *pattribs = 0;
    }
    else
    {
        // Mask out all other bits except these four, since other
        // bits are used by the cabinet format to indicate a
        // special meaning.
        *pattribs = (int) (attrs & (_A_RDONLY | _A_SYSTEM | _A_HIDDEN | _A_ARCH));
    }
    CloseHandle(handle);
    
    
    // Return handle using _open
    hf = _open( pszName, _O_RDONLY | _O_BINARY );
    if (hf == -1)
        return -1; // abort on error
    
    return hf;
}


//---------------------------------------------------------------------
//  CCabFile class

//  CCabFile::SetFile
//      Sets this instance to point to a specific CAB file and prepares the context.

BOOL
CCabFile::SetFile(
    LPCTSTR lpCabName,
    BOOL fTestOnly
    )
{
    TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFname[_MAX_FNAME], szExt[_MAX_EXT];

    // Setup cab
    memset(&m_cab, 0, sizeof (CCAB));
    m_cab.cb = 600 * 1024 * 1024;       // Cabinet has to fit on a CD
    m_cab.cbFolderThresh = m_cab.cb;    // Folder threshold: full size of cab
    m_cab.cbReserveCFHeader = 0;
    m_cab.cbReserveCFFolder = 0;
    m_cab.cbReserveCFData   = 0;
    m_cab.iCab = 1;
    m_cab.iDisk = 0;
    m_cab.setID = 0;

    // Parse the cab name
    _splitpath(lpCabName, szDrive, szDir, szFname, szExt);
    sprintf(m_cab.szCabPath, "%s%s", szDrive, szDir);
    sprintf(m_cab.szCab, "%s%s", szFname, szExt);

    // Make sure we don't have a context open
    if (m_hfci)
        FCIDestroy(m_hfci);

    // Save the testonly flag
    m_fTestOnly = fTestOnly;

    // Test to make sure we could create the file
    if (fTestOnly)
    {
        m_hfci = NULL;
        HANDLE hfile = CreateFile(lpCabName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
            CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL);
        if (hfile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hfile);
            return TRUE;
        }
        else                      
        {
            m_erf.erfOper = FCIERR_OPEN_SRC;
            m_erf.fError = TRUE;
            return FALSE;
        }
    }

    // Create the new context
    m_hfci = FCICreate(
        &m_erf,
        callback_file_placed,
        callback_file_alloc,
        callback_file_free,
        callback_file_open,
        callback_file_read,
        callback_file_write,
        callback_file_close,
        callback_file_seek,
        callback_file_delete,
        callback_file_template,
        &m_cab,
        NULL
        );

    if (!m_hfci)
    {
        fprintf(stderr, "Error: FCICreate failed: error code %08x\n", m_erf.erfOper);
        return FALSE;
    }

    return TRUE;
}


//  CCabFile::SetCompression
//      Sets the compression type and level

BOOL
CCabFile::SetCompression(
    LPCSTR lpType,
    LPCSTR lpLevel
    )
{
    if (_stricmp(lpType, "NONE") == 0)
    {
        m_dwCompression = tcompTYPE_NONE;
    }
    else if (_stricmp(lpType, "MSZIP") == 0)
    {
        m_dwCompression = tcompTYPE_MSZIP;
    }
    else if (_stricmp(lpType, "LZX") == 0)
    {
        DWORD dwLevel = atoi(lpLevel);
        if (dwLevel > 6)
        {
            ::ReportSettingsError(CSTR_COMPRESSIONLEVEL, "compression level for LZX must be in the range of 0-6\n");
            return FALSE;
        }
        m_dwCompression = TCOMPfromLZXWindow(dwLevel + 15);
    }
    else
    {
        ::ReportSettingsError(CSTR_COMPRESSIONTYPE, "Unrecognized compression type. Possible values are NONE, MSZIP, LZX");
        return FALSE;
    }
    return TRUE;
}


//  CCabFile::AddFile
//      Adds a file to the cabinet from the given source path.
//      The pathname stored in the cabinet will be the destination path.
//      Normally, this pathname is a relative pathname.

BOOL
CCabFile::AddFile(
    LPCTSTR lpSourcePath,
    LPCTSTR lpDestPath
    )
{
    // If we're just testing, see if we can find the file
    if (m_fTestOnly)
    {
        HANDLE hfile = CreateFile(lpSourcePath, GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hfile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hfile);
            return TRUE;
        }
        else                      
        {
            m_erf.erfOper = FCIERR_OPEN_SRC;
            m_erf.fError = TRUE;
            return FALSE;
        }
    }

    // Add this file to the cabinet
    if(!FCIAddFile(m_hfci, (LPTSTR)lpSourcePath, (LPTSTR)lpDestPath,
        FALSE, // file is not executable
        callback_file_next_cab, callback_file_status, callback_file_openinfo, (USHORT)m_dwCompression))
    {
        // Error reporting handled elsewhere
        return FALSE;
    }

    return TRUE;
}


//  CCabFile::Flush
//      Writes the cab out to disk

BOOL
CCabFile::Flush()
{
    if (m_fTestOnly)
        return TRUE;

    // Flush out the new cab file
    if (!FCIFlushCabinet(m_hfci, FALSE, callback_file_next_cab, callback_file_status))
    {
        fprintf(stderr, "Error: FCIFlushCabinet failed: error code %08x\n", m_erf.erfOper);
        return FALSE;
    }

    return TRUE;
}


//  CCabFile::~CCabFile
//      Destroys the context, we're done with the object

CCabFile::~CCabFile()
{
    if (m_hfci)
        FCIDestroy(m_hfci);
}

