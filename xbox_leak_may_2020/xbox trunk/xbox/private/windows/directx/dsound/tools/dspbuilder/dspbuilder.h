/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	dspbuilder.h

Abstract:

	Main head file

Author:

	Robert Heitkamp (robheit) 08-Oct-2001

Revision History:

	08-Oct-2001 robheit
		Initial Version

--*/

#if !defined(AFX_DSPBUILDER_H__A4399BA9_E747_41EE_86C6_5881A0DA3E7E__INCLUDED_)
#define AFX_DSPBUILDER_H__A4399BA9_E747_41EE_86C6_5881A0DA3E7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <afxwin.h>         // MFC core and standard components
#include <afxcmn.h>
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#include "xboxverp.h"
#include <stdio.h>
#include <stdlib.h>
#include "xboxdbg.h"
#include "dsfxparmp.h"

#include "parser.h"
#include "..\inc\dsp.h"
#include "..\inc\cipher.h"


#include "resource.h"       // main symbols

//------------------------------------------------------------------------------
//	Globals Defines:
//------------------------------------------------------------------------------
#define MAX_SIZE	65535
#define SCALE		13

//------------------------------------------------------------------------------
//	Globals Macros:
//------------------------------------------------------------------------------
#define MAX(a,b) (((a) >= (b)) ? (a) : (b))
#define MIN(a,b) (((a) <= (b)) ? (a) : (b))
#define MEDIAN(a,b) (((a) <= (b)) ? (((b) - (a)) / 2 + (a)) : (((a) - (b)) / 2 + (b)))

//------------------------------------------------------------------------------
//	Globals Methods
//------------------------------------------------------------------------------
inline BOOL IsPointInRect(int x, int y, const CRect& r)
	{ return ((x >= r.left) && (x <= r.right) && (y >= r.top) && (y <= r.bottom)); };

//------------------------------------------------------------------------------
//	CDspbuilderApp
//------------------------------------------------------------------------------
class CDspbuilderApp : public CWinApp
{
public:
	CDspbuilderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDspbuilderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CDspbuilderApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSPBUILDER_H__A4399BA9_E747_41EE_86C6_5881A0DA3E7E__INCLUDED_)
