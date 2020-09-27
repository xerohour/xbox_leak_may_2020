// ReadOnlyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "readonlyedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MY_BACKGROUND_COLOR ::GetSysColor( COLOR_WINDOW )

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyEdit

CReadOnlyEdit::CReadOnlyEdit()
{
}

CReadOnlyEdit::~CReadOnlyEdit()
{
}


BEGIN_MESSAGE_MAP(CReadOnlyEdit, CEdit)
	//{{AFX_MSG_MAP(CReadOnlyEdit)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyEdit message handlers

HBRUSH CReadOnlyEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	pDC->SetBkColor( MY_BACKGROUND_COLOR );

	// TODO: Return a non-NULL brush if the parent's handler should not be called
	LOGBRUSH lb;
	lb.lbColor = MY_BACKGROUND_COLOR;
	lb.lbHatch = 0;
	lb.lbStyle = BS_SOLID;
	return ::CreateBrushIndirect( &lb );
}
