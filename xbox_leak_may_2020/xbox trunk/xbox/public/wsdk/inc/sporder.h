/*++

Copyright (c) 1996-1999  Microsoft Corporation

Module Name:

    sporder.h

Abstract:

    This header prototypes the 32-Bit Windows functions that are used
    to change the order or WinSock2 transport service providers and
    name space providers.

Revision History:

--*/

#ifndef __SPORDER_H__
#define __SPORDER_H__

#if _MSC_VER > 1000
#pragma once
#endif


#ifdef __cplusplus
extern "C" {
#endif

int
WSPAPI
WSCWriteProviderOrder (
    IN LPDWORD lpwdCatalogEntryId,
    IN DWORD dwNumberOfEntries
    );

typedef
int
(WSPAPI * LPWSCWRITEPROVIDERORDER)(
    IN LPDWORD lpwdCatalogEntryId,
    IN DWORD dwNumberOfEntries
    );


int
WSPAPI
WSCWriteNameSpaceOrder (
    IN LPGUID lpProviderId,
    IN DWORD dwNumberOfEntries
    );

typedef 
int
(WSPAPI * LPWSCWRITENAMESPACEORDER)(
    IN LPGUID lpProviderId,
    IN DWORD dwNumberOfEntries
    );


#ifdef __cplusplus
}
#endif

#endif      // __SPORDER_H__
