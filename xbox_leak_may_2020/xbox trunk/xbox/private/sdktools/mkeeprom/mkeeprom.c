#include <windows.h>
typedef ULONG NTSTATUS;
#include <xcrypt.h>
#include <init.h>
#include <xconfig.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <sha.h>
#include <rc4.h>
#include "av.h"

#define XC_ENCFILE_SIG              'k2ne'
#define RC4_CONFOUNDER_LEN          8
#define HMAC_K_PADSIZE              64

typedef struct _RC4_SHA1_HEADER {
    UCHAR Checksum[A_SHA_DIGEST_LEN];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
} RC4_SHA1_HEADER, *PRC4_SHA1_HEADER;

typedef struct _XC_ENCFILE_HEADER
{
    DWORD Sig;
    RC4_SHA1_HEADER CryptHeader;
    DWORD MsgLen;
    UCHAR MsgDigest[A_SHA_DIGEST_LEN];
} XC_ENCFILE_HEADER, *PXC_ENCFILE_HEADER;

const UCHAR g_rgbDefEepromKey[] = {
    0x7b, 0x35, 0xa8, 0xb7, 0x27, 0xed, 0x43, 0x7a,
    0xa0, 0xba, 0xfb, 0x8f, 0xa4, 0x38, 0x61, 0x80,
};

void GetPasswd(LPSTR sz, int cchMax)
{
    char ch;
    int ich = 0;

    _cputs("Enter password:");
    for(;;) {
        ch = (char)_getch();
        switch(ch) {
        case 8:
            if(ich)
                --ich;
            break;
        case 10:
        case 13:
            sz[ich] = 0;
            _putch('\r');
            _putch('\n');
            return;
        default:
            if(ich < cchMax)
                sz[ich++] = ch;
            break;
        }
    }
}


void 
shaHmac(
    IN PBYTE pbKeyMaterial,
    IN ULONG cbKeyMaterial,
    IN PBYTE pbData,
    IN ULONG cbData,
    IN PBYTE pbData2,
    IN ULONG cbData2,
    OUT PBYTE HmacData // length must be A_SHA_DIGEST_LEN
    )
{
    BYTE Kipad[HMAC_K_PADSIZE];
    BYTE Kopad[HMAC_K_PADSIZE];
    BYTE HMACTmp[HMAC_K_PADSIZE+A_SHA_DIGEST_LEN];
    ULONG dwBlock;
    A_SHA_CTX shaHash;

    // truncate
    if (cbKeyMaterial > HMAC_K_PADSIZE)
        cbKeyMaterial = HMAC_K_PADSIZE;

    RtlZeroMemory(Kipad, HMAC_K_PADSIZE);
    RtlCopyMemory(Kipad, pbKeyMaterial, cbKeyMaterial);

    RtlZeroMemory(Kopad, HMAC_K_PADSIZE);
    RtlCopyMemory(Kopad, pbKeyMaterial, cbKeyMaterial);

    //
    // Kipad, Kopad are padded sMacKey. Now XOR across...
    //
    for(dwBlock=0; dwBlock<HMAC_K_PADSIZE/sizeof(DWORD); dwBlock++)
    {
        ((DWORD*)Kipad)[dwBlock] ^= 0x36363636;
        ((DWORD*)Kopad)[dwBlock] ^= 0x5C5C5C5C;
    }

    //
    // prepend Kipad to data, Hash to get H1
    //

    A_SHAInit(&shaHash);
    A_SHAUpdate(&shaHash, Kipad, HMAC_K_PADSIZE);
    if (cbData != 0)
    {
        A_SHAUpdate(&shaHash, pbData, cbData);
    }
    if (cbData2 != 0)
    {
        A_SHAUpdate(&shaHash, pbData2, cbData2);
    }

    // Finish off the hash
    A_SHAFinal(&shaHash,HMACTmp+HMAC_K_PADSIZE);

    // prepend Kopad to H1, hash to get HMAC
    RtlCopyMemory(HMACTmp, Kopad, HMAC_K_PADSIZE);

    // final hash: output value into passed-in buffer
    A_SHAInit(&shaHash);
    A_SHAUpdate(&shaHash,HMACTmp, sizeof(HMACTmp));
    A_SHAFinal(&shaHash,HmacData);
}


void 
rc4HmacEncrypt(
    IN PUCHAR confounder, // RC4_CONFOUNDER_LEN bytes
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    OUT PUCHAR pbHeader // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes
    )
{
    PRC4_SHA1_HEADER CryptHeader = (PRC4_SHA1_HEADER) pbHeader;
    BYTE LocalKey[A_SHA_DIGEST_LEN];
    RC4_KEYSTRUCT Rc4KeyStruct;

    //
    // Create the header - the confounder & checksum
    //
    RtlZeroMemory( CryptHeader->Checksum, A_SHA_DIGEST_LEN );
    RtlCopyMemory( CryptHeader->Confounder, confounder, RC4_CONFOUNDER_LEN );

    //
    // Checksum everything but the checksum
    //
    shaHmac( pbKey, cbKey,
             CryptHeader->Confounder, RC4_CONFOUNDER_LEN,
             pbInput, cbInput,
             CryptHeader->Checksum );

    //
    // HMAC the checksum into the key
    //
    shaHmac( pbKey, cbKey,
             CryptHeader->Checksum, A_SHA_DIGEST_LEN,
             NULL, 0,
             LocalKey );

    //
    // Use the generated key as the RC4 encryption key
    //
    rc4_key( &Rc4KeyStruct, A_SHA_DIGEST_LEN, LocalKey );

    //
    // Encrypt everything but the checksum
    //
    rc4( &Rc4KeyStruct, RC4_CONFOUNDER_LEN, CryptHeader->Confounder );
    rc4( &Rc4KeyStruct, cbInput, pbInput );
}


BOOL 
rc4HmacDecrypt(
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    IN PUCHAR pbHeader // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes
    )
{
    PRC4_SHA1_HEADER CryptHeader = (PRC4_SHA1_HEADER) pbHeader;
    RC4_SHA1_HEADER TempHeader;
    BYTE LocalKey[A_SHA_DIGEST_LEN];
    RC4_KEYSTRUCT Rc4KeyStruct;

    RtlCopyMemory( TempHeader.Confounder, CryptHeader->Confounder, RC4_CONFOUNDER_LEN );

    //
    // HMAC the checksum into the key
    //
    shaHmac( pbKey, cbKey,
             CryptHeader->Checksum, A_SHA_DIGEST_LEN,
             NULL, 0,
             LocalKey );

    //
    // Use the generated key as the RC4 encryption key
    //
    rc4_key( &Rc4KeyStruct, A_SHA_DIGEST_LEN, LocalKey );

    //
    // Decrypt confounder and data
    //
    rc4( &Rc4KeyStruct, RC4_CONFOUNDER_LEN, TempHeader.Confounder );
    rc4( &Rc4KeyStruct, cbInput, pbInput );

    //
    // Now verify the checksum.
    //
    shaHmac( pbKey, cbKey,
             TempHeader.Confounder, RC4_CONFOUNDER_LEN,
             pbInput, cbInput,
             TempHeader.Checksum );

    //
    // Decrypt is successful only if checksum matches
    //
    return ( RtlEqualMemory(
                 TempHeader.Checksum,
                 CryptHeader->Checksum,
                 A_SHA_DIGEST_LEN) );
}

#if 0
BOOL FGetKey(LPCSTR pszFN, PUCHAR pData, PULONG pLen)
{
    HANDLE hFile;
    DWORD dwBytes;
    DWORD dwEncSize;
    BOOL bRet = FALSE;
    char szPassword[128];

    hFile = CreateFile(pszFN, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        PBYTE pEncBuffer;
        BYTE PasswordDigest[XC_DIGEST_LEN];
        BYTE MessageDigest[XC_DIGEST_LEN];

        GetPasswd(szPassword, sizeof szPassword);

        dwEncSize = GetFileSize(hFile, NULL);

        if (dwEncSize > (4 + XC_DIGEST_LEN))
        {
            if(*pLen < dwEncSize - (4 + XC_DIGEST_LEN))
                goto done;
            *pLen = dwEncSize - (4 + XC_DIGEST_LEN);

            //
            // Allocate memory for encryption buffer
            //
            pEncBuffer = (PBYTE)malloc(dwEncSize);

            //
            // Read the file data into the encryption buffer
            //
            if (ReadFile(hFile, pEncBuffer, dwEncSize, &dwBytes, NULL))
            {
                //
                // Check signature
                //
                if (*((DWORD*)pEncBuffer) == KEYFILE_ENCFILE_SIG)
                {
                    //
                    // Calculate a digest from the password
                    //
                    XCCalcDigest((PBYTE)szPassword, strlen(szPassword), PasswordDigest);

                    //
                    // Decrypt the message (includeding message digest) with the password digest
                    //
                    XCSymmetricEncDec((pEncBuffer + 4), *pLen + XC_DIGEST_LEN,
                        PasswordDigest, XC_DIGEST_LEN);

                    //
                    // Copy the plain text message
                    //
                    memcpy(pData, (pEncBuffer + 4 + XC_DIGEST_LEN), *pLen);

                    //
                    // Calculate message digest
                    //
                    XCCalcDigest(pData, *pLen, MessageDigest);

                    //
                    // Compare the calculated message digest with the decrypted digest
                    //
                    bRet = memcmp(MessageDigest, (pEncBuffer + 4),
                        XC_DIGEST_LEN) == 0;
                }
            }

            //
            // Free memory for encryption buffer
            free(pEncBuffer);
        }
done:
        CloseHandle(hFile);
    }

    return bRet;
}
#endif

BOOL FGetKey(LPCSTR pszFN, PUCHAR pMsgBuf, PULONG pLen)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwBytes;
    BOOL bRet = FALSE;
    DWORD dwEncSize;
    PBYTE pEncBuffer = FALSE;
    PXC_ENCFILE_HEADER pEncHead;
    BYTE CalcDigest[XC_DIGEST_LEN];
    char szPassword[128];

    hFile = CreateFile(pszFN, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
//        ERROR_OUT("Cannot open from encrypted file %s", pszFN);
        goto CleanupAndExit;
    }

    GetPasswd(szPassword, sizeof szPassword);

    dwEncSize = GetFileSize(hFile, NULL);
    if (dwEncSize < sizeof(XC_ENCFILE_HEADER)) {
//        ERROR_OUT("Invalid encrypted file format %s", pszFN);
        goto CleanupAndExit;
    }

    pEncBuffer = (PBYTE)malloc(dwEncSize);
    pEncHead = (PXC_ENCFILE_HEADER)pEncBuffer;

    if (!ReadFile(hFile, pEncBuffer, dwEncSize, &dwBytes, NULL)) {
//        ERROR_OUT("Cannot read from file %s", pszFN);
        goto CleanupAndExit;
    }

    if (pEncHead->Sig != XC_ENCFILE_SIG || pEncHead->MsgLen != (dwEncSize - sizeof(XC_ENCFILE_HEADER))) {
//        ERROR_OUT("File %s is not an encrypted file", pszFN);
        goto CleanupAndExit;
    }
            
    
    if (*pLen < pEncHead->MsgLen) {
//        ERROR_OUT("Not enough space allocated to load the file");                        
        goto CleanupAndExit;
    }
    *pLen = pEncHead->MsgLen;
    
    //
    // Decrypt it
    //
    if (rc4HmacDecrypt((PBYTE)szPassword, strlen(szPassword), (pEncBuffer + sizeof(XC_ENCFILE_HEADER)), 
        pEncHead->MsgLen, (PBYTE)&(pEncHead->CryptHeader))) {

        //
        // Copy the plain text message
        //
        memcpy(pMsgBuf, (pEncBuffer + sizeof(XC_ENCFILE_HEADER)), pEncHead->MsgLen);

        //
        // Calculate message digest
        //
        XCCalcDigest(pMsgBuf, *pLen, CalcDigest);

        //
        // Compare the calculated message digest with the decrypted digest
        //
        if (memcmp(CalcDigest, pEncHead->MsgDigest, XC_DIGEST_LEN) == 0) {

            bRet = TRUE;
        
        } 
    }
    
    if (bRet == FALSE) {
//        ERROR_OUT("Decryption of file %s failed--invalid password", pszFN);
    }
    


CleanupAndExit:
    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);        
    }

    if (pEncBuffer != NULL) {
        free(pEncBuffer);
    }

    if (bRet) {
//        TRACE_OUT(TRACE_ALWAYS, "Successfully read encrypted file %s", pszFN);
    }

    return bRet;
}



int __cdecl main(int argc, char **argv)
{
    FILE *pfl;
    FILE *DecryptSrcFile;
    EEPROM_LAYOUT eep;
    XBOX_ENCRYPTED_SETTINGS *penc;
    XBOX_FACTORY_SETTINGS *pfac;
    XBOX_USER_SETTINGS *pusr;
    UCHAR rgbKey[XBOX_KEY_LENGTH];
    UCHAR rgbkey2[XC_SERVICE_DIGEST_SIZE];
    UCHAR rgbBigKey[76];
    ULONG cb;
    FILETIME ft;
    BOOL fLockHD = FALSE;
    BOOL fLockHDZeroKey = FALSE;
    BOOL fUseDefEepromKey = FALSE;
    ULONG ulGameRegion = XC_GAME_REGION_NA;
    LPCSTR szMACAddr = NULL;
    LPCSTR szDecryptFileName = NULL;
    int i;
    ULONG ul;
    ULONG ulAVRegion = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
        

    if(argc < 2) {
        fprintf(stderr, "usage: mkeeprom [-na] [-namanuf] [-lockhd] [-lockhdz] [-defkey] [-mac addr] [-dec srcfile.bin] eeprom.bin [key.xck]\n\n\n");
        fprintf(stderr, " -na        North American game region\n");
        fprintf(stderr, " -namanuf   North American + Manufacturing game regions\n");
        fprintf(stderr, " -ja        Japan game region + NTSC-J\n");
        fprintf(stderr, " -row       Rest of the world game region + PAL-I + 50Hz\n");
        fprintf(stderr, " -lockhd    Lock HD with sequential key\n");
        fprintf(stderr, " -lockhdz   Lock HD with zero key\n");
        fprintf(stderr, " -defkey    Lock EEPROM with default devkit key\n");
        fprintf(stderr, " -mac       Embed the specified MAC address 'addr'\n");
        fprintf(stderr, " -dec       Decrypt the EEPROM file 'srcfile.bin' and write as 'eeprom.bin'\n");

        return 1;
    }

    while(argc >= 2 && argv[1][0] == '-') {
        ++argv;
        --argc;
        if(0 == _stricmp(*argv, "-na"))
            ulGameRegion = XC_GAME_REGION_NA;
        if(0 == _stricmp(*argv, "-ja")) {
            ulGameRegion = XC_GAME_REGION_JAPAN;
            ulAVRegion = AV_STANDARD_NTSC_J | AV_FLAGS_60Hz;
        } else if(0 == _stricmp(*argv, "-row")) {
            ulGameRegion = XC_GAME_REGION_RESTOFWORLD;
            ulAVRegion = AV_STANDARD_PAL_I | AV_FLAGS_50Hz;
        } else if(0 == _stricmp(*argv, "-testrgn"))
            ulGameRegion = XC_GAME_REGION_INTERNAL_TEST;
        else if(0 == _stricmp(*argv, "-namanuf"))
            ulGameRegion = XC_GAME_REGION_MANUFACTURING | XC_GAME_REGION_NA;
        else if(0 == _stricmp(*argv, "-lockhd"))
            fLockHD = TRUE;
        else if(0 == _stricmp(*argv, "-lockhdz"))
            fLockHDZeroKey = TRUE;
        else if(0 == _stricmp(*argv, "-mac")) {
            if(argc < 2) {
badmac:
                fprintf(stderr, "MAC address is invalid\n");
                return 1;
            }
            szMACAddr = *++argv;
            --argc;
        } else if(0 == _stricmp(*argv, "-dec")) {
            szDecryptFileName = *++argv;
            --argc;
                
        } else if(0 == _stricmp(*argv, "-defkey"))
            ++fUseDefEepromKey;
    }

    //
    // Print out some current settings
    //
    printf("Game Region: 0x%08x\n", ulGameRegion);
    printf("AV Region:   0x%08x\n", ulAVRegion);


    memset(&eep, 0, sizeof eep);
    penc = (XBOX_ENCRYPTED_SETTINGS *)eep.EncryptedSection;
    penc->GameRegion = ulGameRegion;
    if(fLockHD)
        memcpy(penc->HDKey, "\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017", 16);

    if (fLockHDZeroKey) {
        memset(penc->HDKey, 0, 16);
        fLockHD = TRUE;
    }

    pfac = (XBOX_FACTORY_SETTINGS *)eep.FactorySection;
    pfac->AVRegion = ulAVRegion;
    if(szMACAddr) {
        for(i = 0; i < sizeof pfac->EthernetAddr; ++i) {
            if(!szMACAddr[0] || !szMACAddr[1])
                goto badmac;
            if(0 == sscanf(szMACAddr, "%02X", &ul))
                goto badmac;
            pfac->EthernetAddr[i] = (UCHAR)ul;
            szMACAddr += 2;
        }
    }
    pfac->Checksum = ~XConfigChecksum(pfac, sizeof *pfac);
    
    
    pusr = (XBOX_USER_SETTINGS *)eep.UserConfigSection;



    pusr->Checksum = ~XConfigChecksum(pusr, sizeof *pusr);

    pfl = fopen(argv[1], "wb");
    if(!pfl) {
        fprintf(stderr, "could not open %s\n", argv[1]);
        return 1;
    }

    if(argc > 2) {
        cb = sizeof rgbBigKey;
        if(!FGetKey(argv[2], rgbBigKey, &cb) || cb != sizeof rgbBigKey ||
            *(PUSHORT)rgbBigKey != 3)
        {
            fprintf(stderr, "unable to read key\n");
            return 1;
        }
        memcpy(rgbKey, rgbBigKey + 52, 16);
        cb = 16;
    } else if(fUseDefEepromKey) {
        memcpy(rgbKey, g_rgbDefEepromKey, 16);
        cb = 16;
    } else
        cb = 0;

    if(cb) {
        GetSystemTimeAsFileTime(&ft);
        XCSymmetricEncDec(penc->Confounder, sizeof penc->Confounder,
            (PUCHAR)&ft, sizeof ft);
        
        XcHMAC(rgbKey, XBOX_KEY_LENGTH, penc->Confounder,
            sizeof penc->Confounder, penc->HDKey, sizeof penc->HDKey +
            sizeof penc->GameRegion, penc->Checksum);
        
        XcHMAC(rgbKey, XBOX_KEY_LENGTH, penc->Checksum,
            XC_SERVICE_DIGEST_SIZE, NULL, 0, rgbkey2);
        
        XCSymmetricEncDec(penc->Confounder, sizeof *penc -
            sizeof penc->Checksum, rgbkey2, sizeof rgbkey2);
    }

    if (szDecryptFileName != NULL) {
        //
        // Open and read the contents of the source file
        //

        DecryptSrcFile = fopen(szDecryptFileName, "rb");
        
        if(!DecryptSrcFile) {
            fprintf(stderr, "could not open %s\n", szDecryptFileName);
            return 1;
        }

        fread(&eep, sizeof(eep), 1, DecryptSrcFile);

        //
        // Decrypt the section
        //

        if (!rc4HmacDecrypt(rgbKey, XBOX_KEY_LENGTH, penc->HDKey, sizeof(penc->HDKey) + sizeof(penc->GameRegion), penc->Checksum)) {
            fprintf(stderr, "could not decrypt %s\n", szDecryptFileName);
        }
        
        fclose(DecryptSrcFile);
    } 

    memset(rgbKey, 0, sizeof rgbKey);
    memset(rgbkey2, 0, sizeof rgbkey2);

    fwrite(&eep, sizeof eep, 1, pfl);
    fclose(pfl);

    return 0;
}
