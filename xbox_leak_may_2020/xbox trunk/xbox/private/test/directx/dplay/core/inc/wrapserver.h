#ifndef __DNETRSLT_WRAPSERVER__
#define __DNETRSLT_WRAPSERVER__

//==================================================================================
// Classes
//==================================================================================
class CWrapDP8Server
{
	private:
		HANDLE				m_hLog;
		BOOL				m_fInitialized; // has Initialize been successfully called?
		//BOOL				m_fConnected; // has Connect been successfully called?


	public:
		PDIRECTPLAY8SERVER	m_pDP8Server; // pointer to real DirectPlay8Server object


		CWrapDP8Server(HANDLE hLog);
		virtual ~CWrapDP8Server(void);


		// DirectPlay8Server object maintenance functions
		HRESULT CoCreate(void);
		HRESULT Release(void);


		// IDirectPlay8Server methods
		STDMETHODIMP DP8S_Initialize(PVOID const pvUserContext,
									const PFNDPNMESSAGEHANDLER pfn,
									const DWORD dwFlags);

		STDMETHODIMP DP8S_DoWork(const DWORD dwFlags);

		STDMETHODIMP DP8S_EnumServiceProviders(const GUID * const pguidServiceProvider,
												const GUID * const pguidApplication,
												DPN_SERVICE_PROVIDER_INFO * const pSPInfoBuffer,
												DWORD * const pcbEnumData,
												DWORD * const pcReturned,
												const DWORD dwFlags);

		STDMETHODIMP DP8S_EnumPlayersAndGroups(DPNID *const prgdpnid,
												DWORD *const pcdpnid,
												const DWORD dwFlags);

		STDMETHODIMP DP8S_CancelAsyncOperation(const DPNHANDLE hAsyncHandle,
												const DWORD dwFlags);

		STDMETHODIMP DP8S_GetSendQueueInfo(const DPNID dpnid,
											DWORD * const pdwNumMsgs,
											DWORD * const pdwNumBytes,
											const DWORD dwFlags);


		STDMETHODIMP DP8S_GetClientAddress(const DPNID dpnid,
										IDirectPlay8Address ** const pAddress,
										const DWORD dwFlags);

		STDMETHODIMP DP8S_GetClientInfo(const DPNID dpnid,
												DPN_PLAYER_INFO *const pdpnPlayerInfo,
												DWORD *const pdwSize,
												const DWORD dwFlags);

		STDMETHODIMP DP8S_GetLocalHostAddresses(IDirectPlay8Address ** const prgpAddress,
												DWORD * const pcAddress,
												const DWORD dwFlags);

		STDMETHODIMP DP8S_GetConnectionInfo(const DPNID dpnidEndPoint,
													DPN_CONNECTION_INFO *const pdnConnectInfo,
													const DWORD dwFlags);

		STDMETHODIMP DP8S_GetApplicationDesc(DPN_APPLICATION_DESC *const pAppDescBuffer,
													 DWORD *const pcbDataSize,
													 const DWORD dwFlags);

		STDMETHODIMP DP8S_SetApplicationDesc(const DPN_APPLICATION_DESC * const pad,
											const DWORD dwFlags);

		STDMETHODIMP DP8S_SetServerInfo(const DPN_PLAYER_INFO *const pdpnPlayerInfo,
													 PVOID const pvAsyncContext,
													 DPNHANDLE *const phAsyncHandle,
													 const DWORD dwFlags);

		STDMETHODIMP DP8S_SetSPCaps(const GUID *const pguidSP,
											const DPN_SP_CAPS *const pdpSPCaps,
											const DWORD dwFlags);

		STDMETHODIMP DP8S_GetSPCaps(const GUID *const pguidSP,
											DPN_SP_CAPS *const pdpnSPCaps,
											const DWORD dwFlags);

		STDMETHODIMP DP8S_GetCaps(DPN_CAPS * const pdpCaps, const DWORD dwFlags);
		
		STDMETHODIMP DP8S_SetCaps(const DPN_CAPS * const pdpCaps, const DWORD dwFlags);

		STDMETHODIMP DP8S_Host(const DPN_APPLICATION_DESC * const pdnAppDesc,
								IDirectPlay8Address ** const prgpDeviceInfo,
								const DWORD cDeviceInfo,
								const DPN_SECURITY_DESC * const pdnSecurity,
								const DPN_SECURITY_CREDENTIALS * const pdnCredentials,
								void * const pvPlayerContext,
								const DWORD dwFlags);

		STDMETHODIMP DP8S_SendTo(const DPNID dpnid,
								const DPN_BUFFER_DESC * const prgBufferDesc,
								const DWORD cBufferDesc,
								const DWORD dwTimeOut,
								void * const pvAsyncContext,
								DPNHANDLE * const phAsyncHandle,
								const DWORD dwFlags);

		STDMETHODIMP DP8S_DestroyClient(const DPNID dpnidClient,
												const VOID *const pDestroyInfo,
												const DWORD dwDestroyInfoSize,
												const DWORD dwFlags);

		STDMETHODIMP DP8S_GetPlayerContext(const DPNID dpnid,
												PVOID * const ppvPlayerContext,
												const DWORD dwFlags);

		STDMETHODIMP DP8S_CreateGroup(const DPN_GROUP_INFO *const pdpnGroupInfo,
											  VOID *const pvGroupContext,VOID *const pvAsyncContext,
											  DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);

		STDMETHODIMP DP8S_AddPlayerToGroup(const DPNID idGroup, const DPNID idClient, PVOID const pvAsyncContext,
												   DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);

		STDMETHODIMP DP8S_Close(const DWORD dwFlags);



		// Returns whether this object has been initialized or not.
		inline BOOL IsInitialized(void) { return (this->m_fInitialized); };
};

//==================================================================================
// Class type definitions
//==================================================================================
typedef class CWrapDP8Server			CWrapDP8Server,			* PWRAPDP8SERVER;

#endif //__DNETRSLT_WRAPSERVER__
