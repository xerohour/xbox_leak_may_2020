///////////////////////////////////////////////////////////////////////////////
//	UMENEDIT.H
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Description :
//		Declaration of the UIMenEdit class
//

#ifndef __UIMENEDIT_H__
#define __UIMENEDIT_H__

#include "uresedit.h"

#include "export.h"

#ifndef __UIRESEDIT_H__
	#error include 'uresedit.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// UIMenEdit class

// BEGIN_CLASS_HELP
// ClassName: UIMenEdit
// BaseClass: UIResEditor
// Category: Editors
// END_CLASS_HELP
class RES_CLASS UIMenEdit : public UIResEditor
{
	UIWND_COPY_CTOR(UIMenEdit, UIResEditor);

// Attributes
public:
	virtual BOOL IsValid(void) const;

// Utilities
public:
	HWND GetMenuBar(void);

};

// Menu Prop Page mnemonics

#define UIME_MN_SEPARATOR   "%s"
#define UIME_MN_CHECKED     "%k"
#define UIME_MN_POPUP       "%p"
#define UIME_MN_GRAYED      "%g"
#define UIME_MN_INACTIVE    "%n"
#define UIME_MN_HELP        "%l"
#define UIME_MN_BREAK       "%b"
#define UIME_MN_PROMPT      "%m"


// Strings found in SGetPropTitle

#define PT_MENUBAR    "Menu Properties"       //prop page title for the menu bar
#define PT_MENUITEM   "Menu Item Properties"  // for men

#endif //__UIMENEDIT_H__
