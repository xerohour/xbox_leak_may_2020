// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
// Auxiliary System/Screen metrics

struct GLOBAL_DATA
{
	// system metrics
	int cxBorder2, cyBorder2;

	// device metrics for screen
	int cxPixelsPerInch, cyPixelsPerInch;

	// button glyph sizes
	CSize csSmallButtonGlyph;
	CSize csLargeButtonGlyph;
	// button enclosure sizes
	CSize csSmallButton;
	CSize csLargeButton;

	// small icon size (includes caption bar)
	CSize csSmallIcon;
	// right end of caption bar button sizes
	CSize csCaptionButton;
	// gap between restore button and close button (horz and vertical orientations)
	int cxPreCloseGap;
	int cyPreCloseGap;
	int cxPostCloseGap;
	int cyPostCloseGap;
	int cxPreMinimizeGap;
	int cyPreMinimizeGap;

	int tmDoubleClick;
	int cxDoubleClick;
	int cyDoubleClick;

	// solid brushes with convenient gray colors and system colors
	HBRUSH hbrLtGray, hbrDkGray;
	HBRUSH hbrBtnHilite, hbrBtnFace, hbrBtnShadow;
	HBRUSH hbrWindowFrame;
	HPEN hpenBtnHilite, hpenBtnShadow, hpenBtnText;

	// color values of system colors used for CToolBar
	COLORREF clrBtnFace, clrBtnShadow, clr3dDkShadow, clrBtnHilite;
	COLORREF clrBtnText, clrGrayText, clrWindowFrame, clrMenu;

	// special GDI objects allocated on demand
	HFONT   hStatusFont;
	HFONT   hToolTipsFont;
	HFONT   hButtonVerticalFont;
	HFONT	hButtonVerticalFontLarge;

	// auxiliary toolbar bitmaps
	HBITMAP hbmMenuArrow;
	HBITMAP hbmMenuArrowDisabled;
	HBITMAP hbmMenuArrowVertical;
	HBITMAP hbmMenuArrowVerticalDisabled;

	// other system information
	UINT    nWinVer;			// Major.Minor version numbers
	BOOL	bWin32s;			// TRUE if Win32s (or Windows 95)
	BOOL    bWin4;				// TRUE if Windows 4.0
	BOOL    bNotWin4;			// TRUE if not Windows 4.0
	BOOL    bSmCaption;			// TRUE if WS_EX_SMCAPTION is supported
	BOOL	bWin31; 			// TRUE if actually Win32s on Windows 3.1
	int		nMenuShowDelay;		// delay for menus in milliseconds (SPI_MENUSHOWDELAY)

// Implementation
	GLOBAL_DATA();
	~GLOBAL_DATA();
	void UpdateSysColors();
	void UpdateSysMetrics();

	// MenuTextOut -
	// This function does all of the text drawing for CmdBar menus and buttons.
	// Its advantage over ExtTextOut is that it draws shadowed disabled text and
	// it underlines characters after the & character. The reason that we
	// ExtTextOut instead of DrawText is that buttons in vertically dock toolbars
	// require a vertical font which DrawText doesn't support.

	enum eMenuTextOut
	{
		// This are bit flags.
		e_mtoLeftAlign = 1,		// Align with the left edge
		e_mtoCenterAlign = 2,		// Center horizontally.
		e_mtoRightAlign = 4,
		e_mtoVertical = 8,			// Set if text is vertical for docking toolbars
		e_mtoDisabled = 16			// Set if the text is gray.
	};


	void MenuTextOut(CDC* pDC, 
					 const CRect* pRect,	// Bounding rectangle
					 const CString& text, 
					 UINT eFlags = e_mtoLeftAlign) ;


	// this routine is used to draw the 'embossed' disabled text used in menus and
	// buttons under Win95 and NT4. Windows doesn't export a routine that does this,
	// and there are several special cases to be accounted for, including mono monitors
	// the case when the menu and 3d colours are different, and handling for NT3.51.

	// This routine draws using the currently selected font at the point specified, using 
	// TA_TOP | TA_CENTRE
	
	// martynl 2ndApr96

//	void DrawDisabledText(CDC *pDC, CPoint ptPos, const CString &text, 
//						  eDrawDisabledTextFlags eFlags = e_Default,
//						  const CRect* pRect = NULL );
	// DER - 7 Jun 96 Revamped for menus
	// Now uses TA_TOP | TA_LEFT
	void DisabledTextOut(	CDC *pDC, 
							const CRect* pRect,
							const CString &text,
							UINT eFlags) ;


	// Draws text using ExtTextOut, but will underline strings with accelerators.
	// Used by DrawDisabledText which requires ExtTextOut for vertical text.
//	BOOL UnderlineTextOut(CDC* pdc, int x, int y, BOOL bVertical, 
//						  LPCTSTR str) ;//const CString& str);

	void UnderlineTextOut(CDC* pdc, 
						   const CRect* pRect,
						   LPCTSTR str, 
						   UINT eFlags) ;

	// Find the size of the text not counting the accelerator characters.
	CSize GetTextExtentNoAmpersands(CDC* pdc, const CString& str ) const;
	static TCHAR ExtractAccelerator(LPCTSTR item);
	static int StripAmpersands(TCHAR* pstart, int len);

	// duplicates the given bitmap (segment)
	static HBITMAP DuplicateBitmap(CWnd *pWnd, HBITMAP hbmBitmap, CSize size=CSize(0,0), CPoint offset=CPoint(0,0));
	// scales one bitmap into another
	static BOOL ScaleBitmap(CWnd *pWnd, HBITMAP dest, CSize destSize, CPoint destOffset, const HBITMAP src, CSize srcSize);
	// use when the dc is already prepared
	static BOOL ScaleBitmap(HDC hdcDest, CSize destSize, CPoint destOffset, const HBITMAP src, CSize srcSize);
	// pastes a clipboard glyph into an image well
	static void PasteGlyph(CWnd *pWnd, HBITMAP hbmWell, int nIndex, CSize sizeImage);

	// accessor functions for load-on-demand items
	HFONT GetButtonFont(BOOL bLarge);
	HFONT GetButtonVerticalFont(BOOL bLarge);

	static HBITMAP LoadSysColorBitmap(UINT id);
	HBITMAP GetMenuArrow(void);
	HBITMAP GetMenuArrowDisabled(void);
	HBITMAP GetMenuArrowVertical(void);
	HBITMAP GetMenuArrowVerticalDisabled(void);

	// gets the bold/nonbold menu font pointer
	CFont *GetMenuFont(BOOL bDefault);
	CFont *GetMenuVerticalFont();
	int GetMenuTextHeight(void);
	int GetMenuVerticalHeight(void);
	BOOL GetLargeMenuButtons(void);
	// flushes the menu fonts during update
	void FlushMenuFonts(void);

	// Bitmap and DIB utility functions
	static WORD GLOBAL_DATA::DibNumColors(VOID FAR * pv); // Pointer to the bitmap info header structure
	static WORD GLOBAL_DATA::PaletteSize(VOID FAR * pv);  // Pointer to the bitmap info header structure
	static HANDLE GLOBAL_DATA::SaveSysColorBitmap(HBITMAP hbmSrc);
	static HANDLE GLOBAL_DATA::CreateLogicalDib(	HBITMAP hbm,        // DDB to copy
											WORD biBits,        // New DIB: bit count: 8, 4, or 0
											HPALETTE hpal);      // New DIB: palette
	static HBITMAP GLOBAL_DATA::LoadSysColorBitmap(LPBITMAPINFOHEADER lpBitmap);

	static BOOL CanChooseImageOnly(APPEARANCEOPTION aoText, APPEARANCEOPTION aoImage);
	static BOOL CanChooseTextOnly(APPEARANCEOPTION aoText, APPEARANCEOPTION aoImage);
	static BOOL CanChooseImageText(APPEARANCEOPTION aoText, APPEARANCEOPTION aoImage);

private:
	CFont *	pFontMenuText;				// font used to draw menu items. In dire emergencies, this might be a ptr to font_normal
	BOOL    bDeleteMenuText;
	CFont *	pFontMenuVertical;				// font used to draw vertical menu bars. In dire emergencies, this might be a ptr to something else
	BOOL    bDeleteMenuVertical;
	CFont *	pFontMenuBold;				// font used to draw bold menu items. In dire emergencies, this might be a ptr to font_large
	BOOL    bDeleteMenuBold;
	int		nMenuTextHeight;			// height of pFontMenuText
	int		nMenuVerticalHeight;		// height of pFontMenuVertical
	BOOL	bLargeMenuButtons;			// toolbar buttons in menus are large 

};

extern GLOBAL_DATA globalData;

// Note: afxData.cxBorder and afxData.cyBorder aren't used anymore
#define CX_BORDER   1
#define CY_BORDER   1

// determine number of elements in an array (not bytes)
#define _countof(array) (sizeof(array)/sizeof(array[0]))

BOOL AFXAPI AfxCustomLogFont(UINT nIDS, LOGFONT* pLogFont);
void AFXAPI AfxDeleteObject(HGDIOBJ* pObject);

/////////////////////////////////////////////////////////////////////////////
