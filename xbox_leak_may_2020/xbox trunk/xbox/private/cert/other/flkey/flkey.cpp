/*
 *
 * flkey.cpp
 *
 * Encrypt a key set
 *
 */

#include "precomp.h"

int __cdecl main(int argc, char **argv)
{
    CFloppyCrypt *pfk;
    int ch;
    BOOL fWritePub = FALSE;
    BOOL fNCipher = FALSE;
    CNCipherCrypt *pcc = NULL;
    int argcMin = 2;

    for(;;) {
        if(argc < argcMin) {
            fprintf(stderr, "usage: flkey keyid\n");
            return 1;
        }

        if(argc == 1)
            break;
        else if(0 == _stricmp(argv[1], "-pub")) {
            if(!fWritePub)
                --argcMin;
            ++fWritePub;
            --argc;
            ++argv;
        } else if(0 == _stricmp(argv[1], "-nc")) {
            ++fNCipher;
            --argc;
            ++argv;
            argcMin = 4;
        } else
            break;
    }

    if(fNCipher) {
        pcc = CNCipherCrypt::PccCreate(argv[1], argv[2]);
        if(!pcc) {
noncipher:
            fprintf(stderr, "error: unable to acquire nCipher context\n");
            return 1;
        }
        argv += 2;
        pfk = CFloppyCrypt::PccImportKey(pcc);
        if(!pfk)
            goto noncipher;
    } else {
        _cputs("Enter the source floppy and press <return>");
        for(;;) {
            ch = _getch();
            if(ch == 10 || ch == 13)
                break;
        }
        _putch(13);
        _putch(10);

        /* Acquire the crypt context */
        /* For now, allow unsecure keys */
        pfk = CFloppyCrypt::PccCreate(TRUE);
        if(!pfk)
            pfk = CFloppyCrypt::PccCreate();
        if(!pfk) {
            fprintf(stderr, "Unable to acquire signing keys\n");
            return 1;
        }
    }

    _cputs("Enter the destination floppy and press <return>");
    for(;;) {
        ch = _getch();
        if(ch == 10 || ch == 13)
            break;
    }
    _putch(13);
    _putch(10);

    if(fWritePub) {
        if(!pfk->FWritePub()) {
            fprintf(stderr, "Unable to write public key\n");
            return 1;
        }
    } else {
        if(!pfk->FWrite(argv[1])) {
            fprintf(stderr, "Unable to write encrypted key\n");
            return 1;
        }
    }

    fprintf(stderr, "Key written successfully\n");

    delete pfk;
    if(pcc)
        delete pcc;

    return 0;
}
