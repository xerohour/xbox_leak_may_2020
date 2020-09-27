//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1996.
//
//  File:       certmap.h
//
//  Contents:   CertMapper 
//              Prototypes and structure definitions
//
//  Functions:  CertMapper API
//
//  History:    5/29/96   a-petesk   Created
//
//----------------------------------------------------------------------------

#ifndef __CERTMAP_H__
#define __CERTMAP_H__

#define CERT_FAR
#define CERT_API __stdcall

#ifdef __cplusplus__
extern "C" 
{
#endif //__cplusplus__


// definitions of mapper basic types.

struct _MAPPER_VTABLE;

typedef struct  _HMAPPER
{
    struct _MAPPER_VTABLE *m_vtable;
    DWORD                  m_dwMapperVersion;
    VOID                  *m_Reserved1;   // For schannel use only
}   HMAPPER, *PHMAPPER;

typedef DWORD HLOCATOR, *PHLOCATOR;


LONG WINAPI ReferenceMapper(
    HMAPPER     *phMapper     // in
);

typedef LONG (WINAPI FAR *REF_MAPPER_FN)(
    HMAPPER     *phMapper     // in
);

LONG WINAPI DeReferenceMapper(
    HMAPPER     *phMapper     // in
);

typedef LONG (WINAPI FAR *DEREF_MAPPER_FN)(
    HMAPPER     *phMapper     // in
);


BOOL WINAPI GetIssuerList(
    HMAPPER        *phMapper,           // in
    VOID *          Reserved,           // in
    BYTE *          pIssuerList,       // out
    DWORD *         pcbIssuerList       // out
);

typedef BOOL (WINAPI FAR * GET_ISSUER_LIST_FN)(
    HMAPPER	        *phMapper	,       // in
    VOID *          Reserved,           // in
    BYTE *          pIssuerList,       // out
    DWORD *         pcbIssuerList       // out
);

BOOL WINAPI GetChallenge(
    HMAPPER         *phMapper,          // in
    BYTE *          pAuthenticatorId,   // in
    DWORD           cbAuthenticatorId,  // in
    BYTE *          pChallenge,        // out
    DWORD *         pcbChallenge        // out
);

typedef BOOL (WINAPI FAR * GET_CHALLENGE_FN)(
    HMAPPER	        *phMapper	,           // in
    BYTE *          pAuthenticatorId,   // in
    DWORD           cbAuthenticatorId,  // in
    BYTE *          pChallenge,        // out
    DWORD *         pcbChallenge        // out
);


BOOL WINAPI MapCredential(
    HMAPPER     *phMapper,            // in
    DWORD       dwCredentialType,   // in
    const BYTE *    pbCredential,   // in
    DWORD       cbCredential,       // in
    HLOCATOR *  phLocator           // out
);


typedef BOOL (WINAPI FAR * MAP_CREDENTIAL_FN)(
	HMAPPER		*phMapper,            // in
    DWORD 		dwCredentialType,	// in
    const BYTE *	pbCredential,   // in
	DWORD		cbCredential,		// in
	HLOCATOR *	phLocator           // out
);


BOOL WINAPI CloseLocator(
    HMAPPER  *phMapper,
	HLOCATOR hLocator	//in
);

typedef BOOL (WINAPI FAR * CLOSE_LOCATOR_FN)(
    HMAPPER  *phMapper,
	HLOCATOR hLocator	//in
);


BOOL WINAPI GetAccessToken(
    HMAPPER     *phMapper,
    HLOCATOR    hLocator,   // in
    HANDLE *    phToken     // out
);

typedef BOOL (WINAPI FAR * GET_ACCESS_TOKEN_FN)(
    HMAPPER     *phMapper,
    HLOCATOR    hLocator,   // in
    HANDLE *    phToken     // out
);


typedef struct _MAPPER_VTABLE {
    REF_MAPPER_FN       ReferenceMapper;
    DEREF_MAPPER_FN     DeReferenceMapper;
    GET_ISSUER_LIST_FN  GetIssuerList;
    GET_CHALLENGE_FN    GetChallenge;
    MAP_CREDENTIAL_FN   MapCredential;
    GET_ACCESS_TOKEN_FN GetAccessToken;
    CLOSE_LOCATOR_FN    CloseLocator;	
} MAPPER_VTABLE, *PMAPPER_VTABLE;

#define MAPPER_INTERFACE_VER 0x00000001


#define X509_ASN_CHAIN      0x00000001


typedef struct _MAPPER_BLOB
{
    DWORD   cbBytes;
    BYTE *  pBytes;
} MAPPER_BLOB, *PMAPPER_BLOB;

typedef struct _MAPPER_X509_ASN_CHAIN
{
    DWORD   dwFlags;            // See definitions below

    DWORD   SerialNumber[4];    // Serial number of leaf cert.
    DWORD   cbSubject;
    PBYTE   pSubject;           // DER encoded subject
    DWORD   cbIssuer;
    PBYTE   pIssuer;            // DER encoded base issuer name
    FILETIME        ValidFrom;
    FILETIME        ValidUntil;

    DWORD   cCerts;             // Number of certs in the chain
    MAPPER_BLOB     aCerts[1];  // An array of BLOBS pointing to X509 Certs
} MAPPER_X509_ASN_CHAIN, *PMAPPER_X509_ASN_CHAIN;

// Flag definitions
#define X509_UNKNOWN_ISSUER     0x00000002    // We do not know the root issuer for the chain
#define X509_EXPIRED            0x00000004    // A certificate in the chain has expired


#ifdef __cplusplus__
}
#endif //__cplusplus__

#endif // __CERTMAP_H__
