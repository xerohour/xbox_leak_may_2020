///////////////////////////////////////////////////////////////////////////////
//	TESTWRAP.H
//
//	Created by :			Date :
//		DavidGa					8/14/93
//
//	Description :
//		Declaration of MS Test wrapping class
//

#ifndef __TESTWRAP_H__
#define __TESTWRAP_H__
#include "t4ctrl.h"		// MS Test DLL calls

#include "guixprt.h"

#pragma comment(lib, "mstest40.LIB")

#define TIMEOUT 3
//timeout in seconds for the functions in mstest40.DLL, which must have its timeout:
/*
	WFndWndC
	WEditClick
	WEditSetFocus
	WComboSetText
*/


class GUI_CLASS CMSTest
{
/////////////////////////////////////////////////////////////////////////////
// TEST struct conversions
public:
	static void Convert(WNDPOS const wndpos, CPoint& pt);
	static void Convert(CPoint const pt, WNDPOS& wndpos);
	static CPoint Convert(WNDPOS const wndpos);

	static void Convert(WNDSIZ const wndsiz, CSize& size);
	static void Convert(CSize const size, WNDSIZ& wndsiz);
	static CSize Convert(WNDSIZ const wndsiz);

	static void Convert(WNDPOSSIZ const wndpossiz, CRect& rc);
	static void Convert(CRect const rc, WNDPOSSIZ& wndpossiz);
	static CRect Convert(WNDPOSSIZ const wndpossiz);

/////////////////////////////////////////////////////////////////////////////
// TESTCTRL Wrappers
public:

//---------------------------------------------------------------------------
// Miscelaneous routines
//---------------------------------------------------------------------------

//static long WMessage        (HWND hWnd, UINT wMsg);
//static long WMessageW       (HWND hWnd, UINT wMsg, WPARAM wp);
//static long WMessageL       (HWND hWnd, UINT wMsg, LPARAM lp);
//static long WMessageWL      (HWND hWnd, UINT wMsg, WPARAM wp, LPARAM lp);
static HWND WGetFocus       (void);
//static void WDisplayInfo    (HWND, UINT);
//static void WGetInfo        (HWND, LPINFO);
//static void WStaticSetClass (LPSTR);
//static void WResetClasses   (void);
static BOOL WIsVisible      (HWND hwnd);
//static long WTextLen        (HWND hWnd);
static void WGetText        (HWND hwnd, CString& strBuffer);
//static void WSetText        (HWND hWnd, LPCSTR lpcszText);
//static int  WNumAltKeys     (void);
//static void WGetAltKeys     (CString& strBuff);
//static int  WNumDupAltKeys  (void);
//static void WGetDupAltKeys  (CString& strBuff);

//---------------------------------------------------------------------------
// General Window routines
//---------------------------------------------------------------------------

static HWND WFndWndWait   (LPCSTR lpcszCaption, UINT uFlags, UINT uSeconds);
static HWND WFndWndWaitC  (LPCSTR lpcszCaption, LPCSTR lpcszClass, UINT uFlags, UINT uSeconds);
static HWND WFndWnd       (LPCSTR lpcszCaption, UINT uFlags);
static HWND WFndWndC      (LPCSTR lpcszCaption, LPCSTR lpcszClass, UINT uFlags);
//static void WMinWnd       (HWND hWnd);
static void WMaxWnd       (HWND hWnd);
//static void WResWnd       (HWND hWnd);
//static void WSetWndPosSiz (HWND hWnd, int x,  int y, int w, int h);
//static void WSetWndPos    (HWND hWnd, int x,  int y);
static void  WSetWndSiz    (HWND hwnd, int w,  int h);
//static void WAdjWndPosSiz (HWND hWnd, int dx, int dy, int dw, int dh);
//static void WAdjWndPos    (HWND hWnd, int dx, int dy);
//static void WAdjWndSiz    (HWND hWnd, int dw, int dh);
//static void WGetWndPosSiz (HWND hWnd, LPWNDPOSSIZ lpWndPosSiz, BOOL fRelative);
//static void WGetWndPos    (HWND hWnd, LPWNDPOS    lpWndPos,    BOOL fRelative);
static CSize WGetWndSiz    (HWND hwnd);
static void  WSetActWnd    (HWND hwnd);
static HWND WGetActWnd    (HWND hwnd);
//static BOOL WIsMaximized  (HWND hWnd);
//static BOOL WIsMinimized  (HWND hWnd);

//---------------------------------------------------------------------------
// Menu routines
//---------------------------------------------------------------------------
static void WMenu              (LPCSTR lpcszName);
//static void FAR     WMenuEx            (LPCSTR lpcszName, ...)
static BOOL WMenuExists        (LPCSTR lpcszName);
static BOOL WMenuGrayed        (LPCSTR lpcszName);
static BOOL WMenuChecked       (LPCSTR lpcszName);
static BOOL WMenuEnabled       (LPCSTR lpcszName);
static int  WMenuCount         (void);
static void WMenuText          (LPCSTR lpcszName, CString& strBuffer);
//static int  WMenuLen           (LPCSTR lpcszName);
//static void WMenuFullText      (LPCSTR lpcszName, CString& strBuffer);
//static int  WMenuFullLen       (LPCSTR lpcszName);
static void WMenuEnd           (void);
//static BOOL WSysMenuExists     (HWND hWnd);
//static void WSysMenu           (HWND hWnd);
//static int  WMenuNumAltKeys    (void);
//static void WMenuGetAltKeys    (CString& strBuff);
//static int  WMenuNumDupAltKeys (void);
//static void WMenuGetDupAltKeys (CString& strBuff);
//static BOOL WMenuSeparator     (int iIndex);

// Obsolete.
//----------
//static void WMenuX             (int iIndex);
//static BOOL WMenuGrayedX       (int iIndex);
//static BOOL WMenuCheckedX      (int iIndex);
//static BOOL WMenuEnabledX      (int iIndex);

//---------------------------------------------------------------------------
// Command button routines.
//---------------------------------------------------------------------------

//static void WButtonSetClass (LPCSTR lpcszClassName);
static BOOL WButtonExists   (LPCSTR lpcszName);
static BOOL WButtonEnabled  (LPCSTR lpcszName);
static BOOL WButtonFocus    (LPCSTR lpcszName);
static void WButtonClick    (LPCSTR lpcszName);
//static void WButtonHide     (LPCSTR lpcszName);
//static void WButtonShow     (LPCSTR lpcszName);
//static void WButtonEnable   (LPCSTR lpcszName);
//static void WButtonDisable  (LPCSTR lpcszName);
//static BOOL WButtonDefault  (LPCSTR lpcszName);
//static int WButtonDefaults (void);
static void WButtonSetFocus (LPCSTR lpcszName);

//---------------------------------------------------------------------------
// CheckBox routines
//---------------------------------------------------------------------------
//static void WCheckSetClass (LPCSTR lpcszClassName);
static BOOL WCheckExists   (LPCSTR lpcszName);
static BOOL WCheckEnabled  (LPCSTR lpcszName);
static BOOL WCheckFocus    (LPCSTR lpcszName);
static int  WCheckState    (LPCSTR lpcszName);
static void WCheckClick    (LPCSTR lpcszName);
//static void WCheckHide     (LPCSTR lpcszName);
//static void WCheckShow     (LPCSTR lpcszName);
//static void WCheckEnable   (LPCSTR lpcszName);
//static void WCheckDisable  (LPCSTR lpcszName);
static void WCheckCheck    (LPCSTR lpcszName);
static void WCheckUnCheck  (LPCSTR lpcszName);
static void WCheckSetFocus (LPCSTR lpcszName);


//---------------------------------------------------------------------------
// Option Button routines
//---------------------------------------------------------------------------
//static void WOptionSetClass (LPCSTR lpcszClassName);
static BOOL WOptionExists   (LPCSTR lpcszName);
static BOOL WOptionEnabled  (LPCSTR lpcszName);
static BOOL WOptionFocus    (LPCSTR lpcszName);
static int  WOptionState    (LPCSTR lpcszName);
static void WOptionClick    (LPCSTR lpcszName);
//static void WOptionHide     (LPCSTR lpcszName);
//static void WOptionShow     (LPCSTR lpcszName);
//static void WOptionEnable   (LPCSTR lpcszName);
//static void WOptionDisable  (LPCSTR lpcszName);
static void WOptionSelect   (LPCSTR lpcszName);
static void WOptionSetFocus (LPCSTR lpcszName);


//---------------------------------------------------------------------------
// Listbox routines
//---------------------------------------------------------------------------
static void WListSetClass     (LPCSTR lpcszClassName);
static BOOL WListExists       (LPCSTR lpcszName);
static int  WListCount        (LPCSTR lpcszName);
static void WListText         (LPCSTR lpcszName, CString& strBuffer);
static int  WListLen          (LPCSTR lpcszName);
static int  WListIndex        (LPCSTR lpcszName);
//static int  WListTopIndex     (LPCSTR lpcszName);
static void WListItemText     (LPCSTR lpcszName, int iIndex, CString& strBuffer);
static int  WListItemLen      (LPCSTR lpcszName, int iIndex);
static int  WListItemExists   (LPCSTR lpcszName, LPCSTR lpcszItem);
static void WListItemClk      (LPCSTR lpcszName, int iIndex, int nOffset = 0);
static void WListItemClk      (LPCSTR lpcszName, LPCSTR lpcszItem, int nOffset = 0);
static void WListItemCtrlClk  (LPCSTR lpcszName, int iIndex, int nOffset = 0);
static void WListItemCtrlClk  (LPCSTR lpcszName, LPCSTR lpcszItem, int nOffset = 0);
static void WListItemShftClk  (LPCSTR lpcszName, int iIndex, int nOffset = 0);
static void WListItemShftClk  (LPCSTR lpcszName, LPCSTR lpcszItem, int nOffset = 0);
static void WListItemDblClk   (LPCSTR lpcszName, int iIndex, int nOffset = 0);
static void WListItemDblClk   (LPCSTR lpcszName, LPCSTR lpcszItem, int nOffset = 0);
static int  WListSelCount     (LPCSTR lpcszName);
static void WListSelItems     (LPCSTR lpcszName, LPINT lpIntArray);
//static void WListClear        (LPCSTR lpcszName);
//static void WListAddItem      (LPCSTR lpcszName, LPCSTR lpcszItem);
//static void WListDelItem      (LPCSTR lpcszName, int iIndex);
//static void WListDelItem      (LPCSTR lpcszName, LPCSTR lpcszItem);
static BOOL WListEnabled      (LPCSTR lpcszName);
static void WListSetFocus     (LPCSTR lpcszName);

//---------------------------------------------------------------------------
// Listview routines
//---------------------------------------------------------------------------
static int  WViewCount      (LPCSTR lpcszName);
static BOOL WViewEnabled    (LPCSTR lpcszName);
static void WViewSetFocus   (LPCSTR lpcszName);
static int  WViewSelItem    (LPCSTR lpcszName, LPCSTR lpcszItem);
static void WViewItemClk	(LPCSTR lpcszName, LPCSTR lpcszItem);
static void WViewItemClk	(LPCSTR lpcszName, int iIndex);
static BOOL WViewItemExists	(LPCSTR lpcszName,  LPCSTR lpcszItem);
static int  WViewItemIndex	(LPCSTR lpcszName,  LPCSTR lpcszItem);


//---------------------------------------------------------------------------
// Combobox routines
//---------------------------------------------------------------------------
//static void WComboSetClass    (LPCSTR lpcszName);
//static void WComboSetLBClass  (LPCSTR lpcszName);
static BOOL WComboExists      (LPCSTR lpcszName);
static int  WComboCount       (LPCSTR lpcszName);
static void WComboText        (LPCSTR lpcszName, CString& strBuff);
static int  WComboLen         (LPCSTR lpcszName);
static int  WComboIndex       (LPCSTR lpcszName);
static void WComboSetText     (LPCSTR lpcszName, LPCSTR lpcszText);
static void WComboSelText     (LPCSTR lpcszName, CString& strBuff);
static int  WComboSelLen      (LPCSTR lpcszName);
static void WComboItemText    (LPCSTR lpcszName, int iIndex, CString& strBuff);
static int  WComboItemLen     (LPCSTR lpcszName, int iIndex);
static int  WComboItemExists  (LPCSTR lpcszName, LPCSTR lpcszItem);
static void WComboItemClk     (LPCSTR lpcszName, int iIndex);
static void WComboItemClk     (LPCSTR lpcszName, LPCSTR lpcszItem);
static void WComboItemDblClk  (LPCSTR lpcszName, int iIndex);
static void WComboItemDblClk  (LPCSTR lpcszName, LPCSTR lpcszItem);
//static void WComboClear       (LPCSTR lpcszName);
//static void WComboAddItem     (LPCSTR lpcszName, LPCSTR lpcszText);
//static void WComboDelItem     (LPCSTR lpcszName, int iIndex);
//static void WComboDelItem     (LPCSTR lpcszName, LPCSTR lpcszItem);
static BOOL WComboEnabled     (LPCSTR lpcszName);
static void WComboSetFocus    (LPCSTR lpcszName);


//---------------------------------------------------------------------------
// Editbox routines
//---------------------------------------------------------------------------
//static void WEditSetClass (LPCSTR lpcszClassName);
static BOOL WEditExists   (LPCSTR lpcszName);
static long WEditLen      (LPCSTR lpcszName);
static void WEditText     (LPCSTR lpcszName, CString& strBuff);
static void WEditSetText  (LPCSTR lpcszName, LPCSTR lpcszText);
static void WEditSelText  (LPCSTR lpcszName, CString& strBuff);
static long WEditSelLen   (LPCSTR lpcszName);
//static void WEditLineText (LPCSTR lpcszName, long lIndex, CString& strBuff);
//static long WEditLineLen  (LPCSTR lpcszName, long lIndex);
//static long WEditPos      (LPCSTR lpcszName);
//static long WEditLine     (LPCSTR lpcszName);
//static long WEditChar     (LPCSTR lpcszName);
//static long WEditFirst    (LPCSTR lpcszName);
//static long WEditLines    (LPCSTR lpcszName);
static void WEditClick    (LPCSTR lpcszName);
static BOOL WEditEnabled  (LPCSTR lpcszName);
static void WEditSetFocus (LPCSTR lpcszName) ;
//---------------------------------------------------------------------------
// Static control routines
//---------------------------------------------------------------------------
static BOOL WStaticExists   (LPCSTR lpcszName);
//MSTEST40.DLL does not export WStaticLen nor WStaticText nor WStaticSetClass
//static long WStaticLen		(LPCSTR lpcszName);
static void WStaticText     (LPCSTR lpcszName, CString& strBuff);
//static void WStaticSetClass (LPCSTR);

//---------------------------------------------------------------------------
// Error routines
//---------------------------------------------------------------------------
//static int  WError     (void);
//static void WErrorSet  (int errValue);
//static void WErrorText (CString& strBuff);
//static int  WErrorLen  (void);
//static void WErrorTrap (int iTrapID, int iAction, FARPROC lpfnCallBack);

/////////////////////////////////////////////////////////////////////////////
// TESTEVNT Wrappers
//

static int QueKeys (LPCSTR);
static int QueKeyDn (LPCSTR);
static int QueKeyUp (LPCSTR);
static int DoKeys (LPCSTR sz, BOOL bLiteral = FALSE, int nWaitForIdleTime = 5000);
static int DoKeyshWnd (HWND, LPCSTR, BOOL bLiteral = FALSE, int nWaitForIdleTime  = 5000);

static void QuePause (DWORD dw);
static int QueSetSpeed (WORD w);
static int QueSetFocus (HWND hwnd);
static int QueSetRelativeWindow (HWND hwnd);

static int QueMouseMove (POINT pt);
static int QueMouseDn (int, POINT pt);
static int QueMouseUp (int, POINT pt);
static int QueMouseClick (int, POINT pt);
static int QueMouseDblClk (int, POINT pt);
static int QueMouseDblDn (int, POINT pt);

//static int TimeDelay (int);

static int QueFlush (BOOL bRestoreKeyState = TRUE);
//static void QueEmpty (void);

/////////////////////////////////////////////////////////////////////////////
// TESTEVNT Wrappers
//

//static int CompFiles (CHAR FAR *lpszFileName1, INT Scr1, CHAR FAR *lpszFileName2, INT Scr2, INT CompareType);
//static int CompScreenActivate(CHAR FAR *lpszFileName, CHAR FAR *OpenKeys, CHAR FAR *CloseKeys, wRect *lpRect, INT Scr1, INT Hide, //static int lag);
//static int CompScreen(CHAR FAR *lpszFileName, wRect *lpRect, INT Scr1, INT Hide, //static int lag);
//static int CompWindowActivate(CHAR FAR *lpszFileName, CHAR FAR *OpenKeys, CHAR FAR *CloseKeys, INT Scr1, INT Hide, //static int lag);
//static int CompWindow(CHAR FAR *lpszFileName, INT hWnd, INT Scr1, INT Hide, //static int lag);
//static int DelScreen (CHAR FAR *lpszFileName, INT Scr);
//static int DumpFileToClip(CHAR FAR *lpszFileName, INT Scr);
//static int DumpScreenActivate(CHAR FAR *lpszFileName, CHAR FAR *OpenKeys, CHAR FAR *CloseKeys, wRect *lpRect, INT Action, INT Scr1, //static int lag);
//static int DumpScreen(CHAR FAR *lpszFileName, wRect *lpRect, INT Action, INT Scr1, //static int lag);
//static int DumpSrnToClipActivate (CHAR FAR *OpenKeys, CHAR FAR *CloseKeys, wRect *lpRect, INT Hide);
//static int DumpSrnToClip (wRect *lpRect, INT Hide);
//static int DumpWindowActivate(CHAR FAR *lpszFileName, CHAR FAR *OpenKeys, CHAR FAR *CloseKeys, INT Action, INT Scr1, //static int lag);
//static int DumpWindow(CHAR FAR *lpszFileName, INT wHnd, INT Action, INT Scr1, //static int lag);
//static int DumpWndToClipActivate (CHAR FAR *OpenKeys, CHAR FAR *CloseKeys, INT Hide);
//static int DumpWndToClip (INT hWnd, INT Hide);
//static int FileInfo(CHAR FAR *lpszFileName, wRect *lpRect, INT *VideoMode, INT *Count);
//static int GetDLLVersion (CHAR FAR *lpszFileName);
//static int GetMaxScreen(CHAR FAR *lpszFileName);
//static int GetOS (CHAR FAR *lpszFileName);
//static int SaveFileToDIB (CHAR FAR *lpszFileName1, INT Scr, CHAR FAR *lpszFileName2);
//static int SaveSrnToDIBActivate(CHAR FAR *lpszFileName, CHAR FAR *OpenKeys, CHAR FAR *CloseKeys, wRect *lpRect, INT Hide);
//static int SaveSrnToDIB(CHAR FAR *lpszFileName, wRect *lpRect, INT Hide);
//static int SaveWndToDIBActivate(CHAR FAR *lpszFileName, CHAR FAR *OpenKeys, CHAR FAR *CloseKeys, INT Hide);
//static int SaveWndToDIB(CHAR FAR *lpszFileName, INT hWnd, INT Hide);
//static int ViewScreen(CHAR FAR *lpszFileName, INT hWnd, INT Scr1, INT Action);

};

// REVIEW(briancr): this object is declared as a global for backward compatibility
// REVIEW(briancr): this is just to make things work. Is there a better way to do this?
extern GUI_DATA CMSTest MST;

#endif //__TESTWRAP_H__
