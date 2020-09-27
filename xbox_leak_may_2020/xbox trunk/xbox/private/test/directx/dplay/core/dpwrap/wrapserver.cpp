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
#include "wrapserver.h"

#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::CWrapDP8Server()"
//==================================================================================
// CWrapDP8Server constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CWrapDP8Server object.
//
// Arguments: None.
//
// Returns: None (the object).
//==================================================================================
CWrapDP8Server::CWrapDP8Server(HANDLE hLog):
	m_pDP8Server(NULL),
	m_hLog(hLog),
	m_fInitialized(FALSE)
{
} // CWrapDP8Server::CWrapDP8Server
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::~CWrapDP8Server()"
//==================================================================================
// CWrapDP8Server destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CWrapDP8Server object.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CWrapDP8Server::~CWrapDP8Server(void)
{
	HRESULT			hr;

//#pragma BUGBUG(vanceo, "make these an option (graceful or un)")

	if (this->m_fInitialized)
	{
		hr = this->DP8S_Close(0);
		if (hr != DPN_OK)
		{
			xLog(m_hLog, XLL_WARN, "Failed closing!  0x%08x", hr);
		} // end if (failed closing)
	} // end if (initialized object)

	if (this->m_pDP8Server != NULL)
	{
		hr = this->Release();
		if (hr != S_OK)
		{
			xLog(m_hLog, XLL_WARN, "Failed releasing DirectPlay8Server object!  0x%08x", hr);
		} // end if (failed releasing object)
	} // end if (we have an object pointer)
} // CWrapDP8Server::~CWrapDP8Server
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::CoCreate()"
//==================================================================================
// CWrapDP8Server::CoCreate
//----------------------------------------------------------------------------------
//
// Description: Calls CoCreateInstance for a new DirectPlay8Server object.
//
// Arguments: None.
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
HRESULT CWrapDP8Server::CoCreate(void)
{
	HRESULT		hr;

	if (this->m_pDP8Server != NULL)
	{
		xLog(m_hLog, XLL_WARN, "Can't create a DirectPlay8Server object because one already exists (%x)!",
			this->m_pDP8Server);
		hr = ERROR_ALREADY_EXISTS;
	} // end if (already have an object)
	else
	{
		hr = DirectPlay8Create(IID_IDirectPlay8Server, (LPVOID *) &this->m_pDP8Server, NULL);
	} // end else (don't have an object)

	return (hr);
} // CWrapDP8Server::CoCreate
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::Release()"
//==================================================================================
// CWrapDP8Server::Release
//----------------------------------------------------------------------------------
//
// Description: Releases the DirectPlay8Server object.
//
// Arguments: None.
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
HRESULT CWrapDP8Server::Release(void)
{
	HRESULT		hr = DPN_OK;

	if (this->m_pDP8Server == NULL)
	{
		xLog(m_hLog, XLL_WARN, "Can't release DirectPlay8Server object because one doesn't exist!");
		hr = ERROR_BAD_ENVIRONMENT;
	} // end if (no object yet)
	else
	{
		this->m_pDP8Server->Release();
		this->m_pDP8Server = NULL;
		
		this->m_fInitialized = FALSE;
	} // end else (there's an object)

	return (hr);
} // CWrapDP8Server::Release
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_Initialize()"
//==================================================================================
// CWrapDP8Server::DP8S_Initialize
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
STDMETHODIMP CWrapDP8Server::DP8S_Initialize(PVOID const pvUserContext,
											const PFNDPNMESSAGEHANDLER pfn,
											const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->Initialize(pvUserContext, pfn, dwFlags);

	if (hr == DPN_OK)
	{
		this->m_fInitialized = TRUE;
	} // end if (successfully initialized object)

	return (hr);
} // CWrapDP8Server::DP8S_Initialize
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DoWork()"
//==================================================================================
// CWrapDP8Server::DP8S_DoWork
//----------------------------------------------------------------------------------
//
// Description: Pump function that causes DirectPlay to do some processing
//
// Arguments:
//	DWORD dwFlags				?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Server::DP8S_DoWork(const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->DoWork(dwFlags);

	return (hr);
} // CWrapDP8Server::DP8S_DoWork
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_EnumServiceProviders()"
//==================================================================================
// CWrapDP8Server::DP8S_EnumServiceProviders
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
STDMETHODIMP CWrapDP8Server::DP8S_EnumServiceProviders(const GUID * const pguidServiceProvider,
													const GUID * const pguidApplication,
													DPN_SERVICE_PROVIDER_INFO * const pSPInfoBuffer,
													DWORD * const pcbEnumData,
													DWORD * const pcReturned,
													const DWORD dwFlags)
{
	HRESULT		hr;
	
	hr = this->m_pDP8Server->EnumServiceProviders(pguidServiceProvider,
		pguidApplication,
		pSPInfoBuffer,
		pcbEnumData,
		pcReturned,
		dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_EnumServiceProviders
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


HRESULT EnumPlayersAndGroups(
DPNID *const prgdpnid,
DWORD *const pcdpnid,
const DWORD dwFlags
);
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_EnumPlayersAndGroups()"
//==================================================================================
// CWrapDP8Server::DP8S_EnumPlayersAndGroups
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
STDMETHODIMP CWrapDP8Server::DP8S_EnumPlayersAndGroups(DPNID *const prgdpnid,
													   DWORD *const pcdpnid,
													   const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->EnumPlayersAndGroups(prgdpnid, pcdpnid, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_EnumPlayersAndGroups
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_CancelAsyncOperation()"
//==================================================================================
// CWrapDP8Server::DP8S_CancelAsyncOperation
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
STDMETHODIMP CWrapDP8Server::DP8S_CancelAsyncOperation(const DPNHANDLE hAsyncHandle,
													const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->CancelAsyncOperation(hAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_CancelAsyncOperation
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_GetSendQueueInfo()"
//==================================================================================
// CWrapDP8Server::DP8S_GetSendQueueInfo
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
STDMETHODIMP CWrapDP8Server::DP8S_GetSendQueueInfo(const DPNID dpnid,
												DWORD * const pdwNumMsgs,
												DWORD * const pdwNumBytes,
												const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->GetSendQueueInfo(dpnid, pdwNumMsgs, pdwNumBytes, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_GetSendQueueInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_GetClientAddress()"
//==================================================================================
// CWrapDP8Server::DP8S_GetClientAddress
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
STDMETHODIMP CWrapDP8Server::DP8S_GetClientAddress(const DPNID dpnid,
												IDirectPlay8Address ** const pAddress,
												const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->GetClientAddress(dpnid, pAddress, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_GetClientAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_GetClientInfo()"
//==================================================================================
// CWrapDP8Server::DP8S_GetClientInfo
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
STDMETHODIMP CWrapDP8Server::DP8S_GetClientInfo(const DPNID dpnid,
												DPN_PLAYER_INFO *const pdpnPlayerInfo,
												DWORD *const pdwSize,
												const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->GetClientInfo(dpnid, pdpnPlayerInfo, pdwSize, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_GetClientInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_GetLocalHostAddresses()"
//==================================================================================
// CWrapDP8Server::DP8S_GetLocalHostAddresses
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
STDMETHODIMP CWrapDP8Server::DP8S_GetLocalHostAddresses(IDirectPlay8Address ** const prgpAddress,
														DWORD * const pcAddress,
														const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->GetLocalHostAddresses(prgpAddress, pcAddress, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_GetLocalHostAddresses
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_GetConnectionInfo()"
//==================================================================================
// CWrapDP8Server::DP8S_GetConnectionInfo
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
STDMETHODIMP CWrapDP8Server::DP8S_GetConnectionInfo(const DPNID dpnidEndPoint,
													DPN_CONNECTION_INFO *const pdnConnectInfo,
													const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->GetConnectionInfo(dpnidEndPoint, pdnConnectInfo, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::GetConnectionInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_GetApplicationDesc()"
//==================================================================================
// CWrapDP8Server::DP8S_GetApplicationDesc
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
STDMETHODIMP CWrapDP8Server::DP8S_GetApplicationDesc(DPN_APPLICATION_DESC *const pAppDescBuffer,
													 DWORD *const pcbDataSize,
													 const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->GetApplicationDesc(pAppDescBuffer, pcbDataSize, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::GetApplicationDesc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_SetApplicationDesc()"
//==================================================================================
// CWrapDP8Server::DP8S_SetApplicationDesc
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
STDMETHODIMP CWrapDP8Server::DP8S_SetApplicationDesc(const DPN_APPLICATION_DESC * const pad,
													const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->SetApplicationDesc(pad, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_SetApplicationDesc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_SetServerInfo()"
//==================================================================================
// CWrapDP8Server::DP8S_SetServerInfo
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
STDMETHODIMP CWrapDP8Server::DP8S_SetServerInfo(const DPN_PLAYER_INFO *const pdpnPlayerInfo,
													 PVOID const pvAsyncContext,
													 DPNHANDLE *const phAsyncHandle,
													 const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->SetServerInfo(pdpnPlayerInfo, pvAsyncContext, phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_SetServerInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_SetSPCaps()"
//==================================================================================
// CWrapDP8Server::DP8S_SetSPCaps
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Server::DP8S_SetSPCaps(const GUID *const pguidSP,
											const DPN_SP_CAPS *const pdpSPCaps,
											const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->SetSPCaps(pguidSP, pdpSPCaps, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_SetSPCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_GetSPCaps()"
//==================================================================================
// CWrapDP8Server::DP8S_GetSPCaps
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Server::DP8S_GetSPCaps(const GUID *const pguidSP,
											DPN_SP_CAPS *const pdpnSPCaps,
											const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->GetSPCaps(pguidSP, pdpnSPCaps, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_GetSPCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_GetCaps()"
//==================================================================================
// CWrapDP8Server::DP8S_GetCaps
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
STDMETHODIMP CWrapDP8Server::DP8S_GetCaps(DPN_CAPS * const pdpCaps, const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->GetCaps(pdpCaps, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_GetCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_SetCaps()"
//==================================================================================
// CWrapDP8Server::DP8S_SetCaps
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
STDMETHODIMP CWrapDP8Server::DP8S_SetCaps(const DPN_CAPS * const pdpCaps,
										const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->SetCaps(pdpCaps, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_SetCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_Host()"
//==================================================================================
// CWrapDP8Server::DP8S_Host
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
STDMETHODIMP CWrapDP8Server::DP8S_Host(const DPN_APPLICATION_DESC * const pdnAppDesc,
									IDirectPlay8Address ** const prgpDeviceInfo,
									const DWORD cDeviceInfo,
									const DPN_SECURITY_DESC * const pdnSecurity,
									const DPN_SECURITY_CREDENTIALS * const pdnCredentials,
									void * const pvPlayerContext,
									const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->Host(pdnAppDesc, prgpDeviceInfo, cDeviceInfo,
							pdnSecurity, pdnCredentials, pvPlayerContext,
							dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_Host
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_SendTo()"
//==================================================================================
// CWrapDP8Server::DP8S_SendTo
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
STDMETHODIMP CWrapDP8Server::DP8S_SendTo(const DPNID dpnid,
										const DPN_BUFFER_DESC * const prgBufferDesc,
										const DWORD cBufferDesc,
										const DWORD dwTimeOut,
										void * const pvAsyncContext,
										DPNHANDLE * const phAsyncHandle,
										const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->SendTo(dpnid, prgBufferDesc, cBufferDesc, dwTimeOut,
								pvAsyncContext, phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_SendTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_DestroyClient()"
//==================================================================================
// CWrapDP8Server::DP8S_DestroyClient
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
STDMETHODIMP CWrapDP8Server::DP8S_DestroyClient(const DPNID dpnidClient,
												const VOID *const pDestroyInfo,
												const DWORD dwDestroyInfoSize,
												const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->DestroyClient(dpnidClient, pDestroyInfo, dwDestroyInfoSize, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_DestroyClient
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_CreateGroup()"
//==================================================================================
// CWrapDP8Server::DP8S_CreateGroup
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPN_GROUP_INFO	*pdpnGroupInfo,
//	VOID			*pvGroupContext,
//	VOID			*pvAsyncContext,
//	DPNHANDLE		*phAsyncHandle,
//	DWORD			dwFlags
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Server::DP8S_CreateGroup(const DPN_GROUP_INFO *const pdpnGroupInfo,
											  VOID *const pvGroupContext,VOID *const pvAsyncContext,
											  DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->CreateGroup(pdpnGroupInfo, pvGroupContext, pvAsyncContext, phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_CreateGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_AddPlayerToGroup()"
//==================================================================================
// CWrapDP8Server::DP8S_AddPlayerToGroup
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DPNID idGroup,
//	DPNID idClient,
//	PVOID pvAsyncContext,
//	DPNHANDLE *phAsyncHandle,
//	DWORD dwFlags
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Server::DP8S_AddPlayerToGroup(const DPNID idGroup, const DPNID idClient, PVOID const pvAsyncContext,
												   DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->AddPlayerToGroup(idGroup, idClient, pvAsyncContext, phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_AddPlayerToGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_GetPlayerContext()"
//==================================================================================
// CWrapDP8Server::DP8S_GetPlayerContext
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
STDMETHODIMP CWrapDP8Server::DP8S_GetPlayerContext(const DPNID dpnid,
												PVOID * const ppvPlayerContext,
												const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Server->GetPlayerContext(dpnid, ppvPlayerContext, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Server->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Server::DP8S_GetPlayerContext
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Server::DP8S_Close()"
//==================================================================================
// CWrapDP8Server::DP8S_Close
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DWORD dwFlags	?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Server::DP8S_Close(const DWORD dwFlags)
{
	HRESULT		hr;
	
	if (this->m_pDP8Server == NULL)
	{
		xLog(m_hLog, XLL_WARN, "Can't close DirectPlay8Server object because one doesn't exist!");
		hr = ERROR_BAD_ENVIRONMENT;
	} // end if (no real object)
	else
	{
		hr = this->m_pDP8Server->Close(dwFlags);
		
		if (hr == DPN_OK)
		{
			this->m_fInitialized = FALSE;
		} // end if (successfully closed)
	} // end else (real object)

	return (hr);
} // CWrapDP8Server::DP8S_Close
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
