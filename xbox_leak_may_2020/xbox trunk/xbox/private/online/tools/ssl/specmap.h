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

typedef struct csel
{
    DWORD               fProtocol;
    DWORD               fDefault;
    CipherSpec          Spec;
    PTSTR               szName;
    const CryptoSystem *      System;
} CipherInfo, *PCipherInfo;

typedef struct hsel
{
    DWORD               fProtocol;
    DWORD               fDefault;
    HashSpec            Spec;
    PTSTR               szName;
    const CheckSumFunction *  System;
} HashInfo, *PHashInfo;

typedef struct kexch
{
    DWORD               fProtocol;
    DWORD               fDefault;
    ExchSpec            Spec;
    PTSTR               szName;
    const KeyExchangeSystem * System;
} KeyExchangeInfo, *PKeyExchangeInfo;

typedef struct certsel
{
    DWORD               fProtocol;
    DWORD               fDefault;
    CertSpec            Spec;
    PTSTR               szName;
    const CertSystem   *      System;
} CertSysInfo, *PCertSysInfo;

typedef struct sigsel
{
    DWORD               fProtocol;
    DWORD               fDefault;
    SigSpec             Spec;
    PTSTR               szName;
    const SignatureSystem  *  System;
} SigInfo, *PSigInfo;


PCipherInfo         GetCipherInfo(CipherSpec Spec);

PHashInfo           GetHashInfo(HashSpec Spec);

PKeyExchangeInfo    GetKeyExchangeInfo(ExchSpec Spec);

PCertSysInfo        GetCertSysInfo(CertSpec Spec);

PSigInfo            GetSigInfo(SigSpec Spec);


const CryptoSystem *      CipherFromSpec(CipherSpec Spec, DWORD fProtocol);

const CheckSumFunction *  HashFromSpec(HashSpec Spec, DWORD fProtocol);

const KeyExchangeSystem * KeyExchangeFromSpec(ExchSpec Spec, DWORD fProtocol);

const CertSystem *        CertFromSpec(CertSpec Spec, DWORD fProtocol);

const SignatureSystem *   SigFromSpec(SigSpec Spec, DWORD fProtocol);


extern CipherInfo  SPAvailableCiphers[];
extern const DWORD SPNumAvailableCiphers;

extern HashInfo  SPAvailableHashes[];
extern const DWORD SPNumAvailableHashes;

extern CertSysInfo   SPAvailableCerts[];
extern const DWORD SPNumAvailableCerts;

extern SigInfo SPAvailableSigs[];
extern const DWORD SPNumAvailableSigs;

extern KeyExchangeInfo SPAvailableExch[];
extern const DWORD SPNumAvailableExch;
