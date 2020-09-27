/*
 *
 * ncipher.h
 *
 * CNCipherCrypt
 *
 */

#ifndef _NCIPHER_H
#define _NCIPHER_H

#include <crypt.h>

class CNCipherCrypt : public CCryptContainer
{
public:
    ~CNCipherCrypt();
    static CNCipherCrypt *PccCreate(LPCSTR szProvider, LPCSTR szContainer);
    static CNCipherCrypt *PccCreate(HCRYPTPROV hprov);
    virtual BOOL FComputeDigest(const BYTE *pb, DWORD cb, LPBYTE pbDigest) const;
    virtual BOOL FComputeEncryptedDigest(const BYTE *pb, DWORD cb,
        LPBYTE pbDigest) const;
    virtual BOOL FStartProgressiveHash(void);
    virtual BOOL FProgressiveHashData(const BYTE *pb, DWORD cb);
    virtual BOOL FComputeProgressiveHash(LPBYTE pbDigest) const;
    virtual BOOL FEncryptProgressiveHash(LPBYTE pbDigest) const;
    virtual int CbGetDigestSize(void) const { return m_cbHash; };
    virtual int CbGetEncryptedDigestSize(void) const { return m_cbSigned; };
    virtual BOOL FVerifyEncryptedDigest(const BYTE *pb, DWORD cb,
        const BYTE *pbSigned) const;
    virtual BOOL FVerifyEncryptedProgressiveHash(const BYTE *pbSigned) const;
    virtual const BYTE *PbGetPublicKey(void) const { return m_pbPublicKey; }
    virtual BOOL FComputeSessionKeyFromData(const BYTE *pb, DWORD cb, DWORD cbit=0);
    virtual BOOL FSessionEncrypt(const BYTE *pbIn, DWORD cbIn, LPBYTE pbOut) const;
    virtual BOOL FSessionDecrypt(const BYTE *pbIn, DWORD cbIn, LPBYTE pbOut) const;
    virtual void DestroySessionKey(void);

    HCRYPTPROV HProvider(void) const { return m_hcprov; }
private:
    CNCipherCrypt(void);
    HCRYPTPROV m_hcprov;
    HCRYPTKEY m_hkeyPub;
    HCRYPTKEY m_hkeySession;
    HCRYPTHASH m_hhProgressive;
    DWORD m_cbHash;
    DWORD m_cbSigned;
    PBYTE m_pbPublicKey;
    BOOL m_fWeAcquiredContext;

    BOOL FCreateInternal(void);
    HCRYPTHASH HhComputeHash(const BYTE *pb, DWORD cb) const;
    BOOL FSignHash(HCRYPTHASH hh, LPBYTE pbDigest) const;
};

#endif // _NCIPHER_H
