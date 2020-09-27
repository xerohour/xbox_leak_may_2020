/*
 *
 * gdf2file.cpp
 *
 * Convert a GDF image to a file tree
 *
 */

#include "gdf2file.h"

BOOL g_fXbox;
PDM_CONNECTION g_pdcon;
CXboxFile *CXboxFile::g_pxfCur;

void GDQ::PushDir(GDIR *pgdir)
{
    GDQE *pgdqe = new GDQE;

    pgdqe->pgdir = pgdir;
    pgdqe->pgdqeNext = NULL;

    *m_ppgdqeLast = pgdqe;
    m_ppgdqeLast = &pgdqe->pgdqeNext;
}

GDIR *GDQ::PopDir(void)
{
    GDQE *pgdqe = m_pgdqeFirst;
    GDIR *pgdir;

    if(!pgdqe)
        return NULL;

    pgdir = pgdqe->pgdir;
    m_pgdqeFirst = pgdqe->pgdqeNext;
    delete pgdqe;
    if(!m_pgdqeFirst)
        m_ppgdqeLast = &m_pgdqeFirst;
    return pgdir;
}

CXboxFile::CXboxFile(LPCSTR szName, DWORD cb)
{
    char szCmd[MAX_PATH + 128];
    HRESULT hr;

    m_cbLeft = cb;
    if(!g_pxfCur) {
        if(!g_pdcon && FAILED(DmOpenConnection(&g_pdcon)))
            g_pdcon = NULL;
        if(g_pdcon) {
            sprintf(szCmd, "SENDFILE NAME=\"%s\" LENGTH=%lu", szName, cb);
            hr = DmSendCommand(g_pdcon, szCmd, NULL, 0);
            if(hr == XBDM_READYFORBIN)
                g_pxfCur = this;
            else if(SUCCEEDED(hr)) {
                DmCloseConnection(g_pdcon);
                g_pdcon = NULL;
            }
        }
    }
}

CXboxFile::~CXboxFile()
{
    DWORD cb;
    BYTE rgb[8192];

    if(g_pxfCur == this && g_pdcon) {
        if(m_cbLeft) {
            memset(rgb, 0, sizeof rgb);
            while(m_cbLeft) {
                cb = m_cbLeft;
                if(cb > sizeof rgb)
                    cb = sizeof rgb;
                if(FAILED(DmSendBinary(g_pdcon, rgb, cb))) {
                    DmCloseConnection(g_pdcon);
                    g_pdcon = NULL;
                    break;
                }
                m_cbLeft -= cb;
            }
        }
        DmReceiveStatusResponse(g_pdcon, NULL, 0);
        g_pxfCur = NULL;
    }
}

DWORD CXboxFile::CbWrite(DWORD cb, const BYTE *pbBuf)
{
    if(cb > m_cbLeft)
        cb = m_cbLeft;
    if(g_pxfCur != this || !g_pdcon || !cb)
        return 0;
    if(FAILED(DmSendBinary(g_pdcon, pbBuf, cb))) {
        DmCloseConnection(g_pdcon);
        g_pdcon = NULL;
        g_pxfCur = NULL;
        return 0;
    }
    m_cbLeft -= cb;
    return cb;
}

BOOL FCreateDirectory(LPCSTR szName)
{
    return g_fXbox ? SUCCEEDED(DmMkdir(szName)) : CreateDirectory(szName, NULL);
}

GDIR *PgdirCreate(CFileStmSeq *pstm, DWORD cb, LPCSTR szName)
{
    PBYTE pb;
    DWORD cbRead;

    /* Need to read in a multiple of sectors since we're doing unbuffered
     * I/O */
    cbRead = (cb + 2047) & ~2047;
    pb = (PBYTE)malloc(cbRead);
    if(!pstm->CbRead(cbRead, pb)) {
        free(pb);
        pb = NULL;
    }
    return new GDIR(pb, cb, szName);
}

int __cdecl main(int argc, char **argv)
{
    int i;
    CDiskFile *pstmIn;
    CFileStmSeq *pstmOut;
    GDF_VOLUME_DESCRIPTOR gvd;
    PGDF_DIRECTORY_ENTRY pgde;
    BYTE rgb[8192];
    GDIR *pgdir, *pgdirRoot, *pgdirT;
    GDQ gdq;
    char szFileName[MAX_PATH+1];
    DWORD dw;
    DWORD cb, cbCur, cbRead;
    LARGE_INTEGER liOffset;

    if(argc >= 2 && 0 == _stricmp(argv[1], "-xbox")) {
        --argc;
        ++argv;
        g_fXbox = TRUE;
    } else
        g_fXbox = FALSE;

    if(argc < 3) {
        fprintf(stderr, "usage: gdf2file iso tree\n");
        return 1;
    }

    /* Make sure the output dir tree exists */
    dw = GetFileAttributes(argv[2]);
    if(dw == -1) {
        if(!FCreateDirectory(argv[2])) {
            fprintf(stderr, "cannot create %s\n", argv[2]);
            return 1;
        }
    } else if(!(dw & FILE_ATTRIBUTE_DIRECTORY)) {
        fprintf(stderr, "%s exists and is not a directory\n", argv[2]);
        return 1;
    }
    pstmIn = new CDiskFile(argv[1], GENERIC_READ | (FILE_FLAG_NO_BUFFERING >>
        16));
    if(!pstmIn->FIsOpen()) {
        fprintf(stderr, "could not open %s\n", argv[1]);
        return 1;
    }

    /* Grab the GDF volume descriptor */
    if(!pstmIn->FSeek(GDF_VOLUME_DESCRIPTOR_SECTOR * 2048))
        goto badformat;
    if(pstmIn->CbRead(2048, (PBYTE)&gvd) != 2048) {
badformat:
        fprintf(stderr, "bad image format or read error\n");
        return 1;
    }
    if(memcmp(gvd.HeadSignature, GDF_VOLUME_DESCRIPTOR_SIGNATURE,
            GDF_VOLUME_DESCRIPTOR_SIGNATURE_LENGTH) ||
            memcmp(gvd.TailSignature, GDF_VOLUME_DESCRIPTOR_SIGNATURE,
            GDF_VOLUME_DESCRIPTOR_SIGNATURE_LENGTH))
        goto badformat;

    /* Read the root directory and push it on the dir stack */
    printf("Root dir at %d\n", gvd.RootDirectoryFirstSector);
    liOffset.QuadPart = gvd.RootDirectoryFirstSector;
    liOffset.QuadPart *= 2048;
    if(!pstmIn->FSeek(liOffset))
        goto badformat;
    pgdirRoot = PgdirCreate(pstmIn, gvd.RootDirectoryFileSize, argv[2]);
    if(!pgdirRoot->FGotData())
        goto badformat;
    gdq.PushDir(pgdirRoot);

    /* Now process all directories */
    while(pgdir = gdq.PopDir()) {
        pgdir->ResetWalk();
        while(pgde = pgdir->PgdeGetNextEntry()) {
            sprintf(szFileName, "%s\\%.*s", pgdir->SzName(),
                pgde->FileNameLength, pgde->FileName);
            printf("%s at %lu\n", szFileName, pgde->FirstSector);
            liOffset.QuadPart = pgde->FirstSector;
            liOffset.QuadPart *= 2048;
            if(!pstmIn->FSeek(liOffset))
                goto badformat;
            if(pgde->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                pgdirT = PgdirCreate(pstmIn, pgde->FileSize, szFileName);
                if(!pgdirT->FGotData())
                    goto badformat;
                if(!FCreateDirectory(szFileName)) {
writeerr:
                    fprintf(stderr, "could not create %s\n", szFileName);
                    return 1;
                }
                gdq.PushDir(pgdirT);
            } else {
                cb = pgde->FileSize;
                if(g_fXbox)
                    pstmOut = new CXboxFile(szFileName, cb);
                else
                    pstmOut = new CDiskFile(szFileName, GENERIC_WRITE);
                if(!pstmOut->FIsOpen())
                    goto writeerr;
                while(cb) {
                    cbCur = cb;
                    if(cbCur > sizeof rgb)
                        cbRead = cbCur = sizeof rgb;
                    else
                        cbRead = (cb + 2047) & ~2047;
                    if(pstmIn->CbRead(cbRead, rgb) != cbRead)
                        goto badformat;
                    if(pstmOut->CbWrite(cbCur, rgb) != cbCur)
                        goto writeerr;
                    cb -= cbCur;
                }
                delete pstmOut;
            }
        }
    }

    delete pstmIn;
    if(g_pdcon)
        DmCloseConnection(g_pdcon);

    return 0;
}
