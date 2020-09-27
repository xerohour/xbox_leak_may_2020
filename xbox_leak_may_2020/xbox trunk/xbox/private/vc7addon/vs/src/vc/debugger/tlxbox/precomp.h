/*++

Copyright (c) Microsoft Corporation

Module Name:

    precomp.h

Abstract:
    
    Precompiled header for the VC7 debugger plug-in for Xbox.
    
--*/

//Standard Windows\C\C++ stuff
#include <windows.h>
#include <process.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include <queue>

//VC7 header files.
#include <internalnt.h>
#include <odtypes.h>
#include <od.h>
#include <odp2.h>
#include <emdm.h>
#include <vcexcept.h>

//Xbox Headers
#include <xboxdbg.h>

// Useful macros
#define ARRAYSIZE(_x_) (sizeof(_x_)/sizeof(_x_[0]))

//Header Files for this project
#include "debug.h"
#include "utils.h"
#include "dmxbox.h"
#include "events.h"
#include "xprocess.h"

