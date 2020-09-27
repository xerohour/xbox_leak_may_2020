//+--------------------------------------------------------------------------
//
// Microsoft Windows
// Copyright (C) Microsoft Corporation, 1996 - 1999
//
// File:        certca.h
//
// Contents:    Definition of the CA Info API
//
// History:     12-dec-97  petesk  created
//
//---------------------------------------------------------------------------


#ifndef __CERTCA_H__
#define __CERTCA_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C"{
#endif


#include <wincrypt.h>

#if !defined(_CERTCLI_)
#define CERTCLIAPI DECLSPEC_IMPORT
#else
#define CERTCLIAPI
#endif


typedef VOID *  HCAINFO;

typedef VOID *  HCERTTYPE;


// Flags used by the API's
#define CA_FLAG_SCOPE_DNS               0x00000001
#define CA_FLAG_SCOPE_LOCAL             0x00000002
#define CA_FLAG_ENUM_ALL_TYPES          0x00000004
#define CT_FIND_INCLUDE_DELETED         0x00000008
#define CA_FIND_INCLUDE_UNTRUSTED       0x00000010

// we're running as local system
#define CT_FIND_LOCAL_SYSTEM            0x00000020
#define CA_FIND_LOCAL_SYSTEM            CT_FIND_LOCAL_SYSTEM

#define CA_FIND_INCLUDE_NON_TEMPLATE_CA 0x00000040   // Include Ca's that do not support templates 



// Return machine types, as opposed to user types
#define CT_ENUM_MACHINE_TYPES           0x00000040

// Return user types, as opposed to user types
#define CT_ENUM_USER_TYPES              0x00000080

// Enumerate only those types in the DS
#define CT_FLAG_ENUM_REAL_TYPES         0x00000100

// Enumerate/find only those CA's in our own domain
#define CA_FIND_LOCAL_DOMAIN            0x00000200

// Disable the cache expiration check
#define CT_FLAG_NO_CACHE_LOOKUP         0x00000400

// Enumerate only those types in the DS
#define CT_FLAG_NO_DOWNLEVEL_TYPES      0x00000100

// The value passed in for scop is an LDAP binding handle to use during finds
#define CT_FLAG_SCOPE_IS_LDAP_HANDLE     0x00000800  
#define CA_FLAG_SCOPE_IS_LDAP_HANDLE     CT_FLAG_SCOPE_IS_LDAP_HANDLE

//
// Certification Authority manipulation API's
//

//
// API's for finding CA's
//
// The following section defines API's for finding CA's
// The following parameters are used in these API's
//

//
// wszScope            
//	- if CA_FLAG_SCOPE_LOCAL is set, scope indicates a particular machine,
//	  in DNS format.  (not currently implemented)
//
// dwFlags
//	- An or'd selection of flags:
//
//	  CA_FLAG_SCOPE_LOCAL: Indicates that this CA is a standalone CA, with
//	  wszScope indicating machine name.  wszScope may be set to NULL to
//	  indicate the current machine.  (not currently implemented)


// CAFindCAByName
//
// Given the Name of a CA (CN), find the CA within
// the given domain and return the given phCAInfo structure.
//
// wszCAName           - Common name of the CA
//
// phCAInfo            - Handle to the returned CA.
//
// See above for other parameter definitions
//
// Return:               Returns S_OK if CA was found.
//

CERTCLIAPI
HRESULT
WINAPI
CAFindByName(
        IN  LPCWSTR     wszCAName,
        IN  LPCWSTR     wszScope,
        IN  DWORD       dwFlags,
        OUT HCAINFO *   phCAInfo
        );

//
// CAFindByCertType
//
// Given the Name of a Cert Type, find all the CAs within
// the given domain and return the given phCAInfo structure.
//
// wszCertType      - Common Name of the cert type
//
// phCAInfo         - Handle to enumeration of CA's supporting
//                  - cert type.
//
//
// See above for other parameter definitions
//
// Return:               Returns S_OK on success.
//                       Will return S_OK if none are found.
//                       *phCAInfo will contain NULL
//

CERTCLIAPI
HRESULT
WINAPI
CAFindByCertType(
        IN  LPCWSTR     wszCertType,
        IN  LPCWSTR     wszScope,
        IN  DWORD       fFlags,
        OUT HCAINFO *   phCAInfo
        );


//
// CAFindByIssuerDN
// Given the DN of a CA, find the CA within
// the given domain and return the given phCAInfo handle.
//
// pIssuerDN - a cert name blob from the CA's certificate.
//
// See above for other parameter definitions
//
// Return:               Returns S_OK if CA was found.
//


CERTCLIAPI
HRESULT
WINAPI
CAFindByIssuerDN(
        IN  CERT_NAME_BLOB const *  pIssuerDN,
        IN  LPCWSTR                 wszScope,
        IN  DWORD                   fFlags,
        OUT HCAINFO *               phCAInfo
        );


//
// CAEnumFirstCA
// Enumerate the CA's in a scope
//
// phCAInfo         - Handle to enumeration of CA's supporting
//                  - cert type.
//
// See above for other parameter definitions
//
// Return:               Returns S_OK on success.
//                       Will return S_OK if none are found.
//                       *phCAInfo will contain NULL
//

CERTCLIAPI
HRESULT
WINAPI
CAEnumFirstCA(
    IN  LPCWSTR          wszScope,
    IN  DWORD            fFlags,
    OUT HCAINFO *        phCAInfo
    );


//
// CAEnumNextCA
// Find the Next CA in an enumeration.
//
// hPrevCA          - Current ca in an enumeration.
//
// phCAInfo         - next ca in an enumeration.
//
// Return:               Returns S_OK on success.
//                       Will return S_OK if none are found.
//                       *phCAInfo will contain NULL
//

CERTCLIAPI
HRESULT
WINAPI
CAEnumNextCA(
    IN  HCAINFO          hPrevCA,
    OUT HCAINFO *        phCAInfo
    );

//
// CACreateNewCA
// Create a new CA of given name.
//
// wszCAName           - Common name of the CA
//
// phCAInfo            - Handle to the returned CA.
//
// See above for other parameter definitions
//
// Return:               Returns S_OK if CA was created.
//
// NOTE:  Actual updates to the CA object may not occur
//        until CAUpdateCA is called.
//        In order to successfully update a created CA,
//        the Certificate must be set, as well as the
//        Certificate Types property.
//

CERTCLIAPI
HRESULT
WINAPI
CACreateNewCA(
        IN  LPCWSTR     wszCAName,
        IN  LPCWSTR     wszScope,
        IN  DWORD       fFlags,
        OUT HCAINFO *   phCAInfo
        );

//
// CAUpdateCA
// Write any changes made to the CA back to the CA object.
//
// hCAInfo - Handle to an open CA object.
//

CERTCLIAPI
HRESULT
WINAPI
CAUpdateCA(
        IN HCAINFO    hCAInfo
        );

//
// CADeleteCA
// Delete the CA object from the DS.
//
// hCAInfo - Handle to an open CA object.
//

CERTCLIAPI
HRESULT
WINAPI
CADeleteCA(
        IN HCAINFO    hCAInfo
        );

//
// CACountCAs
// return the number of CAs in this enumeration
//

CERTCLIAPI
DWORD
WINAPI
CACountCAs(
           IN  HCAINFO  hCAInfo
           );


//
// CACloseCA
// Close an open CA handle
//
// hCAInfo - Handle to an open CA object.
//

CERTCLIAPI
HRESULT
WINAPI
CACloseCA(
          IN HCAINFO hCA
          );



//
// CAGetCAProperty - Given a property name, retrieve a
// property from a CAInfo.
//
// hCAInfo              - Handle to an open CA object.
//
// wszPropertyName      - Name of the CA property
//
// pawszPropertyValue   - A pointer into which an array
//                        of WCHAR strings is written, containing
//                        the values of the property.  The last
//                        element of the array points to NULL.
//                        If the property is single valued, then
//                        the array returned contains 2 elements,
//                        the first pointing to the value, the second
//                        pointing to NULL.  This pointer must be
//                        freed by CAFreeCAProperty.
//
//
// Returns              - S_OK on success.
//

CERTCLIAPI
HRESULT
WINAPI
CAGetCAProperty(
    IN  HCAINFO     hCAInfo,
    IN  LPCWSTR     wszPropertyName,
    OUT LPWSTR **   pawszPropertyValue
    );


//
// CAFreeProperty
// Free's a previously retrieved property value.
//
// hCAInfo              - Handle to an open CA object.
//
// awszPropertyValue - pointer to the previously retrieved
//                   - property value.
//

CERTCLIAPI
HRESULT
WINAPI
CAFreeCAProperty(
    IN  HCAINFO     hCAInfo,
    LPWSTR *        awszPropertyValue
    );


//
// CASetCAProperty - Given a property name, set it's value.
//
// hCAInfo              - Handle to an open CA object.
//
// wszPropertyName      - Name of the CA property
//
// awszPropertyValue   - An array of values to set
//                      - for this property.  The last element of this
//                      - array should be NULL.
//                      - For single valued properties, the values beyond thie
//                      - first will be ignored upon update.
//
// Returns              - S_OK on success.
//

CERTCLIAPI
HRESULT
WINAPI
CASetCAProperty(
    IN HCAINFO      hCAInfo,
    IN LPCWSTR     wszPropertyName,
    IN LPWSTR *    awszPropertyValue
    );


//
// CA Properties
//
// simple name of the CA
#define CA_PROP_NAME                    L"cn"

// dns name of the machine
#define CA_PROP_DISPLAY_NAME            L"displayName"

// dns name of the machine
#define CA_PROP_DNSNAME                 L"dNSHostName"

// DS Location of CA object
#define CA_PROP_DSLOCATION              L"distinguishedName"

// Supported cert types
#define CA_PROP_CERT_TYPES              L"certificateTemplates"

// Base Supported cert types
#define CA_PROP_BASE_CERT_TYPES		L"baseCertificateTemplates"

// Supported signature algs
#define CA_PROP_SIGNATURE_ALGS          L"signatureAlgorithms"

// DN of the CA's cert
#define CA_PROP_CERT_DN                 L"cACertificateDN"

// DN of the CA's cert
#define CA_PROP_ENROLLMENT_PROVIDERS    L"enrollmentProviders"

// CA's description
#define CA_PROP_DESCRIPTION		L"Description"

//
// CAGetCACertificate - Return the current certificate for
// this ca.
//
// hCAInfo              - Handle to an open CA object.
//
// ppCert               - Pointer into which a certificate
//                      - is written.  This certificate must
//                      - be freed via CertFreeCertificateContext.
//                      - This value will be NULL if no certificate
//                      - is set for this CA.
//

//
// CAGetCertTypeFlags
// Retrieve cert type flags
//
// hCertType            - handle to the CertType
//
// pdwFlags             - pointer to DWORD receiving flags
//

CERTCLIAPI
HRESULT
WINAPI
CAGetCAFlags(
    IN HCAINFO  hCAInfo,
    OUT DWORD  *pdwFlags
    );

//
// Cert Type Flags
// The CA supports certificate templates
#define CA_FLAG_NO_TEMPLATE_SUPPORT                 0x00000001

// The CA supports NT authentication for requests
#define CA_FLAG_SUPPORTS_NT_AUTHENTICATION          0x00000002

// The cert requests may be pended
#define CA_FLAG_CA_SUPPORTS_MANUAL_AUTHENTICATION   0x00000004

#define CA_MASK_SETTABLE_FLAGS              0x0000ffff


// Old name...
#define CT_FLAG_IS_SUBJECT_REQ  CT_FLAG_ENROLLEE_SUPPLIES_SUBJECT
//
// CASetCAFlags
// Sets the Flags of a cert type
//
// hCertType            - handle to the CertType
//
// dwFlags              - Flags to be set
//

CERTCLIAPI
HRESULT
WINAPI
CASetCAFlags(
    IN HCAINFO             hCAInfo,
    IN DWORD               dwFlags
    );

CERTCLIAPI
HRESULT
WINAPI
CAGetCACertificate(
    IN  HCAINFO     hCAInfo,
    OUT PCCERT_CONTEXT *ppCert
    );


//
// CASetCACertificate - Set the certificate for a CA
// this ca.
//
// hCAInfo              - Handle to an open CA object.
//
// pCert                - Pointer to a certificat to set as the CA's certificte.
//

CERTCLIAPI
HRESULT
WINAPI
CASetCACertificate(
    IN  HCAINFO     hCAInfo,
    IN PCCERT_CONTEXT pCert
    );


//
// CAGetCAExpiration
// Get the expirations period for a CA.
//
// hCAInfo              - Handle to an open CA handle.
//
// pdwExpiration        - expiration period in dwUnits time
//
// pdwUnits             - Units identifier
//

CERTCLIAPI
HRESULT
WINAPI
CAGetCAExpiration(
                HCAINFO hCAInfo,
                DWORD * pdwExpiration,
                DWORD * pdwUnits
                );

#define CA_UNITS_DAYS   1
#define CA_UNITS_WEEKS  2
#define CA_UNITS_MONTHS 3
#define CA_UNITS_YEARS  4


//
// CASetCAExpiration
// Set the expirations period for a CA.
//
// hCAInfo              - Handle to an open CA handle.
//
// dwExpiration         -
//
// dwUnits              - Units identifier
//

CERTCLIAPI
HRESULT
WINAPI
CASetCAExpiration(
                HCAINFO hCAInfo,
                DWORD dwExpiration,
                DWORD dwUnits
                );
//
// CASetCASecurity
// Set the list of Users, Groups, and Machines allowed
// to access this CA.
//
// hCAInfo              - Handle to an open CA handle.
//
// pSD                  - Security descriptor for this CA
//

CERTCLIAPI
HRESULT
WINAPI
CASetCASecurity(
                     IN HCAINFO                 hCAInfo,
                     IN PSECURITY_DESCRIPTOR    pSD
                     );
//
// CAGetCASecurity
// Get the list of Users, Groups, and Machines allowed
// to access this CA.
//
// hCAInfo              - Handle to an open CA handle.
//
// ppSD                 - Pointer to a location receiving
//                      - the pointer to the security descriptor
//                      - Free via LocalFree
//

CERTCLIAPI
HRESULT
WINAPI
CAGetCASecurity(
                     IN  HCAINFO                    hCAInfo,
                     OUT PSECURITY_DESCRIPTOR *     ppSD
                     );
//
//
// CAAccessCheck
// Determine whether the principal specified by
// ClientToken can get a cert from the CA.
//
// hCAInfo              - Handle to the CA
//
// ClientToken          - Handle to an impersonation token
//                      - that represents the client attempting
//                      - request this cert type. The handle must
//                      - have TOKEN_QUERY access to the token;
//                      - otherwise, the function fails with
//                      - ERROR_ACCESS_DENIED.
//
// Return: S_OK on success
//

CERTCLIAPI
HRESULT
WINAPI
CAAccessCheck(
    IN  HCAINFO     hCAInfo,
    IN HANDLE       ClientToken
    );

//
// CAEnumCertTypesForCA - Given a HCAINFO, retrieve handle to
// the cert types supported, or known by this CA.
// CAEnumNextCertType can be used to enumerate through the
// cert types.
//
// hCAInfo              - Handle to an open CA handle.
//
// dwFlags              - The following flags may be or'd together
//                      - CA_FLAG_ENUM_ALL_TYPES - Instead of enumerating
//                      - the certificate types suppoerted by the CA,
//                      - enumerate ALL certificate types which
//                      - the CA may choose to support.
//
//  phCertType          - Enumeration of certificate types.
//


CERTCLIAPI
HRESULT
WINAPI
CAEnumCertTypesForCA(
    IN  HCAINFO     hCAInfo,
    IN  DWORD       dsFlags,
    OUT HCERTTYPE * phCertType
    );


//
// CAAddCACertificateType
// Add a certificate type to a CA.
// If the cert type has already been added to the
// ca, it will not be added again.
//
// hCAInfo              - Handle to an open CA.
//
// hCertType            - Cert type to add to CA.
//

CERTCLIAPI
HRESULT
WINAPI
CAAddCACertificateType(
                HCAINFO hCAInfo,
                HCERTTYPE hCertType
                );


//
// CADeleteCACertificateType
// Remove a certificate type from a CA.
// If the CA does not include this cert type.
// This call does nothing.
//
// hCAInfo              - Handle to an open CA.
//
// hCertType            - Cert type to delete from CA.
//

CERTCLIAPI
HRESULT
WINAPI
CARemoveCACertificateType(
                HCAINFO hCAInfo,
                HCERTTYPE hCertType
                );




//
//
// Certificate Type API's
//
//




//
// CAEnumCertTypes - Retrieve a handle to all known cert types
// CAEnumNextCertType can be used to enumerate through the
// cert types.
//

// dwFlags      - an oring of the following
//                CT_FIND_INCLUDE_DELETED
//
//  phCertType          - Enumeration of certificate types.
//


CERTCLIAPI
HRESULT
WINAPI
CAEnumCertTypes(
    IN  DWORD       dwFlags,
    OUT HCERTTYPE * phCertType
    );



//
// CAFindCertTypeByName
// Find a cert type given a Name.
//
// wszCertType  - Name of the cert type
//
// hCAInfo      - NULL if used within an enterprise.
//              - A handle to a CA if used to find
//              - a cert type on a standalone CA.
//              - (standalone not currently implemented)
//
// dwFlags      - an oring of the following
//                CT_FIND_INCLUDE_DELETED
//
// phCertType   - Poiter to a cert type in which result is returned.
//

CERTCLIAPI
HRESULT
WINAPI
CAFindCertTypeByName(
        IN  LPCWSTR     wszCertType,
        IN  HCAINFO     hCAInfo,
        IN  DWORD       dwFlags,
        OUT HCERTTYPE * phCertType
        );

//
// Default cert type names
//

#define wszCERTTYPE_USER                    L"User"
#define wszCERTTYPE_USER_SIGNATURE          L"UserSignature"
#define wszCERTTYPE_SMARTCARD_USER          L"SmartcardUser"
#define wszCERTTYPE_USER_AS                 L"ClientAuth"
#define wszCERTTYPE_USER_SMARTCARD_LOGON    L"SmartcardLogon"
#define wszCERTTYPE_EFS                     L"EFS"
#define wszCERTTYPE_ADMIN                   L"Administrator"
#define wszCERTTYPE_EFS_RECOVERY            L"EFSRecovery"
#define wszCERTTYPE_CODE_SIGNING            L"CodeSigning"
#define wszCERTTYPE_CTL_SIGNING             L"CTLSigning"
#define wszCERTTYPE_ENROLLMENT_AGENT        L"EnrollmentAgent"


#define wszCERTTYPE_MACHINE                 L"Machine"
#define wszCERTTYPE_DC                      L"DomainController"
#define wszCERTTYPE_WEBSERVER               L"WebServer"
#define wszCERTTYPE_KDC                     L"KDC"
#define wszCERTTYPE_CA                      L"CA"
#define wszCERTTYPE_SUBORDINATE_CA          L"SubCA"


#define wszCERTTYPE_IPSEC_ENDENTITY_ONLINE      L"IPSECEndEntityOnline"
#define wszCERTTYPE_IPSEC_ENDENTITY_OFFLINE     L"IPSECEndEntityOffline"
#define wszCERTTYPE_IPSEC_INTERMEDIATE_ONLINE   L"IPSECIntermediateOnline"
#define wszCERTTYPE_IPSEC_INTERMEDIATE_OFFLINE  L"IPSECIntermediateOffline"

#define wszCERTTYPE_ROUTER_OFFLINE              L"OfflineRouter"
#define wszCERTTYPE_ENROLLMENT_AGENT_OFFLINE    L"EnrollmentAgentOffline"
#define wszCERTTYPE_EXCHANGE_USER               L"ExchangeUser"
#define wszCERTTYPE_EXCHANGE_USER_SIGNATURE     L"ExchangeUserSignature"
#define wszCERTTYPE_MACHINE_ENROLLMENT_AGENT    L"MachineEnrollmentAgent"
#define wszCERTTYPE_CEP_ENCRYPTION              L"CEPEncryption"

//
// CAUpdateCertType
// Write any changes made to the cert type back to the type store
//
CERTCLIAPI
HRESULT
WINAPI
CAUpdateCertType(
        IN HCERTTYPE           hCertType
        );


//
// CADeleteCertType
// Delete a CertType
//
// hCertType        - Cert type to delete.
//
// NOTE:  If this is called for a default cert type,
//        it will revert back to it's default attributes
//        (if it has been modified)
//
CERTCLIAPI
HRESULT
WINAPI
CADeleteCertType(
        IN HCERTTYPE            hCertType
        );
//
// CACreateCertType
// Create a new cert type
//
// wszCertType - Name of the cert type
//
// wszScope - Unused
//
// dwFlags -      not used
//
// phCertType  - returned cert type
//
CERTCLIAPI
HRESULT
WINAPI
CACreateCertType(
        IN  LPCWSTR             wszCertType,
        IN  LPCWSTR             wszScope,
        IN  DWORD               fFlags,
        OUT HCERTTYPE *         phCertType
        );







//
// CAEnumNextCertType
// Find the Next Cert Type in an enumeration.
//
// hPrevCertType        - Previous cert type in enumeration
//
// phCertType           - Poiner to a handle into which
//                      - result is placed.  NULL if
//                      - there are no more cert types in
//                      - enumeration.
//

CERTCLIAPI
HRESULT
WINAPI
CAEnumNextCertType(
    IN  HCERTTYPE          hPrevCertType,
    OUT HCERTTYPE *        phCertType
    );


//
// CACountCertTypes
// return the number of cert types in this enumeration
//

CERTCLIAPI
DWORD
WINAPI
CACountCertTypes(
                 IN  HCERTTYPE  hCertType
                 );


//
// CACloseCertType
// Close an open CertType handle
//

CERTCLIAPI
HRESULT
WINAPI
CACloseCertType(
                IN HCERTTYPE hCertType
                );


//
// CAGetCertTypeProperty
// Retrieve a property from a certificate type.
//
// hCertType            - Handle to an open CertType object.
//
// wszPropertyName      - Name of the CertType property
//
// pawszPropertyValue   - A pointer into which an array
//                        of WCHAR strings is written, containing
//                        the values of the property.  The last
//                        element of the array points to NULL.
//                        If the property is single valued, then
//                        the array returned contains 2 elements,
//                        the first pointing to the value, the second
//                        pointing to NULL.  This pointer must be
//                        freed by CAFreeCertTypeProperty.
//
//
// Returns              - S_OK on success.
//

CERTCLIAPI
HRESULT
WINAPI
CAGetCertTypeProperty(
    IN  HCERTTYPE   hCertType,
    IN  LPCWSTR     wszPropertyName,
    OUT LPWSTR **   pawszPropertyValue);

// Common name of the certificate type
#define CERTTYPE_PROP_CN                    L"cn"

// Full Distinguished Name of the certificate type
#define CERTTYPE_PROP_DN                    L"distinguishedName"


// The display name of a cert type
#define CERTTYPE_PROP_FRIENDLY_NAME         L"displayName"

// An array of extended key usage OID's for a cert type
// NOTE: This property can also be set by setting
// the Extended Key Usage extension.
#define CERTTYPE_PROP_EXTENDED_KEY_USAGE    L"pKIExtendedKeyUsage"

// The list of default CSP's for this cert type

#define CERTTYPE_PROP_CSP_LIST              L"pKIDefaultCSPs"

//
// CASetCertTypeProperty
// Set a property of a CertType
//
// hCertType            - Handle to an open CertType object.
//
// wszPropertyName      - Name of the CertType property
//
// awszPropertyValue    - An array of values to set
//                      - for this property.  The last element of this
//                      - array should be NULL.
//                      - For single valued properties, the values beyond thie
//                      - first will be ignored upon update.
//
// Returns              - S_OK on success.
//

CERTCLIAPI
HRESULT
WINAPI
CASetCertTypeProperty(
    IN  HCERTTYPE   hCertType,
    IN  LPCWSTR     wszPropertyName,
    IN  LPWSTR *    awszPropertyValue
    );


//
// CAFreeCertTypeProperty
// Free's a previously retrieved property value.
//
// hCertType            - Handle to an open CertType object.
//
// awszPropertyValue   - The values to be freed.
//
CERTCLIAPI
HRESULT
WINAPI
CAFreeCertTypeProperty(
    IN  HCERTTYPE   hCertType,
    IN  LPWSTR *    awszPropertyValue
    );


//
// CAGetCertTypeExtensions
// Retrieves the extensions associated with this CertType.
//
// hCertType            - Handle to an open CertType object.
// ppCertExtensions     - Pointer to a PCERT_EXTENSIONS to receive
//                      - the result of this call.  Should be freed
//                      - via a CAFreeCertTypeExtensions call.
//

CERTCLIAPI
HRESULT
WINAPI
CAGetCertTypeExtensions(
    IN  HCERTTYPE           hCertType,
    OUT PCERT_EXTENSIONS *  ppCertExtensions
    );


//
// CAFreeCertTypeExtensions
// Free a PCERT_EXTENSIONS allocated by CAGetCertTypeExtensions
//
CERTCLIAPI
HRESULT
WINAPI
CAFreeCertTypeExtensions(
    IN  HCERTTYPE           hCertType,
    IN  PCERT_EXTENSIONS    pCertExtensions
    );

//
// CASetCertTypeExtension
// Set the value of an extension for this
// cert type.
//
// hCertType            - handle to the CertType
//
// wszExtensionId       - OID for the extension
//
// pExtension           - pointer to the appropriate extension structure
//
// Supported extensions/structures
//
// szOID_ENHANCED_KEY_USAGE     CERT_ENHKEY_USAGE
// szOID_KEY_USAGE              CRYPT_BIT_BLOB
// szOID_BASIC_CONSTRAINTS2     CERT_BASIC_CONSTRAINTS2_INFO
//
// Returns S_OK if successful.
//

CERTCLIAPI
HRESULT
WINAPI
CASetCertTypeExtension(
    IN HCERTTYPE   hCertType,
    IN LPCWSTR wszExtensionId,
    IN DWORD   dwFlags,
    IN LPVOID pExtension
    );

#define CA_EXT_FLAG_CRITICAL   0x00000001



//
// CAGetCertTypeFlags
// Retrieve cert type flags
//
// hCertType            - handle to the CertType
//
// pdwFlags             - pointer to DWORD receiving flags
//

CERTCLIAPI
HRESULT
WINAPI
CAGetCertTypeFlags(
    IN  HCERTTYPE           hCertType,
    OUT DWORD *             pdwFlags
    );

//
// Cert Type Flags
// The enrolling application must supply the subject name.
#define CT_FLAG_ENROLLEE_SUPPLIES_SUBJECT   0x00000001

// The e-mail name of the principal will be added to the cert
#define CT_FLAG_ADD_EMAIL                   0x00000002

// Add the object GUID for this principal
#define CT_FLAG_ADD_OBJ_GUID                0x00000004

// Publish the resultant cert to the userCertificate property
// in the DS
#define CT_FLAG_PUBLISH_TO_DS               0x00000008

// Make the key for this cert exportable.
#define CT_FLAG_EXPORTABLE_KEY              0x00000010

// This key is appropriate for auto-enrollment
#define CT_FLAG_AUTO_ENROLLMENT             0x00000020

// This is a machine cert type
#define CT_FLAG_MACHINE_TYPE                0x00000040

// This is a CA
#define CT_FLAG_IS_CA                       0x00000080

// This is a machine cert type
#define CT_FLAG_ADD_DIRECTORY_PATH          0x00000100

// This is a machine cert type
#define CT_FLAG_ADD_TEMPLATE_NAME           0x00000200

// The type is a default cert type (cannot be set)
#define CT_FLAG_IS_DEFAULT                  0x00010000

// The type has been modified, if it is default (cannot be set)
#define CT_FLAG_IS_MODIFIED                 0x00020000

// The type is deleted
#define CT_FLAG_IS_DELETED                  0x00040000

// This cert type contains a policy id that doesn't match
// that requested in the Find
#define CT_FLAG_POLICY_MISMATCH             0x00080000

#define CT_MASK_SETTABLE_FLAGS              0x0000ffff


// Old name...
#define CT_FLAG_IS_SUBJECT_REQ  CT_FLAG_ENROLLEE_SUPPLIES_SUBJECT
//
// CASetCertTypeFlags
// Sets the Flags of a cert type
//
// hCertType            - handle to the CertType
//
// dwFlags              - Flags to be set
//

CERTCLIAPI
HRESULT
WINAPI
CASetCertTypeFlags(
    IN HCERTTYPE           hCertType,
    IN DWORD               dwFlags
    );



//
// CAGetCertTypeKeySpec
// Retrieve the CAPI Key Spec for this cert type
//
// hCertType            - handle to the CertType
//
// pdwKeySpec           - pointer to DWORD receiving key spec
//

CERTCLIAPI
HRESULT
WINAPI
CAGetCertTypeKeySpec(
    IN  HCERTTYPE           hCertType,
    OUT DWORD *             pdwKeySpec
    );

//
// CACertTypeSetKeySpec
// Sets the CAPI1 Key Spec of a cert type
//
// hCertType            - handle to the CertType
//
// dwKeySpec            - KeySpec to be set
//

CERTCLIAPI
HRESULT
WINAPI
CASetCertTypeKeySpec(
    IN  HCERTTYPE           hCertType,
    IN DWORD               dwKeySpec
    );

//
// CAGetCertTypeExpiration
// Retrieve the Expiration Info for this cert type
//
// pftExpiration        - pointer to the FILETIME structure receiving
//                        the expiration period for this cert type.
//
// pftOverlap           - pointer to the FILETIME structure receiving
//                      - the suggested renewal overlap period for this
//                      - cert type.
//

CERTCLIAPI
HRESULT
WINAPI
CAGetCertTypeExpiration(
    IN  HCERTTYPE           hCertType,
    OUT OPTIONAL FILETIME * pftExpiration,
    OUT OPTIONAL FILETIME * pftOverlap
    );

//
// CASetCertTypeExpiration
// Set the Expiration Info for this cert type
//
// pftExpiration        - pointer to the FILETIME structure containing
//                        the expiration period for this cert type.
//
// pftOverlap           - pointer to the FILETIME structure containing
//                      - the suggested renewal overlap period for this
//                      - cert type.
//

CERTCLIAPI
HRESULT
WINAPI
CASetCertTypeExpiration(
    IN  HCERTTYPE           hCertType,
    IN OPTIONAL FILETIME  * pftExpiration,
    IN OPTIONAL FILETIME  * pftOverlap
    );
//
// CACertTypeSetSecurity
// Set the list of Users, Groups, and Machines allowed
// to access this cert type.
//
// hCertType            - handle to the CertType
//
// pSD                  - Security descriptor for this cert type
//

CERTCLIAPI
HRESULT
WINAPI
CACertTypeSetSecurity(
                     IN HCERTTYPE               hCertType,
                     IN PSECURITY_DESCRIPTOR    pSD
                     );


//
// CACertTypeGetSecurity
// Get the list of Users, Groups, and Machines allowed
// to access this cert type.
//
// hCertType            - handle to the CertType
//
// ppaSidList           - Pointer to a location receiving
//                      - the pointer to the security descriptor
//                      - Free via LocalFree
//

CERTCLIAPI
HRESULT
WINAPI
CACertTypeGetSecurity(
                     IN  HCERTTYPE                  hCertType,
                     OUT PSECURITY_DESCRIPTOR *     ppSD
                     );

#define CERTTYPE_ACCESS_MASK 0x00000001

//
//
// CACertTypeAccessCheck
// Determine whether the principal specified by
// ClientToken can be issued this cert type.
//
// hCertType            - handle to the CertType
//
// ClientToken          - Handle to an impersonation token
//                      - that represents the client attempting
//                      - request this cert type. The handle must
//                      - have TOKEN_QUERY access to the token;
//                      - otherwise, the function fails with
//                      - ERROR_ACCESS_DENIED.
//
// Return: S_OK on success
//

CERTCLIAPI
HRESULT
WINAPI
CACertTypeAccessCheck(
    IN HCERTTYPE    hCertType,
    IN HANDLE       ClientToken
    );




//
// CACertTypeIsSubjectRequired -- Indicates whether the
// CA will be expecting a subject name in the request.
// returns S_OK or S_FALSE;
//
//  NOTE:This function is no longer valid, use CAGetCertTypeFlags
//

CERTCLIAPI
HRESULT
WINAPI
CACertTypeIsSubjectRequired(
    IN  HCERTTYPE           hCertType
    );

#ifndef szOID_ENROLL_CERTTYPE_EXTENSION
#define szOID_CERTTYPE_EXTENSION "1.3.6.1.4.1.311.20.2"
#else
#define szOID_CERTTYPE_EXTENSION szOID_ENROLL_CERTTYPE_EXTENSION
#endif


#ifndef szOID_ALT_NAME_OBJECT_GUID
#define szOID_ALT_NAME_OBJECT_GUID "1.3.6.1.4.1.311.25.1"
#endif


#ifndef szOID_KP_SMARTCARD_LOGON
#define szOID_KP_SMARTCARD_LOGON "1.3.6.1.4.1.311.20.2.2"
#endif

#ifndef szOID_NT_PRINCIPAL_NAME
#define szOID_NT_PRINCIPAL_NAME "1.3.6.1.4.1.311.20.2.3"
#endif
//
// CAEnumGlobalProviders
// Enumerate the global list of CAPI providers.
//
// dwIndex            -  Index to the current provider being enumerates
//
// pdwFlags           -  Return location for the flags associated with this
//                       provider.  Or of the following values:
//                       none
//
// wszProviderName    - pointer to the buffer receiving the provider name
//                    - If null, then pcProviderName is set to the required length
//
// pcProviderName     - pointer to the dword receiving the length of the
//                      provider name, in chars.
//
// Return:             S_OK on success.
//                     Error ..... on dwIndex out of range.
//
//

CERTCLIAPI
HRESULT
WINAPI
CAEnumGlobalProviders(
                      IN DWORD dwIndex,
                      OUT DWORD * pdwFlags,
                      OUT LPWSTR wszProviderName,
                      OUT DWORD * pcProviderName);

//
// CAEnumGPTGlobalProviders
// Enumerate the global list of CAPI providers.
//
// hkeyGPO            - key indicating GPO root key.
//
// dwIndex            -  Index to the current provider being enumerates
//
// pdwFlags           -  Return location for the flags associated with this
//                       provider.  Or of the following values:
//                       GLOBAL_PROV_ENABLED - the provider is enabled
//
// wszProviderName    - pointer to the buffer receiving the provider name
//                    - If null, then pcProviderName is set to the required length
//
// pcProviderName     - pointer to the dword receiving the length of the
//                      provider name, in chars.
//
// Return:             S_OK on success.
//                     Error ..... on dwIndex out of range.
//
//

CERTCLIAPI
HRESULT
WINAPI
CAEnumGPTGlobalProviders(
                      IN  VOID *    pGPEInfo,
                      IN DWORD      dwIndex,
                      OUT DWORD *   pdwFlags,
                      OUT LPWSTR    wszProviderName,
                      OUT DWORD *   pcProviderName);

//
// CAAddGPTGlobalProvider
// Add a provider to the global list.  If the provider is already there,
// the simply set the flags
//
// hkeyGPO            - key indicating GPO root key.
//
// dwFlags            -  Or of the following values:
//                       none
//
// wszProviderName    - pointer to the buffer receiving the provider name
//                    - If null, then pcProviderName is set to the required length
//
// Return:             S_OK on success.
//
//

CERTCLIAPI
HRESULT
WINAPI
CAAddGPTGlobalProvider(
                      IN  VOID *    pGPEInfo,
                      IN DWORD      dwFlags,
                      IN LPCWSTR    wszProviderName);

//
// CARemoveGPTGlobalProvider
// Remove a provider from the global list
//
// hkeyGPO            - key indicating GPO root key.
//
// wszProviderName    - pointer to the buffer receiving the provider name
//                    - If null, then pcProviderName is set to the required length
//
// Return:             S_OK on success.
//
//

CERTCLIAPI
HRESULT
WINAPI
CARemoveGPTGlobalProvider(
                      IN  VOID *    pGPEInfo,
                      IN LPCWSTR    wszProviderName);



//
// CACreateLocalAutoEnrollmentObject
// Create an auto-enrollment object on the local machine.
//
// pwszCertType       - The name of the certificate type for which to create the
//                      auto-enrollment object
//
// awszCAs            - The list of CA's to add to the auto-enrollment object.
//                    - with the last entry in the list being NULL
//                    - if the list is NULL or empty, then it create an auto-enrollment
//                    - object which instructs the system to enroll for a cert at any
//                    - CA supporting the requested certificate type.
//
// pSignerInfo        - not used, must be NULL.
//
// dwFlags            - can be CERT_SYSTEM_STORE_CURRENT_USER, or CERT_SYSTEM_STORE_LOCAL_MACHINE, indicating
//                    - auto-enrollment store in which the auto-enrollment object is created.
//
// Return:             S_OK on success.
//
//

CERTCLIAPI
HRESULT
WINAPI
CACreateLocalAutoEnrollmentObject(
             IN LPCWSTR                              pwszCertType,
             IN OPTIONAL WCHAR **                    awszCAs,
             IN OPTIONAL PCMSG_SIGNED_ENCODE_INFO    pSignerInfo,
             IN DWORD                                dwFlags);

//
// CACreateAutoEnrollmentObjectEx
// Create an auto-enrollment object in the indicated store.
//
// pwszCertType       - The name of the certificate type for which to create the
//                      auto-enrollment object
//
// pwszObjectID       - An identifying string for this autoenrollment object.
//                      NULL may be passed if this object is simply to be identified by 
//                      it's certificate template.  An autoenrollment object is identified
//                      by a combination of it's object id and it's cert type name.
//
// awszCAs            - The list of CA's to add to the auto-enrollment object.
//                    - with the last entry in the list being NULL
//                    - if the list is NULL or empty, then it create an auto-enrollment
//                    - object which instructs the system to enroll for a cert at any
//                    - CA supporting the requested certificate type.
//
// pSignerInfo        - not used, must be NULL.
//
// StoreProvider      - see CertOpenStore
//
// dwFlags            - see CertOpenStore
//
// pvPara             - see CertOpenStore
//
// Return:             S_OK on success.
//
//

CERTCLIAPI
HRESULT
WINAPI
CACreateAutoEnrollmentObjectEx(
             IN LPCWSTR                     pwszCertType,
             IN LPCWSTR                     wszObjectID,
             IN WCHAR **                    awszCAs,
             IN PCMSG_SIGNED_ENCODE_INFO    pSignerInfo,
             IN LPCSTR                      StoreProvider,
             IN DWORD                       dwFlags,
             IN const void *                pvPara);


typedef struct _CERTSERVERENROLL
{
    DWORD   Disposition;
    HRESULT hrLastStatus;
    DWORD   RequestId;
    BYTE   *pbCert;
    DWORD   cbCert;
    BYTE   *pbCertChain;
    DWORD   cbCertChain;
    WCHAR  *pwszDispositionMessage;
} CERTSERVERENROLL;


// Cert Server RPC interfaces:

CERTCLIAPI
HRESULT
WINAPI
CertServerSubmitRequest(
    IN DWORD Flags,
    IN BYTE const *pbRequest,
    IN DWORD cbRequest,
    OPTIONAL IN WCHAR const *pwszRequestAttributes,
    IN WCHAR const *pwszServerName,
    IN WCHAR const *pwszAuthority,
    OUT CERTSERVERENROLL **ppcsEnroll); // free via CertServerFreeMemory

CERTCLIAPI
HRESULT
WINAPI
CertServerRetrievePending(
    IN DWORD RequestId,
    OPTIONAL IN WCHAR const *pwszSerialNumber,
    IN WCHAR const *pwszServerName,
    IN WCHAR const *pwszAuthority,
    OUT CERTSERVERENROLL **ppcsEnroll); // free via CertServerFreeMemory

CERTCLIAPI
VOID
WINAPI
CertServerFreeMemory(
    IN VOID *pv);



#ifdef __cplusplus
}
#endif
#endif //__CERTCA_H__
