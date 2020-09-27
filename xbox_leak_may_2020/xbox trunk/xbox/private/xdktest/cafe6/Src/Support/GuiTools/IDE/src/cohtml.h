///////////////////////////////////////////////////////////////////////////////
//	COHTML.H
//
//	Created by :			Date :
//		MichMa					5/28/97
//
//	Description :
//		Declaration of the COHTML component object class
//

#ifndef __COHTML_H__
#define __COHTML_H__

#include "cosource.h"
#include "uinsctrl.h"

#include "export.h"

///////////////////////////////////////////////////////////////////////////////
// COHTML class

// BEGIN_CLASS_HELP
// ClassName: COHTML
// BaseClass: COSource
// Category: Editors
// END_CLASS_HELP
class SRC_CLASS COHTML : public COSource
{
public:
	COHTML();
// Utilities
	int Create(LPCSTR szSaveAs = NULL, LPCSTR szProject = NULL);
	int InsertActiveXControl(LPCSTR szCtrl);
// Data
private:
	UIInsertCtrlDlg m_uiInsCtrlDlg;
};

#endif //__COHTML_H__
