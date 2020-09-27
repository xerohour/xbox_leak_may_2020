// cafebld.h : main header file for the CAFEBLD application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCAFEBuildApp:
// See cafebld.cpp for the implementation of this class
//

class CCAFEBuildApp : public CWinApp
{
public:
	CCAFEBuildApp();

// operations
public:
	CString GetRegistryKeyName(void)				{ return m_strRegistryKey; }

// operations
protected:
	void ParseCmdLine(LPCSTR pszCmdLine);
	void EliminateLeadingChars(CString &str, CString strSet);
	CString GetToken(CString &str);

	void Usage(LPCSTR pszMessage = NULL);
// data
protected:
	CString m_strRegistryKey;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCAFEBuildApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCAFEBuildApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
