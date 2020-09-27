//---------------------------------------------------------------------------
// Microsoft Visual Studio
//
// Microsoft Confidential
// Copyright (C) 1994 - 2000 Microsoft Corporation. 
// All Rights Reserved.
//
// VsMemAtl.H
//
// Redefining SysAlloc* and   CoTaskMemAlloc* to a debug functions to trace 
// memory leask in code using ATL
//---------------------------------------------------------------------------
#pragma once

#if DEBUG

//=----------------------------------------------------------------------=
// Debug allocation routines.

#include "vsassert.h"

#define CoTaskMemAlloc(cb)        VsDebOleAlloc(cb)
#define CoTaskMemRealloc(pv, cb)  VsDebOleRealloc(pv, cb)
#define CoTaskMemFree(pv)         VsDebOleFree(pv)

#define SysAllocString(str)             VsDebSysAllocString((str))
#define SysAllocStringByteLen(str, cb)  VsDebSysAllocStringByteLen((str), (cb))
#define SysAllocStringLen(str, cch)     VsDebSysAllocStringLen((str), (cch))

#endif // DEBUG



