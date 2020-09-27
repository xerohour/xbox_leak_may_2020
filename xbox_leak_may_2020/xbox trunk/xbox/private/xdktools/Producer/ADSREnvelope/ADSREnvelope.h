#if !defined(AFX_ADSRENVELOPE_H__71AE362C_A9BD_11D0_BCBA_00AA00C08146__INCLUDED_)
#define AFX_ADSRENVELOPE_H__71AE362C_A9BD_11D0_BCBA_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ADSREnvelope.h : main header file for ADSRENVELOPE.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopeApp : See ADSREnvelope.cpp for implementation.

class CADSREnvelopeApp : public COleControlModule
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

#endif // !defined(AFX_ADSRENVELOPE_H__71AE362C_A9BD_11D0_BCBA_00AA00C08146__INCLUDED)
