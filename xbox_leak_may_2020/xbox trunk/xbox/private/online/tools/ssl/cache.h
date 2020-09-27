/*-----------------------------------------------------------------------------
* Copyright (C) Microsoft Corporation, 1995 - 1996.
* All rights reserved.
*
* This file is part of the Microsoft Private Communication Technology
* reference implementation, version 1.0
*
* The Private Communication Technology reference implementation, version 1.0
* ("PCTRef"), is being provided by Microsoft to encourage the development and
* enhancement of an open standard for secure general-purpose business and
* personal communications on open networks.  Microsoft is distributing PCTRef
* at no charge irrespective of whether you use PCTRef for non-commercial or
* commercial use.
*
* Microsoft expressly disclaims any warranty for PCTRef and all derivatives of
* it.  PCTRef and any related documentation is provided "as is" without
* warranty of any kind, either express or implied, including, without
* limitation, the implied warranties or merchantability, fitness for a
* particular purpose, or noninfringement.  Microsoft shall have no obligation
* to provide maintenance, support, upgrades or new releases to you or to anyone
* receiving from you PCTRef or your modifications.  The entire risk arising out
* of use or performance of PCTRef remains with you.
*
* Please see the file LICENSE.txt,
* or http://pct.microsoft.com/pct/pctlicen.txt
* for more information on licensing.
*
* Please see http://pct.microsoft.com/pct/pct.htm for The Private
* Communication Technology Specification version 1.0 ("PCT Specification")
*
* 1/23/96
*----------------------------------------------------------------------------*/



typedef struct _SessCacheItem {
    LIST_ENTRY      _ListEntry;
    LONG            cRef;                 // cache item reference count.
    BOOL            ZombieJuju;           // FALSE, item is owned by the cache
                                          // TRUE, item is not owned by cache.

    DWORD           fProtocol;
    DWORD	        Time;

    HMAPPER        *phMapper;
    BOOL            fSGC;                 // enable SGC ciphers

    // Session ID for this session
    DWORD	        cbSessionID;
    UCHAR	        SessionID[SP_MAX_SESSION_ID];


    // Data used to generate keys/
    DWORD           cbMasterKey;
    UCHAR	        pMasterKey[SP_MAX_MASTER_KEY];

    // Clear key saved for PCT
    DWORD           cbClearKey;
    UCHAR           pClearKey[SP_MAX_MASTER_KEY];

    // cache ID (usually machine name or addr)
    UCHAR	        szCacheID[SP_MAX_CACHE_ID];

    CipherSpec	    SessCiphSpec;   // Ciphers used
    HashSpec	    SessHashSpec;
    ExchSpec	    SessExchSpec;


    // References to certificate objects.

    PPctCertificate pServerCert;
    PPctCertificate pClientCert;

    // Server Side Client Auth related items
    /* HLOCATOR */
    HLOCATOR            hLocator;

#ifdef DEBUG
    DWORD DebugId;
#endif // DEBUG;
} SessCacheItem, *PSessCacheItem;

extern DWORD g_dwCacheMaxSize;
extern DWORD g_dwExpireTime;



/* SPInitSessionCache() */
SP_STATUS SPInitSessionCache(VOID);

// Shutdown cache.
SP_STATUS SPShutdownSessionCache(VOID);

// Reference and dereference cache items
LONG SPCacheReference(SessCacheItem *pItem);

LONG SPCacheDereference(SessCacheItem *pItem);



/* Retrieve item from cache by SessionID.
 * Auto-Reference the item if successful */
BOOL SPCacheRetrieveBySession(
                         DWORD fProtocol,
                         PUCHAR SessionID,
                         DWORD cbSessionID,
                         PSessCacheItem *ppRetItem
                        );

/* Retrieve item from cache by ID.
 * Auto-Reference the item if successful */
BOOL SPCacheRetrieveByName(
                         DWORD Protocol,
                         PCHAR szName,
                         PSessCacheItem *ppRetItem
                        );

/* find an empty cache item for use by a context */
BOOL
SPCacheRetrieveNew(
                   PUCHAR pszTarget,
                   PSessCacheItem *ppRetItem
                  );

/* Locks a recently retrieved item into the cache */
BOOL SPCacheAdd(
                   SessCacheItem *pItem
                );

BOOL
SPCacheClone(PSessCacheItem *ppItem);

#define DEF_EXPIRE_TIME     100 * 1000


