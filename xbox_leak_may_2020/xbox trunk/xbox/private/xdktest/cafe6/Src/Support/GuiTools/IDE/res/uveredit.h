///////////////////////////////////////////////////////////////////////////////
//	UVEREDIT.H
//
//	Created by :			Date :
//		EnriqueP				12/1/93
//
//	Description :
//		Declaration of the UIVerEdit class
//

#ifndef __UIVEREDIT_H__
#define __UIVEREDIT_H__

#include "uresedit.h"

#include "export.h"

#ifndef __UIRESEDIT_H__
	#error include 'uresedit.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// UIVerEdit class

// BEGIN_CLASS_HELP
// ClassName: UIVerEdit
// BaseClass: UIResEditor
// Category: Editors
// END_CLASS_HELP
class RES_CLASS UIVerEdit : public UIResEditor
{
public:
	UIWND_COPY_CTOR(UIVerEdit, UIResEditor);
	

// Data
protected:
	

// Attributes
public:
	virtual BOOL IsValid(void) const;

// Utilities
	

};

// Strings found in SGetPropTitle


#endif //__UIVEREDIT_H__
