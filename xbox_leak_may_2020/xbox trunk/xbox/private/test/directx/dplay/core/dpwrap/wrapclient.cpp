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
#include "wrapclient.h"

#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::CWrapDP8Client()"
//==================================================================================
// CWrapDP8Client constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CWrapDP8Client object.
//
// Arguments: None.
//
// Returns: None (the object).
//==================================================================================
CWrapDP8Client::CWrapDP8Client(HANDLE hLog):
	m_pDP8Client(NULL),
	m_hLog(hLog),
	m_fInitialized(FALSE)
{
} // CWrapDP8Client::CWrapDP8Client
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::~CWrapDP8Client()"
//==================================================================================
// CWrapDP8Client destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CWrapDP8Client object.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CWrapDP8Client::~CWrapDP8Client(void)
{
	HRESULT			hr;

//#pragma BUGBUG(vanceo, "make these an option (graceful or un)")

	if (this->m_fInitialized)
	{
		hr = this->DP8C_Close(0);
		if (hr != DPN_OK)
		{
			xLog(m_hLog, XLL_WARN, "Failed closing!  0x%08x", hr);
		} // end if (failed closing)
	} // end if (initialized object)

	if (this->m_pDP8Client != NULL)
	{
		hr = this->Release();
		if (hr != S_OK)
		{
			xLog(m_hLog, XLL_WARN, "Failed releasing DirectPlay8Client object!  0x%08x", hr);
		} // end if (failed releasing object)
	} // end if (we have an object pointer)
} // CWrapDP8Client::~CWrapDP8Client
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::CoCreate()"
//==================================================================================
// CWrapDP8Client::CoCreate
//----------------------------------------------------------------------------------
//
// Description: Calls CoCreateInstance for a new DirectPlay8Client object.
//
// Arguments: None.
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
HRESULT CWrapDP8Client::CoCreate(void)
{
	HRESULT		hr;


	if (this->m_pDP8Client != NULL)
	{
		xLog(m_hLog, XLL_WARN, "Can't create a DirectPlay8Client object because one already exists (%x)!",
			this->m_pDP8Client);
		hr = ERROR_ALREADY_EXISTS;
	} // end if (already have an object)
	else
	{
		hr = DirectPlay8Create(IID_IDirectPlay8Client, (LPVOID *) &this->m_pDP8Client, NULL);
	} // end else (don't have an object)

	return (hr);
} // CWrapDP8Client::CoCreate
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::Release()"
//==================================================================================
// CWrapDP8Client::Release
//----------------------------------------------------------------------------------
//
// Description: Releases the DirectPlay8Client object.
//
// Arguments: None.
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
HRESULT CWrapDP8Client::Release(void)
{
	HRESULT		hr = DPN_OK;

	if (this->m_pDP8Client == NULL)
	{
		xLog(m_hLog, XLL_WARN, "Can't release DirectPlay8Client object because one doesn't exist!");
		hr = ERROR_BAD_ENVIRONMENT;
	} // end if (no object yet)
	else
	{
		this->m_pDP8Client->Release();
		this->m_pDP8Client = NULL;
		
		this->m_fInitialized = FALSE;
	} // end else (there's an object)

	return (hr);
} // CWrapDP8Client::Release
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_Initialize()"
//==================================================================================
// CWrapDP8Client::DP8C_Initialize
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
STDMETHODIMP CWrapDP8Client::DP8C_Initialize(PVOID const pvUserContext,
											const PFNDPNMESSAGEHANDLER pfn,
											const DWORD dwFlags)
{
	HRESULT		hr;
	
	hr = this->m_pDP8Client->Initialize(pvUserContext, pfn, dwFlags);

	if (hr == DPN_OK)
	{
		this->m_fInitialized = TRUE;
	} // end if (successfully initialized object)

	return (hr);
} // CWrapDP8Client::DP8C_Initialize
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DoWork()"
//==================================================================================
// CWrapDP8Client::DP8C_DoWork
//----------------------------------------------------------------------------------
//
// Description: Pump function that causes DirectPlay to do some processing
//
// Arguments:
//	DWORD dwFlags				?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Client::DP8C_DoWork(const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->DoWork(dwFlags);

	return (hr);
} // CWrapDP8Client::DP8C_DoWork
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_EnumServiceProviders()"
//==================================================================================
// CWrapDP8Client::DP8C_EnumServiceProviders
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
STDMETHODIMP CWrapDP8Client::DP8C_EnumServiceProviders(const GUID * const pguidServiceProvider,
														const GUID * const pguidApplication,
														DPN_SERVICE_PROVIDER_INFO * const pSPInfoBuffer,
														DWORD * const pcbEnumData,
														DWORD * const pcReturned,
														const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->EnumServiceProviders(pguidServiceProvider, pguidApplication, pSPInfoBuffer, pcbEnumData,
		pcReturned, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}
	
	return (hr);
} // CWrapDP8Client::DP8C_EnumServiceProviders
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_EnumHosts()"
//==================================================================================
// CWrapDP8Client::DP8C_EnumHosts
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
STDMETHODIMP CWrapDP8Client::DP8C_EnumHosts(PDPN_APPLICATION_DESC const pApplicationDesc,
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
	HRESULT		hr;

	hr = this->m_pDP8Client->EnumHosts(pApplicationDesc, pAddrHost, pDeviceInfo, pUserEnumData, dwUserEnumDataSize,
								dwEnumCount, dwRetryInterval, dwTimeOut, pvUserContext, pAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::DP8C_EnumHosts
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_CancelAsyncOperation()"
//==================================================================================
// CWrapDP8Client::DP8C_CancelAsyncOperation
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
STDMETHODIMP CWrapDP8Client::DP8C_CancelAsyncOperation(const DPNHANDLE hAsyncHandle,
														const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->CancelAsyncOperation(hAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::DP8C_CancelAsyncOperation
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_Connect()"
//==================================================================================
// CWrapDP8Client::DP8C_Connect
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
//	void* pvAsyncContext						?
//	DPNHANDLE* phAsyncHandle					?
//	DWORD dwFlags								?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Client::DP8C_Connect(const DPN_APPLICATION_DESC * const pdnAppDesc,
										IDirectPlay8Address * const pHostAddr,
										IDirectPlay8Address * const pDeviceInfo,
										const DPN_SECURITY_DESC * const pdnSecurity,
										const DPN_SECURITY_CREDENTIALS * const pdnCredentials,
										const void * const pvUserConnectData,
										const DWORD dwUserConnectDataSize,
										void * const pvAsyncContext,
										DPNHANDLE * const phAsyncHandle,
										const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->Connect(pdnAppDesc, pHostAddr, pDeviceInfo, pdnSecurity, pdnCredentials, pvUserConnectData,
								dwUserConnectDataSize, pvAsyncContext, phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::DP8C_Connect
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_GetSendQueueInfo()"
//==================================================================================
// CWrapDP8Client::DP8C_GetSendQueueInfo
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DWORD* pdwNumMsgs	?
//	DWORD* pdwNumBytes	?
//	DWORD dwFlags		?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Client::DP8C_GetSendQueueInfo(DWORD * const pdwNumMsgs,
												DWORD * const pdwNumBytes,
												const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->GetSendQueueInfo(pdwNumMsgs, pdwNumBytes, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::DP8C_GetSendQueueInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_Send()"
//==================================================================================
// CWrapDP8Client::DP8C_Send
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DWORD* pdwNumMsgs	?
//	DWORD* pdwNumBytes	?
//	DWORD dwFlags		?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Client::DP8C_Send(const DPN_BUFFER_DESC *const pBufferDesc,
									   const DWORD cBufferDesc, const DWORD dwTimeOut,
									   void *const pvAsyncContext, DPNHANDLE *const phAsyncHandle,
									   const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->Send(pBufferDesc, cBufferDesc, dwTimeOut, pvAsyncContext,
		phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::DP8C_Send
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_SetClientInfo()"
//==================================================================================
// CWrapDP8Client::DP8C_SetClientInfo
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
STDMETHODIMP CWrapDP8Client::DP8C_SetClientInfo(const DPN_PLAYER_INFO *const pdpnPlayerInfo,
													 PVOID const pvAsyncContext,
													 DPNHANDLE *const phAsyncHandle,
													 const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->SetClientInfo(pdpnPlayerInfo, pvAsyncContext, phAsyncHandle, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::DP8C_SetClientInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_SetSPCaps()"
//==================================================================================
// CWrapDP8Client::DP8C_SetSPCaps
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Client::DP8C_SetSPCaps(const GUID *const pguidSP,
											const DPN_SP_CAPS *const pdpSPCaps,
											const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->SetSPCaps(pguidSP, pdpSPCaps, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::DP8C_SetSPCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_GetCaps()"
//==================================================================================
// CWrapDP8Client::DP8C_GetCaps
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
STDMETHODIMP CWrapDP8Client::DP8C_GetCaps(DPN_CAPS * const pdpCaps, const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->GetCaps(pdpCaps, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::DP8C_GetCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_SetCaps()"
//==================================================================================
// CWrapDP8Client::DP8C_SetCaps
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
STDMETHODIMP CWrapDP8Client::DP8C_SetCaps(const DPN_CAPS * const pdpCaps,
										const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->SetCaps(pdpCaps, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::DP8C_SetCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_GetServerInfo()"
//==================================================================================
// CWrapDP8Client::DP8C_GetServerInfo
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
STDMETHODIMP CWrapDP8Client::DP8C_GetServerInfo(DPN_PLAYER_INFO *const pdpnPlayerInfo,
												DWORD *const pdwSize,
												const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->GetServerInfo(pdpnPlayerInfo, pdwSize, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::DP8C_GetServerInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_GetConnectionInfo()"
//==================================================================================
// CWrapDP8Client::DP8C_GetConnectionInfo
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
STDMETHODIMP CWrapDP8Client::DP8C_GetConnectionInfo(DPN_CONNECTION_INFO *const pdnConnectInfo,
													const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->GetConnectionInfo(pdnConnectInfo, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::GetConnectionInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_GetApplicationDesc()"
//==================================================================================
// CWrapDP8Client::DP8C_GetApplicationDesc
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
STDMETHODIMP CWrapDP8Client::DP8C_GetApplicationDesc(DPN_APPLICATION_DESC *const pAppDescBuffer,
													 DWORD *const pcbDataSize,
													 const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->GetApplicationDesc(pAppDescBuffer, pcbDataSize, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::GetApplicationDesc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_GetServerAddress()"
//==================================================================================
// CWrapDP8Client::DP8C_GetServerAddress
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	IDirectPlay8Address ** pAddress		?
//	DWORD dwFlags						?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Client::DP8C_GetServerAddress(IDirectPlay8Address ** const pAddress,
													const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->GetServerAddress(pAddress, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::DP8C_GetServerAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_GetSPCaps()"
//==================================================================================
// CWrapDP8Client::DP8C_GetSPCaps
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Client::DP8C_GetSPCaps(const GUID *const pguidSP,
											DPN_SP_CAPS *const pdpnSPCaps,
											const DWORD dwFlags)
{
	HRESULT		hr;

	hr = this->m_pDP8Client->GetSPCaps(pguidSP, pdpnSPCaps, dwFlags);

	DWORD dwTemp = 0;
	while(dwTemp < 50)
	{
		++dwTemp;
		if(this->m_pDP8Client->DoWork(0) == S_FALSE)
			break;
	}

	if(dwTemp >= 50)
	{
//		DbgPrint("DoWork didn't return S_FALSE after %u calls", dwTemp);
		hr = S_FALSE;
	}

	return (hr);
} // CWrapDP8Client::DP8C_GetSPCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Client::DP8C_Close()"
//==================================================================================
// CWrapDP8Client::DP8C_Close
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	DWORD dwFlags	?
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Client::DP8C_Close(const DWORD dwFlags)
{
	HRESULT		hr;

	if (this->m_pDP8Client == NULL)
	{
		xLog(m_hLog, XLL_WARN, "Can't close DirectPlay8Client object because one doesn't exist!");
		hr = ERROR_BAD_ENVIRONMENT;
	} // end if (no real object)
	else
	{
		hr = this->m_pDP8Client->Close(dwFlags);

		if (hr == DPN_OK)
		{
			this->m_fInitialized = FALSE;
		} // end if (successfully closed)
	} // end else (real object)

	return (hr);
} // CWrapDP8Client::DP8C_Close
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
