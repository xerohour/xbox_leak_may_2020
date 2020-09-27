//
// File:         nvsvc_public.h
// Author(s):    Viet-Tam Luu
// Description:  Public exports and definitions for NVSVC clients.
// Target OS:    Windows NT, Windows 2000, Windows XP, Windows 98, Windows ME
// Platform:     IA32, IA64
//
// Copyright 2001 NVIDIA Corporation.
//

#ifndef __nvsvc_public_h__
#define __nvsvc_public_h__

#ifdef __cplusplus
extern "C" {
#endif


// Filenames of NVSVC binary under various OSes.
#define NV_SERVICE_FILE_WINNT   "nvsvc32.exe"
#define NV_SERVICE_FILE_WIN64   "nvsvc64.exe"
#define NV_SERVICE_FILE_WIN9X   "nvsvc.exe"

// Magic args to run Win9x service.
#define NV_SERVICE_ARG_W9XSTART "-runservice"
#define NV_SERVICE_ARG_W9XSTOP  "-stop"

// NVSVC service name and description strings.
#define NV_SERVICE_NAME         "NVSvc"
#define NV_SERVICE_DESC         "NVIDIA Driver Helper Service"

// Client events handled by NVSVC.
#define NVSVC_EVENT_STOP        "NVSvcStop"
#define NVSVC_EVENT_ICONBEGIN   "NVIconBeginEvent"
#define NVSVC_EVENT_ICONEND     "NVIconEndEvent"
#define NVSVC_EVENT_MODESWITCH  "NVModeSwitchEvent"
#define NVSVC_EVENT_DSWHOTKEY   "NVDisplaySwitchHotKey"
#define NVSVC_EVENT_PMM         "NVPMMEvent"

// Maximum length of any named events supported by nVidia drivers.
// This includes any prefixes to the name.
#define NVSVC_EVENT_NAME_MAX_LENGTH    256

// Event names as seen by the Win2K/NT4 drivers.
// We need to prepend a prefix to the name strings.
#define NVSVC_EVENT_ICONBEGIN_DRIVER   "\\BaseNamedObjects\\"NVSVC_EVENT_ICONBEGIN
#define NVSVC_EVENT_ICONEND_DRIVER     "\\BaseNamedObjects\\"NVSVC_EVENT_ICONEND
#define NVSVC_EVENT_MODESWITCH_DRIVER  "\\BaseNamedObjects\\"NVSVC_EVENT_MODESWITCH
#define NVSVC_EVENT_DSWHOTKEY_DRIVER   "\\BaseNamedObjects\\"NVSVC_EVENT_DSWHOTKEY

// Identifier for NVSVC's RM client.
#define NVSVC_RM_CLIENT_HANDLE  0x95510933

// Named mutex taken by first running NVSVC service (Win9x only).
#define NVSVC_MUTEX_INSTANCE    "NVSvcInstanceMutex"

#ifdef __cplusplus
}   // extern "C"
#endif

#endif	// #ifndef __nvsvc_public_h__

