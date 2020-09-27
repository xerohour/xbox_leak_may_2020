/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    key.cpp

Abstract:

    This module contains implementation of CKeyFile and CKeyPair class to handle
    public and private keys


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


//
// CKeyFile class
//


BOOL 
CKeyFile::CreatePubKeyPair(
    DWORD dwKeyBits
    )
{
    DWORD dwKeySize = dwKeyBits;
    DWORD dwPublicKeyBytes;
    DWORD dwPrivateKeyBytes;
    PBYTE pPublicKeyData;
    PBYTE pPrivateKeyData;

    Free();

    //
    // Calculate key size
    // BSsafeComputeKeySizes uses the 3rd parameter for internal calculation thus 
    // modifying the value >> 1.  This new value should be ignored.
    //
    if (!BSafeComputeKeySizes(&dwPublicKeyBytes, &dwPrivateKeyBytes, &dwKeySize))
    {
        ERROR_OUT("Cannot get keysizes because BSafeComputeKeySizes failed");
        return FALSE;
    }

    Alloc(sizeof(XBOX_KEYFILE_HEADER) + dwPublicKeyBytes + dwPrivateKeyBytes);

    KeyHeader()->KeyType = KEYFILE_PUBPAIR;
    KeyHeader()->KeyBits = (USHORT)dwKeyBits;
    KeyHeader()->PublicKeyBytes = dwPublicKeyBytes;
    KeyHeader()->PrivateKeyBytes = dwPrivateKeyBytes;

    pPublicKeyData = m_pData + sizeof(XBOX_KEYFILE_HEADER);
    pPrivateKeyData = pPublicKeyData + dwPublicKeyBytes;
    
    //
    // generate keys
    //
    if(!BSafeMakeKeyPair(
         (LPBSAFE_PUB_KEY)pPublicKeyData,
         (LPBSAFE_PRV_KEY)pPrivateKeyData,
         dwKeyBits)
         )
    {
        ERROR_OUT("Cannot generate keys because BSafeMakeKeyPair failed");
        return FALSE;
    }

    return TRUE;
}


BOOL 
CKeyFile::CreateSymKey(
    DWORD dwKeyBits
    )
{
    DWORD dwKeySize = dwKeyBits;
    DWORD dwPublicKeyBytes;
    DWORD dwPrivateKeyBytes;
    PBYTE pPublicKeyData;
    PBYTE pPrivateKeyData;

    Free();

    //
    // Calculate key size
    // BSsafeComputeKeySizes uses the 3rd parameter for internal calculation thus 
    // modifying the value >> 1.  This new value should be ignored.
    //
    if (!BSafeComputeKeySizes(&dwPublicKeyBytes, &dwPrivateKeyBytes, &dwKeySize))
    {
        ERROR_OUT("Cannot get keysizes because BSafeComputeKeySizes failed");
        return FALSE;
    }

    Alloc(sizeof(XBOX_KEYFILE_HEADER) + dwPublicKeyBytes);

    KeyHeader()->KeyType = KEYFILE_SYM;
    KeyHeader()->KeyBits = (USHORT)dwKeyBits;
    KeyHeader()->PublicKeyBytes = dwPublicKeyBytes;
    KeyHeader()->PrivateKeyBytes = 0;

    pPublicKeyData = m_pData + sizeof(XBOX_KEYFILE_HEADER);
    pPrivateKeyData = (PBYTE)malloc(dwPrivateKeyBytes);
    
    //
    // generate keys
    //
    if(!BSafeMakeKeyPair(
         (LPBSAFE_PUB_KEY)pPublicKeyData,
         (LPBSAFE_PRV_KEY)pPrivateKeyData,
         dwKeyBits)
         )
    {
        free(pPrivateKeyData);
        ERROR_OUT("Cannot generate keys because BSafeMakeKeyPair failed");
        return FALSE;
    }
    free(pPrivateKeyData);

    return TRUE;
}

BOOL 
CKeyFile::CreateSymKey2(
    )
{
    BYTE rgbHash[20];
    FILETIME ft;
    BYTE rgbKeys[32];
    struct RC4_KEYSTRUCT RC4key;
    A_SHA_CTX   SHAHash;
    char ch;

    CreateSymKey(128);

    
    A_SHAInit(&SHAHash);
    GetSystemTimeAsFileTime(&ft);
    A_SHAUpdate(&SHAHash, (PBYTE)&ft, sizeof ft);
    _cputs("Enter random data to create a key:");
    do {
        ch = (char)_getch();
        _asm {
            lea ecx, ft
            rdtsc
            mov [ecx], eax
            mov [ecx+4], edx
        }
        A_SHAUpdate(&SHAHash, (PBYTE)&ft, sizeof ft);
    } while(ch != 10 && ch != 13);
    _putch('\r');
    _putch('\n');
    
    GetSystemTimeAsFileTime(&ft);
    A_SHAUpdate(&SHAHash, (PBYTE)&ft, sizeof ft);
    A_SHAFinal(&SHAHash, rgbHash);
    
    memset(rgbKeys, 0, sizeof rgbKeys); // or leave it unknown
    memcpy(rgbKeys, SymmetricKey(), 16);
    rc4_key(&RC4key, 20, rgbHash);
    rc4(&RC4key, sizeof rgbKeys, rgbKeys);

    memcpy(SymmetricKey(), rgbKeys, 16);

    return TRUE;
}






PBYTE CKeyFile::PublicKey()
{
    if (KeyHeader()->KeyType == KEYFILE_PUBPAIR)
    {
        return m_pData + sizeof(XBOX_KEYFILE_HEADER);
    }
    else
    {
        return NULL;
    }
}


PBYTE CKeyFile::PrivateKey()
{
    if (KeyHeader()->KeyType != KEYFILE_PUBONLY)
    {
        return m_pData + sizeof(XBOX_KEYFILE_HEADER) + KeyHeader()->PublicKeyBytes;
    }
    else
    {
        return NULL;
    }
}


PBYTE CKeyFile::SymmetricKey()
{
    if (KeyHeader()->KeyType == KEYFILE_SYM)
    {
        return m_pData + sizeof(XBOX_KEYFILE_HEADER) + sizeof(BSAFE_PUB_KEY);
    }
    else
    {
        return NULL;
    }
}



void
CKeyFile::Free()
{
    if (m_pData != NULL)
    {
        free(m_pData);
        m_pData = NULL;
    }
    m_dwLen = 0;

}



void
CKeyFile::Alloc(
    IN DWORD dwSize
    )
{
    Free();
    
    m_pData = (PBYTE)malloc(dwSize);
    if (m_pData != NULL)
    {
        m_dwLen = dwSize;
        memset(m_pData, 0, m_dwLen);
    }
}



BOOL
CKeyFile::ReadFromFile(
    IN LPCSTR pszFN
    )
/*++

Routine Description:

    Reads a public or private key from the file

Arguments:

    pszFN - supplies the name of the file

Return Value:

    TRUE if succesful, FALSE otherwise

--*/
{
    HANDLE hFile;
    DWORD dwBytes;
    DWORD dwSize;
    BOOL bRet = FALSE;

    Free();

    hFile = CreateFile(pszFN, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        dwSize = GetFileSize(hFile, NULL);
        if (dwSize > 0)
        {
            Alloc(dwSize);
            if (ReadFile(hFile, m_pData, m_dwLen, &dwBytes, NULL))
            {
                bRet = TRUE;
            }
            else
            {
                Free();
                ERROR_OUT("Cannot read from key file %s", pszFN);
            }
        }
        CloseHandle(hFile);
    }
    else
    {
        ERROR_OUT("Cannot open from key file %s", pszFN);
    }

    if (bRet) 
    {
        TRACE_OUT(TRACE_ALWAYS, "Successfully read plain text keyfile %s", pszFN);
    }

    return bRet;
}


BOOL
CKeyFile::WriteToFile(
    IN LPCSTR pszFN
    )
/*++

Routine Description:

    Writes a key to the a file

Arguments:

Return Value:

    TRUE if succesful, FALSE otherwise

--*/
{
    HANDLE hFile;
    DWORD dwBytes;
    BOOL bRet = FALSE;

    if (IsNull())
    {
        return bRet;
    }

    hFile = CreateFile(pszFN, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        if (WriteFile(hFile, m_pData, m_dwLen, &dwBytes, NULL))
        {
            bRet = TRUE;
        }
        else
        {
            ERROR_OUT("Cannot write to key file %s", pszFN);
        }

        CloseHandle(hFile);
    }
    else
    {
        ERROR_OUT("Cannot create key file %s", pszFN);
    }

    if (bRet) 
    {
        TRACE_OUT(TRACE_ALWAYS, "Plain text file %s created successfully", pszFN);
    }

    return bRet;
}



BOOL 
CKeyFile::ReadFromEncFile(
    LPCSTR pszFN, 
    LPCSTR pszPassword
    )
/*++

Routine Description:

    Reads a public or private key from an encrypted file

Arguments:

Return Value:

    TRUE if succesful, FALSE otherwise

--*/
{
    BOOL bRet = FALSE;
    DWORD MsgLen;

    Free();

    if (XcGetEncryptedFileMsgLen(pszFN, &MsgLen)) {

        //
        // Allocate memory for the plain text message
        //
        Alloc(MsgLen);


        //
        // Read the message from encrypted file
        //
        bRet = XcReadEncryptedFile(pszFN, pszPassword, m_pData, m_dwLen);

        if (bRet == FALSE) {
            Free();
        }
    }
    else {
        ERROR_OUT("File %s is not an encrypted file", pszFN);
    }

    return bRet;
}


BOOL
CKeyFile::WriteToEncFile(
    IN LPCSTR pszFN,
    LPCSTR pszPassword
    )
/*++

Routine Description:

    Writes a key to the an encrypted file

Arguments:

Return Value:

    TRUE if succesful, FALSE otherwise

--*/
{
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
    CRandom rand;

    if (IsNull())
    {
        return FALSE;
    }

    //
    // Generate random confounder
    //
    rand.RandBytes(Confounder, RC4_CONFOUNDER_LEN);

    //
    // rc4hmac encrypt and write the key data to a file
    //
    return XcWriteEncryptedFile(m_pData, m_dwLen, pszFN, pszPassword, Confounder);
}


BOOL 
CKeyFile::IsEncFile(
    LPCSTR pszFN
    )
{
    DWORD MsgLen;


    return XcGetEncryptedFileMsgLen(pszFN, &MsgLen);
}
