#include "precomp.h"

CCheckSum xsumBlk, xsumRed, xsumGrn, xsumMediaIn, xsumVideo;

void MsgOut(LPCSTR szFormat, ...)
{
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

BOOL FChecksumXBE(int ilyr, CXBEFile *pxbe, DWORD cblk, CTapeDevice *ptap)
{
    /* All we want to do is checksum this xbe file */
    if(!pxbe->FCalcChecksums(&xsumBlk, &xsumRed, &xsumGrn)) {
        fprintf(stderr, "error: could not process XBE file on tape %d\n", ilyr);
        return FALSE;
    }
    return TRUE;
}

BOOL FDoATape(CTapeDevice *ptap, int ilyr)
{
    CDX2MLFile xml;
    DXML *pdxml;
    CDiskFile *pstm;
    int idxr, idxi;
    DXL *pdxl;
    DWORD dwDataBlkSize;
    DWORD dwDataPerPhy;
    BOOL fRet = FALSE;
    char szName[256];
    BOOL fSawImage;
    BOOL fSawXImage;
    BYTE rgb[32768];
    int cb;

    /* Start with the XML */
    pstm = PstmCreateTemp("temp.xml", GENERIC_WRITE);
    if(!pstm) {
        fprintf(stderr, "error: could not create temp file\n");
        return FALSE;
    }
    if(!ptap->FReadFile(szName, &dwDataBlkSize, &dwDataPerPhy) ||
        0 != strcmp(szName, "DX2ML.XML") || !ptap->FCopyToStm(pstm))
    {
badxml:
        fprintf(stderr, "error: could not read read XML file from tape %d\n", ilyr);
        goto fatal;
    }
    ptap->FCloseFile();
    if(!pstm->FReopen(GENERIC_READ))
        goto badxml;
    if(!xml.FReadFile(pstm->SzName(), FALSE))
        goto badxml;
    /* TODO: validate the XML so we know we'll be able to use it */
    delete pstm;
    pstm = NULL;

    /* Now loop through the files and process the ximage when we get there */
    pdxml = xml.PdxmlGet();
    pdxl = &pdxml->rgdxl[ilyr];
    fSawImage = fSawXImage = FALSE;
    for(;ptap->FReadFile(szName, &dwDataBlkSize, &dwDataPerPhy); ptap->FCloseFile()) {
        for(idxi = 0; idxi < idxiMax; ++idxi) {
            if(pdxml->rgdxi[idxi].szNameOnTape &&
                    0 == strcmp(szName, pdxml->rgdxi[idxi].szNameOnTape))
                break;
        }
        if(idxi < idxiMax) {
            for(idxr = 0; idxr < idxrMax; ++idxr) {
                if(pdxl->rgdxrAreas[idxr].idxiImage == idxi)
                    break;
            }
            if(idxr == idxrXData) {
                fSawXImage = TRUE;
                if(!FProcessXImage(ilyr, pdxl, idxr, ptap, NULL, FChecksumXBE))
                    goto fatal;
                continue;
            } else if(idxr == idxrData) {
                fSawImage = TRUE;
                while(cb = ptap->CbRead(sizeof rgb, rgb))
                    xsumVideo.SumBytes(rgb, cb);
            }
        }
        /* This is an uninteresting file, but we need to read it anyway to make
         * sure we get the checksum right */
        while(ptap->CbRead(sizeof rgb, rgb));
    }

    if(!fSawXImage)
        fprintf(stderr, "error: never saw ximage.dat on tape %d\n", ilyr);
    else if(!fSawImage)
        fprintf(stderr, "error: never saw image.dat on tape %d\n", ilyr);
    else
        fRet = TRUE;
fatal:
    if(pstm)
        delete pstm;
    return fRet;
}

BOOL FDoBlackTape(CTapeDevice *ptap)
{
    CDX2MLFile xml;
    DXML *pdxml;
    CDiskFile *pstm;
    int ilyr, idxr, idxi;
    DXL *pdxl;
    DXR *pdxr;
    DWORD dwDataBlkSize;
    DWORD dwDataPerPhy;
    BOOL fRet = FALSE;
    char szName[256];
    BOOL fSawImage;
    BYTE rgb[32768];

    /* Start with the XML */
    pstm = PstmCreateTemp("temp.xml", GENERIC_WRITE);
    if(!pstm) {
        fprintf(stderr, "error: could not create temp file\n");
        return FALSE;
    }
    if(!ptap->FReadFile(szName, &dwDataBlkSize, &dwDataPerPhy) ||
        0 != strcmp(szName, "DX2ML.XML") || !ptap->FCopyToStm(pstm))
    {
badxml:
        fprintf(stderr, "error: could not read read XML file from tape\n");
        goto fatal;
    }
    ptap->FCloseFile();
    if(!pstm->FReopen(GENERIC_READ))
        goto badxml;
    if(!xml.FReadFile(pstm->SzName(), FALSE))
        goto badxml;
    /* TODO: validate the XML so we know we'll be able to use it */
    delete pstm;
    pstm = NULL;

    /* Now loop through the files and process the ximages when we get there */
    pdxml = xml.PdxmlGet();
    fSawImage = FALSE;
    for(;ptap->FReadFile(szName, &dwDataBlkSize, &dwDataPerPhy); ptap->FCloseFile()) {
        for(idxi = 0; idxi < idxiMax; ++idxi) {
            if(pdxml->rgdxi[idxi].szNameOnTape &&
                    0 == strcmp(szName, pdxml->rgdxi[idxi].szNameOnTape))
                break;
        }
        pdxr = NULL;
        for(ilyr = 0; !pdxr && ilyr < 2; ++ilyr) {
            pdxl = &pdxml->rgdxl[ilyr];
            for(idxr = 0; idxr < idxrMax; ++idxr) {
                if(pdxl->rgdxrAreas[idxr].idxiImage == idxi)
                    break;
            }
            if(idxr == idxrXData)
                pdxr = &pdxl->rgdxrAreas[idxr];
        }

        /* At this point, ilyr is one past the layer we actually matched */

        if(pdxr) {
            if(fSawImage > (1 << ilyr)) {
                fprintf(stderr, "error: XIMAGE0.DAT must come before XIMAGE1.DAT\n");
                goto fatal;
            }
            fSawImage |= 1 << ilyr;
            if(!FProcessXImage(ilyr - 1, pdxl, idxrXData, ptap, NULL,
                    FChecksumXBE))
                goto fatal;
        } else {
            /* This is an uninteresting file, but we need to read it anyway to make
             * sure we get the checksum right */
            while(ptap->CbRead(sizeof rgb, rgb));
        }
    }

    if(!(fSawImage & 2))
        fprintf(stderr, "error: never saw XIMAGE0.dat\n", ilyr);
    if(!(fSawImage & 4))
        fprintf(stderr, "error: never saw XIMAGE1.dat\n", ilyr);
    if(fSawImage == 6)
        fRet = TRUE;
fatal:
    if(pstm)
        delete pstm;
    return fRet;
}

int __cdecl main(int cArg, char **rgszArg)
{
    CTapeDevice tap0, tap1;
    BOOL fVerbose = FALSE;
    DWORD cb, cbRes;
    CDiskFile *pstmXdr;
    HRSRC hres;
    BOOL fSuccess = FALSE;
    PBYTE pb;

    while(++rgszArg, --cArg) {
        if(_stricmp(*rgszArg, "-verbose") == 0)
            ++fVerbose;
        else
            break;
    }

    if(cArg < 1 || cArg > 2) {
        fprintf(stderr, "usage: xboxcrctapes [-verbose] layer0-tape [layer1-tape]\n");
        return 1;
    }

    if(!tap0.FMountAsTapeOrDisk(rgszArg[0], FALSE)) {
        fprintf(stderr, "error: failed to mount %s\n", rgszArg[1]);
        return 1;
    }

    if(cArg == 2 && !tap1.FMountAsTapeOrDisk(rgszArg[1], FALSE)) {
        fprintf(stderr, "error: failed to mount %s\n", rgszArg[1]);
        return 1;
    }

    /* Get the XDR file handy in case we need it */
    pstmXdr = PstmCreateTemp("dx2ml.xdr", GENERIC_WRITE);
    if(!pstmXdr) {
noxdr:
        fprintf(stderr, "error: unable to create temporary XDR file\n");
        return 1;
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

    if(!tap1.FMounted()) {
        /* Black tape checksum */
        fSuccess = FDoBlackTape(&tap0);
    } else
        fSuccess = FDoATape(&tap0, 0) && FDoATape(&tap1, 1);

    delete pstmXdr;
    if(!fSuccess)
        return 1;

    /* Input media and final media checks should agree */
    if(xsumGrn.DwFinalSum() != xsumMediaIn.DwFinalSum()) {
        fprintf(stderr, "error: internal checksum error\n");
        return 1;
    }

    if(tap1.FMounted()) {
        printf("Tape 0 checksum is 0x%08X\n", tap0.DwCheckSum());
        printf("Tape 1 checksum is 0x%08X\n", tap1.DwCheckSum());
        printf("Final media checksum is 0x%08X\n", xsumGrn.DwFinalSum());
        if(fVerbose) {
            printf("Video checksum is 0x%08X\n", xsumVideo.DwFinalSum());
            printf("Black media checksum is 0x%08X\n", xsumBlk.DwFinalSum());
            printf("Red media checksum is 0x%08X\n", xsumRed.DwFinalSum());
        }
    } else {
        printf("Submission checksum is 0x%08X", tap0.DwCheckSum());
        printf("Media CRC is 0x%08X", xsumBlk.DwFinalSum());
    }

    return 0;
}
