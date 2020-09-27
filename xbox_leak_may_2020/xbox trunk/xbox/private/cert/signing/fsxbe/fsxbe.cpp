/*
 *
 * fsxbe.cpp
 *
 * Sign an XBE using a floppy key
 *
 */

#include "precomp.h"

CDevkitCrypt ccDev;
CCryptContainer *pccSign;
CFloppyCrypt *pfk;
CVarBag *pbagCert;
BOOL fRedKey;

BOOL FSignXBE(LPCSTR szIn, LPCSTR szOut, LPBYTE rgbID)
{
    CXBEFile *pxbe;
    XBEIMAGE_CERTIFICATE cert;
    const XBEIMAGE_CERTIFICATE *pcert;
    CDiskFile stmIn(szIn, GENERIC_READ);
    CDiskFile *pstmOut;

    if(0 == strcmp(szOut, "."))
        szOut = NULL;

    /* Load the input XBE */
    if(!(szOut ? stmIn.FIsOpen() : stmIn.FReopen(GENERIC_READ |
        GENERIC_WRITE)))
    {
        fprintf(stderr, "error: cannot open XBE file %s\n", szIn);
        return FALSE;
    }
    pxbe = new CXBEFile(&stmIn);

    /* Validate the input XBE */
    pxbe->UseCryptContainer(&ccDev);
    if(!pxbe->FVerifySignature()) {
badxbe:
        fprintf(stderr, "error: invalid XBE file %s\n", szIn);
        return FALSE;
    }

    /* Construct a new certificate */
    pcert = pxbe->GetCert();
    if(!pcert)
        goto badxbe;
    if(pbagCert) {
        if(!CXBEFile::FUnpackCertificate(pbagCert, &cert)) {
            fprintf(stderr, "error: illegal certificate data\n");
            return FALSE;
        }
        if(!cert.TitleName[0])
            memcpy(cert.TitleName, pcert->TitleName, sizeof cert.TitleName);
    } else {
        memset(&cert, 0, sizeof cert);
        memcpy(&cert, pcert, pcert->SizeOfCertificate);
        cert.SizeOfCertificate = sizeof cert;
    }
    if(!fRedKey && !CXBEFile::FGenNewKeys(&cert, pccSign)) {
        fprintf(stderr, "error: could not generate certificate keys\n");
        return FALSE;
    }
    if(pfk) {
        /* Use the key ID for the signature key */
        memset(cert.SignatureKey, 0, sizeof cert.SignatureKey - 4);
        pfk->FGetKeyId(cert.SignatureKey);
        /* Use the input ID for the LAN key */
        memcpy(cert.LANKey, rgbID, sizeof cert.SignatureKey - 4);
    }

    /* Inject the certificate */
    if(!pxbe->FInjectNewCertificate(&cert)) {
cantsign:
        fprintf(stderr, "error: could not sign XBE file\n");
        return FALSE;
    }

    /* Sign the XBE */
    pxbe->UseCryptContainer(pccSign);
    if(!pxbe->FSignFile() || !pxbe->FVerifySignature())
        goto cantsign;

    if(szOut) {
        pstmOut = new CDiskFile(szOut, GENERIC_WRITE);
        if(!pstmOut->FIsOpen()) {
writeerr:
            fprintf(stderr, "error: could not write file %s\n", szOut);
            return FALSE;
        }
        if(!pxbe->FWriteToStm(pstmOut))
            goto writeerr;
        delete pstmOut;
    } else {
        szOut = szIn;
        if(!pxbe->FCommitChanges())
            goto writeerr;
    }

    return TRUE;
}

int __cdecl main(int argc, char **argv)
{
    LPCSTR szOut;
    BYTE rgbID[16];
    FILE *pfl;
    CIniFile cif;
    char szLine[1024];
    char szXbeIn[512];
    char szXbeOut[512];
    char szID[512];
    BOOL f = FALSE;

    while(argc >= 2 && argv[1][0] == '-') {
        if(0 == _stricmp(argv[1], "-nc")) {
            if(argc < 4) {
                fprintf(stderr, "-nc option requires provider and container\n");
                return 1;
            }
            pccSign = CNCipherCrypt::PccCreate(argv[2], argv[3]);
            if(!pccSign) {
                fprintf(stderr, "error: could not acquire nCipher context\n");
                return 1;
            }
            argv += 3;
            argc -= 3;
        } else if(0 == _stricmp(argv[1], "-redkey")) {
            pccSign = new CDevkitCrypt;
            --argc;
            ++argv;
            ++fRedKey;
        } else if(0 == _stricmp(argv[1], "-cert")) {
            if(argc < 3) {
                fprintf(stderr, "-cert option requires certificate file name\n");
                return 1;
            }
            if(!cif.FReadFile(argv[2], FALSE)) {
badcert:
                fprintf(stderr, "error: could not process certificate %s\n",
                    argv[2]);
                return 1;
            }
            pbagCert = cif.PbagRoot()->FindProp("XBE Certificate");
            if(!pbagCert)
                goto badcert;
            argv += 2;
            argc -= 2;
        }
    }

    if(argc < 2) {
usage:
        fprintf(stderr, "usage: sign [-cert certfile] input-xbe output-xbe xbeID\n"
            "   or: sign @filelist\n");
        goto fatal;
    }

    if(argv[1][0] == '@') {
        pfl = fopen(&argv[1][1], "r");
        if(!pfl) {
            fprintf(stderr, "error: could not open input file %s\n",
                &argv[1][1]);
            goto fatal;
        }
    } else if(argc < 4)
        goto usage;
    else
        pfl = NULL;

    if(!pccSign) {
        /* Acquire the crypt context */
        pfk = CFloppyCrypt::PccCreate(TRUE);
        if(!pfk)
            pfk = CFloppyCrypt::PccCreate();
        if(!pfk) {
            fprintf(stderr, "Unable to acquire signing keys\n");
            return 1;
        }
        pccSign = pfk;
    }

    if(pfl) {
        f = TRUE;
        while(fgets(szLine, sizeof szLine, pfl)) {
            if(sscanf(szLine, "%s %s %s", szXbeIn, szXbeOut, szID) < 3)
                fprintf(stderr, "warning: unable to parse: %s\n", szLine);
            else if(*szXbeIn != '#') {
                fprintf(stderr, "from %s to %s, ID %s\n", szXbeIn, szXbeOut,
                    szID);
                memset(rgbID, 0, sizeof rgbID);
                strncpy((LPSTR)rgbID, szID, sizeof rgbID);
                if(!FSignXBE(szXbeIn, szXbeOut, rgbID)) {
                    fprintf(stderr, "..FAILED!\n");
                    f = FALSE;
                } else
                    fprintf(stderr, "..Succeeded!\n");
            }
        }
    } else {
        memset(rgbID, 0, sizeof rgbID);
        strncpy((LPSTR)rgbID, argv[3], sizeof rgbID);
        f = FSignXBE(argv[1], argv[2], rgbID);
    }

fatal:
    if(pfk != pccSign)
        delete pfk;
    if(pccSign)
        delete pccSign;

    return !f;
}
