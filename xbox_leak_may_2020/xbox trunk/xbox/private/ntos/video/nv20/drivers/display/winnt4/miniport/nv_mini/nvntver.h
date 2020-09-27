// nvver.h
//      Versions of NV drivers
//
// Copyright (C) NVidia Corporation 1997.
// Portions Copyright 1993,1996 NVidia Corporation. All Rights Reserved.
//

//
// This is the version number for the build. This is the only string which needs
// to be modified to bump the version number.
//
#include "nvver.h"
#define NV_VER                      NV_VERSION_STRING

//
// Common strings used in all binaries.
//
#ifdef VER_COMPANYNAME_STR
#undef VER_COMPANYNAME_STR
#endif

//
// MS build/version number for WHQL compliance - NT4
//
#ifdef VER_PRODUCTBUILD_QFE
#undef VER_PRODUCTBUILD_QFE
#endif
#define VER_PRODUCTBUILD_QFE NV_DRIVER_VERSION

//
// MS build/version number for WHQL compliance - Win2K.
//
#if (_WIN32_WINNT > 0x0400)

#ifdef VER_PRODUCTBUILD
#undef VER_PRODUCTBUILD
#endif
#define VER_PRODUCTBUILD 01

#ifdef VER_PRODUCTBUILD_QFE
#undef VER_PRODUCTBUILD_QFE
#endif
#define VER_PRODUCTBUILD_QFE NV_DRIVER_VERSION

#ifdef VER_PRODUCTVERSION_STRING
#undef VER_PRODUCTVERSION_STRING
#endif
#define VER_PRODUCTVERSION_STRING "5.13"

#ifdef VER_PRODUCTVERSION
#undef VER_PRODUCTVERSION
#endif
#define VER_PRODUCTVERSION 5,13,VER_PRODUCTBUILD,VER_PRODUCTBUILD_QFE

#ifdef VER_PRODUCTVERSION_W
#undef VER_PRODUCTVERSION_W
#endif
#define VER_PRODUCTVERSION_W (0x0513)

#ifdef VER_PRODUCTVERSION_DW
#undef VER_PRODUCTVERSION_DW
#endif
#define VER_PRODUCTVERSION_DW (0x05130000 | VER_PRODUCTBUILD)

#endif  //Win2K

#define VER_COMPANYNAME_STR         NV_COMPANY_NAME_STRING
#define VER_LEGALCOPYRIGHT_STR      "Copyright © NVIDIA Corp. 1996-2001"

//
// Strings used in the miniport driver.
//
#define NV_MINI_VER  NV_NTMINIVER
#define NV_MINI_NAME NV_NTMINIVER

//
// Strings used in the display driver.
//
#define NV_DISP_VER  NV_NTDISPVER
#define NV_DISP_NAME NV_NTDISPVER


//
// This hackery is only necessary for NT4, since the NT4 DDK version
// files expect VER_PRODUCTVERSION_STR to be the version number.
//
#if (_WIN32_WINNT < 0x0500)
#if     (VER_PRODUCTBUILD < 10)
#define VER_BPAD "000"
#elif   (VER_PRODUCTBUILD < 100)
#define VER_BPAD "00"
#elif   (VER_PRODUCTBUILD < 1000)
#define VER_BPAD "0"
#else
#define VER_BPAD
#endif

#define VER_PRODUCTVERSION_STRING "4.00"
#define VER_PRODUCTVERSION_STR2(x,y) VER_PRODUCTVERSION_STRING "." VER_BPAD #x "." #y
#define VER_PRODUCTVERSION_STR1(x,y) VER_PRODUCTVERSION_STR2(x, y)

#ifdef VER_PRODUCTVERSION_STR
#undef VER_PRODUCTVERSION_STR
#endif

#define VER_PRODUCTVERSION_STR VER_PRODUCTVERSION_STR1(VER_PRODUCTBUILD, VER_PRODUCTBUILD_QFE)
#endif


