//      Copyright (c) 1996-1999 Microsoft Corporation


#ifdef XBOX
#include <xtl.h>
#else // XBOX
#include <windows.h>
#endif // XBOX

#include <windowsx.h>
#include <stdio.h>
#include "misc.h"

#ifdef XBOX
#ifndef _SECURITY_ATTRIBUTES_
#define _SECURITY_ATTRIBUTES_
typedef struct  _SECURITY_ATTRIBUTES
    {
    DWORD nLength;
    /* [size_is] */ LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
    }	SECURITY_ATTRIBUTES;
#endif // !_SECURITY_ATTRIBUTES_
#endif
