/***************************************************************************
 *
 *  Copyright (C) 11/8/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       wbndgui.h
 *  Content:    Main header for the Wave Bundler GUI.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/8/2001   dereks  Created.
 *
 ****************************************************************************/

#ifndef __WBNDGUI_H__
#define __WBNDGUI_H__

#define USE_XBOX_COLORS

#define XBOX_BLACK  RGB(0, 0, 0)
#define XBOX_GREEN  RGB(0, 255, 0)

#define WBND_REG_KEY        "Software\\Microsoft\\Microsoft Xbox SDK\\Wave Bundler"
#define WBND_REG_VAL_WNDPOS "Window Placement"

#include "wavbndli.h"
#include <commctrl.h>
#include <shellapi.h>
#include <commdlg.h>
#include <afxres.h>
#include <xboxdbg.h>
#include "resource.h"
#include "wndwrap.h"
#include "mru.h"
#include "listview.h"
#include "sbgui.h"
#include "winmain.h"

#endif // __WBNDGUI_H__
