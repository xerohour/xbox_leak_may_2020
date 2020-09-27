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
CTapeDevice tapSrc;
CTapeDevice tapDst0, tapDst1;
CDiskFile *pstmImage0, *pstmImage1;
CCryptContainer *pccRed;
CFloppyCrypt *pccSign;
CCryptContainer *pccXControl;
CCheckSum xsumBlk, xsumRed, xsumGrn, xsumMediaIn;
CCheckSum xsumDvd;
CIniBag *pbagInput;
CDX2MLFile xml;
CXControl xct;
CECMAControl ect;
DWORD cpsnDvd0, cpsnDvd1;
BYTE rgbDMI[2054];
BOOL fSetCertTitle;
BOOL fDemoMode;
XBEIMAGE_CERTIFICATE cert;
char szVideoPath[1024];

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

BOOL FReadXMLFile(void)
{
    int idxi;
    int idxl;
    int idxr;
    DXML *pdxml;
    DXI *pdxi;
    DXR *pdxr;
    CDiskFile *pstmXML = NULL;
    DWORD dwDataBlkSize;
    DWORD dwDataPerPhy;
    BOOL fRet = FALSE;
    char szFileName[32];

    MsgOut("Reading input DX2ML.XML\n");
//#define WASBADXML
#ifdef WASBADXML
    pstmXML = PstmCreateTemp("dx2ml.xml", GENERIC_READ);
    if(!pstmXML) {
        fprintf(stderr, "error: no XML file to process\n");
        return FALSE;
    }
    pstmXML->SetDeleteOnClose(FALSE);
#else
    /* Get disk space for the files */
    pstmXML = PstmCreateTemp("dx2ml.xml", GENERIC_WRITE);
    if(!pstmXML) {
        fprintf(stderr, "error: cannot create temporary XML file\n");
        goto fatal;
    }
#endif

    if(!tapSrc.FReadFile(szFileName, &dwDataBlkSize, &dwDataPerPhy) ||
        strcmp(szFileName, "DX2ML.XML"))
    {
        fprintf(stderr, "error: source tape 0 missing XML file\n");
        goto fatal;
    }
#ifndef WASBADXML
    if(!tapSrc.FCopyToStm(pstmXML) || !tapSrc.FCloseFile()) {
        fprintf(stderr, "error: could not copy XML file from source tape\n");
        goto fatal;
    }
    if(!pstmXML->FReopen(GENERIC_READ))
        goto badxml;
#endif

    /* Process the DX2ML file */
    if(!xml.FReadFile(pstmXML->SzName(), FALSE)) {
badxml:
        fprintf(stderr, "error: could not process input DX2ML file\n");
        goto fatal;
    }

    if(!xml.FValidateForm())
        goto badxml;

    /* The only data that should be specified in this XML is the Xbox data
     * and the presence of the Xbox leadout stream */
    pdxml = xml.PdxmlGet();
    pdxi = &pdxml->rgdxi[0];
    for(idxi = 0; idxi < idxiMax; ++idxi, ++pdxi) {
        if(pdxi->f2054)
            goto badxml;
        if(idxi == idxiXControl && !pdxi->szNameOnTape)
            goto badxml;
        if(idxi == idxiControl && pdxi->szNameOnTape)
            goto badxml;
    }

    for(idxl = 0; idxl < 2; ++idxl) {
        pdxr = &pdxml->rgdxl[idxl].rgdxrAreas[0];
        for(idxr = 0; idxr < idxrMax; ++idxr, ++pdxr) {
            if(idxr == idxrXData) {
                if(pdxr->psnStart == 0)
                    goto badxml;
                if(pdxr->idxiImage < 0)
                    goto badxml;
                if(!pdxml->rgdxi[pdxr->idxiImage].szNameOnTape)
                    goto badxml;
            } else {
                if(pdxr->psnStart && pdxr->cpsnSize)
                    goto badxml;
                if(pdxr->idxiImage >= 0)
                    goto badxml;
            }
        }
    }

    fRet = TRUE;
fatal:
    if(pstmXML)
        delete pstmXML;
    return fRet;
}

BOOL FSetupXMLFile(void)
{
    int idxi;
    int ilyr;
    DXI dxiX0, dxiX1;
    DXML *pdxml;
    DXI *pdxi;
    DXL *pdxl;
    DXR *pdxr;
    DWORD psnStart0;
    DWORD psnEnd0;
    DWORD psnEnd1;

    MsgOut("Constructing output XML\n");
    pdxml = xml.PdxmlGet();

    /* Mark the xcontrol as being 2054 block mode */
    pdxml->rgdxi[idxiXControl].f2054 = TRUE;
    pdxml->rgdxl[1].dxrLead.idxiImage = idxiXControl;

    /* We need to make sure the input streams show up in the right order, so
     * move the XData streams */
    dxiX0 = pdxml->rgdxi[pdxml->rgdxl[0].dxrXData.idxiImage];
    dxiX1 = pdxml->rgdxi[pdxml->rgdxl[1].dxrXData.idxiImage];
    pdxml->rgdxi[3] = dxiX0;
    pdxml->rgdxi[4] = dxiX1;
    pdxml->rgdxl[0].dxrXData.idxiImage = 3;
    pdxml->rgdxl[1].dxrXData.idxiImage = 4;

    /* Configure the DVD input streams */
    pdxi = &pdxml->rgdxi[idxiControl];
    pdxi->szNameOnTape = _strdup("CONTROL.DAT");
    pdxi->szRefName = _strdup("Leadin0");
    pdxi->dwFlags = 0;
    pdxi->fUsedLyr0 = pdxi->fUsedLyr1 = TRUE;
    pdxml->rgdxl[0].dxrLead.idxiImage = idxiControl;
    pdxi = &pdxml->rgdxi[2];
    memset(pdxi, 0, sizeof *pdxi);
    pdxi->szRefName = _strdup("DvdLayer0Data");
    pdxi->szNameOnTape = _strdup("IMAGE0.DAT");
    pdxi->dwImageSize = cpsnDvd0;
    pdxi->fUsedLyr0 = TRUE;
    pdxml->rgdxl[0].dxrData.idxiImage = 2;
    pdxi = &pdxml->rgdxi[5];
    memset(pdxi, 0, sizeof *pdxi);
    pdxi->szRefName = _strdup("DvdLayer1Data");
    pdxi->szNameOnTape = _strdup("IMAGE1.DAT");
    pdxi->dwImageSize = cpsnDvd1;
    pdxi->fUsedLyr1 = TRUE;
    pdxml->rgdxl[1].dxrData.idxiImage = 5;
    ect.GetPsns(&psnStart0, &psnEnd0, &psnEnd1);

    /* Configure the layer 0 regions */
    pdxl = &pdxml->rgdxl[0];
    pdxr = &pdxl->rgdxrAreas[0];
    
    /* Layer0: leadin */
    pdxl->psnStart = 0x01F360;
    pdxr->psnStart = 0x02F000;
    pdxl->ipsnControlStart = 0x02F200 - pdxr->psnStart;
    pdxr->cpsnSize = psnStart0 - pdxr->psnStart;

    /* Layer0: data */
    ++pdxr;
    pdxr->psnStart = psnStart0;
    pdxr->cpsnSize = psnEnd0 - psnStart0 + 1;

    /* Layer0: middle */
    ++pdxr;
    pdxr->psnStart = psnEnd0 + 1;
    if(pdxr->psnStart > 0x050E00) {
baddata:
        fprintf(stderr, "error: failed to generate XML data\n");
        return FALSE;
    }
    pdxr->cpsnSize = 0x05B200 - pdxr->psnStart;

    /* Layer0: XLeadin */
    ++pdxr;
    pdxr->psnStart = 0x05B200;
    if(pdxr[1].psnStart < pdxr->psnStart + 0x005400)
        goto baddata;
    pdxr->cpsnSize = pdxr[1].psnStart - pdxr->psnStart;

    /* Layer0: XMiddle */
    ++pdxr;
    ++pdxr;
    /* start PSN is already correct from XML read */
    if(pdxr->psnStart + 0x14860 > 0x2377A0)
        goto baddata;
    pdxr->cpsnSize = 0x2377A0 - pdxr->psnStart;

    /* Now configure the layer1 regions */
    ++pdxl;
    pdxl->psnStart = 0xDC8860;
    pdxr = &pdxl->rgdxrAreas[idxrXMiddle];

    /* Layer1: XMiddle */
    pdxr->psnStart = pdxl->psnStart;
    pdxr->cpsnSize = pdxr[-1].psnStart - pdxr->psnStart;

    /* Layer1: XLeadin */
    --pdxr;
    --pdxr;
    pdxr->psnStart = pdxr[1].psnStart + pdxr[1].cpsnSize;
    if(pdxr->psnStart > 0xF9FA00)
        goto baddata;
    pdxr->cpsnSize = 0xFA4E00 - pdxr->psnStart;

    /* Layer1: middle */
    --pdxr;
    pdxr->psnStart = 0xFA4E00;
    psnEnd0 ^= 0xFFFFFF;
    if(psnEnd0 < pdxr->psnStart + 0x005400)
        goto baddata;
    pdxr->cpsnSize = psnEnd0 - pdxr->psnStart;

    /* Layer1: data */
    --pdxr;
    pdxr->psnStart = psnEnd0;
    pdxr->cpsnSize = cpsnDvd1;

    /* Layer1: leadout */
    --pdxr;
    pdxr->psnStart = psnEnd0 + cpsnDvd1;
    if(psnEnd1 > 0xFD0000)
        goto baddata;
    pdxr->cpsnSize = 0xFE0CA0 - pdxr->psnStart;
    pdxl->ipsnControlStart = 0xFD0200 - pdxr->psnStart;

    /* Make sure we did it all right */
    if(!xml.FValidateContent())
        goto baddata;

    return TRUE;
}

BOOL FSetupVideoFiles(void)
{
    LARGE_INTEGER li;
    ULONG cpsn1;
    ULONG psnStart0, psnEnd0, psnEnd1;
    BOOL fRet = FALSE;
    char szName[1024];
    CDiskFile *pstmControl;

    /* Find our DVD video control file */
    sprintf(szName, "%s\\control.dat", szVideoPath);
    pstmControl = new CDiskFile(szName, GENERIC_READ);
    if(!pstmControl->FIsOpen())
        goto readerr;
    if(!ect.FInit(pstmControl)) {
badcontrol:
        fprintf(stderr, "error: bad DVD video control data\n");
        return FALSE;
    }
    ect.GetPsns(&psnStart0, &psnEnd0, &psnEnd1);
    /* Data needs to start in a fixed location */
    if(psnStart0 != 0x030000)
        goto badcontrol;
    /* Layer 0 data needs to end on an ECC boundary and not be too big */
    if((psnEnd0 & 0xF) != 0xF || psnEnd0 >= 0x050E00)
        goto badcontrol;
    /* Layer 1 data can't exceed layer 0 in size */
    if((psnEnd1 ^ 0xFFFFFF) < psnStart0)
        goto badcontrol;

    /* Find our DVD video image files */
    sprintf(szName, "%s\\image0.dat", szVideoPath);
    pstmImage0 = new CDiskFile(szName, GENERIC_READ);
    sprintf(szName, "%s\\image1.dat", szVideoPath);
    pstmImage1 = new CDiskFile(szName, GENERIC_READ);

    if(!pstmImage0->FIsOpen() || !pstmImage1->FIsOpen()) {
readerr:
        fprintf(stderr, "error: could not read DVD video files\n");
        return FALSE;
    }

    /* Get their sizes and validate against the control data */
    if(!GetFileSizeEx(*pstmImage0, &li))
        goto readerr;
    /* Need to have an integral number of disk blocks (not necessarily ECC
     * blocks) */
    if(li.LowPart & 0x7FF)
        goto readerr;
    cpsnDvd0 = (ULONG)(li.QuadPart / 2048);
    if(cpsnDvd0 != psnEnd0 - psnStart0 + 1) {
baddata:
        fprintf(stderr, "DVD video data is of invalid size\n");
        return FALSE;
    }
    if(!GetFileSizeEx(*pstmImage1, &li))
        goto readerr;
    if(li.LowPart & 0x7FF)
        goto readerr;
    cpsnDvd1 = (ULONG)(li.QuadPart / 2048);
    cpsn1 = psnEnd1 - (psnEnd0 ^ 0xFFFFFF) + 1;
    cpsn1 = (cpsn1 + 0xF) & ~0xF;
    if(cpsnDvd1 != cpsn1)
        goto baddata;

    return TRUE;
}

BOOL FReadXControlFile(void)
{
    int i;
    int ib;
    DXR *pdxr0;
    DXR *pdxr1;
    DXML *pdxml;
    DWORD psnStart0;
    DWORD psnEnd0;
    DWORD psnEnd1;
    DWORD dwDataBlkSize;
    DWORD dwDataPerPhy;
    BYTE b;
    BYTE rgb[2048];
    char szFileName[32];

    MsgOut("Reading input XCONTROL.DAT\n");
    if(!tapSrc.FReadFile(szFileName, &dwDataBlkSize, &dwDataPerPhy) ||
        dwDataBlkSize != 2048 || strcmp(szFileName, "XCONTROL.DAT"))
    {
        fprintf(stderr, "error: source tape missing xcontrol.dat file\n");
        return FALSE;
    }
    /* First make sure that we have 30 blocks of zero */
    for(i = 0; i < 30; ++i) {
        if(tapSrc.CbRead(sizeof rgb, rgb) != sizeof rgb) {
readerr:
            fprintf(stderr, "error: could not read XControl from tape\n");
            return FALSE;
        }
        for(b = 0, ib = 0; ib < sizeof rgb; ++ib)
            b |= rgb[ib];
        if(b) {
baddata:
            fprintf(stderr, "error: invalid XControl data on tape\n");
            return FALSE;
        }
    }

    /* Now read the XControl frame */
    if(!xct.FInit(&tapSrc))
        goto baddata;

    /* Make sure that the XControl data matches the XML data */
    if(!xct.FValidate(xml.PdxmlGet(), NULL))
        goto baddata;

    /* We shouldn't have data in the DMI frame; make sure it's empty */
    if(tapSrc.CbRead(sizeof rgb, rgb) != sizeof rgb)
        goto readerr;
    for(b = 0, ib = 0; ib < sizeof rgb; ++ib)
        b |= rgb[ib];
    if(b)
        goto baddata;

    /* There should be no more data in the file */
    if(tapSrc.CbRead(1, rgb) != 0)
        goto baddata;

    return TRUE;
}

BOOL FWriteBaseTapeFiles(int ilyr, CTapeDevice *ptapDst, LPCSTR szMediaRef)
{
    int i;
    BYTE rgb[2054];
    CDX2MLFile *pxml = &xml;
    BOOL fRet = FALSE;
    DXML *pdxml;
    char szLayerRef[256];

    /* Stamp the layer ref into the XML file */
    sprintf(szLayerRef, "%s-L%d", szMediaRef, ilyr);
    pxml->SetMasteringRef(szLayerRef);

    /* First write the DX2ML file */
    MsgOut("Writing DX2ML.XML to layer %d\n", ilyr);
    if(!ptapDst->FWriteFile("DX2ML.XML", 32768, 1)) {
writeerr:
        fprintf(stderr, "error: writing to tape %d\n", ilyr);
        goto fatal;
    }
    pdxml = pxml->PdxmlGet();
    pdxml->iLayer = ilyr;
    if(!pxml->FWriteFile(ptapDst))
        goto writeerr;
    if(!ptapDst->FCloseFile())
        goto writeerr;

    /* Next comes CONTROL.DAT */
    MsgOut("Writing CONTROL.DAT to layer %d\n", ilyr);
    if(!ptapDst->FWriteFile("CONTROL.DAT", 2048, 16))
        goto writeerr;
    /* Control data is first */
    if(!ect.FWriteToStm(ptapDst))
        goto writeerr;
    /* Then the DMI */
    if(ptapDst->CbWrite(2048, rgbDMI + 6) != 2048)
        goto writeerr;
    /* Now 14 blocks of zero */
    memset(rgb, 0, sizeof rgb);
    for(i = 0; i < 14; ++i) {
        if(ptapDst->CbWrite(2048, rgb) != 2048)
            goto writeerr;
    }
    if(!ptapDst->FCloseFile())
        goto writeerr;

    /* Next comes XCONTROL.DAT, which we have to write in 2054-byte mode */
    MsgOut("Writing XCONTROL.DAT to layer %d\n", ilyr);
    if(!ptapDst->FWriteFile("XCONTROL.DAT", 2054, 16))
        goto writeerr;
    /* First we write 30 blocks of zero */
    for(i = 0; i < 30; ++i) {
        if(ptapDst->CbWrite(sizeof rgb, rgb) != sizeof rgb)
            goto writeerr;
    }
    /* Then the xcontrol data */
    if(!xct.FSign(pccXControl) || !xct.FWriteToStm(ptapDst, 2054))
        goto writeerr;
    /* Then the DMI */
    if(ptapDst->CbWrite(sizeof rgbDMI, rgbDMI) != sizeof rgbDMI)
        goto writeerr;
    if(!ptapDst->FCloseFile())
        goto writeerr;

    fRet = TRUE;
fatal:
    return fRet;
}

BOOL FWriteAPData(int ilyr, int ixph, CTapeDevice *ptapDst)
{
    MsgOut("Writing placeholder data\n");
    return xct.FWriteAPData(ilyr, ixph, ptapDst);
}

BOOL FRedsignXBE(int ilyr, CXBEFile *pxbe, DWORD cblk, CTapeDevice *ptapDst)
{
    const XBEIMAGE_CERTIFICATE *pcert;

    MsgOut("Processing XBE file\n");
    pxbe->UseCryptContainer(pccRed);
    if(!pxbe->FVerifySignature()) {
badxbe:
        fprintf(stderr, "error: found an invalid XBE file on tape %d\n", ilyr);
        return FALSE;
    }

    /* Fetch the old certificate data, if necessary */
    pcert = pxbe->GetCert();
    if(!pcert)
        goto badxbe;
    if(!fSetCertTitle)
        memcpy(cert.TitleName, pcert->TitleName, sizeof pcert->TitleName);
    if(fDemoMode)
        cert.GameRatings = pcert->GameRatings;

    /* Inject the new certificate */
    if(!pxbe->FInjectNewCertificate(&cert)) {
        fprintf(stderr, "error: could not inject XBE certificate\n");
        return FALSE;
    }

    if(pccSign)
        pxbe->UseCryptContainer(pccSign);
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

BOOL FCopyTapeImages(void)
{
    int ilyr;
    int idxi;
    DXML *pdxml = xml.PdxmlGet();
    DXL *pdxl;
    CTapeDevice *ptapDst;
    DWORD dwDataBlkSize;
    DWORD dwDataPerPhy;
    char szFileName[32];

    /* We write DVD image 0 first */
    MsgOut("Writing layer 0 DVD video data\n");
    if(!pstmImage0->FSeek(0)) {
cantcopy:
        fprintf(stderr, "error: coyping DVD video to tape %d\n", ilyr);
        return FALSE;
    }
    if(!tapDst0.FWriteFile("IMAGE0.DAT", 2048, 16))
        goto writeerr;
    if(!pstmImage0->FCopyToStm(&tapDst0, &xsumDvd))
        goto cantcopy;
    if(!tapDst0.FCloseFile())
        goto writeerr;

    /* Now we process the tape images */
    while(tapSrc.FReadFile(szFileName, &dwDataBlkSize, &dwDataPerPhy)) {
        /* If this is the layer 0 file, process it */
        pdxl = &pdxml->rgdxl[0];
        for(ilyr = 0; ilyr < 2; ++ilyr, ++pdxl) {
            idxi = pdxl->dxrXData.idxiImage;
            if(0 == strcmp(szFileName, pdxml->rgdxi[idxi].szNameOnTape))
                break;
        }
        if(ilyr == 2) {
            fprintf(stderr, "error: unknown file %s on source tape\n",
                szFileName);
            return FALSE;
        }
        if(dwDataBlkSize != 2048) {
            fprintf(stderr, "error: %s has wrong block size\n", szFileName);
            return FALSE;
        }
        ptapDst = ilyr ? &tapDst1 : &tapDst0;
        MsgOut("Processing Xbox image data for layer %d\n", ilyr);
        if(!ptapDst->FWriteFile(szFileName, 2048, 16)) {
writeerr:
            fprintf(stderr, "error: writing to tape %d\n", ilyr);
            return FALSE;
        }
        if(!FProcessXImage(ilyr, pdxl, idxrXData, &tapSrc, ptapDst,
                FRedsignXBE, FWriteAPData))
            return FALSE;
        if(!ptapDst->FCloseFile())
            goto writeerr;
    }

    /* Finally we write the layer 1 video */
    MsgOut("Writing layer 1 DVD video data\n");
    if(!pstmImage1->FSeek(0))
        goto cantcopy;
    if(!tapDst1.FWriteFile("IMAGE1.DAT", 2048, 16))
        goto writeerr;
    if(!pstmImage1->FCopyToStm(&tapDst1, &xsumDvd))
        goto cantcopy;
    if(!tapDst1.FCloseFile())
        goto writeerr;

    return TRUE;
}

BOOL FSetupSecurity(void)
{
    MsgOut("Preparing xcontrol AP data\n");
    return xct.FSetupAP(xml.PdxmlGet());
}

BOOL FGetAsDword(CIniBag *pbag, LPCSTR szSection, LPCSTR szKey, PDWORD pdw)
{
    CVarBag *pbvp = pbag->FindProp(szSection);
    if(!pbvp)
        return FALSE;
    VPR *pvpValue = pbvp->FindProp(szKey);
    if(!pvpValue || pvpValue->vpType != vpDw)
        return FALSE;
    *pdw = pvpValue->dw;
    return TRUE;
}

int __cdecl main(int cArg, char **rgszArg)
{
    LPCSTR szInputName = "xbcert.ini";
    LPCSTR szOutputName = "xbvalidate.ini";
    LPCSTR szVideoName = NULL;
    BOOL fBeep = FALSE;
    BOOL fTOD = FALSE;
    BOOL fRedDVD = FALSE;
    BOOL fNoCheckPH = FALSE;
    CIniFile cifInput;
    CIniFile cifOutput;
    CDiskFile *pstmT;
    CVarBag *pbagMCR;
    CVarBag *pbagT;
    CLabelPrinter *ppr0 = NULL;
    CLabelPrinter *ppr1 = NULL;
    HKEY hkey;
    int i;
    DWORD dwXsumSrc;
    DWORD dwSubmission;
    FILETIME ftSign;
    DWORD dwOff;
    VPR *pvp;
    LPCSTR szSignKey = NULL;
    LPCSTR szT;
    char szMediaRef[16];

    /* Initial setup */
    while(++rgszArg, --cArg) {
        if(_stricmp(*rgszArg, "-video") == 0) {
            if(!--cArg) {
noarg:
                fprintf(stderr, "error: missing argument for %s\n", *rgszArg);
                exit(1);
            }
            szVideoName = *++rgszArg;
        } else if(_stricmp(*rgszArg, "-input") == 0) {
            if(!--cArg)
                goto noarg;
            szInputName = *++rgszArg;
        } else if(_stricmp(*rgszArg, "-output") == 0) {
            if(!--cArg)
                goto noarg;
            szOutputName = *++rgszArg;
        } else if(_stricmp(*rgszArg, "-beep") == 0) {
            ++fBeep;
        } else if(_stricmp(*rgszArg, "-tod") == 0) {
            ++fTOD;
        } else if(_stricmp(*rgszArg, "-nocheckph") == 0) {
            ++fNoCheckPH;
        } else if(_stricmp(*rgszArg, "-reddvd") == 0) {
            ++fRedDVD;
        } else if(_stricmp(*rgszArg, "-demodisk") == 0) {
            ++fDemoMode;
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

    if(!szVideoName) {
        fprintf(stderr, "error: missing DVD video name\n");
        exit(1);
    }

    if(ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Xbox Game Signing Tool\\Black2RedDLT", &hkey) ||
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

    /* Prepare the DVD video files */
    MsgOut("Processing DVD video files\n");
    sprintf(szVideoPath, "%s\\%s", pbagMCR->FindSzProp("DVDVideoPath"),
        szVideoName);
    if(!FSetupVideoFiles())
        goto fatal;

    /* Prepare the certificate data */
    if(!CXBEFile::FUnpackCertificate(pbagInput->FindProp("XBE Certificate"),
        &cert))
    {
        fprintf(stderr, "error: bad input certificate data\n");
        goto fatal;
    }
    fSetCertTitle = cert.TitleName[0];
    if(!CXBEFile::FGenNewKeys(&cert, NULL)) {
        fprintf(stderr, "error: could not generate XBE keys\n");
        goto fatal;
    }
    if(szSignKey) {
        /* Acquire the signing context before we do anything else */
        pccSign = CFloppyCrypt::PccCreate();
        if(!pccSign) {
            fprintf(stderr, "error: unable to acquire key context\n");
            exit(1);
        }

        /* We're going to use the input string as the LAN key */
        memset(cert.LANKey, 0, sizeof cert.LANKey - 4);
        strncpy((LPSTR)cert.LANKey, szSignKey, sizeof cert.LANKey - 4);
        /* And we're going to use the key ID as the signature key */
        memset(cert.SignatureKey, 0, sizeof cert.SignatureKey - 4);
        pccSign->FGetKeyId(cert.SignatureKey - 4);
    }

    /* Construct the DMI block */
    pbagT = pbagInput->FindProp("XBCERT");
    if(!pbagT) {
badinput:
        fprintf(stderr, "error: missing or incorrect data in input file\n");
        exit(1);
    }
    pvp = pbagT->FindProp("XMID");
    if(!pvp || pvp->vpType != vpSz || strlen(pvp->sz) != 8)
        goto badinput;
    rgbDMI[6] = 1;
    memcpy(rgbDMI + 6 + 8, pvp->sz, 8);
    GetSystemTimeAsFileTime(&ftSign);
    memcpy(rgbDMI + 6 + 16, &ftSign, sizeof ftSign);
    rgbDMI[6 + 24] = 1;

    /* Get other appropriate input data */
    if(!FGetAsDword(pbagInput, "XBCERT", "InputCRC", &dwXsumSrc) ||
            !FGetAsDword(pbagInput, "XBCERT", "Submission", &dwSubmission))
        goto badinput;
    if(dwSubmission)
        sprintf(szMediaRef, "%.*s-R%d", 8, rgbDMI + 6 + 8,
            dwSubmission);
    else
        sprintf(szMediaRef, "%.*s", 8, rgbDMI + 6 + 8);

    /* Make sure we have a place to stick temporary data */
    szTempPath = pbagMCR->FindSzProp("TemporaryPath");
    pstmT = PstmCreateTemp("test.tst", GENERIC_WRITE);
    if(!pstmT) {
        fprintf(stderr, "error: cannot create temporary files\n");
        exit(1);
    }
    delete pstmT;
    pstmT = NULL;

    /* Set up our devkit key */
    pccRed = new CDevkitCrypt;

    /* Set up our XControl signing key */
    if(pccSign && !fRedDVD)
        pccXControl = pccSign;
    else
        pccXControl = pccRed;

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

    /* Mount the source tape */
    MsgOut("Mounting source tape\n");
    if(!FMountTape(&tapSrc, pbagMCR->FindSzProp("SourceTape"), fTOD, FALSE)) {
        fprintf(stderr, "error: could not mount source tape\n");
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

    /* We need to process the DX2ML file and the XControl file first.  They
     * should appear on the tape in that order */
    if(!FReadXMLFile() || !FReadXControlFile())
        goto fatal;

    /* Make sure the placeholder locations are valid */
    if(fNoCheckPH) {
        if(!xml.FValidatePlaceholderLocations(TRUE, FALSE, TRUE))
            goto fatal;
    } else if(!xml.FValidatePlaceholderLocations(FALSE, FALSE, TRUE))
        goto fatal;

    /* Build our security data */
    if(!FSetupSecurity())
        goto fatal;

    /* Now go back and put our XML data together */
    if(!FSetupXMLFile())
        goto fatal;

    /* With that out of the way, we write the base files on each tape */
    if(!FWriteBaseTapeFiles(0, &tapDst0, szMediaRef))
        goto fatal;
    if(!FWriteBaseTapeFiles(1, &tapDst1, szMediaRef))
        goto fatal;

    /* Now we deal with the two image files */
    if(!FCopyTapeImages())
        goto fatal;

    /* Make sure the input CRCs matched */
    if(tapSrc.DwCheckSum() != dwXsumSrc) {
        fprintf(stderr, "error: input data has incorrect checksum\n");
        goto fatal;
    }

    /* Destroy the signing contexts */
    delete pccRed;
    pccRed = NULL;
    if(pccSign) {
        delete pccSign;
        pccSign = NULL;
    }

    /* Unmount the tapes */
    MsgOut("Unounting source tape\n");
    tapSrc.FUnmount();
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

    /* Output XBVALIDATE values */
    MsgOut("Writing output data\n");
    pbagT = new CVarBag;
    cifOutput.PbagRoot()->FAddProp("Disk Manufacturing Information", pbagT);
    rgbDMI[6 + 16] = 0;
    pbagT->FAddProp("XMID", (LPSTR)(rgbDMI + 6 + 8));
    pbagT->FAddProp("TimeStampLow", ftSign.dwLowDateTime);
    pbagT->FAddProp("TimeStampHigh", ftSign.dwHighDateTime);
    pbagT = new CVarBag;
    cifOutput.PbagRoot()->FAddProp("XBE Information", pbagT);
    pbagT->FAddProp("TitleID", cert.TitleID);
    pbagT->FAddProp("Version", cert.Version);
    pbagT = new CVarBag;
    cifOutput.PbagRoot()->FAddProp("Xbox", pbagT);
    pbagT->FAddProp("Submission", dwSubmission);
    pbagT = new CVarBag;
    cifOutput.PbagRoot()->FAddProp("CRC", pbagT);
    pbagT->FAddProp("Layer0CRC", tapDst0.DwCheckSum());
    pbagT->FAddProp("Layer1CRC", tapDst1.DwCheckSum());
    pbagT->FAddProp("XboxMediaCRC", xsumGrn.DwFinalSum());
    pbagT->FAddProp("XboxCRCRed", xsumRed.DwFinalSum());
    pbagT->FAddProp("XboxCRCBlack", xsumBlk.DwFinalSum());
    pbagT->FAddProp("VideoCRC", xsumDvd.DwFinalSum());

    if(!cifOutput.FWriteFile(szOutputName)) {
        fprintf(stderr, "error: could not write output file %s\n",
            szOutputName);
        exit(1);
        }

    if(ppr0) {
        MsgOut("Printing label 0\n");
        if(ppr0->FStartLabel()) {
            ppr0->PrintLine("XMID: %.*s\n", 8, rgbDMI + 6 + 8);
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
            ppr1->PrintLine("XMID: %.*s\n", 8, rgbDMI + 6 + 8);
            ppr1->PrintLine("DLT P/N: %s-L1\n", szMediaRef);
            ppr1->PrintLine("Format: DVD-X2    Layer: 1\n");
            ppr1->PrintLine("DLT CRC: %08X\n", tapDst1.DwCheckSum());
            ppr1->PrintLine("Xbox Media CRC: %08X\n", xsumGrn.DwFinalSum());
            ppr1->PrintLine("DVD Video CRC: %08X\n", xsumDvd.DwFinalSum());
            ppr1->EndLabel();
        } else
            fprintf(stderr, "warning: could not print label 1\n");
    }

    /* Clean up the printers */
    if(ppr1 != ppr0)
        delete ppr1;
    if(ppr0)
        delete ppr0;
    ppr1 = ppr0 = NULL;

    if(fBeep)
        DoneBeep();
    return 0;
fatal:
    /* Clean up files */

    /* Destroy the signing contexts */
    if(pccRed)
        delete pccRed;
    if(pccSign)
        delete pccSign;

    /* Rewind all tapes */
    tapSrc.FRewind();
    tapDst0.FRewind();
    tapDst1.FRewind();

    /* Unmount the tapes */
    tapSrc.FUnmount();
    tapDst0.FUnmount();
    tapDst1.FUnmount();

    /* Clean up the printers */
    if(ppr1 != ppr0)
        delete ppr1;
    if(ppr0)
        delete ppr0;

    if(fBeep)
        DoneBeep();
    return 1;
}
