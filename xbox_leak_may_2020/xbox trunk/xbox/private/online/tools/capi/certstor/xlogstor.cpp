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
    return TRUE;
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
	ASSERT(!"xlogstor.cpp called");
	return TRUE; 
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
	ASSERT(!"xlogstor.cpp called");
	return TRUE; 
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
	ASSERT(!"xlogstor.cpp called");
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
	ASSERT(!"xlogstor.cpp called");
	return TRUE; 
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
	ASSERT(!"xlogstor.cpp called");
	return TRUE; 
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
	ASSERT(!"xlogstor.cpp called");
	return TRUE; 
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
	ASSERT(!"xlogstor.cpp called");
	return TRUE;
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
	ASSERT(!"xlogstor.cpp called");
	return TRUE;
}


BOOL
ILS_DeleteKeyIdElement(
    IN const CRYPT_HASH_BLOB *pKeyIdentifier,
    IN BOOL fLocalMachine,
    IN OPTIONAL LPCWSTR pwszComputerName
    )
{
	ASSERT(!"xlogstor.cpp called");
	return TRUE;
}

BOOL
ILS_OpenAllKeyIdElements(
    IN BOOL fLocalMachine,
    IN OPTIONAL LPCWSTR pwszComputerName,
    IN void *pvArg,
    IN PFN_ILS_OPEN_KEYID_ELEMENT pfnOpenKeyId
    )
{
	ASSERT(!"xlogstor.cpp called");
	return TRUE;
}


