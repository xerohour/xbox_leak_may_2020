///////////////////////////////////////////////////////////////////////////////
//	UDOCKWND.H
//
//	Created by :			Date :
//		DavidGa					12/9/93
//
//	Description :
//		Declaration of the UIDockWindow class
//

#ifndef __UIDOCKWND_H__
#define __UIDOCKWND_H__

#include "..\..\uwindow.h"
#include "shlxprt.h"

#ifndef __UIWINDOW_H__
	#error include 'uwindow.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//	UIDockWindow

// BEGIN_CLASS_HELP
// ClassName: UIDockWindow
// BaseClass: UIWindow
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIDockWindow : public UIWindow
{
	UIWND_COPY_CTOR(UIDockWindow, UIWindow);

	virtual BOOL AttachActive(void);
	virtual BOOL Find(void);

protected:
	virtual BOOL ActivatePane(LPCSTR);		// Don't want direct calls of this from outside.

// UIWindow overrides
public:
	virtual BOOL IsValid(void) const;
	virtual BOOL IsActive(void);

// Utilities
public:
	enum DockLocation 
	{
		DontCare,
		Top,
		Bottom,
		Left,
		Right
	};

	virtual BOOL IsDockable(void);
	virtual BOOL Close(void);
	virtual UINT GetID(void) const;
	virtual BOOL IsDocked(DockLocation loc = UIDockWindow::DontCare);
};

#endif //__UIDOCKWND_H__
