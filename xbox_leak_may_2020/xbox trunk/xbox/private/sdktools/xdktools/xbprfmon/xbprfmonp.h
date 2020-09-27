/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xbprfmonp.h

Abstract:

    This is a pre-compiled header file for Xbox PerfMon

--*/

#include <stdio.h>
#include <windows.h>
#include <shellapi.h>
#include <xboxdbg.h>
#include <tchar.h>
#include <stdlib.h>

#define ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))

#define chINRANGE(low, Num, High) (((low) <= (Num)) && ((Num) <= (High)))

#define chFAIL(szMSG) {                                                   \
      MessageBox(GetActiveWindow(), szMSG,                                \
         __TEXT("Assertion Failed"), MB_OK | MB_ICONERROR);               \
      DebugBreak();                                                       \
   }

//
// Put up an assertion failure message box.
//

#define chASSERTFAIL(file,line,expr) {                                    \
      TCHAR __sz[128];                                                      \
      wsprintf(__sz, __TEXT("File %hs, line %d : %hs"), file, line, expr);  \
      chFAIL(__sz);                                                         \
   }

//
// Put up a message box if an assertion fails in a debug build.
//

#ifdef _DEBUG
#define chASSERT(x) if (!(x)) chASSERTFAIL(__FILE__, __LINE__, #x)
#else
#define chASSERT(x)
#endif

//
// Assert in debug builds, but don't remove the code in retail builds.
//

#ifdef _DEBUG
#define chVERIFY(x)         chASSERT(x)
#else
#define chVERIFY(x)         (x)
#endif

#define XBPRFMON_UUID_STR   _T("e0b8131e-091c-4f6b-914e-353a0b09996c")

#include "Optex.h"
#include "PrfData.h"
#include "PrfDataMap.h"

