#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include "xkdctest.h"


HRESULT MakeAccountWithPassword(
    IN XONLINEP_USER *pUser, 
    IN ULONGLONG id, 
    IN char *szName, 
    IN char *szKingdom, 
    IN char *szDomain,
    IN char *szRealm,
    IN char *szPassword);

HRESULT MakeAccountWithKey(
    IN XONLINEP_USER *pUser, 
    IN ULONGLONG id, 
    IN char *szName, 
    IN char *szKingdom, 
    IN char *szDomain,
    IN char *szRealm,
    IN BYTE *bKey);

HRESULT MakeAccountWithHexKey(
    IN XONLINEP_USER *pUser, 
    IN ULONGLONG id, 
    IN char *szName, 
    IN char *szKingdom, 
    IN char *szDomain,
    IN char *szRealm,
    IN char *szKey);

HRESULT MakeAccountFromString(
    char *szAccount, 
    XONLINEP_USER *Account);

HRESULT HashAndPrintKeyHex(
    IN char *szKey);

#endif
