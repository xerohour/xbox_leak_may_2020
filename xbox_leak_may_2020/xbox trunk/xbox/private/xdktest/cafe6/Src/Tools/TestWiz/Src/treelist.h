// treelist.h : Ownerdraw listbox 

#include "stdafx.h"

#ifndef _AREALISTBOX_
#define _AREALISTBOX_

class CAreasLB : public CListBox
{
public:
// Implementation
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCIS);
};

#endif // _CUSTOMLISTBOX_
