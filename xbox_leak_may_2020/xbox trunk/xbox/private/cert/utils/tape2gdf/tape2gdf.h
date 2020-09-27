/*
 *
 * tape2gdf.h
 *
 * Build a compacted GDF image from tape images
 *
 */

#include <windows.h>
#include <atlbase.h>
#include <msxml.h>
#include <stdio.h>
#include <xsum.h>
#include <tape.h>
#include <malloc.h>
#include <dx2ml.h>
#include <xbefile.h>
#include <gdformat.h>
#include <gdfdir.h>
#include <time.h>
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

class CTapeDevice2 : public CTapeDevice
{
    friend class CBasedFile;
};

class CBasedFile : public CDiskFile
{
public:
    CBasedFile(CTapeDevice2 *ptap);
    CBasedFile(CDiskFile *pstm);
    virtual BOOL FSeek(LARGE_INTEGER li);
    virtual BOOL FSeek(LONG l);
private:
    LARGE_INTEGER m_liBase;
};

class CXboxDVDFile : public CFileStmRandWithCopy
{
public:
    CXboxDVDFile(void);
    virtual BOOL FIsOpen(void) { return m_pdcon != NULL; }
    virtual DWORD CbRead(DWORD cb, PBYTE pbBuf);
    virtual DWORD CbWrite(DWORD cb, const BYTE *pbBuf) { return 0; }
    virtual ~CXboxDVDFile();
    virtual BOOL FSeek(LARGE_INTEGER li);
    virtual BOOL FSeek(LONG l);
private:
    PDM_CONNECTION m_pdcon;
    ULONG m_iblkCur;
    BYTE m_rgbCur[2048];
    ULONG m_ibCur;
};

struct FOBJ
{
    FOBJ(LPCSTR szName, DWORD dwBlk, PGDF_DIRECTORY_ENTRY pgdeIn,
        GDIR *pgdirIn=NULL, DWORD dwBinBlkIn=0)
    {
        memset(this, 0, sizeof *this);
        dwBlkOrig = dwBlk;
        pgde = pgdeIn;
        pgdir = pgdirIn;
        dwBinBlk = dwBinBlkIn;
        if(szName)
            szFullName = _strdup(szName);
    }
    FOBJ *pfobjNext;
    DWORD dwBlkOrig;
    DWORD cblk;
    LPSTR szFullName;
    PGDF_DIRECTORY_ENTRY pgde;
    GDIR *pgdir;
    BOOL fXbe;
    DWORD dwBlkNew;
    CDiskFile *pstmNew;
    DWORD dwBinBlk;

    BOOL FOverlapsWithPlaceholder(void);
};

class CFST : public CDiskFile
{
public:
    CFST(char *szFileName) : CDiskFile(szFileName, GENERIC_WRITE) {
        m_fInited = FIsOpen() ? true : false;
    }

    bool IsInited() {
        return m_fInited;
    }
    BOOL FFlush(CDiskFile *pfssBinFile, CDiskFile *pfssIsoFile);
    
private:
    bool m_fInited;
};

class CFunctestCrypt : public CSimpleCrypt
{
public:
    CFunctestCrypt(void) : CSimpleCrypt(rgbPublicKey, rgbPrivateKey) {}
private:
    static const BYTE rgbPrivateKey[];
    static const BYTE rgbPublicKey[];
};
