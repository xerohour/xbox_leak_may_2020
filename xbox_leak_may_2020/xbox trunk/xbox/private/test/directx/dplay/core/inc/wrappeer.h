#ifndef __DNETRSLT_WRAPPEER__
#define __DNETRSLT_WRAPPEER__

//==================================================================================
// Classes
//==================================================================================
class CWrapDP8Peer
{
	private:
		HANDLE				m_hLog;
		BOOL				m_fInitialized; // has Initialize been successfully called?
		//BOOL				m_fConnected; // has Connect been successfully called?


	public:
#ifdef REMOVE_DPLAY_IMPORTS
		LPVOID				m_pDP8Peer;
#else
		PDIRECTPLAY8PEER	m_pDP8Peer; // pointer to real DirectPlay8Peer object
#endif

		CWrapDP8Peer(HANDLE hLog);
		virtual ~CWrapDP8Peer(void);


		// DirectPlay8Peer object maintenance functions
		HRESULT CoCreate(void);
		HRESULT Release(void);


		// IDirectPlay8Peer methods
		STDMETHODIMP DP8P_Initialize(PVOID const pvUserContext,
									const PFNDPNMESSAGEHANDLER pfn,
									const DWORD dwFlags);

		STDMETHODIMP DP8P_DoWork(const DWORD dwFlags);

		STDMETHODIMP DP8P_EnumServiceProviders(const GUID * const pguidServiceProvider,
												const GUID * const pguidApplication,
												DPN_SERVICE_PROVIDER_INFO * const pSPInfoBuffer,
												DWORD * const pcbEnumData,
												DWORD * const pcReturned,
												const DWORD dwFlags);

		STDMETHODIMP DP8P_CancelAsyncOperation(const DPNHANDLE hAsyncHandle,
												const DWORD dwFlags);

		STDMETHODIMP DP8P_Connect(const DPN_APPLICATION_DESC * const pdnAppDesc,
								IDirectPlay8Address * const pHostAddr,
								IDirectPlay8Address * const pDeviceInfo,
								const DPN_SECURITY_DESC * const pdnSecurity,
								const DPN_SECURITY_CREDENTIALS * const pdnCredentials,
								const void * const pvUserConnectData,
								const DWORD dwUserConnectDataSize,
								void * const pvPlayerContext,
								void * const pvAsyncContext,
								DPNHANDLE * const phAsyncHandle,
								const DWORD dwFlags);

		STDMETHODIMP DP8P_SendTo(const DPNID dpnid,
								const DPN_BUFFER_DESC * const prgBufferDesc,
								const DWORD cBufferDesc,
								const DWORD dwTimeOut,
								void * const pvAsyncContext,
								DPNHANDLE * const phAsyncHandle,
								const DWORD dwFlags);

		STDMETHODIMP DP8P_GetSendQueueInfo(const DPNID dpnid,
											DWORD * const pdwNumMsgs,
											DWORD * const pdwNumBytes,
											const DWORD dwFlags);

		STDMETHODIMP DP8P_Host(const DPN_APPLICATION_DESC * const pdnAppDesc,
								IDirectPlay8Address ** const prgpDeviceInfo,
								const DWORD cDeviceInfo,
								const DPN_SECURITY_DESC * const pdnSecurity,
								const DPN_SECURITY_CREDENTIALS * const pdnCredentials,
								void * const pvPlayerContext,
								const DWORD dwFlags);

		STDMETHODIMP DP8P_GetApplicationDesc(DPN_APPLICATION_DESC * const pAppDescBuffer,
											DWORD * const pcbDataSize,
											const DWORD dwFlags);

		STDMETHODIMP DP8P_SetApplicationDesc(const DPN_APPLICATION_DESC * const pad,
											const DWORD dwFlags);

		STDMETHODIMP DP8P_CreateGroup(const DPN_GROUP_INFO * const pdpnGroupInfo,
									PVOID const pvGroupContext,
									PVOID const pvAsyncContext,
									DPNHANDLE * const phAsyncHandle,
									const DWORD dwFlags);

		STDMETHODIMP DP8P_DestroyGroup(const DPNID idGroup,
									PVOID const pvAsyncContext,
									DPNHANDLE * const phAsyncHandle,
									const DWORD dwFlags);

		STDMETHODIMP DP8P_AddPlayerToGroup(const DPNID idGroup,
										const DPNID idClient,
										PVOID const pvAsyncContext,
										DPNHANDLE * const phAsyncHandle,
										const DWORD dwFlags);

		STDMETHODIMP DP8P_RemovePlayerFromGroup(const DPNID idGroup,
												const DPNID idClient,
												PVOID const pvAsyncContext,
												DPNHANDLE * const phAsyncHandle,
												const DWORD dwFlags);

		STDMETHODIMP DP8P_SetGroupInfo(const DPNID dpnid,
										DPN_GROUP_INFO * const pdpnGroupInfo,
										PVOID const pvAsyncContext,
										DPNHANDLE * const phAsyncHandle,
										const DWORD dwFlags);

		STDMETHODIMP DP8P_GetGroupInfo(const DPNID dpnid,
										DPN_GROUP_INFO * const pdpnGroupInfo,
										DWORD * const pdwSize,
										const DWORD dwFlags);

		STDMETHODIMP DP8P_EnumPlayersAndGroups(DPNID * const prgdpid,
												DWORD * const pcdpid,
												const DWORD dwFlags);

		STDMETHODIMP DP8P_EnumGroupMembers(const DPNID dpid,
										DPNID * const prgdpid,
										DWORD * const pcdpid,
										const DWORD dwFlags);

		STDMETHODIMP DP8P_SetPeerInfo(const DPN_PLAYER_INFO * const pdpnPlayerInfo,
									PVOID const pvAsyncContext,
									DPNHANDLE * const phAsyncHandle,
									const DWORD dwFlags);

		STDMETHODIMP DP8P_GetPeerInfo(const DPNID dpnid,
									DPN_PLAYER_INFO * const pdpnPlayerInfo,
									DWORD * const pdwSize,
									const DWORD dwFlags);

		STDMETHODIMP DP8P_GetPeerAddress(const DPNID dpnid,
										IDirectPlay8Address ** const pAddress,
										const DWORD dwFlags);

		STDMETHODIMP DP8P_GetLocalHostAddresses(IDirectPlay8Address ** const prgpAddress,
												DWORD * const pcAddress,
												const DWORD dwFlags);
		STDMETHODIMP DP8P_Close(const DWORD dwFlags);

		STDMETHODIMP DP8P_EnumHosts(PDPN_APPLICATION_DESC const pApplicationDesc,
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

		STDMETHODIMP DP8P_DestroyPeer(const DPNID dpidClient,
										const void * const pDestroyInfo,
										const DWORD dwDestroyInfoSize,
										const DWORD dwFlags);

		STDMETHODIMP DP8P_ReturnBuffer(const DPNHANDLE hBufferHandle,
										const DWORD dwFlags);

		STDMETHODIMP DP8P_GetPlayerContext(const DPNID dpnid,
											PVOID * const ppvPlayerContext,
											const DWORD dwFlags);

		STDMETHODIMP DP8P_GetGroupContext(const DPNID dpnid,
										PVOID * const ppvGroupContext,
										const DWORD dwFlags);

		STDMETHODIMP DP8P_GetCaps(DPN_CAPS * const pdpCaps,
								const DWORD dwFlags);

		STDMETHODIMP DP8P_SetCaps(const DPN_CAPS * const pdpCaps,
								const DWORD dwFlags);

		STDMETHODIMP DP8P_SetSPCaps(const GUID * const pguidSP,
									const DPN_SP_CAPS * const pdpspCaps,
									const DWORD dwFlags);

		STDMETHODIMP DP8P_GetSPCaps(const GUID * const pguidSP,
									DPN_SP_CAPS *const pdpspCaps,
									const DWORD dwFlags);

		STDMETHODIMP DP8P_GetConnectionInfo(const DPNID dpnid,
											DPN_CONNECTION_INFO * const pdpConnectionInfo,
											const DWORD dwFlags);

		STDMETHODIMP DP8P_TerminateSession(void * const pvTerminateData,
											const DWORD dwTerminateDataSize,
											const DWORD dwFlags);



		// Returns whether this object has been initialized or not.
		inline BOOL IsInitialized(void) { return (this->m_fInitialized); };
};

//==================================================================================
// Class type definitions
//==================================================================================
typedef class CWrapDP8Peer			CWrapDP8Peer,			* PWRAPDP8PEER;

#endif //__DNETRSLT_WRAPPEER__
