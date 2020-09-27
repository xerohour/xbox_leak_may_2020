/*
 *
 * xctrl.cpp
 *
 * CXControl
 *
 */

#include "precomp.h"

#define XCFIELD(field) (int)(&((CXControl*)0)->field)

/* XControl fields */
const CXControl::XCXC CXControl::g_rgxcxc[] = {
    { 5, XCFIELD(m_psnStart0), 3, TRUE },
    { 9, XCFIELD(m_psnEnd1), 3, TRUE },
    { 13, XCFIELD(m_psnEnd0), 3, TRUE },
    { 16, XCFIELD(m_fBCA), 1 },
    { 768, XCFIELD(m_hcrt), 2 },
    { 1055, XCFIELD(m_ftContent), 8 },
    { 1083, XCFIELD(m_guidContent), 16 },
    { 1183, XCFIELD(m_ftAuth), 8 },
    { 1210, XCFIELD(m_bAuthSys), 1 },
    { 1211, XCFIELD(m_guidAuth), 16 },
    { 1227, XCFIELD(m_rgbAuthHash), 20 },
    { 1247, XCFIELD(m_rgbAuthSig), 128 },
    { 1375, XCFIELD(m_rgbAuthSig)+128, 128 },
    // the DART has to be unscrambled, so we don't read it directly
};

/* XControl areas of required zero */
const CXControl::XCXC CXControl::g_rgxcz[] = {
    { 4, 0, 1 },
    { 8, 0, 1 },
    { 12, 0, 1 },
    { 17, 0, 258-17 },
    { 258, 0, 255 },
    { 513, 0, 255 },
    { 1023, 0, 32 },
    { 1063, 0, 20 },
    { 1099, 0, 84 },
    { 1191, 0, 19 },
    { 1503, 0, 1631-1503 },
    { 2047, 0, 1 }
};

#define ECFIELD(field) (int)(&((CECMAControl*)0)->field)

/* ECMAControl fields */
const CECMAControl::XCXC CECMAControl::g_rgxcxc[] = {
    { 5, ECFIELD(m_psnStart0), 3, TRUE },
    { 9, ECFIELD(m_psnEnd1), 3, TRUE },
    { 13, ECFIELD(m_psnEnd0), 3, TRUE },
    { 16, ECFIELD(m_fBCA), 1 },
};

/* ECMA areas of required zero */
const CECMAControl::XCXC CECMAControl::g_rgxcz[] = {
    { 17, 0, 15 },
    { 32, 0, 255 },
    { 287, 0, 255 },
    { 542, 0, 255 },
    { 797, 0, 255 },
    { 1052, 0, 255 },
    { 1307, 0, 255 },
    { 1562, 0, 255 },
    { 1817, 0, 231 },
};        

BOOL CControlStruct::FValidateZero(const BYTE *pb, const XCXC *rgxcz, int cxcz)
{
    BYTE b = 0;
    while(cxcz--) {
        for(int ib = 0; ib < rgxcz[cxcz].cbSize; ++ib)
            b |= pb[ib + rgxcz[cxcz].ibData];
    }
    return b == 0;
};

void CControlStruct::Import(const BYTE *pb, const XCXC *rgxcxc, int cxcxc)
{
    while(cxcxc--) {
        if(rgxcxc[cxcxc].fSwapBytes) {
            for(int ib = 0; ib < rgxcxc[cxcxc].cbSize; ++ib)
                ((PBYTE)this)[ib + rgxcxc[cxcxc].ibStruct] =
                    pb[rgxcxc[cxcxc].cbSize + rgxcxc[cxcxc].ibData - ib - 1];
        } else
            memcpy((PBYTE)this + rgxcxc[cxcxc].ibStruct,
                pb + rgxcxc[cxcxc].ibData, rgxcxc[cxcxc].cbSize);
    }
}

void CControlStruct::Export(BYTE *pb, const XCXC *rgxcxc, int cxcxc) const
{
    while(cxcxc--) {
        if(rgxcxc[cxcxc].fSwapBytes) {
            for(int ib = 0; ib < rgxcxc[cxcxc].cbSize; ++ib)
                pb[rgxcxc[cxcxc].cbSize + rgxcxc[cxcxc].ibData - ib - 1] =
                    ((const BYTE *)this)[ib + rgxcxc[cxcxc].ibStruct];
        } else
            memcpy(pb + rgxcxc[cxcxc].ibData, (PBYTE)this +
                rgxcxc[cxcxc].ibStruct, rgxcxc[cxcxc].cbSize);
    }
}

BOOL CXControl::FInit(CFileStmSeq *pstm, DWORD dwBlkSize)
{
    BYTE *pb;

    pb = (PBYTE)_alloca(dwBlkSize);
    if(pstm->CbRead(dwBlkSize, pb) != dwBlkSize)
        return FALSE;
    return FInit(pb, dwBlkSize);
}

BOOL CXControl::FInit(const BYTE *pb, DWORD dwBlkSize)
{
    BYTE b;
    int ib;
    CSHACrypt *pccSHA;

    /* Start clean */
    pccSHA = m_pccSHA;
    memset(this, 0, sizeof *this);
    m_pccSHA = pccSHA;

    if(dwBlkSize == 2054) {
        if(pb[0] != 0xd1 || pb[5] != 0xff)
            return FALSE;
        memcpy(&m_dwDAC, pb + 1, 4);
        dwBlkSize = 2048;
        pb += 6;
    } else
        m_dwDAC = 0;
    if(dwBlkSize != 2048)
        return FALSE;

    /* Make sure the constant data is right */
    if(pb[0] != 0xd1 || pb[1] != 0xf || pb[2] != 0x31 || pb[3] != 0x10)
        return 0;

    /* Ensure all areas of zero are actually zero */
    if(!FValidateZero(pb, g_rgxcz, sizeof g_rgxcz / sizeof g_rgxcz[0]))
        return FALSE;

    /* Extract the fields */
    Import(pb, g_rgxcxc, sizeof g_rgxcxc / sizeof g_rgxcxc[0]);

    /* Extract the DART, if we believe there's one present */
    if(m_dwDAC) {
        /* First extract and unscramble the index */
        for(ib = 0; ib < 207; ++ib)
            m_rgbDartIndex[ib] = pb[1840 + ib] ^ ((PBYTE)&m_dwDAC)[ib & 3];

        /* Now unscramble the DART */
        for(ib = 0; ib < 207; ++ib)
            ((PBYTE)&m_dart)[ib + 2] = pb[m_rgbDartIndex[ib] + 1633];
        m_dart.bVer = pb[1631];
        m_dart.cdare = pb[1632];

        /* And decrypt the HCRT */
        if(!m_pccSHA)
            m_pccSHA = new CSHACrypt;
        if(!m_pccSHA->FComputeSessionKeyFromData(pb + 1183, 44, 56) ||
                !m_pccSHA->FSessionDecrypt(pb + 770, sizeof
                m_hcrt.rghcre,(PBYTE)m_hcrt.rghcre))
            return FALSE;
    }

    return TRUE;
}

BOOL CXControl::FAssemble(BYTE *pb, BOOL fIncludeDart, BOOL fIncludeHcrt)
{
    int ib;

    /* Start clean */
    memset(pb, 0, 2048);

    /* Set the constant data */
    pb[0] = 0xd1;
    pb[1] = 0xf;
    pb[2] = 0x31;
    pb[3] = 0x10;

    /* Include the fields */
    Export(pb, g_rgxcxc, sizeof g_rgxcxc / sizeof g_rgxcxc[0]);

    if(fIncludeHcrt) {
        /* Encrypt the HCRT */
        if(!m_pccSHA)
            m_pccSHA = new CSHACrypt;
        if(!m_pccSHA->FComputeSessionKeyFromData(pb + 1183, 44, 56) ||
                !m_pccSHA->FSessionEncrypt((PBYTE)m_hcrt.rghcre, sizeof
                m_hcrt.rghcre, pb + 770))
            return FALSE;
    } else
        pb[768] = pb[769] = 0;

    if(fIncludeDart) {
        if(!m_dwDAC)
            /* If no auth code, we can't do anything */
            return FALSE;
        
        /* Scramble the DART */
        for(ib = 0; ib < 207; ++ib)
            pb[m_rgbDartIndex[ib] + 1633] = ((PBYTE)&m_dart)[ib + 2];
        pb[1631] = m_dart.bVer;
        pb[1632] = m_dart.cdare;

        /* And scramble the index */
        for(ib = 0; ib < 207; ++ib)
            pb[1840 + ib] = m_rgbDartIndex[ib] ^ ((PBYTE)&m_dwDAC)[ib & 3];
    }

    return TRUE;
}

BOOL CXControl::FWriteToStm(CFileStmSeq *pstm, DWORD dwBlkSize)
{
    BYTE rgb[2054];

    if(dwBlkSize == 2054) {
        if(!m_dwDAC || !m_fSigned)
            return FALSE;
        rgb[0] = 0xd1;
        rgb[5] = 0xff;
        memcpy(rgb + 1, &m_dwDAC, 4);
        return FAssemble(rgb + 6, TRUE, TRUE) && pstm->CbWrite(dwBlkSize, rgb) ==
            dwBlkSize;
    } else if(dwBlkSize == 2048)
        return FAssemble(rgb, FALSE, FALSE) && pstm->CbWrite(dwBlkSize, rgb) ==
            dwBlkSize;
    else
        return FALSE;
}

BOOL CXControl::FValidate(DXML *pxml, CCryptContainer *pcc)
{
    BYTE rgbHash[sizeof m_rgbAuthHash];
    BYTE rgbData[2048];

    /* If we have xml data, we make sure that the PSNs match */
    if(pxml) {
        if(m_psnStart0 != pxml->rgdxl[0].rgdxrAreas[idxrXData].psnStart)
            return FALSE;
        if(m_psnEnd0 != pxml->rgdxl[0].rgdxrAreas[idxrXData].psnStart +
                pxml->rgdxl[0].rgdxrAreas[idxrXData].cpsnSize - 1)
            return FALSE;
        if((m_psnEnd0 ^ 0xFFFFFF) !=
                pxml->rgdxl[1].rgdxrAreas[idxrXData].psnStart)
            return FALSE;
        if(m_psnEnd1 != pxml->rgdxl[1].rgdxrAreas[idxrXData].psnStart +
                pxml->rgdxl[1].rgdxrAreas[idxrXData].cpsnSize - 1)
            return FALSE;
    }

    /* If we have a signature container, make sure this is signed correctly */
    if(pcc) {
        /* Make sure we have an auth code */
        if(!m_dwDAC)
            return FALSE;

        if(pcc->CbGetDigestSize() != sizeof rgbHash)
            return FALSE;
        if(!FAssemble(rgbData, FALSE, TRUE))
            return FALSE;
        if(!pcc->FComputeDigest(rgbData, 1227, rgbHash))
            return FALSE;
        if(memcmp(rgbHash, m_rgbAuthHash, sizeof rgbHash))
            return FALSE;
        if(!pcc->FVerifyEncryptedDigest(rgbData, 1227, m_rgbAuthSig))
            return FALSE;
        m_fSigned = TRUE;
    }

    /* TODO: figure out whether there's anything else to authenticate */
    return TRUE;
}

static const BYTE mpaptph[] =
    { XPH::phNone, XPH::phMirror, XPH::phGroove, XPH::phVelocity };
static const BYTE mpaptchat[] = { 0xFF, 0x01, 0x02, 0x03 };
static const BYTE mpaptchal[] = { 0x02, 0x03, 0x03, 0x02 };
static const BYTE rgapt[16] =
    { 0, 0, 0, 0, 3, 2, 1, 3, 2, 1, 3, 2, 1, 3, 2, 1 };

BOOL CXControl::FSetupAP(DXML *pdxml)
{
    int i;
    int iT;
    int ilyr;
    int iapd;
    int iaptBase;
    DWORD psnBase;
    PBYTE pbPSN;
    DXL *pdxl;
    APD *papd;
    HCRE *phcre;
    DARE *pdare;
    CRandom rand;
    BYTE rgbBuf[2048];
    BYTE rgbIDs[23];
    BYTE rgbHcrtOrder[23];
    BYTE b;

    m_fSigned = FALSE;
    /* Stamp in a DAC */
    m_dwDAC = rand.Rand();

    /* Mark the HCRT and DART headers */
    m_hcrt.bVer = 1;
    m_dart.bVer = 1;
    m_hcrt.chcre = m_dart.cdare = 23;

    /* Set up the ID table */
    for(i = 0; i < 256; ++i)
        rgbBuf[i] = (BYTE)i;
    iT = 256;
    for(i = 23; i--; ) {
        b = (BYTE)(rand.Rand() % iT);
        rgbIDs[i] = rgbBuf[b];
        if(b != --iT)
            rgbBuf[b] = rgbBuf[iT];
    }

    /* Populate the AP descriptor table from the XML.  First count up the
     * disallowed regions and then change the types */
    iaptBase = 0;
    for(ilyr = 0; ilyr < 2; ++ilyr) {
        pdxl = &pdxml->rgdxl[ilyr];
        for(i = 0; i < pdxl->cxph; ++i) {
            if(pdxl->rgxph[i].fDisallow)
                ++iaptBase;
        }
    }
    for(ilyr = 0; ilyr < 2; ++ilyr) {
        pdxl = &pdxml->rgdxl[ilyr];
        psnBase = pdxl->rgdxrAreas[idxrXData].psnStart;
        for(i = 0; i < pdxl->cxph; ++i) {
            if(pdxl->rgxph[i].fDisallow)
                m_rgapd[ilyr][i].apt = 0;
            else 
                m_rgapd[ilyr][i].apt = rgapt[iaptBase + ((rand.Rand() >> 13) %
                    (16 - iaptBase))];
            pdxl->rgxph[i].ph = mpaptph[m_rgapd[ilyr][i].apt];
            m_rgapd[ilyr][i].psnStart = psnBase + pdxl->rgxph[i].lsnStart;
        }
        for(; i < 8; ++i) 
            m_rgapd[ilyr][i].psnStart = 0;
    }

    /* Set up the scrambling key for the HCRT and DART */
    for(i = 0; i < 23; ++i)
        rgbHcrtOrder[i] = (BYTE)i;

    papd = &m_rgapd[0][0];
    iapd = 0;
    pdare = &m_dart.rgdare[0];
    iT = 23;
    for(i = 0; i < 23; ++i, ++iapd, ++papd, ++pdare) {
        /* Pick an HCRT entry and DART entry at random */
        b = (BYTE)(rand.Rand() % iT);
        phcre = &m_hcrt.rghcre[rgbHcrtOrder[b]];
        if(b != --iT)
            rgbHcrtOrder[b] = rgbHcrtOrder[iT];

        /* Start with random data */
        rand.RandBytes((PBYTE)phcre, sizeof *phcre);
        rand.RandBytes((PBYTE)pdare, sizeof *pdare);

        /* Set the ID */
        phcre->bID = pdare->bID = rgbIDs[i];

        /* Find an AP entry that's not blank */
        while(iapd < 16 && papd->psnStart == 0)
            ++iapd, ++papd;

        /* The first 16 entries will be phase 2 challenges */
        if(iapd < 16) {
            /* Fill in the challenge type we computed earlier */
            phcre->bLevel = mpaptchal[papd->apt];
            pdare->bType = mpaptchat[papd->apt];
            phcre->bModifier = pdare->bModifier = (BYTE)(rand.Rand() & 3);

            /* Remember the challenge and response values we've calculated for
             * this entry */
            memcpy(papd->rgbChallenge, phcre->rgbChallenge, 4);
            memcpy(papd->rgbResponse, phcre->rgbResponse, 4);

            /* And mark the PSN of this entry */
            psnBase = papd->psnStart;
            pbPSN = (PBYTE)&psnBase;
            pdare->rgbUpper[0] = pbPSN[2];
            pdare->rgbUpper[1] = pbPSN[1];
            pdare->rgbUpper[2] = pbPSN[0];
            psnBase += 0xFFF;
            pdare->rgbLower[0] = pbPSN[2];
            pdare->rgbLower[1] = pbPSN[1];
            pdare->rgbLower[2] = pbPSN[0];
        } else if(i <= 16 || (rand.Rand() % 3)) {
            /* Generate a DAC challenge */
            phcre->bLevel = 1;
            pdare->bType = 0;
            phcre->bModifier = pdare->bModifier = 0;
            memcpy(phcre->rgbChallenge, &m_dwDAC, 4);
            pdare->rgbUpper[1] = phcre->rgbResponse[0];
            pdare->rgbUpper[2] = phcre->rgbResponse[1];
            pdare->rgbLower[1] = phcre->rgbResponse[2];
            pdare->rgbLower[2] = phcre->rgbResponse[3];
        } else {
            /* Unused */
            phcre->bLevel = 0xF0 + (BYTE)(rand.Rand() % 16);
            pdare->bType = 0xF0 + (BYTE)(rand.Rand() % 15);
        }
    }

    /* Generate the DART index key */
    for(i = 0; i < 207; ++i)
        m_rgbDartIndex[i] = (BYTE)i;
    while(i--) {
        iT = rand.Rand() % (i + 1);
        if(iT != i) {
            b = m_rgbDartIndex[i];
            m_rgbDartIndex[i] = m_rgbDartIndex[iT];
            m_rgbDartIndex[iT] = b;
        }
    }

    /* Stamp the authoring data */
    GetSystemTimeAsFileTime(&m_ftAuth);
    m_bAuthSys = 1;
    g_rand.RandBytes((PBYTE)&m_guidAuth, sizeof m_guidAuth);

    return TRUE;
#if 0
fatal:
    /* In case of failure, erase our work so we don't accidentally write out a
     * half-baked version */
    m_dwDAC = 0;
    memset(&m_hcrt, 0, sizeof m_hcrt);
    memset(&m_dart, 0, sizeof m_dart);
    memset(m_rgbDartIndex, 0, sizeof m_rgbDartIndex);
    return FALSE;
#endif
}

BOOL CXControl::FWriteAPData(int ilyr, int ixph, CTapeDevice *ptapDst)
{
    APD *papd = &m_rgapd[ilyr][ixph];
    BYTE rgb[32768];
    int iblkCur;
    int iblkCatch;
    PBYTE pb;
    int cb;
    CSimpleCrypt ccRand;

    if(papd->psnStart == 0)
        iblkCatch = 4096 / 16;
    else {
        /* Based on the AP type, we're going to have to insert our data in
         * various places */
        switch(papd->apt) {
        case aptPlaceholder:
            iblkCatch = 0;
            break;
        case aptMirror:
        case aptGroove:
            iblkCatch = (1280 - 128) / 16;
            break;
        case aptVelocity:
            iblkCatch = (1280 + 256) / 16;
            break;
        }
    }

    /* We're going to use a stream cypher for random number generation so we
     * can include more than 32 bits of entropy */
    cb = ccRand.CbGetDigestSize();
    if(cb < sizeof m_rgbAuthHash)
        cb = sizeof m_rgbAuthHash;
    pb = (PBYTE)_alloca(cb);
    memcpy(pb, m_rgbAuthHash, sizeof m_rgbAuthHash);
    *(DWORD *)pb ^= g_rand.Rand();

    for(iblkCur = 0; iblkCur < 4096 / 16; ++iblkCur) {
        if(!ccRand.FComputeSessionKeyFromData(pb, cb) ||
                !ccRand.FSessionEncrypt(rgb, sizeof rgb, rgb))
            g_rand.RandBytes(rgb, sizeof rgb);
        if(iblkCur == iblkCatch) {
            switch(papd->apt) {
            case aptVelocity:
            case aptPlaceholder:
                /* First four bytes are challenge, next four bytes are
                 * response */
                memcpy(rgb, papd->rgbChallenge, 4);
                memcpy(rgb + 4, papd->rgbResponse, 4);
                iblkCatch = 4096 / 16;
                break;
            case aptMirror:
            case aptGroove:
                if(iblkCatch == (1280 - 128) / 16) {
                    /* Lead end; the first two bytes are the first two bytes
                     * of challenge, and the next two bytes are the first
                     * two bytes of response */
                    memcpy(rgb, papd->rgbChallenge, 2);
                    memcpy(rgb + 2, papd->rgbResponse, 2);
                    iblkCatch = ((4096 - 2560) + 256) / 16;
                } else {
                    /* Tail end; the first two bytes are the second two bytes
                     * of challenge, and the next two bytes are the second
                     * two bytes of response */
                    memcpy(rgb, papd->rgbChallenge + 2, 2);
                    memcpy(rgb + 2, papd->rgbResponse + 2, 2);
                    iblkCatch = 4096 / 16;
                }
                break;
            }
        }
        if(ptapDst->CbWrite(sizeof rgb, rgb) != sizeof rgb) {
            fprintf(stderr, "error: writing to destination tape %d\n", ilyr);
            return FALSE;
        }
        if(!ccRand.FComputeDigest(rgb, sizeof rgb, pb))
            ++*(DWORD *)pb;
    }

    return TRUE;
}

BOOL CXControl::FSign(CCryptContainer *pcc)
{
    BYTE rgbData[2048];

    /* Don't sign anything without a DAC */
    if(!m_dwDAC)
        return FALSE;

    if(pcc->CbGetDigestSize() != sizeof m_rgbAuthHash ||
            pcc->CbGetEncryptedDigestSize() != sizeof m_rgbAuthSig)
        return FALSE;
    if(!FAssemble(rgbData, FALSE, TRUE))
        return FALSE;
    if(!pcc->FComputeDigest(rgbData, 1227, m_rgbAuthHash))
        return FALSE;
    if(!pcc->FComputeEncryptedDigest(rgbData, 1227, m_rgbAuthSig))
        return FALSE;
    m_fSigned = TRUE;
    return TRUE;
}

BOOL CECMAControl::FInit(CFileStmSeq *pstm)
{
    BYTE rgb[2048];

    if(pstm->CbRead(sizeof rgb, rgb) != sizeof rgb)
        return FALSE;
    return FInit(rgb);
}

BOOL CECMAControl::FInit(const BYTE *pb)
{
    /* Start clean */
    memset(this, 0, sizeof *this);

    /* Make sure the constant data is right */
    if(pb[0] != 0x01 || pb[1] != 0x02 || pb[2] != 0x31 || pb[3] != 0x10)
        return 0;

    /* Ensure all areas of zero are actually zero */
    if(!FValidateZero(pb, g_rgxcz, sizeof g_rgxcz / sizeof g_rgxcz[0]))
        return FALSE;

    /* Extract the fields */
    Import(pb, g_rgxcxc, sizeof g_rgxcxc / sizeof g_rgxcxc[0]);

    return TRUE;
}

BOOL CECMAControl::FWriteToStm(CFileStmSeq *pstm)
{
    BYTE rgb[2048];

    /* Start clean */
    memset(rgb, 0, sizeof rgb);

    /* Set the constant data */
    rgb[0] = 0x01;
    rgb[1] = 0x02;
    rgb[2] = 0x31;
    rgb[3] = 0x10;

    /* Include the fields */
    Export(rgb, g_rgxcxc, sizeof g_rgxcxc / sizeof g_rgxcxc[0]);

    return pstm->CbWrite(sizeof rgb, rgb) == sizeof rgb;
}

BOOL CECMAControl::FValidate(DXML *pxml) const
{
    DWORD psnEnd1;

    /* The disk regions are rounded to an ECC boundary, so we need to validate
     * by rounding up to an ECC boundary */
    psnEnd1 = ((m_psnEnd1 + 0x10) & ~0xF) - 1;

    return
        m_psnStart0 == pxml->rgdxl[0].dxrData.psnStart &&
        m_psnEnd0 == pxml->rgdxl[0].dxrData.psnStart +
            pxml->rgdxl[0].dxrData.cpsnSize - 1 &&
        (m_psnEnd0 ^ 0xFFFFFF) == pxml->rgdxl[1].dxrData.psnStart &&
        psnEnd1 == pxml->rgdxl[1].dxrData.psnStart +
            pxml->rgdxl[1].dxrData.cpsnSize - 1;
}
