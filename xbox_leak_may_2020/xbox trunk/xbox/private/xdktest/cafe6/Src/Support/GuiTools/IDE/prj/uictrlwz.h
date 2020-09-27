///////////////////////////////////////////////////////////////////////////////
//  UICTRLWZ.H
//
//  Created by :            Date :
//      Ivanl             10/14/94
//
//  Description :
//      Declaration of the UICtrlWizard class
//

#ifndef __UICTRLWZ_H__
#define __UICTRLWZ_H__

#include "..\sym\ctrlwz.h"
#include "uprojwiz.h"
#include "prjxprt.h"

#ifndef __UPROJWIZ_H__
	#error include 'uprojwiz.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UICtrlWizard class

///////////////////////////////////////////////////////////////////////////////
// enum EXTWIZ_TYPES { EXTWIZ_CUSTOM, EXTWIZ_APWZ_CLONE, EXTWIZ_PROJ_BASED } ;

// The summary info dialog of ControlWizard defined below
class PRJ_CLASS UICtrlSummaryDlg : public UIDialog 
	{
    UIWND_COPY_CTOR (UICtrlSummaryDlg, UIDialog) ; 

// Private data
    private:
			CString m_ActiveCtrl ;
// General Utilities
	public:
		inline CString ExpectedTitle(void) const
		{ return ("Summary Info"); }// REVIEW : LOCALIZATION 
		
		HWND Display(void) ;
		int SetShortName(CString Name) ;
		int SetCrlClsName(CString Name) ;
		int SetCtrlHName(CString Name) ;
		int SetCtrlCppName(CString Name) ;
		int SetCtrlUsrName(CString Name) ;
		int SetCtrlIDName(CString Name) ;
		int SetPpgClsName(CString Name) ;
		int SetPpgHName(CString Name) ;
		int SetPpgCppName(CString Name) ;
		int SetPpgUsrName(CString Name) ;
		int SetPpgIDName(CString Name) ;
		int Close(int how = 1) ;
   };
					
// BEGIN_CLASS_HELP
// ClassName: UICtrlWizard
// BaseClass: UIProjectWizard
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UICtrlWizard : public UIProjectWizard 
	{
    UIWND_COPY_CTOR (UICtrlWizard, UIProjectWizard) ; 

// Private data
    private:
	int m_ActiveCtrl ;
	CString m_strPageTitles[2];


// General Utilities
	public:
		UICtrlSummaryDlg SumDlg ;

		virtual CString ExpectedTitle(int page);
		virtual void OnUpdate(void);

		virtual HWND Create(void);
		BOOL SetCount(int count) ;
	   	BOOL SetLicenceOpt(int iLic) ;
		BOOL SetCommentOpt(int icomm) ;
		BOOL SetHelpOpt(int iHelp) ;
		BOOL SetActive(int index) ;
		BOOL SetActive(CString strAct) ;

		BOOL SummaryInfo() ;
		BOOL ActiveIfVisible(int opt) ;
		BOOL Invisible(int opt) ;
		BOOL InInsertDlg(int opt) ;
		BOOL SimpleFrame(int opt) ;
		BOOL SubClassCtrl(int index) ;
		BOOL SubClassCtrl(CString strcls) ;

		CString GetActive() ;
   } ;

#endif //__UICTRLWZ_H__          
