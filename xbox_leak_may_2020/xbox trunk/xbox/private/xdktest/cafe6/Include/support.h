///////////////////////////////////////////////////////////////////////////////
//	support.h
//
//	Created by :			Date :
//		BrianCr				12/05/94
//
//	Description :
//		Precompiled header for all of the CAFE support files
//
//  	Other projects may include CAFE.H in their own precompiled headers,
//		and can then avoid including any other CAFE headers, (or for that 
//		matter <afxwin.h> or <afxext.h>)
//

#ifndef __SUPPORT_H__
#define __SUPPORT_H__

#define __CAFE_H__
#define __WB_H__

#pragma warning(disable: 4275)

// log
#include "log.h"

// target
#include "settings.h"
#include "testxcpt.h"

// gui
#include "mstwrap.h"		// MFC-ized wrapper for MS Test
#include "..\src\support\guitools\imewrap.h"		// wrapper for the Japanese IME
#include "..\src\support\guitools\testutil.h"		// base level utility/cases class
#include "..\src\support\guitools\uwindow.h"		// basis of UIWindow utility classes
#include "..\src\support\guitools\udialog.h"
#include "..\src\support\guitools\umsgbox.h"
#include "..\src\support\guitools\coclip.h"			// Component Object classes

// ide
#include "..\src\support\guitools\ide\sym\cmdids.h"
#include "..\src\support\guitools\ide\sym\menuids.h"
#include "..\src\support\guitools\ide\sym\dockids.h" // taken directly from sushi project
#include "..\src\support\guitools\ide\sym\qcqp.h"   // munged together from sushi project
#include "..\src\support\guitools\ide\sym\vshell.h"
#include "..\src\support\guitools\ide\sym\vres.h"
#include "..\src\support\guitools\ide\sym\vcpp32.h"
#include "..\src\support\guitools\ide\sym\vproj.h"
#include "..\src\support\guitools\ide\sym\shrdres.h"
#include "..\src\support\guitools\ide\sym\appwz.h"
#include "..\src\support\guitools\ide\sym\clswiz.h"
#include "..\src\support\guitools\ide\sym\optnbsc.h"	// options id's
#include "..\src\support\guitools\ide\sym\optncplr.h"
#include "..\src\support\guitools\ide\sym\optnlib.h"
#include "..\src\support\guitools\ide\sym\optnlink.h"
#include "..\src\support\guitools\ide\sym\optnmtl.h"
#include "..\src\support\guitools\ide\sym\optnrc.h"
#include "..\src\support\guitools\ide\sym\projprop.h"	// Project specific properites
#define __UNDO_H__			// Prevents #inclusion of some SUSHI headers
#define __SLOBWND_H__
#include "..\src\support\guitools\ide\sym\slob.h"		// some ID's were moved from props.h
#include "..\src\support\guitools\ide\sym\68k\optncplr.h"
#include "..\src\support\guitools\ide\sym\68k\optnmrc.h"
#include "..\src\support\guitools\ide\filetabl.h"		// File table struct and utilities for table driven tests

// shl
#include "..\src\support\guitools\ide\shl\umainfrm.h"
#include "..\src\support\guitools\ide\shl\wb_props.h"   // defines all workbench properties used by GetProperty() function
#include "Strings.h"									// IDSS_ localized string IDs
#include "..\src\support\guitools\ide\shl\wbutil.h"		// Workbench-specific global utilities
#include "..\src\support\guitools\ide\shl\uwbframe.h"   // UWindow based utility classes
#include "..\src\support\guitools\ide\shl\ucommdlg.h"
#include "..\src\support\guitools\ide\shl\upropwnd.h"
#include "..\src\support\guitools\ide\shl\udockwnd.h"
#include "..\src\support\guitools\ide\shl\utbardlg.h"
#include "..\src\support\guitools\ide\shl\utoolbar.h"
#include "..\src\support\guitools\ide\shl\uitabdlg.h"
#include "..\src\support\guitools\ide\shl\uioptdlg.h"
#include "..\src\support\guitools\ide\shl\ucustdlg.h"
#include "..\src\support\guitools\ide\shl\uioutput.h"
#include "..\src\support\guitools\ide\shl\uiwbmsg.h"
#include "..\src\support\guitools\ide\shl\uiwbdlg.h"
#include "..\src\support\guitools\ide\shl\uiconnec.h"
#include "..\src\support\guitools\ide\shl\uiconset.h"
#include "..\src\support\guitools\ide\shl\coconnec.h"
#include "..\src\support\guitools\ide\shl\cowrkspc.h"
#include "..\src\support\guitools\ide\shl\coenv.h"

// eds
#include "..\src\support\guitools\ide\eds\ueditor.h"
#include "..\src\support\guitools\ide\eds\ufindinf.h"
#include "..\src\support\guitools\ide\eds\cofile.h"		// Component-Object based utility classes.
#include "..\src\support\guitools\ide\eds\macrodlg.h"

// src
#include "..\src\support\guitools\ide\src\cosource.h"
#include "..\src\support\guitools\ide\src\cohtml.h"
#include "..\src\support\guitools\ide\src\uinsctrl.h"
#include "..\src\support\guitools\ide\src\ufindrep.h"

// res
#include "..\src\support\guitools\ide\res\uresedit.h"
#include "..\src\support\guitools\ide\res\urestabl.h"
#include "..\src\support\guitools\ide\res\uresbrws.h"
#include "..\src\support\guitools\ide\res\ustredit.h"
#include "..\src\support\guitools\ide\res\uaccedit.h"
#include "..\src\support\guitools\ide\res\uveredit.h"
#include "..\src\support\guitools\ide\res\udlgedit.h"
#include "..\src\support\guitools\ide\res\uimgedit.h"
#include "..\src\support\guitools\ide\res\umenedit.h"
#include "..\src\support\guitools\ide\res\usymdlg.h"
#include "..\src\support\guitools\ide\res\coresscr.h"

// prj
#include "..\src\support\guitools\ide\prj\uprjfile.h"
#include "..\src\support\guitools\ide\prj\uitarget.h"
#include "..\src\support\guitools\ide\prj\uogaldlg.h"
#include "..\src\support\guitools\ide\prj\uprjtdlg.h"
#include "..\src\support\guitools\ide\prj\uprojwiz.h"
#include "..\src\support\guitools\ide\prj\uiextwiz.h"
#include "..\src\support\guitools\ide\prj\uappwiz.h"
#include "..\src\support\guitools\ide\prj\ucwzdlg.h"
#include "..\src\support\guitools\ide\prj\utstapwz.h"
#include "..\src\support\guitools\ide\prj\odbcdlgs.h"
#include "..\src\support\guitools\ide\prj\uictrlwz.h"
#include "..\src\support\guitools\ide\prj\uiwizbar.h"
#include "..\src\support\guitools\ide\prj\coprject.h"
#include "..\src\support\guitools\ide\prj\cocompon.h"

// wrk
#include "..\src\support\guitools\ide\wrk\uiwrkspc.h"

// dbg
#include "..\src\support\guitools\ide\dbg\parse.h"		//Utility
#include "..\src\support\guitools\ide\dbg\uiqw.h"		// User-Interface based utility classes.
#include "..\src\support\guitools\ide\dbg\uibp.h"
#include "..\src\support\guitools\ide\dbg\uimem.h"
#include "..\src\support\guitools\ide\dbg\uieewnd.h"
#include "..\src\support\guitools\ide\dbg\uiwatch.h"
#include "..\src\support\guitools\ide\dbg\uivar.h"
#include "..\src\support\guitools\ide\dbg\uilocals.h"
#include "..\src\support\guitools\ide\dbg\uireg.h"
#include "..\src\support\guitools\ide\dbg\uicp.h"
#include "..\src\support\guitools\ide\dbg\uidam.h"
#include "..\src\support\guitools\ide\dbg\uiexcpt.h"
#include "..\src\support\guitools\ide\dbg\uistack.h"
#include "..\src\support\guitools\ide\dbg\uithread.h"
#include "..\src\support\guitools\ide\dbg\uibrowse.h"
#include "..\src\support\guitools\ide\dbg\uidebug.h"
#include "..\src\support\guitools\ide\dbg\uiattach.h"
#include "..\src\support\guitools\ide\dbg\coee.h"
#include "..\src\support\guitools\ide\dbg\cobp.h"
#include "..\src\support\guitools\ide\dbg\comem.h"
#include "..\src\support\guitools\ide\dbg\codebug.h"
#include "..\src\support\guitools\ide\dbg\cocp.h"
#include "..\src\support\guitools\ide\dbg\coreg.h"
#include "..\src\support\guitools\ide\dbg\costack.h"
#include "..\src\support\guitools\ide\dbg\cothread.h"
#include "..\src\support\guitools\ide\dbg\coexcpt.h"
#include "..\src\support\guitools\ide\dbg\codam.h"
#include "..\src\support\guitools\ide\dbg\cobrowse.h"
#include "..\src\support\guitools\ide\dbg\colocals.h"
#include "..\src\support\guitools\ide\dbg\cowatch.h"
#include "..\src\support\guitools\ide\dbg\coauto.h"

// backward compatibility
#include "guiv1.h"

#endif //__SUPPORT_H__
