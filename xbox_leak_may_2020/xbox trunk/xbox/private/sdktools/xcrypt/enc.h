/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    enc.h

Abstract:


--*/

#ifndef _ENC_H
#define _ENC_H

#include <windows.h>
 
#define XC_ENCFILE_SIG             'k2ne' // signature dword in the encrypted file


#define RC4_CONFOUNDER_LEN          8
#define HMAC_K_PADSIZE              64

typedef struct _RC4_SHA1_HEADER {
    UCHAR Checksum[A_SHA_DIGEST_LEN];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
} RC4_SHA1_HEADER, *PRC4_SHA1_HEADER;



typedef struct _XC_ENCFILE_HEADER
{
    DWORD Sig;
    RC4_SHA1_HEADER CryptHeader;
    DWORD MsgLen;
    UCHAR MsgDigest[A_SHA_DIGEST_LEN];
} XC_ENCFILE_HEADER, *PXC_ENCFILE_HEADER;



BOOL
XcWriteEncryptedFile(
    PBYTE pMessage,
    DWORD cbMsgLen,
    LPCSTR pszFN,
    LPCSTR pszPassword,
    PBYTE pConfounder  // RC4_CONFOUNDER_LEN bytes
    );
 
BOOL 
XcReadEncryptedFile(
    LPCSTR pszFN, 
    LPCSTR pszPassword,
    PBYTE pMsgBuf,
    DWORD cbMsgBufLen
    );

BOOL 
XcGetEncryptedFileMsgLen(
    LPCSTR pszFN,
    PDWORD pdwSize
    );


#endif // _ENC_H  
