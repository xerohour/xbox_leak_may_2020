/*
 *
 * xbefile.cpp
 *
 * CXBEFile
 *
 */

#include "precomp.h"

CXBEFile::CXBEFile(CFileStmRand *pfs) : m_pfs(pfs), m_pcc(NULL), m_dwFlags(0)
{
    if(!FSetupHeaders())
        m_pfs = NULL;
}

CXBEFile::~CXBEFile()
{
    if(m_pfs)
        delete m_rgbHdr;
}

/* This is here just to make error cleanup easier */
class BytePtr
{
    PBYTE m_pb;
public:
    BytePtr(void) : m_pb(NULL) {}
    ~BytePtr() { if(m_pb) delete m_pb; }
    void Release(void) { m_pb = NULL; }
    void Set(PBYTE pb) { m_pb = pb; }
    operator BYTE *() { return m_pb; }
};

BOOL CXBEFile::FSetupHeaders(void)
{
    int cbHdrData;
    BytePtr pb;
    PXBEIMAGE_SECTION pxsh;
    DWORD cb;

    /* Read all of the header data */
    if(!m_pfs->FSeek(0))
        return FALSE;
    if(m_pfs->CbRead(sizeof m_xih, (PBYTE)&m_xih) != sizeof m_xih)
        return FALSE;
    if(m_xih.Signature != XBEIMAGE_SIGNATURE)
        return FALSE;
    cbHdrData = m_xih.SizeOfHeaders - sizeof m_xih;
    if(cbHdrData <= 0)
        return FALSE;
    /* We refuse to read anything that doesn't have the certificate and all of
     * the section headers as part of the full header */
    if((PBYTE)m_xih.Certificate < (PBYTE)m_xih.BaseAddress ||
            (PBYTE)m_xih.SectionHeaders < (PBYTE)m_xih.BaseAddress ||
            (PBYTE)(m_xih.Certificate + 1) - (PBYTE)m_xih.BaseAddress >
            (int)m_xih.SizeOfHeaders || (PBYTE)(m_xih.SectionHeaders +
            m_xih.NumberOfSections) - (PBYTE)m_xih.BaseAddress >
            (int)m_xih.SizeOfHeaders)
        return FALSE;
    m_rgbHdr = new BYTE[cbHdrData];
    pb.Set(m_rgbHdr);
    if(m_pfs->CbRead(cbHdrData, m_rgbHdr) != (DWORD)cbHdrData)
        return FALSE;
    m_pxcert = (PXBEIMAGE_CERTIFICATE)PvHeaderField(m_xih.Certificate);
    /* Allow for old-style (build 3803) certificates here */
    if(m_pxcert->SizeOfCertificate != 0xD0 &&
            m_pxcert->SizeOfCertificate != sizeof *m_pxcert)
        return FALSE;
    m_pxshFirst = (PXBEIMAGE_SECTION)PvHeaderField(m_xih.SectionHeaders);
    
    /* Calculate the file size */
    m_cbFileSize = 0;
    pxsh = m_pxshFirst + m_xih.NumberOfSections;
    while(--pxsh >= m_pxshFirst) {
        cb = pxsh->PointerToRawData + pxsh->SizeOfRawData;
        if(cb > m_cbFileSize)
            m_cbFileSize = cb;
    }

    pb.Release();
    return TRUE;
}

int CXBEFile::GetLibApprovalLevel(void)
{
    PXBEIMAGE_LIBRARY_VERSION pxlv;
    PXBEIMAGE_LIBRARY_VERSION pxlvXapi;

    if(!m_pfs)
        return 0;

    /* Non-xboxkrnl imports are unacceptable */
    if(m_xih.ImportDirectory)
        return 0;

    pxlv = (PXBEIMAGE_LIBRARY_VERSION)PvHeaderField(m_xih.LibraryVersions);
    if(m_xih.XapiLibraryVersion) {
        pxlvXapi =
            (PXBEIMAGE_LIBRARY_VERSION)PvHeaderField(m_xih.XapiLibraryVersion);
    } else
        pxlvXapi = NULL;

    return CheckLibraryApprovalStatus(pxlvXapi, pxlv,
        m_xih.NumberOfLibraryVersions, NULL);
}

BOOL CXBEFile::FVerifySectionDigest(int isec)
{
    PXBEIMAGE_SECTION pxsh;
    BYTE rgbDigest[XC_DIGEST_LEN];
    BytePtr pbSectionData;
    DWORD cbHead;
    DWORD cbTail;
    DWORD cbBody;
    DWORD ibHead;

    if(!m_pfs || !m_pcc)
        return FALSE;
    /* Make sure we have the right digest size */
    if(m_pcc->CbGetDigestSize() != sizeof pxsh->SectionDigest)
        return FALSE;
    pxsh = m_pxshFirst + isec;

    /* Make sure the section doesn't extend beyond where we think EOF is */
    if(pxsh->PointerToRawData + pxsh->SizeOfRawData < pxsh->PointerToRawData ||
            pxsh->PointerToRawData + pxsh->SizeOfRawData > m_cbFileSize)
        return FALSE;

    /* Make space to stick the section data */
    pbSectionData.Set(new BYTE[pxsh->SizeOfRawData]);

    /* Load the section data */
    if(!m_pfs->FSeek(pxsh->PointerToRawData))
        return FALSE;
    if(m_pfs->CbRead(pxsh->SizeOfRawData, pbSectionData) != pxsh->SizeOfRawData)
        return FALSE;

    /* Now that we have all of the section data, compute the digest */
    if(!m_pcc->FComputeDigest(pbSectionData, pxsh->SizeOfRawData, rgbDigest))
        return FALSE;

    /* Verify that the digest is correct */
    return memcmp(pxsh->SectionDigest, rgbDigest, XC_DIGEST_LEN) == 0;
}

BOOL CXBEFile::FVerifyAllSectionDigests(void)
{
    if(!m_pfs)
        return FALSE;

    if(!m_fVerifiedSectionDigests) {
        m_fVerifiedSectionDigests = TRUE;
        /* Loop over all of the sections and verify the section digest for each */
        for(int isec = m_xih.NumberOfSections; isec--; ) {
            if(!FVerifySectionDigest(isec))
                return FALSE;
        }
        m_fValidSectionDigests = TRUE;
    }

    return m_fValidSectionDigests;
}

BOOL CXBEFile::FVerifySignature(void)
{
    BytePtr pbHeader;
    PXBEIMAGE_HEADER pxih;
    PBYTE pbToSign;
    PULONG pulKey;

    if(!m_pfs || !m_pcc)
        return FALSE;

    if(m_pcc->CbGetEncryptedDigestSize() != sizeof m_xih.EncryptedDigest)
        return FALSE;

    if(!FVerifyAllSectionDigests())
        return FALSE;

    /* Put the headers back together */
    pbHeader.Set(new BYTE[m_xih.SizeOfHeaders]);
    pxih = (PXBEIMAGE_HEADER)(PBYTE)pbHeader;
    memcpy(pxih, &m_xih, sizeof m_xih);
    memcpy(pxih + 1, m_rgbHdr, m_xih.SizeOfHeaders - sizeof m_xih);
    pbToSign = &pxih->EncryptedDigest[sizeof pxih->EncryptedDigest];

    /* Verify the header digest */
    if(!m_pcc->FVerifyEncryptedDigest(pbToSign, pbHeader +
            m_xih.SizeOfHeaders - pbToSign, m_xih.EncryptedDigest))
        return FALSE;

    /* Grab the encrypted header fields */
    pulKey = (PULONG)&m_pcc->PbGetPublicKey()[128];
    m_ulEntryReal = (ULONG)m_xih.AddressOfEntryPoint ^ pulKey[0] ^ pulKey[4];
    m_ulKrnlReal = (ULONG)m_xih.XboxKernelThunkData ^ pulKey[1] ^ pulKey[2];
    m_fGotValidHdrFields = TRUE;
    return TRUE;
}

BOOL CXBEFile::FSignFile(void)
{
    BytePtr pbSignature;
    BytePtr pbHeader;
    PXBEIMAGE_HEADER pxih;
    PBYTE pbToSign;
    PULONG pulKey;

    if(!m_pfs || !m_pcc || !m_fGotValidHdrFields)
        return FALSE;

    /* We require that the encrypted digest hasn't changed size */
    if(m_pcc->CbGetEncryptedDigestSize() != sizeof m_xih.EncryptedDigest)
        return FALSE;

    if(!FVerifyAllSectionDigests())
        return FALSE;

    /* Reinsert the scrambled header fields */
    pulKey = (PULONG)&m_pcc->PbGetPublicKey()[128];
    *(PULONG)&m_xih.AddressOfEntryPoint = m_ulEntryReal ^ pulKey[0] ^ pulKey[4];
    *(PULONG)&m_xih.XboxKernelThunkData = m_ulKrnlReal ^ pulKey[1] ^ pulKey[2];

    /* Put the headers back together */
    pbHeader.Set(new BYTE[m_xih.SizeOfHeaders]);
    pxih = (PXBEIMAGE_HEADER)(PBYTE)pbHeader;
    memcpy(pxih, &m_xih, sizeof m_xih);
    memcpy(pxih + 1, m_rgbHdr, m_xih.SizeOfHeaders - sizeof m_xih);
    pbToSign = &pxih->EncryptedDigest[sizeof pxih->EncryptedDigest];

    /* Compute the encrypted header digest */
    pbSignature.Set(new BYTE[sizeof m_xih.EncryptedDigest]);
    if(!m_pcc->FComputeEncryptedDigest(pbToSign, pbHeader +
            m_xih.SizeOfHeaders - pbToSign, pbSignature))
        return FALSE;
    memcpy(m_xih.EncryptedDigest, pbSignature, sizeof m_xih.EncryptedDigest);
    m_fSignatureChanged = TRUE;
    return TRUE;
}

BOOL CXBEFile::FInjectNewHeader(const XBEIMAGE_HEADER *pxih)
{
    /* We'll act as if this data just came from the file, which means we'll
     * mark all state as initial.  We'll verify the signature when we're done
     * to signify whether we believe this header was good */
    if(!m_pfs || !m_pcc)
        return FALSE;

    memcpy(&m_xih, pxih, sizeof m_xih);
    m_fVerifiedSectionDigests = FALSE;
    m_fValidSectionDigests = FALSE;
    m_fSignatureChanged = TRUE;
    m_fGotValidHdrFields = FALSE;

    return FVerifySignature();
}

BOOL CXBEFile::FCommitChanges(void)
{
    if(!m_pfs)
        return FALSE;

    /* Don't commit an incomplete modification */
    if(m_fCertChanged && !m_fSignatureChanged)
        return FALSE;

    /* If we have a new certificate, write it out */
    if(m_fCertChanged) {
        if(!m_pfs->FSeek((PBYTE)m_xih.Certificate - (PBYTE)m_xih.BaseAddress))
            return FALSE;
        if(m_pfs->CbWrite(sizeof *m_pxcert, (PBYTE)m_pxcert) !=
                sizeof *m_pxcert)
            return FALSE;
        m_fCertChanged = FALSE;
    }

    /* If we have a new signature, write it out */
    if(m_fSignatureChanged) {
        /* We write out the whole base header since the scrambled header
         * fields have changed as well */
        if(!m_pfs->FSeek(0))
            return FALSE;
        if(m_pfs->CbWrite(sizeof m_xih, (PBYTE)&m_xih) != sizeof m_xih)
            return FALSE;
        m_fSignatureChanged = FALSE;
    }

    return TRUE;
}

BOOL CXBEFile::FWriteToStm(CFileStmSeq *pfs, DWORD cbToWrite, DWORD cbBlk) const
{
    BYTE rgbBuf[4096];
    DWORD cb, cbT;

    if(!m_pfs || !pfs->FIsOpen())
        return FALSE;

    /* Write out the header */
    if(pfs->CbWrite(sizeof m_xih, (PBYTE)&m_xih) != sizeof m_xih)
        return FALSE;
    if(pfs->CbWrite(m_xih.SizeOfHeaders - sizeof m_xih, m_rgbHdr) !=
            m_xih.SizeOfHeaders - sizeof m_xih)
        return FALSE;

    /* Copy the rest of the data from the file */
    if(cbToWrite == 0)
        cbToWrite = m_cbFileSize;
    cb = cbToWrite - m_xih.SizeOfHeaders;
    if(!m_pfs->FSeek(m_xih.SizeOfHeaders))
        return FALSE;
    while(cb) {
        cbT = min(cb, sizeof rgbBuf);
        if(m_pfs->CbRead(cbT, rgbBuf) != cbT)
            return FALSE;
        if(pfs->CbWrite(cbT, rgbBuf) != cbT)
            return FALSE;
        cb -= cbT;
    }

    if(cbBlk) {
        /* Need to pad out the write to a block size.  Pad out with zeroes */
        memset(rgbBuf, 0, sizeof rgbBuf);
        cbT = cbToWrite % cbBlk;
        cbBlk -= cbT ? cbT : cbBlk;
        while(cbBlk) {
            cbT = min(cbBlk, sizeof rgbBuf);
            if(pfs->CbWrite(cbT, rgbBuf) != cbT)
                return FALSE;
            cbBlk -= cbT;
        }
    }

    return TRUE;
}

BOOL CXBEFile::FCalcChecksums(CCheckSum *pxsumBlk, CCheckSum *pxsumRed,
    CCheckSum *pxsumGrn) const
{
    BYTE rgbBuf[4096];
    DWORD cb, cbT;
    PXBEIMAGE_HEADER pxihNew;
    PXBEIMAGE_CERTIFICATE pxcertNew;

    if(!m_pfs)
        return FALSE;

    /* Make a copy of the header to use */
    pxihNew = (PXBEIMAGE_HEADER)_alloca(m_xih.SizeOfHeaders);
    memcpy(pxihNew, &m_xih, sizeof m_xih);
    memcpy(pxihNew + 1, m_rgbHdr, m_xih.SizeOfHeaders - sizeof m_xih);

    /* The green checksum includes everything */
    if(pxsumGrn)
        pxsumGrn->SumBytes((PBYTE)pxihNew, m_xih.SizeOfHeaders);

    /* The red checksum excludes the signature, the certificate keys,
     * and the scrambled header fields */
    memset(pxihNew->EncryptedDigest, 0, sizeof pxihNew->EncryptedDigest);
    pxcertNew = (PXBEIMAGE_CERTIFICATE)
        ((PBYTE)(pxihNew + 1) + ((PBYTE)m_pxcert - m_rgbHdr));
    memset(pxcertNew->SignatureKey, 0, sizeof pxcertNew->SignatureKey);
    memset(pxcertNew->LANKey, 0, sizeof pxcertNew->LANKey);
    memset(pxcertNew->AlternateSignatureKeys, 0,
        sizeof pxcertNew->AlternateSignatureKeys);
    pxihNew->AddressOfEntryPoint = NULL;
    pxihNew->XboxKernelThunkData = NULL;
    if(pxsumRed)
        pxsumRed->SumBytes((PBYTE)pxihNew, m_xih.SizeOfHeaders);

    /* The black checksum also excludes the certificate */
    memset(pxcertNew, 0, sizeof *pxcertNew);
    if(pxsumBlk)
        pxsumBlk->SumBytes((PBYTE)pxihNew, m_xih.SizeOfHeaders);

    /* Checksum the rest of the data from the file */
    cb = m_cbFileSize - m_xih.SizeOfHeaders;
    if(!m_pfs->FSeek(m_xih.SizeOfHeaders))
        return FALSE;
    while(cb) {
        cbT = min(cb, sizeof rgbBuf);
        if(m_pfs->CbRead(cbT, rgbBuf) != cbT)
            return FALSE;
        if(pxsumBlk)
            pxsumBlk->SumBytes(rgbBuf, cbT);
        if(pxsumRed)
            pxsumRed->SumBytes(rgbBuf, cbT);
        if(pxsumGrn)
            pxsumGrn->SumBytes(rgbBuf, cbT);
        cb -= cbT;
    }

    /* The green checksum must include the remainder of file data */
    if(pxsumGrn)
        while(cb = m_pfs->CbRead(sizeof rgbBuf, rgbBuf))
            pxsumGrn->SumBytes(rgbBuf, cb);

    return TRUE;
}
