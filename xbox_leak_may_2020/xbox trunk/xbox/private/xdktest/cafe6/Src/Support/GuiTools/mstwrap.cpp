///////////////////////////////////////////////////////////////////////////////
//      TESTWRAP.CPP
//
//      Created by :                    Date :
//              DavidGa                                 8/14/93
//
//      Description :
//              Implementation of the CMSTest class
//

#include "stdafx.h"
#include "mstwrap.h"
#include "guiv1.h"
#include "guitarg.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// REVIEW(briancr): this object is declared as a global for backward compatibility
// REVIEW(briancr): this is just to make things work. Is there a better way to do this?
// instantiate a global CMSTest
GUI_DATA CMSTest MST;

#define BUFFER          256             // arbitrary size of CString buffers

#define CMDBAR 1

// temporary variables
WNDPOS          g_WndPos;
WNDSIZ          g_WndSiz;
WNDPOSSIZ       g_WndPosSiz;

#if CMDBAR
static UINT DSM_TESTMENU = RegisterWindowMessage("DevStudioTestMenu");
static const int cMenuMenu=0;
static const int cMenuExists=1;
// static const int cMenuGrayed=2; // for some reason, these two aren't implemented.
// static const int cMenuChecked=3;
static const int cMenuEnabled=4;
static const int cMenuCount=5;
static const int cMenuText=6;
static const int cMenuEnd=7;
static const int cMenuRealChecked=8;
#endif

/////////////////////////////////////////////////////////////////////////////
// TEST struct conversions

// WNDPOS <==> CPoint
inline void CMSTest::Convert(WNDPOS const wndpos, CPoint& pt)
	{       pt.x = wndpos.left;
		pt.y = wndpos.top; }
inline void CMSTest::Convert(CPoint const pt, WNDPOS& wndpos)
	{       wndpos.left = pt.x;
		wndpos.top = pt.y; }
inline CPoint CMSTest::Convert(WNDPOS const wndpos)
	{       return CPoint(wndpos.left, wndpos.top); }

// WNDSIZ <==> CSize
inline void CMSTest::Convert(WNDSIZ const wndsiz, CSize& size)
	{       size.cx = wndsiz.width;
		size.cy = wndsiz.height; }
inline void CMSTest::Convert(CSize const size, WNDSIZ& wndsiz)
	{       wndsiz.width = size.cx;
		wndsiz.height = size.cy; }
inline CSize CMSTest::Convert(WNDSIZ const wndsiz)
	{       return CSize(wndsiz.width, wndsiz.height); }

// WNDPOSSIZ <==> CRect
inline void CMSTest::Convert(WNDPOSSIZ const wndpossiz, CRect& rc)
	{       rc.left = wndpossiz.left;
		rc.top = wndpossiz.top;
		rc.right = rc.left + wndpossiz.width;
		rc.bottom = rc.top + wndpossiz.height; }
inline void CMSTest::Convert(CRect const rc, WNDPOSSIZ& wndpossiz)
	{       wndpossiz.left = rc.left;
		wndpossiz.top = rc.top;
		wndpossiz.width = rc.right - rc.left;
		wndpossiz.height = rc.bottom - rc.top; }
inline CRect CMSTest::Convert(WNDPOSSIZ const wndpossiz)
	{       return CRect(wndpossiz.left, wndpossiz.top, wndpossiz.left + wndpossiz.width, wndpossiz.top + wndpossiz.height); }

// wRect <==> CRect


/////////////////////////////////////////////////////////////////////////////
// From TESTCTRL.H
//
//---------------------------------------------------------------------------
// Miscelaneous routines
//---------------------------------------------------------------------------

//long WMessage        (HWND hWnd, UINT wMsg)
//long WMessageW       (HWND hWnd, UINT wMsg, WPARAM wp)
//long WMessageL       (HWND hWnd, UINT wMsg, LPARAM lp)
//long WMessageWL      (HWND hWnd, UINT wMsg, WPARAM wp, LPARAM lp)

// BEGIN_HELP_COMMENT
// Function: HWND CMSTest::WGetFocus(void)
// Description: A wrapper for the MS-Test function WGetFocus. See the MS-Test documentation for a description of this function.
// Return: An HWND.
// END_HELP_COMMENT
HWND CMSTest::WGetFocus       (void)
	{       return ::WGetFocus();      }
//void WDisplayInfo    (HWND, UINT)
//void WGetInfo        (HWND, LPINFO)

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WStaticSetClass(LPSTR lpszClassName)
// Description: A wrapper for the MS-Test function WStaticSetClass. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpszClassName A pointer to a string that contains the static window class name.
// END_HELP_COMMENT
// void CMSTest::WStaticSetClass (LPSTR lpszClassName)
//	{ ::WStaticSetClass(lpszClassName); }

//void WResetClasses   (void)

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WIsVisible(HWND hwnd)
// Description: A wrapper for the MS-Test function WIsVisible. See the MS-Test documentation for a description of this function.
// Return: A Boolean value.
// Param: hwnd An HWND that specifies the window.
// END_HELP_COMMENT
BOOL CMSTest::WIsVisible      (HWND hwnd)
	{       return ::WIsVisible(hwnd); }

//long WTextLen        (HWND hWnd)

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WGetText(HWND hwnd, CString& strBuffer)
// Description: A wrapper for the MS-Test function WGetText. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: hwnd An HWND that specifies the window.
// Param: strBuffer A reference to a CString that will contain the text of the specified window.
// END_HELP_COMMENT
void CMSTest::WGetText        (HWND hwnd, CString& strBuffer)
	{   ::WGetText(hwnd, strBuffer.GetBufferSetLength(BUFFER));
		strBuffer.ReleaseBuffer(-1); }

//void WSetText        (HWND hWnd, LPCSTR lpcszText)
//int  WNumAltKeys     (void)
//void WGetAltKeys     (CString& strBuff)
//int  WNumDupAltKeys  (void)
//void WGetDupAltKeys  (CString& strBuff)

//---------------------------------------------------------------------------
// General Window routines
//---------------------------------------------------------------------------

// BEGIN_HELP_COMMENT
// Function: HWND CMSTest::WFndWndWait(LPCSTR lpcszCaption, UINT uFlags, UINT uSeconds)
// Description: A wrapper for the MS-Test function WFndWndWait. See the MS-Test documentation for a description of this function.
// Return: An HWND of the window.
// Param: lpcszCaption A pointer to a string that contains the caption of the window.
// Param: uFlags A bitfield that specifies the type of window.
// Param: uSeconds An integer that specifies the number of seconds to wait for the window.
// END_HELP_COMMENT
HWND CMSTest::WFndWndWait       (LPCSTR lpcszCaption, UINT uFlags, UINT uSeconds)
	{       return ::WFndWnd((LPSTR)lpcszCaption, uFlags, uSeconds); }

// BEGIN_HELP_COMMENT
// Function: HWND CMSTest::WFndWndWaitC(LPCSTR lpcszCaption, LPCSTR lpcszClass, UINT uFlags, UINT uSeconds)
// Description: A wrapper for the MS-Test function WFndWndWaitC. See the MS-Test documentation for a description of this function.
// Return: An HWND of the window.
// Param: lpcszCaption A pointer to a string that contains the caption of the window.
// Param: lpcszClass A pointer to a string that contains the name of the window class of the window.
// Param: uFlags A bitfield that specifies the type of window.
// Param: uSeconds An integer that specifies the number of seconds to wait for the window.
// END_HELP_COMMENT
HWND CMSTest::WFndWndWaitC       (LPCSTR lpcszCaption, LPCSTR lpcszClass, UINT uFlags, UINT uSeconds)
	{       return ::WFndWndC((LPSTR)lpcszCaption, (LPSTR)lpcszClass, uFlags, uSeconds); }

// BEGIN_HELP_COMMENT
// Function: HWND CMSTest::WFndWnd(LPCSTR lpcszCaption, UINT uFlags)
// Description: A wrapper for the MS-Test function WFndWnd. See the MS-Test documentation for a description of this function.
// Return: An HWND of the window.
// Param: lpcszCaption A pointer to a string that contains the caption of the window.
// Param: uFlags A bitfield that specifies the type of window.
// END_HELP_COMMENT
HWND CMSTest::WFndWnd       (LPCSTR lpcszCaption, UINT uFlags)
	{       return ::WFndWnd((LPSTR)lpcszCaption, uFlags, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: HWND CMSTest::WFndWndC(LPCSTR lpcszCaption, LPCSTR lpcszClass, UINT uFlags)
// Description: A wrapper for the MS-Test function WFndWndC. See the MS-Test documentation for a description of this function.
// Return: An HWND of the window.
// Param: lpcszCaption A pointer to a string that contains the caption of the window.
// Param: lpcszClass A pointer to a string that contains the name of the window class of the window.
// Param: uFlags A bitfield that specifies the type of window.
// END_HELP_COMMENT
HWND CMSTest::WFndWndC      (LPCSTR lpcszCaption, LPCSTR lpcszClass, UINT uFlags)
	{       return ::WFndWndC((LPSTR)lpcszCaption, (LPSTR)lpcszClass, uFlags, TIMEOUT); }

//void WMinWnd       (HWND hWnd);
void CMSTest::WMaxWnd(HWND hWnd)
{
	::WMaxWnd(hWnd);
}
//void WResWnd       (HWND hWnd);
//void WSetWndPosSiz (HWND hWnd, int x,  int y, int w, int h);
//void WSetWndPos    (HWND hWnd, int x,  int y);

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WSetWndSiz(HWND hwnd, int w,  int h)
// Description: A wrapper for the MS-Test function WSetWndSiz. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: hwnd The HWND of the window.
// Param: w An integer that contains the width to set.
// Param: h An integer that contains the height to set.
// END_HELP_COMMENT
void CMSTest::WSetWndSiz    (HWND hwnd, int w,  int h)
	{      ::WSetWndSiz(hwnd,w,h); }

//void WAdjWndPosSiz (HWND hWnd, int dx, int dy, int dw, int dh);
//void WAdjWndPos    (HWND hWnd, int dx, int dy);
//void WAdjWndSiz    (HWND hWnd, int dw, int dh);
//void WGetWndPosSiz (HWND hWnd, LPWNDPOSSIZ lpWndPosSiz, BOOL fRelative);
//void WGetWndPos    (HWND hWnd, LPWNDPOS    lpWndPos,    BOOL fRelative);

// BEGIN_HELP_COMMENT
// Function: CSize CMSTest::WGetWndSiz(HWND hwnd)
// Description: A wrapper for the MS-Test function WGetWndSiz. See the MS-Test documentation for a description of this function.
// Return: A CSize object that contains the size of the window.
// Param: hwnd The HWND of the window.
// END_HELP_COMMENT
CSize CMSTest::WGetWndSiz    (HWND hwnd)
	{  //             return ::WGetWndSiz(hwnd, TIMEOUT); }

     ::WGetWndSiz(hwnd, &g_WndSiz);
		return Convert(g_WndSiz); }


// BEGIN_HELP_COMMENT
// Function: void CMSTest::WSetActWnd(HWND hwnd)
// Description: A wrapper for the MS-Test function WSetActWnd. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: hwnd The HWND of the window to make active.
// END_HELP_COMMENT
void CMSTest::WSetActWnd    (HWND hwnd)
	{      ::WSetActWnd(hwnd); }

// BEGIN_HELP_COMMENT
// Function: HWND CMSTest::WGetActWnd(HWND hwnd)
// Description: A wrapper for the MS-Test function WGetActWnd. See the MS-Test documentation for a description of this function.
// Return: The HWND of the active window.
// Param: hwnd The HWND of the window.
// END_HELP_COMMENT
HWND CMSTest::WGetActWnd    (HWND hwnd)
	{       return ::WGetActWnd(hwnd); }

//BOOL WIsMaximized  (HWND hWnd);
//BOOL WIsMinimized  (HWND hWnd);

//---------------------------------------------------------------------------
// Menu routines
//---------------------------------------------------------------------------

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WMenu(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WMenu. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the menu name.
// END_HELP_COMMENT
void CMSTest::WMenu        (LPCSTR lpcszName)
#if CMDBAR
{
	WaitStepInstructions("Exec menu command '%s'", lpcszName);
	HWND hWndActive= ::GetForegroundWindow();
	if(hWndActive)
	{
		ATOM nAtom=GlobalAddAtom(lpcszName);
		::SendMessage(hWndActive, DSM_TESTMENU, cMenuMenu, nAtom);

		// 1 second delay. Extravagant, and we need to work on a better way to do this.
		Sleep(1000);

		GlobalDeleteAtom(nAtom);
	}
}
#else
	{      ::WMenu((LPSTR)lpcszName); }
#endif

//void FAR     WMenuEx            (LPCSTR lpcszName, ...);

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WMenuExists(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WMenuExists. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the menu exists.
// Param: lpcszName A pointer to a string that contains the menu name.
// END_HELP_COMMENT
BOOL CMSTest::WMenuExists        (LPCSTR lpcszName)
#if CMDBAR
{
	HWND hWndActive= ::GetForegroundWindow();
	if(hWndActive)
	{
		ATOM nAtom=GlobalAddAtom(lpcszName);
		int rv=::SendMessage(hWndActive, DSM_TESTMENU, cMenuExists, nAtom);
		GlobalDeleteAtom(nAtom);
		return rv;
	}
	return FALSE;
}
#else
	{      return ::WMenuExists(lpcszName); }
#endif

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WMenuGrayed(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WMenuGrayed. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the menu is grayed.
// Param: lpcszName A pointer to a string that contains the menu name.
// END_HELP_COMMENT
BOOL CMSTest::WMenuGrayed        (LPCSTR lpcszName)
// no cmdbar here, because internally this function does nothing.
	{       return ::WMenuGrayed((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WMenuChecked(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WMenuChecked. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the menu is checked.
// Param: lpcszName A pointer to a string that contains the menu name.
// END_HELP_COMMENT
BOOL CMSTest::WMenuChecked       (LPCSTR lpcszName)
#if CMDBAR
{
	HWND hWndActive= ::GetForegroundWindow();
	if(hWndActive)
	{
		ATOM nAtom=GlobalAddAtom(lpcszName);
		int rv=::SendMessage(hWndActive, DSM_TESTMENU, cMenuRealChecked, nAtom);
		GlobalDeleteAtom(nAtom);
		return rv;
	}
	return FALSE;
}
#else
	{       return ::WMenuChecked((LPSTR)lpcszName, TIMEOUT); }
#endif

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WMenuEnabled(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WMenuEnabled. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the menu is enabled.
// Param: lpcszName A pointer to a string that contains the menu name.
// END_HELP_COMMENT
BOOL CMSTest::WMenuEnabled       (LPCSTR lpcszName)
#if CMDBAR
{
	HWND hWndActive= ::GetForegroundWindow();
	if(hWndActive)
	{
		ATOM nAtom=GlobalAddAtom(lpcszName);
		int rv=::SendMessage(hWndActive, DSM_TESTMENU, cMenuEnabled, nAtom);
		GlobalDeleteAtom(nAtom);
		return rv;
	}
	return FALSE;
}
#else
	{       return  ::WMenuEnabled((LPSTR)lpcszName); } 
#endif

// BEGIN_HELP_COMMENT
// Function: int CMSTest::WMenuCount(void)
// Description: A wrapper for the MS-Test function WMenuCount. See the MS-Test documentation for a description of this function.
// Return: An integer that specifies the number of items in the menu.
// END_HELP_COMMENT
int  CMSTest::WMenuCount         (void)
#if CMDBAR
{
	HWND hWndActive= ::GetForegroundWindow();
	if(hWndActive)
	{
		return ::SendMessage(hWndActive, DSM_TESTMENU, cMenuCount, 0);
	}
	return 0;
}
#else
	{       return ::WMenuCount(); }
#endif

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WMenuText(LPCSTR lpcszName, CString& strBuffer)
// Description: A wrapper for the MS-Test function WMenuText. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the menu name.
// Param: strBuffer A CString reference that will contain the name of the menu.
// END_HELP_COMMENT
void CMSTest::WMenuText                 (LPCSTR lpcszName, CString& strBuffer)
#if CMDBAR
{
	HWND hWndActive= ::GetForegroundWindow();
	if(hWndActive)
	{
		ATOM nAtom=GlobalAddAtom(lpcszName);
		ATOM nNewAtom=(ATOM)::SendMessage(hWndActive, DSM_TESTMENU, cMenuText, nAtom);
		GlobalGetAtomName(nNewAtom, strBuffer.GetBufferSetLength(BUFFER), BUFFER); // ugh. We don't know how big strBuffer is.
		strBuffer.ReleaseBuffer(-1) ;
		GlobalDeleteAtom(nAtom);
		GlobalDeleteAtom(nNewAtom);
	}
}
#else
	{ 
	   ::WMenuText((LPSTR)lpcszName, (LPSTR) strBuffer.GetBufferSetLength(BUFFER));
	   strBuffer.ReleaseBuffer(-1) ; }
#endif

//int  WMenuLen           (LPCSTR lpcszName)
//void WMenuFullText      (LPCSTR lpcszName, CString& strBuffer)
//int  WMenuFullLen       (LPCSTR lpcszName)

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WMenuEnd(void)
// Description: A wrapper for the MS-Test function WMenuEnd. See the MS-Test documentation for a description of this function.
// Return: <none>
// END_HELP_COMMENT
void CMSTest::WMenuEnd           (void)
#if CMDBAR
{
	HWND hWndActive= ::GetForegroundWindow();
	if(hWndActive)
	{
		SendMessage(hWndActive, DSM_TESTMENU, cMenuEnd, 0);
	}
}
#else
	{      ::WMenuEnd(); }
#endif

//BOOL WSysMenuExists     (HWND hWnd)
//void WSysMenu           (HWND hWnd)
//int  WMenuNumAltKeys    (void)
//void WMenuGetAltKeys    (CString& strBuff)
//int  WMenuNumDupAltKeys (void)
//void WMenuGetDupAltKeys (CString& strBuff)
//BOOL WMenuSeparator     (int iIndex)

// Obsolete.
//----------
//void WMenuX             (int iIndex)
//BOOL WMenuGrayedX       (int iIndex)
//BOOL WMenuCheckedX      (int iIndex)
//BOOL WMenuEnabledX      (int iIndex)

//---------------------------------------------------------------------------
// Command button routines.
//---------------------------------------------------------------------------

//void WButtonSetClass (LPCSTR lpcszClassName)

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WButtonExists(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WButtonExists. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the button exists.
// Param: lpcszName A pointer to a string that contains the button name.
// END_HELP_COMMENT
BOOL CMSTest::WButtonExists   (LPCSTR lpcszName)
	{       return ::WButtonExists((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WButtonEnabled(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WButtonEnabled. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the button is enabled.
// Param: lpcszName A pointer to a string that contains the button name.
// END_HELP_COMMENT
BOOL CMSTest::WButtonEnabled  (LPCSTR lpcszName)
	{       return ::WButtonEnabled((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WButtonFocus(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WButtonFocus. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the button has focus.
// Param: lpcszName A pointer to a string that contains the button name.
// END_HELP_COMMENT
BOOL CMSTest::WButtonFocus    (LPCSTR lpcszName)
	{       return ::WButtonFocus((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WButtonClick(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WButtonClick. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the button name.
// END_HELP_COMMENT
void CMSTest::WButtonClick    (LPCSTR lpcszName)
	{       WaitStepInstructions("Click button '%s'", lpcszName);
      ::WButtonClick((LPSTR)lpcszName, TIMEOUT); }

//void WButtonHide     (LPCSTR lpcszName)
//void WButtonShow     (LPCSTR lpcszName)
//void WButtonEnable   (LPCSTR lpcszName)
//void WButtonDisable  (LPCSTR lpcszName)
//BOOL WButtonDefault  (LPCSTR lpcszName)
//int WButtonDefaults (void)

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WButtonSetFocus(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WButtonSetFocus. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the button name.
// END_HELP_COMMENT
void CMSTest::WButtonSetFocus (LPCSTR lpcszName)
	{       WaitStepInstructions("Set focus to button '%s'", lpcszName);
      ::WButtonSetFocus((LPSTR)lpcszName, TIMEOUT); }

//---------------------------------------------------------------------------
// CheckBox routines
//---------------------------------------------------------------------------

//void WCheckSetClass (LPCSTR lpcszClassName)

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WCheckExists(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WCheckExists. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the check box exists.
// Param: lpcszName A pointer to a string that contains the check box name.
// END_HELP_COMMENT
BOOL CMSTest::WCheckExists   (LPCSTR lpcszName)
	{       return ::WCheckExists((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WCheckEnabled(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WCheckEnabled. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the check box is enabled.
// Param: lpcszName A pointer to a string that contains the check box name.
// END_HELP_COMMENT
BOOL CMSTest::WCheckEnabled  (LPCSTR lpcszName)
	{       return ::WCheckEnabled((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WCheckFocus(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WCheckFocus. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the check box has focus.
// Param: lpcszName A pointer to a string that contains the check box name.
// END_HELP_COMMENT
BOOL CMSTest::WCheckFocus    (LPCSTR lpcszName)
	{       return ::WCheckFocus((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::WCheckState(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WCheckState. See the MS-Test documentation for a description of this function.
// Return: An integer value that indicates the state of the check box.
// Param: lpcszName A pointer to a string that contains the check box name.
// END_HELP_COMMENT
int  CMSTest::WCheckState    (LPCSTR lpcszName)
	{       return ::WCheckState((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WCheckClick(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WCheckClick. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the check box name.
// END_HELP_COMMENT
void CMSTest::WCheckClick    (LPCSTR lpcszName)
	{       WaitStepInstructions("Click checkbox '%s'", lpcszName);
      ::WCheckClick((LPSTR)lpcszName, TIMEOUT); }

//void WCheckHide     (LPCSTR lpcszName)
//void WCheckShow     (LPCSTR lpcszName)
//void WCheckEnable   (LPCSTR lpcszName)
//void WCheckDisable  (LPCSTR lpcszName)

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WCheckCheck(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WCheckCheck. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the check box name.
// END_HELP_COMMENT
void CMSTest::WCheckCheck    (LPCSTR lpcszName)
	{       WaitStepInstructions("Check checkbox '%s'", lpcszName);
      ::WCheckCheck((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WCheckUnCheck(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WCheckUnCheck. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the check box name.
// END_HELP_COMMENT
void CMSTest::WCheckUnCheck  (LPCSTR lpcszName)
	{       WaitStepInstructions("Uncheck checkbox '%s'", lpcszName);
      ::WCheckUnCheck((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WCheckSetFocus(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WCheckSetFocus. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the check box name.
// END_HELP_COMMENT
void CMSTest::WCheckSetFocus (LPCSTR lpcszName)
	{       WaitStepInstructions("Set focus to checkbox '%s'", lpcszName);
      ::WCheckSetFocus((LPSTR)lpcszName, TIMEOUT); }

//---------------------------------------------------------------------------
// Option Button routines
//---------------------------------------------------------------------------

//void WOptionSetClass (LPCSTR lpcszClassName)

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WOptionExists(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WOptionExists. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the radio button exists.
// Param: lpcszName A pointer to a string that contains the radio button name.
// END_HELP_COMMENT
BOOL CMSTest::WOptionExists   (LPCSTR lpcszName)
	{       return ::WOptionExists((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WOptionEnabled(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WOptionEnabled. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the radio button is enabled.
// Param: lpcszName A pointer to a string that contains the radio button name.
// END_HELP_COMMENT
BOOL CMSTest::WOptionEnabled  (LPCSTR lpcszName)
	{       return ::WOptionEnabled((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WOptionFocus(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WOptionFocus. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the radio button has focus.
// Param: lpcszName A pointer to a string that contains the radio button name.
// END_HELP_COMMENT
BOOL CMSTest::WOptionFocus    (LPCSTR lpcszName)
	{       return ::WOptionFocus((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::WOptionState(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WOptionState. See the MS-Test documentation for a description of this function.
// Return: An integer that contains the state of the radio button.
// Param: lpcszName A pointer to a string that contains the radio button name.
// END_HELP_COMMENT
int  CMSTest::WOptionState    (LPCSTR lpcszName)
	{       return ::WOptionState((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WOptionClick(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WOptionClick. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the radio button name.
// END_HELP_COMMENT
void CMSTest::WOptionClick    (LPCSTR lpcszName)
	{       WaitStepInstructions("Click radio button '%s'", lpcszName);
      ::WOptionClick((LPSTR)lpcszName, TIMEOUT); }

//void WOptionHide     (LPCSTR lpcszName)
//void WOptionShow     (LPCSTR lpcszName)
//void WOptionEnable   (LPCSTR lpcszName)
//void WOptionDisable  (LPCSTR lpcszName)

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WOptionSelect(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WOptionSelect. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the radio button name.
// END_HELP_COMMENT
void CMSTest::WOptionSelect   (LPCSTR lpcszName)
	{       WaitStepInstructions("Select radio button '%s'", lpcszName);
      ::WOptionSelect((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WOptionSetFocus(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WOptionSetFocus. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the radio button name.
// END_HELP_COMMENT
void CMSTest::WOptionSetFocus (LPCSTR lpcszName)
	{       WaitStepInstructions("Set focus to radio button '%s'", lpcszName);
      ::WOptionSetFocus((LPSTR)lpcszName, TIMEOUT); }

//---------------------------------------------------------------------------
// Listbox routines
//---------------------------------------------------------------------------

 void CMSTest::WListSetClass (LPCSTR lpszClassName)
	{ ::WListSetClass((LPSTR)lpszClassName); }

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WListExists(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WListExists. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the list box exists.
// Param: lpcszName A pointer to a string that contains the list box name.
// END_HELP_COMMENT
BOOL CMSTest::WListExists       (LPCSTR lpcszName)
	{       return ::WListExists((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: int  CMSTest::WListCount(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WListCount. See the MS-Test documentation for a description of this function.
// Return: An integer that contains the number of items in the list.
// Param: lpcszName A pointer to a string that contains the list box name.
// END_HELP_COMMENT
int  CMSTest::WListCount        (LPCSTR lpcszName)
	{       return ::WListCount((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WListText(LPCSTR lpcszName, CString& strBuffer)
// Description: A wrapper for the MS-Test function WListText. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: strBuffer A CString reference that will contain the text of the list box.
// END_HELP_COMMENT
void CMSTest::WListText         (LPCSTR lpcszName, CString& strBuffer)
	{      ::WListText((LPSTR)lpcszName,  strBuffer.GetBufferSetLength(BUFFER), TIMEOUT);
			strBuffer.ReleaseBuffer(-1) ; }

// BEGIN_HELP_COMMENT
// Function: int  CMSTest::WListLen(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WListLen. See the MS-Test documentation for a description of this function.
// Return: An integer that specifies the length of the list.
// Param: lpcszName A pointer to a string that contains the list box name.
// END_HELP_COMMENT
int  CMSTest::WListLen          (LPCSTR lpcszName)
	{       return ::WListLen((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: int  CMSTest::WListIndex(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WListIndex. See the MS-Test documentation for a description of this function.
// Return: An integer that specifies the index.
// Param: lpcszName A pointer to a string that contains the list box name.
// END_HELP_COMMENT
int  CMSTest::WListIndex        (LPCSTR lpcszName)
	{       return ::WListIndex((LPSTR)lpcszName, TIMEOUT); }

//int  WListTopIndex     (LPCSTR lpcszName)

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WListItemText(LPCSTR lpcszName, int iIndex, CString& strBuffer)
// Description: A wrapper for the MS-Test function WListItemText. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: iIndex An integer that specifies the 1-based index into the list box.
// Param: strBuffer A CString reference that will contain the text of the list box item.
// END_HELP_COMMENT
void CMSTest::WListItemText     (LPCSTR lpcszName, int iIndex, CString& strBuffer)
	{   
     ::WListItemText((LPSTR)lpcszName, iIndex, strBuffer.GetBufferSetLength(BUFFER), TIMEOUT);
		strBuffer.ReleaseBuffer(-1) ; }

// BEGIN_HELP_COMMENT
// Function: int  CMSTest::WListItemLen(LPCSTR lpcszName, int iIndex)
// Description: A wrapper for the MS-Test function WListItemLen. See the MS-Test documentation for a description of this function.
// Return: An integer that contains the length of the list box item.
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: iIndex An integer that specififes the 1-based index into the list box.
// END_HELP_COMMENT
int  CMSTest::WListItemLen      (LPCSTR lpcszName, int iIndex)
	{       return ::WListItemLen((LPSTR)lpcszName, iIndex, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::WListItemExists(LPCSTR lpcszName, LPCSTR lpcszItem)
// Description: A wrapper for the MS-Test function WListItemExists. See the MS-Test documentation for a description of this function.
// Return: An integer that indicates whether the list box item exists.
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: lpcszItem A pointer to a string that contains the name of the list box item.
// END_HELP_COMMENT
int  CMSTest::WListItemExists   (LPCSTR lpcszName, LPCSTR lpcszItem)
	{ 
		int nitems=WListCount(lpcszName);
#ifdef _DEBUG
		LOG->Comment("Trying CMSTest::List(%s) ItemExists(%s):%d items...",
			lpcszName,lpcszItem,nitems);
#endif
		char strItem[1024]; //should be enough
		int item;
		for(item=1;item<=nitems;item++)
		{			
//			WListItemText(lpcszName, item, strItem); does not work quite, we need shorter timeout
			::WListItemText((LPSTR)lpcszName, item, strItem, 20); //enough timeout
			if(strcmp(lpcszItem,strItem)!=0)
			{
#ifdef _DEBUG
				LOG->Comment("\tItem#%d: (%s), no match...",item,strItem);
#endif
			}
			else
			{
#ifdef _DEBUG
				LOG->Comment("\tItem#%d: (%s), Found!!!",item,strItem);
#endif
				return item;
			}
		}
		return 0;
//	this MSTEST40 function not work here 
//	return ::WListItemExists((LPSTR)lpcszName, (LPSTR)lpcszItem, TIMEOUT); 
	}

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WListItemClk(LPCSTR lpcszName, int iIndex, int nOffset /*=0*/)
// Description: A wrapper for the MS-Test function WListItemClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: iIndex An integer that specifies the 1-based index of the list box item.
// Param: nOffset An integer that specifies the offset. (Default value is 0.)
// END_HELP_COMMENT
void CMSTest::WListItemClk (LPCSTR lpcszName, int iIndex, int nOffset /*=0*/)
	{       char acBuf[10];
		wsprintf(acBuf, "@%d", iIndex);
		WaitStepInstructions("Click item #%d of listbox '%s'", iIndex, lpcszName);
		::WListItemClkEx((LPSTR)lpcszName, acBuf, nOffset, TIMEOUT); 
	}

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WListItemClk(LPCSTR lpcszName, LPCSTR szItem, int nOffset /*=0*/)
// Description: A wrapper for the MS-Test function WListItemClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: szItem A pointer to a string taht contains the name of the list box item.
// Param: nOffset An integer that specifies the offset. (Default value is 0.)
// END_HELP_COMMENT
void CMSTest::WListItemClk  (LPCSTR lpcszName, LPCSTR szItem, int nOffset /*=0*/)
	{       WaitStepInstructions("Click item '%s' of listbox '%s'", szItem, lpcszName);
//      ::WListItemClkEx((LPSTR)lpcszName, (LPSTR)szItem, nOffset); }
      ::WListItemClkEx((LPSTR)lpcszName, (LPSTR)szItem, nOffset, TIMEOUT); }


// BEGIN_HELP_COMMENT
// Function: void CMSTest::WListItemCtrlClk(LPCSTR lpcszName, int iIndex, int nOffset /*=0*/)
// Description: A wrapper for the MS-Test function WListItemCtrlClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: iIndex An integer that specifies the 1-based index of the list box item.
// Param: nOffset An integer that specifies the offset. (Default value is 0.)
// END_HELP_COMMENT
void CMSTest::WListItemCtrlClk  (LPCSTR lpcszName, int iIndex, int nOffset /*=0*/)
	{       char acBuf[10];
		wsprintf(acBuf, "@%d", iIndex);
		WaitStepInstructions("Ctrl-click item #%d of listbox '%s'", iIndex, lpcszName);
      ::WListItemCtrlClkEx((LPSTR)lpcszName, acBuf, nOffset,TIMEOUT); 
	}


// BEGIN_HELP_COMMENT
// Function: void CMSTest::WListItemCtrlClk(LPCSTR lpcszName, LPCSTR szItem, int nOffset /*=0*/)
// Description: A wrapper for the MS-Test function WListItemCtrlClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: szItem A pointer to a string taht contains the name of the list box item.
// Param: nOffset An integer that specifies the offset. (Default value is 0.)
// END_HELP_COMMENT
void CMSTest::WListItemCtrlClk  (LPCSTR lpcszName, LPCSTR szItem, int nOffset /*=0*/)
	{       WaitStepInstructions("Ctrl-click item '%s' of listbox '%s'", szItem, lpcszName);
      ::WListItemCtrlClkEx((LPSTR)lpcszName, (LPSTR)szItem, nOffset, TIMEOUT); }


// BEGIN_HELP_COMMENT
// Function: void CMSTest::WListItemShftClk(LPCSTR lpcszName, int iIndex, int nOffset /*=0*/)
// Description: A wrapper for the MS-Test function WListItemShftClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: iIndex An integer that specifies the 1-based index of the list box item.
// Param: nOffset An integer that specifies the offset. (Default value is 0.)
// END_HELP_COMMENT
void CMSTest::WListItemShftClk  (LPCSTR lpcszName, int iIndex, int nOffset /*=0*/)
	{       char acBuf[10];
		wsprintf(acBuf, "@%d", iIndex);
		WaitStepInstructions("Shift-click item #%d of listbox '%s'", iIndex, lpcszName);
      ::WListItemShftClkEx((LPSTR)lpcszName, acBuf, nOffset, TIMEOUT);
	}


// BEGIN_HELP_COMMENT
// Function: void CMSTest::WListItemShftClk(LPCSTR lpcszName, LPCSTR szItem, int nOffset /*=0*/)
// Description: A wrapper for the MS-Test function WListItemShftClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: szItem A pointer to a string taht contains the name of the list box item.
// Param: nOffset An integer that specifies the offset. (Default value is 0.)
// END_HELP_COMMENT
void CMSTest::WListItemShftClk  (LPCSTR lpcszName, LPCSTR szItem, int nOffset /*=0*/)
	{       WaitStepInstructions("Shift-click item '%s' of listbox '%s'", szItem, lpcszName);
      ::WListItemShftClkEx((LPSTR)lpcszName, (LPSTR)szItem, nOffset, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WListItemDblClk(LPCSTR lpcszName, int iIndex, int nOffset /*=0*/)
// Description: A wrapper for the MS-Test function WListItemDblClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: iIndex An integer that specifies the 1-based index of the list box item.
// Param: nOffset An integer that specifies the offset. (Default value is 0.)
// END_HELP_COMMENT
void CMSTest::WListItemDblClk  (LPCSTR lpcszName, int iIndex, int nOffset /*=0*/)
	{       char acBuf[10];
		wsprintf(acBuf, "@%d", iIndex);
		WaitStepInstructions("Double-click item #%d of listbox '%s'", iIndex, lpcszName);
      ::WListItemDblClkEx((LPSTR)lpcszName, acBuf, nOffset, TIMEOUT);
	}
      
// BEGIN_HELP_COMMENT
// Function: void CMSTest::WListItemDblClk(LPCSTR lpcszName, LPCSTR szItem, int nOffset /*=0*/)
// Description: A wrapper for the MS-Test function WListItemDblClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: szItem A pointer to a string taht contains the name of the list box item.
// Param: nOffset An integer that specifies the offset. (Default value is 0.)
// END_HELP_COMMENT
void CMSTest::WListItemDblClk  (LPCSTR lpcszName, LPCSTR szItem, int nOffset /*=0*/)
	{       WaitStepInstructions("Double-click item '%s' of listbox '%s'", szItem, lpcszName);
      ::WListItemDblClkEx((LPSTR)lpcszName, (LPSTR)szItem, nOffset, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::WListSelCount(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WListSelCount. See the MS-Test documentation for a description of this function.
// Return: An integer that contains the number of selected items in the list box.
// Param: lpcszName A pointer to a string that contains the list box name.
// END_HELP_COMMENT
int  CMSTest::WListSelCount     (LPCSTR lpcszName)
	{       return ::WListSelCount((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WListSelItems(LPCSTR lpcszName, LPINT lpIntArray)
// Description: A wrapper for the MS-Test function WListSelItems. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the list box name.
// Param: lpIntArray An array of integers that specifies the items to select in the list box.
// END_HELP_COMMENT
void CMSTest::WListSelItems     (LPCSTR lpcszName, LPINT lpIntArray)
	{      ::WListSelItems((LPSTR)lpcszName, lpIntArray, TIMEOUT); }

//void WListClear        (LPCSTR lpcszName)
//void WListAddItem      (LPCSTR lpcszName, LPCSTR lpcszItem)
//void WListDelItem      (LPCSTR lpcszName, int iIndex)
//void WListDelItem      (LPCSTR lpcszName, LPCSTR lpcszItem)

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WListEnabled(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WListEnabled. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the list box is enabled.
// Param: lpcszName A pointer to a string that contains the list box name.
// END_HELP_COMMENT
BOOL CMSTest::WListEnabled      (LPCSTR lpcszName)
	{       return ::WListEnabled((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WListSetFocus(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WListSetFocus. See the MS-Test documentation for a description of this function.
// Return: <void>
// Param: lpcszName A pointer to a string that contains the list box name.
// END_HELP_COMMENT
void CMSTest::WListSetFocus     (LPCSTR lpcszName)
	{       WaitStepInstructions("Set focus to listbox '%s'", lpcszName);
      ::WListSetFocus((LPSTR)lpcszName, TIMEOUT); }

//---------------------------------------------------------------------------
// ListView routines
//---------------------------------------------------------------------------

// BEGIN_HELP_COMMENT
// Function: int  CMSTest::WViewCount(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WListCount. See the MS-Test documentation for a description of this function.
// Return: An integer that contains the number of items in the list.
// Param: lpcszName A pointer to a string that contains the list box name.
// END_HELP_COMMENT
int  CMSTest::WViewCount        (LPCSTR lpcszName)
	{       return ::WViewCount((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WListEnabled(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WViewEnabled. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the listview is enabled.
// Param: lpcszName A pointer to a string that contains the listview name.
// END_HELP_COMMENT
BOOL CMSTest::WViewEnabled      (LPCSTR lpcszName)
	{       return ::WViewEnabled((LPSTR)lpcszName, TIMEOUT); }


// BEGIN_HELP_COMMENT
// Function: void CMSTest::WViewSetFocus(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WViewSetFocus. See the MS-Test documentation for a description of this function.
// Return: <void>
// Param: lpcszName A pointer to a string that contains the listview name.
// END_HELP_COMMENT
void CMSTest::WViewSetFocus     (LPCSTR lpcszName)
	{       WaitStepInstructions("Set focus to listview '%s'", lpcszName);
      ::WViewSetFocus((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WViewSelItem(LPCSTR lpcszName, LPINT lpIntArray)
// Description: A wrapper for the MS-Test function WViewSelItems. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the listview name.
// Param: lpcszItem ListView item name
// END_HELP_COMMENT
int CMSTest::WViewSelItem     (LPCSTR lpcszName, LPCSTR lpcszItem)
	{      return ::WViewSelItem((LPSTR)lpcszName, (LPSTR)lpcszItem, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WViewItemClk(LPCSTR lpcszName, LPCSTR lpcszItem)
// Description: A wrapper for the MS-Test function WViewItemClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the listview name.
// Param: lpcszItem ListView item name
// END_HELP_COMMENT
void CMSTest::WViewItemClk (LPCSTR lpcszName, LPCSTR lpcszItem)
	{       
		WaitStepInstructions("Click item '%s' of listview '%s'", lpcszItem, lpcszName);
		::WViewItemClk((LPSTR)lpcszName, (LPSTR)lpcszItem, VK_LBUTTON, TIMEOUT); 
	}

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WViewItemClk(LPCSTR lpcszName, int iIndex)
// Description: A wrapper for the MS-Test function WViewItemClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the listview name.
// Param: iIndex An integer that specifies the 1-based index of the listview item.
// END_HELP_COMMENT
void CMSTest::WViewItemClk (LPCSTR lpcszName, int iIndex)
	{       char acBuf[10];
		wsprintf(acBuf, "@%d", iIndex);
		WaitStepInstructions("Click item #%d of listview '%s'", iIndex, lpcszName);
		::WViewItemClk((LPSTR)lpcszName, acBuf, VK_LBUTTON, TIMEOUT); 
	}

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WViewItemExists(LPCSTR lpcszName, LPINT lpIntArray)
// Description: A wrapper for the MS-Test function WViewItemExists. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the listview name.
// Param: lpcszItem ListView item name
// END_HELP_COMMENT
BOOL CMSTest::WViewItemExists	(LPCSTR lpcszName,  LPCSTR lpcszItem)
	{      return (WViewItemIndex(lpcszName, lpcszItem)>0); } //::WViewItemExists does not work here

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WViewItemIndex(LPCSTR lpcszName, LPINT lpIntArray)
// Description: A wrapper for the MS-Test function WViewItemIndex. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the listview name.
// Param: lpcszItem ListView item name
// END_HELP_COMMENT
int  CMSTest::WViewItemIndex	(LPCSTR lpcszName,  LPCSTR lpcszItem)
	{      return ::WViewItemIndex((LPSTR)lpcszName, (LPSTR)lpcszItem, TIMEOUT); }
//---------------------------------------------------------------------------
// Combobox routines
//---------------------------------------------------------------------------

//void WComboSetClass    (LPCSTR lpcszName)
//void WComboSetLBClass  (LPCSTR lpcszName)

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WComboExists(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WComboExists. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the combo box exists.
// Param: lpcszName A pointer to a string that contains the combo box name.
// END_HELP_COMMENT
BOOL CMSTest::WComboExists      (LPCSTR lpcszName)
	{       return ::WComboExists((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::WComboCount(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WComboCount. See the MS-Test documentation for a description of this function.
// Return: An integer that contains the number of items in the combo box.
// Param: lpcszName A pointer to a string that contains the combo box name.
// END_HELP_COMMENT
int  CMSTest::WComboCount       (LPCSTR lpcszName)
	{       return ::WComboCount((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WComboText(LPCSTR lpcszName, CString& strBuff)
// Description: A wrapper for the MS-Test function WComboText. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the combo box name.
// Param: strBuff A CString reference that will contain the text of the combo box.
// END_HELP_COMMENT
void CMSTest::WComboText        (LPCSTR lpcszName, CString& strBuff)
	{      ::WComboText((LPSTR)lpcszName, strBuff.GetBufferSetLength(BUFFER), TIMEOUT);
		strBuff.ReleaseBuffer(-1); }

// BEGIN_HELP_COMMENT
// Function: int  CMSTest::WComboLen(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WComboLen. See the MS-Test documentation for a description of this function.
// Return: An integer that contains the length of the combo box.
// Param: lpcszName A pointer to a string that contains the combo box name.
// END_HELP_COMMENT
int  CMSTest::WComboLen         (LPCSTR lpcszName)
	{       return ::WComboLen((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: int  CMSTest::WComboIndex(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WComboIndex. See the MS-Test documentation for a description of this function.
// Return: An integer that contains the combo box index.
// Param: lpcszName A pointer to a string that contains the combo box name.
// END_HELP_COMMENT
int  CMSTest::WComboIndex       (LPCSTR lpcszName)
	{       return ::WComboIndex((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WComboSetText (LPCSTR lpcszName, LPCSTR lpcszText)
// Description: A wrapper for the MS-Test function WComboSetText. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the combo box name.
// Param: lpcszText A pointer to a string that contains the text to set in the combo box.
// END_HELP_COMMENT
void CMSTest::WComboSetText (LPCSTR lpcszName, LPCSTR lpcszText)
	{
	WaitStepInstructions("Set text of combobox '%s' to '%s'", lpcszName, lpcszText);
	//::WComboSetFocus((LPSTR)lpcszName, TIMEOUT);
	//DoKeys(lpcszText,TRUE);
	::WComboSetText((LPSTR)lpcszName, (LPSTR)lpcszText, TIMEOUT);
	}

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WComboSelText(LPCSTR lpcszName, CString& strBuff)
// Description: A wrapper for the MS-Test function WComboSelText. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the combo box name.
// Param: strBuff A CString reference that will contain the selected text in the combo box.
// END_HELP_COMMENT
void CMSTest::WComboSelText     (LPCSTR lpcszName, CString& strBuff)
	{ 
		::WComboSelText((LPSTR)lpcszName, strBuff.GetBufferSetLength(BUFFER), TIMEOUT);
		strBuff.ReleaseBuffer(-1); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::WComboSelLen(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WComboSelLen. See the MS-Test documentation for a description of this function.
// Return: An integer that contains the length of the selected text in the combo box.
// Param: lpcszName A pointer to a string that contains the combo box name.
// END_HELP_COMMENT
int  CMSTest::WComboSelLen      (LPCSTR lpcszName)
	{       return ::WComboSelLen((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WComboItemText(LPCSTR lpcszName, int iIndex, CString& strBuff)
// Description: A wrapper for the MS-Test function WComboItemText. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the combo box name.
// Param: iIndex An integer that specifies the 1-based index of the combo box item.
// Param: strBuff A CString reference that will contain the text of the combo box item.
// END_HELP_COMMENT
void CMSTest::WComboItemText    (LPCSTR lpcszName, int iIndex, CString& strBuff)
	{ ::WComboItemText((LPSTR)lpcszName, iIndex, strBuff.GetBufferSetLength(BUFFER), TIMEOUT);
		strBuff.ReleaseBuffer(-1); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::WComboItemLen(LPCSTR lpcszName, int iIndex)
// Description: A wrapper for the MS-Test function WComboItemLen. See the MS-Test documentation for a description of this function.
// Return: An integer that contains the length of the combo box item.
// Param: lpcszName A pointer to a string that contains the combo box name.
// Param: iIndex An integer that specifies the 1-based index of the combo box item.
// END_HELP_COMMENT
int  CMSTest::WComboItemLen     (LPCSTR lpcszName, int iIndex)
	{       return ::WComboItemLen((LPSTR)lpcszName, iIndex, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::WComboItemExists(LPCSTR lpcszName, LPCSTR lpcszItem)
// Description: A wrapper for the MS-Test function WComboItemExists. See the MS-Test documentation for a description of this function.
// Return: An integer that indicates whether the combo box item exists.
// Param: lpcszName A pointer to a string that contains the combo box name.
// Param: lpcszItem A pointer to a string that contains the name of the combo box item.
// END_HELP_COMMENT
int  CMSTest::WComboItemExists  (LPCSTR lpcszName, LPCSTR lpcszItem)
	{
		int nitems=WComboCount(lpcszName);
#ifdef _DEBUG
		LOG->Comment("Trying CMSTest::Combo(%s) ItemExists(%s):%d items...",
			lpcszName,lpcszItem,nitems);
#endif
		char strItem[1024];
		int item;
		for(item=1;item<=nitems;item++)
		{			
//			WComboItemText(lpcszName, item, strItem); does not work quite, we need shorter timeout
			::WComboItemText((LPSTR)lpcszName, item, strItem, 20); //shld be enough
			if(strcmp(lpcszItem,strItem)!=0)
			{
#ifdef _DEBUG
				LOG->Comment("\tItem#%d: (%s), no match...",item,strItem);
#endif
			}
			else
			{
#ifdef _DEBUG
				LOG->Comment("\tItem#%d: (%s), Found!!!",item,strItem);
#endif
				return item;
			}
		}
		return 0;
//	this MSTEST40 function not work here 
//	return ::WComboItemExists((LPSTR)lpcszName, (LPSTR)lpcszItem, TIMEOUT);
	}

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WComboItemClk(LPCSTR lpcszName, int iIndex)
// Description: A wrapper for the MS-Test function WComboItemClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the combo box name.
// Param: iIndex An integer that specifies the 1-based index of the combo box item.
// END_HELP_COMMENT
void CMSTest::WComboItemClk(LPCSTR lpcszName, int iIndex)
	{       char acBuf[10];
		wsprintf(acBuf, "@%d", iIndex);
		WaitStepInstructions("Click item #%d of combobox '%s'", iIndex, lpcszName);
      ::WComboItemClk((LPSTR)lpcszName, acBuf, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WComboItemClk(LPCSTR lpcszName, LPCSTR szItem)
// Description: A wrapper for the MS-Test function WComboItemClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the combo box name.
// Param: szItem A pointer to a string that contains the name of the combo box item.
// END_HELP_COMMENT
void CMSTest::WComboItemClk(LPCSTR lpcszName, LPCSTR szItem)
	{       WaitStepInstructions("Click item '%s' of combobox '%s'", szItem, lpcszName);
      ::WComboItemClk((LPSTR)lpcszName, (LPSTR)szItem, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WComboItemDblClk(LPCSTR lpcszName, int iIndex)
// Description: A wrapper for the MS-Test function WComboItemDblClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the combo box name.
// Param: iIndex An integer that specifies the 1-based index of the combo box item.
// END_HELP_COMMENT
void CMSTest::WComboItemDblClk(LPCSTR lpcszName, int iIndex)
	{       char acBuf[10];
		wsprintf(acBuf, "@%d", iIndex);
		WaitStepInstructions("Double-click item #%d of combobox '%s'", iIndex, lpcszName);
      ::WComboItemDblClk((LPSTR)lpcszName, acBuf, TIMEOUT); }
// BEGIN_HELP_COMMENT
// Function: void CMSTest::WComboItemDblClk(LPCSTR lpcszName, LPCSTR szItem)
// Description: A wrapper for the MS-Test function WComboItemDblClk. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the combo box name.
// Param: szItem A pointer to a string that contains the name of the combo box item.
// END_HELP_COMMENT
void CMSTest::WComboItemDblClk(LPCSTR lpcszName, LPCSTR szItem)
	{       WaitStepInstructions("Double-click item '%s' of combobox '%s'", szItem, lpcszName);
      ::WComboItemDblClk((LPSTR)lpcszName, (LPSTR)szItem, TIMEOUT); }

//void WComboClear       (LPCSTR lpcszName)
//void WComboAddItem     (LPCSTR lpcszName, LPCSTR lpcszText)
//void WComboDelItem     (LPCSTR lpcszName, int iIndex)
//void WComboDelItem     (LPCSTR lpcszName, LPCSTR lpcszItem)

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WComboEnabled(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WComboEnabled. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the combo box is enabled.
// Param: lpcszName A pointer to a string that contains the combo box name.
// END_HELP_COMMENT
BOOL CMSTest::WComboEnabled     (LPCSTR lpcszName)
	{       return ::WComboEnabled((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WComboSetFocus(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WComboSetFocus. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the combo box name.
// END_HELP_COMMENT
void CMSTest::WComboSetFocus    (LPCSTR lpcszName)
	{       WaitStepInstructions("Set focus to combobox '%s'", lpcszName);
      ::WComboSetFocus((LPSTR)lpcszName, TIMEOUT); }

//---------------------------------------------------------------------------
// Editbox routines
//---------------------------------------------------------------------------

//void WEditSetClass (LPCSTR lpcszClassName)

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WEditExists(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WEditExists. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the edit box exists.
// Param: lpcszName A pointer to a string that contains the edit box name.
// END_HELP_COMMENT
BOOL CMSTest::WEditExists   (LPCSTR lpcszName)
	{       return ::WEditExists((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: long CMSTest::WEditLen(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WEditLen. See the MS-Test documentation for a description of this function.
// Return: A long that contains the length of the text in the edit box.
// Param: lpcszName A pointer to a string that contains the edit box name.
// END_HELP_COMMENT
long CMSTest::WEditLen      (LPCSTR lpcszName)
	{       return ::WEditLen((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WEditText(LPCSTR lpcszName, CString& strBuff)
// Description: A wrapper for the MS-Test function WEditText. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the edit box name.
// Param: strBuff A CString reference that will contain the text from the edit box.
// END_HELP_COMMENT
void CMSTest::WEditText     (LPCSTR lpcszName, CString& strBuff)
	{ ::WEditText((LPSTR)lpcszName, strBuff.GetBufferSetLength(BUFFER), TIMEOUT);
		strBuff.ReleaseBuffer(-1); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WEditSetText(LPCSTR lpcszName, LPCSTR lpcszText)
// Description: A wrapper for the MS-Test function WEditSetText. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the edit box name.
// Param: lpcszText A pointer to a string that contains the text to set in the edit box.
// END_HELP_COMMENT
void CMSTest::WEditSetText  (LPCSTR lpcszName, LPCSTR lpcszText)
	{       WaitStepInstructions("Set text of edit '%s' to '%s'", lpcszName, lpcszText);
      ::WEditSetText((LPSTR)lpcszName, (LPSTR)lpcszText, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WEditSelText(LPCSTR lpcszName, CString& strBuff)
// Description: A wrapper for the MS-Test function WEditSelText. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the edit box name.
// Param: strBuff A CString reference that will contain the selected text in the edit box.
// END_HELP_COMMENT
void CMSTest::WEditSelText  (LPCSTR lpcszName, CString& strBuff)
	{ 
		::WEditSetText((LPSTR)lpcszName, strBuff.GetBufferSetLength(BUFFER), TIMEOUT);
		strBuff.ReleaseBuffer(-1); }

// BEGIN_HELP_COMMENT
// Function: long CMSTest::WEditSelLen(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WEditSelLen. See the MS-Test documentation for a description of this function.
// Return: A long that contains the length of the selected text in the edit box.
// Param: lpcszName A pointer to a string that contains the edit box name.
// END_HELP_COMMENT
long CMSTest::WEditSelLen   (LPCSTR lpcszName)
	{       return ::WEditSelLen((LPSTR)lpcszName, TIMEOUT); }

//void WEditLineText (LPCSTR lpcszName, long lIndex, CString& strBuff)
//long WEditLineLen  (LPCSTR lpcszName, long lIndex)
//long WEditPos      (LPCSTR lpcszName)
//long WEditLine     (LPCSTR lpcszName)
//long WEditChar     (LPCSTR lpcszName)
//long WEditFirst    (LPCSTR lpcszName)
//long WEditLines    (LPCSTR lpcszName)

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WEditClick(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WEditClick. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the edit box name.
// END_HELP_COMMENT
void CMSTest::WEditClick    (LPCSTR lpcszName)
	{       WaitStepInstructions("Click edit '%s'", lpcszName);
      ::WEditClick((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WEditEnabled(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WEditEnabled. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the edit box is enabled.
// Param: lpcszName A pointer to a string that contains the edit box name.
// END_HELP_COMMENT
BOOL CMSTest::WEditEnabled  (LPCSTR lpcszName)
	{       return ::WEditEnabled((LPSTR)lpcszName, TIMEOUT); }

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WEditSetFocus(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WEditSetFocus. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the edit box name.
// END_HELP_COMMENT
void CMSTest::WEditSetFocus (LPCSTR lpcszName)
	{       WaitStepInstructions("Set focus to edit '%s'", lpcszName);
      ::WEditSetFocus((LPSTR)lpcszName, TIMEOUT); }

//---------------------------------------------------------------------------
// Static control routines
//---------------------------------------------------------------------------

// BEGIN_HELP_COMMENT
// Function: BOOL CMSTest::WStaticExists(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WStaticExists. See the MS-Test documentation for a description of this function.
// Return: A Boolean value that indicates whether the static control exists.
// Param: lpcszName A pointer to a string that contains the static control's name.
// END_HELP_COMMENT
BOOL CMSTest::WStaticExists   (LPCSTR lpcszName)
	{       
	WaitStepInstructions("Looking for Static Control '%s'", lpcszName);
    if(::WFndWndC((LPSTR)lpcszName, (LPSTR)"STATIC", FW_PART|FW_ACTIVE, TIMEOUT)!=NULL)
		return TRUE;
	return FALSE;
//WStaticExists is not exported from MSTEST40
//	return ::WStaticExists((LPSTR)lpcszName, TIMEOUT) ;
	}
		
// BEGIN_HELP_COMMENT
// Function: long CMSTest::WStaticLen(LPCSTR lpcszName)
// Description: A wrapper for the MS-Test function WStaticLen. See the MS-Test documentation for a description of this function.
// Return: A long that contains the length of the static control.
// Param: lpcszName A pointer to a string that contains the static control's name.
// END_HELP_COMMENT
//MSTEST40.DLL does not export WStaticLen
//long CMSTest::WStaticLen         (LPCSTR lpcszName)
//	{       return ::WStaticLen         ((LPSTR) lpcszName, TIMEOUT) ;}

// BEGIN_HELP_COMMENT
// Function: void CMSTest::WStaticText(LPCSTR lpcszName, CString& strBuff)
// Description: A wrapper for the MS-Test function WStaticText. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: lpcszName A pointer to a string that contains the static control's name.
// Param: strBuff A CString reference that will contain the text from the static control.
// END_HELP_COMMENT
void CMSTest::WStaticText     (LPCSTR lpcszName, CString& strBuff)
	{
		HWND hwnd=::WFndWndC((LPSTR)lpcszName, (LPSTR)"STATIC", FW_ACTIVE, TIMEOUT);
		if(hwnd!=NULL)
		    WGetText(hwnd, strBuff);
//	    ::WStaticText((LPSTR)lpcszName, strBuff.GetBufferSetLength(BUFFER), TIMEOUT);
//		strBuff.ReleaseBuffer(-1); 
	}

//---------------------------------------------------------------------------
// Error routines
//---------------------------------------------------------------------------

//int  WError     (void)
//void WErrorSet  (int errValue)
//void WErrorText (CString& strBuff)
//int  WErrorLen  (void)
//void WErrorTrap (int iTrapID, int iAction, FARPROC lpfnCallBack)

/////////////////////////////////////////////////////////////////////////////
// From TESTEVNT.H
//

// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueKeys(LPCSTR sz)
// Description: A wrapper for the MS-Test function QueKeys. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: sz A pointer to a string that contains the keys.
// END_HELP_COMMENT
int CMSTest::QueKeys (LPCSTR sz)
	{       return ::QueKeys((LPSTR)sz); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueKeyDn(LPCSTR sz)
// Description: A wrapper for the MS-Test function QueKeyDn. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: sz A pointer to a string that contains the keys.
// END_HELP_COMMENT
int CMSTest::QueKeyDn (LPCSTR sz)
	{       return ::QueKeyDn((LPSTR)sz); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueKeyUp(LPCSTR sz)
// Description: A wrapper for the MS-Test function QueKeyUp. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: sz A pointer to a string that contains the keys.
// END_HELP_COMMENT
int CMSTest::QueKeyUp (LPCSTR sz)
	{       return ::QueKeyUp((LPSTR)sz); }


void ConvertLiteral(LPCSTR sz, CString &str)
{
	str.Empty();
	while( *sz != '\0' )
	{                                               
		if( strchr("+^%(){}~|[]", *sz) == NULL )
		{
			str += *sz++;
			if( IsDBCSLeadByte(*sz) )
				str += *sz++;
		}
		else
		{
			str += "{";             // enclose every control character in {}
			str += *sz++;   // so MS-Test to treats them literally
			str += "}";
		}
	}
}

//#define __SENDINPUT__
#ifdef __SENDINPUT__
#include "sendinpt.cpp"
#endif
// BEGIN_HELP_COMMENT
// Function: int CMSTest::DoKeys(LPCSTR sz, BOOL bLiteral /*=FALSE*/, int nWaitForIdleTime = 5000)
// Description: A wrapper for the MS-Test function DoKeys. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: sz A pointer to a string that contains the keys.
// Param: bLiteral A Boolean value that indicates whether to translate the keys given in sz as DoKeys does (FALSE) or interpret them literally (TRUE).
// Param: nWaitForIdleTime - An amount of time to wait after generating keystrokes for the process to become idle
// END_HELP_COMMENT
int CMSTest::DoKeys (LPCSTR sz, BOOL bLiteral /*=FALSE*/, int nWaitForIdleTime /* = 5000 */)
{
	int ret=0;                        // Return value (whatever ::DoKeys returns.)
	WaitStepInstructions("DoKeys%s:'%s'", (bLiteral?"Lit":""), sz);

#ifdef __SENDINPUT__
	::DoKeyshwnd(NULL, sz, bLiteral);
//	Sleep(500);
#else
	if( !bLiteral )
		ret = ::DoKeys((LPSTR)sz);
	else
	{
		CString str;
		ConvertLiteral(sz, str);
		ret = ::DoKeys((LPSTR)(LPCSTR)str);
	}
#endif
	if (g_hTargetProc)              // This avoids many potential timing problems when using DoKeys to drive an application.
		WaitForInputIdle(g_hTargetProc, nWaitForIdleTime);

	return ret;
}

// BEGIN_HELP_COMMENT
// Function: int CMSTest::DoKeyshWnd(HWND hwnd, LPCSTR sz)
// Description: A wrapper for the MS-Test function DoKeyshWnd. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: hwnd An HWND that specifies the window to send the keys to.
// Param: sz A pointer to a string that contains the keys.
// END_HELP_COMMENT
int CMSTest::DoKeyshWnd (HWND hwnd, LPCSTR sz, BOOL bLiteral /*=FALSE*/, int nWaitForIdleTime /* = 5000 */)
{
	int ret=0;                        // Return value (whatever ::DoKeys returns.)
	WaitStepInstructions("DoKeyWnd%s:'%s' to 0x%X", (bLiteral?"Lit":""), sz, hwnd);

#ifdef __SENDINPUT__
	::DoKeyshwnd(hwnd, sz, bLiteral);
//	Sleep(500);
#else
	if( !bLiteral )
		ret = ::DoKeyshWnd(hwnd, (LPSTR)sz);
	else
	{
		CString str(sz);
		ConvertLiteral(sz, str);
		ret = ::DoKeyshWnd(hwnd, (LPSTR)(LPCSTR)str);
	}
#endif
	if (g_hTargetProc)              // This avoids many potential timing problems when using DoKeys to drive an application.
		WaitForInputIdle(g_hTargetProc, nWaitForIdleTime);

	return ret;
}

// BEGIN_HELP_COMMENT
// Function: void CMSTest::QuePause(DWORD dw)
// Description: A wrapper for the MS-Test function QuePause. See the MS-Test documentation for a description of this function.
// Return: <none>
// Param: dw A DWORD that specifies the number of milliseconds to pause.
// END_HELP_COMMENT
void CMSTest::QuePause (DWORD dw)
	{      ::QuePause(dw); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueSetSpeed(WORD w)
// Description: A wrapper for the MS-Test function QueSetSpeed. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: w A WORD that specifies the queue speed.
// END_HELP_COMMENT
int  CMSTest::QueSetSpeed (WORD w)
	{       return ::QueSetSpeed(w); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueSetFocus(HWND hwnd)
// Description: A wrapper for the MS-Test function QueSetFocus. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: hwnd An HWND that specifies the window to set focus to.
// END_HELP_COMMENT
int  CMSTest::QueSetFocus (HWND hwnd)
	{       return ::QueSetFocus(hwnd); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueSetRelativeWindow(HWND hwnd)
// Description: A wrapper for the MS-Test function QueSetRelativeWindow. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: hwnd An HWND that specifies the relative window.
// END_HELP_COMMENT
int  CMSTest::QueSetRelativeWindow (HWND hwnd)
	{       return ::QueSetRelativeWindow(hwnd); }

// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueMouseMove(POINT pt)
// Description: A wrapper for the MS-Test function QueMouseMove. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: pt A POINT value that specifies the point to move the mouse to.
// END_HELP_COMMENT
int  CMSTest::QueMouseMove (POINT pt)
	{       return ::QueMouseMove((WORD)pt.x, (WORD)pt.y); } 
	//return ::QueMouseMove(pt,0); }     //CHRISKOZ review didn't know what to put here

// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueMouseDn(int n, POINT pt)
// Description: A wrapper for the MS-Test function QueMouseDn. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: n An integer that specifies the button to push.
// Param: pt A POINT value that specifies the point.
// END_HELP_COMMENT
int  CMSTest::QueMouseDn (int n, POINT pt)
{     return ::QueMouseDn(n, (WORD)pt.x, (WORD)pt.y); }  
//	return ::QueMouseDn(n,pt, 0); }  //CHRISKOZ review didn't know what to put here


// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueMouseUp(int n, POINT pt)
// Description: A wrapper for the MS-Test function QueMouseUp. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: n An integer that specifies the button to release.
// Param: pt A POINT value that specifies the point.
// END_HELP_COMMENT
int  CMSTest::QueMouseUp (int n, POINT pt)
{     return ::QueMouseUp(n, (WORD)pt.x, (WORD)pt.y); }  
//		return ::QueMouseUp(n, pt, 0); }  //CHRISKOZ review didn't know what to put here    

// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueMouseClick(int n, POINT pt)
// Description: A wrapper for the MS-Test function QueMouseClick. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: n An integer that specifies the button to click.
// Param: pt A POINT value that specifies the point.
// END_HELP_COMMENT
int  CMSTest::QueMouseClick (int n, POINT pt)
{     return ::QueMouseClick(n, (WORD)pt.x, (WORD)pt.y); }       
//		return ::QueMouseClick(n, pt, 0); }   //CHRISKOZ review didn't know what to put here

// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueMouseDblClk(int n, POINT pt)
// Description: A wrapper for the MS-Test function QueMouseDblClk. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: n An integer that specifies the button to double click.
// Param: pt A POINT value that specifies the point.
// END_HELP_COMMENT
int  CMSTest::QueMouseDblClk (int n, POINT pt)
{     return ::QueMouseDblClk(n, (WORD)pt.x, (WORD)pt.y); }      
//		return ::QueMouseDblClk(n, pt, TIMEOUT); }  

// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueMouseDblDn(int n, POINT pt)
// Description: A wrapper for the MS-Test function QueMouseDblDn. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: n An integer that specifies the button to push.
// Param: pt A POINT value that specifies the point.
// END_HELP_COMMENT
int  CMSTest::QueMouseDblDn (int n, POINT pt)
{     return ::QueMouseDblDn(n, (WORD)pt.x, (WORD)pt.y); }       
//		return ::QueMouseDblDn(n, pt, TIMEOUT); }   

//int  TimeDelay (int)

// BEGIN_HELP_COMMENT
// Function: int CMSTest::QueFlush(BOOL bRestoreKeyState)
// Description: A wrapper for the MS-Test function QueFlush. See the MS-Test documentation for a description of this function.
// Return: An integer value.
// Param: bRestoreKeyState A Boolean value that specifies whether to restore the key state (TRUE) or not.
// END_HELP_COMMENT
int CMSTest::QueFlush (BOOL bRestoreKeyState)
	{ return ::QueFlush(bRestoreKeyState); }   // bRestoreKeyState ignored under NT

//void QueEmpty (void)
