///////////////////////////////////////////////////////////////////////////////
//	SFIND.CPP
//		Implementation for general find architecture.

#include "stdafx.h"
#include "find.h"
#include "remi.h"
#include "resource.h"
#include "findspec.h"
#include "utilauto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


// these strings are defined in profile.cpp
extern char BASED_CODE szSearch[];
extern char BASED_CODE szSearchFind[];
extern char BASED_CODE szSearchReplace[];
extern char BASED_CODE szEmpty[];


FINDREPLACEDISK AFX_EXT_DATADEF findReplace;
CPickList thePickList;

CPickList::CPickList(void)
{
	for (int j = FIND_PICK; j <= REPLACE_PICK; j++)
	{
		nbInPick[j] = 0;
		for (int i = 0; i < MAX_PICK_LIST; i++)
			aaszPickList[j][i] = 0;
	}
}

CPickList::~CPickList(void)
{
	//Free find and replace pick lists
	for (int j = FIND_PICK; j <= REPLACE_PICK; j++)
	{
		for (int i = 0; i < nbInPick[j]; i++)
			delete [] aaszPickList[j][i];
	}
}

FINDREPLACEDISK::FINDREPLACEDISK()
{
	//Initialize Replace/Find structure
	matchCase  = FALSE;
	regExpr = FALSE;
	bWasLineMatch = FALSE;
	wholeWord = FALSE;
	goUp = FALSE;
	loop = TRUE;
	startFromCursor = TRUE;
	pat = NULL;

	findWhat[0] = '\0';
	replaceWith[0] = '\0';
}

FINDREPLACEDISK::~FINDREPLACEDISK()
{
	DisposeOfPattern();
}

void FINDREPLACEDISK::DisposeOfPattern()
{
	if(pat != NULL) 
	{
		free((void *) pat);
		pat = NULL;
	}
}

BOOL FINDREPLACEDISK::CompileRegEx(reSyntax syntax)
{
	if(!regExpr)
		return FALSE;

	DisposeOfPattern();

	//For unknown reasons RE Engine take care of the dynamic Allocs
	//but you have to do the freeing ...
	//Compile the regular expression, 
	//(WARNING : RECompile can return NULL)
	pat = RECompile(findWhat, (flagType)matchCase, syntax);

  return (pat != NULL);
}

reSyntax	g_reCurrentSyntax = reUnix;

BOOL SetRegExprSyntax(reSyntax _reSyntax)
{
	ASSERT(reUnix <= _reSyntax && _reSyntax <= reEpsilon );
	g_reCurrentSyntax = _reSyntax;
	return TRUE;
}

reSyntax GetRegExprSyntax(void)
{
	ASSERT(reUnix <= g_reCurrentSyntax && g_reCurrentSyntax <= reEpsilon );
  return g_reCurrentSyntax;
}

/////////////////////////////////////////////////////////////////////////////
// CButtonEdit
BEGIN_MESSAGE_MAP(CButtonEdit, CEdit)
	//{{AFX_MSG_MAP(CButtonEdit)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CButtonEdit::CButtonEdit()
{
	m_dwLastSel = 0;
}

void CButtonEdit::ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo)
{
	SetSel(m_dwLastSel); // Restore previous selection
	CEdit::ReplaceSel(lpszNewText, bCanUndo);
}

/////////////////////////////////////////////////////////////////////////////
// CButtonEdit message handlers

void CButtonEdit::OnKillFocus(CWnd *pNewWnd)
{
	m_dwLastSel = GetSel();
	CEdit::OnKillFocus(pNewWnd);
}

/////////////////////////////////////////////////////////////////////////////



CPickList &ShellPickList(void)
{
	return thePickList;
}

void CPickList::AllocateEntry(PickListType type, UINT iIndex)
{
	ASSERT(!aaszPickList[type][iIndex]); // this should be null!
	aaszPickList[type][iIndex] = new TCHAR[LIMIT_FIND_COMBO_TEXT + 1];
}

void CPickList::DeallocateEntry(PickListType type, UINT iIndex)
{
	ASSERT(aaszPickList[type][iIndex]); // this should never be nonzero!
	delete [] aaszPickList[type][iIndex];
	aaszPickList[type][iIndex] = 0;
}

BOOL CPickList::InsertEntry(PickListType type, FINDREPLACEDISK *pfr)
{
	if (type == FIND_PICK)
		return InsertEntry(type,pfr->findWhat);
	else
		return InsertEntry(type,pfr->replaceWith);
}

BOOL CPickList::InsertEntry(PickListType type, LPCTSTR newString)
{
	BOOL found = FALSE;
	int nb = nbInPick[type];
	int i;

	// Don't insert an empty string
	if(newString[0] == '\0')
		return FALSE;

	//First check if string is not already in list
	for (i = 0; i < nb; i++)
	{
		if (found = (_ftcscmp(aaszPickList[type][i], newString) == 0))
			break;
	}

	if (found)
	{
		//String already exists, move it to first place
		if (i > 0)
		{
			char szTmp[LIMIT_FIND_COMBO_TEXT + 1];

			_ftcscpy((LPSTR)szTmp, aaszPickList[type][i]);
			for (int j = i; j > 0; j--)
				_ftcscpy(aaszPickList[type][j], aaszPickList[type][j - 1]);
			_ftcscpy(aaszPickList[type][0], (LPSTR)szTmp);
		}
	}
	else
	{
		//String not found, do we have a new space to create ?
		if (nb < MAX_PICK_LIST)
		{
			AllocateEntry(type, nb);
			nb++;
		}

		//Shift list
		for (i = nb - 1; i >= 1; i--)
			_ftcscpy(aaszPickList[type][i], aaszPickList[type][i - 1]);
		_ftcscpy(aaszPickList[type][0], newString);
	}

	nbInPick[type] = nb;

	UpdateFindCombos();
	return !found;
}

void CPickList::RemoveEntry(PickListType type)
{
	if (nbInPick[type] <= 0)
		return;

	//Free the first element
	DeallocateEntry(type,0);

	//Shift the pick list
	for (int i = 1; i < nbInPick[type]; i++)
		aaszPickList[type][i - 1] = aaszPickList[type][i];

#ifdef _DEBUG
	int nLastEntry = nbInPick[type] - 1; // the last is at index [count - 1]
  aaszPickList[type][nLastEntry] = 0; // for our error checking in the allocation code
#endif

	nbInPick[type]--;

	UpdateFindCombos();
}

void CPickList::LoadFromRegistry(FINDREPLACEDISK *pfr)
{
	int	type;   // FIND_PICK or REPLACE_PICK
	int		iLoop;
	LPSTR	lpText;
	CString	str;
	char *	szBaseKey;
	char	szKeyName[32];		// Needs to be big enough to accomodate Find16 and Replace16

	for (type = FIND_PICK; type <= REPLACE_PICK; type++) {
		
		if(type == FIND_PICK)
			szBaseKey = szSearchFind;
		else
			szBaseKey = szSearchReplace;

		for (iLoop = 0; iLoop < MAX_PICK_LIST; iLoop ++) 
		{
			sprintf(szKeyName, szBaseKey, iLoop + 1);
			AllocateEntry(static_cast<PickListType>(type), iLoop);
			VERIFY (lpText = aaszPickList[type][iLoop]);
			str = GetRegString(szSearch, szKeyName);
			_ftcscpy(lpText, str);
			if(iLoop == 0)
			{
				if(type == FIND_PICK)  // So that F3 will work right off the bat.
					_ftcscpy(pfr->findWhat, str);
				else
					_ftcscpy(pfr->replaceWith, str);
			}
		}
		nbInPick[type] = MAX_PICK_LIST;
	}
}

void CPickList::SaveToRegistry(void)
{
	int	type;   // FIND_PICK or REPLACE_PICK
	int		iLoop;
	LPSTR	lpText;
	char *	szBaseKey;
	char	szKeyName[32];		// Needs to be big enough to accomodate Find16 and Replace16

	for (type = FIND_PICK; type <= REPLACE_PICK; type++) {
		
		if(type == FIND_PICK)
			szBaseKey = szSearchFind;
		else
			szBaseKey = szSearchReplace;

		for (iLoop = 0; iLoop < nbInPick[type]; iLoop++) 
		{
			sprintf(szKeyName, szBaseKey, iLoop + 1);
			VERIFY(lpText = aaszPickList[type][iLoop]);
			WriteRegString(szSearch, szKeyName, lpText);
		}
		for ( ; iLoop < MAX_PICK_LIST; iLoop ++) 
		{
			sprintf (szKeyName, szBaseKey, iLoop + 1 );
			WriteRegString(szSearch, szKeyName, szEmpty);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFindStringDlg dialog
//

CFindStringDlg::CFindStringDlg(LONG lStyle, CWnd* pParent /*=NULL*/)
	: C3dDialog(IDD_FIND, pParent)
{
	//{{AFX_DATA_INIT(CFindStringDlg)
	m_bMatchCase = findReplace.matchCase;
	m_bRegExp = findReplace.regExpr;
	m_bWholeWord = findReplace.wholeWord;
	m_nDirection = findReplace.goUp ? 0 : 1;
	m_bSearchAllDocs = !findReplace.loop;
	m_strFindString = "";
	//}}AFX_DATA_INIT

	m_lStyle = lStyle;
	m_bInitString = FALSE;

	// If we don't support some of the find modifiers, then clear
	// the value, so the checkbox won't show up checked.

	if ((m_lStyle & FDS_MATCHCASE) == 0)
		m_bMatchCase = FALSE;
	if ((m_lStyle & FDS_REGEXP) == 0)
		m_bRegExp = FALSE;
	if ((m_lStyle & FDS_WHOLEWORD) == 0)
		m_bWholeWord = FALSE;
	if ((m_lStyle & FDS_SEARCHALL) == 0)
		m_bSearchAllDocs = FALSE;
}

CFindStringDlg::~CFindStringDlg()
{
	if (m_bInitString)
	{
		LPCSTR lpsz;
		ShellPickList().RemoveEntry(FIND_PICK);	// Remove the guess.
		// And restore the findReplace value
		VERIFY(lpsz = ShellPickList().GetEntry(FIND_PICK,0));
		_tcscpy(findReplace.findWhat, lpsz);
	}
}

void CFindStringDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindStringDlg)
	DDX_Check(pDX, IDC_FIND_MATCHUPLO, m_bMatchCase);
	DDX_Check(pDX, IDC_FIND_REGEXP, m_bRegExp);
	DDX_Check(pDX, IDC_FIND_WHOLEWORD, m_bWholeWord);
	DDX_Check(pDX, IDC_FIND_SEARCH_ALL, m_bSearchAllDocs);
	DDX_Radio(pDX, IDC_FIND_UP, m_nDirection);
	DDX_CBString(pDX, IDC_FIND_WHAT, m_strFindString);
	DDX_Control(pDX, IDC_FIND_NEXT, m_btnFindNext);
	//}}AFX_DATA_MAP
}

BOOL CFindStringDlg::OnInitDialog()
{
	if(_tcscmp(m_strFindString, findReplace.findWhat) != 0)
	{
		if (m_strFindString.IsEmpty())
			m_strFindString = findReplace.findWhat;
		else
		{
			m_bInitString = TRUE;

			_tcscpy(findReplace.findWhat, (const _TCHAR*)m_strFindString);
			ShellPickList().InsertEntry(FIND_PICK, &findReplace);
		}
	}
	C3dDialog::OnInitDialog();
	
	LPCSTR lpsz;
	CWnd *pwndCombo;
	pwndCombo = GetDlgItem(IDC_FIND_WHAT);
	ASSERT(pwndCombo != NULL);
	::SendMessage(pwndCombo->m_hWnd, CB_LIMITTEXT, LIMIT_FIND_COMBO_TEXT, 0);
	::SendMessage(pwndCombo->m_hWnd, CB_RESETCONTENT, 0, 0);

	for (int i = 0 ; i < ShellPickList().GetEntryCount(FIND_PICK); i++)
	{
		VERIFY(lpsz = ShellPickList().GetEntry(FIND_PICK,i));
		if(lpsz[0] != '\0')	// Do not add empty strings.
			::SendMessage(pwndCombo->m_hWnd, CB_ADDSTRING, 0, (LPARAM)lpsz);
	}

	::SendMessage(pwndCombo->m_hWnd, CB_SETEXTENDEDUI, TRUE, 0);
	::SendMessage(pwndCombo->m_hWnd, CB_SETCURSEL, 0, 0);
	OnSelChangeFindWhat();

	if ((m_lStyle & FDS_WHOLEWORD) == 0)
		GetDlgItem(IDC_FIND_WHOLEWORD)->EnableWindow(FALSE);
	if ((m_lStyle & FDS_MATCHCASE) == 0)
		GetDlgItem(IDC_FIND_MATCHUPLO)->EnableWindow(FALSE);
	if ((m_lStyle & FDS_REGEXP) == 0)
		GetDlgItem(IDC_FIND_REGEXP)->EnableWindow(FALSE);
	if ((m_lStyle & FDS_SEARCHALL) == 0)
		GetDlgItem(IDC_FIND_SEARCH_ALL)->EnableWindow(FALSE);
	// Mark all window gets enabled if it is supported AND we are not searching
	// across documents.
	GetDlgItem(IDC_FIND_MARK_ALL)->EnableWindow((m_lStyle & FDS_MARKALL) && !m_bSearchAllDocs);

	m_btnSpecial.SubclassDlgItem(IDC_SPECIAL, this);
	VERIFY(m_btnSpecial.LoadMenu(IDR_FIND_SPECIAL));
	
	// Now subclass the edit control, part of the combo Box
	// so that we can remember where its selection is.
	m_comboEdit.SubclassDlgItem(1001, pwndCombo);

	UpdateButtons(!m_strFindString.IsEmpty());

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFindStringDlg::UpdateState()
{
	if (m_bInitString)
	{
		ShellPickList().RemoveEntry(FIND_PICK);
		m_bInitString = FALSE;
	}

	UpdateData();

	ASSERT(!m_strFindString.IsEmpty());
	_ftcscpy(findReplace.findWhat, (const _TCHAR *)m_strFindString);
	ShellPickList().InsertEntry(FIND_PICK, &findReplace);

	// Only reset global flags if the dialog support user input to these flags.
	if (m_lStyle & FDS_MATCHCASE)
		findReplace.matchCase = m_bMatchCase;
	if (m_lStyle & FDS_REGEXP)
		findReplace.regExpr = m_bRegExp;
	if (m_lStyle & FDS_WHOLEWORD)
		findReplace.wholeWord = m_bWholeWord;
	if (m_lStyle & FDS_SEARCHALL)
		findReplace.loop = !m_bSearchAllDocs;
	findReplace.goUp = (m_nDirection == 0);
	findReplace.startFromCursor = TRUE;
}

BEGIN_MESSAGE_MAP(CFindStringDlg, C3dDialog)
	//{{AFX_MSG_MAP(CFindStringDlg)
	ON_BN_CLICKED(IDC_FIND_NEXT, OnClickedFindNext)
	ON_BN_CLICKED(IDC_FIND_MARK_ALL, OnClickedMarkAll)
	ON_BN_CLICKED(IDC_FIND_SEARCH_ALL, OnClickedSearchAll)
	ON_CBN_EDITCHANGE(IDC_FIND_WHAT, OnEditChangeFindWhat)
	ON_CBN_SELCHANGE(IDC_FIND_WHAT, OnSelChangeFindWhat)
	ON_CBN_SELENDOK(IDC_FIND_WHAT, OnSelChangeFindWhat)
	ON_COMMAND_RANGE(ID_FIND_ANYCHARACTER, ID_FIND_STRING, OnSpecialItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFindStringDlg message handlers

void CFindStringDlg::OnClickedFindNext()
{
	UpdateState();
	HWND hwndView = theApp.GetActiveView();
	ASSERT(hwndView != NULL);
	CPartView* pView = (CPartView*) FromHandlePermanent(hwndView);
	ASSERT(pView != NULL && pView->IsKindOf(RUNTIME_CLASS(CPartView)));

	if(findReplace.pat != NULL) 
	{
		free((void *) findReplace.pat);
		findReplace.pat = NULL;
	}

	BOOL bFound;
	pView->OnFindNext(&bFound);

	if (bFound)
		EndDialog(IDC_FIND_NEXT);
	else
		GetDlgItem(IDC_FIND_WHAT)->SetFocus();
}

void CFindStringDlg::OnSpecialItem(UINT nId)
{
	if (FindSpecialHandler(m_comboEdit,nId))
		((CButton *)GetDlgItem(IDC_FIND_REGEXP))->SetCheck(1);
	OnEditChangeFindWhat(); // to properly the state of the Find Next button
}


void CFindStringDlg::OnClickedMarkAll()
{
	UpdateState();
	HWND hwndView = theApp.GetActiveView();
	ASSERT(hwndView != NULL);
	CPartView* pView = (CPartView*) FromHandlePermanent(hwndView);
	ASSERT(pView != NULL && pView->IsKindOf(RUNTIME_CLASS(CPartView)));
	BOOL bFound;

	{
		// DS #10524 [CFlaat]: we don't want to record this preparatory find operation
		HALT_RECORDING();
		pView->OnFindNext(&bFound);
	}
	if(bFound && pView->MarkAll(&findReplace))
		EndDialog(IDC_FIND_NEXT);
	else
		GetDlgItem(IDC_FIND_WHAT)->SetFocus();
}
void CFindStringDlg::UpdateButtons(BOOL fEnable)
{
	m_btnFindNext.EnableWindow(fEnable);
	if(m_lStyle & FDS_MARKALL)
		GetDlgItem(IDC_FIND_MARK_ALL)->EnableWindow(!m_bSearchAllDocs && 
													fEnable);
}

void CFindStringDlg::OnEditChangeFindWhat()
{
	UpdateData();
	if(m_strFindString.GetLength()>LIMIT_FIND_COMBO_TEXT)
	{
		m_strFindString = m_strFindString.Left(LIMIT_FIND_COMBO_TEXT);
		UpdateData(FALSE);
	}
	UpdateButtons(!m_strFindString.IsEmpty());
}

void CFindStringDlg::OnSelChangeFindWhat()
{
	UpdateData();
	UpdateButtons(TRUE);
}

void CFindStringDlg::OnClickedSearchAll()
{
	UpdateData();
	UpdateButtons(!m_strFindString.IsEmpty());
}
