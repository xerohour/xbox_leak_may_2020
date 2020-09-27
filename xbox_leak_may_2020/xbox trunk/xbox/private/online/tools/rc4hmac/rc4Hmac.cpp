//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include <windows.h>
#include <objbase.h>
#include <stdio.h>
#include <assert.h>
#include "md5.h"
#include "sha.h"
#include "rc4.h"

//////////////////////////////////////////////////////////////////////////
//
// SHA1 HMAC calculation
//
//////////////////////////////////////////////////////////////////////////
static void shaHmac(
    IN PBYTE pbKeyMaterial,
    IN ULONG cbKeyMaterial,
    IN PBYTE pbData,
    IN ULONG cbData,
    IN PBYTE pbData2,
    IN ULONG cbData2,
    OUT PBYTE HmacData // length must be A_SHA_DIGEST_LEN
    )
{
	#define HMAC_K_PADSIZE 64
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

//////////////////////////////////////////////////////////////////////////
//
// MD5 HMAC calculation
//
//////////////////////////////////////////////////////////////////////////
static void md5Hmac(
    IN PBYTE pbKeyMaterial,
    IN ULONG cbKeyMaterial,
    IN PBYTE pbData,
    IN ULONG cbData,
    IN PBYTE pbData2,
    IN ULONG cbData2,
    OUT PBYTE HmacData // length must be MD5_LEN
    )
{
	#define HMAC_K_PADSIZE 64
    BYTE Kipad[HMAC_K_PADSIZE];
    BYTE Kopad[HMAC_K_PADSIZE];
    BYTE HMACTmp[HMAC_K_PADSIZE+MD5_LEN];
    ULONG dwBlock;
    MD5_CTX Md5Hash;

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
    MD5Init(&Md5Hash);
    MD5Update(&Md5Hash, Kipad, HMAC_K_PADSIZE);
    if (cbData != 0)
    {
        MD5Update(&Md5Hash, pbData, cbData);
    }
    if (cbData2 != 0)
    {
        MD5Update(&Md5Hash, pbData2, cbData2);
    }

    // Finish off the hash
    MD5Final(&Md5Hash);
    RtlCopyMemory(HMACTmp+HMAC_K_PADSIZE, Md5Hash.digest, MD5_LEN);

    // prepend Kopad to H1, hash to get HMAC
    RtlCopyMemory(HMACTmp, Kopad, HMAC_K_PADSIZE);

    // final hash: output value into passed-in buffer
    MD5Init(&Md5Hash);
    MD5Update(&Md5Hash,HMACTmp, sizeof(HMACTmp));
    MD5Final(&Md5Hash);
    RtlCopyMemory(HmacData, Md5Hash.digest, MD5_LEN);

}


#define RC4_CONFOUNDER_LEN 8

typedef struct RC4_SHA1_HEADER {
    UCHAR Checksum[A_SHA_DIGEST_LEN];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
} *PRC4_SHA1_HEADER;

typedef struct RC4_MD5_HEADER {
    UCHAR Checksum[MD5_LEN];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
} *PRC4_MD5_HEADER;

static void rc4Sha1HmacEncrypt(
    IN PUCHAR confounder, // RC4_CONFOUNDER_LEN bytes
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    OUT PUCHAR pbHeader // A_SHA_DIGEST_LEN + RC4_CONFOUNDER_LEN bytes
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

static BOOL rc4Sha1HmacDecrypt(
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    IN PUCHAR pbHeader // A_SHA_DIGEST_LEN + RC4_CONFOUNDER_LEN bytes
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

static void rc4Md5HmacEncrypt(
    IN PUCHAR confounder, // RC4_CONFOUNDER_LEN bytes
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    OUT PUCHAR pbHeader // MD5_LEN + RC4_CONFOUNDER_LEN bytes
    )
{
    PRC4_MD5_HEADER CryptHeader = (PRC4_MD5_HEADER) pbHeader;
    BYTE LocalKey[MD5_LEN];
    RC4_KEYSTRUCT Rc4KeyStruct;

    //
    // Create the header - the confounder & checksum
    //
    RtlZeroMemory( CryptHeader->Checksum, MD5_LEN );
    RtlCopyMemory( CryptHeader->Confounder, confounder, RC4_CONFOUNDER_LEN );

    //
    // Checksum everything but the checksum
    //
    md5Hmac( pbKey, cbKey,
             CryptHeader->Confounder, RC4_CONFOUNDER_LEN,
             pbInput, cbInput,
             CryptHeader->Checksum );

    //
    // HMAC the checksum into the key
    //
    md5Hmac( pbKey, cbKey,
             CryptHeader->Checksum, MD5_LEN,
             NULL, 0,
             LocalKey );

    //
    // Use the generated key as the RC4 encryption key
    //
    rc4_key( &Rc4KeyStruct, MD5_LEN, LocalKey );

    //
    // Encrypt everything but the checksum
    //
    rc4( &Rc4KeyStruct, RC4_CONFOUNDER_LEN, CryptHeader->Confounder );
    rc4( &Rc4KeyStruct, cbInput, pbInput );
}

static BOOL rc4Md5HmacDecrypt(
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    IN PUCHAR pbHeader // MD5_LEN + RC4_CONFOUNDER_LEN bytes
    )
{
    PRC4_MD5_HEADER CryptHeader = (PRC4_MD5_HEADER) pbHeader;
    RC4_MD5_HEADER TempHeader;
    BYTE LocalKey[MD5_LEN];
    RC4_KEYSTRUCT Rc4KeyStruct;

    RtlCopyMemory( TempHeader.Confounder, CryptHeader->Confounder, RC4_CONFOUNDER_LEN );

    //
    // HMAC the checksum into the key
    //
    md5Hmac( pbKey, cbKey,
             CryptHeader->Checksum, MD5_LEN,
             NULL, 0,
             LocalKey );

    //
    // Use the generated key as the RC4 encryption key
    //
    rc4_key( &Rc4KeyStruct, MD5_LEN, LocalKey );

    //
    // Decrypt confounder and data
    //
    rc4( &Rc4KeyStruct, RC4_CONFOUNDER_LEN, TempHeader.Confounder );
    rc4( &Rc4KeyStruct, cbInput, pbInput );

    //
    // Now verify the checksum.
    //
    md5Hmac( pbKey, cbKey,
             TempHeader.Confounder, RC4_CONFOUNDER_LEN,
             pbInput, cbInput,
             TempHeader.Checksum );

    {
        char statekey[64],localkey[64];
        DWORD k;
        for (k=0; k<cbKey; ++k)
        {
            sprintf(statekey+2*k,"%02X", pbKey[k]);
            sprintf(localkey+2*k,"%02X", LocalKey[k]);
        }
        statekey[2*cbKey] = localkey[2*cbKey] = '\0';	        
        printf( "rc4Md5HmacDecrypt salted key is %s\n",
            statekey);
        printf( "rc4Md5HmacDecrypt local key is %s\n",
            localkey );
    }

    //
    // Decrypt is successful only if checksum matches
    //
    return ( RtlEqualMemory(
                 TempHeader.Checksum,
                 CryptHeader->Checksum,
                 MD5_LEN) );
}

void PrintUsage()
{
    printf("Usage: rc4Hmac Type KeyInHex DataInHex\n");
}

int __cdecl main(
    int argc,
    char** argv
    )
{
	BYTE key[24];
	BYTE StateKey[MD5_LEN];
	BYTE data[1024];
	int byte;
	ULONG ulType;
	
	int i;
	int cbKey;
	int cbData;
	
	if (argc < 4)
	{
		PrintUsage();
		return 1;
	}

	ulType = atol(argv[1]);
	
	cbKey = strlen(argv[2]) >> 1;
	assert( cbKey < sizeof(key) );
	for (i = 0;  i < cbKey; ++i)
	{
        sscanf(&(argv[2][i*2]), "%2x", &byte );
        key[i] = (BYTE)byte;
	}
	
	cbData = strlen(argv[3]) >> 1;
	assert( cbData < sizeof(data) );
	assert( cbData > sizeof(RC4_MD5_HEADER) );
	for (i = 0;  i < cbData; ++i)
	{
        sscanf(&(argv[3][i*2]), "%2x", &byte );
        data[i] = (BYTE)byte;
	}

    md5Hmac( key, cbKey, (PBYTE) &ulType, sizeof(ULONG), NULL, 0, StateKey);

    {
        char keybuf[64];
        int k;
        for (k=0; k<cbKey; ++k)
        {
            sprintf(keybuf+2*k,"%02X", key[k]);
        }
        keybuf[2*cbKey] = '\0';	        
        printf( "rc4Md5HmacDecrypt salt is %d\n",
            ulType);
        printf( "rc4Md5HmacDecrypt raw key is %s\n",
            keybuf);
    }

	if (rc4Md5HmacDecrypt( StateKey, MD5_LEN, data + sizeof(RC4_MD5_HEADER), cbData - sizeof(RC4_MD5_HEADER), data ))
	{
		printf("rc4Md5HmacDecrypt succeeded!\n");
		for (i = sizeof(RC4_MD5_HEADER); i < cbData; ++i)
		{
	        printf( "%02X", data[i] );
		}
		printf("\n");
		for (i = sizeof(RC4_MD5_HEADER); i < cbData; ++i)
		{
	        printf( "%c", (char)data[i] );
		}		
		printf("\n");
	}
	else
	{
		printf("rc4Md5HmacDecrypt failed!\n");
	}

    if (cbData > 100000)
    {
    	rc4Sha1HmacEncrypt( 0,0,0,0,0,0 );
    	rc4Sha1HmacDecrypt( 0,0,0,0,0 );
    	rc4Md5HmacEncrypt( 0,0,0,0,0,0 );
    }
    
    return 0;
}
