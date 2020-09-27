//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1995.
//
//  File:       mgmt.c
//
//  Contents:   Management Functions
//
//  Classes:
//
//  Functions:
//
//  History:    8-07-95   RichardW   Created
//
//----------------------------------------------------------------------------

#include <spbase.h>
#include <security.h>
#include "spsspi.h"

#include "hack.h"

#define lstrcmpiA stricmp

const SecPkgInfoA SecPackagesA[] =
{
    {
	SECPKG_FLAG_INTEGRITY |
	    SECPKG_FLAG_PRIVACY |
	    SECPKG_FLAG_CONNECTION |
	    SECPKG_FLAG_STREAM |
	    SECPKG_FLAG_MULTI_REQUIRED |
        SECPKG_FLAG_IMPERSONATION |
	    SECPKG_FLAG_EXTENDED_ERROR,
	1,
	UNISP_RPC_ID,
	768,
	UNISP_NAME_A,
	"Microsoft Unified Security Provider"
    },
#ifdef SCHANNEL_PCT
    {
	SECPKG_FLAG_INTEGRITY |
	    SECPKG_FLAG_PRIVACY |
	    SECPKG_FLAG_CONNECTION |
	    SECPKG_FLAG_STREAM |
	    SECPKG_FLAG_MULTI_REQUIRED |
        SECPKG_FLAG_IMPERSONATION |
	    SECPKG_FLAG_EXTENDED_ERROR,
	1,
	UNISP_RPC_ID,
	768,
	PCT1SP_NAME_A,
	"Microsoft PCT 1.0  Security Provider"
    },
#endif
    {
	SECPKG_FLAG_INTEGRITY |
	    SECPKG_FLAG_PRIVACY |
	    SECPKG_FLAG_CONNECTION |
	    SECPKG_FLAG_STREAM |
	    SECPKG_FLAG_MULTI_REQUIRED |
        SECPKG_FLAG_IMPERSONATION |
	    SECPKG_FLAG_EXTENDED_ERROR,
	1,
	UNISP_RPC_ID,
	768,
	SSL3SP_NAME_A,
	"Microsoft SSL 3.0 Security Provider"
    },
    {
	SECPKG_FLAG_INTEGRITY |
	    SECPKG_FLAG_PRIVACY |
	    SECPKG_FLAG_CONNECTION |
	    SECPKG_FLAG_STREAM |
	    SECPKG_FLAG_MULTI_REQUIRED |
        SECPKG_FLAG_IMPERSONATION |
	    SECPKG_FLAG_EXTENDED_ERROR,
	1,
	UNISP_RPC_ID,
	768,
	SSL2SP_NAME_A,
	"Microsoft SSL 2.0 Security Provider"
    }
};



const SecPkgInfoW SecPackagesW[] =
{
    {
	SECPKG_FLAG_INTEGRITY |
	    SECPKG_FLAG_PRIVACY |
	    SECPKG_FLAG_CONNECTION |
	    SECPKG_FLAG_STREAM |
	    SECPKG_FLAG_MULTI_REQUIRED |
        SECPKG_FLAG_IMPERSONATION |
	    SECPKG_FLAG_EXTENDED_ERROR,
	1,
	UNISP_RPC_ID,
	768,
	UNISP_NAME_W,
	L"Microsoft Unified Security Provider"
    },
#ifdef SCHANNEL_PCT
    {
	SECPKG_FLAG_INTEGRITY |
	    SECPKG_FLAG_PRIVACY |
	    SECPKG_FLAG_CONNECTION |
	    SECPKG_FLAG_STREAM |
	    SECPKG_FLAG_MULTI_REQUIRED |
        SECPKG_FLAG_IMPERSONATION |
	    SECPKG_FLAG_EXTENDED_ERROR,
	1,
	UNISP_RPC_ID,
	768,
	PCT1SP_NAME_W,
	L"Microsoft PCT 1.0  Security Provider"
    },
#endif
    {
	SECPKG_FLAG_INTEGRITY |
	    SECPKG_FLAG_PRIVACY |
	    SECPKG_FLAG_CONNECTION |
	    SECPKG_FLAG_STREAM |
	    SECPKG_FLAG_MULTI_REQUIRED |
        SECPKG_FLAG_IMPERSONATION |
	    SECPKG_FLAG_EXTENDED_ERROR,
	1,
	UNISP_RPC_ID,
	768,
	SSL3SP_NAME_W,
	L"Microsoft SSL 3.0 Security Provider"
    },
    {
	SECPKG_FLAG_INTEGRITY |
	    SECPKG_FLAG_PRIVACY |
	    SECPKG_FLAG_CONNECTION |
	    SECPKG_FLAG_STREAM |
	    SECPKG_FLAG_MULTI_REQUIRED |
        SECPKG_FLAG_IMPERSONATION |
	    SECPKG_FLAG_EXTENDED_ERROR,
	1,
	UNISP_RPC_ID,
	768,
	SSL2SP_NAME_W,
	L"Microsoft SSL 2.0 Security Provider"
    }
};

const DWORD NumPackages = sizeof(SecPackagesW)/sizeof(SecPkgInfoW);


DWORD pGetInfoSizeW(const SecPkgInfoW * pInfo)
{
    return(
	sizeof(SecPkgInfoW) +
	(lstrlenW(pInfo->Name) + lstrlenW(pInfo->Comment) + 2) * sizeof(WCHAR));

}


DWORD pGetInfoSizeA(const SecPkgInfoA * pInfo)
{
    return(
	sizeof(SecPkgInfoA) +
	strlen(pInfo->Name) +
	strlen(pInfo->Comment) +
	2);
}


SECURITY_STATUS SEC_ENTRY
EnumerateSecurityPackagesW(
    unsigned long SEC_FAR *     pcPackages,         // Receives num. packages
    PSecPkgInfoW SEC_FAR *       ppPackageInfo       // Receives array of info
    )
{
    DWORD i;
    DWORD dwTotalSize;
    PWSTR pPack;
    PSecPkgInfoW pCurInfo;
    *ppPackageInfo = NULL;
    *pcPackages = 0;

    /* get size */
    dwTotalSize = 0;
    for(i=0; i<NumPackages; i++) {
        dwTotalSize += pGetInfoSizeW(&SecPackagesW[i]);
    }
    *ppPackageInfo = SPExternalAlloc(dwTotalSize);
    if(*ppPackageInfo == NULL) return(SEC_E_INSUFFICIENT_MEMORY);

    pPack = (PWSTR)&(*ppPackageInfo)[NumPackages];
    for(i=0; i<NumPackages; i++) {
        pCurInfo = &(*ppPackageInfo)[i];

        CopyMemory(pCurInfo, &SecPackagesW[i], sizeof(SecPkgInfoW));
        pCurInfo->Name = (PWSTR)pPack;
        lstrcpyW(pCurInfo->Name, SecPackagesW[i].Name);
        pPack += (lstrlenW(SecPackagesW[i].Name) + 1);

        pCurInfo->Comment = (PWSTR)pPack;
        lstrcpyW(pCurInfo->Comment, SecPackagesW[i].Comment);
        pPack += (lstrlenW(SecPackagesW[i].Comment) + 1);
    }

    *pcPackages = NumPackages;
    return(SEC_E_OK);

}

SECURITY_STATUS SEC_ENTRY
EnumerateSecurityPackagesA(
    unsigned long SEC_FAR *     pcPackages,         // Receives num. packages
    PSecPkgInfoA SEC_FAR *       ppPackageInfo       // Receives array of info
    )
{
    DWORD i;
    DWORD dwTotalSize;
    PUCHAR pPack;
    PSecPkgInfoA pCurInfo;
    *ppPackageInfo = NULL;
    *pcPackages = 0;

    /* get size */
    dwTotalSize = 0;
    for(i=0; i<NumPackages; i++) {
        dwTotalSize += pGetInfoSizeA(&SecPackagesA[i]);
    }
    *ppPackageInfo = SPExternalAlloc(dwTotalSize);
    if(*ppPackageInfo == NULL) return(SEC_E_INSUFFICIENT_MEMORY);

    pPack = (PUCHAR)&(*ppPackageInfo)[NumPackages];
    for(i=0; i<NumPackages; i++) {
        pCurInfo = &(*ppPackageInfo)[i];

        CopyMemory(pCurInfo, &SecPackagesA[i], sizeof(SecPkgInfoA));
        pCurInfo->Name = (PSTR)pPack;
        strcpy(pCurInfo->Name, SecPackagesA[i].Name);
        pPack += (strlen(SecPackagesA[i].Name) + 1);

        pCurInfo->Comment = (PSTR)pPack;
        strcpy(pCurInfo->Comment, SecPackagesA[i].Comment);
        pPack += (strlen(SecPackagesA[i].Comment) + 1);
    }

    *pcPackages = NumPackages;
    return(SEC_E_OK);

}


SECURITY_STATUS SEC_ENTRY
QuerySecurityPackageInfoW(
    SEC_WCHAR SEC_FAR *         pszPackageName,     // Name of package
    PSecPkgInfoW *               ppPackageInfo       // Receives package info
    )
{
    DWORD i;
    PWCHAR pPack;
    PSecPkgInfoW pCurInfo;

    for (i = 0; i < NumPackages; i++)
    {
        if (lstrcmpiW(pszPackageName, SecPackagesW[i].Name) == 0)
        {
            break;
        }
    }
    if (i == NumPackages)
    {
        return(SEC_E_SECPKG_NOT_FOUND);
    }
    pCurInfo = SPExternalAlloc(pGetInfoSizeW(&SecPackagesW[i]));
    if(pCurInfo == NULL) return(SEC_E_INSUFFICIENT_MEMORY);
    pPack = (PWSTR)(pCurInfo+1);

    CopyMemory(pCurInfo, &SecPackagesW[i], sizeof(SecPkgInfoW));
    pCurInfo->Name = (PWSTR)pPack;
    lstrcpyW(pCurInfo->Name, SecPackagesW[i].Name);
    pPack += (lstrlenW(SecPackagesW[i].Name) + 1);

    pCurInfo->Comment = (PWSTR)pPack;
    lstrcpyW(pCurInfo->Comment, SecPackagesW[i].Comment);
    *ppPackageInfo = pCurInfo;
    return(SEC_E_OK);
}


SECURITY_STATUS SEC_ENTRY
QuerySecurityPackageInfoA(
    SEC_CHAR SEC_FAR *         pszPackageName,     // Name of package
    PSecPkgInfoA *               ppPackageInfo     // Receives package info
    )
{
    DWORD i;
    PCHAR pPack;
    PSecPkgInfoA pCurInfo;

    for(i=0; i< NumPackages; i++) {
        if (!lstrcmpiA(pszPackageName, SecPackagesA[i].Name)) break;
    }
    if (i == NumPackages)
    {
        return(SEC_E_SECPKG_NOT_FOUND);
    }
    pCurInfo = SPExternalAlloc(pGetInfoSizeA(&SecPackagesA[i]));
    if(pCurInfo == NULL) return(SEC_E_INSUFFICIENT_MEMORY);
    pPack = (PUCHAR)(pCurInfo+1);

    CopyMemory(pCurInfo, &SecPackagesA[i], sizeof(SecPkgInfoA));
    pCurInfo->Name = (PSTR)pPack;
    strcpy(pCurInfo->Name, SecPackagesA[i].Name);
    pPack += (strlen(SecPackagesA[i].Name) + 1);

    pCurInfo->Comment = (PSTR)pPack;
    strcpy(pCurInfo->Comment, SecPackagesA[i].Comment);
    *ppPackageInfo = pCurInfo;
    return(SEC_E_OK);
}
