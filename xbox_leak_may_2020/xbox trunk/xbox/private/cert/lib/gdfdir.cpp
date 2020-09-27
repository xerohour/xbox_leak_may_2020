/*
 *
 * gdfdir.cpp
 *
 * Manipulate GDF directories
 *
 */

#include "precomp.h"

GDIR::GDIR(CFileStmSeq *pstm, DWORD cb, LPCSTR szName)
{
    m_szName = szName ? _strdup(szName) : NULL;
    m_pbDirData = (PBYTE)malloc(m_cbSize = cb);
    if(!pstm || pstm->CbRead(cb, m_pbDirData) != cb) {
        free(m_pbDirData);
        m_pbDirData = NULL;
    }
    m_pgdeGet = NULL;
}

GDIR::GDIR(PBYTE pb, DWORD cb, LPCSTR szName)
{
    m_szName = szName ? _strdup(szName) : NULL;
    m_pbDirData = pb;
    m_cbSize = cb;
}

BOOL GDIR::FWriteToStm(CFileStmSeq *pstm) const
{
    DWORD cb = ((m_cbSize + 2047) & ~2047) - m_cbSize;
    BYTE rgb[2048];

    if(pstm->CbWrite(m_cbSize, m_pbDirData) != m_cbSize)
        return FALSE;
    if(cb) {
        memset(rgb, -1, cb);
        if(pstm->CbWrite(cb, rgb) != cb)
            return FALSE;
    }
    return TRUE;
}

void GDIR::ResetWalk(void)
{
    if(m_cbSize) {
        m_pgdeGet = (PGDF_DIRECTORY_ENTRY)m_pbDirData;
        if(m_pgdeGet && (WORD)m_pgdeGet->LeftEntryIndex == (WORD)-1)
            m_pgdeGet = NULL;
    } else
        m_pgdeGet = NULL;
}


PGDF_DIRECTORY_ENTRY GDIR::PgdeGetNextEntry(void)
{
    PGDF_DIRECTORY_ENTRY pgde = m_pgdeGet;
    DWORD cb;

    if(!pgde)
        return pgde;
    cb = pgde->FileNameLength + ((PBYTE)pgde->FileName - (PBYTE)pgde);
    cb = (cb + 3) & ~3;
    m_pgdeGet = (PGDF_DIRECTORY_ENTRY)((PBYTE)pgde + cb);
    cb = (PBYTE)m_pgdeGet - m_pbDirData;
    if(cb >= m_cbSize)
        m_pgdeGet = NULL;
    else if((WORD)m_pgdeGet->LeftEntryIndex == (WORD)-1) {
        cb = (cb + 2048) & ~2047;
        if(cb >= m_cbSize)
            m_pgdeGet = NULL;
        else
            m_pgdeGet = (PGDF_DIRECTORY_ENTRY)(m_pbDirData + cb);
    }

    return pgde;
}
