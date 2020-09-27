///////////////////////////////////////////////////////////////////////////////
//  uiextwiz.H
//
//  Created by :            Date :
//      Ivanl             10/14/94
//
//  Description :
//      Declaration of the UIExtWizard class
//

#ifndef __UXTWZDLG_H__
#define __UXTWZDLG_H__

#include "uprojwiz.h"

#include "prjxprt.h"

#ifndef __UPROJWIZ_H__
	#error include 'uprojwiz.h' before including this file
#endif
#include "..\sym\awx.h"

///////////////////////////////////////////////////////////////////////////////
//  UIExtWizard class

///////////////////////////////////////////////////////////////////////////////
enum EXTWIZ_TYPES { EXTWIZ_CUSTOM, EXTWIZ_APWZ_CLONE, EXTWIZ_PROJ_BASED } ;
					
// BEGIN_CLASS_HELP
// ClassName: UIExtWizard
// BaseClass: UIProjectWizard
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UIExtWizard : public UIProjectWizard 
	{
    UIWND_COPY_CTOR (UIExtWizard, UIProjectWizard) ; 

// Private data
    private:
// General Utilities
	public:
//		inline virtual CString ExpectedTitle(void) const
//		{	return ExpectedTitle(-1); }
	//	virtual CString ExpectedTitle(int nPage) const;
		virtual HWND Create(void);
		BOOL SetExtType(int Type) ;
	   	BOOL SetNumPages(int Pages) ;
		BOOL SetBasePrjName(CString PrjName) ;
		BOOL SetDllExeType(int Type) ;
		BOOL SetLangs(int index) ;
		BOOL SetLangs(CString LangStr) ;

   } ;

#endif //__UXTWZDLG_H__          
