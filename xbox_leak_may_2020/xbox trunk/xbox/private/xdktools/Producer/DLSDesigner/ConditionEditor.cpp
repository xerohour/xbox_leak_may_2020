// ConditionEditor.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "ConditionEditor.h"
#include "ConditionalChunk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConditionEditor dialog


CConditionEditor::CConditionEditor(CWnd* pParent /*=NULL*/, CDLSComponent* pComponent, CConditionalChunk* pConditionalChunk, CInstrument* pInstrument)
	: CDialog(CConditionEditor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConditionEditor)
	//}}AFX_DATA_INIT

	ASSERT(pComponent);
	m_pComponent = pComponent;
	m_pInstrument = pInstrument;
	m_pConditionalChunk = pConditionalChunk;
}


void CConditionEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConditionEditor)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConditionEditor, CDialog)
	//{{AFX_MSG_MAP(CConditionEditor)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConditionEditor message handlers

void CConditionEditor::OnOK() 
{
	CString sExpression;
	m_ScriptEdit.GetWindowText(sExpression);

	if(m_pConditionalChunk)
	{
		CString sOldExpression = m_pConditionalChunk->GetCondition();
		if(FAILED(m_pConditionalChunk->RegularToRPN(sExpression)))
		{
			m_pConditionalChunk->RegularToRPN(sOldExpression);
			return;
		}
		
		// Set the old expression so we can save the undo state correctly....
		m_pConditionalChunk->RegularToRPN(sOldExpression);
		if(m_pInstrument)
		{
			m_pInstrument->SaveStateForUndo(IDS_UNDO_CONDITION);
		}

		// Set the new expression....
		m_pConditionalChunk->RegularToRPN(sExpression);

		CDialog::OnOK();
	}
}


BOOL CConditionEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Subclass the edit control
	m_ScriptEdit.SubclassDlgItem(IDC_SCRIPT_EDIT, this);
	m_ScriptEdit.SetEditor(this);
	
	if(m_pConditionalChunk)
	{
		CString sCondition = m_pConditionalChunk->GetCondition();
		int nLength = sCondition.GetLength();
		m_ScriptEdit.SetWindowText(sCondition);	
		m_ScriptEdit.SetSel(nLength, nLength);
	}
	else
	{
		m_ScriptEdit.EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConditionEditor::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	ShowPopupMenu(pWnd, point);
}


void CConditionEditor::ShowPopupMenu(CWnd* pWnd, CPoint point)
{
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
		return;

	// should this happen?
	if(pWnd == NULL)
		return;
	
	CMenu popupMenu;
	if(popupMenu.CreatePopupMenu() == FALSE)
	{
		return;
	}

	CString sQueriesPopupMenu;
	sQueriesPopupMenu.LoadString(IDS_QUERIES);
	CMenu menuQueries;
	if(menuQueries.CreatePopupMenu() == FALSE)
	{
		return;
	}

	CString sOperatorsPopupMenu;
	sOperatorsPopupMenu.LoadString(IDS_OPERATORS);
	CMenu menuOperators;
	if(menuOperators.CreatePopupMenu() == FALSE)
	{
		return;
	}

	// Get the list of queries	
	const CPtrList& lstDLSQueries = m_pComponent->GetListOfQueries();
	if(lstDLSQueries.IsEmpty())
	{
		return;
	}

	int nID = 0;
	POSITION position = lstDLSQueries.GetHeadPosition();
	while(position)
	{
		CDLSQuery* pDLSQuery = (CDLSQuery*) lstDLSQueries.GetNext(position);
		ASSERT(pDLSQuery);
		if(pDLSQuery)
		{
			menuQueries.AppendMenu(MF_STRING, nID++, pDLSQuery->GetName());
		}

	}

	// Add the operators
	for(int nIndex = 0; nIndex < MAX_OPERATORS; nIndex++)
	{
		Opcode opcode = CConditionalChunk::m_arrOpcodes[nIndex];
		CString sSymbol = opcode.m_sSymbol;
		
		// Add escape sequences for '&' characters
		sSymbol.Replace("&", "&&");

		CString sHelpString = opcode.m_sHelpString;
		CString sMenuItem = sSymbol + "\t" + sHelpString;

		menuOperators.AppendMenu(MF_STRING, nID++, sMenuItem);
	}

	popupMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT) menuQueries.m_hMenu, sQueriesPopupMenu);
	popupMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT) menuOperators.m_hMenu, sOperatorsPopupMenu);

	popupMenu.TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, this);

	menuQueries.Detach();
	menuOperators.Detach();

}

BOOL CConditionEditor::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// This command is from the menu
	if(HIWORD(wParam) == 0 && lParam == 0)
	{
		WORD wID = LOWORD(wParam);
		WriteToExpression(wID);		
		
		return TRUE;
	}
	
	return CDialog::OnCommand(wParam, lParam);
}


void CConditionEditor::WriteToExpression(WORD wID)
{
	const CPtrList& lstDLSQueries = m_pComponent->GetListOfQueries();
	if(lstDLSQueries.IsEmpty())
	{
		return;
	}

	CString sCurrentExpression;
	m_ScriptEdit.GetWindowText(sCurrentExpression);
	sCurrentExpression += " ";

	// Try the queries first
	if(wID < lstDLSQueries.GetCount())
	{
		int nID = 0;
		POSITION position = lstDLSQueries.GetHeadPosition();
		while(position)
		{
			CDLSQuery* pDLSQuery = (CDLSQuery*) lstDLSQueries.GetNext(position);
			if(nID == wID)
			{
				m_ScriptEdit.ReplaceSel(" " + pDLSQuery->GetName());
				return;
			}
			nID++;
		}
	}
	else
	{
		int nOperatorIndex = wID - lstDLSQueries.GetCount();
		if( nOperatorIndex >= 0 && nOperatorIndex <= MAX_OPERATORS)
		{
			Opcode opcode = CConditionalChunk::m_arrOpcodes[nOperatorIndex];
			m_ScriptEdit.ReplaceSel(" " + opcode.m_sSymbol);
		}
	}
}