// fcdialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include <wingdi.h>
#include "main.h"

#include "fcdialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFontColorDlg dialog

IMPLEMENT_DYNAMIC (CFontColorDlg, CDlgTab)

#pragma warning (disable : 4355 )

CFontColorDlg::CFontColorDlg(CWnd* pParent /*=NULL*/)
	: CDlgTab(CFontColorDlg::IDD, IDS_FONTSANDCOLORS), m_state(this)
{
	m_idsSample = 0;
	m_nOrder = 120;

	//{{AFX_DATA_INIT(CFontColorDlg)
	//m_bProportional = FALSE;
	//}}AFX_DATA_INIT
}
#pragma warning (default: 4355 )


void CFontColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDlgTab::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontColorDlg)
	DDX_Control(pDX, IDC_SAMPLE, m_stcSample);
	DDX_Control(pDX, IDC_LIST_WINDOWS, m_lstWindows);
	DDX_Control(pDX, IDC_COMBO_FONT_SIZE, m_cmbFontSize);
	DDX_Control(pDX, IDC_COMBO_FONT_NAME, m_cmbFontName);
	DDX_Control(pDX, IDC_LIST_ELEMENTS, m_lstElements);
	DDX_Control(pDX, IDC_COMBO_BACK_COLOR, m_cmbBackColor);
	DDX_Control(pDX, IDC_COMBO_FORE_COLOR, m_cmbForeColor);
	//DDX_Check(pDX, IDC_CHECK_PROPORTIONAL, m_bProportional);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFontColorDlg, CDlgTab)
	//{{AFX_MSG_MAP(CFontColorDlg)
	ON_CBN_SELCHANGE(IDC_LIST_WINDOWS, OnChangeWindow)
	ON_CBN_SELCHANGE(IDC_COMBO_FONT_NAME, OnChangeFont)
	ON_CBN_SELCHANGE(IDC_COMBO_FORE_COLOR, OnChangeForeColor)
	ON_CBN_SELCHANGE(IDC_COMBO_FONT_SIZE, OnChangeSize)
	ON_LBN_SELCHANGE(IDC_LIST_ELEMENTS, OnChangeElement)
	ON_CBN_SELCHANGE(IDC_COMBO_BACK_COLOR, OnChangeBackColor)
	ON_CBN_KILLFOCUS(IDC_COMBO_FONT_SIZE, OnKillFocusSize)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOLORCHANGE()
	//ON_BN_CLICKED(IDC_CHECK_PROPORTIONAL, OnCheckProportional)
	ON_BN_CLICKED(IDC_BTN_DEFAULTS, OnRestoreDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


///// UpdateSampleFont - Redraw the sample font with current LOGFONT
//
///
void CFontColorDlg::UpdateSampleFont ()
{
	UINT idsSampleNew;

	m_SampleFont.DeleteObject();

	const LOGFONT * pLogFont = m_state.GetCurrentLogFont();
	if (!pLogFont) {
		pLogFont = GetStdLogfont (font_Fixed);
		idsSampleNew = IDS_MULTIPLE_FONTS;
		}
	else
	{
		switch (pLogFont->lfCharSet)
		{
			case TURKISH_CHARSET:
				idsSampleNew = IDS_SAMPLE_TURKISH;
				break;

			case HEBREW_CHARSET:
				idsSampleNew = IDS_SAMPLE_HEBREW;
				break;

			default:
				idsSampleNew = IDS_SAMPLE_LINE;
				break;
		}
	}

	if (m_idsSample != idsSampleNew)
	{
		m_idsSample = idsSampleNew;
		m_strSample.LoadString (m_idsSample);
	}

	m_SampleFont.CreateFontIndirect (pLogFont);

	CRect rect;

	m_stcSample.GetWindowRect (&rect);
	ScreenToClient (&rect);
	InvalidateRect (rect);
}


/////////////////////////////////////////////////////////////////////////////
// CFontColorDlg message handlers

BOOL CFontColorDlg::OnInitDialog() 
{
	CDlgTab::OnInitDialog();

	// Rank bogosity!  cmbFontName should get a MeasureItem call, but it doesn't.
	
	int nHeight = m_cmbFontSize.GetItemHeight(-1);
	if (nHeight != CB_ERR)
		m_cmbFontName.SetItemHeight (-1, nHeight);

	CDC * pDC = GetDC();
	m_nPixPerInchY = pDC->GetDeviceCaps(LOGPIXELSY);
	ReleaseDC (pDC);
	
	m_stcSample.GetWindowRect (&m_rcSample);
	ScreenToClient (&m_rcSample);

	m_cmbForeColor.SetStandardColors();
	m_cmbBackColor.SetStandardColors();

	m_state.InitDialog();	// Fill data dependent controls, notify handlers of change.

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


///// OnRestoreDefaults - Restore default colors and fonts
//
//	Restores the default fonts and color settings for the currently
//	selected window or window group.
//
///
void CFontColorDlg::OnRestoreDefaults ()
{
	m_state.RestoreDefaults();
}


///// OnChangeWindow - User selected a different Window
//
//	Change list of elements.  Propogate change to fonts, etc.
//
///
void CFontColorDlg::OnChangeWindow() 
{
	m_state.SetCurrentWindow (m_lstWindows.GetCurSel());
}


///// OnChangeFont - User or code has changed currently selected font name
//
//	When the font name changes, the list of available sizes must also change.
//
/// 
void CFontColorDlg::OnChangeFont() 
{
	RegenSizes (TRUE);
}


void CFontColorDlg::RegenSizes (BOOL bUpdateState)
{
CString strFaceName;
int	ptSize;
CDC * pDC = GetDC();
BOOL bTrueType = FALSE;

	ASSERT (pDC);

	if (m_cmbFontName.GetCurSel() == -1)
		return;

	m_cmbFontSize.EnableWindow (TRUE);

	// We assume that we wouldn't be here unless all windows in the
	// current set were using the same font.
	FMT_WINDOW * pWinCur = m_state.GetWinCur (FALSE, TRUE);

	// Pick up the new font name
	m_cmbFontName.GetLBText (m_cmbFontName.GetCurSel(), strFaceName);

	// Pick up the current font size
	//if (CB_ERR == (ptSize = m_cmbFontSize.GetSize()))
	ptSize = pWinCur -> nFontSize;

	// Blow off current size list, then regenerate it for the new font.
	m_cmbFontSize.ResetContent ();

	switch (EnumFontFamilies( pDC->m_hDC, strFaceName, (FONTENUMPROC)SizeCallBack , (LPARAM)this))
	{
		case TRUETYPE_FONTTYPE:
			m_cmbFontSize.SetStandardSizes();
			bTrueType = TRUE;
			break;

		case RASTER_FONTTYPE:
			break;

		default:
			ASSERT (FALSE);
	}

	// Update the dialog state
	if (bUpdateState)
		m_state.SetCurrentFont (strFaceName, ptSize, bTrueType);

	UpdateSampleFont();

	ReleaseDC (pDC);
}

///// SizeCallBack - EnumFontFamilies callback to fill size combo
//
///
int CALLBACK CFontColorDlg::SizeCallBack (
CONST ENUMLOGFONT * pEnumLogFont,
CONST NEWTEXTMETRIC *  pTextMetric,
int fontType,
LPARAM lParam
) {
CFontColorDlg * const pThis = (CFontColorDlg * const)lParam;

	ASSERT (pThis->IsKindOf (RUNTIME_CLASS (CFontColorDlg)));
	
	if (fontType & TRUETYPE_FONTTYPE) {
		return TRUETYPE_FONTTYPE;
		}
	else
	{
		UINT nCharSet = theApp.GetCharSet();
		if ((nCharSet == DEFAULT_CHARSET || nCharSet == pTextMetric->tmCharSet || pTextMetric->tmCharSet == OEM_CHARSET))
		{
			pThis->m_cmbFontSize.AddSize (MulDiv(pTextMetric->tmHeight - pTextMetric->tmInternalLeading,72,pThis->m_nPixPerInchY));
		}
		return RASTER_FONTTYPE;
	}
}


///// OnChangeForeColor - Foreground color has been changed
//
//	Propogates change to underlying data
///
void CFontColorDlg::OnChangeForeColor() 
{
	m_state.SetForeColor (m_cmbForeColor.GetItemData (m_cmbForeColor.GetCurSel()));
	UpdateSampleFont();
}


///// OnChangeBackColor - Background color has been changed
//
//	Propogates change to underlying data
///
void CFontColorDlg::OnChangeBackColor() 
{
	m_state.SetBackColor (m_cmbBackColor.GetItemData (m_cmbBackColor.GetCurSel()));
	UpdateSampleFont();
}


///// OnChangeSize - Font size has been changed
//
//	Propogates change to underlying data
///
void CFontColorDlg::OnChangeSize() 
{
CString strText;

	int cPointsNew = m_cmbFontSize.GetSize();

	//int cPointsNew = m_cmbFontSize.GetItemData ( m_cmbFontSize.GetCurSel() );
	m_state.SetCurrentFontSize (cPointsNew);
	m_cmbFontSize.SetPreferredSize (cPointsNew);
	UpdateSampleFont ();
}


///// OnKillFocusSize - User typed in a new size
//
///
void CFontColorDlg::OnKillFocusSize ()
{
CString strText;

	if (m_cmbFontSize.IsWindowEnabled())
	{
		int ptEditField = m_cmbFontSize.GetSize ();
		int ptCur = m_state.GetWinCur (FALSE, TRUE)->nFontSize;
		//int ptListBox = m_cmbFontSize.GetItemData ( m_cmbFontSize.GetCurSel() );

		if (ptEditField != ptCur)
		{
			m_cmbFontSize.SetPreferredSize (ptEditField);

			BOOL bTrueType = m_cmbFontName.GetItemData (m_cmbFontName.GetCurSel()) & TRUETYPE_FONTTYPE;
			m_cmbFontSize.SetToSize (ptEditField, !bTrueType);

			OnChangeSize ();
		}
	}
}


///// OnChangeElement - Current element has been changed
//
//	Propogates change to underlying data
///
void CFontColorDlg::OnChangeElement() 
{
	m_state.SetCurrentElement(m_lstElements.GetCurSel());

	UpdateSampleFont();
}


///// CommitTab - User has chosen 'OK', or has switched to another tab
//
//	Copy user changes back to the CFormatInfo belonging to the packages.
///
void CFontColorDlg::CommitTab()
{
	OnKillFocusSize ();
	m_state.Commit ();
}


///// OnPaint() - Handle painting of Sample Box
//
///
void CFontColorDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (dc.SelectObject (&m_SampleFont))
	{

		dc.SetTextColor (m_state.GetForeColor ());
		dc.SetBkColor (m_state.GetBackColor ());
		dc.SetBkMode (TRANSPARENT);		// Fixes white background color.

		CSize extSample = dc.GetOutputTextExtent (m_strSample, m_strSample.GetLength());

		CPoint ptSample (	m_rcSample.left + (m_rcSample.right - m_rcSample.left) / 2 - extSample.cx / 2,
							m_rcSample.top + (m_rcSample.bottom - m_rcSample.top) / 2 - extSample.cy / 2);

		dc.ExtTextOut (ptSample.x, ptSample.y, ETO_CLIPPED, m_rcSample, m_strSample, m_strSample.GetLength(), NULL);
		// Do not call CDlgTab::OnPaint() for painting messages
	}
}


///// OnEraseBkgnd - Fills the sample box with selected background color
//
///
BOOL CFontColorDlg::OnEraseBkgnd(CDC * pDC)
{
CBrush brush;

	CDlgTab::OnEraseBkgnd (pDC);

	brush.CreateSolidBrush (m_state.GetBackColor ());

	pDC->FillRect (m_rcSample, &brush);

	return TRUE;
}


///// OnSysColorChange - The operating system's colors have been changed
//
//	Fixes up any system-auto colors to use the new system colors.
///
void CFontColorDlg::OnSysColorChange ()
{
	m_state.UpdateAllAutoColors ();

	UpdateSampleFont ();

}




/////////////////////////////////////////////////////////////////////////////
// CColorCombo
COLORREF		CColorCombo::m_rgStandardColors[16] =
{
	RGB (0, 0, 0),
	RGB (0xFF, 0xFF, 0xFF),
	RGB (0x80, 0, 0),
	RGB (0, 0x80, 0),
	RGB (0x80, 0x80, 0),
	RGB (0, 0, 0x80),
	RGB (0x80,0, 0x80),
	RGB (0, 0x80, 0x80),
	RGB (0xC0, 0xC0, 0xC0),
	RGB (0x80, 0x80, 0x80),
	RGB (0xFF, 0, 0),
	RGB (0, 0xFF, 0),
	RGB (0xFF, 0xFF, 0),
	RGB (0, 0, 0xFF),
	RGB (0xFF, 0, 0xFF),
	RGB (0, 0xFF, 0xFF)
};

#define AUTO_INDEX	(sizeof (m_rgStandardColors) / sizeof (m_rgStandardColors[0]))


///// m_brushSwatchFrame - Brush to paint rectangle around color swatches
//
///
CBrush CColorCombo::m_brushSwatchFrame;


///// Constructor
//
//	Create SwatchFrame brush as inverse of window background
//	Load "Automatic" string
///
CColorCombo::CColorCombo() 
{
	if (m_brushSwatchFrame.m_hObject == NULL)
		m_brushSwatchFrame.CreateSolidBrush (~GetSysColor (COLOR_WINDOW) & 0xFFFFFF);

	CString strAuto;
	VERIFY (strAuto.LoadString (IDS_AUTOMATIC));
	m_szAutomatic = new _TCHAR [strAuto.GetLength() + 1];
	_tcscpy (m_szAutomatic, strAuto);
}


///// Destructor
//
//	Blow away auto string and frame brush
///
CColorCombo::~CColorCombo()
{
	ASSERT (m_szAutomatic != NULL);
	delete [] m_szAutomatic;

	// Note that this deletes the brush whenever a CColorCombo is deleted,
	// which isn't the correct algorithm -- we need to keep a brush around
	// until they've ALL been deleted.  However, this works for the limited
	// cases we need right now.
	if (m_brushSwatchFrame.m_hObject != NULL)
	{
		m_brushSwatchFrame.DeleteObject ();
		ASSERT(m_brushSwatchFrame.m_hObject == NULL);
	}
}


///// AddColor - Adds an RGB value to end of list
//
///
int CColorCombo::AddColor ( COLORREF rgb )
{
	return AddString ((LPCSTR) rgb);
}


///// SetStandardColors - Add the "standard colors" to the combo box
//
///
void CColorCombo::SetStandardColors ()
{
	for (int i = 0; i < sizeof(m_rgStandardColors) / sizeof (m_rgStandardColors[0]); i++) {
		// FUTURE :  Isn't there a better way to do this?
		int ix = AddString ( (LPCTSTR) m_rgStandardColors[i]);
		if (ix != CB_ERR)
			SetItemData (ix, m_rgStandardColors[i]);
		}

	VERIFY (AUTO_INDEX == AddString ((LPCTSTR)CColorCombo::rgbAuto));
	VERIFY (0 <= SetItemData (AUTO_INDEX, CColorCombo::rgbAuto));
}


BEGIN_MESSAGE_MAP(CColorCombo, CComboBox)
	//{{AFX_MSG_MAP(CColorCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CColorCombo message handlers


///// DrawItem - draws color items whenever asked
//
///
void CColorCombo::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	DrawEntireItem (pDC, lpDrawItemStruct);

	if (lpDrawItemStruct->itemAction & ODA_SELECT)
		DrawSelectionChange (pDC, lpDrawItemStruct);

	if (lpDrawItemStruct->itemAction & ODA_FOCUS)
		DrawFocusChange (pDC, lpDrawItemStruct);
		
}


///// DrawEntireItem - Callback to draw individual color swatches
//
///
void CColorCombo::DrawEntireItem (
CDC * pDC,
LPDRAWITEMSTRUCT lpDrawItemStruct
) {
	if ((int)lpDrawItemStruct->itemID < 0)
		DrawFocusChange (pDC, lpDrawItemStruct);
	else {
		CRect rect (lpDrawItemStruct->rcItem);
		// We need to shrink the items a little to keep them from running
		// together
		rect.InflateRect ( -2, -2 );

		if ((int)lpDrawItemStruct->itemID == AUTO_INDEX) {
			// Center text
			CSize sizeAuto (pDC->GetTextExtent (m_szAutomatic, _tcslen (m_szAutomatic)));
			pDC->TextOut (rect.left + ((rect.Width() - sizeAuto.cx) / 2), rect.top, m_szAutomatic);
			}
		else {
			CBrush brushItem (lpDrawItemStruct->itemData);
			pDC->FillRect (rect, &brushItem);
			}
		// The frame is necessary to delineate the swatch that is the same as
		// the background color.
		pDC->FrameRect (rect, &m_brushSwatchFrame);
		}

}


///// DrawSelectionChange
//
///
void CColorCombo::DrawSelectionChange (
CDC * pDC,
LPDRAWITEMSTRUCT lpDrawItemStruct
) {
	CBrush brush (lpDrawItemStruct->itemState & ODS_SELECTED ? pDC->GetTextColor() : pDC->GetBkColor());

	pDC->FrameRect (&lpDrawItemStruct->rcItem, &brush);
}


///// DrawFocusChange
//
///
void CColorCombo::DrawFocusChange (
CDC * pDC,
LPDRAWITEMSTRUCT lpDrawItemStruct
) {
 	pDC->DrawFocusRect (&lpDrawItemStruct->rcItem);
}



/////////////////////////////////////////////////////////////////////////////
// CSizeCombo

///// m_rgTTSizes - Standard sizes
//
//	The list of sizes is taken from PSS note Q99672
//
///
DWORD	CSizeCombo::m_rgTTSizes[] =
{
	8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72
};


CSizeCombo::CSizeCombo()
{
}

CSizeCombo::~CSizeCombo()
{
	if (::IsWindow (m_hWnd))
		EnableWindow (FALSE);	// Bogosity to work around spurious KillFocus message
}


///// AddSize - Add a size, making sure it is unique and in order
//
///
int CSizeCombo::AddSize ( int cPoints )
{
int nInsert;
int cItems = GetCount();

	ASSERT (cPoints);

	for (nInsert = 0; nInsert < cItems; nInsert++) {
		DWORD itemData;
		VERIFY ((itemData = GetItemData (nInsert)) != CB_ERR);

		if (itemData == (DWORD)cPoints)
			return nInsert;
				
		if (itemData > (DWORD)cPoints)
			break;
		}

	_TCHAR buf[20];

	_itoa (cPoints, buf, 10);

	if (CB_ERR != InsertString (nInsert, buf))
	{
		SetItemData (nInsert, cPoints);
		return nInsert;
	}

	return -1;
}


///// SetStandardSizes - Add the list of standard font sizes
//
//	TrueType fonts can be scaled to any size.  To make things easier on the user, we
//	provide a standard set of sizes.
//
///
void CSizeCombo::SetStandardSizes ()
{
	for (int i = 0; i < sizeof(m_rgTTSizes) / sizeof (m_rgTTSizes[0]); i++)
		AddSize (m_rgTTSizes[i]);
}

 
///// GetSize - Return the point size at the given index
//
///
DWORD	CSizeCombo::GetSize ( int ixSize /* = -1 */ )
{
	if (ixSize == -1)	// Return current size
	{
		int iCurSel = GetCurSel ();
		if (iCurSel != CB_ERR)
			return GetItemData (iCurSel);
		else
		{
			CString strSize;
			GetWindowText (strSize);
			return atol (strSize);
		}
	}
	else
		return GetItemData (ixSize);
}


///// SetToSize - Set current selection to the given size
//
//	Sets the current size selection based on the user's preferred size,
//	the sizes available, and the passed-in new size.
//
//		1. Try to match the preferred size exactly.
//		2. If #1 fails:
//			bMatchClosest == True -> Match the new size as closely as possible (larger size wins).
//			bMatchClosest == False -> Match new size exactly, or add it to list.
//
//	Returns the index of the closest match.
//
//	NOTE: The resulting behaviour depends on SetPreferredSize() being called before this function
//	whenever appropriate.
///
int	CSizeCombo::SetToSize (int ptNewSize, BOOL bMatchClosest)
{
	int nItems = GetCount ();
	int iSelBestMatch;
	int dBestMatchDiff = 0x7FFF;

	if (!bMatchClosest)
	{
		if (ptNewSize < 6)
			ptNewSize = 6;
		else
		if (ptNewSize > 72)
			ptNewSize = 72;
	}

	for (int ptSearchSize = m_ptPreferredSize, cLoops = 0;
			cLoops < 2;
			ptSearchSize = ptNewSize, cLoops++ )
	{
		for (int iItem = 0; iItem < nItems; iItem++)
		{
			int dMatchDiff = GetItemData (iItem) - ptSearchSize;
			// If there are two sizes in the list that are the same distance from
			// the target size (i.e. one smaller and one larger), we want to
			// consistently pick one or the other.
			// The expression below picks the larger.
			if (dMatchDiff > 0
				? dMatchDiff <= dBestMatchDiff
				: abs(dMatchDiff) < dBestMatchDiff)
			{
				iSelBestMatch = iItem;
				dBestMatchDiff = abs(dMatchDiff);
				if (dBestMatchDiff == 0)
				{
					SetCurSel (iSelBestMatch);
					return iSelBestMatch;
				}
			}
		}
	}


	// No exact match
	if (bMatchClosest)
	{
		SetCurSel (iSelBestMatch);
		return iSelBestMatch;
	}
	else
	{
		return SetCurSel (AddSize (ptNewSize));
	}
}


BEGIN_MESSAGE_MAP(CSizeCombo, CComboBox)
	//{{AFX_MSG_MAP(CSizeCombo)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CSizeCombo message handlers


/////////////////////////////////////////////////////////////////////////////
// CFontNameCombo

IMPLEMENT_DYNAMIC (CFontNameCombo, CComboBox)

CFontNameCombo::CFontNameCombo()
{
}

CFontNameCombo::~CFontNameCombo()
{

	for (int i = 0; i < m_rgFontInfo.GetSize (); i++)
	{
		delete [] ((FontInfo *)m_rgFontInfo[i])->szName;
		delete m_rgFontInfo[i];
	}
}


///// AddEntry - Add font name string and associated data
//
///
void CFontNameCombo::AddEntry (
const CString& strName,
DWORD fontType
) {
	FontInfo * pFI = new FontInfo;
	pFI->szName = NewString (strName);
	pFI->dwFontType = fontType;

	m_rgFontInfo.Add (pFI);

	AddString ((LPCTSTR)pFI);
}

///// Enumerate and Callbacks - Get font list from system and fill combo
//
//	Enumerate works with the MainCallBack() to get the list of font face names.
//
///
void CFontNameCombo::Enumerate ( BOOL bProportional )
{
	CDC * pDC = GetDC();

	ASSERT (pDC);

	CString strFontName;
	int 	iCurSelFontName = GetCurSel();

	if (CB_ERR != iCurSelFontName)
		GetLBText (iCurSelFontName, strFontName);

	ResetContent();

	m_bProportional = FALSE;
	EnumFontFamilies ( pDC->m_hDC, NULL, (FONTENUMPROC)FillCallBack , (LPARAM)this );
	if (bProportional)
	{
		m_bProportional = TRUE;
		EnumFontFamilies ( pDC->m_hDC, NULL, (FONTENUMPROC)FillCallBack , (LPARAM)this );
	}

	// This code tries to maintain the same font name in the font name field
	// across a font enumeration.
	if (iCurSelFontName != CB_ERR)
		iCurSelFontName = FindStringExact (-1, strFontName);

	SetCurSel (iCurSelFontName == CB_ERR ? 0 : iCurSelFontName);

	ReleaseDC (pDC);
}

int CALLBACK CFontNameCombo::FillCallBack(
CONST ENUMLOGFONT	* pEnumLogFont,
CONST NEWTEXTMETRIC	* pTextMetric,
int fontType,
LPARAM lParam
) {
CFontNameCombo * const pThis = (CFontNameCombo * const)lParam;

	ASSERT (pThis->IsKindOf (RUNTIME_CLASS (CFontNameCombo)));

	UINT nCharSet = theApp.GetCharSet();
	if ((nCharSet == DEFAULT_CHARSET || nCharSet == ANSI_CHARSET || nCharSet == pTextMetric->tmCharSet || pTextMetric->tmCharSet == OEM_CHARSET) && pEnumLogFont->elfLogFont.lfFaceName[0] != '@')
	{
		// Note - The meaning of TMPF_FIXED_PITCH is the opposite of what the name implies.
		if (	((pTextMetric->tmPitchAndFamily & TMPF_FIXED_PITCH) && pThis->m_bProportional)
			||	(!(pTextMetric->tmPitchAndFamily & TMPF_FIXED_PITCH) && !pThis->m_bProportional) )
		{
			pThis->AddEntry (pEnumLogFont->elfLogFont.lfFaceName, MAKELONG(fontType, pEnumLogFont->elfLogFont.lfPitchAndFamily));
		}
	}

	return TRUE;
}

DWORD CFontNameCombo::GetItemData (int index) const
{
	ASSERT (index > -1 && index < m_rgFontInfo.GetSize ());

	// Initialize pFI to NULL so that it will crash in debug mode if it is not set.
	FontInfo * pFI = NULL;
	CComboBox::GetLBText (index, (char *)&pFI);

	return pFI->dwFontType;
}

int CFontNameCombo::GetLBText (int index, LPTSTR szText) const
{
	ASSERT (index > -1 && index < m_rgFontInfo.GetSize ());

	FontInfo * pFI = NULL;
	CComboBox::GetLBText (index, (char *)&pFI);

	_tcscpy (szText, pFI->szName);
	return _tcslen (szText);
}

void CFontNameCombo::GetLBText (int index, CString & strText) const
{
	ASSERT (index > -1 && index < m_rgFontInfo.GetSize ());

	FontInfo * pFI = NULL;
	CComboBox::GetLBText (index, (char *)&pFI);

	strText = pFI->szName;
}

int CFontNameCombo::FindStringExact (int index, LPCTSTR szFind) const
{
int cMac = m_rgFontInfo.GetSize();

	for (int i = 0; i < cMac; i++)
	{
		if (!_tcsicmp(((FontInfo *)m_rgFontInfo[i])->szName, szFind))
			return CComboBox::FindStringExact (index, (const char *)m_rgFontInfo[i]);
	}

	return CB_ERR;
}


BEGIN_MESSAGE_MAP(CFontNameCombo, CComboBox)
	//{{AFX_MSG_MAP(CFontNameCombo)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFontNameCombo message handlers


///// DrawItem - Draws font name, possibly including TrueType logo
//
///
void CFontNameCombo::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	
	if (lpDrawItemStruct->itemID != -1L &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) != 0)
	{
		CRect rect(lpDrawItemStruct->rcItem);
		CRect rectText (rect);
		COLORREF rgbText = 0x80000000;	// Value is a flag for "color was changed"
		COLORREF rgbBack;

		m_bmpTrueType.AdjustDrawRect (rectText);

		if (lpDrawItemStruct->itemState & ODS_SELECTED)
		{	// Show item as selected with standard hilite colors.
			rgbText = pDC->SetTextColor (GetSysColor(COLOR_HIGHLIGHTTEXT));
			rgbBack = pDC->SetBkColor (GetSysColor (COLOR_HIGHLIGHT));
		}

		// Draw font name, plus background for entire item.
		CFont * pFontPrev = NULL;
		CFont font;

		LOGFONT lf;

		ASSERT (::GetCurrentObject (pDC->m_hDC, OBJ_FONT));
		VERIFY (::GetObject (::GetCurrentObject (pDC->m_hDC, OBJ_FONT), sizeof lf, &lf));

		if (lf.lfWeight == FW_NORMAL)
			if (HIWORD (GetItemData (lpDrawItemStruct->itemID)) & FIXED_PITCH)
			{
				lf.lfWeight = FW_BOLD;
				VERIFY (font.CreateFontIndirect (&lf));
				VERIFY (pFontPrev = pDC->SelectObject (&font));
			}
			else
				;
		else
			if (HIWORD (GetItemData (lpDrawItemStruct->itemID)) & VARIABLE_PITCH)
			{
				lf.lfWeight = FW_NORMAL;
				VERIFY (font.CreateFontIndirect (&lf));
				VERIFY (pFontPrev = pDC->SelectObject (&font));
			}
			else
				;
		CString strItem;
		GetLBText (lpDrawItemStruct->itemID, strItem);
		pDC->ExtTextOut (rectText.left, rectText.top, ETO_CLIPPED|ETO_OPAQUE, rect, strItem, strItem.GetLength(), NULL);

		if (pFontPrev)
		{
			VERIFY (pDC->SelectObject (pFontPrev) == &font);
			font.DeleteObject ();
		}

		// Now it's safe to draw this.
		if (GetItemData (lpDrawItemStruct->itemID) & TRUETYPE_FONTTYPE)	
		{
			BOOL b = m_bmpTrueType.DrawImage (pDC, rect);

		}

		if (rgbText != 0x80000000)
		{
			 pDC->SetTextColor (rgbText);
			 pDC->SetBkColor (rgbBack);
		}
	}

}


///// CompareItem - Standard string compare
//
///
int CFontNameCombo::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
{
	FontInfo * pFI1 = (FontInfo *)lpCompareItemStruct->itemData1;
	FontInfo * pFI2 = (FontInfo *)lpCompareItemStruct->itemData2;

	WORD fontType1 = HIWORD(pFI1->dwFontType);
	WORD fontType2 = HIWORD(pFI2->dwFontType);

	// Fixed pitch fonts come before proportional pitch fonts
	//
	if ( (fontType1 & (VARIABLE_PITCH|FIXED_PITCH)) != (fontType2 & (VARIABLE_PITCH|FIXED_PITCH)) )
	{
		if (fontType1 & FIXED_PITCH)
			return -1;
		else
			return 1;
	}


	CString str1 (pFI1->szName);
	CString str2 (pFI2->szName);

	if (str1 < str2)
		return -1;
	else
		return str1 != str2;
}




/////////////////////////////////////////////////////////////////////////////
// CTTBitmap
//
//	Holds and draws the TrueType bitmap


///// Constructor
//
//	Loads bitmap resource and creates the mask.  The Mask allows the glyph
//	to be drawn transparently
///
CTTBitmap::CTTBitmap ()
{
	m_bValid =		LoadBitmap (IDB_BITMAP_TRUETYPE)
				&&	GetObject (sizeof(m_bmData), &m_bmData)
				&&	CreateMask ();
}

CTTBitmap::~CTTBitmap ()
{
}


///// CreateMask - Create foreground/background mask
//
//	Creates the monochrome bitmap used in DrawImage.
//
///
BOOL CTTBitmap::CreateMask ()
{
CDC		MaskDC;
CDC		BitmapDC;
BOOL	bReturn = FALSE;

	if (BitmapDC.CreateCompatibleDC (NULL) &&
		MaskDC.CreateCompatibleDC (NULL) &&
		m_bmpMask.CreateBitmap (m_bmData.bmWidth, m_bmData.bmHeight, 1, 1, NULL)) {

			//
			//	BitBlt() converts a color bitmap to monochrome by turning pixels of the current
			//	background color into white (1) and all others into black (0).  Since this is
			//	the inverse of the convention for the mask, we use NOTSRCCOPY.
			//
			//	However, Since MskBlt() is not implemented on Win95, we use the standard
			//	"True Mask Method" of drawing a bitmap with transparency.  This technique is
			//	described in "Bitmaps with Transparency" by Ron Gery - available on the MSDN CD.
			//	This technique uses 1 for background (transparent) and 0 for foreground.  So
			//	we use SRCCOPY.
			//
			(void)MaskDC.SelectObject (&m_bmpMask);
			(void)BitmapDC.SelectObject (this);
			(void)BitmapDC.SetBkColor(BitmapDC.GetPixel(0, 0));
			if (MaskDC.BitBlt (0, 0, m_bmData.bmWidth, m_bmData.bmHeight, &BitmapDC, 0, 0, SRCCOPY))
				bReturn = TRUE;
		}

	return bReturn;
}


/////	DrawImage - Draws the bitmap
//
//	pDC		-> Device Context to draw into
//	rect	-> Rectangle to draw into.  Image is drawn at 0,0, but is not stretched.
//
//	The image is drawn "transparently".  That is, background pixels in the bitmap take
//	on the background color of the destination.  The "background color" is the color of the
//	pixel in the upper left hand corner of the bitmap.
//
//	The technique used is to generate a monochrome bitmap mask of the same size and shape as
//	the original bitmap.  In the mask, a 1 indicates that the corresponding pixel in the
//	original bitmap is foreground, and a 0 indicates background.  The Win32 API MaskBlt()
//	uses such a mask to support a BitBlt using two raster ops - one for the foreground pixels,
//	one for the background.
///

#define TRANSBACK	0xAACC0000

BOOL CTTBitmap::DrawImage (CDC * pDC, CRect& rect)
{
	CDC dcBitmap;

	if (m_bValid && dcBitmap.CreateCompatibleDC (pDC)) {

		dcBitmap.SelectObject (this);

		// To achieve the same effect as as MaskBlt(), we make a copy of the bitmap into
		// yet another DC, then floodfill the background with the background color from
		// the target device.
		CDC dcMask;
		dcMask.CreateCompatibleDC (pDC);
		dcMask.SelectObject (&m_bmpMask);

		COLORREF rgbBackOld = pDC->SetBkColor (RGB(255, 255, 255));
		COLORREF rgbTextOld = pDC->SetTextColor (RGB(0,0,0));

		pDC->BitBlt (rect.left, rect.top, m_bmData.bmWidth, m_bmData.bmHeight, &dcBitmap, 0, 0, SRCINVERT);
		pDC->BitBlt (rect.left, rect.top, m_bmData.bmWidth, m_bmData.bmHeight, &dcMask, 0, 0, SRCAND);
		pDC->BitBlt (rect.left, rect.top, m_bmData.bmWidth, m_bmData.bmHeight, &dcBitmap, 0, 0, SRCINVERT);

		pDC->SetBkColor (rgbBackOld);
		pDC->SetTextColor (rgbTextOld);

		return TRUE;

		// Everything above starting with 'CDC dcAdjusted' is the workaround for the lack of
		// MaskBlt() in Win95.
		//return pDC->MaskBlt (rect.left, rect.top, m_bmData.bmWidth, m_bmData.bmHeight, &SrcDC, 0, 0, m_bmpMask, 0, 0, TRANSBACK);
		}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CWindowList

CWindowList::CWindowList()
{
}

CWindowList::~CWindowList()
{
}

void CWindowList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if (lpDrawItemStruct->itemID != -1L &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) != 0)
	{
		// The items representing a collection of windows will be displayed in BOLD
		//
		CFont * pFontPrev = NULL;
		CFont font;
		COLORREF rgbText = 0x80000000;	// Value is a flag for "color was changed"
		COLORREF rgbBack;

		if (lpDrawItemStruct->itemState & ODS_SELECTED)
		{	// Show item as selected with standard hilite colors.
			rgbText = pDC->SetTextColor (GetSysColor(COLOR_HIGHLIGHTTEXT));
			rgbBack = pDC->SetBkColor (GetSysColor (COLOR_HIGHLIGHT));
		}

		LOGFONT lf;

		ASSERT (::GetCurrentObject (pDC->m_hDC, OBJ_FONT));
		VERIFY (::GetObject (::GetCurrentObject (pDC->m_hDC, OBJ_FONT), sizeof lf, &lf));

		CString strItem;
		BOOL bGroupName = m_pWinList->GetName (lpDrawItemStruct->itemID, strItem);

		if (lf.lfWeight == FW_NORMAL)
			if (bGroupName)
			{
				lf.lfWeight = FW_BOLD;
				VERIFY (font.CreateFontIndirect (&lf));
				VERIFY (pFontPrev = pDC->SelectObject (&font));
			}
			else
				;
		else
			if (!bGroupName)
			{
				lf.lfWeight = FW_NORMAL;
				VERIFY (font.CreateFontIndirect (&lf));
				VERIFY (pFontPrev = pDC->SelectObject (&font));
			}
			else
				;


		CRect rectText (lpDrawItemStruct->rcItem);
		pDC->ExtTextOut (rectText.left + 2, rectText.top, ETO_CLIPPED|ETO_OPAQUE, rectText, strItem, strItem.GetLength(), NULL);

		if (pFontPrev)
		{
			VERIFY (pDC->SelectObject (pFontPrev) == &font);
			font.DeleteObject ();
		}

		if (rgbText != 0x80000000)
		{
			 pDC->SetTextColor (rgbText);
			 pDC->SetBkColor (rgbBack);
		}
	}
}

BEGIN_MESSAGE_MAP(CWindowList, CListBox)
	//{{AFX_MSG_MAP(CWindowList)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWindowList message handlers
/////////////////////////////////////////////////////////////////////////////

