/*
 *
 * flcrypt.cpp
 *
 * CFloppyCrypt
 *
 */

#include "precomp.h"

const char CFloppyCrypt::szExtraPasswdBytes[] =
    "EXTRAPASSWORDCHARACTERSATLEAST32";

int CFloppyCrypt::CchGetPasswd(LPCSTR szPrompt, LPSTR sz, int cchMax)
{
    int ich = 0;
    char ch;
    
    _cputs(szPrompt);
    for(;;) {
        ch = (char)_getch();
        switch(ch) {
        case 8:
        case 127:
            if(ich > 0)
                --ich;
            break;
        case 10:
        case 13:
            _putch('\r');
            _putch('\n');
            return ich;
        default:
            if(ich < cchMax)
                sz[ich++] = ch;
            else
                ++ich;
            break;
        }
    }
}

BOOL CFloppyCrypt::FRead(BOOL fUnsecure)
{
    HANDLE hFile;
    BYTE rgbBuf[8192];
    XCKH *pxckh;
    FLKH *pflkh;
    OFLKH *poflkh;
    PBYTE pbPub;
    PBYTE pbPri;
    PBYTE pbEncDigest;
    BOOL fRet = FALSE;
    char szPasswd[32];
    const char *pch;
    int ich;
    int cb;

    /* We're going to do unbuffered I/O, so we need an aligned buffer */
    pflkh = (FLKH *)(((ULONG)rgbBuf + 0xFFF) & ~0xFFF);

    /* Get the data */
    hFile = CreateFile(fUnsecure ? "A:\\KEYPAIR.XCK" : "A:\\KEYPAIR.FLK",
        GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    if(INVALID_HANDLE_VALUE == hFile)
        return FALSE;
    ReadFile(hFile, pflkh, 4096, &m_cb, NULL);
    CloseHandle(hFile);

    /* Decrypt the keys */
    if(fUnsecure)
        m_cb += sizeof *pflkh;
    m_pflkh = (FLKH *)malloc(m_cb);
    pxckh = (XCKH *)(m_pflkh + 1);
    if(fUnsecure) {
        memset(m_pflkh, 0, sizeof *m_pflkh);
        m_pflkh->sig = 0x4346;
        memcpy(m_pflkh->rgbID, "UNSECURE", 8);
        memcpy(m_pflkh + 1, pflkh, m_cb - sizeof *pflkh);
#if 0
    } else if(pflkh->sig == 0x4B46) {
        /* Handle old keys for conversion */
        memcpy(m_pflkh, pflkh, pflkh->rgbLeadin - (PBYTE)pflkh);
        ich = CchGetPasswd("Enter key password: ", szPasswd, sizeof szPasswd);
        pch = szExtraPasswdBytes;
        while(ich < sizeof szPasswd)
            szPasswd[ich++] = *pch++;
        if(!FComputeSessionKeyFromData((PBYTE)szPasswd, sizeof szPasswd))
            goto fatal;
        if(!FSessionDecrypt(pflkh->rgbLeadin, m_cb - (pflkh->rgbLeadin -
                (PBYTE)pflkh), m_pflkh->rgbLeadin))
            goto fatal;
        memset(m_pflkh->rgbLeadin, 0, sizeof m_pflkh->rgbLeadin);
#endif
    } else if(pflkh->sig == 0x4C46) {
        memset(m_pflkh, 0, sizeof *m_pflkh);
        m_pflkh->sig = 0x4346;
        poflkh = (OFLKH *)pflkh;
        ich = CchGetPasswd("Enter key password: ", szPasswd, sizeof szPasswd);
        pch = szExtraPasswdBytes;
        while(ich < sizeof szPasswd)
            szPasswd[ich++] = *pch++;
        if(!FComputeSessionKeyFromData((PBYTE)szPasswd, sizeof szPasswd))
            goto fatal;
        /* This gives us the key to decrypt the lead key */
        if(!FSessionEncrypt(poflkh->rgbLeadin, sizeof poflkh->rgbLeadin,
                poflkh->rgbLeadin))
            goto fatal;
        /* Now we can compute the message key */
        if(!FComputeSessionKeyFromData(poflkh->rgbLeadin,
                sizeof poflkh->rgbLeadin))
            goto fatal;
        if(!FSessionDecrypt(poflkh->rgbID, m_cb - (m_pflkh->rgbID -
                (PBYTE)m_pflkh), m_pflkh->rgbID))
            goto fatal;
    } else {
        if(pflkh->sig != 0x4346)
            goto fatal;
        memcpy(m_pflkh, pflkh, pflkh->rgbLeadin - (PBYTE)pflkh);
        ich = CchGetPasswd("Enter key password: ", szPasswd, sizeof szPasswd);
        pch = szExtraPasswdBytes;
        while(ich < sizeof szPasswd)
            szPasswd[ich++] = *pch++;
        if(!FComputeSessionKeyFromData((PBYTE)szPasswd, sizeof szPasswd))
            goto fatal;
        /* This gives us the key to decrypt the lead key */
        if(!FSessionEncrypt(pflkh->rgbLeadin, sizeof pflkh->rgbLeadin,
                m_pflkh->rgbLeadin))
            goto fatal;
        /* Now we can compute the message key */
        if(!FComputeSessionKeyFromData(m_pflkh->rgbLeadin,
                sizeof m_pflkh->rgbLeadin))
            goto fatal;
        if(!FSessionDecrypt(pflkh->rgbDigest, m_cb - (pflkh->rgbDigest -
                (PBYTE)pflkh), m_pflkh->rgbDigest))
            goto fatal;
        memset(m_pflkh->rgbLeadin, 0, sizeof m_pflkh->rgbLeadin);
        /* Verify the key digest */
        cb = sizeof *pxckh + pxckh->cbPub + pxckh->cbPri;
        if(cb + sizeof *m_pflkh > m_cb)
            goto fatal;
        if(!FComputeDigest((PBYTE)pxckh, cb, pflkh->rgbDigest))
            goto fatal;
        if(memcmp(pflkh->rgbDigest, m_pflkh->rgbDigest,
                sizeof m_pflkh->rgbDigest))
            goto fatal;
        memset(m_pflkh->rgbDigest, 0, sizeof m_pflkh->rgbDigest);
    }

    if(pxckh->u1 != 1)
        goto fatal;
    if(m_cb < sizeof *pxckh + sizeof *pflkh)
        goto fatal;
    if(m_cb < sizeof *pxckh + sizeof *pflkh + pxckh->cbPub + pxckh->cbPri)
        goto fatal;

    /* Select in the key */
    pbPub = (PBYTE)(pxckh + 1);
    pbPri = pxckh->cbPri > 32 ? pbPub + pxckh->cbPub : NULL;
    SetKeys(pbPub, pbPri);

    /* Validate the key pair */
    if(pbPri) {
        pbEncDigest = (PBYTE)_alloca(CbGetEncryptedDigestSize());
        if(!FComputeEncryptedDigest(rgbBuf, sizeof rgbBuf, pbEncDigest) ||
                !FVerifyEncryptedDigest(rgbBuf, sizeof rgbBuf, pbEncDigest))
            goto fatal;
    }

    /* At this point, we're good to go */
    fRet = TRUE;

fatal:
    memset(rgbBuf, 0, sizeof rgbBuf);
    memset(szPasswd, 0, sizeof szPasswd);
    DestroySessionKey();
    if(!fRet)
        Destroy();
    return fRet;
}

CFloppyCrypt *CFloppyCrypt::PccCreate(BOOL fUnsecure)
{
    CFloppyCrypt *pcc = new CFloppyCrypt(NULL, 0, NULL);
    if(pcc->FRead(fUnsecure))
        return pcc;
    delete pcc;
    return FALSE;
}

BOOL CFloppyCrypt::FWrite(LPCSTR szID)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    BYTE rgbBuf[8192];
    XCKH *pxckh;
    FLKH *pflkh;
    BOOL fRet = FALSE;
    char szPasswd1[32];
    char szPasswd2[32];
    BYTE rgbTempID[14];
    const char *pch;
    int ich;
    DWORD cb;
    BOOL fT;
    CRandom rand;

    if(!m_pflkh)
        return FALSE;

    /* We're going to do unbuffered I/O, so we need an aligned buffer */
    pflkh = (FLKH *)(((ULONG)rgbBuf + 0xFFF) & ~0xFFF);
    memset(pflkh, 0, 4096);

    /* Prepare the header */
    pflkh->sig = 0x4346;

    /* Get the password */
    ich = CchGetPasswd("Enter new key password: ", szPasswd1, sizeof szPasswd1);
    if(ich < 8) {
        fprintf(stderr, "Passwords must be at least 8 chars\n");
        goto fatal;
    }
    if(ich != CchGetPasswd("Enter it again: ", szPasswd2, sizeof szPasswd2) ||
        memcmp(szPasswd1, szPasswd2, ich))
    {
        fprintf(stderr, "Passwords do not match\n");
        goto fatal;
    }

    /* Digest the key */
    pxckh = (XCKH *)(m_pflkh + 1);
    cb = sizeof *pxckh + pxckh->cbPub + pxckh->cbPri;
    if(cb + sizeof *m_pflkh > m_cb)
        goto fatal;
    if(!FComputeDigest((PBYTE)pxckh, cb, m_pflkh->rgbDigest))
        goto fatal;

    /* Set up the password key */
    pch = szExtraPasswdBytes;
    while(ich < sizeof szPasswd1)
        szPasswd1[ich++] = *pch++;
    if(!FComputeSessionKeyFromData((PBYTE)szPasswd1, sizeof szPasswd1))
        goto fatal;

    /* Set up the lead key */
    rand.RandBytes(pflkh->rgbLeadin, sizeof pflkh->rgbLeadin);
    if(!FSessionEncrypt(pflkh->rgbLeadin, sizeof pflkh->rgbLeadin,
            m_pflkh->rgbLeadin))
        goto fatal;

    /* Set up the message key */
    if(!FComputeSessionKeyFromData(m_pflkh->rgbLeadin, sizeof m_pflkh->rgbLeadin))
        goto fatal;

    /* Set the ID and encrypt the data */
    memcpy(rgbTempID, m_pflkh->rgbID, sizeof pflkh->rgbID);
    memset(m_pflkh->rgbID, 0, sizeof m_pflkh->rgbID);
    strncpy((LPSTR)m_pflkh->rgbID, szID, sizeof m_pflkh->rgbID);
    fT = FSessionEncrypt(m_pflkh->rgbDigest, m_cb - (m_pflkh->rgbDigest -
        (PBYTE)m_pflkh), pflkh->rgbDigest);
    
    /* Restore the old ID */
    memcpy(m_pflkh->rgbID, rgbTempID, sizeof pflkh->rgbID);
    memset(m_pflkh->rgbDigest, 0, sizeof m_pflkh->rgbDigest);

    if(!fT)
        goto fatal;

    /* Now write the file */
    hFile = CreateFile("A:\\KEYPAIR.FLK", GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_FLAG_NO_BUFFERING, NULL);
    if(INVALID_HANDLE_VALUE == hFile)
        goto fatal;
    if(!WriteFile(hFile, pflkh, 4096, &cb, NULL))
        cb = 0;
    if(cb != 4096)
        goto fatal;
    /* We'll keep the extra data until I can figure out how to truncate it
     * without buffering */
#if 0
    if(INVALID_SET_FILE_POINTER == SetFilePointer(hFile, m_cb, NULL,
            FILE_BEGIN) || !SetEndOfFile(hFile))
        goto fatal;
#endif

    /* We've succeeded */
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;
    fRet = TRUE;

fatal:
    if(INVALID_HANDLE_VALUE != hFile) {
        CloseHandle(hFile);
        DeleteFile("A:\\KEYPAIR.FLK");
    }
    memset(szPasswd1, 0, sizeof szPasswd1);
    memset(szPasswd2, 0, sizeof szPasswd2);
    memset(rgbBuf, 0, sizeof rgbBuf);
    memset(m_pflkh->rgbLeadin, 0, sizeof m_pflkh->rgbLeadin);
    memset(m_pflkh->rgbDigest, 0, sizeof m_pflkh->rgbDigest);
    DestroySessionKey();
    return fRet;
}

BOOL CFloppyCrypt::FWritePub(void) const
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    BYTE rgbBuf[8192];
    XCKH *pxckh;
    XCKH *pxckhOut;
    BOOL fRet = FALSE;
    const char *pch;
    int ich;
    DWORD cb;
    BOOL fT;

    if(!m_pflkh)
        return FALSE;

    /* We're going to do unbuffered I/O, so we need an aligned buffer */
    pxckhOut = (XCKH *)(((ULONG)rgbBuf + 0xFFF) & ~0xFFF);
    memset(pxckhOut, 0, 4096);

    /* Copy only as much data as is in the public key */
    pxckh = (XCKH *)(m_pflkh + 1);
    memcpy(pxckhOut, pxckh, sizeof *pxckh + pxckh->cbPub);

    /* Mark the private key as 16 bytes of zero */
    pxckhOut->cbPri = 16;

    /* Now write the file */
    hFile = CreateFile("A:\\PUBKEY.XCK", GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_FLAG_NO_BUFFERING, NULL);
    if(INVALID_HANDLE_VALUE == hFile)
        goto fatal;
    if(!WriteFile(hFile, pxckhOut, 4096, &cb, NULL))
        cb = 0;
    if(cb != 4096)
        goto fatal;
    /* We'll keep the extra data until I can figure out how to truncate it
     * without buffering */

    /* We've succeeded */
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;
    fRet = TRUE;

fatal:
    if(INVALID_HANDLE_VALUE != hFile) {
        CloseHandle(hFile);
        DeleteFile("A:\\PUBKEY.XCK");
    }
    memset(rgbBuf, 0, sizeof rgbBuf);
    return fRet;
}

void CFloppyCrypt::Destroy(void)
{
    SetKeys(NULL, NULL);
    if(m_pflkh) {
        memset(m_pflkh, 0, m_cb);
        free(m_pflkh);
    }
    m_pflkh = NULL;
}

BOOL CFloppyCrypt::FGetKeyId(LPBYTE pb) const
{
    if(!m_pflkh)
        return FALSE;
    memcpy(pb, m_pflkh->rgbID, sizeof m_pflkh->rgbID);
    return TRUE;
}

CFloppyCrypt *CFloppyCrypt::PccImportKey(const CNCipherCrypt *pcc)
{
    XCKH *pxckh;
    FLKH *pflkh;
    int cb;
    BSAFE_PUB_KEY *pbpk;

    /* Get the public key */
    pbpk = (BSAFE_PUB_KEY *)pcc->PbGetPublicKey();
    if(!pbpk || pbpk->magic != RSA1)
        return NULL;

    /* Allocate some space for our version of the data */
    cb = sizeof *pbpk + pbpk->keylen;
    pflkh = (FLKH *)malloc(cb + sizeof *pflkh + sizeof *pxckh + 16);
    memset(pflkh, 0, sizeof *pflkh);
    pxckh = (XCKH *)(pflkh + 1);
    memset(pxckh, 0, sizeof *pxckh);

    /* Copy in the public key */
    pxckh->cbPub = cb;
    memcpy(pxckh + 1, pbpk, cb);

    /* Set the private key as 16 bytes of zero */
    pxckh->cbPri = 16;
    memset((PBYTE)(pxckh + 1) + cb, 0, 16);

    /* Set up the rest of the key headers */
    pxckh->u1 = 1;
    pxckh->u2 = (USHORT)pbpk->bitlen;

    /* And create our object */
    return new CFloppyCrypt(pflkh, cb + sizeof *pflkh + sizeof *pxckh + 16,
        (PBYTE)pbpk);
}
