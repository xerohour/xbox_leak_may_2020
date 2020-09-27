//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1995 - 1996
//
//  File:       policy.cpp
//
//  Contents:   Certificate Chain Policy APIs
//
//  Functions:  CertChainPolicyDllMain
//              CertVerifyCertificateChainPolicy
//              CertDllVerifyBaseCertificateChainPolicy
//              CertDllVerifyBasicConstraintsCertificateChainPolicy
//              CertDllVerifyAuthenticodeCertificateChainPolicy
//              CertDllVerifyAuthenticodeTimeStampCertificateChainPolicy
//              CertDllVerifySSLCertificateChainPolicy
//              CertDllVerifyNTAuthCertificateChainPolicy
//
//  History:    16-Feb-98   philh   created
//--------------------------------------------------------------------------


#include "global.hxx"
#include <dbgdef.h>
#include "wintrust.h"
#include "softpub.h"

#include "wininet.h"
#ifndef SECURITY_FLAG_IGNORE_REVOCATION
#   define SECURITY_FLAG_IGNORE_REVOCATION          0x00000080
#   define SECURITY_FLAG_IGNORE_UNKNOWN_CA          0x00000100
#endif

#ifndef SECURITY_FLAG_IGNORE_WRONG_USAGE
#   define  SECURITY_FLAG_IGNORE_WRONG_USAGE        0x00000200
#endif

#define INTERNET_FLAG_IGNORE_CERT_DATE_INVALID  0x00002000 // expired X509 Cert.
#define INTERNET_FLAG_IGNORE_CERT_CN_INVALID    0x00001000 // bad common name in X509 Cert.

#define SECURITY_FLAG_IGNORE_CERT_CN_INVALID    INTERNET_FLAG_IGNORE_CERT_CN_INVALID
#define SECURITY_FLAG_IGNORE_CERT_DATE_INVALID  INTERNET_FLAG_IGNORE_CERT_DATE_INVALID

//+-------------------------------------------------------------------------
//  Global cert policy critical section.
//--------------------------------------------------------------------------
static CRITICAL_SECTION CertPolicyCriticalSection;

//+-------------------------------------------------------------------------
//  Cached certificate store used for NTAuth certificate chain policy.
//--------------------------------------------------------------------------
static HCERTSTORE hNTAuthCertStore = NULL;

//
//  support for MS test roots!!!!
//
static BYTE rgbTestRoot[] = 
{
#include "mstest1.h"
};

static BYTE rgbTestRootCorrected[] = 
{
#include "mstest2.h"
};

static BYTE rgbTestRootBeta1[] = 
{
#include "mstestb1.h"
};

static CERT_PUBLIC_KEY_INFO rgTestRootPublicKeyInfo[] = 
{
    {szOID_RSA_RSA, 0, NULL, sizeof(rgbTestRoot), rgbTestRoot, 0},
    {szOID_RSA_RSA, 0, NULL,
        sizeof(rgbTestRootCorrected), rgbTestRootCorrected, 0},
    {szOID_RSA_RSA, 0, NULL, sizeof(rgbTestRootBeta1), rgbTestRootBeta1, 0}
};
#define NTESTROOTS (sizeof(rgTestRootPublicKeyInfo)/ \
                            sizeof(rgTestRootPublicKeyInfo[0]))

HCRYPTOIDFUNCSET hChainPolicyFuncSet;

typedef BOOL (WINAPI *PFN_CHAIN_POLICY_FUNC) (
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    );

BOOL
WINAPI
CertDllVerifyBaseCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    );

BOOL
WINAPI
CertDllVerifyAuthenticodeCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    );

BOOL
WINAPI
CertDllVerifyAuthenticodeTimeStampCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    );

BOOL
WINAPI
CertDllVerifySSLCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    );

BOOL
WINAPI
CertDllVerifyBasicConstraintsCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    );

BOOL
WINAPI
CertDllVerifyNTAuthCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    );

static const CRYPT_OID_FUNC_ENTRY ChainPolicyFuncTable[] = {
    CERT_CHAIN_POLICY_BASE, CertDllVerifyBaseCertificateChainPolicy,
    CERT_CHAIN_POLICY_AUTHENTICODE,
        CertDllVerifyAuthenticodeCertificateChainPolicy,
    CERT_CHAIN_POLICY_AUTHENTICODE_TS,
        CertDllVerifyAuthenticodeTimeStampCertificateChainPolicy,
    CERT_CHAIN_POLICY_SSL,
        CertDllVerifySSLCertificateChainPolicy,
    CERT_CHAIN_POLICY_BASIC_CONSTRAINTS,
        CertDllVerifyBasicConstraintsCertificateChainPolicy,
    CERT_CHAIN_POLICY_NT_AUTH,
        CertDllVerifyNTAuthCertificateChainPolicy,
};

#define CHAIN_POLICY_FUNC_COUNT (sizeof(ChainPolicyFuncTable) / \
                                    sizeof(ChainPolicyFuncTable[0]))


BOOL
WINAPI
CertChainPolicyDllMain(
        HMODULE hInst,
        ULONG  ulReason,
        LPVOID lpReserved)
{
    BOOL    fRet;

    switch (ulReason) {
    case DLL_PROCESS_ATTACH:
        InitializeCriticalSection(&CertPolicyCriticalSection);

        if (NULL == (hChainPolicyFuncSet = CryptInitOIDFunctionSet(
                CRYPT_OID_VERIFY_CERTIFICATE_CHAIN_POLICY_FUNC,
                0)))
            goto CryptInitOIDFunctionSetError;

        if (!CryptInstallOIDFunctionAddress(
                NULL,                       // hModule
                0,                          // dwEncodingType
                CRYPT_OID_VERIFY_CERTIFICATE_CHAIN_POLICY_FUNC,
                CHAIN_POLICY_FUNC_COUNT,
                ChainPolicyFuncTable,
                0))                         // dwFlags
            goto CryptInstallOIDFunctionAddressError;
        break;

    case DLL_PROCESS_DETACH:
        DeleteCriticalSection(&CertPolicyCriticalSection);
        if (hNTAuthCertStore)
            CertCloseStore(hNTAuthCertStore, 0);
        break;

    case DLL_THREAD_DETACH:
    default:
        break;
    }

    fRet = TRUE;
CommonReturn:
    return fRet;

ErrorReturn:
    fRet = FALSE;
    goto CommonReturn;
TRACE_ERROR(CryptInitOIDFunctionSetError)
TRACE_ERROR(CryptInstallOIDFunctionAddressError)
}


//+-------------------------------------------------------------------------
//  Lock and unlock global cert policy functions
//--------------------------------------------------------------------------
static inline void CertPolicyLock()
{
    EnterCriticalSection(&CertPolicyCriticalSection);
}
static inline void CertPolicyUnlock()
{
    LeaveCriticalSection(&CertPolicyCriticalSection);
}


//+-------------------------------------------------------------------------
//  Verify that the certificate chain satisfies the specified policy
//  requirements. If we were able to verify the chain policy, TRUE is returned
//  and the dwError field of the pPolicyStatus is updated. A dwError of 0
//  (ERROR_SUCCESS, S_OK) indicates the chain satisfies the specified policy.
//
//  If dwError applies to the entire chain context, both lChainIndex and
//  lElementIndex are set to -1. If dwError applies to a simple chain,
//  lElementIndex is set to -1 and lChainIndex is set to the index of the
//  first offending chain having the error. If dwError applies to a
//  certificate element, lChainIndex and lElementIndex are updated to 
//  index the first offending certificate having the error, where, the
//  the certificate element is at:
//      pChainContext->rgpChain[lChainIndex]->rgpElement[lElementIndex].
//
//  The dwFlags in pPolicyPara can be set to change the default policy checking
//  behaviour. In addition, policy specific parameters can be passed in
//  the pvExtraPolicyPara field of pPolicyPara.
//
//  In addition to returning dwError, in pPolicyStatus, policy OID specific
//  extra status may be returned via pvExtraPolicyStatus.
//--------------------------------------------------------------------------
BOOL
WINAPI
CertVerifyCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    )
{
    BOOL fResult;
    void *pvFuncAddr;
    HCRYPTOIDFUNCADDR hFuncAddr = NULL;

    assert(pPolicyStatus && offsetof(CERT_CHAIN_POLICY_STATUS, lElementIndex) <
            pPolicyStatus->cbSize);
    pPolicyStatus->dwError = 0;
    pPolicyStatus->lChainIndex = -1;
    pPolicyStatus->lElementIndex = -1;

    if (!CryptGetOIDFunctionAddress(
            hChainPolicyFuncSet,
            0,                      // dwEncodingType,
            pszPolicyOID,
            0,                      // dwFlags
            &pvFuncAddr,
            &hFuncAddr))
        goto GetOIDFuncAddrError;

    fResult = ((PFN_CHAIN_POLICY_FUNC) pvFuncAddr)(
        pszPolicyOID,
        pChainContext,
        pPolicyPara,
        pPolicyStatus
        );
    CryptFreeOIDFunctionAddress(hFuncAddr, 0);
CommonReturn:
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
TRACE_ERROR(GetOIDFuncAddrError)
}


static inline PCERT_CHAIN_ELEMENT GetRootChainElement(
    IN PCCERT_CHAIN_CONTEXT pChainContext
    )
{
    DWORD dwRootChainIndex = pChainContext->cChain - 1;
    DWORD dwRootElementIndex =
        pChainContext->rgpChain[dwRootChainIndex]->cElement - 1;

    return pChainContext->rgpChain[dwRootChainIndex]->
                                        rgpElement[dwRootElementIndex];
}

void GetElementIndexOfFirstError(
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN DWORD dwErrorStatusMask,
    OUT LONG *plChainIndex,
    OUT LONG *plElementIndex
    )
{
    DWORD i;
    for (i = 0; i < pChainContext->cChain; i++) {
        DWORD j;
        PCERT_SIMPLE_CHAIN pChain = pChainContext->rgpChain[i];

        for (j = 0; j < pChain->cElement; j++) {
            PCERT_CHAIN_ELEMENT pEle = pChain->rgpElement[j];

            if (pEle->TrustStatus.dwErrorStatus & dwErrorStatusMask) {
                *plChainIndex = (LONG) i;
                *plElementIndex = (LONG) j;
                return;
            }
        }
    }

    *plChainIndex = -1;
    *plElementIndex = -1;
}

void GetChainIndexOfFirstError(
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN DWORD dwErrorStatusMask,
    OUT LONG *plChainIndex
    )
{
    DWORD i;
    for (i = 0; i < pChainContext->cChain; i++) {
        PCERT_SIMPLE_CHAIN pChain = pChainContext->rgpChain[i];

        if (pChain->TrustStatus.dwErrorStatus & dwErrorStatusMask) {
            *plChainIndex = (LONG) i;
            return;
        }
    }

    *plChainIndex = -1;
}


//+=========================================================================
//  CertDllVerifyBaseCertificateChainPolicy Functions
//==========================================================================

BOOL
WINAPI
CertDllVerifyBaseCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    )
{
    DWORD dwError;
    DWORD dwFlags;
    DWORD dwContextError;
    LONG lChainIndex = -1;
    LONG lElementIndex = -1;
    DWORD dwErrorStatusMask;

    dwContextError = pChainContext->TrustStatus.dwErrorStatus;


    if (0 == dwContextError) {
        // Valid chain
        dwError = 0;
        goto CommonReturn;
    }

    if (pPolicyPara && offsetof(CERT_CHAIN_POLICY_PARA, dwFlags) <
            pPolicyPara->cbSize)
        dwFlags = pPolicyPara->dwFlags;
    else
        dwFlags = 0;

    if (dwContextError &
            (CERT_TRUST_IS_NOT_SIGNATURE_VALID |
                CERT_TRUST_CTL_IS_NOT_SIGNATURE_VALID)) {
        dwError = TRUST_E_CERT_SIGNATURE;
        dwErrorStatusMask =
            CERT_TRUST_IS_NOT_SIGNATURE_VALID |
                CERT_TRUST_CTL_IS_NOT_SIGNATURE_VALID;
        if (dwErrorStatusMask & CERT_TRUST_IS_NOT_SIGNATURE_VALID)
            goto GetElementIndexReturn;
        else
            goto GetChainIndexReturn;
    } 

    if (dwContextError & CERT_TRUST_IS_UNTRUSTED_ROOT) {
        dwErrorStatusMask = CERT_TRUST_IS_UNTRUSTED_ROOT;
        if (dwFlags & CERT_CHAIN_POLICY_ALLOW_UNKNOWN_CA_FLAG) {
            ;
        } else if (0 == (dwFlags & CERT_CHAIN_POLICY_ALLOW_TESTROOT_FLAG)) {
            dwError = CERT_E_UNTRUSTEDROOT;
            goto GetElementIndexReturn;
        } else {
            // Check if one of the "test" roots
            DWORD i;
            BOOL fTestRoot;
            PCERT_CHAIN_ELEMENT pRootElement;
            PCCERT_CONTEXT pRootCert;

            pRootElement = GetRootChainElement(pChainContext);
            assert(pRootElement->TrustStatus.dwInfoStatus &
                CERT_TRUST_IS_SELF_SIGNED);
            pRootCert = pRootElement->pCertContext;

            fTestRoot = FALSE;
            for (i = 0; i < NTESTROOTS; i++) {
                if (CertComparePublicKeyInfo(
                        pRootCert->dwCertEncodingType,
                        &pRootCert->pCertInfo->SubjectPublicKeyInfo,
                        &rgTestRootPublicKeyInfo[i])) {
                    fTestRoot = TRUE;
                    break;
                }
            }
            if (fTestRoot) {
                if (0 == (dwFlags & CERT_CHAIN_POLICY_TRUST_TESTROOT_FLAG)) {
                    dwError = CERT_E_UNTRUSTEDTESTROOT;
                    goto GetElementIndexReturn;
                }
            } else {
                dwError = CERT_E_UNTRUSTEDROOT;
                goto GetElementIndexReturn;
            }
        }
    }

    if (dwContextError & CERT_TRUST_IS_PARTIAL_CHAIN) {
        if (0 == (dwFlags & CERT_CHAIN_POLICY_ALLOW_UNKNOWN_CA_FLAG)) {
            dwError = CERT_E_CHAINING;
            dwErrorStatusMask = CERT_TRUST_IS_PARTIAL_CHAIN;
            goto GetChainIndexReturn;
        }
    }

    if (dwContextError & CERT_TRUST_IS_REVOKED) {
        dwError = CRYPT_E_REVOKED;
        dwErrorStatusMask = CERT_TRUST_IS_REVOKED;
        goto GetElementIndexReturn;
    }

    if (dwContextError & (CERT_TRUST_IS_NOT_VALID_FOR_USAGE |
            CERT_TRUST_CTL_IS_NOT_VALID_FOR_USAGE)) {
        if (0 == (dwFlags & CERT_CHAIN_POLICY_IGNORE_WRONG_USAGE_FLAG)) {
            dwError = CERT_E_WRONG_USAGE;
            dwErrorStatusMask = CERT_TRUST_IS_NOT_VALID_FOR_USAGE |
                CERT_TRUST_CTL_IS_NOT_VALID_FOR_USAGE;
            if (dwContextError & CERT_TRUST_IS_NOT_VALID_FOR_USAGE)
                goto GetElementIndexReturn;
            else
                goto GetChainIndexReturn;
        }
    }

    if (dwContextError & CERT_TRUST_IS_NOT_TIME_VALID) {
        if (0 == (dwFlags & CERT_CHAIN_POLICY_IGNORE_NOT_TIME_VALID_FLAG)) {
            dwError = CERT_E_EXPIRED;
            dwErrorStatusMask = CERT_TRUST_IS_NOT_TIME_VALID;
            goto GetElementIndexReturn;
        }
    }

    if (dwContextError & CERT_TRUST_CTL_IS_NOT_TIME_VALID) {
        if (0 == (dwFlags & CERT_CHAIN_POLICY_IGNORE_CTL_NOT_TIME_VALID_FLAG)) {
            dwErrorStatusMask = CERT_TRUST_CTL_IS_NOT_TIME_VALID;
            dwError = CERT_E_EXPIRED;
            goto GetChainIndexReturn;
        }
    }

    if (dwContextError & CERT_TRUST_IS_NOT_TIME_NESTED) {
        if (0 == (dwFlags & CERT_CHAIN_POLICY_IGNORE_NOT_TIME_NESTED_FLAG)) {
            dwErrorStatusMask = CERT_TRUST_IS_NOT_TIME_NESTED;
            dwError = CERT_E_VALIDITYPERIODNESTING;
            goto GetElementIndexReturn;
        }
    }

    if (dwContextError & CERT_TRUST_REVOCATION_STATUS_UNKNOWN) {
        if ((dwFlags & CERT_CHAIN_POLICY_IGNORE_ALL_REV_UNKNOWN_FLAGS) !=
                CERT_CHAIN_POLICY_IGNORE_ALL_REV_UNKNOWN_FLAGS) {
            DWORD i;
            for (i = 0; i < pChainContext->cChain; i++) {
                DWORD j;
                PCERT_SIMPLE_CHAIN pChain = pChainContext->rgpChain[i];

                for (j = 0; j < pChain->cElement; j++) {
                    PCERT_CHAIN_ELEMENT pEle = pChain->rgpElement[j];
                    DWORD dwEleError = pEle->TrustStatus.dwErrorStatus;
                    DWORD dwEleInfo = pEle->TrustStatus.dwInfoStatus;

                    if (0 == (dwEleError &
                            CERT_TRUST_REVOCATION_STATUS_UNKNOWN))
                        continue;

                    assert(pEle->pRevocationInfo);
                    dwError = pEle->pRevocationInfo->dwRevocationResult;
                    if (CRYPT_E_NO_REVOCATION_CHECK != dwError)
                        dwError = CRYPT_E_REVOCATION_OFFLINE;

                    if (dwEleInfo & CERT_TRUST_IS_SELF_SIGNED) {
                        // Chain Root
                        if (0 == (dwFlags &
                                CERT_CHAIN_POLICY_IGNORE_ROOT_REV_UNKNOWN_FLAG)) {
                            lChainIndex = (LONG) i;
                            lElementIndex = (LONG) j;
                            goto CommonReturn;
                        }
                    } else if (0 == i && 0 == j) {
                        // End certificate
                        if (0 == (dwFlags &
                                CERT_CHAIN_POLICY_IGNORE_END_REV_UNKNOWN_FLAG)) {
                            lChainIndex = (LONG) i;
                            lElementIndex = (LONG) j;
                            goto CommonReturn;
                        }
                    } else if (0 == j) {
                        // CTL signer certificate
                        if (0 ==
                                (dwFlags & CERT_CHAIN_POLICY_IGNORE_CTL_SIGNER_REV_UNKNOWN_FLAG)) {
                            lChainIndex = (LONG) i;
                            lElementIndex = (LONG) j;
                            goto CommonReturn;
                        }
                    } else  {
                        // CA certificate
                        if (0 ==
                                (dwFlags & CERT_CHAIN_POLICY_IGNORE_CA_REV_UNKNOWN_FLAG)) {
                            lChainIndex = (LONG) i;
                            lElementIndex = (LONG) j;
                            goto CommonReturn;
                        }
                    }
                }
            }
        }
    }

    dwError = 0;

CommonReturn:
    assert(pPolicyStatus && offsetof(CERT_CHAIN_POLICY_STATUS, lElementIndex) <
            pPolicyStatus->cbSize);

    pPolicyStatus->dwError = dwError;
    pPolicyStatus->lChainIndex = lChainIndex;
    pPolicyStatus->lElementIndex = lElementIndex;
    return TRUE;

GetElementIndexReturn:
    GetElementIndexOfFirstError(pChainContext, dwErrorStatusMask,
        &lChainIndex, &lElementIndex);
    goto CommonReturn;

GetChainIndexReturn:
    GetChainIndexOfFirstError(pChainContext, dwErrorStatusMask,
        &lChainIndex);
    goto CommonReturn;
}

//+=========================================================================
//  CertDllVerifyBasicConstraintsCertificateChainPolicy Functions
//==========================================================================

// If dwFlags is 0, allow either CA or END_ENTITY for dwEleIndex == 0
BOOL CheckChainElementBasicConstraints(
    IN PCERT_CHAIN_ELEMENT pEle,
    IN DWORD dwEleIndex,
    IN DWORD dwFlags
    )
{
    BOOL fResult;

    PCERT_INFO pCertInfo = pEle->pCertContext->pCertInfo;
    PCERT_EXTENSION pExt;
    PCERT_BASIC_CONSTRAINTS_INFO pInfo = NULL;
    PCERT_BASIC_CONSTRAINTS2_INFO pInfo2 = NULL;
    DWORD cbInfo;

    BOOL fCA;
    BOOL fEndEntity;
    BOOL fPathLenConstraint;
    DWORD dwPathLenConstraint;

    if (0 == pCertInfo->cExtension) 
        goto SuccessReturn;

    if (pExt = CertFindExtension(
            szOID_BASIC_CONSTRAINTS2,
            pCertInfo->cExtension,
            pCertInfo->rgExtension
            )) {
        if (!CryptDecodeObjectEx(
                pEle->pCertContext->dwCertEncodingType,
                X509_BASIC_CONSTRAINTS2, 
                pExt->Value.pbData,
                pExt->Value.cbData,
                CRYPT_DECODE_NOCOPY_FLAG | CRYPT_DECODE_ALLOC_FLAG |
                    CRYPT_DECODE_SHARE_OID_STRING_FLAG,
                &PkiDecodePara,
                (void *) &pInfo2,
                &cbInfo
                )) {
            if (pExt->fCritical) 
                goto DecodeError;
            else
                goto SuccessReturn;
        }
        fCA = pInfo2->fCA;
        fEndEntity = !fCA;
        fPathLenConstraint = pInfo2->fPathLenConstraint;
        dwPathLenConstraint = pInfo2->dwPathLenConstraint;
    } else if (pExt = CertFindExtension(
            szOID_BASIC_CONSTRAINTS,
            pCertInfo->cExtension,
            pCertInfo->rgExtension
            )) {
        if (!CryptDecodeObjectEx(
                pEle->pCertContext->dwCertEncodingType,
                X509_BASIC_CONSTRAINTS, 
                pExt->Value.pbData,
                pExt->Value.cbData,
                CRYPT_DECODE_NOCOPY_FLAG | CRYPT_DECODE_ALLOC_FLAG |
                    CRYPT_DECODE_SHARE_OID_STRING_FLAG,
                &PkiDecodePara,
                (void *) &pInfo,
                &cbInfo
                )) {
            if (pExt->fCritical) 
                goto DecodeError;
            else
                goto SuccessReturn;
        }
        if (pExt->fCritical && pInfo->cSubtreesConstraint)
            goto SubtreesError;

        if (pInfo->SubjectType.cbData > 0) {
            BYTE bRole = pInfo->SubjectType.pbData[0];
            fCA = (0 != (bRole & CERT_CA_SUBJECT_FLAG));
            fEndEntity = (0 != (bRole & CERT_END_ENTITY_SUBJECT_FLAG));
        } else {
            fCA = FALSE;
            fEndEntity = FALSE;
        }
        fPathLenConstraint = pInfo->fPathLenConstraint;
        dwPathLenConstraint = pInfo->dwPathLenConstraint;
    } else
        goto SuccessReturn;


    if (0 == dwEleIndex) {
        if (dwFlags & BASIC_CONSTRAINTS_CERT_CHAIN_POLICY_END_ENTITY_FLAG) {
            if (!fEndEntity)
                goto NotAnEndEntity;
        }
        if (dwFlags & BASIC_CONSTRAINTS_CERT_CHAIN_POLICY_CA_FLAG) {
            if (!fCA)
                goto NotACA;
        }
    } else {
        if (!fCA)
            goto NotACA;

        if (fPathLenConstraint) {
            // Check count of CAs below
            if (dwEleIndex - 1 > dwPathLenConstraint)
                goto PathLengthError;
        }
    }
    
SuccessReturn:
    fResult = TRUE;
CommonReturn:
    PkiFree(pInfo);
    PkiFree(pInfo2);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
 
TRACE_ERROR(NotACA)
TRACE_ERROR(NotAnEndEntity)
TRACE_ERROR(SubtreesError)
TRACE_ERROR(PathLengthError)
TRACE_ERROR(DecodeError)
}

BOOL
WINAPI
CertDllVerifyBasicConstraintsCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    )
{
    DWORD dwFlags;

    if (pPolicyPara && offsetof(CERT_CHAIN_POLICY_PARA, dwFlags) <
            pPolicyPara->cbSize) {
        dwFlags = pPolicyPara->dwFlags;
        dwFlags &= (BASIC_CONSTRAINTS_CERT_CHAIN_POLICY_CA_FLAG |
            BASIC_CONSTRAINTS_CERT_CHAIN_POLICY_END_ENTITY_FLAG);
        if (dwFlags == (BASIC_CONSTRAINTS_CERT_CHAIN_POLICY_CA_FLAG |
                BASIC_CONSTRAINTS_CERT_CHAIN_POLICY_END_ENTITY_FLAG))
            dwFlags = 0;    // 0 => allow CA or END_ENTITY
    } else
        dwFlags = 0;

    DWORD i;
    for (i = 0; i < pChainContext->cChain; i++) {
        DWORD j;
        PCERT_SIMPLE_CHAIN pChain = pChainContext->rgpChain[i];

        for (j = 0; j < pChain->cElement; j++) {
            if (!CheckChainElementBasicConstraints(pChain->rgpElement[j], j,
                    dwFlags)) {
                assert(pPolicyStatus && offsetof(CERT_CHAIN_POLICY_STATUS,
                    lElementIndex) < pPolicyStatus->cbSize);
                pPolicyStatus->dwError = TRUST_E_BASIC_CONSTRAINTS;
                pPolicyStatus->lChainIndex = (LONG) i;
                pPolicyStatus->lElementIndex = (LONG) j;
                return TRUE;
            }
        }
        // Allow CTL to be signed by either a CA or END_ENTITY
        dwFlags = 0;
    }

    assert(pPolicyStatus && offsetof(CERT_CHAIN_POLICY_STATUS, lElementIndex) <
            pPolicyStatus->cbSize);
    pPolicyStatus->dwError = 0;
    pPolicyStatus->lChainIndex = -1;
    pPolicyStatus->lElementIndex = -1;
    return TRUE;
}

//+=========================================================================
//  CertDllVerifyAuthenticodeCertificateChainPolicy Functions
//==========================================================================

BOOL VerifyAuthenticodeSignerStatement(
    IN PCMSG_SIGNER_INFO pSignerInfo,
    OUT BOOL *pfCommercial
    )
{
    BOOL fResult;
    BOOL fIndividual;
    PCRYPT_ATTRIBUTE pAttr;             // not allocated
    PSPC_STATEMENT_TYPE pInfo = NULL;
    DWORD cbInfo;
    DWORD cKeyPurposeId;
    LPSTR *ppszKeyPurposeId;            // not allocated

    *pfCommercial = FALSE;
    if (NULL == pSignerInfo)
        return TRUE;

    if (0 == pSignerInfo->AuthAttrs.cAttr)
        goto NoAttributes;

    if (NULL == (pAttr = CertFindAttribute(
            SPC_STATEMENT_TYPE_OBJID,
            pSignerInfo->AuthAttrs.cAttr,
            pSignerInfo->AuthAttrs.rgAttr
            )) || 0 == pAttr->cValue)
        goto NoStatementType;

    if (!CryptDecodeObjectEx(
            X509_ASN_ENCODING,
            SPC_STATEMENT_TYPE_STRUCT,
            pAttr->rgValue[0].pbData,
            pAttr->rgValue[0].cbData,
            CRYPT_DECODE_NOCOPY_FLAG | CRYPT_DECODE_ALLOC_FLAG |
                CRYPT_DECODE_SHARE_OID_STRING_FLAG,
            &PkiDecodePara,
            (void *) &pInfo,
            &cbInfo
            ))
        goto DecodeError;

    fIndividual = FALSE;
    cKeyPurposeId = pInfo->cKeyPurposeId;
    ppszKeyPurposeId = pInfo->rgpszKeyPurposeId;
    for ( ; cKeyPurposeId > 0; cKeyPurposeId--, ppszKeyPurposeId++) {
        if (0 == strcmp(*ppszKeyPurposeId,
                SPC_COMMERCIAL_SP_KEY_PURPOSE_OBJID))
            *pfCommercial  = TRUE;
        else if (0 == strcmp(*ppszKeyPurposeId,
                 SPC_INDIVIDUAL_SP_KEY_PURPOSE_OBJID))
            fIndividual = TRUE;
    }
    
    if (!fIndividual && !*pfCommercial)
        goto NoPurpose;

    fResult = TRUE;
CommonReturn:
    PkiFree(pInfo);
    return fResult;

ErrorReturn:
// On 12-Jun-1998 :: changed the authenticode policy to no longer require the
// StatementType attribute

    fResult = TRUE;
    goto CommonReturn;
 
TRACE_ERROR(NoAttributes);    
TRACE_ERROR(NoStatementType);
TRACE_ERROR(NoPurpose);
TRACE_ERROR(DecodeError);
}

#define CERT_PURPOSE_IS_SIGNER_FLAG         0x1
#define CERT_PURPOSE_IS_END_CERT_FLAG       0x2
#define CERT_PURPOSE_GET_COMMERCIAL_FLAG    0x4


BOOL CheckAuthenticodeEnhancedKeyUsage(
    IN PCCERT_CONTEXT pCert,
    IN DWORD dwPurposeFlags,
    IN OUT OPTIONAL BOOL *pfCommercial
    )
{
    BOOL fResult;
    BOOL fCommercialMsg = *pfCommercial;
    PCERT_INFO pCertInfo = pCert->pCertInfo;
    PCERT_EXTENSION pExt;
    PCERT_ENHKEY_USAGE pInfo = NULL;
    DWORD cbInfo;

    BOOL fCodeSigning;
    BOOL fCommercial;
    BOOL fIndividual;
    DWORD cUsage;
    LPSTR *ppszUsage;

    if (0 == pCertInfo->cExtension)
        goto SuccessReturn;
    
    if (NULL == (pExt = CertFindExtension(
            szOID_ENHANCED_KEY_USAGE,
            pCertInfo->cExtension,
            pCertInfo->rgExtension
            )))
        goto SuccessReturn;

    if (!CryptDecodeObjectEx(
            pCert->dwCertEncodingType,
            X509_ENHANCED_KEY_USAGE,
            pExt->Value.pbData,
            pExt->Value.cbData,
            CRYPT_DECODE_NOCOPY_FLAG | CRYPT_DECODE_ALLOC_FLAG |
                CRYPT_DECODE_SHARE_OID_STRING_FLAG,
            &PkiDecodePara,
            (void *) &pInfo,
            &cbInfo
            ))
        goto DecodeError;


    fCodeSigning = FALSE;
    fCommercial = FALSE;
    fIndividual = FALSE;
    cUsage = pInfo->cUsageIdentifier;
    ppszUsage = pInfo->rgpszUsageIdentifier;
    for ( ; cUsage > 0; cUsage--, ppszUsage++) {
        if (0 == strcmp(*ppszUsage, szOID_PKIX_KP_CODE_SIGNING)) {
            fCodeSigning = TRUE;
        } else if (0 == strcmp(*ppszUsage,
                SPC_COMMERCIAL_SP_KEY_PURPOSE_OBJID)) {
            fCodeSigning = TRUE;
            fCommercial = TRUE;
        } else if (0 == strcmp(*ppszUsage,
                SPC_INDIVIDUAL_SP_KEY_PURPOSE_OBJID)) {
            fCodeSigning = TRUE;
            fIndividual = TRUE;
        }
    }

    if (!fCodeSigning) {
        if (dwPurposeFlags & CERT_PURPOSE_IS_SIGNER_FLAG)
            goto KeyPurposeError;
    } else {
        if (dwPurposeFlags & CERT_PURPOSE_GET_COMMERCIAL_FLAG) {
            assert(pfCommercial);
            *pfCommercial = fCommercial;
        } else if (fCommercial && fIndividual)
            ; // It supports both
        else if (!fIndividual && !fCommercial &&
                0 == (dwPurposeFlags & CERT_PURPOSE_IS_END_CERT_FLAG))
            ; // CA supports CodeSigning EKU
        else if ((fCommercialMsg && !fCommercial) ||
                 (!fCommercialMsg && fCommercial))
            goto KeyPurposeError;
    }
    
SuccessReturn:
    fResult = TRUE;
CommonReturn:
    PkiFree(pInfo);
    return fResult;

ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
 
TRACE_ERROR(KeyPurposeError);
TRACE_ERROR(DecodeError);
}

BOOL CheckAuthenticodeCertPurpose(
    IN PCCERT_CONTEXT pCert,
    IN DWORD dwPurposeFlags,
    IN OUT OPTIONAL BOOL *pfCommercial
    )
{
    BOOL fResult;
    PCERT_INFO pCertInfo = pCert->pCertInfo;
    PCERT_EXTENSION pExt;
    PCERT_KEY_USAGE_RESTRICTION_INFO pInfo = NULL;
    DWORD cbInfo;

    BOOL fCommercialMsg = *pfCommercial;

    if (0 == pCertInfo->cExtension)
        goto SuccessReturn;
    
    if (NULL == (pExt = CertFindExtension(
            szOID_KEY_USAGE_RESTRICTION,
            pCertInfo->cExtension,
            pCertInfo->rgExtension
            )))
        return CheckAuthenticodeEnhancedKeyUsage(
            pCert,
            dwPurposeFlags,
            pfCommercial
            );

    if (!CryptDecodeObjectEx(
            pCert->dwCertEncodingType,
            X509_KEY_USAGE_RESTRICTION,
            pExt->Value.pbData,
            pExt->Value.cbData,
            CRYPT_DECODE_NOCOPY_FLAG | CRYPT_DECODE_ALLOC_FLAG |
                CRYPT_DECODE_SHARE_OID_STRING_FLAG,
            &PkiDecodePara,
            (void *) &pInfo,
            &cbInfo
            ))
        goto DecodeError;

    if (pInfo->cCertPolicyId) {
        BYTE bKeyUsage;
        BOOL fCommercial;
        BOOL fIndividual;
        DWORD cPolicyId;
        PCERT_POLICY_ID pPolicyId;

        fCommercial = FALSE;
        fIndividual = FALSE;
        cPolicyId = pInfo->cCertPolicyId;
        pPolicyId = pInfo->rgCertPolicyId;
        for ( ; cPolicyId > 0; cPolicyId--, pPolicyId++) {
            DWORD cElementId = pPolicyId->cCertPolicyElementId;
            LPSTR *ppszElementId = pPolicyId->rgpszCertPolicyElementId;

            for ( ; cElementId > 0; cElementId--, ppszElementId++) 
            {
                if (0 == strcmp(*ppszElementId,
                        SPC_COMMERCIAL_SP_KEY_PURPOSE_OBJID))
                    fCommercial = TRUE;
                else if (0 == strcmp(*ppszElementId,
                        SPC_INDIVIDUAL_SP_KEY_PURPOSE_OBJID))
                    fIndividual = TRUE;
            }
        }

        if (!fCommercial && !fIndividual &&
                (dwPurposeFlags & CERT_PURPOSE_IS_SIGNER_FLAG))
            goto KeyPurposeError;

        if (dwPurposeFlags & CERT_PURPOSE_GET_COMMERCIAL_FLAG) {
            assert(pfCommercial);
            *pfCommercial = fCommercial;
        } else if (fCommercial && fIndividual)
           ; // place holder -- it supports both (??? -- test cert only)
        else if ((fCommercialMsg && !fCommercial) ||
                 (!fCommercialMsg && fCommercial))
            goto KeyPurposeError;

        if (pInfo->RestrictedKeyUsage.cbData) {
            bKeyUsage = pInfo->RestrictedKeyUsage.pbData[0];
            
            if (0 == (bKeyUsage &
                    (CERT_DIGITAL_SIGNATURE_KEY_USAGE |
                        CERT_KEY_CERT_SIGN_KEY_USAGE))) {
                if (fCommercial || fIndividual)
                    goto KeyUsageError;
            }
        }
    }
    
SuccessReturn:
    fResult = TRUE;
CommonReturn:
    PkiFree(pInfo);
    return fResult;

ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
 
TRACE_ERROR(KeyPurposeError);
TRACE_ERROR(KeyUsageError);
TRACE_ERROR(DecodeError);
}



void MapAuthenticodeRegPolicySettingsToBaseChainPolicyFlags(
    IN DWORD dwRegPolicySettings,
    IN BOOL fCommercial,
    IN OUT DWORD *pdwFlags
    )
{
    DWORD dwFlags;

    if (0 == dwRegPolicySettings)
        return;

    dwFlags = *pdwFlags;
    if (dwRegPolicySettings & WTPF_TRUSTTEST)
        dwFlags |= CERT_CHAIN_POLICY_TRUST_TESTROOT_FLAG;
    if (dwRegPolicySettings & WTPF_TESTCANBEVALID)
        dwFlags |= CERT_CHAIN_POLICY_ALLOW_TESTROOT_FLAG;
    if (dwRegPolicySettings & WTPF_IGNOREEXPIRATION)
        dwFlags |= CERT_CHAIN_POLICY_IGNORE_ALL_NOT_TIME_VALID_FLAGS;

    if (dwRegPolicySettings & WTPF_IGNOREREVOKATION)
        dwFlags |= CERT_CHAIN_POLICY_IGNORE_ALL_REV_UNKNOWN_FLAGS;
    else {
        if (fCommercial) {
            if (dwRegPolicySettings & WTPF_OFFLINEOK_COM)
                dwFlags |= CERT_CHAIN_POLICY_IGNORE_END_REV_UNKNOWN_FLAG;
        } else {
            if (dwRegPolicySettings & WTPF_OFFLINEOK_IND)
                dwFlags |= CERT_CHAIN_POLICY_IGNORE_END_REV_UNKNOWN_FLAG;
        }
    }

    *pdwFlags = dwFlags;
}


void GetAuthenticodePara(
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    OUT DWORD *pdwRegPolicySettings,
    OUT PCMSG_SIGNER_INFO *ppSignerInfo
    )
{
    *ppSignerInfo = NULL;
    *pdwRegPolicySettings = 0;
    if (pPolicyPara && offsetof(CERT_CHAIN_POLICY_PARA, pvExtraPolicyPara) <
            pPolicyPara->cbSize && pPolicyPara->pvExtraPolicyPara) {
        PAUTHENTICODE_EXTRA_CERT_CHAIN_POLICY_PARA pAuthPara =
            (PAUTHENTICODE_EXTRA_CERT_CHAIN_POLICY_PARA)
                pPolicyPara->pvExtraPolicyPara;

        if (offsetof(AUTHENTICODE_EXTRA_CERT_CHAIN_POLICY_PARA,
                dwRegPolicySettings) < pAuthPara->cbSize)
            *pdwRegPolicySettings = pAuthPara->dwRegPolicySettings;
        if (offsetof(AUTHENTICODE_EXTRA_CERT_CHAIN_POLICY_PARA,
                pSignerInfo) < pAuthPara->cbSize)
            *ppSignerInfo = pAuthPara->pSignerInfo;
    }
}

// Map the CRYPT_E_ revocation errors to the corresponding CERT_E_
// revocation errors
static DWORD MapToAuthenticodeError(
    IN DWORD dwError
    )
{
    switch (dwError) {
        case CRYPT_E_REVOKED:
            return CERT_E_REVOKED;
            break;
        case CRYPT_E_NO_REVOCATION_DLL:
        case CRYPT_E_NO_REVOCATION_CHECK:
        case CRYPT_E_REVOCATION_OFFLINE:
        case CRYPT_E_NOT_IN_REVOCATION_DATABASE:
            return CERT_E_REVOCATION_FAILURE;
            break;
    }
    return dwError;
}

BOOL
WINAPI
CertDllVerifyAuthenticodeCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    )
{
    DWORD dwError;
    DWORD dwFlags;
    DWORD dwRegPolicySettings;
    PCMSG_SIGNER_INFO pSignerInfo;
    BOOL fAuthSignerStatement;
    BOOL fAuthCertPurpose;
    DWORD dwPurposeFlags;
    BOOL fCommercial;
    LONG lChainIndex = -1;
    LONG lElementIndex = -1;


    CERT_CHAIN_POLICY_PARA BasePolicyPara;
    memset(&BasePolicyPara, 0, sizeof(BasePolicyPara));
    BasePolicyPara.cbSize = sizeof(BasePolicyPara);

    CERT_CHAIN_POLICY_STATUS BasePolicyStatus;
    memset(&BasePolicyStatus, 0, sizeof(BasePolicyStatus));
    BasePolicyStatus.cbSize = sizeof(BasePolicyStatus);

    if (pPolicyPara && offsetof(CERT_CHAIN_POLICY_PARA, dwFlags) <
            pPolicyPara->cbSize)
        dwFlags = pPolicyPara->dwFlags;
    else
        dwFlags = 0;
    GetAuthenticodePara(pPolicyPara, &dwRegPolicySettings, &pSignerInfo);

    if (pSignerInfo) {
        fAuthSignerStatement = VerifyAuthenticodeSignerStatement(
            pSignerInfo, &fCommercial);

        dwPurposeFlags = CERT_PURPOSE_IS_SIGNER_FLAG |
            CERT_PURPOSE_IS_END_CERT_FLAG;
    } else {
        fAuthSignerStatement = TRUE;
        fCommercial = FALSE;
        dwPurposeFlags = CERT_PURPOSE_IS_END_CERT_FLAG |
            CERT_PURPOSE_GET_COMMERCIAL_FLAG;
    }

    // If no signer info, get fCommercial from the end certificate
    // itself.
    fAuthCertPurpose = CheckAuthenticodeCertPurpose(
        pChainContext->rgpChain[0]->rgpElement[0]->pCertContext,
        dwPurposeFlags,
        &fCommercial
        );

    dwPurposeFlags &= ~(CERT_PURPOSE_IS_END_CERT_FLAG |
        CERT_PURPOSE_GET_COMMERCIAL_FLAG);

    MapAuthenticodeRegPolicySettingsToBaseChainPolicyFlags(
        dwRegPolicySettings, fCommercial, &dwFlags);

    // Do the basic chain policy verification. Authenticode overrides
    // the defaults for the following:
    dwFlags |=
        CERT_CHAIN_POLICY_ALLOW_TESTROOT_FLAG |
        CERT_CHAIN_POLICY_IGNORE_CTL_SIGNER_REV_UNKNOWN_FLAG |
        CERT_CHAIN_POLICY_IGNORE_CA_REV_UNKNOWN_FLAG |
        CERT_CHAIN_POLICY_IGNORE_ROOT_REV_UNKNOWN_FLAG |
        CERT_CHAIN_POLICY_IGNORE_NOT_TIME_NESTED_FLAG;

    BasePolicyPara.dwFlags = dwFlags;
    if (!CertVerifyCertificateChainPolicy(
            CERT_CHAIN_POLICY_BASE,
            pChainContext,
            &BasePolicyPara,
            &BasePolicyStatus
            ))
        return FALSE;
    if (dwError = BasePolicyStatus.dwError) {
        dwError = MapToAuthenticodeError(dwError);
        lChainIndex = BasePolicyStatus.lChainIndex;
        lElementIndex = BasePolicyStatus.lElementIndex;
        goto CommonReturn;
    }
        
    if (!fAuthSignerStatement || !fAuthCertPurpose) {
        // Note, any errors detected while doing basic chain policy
        // verification take precedence over a bad signer statement.
        dwError = CERT_E_PURPOSE;
        lChainIndex = 0;
        lElementIndex = 0;
        goto CommonReturn;
    }

    {
        // Check that all certs in the first chain have the appropriate
        // Commercial or Individual publisher purpose.
        //
        // For no signer, the first certificate determines if it has
        // commercial purpose

        DWORD i;
        PCERT_SIMPLE_CHAIN pChain = pChainContext->rgpChain[0];

        for (i = 1; i < pChain->cElement; i++) {
            PCERT_CHAIN_ELEMENT pEle = pChain->rgpElement[i];

            if (!CheckAuthenticodeCertPurpose(
                    pEle->pCertContext,
                    dwPurposeFlags,
                    &fCommercial
                    )) {
                dwError = CERT_E_PURPOSE;
                lChainIndex = 0;
                lElementIndex = (LONG) i;
                goto CommonReturn;
            }
        }
    }

    if (pSignerInfo)
        BasePolicyPara.dwFlags =
            BASIC_CONSTRAINTS_CERT_CHAIN_POLICY_END_ENTITY_FLAG;
    else
        BasePolicyPara.dwFlags = 0;
    if (!CertVerifyCertificateChainPolicy(
            CERT_CHAIN_POLICY_BASIC_CONSTRAINTS,
            pChainContext,
            &BasePolicyPara,
            &BasePolicyStatus
            ))
        return FALSE;
    if (dwError = BasePolicyStatus.dwError) {
        lChainIndex = BasePolicyStatus.lChainIndex;
        lElementIndex = BasePolicyStatus.lElementIndex;
        goto CommonReturn;
    }

    dwError = 0;

CommonReturn:
    assert(pPolicyStatus && offsetof(CERT_CHAIN_POLICY_STATUS, lElementIndex) <
            pPolicyStatus->cbSize);
    pPolicyStatus->dwError = dwError;
    pPolicyStatus->lChainIndex = lChainIndex;
    pPolicyStatus->lElementIndex = lElementIndex;

    if (offsetof(CERT_CHAIN_POLICY_STATUS, pvExtraPolicyStatus) <
            pPolicyStatus->cbSize &&
                pPolicyStatus->pvExtraPolicyStatus) {
        // Return signer statement's Commercial/Individual flag
        PAUTHENTICODE_EXTRA_CERT_CHAIN_POLICY_STATUS pAuthStatus =
            (PAUTHENTICODE_EXTRA_CERT_CHAIN_POLICY_STATUS)
                pPolicyStatus->pvExtraPolicyStatus;
        if (offsetof(AUTHENTICODE_EXTRA_CERT_CHAIN_POLICY_STATUS,
                fCommercial) < pAuthStatus->cbSize)
            pAuthStatus->fCommercial = fCommercial;
    }
    return TRUE;
}

//+=========================================================================
//  CertDllVerifyAuthenticodeTimeStampCertificateChainPolicy Functions
//==========================================================================

void MapAuthenticodeTimeStampRegPolicySettingsToBaseChainPolicyFlags(
    IN DWORD dwRegPolicySettings,
    IN BOOL fCommercial,
    IN OUT DWORD *pdwFlags
    )
{
    DWORD dwFlags;

    if (0 == dwRegPolicySettings)
        return;

    dwFlags = *pdwFlags;
    if (dwRegPolicySettings & WTPF_TRUSTTEST)
        dwFlags |= CERT_CHAIN_POLICY_TRUST_TESTROOT_FLAG;
    if (dwRegPolicySettings & WTPF_TESTCANBEVALID)
        dwFlags |= CERT_CHAIN_POLICY_ALLOW_TESTROOT_FLAG;
    if (dwRegPolicySettings & WTPF_IGNOREEXPIRATION)
        dwFlags |= CERT_CHAIN_POLICY_IGNORE_ALL_NOT_TIME_VALID_FLAGS;

    if (dwRegPolicySettings & WTPF_IGNOREREVOCATIONONTS)
        dwFlags |= CERT_CHAIN_POLICY_IGNORE_ALL_REV_UNKNOWN_FLAGS;
    else {
        if (fCommercial) {
            if (dwRegPolicySettings & WTPF_OFFLINEOK_COM)
                dwFlags |= CERT_CHAIN_POLICY_IGNORE_END_REV_UNKNOWN_FLAG;
        } else {
            if (dwRegPolicySettings & WTPF_OFFLINEOK_IND)
                dwFlags |= CERT_CHAIN_POLICY_IGNORE_END_REV_UNKNOWN_FLAG;
        }
    }

    *pdwFlags = dwFlags;
}


void GetAuthenticodeTimeStampPara(
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    OUT DWORD *pdwRegPolicySettings,
    OUT BOOL *pfCommercial
    )
{
    *pdwRegPolicySettings = 0;
    *pfCommercial = FALSE;
    if (pPolicyPara && offsetof(CERT_CHAIN_POLICY_PARA, pvExtraPolicyPara) <
            pPolicyPara->cbSize && pPolicyPara->pvExtraPolicyPara) {
        PAUTHENTICODE_TS_EXTRA_CERT_CHAIN_POLICY_PARA pAuthPara =
            (PAUTHENTICODE_TS_EXTRA_CERT_CHAIN_POLICY_PARA)
                pPolicyPara->pvExtraPolicyPara;

        if (offsetof(AUTHENTICODE_TS_EXTRA_CERT_CHAIN_POLICY_PARA,
                dwRegPolicySettings) < pAuthPara->cbSize)
            *pdwRegPolicySettings = pAuthPara->dwRegPolicySettings;
        if (offsetof(AUTHENTICODE_TS_EXTRA_CERT_CHAIN_POLICY_PARA,
                fCommercial) < pAuthPara->cbSize)
            *pfCommercial = pAuthPara->fCommercial;
    }
}


BOOL
WINAPI
CertDllVerifyAuthenticodeTimeStampCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    )
{
    DWORD dwError;
    DWORD dwFlags;
    DWORD dwRegPolicySettings;
    BOOL fCommercial;
    LONG lChainIndex = -1;
    LONG lElementIndex = -1;

    CERT_CHAIN_POLICY_PARA BasePolicyPara;
    memset(&BasePolicyPara, 0, sizeof(BasePolicyPara));
    BasePolicyPara.cbSize = sizeof(BasePolicyPara);

    CERT_CHAIN_POLICY_STATUS BasePolicyStatus;
    memset(&BasePolicyStatus, 0, sizeof(BasePolicyStatus));
    BasePolicyStatus.cbSize = sizeof(BasePolicyStatus);

    if (pPolicyPara && offsetof(CERT_CHAIN_POLICY_PARA, dwFlags) <
            pPolicyPara->cbSize)
        dwFlags = pPolicyPara->dwFlags;
    else
        dwFlags = 0;
    GetAuthenticodeTimeStampPara(
        pPolicyPara, &dwRegPolicySettings, &fCommercial);

    MapAuthenticodeTimeStampRegPolicySettingsToBaseChainPolicyFlags(
        dwRegPolicySettings, fCommercial, &dwFlags);

    // Do the basic chain policy verification. Authenticode overrides
    // the defaults for the following:
    dwFlags |=
        CERT_CHAIN_POLICY_ALLOW_TESTROOT_FLAG |
        CERT_CHAIN_POLICY_IGNORE_CTL_SIGNER_REV_UNKNOWN_FLAG |
        CERT_CHAIN_POLICY_IGNORE_CA_REV_UNKNOWN_FLAG |
        CERT_CHAIN_POLICY_IGNORE_ROOT_REV_UNKNOWN_FLAG |
        CERT_CHAIN_POLICY_IGNORE_NOT_TIME_NESTED_FLAG;

    BasePolicyPara.dwFlags = dwFlags;
    if (!CertVerifyCertificateChainPolicy(
            CERT_CHAIN_POLICY_BASE,
            pChainContext,
            &BasePolicyPara,
            &BasePolicyStatus
            ))
        return FALSE;
    if (dwError = BasePolicyStatus.dwError) {
        dwError = MapToAuthenticodeError(dwError);
        lChainIndex = BasePolicyStatus.lChainIndex;
        lElementIndex = BasePolicyStatus.lElementIndex;
        goto CommonReturn;
    }

    dwError = 0;

CommonReturn:
    assert(pPolicyStatus && offsetof(CERT_CHAIN_POLICY_STATUS, lElementIndex) <
            pPolicyStatus->cbSize);
    pPolicyStatus->dwError = dwError;
    pPolicyStatus->lChainIndex = lChainIndex;
    pPolicyStatus->lElementIndex = lElementIndex;
    return TRUE;
}

//+=========================================================================
//  CertDllVerifySSLCertificateChainPolicy Functions
//==========================================================================

BOOL CompareSSLDNStoCommonName(LPCWSTR pDNS, LPCWSTR pCN)
{
    int      nCountPeriods;
    BOOL     fExactMatch;
    LPCWSTR  pBakDNS;
    LPCWSTR  pBakCN;

    nCountPeriods   = 1;  // start of DNS amount to virtual '.' as prefix
    fExactMatch     = TRUE;
    pBakDNS         = pDNS;
    pBakCN          = pCN;

    // Note: Verisign use to put "*" in certificate subject names
    while (((towupper(*pDNS) == towupper(*pCN)) || (*pCN == L'*')) && (*pDNS) && (*pCN))
    {
        if (towupper(*pDNS) != towupper(*pCN))
        {
            fExactMatch = FALSE;
        }

        if (*pCN == L'*')
        {
            nCountPeriods = 0;

            if (*pDNS == L'.')
            {
                pCN++;
            }
            else
            {
                pDNS++;
            }
        }
        else
        {
            if (*pDNS == L'.')
            {
                nCountPeriods++;
            }

            pDNS++;
            pCN++;
        }
    }
    //
    // if they are sized 0, we make sure not to say they match.
    //
    if ((pBakDNS == pDNS) || (pBakCN == pCN))
    {
         fExactMatch = FALSE;
    }

    return((*pDNS == NULL) && (*pCN == NULL) && ((nCountPeriods >= 2) ||  (fExactMatch)));
}

BOOL IsSSLServerNameInNameInfo(
    IN DWORD dwCertEncodingType,
    IN PCERT_NAME_BLOB pNameInfoBlob,
    IN LPCWSTR pwszServerName
    )
{
    BOOL fResult;
    PCERT_NAME_INFO pInfo = NULL;
    DWORD cbInfo;
    DWORD cRDN;
    PCERT_RDN pRDN;
    
    if (!CryptDecodeObjectEx(
            dwCertEncodingType,
            X509_UNICODE_NAME,
            pNameInfoBlob->pbData,
            pNameInfoBlob->cbData,
            CRYPT_DECODE_NOCOPY_FLAG | CRYPT_DECODE_ALLOC_FLAG |
                CRYPT_DECODE_SHARE_OID_STRING_FLAG,
            &PkiDecodePara,
            (void *) &pInfo,
            &cbInfo
            ))
        goto DecodeError;

    cRDN = pInfo->cRDN;
    pRDN = pInfo->rgRDN;
    for ( ; cRDN > 0; cRDN--, pRDN++) {
        DWORD cAttr = pRDN->cRDNAttr;
        PCERT_RDN_ATTR pAttr = pRDN->rgRDNAttr;
        for ( ; cAttr > 0; cAttr--, pAttr++) {
            if (!IS_CERT_RDN_CHAR_STRING(pAttr->dwValueType))
                continue;
            if (0 == strcmp(pAttr->pszObjId, szOID_COMMON_NAME)) {
                if (CompareSSLDNStoCommonName(pwszServerName,
                        (LPCWSTR) pAttr->Value.pbData))
                    goto SuccessReturn;
            }
        }
    }

    goto ErrorReturn;
SuccessReturn:
    fResult = TRUE;
CommonReturn:
    PkiFree(pInfo);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(DecodeError)
}

BOOL IsSSLServerNameInAltName(
    IN DWORD dwCertEncodingType,
    IN PCRYPT_DER_BLOB pAltNameBlob,
    IN LPCWSTR pwszServerName
    )
{
    BOOL fResult;
    PCERT_ALT_NAME_INFO pInfo = NULL;
    DWORD cbInfo;
    DWORD cEntry;
    PCERT_ALT_NAME_ENTRY pEntry;
    
    if (!CryptDecodeObjectEx(
            dwCertEncodingType,
            X509_ALTERNATE_NAME,
            pAltNameBlob->pbData,
            pAltNameBlob->cbData,
            CRYPT_DECODE_NOCOPY_FLAG | CRYPT_DECODE_ALLOC_FLAG |
                CRYPT_DECODE_SHARE_OID_STRING_FLAG,
            &PkiDecodePara,
            (void *) &pInfo,
            &cbInfo
            ))
        goto DecodeError;

    cEntry = pInfo->cAltEntry;
    pEntry = pInfo->rgAltEntry;
    for ( ; cEntry > 0; cEntry--, pEntry++) {
        switch (pEntry->dwAltNameChoice) {
            case CERT_ALT_NAME_DNS_NAME:
                if (CompareSSLDNStoCommonName(pwszServerName,
                        pEntry->pwszDNSName))
                    goto SuccessReturn;
                break;
            case CERT_ALT_NAME_DIRECTORY_NAME:
                if (IsSSLServerNameInNameInfo(dwCertEncodingType,
                        &pEntry->DirectoryName, pwszServerName))
                    goto SuccessReturn;
                break;
            default:
                break;
        }
    }

    goto ErrorReturn;
SuccessReturn:
    fResult = TRUE;
CommonReturn:
    PkiFree(pInfo);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(DecodeError)
}

BOOL IsSSLServerName(
    IN PCCERT_CONTEXT pCertContext,
    IN LPCWSTR pwszServerName
    )
{
    PCERT_INFO pCertInfo = pCertContext->pCertInfo;
    DWORD dwCertEncodingType = pCertContext->dwCertEncodingType;
    PCERT_EXTENSION pExt;

    if (pExt = CertFindExtension(
            szOID_SUBJECT_ALT_NAME2,
            pCertInfo->cExtension,
            pCertInfo->rgExtension
            )) {
        if (IsSSLServerNameInAltName(dwCertEncodingType,
                &pExt->Value, pwszServerName))
            return TRUE;
    }

    if (pExt = CertFindExtension(
            szOID_SUBJECT_ALT_NAME,
            pCertInfo->cExtension,
            pCertInfo->rgExtension
            )) {
        if (IsSSLServerNameInAltName(dwCertEncodingType,
                &pExt->Value, pwszServerName))
            return TRUE;
    }

    return IsSSLServerNameInNameInfo(dwCertEncodingType,
                &pCertInfo->Subject, pwszServerName);
}


BOOL
WINAPI
CertDllVerifySSLCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    )
{
    DWORD dwError;
    DWORD dwFlags;
    DWORD fdwChecks;
    LONG lChainIndex = -1;
    LONG lElementIndex = -1;

    SSL_EXTRA_CERT_CHAIN_POLICY_PARA NullSSLExtraPara;
    PSSL_EXTRA_CERT_CHAIN_POLICY_PARA pSSLExtraPara;    // not allocated

    CERT_CHAIN_POLICY_PARA BasePolicyPara;
    memset(&BasePolicyPara, 0, sizeof(BasePolicyPara));
    BasePolicyPara.cbSize = sizeof(BasePolicyPara);

    CERT_CHAIN_POLICY_STATUS BasePolicyStatus;
    memset(&BasePolicyStatus, 0, sizeof(BasePolicyStatus));
    BasePolicyStatus.cbSize = sizeof(BasePolicyStatus);


    if (pPolicyPara && offsetof(CERT_CHAIN_POLICY_PARA, dwFlags) <
            pPolicyPara->cbSize)
        dwFlags = pPolicyPara->dwFlags;
    else
        dwFlags = 0;

    if (pPolicyPara && offsetof(CERT_CHAIN_POLICY_PARA, pvExtraPolicyPara) <
            pPolicyPara->cbSize && pPolicyPara->pvExtraPolicyPara) {
        pSSLExtraPara =
            (PSSL_EXTRA_CERT_CHAIN_POLICY_PARA) pPolicyPara->pvExtraPolicyPara;
        if (offsetof(SSL_EXTRA_CERT_CHAIN_POLICY_PARA, pwszServerName) >=
                pSSLExtraPara->cbSize) {
            SetLastError((DWORD) ERROR_INVALID_PARAMETER);
            return FALSE;
        }
    } else {
        pSSLExtraPara = &NullSSLExtraPara;
        memset(&NullSSLExtraPara, 0, sizeof(NullSSLExtraPara));
        NullSSLExtraPara.cbSize = sizeof(NullSSLExtraPara);
        NullSSLExtraPara.dwAuthType = AUTHTYPE_SERVER;
    }
        
    fdwChecks = pSSLExtraPara->fdwChecks;
    if (fdwChecks) {
        if (fdwChecks & SECURITY_FLAG_IGNORE_UNKNOWN_CA)
            // 11-Nov-98 per Sanjay Shenoy removed
            // CERT_CHAIN_POLICY_IGNORE_WRONG_USAGE_FLAG;
            dwFlags |= CERT_CHAIN_POLICY_ALLOW_UNKNOWN_CA_FLAG;

        if (fdwChecks & SECURITY_FLAG_IGNORE_WRONG_USAGE)
            dwFlags |= CERT_CHAIN_POLICY_IGNORE_WRONG_USAGE_FLAG;
        if (fdwChecks & SECURITY_FLAG_IGNORE_CERT_DATE_INVALID)
            dwFlags |= CERT_CHAIN_POLICY_IGNORE_ALL_NOT_TIME_VALID_FLAGS;
    }

    // Do the basic chain policy verification. SSL overrides
    // the defaults for the following:
    dwFlags |=
        CERT_CHAIN_POLICY_IGNORE_CTL_SIGNER_REV_UNKNOWN_FLAG |
        CERT_CHAIN_POLICY_IGNORE_CA_REV_UNKNOWN_FLAG |
        CERT_CHAIN_POLICY_IGNORE_ROOT_REV_UNKNOWN_FLAG |
        CERT_CHAIN_POLICY_IGNORE_NOT_TIME_NESTED_FLAG;

    BasePolicyPara.dwFlags = dwFlags;
    if (!CertVerifyCertificateChainPolicy(
            CERT_CHAIN_POLICY_BASE,
            pChainContext,
            &BasePolicyPara,
            &BasePolicyStatus
            ))
        return FALSE;
    if (dwError = BasePolicyStatus.dwError) {
        if (CERT_E_CHAINING == dwError)
            dwError = CERT_E_UNTRUSTEDROOT;

        // Note, in looking at the wininet source, it appears they
        // look at CRYPT_E_REVOKED and not CERT_E_REVOKED.
        lChainIndex = BasePolicyStatus.lChainIndex;
        lElementIndex = BasePolicyStatus.lElementIndex;
        goto CommonReturn;
    }
        

    if (pSSLExtraPara->pwszServerName) {
        BOOL fSGCCert = FALSE;
        if (0 == (fdwChecks & SECURITY_FLAG_IGNORE_WRONG_USAGE) && 
                0 == (fdwChecks & SECURITY_FLAG_IGNORE_UNKNOWN_CA)) {
            if (pSSLExtraPara->dwAuthType == AUTHTYPE_SERVER) {
                if (!IFC_IsEndCertValidForUsage(
                        pChainContext->rgpChain[0]->rgpElement[0]->pCertContext,
                        szOID_PKIX_KP_SERVER_AUTH
                        )) {
                    //  this flag controls:
                    //      1. CN names MUST match
                    //      2. if CN names don't match, return an error code
                    //         that is unknown to wininet.dll so that the user
                    //         can NOT select "Yes" to go to the site anyway...
                    fSGCCert = TRUE;
                }
            }
        }

        if (0 == (fdwChecks & SECURITY_FLAG_IGNORE_CERT_CN_INVALID) ||
                fSGCCert) {
            if (!IsSSLServerName(
                    pChainContext->rgpChain[0]->rgpElement[0]->pCertContext,
                    pSSLExtraPara->pwszServerName
                    )) {
                dwError = fSGCCert ? CERT_E_ROLE : CERT_E_CN_NO_MATCH;
                goto EndCertError;
            }
        }
    }

    dwError = 0;

CommonReturn:
    assert(pPolicyStatus && offsetof(CERT_CHAIN_POLICY_STATUS, lElementIndex) <
            pPolicyStatus->cbSize);
    pPolicyStatus->dwError = dwError;
    pPolicyStatus->lChainIndex = lChainIndex;
    pPolicyStatus->lElementIndex = lElementIndex;

    return TRUE;

EndCertError:
    lChainIndex = 0;
    lElementIndex = 0;
    goto CommonReturn;
}


//+=========================================================================
//  CertDllVerifyNTAuthCertificateChainPolicy Functions
//==========================================================================

// Open and cache the store containing CAs trusted for NT Authentication.
// Also, enable auto resync for the cached store.
HCERTSTORE OpenNTAuthStore()
{
    HCERTSTORE hStore;

    hStore = hNTAuthCertStore;
    if (NULL == hStore) {
        // Serialize opening of the cached store
        CertPolicyLock();

        hStore = hNTAuthCertStore;
        if (NULL == hStore) {
            hStore = CertOpenStore(
                CERT_STORE_PROV_SYSTEM_REGISTRY_W, 
                0,                  // dwEncodingType
                0,                  // hCryptProv
                CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE |
                    CERT_STORE_MAXIMUM_ALLOWED_FLAG,
                L"NTAuth"
                );
            if (hStore) {
                CertControlStore(
                    hStore,
                    0,                  // dwFlags
                    CERT_STORE_CTRL_AUTO_RESYNC,
                    NULL                // pvCtrlPara
                    );
                hNTAuthCertStore = hStore;
            }
        }

        CertPolicyUnlock();
    }

    return hStore;
}

BOOL
WINAPI
CertDllVerifyNTAuthCertificateChainPolicy(
    IN LPCSTR pszPolicyOID,
    IN PCCERT_CHAIN_CONTEXT pChainContext,
    IN PCERT_CHAIN_POLICY_PARA pPolicyPara,
    IN OUT PCERT_CHAIN_POLICY_STATUS pPolicyStatus
    )
{
    BOOL fResult;
    DWORD dwError;
    PCERT_SIMPLE_CHAIN pChain;
    PCCERT_CONTEXT pFindCert;           // freed if found
    HCERTSTORE hStore;                  // cached, don't close
    BYTE rgbCertHash[SHA1_HASH_LEN];
    CRYPT_HASH_BLOB CertHash;

    assert(pPolicyStatus && offsetof(CERT_CHAIN_POLICY_STATUS, lElementIndex) <
            pPolicyStatus->cbSize);

    fResult = CertDllVerifyBaseCertificateChainPolicy(
        pszPolicyOID,
        pChainContext,
        pPolicyPara,
        pPolicyStatus
        );
    if (!fResult || 0 != pPolicyStatus->dwError)
        return fResult;

    fResult = CertDllVerifyBasicConstraintsCertificateChainPolicy(
        pszPolicyOID,
        pChainContext,
        pPolicyPara,
        pPolicyStatus
        );
    if (!fResult || 0 != pPolicyStatus->dwError)
        return fResult;

    // Check if we have a CA certificate that issued the end entity
    // certificate. Its Element[1] in the first simple chain.
    pChain = pChainContext->rgpChain[0];
    if (2 > pChain->cElement)
        goto MissingNTAuthCert;

    // Open the store where the CA certificate must exist to be trusted.
    // Note, this store is cached with auto resync enabled.
    if (NULL == (hStore = OpenNTAuthStore()))
        goto OpenNTAuthStoreError;

    // Try to find the CA certificate in the store
    CertHash.cbData = sizeof(rgbCertHash);
    CertHash.pbData = rgbCertHash;
    if (!CertGetCertificateContextProperty(
            pChain->rgpElement[1]->pCertContext,
            CERT_SHA1_HASH_PROP_ID,
            CertHash.pbData,
            &CertHash.cbData
            ))
        goto GetHashPropertyError;
    if (NULL == (pFindCert = CertFindCertificateInStore(
            hStore,
            0,                      // dwCertEncodingType
            0,                      // dwFindFlags
            CERT_FIND_SHA1_HASH,
            &CertHash,
            NULL                    // pPrevCertContext
            )))
        goto UntrustedNTAuthCert;
    CertFreeCertificateContext(pFindCert);

    dwError = 0;
CommonReturn:
    pPolicyStatus->dwError = dwError;
    return TRUE;

ErrorReturn:
    pPolicyStatus->lChainIndex = 0;
    pPolicyStatus->lElementIndex = 1;
MissingNTAuthCert:
    dwError = CERT_E_UNTRUSTEDCA;
    goto CommonReturn;
TRACE_ERROR(OpenNTAuthStoreError)
TRACE_ERROR(GetHashPropertyError)
TRACE_ERROR(UntrustedNTAuthCert)
}
