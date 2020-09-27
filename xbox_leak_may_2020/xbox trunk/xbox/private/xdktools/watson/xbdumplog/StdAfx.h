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

// "xboxdbg.h"      -- Functionality that communicates with Xbox.  Necessary for structures...
#include "xboxdbg.h"


// ==== xbWatson Header Files ====
#include "..\xbWatson\Resource.h"
#include "..\xbWatson\GetBreakInfo.h"


// ==== Local Header Files ====
#include "xbDumpLog.h"
#include "ParseLogFile.h"
#include "ReadWriteBreakInfo.h"
#include "Output.h"
