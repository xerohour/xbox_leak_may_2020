//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1995.
//
//  File:       msgs.h
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    8-02-95   RichardW   Created
//
//----------------------------------------------------------------------------

#ifndef __SSL3MSG_H__
#define __SSL3MSG_H__


#define CB_SSL3_SESSION_ID 	 32
#define CB_SSL3_RANDOM		 32
#define CB_SSL3_UNIXTIME	  4
#define CB_SSL3_PRE_MASTER_SECRET  48
#define CB_SSL3_MAX_SRV_KEY_EXCHANGE 500
#define CB_SSL3_PROTOLCOL	  2
#define CB_SSL3_SHA_PAD       40
#define CB_SSL3_MD5_PAD       48
#define CB_SSL3_MAX_CERT_VERIFY 500

#define CB_SLL3_CHANGE_CIPHER_SPEC    (sizeof(SWRAP) + 1)
#define CB_MD5DIGESTLEN 	  16
#define CB_A_SHA_DIGEST_LEN   20
#define CB_SSL3_FINISHED_MSG  (sizeof(FMWIRE))

#define CH_SLL3_MD5_PAD_CHAR	0x36
#define CH_SSL3_SHA_PAD_CHAR    0x5c

#define SSL3_NULL_WITH_NULL_NULL	    0x0000
#define SSL3_RSA_WITH_NULL_MD5		    0x0001
#define SSL3_RSA_WITH_NULL_SHA		    0x0002
#define SSL3_RSA_EXPORT_WITH_RC4_40_MD5     0x0003
#define SSL3_RSA_WITH_RC4_128_MD5	    0x0004
#define SSL3_RSA_WITH_RC4_128_SHA	    0x0005
#define SSL3_RSA_EXPORT_WITH_RC2_CBC_40_MD5 0x0006  // not supported
#define SSL3_RSA_WITH_IDEA_CBC_SHA	    0x0007  // not supported
#define SSL3_RSA_EXPORT_WITH_DES40_CBC_SHA  0x0008  // not supported
#define SSL3_RSA_WITH_DES_CBC_SHA	    0x0009  // not supported
#define SSL3_RSA_WITH_3DES_EDE_CBC_SHA	    0x000A  // not supported

#define SSL_RSA_FINANCE64_WITH_RC4_64_MD5    0x0080
#define SSL_RSA_FINANCE64_WITH_RC4_64_SHA    0x0081

#define  SSL3_HS_HELLO_REQUEST			0x00
#define  SSL3_HS_CLIENT_HELLO			0x01
#define  SSL3_HS_SERVER_HELLO			0x02
#define  SSL3_HS_CERTIFICATE			0x0B
#define  SSL3_HS_SERVER_KEY_EXCHANGE 	0x0C
#define  SSL3_HS_CERTIFICATE_REQUEST	0x0D
#define  SSL3_HS_SERVER_HELLO_DONE		0x0E
#define  SSL3_HS_CERTIFICATE_VERIFY		0x0F
#define  SSL3_HS_CLIENT_KEY_EXCHANGE	0x10
#define  SSL3_HS_FINISHED				0x14
#define  SSL3_CT_CHANGE_CIPHER_SPEC     20
#define  SSL3_CT_ALERT					21
#define  SSL3_CT_HANDSHAKE			    22
#define  SSL3_CT_APPLICATIONDATA		23
#define  SSL3_NULL_WRAP					15
#define  SSL3_CERTTYPE_RSA_SIGN			1
#define SSL3_CLIENT_VERSION_MSB			0x03
#define SSL3_CLIENT_VERSION_LSB			0x00
#define CB_SSL3_CERT_VECTOR   			3
#define CB_SSL3_ALERT					(sizeof(SWRAP) +2)

// Alert levels
#define SSL3_ALERT_WARNING		1
#define SSL3_ALERT_FATAL		2

// Alert message types
#define SSL3_ALERT_CLOSE_NOTIFY 	0
#define SSL3_ALERT_UNEXPECTED_MESSAGE	10
#define SSL3_ALERT_BAD_RECORD_MAC	20
#define SSL3_ALERT_DECOMPRESSION_FAIL	30
#define SSL3_ALERT_HANDSHAKE_FAILURE	40
#define SSL3_ALERT_NO_CERTIFICATE	41
#define SSL3_ALERT_BAD_CERTIFICATE	42
#define SSL3_ALERT_UNSUPPORTED_CERT	43
#define SSL3_ALERT_CERTIFICATE_REVOKED	44
#define SSL3_ALERT_CERTIFICATE_EXPIRED	45
#define SSL3_ALERT_CERTIFICATE_UNKNOWN	46
#define SSL3_ALERT_ILLEGAL_PARAMETER	47

#define SSL3_MASTER_KEY_SIZE            16
#define SSL3_ENCRYPTED_KEY_SIZE         272
#define SSL3_MAX_MESSAGE_LENGTH         32768
#define MAX_DIGEST_LEN                  32
#define SSL3_CLIENT_VERSION             0x0300

#define SSL3_CHANGE_CIPHER_MSG 	{ 0x14, 0x03, 0x00, 0x00, 0x01, 0x01 }

#define FSsl3CipherClient() (0 != pContext->wS3CipherSuiteClient)
#define FSsl3CipherServer() (0 != pContext->wS3CipherSuiteServer)

#define FSsl3Cipher(fClient) ((fClient ? pContext->wS3CipherSuiteClient : pContext->wS3CipherSuiteServer))
extern PctPrivateKey *g_pSsl3Private;
extern PctPublicKey *g_pSsl3Public;

#define MS24BOF(x)    ((UCHAR) ((x >> 16) & 0xFF) )

extern const Ssl2CipherMap Ssl3CipherRank[];
extern const DWORD Ssl3NumCipherRanks;

extern CertSpec Ssl3CertRank[];
extern const DWORD Ssl3NumCert;

typedef struct _shsh  //Structure hand shake header
	{
	UCHAR   typHS;
	UCHAR   bcb24;
	UCHAR   bcbMSB;
	UCHAR   bcbLSB;
	} SHSH;


typedef struct _swrap
	{
	UCHAR  bCType;
	UCHAR  bMajor;
	UCHAR  bMinor;
	UCHAR  bcbMSBSize;
	UCHAR  bcbLSBSize;
//	UCHAR  rgb[];
	} SWRAP;
	
typedef struct _ssh
	{
	SHSH;
	UCHAR 	bMajor;
	UCHAR   bMinor;
	UCHAR   rgbRandom[CB_SSL3_RANDOM];
	UCHAR   cbSessionId;
	UCHAR   rgbSessionId[CB_SSL3_SESSION_ID];
	UCHAR	wCipherSelectedMSB;
	UCHAR	wCipherSelectedLSB;
	UCHAR 	bCMSelected;
	} SSH;


typedef struct _alrt
	{
	SWRAP;
	UCHAR bAlertLevel;
	UCHAR bAlertDesc;
	} ALRT;

	
typedef struct _fm
	{
	UCHAR	rgbMD5[CB_MD5DIGESTLEN];
	UCHAR   rgbSHA[CB_A_SHA_DIGEST_LEN];
	} FM; //finished message

typedef struct _fmwire
	{
	SWRAP;
	SHSH;
	FM;
	} FMWIRE;

typedef struct _clh
	{
	SHSH;
	UCHAR 	bMajor;
	UCHAR   bMinor;
	UCHAR   rgbRandom[CB_SSL3_RANDOM];
	UCHAR   cbSessionId;
	UCHAR   rgbSessionId[CB_SSL3_SESSION_ID];
	UCHAR   bMSBCipher;
	UCHAR	bLSBCipher;
	// short   rgbCipher[0];  // ALignment problem, but never used.
	//UCHAR	bCM;
	//UCHAR rgbCM[bCM];
	} CLH ;	//CLient Hello	

								
typedef struct _cert
	{
	SHSH;
	UCHAR bcbClist24;
	UCHAR bcbMSBClist;
	UCHAR bcbLSBClist;
	UCHAR bcbCert24;
	UCHAR bcbMSBCert;
	UCHAR bcbLSBCert;
	UCHAR rgbCert[];
	/* followed by the real cert */
	} CERT;

	
typedef struct _certReq
    {
    SHSH;
    UCHAR bcbCertType; //This value be just 1 byte
    UCHAR bCertType;  //This will be Only one for quite sometime
    UCHAR bcbMSBList;
    UCHAR bcbLSBList;
    UCHAR rgbIss[];
    } CERTREQ;

typedef struct _shs
	{
	UCHAR	typHs;  //type of handshake
	UCHAR	cb24;
	short	cb16;
	union
		{
//		struct SHR  shr;
//		struct SCH  sch;
		SSH  ssh;
		CERT cert;
//		struct SRVKEYEXCH srvkeyex;
//		struct CERTRQ certrq;
//		struct SRVDONE srvdone;
//		struct SRVVER srvver;
//		struct CLKEYEX clkeyex;
//		struct FINISH  finish;
		} ;
	} SHS;

typedef struct _shwire
	{
	PUCHAR  pcbCipher;
	PUCHAR  pCiperSpec;
	PUCHAR  pcbCompM;
	PUCHAR  pCompM;
	PUCHAR  pcbCert;
	PUCHAR  pCert;
	PUCHAR  pHelloDone;
	} SHWIRE ;


SP_STATUS WINAPI
Ssl3DecryptHandler(
    PSPContext pContext,
    PSPBuffer pCommInput,
    PSPBuffer pAppOutput);

SP_STATUS WINAPI
GenerateSsl3ClientHello(
    PSPContext             pContext,
    PSPBuffer               pOutput);
	
void Ssl3BuildMasterKeys(PSPContext pContext, PUCHAR pbPreMaster);

SP_STATUS
Ssl3PackClientHello(PSPContext pContext,
    PSsl2_Client_Hello       pCanonical,
    PSPBuffer          pCommOutput);



SP_STATUS Ssl3CliHandleServerHello(PSPContext pContext,
                                   PUCHAR pSrvHello,
                                   DWORD cbMessage,
                                   PSPBuffer  pCommOutput);
SP_STATUS Ssl3HandleServerFinish(PSPContext pContext,
                                   PUCHAR pSrvHello,
                                   DWORD cbMessage,
                                   PSPBuffer  pCommOutput);


SP_STATUS Ssl3SrvHandleCMKey(PSPContext pContext,
                              PUCHAR  pCommInput,
                              DWORD cbMsg,
                              PSPBuffer  pCommOutput);

BOOL FVerifyFinishedMessage(PSPContext  pContext, PUCHAR pb, BOOL fClient);

SP_STATUS
ParseAlertMessage
(
PSPContext pContext,
PUCHAR pSrvHello,
DWORD cbMessage,
PSPBuffer  pCommOutput
);

SP_STATUS
Ssl3SelectCipher    (
    PSPContext pContext,
    DWORD   dwCipher
);

void BuildAlertMessage(PBYTE pb, UCHAR bAlertLevel, UCHAR bAlertDesc);
void Ssl3BuildFinishMessage(PSPContext pContext, BYTE *pbMd5Digest, BYTE *pbSHADigest, BOOL fClient);
void SetWrap(PSPContext pContext, PUCHAR pb, UCHAR bCType, DWORD wT, BOOL fClient);
void SetHandshake(PUCHAR pb, BYTE bHandshake, PUCHAR pbData, WORD wSize);
void   UpdateHandshakeHash(PSPContext pContext, PUCHAR pb, DWORD dwcb, BOOL fInit);

void BuildS3FinalFinish(PSPContext pContext, PUCHAR pb, BOOL fClient);
SP_STATUS
VerifyCCSAndFinishMsg(PSPContext pContext, PBYTE pbMsg, DWORD cbMessage, BOOL fClient);


void ComputeCertVerifyHashes(PSPContext pContext, PBYTE pbHash);

void BuildCertificateMessage(PBYTE pb, PBYTE rgbCert, DWORD dwCert);
SP_STATUS
BuildCCSAndFinishMessage
(
PSPContext pContext,
PUCHAR pb,
DWORD cbMessage,
BOOL fClient
);
SP_STATUS
ProcessCertificateMessage
(
PSPContext pContext,
BOOL fServer,
DWORD   dwCipher,
CERT *pcert,
DWORD dwCert,
Ssl2_Client_Master_Key *pKey
);
VOID
ReverseMemCopy(
    PUCHAR      Dest,
    PUCHAR      Source,
    ULONG       Size) ;

SP_STATUS WINAPI
Ssl3ServerProtocolHandler(
    PSPContext pContext,
    PSPBuffer pCommInput,
    PSPBuffer pCommOutput,
    PSPBuffer pAppOutput);
void Ssl3GetCipherSpec(
    short  wCipherSuite, // in
    PINT pwKeySize,    // out
    PINT pwMacSize) ; // out
VOID ComputeServerExchangeHashes(
	PSPContext pContext,
    PBYTE pbServerParams,      // in
    INT   iServerParamsLen,    // in
    PBYTE pbMd5HashVal,        // out
    PBYTE pbShaHashVal) ;       // out

SP_STATUS
UnwrapSsl3Message
(
PSPContext pContext,
PSPBuffer MsgInput
);

void SetWrapNoEncrypt(PUCHAR pb, UCHAR bCType, DWORD wT);

SP_STATUS Ssl3HandleCCS(PSPContext pContext,
				   PUCHAR pb,
				   DWORD cbMessage,
				   BOOL fClient);
SP_STATUS
VerifyFinishMsg(PSPContext pContext, PBYTE pbMsg, DWORD cbMessage, BOOL fClient);
SP_STATUS
WrapSsl3Message
(
PSPContext pContext,
PSPBuffer pMsgInput,
UCHAR bContentType
);

BOOL Ssl3ParseClientHello
(
PSPContext  pContext,
PBYTE pbMessage,
INT iMessageLen,
BOOL *pfRestart
);

#endif //__SSL3MSG_H__
