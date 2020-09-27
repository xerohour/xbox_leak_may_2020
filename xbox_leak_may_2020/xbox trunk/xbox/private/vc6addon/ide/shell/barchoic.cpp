///////////////////////////////////////////////////////////////////////////////
//  BARCHOIC.CPP
//      Implements CChoiceBar, the choose a button bar class.

// This subclass is requred because the choose button dialog box contains a CCustomBar which does not wish to
// share its customizer with the other CASBars (because we want to display one selected toolbar button at the same
// time as allowing selection changes within the choose button dialog).

// This subclass also ensures that the embedded bar has a default orientation of orNil, to avoid grabbers being drawn in
// the dialog.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "barchoic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
//	CChoiceBar
//		Derived from CCustomBar, provides a bar with a unique customiser

IMPLEMENT_DYNAMIC(CChoiceBar, CCustomBar)

//	CChoiceBar::CChoiceBar
CChoiceBar::CChoiceBar()           
{
	// this bar has a private customizer
	m_pCustomizer=NULL;
}

//	CChoiceBar::~CChoiceBar
CChoiceBar::~CChoiceBar()
{
}

CToolCustomizer *CChoiceBar::GetCustomizer()
{
	return m_pCustomizer;
}

void CChoiceBar::SetCustomizer(CToolCustomizer *theCustomizer)
{
	m_pCustomizer=theCustomizer;
}

ORIENTATION CChoiceBar::GetOrient() const 
{
	return orNil;
}

#ifdef _DEBUG
void CChoiceBar::AssertValid() const
{
	// deliberate skipping of base class, because we changed the assumptions... 
	CWnd::AssertValid();

	ASSERT(m_nCount == 0 || m_pData != NULL);
}
#endif
