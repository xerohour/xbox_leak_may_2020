///////////////////////////////////////////////////////////////////////////////
//	UTSTAPWZ.H
//
//	Created by :			Date :
//		Ivanl				12/29/94
//
//	Description :
//		Declaration of the UITestAppWizard class
//
#define IDC_ITERATION 1294

#ifndef __UTSTAPWZ_H__
#define __UTSTAPWZ_H__

#include "uappwiz.h"

#include "prjxprt.h"

#ifndef __UIAPPWIZ_H__ 
	#error include 'uappwiz.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//	UITestAppWizard class

// BEGIN_CLASS_HELP
// ClassName: UITestAppWizard
// BaseClass: UIAppWizard
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UITestAppWizard: public UIAppWizard
{
	UIWND_COPY_CTOR(UITestAppWizard, UIAppWizard);
	virtual void OnUpdate(void);
	
	// Data
	WORD pg0 , pg1, pg2, pg3,pg4, pg5, pg6, confirm;

protected:
// Utilities
public:

	virtual void Initialize() ;
	virtual HWND Create(void);
	void SetIteration(CString iteration) ;
	CString GetIteration() ;
	
};

#endif //__UTSTAPWZ__
