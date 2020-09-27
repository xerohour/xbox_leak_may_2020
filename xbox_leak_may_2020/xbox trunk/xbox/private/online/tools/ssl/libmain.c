//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1995.
//
//  File:       pctsspi.c
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    8-01-95   RichardW   Created
//              8-13-95   TerenceS   Mutated to PCT
//
//----------------------------------------------------------------------------

#include <spbase.h>
#include <security.h>
#include <certmap.h>
#include "spsspi.h"

#ifndef XBOX
CRITICAL_SECTION    csSsl;
#endif
BOOLEAN             CryptoOk;

DWORD g_ProtSupported = SP_PROT_ALL;

#if DBG
DWORD csSslOwner;

DWORD   csPctOwner;

#endif

#ifdef DEBUG

DBGPARAM dpCurSettings =
    {
        TEXT("Schannel"),
        {
            TEXT("Init"),       TEXT("ServerCache"),
            TEXT("RNG"),        TEXT("CertMgmt"),
            TEXT("Undefined"),  TEXT("Undefined"),
            TEXT("Undefined"),  TEXT("Undefined"),
            TEXT("Undefined"),  TEXT("Undefined"),
            TEXT("Undefined"),  TEXT("Undefined"),
            TEXT("Undefined"),  TEXT("Undefined"),
            TEXT("Warning"),    TEXT("Error")
        },
        0x0000c000
    };

char aszDebugDump[512];
void    CESPDebugLog(long fDebug, const char *szFormat, ...)
{
#if 0   // vsprintf not in all projects
    va_list paramlist;
    va_start(paramlist,szFormat);
    vsprintf(aszDebugDump,szFormat,paramlist);
    DEBUGMSG(fDebug,(TEXT("%a"),aszDebugDump));
#endif
}


#endif // DEBUG

// MyStrToL
//      Can't use CRT routines, so steal from the C runtime sources

DWORD MyStrToL(CHAR *InStr)
{
	DWORD dwVal = 0;

	while(*InStr)
	{
		dwVal = (10 * dwVal) + (*InStr - '0');
		InStr++;
	}

	return dwVal;
}

BOOLEAN
IsEncryptionPermitted(VOID)
/*++

Routine Description:

    This routine checks whether encryption is getting the system default
    LCID and checking whether the country code is CTRY_FRANCE.

Arguments:

    none


Return Value:

    TRUE - encryption is permitted
    FALSE - encryption is not permitted

    Encryption is always permitted! New France govt regulation...allow SSL, but
    disable PCT.

--*/

{
#ifdef SCHANNEL_PCT
    LCID  DefaultLcid;
    WCHAR CountryCode[10];
    CHAR  CountryCodeA[10];
    ULONG CountryValue;
    BOOL  fPCTAllowed = TRUE;

    DefaultLcid = GetSystemDefaultLCID();

    //
    // Check if the default language is Standard French or the user's country
    // is set to France.
    //

    if (LANGIDFROMLCID(DefaultLcid) == 0x40c)
    {
        fPCTAllowed = FALSE;
        goto done;
    }

    //
    // Check if the users's country is set to FRANCE
    //

    if (GetLocaleInfoW(DefaultLcid,LOCALE_ICOUNTRY,CountryCode,10) == 0)
    {
        fPCTAllowed = FALSE;
        goto done;
    }

    wcstombs(CountryCodeA, CountryCode, wcslen(CountryCode));

    CountryValue = (ULONG) MyStrToL(CountryCodeA);

    if (CountryValue == CTRY_FRANCE)
    {
        fPCTAllowed = FALSE;
        goto done;
    }

done:

    //
    // If PCT is NOT allowed and it is set, disable it and disable it in the
    // registry.
    //

    if ((fPCTAllowed == FALSE) &&
        (g_ProtEnabled & SP_PROT_PCT1)
        )
    {
        BOOL  fVal = 0; // Disabled.
        HKEY  hKey;
        DWORD dwDisp;
        DWORD err;

        g_ProtEnabled   &= ~SP_PROT_PCT1;
        g_ProtSupported &= ~SP_PROT_PCT1;

        err = RegCreateKeyEx(
            g_hkProtocols,              // Currently open protocol key.
            SP_REG_KEY_PCT1 TEXT("\\") SP_REG_KEY_CLIENT, // Key to open/create.
            0,                          // Must be 0.
            TEXT(""),                   // Class of object.
            REG_OPTION_NON_VOLATILE,    // Not used.
            KEY_ALL_ACCESS,             // Desired access. Not used.
            NULL,                       // Security attributes. Not used.
            &hKey,                      // Receives new/open key.
            &dwDisp                     // Receives disposition. (Opened | Created).
            );

        if (err == 0)
        {
            RegSetValueEx(
                hKey,               // Current open key.
                SP_REG_VAL_ENABLED, // Value "Enabled".
                0,                  // Must be 0.
                REG_DWORD,          // Key type.
                (PUCHAR)&fVal,      // Data.
                sizeof(fVal)        // Size of data.
                );

            RegCloseKey(hKey);
        }

        err = RegCreateKeyEx(
            g_hkProtocols,
            SP_REG_KEY_PCT1 TEXT("\\") SP_REG_KEY_SERVER,
            0,
            TEXT(""),
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            NULL,
            &hKey,
            &dwDisp
            );

        if (err == 0)
        {
            RegSetValueEx(
                hKey,
                SP_REG_VAL_ENABLED,
                0,
                REG_DWORD,
                (PUCHAR)&fVal,
                sizeof(fVal)
                );

            RegCloseKey(hKey);
        }
    }
#else
// no PCT in the build, no special checks needed
    g_ProtEnabled   &= ~SP_PROT_PCT1;
    g_ProtSupported &= ~SP_PROT_PCT1;
#endif // SCHANNEL_PCT
    

    // Always return success -- new French govt policy.
    return(TRUE);
}


BOOL
WINAPI
InitializeSSL()
{
#ifdef ENABLE_SELECTIVE_CRYPTO
    SPEnableFinanceCipher();
#endif

    InitializeCACache(0);

    SPInitSessionCache();
//  FBuildSsl3ExchgKeys();
    CryptoOk = IsEncryptionPermitted();

    return(TRUE);
}



BOOL WINAPI SPReInit()
{
    return TRUE;
}

#define SECUR32_VER_MS 0x00040000
#define SECUR32_VER_LS 0x00000800
#define SECUR32_FILENAME TEXT("secur32.dll")
#define SCHANNEL_FILENAME TEXT("schannel.dll")
#define VERSION_FILENAME  TEXT("version.dll")
#define SP_REG_KEY_SECURITY_PROVIDERS     TEXT("System\\CurrentControlSet\\Control\\SecurityProviders")
#define SP_REG_VAL_SECURITY_PROVIDERS   TEXT("SecurityProviders")
#define SECUR32_PATH_SIZE  256

#ifdef UNICODE
typedef DWORD
( APIENTRY * GET_FILE_VERSION_INFO_SIZE_FN)(
	LPWSTR lptstrFilename,
	LPDWORD lpdwHandle
	);
#define GET_FILE_VERSION_INFO_SIZE_NAME TEXT("GetFileVersionInfoSizeW")

typedef DWORD
( APIENTRY * GET_FILE_VERSION_INFO_FN)(
	LPWSTR lptstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData
	);
#define GET_FILE_VERSION_INFO_NAME TEXT("GetFileVersionInfoW")

typedef BOOL
( APIENTRY * VER_QUERY_VALUE_FN)(
	const LPVOID pBlock,
	LPWSTR lpSubBlock,
	LPVOID * lplpBuffer,
	PUINT puLen
	);
#define VER_QUERY_VALUE_NAME TEXT("VerQueryValueW")
#else
typedef DWORD
( APIENTRY * GET_FILE_VERSION_INFO_SIZE_FN)(
	LPSTR lptstrFilename,
	LPDWORD lpdwHandle
	);
#define GET_FILE_VERSION_INFO_SIZE_NAME TEXT("GetFileVersionInfoSizeA")
typedef DWORD
( APIENTRY * GET_FILE_VERSION_INFO_FN)(
	LPSTR lptstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData
	);
#define GET_FILE_VERSION_INFO_NAME TEXT("GetFileVersionInfoA")

typedef BOOL
( APIENTRY * VER_QUERY_VALUE_FN)(
	const LPVOID pBlock,
	LPSTR lpSubBlock,
	LPVOID * lplpBuffer,
	PUINT puLen
	);

#define VER_QUERY_VALUE_NAME TEXT("VerQueryValueA")

#endif // !UNICODE

DWORD
( APIENTRY * GET_FILE_VERSION_INFO_SIZE_FNA)(
	LPSTR lptstrFilename,
	LPDWORD lpdwHandle
	);


STDAPI DllRegisterServer(VOID)
{
#if 0
    DWORD  dwHandle;
    LPVOID pInfo;
    DWORD dwInfo;
    DWORD       err;
    DWORD       disp;
    DWORD       dwType;
    DWORD       dwSize;
    LPTSTR      pszProviders;
    LPTSTR      pszNewProviders;
    HKEY  hkSecurityProviders;
    BOOL  fComma;
    HINSTANCE   hVersion;
    GET_FILE_VERSION_INFO_SIZE_FN pGetFileVersionInfoSize = NULL;
    GET_FILE_VERSION_INFO_FN pGetFileVersionInfo = NULL;
    VER_QUERY_VALUE_FN pVerQueryValue = NULL;

    TCHAR       szSecur32Path[SECUR32_PATH_SIZE];

    LPTSTR      pszCurSource;
    LPTSTR      pszCurDest;
    DWORD       i, cSchannel;

    VS_FIXEDFILEINFO *pFixedInfo;
    UINT cbFixedInfo;

    SPLoadRegOptions();
    // Fix up the SecurityProviders key if we are
    // running an old version of secur32.dll

    hVersion = LoadLibrary(VERSION_FILENAME);
    if(hVersion == NULL)
    {
	return S_OK;
    }
    pGetFileVersionInfoSize = (GET_FILE_VERSION_INFO_SIZE_FN)GetProcAddress(hVersion, GET_FILE_VERSION_INFO_SIZE_NAME);
    if(pGetFileVersionInfoSize == NULL)
    {
	return S_OK;
    }

    pGetFileVersionInfo = (GET_FILE_VERSION_INFO_FN)GetProcAddress(hVersion, GET_FILE_VERSION_INFO_NAME);
    if(pGetFileVersionInfo == NULL)
    {
	return S_OK;
    }

    pVerQueryValue = (VER_QUERY_VALUE_FN)GetProcAddress(hVersion, VER_QUERY_VALUE_NAME);
    if(pVerQueryValue == NULL)
    {
	return S_OK;
    }
    GetSystemDirectory(szSecur32Path, SECUR32_PATH_SIZE);
    lstrcat(szSecur32Path, "\\");
    lstrcat(szSecur32Path, SECUR32_FILENAME);
    dwInfo = (*pGetFileVersionInfoSize)(szSecur32Path, &dwHandle);
    if(dwInfo == 0)
    {
	return S_OK;
    }
    pInfo = SPExternalAlloc(dwInfo);

    if(!(*pGetFileVersionInfo)(szSecur32Path, dwHandle, dwInfo, pInfo))
    {
	SPExternalFree(pInfo);
	return S_OK;
    }

    if(!(*pVerQueryValue)(pInfo, TEXT("\\"), &pFixedInfo, &cbFixedInfo))
    {
	SPExternalFree(pInfo);
	return S_OK;
    }

    if(pFixedInfo->dwFileVersionMS > SECUR32_VER_MS)
    {
	SPExternalFree(pInfo);
	return S_OK;
    }

    if(pFixedInfo->dwFileVersionMS == SECUR32_VER_MS && pFixedInfo->dwFileVersionLS > SECUR32_VER_LS)
    {
	SPExternalFree(pInfo);
	return S_OK;
    }
    SPExternalFree(pInfo);

    // We have an old version of SECUR32.DLL, so we must remove
    // schannel.dll from the security providers key.

    err = RegCreateKeyEx(   HKEY_LOCAL_MACHINE,
			    SP_REG_KEY_SECURITY_PROVIDERS,
			    0,
			    TEXT(""),
			    REG_OPTION_NON_VOLATILE,
			    KEY_ALL_ACCESS,
			    NULL,
			    &hkSecurityProviders,
			    &disp);

    if (err)
    {
	return( S_OK );
    }

    dwSize = 0;
    err = RegQueryValueEx(hkSecurityProviders,
			  SP_REG_VAL_SECURITY_PROVIDERS,
			  NULL,
			  &dwType,
			  NULL,
			  &dwSize);
    if(err) {
	return S_OK;
    }
    pszProviders = SPExternalAlloc(dwSize);

    err = RegQueryValueEx(hkSecurityProviders,
			  SP_REG_VAL_SECURITY_PROVIDERS,
			  NULL,
			  &dwType,
			  pszProviders,
			  &dwSize);


    if(err)
    {
	SPExternalFree(pszProviders);
	return S_OK;
    }
    // Now, parse through our list of providers, and
    // remove schannel.dll

    pszNewProviders = SPExternalAlloc(dwSize);

    pszCurSource = pszProviders;
    pszCurDest = pszNewProviders;

    *pszCurDest = 0;

    cSchannel = (DWORD)lstrlen(SCHANNEL_FILENAME);
    while(*pszCurSource != 0)
    {
	// Scan pszCurSource until either a comma or
	// a null.
	i = 0;
	while(pszCurSource[i] != 0 &&
	      pszCurSource[i] != TEXT(','))
	{
	    i++;
	}
	fComma = (pszCurSource[i] == TEXT(','));
	// pszCurSource points to start of dll path,
	// i is the length of the dll path
	pszCurSource[i] = 0;
	if(i < cSchannel ||
	    (lstrcmpi(&pszCurSource[i-cSchannel],
		     SCHANNEL_FILENAME) != 0))
	{
	    if(pszCurDest != pszNewProviders)
	    {
		*pszCurDest++ = TEXT(',');
	    }
	    CopyMemory(pszCurDest, pszCurSource, i);
	    pszCurDest[i]=0;
	    pszCurDest += i;
	}
	pszCurSource += i;
	if(fComma)
	{
	    pszCurSource++;
	}
    }

    err = RegSetValueEx(hkSecurityProviders,
			SP_REG_VAL_SECURITY_PROVIDERS,
			0,
			dwType,
			pszNewProviders,
			lstrlen(pszNewProviders)+1);
    SPExternalFree(pszProviders);
    SPExternalFree(pszNewProviders);
    RegCloseKey(hkSecurityProviders);

#endif // UNDER_CE

    return S_OK;

}

STDAPI DllUnregisterServer(VOID)
{
    return S_OK;
}
