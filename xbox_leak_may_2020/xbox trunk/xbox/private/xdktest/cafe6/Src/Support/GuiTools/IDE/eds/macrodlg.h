///////////////////////////////////////////////////////////////////////////////
//  UMACRODLG.H
//
//  Created by :            Date :
//      EnriqueP              2/4/94
//
//  Description :
//      Declaration of the UIMacrosDlg class
//

#ifndef __UMACRODLG_H__
#define __UMACRODLG_H__
#include "..\..\udialog.h"
#include "..\sym\auto1.h"
#include "guiv1.h"
#include "edsxprt.h"

#ifndef __UIDIALOG_H__
	#error include 'udialog.h' before including this file
#endif
#define ERROR_ERROR -1

///////////////////////////////////////////////////////////////////////////////
//  UIMacrosDlg class


///////////////////////////////////////////////////////////////////////////////
 
// BEGIN_CLASS_HELP
// ClassName: UIMacrosDlg
// BaseClass: UIDialog
// Category: Editors
// END_CLASS_HELP
class EDS_CLASS UIMacrosDlg : public UIDialog 
	{
    UIWND_COPY_CTOR (UIMacrosDlg, UIDialog) ; 

// General Utilities
	public:
			
			HWND Display(void) ;
			
			
	public:
			int SelectMacroFile(CString FileName);
			int SelectMacro(CString MacroName); 
			int RunMacro(void);
			HWND Close(void);
   } ;

#endif //__UMACRODLG_H__
