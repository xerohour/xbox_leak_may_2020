//////////////////////////////
//  UIUPDEP.H
//
//  Created by :            Date :
//      BrianCr				08/04/95
//
//  Description :
//      Declaration of the UIUpdateDepDialog class
//

#ifndef __UIDEPDLG_H__
#define __UIDEPDLG_H__

#include "..\..\udialog.h"
#include "..\shl\wbutil.h"

#include "prjxprt.h"

#ifndef __UIDIALOG_H__
   #error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
// UIUpdateDepDialog class declaration

// BEGIN_CLASS_HELP
// ClassName: UIUpdateDepDialog
// BaseClass: UIDialog
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UIUpdateDepDialog: public UIDialog
{
   	UIWND_COPY_CTOR (UIUpdateDepDialog, UIDialog) ;

	virtual CString ExpectedTitle (void) const
		{	return GetLocString(IDSS_UPDATEDEP_TITLE); }

// Utilities
public:
	BOOL Display(void);
};			

#endif //__UIDEPDLG_H__
