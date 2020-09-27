/*
 *
 * filestm.h
 *
 * file access classes
 *
 */

#include "precomp.h"

void CDiskFileU::DoOpen(DWORD dwFlags, DWORD dwSharing)
{
    DWORD dwDisp;
    DWORD dwAttr = (dwFlags & 0xFFFF) << 16;

    m_hFile = NULL;
    dwFlags &= GENERIC_READ | GENERIC_WRITE;
    if(dwFlags) {
        if(!(dwFlags & GENERIC_WRITE))
            dwSharing |= FILE_SHARE_READ;
        if(!(dwFlags & GENERIC_READ))
            dwDisp = CREATE_ALWAYS;
        else
            dwDisp = OPEN_EXISTING;
        m_hFile = DoCreateFile(PvName(), dwFlags, dwSharing, NULL, dwDisp,
            dwAttr, NULL);
        if(INVALID_HANDLE_VALUE == m_hFile)
            m_hFile = NULL;
    }
}

CDiskFile::CDiskFile(LPCSTR szName, DWORD dwFlags, DWORD dwSharing)
{
    strcpy(m_szName, szName);
    m_fDeleteOnClose = FALSE;
    DoOpen(dwFlags, dwSharing);
}

CDiskFileW::CDiskFileW(LPCWSTR wzName, DWORD dwFlags, DWORD dwSharing)
{
    wcscpy(m_wzName, wzName);
    m_fDeleteOnClose = FALSE;
    DoOpen(dwFlags, dwSharing);
}

BOOL CDiskFileU::FReopen(DWORD dwFlags, DWORD dwSharing)
{
    if(m_hFile)
        CloseHandle(m_hFile);
    DoOpen(dwFlags, dwSharing);
    return m_hFile != NULL;
}

BOOL CHandleFile::FIsOpen(void)
{
    return m_hFile != NULL;
}

DWORD CHandleFile::CbRead(DWORD cb, PBYTE pbBuf)
{
    DWORD cbRead;
    if(!m_hFile)
        return 0;
    return ReadFile(m_hFile, pbBuf, cb, &cbRead, NULL) ? cbRead : 0;
}

DWORD CHandleFile::CbWrite(DWORD cb, const BYTE *pbBuf)
{
    DWORD cbWritten;
    if(!m_hFile)
        return 0;
    return WriteFile(m_hFile, pbBuf, cb, &cbWritten, NULL) ? cbWritten : 0;
}

BOOL CHandleFile::FSeek(LARGE_INTEGER li)
{
    if(!m_hFile)
        return FALSE;
    return SetFilePointerEx(m_hFile, li, NULL, FILE_BEGIN);
}

BOOL CHandleFile::FSeek(LONG l)
{
    if(!m_hFile)
        return FALSE;
    return INVALID_SET_FILE_POINTER != SetFilePointer(m_hFile, l, NULL,
        FILE_BEGIN);
}

void CDiskFileU::HandleDeleteOnClose(void)
{
    if(m_fDeleteOnClose) {
        DoDeleteFile(PvName());
        m_fDeleteOnClose = FALSE;
    }
}

CDiskFileU::~CDiskFileU()
{
    if(m_hFile)
        CloseHandle(m_hFile);
    /* We can't delete from here; if the inherited class didn't clean up, this
     * will force a purecall reference */
    HandleDeleteOnClose();
}

BOOL CFileStmRandWithCopy::FCopyToStm(CFileStmSeq *pstm, CCheckSum *pxsum)
{
    BYTE rgb[8192];
    DWORD cb;

    if(pstm->FPreferredReceive())
        return pstm->FCopyFromStm(this, pxsum);

    if(!FIsOpen() || !pstm->FIsOpen())
        return FALSE;

    /* As long as we have data in our stream, write it out */
    for(;;) {
        cb = CbRead(sizeof rgb, rgb);
        if(cb == 0)
            return TRUE;
        if(pxsum)
            pxsum->SumBytes(rgb, cb);
        if(cb != pstm->CbWrite(cb, rgb))
            return FALSE;
    }
}
