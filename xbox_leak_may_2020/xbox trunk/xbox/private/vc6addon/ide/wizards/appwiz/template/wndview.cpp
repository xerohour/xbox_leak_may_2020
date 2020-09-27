// $$wndview_ifile$$.cpp : implementation of the $$WNDVIEW_CLASS$$ class
//

#include "stdafx.h"
#include "$$root$$.h"
#include "$$wndview_hfile$$.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$WNDVIEW_CLASS$$

$$WNDVIEW_CLASS$$::$$WNDVIEW_CLASS$$()
{
}

$$WNDVIEW_CLASS$$::~$$WNDVIEW_CLASS$$()
{
}


BEGIN_MESSAGE_MAP($$WNDVIEW_CLASS$$,$$WNDVIEW_BASE_CLASS$$ )
	//{{AFX_MSG_MAP($$WNDVIEW_CLASS$$)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// $$WNDVIEW_CLASS$$ message handlers

BOOL $$WNDVIEW_CLASS$$::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void $$WNDVIEW_CLASS$$::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
$$IF(VERBOSE)
	// TODO: Add your message handler code here
$$ENDIF
	
	// Do not call CWnd::OnPaint() for painting messages
}

