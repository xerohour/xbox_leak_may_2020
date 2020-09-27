///////////////////////////////////////////////////////////////////////////////
//	USYMDLG.H
//
//	Created by :			Date :
//		DavidGa					9/20/93
//
//	Description :
//		Declaration of the UISymbolBrowser class
//

#ifndef __UISYMDLG_H__
#define __UISYMDLG_H__

#include "..\..\udialog.h"
#include "..\shl\wbutil.h"

#include "export.h"

#ifndef __UIDIALOG_H__
	#error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//	UISymbolBrowser class

// BEGIN_CLASS_HELP
// ClassName: UISymbolBrowser
// BaseClass: UIDialog
// Category: Editors
// END_CLASS_HELP
class RES_CLASS UISymbolBrowser : public UIDialog
{
	UIWND_COPY_CTOR(UISymbolBrowser, UIDialog);

// Utilities
public:
	virtual CString ExpectedTitle(void) const
		{	return GetLocString(IDSS_SB_TITLE); }
	int CountSymbols(BOOL bReadOnly);
};

#endif //__UISYMDLG_H__
