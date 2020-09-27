////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992-1999.
//
//  File:       winefs.h
//
//  Contents:   EFS Data and prototypes.
//
//----------------------------------------------------------------------------

#ifndef __WINEFS_H__
#define __WINEFS_H__

#if _MSC_VER>1000
#pragma once
#endif


#ifdef __cplusplus
extern "C" {
#endif

//+---------------------------------------------------------------------------------/
//                                                                                  /
//                                                                                  /
//                          Data Structures                                         /
//                                                                                  /
//                                                                                  /
//----------------------------------------------------------------------------------/


//
//  Encoded Certificate
//


typedef struct _CERTIFICATE_BLOB {

    DWORD   dwCertEncodingType;

    DWORD   cbData;

#ifdef MIDL_PASS
    [size_is(cbData)]
#endif // MIDL_PASS
    PBYTE    pbData;

} EFS_CERTIFICATE_BLOB, *PEFS_CERTIFICATE_BLOB;


//
//  Certificate Hash
//

typedef struct _EFS_HASH_BLOB {

    DWORD   cbData;

#ifdef MIDL_PASS
    [size_is(cbData)]
#endif // MIDL_PASS
    PBYTE    pbData;

} EFS_HASH_BLOB, *PEFS_HASH_BLOB;



//
// Input to add a user to an encrypted file
//


typedef struct _ENCRYPTION_CERTIFICATE {
    DWORD cbTotalLength;
    SID * pUserSid;
    PEFS_CERTIFICATE_BLOB pCertBlob;
} ENCRYPTION_CERTIFICATE, *PENCRYPTION_CERTIFICATE;

#define MAX_SID_SIZE 256


typedef struct _ENCRYPTION_CERTIFICATE_HASH {
    DWORD cbTotalLength;
    SID * pUserSid;
    PEFS_HASH_BLOB  pHash;

#ifdef MIDL_PASS
    [string]
#endif // MIDL_PASS
    LPWSTR lpDisplayInformation;

} ENCRYPTION_CERTIFICATE_HASH, *PENCRYPTION_CERTIFICATE_HASH;







typedef struct _ENCRYPTION_CERTIFICATE_HASH_LIST {
    DWORD nCert_Hash;
#ifdef MIDL_PASS
    [size_is(nCert_Hash)]
#endif // MIDL_PASS
     PENCRYPTION_CERTIFICATE_HASH * pUsers;
} ENCRYPTION_CERTIFICATE_HASH_LIST, *PENCRYPTION_CERTIFICATE_HASH_LIST;



typedef struct _ENCRYPTION_CERTIFICATE_LIST {
    DWORD nUsers;
#ifdef MIDL_PASS
    [size_is(nUsers)]
#endif // MIDL_PASS
     PENCRYPTION_CERTIFICATE * pUsers;
} ENCRYPTION_CERTIFICATE_LIST, *PENCRYPTION_CERTIFICATE_LIST;




//+---------------------------------------------------------------------------------/
//                                                                                  /
//                                                                                  /
//                               Prototypes                                         /
//                                                                                  /
//                                                                                  /
//----------------------------------------------------------------------------------/


WINADVAPI
DWORD
WINAPI
QueryUsersOnEncryptedFile(
     IN LPCWSTR lpFileName,
     OUT PENCRYPTION_CERTIFICATE_HASH_LIST * pUsers
    );


WINADVAPI
DWORD
WINAPI
QueryRecoveryAgentsOnEncryptedFile(
     IN LPCWSTR lpFileName,
     OUT PENCRYPTION_CERTIFICATE_HASH_LIST * pRecoveryAgents
    );


WINADVAPI
DWORD
WINAPI
RemoveUsersFromEncryptedFile(
     IN LPCWSTR lpFileName,
     IN PENCRYPTION_CERTIFICATE_HASH_LIST pHashes
    );

WINADVAPI
DWORD
WINAPI
AddUsersToEncryptedFile(
     IN LPCWSTR lpFileName,
     IN PENCRYPTION_CERTIFICATE_LIST pUsers
    );

WINADVAPI
DWORD
WINAPI
SetUserFileEncryptionKey(
    IN PENCRYPTION_CERTIFICATE pEncryptionCertificate
    );


WINADVAPI
VOID
WINAPI
FreeEncryptionCertificateHashList(
    IN PENCRYPTION_CERTIFICATE_HASH_LIST pHashes
    );

WINADVAPI
BOOL
WINAPI
EncryptionDisable(
    IN LPCWSTR DirPath,
    IN BOOL Disable
    );


#ifdef __cplusplus
}       // Balance extern "C" above
#endif

#endif // __WINEFS_H__
