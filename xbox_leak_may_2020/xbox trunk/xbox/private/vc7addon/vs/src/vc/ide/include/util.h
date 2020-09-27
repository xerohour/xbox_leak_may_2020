/////////////////////////////////////////////////////////////////////////////
//	UTIL.H
//		Mostly GDI and file utilities.

#ifndef __UTIL_H__
#define __UTIL_H__

#include "msf.h"
#include "stdfont.h"
#include "slob.h"

#ifndef __UTIL2_H__
// MBS <-> WCS conversion with truncation
#define _wcstombsz(lpszDest, lpszSrc, nMaxBytes) \
	wcstombs(lpszDest, lpszSrc, nMaxBytes), lpszDest[nMaxBytes-1] = 0
#endif	// __UTIL2_H__

//#undef AFX_DATA
//#define AFX_DATA AFX_EXT_DATA
/////////////////////////////////////////////////////////////////////////////
//	CLockDC
//		A CDC which handles locking and unlocking of updates to its window.
//		Used during drag loops.

class CLockDC : public CDC
{
public:
	CLockDC(CWnd* pWnd);
	~CLockDC();

protected:
	BOOL m_bLock;
	HWND m_hWnd;
};

/////////////////////////////////////////////////////////////////////////////
//	CImageWell
//		class provides a convenient way to handle "tiled" bitmaps.

class CImageWell
{
public:
	CImageWell();
	CImageWell(UINT nBitmapID, CSize imageSize);
	~CImageWell();
	void SetBitmap (CBitmap *pBitmap, CSize imageSize);
	
	BOOL Open();
	BOOL IsOpen() { return m_dc.m_hDC != NULL; }
	void Close();
	BOOL Load(UINT nBitmapID, CSize imageSize);
	BOOL LoadSysColor(UINT nBitmapID, CSize imageSize);
	BOOL Attach(HBITMAP hBitmap, CSize imageSize);
	HBITMAP Detach();
	void Unload();
	
	BOOL CalculateMask();
	
	BOOL DrawImage(CDC* pDestDC, CPoint destPoint, UINT nImage, 
		DWORD rop = 0); // rop==0 means image is drawn through a mask

	BOOL DrawImage(CDC* pDestDC, CPoint destPoint, UINT nImage, CSize sizeOverride,
		DWORD rop = 0); // rop==0 means image is drawn through a mask

	BOOL DrawMask(CDC* pDestDC, CPoint destPoint, UINT nImage, DWORD rop = NOTSRCCOPY);

	void GetImageSize(CSize& csImage)
	{
		csImage = m_imageSize;
	}
	
protected:
		
	CSize m_imageSize;
	
	// We use either this pointer or our member bitmap:
	CBitmap *m_pbitmap;

	UINT m_nBitmapID;
	CBitmap m_bitmap;
	
	CDC m_dc;
	CBitmap m_mask;
	CDC m_maskDC;
};

// hard-coded colors to be used instead of COLOR_BTNFACE, _BTNTEXT, etc.
//  - these are for access via MyGetSysColor() - and GetSysBrush()
#define CMP_COLOR_HILITE    21  // RGB(255, 255, 255)
#define CMP_COLOR_LTGRAY    22  // RGB(192, 192, 192) - instead of BtnFace
#define CMP_COLOR_DKGRAY    23  // RGB(128, 128, 128)
#define CMP_COLOR_BLACK     24  // RGB(0, 0, 0) - instead of frame

// - these are for when all you need is a RGB value)
#define CMP_RGB_HILITE      RGB(255, 255, 255)
#define CMP_RGB_LTGRAY      RGB(192, 192, 192)  // instead of BtnFace
#define CMP_RGB_DKGRAY      RGB(128, 128, 128)
#define CMP_RGB_BLACK       RGB(0, 0, 0)        // instead of frame

extern CBrush* GetSysBrush(UINT nSysColor);
extern void ResetSysBrushes();
extern CPen* GetSysPen(UINT nSysColor);
extern COLORREF MyGetSysColor(UINT nSysColor);

#define DSx		0x00660046L		// BitBlt ROPs
#define DSna	0x00220326L

// Paint this button "out," closer to the user.
#ifndef TBBS_EXTRUDED
#define TBBS_EXTRUDED 0x100
#endif

extern void DrawSplitter(CDC *pdc, CRect rect, BOOL bVert);
// Creates a push-button style bitmap
extern HBITMAP CreateButtonBitmap(CWnd* pWnd, UINT nStyle,  LPCRECT lpRect,
	CBitmap* pbmGlyph = NULL, LPCTSTR lpszText = NULL, BOOL bAddArrow = FALSE);
// creates a command bar style bitmap
extern HBITMAP CreateBarBitmap(CWnd* pWnd, UINT nStyle,  LPCRECT lpRect, CSize sizeButton, CSize sizeImage);

extern void ForceRectOnScreen(CRect& rect);
extern void ForceRectBelowTop(CRect& rect);

// For single pixel borders that are not actually window borders.
// Window borders should use system calculated border dimensions.
//
#define CX_BORDER 1
#define CY_BORDER 1
#define CY_TOOLBAR 28	// Toolbar height without borders.

extern BOOL PollForMouseChange(CPoint& pt, BOOL bYield, BOOL& bCancel, BOOL bAllowPaint=FALSE);
extern void FlushKeys();

typedef enum
{
	unknown_platform	= 0,	// Not yet used -- error condition!
	win32x86			= 1,
	win16x86			= 2,
	mac68k				= 3,
	macppc				= 4,
	win32mips			= 5,
	win32alpha			= 6,
	win32ppc			= 7,
    unsupported         = 8,
    generic             = 9,
    vistest             = 10,
	win64merced			= 20,
	win64alpha			= 21,
	xbox				= 30,
} uniq_platform;

extern UINT GetExePlatform(LPCTSTR szExeName);

#include <mmsystem.h>
extern BOOL PlayDevSound(UINT idsEvent, DWORD dwFlags = SND_ASYNC | SND_NODEFAULT);
extern BOOL PlayDevSound(const char* szEvent, DWORD dwFlags = SND_ASYNC | SND_NODEFAULT);
extern BOOL RegisterDevSoundEvent(UINT idsEventName, UINT idsFriendlyName);
extern BOOL RegisterDevSoundEvent(const char* szEventName, const char* szFriendlyName);

BOOL InEmergencyState();

// TRUE if a CCmdui is any kind of context menu
extern BOOL IsContextMenu(CCmdUI* pCmdUI);
extern BOOL IsMenu(CCmdUI* pCmdUI);
// Enable the menu if it has any visible items
extern void UpdateMenuCommand(CCmdUI* pCmdUI);

#ifdef _DEBUG
BOOL GetLogUndoFlag();
#endif

///////////////////////////////////////////////////////////////////////////////
//	CFileSaver
//		This is a little helper class for writing things to temporary files
//		and giving them the correct name after the save suceeds.

class CFileSaver
{
public:
	CFileSaver(const TCHAR* szFileName); // setup and create temp names
	~CFileSaver();                      // make sure temp files are gone
	BOOL CanSave() const;				// checks for R/O
	BOOL MakeWriteable() const;			// forces status of file to NOT R/O
	const TCHAR* GetSafeName() const     // return name of file to create
		{ return m_strTempName; }
	const TCHAR* GetRealName() const		// return name of final file
		{ return m_strName; }
	BOOL Finish();                      // rename new file as original

private:
	CString m_strName;
	CString m_strBackupName;
	CString m_strTempName;
	
	static const TCHAR BASED_CODE c_szAps [];
};

/////////////////////////////////////////////////////////////////////////////
//	CInitFile
class CInitFile : public CMemFile
{
	DECLARE_DYNAMIC(CInitFile)

private:
	using CMemFile::Open;

public:
	enum INIT_DATATYPE { dataNil, dataReg, dataResource, dataStorage, dataMSF };

// Constructors
public:
	CInitFile();
	~CInitFile();
	
// Operations
public:
	BOOL Open(LPCSTR szSection, LPCSTR szKey, UINT nOpenFlags,
		INIT_DATATYPE type = dataReg);
	BOOL Open(LPSTORAGE pStorage, CString strStream, UINT nOpenFlags);
	BOOL Open(MSF *pmsf, WORD sn, UINT nOpenFlags);

	void SetBuffer(HGLOBAL hglobMem, UINT nCountBytes = -1,
		INIT_DATATYPE type = dataReg);
	HGLOBAL GetInitData();

	virtual void Close();	  // throw (CFileException);
	virtual void Abort();

protected:
	BOOL LoadFromSource();
	BOOL WriteToDest();

// Attributes
protected:
	BOOL m_bWrite;
	INIT_DATATYPE m_type;

	MSF *m_pmsf;
	WORD m_sn;

	LPCSTR m_lpszSection;
	LPCSTR m_lpszKey;

	HGLOBAL m_hglobUserMem;

	CString  m_strStream;
	LPSTREAM m_lpStream;

#ifdef _DEBUG
	virtual void AssertValid() const;
#endif
};
///////////////////////////////////////////////////////////////////////////////
//	CStateSaver

#define szWorkspaceSection _T("Workspace")
#define chMonikerSep _T('\n')

class CStateSaver
{
public:
	CStateSaver(BOOL bSaving);
	~CStateSaver();

	BOOL OpenMSF(LPCTSTR szFilePath, BOOL bForceWriteable);
	BOOL OpenStorage(LPCTSTR szFilePath);
	BOOL OpenStream(CInitFile& fileInit, CString strStream);
	void CloseStorage();
	BOOL Finish();

    const TCHAR * GetFileName() const { return (const TCHAR *)m_strFileName; }

	BOOL IsStoring() const { return m_bSaving; }
	BOOL IsLoading() const { return !m_bSaving; }
	BOOL IsFileBased() const { return (m_lpStorage != NULL || m_pMSF != NULL); }
	BOOL OpenFile(CInitFile& fileInit, SN nStream, LPCSTR szSection, LPCSTR szKey);

// Attributes
protected:
	BOOL m_bSaving;
	CFileSaver* m_pSaver;
    CString m_strFileName;
	LPSTORAGE m_lpStorage;

	MSF *m_pMSF;
};

/////////////////////////////////////////////////////////////////////////////
//	COLEStorage

class COLEStorage : public CObject
{
// Construction
public:
	COLEStorage(BOOL bReadOnly = TRUE);
	~COLEStorage();

// Attributes
public:
	LPCTSTR GetName();

// Operations
public:
	void Open(LPCTSTR lpszPath);
	void Create(LPCTSTR lpszPath);

// Sub-storage & stream operations
public:
	void OpenStream(LPCTSTR lpszName, COleStreamFile* pStream);
	void CreateStream(LPCTSTR lpszName, COleStreamFile* pStream);

	void OpenStorage(LPCTSTR lpszName, COLEStorage* pStorage);
	void CreateStorage(LPCTSTR lpszName, COLEStorage* pStorage);

// Implementation
protected:
	void* m_pStorage;
	DWORD m_grfMode;
};


/////////////////////////////////////////////////////////////////////////////
//	CDefProvidedNode
//		this node type is added to the list of default node provider package
//		nodes which the project window asks for if there is no user-preference
//		information	for the project which is being opened/created.
//	REVIEW(PatBr):  This should be turned into an interface, and moved
//					into prjapi.h along with the package workspace interface.

typedef BOOL (CALLBACK *FILESAVECALLBACK)();

class CDefProvidedNode : public CObject
{
public:
	CDefProvidedNode() {}
	virtual ~CDefProvidedNode() {}

	enum SORT_TYPE { content_sort, alpha_sort };

	// packages must subclass the CDefProvidedNode class
	// in order to have meaningful node types. they should provide:

	// whether the default node should reside on its
	// own pane in the project window, and...
	virtual BOOL HasOwnPane() { return(TRUE); }

	// ...if the node has its own pane, whether the pane is 
	// workspace-independent (e.g., the HelpView pane is), and...
	virtual BOOL IsPaneIndependent() { return(FALSE); }

	// .. the type of sorting desired on the top-level nodes in the pane, and...
	virtual SORT_TYPE GetSortType() { return(alpha_sort); }

	// .. the help identifier used when user asks for help with this pane active...
	virtual UINT GetHelpID() { return((UINT)-1); }

	// ...a string to use as the name for the pane, and...
	virtual const TCHAR *GetPaneName() = 0;

	// ...a function which can be called to create the node,
	// which returns a pointer to a slob which will then be
	// inserted into the pane (this can be a CMultiSlob if the
	// node provider wants to insert more than one root node), and...
	virtual CSlob *CreateNode() = 0;

	// ...a function which returns the priority index of the pane.
	// currently: Classes=100, Resources=300, Build=500, Help=700, and...
	virtual int GetPriorityIndex() = 0;

	// ...a function which returns (if appropriate) a file-save callback
	// which the project window can use for command routing/enabling, and...
	virtual FILESAVECALLBACK GetFileSaveCallback() { return(NULL); }

	// ... a function to initialize the glyph bitmap for the pane.  the
	// bitmap should contain three bitmaps (12x12, 9x9, 4x4) in a 36x12
	// bitmap which can be used to initialize a CImageWell.  The three
	// different sizes are for varying sizes of scrollbars (16x16 on
	// WindowsNT but user-defined (default:13x13) on Windows'95).
	// caller of this function will perform DeleteObject() on HBITMAP.
	virtual HBITMAP GetPaneGlyph() = 0;

	//ugly hack.  The node is the only workspace-related thing that the 
	//res package seems to subclass.  So when we're initing the workspace 
	//window, the only way to know if we're dealing with ResourceView 
	//(which we don't want to expand for performance reasons), is to
	//ask the node.  So the res pkg overrides this to return FALSE.
	//All other packages should not need to override this.
	virtual	BOOL ShouldExpand() { return TRUE; }

};

// convert the windows ansi code page to the iso code page
// used by the htmx and htm packages for the code page
// meta data html tag
BOOL AnsiCodePageToIsoCodePage(/*[in]*/ LPCTSTR szAnsi, /*[out]*/ CString *pstrIso);

/////////////////////////////////////////////////////////////////////////////

void SetActiveView(HWND hwnd);

/////////////////////////////////////////////////////////////////////////////
//      Global dimensions
namespace MSDevShell
{
typedef struct tagSCREEN_METRIC
{
    int cxBorder, cyBorder;
    int cxFrame, cyFrame;
    int cxToolBorder, cyToolBorder;
	int cxToolCaption, cyToolCaption;
	int cxToolCapBorder, cyToolCapBorder;
	int cxToolCapGap, cyToolCapGap;			// gap between tool caption buttons
	int cxToolCapButton, cyToolCapButton;
    int nSplitter;
} SCREEN_METRIC;

extern SCREEN_METRIC g_mt;

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
} // Namespace Shell

//#undef AFX_DATA
//#define AFX_DATA NEAR

#endif  // __UTIL_H__
