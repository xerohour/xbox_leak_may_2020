/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    key.h

Abstract:

    Contains definition of key handling class

--*/

#ifndef _KEY_H
#define _KEY_H

#include <windows.h>


#define KEYFILE_PUBPAIR      1      // public and private keys
#define KEYFILE_PUBONLY      2      // public key only
#define KEYFILE_SYM          3      // symmetric key


typedef struct _XBOX_KEYFILE_HEADER
{
    USHORT KeyType;                 // KEYFILE_ defines
    USHORT KeyBits;                 // number of bits in the key
    GUID   KeyGUID;                 // unique guid genereted at the time the key file was created
    ULONG  KeyNumber;               // a number assigned key
    ULONG  PublicKeyBytes;          // bytes used by public key structure
    ULONG  PrivateKeyBytes;         // bytes used by private key structure

} XBOX_KEYFILE_HEADER, *PXBOX_KEYFILE_HEADER;




class CKeyFile 
{
public:
    CKeyFile()
        : m_pData(NULL),
          m_dwLen(0)
    {
    }

    ~CKeyFile()
    {
        Free();
    }

    BOOL IsNull()
    {
        return (m_pData == NULL);
    }


    PXBOX_KEYFILE_HEADER KeyHeader()
    {
        return (PXBOX_KEYFILE_HEADER)m_pData;
    }

    BOOL CreatePubKeyPair(DWORD dwKeyBits);
    BOOL CreateSymKey(DWORD dwKeyBits);
    BOOL CreateSymKey2();

    PBYTE PublicKey();
    PBYTE PrivateKey();
    PBYTE SymmetricKey();

    BOOL ReadFromFile(LPCSTR pszFN);
    BOOL WriteToFile(LPCSTR pszFN);

    BOOL ReadFromEncFile(LPCSTR pszFN, LPCSTR pszPassword);
    BOOL WriteToEncFile(LPCSTR pszFN, LPCSTR pszPassword);

    BOOL IsEncFile(LPCSTR pszFN);

private:
    PBYTE m_pData;
    DWORD m_dwLen;

    void Alloc(DWORD dwSize);
    void Free();
};



#endif // _KEY_H 
