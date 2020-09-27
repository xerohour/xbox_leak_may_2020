/*+----------------------------------------------------------------------------
// Copyright (C) Microsoft Corporation, 1995 - 1996.
// All rights reserved.
//
// This file is part of the Microsoft Private Communication Technology
// reference implementation, version 1.0
//
// The Private Communication Technology reference implementation, version 1.0
// ("PCTRef"), is being provided by Microsoft to encourage the development and
// enhancement of an open standard for secure general-purpose business and
/*-----------------------------------------------------------------------------
* Copyright (C) Microsoft Corporation, 1995 - 1996.
* All rights reserved.
*
* This file is part of the Microsoft Private Communication Technology
* reference implementation, version 1.0
*
* The Private Communication Technology reference implementation, version 1.0
* ("PCTRef"), is being provided by Microsoft to encourage the development and
* enhancement of an open standard for secure general-purpose business and
* personal communications on open networks.  Microsoft is distributing PCTRef
* at no charge irrespective of whether you use PCTRef for non-commercial or
* commercial use.
*
* Microsoft expressly disclaims any warranty for PCTRef and all derivatives of
* it.  PCTRef and any related documentation is provided "as is" without
* warranty of any kind, either express or implied, including, without
* limitation, the implied warranties or merchantability, fitness for a
* particular purpose, or noninfringement.  Microsoft shall have no obligation
* to provide maintenance, support, upgrades or new releases to you or to anyone
* receiving from you PCTRef or your modifications.  The entire risk arising out
* of use or performance of PCTRef remains with you.
*
* Please see the file LICENSE.txt,
* or http://pct.microsoft.com/pct/pctlicen.txt
* for more information on licensing.
*
* Please see http://pct.microsoft.com/pct/pct.htm for The Private
* Communication Technology Specification version 1.0 ("PCT Specification")
*
* 1/23/96
*----------------------------------------------------------------------------*/

#define SERIALNUMBER_LENGTH 16


#ifdef DEBUG
    extern DWORD g_cPctCertDebugIds;
#endif // DEBUG

typedef struct 
{
    UINT cbObjId;    // size of encoded object id
    PBYTE pbObjId;    // points to encoded object id
} ObjIdDesc;

typedef struct
{
    UINT cObjIds;    // count of ObjIdDesc structs in array
    ObjIdDesc aObjIds[1];
} EnhancedKeyUsage;

typedef struct PctCertificate {
#ifdef DEBUG
    DWORD           dwDebugId;
#endif // DEBUG
    long            cRef;       // Reference Count.
    CertSpec        Spec;

    PctPublicKey    *pPublicKey;

    SigSpec         SigType;
    DWORD           cbSig;
    PUCHAR          pSig;
    BOOL            fTrusted;

    struct PctCertificate  *pIssuerCertificate;

    PUCHAR          szSigningAuthority;

    PBYTE           pbIssuer;
    DWORD           cbIssuer;

    PBYTE           pbSubject;
    DWORD           cbSubject;

	PUCHAR          pSignedPortion;
	DWORD           cbSignedPortion;

    FILETIME        ValidFrom;
    FILETIME        ValidUntil;

    PUCHAR          pRawPubKey;
    DWORD           cbRawPubKey;

    PUCHAR          pRawCert;
    DWORD           cbRawCert;

    PUCHAR          pszSubject;
    PUCHAR          pCert;
	EnhancedKeyUsage *pEnhancedKeyUsage;	// list of OIDs
} PctCertificate, * PPctCertificate;


typedef BOOL
(WINAPI *CertDecodeFn)(
    PUCHAR               pbCertificate,
    DWORD               cbCertificate,
    PPctCertificate *  ppCertificate);

typedef BOOL
(WINAPI *CertDeleteFn)(
    PPctCertificate   pCertificate);

typedef struct _CertSystem {
     CertSpec      Spec;
     CertDecodeFn  Decode;
     CertDeleteFn  Delete;
} CertSystem, *PCertSystem;

extern const CertSystem certX509;

LONG ReferenceCert(PPctCertificate pCert);
LONG DereferenceCert(PPctCertificate);

SP_STATUS
SPLoadCertificate(
    DWORD      fProtocol,
    CertSpec   Spec,
    PUCHAR     pCertificate,
    DWORD      cbCertificate,
    PPctCertificate *ppCert,
    PDWORD      pdwSGC
    );

SP_STATUS
SPSerializeCertificate(
    DWORD      fProtocol,
    PUCHAR *   ppCertificate,
    DWORD  *   pcbCertificate,
    PPctCertificate pCert
);
