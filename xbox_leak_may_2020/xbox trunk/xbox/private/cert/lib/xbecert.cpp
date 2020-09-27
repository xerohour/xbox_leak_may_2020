/*
 *
 * xbecert.cpp
 *
 * CXBEFile certificate stuff
 *
 */

#include "precomp.h"

#define CERTFIELDDESC(field) \
    FIELD_OFFSET(XBEIMAGE_CERTIFICATE, field), \
        sizeof (((PXBEIMAGE_CERTIFICATE)0)->field)

#define CERTFIELD(field) \
    { #field, CERTFIELDDESC(field) }

static struct CFI {
    LPCSTR szKey;
    DWORD ibOffset;
    DWORD cbSize;
} rgcfi[] = {
    CERTFIELD(TitleID),
    CERTFIELD(AllowedMediaTypes),
    CERTFIELD(GameRatings),
    CERTFIELD(GameRegion),
    CERTFIELD(Version),
    CERTFIELD(DiskNumber),
    //{ "AltTitleCount", CERTFIELDDESC(Reserved2) },
    { NULL, 0, 0 }
};

BOOL CXBEFile::FUnpackCertificate(const CVarBag *pbag, PXBEIMAGE_CERTIFICATE pcert)
{
    CFI *pcfi;
    PBYTE pb;
    BYTE b;
    int ich;
    DWORD itid;
    DWORD ctid;
    DWORD dw;
    LPCSTR sz;
    char szT[64];

    if(!pbag)
        return FALSE;

    memset(pcert, 0, sizeof *pcert);
    pcert->SizeOfCertificate = sizeof *pcert;
    pcert->TimeDateStamp = time(NULL);

    /* Extract the title name, if present */
    /* TODO: allow unicode */
    sz = pbag->FindSzProp("TitleName");
    if(sz) {
        for(ich = 0; sz[ich] && ich < XBEIMAGE_TITLE_NAME_LENGTH; ++ich)
            pcert->TitleName[ich] = sz[ich];
    }

    /* Extract the fields we need */
    for(pcfi = rgcfi; pcfi->szKey; ++pcfi) {
        /* If the field is missing, we fail */
        if(!pbag->FFindDwProp(pcfi->szKey, &dw))
            return FALSE;
        /* If the data is too big for this field, we fail */
        b = 0;
        pb = (PBYTE)&dw + pcfi->cbSize;
        while(pb < (PBYTE)(&dw+1))
            b |= *pb++;
        if(b)
            return FALSE;

        /* Capture this data into this certificate */
        memcpy((PBYTE)pcert + pcfi->ibOffset, &dw, pcfi->cbSize);
    }

    if(!pbag->FFindDwProp("AltTitleCount", &ctid))
        ctid = 0;
    if(ctid > XBEIMAGE_ALTERNATE_TITLE_ID_COUNT)
        return FALSE;
    for(itid = 0; itid < ctid; ++itid) {
        sprintf(szT, "AltTitleID%d", itid);
        if(!pbag->FFindDwProp(szT, &pcert->AlternateTitleIDs[itid]))
            return FALSE;
    }

    return TRUE;
}

BOOL CXBEFile::FInjectNewCertificate(const CVarBag *pbag)
{
    XBEIMAGE_CERTIFICATE certNew;

    if(!m_pfs)
        return FALSE;

    if(!FUnpackCertificate(pbag, &certNew))
        return FALSE;

    /* Timestamp this injection */
    certNew.TimeDateStamp = time(NULL);

    /* Keep the old keys */
    memcpy(certNew.LANKey, m_pxcert->LANKey, sizeof certNew.LANKey);
    memcpy(certNew.SignatureKey, m_pxcert->SignatureKey,
        sizeof certNew.SignatureKey);
    
    return FInjectNewCertificate(&certNew);
}

BOOL CXBEFile::FInjectNewCertificate(const XBEIMAGE_CERTIFICATE *pcertNew)
{
    DWORD cb;

    if(!m_pfs)
        return FALSE;

    if(pcertNew->SizeOfCertificate != sizeof *m_pxcert)
        return FALSE;

    /* Allow old-style certificates */
    if(m_pxcert->SizeOfCertificate != pcertNew->SizeOfCertificate) {
        cb = m_pxcert->SizeOfCertificate;
        memcpy(m_pxcert, pcertNew, cb);
        m_pxcert->SizeOfCertificate = cb;
    } else
        memcpy(m_pxcert, pcertNew, sizeof *m_pxcert);
    m_fCertChanged = TRUE;
    return TRUE;
}
