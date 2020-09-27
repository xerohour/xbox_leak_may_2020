/*
 *
 * redgrn.cpp
 *
 * Red to Green signing tool
 *
 */

#include "precomp.h"

const char *rgszRSMKeys[] = { "Layer0CRC", "Layer1CRC", "XboxMediaCRC" };
LPCSTR szTempPath;
CTapeDevice tapSrc0, tapSrc1;
CTapeDevice tapDst0, tapDst1;
CDX2MLFile xml0, xml1;
CCryptContainer *pccRed, *pccGrn;
CCheckSum xsumBlk, xsumRed, xsumGrn, xsumMediaIn;
CCheckSum xsumDvd;
CIniBag *pbagInput;
BYTE rgbXControl[2054];
BYTE rgbDMIIn[2054];
BYTE rgbDMIOut[2054];
BYTE rgbControl[2048];
const char szCRC[] = "CRC";
const char szDMI[] = "Disk Manufacturing Information";
const char szXBE[] = "XBE Information";
XBEIMAGE_CERTIFICATE certKeys;

void MsgOut(LPCSTR szFormat, ...)
{
    va_list va;
    va_start(va, szFormat);
    vprintf(szFormat, va);
    va_end(va);
}

void DoneBeep(void)
{
    int i;

    _cputs("Press any key . . .");
    for(i = 0; !_kbhit(); ++i) {
        if(!(i & 0xF)) {
            _putch(7);
            MessageBeep(-1);
        } else
            Sleep(100);
    }
    _getch();
    _putch(13);
    _putch(10);
}

BOOL FMountTape(CTapeDevice *ptap, LPCSTR szName, BOOL fTOD, BOOL fWrite)
{
    HANDLE h;

    if(!szName)
        return FALSE;
    if(0 == strncmp(szName, "\\\\?", 3) || 0 == strncmp(szName, "\\\\.", 3))
        /* Tape device */
        return ptap->FMount(szName, fWrite);
    if(fWrite)
        h = CreateFile(szName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0,
            NULL);
    else
        h = CreateFile(szName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0,
            NULL);
    if(h == INVALID_HANDLE_VALUE)
        return FALSE;
    return ptap->FMount(h, fWrite);
}

CDiskFile *PstmCreateTemp(LPCSTR szName, DWORD dwFlags)
{
    char szFullName[MAX_PATH + 1];
    CDiskFile *pstm;

    if(!szTempPath)
        return NULL;

    sprintf(szFullName, "%s\\%s", szTempPath, szName);
    pstm = new CDiskFile(szFullName, dwFlags);
    if(!pstm->FIsOpen()) {
        delete pstm;
        pstm = NULL;
    } else
        pstm->SetDeleteOnClose(TRUE);
    return pstm;
}

BOOL FCopyXMLFiles(LPCSTR szMediaRef)
{
    CDiskFile *pstmXML0 = NULL, *pstmXML1 = NULL;
    CDiskFile *pstmXDR = NULL;
    DXML *pdxml;
    HRSRC hres;
    LPBYTE pb;
    DWORD cbRes;
    DWORD dwDataBlkSize;
    DWORD dwDataPerPhy;
    BOOL fRet = FALSE;
    char szFileName[32];
    char szLayerRef[256];

    /* Get disk space for the files */
    pstmXDR = PstmCreateTemp("dx2ml.xdr", GENERIC_WRITE);
    pstmXML0 = PstmCreateTemp("dx2ml0.xml", GENERIC_WRITE);
    pstmXML1 = PstmCreateTemp("dx2ml1.xml", GENERIC_WRITE);
    if(!pstmXDR || !pstmXML0|| !pstmXML1) {
notemp:
        fprintf(stderr, "error: cannot create temporary XML files\n");
        goto fatal;
    }

    /* Extract the XDR file */
    MsgOut("Preparing XDR file\n");
    hres = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(1), "FILE");
    if(!hres)
        goto notemp;
    pb = (PBYTE)LockResource(LoadResource(GetModuleHandle(NULL), hres));
    if(!pb)
        goto notemp;
    cbRes = SizeofResource(GetModuleHandle(NULL), hres);
    if(pstmXDR->CbWrite(cbRes, pb) != cbRes)
        goto notemp;
    if(!pstmXDR->FReopen(GENERIC_READ))
        goto notemp;

    MsgOut("Reading DX2ML file from tape 0\n");
    if(!tapSrc0.FReadFile(szFileName, &dwDataBlkSize, &dwDataPerPhy) ||
        strcmp(szFileName, "DX2ML.XML"))
    {
        fprintf(stderr, "error: source tape 0 missing XML file\n");
        goto fatal;
    }
    if(!tapSrc0.FCopyToStm(pstmXML0) || !tapSrc0.FCloseFile()) {
        fprintf(stderr, "error: could not copy XML file from source tape 0\n");
        goto fatal;
    }
    MsgOut("Reading DX2ML file from tape 1\n");
    if(!tapSrc1.FReadFile(szFileName, &dwDataBlkSize, &dwDataPerPhy) ||
        strcmp(szFileName, "DX2ML.XML"))
    {
        fprintf(stderr, "error: source tape 0 missing XML file\n");
        goto fatal;
    }
    if(!tapSrc1.FCopyToStm(pstmXML1) || !tapSrc1.FCloseFile()) {
        fprintf(stderr, "error: could not copy XML file from source tape 1\n");
        goto fatal;
    }
    if(!pstmXML0->FReopen(GENERIC_READ))
        goto bad0;
    if(!pstmXML1->FReopen(GENERIC_READ))
        goto bad1;

    MsgOut("Validating XML files\n");
    /* Process the two DX2ML files */
    if(!xml0.FReadFile(pstmXML0->SzName(), TRUE)) {
bad0:
        fprintf(stderr, "error: could not process DX2ML from tape 0\n");
        goto fatal;
    }
    if(!xml1.FReadFile(pstmXML1->SzName(), TRUE)) {
bad1:
        fprintf(stderr, "error: could not process DX2ML from tape 1\n");
        goto fatal;
    }

    /* validate the XML files */
    if(!xml0.FValidateContent() || xml0.PdxmlGet()->iLayer != 0)
        goto bad0;
    if(!xml1.FValidateContent() || xml1.PdxmlGet()->iLayer != 1)
        goto bad1;
    if(!xml0.FValidatePlaceholderLocations(FALSE, TRUE, FALSE)) {
        fprintf(stderr, "error: DX2ML placeholder data is invalid\n");
        goto fatal;
    }

    /* Strip the previous mastering refs */
    xml0.SetMasteringRef(NULL);
    xml1.SetMasteringRef(NULL);

    /* Make sure they match */
    if(xml0 != xml1) {
        fprintf(stderr, "error: XML files on the two tapes do not match\n");
        goto fatal;
    }

    /* Update the mastering refs */
    sprintf(szLayerRef, "%s-L0", szMediaRef);
    xml0.SetMasteringRef(szMediaRef);
    sprintf(szLayerRef, "%s-L1", szMediaRef);
    xml1.SetMasteringRef(szMediaRef);

    /* Write out the two DX2ML files */
    MsgOut("Writing DX2ML file to tape 0\n");
    if(!tapDst0.FWriteFile("dx2ml.xml", 32768, 1) ||
        !xml0.FWriteFile(&tapDst0) || !tapDst0.FCloseFile())
    {
        fprintf(stderr, "error: could not write DX2ML to tape 0\n");
        goto fatal;
    }
    MsgOut("Writing DX2ML file to tape 1\n");
    if(!tapDst1.FWriteFile("dx2ml.xml", 32768, 1) ||
        !xml1.FWriteFile(&tapDst1) || !tapDst1.FCloseFile())
    {
        fprintf(stderr, "error: could not write DX2ML to tape 1\n");
        goto fatal;
    }
    fRet = TRUE;
fatal:
    if(pstmXML0)
        delete pstmXML0;
    if(pstmXML1)
        delete pstmXML1;
    if(pstmXDR)
        delete pstmXDR;
    return fRet;
}

BOOL FProcessXControl(int ilyr, DXML *pxml, CTapeDevice *ptapSrc,
    CTapeDevice *ptapDst)
{
    int i;
    int ib;
    BYTE b;
    BYTE rgb[2054];
    DWORD dw;
    CXControl xct;
    FILETIME ft;

    MsgOut("Processing XCONTROL.DAT on tape %d\n", ilyr);
    /* First make sure that we have 30 blocks of zero */
    for(i = 0; i < 30; ++i) {
        if(ptapSrc->CbRead(sizeof rgb, rgb) != sizeof rgb) {
readerr:
            fprintf(stderr, "error: could not read XControl from tape %d\n", ilyr);
            return FALSE;
        }
        for(b = 0, ib = 0; ib < sizeof rgb; ++ib)
            b |= rgb[ib];
        if(b) {
baddata:
            fprintf(stderr, "error: invalid XControl data on tape %d\n", ilyr);
            return FALSE;
        }
        if(ptapDst->CbWrite(sizeof rgb, rgb) != sizeof rgb) {
writeerr:
            fprintf(stderr, "error: could not write XControl on tape %d\n", ilyr);
            return FALSE;
        }
    }

    /* Now read the XControl frame.  If we're on layer 0, we just read it.
     * If we're on layer 1, we need to make sure it matches what was on
     * layer 0 */
    if(ilyr == 0) {
        if(ptapSrc->CbRead(sizeof rgbXControl, rgbXControl) !=
                sizeof rgbXControl)
            goto readerr;
    } else {
        if(ptapSrc->CbRead(sizeof rgb, rgb) != sizeof rgb)
            goto readerr;
        if(memcmp(rgb, rgbXControl, sizeof rgb))
            goto baddata;
    }

    /* Read the DMI frame */
    if(ptapSrc->CbRead(sizeof rgb, rgb) != sizeof rgb)
        goto readerr;

    /* Make sure the DMI frame matches the expected DMI */
    if(memcmp(rgb, rgbDMIIn, sizeof rgb))
        goto baddata;

    /* There should be no more data in the file */
    if(ptapSrc->CbRead(sizeof dw, (PBYTE)&dw) != 0)
        goto baddata;

    /* Stamp the XControl and write it */
    memcpy(rgb, rgbXControl, sizeof rgb);
    if(!xct.FInit(rgb, sizeof rgb) || !xct.FValidate(pxml, pccRed))
        goto baddata;
    if(!xct.FSign(pccGrn))
        goto writeerr;
    if(!xct.FWriteToStm(ptapDst, sizeof rgb))
        goto writeerr;

    /* Write out the DMI */
    if(ptapDst->CbWrite(sizeof rgbDMIOut, rgbDMIOut) != sizeof rgbDMIOut)
        goto writeerr;

    return TRUE;
}

BOOL FProcessControl(int ilyr, DXML *pxml, CTapeDevice *ptapSrc,
    CTapeDevice *ptapDst)
{
    int i;
    int ib;
    BYTE b;
    BYTE rgb[2048];
    CECMAControl ect;
    DWORD dw;

    MsgOut("Processing CONTROL.DAT on tape %d\n", ilyr);
    /* Now read the Control frame.  If we're on layer 0, we just read it.
     * If we're on layer 1, we need to make sure it matches what was on
     * layer 0 */
    if(ilyr == 0) {
        if(ptapSrc->CbRead(sizeof rgbControl, rgbControl) !=
            sizeof rgbControl)
        {
readerr:
            fprintf(stderr, "error: could not read Control from tape %d\n", ilyr);
            return FALSE;
        }
    } else {
        if(ptapSrc->CbRead(sizeof rgb, rgb) != sizeof rgb)
            goto readerr;
        if(memcmp(rgb, rgbControl, sizeof rgb)) {
baddata:
            fprintf(stderr, "error: Invalid Control data on tape %d\n", ilyr);
            return FALSE;
        }
    }

    /* Validate the control data */
    if(!ect.FInit(rgbControl) || !ect.FValidate(pxml))
        goto baddata;

    /* Write out the control data we've got */
    if(ptapDst->CbWrite(sizeof rgbControl, rgbControl) != sizeof rgbControl) {
writeerr:
        fprintf(stderr, "error: could not write Control on tape %d\n", ilyr);
        return FALSE;
    }

    /* Now read the DMI and validate it against what we expect */
    if(ptapSrc->CbRead(sizeof rgb, rgb) != sizeof rgb)
        goto readerr;
    if(memcmp(rgb, rgbDMIIn + 6, sizeof rgb))
        goto baddata;

    /* Write out the DMI */
    if(ptapDst->CbWrite(sizeof rgb, rgbDMIOut + 6) != sizeof rgb)
        goto writeerr;

    /* Now make sure that we have 14 blocks of zero */
    for(i = 0; i < 14; ++i) {
        if(ptapSrc->CbRead(sizeof rgb, rgb) != sizeof rgb)
            goto readerr;
        for(b = 0, ib = 0; ib < sizeof rgb; ++ib)
            b |= rgb[ib];
        if(b)
            goto baddata;
        if(ptapDst->CbWrite(sizeof rgb, rgb) != sizeof rgb)
            goto writeerr;
    }

    /* There should be no more data in the file */
    if(ptapSrc->CbRead(sizeof dw, (PBYTE)&dw) != 0)
        goto baddata;

    return TRUE;
}

BOOL FGreensignXBE(int ilyr, CXBEFile *pxbe, DWORD cblk, CTapeDevice *ptapDst)
{
    XBEIMAGE_CERTIFICATE certNew;
    const XBEIMAGE_CERTIFICATE *pcert;

    MsgOut("Processing XBE file\n");
    pxbe->UseCryptContainer(pccRed);
    if(!pxbe->FVerifySignature()) {
badxbe:
        fprintf(stderr, "error: found an invalid XBE file on tape %d\n", ilyr);
        return FALSE;
    }
    
    /* Make sure the certificate matches the data we expect */
    pcert = pxbe->GetCert();
    if(pcert->TitleID != certKeys.TitleID || pcert->Version != certKeys.Version)
        goto badxbe;
    
    /* Copy the certificate keys into the certificate */
    memcpy(&certNew, pcert, sizeof certNew);
    memcpy(certNew.SignatureKey, certKeys.SignatureKey,
        sizeof certKeys.SignatureKey);
    memcpy(certNew.LANKey, certKeys.LANKey, sizeof certKeys.LANKey);
    if(!pxbe->FInjectNewCertificate(&certNew)) {
        fprintf(stderr, "error: could not inject XBE certificate keys on tape %d\n",
            ilyr);
        return FALSE;
    }

    /* Sign and save */
    pxbe->UseCryptContainer(pccGrn);
    if(!pxbe->FSignFile()) {
        fprintf(stderr, "error: unable to sign XBE file\n");
        return FALSE;
    }
    if(!pxbe->FWriteToStm(ptapDst, cblk * 2048)) {
        fprintf(stderr, "error: writing destination tape %d\n", ilyr);
        return FALSE;
    }
    if(!pxbe->FCalcChecksums(&xsumBlk, &xsumRed, &xsumGrn)) {
        fprintf(stderr, "error: reading temporary XBE file\n");
        return FALSE;
    }
    return TRUE;
}

struct {
    int ilyr;
    int idxi;
    int idxr;
} rgfdFileOrder[] = {
    { 0, idxiControl },
    { 0, idxiXControl },
    { 0, -1, idxrData },
    { 0, -1, idxrXData },
    { 1, idxiControl },
    { 1, idxiXControl },
    { 1, -1, idxrXData },
    { 1, -1, idxrData },
    { -1 }
}, *pfd = rgfdFileOrder;

BOOL FGreensignTape(int ilyr, CTapeDevice *ptapSrc, CTapeDevice *ptapDst)
{
    DWORD dwBlkSize;
    DWORD dwDataPerPhy;
    char szFilename[32];
    int idxi, idxr;
    DXML *pdxml = (ilyr ? &xml0 : &xml1)->PdxmlGet();
    DXI *pdxi;
    DXR *pdxr;

    MsgOut("Processing tape %d\n", ilyr);
    /* We'll read only as many files as we expect from this tape; we'll
     * silently discard the rest */
    while(pfd->ilyr == ilyr && ptapSrc->FReadFile(szFilename, &dwBlkSize,
        &dwDataPerPhy))
    {
        /* Find out which file this is */
        for(idxi = 0; idxi < idxiMax; ++idxi) {
            if(0 == strcmp(pdxml->rgdxi[idxi].szNameOnTape, szFilename))
                break;
        }
        if(idxi == idxiMax) {
unkfile:
            /* Unrecognized file, fatal error */
            fprintf(stderr, "error: unknown file %s on tape %d\n", szFilename, ilyr);
            return FALSE;
        }
        pdxi = &pdxml->rgdxi[idxi];

        /* Make sure this file is of the right format */
        if(dwDataPerPhy != 16 || dwBlkSize != (pdxi->f2054 ? 2054U : 2048U)) {
            fprintf(stderr, "error: file %s on tape %d is of the wrong format\n",
                szFilename, ilyr);
            return FALSE;
        }

        /* Figure out which disc region this file corresponds to */
        for(idxr = 0; idxr < idxrMax; ++idxr) {
            if(pdxml->rgdxl[ilyr].rgdxrAreas[idxr].idxiImage == idxi)
                break;
        }

        /* Now what we know what file we're looking at, make sure it comes in
         * the sequence we expect */
        if(ilyr != pfd->ilyr) {
badorder:
            fprintf(stderr, "error: file %s on tape %d comes out of order\n");
            return FALSE;
        }
        if(pfd->idxi >= 0) {
            if(idxi != pfd->idxi)
                goto badorder;
        } else if(idxr != pfd->idxr)
            goto badorder;
        ++pfd;

        /* Set up the destination tape for writing */
        if(!ptapDst->FWriteFile(szFilename, dwBlkSize, dwDataPerPhy)) {
writeerr:
            fprintf(stderr, "error: writing destination tape %d\n", ilyr);
            return FALSE;
        }

        /* Process this file as appropriate */
        switch(idxi) {
        case idxiXControl:
            if(idxr != (ilyr ? idxrLead : idxrMax))
                goto unkfile;
            if(!FProcessXControl(ilyr, pdxml, ptapSrc, ptapDst))
                return FALSE;
            break;
        case idxiControl:
            if(idxr != (ilyr ? idxrMax : idxrLead))
                goto unkfile;
            if(!FProcessControl(ilyr, pdxml, ptapSrc, ptapDst))
                return FALSE;
            break;
        default:
            if(idxr == idxrXData) {
                /* This is an ximage file, so we need to process it */
                if(!FProcessXImage(ilyr, &pdxml->rgdxl[ilyr], idxr, ptapSrc,
                        ptapDst, FGreensignXBE))
                    return FALSE;
            } else if(idxr == idxrData) {
                /* Just copy the file */
                MsgOut("Coping DVD video\n");
                if(!ptapSrc->FCopyToStm(ptapDst, &xsumDvd)) {
                    fprintf(stderr, "error: could not copy file %s on tape %d\n",
                        szFilename, ilyr);
                    return FALSE;
                }
            } else
                goto unkfile;
            break;
        }
        if(!ptapSrc->FCloseFile()) {
            fprintf(stderr, "error: read error on tape %d\n", ilyr);
            return FALSE;
        }
        if(!ptapDst->FCloseFile())
            goto writeerr;
    }

    if(pfd->ilyr == ilyr) {
        fprintf(stderr, "error: files missing from tape %d\n", ilyr);
        return FALSE;
    }
    return TRUE;
}

BOOL FGetAsDword(CIniBag *pbag, LPCSTR szSection, LPCSTR szKey, PDWORD pdw)
{
    CVarBag *pbvp = pbag->FindProp(szSection);
    if(!pbvp)
        return FALSE;
    return pbvp->FFindDwProp(szKey, pdw);
}

#if 0
void AddAsDword(CStringBag *pbag, LPCSTR szKey, DWORD dw)
{
    char sz[16];
    sprintf(sz, "0x%08X", dw);
    pbag->FAddProp(szKey, sz);
}
#endif

int __cdecl main(int cArg, char **rgszArg)
{
    LPCSTR szInputName = "xbvalidate.ini";
    LPCSTR szOutputName = "output.ini";
    BOOL fBeep = FALSE;
    BOOL fTOD = FALSE;
    CIniFile cifInput;
    CIniFile cifOutput;
    CLabelPrinter *ppr0 = NULL;
    CLabelPrinter *ppr1 = NULL;
    CDiskFile *pstmT;
    CVarBag *pbagMCR;
    CVarBag *pbagT;
    CFloppyCrypt *pfk = NULL;
    HKEY hkey;
    int i;
    DWORD dwXsumBlk, dwXsumRed, dwXsumMediaIn;
    DWORD dwXsumSrc0, dwXsumSrc1;
    DWORD dwXsumDvd;
    FILETIME ftSign;
    DWORD dwOff;
    VPR *pvp;
    LPCSTR szT;
    LPCSTR szSignKey = NULL;
    char szMediaRef[256];
    struct {
        LPCSTR szSection;
        LPCSTR szKey;
        PDWORD pdw;
    } rginpf[] = {
        { szCRC, "Layer0CRC", &dwXsumSrc0 },
        { szCRC, "Layer1CRC", &dwXsumSrc1 },
        { szCRC, "XboxCRCRed", &dwXsumRed },
        { szCRC, "XboxCRCBlack", &dwXsumBlk },
        { szCRC, "XboxMediaCRC", &dwXsumMediaIn },
        { szCRC, "VideoCRC", &dwXsumDvd },
        { szDMI, "TimeStampHigh", &ftSign.dwHighDateTime },
        { szDMI, "TimeStampLow", &ftSign.dwLowDateTime },
        { szXBE, "TitleID", &certKeys.TitleID },
        { szXBE, "Version", &certKeys.Version },
    };

    /* Initial setup */
    while(++rgszArg, --cArg) {
        if(_stricmp(*rgszArg, "-input") == 0) {
            if(!--cArg) {
noarg:
                fprintf(stderr, "error: missing argument for %s\n", *rgszArg);
                exit(1);
            }
            szInputName = *++rgszArg;
        } else if(_stricmp(*rgszArg, "-output") == 0) {
            if(!--cArg)
                goto noarg;
            szOutputName = *++rgszArg;
        } else if(_stricmp(*rgszArg, "-beep") == 0) {
            ++fBeep;
        } else if(_stricmp(*rgszArg, "-tod") == 0) {
            ++fTOD;
        } else if(_stricmp(*rgszArg, "-fksign") == 0) {
            /* We're going to be signing with a floppy key, so grab the
             * signing name */
            if(!--cArg || rgszArg[1][0] == '-')
                goto noarg;
            szSignKey = *++rgszArg;
        } else {
            fprintf(stderr, "error: unknown argument %s\n", *rgszArg);
            exit(1);
        }
    }

    if(ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Xbox Game Signing Tool\\Red2GreenDLT", &hkey) ||
        !(pbagMCR = PbagFromRegKey(hkey)))
    {
        fprintf(stderr, "error: could not read machine configuration registry\n");
        exit(1);
    }

    /* Make sure we can read our input file */
    if(!cifInput.FReadFile(szInputName, TRUE)) {
        fprintf(stderr, "error: could not process input file %s\n",
            szInputName);
        exit(1);
    }
    pbagInput = cifInput.PbagRoot();

    /* Extract the appropriate values from the input file */
    for(i = sizeof rginpf / sizeof rginpf[0]; i--; ) {
        if(!FGetAsDword(pbagInput, rginpf[i].szSection, rginpf[i].szKey,
            rginpf[i].pdw))
        {
badinput:
            fprintf(stderr, "error: missing or incorrect data in input file\n");
            exit(1);
        }
    }

    /* Construct the input and output DMI blocks */
    pbagT = pbagInput->FindProp(szDMI);
    if(!pbagT)
        goto badinput;
    pvp = pbagT->FindProp("XMID");
    if(!pvp || pvp->vpType != vpSz || strlen(pvp->sz) != 8)
        goto badinput;
    rgbDMIIn[6] = 1;
    memcpy(rgbDMIIn + 6 + 8, pvp->sz, 8);
    memcpy(rgbDMIIn + 6 + 16, &ftSign, sizeof ftSign);
    rgbDMIIn[6 + 24] = 1;
    memcpy(rgbDMIOut, rgbDMIIn, sizeof rgbDMIIn);
    GetSystemTimeAsFileTime(&ftSign);
    memcpy(rgbDMIOut + 6 + 16, &ftSign, sizeof ftSign);
    rgbDMIOut[6 + 24] = 2;
    sprintf(szMediaRef, "%.8s", rgbDMIIn + 6 + 8);

    /* Make sure we have a place to stick temporary data */
    szTempPath = pbagMCR->FindSzProp("TemporaryPath");
    pstmT = PstmCreateTemp("test.tst", GENERIC_WRITE);
    if(!pstmT) {
        fprintf(stderr, "error: cannot create temporary files\n");
        exit(1);
    }
    delete pstmT;
    pstmT = NULL;

    /* Make sure we can get our signing keys */
    pccRed = new CDevkitCrypt;
    if(szSignKey) {
        pfk = CFloppyCrypt::PccCreate();
        pccGrn = pfk;
    } else
        pccGrn = CNCipherCrypt::PccCreate(pbagMCR->FindSzProp("MSCSPProvider"),
            pbagMCR->FindSzProp("MSCSPKeyContainer"));
    if(!pccRed || !pccGrn) {
        fprintf(stderr, "error: unable to acquire cryptographic contexts\n");
        exit(1);
    }

    /* Configure the label printers */
    szT = pbagMCR->FindSzProp("LabelPrinter0");
    if(szT) {
        MsgOut("Configuring label printers\n");
        if(!pbagMCR->FFindDwProp("PrinterOffset0", &dwOff))
            dwOff = 0;
        ppr0 = CLabelPrinter::PprCreate(szT, -(int)dwOff);
        if(!ppr0) {
            fprintf(stderr, "error: could not configure label printer 0\n");
            goto fatal;
        }
        szT = pbagMCR->FindSzProp("LabelPrinter1");
        if(szT) {
            if(!pbagMCR->FFindDwProp("PrinterOffset1", &dwOff))
                dwOff = 0;
            ppr1 = CLabelPrinter::PprCreate(szT, -(int)dwOff);
            if(!ppr1) {
                fprintf(stderr, "error: could not configure label printer 1\n");
                goto fatal;
            }
        } else
            ppr1 = ppr0;
    }

    /* Set up the certificate keys.  We need lots of entropy here, so we fill
     * in the LAN key with some entropic data and use that */
    g_rand.RandBytes(certKeys.LANKey, 8);
    ((DWORD *)certKeys.LANKey)[2] = dwXsumSrc0;
    ((DWORD *)certKeys.LANKey)[3] = dwXsumSrc1;
    if(!CXBEFile::FGenNewKeys(&certKeys, pccGrn, certKeys.LANKey, 16)) {
        fprintf(stderr, "error: could not generate XBE keys\n");
        exit(1);
    }
    if(szSignKey) {
        /* We're going to use the input string as the LAN key */
        memset(certKeys.LANKey, 0, sizeof certKeys.LANKey - 4);
        strncpy((LPSTR)certKeys.LANKey, szSignKey, sizeof certKeys.LANKey - 4);
        /* And we're going to use the key ID as the signature key */
        memset(certKeys.SignatureKey, 0, sizeof certKeys.SignatureKey - 4);
        pfk->FGetKeyId(certKeys.SignatureKey - 4);
    }

    /* Mount the source tapes */
    MsgOut("Mounting source tape 0\n");
    if(!FMountTape(&tapSrc0, pbagMCR->FindSzProp("SourceTape0"), fTOD, FALSE)) {
        fprintf(stderr, "error: could not mount source tape 0\n");
        exit(1);
    }
    MsgOut("Mounting source tape 1\n");
    if(!FMountTape(&tapSrc1, pbagMCR->FindSzProp("SourceTape1"), fTOD, FALSE)) {
        fprintf(stderr, "error: could not mount source tape 1\n");
        exit(1);
    }

    /* Mount the destination tapes */
    MsgOut("Mounting destination tape 0\n");
    if(!FMountTape(&tapDst0, pbagMCR->FindSzProp("DestinationTape0"), fTOD,
        TRUE))
    {
        fprintf(stderr, "error: could not mount destination tape 0\n");
        exit(1);
    }
    MsgOut("Mounting destination tape 1\n");
    if(!FMountTape(&tapDst1, pbagMCR->FindSzProp("DestinationTape1"), fTOD,
        TRUE))
    {
        fprintf(stderr, "error: could not mount destination tape 1\n");
        exit(1);
    }
    tapDst0.SetWritePadByte('\n');
    tapDst1.SetWritePadByte('\n');

    /* The first file on each source tape must be the DX2ML file.  Deal with
     * them first */
    if(!FCopyXMLFiles(szMediaRef))
        goto fatal;

    /* Cycle through the contents of each tape */
    if(!FGreensignTape(0, &tapSrc0, &tapDst0))
        goto fatal;
    if(!FGreensignTape(1, &tapSrc1, &tapDst1))
        goto fatal;

    /* Make sure the input CRCs matched */
    if(tapSrc0.DwCheckSum() != dwXsumSrc0 ||
        tapSrc1.DwCheckSum() != dwXsumSrc1 ||
        xsumMediaIn.DwFinalSum() != dwXsumMediaIn ||
        xsumBlk.DwFinalSum() != dwXsumBlk ||
        xsumRed.DwFinalSum() != dwXsumRed ||
        xsumDvd.DwFinalSum() != dwXsumDvd)
    {
        fprintf(stderr, "error: input data has incorrect checksum\n");
        goto fatal;
    }

    /* Destroy the signing contexts */
    delete pccRed;
    pccRed = NULL;
    delete pccGrn;
    pccGrn = NULL;

    /* Unmount the tapes */
    MsgOut("Unounting source tape 0\n");
    tapSrc0.FUnmount();
    MsgOut("Unounting source tape 1\n");
    tapSrc1.FUnmount();
    MsgOut("Unounting destination tape 0\n");
    if(!tapDst0.FUnmount()) {
        fprintf(stderr, "error: unmounting destination tape 0\n");
        goto fatal;
    }
    MsgOut("Unounting destination tape 1\n");
    if(!tapDst1.FUnmount()) {
        fprintf(stderr, "error: unmounting destination tape 1\n");
        goto fatal;
    }

    /* Output CRC values */
    MsgOut("Writing output data\n");
    pbagT = new CVarBag;
    cifOutput.PbagRoot()->FAddProp("RSM", pbagT);
    pbagT->FAddProp("Layer0CRC", &VPR(tapDst0.DwCheckSum()));
    pbagT->FAddProp("Layer1CRC", &VPR(tapDst1.DwCheckSum()));
    pbagT->FAddProp("XboxMediaCRC", &VPR(xsumGrn.DwFinalSum()));
    pbagT->FAddProp("VideoCRC", &VPR(xsumDvd.DwFinalSum()));
    pbagT = new CVarBag;
    cifOutput.PbagRoot()->FAddProp("Xbox", pbagT);
    rgbDMIOut[6 + 16] = 0;
    pbagT->FAddProp("XMID", (LPSTR)rgbDMIOut + 6 + 8);

    if(!cifOutput.FWriteFile(szOutputName)) {
        fprintf(stderr, "error: could not write output file %s\n",
            szOutputName);
        exit(1);
        }

    if(ppr0) {
        MsgOut("Printing label 0\n");
        if(ppr0->FStartLabel()) {
            ppr0->PrintLine("XMID: %.*s\n", 8, rgbDMIOut + 6 + 8);
            ppr0->PrintLine("DLT P/N: %s-L0\n", szMediaRef);
            ppr0->PrintLine("Format: DVD-X2    Layer: 0\n");
            ppr0->PrintLine("DLT CRC: %08X\n", tapDst0.DwCheckSum());
            ppr0->PrintLine("Xbox Media CRC: %08X\n", xsumGrn.DwFinalSum());
            ppr0->PrintLine("DVD Video CRC: %08X\n", xsumDvd.DwFinalSum());
            ppr0->EndLabel();
        } else
            fprintf(stderr, "warning: could not print label 0\n");
    }

    if(ppr1) {
        MsgOut("Printing label 1\n");
        if(ppr1->FStartLabel()) {
            ppr1->PrintLine("XMID: %.*s\n", 8, rgbDMIOut + 6 + 8);
            ppr1->PrintLine("DLT P/N: %s-L1\n", szMediaRef);
            ppr1->PrintLine("Format: DVD-X2    Layer: 1\n");
            ppr1->PrintLine("DLT CRC: %08X\n", tapDst1.DwCheckSum());
            ppr1->PrintLine("Xbox Media CRC: %08X\n", xsumGrn.DwFinalSum());
            ppr1->PrintLine("DVD Video CRC: %08X\n", xsumDvd.DwFinalSum());
            ppr1->EndLabel();
        } else
            fprintf(stderr, "warning: could not print label 1\n");
    }

    if(fBeep)
        DoneBeep();
    return 0;
fatal:
    /* Clean up files */

    /* Destroy the signing contexts */
    if(pccRed)
        delete pccRed;
    if(pccGrn)
        delete pccGrn;

    /* Rewind all tapes */
    tapSrc0.FRewind();
    tapSrc1.FRewind();
    tapDst0.FRewind();
    tapDst1.FRewind();

    /* Unmount the tapes */
    tapSrc0.FUnmount();
    tapSrc1.FUnmount();
    tapDst0.FUnmount();
    tapDst1.FUnmount();

    if(fBeep)
        DoneBeep();
    return 1;
}
