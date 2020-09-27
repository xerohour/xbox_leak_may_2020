/*
 *
 * tape2gdf.cpp
 *
 * Build a compacted GDF image from tape images
 *
 */

#include "tape2gdf.h"

CBasedFile *pstmL0, *pstmL1;
CXboxDVDFile *pxdvd;
FOBJ *g_pfobjFirst;
GDF_VOLUME_DESCRIPTOR gvd;
CDX2MLFile g_xml;
DXML *g_pdxml;
CCryptContainer *pccSign;
CDevkitCrypt ccDev;

BOOL FWriteDirTree(GDIR *pgdir, CFileStmSeq *pstm, int *piblkCur, int *piblkDir)
{
    GDIR *pgdirT;
    PGDF_DIRECTORY_ENTRY pgde;

    /* We enumerate all of our entries, and for each child directory, we
     * ask that child to write itself out.  Once that's done, we've got all
     * of the updated block numbers for all of our own entries, so we write
     * ourselves out */
    pgdir->ResetWalk();
    while(pgde = pgdir->PgdeGetNextEntry()) {
        if(pgde->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            pgdirT = (GDIR *)pgde->FirstSector;
            if(!FWriteDirTree(pgdirT, pstm, piblkCur, (int *)&pgde->FirstSector))
                return FALSE;
            delete pgdirT;
        }
    }

    *piblkDir = *piblkCur;
    *piblkCur += (pgdir->CbSize() + 2047) / 2048;
    return pgdir->FWriteToStm(pstm);
}

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

void SortFobjList(FOBJ **ppfobj)
{
    FOBJ *pfobj1;
    FOBJ *pfobj2;
    FOBJ **ppfobj1;
    FOBJ **ppfobj2;

    pfobj1 = *ppfobj;
    if(!pfobj1 || !pfobj1->pfobjNext)
        return;

    ppfobj1 = ppfobj2 = &pfobj1;
    while(*ppfobj2) {
        ppfobj2 = &(*ppfobj2)->pfobjNext;
        if(*ppfobj2) {
            ppfobj1 = &(*ppfobj1)->pfobjNext;
            ppfobj2 = &(*ppfobj2)->pfobjNext;
        }
    }
    pfobj2 = *ppfobj1;
    *ppfobj1 = NULL;

    SortFobjList(&pfobj1);
    SortFobjList(&pfobj2);

    ppfobj1 = ppfobj;
    while(pfobj1 || pfobj2) {
        if(!pfobj1)
            ppfobj2 = &pfobj2;
        else if(!pfobj2)
            ppfobj2 = &pfobj1;
        else if(pfobj1->dwBlkOrig < pfobj2->dwBlkOrig)
            ppfobj2 = &pfobj1;
        else
            ppfobj2 = &pfobj2;
        *ppfobj1 = *ppfobj2;
        ppfobj1 = &(*ppfobj1)->pfobjNext;
        *ppfobj2 = (*ppfobj2)->pfobjNext;
    }
    *ppfobj1 = NULL;
}

void PushFobj(FOBJ *pfobj)
{
    /* Make sure this file doesn't overlap a placeholder */
    if(g_pdxml && pfobj->FOverlapsWithPlaceholder()) {
        fprintf(stderr, "warning: %s overlaps a placeholder\n",
            pfobj->szFullName);
    }
    pfobj->pfobjNext = g_pfobjFirst;
    g_pfobjFirst = pfobj;
}

CBasedFile::CBasedFile(CTapeDevice2 *ptap) : CDiskFile((HANDLE)NULL)
{
    LARGE_INTEGER li;

    if(ptap->m_ptod) {
        li.QuadPart = 0;
        if(SetFilePointerEx(ptap->m_hdev, li, &m_liBase, FILE_CURRENT))
            m_hFile = ptap->m_hdev;
    }
}

CBasedFile::CBasedFile(CDiskFile *pstm) : CDiskFile(*pstm)
{
    m_liBase.QuadPart = 0;
}

BOOL CBasedFile::FSeek(LONG l)
{
    LARGE_INTEGER li;

    li.QuadPart = l + m_liBase.QuadPart;
    return CDiskFile::FSeek(li);
}

BOOL CBasedFile::FSeek(LARGE_INTEGER li)
{
    li.QuadPart += m_liBase.QuadPart;
    return CDiskFile::FSeek(li);
}

CXboxDVDFile::CXboxDVDFile(void)
{
    if(FAILED(DmOpenConnection(&m_pdcon)))
        m_pdcon = NULL;
    m_iblkCur = 0;
    m_ibCur = sizeof m_rgbCur;
}

CXboxDVDFile::~CXboxDVDFile()
{
    if(m_pdcon)
        DmCloseConnection(m_pdcon);
}

DWORD CXboxDVDFile::CbRead(DWORD cb, PBYTE pb)
{
    HRESULT hr;
    DWORD cbT;
    char sz[64];
    DWORD cbRet = 0;

    while(cb) {
        if(m_ibCur < sizeof m_rgbCur) {
            cbT = sizeof m_rgbCur - m_ibCur;
            if(cbT > cb)
                cbT = cb;
            memcpy(pb, m_rgbCur + m_ibCur, cbT);
            m_ibCur += cbT;
            pb += cbT;
            cb -= cbT;
            cbRet += cbT;
            continue;
        }
        sprintf(sz, "DVDBLK BLOCK=%d", m_iblkCur++);
        hr = DmSendCommand(m_pdcon, sz, NULL, NULL);
        if(hr != XBDM_BINRESPONSE)
            hr = E_FAIL;
        else
            hr = DmReceiveBinary(m_pdcon, m_rgbCur, sizeof m_rgbCur, NULL);
        if(FAILED(hr)) {
            DmCloseConnection(m_pdcon);
            m_pdcon = NULL;
            break;
        }
        m_ibCur = 0;
    }

    return cbRet;
}

BOOL CXboxDVDFile::FSeek(LARGE_INTEGER li)
{
    if(li.LowPart % 2048 != 0)
        return FALSE;
    m_iblkCur = (ULONG)(li.QuadPart / 2048);
    m_ibCur = sizeof m_rgbCur;
    return TRUE;
}

BOOL CXboxDVDFile::FSeek(LONG l)
{
    if(l % 2048 != 0 || l < 0)
        return FALSE;
    m_iblkCur = l / 2048;
    m_ibCur = sizeof m_rgbCur;
    return TRUE;
}

CFileStmSeq *PstmSeekTo(DWORD lsn)
{
    CFileStmRand *pstm;
    LARGE_INTEGER li;

    if(pxdvd)
        pstm = pxdvd;
    else if(lsn < 1715632)
        pstm = pstmL0;
    else {
        lsn -= 1715632;
        pstm = pstmL1;
    }
    li.QuadPart = lsn;
	li.QuadPart *= 2048;
    return pstm->FSeek(li) ? pstm : NULL;
}

CDiskFile *PstmCreateTemp(LPCSTR szName, DWORD dwFlags)
{
    CDiskFile *pstm = new CDiskFile(szName, dwFlags);
    if(!pstm->FIsOpen()) {
        delete pstm;
        pstm = NULL;
    } else
        pstm->SetDeleteOnClose(TRUE);
    return pstm;
}

BOOL FGetXMLData(CFileStmSeq *pstm)
{
    CDiskFile *pstmXdr;
    CDiskFile *pstmXml;
    BOOL fRet = FALSE;
    DWORD cbRes;
    HRSRC hres;
    PBYTE pb;

    /* Get the XDR file handy in case we need it */
    pstmXdr = PstmCreateTemp("dx2ml.xdr", GENERIC_WRITE);
    if(!pstmXdr) {
noxdr:
        fprintf(stderr, "error: unable to create temporary XDR file\n");
        goto fatal;
    }
    hres = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(1), "FILE");
    if(!hres)
        goto noxdr;
    pb = (PBYTE)LockResource(LoadResource(GetModuleHandle(NULL), hres));
    if(!pb)
        goto noxdr;
    cbRes = SizeofResource(GetModuleHandle(NULL), hres);
    if(pstmXdr->CbWrite(cbRes, pb) != cbRes)
        goto noxdr;
    if(!pstmXdr->FReopen(GENERIC_READ))
        goto noxdr;

    pstmXml = PstmCreateTemp("dx2ml.xml", GENERIC_WRITE);
    if(!pstm->FCopyToStm(pstmXml))
    {
badxml:
        fprintf(stderr, "warning: could not read read XML file from tape\n");
        goto fatal;
    }
    if(!pstmXml->FReopen(GENERIC_READ))
        goto badxml;
    if(!g_xml.FReadFile(pstmXml->SzName(), FALSE))
        goto badxml;
    g_pdxml = g_xml.PdxmlGet();
    if(g_pdxml->rgdxl[0].rgdxrAreas[idxrXData].cpsnSize != 1715632 ||
        g_pdxml->rgdxl[0].rgdxrAreas[idxrXData].cpsnSize != 1715632)
    {
        g_pdxml = NULL;
        goto badxml;
    }

    fRet = TRUE;

fatal:
    delete pstmXml;
    delete pstmXdr;
    return fRet;
}

BOOL FMarkXbeFiles(void)
{
    int ilyr;
    int ixbi;
    DXL *pdxl;
    FOBJ *pfobj;
    DWORD dwBlkXml;

    ixbi = 0;
    for(ilyr = 0; ilyr < 2; ++ilyr) {
        pdxl = &g_pdxml->rgdxl[ilyr];
        if(pdxl->ixbiMac)
            break;
    }
    if(ilyr == 2)
        dwBlkXml = 2 * 1715632;
    else
        dwBlkXml = ilyr * 1715632 + pdxl->pxbi[0].lsnStart;

    for(pfobj = g_pfobjFirst; pfobj; pfobj = pfobj->pfobjNext) {
        if(pfobj->dwBlkOrig == dwBlkXml) {
            pfobj->fXbe = TRUE;
            if(++ixbi == pdxl->ixbiMac) {
                ixbi = 0;
                if(!ilyr++) {
                    pdxl = &g_pdxml->rgdxl[ilyr];
                    if(!pdxl->ixbiMac) 
                        ++ilyr;
                }
            }
            if(ilyr == 2)
                dwBlkXml = 2 * 1715632;
            else
                dwBlkXml = ilyr * 1715632 + pdxl->pxbi[0].lsnStart;
        } else if(pfobj->dwBlkOrig > dwBlkXml) {
            fprintf(stderr, "error: XBE file at block %d not in filesystem\n",
                dwBlkXml);
            return FALSE;
        }
    }

    return TRUE;
}

BOOL FSignXBE(FOBJ *pfobj, CDiskFile *pstmIn, CDiskFile *pstmOut)
{
    CXBEFile *pxbe;
    BOOL fRet = FALSE;

    pxbe = new CXBEFile(pstmIn);
    pxbe->UseCryptContainer(&ccDev);
    if(!pxbe->FVerifySignature()) {
badxbe:
        fprintf(stderr, "error: %s is not a valid XBE file\n",
            pfobj->szFullName);
        goto fatal;
    }
    pxbe->UseCryptContainer(pccSign);
    if(!pxbe->FSignFile())
        goto badxbe;
    if(!pxbe->FWriteToStm(pstmOut, pfobj->cblk * 2048))
        fprintf(stderr, "error writing to %s\n", pstmOut->SzName());
    else
        fRet = TRUE;
fatal:
    delete pxbe;
    return fRet;
}

BOOL FOBJ::FOverlapsWithPlaceholder(void)
{
    int ixph;
    DXL *pdxl;
    ULONG lsnFileStart;

    /* Any file which spans the layer transition will be considered to over-
     * lap a placeholder */
    if(pgde->FirstSector < 1715632 && pgde->FirstSector +
            (pgde->FileSize + 2047) / 2048 > 1715632)
        return TRUE;

    /* Find the first placeholder that comes after the start block of this
     * file, or the placeholder that contains the start block of this
     * file */
    if(pgde->FirstSector >= 1715632) {
        pdxl = &g_pdxml->rgdxl[1];
        lsnFileStart = pgde->FirstSector - 1715632;
    } else {
        pdxl = &g_pdxml->rgdxl[0];
        lsnFileStart = pgde->FirstSector;
    }

    for(ixph = 0; ixph < pdxl->cxph; ++ixph) {
        if(lsnFileStart < pdxl->rgxph[ixph].lsnStart)
            break;
        if(lsnFileStart >= pdxl->rgxph[ixph].lsnStart &&
                lsnFileStart < pdxl->rgxph[ixph].lsnStart + 4096)
            /* File starts inside of a placeholder */
            return TRUE;
    }

    if(ixph == pdxl->cxph)
        /* File starts after the last placeholder on the layer */
        return FALSE;

    /* Now we just need to check whether the last sector of this file falls
     * before the start of the next placeholder */
    return lsnFileStart + (pgde->FileSize + 2047) / 2048 >
        pdxl->rgxph[ixph].lsnStart;
}

int __cdecl main(int argc, char **argv)
{
    int i;
    BOOL fDvdEmu = FALSE;
    BOOL fRawImages = FALSE;
    BOOL fResign = FALSE;
    CTapeDevice2 tapSrc0, tapSrc1, *ptap;
    CDiskFile *pstmT0, *pstmT1;
    CFileStmSeq *pstmCur;
    CDiskFile *pstmOut;
    CDiskFile *pstmBinOut;
    CDiskFile *pstmCurOut;
    CFST *pfst = NULL;
    char szName[32];
    GDF_DIRECTORY_ENTRY gdeRoot;
    PGDF_DIRECTORY_ENTRY pgde;
    BYTE rgb[8192];
    int iblkCur, iblkDir;
    int iblkBinCur;
    GDIR *pgdir, *pgdirRoot, *pgdirT;
    GDQ gdq;
    DWORD cblkCur, cblk;
    char szFileName[MAX_PATH+1];
    FOBJ *pfobj;

    while(++argv, --argc) {
        if(0 == _stricmp(*argv, "-dvdemu"))
            fDvdEmu = TRUE;
        else if(0 == _stricmp(*argv, "-xbdvd"))
            pxdvd = new CXboxDVDFile;
        else if(0 == _stricmp(*argv, "-raw"))
            fRawImages = TRUE;
        else if(0 == _stricmp(*argv, "-resign")) {
            fResign = TRUE;
            pccSign = new CFunctestCrypt;
        } else
            break;
    }

    if(pxdvd && (fRawImages || fResign)) {
        fprintf(stderr, "can't use -xbdvd with -raw or -resign\n");
        return 1;
    }

    if(fRawImages && fResign) {
        fprintf(stderr, "can't use -raw and -resign together\n");
        return 1;
    }

    if(argc < (pxdvd ? 1 : 2)) {
        fprintf(stderr, "usage: tape2gdf [-dvdemu] [-raw | -resign] <image | -s> tape0 [tape1]\n");
        return 1;
    }

    if(0 == strcmp(argv[0], "-s")) {
        pstmOut = NULL;
        pstmBinOut = NULL;
    } else {
        if(fDvdEmu) {
            sprintf(szFileName, "%s.bin", argv[0]);
            pstmBinOut = new CDiskFile(szFileName, GENERIC_WRITE);
            if(!pstmBinOut->FIsOpen())
                goto noopenwrite;
            sprintf(szFileName, "%s.fst", argv[0]);
            pfst = new CFST(szFileName);
            if (!pfst->IsInited())
                goto noopenwrite;
            sprintf(szFileName, "%s.iso", argv[0]);
        } else {
            pstmBinOut = NULL;
            strcpy(szFileName, argv[0]);
        }
        pstmOut = new CDiskFile(szFileName, GENERIC_WRITE);
        if(!pstmOut->FIsOpen() || !pstmOut->FReopen(GENERIC_READ | GENERIC_WRITE)) {
noopenwrite:
            fprintf(stderr, "could not open %s!\n", szFileName);
            return 1;
        }
    }

    if(pxdvd)
        goto gottapes;

    pstmT0 = new CDiskFile(argv[1], GENERIC_READ);
    if(!pstmT0->FIsOpen()) {
noopen:
        fprintf(stderr, "could not open %s!\n", argv[1]);
        return 1;
    }
    if(fRawImages)
        pstmL0 = new CBasedFile(pstmT0);
    else if(!tapSrc0.FMount(*pstmT0, FALSE))
        goto noopen;
    
    if(argc >= 3) {
        ++argv;
        pstmT1 = new CDiskFile(argv[1], GENERIC_READ);
        if(!pstmT1->FIsOpen())
            goto noopen;
        if(fRawImages)
            pstmL1 = new CBasedFile(pstmT1);
        else if(!tapSrc1.FMount(*pstmT1, FALSE))
            goto noopen;
    }

    /* Find ximage0.dat and ximage1.dat */
    if(!fRawImages) {
        for(i = 0; i < 2; ++i) {
            ptap = i ? &tapSrc1 : &tapSrc0;
            while(ptap->FReadFile(szName, NULL, NULL)) {
                if(0 == _stricmp(szName, "XIMAGE0.DAT"))
                    pstmL0 = new CBasedFile(ptap);
                else if(0 == _stricmp(szName, "XIMAGE1.DAT"))
                    pstmL1 = new CBasedFile(ptap);
                else if(i == 0 && 0 == _stricmp(szName, "DX2ML.XML"))
                    FGetXMLData(ptap);
            }
        }
        if(!pstmL0) {
            fprintf(stderr, "never saw XIMAGE0.dat\n");
            return 1;
        }
        if(!pstmL1) {
            fprintf(stderr, "never saw XIMAGE1.dat\n");
            return 1;
        }
    }

    if(!g_pdxml) {
        if(fResign) {
            fprintf(stderr, "error: never saw valid DX2ML.XML\n");
            return 1;
        }
        fprintf(stderr, "warning: no DX2ML.XML, can't validate placeholders\n");
    }

gottapes:
    
    /* Grab the GDF volume descriptor */
    pstmCur = PstmSeekTo(GDF_VOLUME_DESCRIPTOR_SECTOR);
    if(!pstmCur || pstmCur->CbRead(2048, (PBYTE)&gvd) != 2048) {
badformat:
        fprintf(stderr, "bad image format or read error\n");
        return 1;
    }
    if(memcmp(gvd.HeadSignature, GDF_VOLUME_DESCRIPTOR_SIGNATURE,
            GDF_VOLUME_DESCRIPTOR_SIGNATURE_LENGTH) ||
            memcmp(gvd.TailSignature, GDF_VOLUME_DESCRIPTOR_SIGNATURE,
            GDF_VOLUME_DESCRIPTOR_SIGNATURE_LENGTH))
        goto badformat;

    /* Copy the original volume descriptor to the bin file */
    iblkBinCur = 0;
    if(pstmBinOut) {
        if(pstmBinOut->CbWrite(sizeof gvd, (PBYTE)&gvd) != sizeof gvd) {
binwriteerr:
            fprintf(stderr, "write error: %s\n", pstmBinOut->SzName());
            return 1;
        }
        ++iblkBinCur;
    }

    /* Read the root directory and push it on the dir stack */
    if(!pstmOut)
        printf("Root dir at %d\n", gvd.RootDirectoryFirstSector);
    pgdirRoot = new GDIR(PstmSeekTo(gvd.RootDirectoryFirstSector),
        gvd.RootDirectoryFileSize);
    if(!pgdirRoot->FGotData())
        goto badformat;
    gdq.PushDir(pgdirRoot);
    /* We need to construct a fake dir entry for the root dir */
    memset(&gdeRoot, 0, sizeof gdeRoot);
    gdeRoot.FirstSector = gvd.RootDirectoryFirstSector;
    gdeRoot.FileSize = gvd.RootDirectoryFileSize;
    PushFobj(new FOBJ("\\.", gvd.RootDirectoryFirstSector, &gdeRoot, pgdirRoot,
        iblkBinCur));
    if(pstmBinOut) {
        if(!pgdirRoot->FWriteToStm(pstmBinOut))
            goto binwriteerr;
        iblkBinCur += (gvd.RootDirectoryFileSize + 2047) / 2048;
    }

    /* Step 1: process all directories and build up a file list */
    while(pgdir = gdq.PopDir()) {
        pgdir->ResetWalk();
        while(pgde = pgdir->PgdeGetNextEntry()) {
            sprintf((LPSTR)rgb, "%s\\%.*s", pgdir->SzName(),
                pgde->FileNameLength, pgde->FileName);
            if(!pstmOut)
                printf("%s at %d\n", rgb, pgde->FirstSector);
            if(pgde->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                pgdirT = new GDIR(PstmSeekTo(pgde->FirstSector),
                    pgde->FileSize, (LPSTR)rgb);
                if(!pgdirT->FGotData())
                    goto badformat;
                if(pstmBinOut) {
                    if(!pgdirT->FWriteToStm(pstmBinOut))
                        goto binwriteerr;
                    cblk = (pgde->FileSize + 2047) / 2048;
                    iblkBinCur += cblk;
                }
                gdq.PushDir(pgdirT);
            } else
                pgdirT = NULL;
            /* If this wasn't a directory, we'll end up pushing random garbage
             * for the bin blk number, but that's OK -- we'll ignore it
             * later */
            PushFobj(new FOBJ((LPSTR)rgb, pgde->FirstSector, pgde, pgdirT,
                iblkBinCur - cblk));
        }
    }

    /* TODO: make sure \default.xbe exists, is not a directory, and
     * shows up on the XBE list with the correct block count */

    if(!pstmOut)
        /* No file output, so we're done */
        return 0;

    /* Step 2: sort the file list */
    SortFobjList(&g_pfobjFirst);
    if(fResign && !FMarkXbeFiles())
        return 1;

    /* Step 3: compact the file list */
    iblkCur = GDF_VOLUME_DESCRIPTOR_SECTOR + 1;
    pstmCurOut = pstmOut;
    for(pfobj = g_pfobjFirst; pfobj; pfobj = pfobj->pfobjNext) {
        pfobj->cblk = (pfobj->pgde->FileSize + 2047) / 2048;
        if(iblkCur + pfobj->cblk > 0x200000) {
            /* We've overflowed our 4GB limit.  If not building an emulation
             * image, we error out.  If building an emulation image, we
             * roll over into the bin file and warn that the ISO is bad */
            if(!pfst) {
toobig:
                fprintf(stderr, "error: output image is too big\n");
                return 1;
            } else if(pstmOut == pstmBinOut)
                goto toobig;
            else {
                fprintf(stderr, "warning: output image is too big to be made into an ISO image\n");
                iblkCur = iblkBinCur;
                pstmCurOut = pstmBinOut;
            }
        }
        pfobj->dwBlkNew = iblkCur;
        iblkCur += pfobj->cblk;
        pfobj->pstmNew = pstmCurOut;
        pfobj->pgde->FirstSector = pfobj->dwBlkNew;
    }

    /* Step 4: write out the files */
    memset(rgb, 0, 2048);
    pstmCurOut = pstmOut;
    for(iblkCur = 0; iblkCur < GDF_VOLUME_DESCRIPTOR_SECTOR; ++iblkCur) {
        if(pstmCurOut->CbWrite(2048, rgb) != 2048) {
writeerr:
            fprintf(stderr, "write error: %s\n", pstmCurOut->SzName());
            delete pstmCurOut;
            return 1;
        }
    }

    gvd.RootDirectoryFirstSector = gdeRoot.FirstSector;
    if(pstmOut->CbWrite(sizeof gvd, (PBYTE)&gvd) != sizeof gvd)
        goto writeerr;

    for(pfobj = g_pfobjFirst; pfobj; pfobj = pfobj->pfobjNext) {
        if(pfobj->pgdir) {
            if(!pfobj->pgdir->FWriteToStm(pfobj->pstmNew))
                goto writeerr;
        } else {
            cblk = (pfobj->pgde->FileSize + 2047) / 2048;
            pstmCur = PstmSeekTo(pfobj->dwBlkOrig);
            if(!pstmCur)
                goto badformat;
            cblk = pfobj->cblk;
            if(pfobj->fXbe) {
               pstmCurOut = PstmCreateTemp("temp.xbe", GENERIC_WRITE);
               if(!pstmCurOut) {
                   fprintf(stderr, "error: could not create temporary XBE file\n");
                   return 1;
               }
            } else
                pstmCurOut = pfobj->pstmNew;
            while(cblk) {
                cblkCur = 4;
                if(cblk < cblkCur)
                    cblkCur = cblk;
                if(pstmCur->CbRead(cblkCur * 2048, rgb) != cblkCur * 2048)
                    goto badformat;
                if(pstmCurOut->CbWrite(cblkCur * 2048, rgb) != cblkCur * 2048)
                    goto writeerr;
                cblk -= cblkCur;
            }
            if(pfobj->fXbe) {
                if(!pstmCurOut->FReopen(GENERIC_READ))
                    goto writeerr;
                if(!FSignXBE(pfobj, pstmCurOut, pfobj->pstmNew)) {
                    delete pstmCurOut;
                    return 1;
                }
                delete pstmCurOut;
            }
        }
    }

    /* Force the output files to close so their timestamps are updated */
    pstmOut->FReopen(GENERIC_READ);
    if(pstmBinOut)
        pstmBinOut->FReopen(GENERIC_READ);

    if (pfst)
    {
        if(!pfst->FReopen(GENERIC_WRITE) || !pfst->FFlush(pstmBinOut,
            pstmOut))
        {
            fprintf(stderr, "error: could not write %s\n", pfst->SzName());
            delete pfst;
            return 1;
        }
        delete pfst;
    }

    delete pstmOut;
    if(pstmBinOut)
        delete pstmBinOut;

    return 0;
}

// sTableEntry  -- Contains information about a particular entry in the FST file
struct sTableEntry
{
    // m_dwStart    -- LSN that the entry starts on
    DWORD m_dwStart;
    
    // m_dwStop     -- LSN that the entry stops on
    DWORD m_dwStop;

    // m_iDir       -- String table index for the name of the directory for the entry 
    DWORD m_iDir;

    // m_iName      -- String table index for the name of the file for the entry
    DWORD m_iName;

    // m_dwOffset   -- Offset into the entry at which to begin reading data
    DWORD m_dwOffset;
};

// Taken from AMC's file "MediaBase.h"
struct TFileHeader
{
    char m_szFileType[32]; // FST, Error Map, etc
    char m_szMediaType[32]; // DVD, CD, etc
};

struct TMediaHeader
{
    DWORD uNumSectorsLayer0;
    DWORD uNumSectorsLayer1;
    BYTE m_ImpUseArea[120];       // total = 128 bytes
};
static char *g_szVirtualDvdHeaderString = "AMC Virtual Media";

BOOL CFST::FFlush(CDiskFile *pdfBinFile, CDiskFile *pdfIsoFile)
{
    // Populate the FST file header info
    DWORD dwDummy;
    WORD wDummy;
    int cFiles;
    FOBJ *pfobj;
    char szDrive[MAX_PATH + 1];
    char szDir[MAX_PATH + 1];
    char szFile[MAX_PATH + 1];
    char szExt[MAX_PATH + 1];
    char szT[MAX_PATH + 1];

    // Write out the AMC Header information
    TFileHeader tfh;
    memset(&tfh, 0, sizeof(TFileHeader));
    strcpy(tfh.m_szFileType, g_szVirtualDvdHeaderString);
    strcpy(tfh.m_szMediaType, "Xbox DVD-ROM");
    if (CbWrite(sizeof(TFileHeader), (BYTE*)&tfh) != sizeof(TFileHeader))
        return false;

    TMediaHeader tmh;
    memset(&tmh, 0, sizeof(TMediaHeader));
    tmh.uNumSectorsLayer0 = 1715632;
    tmh.uNumSectorsLayer1 = 1715632;
    if (CbWrite(sizeof(TMediaHeader), (BYTE*)&tmh) != sizeof(TMediaHeader))
        return false;

    // Write out the byte order.
    wDummy = 0xABCD;
    if (CbWrite(2, (BYTE*)&wDummy) != 2)
        return false;

    // Write out a timestamp.
    dwDummy = time(NULL);
    if (CbWrite(4, (BYTE*)&dwDummy) != 4)
        return false;
    
    // Create the entry table
    cFiles = 1;
    for(pfobj = g_pfobjFirst; pfobj; pfobj = pfobj->pfobjNext)
        ++cFiles;
    sTableEntry *rgte = new sTableEntry[cFiles];
    if (!rgte)
        return false;

    // Create the string table that contains the full path names of all the files
    char *rgStrings = new char[3*MAX_PATH];
    if (!rgStrings)
        return false;

    int iEntry = 0;

    // Add 3 elements to string table: (0) root folder, (1) bin name, (2) iso name
    _splitpath(pdfBinFile->SzName(), szDrive, szDir, szFile, szExt);
    if(szDir[0] != '\\') {
        /* Relative path name, so find the current dir */
        GetCurrentDirectory(MAX_PATH, szDrive);
        if(szDir[0]) {
            strcpy(szT, szDir);
            if(szDrive[3])
                sprintf(szDir, "%s\\%s", szDrive + 2, szT);
            else
                sprintf(szDir, "\\%s", szT);
        } else
            sprintf(szDir, "%s\\", szDrive + 2);
    }
    strcpy(rgStrings, szDir);
    int iStringLoc = strlen(szDir);
    rgStrings[iStringLoc - 1] = 0;
    sprintf(rgStrings + iStringLoc, "%s%s", szFile, szExt);
    int iBin = iStringLoc;
    iStringLoc += strlen(rgStrings + iStringLoc) + 1;
    _splitpath(pdfIsoFile->SzName(), NULL, NULL, szFile, szExt);
    sprintf(rgStrings + iStringLoc, "%s%s", szFile, szExt);
    int iIso = iStringLoc;
    iStringLoc += strlen(rgStrings + iStringLoc) + 1;

    // Populate the entry table with our file information
    sTableEntry *pteCur = rgte;
    sTableEntry teVol;
    teVol.m_dwStart  = GDF_VOLUME_DESCRIPTOR_SECTOR;
    teVol.m_dwStop   = GDF_VOLUME_DESCRIPTOR_SECTOR;
    teVol.m_dwOffset = 0;
    teVol.m_iDir     = 0;
    teVol.m_iName    = iBin;
    for(pfobj = g_pfobjFirst; pfobj || teVol.m_dwStart != -1; ++pteCur) {
        if(!pfobj || pfobj->dwBlkOrig > teVol.m_dwStart) {
			memcpy(pteCur, &teVol, sizeof teVol);
            teVol.m_dwStart = -1;
		} else {
			pteCur->m_dwStart  = pfobj->dwBlkOrig;
			pteCur->m_dwStop   = pfobj->dwBlkOrig + pfobj->cblk - 1;
			pteCur->m_iDir     = 0;
			if(pfobj->pgdir) {
				/* Directories always come from the bin file */
				pteCur->m_dwOffset = pfobj->dwBinBlk * 2048;
				pteCur->m_iName = iBin;
			} else {
				pteCur->m_dwOffset = pfobj->dwBlkNew * 2048;
				pteCur->m_iName    = (pfobj->pstmNew == pdfBinFile) ? iBin : iIso;
			}
			pfobj = pfobj->pfobjNext;
		}
    }

    // Write out the total number of objects in the project.
    if (CbWrite(4, (BYTE*)&cFiles) != 4)
        return false;

    // Write out the full size of the string table.
    if (CbWrite(4, (BYTE*)&iStringLoc) != 4)
        return false;

    // Write out the entry table
    if (CbWrite(sizeof(sTableEntry)*cFiles, (BYTE*)rgte) !=
                sizeof(sTableEntry)*cFiles)
        return false;

    // Write out the string table
    if (CbWrite(iStringLoc, (BYTE*)rgStrings) != (DWORD)iStringLoc)
        return false;
 
    delete rgte;

    return true;
}
