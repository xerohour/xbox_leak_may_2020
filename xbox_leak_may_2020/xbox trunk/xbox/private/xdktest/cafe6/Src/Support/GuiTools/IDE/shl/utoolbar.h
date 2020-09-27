//////////////////////////////
//  UTOOLBAR.H
//
//  Created by :            Date :
//      EnriqueP              1/13/94
//
//  Description :
//      Declaration of the UIToolbar class
//

#ifndef __UTOOLBAR_H__
#define __UTOOLBAR_H__

#include "..\..\uwindow.h"

#include "shlxprt.h"

#ifndef __UIWINDOW_H__
   #error include 'uwindow.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
// UITBarDialog #defines


///////////////////////////////////////////////////////////////////////////////
// UIToolbar class declaration

// BEGIN_CLASS_HELP
// ClassName: UIToolbar
// BaseClass: UIWindow
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIToolbar : public UIWindow
{
   	UIWND_COPY_CTOR (UIToolbar, UIWindow) ;

// Utilities
public:
	int DisplayTBDlg(void);
	int ShowTB(int nTbId, BOOL bShow);
	CString GetTBText(int nTbId);
	BOOL IsDocked(HWND hWnd, int nLocation = 0x000F /*DW_DONT_CARE*/); 		
	
	//int Create(LPCSTR szTBName, int aTBButtons);
	//int Delete(LPCSTR szTBName);
	//int Rename(LPCSTR szOldName, LPCSTR szNewName);
	//int AddButtons(LPCSTR szTBName, int aTBButtons, int nPos);
};			



#endif //__UTOOLBAR_H__
