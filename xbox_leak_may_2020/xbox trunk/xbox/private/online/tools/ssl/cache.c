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

//
// SHolden - I have changed the caching for WinCE. Before an array was created
//           (with a default of 100) cache entries. This is totally huge
//           for WinCE. Now I will have a linked list with a persistent
//           maximum of g_dwCacheSize. However, the list size will be able
//           to grow beyond the max and then shrink back later.
//

#include "spbase.h"
#include <linklist.h>
#include <limits.h>

#ifdef DEBUG
    DWORD g_cCacheEntryDebugCount = 0;
#endif // DEBUG

typedef struct _tagSERVER_CACHE_ENTRY
{
    LIST_ENTRY    _ListEntry;
    SessCacheItem _CacheItem;
} SERVER_CACHE_ENTRY, *LPSERVER_CACHE_ENTRY;

#ifdef PCT_CI_EMPTY
# undef PCT_CI_EMPTY
# undef PCT_CI_FULL
# undef CACHE_EXPIRE_TICKS
#endif

#define PCT_CI_EMPTY	0
#define PCT_CI_FULL	1


LIST_ENTRY ServerCacheList;

DWORD g_dwCacheSize;    // = 0;
DWORD g_dwCacheMaxSize  = SP_DEF_SERVER_CACHE_SIZE;
DWORD g_cUsedCacheItems; // = 0;
DWORD g_dwExpireTime    = DEF_EXPIRE_TIME;

CRITICAL_SECTION CacheCritSec;
#define SPAcquireCache(pcritsec) EnterCriticalSection(pcritsec)
#define SPReleaseCache(pcritsec) LeaveCriticalSection(pcritsec)

static BOOL
SPCacheDelete(
    SessCacheItem *pItem
    );

// Implementation.

SP_STATUS
SPInitSessionCache(VOID)
{
    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("+SPInitSessionCache()\r\n"))
             );

    InitializeCriticalSection(&CacheCritSec);
    InitializeListHead(&ServerCacheList);

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("-SPInitSessionCache [PCT_ERR_OK]\r\n"))
             );

    return (PCT_ERR_OK);
}

SP_STATUS
SPShutdownSessionCache(VOID)
{
    PLIST_ENTRY pNextItem;
    PSessCacheItem pCacheItem;

    SPAcquireCache(&CacheCritSec);

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("+SPShutdownSessionCache\r\n"))
             );

    for (pNextItem = ServerCacheList.Flink;
         pNextItem != &ServerCacheList;
         )
    {
        pCacheItem = (PSessCacheItem)pNextItem;
        pNextItem  = pNextItem->Flink;

        // Blindly kill.
        pCacheItem->cRef = 1;
        SPCacheDereference(pCacheItem);
        RemoveEntryList((PLIST_ENTRY)pCacheItem);
        SPExternalFree(pCacheItem);
        g_dwCacheSize--;
    }

    ASSERT(g_dwCacheSize == 0);

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("-SPShutdownSessionCache [PCT_ERR_OK]\r\n"))
             );

    SPReleaseCache(&CacheCritSec);
    DeleteCriticalSection(&CacheCritSec);

    return (PCT_ERR_OK);
}

LONG
SPCacheReference(SessCacheItem *pItem)
{
    long cRet;

    if(pItem == NULL)
    {
        return -1;
    }

    SPAcquireCache(&CacheCritSec);

    if (1 == (cRet = InterlockedIncrement(&pItem->cRef)))
    {
        g_cUsedCacheItems++;
    }

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("SPCacheReference %d: %d->%d\r\n"),
              pItem->DebugId,
              cRet -1, cRet)
             );

    SPReleaseCache(&CacheCritSec);
    return (cRet);
}

LONG
SPCacheDereference(SessCacheItem *pItem)
{
    long cRet;

    if(pItem == NULL)
    {
        return -1;
    }

    SPAcquireCache(&CacheCritSec);

    if(0 == (cRet = InterlockedDecrement(&pItem->cRef)))
    {
        SPCacheDelete(pItem);
        g_cUsedCacheItems--;
    }

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("SPCacheDereference %d: %d->%d\r\n"),
              pItem->DebugId,
              cRet + 1,
              cRet)
             );

    SPReleaseCache(&CacheCritSec);
    return cRet;
}

BOOL
SPCacheDelete(
    SessCacheItem *pItem
    )
{
    if (pItem == NULL)
    {
        return (FALSE);
    }

    // Mark this item as non-retrievable.
    pItem->ZombieJuju = FALSE;

    // Delete all items pointed to by the cache.
    if (pItem->pServerCert)
    {
        DereferenceCert(pItem->pServerCert);
        pItem->pServerCert = NULL;
    }

    if (pItem->pClientCert)
    {
        DereferenceCert(pItem->pClientCert);
        pItem->pClientCert = NULL;
    }

#ifdef FOO
    if(pItem->pMapper)
    {
        if(pItem->hLocator)
        {
            pItem->pMapper->CloseLocator(pItem->hLocator);
            pItem->hLocator = NULL;
        }
    }
#endif
    /* close the HLOCATOR here */

    return (TRUE);
}

BOOL SPCacheRetrieveBySession(
                         DWORD fProtocol,
                         PUCHAR SessionID,
                         DWORD cbSessionID,
                         PSessCacheItem *ppRetItem
                        )
{
    BOOL fFound = FALSE;
    PLIST_ENTRY pNextItem;
    PSessCacheItem pCacheItem;

    SPAcquireCache(&CacheCritSec);

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("+SPCacheRetrieveBySession(0x%.8X, 0x%.8X, 0x%.8X, 0x%.8X)\r\n"),
              fProtocol,
              SessionID,
              cbSessionID,
              ppRetItem)
             );

    if(ppRetItem == NULL)
    {
        DEBUGMSG(ZONE_SERVERCACHE,
                 (TEXT("-SPCacheRetrieveBySession [FALSE]\r\n"))
                 );

        SPReleaseCache(&CacheCritSec);
        return (FALSE);
    }

    if(cbSessionID)
    {
        // NOTE:  Any items that are owned by the cache will have at least
        // on reference count for that.  They are therefore not going to be
        // deleted from the cache during this process because the
        // cache access is crit-sectioned.

        for (pNextItem = ServerCacheList.Flink;
             pNextItem != &ServerCacheList;
             pNextItem = pNextItem->Flink
             )
        {
            pCacheItem = (PSessCacheItem)pNextItem;

            // Is the ref count non zero.
            // The cache should always have a reference
            // to the item if it is a valid item, nobody
            // but the caching code can inc the ref count
            // from zero to 1, and nobody but the cache
            // code can dec from 1 to 0.  Therefore it's safe
            // to say that if the ref count is zero, the
            // item doesn't exist, and it won't exist.
            // If the item is greater than 1, then the referenceres
            // of the item can dereference it during the cache search,
            // but since the cache holds a reference to the item as
            // well, it won't be deleted out from under us.

            if(pCacheItem->cRef == 0)
            {
                continue;
            }

            // Is this item retrievable
            if (!pCacheItem->ZombieJuju)
            {
                continue;
            }

             // Has this item expired?
            if(pCacheItem->Time < GetTickCount())
            {
                continue;
            }
           // Is this item for the protocol we're using.
            if(0 == (fProtocol & pCacheItem->fProtocol))
            {
                continue;
            }

            if(cbSessionID != pCacheItem->cbSessionID)
            {
                continue;
            }

            if ( memcmp(SessionID, pCacheItem->SessionID, cbSessionID) == 0)
            {

                // Found item in cache!!

                pCacheItem->Time = GetTickCount() + g_dwExpireTime;
                // Are we replacing something?
                // Then dereference the thing we are replacing.
                if(*ppRetItem)
                {
                    SPCacheDereference(*ppRetItem);
                }

                // Return item referenced.
                *ppRetItem = pCacheItem;
                SPCacheReference(pCacheItem);
                fFound = TRUE;
                break;
            }
        }
    }

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("-SPCacheRetrieveBySession [%s]\r\n"),
              (fFound == TRUE) ? TEXT("TRUE") : TEXT("FALSE"))
             );

    SPReleaseCache(&CacheCritSec);

    return (fFound);
}


BOOL SPCacheRetrieveByName(
                         DWORD fProtocol,
                         PCHAR szName,
                         PSessCacheItem *ppRetItem
                        )
{
    BOOL fFound = FALSE;
    PLIST_ENTRY pNextItem;
    PSessCacheItem pCacheItem;

    SPAcquireCache(&CacheCritSec);

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("+SPCacheRetrieveByName(0x%.8X, %a, 0x%.8X[%d])\r\n"),
              fProtocol,
              szName,
              ppRetItem,
              ppRetItem ? (*ppRetItem)->DebugId : -1)
             );

    if(ppRetItem == NULL)
    {
        DEBUGMSG(ZONE_SERVERCACHE,
                 (TEXT("-SPCacheRetrieveByName [FALSE]\r\n"))
                 );

        SPReleaseCache(&CacheCritSec);

        return (FALSE);
    }

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("Looking for [%a] in cache.\r\n"),
              szName ? szName : "NULL")
             );

    for (pNextItem = ServerCacheList.Flink;
         pNextItem != &ServerCacheList;
         pNextItem = pNextItem->Flink
         )
    {
        pCacheItem = (PSessCacheItem)pNextItem;

        // Is the ref count non zero.
        // The cache should always have a reference
        // to the item if it is a valid item, nobody
        // but the caching code can inc the ref count
        // from zero to 1, and nobody but the cache
        // code can dec from 1 to 0.  Therefore it's safe
        // to say that if the ref count is zero, the
        // item doesn't exist, and it won't exist.
        // If the item is greater than 1, then the referenceres
        // of the item can dereference it during the cache search,
        // but since the cache holds a reference to the item as
        // well, it won't be deleted out from under us.

        if(pCacheItem->cRef == 0)
        {
            continue;
        }
        // Is this item retrievable
        if (!pCacheItem->ZombieJuju)
        {
            continue;
        }

        // Has this item expired?
        if(pCacheItem->Time < GetTickCount())
        {
            continue;
        }

        // Is this item for the protocol we're using.
        if(0 == (fProtocol & pCacheItem->fProtocol))
        {
            continue;
        }


        if((pCacheItem->szCacheID[0] == 0 && szName == NULL) ||
           (strcmp(szName, pCacheItem->szCacheID) == 0))
        {
            pCacheItem->Time = GetTickCount() + g_dwExpireTime;
            if(*ppRetItem)
            {
                SPCacheDereference(*ppRetItem);
            }
            *ppRetItem = pCacheItem;
            SPCacheReference(pCacheItem);
            fFound = TRUE;
            break;
        }
    }

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("-SPCacheRetrieveByName [%s]\r\n"),
              (fFound == TRUE)? TEXT("TRUE") : TEXT("FALSE"))
             );

    SPReleaseCache(&CacheCritSec);

    return (fFound);
}


/* allocate a new cache item to be used
 * by a context.  Initialize it with the
 * pszTarget if the target exists.
 * Auto-Generate a SessionID
 */
BOOL
SPCacheRetrieveNew(
                   PUCHAR pszTarget,
                   PSessCacheItem *ppRetItem
                  )
{
    DWORD timeNow, timeOldest;
    PLIST_ENTRY pNextItem;
    PSessCacheItem pCacheItem;
    PSessCacheItem pOldestItem = NULL;
#ifdef DEBUG
    DWORD i = 0;
#endif // DEBUG

    SPAcquireCache(&CacheCritSec);

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("+SPCacheRetrieveNew(%a, 0x%.8X)\r\n"),
              pszTarget,
              ppRetItem)
             );

    timeNow = GetTickCount();
    timeOldest = timeNow + g_dwExpireTime;

    for (pNextItem = ServerCacheList.Flink;
         pNextItem != &ServerCacheList;
         pNextItem = pNextItem->Flink
         )
    {
    #ifdef DEBUG
        i++;
    #endif // DEBUG

        pCacheItem = (PSessCacheItem)pNextItem;

        // Did we find an empty slot in cache.
        if (pCacheItem->cRef == 0)
        {
            break;
        }

        // Check to see if we have any elements that we
        // can expire.
        if(pCacheItem->cRef == 1)
        {
            if(pCacheItem->ZombieJuju == FALSE)
            {
                // Special Case, if this item is only held by the
                // cache, but it hasn't been marked as searchable,
                // then this is an aborted zombie (yeuch).
                // We can use this one.

                DEBUGMSG(ZONE_SERVERCACHE,
                         (TEXT("\tExpiring aborted zombie - Id = %d.\r\n"),
                          pCacheItem->DebugId)
                         );

                SPCacheDereference(pCacheItem);
                break;

            }

            if (pCacheItem->Time < timeNow)
            {
                DEBUGMSG(ZONE_SERVERCACHE,
                         (TEXT("\tExpiring old element.\r\n"))
                         );

                SPCacheDereference(pCacheItem);
                break;
            }

            // keep index and time of oldest unexpired entry

            if (timeOldest > pCacheItem->Time)
            {
                timeOldest  = pCacheItem->Time;
                pOldestItem = pCacheItem;

                DEBUGMSG(ZONE_SERVERCACHE,
                         (TEXT("Oldest element - %d.\r\n"),
                          pOldestItem->DebugId)
                         );
            }
        }
    }

    ASSERT(i <= g_dwCacheSize);

    if (pNextItem == &ServerCacheList)
    {
        ASSERT(i == g_dwCacheSize);

        if ((pOldestItem == NULL) || (g_dwCacheSize < g_dwCacheMaxSize))
        {
            // Need to add a new entry to the list.
            pNextItem = (PLIST_ENTRY)SPExternalAlloc(sizeof(SessCacheItem));

            if (pNextItem == NULL)
            {
                SPReleaseCache(&CacheCritSec);

                DEBUGMSG(ZONE_ERROR,
                         (TEXT("-SPCacheRetrieveByName [FALSE] Out of Memory!\r\n"))
                         );

                return (FALSE);
            }

            ZeroMemory(pNextItem, sizeof(SessCacheItem));
            InsertTailList(&ServerCacheList, (PLIST_ENTRY)pNextItem);
            pCacheItem = (PSessCacheItem)pNextItem;
            g_dwCacheSize++;
        #ifdef DEBUG
            pCacheItem->DebugId = ++g_cCacheEntryDebugCount;
        #endif //DEBUG

            DEBUGMSG(ZONE_SERVERCACHE,
                     (TEXT("Creating new cache element - Id = %d.\r\n"),
                      pCacheItem->DebugId)
                     );

        }
        else
        {
            // We will replace the oldest item.
            pCacheItem = pOldestItem;
            DEBUGMSG(ZONE_SERVERCACHE,
                     (TEXT("\tReplacing oldest element - Id = %d\r\n"),
                      pCacheItem->DebugId)
                     );

            SPCacheDereference(pCacheItem);
        }
    }

    ASSERT(pCacheItem != NULL);
    ASSERT(pCacheItem->cRef == 0);

    // pCacheItem is our new element, so
    // "Put it in the cache"
    SPCacheReference(pCacheItem);

    if(pszTarget)
    {
        CopyMemory(pCacheItem->szCacheID,
                   pszTarget,
                   min(strlen(pszTarget)+1, (SP_MAX_CACHE_ID-1))
                  );
    }
    else
    {
        pCacheItem->szCacheID[0] = '\0';
    }

    // Pre seed the session ID with random bits
    GenerateRandomBits(pCacheItem->SessionID, SP_MAX_SESSION_ID);

    // Size of session ID is zero
    pCacheItem->cbSessionID = 0;
    pCacheItem->Time = timeNow + g_dwExpireTime;
    pCacheItem->ZombieJuju = FALSE;
    pCacheItem->fProtocol = g_ProtEnabled;
    pCacheItem->phMapper = NULL;
    pCacheItem->cbClearKey = 0;
    pCacheItem->cbMasterKey = 0;
    pCacheItem->pServerCert = NULL;
    pCacheItem->pClientCert = NULL;
    pCacheItem->hLocator = 0;
    pCacheItem->fSGC = 0;

    // We've set up this element, so reference it for the caller.

    SPCacheReference(pCacheItem);

    *ppRetItem = pCacheItem;

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("%d/%d [Cache items/Cache max peristent size]. ")
              TEXT("%d cache items used.\r\n"),
              g_dwCacheSize,
              g_dwCacheMaxSize,
              g_cUsedCacheItems)
             );

    // If the cache is larger that we want to allow, go through and delete
    // expired zombies and old elements. Also, we can delete unreferenced
    // items, although I am not sure that we really get any of these.
    if (g_dwCacheMaxSize < g_dwCacheSize)
    {
        for (pNextItem = ServerCacheList.Flink;
             pNextItem != &ServerCacheList;)
        {
            pCacheItem = (PSessCacheItem)pNextItem;

            // Need to increment now in case we delete a node in the list.
            pNextItem = pNextItem->Flink;

            if (pCacheItem->cRef == 0)
            {
                DEBUGMSG(ZONE_SERVERCACHE,
                         (TEXT("\tDeleting unreferenced cache item - %d.\r\n"),
                          pCacheItem->DebugId)
                         );
                RemoveEntryList((PLIST_ENTRY)pCacheItem);
                g_dwCacheSize--;
            }
            else if (pCacheItem->cRef == 1)
            {
                if ((pCacheItem->ZombieJuju == FALSE) ||
                    (pCacheItem->Time < timeNow )
                    )
                {
                    DEBUGMSG(ZONE_SERVERCACHE,
                             (TEXT("\tDeleting %s cache item - %d.\r\n"),
                              (pCacheItem->ZombieJuju == FALSE) ?
                                    TEXT("expired zombie") : TEXT("old"),
                              pCacheItem->DebugId)
                             );

                    SPCacheDereference(pCacheItem);
                    RemoveEntryList((PLIST_ENTRY)pCacheItem);
                    SPExternalFree(pCacheItem);
                    g_dwCacheSize--;
                }
            }

            if (g_dwCacheMaxSize >= g_dwCacheSize)
            {
                break;
            }
        }
    }

    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("-SPCacheRetrieveNew [TRUE]\r\n"))
             );

    SPReleaseCache(&CacheCritSec);

    return (TRUE);
}

BOOL SPCacheAdd(
                   SessCacheItem *pItem
                )
{
    DEBUGMSG(ZONE_SERVERCACHE,
             (TEXT("SPCacheAdd %d->ZombieJuju\r\n"),
              pItem->DebugId)
             );

    if(pItem == NULL)
    {
        return FALSE;
    }

    // Allow cache ownership of this item
    pItem->ZombieJuju = TRUE;

    return TRUE;

}

/* Allocate a new cache item, and copy 
 * over relevant information from old item,
 * and dereference old item.  This is a helper
 * for REDO
 */
BOOL
SPCacheClone(PSessCacheItem *ppItem)
{
    PSessCacheItem pNewItem = NULL;

    if(ppItem == NULL || *ppItem == NULL)
    {
        return FALSE;
    }

    // Get a fresh cache item.
    if(!SPCacheRetrieveNew((*ppItem)->szCacheID, &pNewItem))
    {
        return FALSE;
    }
    
    // Copy over old relevant data
    
     
    pNewItem->fProtocol = (*ppItem)->fProtocol;
    pNewItem->fSGC      = (*ppItem)->fSGC;

    SPCacheDereference(*ppItem);
    *ppItem = pNewItem;
    return TRUE;
}





