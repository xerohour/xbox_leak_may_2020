#ifndef _STDAFX_H_
#define _STDAFX_H_

//#include <ntos.h>
#include <xtl.h>
#include <xfont.h>
#include <stdio.h>

#include "videoconstants.h"
#include "xboxvideo.h"
#include "usbmanager.h"
#include "linkedlist.h"
#include "xdcitem.h"

// Menus and Commands
#include "menuscreen.h"
#include "rebootcommand.h"
#include "clockmenuscreen.h"
#include "maformatcommand.h"

#define TITLE_NAME      L"XDash Config"

#define BREAK_INTO_DEBUGGER     _asm { int 3 }
#define ARRAYSIZE(a)            (sizeof(a) / sizeof(a[0]))
#define DATA_DRIVE              L"t:"
#define DATA_PATH               DATA_DRIVE L"\\media"

extern "C" {ULONG DebugPrint(PCHAR Format, ...);}

#endif // _STDAFX_H_