/*==========================================================================;
 *
 *  Copyright (C) 2000 - 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dpaddr.h
 *  Content:   DirectPlayAddress include file
 ***************************************************************************/

#ifndef __DIRECTPLAYADDRESS__
#define __DIRECTPLAYADDRESS__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <dplay8.h>


/****************************************************************************
 *
 * DirectPlay8Address Interface Pointer definitions
 *
 ****************************************************************************/

typedef struct IDirectPlay8Address IDirectPlay8Address;
typedef IDirectPlay8Address         *PDIRECTPLAY8ADDRESS, *LPDIRECTPLAY8ADDRESS;

/****************************************************************************
 *
 * DirectPlay8Address Forward Declarations For External Types
 *
 ****************************************************************************/

typedef struct sockaddr SOCKADDR;

/****************************************************************************
 *
 * DirectPlay8Address Constants
 *
 ****************************************************************************/
//
// Asynchronous operation flags
//
#define DPNA_DATATYPE_STRING                0x00000001
#define DPNA_DATATYPE_DWORD                 0x00000002
#define DPNA_DATATYPE_GUID                  0x00000003
#define DPNA_DATATYPE_BINARY                0x00000004
#define DPNA_DATATYPE_STRING_ANSI           0x00000005

#define DPNA_INDEX_INVALID                  0xFFFFFFFF

/****************************************************************************
 *
 * DirectPlay8Address Address Elements
 *
 ****************************************************************************/

#define DPNA_DPNSVR_PORT                                      6073

#define DPNA_SEPARATOR_KEYVALUE             L'='
#define DPNA_SEPARATOR_USERDATA             L'#'
#define DPNA_SEPARATOR_COMPONENT            L';'
#define DPNA_ESCAPECHAR                     L'%'

// Header
#define DPNA_HEADER                         L"x-directplay:/"

// key names for address components
#define DPNA_KEY_APPLICATION_INSTANCE       L"applicationinstance"
#define DPNA_KEY_BAUD                       L"baud"
#define DPNA_KEY_DEVICE                     L"device"
#define DPNA_KEY_FLOWCONTROL                L"flowcontrol"
#define DPNA_KEY_HOSTNAME                   L"hostname"
#define DPNA_KEY_PARITY                     L"parity"
#define DPNA_KEY_PHONENUMBER                L"phonenumber"
#define DPNA_KEY_PORT                       L"port"
#define DPNA_KEY_PROGRAM                    L"program"
#define DPNA_KEY_PROVIDER                   L"provider"
#define DPNA_KEY_STOPBITS                   L"stopbits"

//// ANSI DEFINITIONS

// Header
#define DPNA_HEADER_A                       "x-directplay:/"
#define DPNA_SEPARATOR_KEYVALUE_A           '='
#define DPNA_SEPARATOR_USERDATA_A           '#'
#define DPNA_SEPARATOR_COMPONENT_A          ';'
#define DPNA_ESCAPECHAR_A                   '%'

// key names for address components
#define DPNA_KEY_APPLICATION_INSTANCE_A     "applicationinstance"
#define DPNA_KEY_BAUD_A                     "baud"
#define DPNA_KEY_DEVICE_A                   "device"
#define DPNA_KEY_FLOWCONTROL_A              "flowcontrol"
#define DPNA_KEY_HOSTNAME_A                 "hostname"
#define DPNA_KEY_PARITY_A                   "parity"
#define DPNA_KEY_PHONENUMBER_A              "phonenumber"
#define DPNA_KEY_PORT_A                     "port"
#define DPNA_KEY_PROGRAM_A                  "program"
#define DPNA_KEY_PROVIDER_A                 "provider"
#define DPNA_KEY_STOPBITS_A                 "stopbits"

// Shortcut values
//
// These can be used instead of the corresponding CLSID_DP8SP_XXXX guids
//
#define DPNA_VALUE_TCPIPPROVIDER            L"IP"
#define DPNA_VALUE_TCPIPPROVIDER_A            "IP"

#define IID_IDirectPlay8Address		0
#define IID_IDirectPlay8AddressIP	1

/****************************************************************************
 *
 * DirectPlay8Address Functions
 *
 ****************************************************************************/

extern HRESULT WINAPI DirectPlay8AddressCreate( DWORD dwIID, void ** ppvInterface, void **pUnknown);

/****************************************************************************
 *
 * DirectPlay8Address Application Interfaces
 *
 ****************************************************************************/

// IUnknown methods
ULONG WINAPI IDirectPlay8Address_AddRef(LPDIRECTPLAY8ADDRESS pDirectPlay8Address);
ULONG WINAPI IDirectPlay8Address_Release(LPDIRECTPLAY8ADDRESS pDirectPlay8Address);

// IDirectPlay8Address methods
HRESULT WINAPI IDirectPlay8Address_BuildFromURLW(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, WCHAR *pwszSourceURL);
HRESULT WINAPI IDirectPlay8Address_BuildFromURLA(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, CHAR *pszSourceURL);
HRESULT WINAPI IDirectPlay8Address_Duplicate(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, PDIRECTPLAY8ADDRESS *ppdpaNewAddress);
HRESULT WINAPI IDirectPlay8Address_SetEqual(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, PDIRECTPLAY8ADDRESS pdpaAddress);
HRESULT WINAPI IDirectPlay8Address_IsEqual(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, PDIRECTPLAY8ADDRESS pdpaAddress);
HRESULT WINAPI IDirectPlay8Address_Clear(LPDIRECTPLAY8ADDRESS pDirectPlay8Address);
HRESULT WINAPI IDirectPlay8Address_GetURLW(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, WCHAR *pwszURL, PDWORD pdwNumChars);
HRESULT WINAPI IDirectPlay8Address_GetURLA(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, CHAR *pszURL, PDWORD pdwNumChars);
HRESULT WINAPI IDirectPlay8Address_GetSP(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, GUID *pguidSP);
HRESULT WINAPI IDirectPlay8Address_GetUserData(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, void *pvUserData, PDWORD pdwBufferSize);
HRESULT WINAPI IDirectPlay8Address_SetSP(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, const GUID *const pguidSP);
HRESULT WINAPI IDirectPlay8Address_SetUserData(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, const void * const pvUserData, const DWORD dwDataSize);
HRESULT WINAPI IDirectPlay8Address_GetNumComponents(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, PDWORD pdwNumComponents);
HRESULT WINAPI IDirectPlay8Address_GetComponentByName(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, const WCHAR * const pwszName, void *pvBuffer, PDWORD pdwBufferSize, PDWORD pdwDataType);
HRESULT WINAPI IDirectPlay8Address_GetComponentByIndex(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, const DWORD dwComponentID, WCHAR * pwszName, PDWORD pdwNameLen, void *pvBuffer, PDWORD pdwBufferSize, PDWORD pdwDataType);
HRESULT WINAPI IDirectPlay8Address_AddComponent(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, const WCHAR * const pwszName, const void * const lpvData, const DWORD dwDataSize, const DWORD dwDataType);
HRESULT WINAPI IDirectPlay8Address_GetDevice(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, GUID *pGUID);
HRESULT WINAPI IDirectPlay8Address_SetDevice(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, const GUID * const pGUID);
HRESULT WINAPI IDirectPlay8Address_BuildFromSockAddr(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, const SOCKADDR * const pSockAddr);
HRESULT WINAPI IDirectPlay8Address_BuildAddress(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, const WCHAR * const wszAddress, const USHORT usPort);
HRESULT WINAPI IDirectPlay8Address_BuildLocalAddress(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, const GUID * const pguidAdapter, const USHORT usPort);
HRESULT WINAPI IDirectPlay8Address_GetSockAddress(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, SOCKADDR *pSockAddr, PDWORD pDword);
HRESULT WINAPI IDirectPlay8Address_GetLocalAddress(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, GUID *pguidAdapter, USHORT *pusPort);
HRESULT WINAPI IDirectPlay8Address_GetAddress(LPDIRECTPLAY8ADDRESS pDirectPlay8Address, WCHAR *wszAddress, PDWORD pdwAddressLength, USHORT *psPort);

#ifdef __cplusplus

// IDirectPlay8Address wrapper class
struct IDirectPlay8Address
{
    __inline ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return IDirectPlay8Address_AddRef(this);
    }

    __inline ULONG STDMETHODCALLTYPE Release(void)
    {
        return IDirectPlay8Address_Release(this);
    }

    __inline HRESULT STDMETHODCALLTYPE QueryInterface(DWORD riid, LPVOID *ppvObj)
    {
	if (ppvObj != NULL)
            *ppvObj = this;

        return S_OK;
    }

    __inline HRESULT STDMETHODCALLTYPE BuildFromURLW(WCHAR *pwszSourceURL)
    {
        return IDirectPlay8Address_BuildFromURLW(this, pwszSourceURL);
    }

    __inline HRESULT STDMETHODCALLTYPE BuildFromURLA(CHAR *pszSourceURL)
    {
        return IDirectPlay8Address_BuildFromURLA(this, pszSourceURL);
    }

    __inline HRESULT STDMETHODCALLTYPE Duplicate(PDIRECTPLAY8ADDRESS *ppdpaNewAddress)
    {
        return IDirectPlay8Address_Duplicate(this, ppdpaNewAddress);
    }

    __inline HRESULT STDMETHODCALLTYPE SetEqual(PDIRECTPLAY8ADDRESS pdpaAddress)
    {
        return IDirectPlay8Address_SetEqual(this, pdpaAddress);
    }

    __inline HRESULT STDMETHODCALLTYPE IsEqual(PDIRECTPLAY8ADDRESS pdpaAddress)
    {
        return IDirectPlay8Address_IsEqual(this, pdpaAddress);
    }

    __inline HRESULT STDMETHODCALLTYPE Clear(void)
    {
        return IDirectPlay8Address_Clear(this);
    }

    __inline HRESULT STDMETHODCALLTYPE GetURLW(WCHAR *pwszURL, PDWORD pdwNumChars)
    {
        return IDirectPlay8Address_GetURLW(this, pwszURL, pdwNumChars);
    }

    __inline HRESULT STDMETHODCALLTYPE GetURLA(CHAR *pszURL, PDWORD pdwNumChars)
    {
        return IDirectPlay8Address_GetURLA(this, pszURL, pdwNumChars);
    }

    __inline HRESULT STDMETHODCALLTYPE GetSP(GUID *pguidSP)
    {
        return IDirectPlay8Address_GetSP(this, pguidSP);
    }

    __inline HRESULT STDMETHODCALLTYPE GetUserData(void *pvUserData, PDWORD pdwBufferSize)
    {
        return IDirectPlay8Address_GetUserData(this, pvUserData, pdwBufferSize);
    }

    __inline HRESULT STDMETHODCALLTYPE SetSP(const GUID *const pguidSP)
    {
        return IDirectPlay8Address_SetSP(this, pguidSP);
    }

    __inline HRESULT STDMETHODCALLTYPE SetUserData(const void * const pvUserData, const DWORD dwDataSize)
    {
        return IDirectPlay8Address_SetUserData(this, pvUserData, dwDataSize);
    }

    __inline HRESULT STDMETHODCALLTYPE GetNumComponents(PDWORD pdwNumComponents)
    {
        return IDirectPlay8Address_GetNumComponents(this, pdwNumComponents);
    }

    __inline HRESULT STDMETHODCALLTYPE GetComponentByName(const WCHAR * const pwszName, void *pvBuffer, PDWORD pdwBufferSize, PDWORD pdwDataType)
    {
        return IDirectPlay8Address_GetComponentByName(this, pwszName, pvBuffer, pdwBufferSize, pdwDataType);
    }

    __inline HRESULT STDMETHODCALLTYPE GetComponentByIndex(const DWORD dwComponentID, WCHAR * pwszName, PDWORD pdwNameLen, void *pvBuffer, PDWORD pdwBufferSize, PDWORD pdwDataType)
    {
        return IDirectPlay8Address_GetComponentByIndex(this, dwComponentID, pwszName, pdwNameLen, pvBuffer, pdwBufferSize, pdwDataType);
    }

    __inline HRESULT STDMETHODCALLTYPE AddComponent(const WCHAR * const pwszName, const void * const lpvData, const DWORD dwDataSize, const DWORD dwDataType)
    {
        return IDirectPlay8Address_AddComponent(this, pwszName, lpvData, dwDataSize, dwDataType);
    }

    __inline HRESULT STDMETHODCALLTYPE GetDevice(GUID *pGUID)
    {
        return IDirectPlay8Address_GetDevice(this, pGUID);
    }

    __inline HRESULT STDMETHODCALLTYPE SetDevice(const GUID * const pGUID)
    {
        return IDirectPlay8Address_SetDevice(this, pGUID);
    }

    // Constructs a IDirectPlay8 TCP Address from a SOCKADDR structure
    __inline HRESULT STDMETHODCALLTYPE BuildFromSockAddr(const SOCKADDR * const pSockAddr)
    {
        return IDirectPlay8Address_BuildFromSockAddr(this, pSockAddr);
    }

    // Constructs a TCP Address from a string (hostname) and port
    __inline HRESULT STDMETHODCALLTYPE BuildAddress(const WCHAR * const wszAddress, const USHORT usPort)
    {
        return IDirectPlay8Address_BuildAddress(this, wszAddress, usPort);
    }

    // Builds a local TCP Address
    __inline HRESULT STDMETHODCALLTYPE BuildLocalAddress(const GUID * const pguidAdapter, const USHORT usPort)
    {
        return IDirectPlay8Address_BuildLocalAddress(this, pguidAdapter, usPort);
    }

    // Gets the address from the structure in SOCKADR format
    __inline HRESULT STDMETHODCALLTYPE GetSockAddress(SOCKADDR *pSockAddr, PDWORD pDword)
    {
        return IDirectPlay8Address_GetSockAddress(this, pSockAddr, pDword);
    }

    // Gets the local afddress
    __inline HRESULT STDMETHODCALLTYPE GetLocalAddress(GUID *pguidAdapter, USHORT *pusPort)
    {
        return IDirectPlay8Address_GetLocalAddress(this, pguidAdapter, pusPort);
    }

    // Gets the remote address
    __inline HRESULT STDMETHODCALLTYPE GetAddress(WCHAR *wszAddress, PDWORD pdwAddressLength, USHORT *psPort)
    {
        return IDirectPlay8Address_GetAddress(this, wszAddress, pdwAddressLength, psPort);
    }

};

#endif // __cplusplus

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
