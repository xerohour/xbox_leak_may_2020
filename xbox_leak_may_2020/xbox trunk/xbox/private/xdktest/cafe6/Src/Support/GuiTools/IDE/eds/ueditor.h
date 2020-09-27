///////////////////////////////////////////////////////////////////////////////
//	UEDITOR.H
//
//	Created by :			Date :
//		DavidGa					9/23/93
//
//	Description :
//		Declaration of the UIEditor class
//

#ifndef __UIEDITOR_H__
#define __UIEDITOR_H__

#include "..\..\uwindow.h"

#include "edsxprt.h"

#ifndef __UIWINDOW_H__
	#error include 'uwindow.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//	UIEditor class

// BEGIN_CLASS_HELP
// ClassName: UIEditor
// BaseClass: UIWindow
// Category: Editors
// END_CLASS_HELP
class EDS_CLASS UIEditor : public UIWindow
{
	UIWND_COPY_CTOR(UIEditor, UIWindow);

// Utilities
public:
	operator HWND ( ) {return HWnd();} ;
	virtual BOOL AttachActive(void);
	virtual BOOL IsActive(void);
	virtual BOOL Activate(void);
	CString GetTitle(void) const;
	BOOL Close(BOOL bLoseChanges = TRUE);
};

#endif //__UIEDITOR_H__
