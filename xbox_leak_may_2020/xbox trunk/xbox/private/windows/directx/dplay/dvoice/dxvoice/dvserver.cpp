/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dvserver.cpp
 *  Content:	Implements functions for the IDirectXVoiceServer interface
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	07/02/99	rodtoll	Created It
 *  07/26/99	rodtoll	Updated QueryInterface to support IDirectXVoiceNotify
 *  08/25/99	rodtoll	General Cleanup/Modifications to support new 
 *						compression sub-system. 
 *						Added parameter to the GetCompressionTypes func
 *  09/10/99	rodtoll	Object validity checking 
 *  09/14/99	rodtoll	Added DVS_SetNotifyMask  
 *  10/05/99	rodtoll	Reversed destruction order to destroy object before
 *						transport.  Fixes crash on host migration shutdown.  
 *  10/18/99	rodtoll	Fix: Passing NULL in QueryInterface casues crash 
 *				rodtoll	Fix: Calling Initialize twice passes 
 *  10/19/99	rodtoll	Fix: Bug #113904 - Shutdown issues
 *                      - Added reference count for notify interface, allows
 *                        determination if stopsession should be called from release
 *  10/25/99	rodtoll	Fix: Bug #114098 - Release/Addref failure from multiple threads 
 *  01/14/2000	rodtoll	Updated with new parameters for Set/GetTransmitTarget
 *				rodtoll	Removed DVFLAGS_SYNC from StopSession call
 *
 ***************************************************************************/


#include "dvserver.h"


#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceServer_Initialize"
HRESULT WINAPI IDirectPlayVoiceServer_Initialize(LPDIRECTPLAYVOICESERVER lpDV, PVOID lpTransport, LPDVMESSAGEHANDLER lpMessageHandler, LPVOID lpUserContext, LPDWORD lpdwMessages, DWORD dwNumElements )
{

    //
    // call the common routine
    //

    return DV_Initialize((LPDIRECTVOICEOBJECT) lpDV, lpTransport,lpMessageHandler, lpUserContext, lpdwMessages, dwNumElements);

}


#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceServer_AddRef"
ULONG WINAPI IDirectPlayVoiceServer_AddRef(LPDIRECTPLAYVOICESERVER This )
{
    LPDIRECTVOICESERVEROBJECT lpDV  = (LPDIRECTVOICESERVEROBJECT) This;
	LONG rc;
    KIRQL irql;
	RIRQL(irql); 

	rc = ++lpDV->lIntRefCnt;

    LIRQL(irql); 
	return rc;
}

#undef DPF_MODNAME
#define DPF_MODNAME "DVS_Release"
ULONG WINAPI IDirectPlayVoiceServer_Release(LPDIRECTPLAYVOICESERVER  lpDV )
{
    HRESULT hr=S_OK;
    LONG rc;
    KIRQL irql;
    LPDIRECTVOICESERVEROBJECT pDVS = (LPDIRECTVOICESERVEROBJECT) lpDV;

	ASSERT( pDVS );

	RIRQL(irql);
	
	if (pDVS->lIntRefCnt == 0)
	{
		LIRQL(irql);
		return 0;
	}

	// dec the interface count
	pDVS->lIntRefCnt--;

	// Special case: Releasing object without stopping session
	if( (pDVS->lIntRefCnt == 0) && pDVS->lpDVServerEngine->GetCurrentState() != DVSSTATE_IDLE )
	{
		DPVF( DPVF_ERRORLEVEL, "Releasing interface without calling StopSession" );

		pDVS->lIntRefCnt = 0;

		// We must release the lock because stopsession may call back into this function
        LIRQL(irql);

		hr = pDVS->lpDVServerEngine->StopSession( 0 );

        RIRQL(irql);

		if( hr != DV_OK && hr != DVERR_SESSIONLOST  )
		{
			DPVF( DPVF_ERRORLEVEL, "StopSession Failed hr=0x%x", hr );
		}

	}

	rc = pDVS->lIntRefCnt;

	if ( pDVS->lIntRefCnt == 0 )
	{
		// Leave the critical section, we may call back into this func.
		// (Shouldn't though).

		ASSERT( pDVS->lpDVServerEngine );

		delete pDVS->lpDVServerEngine;
		pDVS->lpDVServerEngine = NULL;

		if( pDVS->lpDVTransport != 0 )
		{
			ASSERT( pDVS->lpDVTransport->m_lRefCount == 0 );				
			delete pDVS->lpDVTransport;
			pDVS->lpDVTransport = NULL;
		}

        LIRQL(irql);

		free(pDVS);
	} 
	else
	{
        LIRQL(irql);
	}
   	
    return rc;
}


#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceServer_StartSession"
STDMETHODIMP IDirectPlayVoiceServer_StartSession(LPDIRECTPLAYVOICESERVER This, LPDVSESSIONDESC lpdvSessionDesc, DWORD dwFlags )
{
	ASSERT( This != NULL );
	ASSERT( ((LPDIRECTVOICESERVEROBJECT)This)->lpDVEngine != NULL );
	return ((LPDIRECTVOICESERVEROBJECT)This)->lpDVServerEngine->StartSession( lpdvSessionDesc, dwFlags );
}

#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceServer_StopSession"
STDMETHODIMP IDirectPlayVoiceServer_StopSession(LPDIRECTPLAYVOICESERVER This, DWORD dwFlags )
{    
	ASSERT( This != NULL );
	ASSERT( ((LPDIRECTVOICESERVEROBJECT)This)->lpDVEngine != NULL );
	return ((LPDIRECTVOICESERVEROBJECT)This)->lpDVServerEngine->StopSession( dwFlags );
}

#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceServer_GetSessionDesc"
STDMETHODIMP IDirectPlayVoiceServer_GetSessionDesc(LPDIRECTPLAYVOICESERVER This, LPDVSESSIONDESC lpdvSessionDesc )
{
	ASSERT( This != NULL );
	ASSERT( ((LPDIRECTVOICESERVEROBJECT)This)->lpDVEngine != NULL );
	return ((LPDIRECTVOICESERVEROBJECT)This)->lpDVServerEngine->GetSessionDesc( lpdvSessionDesc );
}

#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceServer_SetTransmitTargets"
STDMETHODIMP IDirectPlayVoiceServer_SetTransmitTargets( LPDIRECTPLAYVOICESERVER This, DVID dvidSource, PDVID pdvidTargets, DWORD dwNumTargets, DWORD dwFlags)
{
    
	ASSERT( This != NULL );
	ASSERT( ((LPDIRECTVOICESERVEROBJECT)This)->lpDVEngine != NULL );
	return ((LPDIRECTVOICESERVEROBJECT)This)->lpDVServerEngine->SetTransmitTarget( dvidSource, pdvidTargets, dwNumTargets, dwFlags );
}

#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceServer_GetTransmitTargets"
STDMETHODIMP IDirectPlayVoiceServer_GetTransmitTargets( LPDIRECTPLAYVOICESERVER This, DVID dvidSource, LPDVID pdvidTargets, PDWORD pdwNumElements, DWORD dwFlags)
{    
	ASSERT( This != NULL );
	ASSERT( ((LPDIRECTVOICESERVEROBJECT)This)->lpDVEngine != NULL );
	return ((LPDIRECTVOICESERVEROBJECT)This)->lpDVServerEngine->GetTransmitTarget( dvidSource, pdvidTargets, pdwNumElements, dwFlags );
}

#undef DPF_MODNAME
#define DPF_MODNAME "IDirectPlayVoiceServer_SetNotifyMask"
STDMETHODIMP IDirectPlayVoiceServer_SetNotifyMask( LPDIRECTPLAYVOICESERVER This, LPDWORD lpdwMessages, DWORD dwNumElements )
{
    
	ASSERT( This != NULL );
	ASSERT( ((LPDIRECTVOICESERVEROBJECT)This)->lpDVEngine != NULL );
	return ((LPDIRECTVOICESERVEROBJECT)This)->lpDVServerEngine->SetNotifyMask( lpdwMessages, dwNumElements );
}
