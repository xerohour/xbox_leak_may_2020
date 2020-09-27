//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1995 - 1996
//
//  File:       oidinfo.cpp
//
//  Contents:   Cryptographic Object ID (OID) Info Functions
//
//  Functions:  I_CryptOIDInfoDllMain
//              CryptFindOIDInfo
//              CryptRegisterOIDInfo
//              CryptUnregisterOIDInfo
//              CryptEnumOIDInfo
//              CryptFindLocalizedName
//
//  Comments:
//
//  History:    24-May-97    philh   created
//--------------------------------------------------------------------------

#include "global.hxx"
#include <dbgdef.h>
#include "wintrust.h"  //wintrust.h is needed for SPC_ oids

// Initialized in I_CryptOIDInfoDllMain at ProcessAttach
static HMODULE hOIDInfoInst;

#define MAX_RESOURCE_OID_NAME_LENGTH    256
static LPCWSTR pwszNullName = L"";

#define LEN_ALIGN(Len)  ((Len + 7) & ~7)

#define CONST_OID_GROUP_PREFIX_CHAR    '!'
#define OID_INFO_ENCODING_TYPE          0
#define OID_INFO_NAME_VALUE_NAME        L"Name"
#define OID_INFO_ALGID_VALUE_NAME       L"Algid"
#define OID_INFO_EXTRA_INFO_VALUE_NAME  L"ExtraInfo"
#define OID_INFO_FLAGS_VALUE_NAME       L"Flags"



//+=========================================================================
//  OID Information Tables (by GROUP_ID)
//==========================================================================

#define OID_INFO_LEN sizeof(CRYPT_OID_INFO)

//+-------------------------------------------------------------------------
//  Hash Algorithm Table
//--------------------------------------------------------------------------
#define HASH_ALG_ENTRY(pszOID, pwszName, Algid) \
    OID_INFO_LEN, pszOID, pwszName, CRYPT_HASH_ALG_OID_GROUP_ID, Algid, 0, NULL

static CCRYPT_OID_INFO HashAlgTable[] = {
    HASH_ALG_ENTRY(szOID_OIWSEC_sha1, L"sha1", CALG_SHA1),
    HASH_ALG_ENTRY(szOID_OIWSEC_sha1, L"sha", CALG_SHA1),
    HASH_ALG_ENTRY(szOID_OIWSEC_sha, L"sha", CALG_SHA),
    HASH_ALG_ENTRY(szOID_RSA_MD5, L"md5", CALG_MD5),
    HASH_ALG_ENTRY(szOID_RSA_MD4, L"md4", CALG_MD4),
    HASH_ALG_ENTRY(szOID_RSA_MD2, L"md2", CALG_MD2)
};
#define HASH_ALG_CNT (sizeof(HashAlgTable) / sizeof(HashAlgTable[0]))


//+-------------------------------------------------------------------------
//  Encryption Algorithm Table
//--------------------------------------------------------------------------
#define ENCRYPT_ALG_ENTRY(pszOID, pwszName, Algid) \
    OID_INFO_LEN, pszOID, pwszName, CRYPT_ENCRYPT_ALG_OID_GROUP_ID, \
    Algid, 0, NULL

static CCRYPT_OID_INFO EncryptAlgTable[] = {
    ENCRYPT_ALG_ENTRY(szOID_OIWSEC_desCBC, L"des", CALG_DES),
    ENCRYPT_ALG_ENTRY(szOID_RSA_DES_EDE3_CBC, L"3des", CALG_3DES),
    ENCRYPT_ALG_ENTRY(szOID_RSA_RC2CBC, L"rc2", CALG_RC2),
    ENCRYPT_ALG_ENTRY(szOID_RSA_RC4, L"rc4", CALG_RC4),
#ifdef CMS_PKCS7
    ENCRYPT_ALG_ENTRY(szOID_RSA_SMIMEalgESDHwith3DES, L"ESDHwith3DES", CALG_3DES),
    ENCRYPT_ALG_ENTRY(szOID_RSA_SMIMEalgESDHwithRC2, L"ESDHwithRC2", CALG_RC2),
    ENCRYPT_ALG_ENTRY(szOID_RSA_SMIMEalg3DESwrap, L"3DESwrap", CALG_3DES),
    ENCRYPT_ALG_ENTRY(szOID_RSA_SMIMEalgRC2wrap, L"RC2wrap", CALG_RC2),
#endif  // CMS_PKCS7
};
#define ENCRYPT_ALG_CNT (sizeof(EncryptAlgTable) / sizeof(EncryptAlgTable[0]))


//+-------------------------------------------------------------------------
//  Public Key Algorithm Table
//--------------------------------------------------------------------------
static const DWORD dwMosaicFlags = CRYPT_OID_INHIBIT_SIGNATURE_FORMAT_FLAG |
                                        CRYPT_OID_NO_NULL_ALGORITHM_PARA_FLAG;

static const DWORD dwNoNullParaFlag = CRYPT_OID_NO_NULL_ALGORITHM_PARA_FLAG;

#define PUBKEY_ALG_ENTRY(pszOID, pwszName, Algid) \
    OID_INFO_LEN, pszOID, pwszName, CRYPT_PUBKEY_ALG_OID_GROUP_ID, \
    Algid, 0, NULL

#define PUBKEY_EXTRA_ALG_ENTRY(pszOID, pwszName, Algid, dwFlags) \
    OID_INFO_LEN, pszOID, pwszName, CRYPT_PUBKEY_ALG_OID_GROUP_ID, \
    Algid, sizeof(dwFlags), (BYTE *) &dwFlags

#define DSA_PUBKEY_ALG_ENTRY(pszOID, pwszName) \
    PUBKEY_EXTRA_ALG_ENTRY(pszOID, pwszName, CALG_DSS_SIGN, dwNoNullParaFlag)

#define DH_PUBKEY_ALG_ENTRY(pszOID, pwszName) \
    PUBKEY_EXTRA_ALG_ENTRY(pszOID, pwszName, CALG_DH_SF, dwNoNullParaFlag)

#ifdef CMS_PKCS7
#define ESDH_PUBKEY_ALG_ENTRY(pszOID, pwszName) \
    PUBKEY_EXTRA_ALG_ENTRY(pszOID, pwszName, CALG_DH_EPHEM, dwNoNullParaFlag)
#endif  // CMS_PKCS7

static CCRYPT_OID_INFO PubKeyAlgTable[] = {
    PUBKEY_ALG_ENTRY(szOID_RSA_RSA, L"RSA", CALG_RSA_KEYX),
    DSA_PUBKEY_ALG_ENTRY(szOID_X957_DSA, L"DSA"),
    DH_PUBKEY_ALG_ENTRY(szOID_ANSI_X942_DH, L"DH"),
    PUBKEY_ALG_ENTRY(szOID_RSA_RSA, L"RSA_KEYX", CALG_RSA_KEYX),
    PUBKEY_ALG_ENTRY(szOID_RSA_RSA, L"RSA", CALG_RSA_SIGN),
    PUBKEY_ALG_ENTRY(szOID_RSA_RSA, L"RSA_SIGN", CALG_RSA_SIGN),
    DSA_PUBKEY_ALG_ENTRY(szOID_OIWSEC_dsa, L"DSA"),
    DSA_PUBKEY_ALG_ENTRY(szOID_OIWSEC_dsa, L"DSS"),
    DSA_PUBKEY_ALG_ENTRY(szOID_OIWSEC_dsa, L"DSA_SIGN"),
    DH_PUBKEY_ALG_ENTRY(szOID_RSA_DH, L"DH"),
    PUBKEY_ALG_ENTRY(szOID_OIWSEC_rsaXchg, L"RSA_KEYX", CALG_RSA_KEYX),
    PUBKEY_EXTRA_ALG_ENTRY(szOID_INFOSEC_mosaicKMandUpdSig,
        L"mosaicKMandUpdSig", CALG_DSS_SIGN, dwMosaicFlags),
#ifdef CMS_PKCS7
    ESDH_PUBKEY_ALG_ENTRY(szOID_RSA_SMIMEalgESDHwith3DES, L"ESDHwith3DES"),
    ESDH_PUBKEY_ALG_ENTRY(szOID_RSA_SMIMEalgESDHwithRC2, L"ESDHwithRC2"),
#endif  // CMS_PKCS7
};
#define PUBKEY_ALG_CNT (sizeof(PubKeyAlgTable) / sizeof(PubKeyAlgTable[0]))


//+-------------------------------------------------------------------------
//  Signature Algorithm Table
//--------------------------------------------------------------------------
static const ALG_ID aiRsaPubKey = CALG_RSA_SIGN;
static const DWORD rgdwMosaicSign[] = {
    CALG_DSS_SIGN,
    CRYPT_OID_INHIBIT_SIGNATURE_FORMAT_FLAG |
        CRYPT_OID_NO_NULL_ALGORITHM_PARA_FLAG
};
static const DWORD rgdwDssSign[] = {
    CALG_DSS_SIGN,
    CRYPT_OID_NO_NULL_ALGORITHM_PARA_FLAG
};

#define SIGN_ALG_ENTRY(pszOID, pwszName, aiHash, aiPubKey) \
    OID_INFO_LEN, pszOID, pwszName, CRYPT_SIGN_ALG_OID_GROUP_ID, aiHash, \
    sizeof(aiPubKey), (BYTE *) &aiPubKey
#define RSA_SIGN_ALG_ENTRY(pszOID, pwszName, aiHash) \
    SIGN_ALG_ENTRY(pszOID, pwszName, aiHash, aiRsaPubKey)

#define SIGN_EXTRA_ALG_ENTRY(pszOID, pwszName, aiHash, rgdwExtra) \
    OID_INFO_LEN, pszOID, pwszName, CRYPT_SIGN_ALG_OID_GROUP_ID, aiHash, \
    sizeof(rgdwExtra), (BYTE *) rgdwExtra

#define DSS_SIGN_ALG_ENTRY(pszOID, pwszName) \
    SIGN_EXTRA_ALG_ENTRY(pszOID, pwszName, CALG_SHA1, rgdwDssSign)

static CCRYPT_OID_INFO SignAlgTable[] = {
    RSA_SIGN_ALG_ENTRY(szOID_RSA_SHA1RSA, L"sha1RSA", CALG_SHA1),
    RSA_SIGN_ALG_ENTRY(szOID_RSA_MD5RSA, L"md5RSA", CALG_MD5),
    DSS_SIGN_ALG_ENTRY(szOID_X957_SHA1DSA, L"sha1DSA"),
    RSA_SIGN_ALG_ENTRY(szOID_OIWSEC_sha1RSASign, L"sha1RSA", CALG_SHA1),
    RSA_SIGN_ALG_ENTRY(szOID_OIWSEC_sha1RSASign, L"shaRSA", CALG_SHA1),
    RSA_SIGN_ALG_ENTRY(szOID_OIWSEC_shaRSA, L"shaRSA", CALG_SHA),
    RSA_SIGN_ALG_ENTRY(szOID_OIWSEC_md5RSA, L"md5RSA", CALG_MD5),
    RSA_SIGN_ALG_ENTRY(szOID_RSA_MD2RSA, L"md2RSA", CALG_MD2),
    RSA_SIGN_ALG_ENTRY(szOID_RSA_MD4RSA, L"md4RSA", CALG_MD4),
    RSA_SIGN_ALG_ENTRY(szOID_OIWSEC_md4RSA, L"md4RSA", CALG_MD4),
    RSA_SIGN_ALG_ENTRY(szOID_OIWSEC_md4RSA2, L"md4RSA", CALG_MD4),
    RSA_SIGN_ALG_ENTRY(szOID_OIWDIR_md2RSA, L"md2RSA", CALG_MD2),
    DSS_SIGN_ALG_ENTRY(szOID_OIWSEC_shaDSA, L"sha1DSA"),
    DSS_SIGN_ALG_ENTRY(szOID_OIWSEC_shaDSA, L"shaDSA"),
    DSS_SIGN_ALG_ENTRY(szOID_OIWSEC_dsaSHA1,L"dsaSHA1"),
    SIGN_EXTRA_ALG_ENTRY(szOID_INFOSEC_mosaicUpdatedSig, L"mosaicUpdatedSig",
        CALG_SHA, rgdwMosaicSign),
};
#define SIGN_ALG_CNT (sizeof(SignAlgTable) / sizeof(SignAlgTable[0]))


//+-------------------------------------------------------------------------
//  RDN Attribute Table
//--------------------------------------------------------------------------

// PLEASE UPDATE the following define in certstr.cpp if you add a new entry
// with a longer pwszName
// #define MAX_X500_KEY_LEN    64

// Ordered lists of acceptable RDN attribute value types. 0 terminates.
static const DWORD rgdwPrintableValueType[] = { CERT_RDN_PRINTABLE_STRING, 0 };
static const DWORD rgdwIA5ValueType[] = { CERT_RDN_IA5_STRING, 0 };
static const DWORD rgdwNumericValueType[] = { CERT_RDN_NUMERIC_STRING, 0 };
static const DWORD rgdwIA5orUTF8ValueType[] = { CERT_RDN_IA5_STRING,
                                                CERT_RDN_UTF8_STRING, 0 };

#define RDN_ATTR_ENTRY(pszOID, pwszName, rgdwValueType) \
    OID_INFO_LEN, pszOID, pwszName, CRYPT_RDN_ATTR_OID_GROUP_ID, 0, \
    sizeof(rgdwValueType), (BYTE *) rgdwValueType
#define DEFAULT_RDN_ATTR_ENTRY(pszOID, pwszName) \
    OID_INFO_LEN, pszOID, pwszName, CRYPT_RDN_ATTR_OID_GROUP_ID, 0, 0, NULL

static CCRYPT_OID_INFO RDNAttrTable[] = {
    // Ordered with most commonly used key names at the beginning

    // Labeling attribute types:
    DEFAULT_RDN_ATTR_ENTRY(szOID_COMMON_NAME, L"CN"),
    // Geographic attribute types:
    DEFAULT_RDN_ATTR_ENTRY(szOID_LOCALITY_NAME, L"L"),
    // Organizational attribute types:
    DEFAULT_RDN_ATTR_ENTRY(szOID_ORGANIZATION_NAME, L"O"),
    DEFAULT_RDN_ATTR_ENTRY(szOID_ORGANIZATIONAL_UNIT_NAME, L"OU"),

    // Verisign sticks the following in their cert names. Netscape uses the
    // "E" instead of the "Email". Will let "E" take precedence
    RDN_ATTR_ENTRY(szOID_RSA_emailAddr, L"E", rgdwIA5ValueType),
    RDN_ATTR_ENTRY(szOID_RSA_emailAddr, L"Email", rgdwIA5ValueType),

    // The following aren't used in Verisign's certs

    // Geographic attribute types:
    RDN_ATTR_ENTRY(szOID_COUNTRY_NAME, L"C", rgdwPrintableValueType),
    DEFAULT_RDN_ATTR_ENTRY(szOID_STATE_OR_PROVINCE_NAME, L"S"),
    DEFAULT_RDN_ATTR_ENTRY(szOID_STATE_OR_PROVINCE_NAME, L"ST"),
    DEFAULT_RDN_ATTR_ENTRY(szOID_STREET_ADDRESS, L"STREET"),

    // Organizational attribute types:
    DEFAULT_RDN_ATTR_ENTRY(szOID_TITLE, L"T"),
    DEFAULT_RDN_ATTR_ENTRY(szOID_TITLE, L"Title"),

    DEFAULT_RDN_ATTR_ENTRY(szOID_GIVEN_NAME, L"G"),
    DEFAULT_RDN_ATTR_ENTRY(szOID_GIVEN_NAME, L"GN"),
    DEFAULT_RDN_ATTR_ENTRY(szOID_GIVEN_NAME, L"GivenName"),
    DEFAULT_RDN_ATTR_ENTRY(szOID_INITIALS, L"I"),
    DEFAULT_RDN_ATTR_ENTRY(szOID_INITIALS, L"Initials"),

    // Labeling attribute types:
    DEFAULT_RDN_ATTR_ENTRY(szOID_SUR_NAME, L"SN"),
//    RDN_ATTR_ENTRY(szOID_DEVICE_SERIAL_NUMBER, L"", rgdwPrintableValueType),

    // Pilot user attribute types:
    RDN_ATTR_ENTRY(szOID_DOMAIN_COMPONENT, L"DC", rgdwIA5orUTF8ValueType),

    // Explanatory attribute types:
    DEFAULT_RDN_ATTR_ENTRY(szOID_DESCRIPTION, L"Description"),
//  szOID_SEARCH_GUIDE                  "2.5.4.14"
//    DEFAULT_RDN_ATTR_ENTRY(szOID_BUSINESS_CATEGORY, L""),

    // Postal addressing attribute types:
//  szOID_POSTAL_ADDRESS                "2.5.4.16"
    DEFAULT_RDN_ATTR_ENTRY(szOID_POSTAL_CODE, L"PostalCode"),
    DEFAULT_RDN_ATTR_ENTRY(szOID_POST_OFFICE_BOX, L"POBox"),
//    DEFAULT_RDN_ATTR_ENTRY(szOID_PHYSICAL_DELIVERY_OFFICE_NAME, L""),

    // Telecommunications addressing attribute types:
    RDN_ATTR_ENTRY(szOID_TELEPHONE_NUMBER, L"Phone", rgdwPrintableValueType),
//  szOID_TELEX_NUMBER                  "2.5.4.21"
//  szOID_TELETEXT_TERMINAL_IDENTIFIER  "2.5.4.22"
//  szOID_FACSIMILE_TELEPHONE_NUMBER    "2.5.4.23"

//  Following is used as a test case for a Numeric value
    RDN_ATTR_ENTRY(szOID_X21_ADDRESS, L"X21Address", rgdwNumericValueType),
//    RDN_ATTR_ENTRY(szOID_INTERNATIONAL_ISDN_NUMBER, L"", rgdwNumericValueType),
//  szOID_REGISTERED_ADDRESS            "2.5.4.26"
//    RDN_ATTR_ENTRY(szOID_DESTINATION_INDICATOR, L"", rgdwPrintableValueType)

    // Preference attribute types:
//  szOID_PREFERRED_DELIVERY_METHOD     "2.5.4.28"

    // OSI application attribute types:
//  szOID_PRESENTATION_ADDRESS          "2.5.4.29"
//  szOID_SUPPORTED_APPLICATION_CONTEXT "2.5.4.30"

    // Relational application attribute types:
//  szOID_MEMBER                        "2.5.4.31"
//  szOID_OWNER                         "2.5.4.32"
//  szOID_ROLE_OCCUPANT                 "2.5.4.33"
//  szOID_SEE_ALSO                      "2.5.4.34"

    // Security attribute types:
//  szOID_USER_PASSWORD                 "2.5.4.35"
//  szOID_USER_CERTIFICATE              "2.5.4.36"
//  szOID_CA_CERTIFICATE                "2.5.4.37"
//  szOID_AUTHORITY_REVOCATION_LIST     "2.5.4.38"
//  szOID_CERTIFICATE_REVOCATION_LIST   "2.5.4.39"
//  szOID_CROSS_CERTIFICATE_PAIR        "2.5.4.40"

    // Undocumented attribute types???
//#define szOID_???                         "2.5.4.41"

    DEFAULT_RDN_ATTR_ENTRY(szOID_DN_QUALIFIER, L"dnQualifier"),
};
#define RDN_ATTR_CNT (sizeof(RDNAttrTable) / sizeof(RDNAttrTable[0]))

//+-------------------------------------------------------------------------
//  Extension or Attribute Table (Localized via resource strings)
//--------------------------------------------------------------------------
#define EXT_ATTR_ENTRY(pszOID, ResourceIdORpwszName) \
    OID_INFO_LEN, pszOID, (LPCWSTR) ResourceIdORpwszName, \
        CRYPT_EXT_OR_ATTR_OID_GROUP_ID, 0, 0, NULL

static CRYPT_OID_INFO ExtAttrTable[] = {
    EXT_ATTR_ENTRY(szOID_AUTHORITY_KEY_IDENTIFIER2,
        IDS_EXT_AUTHORITY_KEY_IDENTIFIER),
    EXT_ATTR_ENTRY(szOID_AUTHORITY_KEY_IDENTIFIER,
        IDS_EXT_AUTHORITY_KEY_IDENTIFIER),
    EXT_ATTR_ENTRY(szOID_KEY_ATTRIBUTES,
        IDS_EXT_KEY_ATTRIBUTES),
    EXT_ATTR_ENTRY(szOID_KEY_USAGE_RESTRICTION,
        IDS_EXT_KEY_USAGE_RESTRICTION),
    EXT_ATTR_ENTRY(szOID_SUBJECT_ALT_NAME2,
        IDS_EXT_SUBJECT_ALT_NAME),
    EXT_ATTR_ENTRY(szOID_SUBJECT_ALT_NAME,
        IDS_EXT_SUBJECT_ALT_NAME),
    EXT_ATTR_ENTRY(szOID_ISSUER_ALT_NAME2,
        IDS_EXT_ISSUER_ALT_NAME),
    EXT_ATTR_ENTRY(szOID_ISSUER_ALT_NAME,
        IDS_EXT_ISSUER_ALT_NAME),
    EXT_ATTR_ENTRY(szOID_BASIC_CONSTRAINTS2,
        IDS_EXT_BASIC_CONSTRAINTS),
    EXT_ATTR_ENTRY(szOID_BASIC_CONSTRAINTS,
        IDS_EXT_BASIC_CONSTRAINTS),
    EXT_ATTR_ENTRY(szOID_KEY_USAGE,
        IDS_EXT_KEY_USAGE),
    EXT_ATTR_ENTRY(szOID_CERT_POLICIES,
        IDS_EXT_CERT_POLICIES),
    EXT_ATTR_ENTRY(szOID_SUBJECT_KEY_IDENTIFIER,
        IDS_EXT_SUBJECT_KEY_IDENTIFIER),
    EXT_ATTR_ENTRY(szOID_CRL_REASON_CODE,
        IDS_EXT_CRL_REASON_CODE),
    EXT_ATTR_ENTRY(szOID_CRL_DIST_POINTS,
        IDS_EXT_CRL_DIST_POINTS),
    EXT_ATTR_ENTRY(szOID_ENHANCED_KEY_USAGE,
        IDS_EXT_ENHANCED_KEY_USAGE),
    EXT_ATTR_ENTRY(szOID_AUTHORITY_INFO_ACCESS,
        IDS_EXT_AUTHORITY_INFO_ACCESS),
    EXT_ATTR_ENTRY(szOID_CERT_EXTENSIONS,
        IDS_EXT_CERT_EXTENSIONS),
    EXT_ATTR_ENTRY(szOID_NEXT_UPDATE_LOCATION,
        IDS_EXT_NEXT_UPDATE_LOCATION),
    EXT_ATTR_ENTRY(szOID_YESNO_TRUST_ATTR,
        IDS_EXT_YESNO_TRUST_ATTR),
    EXT_ATTR_ENTRY(szOID_RSA_emailAddr,
        IDS_EXT_RSA_emailAddr),
    EXT_ATTR_ENTRY(szOID_RSA_unstructName,
        IDS_EXT_RSA_unstructName),
    EXT_ATTR_ENTRY(szOID_RSA_contentType,
        IDS_EXT_RSA_contentType),
    EXT_ATTR_ENTRY(szOID_RSA_messageDigest,
        IDS_EXT_RSA_messageDigest),
    EXT_ATTR_ENTRY(szOID_RSA_signingTime,
        IDS_EXT_RSA_signingTime),
    EXT_ATTR_ENTRY(szOID_RSA_counterSign,
        IDS_EXT_RSA_counterSign),
    EXT_ATTR_ENTRY(szOID_RSA_challengePwd,
        IDS_EXT_RSA_challengePwd),
    EXT_ATTR_ENTRY(szOID_RSA_unstructAddr,
        IDS_EXT_RSA_unstructAddr),
    EXT_ATTR_ENTRY(szOID_RSA_extCertAttrs, L""),
    EXT_ATTR_ENTRY(szOID_RSA_SMIMECapabilities,
        IDS_EXT_RSA_SMIMECapabilities),
    EXT_ATTR_ENTRY(szOID_RSA_preferSignedData,
        IDS_EXT_RSA_preferSignedData),
    EXT_ATTR_ENTRY(szOID_PKIX_POLICY_QUALIFIER_CPS,
        IDS_EXT_PKIX_POLICY_QUALIFIER_CPS),
    EXT_ATTR_ENTRY(szOID_PKIX_POLICY_QUALIFIER_USERNOTICE,
        IDS_EXT_PKIX_POLICY_QUALIFIER_USERNOTICE),
    EXT_ATTR_ENTRY(szOID_PKIX_OCSP,
        IDS_EXT_PKIX_OCSP),
    EXT_ATTR_ENTRY(szOID_PKIX_CA_ISSUERS,
        IDS_EXT_PKIX_CA_ISSUERS),
    EXT_ATTR_ENTRY("1.3.6.1.4.1.311.20.2",
        IDS_EXT_MS_CERTIFICATE_TEMPLATE),
    EXT_ATTR_ENTRY(szOID_ENROLLMENT_AGENT,
        IDS_EXT_ENROLLMENT_AGENT),
    EXT_ATTR_ENTRY(szOID_ENROLL_CERTTYPE_EXTENSION,
        IDS_EXT_ENROLL_CERTTYPE),
    EXT_ATTR_ENTRY(szOID_CERT_MANIFOLD,
        IDS_EXT_CERT_MANIFOLD),
    EXT_ATTR_ENTRY(szOID_NETSCAPE_CERT_TYPE,
        IDS_EXT_NETSCAPE_CERT_TYPE),
    EXT_ATTR_ENTRY(szOID_NETSCAPE_BASE_URL,
        IDS_EXT_NETSCAPE_BASE_URL),
    EXT_ATTR_ENTRY(szOID_NETSCAPE_REVOCATION_URL,
        IDS_EXT_NETSCAPE_REVOCATION_URL),
    EXT_ATTR_ENTRY(szOID_NETSCAPE_CA_REVOCATION_URL,
        IDS_EXT_NETSCAPE_CA_REVOCATION_URL),
    EXT_ATTR_ENTRY(szOID_NETSCAPE_CERT_RENEWAL_URL,
        IDS_EXT_NETSCAPE_CERT_RENEWAL_URL),
    EXT_ATTR_ENTRY(szOID_NETSCAPE_CA_POLICY_URL,
        IDS_EXT_NETSCAPE_CA_POLICY_URL),
    EXT_ATTR_ENTRY(szOID_NETSCAPE_SSL_SERVER_NAME,
        IDS_EXT_NETSCAPE_SSL_SERVER_NAME),
    EXT_ATTR_ENTRY(szOID_NETSCAPE_COMMENT,
        IDS_EXT_NETSCAPE_COMMENT),
    EXT_ATTR_ENTRY(SPC_SP_AGENCY_INFO_OBJID,
        IDS_EXT_SPC_SP_AGENCY_INFO_OBJID),
    EXT_ATTR_ENTRY(SPC_FINANCIAL_CRITERIA_OBJID,
        IDS_EXT_SPC_FINANCIAL_CRITERIA_OBJID),
    EXT_ATTR_ENTRY(SPC_MINIMAL_CRITERIA_OBJID,
        IDS_EXT_SPC_MINIMAL_CRITERIA_OBJID),
    EXT_ATTR_ENTRY(szOID_COUNTRY_NAME,
        IDS_EXT_COUNTRY_NAME),
    EXT_ATTR_ENTRY(szOID_ORGANIZATION_NAME,
        IDS_EXT_ORGANIZATION_NAME),
    EXT_ATTR_ENTRY(szOID_ORGANIZATIONAL_UNIT_NAME,
        IDS_EXT_ORGANIZATIONAL_UNIT_NAME),
    EXT_ATTR_ENTRY(szOID_COMMON_NAME,
        IDS_EXT_COMMON_NAME),
    EXT_ATTR_ENTRY(szOID_LOCALITY_NAME,
        IDS_EXT_LOCALITY_NAME),
    EXT_ATTR_ENTRY(szOID_STATE_OR_PROVINCE_NAME,
        IDS_EXT_STATE_OR_PROVINCE_NAME),
    EXT_ATTR_ENTRY(szOID_TITLE,
        IDS_EXT_TITLE),
    EXT_ATTR_ENTRY(szOID_GIVEN_NAME,
        IDS_EXT_GIVEN_NAME),
    EXT_ATTR_ENTRY(szOID_INITIALS,
        IDS_EXT_INITIALS),
    EXT_ATTR_ENTRY(szOID_SUR_NAME,
        IDS_EXT_SUR_NAME),
    EXT_ATTR_ENTRY(szOID_DOMAIN_COMPONENT,
        IDS_EXT_DOMAIN_COMPONENT),
    EXT_ATTR_ENTRY(szOID_STREET_ADDRESS,
        IDS_EXT_STREET_ADDRESS),
    EXT_ATTR_ENTRY(szOID_DEVICE_SERIAL_NUMBER,
        IDS_EXT_DEVICE_SERIAL_NUMBER),
};


#define EXT_ATTR_CNT (sizeof(ExtAttrTable) / sizeof(ExtAttrTable[0]))

//+-------------------------------------------------------------------------
//  Enhanced Key Usage Table (Localized via resource strings)
//--------------------------------------------------------------------------
#define ENHKEY_ENTRY(pszOID, ResourceIdORpwszName) \
    OID_INFO_LEN, pszOID, (LPCWSTR) ResourceIdORpwszName, \
        CRYPT_ENHKEY_USAGE_OID_GROUP_ID, 0, 0, NULL

static CRYPT_OID_INFO EnhKeyTable[] = {
    ENHKEY_ENTRY(szOID_PKIX_KP_SERVER_AUTH,
        IDS_ENHKEY_PKIX_KP_SERVER_AUTH),
    ENHKEY_ENTRY(szOID_PKIX_KP_CLIENT_AUTH,
        IDS_ENHKEY_PKIX_KP_CLIENT_AUTH),
    ENHKEY_ENTRY(szOID_PKIX_KP_CODE_SIGNING,
        IDS_ENHKEY_PKIX_KP_CODE_SIGNING),
    ENHKEY_ENTRY(szOID_PKIX_KP_EMAIL_PROTECTION,
        IDS_ENHKEY_PKIX_KP_EMAIL_PROTECTION),
    ENHKEY_ENTRY(szOID_PKIX_KP_TIMESTAMP_SIGNING,
        IDS_ENHKEY_PKIX_KP_TIMESTAMP_SIGNING),
    ENHKEY_ENTRY(szOID_KP_CTL_USAGE_SIGNING,
        IDS_ENHKEY_KP_CTL_USAGE_SIGNING),
    ENHKEY_ENTRY(szOID_KP_TIME_STAMP_SIGNING,
        IDS_ENHKEY_KP_TIME_STAMP_SIGNING),
    ENHKEY_ENTRY(szOID_PKIX_KP_IPSEC_END_SYSTEM,
        IDS_ENHKEY_PKIX_KP_IPSEC_END_SYSTEM),
    ENHKEY_ENTRY(szOID_PKIX_KP_IPSEC_TUNNEL,
        IDS_ENHKEY_PKIX_KP_IPSEC_TUNNEL),
    ENHKEY_ENTRY(szOID_PKIX_KP_IPSEC_USER,
        IDS_ENHKEY_PKIX_KP_IPSEC_USER),
   // ENHKEY_ENTRY(szOID_SERVER_GATED_CRYPTO,
   //     IDS_ENHKEY_SERVER_GATED_CRYPTO),
   // ENHKEY_ENTRY(szOID_SGC_NETSCAPE,
   //     IDS_ENHKEY_SGC_NETSCAPE),
    ENHKEY_ENTRY(szOID_KP_EFS,
        IDS_ENHKEY_KP_EFS),
    ENHKEY_ENTRY(szOID_WHQL_CRYPTO,
        IDS_ENHKEY_KP_WHQL),
    ENHKEY_ENTRY(szOID_NT5_CRYPTO,
        IDS_ENHKEY_KP_NT5),
    ENHKEY_ENTRY(szOID_OEM_WHQL_CRYPTO,
        IDS_ENHKEY_KP_OEM_WHQL),
    ENHKEY_ENTRY(szOID_EMBEDDED_NT_CRYPTO,
        IDS_ENHKEY_KP_EMBEDDED_NT),
};
#define ENHKEY_CNT (sizeof(EnhKeyTable) / sizeof(EnhKeyTable[0]))

#if 0

//+-------------------------------------------------------------------------
//  Policy Table
//--------------------------------------------------------------------------
#define POLICY_ENTRY(pszOID, pwszName) \
    OID_INFO_LEN, pszOID, pwszName, CRYPT_POLICY_OID_GROUP_ID, 0, 0, NULL

static CCRYPT_OID_INFO PolicyTable[] = {
    POLICY_ENTRY(szOID_, L"")
};
#define POLICY_CNT (sizeof(PolicyTable) / sizeof(PolicyTable[0]))

#endif


//+=========================================================================
//  OID Group Tables
//
//  fLocalize is set to TRUE, if the CRYPT_OID_INFO's pwszName may be
//  a Resource ID that is used to get the localized name via LoadStringU().
//
//  Assumption, Resource ID's <= 0xFFFF.
//==========================================================================
typedef struct _GROUP_ENTRY {
    DWORD               cInfo;
    PCCRYPT_OID_INFO    rgInfo;
    BOOL                fLocalize;
} GROUP_ENTRY, *PGROUP_ENTRY;
typedef const GROUP_ENTRY CGROUP_ENTRY, *PCGROUP_ENTRY;

static CGROUP_ENTRY GroupTable[CRYPT_LAST_OID_GROUP_ID + 1] = {
    0, NULL, FALSE,                             // 0
    HASH_ALG_CNT, HashAlgTable, FALSE,          // 1
    ENCRYPT_ALG_CNT, EncryptAlgTable, FALSE,    // 2
    PUBKEY_ALG_CNT, PubKeyAlgTable, FALSE,      // 3
    SIGN_ALG_CNT, SignAlgTable, FALSE,          // 4
    RDN_ATTR_CNT, RDNAttrTable, FALSE,          // 5
    EXT_ATTR_CNT, ExtAttrTable, TRUE,           // 6
    ENHKEY_CNT, EnhKeyTable, TRUE,              // 7
#if 0
    POLICY_CNT, PolicyTable, FALSE              // 8
#else
    0, NULL, FALSE                              // 8
#endif
};


//+-------------------------------------------------------------------------
//  The following groups are dynamically updated from the registry on
//  CryptFindOIDInfo's first call.
//--------------------------------------------------------------------------
static GROUP_ENTRY RegBeforeGroup;
static GROUP_ENTRY RegAfterGroup;

// Do the load once within a critical section
static BOOL fLoadedFromRegAndResources = FALSE;
static CRITICAL_SECTION LoadFromRegCriticalSection;


//+=========================================================================
//  Localized Name Definitions and Data Structures
//==========================================================================

//+-------------------------------------------------------------------------
//  Localized Name Information
//--------------------------------------------------------------------------
typedef struct _LOCALIZED_NAME_INFO {
    LPCWSTR         pwszCryptName;
    union {
        UINT            uIDLocalizedName;
        LPCWSTR         pwszLocalizedName;
    };
} LOCALIZED_NAME_INFO, *PLOCALIZED_NAME_INFO;


//+-------------------------------------------------------------------------
//  Predefined Localized Names Table (Localized via resource strings)
//--------------------------------------------------------------------------
static LOCALIZED_NAME_INFO PredefinedNameTable[] = {
    // System store names
    L"Root",        IDS_SYS_NAME_ROOT,
    L"My",          IDS_SYS_NAME_MY,
    L"Trust",       IDS_SYS_NAME_TRUST,
    L"CA",          IDS_SYS_NAME_CA,
    L"UserDS",      IDS_SYS_NAME_USERDS,
    L"SmartCard",   IDS_SYS_NAME_SMARTCARD,
    L"AddressBook", IDS_SYS_NAME_ADDRESSBOOK,

    // Physical store names
    CERT_PHYSICAL_STORE_DEFAULT_NAME,           IDS_PHY_NAME_DEFAULT,
    CERT_PHYSICAL_STORE_GROUP_POLICY_NAME,      IDS_PHY_NAME_GROUP_POLICY,
    CERT_PHYSICAL_STORE_LOCAL_MACHINE_NAME,     IDS_PHY_NAME_LOCAL_MACHINE,
    CERT_PHYSICAL_STORE_DS_USER_CERTIFICATE_NAME, IDS_PHY_NAME_DS_USER_CERT,
    CERT_PHYSICAL_STORE_ENTERPRISE_NAME,        IDS_PHY_NAME_ENTERPRISE,
};
#define PREDEFINED_NAME_CNT  (sizeof(PredefinedNameTable) / \
                                    sizeof(PredefinedNameTable[0]))

//+-------------------------------------------------------------------------
//  Localized Name Group Table
//--------------------------------------------------------------------------
typedef struct _LOCALIZED_GROUP_ENTRY {
    DWORD                   cInfo;
    PLOCALIZED_NAME_INFO    rgInfo;
} LOCALIZED_GROUP_ENTRY, *PLOCALIZED_GROUP_ENTRY;

#define REG_LOCALIZED_GROUP             0
#define PREDEFINED_LOCALIZED_GROUP      1
static LOCALIZED_GROUP_ENTRY LocalizedGroupTable[] = {
    // 0 - Loaded from registry
    0, NULL,
    // 1 - Predefined list of names
    PREDEFINED_NAME_CNT, PredefinedNameTable
};
#define LOCALIZED_GROUP_CNT  (sizeof(LocalizedGroupTable) / \
                                    sizeof(LocalizedGroupTable[0]))

// The localized names are loaded once. Uses the above
// LoadFromRegCriticalSection;
static BOOL fLoadedLocalizedNames = FALSE;


//+-------------------------------------------------------------------------
//  OIDInfo allocation and free functions
//--------------------------------------------------------------------------
static void *OIDInfoAlloc(
    IN size_t cbBytes
    )
{
    void *pv;
    pv = malloc(cbBytes);
    if (pv == NULL)
        SetLastError((DWORD) E_OUTOFMEMORY);
    return pv;
}

static void *OIDInfoRealloc(
    IN void *pvOrg,
    IN size_t cb
    )
{
    void *pv;
    if (NULL == (pv = pvOrg ? realloc(pvOrg, cb) : malloc(cb)))
        SetLastError((DWORD) E_OUTOFMEMORY);
    return pv;
}

static void OIDInfoFree(
    IN void *pv
    )
{
    free(pv);
}

//+-------------------------------------------------------------------------
//  Functions called at ProcessDetach to free the groups updated from the
//  registry during CryptFindOIDInfo's first call.
//--------------------------------------------------------------------------
static void FreeGroup(
    PGROUP_ENTRY pGroup
    )
{
    DWORD cInfo = pGroup->cInfo;
    PCCRYPT_OID_INFO pInfo = pGroup->rgInfo;
    for ( ; cInfo > 0; cInfo--, pInfo++)
        OIDInfoFree((LPSTR)pInfo->pszOID);

    OIDInfoFree((PCRYPT_OID_INFO) pGroup->rgInfo);
}

static void FreeRegGroups()
{
    FreeGroup(&RegBeforeGroup);
    FreeGroup(&RegAfterGroup);
}

//+-------------------------------------------------------------------------
//  Free resource strings allocated in groups having localized pwszName's.
//--------------------------------------------------------------------------
static void FreeGroupResources()
{
    DWORD i;
    if (!fLoadedFromRegAndResources)
        // No resource strings allocated
        return;

    for (i = 1; i <= CRYPT_LAST_OID_GROUP_ID; i++) {
        if (GroupTable[i].fLocalize) {
            DWORD cInfo = GroupTable[i].cInfo;
            PCRYPT_OID_INFO pInfo = (PCRYPT_OID_INFO) GroupTable[i].rgInfo;

            for ( ; cInfo > 0; cInfo--, pInfo++) {
                // pwszName is set to pwszNullName if the allocation failed
                if (pwszNullName != pInfo->pwszName) {
                    OIDInfoFree((LPWSTR) pInfo->pwszName);
                    pInfo->pwszName = pwszNullName;
                }
            }
        }
    }
}

//+-------------------------------------------------------------------------
//  Free memory allocated for localized names
//--------------------------------------------------------------------------
static void FreeLocalizedNames()
{
    if (!fLoadedLocalizedNames)
        // No resource strings allocated
        return;

    for (DWORD i = 0; i < LOCALIZED_GROUP_CNT; i++) {
        DWORD cInfo = LocalizedGroupTable[i].cInfo;
        PLOCALIZED_NAME_INFO pInfo = LocalizedGroupTable[i].rgInfo;

        for ( ; cInfo > 0; cInfo--, pInfo++) {
            LPWSTR pwszLocalizedName = (LPWSTR) pInfo->pwszLocalizedName;
            if (pwszNullName != pwszLocalizedName)
                OIDInfoFree(pwszLocalizedName);
        }
    }

    OIDInfoFree(LocalizedGroupTable[REG_LOCALIZED_GROUP].rgInfo);
}

//+-------------------------------------------------------------------------
//  Dll initialization
//--------------------------------------------------------------------------
BOOL
WINAPI
I_CryptOIDInfoDllMain(
        HMODULE hInst,
        ULONG  ulReason,
        LPVOID lpReserved)
{
    switch (ulReason) {
    case DLL_PROCESS_ATTACH:
        hOIDInfoInst = hInst;
        InitializeCriticalSection(&LoadFromRegCriticalSection);
        break;

    case DLL_PROCESS_DETACH:
        FreeRegGroups();
        FreeGroupResources();
        FreeLocalizedNames();
        DeleteCriticalSection(&LoadFromRegCriticalSection);
        break;
    case DLL_THREAD_DETACH:
    default:
        break;
    }

    return TRUE;
}

#ifdef CAPI_INCLUDE_REGISTER_OID

//+-------------------------------------------------------------------------
//  Allocated and format the string consisting of the OID and GROUP_ID:
//
//  For example: 1.2.3.4!6
//--------------------------------------------------------------------------
static LPSTR FormatOIDGroupString(
    IN PCCRYPT_OID_INFO pInfo
    )
{
    LPSTR pszOIDGroupString;
    DWORD cchOIDGroupString;
    char szGroupId[34];

    if (NULL == pInfo || pInfo->cbSize < sizeof(CRYPT_OID_INFO) ||
            (DWORD_PTR) pInfo->pszOID <= 0xFFFF) {
        SetLastError((DWORD) E_INVALIDARG);
        return NULL;
    }

    szGroupId[0] = CONST_OID_GROUP_PREFIX_CHAR;
    _ltoa((long) pInfo->dwGroupId, &szGroupId[1], 10);

    cchOIDGroupString = strlen(pInfo->pszOID) +
        strlen(szGroupId) +
        1;

    if (pszOIDGroupString = (LPSTR) OIDInfoAlloc(cchOIDGroupString)) {
        strcpy(pszOIDGroupString, pInfo->pszOID);
        strcat(pszOIDGroupString, szGroupId);
    }

    return pszOIDGroupString;
}

//+-------------------------------------------------------------------------
//  Wrapper function for calling CryptSetOIDFunctionValue using OID info's
//  encoding type and function name.
//--------------------------------------------------------------------------
static BOOL SetOIDInfoRegValue(
    IN LPCSTR pszOIDGroupString,
    IN LPCWSTR pwszValueName,
    IN DWORD dwValueType,
    IN const BYTE *pbValueData,
    IN DWORD cbValueData
    )
{
    return CryptSetOIDFunctionValue(
        OID_INFO_ENCODING_TYPE,
        CRYPT_OID_FIND_OID_INFO_FUNC,
        pszOIDGroupString,
        pwszValueName,
        dwValueType,
        pbValueData,
        cbValueData
        );
}

//+-------------------------------------------------------------------------
//  Register OID information.
//--------------------------------------------------------------------------
BOOL
WINAPI
CryptRegisterOIDInfo(
    IN PCCRYPT_OID_INFO pInfo,
    IN DWORD dwFlags
    )
{
    BOOL fResult;
    LPSTR pszOIDGroupString = NULL;

    if (NULL == (pszOIDGroupString = FormatOIDGroupString(pInfo)))
        goto FormatOIDGroupStringError;

    if (pInfo->pwszName && L'\0' != *pInfo->pwszName) {
        if (!SetOIDInfoRegValue(
                pszOIDGroupString,
                OID_INFO_NAME_VALUE_NAME,
                REG_SZ,
                (const BYTE *) pInfo->pwszName,
                (wcslen(pInfo->pwszName) + 1) * sizeof(WCHAR)
                )) goto SetOIDInfoRegValueError;
    }
    if (0 != pInfo->Algid) {
        if (!SetOIDInfoRegValue(
                pszOIDGroupString,
                OID_INFO_ALGID_VALUE_NAME,
                REG_DWORD,
                (const BYTE *) &pInfo->Algid,
                sizeof(pInfo->Algid)
                )) goto SetOIDInfoRegValueError;
    }
    if (0 != pInfo->ExtraInfo.cbData) {
        if (!SetOIDInfoRegValue(
                pszOIDGroupString,
                OID_INFO_EXTRA_INFO_VALUE_NAME,
                REG_BINARY,
                pInfo->ExtraInfo.pbData,
                pInfo->ExtraInfo.cbData
                )) goto SetOIDInfoRegValueError;
    }

    if (0 != dwFlags) {
        if (!SetOIDInfoRegValue(
                pszOIDGroupString,
                OID_INFO_FLAGS_VALUE_NAME,
                REG_DWORD,
                (const BYTE *) &dwFlags,
                sizeof(dwFlags)
                )) goto SetOIDInfoRegValueError;
    }

    fResult = TRUE;
CommonReturn:
    OIDInfoFree(pszOIDGroupString);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
TRACE_ERROR(FormatOIDGroupStringError)
TRACE_ERROR(SetOIDInfoRegValueError)
}

//+-------------------------------------------------------------------------
//  Unregister OID information. Only the pszOID and dwGroupId fields are
//  used to identify the OID information to be unregistered.
//--------------------------------------------------------------------------
BOOL
WINAPI
CryptUnregisterOIDInfo(
    IN PCCRYPT_OID_INFO pInfo
    )
{
    BOOL fResult;
    LPSTR pszOIDGroupString = NULL;

    if (NULL == (pszOIDGroupString = FormatOIDGroupString(pInfo)))
        goto FormatOIDGroupStringError;
    if (!CryptUnregisterOIDFunction(
            OID_INFO_ENCODING_TYPE,
            CRYPT_OID_FIND_OID_INFO_FUNC,
            pszOIDGroupString
            ))
        goto UnregisterOIDFunctionError;
    fResult = TRUE;
CommonReturn:
    OIDInfoFree(pszOIDGroupString);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
TRACE_ERROR(FormatOIDGroupStringError)
TRACE_ERROR(UnregisterOIDFunctionError)
}

//+-------------------------------------------------------------------------
//  Called by CryptEnumOIDFunction to enumerate through all the
//  registered OID information.
//
//  Called within critical section
//--------------------------------------------------------------------------
static BOOL WINAPI EnumRegistryCallback(
    IN DWORD dwEncodingType,
    IN LPCSTR pszFuncName,
    IN LPCSTR pszOID,
    IN DWORD cValue,
    IN const DWORD rgdwValueType[],
    IN LPCWSTR const rgpwszValueName[],
    IN const BYTE * const rgpbValueData[],
    IN const DWORD rgcbValueData[],
    IN void *pvArg
    )
{
    DWORD cchOID;
    LPCWSTR pwszName;
    DWORD cchName = 0;
    LPCSTR pszGroupId;
    DWORD dwGroupId = 0;
    ALG_ID Algid = 0;
    CRYPT_DATA_BLOB ExtraInfo = {0, NULL};
    DWORD dwFlags = 0;
    DWORD cbExtra;
    BYTE *pbExtra;
    PGROUP_ENTRY pGroup;
    PCRYPT_OID_INFO pInfo;

    // The pszOID consists of OID!<dwGroupId>, for example, 1.2.3!1
    // Start at the end and search for the '!'
    cchOID = strlen(pszOID);
    pszGroupId = pszOID + cchOID;
    while (pszGroupId > pszOID && CONST_OID_GROUP_PREFIX_CHAR != *pszGroupId)
        pszGroupId--;

    if (CONST_OID_GROUP_PREFIX_CHAR == *pszGroupId) {
        cchOID = (DWORD)(pszGroupId - pszOID);
        dwGroupId = (DWORD) atol(pszGroupId + 1);
    } else
        // Name is missing "!". Skip It.
        return TRUE;

    while (cValue--) {
        LPCWSTR pwszValueName = rgpwszValueName[cValue];
        DWORD dwValueType = rgdwValueType[cValue];
        const BYTE *pbValueData = rgpbValueData[cValue];
        DWORD cbValueData = rgcbValueData[cValue];

        if (0 == _wcsicmp(pwszValueName, OID_INFO_NAME_VALUE_NAME)) {
            if (REG_SZ == dwValueType) {
                pwszName = (LPWSTR) pbValueData;
                cchName = wcslen(pwszName);
            }
        } else if (0 == _wcsicmp(pwszValueName, OID_INFO_ALGID_VALUE_NAME)) {
            if (REG_DWORD == dwValueType &&
                    cbValueData >= sizeof(Algid))
                memcpy(&Algid, pbValueData, sizeof(Algid));
        } else if (0 == _wcsicmp(pwszValueName,
                OID_INFO_EXTRA_INFO_VALUE_NAME)) {
            if (REG_BINARY == dwValueType) {
                ExtraInfo.cbData = cbValueData;
                ExtraInfo.pbData = (BYTE *) pbValueData;
            }
        } else if (0 == _wcsicmp(pwszValueName, OID_INFO_FLAGS_VALUE_NAME)) {
            if (REG_DWORD == dwValueType &&
                    cbValueData >= sizeof(dwFlags))
                memcpy(&dwFlags, pbValueData, sizeof(dwFlags));
        }
    }

    cbExtra = LEN_ALIGN(cchOID + 1) +
        LEN_ALIGN((cchName + 1) * sizeof(WCHAR)) +
        ExtraInfo.cbData;
    if (NULL == (pbExtra = (BYTE *) OIDInfoAlloc(cbExtra)))
        return FALSE;

    if (dwFlags & CRYPT_INSTALL_OID_INFO_BEFORE_FLAG)
        pGroup = &RegBeforeGroup;
    else
        pGroup = &RegAfterGroup;

    if (NULL == (pInfo = (PCRYPT_OID_INFO) OIDInfoRealloc(
            (PCRYPT_OID_INFO) pGroup->rgInfo,
            (pGroup->cInfo + 1) * sizeof(CRYPT_OID_INFO)))) {
        OIDInfoFree(pbExtra);
        return FALSE;
    }
    pGroup->rgInfo = pInfo;
    pInfo = &pInfo[pGroup->cInfo++];

    pInfo->cbSize = sizeof(CRYPT_OID_INFO);
    pInfo->pszOID = (LPCSTR) pbExtra;
    if (cchOID)
        memcpy(pbExtra, pszOID, cchOID);
    *( ((LPSTR) pbExtra) + cchOID) = '\0';
    pbExtra += LEN_ALIGN(cchOID + 1);

    pInfo->pwszName = (LPCWSTR) pbExtra;
    if (cchName)
        memcpy(pbExtra, pwszName, (cchName + 1) * sizeof(WCHAR));
    else
        *((LPWSTR) pbExtra) = L'\0';
    pbExtra += LEN_ALIGN((cchName + 1) * sizeof(WCHAR));

    pInfo->dwGroupId = dwGroupId;
    pInfo->Algid = Algid;
    pInfo->ExtraInfo.cbData = ExtraInfo.cbData;
    if (ExtraInfo.cbData > 0) {
        pInfo->ExtraInfo.pbData = pbExtra;
        memcpy(pbExtra, ExtraInfo.pbData, ExtraInfo.cbData);
    } else
        pInfo->ExtraInfo.pbData = NULL;

    return TRUE;
}
#endif //CAPI_INCLUDE_REGISTER_OID
//+-------------------------------------------------------------------------
//  Allocate and load the string for the specified resource.
//
//  If LoadString or allocation fails, returns predefined pointer to an
//  empty string.
//--------------------------------------------------------------------------
static LPWSTR AllocAndLoadOIDNameString(
    IN UINT uID
    )
{
#ifndef _XBOX
    WCHAR wszResource[MAX_RESOURCE_OID_NAME_LENGTH + 1];
    int cchResource;
    int cbResource;
    LPWSTR pwszDst;

    cchResource = LoadStringU(hOIDInfoInst, uID, wszResource,
        MAX_RESOURCE_OID_NAME_LENGTH);
    assert(0 < cchResource);
    if (0 >= cchResource)
        return (LPWSTR) pwszNullName;

    cbResource = (cchResource + 1) * sizeof(WCHAR);
    pwszDst = (LPWSTR) OIDInfoAlloc(cbResource);
    assert(pwszDst);
    if (NULL == pwszDst)
        return (LPWSTR) pwszNullName;
    memcpy((BYTE *) pwszDst, (BYTE *) wszResource, cbResource);
    return pwszDst;
#else
	return NULL;
#endif
}

//+-------------------------------------------------------------------------
//  Allocate and copy the string.
//
//  If allocation fails, returns predefined pointer to an empty string.
//--------------------------------------------------------------------------
static LPWSTR AllocAndCopyOIDNameString(
    IN LPCWSTR pwszSrc
    )
{
    DWORD cbSrc;
    LPWSTR pwszDst;

    cbSrc = (wcslen(pwszSrc) + 1) * sizeof(WCHAR);
    pwszDst = (LPWSTR) OIDInfoAlloc(cbSrc);
    assert(pwszDst);
    if (NULL == pwszDst)
        return (LPWSTR) pwszNullName;
    memcpy((BYTE *) pwszDst, (BYTE *) pwszSrc, cbSrc);
    return pwszDst;
}

//+-------------------------------------------------------------------------
//  Does a LoadString for pwszName's initialized with resource IDs in groups
//  with fLocalize set.
//--------------------------------------------------------------------------
static void LoadGroupResources()
{
    DWORD i;
    for (i = 1; i <= CRYPT_LAST_OID_GROUP_ID; i++) {
        if (GroupTable[i].fLocalize) {
            DWORD cInfo = GroupTable[i].cInfo;
            PCRYPT_OID_INFO pInfo = (PCRYPT_OID_INFO) GroupTable[i].rgInfo;
            for ( ; cInfo > 0; cInfo--, pInfo++) {
                UINT_PTR uID;
                uID = (UINT_PTR) pInfo->pwszName;
                if (uID <= 0xFFFF)
                    pInfo->pwszName = AllocAndLoadOIDNameString((UINT)uID);
                else
                    // ProcessDetach expects all pwszName's to be allocated
                    pInfo->pwszName = AllocAndCopyOIDNameString(
                        pInfo->pwszName);
            }
        }
    }
}

//+-------------------------------------------------------------------------
//  Load OID Information from the registry. Updates the RegBeforeGroup and
//  RegAfterGroup.
//
//  Loads resource strings in groups enabling localization of pwszName's.
//--------------------------------------------------------------------------
static void LoadFromRegistryAndResources()
{
    if (fLoadedFromRegAndResources)
        return;
    EnterCriticalSection(&LoadFromRegCriticalSection);
    if (!fLoadedFromRegAndResources) {
#ifdef CAPI_INCLUDE_REGISTER_OID
        CryptEnumOIDFunction(
            OID_INFO_ENCODING_TYPE,
            CRYPT_OID_FIND_OID_INFO_FUNC,
            NULL,                           // pszOID
            0,                              // dwFlags
            NULL,                           // pvArg
            EnumRegistryCallback
            );
#endif  //CAPI_INCLUDE_REGISTER_OID          
        LoadGroupResources();
        fLoadedFromRegAndResources = TRUE;
    }
    LeaveCriticalSection(&LoadFromRegCriticalSection);
}

//+-------------------------------------------------------------------------
//  Search the group according to the specified dwKeyType.
//
//  Note, the groups updated from the registry, RegBeforeGroup and
//  RegAfterGroup, may contain any GROUP_ID.
//--------------------------------------------------------------------------
static PCCRYPT_OID_INFO SearchGroup(
    IN DWORD dwKeyType,
    IN void *pvKey,
    IN DWORD dwGroupId,
    IN PCGROUP_ENTRY pGroup
    )
{
    DWORD cInfo = pGroup->cInfo;
    PCCRYPT_OID_INFO pInfo = pGroup->rgInfo;
    for ( ; cInfo > 0; cInfo--, pInfo++) {
        if (dwGroupId && dwGroupId != pInfo->dwGroupId)
            continue;

        switch (dwKeyType) {
            case CRYPT_OID_INFO_OID_KEY:
                if (0 == _stricmp((LPSTR) pvKey, pInfo->pszOID))
                    return pInfo;
                break;
            case CRYPT_OID_INFO_NAME_KEY:
                if (0 == _wcsicmp((LPWSTR) pvKey, pInfo->pwszName))
                    return pInfo;
                break;
            case CRYPT_OID_INFO_ALGID_KEY:
                if (*((ALG_ID *) pvKey) == pInfo->Algid)
                    return pInfo;
                break;
            case CRYPT_OID_INFO_SIGN_KEY:
                {
                    ALG_ID *paiKey = (ALG_ID *) pvKey;
                    ALG_ID aiPubKey;

                    if (sizeof(ALG_ID) <= pInfo->ExtraInfo.cbData)
                        aiPubKey = *((ALG_ID *) pInfo->ExtraInfo.pbData);
                    else
                        aiPubKey = 0;

                    if (paiKey[0] == pInfo->Algid &&
                            paiKey[1] == aiPubKey)
                    return pInfo;
                }
                break;
            default:
                SetLastError((DWORD) E_INVALIDARG);
                return NULL;
        }
    }

    return NULL;
}

//+-------------------------------------------------------------------------
//  Find OID information. Returns NULL if unable to find any information
//  for the specified key and group.
//--------------------------------------------------------------------------
PCCRYPT_OID_INFO
WINAPI
CryptFindOIDInfo(
    IN DWORD dwKeyType,
    IN void *pvKey,
    IN DWORD dwGroupId      // 0 => any group
    )
{
    PCCRYPT_OID_INFO pInfo;

    LoadFromRegistryAndResources();

    if (RegBeforeGroup.cInfo && NULL != (pInfo = SearchGroup(
            dwKeyType,
            pvKey,
            dwGroupId,
            &RegBeforeGroup
            ))) return pInfo;
    if (0 == dwGroupId) {
        DWORD i;
        for (i = 1; i <= CRYPT_LAST_OID_GROUP_ID; i++) {
            if (pInfo = SearchGroup(
                dwKeyType,
                pvKey,
                0,
                &GroupTable[i]
                )) return pInfo;
        }
    } else if (dwGroupId <= CRYPT_LAST_OID_GROUP_ID) {
        if (pInfo = SearchGroup(
                dwKeyType,
                pvKey,
                dwGroupId,
                &GroupTable[dwGroupId]
                )) return pInfo;
    }

    if (RegAfterGroup.cInfo && NULL != (pInfo = SearchGroup(
            dwKeyType,
            pvKey,
            dwGroupId,
            &RegAfterGroup
            ))) return pInfo;

    return NULL;
}


//+-------------------------------------------------------------------------
//  Enumerate the group.
//--------------------------------------------------------------------------
static BOOL EnumGroup(
    IN DWORD dwGroupId,
    IN PCGROUP_ENTRY pGroup,
    IN void *pvArg,
    IN PFN_CRYPT_ENUM_OID_INFO pfnEnumOIDInfo
    )
{
    DWORD cInfo = pGroup->cInfo;
    PCCRYPT_OID_INFO pInfo = pGroup->rgInfo;
    for ( ; cInfo > 0; cInfo--, pInfo++) {
        if (dwGroupId && dwGroupId != pInfo->dwGroupId)
            continue;

        if (!pfnEnumOIDInfo(pInfo, pvArg))
            return FALSE;
    }
    return TRUE;
}

//+-------------------------------------------------------------------------
//  Enumerate the OID information.
//
//  pfnEnumOIDInfo is called for each OID information entry.
//
//  Setting dwGroupId to 0 matches all groups. Otherwise, only enumerates
//  entries in the specified group.
//
//  dwFlags currently isn't used and must be set to 0.
//--------------------------------------------------------------------------
BOOL
WINAPI
CryptEnumOIDInfo(
    IN DWORD dwGroupId,
    IN DWORD dwFlags,
    IN void *pvArg,
    IN PFN_CRYPT_ENUM_OID_INFO pfnEnumOIDInfo
    )
{
    LoadFromRegistryAndResources();

    if (RegBeforeGroup.cInfo && !EnumGroup(
            dwGroupId,
            &RegBeforeGroup,
            pvArg,
            pfnEnumOIDInfo
            )) return FALSE;
    if (0 == dwGroupId) {
        DWORD i;
        for (i = 1; i <= CRYPT_LAST_OID_GROUP_ID; i++) {
            if (!EnumGroup(
                    0,                  // dwGroupId
                    &GroupTable[i],
                    pvArg,
                    pfnEnumOIDInfo
                    )) return FALSE;
        }
    } else if (dwGroupId <= CRYPT_LAST_OID_GROUP_ID) {
        if (!EnumGroup(
                dwGroupId,
                &GroupTable[dwGroupId],
                pvArg,
                pfnEnumOIDInfo
                )) return FALSE;
    }

    if (RegAfterGroup.cInfo && !EnumGroup(
            dwGroupId,
            &RegAfterGroup,
            pvArg,
            pfnEnumOIDInfo
            )) return FALSE;

    return TRUE;
}



//+=========================================================================
//  Localized Name Functions
//==========================================================================
#ifdef CAPI_INCLUDE_REGISTER_OID    

//+-------------------------------------------------------------------------
//  Called by CryptEnumOIDFunction to enumerate through all the
//  registered localized name values.
//
//  Called within critical section
//
//  Note at ProcessDetach, the Info entry pwszLocalizedName strings are freed.
//  Therefore, for each Info entry, do a single allocation for both the
//  pwszLocalizedName and pwszCryptName. The pwszCryptName immediately
//  follows the pwszLocalizedName.
//--------------------------------------------------------------------------
static BOOL WINAPI EnumRegLocalizedNamesCallback(
    IN DWORD dwEncodingType,
    IN LPCSTR pszFuncName,
    IN LPCSTR pszOID,
    IN DWORD cValue,
    IN const DWORD rgdwValueType[],
    IN LPCWSTR const rgpwszValueName[],
    IN const BYTE * const rgpbValueData[],
    IN const DWORD rgcbValueData[],
    IN void *pvArg
    )
{
    BOOL fResult;
    DWORD cInfo = LocalizedGroupTable[REG_LOCALIZED_GROUP].cInfo;
    PLOCALIZED_NAME_INFO pInfo =
        LocalizedGroupTable[REG_LOCALIZED_GROUP].rgInfo;

    assert(CRYPT_LOCALIZED_NAME_ENCODING_TYPE == dwEncodingType);
    assert(0 == _stricmp(CRYPT_OID_FIND_LOCALIZED_NAME_FUNC, pszFuncName));
    assert(0 == _stricmp(CRYPT_LOCALIZED_NAME_OID, pszOID));

    while (cValue--) {
        if (REG_SZ == rgdwValueType[cValue]) {
            LPCWSTR pwszLocalizedName = (LPCWSTR) rgpbValueData[cValue];
            DWORD cchLocalizedName;
            DWORD cbLocalizedName;
            LPCWSTR pwszCryptName = rgpwszValueName[cValue];
            DWORD cbCryptName;

            LPWSTR pwszBothNames;
            PLOCALIZED_NAME_INFO pNewInfo;

            // Check for empty name string
            cchLocalizedName = wcslen(pwszLocalizedName);
            if (0 == cchLocalizedName)
                continue;

            cbLocalizedName = (cchLocalizedName + 1) * sizeof(WCHAR);
            cbCryptName = (wcslen(pwszCryptName) + 1) * sizeof(WCHAR);

            if (NULL == (pwszBothNames = (LPWSTR) OIDInfoAlloc(
                    cbLocalizedName + cbCryptName)))
                goto OutOfMemory;

            if (NULL == (pNewInfo = (PLOCALIZED_NAME_INFO) OIDInfoRealloc(
                    pInfo, (cInfo + 1) * sizeof(LOCALIZED_NAME_INFO)))) {
                OIDInfoFree(pwszBothNames);
                goto OutOfMemory;
            }
            pInfo = pNewInfo;
            pInfo[cInfo].pwszLocalizedName = (LPCWSTR) pwszBothNames;
            memcpy(pwszBothNames, pwszLocalizedName, cbLocalizedName);
            pwszBothNames =
                (LPWSTR) ((BYTE *) pwszBothNames + cbLocalizedName);
            pInfo[cInfo].pwszCryptName = (LPCWSTR) pwszBothNames;
            memcpy(pwszBothNames, pwszCryptName, cbCryptName);
            cInfo++;
        }
    }
    fResult = TRUE;

CommonReturn:
    LocalizedGroupTable[REG_LOCALIZED_GROUP].cInfo = cInfo;
    LocalizedGroupTable[REG_LOCALIZED_GROUP].rgInfo = pInfo;
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
TRACE_ERROR(OutOfMemory)
}
#endif    

static void LoadPredefinedNameResources()
{
    for (DWORD i = 0; i < PREDEFINED_NAME_CNT; i++) {
        LPWSTR pwszLocalizedName;

        // Note, the following always returns a non-NULL string pointer.
        pwszLocalizedName = AllocAndLoadOIDNameString(
            PredefinedNameTable[i].uIDLocalizedName);
        if (L'\0' == *pwszLocalizedName)
            pwszLocalizedName = NULL;
        PredefinedNameTable[i].pwszLocalizedName = (LPCWSTR) pwszLocalizedName;
    }
}

static void LoadLocalizedNamesFromRegAndResources()
{
    if (fLoadedLocalizedNames)
        return;

    EnterCriticalSection(&LoadFromRegCriticalSection);
    if (!fLoadedLocalizedNames) {
#ifdef CAPI_INCLUDE_REGISTER_OID    
        CryptEnumOIDFunction(
            CRYPT_LOCALIZED_NAME_ENCODING_TYPE,
            CRYPT_OID_FIND_LOCALIZED_NAME_FUNC,
            CRYPT_LOCALIZED_NAME_OID,
            0,                              // dwFlags
            NULL,                           // pvArg
            EnumRegLocalizedNamesCallback
            );
#endif //CAPI_INCLUDE_REGISTER_OID          
        LoadPredefinedNameResources();
        fLoadedLocalizedNames = TRUE;
    }
    LeaveCriticalSection(&LoadFromRegCriticalSection);
}

//+-------------------------------------------------------------------------
//  Find the localized name for the specified name. For example, find the
//  localized name for the "Root" system store name. A case insensitive
//  string comparison is done.
//
//  Returns NULL if unable to find the the specified name.
//--------------------------------------------------------------------------
LPCWSTR
WINAPI
CryptFindLocalizedName(
    IN LPCWSTR pwszCryptName
    )
{
    if (NULL == pwszCryptName || L'\0' == *pwszCryptName)
        return NULL;

    LoadLocalizedNamesFromRegAndResources();

    for (DWORD i = 0; i < LOCALIZED_GROUP_CNT; i++) {
        DWORD cInfo = LocalizedGroupTable[i].cInfo;
        PLOCALIZED_NAME_INFO pInfo = LocalizedGroupTable[i].rgInfo;
        for ( ; cInfo > 0; cInfo--, pInfo++) {
            if (0 == _wcsicmp(pwszCryptName, pInfo->pwszCryptName))
                return pInfo->pwszLocalizedName;
        }
    }

    return NULL;
}
