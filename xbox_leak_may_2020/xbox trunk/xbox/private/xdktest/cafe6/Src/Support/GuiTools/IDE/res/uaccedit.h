///////////////////////////////////////////////////////////////////////////////
//	UACCEDIT.H
//
//	Created by :			Date :
//		DavidGa					3/25/94
//
//	Description :
//		Declaration of the UIAccEdit class
//

#ifndef __UIACCEDIT_H__
#define __UIACCEDIT_H__

#include "urestabl.h"

#include "export.h"

#ifndef __UIRESTABL_H__
	#error include 'urestabl.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// UIAccEdit class

// BEGIN_CLASS_HELP
// ClassName: UIAccEdit
// BaseClass: UIResTableEditor
// Category: Editors
// END_CLASS_HELP
class RES_CLASS UIAccEdit : public UIResTableEditor
{
public:
	UIWND_COPY_CTOR(UIAccEdit, UIResTableEditor);

// Attributes
public:
	virtual BOOL IsValid(void) const;

// Utilities
public:	
	int AddAccel(LPCSTR szChar, LPCSTR  szId = NULL, DWORD dwKeyMods = 0x0, BOOL bNextKeyType = FALSE);
	void EditAccel(LPCSTR szChar = NULL, LPCSTR szId = NULL, DWORD dwKeyMods = 0x0);
	BOOL FindAccel(LPCSTR szChar = NULL, LPCSTR szId = 0);
	CString GetKey(int line = -1);
	DWORD GetKeyMods(int line = -1);
};

#define ACC_NO_CONTROL	0x2
#define ACC_CONTROL		0x3
#define ACC_NO_ALT		0x8
#define ACC_ALT			0xB
#define ACC_NO_SHIFT	0x20
#define ACC_SHIFT		0x30
#define ACC_ASCII		0x80
#define ACC_VIRTKEY		0xB0

#endif //__UIACCEDIT_H__
