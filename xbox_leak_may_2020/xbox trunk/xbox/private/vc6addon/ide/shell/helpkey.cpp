////////////////////////////////////////////////////////////////////////////
// Help Keyboard...

#include "stdafx.h"
#include "resource.h"
#include "utilctrl.h"
#include "tap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

extern HICON AfxLoadIcon(UINT nResourceID);
extern void RemoveAccel(CString& strMenu);

// Item sequencing
//
// In 4.x, the displayed version of the keyboard grid was qsort-ed purely on
// its primary key. The printed version was qsorted first on its group and secondly
// on its 'command name' (e.g. FileOpen). A copied text version would be copied in the
// reverse order in which it was selected.
//
// This (inconsistent) system causes several problems:
// 1) Orion bug 102
//    When the user selects some key items to copy, they are copied in the reverse order of
//    their selection.
// 2) Random printout order
//    Printouts occur in seemingly random order. The order is unrelated
//    to what is displayed on screen. This becomes particularly noticeable
//    when the user selects several rows, as they are shuffled. The logic behind
//    the sort order is not clear to the user as the 'command name' is not included
//    in the printout
// 3) Sorting by group
//    When the user chooses to sort by group (e.g. 'Dialog', 'Image', etc), the suborder
//    seems completely random. It is, in fact, indirectly connected to the previous order
//    the list was in.
//
// To solve these problems:
// In the copy code, I am creating an extra list of pointers, which I will sort the same way
// as the displayed/printed list. An alternative would be to modify the internal ordering of the CGrid
// class, but this seems too major a change.
// In the display and print code in Orion I am implementing primary and secondary sorting keys.
// Primary = Group (as now), secondary = Command Name
// The secondary will always be description. 
// For printouts, the primary will always be group. The print secondary will be the display 
// primary, unless the display primary was group, in which case the print secondary
// will be the display secondary.
//
// Aside from cases with the same command name, which should be impossible, this scheme will ensure that 
// the copied and printed order matches the displayed order.
// martynl 12Mar96

////////////////////////////////////////////////////////////////////////////

class CHelpKeyWnd : public CFrameWnd
{
public:
	CHelpKeyWnd();
	~CHelpKeyWnd();

	void FillGrid();
	void FillGridCmds(int nGroup);
	void AddKeymapToGrid(CGridWnd* pGrid, CKeymap* pKeymap, 
		const char* szCategory, const char* szPrefix, WORD kcPrefix);
	void EnumCmdTable(CGridWnd* pGrid, int nGroup);
	void InitGroupNames();

	virtual BOOL Create();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpCreateStruct, 
		CCreateContext* pCreateContext);
	void DeleteGridRows();
	void UpdateGrid(int nGroup);

	void ReadProfile();
	void WriteProfile();

	void OnHelp();
	void OnPrint();
	void OnCopy();

	CRect m_wndRect;
	int m_nSortColumn;
	BOOL m_bAllCmds;
	int m_nPrevSel;

	CAppToolGroups* m_pToolGroups;

	CToolBar m_toolBar;
	CComboBox m_cbGroup;


protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnClose();
	afx_msg void OnToggleAllCmds();
	afx_msg void OnUpdateToggleAllCmds(CCmdUI* pCmdUI);
	afx_msg void OnGroupSelect();

	DECLARE_MESSAGE_MAP()
};

class CHelpKeyGridRow : public CGridRow
{
public:
	CHelpKeyGridRow(CGridWnd* pGrid);

	virtual int Compare(const CGridRow* pCompareRow) const;
	virtual void DrawCell(CDC* pDC, const CRect& rect, int nColumn);
	virtual void ResetSize(CDC* pDC);
	virtual CSize GetCellSize(CDC* pDC, int nColumn) const;

	UINT m_nCmdID;
	CString m_cells [4];
};

class CHelpKeyGrid : public CGridWnd
{
public:
	CHelpKeyGrid(int nSortColumn);
};

////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CHelpKeyWnd, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_PRINT, OnPrint)
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_CBN_SELCHANGE(ID_HELP_KEY_CATEGORY_COMBO, OnGroupSelect)
//	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////

static const char szGeneral [] = "General";
static const char szPosKey [] = "KeyboardWindowPos";
static const char szSortKey [] = "KeyboardWindowSort";

extern BOOL GetRegRect(const char* szSection, const char* szEntry, CRect& rect);
extern void WriteRegRect(const char* szSection, const char* szEntry, const CRect& rect);

void CHelpKeyWnd::ReadProfile()
{
	if (!GetRegRect(szGeneral, szPosKey, m_wndRect))
		m_wndRect.SetRect(30, 30, 30 + 550, 30 + 300);
	m_nSortColumn = GetRegInt(szGeneral, szSortKey, 0);
}

void CHelpKeyWnd::WriteProfile()
{
	WriteRegRect(szGeneral, szPosKey, m_wndRect);
	WriteRegInt(szGeneral, szSortKey, m_nSortColumn);
}

CHelpKeyWnd::CHelpKeyWnd()
: m_pToolGroups(CAppToolGroups::GetAppToolGroups()),
  m_nPrevSel(-1)
{
	m_bAllCmds = FALSE;

	ReadProfile();
}

CHelpKeyWnd::~CHelpKeyWnd()
{
	m_pToolGroups->ReleaseAppToolGroups();

	WriteProfile();
}

BOOL CHelpKeyWnd::Create()
{
	CString strHelpKeyCaption;
	CString str;
	VERIFY(strHelpKeyCaption.LoadString(IDS_HELP_KEY_CAPTION));

	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);

	if (m_wndRect.left >= cxScreen - 32)
		m_wndRect.OffsetRect(m_wndRect.left - cxScreen - 32, 0);

	if (m_wndRect.top >= cyScreen - 32)
		m_wndRect.OffsetRect(0, m_wndRect.top - cyScreen - 32);

	if (!CFrameWnd::Create(AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW),
		0, AfxLoadIcon(IDR_HELP_KEYBOARD)), strHelpKeyCaption, WS_VISIBLE | 
		WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, m_wndRect, 
		AfxGetApp()->m_pMainWnd, NULL, NULL, NULL))
	{
		return FALSE;
	}

	CHelpKeyGrid* pGrid = (CHelpKeyGrid*)GetDlgItem(AFX_IDW_PANE_FIRST);

	VERIFY(str.LoadString(IDS_HELP_KEY_EDITOR));
	pGrid->SetColumnCaption(0, str);
	VERIFY(str.LoadString(IDS_HELP_KEY_COMMAND));
	pGrid->SetColumnCaption(1, str);
	VERIFY(str.LoadString(IDS_HELP_KEY_KEYS));
	pGrid->SetColumnCaption(2, str);
	VERIFY(str.LoadString(IDS_HELP_KEY_DESCRIPTION));
	pGrid->SetColumnCaption(3, str);

	m_bAllCmds = FALSE;
	UpdateGrid(-1);
	
	return TRUE;
}

UINT buttons [] =
{
	ID_FILE_PRINT,
	ID_SEPARATOR,
	ID_EDIT_COPY, 
	ID_SEPARATOR,
	ID_SEPARATOR
};

int CHelpKeyWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CString str;
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_toolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS) ||
		!m_toolBar.LoadBitmap(IDR_HELP_KEYBOARD) ||
		!m_toolBar.SetButtons(buttons, sizeof (buttons) / sizeof (UINT)))
	{
		return -1;
	}

	CRect rect;
	m_toolBar.GetItemRect(4, &rect);
	rect.top++;
	rect.bottom = rect.top + 150;
	rect.right = rect.left + 120;
	if (!m_cbGroup.Create(CBS_DROPDOWNLIST|CBS_HASSTRINGS|WS_VISIBLE|WS_VSCROLL,
			rect, &m_toolBar, ID_HELP_KEY_CATEGORY_COMBO))
	{
		return FALSE;
	}
	m_cbGroup.SetFont(GetStdFont(font_Normal));
	m_cbGroup.SetExtendedUI();

	VERIFY(str.LoadString(IDS_HELP_KEY_BOUND));
	int index = m_cbGroup.AddString(str);
	m_cbGroup.SetItemData(index, (unsigned)-1);
	VERIFY(str.LoadString(IDS_HELP_KEY_ALL));
	index = m_cbGroup.AddString(str);
	m_cbGroup.SetItemData(index, (unsigned)-1);
	InitGroupNames();
	m_cbGroup.SetCurSel(0);
	m_nPrevSel = 0;
	
	AfxGetApp()->m_pMainWnd->EnableWindow(FALSE);
	EnableWindow(TRUE);
	
	return 0;
}

BOOL CHelpKeyWnd::OnCreateClient(LPCREATESTRUCT lpCreateStruct, 
	CCreateContext* pCreateContext)
{
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0L, SWP_DRAWFRAME);

	CHelpKeyGrid* pGrid = new CHelpKeyGrid(m_nSortColumn);
	if (!pGrid->CreateEx(WS_EX_CLIENTEDGE, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 
		AFX_IDW_PANE_FIRST))
	{
		return FALSE;
	}

	return CFrameWnd::OnCreateClient(lpCreateStruct, pCreateContext);
}

void CHelpKeyWnd::OnSetFocus(CWnd* pOldWnd)
{
	GetDlgItem(AFX_IDW_PANE_FIRST)->SetFocus();
}

void CHelpKeyWnd::OnClose()
{
	DeleteGridRows();
	delete (CHelpKeyGrid*) GetDlgItem(AFX_IDW_PANE_FIRST);

	AfxGetApp()->m_pMainWnd->EnableWindow(TRUE);
	GetWindowRect(m_wndRect);
	CFrameWnd::OnClose();
}

BOOL CHelpKeyWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
//		case VK_F1:
//		case VK_RETURN:
//			OnHelp();
//			return TRUE;

		case 'P':
			if (GetKeyState(VK_CONTROL) < 0)
			{
				OnPrint();
				return TRUE;
			}
			break;
			
		case 'C':
			if (GetKeyState(VK_CONTROL) < 0)
			{
				OnCopy();
				return TRUE;
			}
			break;
			
		case VK_ESCAPE:
			if (GetKeyState(VK_CONTROL) >= 0)
			{
				OnClose();
				return TRUE;
			}
			break;
		}
	}

	if (pMsg->message == WM_SYSKEYDOWN && ::IsChild(m_hWnd, pMsg->hwnd))
	{
		PostMessage(WM_SYSKEYDOWN, pMsg->wParam, pMsg->lParam);
		return TRUE;
	}

	if (CFrameWnd::PreTranslateMessage(pMsg))
		return TRUE;

	// Finally, prevent keyboard messages from being PreTranslated by the
	// parent of this window.  Otherwise, the user will be able to execute
	// keyboard commands on the main window.

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);
		return TRUE;
	}

	return FALSE;
}

void CHelpKeyWnd::OnHelp()
{
//	CHelpKeyGrid* pGrid = (CHelpKeyGrid*)GetDlgItem(AFX_IDW_PANE_FIRST);
//	POSITION pos = pGrid->GetHeadSelPosition();
//	if (pos == NULL)
//	{
//		MessageBeep(0);
//		return;
//	}
//	
//	CHelpKeyGridRow* pRow = (CHelpKeyGridRow*)pGrid->GetNextSel(pos);
//	theApp.HelpOnApplication(pRow->m_nCmdID - HID_BASE_COMMAND, HELPTYPE_COMMAND);
}

void CHelpKeyWnd::DeleteGridRows()
{
	CHelpKeyGrid* pGrid = (CHelpKeyGrid*)GetDlgItem(AFX_IDW_PANE_FIRST);
	POSITION pos = pGrid->GetHeadRowPosition();
	pGrid->m_selection.RemoveAll();
	pGrid->m_posCurRow = NULL;
	pGrid->m_posTopVisibleRow = NULL;
	while (pos != NULL)
	{
		POSITION posOld = pos;
		CGridRow* pRow = pGrid->GetNextRow(pos);
		pGrid->m_rows.RemoveAt(posOld);
		delete pRow;
	}
}

void CHelpKeyWnd::UpdateGrid(int nGroup)
{
	BeginWaitCursor();
	CHelpKeyGrid* pGrid = (CHelpKeyGrid*)GetDlgItem(AFX_IDW_PANE_FIRST);
	pGrid->CloseActiveElement();
	DeleteGridRows();

	if (m_bAllCmds)
		FillGridCmds(nGroup);
	else
		FillGrid();
	UpdateWindow();
	EndWaitCursor();
}

void CHelpKeyWnd::OnUpdateToggleAllCmds(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bAllCmds);
}

void CHelpKeyWnd::OnGroupSelect()
{

	int nSel = m_cbGroup.GetCurSel();
	if (nSel == -1)
		return;

	// only refresh if the selection changed
	if (nSel != m_nPrevSel) {
		m_nPrevSel = nSel;

		CHelpKeyGrid* pGrid = (CHelpKeyGrid*)GetDlgItem(AFX_IDW_PANE_FIRST);
		CString str;

		if (nSel == 0) {
			m_bAllCmds = FALSE;
			VERIFY(str.LoadString(IDS_HELP_KEY_EDITOR));
			pGrid->SetColumnCaption(0, str);
		}
		else {
			m_bAllCmds = TRUE;
			VERIFY(str.LoadString(IDS_HELP_KEY_CATEGORY));
			pGrid->SetColumnCaption(0, str);
		}
		int nGroup = m_cbGroup.GetItemData(nSel);
		UpdateGrid(nGroup);
	}
}

static int CopyCompareRows(const void* p1, const void* p2)
{
	const CHelpKeyGridRow* pRow1 = *(const CHelpKeyGridRow**)p1;
	const CHelpKeyGridRow* pRow2 = *(const CHelpKeyGridRow**)p2;
	return pRow1->Compare(pRow2);
}

void CHelpKeyWnd::OnCopy()
{
	CWaitCursor waitCursor;
	
	HGLOBAL hGlobal;
	TCHAR* pch;
	BOOL bUseSelection=FALSE;
	CPtrArray *pSortedSelection=NULL;

	CHelpKeyGrid* pGrid = (CHelpKeyGrid*)GetDlgItem(AFX_IDW_PANE_FIRST);

	// if we're copying a selection, we need to go to the grid, get all the selections,
	// and sort them into the same order as they're displayed on screen. The grid would
	// return them in the reverse order they were selected, which is no use. martynl 12Mar96
	if(!pGrid->m_selection.IsEmpty())
	{
		bUseSelection=TRUE;

		// this ptrarry will contain all of the selected rows
		pSortedSelection=new CPtrArray;
		pSortedSelection->SetSize(pGrid->m_selection.GetCount());

		// copy rows from grid
		POSITION pos = pGrid->GetHeadSelPosition();
		int index=0;
		while (pos != NULL)
		{
			CHelpKeyGridRow* pRow = (CHelpKeyGridRow*)(pGrid->GetNextSel(pos));

			pSortedSelection->SetAt(index,pRow);

			++index;
		}

		// sort them
		if ( pSortedSelection->GetSize( ) > 0 )
		{
			qsort(&((*pSortedSelection)[0]), pSortedSelection->GetSize(), sizeof (void*), CopyCompareRows);
		}
	}
	
	for (int pass = 0; pass < 2; pass += 1)
	{
		int cb = 0;
		// this loop is has two possible iterators. If we're iterating over a selection,
		// we're iterating a CPtrArray with index. If we're iterating over the whole thing,
		// we're iterating a CPtrList with pos.
		POSITION pos=NULL;
		int index=0;
		if(!bUseSelection)
		{
			pos=pGrid->GetHeadRowPosition();
		}

		// ensure that the initial value is valid
		ASSERT(bUseSelection || pos!=NULL);

		while ((bUseSelection && index <pSortedSelection->GetSize()) ||
			   (!bUseSelection && pos != NULL))
		{
			CHelpKeyGridRow* pRow = NULL;
			if(bUseSelection)
			{
				pRow=(CHelpKeyGridRow*)((*pSortedSelection)[index]);
				++index;
			} 
			else
			{
				pRow=(CHelpKeyGridRow*)(pGrid->GetNextRow(pos));
			}

			// ensure that pRow was initialised somehow
			ASSERT(pRow!=NULL);

			CString str = pRow->m_cells[0];
			str += '\t';
			str += pRow->m_cells[1];
			str += '\t';
			str += pRow->m_cells[2];
			str += '\t';
			str += pRow->m_cells[3];
			str += "\r\n";

			if (pass == 0)
			{
				cb += str.GetLength();
			}
			else
			{
				_tcscpy(pch, str);
				pch += str.GetLength();
			}
		}

		if (pass == 0)
		{
			cb += 1; // for '\0' terminator

			hGlobal = GlobalAlloc(GMEM_SHARE, cb);
			if (hGlobal == NULL)
			{
				MessageBeep(0);
				return;
			}
			
			pch = (TCHAR*)GlobalLock(hGlobal);
		}
	}
	
	GlobalUnlock(hGlobal);

	if(bUseSelection)
	{
		pSortedSelection->RemoveAll();
		delete pSortedSelection;
	}
	
	if (!OpenClipboard() || !EmptyClipboard())
	{
		MessageBeep(0);
		GlobalFree(hGlobal);
		return;
	}
	
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
}

static int CompareRows(const void* p1, const void* p2)
{
	const CHelpKeyGridRow* pRow1 = *(const CHelpKeyGridRow**)p1;
	const CHelpKeyGridRow* pRow2 = *(const CHelpKeyGridRow**)p2;

	// Category is primary sort key...
	int cmp = pRow1->m_cells[0].Compare(pRow2->m_cells[0]);
	if (cmp != 0)
		return cmp;

	// the print secondary key is the primary sort, unless that would just
	// duplicate the above sort, in which case we use the command
	int nSortColumn=pRow1->GetGrid()->m_nSortColumn;
	int nSubKey= nSortColumn==0 ? 1 : nSortColumn;

	// Name is secondary sort key...
	return pRow1->m_cells[nSubKey].Compare(pRow2->m_cells[nSubKey]);
}

void CHelpKeyWnd::OnPrint()
{
	CHelpKeyGrid* pGrid = (CHelpKeyGrid*)GetDlgItem(AFX_IDW_PANE_FIRST);

	DWORD dwFlags = PD_ALLPAGES | PD_USEDEVMODECOPIES | 
		PD_NOPAGENUMS | PD_HIDEPRINTTOFILE;
	if (pGrid->m_selection.IsEmpty())
		dwFlags |= PD_NOSELECTION;
	CPrintDialog dlg(FALSE, dwFlags);

	if (dlg.DoModal() != IDOK)
		return;

	UpdateWindow();
	
	HDC hDC = dlg.GetPrinterDC();
	if (hDC == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
		return;
	}

	CWaitCursor waitCursor;

	CPtrArray rows;

	// BLOCK: Fill in local rows array and sort by category
	{
		int nRow = 0;
		if (dlg.PrintSelection())
		{
			rows.SetSize(pGrid->m_selection.GetCount());
			POSITION pos = pGrid->GetHeadSelPosition();
			while (pos != NULL)
				rows[nRow++] = pGrid->GetNextSel(pos);
		}
		else
		{
			rows.SetSize(pGrid->GetRowCount());
			POSITION pos = pGrid->GetHeadRowPosition();
			while (pos != NULL)
				rows[nRow++] = pGrid->GetNextRow(pos);
		}

		// CompareRows is hard-coded to sort first by group
		qsort(&rows[0], rows.GetSize(), sizeof (void*), CompareRows);
	}

	CDC dc;
	dc.CreateCompatibleDC(NULL);
	int nXPelsPerInchScreen = dc.GetDeviceCaps(LOGPIXELSX);
	dc.DeleteDC();

	dc.Attach(hDC);

	// Body text font
	LOGFONT logfont = *GetStdLogfont(font_Normal, &dc);
	logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	CFont font;
	font.CreateFontIndirect(&logfont);

	// Heading font
	logfont = *GetStdLogfont(font_Bold, &dc);
	logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	logfont.lfHeight = logfont.lfHeight * 3 / 2;
	CFont headingFont;
	headingFont.CreateFontIndirect(&logfont);

	// Header/footer font
	logfont = *GetStdLogfont(font_Italic, &dc);
	logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	CFont headerFont;
	headerFont.CreateFontIndirect(&logfont);

	CFont* pOldFont = dc.SelectObject(&font);

	int nPageWidth = dc.GetDeviceCaps(HORZRES);
	int nPageHeight = dc.GetDeviceCaps(VERTRES);
	int nXPelsPerInch = dc.GetDeviceCaps(LOGPIXELSX);
	int nYPelsPerInch = dc.GetDeviceCaps(LOGPIXELSY);
	
	CRect pageRect;
	pageRect.left = nXPelsPerInch;
	pageRect.top = nYPelsPerInch;
	pageRect.right = nPageWidth - nXPelsPerInch;
	pageRect.bottom = nPageHeight - nYPelsPerInch;
	
	CString strTitle;
	VERIFY(strTitle.LoadString(IDS_HELP_KEY_PRINT_TITLE));

	CString strCopyright;
	VERIFY(strCopyright.LoadString(IDS_HELP_KEY_PRINT_CRIGHT));

	CString strFmtPage;
	VERIFY(strFmtPage.LoadString(IDS_HELP_KEY_PRINT_PAGE));

	CString strFooter;
	VERIFY(strFooter.LoadString(IDS_HELP_KEY_PRINT_FOOTER));
	int cxFooter = dc.GetTextExtent(strFooter).cx;

	DOCINFO docinfo;
	memset(&docinfo, 0, sizeof(DOCINFO));		// make sure all members are initialized
	docinfo.cbSize = sizeof(DOCINFO);
	docinfo.lpszDocName = strTitle;
	docinfo.lpszOutput = NULL;

	dc.StartDoc(&docinfo);

	char chEmm = 'M';
	CSize sizeEmm = dc.GetTextExtent(&chEmm, 1);

	int cxColumn1 = 0;
	if (m_bAllCmds)
	{
		for (int i = 0; i < rows.GetSize(); i += 1)
		{
			CHelpKeyGridRow* pRow = (CHelpKeyGridRow*)rows[i];

			CSize size = dc.GetTextExtent(pRow->m_cells[1]);
			if (size.cx > cxColumn1)
				cxColumn1 = size.cx;
		}
	}

	int cxColumn2 = 0;
	for (int i = 0; i < rows.GetSize(); i += 1)
	{
		CHelpKeyGridRow* pRow = (CHelpKeyGridRow*)rows[i];

		CSize size = dc.GetTextExtent(pRow->m_cells[2]);
		if (size.cx > cxColumn2)
			cxColumn2 = size.cx;
	}

	int xCategory = pageRect.left;
	int xColumn1 = pageRect.left + nXPelsPerInch / 8;
	int xColumn2 = xColumn1 + cxColumn1 + nXPelsPerInch / 8;
	int xColumn3 = xColumn2 + cxColumn2 + nXPelsPerInch / 8;
	int xCenterPage = pageRect.left + pageRect.Width() / 2;
	int yHeader = sizeEmm.cy;
	int yFooter = nPageHeight - 2 * sizeEmm.cy;
	int cySpaceItems = 2 * (nYPelsPerInch / 72);

	int y = pageRect.top;
	int nPage = 1;
	CString strCategory;
	BOOL bEndedPage = TRUE;

	for (i = 0; i < rows.GetSize(); i += 1)
	{
		CHelpKeyGridRow* pRow = (CHelpKeyGridRow*)rows[i];

		BOOL bStartNewCategory = strCategory.Compare(pRow->m_cells[0]) != 0;

		// Calculate the height of the potentially multi-line description...
		CRect descRect(xColumn3, y, pageRect.right, y);
		int cyDesc = dc.DrawText(pRow->m_cells[3], descRect, DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK);

		// Calculate the total height of this item (might include new heading)...
		int cyItem = cyDesc;
		if (bStartNewCategory)
			cyItem += 3 * sizeEmm.cy; // Need room for a new heading

		// See if we need to start a new page...
		if (y + cyItem > pageRect.bottom)
		{
			bEndedPage = TRUE;
			dc.EndPage();
			y = pageRect.top;
			descRect.top = y;
			nPage += 1;
		}

		// Print the header and footer first thing for each page...
		if (y == pageRect.top)
		{
			dc.StartPage();
			bEndedPage = FALSE;

			// Print Header
			CFont* pOldFont = dc.SelectObject(&headerFont);

			CSize size = dc.GetTextExtent(strTitle);
			dc.TextOut(xCenterPage - size.cx / 2, yHeader, strTitle);

			size = dc.GetTextExtent(strCopyright);
			dc.TextOut(xCenterPage - size.cx / 2, yHeader + size.cy, strCopyright);

			// Print Footer
			CString str;
			str.Format(strFmtPage, nPage);
			size = dc.GetTextExtent(str);
			dc.TextOut(xCenterPage - size.cx / 2, yFooter, str);

			dc.TextOut(pageRect.right - cxFooter, yFooter, strFooter);

			dc.SelectObject(pOldFont);
		}

		if (bStartNewCategory)
		{
			// Print a new heading since the category changed...

			// One blank line
			y += sizeEmm.cy;

			// Heading in larger bold font
			strCategory = pRow->m_cells[0];
			CFont* pOldFont = dc.SelectObject(&headingFont);
			dc.TextOut(xCategory, y, strCategory);
			y += dc.GetTextExtent(&chEmm, 1).cy;
			dc.SelectObject(pOldFont);

			// Line under the heading
			int cy = nYPelsPerInch / 72;
			dc.PatBlt(xCategory, y + cy, pageRect.right - xCategory, cy, BLACKNESS);
			y += cy * 3;
		}

		// This may have moved due to a new page or category starting...
		descRect.top = y;
		descRect.bottom = descRect.top + cyDesc;

		if (m_bAllCmds)
			dc.TextOut(xColumn1, y, pRow->m_cells[1]);
		dc.TextOut(xColumn2, y, pRow->m_cells[2]);
		dc.DrawText(pRow->m_cells[3], descRect, DT_NOPREFIX | DT_WORDBREAK);

		y += cyDesc + cySpaceItems;
	}

	if (!bEndedPage)
		dc.EndPage();
		
	dc.EndDoc();
	dc.SelectObject(pOldFont);
}

extern void AccelFromKc(ACCEL& acc, WORD kc, UINT nCmdID = 0);
extern void GetCmdDescription(UINT nCmdID, CString& str);

void CHelpKeyWnd::AddKeymapToGrid(CGridWnd* pGrid, CKeymap* pKeymap, 
	const char* szCategory, const char* szPrefix, WORD kcPrefix)
{
	POSITION pos = pKeymap->GetStartPosition();
	while (pos != NULL)
	{
		WORD kc;
		UINT nCmdID;
		CKeymap* pSubKeymap;
		pKeymap->GetNextKey(pos, kc, nCmdID, pSubKeymap);
		
		ACCEL acc;
		AccelFromKc(acc, kc, nCmdID);
		
		if (pSubKeymap == NULL)
		{
			LPCTSTR lpszCommand; 

			// Note: dynamic command strings are stored in a single global CString, so we don't
			// want to hold onto the return value of GetCOmmandString very long. In this case,
			// we copy it to a CString (m_cells) which lets us reuse the global cstring
			// bobz 7/96

			if (!theCmdCache.GetCommandString(nCmdID, STRING_COMMAND, &lpszCommand))
				continue;

			// olympus 12483
			// filter out unnamed commands
			// this needs to be fast, since filling the grid control
			// with all bound commands is already very slow
			if (lpszCommand[0] == _T('\0'))
				continue;

			CHelpKeyGridRow* pRow = new CHelpKeyGridRow(pGrid);
			pRow->m_nCmdID = nCmdID;
			pRow->m_cells[0] = szCategory;
			pRow->m_cells[1] = lpszCommand;	// this copies to a CString, so we are not really holding onto lpszCommand
			pRow->m_cells[2] = CString(szPrefix) + CTap::MakeAccelName(acc);
			GetCmdDescription(nCmdID, pRow->m_cells[3]);
			pGrid->AddRow(pRow, FALSE);
		}
		else
		{
			AddKeymapToGrid(pGrid, pSubKeymap, szCategory, 
				CTap::MakeAccelName(acc) + ", ", kc);
		}
	}
}

void CHelpKeyWnd::FillGrid()
{
	CWaitCursor waitCursor;

	CHelpKeyGrid* pGrid = (CHelpKeyGrid*)GetDlgItem(AFX_IDW_PANE_FIRST);
	
	ASSERT(pGrid->m_rows.GetCount() == 1);

	POSITION pos = CKeymap::c_keymaps.GetHeadPosition();
	while (pos != NULL)
	{
		CKeymap* pKeymap = (CKeymap*)(void*)CKeymap::c_keymaps.GetNext(pos);
		if (pKeymap->m_strName.IsEmpty())
			continue;
		AddKeymapToGrid(pGrid, pKeymap, pKeymap->m_strName, NULL, 0);
	}

	pGrid->ResetSize();
	pGrid->Invalidate(FALSE);
	pGrid->ResetScrollBars();

	pGrid->SizeColumnToFit(2);
	pGrid->SizeColumnToFit(1);
	pGrid->SizeColumnToFit(0);
}

void CHelpKeyWnd::InitGroupNames()
{
	for (int i = 0; i < m_pToolGroups->m_nGroups; i++)
	{
		if (m_pToolGroups->m_rgGroups[i].m_nCmds == 0)
			continue;

		int index = m_cbGroup.AddString(m_pToolGroups->m_rgGroups[i].m_strGroup);
		m_cbGroup.SetItemData(index, i);
	}
}

void CHelpKeyWnd::EnumCmdTable(CGridWnd* pGrid, int nGroup)
{
	CToolGroup* m_pGroup = &(m_pToolGroups->m_rgGroups[nGroup]);

	for (int i = 0; i < m_pGroup->m_nCmds; i++)
	{
		// get the command
		CTE* pCTE = (CTE*)m_pGroup->m_aCmds[i];
		// get the command's name
		LPCTSTR pszCmdName = pCTE->szCommand;

		// only add commands that have a name and are not marked NOKEY
		if (pszCmdName && pszCmdName[0]!=_T('\0') && !(pCTE->flags & CT_NOKEY)) {

			CHelpKeyGridRow* pRow = new CHelpKeyGridRow(pGrid);

			pRow->m_cells[0] = m_pToolGroups->m_rgGroups[nGroup].m_strGroup;
			pRow->m_nCmdID = pCTE->id;
			pRow->m_cells[1] = pszCmdName;
			GetCmdKeyStringAll(pCTE->id, pRow->m_cells[2]);
			GetCmdDescription(pCTE->id, pRow->m_cells[3]);
			pGrid->AddRow(pRow, FALSE);

		}
	}
}

void CHelpKeyWnd::FillGridCmds(int nGroup)
{
	CHelpKeyGrid* pGrid = (CHelpKeyGrid*)GetDlgItem(AFX_IDW_PANE_FIRST);

	// are we doing all groups?
	if (nGroup == -1) {
		for (int i = 0; i < m_pToolGroups->m_nGroups; i++) {
			if (m_pToolGroups->m_rgGroups[i].m_nCmds > 0) {
				EnumCmdTable(pGrid, i);
			}
		}
	}
	else {
		EnumCmdTable(pGrid, nGroup);
	}

	pGrid->ResetSize();
	pGrid->Invalidate(FALSE);
	pGrid->ResetScrollBars();

	pGrid->SizeColumnToFit(2);
	pGrid->SizeColumnToFit(1);
	pGrid->SizeColumnToFit(0);
}

////////////////////////////////////////////////////////////////////////////

CHelpKeyGridRow::CHelpKeyGridRow(CGridWnd* pGrid) : CGridRow(pGrid)
{
	m_cyHeight = pGrid->GetCaptionRow()->GetRowHeight();
}

int CHelpKeyGridRow::Compare(const CGridRow* pCompareRow) const
{
	int nPrimarySortColumn = GetGrid()->m_nSortColumn;
	int dif=m_cells[nPrimarySortColumn].Compare(((CHelpKeyGridRow*)pCompareRow)->m_cells[nPrimarySortColumn]);

	if(dif!=0) {
		return dif;
	}

	int nSecondarySortColumn = 1; // always command name
	return m_cells[nSecondarySortColumn].Compare(((CHelpKeyGridRow*)pCompareRow)->m_cells[nSecondarySortColumn]);
}

void CHelpKeyGridRow::DrawCell(CDC* pDC, const CRect& cellRect, int nColumn)
{
	CRect rect = cellRect;

	if (nColumn == 3)
	{
		CGridWnd* pGrid = GetGrid();
		
		rect.bottom -= 1; // adjust for separator line
		
		pDC->ExtTextOut(0, 0, ETO_OPAQUE, &rect, "", 0, NULL);

		rect.InflateRect(-1, -1);
		pDC->DrawText(m_cells[nColumn], m_cells[nColumn].GetLength(), rect, DT_NOPREFIX | (nColumn == 3 ? DT_WORDBREAK : 0));
		rect.InflateRect(1, 1);
	}
	else
	{	
		pDC->ExtTextOut(rect.left, rect.top, ETO_CLIPPED | ETO_OPAQUE,
			rect, m_cells[nColumn], m_cells[nColumn].GetLength(), NULL);
	}

	CBrush* pOldBrush = pDC->SelectObject(CDC::GetHalftoneBrush());
	pDC->PatBlt(cellRect.left, cellRect.bottom - 1, cellRect.Width(), 1, PATCOPY);
	pDC->SelectObject(pOldBrush);
}

void CHelpKeyGridRow::ResetSize(CDC* pDC)
{
	m_cyHeight = GetCellSize(pDC, 3).cy;
}

CSize CHelpKeyGridRow::GetCellSize(CDC* pDC, int nColumn) const
{
	int cxColumn = GetGrid()->GetColumnWidth(nColumn);
	CSize size;
	if (nColumn == 3 && cxColumn > 0)
	{
		CRect rect(0, 0, cxColumn, 0);
		pDC->DrawText(m_cells[nColumn], m_cells[nColumn].GetLength(), rect, DT_CALCRECT | DT_NOPREFIX | (nColumn == 3 ? DT_WORDBREAK : 0));
		size = rect.Size();
	}
	else
	{
		size = pDC->GetTextExtent(m_cells[nColumn]);
	}
	size.cx += 10;
	size.cy += 3;

	return size;
}

////////////////////////////////////////////////////////////////////////////

CHelpKeyGrid::CHelpKeyGrid(int nSortColumn) : CGridWnd(4, TRUE)
{
	m_nSortColumn = nSortColumn;
}

////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnHelpKeyboard()
{
	CHelpKeyWnd* pHelpKeyWnd = new CHelpKeyWnd;
	pHelpKeyWnd->Create();

	// Deleted in CFrameWnd::PostNcDestroy().
}
