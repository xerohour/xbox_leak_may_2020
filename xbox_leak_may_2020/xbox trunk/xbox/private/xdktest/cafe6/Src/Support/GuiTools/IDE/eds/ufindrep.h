///////////////////////////////////////////////////////////////////////////////
//	UFINDREP.H
//
//	Created by :			Date :
//		DavidGa					4/13/94
//
//	Description :
//		Declaration of the UIFindDlg and UIReplaceDlg classes
//

#ifndef __UIFINDREP_H__
#define __UIFINDREP_H__

#include "..\..\udialog.h"

#include "edsxprt.h"

#ifndef __UIDIALOG_H__
	#error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//	UIFindDlg class

//enum { DIR_DOWN = FALSE, DIR_UP = TRUE };

// BEGIN_CLASS_HELP
// ClassName: UIFindDlg
// BaseClass: UIDialog
// Category: Editors
// END_CLASS_HELP
class EDS_CLASS UIFindDlg : public UIDialog
{
// data types
public:
	enum FindDirection { FD_DOWN, FD_UP };

// base class overrides
public:	
	UIWND_COPY_CTOR(UIFindDlg, UIDialog);

	virtual BOOL VerifyUnique(void) const;
	virtual BOOL Activate(void);
	virtual HWND Close(void);

// operations
public:
	BOOL SetFindString(LPCSTR szFind);
	BOOL SetMatchWord(BOOL bMatchWord = FALSE);
	BOOL SetMatchCase(BOOL bMatchCase = FALSE);
	BOOL SetRegularExpression(BOOL bRegExpr = FALSE);
	BOOL SetDirection(FindDirection fd = FD_DOWN);

	BOOL FindNext(void);
	BOOL SetBookmarks(void);
};

///////////////////////////////////////////////////////////////////////////////
//	UIReplaceDlg class

// BEGIN_CLASS_HELP
// ClassName: UIReplaceDlg
// BaseClass: UIDialog
// Category: Editors
// END_CLASS_HELP
class EDS_CLASS UIReplaceDlg : public UIDialog
{
// data types
public:
	enum ReplaceScope { RS_WHOLEFILE, RS_SELECTION };

// base class overrides
public:
	UIWND_COPY_CTOR(UIReplaceDlg, UIDialog);
	virtual BOOL VerifyUnique(void) const;
	virtual BOOL Activate(void);
	virtual HWND Close(void);

// operations
public:
	BOOL SetFindString(LPCSTR szFind);
	BOOL SetReplaceString(LPCSTR szReplace);
	BOOL SetMatchWord(BOOL bMatchWord = FALSE);
	BOOL SetMatchCase(BOOL bMatchCase = FALSE);
	BOOL SetRegularExpression(BOOL bRegExpr = FALSE);
	BOOL SetReplaceScope(ReplaceScope rs = RS_WHOLEFILE);

	BOOL FindNext(void);
	BOOL Replace(void);
	int ReplaceAll(void);
};

#endif //__UIFINDREP_H__
