///////////////////////////////////////////////////////////////////////////////
//	IMEWRAP.H
//
//	Created by :			Date :
//		DavidGa					3/1/94
//
//	Description :
//		Declaration of the CIME class
//

#ifndef __IMEWRAP_H__
#define __IMEWRAP_H__

#include <ime32.h>		// IME header

#include "guixprt.h"

// REVIEW(briancr): we should probably change all these IME functions to take
// a parameter that is the hwnd of the window we want to the settings/attributes
// to be for (instead of using ::GetForegroundWindow()).

/////////////////////////////////////////////////////////////////////////////
// Flags, Enums and Symbols

#define IME_MODES_ALPHABET	(IME_MODE_ALPHANUMERIC | IME_MODE_KATAKANA | IME_MODE_HIRAGANA)
#define IME_MODES_NUMBYTES	(IME_MODE_DBCSCHAR | IME_MODE_SBCSCHAR)
#define IME_MODES_ROMAN		(IME_MODE_ROMAN | IME_MODE_NOROMAN)
#define IME_MODES_INPUT		(IME_MODE_CODEINPUT | IME_MODE_NOCODEINPUT)

// used for calling ImmSetConversionStatus in imehook.dll on nt/j.
// see cafe\support\guitools\imehook.cpp (michma - 1/1/97).
#define	IME_MODE_DB_HIRAGANA	0x19
#define IME_MODE_DB_KATAKANA	0x1B
#define	IME_MODE_DB_ALPHA		0x18
#define	IME_MODE_SB_KATAKANA	0x13
#define IME_MODE_SB_ALPHA		0x10

/////////////////////////////////////////////////////////////////////////////
// CIME class

// BEGIN_CLASS_HELP
// ClassName: CIME
// BaseClass: CWnd
// Category: General
// END_CLASS_HELP
class GUI_CLASS CIME : public CWnd
{
public:
	CIME();
	~CIME();
	#ifdef OLD_IME_BEHAVIOR
	BOOL Create(void);
	#endif // OLD_IME_BEHAVIOR

	DECLARE_DYNCREATE(CIME)

// Data
protected:
	HANDLE m_hIME;				// handle to the IME
	CEdit m_editCtl;
	bool IME_UseFlag ;
	HINSTANCE m_hNLSLibrary;	// handle to the library
	WORD (FAR WINAPI *m_lpfnSendIMEMessage)(HWND, LONG);	// handle to the library
	BOOL (FAR WINAPI *m_lpfnEnableIME)(HWND, BOOL);			// WINNLSEnableIME function pointer
	BOOL (FAR WINAPI *m_lpfnGetEnableStatus)(HWND);			// WINNLSGetEnableStatus function pointer

// Enums
public:
	enum EAlphabets { DONTCARE, ALPHA, KATAKANA, HIRAGANA };
	
// Operations
public:
	BOOL Enable(BOOL bEnable = TRUE);
	BOOL IsEnabled(void);
	BOOL Open(BOOL bOpen = TRUE);
	int IsOpen(void);
	void Flush(HWND hwnd);		// REVIEW: what is the HWND for?
	UINT GetMode(void);
	UINT SetMode(UINT flags);
	UINT SetMode(EAlphabets eAlphabet = ALPHA, UINT nBytes = 1, BOOL bRoman = TRUE, BOOL bCodeInput = FALSE);
	BOOL SendVKey(UINT vKey);
	BOOL SetConversionMode(int nMode);
	void IMEInUse(bool useFlag);
	bool IsIMEInUse() { return IME_UseFlag ; } ;

// Helper functions
protected:
	HWND GetDumbWindow(void);
	void SwitchFocus(void);

// Implementation
public:
#ifdef _DEBUG
	virtual	void AssertValid() const;
	virtual	void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CIME)
	#ifdef OLD_IME_BEHAVIOR
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	#endif // OLD_IME_BEHAVIOR
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	// the following members are used in conjunction with imehook.dll.
	// see cafe\support\guitools\imehook.cpp (michma - 1/1/97).

	// we only use imehook.dll on nt/j.
	BOOL m_bRunningOnNTJ;
	// messages that CIME functions pass to imehook.dll.
	UINT m_msgIMESetOpen;
	UINT m_msgIMEGetOpen;
	UINT m_msgIMESetMode;
	UINT m_msgIMEGetMode;

};

// REVIEW(briancr): this object is declared as a global for backward compatibility
// REVIEW(briancr): this is just to make things work. Is there a better way to do this?
extern GUI_DATA CIME IME;

#endif //__IMEWRAP_H__
