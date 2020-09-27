// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//		are changed infrequently
//

#pragma warning(disable: 4100)

#include "vshell.h"
#include "srcguid.h"
#include "prjapi.h"
#include "prjguid.h"
#include "bldguid.h"
#include "utilguid.h"
#include "clvwguid.h"

#define USE_CPLUSPLUS_TOKENS
#include "srcapi.h"

#include "bldapi.h"
#include "utilapi.h"
#include "clvwapi.h"

#include <windowsx.h>
#include <errno.h>
#include <fcntl.h>
#include <share.h>
#include <tchar.h>
#include "hash.h"
#include "inc\datatip.h"
#include "inc\idedoc.h"
#include "inc\ideview.h"
#include <shlbar.h>
#include <shldocs.h>
#include <shlfutr_.h>
#include <shlmenu.h>
#include <shlsrvc.h>
#include <util.h>
#include <proppage.h>
#include <dlgbase.h>

#include "types.h"                      // in Langapi
#include <od4.h>
#include <cvinfo.h>                     // in Langapi
#include <shapi.h>                    // in Langapi
#include <symapi.h>						// in Langapi 
#include "symimpl.h"
#include "od.h"                         // in Langapi
#include "tlui.h"                       // in Langapi
#include "resource.h"
#include "qcqp.h"
#include "brkpt.h"
#include "cmgrnew.h"
#include "cmgrlow.h"
#include "cdebug.h"
#include "cl.h"
#include "cp.h"
#include "cvinc\cp.hmd"
#include "dbgnew.h"
#include "colors.h"
#include "dbgguid.h"
#include "dbgpguid.h"
#include "dbgpkapi.h"
#include "dbgpkg.h"
#include "cvinc\sy.hmd"
#include "inc\dbg.h"
#include "inc\fonts.h"
#include "inc\meclass.h"
#include "inc\mw.h"
#include "inc\rwv.h"
#include "inc\treegrid.h"
#include "inc\dbggrid.h"
#include "inc\varsview.h"
#include "inc\qwatch.h"
#include "inc\eetm.h"
#include "inc\watchrow.h"
#include "inc\extern.h"
#include "inc\breakpts.h"
#include "inc\makeeng.h"
#include "inc\ambig.h"
#include "inc\askpath.h"
#include "cvinc\brkpt.hmd"
#include "inc\bpdlgex.h"
#include "cvinc\cvwin32.h"
#include "cvinc\newexe.h"
#include "inc\excep.h"
#include "inc\rundebug.h"
#include "inc\tracefn.h"
#include "inc\retvals.h"
#include "inc\miscdlgs.h"
#include "cvinc\linklist.hmd"
#include "cvinc\debtimer.h"
#include "inc\debugopt.h"
#include "inc\eetm.h"
#include "inc\lnklst.h"
#include "inc\thread.h"
#include "inc\ldouble.h"
#include "inc\keycheck.h"
#include "inc\dam.h"
#include "inc\clw.h"
#include "inc\gotoitem.h"
#include "inc\stsfile.h"
#include "inc\owinwrap.h"
#include <afxctl.h>
#include <image.h>

