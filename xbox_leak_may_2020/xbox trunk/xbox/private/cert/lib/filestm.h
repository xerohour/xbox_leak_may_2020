/*
 *
 * filestm.h
 *
 * file access classes
 *
 */

#ifndef _FILESTM_H
#define _FILESTM_H

class CFileStmSeq
{
public:
    virtual BOOL FIsOpen(void) =0;
    virtual DWORD CbRead(DWORD cb, PBYTE pbBuf) =0;
    virtual DWORD CbWrite(DWORD cb, const BYTE *pbBuf) =0;
    virtual BOOL FCopyToStm(CFileStmSeq *pstm, CCheckSum *pxsum=NULL) =0;
    virtual ~CFileStmSeq() {}
protected:
    virtual BOOL FPreferredReceive(void) { return FALSE; }
    virtual BOOL FCopyFromStm(CFileStmSeq *pstm, CCheckSum *) { return FALSE; }
friend class CFileStmRandWithCopy;
};

class CFileStmRand : public CFileStmSeq
{
public:
    virtual BOOL FSeek(LARGE_INTEGER li) =0;
    virtual BOOL FSeek(LONG l) =0;
};

class CFileStmRandWithCopy : public CFileStmRand
{
public:
    virtual BOOL FCopyToStm(CFileStmSeq *pstm, CCheckSum *pxsum=NULL);
};

class CHandleFile : public CFileStmRandWithCopy
{
public:
    CHandleFile(HANDLE h)
    {
        m_hFile = h == INVALID_HANDLE_VALUE ? NULL : h;
    }
    virtual BOOL FIsOpen(void);
    virtual DWORD CbRead(DWORD cb, PBYTE pbBuf);
    virtual DWORD CbWrite(DWORD cb, const BYTE *pbBuf);
    virtual BOOL FSeek(LARGE_INTEGER li);
    virtual BOOL FSeek(LONG l);
    operator HANDLE() { return m_hFile; }
protected:
    HANDLE m_hFile;
};

class CDiskFileU : public CHandleFile
{
public:
    BOOL FReopen(DWORD dwFlags, DWORD dwSharing=0);
    void SetDeleteOnClose(BOOL f) { m_fDeleteOnClose = f; }
    virtual ~CDiskFileU();
protected:
    BOOL m_fDeleteOnClose;

    CDiskFileU(HANDLE hFile=NULL) : CHandleFile(hFile)
    {
        m_fDeleteOnClose = FALSE;
    }

    void DoOpen(DWORD dwFlags, DWORD dwSharing);
    void HandleDeleteOnClose(void);

    virtual const void *PvName(void) =0;
    virtual HANDLE DoCreateFile(const void *szName, DWORD dwAccess,
        DWORD dwSharing, LPSECURITY_ATTRIBUTES psa, DWORD dwDisp,
        DWORD dwFlags, HANDLE hT) =0;
    virtual void DoDeleteFile(const void *szName) =0;
};

class CDiskFile : public CDiskFileU
{
public:
    CDiskFile(LPCSTR szName, DWORD dwFlags=GENERIC_READ | GENERIC_WRITE,
        DWORD dwSharing=0);
    LPCSTR SzName(void) const { return m_szName; }
    virtual ~CDiskFile()
    {
        HandleDeleteOnClose();
    }

protected:
    char m_szName[MAX_PATH+1];

    CDiskFile(HANDLE hFile=NULL) : CDiskFileU(hFile)
    {
        m_szName[0] = 0;
    }

    virtual const void *PvName(void) { return (const void *)m_szName; };
    virtual HANDLE DoCreateFile(const void *szName, DWORD dwAccess,
        DWORD dwSharing, LPSECURITY_ATTRIBUTES psa, DWORD dwDisp,
        DWORD dwFlags, HANDLE hT)
    {
        return ::CreateFileA((LPCSTR)szName, dwAccess, dwSharing, psa, dwDisp,
            dwFlags, hT);
    }
    virtual void DoDeleteFile(const void *szName)
    {
        ::DeleteFileA((LPCSTR)szName);
    }
};

class CDiskFileW : public CDiskFileU
{
public:
    CDiskFileW(LPCWSTR wzName, DWORD dwFlags=GENERIC_READ | GENERIC_WRITE,
        DWORD dwSharing=0);
    LPCWSTR WzName(void) const { return m_wzName; }
    virtual ~CDiskFileW()
    {
        HandleDeleteOnClose();
    }

protected:
    WCHAR m_wzName[MAX_PATH+1];

    CDiskFileW(HANDLE hFile=NULL) : CDiskFileU(hFile)
    {
        m_wzName[0] = 0;
    }

    virtual const void *PvName(void) { return (const void *)m_wzName; };
    virtual HANDLE DoCreateFile(const void *wzName, DWORD dwAccess,
        DWORD dwSharing, LPSECURITY_ATTRIBUTES psa, DWORD dwDisp,
        DWORD dwFlags, HANDLE hT)
    {
        return ::CreateFileW((LPCWSTR)wzName, dwAccess, dwSharing, psa, dwDisp,
            dwFlags, hT);
    }
    virtual void DoDeleteFile(const void *wzName)
    {
        ::DeleteFileW((LPCWSTR)wzName);
    }
};

#endif // _FILESTM_H
