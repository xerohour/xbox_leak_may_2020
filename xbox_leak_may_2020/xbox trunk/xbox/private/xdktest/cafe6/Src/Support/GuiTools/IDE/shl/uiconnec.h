///////////////////////////////////////////////////////////////////////////////
//  UICONNEC.H
//
//  Created by :            Date :
//      MichMa                  8/24/94
//
//  Description :
//      Declaration of the UIConnection class
//

#ifndef __UICONNEC_H__
#define __UICONNEC_H__

#include "..\..\udialog.h"
#include "Strings.h"
#include "wbutil.h"

#include "shlxprt.h"

#define ERROR_ERROR -1

///////////////////////////////////////////////////////////////////////////////
//  UIConnection class

// BEGIN_CLASS_HELP
// ClassName: UIConnection
// BaseClass: UIDialog
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIConnection : public UIDialog

	{
	UIWND_COPY_CTOR(UIConnection, UIDialog);
	CString ExpectedTitle(void) const {return GetLocString(IDSS_CONNEC_DLG_TITLE);}
	
	// Utilities
	public:
		operator HWND() {return HWnd();} ;

		HWND Settings(CString title);
		
		int SetPlatform(LPCSTR platform);
		int SetConnection(LPCSTR connection);
		
		CString GetPlatform(void);
		CString GetConnection(void);
	};

#endif // __UICONNEC_H__
