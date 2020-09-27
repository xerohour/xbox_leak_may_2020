///////////////////////////////////////////////////////////////////////////////
//	IMEWRAP.CPP
//
//	Created by :			Date :
//		DavidGa					3/1/94
//
//	Description :
//		Implementation of the CIME class
//

#include "stdafx.h"
#include "mstwrap.h"
#include "guiv1.h"
#include "testutil.h"
#include "imewrap.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// REVIEW(briancr): this object is declared as a global for backward compatibility
// REVIEW(briancr): this is just to make things work. Is there a better way to do this?
// instantiate a global CIME
GUI_DATA CIME IME;

// REVIEW(briancr): we should probably change all these IME functions to take
// a parameter that is the hwnd of the window we want to the settings/attributes
// to be for (instead of using ::GetForegroundWindow()).

/////////////////////////////////////////////////////////////////////////////
// CIME class

IMPLEMENT_DYNCREATE(CIME, CWnd)

BEGIN_MESSAGE_MAP(CIME, CWnd)
	//{{AFX_MSG_MAP(CIME)
	#ifdef OLD_IME_BEHAVIOR
	ON_WM_CREATE()
	#endif // OLD_IME_BEHAVIOR
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CIME::CIME()
{
	m_hIME = GlobalAlloc (GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT, 
			(LONG)sizeof(IMESTRUCT));
	m_hNLSLibrary = LoadLibrary ("USER32.DLL");

	if( m_hNLSLibrary )
	{
		m_lpfnSendIMEMessage = (WORD (FAR WINAPI *)(HWND, LONG))GetProcAddress ((HINSTANCE)m_hNLSLibrary, "SendIMEMessageExA");
		m_lpfnEnableIME = (BOOL (FAR WINAPI *)(HWND, BOOL))GetProcAddress ((HINSTANCE)m_hNLSLibrary, "WINNLSEnableIME");
		m_lpfnGetEnableStatus = (BOOL (FAR WINAPI *)(HWND))GetProcAddress ((HINSTANCE)m_hNLSLibrary, "WINNLSGetEnableStatus");
	}
	else
	{
		GlobalFree(m_hIME);
		m_hIME = NULL;
	}
	IME_UseFlag = FALSE ;

	// determine if we are running on nt/j.
	if(m_bRunningOnNTJ = (GetSystem() & SYSTEM_NT) && (GetSystem() & SYSTEM_JAPAN))
		{	
		// register messages for communicating with the IDE'S IME on nt/j. 
		// see cafe\support\guitools\imehook.cpp - michma 1/1/97.
		m_msgIMESetOpen = RegisterWindowMessage("msgIMESetOpen");
		m_msgIMEGetOpen = RegisterWindowMessage("msgIMEGetOpen");
		m_msgIMESetMode = RegisterWindowMessage("msgIMESetMode");
		m_msgIMEGetMode = RegisterWindowMessage("msgIMEGetMode");
		}
}

CIME::~CIME()
{
}

/////////////////////////////////////////////////////////////////////////////
// CIME diagnostics

#ifdef _DEBUG
void CIME::AssertValid() const
{
	CWnd::AssertValid();
}

void CIME::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIME message handlers

#ifdef OLD_IME_BEHAVIOR

BOOL CIME::Create(void)
{
	return CWnd::CreateEx(
		WS_EX_DLGMODALFRAME,	// extended style
		AfxRegisterWndClass(CS_GLOBALCLASS),
		"IME Activator Wnd",	// window name
		WS_POPUPWINDOW | WS_VISIBLE,	// style
		50,0,		// window pos
		100, 20,	  //wnd size
		NULL,		// parent window,
		NULL,		// nlDorHMenu
		NULL		// lpPararm
	);
}

int CIME::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;
	CRect rcClient;
	GetClientRect( rcClient );
	m_editCtl.Create( WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, rcClient, this, 0x0004);

	return 0;
}

#endif // OLD_IME_BEHAVIOR

//  IME Operations

// BEGIN_HELP_COMMENT
// Function: BOOL CIME::Enable(BOOL bEnable /*=TRUE*/)
// Description: Enable or disable the IME for input. The IME must be open to call this function (see CIME::Open).
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: bEnable A Boolean value that specifies whether the enable the IME (TRUE) or disable it. (Default value is TRUE.)
// END_HELP_COMMENT
BOOL CIME::Enable(BOOL bEnable /*=TRUE*/)		//REVIEW(EnriqueP): Do we need to switch focus to window?
{
	BOOL bRet = FALSE;
	IMEInUse(TRUE); // SET in use flag ;

	// REVIEW (michma): on nt/j we do not distinguish between the open status
	// and the enabled status of the IME because we use a different api (Imm functions)
	// which appear not to support the concept.
	// see cafe\support\guitools\imehook.cpp - michma 1/1/97.
	if(m_bRunningOnNTJ)
		{
		WaitStepInstructions("%s IME", bEnable ? "Enable" : "Disable");
		return TRUE;
		}

	if( m_lpfnEnableIME != NULL )
	{
		WaitStepInstructions("%s IME", bEnable ? "Enable" : "Disable");
		
		bRet = (*(m_lpfnEnableIME))(GetDumbWindow(), bEnable);
//		SwitchFocus();

		if( bEnable && !IsEnabled() )	// REVIEW: trying to disable and appearing to fail happens, but is harmless
		{
			Sleep(500);		// sleep half a second
			if( !IsEnabled() )
				LOG->RecordInfo("Unable to %s the IME", bEnable ? "Enable" : "Disable");
		}
	}
	else
		LOG->RecordInfo("No IME found.  Cannot use CIME functions");

	return bRet;
}

// BEGIN_HELP_COMMENT
// Function: BOOL CIME::IsEnabled(void)
// Description: Determine if the IME is enabled.
// Return: A Boolean value that indicates whether the IME is enabled (TRUE) or not.
// END_HELP_COMMENT
BOOL CIME::IsEnabled(void)		//REVIEW(EnriqueP): Do we need to switch focus to window?
{
	BOOL bRet = FALSE;

	// REVIEW (michma): on nt/j we do not distinguish between the open status
	// and the enabled status of the IME because we use a different api (Imm functions)
	// which appear not to support the concept.
	// see cafe\support\guitools\imehook.cpp - michma 1/1/97.
	if(m_bRunningOnNTJ)
		// CIME::IsOpen() on nt/j will return:
		//		-1 if the IME is open.
		//		 0 if the IME is closed.
		return (IsOpen() ? TRUE : FALSE);

	if( m_lpfnEnableIME != NULL )
	{
		#ifdef OLD_IME_BEHAVIOR
			bRet = (*(m_lpfnGetEnableStatus))(NULL);
		#else
			bRet = (*(m_lpfnGetEnableStatus))(GetDumbWindow());
		#endif // OLD_IME_BEHAVIOR
	}

	return bRet;
}

// Flag set when the IME is in use.

void CIME::IMEInUse(bool useFlag)
{
	if(useFlag)
	{
		IME_UseFlag = TRUE ;
	}
	else
		IME_UseFlag = FALSE ;

}
// BEGIN_HELP_COMMENT
// Function: BOOL CIME::Open(BOOL bOpen /*=TRUE*/)
// Description: Open or close the IME. The IME must be open before calling any other IME functions.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: bOpen A Boolean value that specifies whether to open the IME (TRUE) or close it. (Default value is TRUE.)
// END_HELP_COMMENT
BOOL CIME::Open(BOOL bOpen /*=TRUE*/)
{
	LPIMESTRUCT lpIME;
	LRESULT lres = FALSE;

	//ShowWindow(SW_SHOW);
	IMEInUse(TRUE); // SET in use flag ;

	#ifdef OLD_IME_BEHAVIOR
		HWND hwndTop =  ::GetForegroundWindow();
		::SetForegroundWindow(m_hWnd);		 // Set Focus to IME activator Window
	#endif // OLD_IME_BEHAVIOR

	// on nt/j we send a private message to the IDE's subclassed window
	// procedure so we can set the IME open status in-process.
	// see cafe\support\guitools\imehook.cpp - michma 1/1/97.
	if(m_bRunningOnNTJ)
		{
		WaitStepInstructions("%s IME", bOpen ? "Open" : "Close");
		// GetDumbWindow() returns the handle to the IDE's main window.
		// bOpen is passed to the subclassed window procedure as lParam. 
		return ::SendMessage(GetDumbWindow(), m_msgIMESetOpen, 0, bOpen); 		
		}

	if( m_hIME && (lpIME = (LPIMESTRUCT)GlobalLock(m_hIME)) && (m_lpfnSendIMEMessage != NULL) )
	{
		lpIME->fnc = IME_SETOPEN;
		lpIME->wParam = bOpen;
		GlobalUnlock (m_hIME);
		WaitStepInstructions("%s IME", bOpen ? "Open" : "Close");
		lres = (*(m_lpfnSendIMEMessage))(GetDumbWindow(), (LPARAM)m_hIME);

		if( !IsOpen() != !bOpen )
		{
			Sleep(500);		// sleep half a second
			if( !IsOpen() != !bOpen )
				LOG->RecordInfo("Unable to %s the IME", bOpen ? "Open" : "Close");
		}
	}
	else
		LOG->RecordInfo("No IME found.  Cannot use CIME functions");

	#ifdef OLD_IME_BEHAVIOR
		::SetForegroundWindow(hwndTop);		// Set focus back to target app
	#endif // OLD_IME_BEHAVIOR

	return lres;
}

// BEGIN_HELP_COMMENT
// Function: int CIME::IsOpen(void)
// Description: Determine if the IME is open.
// Return: An integer that indicates success. 0 if the IME's not open; -1 if the IME is open with no partial string entry; a positive value of the count of characters entered if the IME is open with a partial string entry.
// END_HELP_COMMENT
int CIME::IsOpen(void)	   //REVIEW(EnriqueP): Do we need to switch focus to window?
{
	LPIMESTRUCT lpIME;
	LRESULT lres = FALSE;

	// on nt/j we send a private message to the IDE's subclassed window
	// procedure so we can get the IME open status in-process.
	// see cafe\support\guitools\imehook.cpp - michma 1/1/97.
	if(m_bRunningOnNTJ)
		// GetDumbWindow() returns the handle to the IDE's main window.
		return ::SendMessage(GetDumbWindow(), m_msgIMEGetOpen, 0, 0);

	if (m_hIME && (lpIME = (LPIMESTRUCT) GlobalLock (m_hIME)))
	{
		lpIME->fnc = IME_GETOPEN;
		GlobalUnlock (m_hIME);
		if (m_lpfnSendIMEMessage != NULL) {
				lres = (*(m_lpfnSendIMEMessage))(GetDumbWindow(), (LPARAM)m_hIME);
//			SwitchFocus();
		}
	}

	if( lres == 0 )
		return 0;		// it's not open
	else if( lpIME->wParam == 0 )
		return -1;				// it is open, with no undetermined string
	else
		return lpIME->wParam;	// it is open, with this many bytes in the undetermined string
}

// BEGIN_HELP_COMMENT
// Function: void CIME::Flush(HWND hwnd)
// Description: Flush any remaining keystrokes to the IME. The IME must be open to call this function (see CIME::Open).
// Return: <none>
// Param: hwnd The HWND of the window using the IME. This parameter is NYI.
// END_HELP_COMMENT
void CIME::Flush(HWND hwnd)		   //REVIEW(EnriqueP): Do we need to switch focus to window?
{
	LPIMESTRUCT lpIME;
	LRESULT lres;

	if (m_hIME && (lpIME = (LPIMESTRUCT) GlobalLock (m_hIME)))
	{
		lpIME->fnc = IME_SENDVKEY;
		lpIME->wParam = VK_DBE_FLUSHSTRING;
		GlobalUnlock (m_hIME);
		if (m_lpfnSendIMEMessage != NULL) {
			lres = (*(m_lpfnSendIMEMessage))(GetDumbWindow(), (LPARAM)m_hIME);
//			SwitchFocus();
		}
	}
}

// BEGIN_HELP_COMMENT
// Function: UINT CIME::GetMode(void)
// Description: Get the current mode of the IME. This value is the combination the current mode of each setting in the IME.
// Return: An unsigned integer that contains the current mode of the IME.
// END_HELP_COMMENT
UINT CIME::GetMode(void)			//REVIEW(EnriqueP): Do we need to switch focus to window?
{
	LPIMESTRUCT lpIME;
	LRESULT lres = 0;

	// on nt/j we send a private message to the IDE's subclassed window
	// procedure so we can get the IME conversion mode in-process.
	// see cafe\support\guitools\imehook.cpp - michma 1/1/97.
	if(m_bRunningOnNTJ)
		// GetDumbWindow() returns the handle to the IDE's main window.
		return ::SendMessage(GetDumbWindow(), m_msgIMEGetMode, 0, 0);

	if (m_hIME && (lpIME = (LPIMESTRUCT) GlobalLock (m_hIME)))
	{
		lpIME->fnc = IME_GETCONVERSIONMODE;
		GlobalUnlock (m_hIME);
		if (m_lpfnSendIMEMessage != NULL) {
			lres = (*(m_lpfnSendIMEMessage))(GetDumbWindow(), (LPARAM)m_hIME);
//			SwitchFocus();
		}
	}
	return lres;
}

// BEGIN_HELP_COMMENT
// Function: UINT CIME::SetMode(UINT flags)
// Description: Set the mode of the IME by value. (See CIME::SetMode(EAlphabets, UINT, BOOL, BOOL) to set by enumeration.)
// Return: An unsigned integer that contains the mode actually set in the IME.
// Param: flags An unsigned integer that specifies the mode to set.
// END_HELP_COMMENT
UINT CIME::SetMode(UINT flags)
{
	LPIMESTRUCT lpIME;
	LRESULT lres = 0;

	IMEInUse(TRUE); // SET in use flag ;

	// on nt/j we send a private message to the IDE's subclassed window
	// procedure so we can set the IME conversion mode in-process.
	// see cafe\support\guitools\imehook.cpp - michma 1/1/97.
	if(m_bRunningOnNTJ)
		{
		WaitStepInstructions("Set IME mode to 0x%08x", flags);
		// GetDumbWindow() returns the handle to the IDE's main window.
		// flags is passed to the subclassed window procedure as lParam. 
		return ::SendMessage(GetDumbWindow(), m_msgIMESetMode, 0, flags);
		}

	#ifdef OLD_IME_BEHAVIOR
		// get the handle of the top level window
		HWND hwndTop =  ::GetForegroundWindow();
		::SetForegroundWindow(m_hWnd);		 // Set Focus to IME activator Window
	#endif // OLD_IME_BEHAVIOR

	if (m_hIME && (lpIME = (LPIMESTRUCT) GlobalLock (m_hIME)))
	{
		lpIME->fnc = IME_SETCONVERSIONMODE;
		lpIME->wParam = flags;
		GlobalUnlock (m_hIME);
		WaitStepInstructions("Set IME mode to 0x%08x", flags);
		if (m_lpfnSendIMEMessage != NULL) {
			lres = (*(m_lpfnSendIMEMessage))(GetDumbWindow(), (LPARAM)m_hIME);
//			SwitchFocus();
		}
	}

	#ifdef OLD_IME_BEHAVIOR
		::SetForegroundWindow(hwndTop);
	#endif // OLD_IME_BEHAVIOR

//	LOG->Comment("SetMode to 0x%08x: 0x%08x", flags, lres);
	return lres;
}

// BEGIN_HELP_COMMENT
// Function: UINT CIME::SetMode(EAlphabets eAlphabet /*=ALPHA*/, UINT nBytes /*=1*/, BOOL bRoman /*=TRUE*/, BOOL bCodeInput /*=FALSE*/)
// Description: Set the mode of the IME by enumeration. (See CIME::SetMode(UINT) to set by value.)
// Return: An unsigned integer that contains the mode actually set in the IME.
// Param: eAlphabet An EAlphabets value that specifies the type of alphabet to set in the IME: DONTCARE, ALPHA, KATAKANA, HIRAGANA. (Default value is ALPHA.)
// Param: nBytes An integer that specifies the number of bytes: SBCS (1) or DBCS (2). (Default value is 1.)
// Param: bRoman A Boolean value that specifies whether to set the IME to Roman mode (TRUE) or NoRoman mode. NoRoman mode has not been found to be useful. (Default value is TRUE.)
// Param: bCodeInput A Boolean value that specifies whether to set the IME to code input mode (TRUE) or no code input mode. Code input mode has not been found to be useful. (Default value is FALSE.)
// END_HELP_COMMENT
UINT CIME::SetMode(EAlphabets eAlphabet /*=ALPHA*/, UINT nBytes /*=1*/, BOOL bRoman /*=TRUE*/, BOOL bCodeInput /*=FALSE*/)
{
	UINT flags = GetMode();
	UINT fAlphabet, fNumBytes, fRoman, fCodeInput;

	// on nt/j we send a private message to the IDE's subclassed window	procedure
	// so we can set the IME conversion mode in-process. The api we	use to access
	// the IME uses different conversion mode values than used on win95, so we 
	// need to create a different "flags" value for  CIME::SetMode(UINT flags).
	// see cafe\support\guitools\imehook.cpp - michma 1/1/97.
	if(m_bRunningOnNTJ)
		{
		if(eAlphabet == ALPHA)
			{
			if(nBytes == 1)
				flags = IME_MODE_SB_ALPHA;	// single-byte alpha.
			else if(nBytes == 2)
				flags = IME_MODE_DB_ALPHA;	// double-byte alpha.
			}	
		else if(eAlphabet == KATAKANA)
			{
			if(nBytes == 1)
				flags = IME_MODE_SB_KATAKANA;	// single-byte katakana.
			else if(nBytes == 2)
				flags = IME_MODE_DB_KATAKANA;	// double-byte katakana.
			}
		else if(eAlphabet == HIRAGANA)	
			{
			if(nBytes == 2)
				flags = IME_MODE_DB_HIRAGANA;	// double-byte hiragana.
			}
 		// this function will send the private message to the IDE' subclassed window procedure.
		return SetMode(flags);	
		}

	if( eAlphabet == ALPHA )						// Use alphabet chosen by user
		fAlphabet = IME_MODE_ALPHANUMERIC;
	else if( eAlphabet == KATAKANA )
		fAlphabet = IME_MODE_KATAKANA;
	else if( eAlphabet == HIRAGANA )
		fAlphabet = IME_MODE_HIRAGANA;
	else											// otherwise, set alphabet
		fAlphabet = flags & IME_MODES_ALPHABET;		// to whatever it already is

	if( nBytes == 1 )								// Set the number of bytes to either
		fNumBytes = IME_MODE_SBCSCHAR;				// SBCS
	else if( nBytes == 2 )							// DBCS
		fNumBytes = IME_MODE_DBCSCHAR;
	else											// or whatever it already is
		fNumBytes = flags & IME_MODES_NUMBYTES;

	if( bRoman )									// Set Roman
		fRoman = IME_MODE_ROMAN;					// I've never seen NoRoman to be useful
	else
		fRoman = IME_MODE_NOROMAN;
		
	if( bCodeInput )								// Set CodeInput
		fCodeInput = IME_MODE_CODEINPUT;			// I've never seen this to be useful
	else
		fCodeInput = IME_MODE_NOCODEINPUT;

	// see if there are any other bits that we didn't know about
	flags &= ~(IME_MODES_ALPHABET | IME_MODES_NUMBYTES | IME_MODES_ROMAN | IME_MODES_INPUT);

	return SetMode( fAlphabet | fNumBytes | fRoman | fCodeInput | flags );		// set new mode
}

// set the conversion mode through the IME setup dialog
// (this is the first button on the IME)
BOOL CIME::SetConversionMode(int nMode)
{
	// the IME should be open
	ASSERT(IsOpen());

	// are we on NT or Win95?
	if (GetSystem() & SYSTEM_WIN) {
		LOG->RecordInfo("Setting the IME conversion mode on Win95...");
		// Win95
		// bring up the IME setup dialog
		MST.DoKeys("^{F10}");
		Sleep(1000);
		MST.DoKeys("{F5}");
		Sleep(1000);
		// wait for the dialog to come up
		if (!MST.WFndWndWait("MS-IME", FW_PART | FW_DIALOG, 5)) {
			LOG->RecordInfo("CIME::SetConversionMode: unable to open the MS IME setup dialog");
			return FALSE;
		}
		Sleep(1000);

		// get to the tab that contains the conversion mode field (two tabs in)
		// tab until the active window is the OK button
		CString strText;
		do {
			MST.DoKeys("{tab}");
			MST.WGetText(MST.WGetFocus(), strText);
		}
		while (strText != "OK");
		// once we're on the OK button, tab twice more to get to the tab
		MST.DoKeys("{tab 2}");

		// move to the third tab
		MST.DoKeys("{right 2}");

		// for some reason ^({tab}) doesn't work in this dialog...
//		MST.DoKeys("^({tab})");
//		MST.DoKeys("^({tab})");

		// select the conversion mode field
		MST.DoKeys("%x{HOME}");
		Sleep(500);
		// select the conversion mode
		char buf[255];
		sprintf(buf, "{DOWN %d}{ENTER}", nMode);
		MST.DoKeys(buf);
		Sleep(500);
		// wait for the dialog to go away
		if (!MST.WFndWndWait("MS-IME", FW_PART | FW_DIALOG | FW_NOEXIST, 5)) {
			LOG->RecordInfo("CIME::SetConversionMode: unable to close the MS IME setup dialog");
			return FALSE;
		}
		LOG->RecordInfo("Successfully set the IME conversion mode on Win95.");
	}
	else if (GetSystem() & SYSTEM_NT) {
		LOG->RecordInfo("Setting the IME conversion mode on NT...");
		// NT
		// set the mode
		// bring up the IME setup dialog
		MST.DoKeys("^{F10}");
		Sleep(1000);
		MST.DoKeys("{F5}");
		Sleep(1000);
		// wait for the dialog to come up
		if (!MST.WFndWndWait("MS-IME", FW_PART | FW_DIALOG, 5)) {
			LOG->RecordInfo("CIME::SetConversionMode: unable to open the MS IME setup dialog");
			return FALSE;
		}
		Sleep(1000);

		// select the conversion mode field
		MST.DoKeys("%x{HOME}");
		Sleep(500);
		// select the conversion mode
		char buf[255];
		sprintf(buf, "{DOWN %d}{ENTER}", nMode);
		MST.DoKeys(buf);
		Sleep(500);
		// wait for the dialog to go away
		if (!MST.WFndWndWait("MS-IME", FW_PART | FW_DIALOG | FW_NOEXIST, 5)) {
			LOG->RecordInfo("CIME::SetConversionMode: unable to close the MS IME setup dialog");
			return FALSE;
		}

		Open(TRUE);
		LOG->RecordInfo("Successfully set the IME conversion mode on NT.");
	}
	else {
		LOG->RecordInfo("CIME::SetConversionMode: unable to set conversion mode; unrecognized OS.");
		return FALSE;
	}
	return TRUE;
}
	
// BEGIN_HELP_COMMENT
// Function: BOOL CIME::SendVKey(UINT vKey)
// Description: Send the IME a virtual keystroke.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: vKey An integer that specifies the virtual key to send to the IME.
// END_HELP_COMMENT
BOOL CIME::SendVKey(UINT vKey)		//REVIEW(EnriqueP): Do we need to switch focus to window?
{
	LPIMESTRUCT lpIME;
	LRESULT lres = 0;

	if (m_hIME && (lpIME = (LPIMESTRUCT) GlobalLock (m_hIME)))
	{
		lpIME->fnc = IME_SENDVKEY;
		lpIME->wParam = vKey;
		lpIME->wCount = vKey;
		GlobalUnlock (m_hIME);
		WaitStepInstructions("Send IME VKey 0x%x", vKey);
		if (m_lpfnSendIMEMessage != NULL) {
			lres = (*(m_lpfnSendIMEMessage))(GetDumbWindow(), (LPARAM)m_hIME);
//			SwitchFocus();
		}
	}
	return lres;
}

//	Because SendIMEMessage requires an HWND, and NULL usually isn't good,
//	the best thing seems to be to send the active window.
//	When we know more, maybe we can adjust this function.
HWND CIME::GetDumbWindow(void)
{
	#ifdef OLD_IME_BEHAVIOR
		return m_editCtl.m_hWnd;
	#else
		CString strTitle;
		CString strClass;

		// get the handle of the top level window
		HWND hwndTop = ::GetTopWindow(NULL);

		// try to find the IDE by iterating through all windows
		for (HWND hwnd = hwndTop; hwnd != NULL; hwnd = ::GetNextWindow(hwnd, GW_HWNDNEXT)) {
			// get the window title
			::GetWindowText(hwnd, strTitle.GetBuffer(255), 255);
			strTitle.ReleaseBuffer();
			// get the window class
			::GetClassName(hwnd, strClass.GetBuffer(255), 255);
			strClass.ReleaseBuffer();
			// do the title and class match the IDE?
			if (strTitle.Find("Microsoft Visual C++") != -1 && strClass.Find("Afx:") != -1) {
				break;
			}
		}

		// was the IDE found?
		if (hwnd != NULL) {
			hwndTop = hwnd;
		}


//		HWND hwndTop = ::GetForegroundWindow();
		::SetFocus(hwndTop);
		return hwndTop;
	#endif // OLD_IME_BEHAVIOR
}

void CIME::SwitchFocus(void)
{
	HWND hwndTop = GetDumbWindow();

	// set focus to the small window
	::SetForegroundWindow(m_editCtl.m_hWnd);
	Sleep(1000);
//	::ShowWindow(m_hWnd, SW_HIDE);

	::SetForegroundWindow(hwndTop);
	Sleep(1000);
}
