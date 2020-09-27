//////////////////////////////
//  UTBARDLG.H
//
//  Created by :            Date :
//      EnriqueP              1/13/94
//
//  Description :
//      Declaration of the UIToolbar class
//

#ifndef __UTBARDLG_H__
#define __UTBARDLG_H__

#include "..\..\udialog.h"
#include "wbutil.h"

#include "shlxprt.h"

#ifndef __UIDIALOG_H__
   #error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
// UITBarDialog Control ID's   as defined in vshell.h
/*
IDSS_TB_TITLE 		...... 	Dialog title:	"Toolbars" 	  defined in wb.rc
VSHELL_IDC_NEW     .......	New Button				
VSHELL_IDC_CUSTOM		....Customize Button					
VSHELL_IDC_DELETE		..  Reset and Delete Buttons				
VSHELL_IDC_TOOLBAR	....Toolbars Listbox				
VSHELL_IDC_NAME		....Toolbar Name Edit box


*/

enum {  UITB_NO_ERROR,				  //  Error return codes
		UITB_ERR_NOT_DISPLAYED,
		UITB_ERR_FAILED
	 };

///////////////////////////////////////////////////////////////////////////////
// UITBarDialog class declaration

// BEGIN_CLASS_HELP
// ClassName: UITBarDialog
// BaseClass: UIDialog
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UITBarDialog : public UIDialog
{
   	UIWND_COPY_CTOR (UITBarDialog, UIDialog) ;

	virtual CString ExpectedTitle (void) const
		{	return GetLocString(IDSS_TB_TITLE); }

// Utilities
public:
	int Display(void);
		
};			



#endif //__UTBARDLG_H__
