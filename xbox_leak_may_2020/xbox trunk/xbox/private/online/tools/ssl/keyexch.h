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

#ifndef __KEYEXCH_H__
#define __KEYEXCH_H__




typedef struct _PctPrivateKey
{
    DWORD Type;
    DWORD cbKey;
    UCHAR pKey[1];
} PctPrivateKey, *PPctPrivateKey;


typedef
long
(WINAPI * KeyExchangeDecodePubFn)(
    PUCHAR          pbEncoded,
    DWORD           cbKey,
    PctPublicKey   **ppKey);

typedef
long
(WINAPI * KeyExchangeDecodePrvFn)(
    PUCHAR          pbEncoded,
    DWORD           cbKey,
    PctPrivateKey   **ppKey);

typedef
BOOL
(WINAPI * KeyExchangeEncryptFn)(
    PUCHAR          pbClean,
    DWORD           cbClean,
    PUCHAR          pbEncrypted,
    DWORD           *pcbEncrypted,
    PctPublicKey    *pKey,
    DWORD           fProtocol);

typedef
BOOL
(WINAPI * KeyExchangeDecryptFn)(
    PUCHAR          pbEncrypted,
    DWORD           cbEncrypted,
    PUCHAR          pbClean,
    DWORD           *pcbClean,
    PctPrivateKey   *pKey,
    DWORD           fProtocol);



typedef struct _KeyExchangeSystem {
    DWORD               Type;
    PSTR            pszName;
    KeyExchangeDecodePubFn DecodePublic;
    KeyExchangeDecodePrvFn DecodePrivate;
    KeyExchangeEncryptFn Encrypt;
    KeyExchangeDecryptFn Decrypt;
} KeyExchangeSystem, * PKeyExchangeSystem;



typedef struct kexchtoalg {
    ALG_ID  idAlg;
    KeyExchangeSystem *System;
} AlgToExch;

extern AlgToExch g_AlgToExchMapping[];
extern int g_iAlgToExchMappings;


/*
 * instantiations of systems
 */

extern const KeyExchangeSystem keyexchPKCS;

#endif /* __KEYEXCH_H__ */
