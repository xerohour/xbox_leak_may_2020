/*++

Copyright (c) Microsoft Corporation

Module Name:

    enc.cpp

Abstract:


--*/

#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <rsa.h>
#include <sha.h>
#include <rc4.h>
#include <xcrypt.h>
#include "key.h"
#include "xcmain.h"
#include "enc.h"



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



BOOL
XcWriteEncryptedFile(
    PBYTE pMessage,
    DWORD cbMsgLen,
    LPCSTR pszFN,
    LPCSTR pszPassword,
    PBYTE pConfounder  // RC4_CONFOUNDER_LEN bytes
    )
{
    HANDLE hFile;
    DWORD dwBytes;
    BOOL bRet = FALSE;

    hFile = CreateFile(pszFN, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile != INVALID_HANDLE_VALUE) {
        
        DWORD dwEncSize = sizeof(XC_ENCFILE_HEADER) + cbMsgLen;
        PBYTE pEncBuffer = (PBYTE)malloc(dwEncSize);
        PXC_ENCFILE_HEADER pEncHead = (PXC_ENCFILE_HEADER)pEncBuffer;

        pEncHead->Sig = XC_ENCFILE_SIG;
        pEncHead->MsgLen = cbMsgLen;

        XCCalcDigest(pMessage, cbMsgLen, pEncHead->MsgDigest);

        //
        // Copy the message data in
        //
        memcpy((pEncBuffer + sizeof(XC_ENCFILE_HEADER)), pMessage, cbMsgLen);

        //
        // Encrypt it
        //
        rc4HmacEncrypt(pConfounder, (PBYTE)pszPassword, strlen(pszPassword), 
            (pEncBuffer + sizeof(XC_ENCFILE_HEADER)), cbMsgLen, (PBYTE)&(pEncHead->CryptHeader)); 

        //
        // Write the buffer out
        //
        if (WriteFile(hFile, pEncBuffer, dwEncSize, &dwBytes, NULL)) {
            bRet = TRUE;
        } else {
            ERROR_OUT("Cannot write to key file %s", pszFN);
        }

        CloseHandle(hFile);

        //
        // Free the buffer used for encryption
        //
        free(pEncBuffer);
    
    } else {
        ERROR_OUT("Cannot create encrypted file %s", pszFN);
    }

    if (bRet) {
        TRACE_OUT(TRACE_ALWAYS, "Encrypted file %s created successfully", pszFN);
    }

    return bRet;
}



BOOL 
XcReadEncryptedFile(
    LPCSTR pszFN, 
    LPCSTR pszPassword,
    PBYTE pMsgBuf,
    DWORD cbMsgBufLen
    )
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwBytes;
    BOOL bRet = FALSE;
    DWORD dwEncSize;
    PBYTE pEncBuffer = FALSE;
    PXC_ENCFILE_HEADER pEncHead;
    BYTE CalcDigest[XC_DIGEST_LEN];

    hFile = CreateFile(pszFN, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        ERROR_OUT("Cannot open from encrypted file %s", pszFN);
        goto CleanupAndExit;
    }

    dwEncSize = GetFileSize(hFile, NULL);
    if (dwEncSize < sizeof(XC_ENCFILE_HEADER)) {
        ERROR_OUT("Invalid encrypted file format %s", pszFN);
        goto CleanupAndExit;
    }

    pEncBuffer = (PBYTE)malloc(dwEncSize);
    pEncHead = (PXC_ENCFILE_HEADER)pEncBuffer;

    if (!ReadFile(hFile, pEncBuffer, dwEncSize, &dwBytes, NULL)) {
        ERROR_OUT("Cannot read from file %s", pszFN);
        goto CleanupAndExit;
    }

    if (pEncHead->Sig != XC_ENCFILE_SIG || pEncHead->MsgLen != (dwEncSize - sizeof(XC_ENCFILE_HEADER))) {
        ERROR_OUT("File %s is not an encrypted file", pszFN);
        goto CleanupAndExit;
    }
            
    
    if (cbMsgBufLen != pEncHead->MsgLen) {
        ERROR_OUT("Not enough space allocated to load the file");                        
        goto CleanupAndExit;
    }
    
    //
    // Decrypt it
    //
    if (rc4HmacDecrypt((PBYTE)pszPassword, strlen(pszPassword), (pEncBuffer + sizeof(XC_ENCFILE_HEADER)), 
        pEncHead->MsgLen, (PBYTE)&(pEncHead->CryptHeader))) {

        //
        // Copy the plain text message
        //
        memcpy(pMsgBuf, (pEncBuffer + sizeof(XC_ENCFILE_HEADER)), pEncHead->MsgLen);

        //
        // Calculate message digest
        //
        XCCalcDigest(pMsgBuf, cbMsgBufLen, CalcDigest);

        //
        // Compare the calculated message digest with the decrypted digest
        //
        if (memcmp(CalcDigest, pEncHead->MsgDigest, XC_DIGEST_LEN) == 0) {

            bRet = TRUE;
        
        } 
    }
    
    if (bRet == FALSE) {
        ERROR_OUT("Decryption of file %s failed--invalid password", pszFN);
    }
    


CleanupAndExit:
    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);        
    }

    if (pEncBuffer != NULL) {
        free(pEncBuffer);
    }

    if (bRet) {
        TRACE_OUT(TRACE_ALWAYS, "Successfully read encrypted file %s", pszFN);
    }

    return bRet;
}


BOOL 
XcGetEncryptedFileMsgLen(
    LPCSTR pszFN,
    PDWORD pdwSize
    )
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    BOOL bRet = FALSE;
    DWORD dwBytes;
    XC_ENCFILE_HEADER EncHead;

    hFile = CreateFile(pszFN, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        goto CleanupAndExit;
    }

    if (!ReadFile(hFile, &EncHead, sizeof(EncHead), &dwBytes, NULL) || dwBytes != sizeof(EncHead)) {
        goto CleanupAndExit;
    }

    if (EncHead.Sig == XC_ENCFILE_SIG) {
        bRet = TRUE;
        *pdwSize = EncHead.MsgLen;
    }


CleanupAndExit:
    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);        
    }

    return bRet;
}
 
 
