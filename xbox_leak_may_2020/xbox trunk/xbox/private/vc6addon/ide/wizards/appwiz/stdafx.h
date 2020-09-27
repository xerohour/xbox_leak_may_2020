
#include <afxwin.h>
#ifdef VS_PACKAGE
#include <afxdisp.h>

#define _WIN32_WINNT 0x0400
#define _ATL_APARTMENT_THREADED

#pragma warning(disable:4100) // 'unreferenced formal parameter'

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#endif	// VS_PACKAGE

#if !defined(_AFX_NO_OLE_SUPPORT)
#include <afxole.h>
#endif
#include <afxdlgs.h>
#include <afxext.h>
//#include <ctype.h>
#include <stdlib.h>
#include <afxtempl.h>
#include <afxpriv.h>   // To get main.h to compile

#if !defined(_AFX_NO_DB_SUPPORT)
#include <afxdb.h>
#endif

#include <afxcmn.h>		// Spin control
