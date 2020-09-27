//+-----------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation
//
//------------------------------------------------------------------------

#include "xonp.h"
#include "xboxp.h"
#include "xonver.h"
#include <md4.h>

NTSTATUS rc4HmacEncrypt(CScratchMemoryAllocator * pScratchMemoryAllocator, PUCHAR pbKey, ULONG KeySize,
                        ULONG MessageType, PUCHAR pbInput, ULONG cbInput, PUCHAR pbOutput, PULONG pcbOutput);
NTSTATUS rc4HmacDecrypt(PUCHAR pbKey, ULONG KeySize, ULONG MessageType, PUCHAR pbInput, ULONG cbInput,
                        PUCHAR pbOutput, PULONG pcbOutput);

#define RC4_CONFOUNDER_LEN  8

typedef struct _RC4_MDx_HEADER {
    UCHAR Checksum[MD5_LEN];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
} RC4_MDx_HEADER, *PRC4_MDx_HEADER;

const XBOX_LIBRARY_VERSION g_ClientVersion = { VER_PRODUCTVERSION };

//
// Sizes for scratch memory during various request
//

#define SCRATCH_MEMORY_SIZE_FOR_BuildAsRequest 2000
#define SCRATCH_MEMORY_SIZE_FOR_VerifyAsReply 2000
#define SCRATCH_MEMORY_SIZE_FOR_BuildTgsRequest 3000
#define SCRATCH_MEMORY_SIZE_FOR_VerifyTgsReply 3000
#define SCRATCH_MEMORY_SIZE_FOR_BuildApRequest 2000
#define SCRATCH_MEMORY_SIZE_FOR_VerifyApReply 2000

//
// Please be very careful when moving around code in this cpp file.
// Make sure you read the scratch memory allocator code and
// understand how memory is manipulated before change the code.
//

//DefineTag(ScratchMemoryVerbose,TAG_ENABLE);
DefineTag(ScratchMemoryVerbose,0);
DefineTag(ScratchMemoryAlloc,0);

DefineTag(KerbVerbose,TAG_ENABLE);
DefineTag(KerbWarn,TAG_ENABLE);
DefineTag(KerbASN1,0);

// ---------------------------------------------------------------------------------------

typedef LONG KERBERR, *PKERBERR;

typedef KERB_HOST_ADDRESS *PKERB_HOST_ADDRESS;
typedef KERB_PRINCIPAL_NAME *PKERB_PRINCIPAL_NAME;
typedef KERB_REALM *PKERB_REALM;
typedef KERB_TIME *PKERB_TIME;
typedef KERB_ENCRYPTED_DATA *PKERB_ENCRYPTED_DATA;
typedef KERB_TICKET *PKERB_TICKET;
typedef KERB_TRANSITED_ENCODING *PKERB_TRANSITED_ENCODING;
typedef KERB_ENCRYPTION_KEY *PKERB_ENCRYPTION_KEY;
typedef KERB_ENCRYPTED_TICKET *PKERB_ENCRYPTED_TICKET;
typedef KERB_CHECKSUM *PKERB_CHECKSUM;
typedef KERB_AUTHENTICATOR *PKERB_AUTHENTICATOR;
typedef KERB_PA_DATA *PKERB_PA_DATA;
typedef KERB_KDC_REQUEST_BODY *PKERB_KDC_REQUEST_BODY;
typedef KERB_KDC_REQUEST *PKERB_KDC_REQUEST;
typedef KERB_AS_REQUEST *PKERB_AS_REQUEST;
typedef KERB_TGS_REQUEST *PKERB_TGS_REQUEST;
typedef KERB_KDC_REPLY *PKERB_KDC_REPLY;
typedef KERB_AS_REPLY *PKERB_AS_REPLY;
typedef KERB_TGS_REPLY *PKERB_TGS_REPLY;
typedef KERB_ENCRYPTED_KDC_REPLY *PKERB_ENCRYPTED_KDC_REPLY;
typedef KERB_ENCRYPTED_AS_REPLY *PKERB_ENCRYPTED_AS_REPLY;
typedef KERB_ENCRYPTED_TGS_REPLY *PKERB_ENCRYPTED_TGS_REPLY;
typedef KERB_AP_OPTIONS *PKERB_AP_OPTIONS;
typedef KERB_AP_REQUEST *PKERB_AP_REQUEST;
typedef KERB_AP_REPLY *PKERB_AP_REPLY;
typedef KERB_ENCRYPTED_AP_REPLY *PKERB_ENCRYPTED_AP_REPLY;
typedef KERB_SAFE_BODY *PKERB_SAFE_BODY;
typedef KERB_SAFE_MESSAGE *PKERB_SAFE_MESSAGE;
typedef KERB_PRIV_MESSAGE *PKERB_PRIV_MESSAGE;
typedef KERB_ENCRYPTED_PRIV *PKERB_ENCRYPTED_PRIV;
typedef KERB_ERROR *PKERB_ERROR;
typedef KERB_ERROR_METHOD_DATA *PKERB_ERROR_METHOD_DATA;
typedef struct PKERB_AUTHORIZATION_DATA_s KERB_AUTHORIZATION_DATA;
typedef struct PKERB_TICKET_EXTENSIONS_s KERB_TICKET_EXTENSIONS;
typedef KERB_CRED *PKERB_CRED;
typedef KERB_ENCRYPTED_CRED *PKERB_ENCRYPTED_CRED;
typedef KERB_CRED_INFO *PKERB_CRED_INFO;
typedef struct PKERB_LAST_REQUEST_s KERB_LAST_REQUEST;
typedef struct PKERB_HOST_ADDRESSES_s KERB_HOST_ADDRESSES;
typedef struct PKERB_ETYPE_INFO_s KERB_ETYPE_INFO;
typedef KERB_ETYPE_INFO_ENTRY * PKERB_ETYPE_INFO_ENTRY;
typedef KERB_ENCRYPTED_TIMESTAMP *PKERB_ENCRYPTED_TIMESTAMP;
typedef struct PKERB_PREAUTH_DATA_LIST_s KERB_PREAUTH_DATA_LIST, *PKERB_PREAUTH_DATA_LIST;
typedef KERB_TICKET_FLAGS *PKERB_TICKET_FLAGS;
typedef KERB_PA_PAC_REQUEST *PKERB_PA_PAC_REQUEST;

#ifndef MIDL_PASS

typedef struct KERB_PRINCIPAL_NAME_name_string_s KERB_PRINCIPAL_NAME_ELEM, *PKERB_PRINCIPAL_NAME_ELEM;
typedef struct KERB_KDC_REQUEST_BODY_encryption_type_s KERB_CRYPT_LIST, *PKERB_CRYPT_LIST;
typedef struct KERB_KDC_REQUEST_BODY_additional_tickets_s KERB_TICKET_LIST, *PKERB_TICKET_LIST;
typedef struct KERB_KDC_REQUEST_preauth_data_s KERB_PA_DATA_LIST, *PKERB_PA_DATA_LIST;
typedef struct KERB_KDC_REPLY_preauth_data_s KERB_REPLY_PA_DATA_LIST, *PKERB_REPLY_PA_DATA_LIST;
typedef struct KERB_CRED_tickets_s KERB_CRED_TICKET_LIST, *PKERB_CRED_TICKET_LIST;
typedef struct KERB_ENCRYPTED_CRED_ticket_info_s KERB_CRED_INFO_LIST, *PKERB_CRED_INFO_LIST;
typedef struct KERB_PA_PK_AS_REQ2_user_certs_s KERB_CERTIFICATE_LIST, *PKERB_CERTIFICATE_LIST;
typedef struct KERB_PA_PK_AS_REQ2_trusted_certifiers_s KERB_CERTIFIER_LIST, *PKERB_CERTIFIER_LIST;
typedef struct KERB_KDC_ISSUED_AUTH_DATA_elements_s KERB_KDC_AUTH_DATA_LIST, *PKERB_KDC_AUTH_DATA_LIST;

#endif // MIDL_PASS

typedef KERB_KDC_ISSUED_AUTH_DATA *PKERB_KDC_ISSUED_AUTH_DATA;
typedef struct PKERB_IF_RELEVANT_AUTH_DATA_ KERB_IF_RELEVANT_AUTH_DATA;
typedef KERB_DH_PARAMTER *PKERB_DH_PARAMTER;
typedef KERB_PA_PK_AS_REQ *PKERB_PA_PK_AS_REQ;
typedef KERB_PA_PK_AS_REQ2 *PKERB_PA_PK_AS_REQ2;
typedef KERB_SIGNED_AUTH_PACKAGE *PKERB_SIGNED_AUTH_PACKAGE;
typedef KERB_AUTH_PACKAGE *PKERB_AUTH_PACKAGE;
typedef KERB_PK_AUTHENTICATOR *PKERB_PK_AUTHENTICATOR;
typedef KERB_SIGNED_REPLY_KEY_PACKAGE *PKERB_SIGNED_REPLY_KEY_PACKAGE;
typedef struct ASN1objectidentifier_s KERB_OBJECT_ID, *PKERB_OBJECT_ID;
typedef KERB_REPLY_KEY_PACKAGE *PKERB_REPLY_KEY_PACKAGE;
typedef KERB_PA_PK_AS_REP *PKERB_PA_PK_AS_REP;
typedef KERB_PA_PK_AS_REP2 *PKERB_PA_PK_AS_REP2;
typedef KERB_CERTIFICATE *PKERB_CERTIFICATE;
typedef KERB_SIGNED_KDC_PUBLIC_VALUE *PKERB_SIGNED_KDC_PUBLIC_VALUE;
typedef KERB_SUBJECT_PUBLIC_KEY_INFO *PKERB_SUBJECT_PUBLIC_KEY_INFO;
typedef KERB_ALGORITHM_IDENTIFIER *PKERB_ALGORITHM_IDENTIFIER;
typedef KERB_SIGNATURE *PKERB_SIGNATURE;
typedef KERB_TGT_REPLY *PKERB_TGT_REPLY;
typedef KERB_TGT_REQUEST *PKERB_TGT_REQUEST;
typedef KERB_PA_SERV_REFERRAL *PKERB_PA_SERV_REFERRAL;
typedef KERB_CHANGE_PASSWORD_DATA *PKERB_CHANGE_PASSWORD_DATA;

#define                     KERB_KDC_OPTIONS_reserved 0x80000000
#define                     KERB_KDC_OPTIONS_forwardable 0x40000000
#define                     KERB_KDC_OPTIONS_forwarded 0x20000000
#define                     KERB_KDC_OPTIONS_proxiable 0x10000000
#define                     KERB_KDC_OPTIONS_proxy 0x08000000
#define                     KERB_KDC_OPTIONS_postdated 0x02000000
#define                     KERB_KDC_OPTIONS_allow_postdate 0x04000000
#define                     KERB_KDC_OPTIONS_unused7 0x01000000
#define                     KERB_KDC_OPTIONS_renewable 0x00800000
#define                     KERB_KDC_OPTIONS_unused9 0x00400000
#define                     KERB_KDC_OPTIONS_name_canonicalize 0x00010000
#define                     KERB_KDC_OPTIONS_renewable_ok 0x00000010
#define                     KERB_KDC_OPTIONS_enc_tkt_in_skey 0x00000008
#define                     KERB_KDC_OPTIONS_renew 0x00000002
#define                     KERB_KDC_OPTIONS_validate 0x00000001

#define                     KERB_AP_OPTIONS_reserved 0x80000000
#define                     KERB_AP_OPTIONS_use_session_key 0x40000000
#define                     KERB_AP_OPTIONS_mutual_required 0x20000000
#define                     KERB_AP_OPTIONS_reserved1 0x00000001

//
// these #define's are done for the conversion from the old oss compiler to the
// new telis compiler.
//
#define KERB_AUTHENTICATOR_sequence_number                      sequence_number
#define KERB_ENCRYPTED_TICKET_client_addresses                  client_addresses
#define KERB_ENCRYPTED_TICKET_renew_until                       renew_until
#define KERB_CRED_INFO_renew_until                              renew_until
#define KERB_KDC_REQUEST_BODY_renew_until                       renew_until
#define KERB_KDC_REQUEST_BODY_server_name                       server_name
#define KERB_KDC_REQUEST_preauth_data                           preauth_data
#define KERB_AUTHENTICATOR_sequence_number                      sequence_number
#define KERB_ENCRYPTED_AP_REPLY_sequence_number                 sequence_number
#define KERB_AUTHENTICATOR_subkey                               subkey
#define KERB_ENCRYPTED_AP_REPLY_subkey                          subkey
#define KERB_TGT_REQUEST_server_name                            server_name
#define KERB_ERROR_client_name                                  client_name
#define KERB_ENCRYPTED_TIMESTAMP_usec                           usec
#define KERB_KDC_REQUEST_BODY_client_name                       client_name
#define KERB_KDC_REPLY_preauth_data                             preauth_data
#define KERB_ENCRYPTED_TIMESTAMP_usec                           usec
#define KERB_ENCRYPTED_KDC_REPLY_starttime                      starttime
#define KERB_CRED_INFO_starttime                                starttime
#define KERB_ENCRYPTED_KDC_REPLY_renew_until                    renew_until
#define KERB_ENCRYPTED_TICKET_authorization_data                authorization_data
#define KERB_ENCRYPTED_TICKET_starttime                         starttime
#define KERB_ENCRYPTED_PRIV_sequence_number                     sequence_number
#define KERB_KDC_REQUEST_BODY_starttime                         starttime
#define KERB_ENCRYPTED_KDC_REPLY_client_addresses               client_addresses

//
// Message types
//

#define KRB_AS_REQ      10      // Request for initial authentication
#define KRB_AS_REP      11      // Response to  KRB_AS_REQ request
#define KRB_TGS_REQ     12      // Request for authentication based on TGT
#define KRB_TGS_REP     13      // Response to KRB_TGS_REQ request
#define KRB_AP_REQ      14      // application request to server
#define KRB_AP_REP      15      // Response to KRB_AP_REQ_MUTUAL
#define KRB_TGT_REQ     16      // Request for TGT for user-to-user
#define KRB_TGT_REP     17      // Reply to TGT request
#define KRB_SAFE        20      // Safe (checksummed) application message
#define KRB_PRIV        21      // Private (encrypted) application message
#define KRB_CRED        22      // Private (encrypted) message to forward
                                // credentials
#define KRB_ERROR       30      // Error response

//
// Name types
//

#define KRB_NT_UNKNOWN   0                // Name type not known
#define KRB_NT_PRINCIPAL 1                // Just the name of the principal as in DCE, or for users
#define KRB_NT_PRINCIPAL_AND_ID -131      // Name of the principal and its SID.
#define KRB_NT_SRV_INST  2                // Service and other unique instance (krbtgt)
#define KRB_NT_SRV_INST_AND_ID -132       // SPN and SID
#define KRB_NT_SRV_HST   3                // Service with host name as instance (telnet, rcommands)
#define KRB_NT_SRV_XHST  4                // Service with host as remaining components
#define KRB_NT_UID       5                // Unique ID
#define KRB_NT_ENTERPRISE_PRINCIPAL 10    // UPN or SPN
#define KRB_NT_ENT_PRINCIPAL_AND_ID -130  // UPN and SID

//
// MS extensions, negative according to the RFC
//

#define KRB_NT_MS_PRINCIPAL         -128        // NT4 style name

#define KRB_NT_MS_PRINCIPAL_AND_ID  -129        // nt4 style name with sid

#define KERB_IS_MS_PRINCIPAL(_x_) (((_x_) <= KRB_NT_MS_PRINCIPAL) || ((_x_) >= KRB_NT_ENTERPRISE_PRINCIPAL))

//
// Pre-auth data types
//
#define KRB5_PADATA_NONE                0
#define KRB5_PADATA_AP_REQ              1
#define KRB5_PADATA_TGS_REQ             KRB5_PADATA_AP_REQ
#define KRB5_PADATA_ENC_TIMESTAMP       2
#define KRB5_PADATA_PW_SALT             3
#define KRB5_PADATA_ENC_UNIX_TIME       5  /* timestamp encrypted in key */
#define KRB5_PADATA_ENC_SANDIA_SECURID  6  /* SecurId passcode */
#define KRB5_PADATA_SESAME              7  /* Sesame project */
#define KRB5_PADATA_OSF_DCE             8  /* OSF DCE */
#define KRB5_CYBERSAFE_SECUREID         9  /* Cybersafe */
#define KRB5_PADATA_AFS3_SALT           10 /* Cygnus */
#define KRB5_PADATA_ETYPE_INFO          11 /* Etype info for preauth */
#define KRB5_PADATA_SAM_CHALLENGE       12 /* draft challenge system */
#define KRB5_PADATA_SAM_RESPONSE        13 /* draft challenge system response */
#define KRB5_PADATA_PK_AS_REQ           14 /* pkinit */
#define KRB5_PADATA_PK_AS_REP           15 /* pkinit */
#define KRB5_PADATA_PK_AS_SIGN          16 /* pkinit */
#define KRB5_PADATA_PK_KEY_REQ          17 /* pkinit */
#define KRB5_PADATA_PK_KEY_REP          18 /* pkinit */
#define KRB5_PADATA_REFERRAL_INFO       20 /* referral names for canonicalization */
#define KRB5_PADATA_PAC_REQUEST         128 /* allow client do request or ignore PAC */
#define KRB5_PADATA_S4U                 129 /* S4U */
#define KRB5_PADATA_COMPOUND_IDENTITY   130 /* authenticate multiple identities */
#define KRB5_PADATA_PAC_REQUEST_EX      131 /* allow client do request, ignore PAC or specify what sections */
#define KRB5_PADATA_CLIENT_VERSION      132 /* allow client do report version info */

//
// PAC type for PASSPORT PUID
//
#define PAC_CLIENT_IDENTITY             13
#define PAC_COMPOUND_IDENTITY           14
#define PAC_PASSPORT_PUIDS              20

//
// Authorization data types
//
#define KERB_AUTH_OSF_DCE               64
#define KERB_AUTH_SESAME                65

//
// NT authorization data type definitions
//

#define KERB_AUTH_DATA_PAC              128     // entry id for a PAC in authorization data

#define KERB_AUTH_DATA_IF_RELEVANT      1       // entry id for optional auth data
#define KERB_AUTH_DATA_KDC_ISSUED       4       // entry id for data generated & signed by KDC

//
// KDC service principal
//
#define KDC_PRINCIPAL_NAME              "krbtgt"
#define KERB_HOST_STRING                "host"
#define SECURITY_GATEWAY_STRING         "sg"

//
// SALT flags for encryption, from rfc1510 update 3des enctype
//

#define KERB_ENC_TIMESTAMP_SALT         1
#define KERB_TICKET_SALT                2
#define KERB_AS_REP_SALT                3
#define KERB_TGS_REQ_SESSKEY_SALT       4
#define KERB_TGS_REQ_SUBKEY_SALT        5
#define KERB_TGS_REQ_AP_REQ_AUTH_CKSUM_SALT     6
#define KERB_TGS_REQ_AP_REQ_AUTH_SALT   7
#define KERB_TGS_REP_SALT               8
#define KERB_TGS_REP_SUBKEY_SALT        9
#define KERB_AP_REQ_AUTH_CKSUM_SALT     10
#define KERB_AP_REQ_AUTH_SALT           11
#define KERB_AP_REP_SALT                12
#define KERB_PRIV_SALT                  13
#define KERB_CRED_SALT                  14
#define KERB_SAFE_SALT                  15
#define KERB_NON_KERB_SALT              16
#define KERB_NON_KERB_CKSUM_SALT        17
#define KERB_KERB_ERROR_SALT            18
#define KERB_KDC_ISSUED_CKSUM_SALT      19
#define KERB_MANDATORY_TKT_EXT_CKSUM_SALT       20
#define KERB_AUTH_DATA_TKT_EXT_CKSUM_SALT       21
#define KERB_PA_COMPOUND_IDENTITY_SALT  1024

//
// TODO remove KERB_PA_WITH_IDENTITY_SALT
//
#define KERB_PA_WITH_IDENTITY_SALT      1024

//
//Types for AP error data
//

#define KERB_AP_ERR_TYPE_NTSTATUS             1
#define KERB_AP_ERR_TYPE_SKEW_RECOVERY        2

// Revision of the Kerberos Protocol.  MS uses Version 5, Revision 6
#define KERBEROS_VERSION    5
#define KERBEROS_REVISION   6

// Encryption Types:
// These encryption types are supported by the default MS KERBSUPP DLL
// as crypto systems.  Values over 127 are local values, and may be changed
// without notice.

#define KERB_ETYPE_NULL             0
#define KERB_ETYPE_DES_CBC_CRC      1
#define KERB_ETYPE_DES_CBC_MD4      2
#define KERB_ETYPE_DES_CBC_MD5      3

#define KERB_ETYPE_OLD_RC4_MD4          128
#define KERB_ETYPE_OLD_RC4_PLAIN        129
#define KERB_ETYPE_OLD_RC4_LM           130
#define KERB_ETYPE_OLD_RC4_SHA          131
#define KERB_ETYPE_OLD_DES_PLAIN        132

#define KERB_ETYPE_RC4_MD4          -128
#define KERB_ETYPE_RC4_PLAIN2       -129
#define KERB_ETYPE_RC4_LM           -130
#define KERB_ETYPE_RC4_SHA          -131
#define KERB_ETYPE_DES_PLAIN        -132
#define KERB_ETYPE_RC4_HMAC_OLD     -133
#define KERB_ETYPE_RC4_PLAIN_OLD    -134
#define KERB_ETYPE_RC4_HMAC_OLD_EXP -135
#define KERB_ETYPE_RC4_PLAIN_OLD_EXP -136
#define KERB_ETYPE_RC4_PLAIN        -140
#define KERB_ETYPE_RC4_PLAIN_EXP    -141

//
// In use types
//
#define KERB_ETYPE_DES_CBC_MD5_NT                          20
#define KERB_ETYPE_RC4_HMAC_NT                             23
#define KERB_ETYPE_RC4_HMAC_NT_EXP                         24

// Checksum algorithms.
// These algorithms are keyed internally for our use.

#define KERB_CHECKSUM_NONE  0
#define KERB_CHECKSUM_CRC32         1
#define KERB_CHECKSUM_MD4           2
#define KERB_CHECKSUM_KRB_DES_MAC   4
#define KERB_CHECKSUM_MD5           7
#define KERB_CHECKSUM_MD5_DES       8


#define KERB_CHECKSUM_LM            -130
#define KERB_CHECKSUM_SHA1          -131
#define KERB_CHECKSUM_REAL_CRC32    -132
#define KERB_CHECKSUM_DES_MAC       -133
#define KERB_CHECKSUM_DES_MAC_MD5   -134
#define KERB_CHECKSUM_MD25          -135
#define KERB_CHECKSUM_RC4_MD5       -136
#define KERB_CHECKSUM_MD5_HMAC      -137                // used by netlogon
#define KERB_CHECKSUM_HMAC_MD5      -138                // used by Kerberos

#define AUTH_REQ_ALLOW_FORWARDABLE      0x00000001
#define AUTH_REQ_ALLOW_PROXIABLE        0x00000002
#define AUTH_REQ_ALLOW_POSTDATE         0x00000004
#define AUTH_REQ_ALLOW_RENEWABLE        0x00000008
#define AUTH_REQ_ALLOW_NOADDRESS        0x00000010
#define AUTH_REQ_ALLOW_ENC_TKT_IN_SKEY  0x00000020
#define AUTH_REQ_ALLOW_VALIDATE         0x00000040
#define AUTH_REQ_VALIDATE_CLIENT        0x00000080
#define AUTH_REQ_OK_AS_DELEGATE         0x00000100
#define AUTH_REQ_PREAUTH_REQUIRED       0x00000200


#define AUTH_REQ_PER_USER_FLAGS         (AUTH_REQ_ALLOW_FORWARDABLE \
                                         AUTH_REQ_ALLOW_PROXIABLE \
                                         AUTH_REQ_ALLOW_POSTDATE \
                                         AUTH_REQ_ALLOW_RENEWABLE \
                                         AUTH_REQ_ALLOW_VALIDATE )
//
// Ticket Flags:
//
#define KERB_TICKET_FLAGS_reserved          0x80000000
#define KERB_TICKET_FLAGS_forwardable       0x40000000
#define KERB_TICKET_FLAGS_forwarded         0x20000000
#define KERB_TICKET_FLAGS_proxiable         0x10000000
#define KERB_TICKET_FLAGS_proxy             0x08000000
#define KERB_TICKET_FLAGS_may_postdate      0x04000000
#define KERB_TICKET_FLAGS_postdated         0x02000000
#define KERB_TICKET_FLAGS_invalid           0x01000000
#define KERB_TICKET_FLAGS_renewable         0x00800000
#define KERB_TICKET_FLAGS_initial           0x00400000
#define KERB_TICKET_FLAGS_pre_authent       0x00200000
#define KERB_TICKET_FLAGS_hw_authent        0x00100000
#define KERB_TICKET_FLAGS_ok_as_delegate    0x00040000
#define KERB_TICKET_FLAGS_name_canonicalize 0x00010000
#define KERB_TICKET_FLAGS_reserved1         0x00000001

//
// Useful globals
//

extern TimeStamp g_KerbGlobalWillNeverTime;

//
// From kerberr.h
//

#define KERB_SUCCESS(_kerberr_) ((KERBERR)(_kerberr_) == KDC_ERR_NONE)

// These are the error codes as defined by the Kerberos V5 R5.2
// spec, section 8.3


#define KDC_ERR_NONE                  ((KERBERR) 0x0 ) // 0 No error
#define KDC_ERR_NAME_EXP              ((KERBERR) 0x1 ) // 1 Client's entry in database has expired
#define KDC_ERR_SERVICE_EXP           ((KERBERR) 0x2 ) // 2 Server's entry in database has expired
#define KDC_ERR_BAD_PVNO              ((KERBERR) 0x3 ) // 3 Requested protocol version number not supported
#define KDC_ERR_C_OLD_MAST_KVNO       ((KERBERR) 0x4 ) // 4 Client's key encrypted in old master key
#define KDC_ERR_S_OLD_MAST_KVNO       ((KERBERR) 0x5 ) // 5 Server's key encrypted in old master key
#define KDC_ERR_C_PRINCIPAL_UNKNOWN   ((KERBERR) 0x6 ) // 6 Client not found in Kerberos database
#define KDC_ERR_S_PRINCIPAL_UNKNOWN   ((KERBERR) 0x7 ) // 7 Server not found in Kerberos database
#define KDC_ERR_PRINCIPAL_NOT_UNIQUE  ((KERBERR) 0x8 ) // 8 Multiple principal entries in database
#define KDC_ERR_NULL_KEY              ((KERBERR) 0x9 ) // 9 The client or server has a null key
#define KDC_ERR_CANNOT_POSTDATE       ((KERBERR) 0xA ) // 10 Ticket not eligible for postdating
#define KDC_ERR_NEVER_VALID           ((KERBERR) 0xB ) // 11 Requested start time is later than end time
#define KDC_ERR_POLICY                ((KERBERR) 0xC ) // 12 KDC policy rejects request
#define KDC_ERR_BADOPTION             ((KERBERR) 0xD ) // 13 KDC cannot accommodate requested option
#define KDC_ERR_ETYPE_NOTSUPP         ((KERBERR) 0xE ) // 14 KDC has no support for encryption type
#define KDC_ERR_SUMTYPE_NOSUPP        ((KERBERR) 0xF ) // 15 KDC has no support for checksum type
#define KDC_ERR_PADATA_TYPE_NOSUPP    ((KERBERR) 0x10 ) // 16 KDC has no support for padata type
#define KDC_ERR_TRTYPE_NO_SUPP        ((KERBERR) 0x11 ) // 17 KDC has no support for transited type
#define KDC_ERR_CLIENT_REVOKED        ((KERBERR) 0x12 ) // 18 Clients credentials have been revoked
#define KDC_ERR_SERVICE_REVOKED       ((KERBERR) 0x13 ) // 19 Credentials for server have been revoked
#define KDC_ERR_TGT_REVOKED           ((KERBERR) 0x14 ) // 20 TGT has been revoked
#define KDC_ERR_CLIENT_NOTYET         ((KERBERR) 0x15 ) // 21 Client not yet valid - try again later
#define KDC_ERR_SERVICE_NOTYET        ((KERBERR) 0x16 ) // 22 Server not yet valid - try again later
#define KDC_ERR_KEY_EXPIRED           ((KERBERR) 0x17 ) // 23 Password has expired - change password to reset
#define KDC_ERR_PREAUTH_FAILED        ((KERBERR) 0x18 ) // 24 Pre-authentication information was invalid
#define KDC_ERR_PREAUTH_REQUIRED      ((KERBERR) 0x19 ) // 25 Additional pre-authenticationrequired [40]
#define KDC_ERR_SERVER_NOMATCH        ((KERBERR) 0x1A ) // 26 Requested server and ticket don't match
#define KDC_ERR_MUST_USE_USER2USER    ((KERBERR) 0x1B ) // 27 Server principal valid for user2user only
#define KDC_ERR_PATH_NOT_ACCPETED     ((KERBERR) 0x1C ) // 28 KDC Policy rejects transited path
#define KDC_ERR_SVC_UNAVAILABLE       ((KERBERR) 0x1D ) // 29 A service is not available
#define KRB_AP_ERR_BAD_INTEGRITY      ((KERBERR) 0x1F ) // 31 Integrity check on decrypted field failed
#define KRB_AP_ERR_TKT_EXPIRED        ((KERBERR) 0x20 ) // 32 Ticket expired
#define KRB_AP_ERR_TKT_NYV            ((KERBERR) 0x21 ) // 33 Ticket not yet valid
#define KRB_AP_ERR_REPEAT             ((KERBERR) 0x22 ) // 34 Request is a replay
#define KRB_AP_ERR_NOT_US             ((KERBERR) 0x23 ) // 35 The ticket isn't for us
#define KRB_AP_ERR_BADMATCH           ((KERBERR) 0x24 ) // 36 Ticket and authenticator don't match
#define KRB_AP_ERR_SKEW               ((KERBERR) 0x25 ) // 37 Clock skew too great
#define KRB_AP_ERR_BADADDR            ((KERBERR) 0x26 ) // 38 Incorrect net address
#define KRB_AP_ERR_BADVERSION         ((KERBERR) 0x27 ) // 39 Protocol version mismatch
#define KRB_AP_ERR_MSG_TYPE           ((KERBERR) 0x28 ) // 40 Invalid msg type
#define KRB_AP_ERR_MODIFIED           ((KERBERR) 0x29 ) // 41 Message stream modified
#define KRB_AP_ERR_BADORDER           ((KERBERR) 0x2A ) // 42 Message out of order
#define KRB_AP_ERR_ILL_CR_TKT         ((KREBERR) 0x2B ) // 43 Illegal cross realm ticket
#define KRB_AP_ERR_BADKEYVER          ((KERBERR) 0x2C ) // 44 Specified version of key is not available
#define KRB_AP_ERR_NOKEY              ((KERBERR) 0x2D ) // 45 Service key not available
#define KRB_AP_ERR_MUT_FAIL           ((KERBERR) 0x2E ) // 46 Mutual authentication failed
#define KRB_AP_ERR_BADDIRECTION       ((KERBERR) 0x2F ) // 47 Incorrect message direction
#define KRB_AP_ERR_METHOD             ((KERBERR) 0x30 ) // 48 Alternative authentication method required
#define KRB_AP_ERR_BADSEQ             ((KERBERR) 0x31 ) // 49 Incorrect sequence number in message
#define KRB_AP_ERR_INAPP_CKSUM        ((KERBERR) 0x32 ) // 50 Inappropriate type of checksum in message
#define KRB_AP_PATH_NOT_ACCEPTED      ((KERBERR) 0x33 ) // 51 Policy rejects transited path
#define KRB_ERR_RESPONSE_TOO_BIG      ((KERBERR) 0x34 ) // 52 Response too big for UDP, retry with TCP
#define KRB_ERR_GENERIC               ((KERBERR) 0x3C ) // 60 Generic error (description in e-text)
#define KRB_ERR_FIELD_TOOLONG         ((KERBERR) 0x3D ) // 61 Field is too long for this implementation
#define KDC_ERR_CLIENT_NOT_TRUSTED    ((KERBERR) 0x3E ) // 62 (pkinit)
#define KDC_ERR_KDC_NOT_TRUSTED       ((KERBERR) 0x3F ) // 63 (pkinit)
#define KDC_ERR_INVALID_SIG           ((KERBERR) 0x40 ) // 64 (pkinit)
#define KDC_ERR_KEY_TOO_WEAK          ((KERBERR) 0x41 ) // 65 (pkinit)
#define KDC_ERR_CERTIFICATE_MISMATCH  ((KERBERR) 0x42 ) // 66 (pkinit)
#define KRB_AP_ERR_NO_TGT             ((KERBERR) 0x43 ) // 67 (user-to-user)
#define KDC_ERR_WRONG_REALM           ((KERBERR) 0x44 ) // 68 (user-to-user)
#define KRB_AP_ERR_USER_TO_USER_REQUIRED ((KERBERR) 0x45 ) // 69 (user-to-user)
#define KDC_ERR_CANT_VERIFY_CERTIFICATE ((KERBERR) 0x46 ) // 70 (pkinit)
#define KDC_ERR_INVALID_CERTIFICATE     ((KERBERR) 0x47 ) // 71 (pkinit)
#define KDC_ERR_REVOKED_CERTIFICATE     ((KERBERR) 0x48 ) // 72 (pkinit)
#define KDC_ERR_REVOCATION_STATUS_UNKNOWN ((KERBERR) 0x49 ) // 73 (pkinit)
#define KDC_ERR_REVOCATION_STATUS_UNAVAILABLE ((KERBERR) 0x4a ) // 74 (pkinit)
#define KDC_ERR_CLIENT_NAME_MISMATCH    ((KERBERR) 0x4b ) // 75 (pkinit)
#define KDC_ERR_KDC_NAME_MISMATCH       ((KERBERR) 0x4c ) // 76 (pkinit)

#define KDC_ERR_MORE_DATA             ((KERBERR) 0x80000001 )

//
// Default flags for use in ticket requests
//
#define KERB_DEFAULT_TICKET_FLAGS ( KERB_KDC_OPTIONS_name_canonicalize )

//
//  Following macro is used to initialize UNICODE strings
//
#define CONSTANT_UNICODE_STRING(s)   { sizeof( s ) - sizeof( WCHAR ), sizeof( s ), s }
#define NULL_UNICODE_STRING {0 , 0, NULL }
#define EMPTY_UNICODE_STRING(s) { (s)->Buffer = NULL; (s)->Length = 0; (s)->MaximumLength = 0; }

#define KerbSetTime(_d_, _s_) (_d_)->QuadPart = (_s_)

#define KerbAddOffsetToTime(_d_, _offset_) (_d_)->QuadPart += (_offset_)

#define KerbGetOffsetBetweenTime(_d_, _s_) ((_s_)->QuadPart - (_d_)->QuadPart)

#define KerbGetTime(_x_) ((_x_).QuadPart)

#define KerbSetTimeInMinutes(_x_, _m_) (_x_)->QuadPart = (LONGLONG) 10000000 * 60 * (_m_)

#define KerbMapKerbNtStatusToNtStatus(x) (x)


//////////////////////////////////////////////////////////////////////////
//
// Structures
//
//////////////////////////////////////////////////////////////////////////

typedef struct _KERB_PREAUTH_DATA {
    ULONG Flags;
} KERB_PREAUTH_DATA, *PKERB_PREAUTH_DATA;

//
// KDC-Kerberos interaction
//

#define KERB_DEFAULT_AP_REQ_CSUM        KERB_CHECKSUM_MD5

#define KerbFreeData(a,b)

VOID
KerbCreateKeyFromBuffer(
    OUT PKERB_ENCRYPTION_KEY NewKey,
    IN PUCHAR Buffer,
    IN ULONG BufferSize,
    IN ULONG EncryptionType
    )
{
    NewKey->keytype = EncryptionType;
    NewKey->keyvalue.length = BufferSize;
    NewKey->keyvalue.value = (PUCHAR) Buffer;
}


KERBERR NTAPI
KerbCreateAuthenticator(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_ENCRYPTION_KEY pkKey,
    IN ULONG EncryptionType,
    IN ULONG SequenceNumber,
    IN LPSTR ClientName,
    IN LPSTR ClientRealm,
    IN PTimeStamp ptsTime,
    IN OPTIONAL PKERB_CHECKSUM Checksum,
    IN BOOLEAN KdcRequest,
    OUT PKERB_ENCRYPTED_DATA Authenticator
    );

KERBERR NTAPI
KerbUnpackKdcReplyBody(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_ENCRYPTED_DATA EncryptedReplyBody,
    IN PKERB_ENCRYPTION_KEY Key,
    IN ULONG Pdu,
    OUT PKERB_ENCRYPTED_KDC_REPLY * ReplyBody
    );

KERBERR NTAPI
KerbPackData(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PVOID Data,
    IN ULONG PduValue,
    OUT PULONG DataSize,
    OUT PUCHAR * MarshalledData
    );

KERBERR NTAPI
KerbUnpackData(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PUCHAR Data,
    IN ULONG DataSize,
    IN ULONG PduValue,
    OUT PVOID * DecodedData
    );

#define KerbUnpackAsReply( pScratchMemoryAllocator, ReplyMessage, ReplySize, Reply ) \
    KerbUnpackData( \
        (pScratchMemoryAllocator), \
        (ReplyMessage), \
        (ReplySize), \
        KERB_AS_REPLY_PDU, \
        (PVOID *) (Reply) \
        )

#define KerbFreeAsReply( Request)

#define KerbUnpackTgsReply( pScratchMemoryAllocator, ReplyMessage, ReplySize, Reply ) \
    KerbUnpackData( \
        (pScratchMemoryAllocator), \
        (ReplyMessage), \
        (ReplySize), \
        KERB_TGS_REPLY_PDU, \
        (PVOID *) (Reply) \
        )

#define KerbFreeTgsReply( Request) 

#define KerbFreeKdcReplyBody( Request)

#define KerbPackAsRequest( pScratchMemoryAllocator, RequestMessage, RequestSize, MarshalledRequest )\
    KerbPackData( \
        (pScratchMemoryAllocator), \
        (PVOID) (RequestMessage), \
        KERB_AS_REQUEST_PDU, \
        (RequestSize), \
        (MarshalledRequest) \
        )

#define KerbFreeAsRequest( Request)

#define KerbPackTgsRequest( pScratchMemoryAllocator, RequestMessage, RequestSize, MarshalledRequest )\
    KerbPackData( \
        (pScratchMemoryAllocator), \
        (PVOID) (RequestMessage), \
        KERB_TGS_REQUEST_PDU, \
        (RequestSize), \
        (MarshalledRequest) \
        )

#define KerbFreeTgsRequest( Request)

#define KerbPackEncryptedData( pScratchMemoryAllocator, EncryptedData, DataSize, MarshalledData ) \
    KerbPackData( \
        (pScratchMemoryAllocator), \
        (PVOID) (EncryptedData), \
        KERB_ENCRYPTED_DATA_PDU, \
        (DataSize), \
        (PUCHAR *) (MarshalledData) \
        )

#define KerbUnpackEncryptedData( pScratchMemoryAllocator, EncryptedData,DataSize,Data ) \
    KerbUnpackData( \
        (pScratchMemoryAllocator), \
        (EncryptedData), \
        (DataSize), \
        KERB_ENCRYPTED_DATA_PDU, \
        (PVOID *) (Data) \
        )

#define KerbFreeEncryptedData( EncryptedData)

#define KerbPackApRequest( pScratchMemoryAllocator, ApRequestMessage, ApRequestSize, MarshalledApRequest ) \
    KerbPackData( \
        (pScratchMemoryAllocator), \
        (PVOID) (ApRequestMessage), \
        KERB_AP_REQUEST_PDU, \
        (ApRequestSize), \
        (MarshalledApRequest) \
        )

#define KerbUnpackApReply( pScratchMemoryAllocator, ApReplyMessage,ApReplySize, ApReply) \
    KerbUnpackData( \
        (pScratchMemoryAllocator), \
        (ApReplyMessage), \
        (ApReplySize), \
        KERB_AP_REPLY_PDU, \
        (PVOID *) (ApReply) \
        )

#define KerbFreeApReply( Reply)

#define KerbUnpackApReplyBody( pScratchMemoryAllocator, ApReplyBodyMessage,ApReplyBodySize, ApReplyBody) \
    KerbUnpackData( \
        (pScratchMemoryAllocator), \
        (ApReplyBodyMessage), \
        (ApReplyBodySize), \
        KERB_ENCRYPTED_AP_REPLY_PDU, \
        (PVOID *) (ApReplyBody) \
        )

#define KerbFreeApReplyBody( ReplyBody)

#define KerbPackEncryptedCred( pScratchMemoryAllocator, EncryptedCred, CredSize, MarshalledCred ) \
    KerbPackData( \
        (pScratchMemoryAllocator), \
        (PVOID) (EncryptedCred), \
        KERB_ENCRYPTED_CRED_PDU, \
        (CredSize), \
        (MarshalledCred) \
        )

#define KerbUnpackEncryptedCred( pScratchMemoryAllocator, EncryptedCred,CredSize,Cred ) \
    KerbUnpackData( \
        (pScratchMemoryAllocator), \
        (EncryptedCred), \
        (CredSize), \
        KERB_ENCRYPTED_CRED_PDU, \
        (PVOID *) (Cred) \
        )

#define KerbFreeEncryptedCred( EncryptedCred)

#define KerbPackKerbCred( pScratchMemoryAllocator, KerbCred, KerbCredSize, MarshalledKerbCred ) \
    KerbPackData( \
        (pScratchMemoryAllocator), \
        (PVOID) (KerbCred), \
        KERB_CRED_PDU, \
        (KerbCredSize), \
        (MarshalledKerbCred) \
        )

#define KerbUnpackKerbCred( pScratchMemoryAllocator, MarshalledKerbCred,KerbCredSize,KerbCred ) \
    KerbUnpackData( \
        (pScratchMemoryAllocator), \
        (MarshalledKerbCred), \
        (KerbCredSize), \
        KERB_CRED_PDU, \
        (PVOID *) (KerbCred) \
        )

#define KerbFreeKerbCred( KerbCred) \
    KerbFreeData( \
        KERB_CRED_PDU, \
        (PVOID) (KerbCred) \
        )

#define KerbPackKerbError( pScratchMemoryAllocator, ErrorMessage, ErrorSize, MarshalledError ) \
    KerbPackData( \
        (pScratchMemoryAllocator), \
        (PVOID) (ErrorMessage), \
        KERB_ERROR_PDU, \
        (ErrorSize), \
        (MarshalledError) \
        )

#define KerbUnpackKerbError( pScratchMemoryAllocator, ErrorMessage, ErrorSize, Error ) \
    KerbUnpackData( \
        (pScratchMemoryAllocator), \
        (ErrorMessage), \
        (ErrorSize), \
        KERB_ERROR_PDU, \
        (PVOID *) (Error) \
        )

#define KerbFreeKerbError( Error )

#define KerbPackEncryptedTime( pScratchMemoryAllocator, EncryptedTimeMessage, EncryptedTimeSize, MarshalledEncryptedTime ) \
    KerbPackData( \
        (pScratchMemoryAllocator), \
        (PVOID) (EncryptedTimeMessage), \
        KERB_ENCRYPTED_TIMESTAMP_PDU, \
        (EncryptedTimeSize), \
        (MarshalledEncryptedTime) \
        )

#define KerbUnpackEncryptedTime( pScratchMemoryAllocator, EncryptedTimeMessage, EncryptedTimeSize, EncryptedTime ) \
    KerbUnpackData( \
        (pScratchMemoryAllocator), \
        (EncryptedTimeMessage), \
        (EncryptedTimeSize), \
        KERB_ENCRYPTED_TIMESTAMP_PDU, \
        (PVOID *) (EncryptedTime) \
        )

#define KerbFreeEncryptedTime( EncryptedTime )

KERBERR NTAPI
KerbEncryptDataEx(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    OUT PKERB_ENCRYPTED_DATA EncryptedData,
    IN ULONG DataSize,
    IN PUCHAR Data,
    IN ULONG Algorithm,
    IN ULONG UsageFlags,
    IN PKERB_ENCRYPTION_KEY Key
    );

KERBERR NTAPI
KerbDecryptDataEx(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_ENCRYPTED_DATA EncryptedData,
    IN PKERB_ENCRYPTION_KEY pkKey,
    IN ULONG UsageFlags,
    OUT PULONG DataSize,
    OUT PUCHAR Data
    );

INLINE ULONG
KerbGetEncryptionOverhead()
{
    return sizeof(RC4_MDx_HEADER);
}

KERBERR
KerbConstructPrincipalName(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    OUT PKERB_PRINCIPAL_NAME PrincipalName,
    IN ULONG NameType,
    IN LPSTR pComp1,
    IN LPSTR pComp2 = NULL
    );

KERBERR
KerbMultiStringToPrincipalName(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN LPSTR pMultiString,
    OUT PKERB_PRINCIPAL_NAME PrincipalName
    );

KERBERR
KerbPrincipalNameToMultiString(
    IN PKERB_PRINCIPAL_NAME PrincipalName,
    IN DWORD dwMultiStringMaxLength,
    OUT LPSTR pMultiString
    );

VOID
KerbConvertLargeIntToGeneralizedTime(
    OUT PKERB_TIME ClientTime,
    OUT OPTIONAL LONG* ClientUsec,
    IN PTimeStamp TimeStamp
    );

VOID
KerbConvertGeneralizedTimeToLargeInt(
    OUT PTimeStamp TimeStamp,
    IN PKERB_TIME ClientTime,
    IN int ClientUsec
    );

PKERB_PA_DATA
KerbFindPreAuthDataEntry(
    IN ULONG EntryId,
    IN PKERB_PA_DATA_LIST AuthData
    );

KERBERR
KerbCreateApRequest(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN LPSTR ClientName,
    IN LPSTR ClientRealm,
    IN PKERB_ENCRYPTION_KEY SessionKey,
    IN ULONG Nonce,
    IN PTimeStamp pAuthenticatorTime,
    IN PKERB_TICKET ServiceTicket,
    IN ULONG ApOptions,
    IN OPTIONAL PKERB_CHECKSUM Checksum,
    IN BOOLEAN KdcRequest,
    OUT PULONG RequestSize,
    OUT PUCHAR * Request
    );

NTSTATUS
KerbMapKerbError(
    IN KERBERR KerbError
    );

ULONG
KerbConvertUlongToFlagUlong(
    IN ULONG Flag
    );

VOID
KerbGetCurrentTime(
    OUT PTimeStamp pCurrentTime
    );
    
VOID
KerbUpdateCurrentTime(
    IN TimeStamp NewCurrentTime
    );

BOOLEAN
md5Hmac(
    IN PBYTE pbKeyMaterial,
    IN ULONG cbKeyMaterial,
    IN PBYTE pbData,
    IN ULONG cbData,
    IN PBYTE pbData2,
    IN ULONG cbData2,
    OUT PBYTE HmacData
    );

INLINE KERBERR
KerbAllocateEncryptionBuffer(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN ULONG EncryptionType,
    IN ULONG BufferSize,
    OUT PULONG EncryptionBufferSize,
    OUT PBYTE * EncryptionBuffer
    )
{
    KERBERR KerbErr = KDC_ERR_NONE;

    *EncryptionBufferSize = sizeof(RC4_MDx_HEADER) + BufferSize;

    *EncryptionBuffer = (PBYTE) pScratchMemoryAllocator->Alloc(*EncryptionBufferSize);
    if (*EncryptionBuffer == NULL)
    {
        KerbErr = KRB_ERR_GENERIC;
    }
    return(KerbErr);

}


//+-------------------------------------------------------------------------
// Start CPP code
//+-------------------------------------------------------------------------

KERB_CRYPT_LIST g_RC4HMAC_CryptList = { NULL , KERB_ETYPE_RC4_HMAC_NT };

NTSTATUS
KerbComputeRequestBodyChecksum(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_KDC_REQUEST_BODY RequestBody,
    OUT PKERB_CHECKSUM Checksum
    )
{
    KERB_MESSAGE_BUFFER MarshalledRequestBody = {0, NULL};
    NTSTATUS Status = STATUS_SUCCESS;
    MD5_CTX MD5Context;

    RtlZeroMemory(
        Checksum,
        sizeof(KERB_CHECKSUM)
        );

    //
    // Allocate enough space for the checksum
    //
    Checksum->checksum.value = (PUCHAR) pScratchMemoryAllocator->Alloc(MD5_LEN);
    if (Checksum->checksum.value == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }
    Checksum->checksum.length = MD5_LEN;
    Checksum->checksum_type = KERB_CHECKSUM_MD5;

    {
        CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( pScratchMemoryAllocator );
    
        if (!KERB_SUCCESS(KerbPackData(
                            pScratchMemoryAllocator,
                            RequestBody,
                            KERB_MARSHALLED_REQUEST_BODY_PDU,
                            &MarshalledRequestBody.BufferSize,
                            &MarshalledRequestBody.Buffer
                            )))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        //
        // Now checksum the buffer
        //

        MD5Init(&MD5Context);
        MD5Update(&MD5Context, MarshalledRequestBody.Buffer, MarshalledRequestBody.BufferSize);
        MD5Final(&MD5Context);
        RtlCopyMemory(Checksum->checksum.value, MD5Context.digest, MD5_LEN);
    }
    
Cleanup:
    if (Status != STATUS_SUCCESS)
    {
        TraceSz1( KerbWarn, "KerbComputeRequestBodyChecksum failure status 0x%X", Status );
    }
    return(Status);
}

//+-------------------------------------------------------------------------

NTSTATUS
KerbAddTimestampPreAuth(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_ENCRYPTION_KEY UserKey,
    IN PTimeStamp pAuthTime,
    OUT PKERB_PA_DATA_LIST * PreAuthData
    )
{
    KERBERR KerbErr = KDC_ERR_NONE;
    NTSTATUS Status = STATUS_SUCCESS;
    PKERB_PA_DATA_LIST ListElement = NULL;
    KERB_ENCRYPTED_TIMESTAMP Timestamp = {0};
    PBYTE EncryptedTime = NULL;
    ULONG EncryptedTimeSize = 0;
    KERB_ENCRYPTED_DATA EncryptedData;
    void* pTempBuffer;

    RtlZeroMemory( &EncryptedData, sizeof(KERB_ENCRYPTED_DATA) );

    //
    // Build the output element
    //

    ListElement = (PKERB_PA_DATA_LIST) pScratchMemoryAllocator->Alloc(sizeof(KERB_PA_DATA_LIST));
    if (ListElement == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    {
        CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( pScratchMemoryAllocator );    

        //
        // Now build the encrypted timestamp
        //
        KerbConvertLargeIntToGeneralizedTime(
            &Timestamp.timestamp,
            &Timestamp.KERB_ENCRYPTED_TIMESTAMP_usec,
            pAuthTime
            );

        Timestamp.bit_mask = KERB_ENCRYPTED_TIMESTAMP_usec_present;

        KerbErr = KerbPackEncryptedTime(
                    pScratchMemoryAllocator,
                    &Timestamp,
                    &EncryptedTimeSize,
                    &EncryptedTime
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        //
        // Now encrypt the time
        //

        KerbErr = KerbAllocateEncryptionBuffer(
                    pScratchMemoryAllocator,
                    UserKey->keytype,
                    EncryptedTimeSize,
                    &EncryptedData.cipher_text.length,
                    &EncryptedData.cipher_text.value
                    );

        if (!KERB_SUCCESS(KerbErr))
        {
            Status = KerbMapKerbError(KerbErr);
            goto Cleanup;
        }


        KerbErr = KerbEncryptDataEx(
                    pScratchMemoryAllocator,
                    &EncryptedData,
                    EncryptedTimeSize,
                    EncryptedTime,
                    UserKey->keytype,
                    KERB_ENC_TIMESTAMP_SALT,
                    UserKey
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        //
        // Now pack the encrypted data
        //

        KerbErr = KerbPackEncryptedData(
                    pScratchMemoryAllocator,
                    &EncryptedData,
                    (PULONG) &ListElement->value.preauth_data.length,
                    (PUCHAR *) &ListElement->value.preauth_data.value
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        //
        // Preserve the only piece of memory we care about
        //
        pTempBuffer = PreserveThisMemory( &onlyPreserveThisMemory, &ListElement->value.preauth_data.value, ListElement->value.preauth_data.length );
        if (pTempBuffer == NULL)
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }
    }
    
    ListElement->value.preauth_data_type = KRB5_PADATA_ENC_TIMESTAMP;
    ListElement->next = *PreAuthData;
    *PreAuthData = ListElement;

Cleanup:
    if (Status != STATUS_SUCCESS)
    {
        TraceSz1( KerbWarn, "KerbAddTimestampPreAuth failure status 0x%X", Status );
    }
    return(Status);
}

//+-------------------------------------------------------------------------

PKERB_PA_DATA
KerbFindPreAuthDataEntry(
    IN ULONG EntryId,
    IN PKERB_PA_DATA_LIST AuthData
    )
{
    PKERB_PA_DATA_LIST TempData = AuthData;

    while (TempData != NULL)
    {
        if (TempData->value.preauth_data_type == (int) EntryId)
        {
            break;
        }
        TempData = TempData->next;
    }
    return(TempData  != NULL ? &TempData->value : NULL);
}


//--------------------------------------------------------------------------

void KerbComputeKdcRequestNonceHmacKey(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_ENCRYPTION_KEY pClientKey,
    IN DWORD nonce,
    OUT PBYTE pNonceHmacKey
    )
{
    MD5_CTX Md5Context;
    UCHAR Key[MD5_LEN];
    ULONG ulSalt = KERB_KDC_REQ_NONCE_CKSUM_SALT;
    
    Assert(MD5_LEN == XONLINE_KEY_LENGTH);
    Assert(MD5_LEN == pClientKey->keyvalue.length);

    md5Hmac(
        pClientKey->keyvalue.value,
        pClientKey->keyvalue.length,
        (PBYTE)"signaturekey",
        sizeof("signaturekey"),
        NULL,
        0,
        Key
        );
        
    MD5Init(&Md5Context);
    
    MD5Update(&Md5Context, (BYTE*)&ulSalt, sizeof(ulSalt));

    MD5Update(&Md5Context, (BYTE*)&nonce, sizeof(nonce));
    
    MD5Final(&Md5Context);

    md5Hmac(
        Key,
        MD5_LEN,
        Md5Context.digest,
        MD5_LEN,
        NULL,
        0,
        pNonceHmacKey
        );
}

//+-------------------------------------------------------------------------

KERBERR
KerbVerifyAccountCreationPreAuth(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_PA_DATA PreAuthData,
    IN PKERB_ENCRYPTION_KEY ClientKey,
    IN OUT PXKERB_TGT_CONTEXT pTGTContext
    )
{
    KERBERR KerbErr;
    BYTE abNonceHmacKey[XONLINE_KEY_LENGTH];
    KERB_ENCRYPTION_KEY NonceHmacKey;
    PKERB_ENCRYPTED_DATA EncryptedData = NULL;
    DWORD dwDecryptedDataLen;
    DWORD i;
    
    CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( pScratchMemoryAllocator );

    //
    // Unpack the pre-auth data into an encrypted data first.
    //
    KerbErr = KerbUnpackEncryptedData(
                pScratchMemoryAllocator,
                PreAuthData->preauth_data.value,
                PreAuthData->preauth_data.length,
                &EncryptedData
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        goto Cleanup;
    }

    if (EncryptedData->cipher_text.length != KerbGetEncryptionOverhead() + sizeof(XKERB_PA_XBOX_ACCOUNT_CREATION))
    {
        AssertSz2( FALSE, "KerbVerifyAccountCreationPreAuth: Preauth size is %d, should be %d",
            EncryptedData->cipher_text.length,
            KerbGetEncryptionOverhead() + sizeof(XKERB_PA_XBOX_ACCOUNT_CREATION)
            );
        KerbErr = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    //
    // Try all nonces
    //
    for (i=0; i<pTGTContext->ctNumNonces; ++i)
    {
        KerbComputeKdcRequestNonceHmacKey(
            pScratchMemoryAllocator,
            ClientKey,
            pTGTContext->Nonce[i],
            abNonceHmacKey
            );

        KerbCreateKeyFromBuffer(
            &NonceHmacKey,
            abNonceHmacKey,
            XONLINE_KEY_LENGTH,
            KERB_ETYPE_RC4_HMAC_NT
            );

        //
        // Now decrypt the encrypted data (in place)
        //
        KerbErr = KerbDecryptDataEx(
                    pScratchMemoryAllocator,
                    EncryptedData,
                    &NonceHmacKey,
                    KERB_PA_XBOX_ACCOUNT_CREATION_SALT,
                    (PULONG) &dwDecryptedDataLen,
                    (BYTE*)pTGTContext->pAccountCreationPreAuth
                    );
        if (KERB_SUCCESS(KerbErr))
        {
            break;
        }
    }
    
    if (i == pTGTContext->ctNumNonces)
    {
        KerbErr = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    Assert( dwDecryptedDataLen == sizeof(XKERB_PA_XBOX_ACCOUNT_CREATION) );
    
    KerbErr = KDC_ERR_NONE;

Cleanup:
    if (!KERB_SUCCESS(KerbErr))
    {
        TraceSz1( KerbWarn, "KerbVerifyAccountCreationPreAuth failure KerbErr 0x%X", KerbErr );
    }
    return(KerbErr);
}

//+-------------------------------------------------------------------------


KERBERR
KerbVerifyServiceAddressPreAuth(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_PA_DATA PreAuthData,
    IN PKERB_ENCRYPTION_KEY TGTSessionKey,
    IN PXKERB_SERVICE_CONTEXT pXKerbServiceContext,
    OUT PXKERB_PA_XBOX_SERVICE_ADDRESS pPAServiceAddress
    )
{
    KERBERR KerbErr;
    BYTE abNonceHmacKey[XONLINE_KEY_LENGTH];
    KERB_ENCRYPTION_KEY NonceHmacKey;
    PKERB_ENCRYPTED_DATA EncryptedData = NULL;
    DWORD dwDecryptedDataLen;
    DWORD i;
    
    CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( pScratchMemoryAllocator );
    
    //
    // Unpack the pre-auth data into an encrypted data first.
    //
    KerbErr = KerbUnpackEncryptedData(
                pScratchMemoryAllocator,
                PreAuthData->preauth_data.value,
                PreAuthData->preauth_data.length,
                &EncryptedData
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        goto Cleanup;
    }

    if (EncryptedData->cipher_text.length != KerbGetEncryptionOverhead() + sizeof(XKERB_PA_XBOX_SERVICE_ADDRESS))
    {
        AssertSz2( FALSE, "KerbVerifyServiceAddressPreAuth: Preauth size is %d, should be %d",
            EncryptedData->cipher_text.length,
            KerbGetEncryptionOverhead() + sizeof(XKERB_PA_XBOX_SERVICE_ADDRESS)
            );
        KerbErr = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    //
    // Try all nonces
    //
    for (i=0; i<pXKerbServiceContext->ctNumNonces; ++i)
    {
        KerbComputeKdcRequestNonceHmacKey(
            pScratchMemoryAllocator,
            TGTSessionKey,
            pXKerbServiceContext->Nonce[i],
            abNonceHmacKey
            );

        KerbCreateKeyFromBuffer(
            &NonceHmacKey,
            abNonceHmacKey,
            XONLINE_KEY_LENGTH,
            KERB_ETYPE_RC4_HMAC_NT
            );

        //
        // Now decrypt the encrypted data (in place)
        //
        KerbErr = KerbDecryptDataEx(
                    pScratchMemoryAllocator,
                    EncryptedData,
                    &NonceHmacKey,
                    KERB_PA_XBOX_SERVICE_ADDRESS_SALT,
                    (PULONG) &dwDecryptedDataLen,
                    (BYTE*)pPAServiceAddress
                    );
        if (KERB_SUCCESS(KerbErr))
        {
            break;
        }
    }
    
    if (i == pXKerbServiceContext->ctNumNonces)
    {
        KerbErr = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }
    
    Assert( dwDecryptedDataLen == sizeof(XKERB_PA_XBOX_SERVICE_ADDRESS) );
    
#ifdef XNET_FEATURE_TRACE
    {
        char buffer1[256],buf[32];
        DWORD k;
        buffer1[0] = '\0';
        for (k=0; k<pPAServiceAddress->dwNumServices; ++k)
        {
            sprintf(buf," %d(0x%X):%d", pPAServiceAddress->serviceResult[k].dwServiceID, pPAServiceAddress->serviceResult[k].hr, pPAServiceAddress->serviceResult[k].wServicePort);
            strcat(buffer1, buf);
        }
        TraceSz14( AuthVerbose, "KerbVerifyServiceAddressPreAuth HR:0x%X IP:%d.%d.%d.%d User HR:FLGS: 0x%X:%X 0x%X:%X 0x%X:%X 0x%X:%X Services:%s",
            pPAServiceAddress->hr,
            pPAServiceAddress->siteIPAddress.S_un.S_un_b.s_b1,
            pPAServiceAddress->siteIPAddress.S_un.S_un_b.s_b2,
            pPAServiceAddress->siteIPAddress.S_un.S_un_b.s_b3,
            pPAServiceAddress->siteIPAddress.S_un.S_un_b.s_b4,
            pPAServiceAddress->hrUser[0],
            pPAServiceAddress->dwUserFlags[0],
            pPAServiceAddress->hrUser[1],
            pPAServiceAddress->dwUserFlags[1],
            pPAServiceAddress->hrUser[2],
            pPAServiceAddress->dwUserFlags[2],
            pPAServiceAddress->hrUser[3],
            pPAServiceAddress->dwUserFlags[3],
            buffer1 );
    }
#endif

    KerbErr = KDC_ERR_NONE;

Cleanup:
    if (!KERB_SUCCESS(KerbErr))
    {
        TraceSz1( KerbWarn, "KerbVerifyServiceAddressPreAuth failure KerbErr 0x%X", KerbErr );
    }

    return(KerbErr);
}

//+-------------------------------------------------------------------------

NTSTATUS
KerbAddPacRequestPreAuth(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN BOOL fIncludePAC,
    IN OUT PKERB_PA_DATA_LIST * PreAuthData
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    PKERB_PA_DATA_LIST ListElement = NULL;
    PKERB_PA_DATA_LIST LastElement = NULL;
    KERB_PA_PAC_REQUEST_EX PacRequestEx = {0};
    KERB_PA_PAC_REQUEST_EX_pac_sections_Element PacSections[2];
    
    ListElement = (PKERB_PA_DATA_LIST) pScratchMemoryAllocator->Alloc(sizeof(KERB_PA_DATA_LIST));
    if (ListElement == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    RtlZeroMemory( &PacRequestEx, sizeof(KERB_PA_PAC_REQUEST_EX) );

    if (fIncludePAC)
    {
        PacRequestEx.include_pac = TRUE;
        PacRequestEx.bit_mask = pac_sections_present;
        PacRequestEx.pac_sections = &PacSections[0];

        PacSections[0].next = &PacSections[1];
        PacSections[0].value = PAC_CLIENT_IDENTITY;
        PacSections[1].next = NULL;
        PacSections[1].value = PAC_COMPOUND_IDENTITY;
    }
    else
    {
        PacRequestEx.include_pac = FALSE;
    }
    
    //
    // Marshall the type into the list element.
    //
    if (!KERB_SUCCESS(KerbPackData(
                        pScratchMemoryAllocator,
                        (PKERB_PA_PAC_REQUEST)(&PacRequestEx),
                        KERB_PA_PAC_REQUEST_EX_PDU,
                        (PULONG) &ListElement->value.preauth_data.length,
                        (PUCHAR *) &ListElement->value.preauth_data.value
                        )))
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }
    
    ListElement->value.preauth_data_type = KRB5_PADATA_PAC_REQUEST_EX;

    //
    // We want this to go at the end, so that it will override any other
    // pa-data that may enable a PAC.
    //

    LastElement = *PreAuthData;
    if (LastElement != NULL)
    {
        while (LastElement->next != NULL)
        {
            LastElement = LastElement->next;
        }
        LastElement->next = ListElement;
    }
    else
    {
        *PreAuthData = ListElement;
    }

    ListElement->next = NULL;
    ListElement = NULL;

Cleanup:
    if (Status != STATUS_SUCCESS)
    {
        TraceSz1( KerbWarn, "KerbAddPacRequestPreAuth failure status 0x%X", Status );
    }

    return(Status);
}


//+-------------------------------------------------------------------------
// Add the preauth to tell the server this is an Xbox
//+-------------------------------------------------------------------------

NTSTATUS
KerbAddClientVersionPreAuth(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_ENCRYPTION_KEY ClientKey,
    IN DWORD nonce,
    IN OUT PKERB_PA_DATA_LIST * PreAuthData
    )
{
    KERBERR KerbErr = KDC_ERR_NONE;
    NTSTATUS Status = STATUS_SUCCESS;
    PKERB_PA_DATA_LIST ListElement;
    char acVersionString[256];
    DWORD dwVersionStringLen;
    BYTE abNonceHmacKey[XONLINE_KEY_LENGTH];
    KERB_ENCRYPTION_KEY NonceHmacKey;
    KERB_ENCRYPTED_DATA EncryptedData;
    void* pTempBuffer;

    ListElement = (PKERB_PA_DATA_LIST) pScratchMemoryAllocator->Alloc(sizeof(KERB_PA_DATA_LIST));
    if (ListElement == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    {
        CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( pScratchMemoryAllocator );
    
        KerbComputeKdcRequestNonceHmacKey(
            pScratchMemoryAllocator,
            ClientKey,
            nonce,
            abNonceHmacKey
            );

        KerbCreateKeyFromBuffer(
            &NonceHmacKey,
            abNonceHmacKey,
            XONLINE_KEY_LENGTH,
            KERB_ETYPE_RC4_HMAC_NT
            );

        //
        // Now build the client version string
        //
        CXo * pXo = pScratchMemoryAllocator->GetXoPtr();
        dwVersionStringLen = sprintf(acVersionString, "Xbox Version=%s Title=0x%X TitleVersion=%d",
                                     VER_PRODUCTVERSION_STR, pXo->GetTitleId(), pXo->GetTitleVersion()) + 1;

        //
        // Encrypt the client version string 
        //
        KerbErr = KerbAllocateEncryptionBuffer(
                    pScratchMemoryAllocator,
                    NonceHmacKey.keytype,
                    dwVersionStringLen,
                    &EncryptedData.cipher_text.length,
                    &EncryptedData.cipher_text.value
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = KerbMapKerbError(KerbErr);
            goto Cleanup;
        }

        KerbErr = KerbEncryptDataEx(
                    pScratchMemoryAllocator,
                    &EncryptedData,
                    dwVersionStringLen,
                    (BYTE*)acVersionString,
                    NonceHmacKey.keytype,
                    KERB_PA_CLIENT_VER_SALT,
                    &NonceHmacKey
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        //
        // Now pack the encrypted data
        //
        KerbErr = KerbPackEncryptedData(
                    pScratchMemoryAllocator,
                    &EncryptedData,
                    (PULONG) &ListElement->value.preauth_data.length,
                    (PUCHAR *) &ListElement->value.preauth_data.value
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        //
        // Preserve the only piece of memory we care about
        //
        pTempBuffer = PreserveThisMemory( &onlyPreserveThisMemory, &ListElement->value.preauth_data.value, ListElement->value.preauth_data.length );
        if (pTempBuffer == NULL)
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }
    }

    //
    // Fill in remaining preauth fields, and put in list
    //
    ListElement->value.preauth_data_type = KRB5_PADATA_CLIENT_VERSION;
    ListElement->next = *PreAuthData;
    *PreAuthData = ListElement;

Cleanup:
    if (Status != STATUS_SUCCESS)
    {
        TraceSz1( KerbWarn, "KerbAddClientVersionPreAuth failure status 0x%X", Status );
    }
    return(Status);
}

//--------------------------------------------------------------------------

NTSTATUS
KerbAddCompoundIdentityPreAuth(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PXKERB_TGT_CONTEXT pSingleTGTContext,
    IN OUT PKERB_PA_DATA_LIST * PreAuthData
    )
{
    KERBERR KerbErr = KDC_ERR_NONE;
    NTSTATUS Status = STATUS_SUCCESS;
    PKERB_PA_DATA_LIST ListElement;
    PKERB_TICKET pTicket;
    PKERB_PA_COMPOUND_IDENTITY TicketList;
    void* pTempBuffer;
    
    Assert(pScratchMemoryAllocator != NULL);
    Assert(pSingleTGTContext != NULL);
    Assert(PreAuthData != NULL);

    ListElement = (PKERB_PA_DATA_LIST) pScratchMemoryAllocator->Alloc(sizeof(KERB_PA_DATA_LIST));
    if (ListElement == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    {
        CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( pScratchMemoryAllocator );

        //
        // Reconstruct kerb ticket structure from packed data
        //
        pTicket = NULL;
        KerbErr = KerbUnpackData(
                    pScratchMemoryAllocator,
                    pSingleTGTContext->Ticket,
                    pSingleTGTContext->dwTicketLength,
                    KERB_TICKET_PDU,
                    (PVOID*)&pTicket
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        //
        // Allocate memory for the element in the ticket list.
        //
        TicketList = (PKERB_PA_COMPOUND_IDENTITY)pScratchMemoryAllocator->Alloc(sizeof(KERB_PA_COMPOUND_IDENTITY_Element));
        if (TicketList == NULL)
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        //
        // Stick the ticket into the ticket list.
        //
        TicketList->value = *pTicket;
        TicketList->next = NULL;

        KerbErr = KerbPackData(pScratchMemoryAllocator,
                               &TicketList,                           
                               KERB_PA_COMPOUND_IDENTITY_PDU,
                               &ListElement->value.preauth_data.length,
                               &ListElement->value.preauth_data.value);
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto    Cleanup;
        }

        //
        // Preserve the only piece of memory we care about
        //
        pTempBuffer = PreserveThisMemory( &onlyPreserveThisMemory, &ListElement->value.preauth_data.value, ListElement->value.preauth_data.length );
        if (pTempBuffer == NULL)
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }
    }
    
    ListElement->value.preauth_data_type = KRB5_PADATA_COMPOUND_IDENTITY;
    ListElement->next = *PreAuthData;
    *PreAuthData = ListElement;

Cleanup:
    if (Status != STATUS_SUCCESS)
    {
        TraceSz1( KerbWarn, "KerbAddCompoundIdentityPreAuth failure status 0x%X", Status );
    }
    return(Status);
}

//--------------------------------------------------------------------------

void KerbComputeCompoundIdentityKey(
    IN PKERB_ENCRYPTION_KEY pClientKey,
    IN PXKERB_TGT_CONTEXT pSingleTGTContext,
    OUT PBYTE pCompoundKey
    )
{
    MD5_CTX Md5Context;
    UCHAR Key[MD5_LEN];
    ULONG ulSalt = KERB_PA_COMPOUND_IDENTITY_SALT;
    
    Assert(MD5_LEN == XONLINE_KEY_LENGTH);
    Assert(MD5_LEN == pClientKey->keyvalue.length);
    Assert (pSingleTGTContext != NULL);

    md5Hmac(
        pClientKey->keyvalue.value,
        pClientKey->keyvalue.length,
        (PBYTE)"signaturekey",
        sizeof("signaturekey"),
        NULL,
        0,
        Key
        );
        
    MD5Init(&Md5Context);
    MD5Update(&Md5Context, (PUCHAR)&ulSalt, sizeof(ulSalt));

    MD5Update(&Md5Context, pSingleTGTContext->SessionKey, sizeof(pSingleTGTContext->SessionKey));

    MD5Final(&Md5Context);

    md5Hmac(
        Key,
        MD5_LEN,
        Md5Context.digest,
        MD5_LEN,
        NULL,
        0,
        pCompoundKey
        );
}


//--------------------------------------------------------------------------


NTSTATUS
KerbAddXboxServiceRequestPreAuth(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_ENCRYPTION_KEY ClientKey,
    IN PXKERB_TGT_CONTEXT pCombinedTGTContext,
    IN DWORD nonce,
    IN XKERB_SERVICE_CONTEXT* pXKerbServiceContext,
    IN OUT PKERB_PA_DATA_LIST* PreAuthData
    )
{
    KERBERR KerbErr = KDC_ERR_NONE;
    NTSTATUS Status = STATUS_SUCCESS;
    PKERB_PA_DATA_LIST ListElement = NULL;
    PXKERB_PA_XBOX_SERVICE_REQUEST pXboxServicePreAuth;
    BYTE abNonceHmacKey[XONLINE_KEY_LENGTH];
    KERB_ENCRYPTION_KEY NonceHmacKey;
    KERB_ENCRYPTED_DATA EncryptedData;
    void* pTempBuffer;
    DWORD i;

    RtlZeroMemory( &EncryptedData, sizeof(KERB_ENCRYPTED_DATA) );

    pXboxServicePreAuth = (PXKERB_PA_XBOX_SERVICE_REQUEST)_alloca( sizeof(XKERB_PA_XBOX_SERVICE_REQUEST) );
    if (pXboxServicePreAuth == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }
    
    RtlZeroMemory( pXboxServicePreAuth, sizeof(XKERB_PA_XBOX_SERVICE_REQUEST) );
    
    ListElement = (PKERB_PA_DATA_LIST) pScratchMemoryAllocator->Alloc(sizeof(KERB_PA_DATA_LIST));
    if (ListElement == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    {
        CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( pScratchMemoryAllocator );
        CXo * pXo = pScratchMemoryAllocator->GetXoPtr();
        
        //
        // Fill in info to the struct
        //
        pXboxServicePreAuth->wServiceRequestVersion = XONLINE_SERVICE_REQUEST_VERSION;

#ifdef XONLINE_FEATURE_XBOX
        ULONG ulType, ulSize;
        Status = ExQueryNonVolatileSetting(XC_FACTORY_ETHERNET_ADDR, &ulType, pXboxServicePreAuth->abEthernetAddr, sizeof(pXboxServicePreAuth->abEthernetAddr), &ulSize);
        Assert(NT_SUCCESS(Status) && ulSize == sizeof(pXboxServicePreAuth->abEthernetAddr));
#endif

        pXboxServicePreAuth->clientVersion = g_ClientVersion;
        pXboxServicePreAuth->dwTitleID = pXo->GetTitleId();
        pXboxServicePreAuth->dwTitleVersion = pXo->GetTitleVersion();
        pXboxServicePreAuth->dwTitleRegion = pXo->GetTitleGameRegion();

        for (i=0; i<XONLINE_MAX_LOGON_USERS; ++i)
        {
            pXboxServicePreAuth->qwUserID[i] = pCombinedTGTContext->UserID[i];
        }

        for (i=0; i<pXKerbServiceContext->dwNumServices; ++i)
        {
            pXboxServicePreAuth->dwServiceID[i] = pXKerbServiceContext->dwServiceID[i];
        }
        pXboxServicePreAuth->dwNumServices = pXKerbServiceContext->dwNumServices;

#ifdef XNET_FEATURE_TRACE
        {
            char buffer1[256],buf[32];
            DWORD k;
            buffer1[0] = '\0';
            for (k=0; k<pXboxServicePreAuth->dwNumServices; ++k)
            {
                sprintf(buf," %d", pXboxServicePreAuth->dwServiceID[k]);
                strcat(buffer1, buf);
            }
            TraceSz8( AuthVerbose, "KerbAddXboxServiceRequestPreAuth packed Title:0x%X:%d:0x%X User: 0x%016I64X 0x%016I64X 0x%016I64X 0x%016I64X Service:%s",
                pXboxServicePreAuth->dwTitleID,
                pXboxServicePreAuth->dwTitleVersion,
                pXboxServicePreAuth->dwTitleRegion,
                pXboxServicePreAuth->qwUserID[0],
                pXboxServicePreAuth->qwUserID[1],
                pXboxServicePreAuth->qwUserID[2],
                pXboxServicePreAuth->qwUserID[3],
                buffer1 );
        }
#endif

        //
        // Compute the Nonce HMAC key
        //
        KerbComputeKdcRequestNonceHmacKey(
            pScratchMemoryAllocator,
            ClientKey,
            nonce,
            abNonceHmacKey
            );

        KerbCreateKeyFromBuffer(
            &NonceHmacKey,
            abNonceHmacKey,
            XONLINE_KEY_LENGTH,
            KERB_ETYPE_RC4_HMAC_NT
            );

        //
        // Encrypt the pre-auth
        //
        KerbErr = KerbAllocateEncryptionBuffer(
                    pScratchMemoryAllocator,
                    NonceHmacKey.keytype,
                    sizeof(XKERB_PA_XBOX_SERVICE_REQUEST),
                    &EncryptedData.cipher_text.length,
                    &EncryptedData.cipher_text.value
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = KerbMapKerbError(KerbErr);
            goto Cleanup;
        }

        KerbErr = KerbEncryptDataEx(
                    pScratchMemoryAllocator,
                    &EncryptedData,
                    sizeof(XKERB_PA_XBOX_SERVICE_REQUEST),
                    (PUCHAR)pXboxServicePreAuth,
                    NonceHmacKey.keytype,
                    KERB_PA_XBOX_SERVICE_REQUEST_SALT,
                    &NonceHmacKey
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        //
        // Now pack the encrypted data
        //
        KerbErr = KerbPackEncryptedData(
                    pScratchMemoryAllocator,
                    &EncryptedData,
                    (PULONG) &ListElement->value.preauth_data.length,
                    (PUCHAR *) &ListElement->value.preauth_data.value
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        //
        // Preserve the only piece of memory we care about
        //
        pTempBuffer = PreserveThisMemory( &onlyPreserveThisMemory, &ListElement->value.preauth_data.value, ListElement->value.preauth_data.length );
        if (pTempBuffer == NULL)
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }
    }

    ListElement->value.preauth_data_type = KRB5_PADATA_XBOX_SERVICE_REQUEST;
    ListElement->next = *PreAuthData;
    *PreAuthData = ListElement;

Cleanup:
    if (Status != STATUS_SUCCESS)
    {
        TraceSz1( KerbWarn, "KerbAddXboxServiceRequestPreAuth failure status 0x%X", Status );
    }
    return(Status);
}

//--------------------------------------------------------------------------


NTSTATUS
CXo::XkerbBuildAsRequest(
    IN BOOL fMachineAccount,
    IN PXONLINEP_USER User,
    OUT PUCHAR pbRequestMessage,
    IN OUT PULONG pdwRequestMessageLength,
    IN OUT PXKERB_TGT_CONTEXT pTGTContext
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    KERBERR KerbErr = KDC_ERR_NONE;
    KERB_KDC_REQUEST TicketRequest;
    PKERB_KDC_REQUEST_BODY RequestBody = &TicketRequest.request_body;
    ULONG KdcFlagOptions;
    LPSTR pszTargetRealm;
    TimeStamp authTime;
    KERB_ENCRYPTION_KEY ClientKey;
    KERB_ENCRYPTION_KEY CompoundKey;
    KERB_MESSAGE_BUFFER RequestMessage;
    PUCHAR pbScratchMemory;
    char acPrincipleBuffer[XONLINE_KERBEROS_PRINCIPLE_NAME_SIZE];
    CScratchMemoryAllocator ScratchMemoryAllocator;
    DWORD i,j;
    
    Assert( pTGTContext->ctNumNonces < MAX_KERB_RETRIES + MAX_TIME_ADJUSTMENTS );

    //
    // Initialize scratch memory
    //
    pbScratchMemory = (PUCHAR)ScratchMemoryAllocator.ScratchMemoryInitialize( SCRATCH_MEMORY_SIZE_FOR_BuildAsRequest, pbRequestMessage, *pdwRequestMessageLength, this );
    if (pbScratchMemory == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    RtlZeroMemory(&TicketRequest, sizeof(TicketRequest));
    
    KerbCreateKeyFromBuffer(
        &ClientKey,
        User->key,
        XONLINE_KEY_LENGTH,
        KERB_ETYPE_RC4_HMAC_NT
        );

    //
    // Stick all the PA data in the request
    //
    TicketRequest.KERB_KDC_REQUEST_preauth_data = NULL;

    if (pTGTContext->pInputTGTContext != NULL)
    {
        //
        // Compute compound identity keys
        //
        KerbComputeCompoundIdentityKey(
            &ClientKey,
            pTGTContext->pInputTGTContext,
            pTGTContext->CompoundKey
            );

        //
        // Do the PA-COMPOUND-IDENTITY first because it consumes the most memory
        //
        Status = KerbAddCompoundIdentityPreAuth(
                    &ScratchMemoryAllocator,
                    pTGTContext->pInputTGTContext,
                    &TicketRequest.KERB_KDC_REQUEST_preauth_data
                    );
        if (!NT_SUCCESS(Status))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }
    }
    else
    {
        RtlCopyMemory( pTGTContext->CompoundKey, ClientKey.keyvalue.value, XONLINE_KEY_LENGTH );
    }
    
    KerbCreateKeyFromBuffer(
        &CompoundKey,
        pTGTContext->CompoundKey,
        XONLINE_KEY_LENGTH,
        KERB_ETYPE_RC4_HMAC_NT
        );

    //
    // Add the preauth to tell the server to include the PUID PAC.
    //

    Status = KerbAddPacRequestPreAuth(
                &ScratchMemoryAllocator,
                TRUE,
                &TicketRequest.KERB_KDC_REQUEST_preauth_data
                );
    if (!NT_SUCCESS(Status))
    {
        goto Cleanup;
    }

    //
    // Get time for Timestamp Preauth
    //
    KerbGetCurrentTime(&authTime);
    // KerbAddOffsetToTime(&authTime, pTGTContext->qwTimeOffset);
    
    //
    // Add the standard timestamp pre-auth
    //
    Status = KerbAddTimestampPreAuth(
                &ScratchMemoryAllocator,
                &CompoundKey,
                &authTime,
                &TicketRequest.KERB_KDC_REQUEST_preauth_data
                );
    if (!NT_SUCCESS(Status))
    {
        goto Cleanup;
    }

    //
    // Add special Xbox Pre-auth data
    //
    Status = KerbAddClientVersionPreAuth(
                &ScratchMemoryAllocator,
                &ClientKey,
                RequestBody->nonce,
                &TicketRequest.KERB_KDC_REQUEST_preauth_data
                );
    if (!NT_SUCCESS(Status))
    {
        goto Cleanup;
    }

    if (TicketRequest.KERB_KDC_REQUEST_preauth_data != NULL)
    {
        TicketRequest.bit_mask |= KERB_KDC_REQUEST_preauth_data_present;
    }

    //
    // Build the request
    //

    KdcFlagOptions = KerbConvertUlongToFlagUlong(KERB_DEFAULT_TICKET_FLAGS);
    RequestBody->kdc_options.value = (PUCHAR) &KdcFlagOptions;
    RequestBody->kdc_options.length = sizeof(ULONG) * 8;

    XNetRandom((BYTE *)&RequestBody->nonce, sizeof(RequestBody->nonce));

    KerbConvertLargeIntToGeneralizedTime(
        &RequestBody->endtime,
        NULL,
        &g_KerbGlobalWillNeverTime
        );

    //
    // Copy all the names into the request message
    //
    strcpy(acPrincipleBuffer,User->name);
    if (User->kingdom[0] != '\0')
    {
        strcat(acPrincipleBuffer,KINGDOM_SEPERATOR_STRING);
        strcat(acPrincipleBuffer,User->kingdom);
    }
    if (User->domain[0] != '\0')
    {
        strcat(acPrincipleBuffer,"@");
        strcat(acPrincipleBuffer,User->domain);
    }
    KerbErr = KerbConstructPrincipalName(
                &ScratchMemoryAllocator,
                &RequestBody->KERB_KDC_REQUEST_BODY_client_name,
                KRB_NT_ENTERPRISE_PRINCIPAL,
                acPrincipleBuffer
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    RequestBody->bit_mask |= KERB_KDC_REQUEST_BODY_client_name_present;

    pszTargetRealm = fMachineAccount ? XBOX_REALM : (char*)User->realm;
    Assert( strlen(pszTargetRealm) <= XONLINE_MAX_REALM_NAME_LENGTH );
    strcpy(pTGTContext->TargetRealm, pszTargetRealm);

    //
    // Fill in the strings in the ticket request
    //
    KerbErr = KerbConstructPrincipalName(
                &ScratchMemoryAllocator,
                &RequestBody->KERB_KDC_REQUEST_BODY_server_name,
                KRB_NT_SRV_INST,
                KDC_PRINCIPAL_NAME,
                pszTargetRealm
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }
    RequestBody->bit_mask |= KERB_KDC_REQUEST_BODY_server_name_present;

    RequestBody->realm = (char*)User->realm;
    
    TicketRequest.version = KERBEROS_VERSION;
    TicketRequest.message_type = KRB_AS_REQ;

    //
    // Build crypt list
    //

    RequestBody->encryption_type = &g_RC4HMAC_CryptList;

    //
    // Pack the request
    //
    RequestMessage.BufferSize = *pdwRequestMessageLength;
    RequestMessage.Buffer = pbRequestMessage;
    KerbErr = KerbPackAsRequest(
                &ScratchMemoryAllocator,
                &TicketRequest,
                &RequestMessage.BufferSize,
                &RequestMessage.Buffer
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    *pdwRequestMessageLength = RequestMessage.BufferSize;

    j = 0;
    if (pTGTContext->pInputTGTContext != NULL)
    {
        for (i=0; i<XONLINE_MAX_LOGON_USERS; ++i)
        {
            if (pTGTContext->pInputTGTContext->UserID[i] != 0)
            {
                pTGTContext->UserID[j++] = pTGTContext->pInputTGTContext->UserID[i];
            }
        }
    }
    if (!fMachineAccount)
    {
        pTGTContext->UserID[j++] = User->xuid.qwUserID;
    }
    Assert( j <= XONLINE_MAX_LOGON_USERS );
    pTGTContext->Nonce[pTGTContext->ctNumNonces++] = RequestBody->nonce;
    RtlCopyMemory( pTGTContext->ClientKey, User->key, XONLINE_KEY_LENGTH );
    Status = KDC_ERR_NONE;
    
    //
    // Verbose trace info
    //
    TraceSz7( KerbVerbose,"XkerbBuildAsRequest try %d generated %d byte request for User:%s%c%s@%s Realm:%s",
        pTGTContext->ctNumNonces,
        RequestMessage.BufferSize, User->name, KINGDOM_SEPERATOR_CHAR, User->kingdom, User->domain, User->realm
        );

Cleanup:
    return(Status);
}

//--------------------------------------------------------------------------

NTSTATUS
CXo::XkerbVerifyAsReply(
    IN PUCHAR pbReplyMessage,
    IN ULONG dwReplyMessageLength,
    IN OUT PXKERB_TGT_CONTEXT pTGTContext
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    KERBERR KerbErr = KDC_ERR_NONE;
    PKERB_KDC_REPLY KdcReply = NULL;
    PKERB_ENCRYPTED_KDC_REPLY ReplyBody = NULL;
    KERB_ENCRYPTION_KEY CompoundKey;
    PKERB_ERROR ErrorMessage = NULL;
    PUCHAR pEncodedTicket;
    PKERB_PA_DATA pPreAuthData;
    PUCHAR pbScratchMemory;
    CScratchMemoryAllocator ScratchMemoryAllocator;
    DWORD i;
    
    //
    // Initialize scratch memory
    //
    pbScratchMemory = (PUCHAR)ScratchMemoryAllocator.ScratchMemoryInitialize( SCRATCH_MEMORY_SIZE_FOR_VerifyAsReply, NULL, 0, this );
    if (pbScratchMemory == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    KerbErr = KerbUnpackAsReply(
                &ScratchMemoryAllocator,
                pbReplyMessage,
                dwReplyMessageLength,
                &KdcReply
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        //
        // Try to unpack it as  kerb_error
        //
        KerbErr =  KerbUnpackKerbError(
                        &ScratchMemoryAllocator,
                        pbReplyMessage,
                        dwReplyMessageLength,
                        &ErrorMessage
                        );
        if (KERB_SUCCESS(KerbErr))
        {
            KerbErr = (KERBERR) ErrorMessage->error_code;
            TraceSz1( KerbWarn,"XkerbVerifyAsReply: Got KerbError 0x%X", KerbErr );

            Status = KerbMapKerbError(KerbErr);

            //
            // Check for time skew. If so, calculate the skew and retry
            //
            if (KerbErr == KRB_AP_ERR_SKEW)
            {
                TimeStamp KdcTime;
                //TimeStamp XboxTime;

                KerbConvertGeneralizedTimeToLargeInt(
                    &KdcTime,
                    &ErrorMessage->server_time,
                    ErrorMessage->server_usec
                    );

                //KerbGetCurrentTime(&XboxTime);
                //pTGTContext->qwTimeOffset = KerbGetOffsetBetweenTime( &XboxTime, &KdcTime );
                KerbUpdateCurrentTime( KdcTime );
                
                Status = STATUS_TIME_DIFFERENCE_AT_DC;
                goto Cleanup;
            }
            else
            {
                Status = KerbMapKerbError(KerbErr);
                goto Cleanup;
            }
        }
        else
        {
            Status = STATUS_INTERNAL_ERROR;
        }

        goto Cleanup;
    }

    KerbCreateKeyFromBuffer(
        &CompoundKey,
        pTGTContext->CompoundKey,
        XONLINE_KEY_LENGTH,
        KERB_ETYPE_RC4_HMAC_NT
        );

    //
    // Get the XKERB_PA_XBOX_ACCOUNT_CREATION pre-auth, if present
    //
    if ((KdcReply->bit_mask & KERB_KDC_REPLY_preauth_data_present) != 0)
    {
        CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( &ScratchMemoryAllocator );
    
        pPreAuthData = KerbFindPreAuthDataEntry(
                        KRB5_PADATA_XBOX_ACCOUNT_CREATION,
                        (PKERB_PA_DATA_LIST)KdcReply->preauth_data
                        );
        if (pPreAuthData != NULL)
        {
            //
            // Allocate pre-auth using SysAlloc
            // Do not use KerbAllocate here! You will end up with scratch memory!
            //
            pTGTContext->pAccountCreationPreAuth = (PXKERB_PA_XBOX_ACCOUNT_CREATION)SysAlloc(sizeof(XKERB_PA_XBOX_ACCOUNT_CREATION), PTAG_XKERB_PA_XBOX_ACCOUNT_CREATION);
            if (pTGTContext->pAccountCreationPreAuth == NULL)
            {
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto Cleanup;
            }

            //
            // Check the pre-auth reply
            //
            KerbErr = KerbVerifyAccountCreationPreAuth(
                &ScratchMemoryAllocator,
                pPreAuthData,
                &CompoundKey,
                pTGTContext
                );
            if (!KERB_SUCCESS(KerbErr))
            {
                Status = KerbErr;
                goto Cleanup;
            }
        }
    }
    
    {
        CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( &ScratchMemoryAllocator );

        //
        // Pack ticket into marshalled form for caching
        //
        pTGTContext->dwTicketLength = 0;
        pEncodedTicket = NULL;
        KerbErr = KerbPackData(
                    &ScratchMemoryAllocator,
                    &KdcReply->ticket,
                    KERB_TICKET_PDU,
                    &pTGTContext->dwTicketLength,
                    &pEncodedTicket
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        if (pTGTContext->dwTicketLength > XONLINE_MAX_TICKET_LENGTH)
        {
            TraceSz1( KerbWarn, "XkerbVerifyAsReply: got a ticket too big: %d", pTGTContext->dwTicketLength );
            Status = STATUS_LOGON_FAILURE;
            goto Cleanup;
        }

        RtlCopyMemory(
            pTGTContext->Ticket,
            pEncodedTicket,
            pTGTContext->dwTicketLength
            );
    }
    
    //
    // Now unpack the reply body:
    //
    KerbErr = KerbUnpackKdcReplyBody(
                &ScratchMemoryAllocator,
                &KdcReply->encrypted_part,
                &CompoundKey,
                KERB_ENCRYPTED_AS_REPLY_PDU,
                &ReplyBody
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    //
    // Verify the nonce is correct:
    //
    for (i=0; i<pTGTContext->ctNumNonces; ++i)
    {
        if (pTGTContext->Nonce[i] == (DWORD)ReplyBody->nonce)
        {
            break;
        }
    }
    if (i == pTGTContext->ctNumNonces)
    {
        TraceSz3( KerbWarn, "XkerbVerifyAsReply: All %d Nonces don't match: 0x%X vs 0x%X", i, pTGTContext->Nonce[0], ReplyBody->nonce);
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    //
    // Cache the session key
    //
    if (ReplyBody->session_key.keytype != KERB_ETYPE_RC4_HMAC_NT ||
        ReplyBody->session_key.keyvalue.length != XONLINE_KEY_LENGTH)
    {
        TraceSz( KerbWarn, "XkerbVerifyAsReply: got a session key of the wrong type or length" );
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    RtlCopyMemory( pTGTContext->SessionKey, ReplyBody->session_key.keyvalue.value, XONLINE_KEY_LENGTH );

    //
    // Copy the principal name (might be different because of name canonicalization)
    //
    KerbErr = KerbPrincipalNameToMultiString(
        &KdcReply->client_name,
        sizeof(pTGTContext->ClientName),
        pTGTContext->ClientName
        );
    if (!KERB_SUCCESS(KerbErr))
    {
        TraceSz( KerbWarn, "XkerbVerifyAsReply: returned client name is too long" );
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }        

    if ( strlen(KdcReply->client_realm) <= XONLINE_MAX_REALM_NAME_LENGTH )
    {
        strcpy(pTGTContext->ClientRealm, KdcReply->client_realm);    
    }
    else
    {
        TraceSz( KerbWarn, "XkerbVerifyAsReply: returned client realm is too long" );
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }        

    //
    // Cache expiration time
    //
    KerbConvertGeneralizedTimeToLargeInt(
        &pTGTContext->ExpirationTime,
        &ReplyBody->endtime,
        0
        );
    
    KerbConvertGeneralizedTimeToLargeInt(
        &pTGTContext->StartTime,
        &ReplyBody->starttime,
        0
        );
    
    //
    // Verbose trace info
    //
    TraceSz2( KerbVerbose,"XkerbVerifyAsReply: extracted %d byte ticket from %d byte reply",
        pTGTContext->dwTicketLength, dwReplyMessageLength
        );

Cleanup:
    return(Status);
}

//--------------------------------------------------------------------------


NTSTATUS
CXo::XkerbBuildTgsRequest(
    IN PXKERB_TGT_CONTEXT pCombinedTGTContext,
    OUT PUCHAR pbRequestMessage,
    IN OUT PULONG pdwRequestMessageLength,
    IN OUT PXKERB_SERVICE_CONTEXT pXKerbServiceContext
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    KERBERR KerbErr;
    KERB_PA_DATA_LIST ApRequest;
    KERB_CHECKSUM RequestChecksum;
    ULONG KdcFlagOptions;
    KERB_KDC_REQUEST TicketRequest;
    PKERB_KDC_REQUEST_BODY RequestBody = &TicketRequest.request_body;
    KERB_MESSAGE_BUFFER RequestMessage;
    KERB_ENCRYPTION_KEY SessionKey;
    PKERB_TICKET pTGT = NULL;
    CHAR buffer[16];
    void* pTempBuffer;
    DWORD i;
    PUCHAR pbScratchMemory;
    CScratchMemoryAllocator ScratchMemoryAllocator;

    Assert(pCombinedTGTContext != NULL);
    Assert(pXKerbServiceContext != NULL);
#ifndef TESTKERB
    Assert(pXKerbServiceContext->dwNumServices != 0);
#endif
    Assert(pXKerbServiceContext->ctNumNonces < MAX_KERB_RETRIES + MAX_TIME_ADJUSTMENTS);

    //
    // Initialize scratch memory
    //
    pbScratchMemory = (PUCHAR)ScratchMemoryAllocator.ScratchMemoryInitialize( SCRATCH_MEMORY_SIZE_FOR_BuildTgsRequest, pbRequestMessage, *pdwRequestMessageLength, this );
    if (pbScratchMemory == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    //
    // Initialize structures
    //
    RtlZeroMemory(
        &ApRequest,
        sizeof(KERB_PA_DATA_LIST)
        );

    RtlZeroMemory(
        &RequestChecksum,
        sizeof(KERB_CHECKSUM)
        );

    RtlZeroMemory(
        &TicketRequest,
        sizeof(KERB_KDC_REQUEST)
        );

    RtlCopyMemory( pXKerbServiceContext->ServiceRealm, pCombinedTGTContext->TargetRealm, sizeof(pXKerbServiceContext->ServiceRealm) );
    Assert( pXKerbServiceContext->UserID[0] == pCombinedTGTContext->UserID[0] );
    Assert( pXKerbServiceContext->UserID[1] == pCombinedTGTContext->UserID[1] );
    Assert( pXKerbServiceContext->UserID[2] == pCombinedTGTContext->UserID[2] );
    Assert( pXKerbServiceContext->UserID[3] == pCombinedTGTContext->UserID[3] );

    //
    // Fill in the ticket request with the defaults.
    //
    KerbConvertLargeIntToGeneralizedTime(
        &RequestBody->endtime,
        NULL,
        &g_KerbGlobalWillNeverTime // BUGBUG: use HasNeverTime instead
        );

    KerbConvertLargeIntToGeneralizedTime(
        &RequestBody->KERB_KDC_REQUEST_BODY_renew_until,
        NULL,
        &g_KerbGlobalWillNeverTime // BUGBUG: use HasNeverTime instead
        );

    XNetRandom((BYTE *)&RequestBody->nonce, sizeof(RequestBody->nonce));
    
    //
    // Build crypt list
    //

    RequestBody->encryption_type = &g_RC4HMAC_CryptList;

    //
    // Fill in the strings in the ticket request
    //
    sprintf( buffer, "S%d", pXKerbServiceContext->dwServiceID[0] );
    
    KerbErr = KerbConstructPrincipalName(
                &ScratchMemoryAllocator,
                &RequestBody->KERB_KDC_REQUEST_BODY_server_name,
                KRB_NT_SRV_INST,
                SECURITY_GATEWAY_STRING,
                buffer
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }
    RequestBody->bit_mask |= KERB_KDC_REQUEST_BODY_server_name_present;

    KdcFlagOptions = KerbConvertUlongToFlagUlong(KERB_DEFAULT_TICKET_FLAGS);
    RequestBody->kdc_options.length = sizeof(ULONG) * 8;
    RequestBody->kdc_options.value = (PUCHAR) &KdcFlagOptions;

    //
    // Marshall the request and compute a checksum of it
    //
    RequestBody->realm = pXKerbServiceContext->ServiceRealm;
    
    //
    // Construct encryption key struct for session key
    //
    KerbCreateKeyFromBuffer(
        &SessionKey,
        pCombinedTGTContext->SessionKey,
        XONLINE_KEY_LENGTH,
        KERB_ETYPE_RC4_HMAC_NT
        );

    {
        CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( &ScratchMemoryAllocator );
    
        //
        // Now compute a checksum of that data
        //

        Status = KerbComputeRequestBodyChecksum(
                    &ScratchMemoryAllocator,
                    RequestBody,
                    &RequestChecksum
                    );
        if (!NT_SUCCESS(Status))
        {
            goto Cleanup;
        }
        
        KerbErr = KerbUnpackData(
                    &ScratchMemoryAllocator,
                    pCombinedTGTContext->Ticket,
                    pCombinedTGTContext->dwTicketLength,
                    KERB_TICKET_PDU,
                    (PVOID*)&pTGT
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        //
        // Create the AP request to the KDC for the ticket to the service
        //
        KerbGetCurrentTime(&pXKerbServiceContext->AuthenticatorTime[pXKerbServiceContext->ctNumNonces]);

        KerbErr = KerbCreateApRequest(
                    &ScratchMemoryAllocator,
                    pCombinedTGTContext->ClientName,
                    pCombinedTGTContext->ClientRealm,
                    &SessionKey,
                    RequestBody->nonce,
                    &pXKerbServiceContext->AuthenticatorTime[pXKerbServiceContext->ctNumNonces],
                    pTGT,
                    0,                              // no AP options
                    &RequestChecksum,
                    TRUE,                           // kdc request
                    (PULONG) &ApRequest.value.preauth_data.length,
                    &ApRequest.value.preauth_data.value
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        //
        // Preserve the only piece of memory we care about
        //
        pTempBuffer = PreserveThisMemory( &onlyPreserveThisMemory, &ApRequest.value.preauth_data.value, ApRequest.value.preauth_data.length );
        if (pTempBuffer == NULL)
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }
    }
    
    ApRequest.next = NULL;
    ApRequest.value.preauth_data_type = KRB5_PADATA_TGS_REQ;
    TicketRequest.KERB_KDC_REQUEST_preauth_data = &ApRequest;
    TicketRequest.bit_mask |= KERB_KDC_REQUEST_preauth_data_present;

    //
    // Add the preauth to tell the server to include the PUID PAC.
    //
    Status = KerbAddPacRequestPreAuth(
                &ScratchMemoryAllocator,
                FALSE,
                &TicketRequest.KERB_KDC_REQUEST_preauth_data
                );
    if (!NT_SUCCESS(Status))
    {
        goto Cleanup;
    }

    //
    // Add Xbox pre-auth info
    //
    Status = KerbAddXboxServiceRequestPreAuth(
                &ScratchMemoryAllocator,
                &SessionKey,
                pCombinedTGTContext,
                RequestBody->nonce,
                pXKerbServiceContext,
                &TicketRequest.KERB_KDC_REQUEST_preauth_data
                );
    if (!NT_SUCCESS(Status))
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }
    
    //
    // Add version and message type info
    //

    TicketRequest.version = KERBEROS_VERSION;
    TicketRequest.message_type = KRB_TGS_REQ;

    //
    // Pack the request
    //
    RequestMessage.BufferSize = *pdwRequestMessageLength;
    RequestMessage.Buffer = pbRequestMessage;
    KerbErr = KerbPackTgsRequest(
                &ScratchMemoryAllocator,
                &TicketRequest,
                &RequestMessage.BufferSize,
                &RequestMessage.Buffer
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    *pdwRequestMessageLength = RequestMessage.BufferSize;

    pXKerbServiceContext->Nonce[pXKerbServiceContext->ctNumNonces++] = RequestBody->nonce;
    Status = KDC_ERR_NONE;

    //
    // Verbose trace info
    //
    TraceSz3( KerbVerbose,"XkerbBuildTgsRequest try %d generated %d byte request with %d byte Combined TGT",
        pXKerbServiceContext->ctNumNonces,
        RequestMessage.BufferSize,
        pCombinedTGTContext->dwTicketLength,
        );

Cleanup:
    return(Status);
}

//--------------------------------------------------------------------------

NTSTATUS
CXo::XkerbVerifyTgsReply(
    IN PXONLINEP_USER pUsers,
    IN PXKERB_TGT_CONTEXT pCombinedTGTContext,
    IN PUCHAR pbReplyMessage,
    IN ULONG dwReplyMessageLength,
    IN OUT PXKERB_SERVICE_CONTEXT pXKerbServiceContext
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    KERBERR KerbErr;
    PKERB_KDC_REPLY KdcReply = NULL;
    PKERB_ENCRYPTED_KDC_REPLY ReplyBody = NULL;
    KERB_ENCRYPTION_KEY SessionKey;
    PKERB_PA_DATA pPreAuthData;
    PXKERB_PA_XBOX_SERVICE_ADDRESS pPAServiceAddress = NULL;
    PUCHAR pEncodedTicket;
    DWORD i,j;
    PUCHAR pbScratchMemory;
    CScratchMemoryAllocator ScratchMemoryAllocator;

    Assert(pCombinedTGTContext != NULL);
    Assert(pXKerbServiceContext != NULL);

    //
    // Initialize scratch memory
    //
    pbScratchMemory = (PUCHAR)ScratchMemoryAllocator.ScratchMemoryInitialize( SCRATCH_MEMORY_SIZE_FOR_VerifyTgsReply, NULL, 0, this );
    if (pbScratchMemory == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    //
    // Unpack the reply
    //
    KerbErr = KerbUnpackTgsReply(
                &ScratchMemoryAllocator,
                pbReplyMessage,
                dwReplyMessageLength,
                &KdcReply
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        PKERB_ERROR ErrorMessage = NULL;

        //
        // Try to unpack it as  kerb_error
        //

        KerbErr =  KerbUnpackKerbError(
                        &ScratchMemoryAllocator,
                        pbReplyMessage,
                        dwReplyMessageLength,
                        &ErrorMessage
                        );
        if (KERB_SUCCESS(KerbErr))
        {
            KerbErr = (KERBERR) ErrorMessage->error_code;
            TraceSz1( KerbWarn,"XkerbVerifyTgsReply: Got KerbError 0x%X", KerbErr );

            //
            // Check for time skew. If we got a skew error, record the time
            // skew between here and the KDC in the ticket so we can retry
            // with the correct time.
            //

            if (KerbErr == KRB_AP_ERR_SKEW)
            {
                TimeStamp KdcTime;

                KerbConvertGeneralizedTimeToLargeInt(
                    &KdcTime,
                    &ErrorMessage->server_time,
                    ErrorMessage->server_usec
                    );

                KerbUpdateCurrentTime( KdcTime );
                
                Status = STATUS_TIME_DIFFERENCE_AT_DC;
                goto Cleanup;
            }
            KerbFreeKerbError(ErrorMessage);
            Status = KerbMapKerbError(KerbErr);
        }
        else
        {
            Status = STATUS_LOGON_FAILURE;
        }
        goto Cleanup;
    }

    //
    // Construct encryption key struct for session key
    //
    KerbCreateKeyFromBuffer(
        &SessionKey,
        pCombinedTGTContext->SessionKey,
        XONLINE_KEY_LENGTH,
        KERB_ETYPE_RC4_HMAC_NT
        );

    //
    // Get the XKERB_PA_XBOX_SERVICE_ADDRESS pre-auth
    //
    pPreAuthData = KerbFindPreAuthDataEntry(
                    KRB5_PADATA_XBOX_SERVICE_ADDRESS,
                    (PKERB_PA_DATA_LIST)KdcReply->preauth_data
                    );
    if (pPreAuthData == NULL)
    {
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    //
    // Allocate Service Address reply memory on stack
    //
    pPAServiceAddress = (PXKERB_PA_XBOX_SERVICE_ADDRESS) _alloca( sizeof(XKERB_PA_XBOX_SERVICE_ADDRESS) );
    if (pPAServiceAddress == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    //
    // Check the pre-auth reply
    //
    KerbErr = KerbVerifyServiceAddressPreAuth(
        &ScratchMemoryAllocator,
        pPreAuthData,
        &SessionKey,
        pXKerbServiceContext,
        pPAServiceAddress
        );
    if (!KERB_SUCCESS(KerbErr))
    {
        Status = KerbErr;
        goto Cleanup;
    }

    //
    // Check the master return HRESULT
    //
    pXKerbServiceContext->hr = pPAServiceAddress->hr;
    if (SUCCEEDED(pPAServiceAddress->hr))
    {
        //
        // Handle the returned Service address results
        //
        pXKerbServiceContext->siteIPAddress = pPAServiceAddress->siteIPAddress;
        Assert( pPAServiceAddress->dwNumServices <= XONLINE_MAX_NUMBER_SERVICE );
        Assert( pPAServiceAddress->dwNumServices == pXKerbServiceContext->dwNumServices );
        for (i=0; i<pXKerbServiceContext->dwNumServices; ++i)
        {
            // Should be same as pPAServiceAddress->dwNumServices
            for (j=0; j<pXKerbServiceContext->dwNumServices; ++j)
            {
                if ( pXKerbServiceContext->dwServiceID[i] == pPAServiceAddress->serviceResult[j].dwServiceID )
                {
                    pXKerbServiceContext->hrService[i] = pPAServiceAddress->serviceResult[j].hr;
                    pXKerbServiceContext->wServicePort[i] = pPAServiceAddress->serviceResult[j].wServicePort;
                    break;
                }
            }
        }

        //
        // Pass returned authorization info about users
        //
        if ( pUsers != NULL )
        {
            for (i=0; i<XONLINE_MAX_LOGON_USERS; ++i)
            {
                for (j=0; j<XONLINE_MAX_LOGON_USERS; ++j)
                {
                    if (pUsers[i].xuid.qwUserID == pCombinedTGTContext->UserID[j])
                    {
                        // Use 'or' to avoid clobbering the guest bits.
                        pUsers[i].xuid.dwUserFlags |= pPAServiceAddress->dwUserFlags[j];
                        pUsers[i].hr = pPAServiceAddress->hrUser[j];
                        break;                    
                    }
                }
            }
        }
    }
    
    //
    // Cache the ticket
    //
    {
        CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( &ScratchMemoryAllocator );

        pXKerbServiceContext->dwTicketLength = 0;
        pEncodedTicket = NULL;
        KerbErr = KerbPackData(
                    &ScratchMemoryAllocator,
                    &KdcReply->ticket,
                    KERB_TICKET_PDU,
                    &pXKerbServiceContext->dwTicketLength,
                    &pEncodedTicket
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        if (pXKerbServiceContext->dwTicketLength > XONLINE_MAX_TICKET_LENGTH)
        {
            TraceSz1( KerbWarn, "XkerbVerifyTgsReply: got a ticket too big: %d", pXKerbServiceContext->dwTicketLength );
            Status = STATUS_LOGON_FAILURE;
            goto Cleanup;
        }

        RtlCopyMemory(
            pXKerbServiceContext->Ticket,
            pEncodedTicket,
            pXKerbServiceContext->dwTicketLength
            );
    }
    
    //
    // Now unpack the reply body:
    //
    KerbErr = KerbUnpackKdcReplyBody(
                &ScratchMemoryAllocator,
                &KdcReply->encrypted_part,
                &SessionKey,
                KERB_ENCRYPTED_TGS_REPLY_PDU,
                &ReplyBody
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    //
    // Verify the nonce is correct:
    //
    for (i=0; i<pXKerbServiceContext->ctNumNonces; ++i)
    {
        if (pXKerbServiceContext->Nonce[i] == (DWORD)ReplyBody->nonce)
        {
            break;
        }
    }
    if (i == pXKerbServiceContext->ctNumNonces)
    {
        TraceSz3( KerbWarn, "XkerbVerifyTgsReply: All %d Nonces don't match: 0x%X vs 0x%X", i, pXKerbServiceContext->Nonce[0], ReplyBody->nonce);
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    //
    // Cache the session key
    //
    if (ReplyBody->session_key.keytype != KERB_ETYPE_RC4_HMAC_NT ||
        ReplyBody->session_key.keyvalue.length != XONLINE_KEY_LENGTH)
    {
        TraceSz2( KerbWarn, "XkerbVerifyTgsReply got a session key of the wrong type (%d) or length(%d)",
            ReplyBody->session_key.keytype, ReplyBody->session_key.keyvalue.length
            );
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    RtlCopyMemory( pXKerbServiceContext->SessionKey, ReplyBody->session_key.keyvalue.value, XONLINE_KEY_LENGTH );

    //
    // Copy the principal name (might be different because of name canonicalization)
    //
    KerbErr = KerbPrincipalNameToMultiString(
        &KdcReply->client_name,
        sizeof(pXKerbServiceContext->ClientName),
        pXKerbServiceContext->ClientName
        );
    if (!KERB_SUCCESS(KerbErr))
    {
        TraceSz( KerbWarn, "XkerbVerifyTgsReply: returned client name is too long" );
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }        

    if ( strlen(KdcReply->client_realm) <= XONLINE_MAX_REALM_NAME_LENGTH )
    {
        strcpy(pXKerbServiceContext->ClientRealm, KdcReply->client_realm);    
    }
    else
    {
        TraceSz( KerbWarn, "XkerbVerifyTgsReply: returned client realm is too long" );
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }        

    //
    // Cache the expiration time
    //
    KerbConvertGeneralizedTimeToLargeInt(
        &pXKerbServiceContext->ExpirationTime,
        &ReplyBody->endtime,
        0
        );
    
    KerbConvertGeneralizedTimeToLargeInt(
        &pXKerbServiceContext->StartTime,
        &ReplyBody->starttime,
        0
        );
    
    //
    // Verbose trace info
    //
    TraceSz2( KerbVerbose,"XkerbVerifyTgsReply: extracted %d byte ticket from %d byte reply",
        pXKerbServiceContext->dwTicketLength, dwReplyMessageLength
        );

Cleanup:
    return(Status);
}


//--------------------------------------------------------------------------
    
NTSTATUS
CXo::XkerbBuildApRequest(
    IN PXKERB_SERVICE_CONTEXT pServiceContext,
    OUT LARGE_INTEGER * pliTime,
    IN BYTE * pbSha,
    IN UINT cbSha, 
    OUT PUCHAR pbRequestMessage,
    IN OUT UINT * pdwRequestMessageLength
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    KERBERR KerbErr;
    ULONG ApOptions = KERB_AP_OPTIONS_mutual_required;
    ULONG RequestSize;
    PUCHAR RequestStart;
    PUCHAR MarshalledApRequest;
    ULONG ApRequestSize;
    KERB_ENCRYPTION_KEY SessionKey;
    PKERB_TICKET pTicket = NULL;
    KERB_CHECKSUM RequestChecksum;
    PUCHAR pbScratchMemory;
    CScratchMemoryAllocator ScratchMemoryAllocator;

    //
    // Initialize scratch memory
    //
    pbScratchMemory = (PUCHAR)ScratchMemoryAllocator.ScratchMemoryInitialize( SCRATCH_MEMORY_SIZE_FOR_BuildApRequest, pbRequestMessage, *pdwRequestMessageLength, this );
    if (pbScratchMemory == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    //
    // Set the authenticator time
    //
    KerbGetCurrentTime( pliTime );

    //
    // Construct encryption key struct for session key
    //
    KerbCreateKeyFromBuffer(
        &SessionKey,
        pServiceContext->SessionKey,
        XONLINE_KEY_LENGTH,
        KERB_ETYPE_RC4_HMAC_NT
        );

    //
    // Construct kerb ticket structure from packed data
    //
    KerbErr = KerbUnpackData(
                &ScratchMemoryAllocator,
                pServiceContext->Ticket,
                pServiceContext->dwTicketLength,
                KERB_TICKET_PDU,
                (PVOID*)&pTicket
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    //
    // Setup the application checksum
    //
    RtlZeroMemory( &RequestChecksum, sizeof(KERB_CHECKSUM) );
    RequestChecksum.checksum.value = pbSha;
    RequestChecksum.checksum.length = cbSha;
    RequestChecksum.checksum_type = KERB_CHECKSUM_SHA1;

    //
    // Create the AP request
    //
    ApRequestSize = *pdwRequestMessageLength;
    MarshalledApRequest = pbRequestMessage;
    KerbErr = KerbCreateApRequest(
                &ScratchMemoryAllocator,
                pServiceContext->ClientName,
                pServiceContext->ClientRealm,
                &SessionKey,
                0, // KerbAllocateNonce(),
                pliTime,
                pTicket,
                ApOptions,
                &RequestChecksum,
                FALSE,                          // not a KDC request
                &ApRequestSize,
                &MarshalledApRequest
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    *pdwRequestMessageLength = ApRequestSize;

    TraceSz2( KerbVerbose, "XkerbBuildApRequest built Ap request %d bytes long (%X checksum)",
        ApRequestSize, XConfigChecksum(pbRequestMessage, *pdwRequestMessageLength) );

Cleanup:
    return(Status);
}

//--------------------------------------------------------------------------

NTSTATUS
CXo::XkerbVerifyApReply(
    IN PXKERB_SERVICE_CONTEXT pServiceContext,
    IN LARGE_INTEGER * pliTime,
    OUT BYTE * pbSha,
    IN UINT cbSha, 
    IN PUCHAR pbReplyMessage,
    IN UINT dwReplyMessageLength
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    KERBERR KerbErr;
    PKERB_AP_REPLY Reply = NULL;
    PKERB_ENCRYPTED_AP_REPLY ReplyBody = NULL;
    KERB_TIME ReplyAuthenticatorTime;
    long ReplyAuthenticatorUsec;
    KERB_ENCRYPTION_KEY SessionKey;
    PUCHAR pbScratchMemory;
    CScratchMemoryAllocator ScratchMemoryAllocator;

    TraceSz2( KerbVerbose, "XkerbVerifyApReply got Ap reply %d bytes long (%X checksum)",
        dwReplyMessageLength, XConfigChecksum(pbReplyMessage, dwReplyMessageLength) );

    //
    // Initialize scratch memory
    //
    pbScratchMemory = (PUCHAR)ScratchMemoryAllocator.ScratchMemoryInitialize( SCRATCH_MEMORY_SIZE_FOR_VerifyApReply, NULL, 0, this );
    if (pbScratchMemory == NULL)
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    if (!KERB_SUCCESS(KerbUnpackApReply(
                        &ScratchMemoryAllocator,
                        pbReplyMessage,
                        dwReplyMessageLength,
                        &Reply
                        )))
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    if ((Reply->version != KERBEROS_VERSION) ||
        (Reply->message_type != KRB_AP_REP))
    {
        TraceSz2( KerbWarn, "XkerbVerifyApReply: Illegal version (0x%X) or message type (0x%X)",
            Reply->version, Reply->message_type
            );
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    //
    // Construct encryption key struct for session key
    //
    KerbCreateKeyFromBuffer(
        &SessionKey,
        pServiceContext->SessionKey,
        XONLINE_KEY_LENGTH,
        KERB_ETYPE_RC4_HMAC_NT
        );

    //
    // Now decrypt the encrypted part.
    //
    KerbErr = KerbDecryptDataEx(
                &ScratchMemoryAllocator,
                &Reply->encrypted_part,
                &SessionKey,
                KERB_AP_REP_SALT,
                (PULONG) &Reply->encrypted_part.cipher_text.length,
                Reply->encrypted_part.cipher_text.value
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        if (KerbErr == KRB_ERR_GENERIC)
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
        }
        else
        {
            Status = STATUS_LOGON_FAILURE;
        }
        goto Cleanup;
    }

    //
    // Decode the contents now
    //
    if (!KERB_SUCCESS(KerbUnpackApReplyBody(
                        &ScratchMemoryAllocator,
                        Reply->encrypted_part.cipher_text.value,
                        Reply->encrypted_part.cipher_text.length,
                        &ReplyBody)))
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    KerbConvertLargeIntToGeneralizedTime(
        &ReplyAuthenticatorTime,
        &ReplyAuthenticatorUsec,
        pliTime
        );
        
    if ((ReplyAuthenticatorUsec != ReplyBody->client_usec) ||
        memcmp( &ReplyAuthenticatorTime, &(ReplyBody->client_time), sizeof(ReplyAuthenticatorTime) ))
    {
        TraceSz( KerbWarn, "XkerbVerifyApReply: clientTime and clientUsec verification failed" );
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    //
    // Check to see if a session key was sent back.
    // This session key is the returned application checksum.
    //
    if ((ReplyBody->bit_mask & KERB_ENCRYPTED_AP_REPLY_subkey_present) == 0)
    {
        TraceSz( KerbWarn, "XkerbVerifyApReply didn't get a application checksum" );
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    if (ReplyBody->KERB_ENCRYPTED_AP_REPLY_subkey.keytype != KERB_CHECKSUM_SHA1 ||
        ReplyBody->KERB_ENCRYPTED_AP_REPLY_subkey.keyvalue.length != cbSha)
    {
        TraceSz2( KerbWarn, "XkerbVerifyApReply got a session key of the wrong type (%d) or length(%d)",
            ReplyBody->KERB_ENCRYPTED_AP_REPLY_subkey.keytype, ReplyBody->KERB_ENCRYPTED_AP_REPLY_subkey.keyvalue.length
            );
        Status = STATUS_LOGON_FAILURE;
        goto Cleanup;
    }

    RtlCopyMemory( pbSha, ReplyBody->KERB_ENCRYPTED_AP_REPLY_subkey.keyvalue.value, cbSha );

    Status = STATUS_SUCCESS;

Cleanup:
    return(Status);
}

//+-------------------------------------------------------------------------

VOID
KerbConvertGeneralizedTimeToLargeInt(
    OUT PTimeStamp TimeStamp,
    IN PKERB_TIME ClientTime,
    IN int ClientUsec
    )
{
    KERB_TIME ZeroTime;
    TIME_FIELDS TimeFields;

    //
    // Special case zero time
    //

    RtlZeroMemory(
        &ZeroTime,
        sizeof(KERB_TIME)
        );

    ZeroTime.universal = TRUE;

    //
    // Skip this check after 3/1/97 - no clients should send this sort of
    // zero time
    //
    if (RtlEqualMemory(
            &ZeroTime,
            ClientTime,
            sizeof(KERB_TIME)
            ))
    {
        KerbSetTime(TimeStamp,0);
        return;
    }

    //
    // Check for MIT zero time
    //

    ZeroTime.year = 1970;
    ZeroTime.month = 1;
    ZeroTime.day = 1;

    if (RtlEqualMemory(
            &ZeroTime,
            ClientTime,
            sizeof(KERB_TIME)
            ))
    {
        KerbSetTime(TimeStamp,0);
        return;
    }
    else
    {
        TimeFields.Year = ClientTime->year;
        TimeFields.Month = ClientTime->month;
        TimeFields.Day = ClientTime->day;
        TimeFields.Hour = ClientTime->hour;
        TimeFields.Minute = ClientTime->minute;
        TimeFields.Second = ClientTime->second;
        TimeFields.Milliseconds = ClientTime->millisecond;  // to convert from micro to milli
        TimeFields.Weekday = 0;

#ifndef TIMESTAMPISINT64
        RtlTimeFieldsToTime(
            &TimeFields,
            TimeStamp
            );
#else // TIMESTAMPISINT64
        LARGE_INTEGER TempTimeStamp;
        RtlTimeFieldsToTime(
            &TimeFields,
            &TempTimeStamp
            );
        *TimeStamp = TempTimeStamp.QuadPart;
#endif // TIMESTAMPISINT64

        //
        // add in any micro seconds
        //

        KerbGetTime(*TimeStamp) += ClientUsec * 10;

    }

}


//+-------------------------------------------------------------------------

VOID
KerbConvertLargeIntToGeneralizedTime(
    OUT PKERB_TIME ClientTime,
    OUT OPTIONAL LONG* ClientUsec,
    IN PTimeStamp TimeStamp
    )
{
    TIME_FIELDS TimeFields;

    //
    // This zeroing is needed or else a useless byte in the middle might be different
    //
    RtlZeroMemory(
        ClientTime,
        sizeof(KERB_TIME)
        );

    //
    // Special case zero time
    //

    if (KerbGetTime(*TimeStamp) == 0)
    {
        //
        // BUGBUG: For MIT compatibility, time zero is 1/1/70
        //

        ClientTime->year = 1970;
        ClientTime->month = 1;
        ClientTime->day = 1;

        if (ARGUMENT_PRESENT(ClientUsec))

        {
            *ClientUsec  = 0;
        }
        ClientTime->universal = TRUE;
    }
    else
    {

        RtlTimeToTimeFields(
            (LARGE_INTEGER*)TimeStamp,
            &TimeFields
            );

        //
        // Generalized times can only contains years up to four digits.
        //

        if (TimeFields.Year > 2037)
        {
            ClientTime->year = 2037;
        }
        else
        {
            ClientTime->year = TimeFields.Year;
        }
        ClientTime->month = (ASN1uint8_t) TimeFields.Month;
        ClientTime->day = (ASN1uint8_t) TimeFields.Day;
        ClientTime->hour = (ASN1uint8_t) TimeFields.Hour;
        ClientTime->minute = (ASN1uint8_t) TimeFields.Minute;
        ClientTime->second = (ASN1uint8_t) TimeFields.Second;

        // BUGBUG: MIT kerberos does not support millseconds
        //
        // ClientTime->millisec = TimeFields.Milliseconds;
        //

        ClientTime->millisecond = 0;

        if (ARGUMENT_PRESENT(ClientUsec))
        {
            //
            // The low part of time is 100 nanoseconds, so one 10th of that
            // is microseconds. We only want the microseconds not included
            // in the milliseconds above.
            //
            //
            // BUGBUG: since we don't includ milliseconds above, use the whole
            // thing here.
            //

#ifndef TIMESTAMPISINT64
            *ClientUsec = (TimeStamp->LowPart / 10) % 1000000;
#else // TIMESTAMPISINT64
            *ClientUsec = (int) ((*TimeStamp / 10) % 1000000);
#endif // TIMESTAMPISINT64
        }

        ClientTime->diff = 0;
        ClientTime->universal = TRUE;
    }

}

//+---------------------------------------------------------------------------

KERBERR NTAPI
KerbCreateAuthenticator(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_ENCRYPTION_KEY pkKey,
    IN ULONG EncryptionType,
    IN ULONG SequenceNumber,
    IN PTimeStamp pAuthenticatorTime,
    IN LPSTR ClientName,
    IN LPSTR ClientRealm,
    IN OPTIONAL PKERB_CHECKSUM Checksum,
    IN BOOLEAN KdcRequest,
    OUT PKERB_ENCRYPTED_DATA Authenticator
    )
{
    KERB_AUTHENTICATOR InternalAuthenticator;
    PKERB_AUTHENTICATOR AuthPointer = &InternalAuthenticator;
    ULONG cbAuthenticator;
    PUCHAR PackedAuthenticator;
    KERBERR KerbErr = KDC_ERR_NONE;

    Authenticator->cipher_text.value = NULL;

    RtlZeroMemory(
        &InternalAuthenticator,
        sizeof(KERB_AUTHENTICATOR)
        );

    // Build an authenticator

    InternalAuthenticator.authenticator_version = KERBEROS_VERSION;

    InternalAuthenticator.client_realm = ClientRealm;
    
    KerbErr = KerbMultiStringToPrincipalName(
                pScratchMemoryAllocator,
                ClientName,
                &InternalAuthenticator.client_name
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        goto Cleanup;
    }

    //
    // Stick the correct time in the authenticator
    //

    KerbConvertLargeIntToGeneralizedTime(
        &InternalAuthenticator.client_time,
        &InternalAuthenticator.client_usec,
        pAuthenticatorTime
        );

    InternalAuthenticator.bit_mask |= KERB_AUTHENTICATOR_sequence_number_present;

    InternalAuthenticator.KERB_AUTHENTICATOR_sequence_number = SequenceNumber;

    //
    // If the checksum is present, include it and set it in the bitmask
    //

    if (ARGUMENT_PRESENT(Checksum))
    {
        InternalAuthenticator.checksum = *Checksum;
        InternalAuthenticator.bit_mask |= checksum_present;
    }

    cbAuthenticator = 0;
    PackedAuthenticator = NULL;
    KerbErr = KerbPackData(
                pScratchMemoryAllocator,
                AuthPointer,
                KERB_AUTHENTICATOR_PDU,
                &cbAuthenticator,
                &PackedAuthenticator
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        goto Cleanup;
    }

    //
    // Now we need to encrypt the buffer
    //

        KerbErr = KerbAllocateEncryptionBuffer(
                pScratchMemoryAllocator,
                EncryptionType,
                cbAuthenticator,
                &Authenticator->cipher_text.length,
                &Authenticator->cipher_text.value
                );

    if (!KERB_SUCCESS(KerbErr))
    {
        goto Cleanup;
    }


    KerbErr = KerbEncryptDataEx(
                pScratchMemoryAllocator,
                Authenticator,
                cbAuthenticator,
                PackedAuthenticator,
                EncryptionType,
                KdcRequest ? KERB_TGS_REQ_AP_REQ_AUTH_SALT : KERB_AP_REQ_AUTH_SALT,
                pkKey
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        goto Cleanup;
    }

Cleanup:
    if (!KERB_SUCCESS(KerbErr))
    {
        TraceSz1( KerbWarn, "KerbCreateAuthenticator failure KerbErr 0x%X", KerbErr );
    }
    return(KerbErr);
}


//+-------------------------------------------------------------------------

KERBERR NTAPI
KerbUnpackKdcReplyBody(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_ENCRYPTED_DATA EncryptedReplyBody,
    IN PKERB_ENCRYPTION_KEY Key,
    IN ULONG Pdu,
    OUT PKERB_ENCRYPTED_KDC_REPLY * ReplyBody
    )
{
    KERBERR KerbErr = KDC_ERR_NONE;
    PUCHAR MarshalledReply = NULL;
    ULONG ReplySize;

    *ReplyBody = NULL;
    MarshalledReply = (PUCHAR) pScratchMemoryAllocator->Alloc(EncryptedReplyBody->cipher_text.length);

    if (MarshalledReply == NULL)
    {
        return(KRB_ERR_GENERIC);
    }

    //
    // First decrypt the buffer
    //

    ReplySize = EncryptedReplyBody->cipher_text.length;
    KerbErr = KerbDecryptDataEx(
                pScratchMemoryAllocator,
                EncryptedReplyBody,
                Key,
                (Pdu == KERB_AS_REPLY_PDU) ? KERB_AS_REP_SALT : KERB_TGS_REP_SALT,
                &ReplySize,
                MarshalledReply
                );

    if (!KERB_SUCCESS(KerbErr))
    {
        goto Cleanup;
    }

    KerbErr = KerbUnpackData(
                pScratchMemoryAllocator,
                MarshalledReply,
                ReplySize,
                Pdu,
                (PVOID *) ReplyBody
                );

    if (!KERB_SUCCESS(KerbErr))
    {

        //
        // MIT KDCs send back TGS reply bodies instead of AS reply bodies
        // so try TGS here
        //

        if (Pdu == KERB_ENCRYPTED_AS_REPLY_PDU)
        {
            KerbErr = KerbUnpackData(
                        pScratchMemoryAllocator,
                        MarshalledReply,
                        ReplySize,
                        KERB_ENCRYPTED_TGS_REPLY_PDU,
                        (PVOID *) ReplyBody
                        );
        }
        if (!KERB_SUCCESS(KerbErr))
        {
            goto Cleanup;
        }

    }
Cleanup:
    if (!KERB_SUCCESS(KerbErr) && (*ReplyBody != NULL))
    {
        *ReplyBody = NULL;
    }
    return(KerbErr);
}

//+-------------------------------------------------------------------------

KERBERR
KerbCreateApRequest(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN LPSTR ClientName,
    IN LPSTR ClientRealm,
    IN PKERB_ENCRYPTION_KEY SessionKey,
    IN ULONG Nonce,
    IN PTimeStamp pAuthenticatorTime,
    IN PKERB_TICKET ServiceTicket,
    IN ULONG ApOptions,
    IN OPTIONAL PKERB_CHECKSUM Checksum,
    IN BOOLEAN KdcRequest,
    OUT PULONG RequestSize,
    OUT PUCHAR * Request
    )
{
    KERBERR KerbErr = KDC_ERR_NONE;
    KERB_AP_REQUEST ApRequest;
    ULONG ApFlags;
    void* pTempBuffer;
    BOOL fDestinationBufferUnspecified = (*Request == NULL);
    
    RtlZeroMemory(
        &ApRequest,
        sizeof(KERB_AP_REQUEST)
        );

    //
    // Fill in the AP request structure.
    //

    ApRequest.version = KERBEROS_VERSION;
    ApRequest.message_type = KRB_AP_REQ;
    ApFlags = KerbConvertUlongToFlagUlong(ApOptions);
    ApRequest.ap_options.value = (PUCHAR) &ApFlags;
    ApRequest.ap_options.length = sizeof(ULONG) * 8;
    ApRequest.ticket = *ServiceTicket;

    {
        CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( pScratchMemoryAllocator );
        
        //
        // Create the authenticator for the request
        //

        KerbErr = KerbCreateAuthenticator(
                    pScratchMemoryAllocator,
                    SessionKey,
                    SessionKey->keytype,
                    Nonce,
                    pAuthenticatorTime,
                    ClientName,
                    ClientRealm,
                    Checksum,
                    KdcRequest,
                    &ApRequest.authenticator
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            goto Cleanup;
        }

        //
        // Now marshall the request
        //
        KerbErr = KerbPackApRequest(
                    pScratchMemoryAllocator,
                    &ApRequest,
                    RequestSize,
                    Request
                    );
        if (!KERB_SUCCESS(KerbErr))
        {
            goto Cleanup;
        }

        if (fDestinationBufferUnspecified)
        {
            //
            // Preserve the only piece of memory we care about
            //
            pTempBuffer = PreserveThisMemory( &onlyPreserveThisMemory, Request, *RequestSize );
            if (pTempBuffer == NULL)
            {
                KerbErr = STATUS_INSUFFICIENT_RESOURCES;
                goto Cleanup;
            }
        }
    }
    
Cleanup:
    if (!KERB_SUCCESS(KerbErr))
    {
        TraceSz1( KerbWarn, "KerbCreateApRequest failure KerbErr 0x%X", KerbErr );
    }
    return(KerbErr);
}

//+-------------------------------------------------------------------------

KERBERR
KerbInitAsn(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN OUT ASN1encoding_t * pEnc,
    IN OUT ASN1decoding_t * pDec
    )
{
    KERBERR KerbErr = KRB_ERR_GENERIC;
    ASN1module_t pASN1Module = pScratchMemoryAllocator->GetXoPtr()->GetASN1Module();
    ASN1error_e Asn1Err;

    if (pEnc != NULL)
    {
            Asn1Err = ASN1_CreateEncoder(
                                     pScratchMemoryAllocator,
                                     pASN1Module,
                                     pEnc,
                                     NULL,           // pbBuf
                                     0,              // cbBufSize
                                     NULL            // pParent
                                     );
    }
    else
    {
            Asn1Err = ASN1_CreateDecoder(
                                     pScratchMemoryAllocator,
                                     pASN1Module,
                                     pDec,
                                     NULL,           // pbBuf
                                     0,              // cbBufSize
                                     NULL            // pParent
                                     );
    }

    if (ASN1_SUCCESS != Asn1Err)
    {
            goto Cleanup;
    }

    KerbErr = KDC_ERR_NONE;

Cleanup:
    if (!KERB_SUCCESS(KerbErr))
    {
        TraceSz1( KerbWarn, "KerbInitAsn failure KerbErr 0x%X", KerbErr );
    }

    return(KerbErr);
}


//+-------------------------------------------------------------------------

VOID
KerbTermAsn(
        IN ASN1encoding_t pEnc,
        IN ASN1decoding_t pDec
    )
{
    if (pEnc != NULL)
    {
            ASN1_CloseEncoder(pEnc);
    }
    else if (pDec != NULL)
    {
            ASN1_CloseDecoder(pDec);
    }
}

//+-------------------------------------------------------------------------

KERBERR NTAPI
KerbPackData(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PVOID Data,
    IN ULONG PduValue,
    OUT PULONG DataSize,
    OUT PUCHAR * MarshalledData
    )
{
    KERBERR KerbErr = KDC_ERR_NONE;
    int Result = 0;
    PUCHAR Buffer = NULL;
    ASN1encoding_t pEnc = NULL;
    ASN1error_e Asn1Err;
    void* pTempBuffer;

    KerbErr = KerbInitAsn(
                pScratchMemoryAllocator,
                &pEnc,          // we are encoding
                NULL
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        goto Cleanup;
    }

    {
        CReleaseAllScratchMemoryUponDestruction onlyPreserveThisMemory( pScratchMemoryAllocator );
        //
        // Encode the data type.
        //

        Assert( (*MarshalledData == NULL && *DataSize == 0) || (*MarshalledData != NULL && *DataSize != 0) );
        AssertSz( *DataSize <= 10000, "Passing buffer larger than 10K, most likely an error");
                
        TraceSz1( KerbASN1, "ASN encoding pdu #%d", PduValue );
        if (*MarshalledData != NULL)
        {
            Asn1Err = ASN1_Encode(
                        pEnc,
                        Data,
                        PduValue,
                        ASN1ENCODE_SETBUFFER,
                        *MarshalledData,         
                        *DataSize                        
                        );
        }
        else
        {
            Asn1Err = ASN1_Encode(
                        pEnc,
                        Data,
                        PduValue,
                        ASN1ENCODE_ALLOCATEBUFFER,
                        NULL,                       // pbBuf
                        0                           // cbBufSize
                        );
        }
        if (!ASN1_SUCCEEDED(Asn1Err))
        {
            KerbErr = KRB_ERR_GENERIC;
            goto Cleanup;
        }
        else
        {
            if ( *MarshalledData != NULL )
            {
                //
                // Caller specified a destination buffer
                //
                if (*MarshalledData == pEnc->buf)
                {
                    //
                    // Caller's destination buffer was used, that's what we want
                    //
                    *DataSize = pEnc->len;
                }
                else
                {
                    //
                    // Caller's destination buffer was too small, report failure
                    //
                    *DataSize = 0;
                    *MarshalledData = NULL;
                    KerbErr = STATUS_INSUFFICIENT_RESOURCES;
                    goto Cleanup;
                }
            }
            else
            {
                //
                // Caller didn't specify a destination buffer
                //
                *DataSize = pEnc->len;
                *MarshalledData = pEnc->buf;
                //
                // Preserve the only piece of memory we care about
                //
                pTempBuffer = PreserveThisMemory( &onlyPreserveThisMemory, MarshalledData, *DataSize );
                if (pTempBuffer == NULL)
                {
                    KerbErr = KRB_ERR_GENERIC;
                    goto Cleanup;
                }
            }
        }

        KerbTermAsn(pEnc, NULL);
    }

Cleanup:
    if (!KERB_SUCCESS(KerbErr))
    {
        TraceSz1( KerbWarn, "KerbPackData failure KerbErr 0x%X", KerbErr );
    }
    return(KerbErr);
}


//+-------------------------------------------------------------------------

KERBERR NTAPI
KerbUnpackData(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PUCHAR Data,
    IN ULONG DataSize,
    IN ULONG PduValue,
    OUT PVOID * DecodedData
    )
{
    KERBERR KerbErr = KDC_ERR_NONE;
    ASN1decoding_t pDec = NULL;
        ASN1error_e Asn1Err;

    Assert((DataSize != 0) && (Data != NULL));

    KerbErr = KerbInitAsn(
                pScratchMemoryAllocator,
                NULL,
                &pDec           // we are decoding
                );
    if (!KERB_SUCCESS(KerbErr))
    {
        goto Cleanup;
    }

    TraceSz1( KerbASN1, "ASN decoding pdu #%d", PduValue );
    *DecodedData = NULL;
    Asn1Err = ASN1_Decode(
                pDec,
                DecodedData,
                PduValue,
                ASN1DECODE_SETBUFFER,
                (BYTE *) Data,
                DataSize
                );

    if (!ASN1_SUCCEEDED(Asn1Err))
    {

        if ((ASN1_ERR_BADARGS == Asn1Err) ||
            (ASN1_ERR_EOD == Asn1Err))
        {
            KerbErr = KDC_ERR_MORE_DATA;
        }
        else
        {
            KerbErr = KRB_ERR_GENERIC;
        }
        *DecodedData = NULL;
    }

    KerbTermAsn(NULL, pDec);

Cleanup:
    return(KerbErr);
}

//+-------------------------------------------------------------------------

NTSTATUS
KerbMapKerbError(
    IN KERBERR KerbError
    )
{
    NTSTATUS Status;
    switch(KerbError) {
    case KDC_ERR_NONE:
        Status = STATUS_SUCCESS;
        break;
    case KDC_ERR_CLIENT_REVOKED:
        Status = STATUS_ACCOUNT_DISABLED;
        break;
    case KDC_ERR_KEY_EXPIRED:
        Status = STATUS_PASSWORD_EXPIRED;
        break;
    case KRB_ERR_GENERIC:
        Status = STATUS_INSUFFICIENT_RESOURCES;
        break;
    case KRB_AP_ERR_SKEW:
        Status = STATUS_TIME_DIFFERENCE_AT_DC;
        break;
    case KDC_ERR_POLICY:
        Status = STATUS_ACCOUNT_RESTRICTION;
        break;
    case KDC_ERR_C_PRINCIPAL_UNKNOWN:
        Status = STATUS_NO_SUCH_USER;
        break;
    case KDC_ERR_S_PRINCIPAL_UNKNOWN:
        Status = STATUS_NO_TRUST_SAM_ACCOUNT;
        break;
    case KRB_AP_ERR_MODIFIED:
    case KDC_ERR_PREAUTH_FAILED:
        //
        // BUGBUG: use wronge_server_password
        //

        Status = STATUS_WRONG_PASSWORD;
        break;
    case KRB_ERR_RESPONSE_TOO_BIG:
        Status = STATUS_INVALID_BUFFER_SIZE;
        break;
    case KDC_ERR_PADATA_TYPE_NOSUPP:
        Status = STATUS_NOT_SUPPORTED;
        break;
    case KRB_AP_ERR_NOT_US:
        Status = SEC_E_WRONG_PRINCIPAL;
        break;

    case KDC_ERR_SVC_UNAVAILABLE:
        Status = STATUS_NO_LOGON_SERVERS;
        break;
    case KDC_ERR_WRONG_REALM:
        Status = STATUS_NO_SUCH_USER;
        break;
    case KDC_ERR_INVALID_CERTIFICATE:
        Status = STATUS_INVALID_PARAMETER;
        break;
    case KDC_ERR_REVOKED_CERTIFICATE:
        Status = CRYPT_E_REVOKED;
        break;
    case KDC_ERR_REVOCATION_STATUS_UNKNOWN:
        Status = CRYPT_E_NO_REVOCATION_CHECK;
        break;
    case KDC_ERR_REVOCATION_STATUS_UNAVAILABLE:
        Status = CRYPT_E_REVOCATION_OFFLINE;
        break;
    case KDC_ERR_CLIENT_NAME_MISMATCH:
        //
        // BUGBUG: wrong error
        //
        Status = STATUS_OBJECT_TYPE_MISMATCH;
        break;
    case KDC_ERR_KDC_NAME_MISMATCH:
        //
        // BUGBUG: wrong error
        //
        Status = STATUS_OBJECT_TYPE_MISMATCH;
        break;

    default:
        Status = STATUS_LOGON_FAILURE;
    }
    return(Status);

}

//+-------------------------------------------------------------------------

ULONG
KerbConvertUlongToFlagUlong(
    IN ULONG Flag
    )
{
    ULONG ReturnFlag;

    ((PUCHAR) &ReturnFlag)[0] = ((PUCHAR) &Flag)[3];
    ((PUCHAR) &ReturnFlag)[1] = ((PUCHAR) &Flag)[2];
    ((PUCHAR) &ReturnFlag)[2] = ((PUCHAR) &Flag)[1];
    ((PUCHAR) &ReturnFlag)[3] = ((PUCHAR) &Flag)[0];

    return(ReturnFlag);
}

#define MAXTIMEQUADPART (LONGLONG)0x7FFFFF36D5969FFF
#define MAXTIMEHIGHPART 0x7FFFFF36
#define MAXTIMELOWPART  0xD5969FFF

TimeStamp g_KerbGlobalWillNeverTime = { MAXTIMELOWPART, MAXTIMEHIGHPART };

//+-------------------------------------------------------------------------

VOID
KerbGetCurrentTime(
    OUT PTimeStamp pCurrentTime
    )
{
    GetSystemTimeAsFileTime((PFILETIME) pCurrentTime);
    
    return;
}
    
//+-------------------------------------------------------------------------

VOID
KerbUpdateCurrentTime(
    IN TimeStamp NewCurrentTime
    )
{
#ifdef XONLINE_FEATURE_WINDOWS
    SYSTEMTIME SystemTime;
    BOOL f = FileTimeToSystemTime( (FILETIME*)&NewCurrentTime, &SystemTime );
    Assert( f );
    f = SetSystemTime( &SystemTime );
    Assert( f );
#else
    NTSTATUS status;
    status = NtSetSystemTime( (PLARGE_INTEGER)&NewCurrentTime, NULL );
    Assert( status == STATUS_SUCCESS );
#endif
 
    TraceSz( KerbWarn,"KerbUpdateCurrentTime: Clock was corrected based on KDC reply" );
 
    return;
}

//+---------------------------------------------------------------------------

KERBERR NTAPI
KerbEncryptDataEx(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    OUT PKERB_ENCRYPTED_DATA EncryptedData,
    IN ULONG DataSize,
    IN PUCHAR Data,
    IN ULONG Algorithm,
    IN ULONG UsageFlags,
    IN PKERB_ENCRYPTION_KEY Key
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    KERBERR KerbErr = KDC_ERR_NONE;

    //
    // Initialize header
    //

    EncryptedData->encryption_type = KERB_ETYPE_RC4_HMAC_NT;

    Status =  rc4HmacEncrypt(
                pScratchMemoryAllocator,
                (PUCHAR) Key->keyvalue.value,
                Key->keyvalue.length,
                UsageFlags,
                Data,
                DataSize,
                EncryptedData->cipher_text.value,
                &EncryptedData->cipher_text.length
                );

    if (!NT_SUCCESS(Status))
    {
        KerbErr = KRB_ERR_GENERIC;
        goto Cleanup;
    }

Cleanup:
    if (!KERB_SUCCESS(KerbErr))
    {
        TraceSz1( KerbWarn, "KerbEncryptDataEx failure KerbErr 0x%X", KerbErr );
    }
    return KerbErr;
}

//+---------------------------------------------------------------------------

KERBERR NTAPI
KerbDecryptDataEx(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PKERB_ENCRYPTED_DATA EncryptedData,
    IN PKERB_ENCRYPTION_KEY pkKey,
    IN ULONG UsageFlags,
    OUT PULONG DataSize,
    OUT PUCHAR Data
    )
{
    NTSTATUS    Status  = STATUS_SUCCESS;
    KERBERR     KerbErr = KDC_ERR_NONE;

    Status = rc4HmacDecrypt(
                (PUCHAR) pkKey->keyvalue.value,
                pkKey->keyvalue.length,
                UsageFlags,
                EncryptedData->cipher_text.value,
                EncryptedData->cipher_text.length,
                Data,
                DataSize
                );

    if (!NT_SUCCESS(Status))
    {
        KerbErr = KRB_AP_ERR_MODIFIED;
        goto Cleanup;
    }

Cleanup:
    return KerbErr;
}

//------------------------------------------------------------------------

KERBERR
KerbConstructPrincipalName(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    OUT PKERB_PRINCIPAL_NAME PrincipalName,
    IN ULONG NameType,
    IN LPSTR pComp1,
    IN LPSTR pComp2
    )
{
    PKERB_PRINCIPAL_NAME_ELEM Elem1,Elem2;
    KERBERR Status = KDC_ERR_NONE;

    PrincipalName->name_type = (int) NameType;

    Assert( pComp1 != NULL );
    
    Elem1 = (PKERB_PRINCIPAL_NAME_ELEM) pScratchMemoryAllocator->Alloc(sizeof(KERB_PRINCIPAL_NAME_ELEM));
    if (Elem1 == NULL)
    {
        Status = KRB_ERR_GENERIC;
        goto Cleanup;
    }
    Elem1->value = pComp1;
    Elem1->next = NULL;
    PrincipalName->name_string = Elem1;

    if ( pComp2 != NULL )
    {
        Elem2 = (PKERB_PRINCIPAL_NAME_ELEM) pScratchMemoryAllocator->Alloc(sizeof(KERB_PRINCIPAL_NAME_ELEM));
        if (Elem2 == NULL)
        {
            Status = KRB_ERR_GENERIC;
            goto Cleanup;
        }
        Elem2->value = pComp2;
        Elem2->next = NULL;
        Elem1->next = Elem2;
    }
    
Cleanup:
    return(Status);
}

//------------------------------------------------------------------------

KERBERR
KerbMultiStringToPrincipalName(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN LPSTR pMultiString,
    OUT PKERB_PRINCIPAL_NAME PrincipalName
    )
{
    PKERB_PRINCIPAL_NAME_ELEM Elem1,Elem2;
    KERBERR Status = KDC_ERR_NONE;
    LPSTR pNextComponent;
    
    RtlZeroMemory( PrincipalName, sizeof(KERB_PRINCIPAL_NAME) );

    PrincipalName->name_type = atoi(pMultiString);

    pNextComponent = pMultiString + strlen(pMultiString) + 1;

    Elem1 = NULL;
    
    while (*pNextComponent != '\0')
    {
        Elem2 = (PKERB_PRINCIPAL_NAME_ELEM) pScratchMemoryAllocator->Alloc(sizeof(KERB_PRINCIPAL_NAME_ELEM));
        if (Elem2 == NULL)
        {
            Status = KRB_ERR_GENERIC;
            goto Cleanup;
        }
        Elem2->value = pNextComponent;
        Elem2->next = NULL;
        if (Elem1 != NULL)
        {
            Elem1->next = Elem2;
        }
        else
        {
            PrincipalName->name_string = Elem2;
        }
        Elem1 = Elem2;
        pNextComponent += strlen(pNextComponent) + 1;        
    }

Cleanup:
    return(Status);
}

//------------------------------------------------------------------------

KERBERR
KerbPrincipalNameToMultiString(
    IN PKERB_PRINCIPAL_NAME PrincipalName,
    IN DWORD dwMultiStringMaxLength,
    OUT LPSTR pMultiString
    )
{
    PKERB_PRINCIPAL_NAME_ELEM Elem;
    KERBERR Status = KDC_ERR_NONE;
    DWORD dwNextComponentLength;
    LPSTR pNextComponent;

    Assert( dwMultiStringMaxLength >= 32 );

    sprintf( pMultiString, "%d", PrincipalName->name_type );

    pNextComponent = pMultiString + strlen(pMultiString) + 1;

    //
    // pMultiString is now pointing at the end of buffer
    //
    pMultiString += dwMultiStringMaxLength;
    
    Elem = PrincipalName->name_string;
    
    while (Elem != NULL)
    {
        dwNextComponentLength = strlen( Elem->value ) + 1;
        if (pNextComponent + dwNextComponentLength + 1 > pMultiString)
        {
            Status = KRB_ERR_GENERIC;
            goto Cleanup;
        }

        memcpy(pNextComponent, Elem->value, dwNextComponentLength);
        pNextComponent += dwNextComponentLength;
        
        Elem = Elem->next;
    }
    *pNextComponent = '\0';

Cleanup:
    return(Status);
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN
md5Hmac(
    IN PBYTE pbKeyMaterial,
    IN ULONG cbKeyMaterial,
    IN PBYTE pbData,
    IN ULONG cbData,
    IN PBYTE pbData2,
    IN ULONG cbData2,
    OUT PBYTE HmacData
    )
{
    BOOLEAN fRet = FALSE;
#define HMAC_K_PADSIZE              64
    BYTE Kipad[HMAC_K_PADSIZE];
    BYTE Kopad[HMAC_K_PADSIZE];
    BYTE HMACTmp[HMAC_K_PADSIZE+MD5_LEN];
    ULONG dwBlock;
    msr_MD5_CTX Md5Hash;

    // truncate
    if (cbKeyMaterial > HMAC_K_PADSIZE)
        cbKeyMaterial = HMAC_K_PADSIZE;


    RtlZeroMemory(Kipad, HMAC_K_PADSIZE);
    RtlCopyMemory(Kipad, pbKeyMaterial, cbKeyMaterial);

    RtlZeroMemory(Kopad, HMAC_K_PADSIZE);
    RtlCopyMemory(Kopad, pbKeyMaterial, cbKeyMaterial);


    //
    // Kipad, Kopad are padded sMacKey. Now XOR across...
    //

    for(dwBlock=0; dwBlock<HMAC_K_PADSIZE/sizeof(DWORD); dwBlock++)
    {
        ((DWORD*)Kipad)[dwBlock] ^= 0x36363636;
        ((DWORD*)Kopad)[dwBlock] ^= 0x5C5C5C5C;
    }


    //
    // prepend Kipad to data, Hash to get H1
    //

    msr_MD5Init(&Md5Hash);
    msr_MD5Update(&Md5Hash, Kipad, HMAC_K_PADSIZE);
    if (cbData != 0)
    {
        msr_MD5Update(&Md5Hash, pbData, cbData);
    }
    if (cbData2 != 0)
    {
        msr_MD5Update(&Md5Hash, pbData2, cbData2);
    }

    // Finish off the hash
    msr_MD5Final(&Md5Hash);

    // prepend Kopad to H1, hash to get HMAC
    RtlCopyMemory(HMACTmp, Kopad, HMAC_K_PADSIZE);
    RtlCopyMemory(HMACTmp+HMAC_K_PADSIZE, Md5Hash.digest, MD5_LEN);

    // final hash: output value into passed-in buffer
    msr_MD5Init(&Md5Hash);
    msr_MD5Update(&Md5Hash,HMACTmp, sizeof(HMACTmp));
    msr_MD5Final(&Md5Hash);
    RtlCopyMemory(
        HmacData,
        Md5Hash.digest,
        MD5_LEN
        );

    return TRUE;
}

NTSTATUS NTAPI
rc4HmacEncrypt(
    IN CScratchMemoryAllocator* pScratchMemoryAllocator,
    IN PUCHAR pbKey,
    IN ULONG KeySize,
    IN ULONG MessageType,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    OUT PUCHAR pbOutput,
    OUT PULONG pcbOutput
    )
{
    PRC4_MDx_HEADER CryptHeader = (PRC4_MDx_HEADER) pbOutput;
    BYTE StateKey[MD5_LEN];
    BYTE LocalKey[MD5_LEN];
    ULONG Offset = 0;
    BYTE Rc4KeyStruct[XC_SERVICE_RC4_KEYSTRUCT_SIZE];

    md5Hmac(
        pbKey,
        KeySize,
        (PBYTE) &MessageType,
        sizeof(ULONG),
        NULL,
        0,
        StateKey
        );

    Offset = sizeof(RC4_MDx_HEADER);
    RtlMoveMemory(
        pbOutput + Offset,
        pbInput,
        cbInput
        );
    *pcbOutput = cbInput + Offset;

    //
    // Create the header - the confounder & checksum
    //

    RtlZeroMemory(
        CryptHeader->Checksum,
        MD5_LEN
        );

    pScratchMemoryAllocator->GetXoPtr()->XNetRandom(CryptHeader->Confounder, RC4_CONFOUNDER_LEN);

    //
    // Checksum everything but the checksum
    //

    md5Hmac(
        StateKey,
        MD5_LEN,
        pbOutput+MD5_LEN,
        *pcbOutput-MD5_LEN,
        NULL,
        0,
        CryptHeader->Checksum
        );


    //
    // HMAC the checksum into the key
    //

    md5Hmac(
        StateKey,
        MD5_LEN,
        CryptHeader->Checksum,
        MD5_LEN,
        NULL,
        0,
        LocalKey
        );

    XcRC4Key( Rc4KeyStruct, MD5_LEN, LocalKey );

    //
    // Encrypt everything but the checksum
    //

    XcRC4Crypt( Rc4KeyStruct, *pcbOutput-MD5_LEN, pbOutput+MD5_LEN );

    return(S_OK);
}

NTSTATUS
rc4HmacDecrypt(
    IN PUCHAR pbKey,
    IN ULONG KeySize,
    IN ULONG MessageType,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    OUT PUCHAR pbOutput,
    OUT PULONG pcbOutput)
{
    RC4_MDx_HEADER TempHeader;
    BYTE StateKey[MD5_LEN];
    BYTE TempChecksum[MD5_LEN];
    ULONG Offset = sizeof(RC4_MDx_HEADER);
    BYTE LocalKey[MD5_LEN];
    BYTE Rc4KeyStruct[XC_SERVICE_RC4_KEYSTRUCT_SIZE];


    if (cbInput < Offset)
    {
        return(STATUS_INVALID_PARAMETER);
    }

    md5Hmac(
        pbKey,
        KeySize,
        (PBYTE) &MessageType,
        sizeof(ULONG),
        NULL,
        0,
        StateKey
        );

    //
    // Copy the input to the output before decrypting
    //

    RtlCopyMemory(
        &TempHeader,
        pbInput,
        Offset
        );

    *pcbOutput = cbInput - Offset;
    RtlMoveMemory(
        pbOutput,
        pbInput + Offset,
        *pcbOutput
        );


    //
    // Build the decryption key from the checksum and the
    // real key
    //

    md5Hmac(
        StateKey,
        MD5_LEN,
        TempHeader.Checksum,
        MD5_LEN,
        NULL,
        0,
        LocalKey
        );

    XcRC4Key( Rc4KeyStruct, MD5_LEN, LocalKey );

    //
    // Now decrypt the two buffers
    //

    XcRC4Crypt( Rc4KeyStruct, Offset - MD5_LEN, TempHeader.Confounder );
    XcRC4Crypt( Rc4KeyStruct, *pcbOutput, pbOutput );

    //
    // Now verify the checksum. First copy it out of the way, zero the
    // header
    //


    md5Hmac(
        StateKey,
        MD5_LEN,
        TempHeader.Confounder,
        Offset-MD5_LEN,
        pbOutput,
        *pcbOutput,
        TempChecksum
        );

    if (RtlEqualMemory(
            TempHeader.Checksum,
            TempChecksum,
            MD5_LEN
            ) != TRUE)
    {
        return(STATUS_UNSUCCESSFUL);
    }

    return(S_OK);
}
