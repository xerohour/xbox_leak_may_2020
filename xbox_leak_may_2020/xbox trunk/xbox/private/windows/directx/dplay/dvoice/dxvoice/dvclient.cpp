/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dvclient.c
 *  Content:	Implements functions for the DirectXVoiceClient interface
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	07/02/99	rodtoll	Created It
 *  07/26/99	rodtoll	Updated QueryInterface to support IDirectXVoiceNotify
 *  08/25/99	rodtoll	General Cleanup/Modifications to support new 
 *						compression sub-system. 
 *						Added new parameter to GetCompressionTypes
 *  09/03/99	rodtoll	Updated parameters for DeleteUserBuffer
 *  09/07/99	rodtoll	Updated EnumCompressionTypes so that object doesn't
 *						need to be Initialized.
 *  09/10/99	rodtoll	Object validity checking
 *  09/14/99	rodtoll	Added DVC_SetNotifyMask  
 *  10/05/99	rodtoll	Reversed destruction order to destroy object before
 *						transport.  Fixes crash in some situations
 *  10/18/99	rodtoll	Fix: Passing NULL in QueryInterface casues crash
 *				rodtoll	Fix: Calling Initialize twice passes
 *  10/19/99	rodtoll	Fix: Bug #113904 - Shutdown issues
 *                      - Added reference count for notify interface, allows
 *                        determination if disconnect should be called from release
 *  10/25/99	rodtoll	Fix: Bug #114098 - Release/Addref failure from multiple threads 
 *  11/17/99	rodtoll	Fix: Bug #117447 - Removed checks for initialization because
 *						DirectVoiceCLientEngine members already do this.
 *  12/16/99	rodtoll	Bug #117405 - 3D Sound APIs misleading - 3d sound apis renamed
 *						The Delete3DSoundBuffer was re-worked to match the create
 *  01/14/2000	rodtoll	Updated parameters to Get/SetTransmitTargets
 *				rodtoll	Added new API call GetSoundDeviceConfig 
 *  01/27/2000	rodtoll	Bug #129934 - Update Create3DSoundBuffer to take DSBUFFERDESC  
 *  03/28/2000  rodtoll   Removed reference to removed header file.
 *  06/21/2000	rodtoll	Bug #35767 - Update Create3DSoundBuffer to take DIRECTSOUNDBUFFERs
 *
 ***************************************************************************/

#include "dvntos.h"
#include  <dvoicep.h>
#include "dvclient.h"
#include "in_core.h"
#include "trnotify.h"

#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_Initialize"
HRESULT WINAPI IDirectPlayVoiceClient_Initialize(LPDIRECTPLAYVOICECLIENT lpDV, PVOID lpTransport, LPDVMESSAGEHANDLER lpMessageHandler, LPVOID lpUserContext, LPDWORD lpdwMessages, DWORD dwNumElements )
{
    HRESULT hr;
    //
    // call the common routine
    //

    hr = DV_Initialize((LPDIRECTVOICEOBJECT) lpDV, lpTransport,lpMessageHandler, lpUserContext, lpdwMessages, dwNumElements);
    PAGED_CODE();
    return hr;

}


#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_DoWork"
HRESULT WINAPI IDirectPlayVoiceClient_DoWork(LPDIRECTPLAYVOICECLIENT lpDV)
{

    LPDIRECTVOICECLIENTOBJECT This  = (LPDIRECTVOICECLIENTOBJECT) lpDV;
    HRESULT hr;
	ASSERT( This != NULL );
	ASSERT( This->lpDVEngine != NULL );


	hr = This->lpDVClientEngine->DoWork();
    PAGED_CODE();
    return hr;

}


#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_AddRef"
ULONG WINAPI IDirectPlayVoiceClient_AddRef(LPDIRECTPLAYVOICECLIENT This )
{
    LPDIRECTVOICECLIENTOBJECT lpDV  = (LPDIRECTVOICECLIENTOBJECT) This;
	LONG rc;
    KIRQL irql;
	RIRQL(irql); 

	rc = ++lpDV->lIntRefCnt;

    LIRQL(irql); 
	return rc;
}

#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_Release"
ULONG WINAPI IDirectPlayVoiceClient_Release(LPDIRECTPLAYVOICECLIENT This )
{
    LPDIRECTVOICECLIENTOBJECT lpDV  = (LPDIRECTVOICECLIENTOBJECT) This;
    HRESULT hr=S_OK;
    LONG rc;
    KIRQL irql;
	ASSERT( lpDV );

	RIRQL(irql);

	if (lpDV->lIntRefCnt == 0)
	{
		LIRQL(irql);
		return 0;
	}

	// dec the interface count
	lpDV->lIntRefCnt--;

	// Special case: Releasing object without stopping session
	// May be more then one transport thread indicating in us 
	if( (lpDV->lIntRefCnt == 0) && lpDV->lpDVClientEngine->GetCurrentState() != DVCSTATE_IDLE  )
	{
		DPVF( DPVF_ERRORLEVEL, "Releasing interface without calling Disconnect" );

		lpDV->lIntRefCnt = 0;

		// We must release the lock because stopsession may call back into this function
		LIRQL(irql);		

		hr = lpDV->lpDVClientEngine->Disconnect( 0 );

		RIRQL(irql);			

		if( FAILED(hr))
		{
			DPVF( DPVF_ERRORLEVEL, "Disconnect Failed hr=0x%x", hr );
		}

	}

    PAGED_CODE();

	rc = lpDV->lIntRefCnt;

	if ( lpDV->lIntRefCnt == 0 )
	{
		// Leave the critical section, we may call back into this func.
		// (Shouldn't though).
		LIRQL(irql);

		delete lpDV->lpDVClientEngine;
		lpDV->lpDVClientEngine = NULL;

		if( lpDV->lpDVTransport != 0 )
		{
			ASSERT( lpDV->lpDVTransport->m_lRefCount == 0 );		
			delete lpDV->lpDVTransport;
			lpDV->lpDVTransport = NULL;
		}

		free(lpDV);
	} 
	else
	{
		LIRQL(irql);
	}
   	
    PAGED_CODE();
    return rc;
}


#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_Connect"
STDMETHODIMP IDirectPlayVoiceClient_Connect(LPDIRECTPLAYVOICECLIENT lpDV,LPDVCLIENTCONFIG lpClientConfig, DWORD dwFlags )
{
    HRESULT hr;
    LPDIRECTVOICECLIENTOBJECT This  = (LPDIRECTVOICECLIENTOBJECT) lpDV;
	ASSERT( This != NULL );
	ASSERT( This->lpDVEngine != NULL );


	hr = This->lpDVClientEngine->Connect(lpClientConfig, dwFlags );
    PAGED_CODE();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_Disconnect"
STDMETHODIMP IDirectPlayVoiceClient_Disconnect(LPDIRECTPLAYVOICECLIENT lpDV, DWORD dwFlags)
{
    HRESULT hr;
    LPDIRECTVOICECLIENTOBJECT This  = (LPDIRECTVOICECLIENTOBJECT) lpDV;
	ASSERT( This != NULL );
	ASSERT( This->lpDVEngine != NULL );

	hr = This->lpDVClientEngine->Disconnect( dwFlags );
    PAGED_CODE();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_GetSessionDesc"
STDMETHODIMP IDirectPlayVoiceClient_GetSessionDesc(LPDIRECTPLAYVOICECLIENT lpDV, LPDVSESSIONDESC lpSessionDesc )
{
    LPDIRECTVOICECLIENTOBJECT This  = (LPDIRECTVOICECLIENTOBJECT) lpDV;
    HRESULT hr;
	ASSERT( This != NULL );
	ASSERT( This->lpDVEngine != NULL );
	hr = This->lpDVClientEngine->GetSessionDesc( lpSessionDesc );
    PAGED_CODE();
    return hr;
}


#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_GetClientConfig"
STDMETHODIMP IDirectPlayVoiceClient_GetClientConfig(LPDIRECTPLAYVOICECLIENT lpDV, LPDVCLIENTCONFIG lpClientConfig )
{
    HRESULT hr;
    LPDIRECTVOICECLIENTOBJECT This  = (LPDIRECTVOICECLIENTOBJECT) lpDV;
	ASSERT( This != NULL );
	ASSERT( This->lpDVEngine != NULL );

	hr = This->lpDVClientEngine->GetClientConfig( lpClientConfig );
    PAGED_CODE();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "DVC_SetClientConfig"
STDMETHODIMP IDirectPlayVoiceClient_SetClientConfig(LPDIRECTPLAYVOICECLIENT lpDV, LPDVCLIENTCONFIG lpClientConfig )
{
    HRESULT hr;
    LPDIRECTVOICECLIENTOBJECT This  = (LPDIRECTVOICECLIENTOBJECT) lpDV;
	ASSERT( This != NULL );
	ASSERT( This->lpDVEngine != NULL );

	hr = This->lpDVClientEngine->SetClientConfig( lpClientConfig );
    PAGED_CODE();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_SetTransmitTargets"
STDMETHODIMP IDirectPlayVoiceClient_SetTransmitTargets( LPDIRECTPLAYVOICECLIENT lpDV, PDVID pdvidTargets, DWORD dwNumTargets, DWORD dwFlags )
{
    HRESULT hr;
    LPDIRECTVOICECLIENTOBJECT This  = (LPDIRECTVOICECLIENTOBJECT) lpDV;
	ASSERT( This != NULL );
	ASSERT( This->lpDVEngine != NULL );
	hr = This->lpDVClientEngine->SetTransmitTarget( pdvidTargets, dwNumTargets, dwFlags );
    PAGED_CODE();
    return hr;
}


#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_GetTransmitTarget"
STDMETHODIMP IDirectPlayVoiceClient_GetTransmitTarget(LPDIRECTPLAYVOICECLIENT lpDV, LPDVID lpdvidTargets, PDWORD pdwNumElements, DWORD dwFlags )
{
    HRESULT hr;
    LPDIRECTVOICECLIENTOBJECT This  = (LPDIRECTVOICECLIENTOBJECT) lpDV;
	ASSERT( This != NULL );
	ASSERT( This->lpDVEngine != NULL );
	hr = This->lpDVClientEngine->GetTransmitTarget( lpdvidTargets, pdwNumElements, dwFlags );
    PAGED_CODE();
    return hr;
}


#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_SetNotifyMask"
STDMETHODIMP IDirectPlayVoiceClient_SetNotifyMask( LPDIRECTPLAYVOICECLIENT lpDV, LPDWORD lpdwMessages, DWORD dwNumElements )
{
    HRESULT hr;
    LPDIRECTVOICECLIENTOBJECT This  = (LPDIRECTVOICECLIENTOBJECT) lpDV;
	ASSERT( This != NULL );
	ASSERT( This->lpDVEngine != NULL );
	hr = This->lpDVClientEngine->SetNotifyMask( lpdwMessages, dwNumElements );
    PAGED_CODE();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_CreateSoundTarget"
STDMETHODIMP IDirectPlayVoiceClient_CreateSoundTarget( LPDIRECTPLAYVOICECLIENT lpDV, DVID dvidID, PWAVEFORMATEX *ppwfxMediaFormat, XMediaObject **ppMediaObject)
{
    HRESULT hr;
    LPDIRECTVOICECLIENTOBJECT This  = (LPDIRECTVOICECLIENTOBJECT) lpDV;
	ASSERT( This != NULL );
	ASSERT( This->lpDVEngine != NULL );
	hr = This->lpDVClientEngine->CreateSoundTarget( dvidID, ppwfxMediaFormat, ppMediaObject);
    PAGED_CODE();
    return hr;


}

#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceClient_DeleteSoundTarget"
STDMETHODIMP IDirectPlayVoiceClient_DeleteSoundTarget( LPDIRECTPLAYVOICECLIENT lpDV, DVID dvidID, XMediaObject **ppMediaObject)
{

    HRESULT hr;
    LPDIRECTVOICECLIENTOBJECT This  = (LPDIRECTVOICECLIENTOBJECT) lpDV;
	ASSERT( This != NULL );
	ASSERT( This->lpDVEngine != NULL );
	hr = This->lpDVClientEngine->DeleteSoundTarget(dvidID, ppMediaObject );
    PAGED_CODE();
    return hr;

}


