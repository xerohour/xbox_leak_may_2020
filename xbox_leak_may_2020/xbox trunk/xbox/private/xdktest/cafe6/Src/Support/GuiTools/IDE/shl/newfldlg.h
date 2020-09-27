///////////////////////////////////////////////////////////////////////////////
//  NEWFLDLG.H
//
//  Created by :            Date :
//      Ivanl              7/1/96
//
//  Description :
//      Declaration of the UINewFileDlg class
//

#include "uitabdlg.h"
#include "wbutil.h"

#ifndef __UITABDLG_H__
	#error include uitabdlg.h' before including this file
#endif

#ifndef __UINEWFILE__
#define __UINEWFILE__

#include "..\sym\vshell.h"
///////////////////////////////////////////////////////////////////////////////
//  UINewFileDlg class

typedef	enum {FILES =	  VSHELL_IDC_NAMEFORFILE,
			  OTHER =	  VSHELL_IDC_NAMEFOROTHERFILE,
			  PROJECTS =  VSHELL_IDC_NAMEFORPROJECT, 
			  TEMPLATES = VSHELL_IDC_NAMEFORTEMPLATE,
			  WORKSPACE = VSHELL_IDC_NAMEFORWORKSPACE } NewThingType ;

 
// BEGIN_CLASS_HELP
// ClassName: UINewFileDlg
// BaseClass: UITabbedDialog
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UINewFileDlg : public UITabbedDialog 
	{
	UIWND_COPY_CTOR (UINewFileDlg, UITabbedDialog) ; 

// General Utilities
	public:
		HWND Display(void) ;
			
			
 
	public:
		int SetName( CString Name, int DocType = PROJECTS );
		int SetLocation( CString  szLocation, int DocType = PROJECTS);
		int SelectFromList(CString ItemName) ;
		int SelectFromList(int Index) ;
		int NewFileTab();
		int NewProjectTab();
		int NewTemplateTab();
		int NewWorkspaceTab();
		int NewOtherTab();
		int AddToProject(BOOL bAdd);
		int SetProject(LPCSTR szProject);
		void SetProjPlatform(CString Platform);
		virtual HWND OK() ;
  } ;

#endif //__UINEWFILE__
