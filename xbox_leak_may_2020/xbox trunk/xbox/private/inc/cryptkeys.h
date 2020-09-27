/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    cryptkeys.h

Abstract:

    Keys we use for munging data

--*/

#ifndef __CRYPTKEYS_H__
#define __CRYPTKEYS_H__

#include <xcrypt.h>

#define SYMMETRIC_KEY_LEN 16

#define HARD_DRIVE_KEY_LEN SYMMETRIC_KEY_LEN
#define ONLINE_KEY_LEN SYMMETRIC_KEY_LEN
#define XBOX_SERIAL_NUMBER_LEN 12
#define RECOVERY_KEY_LEN 8
#define ETHERNET_MAC_ADDR_LEN 6
#define ONLINE_KEY_PKENC_SIZE 128

//
// Following keys can change from version to version
//
#define FACTORY_RAND_KEY "\x71\x82\xEF\xDB\x72\x55\xD0\xA9\x9B\xC6\x3E\xD6\x47\xE5\x45\x47"
#define FACTORY_RAND_KEY_LEN SYMMETRIC_KEY_LEN

#define DEVKIT_ONLINE_RAND_KEY "\x7B\x59\x2E\x4F\x81\x78\xB4\xC7\x57\x88\x53\x1B\x2E\x74\x76\x87"
//#define DEVKIT_ONLINE_RAND_KEY "\xB2\x74\xD2\x92\xFE\x16\xA0\x17\x58\x70\xDB\x61\x7B\x02\xD0\xAD"
#define DEVKIT_ONLINE_RAND_KEY_LEN SYMMETRIC_KEY_LEN

//
// Following keys must be forever fixed
//
#define RECOVERY_RAND_KEY "\xBC\x20\x05\x1A\xB5\x97\xF9\x60\x48\x37\x5A\x83\x78\x7F\xE5\x94"
#define RECOVERY_RAND_KEY_LEN SYMMETRIC_KEY_LEN

//
// TODO: Any use of this key is temporary. We should not release anything that uses this key
//
#define TEMP_RAND_KEY "\x7b\x35\xa8\xb7\x27\xed\x43\x7a\xa0\xba\xfb\x8f\xa4\x38\x61\x80"
#define TEMP_RAND_KEY_LEN SYMMETRIC_KEY_LEN
#define ZERO_KEY "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

//
// This is the struct passed between GeneratePerBoxData and WritePerBoxData
//
typedef struct _XBOX_ENCRYPTED_PER_BOX_DATA {
    char XboxSerialNumber[XBOX_SERIAL_NUMBER_LEN];
    BYTE MACAddress[ETHERNET_MAC_ADDR_LEN];
    DWORD GameRegion;
    DWORD AVRegion;
    BYTE HardDriveKey[HARD_DRIVE_KEY_LEN];
    BYTE OnlineKey[ONLINE_KEY_LEN];
} XBOX_ENCRYPTED_PER_BOX_DATA, *PXBOX_ENCRYPTED_PER_BOX_DATA;

//
// This is the struct encrypted with the online public key.
// From the perspective of the genealogy database, we only
// claim that the online key is stored here. But in reality we
// store the following struct just for information redundency.
//
typedef struct _XBOX_ENCRYPTED_ONLINE_DATA {
    BYTE OnlineKey[ONLINE_KEY_LEN];
    char XboxSerialNumber[XBOX_SERIAL_NUMBER_LEN];
    BYTE HardDriveKey[HARD_DRIVE_KEY_LEN];
    BYTE MACAddress[ETHERNET_MAC_ADDR_LEN];
    DWORD GameRegion;
    DWORD AVRegion;
} XBOX_ENCRYPTED_ONLINE_DATA, *PXBOX_ENCRYPTED_ONLINE_DATA;

//
// This is the encrypted portion of the EEPROM before encryption
//
typedef struct _XBOX_ENCRYPTED_EEPROM_DATA {
    BYTE EncHeader[28] ;
    BYTE HardDriveKey[HARD_DRIVE_KEY_LEN];
    DWORD GameRegion;
} XBOX_ENCRYPTED_EEPROM_DATA, *PXBOX_ENCRYPTED_EEPROM_DATA;

#ifdef _XBOX

__forceinline void ComputeRecoveryKey (
    IN LPBYTE pbHardDriveKey,
    OUT char* pcRecoveryKey )
{
    BYTE Digest[XC_SERVICE_DIGEST_SIZE];
    WORD* pwDigestWORD = (WORD*)(&Digest[0]);
    int i = 4;
    char* recoveryKeyMap = "AXYUDLR";
    
    XcHMAC( (LPBYTE)RECOVERY_RAND_KEY, RECOVERY_RAND_KEY_LEN,
            pbHardDriveKey, HARD_DRIVE_KEY_LEN,
            NULL, 0,
            Digest );

    RtlZeroMemory( pcRecoveryKey, RECOVERY_KEY_LEN );
    while ( --i >= 0 )
    {
        pcRecoveryKey[i] = recoveryKeyMap[ (*pwDigestWORD++) % 7 ];
    }
}

#else // _XBOX

#include <sha.h>

void shaHmac(
    IN PBYTE pbKeyMaterial,
    IN ULONG cbKeyMaterial,
    IN PBYTE pbData,
    IN ULONG cbData,
    IN PBYTE pbData2,
    IN ULONG cbData2,
    OUT PBYTE HmacData // length must be A_SHA_DIGEST_LEN
    );

__forceinline void ComputeRecoveryKey (
    IN LPBYTE pbHardDriveKey,
    OUT char* pcRecoveryKey )
{
    BYTE Digest[A_SHA_DIGEST_LEN];
    WORD* pwDigestWORD = (WORD*)(&Digest[0]);
    int i = 4;
    char* recoveryKeyMap = "AXYUDLR";
    
    shaHmac( (LPBYTE)RECOVERY_RAND_KEY, RECOVERY_RAND_KEY_LEN,
             pbHardDriveKey, HARD_DRIVE_KEY_LEN,
             NULL, 0,
             Digest );

    RtlZeroMemory( pcRecoveryKey, RECOVERY_KEY_LEN );
    while ( --i >= 0 )
    {
        pcRecoveryKey[i] = recoveryKeyMap[ (*pwDigestWORD++) % 7 ];
    }
}

#endif // _XBOX

//
// Following key must be forever fixed
//
#define HDMORPH_RAND_KEY "\x60\x59\xE8\x2E\xDF\xBF\x7F\xD3\x23\x35\x74\x2A\x64\x8B\xB1\x2C"
#define HDMORPH_RAND_KEY_LEN SYMMETRIC_KEY_LEN

//
// Should be only used for online key
//
__forceinline void MorphKeyByHardDriveKeyEx( BYTE * pbHDKey, BYTE* key, DWORD dwKeyLen )
{
    BYTE Digest[XC_SERVICE_DIGEST_SIZE];
    BYTE RC4KeyStruct[XC_SERVICE_RC4_KEYSTRUCT_SIZE];

    XcHMAC( (LPBYTE)HDMORPH_RAND_KEY, HDMORPH_RAND_KEY_LEN,
            pbHDKey, SYMMETRIC_KEY_LEN,
            NULL, 0,
            Digest );

    XcRC4Key( RC4KeyStruct, XC_SERVICE_DIGEST_SIZE, Digest );
    XcRC4Crypt( RC4KeyStruct, dwKeyLen, key );    
}

#ifdef _XBOX
__forceinline void MorphKeyByHardDriveKey( BYTE* key, DWORD dwKeyLen )
{
    MorphKeyByHardDriveKeyEx((LPBYTE)(*XboxHDKey), key, dwKeyLen);
}
#endif

#define KEY_MORPH_RAND_KEY1 "\x2B\xB8\xD9\xEF\xD2\x04\x6D\x9D\x1F\x39\xB1\x5B\x46\x58\x01\xD7"
#define KEY_MORPH_RAND_KEY2 "\x1E\x05\xD7\x3A\xA4\x20\x6A\x7B\xA0\x5B\xCD\xDF\xAD\x26\xD3\xDE"
#define KEY_MORPH_IV        "\x7b\x35\xa8\xb7\x27\xed\x43\x7a"

//
// Should be used to morph all other keys
//
__forceinline void MorphKeyWithKey( BYTE* morphKey, DWORD dwMorphKeyLen, BYTE* key, DWORD dwKeyLen, DWORD dwOp )
{
    BYTE DESKey[XC_SERVICE_DES3_KEYSIZE];
    BYTE DES3Table[XC_SERVICE_DES3_TABLESIZE];
    BYTE Feedback[XC_SERVICE_DES_BLOCKLEN];
    
    memcpy(Feedback, KEY_MORPH_IV, sizeof(Feedback));

    XcHMAC( (LPBYTE)KEY_MORPH_RAND_KEY1, SYMMETRIC_KEY_LEN,
            morphKey, dwMorphKeyLen,
            NULL, 0,
            DESKey );

    XcHMAC( (LPBYTE)KEY_MORPH_RAND_KEY2, SYMMETRIC_KEY_LEN,
            morphKey, dwMorphKeyLen,
            NULL, 0,
            DESKey + XC_SERVICE_DES3_KEYSIZE - XC_SERVICE_DIGEST_SIZE );

    XcDESKeyParity( DESKey, XC_SERVICE_DES3_KEYSIZE );

    XcKeyTable( XC_SERVICE_DES3_CIPHER, DES3Table, DESKey );

    XcBlockCryptCBC( XC_SERVICE_DES3_CIPHER, dwKeyLen, key, key, DES3Table, dwOp, Feedback );
}

//
// Following two routines are used to encrypt/decrypt keys that are stored on HD
//
__forceinline void EncryptKeyWithHardDriveKeyEx( BYTE * pbHDKey, BYTE* key, DWORD dwKeyLen )
{
    MorphKeyWithKey( pbHDKey, SYMMETRIC_KEY_LEN, key, dwKeyLen, XC_SERVICE_ENCRYPT );
}

__forceinline void DecryptKeyWithHardDriveKeyEx( BYTE * pbHDKey, BYTE* key, DWORD dwKeyLen )
{
    MorphKeyWithKey( pbHDKey, SYMMETRIC_KEY_LEN, key, dwKeyLen, XC_SERVICE_DECRYPT );
}


#ifdef _XBOX
__forceinline void EncryptKeyWithHardDriveKey( BYTE* key, DWORD dwKeyLen )
{
    EncryptKeyWithHardDriveKeyEx((LPBYTE)(*XboxHDKey), key, dwKeyLen);
}

__forceinline void DecryptKeyWithHardDriveKey( BYTE* key, DWORD dwKeyLen )
{
    DecryptKeyWithHardDriveKeyEx((LPBYTE)(*XboxHDKey), key, dwKeyLen);
}
#endif

#define MU_ENCRYPT_RAND_KEY "\xA7\x14\x21\x3D\x94\x46\x1E\x05\x97\x6D\xE8\x35\x21\x2A\xE5\x7C"

//
// Following two routines are used to encrypt/decrypt keys that are stored on MU
//
__forceinline void EncryptKeyWithMUKey( BYTE* key, DWORD dwKeyLen )
{
    MorphKeyWithKey( (LPBYTE)MU_ENCRYPT_RAND_KEY, SYMMETRIC_KEY_LEN, key, dwKeyLen, XC_SERVICE_ENCRYPT );
}

__forceinline void DecryptKeyWithMUKey( BYTE* key, DWORD dwKeyLen )
{
    MorphKeyWithKey( (LPBYTE)MU_ENCRYPT_RAND_KEY, SYMMETRIC_KEY_LEN, key, dwKeyLen, XC_SERVICE_DECRYPT );
}

#endif //__CRYPTKEYS_H__

