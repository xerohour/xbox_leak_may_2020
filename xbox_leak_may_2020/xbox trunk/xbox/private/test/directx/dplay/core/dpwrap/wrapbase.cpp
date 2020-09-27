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
#include "wrapbase.h"

//==================================================================================
// Class Definitions for the DP8Address Class
//==================================================================================



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::CWrapDP8Address()"
//==================================================================================
// CWrapDP8Address constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CWrapDP8Address object.
//
// Arguments: None.
//
// Returns: None (the object).
//==================================================================================
CWrapDP8Address::CWrapDP8Address(HANDLE hLog):
	m_pDP8Address(NULL),
	m_hLog(hLog)
{
} // CWrapDP8Address::CWrapDP8Address
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::~CWrapDP8Address()"
//==================================================================================
// CWrapDP8Address destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CWrapDP8Address object.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CWrapDP8Address::~CWrapDP8Address(void)
{
	HRESULT			hr;

	if (this->m_pDP8Address != NULL)
	{
		hr = this->Release();
		if (hr != S_OK)
		{
			xLog(m_hLog, XLL_WARN, "Failed releasing DirectPlay8Address object!  0x%08x", hr);
		} // end if (failed releasing object)
	} // end if (we have an object pointer)
} // CWrapDP8Address::~CWrapDP8Address
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::CoCreate()"
//==================================================================================
// CWrapDP8Address::CoCreate
//----------------------------------------------------------------------------------
//
// Description: Calls CoCreateInstance for a new DirectPlay8Address object.
//
// Arguments: None.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
HRESULT CWrapDP8Address::CoCreate(void)
{
	HRESULT		hr;

	if (this->m_pDP8Address != NULL)
	{
		xLog(m_hLog, XLL_WARN, "Can't create a DirectPlay8Address object because one already exists (%x)!",
			this->m_pDP8Address);
		hr = ERROR_ALREADY_EXISTS;
	} // end if (already have an object)
	else
	{
		hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &this->m_pDP8Address, NULL);
	} // end else (don't have an object)

	return (hr);
} // CWrapDP8Address::CoCreate
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::Release()"
//==================================================================================
// CWrapDP8Address::Release
//----------------------------------------------------------------------------------
//
// Description: Releases the DirectPlay8Address object.
//
// Arguments: None.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
HRESULT CWrapDP8Address::Release(void)
{
	HRESULT		hr = DPN_OK;

	if (this->m_pDP8Address == NULL)
	{
		xLog(m_hLog, XLL_WARN, "Can't release DirectPlay8Address object because one doesn't exist!");
		hr = ERROR_BAD_ENVIRONMENT;
	} // end if (no object yet)
	else
	{
		this->m_pDP8Address->Release();
		this->m_pDP8Address = NULL;
		
		//this->m_fInitialized = FALSE;
		//this->m_fConnected = FALSE;
	} // end else (there's an object)

	return (hr);
} // CWrapDP8Address::Release
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_BuildFromURLW()"
//==================================================================================
// CWrapDP8Address::DPA_BuildFromURLW
//----------------------------------------------------------------------------------
//
// Description: Builds this DNAddress object given a Unicode URL.
//
// Arguments:
//	WCHAR* wszSourceURL		Unicode string with URL from which to build.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_BuildFromURLW(WCHAR* wszSourceURL)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->BuildFromURLW(wszSourceURL);

	return (hr);
} // CWrapDP8Address::DPA_BuildFromURLW
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_BuildFromURLA()"
//==================================================================================
// CWrapDP8Address::DPA_BuildFromURLA
//----------------------------------------------------------------------------------
//
// Description: Builds this DNAddress object given an ANSI URL.
//
// Arguments:
//	CHAR* szSourceURL	ANSI string with URL from which to build.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_BuildFromURLA(CHAR* szSourceURL)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->BuildFromURLA(szSourceURL);
	
	return (hr);
} // CWrapDP8Address::DPA_BuildFromURLA
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_Duplicate()"
//==================================================================================
// CWrapDP8Address::DPA_Duplicate
//----------------------------------------------------------------------------------
//
// Description: Duplicates this address into a new object.
//
// Arguments:
//	PDIRECTPLAY8ADDRESS* ppdpaNewAddress	Place to store new object created.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_Duplicate(PDIRECTPLAY8ADDRESS* ppdpaNewAddress)
{
	HRESULT			hr;

	hr = this->m_pDP8Address->Duplicate(ppdpaNewAddress);

	return (hr);
} // CWrapDP8Address::DPA_Duplicate
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_SetEqual()"
//==================================================================================
// CWrapDP8Address::DPA_Duplicate
//----------------------------------------------------------------------------------
//
// Description: Duplicates this address into a new object.
//
// Arguments:
//	PDIRECTPLAY8ADDRESS* ppdpaNewAddress	Place to store new object created.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_SetEqual(PDIRECTPLAY8ADDRESS pdpaNewAddress)
{
	HRESULT			hr;

	hr = this->m_pDP8Address->SetEqual(pdpaNewAddress);

	return (hr);
} // CWrapDP8Address::DPA_SetEqual
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_IsEqual()"
//==================================================================================
// CWrapDP8Address::DPA_Duplicate
//----------------------------------------------------------------------------------
//
// Description: Duplicates this address into a new object.
//
// Arguments:
//	PDIRECTPLAY8ADDRESS* ppdpaNewAddress	Place to store new object created.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_IsEqual(PDIRECTPLAY8ADDRESS pdpaNewAddress)
{
	HRESULT			hr;

	hr = this->m_pDP8Address->IsEqual(pdpaNewAddress);

	return (hr);
} // CWrapDP8Address::DPA_IsEqual
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_Clear()"
//==================================================================================
// CWrapDP8Address::DPA_Clear
//----------------------------------------------------------------------------------
//
// Description: Duplicates this address into a new object.
//
// Arguments: None.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_Clear(void)
{
	HRESULT			hr;

	hr = this->m_pDP8Address->Clear();

	return (hr);
} // CWrapDP8Address::DPA_Clear
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_GetURLW()"
//==================================================================================
// CWrapDP8Address::DPA_GetURLW
//----------------------------------------------------------------------------------
//
// Description: Retrieves a Unicode URL corresponding to the object's current stored
//				information.
//				If the buffer is NULL or too small, the size required (in
//				characters, including NULL terminator) is returned in the DWORD
//				pointed to by pdwNumChars.
//
// Arguments:
//	WCHAR* pwszURL		Pointer to buffer to store results.
//	PDWORD pdwNumChars	Pointer to length of buffer, or place to store length
//						required.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_GetURLW(WCHAR* pwszURL, PDWORD pdwNumChars)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->GetURLW(pwszURL, pdwNumChars);

	return (hr);
} // CWrapDP8Address::DPA_GetURLW
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_GetURLA()"
//==================================================================================
// CWrapDP8Address::DPA_GetURLA
//----------------------------------------------------------------------------------
//
// Description: Retrieves an ANSI URL corresponding to the object's current stored
//				information.
//				If the buffer is NULL or too small, the size required (in
//				characters, including NULL terminator) is returned in the DWORD
//				pointed to by pdwNumChars.
//
// Arguments:
//	CHAR* pszURL		Pointer to buffer to store results.
//	PDWORD pdwNumChars	Pointer to length of buffer, or place to store size length
//						required.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_GetURLA(CHAR* pszURL, PDWORD pdwNumChars)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->GetURLA(pszURL, pdwNumChars);

	return (hr);
} // CWrapDP8Address::DPA_GetURLA
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_GetSP()"
//==================================================================================
// CWrapDP8Address::DPA_GetSP
//----------------------------------------------------------------------------------
//
// Description: Retrieves the GUID of the SP currently indicated in the object.
//
// Arguments:
//	GUID* pguidSP	Pointer to buffer to store GUID.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_GetSP(GUID* pguidSP)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->GetSP(pguidSP);

	return (hr);
} // CWrapDP8Address::DPA_GetSP
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_GetUserData()"
//==================================================================================
// CWrapDP8Address::DPA_GetUserData
//----------------------------------------------------------------------------------
//
// Description: Retrieves the current user data stored with the object.
//				If the buffer is NULL or too small, the size required (in bytes) is
//				returned in the DWORD pointed to by pdwDataSize.
//
// Arguments:
//	void* pvData			Pointer to buffer to store results.
//	PDWORD pdwBufferSize	Pointer to size of buffer, or place to store size required.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_GetUserData(void* pvUserData, PDWORD pdwBufferSize)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->GetUserData(pvUserData, pdwBufferSize);

	return (hr);
} // CWrapDP8Address::DPA_GetUserData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_SetSP()"
//==================================================================================
// CWrapDP8Address::DPA_SetSP
//----------------------------------------------------------------------------------
//
// Description: Sets the GUID of the SP for the object.
//
// Arguments:
//	GUID* pguidSP	Pointer to GUID to set the provider to.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_SetSP(const GUID * const pguidSP)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->SetSP(pguidSP);

	return (hr);
} // CWrapDP8Address::DPA_SetSP
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_SetUserData()"
//==================================================================================
// CWrapDP8Address::DPA_SetUserData
//----------------------------------------------------------------------------------
//
// Description: Sets the current user data associated with the object.
//
// Arguments:
//	void* pvUserData	Pointer to buffer with data to store.
//	DWORD dwDataSize	Size of user data buffer.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_SetUserData(const void * const pvUserData,
											const DWORD dwDataSize)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->SetUserData(pvUserData, dwDataSize);

	return (hr);
} // CWrapDP8Address::DPA_SetUserData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_GetNumComponents()"
//==================================================================================
// CWrapDP8Address::DPA_GetNumComponents
//----------------------------------------------------------------------------------
//
// Description: Retrieves the number of elements stored in the address.
//
// Arguments:
//	PDWORD pdwNumComponents		Place to store number of components associated with
//								object.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_GetNumComponents(PDWORD pdwNumComponents)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->GetNumComponents(pdwNumComponents);

	return (hr);
} // CWrapDP8Address::DPA_GetNumComponents
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_GetComponentByName()"
//==================================================================================
// CWrapDP8Address::DPA_GetComponentByName
//----------------------------------------------------------------------------------
//
// Description: Retrieves the element with the given name.
//
// Arguments:
//	WCHAR* wszName			Unicode string holding name of element to retrieve.
//	void* pvBuffer			Pointer to buffer to hold value.
//	PDWORD pdwBufferSize	Pointer to size of buffer, or place to store size
//							required.
//	PDWORD pdwDataType		Place to store data type of value.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_GetComponentByName(const WCHAR * const wszName,
													void* pvBuffer,
													PDWORD pdwBufferSize,
													PDWORD pdwDataType)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->GetComponentByName(wszName, pvBuffer, pdwBufferSize, pdwDataType);

	return (hr);
} // CWrapDP8Address::DPA_GetComponentByName
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_GetComponentByIndex()"
//==================================================================================
// CWrapDP8Address::DPA_GetComponentByIndex
//----------------------------------------------------------------------------------
//
// Description: Retrieves the element with the given name.
//
// Arguments:
//	DWORD dwComponentID		Index of component to retrieve.
//	WCHAR* pwszName			Place to store Unicode name of element.
//	PDWORD pdwNameLen		Pointer to length of string, or place to store length
//							required.
//	void* pvBuffer			Pointer to buffer to hold value.
//	PDWORD pdwBufferSize	Pointer to size of buffer, or place to store size
//							required.
//	PDWORD pdwDataType		Place to store data type of value.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_GetComponentByIndex(const DWORD dwComponentID,
													WCHAR* pwszName,
													PDWORD pdwNameLen,
													void* pvBuffer,
													PDWORD pdwBufferSize,
													PDWORD pdwDataType)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->GetComponentByIndex(dwComponentID, pwszName, pdwNameLen, pvBuffer, pdwBufferSize, pdwDataType);

	return (hr);
} // CWrapDP8Address::DPA_GetComponentByIndex
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_AddComponent()"
//==================================================================================
// CWrapDP8Address::DPA_AddComponent
//----------------------------------------------------------------------------------
//
// Description: Adds an element with the given name and value.
//
// Arguments:
//	WCHAR* wszName		Unicode name of element.
//	void* pvData		Pointer to data for element.
//	DWORD dwDataSize	Size of data.
//	DWORD dwDataType	Type of data.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_AddComponent(const WCHAR * const wszName,
												const void * const pvData,
												const DWORD dwDataSize,
												const DWORD dwDataType)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->AddComponent(wszName, pvData, dwDataSize, dwDataType);

	return (hr);
} // CWrapDP8Address::DPA_AddComponent
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_GetDevice()"
//==================================================================================
// CWrapDP8Address::DPA_GetDevice
//----------------------------------------------------------------------------------
//
// Description: Retrieves the GUID of the device currently indicated in the object.
//
// Arguments:
//	GUID* pguidSP	Pointer to buffer to store GUID.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_GetDevice(GUID* pguidSP)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->GetDevice(pguidSP);

	return (hr);
} // CWrapDP8Address::DPA_GetDevice
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_SetDevice()"
//==================================================================================
// CWrapDP8Address::DPA_SetDevice
//----------------------------------------------------------------------------------
//
// Description: Sets the GUID of the device for the object.
//
// Arguments:
//	GUID* pguidSP	Pointer to GUID to set the evice to.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_SetDevice(const GUID * const pguidSP)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->SetDevice(pguidSP);

	return (hr);
} // CWrapDP8Address::DPA_SetDevice
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_BuildFromSockAddr()"
//==================================================================================
// CWrapDP8Address::DPA_BuildFromSockAddr
//----------------------------------------------------------------------------------
//
// Description: Sets the GUID of the device for the object.
//
// Arguments:
//	SOCKADDR* pSockAddr		Pointer to SOCKADDR to set the address to
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_BuildFromSockAddr(const SOCKADDR * const pSockAddr)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->BuildFromSockAddr(pSockAddr);

	return (hr);
} // CWrapDP8Address::DPA_BuildFromSockAddr
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





// TODO - No legacy support for Xbox
/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8Address::DPA_BuildFromDPADDRESS()"
//==================================================================================
// CWrapDP8Address::DPA_BuildFromDPADDRESS
//----------------------------------------------------------------------------------
//
// Description: Builds this DNAddress object given a DPlay7 Address.
//
// Arguments:
//	PVOID pvAddress		DPlay7 Address to convert.
//	DWORD dwDataSize	Size of DPlay7 buffer to convert
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8Address::DPA_BuildFromDPADDRESS(void* pvAddress, DWORD dwDataSize)
{
	HRESULT		hr;

	hr = this->m_pDP8Address->BuildFromDPADDRESS(pvAddress, dwDataSize);

	return (hr);
} // CWrapDP8Address::DPA_BuildFromDPADDRESS
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/


/*************************************************************************************

  
//==================================================================================
// Class Definitions for the DP8AddressIP Class
//==================================================================================


**************************************************************************************/

#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::CWrapDP8AddressIP()"
//==================================================================================
// CWrapDP8AddressIP constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CWrapDP8AddressIP object.
//
// Arguments: None.
//
// Returns: None (the object).
//==================================================================================
CWrapDP8AddressIP::CWrapDP8AddressIP(HANDLE hLog):
	m_pDP8AddressIP(NULL),
	m_hLog(hLog)
{
} // CWrapDP8AddressIP::CWrapDP8AddressIP
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::~CWrapDP8AddressIP()"
//==================================================================================
// CWrapDP8Address destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CWrapDP8AddressIP object.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CWrapDP8AddressIP::~CWrapDP8AddressIP(void)
{
	HRESULT			hr;

	if (this->m_pDP8AddressIP != NULL)
	{
		hr = this->Release();
		if (hr != S_OK)
		{
			xLog(m_hLog, XLL_WARN, "Failed releasing DirectPlay8AddressIP object!  0x%08x", hr);
		} // end if (failed releasing object)
	} // end if (we have an object pointer)
} // CWrapDP8AddressIP::~CWrapDP8AddressIP
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::CoCreate()"
//==================================================================================
// CWrapDP8AddressIP::CoCreate
//----------------------------------------------------------------------------------
//
// Description: Calls CoCreateInstance for a new DirectPlay8AddressIP object.
//
// Arguments: None.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
HRESULT CWrapDP8AddressIP::CoCreate(void)
{
	HRESULT		hr;

	if (this->m_pDP8AddressIP != NULL)
	{
		xLog(m_hLog, XLL_WARN, "Can't create a DirectPlay8AddressIP object because one already exists (%x)!",
			this->m_pDP8AddressIP);
		hr = ERROR_ALREADY_EXISTS;
	} // end if (already have an object)
	else
	{
		hr = DirectPlay8AddressCreate(IID_IDirectPlay8AddressIP, (LPVOID *) &this->m_pDP8AddressIP, NULL);
	} // end else (don't have an object)

	return (hr);
} // CWrapDP8AddressIP::CoCreate
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::Release()"
//==================================================================================
// CWrapDP8AddressIP::Release
//----------------------------------------------------------------------------------
//
// Description: Releases the DirectPlay8AddressIP object.
//
// Arguments: None.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
HRESULT CWrapDP8AddressIP::Release(void)
{
	HRESULT		hr = DPN_OK;

	if (this->m_pDP8AddressIP == NULL)
	{
		xLog(m_hLog, XLL_WARN, "Can't release DirectPlay8AddressIP object because one doesn't exist!");
		hr = ERROR_BAD_ENVIRONMENT;
	} // end if (no object yet)
	else
	{
		this->m_pDP8AddressIP->Release();
		this->m_pDP8AddressIP = NULL;
		
		//this->m_fInitialized = FALSE;
		//this->m_fConnected = FALSE;
	} // end else (there's an object)

	return (hr);
} // CWrapDP8AddressIP::Release
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_Duplicate()"
//==================================================================================
// CWrapDP8AddressIP::DPA_Duplicate
//----------------------------------------------------------------------------------
//
// Description: Duplicates this address into a new object.
//
// Arguments:
//	PDIRECTPLAY8ADDRESS* ppdpaNewAddress	Place to store new object created.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_Duplicate(PDIRECTPLAY8ADDRESS* ppdpaNewAddress)
{
	HRESULT			hr;

	hr = this->m_pDP8AddressIP->Duplicate(ppdpaNewAddress);

	return (hr);
} // CWrapDP8AddressIP::DPA_Duplicate
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_SetEqual()"
//==================================================================================
// CWrapDP8AddressIP::DPA_SetEqual
//----------------------------------------------------------------------------------
//
// Description: Sets the this object equal to the one being passed in.
//
// Arguments:
//	PDIRECTPLAY8ADDRESS pdpaNewAddress	Object we are setting equal to.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_SetEqual(PDIRECTPLAY8ADDRESS pdpaNewAddress)
{
	HRESULT			hr;

	hr = this->m_pDP8AddressIP->SetEqual(pdpaNewAddress);

	return (hr);
} // CWrapDP8AddressIP::DPA_SetEqual
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_IsEqual()"
//==================================================================================
// CWrapDP8AddressIP::DPA_Duplicate
//----------------------------------------------------------------------------------
//
// Description:	Compares this address into a new object.
//
// Arguments:
//	PDIRECTPLAY8ADDRESS* ppdpaNewAddress	Place to store new object created.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_IsEqual(PDIRECTPLAY8ADDRESS pdpaNewAddress)
{
	HRESULT			hr;

	hr = this->m_pDP8AddressIP->IsEqual(pdpaNewAddress);

	return (hr);
} // CWrapDP8AddressIP::DPA_IsEqual
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_GetURLW()"
//==================================================================================
// CWrapDP8AddressIP::DPA_GetURLW
//----------------------------------------------------------------------------------
//
// Description: Retrieves a Unicode URL corresponding to the object's current stored
//				information.
//				If the buffer is NULL or too small, the size required (in
//				characters, including NULL terminator) is returned in the DWORD
//				pointed to by pdwNumChars.
//
// Arguments:
//	WCHAR* pwszURL		Pointer to buffer to store results.
//	PDWORD pdwNumChars	Pointer to length of buffer, or place to store length
//						required.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_GetURLW(WCHAR* pwszURL, PDWORD pdwNumChars)
{
	HRESULT		hr;

	hr = this->m_pDP8AddressIP->GetURLW(pwszURL, pdwNumChars);

	return (hr);
} // CWrapDP8AddressIP::DPA_GetURLW
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_GetURLA()"
//==================================================================================
// CWrapDP8AddressIP::DPA_GetURLA
//----------------------------------------------------------------------------------
//
// Description: Retrieves an ANSI URL corresponding to the object's current stored
//				information.
//				If the buffer is NULL or too small, the size required (in
//				characters, including NULL terminator) is returned in the DWORD
//				pointed to by pdwNumChars.
//
// Arguments:
//	CHAR* pszURL		Pointer to buffer to store results.
//	PDWORD pdwNumChars	Pointer to length of buffer, or place to store size length
//						required.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_GetURLA(CHAR* pszURL, PDWORD pdwNumChars)
{
	HRESULT		hr;

	hr = this->m_pDP8AddressIP->GetURLA(pszURL, pdwNumChars);

	return (hr);
} // CWrapDP8AddressIP::DPA_GetURLA
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_GetUserData()"
//==================================================================================
// CWrapDP8AddressIP::DPA_GetUserData
//----------------------------------------------------------------------------------
//
// Description: Retrieves the current user data stored with the object.
//				If the buffer is NULL or too small, the size required (in bytes) is
//				returned in the DWORD pointed to by pdwDataSize.
//
// Arguments:
//	void* pvData			Pointer to buffer to store results.
//	PDWORD pdwBufferSize	Pointer to size of buffer, or place to store size required.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_GetUserData(void* pvUserData, PDWORD pdwBufferSize)
{
	HRESULT		hr;

	hr = this->m_pDP8AddressIP->GetUserData(pvUserData, pdwBufferSize);

	return (hr);
} // CWrapDP8AddressIP::DPA_GetUserData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_SetUserData()"
//==================================================================================
// CWrapDP8AddressIP::DPA_SetUserData
//----------------------------------------------------------------------------------
//
// Description: Sets the current user data associated with the object.
//
// Arguments:
//	void* pvUserData	Pointer to buffer with data to store.
//	DWORD dwDataSize	Size of user data buffer.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_SetUserData(const void * const pvUserData,
											    const DWORD dwDataSize)
{
	HRESULT		hr;

	hr = this->m_pDP8AddressIP->SetUserData(pvUserData, dwDataSize);

	return (hr);
} // CWrapDP8AddressIP::DPA_SetUserData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_BuildFromSockAddr()"
//==================================================================================
// CWrapDP8AddressIP::DPA_BuildFromSockAddr
//----------------------------------------------------------------------------------
//
// Description: Retrieves the number of elements stored in the address.
//
// Arguments:
//	SOCKADDR pSockAddr	Sock address to build from.
//								
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_BuildFromSockAddr(SOCKADDR* pSockAddr)
{
	HRESULT		hr;

	hr = this->m_pDP8AddressIP->BuildFromSockAddr(pSockAddr);

	return (hr);
} // CWrapDP8AddressIP::DPA_BuildFromSockAddr
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_GetSockAddress()"
//==================================================================================
// CWrapDP8AddressIP::DPA_GetSockAddress
//----------------------------------------------------------------------------------
//
// Description: Retrieves the element with the given name.
//
// Arguments:
//	SOCKADDR* psockAddress. Sock address pointer of SOCKAddr retrieved
//	PDWORD pdwAddressBufferSize		Size of Sock Address Buffer Recieved.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_GetSockAddress(SOCKADDR* psockAddress,
													PDWORD pdwAddressBufferSize)
{
	HRESULT		hr;

	hr = this->m_pDP8AddressIP->GetSockAddress(psockAddress, pdwAddressBufferSize);

	return (hr);
} // CWrapDP8Address::DPA_GetComponentByName
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_BuildAddress()"
//==================================================================================
// CWrapDP8AddressIP::DPA_BuildAddress
//----------------------------------------------------------------------------------
//
// Description: Retrieves the element with the given name.
//
// Arguments:
//	WCHAR* pwszAddress		Remote address to build Address from.
//	USHORT usPort			Port on remote host to connect.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_BuildAddress(WCHAR* pwszAddress,
												 USHORT usPort)
{
	HRESULT		hr;

	hr = this->m_pDP8AddressIP->BuildAddress(pwszAddress, usPort);

	return (hr);
} // CWrapDP8AddressIP::DPA_BuildAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_GetAddress()"
//==================================================================================
// CWrapDP8AddressIP::DPA_GetAddress
//----------------------------------------------------------------------------------
//
// Description: Adds an element with the given name and value.
//
// Arguments:
//	WCHAR* pwszAddress		Buffer to receive hostname.
//	PDWORD pdwAddressLength Size of address buffer.
//	USHORT*	psPort			Port specified in the address structure.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_GetAddress(WCHAR* pwszAddress,
											   PDWORD pdwAddressLength,
											   USHORT* pusPort)
{
	HRESULT		hr;

	hr = this->m_pDP8AddressIP->GetAddress(pwszAddress, pdwAddressLength, pusPort);

	return (hr);
} // CWrapDP8AddressIP::DPA_GetAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_BuildLocalAddress()"
//==================================================================================
// CWrapDP8AddressIP::DPA_BuildLocalAddress
//----------------------------------------------------------------------------------
//
// Description: Retrieves the element with the given name.
//
// Arguments:
//	GUID* pguidAdapter		GUID of adapter to build address from.
//	USHORT usPort			Port on local device to host from.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_BuildLocalAddress(GUID* pguidAdapter,
													  USHORT usPort)
{
	HRESULT		hr;

	hr = this->m_pDP8AddressIP->BuildLocalAddress(pguidAdapter, usPort);

	return (hr);
} // CWrapDP8AddressIP::DPA_BuildLocalAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CWrapDP8AddressIP::DPA_GetLocalAddress()"
//==================================================================================
// CWrapDP8AddressIP::DPA_GetLocalAddress
//----------------------------------------------------------------------------------
//
// Description: Adds an element with the given name and value.
//
// Arguments:
//	GUID* pguidAdapter		Buffer to receive hostname.
//	USHORT*	pusPort			Port specified in the address structure.
//
// Returns: DN_OK if successful, error code otherwise.
//==================================================================================
STDMETHODIMP CWrapDP8AddressIP::DPA_GetLocalAddress(GUID* pguidAdapter,
													USHORT* pusPort)
{
	HRESULT		hr;

	hr = this->m_pDP8AddressIP->GetLocalAddress(pguidAdapter, pusPort);

	return (hr);
} // CWrapDP8AddressIP::DPA_GetLocalAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
