/*
 *
 * xbefile.h
 *
 * CXBEFile
 *
 */

#ifndef _XBEFILE_H
#define _XBEFILE_H

#include <filestm.h>
#include <xbeimage.h>
#include <crypt.h>
#include <propbag.h>
#include <xsum.h>

class CXBEFile
{
public:
    CXBEFile(CFileStmRand *pfs);
    ~CXBEFile();

    BOOL FVerifySignature(void);
    int GetLibApprovalLevel(void);
    BOOL FInjectNewCertificate(const CVarBag *pbag);
    BOOL FInjectNewCertificate(const XBEIMAGE_CERTIFICATE *pcert);
    static BOOL FUnpackCertificate(const CVarBag *pbag,
        PXBEIMAGE_CERTIFICATE pcert);
    const XBEIMAGE_CERTIFICATE *GetCert(void) const
    {
        return m_pfs ? m_pxcert : NULL;
    }
    const XBEIMAGE_HEADER *GetHeader(void) const
    {
        return m_pfs ? &m_xih : NULL;
    }
    static BOOL FGenNewKeys(PXBEIMAGE_CERTIFICATE pcert, CCryptContainer *pcc,
        const BYTE *pbEntropy=NULL, DWORD cbEntropy=0);
    BOOL FGenNewKeys(const BYTE *pbEntropy=NULL, DWORD cbEntropy=0);
    BOOL FSignFile(void);
    BOOL FCommitChanges(void);
    BOOL FWriteToStm(CFileStmSeq *pfs, DWORD cbToWrite=0, DWORD cbBlk=0) const;
    BOOL FCalcChecksums(CCheckSum *pxsumBlk, CCheckSum *pxsumRed,
        CCheckSum *pxsumGrn) const;
    void UseCryptContainer(CCryptContainer *pcc)
    {
        m_pcc = pcc;
    }
    BOOL FInjectNewHeader(const XBEIMAGE_HEADER *pxih);

private:
    union {
        struct {
            BOOL m_fVerifiedSectionDigests:1,
                m_fValidSectionDigests:1,
                m_fCertChanged:1,
                m_fSignatureChanged:1,
                m_fGotValidHdrFields:1,
                unused:27;
        };
        DWORD m_dwFlags;
    };
    CFileStmRand *m_pfs;
    CCryptContainer *m_pcc;
    XBEIMAGE_HEADER m_xih;
    PBYTE m_rgbHdr;
    PXBEIMAGE_CERTIFICATE m_pxcert;
    PXBEIMAGE_SECTION m_pxshFirst;
    ULONG m_ulEntryReal;
    ULONG m_ulKrnlReal;
    DWORD m_cbFileSize;

    BOOL FSetupHeaders(void);
    BOOL FVerifySectionDigest(int);
    BOOL FVerifyAllSectionDigests(void);
    static BOOL FGenKey(DWORD dwTitleID, PBYTE pbSig, int cb,
        CCryptContainer *pcc, const BYTE *pbEntropy, DWORD cbEntropy);

    PVOID PvHeaderField(PVOID pvVA)
    {
        return (PVOID)((PBYTE)m_rgbHdr - sizeof m_xih +
            ((DWORD)pvVA - (DWORD)m_xih.BaseAddress));
    }
};

#endif // XBEFILE_H
