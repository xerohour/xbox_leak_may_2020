#if !defined(AFX_GRID_H__EAB0CD4B_9459_11D0_8C10_00A0C92E1CAC__INCLUDED_)
#define AFX_GRID_H__EAB0CD4B_9459_11D0_8C10_00A0C92E1CAC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Grid.h : main header file for GRID.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CGridApp : See Grid.cpp for implementation.

class CGridApp : public COleControlModule
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

#endif // !defined(AFX_GRID_H__EAB0CD4B_9459_11D0_8C10_00A0C92E1CAC__INCLUDED)
