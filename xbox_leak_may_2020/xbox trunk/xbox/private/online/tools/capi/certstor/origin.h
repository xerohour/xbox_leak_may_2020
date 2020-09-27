//+---------------------------------------------------------------------------
//
//  Microsoft Windows NT Security
//  Copyright (C) Microsoft Corporation, 1997 - 1998
//
//  File:       origin.h
//
//  Contents:   Crypt Origin Identifier Definitions
//
//  History:    10-Sep-97    kirtd    Created
//
//----------------------------------------------------------------------------
#if !defined(__ORIGIN_H__)
#define __ORIGIN_H__

//
// A CRYPT_ORIGIN_IDENTIFIER is an MD5 hash of selected components of a
// CAPI2 object.  This allows a unique identifier to be derived for any
// CAPI2 object where any two objects with the same origin identifier
// are the same object at possibly different points in its evolution.
// If those objects have the same HASH id then they are the same object
// at the same point in time.  For the main CAPI2 objects the selected
// components are as follows:
//
// Certificate: Issuer Name, Subject Name, Public Key
//
// CTL: All Issuer Name and Issuer Serial No. pairs, Usage, List Identifier
//
// CRL: Issuer Name (an issuer can only publish ONE CRL, Hygiene work in
//      progress)
//

// BUGBUG: Move to WINCRYPT.H
typedef BYTE CRYPT_ORIGIN_IDENTIFIER[ MD5DIGESTLEN ];

//
// Function prototypes
//

BOOL WINAPI CertGetOriginIdentifier (
                IN PCCERT_CONTEXT pCertContext,
                IN DWORD dwFlags,
                OUT CRYPT_ORIGIN_IDENTIFIER OriginIdentifier
                );

BOOL WINAPI CtlGetOriginIdentifier (
                IN PCCTL_CONTEXT pCtlContext,
                IN DWORD dwFlags,
                OUT CRYPT_ORIGIN_IDENTIFIER OriginIdentifier
                );

BOOL WINAPI CrlGetOriginIdentifier (
                IN PCCRL_CONTEXT pCrlContext,
                IN DWORD dwFlags,
                OUT CRYPT_ORIGIN_IDENTIFIER OriginIdentifier
                );

#endif
