
#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

extern CipherSpec PctCipherRank[];
extern DWORD CipherRanks;

extern HashSpec PctHashRank[];
extern DWORD HashRanks;

extern CertSpec PctCertRank[];
extern DWORD CertRanks;


extern SigSpec PctSigRank[];
extern DWORD SigRanks;

extern ExchSpec PctExchRank[];
extern DWORD ExchRanks;

typedef struct _SPContext SPContext, *PSPContext;


typedef struct _UNICipherMap {
    DWORD             CipherKind;
    DWORD             fProt;
	HashSpec          Hash;
	CipherSpec        Cipher;
	ExchSpec          KeyExch;
} UNICipherMap, *PUNICipherMap;



SP_STATUS WINAPI
ServerProtocolHandler(PSPContext pContext,
    PSPBuffer  pCommInput,
    PSPBuffer  pCommOutput,
    PSPBuffer  pAppOutput);

SP_STATUS WINAPI
ClientProtocolHandler(PSPContext pContext,
    PSPBuffer  pCommInput,
    PSPBuffer  pCommOutput,
    PSPBuffer  pAppOutput);

SP_STATUS WINAPI
GenerateHello(
    PSPContext              pContext,
    PSPBuffer               pOutput,
    BOOL                    fCache);

SP_STATUS WINAPI
GenerateUniHello(
    PSPContext             pContext,
    PSPBuffer               pOutput,
    DWORD                   fProtocol
    );


// ProtEnabled is what is enabled for the current
extern DWORD g_ProtEnabled;
extern DWORD g_ProtSupported;

typedef SP_STATUS ( WINAPI * SPInitiateHelloFn)(
                    PSPContext             pContext,
                    PSPBuffer              pOutput,
                    BOOL                   fCache);

typedef SP_STATUS ( WINAPI * SPProtocolHandlerFn)(PSPContext pContext,
                              PSPBuffer  pCommInput,
                              PSPBuffer  pCommOutput,
                              PSPBuffer  pAppOutput);

typedef SP_STATUS ( WINAPI * SPDecryptHandlerFn)(PSPContext pContext,
                              PSPBuffer  pCommInput,
                              PSPBuffer  pAppOutput);


typedef SP_STATUS ( WINAPI * SPDecryptMessageFn)(PSPContext pContext,
                              PSPBuffer  pCommInput,
                              PSPBuffer  pAppOutput);

typedef SP_STATUS ( WINAPI * SPEncryptMessageFn)(PSPContext pContext,
                             PSPBuffer  pAppInput,
                             PSPBuffer  pCommOutput);


/* State machine states */

#define SP_STATE_NONE                   0x00
#define PCT1_STATE_CLIENT_HELLO		    0x01
#define PCT1_STATE_SERVER_HELLO		    0x02
#define PCT1_STATE_CLIENT_MASTER_KEY	0x03
#define PCT1_STATE_SERVER_VERIFY		0x04
#define PCT1_STATE_ERROR				0x05
#define PCT1_STATE_RENEGOTIATE          0x06

#define SSL2_STATE_CLIENT_HELLO			0x11
#define SSL2_STATE_SERVER_HELLO			0x12
#define SSL2_STATE_CLIENT_MASTER_KEY	0x13
#define SSL2_STATE_CLIENT_FINISH		0x14
#define SSL2_STATE_SERVER_VERIFY  		0x15
#define SSL2_STATE_SERVER_FINISH  		0x16
#define SSL2_STATE_REQUEST_CERTIFICATE  0x17
#define SSL2_STATE_CLIENT_CERTIFICATE	0x18
#define SSL2_STATE_SERVER_RESTART       0x19
#define SSL2_STATE_CLIENT_RESTART       0x1a
#define SSL3_STATE_CLIENT_HELLO         0x1b
#define SSL3_STATE_CHANGE_CIPHER_SPEC   0x1c
#define SSL3_STATE_RESTART_CCS          0x1d
#define SSL3_STATE_RESTART_SERVER_FINISH 0x1e
#define SSL3_STATE_SERVER_FINISH		0x1f
#define SSL3_STATE_HELLO_REQUEST        0x20
#define UNI_STATE_RECVD_UNIHELLO        0xfe
#define UNI_STATE_CLIENT_HELLO			0xff
#define SSL3_STATE_CLIENT_FINISH		0x21
#define SSL3_STATE_RESTART_CLI_FINISH   0x22
#define SSL3_STATE_REDO					0x23  //Temp...
#define SSL3_STATE_REDO_RESTART         0x24



#define SP_STATE_CONNECTED      0x0000ffff  /* We are connected, and are
                                             * expecting data packets, otherwise
                                             * we are performing a protocol
                                             * negotiation lower word contains
                                             * last message sent, implying what
                                             * the next word will be */


// UNIHELLO codes.

#define PCT_SSL_COMPAT                  0x8f
#define PCT_SSL_CERT_TYPE               0x80
#define PCT_SSL_HASH_TYPE               0x81
#define PCT_SSL_EXCH_TYPE               0x82
#define PCT_SSL_CIPHER_TYPE_1ST_HALF    0x83
#define PCT_SSL_CIPHER_TYPE_2ND_HALF    0x84

#define UNI_CK_PCT  SSL_MKFAST(PCT_SSL_COMPAT, MSBOF(PCT_VERSION_1), LSBOF(PCT_VERSION_1))
#define PCT_SSL_CERT_X509  SSL_MKFAST(0x80, 0x00, 0x00)
#define PCT_SSL_CERT_PKCS7 SSL_MKFAST(0x80, 0x00, 0x01)

#endif /* _PROTOCOL_H_ */
