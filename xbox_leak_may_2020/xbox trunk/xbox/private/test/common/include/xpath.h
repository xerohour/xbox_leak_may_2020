//-----------------------------------------------------------------------------
// File: xpath.h
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __XPATH_H
#define __XPATH_H

#include <tchar.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UNICODE
#define _FN(a) XPath_GetFileNameT(a, NULL)
#else
#define _FN(a) XPath_GetFileNameA(a, NULL)
#endif

#define _FNA(a) XPath_GetFileNameA(a, NULL)

void XPath_SetBasePath(TCHAR *path);
TCHAR *XPath_GetFileNameT(TCHAR *in, TCHAR *out=NULL);
char *XPath_GetFileNameA(char *in, char *out=NULL);

#ifdef __cplusplus
}
#endif
#endif