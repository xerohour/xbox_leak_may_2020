/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  xnetrefp.h

Abstract:

  This module contains additional definitions for xnetref.lib

Author:

  Steven Kehrli (steveke) 24-May-2001

Environment:

  XBox

Revision History:

------------------------------------------------------------------------------*/



#ifdef _XBOX



#ifndef _XNETREFP_H
#define _XNETREFP_H



#include <xtl.h>
#include <winsockp.h>



#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus



ULONG
WINAPI
XNetRefGetParams(
    XNetStartupParams  *pXNetRefStartupParams,
    XNetConfigParams   *pXNetRefConfigParams
);



#ifdef __cplusplus
}
#endif // __cplusplus



#endif // _XNETREFP_H



#endif // _XBOX
