
#ifndef _PCT1PROT_H_
#define _PCT1PROT_H_


/* key derivation separators */

#define PCT_CONST_CWK			"cw"
#define PCT_CONST_CWK_LEN		2

#define PCT_CONST_SWK			"svw"
#define PCT_CONST_SWK_LEN		3

#define PCT_CONST_CMK			"cmac"
#define PCT_CONST_CMK_LEN		4

#define PCT_CONST_SMK			"svmac"
#define PCT_CONST_SMK_LEN		5

#define PCT_CONST_SR			"sv"
#define PCT_CONST_SR_LEN		2

#define PCT_CONST_SLK			"sl"
#define PCT_CONST_SLK_LEN		2

#define PCT_CONST_RESP			"sr"
#define PCT_CONST_RESP_LEN		2

#define PCT_CONST_VP			"cvp"
#define PCT_CONST_VP_LEN		3
#define PCT_USE_CERT					1
#define PCT_MAKE_MAC					2

#define DERIVATION_BUFFER_SIZE			2048

/* data structs */

/* message constants and types */

#define PCT_SESSION_ID_SIZE     32
#define PCT_CHALLENGE_SIZE      32

#define PCT_SIGNATURE_SIZE		8192	/* maximum signature size */

#define MASTER_KEY_SIZE     16
#define ENCRYPTED_KEY_SIZE  272         /* Allows for 2048 bit keys */
#define CERT_SIZE           1024
#define RESPONSE_SIZE       32      /* allows for hash output growth */

#define PCT_MAX_SHAKE_LEN	32768		/* longest handshake message len */
/* implementation constants */

#define PCT_MAX_NUM_SEP			5
#define PCT_MAX_SEP_LEN			5

/* mismatch vector */

#define PCT_NUM_MISMATCHES		6

#define PCT_IMIS_CIPHER			1
#define PCT_IMIS_HASH			2
#define PCT_IMIS_CERT			4
#define PCT_IMIS_EXCH			8
#define PCT_IMIS_CL_CERT		16
#define PCT_IMIS_CL_SIG			32


SP_STATUS WINAPI
Pct1ServerProtocolHandler(
    PSPContext pContext,
    PSPBuffer pCommInput,
    PSPBuffer pCommOutput,
    PSPBuffer pAppOutput);

SP_STATUS WINAPI
Pct1ClientProtocolHandler(
    PSPContext pContext,
    PSPBuffer pCommInput,
    PSPBuffer pCommOutput,
    PSPBuffer pAppOutput);

SP_STATUS WINAPI
Pct1DecryptHandler(
    PSPContext pContext,
    PSPBuffer  pCommInput,
    PSPBuffer  pAppOutput);


SP_STATUS WINAPI Pct1DecryptMessage(PSPContext pContext,
                              PSPBuffer  pCommInput,
                              PSPBuffer  pAppOutput);

SP_STATUS WINAPI Pct1EncryptRaw(PSPContext  pContext,
                             PSPBuffer      pAppInput,
                             PSPBuffer      pCommOutput,
                             DWORD          dwFlags);

#define PCT1_ENCRYPT_ESCAPE 0x00000001


SP_STATUS WINAPI Pct1EncryptMessage(PSPContext pContext,
                             PSPBuffer  pAppInput,
                             PSPBuffer  pCommOutput);



SP_STATUS Pct1SrvHandleClientHello(PSPContext pContext,
                              PSPBuffer  pCommInput,
                              PPct1_Client_Hello pHello,
                              PSPBuffer  pCommOutput);

SP_STATUS Pct1SrvHandleCMKey(PSPContext pContext,
                              PSPBuffer  pCommInput,
                              PSPBuffer  pCommOutput);

SP_STATUS Pct1CliHandleServerHello(PSPContext pContext,
                              PSPBuffer  pCommInput,
                              PPct1_Server_Hello  pHello,
                              PSPBuffer  pCommOutput);

SP_STATUS Pct1CliHandleServerVerify(PSPContext pContext,
                              PSPBuffer  pCommInput,
                              PSPBuffer  pCommOutput);

SP_STATUS Pct1HandleError(PSPContext pContext,
                              PSPBuffer  pCommInput,
                              PSPBuffer  pCommOutput);

SP_STATUS Pct1GenerateError(PSPContext pContext,
                              PSPBuffer  pCommOutput,
                              SP_STATUS  pError,
                              PSPBuffer  pErrData);


SP_STATUS
WINAPI
GeneratePct1StyleHello(
    PSPContext              pContext,
    PSPBuffer               pOutput);

SP_STATUS
Pct1CliRestart(PSPContext  pContext,
              PPct1_Server_Hello pHello,
              PSPBuffer pCommOutput);

SP_STATUS
Pct1SrvRestart(PSPContext  pContext,
              PPct1_Client_Hello pHello,
              PSPBuffer pCommOutput,
              SessCacheItem *pSession);

SP_STATUS
Pct1MakeSessionKeys(PSPContext pContext);

SP_STATUS Pct1EndVerifyPrelude(PSPContext pContext,
                               PUCHAR     VerifyPrelude,
                               DWORD *    pcbVerifyPrelude
                               );

SP_STATUS Pct1BeginVerifyPrelude(PSPContext pContext,
                                 PUCHAR     pClientHello,
                                 DWORD      cbClientHello,
                                 PUCHAR     pServerHello,
                                 DWORD      cbServerHello);


SP_STATUS Pct1BuildRestartResponse(PSPContext pContext,
                                   PUCHAR     Response,
                                   DWORD *    pcbResponse
                               );

extern CipherSpec Pct1CipherRank[];
extern const DWORD Pct1NumCipher;

/* available hashes, in order of preference */
extern HashSpec Pct1HashRank[];
extern const DWORD Pct1NumHash;

extern CertSpec Pct1CertRank[];
extern const DWORD Pct1NumCert;

extern SigSpec Pct1SigRank[];
extern const DWORD Pct1NumSig;

extern ExchSpec Pct1ExchRank[];
extern const DWORD Pct1NumExch;


#endif /* _PCT1PROT_H_ */
