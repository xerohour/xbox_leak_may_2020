#ifndef __DNGURL_WRAPBASE__
#define __DNGURL_WRAPBASE__

//==================================================================================
// Classes
//==================================================================================
class CWrapDP8Address
{
	private:
		HANDLE m_hLog;
		PDIRECTPLAY8ADDRESS		m_pDP8Address; // pointer to real DirectPlay8Address object


	public:
		CWrapDP8Address(HANDLE hLog);
		virtual ~CWrapDP8Address(void);


		// DirectPlay8Address object maintenance functions
		HRESULT CoCreate(void);
		HRESULT Release(void);


		// IDirectPlay8Address methods
		STDMETHODIMP DPA_QueryInterface(REFIID riid, PVOID* ppvObj);
		STDMETHODIMP DPA_BuildFromURLW(WCHAR* wszSourceURL);
		STDMETHODIMP DPA_BuildFromURLA(CHAR* szSourceURL);
		STDMETHODIMP DPA_Duplicate(PDIRECTPLAY8ADDRESS* ppdpaNewAddress);
		STDMETHODIMP DPA_SetEqual(PDIRECTPLAY8ADDRESS pdpaNewAddress);
		STDMETHODIMP DPA_IsEqual(PDIRECTPLAY8ADDRESS pdpaNewAddress);
		STDMETHODIMP DPA_Clear(void);
		STDMETHODIMP DPA_GetURLW(WCHAR* pwszURL, PDWORD pdwNumChars);
		STDMETHODIMP DPA_GetURLA(CHAR* pszURL, PDWORD pdwNumChars);
		STDMETHODIMP DPA_GetSP(GUID* pguidSP);
		STDMETHODIMP DPA_GetUserData(void* pvUserData, PDWORD pdwBufferSize);
		STDMETHODIMP DPA_SetSP(const GUID * const pguidSP);
		STDMETHODIMP DPA_SetUserData(const void * const pvUserData,
									const DWORD dwDataSize);
		STDMETHODIMP DPA_GetNumComponents(PDWORD pdwNumComponents);
		STDMETHODIMP DPA_GetComponentByName(const WCHAR * const wszName,
											void* pvBuffer, PDWORD pdwBufferSize,
											PDWORD pdwDataType);
		STDMETHODIMP DPA_GetComponentByIndex(const DWORD dwComponentID,
											WCHAR* pwszName, PDWORD pdwNameLen,
											void* pvBuffer, PDWORD pdwBufferSize,
											PDWORD pdwDataType);
		STDMETHODIMP DPA_AddComponent(const WCHAR * const wszName,
										const void * const pvData,
										const DWORD dwDataSize,
										const DWORD dwDataType);
		STDMETHODIMP DPA_GetDevice(GUID* pguidDevice);
		STDMETHODIMP DPA_SetDevice(const GUID * const pguidDevice);
		STDMETHODIMP DPA_BuildFromSockAddr(const SOCKADDR * const pSockAddr);

//		STDMETHODIMP DPA_BuildFromDPADDRESS(void* pvAddress, DWORD dwDataSize);

};

class CWrapDP8AddressIP
{
	private:
		HANDLE m_hLog;
/* XBOX - IP functions are merged with address object for now
		PDIRECTPLAY8ADDRESSIP		m_pDP8AddressIP; // pointer to real DirectPlay8AddressIP object
*/

	public:
		PDIRECTPLAY8ADDRESS			m_pDP8AddressIP; // pointer to real DirectPlay8Address object

		CWrapDP8AddressIP(HANDLE hLog);
		virtual ~CWrapDP8AddressIP(void);


		// DirectPlay8AddressIP object maintenance functions
		HRESULT CoCreate(void);
		HRESULT Release(void);


		// IDirectPlay8Address methods
//		STDMETHODIMP DPA_QueryInterface(REFIID riid, PVOID* ppvObj);
		STDMETHODIMP DPA_Duplicate(PDIRECTPLAY8ADDRESS* ppdpaNewAddress);
		STDMETHODIMP DPA_SetEqual(PDIRECTPLAY8ADDRESS pdpaNewAddress);
		STDMETHODIMP DPA_IsEqual(PDIRECTPLAY8ADDRESS pdpaNewAddress);
		STDMETHODIMP DPA_GetURLW(WCHAR* pwszURL, PDWORD pdwNumChars);
		STDMETHODIMP DPA_GetURLA(CHAR* pszURL, PDWORD pdwNumChars);
		STDMETHODIMP DPA_GetUserData(void* pvUserData, PDWORD pdwBufferSize);
		STDMETHODIMP DPA_SetUserData(const void * const pvUserData, const DWORD dwDataSize);
		STDMETHODIMP DPA_BuildFromSockAddr(SOCKADDR* psockAddr);
		STDMETHODIMP DPA_GetSockAddress(SOCKADDR* psockAddress, PDWORD pwdAddressBufferSize);
		STDMETHODIMP DPA_BuildAddress(WCHAR* wszAddress, USHORT usPort);
		STDMETHODIMP DPA_GetAddress(WCHAR* wszAddress, PDWORD pdwAddressLength, USHORT* psPort);
		STDMETHODIMP DPA_BuildLocalAddress(GUID* pguidAdapter, USHORT usPort);
		STDMETHODIMP DPA_GetLocalAddress(GUID* pguidAdapter, USHORT* pusPort);
		

};

//==================================================================================
// Class type definitions
//==================================================================================
typedef class CWrapDP8Address			CWrapDP8Address,			* PWRAPDP8ADDRESS;
typedef class CWrapDP8AddressIP			CWrapDP8AddressIP,			* PWRAPDP8ADDRESSIP;

#endif //__DNGURL_WRAPBASE__
