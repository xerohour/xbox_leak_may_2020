/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    stdafx.h

Abstract:

    This is a pre-compiled header module.

--*/

#if !defined(AFX_STDAFX_H__7A5FDEFE_A50D_49AD_8B8A_0833ACCC7FAB__INCLUDED_)
#define AFX_STDAFX_H__7A5FDEFE_A50D_49AD_8B8A_0833ACCC7FAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <crtdbg.h>
#include <windows.h>

typedef long NTSTATUS;

#include <xprofp.h>
#include <xboxdbg.h>
#include <dbghelp.h>

#include "ptrmap.h"
#include "symbol.h"
#include "capdata.h"

// error functions
VOID errmsg(CHAR* format, ...);
VOID checkerr(BOOL err, CHAR* format, ...);

extern bool verbose;
extern bool noidle;
extern CHAR* symbolpath;

// dump a binary profile data file to human-readable text format
void capdump(CHAR* cap_filename, CHAR* dump_filename);

// spew error debug message if verbose mode is enabled
#define VERBOSE !verbose ? (void)0 : errmsg

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7A5FDEFE_A50D_49AD_8B8A_0833ACCC7FAB__INCLUDED_)
