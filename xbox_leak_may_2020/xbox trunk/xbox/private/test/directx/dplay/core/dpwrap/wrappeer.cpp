//==================================================================================
// Includes
//==================================================================================
#include <xtl.h>
#include <xlog.h>
#include <xtestlib.h>
#include <netsync.h>
#include <xlog.h>

#include <dplay8.h>
#include <dpaddr.h>

#include "macros.h"
#include "wraputil.h"
#include "wrappeer.h"

#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::CWrapDP8Peer()"
//==================================================================================
// CWrapDP8Peer constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CWrapDP8Peer object.
//
// Arguments: None.
//
// Returns: None (the object).
//==================================================================================
CWrapDP8Peer::CWrapDP8Peer(HANDLE hLog):
#ifndef REMOVE_DPLAY_IMPORTS
	m_pDP8Peer(NULL),
#endif
	m_hLog(hLog),
	m_fInitialized(FALSE)
{
} // CWrapDP8Peer::CWrapDP8Peer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::~CWrapDP8Peer()"
//==================================================================================
// CWrapDP8Peer destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CWrapDP8Peer object.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CWrapDP8Peer::~CWrapDP8Peer(void)
{
#ifndef REMOVE_DPLAY_IMPORTS
	HRESULT			hr;

	if (this->m_fInitialized)
	{
		hr = this->DP8P_Close(0);
		if (hr != DPN_OK)
		{
			xLog(m_hLog, XLL_WARN, "Failed closing!  0x%08x", hr);
		} // end if (failed closing)
	} // end if (initialized object)

	if (this->m_pDP8Peer != NULL)
	{
		hr = this->Release();
		if (hr != S_OK)
		{
			xLog(m_hLog, XLL_WARN, "Failed releasing DirectPlay8Peer object!  0x%08x", hr);
		} // end if (failed releasing object)
	} // end if (we have an object pointer)
#endif

	m_hLog = NULL;
} // CWrapDP8Peer::~CWrapDP8Peer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::CoCreate()"
//==================================================================================
// CWrapDP8Peer::CoCreate
//----------------------------------------------------------------------------------
//
// Description: Calls CoCreateInstance for a new DirectPlay8Peer object.
//
// Arguments: None.
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
HRESULT CWrapDP8Peer::CoCreate(void)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	if (this->m_pDP8Peer != NULL)
	{
		xLog(m_hLog, XLL_WARN,  "Can't create a DirectPlay8Peer object because one already exists (%x)!", this->m_pDP8Peer);
		hr = ERROR_ALREADY_EXISTS;
	} // end if (already have an object)
	else
	{
		hr = DirectPlay8Create(IID_IDirectPlay8Peer, (LPVOID *) &this->m_pDP8Peer, NULL);
	} // end else (don't have an object)
#endif

	return (hr);
} // CWrapDP8Peer::CoCreate
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::Release()"
//==================================================================================
// CWrapDP8Peer::Release
//----------------------------------------------------------------------------------
//
// Description: Releases the DirectPlay8Peer object.
//
// Arguments: None.
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
HRESULT CWrapDP8Peer::Release(void)
{
	HRESULT		hr = DPN_OK;
	
#ifndef REMOVE_DPLAY_IMPORTS
	if (this->m_pDP8Peer == NULL)
	{
		xLog(m_hLog, XLL_WARN, "Can't release DirectPlay8Peer object because one doesn't exist!");
		hr = ERROR_BAD_ENVIRONMENT;
	} // end if (no object yet)
	else
	{
		this->m_pDP8Peer->Release();
		this->m_pDP8Peer = NULL;
		
		this->m_fInitialized = FALSE;
	} // end else (there's an object)
#endif

	return (hr);
} // CWrapDP8Peer::Release
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_Initialize()"
//==================================================================================
// CWrapDP8Peer::DP8P_Initialize
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	PVOID pvUserContext			?
//	PFNDPNMESSAGEHANDLER pfn	?
//	DWORD dwFlags				?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_Initialize(PVOID const pvUserContext,
											const PFNDPNMESSAGEHANDLER pfn,
											const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;
	
#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->Initialize(pvUserContext, pfn, dwFlags);

	if (hr == DPN_OK)
	{
		this->m_fInitialized = TRUE;
	} // end if (successfully initialized object)

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_Initialize
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DoWork()"
//==================================================================================
// CWrapDP8Peer::DP8P_DoWork
//----------------------------------------------------------------------------------
//
// Description: Pump function that causes DirectPlay to do some processing
//
// Arguments:
//	DWORD dwFlags				?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_DoWork(const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->DoWork(dwFlags);
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_DoWork
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_EnumServiceProviders()"
//==================================================================================
// CWrapDP8Peer::DP8P_EnumServiceProviders
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	GUID* pguidServiceProvider					?
//	GUID* pguidApplication						?
//	DPN_SERVICE_PROVIDER_INFO* pSPInfoBuffer	?
//	DWORD* pcbEnumData							?
//	DWORD* pcReturned							?
//	DWORD dwFlags								?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_EnumServiceProviders(const GUID * const pguidServiceProvider,
													const GUID * const pguidApplication,
													DPN_SERVICE_PROVIDER_INFO * const pSPInfoBuffer,
													DWORD * const pcbEnumData,
													DWORD * const pcReturned,
													const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->EnumServiceProviders(pguidServiceProvider,
		pguidApplication,
		pSPInfoBuffer,
		pcbEnumData,
		pcReturned,
		dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_EnumServiceProviders
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_CancelAsyncOperation()"
//==================================================================================
// CWrapDP8Peer::DP8P_CancelAsyncOperation
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNHANDLE hAsyncHandle	?
//	DWORD dwFlags			?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_CancelAsyncOperation(const DPNHANDLE hAsyncHandle,
													const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->CancelAsyncOperation(hAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_CancelAsyncOperation
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_Connect()"
//==================================================================================
// CWrapDP8Peer::DP8P_Connect
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPN_APPLICATION_DESC* pdnAppDesc			?
//	IDirectPlay8Address* pHostAddr				?
//	IDirectPlay8Address* pDeviceInfo			?
//	DPN_SECURITY_DESC* pdnSecurity				?
//	DPN_SECURITY_CREDENTIALS* pdnCredentials	?
//	void* pvUserConnectData						?
//	DWORD dwUserConnectDataSize					?
//	void* pvPlayerContext						?
//	void* pvAsyncContext						?
//	DPNHANDLE* phAsyncHandle					?
//	DWORD dwFlags								?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_Connect(const DPN_APPLICATION_DESC * const pdnAppDesc,
										IDirectPlay8Address * const pHostAddr,
										IDirectPlay8Address * const pDeviceInfo,
										const DPN_SECURITY_DESC * const pdnSecurity,
										const DPN_SECURITY_CREDENTIALS * const pdnCredentials,
										const void * const pvUserConnectData,
										const DWORD dwUserConnectDataSize,
										void * const pvPlayerContext,
										void * const pvAsyncContext,
										DPNHANDLE * const phAsyncHandle,
										const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->Connect(pdnAppDesc, pHostAddr, pDeviceInfo,
		pdnSecurity, pdnCredentials,
		pvUserConnectData, dwUserConnectDataSize,
		pvPlayerContext, pvAsyncContext,
		phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_Connect
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_SendTo()"
//==================================================================================
// CWrapDP8Peer::DP8P_SendTo
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID dpnid						?
//	DPN_BUFFER_DESC* prgBufferDesc	?
//	DWORD cBufferDesc				?
//	DWORD dwTimeOut					?
//	void* pvAsyncContext			?
//	DPNHANDLE* phAsyncHandle		?
//	DWORD dwFlags					?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_SendTo(const DPNID dpnid,
										const DPN_BUFFER_DESC * const prgBufferDesc,
										const DWORD cBufferDesc,
										const DWORD dwTimeOut,
										void * const pvAsyncContext,
										DPNHANDLE * const phAsyncHandle,
										const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->SendTo(dpnid, prgBufferDesc, cBufferDesc, dwTimeOut,
								pvAsyncContext, phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_SendTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_GetSendQueueInfo()"
//==================================================================================
// CWrapDP8Peer::DP8P_GetSendQueueInfo
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID dpnid			?
//	DWORD* pdwNumMsgs	?
//	DWORD* pdwNumBytes	?
//	DWORD dwFlags		?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_GetSendQueueInfo(const DPNID dpnid,
												DWORD * const pdwNumMsgs,
												DWORD * const pdwNumBytes,
												const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;
	
#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->GetSendQueueInfo(dpnid, pdwNumMsgs, pdwNumBytes,
		dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_GetSendQueueInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_Host()"
//==================================================================================
// CWrapDP8Peer::DP8P_Host
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPN_APPLICATION_DESC* pdnAppDesc			?
//	IDirectPlay8Address** prgpDeviceInfo		?
//	DWORD cDeviceInfo							?
//	DPN_SECURITY_DESC* pdnSecurity				?
//	DPN_SECURITY_CREDENTIALS* pdnCredentials	?
//	void * pvPlayerContext						?
//	DWORD dwFlags								?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
// BUGBUG - working around requirement for host to have a session name
STDMETHODIMP CWrapDP8Peer::DP8P_Host(const DPN_APPLICATION_DESC * const pdnAppDesc,
									 IDirectPlay8Address ** const prgpDeviceInfo,
									const DWORD cDeviceInfo,
									const DPN_SECURITY_DESC * const pdnSecurity,
									const DPN_SECURITY_CREDENTIALS * const pdnCredentials,
									void * const pvPlayerContext,
									const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->Host(pdnAppDesc, prgpDeviceInfo, cDeviceInfo,
								pdnSecurity, pdnCredentials, pvPlayerContext,
								dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_Host
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_GetApplicationDesc()"
//==================================================================================
// CWrapDP8Peer::DP8P_GetApplicationDesc
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPN_APPLICATION_DESC* pAppDescBuffer	?
//	DWORD* pcbDataSize						?
//	DWORD dwFlags							?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_GetApplicationDesc(DPN_APPLICATION_DESC * const pAppDescBuffer,
													DWORD * const pcbDataSize,
													const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->GetApplicationDesc(pAppDescBuffer, pcbDataSize, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_GetApplicationDesc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_SetApplicationDesc()"
//==================================================================================
// CWrapDP8Peer::DP8P_SetApplicationDesc
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPN_APPLICATION_DESC* pad	?
//	DWORD dwFlags				?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_SetApplicationDesc(const DPN_APPLICATION_DESC * const pad,
													const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->SetApplicationDesc(pad, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_SetApplicationDesc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_CreateGroup()"
//==================================================================================
// CWrapDP8Peer::DP8P_CreateGroup
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPN_GROUP_INFO* pdpnGroupInfo	?
//	PVOID pvGroupContext			?
//	PVOID pvAsyncContext			?
//	DPNHANDLE* phAsyncHandle		?
//	DWORD dwFlags					?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_CreateGroup(const DPN_GROUP_INFO * const pdpnGroupInfo,
											PVOID const pvGroupContext,
											PVOID const pvAsyncContext,
											DPNHANDLE * const phAsyncHandle,
											const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->CreateGroup(pdpnGroupInfo, pvGroupContext,
									pvAsyncContext, phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_CreateGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_DestroyGroup()"
//==================================================================================
// CWrapDP8Peer::DP8P_DestroyGroup
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID idGroup				?
//	PVOID pvAsyncContext		?
//	DPNHANDLE * phAsyncHandle	?
//	DWORD dwFlags				?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_DestroyGroup(const DPNID idGroup,
											PVOID const pvAsyncContext,
											DPNHANDLE * const phAsyncHandle,
											const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->DestroyGroup(idGroup, pvAsyncContext,
									phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_DestroyGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_AddPlayerToGroup()"
//==================================================================================
// CWrapDP8Peer::DP8P_AddPlayerToGroup
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID idGroup				?
//	DPNID idClient				?
//	PVOID pvAsyncContext		?
//	DPNHANDLE * phAsyncHandle	?
//	DWORD dwFlags				?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_AddPlayerToGroup(const DPNID idGroup,
												const DPNID idClient,
												PVOID const pvAsyncContext,
												DPNHANDLE * const phAsyncHandle,
												const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->AddPlayerToGroup(idGroup, idClient, pvAsyncContext,
									phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_AddPlayerToGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_RemovePlayerFromGroup()"
//==================================================================================
// CWrapDP8Peer::DP8P_RemovePlayerFromGroup
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID idGroup				?
//	DPNID idClient				?
//	PVOID pvAsyncContext		?
//	DPNHANDLE * phAsyncHandle	?
//	DWORD dwFlags				?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_RemovePlayerFromGroup(const DPNID idGroup,
													const DPNID idClient,
													PVOID const pvAsyncContext,
													DPNHANDLE * const phAsyncHandle,
													const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->RemovePlayerFromGroup(idGroup, idClient, pvAsyncContext,
											phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_RemovePlayerFromGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_SetGroupInfo()"
//==================================================================================
// CWrapDP8Peer::DP8P_SetGroupInfo
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID dpnid						?
//	DPN_GROUP_INFO* pdpnGroupInfo	?
//	PVOID pvAsyncContext			?
//	DPNHANDLE * phAsyncHandle		?
//	DWORD dwFlags					?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_SetGroupInfo(const DPNID dpnid,
											DPN_GROUP_INFO * const pdpnGroupInfo,
											PVOID const pvAsyncContext,
											DPNHANDLE * const phAsyncHandle,
											const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->SetGroupInfo(dpnid, pdpnGroupInfo, pvAsyncContext,
									phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_SetGroupInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_GetGroupInfo()"
//==================================================================================
// CWrapDP8Peer::DP8P_GetGroupInfo
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID dpnid						?
//	DPN_GROUP_INFO* pdpnGroupInfo	?
//	DWORD* pdwSize					?
//	DWORD dwFlags					?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_GetGroupInfo(const DPNID dpnid,
											DPN_GROUP_INFO * const pdpnGroupInfo,
											DWORD * const pdwSize,
											const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->GetGroupInfo(dpnid, pdpnGroupInfo, pdwSize, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_GetGroupInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_EnumPlayersAndGroups()"
//==================================================================================
// CWrapDP8Peer::DP8P_EnumPlayersAndGroups
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID* prgdpid	?
//	DWORD* pcdpid	?
//	DWORD dwFlags	?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_EnumPlayersAndGroups(DPNID * const prgdpid,
													DWORD * const pcdpid,
													const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->EnumPlayersAndGroups(prgdpid, pcdpid, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_EnumPlayersAndGroups
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_EnumGroupMembers()"
//==================================================================================
// CWrapDP8Peer::DP8P_EnumGroupMembers
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID dpid		?
//	DPNID* prgdpid	?
//	DWORD* pcdpid	?
//	DWORD dwFlags	?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_EnumGroupMembers(const DPNID dpid,
												DPNID * const prgdpid,
												DWORD * const pcdpid,
												const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->EnumGroupMembers(dpid, prgdpid, pcdpid, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_EnumGroupMembers
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_SetPeerInfo()"
//==================================================================================
// CWrapDP8Peer::DP8P_SetPeerInfo
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPN_PLAYER_INFO* pdpnGroupInfo	?
//	PVOID pvAsyncContext			?
//	DPNHANDLE * phAsyncHandle		?
//	DWORD dwFlags					?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_SetPeerInfo(const DPN_PLAYER_INFO * const pdpnPlayerInfo,
											PVOID const pvAsyncContext,
											DPNHANDLE * const phAsyncHandle,
											const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->SetPeerInfo(pdpnPlayerInfo, pvAsyncContext,
									phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_SetPeerInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_GetPeerInfo()"
//==================================================================================
// CWrapDP8Peer::DP8P_GetPeerInfo
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID dpnid							?
//	DPN_PLAYER_INFO* pdpnPlayerInfo		?
//	DWORD* pdwSize						?
//	DWORD dwFlags						?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_GetPeerInfo(const DPNID dpnid,
											DPN_PLAYER_INFO * const pdpnPlayerInfo,
											DWORD * const pdwSize,
											const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->GetPeerInfo(dpnid, pdpnPlayerInfo, pdwSize, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

#endif

	return (hr);
} // CWrapDP8Peer::DP8P_GetPeerInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_GetPeerAddress()"
//==================================================================================
// CWrapDP8Peer::DP8P_GetPeerAddress
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID dpnid							ID of player whose address should be
//										retrieved
//	IDirectPlay8Address ** pAddress		Place to store address object of player.
//	DWORD dwFlags						Unused.
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_GetPeerAddress(const DPNID dpnid,
												IDirectPlay8Address ** const pAddress,
												const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->GetPeerAddress(dpnid, pAddress, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_GetPeerAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_GetLocalHostAddresses()"
//==================================================================================
// CWrapDP8Peer::DP8P_GetLocalHostAddresses
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	IDirectPlay8Address** prgpAddress	?
//	DWORD* pcAddress					?
//	DWORD dwFlags						?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_GetLocalHostAddresses(IDirectPlay8Address ** const prgpAddress,
													DWORD * const pcAddress,
													const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->GetLocalHostAddresses(prgpAddress, pcAddress, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_GetLocalHostAddresses
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_Close()"
//==================================================================================
// CWrapDP8Peer::DP8P_Close
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DWORD dwFlags	?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_Close(const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	if (this->m_pDP8Peer == NULL)
	{
		xLog(m_hLog, XLL_WARN, "Can't close DirectPlay8Peer object because one doesn't exist!");
		hr = ERROR_BAD_ENVIRONMENT;
	} // end if (no real object)
	else
	{
		hr = this->m_pDP8Peer->Close(dwFlags);

		if (hr == DPN_OK)
		{
			this->m_fInitialized = FALSE;
		} // end if (successfully closed)
	} // end else (real object)
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_Close
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_EnumHosts()"
//==================================================================================
// CWrapDP8Peer::DP8P_EnumHosts
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	PDPN_APPLICATION_DESC pApplicationDesc	?
//	IDirectPlay8Address* pAddrHost			?
//	IDirectPlay8Address* pDeviceInfo		?
//	PVOID pUserEnumData						?
//	DWORD dwUserEnumDataSize				?
//	DWORD dwEnumCount						?
//	DWORD dwRetryInterval					?
//	DWORD dwTimeOut							?
//	PVOID pvUserContext						?
//	DPNHANDLE* pAsyncHandle					?
//	DWORD dwFlags							?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_EnumHosts(PDPN_APPLICATION_DESC const pApplicationDesc,
										IDirectPlay8Address * const pAddrHost,
										IDirectPlay8Address * const pDeviceInfo,
										PVOID const pUserEnumData,
										const DWORD dwUserEnumDataSize,
										const DWORD dwEnumCount,
										const DWORD dwRetryInterval,
										const DWORD dwTimeOut,
										PVOID const pvUserContext,
										DPNHANDLE * const pAsyncHandle,
										const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->EnumHosts(pApplicationDesc, pAddrHost, pDeviceInfo,
								pUserEnumData, dwUserEnumDataSize,
								dwEnumCount, dwRetryInterval, dwTimeOut,
								pvUserContext, pAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_EnumHosts
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_DestroyPeer()"
//==================================================================================
// CWrapDP8Peer::DP8P_DestroyPeer
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID dpidClient			?
//	BYTE* pDestroyInfo			?
//	DWORD dwDestroyInfoSize		?
//	DWORD dwFlags				?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_DestroyPeer(const DPNID dpidClient,
											const void * const pDestroyInfo,
											const DWORD dwDestroyInfoSize,
											const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->DestroyPeer(dpidClient,
									pDestroyInfo, dwDestroyInfoSize,
									dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_DestroyPeer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_ReturnBuffer()"
//==================================================================================
// CWrapDP8Peer::DP8P_ReturnBuffer
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNHANDLE hBufferHandle	?
//	DWORD dwFlags			?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_ReturnBuffer(const DPNHANDLE hBufferHandle,
											const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->ReturnBuffer(hBufferHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_ReturnBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_GetPlayerContext()"
//==================================================================================
// CWrapDP8Peer::DP8P_GetPlayerContext
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID dpnid					?
//	PVOID* ppvPlayerContext		?
//	DWORD dwFlags				?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_GetPlayerContext(const DPNID dpnid,
												PVOID * const ppvPlayerContext,
												const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->GetPlayerContext(dpnid, ppvPlayerContext, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_GetPlayerContext
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_GetGroupContext()"
//==================================================================================
// CWrapDP8Peer::DP8P_GetGroupContext
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID dpnid				?
//	PVOID* ppvGroupContext	?
//	DWORD dwFlags			?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_GetGroupContext(const DPNID dpnid,
												PVOID * const ppvGroupContext,
												const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->GetGroupContext(dpnid, ppvGroupContext, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_GetGroupContext
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_GetCaps()"
//==================================================================================
// CWrapDP8Peer::DP8P_GetCaps
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPN_CAPS* pdpCaps	?
//	DWORD dwFlags		?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_GetCaps(DPN_CAPS * const pdpCaps, const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->GetCaps(pdpCaps, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_GetCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_SetCaps()"
//==================================================================================
// CWrapDP8Peer::DP8P_SetCaps
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPN_CAPS* pdpCaps	?
//	DWORD dwFlags		?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_SetCaps(const DPN_CAPS * const pdpCaps,
										const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->SetCaps(pdpCaps, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_SetCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_SetSPCaps()"
//==================================================================================
// CWrapDP8Peer::DP8P_SetSPCaps
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	GUID* pguidSP			?
//	DPN_SP_CAPS* pdpspCaps	?
//	DWORD dwFlags			?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_SetSPCaps(const GUID * const pguidSP,
										const DPN_SP_CAPS * const pdpspCaps,
										const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->SetSPCaps(pguidSP, pdpspCaps, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_SetSPCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_GetSPCaps()"
//==================================================================================
// CWrapDP8Peer::DP8P_GetSPCaps
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	GUID* pguidSP			?
//	DPN_SP_CAPS* pdpspCaps	?
//	DWORD dwFlags			?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_GetSPCaps(const GUID * const pguidSP,
										DPN_SP_CAPS *const pdpspCaps,
										const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->GetSPCaps(pguidSP, pdpspCaps, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_GetSPCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_GetConnectionInfo()"
//==================================================================================
// CWrapDP8Peer::DP8P_GetConnectionInfo
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID dpnid								?
//	DPN_CONNECTION_INFO* pdpConnectionInfo	?
//	DWORD dwFlags							?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_GetConnectionInfo(const DPNID dpnid,
												DPN_CONNECTION_INFO * const pdpConnectionInfo,
												const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->GetConnectionInfo(dpnid, pdpConnectionInfo, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_GetConnectionInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Peer::DP8P_TerminateSession()"
//==================================================================================
// CWrapDP8Peer::DP8P_TerminateSession
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	void* pvTerminateData		?
//	DWORD dwTerminateDataSize	?
//	DWORD dwFlags				?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Peer::DP8P_TerminateSession(void * const pvTerminateData,
												const DWORD dwTerminateDataSize,
												const DWORD dwFlags)
{
	HRESULT		hr = DPN_OK;

#ifndef REMOVE_DPLAY_IMPORTS
	hr = this->m_pDP8Peer->TerminateSession(pvTerminateData, dwTerminateDataSize,
										dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Peer->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
#endif

	return (hr);
} // CWrapDP8Peer::DP8P_TerminateSession
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
