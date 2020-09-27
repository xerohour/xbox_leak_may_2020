//-----------------------------------------------------------------------------
//  
//  File: print.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//
//  Printing to the debug display.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <stdio.h>
#include "Print.h"

#define MAX_OUT_LENGTH 10000
static WCHAR s_strOutBuf[MAX_OUT_LENGTH] = { 0 };	// start with null-terminated string
WCHAR *g_strOut = s_strOutBuf;	// debugging output string

//-----------------------------------------------------------------------------
// Name: Out
// Desc: Append a string to the debug display
//-----------------------------------------------------------------------------
HRESULT Out(CONST WCHAR *str)
{
	// append the current string
	wcsncat(g_strOut, str, MAX_OUT_LENGTH - 1);	// silently clip extra chars
	g_strOut[MAX_OUT_LENGTH - 1] = 0;	// make sure the string is terminated
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Printf
// Desc: Print a formatted string to the debug display
//-----------------------------------------------------------------------------
HRESULT Printf(CONST WCHAR *strFormat, ...)
{
	va_list val;
	va_start(val, strFormat);
	const buflen = 1000;
	WCHAR buf[buflen];
	_vsnwprintf(buf, buflen, strFormat, val);
	return Out(buf);
}
