/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       unk.c
 *  Content:	IUnknown implementation
 *  History:
 *   Date	By		Reason
 *   ====	==		======
 * 07/02/99 rodtoll Modified existing unk.c for use w/DirectXVoice
 * 07/26/99	rodtoll	Added the new IDirectXVoiceNotify Interfaces
 * 08/09/99 rodtoll	Fixed VTable for server notify interface
 * 08/25/99	rodtoll	General Cleanup/Modifications to support new 
 *					compression sub-system. 
 * 08/31/99	rodtoll	Updated to use new debug libs 
 * 09/02/99	pnewson	Added IDirectXVoiceSetup interface
 * 09/07/99	rodtoll	Added DPF_MODNAMEs to the module
 * 			rodtoll	Fixed vtable for server object
 * 09/10/99	rodtoll	Vtables from static to non-static so other modules can access
 * 09/13/99	pnewson added dplobby.h include so lobby GUIDs get created
 * 09/14/99	rodtoll	Modified VTable to add new SetNotifyMask func
 * 10/05/99	rodtoll	Added DPFs
 * 10/07/99	rodtoll	Updated to work in Unicode, Add Init of OS Abstraction Layer
 * 10/18/99	rodtoll	Fix: Passing NULL in QueryInterface casues crash
 * 10/19/99	rodtoll	Fix: Bug #113904 Release Issues
 *					Added init for notify interface count
 * 10/25/99	rodtoll	Fix: Bug #114098 - Release/Addref failure from multiple threads 
 * 11/12/99	rodtoll	Fixed to use new dsound header.
 * 11/30/99	pnewson	Bug #117449 - IDirectPlayVoiceSetup Parameter validation
 * 12/01/99	rodtoll	Added includes to define and instantiate GUID_NULL
 * 12/16/99	rodtoll Bug #117405 - 3D sound APIs misleading
 * 01/14/00	rodtoll	Added new DVC_GetSoundDeviceConfig member to VTable
 * 02/17/00	rodtoll	Bug #133691 - Choppy audio - queue was not adapting
 *					Added instrumentation
 *			rodtoll	Removed self-registration code
 * 03/03/00	rodtoll	Updated to handle alternative gamevoice build. 
 * 04/11/00 rodtoll Added code for redirection for custom builds if registry bit is set 
 * 04/21/00 rodtoll Bug #32889 - Does not run on Win2k on non-admin account 
 * 06/07/00	rodtoll	Bug #34383 Must provide CLSID for each IID to fix issues with Whistler
 *  06/09/00    rmt     Updates to split CLSID and allow whistler compat and support external create funcs 
 * 06/28/2000	rodtoll	Prefix Bug #38022
 * 07/05/00	rodtoll	Moved code to new dllmain.cpp
 ***************************************************************************/

#include "dvntos.h"
#include <dvoicep.h>
#include "dvclient.h"
#include "dvserver.h"
#include "trnotify.h"
#include "in_core.h"

#define EXP __declspec(dllexport)

/*#ifdef __MWERKS__
	#define EXP __declspec(dllexport)
#else
	#define EXP
#endif*/

//
// keep global count of when/if we were initialized
//

LONG   g_DirectVoiceInitCount = 0;
#if DBG

UCHAR g_ModName[256];
PVOID g_This = NULL;
DVDEBUGDATA g_DVDbgData;

ULONG  g_DVDebugLevel = DPVL_ERRORS_ONLY;

#endif

#undef DPF_MODNAME
#define DPF_MODNAME "DirectPlayVoiceCreate"

__inline HRESULT WINAPI XDirectPlayVoiceCreate(
	DWORD dwIID, 
	void** ppvInterface)
{

    return DirectPlayVoiceCreate(dwIID, ppvInterface, NULL);

}


HRESULT WINAPI DirectPlayVoiceCreate( DWORD dwIID, void **ppvInterface, void *pUnknown) 
{
    HRESULT hr = DV_OK;
    LPDIRECTVOICECLIENTOBJECT pDVCInt;

    ASSERT(pUnknown == NULL);

#if DBG

    memset(&g_DVDbgData,0,sizeof(g_DVDbgData));

#endif

#ifndef ASSUME_VALID_PARAMETERS

    if( dwIID != IID_IDirectPlayVoiceClient && 
        dwIID != IID_IDirectPlayVoiceServer)
    {
        DPVF( 0, "Interface ID is not recognized" );
        return DVERR_INVALIDPARAM;
    }

    if( ppvInterface == NULL )
    {
        DPVF( 0, "Invalid pointer specified to receive interface" );
        return DVERR_INVALIDPOINTER;
    }
#endif

    if (g_DirectVoiceInitCount == 0) {

        CDirectVoiceEngine::Startup();
        InterlockedIncrement(&g_DirectVoiceInitCount );

    }

	if( dwIID == IID_IDirectPlayVoiceServer )
	{
		LPDIRECTVOICESERVEROBJECT pDVSInt;
    
        pDVSInt = static_cast<LPDIRECTVOICESERVEROBJECT>( malloc(sizeof(DIRECTVOICESERVEROBJECT)) );
        if (pDVSInt == NULL)
        {
            DPVF( DPVF_ERRORLEVEL, "Out of memory" );
            return E_OUTOFMEMORY;
        }
    
        pDVSInt->lpDVServerEngine = new CDirectVoiceServerEngine(pDVSInt);
        pDVSInt->lpDVEngine = static_cast<CDirectVoiceEngine *>(pDVSInt->lpDVServerEngine);
    
        if (pDVSInt->lpDVEngine == NULL)
        {
            DPVF( DPVF_ERRORLEVEL, "Out of memory" );
            free( pDVSInt );
            return E_OUTOFMEMORY;
        }
    
        pDVSInt->lpDVTransport = NULL;
        pDVSInt->lIntRefCnt = 0;
        pDVSInt->dvNotify.lpDV = pDVSInt;
        pDVSInt->dvNotify.lRefCnt = 0;
    
        *ppvInterface = pDVSInt;

        // bump the refcount
		IDirectPlayVoiceServer_AddRef((LPDIRECTPLAYVOICESERVER) *ppvInterface);

	}
	else if( dwIID == IID_IDirectPlayVoiceClient) {    

        pDVCInt = static_cast<LPDIRECTVOICECLIENTOBJECT>( malloc(sizeof(DIRECTVOICECLIENTOBJECT)) );
        if (pDVCInt == NULL)
        {
            DPVF( DPVF_ERRORLEVEL, "Out of memory" );
            return E_OUTOFMEMORY;
        }
    
        pDVCInt->lpDVClientEngine = new CDirectVoiceClientEngine(pDVCInt);
        pDVCInt->lpDVEngine = static_cast<CDirectVoiceEngine *>(pDVCInt->lpDVClientEngine);
    
        if (pDVCInt->lpDVEngine == NULL)
        {
            DPVF( DPVF_ERRORLEVEL, "Out of memory" );
            free( pDVCInt );
            return E_OUTOFMEMORY;
        }
    
        pDVCInt->lpDVTransport = NULL;
        pDVCInt->lIntRefCnt = 0;
        pDVCInt->dvNotify.lpDV = pDVCInt;
        pDVCInt->dvNotify.lRefCnt = 0;
    
        *ppvInterface = pDVCInt;

        // get the right interface and bump the refcount
		IDirectPlayVoiceClient_AddRef((LPDIRECTPLAYVOICECLIENT) *ppvInterface);
    
    }    

    return hr;
}


