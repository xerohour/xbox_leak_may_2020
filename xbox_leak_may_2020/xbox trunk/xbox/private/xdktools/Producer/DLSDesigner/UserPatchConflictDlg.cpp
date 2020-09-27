// UserPatchConflictDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "UserPatchConflictDlg.h"
#include "DlsDefsPlus.h"
#include "Collection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BInstr abiGS[];
extern BInstr abiDrums[];
extern const DWORD  dwabiGSInstruments;
extern const DWORD dwabiDrums;

/////////////////////////////////////////////////////////////////////////////
// CUserPatchConflictDlg dialog

CUserPatchConflictDlg::CUserPatchConflictDlg(HRESULT hrCode, ULONG ulBank, ULONG ulPatch, CInstrument* pCollidingInstrument)
	: CDialog(CUserPatchConflictDlg::IDD),
m_ulCheckedBank(ulBank),
m_ulCheckedPatch(ulPatch),
m_pCollidingInstrument(pCollidingInstrument),
m_hrConflictCause(hrCode)
{
	//{{AFX_DATA_INIT(CUserPatchConflictDlg)
	m_bIgnoreAll = FALSE;
	//}}AFX_DATA_INIT
}


void CUserPatchConflictDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserPatchConflictDlg)
	DDX_Control(pDX, IDC_CONFLICT_NOT_AGAIN_CHECK, m_NotAgainCheck);
	DDX_Check(pDX, IDC_IGNORE_CHECK, m_bIgnoreAll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserPatchConflictDlg, CDialog)
	//{{AFX_MSG_MAP(CUserPatchConflictDlg)
	ON_BN_CLICKED(IDC_CONFLICT_NOT_AGAIN_CHECK, OnConflictNotAgainCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CUserPatchConflictDlg::InitDlgText()
{
	CString		csDlgPrompt;
	CString		csCatStr;
	CString		csCheckStr;
	CWnd*		pEdit;
	
	//Construct the correct message for the dialog's static text

	csDlgPrompt.LoadString(IDS_ERR_PATCH);
	if (m_hrConflictCause & SC_PATCH_DUP_GM)
	{
		csCatStr.LoadString(IDS_PATCH_CONFLICT_GM);
		
		pEdit = GetDlgItem(IDC_CONFLICTING_INS);
		if (pEdit)
		{
			CString strInstName;
			GetGMInstName(strInstName);
			pEdit->SetWindowText(strInstName);
			pEdit = NULL;
		}

		pEdit = GetDlgItem(IDC_CONFLICT_COLLECTION);
		if (pEdit)
		{
			pEdit->SetWindowText(_T("GM"));
			pEdit = NULL;
		}
		csCheckStr.LoadString(IDS_IGNORE_CHECK_GM);
	}
	else if (m_hrConflictCause & SC_PATCH_DUP_COLLECTION)
	{
		csCatStr.LoadString(IDS_PATCH_CONFLICT_COLLECTION);
		pEdit = GetDlgItem(IDC_CONFLICTING_INS);
		if (pEdit)
		{
			pEdit->SetWindowText(m_pCollidingInstrument->m_Info.m_csName.GetBuffer(0));
			pEdit = NULL;
		}
		pEdit = GetDlgItem(IDC_CONFLICT_COLLECTION);
		if (pEdit && m_pCollidingInstrument->m_pCollection)
		{
			pEdit->SetWindowText(m_pCollidingInstrument->m_pCollection->m_Info.m_csName.GetBuffer(0));
			pEdit = NULL;
		}

		csCheckStr.LoadString(IDS_IGNORE_CHECK_COLLECTION);
	}
	else if (m_hrConflictCause & SC_PATCH_DUP_SYNTH)
	{
		csCatStr.LoadString(IDS_PATCH_CONFLICT_GENERAL);
		pEdit = GetDlgItem(IDC_CONFLICT_COLLECTION);
		if (pEdit)
		{
			pEdit->SetWindowText(m_pCollidingInstrument->m_pCollection->m_Info.m_csName.GetBuffer(0));
			pEdit = NULL;
		}
		pEdit = GetDlgItem(IDC_CONFLICTING_INS);
		if (pEdit)
		{
			pEdit->SetWindowText(m_pCollidingInstrument->m_Info.m_csName.GetBuffer(0));
			pEdit = NULL;
		}
		csCheckStr.LoadString(IDS_IGNORE_CHECK_SYNTH);
	}
	csDlgPrompt += csCatStr; 
	CWnd* pPromptText = GetDlgItem(IDC_CONFLICT_DESC_TEXT);
	if ( pPromptText )
	{
		pPromptText->SetWindowText(csDlgPrompt.GetBuffer(0));
		pPromptText = NULL;
	}
	
	pPromptText = GetDlgItem(IDC_IGNORE_CHECK);
	if ( pPromptText )
	{
		pPromptText->SetWindowText(csCheckStr.GetBuffer(0));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CUserPatchConflictDlg message handlers

BOOL CUserPatchConflictDlg::OnInitDialog() 
{
	InitDlgText();
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserPatchConflictDlg::OnCancel() 
{
	if (!m_bIgnoreAll)
	{
		if (m_hrConflictCause & SC_PATCH_DUP_GM)
		{
			m_hrConflictCause &= ~SC_PATCH_DUP_GM;
			m_iRetVal |= DLS_DLG_IGNORE_ALL_GM;
		}
		else if (m_hrConflictCause & SC_PATCH_DUP_COLLECTION)
		{
			m_hrConflictCause &= ~SC_PATCH_DUP_COLLECTION;
			m_iRetVal |= DLS_DLG_IGNORE_ALL_COLLECTION;
		}
		else if (m_hrConflictCause & SC_PATCH_DUP_SYNTH)
		{
			m_hrConflictCause &= ~SC_PATCH_DUP_SYNTH;
			m_iRetVal |= DLS_DLG_IGNORE_ALL_SYNTH;
		}
		if (DLS_ERR_FIRST == m_hrConflictCause)
		{
			EndDialog(m_iRetVal);
		}
		else
		{
			InitDlgText();
			return;
		}
	}
	else
	{
		EndDialog(IDCANCEL);
	}
}

void CUserPatchConflictDlg::GetGMInstName(CString& strResult)
{
	BYTE bMSB = (BYTE) ((m_ulCheckedBank & 0x00007F00) >> 8);
	BYTE bLSB = (BYTE) (m_ulCheckedBank & 0x0000007F);
	BYTE bPatch = (BYTE)(m_ulCheckedPatch & 0x0000007F);
	BYTE dwDrum =  ((m_ulCheckedBank & 0x80000000) == 0) ? FALSE: TRUE;
	
	BInstr* pArr = dwDrum ? &abiDrums[0] : &abiGS[0];
	ASSERT(pArr);
	if(pArr == NULL)
		return;

	while(pArr->nStringId != IDS_PGMEND)
	{
		if ((pArr->bPatch == bPatch) && (pArr->bMSB == bMSB) &&	(pArr->bLSB == bLSB))
		{
			// This is the one we want!
			strResult.LoadString(pArr->nStringId);
			break;
		}

		pArr++;
	}
}

void CUserPatchConflictDlg::OnConflictNotAgainCheck() 
{
	ASSERT(m_pCollidingInstrument);
	if(m_pCollidingInstrument == NULL)
	{
		return;
	}

	CDLSComponent* pComponent = m_pCollidingInstrument->m_pComponent;
	ASSERT(pComponent);
	if(pComponent == NULL)
	{
		return;
	}

	int nCheck = m_NotAgainCheck.GetCheck();
	bool bShow = nCheck == 0 ? false : true;
	pComponent->SetUserPatchConflict(bShow);
}
