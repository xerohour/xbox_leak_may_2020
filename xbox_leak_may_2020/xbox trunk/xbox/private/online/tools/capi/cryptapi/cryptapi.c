/////////////////////////////////////////////////////////////////////////////
//  FILE          : cryptapi.c                                             //
//  DESCRIPTION   : Crypto API interface                                   //
//  AUTHOR        :                                                        //
//  HISTORY       :                                                        //
//      Dec  6 1994 larrys  New                                            //
//      Jan 16 1995 larrys  Added key verify                               //
//      Jan 25 1995 larrys  Added thread safe                              //
//      Jan 27 1995 larrys  Added Unicode support                          //
//      Feb 21 1995 larrys  Added Unicode support for CryptAcquireContext  //
//      Feb 21 1995 larrys  Fixed Unicode problem in CryptAcquireContext   //
//      Mar 08 1995 larrys  Removed CryptGetLastError                      //
//      Mar 20 1995 larrys  Removed Certificate APIs                       //
//      Mar 22 1995 larrys  #ifdef in WIN95 code                           //
//      Apr 06 1995 larrys  Increased signature key to 1024 bits           //
//      Apr 07 1995 larrys  Removed CryptConfigure                         //
//      Jun 14 1995 larrys  Removed pointer from RSA key struct            //
//      Jun 29 1995 larrys  Changed AcquireContext                         //
//      Jul 17 1995 larrys  Worked on AcquireContext                       //
//      Aug 01 1995 larrys  Removed CryptTranslate                         //
//                          And CryptDeinstallProvider                     //
//                          Changed CryptInstallProvider to                //
//                          CryptSetProvider                               //
//      Aug 03 1995 larrys  Cleanup                                        //
//      Aug 10 1995 larrys  CryptAcquireContext returns error              //
//                          NTE_BAD_KEYSEY_PARAM now                       //
//      Aug 14 1995 larrys  Removed key exchange stuff                     //
//      Aug 17 1995 larrys  Changed registry entry to decimcal             //
//      Aug 23 1995 larrys  Changed CryptFinishHash to CryptGetHashValue   //
//      Aug 28 1995 larrys  Removed parameter from CryptVerifySignature    //
//      Aug 31 1995 larrys  Remove GenRandom                               //
//      Sep 14 1995 larrys  Code review changes                            //
//      Sep 26 1995 larrys  Added Microsoft's signing key                  //
//      Sep 27 1995 larrys  Updated with more review changes               //
//      Oct 06 1995 larrys  Added more APIs Get/SetHash/ProvParam          //
//      Oct 12 1995 larrys  Remove CryptGetHashValue                       //
//      Oct 20 1995 larrys  Changed test key                               //
//      Oct 24 1995 larrys  Removed return of KeySet name                  //
//      Oct 30 1995 larrys  Removed WIN95                                  //
//      Nov  9 1995 larrys  Disable BUILD1057                              //
//      Nov 10 1995 larrys  Fix a problem in EnterHashCritSec              //
//      May 30 1996 larrys  Added hWnd support                             //
//      Oct 10 1996 jeffspel Reordered SetLastErrors and save error on     //
//                           AcquireContext failure                        //
//      Mar 21 1997 jeffspel Added second tier signatures, new APIs        //
//      Apr 11 1997 jeffspel Replace critical sections with interlocked    //
//                           inc/dec                                       //
//                                                                         //
//  Copyright (C) 1993 Microsoft Corporation   All Rights Reserved         //
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <xdbg.h>

#include <wincrypt.h>   // Include here, since not included by LEAN_AND_MEAN
#include <rsa.h>
#include <msr_md5.h>
#include <rc4.h>
#include "scp.h"

#define EMULATE_RC4
#define EMULATE_BSAFE
#include "xcrypt.h"

// #define _HRESULT_TYPEDEF_ (unsigned long)

#ifndef RC4_KEYSIZE
#define RC4_KEYSIZE 5
#endif

#define IDR_PUBKEY1                     102


typedef struct _VTableStruc {
// ******************** WARNING **********************************************
// Do not place anything before these FARPROCs we init the table assuming
// that the first Function to call is the first thing in the table.
// ***************************************************************************
    FARPROC FuncAcquireContext;
    FARPROC FuncReleaseContext;
    FARPROC FuncGenKey;
    FARPROC FuncDeriveKey;
    FARPROC FuncDestroyKey;
    FARPROC FuncSetKeyParam;
    FARPROC FuncGetKeyParam;
    FARPROC FuncExportKey;
    FARPROC FuncImportKey;
    FARPROC FuncEncrypt;
    FARPROC FuncDecrypt;
    FARPROC FuncCreateHash;
    FARPROC FuncHashData;
    FARPROC FuncHashSessionKey;
    FARPROC FuncDestroyHash;
    FARPROC FuncSignHash;
    FARPROC FuncVerifySignature;
    FARPROC FuncGenRandom;
    FARPROC FuncGetUserKey;
    FARPROC FuncSetProvParam;
    FARPROC FuncGetProvParam;
    FARPROC FuncSetHashParam;
    FARPROC FuncGetHashParam;
    FARPROC FuncNULL;

    FARPROC OptionalFuncDuplicateKey;
    FARPROC OptionalFuncDuplicateHash;
    FARPROC OptionalFuncNULL;

    HANDLE      DllHandle;                     // Handle to open DLL
    HCRYPTPROV  hProv;                         // Handle to provider
    DWORD       Version;
    DWORD       Inuse;
    LONG        RefCnt;
} VTableStruc, *PVTableStruc;

typedef struct _VKeyStruc {
// ******************** WARNING **********************************************
// Do not place anything before these FARPROCs we init the table assuming
// that the first Function to call is the first thing in the table.
// ***************************************************************************
    FARPROC FuncGenKey;
    FARPROC FuncDeriveKey;
    FARPROC FuncDestroyKey;
    FARPROC FuncSetKeyParam;
    FARPROC FuncGetKeyParam;
    FARPROC FuncExportKey;
    FARPROC FuncImportKey;
    FARPROC FuncEncrypt;
    FARPROC FuncDecrypt;

    FARPROC OptionalFuncDuplicateKey;

    HCRYPTPROV  hProv;                         // Handle to provider
    HCRYPTKEY   hKey;                          // Handle to key
    DWORD       Version;
    DWORD       Inuse;
} VKeyStruc, *PVKeyStruc;

typedef struct _VHashStruc {
// ******************** WARNING **********************************************
// Do not place anything before these FARPROCs we init the table assuming
// that the first Function to call is the first thing in the table.
// ***************************************************************************
    FARPROC FuncCreateHash;
    FARPROC FuncHashData;
    FARPROC FuncHashSessionKey;
    FARPROC FuncDestroyHash;
    FARPROC FuncSignHash;
    FARPROC FuncVerifySignature;
    FARPROC FuncSetHashParam;
    FARPROC FuncGetHashParam;

    FARPROC OptionalFuncDuplicateHash;

    HCRYPTPROV  hProv;                         // Handle to provider
    HCRYPTHASH  hHash;                         // Handle to hash
    DWORD       Version;
    DWORD       Inuse;
} VHashStruc, *PVHashStruc;

//
// Crypto providers have to have the following entry points:
//
LPCTSTR FunctionNames[] = {
    TEXT("CPAcquireContext"),
    TEXT("CPReleaseContext"),
    TEXT("CPGenKey"),
    TEXT("CPDeriveKey"),
    TEXT("CPDestroyKey"),
    TEXT("CPSetKeyParam"),
    TEXT("CPGetKeyParam"),
    TEXT("CPExportKey"),
    TEXT("CPImportKey"),
    TEXT("CPEncrypt"),
    TEXT("CPDecrypt"),
    TEXT("CPCreateHash"),
    TEXT("CPHashData"),
    TEXT("CPHashSessionKey"),
    TEXT("CPDestroyHash"),
    TEXT("CPSignHash"),
    TEXT("CPVerifySignature"),
    TEXT("CPGenRandom"),
    TEXT("CPGetUserKey"),
    TEXT("CPSetProvParam"),
    TEXT("CPGetProvParam"),
    TEXT("CPSetHashParam"),
    TEXT("CPGetHashParam"),
    NULL
    };

LPCTSTR OptionalFunctionNames[] = {
    TEXT("CPDuplicateKey"),
    TEXT("CPDuplicateHash"),
    NULL
    };

BOOL KeyDecrypted = FALSE;

DWORD hWnd = 0;
BYTE *pbContextInfo = NULL;
DWORD cbContextInfo = 0;

#define KEYSIZE512 0x48
#define KEYSIZE1024 0x88

typedef struct _SECONDTIER_SIG
{
    DWORD           dwMagic;
    DWORD           cbSig;
    BSAFE_PUB_KEY   Pub;
} SECOND_TIER_SIG, *PSECOND_TIER_SIG;

#ifdef TEST_BUILD_EXPONENT
#pragma message("WARNING: building advapai32.dll with TESTKEY enabled!")
struct _TESTKEY {
    BSAFE_PUB_KEY    PUB;
    unsigned char pubmodulus[KEYSIZE512];
} TESTKEY = {
    {
	0x66b8443b,
	0x6f5fc900,
	0xa12132fe,
	0xff1b06cf,
	0x2f4826eb,
    },
    {
	0x3e, 0x69, 0x4f, 0x45, 0x31, 0x95, 0x60, 0x6c,
	0x80, 0xa5, 0x41, 0x99, 0x3e, 0xfc, 0x92, 0x2c,
	0x93, 0xf9, 0x86, 0x23, 0x3d, 0x48, 0x35, 0x81,
	0x19, 0xb6, 0x7c, 0x04, 0x43, 0xe6, 0x3e, 0xd4,
	0xd5, 0x43, 0xaf, 0x52, 0xdd, 0x51, 0x20, 0xac,
	0xc3, 0xca, 0xee, 0x21, 0x9b, 0x4a, 0x2d, 0xf7,
	0xd8, 0x5f, 0x32, 0xeb, 0x49, 0x72, 0xb9, 0x8d,
	0x2e, 0x1a, 0x76, 0x7f, 0xde, 0xc6, 0x75, 0xab,
	0xaf, 0x67, 0xe0, 0xf0, 0x8b, 0x30, 0x20, 0x92,
    }
};
#endif


#ifdef MS_INTERNAL_KEY
struct _mskey {
    BSAFE_PUB_KEY    PUB;
    unsigned char pubmodulus[KEYSIZE1024];
} MSKEY = {
    {
	0x2bad85ae,
	0x883adacc,
	0xb32ebd68,
	0xa7ec8b06,
	0x58dbeb81,
    },
    {
	0x42, 0x34, 0xb7, 0xab, 0x45, 0x0f, 0x60, 0xcd,
	0x8f, 0x77, 0xb5, 0xd1, 0x79, 0x18, 0x34, 0xbe,
	0x66, 0xcb, 0x5c, 0x66, 0x4a, 0x9f, 0x03, 0x18,
	0x13, 0x36, 0x8e, 0x88, 0x21, 0x78, 0xb1, 0x94,
	0xa1, 0xd5, 0x8f, 0x8c, 0xa5, 0xd3, 0x9f, 0x86,
	0x43, 0x89, 0x05, 0xa0, 0xe3, 0xee, 0xe2, 0xd0,
	0xe5, 0x1d, 0x5f, 0xaf, 0xff, 0x85, 0x71, 0x7a,
	0x0a, 0xdb, 0x2e, 0xd8, 0xc3, 0x5f, 0x2f, 0xb1,
	0xf0, 0x53, 0x98, 0x3b, 0x44, 0xee, 0x7f, 0xc9,
	0x54, 0x26, 0xdb, 0xdd, 0xfe, 0x1f, 0xd0, 0xda,
	0x96, 0x89, 0xc8, 0x9e, 0x2b, 0x5d, 0x96, 0xd1,
	0xf7, 0x52, 0x14, 0x04, 0xfb, 0xf8, 0xee, 0x4d,
	0x92, 0xd1, 0xb6, 0x37, 0x6a, 0xe0, 0xaf, 0xde,
	0xc7, 0x41, 0x06, 0x7a, 0xe5, 0x6e, 0xb1, 0x8c,
	0x8f, 0x17, 0xf0, 0x63, 0x8d, 0xaf, 0x63, 0xfd,
	0x22, 0xc5, 0xad, 0x1a, 0xb1, 0xe4, 0x7a, 0x6b,
	0x1e, 0x0e, 0xea, 0x60, 0x56, 0xbd, 0x49, 0xd0,
    }
};
#endif


struct _key {
    BSAFE_PUB_KEY    PUB;
    unsigned char pubmodulus[KEYSIZE1024];
} KEY = {
    {
	0x3fcbf1a9,
	0x08f597db,
	0xe4aecab4,
	0x75360f90,
	0x9d6c0f00,
    },
    {
	0x85, 0xdd, 0x9b, 0xf4, 0x4d, 0x0b, 0xc4, 0x96,
	0x3e, 0x79, 0x86, 0x30, 0x6d, 0x27, 0x31, 0xee,
	0x4a, 0x85, 0xf5, 0xff, 0xbb, 0xa9, 0xbd, 0x81,
	0x86, 0xf2, 0x4f, 0x87, 0x6c, 0x57, 0x55, 0x19,
	0xe4, 0xf4, 0x49, 0xa3, 0x19, 0x27, 0x08, 0x82,
	0x9e, 0xf9, 0x8a, 0x8e, 0x41, 0xd6, 0x91, 0x71,
	0x47, 0x48, 0xee, 0xd6, 0x24, 0x2d, 0xdd, 0x22,
	0x72, 0x08, 0xc6, 0xa7, 0x34, 0x6f, 0x93, 0xd2,
	0xe7, 0x72, 0x57, 0x78, 0x7a, 0x96, 0xc1, 0xe1,
	0x47, 0x38, 0x78, 0x43, 0x53, 0xea, 0xf3, 0x88,
	0x82, 0x66, 0x41, 0x43, 0xd4, 0x62, 0x44, 0x01,
	0x7d, 0xb2, 0x16, 0xb3, 0x50, 0x89, 0xdb, 0x0a,
	0x93, 0x17, 0x02, 0x02, 0x46, 0x49, 0x79, 0x76,
	0x59, 0xb6, 0xb1, 0x2b, 0xfc, 0xb0, 0x9a, 0x21,
	0xe6, 0xfa, 0x2d, 0x56, 0x07, 0x36, 0xbc, 0x13,
	0x7f, 0x1c, 0xde, 0x55, 0xfb, 0x0d, 0x67, 0x0f,
	0xc2, 0x17, 0x45, 0x8a, 0x14, 0x2b, 0xba, 0x55,
    }
};


struct _key2 {
    BSAFE_PUB_KEY    PUB;
    unsigned char pubmodulus[KEYSIZE1024];
} KEY2 =  {
    {
	0x685fc690,
	0x97d49b6b,
	0x1dccd9d2,
	0xa5ec9b52,
	0x64fd29d7,
    },
    {
	0x03, 0x8c, 0xa3, 0x9e, 0xfb, 0x93, 0xb6, 0x72,
	0x2a, 0xda, 0x6f, 0xa5, 0xec, 0x26, 0x39, 0x58,
	0x41, 0xcd, 0x3f, 0x49, 0x10, 0x4c, 0xcc, 0x7e,
	0x23, 0x94, 0xf9, 0x5d, 0x9b, 0x2b, 0xa3, 0x6b,
	0xe8, 0xec, 0x52, 0xd9, 0x56, 0x64, 0x74, 0x7c,
	0x44, 0x6f, 0x36, 0xb7, 0x14, 0x9d, 0x02, 0x3c,
	0x0e, 0x32, 0xb6, 0x38, 0x20, 0x25, 0xbd, 0x8c,
	0x9b, 0xd1, 0x46, 0xa7, 0xb3, 0x58, 0x4a, 0xb7,
	0xdd, 0x0e, 0x38, 0xb6, 0x16, 0x44, 0xbf, 0xc1,
	0xca, 0x4d, 0x6a, 0x9f, 0xcb, 0x6f, 0x3c, 0x5f,
	0x03, 0xab, 0x7a, 0xb8, 0x16, 0x70, 0xcf, 0x98,
	0xd0, 0xca, 0x8d, 0x25, 0x57, 0x3a, 0x22, 0x8b,
	0x44, 0x96, 0x37, 0x51, 0x30, 0x00, 0x92, 0x1b,
	0x03, 0xb9, 0xf9, 0x0d, 0xb3, 0x1a, 0xe2, 0xb4,
	0xc5, 0x7b, 0xc9, 0x4b, 0xe2, 0x42, 0x25, 0xfe,
	0x3d, 0x42, 0xfa, 0x45, 0xc6, 0x94, 0xc9, 0x8e,
	0x87, 0x7e, 0xf6, 0x68, 0x90, 0x30, 0x65, 0x10,
    }
};


#define TABLEPROV       0x11111111
#define TABLEKEY        0x22222222
#define TABLEHASH       0x33333333

WCHAR szusertypeW[] = L"Comm\\Security\\Crypto\\Providers\\Type ";
WCHAR szmachinetypeW[] = L"Comm\\Security\\Crypto\\Defaults\\Provider Types\\Type ";
WCHAR szproviderW[] = L"Comm\\Security\\Crypto\\Defaults\\Provider\\";
WCHAR szenumprovidersW[] = L"Comm\\Security\\Crypto\\Defaults\\Provider";
WCHAR szprovidertypesW[] = L"Comm\\Security\\Crypto\\Defaults\\Provider Types";

BOOL EnterProviderCritSec(IN PVTableStruc pVTable);
void LeaveProviderCritSec(IN PVTableStruc pVTable);
BOOL EnterKeyCritSec(IN PVKeyStruc pVKey);
void LeaveKeyCritSec(IN PVKeyStruc pVKey);
BOOL EnterHashCritSec(IN PVHashStruc pVHash);
void LeaveHashCritSec(IN PVHashStruc pVHash);

BOOL CProvVerifyImage(LPCWSTR lpszImage,
		              BYTE *pSigData);

BOOL NewVerifyImage(LPCWSTR lpszImage,
		            BYTE *pSigData,
                    DWORD cbData,
                    BOOL fUnknownLen);

BOOL BuildVKey(IN PVKeyStruc *ppVKey,
               IN PVTableStruc pVTable);

BOOL BuildVHash(
                IN PVHashStruc *ppVKey,
                IN PVTableStruc pVTable
                );

BOOL CPReturnhWnd(DWORD *phWnd);

static void __ltoaW(DWORD val, WCHAR *buf);


DWORD StrToLW(WCHAR *InStr)
{
    DWORD dwVal = 0;

    while(*InStr)
    {
        dwVal = (10 * dwVal) + (*InStr - L'0');
        InStr++;
    }

    return dwVal;
}

/*
extern FARPROC CPAcquireContext;
extern FARPROC CPReleaseContext;
extern FARPROC CPGenKey;
extern FARPROC CPDeriveKey;
extern FARPROC CPDestroyKey;
extern FARPROC CPSetKeyParam;
extern FARPROC CPGetKeyParam;
extern FARPROC CPExportKey;
extern FARPROC CPImportKey;
extern FARPROC CPEncrypt;
extern FARPROC CPDecrypt;
extern FARPROC CPCreateHash;
extern FARPROC CPHashData;
extern FARPROC CPHashSessionKey;
extern FARPROC CPDestroyHash;
extern FARPROC CPSignHash;
extern FARPROC CPVerifySignature;
extern FARPROC CPGenRandom;
extern FARPROC CPGetUserKey;
extern FARPROC CPSetProvParam;
extern FARPROC CPGetProvParam;
extern FARPROC CPSetHashParam;
extern FARPROC CPGetHashParam;

extern FARPROC CPDuplicateKey;
extern FARPROC CPDuplicateHash;
*/


/*
 -      CryptAcquireContextW
 -
 *      Purpose:
 *               The CryptAcquireContext function is used to acquire a context
 *               handle to a cryptograghic service provider (CSP).
 *
 *
 *      Parameters:
 *               OUT    phProv         -  Handle to a CSP
 *               IN OUT pszIdentity    -  Pointer to the name of the context's
 *                                        keyset.
 *               IN OUT pszProvName    -  Pointer to the name of the provider.
 *               IN     dwReqProvider  -  Requested CSP type
 *               IN     dwFlags        -  Flags values
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptAcquireContextW(OUT    HCRYPTPROV *phProv,
                IN     LPCWSTR pszIdentity,
                IN     LPCWSTR pszProvName,
                IN     DWORD dwReqProvider,
                IN     DWORD dwFlags)
{
    HANDLE          handle = 0;
    DWORD           bufsize;
    DWORD           *pTable;
    PVTableStruc    pVTable = NULL;
    WCHAR           *pszTmpProvName = NULL;
    DWORD           i;
    HKEY            hKey = 0;
    DWORD           cbValue;
    DWORD           cbTemp;
    WCHAR           *pszValue = NULL;
    BYTE            *SignatureBuf = NULL;
    DWORD           provtype;
    BOOL            rt = CRYPT_FAILED;
    DWORD           dwType;
    LONG            err;
    DWORD           dwErr;
    WCHAR            typebuf[9]; // sfield = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    VTableProvStruc VTableProv;
    
    bufsize = sizeof(VTableStruc);

    if ((pVTable = (PVTableStruc) LocalAlloc(LMEM_ZEROINIT,
                                             (UINT) bufsize)) == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    
    pVTable->FuncAcquireContext = CPAcquireContext;
    pVTable->FuncReleaseContext = CPReleaseContext;
    pVTable->FuncGenKey = CPGenKey;
    pVTable->FuncDeriveKey = CPDeriveKey;
    pVTable->FuncDestroyKey = CPDestroyKey;
    pVTable->FuncSetKeyParam = CPSetKeyParam;
    pVTable->FuncGetKeyParam = CPGetKeyParam;
    pVTable->FuncExportKey = CPExportKey;
    pVTable->FuncImportKey = CPImportKey;
    pVTable->FuncEncrypt = CPEncrypt;
    pVTable->FuncDecrypt = CPDecrypt;
    pVTable->FuncCreateHash = CPCreateHash;
    pVTable->FuncHashData = CPHashData;
    pVTable->FuncHashSessionKey = CPHashSessionKey;
    pVTable->FuncDestroyHash = CPDestroyHash;
    pVTable->FuncSignHash = CPSignHash;
    pVTable->FuncVerifySignature = CPVerifySignature;
    pVTable->FuncGenRandom = CPGenRandom;
    pVTable->FuncGetUserKey = CPGetUserKey;
    pVTable->FuncSetProvParam = CPSetProvParam;
    pVTable->FuncGetProvParam = CPGetProvParam;
    pVTable->FuncSetHashParam = CPSetHashParam;
    pVTable->FuncGetHashParam = CPGetHashParam;

    pVTable->OptionalFuncDuplicateKey = CPDuplicateKey;
    pVTable->OptionalFuncDuplicateHash = CPDuplicateHash;
    
    pVTable->DllHandle = handle;

    memset(&VTableProv, 0, sizeof(VTableProv));
    VTableProv.Version = 3;
    VTableProv.FuncVerifyImage = CProvVerifyImage;
    VTableProv.FuncReturnhWnd = CPReturnhWnd;
    VTableProv.dwProvType = dwReqProvider;
    VTableProv.pszProvName = "RSA";
    VTableProv.pbContextInfo = pbContextInfo;
    VTableProv.cbContextInfo = cbContextInfo;

    try
    {
        rt = pVTable->FuncAcquireContext(phProv, pszIdentity, dwFlags,
                                         &VTableProv);
        if (RCRYPT_SUCCEEDED(rt) &&
            ((dwFlags & CRYPT_DELETEKEYSET) != CRYPT_DELETEKEYSET))
        {
            pVTable->hProv = *phProv;
            *phProv = (HCRYPTPROV) pVTable;

            pVTable->Version = TABLEPROV;
            pVTable->Inuse = 1;
            pVTable->RefCnt = 1;
        }
    } except ( EXCEPTION_EXECUTE_HANDLER )
    {
        rt = CRYPT_FAILED;
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }


Ret:
    dwErr = GetLastError();
    if (pszTmpProvName)
        LocalFree(pszTmpProvName);
    if (pszValue)
        LocalFree(pszValue);
    //if (hKey)
    //    RegCloseKey(hKey);
    if (SignatureBuf)
        LocalFree(SignatureBuf);
    if ((CRYPT_SUCCEED != rt) || (dwFlags & CRYPT_DELETEKEYSET))
    {
        //if (handle)
        //    FreeLibrary(handle);
        if (pVTable)
            LocalFree(pVTable);
        SetLastError(dwErr);
    }
    return rt;
}

/*
 -      CryptContextAddRef
 -
 *      Purpose:
 *               Increments the reference counter on the provider handle.
 *
 *      Parameters:
 *               IN  hProv         -  Handle to a CSP
 *               IN  pdwReserved   -  Reserved parameter
 *               IN  dwFlags       -  Flags values
 *
 *      Returns:
 *               BOOL
 *               Use get extended error information use GetLastError
 */
WINADVAPI
BOOL
WINAPI CryptContextAddRef(
                          IN HCRYPTPROV hProv,
                          IN DWORD *pdwReserved,
			              IN DWORD dwFlags
                          )
{
    PVTableStruc    pVTable;
    BOOL            fRet = CRYPT_FAILED;

    try
    {
        if ((NULL != pdwReserved) || (0 != dwFlags))
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        pVTable = (PVTableStruc) hProv;

        if (pVTable->Version != TABLEPROV)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (InterlockedIncrement(&pVTable->RefCnt) <= 0)
            SetLastError(ERROR_INVALID_PARAMETER);
        else
            fRet = CRYPT_SUCCEED;
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

Ret:
    return fRet;
}

/*
 -      CryptReleaseContext
 -
 *      Purpose:
 *               The CryptReleaseContext function is used to release a
 *               context created by CryptAcquireContext.
 *
 *     Parameters:
 *               IN  hProv         -  Handle to a CSP
 *               IN  dwFlags       -  Flags values
 *
 *      Returns:
 *               BOOL
 *               Use get extended error information use GetLastError
 */
WINADVAPI
BOOL
WINAPI CryptReleaseContext(IN HCRYPTPROV hProv,
                           IN DWORD dwFlags)
{
#ifndef _XBOX
    PVTableStruc    pVTable;
    BOOL            rt;
    BOOL            fRet = CRYPT_FAILED;
    DWORD           dwErr = ERROR_INVALID_PARAMETER;

    try
    {
        pVTable = (PVTableStruc) hProv;

        if (pVTable->Version != TABLEPROV)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (pVTable->RefCnt <= 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (0 == InterlockedDecrement(&pVTable->RefCnt))
        {
            if (0 < InterlockedDecrement(&pVTable->Inuse))
            {
                InterlockedIncrement(&pVTable->Inuse);
                SetLastError(ERROR_BUSY);
                goto Ret;
            }
            InterlockedIncrement(&pVTable->Inuse);

            if (FALSE == (rt = pVTable->FuncReleaseContext(pVTable->hProv, dwFlags)))
            {
                dwErr = GetLastError();
            }
            pVTable->Version = 0;
            FreeLibrary(pVTable->DllHandle);
            LocalFree(pVTable);
            if (!rt)
            {
                SetLastError(dwErr);
                goto Ret;
            }
        }
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;
Ret:
    return fRet;
#else
	return TRUE;
#endif
}

/*
 -      CryptGenKey
 -
 *      Purpose:
 *                Generate cryptographic keys
 *
 *
 *      Parameters:
 *               IN      hProv   -  Handle to a CSP
 *               IN      Algid   -  Algorithm identifier
 *               IN      dwFlags -  Flags values
 *               OUT     phKey   -  Handle to a generated key
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptGenKey(IN HCRYPTPROV hProv,
            IN ALG_ID Algid,
            IN DWORD dwFlags,
            OUT HCRYPTKEY * phKey)
{
    PVTableStruc    pVTable;
    PVKeyStruc      pVKey = NULL;
    BOOL            fProvCritSec = FALSE;
    DWORD           dwErr;
    BOOL            fRet = CRYPT_FAILED;

    try
    {
        pVTable = (PVTableStruc) hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fProvCritSec = TRUE;

        if (RCRYPT_FAILED(BuildVKey(&pVKey, pVTable)))
        {
            goto Ret;
        }

        if (RCRYPT_FAILED(pVTable->FuncGenKey(pVTable->hProv, Algid, dwFlags,
                            phKey)))
        {
            goto Ret;
        }

        pVKey->hKey = *phKey;

        *phKey = (HCRYPTKEY) pVKey;

        pVKey->Version = TABLEKEY;

        pVKey->hProv = hProv;

        pVKey->Inuse = 1;

    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;
Ret:
    dwErr = GetLastError();
    if (fProvCritSec)
        LeaveProviderCritSec(pVTable);
    if (CRYPT_SUCCEED != fRet)
    {
        if (pVKey)
            LocalFree(pVKey);
        SetLastError(dwErr);
    }
    return fRet;
}

/*
 -      CryptDuplicateKey
 -
 *      Purpose:
 *                Duplicate a cryptographic key
 *
 *
 *      Parameters:
 *               IN      hKey           -  Handle to the key to be duplicated
 *               IN      pdwReserved    -  Reserved for later use
 *               IN      dwFlags        -  Flags values
 *               OUT     phKey          -  Handle to the new duplicate key
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptDuplicateKey(
                         IN HCRYPTKEY hKey,
		                 IN DWORD *pdwReserved,
		                 IN DWORD dwFlags,
		                 OUT HCRYPTKEY * phKey
                         )
{
    PVTableStruc    pVTable;
    PVKeyStruc      pVKey;
    PVKeyStruc      pVNewKey = NULL;
    HCRYPTKEY       hNewKey;
    BOOL            fProvCritSecSet = FALSE;
    DWORD           dwErr = 0;
    BOOL            fRet = CRYPT_FAILED;

    try
    {
        pVKey = (PVKeyStruc) hKey;

        if (pVKey->Version != TABLEKEY)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (NULL == pVKey->OptionalFuncDuplicateKey)
        {
            SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
            goto Ret;
        }

        pVTable = (PVTableStruc) pVKey->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }
        fProvCritSecSet = TRUE;

        if (RCRYPT_FAILED(BuildVKey(&pVNewKey, pVTable)))
        {
            goto Ret;
        }

        if (RCRYPT_FAILED(pVKey->OptionalFuncDuplicateKey(pVTable->hProv, pVKey->hKey,
                                                          pdwReserved, dwFlags, &hNewKey)))
        {
            goto Ret;
        }

        pVNewKey->hKey = hNewKey;

        pVNewKey->Version = TABLEKEY;

        pVNewKey->hProv = pVKey->hProv;

        pVKey->Inuse = 1;

        *phKey = (HCRYPTKEY) pVNewKey;
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;
Ret:
    dwErr = GetLastError();
    if (fProvCritSecSet)
        LeaveProviderCritSec(pVTable);
    if (fRet == CRYPT_FAILED)
    {
        if (NULL != pVNewKey)
            LocalFree(pVNewKey);
        SetLastError(dwErr);
    }

    return fRet;
}

/*
 -      CryptDeriveKey
 -
 *      Purpose:
 *                Derive cryptographic keys from base data
 *
 *
 *      Parameters:
 *               IN      hProv      -  Handle to a CSP
 *               IN      Algid      -  Algorithm identifier
 *               IN      hHash      -  Handle to hash of base data
 *               IN      dwFlags    -  Flags values
 *               IN OUT  phKey      -  Handle to a generated key
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptDeriveKey(IN HCRYPTPROV hProv,
                IN ALG_ID Algid,
                IN HCRYPTHASH hHash,
                IN DWORD dwFlags,
                IN OUT HCRYPTKEY * phKey)
{
    PVTableStruc    pVTable;
    PVKeyStruc      pVKey = NULL;
    PVHashStruc     pVHash;
    BOOL            fProvCritSec = FALSE;
    BOOL            fKeyCritSec = FALSE;
    BOOL            fHashCritSec = FALSE;
    BOOL            fUpdate = FALSE;
    DWORD           dwErr;
    BOOL            fRet = CRYPT_FAILED;

    try
    {
        pVTable = (PVTableStruc) hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fProvCritSec = TRUE;

        pVHash = (PVHashStruc) hHash;

        if (RCRYPT_FAILED(EnterHashCritSec(pVHash)))
        {
            goto Ret;
        }
        fHashCritSec = TRUE;

        if (dwFlags & CRYPT_UPDATE_KEY)
        {
            fUpdate = TRUE;
            pVKey = (PVKeyStruc) phKey;

            if (RCRYPT_FAILED(EnterKeyCritSec(pVKey)))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                goto Ret;
            }
            fKeyCritSec = TRUE;
        }
        else
        {
            if (RCRYPT_FAILED(BuildVKey(&pVKey, pVTable)))
            {
                goto Ret;
            }
        }

        if (RCRYPT_FAILED(pVTable->FuncDeriveKey(pVTable->hProv, Algid,
                        pVHash->hHash, dwFlags, phKey)))
        {
            goto Ret;
        }

        if ((dwFlags & CRYPT_UPDATE_KEY) != CRYPT_UPDATE_KEY)
        {
            pVKey->hKey = *phKey;

            *phKey = (HCRYPTKEY)pVKey;

            pVKey->hProv = hProv;

            pVKey->Version = TABLEKEY;

            pVKey->Inuse = 1;
        }

    } except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;
Ret:
    if (CRYPT_SUCCEED != fRet)
        dwErr = GetLastError();
    if (fProvCritSec)
        LeaveProviderCritSec(pVTable);
    if (fHashCritSec)
        LeaveHashCritSec(pVHash);
    if (fKeyCritSec)
        LeaveKeyCritSec(pVKey);
    if (CRYPT_SUCCEED != fRet)
    {
        if (pVKey && (!fUpdate))
            LocalFree(pVKey);
        SetLastError(dwErr);
    }
    return fRet;
}


/*
 -      CryptDestroyKey
 -
 *      Purpose:
 *                Destroys the cryptographic key that is being referenced
 *                with the hKey parameter
 *
 *
 *      Parameters:
 *               IN      hKey   -  Handle to a key
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptDestroyKey(IN HCRYPTKEY hKey)
{
    PVTableStruc    pVTable;
    PVKeyStruc      pVKey;
    BOOL            fProvCritSec = FALSE;
    BOOL            rt;
    DWORD           dwErr;
    BOOL            fRet = CRYPT_FAILED;

    try
    {
        pVKey = (PVKeyStruc) hKey;

        if (pVKey->Version != TABLEKEY)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (0 < InterlockedDecrement(&pVKey->Inuse))
        {
            InterlockedIncrement(&pVKey->Inuse);
            SetLastError(ERROR_BUSY);
            goto Ret;
        }
        InterlockedIncrement(&pVKey->Inuse);

        pVTable = (PVTableStruc) pVKey->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fProvCritSec = TRUE;

        if (FALSE == (rt = pVKey->FuncDestroyKey(pVTable->hProv, pVKey->hKey)))
            dwErr = GetLastError();

        pVKey->Version = 0;
        LocalFree(pVKey);

        if (!rt)
        {
            SetLastError(dwErr);
            goto Ret;
        }
    } except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;
Ret:
    if (CRYPT_SUCCEED != fRet)
        dwErr = GetLastError();
    if (fProvCritSec)
        LeaveProviderCritSec(pVTable);
    if (CRYPT_SUCCEED != fRet)
        SetLastError(dwErr);
    return fRet;
}


/*
 -      CryptSetKeyParam
 -
 *      Purpose:
 *                Allows applications to customize various aspects of the
 *                operations of a key
 *
 *      Parameters:
 *               IN      hKey    -  Handle to a key
 *               IN      dwParam -  Parameter number
 *               IN      pbData  -  Pointer to data
 *               IN      dwFlags -  Flags values
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptSetKeyParam(IN HCRYPTKEY hKey,
            IN DWORD dwParam,
            IN BYTE *pbData,
            IN DWORD dwFlags)
{
    PVTableStruc    pVTable;
    PVKeyStruc      pVKey;
    BOOL            rt = CRYPT_FAILED;
    BOOL            fCritSec = FALSE;

    try
    {
        pVKey = (PVKeyStruc) hKey;

        if (pVKey->Version != TABLEKEY)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (0 < InterlockedDecrement(&pVKey->Inuse))
        {
            InterlockedIncrement(&pVKey->Inuse);
            SetLastError(ERROR_BUSY);
            goto Ret;
        }
        InterlockedIncrement(&pVKey->Inuse);

        pVTable = (PVTableStruc) pVKey->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fCritSec = TRUE;

        rt = pVKey->FuncSetKeyParam(pVTable->hProv, pVKey->hKey,
                                    dwParam, pbData, dwFlags);
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fCritSec)
        LeaveProviderCritSec(pVTable);
    return(rt);
}


/*
 -      CryptGetKeyParam
 -
 *      Purpose:
 *                Allows applications to get various aspects of the
 *                operations of a key
 *
 *      Parameters:
 *               IN      hKey       -  Handle to a key
 *               IN      dwParam    -  Parameter number
 *               IN      pbData     -  Pointer to data
 *               IN      pdwDataLen -  Length of parameter data
 *               IN      dwFlags    -  Flags values
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptGetKeyParam(IN HCRYPTKEY hKey,
			IN DWORD dwParam,
			IN BYTE *pbData,
			IN DWORD *pdwDataLen,
			IN DWORD dwFlags)
{
    PVTableStruc    pVTable;
    PVKeyStruc      pVKey;
    BOOL            rt = CRYPT_FAILED;
    BOOL            fKeyCritSec = FALSE;
    BOOL            fTableCritSec = FALSE;

    try
    {
        pVKey = (PVKeyStruc) hKey;

        if (RCRYPT_FAILED(EnterKeyCritSec(pVKey)))
        {
            goto Ret;
        }
        fKeyCritSec = TRUE;

        pVTable = (PVTableStruc) pVKey->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fTableCritSec = TRUE;

        rt = pVKey->FuncGetKeyParam(pVTable->hProv, pVKey->hKey,
                                    dwParam, pbData, pdwDataLen,
                                    dwFlags);

    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fKeyCritSec)
        LeaveKeyCritSec(pVKey);
    if (fTableCritSec)
        LeaveProviderCritSec(pVTable);
    return(rt);
}


/*
 -      CryptGenRandom
 -
 *      Purpose:
 *                Used to fill a buffer with random bytes
 *
 *
 *      Parameters:
 *               IN  hProv      -  Handle to the user identifcation
 *               IN  dwLen      -  Number of bytes of random data requested
 *               OUT pbBuffer   -  Pointer to the buffer where the random
 *                                 bytes are to be placed
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptGenRandom(IN HCRYPTPROV hProv,
		      IN DWORD dwLen,
		      OUT BYTE *pbBuffer)

{
    PVTableStruc    pVTable;
    BOOL            fTableCritSec = FALSE;
    BOOL            rt = CRYPT_FAILED;

    try
    {
        pVTable = (PVTableStruc) hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fTableCritSec = TRUE;
    
	    rt = pVTable->FuncGenRandom(pVTable->hProv, dwLen, pbBuffer);

    } except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fTableCritSec)
        LeaveProviderCritSec(pVTable);
    return(rt);
}

/*
 -      CryptGetUserKey
 -
 *      Purpose:
 *                Gets a handle to a permanent user key
 *
 *
 *      Parameters:
 *               IN  hProv      -  Handle to the user identifcation
 *               IN  dwKeySpec  -  Specification of the key to retrieve
 *               OUT phUserKey  -  Pointer to key handle of retrieved key
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptGetUserKey(IN HCRYPTPROV hProv,
                       IN DWORD dwKeySpec,
                       OUT HCRYPTKEY *phUserKey)
{

    PVTableStruc    pVTable;
    PVKeyStruc      pVKey = NULL;
    BOOL            fTableCritSec = FALSE;
    BOOL            fRet = CRYPT_FAILED;

    try
    {
        pVTable = (PVTableStruc) hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fTableCritSec = TRUE;

        if (RCRYPT_FAILED(BuildVKey(&pVKey, pVTable)))
        {
            goto Ret;
        }

        if (RCRYPT_FAILED(pVTable->FuncGetUserKey(pVTable->hProv, dwKeySpec,
                                                  phUserKey)))
        {
            goto Ret;
        }

        pVKey->hKey = *phUserKey;

        pVKey->hProv = hProv;

        *phUserKey = (HCRYPTKEY)pVKey;

        pVKey->Version = TABLEKEY;

        pVKey->Inuse = 1;

    } except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;
Ret:
    if (fTableCritSec)
        LeaveProviderCritSec(pVTable);
    if ((CRYPT_SUCCEED != fRet) && pVKey)
        LocalFree(pVKey);
    return fRet;
}



/*
 -      CryptExportKey
 -
 *      Purpose:
 *                Export cryptographic keys out of a CSP in a secure manner
 *
 *
 *      Parameters:
 *               IN  hKey       - Handle to the key to export
 *               IN  hPubKey    - Handle to the exchange public key value of
 *                                the destination user
 *               IN  dwBlobType - Type of key blob to be exported
 *               IN  dwFlags -    Flags values
 *               OUT pbData -     Key blob data
 *               OUT pdwDataLen - Length of key blob in bytes
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptExportKey(IN HCRYPTKEY hKey,
                IN HCRYPTKEY hPubKey,
                IN DWORD dwBlobType,
                IN DWORD dwFlags,
                OUT BYTE *pbData,
                OUT DWORD *pdwDataLen)
{
    PVTableStruc    pVTable;
    PVKeyStruc      pVKey;
    PVKeyStruc      pVPublicKey;
    BOOL            fKeyCritSec = FALSE;
    BOOL            fPubKeyCritSec = FALSE;
    BOOL            fTableCritSec = FALSE;
    BOOL            rt = CRYPT_FAILED;

    try
    {
        pVKey = (PVKeyStruc) hKey;

        if (RCRYPT_FAILED(EnterKeyCritSec(pVKey)))
        {
            goto Ret;
        }
        fKeyCritSec = TRUE;

        pVTable = (PVTableStruc) pVKey->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fTableCritSec = TRUE;

        pVPublicKey = (PVKeyStruc) hPubKey;

        if (pVPublicKey != NULL)
        {
            if (RCRYPT_FAILED(EnterKeyCritSec(pVPublicKey)))
            {
                goto Ret;
            }
            fPubKeyCritSec = TRUE;
        }

        rt = pVKey->FuncExportKey(pVTable->hProv, pVKey->hKey,
                                  (pVPublicKey == NULL ? 0 : pVPublicKey->hKey),
                                  dwBlobType, dwFlags, pbData,
                                  pdwDataLen);

    } except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fKeyCritSec)
        LeaveKeyCritSec(pVKey);
    if (fTableCritSec)
        LeaveProviderCritSec(pVTable);
    if (pVPublicKey != NULL)
    {
        if (fPubKeyCritSec)
            LeaveKeyCritSec(pVPublicKey);
    }
    return(rt);
}


/*
 -      CryptImportKey
 -
 *      Purpose:
 *                Import cryptographic keys
 *
 *
 *      Parameters:
 *               IN  hProv     -  Handle to the CSP user
 *               IN  pbData    -  Key blob data
 *               IN  dwDataLen -  Length of the key blob data
 *               IN  hPubKey   -  Handle to the exchange public key value of
 *                                the destination user
 *               IN  dwFlags   -  Flags values
 *               OUT phKey     -  Pointer to the handle to the key which was
 *                                Imported
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptImportKey(IN HCRYPTPROV hProv,
		      IN CONST BYTE *pbData,
		      IN DWORD dwDataLen,
		      IN HCRYPTKEY hPubKey,
		      IN DWORD dwFlags,
		      OUT HCRYPTKEY *phKey)
{
    PVTableStruc    pVTable;
    PVKeyStruc      pVKey = NULL;
    PVKeyStruc      pVPublicKey;
    BOOL            fKeyCritSec = FALSE;
    BOOL            fPubKeyCritSec = FALSE;
    BOOL            fTableCritSec = FALSE;
    BOOL            fBuiltKey = FALSE;
    BOOL            fRet = CRYPT_FAILED;

    {
        pVTable = (PVTableStruc) hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fTableCritSec = TRUE;

        pVPublicKey = (PVKeyStruc)hPubKey;

        if (pVPublicKey != NULL)
        {
            if (RCRYPT_FAILED(EnterKeyCritSec(pVPublicKey)))
            {
                goto Ret;
            }
            fPubKeyCritSec = TRUE;
        }

        if (dwFlags & CRYPT_UPDATE_KEY)
        {
            pVKey = (PVKeyStruc) phKey;

            if (RCRYPT_FAILED(EnterKeyCritSec(pVKey)))
            {
                goto Ret;
            }
            fKeyCritSec = TRUE;
        }
        else
        {
            if (RCRYPT_FAILED(BuildVKey(&pVKey, pVTable)))
            {
                goto Ret;
            }
            fBuiltKey = TRUE;
        }

        if (RCRYPT_FAILED(pVTable->FuncImportKey(pVTable->hProv, pbData,
                                                 dwDataLen,
                                                 (pVPublicKey == NULL ? 0 : pVPublicKey->hKey),
                                                 dwFlags, phKey)))
        {
            goto Ret;
	    }

	    if ((dwFlags & CRYPT_UPDATE_KEY) != CRYPT_UPDATE_KEY)
	    {
            pVKey->hKey = *phKey;

            *phKey = (HCRYPTKEY) pVKey;

            pVKey->hProv = hProv;

            pVKey->Version = TABLEKEY;
        }
    }

    fRet = CRYPT_SUCCEED;
Ret:
    if (fTableCritSec)
        LeaveProviderCritSec(pVTable);
    if (pVPublicKey != NULL)
    {
        if (fPubKeyCritSec)
            LeaveKeyCritSec(pVPublicKey);
    }
    if ((dwFlags & CRYPT_UPDATE_KEY) && fKeyCritSec)
    {
        LeaveKeyCritSec(pVKey);
    }
    else if ((CRYPT_SUCCEED != fRet) && fBuiltKey && pVKey)
    {
        LocalFree(pVKey);
    }

    return fRet;
}


/*
 -      CryptEncrypt
 -
 *      Purpose:
 *                Encrypt data
 *
 *
 *      Parameters:
 *               IN  hKey          -  Handle to the key
 *               IN  hHash         -  Optional handle to a hash
 *               IN  Final         -  Boolean indicating if this is the final
 *                                    block of plaintext
 *               IN  dwFlags       -  Flags values
 *               IN OUT pbData     -  Data to be encrypted
 *               IN OUT pdwDataLen -  Pointer to the length of the data to be
 *                                    encrypted
 *               IN dwBufLen       -  Size of Data buffer
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptEncrypt(IN HCRYPTKEY hKey,
            IN HCRYPTHASH hHash,
		    IN BOOL Final,
		    IN DWORD dwFlags,
		    IN OUT BYTE *pbData,
		    IN OUT DWORD *pdwDataLen,
		    IN DWORD dwBufLen)
{
    PVTableStruc    pVTable;
    PVKeyStruc      pVKey;
    PVHashStruc     pVHash;
    BOOL            fKeyCritSec = FALSE;
    BOOL            fTableCritSec = FALSE;
    BOOL            fHashCritSec = FALSE;
    BOOL            rt = CRYPT_FAILED;

    try
    {
        pVKey = (PVKeyStruc) hKey;

        if (RCRYPT_FAILED(EnterKeyCritSec(pVKey)))
        {
            goto Ret;
        }
        fKeyCritSec = TRUE;

        pVTable = (PVTableStruc) pVKey->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fTableCritSec = TRUE;

        pVHash = (PVHashStruc) hHash;

        if (pVHash != NULL)
        {
            if (RCRYPT_FAILED(EnterHashCritSec(pVHash)))
            {
                goto Ret;
            }
            fHashCritSec = TRUE;
        }

        rt = pVKey->FuncEncrypt(pVTable->hProv, pVKey->hKey,
                                (pVHash == NULL ? 0 : pVHash->hHash),
                                Final, dwFlags, pbData,
                                pdwDataLen, dwBufLen);

    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fTableCritSec)
        LeaveProviderCritSec(pVTable);
    if (fKeyCritSec)
        LeaveKeyCritSec(pVKey);
    if (pVHash != NULL)
    {
        if (fHashCritSec)
            LeaveHashCritSec(pVHash);
    }
    return rt;
}


/*
 -      CryptDecrypt
 -
 *      Purpose:
 *                Decrypt data
 *
 *
 *      Parameters:
 *               IN  hKey          -  Handle to the key
 *               IN  hHash         -  Optional handle to a hash
 *               IN  Final         -  Boolean indicating if this is the final
 *                                    block of ciphertext
 *               IN  dwFlags       -  Flags values
 *               IN OUT pbData     -  Data to be decrypted
 *               IN OUT pdwDataLen -  Pointer to the length of the data to be
 *                                    decrypted
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptDecrypt(IN HCRYPTKEY hKey,
		    IN HCRYPTHASH hHash,
		    IN BOOL Final,
		    IN DWORD dwFlags,
		    IN OUT BYTE *pbData,
		    IN OUT DWORD *pdwDataLen)

{
    PVTableStruc    pVTable;
    PVKeyStruc      pVKey;
    PVHashStruc     pVHash;
    BOOL            fKeyCritSec = FALSE;
    BOOL            fTableCritSec = FALSE;
    BOOL            fHashCritSec = FALSE;
    BOOL            rt = CRYPT_FAILED;

    try
    {
        pVKey = (PVKeyStruc) hKey;

        if (RCRYPT_FAILED(EnterKeyCritSec(pVKey)))
        {
            goto Ret;
        }
        fKeyCritSec = TRUE;

        pVTable = (PVTableStruc) pVKey->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fTableCritSec = TRUE;

        pVHash = (PVHashStruc) hHash;

        if (pVHash != NULL)
        {
            if (RCRYPT_FAILED(EnterHashCritSec(pVHash)))
            {
                goto Ret;
            }
            fHashCritSec = TRUE;
        }
    
        rt = pVKey->FuncDecrypt(pVTable->hProv, pVKey->hKey,
                                (pVHash == NULL ? 0 : pVHash->hHash),
                                Final, dwFlags, pbData, pdwDataLen);
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fTableCritSec)
        LeaveProviderCritSec(pVTable);
    if (fKeyCritSec)
        LeaveKeyCritSec(pVKey);
	if (pVHash != NULL)
    {
        if (fHashCritSec)
            LeaveHashCritSec(pVHash);
    }
    return(rt);
}


/*
 -      CryptCreateHash
 -
 *      Purpose:
 *                initate the hashing of a stream of data
 *
 *
 *      Parameters:
 *               IN  hProv   -  Handle to the user identifcation
 *               IN  Algid   -  Algorithm identifier of the hash algorithm
 *                              to be used
 *               IN  hKey    -  Optional key for MAC algorithms
 *               IN  dwFlags -  Flags values
 *               OUT pHash   -  Handle to hash object
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptCreateHash(IN HCRYPTPROV hProv,
		       IN ALG_ID Algid,
		       IN HCRYPTKEY hKey,
		       IN DWORD dwFlags,
		       OUT HCRYPTHASH *phHash)
{
    PVTableStruc    pVTable;
    DWORD           bufsize;
    PVKeyStruc      pVKey;
    PVHashStruc     pVHash = NULL;
    BOOL            fTableCritSec = FALSE;
    BOOL            fKeyCritSec = FALSE;
    BOOL            fRet = CRYPT_FAILED;

    try
    {
        pVTable = (PVTableStruc) hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }
        fTableCritSec = TRUE;

        pVKey = (PVKeyStruc) hKey;

        if (pVKey != NULL)
        {
            if (RCRYPT_FAILED(EnterKeyCritSec(pVKey)))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                goto Ret;
            }
            fKeyCritSec = TRUE;
        }

        bufsize = sizeof(VHashStruc);

        if (!BuildVHash(&pVHash, pVTable))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }

        if (RCRYPT_FAILED(pVTable->FuncCreateHash(pVTable->hProv, Algid,
                                                  (pVKey == NULL ? 0 : pVKey->hKey),
                                                  dwFlags, phHash)))
	    {
            goto Ret;
	    }

        pVHash->hHash = *phHash;

        *phHash = (HCRYPTHASH) pVHash;

        pVHash->Version = TABLEHASH;

        pVHash->Inuse = 1;
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;
Ret:
    if (fTableCritSec)
        LeaveProviderCritSec(pVTable);
    if (pVKey != NULL)
    {
        if (fKeyCritSec)
            LeaveKeyCritSec(pVKey);
    }
    if ((CRYPT_SUCCEED != fRet) && pVHash)
        LocalFree(pVHash);
    return fRet;
}

/*
 -      CryptDuplicateHash
 -
 *      Purpose:
 *                Duplicate a cryptographic hash
 *
 *
 *      Parameters:
 *               IN      hHash          -  Handle to the hash to be duplicated
 *               IN      pdwReserved    -  Reserved for later use
 *               IN      dwFlags        -  Flags values
 *               OUT     phHash         -  Handle to the new duplicate hash
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptDuplicateHash(
                         IN HCRYPTHASH hHash,
		                 IN DWORD *pdwReserved,
		                 IN DWORD dwFlags,
		                 OUT HCRYPTHASH * phHash
                         )
{
    PVTableStruc    pVTable;
    PVHashStruc     pVHash;
    PVHashStruc     pVNewHash = NULL;
    HCRYPTHASH      hNewHash;
    BOOL            fProvCritSecSet = FALSE;
    DWORD           dwErr;
    BOOL            fRet = CRYPT_FAILED;

    try
    {
        pVHash = (PVHashStruc) hHash;

        if (pVHash->Version != TABLEHASH)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (NULL == pVHash->OptionalFuncDuplicateHash)
        {
            SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
            goto Ret;
        }

        pVTable = (PVTableStruc) pVHash->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        fProvCritSecSet = TRUE;


        if (RCRYPT_FAILED(BuildVHash(&pVNewHash, pVTable)))
        {
            goto Ret;
        }

        if (RCRYPT_FAILED(pVHash->OptionalFuncDuplicateHash(pVTable->hProv, pVHash->hHash,
                                                          pdwReserved, dwFlags, &hNewHash)))
        {
            goto Ret;
        }

        pVNewHash->hHash = hNewHash;

        pVNewHash->Version = TABLEHASH;

        pVNewHash->hProv = pVHash->hProv;

        pVHash->Inuse = 1;

        *phHash = (HCRYPTHASH) pVNewHash;
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;
Ret:
    if (CRYPT_SUCCEED != fRet)
        dwErr = GetLastError();
    if ((fRet == CRYPT_FAILED) && (NULL != pVNewHash))
        LocalFree(pVNewHash);
    if (fProvCritSecSet)
        LeaveProviderCritSec(pVTable);
    if (CRYPT_SUCCEED != fRet)
        SetLastError(dwErr);

    return fRet;
}

/*
 -      CryptHashData
 -
 *      Purpose:
 *                Compute the cryptograghic hash on a stream of data
 *
 *
 *      Parameters:
 *               IN  hHash     -  Handle to hash object
 *               IN  pbData    -  Pointer to data to be hashed
 *               IN  dwDataLen -  Length of the data to be hashed
 *               IN  dwFlags   -  Flags values
 *
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptHashData(IN HCRYPTHASH hHash,
		     IN CONST BYTE *pbData,
		     IN DWORD dwDataLen,
		     IN DWORD dwFlags)
{
    PVTableStruc    pVTable;
    PVHashStruc     pVHash;
    BOOL            fProvCritSec = FALSE;
    BOOL            fHashCritSec = FALSE;
    DWORD           dwErr;
    BOOL            fRet = CRYPT_FAILED;

    try
    {
        pVHash = (PVHashStruc) hHash;

        if (RCRYPT_FAILED(EnterHashCritSec(pVHash)))
        {
            goto Ret;
        }
        fHashCritSec = TRUE;

        pVTable = (PVTableStruc) pVHash->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fProvCritSec = TRUE;

        if (!pVHash->FuncHashData(pVTable->hProv,
                                  pVHash->hHash,
                                  pbData, dwDataLen, dwFlags))
            goto Ret;

    } except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;
Ret:
    if (CRYPT_SUCCEED != fRet)
        dwErr = GetLastError();
    if (fHashCritSec)
        LeaveHashCritSec(pVHash);
    if (fProvCritSec)
        LeaveProviderCritSec(pVTable);
    if (CRYPT_SUCCEED != fRet)
        SetLastError(dwErr);

    return fRet;

}

/*
 -      CryptHashSessionKey
 -
 *      Purpose:
 *                Compute the cryptograghic hash on a key object
 *
 *
 *      Parameters:
 *               IN  hHash     -  Handle to hash object
 *               IN  hKey      -  Handle to a key object
 *               IN  dwFlags   -  Flags values
 *
 *      Returns:
 *               CRYPT_FAILED
 *               CRYPT_SUCCEED
 */
WINADVAPI
BOOL
WINAPI CryptHashSessionKey(IN HCRYPTHASH hHash,
			   IN  HCRYPTKEY hKey,
			   IN DWORD dwFlags)
{
    PVTableStruc    pVTable;
    PVHashStruc     pVHash;
    PVKeyStruc      pVKey;
    BOOL            fHashCritSec = FALSE;
    BOOL            fProvCritSec = FALSE;
    BOOL            fKeyCritSec = FALSE;
    BOOL            rt = CRYPT_FAILED;

    try
    {
        pVHash = (PVHashStruc) hHash;

        if (RCRYPT_FAILED(EnterHashCritSec(pVHash)))
        {
            goto Ret;
        }
        fHashCritSec = TRUE;

        pVTable = (PVTableStruc) pVHash->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fProvCritSec = TRUE;

        pVKey = (PVKeyStruc) hKey;

        if (pVKey != NULL)
        {
            if (RCRYPT_FAILED(EnterKeyCritSec(pVKey)))
            {
                goto Ret;
            }
            fKeyCritSec = TRUE;
        }

        rt = pVHash->FuncHashSessionKey(pVTable->hProv,
                                        pVHash->hHash,
                                        pVKey->hKey,
                                        dwFlags);

    } except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fHashCritSec)
        LeaveHashCritSec(pVHash);
    if (fProvCritSec)
        LeaveProviderCritSec(pVTable);
    if (pVKey != NULL)
    {
        if (fKeyCritSec)
            LeaveKeyCritSec(pVKey);
    }
    return rt;
}


/*
 -      CryptDestoryHash
 -
 *      Purpose:
 *                Destory the hash object
 *
 *
 *      Parameters:
 *               IN  hHash     -  Handle to hash object
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptDestroyHash(IN HCRYPTHASH hHash)
{
    PVTableStruc    pVTable;
    PVHashStruc     pVHash;
    BOOL            fProvCritSec = FALSE;
    BOOL            rt = CRYPT_FAILED;

    try
    {
        pVHash = (PVHashStruc) hHash;

        if (pVHash->Version != TABLEHASH)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (0 < InterlockedDecrement(&pVHash->Inuse))
        {
            InterlockedIncrement(&pVHash->Inuse);
            SetLastError(ERROR_BUSY);
            goto Ret;
        }
        InterlockedIncrement(&pVHash->Inuse);

        pVTable = (PVTableStruc) pVHash->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fProvCritSec = TRUE;

        rt = pVHash->FuncDestroyHash(pVTable->hProv, pVHash->hHash);

        pVHash->Version = 0;
        LocalFree(pVHash);
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fProvCritSec)
        LeaveProviderCritSec(pVTable);
    return rt;
}

WINADVAPI
BOOL
WINAPI LocalSignHashW(IN  HCRYPTHASH hHash,
		      IN  DWORD dwKeySpec,
		      IN  LPCWSTR sDescription,
		      IN  DWORD dwFlags,
		      OUT BYTE *pbSignature,
		      OUT DWORD *pdwSigLen)
{
    PVTableStruc    pVTable;
    PVHashStruc     pVHash;
    BOOL            fHashCritSec = FALSE;
    BOOL            fProvCritSec = FALSE;
    BOOL            rt = CRYPT_FAILED;

    try
    {
        pVHash = (PVHashStruc) hHash;

        if (RCRYPT_FAILED(EnterHashCritSec(pVHash)))
        {
            goto Ret;
        }
        fHashCritSec = TRUE;

        pVTable = (PVTableStruc) pVHash->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fProvCritSec = TRUE;

        rt = pVHash->FuncSignHash(pVTable->hProv, pVHash->hHash,
                                  dwKeySpec,
                                  sDescription, dwFlags,
                                  pbSignature, pdwSigLen);
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fHashCritSec)
        LeaveHashCritSec(pVHash);
    if (fProvCritSec)
        LeaveProviderCritSec(pVTable);
    return rt;
}


/*
 -      CryptSignHashW
 -
 *      Purpose:
 *                Create a digital signature from a hash
 *
 *
 *      Parameters:
 *               IN  hHash        -  Handle to hash object
 *               IN  dwKeySpec    -  Key pair that is used to sign with
 *                                   algorithm to be used
 *               IN  sDescription -  Description of data to be signed
 *               IN  dwFlags      -  Flags values
 *               OUT pbSignture   -  Pointer to signature data
 *               OUT pdwSigLen    -  Pointer to the len of the signature data
 *
 *      Returns:
 */
#ifndef WIN95
WINADVAPI
BOOL
WINAPI CryptSignHashW(IN  HCRYPTHASH hHash,
		      IN  DWORD dwKeySpec,
		      IN  LPCWSTR sDescription,
		      IN  DWORD dwFlags,
		      OUT BYTE *pbSignature,
		      OUT DWORD *pdwSigLen)
{
    return LocalSignHashW(hHash, dwKeySpec, sDescription,
                          dwFlags, pbSignature, pdwSigLen);
}
#else
WINADVAPI
BOOL
WINAPI CryptSignHashW(IN  HCRYPTHASH hHash,
		      IN  DWORD dwKeySpec,
		      IN  LPCWSTR sDescription,
		      IN  DWORD dwFlags,
		      OUT BYTE *pbSignature,
		      OUT DWORD *pdwSigLen)
{
    SetLastError((DWORD)ERROR_CALL_NOT_IMPLEMENTED);
    return CRYPT_FAILED;
}
#endif



WINADVAPI
BOOL
WINAPI LocalVerifySignatureW(IN HCRYPTHASH hHash,
			     IN CONST BYTE *pbSignature,
			     IN DWORD dwSigLen,
			     IN HCRYPTKEY hPubKey,
			     IN LPCWSTR sDescription,
			     IN DWORD dwFlags)
{
    PVTableStruc    pVTable;
    PVHashStruc     pVHash;
    PVKeyStruc      pVKey;
    BOOL            fHashCritSec = FALSE;
    BOOL            fProvCritSec = FALSE;
    BOOL            fKeyCritSec = FALSE;
    BOOL            rt = CRYPT_FAILED;

    try
    {
        pVHash = (PVHashStruc) hHash;

        if (RCRYPT_FAILED(EnterHashCritSec(pVHash)))
        {
            goto Ret;
        }
        fHashCritSec = TRUE;

        pVTable = (PVTableStruc) pVHash->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fProvCritSec = TRUE;

        pVKey = (PVKeyStruc) hPubKey;

        if (RCRYPT_FAILED(EnterKeyCritSec(pVKey)))
        {
            goto Ret;
        }
        fKeyCritSec = TRUE;

        rt = pVHash->FuncVerifySignature(pVTable->hProv,
                        pVHash->hHash, pbSignature,
                        dwSigLen,
                        (pVKey == NULL ? 0 : pVKey->hKey),
                        sDescription, dwFlags);
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fHashCritSec)
        LeaveHashCritSec(pVHash);
    if (fProvCritSec)
        LeaveProviderCritSec(pVTable);
    if (fKeyCritSec)
        LeaveKeyCritSec(pVKey);
    return rt;
}

/*
 -      CryptVerifySignatureW
 -
 *      Purpose:
 *                Used to verify a signature against a hash object
 *
 *
 *      Parameters:
 *               IN  hHash        -  Handle to hash object
 *               IN  pbSignture   -  Pointer to signature data
 *               IN  dwSigLen     -  Length of the signature data
 *               IN  hPubKey      -  Handle to the public key for verifying
 *                                   the signature
 *               IN  sDescription -  String describing the signed data
 *               IN  dwFlags      -  Flags values
 *
 *      Returns:
 */
#ifndef WIN95
WINADVAPI
BOOL
WINAPI CryptVerifySignatureW(IN HCRYPTHASH hHash,
			     IN CONST BYTE *pbSignature,
			     IN DWORD dwSigLen,
			     IN HCRYPTKEY hPubKey,
			     IN LPCWSTR sDescription,
			     IN DWORD dwFlags)
{
    return LocalVerifySignatureW(hHash, pbSignature, dwSigLen,
			                     hPubKey, sDescription, dwFlags);
}
#else
WINADVAPI
BOOL
WINAPI CryptVerifySignatureW(IN HCRYPTHASH hHash,
			     IN CONST BYTE *pbSignature,
			     IN DWORD dwSigLen,
			     IN HCRYPTKEY hPubKey,
			     IN LPCWSTR sDescription,
			     IN DWORD dwFlags)
{
    SetLastError((DWORD)ERROR_CALL_NOT_IMPLEMENTED);
    return CRYPT_FAILED;
}
#endif



/*
 -      CryptSetProvParam
 -
 *      Purpose:
 *                Allows applications to customize various aspects of the
 *                operations of a provider
 *
 *      Parameters:
 *               IN      hProv   -  Handle to a provider
 *               IN      dwParam -  Parameter number
 *               IN      pbData  -  Pointer to data
 *               IN      dwFlags -  Flags values
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptSetProvParam(IN HCRYPTPROV hProv,
			 IN DWORD dwParam,
			 IN BYTE *pbData,
			 IN DWORD dwFlags)
{
    PVTableStruc    pVTable;
    BYTE            *pbTmp;
    CRYPT_DATA_BLOB *pBlob;
    BOOL            rt = CRYPT_FAILED;

    try
    {
        if (dwParam == PP_CLIENT_HWND)
        {
            hWnd = *((DWORD *) pbData);
            rt = CRYPT_SUCCEED;
            goto Ret;
        }
        else if (dwParam == PP_CONTEXT_INFO)
        {
            pBlob = (CRYPT_DATA_BLOB*)pbData;

            // allocate space for the new context info
            if (NULL == (pbTmp = (BYTE*)LocalAlloc(LMEM_ZEROINIT, pBlob->cbData)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }
            memcpy(pbTmp, pBlob->pbData, pBlob->cbData);

            // free any previously allocated context info
            if (NULL != pbContextInfo)
            {
                LocalFree(pbContextInfo);
            }
            cbContextInfo = pBlob->cbData;
            pbContextInfo = pbTmp;

            rt = CRYPT_SUCCEED;
            goto Ret;
        }

        pVTable = (PVTableStruc) hProv;

        if (pVTable->Version != TABLEPROV)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (0 < InterlockedDecrement(&pVTable->Inuse))
        {
            InterlockedIncrement(&pVTable->Inuse);
            SetLastError(ERROR_BUSY);
            goto Ret;
        }
        InterlockedIncrement(&pVTable->Inuse);

        rt = pVTable->FuncSetProvParam(pVTable->hProv, dwParam, pbData,
                                       dwFlags);
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    return(rt);
}


/*
 -      CryptGetProvParam
 -
 *      Purpose:
 *                Allows applications to get various aspects of the
 *                operations of a provider
 *
 *      Parameters:
 *               IN      hProv      -  Handle to a proivder
 *               IN      dwParam    -  Parameter number
 *               IN      pbData     -  Pointer to data
 *               IN      pdwDataLen -  Length of parameter data
 *               IN      dwFlags    -  Flags values
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptGetProvParam(IN HCRYPTPROV hProv,
			 IN DWORD dwParam,
			 IN BYTE *pbData,
			 IN DWORD *pdwDataLen,
			 IN DWORD dwFlags)
{
    PVTableStruc    pVTable;
    BOOL            fProvCritSec = FALSE;
    BOOL            rt = CRYPT_FAILED;

    try
    {
        pVTable = (PVTableStruc) hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fProvCritSec = TRUE;

        rt = pVTable->FuncGetProvParam(pVTable->hProv, dwParam, pbData,
                                       pdwDataLen, dwFlags);
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fProvCritSec)
        LeaveProviderCritSec(pVTable);
    return rt;
}


/*
 -      CryptSetHashParam
 -
 *      Purpose:
 *                Allows applications to customize various aspects of the
 *                operations of a hash
 *
 *      Parameters:
 *               IN      hHash   -  Handle to a hash
 *               IN      dwParam -  Parameter number
 *               IN      pbData  -  Pointer to data
 *               IN      dwFlags -  Flags values
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptSetHashParam(IN HCRYPTHASH hHash,
			 IN DWORD dwParam,
			 IN BYTE *pbData,
			 IN DWORD dwFlags)
{
    PVTableStruc    pVTable;
    PVHashStruc     pVHash;
    BOOL            fProvCritSec = FALSE;
    BOOL            rt = CRYPT_FAILED;

    try
    {
        pVHash = (PVHashStruc) hHash;

        if (pVHash->Version != TABLEHASH)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (0 < InterlockedDecrement(&pVHash->Inuse))
        {
            InterlockedIncrement(&pVHash->Inuse);
            SetLastError(ERROR_BUSY);
            goto Ret;
        }
        InterlockedIncrement(&pVHash->Inuse);

        pVTable = (PVTableStruc) pVHash->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fProvCritSec = TRUE;

        rt = pVHash->FuncSetHashParam(pVTable->hProv, pVHash->hHash,
                                      dwParam, pbData, dwFlags);
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fProvCritSec)
        LeaveProviderCritSec(pVTable);
    return rt;
}


/*
 -      CryptGetHashParam
 -
 *      Purpose:
 *                Allows applications to get various aspects of the
 *                operations of a hash
 *
 *      Parameters:
 *               IN      hHash      -  Handle to a hash
 *               IN      dwParam    -  Parameter number
 *               IN      pbData     -  Pointer to data
 *               IN      pdwDataLen -  Length of parameter data
 *               IN      dwFlags    -  Flags values
 *
 *      Returns:
 */
WINADVAPI
BOOL
WINAPI CryptGetHashParam(IN HCRYPTKEY hHash,
			 IN DWORD dwParam,
			 IN BYTE *pbData,
			 IN DWORD *pdwDataLen,
			 IN DWORD dwFlags)
{
    PVTableStruc    pVTable;
    PVHashStruc     pVHash;
    BOOL            fHashCritSec = FALSE;
    BOOL            fProvCritSec = FALSE;
    BOOL            rt = CRYPT_FAILED;

    try
    {
        pVHash = (PVHashStruc) hHash;

        if (RCRYPT_FAILED(EnterHashCritSec(pVHash)))
        {
            goto Ret;
        }
        fHashCritSec = TRUE;

        pVTable = (PVTableStruc) pVHash->hProv;

        if (RCRYPT_FAILED(EnterProviderCritSec(pVTable)))
        {
            goto Ret;
        }
        fProvCritSec = TRUE;

        rt = pVHash->FuncGetHashParam(pVTable->hProv, pVHash->hHash,
                                      dwParam, pbData, pdwDataLen,
                                      dwFlags);
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fHashCritSec)
        LeaveHashCritSec(pVHash);
    if (fProvCritSec)
        LeaveProviderCritSec(pVTable);
    return rt;
}



/*
 -      CryptSetProviderW
 -
 *      Purpose:
 *                Set a cryptography provider
 *
 *
 *      Parameters:
 *
 *                IN  pszProvName    - Name of the provider to install
 *                IN  dwProvType     - Type of the provider to install
 *
 *      Returns:
 *               BOOL
 *               Use get extended error information use GetLastError
 */
WINADVAPI
BOOL
WINAPI CryptSetProviderW(IN LPCWSTR pszProvName,
			 IN DWORD  dwProvType)
{
#ifndef _XBOX
    HKEY        hKey = 0;
    LONG        err;
    DWORD       dwIgn;
    DWORD       cbValue;
    WCHAR        *pszValue = NULL;
    WCHAR       typebuf[9]; // sfield = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    BOOL        fRet = CRYPT_FAILED;

    try
    {
        if (dwProvType == 0 || dwProvType > 999 || pszProvName == NULL)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        cbValue = (lstrlenW(pszProvName) + 1 ) * sizeof(WCHAR);

        if ((pszValue = (WCHAR *) LocalAlloc(LMEM_ZEROINIT,
                        sizeof(szusertypeW) + ((5 + 1) * sizeof(WCHAR))
                        )) == NULL)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }

        lstrcpyW(pszValue, szusertypeW);
        __ltoaW(dwProvType, typebuf);
        lstrcatW(pszValue, &typebuf[5]);

        if ((err = RegCreateKeyExW(HKEY_CURRENT_USER,
                        (const WCHAR *) pszValue,
                        0L, L"", REG_OPTION_NON_VOLATILE,
                        KEY_READ | KEY_WRITE, NULL, &hKey,
                        &dwIgn)) != ERROR_SUCCESS)
        {
            SetLastError(err);
            goto Ret;
        }

        RegCloseKey(HKEY_CURRENT_USER);

        if ((err = RegSetValueExW(hKey, L"Name", 0L, REG_SZ,
                        (const LPBYTE) pszProvName,
                        cbValue)) != ERROR_SUCCESS)
        {
            SetLastError(err);
            goto Ret;
        }

    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;
Ret:
    if (pszValue)
        LocalFree(pszValue);
    if (hKey)
        RegCloseKey(hKey);

    return fRet;
#else
	return TRUE;
#endif
}


/*
 -      CryptSetProviderExW
 -
 *      Purpose:
 *                Set the cryptographic provider as the default
 *                either for machine or for user.
 *
 *
 *      Parameters:
 *
 *                IN  pszProvName    - Name of the provider to install
 *                IN  dwProvType     - Type of the provider to install
 *                IN  pdwReserved    - Reserved for future use
 *                IN  dwFlags        - Flags parameter (for machine or for user)
 *
 *      Returns:
 *               BOOL
 *               Use get extended error information use GetLastError
 */
WINADVAPI
BOOL
WINAPI CryptSetProviderExW(
                           IN LPCWSTR pszProvName,
                           IN DWORD dwProvType,
                           IN DWORD *pdwReserved,
                           IN DWORD dwFlags
                           )
{
#ifndef _XBOX
    HKEY        hRegKey = 0;
    LONG        err;
    DWORD       dwDisp;
    DWORD       cbValue;
    WCHAR        *pszValue = NULL;
    WCHAR        *pszFullName = NULL;
    DWORD       cbFullName;
    WCHAR        typebuf[9]; // sfield: = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    DWORD       dwKeyType;
    DWORD       dw;
    DWORD       cbProvType;
    BOOL        fRet = CRYPT_FAILED;

    try
    {
        if ((dwProvType == 0) || (dwProvType > 999) ||
            (pszProvName == NULL) || (pdwReserved != NULL))
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if ((dwFlags & ~(CRYPT_MACHINE_DEFAULT | CRYPT_USER_DEFAULT | CRYPT_DELETE_DEFAULT)) ||
            ((dwFlags & CRYPT_MACHINE_DEFAULT) && (dwFlags & CRYPT_USER_DEFAULT)))
        {
            SetLastError((DWORD)NTE_BAD_FLAGS);
            goto Ret;
        }

        cbValue = (lstrlenW(pszProvName) + 1) * sizeof(WCHAR);

        // check if the CSP has been installed
        cbFullName = cbValue + sizeof(szenumprovidersW) + sizeof(WCHAR);

        if (NULL == (pszFullName = (WCHAR *) LocalAlloc(LMEM_ZEROINIT, cbFullName)))
	    {
		    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		    goto Ret;
	    }

        lstrcpyW(pszFullName, szenumprovidersW);
        pszFullName[(sizeof(szenumprovidersW) / sizeof(WCHAR)) - 1] = L'\\';
        lstrcpyW((LPWSTR)((LPBYTE)pszFullName + sizeof(szenumprovidersW)), pszProvName);

        if ((err = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                        (const WCHAR *) pszFullName,
                        0L, KEY_READ, &hRegKey)) != ERROR_SUCCESS)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        cbProvType = sizeof(dw);
        if (ERROR_SUCCESS != (err = RegQueryValueExW(hRegKey,
                                                    (const WCHAR *) L"Type",
                                                    NULL, &dwKeyType, (BYTE*)&dw,
                                                    &cbProvType)))
        {
            SetLastError(err);
            goto Ret;
        }
        if (dwProvType != dw)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (ERROR_SUCCESS != (err = RegCloseKey(hRegKey)))
        {
            SetLastError(err);
            goto Ret;
        }

	    if (dwFlags & CRYPT_MACHINE_DEFAULT)
	    {
            if ((pszValue = (WCHAR *) LocalAlloc(LMEM_ZEROINIT,
                            (lstrlenW(szmachinetypeW) + 5 + 1) * sizeof(WCHAR) )) == NULL)
		    {
			    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			    goto Ret;
		    }

            lstrcpyW(pszValue, szmachinetypeW);
            __ltoaW(dwProvType, typebuf);
            lstrcatW(pszValue, &typebuf[5]);

            if ((err = RegCreateKeyExW(HKEY_LOCAL_MACHINE,
                            (const WCHAR *) pszValue,
                            0L, NULL, REG_OPTION_NON_VOLATILE,
                            KEY_READ | KEY_WRITE, NULL, &hRegKey, &dwDisp)) != ERROR_SUCCESS)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                goto Ret;
            }

            // check the delete flag
            if (dwFlags & CRYPT_DELETE_DEFAULT)
            {
                if (ERROR_SUCCESS != (err = RegDeleteKeyW(HKEY_LOCAL_MACHINE,
                                                         (const WCHAR *)pszValue)))
                {
                    SetLastError(err);
                    goto Ret;
                }
                fRet = CRYPT_SUCCEED;
                goto Ret;
            }
	    }
	    else if (dwFlags & CRYPT_USER_DEFAULT)
	    {
            if ((pszValue = (WCHAR *) LocalAlloc(LMEM_ZEROINIT,
                            (lstrlenW(szusertypeW) + 5 + 1) * sizeof(WCHAR) )) == NULL)
		    {
			    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			    goto Ret;
		    }

            lstrcpyW(pszValue, szusertypeW);
            __ltoaW(dwProvType, typebuf);
            lstrcatW(pszValue, &typebuf[5]);

            if ((err = RegCreateKeyExW(HKEY_CURRENT_USER,
                            (const WCHAR *) pszValue,
                            0L, NULL, REG_OPTION_NON_VOLATILE,
                            KEY_READ | KEY_WRITE, NULL, &hRegKey, &dwDisp)) != ERROR_SUCCESS)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                goto Ret;
            }


            // check the delete flag
            if (dwFlags & CRYPT_DELETE_DEFAULT)
            {
                if (ERROR_SUCCESS != (err = RegDeleteKeyW(HKEY_CURRENT_USER,
                                                         (const WCHAR *)pszValue)))
                {
                    RegCloseKey(HKEY_CURRENT_USER);
                    SetLastError(err);
                    goto Ret;
                }
                fRet = CRYPT_SUCCEED;
                RegCloseKey(HKEY_CURRENT_USER);
                goto Ret;
            }
            RegCloseKey(HKEY_CURRENT_USER);
	    }

        if (ERROR_SUCCESS != (err = RegSetValueExW(hRegKey, L"Name", 0L, REG_SZ,
                                                  (const LPBYTE) pszProvName, cbValue)))
	    {
		    SetLastError(err);
		    goto Ret;
	    }
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

	fRet = CRYPT_SUCCEED;
Ret:
    if (pszFullName)
        LocalFree(pszFullName);
    if (pszValue)
        LocalFree(pszValue);
    if (hRegKey)
        RegCloseKey(hRegKey);
    return fRet;
#else
	return TRUE;
#endif
}

/*
 -      CryptGetDefaultProviderW
 -
 *      Purpose:
 *                Get the default cryptographic provider of the specified
 *                type for either the machine or for the user.
 *
 *
 *      Parameters:
 *                IN  dwProvType     - Type of the provider to install
 *                IN  pdwReserved    - Reserved for future use
 *                IN  dwFlags        - Flags parameter (for machine or for user)
 *                OUT pszProvName    - Name of the default provider
 *                IN OUT pcbProvName - Length in bytes of the provider name
 *                                     including the NULL terminator
 *
 *      Returns:
 *               BOOL
 *               Use get extended error information use GetLastError
 */
WINAPI CryptGetDefaultProviderW(
                                IN DWORD dwProvType,
                                IN DWORD *pdwReserved,
                                IN DWORD dwFlags,
                                OUT LPWSTR pszProvName,
                                IN OUT DWORD *pcbProvName
                                )
{
#ifndef _XBOX
    HKEY        hRegKey = 0;
    LONG        err;
    WCHAR        *pszValue = NULL;
    DWORD       dwValType;
    WCHAR        typebuf[9]; // sfield = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    DWORD       cbProvName = 0;
    BOOL        fRet = CRYPT_FAILED;

    try
    {
        if (dwProvType == 0 || dwProvType > 999 || pdwReserved != NULL)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if ((dwFlags & ~(CRYPT_MACHINE_DEFAULT | CRYPT_USER_DEFAULT)) ||
            ((dwFlags & CRYPT_MACHINE_DEFAULT) && (dwFlags & CRYPT_USER_DEFAULT)))
        {
            SetLastError((DWORD)NTE_BAD_FLAGS);
            goto Ret;
        }

        if (dwFlags & CRYPT_MACHINE_DEFAULT)
        {
            if ((pszValue = (WCHAR *) LocalAlloc(LMEM_ZEROINIT,
                            sizeof(szmachinetypeW) + ((5 + 1) * sizeof(WCHAR)))) == NULL)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            lstrcpyW(pszValue, szmachinetypeW);
            __ltoaW(dwProvType, typebuf);
            lstrcatW(pszValue, &typebuf[5]);

            if ((err = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                            (const WCHAR *) pszValue,
                            0L, KEY_READ, &hRegKey)) != ERROR_SUCCESS)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                goto Ret;
            }
        }
        else
        {
            if ((pszValue = (WCHAR *) LocalAlloc(LMEM_ZEROINIT,
                            sizeof(szusertypeW) + ((5 + 1) * sizeof(WCHAR)))) == NULL)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            lstrcpyW(pszValue, szusertypeW);
            __ltoaW(dwProvType, typebuf);
            lstrcatW(pszValue, &typebuf[5]);

            if ((err = RegOpenKeyExW(HKEY_CURRENT_USER,
                            (const WCHAR *) pszValue,
                            0L, KEY_READ, &hRegKey)) != ERROR_SUCCESS)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                goto Ret;
            }
            RegCloseKey(HKEY_CURRENT_USER);
        }

        if ((err = RegQueryValueExW(hRegKey, L"Name", 0L, &dwValType,
                        NULL,
                        &cbProvName)) != ERROR_SUCCESS)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (NULL == pszProvName)
        {
            fRet = CRYPT_SUCCEED;
            goto Ret;
        }

        if (cbProvName > *pcbProvName)
        {
            SetLastError(ERROR_MORE_DATA);
            goto Ret;
        }

        if ((err = RegQueryValueExW(hRegKey, L"Name", 0L, &dwValType,
                        (BYTE*)pszProvName,
                        &cbProvName)) != ERROR_SUCCESS)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;

Ret:
    *pcbProvName = cbProvName;
    if (hRegKey)
        RegCloseKey(hRegKey);
    if (pszValue)
        LocalFree(pszValue);
    return fRet;
#else
	return TRUE;
#endif
}


/*
 -      CryptEnumProviderTypesW
 -
 *      Purpose:
 *                Enumerate the provider types.
 *
 *      Parameters:
 *                IN  dwIndex        - Index to the provider types to enumerate
 *                IN  pdwReserved    - Reserved for future use
 *                IN  dwFlags        - Flags parameter
 *                OUT pdwProvType    - Pointer to the provider type
 *                OUT pszTypeName    - Name of the enumerated provider type
 *                IN OUT pcbTypeName - Length of the enumerated provider type
 *
 *      Returns:
 *               BOOL
 *               Use get extended error information use GetLastError
 */
WINADVAPI
BOOL
WINAPI CryptEnumProviderTypesW(
                               IN DWORD dwIndex,
                               IN DWORD *pdwReserved,
                               IN DWORD dwFlags,
                               OUT DWORD *pdwProvType,
                               OUT LPWSTR pszTypeName,
                               IN OUT DWORD *pcbTypeName
                               )
{
#ifndef _XBOX
    HKEY        hRegKey = 0;
    LONG        err;
    WCHAR        *pszRegKeyName = NULL;
    DWORD       cbClass;
    FILETIME    ft;
    WCHAR        rgcType[] = {L'T', L'y', L'p', L'e', L' '};
    LPWSTR       pszValue;
    long        Type;
    DWORD       cSubKeys;
    DWORD       cbMaxKeyName;
    DWORD       cbMaxClass;
    DWORD       cValues;
    DWORD       cbMaxValName;
    DWORD       cbMaxValData;
    BOOL        fRet = CRYPT_FAILED;

    try
    {
        if (0 != dwFlags)
        {
            SetLastError((DWORD)NTE_BAD_FLAGS);
            goto Ret;
        }

        if (NULL != pdwReserved)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Ret;
        }

        if (NULL != pcbTypeName)
        {
            *pcbTypeName = 0;   // we never return type names
        }

        if (ERROR_SUCCESS != (err = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                                                 (const WCHAR *) szprovidertypesW,
                                                 0L,
                                                 KEY_READ,
                                                 &hRegKey)))
        {
            SetLastError(err);
            goto Ret;
        }

        if (ERROR_SUCCESS != (err = RegQueryInfoKeyW(hRegKey,
                                                    NULL,
                                                    &cbClass,
                                                    NULL,
                                                    &cSubKeys,
                                                    &cbMaxKeyName,
                                                    &cbMaxClass,
                                                    &cValues,
                                                    &cbMaxValName,
                                                    &cbMaxValData,
                                                    NULL,
                                                    &ft)))
        {
            SetLastError(err);
            goto Ret;
        }
        ++cbMaxKeyName;     // allow for trailing NULL
        if (NULL == (pszRegKeyName = LocalAlloc(LMEM_ZEROINIT, cbMaxKeyName*sizeof(WCHAR))))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }

        if (ERROR_SUCCESS != (err = RegEnumKeyExW(hRegKey,
                                                 dwIndex, pszRegKeyName, &cbMaxKeyName, NULL,
                                                 NULL, NULL, &ft)))
        {
            SetLastError(err);
            goto Ret;
        }

        if (memcmp(pszRegKeyName, rgcType, sizeof(rgcType)))
        {
            SetLastError((DWORD)NTE_FAIL);
            goto Ret;
        }
        pszValue = pszRegKeyName + sizeof(rgcType)/sizeof(TCHAR);

        if (0 == (Type = StrToLW(pszValue)))
        {
            SetLastError((DWORD)NTE_FAIL);
            goto Ret;
        }
        *pdwProvType = (DWORD)Type;
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;
Ret:
    if (hRegKey)
        RegCloseKey(hRegKey);
    if (pszRegKeyName)
        LocalFree(pszRegKeyName);
    return fRet;
#else
	return TRUE;
#endif
}


/*
 -      CryptEnumProvidersW
 -
 *      Purpose:
 *                Enumerate the providers.
 *
 *      Parameters:
 *                IN  dwIndex        - Index to the providers to enumerate
 *                IN  pdwReserved    - Reserved for future use
 *                IN  dwFlags        - Flags parameter
 *                OUT pdwProvType    - The type of the provider
 *                OUT pszProvName    - Name of the enumerated provider
 *                IN OUT pcbProvName - Length of the enumerated provider
 *
 *      Returns:
 *               BOOL
 *               Use get extended error information use GetLastError
 */
WINADVAPI
BOOL
WINAPI CryptEnumProvidersW(
                           IN DWORD dwIndex,
                           IN DWORD *pdwReserved,
                           IN DWORD dwFlags,
                           OUT DWORD *pdwProvType,
                           OUT LPWSTR pszProvName,
                           IN OUT DWORD *pcbProvName
                           )
{
#ifndef _XBOX
    HKEY        hRegKey = 0;
    HKEY        hProvRegKey = 0;
    LONG        err;
    DWORD       cbClass;
    FILETIME    ft;
    DWORD       dwKeyType;
    DWORD       cbProvType;
    DWORD       dw;
    DWORD       cSubKeys;
    DWORD       cbMaxKeyName;
    DWORD       cbMaxClass;
    DWORD       cValues;
    DWORD       cbMaxValName;
    DWORD       cbMaxValData;
    LPWSTR       pszTmpProvName = NULL;
    DWORD       cbTmpProvName;
    BOOL        fRet = CRYPT_FAILED;

    try
    {
        if (0 != dwFlags)
        {
            SetLastError((DWORD)NTE_BAD_FLAGS);
            goto Ret;
        }

        if (ERROR_SUCCESS != (err = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                                                 (const WCHAR *) szenumprovidersW,
                                                 0L, KEY_READ, &hRegKey)))
        {
            SetLastError((DWORD)NTE_FAIL);
            goto Ret;
        }

        if (ERROR_SUCCESS != (err = RegQueryInfoKeyW(hRegKey,
                                                    NULL,
                                                    &cbClass,
                                                    NULL,
                                                    &cSubKeys,
                                                    &cbMaxKeyName,
                                                    &cbMaxClass,
                                                    &cValues,
                                                    &cbMaxValName,
                                                    &cbMaxValData,
                                                    NULL,
                                                    &ft)))
        {
            SetLastError((DWORD)NTE_FAIL);
            goto Ret;
        }

        ++cbMaxKeyName;

        if (NULL == (pszTmpProvName = LocalAlloc(LMEM_ZEROINIT, cbMaxKeyName*sizeof(WCHAR))))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }

        if (ERROR_SUCCESS != (err = RegEnumKeyExW(hRegKey, dwIndex, pszTmpProvName,
                                                 &cbMaxKeyName, NULL,
                                                 NULL, NULL, &ft)))
        {
            SetLastError(err);
            goto Ret;
        }

        if (ERROR_SUCCESS != (err = RegOpenKeyExW(hRegKey,
                                                 (const WCHAR *) pszTmpProvName,
                                                 0L, KEY_READ, &hProvRegKey)))
        {
            SetLastError((DWORD)NTE_FAIL);
            goto Ret;
        }

        cbProvType = sizeof(dw);
        if (ERROR_SUCCESS != (err = RegQueryValueExW(hProvRegKey,
                                                    (const WCHAR *) L"Type",
                                                    NULL, &dwKeyType, (BYTE*)&dw,
                                                    &cbProvType)))
        {
            SetLastError(err);
            goto Ret;
        }
        *pdwProvType = dw;

        // SFIELD: review if cbProvname should be number of chars rather
        // than bytes.
        cbTmpProvName = (lstrlenW(pszTmpProvName) + 1) * sizeof(WCHAR);

        if (NULL != pszProvName)
        {
            if (*pcbProvName < cbTmpProvName)
            {
                *pcbProvName = cbTmpProvName;
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }
            lstrcpyW(pszProvName, pszTmpProvName);
        }

        *pcbProvName = cbTmpProvName;
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    fRet = CRYPT_SUCCEED;
Ret:
    if (pszTmpProvName)
        LocalFree(pszTmpProvName);
    if (hRegKey)
        RegCloseKey(hRegKey);
    if (hProvRegKey)
        RegCloseKey(hProvRegKey);
    return fRet;
#else
	return TRUE;
#endif
}

BOOL EnterProviderCritSec(IN PVTableStruc pVTable)
{
    try
    {
        if (pVTable->Version != TABLEPROV)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Try_Error_Return;
        }

        InterlockedIncrement(&pVTable->Inuse);

    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Try_Error_Return;
    }

    return(CRYPT_SUCCEED);
Try_Error_Return:
    return(CRYPT_FAILED);
}


void LeaveProviderCritSec(IN PVTableStruc pVTable)
{
    InterlockedDecrement(&pVTable->Inuse);
}

BOOL EnterKeyCritSec(IN PVKeyStruc pVKey)
{

    try
    {
        if (pVKey->Version != TABLEKEY)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Try_Error_Return;
        }

        InterlockedIncrement(&pVKey->Inuse);
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Try_Error_Return;
    }

    return(CRYPT_SUCCEED);
Try_Error_Return:
    return(CRYPT_FAILED);

}


void LeaveKeyCritSec(IN PVKeyStruc pVKey)
{
    InterlockedDecrement(&pVKey->Inuse);
}

BOOL EnterHashCritSec(IN PVHashStruc pVHash)
{

    try
    {
        if (pVHash->Version != TABLEHASH)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            goto Try_Error_Return;
        }

        InterlockedIncrement(&pVHash->Inuse);
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Try_Error_Return;
    }

    return(CRYPT_SUCCEED);

Try_Error_Return:
    return(CRYPT_FAILED);

}


void LeaveHashCritSec(IN PVHashStruc pVHash)
{
    InterlockedDecrement(&pVHash->Inuse);
}


BOOL BuildVKey(IN PVKeyStruc *ppVKey,
               IN PVTableStruc pVTable)
{
    DWORD           bufsize;
    PVKeyStruc pVKey;

    bufsize = sizeof(VKeyStruc);

    if ((pVKey = (PVKeyStruc) LocalAlloc(LMEM_ZEROINIT,
                                         (UINT) bufsize)) == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return(CRYPT_FAILED);
    }

    pVKey->FuncGenKey = pVTable->FuncGenKey;
    pVKey->FuncDeriveKey = pVTable->FuncDeriveKey;
    pVKey->FuncDestroyKey = pVTable->FuncDestroyKey;
    pVKey->FuncSetKeyParam = pVTable->FuncSetKeyParam;
    pVKey->FuncGetKeyParam = pVTable->FuncGetKeyParam;
    pVKey->FuncExportKey = pVTable->FuncExportKey;
    pVKey->FuncImportKey = pVTable->FuncImportKey;
    pVKey->FuncEncrypt = pVTable->FuncEncrypt;
    pVKey->FuncDecrypt = pVTable->FuncDecrypt;

    pVKey->OptionalFuncDuplicateKey = pVTable->OptionalFuncDuplicateKey;

    pVKey->hProv = pVTable->hProv;

    *ppVKey = pVKey;

    return(CRYPT_SUCCEED);
}

BOOL BuildVHash(
                IN PVHashStruc *ppVHash,
                IN PVTableStruc pVTable
                )
{
    DWORD           bufsize;
    PVHashStruc     pVHash;


    bufsize = sizeof(VHashStruc);

    if ((pVHash = (PVHashStruc) LocalAlloc(LMEM_ZEROINIT, (UINT) bufsize)) == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return(CRYPT_FAILED);
    }

    pVHash->FuncCreateHash = pVTable->FuncCreateHash;
    pVHash->FuncHashData = pVTable->FuncHashData;
    pVHash->FuncHashSessionKey = pVTable->FuncHashSessionKey;
    pVHash->FuncDestroyHash = pVTable->FuncDestroyHash;
    pVHash->FuncSignHash = pVTable->FuncSignHash;
    pVHash->FuncVerifySignature = pVTable->FuncVerifySignature;
    pVHash->FuncGetHashParam = pVTable->FuncGetHashParam;
    pVHash->FuncSetHashParam = pVTable->FuncSetHashParam;

    pVHash->OptionalFuncDuplicateHash = pVTable->OptionalFuncDuplicateHash;

    pVHash->hProv = (HCRYPTPROV)pVTable;

    *ppVHash = pVHash;

    return(CRYPT_SUCCEED);
}

void EncryptKey(BYTE *pdata, DWORD size, BYTE val)
{
    RC4_KEYSTRUCT key;
    BYTE          RealKey[RC4_KEYSIZE] = {0xa2, 0x17, 0x9c, 0x98, 0xca};
    DWORD         index;

    for (index = 0; index < RC4_KEYSIZE; index++)
    {
        RealKey[index] = RealKey[index] ^ val;
    }

    rc4_key(&key, RC4_KEYSIZE, RealKey);

    rc4(&key, size, pdata);

}

void MD5HashData(
                 BYTE *pb,
                 DWORD cb,
                 BYTE *pbHash
                 )
{
    MD5_CTX     HashState;

    MD5Init(&HashState);

    try
    {
        MD5Update(&HashState, pb, cb);
    } except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError((DWORD) NTE_SIGNATURE_FILE_BAD);
        return;
    }

    // Finish the hash
    MD5Final(&HashState);

    memcpy(pbHash, HashState.digest, 16);
}


BOOL CheckSignature(
                    BYTE *pbKey,
                    DWORD cbKey,
                    BYTE *pbSig,
                    DWORD cbSig,
                    BYTE *pbHash,
                    BOOL fUnknownLen)
{
    BYTE                rgbResult[KEYSIZE1024];
    BYTE                rgbSig[KEYSIZE1024];
    BYTE                rgbKey[sizeof(BSAFE_PUB_KEY) + KEYSIZE1024];
    BYTE                rgbKeyHash[16];
    BYTE                *pbSecondKey;
    DWORD               cbSecondKey;
    BYTE                *pbKeySig;
    PSECOND_TIER_SIG    pSecondTierSig;
    LPBSAFE_PUB_KEY     pTmp;
    BOOL                fRet = FALSE;

    memset(rgbResult, 0, KEYSIZE1024);
    memset(rgbSig, 0, KEYSIZE1024);

    // just check the straight signature if version is 1
    pTmp = (LPBSAFE_PUB_KEY)pbKey;

    // check if sig length is the same as the key length
    if (fUnknownLen || (cbSig == pTmp->keylen))
    {
        memcpy(rgbSig, pbSig, pTmp->keylen);
        BSafeEncPublic(pTmp, rgbSig, rgbResult);

        if (RtlEqualMemory(pbHash, rgbResult, 16) &&
            rgbResult[cbKey-1] == 0 &&
            rgbResult[cbKey-2] == 1 &&
            rgbResult[16] == 0)
        {
            fRet = TRUE;
            goto Ret;
        }
    }

    // check the the second tier signature if the magic equals 2
    pSecondTierSig = (PSECOND_TIER_SIG)pbSig;
    if (0x00000002 != pSecondTierSig->dwMagic)
        goto Ret;

    if (0x31415352 != pSecondTierSig->Pub.magic)
        goto Ret;

    // assign the pointers
    cbSecondKey = sizeof(BSAFE_PUB_KEY) + pSecondTierSig->Pub.keylen;
    pbSecondKey = pbSig + (sizeof(SECOND_TIER_SIG) - sizeof(BSAFE_PUB_KEY));
    pbKeySig = pbSecondKey + cbSecondKey;

    // hash the second tier key
    MD5HashData(pbSecondKey, cbSecondKey, rgbKeyHash);

    // Decrypt the signature data on the second tier key
    memset(rgbResult, 0, sizeof(rgbResult));
    memset(rgbSig, 0, sizeof(rgbSig));
    memcpy(rgbSig, pbKeySig, pSecondTierSig->cbSig);
    BSafeEncPublic(pTmp, rgbSig, rgbResult);

    if ((FALSE == RtlEqualMemory(rgbKeyHash, rgbResult, 16)) ||
        rgbResult[cbKey-1] != 0 ||
        rgbResult[cbKey-2] != 1 ||
        rgbResult[16] != 0)
    {
        goto Ret;
    }

    // Decrypt the signature data on the CSP
    memset(rgbResult, 0, sizeof(rgbResult));
    memset(rgbSig, 0, sizeof(rgbSig));
    memset(rgbKey, 0, sizeof(rgbKey));
    memcpy(rgbSig, pbKeySig + pSecondTierSig->cbSig, pSecondTierSig->cbSig);
    memcpy(rgbKey, pbSecondKey, cbSecondKey);
    pTmp = (LPBSAFE_PUB_KEY)rgbKey;
    BSafeEncPublic(pTmp, rgbSig, rgbResult);

    if (RtlEqualMemory(pbHash, rgbResult, 16) &&
        rgbResult[pTmp->keylen-1] == 0 &&
        rgbResult[pTmp->keylen-2] == 1 &&
        rgbResult[16] == 0)
    {
        fRet = TRUE;
    }
Ret:
    return fRet;
}

/*
 -      NewVerifyImage
 -
 *      Purpose:
 *                Check signature of file
 *
 *
 *      Parameters:
 *                IN lpszImage      - address of file
 *                IN pSigData       - address of signature data
 *                IN cbSig          - length of signature data
 *                IN fUnknownLen    - BOOL to tell if length is not passed in
 *
 *      Returns:
 *                BOOL
 */
BOOL NewVerifyImage(LPCWSTR lpszImage,
		            BYTE *pSigData,
                    DWORD cbSig,
                    BOOL fUnknownLen)
{
#ifndef _XBOX

    HANDLE      hFileProv = INVALID_HANDLE_VALUE;
    DWORD       NumBytes;
    DWORD       lpdwFileSizeHigh;
    LPVOID      lpvAddress = NULL;
    DWORD       NumBytesRead;
    BYTE        rgbHash[16];
    BYTE        rgbKey[sizeof(BSAFE_PUB_KEY) + KEYSIZE1024];
    BYTE        rgbKey2[sizeof(BSAFE_PUB_KEY) + KEYSIZE1024];
#ifdef MS_INTERNAL_KEY
    BYTE        rgbMSKey[sizeof(BSAFE_PUB_KEY) + KEYSIZE1024];
#endif

#ifdef TEST_BUILD_EXPONENT
    BYTE        rgbTestKey[sizeof(BSAFE_PUB_KEY) + KEYSIZE512];
#endif

    BOOL        fRet = CRYPT_FAILED;

    if (INVALID_HANDLE_VALUE == (hFileProv = CreateFileW(
    			lpszImage,
    			GENERIC_READ,
    			FILE_SHARE_READ,
    			NULL,
    			OPEN_EXISTING,
    			0,
    			NULL)))
    {
        SetLastError((DWORD) NTE_PROV_DLL_NOT_FOUND);
        goto Ret;
    }

    if (0xffffffff == (NumBytes = GetFileSize(hFileProv,
                                              &lpdwFileSizeHigh)))
    {
        SetLastError((DWORD) NTE_SIGNATURE_FILE_BAD);
        goto Ret;
    }

    if (NULL == (lpvAddress = VirtualAlloc(NULL, NumBytes, MEM_RESERVE | MEM_COMMIT,
                                           PAGE_READWRITE)))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    if (!ReadFile((HANDLE) hFileProv, lpvAddress, NumBytes, &NumBytesRead, 0))
    {
        SetLastError((DWORD) NTE_SIGNATURE_FILE_BAD);
        goto Ret;
    }

    MD5HashData(lpvAddress, NumBytes, rgbHash);

    // decrypt the keys once for each process
    memcpy(rgbKey, (BYTE*)&KEY, sizeof(BSAFE_PUB_KEY) + KEYSIZE1024);
    EncryptKey(rgbKey, sizeof(BSAFE_PUB_KEY) + KEYSIZE1024, 0);

#ifdef MS_INTERNAL_KEY
    memcpy(rgbMSKey, (BYTE*)&MSKEY, sizeof(BSAFE_PUB_KEY) + KEYSIZE1024);
    EncryptKey(rgbMSKey, sizeof(BSAFE_PUB_KEY) + KEYSIZE1024, 1);
#endif
    memcpy(rgbKey2, (BYTE*)&KEY2, sizeof(BSAFE_PUB_KEY) + KEYSIZE1024);
    EncryptKey(rgbKey2, sizeof(BSAFE_PUB_KEY) + KEYSIZE1024, 2);

#ifdef TEST_BUILD_EXPONENT
    memcpy(rgbTestKey, (BYTE*)&TESTKEY, sizeof(BSAFE_PUB_KEY) + KEYSIZE512);
    EncryptKey(rgbTestKey, sizeof(BSAFE_PUB_KEY) + KEYSIZE512, 3);
#endif // TEST_BUILD_EXPONENT

    if (CRYPT_SUCCEED == (fRet = CheckSignature(rgbKey, 128, pSigData,
                                                cbSig, rgbHash, fUnknownLen)))
        goto Ret;

#ifdef MS_INTERNAL_KEY
    if (CRYPT_SUCCEED == (fRet = CheckSignature(rgbMSKey, 128, pSigData,
                                                cbSig, rgbHash, fUnknownLen)))
        goto Ret;
#endif

    if (CRYPT_SUCCEED == (fRet = CheckSignature(rgbKey2, 128, pSigData,
                                                cbSig, rgbHash, fUnknownLen)))
        goto Ret;

#ifdef TEST_BUILD_EXPONENT
    if (CRYPT_SUCCEED == (fRet = CheckSignature(rgbTestKey, 64, pSigData,
                                                cbSig, rgbHash, fUnknownLen)))
        goto Ret;
#endif // TEST_BUILD_EXPONENT

    SetLastError((DWORD) NTE_BAD_SIGNATURE);
Ret:
    if (NULL != lpvAddress)
        VirtualFree(lpvAddress, 0, MEM_RELEASE);
    if (INVALID_HANDLE_VALUE != hFileProv)
        CloseHandle(hFileProv);

    return fRet;
#else
	ASSERT(!"cryptapi.c called");
	return TRUE;
#endif
}

/*
 -      CProvVerifyImage
 -
 *      Purpose:
 *                Check signature of file
 *
 *
 *      Parameters:
 *                IN lpszImage      - address of file
 *                IN lpSigData      - address of signature data
 *
 *      Returns:
 *                BOOL
 */
BOOL CProvVerifyImage(LPCWSTR lpszImage,
		              BYTE *pSigData)
{
    return NewVerifyImage(lpszImage, pSigData, 0, TRUE);
}


/*
 -      CPReturnhWnd
 -
 *      Purpose:
 *                Return a window handle back to a CSP
 *
 *
 *      Parameters:
 *                OUT phWnd      - pointer to a hWnd to return
 *
 *      Returns:
 *                BOOL
 */
BOOL CPReturnhWnd(DWORD *phWnd)
{
    try
    {

        *phWnd = hWnd;

    } except ( EXCEPTION_EXECUTE_HANDLER )
    {
	SetLastError(ERROR_INVALID_PARAMETER);
	goto Try_Error_Return;
    }

    return(CRYPT_SUCCEED);

Try_Error_Return:
    return(CRYPT_FAILED);

}

static void __ltoaW(DWORD val, WCHAR *buf)
{
    WCHAR *p;            /* pointer to traverse string */
    WCHAR *firstdig;     /* pointer to first digit */
    WCHAR temp;          /* temp char */
    unsigned digval;    /* value of digit */
    int  i;

    p = buf;

    firstdig = p;       /* save pointer to first digit */

    for (i = 0; i < 8; i++) {
        digval = (unsigned) (val % 10);
        val /= 10;      /* get next digit */

        /* convert to ascii and store */
        *p++ = (WCHAR) (digval + L'0');    /* a digit */
    }

    /* We now have the digit of the number in the buffer, but in reverse
       order.  Thus we reverse them now. */

    *p-- = L'\0';                /* terminate string; p points to last digit */

    do {
        temp = *p;
        *p = *firstdig;
        *firstdig = temp;       /* swap *p and *firstdig */
        --p;
        ++firstdig;             /* advance to next two digits */
    } while (firstdig < p); /* repeat until halfway */
}

#define FRENCHCHECKKEY  L"Comm\\Security\\Crypto\\Defaults\\CheckInfo"
#define FRENCHCHECKVALUE  L"Mask"

#define DH_PROV_ENABLED     1
#define RSA_PROV_ENABLED    2
#define DH_SCH_ENABLED      4
#define RSA_SCH_ENABLED     8

BOOL WINAPI
IsEncryptionPermitted(
                           IN DWORD dwProvType
                           )
/*++

Routine Description:

    This routine checks whether encryption is getting the system default
    LCID and checking whether the country code is CTRY_FRANCE.

Arguments:

    none


Return Value:

    TRUE - encryption is permitted
    FALSE - encryption is not permitted


--*/

{
#define NO_FRANCE_CHECK // encryption permitted in France as of 2/4/2000
#ifndef NO_FRANCE_CHECK
    LCID    DefaultLcid;
    WCHAR CountryCode[10];
    LPCWSTR CountryFrance = L"33"; // CTRY_FRANCE converted to Unicode string
    HKEY    hKey = 0;
    DWORD   cb = sizeof(DWORD);
    DWORD   dw = 0;
    DWORD   dwType;
    BOOL    fRet = FALSE;

    //*pfInFrance = FALSE;

    DefaultLcid = GetSystemDefaultLCID();

    //
    // Check if the default language is Standard French
    // or if the users's country is set to FRANCE
    //

    if (GetLocaleInfoW(DefaultLcid,LOCALE_ICOUNTRY,CountryCode,10) == 0)
    {
        goto Ret;
    }
    if ((memcmp(CountryCode, CountryFrance, sizeof(CountryFrance)) == 0)
        || (LANGIDFROMLCID(DefaultLcid) == 0x40c))
    {
        // this is a check to see if a registry key to enable encryption is
        // available, do not remove, use or publicize this check without
        // thorough discussions with Microsoft Legal handling French Import
        // issues (tomalb and/or irar)
	    if (ERROR_SUCCESS != RegOpenKeyExW(HKEY_LOCAL_MACHINE,
		                                  FRENCHCHECKKEY,
		                                  0,		// dwOptions
		                                  KEY_READ,
		                                  &hKey))
        {
            hKey = 0;
            goto Ret;
        }

        // get the mask value from the registry indicating which crypto
        // services are to be allowed
	    if (ERROR_SUCCESS != RegQueryValueExW(hKey,
		                                  FRENCHCHECKVALUE,
		                                  NULL, &dwType,		// dwOptions
		                                  (BYTE*)&dw,
		                                  &cb))
        {
            goto Ret;
        }

        switch(dwProvType)
        {
            case PROV_RSA_FULL:
            case PROV_RSA_SIG:
            {
                if (dw & RSA_PROV_ENABLED)
                {
                    fRet = TRUE;
                }
                break;
            }

            case PROV_RSA_SCHANNEL:
            {
                if (dw & RSA_SCH_ENABLED)
                {
                    fRet = TRUE;
                }
                break;
            }

            case PROV_DSS:
            case PROV_DSS_DH:
            {
                if (dw & DH_PROV_ENABLED)
                {
                    fRet = TRUE;
                }
                break;
            }

#ifdef PROV_DH_SCHANNEL
            case PROV_DH_SCHANNEL:
            {
                if (dw & DH_SCH_ENABLED)
                {
                    fRet = TRUE;
                }
                break;
            }
#endif
            default:
                goto Ret;
        }
    }
    else
    {
        fRet = TRUE;
    }
Ret:
    if (hKey)
        RegCloseKey(hKey);
    return fRet;
#else
    //*pfInFrance = FALSE;
    return TRUE;
#endif
}

