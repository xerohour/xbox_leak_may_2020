/////////////////////////////////////////////////////////////////////////////
//  FILE          : nt_key.c                                               //
//  DESCRIPTION   : Crypto CP interfaces:                                  //
//                  CPGenKey                                               //
//                  CPDeriveKey                                            //
//                  CPExportKey                                            //
//                  CPImportKey                                            //
//                  CPDestroyKey                                           //
//                  CPGetUserKey                                           //
//                  CPSetKeyParam                                          //
//                  CPGetKeyParam                                          //
//  AUTHOR        :                                                        //
//  HISTORY       :                                                        //
//  Jan 25 1995 larrys  Changed from Nametag                               //
//  Feb 16 1995 larrys  Fix problem for 944 build                          //
//  Feb 21 1995 larrys  Added SPECIAL_KEY                                  //
//  Feb 23 1995 larrys  Changed NTag_SetLastError to SetLastError          //
//  Mar 08 1995 larrys  Fixed a few problems                               //
//  Mar 23 1995 larrys  Added variable key length                          //
//  Apr  7 1995 larrys  Removed CryptConfigure                             //
//  Apr 17 1995 larrys  Added 1024 key gen                                 //
//  Apr 19 1995 larrys  Changed CRYPT_EXCH_PUB to AT_KEYEXCHANGE           //
//  May 10 1995 larrys  added private api calls                            //
//  May 17 1995 larrys  added key data for DES test                        //
//  Jul 20 1995 larrys  Changed export of PUBLICKEYBLOB                    //
//  Jul 21 1995 larrys  Fixed Export of AUTHENTICATEDBLOB                  //
//  Aug 03 1995 larrys  Allow CryptG(S)etKeyParam for Public keys &        //
//                      Removed CPTranslate                                //
//  Aug 10 1995 larrys  Fixed a few problems in CryptGetKeyParam           //
//  Aug 11 1995 larrys  Return no key for CryptGetUserKey                  //
//  Aug 14 1995 larrys  Removed key exchange stuff                         //
//  Aug 17 1995 larrys  Removed a error                                    //
//  Aug 18 1995 larrys  Changed NTE_BAD_LEN to ERROR_MORE_DATA             //
//  Aug 30 1995 larrys  Removed RETURNASHVALUE from CryptGetHashValue      //
//  Aug 31 1995 larrys  Fixed CryptExportKey if pbData == NULL             //
//  Sep 05 1995 larrys  Fixed bug # 30                                     //
//  Sep 05 1995 larrys  Fixed bug # 31                                     //
//  Sep 11 1995 larrys  Fixed bug # 34                                     //
//  Sep 12 1995 larrys  Removed 2 DWORDS from exported keys                //
//  Sep 14 1995 Jeffspel/ramas  Merged STT onto CSP                        //
//  Sep 18 1995 larrys  Changed def KP_PERMISSIONS to 0xffffffff           //
//  Oct 02 1995 larrys  Fixed bug 43 return error for importkey on hPubkey //
//  Oct 03 1995 larrys  Fixed bug 37 call InflateKey from SetKeyParam      //
//  Oct 03 1995 larrys  Fixed bug 36, removed OFB from SetKeyParam         //
//  Oct 03 1995 larrys  Fixed bug 38, check key type in SetKeyParam        //
//  Oct 13 1995 larrys  Added CPG/setProv/HashParam                        //
//  Oct 13 1995 larrys  Added code for CryptSetHashValue                   //
//  Oct 16 1995 larrys  Changes for CryptGetHashParam                      //
//  Oct 23 1995 larrys  Added code for GetProvParam PP_CONTAINER           //
//  Oct 27 1995 rajeshk RandSeed Stuff added hUID to PKCS2Encrypt+ others  //
//  Nov  3 1995 larrys  Merge changes for NT checkin                       //
//  Nov  9 1995 larrys  Bug fix 10686                                      //
//  Nov 30 1995 larrys  Bug fix                                            //
//  Dec 11 1995 larrys  Added WIN96 password cache                         //
//  Feb 29 1996 rajeshk Added Check for SetHashParam for HASHVALUE         //
//  May 15 1996 larrys  Added private key export                           //
//  May 28 1996 larrys  Fix bug 88                                         //
//  Jun  6 1996 a-johnb Added support for SSL 3.0 signatures               //
//  Aug 28 1996 mattt   Changed enum to calculate size from #defined sizes //
//  Sep 13 1996 mattt   Compat w/RSABase 88-bit 0 salt, FIsLegalKey()      //
//  Sep 16 1996 mattt   Added KP_KEYLEN ability                            //
//  Sep 16 1996 jeffspel Added triple DES functionality                    //
//  Oct 14 1996 jeffspel Changed GenRandoms to NewGenRandoms               //
//  Apr 29 1997 jeffspel Key storage ability GetProvParam, PStore support  //
//  Apr 29 1997 jeffspel Added EnumAlgsEx tp GetProvParam                  //
//  May 23 1997 jeffspel Added provider type checking                      //
//  Jul 15 1997 jeffspel Added ability to decrypt with large RC2 keys      //
//  Jul 28 1997 jeffspel Added ability to delete a persisted key           //
//  Sep 09 1997 jeffspel Added PP_KEYSET_TYPE to CPGetProvParam            //
//  Sep 12 1997 jeffspel Added Opaque blob support                         //
//                                                                         //
//  Copyright (C) 1993 Microsoft Corporation   All Rights Reserved         //
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"
#include "nt_rsa.h"
#include "nt_blobs.h"
#include "swnt_pk.h"
#include "mac.h"
#include "ntagimp1.h"
#include "tripldes.h"
#include "ntagum.h"
#include "randlib.h"
#ifdef CSP_USE_SSL3
#include "ssl3.h"
#endif
#include "protstor.h"
#include "sgccheck.h"

extern CSP_STRINGS g_Strings;

#define     SUPPORTED_PROTOCOLS     \
            CRYPT_FLAG_PCT1 | CRYPT_FLAG_SSL2 | CRYPT_FLAG_SSL3 | CRYPT_FLAG_TLS1

#define SSL3SHAMD5LEN   (MD5DIGESTLEN+A_SHA_DIGEST_LEN)*8

#define UnsupportedSymKey(pKey) ((CALG_RC4 != pKey->Algid) && \
                                 (CALG_RC2 != pKey->Algid) && \
                                 (CALG_DES != pKey->Algid) && \
                                 (CALG_3DES != pKey->Algid) && \
                                 (CALG_3DES_112 != pKey->Algid))

typedef struct enumalgs {
    ALG_ID    aiAlgid;
    DWORD     dwBitLen;
    DWORD     dwNameLen;
    CHAR      szName[20];
} ENUMALGS;

typedef struct enumalgsex {
    ALG_ID    aiAlgid;
    DWORD     dwDefault;
    DWORD     dwMin;
    DWORD     dwMax;
    DWORD     dwProtocols;
    DWORD     dwNameLen;
    CHAR      szName[20];
    DWORD     dwLongNameLen;
    CHAR      szLongName[40];
} ENUMALGSEX;

const ENUMALGS EnumSig[] =
{
// ALGID            BitLen                              NameLen  szName
#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8,                 6,     "SHA-1",
#endif

#ifdef CSP_USE_MD2
   CALG_MD2,        MD2DIGESTLEN*8,                     4,     "MD2",
#endif

#ifdef CSP_USE_MD4
   CALG_MD4,        MD4DIGESTLEN*8,                     4,     "MD4",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8,                     4,     "MD5",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,                      12,    "SSL3 SHAMD5",

   CALG_RSA_SIGN,   RSA_DEF_SIGN_MODLEN*8,              9,     "RSA_SIGN",

   0,               0,                                  0,     0
};

const ENUMALGS EnumFull[] =
{
// ALGID            BitLen                              NameLen  szName
#ifdef CSP_USE_RC2
   CALG_RC2,        RC2_DEF_KEYSIZE*8,                  4,     "RC2",
#endif

#ifdef CSP_USE_RC4
   CALG_RC4,        RC4_DEF_KEYSIZE*8,                  4,     "RC4",
#endif

#ifdef CSP_USE_DES
   CALG_DES,        (DES_KEYSIZE-1)*8,                  4,     "DES",
#endif

#ifdef STRONG
#ifdef CSP_USE_3DES
   CALG_3DES_112,   (DES2_KEYSIZE-2)*8,                 13,    "3DES TWO KEY",

   CALG_3DES,       (DES3_KEYSIZE-3)*8,                 5,     "3DES",
#endif
#endif // STRONG

#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8,                 6,     "SHA-1",
#endif

#ifdef CSP_USE_MD2
   CALG_MD2,        MD2DIGESTLEN*8,                     4,     "MD2",
#endif

#ifdef CSP_USE_MD4
   CALG_MD4,        MD4DIGESTLEN*8,                     4,     "MD4",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8,                     4,     "MD5",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,                      12,    "SSL3 SHAMD5",

#ifdef CSP_USE_MAC
   CALG_MAC,        64,                                 4,     "MAC",
#endif

   CALG_RSA_SIGN,   RSA_DEF_SIGN_MODLEN*8,              9,     "RSA_SIGN",

   CALG_RSA_KEYX,   RSA_DEF_EXCH_MODLEN*8,              9,     "RSA_KEYX",

   CALG_HMAC,       0,                                  5,     "HMAC",

   0,               0,                                  0,     0

};

const ENUMALGS EnumFullFrance[] =
{
// ALGID            BitLen                              NameLen  szName
#ifdef CSP_USE_RC2
   CALG_RC2,        RC2_DEF_KEYSIZE*8,                  4,     "RC2",
#endif

#ifdef CSP_USE_RC4
   CALG_RC4,        RC4_DEF_KEYSIZE*8,                  4,     "RC4",
#endif

#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8,                 6,     "SHA-1",
#endif

#ifdef CSP_USE_MD2
   CALG_MD2,        MD2DIGESTLEN*8,                     4,     "MD2",
#endif

#ifdef CSP_USE_MD4
   CALG_MD4,        MD4DIGESTLEN*8,                     4,     "MD4",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8,                     4,     "MD5",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,                      12,    "SSL3 SHAMD5",

#ifdef CSP_USE_MAC
   CALG_MAC,        64,                                 4,     "MAC",
#endif

   CALG_RSA_SIGN,   RSA_DEF_SIGN_MODLEN*8,              9,     "RSA_SIGN",

   CALG_RSA_KEYX,   RSA_DEF_EXCH_MODLEN*8,              9,     "RSA_KEYX",

   CALG_HMAC,       0,                                  5,     "HMAC",

   0,               0,                                  0,     0

};

#ifdef STRONG
const ENUMALGS EnumFullNewStrong[] =
{
// ALGID            BitLen                              NameLen  szName
#ifdef CSP_USE_RC2
   CALG_RC2,        RC2_DEF_NEWSTRONG_KEYSIZE*8,        4,     "RC2",
#endif

#ifdef CSP_USE_RC4
   CALG_RC4,        RC4_DEF_NEWSTRONG_KEYSIZE*8,        4,     "RC4",
#endif

#ifdef CSP_USE_DES
   CALG_DES,        (DES_KEYSIZE-1)*8,                  4,     "DES",
#endif

#ifdef CSP_USE_3DES
   CALG_3DES_112,   (DES2_KEYSIZE-2)*8,                 13,    "3DES TWO KEY",

   CALG_3DES,       (DES3_KEYSIZE-3)*8,                 5,     "3DES",
#endif

#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8,                 6,     "SHA-1",
#endif

#ifdef CSP_USE_MD2
   CALG_MD2,        MD2DIGESTLEN*8,                     4,     "MD2",
#endif

#ifdef CSP_USE_MD4
   CALG_MD4,        MD4DIGESTLEN*8,                     4,     "MD4",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8,                     4,     "MD5",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,                      12,    "SSL3 SHAMD5",

#ifdef CSP_USE_MAC
   CALG_MAC,        64,                                 4,     "MAC",
#endif

   CALG_RSA_SIGN,   RSA_DEF_NEWSTRONG_SIGN_MODLEN*8,    9,     "RSA_SIGN",

   CALG_RSA_KEYX,   RSA_DEF_NEWSTRONG_EXCH_MODLEN*8,    9,     "RSA_KEYX",

   CALG_HMAC,       0,                                  5,     "HMAC",

   0,               0,                                  0,     0

};
#endif // STRONG

const ENUMALGS EnumSch[] =
{
// ALGID            BitLen                              NameLen  szName
#ifdef CSP_USE_RC2
   CALG_RC2,        RC2_DEF_KEYSIZE*8,                  4,     "RC2",
#endif

#ifdef CSP_USE_RC4
   CALG_RC4,        RC4_DEF_KEYSIZE*8,                  4,     "RC4",
#endif

#ifdef CSP_USE_DES
   CALG_DES,        (DES_KEYSIZE-1)*8,                  4,     "DES",
#endif

#ifdef STRONG
#ifdef CSP_USE_3DES
   CALG_3DES_112,   (DES2_KEYSIZE-2)*8,                 13,    "3DES TWO KEY",

   CALG_3DES,       (DES3_KEYSIZE-3)*8,                 5,     "3DES",
#endif
#endif // STRONG

#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8,                 6,     "SHA-1",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8,                     4,     "MD5",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,                      12,    "SSL3 SHAMD5",

#ifdef CSP_USE_MAC
   CALG_MAC,        64,                                 4,     "MAC",
#endif

   CALG_RSA_SIGN,   RSA_DEF_EXCH_MODLEN*8,              9,     "RSA_SIGN",

   CALG_RSA_KEYX,   RSA_DEF_EXCH_MODLEN*8,              9,     "RSA_KEYX",

   CALG_HMAC,       0,                                  5,     "HMAC",

   0,               0,                                  0,     0

};

const ENUMALGS EnumSchSGC[] =
{
// ALGID            BitLen                              NameLen  szName
#ifdef CSP_USE_RC2
   CALG_RC2,        SGC_RC2_DEF_KEYSIZE*8,              4,     "RC2",
#endif

#ifdef CSP_USE_RC4
   CALG_RC4,        SGC_RC4_DEF_KEYSIZE*8,              4,     "RC4",
#endif

#ifdef CSP_USE_DES
   CALG_DES,        (DES_KEYSIZE-1)*8,                  4,     "DES",
#endif

#ifdef CSP_USE_3DES
   CALG_3DES_112,   (DES2_KEYSIZE-2)*8,                 13,    "3DES TWO KEY",

   CALG_3DES,       (DES3_KEYSIZE-3)*8,                 5,     "3DES",
#endif

#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8,                 6,     "SHA-1",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8,                     4,     "MD5",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,                      12,    "SSL3 SHAMD5",

#ifdef CSP_USE_MAC
   CALG_MAC,        64,                                 4,     "MAC",
#endif

   CALG_RSA_SIGN,   SGC_RSA_DEF_EXCH_MODLEN*8,          9,     "RSA_SIGN",

   CALG_RSA_KEYX,   SGC_RSA_DEF_EXCH_MODLEN*8,          9,     "RSA_KEYX",

   CALG_HMAC,       0,                                  5,     "HMAC",

   0,               0,                                  0,     0

};

const ENUMALGSEX EnumExSig[] =
{
// ALGID            Default         Min             Max             Protocols            
//                  NameLen  szName     LongNameLen LongName
#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8, CRYPT_FLAG_SIGNING,
                    6,     "SHA-1",     30, "Secure Hash Algorithm (SHA-1)",
#endif

#ifdef CSP_USE_MD2
   CALG_MD2,        MD2DIGESTLEN*8, MD2DIGESTLEN*8, MD2DIGESTLEN*8, CRYPT_FLAG_SIGNING,
                    4,     "MD2",       23, "Message Digest 2 (MD2)",
#endif

#ifdef CSP_USE_MD4
   CALG_MD4,        MD4DIGESTLEN*8, MD4DIGESTLEN*8, MD4DIGESTLEN*8, CRYPT_FLAG_SIGNING,
                    4,     "MD4",       23, "Message Digest 4 (MD4)",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8, MD5DIGESTLEN*8, MD5DIGESTLEN*8, CRYPT_FLAG_SIGNING,
                    4,     "MD5",       23, "Message Digest 5 (MD5)",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  0,
                    12,     "SSL3 SHAMD5", 12,     "SSL3 SHAMD5",

   CALG_RSA_SIGN,   RSA_DEF_SIGN_MODLEN*8, RSA_MIN_SIGN_MODLEN*8, RSA_MAX_SIGN_MODLEN*8, CRYPT_FLAG_SIGNING,
                    9,     "RSA_SIGN",  14, "RSA Signature",

   0,               0,              0,              0,              0,
                    0,     0,           0,  0

};

const ENUMALGSEX EnumExFull[] =
{
// ALGID            Default         Min             Max             Protocols            
//                  NameLen  szName     LongNameLen LongName
#ifdef CSP_USE_RC2
   CALG_RC2,        RC2_DEF_KEYSIZE*8,  RC2_MIN_KEYSIZE*8,  RC2_MAX_KEYSIZE*8,  0,
                    4,      "RC2",      24, "RSA Data Security's RC2",
#endif

#ifdef CSP_USE_RC4
   CALG_RC4,        RC4_DEF_KEYSIZE*8,  RC4_MIN_KEYSIZE*8,  RC4_MAX_KEYSIZE*8,  0,
                    4,     "RC4",       24, "RSA Data Security's RC4",
#endif

#ifdef CSP_USE_DES
   CALG_DES,        56,             56,             56,             0,
                    4,     "DES",       31, "Data Encryption Standard (DES)",
#endif

#ifdef STRONG
#ifdef CSP_USE_3DES
   CALG_3DES_112,   112,            112,            112,            0,
                    13,    "3DES TWO KEY", 19, "Two Key Triple DES",

   CALG_3DES,       168,            168,            168,            0,
                    5,     "3DES",      21, "Three Key Triple DES",
#endif
#endif // STRONG

#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8, CRYPT_FLAG_SIGNING,
                    6,     "SHA-1",     30, "Secure Hash Algorithm (SHA-1)",
#endif

#ifdef CSP_USE_MD2
   CALG_MD2,        MD2DIGESTLEN*8, MD2DIGESTLEN*8, MD2DIGESTLEN*8, CRYPT_FLAG_SIGNING,
                    4,     "MD2",       23, "Message Digest 2 (MD2)",
#endif

#ifdef CSP_USE_MD4
   CALG_MD4,        MD4DIGESTLEN*8, MD4DIGESTLEN*8, MD4DIGESTLEN*8, CRYPT_FLAG_SIGNING,
                    4,     "MD4",       23, "Message Digest 4 (MD4)",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8, MD5DIGESTLEN*8, MD5DIGESTLEN*8, CRYPT_FLAG_SIGNING,
                    4,     "MD5",       23, "Message Digest 5 (MD5)",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  0,
                    12,     "SSL3 SHAMD5", 12,     "SSL3 SHAMD5",

#ifdef CSP_USE_MAC
   CALG_MAC,        0,              0,              0,              0,
                    4,     "MAC",       28, "Message Authentication Code",
#endif

   CALG_RSA_SIGN,   RSA_DEF_SIGN_MODLEN*8, RSA_MIN_SIGN_MODLEN*8, RSA_MAX_SIGN_MODLEN*8,
   CRYPT_FLAG_IPSEC | CRYPT_FLAG_SIGNING,
   9,     "RSA_SIGN",  14, "RSA Signature",

   CALG_RSA_KEYX,   RSA_DEF_EXCH_MODLEN*8, RSA_MIN_EXCH_MODLEN*8, RSA_MAX_EXCH_MODLEN*8,
   CRYPT_FLAG_IPSEC | CRYPT_FLAG_SIGNING,
   9,     "RSA_KEYX",  17, "RSA Key Exchange",

   CALG_HMAC,       0,              0,              0,              0,
                    5,     "HMAC",      18, "Hugo's MAC (HMAC)",

   0,               0,              0,              0,              0,
                    0,     0,           0,  0
};

#ifdef STRONG
const ENUMALGSEX EnumExFullNewStrong[] =
{
// ALGID            Default         Min             Max             Protocols            
//                  NameLen  szName     LongNameLen LongName
#ifdef CSP_USE_RC2
   CALG_RC2,        RC2_DEF_NEWSTRONG_KEYSIZE*8,  RC2_MIN_KEYSIZE*8,  RC2_MAX_KEYSIZE*8,  0,
                    4,      "RC2",      24, "RSA Data Security's RC2",
#endif

#ifdef CSP_USE_RC4
   CALG_RC4,        RC4_DEF_NEWSTRONG_KEYSIZE*8,  RC4_MIN_KEYSIZE*8,  RC4_MAX_KEYSIZE*8,  0,
                    4,     "RC4",       24, "RSA Data Security's RC4",
#endif

#ifdef CSP_USE_DES
   CALG_DES,        56,             56,             56,             0,
                    4,     "DES",       31, "Data Encryption Standard (DES)",
#endif

#ifdef CSP_USE_3DES
   CALG_3DES_112,   112,            112,            112,            0,
                    13,    "3DES TWO KEY", 19, "Two Key Triple DES",

   CALG_3DES,       168,            168,            168,            0,
                    5,     "3DES",      21, "Three Key Triple DES",
#endif

#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8, CRYPT_FLAG_SIGNING,
                    6,     "SHA-1",     30, "Secure Hash Algorithm (SHA-1)",
#endif

#ifdef CSP_USE_MD2
   CALG_MD2,        MD2DIGESTLEN*8, MD2DIGESTLEN*8, MD2DIGESTLEN*8, CRYPT_FLAG_SIGNING,
                    4,     "MD2",       23, "Message Digest 2 (MD2)",
#endif

#ifdef CSP_USE_MD4
   CALG_MD4,        MD4DIGESTLEN*8, MD4DIGESTLEN*8, MD4DIGESTLEN*8, CRYPT_FLAG_SIGNING,
                    4,     "MD4",       23, "Message Digest 4 (MD4)",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8, MD5DIGESTLEN*8, MD5DIGESTLEN*8, CRYPT_FLAG_SIGNING,
                    4,     "MD5",       23, "Message Digest 5 (MD5)",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  0,
                    12,     "SSL3 SHAMD5", 12,     "SSL3 SHAMD5",

#ifdef CSP_USE_MAC
   CALG_MAC,        0,              0,              0,              0,
                    4,     "MAC",       28, "Message Authentication Code",
#endif

   CALG_RSA_SIGN,   RSA_DEF_NEWSTRONG_SIGN_MODLEN*8, RSA_MIN_SIGN_MODLEN*8, RSA_MAX_SIGN_MODLEN*8,
   CRYPT_FLAG_IPSEC | CRYPT_FLAG_SIGNING,
   9,     "RSA_SIGN",  14, "RSA Signature",

   CALG_RSA_KEYX,   RSA_DEF_NEWSTRONG_EXCH_MODLEN*8, RSA_MIN_EXCH_MODLEN*8, RSA_MAX_EXCH_MODLEN*8,
   CRYPT_FLAG_IPSEC | CRYPT_FLAG_SIGNING,
   9,     "RSA_KEYX",  17, "RSA Key Exchange",

   CALG_HMAC,       0,              0,              0,              0,
                    5,     "HMAC",      18, "Hugo's MAC (HMAC)",

   0,               0,              0,              0,              0,
                    0,     0,           0,  0
};
#endif // STRONG

const ENUMALGSEX EnumExFullFrance[] =
{
// ALGID            Default         Min             Max             Protocols            
//                  NameLen  szName     LongNameLen LongName
#ifdef CSP_USE_RC2
   CALG_RC2,        RC2_DEF_KEYSIZE*8,  RC2_MIN_KEYSIZE*8,  RC2_MAX_FRENCH_KEYSIZE*8,  0,
                    4,      "RC2",      24, "RSA Data Security's RC2",
#endif

#ifdef CSP_USE_RC4
   CALG_RC4,        RC4_DEF_KEYSIZE*8,  RC4_MIN_KEYSIZE*8,  RC4_MAX_FRENCH_KEYSIZE*8,  0,
                    4,     "RC4",       24, "RSA Data Security's RC4",
#endif

#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8, CRYPT_FLAG_SIGNING,
                    6,     "SHA-1",     30, "Secure Hash Algorithm (SHA-1)",
#endif

#ifdef CSP_USE_MD2
   CALG_MD2,        MD2DIGESTLEN*8, MD2DIGESTLEN*8, MD2DIGESTLEN*8, CRYPT_FLAG_SIGNING,
                    4,     "MD2",       23, "Message Digest 2 (MD2)",
#endif

#ifdef CSP_USE_MD4
   CALG_MD4,        MD4DIGESTLEN*8, MD4DIGESTLEN*8, MD4DIGESTLEN*8, CRYPT_FLAG_SIGNING,
                    4,     "MD4",       23, "Message Digest 4 (MD4)",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8, MD5DIGESTLEN*8, MD5DIGESTLEN*8, CRYPT_FLAG_SIGNING,
                    4,     "MD5",       23, "Message Digest 5 (MD5)",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  0,
                    12,     "SSL3 SHAMD5", 12,     "SSL3 SHAMD5",

#ifdef CSP_USE_MAC
   CALG_MAC,        0,              0,              0,              0,
                    4,     "MAC",       28, "Message Authentication Code",
#endif

   CALG_RSA_SIGN,   RSA_DEF_SIGN_MODLEN*8, RSA_MIN_SIGN_MODLEN*8, RSA_MAX_SIGN_MODLEN*8,
   CRYPT_FLAG_IPSEC | CRYPT_FLAG_SIGNING,
   9,     "RSA_SIGN",  14, "RSA Signature",

   CALG_RSA_KEYX,   RSA_DEF_EXCH_MODLEN*8, RSA_MIN_EXCH_MODLEN*8, RSA_MAX_EXCH_FRENCH_MODLEN*8,
   CRYPT_FLAG_IPSEC | CRYPT_FLAG_SIGNING,
   9,     "RSA_KEYX",  17, "RSA Key Exchange",

   CALG_HMAC,       0,              0,              0,              0,
                    5,     "HMAC",      18, "Hugo's MAC (HMAC)",

   0,               0,              0,              0,              0,
                    0,     0,           0,  0
};

const ENUMALGSEX EnumExSch[] =
{
// ALGID            Default         Min             Max             Protocols            
//                  NameLen  szName     LongNameLen LongName
#ifdef CSP_USE_RC2
   CALG_RC2,        RC2_DEF_KEYSIZE*8,  RC2_MIN_KEYSIZE*8,  RC2_MAX_KEYSIZE*8,  SUPPORTED_PROTOCOLS,
                    4,      "RC2",      24, "RSA Data Security's RC2",
#endif

#ifdef CSP_USE_RC4
   CALG_RC4,        RC4_DEF_KEYSIZE*8,  RC4_MIN_KEYSIZE*8,  RC4_MAX_KEYSIZE*8,  SUPPORTED_PROTOCOLS,
                    4,     "RC4",       24, "RSA Data Security's RC4",
#endif

#ifdef CSP_USE_DES
   CALG_DES,        56,             56,             56,             SUPPORTED_PROTOCOLS,
                    4,     "DES",       31, "Data Encryption Standard (DES)",
#endif

#ifdef STRONG
#ifdef CSP_USE_3DES
   CALG_3DES_112,   112,            112,            112,            SUPPORTED_PROTOCOLS,
                    13,    "3DES TWO KEY", 19, "Two Key Triple DES",

   CALG_3DES,       168,            168,            168,            SUPPORTED_PROTOCOLS,
                    5,     "3DES",      21, "Three Key Triple DES",
#endif
#endif // STRONG

#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8,
        SUPPORTED_PROTOCOLS | CRYPT_FLAG_SIGNING,
                    6,     "SHA-1",     30, "Secure Hash Algorithm (SHA-1)",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8, MD5DIGESTLEN*8, MD5DIGESTLEN*8,
   SUPPORTED_PROTOCOLS | CRYPT_FLAG_SIGNING,
                    4,     "MD5",       23, "Message Digest 5 (MD5)",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  0,
                    12,     "SSL3 SHAMD5", 12,     "SSL3 SHAMD5",

#ifdef CSP_USE_MAC
   CALG_MAC,        0,              0,              0,              0,
                    4,     "MAC",       28, "Message Authentication Code",
#endif

   CALG_RSA_SIGN,   RSA_DEF_EXCH_MODLEN*8, RSA_MIN_EXCH_MODLEN*8, RSA_MAX_EXCH_MODLEN*8,
   SUPPORTED_PROTOCOLS | CRYPT_FLAG_SIGNING,
                    9,     "RSA_SIGN",  14, "RSA Signature",

   CALG_RSA_KEYX,   RSA_DEF_EXCH_MODLEN*8, RSA_MIN_EXCH_MODLEN*8, RSA_MAX_EXCH_MODLEN*8,
   SUPPORTED_PROTOCOLS | CRYPT_FLAG_SIGNING,
                    9,     "RSA_KEYX",  17, "RSA Key Exchange",

   CALG_HMAC,       0,              0,              0,              0,
                    5,     "HMAC",      18, "Hugo's MAC (HMAC)",

   0,               0,              0,              0,              0,
                    0,     0,           0,  0
};

const ENUMALGSEX EnumExSchFrance[] =
{
// ALGID            Default         Min             Max             Protocols            
//                  NameLen  szName     LongNameLen LongName
#ifdef CSP_USE_RC2
   CALG_RC2,        RC2_DEF_KEYSIZE*8,  RC2_MIN_KEYSIZE*8,  RC2_MAX_FRENCH_KEYSIZE*8,  SUPPORTED_PROTOCOLS,
                    4,      "RC2",      24, "RSA Data Security's RC2",
#endif

#ifdef CSP_USE_RC4
   CALG_RC4,        RC4_DEF_KEYSIZE*8,  RC4_MIN_KEYSIZE*8,  RC4_MAX_FRENCH_KEYSIZE*8,  SUPPORTED_PROTOCOLS,
                    4,     "RC4",       24, "RSA Data Security's RC4",
#endif

#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8, SUPPORTED_PROTOCOLS | CRYPT_FLAG_SIGNING,
                    6,     "SHA-1",     30, "Secure Hash Algorithm (SHA-1)",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8, MD5DIGESTLEN*8, MD5DIGESTLEN*8, SUPPORTED_PROTOCOLS | CRYPT_FLAG_SIGNING,
                    4,     "MD5",       23, "Message Digest 5 (MD5)",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  0,
                    12,     "SSL3 SHAMD5", 12,     "SSL3 SHAMD5",

#ifdef CSP_USE_MAC
   CALG_MAC,        0,              0,              0,              0,
                    4,     "MAC",       28, "Message Authentication Code",
#endif

   CALG_RSA_SIGN,   RSA_DEF_EXCH_MODLEN*8, RSA_MIN_EXCH_MODLEN*8, RSA_MAX_EXCH_FRENCH_MODLEN*8, SUPPORTED_PROTOCOLS | CRYPT_FLAG_SIGNING,
                    9,     "RSA_SIGN",  14, "RSA Signature",

   CALG_RSA_KEYX,   RSA_DEF_EXCH_MODLEN*8, RSA_MIN_EXCH_MODLEN*8, RSA_MAX_EXCH_FRENCH_MODLEN*8, SUPPORTED_PROTOCOLS | CRYPT_FLAG_SIGNING,
                    9,     "RSA_KEYX",  17, "RSA Key Exchange",

   CALG_HMAC,       0,              0,              0,              0,
                    5,     "HMAC",      18, "Hugo's MAC (HMAC)",

   0,               0,              0,              0,              0,
                    0,     0,           0,  0
};

const ENUMALGSEX EnumExSchSGC[] =
{
// ALGID            Default         Min             Max             Protocols            
//                  NameLen  szName     LongNameLen LongName
#ifdef CSP_USE_RC2
   CALG_RC2,        SGC_RC2_DEF_KEYSIZE*8,  RC2_MIN_KEYSIZE*8,  SGC_RC2_MAX_KEYSIZE*8,  SUPPORTED_PROTOCOLS,
                    4,      "RC2",      24, "RSA Data Security's RC2",
#endif

#ifdef CSP_USE_RC4
   CALG_RC4,        SGC_RC4_DEF_KEYSIZE*8,  RC4_MIN_KEYSIZE*8,  SGC_RC4_MAX_KEYSIZE*8,  SUPPORTED_PROTOCOLS,
                    4,     "RC4",       24, "RSA Data Security's RC4",
#endif

#ifdef CSP_USE_DES
   CALG_DES,        56,             56,             56,             SUPPORTED_PROTOCOLS,
                    4,     "DES",       31, "Data Encryption Standard (DES)",
#endif

#ifdef CSP_USE_3DES
   CALG_3DES_112,   112,            112,            112,            SUPPORTED_PROTOCOLS,
                    13,    "3DES TWO KEY", 19, "Two Key Triple DES",

   CALG_3DES,       168,            168,            168,            SUPPORTED_PROTOCOLS,
                    5,     "3DES",      21, "Three Key Triple DES",
#endif

#ifdef CSP_USE_SHA
   CALG_SHA,        A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8, A_SHA_DIGEST_LEN*8,
   SUPPORTED_PROTOCOLS | CRYPT_FLAG_SIGNING,
                    6,     "SHA-1",     30, "Secure Hash Algorithm (SHA-1)",
#endif

#ifdef CSP_USE_MD5
   CALG_MD5,        MD5DIGESTLEN*8, MD5DIGESTLEN*8, MD5DIGESTLEN*8,
   SUPPORTED_PROTOCOLS | CRYPT_FLAG_SIGNING,
                    4,     "MD5",       23, "Message Digest 5 (MD5)",
#endif

   CALG_SSL3_SHAMD5,SSL3SHAMD5LEN,  SSL3SHAMD5LEN,  SSL3SHAMD5LEN, 0,
                    12,     "SSL3 SHAMD5", 12,     "SSL3 SHAMD5",

#ifdef CSP_USE_MAC
   CALG_MAC,        0,              0,              0,              0,
                    4,     "MAC",       28, "Message Authentication Code",
#endif

   CALG_RSA_SIGN,   SGC_RSA_DEF_EXCH_MODLEN*8, RSA_MIN_EXCH_MODLEN*8, SGC_RSA_MAX_EXCH_MODLEN*8,
   SUPPORTED_PROTOCOLS | CRYPT_FLAG_SIGNING,
                    9,     "RSA_SIGN",  14, "RSA Signature",

   CALG_RSA_KEYX,   SGC_RSA_DEF_EXCH_MODLEN*8, RSA_MIN_EXCH_MODLEN*8, SGC_RSA_MAX_EXCH_MODLEN*8,
   SUPPORTED_PROTOCOLS | CRYPT_FLAG_SIGNING,
                    9,     "RSA_KEYX",  17, "RSA Key Exchange",

   CALG_HMAC,       0,              0,              0,              0,
                    5,     "HMAC",      18, "Hugo's MAC (HMAC)",

   0,               0,              0,              0,              0,
                    0,     0,           0,  0
};

#define NTAG_REG_KEY_LOC    "Software\\Microsoft\\Cryptography\\UserKeys"
#define NTAG_MACH_REG_KEY_LOC   "Software\\Microsoft\\Cryptography\\MachineKeys"

BOOL CPInflateKey(
                  IN PNTAGKeyList pTmpKey
                  );

BOOL CopyKey(
             IN PNTAGKeyList pOldKey,
             OUT PNTAGKeyList *ppNewKey);

BOOL SymEncrypt(
                IN PNTAGKeyList pKey,
                IN BOOL fFinal,
                IN OUT BYTE *pbData,
                IN OUT DWORD *pcbData,
                IN DWORD cbBuf
                );

BOOL SymDecrypt(
                IN PNTAGKeyList pKey,
                IN PNTAGHashList pHash,
                IN BOOL fFinal,
                IN OUT BYTE *pbData,
                IN OUT DWORD *pcbData
                );

BOOL BlockEncrypt(void EncFun(BYTE *In, BYTE *Out, void *key, int op),
                  PNTAGKeyList pKey,
                  int BlockLen,
                  BOOL Final,
                  BYTE  *pbData,
                  DWORD *pdwDataLen,
                  DWORD dwBufLen);

static BYTE rgbSymmetricKeyWrapIV[8] = {0x4a, 0xdd, 0xa2, 0x2c, 0x79, 0xe8, 0x21, 0x05};


//
// Set the permissions on the key
//
void SetInitialKeyPermissions(
                              PNTAGKeyList pKey
                              )
{
    if (CRYPT_EXPORTABLE == pKey->Rights)
    {
        pKey->Permissions |= CRYPT_EXPORT;
    }

    // UNDONE - set the appopropriate permission with the appropriate algorithm
    pKey->Permissions |= CRYPT_ENCRYPT | CRYPT_DECRYPT| CRYPT_READ |
                         CRYPT_WRITE | CRYPT_MAC;
}

/* MakeNewKey
 *
 *  Helper routine for ImportKey, GenKey
 *
 *  Allocate a new key record, fill in the data and copy in the key
 *  bytes.
 */
PNTAGKeyList MakeNewKey(
        ALG_ID      aiKeyAlg,
        DWORD       dwRights,
        DWORD       dwKeyLen,
        HCRYPTPROV  hUID,
        BYTE        *pbKeyData,
        BOOL        fUsePassedKeyBuffer
    )
{
    PNTAGKeyList    pKey = NULL;
    BSAFE_PUB_KEY   *pPubKey;
    BOOL            fSuccess = FALSE;

    // allocate space for the key record
    if ((pKey = (PNTAGKeyList)_nt_malloc(sizeof(NTAGKeyList))) == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    if (!fUsePassedKeyBuffer)
    {
        if ((pKey->pKeyValue = (BYTE *)_nt_malloc((size_t)dwKeyLen)) == NULL)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
    }

    pKey->Algid = aiKeyAlg;
    pKey->Rights = dwRights;
    pKey->cbDataLen = 0;
    pKey->pData = NULL;
    pKey->hUID = hUID;
    memset(pKey->IV, 0, CRYPT_BLKLEN);
    memset(pKey->FeedBack, 0, CRYPT_BLKLEN);
    pKey->InProgress = FALSE;

    pKey->cbSaltLen = 0;

    pKey->Padding = PKCS5_PADDING;
    pKey->Mode = CRYPT_MODE_CBC;
    pKey->ModeBits = 0;

    SetInitialKeyPermissions(pKey);

    // for RC2 set a default effective key length
    if (CALG_RC2 == aiKeyAlg)
    {
        pKey->EffectiveKeyLen = RC2_DEFAULT_EFFECTIVE_KEYLEN;
    }

    pKey->cbKeyLen = dwKeyLen;
    if (pbKeyData != NULL)
    {
        if (fUsePassedKeyBuffer)
        {
            pKey->pKeyValue = pbKeyData;
        }
        else
        {
            memcpy (pKey->pKeyValue, pbKeyData, (size_t)dwKeyLen);
        }
    }

    pPubKey = (BSAFE_PUB_KEY *) pKey->pKeyValue;
    fSuccess = TRUE;
Ret:
    if (FALSE == fSuccess)
    {
        _nt_free (pKey, sizeof(NTAGKeyList));
        pKey = NULL;
    }
    return pKey;
}

/* FreeNewKey
 *
 *      Use for cleanup on abort of key build operations.
 *
 */

void FreeNewKey(PNTAGKeyList pOldKey)
{
    if (pOldKey->pKeyValue)
        _nt_free(pOldKey->pKeyValue, pOldKey->cbKeyLen);
    if (pOldKey->pData)
        _nt_free(pOldKey->pData, pOldKey->cbDataLen);
    _nt_free(pOldKey, sizeof(NTAGKeyList));
}

/* FIsLegalSGCKeySize
 *
 *      Check that the length of the key is SGC legal (essentially
 *      complies with export).
 *
 */

BOOL FIsLegalSGCKeySize(
                        IN ALG_ID Algid,
                        IN DWORD cbKey,
                        IN BOOL fRC2BigKeyOK,
                        IN BOOL fGenKey,
                        OUT BOOL *pfPubKey
                        )
{
    BOOL    fRet = FALSE;

    *pfPubKey = FALSE;

    switch (Algid)
    {
#ifdef CSP_USE_RC4
        case CALG_RC4:
        {
            // legality check
            if (fGenKey || (cbKey < RC4_MIN_KEYSIZE) ||
                (cbKey > SGC_RC4_MAX_KEYSIZE))
            {
                goto Ret;
            }
            break;
        }
#endif

#ifdef CSP_USE_RC2
        case CALG_RC2:
        {
            if (!fRC2BigKeyOK)
            {
                if (fGenKey || (cbKey < RC2_MIN_KEYSIZE) ||
                    (cbKey > SGC_RC2_MAX_KEYSIZE))
                {
                    goto Ret;
                }
            }
            break;
        }
#endif

#ifdef CSP_USE_DES
        case CALG_DES:
        {
            if (fGenKey || (cbKey > DES_KEYSIZE))
                goto Ret;
            break;
        }
#endif

#ifdef CSP_USE_3DES
        case CALG_3DES_112:
        {
            if (fGenKey || (cbKey > DES2_KEYSIZE))
                goto Ret;
            break;
        }
        case CALG_3DES:
        {
            if (fGenKey || (cbKey > DES3_KEYSIZE))
                goto Ret;
            break;
        }
#endif

#ifdef CSP_USE_SSL3
        case CALG_SSL3_MASTER:
        case CALG_TLS1_MASTER:
        {
            if (cbKey != SSL3_MASTER_KEYSIZE)
                goto Ret;
            break;
        }

        case CALG_PCT1_MASTER:
        {
            if (cbKey != PCT1_MASTER_KEYSIZE)
                goto Ret;
            break;
        }

        case CALG_SSL2_MASTER:
        {
            if (cbKey > SSL2_MAX_MASTER_KEYSIZE)
                goto Ret;
            break;
        }

        case CALG_SCHANNEL_MAC_KEY:
        {
            break;
        }
#endif

        case CALG_RSA_KEYX:
            if ((cbKey > SGC_RSA_MAX_EXCH_MODLEN) ||
                (cbKey < RSA_MIN_EXCH_MODLEN))
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }
            *pfPubKey = TRUE;
            break;

        default:
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
            break;
    }

    fRet = TRUE;
Ret:
    // not of regulation size
    if (FALSE == fRet)
        SetLastError((DWORD)NTE_BAD_FLAGS);
    return fRet;
}

/* FIsLegalFrenchKeySize
 *
 *      Check that the length of the key is legal (essentially
 *      complies with export).
 *
 */

BOOL FIsLegalFrenchKeySize(
                           IN ALG_ID Algid,
                           IN DWORD cbKey,
                           IN BOOL fRC2BigKeyOK,
                           IN DWORD dwFlags,
                           OUT BOOL *pfPubKey
                           )
{
    BOOL    fRet = FALSE;

    *pfPubKey = FALSE;

    switch (Algid)
    {
#ifdef CSP_USE_RC4
        case CALG_RC4:
        {
            // legality check
            if ((cbKey < RC4_MIN_KEYSIZE) ||
                (cbKey > RC4_MAX_FRENCH_KEYSIZE))
            {
                goto Ret;
            }
            break;
        }
#endif

#ifdef CSP_USE_RC2
        case CALG_RC2:
        {
            if (!fRC2BigKeyOK)
            {
                if ((cbKey < RC2_MIN_KEYSIZE) ||
                    (cbKey > RC2_MAX_FRENCH_KEYSIZE))
                {
                    goto Ret;
                }
            }
            break;
        }
#endif

#ifdef CSP_USE_SSL3
        case CALG_SSL3_MASTER:
        {
            if (cbKey != SSL3_MASTER_KEYSIZE)
                goto Ret;
            break;
        }

        case CALG_SSL2_MASTER:
        {
            if (cbKey > SSL2_MAX_MASTER_KEYSIZE)
                goto Ret;
            break;
        }

        case CALG_SCHANNEL_MAC_KEY:
        {
            break;
        }
#endif

        case CALG_RSA_SIGN:
            if ((cbKey > RSA_MAX_SIGN_MODLEN) ||
                (cbKey < RSA_MIN_SIGN_MODLEN))
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }
            *pfPubKey = TRUE;
            break;
        case CALG_RSA_KEYX:
            if ((cbKey > RSA_MAX_EXCH_FRENCH_MODLEN) ||
                (cbKey < RSA_MIN_EXCH_MODLEN))
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }
            *pfPubKey = TRUE;
            break;

        default:
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
            break;
    }

    fRet = TRUE;
Ret:
    // not of regulation size
    if (FALSE == fRet)
        SetLastError((DWORD)NTE_BAD_FLAGS);
    return fRet;
}

/* FIsLegalKeySize
 *
 *      Check that the length of the key is legal (essentially
 *      complies with export).
 *
 */

BOOL FIsLegalKeySize(
                     IN ALG_ID Algid,
                     IN DWORD cbKey,
                     IN BOOL fRC2BigKeyOK,
                     IN DWORD dwFlags,
                     OUT BOOL *pfPubKey
                     )
{
    BOOL    fRet = FALSE;

    *pfPubKey = FALSE;

    switch (Algid)
    {
#ifdef CSP_USE_RC4
        case CALG_RC4:
        {
            // legality check
            if ((cbKey < RC4_MIN_KEYSIZE) ||
                (cbKey > RC4_MAX_KEYSIZE))
            {
                goto Ret;
            }
            break;
        }
#endif

#ifdef CSP_USE_RC2
        case CALG_RC2:
        {
            if (!fRC2BigKeyOK)
            {
                if ((cbKey < RC2_MIN_KEYSIZE) ||
                    (cbKey > RC2_MAX_KEYSIZE))
                {
                    goto Ret;
                }
            }
            break;
        }
#endif

#ifdef CSP_USE_DES
        case CALG_DES:
        {
            if (cbKey > DES_KEYSIZE)
                goto Ret;
            break;
        }
#endif

#ifdef STRONG
#ifdef CSP_USE_3DES
        case CALG_3DES_112:
        {
            if (cbKey > DES2_KEYSIZE)
                goto Ret;
            break;
        }
        case CALG_3DES:
        {
            if (cbKey > DES3_KEYSIZE)
                goto Ret;
            break;
        }
#endif
#endif // STRONG

#ifdef CSP_USE_SSL3
        case CALG_SSL3_MASTER:
        case CALG_TLS1_MASTER:
        {
            if (cbKey != SSL3_MASTER_KEYSIZE)
                goto Ret;
            break;
        }

        case CALG_PCT1_MASTER:
        {
            if (cbKey != PCT1_MASTER_KEYSIZE)
                goto Ret;
            break;
        }

        case CALG_SSL2_MASTER:
        {
            if (cbKey > SSL2_MAX_MASTER_KEYSIZE)
                goto Ret;
            break;
        }

        case CALG_SCHANNEL_MAC_KEY:
        {
            break;
        }

#endif

        case CALG_RSA_SIGN:
            if ((cbKey > RSA_MAX_SIGN_MODLEN) ||
                (cbKey < RSA_MIN_SIGN_MODLEN))
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }
            *pfPubKey = TRUE;
            break;
        case CALG_RSA_KEYX:
            if (dwFlags & CRYPT_SGCKEY)
            {
                if (cbKey > SGC_RSA_MAX_EXCH_MODLEN)
                {
                    SetLastError((DWORD) NTE_BAD_FLAGS);
                    goto Ret;
                }
            }
            else
            {
                if ((cbKey > RSA_MAX_EXCH_MODLEN) ||
                    (cbKey < RSA_MIN_EXCH_MODLEN))
                {
                    SetLastError((DWORD) NTE_BAD_FLAGS);
                    goto Ret;
                }
            }
            *pfPubKey = TRUE;
            break;

        default:
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
            break;
    }

    fRet = TRUE;
Ret:
    // not of regulation size
    if (FALSE == fRet)
        SetLastError((DWORD)NTE_BAD_FLAGS);
    return fRet;
}

/* FIsLegalKey
 *
 *      Check that the length of the key is legal (essentially
 *      complies with export).
 *
 */

BOOL FIsLegalKey(
                 IN PNTAGUserList pTmpUser,
                 IN PNTAGKeyList pKey,
                 IN BOOL fRC2BigKeyOK
                 )
{
    BOOL    fPubKey;
    BOOL    fRet = FALSE;

    if (pKey == NULL)
    {
        SetLastError((DWORD)NTE_FAIL);
        goto Ret;
    }

    // check if the provider is an SChannel provider and if so if the
    // SGC flag is set then use the FIsLegalSGCKeySize function
    if ((PROV_RSA_SCHANNEL == pTmpUser->dwProvType) &&
        (0 != pTmpUser->dwSGCFlags))
    {
        if (!FIsLegalSGCKeySize(pKey->Algid, pKey->cbKeyLen,
                                fRC2BigKeyOK, FALSE, &fPubKey))
        {
            goto Ret;
        }
    }
    else
    {
        // if in France then different key sizes may apply
        if (pTmpUser->Rights & CRYPT_IN_FRANCE)
        {
            // 4th parameter, dwFlags, is used for SGC Exch keys so just
            // pass zero in this case
            if (!FIsLegalFrenchKeySize(pKey->Algid, pKey->cbKeyLen,
                                       fRC2BigKeyOK, 0, &fPubKey))
            {
                goto Ret;
            }
        }
        else
        {
            // 4th parameter, dwFlags, is used for SGC Exch keys so just
            // pass zero in this case
            if (!FIsLegalKeySize(pKey->Algid, pKey->cbKeyLen,
                                 fRC2BigKeyOK, 0, &fPubKey))
            {
                goto Ret;
            }
        }
    }

    fRet = TRUE;
Ret:
    return fRet;
}

BOOL MakeKeyRSABaseCompatible(
                            HCRYPTPROV hUID,
                            HCRYPTKEY  hKey)
{
    CRYPT_DATA_BLOB sSaltData;
    BYTE            rgbZeroSalt[11];

    ZeroMemory(rgbZeroSalt, 11);

    sSaltData.pbData = rgbZeroSalt;
    sSaltData.cbData = sizeof(rgbZeroSalt);
    if (!CPSetKeyParam(
               hUID,
               hKey,
               KP_SALT_EX,
               (PBYTE)&sSaltData,
               0))
        return FALSE;

    return TRUE;
}

typedef struct {
    DWORD       magic;                  /* Should always be RSA2 */
    DWORD       bitlen;                 // bit size of key
    DWORD       pubexp;                 // public exponent
} EXPORT_PRV_KEY, FAR *PEXPORT_PRV_KEY;

/* GetLengthOfPrivateKeyForExport
 *
 *      Get the length of the private key
 *      blob from the public key.
 *
 */

void GetLengthOfPrivateKeyForExport(
                                    IN BSAFE_PUB_KEY *pPubKey,
                                    IN DWORD PubKeyLen,
                                    OUT PDWORD pcbBlob
                                    )
{
    DWORD           cbHalfModLen;

    cbHalfModLen = (pPubKey->bitlen + 15) / 16;
    *pcbBlob = sizeof(EXPORT_PRV_KEY) + 9 * cbHalfModLen;
}

/* PreparePrivateKeyForExport
 *
 *      Massage the key from the registry
 *      into an exportable format.
 *
 */

BOOL PreparePrivateKeyForExport(
                                IN BSAFE_PRV_KEY *pPrvKey,
                                IN DWORD PrvKeyLen,
                                OUT PBYTE pbBlob,
                                IN OUT PDWORD pcbBlob
                                )
{
    PEXPORT_PRV_KEY pExportKey;
    DWORD           cbHalfModLen;
    DWORD           cbBlobLen;
    DWORD           cbTmpLen;
    DWORD           cbHalfTmpLen;
    PBYTE           pbIn;
    PBYTE           pbOut;

    cbHalfModLen = (pPrvKey->bitlen + 15) / 16;
    cbBlobLen = sizeof(EXPORT_PRV_KEY) + 9 * cbHalfModLen;

    // figure out the number of overflow bytes which are in the private
    // key structure
    cbTmpLen = (sizeof(DWORD) * 2) - (((pPrvKey->bitlen + 7) / 8) % (sizeof(DWORD) * 2));
    if ((sizeof(DWORD) * 2) != cbTmpLen)
        cbTmpLen += sizeof(DWORD) * 2;
    cbHalfTmpLen = cbTmpLen / 2;

    if (NULL == pbBlob)
    {
        *pcbBlob = cbBlobLen;
        return TRUE;
    }

    if (*pcbBlob < cbBlobLen)
    {
        *pcbBlob = cbBlobLen;
        return FALSE;
    }
    else
    {
        // take most of the header info
        pExportKey = (PEXPORT_PRV_KEY)pbBlob;
        pExportKey->magic = pPrvKey->magic;
        pExportKey->bitlen = pPrvKey->bitlen;
        pExportKey->pubexp = pPrvKey->pubexp;

        pbIn = (PBYTE)pPrvKey + sizeof(BSAFE_PRV_KEY);
        pbOut = pbBlob + sizeof(EXPORT_PRV_KEY);

        // copy all the private key info
        CopyMemory(pbOut, pbIn, pExportKey->bitlen / 8);
        pbIn += pExportKey->bitlen / 8 + cbTmpLen;
        pbOut += pExportKey->bitlen / 8;
        CopyMemory(pbOut, pbIn, cbHalfModLen);
        pbIn += cbHalfModLen + cbHalfTmpLen;
        pbOut += cbHalfModLen;
        CopyMemory(pbOut, pbIn, cbHalfModLen);
        pbIn += cbHalfModLen + cbHalfTmpLen;
        pbOut += cbHalfModLen;
        CopyMemory(pbOut, pbIn, cbHalfModLen);
        pbIn += cbHalfModLen + cbHalfTmpLen;
        pbOut += cbHalfModLen;
        CopyMemory(pbOut, pbIn, cbHalfModLen);
        pbIn += cbHalfModLen + cbHalfTmpLen;
        pbOut += cbHalfModLen;
        CopyMemory(pbOut, pbIn, cbHalfModLen);
        pbIn += cbHalfModLen + cbHalfTmpLen;
        pbOut += cbHalfModLen;
        CopyMemory(pbOut, pbIn, pExportKey->bitlen / 8);
    }
    *pcbBlob = cbBlobLen;

    return TRUE;
}

/* PreparePrivateKeyForImport
 *
 *      Massage the incoming into a form acceptable for
 *      the registry.
 *
 */

BOOL PreparePrivateKeyForImport(
                                IN PBYTE pbBlob,
                                IN DWORD cbBlob,
                                OUT BSAFE_PRV_KEY *pPrvKey,
                                IN OUT PDWORD pPrvKeyLen,
                                OUT BSAFE_PUB_KEY *pPubKey,
                                IN OUT PDWORD pPubKeyLen
                                )
{
    PEXPORT_PRV_KEY pExportKey = (PEXPORT_PRV_KEY)pbBlob;
    DWORD           cbHalfModLen;
    DWORD           cbPub;
    DWORD           cbPrv;
    PBYTE           pbIn;
    PBYTE           pbOut;
    DWORD           cbTmpLen;
    DWORD           cbHalfTmpLen;

    if (RSA2 != pExportKey->magic)
        return FALSE;

    // figure out the number of overflow bytes which are in the private
    // key structure
    cbTmpLen = (sizeof(DWORD) * 2) - (((pExportKey->bitlen + 7) / 8) % (sizeof(DWORD) * 2));
    if ((sizeof(DWORD) * 2) != cbTmpLen)
        cbTmpLen += sizeof(DWORD) * 2;
    cbHalfTmpLen = cbTmpLen / 2;
    cbHalfModLen = (pExportKey->bitlen + 15) / 16;

    cbPub = sizeof(BSAFE_PUB_KEY) + (pExportKey->bitlen / 8) + cbTmpLen;
    cbPrv = sizeof(BSAFE_PRV_KEY) + (cbHalfModLen + cbHalfTmpLen) * 10;
    if ((NULL == pPrvKey) || (NULL == pPubKey))
    {
        *pPubKeyLen = cbPub;
        *pPrvKeyLen = cbPrv;
        return TRUE;
    }

    if ((*pPubKeyLen < cbPub) || (*pPrvKeyLen < cbPrv))
    {
        *pPubKeyLen = cbPub;
        *pPrvKeyLen = cbPrv;
        return FALSE;
    }
    else
    {
        // form the public key
        ZeroMemory(pPubKey, *pPubKeyLen);
        pPubKey->magic = RSA1;
        pPubKey->bitlen = pExportKey->bitlen;
        pPubKey->keylen = (pExportKey->bitlen / 8) + cbTmpLen;
        pPubKey->datalen = (pExportKey->bitlen+7)/8 - 1;
        pPubKey->pubexp = pExportKey->pubexp;

        pbIn = pbBlob + sizeof(EXPORT_PRV_KEY);
        pbOut = (PBYTE)pPubKey + sizeof(BSAFE_PUB_KEY);

        CopyMemory(pbOut, pbIn, pExportKey->bitlen / 8);

        // form the private key
        ZeroMemory(pPrvKey, *pPrvKeyLen);
        pPrvKey->magic = pExportKey->magic;
        pPrvKey->keylen = pPubKey->keylen;
        pPrvKey->bitlen = pExportKey->bitlen;
        pPrvKey->datalen = pPubKey->datalen;
        pPrvKey->pubexp = pExportKey->pubexp;

        pbOut = (PBYTE)pPrvKey + sizeof(BSAFE_PRV_KEY);

        CopyMemory(pbOut, pbIn, pExportKey->bitlen / 8);
        pbOut += pExportKey->bitlen / 8 + cbTmpLen;
        pbIn += pExportKey->bitlen / 8;
        CopyMemory(pbOut, pbIn, cbHalfModLen);
        pbOut += cbHalfModLen + cbHalfTmpLen;
        pbIn += cbHalfModLen;
        CopyMemory(pbOut, pbIn, cbHalfModLen);
        pbOut += cbHalfModLen + cbHalfTmpLen;
        pbIn += cbHalfModLen;
        CopyMemory(pbOut, pbIn, cbHalfModLen);
        pbOut += cbHalfModLen + cbHalfTmpLen;
        pbIn += cbHalfModLen;
        CopyMemory(pbOut, pbIn, cbHalfModLen);
        pbOut += cbHalfModLen + cbHalfTmpLen;
        pbIn += cbHalfModLen;
        CopyMemory(pbOut, pbIn, cbHalfModLen);
        pbOut += cbHalfModLen + cbHalfTmpLen;
        pbIn += cbHalfModLen;
        CopyMemory(pbOut, pbIn, pExportKey->bitlen / 8);
    }
    *pPubKeyLen = cbPub;
    *pPrvKeyLen = cbPrv;

    return TRUE;
}

BOOL ValidKeyAlgid(
                   PNTAGUserList pTmpUser,
                   ALG_ID Algid
                   )
{
    BOOL    fRet = FALSE;

    switch(pTmpUser->dwProvType)
    {
        case PROV_RSA_FULL:
            if ((CALG_SSL3_MASTER == Algid) || (CALG_PCT1_MASTER == Algid))
                goto Ret;
            break;

        case PROV_RSA_SIG:
            if (CALG_RSA_SIGN != Algid)
                goto Ret;
            break;

#ifdef CSP_USE_SSL3
        case PROV_RSA_SCHANNEL:
            if (CALG_RSA_SIGN == Algid)
                goto Ret;
            break;
#endif

        default:
            goto Ret;
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/*
 -  GetSGCDefaultKeyLength
 -
 *  Purpose:
 *                Returns the default key size in pcbKey.
 *
 *  Parameters:
 *               IN      Algid   -  For the key to be created
 *               OUT     pcbKey  -  Size of the key in bytes to generate
 *               OUT     pfPubKey-  TRUE if the Algid is a pub key
 *
 *  Returns:  TRUE on success, FALSE on failure.
 */
BOOL GetSGCDefaultKeyLength(
                            IN ALG_ID Algid,
                            OUT DWORD *pcbKey,
                            OUT BOOL *pfPubKey
                            )
{
    BOOL    fRet = FALSE;

    *pfPubKey = FALSE;

    // determine which crypt algorithm is to be used
    switch (Algid)
    {
#ifdef CSP_USE_SSL3
        case CALG_SSL3_MASTER:
        case CALG_TLS1_MASTER:
            *pcbKey = SSL3_MASTER_KEYSIZE;
            break;
        case CALG_PCT1_MASTER:
            *pcbKey = PCT1_MASTER_KEYSIZE;
            break;
        case CALG_SSL2_MASTER:
            *pcbKey = SSL2_MASTER_KEYSIZE;
            break;
#endif

        case CALG_RSA_KEYX:
            *pcbKey = SGC_RSA_DEF_EXCH_MODLEN;
            *pfPubKey = TRUE;
            break;

        default:
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
            break;
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/*
 -  GetDefaultKeyLength
 -
 *  Purpose:
 *                Returns the default key size in pcbKey.
 *
 *  Parameters:
 *               IN      pTmpUser-  The context info
 *               IN      Algid   -  For the key to be created
 *               OUT     pcbKey  -  Size of the key in bytes to generate
 *               OUT     pfPubKey-  TRUE if the Algid is a pub key
 *
 *  Returns:  TRUE on success, FALSE on failure.
 */
BOOL GetDefaultKeyLength(
                         IN PNTAGUserList pTmpUser,
                         IN ALG_ID Algid,
                         OUT DWORD *pcbKey,
                         OUT BOOL *pfPubKey
                         )
{
    BOOL    fRet = FALSE;

    *pfPubKey = FALSE;

    // determine which crypt algorithm is to be used
    switch (Algid)
    {
#ifdef CSP_USE_RC2
        case CALG_RC2:
#ifdef STRONG
            if (pTmpUser->fNewStrongCSP)
            {
                *pcbKey = RC2_DEF_NEWSTRONG_KEYSIZE;
            }
            else
            {
                *pcbKey = RC2_DEF_KEYSIZE;
            }
#else
            *pcbKey = RC2_DEF_KEYSIZE;
#endif // STRONG
            break;
#endif // CSP_USE_RC2

#ifdef CSP_USE_RC4
        case CALG_RC4:
#ifdef STRONG
            if (pTmpUser->fNewStrongCSP)
            {
                *pcbKey = RC4_DEF_NEWSTRONG_KEYSIZE;
            }
            else
            {
                *pcbKey = RC4_DEF_KEYSIZE;
            }
#else
            *pcbKey = RC4_DEF_KEYSIZE;
#endif // STRONG
            break;
#endif

#ifdef CSP_USE_DES
        case CALG_DES:
            *pcbKey = DES_KEYSIZE;
            break;
#endif

#ifdef STRONG
#ifdef CSP_USE_3DES
        case CALG_3DES_112:
            *pcbKey = DES2_KEYSIZE;
            break;

        case CALG_3DES:
            *pcbKey = DES3_KEYSIZE;
            break;
#endif
#endif // STRONG

#ifdef CSP_USE_SSL3
        case CALG_SSL3_MASTER:
        case CALG_TLS1_MASTER:
            *pcbKey = SSL3_MASTER_KEYSIZE;
            break;
        case CALG_PCT1_MASTER:
            *pcbKey = PCT1_MASTER_KEYSIZE;
            break;
        case CALG_SSL2_MASTER:
            *pcbKey = SSL2_MASTER_KEYSIZE;
            break;
#endif

        case CALG_RSA_KEYX:
#ifdef STRONG
            if (pTmpUser->fNewStrongCSP)
            {
                *pcbKey = RSA_DEF_NEWSTRONG_EXCH_MODLEN;
            }
            else
#endif
            {
                *pcbKey = RSA_DEF_EXCH_MODLEN;
            }
            *pfPubKey = TRUE;
            break;
        case CALG_RSA_SIGN:
#ifdef STRONG
            if (pTmpUser->fNewStrongCSP)
            {
                *pcbKey = RSA_DEF_NEWSTRONG_SIGN_MODLEN;
            }
            else
#endif
            {
                *pcbKey = RSA_DEF_SIGN_MODLEN;
            }
            *pfPubKey = TRUE;
            break;

        default:
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
            break;
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/*
 -  CheckKeyLength
 -
 *  Purpose:
 *                Checks the settable key length and if it is OK then
 *                returns that as the size of key to use (pcbKey).  If
 *                no key length is in dwFlags then the default key size
 *                is returned (pcbKey).  If a settable key size is
 *                specified but is not legal then a failure occurs.
 *
 *  Parameters:
 *               IN      Algid   -  For the key to be created
 *               IN      dwFlags -  Flag value with possible key size
 *               OUT     pcbKey  -  Size of the key in bytes to generate
 *               OUT     pfPubKey-  TRUE if the Algid is a pub key
 *
 *  Returns:  TRUE on success, FALSE on failure.
 */
BOOL CheckKeyLength(
                    IN PNTAGUserList pTmpUser,
                    IN ALG_ID Algid,
                    IN DWORD dwFlags,
                    OUT DWORD *pcbKey,
                    OUT BOOL *pfPubKey
                    )
{
    DWORD   cBits;
    DWORD   cbKey;
    BOOL    fRet = FALSE;

    cBits = dwFlags >> 16;
    if (cBits)
    {
        // settable key sizes must be divisible by 8 (by bytes)
        if (0 != (cBits % 8))
        {
            SetLastError((DWORD) NTE_BAD_FLAGS);
            goto Ret;
        }

        // check if requested size is legal
        cbKey = cBits / 8;
        if ((PROV_RSA_SCHANNEL == pTmpUser->dwProvType) &&
            (0 != pTmpUser->dwSGCFlags))
        {
            if (!FIsLegalSGCKeySize(Algid, cbKey,
                                    FALSE, TRUE, pfPubKey))
            {
                goto Ret;
            }
        }
        else
        {
            // if in France then different key sizes may apply
            if (pTmpUser->Rights & CRYPT_IN_FRANCE)
            {
                // 4th parameter, dwFlags, is used for SGC Exch keys so just
                // pass zero in this case
                if (!FIsLegalFrenchKeySize(Algid, cbKey,
                                           FALSE, dwFlags, pfPubKey))
                {
                    goto Ret;
                }
            }
            else
            {
                if (!FIsLegalKeySize(Algid, cbKey,
                                     FALSE, dwFlags, pfPubKey))
                {
                    goto Ret;
                }
            }
        }
        *pcbKey = cbKey;
    }
    else
    {
        if ((PROV_RSA_SCHANNEL == pTmpUser->dwProvType) &&
            (0 != pTmpUser->dwSGCFlags))
        {
            if (!GetSGCDefaultKeyLength(Algid, pcbKey, pfPubKey))
                goto Ret;
        }
        else
        {
            if (!GetDefaultKeyLength(pTmpUser, Algid, pcbKey, pfPubKey))
                goto Ret;
        }
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/*
 -  CheckSGCSimpleForExport
 -
 *  Purpose:
 *                Check if the SGC key values in the context against the
 *                passed in values to see if an simple blob export with
 *                this key is allowed.
 *
 *
 *  Parameters:
 *               IN      hUID    -  Handle to a CSP
 *               IN      Algid   -  Algorithm identifier
 *               IN      dwFlags -  Flags values
 *               OUT     phKey   -  Handle to a generated key
 *
 *  Returns:
 */
BOOL CheckSGCSimpleForExport(
                             IN PNTAGUserList pTmpUser,
                             IN BSAFE_PUB_KEY *pBsafePubKey
                             )
{
    BYTE    *pb;
    BOOL    fRet = FALSE;

    pb = ((BYTE*)pBsafePubKey) + sizeof(BSAFE_PUB_KEY);
    if (((pBsafePubKey->bitlen / 8) != pTmpUser->cbSGCKeyMod) ||
        (pBsafePubKey->pubexp != pTmpUser->dwSGCKeyExpo) ||
        (0 != memcmp(pb, pTmpUser->pbSGCKeyMod, pTmpUser->cbSGCKeyMod)))
    {
        fRet = FALSE;
        goto Ret;
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/*
 -  CPGenKey
 -
 *  Purpose:
 *                Generate cryptographic keys
 *
 *
 *  Parameters:
 *               IN      hUID    -  Handle to a CSP
 *               IN      Algid   -  Algorithm identifier
 *               IN      dwFlags -  Flags values
 *               OUT     phKey   -  Handle to a generated key
 *
 *  Returns:
 */
BOOL CPGenKey(IN HCRYPTPROV hUID,
              IN ALG_ID Algid,
              IN DWORD dwFlags,
              OUT HCRYPTKEY * phKey)
{
    PNTAGUserList   pTmpUser;
    PNTAGKeyList    pTmpKey = NULL;
    DWORD           dwRights = 0;
    BYTE            rgbRandom[MAX_KEY_SIZE];
    int             localAlgid;
    DWORD           cbKey;
    BOOL            fPubKey = FALSE;
    BOOL            fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputGenKey(TRUE, FALSE, hUID, Algid, dwFlags, phKey);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if ((dwFlags & ~(CRYPT_EXPORTABLE | CRYPT_USER_PROTECTED |
                     CRYPT_CREATE_SALT | CRYPT_NO_SALT |
                     KEY_LENGTH_MASK | CRYPT_SGCKEY)) != 0)
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    switch (Algid)
    {
        case AT_KEYEXCHANGE:
            localAlgid = CALG_RSA_KEYX;
            break;

        case AT_SIGNATURE:
            localAlgid = CALG_RSA_SIGN;
            break;

        default:
            localAlgid = Algid;
            break;
    }

    if ((pTmpUser = (PNTAGUserList) NTLCheckList (hUID, USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    if (!ValidKeyAlgid(pTmpUser, localAlgid))
    {
        SetLastError((DWORD)NTE_BAD_ALGID);
        goto Ret;
    }

    // check if the size of the key is set in the dwFlags parameter
    if (!CheckKeyLength(pTmpUser, localAlgid, dwFlags, &cbKey, &fPubKey))
        goto Ret;

    if (fPubKey)
    {
        if(NTF_SUCCEED != ReGenKey(hUID, dwFlags,
                                   (localAlgid == CALG_RSA_KEYX) ?
                                          NTPK_USE_EXCH : NTPK_USE_SIG,
                                   phKey,
                                   cbKey * 8))
        {
            goto Ret;
        }
    }
    else
    {
        // generate the random key
        if (!FIPS186GenRandom(&pTmpUser->hRNGDriver,
                          &pTmpUser->ContInfo.pbRandom,
                          &pTmpUser->ContInfo.ContLens.cbRandom,
                          rgbRandom, cbKey))
        {
            SetLastError((DWORD) NTE_FAIL);
            goto Ret;
        }

#ifdef CSP_USE_DES
        if ((CALG_DES == localAlgid) || (CALG_3DES_112 == localAlgid) ||
            (CALG_3DES == localAlgid))
        {
            if (dwFlags & CRYPT_CREATE_SALT)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }
            desparityonkey(rgbRandom, cbKey);
        }
#endif
#ifdef CSP_USE_SSL3
        else if (CALG_SSL3_MASTER == localAlgid)
        {
            // set the first byte to 0x03 and the second to 0x00
            rgbRandom[0] = 0x03;
            rgbRandom[1] = 0x00;
        }
        else if (CALG_TLS1_MASTER == localAlgid)
        {
            // set the first byte to 0x03 and the second to 0x01
            rgbRandom[0] = 0x03;
            rgbRandom[1] = 0x01;
        }
#endif
        // check if the key is CRYPT_EXPORTABLE
        if (dwFlags & CRYPT_EXPORTABLE)
            dwRights = CRYPT_EXPORTABLE;

        if ((pTmpKey = MakeNewKey(localAlgid,
                                  dwRights,
                                  cbKey,
                                  hUID,
                                  rgbRandom,
                                  FALSE)) == NULL)
        {
            goto Ret;          // error already set
        }

        if (dwFlags & CRYPT_CREATE_SALT)
        {
#ifdef STRONG
            if (pTmpUser->fNewStrongCSP)
            {
                pTmpKey->cbSaltLen = DEFAULT_SALT_NEWSTRONG_LENGTH;
            }
            else
            {
                pTmpKey->cbSaltLen = DEFAULT_SALT_LENGTH;
            }
#else
            pTmpKey->cbSaltLen = DEFAULT_SALT_LENGTH;
#endif // STRONG

            if (!FIPS186GenRandom(&pTmpUser->hRNGDriver,
                              &pTmpUser->ContInfo.pbRandom,
                              &pTmpUser->ContInfo.ContLens.cbRandom,
                              pTmpKey->rgbSalt, pTmpKey->cbSaltLen))
            {
                SetLastError((DWORD) NTE_FAIL);
                goto Ret;
            }
        }

        if (NTLMakeItem(phKey, KEY_HANDLE, (void *)pTmpKey) == NTF_FAILED)
        {
            goto Ret;          // error already set
        }

        // if 40bit key + no mention of salt, set zeroized salt for RSABase compatibility
        if ((5 == cbKey) && (!(dwFlags & CRYPT_NO_SALT)) &&
            (!(dwFlags & CRYPT_CREATE_SALT)) &&
            (CALG_SSL3_MASTER != Algid) && (CALG_TLS1_MASTER != Algid) &&
            (CALG_PCT1_MASTER != Algid) && (CALG_SSL2_MASTER != Algid))
        {
            if (!MakeKeyRSABaseCompatible(hUID, *phKey))
            {
                goto Ret;
            }
        }
    }

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputGenKey(FALSE, fRet, 0, 0, 0, phKey);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (FALSE == fRet)
    {
        if (pTmpKey)
            FreeNewKey(pTmpKey);
    }

    return fRet;
}


/*
 -  CPDeriveKey
 -
 *  Purpose:
 *                Derive cryptographic keys from base data
 *
 *
 *  Parameters:
 *               IN      hUID       -  Handle to a CSP
 *               IN      Algid      -  Algorithm identifier
 *               IN      hBaseData  -  Handle to hash of base data
 *               IN      dwFlags    -  Flags values
 *               OUT     phKey      -  Handle to a generated key
 *
 *  Returns:
 */
BOOL CPDeriveKey(IN HCRYPTPROV hUID,
                 IN ALG_ID Algid,
                 IN HCRYPTHASH hBaseData,
                 IN DWORD dwFlags,
                 OUT HCRYPTKEY * phKey)
{
    PNTAGUserList   pTmpUser = NULL;
    PNTAGKeyList    pTmpKey = NULL;
    DWORD           dwRights = 0;
    BYTE            rgbRandom[32];
    BYTE            rgbBaseVal[NT_HASH_BYTES * 2];
    HCRYPTHASH      h1 = 0;
    HCRYPTHASH      h2 = 0;
    BYTE            rgbBuff1[64];
    BYTE            rgbBuff2[64];
    BYTE            rgbHash1[NT_HASH_BYTES];
    BYTE            rgbHash2[NT_HASH_BYTES];
    DWORD           cb1;
    DWORD           cb2;
    DWORD           i;
    PNTAGHashList   pTmpHash;
    DWORD           temp;
    BOOL            fPubKey = FALSE;
    DWORD           cbKey;
    BOOL            fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputDeriveKey(TRUE, FALSE, hUID, Algid, hBaseData, dwFlags, phKey);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if ((dwFlags & ~(CRYPT_EXPORTABLE | CRYPT_USER_PROTECTED |
             CRYPT_CREATE_SALT | CRYPT_NO_SALT | CRYPT_SERVER |
             KEY_LENGTH_MASK)) != 0)
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    if ((pTmpUser = (PNTAGUserList) NTLCheckList (hUID, USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    if (!ValidKeyAlgid(pTmpUser, Algid))
    {
        SetLastError((DWORD)NTE_BAD_ALGID);
        goto Ret;
    }

    if (NULL == (pTmpHash = (PNTAGHashList) NTLValidate(hBaseData, hUID,
                                                        HASH_HANDLE)))
    {
        // NTLValidate doesn't know what error to set
        // so it set NTE_FAIL -- fix it up.
        if (GetLastError() == NTE_FAIL)
        {
            SetLastError((DWORD) NTE_BAD_HASH);
        }
        goto Ret;
    }

#ifdef CSP_USE_SSL3
    // if the hash is for secure channel usage then go to that derive function
    if (CALG_SCHANNEL_MASTER_HASH == pTmpHash->Algid)
    {
        if (!SecureChannelDeriveKey(pTmpUser, pTmpHash, Algid, dwFlags, phKey))
        {
            goto Ret;
        }
        else
        {
            fRet = NTF_SUCCEED;
            goto Ret;
        }
    }
#endif // CSP_USE_SSL3

    // check if the size of the key is set in the dwFlags parameter
    if (!CheckKeyLength(pTmpUser, Algid, dwFlags, &cbKey, &fPubKey))
    {
        goto Ret;
    }

    if (fPubKey)
    {
        SetLastError((DWORD) NTE_BAD_ALGID);
        goto Ret;
    }

    if (pTmpHash->HashFlags & HF_VALUE_SET)
    {
        SetLastError((DWORD) NTE_BAD_HASH);
        goto Ret;
    }

    memset(rgbBaseVal, 0, NT_HASH_BYTES);

    temp = NT_HASH_BYTES;
    if (!CPGetHashParam(hUID, hBaseData, HP_HASHVAL, rgbBaseVal, &temp, 0))
    {
        goto Ret;
    }

#ifdef CSP_USE_3DES
    if (CALG_3DES == Algid)
    {
        // the hash value is not long enough so we must expand it
        if (!CPCreateHash(hUID, pTmpHash->Algid, 0, 0, &h1))
        {
            goto Ret;
        }
        if (!CPCreateHash(hUID, pTmpHash->Algid, 0, 0, &h2))
        {
            goto Ret;
        }

        // set up the two buffers to be hashed
        memset(rgbBuff1, 0x36, sizeof(rgbBuff1));
        memset(rgbBuff2, 0x5C, sizeof(rgbBuff2));
        for (i=0;i<temp;i++)
        {
            rgbBuff1[i] ^= rgbBaseVal[i];
            rgbBuff2[i] ^= rgbBaseVal[i];
        }

        // hash the two buffers
        if (!CPHashData(hUID, h1, rgbBuff1, sizeof(rgbBuff1), 0))
        {
            goto Ret;
        }
        if (!CPHashData(hUID, h2, rgbBuff2, sizeof(rgbBuff2), 0))
        {
            goto Ret;
        }

        // finish the hashes and copy them into BaseVal
        memset(rgbHash1, 0, sizeof(rgbHash1));
        cb1 = sizeof(rgbHash1);
        if (!CPGetHashParam(hUID, h1, HP_HASHVAL, rgbHash1, &cb1, 0))
        {
            goto Ret;
        }
        memcpy(rgbBaseVal, rgbHash1, cb1);

        memset(rgbHash2, 0, sizeof(rgbHash2));
        cb2 = sizeof(rgbHash2);
        if (!CPGetHashParam(hUID, h2, HP_HASHVAL, rgbHash2, &cb2, 0))
        {
            goto Ret;
        }
        memcpy(rgbBaseVal + cb1, rgbHash2, cb2);
    }
#endif

    memcpy(rgbRandom, rgbBaseVal, cbKey);

    // check if the key is CRYPT_EXPORTABLE
    if (dwFlags & CRYPT_EXPORTABLE)
        dwRights = CRYPT_EXPORTABLE;

    if (NULL == (pTmpKey = MakeNewKey(Algid,
                                      dwRights,
                                      cbKey,
                                      hUID,
                                      rgbRandom,
                                      FALSE)))
    {
        goto Ret;
    }

    if (dwFlags & CRYPT_CREATE_SALT)
    {
#ifdef STRONG
        if (pTmpUser->fNewStrongCSP)
        {
            pTmpKey->cbSaltLen = DEFAULT_SALT_NEWSTRONG_LENGTH;
        }
        else
        {
            pTmpKey->cbSaltLen = DEFAULT_SALT_LENGTH;
        }
#else
        pTmpKey->cbSaltLen = DEFAULT_SALT_LENGTH;
#endif // STRONG

        memcpy(pTmpKey->rgbSalt, rgbBaseVal+cbKey, pTmpKey->cbSaltLen);
    }

    if (NTLMakeItem(phKey, KEY_HANDLE, (void *)pTmpKey) == NTF_FAILED)
    {
        goto Ret;          // error already set
    }

    // if 40bit key + no mention of salt, set zeroized salt for RSABase compatibility
    if ((5 == cbKey) && (!(dwFlags & CRYPT_NO_SALT)) &&
        (!(dwFlags & CRYPT_CREATE_SALT)) )
    {
        if (!MakeKeyRSABaseCompatible(hUID, *phKey))
        {
            goto Ret;
        }
    }

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputDeriveKey(FALSE, fRet, 0, 0, 0, 0, phKey);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if ((NTF_SUCCEED != fRet) && pTmpKey)
        FreeNewKey(pTmpKey);
    if (pTmpUser && h1)
        CPDestroyHash(hUID, h1);
    if (pTmpUser && h2)
        CPDestroyHash(hUID, h2);

    return fRet;
}

BOOL MyGetComputerName(IN PNTAGUserList pUser)
{
    BOOL    fRet = FALSE;
    int i;
    pUser->cbMachineName = sizeof(pUser->rgszMachineName);
    for (i=0; i<(int)pUser->cbMachineName; ++i)
    {
        pUser->rgszMachineName[i] = 'X';
    }
    if (i > 0)
    {
        pUser->rgszMachineName[i-1] = '\0';
    }
    fRet = TRUE;
    return fRet;
}

// RC4 is used so both encryption and decryption are done with this
// function
DWORD EncryptOpaqueBlob(
                        PNTAGUserList pUser,
                        BYTE *pbData,
                        DWORD cbData
                        )
{
    BYTE            rgbStart[] = {0x3A, 0x18, 0x97, 0xC2,
                                  0x44, 0x0B, 0x1E, 0x49};
    A_SHA_CTX       SHACtxt;
    BYTE            rgbHash[A_SHA_DIGEST_LEN];
    RC4_KEYSTRUCT   KeyStruct;
    DWORD           dwErr = 0;

    memset(&SHACtxt, 0, sizeof(SHACtxt));
    memset(rgbHash, 0, sizeof(rgbHash));
    memset(&KeyStruct, 0, sizeof(KeyStruct));
    
    if (!MyGetComputerName(pUser))
    {
        dwErr = GetLastError();
        goto Ret;
    }

    A_SHAInit(&SHACtxt);
    A_SHAUpdate(&SHACtxt, rgbStart, sizeof(rgbStart));
    A_SHAUpdate(&SHACtxt, pUser->rgszMachineName, strlen(pUser->rgszMachineName));
    A_SHAFinal(&SHACtxt, rgbHash);

    rc4_key(&KeyStruct, 16, rgbHash);

    rc4(&KeyStruct, cbData, pbData);
Ret:
    return dwErr;
}

BOOL ExportOpaqueBlob(
                      PNTAGUserList pUser,
                      PNTAGKeyList pKey,
                      DWORD dwFlags,
                      BYTE *pbData,
                      DWORD *pcbData
                      )
{
    DWORD           cb;
    DWORD           cbIndex;
    PNTAGKeyList    pTmpKey;
    BYTE            *pbSalt;
    BOOL            fRet = FALSE;

    // make sure the key is a symmetric key
    if ((CALG_RSA_SIGN == pKey->Algid) || (CALG_RSA_KEYX == pKey->Algid))
    {
        SetLastError((DWORD)NTE_BAD_KEY);
        goto Ret;
    }

    // calculate the length of the blob
    cb = sizeof(BLOBHEADER) + sizeof(NTAGKeyList) + pKey->cbKeyLen +
         pKey->cbDataLen + pKey->cbSaltLen;

    if (pbData == NULL || *pcbData < cb)
    {
        if (pbData == NULL)
        {
            fRet = TRUE;
            goto Ret;
        }
        SetLastError(ERROR_MORE_DATA);
        goto Ret;
    }

    // set up the blob
    pTmpKey = (PNTAGKeyList)((BYTE*)pbData + sizeof(BLOBHEADER));
    memcpy(pTmpKey, pKey, sizeof(NTAGKeyList));

    pTmpKey->hUID = 0;
    cbIndex = sizeof(NTAGKeyList) + sizeof(BLOBHEADER);
    pTmpKey->pKeyValue = pbData + cbIndex;
    memcpy(pTmpKey->pKeyValue, pKey->pKeyValue, pTmpKey->cbKeyLen);
    cbIndex += pTmpKey->cbKeyLen;

    pTmpKey->pData = pbData + cbIndex;
    memcpy(pTmpKey->pData, pKey->pData, pTmpKey->cbDataLen);
    cbIndex += pTmpKey->cbDataLen;

    pbSalt = pbData + cbIndex;
    memcpy(pbSalt, pKey->rgbSalt, pTmpKey->cbSaltLen);
    cbIndex += pTmpKey->cbSaltLen;

    // encrypt the blob
    if (0 != EncryptOpaqueBlob(pUser,
                               pbData + sizeof(BLOBHEADER),
                               cb - sizeof(BLOBHEADER)))
    {
        SetLastError((DWORD)NTE_FAIL);
        goto Ret;
    }

    fRet = TRUE;
Ret:
    *pcbData = cb;
    return fRet;
}

/*
 -  GetRC4KeyForSymWrap
 -
 *  Purpose:
 *            RC4 or more precisely stream ciphers are not supported by the CMS spec
 *            on symmetric key wrapping so we had to do something proprietary since
 *            we want to support RC4 for applications other than SMIME
 *
 *
 *  Parameters:
 *               IN  pTmpUser   - Pointer to the context
 *               IN  pbSalt     - Pointer to the 8 byte salt buffer
 *               IN  pKey       - Pointer to the orignial key
 *               OUT ppNewKey   - Pointer to a pointer to the new key
 */
DWORD GetRC4KeyForSymWrap(
                          IN PNTAGUserList pTmpUser,
                          IN BYTE *pbSalt,
                          IN PNTAGKeyList pKey,
                          OUT PNTAGKeyList *ppNewKey
                          )
{
    BYTE            rgbSalt[MAX_SALT_LEN];
    DWORD           dwErr = 0;

    // duplicate the key
    if (!CopyKey(pKey, ppNewKey))
    {
        dwErr = GetLastError();
        goto Ret;
    }

    // set the value as salt + current salt
    (*ppNewKey)->cbSaltLen += 8;
    memcpy((*ppNewKey)->rgbSalt + ((*ppNewKey)->cbSaltLen - 8), pbSalt, 8);
    if (!CPInflateKey(*ppNewKey))
    {
        dwErr = GetLastError();
    }
Ret:
    return dwErr;
}

/*
 -  GetSymmetricKeyChecksum
 -
 *  Purpose:
 *                Calculates the checksum for a symmetric key which is to be
 *                wrapped with another symmetric key.  This should meet the
 *                CMS specification
 *
 *
 *  Parameters:
 *               IN  pKey       - Pointer to the key
 *               OUT pbChecksum - Pointer to the 8 byte checksum
 */
void GetSymmetricKeyChecksum(
                             IN BYTE *pbKey,
                             IN DWORD cbKey,
                             OUT BYTE *pbChecksum
                             )
{
    A_SHA_CTX   SHACtx;
    BYTE        rgb[A_SHA_DIGEST_LEN];

    A_SHAInit(&SHACtx);
    A_SHAUpdate(&SHACtx, pbKey, cbKey);
    A_SHAFinal(&SHACtx, rgb);

    memcpy(pbChecksum, rgb, 8);
}

/*
 -  WrapSymKey
 -
 *  Purpose:
 *          Wrap a symmetric key with another symmetric key.  This should
 *          meet the CMS specification for symmetric key wrapping.
 *
 *  Parameters:
 *               IN  pTmpUser   - Pointer to the user context
 *               IN  pKey       - Pointer to the key to be wrapped
 *               IN  pWrapKey   - Pointer to the key to be used for wrapping
 *               IN OUT pbBlob  - Pointer to the resulting blob (may be NULL
 *                                to get the length)
 *               IN OUT pcbBlob - Pointer to the length of the blob buffer
 */
DWORD WrapSymKey(
                 IN PNTAGUserList pTmpUser,
                 IN PNTAGKeyList pKey,
                 IN PNTAGKeyList pWrapKey,
                 IN OUT BYTE *pbBlob,
                 IN OUT DWORD *pcbBlob
                 )
{
    DWORD           cb = 0;
    DWORD           cbIndex = 0;
    DWORD           cbPad = 0;
    BLOBHEADER      *pBlobHdr;
    ALG_ID          *pAlgid;
    BYTE            rgbTmp1[49]; // 1 length + 8 padding + 8 checksum + 8 IV + 24 max key
    BYTE            rgbTmp2[49]; // 1 length + 8 padding + 8 checksum + 8 IV + 24 max key
    BYTE            rgbIV[8];
    PNTAGKeyList    pLocalWrapKey = NULL;
    BOOL            fAlloc = FALSE;
    DWORD           i;
    DWORD           dwErr = 0;

    memset(rgbTmp1, 0, sizeof(rgbTmp1));
    memset(rgbTmp2, 0, sizeof(rgbTmp2));
    memset(rgbIV, 0, sizeof(rgbIV));

    // both keys must be supported symmetric keys
    if (UnsupportedSymKey(pKey) || UnsupportedSymKey(pWrapKey))
    {
        dwErr = (DWORD)NTE_BAD_KEY;
        goto Ret;
    }

    if ((!FIsLegalKey(pTmpUser, pKey, FALSE)) ||
        (!FIsLegalKey(pTmpUser, pWrapKey, FALSE)))
    {
        dwErr = (DWORD) NTE_BAD_KEY;
        goto Ret;
    }

    // Check if we should do an auto-inflate
    if (pWrapKey->pData == NULL)
    {
        if (!CPInflateKey(pWrapKey))
        {
            dwErr = GetLastError();
            goto Ret;
        }
    }

    // calculate how long the encrypted data is going to be,
    // 1 byte for length, up to 8 bytes for pad and 8 bytes
    // for the checksum and 8 bytes for the IV
    if ((CALG_RC4 == pKey->Algid) || (CALG_RC2 == pKey->Algid)) // variable key lengths
    {
        cbPad = 8 - ((pKey->cbKeyLen + 1) % 8);
        cb += pKey->cbKeyLen + 9 + cbPad + 8;
        
        // place the length in the buffer
        rgbTmp1[0] = (BYTE)pKey->cbKeyLen;
        cbIndex += 1;
    }
    // calculate how long the encrypted data is going to be,
    // up to 8 bytes for salt and 8 bytes for the checksum and 8 bytes
    // for the IV
    else
    {
        cb += pKey->cbKeyLen + 16;
    }

    // check if just looking for a length
    if (NULL == pbBlob)
    {
        cbIndex = cb;
        dwErr = 0;
        goto Ret;
    }
    else if (*pcbBlob < (cb + sizeof(BLOBHEADER) + sizeof(ALG_ID)))
    {
        cbIndex = cb;
        dwErr = ERROR_MORE_DATA;
        goto Ret;
    }

    // copy the key data
    memcpy(rgbTmp1 + cbIndex, pKey->pKeyValue, pKey->cbKeyLen);
    cbIndex += pKey->cbKeyLen;

    // generate random pad
    if (cbPad)
    {
        if (!FIPS186GenRandom(&pTmpUser->hRNGDriver,
                              &pTmpUser->ContInfo.pbRandom,
                              &pTmpUser->ContInfo.ContLens.cbRandom,
                              rgbTmp1 + cbIndex, cbPad))
        {
            dwErr = GetLastError();
            goto Ret;
        }
        cbIndex += cbPad;
    }

    // get the checksum
    GetSymmetricKeyChecksum(rgbTmp1,
                            cbIndex,
                            rgbTmp1 + cbIndex);
    cbIndex += 8;

    if (!FIPS186GenRandom(&pTmpUser->hRNGDriver,
                          &pTmpUser->ContInfo.pbRandom,
                          &pTmpUser->ContInfo.ContLens.cbRandom,
                          rgbIV, 8))
    {
        dwErr = GetLastError();
        goto Ret;
    }

    // set the IV if the algorithm is not RC4
    if (CALG_RC4 != pWrapKey->Algid)
    {
        memcpy(pWrapKey->IV, rgbIV, 8);
        pWrapKey->InProgress = FALSE;
        pLocalWrapKey = pWrapKey;
    }
    else
    {
        // RC4 or more precisely stream ciphers are not supported by the CMS spec
        // on symmetric key wrapping so we had to do something proprietary since
        // we want to support RC4 for applications other than SMIME
        if (0 != (dwErr = GetRC4KeyForSymWrap(pTmpUser,
                                              rgbIV,
                                              pWrapKey,
                                              &pLocalWrapKey)))
        {
            goto Ret;
        }
        fAlloc = TRUE;
    }

    // encrypt the key blob data
    if (!SymEncrypt(pLocalWrapKey, FALSE, rgbTmp1,
                    &cbIndex, cbIndex))
    {
        dwErr = GetLastError();
        goto Ret;
    }

    // concatenate the initial ciphertext with the IV
    memcpy(rgbTmp2, rgbIV, 8);
    memcpy(rgbTmp2 + 8, rgbTmp1, cbIndex);
    cbIndex += 8;

    // byte reverse the ciphertext + IV buffer
    for (i = 0; i < cbIndex; i++)
    {
        rgbTmp1[i] = rgbTmp2[cbIndex - (i + 1)];
    }

    // encrypt the key blob data again with the hardcoded IV
    if (CALG_RC4 != pWrapKey->Algid)
    {
        memcpy(pWrapKey->IV, rgbSymmetricKeyWrapIV, 8);
        pWrapKey->InProgress = FALSE;
    }
    else
    {
        if (fAlloc && pLocalWrapKey)
        {
            FreeNewKey(pLocalWrapKey);
            pLocalWrapKey = NULL;
            fAlloc = FALSE;
        }

        // RC4 or more precisely stream ciphers are not supported by the CMS spec
        // on symmetric key wrapping so we had to do something proprietary since
        // we want to support RC4 for applications other than SMIME
        if (0 != (dwErr = GetRC4KeyForSymWrap(pTmpUser,
                                              rgbSymmetricKeyWrapIV,
                                              pWrapKey,
                                              &pLocalWrapKey)))
        {
            goto Ret;
        }
        fAlloc = TRUE;
    }

    if (!SymEncrypt(pLocalWrapKey, FALSE, rgbTmp1,
                    &cbIndex, cbIndex))
    {
        dwErr = GetLastError();
        goto Ret;
    }

    // set the header info
    pBlobHdr = (BLOBHEADER*)pbBlob;
    pBlobHdr->aiKeyAlg = pKey->Algid;

    pAlgid = (ALG_ID*)(pbBlob + sizeof(BLOBHEADER));
    *pAlgid = pWrapKey->Algid;

    memcpy(pbBlob + sizeof(BLOBHEADER) + sizeof(ALG_ID),
           rgbTmp1, cbIndex);
Ret:
    if (fAlloc && pLocalWrapKey)
    {
        FreeNewKey(pLocalWrapKey);
    }

    memset(rgbTmp1, 0, sizeof(rgbTmp1));
    memset(rgbTmp2, 0, sizeof(rgbTmp2));
    memset(rgbIV, 0, sizeof(rgbIV));

    *pcbBlob = cbIndex + sizeof(BLOBHEADER) + sizeof(ALG_ID);

    return dwErr;
}

/*
 -  UnWrapSymKey
 -
 *  Purpose:
 *          Unwrap a symmetric key with another symmetric key.  This should
 *          meet the CMS specification for symmetric key wrapping.
 *
 *  Parameters:
 *               IN  pTmpUser   - Pointer to the user context
 *               IN  pWrapKey   - Pointer to the key to be used for unwrapping
 *               IN  pbBlob     - Pointer to the blob to be unwrapped
 *               IN  cbBlob     - The length of the blob buffer
 *               OUT phKey      - Handle to the unwrapped key
 */
DWORD UnWrapSymKey(
                   IN HCRYPTPROV hUID,
                   IN PNTAGUserList pTmpUser,
                   IN PNTAGKeyList pWrapKey,
                   IN BYTE *pbBlob,
                   IN DWORD cbBlob,
                   IN DWORD dwFlags,
                   OUT HCRYPTKEY *phKey
                   )
{
    DWORD           cb = 0;
    DWORD           cbIndex = 0;
    DWORD           cbKey = 0;
    BYTE            rgbChecksum[8];
    BLOBHEADER      *pBlobHdr = (BLOBHEADER*)pbBlob;
    ALG_ID          *pAlgid;
    BYTE            rgbTmp1[49];  // 1 length + 8 padding + 8 checksum + 8 IV + 24 max key
    BYTE            rgbTmp2[49];  // 1 length + 8 padding + 8 checksum + 8 IV + 24 max key
    DWORD           dwRights = 0;
    PNTAGKeyList    pTmpKey = NULL;
    PNTAGKeyList    pLocalWrapKey = NULL;
    BOOL            fAlloc = FALSE;
    DWORD           i;
    BOOL            fPubKey;
    DWORD           dwErr = 0;

    memset(rgbTmp1, 0, sizeof(rgbTmp1));
    memset(rgbTmp2, 0, sizeof(rgbTmp2));

    cb = cbBlob - (sizeof(BLOBHEADER) + sizeof(ALG_ID));
    if ((sizeof(rgbTmp1) < cb) || (0 != (cb % 8)))
    {
        dwErr = (DWORD)NTE_BAD_DATA;
        goto Ret;
    }

    // both keys must be supported symmetric keys
    if (UnsupportedSymKey(pWrapKey) || UnsupportedSymKey(pWrapKey))
    {
        dwErr = (DWORD)NTE_BAD_KEY;
        goto Ret;
    }

    if (!FIsLegalKey(pTmpUser, pWrapKey, FALSE))
    {
        dwErr = (DWORD) NTE_BAD_KEY;
        goto Ret;
    }

    // check the wrapping key ALG_ID
    pAlgid = (ALG_ID*)(pbBlob + sizeof(BLOBHEADER));
    if (pWrapKey->Algid != *pAlgid)
    {
        dwErr = (DWORD) NTE_BAD_KEY;
        goto Ret;
    }

    // Check if we should do an auto-inflate
    if (pWrapKey->pData == NULL)
    {
        if (!CPInflateKey(pWrapKey))
        {
            dwErr = GetLastError();
            goto Ret;
        }
    }

    // set the hardcoded IV
    if (CALG_RC4 != pWrapKey->Algid)
    {
        memcpy(pWrapKey->IV, rgbSymmetricKeyWrapIV, 8);
        pWrapKey->InProgress = FALSE;
        pLocalWrapKey = pWrapKey;
    }
    else
    {
        // RC4 or more precisely stream ciphers are not supported by the CMS spec
        // on symmetric key wrapping so we had to do something proprietary since
        // we want to support RC4 for applications other than SMIME
        if (0 != (dwErr = GetRC4KeyForSymWrap(pTmpUser,
                                              rgbSymmetricKeyWrapIV,
                                              pWrapKey,
                                              &pLocalWrapKey)))
        {
            goto Ret;
        }
        fAlloc = TRUE;
    }

    memcpy(rgbTmp1, pbBlob + sizeof(BLOBHEADER) + sizeof(ALG_ID), cb);

    // decrypt the key blob data
    if (!SymDecrypt(pLocalWrapKey, 0, FALSE, rgbTmp1, &cb))
    {
        dwErr = GetLastError();
        goto Ret;
    }

    // byte reverse the plaintext + IV buffer
    for (i = 0; i < cb; i++)
    {
        rgbTmp2[i] = rgbTmp1[cb - (i + 1)];
    }

    // set the IV if the algorithm is not RC4
    cb -= 8;
    if (CALG_RC4 != pWrapKey->Algid)
    {
        memcpy(pWrapKey->IV, rgbTmp2, 8);
        pWrapKey->InProgress = FALSE;
    }
    else
    {
        if (fAlloc && pLocalWrapKey)
        {
            FreeNewKey(pLocalWrapKey);
            pLocalWrapKey = NULL;
            fAlloc = FALSE;
        }

        // RC4 or more precisely stream ciphers are not supported by the CMS spec
        // on symmetric key wrapping so we had to do something proprietary since
        // we want to support RC4 for applications other than SMIME
        if (0 != (dwErr = GetRC4KeyForSymWrap(pTmpUser,
                                              rgbTmp2,
                                              pWrapKey,
                                              &pLocalWrapKey)))
        {
            goto Ret;
        }
        fAlloc = TRUE;
    }

    // decrypt the key blob data again
    if (!SymDecrypt(pLocalWrapKey, 0, FALSE, rgbTmp2 + 8, &cb))
    {
        dwErr = GetLastError();
        goto Ret;
    }

    // check the length of the key
    if ((CALG_RC4 == pBlobHdr->aiKeyAlg) || (CALG_RC2 == pBlobHdr->aiKeyAlg)) // variable key lengths
    {
        cbKey = (DWORD)rgbTmp2[8];
        cbIndex += 1;
    }
    else
    {
        if (!GetDefaultKeyLength(pTmpUser,
                                 pBlobHdr->aiKeyAlg,
                                 &cbKey,
                                 &fPubKey))
        {
            dwErr = (DWORD)NTE_BAD_ALGID;
            goto Ret;
        }

    }

    // get the checksum and make sure it matches
    cb -= 8;
    GetSymmetricKeyChecksum(rgbTmp2 + 8, cb, rgbChecksum);
    if (memcmp(rgbChecksum, rgbTmp2 + 8 + cb, sizeof(rgbChecksum)))
    {
        dwErr = (DWORD)NTE_BAD_DATA;
        goto Ret;
    }

    // check if the key is to be exportable
    if (dwFlags & CRYPT_EXPORTABLE)
        dwRights = CRYPT_EXPORTABLE;

    if ((pTmpKey = MakeNewKey(pBlobHdr->aiKeyAlg,
                              dwRights,
                              cbKey,
                              hUID,
                              rgbTmp2 + cbIndex + 8,
                              FALSE)) == NULL)
    {
        dwErr = GetLastError();
        goto Ret;
    }

    // check keylength...
    if (!FIsLegalKey(pTmpUser, pTmpKey, TRUE))
    {
        dwErr = GetLastError();
        goto Ret;
    }

    if (NTLMakeItem(phKey, KEY_HANDLE, (void *)pTmpKey) == NTF_FAILED)
    {
        dwErr = GetLastError();
        goto Ret;          // error already set
    }

    // if 40 bit key + no mention of salt, set zeroized salt for RSABase compatibility
    if ((5 == pTmpKey->cbKeyLen) && (!(dwFlags & CRYPT_NO_SALT)))
    {
        if (!MakeKeyRSABaseCompatible(hUID, *phKey))
        {
            dwErr = GetLastError();
            goto Ret;
        }
    }
Ret:
    if (fAlloc && pLocalWrapKey)
    {
        FreeNewKey(pLocalWrapKey);
    }

    memset(rgbTmp1, 0, sizeof(rgbTmp1));
    memset(rgbTmp2, 0, sizeof(rgbTmp2));

    if ((0 != dwErr) && pTmpKey)
        FreeNewKey(pTmpKey);

    return dwErr;
}

/*
 -  CPExportKey
 -
 *  Purpose:
 *                Export cryptographic keys out of a CSP in a secure manner
 *
 *
 *  Parameters:
 *               IN  hUID       - Handle to the CSP user
 *               IN  hKey       - Handle to the key to export
 *               IN  hPubKey    - Handle to the exchange public key value of
 *                                the destination user
 *               IN  dwBlobType - Type of key blob to be exported
 *               IN  dwFlags -    Flags values
 *               OUT pbData -     Key blob data
 *               OUT pdwDataLen - Length of key blob in bytes
 *
 *  Returns:
 */
BOOL CPExportKey(IN HCRYPTPROV hUID,
                 IN HCRYPTKEY hKey,
                 IN HCRYPTKEY hPubKey,
                 IN DWORD dwBlobType,
                 IN DWORD dwFlags,
                 OUT BYTE *pbData,
                 OUT DWORD *pdwDataLen)
{
    // miscellaneous variables
    DWORD           dwLen;
    NTSimpleBlob    *pSimpleHeader;
    BLOBHEADER      *pPreHeader;
    BLOBHEADER      shScratch;
    RSAPUBKEY       *pExpPubKey;
    BSAFE_PUB_KEY   *pBsafePubKey;
    BSAFE_PUB_KEY   *pPublicKey;
    DWORD           PubKeyLen;
    BSAFE_PRV_KEY   *pPrvKey = NULL;
    DWORD           PrvKeyLen = 0;
    BSAFE_PUB_KEY   *pTmpPubKey;
    BSAFE_PRV_KEY   *pTmpPrvKey;
    DWORD           cbPrivateBlob = 0;
    PBYTE           pbPrivateBlob = NULL;
    LPSTR           pszRegValue;
    DWORD           cb = 0;
    BOOL            fExportable = FALSE;
    DWORD           dwErr = 0;
    BOOL            fRet = NTF_FAILED;

    // temporary variables for pointing to user and key records
    PNTAGKeyList        pTmpKey;
    PNTAGKeyList        pPubKey;
    PNTAGUserList       pTmpUser;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputExportKey(TRUE, FALSE, hUID, hKey, hPubKey, dwBlobType,
                            dwFlags, pbData, pdwDataLen);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (0 != (dwFlags & ~(CRYPT_SSL2_FALLBACK | CRYPT_DESTROYKEY | CRYPT_OAEP)))
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    if (pdwDataLen == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    if (((PUBLICKEYBLOB == dwBlobType) || (OPAQUEKEYBLOB == dwBlobType)) &&
        (0 != hPubKey))
    {
        SetLastError((DWORD) NTE_BAD_PUBLIC_KEY);
        goto Ret;
    }

    // check the user identification
    if ((pTmpUser = (PNTAGUserList) NTLCheckList (hUID, USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    // check if the user is just looking for a length.  If so,
    // use a scratchpad to construct a pseudoblob.

    if ((pbData != NULL) && (*pdwDataLen > sizeof(BLOBHEADER)))
        pPreHeader = (BLOBHEADER *)pbData;
    else
        pPreHeader = &shScratch;

    pPreHeader->bType = (BYTE)(dwBlobType & 0xff);
    pPreHeader->bVersion = CUR_BLOB_VERSION;
    pPreHeader->reserved = 0;

    pTmpKey = (PNTAGKeyList)NTLValidate((HNTAG)hKey, hUID,
                        HNTAG_TO_HTYPE(hKey));

    if (pTmpKey == NULL)
    {
        SetLastError((DWORD) NTE_BAD_KEY);
        goto Ret;
    }

    if (dwBlobType != PUBLICKEYBLOB &&
        ((pTmpKey->Rights & CRYPT_EXPORTABLE) != CRYPT_EXPORTABLE))
    {
        SetLastError((DWORD) NTE_BAD_KEY_STATE);
        goto Ret;
    }

    pPreHeader->aiKeyAlg = pTmpKey->Algid;

    switch(dwBlobType)
    {
        case PUBLICKEYBLOB:
        {
            if ((HNTAG_TO_HTYPE(hKey) != SIGPUBKEY_HANDLE) &&
                (HNTAG_TO_HTYPE(hKey) != EXCHPUBKEY_HANDLE))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            pBsafePubKey = (BSAFE_PUB_KEY *) pTmpKey->pKeyValue;

            if (pBsafePubKey == NULL)
            {
                SetLastError((DWORD) NTE_NO_KEY);
                goto Ret;
            }

            //
            // Subtract off 2 extra DWORD needed by RSA code
            //
            dwLen = sizeof(BLOBHEADER) + sizeof(RSAPUBKEY) +
                    ((pBsafePubKey->bitlen + 7) / 8);

            // Check user buffer size
            if (pbData == NULL || *pdwDataLen < dwLen)
            {
                *pdwDataLen = dwLen;
                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            pExpPubKey = (RSAPUBKEY *) (pbData + sizeof(BLOBHEADER));
            pExpPubKey->magic = pBsafePubKey->magic;
            pExpPubKey->bitlen = pBsafePubKey->bitlen;
            pExpPubKey->pubexp = pBsafePubKey->pubexp;

            memcpy((BYTE *) pbData + sizeof(BLOBHEADER) + sizeof(RSAPUBKEY),
                   (BYTE *) pBsafePubKey + sizeof(BSAFE_PUB_KEY),
                   ((pBsafePubKey->bitlen + 7) / 8));

            break;
        }

        case PRIVATEKEYBLOB:
        {
            DWORD   dwBlockLen = 0;
            BOOL    fSigKey;
            LPWSTR  szPrompt;

            cb = sizeof(DWORD);
            if (HNTAG_TO_HTYPE(hKey) == SIGPUBKEY_HANDLE)
            {
                fSigKey = TRUE;
                szPrompt = g_Strings.pwszExportPrivSig;
                pPublicKey = (BSAFE_PUB_KEY*)pTmpUser->ContInfo.pbSigPub;
                PubKeyLen = pTmpUser->ContInfo.ContLens.cbSigPub;
            }
            else if (HNTAG_TO_HTYPE(hKey) == EXCHPUBKEY_HANDLE)
            {
                fSigKey = FALSE;
                szPrompt = g_Strings.pwszExportPrivExch;
                pPublicKey = (BSAFE_PUB_KEY*)pTmpUser->ContInfo.pbExchPub;
                PubKeyLen = pTmpUser->ContInfo.ContLens.cbExchPub;
            }
            else
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            // make sure the public key is available and appropriate
            if ((pPublicKey == NULL) ||
                (PubKeyLen != pTmpKey->cbKeyLen) ||
                memcmp((PBYTE)pPublicKey, pTmpKey->pKeyValue, PubKeyLen))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            GetLengthOfPrivateKeyForExport(pPublicKey, PubKeyLen, &cbPrivateBlob);

            if (hPubKey)
            {
                if (!CPGetKeyParam(hUID, hPubKey, KP_BLOCKLEN,
                                   (PBYTE)&dwBlockLen, &cb, 0))
                {
                    SetLastError((DWORD) NTE_BAD_KEY);
                    goto Ret;
                }

                // convert to byte count
                dwBlockLen /= 8;
            }

            // Check user buffer size
            if (pbData == NULL)
            {
                *pdwDataLen = sizeof(BLOBHEADER) + cbPrivateBlob + dwBlockLen;
                fRet = NTF_SUCCEED;
                goto Ret;
            }
            else if (*pdwDataLen < (sizeof(BLOBHEADER) + cbPrivateBlob + dwBlockLen))
            {
                *pdwDataLen = sizeof(BLOBHEADER) + cbPrivateBlob + dwBlockLen;
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            // if the context being used is a Verify context then the key is not in
            // persisted storage and therefore is in memory
            if (!(pTmpUser->Rights & CRYPT_VERIFYCONTEXT))
            {
                // always read the private key from storage when exporting
                if (!UnprotectPrivKey(pTmpUser, szPrompt, fSigKey, TRUE))
                {
                    SetLastError((DWORD) NTE_BAD_KEYSET);
                    goto Ret;
                }
            }

            if (fSigKey)
            {
	            PrvKeyLen = pTmpUser->SigPrivLen;
	            pPrvKey = (BSAFE_PRV_KEY*)pTmpUser->pSigPrivKey;
                fExportable = pTmpUser->ContInfo.fSigExportable;
            }
            else
            {
	            PrvKeyLen = pTmpUser->ExchPrivLen;
	            pPrvKey = (BSAFE_PRV_KEY*)pTmpUser->pExchPrivKey;
                fExportable = pTmpUser->ContInfo.fExchExportable;
            }

            if (pPrvKey == NULL)
            {
                SetLastError((DWORD) NTE_NO_KEY);
                goto Ret;
            }

            if (!fExportable)
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            if (!PreparePrivateKeyForExport(pPrvKey, PrvKeyLen, NULL, &cbPrivateBlob))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            // allocate memory for the private key blob
            cb = cbPrivateBlob + dwBlockLen;
            if (NULL == (pbPrivateBlob = _nt_malloc(cb)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            if (!PreparePrivateKeyForExport(pPrvKey, PrvKeyLen, pbPrivateBlob, &cbPrivateBlob))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            if (hPubKey)
            {
                if (!LocalEncrypt(hUID, hPubKey, 0, TRUE, 0, pbPrivateBlob,
                                  &cbPrivateBlob, cb, FALSE))
                {
                    goto Ret;
                }
            }

            dwLen = sizeof(BLOBHEADER) + cbPrivateBlob;

            CopyMemory(pbData + sizeof(BLOBHEADER), pbPrivateBlob, cbPrivateBlob);
            break;
        }

        case SIMPLEBLOB:
        {

            if (HNTAG_TO_HTYPE(hKey) != KEY_HANDLE)
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            if (0 == hPubKey)
            {
                SetLastError((DWORD) NTE_NO_KEY);
                goto Ret;
            }

            if (!FIsLegalKey(pTmpUser, pTmpKey, FALSE))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

#ifdef CSP_USE_SSL3
            // if the SSL2_FALLBACK flag is set then make sure the key
            // is an SSL2 master key
            if (CRYPT_SSL2_FALLBACK & dwFlags)
            {
                if (CALG_SSL2_MASTER != pTmpKey->Algid)
                {
                    SetLastError((DWORD) NTE_BAD_KEY);
                    goto Ret;
                }
            }
#endif

            if ((pPubKey = (PNTAGKeyList) NTLValidate((HNTAG)hPubKey,
                                                hUID,
                                                EXCHPUBKEY_HANDLE)) == NULL)
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            pBsafePubKey = (BSAFE_PUB_KEY *) pPubKey->pKeyValue;

            if (pBsafePubKey == NULL)
            {
                SetLastError((DWORD) NTE_NO_KEY);
                goto Ret;
            }

            //
            // Subtract off 8 bytes for 2 extra DWORD needed by RSA code
            //
            dwLen = sizeof(BLOBHEADER) + sizeof(NTSimpleBlob) +
            (pBsafePubKey->bitlen + 7) / 8;

            if (pbData == NULL || *pdwDataLen < dwLen)
            {
                *pdwDataLen = dwLen;    // set what we need
                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            pSimpleHeader = (NTSimpleBlob *) (pbData + sizeof(BLOBHEADER));
            pSimpleHeader->aiEncAlg = CALG_RSA_KEYX;

            // if this is the schannel provider and we are a verify context and
            // the SGC flags are set and the key is large then make sure the
            // key is the same as the SGC key
            if ((PROV_RSA_SCHANNEL == pTmpUser->dwProvType) &&
                (0 != pTmpUser->dwSGCFlags) &&
                (pTmpUser->Rights & CRYPT_VERIFYCONTEXT) &&
                (pBsafePubKey->bitlen > 1024))
            {
                if (!CheckSGCSimpleForExport(pTmpUser, pBsafePubKey))
                {
                    SetLastError((DWORD)NTE_BAD_KEY);
                    goto Ret;
                }
            }

            // perform the RSA encryption.
            if (!RSAEncrypt(pTmpUser, pBsafePubKey, pTmpKey->pKeyValue,
                            pTmpKey->cbKeyLen, pTmpKey->pbParams, pTmpKey->cbParams,
                            dwFlags,
                            pbData + sizeof(BLOBHEADER) + sizeof(NTSimpleBlob)))
            {
                goto Ret;
            }
            break;
        }

        case OPAQUEKEYBLOB:
        {
            dwLen = *pdwDataLen;
            if (!ExportOpaqueBlob(pTmpUser, pTmpKey, dwFlags, pbData, &dwLen))
            {
                goto Ret;
            }

            // if the destroy key flag is set then destroy the key
            if (CRYPT_DESTROYKEY & dwFlags)
            {
                if (!CPDestroyKey(hUID, hKey))
                {
                    goto Ret;
                }
            }

            break;
        }

        case SYMMETRICWRAPKEYBLOB:
        {
            // get a pointer to the symmetric key to wrap with (the variable
            // name pPubKey is a misnomer)
            if (NULL == (pPubKey = (PNTAGKeyList)NTLValidate((HNTAG)hPubKey,
                                                      hUID,
                                                      HNTAG_TO_HTYPE(hKey))))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            if (0 != (dwErr = WrapSymKey(pTmpUser, pTmpKey, pPubKey,
                                         pbData, pdwDataLen)))
            {
                SetLastError(dwErr);
                goto Ret;
            }
			dwLen = *pdwDataLen;

            break;
        }

        default:
            SetLastError((DWORD) NTE_BAD_TYPE);
            goto Ret;

    }

    // set the size of the key blob
    *pdwDataLen = dwLen;
    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputExportKey(FALSE, fRet, 0, 0, 0, 0, 0, pbData, pdwDataLen);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (pbPrivateBlob)
        _nt_free(pbPrivateBlob, cb);

    return fRet;
}

BOOL ImportOpaqueBlob(
                      HCRYPTPROV hUID,
                      PNTAGUserList pUser,
                      DWORD dwFlags,
                      CONST BYTE *pbData,
                      DWORD cbData,
                      HCRYPTKEY *phKey
                      )
{
    PNTAGKeyList    pTmpKey = NULL;
    BYTE            *pbDecrypted = NULL;
    DWORD           cb;
    BYTE            *pb;
    BYTE            *pbTmp;
    BOOL            fRet = FALSE;

    *phKey = 0;

    // allocate a temporary key structure
    if (NULL == (pTmpKey = (PNTAGKeyList)_nt_malloc(sizeof(NTAGKeyList))))
        goto Ret;
    if (NULL == (pbDecrypted = (BYTE*)_nt_malloc(cbData - sizeof(BLOBHEADER))))
        goto Ret;

    // copy the blob to a temporary key structure
    memcpy(pbDecrypted, pbData + sizeof(BLOBHEADER),
           cbData - sizeof(BLOBHEADER));

    // decrypt the blob
    if (0 != EncryptOpaqueBlob(pUser, pbDecrypted, cbData - sizeof(BLOBHEADER)))
    {
        SetLastError((DWORD)NTE_FAIL);
        goto Ret;
    }

    memcpy(pTmpKey, pbDecrypted, sizeof(NTAGKeyList));
    pTmpKey->hUID = hUID;
    
    // allocate for the key value, the salt and the key data
    pTmpKey->pKeyValue = NULL;
    pTmpKey->pData = NULL;
    cb = sizeof(NTAGKeyList);
    if (pTmpKey->cbKeyLen)
    {
        if (NULL == (pbTmp = (BYTE*)_nt_malloc(pTmpKey->cbKeyLen)))
            goto Ret;
        pb = (BYTE*)pbDecrypted + cb;
        memcpy(pbTmp, pb, pTmpKey->cbKeyLen);
        pTmpKey->pKeyValue = pbTmp;
    }
    cb += pTmpKey->cbKeyLen;

    if (pTmpKey->cbDataLen)
    {
        if (NULL == (pbTmp = (BYTE*)_nt_malloc(pTmpKey->cbDataLen)))
            goto Ret;
        pb = (BYTE*)pbDecrypted + cb;
        memcpy(pbTmp, pb, pTmpKey->cbDataLen);
        pTmpKey->pData = pbTmp;
    }
    cb += pTmpKey->cbDataLen;
    
    if (pTmpKey->cbSaltLen)
    {
        pb = (BYTE*)pbDecrypted + cb;
        memcpy(pTmpKey->rgbSalt, pb, pTmpKey->cbSaltLen);
    }

    if (NTF_FAILED == NTLMakeItem(phKey, KEY_HANDLE, (void *)pTmpKey))
        goto Ret;          // error already set

    fRet = TRUE;
Ret:
    if (pbDecrypted)
        _nt_free(pbDecrypted, cbData - sizeof(BLOBHEADER));
    if ((fRet == FALSE) && pTmpKey)
    {
        FreeNewKey(pTmpKey);
    }
    return fRet;
}


/*
 -  CPImportKey
 -
 *  Purpose:
 *                Import cryptographic keys
 *
 *
 *  Parameters:
 *               IN  hUID      -  Handle to the CSP user
 *               IN  pbData    -  Key blob data
 *               IN  dwDataLen -  Length of the key blob data
 *               IN  hPubKey   -  Handle to the exchange public key value of
 *                                the destination user
 *               IN  dwFlags   -  Flags values
 *               OUT phKey     -  Pointer to the handle to the key which was
 *                                Imported
 *
 *  Returns:
 */
BOOL CPImportKey(IN HCRYPTPROV hUID,
                 IN CONST BYTE *pbData,
                 IN DWORD dwDataLen,
                 IN HCRYPTKEY hPubKey,
                 IN DWORD dwFlags,
                 OUT HCRYPTKEY *phKey)
{
    // miscellaneous variables
    DWORD               KeyBufLen;
    CONST BYTE          *pbEncPortion;
    BLOBHEADER          *ThisStdHeader = (BLOBHEADER *)pbData;
    BSAFE_PRV_KEY       *pBsafePrvKey = NULL;
    DWORD               cbBsafePrvKey = 0;
    BYTE                *pKeyBuf = NULL;
    DWORD               dwRights = 0;
    DWORD               cbTmpLen;

    // temporary variables for pointing to user and key records
    PNTAGUserList       pTmpUser;
    PNTAGKeyList        pTmpKey = NULL;
    LPWSTR              szPrompt;
    BLOBHEADER          *pPublic;
    RSAPUBKEY           *pImpPubKey;
    BSAFE_PUB_KEY       *pBsafePubKey;

    PBYTE               pbData2 = NULL;
    DWORD               cb;
    DWORD               *pcbPub;
    BYTE                **ppbPub;
    DWORD               *pcbPrv;
    BYTE                **ppbPrv;
    BOOL                *pfExportable;
    BOOL                fExch;
    PEXPORT_PRV_KEY     pExportKey;
    BOOL                fPubKey = FALSE;

    NTSimpleBlob        *ThisSB;

    PNTAGKeyList        pExPubKey = NULL;
    BOOL                fInCritSec = FALSE;

    BYTE                *pbParams = NULL;
    DWORD               cbParams = 0;
    DWORD               dwErr;

    BOOL                fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputImportKey(TRUE, FALSE, hUID, (BYTE*)pbData, dwDataLen,
                            hPubKey, dwFlags, phKey);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    // Validate user pointer
//    count = *phKey;

    if ((dwFlags & ~(CRYPT_USER_PROTECTED | CRYPT_EXPORTABLE |
                     CRYPT_NO_SALT | CRYPT_SGCKEY | CRYPT_OAEP)) != 0)
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    if ((PUBLICKEYBLOB == ThisStdHeader->bType) && (0 != hPubKey))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    // check the user identification
    if ((pTmpUser = (PNTAGUserList) NTLCheckList ((HNTAG)hUID,
                                                  USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    if (ThisStdHeader->bVersion != CUR_BLOB_VERSION)
    {
        SetLastError((DWORD) NTE_BAD_VER);
        goto Ret;
    }

    // Handy pointer for decrypting the blob...
    pbEncPortion = pbData + sizeof(BLOBHEADER) + sizeof(NTSimpleBlob);

    // determine which key blob is being imported
    switch (ThisStdHeader->bType)
    {
        case PUBLICKEYBLOB:
        {
            pPublic = (BLOBHEADER *) pbData;
            pImpPubKey = (RSAPUBKEY *)(pbData+sizeof(BLOBHEADER));

            if ((pPublic->aiKeyAlg != CALG_RSA_KEYX) &&
                 (pPublic->aiKeyAlg != CALG_RSA_SIGN))
            {
                SetLastError((DWORD) NTE_BAD_DATA);
                goto Ret;
            }

            cbTmpLen = (sizeof(DWORD) * 2) -
                       (((pImpPubKey->bitlen + 7) / 8) % (sizeof(DWORD) * 2));
            if ((sizeof(DWORD) * 2) != cbTmpLen)
                cbTmpLen += sizeof(DWORD) * 2;
            if ((pTmpKey = MakeNewKey(pPublic->aiKeyAlg,
                                      0,
                                      sizeof(BSAFE_PUB_KEY) +
                                      (pImpPubKey->bitlen / 8) + cbTmpLen,
                                      hUID,
                                      0,
                                      FALSE))==NULL)
            {
                goto Ret;
            }

            pBsafePubKey = (BSAFE_PUB_KEY *) pTmpKey->pKeyValue;
            pBsafePubKey->magic = pImpPubKey->magic;
            pBsafePubKey->keylen = (pImpPubKey->bitlen / 8) + cbTmpLen;
            pBsafePubKey->bitlen = pImpPubKey->bitlen;
            pBsafePubKey->datalen = (pImpPubKey->bitlen+7)/8 - 1;
            pBsafePubKey->pubexp = pImpPubKey->pubexp;

            memset((BYTE *) pBsafePubKey + sizeof(BSAFE_PUB_KEY),
                   '\0', pBsafePubKey->keylen);

            memcpy((BYTE *) pBsafePubKey + sizeof(BSAFE_PUB_KEY),
                   (BYTE *) pPublic + sizeof(BLOBHEADER) + sizeof(RSAPUBKEY),
                   (pImpPubKey->bitlen+7)/8);

            if (NTLMakeItem(phKey,
                            (BYTE) (pPublic->aiKeyAlg == CALG_RSA_KEYX ?
                            EXCHPUBKEY_HANDLE : SIGPUBKEY_HANDLE),
                            (void *)pTmpKey) == NTF_FAILED)
            {
                goto Ret;
            }
            break;
        }

        case PRIVATEKEYBLOB:
        {
            // wrap with a try since there is a critical sections in here
            try
            {
                EnterCriticalSection(&pTmpUser->CritSec);
                fInCritSec = TRUE;

                pPublic = (BLOBHEADER *) pbData;

                cb = dwDataLen - sizeof(BLOBHEADER);
                if (NULL == (pbData2 = _nt_malloc(cb)))
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    goto Ret;
                }
                CopyMemory(pbData2, pbData + sizeof(BLOBHEADER), cb);
                if (hPubKey)
                {
                    if (!LocalDecrypt(hUID, hPubKey, 0, TRUE, 0,
                                      pbData2, &cb, FALSE))
                    {
                        goto Ret;
                    }
                }

                if (pPublic->aiKeyAlg == CALG_RSA_KEYX)
                {
                    if (PROV_RSA_SIG == pTmpUser->dwProvType)
                    {
                        SetLastError((DWORD)NTE_BAD_DATA);
                        goto Ret;
                    }
                    pcbPub = &pTmpUser->ContInfo.ContLens.cbExchPub;
                    ppbPub = &pTmpUser->ContInfo.pbExchPub;
                    pcbPrv = &pTmpUser->ExchPrivLen;
                    ppbPrv = &pTmpUser->pExchPrivKey;
                    pfExportable = &pTmpUser->ContInfo.fExchExportable;
                    fExch = TRUE;
                    szPrompt = g_Strings.pwszImportPrivExch;
                }
                else if (pPublic->aiKeyAlg == CALG_RSA_SIGN)
                {
                    if (PROV_RSA_SCHANNEL == pTmpUser->dwProvType)
                    {
                        SetLastError((DWORD)NTE_BAD_DATA);
                        goto Ret;
                    }
                    pcbPub = &pTmpUser->ContInfo.ContLens.cbSigPub;
                    ppbPub = &pTmpUser->ContInfo.pbSigPub;
                    pcbPrv = &pTmpUser->SigPrivLen;
                    ppbPrv = &pTmpUser->pSigPrivKey;
                    fExch = FALSE;
                    pfExportable = &pTmpUser->ContInfo.fSigExportable;
                    szPrompt = g_Strings.pwszImportPrivSig;
                }
                else
                {
                    SetLastError((DWORD) NTE_BAD_DATA);
                    goto Ret;
                }

                // check the length of the key exchange key
                pExportKey = (PEXPORT_PRV_KEY)pbData2;

                // check if the provider is an SChannel provider and if so if the
                // SGC flag is set then use the FIsLegalSGCKeySize function
                if ((PROV_RSA_SCHANNEL == pTmpUser->dwProvType) &&
                    (!(pTmpUser->Rights & CRYPT_VERIFYCONTEXT)) &&  // make sure this is server side
                    (0 != pTmpUser->dwSGCFlags))
                {
                    if (!FIsLegalSGCKeySize(pPublic->aiKeyAlg,
                                            pExportKey->bitlen / 8,
                                            FALSE, FALSE, &fPubKey))
                    {
                        SetLastError((DWORD) NTE_BAD_DATA);
                        goto Ret;
                    }
                }
                else
                {
                    if (!FIsLegalKeySize(pPublic->aiKeyAlg,
                                         pExportKey->bitlen / 8,
                                         FALSE, dwFlags, &fPubKey))
                    {
                        SetLastError((DWORD) NTE_BAD_DATA);
                        goto Ret;
                    }
                }

                if (!fPubKey)
                {
                    SetLastError((DWORD) NTE_BAD_DATA);
                    goto Ret;
                }

                if (*ppbPub)
                {
                    ASSERT(*pcbPub);
                    ASSERT(*pcbPrv);
                    ASSERT(*ppbPrv);

                    _nt_free (*ppbPub, *pcbPub);
                    *ppbPub = NULL;
                    *pcbPub = 0;

                    _nt_free (*ppbPrv, *pcbPrv);
                    *ppbPrv = NULL;
                    *pcbPrv = 0;
                }

                if (!PreparePrivateKeyForImport(pbData2, cb, NULL, pcbPrv, NULL, pcbPub))
                {
                    SetLastError((DWORD) NTE_BAD_DATA);
                    goto Ret;
                }

                if (NULL == (*ppbPub = _nt_malloc(*pcbPub)))
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    goto Ret;
                }
                if (NULL == (*ppbPrv = _nt_malloc(*pcbPrv)))
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    goto Ret;
                }

                if (!PreparePrivateKeyForImport(pbData2, cb, (LPBSAFE_PRV_KEY)*ppbPrv,
                                                pcbPrv, (LPBSAFE_PUB_KEY)*ppbPub, pcbPub))
                {
                    SetLastError((DWORD) NTE_BAD_DATA);
                    goto Ret;
                }

                if (dwFlags & CRYPT_EXPORTABLE)
                    *pfExportable = TRUE;
                else
                    *pfExportable = FALSE;

                // test the RSA key to make sure it works
                if (0 != (dwErr = EncryptAndDecryptWithRSAKey(*ppbPub,
                                                              *ppbPrv,
                                                              TRUE,
                                                              FALSE)))
                {
                    SetLastError((DWORD)NTE_BAD_DATA);
                    goto Ret;
                }

                if (0 != (dwErr = EncryptAndDecryptWithRSAKey(*ppbPub,
                                                              *ppbPrv,
                                                              FALSE,
                                                              FALSE)))
                {
                    SetLastError((DWORD)NTE_BAD_DATA);
                    goto Ret;
                }

                // if the context being used is a Verify Context then the key is not
                // persisted to storage
                if (!(pTmpUser->Rights & CRYPT_VERIFYCONTEXT))
                {
                    // write the new keys to the user storage file
                    if (!ProtectPrivKey(pTmpUser, szPrompt, dwFlags, (!fExch)))
                    {
                        goto Ret;          // error already set
                    }
                }

                if (NTF_FAILED == CPGetUserKey(
                                        hUID,
                                        (fExch ? AT_KEYEXCHANGE : AT_SIGNATURE),
                                        phKey))
                {
                    goto Ret;
                }
            }
            except ( EXCEPTION_EXECUTE_HANDLER )
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                goto Ret;
            }
        break;
        }

        case SIMPLEBLOB:
        {
            ThisSB = (NTSimpleBlob *) (pbData + sizeof(BLOBHEADER));

            if (!ValidKeyAlgid(pTmpUser, ThisStdHeader->aiKeyAlg))
            {
                SetLastError((DWORD) NTE_BAD_TYPE);
                goto Ret;
            }

            if (ThisSB->aiEncAlg != CALG_RSA_KEYX)
            {
                SetLastError((DWORD) NTE_BAD_ALGID);
                goto Ret;
            }

            // if the import key handle is not zero make sure it is the
            if (hPubKey)
            {
                if ((pExPubKey = (PNTAGKeyList) NTLValidate((HNTAG)hPubKey,
                                  hUID, HNTAG_TO_HTYPE((HNTAG)hPubKey))) == NULL)
                {
                    // NTLValidate doesn't know what error to set
                    // so it set NTE_FAIL -- fix it up.
                    if (GetLastError() == NTE_FAIL)
                        SetLastError((DWORD) NTE_BAD_KEY);

                    goto Ret;
                }

                if ((pTmpUser->ContInfo.ContLens.cbExchPub != pExPubKey->cbKeyLen)||
                    memcmp((PBYTE)pExPubKey->pKeyValue, pTmpUser->ContInfo.pbExchPub,
                           pExPubKey->cbKeyLen))
                {
                    SetLastError((DWORD) NTE_BAD_KEY);
                    goto Ret;
                }

                pbParams = pExPubKey->pbParams;
                cbParams = pExPubKey->cbParams;
            }

            // check if the provider is an SChannel provider and if so if the
            // SGC flag is set then use the FIsLegalSGCKeySize function
            pBsafePubKey = (BSAFE_PUB_KEY *)pTmpUser->ContInfo.pbExchPub;
            if (NULL == pBsafePubKey)
            {
                SetLastError((DWORD)NTE_NO_KEY);
                goto Ret;
            }
            if ((PROV_RSA_SCHANNEL == pTmpUser->dwProvType) &&
                (!(pTmpUser->Rights & CRYPT_VERIFYCONTEXT)) &&  // make sure this is server side
                (0 != pTmpUser->dwSGCFlags))
            {
                if (!FIsLegalSGCKeySize(CALG_RSA_KEYX,
                                        pBsafePubKey->bitlen / 8,
                                        FALSE, FALSE, &fPubKey))
                {
                    SetLastError((DWORD) NTE_BAD_DATA);
                    goto Ret;
                }
            }
            else
            {
                if (!FIsLegalKeySize(CALG_RSA_KEYX,
                                     pBsafePubKey->bitlen / 8,
                                     FALSE, 0, &fPubKey))
                {
                    SetLastError((DWORD) NTE_BAD_DATA);
                    goto Ret;
                }
            }

            // get the key to use
            if (!UnprotectPrivKey(pTmpUser, g_Strings.pwszImportSimple,
                                  FALSE, FALSE))
            {
                SetLastError((DWORD)NTE_NO_KEY);
                goto Ret;
            }
            pBsafePrvKey = (BSAFE_PRV_KEY *) pTmpUser->pExchPrivKey;

            if(NULL == pBsafePrvKey)
            {
                SetLastError((DWORD)NTE_NO_KEY);
                goto Ret;
            }

            // perform the RSA decryption
            if (!RSADecrypt(pTmpUser, pBsafePrvKey,
                            (char *)pbData + sizeof(BLOBHEADER) +
                                    sizeof(NTSimpleBlob),
                            sizeof(BLOBHEADER) + sizeof(NTSimpleBlob),
                            pbParams, cbParams, dwFlags,
                            &pKeyBuf,
                            &KeyBufLen))
            {
                goto Ret;
            }

            // check if the key is CRYPT_EXPORTABLE
            if (dwFlags & CRYPT_EXPORTABLE)
                dwRights = CRYPT_EXPORTABLE;

#ifdef CSP_USE_SSL3
            // if SSL3 or TLS1 master key then check the version
            if (CALG_SSL3_MASTER == ThisStdHeader->aiKeyAlg || 
                CALG_TLS1_MASTER == ThisStdHeader->aiKeyAlg)
            {
                if (MAKEWORD(pKeyBuf[1], pKeyBuf[0]) < 0x300)
                {
                    SetLastError((DWORD)NTE_BAD_DATA);
                    goto Ret;
                }
            }
#endif // CSP_USE_SSL3

            if ((pTmpKey = MakeNewKey(ThisStdHeader->aiKeyAlg,
                                      dwRights,
                                      KeyBufLen,
                                      hUID,
                                      pKeyBuf,
                                      TRUE)) == NULL)
            {
                goto Ret;
            }
            pKeyBuf = NULL;

            // check keylength...
            if (!FIsLegalKey(pTmpUser, pTmpKey, TRUE))
            {
                goto Ret;
            }

            if (NTLMakeItem(phKey, KEY_HANDLE, (void *)pTmpKey) == NTF_FAILED)
            {
                goto Ret;          // error already set
            }

            // if 40 bit key + no mention of salt, set zeroized salt for RSABase compatibility
            if ((5 == KeyBufLen) && (!(dwFlags & CRYPT_NO_SALT)) &&
                (CALG_SSL2_MASTER != ThisStdHeader->aiKeyAlg))
            {
                if (!MakeKeyRSABaseCompatible(hUID, *phKey))
                    goto Ret;
            }
            break;
        }

        case OPAQUEKEYBLOB:
        {
            if (!ImportOpaqueBlob(hUID, pTmpUser, dwFlags, pbData, dwDataLen, phKey))
                goto Ret;
            break;
        }

        case SYMMETRICWRAPKEYBLOB:
        {
            // get a pointer to the symmetric key to unwrap with (the variable
            // name pExPubKey is a misnomer)
            if ((pExPubKey = (PNTAGKeyList) NTLValidate((HNTAG)hPubKey,
                              hUID, HNTAG_TO_HTYPE((HNTAG)hPubKey))) == NULL)
            {
                // NTLValidate doesn't know what error to set
                // so it set NTE_FAIL -- fix it up.
                if (GetLastError() == NTE_FAIL)
                    SetLastError((DWORD) NTE_BAD_KEY);

                goto Ret;
            }

            if (0 != (dwErr = UnWrapSymKey(hUID, pTmpUser, pExPubKey,
                                           (BYTE*)pbData, dwDataLen,
                                           dwFlags, phKey)))
            {
                SetLastError(dwErr);
                goto Ret;
            }

            break;
        }

        default:
            SetLastError((DWORD) NTE_BAD_TYPE);
            goto Ret;
    }

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputImportKey(FALSE, fRet, 0, NULL, 0, 0, 0, phKey);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (fInCritSec)
    {
        LeaveCriticalSection(&pTmpUser->CritSec);
    }
    if (pKeyBuf)
        _nt_free(pKeyBuf, KeyBufLen);
    if (pbData2)
        _nt_free(pbData2, dwDataLen - sizeof(BLOBHEADER));
    if ((NTF_FAILED == fRet) && pTmpKey)
        FreeNewKey(pTmpKey);

    return fRet;
}


/*
 -  CPInflateKey
 -
 *  Purpose:
 *                Use to "inflate" (expand) a cryptographic key for use with
 *                the CryptEncrypt and CryptDecrypt functions
 *
 *  Parameters:
 *               IN      hUID    -  Handle to a CSP
 *               IN      hKey    -  Handle to a key
 *               IN      dwFlags -  Flags values
 *
 *  Returns:
 */
BOOL CPInflateKey(
                  IN PNTAGKeyList pTmpKey
                  )
{
    BYTE                *pbRealKey = NULL;
    BOOL                fRet = NTF_FAILED;

    // if space for the key table has been allocated previously
    // then free it
    if (pTmpKey->pData != NULL)
    {
        ASSERT(pTmpKey->cbDataLen);
        _nt_free (pTmpKey->pData, pTmpKey->cbDataLen);
        pTmpKey->cbDataLen = 0;
    }
    else
    {
        ASSERT(pTmpKey->cbDataLen == 0);
    }

    // determine the algorithm to be used
    switch (pTmpKey->Algid)
    {

#ifdef CSP_USE_RC2

        case CALG_RC2:

            if ((pTmpKey->pData = (BYTE *)_nt_malloc(RC2_TABLESIZE)) == NULL)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                return NTF_FAILED;
            }

            if (NULL == (pbRealKey = (BYTE *)_nt_malloc(pTmpKey->cbKeyLen +
                                                        pTmpKey->cbSaltLen)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            memcpy(pbRealKey, pTmpKey->pKeyValue, pTmpKey->cbKeyLen);
            memcpy(pbRealKey+pTmpKey->cbKeyLen, pTmpKey->rgbSalt, pTmpKey->cbSaltLen);

            pTmpKey->cbDataLen = RC2_TABLESIZE;

            RC2KeyEx((WORD *)pTmpKey->pData,
                     pbRealKey,
                     pTmpKey->cbKeyLen + pTmpKey->cbSaltLen,
                     pTmpKey->EffectiveKeyLen);

            break;

#endif

#ifdef CSP_USE_RC4

        case CALG_RC4:

            if ((pTmpKey->pData = (BYTE *)_nt_malloc(sizeof(RC4_KEYSTRUCT)))
                         == NULL)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            if (NULL == (pbRealKey = (BYTE *)_nt_malloc(pTmpKey->cbKeyLen +
                                                        pTmpKey->cbSaltLen)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            memcpy(pbRealKey, pTmpKey->pKeyValue, pTmpKey->cbKeyLen);
            memcpy(pbRealKey+pTmpKey->cbKeyLen, pTmpKey->rgbSalt, pTmpKey->cbSaltLen);

            pTmpKey->cbDataLen = sizeof(RC4_KEYSTRUCT);

            rc4_key((struct RC4_KEYSTRUCT *)pTmpKey->pData, pTmpKey->cbKeyLen+pTmpKey->cbSaltLen,
                    pbRealKey);

            break;

#endif

#ifdef CSP_USE_DES

        case CALG_DES:

            if ((pTmpKey->pData = (BYTE *)_nt_malloc(DES_TABLESIZE)) == NULL)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            pTmpKey->cbDataLen = DES_TABLESIZE;

            deskey((DESTable *)pTmpKey->pData, pTmpKey->pKeyValue);

            break;

#endif

#ifdef CSP_USE_3DES

        case CALG_3DES_112:

            if ((pTmpKey->pData = (BYTE *)_nt_malloc(DES3_TABLESIZE)) == NULL)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            pTmpKey->cbDataLen = DES3_TABLESIZE;

            tripledes2key((PDES3TABLE)pTmpKey->pData, pTmpKey->pKeyValue);

            break;

        case CALG_3DES:

            if ((pTmpKey->pData = (BYTE *)_nt_malloc(DES3_TABLESIZE)) == NULL)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            pTmpKey->cbDataLen = DES3_TABLESIZE;

            tripledes3key((PDES3TABLE)pTmpKey->pData, pTmpKey->pKeyValue);

            break;

#endif

#ifdef CSP_USE_SSL3
        case CALG_SSL3_MASTER:
        case CALG_PCT1_MASTER:
        case CALG_SCHANNEL_MAC_KEY:
            break;
#endif

        default:
            SetLastError((DWORD) NTE_BAD_TYPE);
            goto Ret;
            break;
        }

    fRet = NTF_SUCCEED;
Ret:
    if (pbRealKey)
    {
        _nt_free(pbRealKey, pTmpKey->cbKeyLen + pTmpKey->cbSaltLen);
    }
    return fRet;
}

/*
 -  CPDestroyKey
 -
 *  Purpose:
 *                Destroys the cryptographic key that is being referenced
 *                with the hKey parameter
 *
 *
 *  Parameters:
 *               IN      hUID   -  Handle to a CSP
 *               IN      hKey   -  Handle to a key
 *
 *  Returns:
 */
BOOL CPDestroyKey(IN HCRYPTPROV hUID,
                  IN HCRYPTKEY hKey)
{
    PNTAGKeyList    pTmpKey;
    DWORD           dwErr;
    BOOL            fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputDestroyKey(TRUE, FALSE, hUID, hKey);
#endif // DBG -- NOTE:  This section not compiled for retail builds 

    // check the user identification
    if (NTLCheckList ((HNTAG)hUID, USER_HANDLE) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }
    dwErr = GetLastError();
    if ((pTmpKey = (PNTAGKeyList) NTLValidate((HNTAG)hKey,
                        hUID, SIGPUBKEY_HANDLE)) == NULL &&
        (pTmpKey = (PNTAGKeyList) NTLValidate((HNTAG)hKey,
                        hUID, EXCHPUBKEY_HANDLE)) == NULL &&
        (pTmpKey = (PNTAGKeyList)NTLValidate((HNTAG)hKey,
                        hUID, KEY_HANDLE)) == NULL)
    {
            // NTLValidate doesn't know what error to set
        // so it set NTE_FAIL -- fix it up.
        if (GetLastError() == NTE_FAIL)
        {
            SetLastError((DWORD) NTE_BAD_KEY);
        }
        goto Ret;
    }
    SetLastError(dwErr);

    // Remove from internal list first so others can't get to it, then free.
    NTLDelete((HNTAG)hKey);

    // scrub the memory where the key information was held
    if (pTmpKey->pKeyValue)
    {
        ASSERT(pTmpKey->cbKeyLen);
        memset(pTmpKey->pKeyValue, 0, pTmpKey->cbKeyLen);
        _nt_free (pTmpKey->pKeyValue, pTmpKey->cbKeyLen);
    }
    if (pTmpKey->pbParams)
    {
        _nt_free (pTmpKey->pbParams, pTmpKey->cbParams);
    }

    if (pTmpKey->pData)
    {
        ASSERT(pTmpKey->cbDataLen);
#ifndef _XBOX        
        if ((CALG_SSL3_MASTER == pTmpKey->Algid) ||
            (CALG_PCT1_MASTER == pTmpKey->Algid))
        {
            FreeSChKey((PSCH_KEY)pTmpKey->pData);
        }
#endif
        memset(pTmpKey->pData, 0, pTmpKey->cbDataLen);
        _nt_free (pTmpKey->pData, pTmpKey->cbDataLen);
    }
    _nt_free (pTmpKey, sizeof(NTAGKeyList));

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputDestroyKey(FALSE, fRet, 0, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds 

    return fRet;
}

/*
 -  CPGetUserKey
 -
 *  Purpose:
 *                Gets a handle to a permanent user key
 *
 *
 *  Parameters:
 *               IN  hUID       -  Handle to the user identifcation
 *               IN  dwWhichKey -  Specification of the key to retrieve
 *               OUT phKey      -  Pointer to key handle of retrieved key
 *
 *  Returns:
 */
BOOL CPGetUserKey(IN HCRYPTPROV hUID,
                  IN DWORD dwWhichKey,
                  OUT HCRYPTKEY *phKey)
{

    PNTAGUserList   pUser;
    PNTAGKeyList    pTmpKey;
    ALG_ID          Algid;
    DWORD           cb;
    BYTE            *pb;
    BYTE            bType;
    DWORD           dwExportability = 0;
    DWORD           dwErr;
    BOOL            fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputGetUserKey(TRUE, FALSE, hUID, dwWhichKey, phKey);
#endif // DBG -- NOTE:  This section not compiled for retail builds 

    // check the user identification
    if ((pUser = (PNTAGUserList) NTLCheckList(hUID, USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD)NTE_BAD_UID);
        goto Ret;
    }

    switch (dwWhichKey)
    {
        case AT_KEYEXCHANGE:
            Algid = CALG_RSA_KEYX;
            cb = pUser->ContInfo.ContLens.cbExchPub;
            pb = pUser->ContInfo.pbExchPub;
            if (pUser->ContInfo.fExchExportable)
                dwExportability = CRYPT_EXPORTABLE;
            bType = EXCHPUBKEY_HANDLE;
            break;

        case AT_SIGNATURE:
            Algid = CALG_RSA_SIGN;
            cb = pUser->ContInfo.ContLens.cbSigPub;
            pb = pUser->ContInfo.pbSigPub;
            if (pUser->ContInfo.fSigExportable)
                dwExportability = CRYPT_EXPORTABLE;
            bType = SIGPUBKEY_HANDLE;
            break;

        default:
            SetLastError((DWORD)NTE_BAD_KEY);
            goto Ret;
    }

    if (!ValidKeyAlgid(pUser, Algid))
    {
        SetLastError((DWORD)NTE_BAD_TYPE);
        goto Ret;
    }

    if (0 == cb)
    {
        SetLastError((DWORD)NTE_NO_KEY);
        goto Ret;
    }

    if (NULL == (pTmpKey = MakeNewKey(Algid,
                                      dwExportability,
                                      cb,
                                      hUID,
                                      pb,
                                      FALSE)))
    {
        goto Ret;
    }

    if (NTF_FAILED == NTLMakeItem(phKey, bType, (void *)pTmpKey))
        goto Ret;      // error already set

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputGetUserKey(FALSE, fRet, 0, 0, phKey);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    return fRet;
}

/*
 -  CPSetKeyParam
 -
 *  Purpose:
 *                Allows applications to customize various aspects of the
 *                operations of a key
 *
 *  Parameters:
 *               IN      hUID    -  Handle to a CSP
 *               IN      hKey    -  Handle to a key
 *               IN      dwParam -  Parameter number
 *               IN      pbData  -  Pointer to data
 *               IN      dwFlags -  Flags values
 *
 *  Returns:
 */
BOOL CPSetKeyParam(IN HCRYPTPROV hUID,
                   IN HCRYPTKEY hKey,
                   IN DWORD dwParam,
                   IN BYTE *pbData,
                   IN DWORD dwFlags)
{
    PNTAGUserList       pTmpUser;
    PNTAGKeyList        pTmpKey;
    PCRYPT_DATA_BLOB    psData;
    DWORD               *pdw;
    DWORD               dw;
    DWORD               dwKeySpec;
    BOOL                fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputSetKeyParam(TRUE, FALSE, hUID, hKey, dwParam, pbData, dwFlags);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if ((dwFlags & ~CRYPT_SERVER) != 0)
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    // check the user identification
    if ((pTmpUser = (PNTAGUserList)NTLCheckList(hUID, USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    if ((pTmpKey = (PNTAGKeyList)NTLValidate((HNTAG)hKey,
                                              hUID, KEY_HANDLE)) == NULL &&
        (pTmpKey = (PNTAGKeyList)NTLValidate((HNTAG)hKey,
                                           hUID, SIGPUBKEY_HANDLE)) == NULL &&
        (pTmpKey = (PNTAGKeyList)NTLValidate((HNTAG)hKey,
                                          hUID, EXCHPUBKEY_HANDLE)) == NULL)
    {
        // NTLValidate doesn't know what error to set
        // so it set NTE_FAIL -- fix it up.
        if (GetLastError() == NTE_FAIL)
        {
            SetLastError((DWORD) NTE_BAD_KEY);
        }
        goto Ret;
    }
    switch (dwParam)
    {
        case KP_IV:
            memcpy(pTmpKey->IV, pbData, RC2_BLOCKLEN);
            break;

        case KP_SALT:
            if ((CALG_RC2 != pTmpKey->Algid) && (CALG_RC4 != pTmpKey->Algid))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            if (pbData == NULL)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                goto Ret;
            }

#ifdef STRONG
            if (pTmpUser->fNewStrongCSP)
            {
                pTmpKey->cbSaltLen = DEFAULT_SALT_NEWSTRONG_LENGTH;
            }
            else
            {
                pTmpKey->cbSaltLen = DEFAULT_SALT_LENGTH;
            }
#else
            pTmpKey->cbSaltLen = DEFAULT_SALT_LENGTH;
#endif // STRONG
            if (pTmpKey->cbSaltLen)
            {
                CopyMemory(pTmpKey->rgbSalt, pbData, pTmpKey->cbSaltLen);
            }

            if (NTAG_FAILED(CPInflateKey(pTmpKey)))
                goto Ret;

            break;

        case KP_SALT_EX:
            if ((CALG_RC2 != pTmpKey->Algid) && (CALG_RC4 != pTmpKey->Algid))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            psData = (PCRYPT_DATA_BLOB)pbData;

            if (pbData == NULL)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                goto Ret;
            }

            pTmpKey->cbSaltLen = psData->cbData;
            CopyMemory(pTmpKey->rgbSalt, psData->pbData, pTmpKey->cbSaltLen);

            if (NTAG_FAILED(CPInflateKey(pTmpKey)))
                goto Ret;

            break;

        case KP_PADDING:
            if (*((DWORD *) pbData) != PKCS5_PADDING)
            {
                SetLastError((DWORD) NTE_BAD_DATA);
                goto Ret;
            }
            break;

        case KP_MODE:
            if ((CALG_RSA_SIGN == pTmpKey->Algid) ||
                (CALG_RSA_KEYX == pTmpKey->Algid))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            if (*pbData != CRYPT_MODE_CBC &&
                *pbData != CRYPT_MODE_ECB &&
                *pbData != CRYPT_MODE_CFB &&
                *pbData != CRYPT_MODE_OFB)
            {
                SetLastError((DWORD) NTE_BAD_DATA);
                goto Ret;
            }
            pTmpKey->Mode = *((DWORD *) pbData);
            break;

        case KP_MODE_BITS:
            dw = *((DWORD *) pbData);
            if ((dw == 0) || (dw > 64))       // if 0 or larger than the blocklength
            {
                SetLastError((DWORD) NTE_BAD_DATA);
                goto Ret;
            }

            pTmpKey->ModeBits = dw;
            break;

        case KP_PERMISSIONS:
            if (*pbData & ~(CRYPT_ENCRYPT|CRYPT_DECRYPT|CRYPT_EXPORT|
                            CRYPT_READ|CRYPT_WRITE|CRYPT_MAC))
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }

            // the exportability of a key may not be changed
            if (*((DWORD *) pbData) & CRYPT_EXPORT)
            {
                if (!(pTmpKey->Permissions & CRYPT_EXPORT))
                {
                    SetLastError((DWORD) NTE_BAD_DATA);
                    goto Ret;
                }
            }
            else
            {
                if (pTmpKey->Permissions & CRYPT_EXPORT)
                {
                    SetLastError((DWORD) NTE_BAD_DATA);
                    goto Ret;
                }
            }

            pTmpKey->Permissions = *((DWORD *) pbData);
            break;

        case KP_EFFECTIVE_KEYLEN:
            if (CALG_RC2 != pTmpKey->Algid)
            {
                SetLastError((DWORD)NTE_BAD_KEY);
                goto Ret;
            }

            pdw = (DWORD*)pbData;
            if ((*pdw < RC2_MIN_EFFECTIVE_KEYLEN) || (*pdw > RC2_MAX_EFFECTIVE_KEYLEN))
            {
                SetLastError((DWORD)NTE_BAD_DATA);
                goto Ret;
            }

            pTmpKey->EffectiveKeyLen = *pdw;

            if (NTAG_FAILED(CPInflateKey(pTmpKey)))
                goto Ret;

            break;

#ifdef CSP_USE_SSL3
        case KP_CLIENT_RANDOM:
        case KP_SERVER_RANDOM:
        case KP_CERTIFICATE:
        case KP_CLEAR_KEY:
        case KP_SCHANNEL_ALG:
            if (PROV_RSA_SCHANNEL != pTmpUser->dwProvType)
            {
                SetLastError((DWORD)NTE_BAD_TYPE);
                goto Ret;
            }

            if (!SCHSetKeyParam(pTmpUser, pTmpKey, dwParam, pbData))
                goto Ret;
            break;
#endif // CSP_USE_SSL3

        case KP_OAEP_PARAMS:
            if (CALG_RSA_KEYX != pTmpKey->Algid)
            {
                SetLastError((DWORD)NTE_BAD_KEY);
                goto Ret;
            }

            psData = (PCRYPT_DATA_BLOB)pbData;

            if (pbData == NULL)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                goto Ret;
            }

            // free salt if it already exists
            if (pTmpKey->pbParams)
            {
                _nt_free(pTmpKey->pbParams, pTmpKey->cbParams);
            }
            pTmpKey->pbParams = NULL;

            pTmpKey->cbParams = psData->cbData;

            // alloc variable size
            if ((pTmpKey->pbParams = (BYTE *)_nt_malloc(pTmpKey->cbParams)) == NULL)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                pTmpKey->cbParams = 0;
                goto Ret;
            }
            CopyMemory(pTmpKey->pbParams, psData->pbData, pTmpKey->cbParams);

            break;

#ifdef CSP_USE_SSL3
        case KP_HIGHEST_VERSION:
            if ((CALG_SSL3_MASTER != pTmpKey->Algid) &&
                (CALG_TLS1_MASTER != pTmpKey->Algid))
            {
                SetLastError((DWORD)NTE_BAD_KEY);
                goto Ret;
            }

            if (pbData == NULL)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                goto Ret;
            }

            if (dwFlags & CRYPT_SERVER)
            {
                if((CALG_SSL3_MASTER == pTmpKey->Algid) && (*(DWORD *)pbData >= 0x301))
                {
                    // We're a server doing SSL3, and we also support TLS1.
                    // If the pre_master_secret contains a version number
                    // greater than or equal to TLS1, then abort the connection.
                    if(MAKEWORD(pTmpKey->pKeyValue[1], pTmpKey->pKeyValue[0]) >= 0x301)
                    {
                        SetLastError((DWORD)NTE_BAD_VER);
                        goto Ret;
                    }
                }
            }
            else
            {
                pTmpKey->pKeyValue[0] = HIBYTE(*(DWORD *)pbData);
                pTmpKey->pKeyValue[1] = LOBYTE(*(DWORD *)pbData);
            }

            break;
#endif // CSP_USE_SSL3

        default:
            SetLastError((DWORD) NTE_BAD_TYPE);
            goto Ret;
            break;

    }

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputSetKeyParam(FALSE, fRet, 0, 0, 0, NULL, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    return fRet;
}


/*
 -  CPGetKeyParam
 -
 *  Purpose:
 *                Allows applications to get various aspects of the
 *                operations of a key
 *
 *  Parameters:
 *               IN      hUID       -  Handle to a CSP
 *               IN      hKey       -  Handle to a key
 *               IN      dwParam    -  Parameter number
 *               IN      pbData     -  Pointer to data
 *               IN      pdwDataLen -  Length of parameter data
 *               IN      dwFlags    -  Flags values
 *
 *  Returns:
 */
BOOL CPGetKeyParam(IN HCRYPTPROV hUID,
                   IN HCRYPTKEY hKey,
                   IN DWORD dwParam,
                   IN BYTE *pbData,
                   IN DWORD *pwDataLen,
                   IN DWORD dwFlags)
{
    PNTAGUserList   pTmpUser;
    PNTAGKeyList    pTmpKey;
    BSAFE_PUB_KEY   *pBsafePubKey;
    DWORD           *pdw;
    BOOL            fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputGetKeyParam(TRUE, FALSE, hUID, hKey, dwParam, pbData,
                              pwDataLen, dwFlags);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (dwFlags != 0)
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    // check the user identification
    if (NULL == (pTmpUser = NTLCheckList ((HNTAG)hUID, USER_HANDLE)))
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    if ((pTmpKey = (PNTAGKeyList)NTLValidate((HNTAG)hKey,
                                              hUID, KEY_HANDLE)) == NULL &&
        (pTmpKey = (PNTAGKeyList)NTLValidate((HNTAG)hKey,
                                           hUID, SIGPUBKEY_HANDLE)) == NULL &&
        (pTmpKey = (PNTAGKeyList)NTLValidate((HNTAG)hKey,
                                          hUID, EXCHPUBKEY_HANDLE)) == NULL)
    {
        // NTLValidate doesn't know what error to set
        // so it set NTE_FAIL -- fix it up.
        if (GetLastError() == NTE_FAIL)
        {
            SetLastError((DWORD) NTE_BAD_KEY);
        }
        goto Ret;
    }

    if (pwDataLen == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    switch (dwParam)
    {

        case KP_IV:
            if (pbData == NULL || *pwDataLen < RC2_BLOCKLEN)
            {
                *pwDataLen = RC2_BLOCKLEN;

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }
            memcpy(pbData, pTmpKey->IV, RC2_BLOCKLEN);
            *pwDataLen = RC2_BLOCKLEN;
            break;

        case KP_SALT:
            if ((CALG_RC2 != pTmpKey->Algid) && (CALG_RC4 != pTmpKey->Algid))
            {
                if ((CALG_DES == pTmpKey->Algid) || (CALG_3DES == pTmpKey->Algid) ||
                    (CALG_3DES_112 == pTmpKey->Algid))
                {
                    *pwDataLen = 0;
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                else
                {
                    SetLastError((DWORD) NTE_BAD_KEY);
                    goto Ret;
                }
            }

            if (pbData == NULL || (*pwDataLen < pTmpKey->cbSaltLen))
            {
                *pwDataLen = pTmpKey->cbSaltLen;
                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            CopyMemory(pbData, pTmpKey->rgbSalt, pTmpKey->cbSaltLen);
            *pwDataLen = pTmpKey->cbSaltLen;
            break;

        case KP_PADDING:
            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }
            *((DWORD *) pbData) = PKCS5_PADDING;
            *pwDataLen = sizeof(DWORD);
            break;

        case KP_MODE:
            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }
            *((DWORD *) pbData) = pTmpKey->Mode;
            *pwDataLen = sizeof(DWORD);
            break;

        case KP_MODE_BITS:
            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }
            *((DWORD *) pbData) = pTmpKey->ModeBits;
            *pwDataLen = sizeof(DWORD);
            break;

        case KP_PERMISSIONS:
            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }
            *((DWORD *) pbData) = pTmpKey->Permissions;
            *pwDataLen = sizeof(DWORD);
            break;

        case KP_ALGID:
            if (pbData == NULL || *pwDataLen < sizeof(ALG_ID))
            {
                *pwDataLen = sizeof(ALG_ID);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }
            *((ALG_ID *) pbData) = pTmpKey->Algid;
            *pwDataLen = sizeof(ALG_ID);
            break;

        case KP_KEYLEN:
            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            // ALWAYS report keylen in BITS
            if ((HNTAG_TO_HTYPE(hKey) == SIGPUBKEY_HANDLE) ||
                     (HNTAG_TO_HTYPE(hKey) == EXCHPUBKEY_HANDLE))
            {
                pBsafePubKey = (BSAFE_PUB_KEY *) pTmpKey->pKeyValue;
                if (pBsafePubKey == NULL)
                {
                    SetLastError((DWORD) NTE_NO_KEY);
                    goto Ret;
                }
                *((DWORD *) pbData) = pBsafePubKey->bitlen;
                *pwDataLen = sizeof(DWORD);
            }
            else
            {
                *((DWORD *) pbData) = (pTmpKey->cbKeyLen*8);
                *pwDataLen = sizeof(DWORD);
            }
            break;

        case KP_BLOCKLEN:
            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            if ((HNTAG_TO_HTYPE(hKey) == SIGPUBKEY_HANDLE) ||
                (HNTAG_TO_HTYPE(hKey) == EXCHPUBKEY_HANDLE))
            {
                pBsafePubKey = (BSAFE_PUB_KEY *) pTmpKey->pKeyValue;
                if (pBsafePubKey == NULL)
                {
                    SetLastError((DWORD) NTE_NO_KEY);
                    goto Ret;
                }
                *((DWORD *) pbData) = pBsafePubKey->bitlen;
                *pwDataLen = sizeof(DWORD);
            }
            else
            {
                switch (pTmpKey->Algid)
                {
#ifdef CSP_USE_RC2
                    case CALG_RC2:
                        *((DWORD *) pbData) = RC2_BLOCKLEN * 8;
                        *pwDataLen = sizeof(DWORD);
                        break;
#endif

#ifdef CSP_USE_DES
                    case CALG_DES:
                        *((DWORD *) pbData) = DES_BLOCKLEN * 8;
                        *pwDataLen = sizeof(DWORD);
                        break;
#endif

#ifdef CSP_USE_3DES
                    case CALG_3DES_112:
                    case CALG_3DES:
                        *((DWORD *) pbData) = DES_BLOCKLEN * 8;
                        *pwDataLen = sizeof(DWORD);
                        break;
#endif

                    default:
                        *((DWORD *) pbData) = 0;
                        *pwDataLen = sizeof(DWORD);
                }
            }

            break;

        case KP_EFFECTIVE_KEYLEN:
            if (CALG_RC2 != pTmpKey->Algid)
            {
                SetLastError((DWORD)NTE_BAD_KEY);
                goto Ret;
            }

            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            *pwDataLen = sizeof(DWORD);
            pdw = (DWORD*)pbData;
            *pdw = pTmpKey->EffectiveKeyLen;
            break;

        default:
            SetLastError((DWORD) NTE_BAD_TYPE);
            goto Ret;
            break;

    }

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputGetKeyParam(FALSE, fRet, 0, 0, 0, NULL, NULL, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    return fRet;
}

BOOL DeletePersistedKey(
                        PNTAGUserList pTmpUser,
                        DWORD dwKeySpec
                        )
{
    CHAR        *pszExport;
    CHAR        *pszPrivKey;
    CHAR        *pszPubKey;
    BOOL        fMachineKeySet = FALSE;
    BOOL        fRet = FALSE;

    if (pTmpUser->Rights & CRYPT_MACHINE_KEYSET)
    {
        fMachineKeySet = TRUE;
    }

    if (AT_SIGNATURE == dwKeySpec)
    {
        pszExport = "SExport";
        pszPrivKey = "SPvK";
        pszPubKey = "SPbK";
    }
    else if (AT_KEYEXCHANGE == dwKeySpec)
    {
        pszExport = "EExport";
        pszPrivKey = "EPvK";
        pszPubKey = "EPbK";
    }
    else
    {
        SetLastError((DWORD)NTE_BAD_DATA);
        goto Ret;
    }

    // if protected store is available then delete the key from there
    if (pTmpUser->pPStore)
    {
        if (!DeleteKeyFromProtectedStorage(pTmpUser, &g_Strings, dwKeySpec,
                                           fMachineKeySet, FALSE))
        {
            goto Ret;
        }
    }

    // delete stuff from the registry
#ifndef _XBOX
    RegDeleteValue(pTmpUser->hKeys, pszPrivKey);
    RegDeleteValue(pTmpUser->hKeys, pszPubKey);
    RegDeleteValue(pTmpUser->hKeys, pszExport);
#endif

    fRet = NTF_SUCCEED;
Ret:
    return fRet;
}

/*
 -  CPSetProvParam
 -
 *  Purpose:
 *                Allows applications to customize various aspects of the
 *                operations of a provider
 *
 *  Parameters:
 *               IN      hUID    -  Handle to a CSP
 *               IN      dwParam -  Parameter number
 *               IN      pbData  -  Pointer to data
 *               IN      dwFlags -  Flags values
 *
 *  Returns:
 */
BOOL CPSetProvParam(IN HCRYPTPROV hUID,
                    IN DWORD dwParam,
                    IN BYTE *pbData,
                    IN DWORD dwFlags)
{
    PNTAGUserList   pTmpUser;
    long            lsyserr;
    HCRYPTKEY       hKey = 0;
    BOOL            fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputSetProvParam(TRUE, FALSE, hUID, dwParam, pbData, dwFlags);
#endif // DBG -- NOTE:  This section not compiled for retail builds 

    if (NULL == (pTmpUser = (PNTAGUserList)NTLCheckList (hUID, USER_HANDLE)))
    {
        SetLastError((DWORD)NTE_BAD_UID);
        goto Ret;
    }

    switch (dwParam)
    {
        case PP_KEYSET_SEC_DESCR:
#ifndef _XBOX
            if ((dwFlags & ~(OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION |
                             DACL_SECURITY_INFORMATION | SACL_SECURITY_INFORMATION)) != 0)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }

            if (!(dwFlags & OWNER_SECURITY_INFORMATION) &&
                !(dwFlags & GROUP_SECURITY_INFORMATION) &&
                !(dwFlags & DACL_SECURITY_INFORMATION) &&
                !(dwFlags & SACL_SECURITY_INFORMATION))
            {
                SetLastError((DWORD)NTE_BAD_FLAGS);
                goto Ret;
            }

            // set the security descriptor for the hKey of the keyset
            if (0 != (lsyserr = SetSecurityOnContainer(
                             pTmpUser->ContInfo.rgwszFileName,
                             pTmpUser->dwProvType,
                             pTmpUser->Rights & CRYPT_MACHINE_KEYSET,
                             (SECURITY_INFORMATION)dwFlags, 
                             (PSECURITY_DESCRIPTOR)pbData)))
            {
                SetLastError(lsyserr);
                goto Ret;
            }
#endif
            ASSERT( !"PP_KEYSET_SEC_DESCR" );
            break;

        case PP_KEY_TYPE_SUBTYPE:
        {
            if (dwFlags != 0)
            {
                SetLastError((DWORD)NTE_BAD_FLAGS);
                goto Ret;
            }

            fRet = NTF_SUCCEED;
            goto Ret;
        }
        break;

        case PP_UI_PROMPT:
        {
            if (dwFlags != 0)
            {
                SetLastError((DWORD)NTE_BAD_FLAGS);
                goto Ret;
            }

            if (pTmpUser->pPStore)
            {
                if (!SetUIPrompt(pTmpUser, (LPWSTR)pbData))
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
            }
            else
            {
                SetLastError((DWORD) NTE_BAD_TYPE);
                goto Ret;
            }
        }
        break;

        case PP_DELETEKEY:
        {
            if (dwFlags != 0)
            {
                SetLastError((DWORD)NTE_BAD_FLAGS);
                goto Ret;
            }

            // check if it is a verify context
            if (pTmpUser->Rights & CRYPT_VERIFYCONTEXT)
            {
                SetLastError((DWORD)NTE_BAD_UID);
                goto Ret;
            }

            // check if the keys exists
            if (!CPGetUserKey(hUID, *((DWORD*)pbData), &hKey))
            {
                SetLastError((DWORD)NTE_NO_KEY);
                goto Ret;
            }
            // destroy the key handle right away
            CPDestroyKey(hUID, hKey);

            // delete the key
            if (!DeletePersistedKey(pTmpUser, *((DWORD*)pbData)))
            {
                goto Ret;
            }
        }
        break;

        case PP_SGC_INFO:
#ifndef _XBOX
        {
            if (dwFlags != 0)
            {
                SetLastError((DWORD)NTE_BAD_FLAGS);
                goto Ret;
            }

            // check if it is an SChannel provider
            if (PROV_RSA_SCHANNEL != pTmpUser->dwProvType)
            {
                SetLastError((DWORD)NTE_BAD_TYPE);
                goto Ret;
            }

            // check if the SGC Info (cert) is good
            if (!SetSGCInfo(pTmpUser, pbData))
            {
                SetLastError((DWORD)NTE_FAIL);
                goto Ret;
            }
        }
#endif
        ASSERT( !"PP_SGC_INFO" );
        break;

#ifdef USE_HW_RNG
#ifdef _M_IX86
        case PP_USE_HARDWARE_RNG:
        {
            if (dwFlags != 0)
            {
                SetLastError((DWORD)NTE_BAD_FLAGS);
                goto Ret;
            }

            if (!GetRNGDriverHandle(&(pTmpUser->hRNGDriver)))
            {
                goto Ret;
            }
        }
        break;
#endif // _M_IX86
#endif // USE_HW_RNG

        default:
            SetLastError((DWORD) NTE_BAD_TYPE);
            goto Ret;
            break;
    }

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputSetProvParam(FALSE, fRet, 0, 0, NULL, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds 

    return fRet;
}


/*
 -  CPGetProvParam
 -
 *  Purpose:
 *                Allows applications to get various aspects of the
 *                operations of a provider
 *
 *  Parameters:
 *               IN      hUID       -  Handle to a CSP
 *               IN      dwParam    -  Parameter number
 *               IN      pbData     -  Pointer to data
 *               IN      pdwDataLen -  Length of parameter data
 *               IN      dwFlags    -  Flags values
 *
 *  Returns:
 */
BOOL CPGetProvParam(IN HCRYPTPROV hUID,
                    IN DWORD dwParam,
                    IN BYTE *pbData,
                    IN DWORD *pwDataLen,
                    IN DWORD dwFlags)
{
    PNTAGUserList           pTmpUser;
    long                    lsyserr;
    ENUMALGS                *pEnum;
    ENUMALGSEX              *pEnumEx;
    DWORD                   cbName;
    BYTE                    *pbName;
    LPSTR                   pszName;
    DWORD                   cbTmpData;
    DWORD                   dwErr = 0;
    BOOL                    fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputGetProvParam(TRUE, FALSE, hUID, dwParam, pbData,
                               pwDataLen, dwFlags);
#endif // DBG -- NOTE:  This section not compiled for retail builds 

    if ((pTmpUser = (PNTAGUserList) NTLCheckList (hUID, USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    if (pwDataLen == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    switch (dwParam)
    {
        if (PROV_RSA_SCHANNEL == pTmpUser->dwProvType)
        {
            if ((dwFlags & ~(CRYPT_FIRST | CRYPT_NEXT | CRYPT_SGC_ENUM)) != 0)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }
        }
        else
        {
            if ((dwFlags & ~(CRYPT_FIRST | CRYPT_NEXT)) != 0)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }
        }

        case PP_ENUMALGS:
            switch(pTmpUser->dwProvType)
            {
                case PROV_RSA_FULL:
#ifdef STRONG
                    if (pTmpUser->fNewStrongCSP)
                    {
                        pEnum = (ENUMALGS*)EnumFullNewStrong;
                    }
                    else
                    {
                        pEnum = (ENUMALGS*)EnumFull;
                    }
#else
                    if (pTmpUser->Rights & CRYPT_IN_FRANCE)
                    {
                        pEnum = (ENUMALGS*)EnumFullFrance;
                    }
                    else
                    {
                        pEnum = (ENUMALGS*)EnumFull;
                    }
                    
#endif // STRONG
                    break;


                case PROV_RSA_SIG:
                    pEnum = (ENUMALGS*)EnumSig;
                    break;

                case PROV_RSA_SCHANNEL:
#ifdef STRONG
                    pEnum = (ENUMALGS*)EnumSch;
#else
                    if ((0 != pTmpUser->dwSGCFlags) || (dwFlags & CRYPT_SGC_ENUM))
                    {
                        pEnum = (ENUMALGS*)EnumSchSGC;
                    }
                    else
                    {
                        pEnum = (ENUMALGS*)EnumSch;
                    }
#endif
                    break;
            }

            if (dwFlags & CRYPT_FIRST)
            {
                pTmpUser->dwEnumalgs = 0;
            }
            else if (0xFFFFFFFF == pTmpUser->dwEnumalgs)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }


            if (pEnum[pTmpUser->dwEnumalgs].aiAlgid == 0)
            {
                SetLastError(ERROR_NO_MORE_ITEMS);
                goto Ret;
            }

            if (pbData == NULL || *pwDataLen < sizeof(pEnum[0]))
            {
                *pwDataLen = sizeof(pEnum[0]);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            // each entry in ENUMALGS is of fixed size
            memcpy(pbData, &pEnum[pTmpUser->dwEnumalgs], sizeof(pEnum[0]));
            *pwDataLen = sizeof(pEnum[0]);

            pTmpUser->dwEnumalgs++;

            break;

        case PP_ENUMALGS_EX:
            if (PROV_RSA_SCHANNEL == pTmpUser->dwProvType)
            {
                if ((dwFlags & ~(CRYPT_FIRST | CRYPT_NEXT | CRYPT_SGC_ENUM)) != 0)
                {
                    SetLastError((DWORD) NTE_BAD_FLAGS);
                    goto Ret;
                }
            }
            else
            {
                if ((dwFlags & ~(CRYPT_FIRST | CRYPT_NEXT)) != 0)
                {
                    SetLastError((DWORD) NTE_BAD_FLAGS);
                    goto Ret;
                }
            }

            switch(pTmpUser->dwProvType)
            {
                case PROV_RSA_FULL:
#ifdef STRONG
                    if (pTmpUser->fNewStrongCSP)
                    {
                        pEnumEx = (ENUMALGSEX*)EnumExFullNewStrong;
                    }
                    else
                    {
                        pEnumEx = (ENUMALGSEX*)EnumExFull;
                    }
#else
                    if (pTmpUser->Rights & CRYPT_IN_FRANCE)
                    {
                        pEnumEx = (ENUMALGSEX*)EnumExFullFrance;
                    }
                    else
                    {
                        pEnumEx = (ENUMALGSEX*)EnumExFull;
                    }
#endif // STRONG
                    break;

                case PROV_RSA_SIG:
                    pEnumEx = (ENUMALGSEX*)EnumExSig;
                    break;

                case PROV_RSA_SCHANNEL:
#ifdef STRONG
                    pEnumEx = (ENUMALGSEX*)EnumExSch;
#else
                    if ((0 != pTmpUser->dwSGCFlags) || (dwFlags & CRYPT_SGC_ENUM))
                    {
                        pEnumEx = (ENUMALGSEX*)EnumExSchSGC;
                    }
                    else
                    {
                        if (pTmpUser->Rights & CRYPT_IN_FRANCE)
                        {
                            pEnumEx = (ENUMALGSEX*)EnumExSchFrance;
                        }
                        else
                        {
                            pEnumEx = (ENUMALGSEX*)EnumExSch;
                        }
                    }
#endif
                    break;
            }

            if (dwFlags & CRYPT_FIRST)
            {
                pTmpUser->dwEnumalgsEx = 0;
            }
            else if (0xFFFFFFFF == pTmpUser->dwEnumalgsEx)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }

            if (pEnumEx[pTmpUser->dwEnumalgsEx].aiAlgid == 0)
            {
                SetLastError(ERROR_NO_MORE_ITEMS);
                goto Ret;
            }

            if (pbData == NULL || *pwDataLen < sizeof(pEnumEx[0]))
            {
                *pwDataLen = sizeof(pEnumEx[0]);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            // each entry in ENUMALGSEX is of fixed size
            memcpy(pbData, &pEnumEx[pTmpUser->dwEnumalgsEx], sizeof(pEnumEx[0]));
            *pwDataLen = sizeof(pEnumEx[0]);

            pTmpUser->dwEnumalgsEx++;

            break;

        case PP_ENUMCONTAINERS:
#ifndef _XBOX
            {
                BOOL fMachineKeySet = pTmpUser->Rights & CRYPT_MACHINE_KEYSET;

                if ((dwFlags & ~(CRYPT_FIRST | CRYPT_NEXT)) != 0)
                {
                    SetLastError((DWORD) NTE_BAD_FLAGS);
                    goto Ret;
                }

                if (dwFlags & CRYPT_FIRST)
                {
                    if (0 != pTmpUser->ContInfo.hFind)
                    {
                        FindClose(pTmpUser->ContInfo.hFind);
                        pTmpUser->ContInfo.hFind = 0;
                    }
                    FreeEnumOldMachKeyEntries(&pTmpUser->ContInfo);
                    FreeEnumRegEntries(&pTmpUser->ContInfo);

                    pTmpUser->ContInfo.fCryptFirst = TRUE;
                    pTmpUser->ContInfo.fNoMoreFiles = FALSE;
                }
                else if (FALSE == pTmpUser->ContInfo.fCryptFirst)
                {
                    SetLastError((DWORD) NTE_BAD_FLAGS);
                    goto Ret;
                }

                if (!pTmpUser->ContInfo.fNoMoreFiles)
                {
                    dwErr = GetNextContainer(pTmpUser->dwProvType,
                                             fMachineKeySet, 
                                             dwFlags,
                                             (LPSTR)pbData,
                                             pwDataLen,
                                             &pTmpUser->ContInfo.hFind);
                }

                if ((0 != dwErr) || pTmpUser->ContInfo.fNoMoreFiles)
                {
                    if ((0 != dwErr) && (ERROR_NO_MORE_ITEMS != dwErr))
                    {
                        SetLastError(dwErr);
                        goto Ret;
                    }

                    pTmpUser->ContInfo.fNoMoreFiles = TRUE;

                    if (fMachineKeySet)
                    {
                        if (0 != (dwErr = EnumOldMachineKeys(pTmpUser->dwProvType,
                                                             &pTmpUser->ContInfo)))
                        {
                            if (ERROR_NO_MORE_ITEMS != dwErr)
                            {
                                SetLastError((DWORD)dwErr);
                                goto Ret;
                            }
                        }
                    }
                    if (0 != (dwErr = EnumRegKeys(&pTmpUser->ContInfo,
                                                  fMachineKeySet,
                                                  pTmpUser->dwProvType,
                                                  pbData,
                                                  pwDataLen)))
                    {
                        SetLastError((DWORD)dwErr);
                        goto Ret;
                    }

                    cbTmpData = *pwDataLen;
                    if ((!fMachineKeySet) || 
                        (0 != (dwErr = GetNextEnumedOldMachKeys(&pTmpUser->ContInfo,
                                              fMachineKeySet,
                                              pTmpUser->dwProvType,
                                              pbData,
                                              &cbTmpData))))
                    {
                        if (0 != (dwErr = GetNextEnumedRegKeys(&pTmpUser->ContInfo,
                                                  fMachineKeySet,
                                                  pTmpUser->dwProvType,
                                                  pbData,
                                                  pwDataLen)))
                        {
                            SetLastError((DWORD)dwErr);
                            goto Ret;
                        }
                    }
                    else
                    {
                        *pwDataLen = cbTmpData;
                    }
                }
            }
#endif
            ASSERT (!"PP_ENUMCONTAINERS");
            break;

        case PP_IMPTYPE:

            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            *pwDataLen = sizeof(DWORD);

            *((DWORD *) pbData) = CRYPT_IMPL_SOFTWARE;

            break;

        case PP_NAME:
            switch(pTmpUser->dwProvType)
            {
                case PROV_RSA_FULL:
#ifndef STRONG
                    cbName = sizeof(MS_DEF_PROV);
                    pbName = MS_DEF_PROV;
#else   // STRONG                   
                    if (pTmpUser->fNewStrongCSP)
                    {
                        cbName = sizeof(MS_STRONG_PROV);
                        pbName = MS_STRONG_PROV;
                    }
                    else
                    {
                        cbName = sizeof(MS_ENHANCED_PROV);
                        pbName = MS_ENHANCED_PROV;
                    }
#endif  // STRONG
                    break;

                case PROV_RSA_SIG:
                    cbName = sizeof(PROV_SIG);
                    pbName = PROV_SIG;
                    break;

                case PROV_RSA_SCHANNEL:
                    cbName = sizeof(PROV_SCHANNEL);
                    pbName = PROV_SCHANNEL;
                    break;
            }

            if (pbData == NULL || *pwDataLen < cbName)
            {
                *pwDataLen = cbName;

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            *pwDataLen = cbName;

            memcpy(pbData, pbName, cbName);

            break;

        case PP_VERSION:
            if (dwFlags != 0)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }

            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            *pwDataLen = sizeof(DWORD);

            *((DWORD *) pbData) = 0x200;

            break;

        case PP_CONTAINER:
            pszName = pTmpUser->ContInfo.pszUserName;

            if (pbData == NULL || *pwDataLen < (strlen(pszName) + 1))
            {
                *pwDataLen = strlen(pszName) + 1;

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            *pwDataLen = strlen(pszName) + 1;

            strcpy(pbData, pszName);
            break;

        case PP_UNIQUE_CONTAINER:
            if (0 != (dwErr = GetUniqueContainerName(&pTmpUser->ContInfo,
                                                     pbData,
                                                     pwDataLen)))
            {
                SetLastError(dwErr);
                goto Ret;
            }
            break;

        case PP_KEYSET_SEC_DESCR:
#ifndef _XBOX
            if ((dwFlags & ~(OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION |
                             DACL_SECURITY_INFORMATION | SACL_SECURITY_INFORMATION)) != 0)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }

            if (!(dwFlags & OWNER_SECURITY_INFORMATION) &&
                !(dwFlags & GROUP_SECURITY_INFORMATION) &&
                !(dwFlags & DACL_SECURITY_INFORMATION) &&
                !(dwFlags & SACL_SECURITY_INFORMATION))
            {
                SetLastError((DWORD)NTE_BAD_FLAGS);
                goto Ret;
            }

            if (0 != (lsyserr = GetSecurityOnContainer(
                                    pTmpUser->ContInfo.rgwszFileName,
                                    pTmpUser->dwProvType,
                                    pTmpUser->Rights & CRYPT_MACHINE_KEYSET,
                                    (SECURITY_INFORMATION)dwFlags, 
                                    (PSECURITY_DESCRIPTOR)pbData,
                                    pwDataLen)))
            {
                SetLastError(lsyserr);
                goto Ret;
            }
#endif
            ASSERT( !"PP_KEYSET_SEC_DESCR" );
            break;

        case PP_KEYSTORAGE:
            if (dwFlags != 0)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }

            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            *pwDataLen = sizeof(DWORD);

            if (pTmpUser->pPStore)
                *((DWORD*)pbData) = CRYPT_PSTORE | CRYPT_UI_PROMPT | CRYPT_SEC_DESCR;
            else
                *((DWORD*)pbData) = CRYPT_SEC_DESCR;

            break;

        case PP_PROVTYPE:
            if (dwFlags != 0)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }

            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            *pwDataLen = sizeof(DWORD);
            *((DWORD*)pbData) = pTmpUser->dwProvType;

            break;

        case PP_KEYSET_TYPE:
            if (dwFlags != 0)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }

            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            *pwDataLen = sizeof(DWORD);

            if (pTmpUser->Rights & CRYPT_MACHINE_KEYSET)
                *((DWORD*)pbData) = CRYPT_MACHINE_KEYSET;
            else
                *((DWORD*)pbData) = 0;

            break;

        case PP_SIG_KEYSIZE_INC:
        case PP_KEYX_KEYSIZE_INC:
            if (dwFlags != 0)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }

            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            *pwDataLen = sizeof(DWORD);

            *((DWORD*)pbData) = RSA_KEYSIZE_INC;
            break;

        case PP_SGC_INFO:
        {
            if (dwFlags != 0)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }

            // check if it is an SChannel provider
            if (PROV_RSA_SCHANNEL != pTmpUser->dwProvType)
            {
                SetLastError((DWORD)NTE_BAD_TYPE);
                goto Ret;
            }

            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            *pwDataLen = sizeof(DWORD);
            // return the SGC Flags
            *((DWORD*)pbData) = pTmpUser->dwSGCFlags;
        }
        break;

#ifdef USE_HW_RNG
#ifdef _M_IX86
        case PP_USE_HARDWARE_RNG:
        {
            if (dwFlags != 0)
            {
                SetLastError((DWORD)NTE_BAD_FLAGS);
                goto Ret;
            }

            *pwDataLen = 0;

            // check if the hardware RNG is available for use
            if (!CheckIfRNGAvailable())
            {
                goto Ret;
            }
        }
        break;
#endif // _M_IX86
#endif // USE_HW_RNG

        case PP_KEYSPEC:
            if (dwFlags != 0)
            {
                SetLastError((DWORD) NTE_BAD_FLAGS);
                goto Ret;
            }

            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = TRUE;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            *pwDataLen = sizeof(DWORD);

            if (PROV_RSA_SIG == pTmpUser->dwProvType)
            {
                *((DWORD*)pbData) = AT_SIGNATURE;
            }
            else if (PROV_RSA_FULL == pTmpUser->dwProvType)
            {
                *((DWORD*)pbData) = AT_SIGNATURE | AT_KEYEXCHANGE;
            }
            else if (PROV_RSA_SCHANNEL == pTmpUser->dwProvType)
            {
                *((DWORD*)pbData) = AT_KEYEXCHANGE;
            }
            break;

        case PP_ENUMEX_SIGNING_PROT:
            *pwDataLen = 0;
            break;

        default:
            SetLastError((DWORD) NTE_BAD_TYPE);
            goto Ret;
            break;

    }

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputGetProvParam(FALSE, fRet, 0, 0, NULL, NULL, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds 

    return fRet;

}


/*
 -  CPSetHashParam
 -
 *  Purpose:
 *                Allows applications to customize various aspects of the
 *                operations of a hash
 *
 *  Parameters:
 *               IN      hUID    -  Handle to a CSP
 *               IN      hHash   -  Handle to a hash
 *               IN      dwParam -  Parameter number
 *               IN      pbData  -  Pointer to data
 *               IN      dwFlags -  Flags values
 *
 *  Returns:
 */
BOOL CPSetHashParam(IN HCRYPTPROV hUID,
                    IN HCRYPTHASH hHash,
                    IN DWORD dwParam,
                    IN BYTE *pbData,
                    IN DWORD dwFlags)
{
    PNTAGHashList       pTmpHash;
    PNTAGKeyList        pTmpKey;
    MD4_object          *pMD4Hash;
    MD5_object          *pMD5Hash;
    A_SHA_CTX           *pSHAHash;
    MACstate            *pMAC;
    PHMAC_INFO          pHMACInfo;
    BYTE                *pb;
    BOOL                fExportable = FALSE;
    BOOL                fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputSetHashParam(TRUE, FALSE, hUID, hHash, dwParam,
                               pbData, dwFlags);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (dwFlags != 0)
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    // check the user identification
    if (NTLCheckList ((HNTAG)hUID, USER_HANDLE) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    if ((pTmpHash = (PNTAGHashList) NTLValidate(hHash, hUID,
                                                HASH_HANDLE)) == NULL)
    {
        if (GetLastError() == NTE_FAIL)
            SetLastError((DWORD) NTE_BAD_HASH);

        goto Ret;
    }

    switch (dwParam)
    {
        case HP_HASHVAL:

            switch (pTmpHash->Algid)
            {
#ifdef CSP_USE_MD2
                case CALG_MD2:
                {
                    MD2_object      *pMD2Hash;

                    pMD2Hash = (MD2_object *) pTmpHash->pHashData;

                    if (pMD2Hash->FinishFlag == TRUE)
                    {
                        SetLastError((DWORD) NTE_BAD_HASH_STATE);
                        goto Ret;
                    }

                    memcpy (&pMD2Hash->MD.state, pbData, MD2DIGESTLEN);

                    break;
                }
#endif

#ifdef CSP_USE_MD4
                case CALG_MD4:

                    pMD4Hash = (MD4_object *) pTmpHash->pHashData;

                    if (pMD4Hash->FinishFlag == TRUE)
                    {
                        SetLastError((DWORD) NTE_BAD_HASH_STATE);
                        goto Ret;
                    }

                    memcpy (&pMD4Hash->MD, pbData, MD4DIGESTLEN);

                    break;
#endif

#ifdef CSP_USE_MD5
                case CALG_MD5:

                    pMD5Hash = (MD5_object *) pTmpHash->pHashData;

                    if (pMD5Hash->FinishFlag == TRUE)
                    {
                        SetLastError((DWORD) NTE_BAD_HASH_STATE);
                        goto Ret;
                    }

                    memcpy (pMD5Hash->digest, pbData, MD5DIGESTLEN);

                    break;
#endif

#ifdef CSP_USE_SHA
                case CALG_SHA:

                    pSHAHash = (A_SHA_CTX *) pTmpHash->pHashData;

                    if (pSHAHash->FinishFlag == TRUE)
                    {
                        SetLastError((DWORD) NTE_BAD_HASH_STATE);
                        goto Ret;
                    }

                    memcpy (pSHAHash->HashVal, pbData, A_SHA_DIGEST_LEN);

                    break;
#endif

#ifdef CSP_USE_SSL3SHAMD5
                case CALG_SSL3_SHAMD5:
                    memcpy (pTmpHash->pHashData, pbData, SSL3_SHAMD5_LEN);

                    break;
#endif

#ifdef CSP_USE_MAC
                case CALG_MAC:

                    pMAC = (MACstate *)pTmpHash->pHashData;

                    if ((pTmpKey = (PNTAGKeyList) NTLValidate(pMAC->hKey,
                                                              hUID,
                                                         KEY_HANDLE)) == NULL)
                    {
                        if (GetLastError() == NTE_FAIL)
                        {
                            SetLastError((DWORD) NTE_BAD_KEY);
                        }
                        goto Ret;
                    }

                    if (pMAC->FinishFlag == TRUE)
                    {
                        SetLastError((DWORD) NTE_BAD_HASH_STATE);
                        goto Ret;
                    }

                    memcpy(pTmpKey->FeedBack, pbData, CRYPT_BLKLEN);

                    break;
#endif

                default:
                    SetLastError((DWORD) NTE_BAD_ALGID);
                    goto Ret;

            }
            pTmpHash->dwHashState |= DATA_IN_HASH;
            break;

        case HP_HMAC_INFO:
            if (CALG_HMAC != pTmpHash->Algid)
            {
                SetLastError((DWORD) NTE_BAD_TYPE);
                goto Ret;
            }

            pHMACInfo = (PHMAC_INFO)pbData;

            pTmpHash->HMACAlgid = pHMACInfo->HashAlgid;

            // now that we know the type of hash we can create it
            if (!LocalCreateHash(pTmpHash->HMACAlgid, (BYTE**)&pTmpHash->pHashData,
                                 &pTmpHash->dwDataLen))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            // if the length of the inner string is 0 then use the default string
            if (0 == pHMACInfo->cbInnerString)
                pTmpHash->cbHMACInner = HMAC_DEFAULT_STRING_LEN;
            else
                pTmpHash->cbHMACInner = pHMACInfo->cbInnerString;

            if (NULL == (pb = _nt_malloc(pTmpHash->cbHMACInner)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            if (0 == pHMACInfo->cbInnerString)
                memset(pb, 0x36, pTmpHash->cbHMACInner);
            else
                memcpy(pb, pHMACInfo->pbInnerString, pTmpHash->cbHMACInner);

            if (pTmpHash->pbHMACInner)
                _nt_free(pTmpHash->pbHMACInner, pTmpHash->cbHMACInner);
            pTmpHash->pbHMACInner = pb;

            // if the length of the outer string is 0 then use the default string
            if (0 == pHMACInfo->cbOuterString)
                pTmpHash->cbHMACOuter = HMAC_DEFAULT_STRING_LEN;
            else
                pTmpHash->cbHMACOuter = pHMACInfo->cbOuterString;

            if (NULL == (pb = _nt_malloc(pTmpHash->cbHMACOuter)))
            {
                _nt_free(pTmpHash->pbHMACInner, pTmpHash->cbHMACInner);
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            if (0 == pHMACInfo->cbOuterString)
                memset(pb, 0x5C, pTmpHash->cbHMACOuter);
            else
                memcpy(pb, pHMACInfo->pbOuterString, pTmpHash->cbHMACOuter);

            if (pTmpHash->pbHMACOuter)
                _nt_free(pTmpHash->pbHMACOuter, pTmpHash->cbHMACOuter);
            pTmpHash->pbHMACOuter = pb;
            break;

#ifdef CSP_USE_SSL3
            case HP_TLS1PRF_LABEL:
            case HP_TLS1PRF_SEED:
            {
                if (CALG_TLS1PRF != pTmpHash->Algid)
                {
                    SetLastError((DWORD)NTE_BAD_HASH);
                    goto Ret;
                }

                if (!SetPRFHashParam((PRF_HASH*)pTmpHash->pHashData,
                                     dwParam, pbData))
                {
                    goto Ret;
                }

                pTmpHash->dwHashState |= DATA_IN_HASH;
                break;
            }
#endif

        default:
            SetLastError((DWORD) NTE_BAD_TYPE);
            goto Ret;
            break;
    }

    if (dwParam == HP_HASHVAL)
        pTmpHash->HashFlags |= HF_VALUE_SET;

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputSetHashParam(FALSE, fRet, 0, 0, 0, NULL, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    return fRet;
}

BOOL LocalGetHashVal(
                     IN ALG_ID Algid,
                     IN DWORD dwHashFlags,
                     IN OUT BYTE *pbHashData,
                     OUT BYTE *pbHashVal,
                     OUT DWORD *pcbHashVal
                     )
{
    MD2_object      *pMD2Hash;
    MD4_object      *pMD4Hash;
    MD5_object      *pMD5Hash;
    A_SHA_CTX       *pSHAHash;
    BOOL            fRet = FALSE;

    switch (Algid)
    {
#ifdef CSP_USE_MD2
        case CALG_MD2:
            // make sure there's enough room.
            if (pbHashVal == NULL || *pcbHashVal < MD2DIGESTLEN)
            {
                *pcbHashVal = MD2DIGESTLEN;
                if (pbHashVal == NULL)
                    fRet = TRUE;
                else
                    SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            pMD2Hash = (MD2_object *)pbHashData;

            if ((dwHashFlags & HF_VALUE_SET) == 0)
            {
                if (pMD2Hash->FinishFlag == TRUE)
                {
                    *pcbHashVal = MD2DIGESTLEN;
                    memcpy(pbHashVal, pMD2Hash->MD.state, MD2DIGESTLEN);
                    break;
                }

                // set the finish flag on the hash and
                // process what's left in the buffer.
                pMD2Hash->FinishFlag = TRUE;

                // Finish offthe hash
                MD2Final(&pMD2Hash->MD);
            }

            *pcbHashVal = MD2DIGESTLEN;
            memcpy (pbHashVal, pMD2Hash->MD.state, MD2DIGESTLEN);

            break;
#endif

#ifdef CSP_USE_MD4
        case CALG_MD4:
            // make sure there's enough room.
            if (pbHashVal == NULL || *pcbHashVal < MD4DIGESTLEN)
            {
                *pcbHashVal = MD4DIGESTLEN;
                if (pbHashVal == NULL)
                    fRet = TRUE;
                else
                    SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            pMD4Hash = (MD4_object *)pbHashData;

            if ((dwHashFlags & HF_VALUE_SET) == 0)
            {
                if (pMD4Hash->FinishFlag == TRUE)
                {
                    *pcbHashVal = MD4DIGESTLEN;
                    memcpy(pbHashVal, &pMD4Hash->MD, *pcbHashVal);
                    break;
                }

                // set the finish flag on the hash and
                // process what's left in the buffer.
                pMD4Hash->FinishFlag = TRUE;

                if (MD4_SUCCESS != MDupdate(&pMD4Hash->MD, pMD4Hash->Buf,
                                            MD4BYTESTOBITS(pMD4Hash->BufLen)))
                {
                    SetLastError((DWORD) NTE_FAIL);
                    goto Ret;
                }
            }

            *pcbHashVal = MD4DIGESTLEN;
            memcpy(pbHashVal, &pMD4Hash->MD, *pcbHashVal);

            break;
#endif

#ifdef CSP_USE_MD5
        case CALG_MD5:
            // make sure there's enough room.
            if (pbHashVal == NULL || *pcbHashVal < MD5DIGESTLEN)
            {
                *pcbHashVal = MD5DIGESTLEN;
                if (pbHashVal == NULL)
                    fRet = TRUE;
                else
                    SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            pMD5Hash = (MD5_object *)pbHashData;

            if ((dwHashFlags & HF_VALUE_SET) == 0)
            {
                if (pMD5Hash->FinishFlag == TRUE)
                {
                    *pcbHashVal = MD5DIGESTLEN;
                    memcpy (pbHashVal, pMD5Hash->digest, MD5DIGESTLEN);
                    break;
                }

                // set the finish flag on the hash and
                // process what's left in the buffer.
                pMD5Hash->FinishFlag = TRUE;

                // Finish offthe hash
                MD5Final(pMD5Hash);
            }

            *pcbHashVal = MD5DIGESTLEN;
            memcpy (pbHashVal, pMD5Hash->digest, MD5DIGESTLEN);

            break;
#endif

#ifdef CSP_USE_SHA
        case CALG_SHA:
            // make sure there's enough room.
            if (pbHashVal == NULL || *pcbHashVal < A_SHA_DIGEST_LEN)
            {
                *pcbHashVal = A_SHA_DIGEST_LEN;
                if (pbHashVal == NULL)
                    fRet = TRUE;
                else
                    SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            pSHAHash = (A_SHA_CTX *)pbHashData;

            if ((dwHashFlags & HF_VALUE_SET) == 0)
            {
                if (pSHAHash->FinishFlag == TRUE)
                {
                    *pcbHashVal = A_SHA_DIGEST_LEN;
                    memcpy (pbHashVal, pSHAHash->HashVal, A_SHA_DIGEST_LEN);
                    break;
                }

                // set the finish flag on the hash and
                // process what's left in the buffer.
                pSHAHash->FinishFlag = TRUE;

                // Finish off the hash
                A_SHAFinal(pSHAHash, pSHAHash->HashVal);
            }

            *pcbHashVal = A_SHA_DIGEST_LEN;
            memcpy (pbHashVal, pSHAHash->HashVal, A_SHA_DIGEST_LEN);

            break;
#endif

        default:
            SetLastError((DWORD)NTE_BAD_ALGID);
            goto Ret;
    }

    fRet = TRUE;
Ret:
    return fRet;
}

void GetHashLength(IN ALG_ID Algid,
                   OUT DWORD *pcb)
{
    switch (Algid)
    {
#ifdef CSP_USE_MD2
        case CALG_MD2:
            *pcb = MD2DIGESTLEN;
            break;
#endif

#ifdef CSP_USE_MD4
        case CALG_MD4:
            *pcb = MD4DIGESTLEN;
            break;
#endif

#ifdef CSP_USE_MD5
        case CALG_MD5:
            *pcb = MD5DIGESTLEN;
            break;
#endif

#ifdef CSP_USE_SHA
        case CALG_SHA:
            *pcb = A_SHA_DIGEST_LEN;
            break;
#endif
    }
}

/*
 -  CPGetHashParam
 -
 *  Purpose:
 *                Allows applications to get various aspects of the
 *                operations of a key
 *
 *  Parameters:
 *               IN      hUID       -  Handle to a CSP
 *               IN      hHash      -  Handle to a hash
 *               IN      dwParam    -  Parameter number
 *               IN      pbData     -  Pointer to data
 *               IN      pdwDataLen -  Length of parameter data
 *               IN      dwFlags    -  Flags values
 *
 *  Returns:
 */
BOOL CPGetHashParam(IN HCRYPTPROV hUID,
                    IN HCRYPTHASH hHash,
                    IN DWORD dwParam,
                    IN BYTE *pbData,
                    IN DWORD *pwDataLen,
                    IN DWORD dwFlags)
{
    PNTAGHashList   pTmpHash;
    MACstate        *pMAC;
    BOOL            f;
    BYTE            MACbuf[2*CRYPT_BLKLEN];
    PNTAGKeyList    pTmpKey;

    BYTE            rgbFinalHash[A_SHA_DIGEST_LEN];
    DWORD           cbFinalHash;
    DWORD           cb;
    BYTE            *pb = NULL;
    DWORD           cbHashData;
    BYTE            *pbHashData = NULL;
    DWORD           i;

    BOOL            fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputGetHashParam(TRUE, FALSE, hUID, hHash, dwParam,
                               pbData, pwDataLen, dwFlags);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (dwFlags != 0)
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    // check the user identification
    if (NTLCheckList ((HNTAG)hUID, USER_HANDLE) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    if (pwDataLen == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    if ((pTmpHash = (PNTAGHashList) NTLValidate(hHash, hUID,
                                                HASH_HANDLE)) == NULL)
    {
        if (GetLastError() == NTE_FAIL)
            SetLastError((DWORD) NTE_BAD_HASH);

        goto Ret;
    }

    switch (dwParam)
    {

        case HP_ALGID:
            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            *((DWORD *) pbData) = pTmpHash->Algid;
            *pwDataLen = sizeof(DWORD);
            break;

        case HP_HASHSIZE:
            if (pbData == NULL || *pwDataLen < sizeof(DWORD))
            {
                *pwDataLen = sizeof(DWORD);

                if (pbData == NULL)
                {
                    fRet = NTF_SUCCEED;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }

            switch (pTmpHash->Algid)
            {
                case CALG_MD2:
                case CALG_MD4:
                case CALG_MD5:
                case CALG_SHA:
                    GetHashLength(pTmpHash->Algid, (DWORD *)pbData);
                    break;

#ifdef CSP_USE_MAC
                case CALG_MAC:
                    *((DWORD *) pbData) = CRYPT_BLKLEN;
                    break;
#endif // CSP_USE_MAC

                case CALG_HMAC:
                    GetHashLength(pTmpHash->HMACAlgid, (DWORD *)pbData);
                    break;

#ifdef CSP_USE_SSL3SHAMD5
                case CALG_SSL3_SHAMD5:
                    *((DWORD *) pbData) = SSL3_SHAMD5_LEN;

                    break;
#endif

                default:
                    SetLastError((DWORD) NTE_BAD_ALGID);
                    goto Ret;
            }

            *pwDataLen = sizeof(DWORD);
            break;

        case HP_HASHVAL:
            switch (pTmpHash->Algid)
            {
#ifdef CSP_USE_SSL3SHAMD5
                case CALG_SSL3_SHAMD5:

                    // make sure there's enough room.
                    if (pbData == NULL || *pwDataLen < SSL3_SHAMD5_LEN)
                    {
                        *pwDataLen = SSL3_SHAMD5_LEN;
                        if (pbData == NULL)
                        {
                            fRet = NTF_SUCCEED;
                            goto Ret;
                        }
                        SetLastError(ERROR_MORE_DATA);
                        goto Ret;
                    }

                    // Hash value must have already been set.
                    if ((pTmpHash->HashFlags & HF_VALUE_SET) == 0)
                    {
                        SetLastError((DWORD) NTE_BAD_HASH_STATE);
                        goto Ret;
                    }

                    *pwDataLen = SSL3_SHAMD5_LEN;
                    memcpy (pbData, pTmpHash->pHashData, SSL3_SHAMD5_LEN);

                    break;
#endif

#ifdef CSP_USE_MAC
                case CALG_MAC:

                    pMAC = (MACstate *)pTmpHash->pHashData;

                    if ((pTmpKey = (PNTAGKeyList) NTLValidate(pMAC->hKey,
                                                              hUID,
                                                         KEY_HANDLE)) == NULL)
                    {
                        if (GetLastError() == NTE_FAIL)
                        {
                            SetLastError((DWORD) NTE_BAD_KEY);
                        }
                        goto Ret;
                    }

                    // make sure there is enough room.
                    if (pbData == NULL || (*pwDataLen < CRYPT_BLKLEN))
                    {
                        *pwDataLen = CRYPT_BLKLEN;
                        if (pbData == NULL)
                        {
                            fRet = NTF_SUCCEED;
                            goto Ret;
                        }
                        SetLastError(ERROR_MORE_DATA);
                        goto Ret;
                    }

                    if (pMAC->FinishFlag == TRUE)
                    {
                        *pwDataLen = CRYPT_BLKLEN;
                        memcpy(pbData, pTmpKey->FeedBack, CRYPT_BLKLEN);
                        break;
                    }

                    // set the finish flag on the hash and
                    // process what's left in the buffer.
                    pMAC->FinishFlag = TRUE;

                    if (pMAC->dwBufLen)
                    {
                        memset(MACbuf, 0, 2*CRYPT_BLKLEN);
                        memcpy(MACbuf, pMAC->Buffer, pMAC->dwBufLen);

                        switch (pTmpKey->Algid)
                        {
                            case CALG_RC2:
                            if (BlockEncrypt(RC2, pTmpKey, RC2_BLOCKLEN, TRUE,
                                             MACbuf,  &pMAC->dwBufLen,
                                             2*CRYPT_BLKLEN) == NTF_FAILED)
                            {
                                goto Ret;
                            }
                            break;

                            case CALG_DES:
                            if (BlockEncrypt(des, pTmpKey, DES_BLOCKLEN, TRUE,
                                             MACbuf,  &pMAC->dwBufLen,
                                             2*CRYPT_BLKLEN) == NTF_FAILED)
                            {
                                goto Ret;
                            }
                            break;
#ifdef CSP_USE_3DES
                            case CALG_3DES_112:
                            case CALG_3DES:
                                if (BlockEncrypt(tripledes, pTmpKey, DES_BLOCKLEN,
                                                 TRUE, MACbuf,  &pMAC->dwBufLen,
                                                 2*CRYPT_BLKLEN) == NTF_FAILED)
                                {
                                    goto Ret;
                                }
                                break;
#endif
                        }
                    }

                    *pwDataLen = CRYPT_BLKLEN;
                    memcpy(pbData, pTmpKey->FeedBack, CRYPT_BLKLEN);

                    break;
#endif

                case CALG_HMAC:
                {
                    if (!(pTmpHash->HMACState & HMAC_FINISHED))
                    {
                        cbFinalHash = sizeof(rgbFinalHash);
                        if (!LocalGetHashVal(pTmpHash->HMACAlgid, pTmpHash->HashFlags,
                                             pTmpHash->pHashData, rgbFinalHash, &cbFinalHash))
                        {
                            goto Ret;
                        }

                        // now XOR the outer string with the key and hash
                        // over this and the inner hash
                        if ((pTmpKey = (PNTAGKeyList) NTLValidate(pTmpHash->hKey, hUID,
                                                                  KEY_HANDLE)) == NULL)
                        {
                            if (GetLastError() == NTE_FAIL)
                            {
                                SetLastError((DWORD) NTE_BAD_KEY);
                            }
                            goto Ret;
                        }

                        if (pTmpKey->cbKeyLen < pTmpHash->cbHMACOuter)
                            cb = pTmpHash->cbHMACOuter;
                        else
                            cb = pTmpKey->cbKeyLen;

                        if (NULL == (pb = (BYTE *)_nt_malloc(cb)))
                        {
                            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                            goto Ret;
                        }
                        memcpy(pb, pTmpHash->pbHMACOuter, pTmpHash->cbHMACOuter);

                        // currently no support for byte reversed keys with HMAC
                        for(i=0;i<pTmpKey->cbKeyLen;i++)
                            pb[i] ^= (pTmpKey->pKeyValue)[i];

                        if (!LocalCreateHash(pTmpHash->HMACAlgid, &pbHashData, &cbHashData))
                        {
                            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                            goto Ret;
                        }

                        if (!LocalHashData(pTmpHash->HMACAlgid, pbHashData, pb, cb))
                        {
                            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                            goto Ret;
                        }

                        if (!LocalHashData(pTmpHash->HMACAlgid, pbHashData, rgbFinalHash,
                                           cbFinalHash))
                        {
                            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                            goto Ret;
                        }
                        _nt_free(pTmpHash->pHashData, pTmpHash->dwDataLen);
                        pTmpHash->dwDataLen = cbHashData;
                        pTmpHash->pHashData = pbHashData;
                        pbHashData = NULL;

                        pTmpHash->HMACState |= HMAC_FINISHED;
                    }

                    if (!LocalGetHashVal(pTmpHash->HMACAlgid, pTmpHash->HashFlags,
                                        pTmpHash->pHashData, pbData, pwDataLen))
                    {
                        goto Ret;
                    }

                }
                break;

#ifdef CSP_USE_SSL3
                case CALG_TLS1PRF:
                {
                    if (!CalculatePRF((PRF_HASH*)pTmpHash->pHashData,
                                      pbData, pwDataLen))
                    {
                        goto Ret;
                    }
                    break;
                }
#endif

                default:
                    if (!LocalGetHashVal(pTmpHash->Algid, pTmpHash->HashFlags,
                                         pTmpHash->pHashData, pbData, pwDataLen))
                    {
                        goto Ret;
                    }
            }

            break;

        default:
            SetLastError((DWORD) NTE_BAD_TYPE);
            goto Ret;
            break;

    }

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputGetHashParam(FALSE, fRet, 0, 0, 0, pbData, pwDataLen, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (pb)
        _nt_free(pb, cb);
    if (pbHashData)
        _nt_free(pbHashData, cbHashData);

    return fRet;

}

BOOL CopyKey(
             IN PNTAGKeyList pOldKey,
             OUT PNTAGKeyList *ppNewKey)
{
    PNTAGKeyList    pNewKey;
    BOOL            fRet = FALSE;

    if (NULL == (pNewKey = (PNTAGKeyList)_nt_malloc(sizeof(NTAGKeyList))))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    memcpy(pNewKey, pOldKey, sizeof(NTAGKeyList));
    pNewKey->pKeyValue = NULL;
    pNewKey->cbDataLen = 0;
    pNewKey->pData = NULL;
    pNewKey->cbSaltLen = 0;

    pNewKey->cbKeyLen = pOldKey->cbKeyLen;
    if (pNewKey->cbKeyLen)
    {
        if (NULL == (pNewKey->pKeyValue = (BYTE*)_nt_malloc(pNewKey->cbKeyLen)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
    }

    memcpy(pNewKey->pKeyValue, pOldKey->pKeyValue, pNewKey->cbKeyLen);

    pNewKey->cbDataLen = pOldKey->cbDataLen;
    if (pNewKey->cbDataLen)
    {
        if (NULL == (pNewKey->pData = (BYTE*)_nt_malloc(pNewKey->cbDataLen)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
    }

    memcpy(pNewKey->pData, pOldKey->pData, pNewKey->cbDataLen);

    pNewKey->cbSaltLen = pOldKey->cbSaltLen;

    memcpy(pNewKey->rgbSalt, pOldKey->rgbSalt, pNewKey->cbSaltLen);

    *ppNewKey = pNewKey;

    fRet = TRUE;
Ret:
    if ((FALSE == fRet) && pNewKey)
        FreeNewKey(pNewKey);
    return fRet;
}

/*
 -  CPDuplicateKey
 -
 *  Purpose:
 *                Duplicates the state of a key and returns a handle to it
 *
 *  Parameters:
 *               IN      hUID           -  Handle to a CSP
 *               IN      hKey           -  Handle to a key
 *               IN      pdwReserved    -  Reserved
 *               IN      dwFlags        -  Flags
 *               IN      phKey          -  Handle to the new key
 *
 *  Returns:
 */
BOOL CPDuplicateKey(IN HCRYPTPROV hUID,
                    IN HCRYPTKEY hKey,
                    IN DWORD *pdwReserved,
                    IN DWORD dwFlags,
                    IN HCRYPTKEY *phKey)
{
    PNTAGKeyList    pTmpKey;
    PNTAGKeyList    pNewKey = NULL;
    BYTE            bType = KEY_HANDLE;
    BOOL            fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputDuplicateKey(TRUE, FALSE, hUID, hKey, pdwReserved,
                               dwFlags, phKey);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (NULL != pdwReserved)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    if (0 != dwFlags)
    {
        SetLastError((DWORD)NTE_BAD_FLAGS);
        goto Ret;
    }

    if (NULL == (pTmpKey = (PNTAGKeyList)NTLValidate((HNTAG)hKey, hUID,
                                                      bType)))
    {
        bType = SIGPUBKEY_HANDLE;
        if (NULL == (pTmpKey = (PNTAGKeyList)NTLValidate((HNTAG)hKey, hUID,
                                                         bType)))
        {
            bType = EXCHPUBKEY_HANDLE;
            if (NULL == (pTmpKey = (PNTAGKeyList)NTLValidate((HNTAG)hKey, hUID,
                                                             bType)))
            {
                SetLastError((DWORD)NTE_BAD_KEY);
                goto Ret;
            }
        }
    }

    if (!CopyKey(pTmpKey, &pNewKey))
        goto Ret;          // error already set


    if (NTF_FAILED == NTLMakeItem(phKey, bType, (void *)pNewKey))
        goto Ret;          // error already set

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputDuplicateKey(FALSE, fRet, 0, 0, NULL, 0, NULL);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if ((NTF_FAILED == fRet) && pNewKey)
        FreeNewKey(pNewKey);
    return fRet;
}

//
// Function : TestEncDec
//
// Description : This function expands the passed in key buffer for the appropriate
//               algorithm, and then either encryption or decryption is performed.
//               A comparison is then made to see if the ciphertext or plaintext
//               matches the expected value.
//               The function only uses ECB mode for block ciphers and the plaintext
//               buffer must be the same length as the ciphertext buffer.  The length
//               of the plaintext must be either the block length of the cipher if it
//               is a block cipher or less than MAX_BLOCKLEN if a stream cipher is
//               being used.
//
BOOL TestEncDec(
                IN ALG_ID Algid,
                IN BYTE *pbKey,
                IN DWORD cbKey,
                IN BYTE *pbPlaintext,
                IN DWORD cbPlaintext,
                IN BYTE *pbCiphertext,
                IN BYTE *pbIV,
                IN int iOperation
                )
{
    BYTE    *pbExpandedKey = NULL;
    BYTE    rgbBuffIn[MAX_BLOCKLEN];
    BYTE    rgbBuffOut[MAX_BLOCKLEN];
    DWORD   i;
    BOOL    fRet = FALSE;

    memset(rgbBuffIn, 0, sizeof(rgbBuffIn));
    memset(rgbBuffOut, 0, sizeof(rgbBuffOut));

    // length of data to encrypt must be < MAX_BLOCKLEN 
    if (cbPlaintext > MAX_BLOCKLEN)
    {
        goto Ret;
    }

    // alloc for and expand the key
    switch(Algid)
    {
#ifdef CSP_USE_RC4
        case (CALG_RC4):
        {
            if (NULL == (pbExpandedKey = _nt_malloc(sizeof(RC4_KEYSTRUCT))))
            {
                goto Ret;
            }
            rc4_key((RC4_KEYSTRUCT*)pbExpandedKey, cbKey, pbKey);
            break;
        }
#endif // CSP_USE_RC4

#ifdef CSP_USE_RC2
        case (CALG_RC2):
        {
            if (NULL == (pbExpandedKey = _nt_malloc(RC2_TABLESIZE)))
            {
                goto Ret;
            }
            RC2KeyEx((WORD*)pbExpandedKey, pbKey, cbKey, cbKey * 8);
            break;
        }
#endif // CSP_USE_RC2

#ifdef CSP_USE_DES40
        case (CALG_DES40):
        {
            if (NULL == (pbExpandedKey = _nt_malloc(DES_TABLESIZE)))
            {
                goto Ret;
            }
            pbKey[0] &= 0x0F;    // set 4 leftmost bits of first byte to zero
            pbKey[2] &= 0x0F;    // set 4 leftmost bits of third byte to zero
            pbKey[4] &= 0x0F;    // set 4 leftmost bits of fifth byte to zero
            pbKey[6] &= 0x0F;    // set 4 leftmost bits of seventh byte to zero
            desparityonkey(pbKey, cbKey);
            deskey((DESTable*)pbExpandedKey, pbKey);
            break;
        }
#endif // CSP_USE_DES40

#ifdef CSP_USE_DES
        case (CALG_DES):
        {
            if (NULL == (pbExpandedKey = _nt_malloc(DES_TABLESIZE)))
            {
                goto Ret;
            }
            desparityonkey(pbKey, cbKey);
            deskey((DESTable*)pbExpandedKey, pbKey);
            break;
        }
#endif // CSP_USE_DES

#ifdef CSP_USE_3DES
        desparityonkey(pbKey, cbKey);
        case (CALG_3DES):
        {
            if (NULL == (pbExpandedKey = _nt_malloc(DES3_TABLESIZE)))
            {
                goto Ret;
            }
            tripledes3key((PDES3TABLE)pbExpandedKey, pbKey);
            break;
        }

        case (CALG_3DES_112):
        {
            if (NULL == (pbExpandedKey = _nt_malloc(DES3_TABLESIZE)))
            {
                goto Ret;
            }
            tripledes2key((PDES3TABLE)pbExpandedKey, pbKey);
            break;
        }
#endif // CSP_USE_3DES
    }

    // if encrypting and there is an IV then use it
    if ((ENCRYPT == iOperation) && (CALG_RC4 != Algid))
    {
        memcpy(rgbBuffIn, pbPlaintext, cbPlaintext);

        if (NULL != pbIV)
        {
            for(i = 0; i < cbPlaintext; i++)
            {
                rgbBuffIn[i] = rgbBuffIn[i] ^ pbIV[i];
            }
        }
    }

    // encrypt the plaintext
    switch(Algid)
    {
#ifdef CSP_USE_RC4
        case (CALG_RC4):
        {
            if (ENCRYPT == iOperation)
            {            
                memcpy(rgbBuffOut, pbPlaintext, cbPlaintext);
            }
            else
            {
                memcpy(rgbBuffOut, pbCiphertext, cbPlaintext);
            }
            rc4((RC4_KEYSTRUCT*)pbExpandedKey, cbPlaintext, rgbBuffOut);
            break;
        }
#endif // CSP_USE_RC4

#ifdef CSP_USE_RC2
        case (CALG_RC2):
        {
            if (ENCRYPT == iOperation)
            {
                RC2(rgbBuffOut, rgbBuffIn, pbExpandedKey, ENCRYPT);
            }
            else
            {
                RC2(rgbBuffOut, pbCiphertext, pbExpandedKey, DECRYPT);
            }
            break;
        }
#endif // CSP_USE_RC2

#ifdef CSP_USE_DES40
        case (CALG_DES40):
        {
            if (ENCRYPT == iOperation)
            {
                des(rgbBuffOut, rgbBuffIn, pbExpandedKey, ENCRYPT);
            }
            else
            {
                des(rgbBuffOut, pbCiphertext, pbExpandedKey, DECRYPT);
            }
            break;
        }
#endif // CSP_USE_DES40

#ifdef CSP_USE_DES
        case (CALG_DES):
        {
            if (ENCRYPT == iOperation)
            {
                des(rgbBuffOut, rgbBuffIn, pbExpandedKey, ENCRYPT);
            }
            else
            {
                des(rgbBuffOut, pbCiphertext, pbExpandedKey, DECRYPT);
            }
            break;
        }
#endif // CSP_USE_DES

#ifdef CSP_USE_3DES
        case (CALG_3DES):
        case (CALG_3DES_112):
        {
            if (ENCRYPT == iOperation)
            {
                tripledes(rgbBuffOut, rgbBuffIn, pbExpandedKey, ENCRYPT);
            }
            else
            {
                tripledes(rgbBuffOut, pbCiphertext, pbExpandedKey, DECRYPT);
            }
            break;
        }
#endif // CSP_USE_3DES
    }

    // compare the encrypted plaintext with the passed in ciphertext
    if (ENCRYPT == iOperation)
    {
        if (memcmp(pbCiphertext, rgbBuffOut, cbPlaintext))
        {
            goto Ret;
        }
    }
    // compare the decrypted ciphertext with the passed in plaintext
    else
    {
        // if there is an IV then use it
        if (NULL != pbIV)
        {
            for(i = 0; i < cbPlaintext; i++)
            {
                rgbBuffOut[i] = rgbBuffOut[i] ^ pbIV[i];
            }
        }

        if (memcmp(pbPlaintext, rgbBuffOut, cbPlaintext))
        {
            goto Ret;
        }
    }

    fRet = TRUE;
Ret:
    if (pbExpandedKey)
    {
        _nt_free(pbExpandedKey, 0);
    }

    return fRet;
}

//
// Function : TestSymmetricAlgorithm
//
// Description : This function expands the passed in key buffer for the appropriate algorithm,
//               encrypts the plaintext buffer with the same algorithm and key, and the
//               compares the passed in expected ciphertext with the calculated ciphertext
//               to make sure they are the same.  The opposite is then done with decryption.
//               The function only uses ECB mode for block ciphers and the plaintext
//               buffer must be the same length as the ciphertext buffer.  The length
//               of the plaintext must be either the block length of the cipher if it
//               is a block cipher or less than MAX_BLOCKLEN if a stream cipher is
//               being used.
//
BOOL TestSymmetricAlgorithm(
                            IN ALG_ID Algid,
                            IN BYTE *pbKey,
                            IN DWORD cbKey,
                            IN BYTE *pbPlaintext,
                            IN DWORD cbPlaintext,
                            IN BYTE *pbCiphertext,
                            IN BYTE *pbIV
                            )
{
    BOOL    fRet = FALSE;

    if (!TestEncDec(Algid, pbKey, cbKey, pbPlaintext, cbPlaintext,
                    pbCiphertext, pbIV, ENCRYPT))
    {
        goto Ret;
    }
    if (!TestEncDec(Algid, pbKey, cbKey, pbPlaintext, cbPlaintext,
                    pbCiphertext, pbIV, DECRYPT))
    {
        goto Ret;
    }

    fRet = TRUE;
Ret:
    return fRet;
}
