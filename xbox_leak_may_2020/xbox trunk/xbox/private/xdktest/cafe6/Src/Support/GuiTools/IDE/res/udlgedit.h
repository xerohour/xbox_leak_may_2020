///////////////////////////////////////////////////////////////////////////////
//	UDLGEDIT.H
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Edited by :				Date :			Reason :
//		ScottSe					8/20/96			Updated control defines
//
//	Description :
//		Declaration of the UIDlgEdit class
//

#ifndef __UIDLGEDIT_H__
#define __UIDLGEDIT_H__

#include "uresedit.h"

#include "export.h"

#ifndef __UIRESEDIT_H__
	#error include 'uresedit.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// UIDlgEdit class

// BEGIN_CLASS_HELP
// ClassName: UIDlgEdit
// BaseClass: UIResEditor
// Category: Editors
// END_CLASS_HELP
class RES_CLASS UIDlgEdit : public UIResEditor
{
public:
	UIWND_COPY_CTOR(UIDlgEdit, UIResEditor);
	virtual void OnUpdate(void);

// Data
protected:
	HWND m_hwndDialog;

// Attributes
public:
	virtual BOOL IsValid(void) const;

// Utilities
public:	
	BOOL  DropControl(int nControl, int cX, int cY );
	inline HWND GetDialog(void)
		{	return m_hwndDialog; }

};

//  Control Palette control identifiers


#define CT_ARROW       0
#define CT_PICTURE			1
#define CT_TEXT				2
#define CT_EDIT				3
#define CT_GROUP			4
#define CT_BUTTON			5
#define CT_CHECK			6
#define CT_RADIO			7
#define CT_DROPDOWN			8
#define CT_LIST				9
#define CT_HSCROLL			10
#define CT_VSCROLL			11
#define CT_SPIN				12
#define CT_PROGRESS			13
#define CT_SLIDER			14
#define CT_HOTKEY			15
#define CT_LISTCTL			16
#define CT_TREE				17
#define CT_TAB				18
#define CT_ANIMATE			19
#define CT_RICHEDIT			20
#define CT_DATETIMEPICKER	21
#define CT_MONTHCALENDAR	22
#define CT_IPADDRESS		23
#define CT_USER				24		// use this line for VC98
//#define CT_USER				21		// use this line for VC97

/*
'*  Strings found in SGetPropTitle
'*      DLG_CONTROL_TITLE ( CT_FOO ) == "Foo"

GLOBAL DLG_CONTROL_TITLE (12) AS STRING
DLG_CONTROL_TITLE (0)   = "Dialog Properties"           ' for the dialog frame
DLG_CONTROL_TITLE (1)   = "Picture"
DLG_CONTROL_TITLE (2)   = "Text"
DLG_CONTROL_TITLE (3)   = "Edit"
DLG_CONTROL_TITLE (4)   = "Group Box"
DLG_CONTROL_TITLE (5)   = "Push Button"
DLG_CONTROL_TITLE (6)   = "Check Box"
DLG_CONTROL_TITLE (7)   = "Radio Button"
DLG_CONTROL_TITLE (8)   = "Combo Box"
DLG_CONTROL_TITLE (9)   = "List Box"
DLG_CONTROL_TITLE (10)  = "Scrollbar"
DLG_CONTROL_TITLE (11)  = "Scrollbar"
DLG_CONTROL_TITLE (12)  = "User Control"  */

#endif //__UIDLGEDIT_H__
