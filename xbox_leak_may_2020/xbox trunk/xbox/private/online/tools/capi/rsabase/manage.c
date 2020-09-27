/////////////////////////////////////////////////////////////////////////////
//  FILE          : manage.c                                               //
//  DESCRIPTION   : Misc list/memory management routines.                  //
//  AUTHOR        :                                                        //
//  HISTORY       :                                                        //
//	Jan 25 1995 larrys  Changed from Nametag                               //
//      Feb 23 1995 larrys  Changed NTag_SetLastError to SetLastError      //
//      Apr 19 1995 larrys  Cleanup                                        //
//      Sep 11 1995 Jeffspel/ramas  Merge STT into default CSP             //
//      Oct 27 1995 rajeshk  RandSeed Stuff added hUID to PKCS2Encrypt     //
//      Nov  3 1995 larrys  Merge for NT checkin                           //
//      Nov 13 1995 larrys  Fixed memory leak                              //
//      Dec 11 1995 larrys  Added WIN95 password cache                     //
//      Dec 13 1995 larrys  Remove MTS stuff                               //
//      May 15 1996 larrys  Remove old cert stuff                          //
//      May 28 1996 larrys  Added Win95 registry install stuff             //
//      Jun 12 1996 larrys  Encrypted public keys                          //
//      Jun 26 1996 larrys  Put rsabase.sig into a resource for regsrv32   //
//      Sep 16 1996 mattt   Added Strong provider type in #define          //
//      Oct 14 1996 jeffspel Changed GenRandom to NewGenRandom             //
//      May 23 1997 jeffspel Added provider type checking                  //
//                                                                         //
//  Copyright (C) 1993 Microsoft Corporation   All Rights Reserved         //
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"
#include "resource.h"
#include "nt_rsa.h"
#include "randlib.h"
#include "protstor.h"
#include "ole2.h"
#include "swnt_pk.h"
#include "sgccheck.h"

#define	MAXITER		0xFFFF

#define RSAFULL_TYPE_STRING     "Type 001"
#define RSA_SCH_TYPE_STRING     "Type 012"

#define MS_RSA_TYPE     "RSA Full (Signature and Key Exchange)"
#define MS_RSA_SCH_TYPE "RSA SChannel"

#define PROVPATH        "SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider\\"
#define PROVPATH_LEN    sizeof(PROVPATH)

#define TYPEPATH        "SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider Types\\"
#define TYPEPATH_LEN    sizeof(TYPEPATH)

HINSTANCE hInstance;

BOOL WINAPI FIsWinNT(void);

#ifdef STRONG
CHAR szImagePath[] = "rsaenh.dll";
#define SIG_RESOURCE_NUM    2
#else
CHAR szImagePath[] = "rsabase.dll";
#define SIG_RESOURCE_NUM    1
#endif // STRONG

// MAC in file
#define MAC_RESOURCE_NUM    "#667"

#define KEYSIZE1024 0x88

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



DWORD NTVersion(void)
{
    return 5;
}

BOOL SetCSPInfo(
                LPSTR pszProvider,
                LPSTR pszImagePath,
                BYTE *pbSig,
                DWORD cbSig,
                DWORD dwProvType,
                LPSTR pszType,
                LPSTR pszTypeName,
                BOOL fSigInFile,
                BOOL fMakeDefault
                )
{
#ifndef _XBOX
    DWORD   dwIgn;
    HKEY    hKey = 0;
    HKEY    hTypeKey = 0;
    DWORD   cbProv;
    BYTE    *pszProv = NULL;
    DWORD   cbTypePath;
    BYTE    *pszTypePath = NULL;
    DWORD   dwVal = 0;
    DWORD   err;
    BOOL    fRet = FALSE;

    cbProv = PROVPATH_LEN + strlen(pszProvider);
    if (NULL == (pszProv = (LPSTR)LocalAlloc(LMEM_ZEROINIT, cbProv)))
        goto Ret;

    strcpy(pszProv, PROVPATH);
    strcat(pszProv, pszProvider);

	//
	// Create or open in local machine for provider:
	//
    if (ERROR_SUCCESS != (err = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                                               (const char *)pszProv,
                                               0L, "", REG_OPTION_NON_VOLATILE,
                                               KEY_ALL_ACCESS, NULL, &hKey,
                                               &dwIgn)))
        goto Ret;

	//
	// Set Image path to: scp.dll
	//
    if (ERROR_SUCCESS != (err = RegSetValueEx(hKey, "Image Path", 0L, REG_SZ,
	                                          pszImagePath,
                                              strlen(pszImagePath) + 1)))
        goto Ret;

	//
	// Set Type to: Type 003
	//
    if (ERROR_SUCCESS != (err = RegSetValueEx(hKey, "Type", 0L, REG_DWORD,
                                              (LPTSTR)&dwProvType,
                                              sizeof(DWORD))))
        goto Ret;

    if (fSigInFile)
    {
	    //
	    // Place signature in file value
	    //
        if (ERROR_SUCCESS != (err = RegSetValueEx(hKey, "SigInFile", 0L,
                                                  REG_DWORD, (LPTSTR)&dwVal,
                                                  sizeof(DWORD))))
            goto Ret;
    }
    else
    {
	    //
	    // Place signature
	    //
        if (ERROR_SUCCESS != (err = RegSetValueEx(hKey, "Signature", 0L,
                                                  REG_BINARY, pbSig, cbSig)))
            goto Ret;
    }

	//
	// Create or open in local machine for provider type:
	//

    cbTypePath = TYPEPATH_LEN + strlen(pszType) + 1;
    if (NULL == (pszTypePath = (LPSTR)LocalAlloc(LMEM_ZEROINIT, cbTypePath)))
        goto Ret;

    strcpy(pszTypePath, TYPEPATH);
    strcat(pszTypePath, pszType);

    if (ERROR_SUCCESS != (err = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                                               (const char *) pszTypePath,
                                               0L, "", REG_OPTION_NON_VOLATILE,
                                               KEY_ALL_ACCESS, NULL, &hTypeKey,
                                               &dwIgn)))
        goto Ret;

    if ((REG_CREATED_NEW_KEY == dwIgn) || fMakeDefault)
    {
        if (ERROR_SUCCESS != (err = RegSetValueEx(hTypeKey, "Name", 0L,
                                                  REG_SZ, pszProvider,
                                                  strlen(pszProvider) + 1)))
            goto Ret;

        if (ERROR_SUCCESS != (err = RegSetValueEx(hTypeKey, "TypeName", 0L,
                                                  REG_SZ, pszTypeName,
                                                  strlen(pszTypeName) + 1)))
            goto Ret;
    }

    fRet = TRUE;
Ret:
    if (hKey)
        RegCloseKey(hKey);
    if (hTypeKey)
        RegCloseKey(hTypeKey);
    if (pszProv)
        LocalFree(pszProv);
    if (pszTypePath)
        LocalFree(pszTypePath);
    return fRet;
#else
    ASSERT( !"SetCSPInfo" );
    return TRUE;
#endif
}

BOOL LoadWin96Cache(PNTAGUserList pTmpUser);

void EncryptKey(BYTE *pdata, DWORD size, BYTE val);

void FreeNewKey(PNTAGKeyList pOldKey);

BOOL SelfMACCheck(
                  IN LPSTR pszImage
                  );


// See if a handle of the given type is in the list.
// If it is, return the item itself.

static void *
NTLFindItem(HNTAG hThisThing, BYTE bTypeValue)
{
    HTABLE *pTable;

    pTable = (HTABLE*)(hThisThing ^ HANDLE_MASK);

	if ((BYTE)pTable->dwType != bTypeValue)
		return NULL;

	return (void*)pTable->pItem;
}

// See if a handle of the given type is in the list.
// If it is, return the data that the item holds.
void *
NTLCheckList(HNTAG hThisThing, BYTE bTypeValue)
{
	return NTLFindItem(hThisThing, bTypeValue);
}

// Find & validate the passed list item against the user and type.

void *NTLValidate(HNTAG hItem, HCRYPTPROV hUID, BYTE bTypeValue)
{
	void		*pTmpVal;

	// check to see if the key is in the key list
	if ((pTmpVal = NTLCheckList (hItem, bTypeValue)) == NULL)
	{
		SetLastError((DWORD) NTE_FAIL);		// converted by caller
		return NULL;
	}

	// check to make sure there is a key value
	if ((bTypeValue == KEY_HANDLE) &&
	    (((PNTAGKeyList)pTmpVal)->pKeyValue == NULL))
	{
		ASSERT(((PNTAGKeyList)pTmpVal)->cbKeyLen == 0);
		SetLastError((DWORD) NTE_BAD_KEY);
		return NULL;
	}

	// make sure the UIDs are the same
	if (((PNTAGKeyList)pTmpVal)->hUID != hUID)
	{
		SetLastError((DWORD) NTE_BAD_UID);
		return NULL;
	}

	return pTmpVal;
}

// Make a new list item of the given type, and assign the data to it.

BOOL NTLMakeItem(HNTAG *phItem, BYTE bTypeValue, void *NewData)
{
	HTABLE   *NewMember;

	if ((NewMember = (HTABLE *)_nt_malloc(sizeof(HTABLE))) == NULL)
    {
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return NTF_FAILED;
	}

	NewMember->pItem = NewData;
    NewMember->dwType = bTypeValue;
	
    *phItem = (HNTAG)((HNTAG)NewMember ^ HANDLE_MASK);
	return NTF_SUCCEED;
}

// Remove the handle.  Assumes that any memory used by the handle data has been freed.

BOOL NTLDelete(HNTAG hItem)
{
    HTABLE  *pTable;

    pTable = (HTABLE*)(hItem ^ HANDLE_MASK);

	_nt_free(pTable, sizeof(HTABLE));

	return NTF_SUCCEED;
}

/****************************************************************/
/* FreeUserRec frees the dynamically allocated memory for the	*/
/* appropriate fields of the UserRec structure.					*/
/*																*/
/* MTS: we assume that pUser has only one reference (no			*/
/* MTS: multiple logon's for the same user name).				*/
/****************************************************************/

void
FreeUserRec (PNTAGUserList pUser)
{
    if (pUser != NULL)
    {
        // No need to zero lengths, since entire struct is going away
        if (pUser->pExchPrivKey)
        {
            ZeroMemory( pUser->pExchPrivKey, pUser->ExchPrivLen );
            _nt_free (pUser->pExchPrivKey, 0);
        }

        if (pUser->pSigPrivKey)
        {
            ZeroMemory( pUser->pSigPrivKey, pUser->SigPrivLen );
            _nt_free (pUser->pSigPrivKey, 0);
        }

        if (pUser->pUser)
        {
            _nt_free(pUser->pUser, 0);
        }

        if (pUser->pCachePW)
        {
            ZeroMemory(pUser->pCachePW, STORAGE_RC4_KEYLEN);
            _nt_free(pUser->pCachePW, 0);
        }

        if (pUser->pPStore)
        {
            FreePSInfo(pUser->pPStore);
        }

        // free the SGC key info
#ifndef _XBOX
        SGCDeletePubKeyValues(&pUser->pbSGCKeyMod,
                              &pUser->cbSGCKeyMod,
                              &pUser->dwSGCKeyExpo);
#endif

        FreeContainerInfo(&pUser->ContInfo);

        FreeOffloadInfo(pUser->pOffloadInfo);

        DeleteCriticalSection(&pUser->CritSec);

        ZeroMemory(pUser, sizeof(NTAGUserList));
        _nt_free (pUser, 0);
    }
}

// RSA private key in PRIVATEKEYBLOB format 
static BYTE rgbRSAPriv[] =
{
0x52, 0x53, 0x41, 0x32, 0x48, 0x00, 0x00, 0x00,
0x00, 0x02, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00,
0x01, 0x00, 0x01, 0x00, 0xEF, 0x4C, 0x0D, 0x34,
0xCF, 0x44, 0x0F, 0xB1, 0x73, 0xAC, 0xD4, 0x9B,
0xBE, 0xCC, 0x2D, 0x11, 0x2A, 0x2B, 0xBD, 0x21,
0x04, 0x8E, 0xAC, 0xAD, 0xD5, 0xFC, 0xD2, 0x50,
0x14, 0x35, 0x1B, 0x43, 0x15, 0x62, 0x67, 0x8F,
0x5E, 0x00, 0xB9, 0x25, 0x1B, 0xE2, 0x4F, 0xBE,
0xA1, 0x50, 0xA1, 0x44, 0x3B, 0x17, 0xD8, 0x91,
0xF5, 0x28, 0xF9, 0xFA, 0xAE, 0xE7, 0xC0, 0xFD,
0xB9, 0xCD, 0x76, 0x4F, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xE9, 0xBB, 0x38, 0x52,
0xD9, 0x0D, 0x56, 0xD7, 0x36, 0xBA, 0xDC, 0xE8,
0xB5, 0x57, 0x56, 0x13, 0x1A, 0x3A, 0x43, 0x30,
0xDE, 0x7D, 0x76, 0x6F, 0xBB, 0x71, 0x3B, 0x0A,
0x92, 0xBA, 0x60, 0x94, 0x00, 0x00, 0x00, 0x00,
0x17, 0x33, 0x3D, 0xB5, 0xEF, 0xD8, 0x2B, 0xDE,
0xCD, 0xA6, 0x6A, 0x94, 0x17, 0xC3, 0x57, 0xE9,
0x2E, 0x1C, 0x9F, 0x35, 0xDA, 0xA4, 0xBD, 0x02,
0x5B, 0x9D, 0xD1, 0x38, 0x4C, 0xF2, 0x19, 0x89,
0x00, 0x00, 0x00, 0x00, 0x89, 0x21, 0xCB, 0x3F,
0x0C, 0xA7, 0x71, 0xBC, 0xF6, 0xA1, 0x87, 0xDF,
0x00, 0x2D, 0x27, 0x64, 0x4A, 0xD4, 0x93, 0x9F,
0x58, 0x93, 0x4B, 0x83, 0x1E, 0xAB, 0xD8, 0x5D,
0xBC, 0x0E, 0x58, 0x03, 0x00, 0x00, 0x00, 0x00,
0xAB, 0x09, 0xD7, 0x21, 0xBA, 0x6F, 0x55, 0x08,
0x12, 0xEE, 0x5B, 0x47, 0x6B, 0x9F, 0x3F, 0xD3,
0xFC, 0xEA, 0xB5, 0x25, 0x19, 0xB7, 0x9E, 0xBD,
0xDF, 0x6F, 0x7F, 0x96, 0x00, 0x88, 0xC6, 0x7B,
0x00, 0x00, 0x00, 0x00, 0x95, 0x0B, 0x23, 0xC5,
0x72, 0x98, 0x9D, 0x49, 0x7A, 0x46, 0x4E, 0xE1,
0xE6, 0x2F, 0xC6, 0x63, 0x21, 0x8F, 0x66, 0xDC,
0x9B, 0xCC, 0xE2, 0x27, 0x03, 0x27, 0x85, 0xF0,
0x3A, 0x02, 0xFB, 0x40, 0x00, 0x00, 0x00, 0x00,
0x51, 0x74, 0xF6, 0xF2, 0x23, 0xEC, 0xA1, 0x76,
0x55, 0x58, 0x07, 0x71, 0xBF, 0x7F, 0x0A, 0x1E,
0x6B, 0x48, 0x48, 0xBB, 0x92, 0xB6, 0x2A, 0xB1,
0x07, 0xA4, 0x21, 0xD1, 0xC6, 0xCB, 0x5F, 0x40,
0xCE, 0xDD, 0xBA, 0xDB, 0xFC, 0x17, 0xFB, 0xA7,
0xBD, 0xE1, 0xF4, 0x63, 0xD8, 0x9E, 0x89, 0xE2,
0xDD, 0x7A, 0xEC, 0x11, 0xD6, 0xA9, 0x9C, 0xBA,
0xC7, 0x5E, 0x35, 0x96, 0xA6, 0x6F, 0x7F, 0x2C,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// RSA private key in PUBLICKEYBLOB format 
static BYTE rgbRSAPub[] =
{
0x52, 0x53, 0x41, 0x31, 0x48, 0x00, 0x00, 0x00,
0x00, 0x02, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00,
0x01, 0x00, 0x01, 0x00, 0xEF, 0x4C, 0x0D, 0x34,
0xCF, 0x44, 0x0F, 0xB1, 0x73, 0xAC, 0xD4, 0x9B,
0xBE, 0xCC, 0x2D, 0x11, 0x2A, 0x2B, 0xBD, 0x21,
0x04, 0x8E, 0xAC, 0xAD, 0xD5, 0xFC, 0xD2, 0x50,
0x14, 0x35, 0x1B, 0x43, 0x15, 0x62, 0x67, 0x8F,
0x5E, 0x00, 0xB9, 0x25, 0x1B, 0xE2, 0x4F, 0xBE,
0xA1, 0x50, 0xA1, 0x44, 0x3B, 0x17, 0xD8, 0x91,
0xF5, 0x28, 0xF9, 0xFA, 0xAE, 0xE7, 0xC0, 0xFD,
0xB9, 0xCD, 0x76, 0x4F, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};

// known result of an MD5 hash on the above buffer
static BYTE rgbKnownMD5[] =
{
0xb8, 0x2f, 0x6b, 0x11, 0x31, 0xc8, 0xec, 0xf4,
0xfe, 0x0b, 0xf0, 0x6d, 0x2a, 0xda, 0x3f, 0xc3
};

// known result of an SHA-1 hash on the above buffer
static BYTE rgbKnownSHA1[] =
{
0xe8, 0x96, 0x82, 0x85, 0xeb, 0xae, 0x01, 0x14,
0x73, 0xf9, 0x08, 0x45, 0xc0, 0x6a, 0x6d, 0x3e,
0x69, 0x80, 0x6a, 0x0c
};

// known key, plaintext, and ciphertext for RC4
static BYTE rgbRC4Key[] = {0x61, 0x8a, 0x63, 0xd2, 0xfb};
static BYTE rgbRC4KnownPlaintext[] = {0xDC, 0xEE, 0x4C, 0xF9, 0x2C};
static BYTE rgbRC4KnownCiphertext[] = {0xF1, 0x38, 0x29, 0xC9, 0xDE};

// IV for all block ciphers
BYTE rgbIV[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF};

// known key, plaintext, and ciphertext for RC2
BYTE rgbRC2Key[] = {0x59, 0x45, 0x9a, 0xf9, 0x27, 0x84, 0x74, 0xCA};
BYTE rgbRC2KnownPlaintext[] = {0xD5, 0x58, 0x75, 0x12, 0xCE, 0xEF, 0x77, 0x93};
BYTE rgbRC2KnownCiphertext[] = {0x7b, 0x98, 0xdf, 0x9d, 0xa2, 0xdc, 0x7b, 0x7a};
BYTE rgbRC2CBCCiphertext[] = {0x9d, 0x93, 0x8e, 0xf6, 0x7c, 0x01, 0x5e, 0xeb};

// known key, plaintext, and ciphertext for DES40 (CALG_CYLINK_MEK)
BYTE rgbDES40Key[] = {0x01, 0x23, 0x04, 0x67, 0x08, 0xab, 0x0d, 0xef};
BYTE rgbDES40KnownPlaintext[] = {0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74};
BYTE rgbDES40KnownCiphertext[] = {0xac, 0x97, 0x4d, 0xd9, 0x02, 0x13, 0x88, 0x2c};
BYTE rgbDES40CBCCiphertext[] = {0x47, 0xdc, 0xf0, 0x13, 0x7f, 0xa5, 0xd6, 0x32};

// known key, plaintext, and ciphertext for DES
BYTE rgbDESKey[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
BYTE rgbDESKnownPlaintext[] = {0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74};
BYTE rgbDESKnownCiphertext[] = {0x3F, 0xA4, 0x0E, 0x8A, 0x98, 0x4D, 0x48, 0x15};
BYTE rgbDESCBCCiphertext[] = {0xE5, 0xC7, 0xCD, 0xDE, 0x87, 0x2B, 0xF2, 0x7C};

// known key, plaintext, and ciphertext for 3 key 3DES
BYTE rgb3DESKey[] =
{
0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01,
0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23
};
BYTE rgb3DESKnownPlaintext[] = {0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74};
BYTE rgb3DESKnownCiphertext[] = {0x31, 0x4F, 0x83, 0x27, 0xFA, 0x7A, 0x09, 0xA8};
BYTE rgb3DESCBCCiphertext[] = {0xf3, 0xc0, 0xff, 0x02, 0x6c, 0x02, 0x30, 0x89};

// known key, plaintext, and ciphertext for 2 key 3DES
BYTE rgb3DES112Key[] =
{
0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01
};
BYTE rgb3DES112KnownPlaintext[] = {0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74};
BYTE rgb3DES112KnownCiphertext[] = {0xb7, 0x83, 0x57, 0x79, 0xee, 0x26, 0xac, 0xb7};
BYTE rgb3DES112CBCCiphertext[] = {0x13, 0x4b, 0x98, 0xf8, 0xee, 0xb3, 0xf6, 0x07};

// **********************************************************************
// AlgorithmCheck performs known answer tests using the algorithms
// supported by the provider.
// **********************************************************************
BOOL AlgorithmCheck()
{
    BYTE        rgbMD5[MD5DIGESTLEN];
    BYTE        rgbSHA1[A_SHA_DIGEST_LEN]; 
    BOOL        fRet = FALSE;

    memset(rgbMD5, 0, sizeof(rgbMD5));
    memset(rgbSHA1, 0, sizeof(rgbSHA1));

    // check if RSA is working properly
    if (0 != EncryptAndDecryptWithRSAKey(rgbRSAPub, rgbRSAPriv, TRUE, TRUE))
    {
        goto Ret;
    }

    // check if RSA is working properly
    if (0 != EncryptAndDecryptWithRSAKey(rgbRSAPub, rgbRSAPriv, FALSE, TRUE))
    {
        goto Ret;
    }

#ifdef CSP_USE_MD5
    // known answer test with MD5 (this function is found in hash.c)
    if (!TestMD5("HashThis", 8, rgbMD5))
    {
        goto Ret;
    }
    if (memcmp(rgbMD5, rgbKnownMD5, sizeof(rgbMD5)))
    {
        goto Ret;
    }
#endif // CSP_USE_MD5

#ifdef CSP_USE_SHA1
    // known answer test with SHA-1  (this function is found in hash.c)
    if (!TestSHA1("HashThis", 8, rgbSHA1))
    {
        goto Ret;
    }
    if (memcmp(rgbSHA1, rgbKnownSHA1, sizeof(rgbSHA1)))
    {
        goto Ret;
    }
#endif // CSP_USE_SHA1

#ifdef CSP_USE_RC4
    // known answer test with RC4
    if (!TestSymmetricAlgorithm(CALG_RC4, rgbRC4Key, sizeof(rgbRC4Key),
                                rgbRC4KnownPlaintext,
                                sizeof(rgbRC4KnownPlaintext),
                                rgbRC4KnownCiphertext, NULL))
    {
        goto Ret;
    }
#endif // CSP_USE_RC4

#ifdef CSP_USE_RC2
    // known answer test with RC2 - ECB
    if (!TestSymmetricAlgorithm(CALG_RC2, rgbRC2Key, sizeof(rgbRC2Key),
                                rgbRC2KnownPlaintext,
                                sizeof(rgbRC2KnownPlaintext),
                                rgbRC2KnownCiphertext,
                                NULL))
    {
        goto Ret;
    }

    // known answer test with RC2 - CBC
    if (!TestSymmetricAlgorithm(CALG_RC2, rgbRC2Key, sizeof(rgbRC2Key),
                                rgbRC2KnownPlaintext,
                                sizeof(rgbRC2KnownPlaintext),
                                rgbRC2CBCCiphertext,
                                rgbIV))
    {
        goto Ret;
    }

#endif // CSP_USE_RC2

#ifdef CSP_USE_DES
    // known answer test with DES - ECB
    if (!TestSymmetricAlgorithm(CALG_DES, rgbDESKey, sizeof(rgbDESKey),
                                rgbDESKnownPlaintext,
                                sizeof(rgbDESKnownPlaintext),
                                rgbDESKnownCiphertext,
                                NULL))
    {
        goto Ret;
    }
    // known answer test with DES - CBC
    if (!TestSymmetricAlgorithm(CALG_DES, rgbDESKey, sizeof(rgbDESKey),
                                rgbDESKnownPlaintext,
                                sizeof(rgbDESKnownPlaintext),
                                rgbDESCBCCiphertext,
                                rgbIV))
    {
        goto Ret;
    }
#endif // CSP_USE_DES

#ifdef CSP_USE_3DES
    // known answer test with 3DES - ECB
    if (!TestSymmetricAlgorithm(CALG_3DES, rgb3DESKey, sizeof(rgb3DESKey),
                                rgb3DESKnownPlaintext,
                                sizeof(rgb3DESKnownPlaintext),
                                rgb3DESKnownCiphertext,
                                NULL))
    {
        goto Ret;
    }
    // known answer test with 3DES - CBC
    if (!TestSymmetricAlgorithm(CALG_3DES, rgb3DESKey, sizeof(rgb3DESKey),
                                rgb3DESKnownPlaintext,
                                sizeof(rgb3DESKnownPlaintext),
                                rgb3DESCBCCiphertext,
                                rgbIV))
    {
        goto Ret;
    }

    // known answer test with 3DES 112 - ECB
    if (!TestSymmetricAlgorithm(CALG_3DES_112, rgb3DES112Key,
                                sizeof(rgb3DES112Key),
                                rgb3DES112KnownPlaintext,
                                sizeof(rgb3DES112KnownPlaintext),
                                rgb3DES112KnownCiphertext,
                                NULL))
    {
        goto Ret;
    }
    if (!TestSymmetricAlgorithm(CALG_3DES_112, rgb3DES112Key,
                                sizeof(rgb3DES112Key),
                                rgb3DES112KnownPlaintext,
                                sizeof(rgb3DES112KnownPlaintext),
                                rgb3DES112CBCCiphertext,
                                rgbIV))
    {
        goto Ret;
    }
#endif // CSP_USE_3DES

    fRet = TRUE;
Ret:
    if (FALSE == fRet)
        SetLastError((DWORD)NTE_FAIL);
    return fRet;
}


BOOLEAN
DllInitialize(
    IN PVOID hmod,
    IN ULONG Reason,
    IN PCONTEXT Context
    )
{
#ifndef _XBOX
    hInstance = (HINSTANCE) hmod;

    if ( Reason == DLL_PROCESS_ATTACH )
    {
        DisableThreadLibraryCalls(hmod);

        // load strings from csprc.dll
        if (!LoadStrings())
            return FALSE;

        // do a start up check on all supported algorithms to make sure they
        // are working correctly
        if (!SelfMACCheck(szImagePath))
            return FALSE;

        // load library : DON'T FREE IT because it reloads leak
        LoadLibrary("ole32.dll");

        // do a start up check on all supported algorithms to make sure they
        // are working correctly
        if (!AlgorithmCheck())
            return FALSE;
    }
    else if ( Reason == DLL_PROCESS_DETACH )
    {
        // free the strings loaded from csprc.dll
        UnloadStrings();
    }
#endif
    return( TRUE );

}

CHAR sztype[] = "SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider Types\\Type 001";


DWORD
NT5RegisterServer(void)
{
    DWORD   dwErr = S_OK;

	//
	// install the default RSA provider
	// Type 001
	//
    if (!SetCSPInfo(
#ifdef STRONG
                    MS_ENHANCED_PROV,
#else
                    MS_DEF_PROV,
#endif // STRONG
                    szImagePath,
                    NULL,
                    0,
                    PROV_RSA_FULL,
                    RSAFULL_TYPE_STRING,
                    MS_RSA_TYPE,
                    TRUE,
#ifdef STRONG
                    FALSE
#else
                    FALSE
#endif // STRONG
                    ))
    {
        dwErr = E_UNEXPECTED;
        goto Ret;
    }

#ifdef STRONG
	//
	// install the RSA domestic provider (diff from enhanced because set as default)
	// Type 001
	//
    if (!SetCSPInfo(
                    MS_STRONG_PROV,
                    szImagePath,
                    NULL,
                    0,
                    PROV_RSA_FULL,
                    RSAFULL_TYPE_STRING,
                    MS_RSA_TYPE,
                    TRUE,
                    TRUE
                    ))
    {
        dwErr = E_UNEXPECTED;
        goto Ret;
    }
#endif // STRONG

	//
	// install the default RSA SChannel provider
	// Type 012
	//
    if (!SetCSPInfo(
                    MS_DEF_RSA_SCHANNEL_PROV,
                    szImagePath,
                    NULL,
                    0,
                    PROV_RSA_SCHANNEL,
                    RSA_SCH_TYPE_STRING,
                    MS_RSA_SCH_TYPE,
                    TRUE,
                    TRUE))
    {
        dwErr = E_UNEXPECTED;
        goto Ret;
    }
Ret:
    return dwErr;
}

DWORD
DllRegisterServer(void)
{
    DWORD dwErr = 0;

//    if (0 != (dwErr = SetMachineGUID()))
//        return dwErr;

    return NT5RegisterServer();
}

STDAPI DllUnregisterServer(void)
{
    return (S_OK);
}
