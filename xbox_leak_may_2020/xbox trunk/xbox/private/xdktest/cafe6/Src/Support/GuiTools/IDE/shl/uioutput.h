///////////////////////////////////////////////////////////////////////////////
//	UOUTPUT.H
//
//	Created by :			Date :
//		DavidGa					12/9/93
//
//	Description :
//		Declaration of the various UIDockWindow derived classes
//

#ifndef __UIOUTPUT_H__
#define __UIOUTPUT_H__

#include "udockwnd.h"
#include "..\sym\qcqp.h"

#include "shlxprt.h"

#ifndef __UIDOCKWND_H__
	#error include 'udockwnd.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//	UISymbolBrowser class

// BEGIN_CLASS_HELP
// ClassName: UIOutput
// BaseClass: UIDockWindow
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIOutput : public UIDockWindow
{
	UIWND_COPY_CTOR(UIOutput, UIDockWindow);

	virtual BOOL Activate(void);
	virtual UINT GetID(void) const
		{	return IDW_OUTPUT_WIN; }
};

#endif //__UIOUTPUT_H__
