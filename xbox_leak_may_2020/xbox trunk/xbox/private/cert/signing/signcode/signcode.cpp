/*
 *
 * signcode.cpp
 *
 * Acts like PRS's signcode, but signs XBEs
 *
 */

#include "precomp.h"

XBEIMAGE_CERTIFICATE certDefault = {
    // size
    sizeof(XBEIMAGE_CERTIFICATE),
    // timestamp
    0,
    // title ID
    0xFFFD0001,
    // title name
    { 0 },
    // alternate title IDs
    { 0 },
    // allowed media
    XBEIMAGE_MEDIA_TYPE_HARD_DISK,
    // region
    XBEIMAGE_GAME_REGION_MANUFACTURING,
    // ratings
    0xFFFFFFFF,
    // the rest is zero
};

BOOL FReplaceCertificate(CXBEFile *pxbe, CCryptContainer *pcc)
{
    const XBEIMAGE_CERTIFICATE *pxcert;

    pxcert = pxbe->GetCert();
    if(!pxcert)
        return NULL;

    certDefault.TimeDateStamp = time(NULL);
    wcsncpy(certDefault.TitleName, pxcert->TitleName,
        XBEIMAGE_TITLE_NAME_LENGTH);
    return CXBEFile::FGenNewKeys(&certDefault, pcc) &&
        pxbe->FInjectNewCertificate(&certDefault);
}

int __cdecl main(int argc, char **argv)
{
    CCryptContainer *pccSign = NULL;
    CXBEFile *pxbe = NULL;
    CDiskFile *pstm = NULL;
    LPCSTR szProvider = NULL;
    LPCSTR szContainer = NULL;
    LPCSTR szFileName = NULL;
    int cskip;
    int nRet = 1;

    ++argv;
    --argc;
    while(argc) {
        if(argv[0][0] == '-') {
            /* Options that we'll ignore, which have 1 parameter */
            if(0 == _stricmp(argv[0], "-spc") ||
                0 == _stricmp(argv[0], "-v") ||
                0 == _stricmp(argv[0], "-n") ||
                0 == _stricmp(argv[0], "-i") ||
                0 == _stricmp(argv[0], "-y") ||
                0 == _stricmp(argv[0], "-ky") ||
                0 == _stricmp(argv[0], "-$") ||
                0 == _stricmp(argv[0], "-a") ||
                0 == _stricmp(argv[0], "-t") ||
                0 == _stricmp(argv[0], "-tr") ||
                0 == _stricmp(argv[0], "-tw") ||
                0 == _stricmp(argv[0], "-j") ||
                0 == _stricmp(argv[0], "-jp") ||
                0 == _stricmp(argv[0], "-c") ||
                0 == _stricmp(argv[0], "-s") ||
                0 == _stricmp(argv[0], "-r") ||
                0 == _stricmp(argv[0], "-sp") ||
                0 == _stricmp(argv[0], "-cn") ||
                0 == _stricmp(argv[0], "-sha1"))
            {
                cskip = 1;
            } else if(0 == _stricmp(argv[0], "-x"))
                cskip = 0;
            else if(0 == _stricmp(argv[0], "-k")) {
                szContainer = argv[1];
                cskip = 1;
            } else if(0 == _stricmp(argv[0], "-xbekey")) {
                cskip = 0;
                szProvider = "nCipher Enhanced Cryptographic Provider";
                szContainer = "XBE1";
            } else if(0 == _stricmp(argv[0], "-p")) {
                szProvider = argv[1];
                cskip = 1;
            } else
                cskip = -1;
        } else {
            szFileName = argv[0];
            cskip = 0;
        }
        if(cskip < 0 || argc < cskip + 1) {
usage:
            fprintf(stderr, "usage: signcode -p provider -k container name\n");
            return 1;
        }
        argv += cskip + 1;
        argc -= cskip + 1;
    }

    if(!szContainer || !szProvider || !szFileName)
        goto usage;

    pccSign = CNCipherCrypt::PccCreate(szProvider, szContainer);
    if(!pccSign) {
        fprintf(stderr, "could not acquire crypto context\n");
        goto fatal;
    }
    pstm = new CDiskFile(szFileName, GENERIC_READ | GENERIC_WRITE);
    if(!pstm->FIsOpen()) {
        fprintf(stderr, "could not open %s\n", szFileName);
        goto fatal;
    }
    pxbe = new CXBEFile(pstm);
    pxbe->UseCryptContainer(new CDevkitCrypt);
    if(!pxbe->FVerifySignature()) {
        fprintf(stderr, "%s is not a valid XBE file\n", szFileName);
        goto fatal;
    }
    if(!FReplaceCertificate(pxbe, pccSign)) {
nosign:
        fprintf(stderr, "Could not sign %s\n", szFileName);
        goto fatal;
    }
    pxbe->UseCryptContainer(pccSign);
    if(!pxbe->FSignFile())
        goto nosign;
    if(!pxbe->FVerifySignature())
        goto nosign;
    if(!pxbe->FCommitChanges()) {
        fprintf(stderr, "could not write to %s\n", szFileName);
        goto fatal;
    }
    nRet = 0;

fatal:
    if(pxbe)
        delete pxbe;
    if(pstm)
        delete pstm;
    if(pccSign)
        delete pccSign;
    return nRet;
}
