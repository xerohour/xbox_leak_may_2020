/***************************************************************************\
* Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
*  Portions: Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.*
*                                                                           *
* Module: Duma.h															*
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*		Andrei Osnovich    01/17/99  Created								*
*                                                                           *
\***************************************************************************/

#ifndef DUMA_H
#define DUMA_H

#ifndef DUMA_VXD
#include <windows.h>
#endif

/*
 *Duma delegated services.
 */

#define DUMA_HOT_KEY_CREATE 		  1
#define DUMA_HOT_KEY_DESTROY		  2
#define DUMA_HOT_KEY_RESET			  3
#define DUMA_CACHE_CONTROL			  4
#define DUMA_SOFTICE_CONTROL          5
#define DUMA_ENFORCE_FILTER           6
// Errors
#define DUMA_OK 		   0

#define CACHE_INVALIDATE	 0x01
#define CACHE_ENABLE_WT 	 0x02
#define CACHE_ENABLE_WB 	 0x04
#define CACHE_DISABLE		 0x08
#define CACHE_ENABLE_AGP	 0x10
#define CACHE_DISABLE_AGP	 0x20

typedef struct _CacheControlData {
	DWORD dwFlags;
	DWORD dwStartAddr;
	DWORD dwLength;
} CACHECONTROLDATA, *LPCACHECONTROLDATA;

// Hot key options:
#define HK_BUSY_MASK	   0x80000000 // mask for MSB - controls busy of hotkey
#define HK_OCCURED_MASK    0x7FFFFFFF // mask for value (w/o MSB)

#ifndef DUMA_VXD

typedef DWORD* HKHANDLE;  // HotKey Handle

typedef struct _HOTKEY
{
	DWORD	  dwValue;
	HKHANDLE  hkHandle;
} HOTKEY, *LPHOTKEY;

#ifdef __cplusplus

// C++ interface class for Duma VxD services.
class CHotKeyInterface
{
  protected:
	  HANDLE DumaHandle;

  public:
	  HKHANDLE CreateHotKey(DWORD dwKey);
	  BOOL DestroyHotKey(HKHANDLE );
	  BOOL ResetHotKey(HKHANDLE );
      BOOL EnforceFilter(void);
	  inline BOOL isInitialized()
		{ return DumaHandle!=NULL; }

  public:
	  CHotKeyInterface();  // constructor
	  ~CHotKeyInterface(); // destructor
};

typedef CHotKeyInterface CHOTKEYINTERFACE, *LPCHOTKEYINTERFACE;

/*
 * Macros for easy Hot Key Interface access.
 */

#define CREATEHOTKEY(pInterface,HotKey)                                 \
	if ((HotKey).dwValue)                                               \
    {                                                                   \
	  if (pInterface)                                                   \
		(HotKey).hkHandle = pInterface->CreateHotKey((HotKey).dwValue); \
    }

#define DESTROYHOTKEY(pInterface,HotKey)                                \
	if (pInterface && (HotKey).hkHandle)                                \
    {                                                                   \
	   pInterface->DestroyHotKey((HotKey).hkHandle);                    \
	   (HotKey).hkHandle = 0;                                           \
    }

#define ENFORCEFILTER(pInterface)                                       \
    res_ = FALSE;                                                       \
	if (pInterface)                                                     \
    {                                                                   \
	   res_ = pInterface->EnforceFilter();                              \
    }

// Warning!!! The function that uses the POLLHOTKEY macros has to 
// define the following local variable:
//      BOOL res_; 
#define POLLHOTKEY(pInterface,HotKey)                                   \
    res_ = FALSE;                                                       \
	if ((HotKey).hkHandle && (*((HotKey).hkHandle) & HK_OCCURED_MASK))  \
    {                                                                   \
        res_ = TRUE;                                                    \
		if (pInterface) pInterface->ResetHotKey((HotKey).hkHandle);     \
    }                                                                   \
    if (res_ == TRUE)

#ifdef  DEFINE_HOTKEY_INTERFACES

// CHotKeyInterface class members implementation is to be included once 
// somewhere in a ring 3 application that wants to utilize the DUMA hot 
// key support. 

CHotKeyInterface::CHotKeyInterface()
{
    if ((DumaHandle = CreateFile("\\\\.\\DUMA.VXD", 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL))==INVALID_HANDLE_VALUE)
        DumaHandle = NULL;
}

CHotKeyInterface::~CHotKeyInterface()
{
    if (DumaHandle)
    {     
        CloseHandle(DumaHandle);
        DumaHandle = NULL;
    }
}

HKHANDLE CHotKeyInterface::CreateHotKey(DWORD dwKey)
{
    HKHANDLE hkHandle = NULL;
    DWORD RetVal = 0;

    if (DumaHandle) {
        DeviceIoControl(DumaHandle, DUMA_HOT_KEY_CREATE, (LPVOID)&dwKey, sizeof(DWORD), &hkHandle, sizeof(HKHANDLE), &RetVal, NULL);
        if (RetVal != sizeof(HKHANDLE)) return NULL;
        return hkHandle;
    }
    return NULL;
}

BOOL CHotKeyInterface::DestroyHotKey(HKHANDLE hkHandle)
{
    BOOL ServiceOutput = FALSE;
    DWORD RetVal = 0;

    if (DumaHandle) {
        DeviceIoControl(DumaHandle, DUMA_HOT_KEY_DESTROY, hkHandle, sizeof(HKHANDLE), &ServiceOutput, sizeof(ServiceOutput), &RetVal, NULL);
        if (RetVal != sizeof(ServiceOutput)) return FALSE;
        return ServiceOutput;
    }
    return FALSE;  
}

BOOL CHotKeyInterface::ResetHotKey(HKHANDLE hkHandle)
{
    BOOL ServiceOutput = FALSE;
    DWORD RetVal = 0;

    if (DumaHandle) {
        DeviceIoControl(DumaHandle, DUMA_HOT_KEY_RESET, hkHandle, sizeof(HKHANDLE), &ServiceOutput, sizeof(ServiceOutput), &RetVal, NULL);
        if (RetVal != sizeof(ServiceOutput)) return FALSE;
        return ServiceOutput;
    }
    return FALSE;  
}

BOOL CHotKeyInterface::EnforceFilter(void)
{
    BOOL ServiceOutput = FALSE;
    DWORD RetVal = 0;

    if (DumaHandle) {
        DeviceIoControl(DumaHandle, DUMA_ENFORCE_FILTER, NULL, 0, &ServiceOutput, sizeof(ServiceOutput), &RetVal, NULL);
        if (RetVal != sizeof(ServiceOutput)) return FALSE;
        return ServiceOutput;
    }
    return FALSE;  
}

#endif  //DEFINE_HOTKEY_INTERFACES

#else   //__cplusplus==0

// "C" interface structure for Duma VxD services.
typedef struct _HotKeyInterface
{
	  HANDLE DumaHandle;
	  HKHANDLE (*CreateHotKey)(struct _HotKeyInterface *, DWORD dwKey);
	  BOOL (*DestroyHotKey)(struct _HotKeyInterface *, HKHANDLE );
	  BOOL (*ResetHotKey)(struct _HotKeyInterface *, HKHANDLE );
      BOOL (*EnforceFilter)(struct _HotKeyInterface *);
} CHOTKEYINTERFACE, *LPCHOTKEYINTERFACE;

/*
 * Macros for easy Hot Key Interface access.
 */

#define CREATEHOTKEY(pInterface,HotKey)                                             \
	if ((HotKey).dwValue)                                                           \
    {                                                                               \
	  if (pInterface)                                                               \
		(HotKey).hkHandle = pInterface->CreateHotKey(pInterface,(HotKey).dwValue);  \
    }

#define DESTROYHOTKEY(pInterface,HotKey)                                            \
	if (pInterface && (HotKey).hkHandle)                                            \
    {                                                                               \
	   pInterface->DestroyHotKey(pInterface,(HotKey).hkHandle);                     \
	   (HotKey).hkHandle = 0;                                                       \
    }

#define ENFORCEFILTER(pInterface)                                                   \
    res_ = FALSE;                                                                   \
	if (pInterface)                                                                 \
    {                                                                               \
	   res_ = pInterface->EnforceFilter(pInterface);                                \
    }

// Warning!!! The function that uses the POLLHOTKEY macros has to 
// define the following local variable:
//      BOOL res_; 
#define POLLHOTKEY(pInterface,HotKey)                                               \
    res_ = FALSE;                                                                   \
	if ((HotKey).hkHandle && (*((HotKey).hkHandle) & HK_OCCURED_MASK))              \
    {                                                                               \
        res_ = TRUE;                                                                \
		if (pInterface) pInterface->ResetHotKey(pInterface,(HotKey).hkHandle);      \
    }                                                                               \
    if (res_ == TRUE)

#ifdef  DEFINE_HOTKEY_INTERFACES

// HotKeyInterface "C" interface's implementation is to be included once 
// somewhere in a ring 3 application that wants to utilize the DUMA hot 
// key support. 

HKHANDLE CreateHotKey(LPCHOTKEYINTERFACE pInterface, DWORD dwKey)
{
    HKHANDLE hkHandle = NULL;
    DWORD RetVal = 0;

    if (pInterface->DumaHandle) {
        DeviceIoControl(pInterface->DumaHandle, DUMA_HOT_KEY_CREATE, (LPVOID)&dwKey, sizeof(DWORD), &hkHandle, sizeof(HKHANDLE), &RetVal, NULL);
        if (RetVal != sizeof(HKHANDLE)) return NULL;
        return hkHandle;
    }
    return NULL;
}

BOOL DestroyHotKey(LPCHOTKEYINTERFACE pInterface, HKHANDLE hkHandle)
{
    BOOL ServiceOutput = FALSE;
    DWORD RetVal = 0;

    if (pInterface->DumaHandle) {
        DeviceIoControl(pInterface->DumaHandle, DUMA_HOT_KEY_DESTROY, hkHandle, sizeof(HKHANDLE), &ServiceOutput, sizeof(ServiceOutput), &RetVal, NULL);
        if (RetVal != sizeof(ServiceOutput)) return FALSE;
        return RetVal;
    }
    return ServiceOutput;  
}

BOOL ResetHotKey(LPCHOTKEYINTERFACE pInterface, HKHANDLE hkHandle)
{
    BOOL ServiceOutput = FALSE;
    DWORD RetVal = 0;

    if (pInterface->DumaHandle) {
        DeviceIoControl(pInterface->DumaHandle, DUMA_HOT_KEY_RESET, hkHandle, sizeof(HKHANDLE), &ServiceOutput, sizeof(ServiceOutput), &RetVal, NULL);
        if (RetVal != sizeof(ServiceOutput)) return FALSE;
        return ServiceOutput;
    }
    return FALSE;  
}

BOOL EnforceFilter(LPCHOTKEYINTERFACE pInterface)
{
    BOOL ServiceOutput = FALSE;
    DWORD RetVal = 0;

    if (pInterface->DumaHandle) {
        DeviceIoControl(pInterface->DumaHandle, DUMA_ENFORCE_FILTER, NULL, 0, &ServiceOutput, sizeof(ServiceOutput), &RetVal, NULL);
        if (RetVal != sizeof(ServiceOutput)) return FALSE;
        return ServiceOutput;
    }
    return FALSE;  
}

BOOL CreateHotKeyInterface(LPCHOTKEYINTERFACE pInterface)
{
    if (!pInterface)
        return FALSE;
    if ((pInterface->DumaHandle = CreateFile("\\\\.\\DUMA.VXD", 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL))==INVALID_HANDLE_VALUE)
        return FALSE;
    pInterface->CreateHotKey  = CreateHotKey;
    pInterface->DestroyHotKey = DestroyHotKey;
    pInterface->ResetHotKey   = ResetHotKey;
}

void DestroyHotKeyInterface(LPCHOTKEYINTERFACE pInterface)
{
    if (pInterface->DumaHandle)
    {     
        CloseHandle(pInterface->DumaHandle);
        pInterface->DumaHandle = NULL;
    }
}

#else   //DEFINE_HOTKEY_INTERFACES==0
extern  BOOL CreateHotKeyInterface(LPCHOTKEYINTERFACE pInterface);
extern  void DestroyHotKeyInterface(LPCHOTKEYINTERFACE pInterface);
#endif  //DEFINE_HOTKEY_INTERFACES

#endif  //__cplusplus==0

#endif //DUMA_VXD

#endif // #define DUMA_H
