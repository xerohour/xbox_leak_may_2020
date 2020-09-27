// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     stdafx.h
// Contents: Precompiled header file.  Include all common header files here.
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ==== Windows Header Files ====
#include <windows.h>
#include <commdlg.h>

// ==== C RunTime Header Files ====
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// ==== External Support Header Files ====
#include <imagehlp.h>


// ==== Xbox header files ====

// "xboxdbg.h"      -- Functionality that communicates with Xbox
#include "xboxdbg.h"


// ==== Local Header Files ====
#include "resource.h"
#include "xbwatson.h"
#include "SaveLogFile.h"
#include "InitDM.h"
#include "XboxNotify.h"
#include "GetBreakInfo.h"
#include "ReadWriteBreakInfo.h"
#include "Output.h"

// Event notification dialog box handlers
#include "HandleAssert.h"
#include "HandleException.h"
#include "HandleRIP.h"
