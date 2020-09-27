///////////////////////////////////////////////////////////////////////////////
//  UDIALOG.H
//
//  Created by :            Date :
//      DavidGa                 9/20/93
//
//  Description :
//      Declaration of the UIDialog class
//

#ifndef __UIDIALOG_H__
#define __UIDIALOG_H__

#include "uwindow.h"

#include "guixprt.h"

///////////////////////////////////////////////////////////////////////////////
// CUniqueID class

// BEGIN_CLASS_HELP
// ClassName: CUniqueDlgCtrl
// BaseClass: None
// Category: General
// END_CLASS_HELP
class GUI_CLASS CUniqueDlgCtrl
{
public:
	inline CUniqueDlgCtrl(void)
		{	m_id = 0; m_type = -1; }
	UINT m_id;
	int m_type;
	inline BOOL IsValid(void) const
		{	return m_id != 0; }
};

///////////////////////////////////////////////////////////////////////////////
// UIDialog class

// BEGIN_CLASS_HELP
// ClassName: UIDialog
// BaseClass: UIWindow
// Category: General
// END_CLASS_HELP
class GUI_CLASS UIDialog : public UIWindow
{
    UIWND_COPY_CTOR( UIDialog, UIWindow);
	UIDialog( LPCSTR szTitle );
	UIDialog( UINT idCtrl, int nType );

// Data
public:
	CString m_strTitle;
	CUniqueDlgCtrl m_UniqueCtrl;

// Utilities
public:
	virtual BOOL Open(void) const;
    virtual BOOL IsValid(void) const;
	virtual BOOL VerifyClass(void) const;
	virtual BOOL VerifyUnique(void) const;
    virtual CString ExpectedTitle(void) const;
    virtual BOOL VerifyTitle(void) const;
	void SetExpectedTitle( LPCSTR sz );
	virtual HWND GetDlgItem(UINT id) const;
	BOOL VerifyDlgItemClass(UINT id, int nClass) const;
	operator HWND ( ) {return  HWnd();} ;

// Standard buttons
    virtual HWND OK(void);
    virtual HWND Cancel(void);
    virtual HWND Close(void);
    virtual HWND Help(void);
};

#endif //__UIDIALOG_H__
