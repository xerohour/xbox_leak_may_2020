/************************************************************************
*                                                                       *
*   mswteError.h --  error code definitions for Web Telephony Engine.   *
*                                                                       *
*   Copyright (c) 1991-1999, Microsoft Corp. All rights reserved.       *
*                                                                       *
************************************************************************/

#ifndef MSWTEERROR_H_INCLUDED
#define MSWTEERROR_H_INCLUDED

   
#include <WinError.h>

#define WTE_ERR(_code) MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, _code)

#define E_WTE_NO_STORAGE                          WTE_ERR(0x200)

#define E_WTE_OBJECT_INVALID                      WTE_ERR(0x201)

#define E_WTE_CORRUPTED_OBJECT_DATA               WTE_ERR(0x202)

#define E_WTE_INVALID_SERVER_NAME                 WTE_ERR(0x203)

#define E_WTE_STRING_CONTAINS_INVALID_CHARACTERS  WTE_ERR(0x204)

#define E_WTE_EMPTY_STRING                        WTE_ERR(0x205)

#define E_WTE_VALUE_TOO_LARGE                     WTE_ERR(0x206)

#define E_WTE_VALUE_TOO_SMALL                     WTE_ERR(0x207)

#define E_WTE_CREATE_CHILD_INSTANCE_FAILED        WTE_ERR(0x208)

#endif // ! MSWTEERROR_H_INCLUDED


