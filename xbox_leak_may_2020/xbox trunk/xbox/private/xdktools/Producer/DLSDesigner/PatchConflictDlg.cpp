// PatchConflictDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "PatchConflictDlg.h"
#include "DlsDefsPlus.h"
#include "Collection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "GMStrings.cpp"

/////////////////////////////////////////////////////////////////////////////
// CPatchConflictDlg dialog


CPatchConflictDlg::CPatchConflictDlg(HRESULT hrCode, 
									 CInstrument* pCollidingInstrument,
									 CCollection* pLoadingCollection, 
									 CInstrument* pParentInstrument /*=NULL*/ ) : CDialog(IDD_PATCH_CONFLICT),
m_hrConflictCause(hrCode),
m_pCollidingInstrument(pCollidingInstrument),
m_pLoadingCollection(pLoadingCollection),
m_pInstrument(pParentInstrument),
m_bTouchedByUser(false)
{
	//{{AFX_DATA_INIT(CPatchConflictDlg)
	m_Patch = 0;
	m_LSB = 0;
	m_MSB = 0;
	m_Drum = FALSE;
	//}}AFX_DATA_INIT
	m_iRetVal = 0;
}


void CPatchConflictDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchConflictDlg)
	DDX_Control(pDX, IDC_CONFLICT_NOT_AGAIN_CHECK, m_NotAgainCheck);
	DDX_Text(pDX, IDC_CONFLICT_PATCH, m_Patch);
	DDV_MinMaxUInt(pDX, m_Patch, 0, 127);
	DDX_Text(pDX, IDC_CONFLICT_LSB, m_LSB);
	DDV_MinMaxUInt(pDX, m_LSB, 0, 127);
	DDX_Text(pDX, IDC_CONFLICT_MSB, m_MSB);
	DDV_MinMaxUInt(pDX, m_MSB, 0, 127);
	DDX_Check(pDX, IDC_DRUM, m_Drum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPatchConflictDlg, CDialog)
	//{{AFX_MSG_MAP(CPatchConflictDlg)
	ON_EN_CHANGE(IDC_CONFLICT_LSB, OnChangeLSB)
	ON_EN_CHANGE(IDC_CONFLICT_MSB, OnChangeMSB)
	ON_EN_CHANGE(IDC_CONFLICT_PATCH, OnChangePatch)
	ON_BN_CLICKED(IDC_BTN_FIX, OnBtnFix)
	ON_BN_CLICKED(IDC_BTN_FIX_ALL, OnBtnFixAll)
	ON_BN_CLICKED(IDC_BTN_IGNORE_ALL, OnBtnIgnoreAll)
	ON_BN_CLICKED(IDC_DRUM, OnDrum)
	ON_BN_CLICKED(ID_BTN_IGNORE, OnBtnIgnore)
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_CONFLICT_NOT_AGAIN_CHECK, OnConflictNotAgainCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPatchConflictDlg::InitDlgText()
{
	CString		csDlgPrompt;
	CString		csCatStr;
	CWnd*		pEdit;
	m_MSB = (m_pInstrument->m_rInstHeader.Locale.ulBank & 0x00007F00) >> 8;
	m_LSB = m_pInstrument->m_rInstHeader.Locale.ulBank & 0x0000007F;
	m_Patch = m_pInstrument->m_rInstHeader.Locale.ulInstrument;
	m_Drum = ((m_pInstrument->m_rInstHeader.Locale.ulBank & 0x80000000) == 0) ? FALSE : TRUE;
	
	UpdateData(FALSE);
	//Construct the correct message for the dialog's static text

	csDlgPrompt.LoadString(IDS_PATCH_CONFLICT_COMMON);
	if (m_hrConflictCause & SC_PATCH_DUP_GM)
	{
		csCatStr.LoadString(IDS_PATCH_CONFLICT_GM);
		//To Do: load GM name?
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
	}
	csDlgPrompt += csCatStr; 
	CWnd* pPromptText = GetDlgItem(IDC_CONFLICT_DESC_TEXT);
	if ( pPromptText )
	{
		pPromptText->SetWindowText(csDlgPrompt.GetBuffer(0));
	}
	pEdit = GetDlgItem(IDC_CONFLICT_NAME);
	if (pEdit)
	{
		pEdit->SetWindowText(m_pInstrument->m_Info.m_csName.GetBuffer(0));
		pEdit = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPatchConflictDlg message handlers

BOOL CPatchConflictDlg::OnInitDialog() 
{
	InitDlgText();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPatchConflictDlg::OnChangeLSB() 
{
	m_bTouchedByUser = true;
}

void CPatchConflictDlg::OnChangeMSB() 
{
	m_bTouchedByUser = true;	
}

void CPatchConflictDlg::OnChangePatch() 
{
	m_bTouchedByUser = true;
}

void CPatchConflictDlg::OnDrum() 
{
	m_bTouchedByUser = true;
}

void CPatchConflictDlg::OnBtnFix() 
{

	CInstrument*	pDummy;
	HRESULT			hr;

	if (m_bTouchedByUser)
	{
		if (UpdateData(TRUE))
		{
			m_bTouchedByUser = false;
			//test patch
			hr = m_pInstrument->m_pComponent->IsValidPatch(m_pInstrument, MAKE_BANK(m_Drum, m_MSB, m_LSB), m_Patch, NULL, &pDummy);
			if ( !SUCCEEDED(hr) )
			{
				//To Do: message notifying user to try again or press fix to auto fix
				return;
			}
		}
		else
		{
			return;
		}
	}
	else
	{
		hr = m_pInstrument->m_pComponent->GetNextValidPatch((DWORD)m_Drum, (BYTE*)&m_MSB, (BYTE*)&m_LSB, (BYTE*)&m_Patch);
		if ( FAILED(hr) )
		{
			//To Do: notify user all valid patches are taken.
			//hr = DLS_NO_VALID_PATCHES;
			//EndDialog(IDCANCEL);
			EndDialog(DLS_NO_VALID_PATCHES);
		}
	}
	m_pInstrument->m_rInstHeader.Locale.ulBank = MAKE_BANK(m_Drum, m_MSB, m_LSB);
	m_pInstrument->m_rInstHeader.Locale.ulInstrument = m_Patch;
	EndDialog(IDOK);
}

void CPatchConflictDlg::OnBtnFixAll() 
{
	//To Do: Check if touched?
	//auto fix
	m_pInstrument->m_pComponent->GetNextValidPatch((DWORD)m_Drum, (BYTE*)&m_MSB, (BYTE*)&m_LSB, (BYTE*)&m_Patch);
	m_pInstrument->m_rInstHeader.Locale.ulBank = MAKE_BANK(m_Drum, m_MSB, m_LSB);
	m_pInstrument->m_rInstHeader.Locale.ulInstrument = m_Patch;
	m_hrConflictCause = S_OK;
	//return autofix
	EndDialog(DLS_DLG_FIX_ALL);
}

void CPatchConflictDlg::OnBtnIgnoreAll() 
{
	if (m_hrConflictCause & SC_PATCH_DUP_GM)
	{
		m_hrConflictCause &= ~SC_PATCH_DUP_GM;
		m_iRetVal |= DLS_DLG_IGNORE_ALL_GM;
	}
	if (m_hrConflictCause & SC_PATCH_DUP_COLLECTION)
	{
		m_hrConflictCause &= ~SC_PATCH_DUP_COLLECTION;
		m_iRetVal |= DLS_DLG_IGNORE_ALL_COLLECTION;
	}
	if (m_hrConflictCause & SC_PATCH_DUP_SYNTH)
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

void CPatchConflictDlg::OnBtnIgnore() 
{
	if (m_hrConflictCause & SC_PATCH_DUP_GM)
	{
		m_hrConflictCause &= ~SC_PATCH_DUP_GM;
	}
	else if (m_hrConflictCause & SC_PATCH_DUP_COLLECTION)
	{
		m_hrConflictCause &= ~SC_PATCH_DUP_COLLECTION;
	}
	else if (m_hrConflictCause & SC_PATCH_DUP_SYNTH)
	{
		m_hrConflictCause &= ~SC_PATCH_DUP_SYNTH;
	}
	if (DLS_ERR_FIRST == m_hrConflictCause)
	{
		EndDialog(IDCANCEL);
	}
	else
	{
		InitDlgText();
		return;
	}
}

void CPatchConflictDlg::GetGMInstName(CString & strResult)
{
	BInstr* pArr = m_Drum ? &abiDrums[0] : &abiGS[0];
	ASSERT(pArr);
	if(pArr == NULL)
		return;
	
	while(pArr->nStringId != IDS_PGMEND)
	{
		if ((pArr->bPatch == m_Patch) && (pArr->bMSB == m_MSB) &&	(pArr->bLSB == m_LSB))
		{
			// This is the one we want!
			strResult.LoadString(pArr->nStringId);
			break;
		}

		pArr++;
	}
}




void CPatchConflictDlg::OnConflictNotAgainCheck() 
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return;
	}

	CDLSComponent* pComponent = m_pInstrument->m_pComponent;
	ASSERT(pComponent);
	if(pComponent == NULL)
	{
		return;
	}

	int nCheck = m_NotAgainCheck.GetCheck();
	bool bShow = nCheck == 0 ? false : true;
	pComponent->SetCollectionPatchConflict(bShow);
}
