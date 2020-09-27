// ..\qwatch.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CStringArray CQuickW::s_rgstr;
extern DWORD dwFuncTimeOut;


/////////////////////////////////////////////////////////////////////////////
// CQWGridEdit window

class CQWGridEdit : public CEdit
{
    // Construction
    public:
    	CQWGridEdit(CQWGrid *);

    // Attributes
    public:
    	CQWGrid * m_pGrid;

    // Operations
    public:

    // Overrides
    	// ClassWizard generated virtual function overrides
    	//{{AFX_VIRTUAL(CQWGridEdit)
    	//}}AFX_VIRTUAL

    // Implementation
    public:
    	virtual ~CQWGridEdit();

    	// Generated message map functions
    protected:
    	//{{AFX_MSG(CQWGridEdit)
    	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    	afx_msg UINT OnGetDlgCode();
    	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    	//}}AFX_MSG

    	DECLARE_MESSAGE_MAP()
};


CQWGridEdit::CQWGridEdit(CQWGrid *pGrid)
	:  CEdit(), m_pGrid(pGrid)
{
}

CQWGridEdit::~CQWGridEdit()
{
}


BEGIN_MESSAGE_MAP(CQWGridEdit, CEdit)
	//{{AFX_MSG_MAP(CQWGridEdit)
	ON_WM_CREATE()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CQWGridEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// REVIEW: Figure out what we need to do to deal with Ctl3d here.
	
	return 0;
}

UINT CQWGridEdit::OnGetDlgCode()
{
	return CEdit::OnGetDlgCode() | DLGC_WANTTAB | DLGC_WANTARROWS | DLGC_WANTALLKEYS;
}

void CQWGridEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE || nChar == VK_F2)
		m_pGrid->ProcessKeyboard((MSG*)GetCurrentMessage());
	else
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// CQWGridRow

class CQWGridRow : public CWatchRow
{
// Construction.
public:
	CQWGridRow(CQWGrid *pGrid, CTM *pTM, CQWGridRow *pParent = NULL );
	~CQWGridRow();
	typedef CWatchRow CBaseClass;

	virtual CWatchRow * NewWatchRow(CTreeGridWnd *, CTM *, CTreeGridRow *);
	
	virtual void OnModify();
	virtual void OnSelect(BOOL bSelected);
	virtual CEdit * NewEdit( );	
};

CQWGridRow::CQWGridRow(CQWGrid *pGrid, CTM *pTM, CQWGridRow *pParent /* = NULL*/)
	: CBaseClass(pGrid, pTM, pParent)
{
}	

CQWGridRow::~CQWGridRow( )
{
}

void CQWGridRow::OnModify()
{
	CQWGrid *pGrid = (CQWGrid *)GetTreeGrid();
	pGrid->OnUpdateDebug(0,0);
}

CWatchRow * CQWGridRow::NewWatchRow(CTreeGridWnd *pGrid, CTM *pTM, CTreeGridRow *pRow)
{
	return new CQWGridRow((CQWGrid *)pGrid, pTM, (CQWGridRow *)pRow);
}

void CQWGridRow::OnSelect(BOOL bSelected)
{
	CBaseClass::OnSelect(bSelected);

	if (bSelected)
	{
		CQuickW * pDlg = ((CQWGrid *)m_pGrid)->m_pDlg;
		pDlg->OnSelChanged(this);
	}
}

CEdit* CQWGridRow::NewEdit( )
{
	CQWGridEdit *pEdit = new CQWGridEdit((CQWGrid *)GetGrid());
	
	return pEdit;
}
			
/////////////////////////////////////////////////////////////////////////////
// CQWGrid

CQWGrid::CQWGrid()
	: CBaseClass("")
{
	m_bDockable = FALSE;
	m_bIsModified = FALSE;

	// turn off multiple selection in the quick watch grid only.
	EnableMultiSelect(FALSE);
	ShowSelection(FALSE);
}

CQWGrid::~CQWGrid()
{
}

// Adds a new expression to the quick watch window.
// RETURNS- .
// returns FALSE otherwise.

expr_status CQWGrid::AddNewExpression(const CString& str)
{
	ClearCurEntries();

	if (str.IsEmpty())
		return expr_illegal;

	CWaitCursor wc; // turn on a wait cursor

	// We can afford to have a longer time out for the quickwatch
	// window. The compiler backend guys need it because they
	// evaluate huge functions from the debugger.
	DWORD dwOldFuncTimeOut = dwFuncTimeOut;
	dwFuncTimeOut =  10 * 60 * 1000;

	CRootTM *pTM = new CRootTM(str);

	dwFuncTimeOut = dwOldFuncTimeOut;

	if (!pTM->CouldParseName())
	{
		delete pTM;
		return expr_illegal;
	}

	if (pTM->HadSideEffects())
	{
		::UpdateDebuggerState(UPDATE_ALLDBGWIN);
	}

	CWatchRow *pRow = new CQWGridRow(this, pTM);
	VERIFY(InsertTopLevelRow(pRow, FALSE));

	expr_status expr_return;

	if (pTM->IsValueValid() )
	{
		expr_return = expr_valid;
		// By default we always expand one-level if possible.
		if (pRow->IsExpandable())
			Expand(pRow, FALSE);
	}
	else
	{
		expr_return = expr_invalid;
	}
	
	Select(pRow);	// Select the first row that was added.	
	InvalidateGrid();
	return expr_return;
}
							
BOOL CQWGrid::ProcessKeyboard(MSG *pMsg, BOOL bPreTrans /* = FALSE */ )
{
	switch(pMsg->message)
	{
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		switch(pMsg->wParam)
		{
		
		case VK_RETURN:
			if (m_pControlWnd == NULL)
			{
				BOOL bMenu =  GetKeyState(VK_MENU) < 0;

				if (!bMenu)
				{
					CTreeGridRow *pRow = GetCurSel();
					
					if (pRow != NULL)
					{
						ToggleExpansion(pRow);
						return TRUE;
					}
				}
				
			}
			// fall through

		case VK_ESCAPE:
			if (m_pControlWnd == NULL)
			{
				CWnd *pParent = GetParent();
				
				if (pParent != NULL)
				{
					if (bPreTrans)
						pMsg->hwnd = pParent->m_hWnd;
					else
						pParent->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
				}

				return CGridWnd::ProcessKeyboard(pMsg, bPreTrans);
			}
			break;
						
		case VK_TAB:
			// control-tab is mdi window switching.
			if (GetKeyState(VK_CONTROL) < 0)
				break;

			if (!AcceptControl(FALSE))
				return TRUE;

			// BLOCK: Tab between elements, or dlg controls
			{
				CGridElement* pOldActive = m_pActiveElement;

				if (GetKeyState(VK_SHIFT) < 0)
					ActivatePrevControl();
				else
					ActivateNextControl();

				if (pOldActive == m_pActiveElement)
				{
					// We must be at the last (or first) element in the grid
					CloseActiveElement();

					CWnd* pParent = GetParent();
					// Move focus to the next/previous control.
					if (m_pDlg == pParent)
					{
						if (GetKeyState(VK_SHIFT) < 0)
							m_pDlg->PrevDlgCtrl();
						else
							m_pDlg->NextDlgCtrl();
					}
					return CGridWnd::ProcessKeyboard(pMsg, bPreTrans); // must skip CGridControlWnd
				}

				return TRUE;
			}
			break;
		case 'C':
			if (m_pControlWnd == NULL)
			{
				// No Alt, just Ctrl-C implies copy to clipboard.
				if (GetKeyState(VK_MENU) >= 0 && GetKeyState(VK_CONTROL) < 0)
				{
					SendMessage(WM_COMMAND, ID_EDIT_COPY);
					return TRUE;
				}
			}
			break;
		}
	}

	return CBaseClass::ProcessKeyboard(pMsg, bPreTrans);
}

LRESULT CQWGrid::OnUpdateDebug(WPARAM wParam, LPARAM lParam)
{
	m_bIsModified = TRUE;
	return CBaseClass::OnUpdateDebug(wParam, lParam);
}
																			
BEGIN_MESSAGE_MAP(CQWGrid, CQWGrid::CBaseClass)
	//{{AFX_MSG_MAP(CQWGrid)
	ON_WM_GETDLGCODE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CQWGrid message handlers

UINT CQWGrid::OnGetDlgCode()
{
	return (CBaseClass::OnGetDlgCode() | DLGC_WANTARROWS | DLGC_WANTALLKEYS);
}

int CQWGrid::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBaseClass::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;
	GetWindowRect(rc);
		
	int widthName = runDebugParams.iQWNameColWidth;

	// zero implies un-initialized, the second case could
	// happen if the user switches resolutions.
	if (widthName == 0 || widthName >= rc.Width())
	{
		widthName = (2 * rc.Width())/5;
	}

	SetColumnWidth(COL_NAME, widthName);
	
	
	return 0;
}

void CQWGrid::OnDestroy()
{
	runDebugParams.iQWNameColWidth = GetColumnWidth(COL_NAME);

	CBaseClass::OnDestroy();
}

void CQWGrid::OnKillFocus(CWnd* pNewWnd)
{
	ShowSelection(FALSE);
	CBaseClass::OnKillFocus(pNewWnd);
	
}

void CQWGrid::OnSetFocus(CWnd* pOldWnd)
{
	ShowSelection(TRUE);	
	CBaseClass::OnSetFocus(pOldWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CQuickW dialog


CQuickW::CQuickW(CWnd* pParent /*=NULL*/)
	: CBaseClass(CQuickW::IDD, pParent)
{
	m_qwGrid.m_pDlg = this;

	// If the find combo is active when the command is issued, remember the
	// string in the find combo.
	if (IsFindComboActive())
		GetFocus()->GetWindowText(m_strInit);
			
	//{{AFX_DATA_INIT(CQuickW)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

inline CTM * CQuickW::GetCurTM( )
{
	CQWGridRow *pRow = (CQWGridRow *)m_qwGrid.GetCurSel();
	
	if ( pRow )
	{
		return pRow->GetTM();
	}
	
	return NULL;
}

inline BOOL CQuickW::IsCurRowEditable()
{
	CTM *pTM = GetCurTM();

	if ( pTM ) return pTM->IsValueEditable();
	
	return FALSE;
}

// strips leading and trailing white space from a string.			
CString CQuickW::StripWhiteSpace(const CString& str)
{

	if (str.IsEmpty())
	{
		return str;
	}

	int lindex;
	for (lindex = 0 ; lindex < str.GetLength() ; lindex++ )
	{
		if (!whitespace(str[lindex]))
			break;
	}
	
	if (lindex == str.GetLength())
		return "";

	int rindex;
	for (rindex = str.GetLength() - 1 ; rindex >= 0; --rindex)
	{
		if (!whitespace(str[rindex]))
			break;
	}

	ASSERT(rindex >= lindex);

	return str.Mid(lindex, rindex - lindex + 1);
}

void CQuickW::ChangeDefButton(UINT nID)
{
	ASSERT(nID == IDCANCEL || nID == ID_QUICKW_NEWEXPR /* || nID == ID_QUICKW_MODIFY */);

	// If we are going to make this the default button it better be enabled.
	if ( !GetDlgItem(nID)->IsWindowEnabled() )
		nID = IDCANCEL;

	UINT oldDefID = LOWORD(GetDefID());

	if (oldDefID != nID)
	{
		// tell the old button it is no longer the default.
		if (oldDefID != 0)
		{
			CButton *pButton = ((CButton *)GetDlgItem(oldDefID));

			if ( pButton) pButton->SetButtonStyle(BS_PUSHBUTTON);
		}

		SetDefID(nID);
	}
}

void CQuickW::UpdateControls(UINT nForceID /* = 0 */ )
{
	UINT defID;
	BOOL bIsModifiable = m_bIsValidExpr && IsCurRowEditable();

	CComboBox * pComboExpr =  (CComboBox *)GetDlgItem(ID_QUICKW_EXPRESSION);
	int cbExpr = 0;
	cbExpr = pComboExpr->GetWindowTextLength();

	// the edit control might not have the text yet, but the list box
	// could have the element selected. Happens when we get a CBN_SELCHANGE
	// on dismissing the drop-down.

	if (cbExpr == 0)
	{
		int index = pComboExpr->GetCurSel();
		if ( index != CB_ERR )
			cbExpr = pComboExpr->GetLBTextLen(index);
	}
				
	GetDlgItem(ID_QUICKW_NEWEXPR)->EnableWindow(cbExpr > 0);

#if ENABLE_VALUE_EDIT	// FUTURE: remove if we don't re-introduce the value edit field.
	CWnd * pEditValue = GetDlgItem(ID_QUICKW_VALUE);
	CString strValue;
	pEditValue->GetWindowText(strValue);

	GetDlgItem(ID_QUICKW_MODIFY)->EnableWindow(bIsModifiable && !strValue.IsEmpty());
	GetDlgItem(ID_QUICKW_TXT_VALUE)->EnableWindow(bIsModifiable && DebuggeeAlive());
	pEditValue->EnableWindow(bIsModifiable && DebuggeeAlive());
#endif
	
	BOOL bHasRows = m_qwGrid.GetRowCount() != 0;
	m_qwGrid.EnableWindow(bHasRows);
	GetDlgItem(ID_QUICKW_TXT_MEMBERS)->EnableWindow(bHasRows);

	GetDlgItem(ID_QUICKW_ADD)->EnableWindow(cbExpr > 0 && DebuggeeAlive());

	if ( !m_bIsValidExpr && (cbExpr > 0))
	{
		defID = ID_QUICKW_NEWEXPR;
	}
	else if ( nForceID != 0 )
	{
		defID = nForceID;
	}

#if ENABLE_VALUE_EDIT
	else if ( bIsModifiable && !strValue.IsEmpty())
	{
		defID = ID_QUICKW_MODIFY;
	}
#endif
	else
	{
		defID = IDCANCEL;
	}

	ChangeDefButton(defID);
}
		
expr_status CQuickW::ReplaceQWExpr(const CString& str)
{
	if (str.IsEmpty())
		return expr_invalid;
		
	int iStr;
	CComboBox *pExprCombo = (CComboBox *)GetDlgItem(ID_QUICKW_EXPRESSION);

	for (iStr = 0 ; iStr < s_rgstr.GetSize(); ++iStr)
	{
		if (s_rgstr[iStr] == str)
		{
			// remove it from its current position.
			pExprCombo->DeleteString(iStr);
			s_rgstr.RemoveAt(iStr);
			break;
		}
	}
	
	// Insert the latest string at the beginining of the list.
	pExprCombo->InsertString(0, str);
	s_rgstr.InsertAt(0, str);
	
	expr_status expr_return = m_qwGrid.AddNewExpression(str);

	if ( expr_return == expr_valid )
	{
		m_bIsValidExpr = TRUE;
	}
	else
	{
		m_bIsValidExpr = FALSE;
	}

	return expr_return;
}

void CQuickW::DoDataExchange(CDataExchange* pDX)
{
	CBaseClass::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuickW)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuickW, CQuickW::CBaseClass)
	//{{AFX_MSG_MAP(CQuickW)
	ON_BN_CLICKED(ID_QUICKW_NEWEXPR, OnQuickwNewexpr)
	ON_CBN_EDITCHANGE(ID_QUICKW_EXPRESSION, OnCBEditChange)
	ON_CBN_SETFOCUS(ID_QUICKW_EXPRESSION, OnCBSetFocus)
	ON_CBN_SELCHANGE(ID_QUICKW_EXPRESSION, OnCBSelChange)
	ON_BN_CLICKED(ID_QUICKW_ADD, OnQuickwAdd)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CQuickW message handlers

BOOL CQuickW::OnInitDialog()
{
	CBaseClass::OnInitDialog();
	
	m_bIsValidExpr = FALSE;

	CComboBox * pExprCombo = (CComboBox *)GetDlgItem(ID_QUICKW_EXPRESSION);

	ASSERT(pExprCombo != NULL);

	VERIFY(pExprCombo->SetExtendedUI() == CB_OKAY);

	// Add previous quickwatch'd strings to the combo box.
	int iStr;
	for ( iStr = 0; iStr < s_rgstr.GetSize() ; ++iStr)
		VERIFY(pExprCombo->AddString(s_rgstr[iStr]) >= 0);
												
	// replace placeholder by the grid control.
	m_qwGrid.ReplaceControl(this, IDC_PLACEHOLDER, IDC_QW_GRID);
	
	CView   *pCurView = GetCurView();

	if (m_strInit.IsEmpty() && pCurView != NULL)
	{
		BOOL lookAround = TRUE;

		LPSTR lpStr = m_strInit.GetBuffer(MAX_USER_LINE);
		if (gpISrc->ViewBelongsToSrc(pCurView))
		{
			gpISrc->GetCurrentText(pCurView, &lookAround, lpStr, MAX_USER_LINE, NULL, NULL);
		}
		else if (pCurView->IsKindOf(RUNTIME_CLASS(CMultiEdit)))
		{
			CMultiEdit *pMultiEdit = (CMultiEdit *)pCurView;
			pMultiEdit->GetCurrentText(&lookAround, lpStr, MAX_USER_LINE, NULL, NULL );
		}

		m_strInit.ReleaseBuffer();
	}
	
	// Skip over any leading white space.
	m_strInit = StripWhiteSpace(m_strInit);
	
	BOOL bRet = FALSE;
	if (m_strInit.IsEmpty())
	{
		bRet = TRUE;
	}
	else
	{
		if (ReplaceQWExpr(m_strInit) == expr_valid )
		{
			// We might want to change this behavior to do
			// some intelligent determination if the user
			// explicitly picked the value to be qwatched,
			// (for ex: if the value is from the find combo)
			// and set the focus to the grid in that case.
			GotoDlgCtrl(GetDlgItem(ID_QUICKW_EXPRESSION));
		}
		else
			GotoDlgCtrl(GetDlgItem(ID_QUICKW_EXPRESSION));
	}

	return bRet;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CQuickW::OnQuickwNewexpr()
{
	CString strNew;
	CComboBox *pExprCombo = (CComboBox *)GetDlgItem(ID_QUICKW_EXPRESSION);

	pExprCombo->GetWindowText(strNew);
	strNew = StripWhiteSpace(strNew);
	
	if (strNew.IsEmpty())
	{
		GotoDlgCtrl(pExprCombo);
		return;
	}		
		
	switch ( ReplaceQWExpr(strNew) )
	{
		case expr_valid:
			m_qwGrid.SetFocus();
			break;
		case expr_illegal:
			ErrorBox(SYS_Cannot_Watch, strNew);		
			// fall through.
		case expr_invalid:
			GotoDlgCtrl(GetDlgItem(ID_QUICKW_EXPRESSION));
			break;
		default:
			ASSERT(FALSE);
	}

}


void CQuickW::OnSelChanged(CQWGridRow *pRow)
{
	ASSERT(m_qwGrid.IsSelected(pRow));

	CString str = pRow->GetTM()->GetExpr();
	if ( str.GetLength() == 0 )
		str = pRow->GetTM()->GetName();	// this could happen if the string is in error.


	SetDlgItemText(ID_QUICKW_EXPRESSION, str);
	SendMessage(WM_COMMAND, GET_WM_COMMAND_MPS(
								ID_QUICKW_EXPRESSION,
								GetDlgItem(ID_QUICKW_EXPRESSION)->GetSafeHwnd(),
								CBN_EDITCHANGE
							  ));
}


void CQuickW::OnOK()
{
	UINT nID = GetDefID();
	if (nID == ID_QUICKW_NEWEXPR)
		OnQuickwNewexpr();
#if ENABLE_VALUE_EDIT		
	else if (nID == ID_QUICKW_MODIFY)
		OnQuickwModify();
#endif
	else
	{
		ASSERT(nID == IDCANCEL);
		OnCancel();
	}
}

void CQuickW::OnCancel()
{
	// TODO: Add extra cleanup here
	
	CBaseClass::OnCancel();
}


void CQuickW::OnCBEditChange()
{
	if ( GetDlgItem(ID_QUICKW_EXPRESSION)->IsChild(GetFocus()) )	
		m_bIsValidExpr = FALSE;

	UpdateControls(ID_QUICKW_NEWEXPR);
}

void CQuickW::OnCBSetFocus()
{
	UpdateControls(ID_QUICKW_NEWEXPR);	
}

void CQuickW::OnCBSelChange()
{
	m_bIsValidExpr = FALSE;
	UpdateControls(ID_QUICKW_NEWEXPR);
}


void CQuickW::OnQuickwAdd()
{
	CString strExpr;
	GetDlgItem(ID_QUICKW_EXPRESSION)->GetWindowText(strExpr);

	CRootTM rootTM(strExpr);

	if (rootTM.CouldParseName())
	{
		// first close down the dialog.
		EndDialog(IDOK);

		if (CGridWatchView::EnsureWatchWindow())
			g_pWatchView->AddNewWatch(strExpr);
	}
	else
	{
		ErrorBox(SYS_Cannot_Watch, strExpr);
		GotoDlgCtrl(GetDlgItem(ID_QUICKW_EXPRESSION));
	}		

}

#if ENABLE_VALUE_EDIT

void CQuickW::OnQuickwModify()
{
	// TODO: Add your control notification handler code here
	
}

void CQuickW::OnENChange()
{
	UpdateControls(ID_QUICKW_MODIFY);
}

void CQuickW::OnENSetFocus()
{
	UpdateControls(ID_QUICKW_MODIFY);	
}

#endif

void CQuickW::OnDestroy()
{
	CBaseClass::OnDestroy();
	
	if (m_qwGrid.m_bIsModified)
		::UpdateDebuggerState(UPDATE_ALLDBGWIN);
}




