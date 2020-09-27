///////////////////////////////////////////////////////////////////////////////
//  UIEXCPT.H
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Declaration of the UIExceptions class
//

#ifndef __UIEXCPT_H__
#define __UIEXCPT_H__

#include "..\..\udialog.h"
#include "..\shl\wbutil.h"

#include "dbgxprt.h"

#ifndef __UIDIALOG_H__
   #error include 'udialog.h' before including this file
#endif


///////////////////////////////////////////////////////////////////////////////
//  UIExceptions symbols

/*
//#define  UIEXCPT_TITLE 			"Exceptions"
//#define  UIEXCPT_EB_NUMBER	"&Exception Number:"
#define  UIEXCPT_EB_NAME			"&Name:"
//#define  UIEXCPT_RB_ALWAYS			"&Stop Always"
//#define  UIEXCPT_RB_PASS_ONCE			"Stop if &Not Handled"
//#define  UIEXCPT_LB_LIST			"Exception &List:"
#define  UIEXCPT_BTN_HELP			"&Help"
//#define  UIEXCPT_BTN_DEFAULT		"&Defaul&t"
//#define  UIEXCPT_BTN_ADD		"&Add"
//#define  UIEXCPT_BTN_CHANGE		"&Change"
//#define  UIEXCPT_BTN_DELETE		"&Delete"
*/

// Intel Specific?
enum {STOP_ALWAYS, IF_NOT_HANDLED };
const MaxOption=2;



///////////////////////////////////////////////////////////////////////////////
//  UIExceptions class

// BEGIN_CLASS_HELP
// ClassName: UIExceptions
// BaseClass: UIDialog
// Category: Debug
// END_CLASS_HELP
class AFX_EXT_CLASS UIExceptions : public UIDialog

	{

// base class overrides
public:
	UIWND_COPY_CTOR (UIExceptions, UIDialog) ;
	virtual CString ExpectedTitle (void) const{return GetLocString(IDSS_EXCPT_TITLE);}
	virtual BOOL Activate(void);

	// Internal Utilities
	private:
	CString ExcptToString(int thread);

	// Utilities
	public:
		BOOL SetException(int code, int option);
		BOOL SetNumber(int code);
		BOOL SetName(LPCSTR name);
		BOOL SetOption(int option);
		BOOL AddException(void);
		BOOL ChangeException(void);
		BOOL DeleteException(int code);
		CString  GetException(int code);
		BOOL RestoreDefualts(void);

	};

#endif // __UIEXCPT_H__
