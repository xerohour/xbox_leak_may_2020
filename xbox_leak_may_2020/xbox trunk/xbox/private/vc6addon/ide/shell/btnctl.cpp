// btnctl.cpp : List control for dragging a button
//

#include "stdafx.h"
#include "btnctl.h"
#include "bardockx.h"
#include "prxycust.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonDragListBox
//
// An almost-normal listbox which allows toolbar buttons to be dragged from it

IMPLEMENT_DYNAMIC(CButtonDragListBox, CListBox)

CButtonDragListBox::CButtonDragListBox()
{
}

CButtonDragListBox::~CButtonDragListBox()
{
}

BOOL CButtonDragListBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
	UINT nID)
{
	if (!CListBox::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;

	return TRUE;
}

BEGIN_MESSAGE_MAP(CButtonDragListBox, CListBox)
	//{{AFX_MSG_MAP(CButtonDragListBox)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonDragListBox message handlers

void CButtonDragListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonDown(nFlags, point);

	int nSel=GetCurSel();
	if(nSel!=LB_ERR)
	{
		// this is the command id
		int data=GetItemData(nSel);

		ASSERT(CASBar::s_pCustomizer);

		if (CASBar::s_pCustomizer != NULL)
		{
			// ensure that the list box gets an lbutton up message
			SendMessage(WM_LBUTTONUP, nFlags, MAKELPARAM(point.x, point.y));

			// it's a menu
			if(data==cNextFreeMenu)
			{
				data=theCmdCache.GetNextFreeMenu();
			}

			// do the drag
			CPoint ptScreen=point;
			ClientToScreen(&point);
			theApp.DragCommandButton(ptScreen, data);
		}
	}
}
