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

/* versions */

/* spec codes */

/* keyexchange algs */
#define SP_EXCH_RSA_PKCS1		        0x0001
#define SP_EXCH_RSA_PKCS1_TOKEN_DES     0x0002
#define SP_EXCH_RSA_PKCS1_TOKEN_DES3    0x0003
#define SP_EXCH_RSA_PKCS1_TOKEN_RC2     0x0004
#define SP_EXCH_RSA_PKCS1_TOKEN_RC4     0x0005

#define SP_EXCH_DH_PKCS3                0x0006
#define SP_EXCH_DH_PKCS3_TOKEN_DES      0x0007
#define SP_EXCH_DH_PKCS3_TOKEN_DES3     0x0008
#define SP_EXCH_FORTEZZA_TOKEN          0x0009

#define SP_EXCH_UNKNOWN                 0xffff


/* data encryption algs */
/* encryption alg masks */
#define SP_CIPHER_ALG			        0xffff0000
#define SP_CIPHER_STRENGTH		        0x0000ff00
#define SP_CSTR_POS			            8
#define SP_CIPHER_MAC			        0x000000ff

/* specific algs */
#define SP_CIPHER_DES			        0x00010000
#define SP_CIPHER_IDEA			        0x00020000
#define SP_CIPHER_RC2			        0x00030000
#define SP_CIPHER_RC4			        0x00040000
#define SP_CIPHER_DES_112               0x00050000
#define SP_CIPHER_DES_168               0x00060000
#define SP_CIPHER_NONE                  0x00000000

#define SP_CIPHER_UNKNOWN               0xffffffff

/* data encryption strength specs */
#define SP_ENC_BITS_40			        0x00002800
#define SP_ENC_BITS_64			        0x00004000
#define SP_ENC_BITS_128		            0x00008000

/* mac strength specs */
#define SP_MAC_BITS_128		            0x00000040

/* hashing algs */
#define SP_HASH_MD5			            0x0001
#define SP_HASH_MD5_TRUNC_64	        0x0002
#define SP_HASH_SHA                     0x0003
#define SP_HASH_SHA_TRUNC_80            0x0004
#define SP_HASH_DES_DM                  0x0005
#define SP_HASH_MD2			            0x0006
#define SP_HASH_UNKNOWN                 0xffff

/* certificate types */
#define SP_CERT_NONE			        0x0000
#define SP_CERT_X509			        0x0001
#define SP_CERT_PKCS7			        0x0002

/* signature algorithms */
#define SP_SIG_NONE			            0x0000
#define SP_SIG_RSA_MD5		            0x0001
#define SP_SIG_RSA_SHA                  0x0002
#define SP_SIG_DSA_SHA                  0x0003

/* these are for internal use only */
#define SP_SIG_RSA_MD2			        0x0004
#define SP_SIG_RSA  			        0x0005
#define SP_SIG_RSA_SHAMD5               0x0006

/* flag/identifiers for protocols we support */
#define SP_PROT_PCT1_SERVER             0x00000001
#define SP_PROT_PCT1_CLIENT             0x00000002
#define SP_PROT_PCT1                    (SP_PROT_PCT1_SERVER | SP_PROT_PCT1_CLIENT)

#define SP_PROT_SSL2_SERVER             0x00000004
#define SP_PROT_SSL2_CLIENT             0x00000008
#define SP_PROT_SSL2                    (SP_PROT_SSL2_SERVER | SP_PROT_SSL2_CLIENT)

#define SP_PROT_SSL3_SERVER             0x00000010
#define SP_PROT_SSL3_CLIENT             0x00000020
#define SP_PROT_SSL3                    (SP_PROT_SSL3_SERVER | SP_PROT_SSL3_CLIENT)

#define SP_PROT_UNI_SERVER              0x40000000
#define SP_PROT_UNI_CLIENT              0x80000000
#define SP_PROT_UNI                     (SP_PROT_UNI_SERVER | SP_PROT_UNI_CLIENT)


#define SP_PROT_CRED                    0x00010000

// flag to identify ciphers allowed with Server-Gated-Crypt
// use along with SP_PROT_SSL3
#define SP_PROT_SGC                     0x00020000

#define SP_PROT_ALL                     0xffffffff
#undef SP_PROT_CLIENTS
#define SP_PROT_CLIENTS                 (SP_PROT_PCT1_CLIENT | SP_PROT_SSL2_CLIENT | SP_PROT_SSL3_CLIENT | SP_PROT_UNI_CLIENT )
#undef SP_PROT_SERVERS
#define SP_PROT_SERVERS                 (SP_PROT_PCT1_SERVER | SP_PROT_SSL2_SERVER | SP_PROT_SSL3_SERVER | SP_PROT_UNI_SERVER )


/* sizing of local structures */
#define SP_MAX_SESSION_ID           32
#define SP_MAX_MASTER_KEY           48
#define SP_MAX_MAC_KEY              48
#define SP_MAX_CACHE_ID             64
#define SP_MAX_CHALLENGE            32
#define SP_MAX_CONNECTION_ID        32
#define SP_MAX_KEY_ARGS             32

#define SP_OFFSET_OF(t, v) (DWORD)&(((t)NULL)->v)
/* tuning constants */

#define SP_DEF_SERVER_CACHE_SIZE		10
#define SP_DEF_CLIENT_CACHE_SIZE		10


typedef DWORD SP_STATUS;


/* internal representations of algorithm specs */

typedef DWORD   CipherSpec, *PCipherSpec;
typedef DWORD   KeyExchangeSpec, *PKeyExchangeSpec;
typedef DWORD   HashSpec,   *PHashSpec;
typedef DWORD   CertSpec,   *PCertSpec;
typedef DWORD   ExchSpec,   *PExchSpec;
typedef DWORD   SigSpec,    *PSigSpec;



typedef struct _SPBuffer {
    unsigned long cbBuffer;             /* Size of the buffer, in bytes */
    unsigned long cbData;               /* size of the actual data in the
                                         * buffer, in bytes */
    void * pvBuffer;                    /* Pointer to the buffer */
} SPBuffer, * PSPBuffer;
