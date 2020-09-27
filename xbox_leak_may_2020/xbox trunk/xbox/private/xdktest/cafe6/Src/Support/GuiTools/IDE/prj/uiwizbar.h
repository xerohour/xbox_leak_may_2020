///////////////////////////////////////////////////////////////////////////////
//  UIWIZBAR.H
//
//  Created by :            Date :
//      Anita George        9/4/96		copied and modified from UIWIZBAR.h
//
//  Description :
//      Declaration of the UIWizardBar class
//

#ifndef __UIWIZBAR_H__
#define __UIWIZBAR_H__
//#include "odbcdlgs.h"
#include "..\sym\clswiz.h"
//#include "..\sym\vcpp32.h"
#include "..\shl\udockwnd.h"
#include "..\src\cosource.h"

//#include "prjxprt.h"

//#ifndef __UITABDLG_H__
//	#error include 'uitabdlg.h' before including this file
//#endif

#define COMMAND_STR		"WBClassComboActive"

///////////////////////////////////////////////////////////////////////////////
//  UIWizardBar class
///////////////////////////////////////////////////////////////////////////////


// BEGIN_CLASS_HELP
// ClassName: UIWizardBar
// BaseClass: UIDockWindow
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UIWizardBar : public UIDockWindow
	{
    UIWND_COPY_CTOR (UIWizardBar, UIDockWindow) ; 

	public:
		BOOL InitializeWizBar();
		BOOL AddNewClass();
		void SelectClass(CString strClassName);
		BOOL GotoClassDefinition(LPCSTR pszClassName, COSource* pSrcFile/* = NULL*/, LPCSTR pszClassDef);
		BOOL GotoDefinitionFromHere(LPCSTR pszName, LPCSTR pszClassDef);
} ;

#endif //__UIWIZBAR_H__          
