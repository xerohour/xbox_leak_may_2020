///////////////////////////////////////////////////////////////////////////////
//	URESEDIT.H
//
//	Created by :			Date :
//		DavidGa					9/23/93
//
//	Description :
//		Declaration of the UIEditor class
//

#ifndef __UIRESEDIT_H__
#define __UIRESEDIT_H__

#include "..\eds\ueditor.h"

#include "export.h"

#ifndef __UIEDITOR_H__
	#error include 'ueditor.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//	UIResEditor class

// BEGIN_CLASS_HELP
// ClassName: UIResEditor
// BaseClass: UIEditor
// Category: Editors
// END_CLASS_HELP
class RES_CLASS UIResEditor : public UIEditor
{
	UIWND_COPY_CTOR(UIResEditor, UIEditor);

// Data
protected:
	static LPCSTR* m_pszTypeText;

// Attributes
public:
	virtual BOOL IsValid(void) const;
	virtual BOOL IsValidType(LPCSTR szType) const;

// Utilities
public:
	BOOL ExportResource(LPCSTR szFileName);
};

enum
{
	ED_BROWSER = -1, 
	ED_DIALOG = 0, 
	ED_MENU, 
	ED_CURSOR, 
	ED_ICON, 
	ED_BITMAP, 
	ED_STRING, 
	ED_ACCEL, 
	ED_VERSION, 
	ED_BINARY
};

#endif //__UIRESEDIT_H__
