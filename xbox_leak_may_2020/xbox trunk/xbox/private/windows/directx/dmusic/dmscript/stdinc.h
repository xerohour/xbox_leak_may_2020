// Copyright (c) 1999 Microsoft Corporation
#pragma once

#define IDirectSoundWave IUnknown
#define IDirectSoundConnect IUnknown

#include <xutility>

#include "dmusicip.h"
#include "Validate.h"
#include "debug.h"
#include "smartref.h"
#include "miscutil.h"

// undefine min and max from WINDEF.H
// use std::_MIN and std::_MAX instead
#undef min
#undef max

//const g_ScriptCallTraceLevel = -1; // always log
const g_ScriptCallTraceLevel = 4; // only log at level 4 and above

// Pragma reminders
#define QUOTE0(a)           #a
#define QUOTE1(a)           QUOTE0(a)
#define MESSAGE(a)          message(__FILE__ ", line " QUOTE1(__LINE__) ": " a)
#define TODO(a)             MESSAGE("TODO: " a)

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
