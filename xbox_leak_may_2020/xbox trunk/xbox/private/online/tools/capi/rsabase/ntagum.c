/////////////////////////////////////////////////////////////////////////////
//  FILE          : ntagum.c                                               //
//  DESCRIPTION   : Crypto CP interfaces:                                  //
//                  CPAcquireContext                                       //
//                  CPReleaseContext                                       //
//  AUTHOR        :                                                        //
//  HISTORY       :                                                        //
//	Jan 25 1995 larrys  Changed from Nametag                               //
//      Feb 16 1995 larrys  Fix problem for 944 build                      //
//      Feb 23 1995 larrys  Changed NTag_SetLastError to SetLastError      //
//      Mar 23 1995 larrys  Added variable key length                      //
//      Apr 19 1995 larrys  Cleanup                                        //
//      May  9 1995 larrys  Removed warnings                               //
//      May 10 1995 larrys  added private api calls                        //
//      Jul 19 1995 larrys  Changed registry location                      //
//      Aug 09 1995 larrys  Changed error code                             //
//      Aug 31 1995 larrys  Fixed bug 27 CryptAcquireContext               //
//      Sep 12 1995 Jeffspel/ramas  Merged STT code into scp               //
//      Oct 02 1995 larrys  Fixed bug 27 return error NTE_BAD_KEYSET       //
//      Oct 13 1995 larrys  Added verify only context                      //
//      Oct 23 1995 larrys  Added GetProvParam stuff                       //
//      Nov  2 1995 larrys  Fixed bug 41                                   //
//      Oct 27 1995 rajeshk Added RandSeed stuff                           //
//      Nov  3 1995 larrys  Merged for NT checkin                          //
//      Nov  8 1995 larrys  Fixed SUR bug 10769                            //
//      Nov 13 1995 larrys  Fixed memory leak                              //
//      Nov 30 1995 larrys  Bug fix                                        //
//      Dec 11 1995 larrys  Added WIN96 password cache                     //
//      Dec 13 1995 larrys  Changed random number update                   //
//      Mar 01 1996 rajeshk Fixed the stomp bug                            //
//      May 15 1996 larrys  Added private key export                       //
//      May 28 1996 larrys  Fix bug in cache code                          //
//      Jun 11 1996 larrys  Added NT encryption of registry keys           //
//      Sep 13 1996 mattt   Varlen salt, 40-bit RC4 key storage, interop   //
//      Oct 14 1996 jeffspel Changed GenRandom to NewGenRandom             //
//      May 23 1997 jeffspel Added provider type checking                  //
//      Jun 18 1997 jeffspel Check if process is LocalSystem               //
//                                                                         //
//  Copyright (C) 1993, 1999 Microsoft Corporation   All Rights Reserved   //
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"
#include <xwinreg.h>
#include <stdio.h>
#include <stdlib.h>
#include "shacomm.h"
#include "nt_rsa.h"
#include "randlib.h"
#include "ntagum.h"
#include "protstor.h"
#include "ntagimp1.h"
#include "contman.h"
#include "swnt_pk.h"

extern void FreeUserRec(PNTAGUserList pUser);
extern CSP_STRINGS g_Strings;


#define NTAG_DEF_MACH_CONT_NAME "DefaultKeys"
#define NTAG_DEF_MACH_CONT_NAME_LEN sizeof(NTAG_DEF_MACH_CONT_NAME)

#define PSKEYS      "PSKEYS"

PNTAGKeyList MakeNewKey(
        ALG_ID      aiKeyAlg,
        DWORD       dwRights,
        DWORD       dwKeyLen,
        HCRYPTPROV  hUID,
        BYTE        *pbKeyData,
        BOOL        fUsePassedKeyBuffer
    );

void FreeNewKey(PNTAGKeyList pOldKey);

BOOL CPCreateHash(IN HCRYPTPROV hUID,
                  IN ALG_ID Algid,
                  IN HCRYPTKEY hKey,
                  IN DWORD dwFlags,
                  OUT HCRYPTHASH *phHash);

BOOL CPHashData(IN HCRYPTPROV hUID,
                IN HCRYPTHASH hHash,
                IN CONST BYTE *pbData,
                IN DWORD dwDataLen,
                IN DWORD dwFlags);

BOOL CPSetHashParam(IN HCRYPTPROV hUID,
                    IN HCRYPTHASH hHash,
                    IN DWORD dwParam,
                    IN BYTE *pbData,
                    IN DWORD dwFlags);

BOOL CPGetHashParam(IN HCRYPTPROV hUID,
                    IN HCRYPTHASH hHash,
                    IN DWORD dwParam,
                    IN BYTE *pbData,
                    IN DWORD *pwDataLen,
                    IN DWORD dwFlags);

BOOL CPDestroyHash(IN HCRYPTPROV hUID,
                   IN HCRYPTHASH hHash);


BOOL ReadRegValue(
                  HKEY hLoc,
                  char *pszName,
                  BYTE **ppbData,
                  DWORD *pcbLen,
                  BOOL fAlloc
                  )
{
    BOOL    fRet = NTF_FAILED;

    if (fAlloc)
    {
        *pcbLen = 0;
        *ppbData = NULL;

        // Need to get the size of the value first
        if (RegQueryValueEx(hLoc, pszName, 0, NULL, NULL,
	                        pcbLen) != ERROR_SUCCESS)
        {
            fRet = NTF_SUCCEED;
            goto Ret;
        }

        if (*pcbLen == 0)
        {
            fRet = NTF_SUCCEED;
            goto Ret;
        }

        if ((*ppbData = (BYTE *) _nt_malloc(*pcbLen)) == NULL)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
    }

    // Now get the key
    if (RegQueryValueEx(hLoc, pszName, 0, NULL, (BYTE *) *ppbData,
	                    pcbLen) != ERROR_SUCCESS)
    {
        SetLastError((DWORD) NTE_SYS_ERR);
        goto Ret;
    }

    fRet = NTF_SUCCEED;
Ret:
    if (NTF_FAILED == fRet)
    {
        if (fAlloc)
        {
            if (*ppbData)
                _nt_free(*ppbData, *pcbLen);
            ppbData = NULL;
        }
    }
    return fRet;
}

void CheckForStorageType(
                         HKEY hRegKey,
                         DWORD *pdwProtStor
                         )
{
    BYTE    **ppb = (BYTE**)(&pdwProtStor);
    DWORD   cb = sizeof(DWORD);

    if (hRegKey)
    {
        if (!ReadRegValue(hRegKey, PSKEYS, ppb, &cb, FALSE))
        {
            *pdwProtStor = 0;
        }
        // BUGBUG - this is done because the value of PROTECTION_API_KEYS was
        // mistakenly also used in IE 5 on Win9x for a key format in the
        // registry where the public keys were not encrypted.
        if (PROTECTION_API_KEYS == *pdwProtStor)
        {
            *pdwProtStor = PROTECTED_STORAGE_KEYS;
        }
    }
    else
    {
        *pdwProtStor = PROTECTION_API_KEYS;
    }
}



BOOL OpenUserReg(
                 LPSTR pszUserName,
                 DWORD dwProvType,
                 DWORD dwFlags,
                 BOOL fUserKeys,
                 HKEY *phRegKey,
                 DWORD *pdwOldKeyFlags
                 )
{
    HKEY        hTopRegKey = 0;
    TCHAR       *pszLocbuf = NULL;
    DWORD       cbLocBuf;
    long        lsyserr;
    DWORD       dwResult;
    BOOL        fLeaveOldKeys = FALSE;
    BOOL        fStatus = FALSE;
    DWORD       fRet = FALSE;

    cbLocBuf = AllocAndSetLocationBuff(dwFlags & CRYPT_MACHINE_KEYSET,
                                       dwProvType,
                                       pszUserName,
                                       &hTopRegKey,
                                       &pszLocbuf,
                                       fUserKeys,
                                       &fLeaveOldKeys);

    if (!cbLocBuf)
    {
        goto Ret;
    }
    if (fLeaveOldKeys)
    {
        *pdwOldKeyFlags |= LEAVE_OLD_KEYS;
    }

    // try to open the old storage location
    fStatus = OpenRegKeyWithTokenPriviledges(hTopRegKey,
                                             pszLocbuf,
                                             phRegKey,
                                             pdwOldKeyFlags);

    if (dwFlags & CRYPT_NEWKEYSET)
    {
        if (fStatus)
        {
            RegCloseKey(*phRegKey);
            *phRegKey = 0;
            goto Ret;
        }
    }
    else
    {
        if (!fStatus)
        {
            goto Ret;
        }
    }

    fRet = TRUE;
Ret:
    if (hTopRegKey && (HKEY_CURRENT_USER != hTopRegKey) &&
        (HKEY_LOCAL_MACHINE != hTopRegKey))
    {
        RegCloseKey(hTopRegKey);
    }
    if (pszLocbuf)
        _nt_free(pszLocbuf, cbLocBuf);

    return fRet;
}

DWORD OpenUserKeyGroup(
                       PNTAGUserList pTmpUser,
                       LPSTR szUserName,
                       DWORD dwFlags,
                       BOOL fUserKeys
                       )
{
    BOOL        fMachineKeyset;
    DWORD       dwErr = 0;

    fMachineKeyset = dwFlags & CRYPT_MACHINE_KEYSET;

    dwErr = ReadContainerInfo(pTmpUser->dwProvType,
                              szUserName,
                              fMachineKeyset,
                              dwFlags,
                              &pTmpUser->ContInfo);

    if (CRYPT_NEWKEYSET & dwFlags)
    {
        if (NTE_BAD_KEYSET != dwErr)
        {
            if (0 == dwErr)
            {
                dwErr = (DWORD)NTE_EXISTS;
            }
            goto Ret;
        }
        else
        {
            // allocate space for the container/user name
            if (0 != (dwErr = SetContainerUserName(szUserName,
                                                   &pTmpUser->ContInfo)))
            {
                goto Ret;
            }

            if (!OpenUserReg(pTmpUser->ContInfo.pszUserName,
                             pTmpUser->dwProvType,dwFlags,
                             FALSE,
                             &pTmpUser->hKeys,
                             &pTmpUser->dwOldKeyFlags))
            {
                dwErr = NTE_EXISTS;
                goto Ret;
            }

            // no key set so create one
            if (0 == pTmpUser->hKeys)
            {
                // if the is for user key then make sure that Data Protection API
                // works so a container isn't created which can't be used to store keys
                if (!fMachineKeyset)
                {
                    if (0 != (dwErr = TryDPAPI()))
                    {
                        goto Ret;
                    }
                }

                pTmpUser->dwKeyStorageType = PROTECTION_API_KEYS;
                if (0 != (dwErr = WriteContainerInfo(pTmpUser->dwProvType,
                                 pTmpUser->ContInfo.rgwszFileName,
                                 fMachineKeyset,
                                 &pTmpUser->ContInfo)))
                {
                    goto Ret;
                }
            }
        }
    }
    else
    {
        if (0 == dwErr)
        {
            pTmpUser->dwKeyStorageType = PROTECTION_API_KEYS;
        }
        else
        {
            if (!OpenUserReg(szUserName,
                             pTmpUser->dwProvType,
                             dwFlags,
                             FALSE,
                             &pTmpUser->hKeys,
                             &pTmpUser->dwOldKeyFlags))
            {
                goto Ret;
            }

            // migrating keys need to set the user name
            if (0 != (dwErr = SetContainerUserName(szUserName,
                                                   &pTmpUser->ContInfo)))
            {
                goto Ret;
            }
            dwErr = 0;
        }
    }
Ret:
    return dwErr;
}

BOOL DeleteOldUserKeyGroup(
                           CONST char *pszUserID,
                           DWORD dwProvType,
                           DWORD dwFlags
                           )
{
    HKEY        hTopRegKey = 0;
    TCHAR       *locbuf = NULL;
    DWORD       dwLocBuffLen;
    long        lsyserr;
    HKEY        hRegKey = 0;
    BOOL        fProtStor;
    BOOL        fMachineKeySet = FALSE;
    DWORD       dwStorageType;
    BOOL        fLeaveOldKeys = FALSE;
    BOOL        fRet = NTF_FAILED;

    // Copy the location of the key groups, append the userID to it
    if (dwFlags & CRYPT_MACHINE_KEYSET)
        fMachineKeySet = TRUE;

    dwLocBuffLen = AllocAndSetLocationBuff(fMachineKeySet,
                                           dwProvType,
                                           pszUserID,
                                           &hTopRegKey,
                                           &locbuf,
                                           FALSE,
                                           &fLeaveOldKeys);
    if (!dwLocBuffLen)
        goto Ret;

    // open it for all access so we can save later, if necessary
    if (ERROR_SUCCESS != MyRegOpenKeyEx(hTopRegKey,
                                        locbuf,
                                        0,
                                        KEY_ALL_ACCESS,
                                        &hRegKey))
    {
        SetLastError((DWORD)NTE_BAD_KEYSET);
        goto Ret;
    }

    CheckForStorageType(hRegKey, &dwStorageType);

    if (PROTECTED_STORAGE_KEYS == dwStorageType)
    {
        if (!DeleteFromProtectedStorage(pszUserID, &g_Strings, hRegKey,
                                        fMachineKeySet))
        {
            goto Ret;
        }
    }

    lsyserr = MyRegDeleteKey(hTopRegKey, locbuf);

    if (lsyserr != ERROR_SUCCESS)
    {
        SetLastError((DWORD) NTE_BAD_KEYSET);
        goto Ret;
    }

    fRet = NTF_SUCCEED;
Ret:
    if (hTopRegKey && (HKEY_CURRENT_USER != hTopRegKey) &&
        (HKEY_LOCAL_MACHINE != hTopRegKey))
    {
        RegCloseKey(hTopRegKey);
    }
    if (hRegKey)
        RegCloseKey(hRegKey);
    if (locbuf)
        _nt_free(locbuf, dwLocBuffLen);
    return fRet;
}

BOOL DeleteUserKeyGroup(
                        LPSTR pszUserName,
                        DWORD dwProvType,
                        DWORD dwFlags
                        )
{
    BOOL    fRet = FALSE;

    if (0 != DeleteContainerInfo(dwProvType,
                                 pszUserName,
                                 dwFlags & CRYPT_MACHINE_KEYSET))
    {
        if (!DeleteOldUserKeyGroup(pszUserName, dwProvType, dwFlags))
            goto Ret;
    }
    fRet = TRUE;
Ret:
    return fRet;
}


// read the exportability flag for the private key from the registry
void ReadPrivateKeyExportability(IN PNTAGUserList pUser,
                                 IN BOOL fExchange)
{
    DWORD dwType;
    DWORD cb = 1;
    BYTE b;
    BOOL *pf;

    if (fExchange)
    {
        pf = &pUser->ContInfo.fExchExportable;
        if (ERROR_SUCCESS != RegQueryValueEx(pUser->hKeys, "EExport", NULL,
                                             &dwType, &b, &cb))
            return;
    }
    else
    {
        pf = &pUser->ContInfo.fSigExportable;
        if (ERROR_SUCCESS != RegQueryValueEx(pUser->hKeys, "SExport", NULL,
                                             &dwType, &b, &cb))
            return;
    }
	
    if ((sizeof(b) == cb) && (0x01 == b))
        *pf = TRUE;
}

BOOL ReadKey(
             HKEY hLoc,
             char *pszName,
             BYTE **ppbData,
             DWORD *pcbLen,
             PNTAGUserList pUser,
             HCRYPTKEY hKey,
             BOOL *pfPrivKey,
             BOOL fKeyExKey,
             BOOL fLastKey
             )
{
    DWORD                       dwTemp;
    BOOL                        rt;
    CHAR                        *pch;
    CRYPTPROTECT_PROMPTSTRUCT   PromptStruct;
    CRYPT_DATA_BLOB             DataIn;
    CRYPT_DATA_BLOB             DataOut;
    BOOL                        fRet = NTF_FAILED;

    memset(&DataIn, 0, sizeof(DataIn));
    memset(&DataOut, 0, sizeof(DataOut));
    memset(&PromptStruct, 0, sizeof(PromptStruct));

    if (NTF_SUCCEED != ReadRegValue(hLoc, pszName, ppbData, pcbLen, TRUE))
        goto Ret;

    if (*ppbData)
    {
        if ((REG_KEYS == pUser->dwKeyStorageType) && *pfPrivKey)
        {
            ReadPrivateKeyExportability(pUser, fKeyExKey);
        }

        if ((REG_KEYS == pUser->dwKeyStorageType) ||
            (PROTECTED_STORAGE_KEYS == pUser->dwKeyStorageType))
        {
            if (hKey != 0)
            {
                dwTemp = *pcbLen;
                rt = LocalDecrypt(pUser->hUID, hKey, 0, fLastKey, 0,
                                  *ppbData, &dwTemp, FALSE);
            }
        }

        pch = *ppbData;
        if ((strcmp(pszName, "RandSeed") != 0) &&
            (pch[0] != 'R' ||
             pch[1] != 'S' ||
             pch[2] != 'A'))
        {
            if (!*pfPrivKey)  // this may be a Win9x public key
            {
                *pfPrivKey = TRUE;
            }
            else
            {
                SetLastError((DWORD) NTE_KEYSET_ENTRY_BAD);
                goto Ret;
            }
        }
    }
    fRet = NTF_SUCCEED;
Ret:
    // free the DataOut struct if necessary
    if (DataOut.pbData)
        LocalFree(DataOut.pbData);

    return fRet;
}

//
// Routine : MigrateProtectedStorageKeys
//
// Description : Try and retrieve both the sig and exch private keys and
//               migrate them to the protection APIs and then delete the keys
//               from the protected storage.  The fSigPubKey and fExchPubKey
//               parameters indicate the public keys should be derived from
//               the private keys.
//

BOOL MigrateProtectedStorageKeys(
                                 IN PNTAGUserList pUser,
                                 IN LPWSTR szPrompt,
                                 IN BOOL fSigPubKey,
                                 IN BOOL fExchPubKey
                                 )
{
    BOOL    fMachineKeySet = FALSE;
    DWORD   dwFlags = 0;
    DWORD   dwSigFlags = 0;
    DWORD   dwExchFlags = 0;
    BOOL    fUIOnSigKey = FALSE;
    BOOL    fUIOnExchKey = FALSE;
    DWORD   dwErr = 0;
    BOOL    fRet = FALSE;

    __try
    {
        if (CRYPT_MACHINE_KEYSET & pUser->Rights)
        {
            fMachineKeySet = TRUE;
        }

        // NOTE - the appropriate exportable flag is set by
        // RestoreKeysetFromProtectedStorage
        if (pUser->ContInfo.ContLens.cbSigPub || fSigPubKey)
        {
            if (!RestoreKeysetFromProtectedStorage(pUser, szPrompt,
                    &pUser->pSigPrivKey, &pUser->SigPrivLen,
                    TRUE, fMachineKeySet,
                    &fUIOnSigKey))
            {
                goto Ret;
            }

            // check if the sig key is there and the public key is supposed to
            // be derived from it
            if (fSigPubKey)
            {
                if (!DerivePublicFromPrivate(pUser, TRUE))
                {
                    goto Ret;
                }
            }
        }

        // open the other key (sig or exch) and store
        if (pUser->ContInfo.ContLens.cbExchPub || fExchPubKey)
        {
            if (!RestoreKeysetFromProtectedStorage(pUser, szPrompt,
                    &pUser->pExchPrivKey, &pUser->ExchPrivLen,
                    FALSE, fMachineKeySet,
                    &fUIOnExchKey))
            {
                goto Ret;
            }

            // check if the sig key is there and the public key is supposed to
            // be derived from it
            if (fExchPubKey)
            {
                if (!DerivePublicFromPrivate(pUser, FALSE))
                {
                    goto Ret;
                }
            }
        }

        // set UI flags if necessary
        if (fUIOnSigKey)
        {
            dwSigFlags = CRYPT_USER_PROTECTED;
        }
        if (fUIOnExchKey)
        {
            dwExchFlags = CRYPT_USER_PROTECTED;
        }

        if ((NULL == pUser->pSigPrivKey) &&
            (NULL == pUser->pExchPrivKey))
        {
            pUser->dwKeyStorageType = PROTECTION_API_KEYS;
            if (0 != (dwErr = WriteContainerInfo(pUser->dwProvType,
                             pUser->ContInfo.rgwszFileName,
                             fMachineKeySet,
                             &pUser->ContInfo)))
            {
                goto Ret;
            }
        }
        else
        {
            // migrate the keys to the protection APIs
            if (pUser->pSigPrivKey)
            {
                if (!ProtectPrivKey(pUser, g_Strings.pwszMigrKeys,
                                    dwSigFlags, TRUE))
                {
                    goto Ret;
                }

                // delete the sig key from the protected storage
                DeleteKeyFromProtectedStorage(pUser, &g_Strings, AT_SIGNATURE,
                                              fMachineKeySet, TRUE);
            }
            if (pUser->pExchPrivKey)
            {
                if (!ProtectPrivKey(pUser, g_Strings.pwszMigrKeys,
                                    dwExchFlags, FALSE))
                {
                    goto Ret;
                }

                // delete the key exchange key from the protected storage
                DeleteKeyFromProtectedStorage(pUser, &g_Strings, AT_KEYEXCHANGE,
                                              fMachineKeySet, TRUE);
            }
        }

        pUser->dwKeyStorageType = PROTECTION_API_KEYS;

        RegCloseKey(pUser->hKeys);
        pUser->hKeys = 0;

        if (pUser->Rights & CRYPT_MACHINE_KEYSET)
            dwFlags = CRYPT_MACHINE_KEYSET;

        // delete the registry key
        if (!DeleteOldUserKeyGroup(pUser->ContInfo.pszUserName,
                                   pUser->dwProvType,
                                   dwFlags))
        {
            goto Ret;
        }

        fRet = TRUE;
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    return fRet;
}


//
// Routine : ProtectPrivKey
//
// Description : Encrypts the private key and persistently stores it.
//

BOOL ProtectPrivKey(
                    IN OUT PNTAGUserList pTmpUser,
                    IN LPWSTR szPrompt,
                    IN DWORD dwFlags,
                    IN BOOL fSigKey
                    )
{
    BYTE                        *pbKey;
    DWORD                       cbKey;
    BYTE                        **ppbEncKey;
    DWORD                       *pcbEncKey;
    BYTE                        *pbTmpEncKey = NULL;
    CRYPT_DATA_BLOB             DataIn;
    CRYPT_DATA_BLOB             DataOut;
    CRYPTPROTECT_PROMPTSTRUCT   PromptStruct;
    DWORD                       dwProtectFlags = 0;
    DWORD                       dwErr = 0;
    BOOL                        fRet = NTF_FAILED;

    memset(&DataIn, 0, sizeof(DataIn));
    memset(&DataOut, 0, sizeof(DataOut));
    memset(&PromptStruct, 0, sizeof(PromptStruct));

    // wrap with a try since there is a critical sections in here
    try
    {
        EnterCriticalSection(&pTmpUser->CritSec);

        if (fSigKey)
        {
            DataIn.cbData = pTmpUser->SigPrivLen;
            DataIn.pbData = pTmpUser->pSigPrivKey;
            ppbEncKey = &(pTmpUser->ContInfo.pbSigEncPriv);
            pcbEncKey = &(pTmpUser->ContInfo.ContLens.cbSigEncPriv);
        }
        else
        {
            DataIn.cbData = pTmpUser->ExchPrivLen;
            DataIn.pbData = pTmpUser->pExchPrivKey;
            ppbEncKey = &(pTmpUser->ContInfo.pbExchEncPriv);
            pcbEncKey = &(pTmpUser->ContInfo.ContLens.cbExchEncPriv);
        }

        //
        // Two checks (needed for FIPS) before offloading to the offload
        // module.
        //
        // First check is if the user protected flag was requested so
        // that UI is attached to the decryption of the key.
        //
        // Second check is if this is a user key or a machine key.
        //

        // protect the key with the data protection API
        PromptStruct.cbSize = sizeof(PromptStruct);
        if (CRYPT_USER_PROTECTED & dwFlags)
        {
            if (pTmpUser->ContInfo.fCryptSilent)
            {
                SetLastError((DWORD) NTE_SILENT_CONTEXT);
                goto Ret;
            }

            if (fSigKey)
            {
                pTmpUser->ContInfo.ContLens.dwUIOnKey |= AT_SIGNATURE;
            }
            else
            {
                pTmpUser->ContInfo.ContLens.dwUIOnKey |= AT_KEYEXCHANGE;
            }
            PromptStruct.dwPromptFlags = CRYPTPROTECT_PROMPT_ON_UNPROTECT |
                                         CRYPTPROTECT_PROMPT_ON_PROTECT;
        }
        if (szPrompt)
        {
            PromptStruct.hwndApp = pTmpUser->hWnd;
            if (pTmpUser->pwszPrompt)
                PromptStruct.szPrompt = pTmpUser->pwszPrompt;
            else
                PromptStruct.szPrompt = szPrompt;
        }
    
        // protect as machine data if necessary
        if (pTmpUser->Rights & CRYPT_MACHINE_KEYSET)
            dwProtectFlags = CRYPTPROTECT_LOCAL_MACHINE;

        if (!MyCryptProtectData(&DataIn, L"", NULL, NULL,
                                &PromptStruct, dwProtectFlags, &DataOut))
        {
            dwFlags = GetLastError();
            SetLastError((DWORD) NTE_FAIL);
            goto Ret;
        }
        if (NULL == (pbTmpEncKey = _nt_malloc(DataOut.cbData)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
        memcpy(pbTmpEncKey, DataOut.pbData, DataOut.cbData);
        if (*ppbEncKey)
        {
            _nt_free(*ppbEncKey, *pcbEncKey);
        }
        *pcbEncKey = DataOut.cbData;
        *ppbEncKey = pbTmpEncKey;

        // write out the key to the file
        if (0 != (dwErr = WriteContainerInfo(pTmpUser->dwProvType,
                             pTmpUser->ContInfo.rgwszFileName,
                             pTmpUser->Rights & CRYPT_MACHINE_KEYSET,
                             &pTmpUser->ContInfo)))
        {
            SetLastError(dwErr);
            goto Ret;
        }

        fRet = NTF_SUCCEED;
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    LeaveCriticalSection(&pTmpUser->CritSec);
    // free the DataOut struct if necessary
    if (DataOut.pbData)
        LocalFree(DataOut.pbData);

    return fRet;
}

//
// Routine : UnprotectPrivKey
//
// Description : Decrypts the private key.  If the fAlwaysDecrypt flag is set
//               then it checks if the private key is already in the buffer
//               and if so then it does not decrypt.
//

BOOL UnprotectPrivKey(
                      IN OUT PNTAGUserList pTmpUser,
                      IN LPWSTR szPrompt,
                      IN BOOL fSigKey,
                      IN BOOL fAlwaysDecrypt
                      )
{
    BYTE                        **ppbKey;
    DWORD                       *pcbKey;
    BYTE                        *pbEncKey;
    DWORD                       cbEncKey;
    BYTE                        *pbTmpKey = NULL;
    CRYPTPROTECT_PROMPTSTRUCT   PromptStruct;
    CRYPT_DATA_BLOB             DataIn;
    CRYPT_DATA_BLOB             DataOut;
    CHAR                        *pch;
    DWORD                       dwProtectFlags = 0;
    BOOL                        fInCritSec = FALSE;
    BOOL                        fRet = FALSE;

    memset(&DataOut, 0, sizeof(DataOut));

    if(!fAlwaysDecrypt)
    {
        //
        // avoid taking critical section if we aren't forced to cause
        // a re-decrypt (which is necessary when UI is forced).
        //

        if (fSigKey)
        {
            ppbKey = &(pTmpUser->pSigPrivKey);
        }
        else
        {
            ppbKey = &(pTmpUser->pExchPrivKey);
        }

        if(*ppbKey)
        {
            fRet = TRUE;
            goto Ret;
        }
    }

    // wrap with a try since there is a critical sections in here
    try
    {
        //
        // take critical section around reads and writes to the context
        // when it hasn't been initialized.  Also, the critical section is
        // taken when fAlwaysDecrypt is specified which prevents multiple
        // outstanding UI requests.
        //
        EnterCriticalSection(&pTmpUser->CritSec);
        fInCritSec = TRUE;

        if (fSigKey)
        {
            pcbKey = &(pTmpUser->SigPrivLen);
            ppbKey = &(pTmpUser->pSigPrivKey);
            pbEncKey = pTmpUser->ContInfo.pbSigEncPriv;
            cbEncKey = pTmpUser->ContInfo.ContLens.cbSigEncPriv;
        }
        else
        {
            pcbKey = &(pTmpUser->ExchPrivLen);
            ppbKey = &(pTmpUser->pExchPrivKey);
            pbEncKey = pTmpUser->ContInfo.pbExchEncPriv;
            cbEncKey = pTmpUser->ContInfo.ContLens.cbExchEncPriv;
        }

        if ((NULL == *ppbKey) || fAlwaysDecrypt)
        {
            memset(&DataIn, 0, sizeof(DataIn));
            memset(&PromptStruct, 0, sizeof(PromptStruct));

            if (pTmpUser->Rights & CRYPT_MACHINE_KEYSET)
                dwProtectFlags = CRYPTPROTECT_LOCAL_MACHINE;

            //  set up the prompt structure
            PromptStruct.cbSize = sizeof(PromptStruct);
            PromptStruct.hwndApp = pTmpUser->hWnd;
            PromptStruct.szPrompt = szPrompt;

            DataIn.cbData = cbEncKey;
            DataIn.pbData = pbEncKey;
            if (!MyCryptUnprotectData(&DataIn, NULL, NULL, NULL,
                                      &PromptStruct, dwProtectFlags, &DataOut))
            {
                SetLastError((DWORD)NTE_KEYSET_ENTRY_BAD);
                goto Ret;
            }

            pch = DataOut.pbData;
            if ((sizeof(DWORD) > DataOut.cbData) || pch[0] != 'R' ||
                pch[1] != 'S' || pch[2] != 'A')
            {
                SetLastError((DWORD) NTE_KEYSET_ENTRY_BAD);
                goto Ret;
            }

            if (NULL == *ppbKey)
            {
                if (NULL == (pbTmpKey = (BYTE*)_nt_malloc(DataOut.cbData)))
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    goto Ret;
                }
                memcpy(pbTmpKey, DataOut.pbData, DataOut.cbData);

                // move the new key into the context
                if (*ppbKey)
                {
                    _nt_free(*ppbKey, *pcbKey);
                }
                *pcbKey = DataOut.cbData;
                *ppbKey = pbTmpKey;
            }
        }

        fRet = TRUE;
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fInCritSec)
    {
        LeaveCriticalSection(&pTmpUser->CritSec);
    }

    // free the DataOut struct if necessary
    if (DataOut.pbData)
    {
        ZeroMemory(DataOut.pbData, DataOut.cbData);
        LocalFree(DataOut.pbData);
    }

    return fRet;
}

BOOL LoadWin96Cache(
                    PNTAGUserList pTmpUser,
                    LPSTR szUserName,
                    DWORD dwFlags,
                    BOOL fLowerUserName
                    )
{
    HANDLE              handle = NULL;
    char                *szResource = NULL;
    char                *szLowerUserName = NULL;
    WORD                wcbRandom;
    DWORD               cbsize;
    FARPROC             CachePW;
    FARPROC             GetCachePW;
    BYTE                rgbRandom[STORAGE_RC4_KEYLEN];
    DWORD               rc;
    BOOL                fKey = FALSE;
    BYTE                HashData[MD5DIGESTLEN] = {0x70, 0xf2, 0x85, 0x1e, 
                                                  0x4e, 0x00, 0x00, 0x00,
                                                  0x00, 0x00, 0x00, 0x00,
                                                  0x00, 0x00, 0x00, 0x00};
    HCRYPTHASH          hHash;
    DWORD               dwDataLen = MD5DIGESTLEN;
    PNTAGHashList       pTmpHash;
    BOOL                fRet = FALSE;

#define PREFIX "crypt_"
#define CACHE "WNetCachePassword"
#define GET_CACHE "WNetGetCachedPassword"

    cbsize = strlen(szUserName) + strlen(PREFIX);

#ifndef _XBOX
    if (!FIsWinNT())
    {
        // Try to load MPR.DLL for WIN96 password cache
        if (NULL != (handle = LoadLibrary("MPR.DLL")))
        {
            if ((!(dwFlags & CRYPT_MACHINE_KEYSET)) &&
                (CachePW = GetProcAddress(handle, CACHE)) &&
                (GetCachePW = GetProcAddress(handle, GET_CACHE)))
            {
                if ((szResource = (char *) _nt_malloc(cbsize + 1)) == NULL)
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    goto Ret;
                }

                strcpy(szResource, PREFIX);
                strcat(szResource, szUserName);

                wcbRandom = STORAGE_RC4_KEYLEN;
                if (((rc = GetCachePW(szResource, cbsize, rgbRandom, &wcbRandom, 6)) != NO_ERROR) ||
                     (wcbRandom != STORAGE_RC4_KEYLEN))
                {
                    if (rc == ERROR_NOT_SUPPORTED)
                    {
                        goto no_cache;
                    }

                    if (!FIPS186GenRandom(&pTmpUser->hRNGDriver,
                                           &pTmpUser->ContInfo.pbRandom,
                                           &pTmpUser->ContInfo.ContLens.cbRandom,
                                           rgbRandom, STORAGE_RC4_KEYLEN))
                    {
                        SetLastError((DWORD) NTE_FAIL);
                        goto Ret;
                    }

                    CachePW(szResource, cbsize, rgbRandom, STORAGE_RC4_KEYLEN, 6, 0);
                }

                fKey = TRUE;

                if ((pTmpUser->pCachePW=(char *)_nt_malloc(STORAGE_RC4_KEYLEN)) == NULL)
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    goto Ret;
                }

                memcpy(pTmpUser->pCachePW, rgbRandom, STORAGE_RC4_KEYLEN);

            }
        }
    }
no_cache:
#endif // _XBOX

    if (!fKey)
    {
        if (RCRYPT_FAILED(CPCreateHash(pTmpUser->hUID,
                                       CALG_MD5,
                                       0,
                                       0,
                                       &hHash)))
        {
            SetLastError((DWORD) NTE_FAIL);
            goto Ret;
        }

        if (RCRYPT_FAILED(CPSetHashParam(pTmpUser->hUID,
                                         hHash,
                                         HP_HASHVAL,
                                         HashData,
                                         0)))
        {
            SetLastError((DWORD) NTE_FAIL);
            goto Ret;
        }

        if ((pTmpHash = (PNTAGHashList) NTLValidate(hHash, pTmpUser->hUID,
                                                    HASH_HANDLE)) == NULL)
        {
            SetLastError((DWORD) NTE_BAD_HASH);
            goto Ret;
        }

        pTmpHash->HashFlags &= ~HF_VALUE_SET;

        // make the user name lower case
        cbsize = lstrlen(szUserName) + sizeof(CHAR);
        if (NULL == (szLowerUserName = (char *) _nt_malloc(cbsize)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
        lstrcpy(szLowerUserName, szUserName);
        if (fLowerUserName)
            _strlwr(szLowerUserName);

        if (RCRYPT_FAILED(CPHashData(pTmpUser->hUID,
                                     hHash,
                                     szLowerUserName,
                                     lstrlen(szLowerUserName) + sizeof(CHAR),
                                     0)))
        {
            SetLastError((DWORD) NTE_FAIL);
            goto Ret;
        }

        if (RCRYPT_FAILED(CPGetHashParam(pTmpUser->hUID,
                                         hHash,
                                         HP_HASHVAL,
                                         HashData,
                                         &dwDataLen,
                                         0)))
        {
            SetLastError((DWORD) NTE_FAIL);
            goto Ret;
        }

        if (RCRYPT_FAILED(CPDestroyHash(pTmpUser->hUID,
                                        hHash)))
        {
            SetLastError((DWORD) NTE_FAIL);
            goto Ret;
        }

        if ((pTmpUser->pCachePW=(char *)_nt_malloc(STORAGE_RC4_KEYLEN)) == NULL)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }

        memcpy(pTmpUser->pCachePW, HashData, STORAGE_RC4_KEYLEN);

    }    

    fRet = TRUE;
Ret:
    if (szLowerUserName)
        _nt_free(szLowerUserName, cbsize);
    if (szResource)
        _nt_free(szResource, cbsize);
#ifndef _XBOX
    if (handle)
        FreeLibrary(handle);
#endif

    return fRet;

}

/*
 * Retrieve the security descriptor for a registry key
 */
BOOL GetRegKeySecDescr(
                       PNTAGUserList pUser,
                       HKEY hRegKey,
                       BYTE **ppbSecDescr,
                       DWORD *pcbSecDescr,
                       DWORD *pdwSecDescrFlags
                       )
{
    DWORD   cb = 0;
    DWORD   dwErr = 0;
    BOOL    fRet = FALSE;

    if (pUser->dwOldKeyFlags & PRIVILEDGE_FOR_SACL)
    {
        *pdwSecDescrFlags = SACL_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION |
                            DACL_SECURITY_INFORMATION;
    }
    else
    {
        *pdwSecDescrFlags = GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;
    }

    // get the security descriptor for the hKey of the keyset
    if (ERROR_INSUFFICIENT_BUFFER !=
        (dwErr = RegGetKeySecurity(hRegKey,
                                     (SECURITY_INFORMATION)*pdwSecDescrFlags,
                                     &cb, &cb)))
    {
        SetLastError(dwErr);
        goto Ret;
    }

    if (NULL == (*ppbSecDescr = (BYTE*)_nt_malloc(cb)))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    if (dwErr = RegGetKeySecurity(hRegKey,
                                    (SECURITY_INFORMATION)*pdwSecDescrFlags,
                                    (PSECURITY_DESCRIPTOR)*ppbSecDescr,
                                    &cb))
    {
        SetLastError(dwErr);
        goto Ret;
    }

    *pcbSecDescr = cb;
    fRet = TRUE;
Ret:
    if ((FALSE == fRet) && *ppbSecDescr)
    {
        _nt_free(*ppbSecDescr, cb);
        *ppbSecDescr = NULL;
    }

    return fRet;
}

/*
 * Retrieve the keys from persistant storage
 *
 * NOTE: caller must have zeroed out pUser to allow for non-existent keys
 */
// MTS: Assumes the registry won't change between ReadKey calls.
BOOL RestoreUserKeys(
                     HKEY hKeys,
                     PNTAGUserList pUser,
                     char *User
                     )
{
    PNTAGKeyList        pTmpKey;
    HCRYPTKEY           hKey = 0;
    BYTE                rgbKeyBuffer[STORAGE_RC4_TOTALLEN];
    CRYPT_DATA_BLOB     sSaltData;
    DWORD               dwFlags = 0;
    BOOL                fMachineKeyset = FALSE;
    BYTE                *pbSecDescr = NULL;
    DWORD               cbSecDescr;
    DWORD               dwSecDescrFlags = 0;
    BOOL                fPrivKey;
    BOOL                fEPbk = FALSE;
    BOOL                fSPbk = FALSE;
    DWORD               dwErr = 0;
    BOOL                fRet = NTF_FAILED;


    if (pUser->pCachePW != NULL)
    {
        ZeroMemory(rgbKeyBuffer, STORAGE_RC4_TOTALLEN);
        CopyMemory(rgbKeyBuffer, pUser->pCachePW, STORAGE_RC4_KEYLEN);
        
        if ((pTmpKey = MakeNewKey(CALG_RC4, 0, STORAGE_RC4_KEYLEN, pUser->hUID,
                                  rgbKeyBuffer, FALSE)) != NULL)
        {
            if (NTLMakeItem(&hKey, KEY_HANDLE, (void *)pTmpKey) == NTF_FAILED)
            {
                FreeNewKey(pTmpKey);
                hKey = 0;
            }
        }

        // set zeroized salt for RSABase compatibility
        sSaltData.pbData = rgbKeyBuffer + STORAGE_RC4_KEYLEN;
        sSaltData.cbData = STORAGE_RC4_TOTALLEN - STORAGE_RC4_KEYLEN;
        if (!CPSetKeyParam(
                   pUser->hUID,
                   hKey,
                   KP_SALT_EX,
                   (PBYTE)&sSaltData,
                   0))
            goto Ret;
    }

    if (ReadKey(hKeys, "EPbK", &pUser->ContInfo.pbExchPub,
	            &pUser->ContInfo.ContLens.cbExchPub, pUser, hKey,
                &fEPbk, TRUE, FALSE) == NTF_FAILED)
    {
        goto Ret;
    }

    if (REG_KEYS == pUser->dwKeyStorageType)
    {
        fPrivKey = TRUE;
        if (ReadKey(hKeys, "EPvK", &pUser->pExchPrivKey,
	                &pUser->ExchPrivLen, pUser, hKey,
                    &fPrivKey, TRUE, FALSE) == NTF_FAILED)
        {
            goto Ret;			// error already set
        }

#ifndef BBN
        fPrivKey = TRUE;
        if (ReadKey(hKeys, "SPvK", &pUser->pSigPrivKey,
	                &pUser->SigPrivLen, pUser, hKey,
                    &fPrivKey, FALSE, FALSE) == NTF_FAILED)
        {
            goto Ret;			// error already set
        }
#endif
    }

    if (ReadKey(hKeys, "SPbK", &pUser->ContInfo.pbSigPub,
	            &pUser->ContInfo.ContLens.cbSigPub, pUser, hKey,
                &fSPbk, FALSE, TRUE) == NTF_FAILED)
    {
        goto Ret;			// error already set
    }

    // get the security descriptor for the old keyset
    GetRegKeySecDescr(pUser, hKeys, &pbSecDescr, &cbSecDescr, &dwSecDescrFlags);

    if (CRYPT_MACHINE_KEYSET & pUser->Rights)
    {
        fMachineKeyset = TRUE;
    }

    // if keys are in the protected storage then migrate them
    if (PROTECTED_STORAGE_KEYS == pUser->dwKeyStorageType)
    {
        // migrate the keys from the protected storage
        if (!MigrateProtectedStorageKeys(pUser,
                                         g_Strings.pwszMigrKeys,
                                         fSPbk,
                                         fEPbk))
        {
            goto Ret;
        }
    }
    else if (!(pUser->dwOldKeyFlags & LEAVE_OLD_KEYS))
    {
        if ((NULL == pUser->pSigPrivKey) &&
            (NULL == pUser->pExchPrivKey))
        {
            pUser->dwKeyStorageType = PROTECTION_API_KEYS;
            if (0 != (dwErr = WriteContainerInfo(pUser->dwProvType,
                             pUser->ContInfo.rgwszFileName,
                             fMachineKeyset,
                             &pUser->ContInfo)))
            {
                goto Ret;
            }
        }
        else
        {
            // migrate the keys to use protection APIs
            if (pUser->pSigPrivKey)
            {
                if (!ProtectPrivKey(pUser, NULL, 0, TRUE))
                    goto Ret;
            }
            if (pUser->pExchPrivKey)
            {
                if (!ProtectPrivKey(pUser, NULL, 0, FALSE))
                    goto Ret;
            }
        }

        RegCloseKey(pUser->hKeys);
        pUser->hKeys = 0;

        if (pUser->Rights & CRYPT_MACHINE_KEYSET)
            dwFlags = CRYPT_MACHINE_KEYSET;

        // delete the registry key
        if (!DeleteOldUserKeyGroup(pUser->ContInfo.pszUserName,
                                   pUser->dwProvType,
                                   dwFlags))
        {
            goto Ret;
        }
    }
    // not migrating the keys so just leave things as is and return success
    else
    {
        fRet = NTF_SUCCEED;
        goto Ret;
    }

    fRet = NTF_SUCCEED;
Ret:
    if (pbSecDescr)
        _nt_free(pbSecDescr, cbSecDescr);

    if (hKey != 0)
    {
        CPDestroyKey(pUser->hUID, hKey);
    }

    return fRet;
}

// This function is for a bug fix and trys to decrypt and re-encrypt the keyset
BOOL MixedCaseKeysetBugCheck(
                             HKEY hKeySetRegKey,
                             PNTAGUserList pTmpUser, 
                             char *szUserName1,
                             DWORD dwFlags
                             )
{
    char        *szUserName2 = NULL;
    char        *szKeyName1 = NULL;
    char        *szKeyName2 = NULL;
    DWORD       cbUserName = 0;
    DWORD       cbKeyName1;
    DWORD       cbKeyName2;
    FILETIME    ft;
    BOOL        fRet = FALSE;
    HKEY        hRegKey = 0;
    DWORD       i;
    DWORD       dwErr;
    DWORD       cSubKeys;
    DWORD       cchMaxClass;
    DWORD       cValues;
    DWORD       cchMaxValueName;
    DWORD       cbMaxValueData;
    DWORD       cbSecurityDesriptor;

    if (!OpenUserReg(szUserName1,
                     pTmpUser->dwProvType,
                     dwFlags,
                     TRUE,
                     &hRegKey,
                     &pTmpUser->dwOldKeyFlags))
    {
        SetLastError((DWORD)NTE_BAD_KEYSET);
        goto Ret;
    }

    cbUserName = lstrlen(szUserName1) + sizeof(char);
    if (NULL == (szUserName2 = _nt_malloc(cbUserName)))
        goto Ret;
    strcpy(szUserName2, szUserName1);
    _strlwr(szUserName2);

    if (ERROR_SUCCESS != RegQueryInfoKey(hRegKey,
                                   NULL,
                                   NULL,
                                   NULL,
                                   &cSubKeys,
                                   &cbKeyName1,
                                   &cchMaxClass,
                                   &cValues,
                                   &cchMaxValueName,
                                   &cbMaxValueData,
                                   &cbSecurityDesriptor,
                                   &ft))
        goto Ret;

    if (NULL == (szKeyName1 = _nt_malloc(cbKeyName1 + 2)))
        goto Ret;
    if (NULL == (szKeyName2 = _nt_malloc(cbKeyName1 + 2)))
        goto Ret;

    for (i=0;i<cSubKeys;i++)
    {
        cbKeyName2 = cbKeyName1 + 2;
        if (ERROR_SUCCESS != RegEnumKeyEx(hRegKey,
                                          i,
                                          szKeyName1,
                                          &cbKeyName2,
                                          NULL,
                                          NULL,
                                          NULL,
                                          &ft))
        {
            dwErr = GetLastError();
            goto Ret;
        }

        lstrcpy(szKeyName2, szKeyName1);
        _strlwr(szKeyName1);
        if (0 == lstrcmp(szKeyName1, szUserName2))
        {
            _nt_free(pTmpUser->pCachePW, STORAGE_RC4_KEYLEN);
            pTmpUser->pCachePW = NULL;
            if (NTF_FAILED == LoadWin96Cache(pTmpUser,
                                             szKeyName2,
                                             dwFlags,
                                             FALSE))
            {
                goto Ret;
            }
            if (NTF_FAILED == RestoreUserKeys(hKeySetRegKey,
                                              pTmpUser,
                                              szKeyName2))
            {
                goto Ret;
            }
            break;
        }
    }

    fRet = TRUE;
Ret:
    if (hRegKey)
        RegCloseKey(hRegKey);
    if (szUserName2) 
        _nt_free(szUserName2, cbUserName);
    if (szKeyName1) 
        _nt_free(szKeyName1, cbKeyName1 + 2);
    if (szKeyName2) 
        _nt_free(szKeyName2, cbKeyName1 + 2);
    return fRet;
}

PNTAGUserList InitUser()
{
    PNTAGUserList pTmpUser = NULL;

    if (NULL != (pTmpUser = (PNTAGUserList) _nt_malloc(sizeof(NTAGUserList))))
    {
        InitializeCriticalSection(&pTmpUser->CritSec);
    }

    // initialize the mod expo offload information
    InitExpOffloadInfo(&pTmpUser->pOffloadInfo);

    pTmpUser->dwEnumalgs = 0xFFFFFFFF;
    pTmpUser->dwEnumalgsEx = 0xFFFFFFFF;
    pTmpUser->hRNGDriver = INVALID_HANDLE_VALUE;

    return pTmpUser;
}

/************************************************************************/
/* LogonUser validates a user and returns the package-specific info for */
/* that user.                                                           */
/************************************************************************/
BOOL NTagLogonUser (
                    char *pszUserID,
                    DWORD dwFlags,
                    void **UserInfo,
                    HCRYPTPROV *phUID,
                    DWORD dwProvType,
                    LPSTR pszProvName
                    )
{
    PNTAGUserList   pTmpUser = NULL;
    DWORD           dwUserLen = 0;
    char            *szUserName = NULL;
    DWORD           dwTemp;
    HKEY            hRegKey;
    BOOL            fProtStor;
    char            random[10];
    DWORD           dwErr = 0;
    BOOL            fInFrance = FALSE;
    BOOL            fRet = NTF_FAILED;

// load the resource strings if this is a static lib
#ifdef STATIC_BUILD
    if (!LoadStrings())
    {
        SetLastError((DWORD) NTE_FAIL);
        goto Ret;
    }
#endif // STATIC_BUILD

    SetMachineGUID();

    // Check for Invalid flags
    if (dwFlags & ~(CRYPT_NEWKEYSET|CRYPT_DELETEKEYSET|CRYPT_VERIFYCONTEXT|
                    CRYPT_MACHINE_KEYSET|CRYPT_SILENT))
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    if (((dwFlags & CRYPT_VERIFYCONTEXT) == CRYPT_VERIFYCONTEXT) &&
        (NULL != pszUserID) && (0x00 != *pszUserID))

    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    // Check that user provided pointer is valid
    if (IsBadWritePtr(phUID, sizeof(HCRYPTPROV)))
    {
        SetLastError((DWORD)ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    // If the user didn't supply a name, then we need to get it
    if (pszUserID != NULL && *pszUserID == '\0' ||
        (pszUserID == NULL &&
         ((dwFlags & CRYPT_VERIFYCONTEXT) != CRYPT_VERIFYCONTEXT)))
    {
        dwUserLen = MAXUIDLEN;

        if ((szUserName = (char *) _nt_malloc(dwUserLen)) == NULL)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }


        if (dwFlags & CRYPT_MACHINE_KEYSET)
        {
            strcpy(szUserName, NTAG_DEF_MACH_CONT_NAME);
            dwTemp = NTAG_DEF_MACH_CONT_NAME_LEN;
        }
        else
        {
            dwTemp = dwUserLen;

            dwTemp = sizeof("*Default*");
            memcpy(szUserName, "*Default*", dwTemp);
        }

    }
    else if (pszUserID != NULL)
    {
        dwUserLen = strlen(pszUserID) + sizeof(CHAR);
        if ((dwFlags & CRYPT_NEWKEYSET) && (dwUserLen > MAX_PATH + 1))
        {
            SetLastError((DWORD)NTE_BAD_KEYSET);
            goto Ret;
        }

        if ((szUserName = (char *) _nt_malloc(dwUserLen)) == NULL)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
        strcpy(szUserName, pszUserID);
    }

    if (dwFlags & CRYPT_DELETEKEYSET)
    {
        if (!DeleteUserKeyGroup(szUserName, dwProvType,
                                dwFlags))
        {
            goto Ret;
        }
        fRet = NTF_SUCCEED;
        goto Ret;
    }

    // Zero to ensure valid fields for non-existent keys
    if (NULL == (pTmpUser = InitUser()))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

#ifdef STRONG
    if (NULL != pszProvName)
    {
        if (0 == lstrcmp(pszProvName, MS_STRONG_PROV))
        {
            pTmpUser->fNewStrongCSP = TRUE;
        }
    }
#endif // STRONG

    pTmpUser->dwProvType = dwProvType;

    // check if local machine keys
    if ((dwFlags & CRYPT_SILENT) || (dwFlags & CRYPT_VERIFYCONTEXT))
    {
        pTmpUser->ContInfo.fCryptSilent = TRUE;
    }

#ifndef _XBOX
    if (((dwFlags & CRYPT_VERIFYCONTEXT) != CRYPT_VERIFYCONTEXT))
    {
        if (0 != (dwErr = OpenUserKeyGroup(pTmpUser, szUserName,
                                           dwFlags, FALSE)))
        {
            SetLastError(dwErr);
            goto Ret;
        }
    }
#endif

    // check if local machine keys
    if (dwFlags & CRYPT_MACHINE_KEYSET)
    {
        pTmpUser->Rights |= CRYPT_MACHINE_KEYSET;
    }

    IsLocalSystem(&pTmpUser->fIsLocalSystem);

    if (!NTLMakeItem(phUID, USER_HANDLE, (void *)pTmpUser))
    {
        goto Ret;
    }

    pTmpUser->hUID = *phUID;

    if (((dwFlags & CRYPT_VERIFYCONTEXT) != CRYPT_VERIFYCONTEXT) &&
        (PROTECTION_API_KEYS != pTmpUser->dwKeyStorageType) && 
        (!(CRYPT_NEWKEYSET & dwFlags)))
    {
        CheckForStorageType(pTmpUser->hKeys, &pTmpUser->dwKeyStorageType);

        if (PROTECTED_STORAGE_KEYS == pTmpUser->dwKeyStorageType)
        {
            // check for PStore availability and use it if its there
            if (NULL == (pTmpUser->pPStore =
                    (PSTORE_INFO*)_nt_malloc(sizeof(PSTORE_INFO))))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            // check if PStore is available
            if (!CheckPStoreAvailability(pTmpUser->pPStore))
            {
                SetLastError((DWORD)NTE_FAIL);
                goto Ret;
            }

            if (!GetKeysetTypeAndSubType(pTmpUser))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }
        }

        // migrate old keys
        if ((REG_KEYS == pTmpUser->dwKeyStorageType) ||
            (PROTECTED_STORAGE_KEYS == pTmpUser->dwKeyStorageType))
        {
            if (!LoadWin96Cache(pTmpUser,
                                pTmpUser->ContInfo.pszUserName,
                                dwFlags,
                                TRUE))
            {
                goto Ret;          // error already set
            }

            if (!RestoreUserKeys(pTmpUser->hKeys, pTmpUser, szUserName))
            {
                if (!MixedCaseKeysetBugCheck(pTmpUser->hKeys, pTmpUser,
                                             szUserName, dwFlags))
                {
                    SetLastError((DWORD)NTE_BAD_KEYSET);
                    goto Ret;
                }
            }
        }
        else
        {
            SetLastError((DWORD)NTE_BAD_KEYSET);
            goto Ret;
        }
    }

    //
    // If lang French disable encryption
    //
#ifndef STATIC_BUILD
    if (!IsEncryptionPermitted(pTmpUser->dwProvType, &fInFrance))
    {
        pTmpUser->Rights |= CRYPT_DISABLE_CRYPT;
    }
    if (fInFrance)
    {
        pTmpUser->Rights |= CRYPT_IN_FRANCE;
    }
#endif

    if (pTmpUser->ContInfo.ContLens.cbExchPub && pTmpUser->ContInfo.pbExchPub)
    {
        BSAFE_PUB_KEY *pPubKey = (BSAFE_PUB_KEY *)pTmpUser->ContInfo.pbExchPub;

        if (PROV_RSA_SCHANNEL == pTmpUser->dwProvType)
        {
#ifndef STRONG
            if ((pPubKey->bitlen / 8) > SGC_RSA_MAX_EXCH_MODLEN)
            {
                SetLastError((DWORD)NTE_BAD_KEYSET);
                goto Ret;
            }
#endif
        }
        else
        {
            if (pTmpUser->Rights & CRYPT_IN_FRANCE)
            {
                if ((pPubKey->bitlen / 8) > RSA_MAX_EXCH_FRENCH_MODLEN)
                {
                    SetLastError((DWORD)NTE_BAD_KEYSET);
                    goto Ret;
                }
            }
            else
            {
                if ((pPubKey->bitlen / 8) > RSA_MAX_EXCH_MODLEN)
                {
                    SetLastError((DWORD)NTE_BAD_KEYSET);
                    goto Ret;
                }
            }
        }
    }

#ifdef TEST_HW_RNG
    // checks if the HWRNG is to be used
    if (!SetupHWRNGIfRegistered(&pTmpUser->hRNGDriver))
    {
        SetLastError((DWORD)NTE_FAIL);
        goto Ret;
    }
#endif // 0

    if (!FIPS186GenRandom(&pTmpUser->hRNGDriver,
                          &pTmpUser->ContInfo.pbRandom,
                          &pTmpUser->ContInfo.ContLens.cbRandom,
                          random, sizeof(random)))
    {
        goto Ret;
    }

#ifdef STRONG
    if ((!pTmpUser->fNewStrongCSP) &&
        (ERROR_SUCCESS == RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                                        "Software\\Microsoft\\Cryptography\\DESHashSessionKeyBackward",
                                        0,
                                        KEY_READ,
                                        &hRegKey)))
    {
        pTmpUser->Rights |= CRYPT_DES_HASHKEY_BACKWARDS;
        RegCloseKey(hRegKey);
        hRegKey = 0;
    }
#endif // STRONG

    if (dwFlags & CRYPT_VERIFYCONTEXT)
    {
        pTmpUser->Rights |= CRYPT_VERIFYCONTEXT;
    }

    *UserInfo = pTmpUser;
    fRet = NTF_SUCCEED;
Ret:
    if (szUserName)
        _nt_free(szUserName, dwUserLen);
    if (NTF_SUCCEED != fRet)
    {
        dwErr = GetLastError();

        if (NULL != pTmpUser)
        {
            FreeUserRec(pTmpUser);
        }
        SetLastError(dwErr);
    }
    return fRet;
}

/************************************************************************/
/* LogoffUser removes a user from the user list.  The handle to that	*/
/* will therefore no longer be valid.					*/
/************************************************************************/
BOOL LogoffUser (void *UserInfo)
{
    PNTAGUserList	pTmpUser = (PNTAGUserList)UserInfo;
    HKEY                hKeys;

    hKeys = pTmpUser->hKeys;
    FreeUserRec(pTmpUser);

    if (RegCloseKey(hKeys) != ERROR_SUCCESS)
    {
	; // debug message, notify user somehow?
    }

    return NTF_SUCCEED;
}

/*
 -	CPAcquireContext
 -
 *	Purpose:
 *               The CPAcquireContext function is used to acquire a context
 *               handle to a cryptograghic service provider (CSP).
 *
 *
 *	Parameters:
 *               OUT phUID         -  Handle to a CSP
 *               IN  pUserID       -  Pointer to a string which is the
 *                                    identity of the logged on user
 *               IN  dwFlags       -  Flags values
 *               IN  pVTable       -  Pointer to table of function pointers
 *
 *	Returns:
 */
BOOL CPAcquireContext(OUT HCRYPTPROV *phUID,
                      IN  CHAR *pUserID,
                      IN DWORD dwFlags,
                      IN PVTableProvStruc pVTable)
{
    void            *UserData;
    PNTAGUserList   pTmpUser;
    DWORD           dwProvType = PROV_RSA_FULL;
    LPSTR           pszProvName = NULL;
    BOOL            fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputAcqCtxt(TRUE, FALSE, phUID, pUserID, dwFlags, pVTable);
#endif // DBG -- NOTE:  This section not compiled for retail builds 

    if (pVTable->Version >= 2)
        dwProvType = pVTable->dwProvType;

    if (pVTable->Version >= 3)
        pszProvName = pVTable->pszProvName;

    if (NTF_FAILED == NTagLogonUser(pUserID, dwFlags, &UserData,
                                    phUID, dwProvType, pszProvName))
    {
        goto Ret;
    }

    if (dwFlags & CRYPT_DELETEKEYSET)
    {
        fRet = NTF_SUCCEED;
        goto Ret;
    }

    pTmpUser = (PNTAGUserList) UserData;
    pTmpUser->hPrivuid = 0;

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputAcqCtxt(FALSE, fRet, NULL, NULL, 0, NULL);
#endif // DBG -- NOTE:  This section not compiled for retail builds 

    return fRet;
}

/*
 -      CPReleaseContext
 -
 *      Purpose:
 *               The CPReleaseContext function is used to release a
 *               context created by CrytAcquireContext.
 *
 *     Parameters:
 *               IN  hUID          -  Handle to a CSP
 *               IN  dwFlags       -  Flags values
 *
 *	Returns:
 */
BOOL CPReleaseContext(IN HCRYPTPROV hUID,
                      IN DWORD dwFlags)
{
    void	*UserData = NULL;
    BOOL	f = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputReleaseCtxt(TRUE, FALSE, hUID, dwFlags);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    // check to see if this is a valid user handle
    // ## MTS: No user structure locking
    if ((UserData = NTLCheckList (hUID, USER_HANDLE)) == NULL)
    {
	    SetLastError((DWORD) NTE_BAD_UID);
	    goto Ret;
    }

    f = LogoffUser (UserData);
Ret:
    // Remove from internal list first so others
    // can't get to it, then logoff the current user
    if (UserData)
        NTLDelete(hUID);

    // Check for Invalid flags
    if (dwFlags != 0)
    {
	    SetLastError((DWORD) NTE_BAD_FLAGS);
        f = NTF_FAILED;
    }


#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputReleaseCtxt(FALSE, f, 0, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds 

    return f;

}


BOOL RemovePublicKeyExportability(IN PNTAGUserList pUser,
                                  IN BOOL fExchange)
{
    DWORD dwType;
    DWORD cb;
    BOOL fRet = FALSE;

    if (fExchange)
    {
        if (ERROR_SUCCESS == RegQueryValueEx(pUser->hKeys, "EExport", NULL,
                                             &dwType, NULL, &cb))
        {
            if (ERROR_SUCCESS != RegDeleteValue(pUser->hKeys, "EExport"))
            {
                goto Ret;
            }
        }
    }
    else
    {
        if (ERROR_SUCCESS == RegQueryValueEx(pUser->hKeys, "SExport", NULL,
                                             &dwType, NULL, &cb))
        {
            if (ERROR_SUCCESS != RegDeleteValue(pUser->hKeys, "SExport"))
            {
                goto Ret;
            }
        }
    }

    fRet = TRUE;

Ret:
    return fRet;
}

BOOL MakePublicKeyExportable(IN PNTAGUserList pUser,
                             IN BOOL fExchange)
{
    BYTE b = 0x01;
    BOOL fRet = FALSE;
	
    if (fExchange)
    {
        if (ERROR_SUCCESS != RegSetValueEx(pUser->hKeys, "EExport", 0,
                                           REG_BINARY, &b, sizeof(b)))
        {
            goto Ret;
        }
    }
    else
    {
        if (ERROR_SUCCESS != RegSetValueEx(pUser->hKeys, "SExport", 0,
                                           REG_BINARY, &b, sizeof(b)))
        {
            goto Ret;
        }
    }

    fRet = TRUE;

Ret:
    return fRet;
}

BOOL CheckPublicKeyExportability(IN PNTAGUserList pUser,
                                 IN BOOL fExchange)
{
    DWORD dwType;
    DWORD cb = 1;
    BYTE b;
    BOOL fRet = FALSE;

    if (fExchange)
    {
        if (ERROR_SUCCESS != RegQueryValueEx(pUser->hKeys, "EExport", NULL,
                                             &dwType, &b, &cb))
        {
            goto Ret;
        }
    }
    else
    {
        if (ERROR_SUCCESS != RegQueryValueEx(pUser->hKeys, "SExport", NULL,
                                             &dwType, &b, &cb))
        {
            goto Ret;
        }
    }
	
    if ((sizeof(b) != cb) || (0x01 != b))
    {
        goto Ret;
    }

    fRet = TRUE;

Ret:
    return fRet;
}
