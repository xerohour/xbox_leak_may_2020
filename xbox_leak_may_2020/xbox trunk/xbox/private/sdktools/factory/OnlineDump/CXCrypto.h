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

    BOOL  rc4HmacDecrypt(
                    IN PUCHAR pbKey,
                    IN ULONG cbKey,
                    IN PUCHAR pbInput,
                    IN ULONG cbInput,
                    IN PUCHAR pbHeader); // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes 
                         
    void rc4HmacEncrypt(
                    IN PUCHAR confounder, // RC4_CONFOUNDER_LEN bytes
                    IN PUCHAR pbKey,
                    IN ULONG cbKey,
                    IN PUCHAR pbInput,
                    IN ULONG cbInput,
                    OUT PUCHAR pbHeader); // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes

    ~CXCrypto() {}
    CXCrypto() {}
};
#endif
