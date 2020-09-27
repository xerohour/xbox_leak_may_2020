/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  datamask.h

Abstract:

  This module contains the definitions for datamask.lib

Author:

  Steven Kehrli (steveke) 4-Jan-2002

Environment:

  XBox

Revision History:

------------------------------------------------------------------------------*/



#ifndef _DATAMASK_H
#define _DATAMASK_H



#ifdef _XBOX
#include <xtl.h>
#else  // XBOX
#include <windows.h>
#endif // XBOX



#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus



VOID
WINAPI
DataMask(
    BYTE  *pBuffer,
    UINT  nBufferSize
);



#ifdef __cplusplus
}
#endif // __cplusplus



#endif // _DATAMASK_H
