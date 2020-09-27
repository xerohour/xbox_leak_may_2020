// stdafx.h : include file for standard system include files,
//		or project specific include files that are used
//		frequently, but are changed infrequently
//

#define VC_EXTRALEAN
#define NO_ANSIUNI_ONLY


#include <afxwin.h>		// MFC core and standard components
#include <afxext.h>		// MFC extensions

//include these two since VC_EXTRALEAN removes then but they are needed.
#include <winreg.h>
#include <winnls.h>

#include <malloc.h>		
#include <afxole.h>
#include <afxpriv.h>	// MFC private definitions (like WM_COMMANDHELP)
#include <mbctype.h>
#include <tchar.h>
#include <ctl3d.h>		// 3-D dialog controls
#include <winperf.h>	// performance registry access defines
#include <commctrl.h>	// comm controls (Windows95 new controls)
#include "tlhelp32.h"	// ToolHelp32 APIs (Windows95 process/thread info)

#include "resource.h"
#include "spyxxhk.h"
#include "ime32.h"		// should get this from a common global project!
#include "mstream.h"
#include "msgcrack.h"
#include "tabdlg.h"
#include "prfdb.h"
#include "procdb.h"
#include "property.h"
#include "propinsp.h"
#include "spyxx.h"
