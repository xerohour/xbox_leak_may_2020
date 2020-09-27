/////////////////////////////////////////////////////////////////////////////
// suitebar.cpp
//
// email	date		change
// briancr	11/03/94	created
//
// copyright 1994 Microsoft

// Implementation of the CSuiteBar class

#include "stdafx.h"
#include "suitebar.h"
#include "caferes.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSuiteBar buttons
static UINT BASED_CODE btnsSubSuiteBar[] =
{
	IDM_SubSuiteRun,
		ID_SEPARATOR,
#ifdef CAFE_V31
	IDM_SubSuiteAdd,
	IDM_SubSuiteDelete,
		ID_SEPARATOR,
#endif
	IDM_SubSuiteSelectAll,
	IDM_SubSuiteDeselectAll,
	IDM_SubSuiteToggleAll,
#ifdef CAFE_V31
		ID_SEPARATOR,
//	IDM_SubSuiteProperties,
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CSubSuiteBar

BOOL CSubSuiteBar::CreateBar(CWnd* pParent)
{
	// create the toolbar
	if (!Create(pParent, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, IDT_SubSuiteBar) ||
			!LoadBitmap(IDB_SUBSUITEBAR) ||
			!SetButtons(btnsSubSuiteBar, sizeof(btnsSubSuiteBar)/sizeof(UINT))) {
		TRACE("CSubSuiteBar::CreateBar: Failed to create SubSuiteBar\n");
		return FALSE;       // fail to create
	}

	// make the toolbar dockable
	EnableDocking(CBRS_ALIGN_ANY);

	// set the title
	SetWindowText("SubSuite");

	return TRUE;
}
