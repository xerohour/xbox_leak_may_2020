/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#define _WIN32_DCOM
#include "winsock2.h"
#include <windows.h>
#include <stdio.h>
#include <crtdbg.h>
#include <atlbase.h>

//QWORD Macros
#define MAKEULONGLONG(a, b) ((ULONGLONG)(((DWORD)(a)) | ((ULONGLONG)((DWORD)(b))) << 32))
#define MAKEQWORD(a, b)     MAKEULONGLONG((a),(b))
#define LODWORD(l)          ((DWORD)(l))
#define HIDWORD(l)          ((DWORD)(((ULONGLONG)(l) >> 32) & 0xFFFFFFFF))
typedef ULONGLONG           QWORD;
typedef QWORD               *LPQWORD;

//Other Macros & Definitions
#define RANDOMDWORD ((rand()%0xFF<<24)+(rand()%0xFF<<16)+(rand()%0xFF<<8)+rand()%0xFF)
#define TICKLEPORT 1001