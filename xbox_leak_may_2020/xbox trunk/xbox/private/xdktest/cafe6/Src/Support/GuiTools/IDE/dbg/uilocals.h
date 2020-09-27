///////////////////////////////////////////////////////////////////////////////
//  UILOCALS.H
//
//  Created by :            Date :
//      MichMa              	1/17/94
//
//  Description :
//      Declaration of the UILocals class
//

#ifndef __UILOCALS_H__
#define __UILOCALS_H__

#include "..\sym\qcqp.h"
#include "..\shl\udockwnd.h"

#include "dbgxprt.h"

#ifndef __UIDOCKWND_H__
   #error include 'udockwnd.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UILocals class

// BEGIN_CLASS_HELP
// ClassName: UILocals
// BaseClass: UIDockWindow
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UILocals : public UIDockWindow
   {
	UIWND_COPY_CTOR(UILocals, UIDockWindow);

	virtual BOOL Activate(void);
	virtual UINT GetID(void) const
		{	return IDW_LOCALS_WIN; }
   };

#endif //__UILOCALS_H__
