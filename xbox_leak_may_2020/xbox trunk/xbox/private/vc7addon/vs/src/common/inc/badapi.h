// badapi.h
// Disallow functions not implemented on Win95/98
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// June 1, 1998 [paulde]
//

#pragma once
#ifndef __BADAPI_H__
#define __BADAPI_H__

#define API_NOT_IMPL(api) (__##api##_not_implemented95__)
#define DONT_USE(badapi, goodapi)    (__DO_NOT_USE_##badapi##__USE##goodapi##__)

#define lstrcmpW             API_NOT_IMPL(lstrcmpW)
#define lstrcmpiW            API_NOT_IMPL(lstrcmpiW)
#define lstrcpyW             API_NOT_IMPL(lstrcpyW)
#define lstrcpynW            API_NOT_IMPL(lstrcpynW)
#define LCMapStringW         API_NOT_IMPL(LCMapStringW)
#define FoldStringW          API_NOT_IMPL(FoldStringW)
#define GetLocaleInfoW       API_NOT_IMPL(GetLocaleInfoW)

#endif // __BADAPI_H__
