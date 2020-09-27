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

#ifndef __MSGS_H__
#define __MSGS_H__



/*
 *
 * Useful Macros
 *
 */

#define LSBOF(x)    ((UCHAR) (x & 0xFF))
#define MSBOF(x)    ((UCHAR) ((x >> 8) & 0xFF) )

#define COMBINEBYTES(Msb, Lsb)  ((DWORD) (((DWORD) (Msb) << 8) | (DWORD) (Lsb)))

/* external representations of algorithm specs */

typedef DWORD   ExtCipherSpec, *PExtCipherSpec;
typedef WORD    ExtHashSpec,   *PExtHashSpec;
typedef WORD    ExtCertSpec,   *PExtCertSpec;
typedef WORD    ExtExchSpec,   *PExtExchSpec;
typedef WORD    ExtSigSpec,    *PExtSigSpec;

typedef struct _Pct_Message_Header {
    UCHAR   Byte0;
    UCHAR   Byte1;
} Pct_Message_Header, * PPct_Message_Header;

typedef struct _Pct_Message_Header_Ex {
    UCHAR   Byte0;
    UCHAR   Byte1;
    UCHAR   PaddingSize;
} Pct_Message_Header_Ex, * PPct_Message_Header_Ex;


typedef struct _Pct_Error {
    Pct_Message_Header   Header;
    UCHAR               MessageId;
    UCHAR               ErrorMsb;
    UCHAR               ErrorLsb;
    UCHAR               ErrorInfoMsb;
    UCHAR               ErrorInfoLsb;
    UCHAR               VariantData[1];
} Pct_Error, * PPct_Error;


typedef struct _Pct_Client_Hello {
    Pct_Message_Header   Header;
    UCHAR               MessageId;
    UCHAR               VersionMsb;
    UCHAR               VersionLsb;
    UCHAR               Pad;
    UCHAR               SessionIdData[PCT_SESSION_ID_SIZE];
    UCHAR               ChallengeData[PCT_CHALLENGE_SIZE];
    UCHAR               OffsetMsb;
    UCHAR               OffsetLsb;
    UCHAR               CipherSpecsLenMsb;
    UCHAR               CipherSpecsLenLsb;
    UCHAR               HashSpecsLenMsb;
    UCHAR               HashSpecsLenLsb;
    UCHAR               CertSpecsLenMsb;
    UCHAR               CertSpecsLenLsb;
    UCHAR               ExchSpecsLenMsb;
    UCHAR               ExchSpecsLenLsb;
    UCHAR               KeyArgLenMsb;
    UCHAR               KeyArgLenLsb;
    UCHAR               VariantData[1];
} Pct_Client_Hello, * PPct_Client_Hello;


typedef struct _Pct_Server_Hello {
    Pct_Message_Header   Header;
    UCHAR               MessageId;
    UCHAR               Pad;
    UCHAR               ServerVersionMsb;
    UCHAR               ServerVersionLsb;
    UCHAR               RestartSessionOK;
    UCHAR               ClientAuthReq;
    ExtCipherSpec       CipherSpecData;
    ExtHashSpec         HashSpecData;
    ExtCertSpec         CertSpecData;
    ExtExchSpec         ExchSpecData;
    UCHAR               ConnectionIdData[PCT_SESSION_ID_SIZE];
    UCHAR               CertificateLenMsb;
    UCHAR               CertificateLenLsb;
    UCHAR               CertSpecsLenMsb;
    UCHAR               CertSpecsLenLsb;
    UCHAR               ClientSigSpecsLenMsb;
    UCHAR               ClientSigSpecsLenLsb;
    UCHAR               ResponseLenMsb;
    UCHAR               ResponseLenLsb;
    UCHAR               VariantData[1];
} Pct_Server_Hello, * PPct_Server_Hello;

typedef struct _Pct_Client_Master_Key {
    Pct_Message_Header   Header;
    UCHAR               MessageId;
    UCHAR               Pad;
    ExtCertSpec         ClientCertSpecData;
    ExtSigSpec          ClientSigSpecData;
    UCHAR               ClearKeyLenMsb;
    UCHAR               ClearKeyLenLsb;
    UCHAR               EncryptedKeyLenMsb;
    UCHAR               EncryptedKeyLenLsb;
    UCHAR               KeyArgLenMsb;
    UCHAR               KeyArgLenLsb;
    UCHAR               VerifyPreludeLenMsb;
    UCHAR               VerifyPreludeLenLsb;
    UCHAR               ClientCertLenMsb;
    UCHAR               ClientCertLenLsb;
    UCHAR               ResponseLenMsb;
    UCHAR               ResponseLenLsb;
    UCHAR               VariantData[1];
} Pct_Client_Master_Key, * PPct_Client_Master_Key;


typedef struct _Pct_Server_Verify {
    Pct_Message_Header   Header;
    UCHAR               MessageId;
    UCHAR               Pad;
    UCHAR               SessionIdData[PCT_SESSION_ID_SIZE];
    UCHAR               ResponseLenMsb;
    UCHAR               ResponseLenLsb;
    UCHAR               VariantData[1];
} Pct_Server_Verify, * PPct_Server_Verify;



/*
 *
 * Expanded Form Messages:
 *
 */

typedef struct _PctError {
	DWORD			Error;
	DWORD			ErrInfoLen;
	BYTE			*ErrInfo;
} PctError, *PPctError;

typedef struct _Client_Hello {
    DWORD           cCipherSpecs;
    DWORD           cHashSpecs;
    DWORD           cCertSpecs;
    DWORD           cExchSpecs;
    DWORD           cbKeyArgSize;
    CipherSpec *    pCipherSpecs;
    HashSpec *      pHashSpecs;
    CertSpec *      pCertSpecs;
    ExchSpec *      pExchSpecs;
    PctSessionId    SessionId;
    PctChallenge    Challenge;
    PUCHAR          pKeyArg;
} Client_Hello, * PClient_Hello;


typedef struct _Server_Hello {
    DWORD           RestartOk;
    DWORD           ClientAuthReq;
    DWORD           CertificateLen;
    DWORD           ResponseLen;
    DWORD           cSigSpecs;
    DWORD           cCertSpecs;
    PctSessionId    Connection;
    UCHAR *         pCertificate;
    UCHAR           Response[RESPONSE_SIZE];
    CipherSpec      SrvCipherSpec;
    HashSpec        SrvHashSpec;
    CertSpec        SrvCertSpec;
    ExchSpec        SrvExchSpec;
    SigSpec *       pClientSigSpecs;
    CertSpec *      pClientCertSpecs;
} Server_Hello, * PServer_Hello;

typedef struct _Client_Master_Key {
    DWORD           ClearKeyLen;
    DWORD           EncryptedKeyLen;
    DWORD           KeyArgLen;
    DWORD           VerifyPreludeLen;
    DWORD           ClientCertLen;
    DWORD           ResponseLen;
    CertSpec        ClientCertSpec;
    SigSpec         ClientSigSpec;
    UCHAR           ClearKey[MASTER_KEY_SIZE];
    UCHAR           EncryptedKey[ENCRYPTED_KEY_SIZE];
    UCHAR           KeyArg[MASTER_KEY_SIZE];
    UCHAR           ClientCert[CERT_SIZE];
    UCHAR           Response[PCT_SIGNATURE_SIZE];
    UCHAR           VerifyPrelude[RESPONSE_SIZE];
} Client_Master_Key, * PClient_Master_Key;

typedef struct _Server_Verify {
    UCHAR           SessionIdData[PCT_SESSION_ID_SIZE];
    DWORD           ResponseLen;
    UCHAR           Response[RESPONSE_SIZE];
} Server_Verify, * PServer_Verify;

/*
 *
 * Pickling Prototypes
 *
 */

SP_STATUS
PackClientHello(
    PClient_Hello       pCanonical,
    PPctBuffer          pCommOutput);

SP_STATUS
UnpackClientHello(
    PPctBuffer          pInput,
    PClient_Hello *     ppClient);

SP_STATUS
PackServerHello(
    PServer_Hello       pCanonical,
    PPctBuffer          pCommOutput);

SP_STATUS
UnpackServerHello(
    PPctBuffer          pInput,
    PServer_Hello *     ppServer);

SP_STATUS
PackClientMasterKey(
    PClient_Master_Key      pCanonical,
    PPctBuffer              pCommOutput);

SP_STATUS
UnpackClientMasterKey(
    PPctBuffer              pInput,
    PClient_Master_Key *    ppClient);

SP_STATUS
PackServerVerify(
    PServer_Verify          pCanonical,
    PPctBuffer              pCommOutput);

SP_STATUS
UnpackServerVerify(
    PPctBuffer              pInput,
    PServer_Verify *        ppServer);

SP_STATUS
PackPctError(
    PPctError               pCanonical,
    PPctBuffer              pCommOutput);

#endif /* __MSGS_H__ */
