/*
 *
 * tape2gdf.h
 *
 * Build a compacted GDF image from tape images
 *
 */

#include <windows.h>
#include <stdio.h>
#include <xsum.h>
#include <filestm.h>
#include <malloc.h>
#include <gdfdir.h>
#include <xboxdbg.h>

class GDQ {
public:
    GDQ(void) { m_pgdqeFirst = NULL; m_ppgdqeLast = &m_pgdqeFirst; }
    ~GDQ() { while(PopDir()); }

    void PushDir(GDIR *pgdir);
    GDIR *PopDir(void);

private:
    struct GDQE {
        GDQE *pgdqeNext;
        GDIR *pgdir;
    };

    GDQE *m_pgdqeFirst;
    GDQE **m_ppgdqeLast;
};

class CXboxFile : public CFileStmSeq
{
public:
    CXboxFile(LPCSTR szName, DWORD cb);
    virtual BOOL FIsOpen(void) { return g_pxfCur == this; }
    virtual DWORD CbRead(DWORD cb, PBYTE pbBuf) { return FALSE; }
    virtual DWORD CbWrite(DWORD cb, const BYTE *pbBuf);
    virtual BOOL FCopyToStm(CFileStmSeq *pstm, CCheckSum *pxsum=NULL) { return FALSE; }
    virtual ~CXboxFile();
protected:
    static CXboxFile *g_pxfCur;
    DWORD m_cbLeft;
};
