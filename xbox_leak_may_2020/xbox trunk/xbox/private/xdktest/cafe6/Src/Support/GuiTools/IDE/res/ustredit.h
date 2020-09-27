///////////////////////////////////////////////////////////////////////////////
//	USTREDIT.H
//
//	Created by :			Date :
//		DavidGa					3/25/94
//
//	Description :
//		Declaration of the UIStrEdit class
//

#ifndef __UISTREDIT_H__
#define __UISTREDIT_H__

#include "urestabl.h"

#include "export.h"

#ifndef __UIRESTABL_H__
	#error include 'urestabl.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// UIStrEdit class

// BEGIN_CLASS_HELP
// ClassName: UIStrEdit
// BaseClass: UIResTableEditor
// Category: Editors
// END_CLASS_HELP
class RES_CLASS UIStrEdit : public UIResTableEditor
{
public:
	UIWND_COPY_CTOR(UIStrEdit, UIResTableEditor);

// Attributes
public:
	virtual BOOL IsValid(void) const;

// Utilities
public:	
	BOOL AddString(LPCSTR szCaption, LPCSTR id = NULL, BOOL bIdFirst = FALSE);
	void EditString(LPCSTR szCaption = NULL, LPCSTR szId = NULL);
	BOOL FindString(LPCSTR szCaption = NULL, LPCSTR szId = 0);
	CString GetString(int line = -1);
};

#endif //__UISTREDIT_H__
