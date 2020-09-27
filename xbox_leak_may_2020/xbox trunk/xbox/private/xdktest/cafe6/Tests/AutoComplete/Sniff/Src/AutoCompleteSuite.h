///////////////////////////////////////////////////////////////////////////////
//	AutoCompleteSuite.H
//
//	Description :
//		Declaration of the CAutoCompleteSuite class
//

#ifndef __AutoCompleteSuite_H__
#define __AutoCompleteSuite_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

// Messages for QA hooks
// See M:\dev\ide\pkgs\edit\tip.cpp

#define WM_GETLISTCOUNT         (WM_USER+401) // Return number of items in completion set
#define WM_GETLISTITEMLEN       (WM_USER+402) // Return length of item (0-based index in wParam)
#define WM_GETLISTITEMTEXT      (WM_USER+403) // Copy text of item (index in wParam) to lParam
#define WM_GETLISTMATCH         (WM_USER+404) // Return index of matched (not necessarily selected) item
#define WM_GETLISTSELECTION     (WM_USER+405) // Return index of selected item (-1 if nothing selected)
#define WM_GETDESCWINDOW        (WM_USER+406) // Return the window handle of the description window (an edit control)
#define WM_SETTITLETOITEM       (WM_USER+407) // Set the window title of the namelist to the current selection

#define WM_GETOVERLOADCOUNT     (WM_USER+501) // Return number of overloaded methods
#define WM_GETMETHODTEXTLEN     (WM_USER+502) // Return length of text of current method (methodtexttype in wParam)
#define WM_GETMETHODTEXT        (WM_USER+503) // Copy text of current method (methodtexttype in wParam) to lParam
#define WM_GETPARMCOUNT         (WM_USER+504) // Return number of parameters in current method
#define WM_GETPARMTEXTLEN       (WM_USER+505) // Return length of parameter text (LOWORD(wParam) = parm#,
                                              //  HIWORD(wParam) = parametertexttype)
#define WM_GETPARMTEXT          (WM_USER+506) // Copy parameter text to lParam (LOWORD(wParam) = parm#,
                                              //  HIWORD(wParam) = parametertexttype)
#define WM_GETCURRENTPARM       (WM_USER+507) // Return index of current (bolded) parameter
#define WM_ISMETHODTIP          (WM_USER+508) // Return non-zero if this is a method tip window
#define WM_SETTITLETOMETHOD     (WM_USER+509) // Set Window Title to Method name
#define WM_SETTITLETOPARAM		(WM_USER+510) // Set Window Title to Param Text

#define WM_SETTITLETOTEXT       (WM_USER+601) // Set the window title to the text in the window


// Titles of Autocomplete windows
#define TITLE_AUTOCOMPLETE		"VsCompletorPane"
#define TITLE_CODE_COMMENTS		"VcTipWindow"
#define TITLE_PARAMETER_HELP	"VsTipWindow"
#define TITLE_TYPE_INFO			"VcTipWindow"

///////////////////////////////////////////////////////////////////////////////
// CAutoCompleteSuite class

class CAutoCompleteSuite : public CIDESubSuite
{
	DECLARE_SUBSUITE(CAutoCompleteSuite)

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

#endif //__AutoCompleteSuite_H__
