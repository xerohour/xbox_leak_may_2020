///////////////////////////////////////////////////////////////////////////////
//	ParamHlpSuite.h
//
//	Description :
//		Declaration of the CParamHlpSuite class
//

#ifndef __ParamHlpSuite_H__
#define __ParamHlpSuite_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define WM_GETLISTCOUNT     (WM_USER+401) // Return number of items in completion set
#define WM_GETLISTITEMLEN   (WM_USER+402) // Return length of item (0-based index in wParam)
#define WM_GETLISTITEMTEXT  (WM_USER+403) // Copy text of item (index in wParam) to lParam
#define WM_GETLISTMATCH     (WM_USER+404) // Return index of matched (not necessarily selected) item
#define WM_GETLISTSELECTION (WM_USER+405) // Return index of selected item (-1 if nothing selected)
#define WM_GETDESCWINDOW    (WM_USER+406) // Return the window handle of the description window (an edit control)
#define WM_SETTITLETOITEM   (WM_USER+407) // Set the window title of the namelist to the current selection

#define WM_GETMETHODTEXTLEN (WM_USER+502) // Return length of text of current method (methodtexttype in wParam)
#define WM_GETPARMTEXTLEN   (WM_USER+505) // Return length of parameter text (LOWORD(wParam) = parm#,

#define WM_SETTITLETOMETHOD	(WM_USER+509) // Set Window Title to Method name
#define WM_SETTITLETOPARAM	(WM_USER+510) // Set Window Title to Param Text

#define WM_SETTITLETOTEXT   (WM_USER+601) // Set the window title to the text in the window
// #define WM_GETTEXTLEN	(WM_USER+409) // Get the length of the text in the window

///////////////////////////////////////////////////////////////////////////////
// CParamHlpSuite class

class CParamHlpSuite : public CIDESubSuite
{
	DECLARE_SUBSUITE(CParamHlpSuite)

	DECLARE_TESTLIST();

// overrides
public:
	virtual void SetUp(BOOL bCleanUp);
	virtual void CleanUp(void);

// data members
public:
	COProject prj;
	COSource  src;
};

#endif //__ParamHlpSuite_H__
