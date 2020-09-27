/////////////////////////////////////////////////////////////////////////////
//  FILE          : protstor.c                                             //
//  DESCRIPTION   : Code for storing keys in the protected store:          //
//  AUTHOR        :                                                        //
//  HISTORY       :                                                        //
//      Dec  4 1996 jeffspel Created                                       //
//      Apr 21 1997 jeffspel  Changes for NT 5 tree                        //
//      Jul 28 1997 jeffspel Added ability to delete a persisted key       //
//                                                                         //
//  Copyright (C) 1993 Microsoft Corporation   All Rights Reserved         //
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"
#include "wincrypt.h"
#include "pstore.h"
#include "protstor.h"
#include "ntagum.h"


typedef HRESULT PSCREATEINST(
    IPStore **ppProvider,
    PST_PROVIDERID *pProviderID,
    void *pReserved,
    DWORD dwFlags);

#define CRYPTO_KEY_TYPE_STRING       L"Cryptographic Keys"
static GUID DefTypeGuid = {0x4d1fa410,
                    0x6fd9,
                    0x11d0,
                    {0x8C, 0x58, 0x00, 0xC0, 0x4F, 0xD9, 0x12, 0x6B}};

#define CRYPTO_SIG_SUBTYPE_STRING    L"RSA Signature Keys"
static GUID DefSigGuid = {0x4d1fa411,
                    0x6fd9,
                    0x11D0,
                    {0x8C, 0x58, 0x00, 0xC0, 0x4F, 0xD9, 0x12, 0x6B}};

#define CRYPTO_EXCH_SUBTYPE_STRING   L"RSA Exchange Keys"
static GUID DefExchGuid = {0x4d1fa412,
                    0x6fd9,
                    0x11D0,
                    {0x8C, 0x58, 0x00, 0xC0, 0x4F, 0xD9, 0x12, 0x6B}};

static GUID SysProv = MS_BASE_PSTPROVIDER_ID;

void FreePSInfo(
                PSTORE_INFO *pPStore
                )
{
    IPStore     *pIPS = (IPStore*)pPStore->pProv;

    if (pPStore)
    {
        if (pIPS)
            pIPS->Release();
#ifndef _XBOX
        if (pPStore->hInst)
            FreeLibrary(pPStore->hInst);
#endif
        if (pPStore->szPrompt)
            _nt_free(pPStore->szPrompt, pPStore->cbPrompt);
        _nt_free(pPStore, sizeof(PSTORE_INFO));
    }
}

BOOL CheckPStoreAvailability(
                             PSTORE_INFO *pPStore
                             )
{
#ifndef _XBOX
    PSCREATEINST    *pfnCreateInstProc = NULL;
    BOOL            fRet = FALSE;

    if (NULL == (pPStore->hInst = LoadLibrary("pstorec.dll")))
        goto Ret;


    if (0 == (pfnCreateInstProc =
              (PSCREATEINST*)GetProcAddress(pPStore->hInst,
                                            "PStoreCreateInstance")))
        goto Ret;

    if (S_OK != pfnCreateInstProc((IPStore**)(&pPStore->pProv),
                                   &SysProv, NULL, 0))
        goto Ret;

    memcpy(&pPStore->SigType, &DefTypeGuid, sizeof(GUID));
    memcpy(&pPStore->SigSubtype, &DefSigGuid, sizeof(GUID));
    memcpy(&pPStore->ExchType, &DefTypeGuid, sizeof(GUID));
    memcpy(&pPStore->ExchSubtype, &DefExchGuid, sizeof(GUID));

    fRet = TRUE;
Ret:
    return fRet;
#else
    ASSERT( !"CheckPStoreAvailability" );
    return FALSE;
#endif
}

BOOL CreateNewPSKeyset(
                       PSTORE_INFO *pPStore,
                       DWORD dwFlags
                       )
{
    PST_ACCESSCLAUSE    rgClauses[2];
    PST_ACCESSRULE      rgRules[2];
    PST_ACCESSRULESET   Rules;
    PST_TYPEINFO        Info;
    PST_PROMPTINFO      PromptInfo = {NULL, NULL};
    HRESULT             hr;
    IPStore             *pIPS = (IPStore*)pPStore->pProv;
    DWORD               dwRegLoc = PST_KEY_CURRENT_USER;
    BOOL                fRet = FALSE;

    if (dwFlags & CRYPT_MACHINE_KEYSET)
        dwRegLoc = PST_KEY_LOCAL_MACHINE;

    // if type is not available the create it
    memset(&Info, 0, sizeof(Info));
    Info.cbSize = sizeof(PST_TYPEINFO);
    Info.szDisplayName = CRYPTO_KEY_TYPE_STRING;
    if (S_OK != (hr = pIPS->CreateType(dwRegLoc,
                                       &pPStore->SigType,
                                       &Info,
                                       0)))
    {
        if (PST_E_TYPE_EXISTS != hr)
            goto Ret;
    }

    // make same rules for read, write access
    rgRules[0].cbSize = sizeof(PST_ACCESSRULE);
    rgRules[0].AccessModeFlags = PST_READ;
    rgRules[0].cClauses = 0;
    rgRules[0].rgClauses = NULL;
    rgRules[1].cbSize = sizeof(PST_ACCESSRULE);
    rgRules[1].AccessModeFlags = PST_WRITE;
    rgRules[1].cClauses = 0;
    rgRules[1].rgClauses = NULL;

    Rules.cbSize = sizeof(PST_ACCESSRULESET);
    Rules.cRules = 2;
    Rules.rgRules = rgRules;

    // create the signature subtype
    Info.szDisplayName = CRYPTO_SIG_SUBTYPE_STRING;
    PromptInfo.szPrompt = L"";
    if (S_OK != (hr = pIPS->CreateSubtype(dwRegLoc,
                                          &pPStore->SigType,
                                          &pPStore->SigSubtype,
                                          &Info,
                                          &Rules,
                                          0)))
    {
        if (PST_E_TYPE_EXISTS != hr)
            goto Ret;
    }

    // create the exchange subtype
    Info.szDisplayName = CRYPTO_EXCH_SUBTYPE_STRING;
    if (S_OK != (hr = pIPS->CreateSubtype(dwRegLoc,
                                          &pPStore->SigType,
                                          &pPStore->ExchSubtype,
                                          &Info,
                                          &Rules,
                                          0)))
    {
        if (PST_E_TYPE_EXISTS != hr)
            goto Ret;
    }

    fRet = TRUE;
Ret:
    return fRet;
}

BOOL GetKeysetTypeAndSubType(
                             PNTAGUserList pUser
                             )
{
    BOOL    fDefault = TRUE;
    BYTE    *pb1 = NULL;
    DWORD   cb1;
    BYTE    *pb2 = NULL;
    DWORD   cb2;
    BOOL    fRet = FALSE;

    memcpy(&pUser->pPStore->SigType, &DefTypeGuid, sizeof(GUID));
    memcpy(&pUser->pPStore->SigSubtype, &DefSigGuid, sizeof(GUID));
    memcpy(&pUser->pPStore->ExchType, &DefTypeGuid, sizeof(GUID));
    memcpy(&pUser->pPStore->ExchSubtype, &DefExchGuid, sizeof(GUID));

    // look in registry and see if the type and subtype Guids are there
    if (NTF_SUCCEED != ReadRegValue(pUser->hKeys, "SigTypeSubtype",
                                    &pb1, &cb1, TRUE))
        goto Ret;

    if (NTF_SUCCEED != ReadRegValue(pUser->hKeys, "ExchTypeSubtype",
                                    &pb2, &cb2, TRUE))
        goto Ret;

    if (pb1 && pb2)
        fDefault = FALSE;

    if (pb1)
    {
        memcpy(&pUser->pPStore->SigType, pb1, sizeof(GUID));
        memcpy(&pUser->pPStore->SigSubtype, pb1 + sizeof(GUID), sizeof(GUID));
    }

    if (pb2)
    {
        memcpy(&pUser->pPStore->ExchType, pb2, sizeof(GUID));
        memcpy(&pUser->pPStore->ExchSubtype, pb2 + sizeof(GUID), sizeof(GUID));
    }

    fRet = TRUE;
Ret:
    if (pb1)
        _nt_free(pb1, cb1);
    if (pb2)
        _nt_free(pb2, cb2);
    return fRet;
}

BOOL SetUIPrompt(
                 PNTAGUserList pUser,
                 LPWSTR szPrompt
                 )
{
    DWORD   cb;
    LPWSTR  sz = NULL;
    BOOL    fRet = FALSE;

    // check if sig or exch keys are loaded and if so error
    if (NULL == pUser->pPStore)
    {
        SetLastError((DWORD)NTE_BAD_KEYSET);
        goto Ret;
    }

    if (NULL != szPrompt)
    {
        cb = (lstrlenW(szPrompt) + 1) * sizeof(WCHAR);

        if (NULL == (sz = (LPWSTR)_nt_malloc(cb)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }

        lstrcpyW(sz, szPrompt);
    }

    if (pUser->pPStore->szPrompt)
        _nt_free(pUser->pPStore->szPrompt, pUser->pPStore->cbPrompt);

    pUser->pPStore->cbPrompt = cb;
    pUser->pPStore->szPrompt = sz;

    fRet = TRUE;
Ret:
    return fRet;
}

BOOL PickleKey(
               BOOL fExportable,
	           size_t cbPriv,
	           PBYTE pbPriv,
               PBYTE *ppbData,
               PDWORD pcbData
               )
{
    BOOL fRet = FALSE;

    if (NULL != pbPriv)
    {
        // alloc the appropriate amount of space
        *pcbData = cbPriv + sizeof(DWORD) + sizeof(BOOL);
        if (NULL == (*ppbData = (PBYTE)_nt_malloc(*pcbData)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }

        // copy exportable info into buffer
        memcpy(*ppbData, &fExportable, sizeof(BOOL));

        // copy length of keying material into buffer
        memcpy(*ppbData + sizeof(BOOL), &cbPriv, sizeof(DWORD));

        // copy keying material into buffer
        memcpy(*ppbData + sizeof(DWORD) + sizeof(BOOL), pbPriv, cbPriv);
    }

    fRet = TRUE;
Ret:
    return fRet;
}

BOOL UnpickleKey(
                 PBYTE pbData,
                 DWORD cbData,
                 BOOL *pfExportable,
	             DWORD *pcbPriv,
	             PBYTE *ppbPriv
                 )
{
    BOOL fRet = FALSE;

    if (NULL != pbData)
    {
        // pull out the exportable info
        memcpy(pfExportable, pbData, sizeof(BOOL));

        // pull out the length of the key material
        memcpy(pcbPriv, pbData + sizeof(BOOL), sizeof(DWORD));

        // free the current key material memory
        if (NULL != *ppbPriv)
            _nt_free(*ppbPriv, *pcbPriv);
        
        // alloc new memory for the key material
        if (NULL == (*ppbPriv = (PBYTE)_nt_malloc(*pcbPriv)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }

        // copy key material
        memcpy(*ppbPriv, pbData + sizeof(DWORD) + sizeof(BOOL), *pcbPriv);
    }

    fRet = TRUE;
Ret:
    return fRet;
}

BOOL RestoreKeyFromProtectedStorage(
                                    PNTAGUserList pUser,
                                    LPWSTR szKeyName,
                                    BYTE **ppbKey,
                                    DWORD *pcbKey,
                                    LPWSTR szPrompt,
                                    BOOL fSigKey,
                                    BOOL fMachineKeySet,
                                    BOOL *pfUIOnKey
                                    )
{
    HRESULT         hr;
    DWORD           cb;
    BYTE            *pb = NULL;
    GUID            *pType;
    GUID            *pSubtype;
    BOOL            *pf;
    PST_PROMPTINFO  PromptInfo;
    IPStore         *pIPS = (IPStore*)(pUser->pPStore->pProv);
    DWORD           dwRegLoc = PST_KEY_CURRENT_USER;
    BOOL            fRet = FALSE;

    *pfUIOnKey = FALSE;

    if (fMachineKeySet)
        dwRegLoc = PST_KEY_LOCAL_MACHINE;

    memset(&PromptInfo, 0, sizeof(PromptInfo));
    PromptInfo.cbSize = sizeof(PST_PROMPTINFO);
    if (fSigKey)
    {
        if (0 == pUser->ContInfo.ContLens.cbSigPub)
        {
            fRet = TRUE;
            goto Ret;
        }
        pType = &pUser->pPStore->SigType;
        pSubtype = &pUser->pPStore->SigSubtype;
        pf = &pUser->ContInfo.fSigExportable;
    }
    else
    {
        if (0 == pUser->ContInfo.ContLens.cbExchPub)
        {
            fRet = TRUE;
            goto Ret;
        }
        pType = &pUser->pPStore->ExchType;
        pSubtype = &pUser->pPStore->ExchSubtype;
        pf = &pUser->ContInfo.fExchExportable;
    }

    // read the item from secure storage
    PromptInfo.hwndApp = NULL;
    if (NULL == pUser->pPStore->szPrompt)
        PromptInfo.szPrompt = szPrompt;
    else
        PromptInfo.szPrompt = pUser->pPStore->szPrompt;

    if (S_OK != (hr = pIPS->ReadItem(dwRegLoc,
                                     pType,
                                     pSubtype,
                                     szKeyName,
                                     &cb,
                                     &pb,
                                     &PromptInfo,
                                     PST_PROMPT_QUERY | PST_NO_UI_MIGRATION)))
    {
        // this function returns PST_E_ITEM_EXISTS if there is UI on the item
        if (PST_E_ITEM_EXISTS == hr)
        {
            *pfUIOnKey = TRUE;
        }
        else
        {
            goto Ret;
        }
    }

    if (!UnpickleKey(pb, cb, pf, pcbKey, ppbKey))
        goto Ret;

    fRet = TRUE;
Ret:
    if (pb)
        LocalFree(pb);
    return fRet;
}

BOOL MakeUnicodeKeysetName(
                           BYTE *pszName,
                           LPWSTR *ppszWName
                           )
{
    long    i;
    DWORD   cb;
    BOOL    fRet = FALSE;

    cb = (DWORD)lstrlenA((LPSTR)pszName);
    if (NULL == (*ppszWName = (LPWSTR)_nt_malloc((cb + 1) * sizeof(WCHAR))))
    {
        SetLastError((DWORD)ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    for(i=0;i<(long)cb;i++)
        (*ppszWName)[i] = (WCHAR)(pszName[i]);

    fRet = TRUE;
Ret:
    return fRet;
}

BOOL RestoreKeysetFromProtectedStorage(
                                       PNTAGUserList pUser,
                                       LPWSTR szPrompt,
                                       BYTE **ppbKey,
                                       DWORD *pcbKey,
                                       BOOL fSigKey,
                                       BOOL fMachineKeySet,
                                       BOOL *pfUIOnKey
                                       )
{
    LPWSTR  pszWName = NULL;
    BOOL    fRet = FALSE;

    // convert the keyset name to unicode
    if (!MakeUnicodeKeysetName((BYTE*)pUser->ContInfo.pszUserName, &pszWName))
        goto Ret;

    // restore the signature key
    if (!RestoreKeyFromProtectedStorage(pUser, pszWName, ppbKey, pcbKey,
                                        szPrompt, fSigKey, fMachineKeySet,
                                        pfUIOnKey))
        goto Ret;

    fRet = TRUE;
Ret:
    if (pszWName)
        _nt_free(pszWName, (wcslen(pszWName) + 1) * sizeof(WCHAR));
    return fRet;
}

void RemoveKeysetFromMemory(
                            PNTAGUserList pUser
                            )
{
    pUser->ContInfo.fSigExportable = FALSE;
    if (pUser->pSigPrivKey)
    {
        _nt_free(pUser->pSigPrivKey, pUser->SigPrivLen);
        pUser->SigPrivLen = 0;
        pUser->pSigPrivKey = NULL;
    }

    pUser->ContInfo.fExchExportable = FALSE;
    if (pUser->pExchPrivKey)
    {
        _nt_free(pUser->pExchPrivKey, pUser->ExchPrivLen);
        pUser->ExchPrivLen = 0;
        pUser->pExchPrivKey = NULL;
    }
}

BOOL SaveKeyToProtectedStorage(
                               PNTAGUserList pUser,
                               DWORD dwFlags,
                               LPWSTR szPrompt,
                               BOOL fSigKey,
                               BOOL fMachineKeySet
                               )
{
    HRESULT         hr;
    PBYTE           pb = NULL;
    DWORD           cb;
    GUID            *pType;
    GUID            *pSubtype;
    BOOL            f;
    BYTE            *pbKey;
    size_t          cbKey;
    LPWSTR          pszWName = NULL;
    LPSTR           szKeyName;
    PST_PROMPTINFO  PromptInfo;
    IPStore         *pIPS = (IPStore*)(pUser->pPStore->pProv);
    DWORD           dwRegLoc = PST_KEY_CURRENT_USER;
    DWORD           dwConfirm = PST_CF_NONE;

    BOOL            fRet = FALSE;

    if (fMachineKeySet)
        dwRegLoc = PST_KEY_LOCAL_MACHINE;

    memset(&PromptInfo, 0, sizeof(PromptInfo));
    PromptInfo.cbSize = sizeof(PST_PROMPTINFO);
    if (fSigKey)
    {
        pType = &pUser->pPStore->SigType;
        pSubtype = &pUser->pPStore->SigSubtype;
        f = pUser->ContInfo.fSigExportable;
        cbKey = pUser->SigPrivLen;
        pbKey = pUser->pSigPrivKey;
        szKeyName = "SPbK";
        if (dwFlags & CRYPT_USER_PROTECTED)
            dwConfirm = PST_CF_DEFAULT;
    }
    else
    {
        pType = &pUser->pPStore->ExchType;
        pSubtype = &pUser->pPStore->ExchSubtype;
        f = pUser->ContInfo.fExchExportable;
        cbKey = pUser->ExchPrivLen;
        pbKey = pUser->pExchPrivKey;
        szKeyName = "EPbK";
        if (dwFlags & CRYPT_USER_PROTECTED)
            dwConfirm = PST_CF_DEFAULT;
    }

    // format the signature key and exportable info
    if (!PickleKey(f, cbKey, pbKey, &pb, &cb))
        goto Ret;

    if (pb)
    {
        // make a unicode version of the keyset name
        if (!MakeUnicodeKeysetName((BYTE*)pUser->ContInfo.pszUserName, &pszWName))
            goto Ret;

        PromptInfo.hwndApp = NULL;
        if (NULL == pUser->pPStore->szPrompt)
            PromptInfo.szPrompt = szPrompt;
        else
            PromptInfo.szPrompt = pUser->pPStore->szPrompt;

        if (S_OK != (hr = pIPS->WriteItem(dwRegLoc,
                                          pType,
                                          pSubtype,
                                          pszWName,
                                          cb,
                                          pb,
                                          &PromptInfo,
                                          dwConfirm,
                                          0)))
            goto Ret;
    }

    fRet = TRUE;
Ret:
    if (pb)
        _nt_free(pb, cb);
    if (pszWName)
        _nt_free(pszWName, (wcslen(pszWName) + 1) * sizeof(WCHAR));
    return fRet;
}

BOOL DeleteKeyFromProtectedStorage(
                                   NTAGUserList *pUser,
                                   PCSP_STRINGS pStrings,
                                   DWORD dwKeySpec,
                                   BOOL fMachineKeySet,
                                   BOOL fMigration
                                   )
{
    LPWSTR          szWUserName = NULL;
    PST_PROMPTINFO  PromptInfo;
    IPStore         *pIPS;
    DWORD           dwRegLoc = PST_KEY_CURRENT_USER;
    BOOL            fRet = FALSE;

    memset(&PromptInfo, 0, sizeof(PromptInfo));
    PromptInfo.cbSize = sizeof(PST_PROMPTINFO);
    PromptInfo.hwndApp = NULL;

    if (fMachineKeySet)
        dwRegLoc = PST_KEY_LOCAL_MACHINE;

    // make a unicode name
    if (!MakeUnicodeKeysetName((BYTE*)pUser->ContInfo.pszUserName,
                               &szWUserName))
    {
        goto Ret;
    }

    pIPS = (IPStore*)(pUser->pPStore->pProv);

    if (AT_SIGNATURE == dwKeySpec)
    {
        if (fMigration)
        {
            PromptInfo.szPrompt = pStrings->pwszDeleteMigrSig;
        }
        else
        {
            PromptInfo.szPrompt = pStrings->pwszDeleteSig;
        }
        pIPS->DeleteItem(dwRegLoc,
                         &pUser->pPStore->SigType,
                         &pUser->pPStore->SigSubtype,
                         szWUserName,
                         &PromptInfo,
                         PST_NO_UI_MIGRATION);
    }
    else
    {
        if (fMigration)
        {
            PromptInfo.szPrompt = pStrings->pwszDeleteMigrExch;
        }
        else
        {
            PromptInfo.szPrompt = pStrings->pwszDeleteExch;
        }
        pIPS->DeleteItem(dwRegLoc,
                         &pUser->pPStore->ExchType,
                         &pUser->pPStore->ExchSubtype,
                         szWUserName,
                         &PromptInfo,
                         PST_NO_UI_MIGRATION);
    }

    fRet = TRUE;
Ret:
    if (szWUserName)
        _nt_free(szWUserName, (wcslen(szWUserName) + 1) * sizeof(WCHAR));
    return fRet;
}

BOOL DeleteFromProtectedStorage(
                                CONST char *pszUserID,
                                PCSP_STRINGS pStrings,
                                HKEY hRegKey,
                                BOOL fMachineKeySet
                                )
{
    GUID            Guid;
    NTAGUserList    User;
    BOOL            fRet = FALSE;

    // set up the User List structure
    memset(&User, 0, sizeof(User));
    User.ContInfo.pszUserName = (LPSTR)pszUserID;
    User.hKeys = hRegKey;

    if (NULL == (User.pPStore = (PSTORE_INFO*)_nt_malloc(sizeof(PSTORE_INFO))))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    if (!CheckPStoreAvailability(User.pPStore))
    {
        SetLastError((DWORD)NTE_FAIL);
        goto Ret;
    }

    // get type and subtypes
    if (!GetKeysetTypeAndSubType(&User))
        goto Ret;

    // delete each key
    if (!DeleteKeyFromProtectedStorage(&User, pStrings, AT_SIGNATURE,
                                       fMachineKeySet, FALSE))
    {
        goto Ret;
    }
    if (!DeleteKeyFromProtectedStorage(&User, pStrings, AT_KEYEXCHANGE,
                                       fMachineKeySet, FALSE))
    {
        goto Ret;
    }

    fRet = TRUE;
Ret:
    if (User.pPStore)
        FreePSInfo(User.pPStore);
    return fRet;
}

