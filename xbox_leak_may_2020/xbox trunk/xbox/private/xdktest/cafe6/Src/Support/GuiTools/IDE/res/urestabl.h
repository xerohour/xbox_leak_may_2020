///////////////////////////////////////////////////////////////////////////////
//	URESTABL.H
//
//	Created by :			Date :
//		DavidGa					3/25/94
//
//	Description :
//		Declaration of the UIResTableEditor class
//

#ifndef __UIRESTABL_H__
#define __UIRESTABL_H__

#include "uresedit.h"

#include "export.h"

#ifndef __UIRESEDIT_H__
	#error include 'uresedit.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// UIResTableEditor class

// BEGIN_CLASS_HELP
// ClassName: UIResTableEditor
// BaseClass: UIResEditor
// Category: Editors
// END_CLASS_HELP
class RES_CLASS UIResTableEditor : public UIResEditor
{
public:
	UIWND_COPY_CTOR(UIResTableEditor, UIResEditor);
	virtual void OnUpdate(void);

// Data
protected:
	HWND m_hwndListbox;

// Attributes
public:
	virtual BOOL IsValid(void) const;
	inline HWND GetListbox(void)
		{	return m_hwndListbox; }

// Utilities
public:	
	BOOL SelectLine(int line, BOOL bMulti = FALSE);
	CString GetCaption(int line = -1);
	CString GetId(int line = -1);
	int GetCurLine(void);
	int GetLineCount(void);
};

#endif //__UIRESTABL_H__
