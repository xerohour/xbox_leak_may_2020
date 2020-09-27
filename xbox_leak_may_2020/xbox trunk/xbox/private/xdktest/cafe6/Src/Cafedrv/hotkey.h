///////////////////////////////////////////////////////////////////////////////
//	HOTKEY.H
//
//	Created by :			Date :
//		DavidGa					8/13/93
//
//	Description :
//		Declaration of the CHotKeyWnd class
//

#ifndef __HOTKEY_H__
#define __HOTKEY_H__
//we need a single instance of any type of CAFE driver because of hotkey clash
#define STARTUPMUTEXNAME "CAFEMutex_5371af09" //say, it's a unique name

// REVIEW(briancr): we need a better type of communication between the
// support system and the single step window. Currently, the support system
// contains code that records information in the single step window and the
// driver contains this code that manages the single step window.
#include "guiv1.h"
#include "settings.h"

///////////////////////////////////////////////////////////////////////////////
// hot key IDs

//#define HOTKEY_RUN			65123
//#define HOTKEY_SINGLE		65124
//#define HOTKEY_STEPOVER		65125
//#define HOTKEY_BREAK		65126
//#define HOTKEY_STEPBREAK		65127
#define HOTKEY_ABORT		65999

///////////////////////////////////////////////////////////////////////////////
// hot key globals
/*
extern AFX_EXT_DATA HANDLE g_hSingleStep;
extern AFX_EXT_DATA HANDLE g_hUserAbort;
extern AFX_EXT_DATA HWND g_hwndHotKey;
enum HotKeyTypes { HOTKEY_RUN, HOTKEY_SINGLE, HOTKEY_STEPOVER, HOTKEY_BREAK, HOTKEY_STEPBREAK};
extern AFX_EXT_DATA HotKeyTypes g_hkToDo;
*/ 
///////////////////////////////////////////////////////////////////////////////
// CHotKeyWnd class

#define WM_REGISTERHOTKEYS WM_USER + 10
#define WM_UNREGISTERHOTKEYS WM_USER + 11

class CHotKeyWnd : public CWnd
{
public:
	CHotKeyWnd();
	~CHotKeyWnd();
	BOOL Create(void);

	DECLARE_DYNCREATE(CHotKeyWnd)

// Implementation
public:
#ifdef _DEBUG
	virtual	void AssertValid() const;
	virtual	void Dump(CDumpContext& dc) const;
#endif
	void RegisterHotKeys(void);
	void UnregisterHotKeys(void);
	CSettings* GetSettings(void) { ASSERT(m_psettingsWnd); return m_psettingsWnd; };
// data
protected:
	CSettings* m_psettingsWnd;

// Generated message map functions
protected:
	//{{AFX_MSG(CHotKeyWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg void OnMove(int x, int y);
	afx_msg void OnSizing( UINT nSide, LPRECT lpRect );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	LRESULT OnRegisterHotKeys(WPARAM wParam, LPARAM lParam);
	LRESULT OnUnregisterHotKeys(WPARAM wParam, LPARAM lParam);
    void SavePosition();
};

///////////////////////////////////////////////////////////////////////////////
// CHotKeyWndThread class

class CHotKeyThread : public CWinThread
{
public:
	CHotKeyThread();
	~CHotKeyThread();

	DECLARE_DYNCREATE(CHotKeyThread)

// Overridables
	// thread initialization
	virtual BOOL InitInstance();

	CWnd* GetHKWnd(void)  { ASSERT(IsWindow(m_wndHK.m_hWnd)); return &m_wndHK; }

// Data
protected:
	CHotKeyWnd m_wndHK;
};

#endif //__HOTKEY_H__
