#if !defined(AFX_KEYSOCX_H__CDD09F8C_E73C_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_KEYSOCX_H__CDD09F8C_E73C_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// KeysOCX.h : main header file for KEYSOCX.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CKeysOCXApp : See KeysOCX.cpp for implementation.

class CKeysOCXApp : public COleControlModule
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

#endif // !defined(AFX_KEYSOCX_H__CDD09F8C_E73C_11D0_89AB_00A0C9054129__INCLUDED)
