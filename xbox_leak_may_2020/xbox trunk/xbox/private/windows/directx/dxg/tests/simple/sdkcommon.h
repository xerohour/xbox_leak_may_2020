/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    SDKCommon.h

Abstract:

    Common header files and macros for the Xbox SDK

Revision History:

--*/

#if !defined(SDK_HEADER_SDKCOMMON_H)
#define SDK_HEADER_SDKCOMMON_H

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef STRICT
#define STRICT 1
#endif
#undef D3D_OVERLOADS

// Xbox headers

#if defined(XBOX)
    #include <xtl.h>
#else
    #include <windows.h>
#endif


#pragma warning (disable:4850)

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

// __$ReturnUdt is the address of the destination when returning a structure
// It is Microsoft specific so will need to be adjusted to work with other
// compilers. The best way is to declare a local of the correct type and
// then return in.
#define that_ptr __$ReturnUdt

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif //#ifndef SDK_HEADER_RENDERSURFACE_H
