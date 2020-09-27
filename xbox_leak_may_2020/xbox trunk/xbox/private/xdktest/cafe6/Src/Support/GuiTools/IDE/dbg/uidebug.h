///////////////////////////////////////////////////////////////////////////////
//  UIDEBUG.H
//
//  Created by :            Date :
//      BrianCr             1/17/95
//  Description :
//      Declaration of the UIDebug class
//

#ifndef __UIDEBUG_H__
#define __UIDEBUG_H__

#include "dbgxprt.h"

///////////////////////////////////////////////////////////////////////////////
//  UIDebug class

// BEGIN_CLASS_HELP
// ClassName: UIDebug
// BaseClass: None
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIDebug
{
public:
	// this function is static to make porting easier: UIWB.ShowDockWindow becomes UIDebug::ShowDockWindow
	static HWND UIDebug::ShowDockWindow(UINT id, BOOL bShow = TRUE);
};

#endif //__UIDEBUG_H__
