//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
//===================================================================
#ifndef _CCRYPTO_H_
#define _CCRYPTO_H_

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <cryptkeys.h>
#include <sha.h>
#include <rc4.h>
#include <wincrypt.h>
#include "..\include\DataPacket.h"

  
#define DAT_ENCRYPTION_KEY "\x7a\x3b\xa8\xb7\x27\xed\x43\x7a\xd0\xba\xfb\x8f\xa4\xd8\x31\x90"
CONST DWORD DAT_ENCRYPTION_KEY_LEN = 16;

//The LPBYTE out buffer is 52  XboxEncryptedPerBoxData +
//28 bytes of RC4_SHA1_HEADER.
#define RC4_CONFOUNDER_LEN 8

typedef struct _RC4_SHA1_HEADER {
   UCHAR Checksum[A_SHA_DIGEST_LEN];
   UCHAR Confounder[RC4_CONFOUNDER_LEN];
} RC4_SHA1_HEADER, *PRC4_SHA1_HEADER;



class CCrypto {

private:
   void shaHmac(
             IN PBYTE pbKeyMaterial,
             IN ULONG cbKeyMaterial,
             IN PBYTE pbData,
             IN ULONG cbData,
             IN PBYTE pbData2,
             IN ULONG cbData2,
             OUT PBYTE HmacData); // length must be A_SHA_DIGEST_LEN 

   HCRYPTPROV m_hProv;
   HCRYPTKEY m_hPrvKey;

public:


   BOOL Decrypt ( IN OUT LPBYTE pbInputBuffer,
                  IN     DWORD dwInputBufferSize );

   BOOL rc4HmacDecrypt(
                   IN PUCHAR pbKey,
                   IN ULONG cbKey,
                   IN PUCHAR pbInput,
                   IN ULONG cbInput,
                   IN PUCHAR pbHeader // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes
                   );

   void rc4HmacEncrypt(
              IN PUCHAR confounder, // RC4_CONFOUNDER_LEN bytes
              IN PUCHAR pbKey,
              IN ULONG cbKey,
              IN PUCHAR pbInput,
              IN ULONG cbInput,
              OUT PUCHAR pbHeader // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes
              );
   HRESULT InitializeProvider();
   HRESULT DecryptOnlineKey( IN  LPBYTE pbEncrypted_128_Data, 
                             OUT LPBYTE pbUnencrypted_16_Data,
                             IN  DWORD  dwSizeOfOutBuffer = 0);

   ~CCrypto(){ if (NULL != m_hProv) CryptReleaseContext( m_hProv, 0 ); }
   CCrypto();

};
#endif
