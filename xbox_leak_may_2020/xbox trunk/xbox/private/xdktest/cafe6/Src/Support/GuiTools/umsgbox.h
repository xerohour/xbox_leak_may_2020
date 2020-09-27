///////////////////////////////////////////////////////////////////////////////
//   UIMSGBOX.H
//
//  Created by :            Date :
//      DavidGa                 1/12/94
//
//  Description :
//      Declaration of the  UIMessageBox class
//

#ifndef __UIMSGBOX_H__
#define __UIMSGBOX_H__

#include "uwindow.h"

#include "guixprt.h"

#ifndef __UIWINDOW_H__
    #error include 'uwindow.h' before including this file
#endif

#define MSG_DEFAULT UIMessageBox::m_idDefaultButton
#define MSG_OK		UIMessageBox::m_idOKButton
#define MSG_CANCEL	UIMessageBox::m_idCancelButton
#define MSG_ABORT	UIMessageBox::m_idAbortButton
#define MSG_RETRY	UIMessageBox::m_idRetryButton
#define MSG_IGNORE	UIMessageBox::m_idIgnoreButton
#define MSG_YES		UIMessageBox::m_idYesButton
#define MSG_NO		UIMessageBox::m_idNoButton


///////////////////////////////////////////////////////////////////////////////
//   UIMessageBox class

// BEGIN_CLASS_HELP
// ClassName: UIMessageBox
// BaseClass: UIWindow
// Category: General
// END_CLASS_HELP
class GUI_CLASS UIMessageBox : public  UIWindow
{
     UIWND_COPY_CTOR(UIMessageBox,  UIWindow);

// Utilities
public:
	virtual BOOL IsValid(void) const;
	CString GetMessageText(void);

// Standard buttons
public:
	BOOL ButtonClick(int btn = MSG_DEFAULT);
	BOOL ButtonExists(int btn);
	int GetActiveButton(void);

	static int m_idDefaultButton;
	static int m_idOKButton;
	static int m_idCancelButton;
	static int m_idAbortButton;
	static int m_idRetryButton;
	static int m_idIgnoreButton;
	static int m_idYesButton;
	static int m_idNoButton;
};

#endif //__UIMSGBOX_H__
