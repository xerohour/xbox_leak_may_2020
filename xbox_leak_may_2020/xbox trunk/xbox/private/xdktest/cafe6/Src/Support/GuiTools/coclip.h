///////////////////////////////////////////////////////////////////////////////
//	COCLIP.H
//
//	Created by :			Date :
//		DavidGa					1/10/94
//
//	Description :
//		Declaration of the COClipboard component object class
//

#ifndef __COCLIP_H__
#define __COCLIP_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "guixprt.h"

///////////////////////////////////////////////////////////////////////////////
// COClipboard class

// BEGIN_CLASS_HELP
// ClassName: COClipboard
// BaseClass: none
// Category: General
// END_CLASS_HELP
class GUI_CLASS COClipboard
{
public:
	COClipboard();

// Data

// Attributes
public:
	CString GetText(void);

// Operations
public:
	BOOL Empty(void);
	BOOL SetText(LPCSTR szTextIn);
};

#endif //__COCLIP_H__
