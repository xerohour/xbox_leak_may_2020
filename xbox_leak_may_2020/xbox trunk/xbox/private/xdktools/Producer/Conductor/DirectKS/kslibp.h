// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation, 1996 - 2000.  All rights reserved.
//
// File Name:
//
//	kslibp.h
//
// Abstract:
//
//  private header for kslib project
//
// @@BEGIN_MSINTERNAL
//      mitchr      08/08/1998  - created
//      jwexler     05/10/2000 - moved out of kslib.cpp
// @@END_MSINTERNAL
// -------------------------------------------------------------------------------


#ifndef _KSLIBP_H
#define _KSLIBP_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include "ilog.h"
#include "directks.h"
//#include "debug.h"

//
// macros
//
#define SafeCloseHandle(h)  if (IsValidHandle((h))) CloseHandle((h));    \
                            h = INVALID_HANDLE_VALUE
#define SafeLocalFree(p)    if ((p)) LocalFree((p));  \
                            p = NULL
#define IsValidHandle(h)    (!(((h) == NULL) || ((h) == INVALID_HANDLE_VALUE)))
#define LOG                 s_iLog->Log                         

//
// because we can't include both windows.h and wdm.h
//
#define CTL_CODE(DeviceType, Function, Method, Access)  \
        (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
        
#define FILE_DEVICE_KS         0x0000002f
#define METHOD_NEITHER                  3

#define FILE_ANY_ACCESS            0x0000
#define FILE_READ_ACCESS           0x0001    // file & pipe
#define FILE_WRITE_ACCESS          0x0002    // file & pipe

inline BOOL
IsEqualGUIDAligned(GUID guid1, GUID guid2)
{
    return ((*(PLONGLONG)(&guid1) == *(PLONGLONG)(&guid2)) && (*((PLONGLONG)(&guid1) + 1) == *((PLONGLONG)(&guid2) + 1)));
}
//
// SetupAPI, KS function typedefs
//
typedef LPCSTR PCTSTR, LPCTSTR, PCUTSTR, LPCUTSTR;

typedef	HDEVINFO
(WINAPI *GETCLASSDEVS)
(
    IN CONST GUID *ClassGuid,  OPTIONAL
    IN PCTSTR      Enumerator, OPTIONAL
    IN HWND        hwndParent, OPTIONAL
    IN DWORD       Flags
);

typedef BOOL
(WINAPI *ENUMDEVICEINTERFACES)
(
    IN  HDEVINFO                   DeviceInfoSet,
    IN  PSP_DEVINFO_DATA           DeviceInfoData,     OPTIONAL
    IN  CONST GUID                *InterfaceClassGuid,
    IN  DWORD                      MemberIndex,
    OUT PSP_DEVICE_INTERFACE_DATA  DeviceInterfaceData
);

typedef BOOL
(WINAPI *ENUMDEVICEINFO)
(
    IN  HDEVINFO                   DeviceInfoSet,
    IN  DWORD                      MemberIndex,
    OUT PSP_DEVINFO_DATA           DeviceInfoData
);

typedef BOOL
(WINAPI *GETDEVICEINTERFACEDETAIL)
(
    IN  HDEVINFO                           DeviceInfoSet,
    IN  PSP_DEVICE_INTERFACE_DATA          DeviceInterfaceData,
#ifdef UNICODE
    OUT PSP_DEVICE_INTERFACE_DETAIL_DATA_W DeviceInterfaceDetailData,     OPTIONAL
#else
    OUT PSP_DEVICE_INTERFACE_DETAIL_DATA_A DeviceInterfaceDetailData,     OPTIONAL
#endif
    IN  DWORD                              DeviceInterfaceDetailDataSize,
    OUT PDWORD                             RequiredSize,                  OPTIONAL
    OUT PSP_DEVINFO_DATA                   DeviceInfoData                 OPTIONAL
);

typedef BOOL
(WINAPI *GETDEVICEREGISTRYPROPERTY)
(
    IN  HDEVINFO         DeviceInfoSet,
    IN  PSP_DEVINFO_DATA DeviceInfoData,
    IN  DWORD            Property,
    OUT PDWORD           PropertyRegDataType, OPTIONAL
    OUT PBYTE            PropertyBuffer,
    IN  DWORD            PropertyBufferSize,
    OUT PDWORD           RequiredSize         OPTIONAL
);

typedef HKEY
(WINAPI *OPENDEVICEINTERFACEREGKEY)
(
    IN HDEVINFO                  DeviceInfoSet,
    IN PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,
    IN DWORD                     Reserved,
    IN REGSAM                    samDesired
);

typedef BOOL
(WINAPI *GETDEVICEINTERFACEALIAS)
(
    IN  HDEVINFO                   DeviceInfoSet,
    IN  PSP_DEVICE_INTERFACE_DATA  DeviceInterfaceData,
    IN  CONST GUID                *AliasInterfaceClassGuid,
    OUT PSP_DEVICE_INTERFACE_DATA  AliasDeviceInterfaceData
);

typedef BOOL
(WINAPI *DESTROYDEVICEINFOLIST)
(
    IN HDEVINFO DeviceInfoSet
);

typedef DWORD
(WINAPI *KSCREATEPIN)
(
    HANDLE, 
    PKSPIN_CONNECT, 
    DWORD, 
    PHANDLE
);

//
// kslib function prototypes
//
ULONG
CalculateFrameSize
(
    ULONG   nFrame,
    ULONG   cFrames,
    ULONG   cbBuffer
);

//
// externs
//
extern ILog                         *s_iLog;
extern GETCLASSDEVS                 fxnSetupDiGetClassDevs;
extern ENUMDEVICEINTERFACES         fxnSetupDiEnumDeviceInterfaces;
extern ENUMDEVICEINFO               fxnSetupDiEnumDeviceInfo;
extern GETDEVICEINTERFACEDETAIL     fxnSetupDiGetDeviceInterfaceDetail;
extern GETDEVICEREGISTRYPROPERTY    fxnSetupDiGetDeviceRegistryProperty;
extern OPENDEVICEINTERFACEREGKEY    fxnSetupDiOpenDeviceInterfaceRegKey;
extern GETDEVICEINTERFACEALIAS      fxnSetupDiGetDeviceInterfaceAlias;
extern DESTROYDEVICEINFOLIST        fxnSetupDiDestroyDeviceInfoList;
extern KSCREATEPIN                  fxnKsCreatePin;

#endif

