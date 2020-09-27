/////////////////////////////////////////////////////////////////////////////
// cafebar.cpp
//
// email	date		change
// briancr	11/03/94	created
//
// copyright 1994 Microsoft

// Implementation of the CCAFEBar class

#include "stdafx.h"
#include "cafebar.h"
#include "toolset.h"
#include "cafedrv.h"
#include "guiv1.h"
#include "caferes.h"
#include "platform.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAFEBar buttons
static UINT BASED_CODE btnsCAFEBar[] =
{
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
		ID_SEPARATOR,			// space between button combo box
		ID_SEPARATOR,			// toolset combo box
		ID_SEPARATOR,			// space between combo boxes
		ID_SEPARATOR,			// build type combo box
		ID_SEPARATOR,			// space between combo boxes
		ID_SEPARATOR,			// language combo box
		ID_SEPARATOR,			// space between combo box and button
	IDM_OptionsSingleStep,
	IDM_OptionsBreakStep,
		ID_SEPARATOR,
	IDM_FileViewLog,
		ID_SEPARATOR,
	ID_APP_ABOUT,
};

const int ncbSpaceWidth = 12;
const int ncbToolsetWidth = 200;
const int ncbBuildTypeWidth = 100;
const int ncbLanguageWidth = 100;
//const int ncbToolsetDrop = 134;
//const int ncbBuildTypeDrop = 100;
//const int ncbLanguageDrop = 100;

/////////////////////////////////////////////////////////////////////////////
// CCAFEBar

CCAFEBar::~CCAFEBar()
{
	if (m_pfont) {
		delete m_pfont;
	}
	if (m_pcbToolset) {
		delete m_pcbToolset;
	}
	if (m_pcbBuildType) {
		delete m_pcbBuildType;
	}
	if (m_pcbLanguage) {
		delete m_pcbLanguage;
	}
}

BOOL CCAFEBar::CreateBar(CWnd* pParent)
{
	// set the font for the combo boxes
	m_pfont = CreateFont();

	// create the toolbar
	if (!Create(pParent, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, IDT_CAFEBar) ||
			!LoadBitmap(IDB_CAFEBAR) ||
			!SetButtons(btnsCAFEBar, sizeof(btnsCAFEBar)/sizeof(UINT))) {
		TRACE("CCAFEBar::CreateBar: Failed to create CAFEBar\n");
		return FALSE;       // fail to create
	}

	// create the platform combo box
	m_pcbToolset = CreateToolsetCombo(4);

	// create the target type combo box
	m_pcbBuildType = CreateBuildTypeCombo(6);

	// create the language combo box
	m_pcbLanguage = CreateLanguageCombo(8);

	// select the appropriate item in each combo box
	SelectToolset();
	SelectBuildType();
	SelectLanguage();

	// make the toolbar dockable
	EnableDocking(CBRS_ALIGN_ANY);

	// set the title
	SetWindowText("Standard");

	return TRUE;
}

// select the correct toolset in the toolset combo box
BOOL CCAFEBar::SelectToolset(void)
{
	CString strToolset;
	int i;

	// the toolset combo must be valid
	ASSERT(m_pcbToolset);

	// get the current toolset
	strToolset = m_pSettings->GetTextValue(settingPlatform);
	gplatformStr = strToolset;

	// find the toolset

  const CPlatform::COSArray *paryOS = CPlatform::GetOSArray();
  int nSize = paryOS->GetSize();

	for (i = 0; i < nSize; i++) 
  {
		if (strToolset.CompareNoCase(paryOS->GetAt(i)->GetShortName()) == 0)
    {
			// select this toolset
			m_pcbToolset->SetCurSel(i);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CCAFEBar::UpdateToolset(void)
{
	int nToolset;
	CString strToolset;
	int i;

	// the toolset combo must be valid
	ASSERT(m_pcbToolset);

	// get the currently selected string in the toolset combo box
	nToolset = m_pcbToolset->GetCurSel();

	// get the text of the currently selected combo box item
	m_pcbToolset->GetLBText(nToolset, strToolset);

	// find which string this is

  const CPlatform::COSArray *paryOS = CPlatform::GetOSArray();
  int nSize = paryOS->GetSize();

	for (i = 0; i < nSize; i++) 
  {
		if (strToolset == paryOS->GetAt(i)->GetName()) 
    {
			// change toolsets
			m_pSettings->SetTextValue(settingPlatform, paryOS->GetAt(i)->GetShortName());
			gplatformStr = paryOS->GetAt(i)->GetShortName();
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CCAFEBar::SelectBuildType(void)
{
	// the build type combo must be valid
	ASSERT(m_pcbBuildType);

	// select the build type
	m_pcbBuildType->SetCurSel(1 - m_pSettings->GetBooleanValue(settingDebugBuild));

	return TRUE;
}

BOOL CCAFEBar::UpdateBuildType(void)
{
	int nBuildType;
	CString strBuildType;
	int i;

	// the build type combo must be valid
	ASSERT(m_pcbBuildType);

	// get the currently selected string in the toolset combo box
	nBuildType = m_pcbBuildType->GetCurSel();

	// get the text of the currently selected combo box item
	m_pcbBuildType->GetLBText(nBuildType, strBuildType);

	// find which string this is
	for (i = 0; !CToolset::m_strBuildType[i].IsEmpty(); i++) {
		if (strBuildType == CToolset::m_strBuildType[i]) {
			m_pSettings->SetBooleanValue(settingDebugBuild, 1-i);

			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCAFEBar::SelectLanguage(void)
{
	CString strLanguage;
	int i;

	// the language combo must be valid
	ASSERT(m_pcbLanguage);

	// get the current language
	strLanguage = m_pSettings->GetTextValue(settingLanguage);

	// find the language

  const CPlatform::CLangArray *paryLang = CPlatform::GetLangArray();
  int nSize = paryLang->GetSize();

	for (i = 0; i < nSize; i++) 
  {
		if (strLanguage.CompareNoCase(paryLang->GetAt(i)->GetName()) == 0)
    {
			// select this language
			m_pcbLanguage->SetCurSel(i);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CCAFEBar::UpdateLanguage(void)
{
	int nLanguage;
	CString strLanguage;

	// the language combo must be valid
	ASSERT(m_pcbLanguage);

	// get the currently selected string in the toolset combo box
	nLanguage = m_pcbLanguage->GetCurSel();

	// get the text of the currently selected combo box item
	m_pcbLanguage->GetLBText(nLanguage, strLanguage);

	// store the language setting
	m_pSettings->SetTextValue(settingLanguage, strLanguage);

	return TRUE;
}


CFont* CCAFEBar::CreateFont(void)
{
	LOGFONT lf;
	CFont* pfont;
	CString strDefaultFont;

	// clear out the logfont memory
	memset(&lf, 0, sizeof(lf));

	// create a new font object
	pfont = new CFont;

	// is this a DBCS OS?
	if (!::GetSystemMetrics(SM_DBCSENABLED)) {
		// Since design guide says toolbars are fixed height so is the font.
		lf.lfHeight = -12;
		lf.lfWeight = FW_BOLD;
		lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		strDefaultFont.LoadString(IDS_TOOLBAR_FONT);
		lstrcpy(lf.lfFaceName, strDefaultFont);
		if (!pfont->CreateFontIndirect(&lf)) {
			TRACE("CCAFEBar::GetFont: could not create font for CCAFEBar\n");
		}
	}
	else {
		pfont->Attach(::GetStockObject(SYSTEM_FONT));
	}

	return pfont;
}

CComboBox* CCAFEBar::CreateToolsetCombo(int nPos)
{
	CComboBox* pcbToolset;
	int i;

	// get a pointer to the app
	CCAFEDriver *pApp = ((CCAFEDriver*)AfxGetApp());

	// create the combo box
	pcbToolset = CreateCombo(IDC_CAFEBarToolset, nPos, ncbToolsetWidth);

	// fill the target combo box

/* OLD:
	for (i = 0; !CToolset::m_strToolsets[i].IsEmpty(); i++) {
		pcbToolset->AddString(CToolset::m_strToolsets[i]);
	}
*/

// NEW:

    const CPlatform::COSArray *paryOS = CPlatform::GetOSArray();
    int nSize = paryOS -> GetSize();

  CPlatform::COS *pOS;

	for (i = 0; i < nSize; i++)
	{
    pOS = paryOS->GetAt(i);
    pcbToolset->AddString(pOS->GetName());
	}
 
	// size the combo box to fit the content
	SizeComboToContent(pcbToolset);

	return pcbToolset;
}

CComboBox* CCAFEBar::CreateBuildTypeCombo(int nPos)
{
	CComboBox* pcbBuildType;
	int i;

	// create the combo box
	pcbBuildType = CreateCombo(IDC_CAFEBarBuildType, nPos, ncbBuildTypeWidth);


	// fill the build type combo box
	for (i = 0; !CToolset::m_strBuildType[i].IsEmpty(); i++) {
		pcbBuildType->AddString(CToolset::m_strBuildType[i]);
	}


	// size the combo box to fit the content
	SizeComboToContent(pcbBuildType);

	return pcbBuildType;
}

CComboBox* CCAFEBar::CreateLanguageCombo(int nPos)
{
	CComboBox* pcbLanguage;
	int i;

	// get a pointer to the app
	CCAFEDriver *pApp = ((CCAFEDriver*)AfxGetApp());

	// create the combo box
	pcbLanguage = CreateCombo(IDC_CAFEBarLanguage, nPos, ncbLanguageWidth);

/* OLD:
	// fill the language combo box
	for (i = 0; !CToolset::m_strLanguage[i].IsEmpty(); i++) {
		pcbLanguage->AddString(CToolset::m_strLanguage[i]);
	}
*/

// NEW:

    const CPlatform::CLangArray *paryLang = CPlatform::GetLangArray();
    int nSize = paryLang -> GetSize();

  CPlatform::CLanguage *pLang;

	for (i = 0; i < nSize; i++)
	{
    pLang = paryLang->GetAt(i);
    if (*pLang != CPlatform::AnyLanguage)
      pcbLanguage->AddString(pLang->GetName());
	}



	// size the combo box to fit the content
	SizeComboToContent(pcbLanguage);

	return pcbLanguage;
}

CComboBox* CCAFEBar::CreateCombo(UINT nId, int nPos, int nWidth)
{
	CComboBox* pcb;
	CRect rect;

	// the position must be at least 1
	ASSERT(nPos > 0);

	// create a combo box object
	pcb = new CComboBox;

	// create a 12 pixel gap between the previous toolbar item and this combo box
	SetButtonInfo(nPos-1, ID_SEPARATOR, TBBS_SEPARATOR, ncbSpaceWidth);

	// create space for the combo box
	SetButtonInfo(nPos, nId, TBBS_SEPARATOR, nWidth);
	
	// create a 12 pixel gap between this combo box and the next toolbar item
	SetButtonInfo(nPos+1, ID_SEPARATOR, TBBS_SEPARATOR, ncbSpaceWidth);

	// get and set rectangle information about this combo box's space
	GetItemRect(nPos, &rect);
	rect.top = theApp.IsOnWin95() ? 2 : 3;
	rect.bottom = rect.top + 10;

	// create the combo box on the toolbar
	if (!pcb->Create(CBS_DROPDOWNLIST | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP, rect, this, nId)) {
		TRACE("CCAFEBar::CreateCombo: failed to create combo box\n");
		delete pcb;
		return NULL;
	}

	// set font for the combo box
	pcb->SetFont(m_pfont);

	return pcb;
}

void CCAFEBar::SizeComboToContent(CComboBox* pCombo, int nMax /*= 0*/)
{
	ASSERT_VALID(pCombo);
	int cyEdit = (int)::SendMessage(pCombo->m_hWnd, CB_GETITEMHEIGHT, UINT(-1), 0);

	CDC* pDC = pCombo->GetDC();
	CFont* pFontOld;
	CFont* pFont = pCombo->GetFont();

	if (pFont != NULL)
		pFontOld = pDC->SelectObject(pFont);

	// FUTURE:  Need better way to get height
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	if (pFont != NULL)
		pDC->SelectObject(pFontOld);

	pCombo->ReleaseDC(pDC);

	int cyHeight = pCombo->GetCount();
	if (nMax > 0 && cyHeight > nMax)
		cyHeight = nMax;

	cyHeight = cyEdit +	cyHeight * tm.tmHeight +
		GetSystemMetrics(SM_CYBORDER) * 3;

	CRect rectCombo;
	pCombo->GetDroppedControlRect(&rectCombo);

	if (theApp.IsOnWin95())
	{
		// Win95 does its own size to content.  Leave it alone, if we have
		// enough space, otherwise use max, and  let the system fix it.

		if (rectCombo.Height() > cyHeight)
			cyHeight = rectCombo.Height();
		else
		{
			cyHeight = cyEdit +	cyHeight * tm.tmHeight +
				GetSystemMetrics(SM_CYBORDER) * 5;
		}
	}

	cyHeight *= 2 ;
	if (rectCombo.Height() != cyHeight)
	{
		pCombo->SetWindowPos(NULL, 0, 0, rectCombo.Width(), cyHeight,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCAFEBar message handlers

