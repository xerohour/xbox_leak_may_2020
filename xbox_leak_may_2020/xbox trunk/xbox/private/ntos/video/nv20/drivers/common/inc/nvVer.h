// nvver.h
//      Versions of NV drivers
//
// Copyright (C) Microsoft Corporation 1993.
// Portions Copyright 1993,1996 NVidia Corporation. All Rights Reserved.
//

// The following version numbers are owned by Microsoft.  We have permission
// to change the last 3 digits of each version string but no other digits
// until the next major Microsoft release.
//
// NOTE: This release must be bumped up to 4.10... (0x040A...) when starting
// to build Win98 drivers
//
//
// The following #defines are used for the driver version number. They should be
// updated for each build and they should agree with each other.
//
// Also, because NV_DRIVER_VERSION has a leading 0, it can't be used as a number
// in drivers (it'll be interpretted as octal). And since it does contain non-octal
// values drivers should use NV_DRIVER_VERSION_NUMBER instead.
//
#ifndef WIN31
#define NV_DRIVER_VERSION              1440
#endif
#define NV_DRIVER_VERSION_NUMBER       1440
#define NV_VERSION_NUMBER               4,13,01,NV_DRIVER_VERSION
#define NV_VERSION_NUMBER_NT4           4,00,1381,NV_DRIVER_VERSION
#define NV_VERSION_NUMBER_NT5           5,13,01,NV_DRIVER_VERSION
#define NV_VERSION_NUMBER_DW            (0x0413010 | NV_DRIVER_VERSION)
#define NV_VERSION_STRING              "14.40"
#define NV_VERSION_MS_STRING           "4.13.01.1440\0"
#define NV_VERSION_MS_STRING_NT4       "4.00.1381.1440\0"
#define NV_VERSION_MS_STRING_NT5       "5.13.01.1440\0"

#define NV_COMPANY_NAME_STRING_SHORT    "NVIDIA"
#define NV_COMPANY_NAME_STRING_FULL     "NVIDIA Corporation"
#define NV_COMPANY_NAME_STRING          NV_COMPANY_NAME_STRING_FULL

#define NV04_PART_NAME_STRING           "RIVA TNT"
#define NV05_PART_NAME_STRING           "RIVA TNT2"
#define NV10_PART_NAME_STRING           "GeForce 256"
#define NV10GL_PART_NAME_STRING         "Quadro"
#define NV11_PART_NAME_STRING           "GeForce2 MX"
#define NV11GL_PART_NAME_STRING         "Quadro2 MXR"
#define NV15_PART_NAME_STRING           "GeForce2 GTS"
#define NV15GL_PART_NAME_STRING         "Quadro2 Pro"
#define NV20_PART_NAME_STRING           "GeForce3"
#define NV04_COMPAT_PART_NAME_STRING    NV_COMPANY_NAME_STRING_SHORT " Compatible"

#define NV_PART_NAME_STRING             NV04_COMPAT_PART_NAME_STRING



// #define MSBLD               // Defined for MS source builds only
                               // comment out for NVidia builds.
#ifndef MSBLD
#ifndef WIN32
#define VERSION                     NV_VERSION_MS_STRING
#endif


#ifndef _WIN32_WINNT
//
// Use NVidia standard version numbers for Win9x builds.
//
#define VER_PRODUCTVERSION_STR      NV_VERSION_MS_STRING
#define VER_PRODUCTVERSION          NV_VERSION_NUMBER
#define VER_PRODUCTVERSION_DW       NV_VERSION_NUMBER_DW
#else

//
// Use "standard" NT version numbers.
//
#include "ntverp.h"
#endif // #ifdef _WIN32_WINNT
#endif // #ifdef MSBLD




//
// NVDISP.drv NVidia Display Driver
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Display driver, Version X.XX\0"
//
#define NV_NVDISPVER95  NV_PART_NAME_STRING " Windows 95/98 Display driver, Version " NV_VERSION_STRING " \0"

//
//

//
// NV.vxd NVidia Display MiniVDD
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Display MiniVDD, Version X.XX\0"
//
#define NV_NVVDDVER     NV_PART_NAME_STRING " Display MiniVDD, Version " NV_VERSION_STRING " \0"
//
//


//
// NVDD32.DLL NVidia Direct Draw Driver
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Direct Draw Driver, Version X.XX\0"
//
#define NV_NVDD32VER    NV_PART_NAME_STRING " Direct Draw Driver, Version " NV_VERSION_STRING " \0"
//
//
//
// NVCPL.DLL NVidia Control Panel Extension
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Direct Draw Driver, Version X.XX\0"
//
#define NV_NVCPLVER    NV_COMPANY_NAME_STRING_SHORT " Display Properties Extension\0"


// NVQTWK.DLL NVidia Taskbar Utility Library
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Direct Draw Driver, Version X.XX\0"
//
#define NV_NVTASKBARVER   NV_COMPANY_NAME_STRING_SHORT " Taskbar Utility Library\0"


// NVDESK32.DLL NVidia Desktop Manager Library
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Direct Draw Driver, Version X.XX\0"
//
#define NV_NVDESKMGRVER   NV_COMPANY_NAME_STRING_SHORT " Desktop Manager Hook Library\0"


// NVSTERCP.DLL NVidia Stereo Control Panel
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Direct Draw Driver, Version X.XX\0"
//
#define NV_NVSTERCPVER   NV_COMPANY_NAME_STRING_SHORT " Stereo Properties Extension\0"


// NVSVC*.DLL NVIDIA Driver Helper Service
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Direct Draw Driver, Version X.XX\0"
//
#define NV_NVSVCVER   NV_COMPANY_NAME_STRING_SHORT " Driver Helper Service, Version " NV_VERSION_STRING "\0"


//
// NVDCI.drv NVidia DCI Driver
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia DCI driver, Version X.XX\0"
//
#define NV_NVDCIVER     NV_PART_NAME_STRING " DCI driver, Version " NV_VERSION_STRING " \0"
//
//

//
//
//
// NVINST32.DLL NVidia Driver Installation Library
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Driver Insatllation Library, Version X.XX\0"
//
#define NV_NVINST32VER      NV_PART_NAME_STRING " Driver Install Library, Version " NV_VERSION_STRING " \0"

//
//
//
// NVINSTNT.DLL NVidia Driver Installation Library
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Driver Insatllation Library, Version X.XX\0"
//
#define NV_NVINSTNTVER      NV_PART_NAME_STRING " Driver Install Library, Version " NV_VERSION_STRING " \0"

//
// NVRM NVidia Resource Manager
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Resource Manager DLL, Version X.XX\0"
//
#define NV_NVRMVER      NV_PART_NAME_STRING " Resource Manager, Version " NV_VERSION_STRING " \0"
//
//


//
//
//
// NVDD32.DLL NVidia Direct Draw/Direct 3D Driver
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Direct Draw 2.x Driver, Version X.XX\0"
//
//#define NV_D3D32VER     NV_PART_NAME_STRING " Direct Draw/Direct 3D Driver, Version " NV_VERSION_STRING " \0"
#define NV_DD32VER      NV_PART_NAME_STRING " Direct Draw/Direct 3D Driver, Version " NV_VERSION_STRING " \0"

//
//
//
// NV4_MINI.SYS NVidia Windows NT Miniport Driver
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Windows NT Miniport Driver, Version X.XX\0"
//
#ifdef _WIN32_WINNT
#if (_WIN32_WINNT < 0x0500)
#define NV_NTMINIVER    NV_PART_NAME_STRING " Windows NT 4.0 Miniport Driver, Version " NV_VERSION_STRING " \0"
#else
#define NV_NTMINIVER    NV_PART_NAME_STRING " Windows 2000 Miniport Driver, Version " NV_VERSION_STRING " \0"
#endif // #else
#endif // #ifdef _WIN32_WINNT

//
//
//
// NV4_DISP.DLL NVidia Windows NT Display Driver
//          Please update version in string below, DO NOT change
//          string, just the Number after Version.  It should
//          always look like this
//                  "NVidia Windows NT Display Driver, Version X.XX\0"
//
#ifdef _WIN32_WINNT
#if (_WIN32_WINNT < 0x0500)
#define NV_NTDISPVER    NV_PART_NAME_STRING " Windows NT 4.0 Display driver, Version " NV_VERSION_STRING " \0"
#else
#define NV_NTDISPVER    NV_PART_NAME_STRING " Windows 2000 Display driver, Version " NV_VERSION_STRING " \0"
#endif // #else
#endif // #ifdef _WIN32_WINNT


