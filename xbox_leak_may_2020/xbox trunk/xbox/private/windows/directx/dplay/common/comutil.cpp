/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       comutil.cpp
 *  Content:    Contains implementation of COM helper functions for DPLAY8 project.
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *   06/07/00	rmt		Created
 *   06/15/2000 rmt     Fixed small bug in COM_CoCreateInstance which was causing AV
 *   06/27/00	rmt		Added abstraction for COM_Co(Un)Initialize
 *   07/06/00	rmt		Modified to match updated creg usage
 *   08/08/2000	rmt		Bug #41736 - AV in call to lstrcpy by COM_GetDllName
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "commoni.h"

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_COMMON

typedef HRESULT (*PFNDLLGETCLASSOBJECT)(REFCLSID rclsid,REFIID riid,LPVOID *ppvObj );
typedef HRESULT (*PFNDLLCANUNLOADNOW)(void);

extern HRESULT SPDllGetClassObject( REFCLSID rclsid, REFIID riid, LPVOID *ppvObj );
extern HRESULT SPDllCanUnloadNow( void );

CBilink g_blComEntriesGlobal;
CRITICAL_SECTION csComEntriesLock;

typedef struct _COMDLL_ENTRY
{
    GUID                    clsid;
    PFNDLLGETCLASSOBJECT    pfnGetClassObject;
    PFNDLLCANUNLOADNOW      pfnCanUnloadNow;
    CBilink                 blComEntries;
} COMDLL_ENTRY, *PCOMDLL_ENTRY;

#undef DPF_MODNAME
#define DPF_MODNAME "COM_Init"
HRESULT COM_Init()
{
    g_blComEntriesGlobal.Initialize();
    InitializeCriticalSection( &csComEntriesLock );
    return DPN_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "COM_Free"
HRESULT COM_Free()
{
    CBilink *pblSearch;
    PCOMDLL_ENTRY pEntry;

    pblSearch = g_blComEntriesGlobal.GetNext();

    while( pblSearch != &g_blComEntriesGlobal )
    {
        pEntry = CONTAINING_RECORD( pblSearch, COMDLL_ENTRY, blComEntries );
        pblSearch = pblSearch->GetNext();

        delete pEntry;
    }

    DeleteCriticalSection( &csComEntriesLock );
    return DPN_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "COM_CoInitialize"
HRESULT COM_CoInitialize( void * pvParam )
{
	return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "COM_CoUninitialize"
void COM_CoUninitialize()
{
	return;
}

#undef DPF_MODNAME
#define DPF_MODNAME "COM_GetEntry"
HRESULT COM_GetEntry( GUID clsid, PCOMDLL_ENTRY *ppEntry )
{
    CBilink *pblSearch;
    PCOMDLL_ENTRY pEntry;
    HRESULT hr = E_FAIL;

    EnterCriticalSection( &csComEntriesLock );

    pblSearch = g_blComEntriesGlobal.GetNext();

    while( pblSearch != &g_blComEntriesGlobal )
    {
        pEntry = CONTAINING_RECORD( pblSearch, COMDLL_ENTRY, blComEntries );

		// This should never happen, but makes prefix happy
		if( !pEntry )
		{
			DNASSERT( FALSE );
			return DPNERR_GENERIC;
		}

        if( pEntry->clsid == clsid )
        {
            *ppEntry = pEntry;
            LeaveCriticalSection( &csComEntriesLock );
            return DPN_OK;
        }

        pblSearch = pblSearch->GetNext();
    }

    pEntry = new COMDLL_ENTRY;
    memset( pEntry, 0x00, sizeof( COMDLL_ENTRY ) );

    pEntry->clsid = clsid;
	pEntry->blComEntries.Initialize();

    pEntry->pfnGetClassObject = SPDllGetClassObject;
    pEntry->pfnCanUnloadNow = SPDllCanUnloadNow;

	pEntry->blComEntries.InsertBefore( &g_blComEntriesGlobal );

    LeaveCriticalSection( &csComEntriesLock );

    *ppEntry = pEntry;

    return DPN_OK;
}


// DP_CoCreateInstance
//
// This CoCreateInstance can be used instead of CoCreateInstance and will manually perform the
// steps neccessary to do a CoCreateInstance if COM has not been initialized.
//
#undef DPF_MODNAME
#define DPF_MODNAME "COM_CoCreateInstance"
STDAPI COM_CoCreateInstance( REFCLSID rclsid, LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppv )
{
    HRESULT hr;
    PCOMDLL_ENTRY pEntry;
    IClassFactory *pClassFactory;

        hr = COM_GetEntry( rclsid, &pEntry );

        if( FAILED( hr ) )
            return hr;

        hr = (*pEntry->pfnGetClassObject)( rclsid, GUID_NULL, (void **) &pClassFactory );

        if( FAILED( hr ) )
        {
            DPF( 0, "Failed getting class object on dynamic entry hr=0x%x", hr );
            return hr;
        }


        hr = pClassFactory->lpVtbl->CreateInstance( pClassFactory, pUnkOuter, riid, ppv );

        if( FAILED( hr ) )
        {
            DPF( 0, "Class factory returned an error hr=0x%x", hr );
        }

        pClassFactory->lpVtbl->Release(pClassFactory);

        return hr;
}

