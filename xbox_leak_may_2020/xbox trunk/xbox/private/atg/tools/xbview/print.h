//-----------------------------------------------------------------------------
//  
//  File: print.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//
//  Printing to the debug display.
//-----------------------------------------------------------------------------
#pragma once

extern WCHAR *g_strOut;

HRESULT Out(CONST WCHAR *str);
HRESULT Printf(CONST WCHAR *strFormat, ...);
