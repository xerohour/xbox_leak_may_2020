#if !defined(AFX_REGIONKEYBOARD_H__36CD318D_EE61_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_REGIONKEYBOARD_H__36CD318D_EE61_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// RegionKeyboard.h : main header file for REGIONKEYBOARD.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardApp : See RegionKeyboard.cpp for implementation.

class CRegionKeyboardApp : public COleControlModule
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

#endif // !defined(AFX_REGIONKEYBOARD_H__36CD318D_EE61_11D0_876A_00AA00C08146__INCLUDED)
