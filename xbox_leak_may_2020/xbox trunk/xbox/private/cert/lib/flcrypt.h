/*
 *
 * flcrypt.h
 *
 * CFloppyCrypt
 *
 */

#ifndef _FLCRYPT_H
#define _FLCRYPT_H

class CNCipherCrypt;

class CFloppyCrypt : public CSimpleCrypt
{
public:
    static CFloppyCrypt *PccImportKey(const CNCipherCrypt *);
    static CFloppyCrypt *PccCreate(BOOL fUnsecure=FALSE);
    BOOL FWrite(LPCSTR szID);
    BOOL FWritePub(void) const;
    BOOL FGetKeyId(LPBYTE pb) const;
    ~CFloppyCrypt() { Destroy(); }

private:
    struct XCKH {
        USHORT u1;
        USHORT u2;
        BYTE rgb[16];
        ULONG u3;
        ULONG cbPub;
        ULONG cbPri;
    };

    struct FLKH {
        USHORT sig;
        BYTE rgbLeadin[20];
        BYTE rgbDigest[20];
        BYTE rgbID[14];
    };

    struct OFLKH {
        USHORT sig;
        BYTE rgbLeadin[16];
        BYTE rgbID[14];
    };

    FLKH *m_pflkh;
    DWORD m_cb;

    BOOL FRead(BOOL fUnsecure);
    CFloppyCrypt(FLKH *pflkh, DWORD cb, PBYTE pbPubKey) :
        CSimpleCrypt(pbPubKey, NULL)
    {
        m_pflkh = pflkh;
        m_cb = cb;
    }

    void Destroy(void);

    static int CchGetPasswd(LPCSTR szPrompt, LPSTR sz, int cchMax);
    static const char szExtraPasswdBytes[];
};

#endif // _FLCRYPT_H