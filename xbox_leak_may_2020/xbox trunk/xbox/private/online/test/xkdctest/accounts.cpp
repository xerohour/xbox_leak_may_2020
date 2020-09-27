#include "xkdctest.h"
#include <stdio.h>
#include <stdlib.h>
#include "accounts.h"


ULONGLONG HexStrToQW(IN char *szHex)
{
    ULONGLONG qw = 0;

    while (*szHex)
    {
        BYTE b;
        if (FAILED(HexCharToByte(szHex[0], &b)))
        {
            return 0;
        }
        qw = (qw << 4) + b;

        szHex++;
    }

    return qw;
}


HRESULT KeyStrToKey(
    IN char *szKey, 
    IN INT nKeyLength, 
    OUT BYTE *key)
{
    INT cStrKeyLen = strlen(szKey);
    if (cStrKeyLen != nKeyLength * 2)
    {
        return E_FAIL;
    }

    for (INT i = 0; i < XONLINE_KEY_LENGTH; i++)
    {
        if (HexPairToByte(szKey, &key[i]) != S_OK)
        {
            return E_FAIL;
        }
        szKey += 2;
    }

    return S_OK;
}


HRESULT MakeAccountWithPassword(
    IN XONLINEP_USER *pUser, 
    IN ULONGLONG id, 
    IN char *szName, 
    IN char *szKingdom, 
    IN char *szDomain,
    IN char *szRealm,
    IN char *szPassword)
{
    BYTE bKey[XONLINE_KEY_LENGTH];

    if (KerbPasswordToKey(szPassword, bKey))
    {
        return MakeAccountWithKey(pUser, id, szName, szKingdom, szDomain, szRealm, bKey);
    }
    else
    {
        return E_FAIL;
    }
}


HRESULT MakeAccountWithKey(
    IN XONLINEP_USER *pUser, 
    IN ULONGLONG id, 
    IN char *szName, 
    IN char *szKingdom, 
    IN char *szDomain,
    IN char *szRealm,
    IN BYTE *bKey)
{
    memset(pUser, 0, sizeof(XONLINEP_USER));

    pUser->xuid.qwUserID = id;
    pUser->xuid.dwUserFlags = 0;
    strncpy(pUser->name, szName, XONLINE_NAME_SIZE);
    strncpy(pUser->kingdom, szKingdom, XONLINE_KINGDOM_SIZE);
    strncpy(pUser->domain, szDomain, XONLINE_USERDOMAIN_SIZE);
    strncpy(pUser->realm, szRealm, XONLINE_REALM_NAME_SIZE);
    memset(pUser->pin, 0, 4);
    memcpy(pUser->key, bKey, XONLINE_KEY_LENGTH);

    return S_OK;
}

HRESULT MakeAccountWithHexKey(
    IN XONLINEP_USER *pUser, 
    IN ULONGLONG id, 
    IN char *szName, 
    IN char *szKingdom, 
    IN char *szDomain,
    IN char *szRealm,
    IN char *szKey)
{
    HRESULT hr = S_OK;

    BYTE bKey[XONLINE_KEY_LENGTH];

    if (KeyStrToKey(szKey, XONLINE_KEY_LENGTH, bKey) == S_OK)
    {
        return hr = MakeAccountWithKey(pUser, id, szName, szKingdom, szDomain, szRealm, bKey);
    }
    else
    {
        return E_FAIL;
    }
}


// Returns the new position of szList
void GetCommaListItem(char **szList, char *szItem, INT nMax)
{
    while (*szList && **szList == ' ') (*szList)++;
    if (**szList == '"') (*szList)++;

    while (**szList && **szList != ',' && nMax > 0)
    {
        *szItem = **szList;
        szItem++;
        (*szList)++;
        nMax--;
    }

    while (**szList && **szList != ',')
    {
        (*szList)++;
    }

    (*szList)++;

    while (*(szItem - 1) == ' ') szItem--;
    if (*(szItem - 1) == '"') szItem--;

    *szItem = 0;
}


HRESULT MakeAccountFromString(
    char *szAccount, 
    XONLINEP_USER *Account)
{
    HRESULT hr = E_FAIL;
    
    ULONGLONG qwUserID;
    char szPUID[33];
    char szName[XONLINE_NAME_SIZE+1];
    char szKingdom[XONLINE_KINGDOM_SIZE+1];
    char szDomain[XONLINE_USERDOMAIN_SIZE+1];
    char szRealm[XONLINE_REALM_NAME_SIZE+1];
    char szKey[XONLINE_KEY_LENGTH*2+3];

    GetCommaListItem(&szAccount, szPUID, 32);
    qwUserID = HexStrToQW(szPUID);
    GetCommaListItem(&szAccount, szName, XONLINE_NAME_SIZE);
    GetCommaListItem(&szAccount, szKingdom, XONLINE_KINGDOM_SIZE);
    GetCommaListItem(&szAccount, szDomain, XONLINE_USERDOMAIN_SIZE);
    GetCommaListItem(&szAccount, szRealm, XONLINE_REALM_NAME_SIZE);
    GetCommaListItem(&szAccount, szKey, XONLINE_KEY_LENGTH*2+2);

    if (szKey[0] == '0' && (szKey[1] == 'x' || szKey[1] == 'X'))
    {
        return MakeAccountWithHexKey(Account, qwUserID, szName, szKingdom, szDomain, szRealm, szKey+2);
    }
    else
    {
        return MakeAccountWithPassword(Account, qwUserID, szName, szKingdom, szDomain, szRealm, szKey);
    }
}

HRESULT HashAndPrintKeyHex(
    IN char *szKey)
{
    BYTE bHashedKey[16];

    INT err = KerbPasswordToKey(szKey, bHashedKey);
    
    for (INT i = 0; i < 16; i++ )
    {
        printf("%2.2X", bHashedKey[i]);
    }

    return err == 0 ? S_OK : E_FAIL;
}

