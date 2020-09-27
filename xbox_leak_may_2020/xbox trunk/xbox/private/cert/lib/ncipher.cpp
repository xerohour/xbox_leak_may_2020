/*
 *
 * ncipher.h
 *
 * CNCipherCrypt
 *
 */

#include "precomp.h"

CNCipherCrypt::CNCipherCrypt(void)
{
    m_pbPublicKey = NULL;
    m_hkeyPub = NULL;
    m_hkeySession = NULL;
    m_hcprov = NULL;
    m_fWeAcquiredContext = FALSE;
}

CNCipherCrypt::~CNCipherCrypt()
{
    if(m_pbPublicKey)
        free(m_pbPublicKey);
    if(m_hkeyPub)
        CryptDestroyKey(m_hkeyPub);
    if(m_hkeySession)
        CryptDestroyKey(m_hkeySession);
    if(m_hcprov && m_fWeAcquiredContext)
        CryptReleaseContext(m_hcprov, 0);
}

CNCipherCrypt *CNCipherCrypt::PccCreate(LPCSTR szProvider, LPCSTR szContainer)
{
    CNCipherCrypt *pcc = new CNCipherCrypt;

    /* Get access to the crypt provider */
    if(!CryptAcquireContext(&pcc->m_hcprov, szContainer, szProvider,
            PROV_RSA_FULL, CRYPT_MACHINE_KEYSET))
        goto fatal;
    pcc->m_fWeAcquiredContext = TRUE;
    if(pcc->FCreateInternal())
        return pcc;
fatal:
    delete pcc;
    return NULL;
}

CNCipherCrypt *CNCipherCrypt::PccCreate(HCRYPTPROV hprov)
{
    CNCipherCrypt *pcc = new CNCipherCrypt;

    pcc->m_hcprov = hprov;
    if(pcc->FCreateInternal())
        return pcc;
    delete pcc;
    return NULL;
}

BOOL CNCipherCrypt::FCreateInternal(void)
{
    HCRYPTHASH hhash;
    DWORD cb;
    DWORD cbKey;
    PUBLICKEYSTRUC *pkey;
    RSAPUBKEY *prsa;
    BSAFE_PUB_KEY *pbpk;

    /* Fetch the public key and convert to xcrypt format */
    if(!CryptGetUserKey(m_hcprov, AT_SIGNATURE, &m_hkeyPub))
        return FALSE;
    if(!CryptExportKey(m_hkeyPub, NULL, PUBLICKEYBLOB, 0, NULL, &cb))
        return FALSE;
    pkey = (PUBLICKEYSTRUC *)_alloca(cb);
    if(!CryptExportKey(m_hkeyPub, NULL, PUBLICKEYBLOB, 0, (PBYTE)pkey, &cb))
        return FALSE;
    if(pkey->bType != PUBLICKEYBLOB || pkey->bVersion != 2 ||
        pkey->aiKeyAlg != CALG_RSA_SIGN)
    {
badpubkey:
        SetLastError(NTE_BAD_PUBLIC_KEY);
        return FALSE;
    }
    prsa = (RSAPUBKEY *)(pkey + 1);
    if(prsa->magic != RSA1)
        goto badpubkey;
    cb -= sizeof *prsa + sizeof *pkey;
    cbKey = ((prsa->bitlen + 7) / 8 + 7) & ~7;
    if(cb < cbKey)
        goto badpubkey;
    cbKey = ((prsa->bitlen + 7) / 8 + 8) & ~7;
    if(cbKey < cb)
        goto badpubkey;
    m_pbPublicKey = (PBYTE)malloc(cbKey + sizeof *pbpk);
    pbpk = (BSAFE_PUB_KEY *)m_pbPublicKey;
    pbpk->bitlen = prsa->bitlen;
    pbpk->keylen = ((pbpk->bitlen + 7) / 8 + 8) & ~7;
    pbpk->magic = prsa->magic;
    pbpk->pubexp = prsa->pubexp;
    pbpk->datalen = (pbpk->bitlen + 7) / 8 - 1;
    memcpy(pbpk + 1, prsa + 1, cb);
    if(cb > pbpk->keylen)
        memset((PBYTE)(pbpk + 1) + pbpk->keylen, 0, cb - pbpk->keylen);
    else if(cb < pbpk->keylen)
        memset((PBYTE)(pbpk + 1) + cb, 0, pbpk->keylen - cb);

    /* Get the hash size */
    if(!CryptCreateHash(m_hcprov, CALG_SHA1, NULL, 0, &hhash))
        return FALSE;
    cb = sizeof m_cbHash;
    if(!CryptGetHashParam(hhash, HP_HASHSIZE, (PBYTE)&m_cbHash, &cb, 0))
        m_cbHash = 0;
    CryptDestroyHash(hhash);
    if(!m_cbHash)
        return FALSE;
    m_hhProgressive = NULL;
    m_cbSigned = ((prsa->bitlen + 7) / 8 + 7) & ~7;

    return TRUE;
}

HCRYPTHASH CNCipherCrypt::HhComputeHash(const BYTE *pb, DWORD cb) const
{
    HCRYPTHASH hhash;

    if(!CryptCreateHash(m_hcprov, CALG_SHA1, NULL, 0, &hhash))
        return NULL;

    /* The XC digests all begin with the length dword first */
    if(!CryptHashData(hhash, (PBYTE)&cb, sizeof cb, 0))
        goto fatal;
    /* and then the actual data */
    if(!CryptHashData(hhash, pb, cb, 0))
        goto fatal;
    return hhash;
fatal:
    CryptDestroyHash(hhash);
    return NULL;
}

BOOL CNCipherCrypt::FComputeDigest(const BYTE *pb, DWORD cb, LPBYTE pbDigest) const
{
    HCRYPTHASH hhash;
    BOOL fRet;
    DWORD cbRet;

    hhash = HhComputeHash(pb, cb);
    if(!hhash)
        return NULL;
    /* We just assume that we have enough space to stick the data */
    cbRet = m_cbHash;
    fRet = CryptGetHashParam(hhash, HP_HASHVAL, pbDigest, &cbRet, 0);
    CryptDestroyHash(hhash);
    return fRet;
}

BOOL CNCipherCrypt::FStartProgressiveHash(void)
{
    if(m_hhProgressive)
        CryptDestroyHash(m_hhProgressive);
    if(!CryptCreateHash(m_hcprov, CALG_SHA1, NULL, 0, &m_hhProgressive)) {
        m_hhProgressive = NULL;
        return FALSE;
    }
    return TRUE;
}

BOOL CNCipherCrypt::FProgressiveHashData(const BYTE *pb, DWORD cb)
{
    if(!m_hhProgressive)
        return FALSE;
    if(!CryptHashData(m_hhProgressive, pb, cb, 0)) {
        CryptDestroyHash(m_hhProgressive);
        m_hhProgressive = NULL;
        return FALSE;
    }
    return TRUE;
}

BOOL CNCipherCrypt::FComputeProgressiveHash(LPBYTE pbDigest) const
{
    DWORD cbRet;

    if(!m_hhProgressive)
        return FALSE;
    /* We just assume that we have enough space to stick the data */
    cbRet = m_cbHash;
    return CryptGetHashParam(m_hhProgressive, HP_HASHVAL, pbDigest, &cbRet, 0);
}

BOOL CNCipherCrypt::FSignHash(HCRYPTHASH hh, LPBYTE pbDigest) const
{
    DWORD cbRet;
    BOOL fRet;

    /* We just assume that we have enough space to stick the data */
    cbRet = m_cbSigned;
    fRet = CryptSignHash(hh, AT_SIGNATURE, NULL, CRYPT_NOHASHOID, pbDigest,
        &cbRet);
    if(fRet && cbRet < m_cbSigned) {
        /* Fill the rest of the space with random data */
        g_rand.RandBytes(pbDigest + cbRet, m_cbSigned - cbRet);
    }

    return fRet;
}

BOOL CNCipherCrypt::FEncryptProgressiveHash(LPBYTE pbDigest) const
{
    DWORD cbRet;

    if(!m_hhProgressive)
        return FALSE;
    /* We just assume that we have enough space to stick the data */
    cbRet = m_cbSigned;
    return FSignHash(m_hhProgressive, pbDigest);
}

BOOL CNCipherCrypt::FComputeEncryptedDigest(const BYTE *pb, DWORD cb,
    LPBYTE pbDigest) const
{
    HCRYPTHASH hhash;
    BOOL fRet;
    DWORD cbRet;

    hhash = HhComputeHash(pb, cb);
    if(!hhash)
        return NULL;
    /* We just assume that we have enough space to stick the data */
    cbRet = m_cbSigned;
    fRet = FSignHash(hhash, pbDigest);
    CryptDestroyHash(hhash);

    return fRet;
}

BOOL CNCipherCrypt::FVerifyEncryptedDigest(const BYTE *pb, DWORD cb,
    const BYTE *pbSigned) const
{
    HCRYPTHASH hhash;
    BOOL fRet;

    hhash = HhComputeHash(pb, cb);
    if(!hhash)
        return NULL;

    fRet = CryptVerifySignature(hhash, pbSigned, m_cbSigned, m_hkeyPub,
        NULL, CRYPT_NOHASHOID);
    CryptDestroyHash(hhash);
    return fRet;
}

BOOL CNCipherCrypt::FVerifyEncryptedProgressiveHash(const BYTE *pbSigned) const
{
    if(!m_hhProgressive)
        return FALSE;
    return CryptVerifySignature(m_hhProgressive, pbSigned, m_cbSigned,
        m_hkeyPub, NULL, CRYPT_NOHASHOID);
}

BOOL CNCipherCrypt::FComputeSessionKeyFromData(const BYTE *pb, DWORD cb,
    DWORD cbit)
{
    HCRYPTHASH hhash;
    BOOL fRet = FALSE;

    DestroySessionKey();
    if(!CryptCreateHash(m_hcprov, CALG_SHA1, NULL, 0, &hhash))
        return NULL;
    if(!CryptHashData(hhash, pb, cb, 0))
        goto fatal;
    if(cbit == 0)
        cbit = 8 * m_cbHash;
    fRet = CryptDeriveKey(m_hcprov, CALG_RC4, hhash, cbit << 16,
        &m_hkeySession);

fatal:
    CryptDestroyHash(hhash);
    return NULL;
}

BOOL CNCipherCrypt::FSessionEncrypt(const BYTE *pbIn, DWORD cbIn, LPBYTE pbOut) const
{
    if(!m_hkeySession)
        return FALSE;

    memcpy(pbOut, pbIn, cbIn);
    return CryptEncrypt(m_hkeySession, NULL, 0, TRUE, pbOut, &cbIn, cbIn);
}

BOOL CNCipherCrypt::FSessionDecrypt(const BYTE *pbIn, DWORD cbIn, LPBYTE pbOut) const
{
    if(!m_hkeySession)
        return FALSE;

    memcpy(pbOut, pbIn, cbIn);
    return CryptDecrypt(m_hkeySession, NULL, TRUE, 0, pbOut, &cbIn);
}

void CNCipherCrypt::DestroySessionKey(void)
{
    if(m_hkeySession) {
        CryptDestroyKey(m_hkeySession);
        m_hkeySession = NULL;
    }
}
