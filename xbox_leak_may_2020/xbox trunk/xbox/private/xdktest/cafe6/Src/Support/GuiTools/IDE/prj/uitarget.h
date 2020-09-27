///////////////////////////////////////////////////////////////////////////////
//	UITARGET.H
//
//	Created by :			Date :
//		IvanL				2/7/94
//
//	Description :
//		Declaration of the UIProjectTarget class
//

#ifndef __UITARGET_H__
#define __UITARGET_H__

#include "..\..\udialog.h"

#include "prjxprt.h"

#ifndef __UIDIALOG_H__
	#error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//	UIProjectTarget class

// BEGIN_CLASS_HELP
// ClassName: UIProjectTarget
// BaseClass: UIDialog
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UIProjectTarget : public UIDialog
{
	UIWND_COPY_CTOR(UIProjectTarget, UIDialog);

// Data
protected:
	CString m_ActTar;

// Utilities
public:
	HWND Show() ;
	int New(LPCSTR Name, int Type, BOOL Debug) ;
	int New(LPCSTR Name, LPCSTR Target) ;
	int Delete(LPCSTR DelName) ;
	int Rename(LPCSTR OldName, LPCSTR NewName) ;
	int SetTarget(LPCSTR Target) ;
	LPCSTR GetTarget(int Index = 0) ;
	HWND Close() ;
private:
	int Select(LPCSTR SelName, BOOL LogWarning = TRUE) ;
	};

#endif //__UITARGET_H__
