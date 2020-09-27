/*
 *
 * crypt.h
 *
 * CCryptProvider and related interfaces
 *
 */

#ifndef _CRYPT_H
#define _CRYPT_H

#include <sha.h>

class CCryptContainer
{
public:
    virtual BOOL FComputeDigest(const BYTE *pb, DWORD cb, LPBYTE pbDigest) const =0;
    virtual BOOL FComputeEncryptedDigest(const BYTE *pb, DWORD cb,
        LPBYTE pbDigest) const =0;
    virtual BOOL FStartProgressiveHash(void) =0;
    virtual BOOL FProgressiveHashData(const BYTE *pb, DWORD cb) =0;
    virtual BOOL FComputeProgressiveHash(LPBYTE pbDigest) const =0;
    virtual BOOL FEncryptProgressiveHash(LPBYTE pbDigest) const =0;
    virtual int CbGetDigestSize(void) const =0;
    virtual int CbGetEncryptedDigestSize(void) const =0;
    virtual BOOL FVerifyEncryptedDigest(const BYTE *pb, DWORD cb,
        const BYTE *pbSigned) const =0;
    virtual BOOL FVerifyEncryptedProgressiveHash(const BYTE *pbSigned) const =0;
    virtual const BYTE *PbGetPublicKey(void) const =0;
    virtual BOOL FComputeSessionKeyFromData(const BYTE *pb, DWORD cb, DWORD cbit=0) =0;
    virtual BOOL FSessionEncrypt(const BYTE *pbIn, DWORD cbIn, LPBYTE pbOut) const =0;
    virtual BOOL FSessionDecrypt(const BYTE *pbIn, DWORD cbIn, LPBYTE pbOut) const =0;
    virtual void DestroySessionKey(void) =0;
    virtual ~CCryptContainer() {}
};

class CSimpleCrypt : public CCryptContainer
{
public:
    CSimpleCrypt(void)
    {
        Setup();
        SetKeys(NULL, NULL);
    }
    virtual BOOL FComputeDigest(const BYTE *pb, DWORD cb, LPBYTE pbDigest) const;
    virtual BOOL FComputeEncryptedDigest(const BYTE *pb, DWORD cb,
        LPBYTE pbDigest) const;
    virtual BOOL FStartProgressiveHash(void);
    virtual BOOL FProgressiveHashData(const BYTE *pb, DWORD cb);
    virtual BOOL FComputeProgressiveHash(LPBYTE pbDigest) const;
    virtual BOOL FEncryptProgressiveHash(LPBYTE pbDigest) const;
    virtual int CbGetDigestSize(void) const;
    virtual int CbGetEncryptedDigestSize(void) const;
    virtual BOOL FVerifyEncryptedDigest(const BYTE *pb, DWORD cb,
        const BYTE *pbSigned) const;
    virtual BOOL FVerifyEncryptedProgressiveHash(const BYTE *pbSigned) const;
    virtual const BYTE *PbGetPublicKey(void) const { return m_pbPublicKey; }
    virtual BOOL FComputeSessionKeyFromData(const BYTE *pb, DWORD cb, DWORD cbit=0);
    virtual BOOL FSessionEncrypt(const BYTE *pbIn, DWORD cbIn, LPBYTE pbOut) const;
    virtual BOOL FSessionDecrypt(const BYTE *pbIn, DWORD cbIn, LPBYTE pbOut) const;
    virtual void DestroySessionKey(void);
    virtual ~CSimpleCrypt() { DestroySessionKey(); }
protected:
    CSimpleCrypt(const BYTE *pbPublic, const BYTE *pbPrivate)
    {
        Setup();
        SetKeys(pbPublic, pbPrivate);
    }
    void Setup(void);
    void SetKeys(const BYTE *pbPublic, const BYTE *pbPrivate);
private:
    const BYTE *m_pbPublicKey;
    const BYTE *m_pbPrivateKey;
    BYTE *m_pbSessionKey;
    int m_cbSessionKey;
    A_SHA_CTX m_sha;
};

class CDevkitCrypt : public CSimpleCrypt
{
public:
    CDevkitCrypt(void) : CSimpleCrypt(rgbPublicKey, rgbPrivateKey) {}
private:
    static const BYTE rgbPrivateKey[];
    static const BYTE rgbPublicKey[];
};

class CSHACrypt : public CSimpleCrypt
{
public:
    CSHACrypt(void) : CSimpleCrypt(NULL, NULL) {}
    virtual BOOL FComputeDigest(const BYTE *pb, DWORD cb, LPBYTE pbDigest) const;
};

class CRetailCrypt : public CSimpleCrypt
{
public:
    CRetailCrypt(void);
private:
    BYTE m_rgbPublicKey[284];
    static const BYTE rgbLeftHalf[];
    static const BYTE rgbRightHalf[];
};
#endif // _CRYPT_H
