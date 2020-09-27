/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dvshared.cpp
 *  Content:	Utility functions for DirectXVoice structures.
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	07/06/99	rodtoll	Created It
 *  09/01/2000  georgioc started port/re-write for xbox
 ***************************************************************************/
#pragma warning( disable : 4786 )  

#include "dvntos.h"
#include "dvshared.h"

#include "dvserver.h"
#include "dvclient.h"

#include "dvdxtran.h"

// Useful macros for checking record/suppression volumes
#define DV_ValidRecordVolume( x ) DV_ValidPlaybackVolume( x )
#define DV_ValidSuppressionVolume( x ) DV_ValidPlaybackVolume( x )

#if DBG

__inline VOID
DPVF
(
    DWORD                   dwLevel,
    LPCSTR                  pszFormat,
    ...
)
{
    CHAR                    szString[0x400];
    va_list                 va;

    if(g_DVDebugLevel & dwLevel)
    {
        va_start(va, pszFormat);
        vsprintf(szString, pszFormat, va);
        va_end(va);

        DebugPrint("%s:(%x): %s\n",g_ModName,g_This, szString);
                
    }
}

#endif

//
// LINKED list and pool functions
//


PVOID DV_REMOVE_TAIL(PLIST_ENTRY pListHead)
{
    KIRQL irql;
    PDVPOOLALLOC pEntry;

    ASSERT(pListHead->Flink);
    ASSERT(pListHead->Blink);

    RIRQL(irql);

    if (IsListEmpty(pListHead)) {

        LIRQL(irql);
        DPVF( DPVF_ERRORLEVEL, "Attempted Empty List dequeue on list %x!",pListHead );
        return NULL;

    }

    pEntry = (PDVPOOLALLOC)RemoveTailList(pListHead);
    LIRQL(irql);

    ASSERT(pEntry);
    ASSERT(pListHead->Flink);
    ASSERT(pListHead->Blink);

    return  &pEntry->Data;
}
    
PVOID DV_REMOVE_HEAD(PLIST_ENTRY pListHead)
{
    PDVPOOLALLOC pEntry;
    KIRQL irql;

    RIRQL(irql);

    if (IsListEmpty(pListHead)) {

        LIRQL(irql);
        DPVF( DPVF_ERRORLEVEL, "Attempted Empty List dequeue on list %x!",pListHead );        
        return NULL;

    }

    pEntry = (PDVPOOLALLOC) RemoveHeadList(pListHead);
    LIRQL(irql);

    ASSERT(pListHead->Flink);
    ASSERT(pListHead->Blink);

    return  &pEntry->Data;
}


PVOID DV_GET_NEXT(PLIST_ENTRY pListHead, PVOID pData)
{
    PDVPOOLALLOC pEntry;
    KIRQL irql;

    RIRQL(irql);

    pEntry = (PDVPOOLALLOC) ((PUCHAR)pData - FIELD_OFFSET(DVPOOLALLOC,Data));
    pEntry = (PDVPOOLALLOC) pEntry->ListEntry.Flink;

    if ((PLIST_ENTRY)pEntry == pListHead) {
        LIRQL(irql);
        return NULL;
    }

    LIRQL(irql);

    return &pEntry->Data;
}

PVOID DV_GET_LIST_HEAD(PLIST_ENTRY pListHead)
{
    PDVPOOLALLOC pEntry;
    KIRQL irql;

    RIRQL(irql);
    pEntry = (PDVPOOLALLOC)pListHead->Flink;
    LIRQL(irql);

    ASSERT(pListHead->Flink);
    ASSERT(pListHead->Blink);

    return &pEntry->Data;
}


VOID DV_REMOVE_ENTRY(PVOID pData)
{
    PLIST_ENTRY pEntry;
    KIRQL irql;

    RIRQL(irql);
    pEntry = (PLIST_ENTRY) ((PUCHAR)pData - FIELD_OFFSET(DVPOOLALLOC,Data));
    RemoveEntryList(pEntry);
    LIRQL(irql);
}


VOID DV_INSERT_TAIL(PLIST_ENTRY pListHead, PVOID data)
{

    PLIST_ENTRY pEntry;
    KIRQL irql;

    RIRQL(irql);
    pEntry = (PLIST_ENTRY) ((PUCHAR)data - FIELD_OFFSET(DVPOOLALLOC,Data));
    InsertTailList(pListHead,pEntry);
    LIRQL(irql);

    ASSERT(pListHead->Flink);
    ASSERT(pListHead->Blink);

}
 
VOID DV_INSERT_HEAD(PLIST_ENTRY pListHead, PVOID data)
{

    PLIST_ENTRY pEntry;
    KIRQL irql;

    RIRQL(irql);
    pEntry = (PLIST_ENTRY) ((PUCHAR)data - FIELD_OFFSET(DVPOOLALLOC,Data));
    InsertHeadList(pListHead,pEntry);
    LIRQL(irql);

    ASSERT(pListHead->Flink);
    ASSERT(pListHead->Blink);

}



HRESULT DV_InitializeList(PLIST_ENTRY pListHead, DWORD dwNumElements,DWORD dwSize, DWORD dwType)
{
    ULONG i;
    
    PUCHAR buffer;
    PDVPOOLALLOC pAlloc;


    InitializeListHead(pListHead);

    if (dwType != DVLAT_CVOICEPLAYER) {

        ASSERT(dwSize >= sizeof(DVPOOLALLOC));

        buffer = (PUCHAR)DV_POOL_ALLOC((dwSize+sizeof(DVPOOLALLOC))*dwNumElements);

        ASSERT(buffer);

        if (!buffer) {
            return E_OUTOFMEMORY;
        }
    

    }

    for (i=0;i<dwNumElements;i++) {

        switch (dwType) {
        case DVLAT_RAW:

            pAlloc = (PDVPOOLALLOC) ((PUCHAR)buffer + i*(dwSize+sizeof(DVPOOLALLOC)));
            InsertTailList(pListHead,&pAlloc->ListEntry);

            break;

        case DVLAT_CVOICEPLAYER:

            CVoicePlayer *pPlayer = new CVoicePlayer();
            ASSERT(pPlayer);
            InsertTailList(pListHead,&pPlayer->m_PoolEntry);
            break;

        }
        
    }

    return NOERROR;

}


VOID DV_FreeList(PLIST_ENTRY pListHead, DWORD dwType)
{

    PDVPOOLALLOC pAlloc,pNextAlloc;
    KIRQL irql;

    RIRQL(irql);

    if (IsListEmpty(pListHead)) {

        LIRQL(irql);
        return;

    }

    pAlloc = (PDVPOOLALLOC) pListHead->Flink;

    switch (dwType) {
    case DVLAT_RAW:

        //
        // raw allocations use a single buffer pointed to by the head.
        // the caller is supposed to keep track of it and free it
        //

        InitializeListHead(pListHead);

        break;

    case DVLAT_CVOICEPLAYER:

        while ((PLIST_ENTRY)pAlloc != pListHead) {
    
            CVoicePlayer *pPlayer = CONTAINING_RECORD(pAlloc, CVoicePlayer,m_PoolEntry);
            pNextAlloc = (PDVPOOLALLOC) pAlloc->ListEntry.Flink;
            delete pPlayer;
            pAlloc = pNextAlloc;
    
        }

        break;

    }

    LIRQL(irql);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



#undef DPF_MODNAME
#define DPF_MODNAME "DV_Initialize"
//
// DV_Initialize
//
// Responsible for initializing the specified directvoice object with the specified parameters.
//
STDAPI DV_Initialize( LPDIRECTVOICEOBJECT lpdvObject, PVOID lpTransport, LPDVMESSAGEHANDLER lpMessageHandler, LPVOID lpUserContext, LPDWORD lpdwMessages, DWORD dwNumElements )
{
	HRESULT hr = S_OK;

	LPDIRECTPLAYVOICETRANSPORT lpdvDplayTransport=NULL;
	CDirectVoiceDirectXTransport *lpdxTransport;

	if( lpTransport == NULL )
	{
		DPVF( DPVF_ERRORLEVEL, "Bad pointer" );
		return DVERR_NOTRANSPORT;
	}

	// Fix a memory leak if you Connect/Disconnect and then reconnect.  
	if( lpdvObject->lpDVTransport )
	{
		delete lpdvObject->lpDVTransport;
		lpdvObject->lpDVTransport = NULL;
	}

	// Try and retrieve transport interface from the object we got.
	lpdvDplayTransport = (LPDIRECTPLAYVOICETRANSPORT) lpTransport;

	lpdxTransport = new CDirectVoiceDirectXTransport(lpdvDplayTransport);

	if( lpdxTransport == NULL )
	{
		DPVF( DPVF_ERRORLEVEL, "Unable to allocate transport" );
		lpdvDplayTransport->Release();
		return DVERR_OUTOFMEMORY;
	}

	hr = lpdvObject->lpDVEngine->Initialize( static_cast<CDirectVoiceTransport *>(lpdxTransport), lpMessageHandler, lpUserContext, lpdwMessages, dwNumElements  );

	if( FAILED( hr ) )
	{
		delete lpdxTransport;

		return hr;
	}

	lpdvObject->lpDVTransport = static_cast<CDirectVoiceTransport *>(lpdxTransport);

	return S_OK;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DV_CreateConverter"
HRESULT DV_CreateConverter(
    GUID guidCT,
    XMediaObject **ppConverter,
    WAVEFORMATEX *pwfxUnencodedFormat,
    WAVEFORMATEX **ppwfxEncodedFormat,
    DWORD dwUncompressedPacketSize,
    BOOLEAN bCreateEncoder
    )
{

    WORD tag;
    HRESULT hr = NOERROR;

    //
    // get format tag from GUID
    //

    if (guidCT == DSCTID_SC03) {    

        //
        // vox sc03
        //

        tag = WAVE_FORMAT_VOXWARE_SC03;

    } else if (guidCT == DSCTID_SC06){

        //
        // vox sc06
        //

        tag = WAVE_FORMAT_VOXWARE_SC06;

    }  else if (guidCT == DSCTID_VR12){

        //
        // vox vr12
        //

        tag = WAVE_FORMAT_VOXWARE_VR12;

    } else {

        //
        // no compression, return NULL
        //                

        *ppConverter = NULL;

        //
        // not a supported format!
        //

        DPVF( DPVF_ERRORLEVEL, "Request to create compressor for unssuported compressor!!");
        ASSERT(FALSE);
        return DVERR_NOTSUPPORTED;

    }

    PVOICECODECWAVEFORMAT pwfxVox;

    if (ppwfxEncodedFormat) {

        pwfxVox = new VOICECODECWAVEFORMAT; 
        *ppwfxEncodedFormat = (PWAVEFORMATEX) pwfxVox;

    } else {

        pwfxVox = NULL;

    }

    //
    // instantiate the proper vox compressor
    //


    hr = VoxwareCreateConverter(tag,
                                bCreateEncoder,
                                ((pwfxVox == NULL) ? NULL : pwfxVox),
                                pwfxUnencodedFormat,
                                ppConverter);

    return hr;

}




HRESULT
DV_InitMediaPacketPool(
    PLIST_ENTRY pPool,
    DWORD dwBufferSize,
    DWORD   bAlloc
    )
{
    PDVMEDIAPACKET pPacket;

    pPacket = (PDVMEDIAPACKET) DV_GET_LIST_HEAD(pPool);

    while (pPacket) {
    
        if (bAlloc) {
    
            pPacket->xmp.pvBuffer = (PBYTE) DV_POOL_ALLOC(dwBufferSize);
            ASSERT(pPacket->xmp.pvBuffer);
            if (!pPacket->xmp.pvBuffer) {

                return DVERR_OUTOFMEMORY;

            }
            
        }

        pPacket->dwCompletedSize = 0;
        pPacket->dwStatus = XMEDIAPACKET_STATUS_PENDING;
        pPacket->xmp.pdwCompletedSize = &pPacket->dwCompletedSize;
        pPacket->xmp.pdwStatus = &pPacket->dwStatus;
        pPacket->xmp.dwMaxSize = dwBufferSize;
        pPacket->xmp.pContext = pPacket;
        pPacket->xmp.prtTimestamp = NULL;

        pPacket = (PDVMEDIAPACKET) DV_GET_NEXT(pPool,pPacket);

    }

    return DV_OK;

}


VOID
DV_FreeMediaPacketPool(
    LIST_ENTRY *pPool,
    DWORD   bFree
    )
{
    PDVMEDIAPACKET pPacket;
    KIRQL irql;

    RIRQL(irql);

    if (IsListEmpty(pPool)) {
        LIRQL(irql);
        return;
    }

    pPacket = (PDVMEDIAPACKET) DV_GET_LIST_HEAD(pPool);

    while (pPacket) {
    
        if (bFree) {
    
            DV_POOL_FREE(pPacket->xmp.pvBuffer);
            
        }

        pPacket->dwCompletedSize = 0;
        pPacket->dwStatus = XMEDIAPACKET_STATUS_FLUSHED;

        pPacket = (PDVMEDIAPACKET) DV_GET_NEXT(pPool, pPacket);

    }

    LIRQL(irql);

}





/******************************************************************************
/ VALIDATION ROUTINES
/*****************************************************************************/

#undef DPF_MODNAME
#define DPF_MODNAME "DV_ValidTargetList"
HRESULT DV_ValidTargetList( PDVID pdvidTargets, DWORD dwNumTargets )
{
	if( (pdvidTargets != NULL && dwNumTargets == 0) ||
	    (pdvidTargets == NULL && dwNumTargets > 0 ) )
	{
		DPVF( DPVF_ERRORLEVEL, "Invalid params" );
		return DVERR_INVALIDPARAM;
	}

	if( dwNumTargets == 0 )
		return DV_OK;
	
	if( pdvidTargets == NULL)
	{
		DPVF( DPVF_ERRORLEVEL, "Invalid array of player targets" );
		return DVERR_INVALIDPOINTER;
	}

	// Search for duplicates in the targets
	for( DWORD dwIndex = 0; dwIndex < dwNumTargets; dwIndex++ )
	{
		if( pdvidTargets[dwIndex] == DVID_ALLPLAYERS && dwNumTargets > 1 )
		{
			DPVF( DPVF_ERRORLEVEL, "Cannot specify allplayers (or noplayers) in addition to other ids" );
			return DVERR_INVALIDPARAM;
		}

		for( DWORD dwInnerIndex = dwIndex+1; dwInnerIndex < dwNumTargets; dwInnerIndex++ )
		{
			if( pdvidTargets[dwInnerIndex] == pdvidTargets[dwIndex] )
			{
				DPVF( DPVF_ERRORLEVEL, "Duplicate found in target list" );
				return DVERR_INVALIDPARAM;
			}
		}
	}

	return DV_OK;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DV_ValidClientConfig"
// DV_ValidClientConfig
//
// Checks the valid client configuration structure to ensure it's valid
//
HRESULT DV_ValidClientConfig( LPDVCLIENTCONFIG lpClientConfig )
{
	if( lpClientConfig == NULL)
	{
		DPVF( DPVF_ERRORLEVEL, "Invalid pointer" );
		return E_POINTER;
	}

	if( lpClientConfig->dwSize != sizeof( DVCLIENTCONFIG ) )
	{
		DPVF( DPVF_ERRORLEVEL, "Invalid size" );
		return DVERR_INVALIDPARAM;
	}

	if( lpClientConfig->dwFlags & 
	    ~(DVCLIENTCONFIG_RECORDMUTE | DVCLIENTCONFIG_PLAYBACKMUTE | 
	      DVCLIENTCONFIG_AUTOVOICEACTIVATED | DVCLIENTCONFIG_MUTEGLOBAL |
          DVCLIENTCONFIG_MANUALVOICEACTIVATED) 
	  )
	{
		DPVF( DPVF_ERRORLEVEL, "Invalid flags" );
		return DVERR_INVALIDFLAGS;
	}

	if( lpClientConfig->dwFlags & DVCLIENTCONFIG_MANUALVOICEACTIVATED && 
	   lpClientConfig->dwFlags & DVCLIENTCONFIG_AUTOVOICEACTIVATED )
	{
		DPVF( DPVF_ERRORLEVEL, "Cannot specify manual AND auto voice activated" );
		return DVERR_INVALIDFLAGS;
	}

	// If it's NOT manual, this parameter must be 0.
	if( !(lpClientConfig->dwFlags & DVCLIENTCONFIG_MANUALVOICEACTIVATED) )
	{
		if( lpClientConfig->dwThreshold != DVTHRESHOLD_UNUSED )
		{
			DPVF( DPVF_ERRORLEVEL, "Invalid sensitivity w/auto" );
			return DVERR_INVALIDPARAM;
		}

	}
	else
	{
		if( !DV_ValidSensitivity( lpClientConfig->dwThreshold ) )
		{
			DPVF( DPVF_ERRORLEVEL, "Invalid sensitivity" );
			return DVERR_INVALIDPARAM;
		}
	}

    if (!lpClientConfig->dwFlags) {

        DPVF( DPVF_ERRORLEVEL, "You must specify at least one flag!" );
	    return DVERR_INVALIDPARAM;
    }

	return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "DV_ValidSessionDesc"
// DV_ValidSessionDesc
//
// Checks the specified session description to ensure it's valid.
//
HRESULT DV_ValidSessionDesc( LPDVSESSIONDESC lpSessionDesc )
{
	if( lpSessionDesc == NULL)
	{
		DPVF( DPVF_ERRORLEVEL, "Invalid pointer" );				
		return E_POINTER;
	}


	if( lpSessionDesc->dwSessionType != DVSESSIONTYPE_PEER &&
	    lpSessionDesc->dwSessionType != DVSESSIONTYPE_FORWARDING)
	{
		DPVF( DPVF_ERRORLEVEL, "Invalid session type" );	
		return DVERR_INVALIDPARAM;
	}

	if( lpSessionDesc->dwFlags & ~(DVSESSION_SERVERCONTROLTARGET | DVSESSION_NOHOSTMIGRATION) )
	{
		DPVF( DPVF_ERRORLEVEL, "Invalid flags" );
		return DVERR_INVALIDFLAGS;
	}

	return DV_OK;

}

// DV_ValidSensitivity
//
// Checks the sensitivity to ensure it's valid
//
BOOL DV_ValidSensitivity( DWORD dwValue )
{
	if( dwValue != DVTHRESHOLD_DEFAULT &&
	    (// Commented out because min is currently 0 (dwValue < DVTHRESHOLD_MIN) || 
	     (dwValue > DVTHRESHOLD_MAX) ) )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

#undef DPF_MODNAME
#define DPF_MODNAME "DV_CopySessionDescToBuffer"
//
// DV_CopySessionDescToBuffer
//
// Checks the parameters for validity and then copies the specified session description
// to the specified buffer.  (If it will fit).
//
HRESULT DV_CopySessionDescToBuffer( LPVOID lpTarget, LPDVSESSIONDESC lpdvSessionDesc, LPDWORD lpdwSize )
{ 
	LPDVSESSIONDESC lpSessionDesc = (LPDVSESSIONDESC) lpTarget;

	if( lpdwSize == NULL)
	{
		DPVF( DPVF_ERRORLEVEL, "Invalid pointer" );
		return E_POINTER;
	}

	if( (*lpdwSize) < sizeof( DVSESSIONDESC ) )
	{
		*lpdwSize = sizeof( DVSESSIONDESC );	

		DPVF(  DPVF_INFOLEVEL, "Error size" );
		return DVERR_BUFFERTOOSMALL;
	}

	*lpdwSize = sizeof( DVSESSIONDESC );	

	if( lpTarget == NULL)
	{
		DPVF( DPVF_ERRORLEVEL, "Target buffer pointer bad" );
		return E_POINTER;
	}

	memcpy( lpTarget, lpdvSessionDesc, sizeof( DVSESSIONDESC ) ); 

	DPVF(  DPVF_ENTRYLEVEL, "DVCE::GetSessionDesc() Success" );

	return DV_OK;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DV_DUMP_GUID"
void DV_DUMP_GUID( GUID guid )
{
    DPVF( DPVF_INFOLEVEL, "{%-08.8X-%-04.4X-%-04.4X-%02.2X%02.2X-%02.2X%02.2X%02.2X%02.2X%02.2X%02.2X}", guid.Data1, guid.Data2, guid.Data3, 
               guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
               guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7] );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DV_DUMP_SessionDesc"
void DV_DUMP_SessionDesc( LPDVSESSIONDESC lpdvSessionDesc )
{
	ASSERT( lpdvSessionDesc != NULL );

	// 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPVF( DPVF_STRUCTUREDUMP, "DVSESSIONDESC Dump Addr=0x%p", lpdvSessionDesc );
	DPVF( DPVF_STRUCTUREDUMP, "dwSize = %d", lpdvSessionDesc->dwSize );
	DPVF( DPVF_STRUCTUREDUMP, "dwFlags = 0x%x", lpdvSessionDesc->dwFlags );
	DPVF( DPVF_STRUCTUREDUMP, "          %s", (lpdvSessionDesc->dwFlags & DVSESSION_SERVERCONTROLTARGET) ? "DVSESSION_SERVERCONTROLTARGET," : "");

	switch( lpdvSessionDesc->dwSessionType )
	{
	case DVSESSIONTYPE_PEER:
		DPVF( DPVF_STRUCTUREDUMP, "dwSessionType = DVSESSIONTYPE_PEER" );
		break;
	case DVSESSIONTYPE_FORWARDING: 
		DPVF( DPVF_STRUCTUREDUMP, "dwSessionType = DVSESSIONTYPE_FORWARDING" );
		break;
	default:
		DPVF( DPVF_STRUCTUREDUMP, "dwSessionType = Unknown" );
		break;
	}

	DPVF( DPVF_STRUCTUREDUMP, "guidCT = " );
	DV_DUMP_GUID( lpdvSessionDesc->guidCT );

}

#undef DPF_MODNAME
#define DPF_MODNAME "DV_DUMP_DSBDESC"
void DV_DUMP_DSBDESC( LPDSBUFFERDESC lpdsBufferDesc )
{
	// 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPVF( DPVF_STRUCTUREDUMP, "DSBUFFERDESC DUMP Addr=0x%p", lpdsBufferDesc );

	DPVF( DPVF_STRUCTUREDUMP, "dwSize = %d", lpdsBufferDesc->dwSize );
	DPVF( DPVF_STRUCTUREDUMP, "dwFlags = 0x%x", lpdsBufferDesc->dwFlags );
	DPVF( DPVF_STRUCTUREDUMP, "dwBufferBytes = %d", lpdsBufferDesc->dwBufferBytes );

	if( lpdsBufferDesc->lpwfxFormat == NULL )
	{
		DPVF( DPVF_STRUCTUREDUMP, "lpwfxFormat = NULL" );
	}
	else
	{
		DPVF( DPVF_STRUCTUREDUMP, "lpwfxFormat = " );	
		DV_DUMP_WaveFormatEx(lpdsBufferDesc->lpwfxFormat);
	}
	
}

#undef DPF_MODNAME
#define DPF_MODNAME "DV_DUMP_ClientConfig"
void DV_DUMP_ClientConfig( LPDVCLIENTCONFIG lpdvClientConfig )
{
	ASSERT( lpdvClientConfig != NULL );

	// 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPVF( DPVF_STRUCTUREDUMP, "DVCLIENTCONFIG Dump Addr = 0x%p", lpdvClientConfig );
	DPVF( DPVF_STRUCTUREDUMP, "dwSize = %d", lpdvClientConfig->dwSize );
	DPVF( DPVF_STRUCTUREDUMP, "dwFlags = 0x%x", lpdvClientConfig->dwFlags );
	DPVF( DPVF_STRUCTUREDUMP, "          %s%s%s%s%s%s", 
							(lpdvClientConfig->dwFlags & DVCLIENTCONFIG_RECORDMUTE) ? "DVCLIENTCONFIG_RECORDMUTE," : "", 
							(lpdvClientConfig->dwFlags & DVCLIENTCONFIG_PLAYBACKMUTE) ? "DVCLIENTCONFIG_PLAYBACKMUTE," : "",
							(lpdvClientConfig->dwFlags & DVCLIENTCONFIG_MANUALVOICEACTIVATED) ? "DVCLIENTCONFIG_MANUALVOICEACTIVATED," : "", 
							(lpdvClientConfig->dwFlags & DVCLIENTCONFIG_AUTOVOICEACTIVATED) ? "DVCLIENTCONFIG_AUTOVOICEACTIVATED," : "",
							(lpdvClientConfig->dwFlags & DVCLIENTCONFIG_MUTEGLOBAL) ? "DVCLIENTCONFIG_MUTEGLOBAL," : "");


}

#undef DPF_MODNAME
#define DPF_MODNAME "DV_DUMP_WaveFormatEx"
void DV_DUMP_WaveFormatEx( LPWAVEFORMATEX lpwfxFormat )
{
	ASSERT( lpwfxFormat != NULL );

	// 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
	DPVF( DPVF_STRUCTUREDUMP, "WAVEFORMATEX Dump Addr = 0x%p", lpwfxFormat );
	DPVF( DPVF_STRUCTUREDUMP, "wFormatTag = %d", lpwfxFormat->wFormatTag );
	DPVF( DPVF_STRUCTUREDUMP, "nSamplesPerSec = %d", lpwfxFormat->nSamplesPerSec );
	DPVF( DPVF_STRUCTUREDUMP, "nChannels = %d", lpwfxFormat->nChannels );
	DPVF( DPVF_STRUCTUREDUMP, "wBitsPerSample = %d", lpwfxFormat->wBitsPerSample );
	DPVF( DPVF_STRUCTUREDUMP, "nAvgBytesPerSec = %d", lpwfxFormat->nAvgBytesPerSec );
	DPVF( DPVF_STRUCTUREDUMP, "nBlockAlign = %d", lpwfxFormat->nBlockAlign );
	DPVF( DPVF_STRUCTUREDUMP, "cbSize = %d", lpwfxFormat->cbSize );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DV_ValidMessageArray"
// Validate message mask.
//
// May be too strict to enforce server only or client/only.
//
HRESULT DV_ValidMessageArray( LPDWORD lpdwMessages, DWORD dwNumMessages, BOOL fServer )
{
	if( dwNumMessages > 0 &&
	    (lpdwMessages == NULL))
	{
		DPVF( DPVF_ERRORLEVEL, "Invalid pointer passed for the lpdwMessages parameter." );
		return DVERR_INVALIDPOINTER;
	}

	if( lpdwMessages != NULL && dwNumMessages == 0 )
	{
		DPVF( DPVF_ERRORLEVEL, "Non-NULL notification array with 0 size" );
		return DVERR_INVALIDPARAM;
	}

	DWORD dwIndex, dwSubIndex;

	for( dwIndex = 0; dwIndex < dwNumMessages; dwIndex++ )
	{
		if( lpdwMessages[dwIndex] < DVMSGID_MINBASE || lpdwMessages[dwIndex] > DVMSGID_MAXBASE )
		{
			DPVF( DPVF_ERRORLEVEL, "Invalid message specified in notification array" );
			return DVERR_INVALIDPARAM;
		}
			
		switch( lpdwMessages[dwIndex] )
		{
		// Player only messages
		case DVMSGID_PLAYERVOICESTART:
		case DVMSGID_PLAYERVOICESTOP:
		case DVMSGID_RECORDSTART:
		case DVMSGID_RECORDSTOP:
		case DVMSGID_CONNECTRESULT:
		case DVMSGID_DISCONNECTRESULT:
		case DVMSGID_INPUTLEVEL:
		case DVMSGID_SETTARGETS:
		case DVMSGID_PLAYEROUTPUTLEVEL:
		case DVMSGID_HOSTMIGRATED:
			if( fServer )
			{
				DPVF( 0, "Client-only notification ID specified in server notification mask" );
				return DVERR_INVALIDPARAM;
			}
			break;
		}

		for( dwSubIndex = 0; dwSubIndex < dwNumMessages; dwSubIndex++ )
		{	
			if( dwIndex != dwSubIndex && 
			    lpdwMessages[dwIndex] == lpdwMessages[dwSubIndex] ) 
			{
				DPVF( 0, "Duplicate IDs specified in notification mask" );
				return DVERR_INVALIDPARAM;
			}
		}
	}

	return TRUE;
}


