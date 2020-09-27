// proptab.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "proptab.h"
#include "hotlinkc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CPropertyTab

CPropertyTab::~CPropertyTab()
{
}


BEGIN_MESSAGE_MAP(CPropertyTab, CDlgTab)
	//{{AFX_MSG_MAP(CPropertyTab)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HOTLINK, OnHotLink)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyTab message handlers


BOOL CPropertyTab::OnInitDialog()
{
	CDlgTab::OnInitDialog();

	PINT paControlIDs = GetControlIDTable();

	if (paControlIDs)
	{
		CFont* pFont = GetStdFont(font_Normal);
		ASSERT(pFont);
		HFONT hfont = (HFONT)pFont->m_hObject;

		while (*paControlIDs)
		{
			SendDlgItemMessage(*paControlIDs, WM_SETFONT, (WPARAM)hfont, FALSE);
			paControlIDs++;
		}
	}

	//
	// Update the dialogs fields with the current set of data,
	// but only if the object is valid (which means the data
	// has also been properly initialized).
	//
	if (((CPropertyInspector*)GetParent())->GetValidObjectFlag())
	{
		UpdateFields();
		SetRefreshFlag(FALSE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}



BOOL CPropertyTab::Activate(CTabbedDialog* pParentWnd, CPoint position)
{
	//
	// If this tab has already been loaded and the data in it
	// needs to be refreshed, do the updating now before it is
	// activated (which will show it) to prevent the old data
	// briefly showing.  If it has not already been loaded, we
	// don't need to do anything because the load will refresh
	// the data at OnInitDialog time.
	//
	if (m_hWnd != NULL && GetRefreshFlag())
	{
		UpdateFields();
		SetRefreshFlag(FALSE);
	}

	return CDlgTab::Activate(pParentWnd, position);
}


LPARAM CPropertyTab::OnHotLink(WPARAM wParam, LPARAM lParam)
{
	CPropertyInspector* pTabDlg = (CPropertyInspector*)GetParent();

	pTabDlg->ChangeObject((DWORD)wParam, (int)lParam);

	return 0;
}
