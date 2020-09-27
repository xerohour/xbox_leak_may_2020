/*
 *
 * xbekeys.cpp
 *
 * CXBEFile key functions
 *
 */

#include "precomp.h"

static BYTE szTestKey[] = "TESTTESTTESTTEST";

BOOL CXBEFile::FGenKey(DWORD dwTitleID, PBYTE pbSig, int cb,
    CCryptContainer *pcc, const BYTE *pbEntropy, DWORD cbEntropy)
{
    BOOL fRet = FALSE;
    CSimpleCrypt *pccRand;
    BOOL fFinal = FALSE;
    PBYTE pbKeyData;
    int cbKeyData;

    /* We set the keys to zero and then generate an rc4 key to encrypt
     * the zero bytes.  The encrypted zero-bytes constitute random data.  If
     * we're doing a signature key, we use the title ID, convolved with a
     * portion of the XBE public key, as the rc4 key used to generate the
     * random data */
    if(dwTitleID) {
        if(!pcc) {
            /* For non-final TitleID-based keys, we need to use the same key
             * that imagebld uses */
            if(cb < 16)
                memcpy(pbSig, szTestKey, cb);
            else {
                memcpy(pbSig, szTestKey, 16);
                if(cb > 16)
                    memset(pbSig + 16, 0, cb - 16);
            }
            return TRUE;
        }
        pbKeyData = (PBYTE)_alloca(cbKeyData = 2 * sizeof (DWORD));
        ((PDWORD)pbKeyData)[0] = dwTitleID;
        ((PDWORD)pbKeyData)[1] = ((PDWORD)pcc->PbGetPublicKey())[16];
    } else {
        /* We'll use whatever caller-provided entropy we have, plus our own
         * random data and some key data if available */
        cbKeyData = cbEntropy + 4;
        if(pcc)
            cbKeyData += 4;
        pbKeyData = (PBYTE)_alloca(cbKeyData);
        g_rand.RandBytes(pbKeyData, 4);
        if(pcc)
            ((PDWORD)pbKeyData)[1] = ((PDWORD)pcc->PbGetPublicKey())[15];
        if(pbEntropy)
            memcpy(pbKeyData + cbKeyData - cbEntropy, pbEntropy, cbEntropy);
    }
    pccRand = new CSimpleCrypt;
    if(!pccRand->FComputeSessionKeyFromData(pbKeyData, cbKeyData))
        goto fatal;
    if(pccRand->FSessionEncrypt(pbSig, cb, pbSig))
        fRet = TRUE;
fatal:
    delete pccRand;
    return fRet;
}

BOOL CXBEFile::FGenNewKeys(PXBEIMAGE_CERTIFICATE pcert, CCryptContainer *pcc,
    const BYTE *pbEntropy, DWORD cbEntropy)
{
    int itid;

    /* Do the LAN and signature keys first */
    if(!FGenKey(0, pcert->LANKey, sizeof pcert->LANKey, pcc, pbEntropy,
            cbEntropy) || !FGenKey(pcert->TitleID, pcert->SignatureKey,
            sizeof pcert->SignatureKey, pcc, pbEntropy, cbEntropy))
        return FALSE;

    /* Now do the alt title ID keys */
    for(itid = 0; itid < XBEIMAGE_ALTERNATE_TITLE_ID_COUNT; ++itid) {
        if(!FGenKey(pcert->AlternateTitleIDs[itid],
                pcert->AlternateSignatureKeys[itid],
                sizeof pcert->AlternateSignatureKeys[itid], pcc, pbEntropy,
                cbEntropy))
            return FALSE;
    }

    return TRUE;
}

BOOL CXBEFile::FGenNewKeys(const BYTE *pbEntropy, DWORD cbEntropy)
{
    if(!m_pfs)
        return FALSE;

    m_fCertChanged = TRUE;
    return FGenNewKeys(m_pxcert, m_pcc, pbEntropy, cbEntropy);
}
