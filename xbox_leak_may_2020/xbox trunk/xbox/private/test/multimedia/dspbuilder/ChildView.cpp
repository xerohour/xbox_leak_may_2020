/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	ChildView.cpp

Abstract:

	The primary window for the dsp builder

Author:

	Robert Heitkamp (robheit) 07-May-2001

Revision History:

	07-May-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "dspbuilder.h"
#include "ChildView.h"

//------------------------------------------------------------------------------
//	Stuff:
//------------------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------
//	CChildView::CChildView
//------------------------------------------------------------------------------
CChildView::CChildView(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	m_x	= 0;
	m_y	= 0;
}

//------------------------------------------------------------------------------
//	CChildView::~CChildView
//------------------------------------------------------------------------------
CChildView::~CChildView(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_FILE_SAVE_AS_IMAGE, OnFileSaveAsImage)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//------------------------------------------------------------------------------
//	CChildView::PreCreateWindow
//------------------------------------------------------------------------------
BOOL 
CChildView::PreCreateWindow(
							IN OUT CREATESTRUCT&	cs
							) 
/*++

Routine Description:

	Called before the window is created to adjust in initialization params

Arguments:

	IN OUT cs -	CREATESTRUCT (modify this)

Return Value:

	TRUE on success, otherwise FALSE

--*/
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.style |= WS_VSCROLL | WS_HSCROLL;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

//------------------------------------------------------------------------------
//	CChildView::OnPaint
//------------------------------------------------------------------------------
void 
CChildView::OnPaint(void) 
/*++

Routine Description:

	Handles all expose events

Arguments:

	None

Return Value:

	None

--*/
{
	CRect	rect;

	// Get the update region before creating the CPaintDC because CPaintDC
	// will clear the region
	GetUpdateRect(rect);

	rect.left	+= (m_x - 1);
	rect.right	+= (m_x + 1);
	rect.top	+= (m_y - 1);
	rect.bottom	+= (m_y + 1);

	CPaintDC	dc(this);

	dc.SetWindowOrg(m_x, m_y);	
	m_grid.Draw(rect, &dc);
}


//------------------------------------------------------------------------------
//	CChildView::OnCreate
//------------------------------------------------------------------------------
int 
CChildView::OnCreate(
					 IN OUT LPCREATESTRUCT	lpCreateStruct
					 ) 
/*++

Routine Description:

	Create event. Create the grid and setup the scroll bars

Arguments:

	IN OUT lpCreateStruct -	Create parameters

Return Value:

	0 on success, anything else on failure

--*/
{
	SCROLLINFO	scrollInfo;
	CRect		rect;

	// Base class callback
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the grid
	m_grid.Create(this);
	m_gridVSize = m_grid.GetHeight() * m_grid.GetCellSize() + m_grid.GetYOffset() * 2;
	m_gridHSize = m_grid.GetWidth() * m_grid.GetCellSize() + m_grid.GetXOffset() * 2;

	GetClientRect(rect);

	// Vertical scroll bar
	scrollInfo.cbSize		= sizeof(SCROLLINFO);
	scrollInfo.fMask		= SIF_ALL;
	scrollInfo.nMin			= 0;
	scrollInfo.nMax			= m_gridVSize-1;
	scrollInfo.nPage		= rect.bottom - rect.top;
	scrollInfo.nPos			= 0;
	scrollInfo.nTrackPos	= 0;
	SetScrollInfo(SB_VERT, &scrollInfo);

	// Horizontal scroll bar (hidden for now)
	scrollInfo.cbSize		= sizeof(SCROLLINFO);
	scrollInfo.fMask		= SIF_ALL;
	scrollInfo.nMin			= 0;
	scrollInfo.nMax			= m_gridHSize-1;
	scrollInfo.nPage		= rect.right - rect.left;
	scrollInfo.nPos			= 0;
	scrollInfo.nTrackPos	= 0;
	SetScrollInfo(SB_HORZ, &scrollInfo);

	return 0;
}

void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd ::OnLButtonDblClk(nFlags, point);
}

//------------------------------------------------------------------------------
//	CChildView::OnLButtonDown
//------------------------------------------------------------------------------
void 
CChildView::OnLButtonDown(
						  IN UINT	nFlags, 
						  IN CPoint	point
						  ) 
/*++

Routine Description:

	Handles left mouse down events by passing them to the grid

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was clicked

ReturnValue:

	None

--*/
{
	// Trap the mouse
	SetCapture();
	m_grid.OnLButtonDown(nFlags, point);
}

//------------------------------------------------------------------------------
//	CChildView::OnLButtonUp
//------------------------------------------------------------------------------
void 
CChildView::OnLButtonUp(
						IN UINT		nFlags, 
						IN CPoint	point
						) 
/*++

Routine Description:

	Passes left mouse up events to the grid

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was released

Return Value:

	None

--*/
{
	ReleaseCapture();
	m_grid.OnLButtonUp(nFlags, point);
}

//------------------------------------------------------------------------------
//	CChildView::OnRButtonDown
//------------------------------------------------------------------------------
void 
CChildView::OnRButtonDown(
						  IN UINT	nFlags, 
						  IN CPoint	point
						  ) 
/*++

Routine Description:

	Passes right mouse up events to the grid

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was pressed

Return Value:

	None

--*/
{
	m_grid.OnRButtonDown(nFlags, point);
}

//------------------------------------------------------------------------------
//	CChildView::OnSize
//------------------------------------------------------------------------------
void 
CChildView::OnSize(
				   IN UINT	nType, 
				   IN int	cx, 
				   IN int	cy
				   ) 
{
	SCROLLINFO	scrollInfo;

	CWnd ::OnSize(nType, cx, cy);
	
	// Adjust the scroll bars
	scrollInfo.cbSize		= sizeof(SCROLLINFO);
	scrollInfo.fMask		= SIF_PAGE;
	scrollInfo.nPage		= cy;
	SetScrollInfo(SB_VERT, &scrollInfo);

	scrollInfo.nPage		= cx;
	SetScrollInfo(SB_HORZ, &scrollInfo);
}

//------------------------------------------------------------------------------
//	CChildView::OnVScroll
//------------------------------------------------------------------------------
void 
CChildView::OnVScroll(
					  IN UINT			nSBCode, 
					  IN UINT			nPos, 
					  IN CScrollBar*	pScrollBar
					  ) 
{
	int			pos;
	SCROLLINFO	scrollInfo;

	if(!m_grid.IsOkToScroll())
		return;

	if(!GetScrollInfo(SB_VERT, &scrollInfo, SIF_PAGE))
		return;

	switch(nSBCode)
	{
	case SB_LINEUP:
		pos	= m_y;
		m_y = GetScrollPos(SB_VERT);
		m_y -= m_grid.GetCellSize();
		if(m_y < 0)
			m_y = 0;
		SetScrollPos(SB_VERT, m_y);
		ScrollWindow(0, pos - m_y);
		break;

	case SB_LINEDOWN:
		pos	= m_y;
		m_y = GetScrollPos(SB_VERT);
		m_y += m_grid.GetCellSize();
		if(m_y > (m_gridVSize - (int)scrollInfo.nPage))
			m_y = m_gridVSize - (int)scrollInfo.nPage;
		SetScrollPos(SB_VERT, m_y);
		ScrollWindow(0, pos - m_y);
		break;

	case SB_PAGEUP:
		m_y = GetScrollPos(SB_VERT);
		m_y -= scrollInfo.nPage;
		if(m_y < 0)
			m_y = 0;
		SetScrollPos(SB_VERT, m_y);
		Invalidate();
		break;

	case SB_PAGEDOWN:
		m_y = GetScrollPos(SB_VERT);
		m_y += (int)scrollInfo.nPage;
		if(m_y > (m_gridVSize - (int)scrollInfo.nPage))
			m_y = m_gridVSize - (int)scrollInfo.nPage;
		SetScrollPos(SB_VERT, m_y);
		Invalidate();
		break;

	case SB_THUMBTRACK:
		ScrollWindow(0, m_y - (int)nPos);
		m_y = (int)nPos;
		SetScrollPos(SB_VERT, nPos);
		break;
	}
}

//------------------------------------------------------------------------------
//	CChildView::OnHScroll
//------------------------------------------------------------------------------
void 
CChildView::OnHScroll(
					  IN UINT			nSBCode, 
					  IN UINT			nPos, 
					  IN CScrollBar*	pScrollBar
					  ) 
{
	int			pos;
	SCROLLINFO	scrollInfo;

	if(!m_grid.IsOkToScroll())
		return;

	if(!GetScrollInfo(SB_HORZ, &scrollInfo, SIF_PAGE))
		return;

	switch(nSBCode)
	{
	case SB_LINEUP:
		pos	= m_x;
		m_x = GetScrollPos(SB_HORZ);
		m_x -= m_grid.GetCellSize();
		if(m_x < 0)
			m_x = 0;
		SetScrollPos(SB_HORZ, m_x);
		ScrollWindow(pos - m_x, 0);
		break;

	case SB_LINEDOWN:
		pos	= m_x;
		m_x = GetScrollPos(SB_HORZ);
		m_x += m_grid.GetCellSize();
		if(m_x > (m_gridHSize - (int)scrollInfo.nPage))
			m_x = m_gridHSize - (int)scrollInfo.nPage;
		SetScrollPos(SB_HORZ, m_x);
		ScrollWindow(pos - m_x, 0);
		break;

	case SB_PAGEUP:
		m_x = GetScrollPos(SB_HORZ);
		m_x -= scrollInfo.nPage;
		if(m_x < 0)
			m_x = 0;
		SetScrollPos(SB_HORZ, m_x);
		Invalidate();
		break;

	case SB_PAGEDOWN:
		m_x = GetScrollPos(SB_HORZ);
		m_x += (int)scrollInfo.nPage;
		if(m_x > (m_gridHSize - (int)scrollInfo.nPage))
			m_x = m_gridHSize - (int)scrollInfo.nPage;
		SetScrollPos(SB_HORZ, m_x);
		Invalidate();
		break;

	case SB_THUMBTRACK:
		ScrollWindow(m_x - (int)nPos, 0);
		m_x = (int)nPos;
		SetScrollPos(SB_HORZ, nPos);
		break;
	}
}

//------------------------------------------------------------------------------
//	CChildView::OnMouseWheel
//------------------------------------------------------------------------------
BOOL 
CChildView::OnMouseWheel(
						 IN UINT	nFlags, 
						 IN short	zDelta, 
						 IN CPoint	pt
						 ) 
/*++

Routine Description:

	Respond to mouse wheel movement (VScroll)

Arguments:

	IN nFlags -	State of mouse buttons and control/shift keys
	IN zDelta -	Mouse wheel movement
	IN pt -		Mouse location

Return Value:

	None

--*/
{
	int			pos;
	SCROLLINFO	scrollInfo;

	if(!m_grid.IsOkToScroll())
		return TRUE;

	if(!GetScrollInfo(SB_VERT, &scrollInfo, SIF_PAGE))
		return TRUE;

	if(zDelta > 0)
	{
		pos	= m_y;
		m_y = GetScrollPos(SB_VERT);
		m_y -= m_grid.GetCellSize();
		if(m_y < 0)
			m_y = 0;
		SetScrollPos(SB_VERT, m_y);
		ScrollWindow(0, pos - m_y);
	}
	else if(zDelta < 0)
	{
		pos	= m_y;
		m_y = GetScrollPos(SB_VERT);
		m_y += m_grid.GetCellSize();
		if(m_y > (m_gridVSize - (int)scrollInfo.nPage))
			m_y = m_gridVSize - (int)scrollInfo.nPage;
		SetScrollPos(SB_VERT, m_y);
		ScrollWindow(0, pos - m_y);
	}
	return TRUE;
}

//------------------------------------------------------------------------------
//	CChildView::OnMouseMove
//------------------------------------------------------------------------------
void 
CChildView::OnMouseMove(
						IN UINT		nFlags, 
						IN CPoint	point
						) 
/*++

Routine Description:

	Passes mouse move events to the grid

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was pressed

Return Value:

	None

--*/
{
	m_grid.OnMouseMove(nFlags, point);
}

//------------------------------------------------------------------------------
//	CChildView::EnableMenuItem
//------------------------------------------------------------------------------
void 
CChildView::EnableMenuItem(
						   IN CMenu*	pPopupMenu,
						   IN UINT		nIndex
						   )
/*++

Routine Description:

	Enables or disables menu items

Arguments:

	IN pPopupMenu -	Menu
	IN nIndex -		Item

Return Value:

	None

--*/
{
	switch(nIndex)
	{
	case 0:	// File
		pPopupMenu->EnableMenuItem(ID_FILE_NEW, m_grid.IsPatched() || (m_grid.GetNumEffects() > 0) ? MF_ENABLED : MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_FILE_OPEN, MF_ENABLED);
		pPopupMenu->EnableMenuItem(ID_FILE_SAVE, m_grid.GetChangesMade() ? MF_ENABLED : MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_FILE_SAVE_AS, MF_ENABLED);
		pPopupMenu->EnableMenuItem(ID_FILE_SAVE_AS_IMAGE, m_grid.GetNumEffects() > 0 ? MF_ENABLED : MF_GRAYED);
		break;

	case 1:	// Edit
		pPopupMenu->EnableMenuItem(ID_EDIT_UNDO, MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_EDIT_CUT, MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_EDIT_COPY, MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_EDIT_PASTE, MF_GRAYED);
		break;

	case 3:	// Xbox
		pPopupMenu->EnableMenuItem(ID_XBOX_CONNECT, MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_XBOX_EXPORT_IMAGE, MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_XBOX_TEST, MF_GRAYED);
		break;

	case 4:	// Tools
		pPopupMenu->EnableMenuItem(ID_TOOLS_OPTIONS, MF_GRAYED);
		break;	

	case 5:	// Help
		break;
	}

}

//------------------------------------------------------------------------------
//	CChildView::OnFileSaveAsImage
//------------------------------------------------------------------------------
void 
CChildView::OnFileSaveAsImage(void) 
/*++

Routine Description:

	Saves the data as a dsp image

Arguments:

	None

Return Value:

	None

--*/
{
	CFileDialog	fileDialog(FALSE, _T("ini"), NULL, OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
						   _T("DSP Image Files (.ini)|*.ini|All Files (*.*)|*.*||"));
	if(fileDialog.DoModal() == IDOK)
		m_grid.SaveImage(fileDialog.GetPathName());
}

//------------------------------------------------------------------------------
//	CChildView::OnFileSave
//------------------------------------------------------------------------------
void 
CChildView::OnFileSave(void) 
/*++

Routine Description:

	Saves the data as a grid file

Arguments:

	None

Return Value:

	None

--*/
{
	m_grid.Save(NULL);
}

//------------------------------------------------------------------------------
//	CChildView::OnFileSaveAs
//------------------------------------------------------------------------------
void 
CChildView::OnFileSaveAs(void) 
/*++

Routine Description:

	Saves the grid as a new name

Arguments:

	None

Return Value:

	None

--*/
{
	CFileDialog	fileDialog(FALSE, _T("dsp"), NULL, OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
						   _T("DSP Builder Files (.dsp)|*.dsp|All Files (*.*)|*.*||"));
	if(fileDialog.DoModal() == IDOK)
		m_grid.Save(fileDialog.GetPathName());
}

//------------------------------------------------------------------------------
//	CChildView::OnFileNew
//------------------------------------------------------------------------------
void 
CChildView::OnFileNew(void) 
/*++

Routine Description:

	Create a new grid file

Arguments:

	None

Return Value:

	None

--*/
{
	m_grid.New();
}

//------------------------------------------------------------------------------
//	CChildView::OnFileOpen
//------------------------------------------------------------------------------
void 
CChildView::OnFileOpen(void) 
/*++

Routine Description:

	Opens a new grid file

Arguments:

	None

Return Value:

	None

--*/
{
	CFileDialog	fileDialog(TRUE, _T("dsp"), NULL, 
						   OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
						   _T("DSP Builder Files (.dsp)|*.dsp|All Files (*.*)|*.*||"));
	if(fileDialog.DoModal() == IDOK)
		m_grid.Open(fileDialog.GetPathName());
}
