///////////////////////////////////////////////////////////////////////////////
//  UISTACK.H
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Declaration of the UIStack class
//

#ifndef __UISTACK_H__
#define __UISTACK_H__

#include "..\sym\qcqp.h"
#include "..\shl\udockwnd.h"

#include "dbgxprt.h"

#ifndef __UIDOCKWND_H__
	#error include 'udockwnd.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UIStack class : public UIDockWindow

// BEGIN_CLASS_HELP
// ClassName: UIStack
// BaseClass: UIDockWindow
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIStack : public UIDockWindow
	{
	UIWND_COPY_CTOR(UIStack, UIDockWindow);

	virtual BOOL Activate(void);
	virtual UINT GetID(void) const
		{	return IDW_CALLS_WIN; }

	virtual BOOL AttachActive(void);
	virtual BOOL Find(void);

	// Initialization
	public:

	// Data
	private:
		int m_option;

	// UIWindow overrides
	public:
		virtual BOOL IsValid(void) const;
		virtual BOOL IsActive(void);	// derived classes must provide their own Activate (usually a DoCommand)


	// Utilities
	public:
		CString GetFunction(int level = 0);
		CString GetAllFunctions(void);
		int  NavigateStack(int level = 0);
		BOOL GoToFrame(int level);
	};

#endif
