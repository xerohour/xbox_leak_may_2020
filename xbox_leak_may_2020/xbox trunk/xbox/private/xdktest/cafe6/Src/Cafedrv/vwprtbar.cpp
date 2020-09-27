/////////////////////////////////////////////////////////////////////////////
// vwprtbar.cpp
//
// email	date		change
// briancr	11/03/94	created
//
// copyright 1994 Microsoft

// Implementation of the CSuiteBar class

#include "stdafx.h"
#include "vwprtbar.h"
#include "caferes.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewportBar buttons
static UINT BASED_CODE btnsViewportBar[] =
{
	IDM_ViewportHeaderFilter,
	IDM_ViewportSummaryFilter,
	IDM_ViewportCommentsFilter,
	IDM_ViewportResultsFilter,
	IDM_ViewportFailureFilter,
	IDM_ViewportInfoFilter,
		ID_SEPARATOR,
	IDM_ViewportSeparatorFilter,
	IDM_ViewportSuccessFilter,
	IDM_ViewportStepFilter
};

/////////////////////////////////////////////////////////////////////////////
// CViewportBar

BOOL CViewportBar::CreateBar(CWnd* pParent)
{
	// create the toolbar
	if (!Create(pParent, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, IDT_ViewportBar) ||
			!LoadBitmap(IDB_VIEWPORTBAR) ||
			!SetButtons(btnsViewportBar, sizeof(btnsViewportBar)/sizeof(UINT))) {
		TRACE("CViewportBar::CreateBar: Failed to create ViewportBar\n");
		return FALSE;       // fail to create
	}

	// make the toolbar dockable
	EnableDocking(CBRS_ALIGN_ANY);

	// set the title
	SetWindowText("Viewport");

	return TRUE;
}
