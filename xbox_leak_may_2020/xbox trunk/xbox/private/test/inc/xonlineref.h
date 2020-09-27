/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  xonlineref.h

Abstract:

  This module contains the definitions for xonlineref.lib

Author:

  Steven Kehrli (steveke) 21-Jan-2002

------------------------------------------------------------------------------*/



#pragma once


#ifdef _XBOX



#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus



ULONG
WINAPI
XOnlineAddRef();



ULONG
WINAPI
XOnlineRelease();



#ifdef __cplusplus
}
#endif // __cplusplus



#endif // _XBOX
