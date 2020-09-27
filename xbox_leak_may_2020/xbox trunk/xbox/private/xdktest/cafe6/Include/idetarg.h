/////////////////////////////////////////////////////////////////////////////
// idetargt.h
//
// email	date		change
// briancr	11/04/94	created
//
// copyright 1994 Microsoft

// Interface of the CTarget class

#ifndef __IDETARGT_H__
#define __IDETARGT_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "settings.h"
#include "guitarg.h"
#include "..\src\support\guitools\ide\shl\coconnec.h"
#include "idexprt.h"
#include "..\src\support\guitools\ide\connec.h"


/////////////////////////////////////////////////////////////////////////////
// CIDE

const CString strIDETargetKey = "IDE";

class IDE_CLASS CIDE: public CGUITarget
{
// ctor/dtor
public:
	CIDE();
	virtual ~CIDE();

// operations
public:
	// launch the IDE
	virtual BOOL Launch(void);

	// exit the IDE
	virtual BOOL Exit(void);

	// active status
	virtual BOOL IsActive(void);

	virtual BOOL SetEnvironmentVar(LPCSTR pszEnvVar, LPCSTR pszValue);
	virtual BOOL GetEnvironmentVar(LPCSTR pszEnvVar, CString &strValue);

	// The following 2 functions enable/disable writing a message to the log that the Exception dilaog was found
	void EnableExceptionMessageHandling(void)	{ m_EnableExceptionMessageHandling = TRUE; }
	void DisableExceptionMessageHandling(void)	{ m_EnableExceptionMessageHandling = FALSE; }

// operations
public:
	virtual BOOL Initialize(CToolset::BUILD_TYPE, CString strKey);
	virtual BOOL ApplySettings(CSettings* pSettings);
	virtual BOOL SetConnections(HWND hwndDlg) = 0;
	// record heap usage information from the IDE
	BOOL RecordHeapUse(void);

// testing operations
public:
	virtual COConnection* GetConnectionInfo(void);

/*
	virtual COSource* NewSourceEditor(void);
	virtual COProject* NewProject(void);
	virtual CODebug* NewDebugger(void);

	virtual COSource* GetSourceEditor(void);
	virtual COProject* GetProject(void);
	virtual CODebug* GetDebugger(void);
*/

// operations (internal)
protected:
	virtual BOOL InitializeSettings(void);

	CString GetKeyExt(void);
	void DeleteKeyExt(void);
	
// data
public:
	DWORD m_dwLaunchTime;
	// each IDE has a single connection info object
	COConnection* m_pConnectionInfo;

protected:
	CConnections m_connec;
	// heap use message id
	static UINT m_msgHeapUse;
	// heap use log path/filename
	CString m_strHeapUseFilename;

protected:
	// the following members are used in conjunction with imehook.dll.
	// see cafe\support\guitools\imehook.cpp (michma - 1/1/97).

	// we use imehook.dll only on nt/j. 
	BOOL m_bRunningOnNTJ;
	// handle to imehook.dll.
	HINSTANCE m_hIMEHookDll;
	// pointers to HookIDE() and UnhookIDE() in imehook.dll.
	void (FAR WINAPI *m_lpfnHookIDE)(HWND);
	void (FAR WINAPI *m_lpfnUnhookIDE)(HWND);
};


/////////////////////////////////////////////////////////////////////////////
// CX86IDE

class IDE_CLASS CX86IDE: public CIDE
{
// ctor/dtor
public:
	CX86IDE();
	virtual ~CX86IDE();
	virtual BOOL Initialize(CToolset::BUILD_TYPE, CString strKey);
	virtual BOOL ApplySettings(CSettings* pSettings);
	virtual BOOL InitializeSettings(void);
	virtual BOOL SetConnections(HWND hwndDlg);
// data
private:
	CSettings *m_psettingsTCPIP;
	CSettings *m_psettingsSerial;
};

/////////////////////////////////////////////////////////////////////////////
// CWin32sIDE

class IDE_CLASS CWin32sIDE: public CX86IDE
{
// ctor/dtor
public:
	CWin32sIDE();
	virtual ~CWin32sIDE();
	virtual BOOL Initialize(CToolset::BUILD_TYPE, CString strKey);
	virtual BOOL ApplySettings(CSettings* pSettings);
	virtual BOOL InitializeSettings(void);
	virtual BOOL SetConnections(HWND hwndDlg);
// data
private:
	CSettings *m_psettingsSerial;
};

/////////////////////////////////////////////////////////////////////////////
// C68KIDE

class IDE_CLASS C68KIDE: public CX86IDE
{
// ctor/dtor
public:
	C68KIDE();
	virtual ~C68KIDE();
	virtual BOOL Initialize(CToolset::BUILD_TYPE, CString strKey);
	virtual BOOL ApplySettings(CSettings* pSettings);
	virtual BOOL InitializeSettings(void);
	virtual BOOL SetConnections(HWND hwndDlg);
// data
private:
	CSettings *m_psettingsTCPIP;
	CSettings *m_psettingsSerial;
	CSettings *m_psettingsAT;
};

/////////////////////////////////////////////////////////////////////////////
// CPMacIDE

class IDE_CLASS CPMacIDE: public CX86IDE
{
// ctor/dtor
public:
	CPMacIDE();
	virtual ~CPMacIDE();
	virtual BOOL Initialize(CToolset::BUILD_TYPE, CString strKey);
	virtual BOOL ApplySettings(CSettings* pSettings);
	virtual BOOL InitializeSettings(void);
	virtual BOOL SetConnections(HWND hwndDlg);
// data
private:
	CSettings *m_psettingsTCPIP;
	CSettings *m_psettingsSerial;
	CSettings *m_psettingsAT;
};

/////////////////////////////////////////////////////////////////////////////
// CMIPSIDE

class IDE_CLASS CMIPSIDE: public CX86IDE
{
// ctor/dtor
public:
	CMIPSIDE();
	virtual ~CMIPSIDE();
};

/////////////////////////////////////////////////////////////////////////////
// CAlphaIDE

class IDE_CLASS CAlphaIDE: public CX86IDE
{
// ctor/dtor
public:
	CAlphaIDE();
	virtual ~CAlphaIDE();
};

/////////////////////////////////////////////////////////////////////////////
// CPowerPCIDE

class IDE_CLASS CPowerPCIDE: public CX86IDE  // Review: PowerMac derives like this as well.  It doesn't really make sense.
{
// ctor/dtor
public:
	CPowerPCIDE();
	virtual ~CPowerPCIDE();
};

#endif // __IDETARGT_H__
