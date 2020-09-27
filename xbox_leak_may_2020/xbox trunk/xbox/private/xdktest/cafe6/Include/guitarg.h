/////////////////////////////////////////////////////////////////////////////
// guitargt.h
//
// email	date		change
// briancr	11/04/94	created
//
// copyright 1994 Microsoft

// Interface of the CTarget class

#ifndef __GUITARGT_H__
#define __GUITARGT_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "settings.h"
#include "target.h"

#include "guixprt.h"

extern GUI_DATA HANDLE g_hTargetProc;


// forward declaration
class CToolset;

/////////////////////////////////////////////////////////////////////////////
// CGUITarget

class GUI_CLASS CGUITarget: public CTarget
{
    CString m_strTitle;

// ctor/dtor
public:
	CGUITarget(const CString& );
	virtual ~CGUITarget();

// operations
public:
	// launch the target
	virtual BOOL Launch(void);
	virtual BOOL Attach(void);
	
	// exit the target
	virtual BOOL Exit(void);

	// active status
	virtual BOOL IsActive(void);

// attributes
public:
	virtual BOOL Initialize(CToolset::BUILD_TYPE, CString strKey) = 0;

	virtual BOOL ApplySettings(CSettings* pSettings);

// operations (internal)
protected:
	virtual HWND FindTarget(CString strClass, DWORD dwProcessId = 0);
	virtual BOOL InitializeSettings(void);

  // create the target's process
  virtual BOOL CreateTargetProcess(LPCSTR szCmdLine, DWORD dwCreationFlags, LPCSTR szEnvBlock, LPCSTR szWorkDir);

// data
public:
	// REVIEW(briancr): shouldn't this class really contain a UIWindow class or something, instead of an HWND?
	HWND m_hWnd;
};

#endif // __GUITARGT_H__
