#ifndef _transbut_h
#define _transbut_h

#include "btnextra.h"

class CToolBarProxy;

// Callback for destroying toolbar transfer data.
typedef void (FAR PASCAL *TRANSDATA_CALLBACK)(DWORD dwData);

struct TRANSFER_TBBUTTON
{
	// This one is used when the transfer is sourced from another bar. This includes
	// transfers from the button wells in the customize toolbars dialog, as this dialog
	// contains real CCustomBars.
	TRANSFER_TBBUTTON(CToolBarProxy* pBar, int nIndex, BOOL bDeleting);
	// These constructors are used where there isn't a source bar.
	// This one, for a new textual button
	TRANSFER_TBBUTTON(int nCommand, CString *text, BOOL bCustomText);
	// This one, for a new button with a custom glyph
	TRANSFER_TBBUTTON(int nCommand, HBITMAP hbmCustom, BOOL bLarge);
	// This one, for a new button with a custom glyph
	TRANSFER_TBBUTTON(int nCommand, HBITMAP hbmCustom, int nIndex, BOOL bLarge);
	// This one for a new button which is going to use its own inbuilt glyph
	TRANSFER_TBBUTTON(int nCommand);
	// glyph plus text button in a new button with no default glyph
	TRANSFER_TBBUTTON(int nCommand, CString *text, HBITMAP hbmCustom, BOOL bLarge, BOOL bCustomText);
	~TRANSFER_TBBUTTON();

	// Used once we know the destination of a transfer, to load the appropriate bitmap. Also, if the bitmap is
	// non-custom and is the wrong size we toss that and get the right one.
	void LoadCommandBitmap(BOOL bLarge);

	UINT nID;
	UINT nStyle;
	int iImage;
	CSize sizeImage;

	BOOL bDel;
	HBITMAP hbmImageWell;
	TBBUTTON_EXTRA extra;

	// Extra data added by the toolbar derived class.
	DWORD dwData;
	TRANSDATA_CALLBACK lpDataCallback;
};

#endif
