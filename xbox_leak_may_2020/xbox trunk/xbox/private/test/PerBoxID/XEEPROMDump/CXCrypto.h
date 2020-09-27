//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
//===================================================================
#ifndef _CXCRYPTO_H_
#define _CXCRYPTO_H_

#include <xtl.h>
#include <init.h>
#include <xboxp.h>
#include <av.h>

#include <stdio.h>
#include <xdbg.h>
#include <malloc.h>

#include <wincrypt.h>
#include <cryptkeys.h>
#include <sha.h>
#include <rc4.h>
#include "..\include\DataPacket.h"
  
#define DAT_ENCRYPTION_KEY "\x7a\x3b\xa8\xb7\x27\xed\x43\x7a\xd0\xba\xfb\x8f\xa4\xd8\x31\x90"
CONST DWORD DAT_ENCRYPTION_KEY_LEN = 16;


//The LPBYTE out buffer is 52  XboxEncryptedPerBoxData +
//28 bytes of RC4_SHA1_HEADER.
#define RC4_CONFOUNDER_LEN 8
//
// Header before encrypted data
//
typedef struct _RC4_SHA1_HEADER {
    UCHAR Checksum[XC_SERVICE_DIGEST_SIZE];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
} RC4_SHA1_HEADER, *PRC4_SHA1_HEADER;


class CXCrypto {
private:
   
public:

   BOOL  rc4HmacDecrypt( IN PUCHAR pbKey,
                         IN ULONG cbKey,
                         IN PUCHAR pbInput,
                         IN ULONG cbInput,
                         IN PUCHAR pbHeader); // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes 
   void 
      rc4HmacEncrypt(
                   IN PUCHAR confounder, // RC4_CONFOUNDER_LEN bytes
                   IN PUCHAR pbKey,
                   IN ULONG cbKey,
                   IN PUCHAR pbInput,
                   IN ULONG cbInput,
                   OUT PUCHAR pbHeader); // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes

BOOL Decrypt ( IN OUT LPBYTE pbInputBuffer,
               IN     DWORD dwInputBufferSize );
   HRESULT 
   EncryptCriticalKeyPair ( IN  LPBYTE pbOnlineKey,
                            IN  LPBYTE pbHarDriveKey,
                            IN  LPBYTE bConfounder,
                            OUT LPBYTE pbEncBlob);

~CXCrypto(){}
   CXCrypto() {}
};
#endif
