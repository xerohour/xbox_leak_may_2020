#include "stdafx.h"

#include "transbut.h"

#include "barglob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// This constructor is always used when a button is being dragged from an existing toolbar
TRANSFER_TBBUTTON::TRANSFER_TBBUTTON(CToolBarProxy* pBar, int nIndex, BOOL bDeleting)
{
	ASSERT_VALID(pBar->GetWindow());

	lpDataCallback = NULL;

	pBar->GetButtonInfo(nIndex, nID, nStyle, iImage);

	// get button details
	if(pBar->HasExInfo(nIndex))
	{
		HWND hControl;
		CString strText;
		HBITMAP hImage;
		pBar->GetButtonInfoEx(nIndex, hControl, strText, hImage);

		extra.Create(hControl, &strText, hImage);
	}
	else
	{
		extra.Create();
	}

	sizeImage = pBar->GetImageSize();

	if(nStyle & TBBS_CUSTOMBITMAP)
	{
		// as below, make a copy if the original is about to go away
		if(bDeleting)
		{
			extra.SetGlyph(GLOBAL_DATA::DuplicateBitmap(pBar->GetWindow(), extra.GetGlyph()));
		}
	}

	if(nStyle & TBBS_TEXTUAL)
	{
		if((nStyle & TBBS_CUSTOMTEXT)==0)
		{
			LPCTSTR pszText;
			CString strCommand;
			// gett the default string for this item
			theCmdCache.GetCommandString(nID, STRING_MENUTEXT, &pszText);
			if (pszText == NULL || pszText[0] == _T('\0'))
			{
				theCmdCache.GetCommandString(nID, STRING_COMMAND, &pszText);
				// (bobz, paranoia:) we do not want to hold onto the return string from GetCommandString
				// (could be dynamic string held in single global CString). So even though current code
				// would be ok - (InsertMenu below copies the string), I am copying to a local CString
				// to ensure no weird future problems

				strCommand = pszText;
				pszText = (LPCTSTR)strCommand;
			}
			extra.SetLabel(pszText);
			
		}
		else
		{
			// as below, make a copy if the original is about to go away
			if(bDeleting)
			{
				extra.SetLabel(extra.GetLabel());
			}
		}
	}

	pBar->GetButtonTransferInfo(nIndex, dwData, &lpDataCallback);

	bDel = bDeleting;

	if (!bDel)
	{
		extra.SetControl(NULL);
		hbmImageWell = pBar->GetImageWell(nIndex);
	}
	else
	{
		// When deleting the image well must be copied, since the deletion
		// may cause the toolbar to be destroyed.
		hbmImageWell=GLOBAL_DATA::DuplicateBitmap(pBar->GetWindow(), pBar->GetImageWell(nIndex),
			pBar->GetImageSize(),  CPoint(pBar->GetImageSize().cx*iImage,0));
		iImage = 0;
	}
}

// Constructor for transfer of a command button with a custom bitmap object
/* For use only when the button has just been created as a result of dragging
a new button which doesn't have a default bitmap

To simplify the destination's task, we pretend that the custom bitmap is an
image well with only one button inside it
*/
TRANSFER_TBBUTTON::TRANSFER_TBBUTTON(int nCommand, HBITMAP hbmCustom, BOOL bLarge)
{
	ASSERT(nCommand!=0);
	ASSERT(hbmCustom!=NULL);

	lpDataCallback = NULL;

	nStyle=TBBS_CUSTOMBITMAP | TBBS_GLYPH;
	nID=nCommand;

	bDel = FALSE;
	extra.Create();

	sizeImage=CCustomBar::GetDefaultBitmapSize(bLarge);

	// fake the image well as the custom bitmap bitmap
	iImage=0;
	hbmImageWell = hbmCustom;
}

// Constructor for transfer of a command button with a known bitmap in another well
/* For use only when the button has just been created as a result of dragging
a new button which doesn't have a default bitmap

To simplify the destination's task, we pretend that the custom bitmap is an
image well with only one button inside it
*/
TRANSFER_TBBUTTON::TRANSFER_TBBUTTON(int nCommand, HBITMAP hbmCustom, int nIndex, BOOL bLarge)
{
	ASSERT(nCommand!=0);
	ASSERT(hbmCustom!=NULL);

	lpDataCallback = NULL;

	nStyle=TBBS_CUSTOMBITMAP | TBBS_GLYPH;
	nID=nCommand;

	bDel = FALSE;
	extra.Create();

	sizeImage=CCustomBar::GetDefaultBitmapSize(bLarge);

	// !!! handle scaling when sizes are wrong?

	// fake the image well as the custom bitmap bitmap
	iImage=nIndex;
	hbmImageWell = hbmCustom;
}

// Constructor for transfer of a command button which will use the package's
// default bitmap
/* For use only when the button has just been created as a result of dragging
a new button which has a default bitmap
*/
TRANSFER_TBBUTTON::TRANSFER_TBBUTTON(int nCommand)
{
	// slighly hacky way to do separators
	if(nCommand==ID_SEPARATOR)
	{
		nStyle=TBBS_SEPARATOR;
		iImage=-1;
		sizeImage=CSize(0,0);
		hbmImageWell=NULL;
	}
	else
	{
		CPackage *pPackage;
		// copy the package info 
		nStyle=theApp.GetCommandBtnInfo(nCommand, &pPackage);
		if(nStyle & TBBS_TEXTUAL)
		{
			LPCTSTR pText=NULL;
			theCmdCache.GetCommandString(nCommand, STRING_MENUTEXT, &pText);
			if (!pText || *pText=='\0')
			{
				theCmdCache.GetCommandString(nCommand, STRING_COMMAND, &pText);
			}

			CString text=pText;
			extra.Create(NULL, &text, NULL);
		}
		else
		{
			extra.Create();
		}

		ASSERT(pPackage!=NULL);

		// don't get the glyph until we know what the destination is
		sizeImage.cx=sizeImage.cy=0;
		hbmImageWell=NULL;
		iImage=-1;
	}

	lpDataCallback = NULL;

	nID=nCommand;

	bDel = FALSE;
}

// Constructor for transfer of a command button which will use a text string
/* For use only when the button has just been created as a result of dragging
a new button which doesn't have a default bitmap
*/
TRANSFER_TBBUTTON::TRANSFER_TBBUTTON(int nCommand, CString *text, BOOL bCustomText)
{
	ASSERT(nCommand!=0);
	ASSERT(!text->IsEmpty()); // although an empty string won't cause crashes, it's 
							 // probably useful to spot it here in case validation
							 // failed upstream

	lpDataCallback = NULL;

	nStyle=TBBS_TEXTUAL;
	if(bCustomText)
	{
		nStyle |= TBBS_CUSTOMTEXT;
	}
	iImage=0;
	nID=nCommand;
	extra.Create(NULL, text, NULL);

	bDel = TRUE; // always delete this one

	hbmImageWell = NULL;
	sizeImage.cx=sizeImage.cy=0;
}

// Constructor for transfer of a command button with a text and custom bitmap 
/* For use only when the button has just been created as a result of dragging
a new button which doesn't have a default bitmap

To simplify the destination's task, we pretend that the custom bitmap is an
image well with only one button inside it
*/
TRANSFER_TBBUTTON::TRANSFER_TBBUTTON(int nCommand, CString *text, HBITMAP hbmCustom, BOOL bLarge, BOOL bCustomText)
{
	ASSERT(!text->IsEmpty()); // although an empty string won't cause crashes, it's 
							 // probably useful to spot it here in case validation
							 // failed upstream
	ASSERT(nCommand!=0);
	ASSERT(hbmCustom!=NULL);

	lpDataCallback = NULL;

	nStyle=TBBS_CUSTOMBITMAP | TBBS_TEXTUAL | TBBS_GLYPH;
	if(bCustomText)
	{
		nStyle |= TBBS_CUSTOMTEXT;
	}
	nID=nCommand;
	iImage=0;
	bDel = FALSE;
	extra.Create(NULL, new CString(*text), NULL);

	sizeImage=CCustomBar::GetDefaultBitmapSize(bLarge);

	// fake the image well as the custom bitmap bitmap
	iImage=0;
	hbmImageWell = hbmCustom;
}

void TRANSFER_TBBUTTON::LoadCommandBitmap(BOOL bLarge)
{
	if((nStyle & TBBS_GLYPH) &&
		nID!=0)
	{
		if(hbmImageWell!=NULL &&
			iImage!=-1)
		{
			// first decide whether what we've got now is fine
			if(nStyle & TBBS_CUSTOMBITMAP)
			{
				return;
			}

			CSize sizeNewImage=CCustomBar::GetDefaultBitmapSize(bLarge);

			if(sizeNewImage==sizeImage)
			{
				return;
			}

			sizeImage=sizeNewImage;

			if(bDel)
			{
				::DeleteObject(hbmImageWell);
			}

			hbmImageWell=NULL;
			iImage=-1;
		}
		
		// at this point, we know we want to try to get one from the command table
		if(theApp.GetCommandBitmap(nID, &hbmImageWell, &iImage, bLarge))
		{
			return;
		}
		
		// should never reach here, since we shouldn't be tbbs glyph if we don't have one.
		ASSERT(FALSE);
	}
}

TRANSFER_TBBUTTON::~TRANSFER_TBBUTTON()
{
	if (bDel && hbmImageWell != NULL)
		::DeleteObject(hbmImageWell);

	if (bDel)
	{
		// we need to delete the transient items (bitmaps, strings), but the window
		// is always retained to save unnecessary recreation.
		extra.SetControl(NULL);
		extra.Destroy();
	}

	if (lpDataCallback != NULL)
		(*lpDataCallback)(dwData);
}

