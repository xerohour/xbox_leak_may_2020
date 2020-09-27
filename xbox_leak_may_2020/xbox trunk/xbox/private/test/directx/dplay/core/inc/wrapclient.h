#ifndef __DNETRSLT_WRAPCLIENT__
#define __DNETRSLT_WRAPCLIENT__

//==================================================================================
// Classes
//==================================================================================
class CWrapDP8Client
{
	private:
		HANDLE				m_hLog;
		BOOL				m_fInitialized; // has Initialize been successfully called?
		//BOOL				m_fConnected; // has Connect been successfully called?


	public:
		PDIRECTPLAY8CLIENT	m_pDP8Client; // pointer to real DirectPlay8Client object


		CWrapDP8Client(HANDLE hLog);
		virtual ~CWrapDP8Client(void);


		// DirectPlay8Client object maintenance functions
		HRESULT CoCreate(void);
		HRESULT Release(void);


		// IDirectPlay8Client methods
		STDMETHODIMP DP8C_Initialize(PVOID const pvUserContext,
									const PFNDPNMESSAGEHANDLER pfn,
									const DWORD dwFlags);

		STDMETHODIMP DP8C_DoWork(const DWORD dwFlags);

		STDMETHODIMP DP8C_EnumServiceProviders(const GUID * const pguidServiceProvider,
												const GUID * const pguidApplication,
												DPN_SERVICE_PROVIDER_INFO * const pSPInfoBuffer,
												DWORD * const pcbEnumData,
												DWORD * const pcReturned,
												const DWORD dwFlags);

		STDMETHODIMP DP8C_EnumHosts(PDPN_APPLICATION_DESC const pApplicationDesc,
									IDirectPlay8Address * const pAddrHost,
									IDirectPlay8Address * const pDeviceInfo,
									PVOID const pUserEnumData,
									const DWORD dwUserEnumDataSize,
									const DWORD dwEnumCount,
									const DWORD dwRetryInterval,
									const DWORD dwTimeOut,
									PVOID const pvUserContext,
									DPNHANDLE * const pAsyncHandle,
									const DWORD dwFlags);

		STDMETHODIMP DP8C_CancelAsyncOperation(const DPNHANDLE hAsyncHandle,
												const DWORD dwFlags);

		STDMETHODIMP DP8C_Connect(const DPN_APPLICATION_DESC * const pdnAppDesc,
								IDirectPlay8Address * const pHostAddr,
								IDirectPlay8Address * const pDeviceInfo,
								const DPN_SECURITY_DESC * const pdnSecurity,
								const DPN_SECURITY_CREDENTIALS * const pdnCredentials,
								const void * const pvUserConnectData,
								const DWORD dwUserConnectDataSize,
								void * const pvAsyncContext,
								DPNHANDLE * const phAsyncHandle,
								const DWORD dwFlags);


		STDMETHODIMP DP8C_GetSendQueueInfo(DWORD * const pdwNumMsgs,
											DWORD * const pdwNumBytes,
											const DWORD dwFlags);


		STDMETHODIMP DP8C_Send(const DPN_BUFFER_DESC *const pBufferDesc,
									   const DWORD cBufferDesc, const DWORD dwTimeOut,
									   void *const pvAsyncContext, DPNHANDLE *const phAsyncHandle,
									   const DWORD dwFlags);

		STDMETHODIMP DP8C_SetClientInfo(const DPN_PLAYER_INFO *const pdpnPlayerInfo,
													 PVOID const pvAsyncContext,
													 DPNHANDLE *const phAsyncHandle,
													 const DWORD dwFlags);

		STDMETHODIMP DP8C_SetSPCaps(const GUID *const pguidSP,
											const DPN_SP_CAPS *const pdpSPCaps,
											const DWORD dwFlags);

		STDMETHODIMP DP8C_GetCaps(DPN_CAPS * const pdpCaps, const DWORD dwFlags);
		
		STDMETHODIMP DP8C_SetCaps(const DPN_CAPS * const pdpCaps, const DWORD dwFlags);

		STDMETHODIMP DP8C_GetServerInfo(DPN_PLAYER_INFO *const pdpnPlayerInfo,
												DWORD *const pdwSize,
												const DWORD dwFlags);

		STDMETHODIMP DP8C_GetConnectionInfo(DPN_CONNECTION_INFO *const pdnConnectInfo,
													const DWORD dwFlags);

		STDMETHODIMP DP8C_GetApplicationDesc(DPN_APPLICATION_DESC *const pAppDescBuffer,
													 DWORD *const pcbDataSize,
													 const DWORD dwFlags);

		STDMETHODIMP DP8C_GetSPCaps(const GUID *const pguidSP,
											DPN_SP_CAPS *const pdpnSPCaps,
											const DWORD dwFlags);

		STDMETHODIMP DP8C_GetServerAddress(IDirectPlay8Address ** const pAddress,
											const DWORD dwFlags);

		STDMETHODIMP DP8C_Close(const DWORD dwFlags);



		// Returns whether this object has been initialized or not.
		inline BOOL IsInitialized(void) { return (this->m_fInitialized); };
};

//==================================================================================
// Class type definitions
//==================================================================================
typedef class CWrapDP8Client			CWrapDP8Client,			* PWRAPDP8CLIENT;

#endif //__DNETRSLT_WRAPCLIENT__
