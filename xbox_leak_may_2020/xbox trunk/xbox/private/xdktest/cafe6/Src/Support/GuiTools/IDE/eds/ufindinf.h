///////////////////////////////////////////////////////////////////////////////
//	UFNDINF.H
//
//	Created by :			Date :
//		DavidGa					12/9/93
//
//	Description :
//		Declaration of the UFindInFilesDlg class
//

#ifndef __UIFNDINF_H__
#define __UIFNDINF_H__

#include "..\..\udialog.h"
#include "..\shl\wbutil.h"

#include "edsxprt.h"

#ifndef __UIDIALOG_H__
	#error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//	UISymbolBrowser class

// BEGIN_CLASS_HELP
// ClassName: UIFindInFilesDlg
// BaseClass: UIDialog
// Category: Editors
// END_CLASS_HELP
class EDS_CLASS UIFindInFilesDlg : public UIDialog
{
	UIWND_COPY_CTOR(UIFindInFilesDlg, UIDialog);

// Utilities
public:
	virtual CString ExpectedTitle(void) const
		{	return GetLocString(IDSS_FIF_TITLE); }
	void AddFile(LPCSTR szFile);
	void FindWhat(LPCSTR szFind);
	void DoFind(void);
};

#endif //__UIFNDINF_H__
