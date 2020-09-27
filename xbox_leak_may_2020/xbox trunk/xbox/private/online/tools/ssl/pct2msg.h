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

#ifndef __PCT2MSG_H__
#define __PCT2MSG_H__

/* record types */
#define PCT2_RT_HANDSHAKE        0x0301
#define PCT2_RT_KEY_MGMT         0x0302
#define PCT2_RT_DATAGRAM         0x0303
#define PCT2_RT_ERROR            0x0304
#define PCT2_RT_USER_DATA        0x0305
#define PCT2_RT_PCT_VERSION_1_CH 0x0180
#define PCT2_RT_PCT_VERSION_1_SH 0x0280
#define PCT2_RT_SSL_VERSION_2_CH 0x0100
#define PCT2_RT_SSL_VERSION_3_CH 0x0000
#define PCT2_RT_CD_RESERVED      0x6364
#define PCT2_RT_KR_RESERVED      0x6B72
#define PCT2_RT_ESCROW           0x0310


/* HS message Types */
#define PCT2_HS_CLIENT_HELLO      0x0000
#define PCT2_HS_SERVER_HELLO      0x0001
#define PCT2_HS_CLIENT_MASTER_KEY 0x0002
#define PCT2_HS_SERVER_VERIFY     0x0003
#define PCT2_HS_CLIENT_VERIFY     0x0004

/* KM message types */
#define PCT2_KM_TYPE_FIXED_KEY       0x0001
#define PCT2_KM_TYPE_RESUME_KEY      0x0002
#define PCT2_KM_TYPE_REDO_HANDSHAKE  0x0003
#define PCT2_KM_TYPE_CLOSE_CONN      0x0004

/* DM message types */
#define PCT2_DM_TYPE_USER_DATA       0x0000

/* PV types */
#define PCT2_PV_TYPE_CERTIFICATE     0x0001
#define PCT2_PV_TYPE_PKCS_TOKEN      0x0002
#define PCT2_PV_TYPE_KEA             0x0003
#define PCT2_PV_TYPE_EPHEMERAL_RSA   0x0004
#define PCT2_PV_TYPE_EPHERERAL_DH    0x0005


/* CH Auth Types */
#define PCT2_CH_REQUEST_KEY_EXCH_SEND  0x0001
#define PCT2_CH_DEMAND_AUTH_KEY_EXCH   0x0002
#define PCT2_CH_DEMAND_AUTH_SIG        0x0004
#define PCT2_CH_DEMAND_AUTH_PASSWORD   0x0008
#define PCT2_CH_OFFER_KEY_EXCH_RECEIVE 0x0010
#define PCT2_CH_OFFER_AUTH_KEY_EXCH    0x0020
#define PCT2_CH_OFFER_AUTH_SIG         0x0040
#define PCT2_CH_OFFER_AUTH_PASSWORD    0x0080

/* SH auth types */
#define PCT2_SH_ACCEDE_KEY_EXCH_RECEIVE  0x0001
#define PCT2_SH_ACCEDE_AUTH_KEY_EXCH     0x0002
#define PCT2_SH_ACCEDE_AUTH_SIG          0x0004
#define PCT2_SH_ACCEDE_AUTH_PASSWORD     0x0008
#define PCT2_SH_ACCEPT_KEY_EXCH_SEND     0x0010
#define PCT2_SH_ACCEPT_AUTH_KEY_EXCH     0x0020
#define PCT2_SH_ACCEPT_AUTH_SIG          0x0040
#define PCT2_SH_ACCEPT_AUTH_PASSWORD     0x0080
#define PCT2_SH_ACCEPT_RECONNECT         0x0100


#define PCT2_HS_FLAG_TO_BE_CONTD  0x0001
#define PCT2_HS_FLAG_CONTINUATION 0x0002
#define PCT2_CONNECTION_ID_SIZE  30
#define PCT2_SESSION_ID_SIZE     30
#define PCT2_CHALLENGE_SIZE      30

#define PCT_VERSION_2   0x8002
#define PCT_CH_OFFSET_V2 0x0012
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
typedef WORD    ExtMsgSpec,    *PExtMsgSpec;

typedef WORD    Pct2MsgSpec , *PPct2MsgSpec;

typedef struct _PCT2_RECORD_HEADER {
    UCHAR   LengthMSB;
    UCHAR   LengthLSB;
    UCHAR   TypeMSB;
    UCHAR   TypeLSB;
} PCT2_RECORD_HEADER, * PPCT2_RECORD_HEADER;

typedef struct _PCT2_HS_MESSAGE_HEADER {
    PCT2_RECORD_HEADER  RecHeader;
    UCHAR               MessageTypeMsb;
    UCHAR               MessageTypeLsb;
    UCHAR               RecordFlagsMsb;
    UCHAR               RecordFlagsLsb;
} PCT2_HS_MESSAGE_HEADER, *PPCT2_HS_MESSAGE_HEADER;

typedef struct _PCT2_CLIENT_HELLO {
    UCHAR               SessionIdData[PCT2_SESSION_ID_SIZE];
    UCHAR               ChallengeData[PCT2_CHALLENGE_SIZE];
    UCHAR               VersionMsb;
    UCHAR               VersionLsb;
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
    /* Start of PCT2 specific data */

    UCHAR               MsgListLenMsb;
    UCHAR               MsgListLenLsb;
    UCHAR               SigListLenMsb;
    UCHAR               SigListLenLsb;
    UCHAR               CertifierListLenMsb;
    UCHAR               CertifierListLenLsb;
    UCHAR               QuickPublicValueLenMsb;
    UCHAR               QuickPublicValueLenLsb;
    UCHAR               QuickServerPublicValueLenMsb;
    UCHAR               QuickServerPublicValueLenLsb;
    UCHAR               QuickEncryptedKeyLenMsb;
    UCHAR               QuickEncryptedKeyLenLsb;
    UCHAR               AuthOptionsMsb;
    UCHAR               AuthOptionsLsb;

    UCHAR               VariantData[1];
} PCT2_CLIENT_HELLO, * PPCT2_CLIENT_HELLO;


typedef struct _PCT2_SERVER_HELLO {
    UCHAR               VersionMsb;
    UCHAR               VersionLsb;
    UCHAR               AuthOptionsMsb;
    UCHAR               AuthOptionsLsb;
   
    ExtCipherSpec       CipherSpecData;
    ExtHashSpec         HashSpecData;
    ExtExchSpec         ExchSpecData;
    ExtSigSpec          SigSpecData;

    UCHAR               ConnectionIdData[PCT2_CONNECTION_ID_SIZE];
    UCHAR               SessionIdData[PCT2_SESSION_ID_SIZE];
    UCHAR               AltCipherListLenMsb;
    UCHAR               AltCipherListLenLsb;
    UCHAR               AltHashListLenMsb;
    UCHAR               AltHashListLenLsb;
    UCHAR               MsgListLenMsb;
    UCHAR               MsgListLenLsb;
    UCHAR               ExchListLenMsb;
    UCHAR               ExchListLenLsb;
    UCHAR               CertListLenMsb;
    UCHAR               CertListLenLsb;
    UCHAR               QuickEncryptedKeyLenMsb;
    UCHAR               QuickEncryptedKeyLenLsb;
    UCHAR               ResponseLenMsb;
    UCHAR               ResponseLenLsb;
    UCHAR               CertifierListLenMsb;
    UCHAR               CertifierListLenLsb;
    UCHAR               PublicValueLenMsb;
    UCHAR               PublicValueLenLsb;
    UCHAR               SigCertLenMsb;
    UCHAR               SigCertLenLsb;

    UCHAR               VariantData[1];
} PCT2_SERVER_HELLO, * PPCT2_SERVER_HELLO;



typedef struct _PCT2_CLIENT_MASTER_KEY {

    UCHAR               EncryptedKeyLenMsb;
    UCHAR               EncryptedKeyLenLsb;
    UCHAR               ResponseLenMsb;
    UCHAR               ResponseLenLsb;
    UCHAR               PublicValueLenMsb;
    UCHAR               PublicValueLenLsb;
    UCHAR               SigCertLenMsb;
    UCHAR               SigCertLenLsb;
    UCHAR               VariantData[1];
} PCT2_CLIENT_MASTER_KEY, * PPCT2_CLIENT_MASTER_KEY;


typedef struct _PCT2_SERVER_VERIFY {

    UCHAR               EncryptedKeyLenMsb;
    UCHAR               EncryptedKeyLenLsb;
    UCHAR               ResponseLenMsb;
    UCHAR               ResponseLenLsb;
    UCHAR               SigCertLenMsb;
    UCHAR               SigCertLenLsb;
    UCHAR               VariantData[1];
} PCT2_SERVER_VERIFY, * PPCT2_SERVER_VERIFY;

typedef struct _PCT2_CLIENT_VERIFY {

    UCHAR               ResponseLenMsb;
    UCHAR               ResponseLenLsb;
    UCHAR               SigCertLenMsb;
    UCHAR               SigCertLenLsb;
    UCHAR               VariantData[1];
} PCT2_CLIENT_VERIFY, * PPCT2_CLIENT_VERIFY;

typedef struct _PCT2_PUBLIC_VALUE {
    UCHAR               ValueTypeMsb;
    UCHAR               ValueTypeLsb;
    UCHAR               UserSpecInfoLenMsb;
    UCHAR               UserSpecInfoLenLsb;
    UCHAR               Parameter1LenMsb;
    UCHAR               Parameter1LenLsb;
    UCHAR               Parameter2LenMsb;
    UCHAR               Parameter2LenLsb;
    UCHAR               VariantData[1];
} PCT2_PUBLIC_VALUE, *PPCT2_PUBLIC_VALUE, PCT2_QUICK_PUBLIC_VALUE, *PPCT2_QUICK_PUBLIC_VALUE;



typedef struct _PCT2_ENCRYPTED_KEY {
    UCHAR               EncryptedKey1LenMsb;
    UCHAR               EncryptedKey1LenLsb;
    UCHAR               EncryptedKey2LenMsb;
    UCHAR               EncryptedKey2LenLsb;
    UCHAR               KeyArgLenMsb;
    UCHAR               KeyArgLenLsb;
    UCHAR               VariantData[1];
} PCT2_ENCRYPTED_KEY, *PPCT2_ENCRYPTED_KEY, PCT2_QUICK_ENCRYPTED_KEY, *PPCT2_QUICK_ENCRYPTED_KEY;


/*
 *
 * Expanded Form Messages:
 *
 */

typedef struct _Pct2_Public_Value {
    DWORD           ValueType;
    DWORD           cbUserInfo;
    DWORD           cbParameter1;
    DWORD           cbParameter2;
    PUCHAR          pUserInfo;
    PUCHAR          pParameter1;
    PUCHAR          pParameter2;
} Pct2_Public_Value, *PPct2_Public_Value;

typedef struct _Pct2_Encrypted_Key_Data {
    DWORD           cbEncryptedKey1;
    DWORD           cbEncryptedKey2;
    DWORD           cbKeyArg;
    PUCHAR          pEncryptedKey1;
    PUCHAR          pEncryptedKey2;
    PUCHAR          pKeyArg;
} Pct2_Encrypted_Key, *PPct2_Encrypted_Key;

typedef struct _Pct2_Client_Hello {
    DWORD               AuthOptions;
    DWORD               cCipherSpecs;
    DWORD               cHashSpecs;
    DWORD               cCertSpecs;
    DWORD               cExchSpecs;
    DWORD               cbKeyArg;
    DWORD               cMessageSpecs;
    DWORD               cSigSpecs;
    DWORD               cbCertifiers;
	DWORD               cbSessionID;
	DWORD               cbChallenge;
    UCHAR               SessionID[PCT2_SESSION_ID_SIZE];
	UCHAR               Challenge[PCT2_CHALLENGE_SIZE];
    PUCHAR              pKeyArg;
    CipherSpec *        pCipherSpecs;
    HashSpec *          pHashSpecs;
    CertSpec *          pCertSpecs;
    ExchSpec *          pExchSpecs;
    Pct2MsgSpec *       pMessageSpecs;
    SigSpec *           pSigSpecs;
    PUCHAR              pCertifiers;
    PPct2_Public_Value  pQuickPublicValue;
    PPct2_Public_Value  pQuickServerPublicValue;
    PPct2_Encrypted_Key pQuickEncryptedKey; 
} Pct2_Client_Hello, * PPct2_Client_Hello;


typedef struct _Pct2_Server_Hello {
    DWORD           AuthOptions;
    CipherSpec      SrvCipherSpec;
    HashSpec        SrvHashSpec;
    CertSpec        SrvCertSpec;
    ExchSpec        SrvExchSpec;
    SigSpec         SrvSigSpec;

	DWORD           cbConnectionID;
    DWORD           cbSessionID;
    UCHAR           SessionID[PCT2_SESSION_ID_SIZE];
    UCHAR           ConnectionID[PCT2_SESSION_ID_SIZE];

    DWORD           cAltCipherSpecs;
    DWORD           cAltHashSpecs;
    DWORD           cMessageSpecs;
    DWORD           cExchSpecs;
    DWORD           cCertSpecs;
    DWORD           cbCertifiers;

    DWORD           cbSigCert;
    DWORD           cbResponse;

    CipherSpec *        pAltCipherSpecs;
    HashSpec *          pAltHashSpecs;
    Pct2MsgSpec *       pMessageSpecs;
    ExchSpec *          pExchSpecs;
    CertSpec *          pCertSpecs;
    PPct2_Encrypted_Key pQuickEncryptedKey; 
    PUCHAR              pResponse;

    PUCHAR              pCertifiers;
    PPct2_Public_Value  pPublicValue;
    PUCHAR              pSigCert;

    /* temporary data used in building server hello */
    PUCHAR          pCurUnpack;
    DWORD           UnpackState;
    DWORD           SubState;
} Pct2_Server_Hello, * PPct2_Server_Hello;
#define SH_UNPACK_CERTIFIERS   0
#define SH_UNPACK_PUBLIC_VALUE 1
#define SH_UNPACK_SIGCERT      2

typedef struct _Pct2_Client_Master_Key {
    DWORD           cbResponse;
    DWORD           cbSigCert;
    PPct2_Encrypted_Key pEncryptedKey; 
    PUCHAR          pResponse;
    PPct2_Public_Value  pPublicValue;
    PUCHAR          pSigCert;

    /* Helper members for unpacking */
    PUCHAR          pCurUnpack;
    DWORD           UnpackState;
    DWORD           SubState;
} Pct2_Client_Master_Key, * PPct2_Client_Master_Key;
#define CMK_UNPACK_PUBLIC_VALUE 0
#define CMK_UNPACK_SIGCERT      1

typedef struct _Pct2_Server_Verify {
    DWORD           cbResponse;
    DWORD           cbSigCert;
    PPct2_Encrypted_Key pEncryptedKey; 
    PUCHAR          pResponse;
    PUCHAR          pSigCert;

    /* Helper members for unpacking */
    PUCHAR          pCurUnpack;
    DWORD           SubState;
} Pct2_Server_Verify, * PPct2_Server_Verify;

typedef struct _Pct2_Client_Verify {
    DWORD           cbResponse;
    DWORD           cbSigCert;
    PUCHAR          pResponse;
    PUCHAR          pSigCert;
    /* Helper members for unpacking */
    PUCHAR          pCurUnpack;
    DWORD           SubState;
} Pct2_Client_Verify, * PPct2_Client_Verify;

/*
 *
 * Pickling Prototypes
 *
 */

SP_STATUS
Pct2PackClientHello(
    PPct2_Client_Hello       pCanonical,
    PSPBuffer          pCommOutput);

SP_STATUS
Pct2UnpackClientHello(
    PSPBuffer                pInput,
    PPct2_Client_Hello *     ppClient);

SP_STATUS
Pct2PackServerHello(
    PPct2_Server_Hello       pCanonical,
    PSPBuffer                pCommOutput);

SP_STATUS
Pct2UnpackServerHello(
    PSPBuffer                pInput,
    PPct2_Server_Hello *     ppServer);

SP_STATUS
Pct2PackClientMasterKey(
    PPct2_Client_Master_Key pCanonical,
    PSPBuffer               pCommOutput);

SP_STATUS
Pct2UnpackClientMasterKey(
    PSPBuffer                    pInput,
    PPct2_Client_Master_Key *    ppClient);

SP_STATUS
Pct2PackServerVerify(
    PPct2_Server_Verify    pCanonical,
    PSPBuffer              pCommOutput);

SP_STATUS
Pct2UnpackServerVerify(
    PSPBuffer                    pInput,
    PPct2_Server_Verify *        ppServer);

SP_STATUS
Pct2PackClientVerify(
    PPct2_Client_Verify    pCanonical,
    PSPBuffer              pCommOutput);

SP_STATUS
Pct2UnpackClientVerify(
    PSPBuffer                    pInput,
    PPct2_Client_Verify *        ppClient);



#endif /* __PCT1MSG_H__ */
