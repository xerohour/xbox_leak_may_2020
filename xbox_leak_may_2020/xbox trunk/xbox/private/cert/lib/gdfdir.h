/*
 *
 * gdfdir.h
 *
 * Manipulate GDF directories
 *
 */

#ifndef _GDFDIR_H
#define _GDFDIR_H

#include <gdformat.h>

class GDIR {
public:
    GDIR(CFileStmSeq *pstm, DWORD cb, LPCSTR szName=NULL);
    GDIR(PBYTE pb, DWORD cb, LPCSTR szName=NULL);
    ~GDIR()
    {
        if(m_pbDirData)
            free(m_pbDirData);
        if(m_szName)
            free((PVOID)m_szName);
    }

    DWORD CbSize(void) const { return m_cbSize; }
    BOOL FGotData(void) const { return m_pbDirData != NULL; }
    BOOL FWriteToStm(CFileStmSeq *pstm) const;
    PGDF_DIRECTORY_ENTRY PgdeGetNextEntry(void);
    void ResetWalk(void);
    LPCSTR SzName(void) const { return m_szName ? m_szName : ""; }

private:
    PBYTE m_pbDirData;
    DWORD m_cbSize;
    LPCSTR m_szName;

    PGDF_DIRECTORY_ENTRY m_pgdeGet;
};

#endif // _GDFDIR_H
