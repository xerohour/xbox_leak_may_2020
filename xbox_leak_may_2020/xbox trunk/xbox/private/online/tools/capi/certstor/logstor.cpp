//+-------------------------------------------------------------------------
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1995 - 1997
//
//  File:       logstor.cpp
//
//  Contents:   Registry Certificate Store Provider APIs
//
//  Functions:  I_RegStoreDllMain
//              I_CertDllOpenRegStoreProv
//              CertRegisterSystemStore
//              CertRegisterPhysicalStore
//              CertUnregisterSystemStore
//              CertUnregisterPhysicalStore
//              CertEnumSystemStoreLocation
//              CertEnumSystemStore
//              CertEnumPhysicalStore
//              I_CertDllOpenSystemRegistryStoreProvW
//              I_CertDllOpenSystemRegistryStoreProvA
//              I_CertDllOpenSystemStoreProvW
//              I_CertDllOpenSystemStoreProvA
//              I_CertDllOpenPhysicalStoreProvW
//
//  History:    28-Dec-96    philh   created
//              13-Aug-96    philh   added change notify and resync support
//              24-Aug-96    philh   added logical store support
//--------------------------------------------------------------------------

#include "global.hxx"
#include <dbgdef.h>
#include "xwinreg.h"
#include "xdbg.h"

#ifdef STATIC
#undef STATIC
#endif
#define STATIC


// Pointer to an allocated LONG containing thread's enum recursion depth
static HCRYPTTLS hTlsEnumPhysicalStoreDepth;
#define MAX_ENUM_PHYSICAL_STORE_DEPTH       20

#define SYSTEM_STORE_REGPATH        L"Software\\Microsoft\\SystemCertificates"
#define PHYSICAL_STORES_SUBKEY_NAME L"PhysicalStores"
#define CONST_OID_STR_PREFIX_CHAR   '#'
#define SERVICES_REGPATH            L"Software\\Microsoft\\Cryptography\\Services"
#define SYSTEM_CERTIFICATES_SUBKEY_NAME L"SystemCertificates"
#define GROUP_POLICY_STORE_REGPATH  L"Software\\Policies\\Microsoft\\SystemCertificates"
#define ENTERPRISE_STORE_REGPATH    L"Software\\Microsoft\\EnterpriseCertificates"

#define ROAMING_MY_STORE_SUBDIR     L"Microsoft\\SystemCertificates\\My"

#define REGISTER_FLAGS_MASK         (CERT_SYSTEM_STORE_MASK | \
                                        CERT_STORE_CREATE_NEW_FLAG)
#define UNREGISTER_FLAGS_MASK       (CERT_SYSTEM_STORE_MASK | \
                                        CERT_STORE_DELETE_FLAG | \
                                        CERT_STORE_OPEN_EXISTING_FLAG)
#define ENUM_FLAGS_MASK             (CERT_SYSTEM_STORE_MASK | \
                                        CERT_STORE_OPEN_EXISTING_FLAG | \
                                        CERT_STORE_MAXIMUM_ALLOWED_FLAG | \
                                        CERT_STORE_READONLY_FLAG)

#define OPEN_REG_FLAGS_MASK         (CERT_STORE_CREATE_NEW_FLAG | \
                                        CERT_STORE_DELETE_FLAG | \
                                        CERT_STORE_OPEN_EXISTING_FLAG | \
                                        CERT_STORE_MAXIMUM_ALLOWED_FLAG | \
                                        CERT_STORE_READONLY_FLAG | \
                                        CERT_STORE_MANIFOLD_FLAG | \
                                        CERT_STORE_UPDATE_KEYID_FLAG | \
                                        CERT_STORE_ENUM_ARCHIVED_FLAG | \
                                        CERT_STORE_SET_LOCALIZED_NAME_FLAG | \
                                        CERT_STORE_NO_CRYPT_RELEASE_FLAG | \
                                        CERT_REGISTRY_STORE_REMOTE_FLAG | \
                                        CERT_REGISTRY_STORE_SERIALIZED_FLAG | \
                                        CERT_REGISTRY_STORE_ROAMING_FLAG | \
                                        CERT_REGISTRY_STORE_CLIENT_GPT_FLAG | \
                                        CERT_REGISTRY_STORE_MY_IE_DIRTY_FLAG | \
                                        CERT_REGISTRY_STORE_LM_GPT_FLAG)
#define OPEN_SYS_FLAGS_MASK         (CERT_SYSTEM_STORE_MASK | \
                                        CERT_STORE_CREATE_NEW_FLAG | \
                                        CERT_STORE_DELETE_FLAG | \
                                        CERT_STORE_OPEN_EXISTING_FLAG | \
                                        CERT_STORE_MAXIMUM_ALLOWED_FLAG | \
                                        CERT_STORE_READONLY_FLAG | \
                                        CERT_STORE_MANIFOLD_FLAG | \
                                        CERT_STORE_UPDATE_KEYID_FLAG | \
                                        CERT_STORE_ENUM_ARCHIVED_FLAG | \
                                        CERT_STORE_SET_LOCALIZED_NAME_FLAG | \
                                        CERT_STORE_NO_CRYPT_RELEASE_FLAG)
#define OPEN_PHY_FLAGS_MASK         (CERT_SYSTEM_STORE_MASK | \
                                        CERT_STORE_DELETE_FLAG | \
                                        CERT_STORE_OPEN_EXISTING_FLAG | \
                                        CERT_STORE_MAXIMUM_ALLOWED_FLAG | \
                                        CERT_STORE_READONLY_FLAG | \
                                        CERT_STORE_MANIFOLD_FLAG | \
                                        CERT_STORE_UPDATE_KEYID_FLAG | \
                                        CERT_STORE_ENUM_ARCHIVED_FLAG | \
                                        CERT_STORE_SET_LOCALIZED_NAME_FLAG | \
                                        CERT_STORE_NO_CRYPT_RELEASE_FLAG)
//+-------------------------------------------------------------------------
//  Common, global logical store critical section. Used by:
//      GptStore, Win95Store, RoamingStore.
//--------------------------------------------------------------------------
static CRITICAL_SECTION ILS_CriticalSection;


//+-------------------------------------------------------------------------
//  Registry Store Context SubKeys
//--------------------------------------------------------------------------

#ifdef CAPI_INCLUDE_CTL
#define CONTEXT_COUNT       3
#elif defined(CAPI_INCLUDE_CRL)
#define CONTEXT_COUNT		2
#else
#define CONTEXT_COUNT		1
#endif

static const LPCWSTR rgpwszContextSubKeyName[CONTEXT_COUNT] = {
    L"Certificates",

#if CONTEXT_COUNT > 1
    L"CRLs",
#endif

#if CONTEXT_COUNT > 2
    L"CTLs"
#endif    
};

#define KEYID_CONTEXT_NAME          L"Keys"

static DWORD rgdwContextTypeFlags[CONTEXT_COUNT] = {
    CERT_STORE_CERTIFICATE_CONTEXT_FLAG,

#if CONTEXT_COUNT > 1
    CERT_STORE_CRL_CONTEXT_FLAG,
#endif

#if CONTEXT_COUNT > 2
    CERT_STORE_CTL_CONTEXT_FLAG
#endif    
};

#define MY_SYSTEM_INDEX         0
#define ROOT_SYSTEM_INDEX       1
#define TRUST_SYSTEM_INDEX      2
#define CA_SYSTEM_INDEX         3
#define USER_DS_SYSTEM_INDEX    4

#define MY_SYSTEM_FLAG          (1 << MY_SYSTEM_INDEX)
#define ROOT_SYSTEM_FLAG        (1 << ROOT_SYSTEM_INDEX)
#define TRUST_SYSTEM_FLAG       (1 << TRUST_SYSTEM_INDEX)
#define CA_SYSTEM_FLAG          (1 << CA_SYSTEM_INDEX)
#define USER_DS_SYSTEM_FLAG     (1 << USER_DS_SYSTEM_INDEX)

#define COMMON_SYSTEM_FLAGS     ( \
    MY_SYSTEM_FLAG | \
    ROOT_SYSTEM_FLAG | \
    TRUST_SYSTEM_FLAG | \
    CA_SYSTEM_FLAG \
    )

#define wsz_MY_STORE        L"My"
#define wsz_ROOT_STORE      L"Root"
#define wsz_TRUST_STORE     L"Trust"
#define wsz_CA_STORE        L"CA"
#define wsz_USER_DS_STORE   L"UserDS"
static LPCWSTR rgpwszPredefinedSystemStore[] = {
    wsz_MY_STORE,
    wsz_ROOT_STORE,
    wsz_TRUST_STORE,
    wsz_CA_STORE,
    wsz_USER_DS_STORE
};
#define NUM_PREDEFINED_SYSTEM_STORE (sizeof(rgpwszPredefinedSystemStore) / \
                                        sizeof(rgpwszPredefinedSystemStore[0]))


#define DEFAULT_PHYSICAL_INDEX          0
#define GROUP_POLICY_PHYSICAL_INDEX     1
#define LOCAL_MACHINE_PHYSICAL_INDEX    2
#define DS_USER_CERT_PHYSICAL_INDEX     3
#define LMGP_PHYSICAL_INDEX             4
#define ENTERPRISE_PHYSICAL_INDEX       5
#define NUM_PREDEFINED_PHYSICAL         6

#define DEFAULT_PHYSICAL_FLAG           (1 << DEFAULT_PHYSICAL_INDEX)
#ifdef CE_BUILD
#define GROUP_POLICY_PHYSICAL_FLAG		0	// hack to make sure GP store are not in predefined collections
#else
#define GROUP_POLICY_PHYSICAL_FLAG      (1 << GROUP_POLICY_PHYSICAL_INDEX)
#endif
#define LOCAL_MACHINE_PHYSICAL_FLAG     (1 << LOCAL_MACHINE_PHYSICAL_INDEX)
#define DS_USER_CERT_PHYSICAL_FLAG      (1 << DS_USER_CERT_PHYSICAL_INDEX)
#define LMGP_PHYSICAL_FLAG              (1 << LMGP_PHYSICAL_INDEX)
#define ENTERPRISE_PHYSICAL_FLAG        (1 << ENTERPRISE_PHYSICAL_INDEX)

static LPCWSTR rgpwszPredefinedPhysical[NUM_PREDEFINED_PHYSICAL] = {
    CERT_PHYSICAL_STORE_DEFAULT_NAME,
    CERT_PHYSICAL_STORE_GROUP_POLICY_NAME,
    CERT_PHYSICAL_STORE_LOCAL_MACHINE_NAME,
    CERT_PHYSICAL_STORE_DS_USER_CERTIFICATE_NAME,
    CERT_PHYSICAL_STORE_LOCAL_MACHINE_GROUP_POLICY_NAME,
    CERT_PHYSICAL_STORE_ENTERPRISE_NAME,
};

#define NOT_IN_REGISTRY_SYSTEM_STORE_LOCATION_FLAG  0x1
#define REMOTABLE_SYSTEM_STORE_LOCATION_FLAG        0x2
#define SERIALIZED_SYSTEM_STORE_LOCATION_FLAG       0x4

typedef struct _SYSTEM_STORE_LOCATION_INFO {
    DWORD       dwFlags;
    DWORD       dwPredefinedSystemFlags;
    DWORD       dwPredefinedPhysicalFlags;
} SYSTEM_STORE_LOCATION_INFO, *PSYSTEM_STORE_LOCATION_INFO;


static const SYSTEM_STORE_LOCATION_INFO rgSystemStoreLocationInfo[] = {
    //  Not Defined                                     0
    NOT_IN_REGISTRY_SYSTEM_STORE_LOCATION_FLAG,
    0,
    0,

    //  CERT_SYSTEM_STORE_CURRENT_USER_ID               1
    0,
    COMMON_SYSTEM_FLAGS | USER_DS_SYSTEM_FLAG,
    DEFAULT_PHYSICAL_FLAG | GROUP_POLICY_PHYSICAL_FLAG |
        LOCAL_MACHINE_PHYSICAL_FLAG,

    // CERT_SYSTEM_STORE_LOCAL_MACHINE_ID               2
    REMOTABLE_SYSTEM_STORE_LOCATION_FLAG,
    COMMON_SYSTEM_FLAGS,
    DEFAULT_PHYSICAL_FLAG | GROUP_POLICY_PHYSICAL_FLAG |
        ENTERPRISE_PHYSICAL_FLAG,

    //  Not Defined                                     3
    NOT_IN_REGISTRY_SYSTEM_STORE_LOCATION_FLAG,
    0,
    0,

#ifndef UNDER_CE
    // CERT_SYSTEM_STORE_CURRENT_SERVICE_ID             4
    0,
    COMMON_SYSTEM_FLAGS,
    DEFAULT_PHYSICAL_FLAG | LOCAL_MACHINE_PHYSICAL_FLAG,

    // CERT_SYSTEM_STORE_SERVICES_ID                    5
    REMOTABLE_SYSTEM_STORE_LOCATION_FLAG,
    COMMON_SYSTEM_FLAGS,
    DEFAULT_PHYSICAL_FLAG | LOCAL_MACHINE_PHYSICAL_FLAG,

    // CERT_SYSTEM_STORE_USERS_ID                       6
    REMOTABLE_SYSTEM_STORE_LOCATION_FLAG,
    COMMON_SYSTEM_FLAGS,
    DEFAULT_PHYSICAL_FLAG | LOCAL_MACHINE_PHYSICAL_FLAG,

    // CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY_ID   7
    //SERIALIZED_SYSTEM_STORE_LOCATION_FLAG,
    0,
    COMMON_SYSTEM_FLAGS,
    DEFAULT_PHYSICAL_FLAG,

    // CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY_ID  8
    //SERIALIZED_SYSTEM_STORE_LOCATION_FLAG |
        REMOTABLE_SYSTEM_STORE_LOCATION_FLAG,
    COMMON_SYSTEM_FLAGS,
    DEFAULT_PHYSICAL_FLAG,

    // CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE_ID    9
    REMOTABLE_SYSTEM_STORE_LOCATION_FLAG,
    COMMON_SYSTEM_FLAGS,
    DEFAULT_PHYSICAL_FLAG
#else
// CE does not support the following locations:
    // CERT_SYSTEM_STORE_CURRENT_SERVICE_ID             4
    NOT_IN_REGISTRY_SYSTEM_STORE_LOCATION_FLAG,
    0,
    0,

    // CERT_SYSTEM_STORE_SERVICES_ID                    5
    NOT_IN_REGISTRY_SYSTEM_STORE_LOCATION_FLAG,
    0,
    0,

    // CERT_SYSTEM_STORE_USERS_ID                       6
    NOT_IN_REGISTRY_SYSTEM_STORE_LOCATION_FLAG,
    0,
    0,

    // CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY_ID   7
    //SERIALIZED_SYSTEM_STORE_LOCATION_FLAG,
    NOT_IN_REGISTRY_SYSTEM_STORE_LOCATION_FLAG,
    0,
    0,
    // CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY_ID  8
    //SERIALIZED_SYSTEM_STORE_LOCATION_FLAG |
    NOT_IN_REGISTRY_SYSTEM_STORE_LOCATION_FLAG,
    0,
    0,

    // CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE_ID    9
    NOT_IN_REGISTRY_SYSTEM_STORE_LOCATION_FLAG,
    0,
    0,
#endif
};

#define NUM_SYSTEM_STORE_LOCATION   (sizeof(rgSystemStoreLocationInfo) / \
                                        sizeof(rgSystemStoreLocationInfo[0]))

#define CURRENT_USER_ROOT_PHYSICAL_FLAGS ( \
    DEFAULT_PHYSICAL_FLAG | \
    LOCAL_MACHINE_PHYSICAL_FLAG \
    )

#define USERS_ROOT_PHYSICAL_FLAGS ( \
    LOCAL_MACHINE_PHYSICAL_FLAG \
    )

#define MY_PHYSICAL_FLAGS ( \
    DEFAULT_PHYSICAL_FLAG \
    )

#define USER_DS_PHYSICAL_FLAGS ( \
    DS_USER_CERT_PHYSICAL_FLAG \
    )


#define sz_CRYPTNET_DLL             "cryptnet.dll"
#define sz_GetUserDsStoreUrl        "I_CryptNetGetUserDsStoreUrl"
typedef BOOL (WINAPI *PFN_GET_USER_DS_STORE_URL)(
          IN LPWSTR pwszUserAttribute,
          OUT LPWSTR* ppwszUrl
          );

#define wsz_USER_CERTIFICATE_ATTR   L"userCertificate"


#define PHYSICAL_NAME_INDEX     0
#define SYSTEM_NAME_INDEX       1
#define SERVICE_NAME_INDEX      2
#define USER_NAME_INDEX         2
#define COMPUTER_NAME_INDEX     3
#define SYSTEM_NAME_PATH_COUNT  4

#define DEFAULT_USER_NAME       L".Default"

typedef struct _SYSTEM_NAME_INFO {
    LPWSTR      rgpwszName[SYSTEM_NAME_PATH_COUNT];
    // non-NULL for relocated store. Note hKeyBase isn't opened and
    // doesn't need to be closed
    HKEY        hKeyBase;
} SYSTEM_NAME_INFO, *PSYSTEM_NAME_INFO;


typedef struct _REG_STORE REG_STORE, *PREG_STORE;

typedef struct _ILS_RESYNC_ENTRY {
    HANDLE              hOrigEvent;
    HANDLE              hEvent;
    PREG_STORE          pRegStore;
} ILS_RESYNC_ENTRY, *PILS_RESYNC_ENTRY;

typedef struct _REGISTRY_STORE_CHANGE_INFO {
    HANDLE              hChange;
    HANDLE              hRegWaitFor;
    DWORD               cNotifyEntry;
    PILS_RESYNC_ENTRY   rgNotifyEntry;
} REGISTRY_STORE_CHANGE_INFO, *PREGISTRY_STORE_CHANGE_INFO;

//+-------------------------------------------------------------------------
//  Registry Store Provider handle information
//
//  hMyNotifyChange is our internal NotifyChange event handle.
//--------------------------------------------------------------------------
struct _REG_STORE {
    HCERTSTORE          hCertStore;         // not duplicated
    CRITICAL_SECTION    CriticalSection;
    HANDLE              hMyNotifyChange;
    BOOL                fResync;            // when set, ignore callback deletes
    HKEY                hKey;
    DWORD               dwFlags;

    // Following field is applicable to the CurrentUser "Root" store
    BOOL                fProtected;

    // Following field is applicable when
    // CERT_REGISTRY_STORE_SERIALIZED_FLAG is set in dwFlags
    BOOL                fTouched;      // set for write, delete or set property

    union {
        // Following field is applicable when
        // CERT_REGISTRY_STORE_CLIENT_GPT_FLAG is set in dwFlags
        CERT_REGISTRY_STORE_CLIENT_GPT_PARA GptPara;

        // Following field is applicable when
        // CERT_REGISTRY_STORE_ROAMING_FLAG is set in dwFlags
        LPWSTR              pwszStoreDirectory;
    };

    // Following field is applicable for change notify of registry or
    // roaming file store
    PREGISTRY_STORE_CHANGE_INFO pRegistryStoreChangeInfo;
};


typedef struct _ENUM_SYSTEM_STORE_LOCATION_INFO {
    DWORD               dwFlags;
    LPCWSTR             pwszLocation;
} ENUM_SYSTEM_STORE_LOCATION_INFO, *PENUM_SYSTEM_STORE_LOCATION_INFO;

// Predefined crypt32.dll locations. MUST NOT BE REGISTERED!!!
static const ENUM_SYSTEM_STORE_LOCATION_INFO rgEnumSystemStoreLocationInfo[] = {
    CERT_SYSTEM_STORE_CURRENT_USER, L"CurrentUser",
    CERT_SYSTEM_STORE_LOCAL_MACHINE, L"LocalMachine",
    CERT_SYSTEM_STORE_CURRENT_SERVICE, L"CurrentService",
    CERT_SYSTEM_STORE_SERVICES, L"Services",
    CERT_SYSTEM_STORE_USERS, L"Users",
    CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY, L"CurrentUserGroupPolicy",
    CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY, L"LocalMachineGroupPolicy",
    CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE, L"LocalMachineEnterprise"
};
#define ENUM_SYSTEM_STORE_LOCATION_CNT \
        (sizeof(rgEnumSystemStoreLocationInfo) / \
            sizeof(rgEnumSystemStoreLocationInfo[0]))

#define OPEN_SYSTEM_STORE_PROV_FUNC_SET     0
#define REGISTER_SYSTEM_STORE_FUNC_SET      1
#define UNREGISTER_SYSTEM_STORE_FUNC_SET    2
#define ENUM_SYSTEM_STORE_FUNC_SET          3
#define REGISTER_PHYSICAL_STORE_FUNC_SET    4
#define UNREGISTER_PHYSICAL_STORE_FUNC_SET  5
#define ENUM_PHYSICAL_STORE_FUNC_SET        6
#define FUNC_SET_COUNT                      7

static HCRYPTOIDFUNCSET rghFuncSet[FUNC_SET_COUNT];
static const LPCSTR rgpszFuncName[FUNC_SET_COUNT] = {
    CRYPT_OID_OPEN_SYSTEM_STORE_PROV_FUNC,
    CRYPT_OID_REGISTER_SYSTEM_STORE_FUNC,
    CRYPT_OID_UNREGISTER_SYSTEM_STORE_FUNC,
    CRYPT_OID_ENUM_SYSTEM_STORE_FUNC,
    CRYPT_OID_REGISTER_PHYSICAL_STORE_FUNC,
    CRYPT_OID_UNREGISTER_PHYSICAL_STORE_FUNC,
    CRYPT_OID_ENUM_PHYSICAL_STORE_FUNC
};

typedef BOOL (WINAPI *PFN_REGISTER_SYSTEM_STORE)(
    IN const void *pvSystemStore,
    IN DWORD dwFlags,
    IN PCERT_SYSTEM_STORE_INFO pStoreInfo,
    IN OPTIONAL void *pvReserved
    );
typedef BOOL (WINAPI *PFN_UNREGISTER_SYSTEM_STORE)(
    IN const void *pvSystemStore,
    IN DWORD dwFlags
    );
typedef BOOL (WINAPI *PFN_ENUM_SYSTEM_STORE)(
    IN DWORD dwFlags,
    IN OPTIONAL void *pvSystemStoreLocationPara,
    IN void *pvArg,
    IN PFN_CERT_ENUM_SYSTEM_STORE pfnEnum
    );

typedef BOOL (WINAPI *PFN_REGISTER_PHYSICAL_STORE)(
    IN const void *pvSystemStore,
    IN DWORD dwFlags,
    IN LPCWSTR pwszStoreName,
    IN PCERT_PHYSICAL_STORE_INFO pStoreInfo,
    IN OPTIONAL void *pvReserved
    );
typedef BOOL (WINAPI *PFN_UNREGISTER_PHYSICAL_STORE)(
    IN const void *pvSystemStore,
    IN DWORD dwFlags,
    IN LPCWSTR pwszStoreName
    );
typedef BOOL (WINAPI *PFN_ENUM_PHYSICAL_STORE)(
    IN const void *pvSystemStore,
    IN DWORD dwFlags,
    IN void *pvArg,
    IN PFN_CERT_ENUM_PHYSICAL_STORE pfnEnum
    );


//+-------------------------------------------------------------------------
//  Registry Store Provider Functions.
//--------------------------------------------------------------------------
STATIC void WINAPI RegStoreProvClose(
        IN HCERTSTOREPROV hStoreProv,
        IN DWORD dwFlags
        );
STATIC BOOL WINAPI RegStoreProvReadCert(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pStoreCertContext,
        IN DWORD dwFlags,
        OUT PCCERT_CONTEXT *ppProvCertContext
        );
STATIC BOOL WINAPI RegStoreProvWriteCert(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pCertContext,
        IN DWORD dwFlags
        );
STATIC BOOL WINAPI RegStoreProvDeleteCert(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pCertContext,
        IN DWORD dwFlags
        );
STATIC BOOL WINAPI RegStoreProvSetCertProperty(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pCertContext,
        IN DWORD dwPropId,
        IN DWORD dwFlags,
        IN const void *pvData
        );

STATIC BOOL WINAPI RegStoreProvReadCrl(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCRL_CONTEXT pStoreCrlContext,
        IN DWORD dwFlags,
        OUT PCCRL_CONTEXT *ppProvCrlContext
        );
STATIC BOOL WINAPI RegStoreProvWriteCrl(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCRL_CONTEXT pCrlContext,
        IN DWORD dwFlags
        );
STATIC BOOL WINAPI RegStoreProvDeleteCrl(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCRL_CONTEXT pCrlContext,
        IN DWORD dwFlags
        );
STATIC BOOL WINAPI RegStoreProvSetCrlProperty(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCRL_CONTEXT pCrlContext,
        IN DWORD dwPropId,
        IN DWORD dwFlags,
        IN const void *pvData
        );

STATIC BOOL WINAPI RegStoreProvReadCtl(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCTL_CONTEXT pStoreCtlContext,
        IN DWORD dwFlags,
        OUT PCCTL_CONTEXT *ppProvCtlContext
        );
STATIC BOOL WINAPI RegStoreProvWriteCtl(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCTL_CONTEXT pCtlContext,
        IN DWORD dwFlags
        );
STATIC BOOL WINAPI RegStoreProvDeleteCtl(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCTL_CONTEXT pCtlContext,
        IN DWORD dwFlags
        );
STATIC BOOL WINAPI RegStoreProvSetCtlProperty(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCTL_CONTEXT pCtlContext,
        IN DWORD dwPropId,
        IN DWORD dwFlags,
        IN const void *pvData
        );

STATIC BOOL WINAPI RegStoreProvControl(
        IN HCERTSTOREPROV hStoreProv,
        IN DWORD dwFlags,
        IN DWORD dwCtrlType,
        IN void const *pvCtrlPara
        );

static void * const rgpvRegStoreProvFunc[] = {
    // CERT_STORE_PROV_CLOSE_FUNC              0
    RegStoreProvClose,
    // CERT_STORE_PROV_READ_CERT_FUNC          1
    RegStoreProvReadCert,
    // CERT_STORE_PROV_WRITE_CERT_FUNC         2
    RegStoreProvWriteCert,
    // CERT_STORE_PROV_DELETE_CERT_FUNC        3
    RegStoreProvDeleteCert,
    // CERT_STORE_PROV_SET_CERT_PROPERTY_FUNC  4
    RegStoreProvSetCertProperty,
#ifdef CAPI_INCLUDE_CRL    
    // CERT_STORE_PROV_READ_CRL_FUNC           5
    RegStoreProvReadCrl,
    // CERT_STORE_PROV_WRITE_CRL_FUNC          6
    RegStoreProvWriteCrl,
    // CERT_STORE_PROV_DELETE_CRL_FUNC         7
    RegStoreProvDeleteCrl,
    // CERT_STORE_PROV_SET_CRL_PROPERTY_FUNC   8
    RegStoreProvSetCrlProperty,
#else
	NULL,NULL,NULL,NULL,
#endif
#ifdef CAPI_INCLUDE_CTL
    // CERT_STORE_PROV_READ_CTL_FUNC           9
    RegStoreProvReadCtl,
    // CERT_STORE_PROV_WRITE_CTL_FUNC          10
    RegStoreProvWriteCtl,
    // CERT_STORE_PROV_DELETE_CTL_FUNC         11
    RegStoreProvDeleteCtl,
    // CERT_STORE_PROV_SET_CTL_PROPERTY_FUNC   12
    RegStoreProvSetCtlProperty,
  #else
 	NULL,NULL,NULL,NULL,
 #endif

#ifndef CE_BUILD
   // CERT_STORE_PROV_CONTROL_FUNC            13
    RegStoreProvControl
#else
	NULL
#endif	
};
#define REG_STORE_PROV_FUNC_COUNT (sizeof(rgpvRegStoreProvFunc) / \
                                    sizeof(rgpvRegStoreProvFunc[0]))

STATIC BOOL WINAPI RootStoreProvWriteCert(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pCertContext,
        IN DWORD dwFlags
        );

STATIC BOOL WINAPI RootStoreProvDeleteCert(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pCertContext,
        IN DWORD dwFlags
        );

static void * const rgpvRootStoreProvFunc[] = {
    // CERT_STORE_PROV_CLOSE_FUNC              0
    RegStoreProvClose,
    // CERT_STORE_PROV_READ_CERT_FUNC          1
    RegStoreProvReadCert,
    // CERT_STORE_PROV_WRITE_CERT_FUNC         2
    RootStoreProvWriteCert,
    // CERT_STORE_PROV_DELETE_CERT_FUNC        3
    RootStoreProvDeleteCert,
    // CERT_STORE_PROV_SET_CERT_PROPERTY_FUNC  4
    RegStoreProvSetCertProperty,
#ifdef CAPI_INCLUDE_CRL
    // CERT_STORE_PROV_READ_CRL_FUNC           5
    RegStoreProvReadCrl,
    // CERT_STORE_PROV_WRITE_CRL_FUNC          6
    RegStoreProvWriteCrl,
    // CERT_STORE_PROV_DELETE_CRL_FUNC         7
    RegStoreProvDeleteCrl,
    // CERT_STORE_PROV_SET_CRL_PROPERTY_FUNC   8
    RegStoreProvSetCrlProperty,
#else
	NULL,NULL,NULL,NULL,
#endif
#ifdef CAPI_INCLUDE_CTL
    // CERT_STORE_PROV_READ_CTL_FUNC           9
    RegStoreProvReadCtl,
    // CERT_STORE_PROV_WRITE_CTL_FUNC          10
    RegStoreProvWriteCtl,
    // CERT_STORE_PROV_DELETE_CTL_FUNC         11
    RegStoreProvDeleteCtl,
    // CERT_STORE_PROV_SET_CTL_PROPERTY_FUNC   12
    RegStoreProvSetCtlProperty,
#else
	NULL,NULL,NULL,NULL,
#endif

#ifndef CE_BUILD
    // CERT_STORE_PROV_CONTROL_FUNC            13
    RegStoreProvControl
#else
	NULL
#endif
};
#define ROOT_STORE_PROV_FUNC_COUNT (sizeof(rgpvRootStoreProvFunc) / \
                                    sizeof(rgpvRootStoreProvFunc[0]))

//+-------------------------------------------------------------------------
//  Add the serialized store to the store.
//
//  from newstor.cpp
//--------------------------------------------------------------------------
extern BOOL WINAPI I_CertAddSerializedStore(
        IN HCERTSTORE hCertStore,
        IN BYTE *pbStore,
        IN DWORD cbStore
        );

//+-------------------------------------------------------------------------
//  CertStore allocation and free functions
//--------------------------------------------------------------------------
STATIC LPWSTR AllocAndCopyString(
    IN LPCWSTR pwszSrc,
    IN LONG cchSrc = -1
    )
{
    LPWSTR pwszDst;

    if (cchSrc < 0)
        cchSrc = wcslen(pwszSrc);
    if (NULL == (pwszDst = (LPWSTR) PkiNonzeroAlloc(
            (cchSrc + 1) * sizeof(WCHAR))))
        return NULL;
    if (0 < cchSrc)
        memcpy((BYTE *) pwszDst, (BYTE *) pwszSrc, cchSrc * sizeof(WCHAR));
    pwszDst[cchSrc] = L'\0';
    return pwszDst;
}

#if 1
// BUGBUG move to crypt32.dll
extern
BOOL
WINAPI
I_ProtectedRootDllMain(
        HMODULE hInst,
        ULONG  ulReason,
        LPVOID lpReserved);
#endif

//+=========================================================================
//  Register WaitFor Forward Function References
//==========================================================================
STATIC void RegWaitForProcessAttach();
STATIC void RegWaitForProcessDetach();

//+=========================================================================
//  Client "GPT" Store Forward Function References
//==========================================================================
STATIC void GptStoreProcessAttach();
STATIC void GptStoreProcessDetach();

STATIC BOOL OpenAllFromGptRegistry(
    IN PREG_STORE pRegStore,
    IN HCERTSTORE hCertStore
    );

STATIC BOOL CommitAllToGptRegistry(
    IN PREG_STORE pRegStore,
    IN DWORD dwFlags
    );

STATIC void GptStoreSignalAndFreeRegStoreResyncEntries(
    IN PREG_STORE pRegStore
    );

STATIC BOOL RegGptStoreChange(
    IN PREG_STORE pRegStore,
    IN HANDLE hEvent
    );

static inline BOOL IsClientGptStore(
    IN PSYSTEM_NAME_INFO pInfo,
    IN DWORD dwFlags
    )
{
    DWORD dwStoreLocation = dwFlags & CERT_SYSTEM_STORE_LOCATION_MASK;

    if (!(CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY == dwStoreLocation ||
          CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY == dwStoreLocation))
        return FALSE;

    if (dwFlags & (CERT_SYSTEM_STORE_RELOCATE_FLAG | CERT_STORE_DELETE_FLAG))
        return FALSE;

    return TRUE;
}

//+=========================================================================
//  Win95 Notify Store Forward Function References
//==========================================================================

// Following is created at ProcessAttach for Win95 clients
static HANDLE hWin95NotifyEvent = NULL;

STATIC void Win95StoreProcessAttach();
STATIC void Win95StoreProcessDetach();

STATIC void Win95StoreSignalAndFreeRegStoreResyncEntries(
    IN PREG_STORE pRegStore
    );

STATIC BOOL RegWin95StoreChange(
    IN PREG_STORE pRegStore,
    IN HANDLE hEvent
    );

//+=========================================================================
// Roaming Store Forward Function References
//==========================================================================
STATIC void RoamingStoreProcessAttach();
STATIC void RoamingStoreProcessDetach();

LPWSTR
ILS_GetRoamingStoreDirectory(
    IN LPCWSTR pwszStoreName
    );

BOOL
ILS_WriteElementToFile(
    IN LPCWSTR pwszStoreDir,
    IN LPCWSTR pwszContextName,
    IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
    IN DWORD dwFlags,       // CERT_STORE_CREATE_NEW_FLAG may be set
    IN const BYTE *pbElement,
    IN DWORD cbElement
    );

BOOL
ILS_ReadElementFromFile(
    IN LPCWSTR pwszStoreDir,
    IN LPCWSTR pwszContextName,
    IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
    IN DWORD dwFlags,
    OUT BYTE **ppbElement,
    OUT DWORD *pcbElement
    );

BOOL
ILS_DeleteElementFromDirectory(
    IN LPCWSTR pwszStoreDir,
    IN LPCWSTR pwszContextName,
    IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
    IN DWORD dwFlags
    );

typedef BOOL (*PFN_ILS_OPEN_ELEMENT)(
    IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
    IN const BYTE *pbElement,
    IN DWORD cbElement,
    IN void *pvArg
    );

BOOL
ILS_OpenAllElementsFromDirectory(
    IN LPCWSTR pwszStoreDir,
    IN LPCWSTR pwszContextName,
    IN DWORD dwFlags,
    IN void *pvArg,
    IN PFN_ILS_OPEN_ELEMENT pfnOpenElement
    );

//+=========================================================================
// Registry or Roaming Store Change Notify Functions
//==========================================================================
STATIC BOOL RegRegistryStoreChange(
    IN PREG_STORE pRegStore,
    IN HANDLE hEvent
    );

STATIC void FreeRegistryStoreChange(
    IN PREG_STORE pRegStore
    );

//+-------------------------------------------------------------------------
//  Dll initialization
//--------------------------------------------------------------------------
BOOL
WINAPI
I_RegStoreDllMain(
        HMODULE hInst,
        ULONG  ulReason,
        LPVOID lpReserved)
{
    BOOL    fRet;
    DWORD   i;

#ifndef CE_BUILD
    // BUGBUG move to crypt32.dll
    if (!I_ProtectedRootDllMain(hInst, ulReason, lpReserved))
        goto ErrorReturn;
#endif

    switch (ulReason) {
    case DLL_PROCESS_ATTACH:
        InitializeCriticalSection(&ILS_CriticalSection);
#ifndef CE_BUILD     
        RegWaitForProcessAttach();
        GptStoreProcessAttach();
        Win95StoreProcessAttach();
        RoamingStoreProcessAttach();
#endif
        for (i = 0; i < FUNC_SET_COUNT; i++) {
            if (NULL == (rghFuncSet[i] = CryptInitOIDFunctionSet(
                rgpszFuncName[i], 0)))
            goto CryptInitOIDFunctionSetError;
        }
        if (NULL == (hTlsEnumPhysicalStoreDepth = I_CryptAllocTls()))
            goto CryptAllocTlsError;
        break;

    case DLL_PROCESS_DETACH:
#ifndef CE_BUILD    
        RoamingStoreProcessDetach();
        Win95StoreProcessDetach();
        GptStoreProcessDetach();
        RegWaitForProcessDetach();
#endif        
        DeleteCriticalSection(&ILS_CriticalSection);
        I_CryptFreeTls(hTlsEnumPhysicalStoreDepth, PkiFree);
        break;

    case DLL_THREAD_DETACH:
        PkiFree(I_CryptDetachTls(hTlsEnumPhysicalStoreDepth));
        break;
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
TRACE_ERROR(CryptAllocTlsError)
}

//+-------------------------------------------------------------------------
//  Converts the bytes into UNICODE ASCII HEX
//
//  Needs (cb * 2 + 1) * sizeof(WCHAR) bytes of space in wsz
//--------------------------------------------------------------------------
STATIC void BytesToWStr(DWORD cb, void* pv, LPWSTR wsz)
{
    BYTE* pb = (BYTE*) pv;
    for (DWORD i = 0; i<cb; i++) {
        int b;
        b = (*pb & 0xF0) >> 4;
        *wsz++ = (b <= 9) ? b + L'0' : (b - 10) + L'A';
        b = *pb & 0x0F;
        *wsz++ = (b <= 9) ? b + L'0' : (b - 10) + L'A';
        pb++;
    }
    *wsz++ = 0;
}

//+-------------------------------------------------------------------------
//  Converts the UNICODE ASCII HEX to an array of bytes
//--------------------------------------------------------------------------
STATIC void WStrToBytes(
    IN const WCHAR wsz[MAX_HASH_NAME_LEN],
    OUT BYTE rgb[MAX_HASH_LEN],
    OUT DWORD *pcb
    )
{
    BOOL fUpperNibble = TRUE;
    DWORD cb = 0;
    LPCWSTR pwsz = wsz;
    WCHAR wch;

    while (cb < MAX_HASH_LEN && (wch = *pwsz++)) {
        BYTE b;

        // only convert ascii hex characters 0..9, a..f, A..F
        // silently ignore all others
        if (wch >= L'0' && wch <= L'9')
            b = wch - L'0';
        else if (wch >= L'a' && wch <= L'f')
            b = 10 + wch - L'a';
        else if (wch >= L'A' && wch <= L'F')
            b = 10 + wch - L'A';
        else
            continue;

        if (fUpperNibble) {
            rgb[cb] = b << 4;
            fUpperNibble = FALSE;
        } else {
            rgb[cb] = rgb[cb] | b;
            cb++;
            fUpperNibble = TRUE;
        }
    }

    *pcb = cb;
}

//+-------------------------------------------------------------------------
//  Lock and unlock registry functions
//--------------------------------------------------------------------------
static inline void LockRegStore(IN PREG_STORE pRegStore)
{
    EnterCriticalSection(&pRegStore->CriticalSection);
}
static inline void UnlockRegStore(IN PREG_STORE pRegStore)
{
    LeaveCriticalSection(&pRegStore->CriticalSection);
}

//+-------------------------------------------------------------------------
//  Checks if current thread is doing a Resync. Other threads block until
//  the resync completes
//--------------------------------------------------------------------------
STATIC BOOL IsInResync(IN PREG_STORE pRegStore)
{
    BOOL fResync;

    LockRegStore(pRegStore);
    fResync = pRegStore->fResync;
    UnlockRegStore(pRegStore);
    return fResync;
}

//+=========================================================================
//  Low level registry support functions
//==========================================================================

// LastError can get globbered when doing remote registry access
void
ILS_CloseRegistryKey(
    IN HKEY hKey
    )
{
#ifndef _XBOX
    if (hKey) {
        DWORD dwErr = GetLastError();
        LONG RegCloseKeyStatus;
        RegCloseKeyStatus = RegCloseKey(hKey);
        assert(ERROR_SUCCESS == RegCloseKeyStatus);
        SetLastError(dwErr);
    }
#endif
}

STATIC BOOL WriteDWORDValueToRegistry(
    IN HKEY hKey,
    IN LPCWSTR pwszValueName,
    IN DWORD dwValue
    )
{
#ifndef _XBOX
    LONG err;
    if (ERROR_SUCCESS == (err = RegSetValueExU(
            hKey,
            pwszValueName,
            0,          // dwReserved
            REG_DWORD,
            (BYTE *) &dwValue,
            sizeof(DWORD))))
        return TRUE;
    else {
        SetLastError((DWORD) err);
        return FALSE;
    }
#else
	return TRUE;
#endif
}

BOOL
ILS_ReadDWORDValueFromRegistry(
    IN HKEY hKey,
    IN LPCWSTR pwszValueName,
    IN DWORD *pdwValue
    )
{
#ifndef _XBOX
    BOOL fResult;
    LONG err;
    DWORD dwType;
    DWORD dwValue;
    DWORD cbValue = sizeof(DWORD);

    if (ERROR_SUCCESS != (err = RegQueryValueExU(
            hKey,
            pwszValueName,
            NULL,       // pdwReserved
            &dwType,
            (BYTE *) &dwValue,
            &cbValue))) goto RegQueryValueError;
    if (dwType != REG_DWORD || cbValue != sizeof(DWORD))
        goto InvalidRegistryValue;
    fResult = TRUE;
CommonReturn:
    *pdwValue = dwValue;
    return fResult;
ErrorReturn:
    dwValue = 0;
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR_VAR(RegQueryValueError, err)
SET_ERROR(InvalidRegistryValue, CRYPT_E_FILE_ERROR)
#else
	return TRUE;
#endif
}

// Ensure a binary value that may contain a LPCWSTR is NULL terminated.
// Always adds a NULL terminator not included in the returned cbValue.
//
// May return an allocated pbValue with a cbValue = 0.
BOOL
ILS_ReadBINARYValueFromRegistry(
    IN HKEY hKey,
    IN LPCWSTR pwszValueName,
    OUT BYTE **ppbValue,
    OUT DWORD *pcbValue
    )
{
#ifndef _XBOX
    BOOL fResult;
    LONG err;
    DWORD dwType;
    BYTE *pbValue = NULL;
    DWORD cbValue;
    DWORD cbAllocValue;

    err = RegQueryValueExU(
            hKey,
            pwszValueName,
            NULL,       // pdwReserved
            &dwType,
            NULL,       // lpData
            &cbValue);
    // For Win95 Remote Registry Access:: returns ERROR_MORE_DATA
    if (!(ERROR_SUCCESS == err || ERROR_MORE_DATA == err))
        goto RegQueryValueError;
    if (dwType != REG_BINARY)
        goto InvalidRegistryValue;
    cbAllocValue = cbValue + 3;
    if (NULL == (pbValue = (BYTE *) PkiNonzeroAlloc(cbAllocValue)))
        goto OutOfMemory;
    if (0 < cbValue) {
        if (ERROR_SUCCESS != (err = RegQueryValueExU(
                hKey,
                pwszValueName,
                NULL,       // pdwReserved
                &dwType,
                pbValue,
                &cbValue))) goto RegQueryValueError;
    }
    assert(cbAllocValue >= cbValue + 3);

    // Ensure an LPWSTR is null terminated
    memset(pbValue + cbValue, 0, 3);

    fResult = TRUE;
CommonReturn:
    *ppbValue = pbValue;
    *pcbValue = cbValue;
    return fResult;
ErrorReturn:
    fResult = FALSE;
    PkiFree(pbValue);
    pbValue = NULL;
    cbValue = 0;
    goto CommonReturn;

SET_ERROR_VAR(RegQueryValueError, err)
SET_ERROR(InvalidRegistryValue, CRYPT_E_FILE_ERROR)
TRACE_ERROR(OutOfMemory)
#else
	return TRUE;
#endif
}

//+-------------------------------------------------------------------------
//  Get and allocate the REG_SZ value
//--------------------------------------------------------------------------
STATIC LPWSTR ReadSZValueFromRegistry(
    IN HKEY hKey,
    IN LPCWSTR pwszValueName
    )
{
#ifndef _XBOX
    LONG err;
    DWORD dwType;
    LPWSTR pwszValue = NULL;
    DWORD cbValue;

    err = RegQueryValueExU(
            hKey,
            pwszValueName,
            NULL,       // pdwReserved
            &dwType,
            NULL,       // lpData
            &cbValue);
    // For Win95 Remote Registry Access:: returns ERROR_MORE_DATA
    if (!(ERROR_SUCCESS == err || ERROR_MORE_DATA == err))
        goto RegQueryValueError;
    if (dwType != REG_SZ || cbValue == 0)
        goto InvalidRegistryValue;
    if (NULL == (pwszValue = (LPWSTR) PkiNonzeroAlloc(cbValue)))
        goto OutOfMemory;
    if (ERROR_SUCCESS != (err = RegQueryValueExU(
            hKey,
            pwszValueName,
            NULL,       // pdwReserved
            &dwType,
            (BYTE *) pwszValue,
            &cbValue))) goto RegQueryValueError;
CommonReturn:
    return pwszValue;
ErrorReturn:
    PkiFree(pwszValue);
    pwszValue = NULL;
    goto CommonReturn;

SET_ERROR_VAR(RegQueryValueError, err)
SET_ERROR(InvalidRegistryValue, CRYPT_E_FILE_ERROR)
TRACE_ERROR(OutOfMemory)
#else
	return NULL;
#endif
}

#ifndef CE_BUILD
STATIC LPSTR ReadSZValueFromRegistry(
    IN HKEY hKey,
    IN LPCSTR pszValueName
    )
{
    LONG err;
    DWORD dwType;
    LPSTR pszValue = NULL;
    DWORD cbValue;

    err = RegQueryValueExA(
            hKey,
            pszValueName,
            NULL,       // pdwReserved
            &dwType,
            NULL,       // lpData
            &cbValue);
    // For Win95 Remote Registry Access:: returns ERROR_MORE_DATA
    if (!(ERROR_SUCCESS == err || ERROR_MORE_DATA == err))
        goto RegQueryValueError;
    if (dwType != REG_SZ || cbValue == 0)
        goto InvalidRegistryValue;
    if (NULL == (pszValue = (LPSTR) PkiNonzeroAlloc(cbValue)))
        goto OutOfMemory;
    if (ERROR_SUCCESS != (err = RegQueryValueExA(
            hKey,
            pszValueName,
            NULL,       // pdwReserved
            &dwType,
            (BYTE *) pszValue,
            &cbValue))) goto RegQueryValueError;
CommonReturn:
    return pszValue;
ErrorReturn:
    PkiFree(pszValue);
    pszValue = NULL;
    goto CommonReturn;

SET_ERROR_VAR(RegQueryValueError, err)
SET_ERROR(InvalidRegistryValue, CRYPT_E_FILE_ERROR)
TRACE_ERROR(OutOfMemory)
}

#endif

STATIC BOOL GetSubKeyInfo(
    IN HKEY hKey,
    OUT OPTIONAL DWORD *pcSubKeys,
    OUT OPTIONAL DWORD *pcchMaxSubKey = NULL
    )
{
    BOOL fResult;
    LONG err;
    if (ERROR_SUCCESS != (err = RegQueryInfoKeyU(
            hKey,
            NULL,       // lpszClass
            NULL,       // lpcchClass
            NULL,       // lpdwReserved
            pcSubKeys,
            pcchMaxSubKey,
            NULL,       // lpcchMaxClass
            NULL,       // lpcValues
            NULL,       // lpcchMaxValuesName
            NULL,       // lpcbMaxValueData
            NULL,       // lpcbSecurityDescriptor
            NULL        // lpftLastWriteTime
            ))) goto RegQueryInfoKeyError;
    fResult = TRUE;

CommonReturn:
    // For Win95 Remote Registry Access:: returns half of the cch
    if (pcchMaxSubKey && *pcchMaxSubKey)
        *pcchMaxSubKey = (*pcchMaxSubKey + 1) * 2 + 2;
    return fResult;
ErrorReturn:
    fResult = FALSE;
    if (pcSubKeys)
        *pcSubKeys = 0;
    if (pcchMaxSubKey)
        *pcchMaxSubKey = 0;
    goto CommonReturn;
SET_ERROR_VAR(RegQueryInfoKeyError, err)
}

//+-------------------------------------------------------------------------
//  Open the SubKey.
//
//  dwFlags:
//      CERT_STORE_READONLY_FLAG
//      CERT_STORE_OPEN_EXISTING_FLAG
//      CERT_STORE_CREATE_NEW_FLAG
//--------------------------------------------------------------------------
STATIC HKEY OpenSubKey(
    IN HKEY hKey,
    IN LPCWSTR pwszSubKeyName,
    IN DWORD dwFlags
    )
{
#ifndef _XBOX
    LONG err;
    HKEY hSubKey;

    if (dwFlags & CERT_STORE_CREATE_NEW_FLAG) {
        // First check if SubKey already exists
        if (hSubKey = OpenSubKey(
                hKey,
                pwszSubKeyName,
                (dwFlags & ~CERT_STORE_CREATE_NEW_FLAG) |
                    CERT_STORE_OPEN_EXISTING_FLAG |
                    CERT_STORE_READONLY_FLAG
                )) {
            RegCloseKey(hSubKey);
            goto ExistingSubKey;
        } else if (ERROR_FILE_NOT_FOUND != GetLastError())
            goto OpenNewSubKeyError;
    }

    if (dwFlags & (CERT_STORE_READONLY_FLAG | CERT_STORE_OPEN_EXISTING_FLAG)) {
        REGSAM samDesired;
        if (dwFlags & CERT_STORE_READONLY_FLAG)
            samDesired = KEY_READ;
        else
            samDesired = KEY_ALL_ACCESS;

        if (ERROR_SUCCESS != (err = RegOpenHKCUKeyExU(
                hKey,
                pwszSubKeyName,
                0,                      // dwReserved
                samDesired,
                &hSubKey)))
            goto RegOpenKeyError;
    } else {
        DWORD dwDisposition;
        if (ERROR_SUCCESS != (err = RegCreateHKCUKeyExU(
                hKey,
                pwszSubKeyName,
                0,                      // dwReserved
                NULL,                   // lpClass
                REG_OPTION_NON_VOLATILE,
                KEY_ALL_ACCESS,
                NULL,                   // lpSecurityAttributes
                &hSubKey,
                &dwDisposition)))
            goto RegCreateKeyError;
    }

CommonReturn:
    return hSubKey;
ErrorReturn:
    hSubKey = NULL;
    goto CommonReturn;

SET_ERROR_VAR(ExistingSubKey, ERROR_FILE_EXISTS)
TRACE_ERROR(OpenNewSubKeyError)
SET_ERROR_VAR(RegOpenKeyError, err)
SET_ERROR_VAR(RegCreateKeyError, err)
#else
	ASSERT(!"OpenSubKey called");
	return NULL;
#endif
}


STATIC BOOL RecursiveDeleteSubKey(
    IN HKEY hKey,
    IN LPCWSTR pwszSubKeyName
    )
{
#ifndef _XBOX
    BOOL fResult;
    LONG err;

    while (TRUE) {
        HKEY hSubKey;
        DWORD cSubKeys;
        DWORD cchMaxSubKey;
        BOOL fDidDelete;

        if (ERROR_SUCCESS != RegOpenHKCUKeyExU(
                hKey,
                pwszSubKeyName,
                0,                      // dwReserved
                KEY_ALL_ACCESS,
                &hSubKey))
            break;

        GetSubKeyInfo(
            hSubKey,
            &cSubKeys,
            &cchMaxSubKey
            );

        fDidDelete = FALSE;
        if (cSubKeys && cchMaxSubKey) {
            LPWSTR pwszEnumSubKeyName;
            cchMaxSubKey++;

            if (pwszEnumSubKeyName = (LPWSTR) PkiNonzeroAlloc(
                    cchMaxSubKey * sizeof(WCHAR))) {
                if (ERROR_SUCCESS == RegEnumKeyExU(
                        hSubKey,
                        0,
                        pwszEnumSubKeyName,
                        &cchMaxSubKey,
                        NULL,               // lpdwReserved
                        NULL,               // lpszClass
                        NULL,               // lpcchClass
                        NULL                // lpftLastWriteTime
                        ))
                    fDidDelete = RecursiveDeleteSubKey(
                        hSubKey, pwszEnumSubKeyName);
                PkiFree(pwszEnumSubKeyName);
            }
        }
        RegCloseKey(hSubKey);
        if (!fDidDelete)
            break;
    }

    if (ERROR_SUCCESS != (err = RegDeleteKeyU(hKey, pwszSubKeyName)))
        goto RegDeleteKeyError;
    fResult = TRUE;
CommonReturn:
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR_VAR(RegDeleteKeyError, err)
#else
	ASSERT(!"RecursiveDeleteSubKey called");
	return NULL;
#endif
}

//+=========================================================================
//  Win95 Registry Functions
//
//  Note, as of 10/17/97 the following is also done on NT to allow
//  registry hive roaming from NT to Win95 systems.
//
//  Certs, CRLs and CTLs are stored in SubKeys instead of as Key values.
//
//  Note: Win95 has the following registry limitations:
//   - Max single key value is 16K
//   - Max total values per key is 64K
//
//  For WIN95, write each cert, CRL, CTL to its own key when the
//  above limitations are exceeded. If encoded blob exceeds 12K, partition
//  and write to multiple SubKeys. Blobs are written to values named
//  "Blob". Partitioned blobs, have "BlobCount" and "BlobLength" values and
//  SubKeys named "Blob0", "Blob1", "Blob2" ... .
//
//  The IE4.0 version of crypt32 wrote the blob to a "File" if the blob
//  exceeded 12K. For backwards compatibility, continue to read "File" based
//  blobs. On write enabled, non-remote opens, "File" blobs are moved to
//  "Blob0", ... SubKeys and the file is deleted.
//
//  If CERT_REGISTRY_STORE_SERIALIZED_FLAG is set when the registry store
//  is opened, then, the entire store resides in a partitioned blob under the
//  "Serialized" subkey.
//==========================================================================
#define KEY_BLOB_VALUE_NAME             L"Blob"
#define KEY_FILE_VALUE_NAME             L"File"
#define KEY_BLOB_COUNT_VALUE_NAME       L"BlobCount"
#define KEY_BLOB_LENGTH_VALUE_NAME      L"BlobLength"
#define KEY_BLOB_N_SUBKEY_PREFIX        "Blob"
#define KEY_BLOB_N_SUBKEY_PREFIX_LENGTH 4
#define SYSTEM_STORE_SUBDIR             L"SystemCertificates"
#define FILETIME_ASCII_HEX_LEN          (2 * sizeof(FILETIME) + 1)
#define MAX_KEY_BLOB_VALUE_LEN          0x3000
#define MAX_NEW_FILE_CREATE_ATTEMPTS    100

#define SERIALIZED_SUBKEY_NAME          L"Serialized"


//+-------------------------------------------------------------------------
//  Read and allocate the element bytes by reading the file pointed to
//  by the SubKey's "File" value
//--------------------------------------------------------------------------
STATIC BOOL ReadKeyFileElementFromRegistry(
    IN HKEY hSubKey,
    OUT BYTE **ppbElement,
    OUT DWORD *pcbElement
    )
{
#ifndef _XBOX
    BOOL fResult;
    LPWSTR pwszFilename = NULL;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD cbBytesRead;
    BYTE *pbElement = NULL;
    DWORD cbElement;

    if (NULL == (pwszFilename = ReadSZValueFromRegistry(
            hSubKey, KEY_FILE_VALUE_NAME)))
        goto GetKeyFilenameError;

    if (INVALID_HANDLE_VALUE == (hFile = CreateFileU(
              pwszFilename,
              GENERIC_READ,
              FILE_SHARE_READ,
              NULL,                   // lpsa
              OPEN_EXISTING,
              FILE_ATTRIBUTE_NORMAL,
              NULL                    // hTemplateFile
              )))
        goto CreateFileError;

    cbElement = GetFileSize(hFile, NULL);
    if (0xFFFFFFFF == cbElement) goto FileError;
    if (0 == cbElement) goto EmptyFile;
    if (NULL == (pbElement = (BYTE *) PkiNonzeroAlloc(cbElement)))
        goto OutOfMemory;
    if (!ReadFile(
            hFile,
            pbElement,
            cbElement,
            &cbBytesRead,
            NULL            // lpOverlapped
            )) goto FileError;

    fResult = TRUE;
CommonReturn:
    PkiFree(pwszFilename);
    if (INVALID_HANDLE_VALUE != hFile)
        CloseHandle(hFile);
    *ppbElement = pbElement;
    *pcbElement = cbElement;
    return fResult;
ErrorReturn:
    fResult = FALSE;
    PkiFree(pbElement);
    pbElement = NULL;
    cbElement = 0;
    goto CommonReturn;

TRACE_ERROR(GetKeyFilenameError)
TRACE_ERROR(OutOfMemory)
TRACE_ERROR(CreateFileError)
TRACE_ERROR(FileError)
SET_ERROR(EmptyFile, CRYPT_E_FILE_ERROR)
#else
	ASSERT(!"ReadKeyFileElementFromRegistry called");
	return NULL;
#endif
}

//+-------------------------------------------------------------------------
//  Read as multiple SubKeys containing the element bytes. The SubKeys
//  are named Blob0, Blob1, Blob2, ... BlobN.
//  Each BlobN SubKey has a value named "Blob" containing the bytes to be read.
//
//  The passed in SubKey is expected to have 2 values:
//      BlobCount - # of BlobN SubKeys
//      BlobLength - total length of all the concatenated Blob Subkey bytes
//
//  A single allocated element byte array is returned.
//--------------------------------------------------------------------------
STATIC BOOL ReadMultipleKeyBlobsFromRegistry(
    IN HKEY hSubKey,
    OUT BYTE **ppbElement,
    OUT DWORD *pcbElement
    )
{
    BOOL fResult;
    LONG err;
    HKEY hBlobKey = NULL;
    BYTE *pbElement = NULL;
    DWORD cbElement;
    DWORD BlobCount;
    DWORD BlobLength;
    DWORD i;
    char szBlobN[KEY_BLOB_N_SUBKEY_PREFIX_LENGTH + 33];

    ILS_ReadDWORDValueFromRegistry(
            hSubKey,
            KEY_BLOB_COUNT_VALUE_NAME,
            &BlobCount
            );
    ILS_ReadDWORDValueFromRegistry(
            hSubKey,
            KEY_BLOB_LENGTH_VALUE_NAME,
            &BlobLength
            );

    if (0 == BlobCount || 0 == BlobLength)
        goto NoMultipleKeyBlobs;

    if (NULL == (pbElement = (BYTE *) PkiNonzeroAlloc(BlobLength)))
        goto OutOfMemory;

    cbElement = 0;
    strcpy(szBlobN, KEY_BLOB_N_SUBKEY_PREFIX);
    for (i = 0; i < BlobCount; i++) {
        DWORD cbData;
        DWORD dwType;

        _ltoa((long) i, szBlobN + KEY_BLOB_N_SUBKEY_PREFIX_LENGTH, 10);
        if (ERROR_SUCCESS != (err = RegOpenKeyExA(
                hSubKey,
                szBlobN,
                0,                  // dwReserved
                KEY_READ,
                &hBlobKey)))
            goto OpenBlobNError;

        cbData = BlobLength - cbElement;
        if (0 == cbData)
            goto ExtraMultipleKeyBlobs;

        if (ERROR_SUCCESS != (err = RegQueryValueExU(
                hBlobKey,
                KEY_BLOB_VALUE_NAME,
                NULL,       // pdwReserved
                &dwType,
                pbElement + cbElement,
                &cbData)))
            goto RegQueryValueError;
        if (dwType != REG_BINARY)
            goto InvalidRegistryValue;

        cbElement += cbData;
        if (cbElement > BlobLength)
            goto UnexpectedError;

        RegCloseKey(hBlobKey);
        hBlobKey = NULL;
    }

    if (cbElement != BlobLength)
        goto MissingMultipleKeyBlobsBytes;

    assert(NULL == hBlobKey);

    fResult = TRUE;
CommonReturn:
    *ppbElement = pbElement;
    *pcbElement = cbElement;
    return fResult;
ErrorReturn:
    PkiFree(pbElement);
    ILS_CloseRegistryKey(hBlobKey);
    fResult = FALSE;
    pbElement = NULL;
    cbElement = 0;
    goto CommonReturn;

SET_ERROR(NoMultipleKeyBlobs, ERROR_FILE_NOT_FOUND)
TRACE_ERROR(OutOfMemory)
SET_ERROR_VAR(OpenBlobNError, err)
SET_ERROR(UnexpectedError, E_UNEXPECTED)
SET_ERROR(InvalidRegistryValue, CRYPT_E_FILE_ERROR)
SET_ERROR(ExtraMultipleKeyBlobs, CRYPT_E_FILE_ERROR)
SET_ERROR_VAR(RegQueryValueError, err)
SET_ERROR(MissingMultipleKeyBlobsBytes, CRYPT_E_FILE_ERROR)
}

//+-------------------------------------------------------------------------
//  Write as multiple BlobN SubKeys containing the element bytes.
//
//  See ReadMultipleKeyBlobsFromRegistry() for details.
//--------------------------------------------------------------------------
STATIC BOOL WriteMultipleKeyBlobsToRegistry(
    IN HKEY hSubKey,
    IN const BYTE *pbElement,
    IN DWORD cbElement
    )
{
    BOOL fResult;
    LONG err;
    HKEY hBlobKey = NULL;
    DWORD BlobCount = 0;
    DWORD BlobLength;
    DWORD i;
    char szBlobN[KEY_BLOB_N_SUBKEY_PREFIX_LENGTH + 33];

    if (0 == cbElement)
        goto UnexpectedError;
    BlobCount = cbElement / MAX_KEY_BLOB_VALUE_LEN;
    if (cbElement % MAX_KEY_BLOB_VALUE_LEN)
        BlobCount++;

    BlobLength = 0;
    strcpy(szBlobN, KEY_BLOB_N_SUBKEY_PREFIX);
    for (i = 0; i < BlobCount; i++) {
        DWORD cbData;
        DWORD dwDisposition;

        _ltoa((long) i, szBlobN + KEY_BLOB_N_SUBKEY_PREFIX_LENGTH, 10);
        if (ERROR_SUCCESS != (err = RegCreateKeyExA(
                hSubKey,
                szBlobN,
                0,                      // dwReserved
                NULL,                   // lpClass
                REG_OPTION_NON_VOLATILE,
                KEY_WRITE,
                NULL,                   // lpSecurityAttributes
                &hBlobKey,
                &dwDisposition))) goto RegCreateKeyError;

        assert(cbElement > BlobLength);
        cbData = cbElement - BlobLength;
        if (cbData > MAX_KEY_BLOB_VALUE_LEN)
            cbData = MAX_KEY_BLOB_VALUE_LEN;

        if (ERROR_SUCCESS != (err = RegSetValueExU(
                hBlobKey,
                KEY_BLOB_VALUE_NAME,
                NULL,
                REG_BINARY,
                pbElement + BlobLength,
                cbData))) goto RegSetValueError;

        BlobLength += cbData;

        RegCloseKey(hBlobKey);
        hBlobKey = NULL;
    }

    assert(BlobLength == cbElement);

    if (!WriteDWORDValueToRegistry(
            hSubKey,
            KEY_BLOB_COUNT_VALUE_NAME,
            BlobCount))
        goto WriteDWORDError;
    if (!WriteDWORDValueToRegistry(
            hSubKey,
            KEY_BLOB_LENGTH_VALUE_NAME,
            BlobLength))
        goto WriteDWORDError;

    assert(NULL == hBlobKey);
    fResult = TRUE;

CommonReturn:
    return fResult;
ErrorReturn:
    ILS_CloseRegistryKey(hBlobKey);
    for (i = 0; i < BlobCount; i++) {
        _ltoa((long) i, szBlobN + KEY_BLOB_N_SUBKEY_PREFIX_LENGTH, 10);
        RegDeleteKeyA(hSubKey, szBlobN);
    }
    RegDeleteValueU(hSubKey, KEY_BLOB_COUNT_VALUE_NAME);
    RegDeleteValueU(hSubKey, KEY_BLOB_LENGTH_VALUE_NAME);
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(UnexpectedError, E_UNEXPECTED)
SET_ERROR_VAR(RegCreateKeyError, err)
SET_ERROR_VAR(RegSetValueError, err)
TRACE_ERROR(WriteDWORDError)
}

//+-------------------------------------------------------------------------
//  If the SubKey has a "File" value, delete the file.
//--------------------------------------------------------------------------
STATIC void DeleteKeyFile(
    IN HKEY hKey,
    IN const WCHAR wszSubKeyName[MAX_CERT_REG_VALUE_NAME_LEN]
    )
{
    HKEY hSubKey;

    if (ERROR_SUCCESS == RegOpenHKCUKeyExU(
            hKey,
            wszSubKeyName,
            0,
            KEY_ALL_ACCESS,
            &hSubKey)) {
        LPWSTR pwszFilename;
        if (pwszFilename = ReadSZValueFromRegistry(hSubKey,
                KEY_FILE_VALUE_NAME)) {
            SetFileAttributesU(pwszFilename, FILE_ATTRIBUTE_NORMAL);
            DeleteFileU(pwszFilename);
            PkiFree(pwszFilename);
        }
        RegDeleteValueU(hSubKey, KEY_FILE_VALUE_NAME);
        RegCloseKey(hSubKey);
    }
}

//+-------------------------------------------------------------------------
//  Get the context by either getting the SubKey's "Blob" value or getting
//  the SubKey's "BlobCount" and "BlobLength" values and then
//  reading and concatenating multiple Blob<N> SubKeys containing the bytes or
//  reading the file pointed to by the SubKey's "File" value.
//
//  If the "File" value is found and used, then, migrate to being stored
//  in the registry using multiple Blob<N> SubKeys.
//
//  If CERT_REGISTRY_STORE_REMOTE_FLAG is set, then, don't attempt to read
//  from the file.
//
//  If CERT_STORE_READONLY_FLAG is set, don't attempt to migrate from the
//  "File".
//--------------------------------------------------------------------------
STATIC BOOL ReadKeyElementFromRegistry(
        IN HKEY hKey,
        IN const WCHAR wszSubKeyName[MAX_CERT_REG_VALUE_NAME_LEN],
        IN DWORD dwFlags,
        OUT BYTE **ppbElement,
        OUT DWORD *pcbElement
        )
{
    LONG err;
    BOOL fResult;
    BYTE *pbElement = NULL;
    DWORD cbElement;
    HKEY hSubKey = NULL;

    if (ERROR_SUCCESS != (err = RegOpenHKCUKeyExU(
            hKey,
            wszSubKeyName,
            0,                      // dwReserved
            KEY_READ,
            &hSubKey)))
        goto RegOpenKeyError;

    fResult = ILS_ReadBINARYValueFromRegistry(hSubKey, KEY_BLOB_VALUE_NAME,
         &pbElement, &cbElement);
    if (!fResult || 0 == cbElement) {
        PkiFree(pbElement);

        fResult = ReadMultipleKeyBlobsFromRegistry(hSubKey, &pbElement,
            &cbElement);
        if (!fResult && 0 == (dwFlags & CERT_REGISTRY_STORE_REMOTE_FLAG)) {
            // For backwards compatibility with IE4.0. See if it exists
            // in a file
            fResult = ReadKeyFileElementFromRegistry(hSubKey, &pbElement,
                &cbElement);
            if (fResult && 0 == (dwFlags & CERT_STORE_READONLY_FLAG)) {
                // Move from the file back to the registry.
                if (WriteMultipleKeyBlobsToRegistry(hSubKey, pbElement,
                        cbElement))
                    DeleteKeyFile(hKey, wszSubKeyName);
            }
        }

        if (!fResult)
            goto ReadKeyElementError;
    }

    fResult = TRUE;
CommonReturn:
    ILS_CloseRegistryKey(hSubKey);
    *ppbElement = pbElement;
    *pcbElement = cbElement;

    return fResult;
ErrorReturn:
    fResult = FALSE;
    PkiFree(pbElement);
    pbElement = NULL;
    cbElement = 0;
    goto CommonReturn;

SET_ERROR_VAR(RegOpenKeyError, err)
TRACE_ERROR(ReadKeyElementError)
}

STATIC BOOL ReadKeyFromRegistry(
        IN HKEY hKey,
        IN const WCHAR wszSubKeyName[MAX_CERT_REG_VALUE_NAME_LEN],
        IN HCERTSTORE hCertStore,
        IN DWORD dwContextTypeFlags,
        IN DWORD dwFlags,
        OUT OPTIONAL const void **ppvContext
        )
{
    BOOL fResult;
    BYTE *pbElement = NULL;
    DWORD cbElement;

    if (!ReadKeyElementFromRegistry(
            hKey,
            wszSubKeyName,
            dwFlags,
            &pbElement,
            &cbElement
            ))
        goto ErrorReturn;

    if (!CertAddSerializedElementToStore(
            hCertStore,
            pbElement,
            cbElement,
            CERT_STORE_ADD_ALWAYS,
            0,                              // dwFlags
            dwContextTypeFlags,
            NULL,                           // pdwContextType
            ppvContext))
        goto AddSerializedElementError;

    fResult = TRUE;
CommonReturn:
    PkiFree(pbElement);

    return fResult;
ErrorReturn:
    fResult = FALSE;
    if (ppvContext)
        *ppvContext = NULL;
    goto CommonReturn;

TRACE_ERROR(AddSerializedElementError)
}


//+-------------------------------------------------------------------------
//  Get the Certificates, CRLs or CTLs from the registry by reading as
//  SubKeys and not Key values as done by OpenFromRegistry.
//
//  If CERT_STORE_DELETE_FLAG is set, delete the file, if stored there.
//
//  If CERT_REGISTRY_STORE_REMOTE_FLAG is set, then, don't attempt to read
//  from the file.
//
//  If CERT_STORE_READONLY_FLAG is set, don't attempt to migrate from the
//  "File".
//--------------------------------------------------------------------------
STATIC BOOL OpenKeysFromRegistry(
    IN HCERTSTORE hCertStore,
    IN HKEY hKey,
    IN DWORD dwFlags
    )
{
    BOOL fResult;
    LONG err;
    DWORD cSubKeys;
    DWORD i;

    // see how many SubKeys in the registry
    if (!GetSubKeyInfo(hKey, &cSubKeys))
        goto GetSubKeyInfoError;

    for (i = 0; i < cSubKeys; i++) {
        WCHAR wszSubKeyName[MAX_CERT_REG_VALUE_NAME_LEN];
        DWORD cchSubKeyName = MAX_CERT_REG_VALUE_NAME_LEN;
        err = RegEnumKeyExU(
            hKey,
            i,
            wszSubKeyName,
            &cchSubKeyName,
            NULL,               // lpdwReserved
            NULL,               // lpszClass
            NULL,               // lpcchClass
            NULL                // lpftLastWriteTime
            );
        if (ERROR_SUCCESS != err)
            continue;
        else if (dwFlags & CERT_STORE_DELETE_FLAG) {
            if (0 == (dwFlags & CERT_REGISTRY_STORE_REMOTE_FLAG))
                DeleteKeyFile(hKey, wszSubKeyName);
        } else
            // Ignore any read errors
            ReadKeyFromRegistry(
                hKey,
                wszSubKeyName,
                hCertStore,
                CERT_STORE_ALL_CONTEXT_FLAG,
                dwFlags,
                NULL                            // ppvContext
                );
    }
    fResult = TRUE;

CommonReturn:
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
TRACE_ERROR(GetSubKeyInfoError)
}

#if 0
//
// The following was done in IE4.0 on Win95
//

//+-------------------------------------------------------------------------
//  Create the filename to contain the encoded element. The filename will
//  be something like:
//      C:\Windows\SystemCertificates\
//          00112233445566778899AABBCCDDEEFF00112233.0011223344556677
//  Where:
//      C:\Windows              - obtained via GetWindowsDirectory
//      SystemCertificates      - subdirectory containing all file elements
//      00112233445566778899AABBCCDDEEFF00112233
//                              - wszSubKeyName (ascii hex sha1)
//      0011223344556677        - ascii hex of current filetime
//
//
//  In addition to creating the filename, also creates the
//  "SystemCertificates" directory under C:\Windows.
//--------------------------------------------------------------------------
STATIC LPWSTR CreateKeyFilename(
    IN const WCHAR wszSubKeyName[MAX_CERT_REG_VALUE_NAME_LEN],
    IN LPFILETIME pft
    )
{
    LPWSTR pwszWindowsDir = NULL;
    DWORD cchWindowsDir;
    WCHAR rgwc[1];

    BYTE rgbft[sizeof(FILETIME)];
    WCHAR wszft[FILETIME_ASCII_HEX_LEN];

    LPWSTR pwszFilename = NULL;
    DWORD cchFilename;

    if (0 == (cchWindowsDir = GetWindowsDirectoryU(rgwc, 1)))
        goto GetWindowsDirError;
    cchWindowsDir++;    // bump to include null terminator
    if (NULL == (pwszWindowsDir = (LPWSTR) PkiNonzeroAlloc(
            cchWindowsDir * sizeof(WCHAR))))
        goto OutOfMemory;
    if (0 == GetWindowsDirectoryU(pwszWindowsDir, cchWindowsDir))
        goto GetWindowsDirError;

    // Convert filetime to ascii hex. First reverse the filetime bytes.
    memcpy(rgbft, pft, sizeof(rgbft));
    PkiAsn1ReverseBytes(rgbft, sizeof(rgbft));
    BytesToWStr(sizeof(rgbft), rgbft, wszft);

    // Get total length of filename and allocate
    cchFilename = cchWindowsDir + 1 +
        wcslen(SYSTEM_STORE_SUBDIR) + 1 +
        MAX_CERT_REG_VALUE_NAME_LEN + 1 +
        FILETIME_ASCII_HEX_LEN + 1;
    if (NULL == (pwszFilename = (LPWSTR) PkiNonzeroAlloc(
            cchFilename * sizeof(WCHAR))))
        goto OutOfMemory;

    // Create C:\Windows\SystemCertificates directory if it doesn't already
    // exist
    wcscpy(pwszFilename, pwszWindowsDir);
    cchWindowsDir = wcslen(pwszWindowsDir);
    if (cchWindowsDir && L'\\' != pwszWindowsDir[cchWindowsDir - 1])
        wcscat(pwszFilename, L"\\");
    wcscat(pwszFilename, SYSTEM_STORE_SUBDIR);
    if (0xFFFFFFFF == GetFileAttributesU(pwszFilename)) {
        if (!CreateDirectoryU(
            pwszFilename,
            NULL            // lpsa
            )) goto CreateDirError;
    }

    // Append \<AsciiHexSubKeyName>.<AsciiHexFileTime> to the above directory
    // name to complete the filename string
    wcscat(pwszFilename, L"\\");
    wcscat(pwszFilename, wszSubKeyName);
    wcscat(pwszFilename, L".");
    wcscat(pwszFilename, wszft);

CommonReturn:
    PkiFree(pwszWindowsDir);
    return pwszFilename;
ErrorReturn:
    PkiFree(pwszFilename);
    pwszFilename = NULL;
    goto CommonReturn;
TRACE_ERROR(GetWindowsDirError)
TRACE_ERROR(OutOfMemory)
TRACE_ERROR(CreateDirError)
}

//+-------------------------------------------------------------------------
//  Write the bytes to a a file and update the SubKey's "File" value to
//  point to.
//
//  This code is here to show what was done in IE4.0.
//--------------------------------------------------------------------------
STATIC BOOL WriteKeyFileElementToRegistry(
    IN HKEY hSubKey,
    IN const WCHAR wszSubKeyName[MAX_CERT_REG_VALUE_NAME_LEN],
    IN BYTE *pbElement,
    IN DWORD cbElement
    )
{
    BOOL fResult;
    LONG err;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    LPWSTR pwszFilename = NULL;
    SYSTEMTIME st;
    FILETIME ft;
    DWORD i;

    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &ft);

    for (i = 0; i < MAX_NEW_FILE_CREATE_ATTEMPTS; i++) {
        DWORD cbBytesWritten;

        if (NULL == (pwszFilename = CreateKeyFilename(wszSubKeyName, &ft)))
            goto CreateKeyFilenameError;

        if (INVALID_HANDLE_VALUE == (hFile = CreateFileU(
                  pwszFilename,
                  GENERIC_WRITE,
                  0,                        // fdwShareMode
                  NULL,                     // lpsa
                  CREATE_NEW,
                  FILE_ATTRIBUTE_NORMAL,
                  NULL                      // hTemplateFile
                  ))) {
            if (ERROR_FILE_EXISTS != GetLastError())
                goto CreateFileError;
            else {
                PkiFree(pwszFilename);
                pwszFilename = NULL;
                *((_int64 *) &ft) += 1;
                continue;
            }
        }

        if (!WriteFile(
                hFile,
                pbElement,
                cbElement,
                &cbBytesWritten,
                NULL            // lpOverlapped
                )) goto WriteFileError;

        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
        if (!SetFileAttributesU(pwszFilename,
                FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY ))
            goto SetFileAttributesError;

        if (ERROR_SUCCESS != (err = RegSetValueExU(
                hSubKey,
                KEY_FILE_VALUE_NAME,
                NULL,
                REG_SZ,
                (BYTE *) pwszFilename,
                (wcslen(pwszFilename) + 1) * sizeof(WCHAR))))
            goto RegSetValueError;
        else
            goto SuccessReturn;
    }

    goto ExceededMaxFileCreateAttemptsError;

SuccessReturn:
    fResult = TRUE;
CommonReturn:
    if (INVALID_HANDLE_VALUE != hFile)
        CloseHandle(hFile);
    PkiFree(pwszFilename);

    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR_VAR(RegSetValueError, err)
TRACE_ERROR(CreateKeyFilenameError)
TRACE_ERROR(CreateFileError)
TRACE_ERROR(WriteFileError)
TRACE_ERROR(SetFileAttributesError)
SET_ERROR(ExceededMaxFileCreateAttemptsError, CRYPT_E_FILE_ERROR)

}

#endif  // end of IE4.0 "File" support

//+-------------------------------------------------------------------------
//  If the length of the element is <= MAX_KEY_BLOB_VALUE_LEN, then,
//  write it as the SubKey's "Blob" value. Otherwise, write it as multiple
//  SubKeys each containing a "Blob" value no larger than
//  MAX_KEY_BLOB_VALUE_LEN.
//--------------------------------------------------------------------------
STATIC BOOL WriteKeyToRegistry(
        IN HKEY hKey,
        IN const WCHAR wszSubKeyName[MAX_CERT_REG_VALUE_NAME_LEN],
        IN const BYTE *pbElement,
        IN DWORD cbElement
        )
{
    BOOL fResult;
    LONG err;
    HKEY hSubKey = NULL;
    DWORD dwDisposition;

    if (ERROR_SUCCESS != (err = RegCreateHKCUKeyExU(
            hKey,
            wszSubKeyName,
            NULL,
            NULL,
            0,
            KEY_ALL_ACCESS,
            NULL,
            &hSubKey,
            &dwDisposition))) goto RegCreateKeyError;

    if (MAX_KEY_BLOB_VALUE_LEN >= cbElement) {
        // Write as a single "Blob" value
        if (ERROR_SUCCESS != (err = RegSetValueExU(
                hSubKey,
                KEY_BLOB_VALUE_NAME,
                NULL,
                REG_BINARY,
                pbElement,
                cbElement))) goto RegSetValueError;
    } else {
        // Write as a multiple Blob<N> SubKeys
        if (!WriteMultipleKeyBlobsToRegistry(hSubKey, pbElement, cbElement))
            goto WriteMultipleKeyBlobsError;
//        if (!WriteKeyFileElementToRegistry(wszSubKeyName, hSubKey, pbElement, cbElement))
//            goto ErrorReturn;
    }

    fResult = TRUE;
CommonReturn:
    ILS_CloseRegistryKey(hSubKey);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR_VAR(RegCreateKeyError, err)
SET_ERROR_VAR(RegSetValueError, err)
TRACE_ERROR(WriteMultipleKeyBlobsError)
}

//+=========================================================================
//  Registry Functions
//==========================================================================

//+-------------------------------------------------------------------------
//  First attempt to read as Key's value. If that fails for Win95, then,
//  read as a value in one or more SubKeys or as a
//  file with a SubKey pointing to it.
//
//  If CERT_REGISTRY_STORE_REMOTE_FLAG is set, then, don't attempt to read
//  from the file.
//
//  If CERT_STORE_READONLY_FLAG is set, don't attempt to migrate from the
//  "File".
//--------------------------------------------------------------------------
BOOL
ILS_ReadElementFromRegistry(
    IN HKEY hKey,
    IN LPCWSTR pwszContextName,
    IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
    IN DWORD dwFlags,
    OUT BYTE **ppbElement,
    OUT DWORD *pcbElement
    )
{
    LONG err;
    BOOL fResult;
    HKEY hSubKey = NULL;
    DWORD dwType;
    BYTE *pbElement = NULL;
    DWORD cbElement;

    if (pwszContextName) {
        if (NULL == (hSubKey = OpenSubKey(
                hKey,
                pwszContextName,
                dwFlags | CERT_STORE_READONLY_FLAG
                )))
            goto OpenSubKeyError;
    } else
        hSubKey = hKey;

    err = RegQueryValueExU(
            hSubKey,
            wszHashName,
            NULL,       // pdwReserved
            &dwType,
            NULL,       // lpData
            &cbElement);
    // For Win95 Remote Registry Access:: returns ERROR_MORE_DATA
    if (!(ERROR_SUCCESS == err || ERROR_MORE_DATA == err)) {
        fResult = ReadKeyElementFromRegistry(
            hSubKey,
            wszHashName,
            dwFlags,
            &pbElement,
            &cbElement
            );
        goto CommonReturn;
    }
    if (dwType != REG_BINARY || cbElement == 0)
        goto InvalidRegistryValue;
    if (NULL == (pbElement = (BYTE *) PkiNonzeroAlloc(cbElement)))
        goto OutOfMemory;
    if (ERROR_SUCCESS != (err = RegQueryValueExU(
            hSubKey,
            wszHashName,
            NULL,       // pdwReserved
            &dwType,
            pbElement,
            &cbElement))) goto RegQueryValueError;

    fResult = TRUE;
CommonReturn:
    if (pwszContextName)
        ILS_CloseRegistryKey(hSubKey);
    *ppbElement = pbElement;
    *pcbElement = cbElement;
    return fResult;
ErrorReturn:
    fResult = FALSE;
    PkiFree(pbElement);
    pbElement = NULL;
    cbElement = 0;
    goto CommonReturn;

TRACE_ERROR(OpenSubKeyError)
SET_ERROR_VAR(RegQueryValueError, err)
SET_ERROR(InvalidRegistryValue, CRYPT_E_FILE_ERROR)
TRACE_ERROR(OutOfMemory)
}

//+-------------------------------------------------------------------------
//  First delete as the Key's value. Then, for Win95 also delete as the
//  Key's SubKey and possibly file.
//--------------------------------------------------------------------------
BOOL
ILS_DeleteElementFromRegistry(
    IN HKEY hKey,
    IN LPCWSTR pwszContextName,
    IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
    IN DWORD dwFlags
    )
{
    BOOL fResult;
    HKEY hSubKey = NULL;

    if (NULL == (hSubKey = OpenSubKey(
            hKey,
            pwszContextName,
            dwFlags | CERT_STORE_OPEN_EXISTING_FLAG
            )))
        goto OpenSubKeyError;

    RegDeleteValueU(hSubKey, wszHashName);
    if (0 == (dwFlags & CERT_REGISTRY_STORE_REMOTE_FLAG))
        DeleteKeyFile(hSubKey, wszHashName);
    fResult = RecursiveDeleteSubKey(hSubKey, wszHashName);

CommonReturn:
    ILS_CloseRegistryKey(hSubKey);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
TRACE_ERROR(OpenSubKeyError)
}

//+-------------------------------------------------------------------------
//  If the length of the element is less than the maximum allowed Win95 value
//  length, then, attempt to set the wszRegName SubKey's "Blob" value as
//  a single registry API call. Versus, first doing registry deletes.
//--------------------------------------------------------------------------
STATIC BOOL AtomicUpdateRegistry(
        IN HKEY hKey,
        IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
        IN const BYTE *pbElement,
        IN DWORD cbElement
        )
{
    BOOL fResult;
    LONG err;
    HKEY hSubKey = NULL;

    if (MAX_KEY_BLOB_VALUE_LEN < cbElement)
        return FALSE;

    // In case the element still exists as a wszHashName value instead of as a
    // wszHashName subkey
    RegDeleteValueU(hKey, wszHashName);

    err = RegOpenHKCUKeyExU(
        hKey,
        wszHashName,
        0,                      // dwReserved
        KEY_ALL_ACCESS,
        &hSubKey);
    if (ERROR_SUCCESS == err) {
        DWORD dwType;
        DWORD cbData;

        assert(hSubKey);
        err = RegQueryValueExU(
            hSubKey,
            KEY_BLOB_VALUE_NAME,
            NULL,       // pdwReserved
            &dwType,
            NULL,       // lpData
            &cbData);
        if (!(ERROR_SUCCESS == err || ERROR_MORE_DATA == err))
            // Most likely persisted as partioned "Blob0", "Blob1" values.
            // These can't be updated in a single atomic set value.
            goto AtomicQueryValueError;

        // "Blob" value exists. We can do an atomic update.

    } else if (ERROR_FILE_NOT_FOUND == err) {
        DWORD dwDisposition;

        if (ERROR_SUCCESS != (err = RegCreateHKCUKeyExU(
                hKey,
                wszHashName,
                NULL,
                NULL,
                0,
                KEY_ALL_ACCESS,
                NULL,
                &hSubKey,
                &dwDisposition))) goto AtomicRegCreateKeyError;

        // SubKey doesn't exist. We can do an atomic update
    } else
        goto AtomicRegOpenKeyError;

    assert(hSubKey);
    // Either update or create the "Blob" value
    if (ERROR_SUCCESS != (err = RegSetValueExU(
            hSubKey,
            KEY_BLOB_VALUE_NAME,
            NULL,
            REG_BINARY,
            pbElement,
            cbElement))) goto AtomicRegSetValueError;
    fResult = TRUE;

CommonReturn:
    ILS_CloseRegistryKey(hSubKey);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR_VAR(AtomicRegOpenKeyError, err)
SET_ERROR_VAR(AtomicQueryValueError, err)
SET_ERROR_VAR(AtomicRegCreateKeyError, err)
SET_ERROR_VAR(AtomicRegSetValueError, err)
}

//+-------------------------------------------------------------------------
//  First attempt as an atomic registry update of the wszRegName's "Blob"
//  value. If that fails, then, delete everything and write as either a
//  single or partitioned blob value under the wszRegName's subkey.
//--------------------------------------------------------------------------
BOOL
ILS_WriteElementToRegistry(
    IN HKEY hKey,
    IN LPCWSTR pwszContextName,
    IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
    IN DWORD dwFlags,       //  CERT_REGISTRY_STORE_REMOTE_FLAG may be set
    IN const BYTE *pbElement,
    IN DWORD cbElement
    )
{
    BOOL fResult;
    HKEY hSubKey = NULL;

    if (NULL == (hSubKey = OpenSubKey(
            hKey,
            pwszContextName,
            dwFlags
            )))
        goto OpenSubKeyError;

    // See if we can do the update as a single, atomic, set registry value API
    // call.
    if (AtomicUpdateRegistry(
            hSubKey,
            wszHashName,
            pbElement,
            cbElement
            )) {
        fResult = TRUE;
        goto CommonReturn;
    }

    // If any version exists for this guy, get rid of it.
    ILS_DeleteElementFromRegistry(hKey, pwszContextName, wszHashName,
        dwFlags);

#if 1
    fResult = WriteKeyToRegistry(hSubKey, wszHashName, pbElement, cbElement);
#else
    LONG err;
    if (ERROR_SUCCESS != (err = RegSetValueExU(
            hSubKey,
            wszHashName,
            NULL,
            REG_BINARY,
            pbElement,
            cbElement))) {
        // Win95 returns:
        //  ERROR_INVALID_PARAMETER if exceeded single SubKey value byte
        //      limitation
        //  ERROR_OUTOFMEMORY if exceeded total SubKey values byte
        //      limitation
        if (ERROR_INVALID_PARAMETER == err ||
                ERROR_OUTOFMEMORY == err ||
                MAX_KEY_BLOB_VALUE_LEN < cbElement)
            return WriteKeyToRegistry(hSubKey, wszHashName, pbElement,
                cbElement);

         goto RegSetValueError;
    }
    fResult = TRUE;
#endif

CommonReturn:
    ILS_CloseRegistryKey(hSubKey);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(OpenSubKeyError)
#if 1
#else
SET_ERROR_VAR(RegSetValueError, err)
#endif
}

BOOL
ILS_OpenAllElementsFromRegistry(
    IN HKEY hKey,
    IN LPCWSTR pwszContextName,
    IN DWORD dwFlags,
    IN void *pvArg,
    IN PFN_ILS_OPEN_ELEMENT pfnOpenElement
    )
{
    BOOL fResult;
    HKEY hSubKey = NULL;
    LONG err;
    DWORD cSubKeys;
    DWORD i;

    dwFlags |= CERT_STORE_READONLY_FLAG;

    if (NULL == (hSubKey = OpenSubKey(
            hKey,
            pwszContextName,
            dwFlags
            )))
        goto OpenSubKeyError;

    // see how many SubKeys in the registry
    if (!GetSubKeyInfo(hSubKey, &cSubKeys))
        goto GetSubKeyInfoError;

    for (i = 0; i < cSubKeys; i++) {
        WCHAR wszHashName[MAX_HASH_NAME_LEN];
        DWORD cchHashName = MAX_HASH_NAME_LEN;
        BYTE *pbElement;
        DWORD cbElement;

        err = RegEnumKeyExU(
            hSubKey,
            i,
            wszHashName,
            &cchHashName,
            NULL,               // lpdwReserved
            NULL,               // lpszClass
            NULL,               // lpcchClass
            NULL                // lpftLastWriteTime
            );
        if (ERROR_SUCCESS != err)
            continue;

        if (ILS_ReadElementFromRegistry(
                hSubKey,
                NULL,                   // pwszContextName
                wszHashName,
                0,                      // dwFlags
                &pbElement,
                &cbElement
                )) {
            fResult = pfnOpenElement(
                wszHashName,
                pbElement,
                cbElement,
                pvArg
                );

            PkiFree(pbElement);
            if (!fResult)
                goto CommonReturn;
        }
    }
    fResult = TRUE;

CommonReturn:
    ILS_CloseRegistryKey(hSubKey);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(GetSubKeyInfoError)
TRACE_ERROR(OpenSubKeyError)
}

//+-------------------------------------------------------------------------
//  Get the Certificates, CRLs or CTLs from the registry
//
//  If CERT_REGISTRY_STORE_REMOTE_FLAG is set, then, don't attempt to read
//  from the file.
//
//  If CERT_STORE_READONLY_FLAG is set, don't attempt to migrate from the
//  "File".
//
//  If any contexts are persisted as values instead of as subkeys, then,
//  if not READONLY, migrate from values to subkeys.
//--------------------------------------------------------------------------
STATIC BOOL OpenFromRegistry(
    IN HCERTSTORE hCertStore,
    IN HKEY hKeyT,
    IN DWORD dwFlags
    )
{
    BOOL    fOK = TRUE;
    LONG    err;
    DWORD   cValues, cchValuesNameMax, cbValuesMax;
    WCHAR * wszValueName = NULL;
    DWORD   i, dwType, cchHash;
    BYTE  * pbElement = NULL;
    DWORD   cbElement;

    // see how many and how big the registry is
    if (ERROR_SUCCESS != (err = RegQueryInfoKeyU(
            hKeyT,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            &cValues,
            &cchValuesNameMax,
            &cbValuesMax,
            NULL,
            NULL))) goto RegQueryInfoKeyError;


    if (cValues && cbValuesMax) {
        // allocate the memory needed to read the reg
        // Remote Registry calls on Win95 includes the NULL terminator, that's
        // why we add +2 and not just +1
        if (NULL == (wszValueName = (WCHAR *) PkiNonzeroAlloc(
                (cchValuesNameMax+2) * sizeof(WCHAR))))
            goto OutOfMemory;
        if (NULL == (pbElement = (BYTE *) PkiNonzeroAlloc(cbValuesMax)))
            goto OutOfMemory;

        // enum the registry getting certs, CRLs or CTLs
        for (i=0; i<cValues; i++ ) {
            cbElement = cbValuesMax;
            // Remote Registry calls on Win95 includes the NULL terminator
            cchHash = cchValuesNameMax + 2;
            err = RegEnumValueU( hKeyT,
                                i,
                                wszValueName,
                                &cchHash,
                                NULL,
                                &dwType,
                                pbElement,
                                &cbElement);
            // any error get it set
            // but we want to continue to get all good certs
            if( err != ERROR_SUCCESS )
                continue;
            else
                fOK &= CertAddSerializedElementToStore(
                    hCertStore,
                    pbElement,
                    cbElement,
                    CERT_STORE_ADD_ALWAYS,
                    0,                              // dwFlags
                    CERT_STORE_ALL_CONTEXT_FLAG,
                    NULL,                           // pdwContextType
                    NULL);                          // ppvContext
        }

    }

    fOK &= OpenKeysFromRegistry(hCertStore, hKeyT, dwFlags);

    if (cValues && cbValuesMax && fOK &&
            0 == (dwFlags & CERT_STORE_READONLY_FLAG)) {
        // Migrate from values to subkeys. This allows registry roaming
        // from NT to Win95 without exceeding the Win95 registry
        // limitations

        HKEY hSubKey = NULL;
        while (TRUE) {
            if (NULL == (hSubKey = OpenSubKey(
                    hKeyT,
                    NULL,       // pwszSubKey
                    CERT_STORE_OPEN_EXISTING_FLAG
                    )))
                break;

            cbElement = cbValuesMax;
            // Remote Registry calls on Win95 includes the NULL terminator
            cchHash = cchValuesNameMax + 2;
            if (ERROR_SUCCESS != RegEnumValueU(
                    hSubKey,
                    0,                  // iValue
                    wszValueName,
                    &cchHash,
                    NULL,
                    &dwType,
                    pbElement,
                    &cbElement))
                break;

            if (!WriteKeyToRegistry(hSubKey, wszValueName, pbElement,
                    cbElement))
                break;
            if (ERROR_SUCCESS != RegDeleteValueU(hSubKey, wszValueName))
                break;
            RegCloseKey(hSubKey);
        }

        if (hSubKey)
            RegCloseKey(hSubKey);
    }

CommonReturn:
    // done with our memory
    PkiFree(wszValueName);
    PkiFree(pbElement);

    return fOK;
ErrorReturn:
    fOK = FALSE;
    goto CommonReturn;
SET_ERROR_VAR(RegQueryInfoKeyError, err)
TRACE_ERROR(OutOfMemory)
}


STATIC BOOL MoveFromRegistryToRoamingFiles(
    IN HKEY hSubKey,
    IN LPCWSTR pwszStoreDirectory,
    IN LPCWSTR pwszContextName
    )
{
    BYTE *pbElement = NULL;
    DWORD cbElement;

    while (TRUE) {
        WCHAR wszSubKeyName[MAX_CERT_REG_VALUE_NAME_LEN];
        DWORD cchSubKeyName = MAX_CERT_REG_VALUE_NAME_LEN;

        if (ERROR_SUCCESS != RegEnumKeyExU(
                hSubKey,
                0,
                wszSubKeyName,
                &cchSubKeyName,
                NULL,               // lpdwReserved
                NULL,               // lpszClass
                NULL,               // lpcchClass
                NULL                // lpftLastWriteTime
                ))
            break;

        if (!ILS_ReadElementFromRegistry(
                hSubKey,
                NULL,               // pwszContextName
                wszSubKeyName,
                CERT_STORE_READONLY_FLAG,
                &pbElement,
                &cbElement
                ))
            goto ReadElementFromRegistryError;

        if (!ILS_WriteElementToFile(
                pwszStoreDirectory,
                pwszContextName,
                wszSubKeyName,
                CERT_STORE_CREATE_NEW_FLAG,
                pbElement,
                cbElement
                )) {
            if (ERROR_FILE_EXISTS != GetLastError())
                goto WriteElementToFileError;
        }

        PkiFree(pbElement);
        pbElement = NULL;

        if (!RecursiveDeleteSubKey(
                hSubKey,
                wszSubKeyName
                ))
            goto DeleteSubKeyError;
    }

CommonReturn:
    return TRUE;
ErrorReturn:
    PkiFree(pbElement);
    goto CommonReturn;

TRACE_ERROR(ReadElementFromRegistryError)
TRACE_ERROR(WriteElementToFileError)
TRACE_ERROR(DeleteSubKeyError)
}


typedef struct _READ_CONTEXT_CALLBACK_ARG {
    BOOL                        fOK;
    HCERTSTORE                  hCertStore;
} READ_CONTEXT_CALLBACK_ARG, *PREAD_CONTEXT_CALLBACK_ARG;

STATIC BOOL ReadContextCallback(
    IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
    IN const BYTE *pbElement,
    IN DWORD cbElement,
    IN void *pvArg
    )
{
    PREAD_CONTEXT_CALLBACK_ARG pReadContextArg =
        (PREAD_CONTEXT_CALLBACK_ARG) pvArg;

    pReadContextArg->fOK &= CertAddSerializedElementToStore(
            pReadContextArg->hCertStore,
            pbElement,
            cbElement,
            CERT_STORE_ADD_ALWAYS,
            0,                              // dwFlags
            CERT_STORE_ALL_CONTEXT_FLAG,
            NULL,                           // pdwContextType
            NULL                            // ppvContext
            );

    return TRUE;
}

//+-------------------------------------------------------------------------
//  Get all the Certificates, CRLs and CTLs from the registry
//--------------------------------------------------------------------------
STATIC BOOL OpenAllFromRegistry(
    IN PREG_STORE pRegStore,
    IN HCERTSTORE hCertStore
    )
{
    BOOL fResult;
    HKEY hSubKey = NULL;
    DWORD i;

    for (i = 0; i < CONTEXT_COUNT; i++) {
        if (pRegStore->hKey) {
            if (NULL == (hSubKey = OpenSubKey(
                    pRegStore->hKey,
                    rgpwszContextSubKeyName[i],
                    pRegStore->dwFlags
                    ))) {
                if (ERROR_FILE_NOT_FOUND != GetLastError())
                    goto OpenSubKeyError;
            } else {
                // Ignore any registry errors
                OpenFromRegistry(hCertStore, hSubKey, pRegStore->dwFlags);
            }
        }

        if (pRegStore->dwFlags & CERT_REGISTRY_STORE_ROAMING_FLAG) {
            READ_CONTEXT_CALLBACK_ARG ReadContextArg;

            ReadContextArg.fOK = TRUE;
            ReadContextArg.hCertStore = hCertStore;

            if (!ILS_OpenAllElementsFromDirectory(
                    pRegStore->pwszStoreDirectory,
                    rgpwszContextSubKeyName[i],
                    pRegStore->dwFlags,
                    (void *) &ReadContextArg,
                    ReadContextCallback
                    )) {
                DWORD dwErr = GetLastError();
                if (!(ERROR_PATH_NOT_FOUND == dwErr ||
                        ERROR_FILE_NOT_FOUND == dwErr))
                    goto OpenRoamingFilesError;
            }
            // Ignore any read context errors

            if (hSubKey &&
                    0 == (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)) {
                MoveFromRegistryToRoamingFiles(
                    hSubKey,
                    pRegStore->pwszStoreDirectory,
                    rgpwszContextSubKeyName[i]
                    );
            }
        }

        if (hSubKey) {
            ILS_CloseRegistryKey(hSubKey);
            hSubKey = NULL;
        }
    }

    if ((pRegStore->dwFlags & CERT_REGISTRY_STORE_ROAMING_FLAG) &&
            pRegStore->hKey &&
            0 == (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)) {
        // Move the Key Identifiers from the registry to roaming files
        if (hSubKey = OpenSubKey(
                pRegStore->hKey,
                KEYID_CONTEXT_NAME,
                pRegStore->dwFlags
                )) {
            MoveFromRegistryToRoamingFiles(
                hSubKey,
                pRegStore->pwszStoreDirectory,
                KEYID_CONTEXT_NAME
                );

            ILS_CloseRegistryKey(hSubKey);
            hSubKey = NULL;
        }
    }

    fResult = TRUE;
CommonReturn:
    return fResult;
ErrorReturn:
    ILS_CloseRegistryKey(hSubKey);
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(OpenSubKeyError)
TRACE_ERROR(OpenRoamingFilesError)
}

//+-------------------------------------------------------------------------
//  Delete all the Certificates, CRLs and CTLs context subkeys. For Win95
//  also delete context files.
//
//  Also, if it exists, delete the "Serialized" subkey.
//--------------------------------------------------------------------------
STATIC BOOL DeleteAllFromRegistry(
    IN HKEY hKey,
    IN DWORD dwFlags        //  CERT_REGISTRY_STORE_REMOTE_FLAG may be set
    )
{
    BOOL fResult;
    DWORD i;

    for (i = 0; i < CONTEXT_COUNT; i++) {
        LPCWSTR pwszSubKeyName = rgpwszContextSubKeyName[i];
        if (0 == (dwFlags & CERT_REGISTRY_STORE_REMOTE_FLAG)) {
            // For WIN95, if a context is stored in a file, delete the
            // file
            HKEY hSubKey;
            if (NULL == (hSubKey = OpenSubKey(
                    hKey,
                    pwszSubKeyName,
                    CERT_STORE_OPEN_EXISTING_FLAG
                    ))) {
                if (ERROR_FILE_NOT_FOUND != GetLastError())
                    goto OpenContextSubKeyError;
                continue;
            }
            fResult = OpenKeysFromRegistry(
                NULL,       // hCertStore
                hSubKey,
                dwFlags
                );
            ILS_CloseRegistryKey(hSubKey);
            if (!fResult)
                goto DeleteKeysError;
        }

        if (!RecursiveDeleteSubKey(hKey, pwszSubKeyName)) {
            if (ERROR_FILE_NOT_FOUND != GetLastError()
            	&& ERROR_INVALID_PARAMETER != GetLastError())	// for CE
                goto DeleteSubKeyError;
        }
    }

    if (!RecursiveDeleteSubKey(hKey, SERIALIZED_SUBKEY_NAME)) {
        if (ERROR_FILE_NOT_FOUND != GetLastError() 
        	&& ERROR_INVALID_PARAMETER != GetLastError())	// for CE
            goto DeleteSubKeyError;
    }


    fResult = TRUE;
CommonReturn:
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(OpenContextSubKeyError)
TRACE_ERROR(DeleteKeysError)
TRACE_ERROR(DeleteSubKeyError)
}



//+=========================================================================
//  Serialized Registry Functions
//==========================================================================

static inline BOOL IsReadSerializedRegistry(
    IN PREG_STORE pRegStore
    )
{
    return (pRegStore->dwFlags & CERT_REGISTRY_STORE_SERIALIZED_FLAG);
}

static inline BOOL IsWriteSerializedRegistry(
    IN PREG_STORE pRegStore
    )
{
    if (0 == (pRegStore->dwFlags & CERT_REGISTRY_STORE_SERIALIZED_FLAG))
        return FALSE;

    pRegStore->fTouched = TRUE;
    return TRUE;
}

#ifdef CAPI_INCLUDE_SERIALIZED_STORE
//+-------------------------------------------------------------------------
//  Get all the Certificates, CRLs and CTLs from a single serialized
//  partitioned "blob" stored in the registry. The "blob" is stored under
//  the "Serialized" subkey.
//
//  Either called during initial open or with RegStore locked.
//--------------------------------------------------------------------------
STATIC BOOL OpenAllFromSerializedRegistry(
    IN PREG_STORE pRegStore,
    IN HCERTSTORE hCertStore
    )
{
    BOOL fResult;
    HKEY hSubKey = NULL;
    BYTE *pbStore = NULL;
    DWORD cbStore;

    assert(pRegStore->dwFlags & CERT_REGISTRY_STORE_SERIALIZED_FLAG);

    if (NULL == (hSubKey = OpenSubKey(
            pRegStore->hKey,
            SERIALIZED_SUBKEY_NAME,
            pRegStore->dwFlags
            )))
        goto OpenSubKeyError;

    if (!ReadMultipleKeyBlobsFromRegistry(
            hSubKey,
            &pbStore,
            &cbStore
            ))
        goto ReadError;

    if (!I_CertAddSerializedStore(
            hCertStore,
            pbStore,
            cbStore
            ))
        goto AddError;

    fResult = TRUE;
CommonReturn:
    ILS_CloseRegistryKey(hSubKey);
    PkiFree(pbStore);
    return fResult;
ErrorReturn:
    if (ERROR_FILE_NOT_FOUND == GetLastError())
        fResult = TRUE;
    else
        fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(OpenSubKeyError)
TRACE_ERROR(ReadError)
TRACE_ERROR(AddError)
}
#else
#define OpenAllFromSerializedRegistry(pRegStore,hCertStore) (FALSE)
#endif


//+=========================================================================
//  Serialized Control Functions
//==========================================================================

STATIC BOOL IsEmptyStore(
    IN HCERTSTORE hCertStore
    )
{
    PCCERT_CONTEXT pCert;

    if (pCert = CertEnumCertificatesInStore(hCertStore, NULL)) {
        CertFreeCertificateContext(pCert);
        return FALSE;
    }

#ifdef CAPI_INCLUDE_CRL
    PCCRL_CONTEXT pCrl;
    if (pCrl = CertEnumCRLsInStore(hCertStore, NULL)) {
        CertFreeCRLContext(pCrl);
        return FALSE;
    }
#endif

#ifdef CAPI_INCLUDE_CTL
    PCCTL_CONTEXT pCtl;
    if (pCtl = CertEnumCTLsInStore(hCertStore, NULL)) {
        CertFreeCTLContext(pCtl);
        return FALSE;
    }
#endif

    return TRUE;

}

STATIC BOOL CommitAllToSerializedRegistry(
    IN PREG_STORE pRegStore,
    IN DWORD dwFlags
    )
{
    BOOL fResult;
    BOOL fTouched;
    CRYPT_DATA_BLOB SerializedData = {0, NULL};
    HKEY hSubKey = NULL;

    LockRegStore(pRegStore);

    assert(pRegStore->dwFlags & CERT_REGISTRY_STORE_SERIALIZED_FLAG);

    if (dwFlags & CERT_STORE_CTRL_COMMIT_FORCE_FLAG)
        fTouched = TRUE;
    else if (dwFlags & CERT_STORE_CTRL_COMMIT_CLEAR_FLAG)
        fTouched = FALSE;
    else
        fTouched = pRegStore->fTouched;

    if (fTouched) {
        BOOL fEmpty;

        if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
            goto AccessDenied;

        fEmpty = IsEmptyStore(pRegStore->hCertStore);
        if (!fEmpty) {
            if (!CertSaveStore(
                    pRegStore->hCertStore,
                    0,                      // dwEncodingType
                    CERT_STORE_SAVE_AS_STORE,
                    CERT_STORE_SAVE_TO_MEMORY,
                    &SerializedData,
                    0))                     // dwFlags
                goto SaveStoreError;
            assert(SerializedData.cbData);
            if (NULL == (SerializedData.pbData = (BYTE *) PkiNonzeroAlloc(
                    SerializedData.cbData)))
                goto OutOfMemory;
            if (!CertSaveStore(
                    pRegStore->hCertStore,
                    0,                      // dwEncodingType
                    CERT_STORE_SAVE_AS_STORE,
                    CERT_STORE_SAVE_TO_MEMORY,
                    &SerializedData,
                    0))                     // dwFlags
                goto SaveStoreError;
        }

        if (!RecursiveDeleteSubKey(pRegStore->hKey, SERIALIZED_SUBKEY_NAME)) {
            if (ERROR_FILE_NOT_FOUND != GetLastError())
                goto DeleteSubKeyError;
        }

        if (!fEmpty) {
            if (NULL == (hSubKey = OpenSubKey(
                    pRegStore->hKey,
                    SERIALIZED_SUBKEY_NAME,
                    pRegStore->dwFlags
                    )))
                goto OpenSubKeyError;

            if (!WriteMultipleKeyBlobsToRegistry(
                    hSubKey,
                    SerializedData.pbData,
                    SerializedData.cbData
                    ))
                goto WriteStoreError;
        }
    }
    pRegStore->fTouched = FALSE;
    fResult = TRUE;

CommonReturn:
    ILS_CloseRegistryKey(hSubKey);
    PkiFree(SerializedData.pbData);
    UnlockRegStore(pRegStore);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
SET_ERROR(AccessDenied, E_ACCESSDENIED)
TRACE_ERROR(SaveStoreError)
TRACE_ERROR(OutOfMemory)
TRACE_ERROR(DeleteSubKeyError)
TRACE_ERROR(OpenSubKeyError)
TRACE_ERROR(WriteStoreError)
}


//+-------------------------------------------------------------------------
//  Open the registry's store by reading its serialized certificates,
//  CRLs and CTLs and adding to the specified certificate store.
//
//  Note for an error return, the caller will free any certs, CRLs or CTLs
//  successfully added to the store.
//
//  Only return HKEY for success. For a CertOpenStore error the caller
//  will close the HKEY.
//--------------------------------------------------------------------------
BOOL
WINAPI
I_CertDllOpenRegStoreProv(
        IN LPCSTR lpszStoreProvider,
        IN DWORD dwEncodingType,
        IN HCRYPTPROV hCryptProv,
        IN DWORD dwFlags,
        IN const void *pvPara,
        IN HCERTSTORE hCertStore,
        IN OUT PCERT_STORE_PROV_INFO pStoreProvInfo
        )
{
    BOOL fResult;
    HKEY hKey = (HKEY) pvPara;
    PREG_STORE pRegStore = NULL;

    assert(hKey);

    if (dwFlags & ~OPEN_REG_FLAGS_MASK)
        goto InvalidArg;

    if (dwFlags & CERT_STORE_DELETE_FLAG) {
        if (DeleteAllFromRegistry(hKey, dwFlags)) {
            pStoreProvInfo->dwStoreProvFlags |= CERT_STORE_PROV_DELETED_FLAG;
            return TRUE;
        } else
            return FALSE;
    }


    if (NULL == (pRegStore = (PREG_STORE) PkiZeroAlloc(sizeof(REG_STORE))))
        goto OutOfMemory;
    InitializeCriticalSection(&pRegStore->CriticalSection);
    pRegStore->hCertStore = hCertStore;
    pRegStore->dwFlags = dwFlags;

#ifndef CE_BUILD
    if (dwFlags & CERT_REGISTRY_STORE_CLIENT_GPT_FLAG) {
        PCERT_REGISTRY_STORE_CLIENT_GPT_PARA pGptPara =
            (PCERT_REGISTRY_STORE_CLIENT_GPT_PARA) pvPara;
        DWORD cbRegPath = (wcslen(pGptPara->pwszRegPath) + 1) * sizeof(WCHAR);

        if (NULL == (pRegStore->GptPara.pwszRegPath =
                (LPWSTR) PkiNonzeroAlloc(cbRegPath)))
            goto OutOfMemory;
        memcpy(pRegStore->GptPara.pwszRegPath, pGptPara->pwszRegPath,
            cbRegPath);

        // Make a copy of the base hKey
        // BUG in NT4.0 and NT5.0. Doesn't support opening of the HKLM with
        // a NULL pwszSubKey
        if (HKEY_LOCAL_MACHINE == pGptPara->hKeyBase)
            pRegStore->GptPara.hKeyBase = HKEY_LOCAL_MACHINE;
        else if (NULL == (pRegStore->GptPara.hKeyBase = OpenSubKey(
                pGptPara->hKeyBase,
                NULL,       // pwszSubKey
                (dwFlags & ~CERT_STORE_CREATE_NEW_FLAG) |
                    CERT_STORE_OPEN_EXISTING_FLAG
                )))
            goto OpenSubKeyError;

        fResult = OpenAllFromGptRegistry(pRegStore,
            pRegStore->hCertStore);

#if 1
        // For subsequent opens, allow subkey create if it doesn't already
        // exist.
        pRegStore->dwFlags &= ~(CERT_STORE_OPEN_EXISTING_FLAG |
            CERT_STORE_CREATE_NEW_FLAG);
#else

        // For subsequent opens, allow subkey create if it doesn't already
        // exist. However, preserve open existing.
        pRegStore->dwFlags &= ~CERT_STORE_CREATE_NEW_FLAG;
#endif

    }    
    else
#endif // CE_BUILD    
    if (dwFlags & CERT_REGISTRY_STORE_ROAMING_FLAG) {
        PCERT_REGISTRY_STORE_ROAMING_PARA pRoamingPara =
            (PCERT_REGISTRY_STORE_ROAMING_PARA) pvPara;
        DWORD cbDir = (wcslen(pRoamingPara->pwszStoreDirectory) + 1) *
            sizeof(WCHAR);

        if (NULL == (pRegStore->pwszStoreDirectory = (LPWSTR) PkiNonzeroAlloc(
                cbDir)))
            goto OutOfMemory;
        memcpy(pRegStore->pwszStoreDirectory, pRoamingPara->pwszStoreDirectory,
            cbDir);

        dwFlags &= ~CERT_STORE_CREATE_NEW_FLAG;
        dwFlags |= CERT_STORE_OPEN_EXISTING_FLAG;
        pRegStore->dwFlags = dwFlags;
        if (pRoamingPara->hKey) {
            // Make a copy of the input hKey
            if (NULL == (pRegStore->hKey = OpenSubKey(
                    pRoamingPara->hKey,
                    NULL,       // pwszSubKey
                    dwFlags
                    )))
                goto OpenSubKeyError;
        }

        fResult = OpenAllFromRegistry(pRegStore, pRegStore->hCertStore);
    } else {
        // Make a copy of the input hKey
        if (NULL == (pRegStore->hKey = OpenSubKey(
                hKey,
                NULL,       // pwszSubKey
                (dwFlags & ~CERT_STORE_CREATE_NEW_FLAG) |
                    CERT_STORE_OPEN_EXISTING_FLAG
                )))
            goto OpenSubKeyError;

        if (dwFlags & CERT_REGISTRY_STORE_SERIALIZED_FLAG)
            fResult = OpenAllFromSerializedRegistry(pRegStore,
                pRegStore->hCertStore);
        else
            fResult = OpenAllFromRegistry(pRegStore, pRegStore->hCertStore);

        // For subsequent opens, allow subkey create if it doesn't already
        // exist.
        pRegStore->dwFlags &= ~(CERT_STORE_OPEN_EXISTING_FLAG |
            CERT_STORE_CREATE_NEW_FLAG);
    }
    if (!fResult)
        goto OpenAllError;


    pStoreProvInfo->cStoreProvFunc = REG_STORE_PROV_FUNC_COUNT;
    pStoreProvInfo->rgpvStoreProvFunc = (void **) rgpvRegStoreProvFunc;
    pStoreProvInfo->hStoreProv = (HCERTSTOREPROV) pRegStore;
    fResult = TRUE;

CommonReturn:
    return fResult;

ErrorReturn:
    RegStoreProvClose((HCERTSTOREPROV) pRegStore, 0);
    fResult = FALSE;
    goto CommonReturn;
SET_ERROR(InvalidArg, E_INVALIDARG)
TRACE_ERROR(OutOfMemory)
TRACE_ERROR(OpenSubKeyError)
TRACE_ERROR(OpenAllError)
}


//+-------------------------------------------------------------------------
//  Close the registry's store by closing its opened registry subkeys
//--------------------------------------------------------------------------
STATIC void WINAPI RegStoreProvClose(
        IN HCERTSTOREPROV hStoreProv,
        IN DWORD dwFlags
        )
{
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    if (pRegStore) {
        if (pRegStore->hMyNotifyChange)
            CloseHandle(pRegStore->hMyNotifyChange);
#ifndef CE_BUILD
        FreeRegistryStoreChange(pRegStore);

        if (hWin95NotifyEvent)
            Win95StoreSignalAndFreeRegStoreResyncEntries(pRegStore);

        if (pRegStore->dwFlags & CERT_REGISTRY_STORE_CLIENT_GPT_FLAG) {
            if (pRegStore->fTouched)
                CommitAllToGptRegistry(
                    pRegStore,
                    0               // dwFlags
                    );
            GptStoreSignalAndFreeRegStoreResyncEntries(pRegStore);
            PkiFree(pRegStore->GptPara.pwszRegPath);
            // BUG in NT4.0 and NT5.0. Doesn't support opening of the HKLM with
            // a NULL pwszSubKey
            if (pRegStore->GptPara.hKeyBase &&
                    HKEY_LOCAL_MACHINE != pRegStore->GptPara.hKeyBase)
                RegCloseKey(pRegStore->GptPara.hKeyBase);
        } else 
#endif        
        if (pRegStore->dwFlags & CERT_REGISTRY_STORE_ROAMING_FLAG) {
            PkiFree(pRegStore->pwszStoreDirectory);
        } else if (pRegStore->dwFlags & CERT_REGISTRY_STORE_SERIALIZED_FLAG) {
            if (pRegStore->fTouched)
                CommitAllToSerializedRegistry(
                    pRegStore,
                    0               // dwFlags
                    );
        }

        if (pRegStore->hKey)
            RegCloseKey(pRegStore->hKey);
        DeleteCriticalSection(&pRegStore->CriticalSection);
        PkiFree(pRegStore);
    }
}

//+-------------------------------------------------------------------------
//  Get the certificate's registry value name by formatting its SHA1 hash as
//  UNICODE hex.
//--------------------------------------------------------------------------
STATIC BOOL GetCertRegValueName(
        IN PCCERT_CONTEXT pCertContext,
        OUT WCHAR wszRegName[MAX_CERT_REG_VALUE_NAME_LEN]
        )
{
    BYTE    rgbHash[MAX_HASH_LEN];
    DWORD   cbHash = MAX_HASH_LEN;

    // get the thumbprint
    if(!CertGetCertificateContextProperty(
            pCertContext,
            CERT_SHA1_HASH_PROP_ID,
            rgbHash,
            &cbHash))
        return FALSE;

    // convert to a string
    BytesToWStr(cbHash, rgbHash, wszRegName);
    return TRUE;
}

#ifdef CAPI_INCLUDE_CRL
//+-------------------------------------------------------------------------
//  Get the CRL's registry value name by formatting its SHA1 hash as
//  UNICODE hex.
//--------------------------------------------------------------------------
STATIC BOOL GetCrlRegValueName(
        IN PCCRL_CONTEXT pCrlContext,
        OUT WCHAR wszRegName[MAX_CERT_REG_VALUE_NAME_LEN]
        )
{
    BYTE    rgbHash[MAX_HASH_LEN];
    DWORD   cbHash = MAX_HASH_LEN;

    // get the thumbprint
    if(!CertGetCRLContextProperty(
            pCrlContext,
            CERT_SHA1_HASH_PROP_ID,
            rgbHash,
            &cbHash))
        return FALSE;

    // convert to a string
    BytesToWStr(cbHash, rgbHash, wszRegName);
    return TRUE;
}
#endif

#ifdef CAPI_INCLUDE_CTL
//+-------------------------------------------------------------------------
//  Get the CTL's registry value name by formatting its SHA1 hash as
//  UNICODE hex.
//--------------------------------------------------------------------------
STATIC BOOL GetCtlRegValueName(
        IN PCCTL_CONTEXT pCtlContext,
        OUT WCHAR wszRegName[MAX_CERT_REG_VALUE_NAME_LEN]
        )
{
    BYTE    rgbHash[MAX_HASH_LEN];
    DWORD   cbHash = MAX_HASH_LEN;

    // get the thumbprint
    if(!CertGetCTLContextProperty(
            pCtlContext,
            CERT_SHA1_HASH_PROP_ID,
            rgbHash,
            &cbHash))
        return FALSE;

    // convert to a string
    BytesToWStr(cbHash, rgbHash, wszRegName);
    return TRUE;
}
#endif

//+-------------------------------------------------------------------------
//  Read the serialized copy of the context from either the registry or
//  a roaming file and create a new context.
//--------------------------------------------------------------------------
STATIC BOOL ReadContext(
    IN PREG_STORE pRegStore,
    IN DWORD dwContextType,
    IN const WCHAR wszSubKeyName[MAX_CERT_REG_VALUE_NAME_LEN],
    OUT const void **ppvContext
    )
{
    BOOL fResult;
    BYTE *pbElement = NULL;
    DWORD cbElement;

	assert(dwContextType < CONTEXT_COUNT);
    if (pRegStore->dwFlags & CERT_REGISTRY_STORE_ROAMING_FLAG) {
        if (!ILS_ReadElementFromFile(
                pRegStore->pwszStoreDirectory,
                rgpwszContextSubKeyName[dwContextType],
                wszSubKeyName,
                pRegStore->dwFlags,
                &pbElement,
                &cbElement
                ))
            goto ReadElementFromFileError;
    } else {
        if (pRegStore->dwFlags & CERT_REGISTRY_STORE_CLIENT_GPT_FLAG) {
            assert(NULL == pRegStore->hKey);
            if (NULL == (pRegStore->hKey = OpenSubKey(
                    pRegStore->GptPara.hKeyBase,
                    pRegStore->GptPara.pwszRegPath,
                    pRegStore->dwFlags
                    )))
                goto OpenSubKeyError;
        }

        fResult = ILS_ReadElementFromRegistry(
                pRegStore->hKey,
                rgpwszContextSubKeyName[dwContextType],
                wszSubKeyName,
                pRegStore->dwFlags,
                &pbElement,
                &cbElement
                );

        if (pRegStore->dwFlags & CERT_REGISTRY_STORE_CLIENT_GPT_FLAG) {
            ILS_CloseRegistryKey(pRegStore->hKey);
            pRegStore->hKey = NULL;
        }

        if (!fResult)
            goto ReadElementFromRegistryError;
    }

    if (!CertAddSerializedElementToStore(
            NULL,                           // hCertStore,
            pbElement,
            cbElement,
            CERT_STORE_ADD_ALWAYS,
            0,                              // dwFlags
            rgdwContextTypeFlags[dwContextType],
            NULL,                           // pdwContextType
            ppvContext))
        goto AddSerializedElementError;

    fResult = TRUE;
CommonReturn:
    PkiFree(pbElement);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    *ppvContext = NULL;
    goto CommonReturn;

TRACE_ERROR(ReadElementFromFileError)
TRACE_ERROR(ReadElementFromRegistryError)
TRACE_ERROR(AddSerializedElementError)
TRACE_ERROR(OpenSubKeyError)
}

//+-------------------------------------------------------------------------
//  Write the serialized context and its properties to
//  the registry or a roaming file.
//
//  Called before the context is written to the store.
//--------------------------------------------------------------------------
STATIC BOOL WriteSerializedContext(
    IN PREG_STORE pRegStore,
    IN DWORD dwContextType,
    IN const WCHAR wszSubKeyName[MAX_CERT_REG_VALUE_NAME_LEN],
    IN const BYTE *pbElement,
    IN DWORD cbElement
    )
{
    BOOL fResult;

	assert(dwContextType < CONTEXT_COUNT);
    if (pRegStore->dwFlags & CERT_REGISTRY_STORE_ROAMING_FLAG)
        fResult = ILS_WriteElementToFile(
                pRegStore->pwszStoreDirectory,
                rgpwszContextSubKeyName[dwContextType],
                wszSubKeyName,
                pRegStore->dwFlags,
                pbElement,
                cbElement
                );
    else {
        if (pRegStore->dwFlags & CERT_REGISTRY_STORE_CLIENT_GPT_FLAG) {
            assert(NULL == pRegStore->hKey);
            if (NULL == (pRegStore->hKey = OpenSubKey(
                    pRegStore->GptPara.hKeyBase,
                    pRegStore->GptPara.pwszRegPath,
                    pRegStore->dwFlags
                    )))
                return FALSE;
        }

        fResult = ILS_WriteElementToRegistry(
                pRegStore->hKey,
                rgpwszContextSubKeyName[dwContextType],
                wszSubKeyName,
                pRegStore->dwFlags,
                pbElement,
                cbElement
                );
        if (pRegStore->dwFlags & CERT_REGISTRY_STORE_CLIENT_GPT_FLAG) {
            ILS_CloseRegistryKey(pRegStore->hKey);
            pRegStore->hKey = NULL;
        }

        if (hWin95NotifyEvent && fResult)
            PulseEvent(hWin95NotifyEvent);
    }
    return fResult;
}

//+-------------------------------------------------------------------------
//  Delete the context and its properties from the
//  the registry or a roaming file.
//
//  Called before the context is deleted from the store.
//--------------------------------------------------------------------------
STATIC BOOL DeleteContext(
    IN PREG_STORE pRegStore,
    IN DWORD dwContextType,
    IN const WCHAR wszSubKeyName[MAX_CERT_REG_VALUE_NAME_LEN]
    )
{
    BOOL fResult;

	assert(dwContextType < CONTEXT_COUNT);
    if (pRegStore->dwFlags & CERT_REGISTRY_STORE_ROAMING_FLAG)
        fResult = ILS_DeleteElementFromDirectory(
                pRegStore->pwszStoreDirectory,
                rgpwszContextSubKeyName[dwContextType],
                wszSubKeyName,
                pRegStore->dwFlags
                );
    else {
        if (pRegStore->dwFlags & CERT_REGISTRY_STORE_CLIENT_GPT_FLAG) {
            assert(NULL == pRegStore->hKey);
            if (NULL == (pRegStore->hKey = OpenSubKey(
                    pRegStore->GptPara.hKeyBase,
                    pRegStore->GptPara.pwszRegPath,
                    pRegStore->dwFlags
                    )))
                return FALSE;
        }

        fResult = ILS_DeleteElementFromRegistry(
                pRegStore->hKey,
                rgpwszContextSubKeyName[dwContextType],
                wszSubKeyName,
                pRegStore->dwFlags
                );

#ifdef UNDER_CE
		// RegDeleteKey returns the wrong error. Remove when the right error is returned
		if (!fResult && GetLastError() == ERROR_INVALID_PARAMETER)
			SetLastError(ERROR_FILE_NOT_FOUND);
#endif			

        if (pRegStore->dwFlags & CERT_REGISTRY_STORE_CLIENT_GPT_FLAG) {
            ILS_CloseRegistryKey(pRegStore->hKey);
            pRegStore->hKey = NULL;
        }

        if (hWin95NotifyEvent && fResult)
            PulseEvent(hWin95NotifyEvent);
    }

    if (!fResult) {
        DWORD dwErr = GetLastError();
        if (ERROR_PATH_NOT_FOUND == dwErr || ERROR_FILE_NOT_FOUND == dwErr)
            fResult = TRUE;
    }
    return fResult;
}

//+-------------------------------------------------------------------------
//  Read the serialized copy of the certificate and its properties from
//  the registry and create a new certificate context.
//--------------------------------------------------------------------------
STATIC BOOL WINAPI RegStoreProvReadCert(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pStoreCertContext,
        IN DWORD dwFlags,
        OUT PCCERT_CONTEXT *ppProvCertContext
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    WCHAR wsz[MAX_CERT_REG_VALUE_NAME_LEN];

    assert(pRegStore);
    if (IsReadSerializedRegistry(pRegStore))
        goto UnexpectedReadError;

    if (!GetCertRegValueName(pStoreCertContext, wsz))
        goto GetRegValueNameError;

    fResult = ReadContext(
        pRegStore,
        CERT_STORE_CERTIFICATE_CONTEXT - 1,
        wsz,
        (const void **) ppProvCertContext
        );

CommonReturn:
    return fResult;
ErrorReturn:
    fResult = FALSE;
    *ppProvCertContext = NULL;
    goto CommonReturn;

SET_ERROR(UnexpectedReadError, E_UNEXPECTED)
TRACE_ERROR(GetRegValueNameError)
}

STATIC void SetMyIEDirtyFlag()
{
    LONG err;
    DWORD dwDisposition;
    HKEY hSubKey;

    if (ERROR_SUCCESS != (err = RegCreateKeyExU(
            HKEY_LOCAL_MACHINE,
            CERT_IE_DIRTY_FLAGS_REGPATH,
            0,                      // dwReserved
            NULL,                   // lpClass
            REG_OPTION_NON_VOLATILE,
            MAXIMUM_ALLOWED,
            NULL,                   // lpSecurityAttributes
            &hSubKey,
            &dwDisposition))) {
#if DBG
        DbgPrintf(DBG_SS_CRYPT32,
            "RegCreateKeyEx(%S) returned error: %d 0x%x\n",
            CERT_IE_DIRTY_FLAGS_REGPATH, err, err);
#endif
    } else {
        WriteDWORDValueToRegistry(hSubKey, L"My", 0x1);
        RegCloseKey(hSubKey);
    }
}

//+-------------------------------------------------------------------------
//  Serialize the encoded certificate and its properties and write to
//  the registry.
//
//  Called before the certificate is written to the store.
//
//  Note, don't set the IEDirtyFlag if setting a property.
//--------------------------------------------------------------------------

STATIC BOOL WINAPI RegStoreProvWriteCertEx(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pCertContext,
        IN DWORD dwFlags,
        IN BOOL fSetProperty
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    WCHAR wsz[MAX_CERT_REG_VALUE_NAME_LEN];
    BYTE *pbElement = NULL;
    DWORD cbElement;

    assert(pRegStore);
    if (IsInResync(pRegStore))
        // Only update the store cache, don't write back to registry
        return TRUE;

    if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
        goto AccessDenied;
    if (IsWriteSerializedRegistry(pRegStore))
        return TRUE;

    if (!GetCertRegValueName(pCertContext, wsz))
        goto GetRegValueNameError;

    // get the size
    if (!CertSerializeCertificateStoreElement(
            pCertContext, 0, NULL, &cbElement))
        goto SerializeStoreElementError;
    if (NULL == (pbElement = (BYTE *) PkiNonzeroAlloc(cbElement)))
        goto OutOfMemory;

    // put it into the buffer
    if (!CertSerializeCertificateStoreElement(
            pCertContext, 0, pbElement, &cbElement))
        goto SerializeStoreElementError;

    // write it to the registry or roaming file
    fResult = WriteSerializedContext(
        pRegStore,
        CERT_STORE_CERTIFICATE_CONTEXT - 1,
        wsz,
        pbElement,
        cbElement
        );

    if (fResult && !fSetProperty &&
            0 != (pRegStore->dwFlags & CERT_REGISTRY_STORE_MY_IE_DIRTY_FLAG))
        SetMyIEDirtyFlag();

CommonReturn:
    PkiFree(pbElement);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(AccessDenied, E_ACCESSDENIED)
TRACE_ERROR(GetRegValueNameError)
TRACE_ERROR(OutOfMemory)
TRACE_ERROR(SerializeStoreElementError)
}

STATIC BOOL WINAPI RegStoreProvWriteCert(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pCertContext,
        IN DWORD dwFlags
        )
{
    return RegStoreProvWriteCertEx(
        hStoreProv,
        pCertContext,
        dwFlags,
        FALSE                       // fSetProperty
        );
}


//+-------------------------------------------------------------------------
//  Delete the specified certificate from the registry.
//
//  Called before the certificate is deleted from the store.
//+-------------------------------------------------------------------------
STATIC BOOL WINAPI RegStoreProvDeleteCert(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pCertContext,
        IN DWORD dwFlags
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    WCHAR wsz[MAX_CERT_REG_VALUE_NAME_LEN];

    assert(pRegStore);
    if (IsInResync(pRegStore))
        // Only delete from store cache, not from registry
        return TRUE;

    if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
        goto AccessDenied;
    if (IsWriteSerializedRegistry(pRegStore))
        return TRUE;

    if (!GetCertRegValueName(pCertContext, wsz))
        goto GetRegValueNameError;

    // delete this cert
    fResult = DeleteContext(
        pRegStore,
        CERT_STORE_CERTIFICATE_CONTEXT - 1,
        wsz
        );
CommonReturn:
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(AccessDenied, E_ACCESSDENIED)
TRACE_ERROR(GetRegValueNameError)
}

//+-------------------------------------------------------------------------
//  Read the specified certificate from the registry and update its
//  property.
//
//  Note, ignore the CERT_SHA1_HASH_PROP_ID property which is implicitly
//  set before we write the certificate to the registry. If we don't ignore,
//  we will have indefinite recursion.
//
//  Called before setting the property of the certificate in the store.
//--------------------------------------------------------------------------
STATIC BOOL WINAPI RegStoreProvSetCertProperty(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pCertContext,
        IN DWORD dwPropId,
        IN DWORD dwFlags,
        IN const void *pvData
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    PCCERT_CONTEXT pProvCertContext = NULL;

    // This property is implicitly written whenever we do a CertWrite.
    if (CERT_SHA1_HASH_PROP_ID == dwPropId)
        return TRUE;

    assert(pRegStore);
    if (IsInResync(pRegStore))
        // Only update the store cache, don't write back to registry
        return TRUE;

    if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
        goto AccessDenied;
    if (IsWriteSerializedRegistry(pRegStore))
        return TRUE;

    // Create a certificate context from the current serialized value stored
    // in the registry.
    if (!RegStoreProvReadCert(
            hStoreProv,
            pCertContext,
            0,              // dwFlags
            &pProvCertContext)) goto ReadError;

    // Set the property in the above created certificate context.
    if (!CertSetCertificateContextProperty(
            pProvCertContext,
            dwPropId,
            dwFlags,
            pvData)) goto SetPropertyError;

    // Serialize and write the above updated certificate back to the
    // registry.
    if (!RegStoreProvWriteCertEx(
            hStoreProv,
            pProvCertContext,
            0,                  // dwFlags
            TRUE                // fSetProperty
            ))
        goto WriteError;
    fResult = TRUE;
CommonReturn:
    CertFreeCertificateContext(pProvCertContext);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
SET_ERROR(AccessDenied, E_ACCESSDENIED)
TRACE_ERROR(ReadError)
TRACE_ERROR(SetPropertyError)
TRACE_ERROR(WriteError)
}

#ifdef CAPI_INCLUDE_CRL
//+-------------------------------------------------------------------------
//  Read the serialized copy of the CRL and its properties from
//  the registry and create a new CRL context.
//--------------------------------------------------------------------------
STATIC BOOL WINAPI RegStoreProvReadCrl(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCRL_CONTEXT pStoreCrlContext,
        IN DWORD dwFlags,
        OUT PCCRL_CONTEXT *ppProvCrlContext
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    WCHAR wsz[MAX_CERT_REG_VALUE_NAME_LEN];

    assert(pRegStore);
    if (IsReadSerializedRegistry(pRegStore))
        goto UnexpectedReadError;

    if (!GetCrlRegValueName(pStoreCrlContext, wsz))
        goto GetRegValueNameError;

    fResult = ReadContext(
        pRegStore,
        CERT_STORE_CRL_CONTEXT - 1,
        wsz,
        (const void **) ppProvCrlContext
        );

CommonReturn:
    return fResult;
ErrorReturn:
    fResult = FALSE;
    *ppProvCrlContext = NULL;
    goto CommonReturn;

SET_ERROR(UnexpectedReadError, E_UNEXPECTED)
TRACE_ERROR(GetRegValueNameError)
}

//+-------------------------------------------------------------------------
//  Serialize the encoded CRL and its properties and write to
//  the registry.
//
//  Called before the CRL is written to the store.
//--------------------------------------------------------------------------
STATIC BOOL WINAPI RegStoreProvWriteCrl(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCRL_CONTEXT pCrlContext,
        IN DWORD dwFlags
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    WCHAR wsz[MAX_CERT_REG_VALUE_NAME_LEN];
    BYTE *pbElement = NULL;
    DWORD cbElement;

    assert(pRegStore);
    if (IsInResync(pRegStore))
        // Only update the store cache, don't write back to registry
        return TRUE;

    if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
        goto AccessDenied;
    if (IsWriteSerializedRegistry(pRegStore))
        return TRUE;

    if (!GetCrlRegValueName(pCrlContext, wsz))
        goto GetRegValueNameError;

    // get the size
    if (!CertSerializeCRLStoreElement(pCrlContext, 0, NULL, &cbElement))
        goto SerializeStoreElementError;
    if (NULL == (pbElement = (BYTE *) PkiNonzeroAlloc(cbElement)))
        goto OutOfMemory;

    // put it into the buffer
    if (!CertSerializeCRLStoreElement(pCrlContext, 0, pbElement, &cbElement))
        goto SerializeStoreElementError;

    // write it to the registry or roaming file
    fResult = WriteSerializedContext(
        pRegStore,
        CERT_STORE_CRL_CONTEXT - 1,
        wsz,
        pbElement,
        cbElement
        );

CommonReturn:
    PkiFree(pbElement);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(AccessDenied, E_ACCESSDENIED)
TRACE_ERROR(GetRegValueNameError)
TRACE_ERROR(OutOfMemory)
TRACE_ERROR(SerializeStoreElementError)
}


//+-------------------------------------------------------------------------
//  Delete the specified CRL from the registry.
//
//  Called before the CRL is deleted from the store.
//+-------------------------------------------------------------------------
STATIC BOOL WINAPI RegStoreProvDeleteCrl(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCRL_CONTEXT pCrlContext,
        IN DWORD dwFlags
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    WCHAR wsz[MAX_CERT_REG_VALUE_NAME_LEN];

    assert(pRegStore);
    if (IsInResync(pRegStore))
        // Only delete from store cache, not from registry
        return TRUE;

    if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
        goto AccessDenied;
    if (IsWriteSerializedRegistry(pRegStore))
        return TRUE;

    if (!GetCrlRegValueName(pCrlContext, wsz))
        goto GetRegValueNameError;

    // delete this CRL
    fResult = DeleteContext(
        pRegStore,
        CERT_STORE_CRL_CONTEXT - 1,
        wsz
        );

CommonReturn:
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(AccessDenied, E_ACCESSDENIED)
TRACE_ERROR(GetRegValueNameError)
}

//+-------------------------------------------------------------------------
//  Read the specified CRL from the registry and update its
//  property.
//
//  Note, ignore the CERT_SHA1_HASH_PROP_ID property which is implicitly
//  set before we write the CRL to the registry. If we don't ignore,
//  we will have indefinite recursion.
//
//  Called before setting the property of the CRL in the store.
//--------------------------------------------------------------------------
STATIC BOOL WINAPI RegStoreProvSetCrlProperty(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCRL_CONTEXT pCrlContext,
        IN DWORD dwPropId,
        IN DWORD dwFlags,
        IN const void *pvData
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    PCCRL_CONTEXT pProvCrlContext = NULL;

    // This property is implicitly written whenever we do a CertWrite.
    if (CERT_SHA1_HASH_PROP_ID == dwPropId)
        return TRUE;

    assert(pRegStore);
    if (IsInResync(pRegStore))
        // Only update the store cache, don't write back to registry
        return TRUE;

    if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
        goto AccessDenied;
    if (IsWriteSerializedRegistry(pRegStore))
        return TRUE;

    // Create a certificate context from the current serialized value stored
    // in the registry.
    if (!RegStoreProvReadCrl(
            hStoreProv,
            pCrlContext,
            0,              // dwFlags
            &pProvCrlContext)) goto ReadError;

    // Set the property in the above created certificate context.
    if (!CertSetCRLContextProperty(
            pProvCrlContext,
            dwPropId,
            dwFlags,
            pvData)) goto SetPropertyError;

    // Serialize and write the above updated certificate back to the
    // registry.
    if (!RegStoreProvWriteCrl(
            hStoreProv,
            pProvCrlContext,
            0))                 //dwFlags
        goto WriteError;
    fResult = TRUE;
CommonReturn:
    CertFreeCRLContext(pProvCrlContext);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
SET_ERROR(AccessDenied, E_ACCESSDENIED)
TRACE_ERROR(ReadError)
TRACE_ERROR(SetPropertyError)
TRACE_ERROR(WriteError)
}

#endif CAPI_INCLUDE_CRL

#ifdef CAPI_INCLUDE_CTL
//+-------------------------------------------------------------------------
//  Read the serialized copy of the CTL and its properties from
//  the registry and create a new CTL context.
//--------------------------------------------------------------------------
STATIC BOOL WINAPI RegStoreProvReadCtl(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCTL_CONTEXT pStoreCtlContext,
        IN DWORD dwFlags,
        OUT PCCTL_CONTEXT *ppProvCtlContext
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    WCHAR wsz[MAX_CERT_REG_VALUE_NAME_LEN];

    assert(pRegStore);
    if (IsReadSerializedRegistry(pRegStore))
        goto UnexpectedReadError;

    if (!GetCtlRegValueName(pStoreCtlContext, wsz))
        goto GetRegValueNameError;

    fResult = ReadContext(
        pRegStore,
        CERT_STORE_CTL_CONTEXT - 1,
        wsz,
        (const void **) ppProvCtlContext
        );
CommonReturn:
    return fResult;
ErrorReturn:
    fResult = FALSE;
    *ppProvCtlContext = NULL;
    goto CommonReturn;

SET_ERROR(UnexpectedReadError, E_UNEXPECTED)
TRACE_ERROR(GetRegValueNameError)
}

//+-------------------------------------------------------------------------
//  Serialize the encoded CTL and its properties and write to
//  the registry.
//
//  Called before the CTL is written to the store.
//--------------------------------------------------------------------------
STATIC BOOL WINAPI RegStoreProvWriteCtl(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCTL_CONTEXT pCtlContext,
        IN DWORD dwFlags
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    WCHAR wsz[MAX_CERT_REG_VALUE_NAME_LEN];
    BYTE *pbElement = NULL;
    DWORD cbElement;

    assert(pRegStore);
    if (IsInResync(pRegStore))
        // Only update the store cache, don't write back to registry
        return TRUE;

    if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
        goto AccessDenied;
    if (IsWriteSerializedRegistry(pRegStore))
        return TRUE;

    if (!GetCtlRegValueName(pCtlContext, wsz))
        goto GetRegValueNameError;

    // get the size
    if (!CertSerializeCTLStoreElement(pCtlContext, 0, NULL, &cbElement))
        goto SerializeStoreElementError;
    if (NULL == (pbElement = (BYTE *) PkiNonzeroAlloc(cbElement)))
        goto OutOfMemory;

    // put it into the buffer
    if (!CertSerializeCTLStoreElement(pCtlContext, 0, pbElement, &cbElement))
        goto SerializeStoreElementError;

    // write it to the registry or roaming file
    fResult = WriteSerializedContext(
        pRegStore,
        CERT_STORE_CTL_CONTEXT - 1,
        wsz,
        pbElement,
        cbElement
        );
CommonReturn:
    PkiFree(pbElement);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(AccessDenied, E_ACCESSDENIED)
TRACE_ERROR(GetRegValueNameError)
TRACE_ERROR(OutOfMemory)
TRACE_ERROR(SerializeStoreElementError)
}


//+-------------------------------------------------------------------------
//  Delete the specified CTL from the registry.
//
//  Called before the CTL is deleted from the store.
//+-------------------------------------------------------------------------
STATIC BOOL WINAPI RegStoreProvDeleteCtl(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCTL_CONTEXT pCtlContext,
        IN DWORD dwFlags
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    WCHAR wsz[MAX_CERT_REG_VALUE_NAME_LEN];

    assert(pRegStore);
    if (IsInResync(pRegStore))
        // Only delete from store cache, not from registry
        return TRUE;

    if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
        goto AccessDenied;
    if (IsWriteSerializedRegistry(pRegStore))
        return TRUE;

    if (!GetCtlRegValueName(pCtlContext, wsz))
        goto GetRegValueNameError;

    // delete this CTL
    fResult = DeleteContext(
        pRegStore,
        CERT_STORE_CTL_CONTEXT - 1,
        wsz
        );
CommonReturn:
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(AccessDenied, E_ACCESSDENIED)
TRACE_ERROR(GetRegValueNameError)
}

//+-------------------------------------------------------------------------
//  Read the specified CTL from the registry and update its
//  property.
//
//  Note, ignore the CERT_SHA1_HASH_PROP_ID property which is implicitly
//  set before we write the CTL to the registry. If we don't ignore,
//  we will have indefinite recursion.
//
//  Called before setting the property of the CTL in the store.
//--------------------------------------------------------------------------
STATIC BOOL WINAPI RegStoreProvSetCtlProperty(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCTL_CONTEXT pCtlContext,
        IN DWORD dwPropId,
        IN DWORD dwFlags,
        IN const void *pvData
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    PCCTL_CONTEXT pProvCtlContext = NULL;

    // This property is implicitly written whenever we do a CertWrite.
    if (CERT_SHA1_HASH_PROP_ID == dwPropId)
        return TRUE;

    assert(pRegStore);
    if (IsInResync(pRegStore))
        // Only update the store cache, don't write back to registry
        return TRUE;

    if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
        goto AccessDenied;
    if (IsWriteSerializedRegistry(pRegStore))
        return TRUE;

    // Create a CTL context from the current serialized value stored
    // in the registry.
    if (!RegStoreProvReadCtl(
            hStoreProv,
            pCtlContext,
            0,              // dwFlags
            &pProvCtlContext)) goto ReadError;

    // Set the property in the above created certificate context.
    if (!CertSetCTLContextProperty(
            pProvCtlContext,
            dwPropId,
            dwFlags,
            pvData)) goto SetPropertyError;

    // Serialize and write the above updated certificate back to the
    // registry.
    if (!RegStoreProvWriteCtl(
            hStoreProv,
            pProvCtlContext,
            0))                 //dwFlags
        goto WriteError;
    fResult = TRUE;
CommonReturn:
    CertFreeCTLContext(pProvCtlContext);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
SET_ERROR(AccessDenied, E_ACCESSDENIED)
TRACE_ERROR(ReadError)
TRACE_ERROR(SetPropertyError)
TRACE_ERROR(WriteError)
}

#endif

#ifndef CE_BUILD
//+=========================================================================
//  Control functions
//==========================================================================

STATIC BOOL RegNotifyChange(
    IN PREG_STORE pRegStore,
    HANDLE hEvent
    )
{
    if (pRegStore->dwFlags & CERT_REGISTRY_STORE_ROAMING_FLAG)
        return RegRegistryStoreChange(pRegStore, hEvent);
    else if (hWin95NotifyEvent)
        return RegWin95StoreChange(pRegStore, hEvent);
    else if (pRegStore->dwFlags & CERT_REGISTRY_STORE_CLIENT_GPT_FLAG)
        return RegGptStoreChange(pRegStore, hEvent);
    else
        return RegRegistryStoreChange(pRegStore, hEvent);
}

STATIC BOOL ResyncFromRegistry(
    IN PREG_STORE pRegStore,
    IN OPTIONAL HANDLE hEvent
    )
{
    BOOL fResult;
    HCERTSTORE hNewStore = NULL;
    HANDLE hMyNotifyChange;

    // Serialize resyncs
    LockRegStore(pRegStore);

    if (hEvent) {
        // Re-arm the specified event
        if (!RegNotifyChange(pRegStore, hEvent))
            goto NotifyChangeError;
    }

    hMyNotifyChange = pRegStore->hMyNotifyChange;
    if (hMyNotifyChange) {
        // Check if any changes since last resync
        if (WAIT_TIMEOUT == WaitForSingleObjectEx(
                hMyNotifyChange,
                0,                          // dwMilliseconds
                FALSE                       // bAlertable
                )) {
            // No change
            fResult = TRUE;
            goto CommonReturn;
        } else {
            // Re-arm our event handle
            if (!RegNotifyChange(pRegStore, hMyNotifyChange))
                goto NotifyChangeError;
        }
    }

    if (NULL == (hNewStore = CertOpenStore(
            CERT_STORE_PROV_MEMORY,
            0,                      // dwEncodingType
            0,                      // hCryptProv
            0,                      // dwFlags
            NULL                    // pvPara
            )))
        goto OpenMemoryStoreError;

    if (pRegStore->dwFlags & CERT_REGISTRY_STORE_CLIENT_GPT_FLAG) {
        fResult = OpenAllFromGptRegistry(pRegStore, hNewStore);
        pRegStore->fTouched = FALSE;
    } else if (pRegStore->dwFlags & CERT_REGISTRY_STORE_SERIALIZED_FLAG) {
        fResult = OpenAllFromSerializedRegistry(pRegStore, hNewStore);
        pRegStore->fTouched = FALSE;
    } else
        fResult = OpenAllFromRegistry(pRegStore, hNewStore);

    if (!fResult) {
        if (ERROR_KEY_DELETED == GetLastError())
            fResult = TRUE;
    }

    if (fResult) {
        if (pRegStore->fProtected) {
            BOOL fProtected;

            // For the "Root" delete any roots that aren't in the protected root
            // list.
            if (!IPR_DeleteUnprotectedRootsFromStore(
                    hNewStore,
                    &fProtected
                    )) goto DeleteUnprotectedRootsError;
        }

        // Set fResync to inhibit the sync from writing back to the registry.
        pRegStore->fResync = TRUE;
        I_CertSyncStore(pRegStore->hCertStore, hNewStore);
        pRegStore->fResync = FALSE;
    }

CommonReturn:
    UnlockRegStore(pRegStore);
    if (hNewStore)
        CertCloseStore(hNewStore, 0);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
TRACE_ERROR(NotifyChangeError)
TRACE_ERROR(OpenMemoryStoreError)
TRACE_ERROR(DeleteUnprotectedRootsError)
}


STATIC BOOL RegistryNotifyChange(
    IN PREG_STORE pRegStore,
    IN HANDLE hEvent
    )
{
    BOOL fResult;
    HANDLE hMyNotifyChange;

    LockRegStore(pRegStore);

    hMyNotifyChange = pRegStore->hMyNotifyChange;
    if (NULL == hMyNotifyChange) {
        // Create "my" event and register it to be signaled for any changes
        if (NULL == (hMyNotifyChange = CreateEvent(
                NULL,       // lpsa
                FALSE,      // fManualReset
                FALSE,      // fInitialState
                NULL)))     // lpszEventName
            goto CreateEventError;

        // For the first notification, want to ensure the store is in sync.
        // Also does a RegNotifyChange
        if (!ResyncFromRegistry(pRegStore, hMyNotifyChange)) {
            DWORD dwErr = GetLastError();
            CloseHandle(hMyNotifyChange);
            SetLastError(dwErr);
            goto ResyncFromRegistryError;
        }

        // Note, must update after making the above Resync call to
        // force the store to be resync'ed
        pRegStore->hMyNotifyChange = hMyNotifyChange;
    }
    // else
    //  Already created and registered my event

    if (hEvent) {
        if (!RegNotifyChange(pRegStore, hEvent))
            goto NotifyChangeError;
    }

    fResult = TRUE;
CommonReturn:
    UnlockRegStore(pRegStore);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(CreateEventError)
TRACE_ERROR(ResyncFromRegistryError)
TRACE_ERROR(NotifyChangeError)
}


STATIC BOOL WINAPI RegStoreProvControl(
        IN HCERTSTOREPROV hStoreProv,
        IN DWORD dwFlags,
        IN DWORD dwCtrlType,
        IN void const *pvCtrlPara
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;

    switch (dwCtrlType) {
        case CERT_STORE_CTRL_RESYNC:
            {
                HANDLE *phEvent = (HANDLE *) pvCtrlPara;
                HANDLE hEvent = phEvent ? *phEvent : NULL;
                fResult = ResyncFromRegistry(pRegStore, hEvent);
            }
            break;
        case CERT_STORE_CTRL_NOTIFY_CHANGE:
            {
                HANDLE *phEvent = (HANDLE *) pvCtrlPara;
                HANDLE hEvent = phEvent ? *phEvent : NULL;
                fResult = RegistryNotifyChange(pRegStore, hEvent);
            }
            break;
        case CERT_STORE_CTRL_COMMIT:
            if (pRegStore->dwFlags & CERT_REGISTRY_STORE_CLIENT_GPT_FLAG)
                fResult = CommitAllToGptRegistry(pRegStore, dwFlags);
            else if (pRegStore->dwFlags & CERT_REGISTRY_STORE_SERIALIZED_FLAG)
                fResult = CommitAllToSerializedRegistry(pRegStore, dwFlags);
            else
                fResult = TRUE;
            break;
        default:
            goto NotSupported;
    }

CommonReturn:
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(NotSupported, ERROR_CALL_NOT_IMPLEMENTED)
}

#endif

//+=========================================================================
//  System and physical store support functions
//==========================================================================

STATIC BOOL HasBackslash(
    IN LPCWSTR pwsz
    )
{
    WCHAR wch;

    if (NULL == pwsz)
        return FALSE;

    while (L'\0' != (wch = *pwsz++)) {
        if (L'\\' == wch)
            return TRUE;
    }
    return FALSE;
}

static inline LPCSTR GetSystemStoreLocationOID(
    IN DWORD dwFlags
    )
{
    return (LPCSTR)(DWORD_PTR) ((dwFlags & CERT_SYSTEM_STORE_LOCATION_MASK) >>
        CERT_SYSTEM_STORE_LOCATION_SHIFT);
}

static inline DWORD GetSystemStoreLocationID(
    IN DWORD dwFlags
    )
{
    return ((dwFlags & CERT_SYSTEM_STORE_LOCATION_MASK) >>
        CERT_SYSTEM_STORE_LOCATION_SHIFT);
}

static inline BOOL IsSystemStoreLocationInRegistry(
    IN DWORD dwFlags
    )
{
    DWORD dwID = GetSystemStoreLocationID(dwFlags);
    return (dwID < NUM_SYSTEM_STORE_LOCATION &&
        0 == (rgSystemStoreLocationInfo[dwID].dwFlags &
                NOT_IN_REGISTRY_SYSTEM_STORE_LOCATION_FLAG));
}

static inline BOOL IsRemotableSystemStoreLocationInRegistry(
    IN DWORD dwFlags
    )
{
    DWORD dwID = GetSystemStoreLocationID(dwFlags);
    return (dwID < NUM_SYSTEM_STORE_LOCATION &&
        0 != (rgSystemStoreLocationInfo[dwID].dwFlags &
                REMOTABLE_SYSTEM_STORE_LOCATION_FLAG));
}


static inline BOOL IsSerializedSystemStoreLocationInRegistry(
    IN DWORD dwFlags
    )
{
    DWORD dwID = GetSystemStoreLocationID(dwFlags);
    return (dwID < NUM_SYSTEM_STORE_LOCATION &&
        0 != (rgSystemStoreLocationInfo[dwID].dwFlags &
                SERIALIZED_SYSTEM_STORE_LOCATION_FLAG));
}

STATIC BOOL IsPredefinedSystemStore(
    IN LPCWSTR pwszSystemName,
    IN DWORD dwFlags
    )
{
    DWORD i;
    DWORD dwCheckFlag;
    DWORD dwLocID;
    DWORD dwPredefinedSystemFlags;

    dwLocID = GetSystemStoreLocationID(dwFlags);
    assert(NUM_SYSTEM_STORE_LOCATION > dwLocID);
    dwPredefinedSystemFlags =
        rgSystemStoreLocationInfo[dwLocID].dwPredefinedSystemFlags;

    for (i = 0, dwCheckFlag = 1; i < NUM_PREDEFINED_SYSTEM_STORE;
                                        i++, dwCheckFlag = dwCheckFlag << 1) {
        if ((dwCheckFlag & dwPredefinedSystemFlags) &&
                0 == _wcsicmp(rgpwszPredefinedSystemStore[i], pwszSystemName))
            return TRUE;
    }
    return FALSE;
}

#define UNICODE_SYSTEM_PROVIDER_FLAG    0x1
#define ASCII_SYSTEM_PROVIDER_FLAG      0x2
#define PHYSICAL_PROVIDER_FLAG          0x4

STATIC DWORD GetSystemProviderFlags(
    IN LPCSTR pszStoreProvider
    )
{
    DWORD dwFlags;

    if (0xFFFF < (DWORD_PTR) pszStoreProvider &&
            CONST_OID_STR_PREFIX_CHAR == pszStoreProvider[0])
        // Convert "#<number>" string to its corresponding constant OID value
        pszStoreProvider = (LPCSTR)(DWORD_PTR) atol(pszStoreProvider + 1);

    dwFlags = 0;
    if (CERT_STORE_PROV_SYSTEM_A == pszStoreProvider)
        dwFlags = ASCII_SYSTEM_PROVIDER_FLAG;
    else if (CERT_STORE_PROV_SYSTEM_W == pszStoreProvider)
        dwFlags = UNICODE_SYSTEM_PROVIDER_FLAG;
    else if (CERT_STORE_PROV_SYSTEM_REGISTRY_A == pszStoreProvider)
        dwFlags = ASCII_SYSTEM_PROVIDER_FLAG;
    else if (CERT_STORE_PROV_SYSTEM_REGISTRY_W == pszStoreProvider)
        dwFlags = UNICODE_SYSTEM_PROVIDER_FLAG;
    else if (CERT_STORE_PROV_PHYSICAL_W == pszStoreProvider)
        dwFlags = UNICODE_SYSTEM_PROVIDER_FLAG | PHYSICAL_PROVIDER_FLAG;
    else if (0xFFFF < (DWORD_PTR) pszStoreProvider) {
        if (0 == _stricmp(sz_CERT_STORE_PROV_SYSTEM_W, pszStoreProvider))
            dwFlags = UNICODE_SYSTEM_PROVIDER_FLAG;
        else if (0 == _stricmp(sz_CERT_STORE_PROV_SYSTEM_REGISTRY_W,
                pszStoreProvider))
            dwFlags = UNICODE_SYSTEM_PROVIDER_FLAG;
        else if (0 == _stricmp(sz_CERT_STORE_PROV_PHYSICAL_W,
                pszStoreProvider))
            dwFlags = UNICODE_SYSTEM_PROVIDER_FLAG | PHYSICAL_PROVIDER_FLAG;
    }

    return dwFlags;
}

STATIC LPCSTR ChangeAsciiToUnicodeProvider(
    IN LPCSTR pszStoreProvider
    )
{
    LPCSTR pszUnicodeProvider = NULL;

    if (0xFFFF < (DWORD_PTR) pszStoreProvider &&
            CONST_OID_STR_PREFIX_CHAR == pszStoreProvider[0])
        // Convert "#<number>" string to its corresponding constant OID value
        pszStoreProvider = (LPCSTR)(DWORD_PTR) atol(pszStoreProvider + 1);

    if (CERT_STORE_PROV_SYSTEM_A == pszStoreProvider)
        pszUnicodeProvider = CERT_STORE_PROV_SYSTEM_W;
    else if (CERT_STORE_PROV_SYSTEM_REGISTRY_A == pszStoreProvider)
        pszUnicodeProvider = CERT_STORE_PROV_SYSTEM_REGISTRY_W;

    assert(pszUnicodeProvider);
    return pszUnicodeProvider;
}


STATIC void FreeSystemNameInfo(
    IN PSYSTEM_NAME_INFO pInfo
    )
{
    DWORD i;
    for (i = 0; i < SYSTEM_NAME_PATH_COUNT; i++) {
        if (pInfo->rgpwszName[i]) {
            PkiFree(pInfo->rgpwszName[i]);
            pInfo->rgpwszName[i] = NULL;
        }
    }
}

//+-------------------------------------------------------------------------
//  If CERT_SYSTEM_STORE_RELOCATE_FLAG is set in dwFlags, then, treat the
//  parameter as a pointer to a relocate data structure consisting of
//  the relocate HKEY base and the pointer to the system name path.
//  Otherwise, treat the parameter as a pointer to the system name path.
//
//  Parses and validates the system name path according to the system store
//  location and the number of required System and Physical name components.
//  All name components are separated by the backslash, "\", character.
//
//  Depending on the system store location and the number of required System
//  and Physical name components, the system name path can have the following
//  name components:
//
//  CERT_SYSTEM_STORE_CURRENT_USER or
//  CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY
//      []
//      SystemName
//      SystemName\PhysicalName
//  CERT_SYSTEM_STORE_LOCAL_MACHINE or
//  CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY or
//  CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE
//      []
//      [[\\]ComputerName]
//      [[\\]ComputerName\]SystemName
//      [[\\]ComputerName\]SystemName\PhysicalName
//  CERT_SYSTEM_STORE_CURRENT_SERVICE
//      []
//      SystemName
//      SystemName\PhysicalName
//  CERT_SYSTEM_STORE_SERVICES
//      []
//      [\\ComputerName]
//      [[\\]ComputerName\]
//      [ServiceName]
//      [[\\]ComputerName\ServiceName]
//      [[\\]ComputerName\]ServiceName\SystemName
//      [[\\]ComputerName\]ServiceName\SystemName\PhysicalName
//  CERT_SYSTEM_STORE_USERS
//      []
//      [\\ComputerName]
//      [[\\]ComputerName\]
//      [UserName]
//      [[\\]ComputerName\UserName]
//      [[\\]ComputerName\]UserName\SystemName
//      [[\\]ComputerName\]UserName\SystemName\PhysicalName
//
//  For enumeration, where cReqName = 0, all store locations allow the no name
//  components option. CERT_SYSTEM_STORE_CURRENT_USER,
//  CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY,
//  CERT_SYSTEM_CURRENT_SERVICE only allow the no name component option.
//
//  The leading \\ before the ComputerName is optional.
//
//  A PhysicalName always requires a preceding SystemName.
//
//  For CERT_SYSTEM_STORE_SERVICES or CERT_SYSTEM_STORE_USERS,
//  for enumeration, if only a single
//  name component, then, interpretted as a ServiceName or UserName unless it
//  contains a leading \\ or a trailing \, in which case its interpretted as a
//  ComputerName. Otherwise, when not enumeration, the ServiceName or UserName
//  is required.
//--------------------------------------------------------------------------
STATIC BOOL ParseSystemStorePara(
    IN const void *pvPara,
    IN DWORD dwFlags,
    IN DWORD cReqName,      // 0 for enum, 1 for OpenSystem, 2 for OpenPhysical
    OUT PSYSTEM_NAME_INFO pInfo
    )
{
    LPCWSTR pwszPath;       // not allocated
    BOOL fResult;
    DWORD cMaxOptName;
    DWORD cMaxTotalName;
    DWORD cOptName;
    DWORD cTotalName;
    BOOL fHasComputerNameBackslashes;
    DWORD i;

    LPCWSTR pwszEnd;
    LPCWSTR pwsz;
    LPCWSTR rgpwszStart[SYSTEM_NAME_PATH_COUNT];
    DWORD cchName[SYSTEM_NAME_PATH_COUNT];

    memset(pInfo, 0, sizeof(*pInfo));
    if (dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG) {
        PCERT_SYSTEM_STORE_RELOCATE_PARA pRelocatePara =
            (PCERT_SYSTEM_STORE_RELOCATE_PARA) pvPara;

        if (NULL == pRelocatePara)
            goto NullRelocateParaError;

        if (NULL == pRelocatePara->hKeyBase)
            goto NullRelocateHKEYError;
        pInfo->hKeyBase = pRelocatePara->hKeyBase;
        pwszPath = pRelocatePara->pwszSystemStore;
    } else
        pwszPath = (LPCWSTR) pvPara;

    if (NULL == pwszPath || L'\0' == *pwszPath) {
        if (0 == cReqName)
            goto SuccessReturn;
        else
            goto MissingSystemName;
    }

    dwFlags &= CERT_SYSTEM_STORE_LOCATION_MASK;
    switch (dwFlags) {
        case CERT_SYSTEM_STORE_CURRENT_USER:
        case CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY:
        case CERT_SYSTEM_STORE_CURRENT_SERVICE:
            cMaxOptName = 0;
            break;
        case CERT_SYSTEM_STORE_LOCAL_MACHINE:
        case CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY:
        case CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE:
            cMaxOptName = 1;        // Allow ComputerName
            break;
        case CERT_SYSTEM_STORE_SERVICES:
        case CERT_SYSTEM_STORE_USERS:
            // Allow ComputerName and/or ServiceOrUserName
            cMaxOptName = 2;
            break;
        default:
            goto InvalidStoreLocation;
    }

    cMaxTotalName = cReqName + cMaxOptName;
    assert(cReqName <= SERVICE_NAME_INDEX);
    assert(cMaxTotalName <= SYSTEM_NAME_PATH_COUNT);
    if (0 == cMaxTotalName)
        goto MachineOrServiceNameNotAllowed;

    if (L'\\' == pwszPath[0] && L'\\' == pwszPath[1]) {
        pwszPath += 2;
        fHasComputerNameBackslashes = TRUE;
    } else
        fHasComputerNameBackslashes = FALSE;

    // Starting at the end, get up through cMaxTotalName strings separated
    // by backslashes. Note, don't parse the left-most name component. This
    // allows a ComputerName to contain embedded backslashes.
    pwszEnd = pwszPath + wcslen(pwszPath);
    pwsz = pwszEnd;

    cTotalName = 0;
    while (cTotalName < cMaxTotalName - 1) {
        while (pwsz > pwszPath && L'\\' != *pwsz)
            pwsz--;
        if (L'\\' != *pwsz) {
            // Left-most name component.
            assert(pwsz == pwszPath);
            break;
        }
        assert(L'\\' == *pwsz);
        cchName[cTotalName] = (DWORD)(pwszEnd - pwsz) - 1; // exclude "\"
        rgpwszStart[cTotalName] = pwsz + 1;         // exclude "\"
        cTotalName++;

        pwszEnd = pwsz;         // remember pointer to "\"
        if (pwsz == pwszPath)
            // Left-most name component is empty
            break;
        pwsz--;                 // skip before the "\"
    }
    // Left-most name component. Note, it may have embedded backslashes
    cchName[cTotalName] = (DWORD)(pwszEnd - pwszPath);
    rgpwszStart[cTotalName] = pwszPath;
    cTotalName++;

    if (cTotalName < cReqName)
        goto MissingSystemOrPhysicalName;

    // Allocate and copy the required name components
    for (i = 0; i < cReqName; i++) {
        if (0 == cchName[i])
            goto EmptySystemOrPhysicalName;
        if (NULL == (pInfo->rgpwszName[SERVICE_NAME_INDEX - cReqName + i] =
                AllocAndCopyString(rgpwszStart[i], cchName[i])))
            goto OutOfMemory;
    }

    cOptName = cTotalName - cReqName;
    assert(cOptName || cReqName);
    if (0 == cOptName) {
        assert(cReqName);
        // No ComputerName and/or ServiceName prefix

        // Check if left-most name component (SystemName) has any backslashes
        assert(pInfo->rgpwszName[SYSTEM_NAME_INDEX]);
        if (fHasComputerNameBackslashes || HasBackslash(
                pInfo->rgpwszName[SYSTEM_NAME_INDEX]))
            goto InvalidBackslashInSystemName;
        if (CERT_SYSTEM_STORE_SERVICES == dwFlags ||
                CERT_SYSTEM_STORE_USERS == dwFlags)
            // For non-enumeration, require the ServiceName
            goto MissingServiceOrUserName;
    } else {
        if (CERT_SYSTEM_STORE_SERVICES == dwFlags ||
                CERT_SYSTEM_STORE_USERS == dwFlags) {
            // ServiceName or UserName prefix

            if (0 == cchName[cReqName] ||
                    (1 == cOptName && fHasComputerNameBackslashes)) {
                if (0 != cReqName)
                    goto MissingServiceOrUserName;
                // else
                //  ComputerName only Enumeration with either:
                //      ComputerName\       <- trailing backslash
                //      \\ComputerName      <- leading backslashes
                //      \\ComputerName\     <- both
            } else {
                if (NULL == (pInfo->rgpwszName[SERVICE_NAME_INDEX] =
                        AllocAndCopyString(rgpwszStart[cReqName],
                            cchName[cReqName])))
                    goto OutOfMemory;
            }
        }

        if (CERT_SYSTEM_STORE_LOCAL_MACHINE == dwFlags ||
                CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY == dwFlags ||
                CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE == dwFlags ||
                2 == cOptName || fHasComputerNameBackslashes) {
            // ComputerName prefix
            DWORD cchComputer = cchName[cTotalName - 1];
            if (0 == cchComputer)
                goto EmptyComputerName;

            if (pInfo->hKeyBase)
                goto BothRemoteAndRelocateNotAllowed;

            if (NULL == (pInfo->rgpwszName[COMPUTER_NAME_INDEX] =
                    (LPWSTR) PkiNonzeroAlloc(
                    (2 + cchComputer + 1) * sizeof(WCHAR))))
                goto OutOfMemory;
            wcscpy(pInfo->rgpwszName[COMPUTER_NAME_INDEX], L"\\\\");
            memcpy((BYTE *) (pInfo->rgpwszName[COMPUTER_NAME_INDEX] + 2),
                (BYTE *) rgpwszStart[cTotalName -1],
                cchComputer * sizeof(WCHAR));
            *(pInfo->rgpwszName[COMPUTER_NAME_INDEX] + 2 + cchComputer) = L'\0';
        }
    }

SuccessReturn:
    fResult = TRUE;
CommonReturn:
    return fResult;
ErrorReturn:
    FreeSystemNameInfo(pInfo);
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(MissingSystemName, E_INVALIDARG)
SET_ERROR(NullRelocateParaError, E_INVALIDARG)
SET_ERROR(NullRelocateHKEYError, E_INVALIDARG)
SET_ERROR(MissingSystemOrPhysicalName, E_INVALIDARG)
SET_ERROR(InvalidStoreLocation, E_INVALIDARG)
SET_ERROR(MachineOrServiceNameNotAllowed, E_INVALIDARG)
SET_ERROR(EmptySystemOrPhysicalName, E_INVALIDARG)
SET_ERROR(InvalidBackslashInSystemName, E_INVALIDARG)
SET_ERROR(MissingServiceOrUserName, E_INVALIDARG)
SET_ERROR(EmptyComputerName, E_INVALIDARG)
SET_ERROR(BothRemoteAndRelocateNotAllowed, E_INVALIDARG)
TRACE_ERROR(OutOfMemory)
}

typedef struct _SYSTEM_NAME_GROUP {
    DWORD cName;
    LPCWSTR *rgpwszName;
} SYSTEM_NAME_GROUP, *PSYSTEM_NAME_GROUP;

//+-------------------------------------------------------------------------
//  Formats a System Name Path by concatenating together the name components
//  with an intervening "\" separator.
//--------------------------------------------------------------------------
STATIC LPWSTR FormatSystemNamePath(
    IN DWORD cNameGroup,
    IN SYSTEM_NAME_GROUP rgNameGroup[]
    )
{
    DWORD cchPath;
    LPWSTR pwszPath;
    BOOL fFirst;
    DWORD iGroup;

    // First, get total length of formatted path
    cchPath = 0;
    fFirst = TRUE;
    for (iGroup = 0; iGroup < cNameGroup; iGroup++) {
        DWORD iName;
        for (iName = 0; iName < rgNameGroup[iGroup].cName; iName++) {
            LPCWSTR pwszName = rgNameGroup[iGroup].rgpwszName[iName];
            if (pwszName && *pwszName) {
                if (fFirst)
                    fFirst = FALSE;
                else
                    cchPath++;          // "\" separator
                cchPath += wcslen(pwszName);
            }
        }
    }
    cchPath++;          // "\0" terminator

    if (NULL == (pwszPath = (LPWSTR) PkiNonzeroAlloc(cchPath * sizeof(WCHAR))))
        return NULL;

    // Now do concatenated copies with intervening '\'
    fFirst = TRUE;
    for (iGroup = 0; iGroup < cNameGroup; iGroup++) {
        DWORD iName;
        for (iName = 0; iName < rgNameGroup[iGroup].cName; iName++) {
            LPCWSTR pwszName = rgNameGroup[iGroup].rgpwszName[iName];
            if (pwszName && *pwszName) {
                if (fFirst) {
                    wcscpy(pwszPath, pwszName);
                    fFirst = FALSE;
                } else {
                    wcscat(pwszPath, L"\\");
                    wcscat(pwszPath, pwszName);
                }
            }
        }
    }
    if (fFirst)
        // Empty string
        *pwszPath = L'\0';
    return pwszPath;
}

//+-------------------------------------------------------------------------
//  If the SystemNameInfo has a non-NULL hKeyBase, then, the returned
//  pvPara is a pointer to a CERT_SYSTEM_STORE_RELOCATE_PARA containing both
//  the hKeyBase and the formatted system name path. Otherwise, returns
//  pointer to only the formatted system name path.
//
//  Calls the above FormatSystemNamePath() to do the actual formatting.
//--------------------------------------------------------------------------
STATIC void * FormatSystemNamePara(
    IN DWORD cNameGroup,
    IN SYSTEM_NAME_GROUP rgNameGroup[],
    IN PSYSTEM_NAME_INFO pSystemNameInfo
    )
{
    if (NULL == pSystemNameInfo->hKeyBase)
        return FormatSystemNamePath(cNameGroup, rgNameGroup);
    else {
        PCERT_SYSTEM_STORE_RELOCATE_PARA pRelocatePara;

        if (NULL == (pRelocatePara =
                (PCERT_SYSTEM_STORE_RELOCATE_PARA) PkiNonzeroAlloc(
                    sizeof(CERT_SYSTEM_STORE_RELOCATE_PARA))))
            return NULL;

        pRelocatePara->hKeyBase = pSystemNameInfo->hKeyBase;

        if (NULL == (pRelocatePara->pwszSystemStore = FormatSystemNamePath(
                cNameGroup, rgNameGroup))) {
            PkiFree(pRelocatePara);
            return NULL;
        } else
            return pRelocatePara;
    }
}

STATIC void FreeSystemNamePara(
    IN void *pvSystemNamePara,
    IN PSYSTEM_NAME_INFO pSystemNameInfo
    )
{
    if (pvSystemNamePara) {
        if (pSystemNameInfo->hKeyBase) {
            PCERT_SYSTEM_STORE_RELOCATE_PARA pRelocatePara =
                (PCERT_SYSTEM_STORE_RELOCATE_PARA) pvSystemNamePara;
            PkiFree((LPWSTR) pRelocatePara->pwszSystemStore);
        }
        PkiFree(pvSystemNamePara);
    }
}


//+-------------------------------------------------------------------------
//  Localizes the physical, system and service name components. If unable
//  to find a localized name string, uses the unlocalized name component.
//
//  Re-formats the system name path with intervening backslashes and
//  sets the store's CERT_STORE_LOCALIZED_NAME_PROP_ID property.
//--------------------------------------------------------------------------
STATIC void SetLocalizedNameStoreProperty(
    IN HCERTSTORE hCertStore,
    IN PSYSTEM_NAME_INFO pSystemNameInfo
    )
{
    LPWSTR pwszLocalizedPath = NULL;
    LPCWSTR rgpwszLocalizedName[SYSTEM_NAME_PATH_COUNT];
    SYSTEM_NAME_GROUP NameGroup;
    CRYPT_DATA_BLOB Property;
    DWORD i;

    // Except for the computer name, try to get localized name components.
    // If unable to find a localized name, use the original name component.
    for (i = 0; i < SYSTEM_NAME_PATH_COUNT; i++) {
        LPCWSTR pwszName;
        LPCWSTR pwszLocalizedName;

        pwszName = pSystemNameInfo->rgpwszName[i];
        if (NULL == pwszName || COMPUTER_NAME_INDEX == i)
            pwszLocalizedName = pwszName;
        else {
            // Returned pwszLocalizedName isn't allocated
            if (NULL == (pwszLocalizedName = CryptFindLocalizedName(
                    pwszName)) || L'\0' == *pwszLocalizedName)
                pwszLocalizedName = pwszName;
        }

        // Before formatting, need to reverse.
        rgpwszLocalizedName[SYSTEM_NAME_PATH_COUNT - 1 - i] =
            pwszLocalizedName;
    }

    NameGroup.cName = SYSTEM_NAME_PATH_COUNT;
    NameGroup.rgpwszName = rgpwszLocalizedName;
    if (NULL == (pwszLocalizedPath = FormatSystemNamePath(1, &NameGroup)))
        goto FormatSystemNamePathError;

    Property.pbData = (BYTE *) pwszLocalizedPath;
    Property.cbData = (wcslen(pwszLocalizedPath) + 1) * sizeof(WCHAR);
    if (!CertSetStoreProperty(
            hCertStore,
            CERT_STORE_LOCALIZED_NAME_PROP_ID,
            0,                                  // dwFlags
            (const void *) &Property
            ))
        goto SetStorePropertyError;

CommonReturn:
    PkiFree(pwszLocalizedPath);
    return;
ErrorReturn:
    goto CommonReturn;
TRACE_ERROR(FormatSystemNamePathError)
TRACE_ERROR(SetStorePropertyError)
}

#ifndef CE_BUILD
//+-------------------------------------------------------------------------
//  For NT, get the formatted SID. For Win95, get the current user name.
//--------------------------------------------------------------------------
STATIC LPWSTR GetCurrentServiceOrUserName()
{
    LPWSTR pwszCurrentService = NULL;

    if (!FIsWinNT()) {
        DWORD cch = _MAX_PATH;
        if (NULL == (pwszCurrentService = (LPWSTR) PkiNonzeroAlloc(
                (cch + 1) * sizeof(WCHAR))))
            goto OutOfMemory;
        if (!GetUserNameU(pwszCurrentService, &cch))
            goto GetUserNameError;
    } else {
        DWORD cch = 256;
        if (NULL == (pwszCurrentService = (LPWSTR) PkiNonzeroAlloc(
                (cch + 1) * sizeof(WCHAR))))
            goto OutOfMemory;
        if (!GetUserTextualSidHKCU(pwszCurrentService, &cch)) {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                goto GetUserTextualSidHKUCError;
            PkiFree(pwszCurrentService);
            if (NULL == (pwszCurrentService = (LPWSTR) PkiNonzeroAlloc(
                    (cch + 1) * sizeof(WCHAR))))
                goto OutOfMemory;
            if (!GetUserTextualSidHKCU(pwszCurrentService, &cch))
                goto GetUserTextualSidHKUCError;
        }
    }

CommonReturn:
    return pwszCurrentService;

ErrorReturn:
    if (pwszCurrentService)
        wcscpy(pwszCurrentService, DEFAULT_USER_NAME);
    goto CommonReturn;
TRACE_ERROR(OutOfMemory)
TRACE_ERROR(GetUserNameError)
TRACE_ERROR(GetUserTextualSidHKUCError)
}

//+-------------------------------------------------------------------------
//  For NT and Win95, get the computer name
//--------------------------------------------------------------------------
STATIC LPWSTR GetCurrentComputerName()
{
    LPWSTR pwszCurrentComputer = NULL;
    DWORD cch = _MAX_PATH;
    if (NULL == (pwszCurrentComputer = (LPWSTR) PkiNonzeroAlloc(
            (cch + 1) * sizeof(WCHAR))))
        goto OutOfMemory;
    if (!GetComputerNameU(pwszCurrentComputer, &cch))
        goto GetComputerNameError;

CommonReturn:
    return pwszCurrentComputer;

ErrorReturn:
    PkiFree(pwszCurrentComputer);
    pwszCurrentComputer = NULL;
    goto CommonReturn;
TRACE_ERROR(OutOfMemory)
TRACE_ERROR(GetComputerNameError)
}

#else // CE_BUILD
#define GetCurrentServiceOrUserName() (NULL)
#define GetCurrentComputerName() (NULL)
#endif

//+-------------------------------------------------------------------------
//  Uses the store location in the upper word of dwFlags, the parsed
//  System Name components consisting of: computer, service/user, system and
//  physical names, and the optional SubKey name to open the appropriate
//  registry key. If the Computer name is non-NULL, does a RegConnectRegistry
//  to connect a registry key on a remote computer. If the hKeyBase is
//  non-NULL, does a relocated open instead of using HKCU or HKLM.
//--------------------------------------------------------------------------
STATIC LPWSTR FormatSystemRegPath(
    IN PSYSTEM_NAME_INFO pInfo,
    IN OPTIONAL LPCWSTR pwszSubKeyName,
    IN DWORD dwFlags,
    OUT HKEY *phKey
    )
{
    HKEY hKey = NULL;
    LPWSTR pwszRegPath = NULL;
    LPWSTR pwszCurrentService = NULL;
    DWORD dwStoreLocation;

    SYSTEM_NAME_GROUP rgNameGroup[3];
    DWORD cNameGroup;
    LPCWSTR rgpwszService[3];
    LPCWSTR rgpwszUser[2];
    LPCWSTR rgpwszStore[3];

    if (pwszSubKeyName) {
        cNameGroup = 3;
        rgNameGroup[2].cName = 1;
        rgNameGroup[2].rgpwszName = &pwszSubKeyName;
    } else
        cNameGroup = 2;

    dwStoreLocation = dwFlags & CERT_SYSTEM_STORE_LOCATION_MASK;
    switch (dwStoreLocation) {
        case CERT_SYSTEM_STORE_CURRENT_SERVICE:
        case CERT_SYSTEM_STORE_SERVICES:
            rgNameGroup[0].cName = 3;
            rgNameGroup[0].rgpwszName = rgpwszService;
            rgpwszService[0] = SERVICES_REGPATH;
            rgpwszService[2] = SYSTEM_CERTIFICATES_SUBKEY_NAME;

            if (CERT_SYSTEM_STORE_CURRENT_SERVICE == dwStoreLocation) {
                assert(NULL == pInfo->rgpwszName[COMPUTER_NAME_INDEX]);
                assert(NULL == pInfo->rgpwszName[SERVICE_NAME_INDEX]);
                if (NULL == (pwszCurrentService =
                        GetCurrentServiceOrUserName()))
                    goto GetCurrentServiceNameError;
                rgpwszService[1] = pwszCurrentService;
            } else {
                if (NULL == pInfo->rgpwszName[SERVICE_NAME_INDEX]) {
                    // May be NULL for CertEnumSystemStore
                    assert(NULL == pInfo->rgpwszName[SYSTEM_NAME_INDEX]);
                    assert(NULL == pInfo->rgpwszName[PHYSICAL_NAME_INDEX]);
                    rgNameGroup[0].cName = 1;
                } else
                    rgpwszService[1] = pInfo->rgpwszName[SERVICE_NAME_INDEX];
            }
            break;
        case CERT_SYSTEM_STORE_CURRENT_USER:
        case CERT_SYSTEM_STORE_LOCAL_MACHINE:
            rgpwszUser[0] = SYSTEM_STORE_REGPATH;
            rgNameGroup[0].cName = 1;
            rgNameGroup[0].rgpwszName = rgpwszUser;
            break;
        case CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY:
        case CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY:
            rgpwszUser[0] = GROUP_POLICY_STORE_REGPATH;
            rgNameGroup[0].cName = 1;
            rgNameGroup[0].rgpwszName = rgpwszUser;
            break;
        case CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE:
            rgpwszUser[0] = ENTERPRISE_STORE_REGPATH;
            rgNameGroup[0].cName = 1;
            rgNameGroup[0].rgpwszName = rgpwszUser;
            break;
        case CERT_SYSTEM_STORE_USERS:
            if (NULL == pInfo->rgpwszName[USER_NAME_INDEX]) {
                // May be NULL for CertEnumSystemStore
                assert(NULL == pInfo->rgpwszName[SYSTEM_NAME_INDEX]);
                assert(NULL == pInfo->rgpwszName[PHYSICAL_NAME_INDEX]);
                rgNameGroup[0].cName = 0;
            } else {
                rgpwszUser[0] = pInfo->rgpwszName[USER_NAME_INDEX];
                rgpwszUser[1] = SYSTEM_STORE_REGPATH;
                rgNameGroup[0].cName = 2;
                rgNameGroup[0].rgpwszName = rgpwszUser;
            }
            break;
        default:
            goto InvalidArg;
    }

    rgNameGroup[1].rgpwszName = rgpwszStore;
    rgpwszStore[0] = pInfo->rgpwszName[SYSTEM_NAME_INDEX];
    if (pInfo->rgpwszName[PHYSICAL_NAME_INDEX]) {
        assert(pInfo->rgpwszName[SYSTEM_NAME_INDEX]);
        rgNameGroup[1].cName = 3;
        rgpwszStore[1] = PHYSICAL_STORES_SUBKEY_NAME;
        rgpwszStore[2] = pInfo->rgpwszName[PHYSICAL_NAME_INDEX];
    } else
        rgNameGroup[1].cName = 1;

#ifndef CE_BUILD
    if (pInfo->rgpwszName[COMPUTER_NAME_INDEX]) {
    	LONG err;
        assert(IsRemotableSystemStoreLocationInRegistry(dwFlags));
        assert(NULL == pInfo->hKeyBase);
        if (ERROR_SUCCESS != (err = RegConnectRegistryU(
                pInfo->rgpwszName[COMPUTER_NAME_INDEX],
                (CERT_SYSTEM_STORE_USERS == dwStoreLocation) ?
                    HKEY_USERS : HKEY_LOCAL_MACHINE,
                &hKey)))
            goto RegConnectRegistryError;
    } 
    else 
#endif
    if (pInfo->hKeyBase) {
        assert(dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG);
        hKey = pInfo->hKeyBase;
    } else {
        switch (dwStoreLocation) {
            case CERT_SYSTEM_STORE_CURRENT_USER:
            case CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY:
                hKey = HKEY_CURRENT_USER;
                break;
            case CERT_SYSTEM_STORE_LOCAL_MACHINE:
            case CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY:
            case CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE:
            case CERT_SYSTEM_STORE_CURRENT_SERVICE:
            case CERT_SYSTEM_STORE_SERVICES:
                hKey = HKEY_LOCAL_MACHINE;
                break;
            case CERT_SYSTEM_STORE_USERS:
                hKey = HKEY_USERS;
                break;
            default:
                goto InvalidArg;
        }
    }

    if (NULL == (pwszRegPath = FormatSystemNamePath(
            cNameGroup,
            rgNameGroup
            )))
        goto FormatSystemNamePathError;

CommonReturn:
    PkiFree(pwszCurrentService);
    *phKey = hKey;
    return pwszRegPath;
ErrorReturn:
    pwszRegPath = NULL;
    goto CommonReturn;

TRACE_ERROR(GetCurrentServiceNameError)
SET_ERROR(InvalidArg, E_INVALIDARG)
#ifndef CE_BUILD
SET_ERROR_VAR(RegConnectRegistryError, err)
#endif
TRACE_ERROR(FormatSystemNamePathError)
}

STATIC HKEY OpenSystemRegPathKey(
    IN PSYSTEM_NAME_INFO pInfo,
    IN OPTIONAL LPCWSTR pwszSubKeyName,
    IN DWORD dwFlags
    )
{
    LPWSTR pwszRegPath;
    HKEY hKey = NULL;
    HKEY hKeyRegPath;

    if (NULL == (pwszRegPath = FormatSystemRegPath(
            pInfo,
            pwszSubKeyName,
            dwFlags,
            &hKey
            )))
        goto FormatSystemRegPathError;

    hKeyRegPath = OpenSubKey(
        hKey,
        pwszRegPath,
        dwFlags
        );

CommonReturn:
    PkiFree(pwszRegPath);
    if (pInfo->rgpwszName[COMPUTER_NAME_INDEX] && hKey)
        ILS_CloseRegistryKey(hKey);
    return hKeyRegPath;
ErrorReturn:
    hKeyRegPath = NULL;
    goto CommonReturn;

TRACE_ERROR(FormatSystemRegPathError)
}


STATIC HKEY OpenSystemStore(
    IN const void *pvPara,
    IN DWORD dwFlags
    )
{
    HKEY hKey;
    SYSTEM_NAME_INFO SystemNameInfo;

    if (!ParseSystemStorePara(
            pvPara,
            dwFlags,
            1,                  // cReqName
            &SystemNameInfo))   // zero'ed on error
        goto ParseSystemStoreParaError;

    hKey = OpenSystemRegPathKey(
        &SystemNameInfo,
        NULL,               // pwszSubKeyName
        dwFlags
        );

CommonReturn:
    FreeSystemNameInfo(&SystemNameInfo);
    return hKey;
ErrorReturn:
    hKey = NULL;
    goto CommonReturn;
TRACE_ERROR(ParseSystemStoreParaError)
}

STATIC HKEY OpenPhysicalStores(
    IN const void *pvPara,
    IN DWORD dwFlags
    )
{
    HKEY hKey;
    SYSTEM_NAME_INFO SystemNameInfo;

    if (!ParseSystemStorePara(
            pvPara,
            dwFlags,
            1,                  // cReqName
            &SystemNameInfo))   // zero'ed on error
        goto ParseSystemStoreParaError;

    hKey = OpenSystemRegPathKey(
        &SystemNameInfo,
        PHYSICAL_STORES_SUBKEY_NAME,
        dwFlags
        );

CommonReturn:
    FreeSystemNameInfo(&SystemNameInfo);
    return hKey;
ErrorReturn:
    hKey = NULL;
    goto CommonReturn;
TRACE_ERROR(ParseSystemStoreParaError)
}


#ifdef CAPI_INCLUDE_REGISTER_STORE
//+-------------------------------------------------------------------------
//  Register a system store.
//
//  The upper word of the dwFlags parameter is used to specify the location of
//  the system store.
//
//  Set CERT_STORE_CREATE_NEW_FLAG to cause a failure if the system store
//  already exists in the store location.
//--------------------------------------------------------------------------
BOOL
WINAPI
CertRegisterSystemStore(
    IN const void *pvSystemStore,
    IN DWORD dwFlags,
    IN PCERT_SYSTEM_STORE_INFO pStoreInfo,
    IN OPTIONAL void *pvReserved
    )
{
    BOOL fResult;
    HKEY hKey;

    if (!IsSystemStoreLocationInRegistry(dwFlags)) {
        void *pvFuncAddr;
        HCRYPTOIDFUNCADDR hFuncAddr;

        if (!CryptGetOIDFunctionAddress(
                rghFuncSet[REGISTER_SYSTEM_STORE_FUNC_SET],
                0,                      // dwEncodingType,
                GetSystemStoreLocationOID(dwFlags),
                0,                      // dwFlags
                &pvFuncAddr,
                &hFuncAddr))
            return FALSE;

        fResult = ((PFN_REGISTER_SYSTEM_STORE) pvFuncAddr)(
            pvSystemStore,
            dwFlags,
            pStoreInfo,
            pvReserved
            );
        CryptFreeOIDFunctionAddress(hFuncAddr, 0);
        return fResult;
    }

    if (dwFlags & ~REGISTER_FLAGS_MASK)
        goto InvalidArg;

    if (NULL == (hKey = OpenSystemStore(pvSystemStore, dwFlags)))
        goto OpenSystemStoreError;
    RegCloseKey(hKey);
    fResult = TRUE;
CommonReturn:
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(InvalidArg, E_INVALIDARG)
TRACE_ERROR(OpenSystemStoreError)
}

//+-------------------------------------------------------------------------
//  Register a physical store for the specified system store.
//
//  The upper word of the dwFlags parameter is used to specify the location of
//  the system store.
//
//  Set CERT_STORE_CREATE_NEW_FLAG to cause a failure if the physical store
//  already exists in the system store.
//--------------------------------------------------------------------------
BOOL
WINAPI
CertRegisterPhysicalStore(
    IN const void *pvSystemStore,
    IN DWORD dwFlags,
    IN LPCWSTR pwszStoreName,
    IN PCERT_PHYSICAL_STORE_INFO pStoreInfo,
    IN OPTIONAL void *pvReserved
    )
{
    BOOL fResult;
    LONG err;
    HKEY hKey = NULL;

    SYSTEM_NAME_INFO SystemNameInfo;

    char szOID[34];
    LPCSTR pszOID;

    if (!IsSystemStoreLocationInRegistry(dwFlags)) {
        void *pvFuncAddr;
        HCRYPTOIDFUNCADDR hFuncAddr;

        if (!CryptGetOIDFunctionAddress(
                rghFuncSet[REGISTER_PHYSICAL_STORE_FUNC_SET],
                0,                      // dwEncodingType,
                GetSystemStoreLocationOID(dwFlags),
                0,                      // dwFlags
                &pvFuncAddr,
                &hFuncAddr))
            return FALSE;

        fResult = ((PFN_REGISTER_PHYSICAL_STORE) pvFuncAddr)(
            pvSystemStore,
            dwFlags,
            pwszStoreName,
            pStoreInfo,
            pvReserved
            );
        CryptFreeOIDFunctionAddress(hFuncAddr, 0);
        return fResult;
    }

    if (!ParseSystemStorePara(
            pvSystemStore,
            dwFlags,
            1,                  // cReqName
            &SystemNameInfo))   // zero'ed on error
        goto ParseSystemStoreParaError;

    if (NULL == pwszStoreName || L'\0' == *pwszStoreName ||
            HasBackslash(pwszStoreName))
        goto InvalidArg;
    assert(SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX]);
    assert(NULL == SystemNameInfo.rgpwszName[PHYSICAL_NAME_INDEX]);
    SystemNameInfo.rgpwszName[PHYSICAL_NAME_INDEX] = (LPWSTR) pwszStoreName;

    if (dwFlags & ~REGISTER_FLAGS_MASK)
        goto InvalidArg;

    if (NULL == pStoreInfo ||
            sizeof(CERT_PHYSICAL_STORE_INFO) > pStoreInfo->cbSize)
        goto InvalidArg;

    if (NULL == (hKey = OpenSystemRegPathKey(
            &SystemNameInfo,
            NULL,               // pwszSubKeyName
            dwFlags
            )))
        goto OpenSystemRegPathKeyError;

    pszOID = pStoreInfo->pszOpenStoreProvider;
    if (0xFFFF >= (DWORD_PTR) pszOID) {
        // Convert to "#<number>" string
        szOID[0] = CONST_OID_STR_PREFIX_CHAR;
        _ltoa((long) ((DWORD_PTR) pszOID), szOID + 1, 10);
        pszOID = szOID;
    }
    if (ERROR_SUCCESS != (err = RegSetValueExA(
            hKey,
            "OpenStoreProvider",
            0,          // dwReserved
            REG_SZ,
            (BYTE *) pszOID,
            strlen(pszOID) + 1)))
        goto RegSetOpenStoreProviderError;

    if (!WriteDWORDValueToRegistry(
            hKey,
            L"OpenEncodingType",
            pStoreInfo->dwOpenEncodingType))
        goto WriteDWORDError;
    if (!WriteDWORDValueToRegistry(
            hKey,
            L"OpenFlags",
            pStoreInfo->dwOpenFlags))
        goto WriteDWORDError;

    if (ERROR_SUCCESS != (err = RegSetValueExU(
            hKey,
            L"OpenParameters",
            0,          // dwReserved
            REG_BINARY,
            pStoreInfo->OpenParameters.pbData,
            pStoreInfo->OpenParameters.cbData)))
        goto RegSetOpenParametersError;

    if (!WriteDWORDValueToRegistry(
            hKey,
            L"Flags",
            pStoreInfo->dwFlags))
        goto WriteDWORDError;
    if (!WriteDWORDValueToRegistry(
            hKey,
            L"Priority",
            pStoreInfo->dwPriority))
        goto WriteDWORDError;

    fResult = TRUE;

CommonReturn:
    SystemNameInfo.rgpwszName[PHYSICAL_NAME_INDEX] = NULL;   // not allocated
    FreeSystemNameInfo(&SystemNameInfo);
    ILS_CloseRegistryKey(hKey);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(InvalidArg, E_INVALIDARG)
TRACE_ERROR(ParseSystemStoreParaError)
TRACE_ERROR(OpenSystemRegPathKeyError)
SET_ERROR_VAR(RegSetOpenStoreProviderError, err)
SET_ERROR_VAR(RegSetOpenParametersError, err)
TRACE_ERROR(WriteDWORDError)
}

#endif // CAPI_INCLUDE_REGISTER_STORE
// 
// Unregister functions used by CERT_DELETE_STORE

//+-------------------------------------------------------------------------
//  Unregister the specified system store.
//--------------------------------------------------------------------------
BOOL
WINAPI
CertUnregisterSystemStore(
    IN const void *pvSystemStore,
    IN DWORD dwFlags
    )
{
    BOOL fResult;
    HKEY hKey = NULL;
    SYSTEM_NAME_INFO SystemNameInfo;
    LPWSTR pwszStore;       // not allocated

    if (!IsSystemStoreLocationInRegistry(dwFlags)) {
        void *pvFuncAddr;
        HCRYPTOIDFUNCADDR hFuncAddr;

        if (!CryptGetOIDFunctionAddress(
                rghFuncSet[UNREGISTER_SYSTEM_STORE_FUNC_SET],
                0,                      // dwEncodingType,
                GetSystemStoreLocationOID(dwFlags),
                0,                      // dwFlags
                &pvFuncAddr,
                &hFuncAddr))
            return FALSE;

        fResult = ((PFN_UNREGISTER_SYSTEM_STORE) pvFuncAddr)(
            pvSystemStore,
            dwFlags
            );
        CryptFreeOIDFunctionAddress(hFuncAddr, 0);
        return fResult;
    }

    if (!ParseSystemStorePara(
            pvSystemStore,
            dwFlags,
            1,                      // cReqName
            &SystemNameInfo))       // zero'ed on error
        goto ParseSystemStoreParaError;

    if (dwFlags & ~UNREGISTER_FLAGS_MASK)
        goto InvalidArg;

    // Delete the SystemRegistry components
    if (NULL == (hKey = OpenSystemRegPathKey(
            &SystemNameInfo,
            NULL,                   // pwszSubKeyName
            dwFlags | CERT_STORE_OPEN_EXISTING_FLAG
            )))
        goto OpenSystemRegPathKeyError;
    if (!DeleteAllFromRegistry(
            hKey,
            SystemNameInfo.rgpwszName[COMPUTER_NAME_INDEX] ?
                    CERT_REGISTRY_STORE_REMOTE_FLAG : 0
            ))
        goto DeleteAllError;

    RegCloseKey(hKey);
    hKey = NULL;

    // Open SystemCertificates SubKey preceding the store. In order to do this
    // the SYSTEM_NAME component must be NULL.
    assert(SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX]);
    pwszStore = SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX];
    SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX] = NULL;
    hKey = OpenSystemRegPathKey(
            &SystemNameInfo,
            NULL,                   // pwszSubKeyName
            dwFlags | CERT_STORE_OPEN_EXISTING_FLAG
            );
    SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX] = pwszStore;
    if (NULL == hKey)
        goto OpenSystemRegPathKeyError;

    // Delete the remaining System components (such as PhysicalStores) and
    // the System store itself
    if (!RecursiveDeleteSubKey(hKey, pwszStore))
        goto DeleteSubKeyError;
    fResult = TRUE;

CommonReturn:
    FreeSystemNameInfo(&SystemNameInfo);
    ILS_CloseRegistryKey(hKey);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(ParseSystemStoreParaError)
SET_ERROR(InvalidArg, E_INVALIDARG)
TRACE_ERROR(DeleteAllError)
TRACE_ERROR(OpenSystemRegPathKeyError)
TRACE_ERROR(DeleteSubKeyError)
}

//+-------------------------------------------------------------------------
//  Unregister the physical store from the specified system store.
//--------------------------------------------------------------------------
BOOL
WINAPI
CertUnregisterPhysicalStore(
    IN const void *pvSystemStore,
    IN DWORD dwFlags,
    IN LPCWSTR pwszStoreName
    )
{
    BOOL fResult;
    HKEY hKey = NULL;
    LPWSTR pwszSubKeyName = NULL;

    if (!IsSystemStoreLocationInRegistry(dwFlags)) {
        void *pvFuncAddr;
        HCRYPTOIDFUNCADDR hFuncAddr;

        if (!CryptGetOIDFunctionAddress(
                rghFuncSet[UNREGISTER_PHYSICAL_STORE_FUNC_SET],
                0,                      // dwEncodingType,
                GetSystemStoreLocationOID(dwFlags),
                0,                      // dwFlags
                &pvFuncAddr,
                &hFuncAddr))
            return FALSE;

        fResult = ((PFN_UNREGISTER_PHYSICAL_STORE) pvFuncAddr)(
            pvSystemStore,
            dwFlags,
            pwszStoreName
            );
        CryptFreeOIDFunctionAddress(hFuncAddr, 0);
        return fResult;
    }

    if (dwFlags & ~UNREGISTER_FLAGS_MASK)
        goto InvalidArg;

    if (NULL == (hKey = OpenPhysicalStores(
            pvSystemStore,
            dwFlags | CERT_STORE_OPEN_EXISTING_FLAG
            )))
        goto OpenPhysicalStoresError;
    if (!RecursiveDeleteSubKey(hKey, pwszStoreName))
        goto DeleteSubKeyError;
    fResult = TRUE;

CommonReturn:
    ILS_CloseRegistryKey(hKey);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(InvalidArg, E_INVALIDARG)
TRACE_ERROR(OpenPhysicalStoresError)
TRACE_ERROR(DeleteSubKeyError)
}


typedef struct _ENUM_REGISTERED_SYSTEM_STORE_LOCATION_INFO {
    DWORD                               dwLastError;
    void                                *pvArg;
    PFN_CERT_ENUM_SYSTEM_STORE_LOCATION pfnEnum;
} ENUM_REGISTERED_SYSTEM_STORE_LOCATION_INFO,
    *PENUM_REGISTERED_SYSTEM_STORE_LOCATION_INFO;

#ifdef CAPI_INCLUDE_REGISTER_OID
STATIC BOOL WINAPI EnumRegisteredSystemStoreLocationCallback(
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
    PENUM_REGISTERED_SYSTEM_STORE_LOCATION_INFO pEnumRegisteredInfo =
        (PENUM_REGISTERED_SYSTEM_STORE_LOCATION_INFO) pvArg;

    LPCWSTR pwszLocation = L"";
    DWORD dwFlags;

    if (0 != pEnumRegisteredInfo->dwLastError)
        return FALSE;

    if (CONST_OID_STR_PREFIX_CHAR != *pszOID)
        return TRUE;
    dwFlags =
        (((DWORD) atol(pszOID + 1)) << CERT_SYSTEM_STORE_LOCATION_SHIFT) &
            CERT_SYSTEM_STORE_LOCATION_MASK;
    if (0 == dwFlags)
        return TRUE;

    // Try to find the SystemStoreLocation value
    while (cValue--) {
        if (0 == _wcsicmp(rgpwszValueName[cValue],
                    CRYPT_OID_SYSTEM_STORE_LOCATION_VALUE_NAME) &&
                REG_SZ == rgdwValueType[cValue]) {
            pwszLocation = (LPCWSTR) rgpbValueData[cValue];
            break;
        }
    }

    if (!pEnumRegisteredInfo->pfnEnum(
            pwszLocation,
            dwFlags,
            NULL,                                       // pvReserved
            pEnumRegisteredInfo->pvArg
            )) {
        if (0 == (pEnumRegisteredInfo->dwLastError = GetLastError()))
            pEnumRegisteredInfo->dwLastError = (DWORD) E_UNEXPECTED;
        return FALSE;
    } else
        return TRUE;
}

#endif
//+-------------------------------------------------------------------------
//  Enumerate the system store locations.
//--------------------------------------------------------------------------
BOOL
WINAPI
CertEnumSystemStoreLocation(
    IN DWORD dwFlags,
    IN void *pvArg,
    IN PFN_CERT_ENUM_SYSTEM_STORE_LOCATION pfnEnum
    )
{
    DWORD i;
    ENUM_REGISTERED_SYSTEM_STORE_LOCATION_INFO EnumRegisteredInfo;

    if (dwFlags & ~ENUM_FLAGS_MASK) {
        SetLastError((DWORD) E_INVALIDARG);
        return FALSE;
    }

    // Enumerate through the predefined, crypt32.dll system store locations
    for (i = 0; i < ENUM_SYSTEM_STORE_LOCATION_CNT; i++) {
        if (!pfnEnum(
                rgEnumSystemStoreLocationInfo[i].pwszLocation,
                rgEnumSystemStoreLocationInfo[i].dwFlags,
                NULL,                                       // pvReserved
                pvArg
                ))
            return FALSE;
    }

    // Enumerate through the registered system store locations
    EnumRegisteredInfo.dwLastError = 0;
    EnumRegisteredInfo.pvArg = pvArg;
    EnumRegisteredInfo.pfnEnum = pfnEnum;
#ifdef CAPI_INCLUDE_REGISTER_OID
    CryptEnumOIDFunction(
            0,                              // dwEncodingType
            CRYPT_OID_ENUM_SYSTEM_STORE_FUNC,
            NULL,                           // pszOID
            0,                              // dwFlags
            (void *) &EnumRegisteredInfo,   // pvArg
            EnumRegisteredSystemStoreLocationCallback
            );
#endif //CAPI_INCLUDE_REGISTER_OID
    if (0 != EnumRegisteredInfo.dwLastError) {
        SetLastError(EnumRegisteredInfo.dwLastError);
        return FALSE;
    } else
        return TRUE;
}

STATIC BOOL EnumServicesOrUsersSystemStore(
    IN OUT PSYSTEM_NAME_INFO pLocationNameInfo,
    IN DWORD dwFlags,
    IN void *pvArg,
    IN PFN_CERT_ENUM_SYSTEM_STORE pfnEnum
    )
{
    BOOL fResult;
    HKEY hKey = NULL;
    DWORD cSubKeys;
    DWORD cchMaxSubKey;
    LPWSTR pwszEnumServiceName = NULL;
    void *pvEnumServicePara = NULL;
    BOOL fDidEnum;

    assert(NULL == pLocationNameInfo->rgpwszName[SERVICE_NAME_INDEX]);

    // Opens ..\Cryptography\Services SubKey or HKEY_USERS SubKey
    if (NULL == (hKey = OpenSystemRegPathKey(
            pLocationNameInfo,
            NULL,               // pwszSubKeyName
            dwFlags | CERT_STORE_OPEN_EXISTING_FLAG | CERT_STORE_READONLY_FLAG
            )))
        goto OpenSystemRegPathKeyError;

    if (!GetSubKeyInfo(
            hKey,
            &cSubKeys,
            &cchMaxSubKey
            ))
        goto GetSubKeyInfoError;

    // Enumerates the ServiceOrUserNames
    fDidEnum = FALSE;
    if (cSubKeys && cchMaxSubKey) {
        DWORD i;

        LPCWSTR rgpwszEnumName[2];
        SYSTEM_NAME_GROUP EnumNameGroup;
        EnumNameGroup.cName = 2;
        EnumNameGroup.rgpwszName = rgpwszEnumName;

        cchMaxSubKey++;
        if (NULL == (pwszEnumServiceName = (LPWSTR) PkiNonzeroAlloc(
                cchMaxSubKey * sizeof(WCHAR))))
            goto OutOfMemory;

        rgpwszEnumName[0] = pLocationNameInfo->rgpwszName[COMPUTER_NAME_INDEX];
        rgpwszEnumName[1] = pwszEnumServiceName;

        for (i = 0; i < cSubKeys; i++) {
            DWORD cchEnumServiceName = cchMaxSubKey;
            LONG err;
            if (ERROR_SUCCESS != (err = RegEnumKeyExU(
                    hKey,
                    i,
                    pwszEnumServiceName,
                    &cchEnumServiceName,
                    NULL,               // lpdwReserved
                    NULL,               // lpszClass
                    NULL,               // lpcchClass
                    NULL                // lpftLastWriteTime
                    )) || 0 == cchEnumServiceName ||
                            L'\0' == *pwszEnumServiceName)
                continue;
            if (NULL == (pvEnumServicePara = FormatSystemNamePara(
                        1, &EnumNameGroup, pLocationNameInfo)))
                goto FormatSystemNameParaError;

            if (!CertEnumSystemStore(
                    dwFlags,
                    pvEnumServicePara,
                    pvArg,
                    pfnEnum
                    )) {
                if (ERROR_FILE_NOT_FOUND != GetLastError())
                    goto EnumSystemStoreError;
            } else
                fDidEnum = TRUE;
            FreeSystemNamePara(pvEnumServicePara, pLocationNameInfo);
            pvEnumServicePara = NULL;
        }
    }

    if (!fDidEnum)
        goto NoSystemStores;
    fResult = TRUE;
CommonReturn:
    ILS_CloseRegistryKey(hKey);
    PkiFree(pwszEnumServiceName);
    FreeSystemNamePara(pvEnumServicePara, pLocationNameInfo);
    FreeSystemNameInfo(pLocationNameInfo);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(OpenSystemRegPathKeyError)
TRACE_ERROR(GetSubKeyInfoError)
TRACE_ERROR(OutOfMemory)
TRACE_ERROR(FormatSystemNameParaError)
TRACE_ERROR(EnumSystemStoreError)
SET_ERROR(NoSystemStores, ERROR_FILE_NOT_FOUND)
}

//+-------------------------------------------------------------------------
//  Enumerate the system stores.
//
//  The upper word of the dwFlags parameter is used to specify the location of
//  the system store.
//
//  All registry based system store locations have the predefined stores
//  of: My, Root, Trust and CA.
//--------------------------------------------------------------------------
BOOL
WINAPI
CertEnumSystemStore(
    IN DWORD dwFlags,
    IN OPTIONAL void *pvSystemStoreLocationPara,
    IN void *pvArg,
    IN PFN_CERT_ENUM_SYSTEM_STORE pfnEnum
    )
{
    BOOL fResult;
    HKEY hKey = NULL;
    DWORD cSubKeys;
    DWORD cchMaxSubKey;
    SYSTEM_NAME_INFO LocationNameInfo;
    LPWSTR pwszEnumSystemStore = NULL;
    void *pvEnumSystemPara = NULL;

    DWORD i;
    DWORD dwCheckFlag;
    DWORD dwLocID;
    DWORD dwPredefinedSystemFlags;

    CERT_SYSTEM_STORE_INFO NullSystemStoreInfo;
    LPCWSTR rgpwszEnumName[3];
    SYSTEM_NAME_GROUP EnumNameGroup;

    if (!IsSystemStoreLocationInRegistry(dwFlags)) {
        void *pvFuncAddr;
        HCRYPTOIDFUNCADDR hFuncAddr;

        if (!CryptGetOIDFunctionAddress(
                rghFuncSet[ENUM_SYSTEM_STORE_FUNC_SET],
                0,                      // dwEncodingType,
                GetSystemStoreLocationOID(dwFlags),
                0,                      // dwFlags
                &pvFuncAddr,
                &hFuncAddr))
            return FALSE;

        fResult = ((PFN_ENUM_SYSTEM_STORE) pvFuncAddr)(
            dwFlags,
            pvSystemStoreLocationPara,
            pvArg,
            pfnEnum
            );
        CryptFreeOIDFunctionAddress(hFuncAddr, 0);
        return fResult;
    }

    if (!ParseSystemStorePara(
            pvSystemStoreLocationPara,
            dwFlags,
            0,                  // cReqName, none for enumeration
            &LocationNameInfo   // zero'ed for error
            ))
        goto ParseSystemStoreParaError;

    if (dwFlags & ~ENUM_FLAGS_MASK)
        goto InvalidArg;

    dwLocID = GetSystemStoreLocationID(dwFlags);
    if ((CERT_SYSTEM_STORE_SERVICES_ID == dwLocID ||
             CERT_SYSTEM_STORE_USERS_ID == dwLocID)
                                &&
            NULL == LocationNameInfo.rgpwszName[SERVICE_NAME_INDEX])
        // Following frees rgpwszLocationName entries
        return EnumServicesOrUsersSystemStore(
            &LocationNameInfo,
            dwFlags,
            pvArg,
            pfnEnum
            );

    // Opens SystemCertificates subkey
    if (NULL == (hKey = OpenSystemRegPathKey(
            &LocationNameInfo,
            NULL,               // pwszSubKeyName
            dwFlags | CERT_STORE_OPEN_EXISTING_FLAG | CERT_STORE_READONLY_FLAG
            ))) {
        if (ERROR_FILE_NOT_FOUND != GetLastError())
            goto OpenSystemRegPathKeyError;

        // Note, a registry entry isn't needed for the predefined stores
        cSubKeys = 0;
    } else if (!GetSubKeyInfo(
            hKey,
            &cSubKeys,
            &cchMaxSubKey
            ))
        goto GetSubKeyInfoError;

    memset(&NullSystemStoreInfo, 0, sizeof(NullSystemStoreInfo));
    NullSystemStoreInfo.cbSize = sizeof(NullSystemStoreInfo);
    EnumNameGroup.cName = 3;
    EnumNameGroup.rgpwszName = rgpwszEnumName;
    rgpwszEnumName[0] = LocationNameInfo.rgpwszName[COMPUTER_NAME_INDEX];
    rgpwszEnumName[1] = LocationNameInfo.rgpwszName[SERVICE_NAME_INDEX];

    // Enumerate the predefined system stores.
    assert(NUM_SYSTEM_STORE_LOCATION > dwLocID);
    dwPredefinedSystemFlags =
        rgSystemStoreLocationInfo[dwLocID].dwPredefinedSystemFlags;
    for (i = 0, dwCheckFlag = 1; i < NUM_PREDEFINED_SYSTEM_STORE;
                                        i++, dwCheckFlag = dwCheckFlag << 1) {
        if (0 == (dwCheckFlag & dwPredefinedSystemFlags))
            continue;
        rgpwszEnumName[2] = rgpwszPredefinedSystemStore[i];
        if (NULL == (pvEnumSystemPara = FormatSystemNamePara(
                1, &EnumNameGroup, &LocationNameInfo)))
            goto FormatSystemNameParaError;
        if (!pfnEnum(
                pvEnumSystemPara,
                dwFlags & CERT_SYSTEM_STORE_MASK,
                &NullSystemStoreInfo,
                NULL,               // pvReserved
                pvArg
                ))
            goto EnumCallbackError;
        FreeSystemNamePara(pvEnumSystemPara, &LocationNameInfo);
        pvEnumSystemPara = NULL;
    }

    // Enumerate the registered systems stores. Skip past any of the above
    // predefined stores
    if (cSubKeys && cchMaxSubKey) {
        cchMaxSubKey++;
        if (NULL == (pwszEnumSystemStore = (LPWSTR) PkiNonzeroAlloc(
                cchMaxSubKey * sizeof(WCHAR))))
            goto OutOfMemory;

        rgpwszEnumName[2] = pwszEnumSystemStore;

        for (i = 0; i < cSubKeys; i++) {
            DWORD cchEnumSystemStore = cchMaxSubKey;
            if (ERROR_SUCCESS != RegEnumKeyExU(
                    hKey,
                    i,
                    pwszEnumSystemStore,
                    &cchEnumSystemStore,
                    NULL,               // lpdwReserved
                    NULL,               // lpszClass
                    NULL,               // lpcchClass
                    NULL                // lpftLastWriteTime
                    ) || 0 == cchEnumSystemStore)
                continue;
            if (IsPredefinedSystemStore(pwszEnumSystemStore, dwFlags))
                // Already enumerated above
                continue;
            if (NULL == (pvEnumSystemPara = FormatSystemNamePara(
                        1, &EnumNameGroup, &LocationNameInfo)))
                goto FormatSystemNameParaError;

            if (!pfnEnum(
                    pvEnumSystemPara,
                    dwFlags & CERT_SYSTEM_STORE_MASK,
                    &NullSystemStoreInfo,
                    NULL,               // pvReserved
                    pvArg
                    ))
                goto EnumCallbackError;
            FreeSystemNamePara(pvEnumSystemPara, &LocationNameInfo);
            pvEnumSystemPara = NULL;
        }
    }

    fResult = TRUE;
CommonReturn:
    ILS_CloseRegistryKey(hKey);
    PkiFree(pwszEnumSystemStore);
    FreeSystemNamePara(pvEnumSystemPara, &LocationNameInfo);
    FreeSystemNameInfo(&LocationNameInfo);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(ParseSystemStoreParaError)
SET_ERROR(InvalidArg, E_INVALIDARG)
TRACE_ERROR(OpenSystemRegPathKeyError)
TRACE_ERROR(GetSubKeyInfoError)
TRACE_ERROR(OutOfMemory)
TRACE_ERROR(FormatSystemNameParaError)
TRACE_ERROR(EnumCallbackError)
}

typedef struct _ENUM_PHYSICAL_STORE_INFO ENUM_PHYSICAL_STORE_INFO,
    *PENUM_PHYSICAL_STORE_INFO;
struct _ENUM_PHYSICAL_STORE_INFO {
    CERT_PHYSICAL_STORE_INFO    RegistryInfo;
    LPWSTR                      pwszStoreName;
    PENUM_PHYSICAL_STORE_INFO   pNext;
};

STATIC void FreeEnumPhysicalStoreInfo(
    IN PENUM_PHYSICAL_STORE_INFO pStoreInfo
    )
{
    PCERT_PHYSICAL_STORE_INFO pRegistryInfo = &pStoreInfo->RegistryInfo;
    PkiFree(pRegistryInfo->OpenParameters.pbData);
    PkiFree(pRegistryInfo->pszOpenStoreProvider);
    PkiFree(pStoreInfo->pwszStoreName);
    PkiFree(pStoreInfo);
}


STATIC PENUM_PHYSICAL_STORE_INFO GetEnumPhysicalStoreInfo(
    IN HKEY hKey,
    IN LPCWSTR pwszStoreName
    )
{
    LONG err;
    HKEY hSubKey = NULL;
    PENUM_PHYSICAL_STORE_INFO pStoreInfo;
    PCERT_PHYSICAL_STORE_INFO pRegistryInfo;        // not allocated

    if (NULL == (pStoreInfo = (PENUM_PHYSICAL_STORE_INFO) PkiZeroAlloc(
            sizeof(ENUM_PHYSICAL_STORE_INFO))))
        return NULL;
    pRegistryInfo = &pStoreInfo->RegistryInfo;
    pRegistryInfo->cbSize = sizeof(*pRegistryInfo);

    if (NULL == (pStoreInfo->pwszStoreName =
            AllocAndCopyString(pwszStoreName)))
        goto OutOfMemory;

    if (ERROR_SUCCESS != (err = RegOpenKeyExU(
            hKey,
            pwszStoreName,
            0,                      // dwReserved
            KEY_READ,
            &hSubKey)))
        goto RegOpenKeyError;

    if (!ILS_ReadBINARYValueFromRegistry(
            hSubKey,
            L"OpenParameters",
            &pRegistryInfo->OpenParameters.pbData,
            &pRegistryInfo->OpenParameters.cbData
            )) {
        LPWSTR pwszParameters;
        if (pwszParameters = ReadSZValueFromRegistry(
                hSubKey,
                L"OpenParameters"
                )) {
            pRegistryInfo->OpenParameters.pbData = (BYTE *) pwszParameters;
            pRegistryInfo->OpenParameters.cbData =
                (wcslen(pwszParameters) + 1) * sizeof(WCHAR);
        } else {
            // Default to empty string
            if (NULL == (pRegistryInfo->OpenParameters.pbData =
                    (BYTE *) AllocAndCopyString(L"")))
                goto OutOfMemory;
            pRegistryInfo->OpenParameters.cbData = 0;
        }
    }

#ifdef CE_BUILD
	// doesnt make sense why pszOpenStoreProvider is LPSTR and not LPWSTR
	{
	LPWSTR pwsz;
	CHAR szTemp[MAX_PATH];
	size_t len;
	pRegistryInfo->pszOpenStoreProvider = NULL;       
	if (NULL == (pwsz = ReadSZValueFromRegistry(
            hSubKey,
            L"OpenStoreProvider"
            )))
         
        goto NoOpenStoreProviderError;
	len = wcstombs(szTemp,pwsz,sizeof(szTemp));
	memcpy(pwsz,szTemp,len+1);
	pRegistryInfo->pszOpenStoreProvider = (LPSTR)pwsz;
	}
    
#else
	if (NULL == (pRegistryInfo->pszOpenStoreProvider = ReadSZValueFromRegistry(
            hSubKey,
            "OpenStoreProvider"
            )))
        goto NoOpenStoreProviderError;

#endif        

    ILS_ReadDWORDValueFromRegistry(
        hSubKey,
        L"OpenFlags",
        &pRegistryInfo->dwOpenFlags
        );

    ILS_ReadDWORDValueFromRegistry(
        hSubKey,
        L"OpenEncodingType",
        &pRegistryInfo->dwOpenEncodingType
        );

    ILS_ReadDWORDValueFromRegistry(
        hSubKey,
        L"Flags",
        &pRegistryInfo->dwFlags
        );

    ILS_ReadDWORDValueFromRegistry(
        hSubKey,
        L"Priority",
        &pRegistryInfo->dwPriority
        );

CommonReturn:
    ILS_CloseRegistryKey(hSubKey);
    return pStoreInfo;
ErrorReturn:
    FreeEnumPhysicalStoreInfo(pStoreInfo);
    pStoreInfo = NULL;
    goto CommonReturn;

TRACE_ERROR(OutOfMemory)
SET_ERROR_VAR(RegOpenKeyError, err)
TRACE_ERROR(NoOpenStoreProviderError)
}


STATIC BOOL IsSelfPhysicalStoreInfo(
    IN PSYSTEM_NAME_INFO pSystemNameInfo,
    IN DWORD dwFlags,
    IN PCERT_PHYSICAL_STORE_INFO pStoreInfo,
    OUT DWORD *pdwSystemProviderFlags
    )
{
    BOOL fResult;
    DWORD dwSystemProviderFlags;
    LPWSTR pwszStoreName = (LPWSTR) pStoreInfo->OpenParameters.pbData;
    SYSTEM_NAME_INFO StoreNameInfo;

    LPWSTR pwszCurrentServiceName = NULL;
    LPWSTR pwszCurrentComputerName = NULL;

    DWORD dwSystemLocation;
    DWORD dwInfoLocation;
    BOOL fSameLocation;

    *pdwSystemProviderFlags = 0;

    dwSystemLocation = dwFlags & CERT_SYSTEM_STORE_LOCATION_MASK;
    // Note, if the RELOCATE_FLAG is incorrectly set in the dwOpenFlags
    // then, never match
    dwInfoLocation = pStoreInfo->dwOpenFlags &
        (CERT_SYSTEM_STORE_LOCATION_MASK | CERT_SYSTEM_STORE_RELOCATE_FLAG);


    // Check if in same system store location
    fSameLocation = (dwSystemLocation == dwInfoLocation);
    if (!fSameLocation) {
        if (CERT_SYSTEM_STORE_CURRENT_SERVICE == dwInfoLocation)
            dwInfoLocation = CERT_SYSTEM_STORE_SERVICES;
        if (CERT_SYSTEM_STORE_CURRENT_SERVICE == dwSystemLocation)
            dwSystemLocation = CERT_SYSTEM_STORE_SERVICES;
        if (CERT_SYSTEM_STORE_CURRENT_USER == dwInfoLocation)
            dwInfoLocation = CERT_SYSTEM_STORE_USERS;
        if (CERT_SYSTEM_STORE_CURRENT_USER == dwSystemLocation)
            dwSystemLocation = CERT_SYSTEM_STORE_USERS;

        if (dwSystemLocation != dwInfoLocation)
            return FALSE;
    }

    // Check if SYSTEM or SYSTEM_REGISTRY store.
    dwSystemProviderFlags = GetSystemProviderFlags(
        pStoreInfo->pszOpenStoreProvider);
    if (0 == dwSystemProviderFlags ||
            (dwSystemProviderFlags & PHYSICAL_PROVIDER_FLAG))
        return FALSE;

    if (dwSystemProviderFlags & ASCII_SYSTEM_PROVIDER_FLAG) {
        if (NULL == (pwszStoreName = MkWStr((LPSTR) pwszStoreName)))
            return FALSE;
    }

    if (!ParseSystemStorePara(
            pwszStoreName,
            pStoreInfo->dwOpenFlags,
            1,                  // cReq, 1 for OpenSystemStore
            &StoreNameInfo      // zero'ed for error
            ))
        goto ParseSystemStoreParaError;

    // Default to not self
    fResult = FALSE;

    if (StoreNameInfo.rgpwszName[COMPUTER_NAME_INDEX]) {
        if (NULL == pSystemNameInfo->rgpwszName[COMPUTER_NAME_INDEX]) {
            LPCWSTR pwszStoreComputerName;

            if (NULL == (pwszCurrentComputerName = GetCurrentComputerName()))
                goto GetCurrentComputerNameError;

            pwszStoreComputerName =
                StoreNameInfo.rgpwszName[COMPUTER_NAME_INDEX];
            assert(L'\\' == pwszStoreComputerName[0] &&
                L'\\' == pwszStoreComputerName[1]);
            if (!('\\' == pwszCurrentComputerName[0] &&
                    L'\\' == pwszCurrentComputerName[1]))
                pwszStoreComputerName += 2;
            if (0 != _wcsicmp(pwszStoreComputerName, pwszCurrentComputerName))
                goto CommonReturn;
        } else if (0 != _wcsicmp(StoreNameInfo.rgpwszName[COMPUTER_NAME_INDEX],
                pSystemNameInfo->rgpwszName[COMPUTER_NAME_INDEX]))
            goto CommonReturn;
    }
    // else
    //  Opening using none or the same computer name

    if (StoreNameInfo.rgpwszName[SERVICE_NAME_INDEX]) {
        if (NULL == pSystemNameInfo->rgpwszName[SERVICE_NAME_INDEX]) {
            if (NULL == (pwszCurrentServiceName =
                    GetCurrentServiceOrUserName()))
                goto GetCurrentServiceOrUserNameError;
            if (0 != _wcsicmp(StoreNameInfo.rgpwszName[SERVICE_NAME_INDEX],
                    pwszCurrentServiceName))
                goto CommonReturn;
        } else if (0 != _wcsicmp(StoreNameInfo.rgpwszName[SERVICE_NAME_INDEX],
                pSystemNameInfo->rgpwszName[SERVICE_NAME_INDEX]))
            goto CommonReturn;
    }
    // else
    //  Opening using none or the same service/user name

    assert(StoreNameInfo.rgpwszName[SYSTEM_NAME_INDEX] &&
         pSystemNameInfo->rgpwszName[SYSTEM_NAME_INDEX]);
    if (0 != _wcsicmp(StoreNameInfo.rgpwszName[SYSTEM_NAME_INDEX],
            pSystemNameInfo->rgpwszName[SYSTEM_NAME_INDEX]))
        goto CommonReturn;

    // We have a match !!!
    fResult = TRUE;
    *pdwSystemProviderFlags = dwSystemProviderFlags;

CommonReturn:
    if (dwSystemProviderFlags & ASCII_SYSTEM_PROVIDER_FLAG)
        FreeWStr(pwszStoreName);
    FreeSystemNameInfo(&StoreNameInfo);
    PkiFree(pwszCurrentServiceName);
    PkiFree(pwszCurrentComputerName);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
TRACE_ERROR(GetCurrentComputerNameError)
TRACE_ERROR(GetCurrentServiceOrUserNameError)
TRACE_ERROR(ParseSystemStoreParaError)
}

// List is sorted according to physical store priority
STATIC void AddToEnumPhysicalStoreList(
    IN PENUM_PHYSICAL_STORE_INFO *ppStoreInfoHead,
    IN PENUM_PHYSICAL_STORE_INFO pAddInfo
    )
{

    if (NULL == *ppStoreInfoHead)
        *ppStoreInfoHead = pAddInfo;
    else {
        PENUM_PHYSICAL_STORE_INFO pListInfo;
        DWORD dwPriority = pAddInfo->RegistryInfo.dwPriority;

        pListInfo = *ppStoreInfoHead;
        if (dwPriority > pListInfo->RegistryInfo.dwPriority) {
            // Insert at beginning before first entry
            pAddInfo->pNext = pListInfo;
            *ppStoreInfoHead = pAddInfo;
        } else {
            // Insert after the entry whose next entry has
            // lower priority or insert after the last entry
            while (pListInfo->pNext &&
                    dwPriority <= pListInfo->pNext->RegistryInfo.dwPriority)
                pListInfo = pListInfo->pNext;

            pAddInfo->pNext = pListInfo->pNext;
            pListInfo->pNext = pAddInfo;
        }
    }
}


STATIC void FreeEnumPhysicalStoreList(
    IN PENUM_PHYSICAL_STORE_INFO pStoreInfoHead
    )
{
    while (pStoreInfoHead) {
        PENUM_PHYSICAL_STORE_INFO pStoreInfo = pStoreInfoHead;
        pStoreInfoHead = pStoreInfo->pNext;
        FreeEnumPhysicalStoreInfo(pStoreInfo);
    }

}

#ifndef CE_BUILD
// Returns NULL if unable to successfully get the Url. Returned string
// must be freed by calling CryptMemFree
STATIC LPWSTR GetUserDsUserCertificateUrl()
{
    DWORD dwErr;
    LPWSTR pwszUrl = NULL;
    HMODULE hDll = NULL;
    PFN_GET_USER_DS_STORE_URL pfnGetUserDsStoreUrl;

    if (NULL == (hDll = LoadLibraryA(sz_CRYPTNET_DLL)))
        goto LoadCryptNetDllError;

    if (NULL == (pfnGetUserDsStoreUrl =
            (PFN_GET_USER_DS_STORE_URL) GetProcAddress(hDll,
                sz_GetUserDsStoreUrl)))
        goto GetUserDsStoreUrlProcAddressError;

    if (!pfnGetUserDsStoreUrl(wsz_USER_CERTIFICATE_ATTR, &pwszUrl)) {
        dwErr = GetLastError();
        goto GetUserDsStoreUrlError;
    }

CommonReturn:
    if (hDll)
        FreeLibrary(hDll);
    return pwszUrl;
ErrorReturn:
    pwszUrl = NULL;
    goto CommonReturn;
TRACE_ERROR(LoadCryptNetDllError)
TRACE_ERROR(GetUserDsStoreUrlProcAddressError)
SET_ERROR_VAR(GetUserDsStoreUrlError, dwErr)
}
#else
#define GetUserDsUserCertificateUrl() (NULL)
#endif

//+-------------------------------------------------------------------------
//  Unless, CERT_STORE_OPEN_EXISTING_FLAG or CERT_STORE_READONLY_FLAG is
//  set, the pvSystemStore will be created if it doesn't already exist.
//
//  Note, depending on the store location and possibly the store name, there
//  are predefined physical stores of .Default, .LocalMachine, .GroupPolicy,
//  .Enterprise
//--------------------------------------------------------------------------
#ifdef _ARM_
#pragma optimize("", off)
#endif

STATIC BOOL EnumPhysicalStore(
    IN const void *pvSystemStore,
    IN DWORD dwFlags,
    IN void *pvArg,
    IN PFN_CERT_ENUM_PHYSICAL_STORE pfnEnum
    )
{
    BOOL fResult;
    LONG *plDepth = NULL;    // allocated per thread, don't free here
    HKEY hKey = NULL;
    DWORD cSubKeys;
    DWORD cchMaxSubKey;
    LPWSTR pwszStoreName = NULL;
    PENUM_PHYSICAL_STORE_INFO pStoreInfoHead = NULL;
    PENUM_PHYSICAL_STORE_INFO pStoreInfo;       // not allocated
    SYSTEM_NAME_INFO SystemNameInfo;

    DWORD dwStoreLocationID;
    DWORD dwPredefinedPhysicalFlags;

    if (!IsSystemStoreLocationInRegistry(dwFlags)) {
        void *pvFuncAddr;
        HCRYPTOIDFUNCADDR hFuncAddr;

        if (!CryptGetOIDFunctionAddress(
                rghFuncSet[ENUM_PHYSICAL_STORE_FUNC_SET],
                0,                      // dwEncodingType,
                GetSystemStoreLocationOID(dwFlags),
                0,                      // dwFlags
                &pvFuncAddr,
                &hFuncAddr))
            return FALSE;

        fResult = ((PFN_ENUM_PHYSICAL_STORE) pvFuncAddr)(
            pvSystemStore,
            dwFlags,
            pvArg,
            pfnEnum
            );
        CryptFreeOIDFunctionAddress(hFuncAddr, 0);
        return fResult;
    }

    if (!ParseSystemStorePara(
            pvSystemStore,
            dwFlags,
            1,                      // cReqName
            &SystemNameInfo))       // zero'ed on error
        goto ParseSystemStoreParaError;

    if (dwFlags & ~ENUM_FLAGS_MASK)
        goto InvalidArg;

    // Check for cross store recursion by checking the thread's enum
    // depth
    if (NULL == (plDepth = (LONG *) I_CryptGetTls(
            hTlsEnumPhysicalStoreDepth))) {
        if (NULL == (plDepth = (LONG *) PkiNonzeroAlloc(sizeof(*plDepth))))
            goto OutOfMemory;
        *plDepth = 1;
        I_CryptSetTls(hTlsEnumPhysicalStoreDepth, plDepth);
    } else {
        *plDepth += 1;
        if (MAX_ENUM_PHYSICAL_STORE_DEPTH < *plDepth)
            goto ExceededEnumPhysicalStoreDepth_PossibleCrossStoreRecursion;
    }

    if (IsClientGptStore(&SystemNameInfo, dwFlags)) {
        cSubKeys = 0;
    } else if (NULL == (hKey = OpenSystemRegPathKey(
            &SystemNameInfo,
            PHYSICAL_STORES_SUBKEY_NAME,
            dwFlags | CERT_STORE_OPEN_EXISTING_FLAG | CERT_STORE_READONLY_FLAG
            ))) {
        if (ERROR_FILE_NOT_FOUND != GetLastError())
            goto OpenPhysicalStoresError;

        // Check if we have a system store without the "PhysicalStores" subkey
        if (NULL == (hKey = OpenSystemRegPathKey(
                &SystemNameInfo,
                NULL,               // pwszSubKeyName
                dwFlags
                ))) {
            if (dwFlags & CERT_STORE_MAXIMUM_ALLOWED_FLAG)
                hKey = OpenSystemRegPathKey(
                    &SystemNameInfo,
                    NULL,               // pwszSubKeyName
                    dwFlags | CERT_STORE_OPEN_EXISTING_FLAG |
                        CERT_STORE_READONLY_FLAG
                    );
        }

        if (NULL == hKey) {
            // Note, the predefined stores don't need to exist in the
            // registry
            if (ERROR_FILE_NOT_FOUND != GetLastError())
                goto OpenSystemStoreError;
        } else {
            RegCloseKey(hKey);
            hKey = NULL;
        }

        cSubKeys = 0;
    } else if (!GetSubKeyInfo(
            hKey,
            &cSubKeys,
            &cchMaxSubKey
            ))
        goto GetSubKeyInfoError;

    // Get flags containing list of predefined physical stores according to
    // store name and/or store location
    dwStoreLocationID = GetSystemStoreLocationID(dwFlags);
    if (0 == _wcsicmp(SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX],
            wsz_MY_STORE))
        // Only .Default is predefined for "My" store
        dwPredefinedPhysicalFlags = MY_PHYSICAL_FLAGS;
    else if (0 == _wcsicmp(SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX],
             wsz_ROOT_STORE)) {
        if (CERT_SYSTEM_STORE_CURRENT_USER_ID == dwStoreLocationID) {
            if (IPR_OnlyLocalMachineGroupPolicyRootsAllowed()) {
                // Only .LocalMachineGroupPolicy. Don't read CurrentUser's
                // SystemRegistry or LocalMachine SystemRegistry.
                dwPredefinedPhysicalFlags = LMGP_PHYSICAL_FLAG;
                // Since we won't be reading the SystemRegistry, ensure
                // the protected list of roots is initialized.
                IPR_InitProtectedRootInfo();
            } else if (IPR_IsCurrentUserRootsAllowed()) {
                // .Default and .LocalMachine physical stores are predefined
                dwPredefinedPhysicalFlags = CURRENT_USER_ROOT_PHYSICAL_FLAGS;
            } else {
                // Don't read the CurrentUser's SystemRegistry
                dwPredefinedPhysicalFlags = CURRENT_USER_ROOT_PHYSICAL_FLAGS &
                    ~DEFAULT_PHYSICAL_FLAG;
                // Since we won't be reading the SystemRegistry, ensure
                // the protected list of roots is initialized.
                IPR_InitProtectedRootInfo();
            }
            // Only the predefined physical stores are allowed for Root
            cSubKeys = 0;
        } else if (CERT_SYSTEM_STORE_USERS_ID == dwStoreLocationID) {
            // Only .LocalMachine physical stores is predefined

            dwPredefinedPhysicalFlags = USERS_ROOT_PHYSICAL_FLAGS;
            // Only the predefined physical stores are allowed for Root
            cSubKeys = 0;
        } else {
            // According to store location.
            dwPredefinedPhysicalFlags =
                rgSystemStoreLocationInfo[
                    dwStoreLocationID].dwPredefinedPhysicalFlags;
        }
    } else if (0 == _wcsicmp(SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX],
             wsz_USER_DS_STORE)) {
        // Only .UserCertificate is predefined for "UserDS"
        dwPredefinedPhysicalFlags = USER_DS_PHYSICAL_FLAGS;
    } else
        // According to store location
        dwPredefinedPhysicalFlags =
            rgSystemStoreLocationInfo[
                dwStoreLocationID].dwPredefinedPhysicalFlags;


    if (cSubKeys && cchMaxSubKey) {
        DWORD i;

        cchMaxSubKey++;
        if (NULL == (pwszStoreName = (LPWSTR) PkiNonzeroAlloc(
                cchMaxSubKey * sizeof(WCHAR))))
            goto OutOfMemory;

        for (i = 0; i < cSubKeys; i++) {
            DWORD cchStoreName = cchMaxSubKey;

            if (ERROR_SUCCESS != RegEnumKeyExU(
                    hKey,
                    i,
                    pwszStoreName,
                    &cchStoreName,
                    NULL,               // lpdwReserved
                    NULL,               // lpszClass
                    NULL,               // lpcchClass
                    NULL                // lpftLastWriteTime
                    ) || 0 == cchStoreName)
                continue;

            if (NULL == (pStoreInfo = GetEnumPhysicalStoreInfo(
                    hKey,
                    pwszStoreName
                    )))
                continue;
            AddToEnumPhysicalStoreList(&pStoreInfoHead, pStoreInfo);
        }
    }

    for (pStoreInfo = pStoreInfoHead; pStoreInfo;
                                            pStoreInfo = pStoreInfo->pNext) {
        PCERT_PHYSICAL_STORE_INFO pRegistryInfo = &pStoreInfo->RegistryInfo;
        BOOL fSelfPhysicalStoreInfo;
        DWORD dwSystemProviderFlags;
        char szOID[34];

        if (IsSelfPhysicalStoreInfo(
                &SystemNameInfo,
                dwFlags,
                pRegistryInfo,
                &dwSystemProviderFlags)) {
            assert((dwSystemProviderFlags & UNICODE_SYSTEM_PROVIDER_FLAG) ||
                (dwSystemProviderFlags & ASCII_SYSTEM_PROVIDER_FLAG));
            // Force to use SYSTEM_REGISTRY provider to inhibit recursion.
            PkiFree(pRegistryInfo->pszOpenStoreProvider);
            if (dwSystemProviderFlags & ASCII_SYSTEM_PROVIDER_FLAG) {
                // Convert to "#<number>" string
                szOID[0] = CONST_OID_STR_PREFIX_CHAR;
                _ltoa((long) ((DWORD_PTR)CERT_STORE_PROV_SYSTEM_REGISTRY_A), szOID + 1, 10);
                pRegistryInfo->pszOpenStoreProvider = szOID;
            } else
                pRegistryInfo->pszOpenStoreProvider =
                    sz_CERT_STORE_PROV_SYSTEM_REGISTRY_W;

            dwPredefinedPhysicalFlags &= ~DEFAULT_PHYSICAL_FLAG;
            fSelfPhysicalStoreInfo = TRUE;
        } else {
            if (0 != dwPredefinedPhysicalFlags) {
                // Check if matches one of the predefined physical stores

                DWORD i;
                DWORD dwCheckFlag;
                for (i = 0, dwCheckFlag = 1; i < NUM_PREDEFINED_PHYSICAL;
                                        i++, dwCheckFlag = dwCheckFlag << 1) {
                    if ((dwCheckFlag & dwPredefinedPhysicalFlags) &&
                            0 == _wcsicmp(pStoreInfo->pwszStoreName,
                                rgpwszPredefinedPhysical[i])) {
                        dwPredefinedPhysicalFlags &= ~dwCheckFlag;
                        break;
                    }
                }
            }
            fSelfPhysicalStoreInfo = FALSE;
        }

        if (dwFlags & CERT_STORE_MAXIMUM_ALLOWED_FLAG) {
            pRegistryInfo->dwOpenFlags |= CERT_STORE_MAXIMUM_ALLOWED_FLAG;
            pRegistryInfo->dwOpenFlags &= ~CERT_STORE_READONLY_FLAG;
        }
        fResult = pfnEnum(
            pvSystemStore,
            dwFlags & CERT_SYSTEM_STORE_MASK,
            pStoreInfo->pwszStoreName,
            &pStoreInfo->RegistryInfo,
            NULL,                           // pvReserved
            pvArg
            );

        if (fSelfPhysicalStoreInfo) {
            // Not allocated. Set to NULL to inhibit subsequent free.
            pRegistryInfo->pszOpenStoreProvider = NULL;
        }

        if (!fResult)
            goto EnumCallbackError;
    }


    if (0 != dwPredefinedPhysicalFlags) {
        CERT_PHYSICAL_STORE_INFO SelfInfo;
        LPWSTR pwszLocalStore;
        DWORD cbLocalStore;
        DWORD i;
        DWORD dwCheckFlag;

        if (SystemNameInfo.rgpwszName[COMPUTER_NAME_INDEX]) {
            // Format local store name without the ComputerName
            LPCWSTR rgpwszGroupName[2];
            SYSTEM_NAME_GROUP NameGroup;
            NameGroup.cName = 2;
            NameGroup.rgpwszName = rgpwszGroupName;

            assert(IsRemotableSystemStoreLocationInRegistry(dwFlags));
            rgpwszGroupName[0] = SystemNameInfo.rgpwszName[SERVICE_NAME_INDEX];
            rgpwszGroupName[1] = SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX];
            if (NULL == (pwszLocalStore = FormatSystemNamePath(1, &NameGroup)))
                goto FormatSystemNamePathError;
        } else {
            if (dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG) {
                PCERT_SYSTEM_STORE_RELOCATE_PARA pRelocatePara =
                    (PCERT_SYSTEM_STORE_RELOCATE_PARA) pvSystemStore;
                pwszLocalStore = (LPWSTR) pRelocatePara->pwszSystemStore;
            } else
                pwszLocalStore = (LPWSTR) pvSystemStore;
        }
        cbLocalStore = (wcslen(pwszLocalStore) + 1) * sizeof(WCHAR);

        memset(&SelfInfo, 0, sizeof(SelfInfo));
        SelfInfo.cbSize = sizeof(SelfInfo);

        fResult = TRUE;
        for (i = 0, dwCheckFlag = 1; i < NUM_PREDEFINED_PHYSICAL;
                                        i++, dwCheckFlag = dwCheckFlag << 1) {
            LPWSTR pwszUserDsUserCertificateUrl;
            if (0 == (dwCheckFlag & dwPredefinedPhysicalFlags))
                continue;

            SelfInfo.pszOpenStoreProvider = sz_CERT_STORE_PROV_SYSTEM_W;
            SelfInfo.OpenParameters.pbData =
                (BYTE *) SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX];
            SelfInfo.OpenParameters.cbData =
                (wcslen(SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX]) + 1) *
                    sizeof(WCHAR);
            SelfInfo.dwFlags = 0;
            pwszUserDsUserCertificateUrl = NULL;
            switch (i) {
                case DEFAULT_PHYSICAL_INDEX:
                    SelfInfo.pszOpenStoreProvider =
                        sz_CERT_STORE_PROV_SYSTEM_REGISTRY_W;
                    SelfInfo.dwOpenFlags = dwFlags &
                        CERT_SYSTEM_STORE_LOCATION_MASK;
                    if (0 == _wcsicmp(
                        SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX],
                            wsz_MY_STORE))
                        SelfInfo.dwOpenFlags |= CERT_STORE_UPDATE_KEYID_FLAG;
                    SelfInfo.OpenParameters.pbData = (BYTE *) pwszLocalStore;
                    SelfInfo.OpenParameters.cbData = cbLocalStore;
                    SelfInfo.dwFlags = CERT_PHYSICAL_STORE_ADD_ENABLE_FLAG;
                    break;
                case GROUP_POLICY_PHYSICAL_INDEX:
                    if (CERT_SYSTEM_STORE_LOCAL_MACHINE_ID ==
                            dwStoreLocationID)
                        SelfInfo.dwOpenFlags =
                            CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY |
                                CERT_STORE_READONLY_FLAG;
                    else
                        SelfInfo.dwOpenFlags =
                            CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY |
                                CERT_STORE_READONLY_FLAG;
                    break;
                case LOCAL_MACHINE_PHYSICAL_INDEX:
                    SelfInfo.dwOpenFlags = CERT_SYSTEM_STORE_LOCAL_MACHINE |
                        CERT_STORE_READONLY_FLAG;
                    break;
                case DS_USER_CERT_PHYSICAL_INDEX:
                    if (NULL == (pwszUserDsUserCertificateUrl =
                            GetUserDsUserCertificateUrl()))
                        continue;
                    SelfInfo.pszOpenStoreProvider = sz_CERT_STORE_PROV_LDAP_W;
                    SelfInfo.dwOpenFlags = 0;
                    SelfInfo.OpenParameters.pbData =
                        (BYTE *) pwszUserDsUserCertificateUrl;
                    SelfInfo.OpenParameters.cbData = (wcslen(
                        pwszUserDsUserCertificateUrl) + 1) * sizeof(WCHAR);
                    SelfInfo.dwFlags = CERT_PHYSICAL_STORE_ADD_ENABLE_FLAG;
                    break;
                case LMGP_PHYSICAL_INDEX:
                    SelfInfo.dwOpenFlags =
                        CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY |
                            CERT_STORE_READONLY_FLAG;
                    break;
                case ENTERPRISE_PHYSICAL_INDEX:
                    SelfInfo.dwOpenFlags =
                        CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE |
                            CERT_STORE_READONLY_FLAG;
                    break;
                default:
                    assert(i < NUM_PREDEFINED_PHYSICAL);
                    continue;

            }

            if (dwFlags & CERT_STORE_MAXIMUM_ALLOWED_FLAG) {
                SelfInfo.dwOpenFlags |= CERT_STORE_MAXIMUM_ALLOWED_FLAG;
                SelfInfo.dwOpenFlags &= ~CERT_STORE_READONLY_FLAG;
            }

            fResult = pfnEnum(
                    pvSystemStore,
                    (dwFlags & CERT_SYSTEM_STORE_MASK) |
                        CERT_PHYSICAL_STORE_PREDEFINED_ENUM_FLAG,
                    rgpwszPredefinedPhysical[i],        // pwszStoreName
                    &SelfInfo,
                    NULL,                               // pvReserved
                    pvArg
                    );
            if (pwszUserDsUserCertificateUrl)
                CryptMemFree(pwszUserDsUserCertificateUrl);
            if (!fResult)
                break;
        }

        if (SystemNameInfo.rgpwszName[COMPUTER_NAME_INDEX])
            PkiFree(pwszLocalStore);
        if (!fResult)
            goto EnumCallbackError;
    }

    fResult = TRUE;
CommonReturn:
    if (plDepth)
        *plDepth -= 1;
    ILS_CloseRegistryKey(hKey);
    FreeSystemNameInfo(&SystemNameInfo);
    PkiFree(pwszStoreName);
    FreeEnumPhysicalStoreList(pStoreInfoHead);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(ParseSystemStoreParaError)
SET_ERROR(InvalidArg, E_INVALIDARG)
TRACE_ERROR(OutOfMemory)
SET_ERROR(ExceededEnumPhysicalStoreDepth_PossibleCrossStoreRecursion, E_UNEXPECTED)
TRACE_ERROR(OpenPhysicalStoresError)
TRACE_ERROR(OpenSystemStoreError)
TRACE_ERROR(GetSubKeyInfoError)
TRACE_ERROR(FormatSystemNamePathError)
TRACE_ERROR(EnumCallbackError)
}

#ifdef _ARM_
#pragma optimize ("", on)
#endif

//+-------------------------------------------------------------------------
//  Enumerate the physical stores for the specified system store.
//
//  The upper word of the dwFlags parameter is used to specify the location of
//  the system store.
//
//  If the system store location only supports system stores and doesn't
//  support physical stores, LastError is set to ERROR_CALL_NOT_IMPLEMENTED.
//--------------------------------------------------------------------------
BOOL
WINAPI
CertEnumPhysicalStore(
    IN const void *pvSystemStore,
    IN DWORD dwFlags,
    IN void *pvArg,
    IN PFN_CERT_ENUM_PHYSICAL_STORE pfnEnum
    )
{
    return EnumPhysicalStore(
        pvSystemStore,
        dwFlags | CERT_STORE_OPEN_EXISTING_FLAG | CERT_STORE_READONLY_FLAG,
        pvArg,
        pfnEnum
        );
}

STATIC BOOL IsHKCUMyStore(
    IN PSYSTEM_NAME_INFO pInfo,
    IN DWORD dwFlags
    )
{
    DWORD dwStoreLocation = dwFlags & CERT_SYSTEM_STORE_LOCATION_MASK;

    if (CERT_SYSTEM_STORE_CURRENT_USER != dwStoreLocation ||
            0 != _wcsicmp(pInfo->rgpwszName[SYSTEM_NAME_INDEX], wsz_MY_STORE))
        return FALSE;

    if (dwFlags & (CERT_SYSTEM_STORE_RELOCATE_FLAG | CERT_STORE_DELETE_FLAG))
        return FALSE;

    return TRUE;
}

//+-------------------------------------------------------------------------
//  Open the system registry store provider (unicode version)
//
//  Open the system registry store specified by its name. For example,
//  L"My".
//
//  pvPara contains the LPCWSTR system registry store name.
//
//  Note for an error return, the caller will free any certs, CRLs or CTLs
//  successfully added to the store.
//--------------------------------------------------------------------------
BOOL
WINAPI
I_CertDllOpenSystemRegistryStoreProvW(
        IN LPCSTR lpszStoreProvider,
        IN DWORD dwEncodingType,
        IN HCRYPTPROV hCryptProv,
        IN DWORD dwFlags,
        IN const void *pvPara,
        IN HCERTSTORE hCertStore,
        IN OUT PCERT_STORE_PROV_INFO pStoreProvInfo
        )
{
    BOOL fResult;
    LONG err;
    HKEY hKey = NULL;
    SYSTEM_NAME_INFO SystemNameInfo;
    BOOL fUserRoot;
    HKEY hHKCURoot = NULL;
    DWORD dwOpenRegFlags;
    const void *pvOpenRegPara;
    LPWSTR pwszRoamingDirectory = NULL;
    CERT_REGISTRY_STORE_ROAMING_PARA RoamingStorePara;

    CERT_REGISTRY_STORE_CLIENT_GPT_PARA ClientGptStorePara;
    memset(&ClientGptStorePara, 0, sizeof(ClientGptStorePara));

    if (!ParseSystemStorePara(
            pvPara,
            dwFlags,
            1,                  // cReqName
            &SystemNameInfo))   // zero'ed on error
        goto ParseSystemStoreParaError;

    if (dwFlags & ~OPEN_SYS_FLAGS_MASK)
        goto InvalidArg;

    // Check for the CurrentUser "Root" store.
    fUserRoot = FALSE;
    if (0 == _wcsicmp(SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX],
            wsz_ROOT_STORE) &&
                0 == (dwFlags & CERT_SYSTEM_STORE_UNPROTECTED_FLAG)) {
        DWORD dwStoreLocation = dwFlags & CERT_SYSTEM_STORE_LOCATION_MASK;

        // Note, LOCAL_MACHINE check is needed to prevent use of relocation
        // to access the current user's root store
        if (CERT_SYSTEM_STORE_CURRENT_USER == dwStoreLocation) {
            fUserRoot = TRUE;
            if (NULL == SystemNameInfo.hKeyBase) {
                if (ERROR_SUCCESS != (err = RegOpenHKCUEx(
                        &hHKCURoot,
                        REG_HKCU_DISABLE_DEFAULT_FLAG
                        )))
                    goto RegOpenHKCUExRootError;

                SystemNameInfo.hKeyBase = hHKCURoot;
                dwFlags |= CERT_SYSTEM_STORE_RELOCATE_FLAG;
            }
        } else if (CERT_SYSTEM_STORE_USERS == dwStoreLocation ||
                ((dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG) &&
                    CERT_SYSTEM_STORE_LOCAL_MACHINE == dwStoreLocation))
            goto RootAccessDenied;
    }

    if (IsClientGptStore(&SystemNameInfo, dwFlags)) {
        DWORD dwStoreLocation;

        assert(!fUserRoot);
        if (NULL == (ClientGptStorePara.pwszRegPath = FormatSystemRegPath(
                &SystemNameInfo,
                NULL,               // pwszSubKeyName
                dwFlags,
                &ClientGptStorePara.hKeyBase)))
            goto FormatSystemRegPathError;
        pvOpenRegPara = (const void *) &ClientGptStorePara;

        dwOpenRegFlags =
            dwFlags & ~(CERT_SYSTEM_STORE_MASK |
                CERT_STORE_SET_LOCALIZED_NAME_FLAG);
        dwOpenRegFlags |= CERT_REGISTRY_STORE_CLIENT_GPT_FLAG;
            // | CERT_REGISTRY_STORE_SERIALIZED_FLAG;

        dwStoreLocation = dwFlags & CERT_SYSTEM_STORE_LOCATION_MASK;
        if (CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY == dwStoreLocation)
            dwOpenRegFlags |= CERT_REGISTRY_STORE_LM_GPT_FLAG;

        if (SystemNameInfo.rgpwszName[COMPUTER_NAME_INDEX])
            dwOpenRegFlags |= CERT_REGISTRY_STORE_REMOTE_FLAG;
    } else {
        BOOL fIsHKCUMyStore;

        fIsHKCUMyStore = IsHKCUMyStore(&SystemNameInfo, dwFlags);

        if (fIsHKCUMyStore && NULL != (pwszRoamingDirectory =
                ILS_GetRoamingStoreDirectory(ROAMING_MY_STORE_SUBDIR))) {
            // OK for this to fail. After the first open, all contexts should
            // be persisted in files and not the registry.
            hKey = OpenSystemRegPathKey(
                &SystemNameInfo,
                NULL,               // pwszSubKeyName
                (dwFlags & ~CERT_STORE_CREATE_NEW_FLAG) |
                    CERT_STORE_OPEN_EXISTING_FLAG
                );

            RoamingStorePara.hKey = hKey;
            RoamingStorePara.pwszStoreDirectory = pwszRoamingDirectory;
            pvOpenRegPara = (const void *) &RoamingStorePara;

            dwOpenRegFlags =
                dwFlags & ~(CERT_SYSTEM_STORE_MASK |
                    CERT_STORE_CREATE_NEW_FLAG |
                    CERT_STORE_SET_LOCALIZED_NAME_FLAG);
            dwOpenRegFlags |= CERT_REGISTRY_STORE_ROAMING_FLAG;
        } else {
            if (NULL == (hKey = OpenSystemRegPathKey(
                    &SystemNameInfo,
                    NULL,               // pwszSubKeyName
                    dwFlags)))
                goto OpenSystemStoreError;
            pvOpenRegPara = (const void *) hKey;

            dwOpenRegFlags =
                dwFlags & ~(CERT_SYSTEM_STORE_MASK |
                    CERT_STORE_CREATE_NEW_FLAG |
                    CERT_STORE_SET_LOCALIZED_NAME_FLAG);
            if (SystemNameInfo.rgpwszName[COMPUTER_NAME_INDEX])
                dwOpenRegFlags |= CERT_REGISTRY_STORE_REMOTE_FLAG;
            if (IsSerializedSystemStoreLocationInRegistry(dwFlags)) {
                assert(!fUserRoot);
                dwOpenRegFlags |= CERT_REGISTRY_STORE_SERIALIZED_FLAG;
            }
        }

        if (fIsHKCUMyStore)
            dwOpenRegFlags |= CERT_REGISTRY_STORE_MY_IE_DIRTY_FLAG;
    }

    if (fUserRoot)
        IPR_InitProtectedRootInfo();

    if (!I_CertDllOpenRegStoreProv(
            NULL,                       // lpszStoreProvider
            dwEncodingType,
            hCryptProv,
            dwOpenRegFlags,
            pvOpenRegPara,
            hCertStore,
            pStoreProvInfo))
        goto OpenRegStoreProvError;

    if (fUserRoot) {
        PREG_STORE pRegStore = (PREG_STORE) pStoreProvInfo->hStoreProv;

        // Set count to 0 to inhibit any callbacks from being called.
        pStoreProvInfo->cStoreProvFunc = 0;

        // For the "Root" delete any roots that aren't in the protected root
        // list.
        if (!IPR_DeleteUnprotectedRootsFromStore(
                hCertStore,
                &pRegStore->fProtected
                )) goto DeleteUnprotectedRootsError;

        // For the "Root" replace some of the provider callback functions
        // that first prompt the user directly (if not protected) or
        // prompt the user via the system service (if protected).
        pStoreProvInfo->cStoreProvFunc = ROOT_STORE_PROV_FUNC_COUNT;
        pStoreProvInfo->rgpvStoreProvFunc = (void **) rgpvRootStoreProvFunc;
    }

    if (dwFlags & CERT_STORE_SET_LOCALIZED_NAME_FLAG)
        SetLocalizedNameStoreProperty(hCertStore, &SystemNameInfo);

    pStoreProvInfo->dwStoreProvFlags |= CERT_STORE_PROV_SYSTEM_STORE_FLAG;
    fResult = TRUE;
CommonReturn:
    if (SystemNameInfo.rgpwszName[COMPUTER_NAME_INDEX] &&
            ClientGptStorePara.hKeyBase)
        ILS_CloseRegistryKey(ClientGptStorePara.hKeyBase);
    PkiFree(ClientGptStorePara.pwszRegPath);

    FreeSystemNameInfo(&SystemNameInfo);
    PkiFree(pwszRoamingDirectory);
    ILS_CloseRegistryKey(hKey);
    if (hHKCURoot)
        RegCloseHKCU(hHKCURoot);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(ParseSystemStoreParaError)
SET_ERROR(InvalidArg, E_INVALIDARG)
SET_ERROR(RootAccessDenied, E_ACCESSDENIED)
SET_ERROR_VAR(RegOpenHKCUExRootError, err)
TRACE_ERROR(FormatSystemRegPathError)
TRACE_ERROR(OpenSystemStoreError)
TRACE_ERROR(OpenRegStoreProvError)
TRACE_ERROR(DeleteUnprotectedRootsError)
}

//+-------------------------------------------------------------------------
//  Open the system registry store provider (ascii version)
//
//  Open the system registry store specified by its name. For example,
//  "My".
//
//  pvPara contains the LPCSTR system store name.
//
//  Note for an error return, the caller will free any certs or CRLs
//  successfully added to the store.
//--------------------------------------------------------------------------
BOOL
WINAPI
I_CertDllOpenSystemRegistryStoreProvA(
        IN LPCSTR lpszStoreProvider,
        IN DWORD dwEncodingType,
        IN HCRYPTPROV hCryptProv,
        IN DWORD dwFlags,
        IN const void *pvPara,
        IN HCERTSTORE hCertStore,
        IN OUT PCERT_STORE_PROV_INFO pStoreProvInfo
        )
{
    BOOL fResult;
    LPCSTR pszStoreName;    // not allocated
    LPWSTR pwszStoreName;

    CERT_SYSTEM_STORE_RELOCATE_PARA RelocatePara;

    if (dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG) {
        PCERT_SYSTEM_STORE_RELOCATE_PARA pInPara;

        assert(pvPara);
        pInPara = (PCERT_SYSTEM_STORE_RELOCATE_PARA) pvPara;
        RelocatePara.hKeyBase = pInPara->hKeyBase;
        pszStoreName = pInPara->pszSystemStore;
    } else
        pszStoreName = (LPCSTR) pvPara;

    assert(pszStoreName);

    if (NULL == (pwszStoreName = MkWStr((LPSTR) pszStoreName)))
        fResult = FALSE;
    else {
        const void *pvParaW;

        if (dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG) {
            RelocatePara.pwszSystemStore = pwszStoreName;
            pvParaW = (const void *) &RelocatePara;
        } else
            pvParaW = (const void *) pwszStoreName;

        fResult = I_CertDllOpenSystemRegistryStoreProvW(
            NULL,                       // lpszStoreProvider
            dwEncodingType,
            hCryptProv,
            dwFlags,
            pvParaW,
            hCertStore,
            pStoreProvInfo
            );
        FreeWStr(pwszStoreName);
    }
    return fResult;
}

typedef struct _OPEN_PHYSICAL_STORE_INFO {
    HCERTSTORE      hCollectionStore;
    LPCWSTR         pwszComputerName;       // NULL implies local
    LPCWSTR         pwszServiceName;        // NULL implies current
    LPCWSTR         pwszPhysicalName;       // NULL implies any
    HKEY            hKeyBase;               // non-NULL, relocatable
    DWORD           dwFlags;
    BOOL            fDidOpen;
} OPEN_PHYSICAL_STORE_INFO, *POPEN_PHYSICAL_STORE_INFO;


STATIC BOOL WINAPI OpenPhysicalStoreCallback(
    IN const void *pvSystemStore,
    IN DWORD dwFlags,
    IN LPCWSTR pwszStoreName,
    IN PCERT_PHYSICAL_STORE_INFO pStoreInfo,
    IN OPTIONAL void *pvReserved,
    IN OPTIONAL void *pvArg
    )
{
    BOOL fResult;
    HCERTSTORE hPhysicalStore = NULL;
    POPEN_PHYSICAL_STORE_INFO pOpenInfo =
        (POPEN_PHYSICAL_STORE_INFO) pvArg;
    void *pvOpenParameters;
    LPWSTR pwszRemoteOpenParameters = NULL;
    LPCSTR pszOpenStoreProvider;
    DWORD dwOpenFlags;
    DWORD dwAddFlags;

    CERT_SYSTEM_STORE_RELOCATE_PARA RelocateOpenParameters;

    if ((pStoreInfo->dwFlags & CERT_PHYSICAL_STORE_OPEN_DISABLE_FLAG)
                        ||
            (pOpenInfo->pwszPhysicalName &&
                0 != _wcsicmp(pOpenInfo->pwszPhysicalName, pwszStoreName))
                        ||
            (pOpenInfo->pwszComputerName &&
                (pStoreInfo->dwFlags &
                    CERT_PHYSICAL_STORE_REMOTE_OPEN_DISABLE_FLAG)))
        return TRUE;

    pvOpenParameters = pStoreInfo->OpenParameters.pbData;
    assert(pvOpenParameters);
    dwOpenFlags = pStoreInfo->dwOpenFlags;
    pszOpenStoreProvider = pStoreInfo->pszOpenStoreProvider;

    if (pOpenInfo->pwszComputerName || pOpenInfo->pwszServiceName) {
        // Possibly insert the \\ComputerName\ServiceName before the
        // OpenParameters

        LPCWSTR pwszComputerName = NULL;
        LPCWSTR pwszServiceName = NULL;
        LPWSTR pwszSystemStore = (LPWSTR) pvOpenParameters;
        DWORD dwSystemProviderFlags =
            GetSystemProviderFlags(pszOpenStoreProvider);

        if (0 != dwSystemProviderFlags) {
            SYSTEM_NAME_INFO ProviderNameInfo;
            DWORD cReqName;

            if (dwOpenFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG)
                goto RelocateFlagSetInPhysicalStoreInfoError;

            if (dwSystemProviderFlags & ASCII_SYSTEM_PROVIDER_FLAG) {
                if (NULL == (pwszSystemStore =
                        MkWStr((LPSTR) pvOpenParameters)))
                    goto OutOfMemory;
            }

            if (dwSystemProviderFlags & PHYSICAL_PROVIDER_FLAG)
                cReqName = 2;
            else
                cReqName = 1;

            ParseSystemStorePara(
                    pwszSystemStore,
                    dwOpenFlags,
                    cReqName,
                    &ProviderNameInfo      // zero'ed on error
                    );
            if (ProviderNameInfo.rgpwszName[COMPUTER_NAME_INDEX]) {
                // Already has \\ComputerName\ prefix. For Services or
                // Users, already has ServiceName\ prefix.
                ;
            } else if (ProviderNameInfo.rgpwszName[SYSTEM_NAME_INDEX]) {
                // Needed above check if ParseSystemStorePara failed.
                pwszComputerName = pOpenInfo->pwszComputerName;

                if (pOpenInfo->pwszServiceName) {
                    // If the provider store is located in CURRENT_SERVICE or
                    // CURRENT_USER use outer store's SERVICE_NAME and change
                    // store location accordingly

                    DWORD dwOpenLocation =
                        dwOpenFlags & CERT_SYSTEM_STORE_LOCATION_MASK;
                    if (CERT_SYSTEM_STORE_CURRENT_SERVICE == dwOpenLocation) {
                        pwszServiceName = pOpenInfo->pwszServiceName;
                        dwOpenFlags =
                            (dwOpenFlags & ~CERT_SYSTEM_STORE_LOCATION_MASK) |
                                CERT_SYSTEM_STORE_SERVICES;
                    } else if (CERT_SYSTEM_STORE_CURRENT_USER ==
                            dwOpenLocation) {
                        pwszServiceName = pOpenInfo->pwszServiceName;
                        dwOpenFlags =
                            (dwOpenFlags & ~CERT_SYSTEM_STORE_LOCATION_MASK) |
                                CERT_SYSTEM_STORE_USERS;
                    }

                }
            }
            FreeSystemNameInfo(&ProviderNameInfo);
        } else if (pStoreInfo->dwFlags &
                CERT_PHYSICAL_STORE_INSERT_COMPUTER_NAME_ENABLE_FLAG)
            pwszComputerName = pOpenInfo->pwszComputerName;

        if (pwszComputerName || pwszServiceName) {
            // Insert \\ComputerName\ServiceName before and re-format
            // open parameters
            LPCWSTR rgpwszName[3];
            SYSTEM_NAME_GROUP NameGroup;

            assert(pwszSystemStore);

            NameGroup.cName = 3;
            NameGroup.rgpwszName = rgpwszName;
            rgpwszName[0] = pwszComputerName;
            rgpwszName[1] = pwszServiceName;
            rgpwszName[2] = pwszSystemStore;
            pwszRemoteOpenParameters = FormatSystemNamePath(1, &NameGroup);
            pvOpenParameters = pwszRemoteOpenParameters;

            if (dwSystemProviderFlags & ASCII_SYSTEM_PROVIDER_FLAG)
                pszOpenStoreProvider = ChangeAsciiToUnicodeProvider(
                    pszOpenStoreProvider);
        }

        if (dwSystemProviderFlags & ASCII_SYSTEM_PROVIDER_FLAG) {
            FreeWStr(pwszSystemStore);
            if (NULL == pszOpenStoreProvider)
                goto UnableToChangeToUnicodeProvider;
        }
        if (NULL == pvOpenParameters)
            goto FormatSystemNamePathError;
    }

    if (NULL != pOpenInfo->hKeyBase &&
            0 != GetSystemProviderFlags(pszOpenStoreProvider)) {
        if (dwOpenFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG)
            goto RelocateFlagSetInPhysicalStoreInfoError;

        // Inherit outer store's hKeyBase and convert to a relocated
        // physical store
        RelocateOpenParameters.hKeyBase = pOpenInfo->hKeyBase;
        RelocateOpenParameters.pvSystemStore = pvOpenParameters;
        pvOpenParameters = &RelocateOpenParameters;
        dwOpenFlags |= CERT_SYSTEM_STORE_RELOCATE_FLAG;
    }

    if (NULL == (hPhysicalStore = CertOpenStore(
            pszOpenStoreProvider,
            pStoreInfo->dwOpenEncodingType,
            0,                                  // hCryptProv
            dwOpenFlags | (pOpenInfo->dwFlags &
                                 (CERT_STORE_READONLY_FLAG |
                                  CERT_STORE_OPEN_EXISTING_FLAG |
                                  CERT_STORE_MANIFOLD_FLAG |
                                  CERT_STORE_UPDATE_KEYID_FLAG |
                                  CERT_STORE_ENUM_ARCHIVED_FLAG)),
            pvOpenParameters))) {
        DWORD dwErr = GetLastError();
        if (ERROR_FILE_NOT_FOUND == dwErr || ERROR_PROC_NOT_FOUND == dwErr ||
                ERROR_GEN_FAILURE == dwErr) {
            if (pOpenInfo->pwszPhysicalName &&
                    (dwFlags & CERT_PHYSICAL_STORE_PREDEFINED_ENUM_FLAG)) {
                // For a predefined physical convert to an empty collection
                CertAddStoreToCollection(
                    pOpenInfo->hCollectionStore,
                    NULL,           // hSiblingStore, NULL implies convert only
                    0,              // dwFlags
                    0               // dwPriority
                    );
                goto OpenReturn;
            } else
                goto SuccessReturn;
        } else
            goto OpenPhysicalStoreError;
    }

    dwAddFlags = pStoreInfo->dwFlags;
    if ((dwOpenFlags & CERT_STORE_MAXIMUM_ALLOWED_FLAG) &&
            0 == (dwAddFlags & CERT_PHYSICAL_STORE_ADD_ENABLE_FLAG) &&
            pOpenInfo->pwszPhysicalName) {
        DWORD dwAccessStateFlags;
        DWORD cbData = sizeof(dwAccessStateFlags);

        if (CertGetStoreProperty(
                hPhysicalStore,
                CERT_ACCESS_STATE_PROP_ID,
                &dwAccessStateFlags,
                &cbData
                )) {
            if (dwAccessStateFlags & CERT_ACCESS_STATE_WRITE_PERSIST_FLAG)
                dwAddFlags |= CERT_PHYSICAL_STORE_ADD_ENABLE_FLAG;
        }
    }

    if (!CertAddStoreToCollection(
            pOpenInfo->hCollectionStore,
            hPhysicalStore,
            dwAddFlags,
            pStoreInfo->dwPriority))
        goto AddStoreToCollectionError;

OpenReturn:
    pOpenInfo->fDidOpen = TRUE;
SuccessReturn:
    fResult = TRUE;
CommonReturn:
    PkiFree(pwszRemoteOpenParameters);
    if (hPhysicalStore)
        CertCloseStore(hPhysicalStore, 0);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;
TRACE_ERROR(OutOfMemory)
SET_ERROR(UnableToChangeToUnicodeProvider, E_UNEXPECTED)
TRACE_ERROR(OpenPhysicalStoreError)
TRACE_ERROR(AddStoreToCollectionError)
TRACE_ERROR(FormatSystemNamePathError)
SET_ERROR(RelocateFlagSetInPhysicalStoreInfoError, E_INVALIDARG)
}

//+-------------------------------------------------------------------------
//  Open the system store provider (unicode version)
//
//  Open the system store specified by its name. For example,
//  L"My".
//
//  pvPara contains the LPCWSTR system store name.
//
//  Note for an error return, the caller will free any certs, CRLs or CTLs
//  successfully added to the store.
//--------------------------------------------------------------------------
BOOL
WINAPI
I_CertDllOpenSystemStoreProvW(
        IN LPCSTR lpszStoreProvider,
        IN DWORD dwEncodingType,
        IN HCRYPTPROV hCryptProv,
        IN DWORD dwFlags,
        IN const void *pvPara,
        IN HCERTSTORE hCertStore,
        IN OUT PCERT_STORE_PROV_INFO pStoreProvInfo
        )
{
    BOOL fResult;
    SYSTEM_NAME_INFO SystemNameInfo;

    pStoreProvInfo->dwStoreProvFlags |= CERT_STORE_PROV_SYSTEM_STORE_FLAG;

    if (!IsSystemStoreLocationInRegistry(dwFlags)) {
        void *pvFuncAddr;

        assert(NULL == pStoreProvInfo->hStoreProvFuncAddr2);
        if (!CryptGetOIDFunctionAddress(
                rghFuncSet[OPEN_SYSTEM_STORE_PROV_FUNC_SET],
                0,                      // dwEncodingType,
                GetSystemStoreLocationOID(dwFlags),
                0,                      // dwFlags
                &pvFuncAddr,
                &pStoreProvInfo->hStoreProvFuncAddr2))
            return FALSE;

        fResult = ((PFN_CERT_DLL_OPEN_STORE_PROV_FUNC) pvFuncAddr)(
            lpszStoreProvider,
            dwEncodingType,
            hCryptProv,
            dwFlags,
            pvPara,
            hCertStore,
            pStoreProvInfo
            );
        // Note, hStoreProvFuncAddr2 is CryptFreeOIDFunctionAddress'ed by
        // CertCloseStore()
        return fResult;
    }

    if (!ParseSystemStorePara(
            pvPara,
            dwFlags,
            1,                      // cReqName
            &SystemNameInfo))       // zero'ed on error
        goto ParseSystemStoreParaError;

    if (dwFlags & ~OPEN_SYS_FLAGS_MASK)
        goto InvalidArg;

    if (dwFlags & CERT_STORE_DELETE_FLAG) {
        // Need to clear out CERT_STORE_NO_CRYPT_RELEASE_FLAG
        if (!CertUnregisterSystemStore(
                pvPara,
                dwFlags & UNREGISTER_FLAGS_MASK
                ))
            goto UnregisterSystemStoreError;
        pStoreProvInfo->dwStoreProvFlags |= CERT_STORE_PROV_DELETED_FLAG;
    } else {
        OPEN_PHYSICAL_STORE_INFO OpenInfo;

        if (dwFlags & CERT_STORE_CREATE_NEW_FLAG) {
            HKEY hKey;
            if (NULL == (hKey = OpenSystemStore(pvPara, dwFlags)))
                goto OpenSystemStoreError;
            RegCloseKey(hKey);
            dwFlags &= ~CERT_STORE_CREATE_NEW_FLAG;
        }

        OpenInfo.hCollectionStore = hCertStore;
        OpenInfo.pwszComputerName =
            SystemNameInfo.rgpwszName[COMPUTER_NAME_INDEX];
        OpenInfo.pwszServiceName =
            SystemNameInfo.rgpwszName[SERVICE_NAME_INDEX];
        OpenInfo.pwszPhysicalName = NULL;       // NULL implies any
        OpenInfo.hKeyBase = SystemNameInfo.hKeyBase;
        OpenInfo.dwFlags = dwFlags & ~CERT_STORE_SET_LOCALIZED_NAME_FLAG;
        OpenInfo.fDidOpen = FALSE;

        // Need to clear out CERT_STORE_NO_CRYPT_RELEASE_FLAG
        if (!EnumPhysicalStore(
                pvPara,
                dwFlags & ENUM_FLAGS_MASK,
                &OpenInfo,
                OpenPhysicalStoreCallback
                ))
            goto EnumPhysicalStoreError;

        if (!OpenInfo.fDidOpen) {
            if (IsPredefinedSystemStore(
                    SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX], dwFlags))
                // Convert to a collection store
                CertAddStoreToCollection(
                    hCertStore,
                    NULL,           // hSiblingStore, NULL implies convert only
                    0,              // dwFlags
                    0               // dwPriority
                    );
            else
                goto PhysicalStoreNotFound;
        }

        if (dwFlags & CERT_STORE_SET_LOCALIZED_NAME_FLAG)
            SetLocalizedNameStoreProperty(hCertStore, &SystemNameInfo);
    }

    fResult = TRUE;
CommonReturn:
    FreeSystemNameInfo(&SystemNameInfo);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(ParseSystemStoreParaError)
SET_ERROR(InvalidArg, E_INVALIDARG)
TRACE_ERROR(UnregisterSystemStoreError)
TRACE_ERROR(OpenSystemStoreError)
TRACE_ERROR(EnumPhysicalStoreError)
SET_ERROR(PhysicalStoreNotFound, ERROR_FILE_NOT_FOUND)
}

//+-------------------------------------------------------------------------
//  Open the system store provider (ascii version)
//
//  Open the system store specified by its name. For example,
//  "My".
//
//  pvPara contains the LPCSTR system store name.
//
//  Note for an error return, the caller will free any certs or CRLs
//  successfully added to the store.
//--------------------------------------------------------------------------
BOOL
WINAPI
I_CertDllOpenSystemStoreProvA(
        IN LPCSTR lpszStoreProvider,
        IN DWORD dwEncodingType,
        IN HCRYPTPROV hCryptProv,
        IN DWORD dwFlags,
        IN const void *pvPara,
        IN HCERTSTORE hCertStore,
        IN OUT PCERT_STORE_PROV_INFO pStoreProvInfo
        )
{
    BOOL fResult;
    LPCSTR pszStoreName;    // not allocated
    LPWSTR pwszStoreName;

    CERT_SYSTEM_STORE_RELOCATE_PARA RelocatePara;

    if (dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG) {
        PCERT_SYSTEM_STORE_RELOCATE_PARA pInPara;

        assert(pvPara);
        pInPara = (PCERT_SYSTEM_STORE_RELOCATE_PARA) pvPara;
        RelocatePara.hKeyBase = pInPara->hKeyBase;
        pszStoreName = pInPara->pszSystemStore;
    } else
        pszStoreName = (LPCSTR) pvPara;

    assert(pszStoreName);

    if (NULL == (pwszStoreName = MkWStr((LPSTR) pszStoreName)))
        fResult = FALSE;
    else {
        const void *pvParaW;

        if (dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG) {
            RelocatePara.pwszSystemStore = pwszStoreName;
            pvParaW = (const void *) &RelocatePara;
        } else
            pvParaW = (const void *) pwszStoreName;

        fResult = I_CertDllOpenSystemStoreProvW(
            NULL,                       // lpszStoreProvider
            dwEncodingType,
            hCryptProv,
            dwFlags,
            pvParaW,
            hCertStore,
            pStoreProvInfo
            );
        FreeWStr(pwszStoreName);
    }
    return fResult;
}


//+-------------------------------------------------------------------------
//  Open the physical store provider (unicode version)
//
//  Open the physical store in the specified system store. For example,
//  L"My\.Default".
//
//  pvPara contains the LPCWSTR pwszSystemAndPhysicalName which is the
//  concatenation of the system and physical store names with an
//  intervening "\".
//
//  Note for an error return, the caller will free any certs, CRLs or CTLs
//  successfully added to the store.
//--------------------------------------------------------------------------
BOOL
WINAPI
I_CertDllOpenPhysicalStoreProvW(
        IN LPCSTR lpszStoreProvider,
        IN DWORD dwEncodingType,
        IN HCRYPTPROV hCryptProv,
        IN DWORD dwFlags,
        IN const void *pvPara,
        IN HCERTSTORE hCertStore,
        IN OUT PCERT_STORE_PROV_INFO pStoreProvInfo
        )
{
    BOOL fResult;
    LPCWSTR pwszBoth;           // not allocated
    LPWSTR pwszSystem = NULL;   // allocated
    DWORD cchSystem;
    LPCWSTR pwszPhysical;       // not allocated

    void *pvSystemPara;         // not allocated
    CERT_SYSTEM_STORE_RELOCATE_PARA RelocatePara;

    if (dwFlags & ~OPEN_PHY_FLAGS_MASK)
        goto InvalidArg;

    if (dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG) {
        PCERT_SYSTEM_STORE_RELOCATE_PARA pInPara;

        if (NULL == pvPara)
            goto InvalidArg;
        pInPara = (PCERT_SYSTEM_STORE_RELOCATE_PARA) pvPara;
        pwszBoth = pInPara->pwszSystemStore;
    } else
        pwszBoth = (LPCWSTR) pvPara;

    // Extract the system and physical name components by starting at
    // the end and searching backwards for the first "\"
    if (NULL == pwszBoth)
        goto InvalidArg;
    pwszPhysical = pwszBoth + wcslen(pwszBoth);
    while (pwszPhysical > pwszBoth && L'\\' != *pwszPhysical)
        pwszPhysical--;

    cchSystem = (DWORD)(pwszPhysical - pwszBoth);
    pwszPhysical++;     // advance past "\"
    if (0 < cchSystem && L'\0' != *pwszPhysical) {
        if (NULL == (pwszSystem = AllocAndCopyString(pwszBoth, cchSystem)))
            goto OutOfMemory;
    } else
        // Missing "\" or empty System or Physical Name.
        goto InvalidArg;

    if (dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG) {
        PCERT_SYSTEM_STORE_RELOCATE_PARA pInPara =
            (PCERT_SYSTEM_STORE_RELOCATE_PARA) pvPara;
        RelocatePara.hKeyBase = pInPara->hKeyBase;
        RelocatePara.pwszSystemStore = pwszSystem;
        pvSystemPara = &RelocatePara;
    } else
        pvSystemPara = pwszSystem;

    if (dwFlags & CERT_STORE_DELETE_FLAG) {
        // Need to clear out CERT_STORE_NO_CRYPT_RELEASE_FLAG
        if (!CertUnregisterPhysicalStore(
                pvSystemPara,
                dwFlags & UNREGISTER_FLAGS_MASK,
                pwszPhysical
                ))
            goto UnregisterPhysicalStoreError;
        pStoreProvInfo->dwStoreProvFlags |= CERT_STORE_PROV_DELETED_FLAG;
    } else {
        SYSTEM_NAME_INFO SystemNameInfo;
        OPEN_PHYSICAL_STORE_INFO OpenInfo;

        // Note, already removed PhysicalName above. That's why
        // cReqName is 1 and not 2.
        if (!ParseSystemStorePara(
                pvSystemPara,
                dwFlags,
                1,                      // cReqName
                &SystemNameInfo))       // zero'ed on error
            goto ParseSystemStoreParaError;

        OpenInfo.hCollectionStore = hCertStore;
        OpenInfo.pwszComputerName =
            SystemNameInfo.rgpwszName[COMPUTER_NAME_INDEX];
        OpenInfo.pwszServiceName =
            SystemNameInfo.rgpwszName[SERVICE_NAME_INDEX];
        OpenInfo.pwszPhysicalName = pwszPhysical;
        OpenInfo.hKeyBase = SystemNameInfo.hKeyBase;
        OpenInfo.dwFlags = dwFlags & ~CERT_STORE_SET_LOCALIZED_NAME_FLAG;
        OpenInfo.fDidOpen = FALSE;

        // For .Default physical store, allow the store to be created.
        // Otherwise, the store must already exist.
        if (0 != _wcsicmp(CERT_PHYSICAL_STORE_DEFAULT_NAME, pwszPhysical))
            dwFlags |= CERT_STORE_OPEN_EXISTING_FLAG |
                CERT_STORE_READONLY_FLAG;

        // Need to clear out CERT_STORE_NO_CRYPT_RELEASE_FLAG
        fResult = EnumPhysicalStore(
                pvSystemPara,
                dwFlags & ENUM_FLAGS_MASK,
                &OpenInfo,
                OpenPhysicalStoreCallback
                );

        if (dwFlags & CERT_STORE_SET_LOCALIZED_NAME_FLAG) {
            assert(NULL == SystemNameInfo.rgpwszName[PHYSICAL_NAME_INDEX]);
            SystemNameInfo.rgpwszName[PHYSICAL_NAME_INDEX] =
                (LPWSTR) pwszPhysical;
            SetLocalizedNameStoreProperty(hCertStore, &SystemNameInfo);
            SystemNameInfo.rgpwszName[PHYSICAL_NAME_INDEX] = NULL;
        }

        FreeSystemNameInfo(&SystemNameInfo);
        if (!fResult)
            goto EnumPhysicalStoreError;
        if (!OpenInfo.fDidOpen)
            goto PhysicalStoreNotFound;

    }

    pStoreProvInfo->dwStoreProvFlags |= CERT_STORE_PROV_SYSTEM_STORE_FLAG;
    fResult = TRUE;
CommonReturn:
    PkiFree(pwszSystem);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(OutOfMemory)
SET_ERROR(InvalidArg, E_INVALIDARG)
TRACE_ERROR(UnregisterPhysicalStoreError)
TRACE_ERROR(ParseSystemStoreParaError)
TRACE_ERROR(EnumPhysicalStoreError)
SET_ERROR(PhysicalStoreNotFound, ERROR_FILE_NOT_FOUND)
}



//+=========================================================================
//  "ROOT" STORE
//==========================================================================
#ifdef CE_BUILD
#define ROOTMSGBOX_REGPATH            L"Software\\Microsoft\\Cryptography\\UI"
#define ADD_ROOT_CERT		0
#define DELETE_ROOT_CERT	1


//
// On Windows CE, the warning message that is displayed when a certificate is imported or
// removed from a root store is optional. If a UI dll is specified in
// HKLM\Software\Microsoft\Cryptography\UI, CAPI2 loads this dll and calls its "RootStorePrompt"
// entry point. The implementation of this function can display a suitable prompt to the
// user. If the function returns anything other than IDYES, the operation is cancelled.
// If the registry key is not present or the Dll is not found, we assume it is okay
// to import/delete the root cert and return IDYES.
//
typedef int (* PFNROOTSTOREPROMPT)(PCCERT_CONTEXT, UINT, UINT);

int
CeRootMessageBox(
    IN PCCERT_CONTEXT pCert, // cert being added or deleted
    IN UINT wActionID,	// 0 -> Add, 1 -> Delete
    IN UINT uFlags
    )
{
	HMODULE hDll = NULL;
	int iRet = IDYES;
	LPWSTR pszDllPath = NULL;
	HKEY hKey;
	PFNROOTSTOREPROMPT pfnRootStoreMessageBox;

	DEBUGMSG(1,(TEXT("CRYPT32: %s certificate in Root Store\n"), wActionID ? TEXT("Deleting") : TEXT("Adding")));
    if (ERROR_SUCCESS != RegOpenKeyExU(
            HKEY_LOCAL_MACHINE,
            ROOTMSGBOX_REGPATH,
            0,                      // dwReserved
            KEY_READ,
            &hKey))
	
	{
		return iRet;
	}
	pszDllPath =  ReadSZValueFromRegistry(hKey, TEXT("Dll"));
	if (pszDllPath
		&& (hDll = LoadLibraryW(pszDllPath))
		&& (pfnRootStoreMessageBox = (PFNROOTSTOREPROMPT) GetProcAddress(hDll,TEXT("RootStorePrompt"))))
	{
		__try {
			iRet = pfnRootStoreMessageBox(pCert, wActionID, uFlags);
		} __except(EXCEPTION_EXECUTE_HANDLER)
		{
			// there was a DLL but it faulted
			// play it safe and return a failure
			iRet = IDABORT;
		}
	}
	if (hDll)
		FreeLibrary(hDll);
	if (hKey)
		RegCloseKey(hKey);
	PkiFree(pszDllPath);
	
	return iRet;
}

#endif
//+-------------------------------------------------------------------------
//  For "Root": prompt before adding a cert.
//--------------------------------------------------------------------------
STATIC BOOL WINAPI RootStoreProvWriteCert(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pCertContext,
        IN DWORD dwFlags
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    PCCERT_CONTEXT pProvCertContext = NULL;
    BYTE *pbSerializedElement = NULL;
    DWORD cbSerializedElement = 0;

    assert(pRegStore);
    if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
        goto AccessDenied;
#ifndef CE_BUILD
    if (pRegStore->fProtected) {
        if (!CertSerializeCertificateStoreElement(
                pCertContext,
                0,              // dwFlags
                NULL,           // pbElement
                &cbSerializedElement
                )) goto SerializeCertError;
        if (NULL == (pbSerializedElement = (BYTE *) PkiNonzeroAlloc(
                cbSerializedElement)))
            goto OutOfMemory;
        if (!CertSerializeCertificateStoreElement(
                pCertContext,
                0,              // dwFlags
                pbSerializedElement,
                &cbSerializedElement
                )) goto SerializeCertError;

        fResult = I_CertProtectFunction(
            CERT_PROT_ADD_ROOT_FUNC_ID,
            0,                          // dwFlags
            NULL,                       // pwszIn
            pbSerializedElement,
            cbSerializedElement,
            NULL,                       // ppbOut
            NULL                        // pcbOut
            );
    } else 
#endif    
    {
        // If the certificate doesn't already exist, then, prompt the user
        if (!RegStoreProvReadCert(
                hStoreProv,
                pCertContext,
                0,              // dwFlags
                &pProvCertContext)) {
#ifdef CE_BUILD
            if (IDYES != CeRootMessageBox(pCertContext,
                    ADD_ROOT_CERT, 0))
#else
            if (IDYES != IPR_ProtectedRootMessageBox(pCertContext,
                    IDS_ROOT_MSG_BOX_ADD_ACTION, 0))
#endif                    
                goto Cancelled;
        }

        fResult = RegStoreProvWriteCert(
            hStoreProv,
            pCertContext,
            dwFlags
            );
    }

CommonReturn:
    CertFreeCertificateContext(pProvCertContext);
    PkiFree(pbSerializedElement);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(AccessDenied, E_ACCESSDENIED)
SET_ERROR(Cancelled, ERROR_CANCELLED)
#ifndef CE_BUILD
TRACE_ERROR(SerializeCertError)
TRACE_ERROR(OutOfMemory)
#endif
}


//+-------------------------------------------------------------------------
//  For "Root": prompt before deleting a cert.
//--------------------------------------------------------------------------
STATIC BOOL WINAPI RootStoreProvDeleteCert(
        IN HCERTSTOREPROV hStoreProv,
        IN PCCERT_CONTEXT pCertContext,
        IN DWORD dwFlags
        )
{
    BOOL fResult;
    PREG_STORE pRegStore = (PREG_STORE) hStoreProv;
    PCCERT_CONTEXT pProvCertContext = NULL;

    assert(pRegStore);
    if (IsInResync(pRegStore))
        // Only delete from store cache, not from registry
        return TRUE;

    if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
        goto AccessDenied;

#ifndef CE_BUILD
    if (pRegStore->fProtected) {
        BYTE    rgbHash[MAX_HASH_LEN];
        DWORD   cbHash = MAX_HASH_LEN;

        // get the thumbprint
        if(!CertGetCertificateContextProperty(
                pCertContext,
                CERT_SHA1_HASH_PROP_ID,
                rgbHash,
                &cbHash
                )) goto GetCertHashPropError;
        fResult = I_CertProtectFunction(
            CERT_PROT_DELETE_ROOT_FUNC_ID,
            0,                          // dwFlags
            NULL,                       // pwszIn
            rgbHash,
            cbHash,
            NULL,                       // ppbOut
            NULL                        // pcbOut
            );
    } else 
#endif    
    {
        // Prompt the user before deleting
        if (RegStoreProvReadCert(
                hStoreProv,
                pCertContext,
                0,              // dwFlags
                &pProvCertContext)) {
#ifdef CE_BUILD
            if (IDYES != CeRootMessageBox(pCertContext,
                    DELETE_ROOT_CERT, 0))
#else
            if (IDYES != IPR_ProtectedRootMessageBox(pCertContext,
                    IDS_ROOT_MSG_BOX_DELETE_ACTION, 0))
#endif                    
                goto Cancelled;

            fResult = RegStoreProvDeleteCert(
                hStoreProv,
                pCertContext,
                dwFlags
                );
        } else
            fResult = TRUE;
    }

CommonReturn:
    CertFreeCertificateContext(pProvCertContext);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(AccessDenied, E_ACCESSDENIED)
SET_ERROR(Cancelled, ERROR_CANCELLED)
#ifndef CE_BUILD
TRACE_ERROR(GetCertHashPropError)
#endif
}

//+=========================================================================
// Change Notify Support Functions
//==========================================================================

#if 0
typedef VOID (NTAPI * WAITORTIMERCALLBACKFUNC) (PVOID, BOOLEAN );
typedef WAITORTIMERCALLBACKFUNC WAITORTIMERCALLBACK ;

WINBASEAPI
HANDLE
WINAPI
RegisterWaitForSingleObject(
    HANDLE hObject,
    WAITORTIMERCALLBACK Callback,
    PVOID Context,
    ULONG dwMilliseconds
    );

WINBASEAPI
BOOL
WINAPI
UnregisterWait(
    HANDLE WaitHandle
    );
#endif


typedef HANDLE (WINAPI *PFN_ILS_REGISTER_WAIT_FOR_SINGLE_OBJECT_EX)(
    HANDLE hObject,
    ILS_WAITORTIMERCALLBACK Callback,
    PVOID Context,
    ULONG dwMilliseconds,
    ULONG dwFlags
    );

typedef BOOL (WINAPI *PFN_ILS_UNREGISTER_WAIT)(
    HANDLE WaitHandle
    );

#define sz_KERNEL32_DLL                 "kernel32.dll"
#define sz_RegisterWaitForSingleObjectEx "RegisterWaitForSingleObjectEx"
#define sz_UnregisterWait               "UnregisterWait"

static HMODULE hKernel32Dll = NULL;
PFN_ILS_REGISTER_WAIT_FOR_SINGLE_OBJECT_EX pfnILS_RegisterWaitForSingleObjectEx;
PFN_ILS_UNREGISTER_WAIT pfnILS_UnregisterWait;

#define ILS_REG_WAIT_EXIT_HANDLE_INDEX      0
#define ILS_REG_WAIT_OBJECT_HANDLE_INDEX    1
#define ILS_REG_WAIT_HANDLE_COUNT           2

typedef struct _ILS_REG_WAIT_INFO {
    HANDLE                  hThread;
    DWORD                   dwThreadId;
    HANDLE                  rghWait[ILS_REG_WAIT_HANDLE_COUNT];
    ILS_WAITORTIMERCALLBACK Callback;
    PVOID                   Context;
    ULONG                   dwMilliseconds;
} ILS_REG_WAIT_INFO, *PILS_REG_WAIT_INFO;


DWORD WINAPI ILS_WaitForThreadProc(
    LPVOID lpThreadParameter
    )
{
    PILS_REG_WAIT_INFO pWaitInfo = (PILS_REG_WAIT_INFO) lpThreadParameter;
    DWORD cWait;

    if (pWaitInfo->rghWait[ILS_REG_WAIT_OBJECT_HANDLE_INDEX])
        cWait = ILS_REG_WAIT_HANDLE_COUNT;
    else
        cWait = ILS_REG_WAIT_HANDLE_COUNT - 1;

    while (TRUE) {
        DWORD dwWaitObject;

        dwWaitObject = WaitForMultipleObjectsEx(
            cWait,
            pWaitInfo->rghWait,
            FALSE,      // bWaitAll
            pWaitInfo->dwMilliseconds,
            FALSE       // bAlertable
            );

        switch (dwWaitObject) {
            case WAIT_OBJECT_0 + ILS_REG_WAIT_OBJECT_HANDLE_INDEX:
                pWaitInfo->Callback(pWaitInfo->Context, TRUE);
                break;
            case WAIT_TIMEOUT:
                pWaitInfo->Callback(pWaitInfo->Context, FALSE);
                break;
            case WAIT_OBJECT_0 + ILS_REG_WAIT_EXIT_HANDLE_INDEX:
                goto CommonReturn;
                break;
            default:
                goto InvalidWaitForObject;
        }
    }

CommonReturn:
    return 0;
ErrorReturn:
    goto CommonReturn;
TRACE_ERROR(InvalidWaitForObject)
}

#ifndef CE_BUILD
HANDLE
WINAPI
ILS_RegisterWaitForSingleObjectEx(
    HANDLE hObject,
    ILS_WAITORTIMERCALLBACK Callback,
    PVOID Context,
    ULONG dwMilliseconds,
    ULONG dwFlags
    )
{
    PILS_REG_WAIT_INFO pWaitInfo = NULL;
    HANDLE hDupObject = NULL;

    if ( dwMilliseconds == 0 )
    {
        dwMilliseconds = INFINITE ;
    }

    if (NULL == (pWaitInfo = (PILS_REG_WAIT_INFO) PkiZeroAlloc(
            sizeof(ILS_REG_WAIT_INFO))))
        goto OutOfMemory;

    if (hObject) {
        if (!DuplicateHandle(
                GetCurrentProcess(),
                hObject,
                GetCurrentProcess(),
                &hDupObject,
                0,                      // dwDesiredAccess
                FALSE,                  // bInheritHandle
                DUPLICATE_SAME_ACCESS
                ) || NULL == hDupObject)
            goto DuplicateEventError;
        pWaitInfo->rghWait[ILS_REG_WAIT_OBJECT_HANDLE_INDEX] = hDupObject;
    }
    pWaitInfo->Callback = Callback;
    pWaitInfo->Context = Context;
    pWaitInfo->dwMilliseconds = dwMilliseconds;

    // Create event to be signaled to terminate the thread
    if (NULL == (pWaitInfo->rghWait[ILS_REG_WAIT_EXIT_HANDLE_INDEX] =
            CreateEvent(
                NULL,       // lpsa
                FALSE,      // fManualReset
                FALSE,      // fInitialState
                NULL)))     // lpszEventName
        goto CreateThreadExitEventError;

    // Create the thread to do the wait for
    if (NULL == (pWaitInfo->hThread = CreateThread(
            NULL,           // lpThreadAttributes
            0,              // dwStackSize
            ILS_WaitForThreadProc,
            pWaitInfo,
            0,              // dwCreationFlags
            &pWaitInfo->dwThreadId
            )))
        goto CreateThreadError;

CommonReturn:
    return (HANDLE) pWaitInfo;

ErrorReturn:
    if (pWaitInfo) {
        DWORD dwErr = GetLastError();

        for (DWORD i = 0; i < ILS_REG_WAIT_HANDLE_COUNT; i++) {
            if (pWaitInfo->rghWait[i])
                CloseHandle(pWaitInfo->rghWait[i]);
        }
        PkiFree(pWaitInfo);
        pWaitInfo = NULL;

        SetLastError(dwErr);
    }
    goto CommonReturn;

TRACE_ERROR(OutOfMemory)
TRACE_ERROR(DuplicateEventError)
TRACE_ERROR(CreateThreadExitEventError)
TRACE_ERROR(CreateThreadError)
}


BOOL
WINAPI
ILS_UnregisterWait(
    HANDLE WaitHandle
    )
{
    PILS_REG_WAIT_INFO pWaitInfo = (PILS_REG_WAIT_INFO) WaitHandle;

    // Terminate the wait for thread and wait for it to exit
    SetEvent(pWaitInfo->rghWait[ILS_REG_WAIT_EXIT_HANDLE_INDEX]);
    if (pWaitInfo->dwThreadId != GetCurrentThreadId())
        // On Win98 at ProcessDetach it might switch to one of the
        // threads we created.
        //
        // Alternatively, we may be called from the callback itself via
        // ILS_ExitWait()
        WaitForSingleObjectEx(
            pWaitInfo->hThread,
            INFINITE,
            FALSE                       // bAlertable
            );
    CloseHandle(pWaitInfo->hThread);

    for (DWORD i = 0; i < ILS_REG_WAIT_HANDLE_COUNT; i++) {
        if (pWaitInfo->rghWait[i])
            CloseHandle(pWaitInfo->rghWait[i]);
    }
    PkiFree(pWaitInfo);

    return TRUE;
}

// Called from the callback function
BOOL
WINAPI
ILS_ExitWait(
    HANDLE WaitHandle
    )
{
    ILS_UnregisterWait(WaitHandle);
    ExitThread(0);
    assert(FALSE);
    return TRUE;
}

STATIC void RegWaitForProcessAttach()
{
    if (NULL == (hKernel32Dll = LoadLibraryA(sz_KERNEL32_DLL)))
        goto LoadKernel32DllError;

    if (NULL == (pfnILS_RegisterWaitForSingleObjectEx =
            (PFN_ILS_REGISTER_WAIT_FOR_SINGLE_OBJECT_EX) GetProcAddress(
                hKernel32Dll, sz_RegisterWaitForSingleObjectEx)))
        goto GetRegisterWaitForSingleObjectProcAddressError;
    if (NULL == (pfnILS_UnregisterWait =
            (PFN_ILS_UNREGISTER_WAIT) GetProcAddress(
                hKernel32Dll, sz_UnregisterWait)))
        goto GetUnregisterWaitProcAddressError;

CommonReturn:
    return;
ErrorReturn:
    pfnILS_RegisterWaitForSingleObjectEx = ILS_RegisterWaitForSingleObjectEx;
    pfnILS_UnregisterWait = ILS_UnregisterWait;
    goto CommonReturn;

TRACE_ERROR(LoadKernel32DllError)
TRACE_ERROR(GetRegisterWaitForSingleObjectProcAddressError)
TRACE_ERROR(GetUnregisterWaitProcAddressError)
}

STATIC void RegWaitForProcessDetach()
{
    if (hKernel32Dll) {
        FreeLibrary(hKernel32Dll);
        hKernel32Dll = NULL;
    }
}


// Upon entry/exit, the resync list is locked by the caller
BOOL ILS_AddEventToResyncList(
    IN PREG_STORE pRegStore,
    IN HANDLE hEvent,
    IN OUT DWORD *pcEntry,
    IN OUT PILS_RESYNC_ENTRY *ppEntry
    )
{
    BOOL fResult;
    HANDLE hDupEvent = NULL;
    DWORD cEntry;
    PILS_RESYNC_ENTRY pEntry;
    DWORD i;

    assert(hEvent);
    cEntry = *pcEntry;
    pEntry = *ppEntry;

    // First check if the hEvent is already in the list
    for (i = 0; i < cEntry; i++) {
        if (hEvent == pEntry[i].hOrigEvent)
            return TRUE;
    }

    if (!DuplicateHandle(
            GetCurrentProcess(),
            hEvent,
            GetCurrentProcess(),
            &hDupEvent,
            0,                      // dwDesiredAccess
            FALSE,                  // bInheritHandle
            DUPLICATE_SAME_ACCESS
            ) || NULL == hDupEvent)
        goto DuplicateEventError;

    if (NULL == (pEntry = (PILS_RESYNC_ENTRY) PkiRealloc(pEntry,
            (cEntry + 1) * sizeof(ILS_RESYNC_ENTRY))))
        goto OutOfMemory;
    pEntry[cEntry].hOrigEvent = hEvent;
    pEntry[cEntry].pRegStore = pRegStore;
    pEntry[cEntry].hEvent = hDupEvent;
    *pcEntry = cEntry + 1;
    *ppEntry = pEntry;
    fResult = TRUE;

CommonReturn:
    return fResult;
ErrorReturn:
    if (hDupEvent)
        CloseHandle(hDupEvent);
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(DuplicateEventError)
TRACE_ERROR(OutOfMemory)
}

// Upon entry/exit, the resync list is locked by the caller
void ILS_SignalEventsOnResyncList(
    IN OUT DWORD *pcEntry,
    IN OUT PILS_RESYNC_ENTRY *ppEntry
    )
{
    DWORD cEntry = *pcEntry;
    PILS_RESYNC_ENTRY pEntry = *ppEntry;

    while (cEntry--) {
        HANDLE hEvent = pEntry[cEntry].hEvent;
        SetEvent(hEvent);
        CloseHandle(hEvent);
    }

    PkiFree(pEntry);

    *pcEntry = 0;
    *ppEntry = NULL;
}

// Upon entry/exit, the resync list is locked by the caller
void ILS_SignalAndFreeRegStoreResyncEntries(
    IN PREG_STORE pRegStore,
    IN OUT DWORD *pcEntry,
    IN OUT PILS_RESYNC_ENTRY *ppEntry
    )
{
    DWORD cOrigEntry = *pcEntry;
    DWORD cNewEntry = 0;
    PILS_RESYNC_ENTRY pEntry = *ppEntry;
    DWORD i;

    for (i = 0; i < cOrigEntry; i++) {
        if (pEntry[i].pRegStore == pRegStore) {
            HANDLE hEvent = pEntry[i].hEvent;
            SetEvent(hEvent);
            CloseHandle(hEvent);
        } else {
            if (i != cNewEntry)
                pEntry[cNewEntry] = pEntry[i];
            cNewEntry++;
        }
    }

    *pcEntry = cNewEntry;
}

STATIC BOOL ILS_RegNotifyChangeKeyValue(
    IN HKEY hKey,
    IN HANDLE hEvent
    )
{
    BOOL fResult;
    LONG err;

    err = RegNotifyChangeKeyValue(
        hKey,
        TRUE,                       // bWatchSubtree
        REG_NOTIFY_CHANGE_NAME |
        REG_NOTIFY_CHANGE_LAST_SET,
        hEvent,
        TRUE                        // fAsynchronus
        );
    if (!(ERROR_SUCCESS == err || ERROR_KEY_DELETED == err))
        goto RegNotifyChangeKeyValueError;

    fResult = TRUE;

CommonReturn:
    return fResult;

ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR_VAR(RegNotifyChangeKeyValueError, err)
}


//+=========================================================================
//  Client "GPT" Store Data Structures and Functions
//==========================================================================

#define CU_GPT_STORE_INDEX                  0
#define LM_GPT_STORE_INDEX                  1
#define GPT_STORE_COUNT                     2

#ifndef GROUP_POLICY_USER_EVENTA
#define GROUP_POLICY_USER_EVENTA       "UserGroupPolicyApplied"
#endif

#ifndef GROUP_POLICY_MACHINE_EVENTA
#define GROUP_POLICY_MACHINE_EVENTA    "MachineGroupPolicyApplied"
#endif

static LPCSTR rgpszGptStorePolicyEvent[GPT_STORE_COUNT] = {
    GROUP_POLICY_USER_EVENTA,
    GROUP_POLICY_MACHINE_EVENTA
};

static const HKEY rghGptPoliciesBaseKey[GPT_STORE_COUNT] = {
    HKEY_CURRENT_USER,
    HKEY_LOCAL_MACHINE
};

static BOOL fGptStoreInitialized[GPT_STORE_COUNT];
static BOOL fRegisterGPNotification[GPT_STORE_COUNT];
static HKEY rghGptPoliciesKey[GPT_STORE_COUNT];
static HANDLE rghGptStorePolicyEvent[GPT_STORE_COUNT];
static HANDLE rghGptRegWaitFor[GPT_STORE_COUNT];

static DWORD rgcGptStoreResyncEntry[GPT_STORE_COUNT];
static ILS_RESYNC_ENTRY *rgpGptStoreResyncEntry[GPT_STORE_COUNT];


typedef HANDLE (WINAPI *PFN_ENTER_CRITICAL_POLICY_SECTION)(
    IN BOOL bMachine
    );
typedef BOOL (WINAPI *PFN_LEAVE_CRITICAL_POLICY_SECTION)(
    IN HANDLE hSection
    );

typedef BOOL (WINAPI *PFN_REGISTER_GP_NOTIFICATION)(
    IN HANDLE hEvent,
    IN BOOL bMachine
    );

typedef BOOL (WINAPI *PFN_UNREGISTER_GP_NOTIFICATION)(
    IN HANDLE hEvent
    );

#define sz_USERENV_DLL                  "userenv.dll"
#define sz_EnterCriticalPolicySection   "EnterCriticalPolicySection"
#define sz_LeaveCriticalPolicySection   "LeaveCriticalPolicySection"
#define sz_RegisterGPNotification       "RegisterGPNotification"
#define sz_UnregisterGPNotification     "UnregisterGPNotification"

static fLoadedUserEnvDll = FALSE;
static HMODULE hUserEnvDll = NULL;
static PFN_ENTER_CRITICAL_POLICY_SECTION pfnEnterCriticalPolicySection = NULL;
static PFN_LEAVE_CRITICAL_POLICY_SECTION pfnLeaveCriticalPolicySection = NULL;
static PFN_REGISTER_GP_NOTIFICATION pfnRegisterGPNotification = NULL;
static PFN_UNREGISTER_GP_NOTIFICATION pfnUnregisterGPNotification = NULL;


//+-------------------------------------------------------------------------
//  Lock and unlock GPT_STORE functions
//--------------------------------------------------------------------------
static inline void GptStoreLock()
{
    EnterCriticalSection(&ILS_CriticalSection);
}
static inline void GptStoreUnlock()
{
    LeaveCriticalSection(&ILS_CriticalSection);
}

STATIC void GptLoadUserEnvDll()
{
    HMODULE hDll;
    if (fLoadedUserEnvDll)
        return;

    // Do load library without holding a lock
    hDll = LoadLibraryA(sz_USERENV_DLL);

    GptStoreLock();
    if (fLoadedUserEnvDll) {
        if (hDll)
            FreeLibrary(hDll);
        goto CommonReturn;
    }

    if (NULL == hDll)
        goto LoadUserEnvDllError;

    if (pfnEnterCriticalPolicySection =
            (PFN_ENTER_CRITICAL_POLICY_SECTION) GetProcAddress(
                hDll, sz_EnterCriticalPolicySection)) {
        if (NULL == (pfnLeaveCriticalPolicySection =
                (PFN_LEAVE_CRITICAL_POLICY_SECTION) GetProcAddress(
                    hDll, sz_LeaveCriticalPolicySection))) {
            pfnEnterCriticalPolicySection = NULL;
#if DBG
            DWORD dwErr = GetLastError();
            DbgPrintf(DBG_SS_CRYPT32,
                "userenv.dll:: GetProcAddress(%s) returned error: %d 0x%x\n",
                    sz_LeaveCriticalPolicySection, dwErr, dwErr);
#endif
        }
    } else {
#if DBG
        DWORD dwErr = GetLastError();
        DbgPrintf(DBG_SS_CRYPT32,
            "userenv.dll:: GetProcAddress(%s) returned error: %d 0x%x\n",
                sz_EnterCriticalPolicySection, dwErr, dwErr);
#endif
    }

    if (pfnRegisterGPNotification = 
        (PFN_REGISTER_GP_NOTIFICATION) GetProcAddress(
                hDll, sz_RegisterGPNotification)) {
        if (NULL == (pfnUnregisterGPNotification =
                (PFN_UNREGISTER_GP_NOTIFICATION) GetProcAddress(
                    hDll, sz_UnregisterGPNotification))) {
            pfnRegisterGPNotification = NULL; 
#if DBG
            DWORD dwErr = GetLastError();
            DbgPrintf(DBG_SS_CRYPT32,
                "userenv.dll:: GetProcAddress(%s) returned error: %d 0x%x\n",
                    sz_UnregisterGPNotification, dwErr, dwErr);
#endif
        }
    } else {
#if DBG
        DWORD dwErr = GetLastError();
        DbgPrintf(DBG_SS_CRYPT32,
            "userenv.dll:: GetProcAddress(%s) returned error: %d 0x%x\n",
                sz_RegisterGPNotification, dwErr, dwErr);
#endif
    }

    if (pfnEnterCriticalPolicySection || pfnRegisterGPNotification)
        hUserEnvDll = hDll;
    else
        FreeLibrary(hDll);

CommonReturn:
    fLoadedUserEnvDll = TRUE;
    GptStoreUnlock();

    return;
ErrorReturn:
    goto CommonReturn;
TRACE_ERROR(LoadUserEnvDllError)
}

STATIC HANDLE GptStoreEnterCriticalPolicySection(
    IN BOOL bMachine
    )
{
#if 1
    // BUGBUG entering this critical section is causing numerous hanging,
    // deadlock problems
    return NULL;
#else
    HANDLE hSection;

    GptLoadUserEnvDll();
    if (NULL == pfnEnterCriticalPolicySection)
        return NULL;

    assert(hUserEnvDll);
    assert(pfnLeaveCriticalPolicySection);
    if (NULL == (hSection = pfnEnterCriticalPolicySection(bMachine)))
        goto EnterCriticalPolicySectionError;

CommonReturn:
    return hSection;
ErrorReturn:
    goto CommonReturn;

TRACE_ERROR(EnterCriticalPolicySectionError)
#endif
}

STATIC void GptStoreLeaveCriticalPolicySection(
    IN HANDLE hSection
    )
{
    if (hSection) {
        assert(hUserEnvDll);
        assert(pfnLeaveCriticalPolicySection);
        if (!pfnLeaveCriticalPolicySection(hSection))
            goto LeaveCriticalPolicySectionError;
    }

CommonReturn:
    return;
ErrorReturn:
    goto CommonReturn;

TRACE_ERROR(LeaveCriticalPolicySectionError)
}

STATIC void GptStoreSignalAndFreeRegStoreResyncEntries(
    IN PREG_STORE pRegStore
    )
{
    DWORD dwGptStoreIndex;

    GptStoreLock();

    for (dwGptStoreIndex = 0; dwGptStoreIndex < GPT_STORE_COUNT;
                                                        dwGptStoreIndex++) {

        ILS_SignalAndFreeRegStoreResyncEntries(
            pRegStore,
            &rgcGptStoreResyncEntry[dwGptStoreIndex],
            &rgpGptStoreResyncEntry[dwGptStoreIndex]
            );
    }

    GptStoreUnlock();
}

STATIC void GptStoreProcessAttach()
{
}

STATIC void GptStoreProcessDetach()
{
    DWORD dwGptStoreIndex;
    for (dwGptStoreIndex = 0; dwGptStoreIndex < GPT_STORE_COUNT;
                                                        dwGptStoreIndex++) {
        if (!fGptStoreInitialized[dwGptStoreIndex])
            continue;

        // Unregister the wait for callback
        if (rghGptRegWaitFor[dwGptStoreIndex])
            pfnILS_UnregisterWait(rghGptRegWaitFor[dwGptStoreIndex]);

        if (fRegisterGPNotification[dwGptStoreIndex]) {
            assert(hUserEnvDll && pfnUnregisterGPNotification &&
                rghGptStorePolicyEvent[dwGptStoreIndex]);
            pfnUnregisterGPNotification(
                rghGptStorePolicyEvent[dwGptStoreIndex]);
        }

        if (rghGptStorePolicyEvent[dwGptStoreIndex])
            CloseHandle(rghGptStorePolicyEvent[dwGptStoreIndex]);

        // If no RegisterGPNotification API or
        // if we were unable to open the GPT event, then, the following
        // registry key was opened
        ILS_CloseRegistryKey(rghGptPoliciesKey[dwGptStoreIndex]);

        // To inhibit any potential deadlock, do following without entering
        // the critical section
        ILS_SignalEventsOnResyncList(
            &rgcGptStoreResyncEntry[dwGptStoreIndex],
            &rgpGptStoreResyncEntry[dwGptStoreIndex]
            );

        fGptStoreInitialized[dwGptStoreIndex] = FALSE;
    }

    if (hUserEnvDll) {
        FreeLibrary(hUserEnvDll);
        hUserEnvDll = NULL;
    }
}

STATIC VOID NTAPI GptWaitForCallback(
    PVOID Context,
    BOOLEAN fWaitOrTimedOut        // ???
    )
{
    DWORD cEntry;
    PILS_RESYNC_ENTRY pEntry;

    const DWORD dwGptStoreIndex = (DWORD) ((DWORD_PTR)Context);
    assert(dwGptStoreIndex < GPT_STORE_COUNT);

    if (rghGptPoliciesKey[dwGptStoreIndex])
        // Re-arm the registry notify
        ILS_RegNotifyChangeKeyValue(
            rghGptPoliciesKey[dwGptStoreIndex],
            rghGptStorePolicyEvent[dwGptStoreIndex]
            );

    GptStoreLock();
        cEntry = rgcGptStoreResyncEntry[dwGptStoreIndex];
        pEntry = rgpGptStoreResyncEntry[dwGptStoreIndex];

        rgcGptStoreResyncEntry[dwGptStoreIndex] = 0;
        rgpGptStoreResyncEntry[dwGptStoreIndex] = NULL;
    GptStoreUnlock();

    ILS_SignalEventsOnResyncList(
        &cEntry,
        &pEntry
        );

}

STATIC BOOL GptStoreChangeInit(
    IN DWORD dwGptStoreIndex
    )
{
    BOOL fResult;
    DWORD dwErr;
    HANDLE hRegWaitFor;
    HANDLE hTmpEvent;

    if (fGptStoreInitialized[dwGptStoreIndex])
        return TRUE;

    GptLoadUserEnvDll();

    GptStoreLock();

    if (fGptStoreInitialized[dwGptStoreIndex])
        goto SuccessReturn;

    // Create our own event to be notified on a change
    if (NULL ==
        (rghGptStorePolicyEvent[dwGptStoreIndex] =
            CreateEvent(
                NULL,       // lpsa
                FALSE,      // fManualReset
                FALSE,      // fInitialState
                NULL)))     // lpszEventName
            goto CreateEventError;

    // If the RegisterGPNotification API exists in userenv.dll, use it.
    if (pfnRegisterGPNotification) {
        if (pfnRegisterGPNotification(
                rghGptStorePolicyEvent[dwGptStoreIndex],
                dwGptStoreIndex == LM_GPT_STORE_INDEX
                )) {
            fRegisterGPNotification[dwGptStoreIndex] = TRUE;
            goto RegWaitFor;
        }
#if DBG
        else {
            DWORD dwErr = GetLastError();
            DbgPrintf(DBG_SS_CRYPT32,
                "RegisterGPNotification returned error: %d 0x%x\n",
                    dwErr, dwErr);
        }
#endif
    }
    
    // Open the GPT event to be signaled after policy is applied to the
    // registry
    if (hTmpEvent = OpenEventA(
            SYNCHRONIZE,
            FALSE,                  // bInheritHandle
            rgpszGptStorePolicyEvent[dwGptStoreIndex]
            )) {
        CloseHandle(rghGptStorePolicyEvent[dwGptStoreIndex]);
        rghGptStorePolicyEvent[dwGptStoreIndex] = hTmpEvent;
    } else {
#if DBG
        DWORD dwErr = GetLastError();
        DbgPrintf(DBG_SS_CRYPT32, "OpenEvent(%s) returned error: %d 0x%x\n",
            rgpszGptStorePolicyEvent[dwGptStoreIndex], dwErr, dwErr);
#endif

        // Open the Software\Policies\Microsoft\SystemCertificates registry key
        if (NULL == (rghGptPoliciesKey[dwGptStoreIndex] =
                OpenSubKey(
                    rghGptPoliciesBaseKey[dwGptStoreIndex],
                    GROUP_POLICY_STORE_REGPATH,
                    CERT_STORE_READONLY_FLAG
                    ))) {
            // Ignore error if subkey doesn't exist.
            if (ERROR_FILE_NOT_FOUND == GetLastError())
                goto SuccessReturn;
            goto OpenSubKeyError;
        }

        // Arm the registry notify
        if (!ILS_RegNotifyChangeKeyValue(
                rghGptPoliciesKey[dwGptStoreIndex],
                rghGptStorePolicyEvent[dwGptStoreIndex]
                ))
            goto RegNotifyChangeKeyValueError;
    }

RegWaitFor:
    hRegWaitFor = pfnILS_RegisterWaitForSingleObjectEx(
        rghGptStorePolicyEvent[dwGptStoreIndex],
        GptWaitForCallback,
        (PVOID)(DWORD_PTR) dwGptStoreIndex,
        INFINITE,  // no timeout
        (fRegisterGPNotification[dwGptStoreIndex] ||
            rghGptPoliciesKey[dwGptStoreIndex]) ? WT_EXECUTEINWAITTHREAD : 0
        );
    if (INVALID_HANDLE_VALUE == hRegWaitFor || NULL == hRegWaitFor) {
        dwErr = GetLastError();
        goto RegisterWaitForError;
    }

    rghGptRegWaitFor[dwGptStoreIndex] = hRegWaitFor;

SuccessReturn:
    fResult = TRUE;
    fGptStoreInitialized[dwGptStoreIndex] = TRUE;

CommonReturn:
    GptStoreUnlock();
    return fResult;;
ErrorReturn:
    dwErr = GetLastError();

    if (fRegisterGPNotification[dwGptStoreIndex]) {
        assert(hUserEnvDll && pfnUnregisterGPNotification &&
            rghGptStorePolicyEvent[dwGptStoreIndex]);
        pfnUnregisterGPNotification(
            rghGptStorePolicyEvent[dwGptStoreIndex]);
        fRegisterGPNotification[dwGptStoreIndex] = FALSE;
    }

    if (rghGptStorePolicyEvent[dwGptStoreIndex]) {
        CloseHandle(rghGptStorePolicyEvent[dwGptStoreIndex]);
        rghGptStorePolicyEvent[dwGptStoreIndex] = NULL;
    }

    ILS_CloseRegistryKey(rghGptPoliciesKey[dwGptStoreIndex]);
    rghGptPoliciesKey[dwGptStoreIndex] = NULL;

    SetLastError(dwErr);
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(CreateEventError)
TRACE_ERROR(OpenSubKeyError)
TRACE_ERROR(RegNotifyChangeKeyValueError)
SET_ERROR_VAR(RegisterWaitForError, dwErr)
}

STATIC BOOL RegGptStoreChange(
    IN PREG_STORE pRegStore,
    IN HANDLE hEvent
    )
{
    BOOL fResult;

    DWORD dwGptStoreIndex =
        (pRegStore->dwFlags & CERT_REGISTRY_STORE_LM_GPT_FLAG) ?
            LM_GPT_STORE_INDEX : CU_GPT_STORE_INDEX;

    if (pRegStore->dwFlags & CERT_REGISTRY_STORE_REMOTE_FLAG) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if (!GptStoreChangeInit(dwGptStoreIndex))
        return FALSE;

    GptStoreLock();
    fResult = ILS_AddEventToResyncList(
            pRegStore,
            hEvent,
            &rgcGptStoreResyncEntry[dwGptStoreIndex],
            &rgpGptStoreResyncEntry[dwGptStoreIndex]
            );
    GptStoreUnlock();
    return fResult;
}

STATIC BOOL OpenAllFromGptRegistry(
    IN PREG_STORE pRegStore,
    IN HCERTSTORE hCertStore
    )
{
    BOOL fResult;
    HANDLE hSection = NULL;

    LockRegStore(pRegStore);

    if (0 == (pRegStore->dwFlags & CERT_REGISTRY_STORE_REMOTE_FLAG))
        hSection = GptStoreEnterCriticalPolicySection(
            pRegStore->dwFlags & CERT_REGISTRY_STORE_LM_GPT_FLAG
            );

    assert(NULL == pRegStore->hKey);
    if (NULL == (pRegStore->hKey = OpenSubKey(
            pRegStore->GptPara.hKeyBase,
            pRegStore->GptPara.pwszRegPath,
            pRegStore->dwFlags
            ))) {
        if (ERROR_FILE_NOT_FOUND != GetLastError() ||
                (pRegStore->dwFlags & CERT_STORE_OPEN_EXISTING_FLAG))
            goto OpenSubKeyError;
        fResult = TRUE;
        goto CommonReturn;
    }

//    fResult = OpenAllFromSerializedRegistry(pRegStore, hCertStore);

    // Ignore any errors
    OpenAllFromRegistry(pRegStore, hCertStore);
    fResult = TRUE;

CommonReturn:
    ILS_CloseRegistryKey(pRegStore->hKey);
    pRegStore->hKey = NULL;
    GptStoreLeaveCriticalPolicySection(hSection);
    UnlockRegStore(pRegStore);
    return fResult;

ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(OpenSubKeyError)
}

STATIC BOOL CommitAllToGptRegistry(
    IN PREG_STORE pRegStore,
    IN DWORD dwFlags
    )
{
#if 1
    return TRUE;
#else
    BOOL fResult;
    BOOL fTouched;
    DWORD dwSaveFlags;

    LockRegStore(pRegStore);

    if (dwFlags & CERT_STORE_CTRL_COMMIT_FORCE_FLAG)
        fTouched = TRUE;
    else if (dwFlags & CERT_STORE_CTRL_COMMIT_CLEAR_FLAG)
        fTouched = FALSE;
    else
        fTouched = pRegStore->fTouched;

    if (fTouched) {
        if (pRegStore->dwFlags & CERT_STORE_READONLY_FLAG)
            goto AccessDenied;
    } else {
        pRegStore->fTouched = FALSE;
        fResult = TRUE;
        goto CommonReturn;
    }

    assert(NULL == pRegStore->hKey);
    if (NULL == (pRegStore->hKey = OpenSubKey(
            pRegStore->GptPara.hKeyBase,
            pRegStore->GptPara.pwszRegPath,
            pRegStore->dwFlags
            )))
        goto OpenSubKeyError;

    dwSaveFlags = pRegStore->dwFlags;
    pRegStore->dwFlags &= ~CERT_STORE_OPEN_EXISTING_FLAG;
    fResult = CommitAllToSerializedRegistry(pRegStore, dwFlags);
    pRegStore->dwFlags = dwSaveFlags;
CommonReturn:
    ILS_CloseRegistryKey(pRegStore->hKey);
    pRegStore->hKey = NULL;
    UnlockRegStore(pRegStore);
    return fResult;

ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(AccessDenied, E_ACCESSDENIED)
TRACE_ERROR(OpenSubKeyError)
#endif
}

#endif

#ifndef CE_BUILD // GEORGEJ - could add this back later, if needed 
//+=========================================================================
//  Win95 Notify Store Data Structures and Functions
//
//  Win95/Win98 don't support registry change notification.
//
//  On Win95 we pulse an event each time a store context element is written
//  or deleted.
//==========================================================================
static BOOL fWin95StoreInitialized;
static HANDLE hWin95RegWaitFor;

static DWORD cWin95StoreResyncEntry;
static ILS_RESYNC_ENTRY *pWin95StoreResyncEntry;


//+-------------------------------------------------------------------------
//  Lock and unlock WIN95_STORE functions
//--------------------------------------------------------------------------
static inline void Win95StoreLock()
{
    EnterCriticalSection(&ILS_CriticalSection);
}
static inline void Win95StoreUnlock()
{
    LeaveCriticalSection(&ILS_CriticalSection);
}

STATIC void Win95StoreSignalAndFreeRegStoreResyncEntries(
    IN PREG_STORE pRegStore
    )
{
    if (NULL == hWin95NotifyEvent)
        return;

    Win95StoreLock();

    ILS_SignalAndFreeRegStoreResyncEntries(
        pRegStore,
        &cWin95StoreResyncEntry,
        &pWin95StoreResyncEntry
        );

    Win95StoreUnlock();
}

STATIC void Win95StoreProcessAttach()
{
    HKEY hKey = NULL;
    HANDLE hEvent = NULL;

    if (FIsWinNT())
        return;

    // Check if Win98. Win98 supports Registry Change Notify
    if (NULL == (hKey = OpenSubKey(
            HKEY_CURRENT_USER,
            L"Software",
            CERT_STORE_READONLY_FLAG
            )))
        goto Win95StoreProcessAttachOpenSubKeyError;

    if (NULL == (hEvent = CreateEvent(
            NULL,       // lpsa
            FALSE,      // fManualReset
            FALSE,      // fInitialState
            NULL)))     // lpszEventName
        goto Win95StoreProcessAttachCreateEventError;

    if (ILS_RegNotifyChangeKeyValue(
            hKey,
            hEvent
            )) {
#if DBG
        DbgPrintf(DBG_SS_CRYPT32,
            "Win95Store:: RegNotifyChangeKeyValue Succeeded\n");
#endif
        goto CommonReturn;
    }


#if DBG
    DbgPrintf(DBG_SS_CRYPT32,
        "Win95Store:: RegNotifyChangeKeyValue Failed\n");
#endif

    hWin95NotifyEvent = CreateEventA(
            NULL,           // lpsa
            TRUE,           // fManualReset
            FALSE,          // fInitialState
            "Win95CertStoreNotifyEvent"
            );
    if (NULL == hWin95NotifyEvent)
        goto CreateWin95NotifyEventError;

CommonReturn:
    ILS_CloseRegistryKey(hKey);
    if (hEvent)
        CloseHandle(hEvent);
    return;
ErrorReturn:
    goto CommonReturn;
TRACE_ERROR(Win95StoreProcessAttachOpenSubKeyError)
TRACE_ERROR(Win95StoreProcessAttachCreateEventError)
TRACE_ERROR(CreateWin95NotifyEventError)
}

STATIC void Win95StoreProcessDetach()
{
    if (NULL == hWin95NotifyEvent)
        return;

    if (fWin95StoreInitialized) {
        // Unregister the wait for callback
        assert(hWin95RegWaitFor);
        pfnILS_UnregisterWait(hWin95RegWaitFor);

        // To inhibit any potential deadlock, do following without entering
        // the critical section
        ILS_SignalEventsOnResyncList(
            &cWin95StoreResyncEntry,
            &pWin95StoreResyncEntry
            );

        fWin95StoreInitialized = FALSE;
    }

    CloseHandle(hWin95NotifyEvent);
}

STATIC VOID NTAPI Win95WaitForCallback(
    PVOID Context,
    BOOLEAN fWaitOrTimedOut        // ???
    )
{
    DWORD cEntry;
    PILS_RESYNC_ENTRY pEntry;

    Win95StoreLock();
        cEntry = cWin95StoreResyncEntry;
        pEntry = pWin95StoreResyncEntry;

        cWin95StoreResyncEntry = 0;
        pWin95StoreResyncEntry = NULL;
    Win95StoreUnlock();

    ILS_SignalEventsOnResyncList(
        &cEntry,
        &pEntry
        );
}

STATIC BOOL Win95StoreChangeInit()
{
    BOOL fResult;
    DWORD dwErr;
    HANDLE hRegWaitFor;

    if (fWin95StoreInitialized)
        return TRUE;

    Win95StoreLock();

    if (fWin95StoreInitialized)
        goto SuccessReturn;

    assert(hWin95NotifyEvent);
    hRegWaitFor = pfnILS_RegisterWaitForSingleObjectEx(
        hWin95NotifyEvent,
        Win95WaitForCallback,
        NULL,                   // Context
        INFINITE,               // no timeout
        0                       // no flags (normal)
        );
    if (INVALID_HANDLE_VALUE == hRegWaitFor || NULL == hRegWaitFor) {
        dwErr = GetLastError();
        goto RegisterWaitForError;
    }

    hWin95RegWaitFor = hRegWaitFor;

SuccessReturn:
    fResult = TRUE;
    fWin95StoreInitialized = TRUE;

CommonReturn:
    Win95StoreUnlock();
    return fResult;;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR_VAR(RegisterWaitForError, dwErr)
}

STATIC BOOL RegWin95StoreChange(
    IN PREG_STORE pRegStore,
    IN HANDLE hEvent
    )
{
    BOOL fResult;

    assert(hWin95NotifyEvent);

    if (!Win95StoreChangeInit())
        return FALSE;

    Win95StoreLock();
    fResult = ILS_AddEventToResyncList(
            pRegStore,
            hEvent,
            &cWin95StoreResyncEntry,
            &pWin95StoreResyncEntry
            );
    Win95StoreUnlock();
    return fResult;
}

#endif //CE_BUILD

//+=========================================================================
// Roaming Store Functions
//==========================================================================

#if 0
SHSTDAPI SHGetFolderPathW(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR lpszPath);
#endif

typedef HRESULT (STDAPICALLTYPE *PFN_GET_FOLDER_PATH) (
    HWND hwnd,
    int csidl,
    HANDLE hToken,
    DWORD dwFlags,
    LPWSTR lpszPath
    );

#define sz_SHELL32_DLL              "shell32.dll"
#define sz_GetFolderPath            "SHGetFolderPathW"

static fLoadedShell32Dll = FALSE;
static HMODULE hShell32Dll = NULL;
static PFN_GET_FOLDER_PATH pfnGetFolderPath = NULL;

static inline void RoamingStoreLock()
{
    EnterCriticalSection(&ILS_CriticalSection);
}
static inline void RoamingStoreUnlock()
{
    LeaveCriticalSection(&ILS_CriticalSection);
}

STATIC void RoamingStoreProcessAttach()
{
}

STATIC void RoamingStoreProcessDetach()
{
    if (hShell32Dll) {
        FreeLibrary(hShell32Dll);
        hShell32Dll = NULL;
    }
}

#ifndef CE_BUILD

STATIC void RoamingStoreLoadShell32Dll()
{
    if (fLoadedShell32Dll)
        return;

    RoamingStoreLock();
    if (fLoadedShell32Dll)
        goto CommonReturn;

    if (NULL == (hShell32Dll = LoadLibraryA(sz_SHELL32_DLL)))
        goto LoadShell32DllError;

    if (NULL == (pfnGetFolderPath =
            (PFN_GET_FOLDER_PATH) GetProcAddress(hShell32Dll,
                sz_GetFolderPath)))
        goto GetFolderPathProcAddressError;

CommonReturn:
    fLoadedShell32Dll = TRUE;
    RoamingStoreUnlock();
    return;

ErrorReturn:
    if (hShell32Dll) {
        FreeLibrary(hShell32Dll);
        hShell32Dll = NULL;
        pfnGetFolderPath = NULL;
    }
    goto CommonReturn;
TRACE_ERROR(LoadShell32DllError)
TRACE_ERROR(GetFolderPathProcAddressError)
}

STATIC HANDLE GetRoamingToken()
{
    HANDLE hToken = NULL;
    DWORD dwErr;

    if (!FIsWinNT()) {
        return NULL;
    }

    //
    // first, attempt to look at the thread token.  If none exists,
    // which is true if the thread is not impersonating, try the
    // process token.
    //

    if (!OpenThreadToken(
                GetCurrentThread(),
                TOKEN_QUERY | TOKEN_IMPERSONATE,
                TRUE,
                &hToken
                )) {
        dwErr = GetLastError();
        if (ERROR_NO_TOKEN != dwErr)
            goto OpenThreadTokenError;

        if (!OpenProcessToken(GetCurrentProcess(),
                TOKEN_QUERY | TOKEN_IMPERSONATE | TOKEN_DUPLICATE, &hToken)) {
            dwErr = GetLastError();
            goto OpenProcessTokenError;
        }
    }

CommonReturn:
    return hToken;
ErrorReturn:
    hToken = NULL;
    goto CommonReturn;
SET_ERROR_VAR(OpenThreadTokenError, dwErr)
SET_ERROR_VAR(OpenProcessTokenError, dwErr)
}

#endif

LPWSTR
ILS_GetRoamingStoreDirectory(
    IN LPCWSTR pwszStoreName
    )
{
    HANDLE hToken = NULL;
    LPWSTR pwszDir = NULL;
    WCHAR wszFolderPath[MAX_PATH];
    LPCWSTR rgpwszName[] = { wszFolderPath, pwszStoreName };
    SYSTEM_NAME_GROUP NameGroup;

// BUGBUG:: Remove following goto once the following APIs work during
// impersonation.
#if 0
    goto ErrorReturn;
#endif

#ifdef CE_BUILD
    wcscpy(wszFolderPath, L"\\Roaming");
#else
    HRESULT hr;
    RoamingStoreLoadShell32Dll();
    if (NULL == hShell32Dll)
        goto ErrorReturn;
    assert(pfnGetFolderPath);

    hToken = GetRoamingToken();
    wszFolderPath[0] = L'\0';
    hr = pfnGetFolderPath(
            NULL,                   // hwndOwner
            CSIDL_APPDATA | CSIDL_FLAG_CREATE,
            hToken,
            0,                      // dwFlags
            wszFolderPath
            );
    if (S_OK != hr || L'\0' == wszFolderPath[0])
        goto GetFolderPathError;
#endif

#if DBG
        DbgPrintf(DBG_SS_CRYPT32, "SHFolderPath(CSIDL_APPDATA):: %S\n",
            wszFolderPath);
#endif

    NameGroup.cName = sizeof(rgpwszName) / sizeof(rgpwszName[0]);
    NameGroup.rgpwszName = rgpwszName;
    if (NULL == (pwszDir = FormatSystemNamePath(1, &NameGroup)))
        goto FormatSystemNamePathError;

CommonReturn:
    if (hToken)
        CloseHandle(hToken);
    return pwszDir;
ErrorReturn:
    pwszDir = NULL;
    goto CommonReturn;

#ifndef CE_BUILD
SET_ERROR_VAR(GetFolderPathError, hr)
#endif
TRACE_ERROR(FormatSystemNamePathError)
}

STATIC BOOL RecursiveCreateDirectory(
    IN LPCWSTR pwszDir
    )
{
    BOOL fResult;

    DWORD dwAttr;
    DWORD dwErr;
    LPCWSTR pwsz;
    DWORD cch;
    WCHAR wch;
    LPWSTR pwszParent = NULL;

    dwAttr = GetFileAttributesU(pwszDir);

    if (0xFFFFFFFF != dwAttr) {
        if (FILE_ATTRIBUTE_DIRECTORY & dwAttr)
            return TRUE;
        goto InvalidDirectoryAttr;
    }

    dwErr = GetLastError();
    if (!(ERROR_PATH_NOT_FOUND == dwErr || ERROR_FILE_NOT_FOUND == dwErr))
        goto GetFileAttrError;

    if (CreateDirectoryU(
            pwszDir,
            NULL                // lpSecurityAttributes
            )) {
        SetFileAttributesU(pwszDir, FILE_ATTRIBUTE_SYSTEM);
        return TRUE;
    }

    dwErr = GetLastError();
    if (!(ERROR_PATH_NOT_FOUND == dwErr || ERROR_FILE_NOT_FOUND == dwErr))
        goto CreateDirectoryError;

    // Peal off the last path name component
    cch = wcslen(pwszDir);
    pwsz = pwszDir + cch;

    while (L'\\' != *pwsz) {
        if (pwsz == pwszDir)
            // Path didn't have a \.
            goto BadDirectoryPath;
        pwsz--;
    }

    cch = (DWORD)(pwsz - pwszDir);
    if (0 == cch)
        // Detected leading \Path
        goto BadDirectoryPath;


    // Check for leading \\ or x:\.
    wch = *(pwsz - 1);
    if ((1 == cch && L'\\' == wch) || (2 == cch && L':' == wch))
        goto BadDirectoryPath;

    if (NULL == (pwszParent = (LPWSTR) PkiNonzeroAlloc((cch + 1) *
            sizeof(WCHAR))))
        goto OutOfMemory;
    memcpy(pwszParent, pwszDir, cch * sizeof(WCHAR));
    pwszParent[cch] = L'\0';

    if (!RecursiveCreateDirectory(pwszParent))
        goto ErrorReturn;
    if (!CreateDirectoryU(
            pwszDir,
            NULL                // lpSecurityAttributes
            )) {
        dwErr = GetLastError();
        goto CreateDirectory2Error;
    }
    SetFileAttributesU(pwszDir, FILE_ATTRIBUTE_SYSTEM);

    fResult = TRUE;
CommonReturn:
    PkiFree(pwszParent);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(InvalidDirectoryAttr, ERROR_FILE_INVALID)
SET_ERROR_VAR(GetFileAttrError, dwErr)
SET_ERROR_VAR(CreateDirectoryError, dwErr)
SET_ERROR(BadDirectoryPath, ERROR_BAD_PATHNAME)
TRACE_ERROR(OutOfMemory)
SET_ERROR_VAR(CreateDirectory2Error, dwErr)
}

static DWORD rgdwCreateFileRetryMilliseconds[] =
    { 1, 10, 100, 500, 1000, 5000 };

#define MAX_CREATE_FILE_RETRY_COUNT     \
            (sizeof(rgdwCreateFileRetryMilliseconds) / \
                sizeof(rgdwCreateFileRetryMilliseconds[0]))

BOOL
ILS_ReadElementFromFile(
    IN LPCWSTR pwszStoreDir,
    IN LPCWSTR pwszContextName,
    IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
    IN DWORD dwFlags,
    OUT BYTE **ppbElement,
    OUT DWORD *pcbElement
    )
{
    BOOL fResult;
    DWORD dwErr;
    LPWSTR pwszFilename = NULL;
    LPCWSTR rgpwszName[] = { pwszStoreDir, pwszContextName, wszHashName };
    SYSTEM_NAME_GROUP NameGroup;

    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD cbBytesRead;
    BYTE *pbElement = NULL;
    DWORD cbElement;
    DWORD dwRetryCount;

    NameGroup.cName = sizeof(rgpwszName) / sizeof(rgpwszName[0]);
    NameGroup.rgpwszName = rgpwszName;
    if (NULL == (pwszFilename = FormatSystemNamePath(1, &NameGroup)))
        goto FormatSystemNamePathError;

    dwRetryCount = 0;
    while (INVALID_HANDLE_VALUE == (hFile = CreateFileU(
              pwszFilename,
              GENERIC_READ,
              FILE_SHARE_READ,
              NULL,                   // lpsa
              OPEN_EXISTING,
              FILE_ATTRIBUTE_NORMAL,
              NULL                    // hTemplateFile
              ))) {
        dwErr = GetLastError();
        if ((ERROR_SHARING_VIOLATION == dwErr ||
                ERROR_ACCESS_DENIED == dwErr) &&
                MAX_CREATE_FILE_RETRY_COUNT > dwRetryCount) {
            Sleep(rgdwCreateFileRetryMilliseconds[dwRetryCount]);
            dwRetryCount++;
        } else {
            if (ERROR_PATH_NOT_FOUND == dwErr)
                dwErr = ERROR_FILE_NOT_FOUND;
            goto CreateFileError;
        }
    }

    cbElement = GetFileSize(hFile, NULL);
    if (0xFFFFFFFF == cbElement) goto FileSizeError;
    if (0 == cbElement) goto EmptyFile;
    if (NULL == (pbElement = (BYTE *) PkiNonzeroAlloc(cbElement)))
        goto OutOfMemory;
    if (!ReadFile(
            hFile,
            pbElement,
            cbElement,
            &cbBytesRead,
            NULL            // lpOverlapped
            )) {
        dwErr = GetLastError();
        goto FileError;
    }

    fResult = TRUE;
CommonReturn:
    PkiFree(pwszFilename);
    if (INVALID_HANDLE_VALUE != hFile)
        CloseHandle(hFile);
    *ppbElement = pbElement;
    *pcbElement = cbElement;
    return fResult;
ErrorReturn:
    fResult = FALSE;
    PkiFree(pbElement);
    pbElement = NULL;
    cbElement = 0;
    goto CommonReturn;

TRACE_ERROR(FormatSystemNamePathError)
TRACE_ERROR(OutOfMemory)
SET_ERROR_VAR(CreateFileError, dwErr)
TRACE_ERROR(FileSizeError)
SET_ERROR_VAR(FileError, dwErr)
SET_ERROR(EmptyFile, CRYPT_E_FILE_ERROR)
}

BOOL
ILS_WriteElementToFile(
    IN LPCWSTR pwszStoreDir,
    IN LPCWSTR pwszContextName,
    IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
    IN DWORD dwFlags,       // CERT_STORE_CREATE_NEW_FLAG may be set
    IN const BYTE *pbElement,
    IN DWORD cbElement
    )
{
    BOOL fResult;
    DWORD dwErr;
    LPWSTR pwszDir = NULL;
    LPWSTR pwszFilename = NULL;
    LPCWSTR rgpwszName[] = { pwszStoreDir, pwszContextName, wszHashName };
    SYSTEM_NAME_GROUP NameGroup;

    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD cbBytesWritten;
    DWORD dwRetryCount;

    NameGroup.cName = sizeof(rgpwszName) / sizeof(rgpwszName[0]);
    NameGroup.rgpwszName = rgpwszName;
    if (NULL == (pwszFilename = FormatSystemNamePath(1, &NameGroup)))
        goto FormatSystemNamePathError;
    NameGroup.cName--;
    if (NULL == (pwszDir = FormatSystemNamePath(1, &NameGroup)))
        goto FormatSystemNamePathError;

    if (!RecursiveCreateDirectory(pwszDir))
        goto CreateDirError;

    dwRetryCount = 0;
    while (INVALID_HANDLE_VALUE == (hFile = CreateFileU(
            pwszFilename,
            GENERIC_WRITE,
            0,                        // fdwShareMode
            NULL,                     // lpsa
            (dwFlags & CERT_STORE_CREATE_NEW_FLAG) ?
                CREATE_NEW : CREATE_ALWAYS,
            FILE_ATTRIBUTE_SYSTEM,
            NULL                      // hTemplateFile
            ))) {
        dwErr = GetLastError();
        if ((ERROR_SHARING_VIOLATION == dwErr ||
                ERROR_ACCESS_DENIED == dwErr) &&
                MAX_CREATE_FILE_RETRY_COUNT > dwRetryCount) {
            Sleep(rgdwCreateFileRetryMilliseconds[dwRetryCount]);
            dwRetryCount++;
        } else
            goto CreateFileError;
    }

    if (!WriteFile(
            hFile,
            pbElement,
            cbElement,
            &cbBytesWritten,
            NULL            // lpOverlapped
            )) {
        dwErr = GetLastError();
        goto WriteFileError;
    }

    fResult = TRUE;
CommonReturn:
    if (INVALID_HANDLE_VALUE != hFile)
        CloseHandle(hFile);
    PkiFree(pwszFilename);
    PkiFree(pwszDir);

    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(FormatSystemNamePathError)
TRACE_ERROR(CreateDirError)
SET_ERROR_VAR(CreateFileError, dwErr)
SET_ERROR_VAR(WriteFileError, dwErr)
}

BOOL
ILS_DeleteElementFromDirectory(
    IN LPCWSTR pwszStoreDir,
    IN LPCWSTR pwszContextName,
    IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
    IN DWORD dwFlags
    )
{
    BOOL fResult;
    DWORD dwErr;
    LPWSTR pwszFilename = NULL;
    LPCWSTR rgpwszName[] = { pwszStoreDir, pwszContextName, wszHashName };
    SYSTEM_NAME_GROUP NameGroup;
    DWORD dwRetryCount;

    NameGroup.cName = sizeof(rgpwszName) / sizeof(rgpwszName[0]);
    NameGroup.rgpwszName = rgpwszName;
    if (NULL == (pwszFilename = FormatSystemNamePath(1, &NameGroup)))
        goto FormatSystemNamePathError;

    dwRetryCount = 0;
    while (!DeleteFileU(pwszFilename)) {
        dwErr = GetLastError();
        if ((ERROR_SHARING_VIOLATION == dwErr ||
                ERROR_ACCESS_DENIED == dwErr) &&
                MAX_CREATE_FILE_RETRY_COUNT > dwRetryCount) {
            Sleep(rgdwCreateFileRetryMilliseconds[dwRetryCount]);
            dwRetryCount++;
        } else
            goto DeleteFileError;
    }

    fResult = TRUE;

CommonReturn:
    PkiFree(pwszFilename);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(FormatSystemNamePathError)
SET_ERROR_VAR(DeleteFileError, dwErr)
}

#ifndef CE_BUILD
STATIC HANDLE FindFirstFileU(
    LPCWSTR pwszDir,
    LPWIN32_FIND_DATAA lpFindFileData
    )
{
    HANDLE hFindFile;
    BYTE rgb[_MAX_PATH];
    LPSTR pszDir;

    if (!MkMBStr(rgb, _MAX_PATH, pwszDir, &pszDir))
        return INVALID_HANDLE_VALUE;

    hFindFile = FindFirstFileA(pszDir, lpFindFileData);
    FreeMBStr(rgb, pszDir);
    return hFindFile;
}
#else // CE_BUILD
STATIC HANDLE FindFirstFileU(
    LPCWSTR pwszDir,
    LPWIN32_FIND_DATAW lpFindFileData
    )
{
    HANDLE hFindFile;
	hFindFile = FindFirstFileW(pwszDir, lpFindFileData);
	if (hFindFile==INVALID_HANDLE_VALUE && GetLastError() == ERROR_NO_MORE_FILES )
	{	// Fix the error code on CE
		SetLastError(ERROR_FILE_NOT_FOUND);
	}
    return hFindFile;
}
#endif


BOOL
ILS_OpenAllElementsFromDirectory(
    IN LPCWSTR pwszStoreDir,
    IN LPCWSTR pwszContextName,
    IN DWORD dwFlags,
    IN void *pvArg,
    IN PFN_ILS_OPEN_ELEMENT pfnOpenElement
    )
{
    BOOL fResult;
    DWORD dwErr;
    LPWSTR pwszDir = NULL;
    LPCWSTR rgpwszName[] = { pwszStoreDir, pwszContextName, L"*" };
    SYSTEM_NAME_GROUP NameGroup;

    HANDLE hFindFile = INVALID_HANDLE_VALUE;
#ifndef CE_BUILD    
    WIN32_FIND_DATAA FindFileData;
#else
	WIN32_FIND_DATAW FindFileData;
#endif	

    NameGroup.cName = sizeof(rgpwszName) / sizeof(rgpwszName[0]);
    NameGroup.rgpwszName = rgpwszName;
    if (NULL == (pwszDir = FormatSystemNamePath(1, &NameGroup)))
        goto FormatSystemNamePathError;

    if (INVALID_HANDLE_VALUE == (hFindFile = FindFirstFileU(
            pwszDir,
            &FindFileData
            ))) {
        dwErr = GetLastError();
        if (!(ERROR_PATH_NOT_FOUND == dwErr || ERROR_FILE_NOT_FOUND == dwErr))
            goto FindFirstFileError;

        if (dwFlags & CERT_STORE_READONLY_FLAG)
            goto FindFirstFileError;

        // Attempt to create the directory. Need to remove trailing L"*".
        PkiFree(pwszDir);
        NameGroup.cName--;
        if (NULL == (pwszDir = FormatSystemNamePath(1, &NameGroup)))
            goto FormatSystemNamePathError;
        if (!RecursiveCreateDirectory(pwszDir))
            goto CreateDirError;

        goto SuccessReturn;
    }

    while (TRUE) {
        if (0 == (FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes) &&
                0 == FindFileData.nFileSizeHigh &&
                0 != FindFileData.nFileSizeLow) {
#ifndef CE_BUILD
            WCHAR wszHashName[MAX_HASH_NAME_LEN];
            DWORD cchFilename;

            cchFilename = strlen(FindFileData.cFileName);
            if (0 != cchFilename && MAX_HASH_NAME_LEN > cchFilename &&
                    0 < MultiByteToWideChar(
                        CP_ACP,
                        0,                      // dwFlags
                        FindFileData.cFileName,
                        cchFilename + 1,
                        wszHashName,
                        MAX_HASH_NAME_LEN
                        )) 
#else
			WCHAR *wszHashName = FindFileData.cFileName;
			if (*wszHashName)
#endif			
            {
                BYTE *pbElement;
                DWORD cbElement;

                if (ILS_ReadElementFromFile(
                        pwszStoreDir,
                        pwszContextName,
                        wszHashName,
                        0,                      // dwFlags
                        &pbElement,
                        &cbElement
                        )) {
                    fResult = pfnOpenElement(
                        wszHashName,
                        pbElement,
                        cbElement,
                        pvArg
                        );

                    PkiFree(pbElement);
                    if (!fResult)
                        goto CommonReturn;
                }
            }
        }


#ifndef CE_BUILD
        if (!FindNextFileA(hFindFile, &FindFileData)) 
#else
        if (!FindNextFileW(hFindFile, &FindFileData)) 
#endif	
        {
            dwErr = GetLastError();
            if (ERROR_NO_MORE_FILES == dwErr)
                goto SuccessReturn;
            else
                goto FindNextFileError;
        }
    }

SuccessReturn:
    fResult = TRUE;

CommonReturn:
    PkiFree(pwszDir);
    if (INVALID_HANDLE_VALUE != hFindFile)
        FindClose(hFindFile);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(FormatSystemNamePathError)
SET_ERROR_VAR(FindFirstFileError, dwErr)
TRACE_ERROR(CreateDirError)
SET_ERROR_VAR(FindNextFileError, dwErr)
}

#ifndef CE_BUILD
//+=========================================================================
// Registry or Roaming Store Change Notify Functions
//==========================================================================

HANDLE
WINAPI
FindFirstChangeNotificationU(
    LPCWSTR pwszPath,
    BOOL bWatchSubtree,
    DWORD dwNotifyFilter
    )
{
    if (FIsWinNT())
        return FindFirstChangeNotificationW(
            pwszPath, bWatchSubtree, dwNotifyFilter);

    HANDLE hChange;
    BYTE rgb[_MAX_PATH];
    LPSTR pszPath;

    if (!MkMBStr(rgb, _MAX_PATH, pwszPath, &pszPath))
        return INVALID_HANDLE_VALUE;

    hChange = FindFirstChangeNotificationA(pszPath, bWatchSubtree,
        dwNotifyFilter);
    FreeMBStr(rgb, pszPath);
    return hChange;
}

STATIC VOID NTAPI RegistryStoreChangeCallback(
    PVOID Context,
    BOOLEAN fWaitOrTimedOut        // ???
    )
{
    BOOL fRearm;
    DWORD dwErr;
    PREG_STORE pRegStore = (PREG_STORE) Context;
    PREGISTRY_STORE_CHANGE_INFO pInfo;

    DWORD cEntry;
    PILS_RESYNC_ENTRY pEntry;

    pInfo = pRegStore->pRegistryStoreChangeInfo;
    assert(pInfo);
    if (NULL == pInfo)
        return;

    if (pRegStore->dwFlags & CERT_REGISTRY_STORE_ROAMING_FLAG)
        fRearm = FindNextChangeNotification(pInfo->hChange);
    else
        fRearm = ILS_RegNotifyChangeKeyValue(pRegStore->hKey, pInfo->hChange);
    if (!fRearm)
        dwErr = GetLastError();

    // Minimize window of potential deadlock by only getting the values
    // while holding the lock.
    LockRegStore(pRegStore);
    cEntry = pInfo->cNotifyEntry;
    pEntry = pInfo->rgNotifyEntry;

    pInfo->cNotifyEntry = 0;
    pInfo->rgNotifyEntry = NULL;
    UnlockRegStore(pRegStore);

    ILS_SignalEventsOnResyncList(&cEntry, &pEntry);

    if (!fRearm)
        goto RegistryStoreChangeRearmError;
CommonReturn:
    return;
ErrorReturn:
    goto CommonReturn;
SET_ERROR_VAR(RegistryStoreChangeRearmError, dwErr)
}

// Upon entry/exit the pRegStore is locked
STATIC BOOL InitRegistryStoreChange(
    IN PREG_STORE pRegStore
    )
{
    BOOL fResult;
    DWORD dwErr;
    BOOL fRoaming;
    PREGISTRY_STORE_CHANGE_INFO pInfo = NULL;
    HANDLE hChange = INVALID_HANDLE_VALUE;
    HANDLE hRegWaitFor;

    fRoaming = (pRegStore->dwFlags & CERT_REGISTRY_STORE_ROAMING_FLAG);

    assert(NULL == pRegStore->pRegistryStoreChangeInfo);
    if (NULL == (pInfo = (PREGISTRY_STORE_CHANGE_INFO) PkiZeroAlloc(
            sizeof(REGISTRY_STORE_CHANGE_INFO))))
        goto OutOfMemory;

    if (fRoaming) {
        if (INVALID_HANDLE_VALUE == (hChange = FindFirstChangeNotificationU(
                pRegStore->pwszStoreDirectory,
                TRUE,                           // bWatchSubtree
                FILE_NOTIFY_CHANGE_FILE_NAME |
                    FILE_NOTIFY_CHANGE_DIR_NAME |
                    FILE_NOTIFY_CHANGE_SIZE |
                    FILE_NOTIFY_CHANGE_LAST_WRITE
                ))) {
            dwErr = GetLastError();
            goto FindFirstChangeNotificationError;
        }
    } else {
        if (NULL == (hChange = CreateEvent(
                NULL,       // lpsa
                FALSE,      // fManualReset
                FALSE,      // fInitialState
                NULL)))     // lpszEventName
            goto CreateEventError;
        assert(pRegStore->hKey);
        if (!ILS_RegNotifyChangeKeyValue(pRegStore->hKey, hChange))
            goto RegNotifyChangeKeyValueError;
    }
    pInfo->hChange = hChange;

    // The following must be set before the following register.
    // The thread may be scheduled to run before the function returns.
    pRegStore->pRegistryStoreChangeInfo = pInfo;

    hRegWaitFor = pfnILS_RegisterWaitForSingleObjectEx(
        hChange,
        RegistryStoreChangeCallback,
        (PVOID) pRegStore,
        INFINITE,                                      // no timeout
        WT_EXECUTEINWAITTHREAD
        );
    if (INVALID_HANDLE_VALUE == hRegWaitFor || NULL == hRegWaitFor) {
        pRegStore->pRegistryStoreChangeInfo = NULL;
        dwErr = GetLastError();
        goto RegisterWaitForError;
    }

    pInfo->hRegWaitFor = hRegWaitFor;
    fResult = TRUE;

CommonReturn:
    return fResult;
ErrorReturn:
    if (INVALID_HANDLE_VALUE != hChange && hChange) {
        if (fRoaming)
            FindCloseChangeNotification(hChange);
        else
            CloseHandle(hChange);
    }
    PkiFree(pInfo);
    fResult = FALSE;
    goto CommonReturn;
TRACE_ERROR(OutOfMemory)
SET_ERROR_VAR(FindFirstChangeNotificationError, dwErr)
TRACE_ERROR(CreateEventError)
TRACE_ERROR(RegNotifyChangeKeyValueError)
SET_ERROR_VAR(RegisterWaitForError, dwErr)
}

STATIC BOOL RegRegistryStoreChange(
    IN PREG_STORE pRegStore,
    IN HANDLE hEvent
    )
{
    BOOL fResult;
    PREGISTRY_STORE_CHANGE_INFO pInfo;

    LockRegStore(pRegStore);

    if (NULL == (pInfo = pRegStore->pRegistryStoreChangeInfo)) {
        if (!InitRegistryStoreChange(pRegStore))
            goto ChangeInitError;
        pInfo = pRegStore->pRegistryStoreChangeInfo;
        assert(pInfo);
    }

    if (!ILS_AddEventToResyncList(
            pRegStore,
            hEvent,
            &pInfo->cNotifyEntry,
            &pInfo->rgNotifyEntry
            ))
        goto AddEventError;

    fResult = TRUE;

CommonReturn:
    UnlockRegStore(pRegStore);
    return fResult;
ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(ChangeInitError)
TRACE_ERROR(AddEventError)
}


STATIC void FreeRegistryStoreChange(
    IN PREG_STORE pRegStore
    )
{
    PREGISTRY_STORE_CHANGE_INFO pInfo;
    if (NULL == (pInfo = pRegStore->pRegistryStoreChangeInfo))
        return;

    assert(pInfo->hRegWaitFor);
    pfnILS_UnregisterWait(pInfo->hRegWaitFor);

    assert(pInfo->hChange);
    if (pRegStore->dwFlags & CERT_REGISTRY_STORE_ROAMING_FLAG)
        FindCloseChangeNotification(pInfo->hChange);
    else
        CloseHandle(pInfo->hChange);

    ILS_SignalEventsOnResyncList(
        &pInfo->cNotifyEntry,
        &pInfo->rgNotifyEntry
        );

    PkiFree(pInfo);
    pRegStore->pRegistryStoreChangeInfo = NULL;
}

#else	//CE_BUILD
STATIC BOOL RegRegistryStoreChange(
    IN PREG_STORE pRegStore,
    IN HANDLE hEvent
    )
{
	return FALSE;
}
#endif

//+=========================================================================
// Key Identifier Functions
//==========================================================================

STATIC HKEY OpenKeyIdStoreSubKey(
    IN DWORD dwFlags,
    IN OPTIONAL LPCWSTR pwszComputerName
    )
{
    SYSTEM_NAME_INFO SystemNameInfo;
    memset(&SystemNameInfo, 0, sizeof(SystemNameInfo));
    SystemNameInfo.rgpwszName[SYSTEM_NAME_INDEX] = wsz_MY_STORE;
    SystemNameInfo.rgpwszName[COMPUTER_NAME_INDEX] = (LPWSTR) pwszComputerName;

    return OpenSystemRegPathKey(
        &SystemNameInfo,
        NULL,               // pwszSubKeyName
        dwFlags
        );
}

BOOL
ILS_ReadKeyIdElement(
    IN const CRYPT_HASH_BLOB *pKeyIdentifier,
    IN BOOL fLocalMachine,
    IN OPTIONAL LPCWSTR pwszComputerName,
    OUT BYTE **ppbElement,
    OUT DWORD *pcbElement
    )
{
    BOOL fResult;
    BYTE *pbElement = NULL;
    DWORD cbElement = 0;
    WCHAR wszHashName[MAX_HASH_NAME_LEN];

    if (0 == pKeyIdentifier->cbData || MAX_HASH_LEN < pKeyIdentifier->cbData)
        goto InvalidArg;
    BytesToWStr(pKeyIdentifier->cbData, pKeyIdentifier->pbData, wszHashName);

    if (!fLocalMachine) {
        LPWSTR pwszRoamingStoreDir;
        if (pwszRoamingStoreDir = ILS_GetRoamingStoreDirectory(
                ROAMING_MY_STORE_SUBDIR)) {
            fResult = ILS_ReadElementFromFile(
                pwszRoamingStoreDir,
                KEYID_CONTEXT_NAME,
                wszHashName,
                0,                          // dwFlags
                &pbElement,
                &cbElement
                );
            PkiFree(pwszRoamingStoreDir);
            if (!fResult) {
                if (ERROR_FILE_NOT_FOUND != GetLastError())
                    goto ReadElementFromFileError;
            } else
                goto CommonReturn;
        }
    }

    {
        HKEY hKey;
        DWORD dwOpenFlags;

        if (fLocalMachine)
            dwOpenFlags = CERT_SYSTEM_STORE_LOCAL_MACHINE;
        else
            dwOpenFlags = CERT_SYSTEM_STORE_CURRENT_USER;
        if (NULL == (hKey = OpenKeyIdStoreSubKey(
                dwOpenFlags | CERT_STORE_READONLY_FLAG,
                pwszComputerName
                )))
            goto OpenKeyIdStoreSubKeyError;

        fResult = ILS_ReadElementFromRegistry(
            hKey,
            KEYID_CONTEXT_NAME,
            wszHashName,
            0,                          // dwFlags
            &pbElement,
            &cbElement
            );

        ILS_CloseRegistryKey(hKey);
        if (!fResult)
            goto ReadElementFromRegistryError;
    }

    fResult = TRUE;

CommonReturn:
    *ppbElement = pbElement;
    *pcbElement = cbElement;
    return fResult;

ErrorReturn:
    assert(NULL == pbElement && 0 == cbElement);
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(InvalidArg, E_INVALIDARG)
TRACE_ERROR(ReadElementFromFileError)
TRACE_ERROR(OpenKeyIdStoreSubKeyError)
TRACE_ERROR(ReadElementFromRegistryError)
}

BOOL
ILS_WriteKeyIdElement(
    IN const CRYPT_HASH_BLOB *pKeyIdentifier,
    IN BOOL fLocalMachine,
    IN OPTIONAL LPCWSTR pwszComputerName,
    IN const BYTE *pbElement,
    IN DWORD cbElement
    )
{
    BOOL fResult;
    WCHAR wszHashName[MAX_HASH_NAME_LEN];

    if (0 == pKeyIdentifier->cbData || MAX_HASH_LEN < pKeyIdentifier->cbData)
        goto InvalidArg;
    BytesToWStr(pKeyIdentifier->cbData, pKeyIdentifier->pbData, wszHashName);

    if (!fLocalMachine) {
        LPWSTR pwszRoamingStoreDir;
        if (pwszRoamingStoreDir = ILS_GetRoamingStoreDirectory(
                ROAMING_MY_STORE_SUBDIR)) {
            fResult = ILS_WriteElementToFile(
                pwszRoamingStoreDir,
                KEYID_CONTEXT_NAME,
                wszHashName,
                0,                          // dwFlags
                pbElement,
                cbElement
                );
            PkiFree(pwszRoamingStoreDir);
            if (!fResult)
                goto WriteElementToFileError;
            else
                goto CommonReturn;
        }
    }

    {
        HKEY hKey;
        DWORD dwOpenFlags;

        if (fLocalMachine)
            dwOpenFlags = CERT_SYSTEM_STORE_LOCAL_MACHINE;
        else
            dwOpenFlags = CERT_SYSTEM_STORE_CURRENT_USER;
        if (NULL == (hKey = OpenKeyIdStoreSubKey(
                dwOpenFlags,
                pwszComputerName
                )))
            goto OpenKeyIdStoreSubKeyError;

        fResult = ILS_WriteElementToRegistry(
            hKey,
            KEYID_CONTEXT_NAME,
            wszHashName,
            0,                          // dwFlags
            pbElement,
            cbElement
            );

        ILS_CloseRegistryKey(hKey);
        if (!fResult)
            goto WriteElementToRegistryError;
    }

    fResult = TRUE;

CommonReturn:
    return fResult;

ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(InvalidArg, E_INVALIDARG)
TRACE_ERROR(WriteElementToFileError)
TRACE_ERROR(OpenKeyIdStoreSubKeyError)
TRACE_ERROR(WriteElementToRegistryError)
}


BOOL
ILS_DeleteKeyIdElement(
    IN const CRYPT_HASH_BLOB *pKeyIdentifier,
    IN BOOL fLocalMachine,
    IN OPTIONAL LPCWSTR pwszComputerName
    )
{
    BOOL fResult;
    WCHAR wszHashName[MAX_HASH_NAME_LEN];

    if (0 == pKeyIdentifier->cbData || MAX_HASH_LEN < pKeyIdentifier->cbData)
        goto InvalidArg;
    BytesToWStr(pKeyIdentifier->cbData, pKeyIdentifier->pbData, wszHashName);

    if (!fLocalMachine) {
        LPWSTR pwszRoamingStoreDir;
        if (pwszRoamingStoreDir = ILS_GetRoamingStoreDirectory(
                ROAMING_MY_STORE_SUBDIR)) {
            fResult = ILS_DeleteElementFromDirectory(
                pwszRoamingStoreDir,
                KEYID_CONTEXT_NAME,
                wszHashName,
                0                           // dwFlags
                );
            PkiFree(pwszRoamingStoreDir);
            if (!fResult)
                goto DeleteElementFromDirectoryError;
            else
                goto CommonReturn;
        }
    }

    {
        HKEY hKey;
        DWORD dwOpenFlags;

        if (fLocalMachine)
            dwOpenFlags = CERT_SYSTEM_STORE_LOCAL_MACHINE;
        else
            dwOpenFlags = CERT_SYSTEM_STORE_CURRENT_USER;
        if (NULL == (hKey = OpenKeyIdStoreSubKey(
                dwOpenFlags,
                pwszComputerName
                )))
            goto OpenKeyIdStoreSubKeyError;

        fResult = ILS_DeleteElementFromRegistry(
            hKey,
            KEYID_CONTEXT_NAME,
            wszHashName,
            0                           // dwFlags
            );

        ILS_CloseRegistryKey(hKey);
        if (!fResult)
            goto DeleteElementFromRegistryError;
    }

    fResult = TRUE;

CommonReturn:
    return fResult;

ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

SET_ERROR(InvalidArg, E_INVALIDARG)
TRACE_ERROR(DeleteElementFromDirectoryError)
TRACE_ERROR(OpenKeyIdStoreSubKeyError)
TRACE_ERROR(DeleteElementFromRegistryError)
}

typedef struct _OPEN_KEYID_CALLBACK_ARG {
    void                        *pvArg;
    PFN_ILS_OPEN_KEYID_ELEMENT  pfnOpenKeyId;
} OPEN_KEYID_CALLBACK_ARG, *POPEN_KEYID_CALLBACK_ARG;

STATIC BOOL OpenKeyIdElementCallback(
    IN const WCHAR wszHashName[MAX_HASH_NAME_LEN],
    IN const BYTE *pbElement,
    IN DWORD cbElement,
    IN void *pvArg
    )
{
    POPEN_KEYID_CALLBACK_ARG pKeyIdArg = (POPEN_KEYID_CALLBACK_ARG) pvArg;

    DWORD cbHash;
    BYTE rgbHash[MAX_HASH_LEN];
    CRYPT_HASH_BLOB KeyIdentifier;

    WStrToBytes(wszHashName, rgbHash, &cbHash);
    KeyIdentifier.cbData = cbHash;
    KeyIdentifier.pbData = rgbHash;

    return pKeyIdArg->pfnOpenKeyId(
        &KeyIdentifier,
        pbElement,
        cbElement,
        pKeyIdArg->pvArg
        );
}

BOOL
ILS_OpenAllKeyIdElements(
    IN BOOL fLocalMachine,
    IN OPTIONAL LPCWSTR pwszComputerName,
    IN void *pvArg,
    IN PFN_ILS_OPEN_KEYID_ELEMENT pfnOpenKeyId
    )
{
    BOOL fResult;
    BOOL fOpenFile = FALSE;

    OPEN_KEYID_CALLBACK_ARG KeyIdArg = { pvArg, pfnOpenKeyId };

    if (!fLocalMachine) {
        LPWSTR pwszRoamingStoreDir;
        if (pwszRoamingStoreDir = ILS_GetRoamingStoreDirectory(
                ROAMING_MY_STORE_SUBDIR)) {
            fResult = ILS_OpenAllElementsFromDirectory(
                pwszRoamingStoreDir,
                KEYID_CONTEXT_NAME,
                0,                          // dwFlags
                (void *) &KeyIdArg,
                OpenKeyIdElementCallback
                );
            PkiFree(pwszRoamingStoreDir);
            if (!fResult)
                goto ErrorReturn;
            else
                fOpenFile = TRUE;
        }
    }

    {
        HKEY hKey;
        DWORD dwOpenFlags;

        if (fLocalMachine)
            dwOpenFlags = CERT_SYSTEM_STORE_LOCAL_MACHINE;
        else
            dwOpenFlags = CERT_SYSTEM_STORE_CURRENT_USER;
        if (NULL == (hKey = OpenKeyIdStoreSubKey(
                dwOpenFlags | CERT_STORE_READONLY_FLAG,
                pwszComputerName
                )))
            goto OpenKeyIdStoreSubKeyError;

        fResult = ILS_OpenAllElementsFromRegistry(
            hKey,
            KEYID_CONTEXT_NAME,
            0,                          // dwFlags
            (void *) &KeyIdArg,
            OpenKeyIdElementCallback
            );

        ILS_CloseRegistryKey(hKey);
    }

CommonReturn:
    if (fOpenFile)
        // Ignore any registry errors
        return TRUE;
    else
        return fResult;

ErrorReturn:
    fResult = FALSE;
    goto CommonReturn;

TRACE_ERROR(OpenKeyIdStoreSubKeyError)
}
