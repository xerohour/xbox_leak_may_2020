#if !defined(AFX_MFCAXCW_H__7EFBEC01_CAA8_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_MFCAXCW_H__7EFBEC01_CAA8_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// mfcaxcw.h : main header file for MFCAXCW.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwApp : See mfcaxcw.cpp for implementation.

class CMfcaxcwApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAXCW_H__7EFBEC01_CAA8_11D0_84BF_00AA00C00848__INCLUDED)
