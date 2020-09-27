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

class CMemFile
{
public:
    static CMemFile *PmfCreate(LPCSTR szName, BOOL fWrite,
        DWORD dwMaxCreateSize=0);
    operator unsigned char *()
    {
        return m_pb;
    }
    //unsigned char &operator[](int i) { return m_pb[i]; }
    DWORD CbSize(void) const { return m_cb; }
    void SetDeleteOnClose(BOOL f) { m_fDelete = f; }
    BOOL FClose(void);
    ~CMemFile()
    {
        if(m_pb)
            FClose();
    }

private:
    CMemFile(void) {}
    HANDLE m_hFile;
    HANDLE m_hMapping;
    PUCHAR m_pb;
    DWORD m_cb;
    LPCSTR m_szName;
    BOOL m_fDelete;
};

CMemFile *CMemFile::PmfCreate(LPCSTR szName, BOOL fWrite, DWORD dwMaxCreateSize)
{
    HANDLE hFile;
    HANDLE hMapping;
    PUCHAR pb;
    BYTE rgb[8192];
    DWORD cb;
    DWORD cbT;
    CMemFile *pmf;

    if(fWrite) {
        hFile = CreateFile(szName, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
            dwMaxCreateSize ? CREATE_ALWAYS : OPEN_EXISTING, 0, NULL);
    } else {
        hFile = CreateFile(szName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0,
            NULL);
    }

    if(INVALID_HANDLE_VALUE == hFile) {
openerr:
        fprintf(stderr, "error: could not open %s\n", szName);
        return NULL;
    }

    if(dwMaxCreateSize) {
        /* First need to fill with zeroes */
        memset(rgb, 0, sizeof rgb);
        cb = dwMaxCreateSize;
        while(cb) {
            cbT = sizeof rgb;
            if(cb < cbT)
                cbT = cb;
            if(!WriteFile(hFile, rgb, cbT, &cbT, NULL)) {
                fprintf(stderr, "error: could not write to %s\n", szName);
                CloseHandle(hFile);
                return NULL;
            }
            cb -= cbT;
        }
    }

    hMapping = CreateFileMapping(hFile, NULL, fWrite ? PAGE_READWRITE :
        PAGE_READONLY, 0, dwMaxCreateSize, NULL);

    if(!hMapping) {
        CloseHandle(hFile);
        goto openerr;
    }

    pb = (PBYTE)MapViewOfFile(hMapping, fWrite ? FILE_MAP_WRITE : FILE_MAP_READ,
        0, 0, 0);

    if(!pb) {
        CloseHandle(hFile);
        CloseHandle(hMapping);
        goto openerr;
    }

    pmf = new CMemFile;
    pmf->m_hFile = hFile;
    pmf->m_hMapping = hMapping;
    pmf->m_pb = pb;
    pmf->m_cb = GetFileSize(hFile, NULL);
    pmf->m_szName = _strdup(szName);
    pmf->m_fDelete = FALSE;
    return pmf;
}

BOOL CMemFile::FClose(void)
{
    BOOL f;

    if(!m_pb)
        return TRUE;
    if(!UnmapViewOfFile(m_pb))
        fprintf(stderr, "error: could not write to %s\n", m_szName);
    else
        m_fDelete = FALSE;
    m_pb = NULL;
    CloseHandle(m_hMapping);
    CloseHandle(m_hFile);
    if(m_fDelete) {
        DeleteFile(m_szName);
        f = FALSE;
    } else
        f = TRUE;
    free((PVOID)m_szName);
    return f;
}

BOOL FComputeROMHash(CCryptContainer *pcc, PBYTE pbInitBase, PBYTE pbInitTop,
    PBYTE pbKernelBase, PBYTE pbSignature)
{
    return
        pcc->FStartProgressiveHash() &&
        pcc->FProgressiveHashData(pbSignature + 0x100, 0x80) &&
        pcc->FProgressiveHashData(pbInitBase, pbInitTop - pbInitBase) &&
        pcc->FProgressiveHashData(pbKernelBase, pbSignature - pbKernelBase);
};

BOOL FSignROM(LPCSTR szIn, LPCSTR szOut)
{
    CMemFile *pmfIn, *pmfOut;
    PBYTE pbNewBase;
    PBYTE pbInitBase;
    PBYTE pbInitTop;
    PBYTE pbKernelBase;
    PBYTE pbSignature;
    PDWORD pbRomSizes;
    DWORD dwRomBase;
    DWORD dwRomSize;
    int cCopies;

    if(0 == strcmp(szOut, "."))
        szOut = NULL;

    /* Map in the input ROM */
    pmfIn = CMemFile::PmfCreate(szIn, szOut == NULL);
    if(!pmfIn) {
        fprintf(stderr, "error: cannot open ROM file %s\n", szIn);
        return FALSE;
    }

    /* We can't do much validation on this ROM, so we'll just make sure the
     * sizes look reasonable and that it's correctly signed */
    pbRomSizes = (PULONG)(*pmfIn + pmfIn->CbSize() - ROM_DEC_SIZE - 0x80);
    dwRomBase = pbRomSizes[0];
    /* Ensure we're 256k aligned */
    if(dwRomBase & 0x3FFFF) {
badrom:
        fprintf(stderr, "error: invalid ROM file %s\n", szIn);
        delete pmfIn;
        return FALSE;
    }

    /* Ensure that we have an integral number of copies of this ROM */
    dwRomSize = (DWORD)-(int)dwRomBase;
    cCopies = pmfIn->CbSize() / dwRomSize;
    if(cCopies * dwRomSize != pmfIn->CbSize())
        goto badrom;

    /* Find the top of the inittbl, the base of the kernel, and the signature,
     * and make sure they're all in reasonable places */
    pbInitBase = &(*pmfIn)[pmfIn->CbSize() - dwRomSize];
    pbInitTop = pbInitBase + pbRomSizes[1];
    pbKernelBase = pbInitBase + (pbRomSizes[2] - dwRomBase);
    pbSignature = pbInitBase + (dwRomSize - ROM_DEC_SIZE - 0x180);

    if(pbInitTop < pbInitBase || pbKernelBase < pbInitTop || pbSignature <
            pbKernelBase)
        goto badrom;

    /* Hash the data to verify the signature */
    if(!FComputeROMHash(&ccDev, pbInitBase, pbInitTop, pbKernelBase,
            pbSignature))
        goto badrom;
    if(!ccDev.FVerifyEncryptedProgressiveHash(pbSignature))
        goto badrom;

    /* Now map in the output ROM */
    if(szOut) {
        pmfOut = CMemFile::PmfCreate(szOut, TRUE, dwRomSize * cCopies);
        if(!pmfOut)
            goto cantwrite;
        pmfOut->SetDeleteOnClose(TRUE);
        pbNewBase = &(*pmfOut)[dwRomSize * (cCopies - 1)];
        memcpy(pbNewBase, pbInitBase, dwRomSize);
        pbInitTop = &pbNewBase[pbInitTop - pbInitBase];
        pbKernelBase = &pbNewBase[pbKernelBase - pbInitBase];
        pbSignature = &pbNewBase[pbSignature - pbInitBase];
        pbInitBase = pbNewBase;
    } else {
        szOut = szIn;
        pmfOut = pmfIn;
    }

    /* We're going to need to ensure that the init table we see at runtime is
     * the same one that the romdec parsed, so we'll change its address */
    *(PULONG)(pbSignature + 0x100) = 0xFFF00000;

    /* Sign the ROM */
    if(!FComputeROMHash(pccSign, pbInitBase, pbInitTop, pbKernelBase,
        pbSignature))
    {
writeerr:
        delete pmfOut;
cantwrite:
        fprintf(stderr, "error: could not write to %s\n", szOut);
        delete pmfIn;
        return FALSE;
    }
    if(!pccSign->FEncryptProgressiveHash(pbSignature))
        goto writeerr;

    if(pmfOut != pmfIn) {
        /* We need to replicate the signed ROM */
        while(--cCopies) {
            pbInitBase -= dwRomSize;
            memcpy(pbInitBase, pbNewBase, dwRomSize);
        }
    } else {
        /* We need to replicate the signature */
        pbInitBase = pbSignature;
        while(--cCopies) {
            pbSignature -= dwRomSize;
            memcpy(pbSignature, pbInitBase, 0x100);
        }
    }

    /* Write out the final bits */
    if(!pmfOut->FClose())
        goto cantwrite;

    /* Now we're done! */
    if(pmfOut != pmfIn)
        delete pmfOut;
    delete pmfIn;
    return TRUE;
}

int __cdecl main(int argc, char **argv)
{
    LPCSTR szOut;
    char szLine[1024];
    char szXbeIn[512];
    char szXbeOut[512];
    char szID[512];
    FILE *pfl;
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
            ++argv;
            --argc;
        }
    }

    if(argc < 2) {
usage:
        fprintf(stderr, "usage: romsign input-file output-file\n"
            "\tor romsign @filelist\n");
        goto fatal;
    }

    if(argv[1][0] == '@') {
        pfl = fopen(&argv[1][1], "r");
        if(!pfl) {
            fprintf(stderr, "error: could not open input file %s\n",
                &argv[1][1]);
            goto fatal;
        }
    } else if(argc < 3)
        goto usage;
    else
        pfl = NULL;

    if(!pccSign) {
        /* Acquire the crypt context */
#if 0
        pfk = CFloppyCrypt::PccCreate(TRUE);
        if(!pfk)
#endif
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
            if(sscanf(szLine, "%s %s", szXbeIn, szXbeOut) < 2)
                fprintf(stderr, "warning: unable to parse: %s\n", szLine);
            else if(*szXbeIn != '#') {
                fprintf(stderr, "from %s to %s\n", szXbeIn, szXbeOut);
                if(!FSignROM(szXbeIn, szXbeOut)) {
                    fprintf(stderr, "..FAILED!\n");
                    f = FALSE;
                } else
                    fprintf(stderr, "..Succeeded!\n");
            }
        }
    } else
        f = FSignROM(argv[1], argv[2]);

fatal:
    if(pfk != pccSign)
        delete pfk;
    if(pccSign)
        delete pccSign;

    return !f;
}
