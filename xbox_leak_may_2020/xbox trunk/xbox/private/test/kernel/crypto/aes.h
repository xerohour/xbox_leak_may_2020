/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    aes.h

Abstract:

    This module contains the public data structures and API definitions
    needed to utilize the low-level AES encryption routines


Author:

    Scott Field (SField) 09-October-2000

Revision History:

--*/


#ifndef __AES_H__
#define __AES_H__

#ifndef RSA32API
#define RSA32API __stdcall
#endif

#ifdef __cplusplus
extern "C" {
#endif


#define AES_MAXROUNDS   (14)

typedef struct {
    int             rounds; // keytab data ends up padded.
    unsigned char   keytabenc[AES_MAXROUNDS+1][4][4];
    unsigned char   keytabdec[AES_MAXROUNDS+1][4][4];
} AESTable;

#define CRYPT_AES128_ROUNDS             10
#define CRYPT_AES192_ROUNDS             12
#define CRYPT_AES256_ROUNDS             14

#define AES_TABLESIZE   (sizeof(AESTable))
#define AES_BLOCKLEN    (16)
#define AES_KEYSIZE     (32)


void
RSA32API
aes(
    BYTE    *pbOut,
    BYTE    *pbIn,
    void    *keyin,
    int     op
    );

void
RSA32API
aeskey(
    AESTable    *KeyTable,
    BYTE        *Key,
    int         rounds
    );



#ifdef __cplusplus
}
#endif

#endif // __AES_H__

