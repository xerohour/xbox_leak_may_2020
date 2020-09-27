///////////////////////////////////////////////////////////////////////////////
//  TESTUTIL.H
//
//  Created by :            Date :
//      DavidGa                 8/13/93
//
//  Description :
//      Declaration of global utility functions
//

#ifndef __TESTUTIL_H__
#define __TESTUTIL_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "guixprt.h"

///////////////////////////////////////////////////////////////////////////////
//  Global Data items and enums

typedef enum {TOGGLE_OFF, TOGGLE_ON} TOGGLE_TYPE;

enum {	GL_NONE = -1,		
		GL_STATIC = 0,
		GL_BUTTON = 1,
		GL_EDIT = 2,
		GL_LISTBOX = 3,
		GL_COMBOBOX = 4,
		GL_LISTVIEW = 5,
		GL_TABVIEW = 6,
		GL_TREEVIEW = 7,
		GL_SCROLLBAR = 8,
		GL_SLIDER = 9,
		GL_SPIN = 10,
		GL_STATUSBAR = 11,
		GL_MSVCSTATIC = 12,
		GL_DIALOG = 13,
		GL_CHECKBOX = 14,
		GL_RADIOBUTTON = 15};

#define WAIT_CONTROL_0 0
#define WAIT_CONTROL_1 (WAIT_CONTROL_0 + 1)
#define WAIT_CONTROL_2 (WAIT_CONTROL_0 + 2)
#define WAIT_CONTROL_3 (WAIT_CONTROL_0 + 3)
#define WAIT_CONTROL_4 (WAIT_CONTROL_0 + 4)
#define WAIT_CONTROL_5 (WAIT_CONTROL_0 + 5)
#define WAIT_CONTROL_6 (WAIT_CONTROL_0 + 6)
#define WAIT_CONTROL_7 (WAIT_CONTROL_0 + 7)
#define WAIT_CONTROL_8 (WAIT_CONTROL_0 + 8)
#define WAIT_CONTROL_9 (WAIT_CONTROL_0 + 9)

///////////////////////////////////////////////////////////////////////////////
//  Global utility functions

GUI_API void EditUndo(int count);
GUI_API void EditRedo(int count);
GUI_API BOOL GetDragFullWindows(BOOL *bResult);

//  Process Control
GUI_API void MilliSleep( DWORD milliseconds );

//  Clipboard
// OBSOLETE(briancr): BTHROW is no longer used
GUI_API BOOL EmptyClip( /*BTHROW bThrowFail = DO_THROW */);
GUI_API BOOL GetClipText( CString& rstrTextOut/*, BTHROW bThrowFail = DO_THROW */);
GUI_API BOOL SetClipText( LPCSTR szTextIn/*, BTHROW bThrowFail = DO_THROW */);
GUI_API void PasteKeys(LPCSTR szPasteText/*, BTHROW bThrowFail = DO_THROW*/);
GUI_API void DoKeysSlow(LPCSTR szKeys, WORD wMilliseconds = 0, BOOL bLiteral = FALSE);
GUI_API CString ExtractHotKey(CString Caption);
//  Mouse Actions
GUI_API void ClickMouse(int nButton,HWND hwnd, int cX,  int cY);
GUI_API void DragMouse(int nButton,HWND hwnd1,int cX1,int cY1, HWND hwnd2,int cX2,int cY2);
GUI_API BOOL ScreenToClient(HWND hwnd, CRect* pRect);
GUI_API BOOL ClientToScreen(HWND hwnd, CRect* pRect);

//  Files and Directories
// return values for CompareFile
enum {  CF_SAME = 0,    // The 2 files are the same
		CF_DUPNAME,     // can't compare a file to itself
		CF_CANTOPEN1,   // can't open source file
		CF_CANTOPEN2,   // can't open target file
		CF_DIFFSIZE,    // files are different sizes
		CF_NOTSAME,     // some byte was different
	 };

GUI_API int CompareFiles(LPCSTR strSource, LPCSTR strTarget, int nStart);   
GUI_API void KillFile(LPCSTR szPathName, LPCSTR szFileName = NULL, BOOL bIgnoreReadOnly = FALSE);
GUI_API BOOL KillAllFiles(LPCSTR szPathName, BOOL bRemoveDir = TRUE);
GUI_API BOOL CopyTree(LPCSTR szFrom, LPCSTR szTo);
GUI_API int FindStrInFile(LPCSTR szFileName, LPCSTR szStr, int nStart = 1);

// Menu Access
GUI_API int DoMenu(LPCSTR szMenu1,        LPCSTR szMenu2,        LPCSTR szMenu3 = NULL,
		   LPCSTR szMenu4 = NULL, LPCSTR szMenu5 = NULL, LPCSTR szMenu6 = NULL,
		   LPCSTR szMenu7 = NULL, LPCSTR szMenu8 = NULL, LPCSTR szMenu9 = NULL);
GUI_API int DoMenu(LPCSTR szMenu);
GUI_API BOOL DoSingleMenu(LPCSTR szMenu);

// Other
GUI_API LPCSTR GetText(HWND hwnd, LPSTR sz, int cMax);
GUI_API CString GetText(HWND hwnd);
GUI_API BOOL IsWindowActive(LPCSTR szWndTitle);
GUI_API BOOL WaitMsgBoxText(LPCSTR szExpect, int nSeconds);
GUI_API int WaitForOriginalWnd(HWND hOrig, int Sec);	
GUI_API int GetDlgControlClass(HWND hWnd) ;
GUI_API void DayRandSeed(void);
GUI_API int DayRand(int nRange);
GUI_API BOOL SetFocusToDlgControl(UINT id, HWND hwnd = NULL);

// window manipulation
GUI_API HWND FindFirstChild(HWND hWnd, int nID);
GUI_API HWND FindChildWindow(HWND hWnd, LPCSTR pCaption, LPCSTR pClassType =NULL);
GUI_API DWORD WaitForSingleControl(const DWORD dwProcessId, const int idControl, const DWORD dwTimeout);
GUI_API DWORD WaitForMultipleControls(const DWORD dwProcessId, const int cControls, const int aControls[], const DWORD dwTimeout);

// Dialog navigation support functions
GUI_API BOOL CALLBACK WGetPageHandle(HWND, LPARAM) ;
GUI_API HWND ControlOnPropPage(int iCtrlId) ;
GUI_API HWND WaitForWndWithCtrl(INT ID, DWORD dwMillSec = 3000) ;
// Localization
GUI_API LPCSTR GetLabel(UINT id, HWND hwnd = NULL, BOOL bOkNoExist = FALSE);

enum { 	SYSTEM_NT_4		= 0x01,
		SYSTEM_NT_5		= 0x02,
		SYSTEM_NT		= 0x83,		// SYSTEM_NT_4 | SYSTEM_NT_5 - probably should also include SYSTEM_NT_51
		SYSTEM_WIN_95	= 0x04,  
		SYSTEM_WIN_98	= 0x08,
		SYSTEM_WIN_MILL = 0x10,
		SYSTEM_WIN		= 0x1C,		// SYSTEM_WIN_95 | SYSTEM_WIN_98 | SYSTEM_WIN_MILL
		SYSTEM_DBCS		= 0x20,
		SYSTEM_JAPAN	= 0x40,
		SYSTEM_NT_51	= 0x80,		//	WHISTLER i.e. NT5.1
	 };

GUI_API DWORD GetSystem(void);

GUI_API int GetPowerOfTwo(int i);
GUI_API void TrimString(CString* pStr);
GUI_API int GetLabelNo(HWND hCtl);

enum {VC_PRO, VC_ENT, VC_LRN};
GUI_API DWORD GetProductType(void);
										
// BEGIN_CLASS_HELP
// ClassName: RandomNumberCheckList
// BaseClass: None
// Category: Global
// END_CLASS_HELP
class GUI_CLASS RandomNumberCheckList
{
public:
	// the default constructor is so the class can be contained.
	// just call the (int) constructor afterward to set iMax.
	RandomNumberCheckList(void);
	RandomNumberCheckList(int iMax);
	void SetMax(int iMax);
	int GetNext(void);
	void UndoLast(void);
private:
	int m_iMax;
	CArray<int, int> m_iaUsed;
};


///////////////////////////////////////////////////////////////////////////////
// COApplication class
//
// BEGIN_CLASS_HELP
// ClassName: COApplication
// BaseClass: None
// Category: Global
// END_CLASS_HELP
class GUI_CLASS COApplication

{
private:
	HWND m_hWnd;
public:
	COApplication(void) {m_hWnd = NULL;}
	virtual BOOL Attach(LPCSTR szAppTitle = NULL, int secWait = 0);
	virtual BOOL Activate(void);
	virtual void SendKeys(LPCSTR szKeys);
};

#endif //__TESTUTIL_H__
