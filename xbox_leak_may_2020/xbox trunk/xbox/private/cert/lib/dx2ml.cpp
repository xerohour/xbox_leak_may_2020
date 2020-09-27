/*
 *
 * dx2ml.cpp
 *
 * CDX2ML
 *
 */

#include "precomp.h"

#define SHOWERRS

#ifndef DVDEMU
__declspec(thread) BOOL fDidCoInit;
#endif

CDX2MLFile::CDX2MLFile(const CDX2MLFile &xml)
#ifndef DVDEMU
    : m_pxdoc(NULL)
#endif
{
    int i;
    DXI *pdxi;
    DXL *pdxl;
    XBI *pxbi;

    if(xml.m_pxml) {
        /* We'll memcpy the whole thing over and then replicate the fields
         * as appropriate */
        m_pxml = new DXML(*xml.m_pxml);
        pdxi = &m_pxml->rgdxi[0];
        for(i = 0; i < idxiMax; ++i, ++pdxi) {
            if(pdxi->szNameOnDisk)
                pdxi->szNameOnDisk = _strdup(pdxi->szNameOnDisk);
            if(pdxi->szNameOnTape)
                pdxi->szNameOnTape = _strdup(pdxi->szNameOnTape);
            if(pdxi->szRefName)
                pdxi->szRefName = _strdup(pdxi->szRefName);
        }
        pdxl = &m_pxml->rgdxl[0];
        for(i = 0; i < 2; ++i, ++pdxl) {
            if(pdxl->pxbi) {
                pxbi = pdxl->pxbi;
                pdxl->pxbi = new XBI[pdxl->ixbiMax];
                memcpy(pdxl->pxbi, pxbi, pdxl->ixbiMac * sizeof(XBI));
            }
        }
    }
}
CDX2MLFile::~CDX2MLFile()
{
    if(m_pxml) {
        for(int idxi = 0; idxi < idxiMax; ++idxi) {
            if(m_pxml->rgdxi[idxi].szNameOnDisk)
                delete m_pxml->rgdxi[idxi].szNameOnDisk;
            if(m_pxml->rgdxi[idxi].szNameOnTape)
                delete m_pxml->rgdxi[idxi].szNameOnTape;
            if(m_pxml->rgdxi[idxi].szRefName)
                delete m_pxml->rgdxi[idxi].szRefName;
        }
        if(m_pxml->szMasteringRef)
            delete m_pxml->szMasteringRef;
#ifndef DVDEMU
        delete m_pxml;
#endif
    }
}

static BOOL FCompareStrings(LPCSTR sz1, LPCSTR sz2)
{
    if(sz1) {
        if(sz2)
            return 0 == strcmp(sz1, sz2);
        else
            return FALSE;
    } else
        return NULL == sz2;
}

bool DXI::operator==(const DXI &dxi) const
{
    return
        FCompareStrings(szRefName, dxi.szRefName) &&
        FCompareStrings(szNameOnDisk, dxi.szNameOnDisk) &&
        FCompareStrings(szNameOnTape, dxi.szNameOnTape) &&
        dwImageSize == dxi.dwImageSize &&
        dwFlags == dxi.dwFlags;
}

bool DXL::operator==(const DXL &dxl) const
{
    return
        0 == memcmp(this, &dxl, (PBYTE)&dxl.pxbi - (PBYTE)&dxl) &&
        0 == memcmp(pxbi, dxl.pxbi, ixbiMac * sizeof(XBI));
}

bool DXML::operator==(const DXML &dxml) const
{
    int idxi;

    for(idxi = 0; idxi < idxiMax; ++idxi) {
        if(rgdxi[idxi] != dxml.rgdxi[idxi])
            return FALSE;
    }
    return
        rgdxl[0] == dxml.rgdxl[0] &&
        rgdxl[1] == dxml.rgdxl[1] &&
        FCompareStrings(szMasteringRef, dxml.szMasteringRef) &&
        fLayoutTape == dxml.fLayoutTape;
}

bool CDX2MLFile::operator==(const CDX2MLFile &xml) const
{
    int idxi;

    if(m_pxml) {
        if(xml.m_pxml)
            return *m_pxml == *xml.m_pxml;
        else
            return FALSE;
    } else
        return NULL == xml.m_pxml;
}

static int ilexXml;
static struct XI {
    XI *pxiPrev;
    LPCSTR szTagCur;
} *pxiTop;

static void PrintXml(CFileStmSeq *pstm, LPCSTR sz)
{
    for(int i = 0; i < ilexXml; ++i)
        pstm->CbWrite(2, (PBYTE)"  ");
    pstm->CbWrite(strlen(sz), (PBYTE)sz);
    pstm->CbWrite(2, (PBYTE)"\r\n");
}

static void StartElem(CFileStmSeq *pstm, LPCSTR szTag, LPCSTR szAttrs)
{
    char sz[1024];

    XI *pxi = new XI;
    pxi->pxiPrev = pxiTop;
    pxiTop = pxi;
    pxi->szTagCur = _strdup(szTag);
    if(szAttrs)
        sprintf(sz, "<%s %s>", szTag, szAttrs);
    else
        sprintf(sz, "<%s>", szTag);
    PrintXml(pstm, sz);
    ++ilexXml;
}

static void EndElem(CFileStmSeq *pstm)
{
    char sz[64];

    XI *pxi = pxiTop;
    pxiTop = pxiTop->pxiPrev;
    --ilexXml;
    sprintf(sz, "</%s>", pxi->szTagCur);
    PrintXml(pstm, sz);
    free((PVOID)pxi->szTagCur);
    delete pxi;
}

/* Here's the list of disc regions */
static struct {
    int ilyr;
    LPCSTR szType;
    int idxr;
} rgdsc[] = {
#ifndef DVDEMU
    { 0, "LEAD-IN", idxrLead },
    { 0, "DATA", idxrData },
    { 0, "MIDDLE", idxrMiddle },
    { 0, "X-LEAD-IN", idxrXLead },
#endif
    { 0, "X-DATA", idxrXData },
#ifndef DVDEMU
    { 0, "X-MIDDLE", idxrXMiddle },
    { 1, "X-MIDDLE", idxrXMiddle },
#endif
    { 1, "X-DATA", idxrXData },
#ifndef DVDEMU
    { 1, "X-LEAD-OUT", idxrXLead },
    { 1, "MIDDLE", idxrMiddle },
    { 1, "DATA", idxrData },
    { 1, "LEAD-OUT", idxrLead },
#endif
};

static LPCSTR rgszPlaceholders[] = {
    "DVD-X2-PLACEHOLDER",
    "DVD-X2-MIRROR-REGION",
    "DVD-X2-GROOVE-REGION",
    "DVD-X2-VELOCITY-REGION",
};

#ifndef DVDEMU

BOOL CDX2MLFile::FWriteFile(LPCSTR sz)
{
    BOOL fRet;
    CDiskFile *pstm;
    
    pstm = new CDiskFile(sz, GENERIC_WRITE);
    if(pstm->FIsOpen())
        fRet = FWriteFile(pstm);
    else
        fRet = FALSE;
    delete pstm;
    return fRet;
}

#endif // DVDEMU

static int rgidxiStreams[] = {
    idxiControl,
    idxiXControl,
    2, 3, 4, 5
};

BOOL CDX2MLFile::FWriteFile(CFileStmSeq *pstm)
{
    int ilyr;
    int idxi;
    int iidxi;
    int ipsn;
    int i;
    DXI *pdxi;
    char szT[1024];
    char szLenT[32];
    LPSTR szStreamType;
    BOOL fFakeName;

    if(!m_pxml)
        return FALSE;

    ilexXml = 0;
    PrintXml(pstm, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    StartElem(pstm, "dx2ml",
#ifdef DVDEMU
        NULL
#else
        "xmlns=\"x-schema:Dx2ml.xdr\""
#endif
        );

    /* Input streams */
    StartElem(pstm, "input", NULL);
    for(ilyr = 0; ilyr < 2; ++ilyr) {
        sprintf(szT, "id=\"Tape%d\"", ilyr + 1);
        StartElem(pstm, "media", szT);
        for(iidxi = 0; iidxi < idxiMax; ++iidxi) {
            idxi = rgidxiStreams[iidxi];
            pdxi = &m_pxml->rgdxi[idxi];
            /* Skip those files not on this layer */
            if(ilyr == 0 && !pdxi->fUsedLyr0)
                continue;
            if(ilyr == 1 && !pdxi->fUsedLyr1)
                continue;
            switch(idxi) {
            case idxiControl:
                szStreamType = "leadin-stream";
                fFakeName = ilyr == 1;
                break;
            case idxiXControl:
                szStreamType = "xbox-leadout-stream";
                fFakeName = ilyr == 0;
                break;
            default:
                szStreamType = "input-stream";
                fFakeName = FALSE;
                break;
            }
            if(m_pxml->fLayoutTape)
                fFakeName = FALSE;

            if(pdxi->dwImageSize)
                sprintf(szLenT, " length=\"%d\"", pdxi->dwImageSize);
            else
                szLenT[0] = 0;
            sprintf(szT, "<%s id=\"%s%s\" file-name=\"%s\"%s%s/>",
                szStreamType, pdxi->szRefName, fFakeName ? "_alt" : "",
                pdxi->szNameOnTape, szLenT, pdxi->f2054 ?
                " storage-mode=\"DATA2054\"" : "");
            PrintXml(pstm, szT);
        }
        EndElem(pstm);
        if(m_pxml->fLayoutTape)
            /* Only one medium */
            break;
    }
    EndElem(pstm);

    /* Disc data */
    StartElem(pstm, "disc", "type=\"DVD-X2\"");
    StartElem(pstm, "side", NULL);
    for(ilyr = 0; ilyr < 2; ++ilyr) {
        sprintf(szT, "id=\"Layer%d\"", ilyr);
        if(ilyr)
            strcat(szT, " direction=\"OUTER-TO-INNER\"");
        StartElem(pstm, "layer", szT);
        sprintf(szT, "start-address=\"%06X\"", m_pxml->rgdxl[ilyr].psnStart);
        StartElem(pstm, "main-band", szT);
        
        /* Loop over all of the disc regions and print out the appropriate
         * data for each */
        for(i = 0; i < (sizeof rgdsc / sizeof rgdsc[0]); ++i) {
            if(rgdsc[i].ilyr == ilyr) {
                DXR *pdxr = &m_pxml->rgdxl[ilyr].rgdxrAreas[rgdsc[i].idxr];
                sprintf(szT, "type=\"%s\"", rgdsc[i].szType);
                StartElem(pstm, "region", szT);
                idxi = pdxr->idxiImage;
                switch(rgdsc[i].idxr) {
                case idxrLead:
                    if(ilyr)
                        /* Layer 1 is leadout */
                        WriteXLeadOut(pstm);
                    else
                        WriteLeadIn(pstm);
                    break;
                case idxrXData:
                    /* We have to print out all of the placeholder and XBE
                     * image data we have accumulated */
                    if(idxi >= 0 && m_pxml->rgdxi[idxi].szRefName) {
                        WriteXData(pstm, ilyr);
                        break;
                    }
                    // else write an empty stream
                default:
                    if(idxi >= 0 && m_pxml->rgdxi[idxi].szRefName) {
                        sprintf(szT, "<input-stream-ref stream-id=\"%s\"/>",
                            m_pxml->rgdxi[idxi].szRefName);
                    } else
                        sprintf(szT, "<zero-stream length-used=\"%d\"/>",
                            m_pxml->rgdxl[ilyr].rgdxrAreas[rgdsc[i].idxr].cpsnSize);
                    PrintXml(pstm, szT);
                    break;
                }
                EndElem(pstm);
            }
        }
        /* Close the band */
        EndElem(pstm);
        
        /* Close the layer */
        EndElem(pstm);
    }
    EndElem(pstm);
    EndElem(pstm);

#ifndef DVDEMU
    /* Identify the layer */
    char szBufT[256];
    if(m_pxml->szMasteringRef)
        sprintf(szBufT, " master-id=\"%s\"", m_pxml->szMasteringRef);
    else
        szBufT[0] = 0;
    sprintf(szT, "<mastering-job media-ref=\"Tape%d\" layer-ref=\"Layer%d\"%s/>",
        m_pxml->iLayer+1, m_pxml->iLayer, szBufT);
    PrintXml(pstm, szT);
#endif
    EndElem(pstm);

    return TRUE;
}

void CDX2MLFile::WriteLeadIn(CFileStmSeq *pstm)
{
    char szT[1024];
    DXL *pdxl = &m_pxml->rgdxl[0];
    DXR *pdxr = &pdxl->rgdxrAreas[idxrLead];

    /* First write out padding up to the reference stream */
    if(pdxl->psnStart < pdxr->psnStart) {
        sprintf(szT, "<zero-stream length-used=\"%d\"/>", pdxr->psnStart -
            pdxl->psnStart);
        PrintXml(pstm, szT);
    }
    /* Then comes the reference stream */
    PrintXml(pstm, "<reference-code-stream/>");
    /* Now pad up to the control stream */
    if(pdxl->ipsnControlStart > 32) {
        sprintf(szT, "<zero-stream length-used=\"%d\"/>",
            pdxl->ipsnControlStart - 32);
        PrintXml(pstm, szT);
    }
    /* Here's the leadin stream */
    sprintf(szT, "<leadin-stream-ref stream-id=\"%s\"/>",
        m_pxml->rgdxi[idxiControl].szRefName);
    PrintXml(pstm, szT);
    /* Pad out this region */
    if((pdxl->ipsnControlStart + 16 * 192) < pdxr->cpsnSize) {
        sprintf(szT, "<zero-stream length-used=\"%d\"/>", pdxr->cpsnSize -
            (pdxl->ipsnControlStart + 16 * 192));
        PrintXml(pstm, szT);
    }
}

void CDX2MLFile::WriteXLeadOut(CFileStmSeq *pstm)
{
    char szT[1024];
    DXL *pdxl = &m_pxml->rgdxl[1];
    DXR *pdxr = &pdxl->rgdxrAreas[idxrLead];

    /* Write the appropriate amount of blank space */
    if(pdxl->ipsnControlStart) {
        sprintf(szT, "<zero-stream length-used=\"%d\"/>",
            pdxl->ipsnControlStart);
        PrintXml(pstm, szT);
    }
    /* Write out the leadout stream */
    sprintf(szT,  "<xbox-leadout-stream-ref stream-id=\"%s\"/>",
        m_pxml->rgdxi[idxiXControl].szRefName);
    PrintXml(pstm, szT);
    /* Write out the remaining blank space */
    if(pdxl->ipsnControlStart + 32 * 80 < pdxr->cpsnSize) {
        sprintf(szT, "<zero-stream length-used=\"%d\"/>", pdxr->cpsnSize -
            (pdxl->ipsnControlStart + 32 * 80));
        PrintXml(pstm, szT);
    }
}

void CDX2MLFile::WriteXData(CFileStmSeq *pstm, int ilyr)
{
    DXL *pdxl = &m_pxml->rgdxl[ilyr];
    DXR *pdxr = &pdxl->rgdxrAreas[idxrXData];
    DXI *pdxi = &m_pxml->rgdxi[pdxr->idxiImage];
    int i = 0;
    int ixbi = 0;
    DWORD psnCur = 0;
    DWORD psnMax = pdxr->cpsnSize;
    BOOL fXbiNext;
    LPCSTR szType;
    DWORD dwSize;
    DWORD dwStart;
    char szT[1024];

    while(i < pdxl->cxph || ixbi < pdxl->ixbiMac) {
        if(ixbi == pdxl->ixbiMac)
            fXbiNext = FALSE;
        else if(i == pdxl->cxph)
            fXbiNext = TRUE;
        else
            fXbiNext = pdxl->pxbi[ixbi].lsnStart < pdxl->rgxph[i].lsnStart;
        if(fXbiNext) {
            dwStart = pdxl->pxbi[ixbi].lsnStart;
            dwSize = pdxl->pxbi[ixbi++].cpsnSize;
            szType = "XBOX-EXECUTABLE";
        } else {
            dwStart = pdxl->rgxph[i].lsnStart;
            dwSize = 4096;
            szType = rgszPlaceholders[pdxl->rgxph[i++].ph];
        }
        /* If we've somehow gotten an overlap, we have to skip this */
        if(psnCur > dwStart)
            continue;
        /* Print out the null content to the next point */
        if(psnCur < dwStart) {
            sprintf(szT, "<input-stream-ref stream-id=\"%s\" "
                "length-used=\"%d\"/>", pdxi->szRefName, dwStart - psnCur);
            PrintXml(pstm, szT);
            psnCur = dwStart;
        }
        /* Print out the current content */
        sprintf(szT, "<input-stream-ref stream-id=\"%s\" "
            "length-used=\"%d\" content=\"%s\"/>", pdxi->szRefName,
            dwSize, szType);
        PrintXml(pstm, szT);
        psnCur += dwSize;
    }

    /* If we have room to go, then print out the tail */
    if(psnCur < psnMax) {
        sprintf(szT, "<input-stream-ref stream-id=\"%s\" length-used=\"%d\"/>", pdxi->szRefName,
            psnMax - psnCur);
        PrintXml(pstm, szT);
    }
}

#ifndef DVDEMU

LPSTR SzCopyWz(LPCWSTR wz)
{
    LPSTR sz;
    int cch;
    
    cch = WideCharToMultiByte(CP_ACP, 0, wz, -1, NULL, 0, NULL, NULL);
    if(cch > 0) {
        sz = new char[cch + 1];
        WideCharToMultiByte(CP_ACP, 0, wz, -1, sz, cch + 1, NULL, NULL);
        sz[cch] = 0;
    }
    return sz;
}

BOOL FEqualSzWz(LPCSTR sz, LPCWSTR wz, BOOL fIgnCase)
{
    for(; *sz; ++sz, ++wz) {
        if(fIgnCase) {
            if(isupper(*sz)) {
                if(*sz != (isupper(*wz) ? *wz : toupper(*wz)))
                    return FALSE;
                continue;
            } else if(islower(*sz)) { 
                if(*sz != (islower(*wz) ? *wz : tolower(*wz)))
                    return FALSE;
                continue;
            }
        }
        if(*sz != *wz)
            return FALSE;
    }
    return !*wz;
}

/* Define a COM ptr class helper that will release upon requery */
template <class T> class CMyComPtr : public CComPtr<T>
{
public:
    T** operator&()
    {
        if(p != NULL) {
            p->Release();
            p = NULL;
        }
        return &p;
    }
};

static BOOL FGetAttribute(IXMLDOMNode *pxn, LPCOLESTR wz, CComVariant *pvar)
{
    CMyComPtr<IXMLDOMNamedNodeMap> pxmap;
    CMyComPtr<IXMLDOMNode> pxnAttr;

    if(S_OK != pxn->get_attributes(&pxmap))
        return FALSE;
    if(S_OK != pxmap->getNamedItem(CComBSTR(wz), &pxnAttr))
        return FALSE;
    pvar->Clear();
    return S_OK == pxnAttr->get_nodeValue(pvar);
}

BOOL CDX2MLFile::FReadFile(LPCSTR sz, BOOL fRequireFull)
{
    LPOLESTR wz;
    int cwch;
    HRESULT hr;
    VARIANT_BOOL f;
    CMyComPtr<IXMLDOMNodeList> pxnlLyrList;
    CMyComPtr<IXMLDOMNode> pxn;
    CComVariant var;
    int ilyr;
    int idxr;

    /* Dump any document we might have */
    if(m_pxml) {
        delete m_pxml;
        m_pxml = NULL;
    }

    /* Make sure we've got COM going */
    if(!fDidCoInit) {
        if(FAILED(CoInitialize(NULL)))
            return FALSE;
        fDidCoInit = TRUE;
    }

    /* Make sure we've got a DOMDocument */
    if(!m_pxdoc) {
        CLSID clsid;

        if(FAILED(CLSIDFromProgID(L"Msxml.DOMDocument", &clsid)))
            return FALSE;
        if(FAILED(CoCreateInstance(clsid, NULL, CLSCTX_ALL,
                IID_IXMLDOMDocument, (void **)&m_pxdoc)))
            return FALSE;
    }

    /* Set document parameters */
    m_pxdoc->put_async(false);
    m_pxdoc->put_validateOnParse(!!fRequireFull);

    /* Load the file */
    cwch = 1 + strlen(sz);
    wz = (LPOLESTR)_alloca(cwch * sizeof(WCHAR));
    if(!MultiByteToWideChar(CP_ACP, 0, sz, -1, wz, cwch))
        return FALSE;
    hr = m_pxdoc->load(CComVariant(wz), &f);
    if(hr != S_OK || !f) {
#ifdef SHOWERRS
        CMyComPtr<IXMLDOMParseError> ppe;
        if(S_OK == m_pxdoc->get_parseError(&ppe)) {
            CComBSTR bst;
            if(S_OK == ppe->get_reason(&bst))
                fwprintf(stderr, L"XML parse error: %s\n", (LPOLESTR)bst);
        }
#endif
        return FALSE;
    }
    m_pxml = new DXML;
    memset(m_pxml, 0, sizeof *m_pxml);

    /* We've got a document, now pick apart the pieces.  First get the input
     * streams */
    if(S_OK == m_pxdoc->selectNodes(CComBSTR(L"/dx2ml/input/media"),
        &pxnlLyrList))
    {
        pxnlLyrList->reset();
        ilyr = 0;
        while(S_OK == pxnlLyrList->nextNode(&pxn) && ilyr < 2) {
            /* Figure out what layer we're looking at */
            if(!FGetAttribute(pxn, L"id", &var))
                continue;
            if(var.vt != VT_BSTR)
                continue;
            ReadInputStreams(ilyr, pxn);
            ++ilyr;
        }
    }

    /* Now read the disc regions */
    if(S_OK == m_pxdoc->selectNodes(CComBSTR(L"/dx2ml/disc/side/layer"),
        &pxnlLyrList))
    {
        pxnlLyrList->reset();
        while(S_OK == pxnlLyrList->nextNode(&pxn)) {
            /* Figure out what layer we're looking at */
            if(!FGetAttribute(pxn, L"id", &var))
                continue;
            if(var.vt != VT_BSTR)
                continue;
            if(0 == _wcsicmp(var.bstrVal, L"Layer0"))
                ilyr = 0;
            else if(0 == _wcsicmp(var.bstrVal, L"Layer1"))
                ilyr = 1;
            else
                continue;
            ReadDiscRegions(ilyr, pxn);
        }
    }

    /* Get the mastering job layer ID and mastering reference */
    m_pxml->iLayer = -1;
    if(S_OK == m_pxdoc->selectNodes(CComBSTR(L"/dx2ml/mastering-job"),
        &pxnlLyrList))
    {
        pxnlLyrList->reset();
        /* Only read the first one */
        if(S_OK == pxnlLyrList->nextNode(&pxn)) {
            if(FGetAttribute(pxn, L"layer-ref", &var) && var.vt == VT_BSTR) {
                if(0 == _wcsicmp(var.bstrVal, L"Layer0"))
                    m_pxml->iLayer = 0;
                else if(0 == _wcsicmp(var.bstrVal, L"Layer1"))
                    m_pxml->iLayer = 1;
            } else if(FGetAttribute(pxn, L"master-id", &var) && var.vt ==
                    VT_BSTR)
                m_pxml->szMasteringRef = SzCopyWz(var.bstrVal);
        }
    }

    return TRUE;
}

void CDX2MLFile::ReadInputStreams(int ilyr, IXMLDOMNode *pxnIn)
{
    CMyComPtr<IXMLDOMNodeList> pxnl;
    CMyComPtr<IXMLDOMNode> pxn;
    CComBSTR bstrType;
    CComVariant varId;
    CComVariant var;
    int idxi;
    DXI *pdxi;

    if(S_OK == pxnIn->selectNodes(CComBSTR(L"*"), &pxnl)) {
        pxnl->reset();
        while(S_OK == pxnl->nextNode(&pxn)) {
            bstrType.Empty();
            if(S_OK != pxn->get_nodeName(&bstrType))
                continue;
            if(FEqualSzWz("leadin-stream", bstrType, FALSE)) {
                idxi = idxiControl;
                m_pxml->rgdxi[idxi].fUsedLyr0 = TRUE;
                m_pxml->rgdxi[idxi].fUsedLyr1 = TRUE;
            } else if(FEqualSzWz("xbox-leadout-stream", bstrType, FALSE)) {
                /* We only want this from layer 1, but if we're reading a
                 * single-layer XML, then we need to take it from layer 0.
                 * We'll just parse whatever is here and discard any previously
                 * read value */
                idxi = idxiXControl;
                if(m_pxml->rgdxi[idxi].szRefName) {
                    free(m_pxml->rgdxi[idxi].szRefName);
                    m_pxml->rgdxi[idxi].szRefName = NULL;
                    if(m_pxml->rgdxi[idxi].szNameOnTape)
                        free(m_pxml->rgdxi[idxi].szNameOnTape);
                }
                m_pxml->rgdxi[idxi].fUsedLyr0 = TRUE;
                m_pxml->rgdxi[idxi].fUsedLyr1 = TRUE;
            } else if(FEqualSzWz("input-stream", bstrType, FALSE)) {
                for(idxi = 2; idxi < idxiMax && m_pxml->rgdxi[idxi].szRefName;
                    ++idxi);
                if(idxi == idxiMax)
                    /* Too much data, ignore */
                    continue;
            } else
                continue;
            if(!FGetAttribute(pxn, L"id", &varId) || varId.vt != VT_BSTR)
                continue;
            pdxi = &m_pxml->rgdxi[idxi];
            if(pdxi->szRefName)
                /* A duplicate somehow */
                continue;
            pdxi->szRefName = SzCopyWz(varId.bstrVal);
            if(FGetAttribute(pxn, L"file-name", &var) && var.vt == VT_BSTR)
                pdxi->szNameOnTape = SzCopyWz(var.bstrVal);
            if(FGetAttribute(pxn, L"length", &var) && var.vt == VT_BSTR)
                swscanf(var.bstrVal, L"%d", &pdxi->dwImageSize);
            if(FGetAttribute(pxn, L"storage-mode", &var) && var.vt == VT_BSTR &&
                    FEqualSzWz("DATA2054", var.bstrVal, FALSE))
                pdxi->f2054 = TRUE;
        }
    }
}

void CDX2MLFile::ReadDiscRegions(int ilyr, IXMLDOMNode *pxnIn)
{
    CMyComPtr<IXMLDOMNodeList> pxnlRgns, pxnlData;
    CMyComPtr<IXMLDOMNode> pxn, pxnData;
    CComVariant var;
    CComBSTR bstrName;
    int idxi;
    int i;
    int iMax = sizeof rgdsc / sizeof rgdsc[0];
    DXL *pdxl = &m_pxml->rgdxl[ilyr];
    DXR *pdxr;
    BOOL fGotStream;
    BOOL fWantSize;
    BOOL fGotSize;
    DWORD dwSize;
    DWORD psnCur;

    /* First mark all of the regions as absent */
    for(i = 0; i < idxrMax; ++i)
        pdxl->rgdxrAreas[i].idxiImage = -1;

    /* Find the start PSN for this side */
    if(S_OK != pxnIn->selectSingleNode(CComBSTR(L"main-band"), &pxn) ||
            !FGetAttribute(pxn, L"start-address", &var) ||
            !swscanf(var.bstrVal, L"%x", &pdxl->psnStart))
        pdxl->psnStart = 0;
    psnCur = pdxl->psnStart;

    /* Scan the region list */
    if(S_OK == pxnIn->selectNodes(CComBSTR(L"main-band/region"), &pxnlRgns)) {
        pxnlRgns->reset();
        while(S_OK == pxnlRgns->nextNode(&pxn)) {
            /* Get the region type */
            if(!FGetAttribute(pxn, L"type", &var) || var.vt != VT_BSTR)
                continue;

            /* See whether we can figure out which area we're talking about */
            for(i = 0; i < iMax; ++i) {
                if(rgdsc[i].ilyr == ilyr && FEqualSzWz(rgdsc[i].szType,
                        var.bstrVal, FALSE))
                    break;
            }
            if(i == iMax)
                continue;

            pdxr = &pdxl->rgdxrAreas[rgdsc[i].idxr];
            pdxr->psnStart = psnCur;
            pdxr->cpsnSize = 0;
            pdxr->idxiImage = -1;
            if(rgdsc[i].idxr == idxrLead)
                /* In case we don't see any good data, make it up */
                pdxl->ipsnControlStart = ilyr ? 0xFD0200 - psnCur : 512;

            /* Figure out what's in this section */
            if(S_OK == pxn->selectNodes(CComBSTR("*"), &pxnlData)) {
                pxnlData->reset();
                while(S_OK == pxnlData->nextNode(&pxnData)) {
                    bstrName.Empty();
                    if(S_OK != pxnData->get_nodeName(&bstrName))
                        continue;
                    fGotStream = FALSE;
                    fWantSize = FALSE;
                    dwSize = 0;
                    if(FEqualSzWz("zero-stream", bstrName, FALSE))
                        fWantSize = TRUE;
                    else if(FEqualSzWz("leadin-stream-ref", bstrName, FALSE) &&
                        ilyr == 0)
                    {
                        fGotStream = TRUE;
                        dwSize = 16 * 192;
                        /* Remember this starting block */
                        if(rgdsc[i].idxr == idxrLead && ilyr == 0) {
                            pdxl->ipsnControlStart = psnCur - pdxr->psnStart;
                        }
                    } else if(FEqualSzWz("xbox-leadout-stream-ref", bstrName,
                        FALSE) && ilyr == 1)
                    {
                        fGotStream = TRUE;
                        dwSize = 32 * 80;
                        if(rgdsc[i].idxr == idxrLead && ilyr == 1)
                            pdxl->ipsnControlStart = psnCur - pdxr->psnStart;
                    } else if(FEqualSzWz("reference-code-stream", bstrName,
                        FALSE) && ilyr == 0)
                    {
                        dwSize = 16 * 2;
                        /* This is the real start of the leadin area */
                        if(rgdsc[i].idxr == idxrLead && ilyr == 0) {
                            pdxr->psnStart = psnCur;
                            pdxr->cpsnSize = 0;
                        }
                    } else if(FEqualSzWz("input-stream-ref", bstrName, FALSE)) {
                        fGotStream = fWantSize = TRUE;
                    }
                    /* If we've got a size to find, get the data */
                    if(fWantSize && FGetAttribute(pxnData, L"length-used",
                            &var) && var.vt == VT_BSTR)
                        fGotSize = 1 == swscanf(var.bstrVal, L"%lu", &dwSize);
                    else
                        fGotSize = 0;
                    /* If we've got a stream name, find it so we can hook up
                     * to the appropriate input stream */
                    if(fGotStream && FGetAttribute(pxnData, L"stream-id",
                        &var) && var.vt == VT_BSTR)
                    {
                        /* We're only going to look up the image name if
                         * we haven't seen one already for this region.
                         * TODO: error out */
                        if(pdxr->idxiImage < 0) {
                            for(idxi = 0; idxi < idxiMax; ++idxi) {
                                if(m_pxml->rgdxi[idxi].szRefName &&
                                    FEqualSzWz(m_pxml->rgdxi[idxi].szRefName,
                                    var.bstrVal, FALSE))
                                {
                                    pdxr->idxiImage = idxi;
                                    if(ilyr == 0)
                                        m_pxml->rgdxi[idxi].fUsedLyr0 = TRUE;
                                    else
                                        m_pxml->rgdxi[idxi].fUsedLyr1 = TRUE;
                                    break;
                                }
                            }
                        }
                    }

                    /* If we didn't get a size, but we have an input stream,
                     * we'll take its size */
                    if(fWantSize && !fGotSize && pdxr->idxiImage >= 0)
                        dwSize = m_pxml->rgdxi[pdxr->idxiImage].dwImageSize;

                    /* If this is Xbox image data, we need to track this as a
                     * placeholder, XBE, etc. */
                    if(rgdsc[i].idxr == idxrXData && FGetAttribute(pxnData,
                        L"content", &var) && var.vt == VT_BSTR)
                    {
                        if(FEqualSzWz("XBOX-EXECUTABLE", var.bstrVal,
                            FALSE))
                        {
                            if(!pdxl->pxbi || pdxl->ixbiMax == pdxl->ixbiMac) {
                                XBI *pxbiNew = new XBI[pdxl->ixbiMax += 32];
                                if(pdxl->pxbi) {
                                    memcpy(pxbiNew, pdxl->pxbi,
                                        sizeof *pxbiNew * pdxl->ixbiMac);
                                    delete pdxl->pxbi;
                                }
                                pdxl->pxbi = pxbiNew;
                            }
                            pdxl->pxbi[pdxl->ixbiMac].lsnStart = pdxr->cpsnSize;
                            pdxl->pxbi[pdxl->ixbiMac++].cpsnSize = dwSize;
                        } else if(dwSize >= 4096 && pdxl->cxph < 8) {
                            /* Placeholders have to be at least 4096 blocks.
                             * If we find something smaller, ignore it; if we
                             * find something bigger, we roll the excess into
                             * the next block */
                            pdxl->rgxph[pdxl->cxph].lsnStart =
                                pdxr->cpsnSize;
                            for(int ph = 0; ph < XPH::phMax; ++ph) {
                                if(FEqualSzWz(rgszPlaceholders[ph],
                                        var.bstrVal, FALSE))
                                    break;
                            }
                            if(ph < XPH::phMax)
                                pdxl->rgxph[pdxl->cxph++].ph = ph;
                        }
                    }

                    /* Account for the size */
                    pdxr->cpsnSize += dwSize;
                    psnCur += dwSize;
                }
            }
        }
    }

    /* Now place all of the empty regions */
    i = ilyr ? idxrMax-1 : 0;
    pdxr = &pdxl->rgdxrAreas[i];
    psnCur = pdxl->psnStart;
    if(pdxr->cpsnSize == 0 && pdxr->psnStart != 0)
        psnCur = pdxr->psnStart;
    for(;;) {
        pdxr = &pdxl->rgdxrAreas[i];
        if(pdxr->cpsnSize == 0)
            pdxr->psnStart = psnCur;
        psnCur = pdxr->psnStart + pdxr->cpsnSize;
        if(ilyr) {
            if(--i < 0)
                break;
        } else {
            if(++i == idxrMax)
                break;
        }
    }
}

#endif // DVDEMU

BOOL CDX2MLFile::FValidateForm(void) const
{
    DWORD psnCur;
    int ilyr;
    int idxr;
    DXR *pdxr;
    DXL *pdxl;

    if(!m_pxml)
        return FALSE;

    /* Make sure that the layers line up and that those sections that were
     * specified show up in the right order */
    for(idxr = ilyr = 0; ilyr < 2; ++ilyr, --idxr) {
        pdxl = &m_pxml->rgdxl[ilyr];
        pdxr = &pdxl->rgdxrAreas[idxr];
        psnCur = pdxl->psnStart;
        /* On layer 0, the psn of the first area is allowed to be different
         * from the start psn of the layer, but only if it specifies lead
         * spacing */
        if(ilyr == 0) {
            if(pdxr->psnStart < psnCur)
                return FALSE;
            psnCur = pdxr->psnStart;
        }
        for(;;) {
            if(pdxr->psnStart != psnCur)
                return FALSE;
            if(pdxr->idxiImage >= 2 && pdxr->cpsnSize !=
                    m_pxml->rgdxi[pdxr->idxiImage].dwImageSize)
                return FALSE;
            psnCur += pdxr->cpsnSize;
            if(ilyr == 0) {
                ++pdxr;
                if(++idxr == idxrMax)
                    break;
            } else {
                --pdxr;
                if(--idxr < 0)
                    break;
            }
        }
        /* The last psn must equal the first psn of the opposite side */
        if(0x1000000 - psnCur != m_pxml->rgdxl[1 - ilyr].psnStart)
            return FALSE;
    }

    return TRUE;
}

BOOL CDX2MLFile::FValidateContent(void) const
{
    if(!FValidateForm())
        return FALSE;

    /* There are some things we don't validate here: that the image and ximage
     * data streams match up layer-to-layer and match the control data.  That's
     * done when we validate the control data */

    return
        /* Make sure the images occur in the right order */
        m_pxml->rgdxl[0].dxrLead.idxiImage == idxiControl &&
        m_pxml->rgdxl[0].dxrData.idxiImage == 2 &&
        m_pxml->rgdxl[0].dxrXData.idxiImage == 3 &&
        m_pxml->rgdxl[1].dxrXData.idxiImage == 4 &&
        m_pxml->rgdxl[1].dxrData.idxiImage == 5 &&
        m_pxml->rgdxl[1].dxrLead.idxiImage == idxiXControl &&

        /* Make sure the ECMA Control is in the right place */
        m_pxml->rgdxl[0].psnStart <= 0x020000 &&
        m_pxml->rgdxl[0].dxrLead.psnStart == 0x02F000 &&
        m_pxml->rgdxl[0].ipsnControlStart == 0x02F200 -
            m_pxml->rgdxl[0].dxrLead.psnStart &&

        /* Make sure the layer 0 middle area is big enough */
        m_pxml->rgdxl[0].dxrMiddle.psnStart <= 0x050E00 &&

        /* Make sure the layer 0 XLeadin is the right size */
        m_pxml->rgdxl[0].dxrXLead.psnStart == 0x05B200 &&
        m_pxml->rgdxl[0].dxrXLead.cpsnSize >= 0x5400 &&

        /* Make sure the layer 0 XMiddle is the right size */
        m_pxml->rgdxl[0].dxrXMiddle.psnStart <= 0x222F40 &&

        /* Make sure the layer 1 XMiddle is the right size */
        m_pxml->rgdxl[1].psnStart == 0xDC8860 &&

        /* Make sure the layer 1 XLeadout is the right size */
        m_pxml->rgdxl[1].dxrXLead.psnStart <= 0xF9FA00 &&

        /* Make sure the layer 1 middle area is the right size */
        m_pxml->rgdxl[1].dxrMiddle.psnStart == 0xFA4E00 &&
        m_pxml->rgdxl[1].dxrMiddle.cpsnSize >= 0xA400 &&

        /* Make sure the XControl is in the right place */
        m_pxml->rgdxl[1].dxrLead.psnStart <= 0xFD0000 &&
        m_pxml->rgdxl[1].ipsnControlStart == 0xFD0200 -
            m_pxml->rgdxl[1].dxrLead.psnStart;
}

BOOL CDX2MLFile::FValidatePlaceholderLocations(BOOL fFixup, BOOL fAPOnly,
    BOOL fPrintResults)
{
    int i;
    int iPrev;
    int ipsn;
    int ipsnMac;
    DWORD psn;
    DWORD dpsn;
    struct {
        DWORD psn;
        WORD ilyr;
        WORD ixph;
    } rgnxph[16];

    if(!m_pxml)
        return FALSE;

    /* Copy the layer 0 placeholders.  Make sure they are sufficiently
     * separated */
    ipsnMac = 0;
    for(i = 0; i < m_pxml->rgdxl[0].cxph; ++i) {
        /* All placeholders have to be on even boundaries, so check that
         * first */
        if(m_pxml->rgdxl[0].rgxph[i].lsnStart & 1) {
            if(fPrintResults) {
                fprintf(stderr, "error: layer 0 PH %d is not on an even sector\n",
                    i);
            }
            return FALSE;
        }
        if(fAPOnly && m_pxml->rgdxl[0].rgxph[i].ph == XPH::phNone)
            continue;
        rgnxph[ipsnMac].psn = m_pxml->rgdxl[0].rgxph[i].lsnStart +
            m_pxml->rgdxl[0].dxrXData.psnStart;
        rgnxph[ipsnMac].ilyr = 0;
        rgnxph[ipsnMac].ixph = (WORD)i;
        ++ipsnMac;
    }

    /* Copy the layer 1 placeholders.  We convert each placholder's layer 1 end
     * PSN into the matching layer 0 PSN and insert it into the sorted array */
    for(i = 0; i < m_pxml->rgdxl[1].cxph; ++i) {
        /* All placeholders have to be on even boundaries, so check that
         * first */
        if((m_pxml->rgdxl[0].dxrXData.cpsnSize +
            m_pxml->rgdxl[1].rgxph[i].lsnStart) & 1)
        {
            if(fPrintResults) {
                fprintf(stderr, "error: layer 1 PH %d is not on an even sector\n",
                    i);
            }
            return FALSE;
        }
        if(fAPOnly && m_pxml->rgdxl[1].rgxph[i].ph == XPH::phNone)
            continue;
        psn = (m_pxml->rgdxl[1].rgxph[i].lsnStart +
            m_pxml->rgdxl[1].dxrXData.psnStart + 0xFFF) ^ 0xFFFFFF;
        for(ipsn = 0; ipsn < ipsnMac; ++ipsn) {
            if(psn < rgnxph[ipsn].psn)
                break;
        }
        if(ipsn < ipsnMac) {
            memmove(&rgnxph[ipsn+1], &rgnxph[ipsn], (ipsnMac - ipsn) *
                sizeof(rgnxph[0]));
        }
        rgnxph[ipsn].psn = psn;
        rgnxph[ipsn].ilyr = 1;
        rgnxph[ipsn].ixph = (WORD)i;
        ++ipsnMac;
    }

    /* Make sure no placeholder is too close to the x-leadin */
    /* TODO: this should be 2mm, not a fixed number of sectors */
    for(i = 0; i < ipsnMac && rgnxph[i].psn < 0x6B000 || rgnxph[i].psn -
        m_pxml->rgdxl[0].dxrXData.psnStart < 0x15F00; ++i)
    {
        if(fPrintResults)
            fprintf(stderr, "%s: layer %d PH %d is too close to leadin\n",
                fFixup ? "warning" : "error", rgnxph[i].ilyr, rgnxph[i].ixph);
        if(!fFixup)
            return FALSE;
        m_pxml->rgdxl[rgnxph[i].ilyr].rgxph[rgnxph[i].ixph].fDisallow = TRUE;
    }
    i = ipsnMac;
    while(i-- && rgnxph[i].psn + 0x1000 > m_pxml->rgdxl[0].dxrXMiddle.psnStart -
        0x10000)
    {
        if(fPrintResults)
            fprintf(stderr, "%s: layer %d PH %d is too close to leadin\n",
                fFixup ? "warning" : "error", rgnxph[i].ilyr, rgnxph[i].ixph);
        if(!fFixup)
            return FALSE;
        m_pxml->rgdxl[rgnxph[i].ilyr].rgxph[rgnxph[i].ixph].fDisallow = TRUE;
    }
    iPrev = ipsnMac - 1;
    for(i = iPrev - 1; i >= 0; --i) {
        dpsn = rgnxph[i].ilyr == rgnxph[iPrev].ilyr ? 0x21000 : 0x11000;
        if(rgnxph[iPrev].psn - rgnxph[i].psn < dpsn) {
            if(fPrintResults) {
                fprintf(stderr, "%s: layer %d PH %d is too close to layer %d PH %d\n",
                    fFixup ? "warning" : "error", rgnxph[i].ilyr, rgnxph[i].ixph,
                    rgnxph[iPrev].ilyr, rgnxph[iPrev].ixph);
            }
            if(!fFixup)
                return FALSE;
            m_pxml->rgdxl[rgnxph[i].ilyr].rgxph[rgnxph[i].ixph].fDisallow = TRUE;
        } else
            iPrev = i;
    }

    return TRUE;
}

void CDX2MLFile::SetMasteringRef(LPCSTR sz)
{
    if(m_pxml) {
        if(m_pxml->szMasteringRef)
            delete m_pxml->szMasteringRef;
        if(sz)
            m_pxml->szMasteringRef = _strdup(sz);
        else
            m_pxml->szMasteringRef = NULL;
    }
}
