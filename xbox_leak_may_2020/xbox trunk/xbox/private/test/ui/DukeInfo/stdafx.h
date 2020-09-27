#ifndef _STDAFX_H_
#define _STDAFX_H_

#define BREAK_INTO_DEBUGGER     _asm { int 3 }
#define ARRAYSIZE(a)            (sizeof(a) / sizeof(a[0]))
#define XFONT_TRUETYPE

#include <ntos.h>
#include <xtl.h>
#include <xfont.h>
#include <xdbg.h>
#include <stdio.h>

#include "constants.h"
#include "linkedlist.h"
#include "xboxvideo.h"
#include "usbmanager.h"
#include "xitem.h"

// Menus and Commands

#endif // _STDAFX_H_