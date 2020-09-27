//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include "CCrypto.h"
#include "stdio.h"

BOOL
CCrypto::
DumpBytes(IN LPBYTE lpbID,
          IN UINT   uiSize)
{
   for ( UINT i = 0; i < uiSize; i++ ) {
      printf("%02X", lpbID[i]);
   }
   printf("\n");
   return TRUE;
}


void
CCrypto::
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


#define HMAC_K_PADSIZE              64
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


BOOL
CCrypto::
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
   return( RtlEqualMemory(
                         TempHeader.Checksum,
                         CryptHeader->Checksum,
                         A_SHA_DIGEST_LEN) );
}


BOOL 
CCrypto::
Decrypt ( IN OUT LPBYTE pbInputBuffer,
          IN DWORD dwInputBufferSize )
{
   
   return rc4HmacDecrypt( (LPBYTE)FACTORY_RAND_KEY, 
                           FACTORY_RAND_KEY_LEN,
                           pbInputBuffer + sizeof(RC4_SHA1_HEADER),
                           dwInputBufferSize,
                           pbInputBuffer );
                           
}

CCrypto::CCrypto()
{ 

}


VOID
CCrypto::
InitCrypto( VOID )
{ 
   m_hProv   = 0; 
   m_hPrvKey = 0; 
   HRESULT   hr;
   BOOL bResult;
   #define PROVIDER "nCipher Enhanced Cryptographic Provider"

   CryptAcquireContext( &m_hProv,
                                  "XboxOnlineKeyV1",
                                  PROVIDER,
                                  PROV_RSA_FULL,
                                  CRYPT_DELETEKEYSET);
   // open crypto context with the private key
   bResult = CryptAcquireContext(&m_hProv,
                                 "XboxOnlineKeyV1",
                                 PROVIDER,
                                 PROV_RSA_FULL,
                                 CRYPT_MACHINE_KEYSET);
   if (!bResult) {
      hr = GetLastError();
      printf("CryptAcquireContext create MachineKeySet failed with %x\n", hr);
      m_hProv = 0;
      //exit(1);
   }

   bResult = CryptGetUserKey( m_hProv, AT_KEYEXCHANGE, &m_hPrvKey );
   if (!bResult) {
      hr = GetLastError();
      printf("CryptGetUserKey failed with %x\n", hr);
      //exit(1);
   }

}


HRESULT
CCrypto::
DecryptOnlineKey( IN  LPBYTE pbEncrypted_128_Data, 
                  OUT LPBYTE pbUnencrypted_16_Data )
{

   BOOL bResult;
   HRESULT hr;
   DWORD dwDataLen;

   // Decrypt the online key with the private key.
   BYTE buffer[ONLINE_KEY_PKENC_SIZE];

   RtlCopyMemory( buffer, pbEncrypted_128_Data, ONLINE_KEY_PKENC_SIZE );
   
   dwDataLen = ONLINE_KEY_PKENC_SIZE;
   
   bResult = CryptDecrypt( m_hPrvKey, NULL, TRUE, 0, buffer, &dwDataLen );
   if (!bResult) {
      hr = GetLastError();
      printf("Error %x during CryptDecrypt! Bytes output:%lu\n", hr, dwDataLen);
      goto error;
   }

   // Reverse the bytes for now.
   BYTE *s,*e, t;
   s = &buffer[0];
   e = &buffer[dwDataLen - 1];

   while (s < e) {
      t = *s;
      *s++ = *e;
      *e-- = t;
   }

   ZeroMemory (pbUnencrypted_16_Data, ONLINE_KEY_PKENC_SIZE );
   RtlCopyMemory( pbUnencrypted_16_Data, buffer, ONLINE_KEY_PKENC_SIZE );
   
   printf("\nENCRYPTED::\n");
   DumpBytes(pbEncrypted_128_Data, 128);
   printf("\nUNENCRYPTED:: \n");
   DumpBytes(pbUnencrypted_16_Data, 16);

   hr = S_OK;
   
error:
   return hr;
}


