#include "stdafx.h"
#include "btnextra.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void TBBUTTON_EXTRA::Create(HWND theControl, CString *theLabel, HBITMAP theCustom)
{
	hwndControl=theControl;
	if (theLabel != NULL)
		strLabel = *theLabel;
	hbmGlyph=theCustom;
	nLabelSize=CSize(0,0);
};

void TBBUTTON_EXTRA::Copy(TBBUTTON_EXTRA *src)
{ 
	AssertValid(); 

	SetControl(src->GetControl());
	SetLabel(src->GetLabel());
	SetGlyph(src->GetGlyph());
	SetLabelSize(CSize(0,0)); // don't copy this - may well be different in new context
};

void TBBUTTON_EXTRA::Destroy()
{
	AssertValid(); 

	if(hwndControl!=NULL)
	{
		::DestroyWindow(hwndControl);
		hwndControl=NULL;
	}
	strLabel.Empty();
	if(hbmGlyph!=NULL)
	{
		::DeleteObject(hbmGlyph);
		hbmGlyph=NULL;
	}
}

#ifdef _DEBUG
void TBBUTTON_EXTRA::AssertValid() const
{
	ASSERT(this!=NULL);

	ASSERT(hwndControl==NULL || IsWindow(hwndControl));
}
#endif

