// ArticulationListPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "ArticulationListPropPageMgr.h"
#include "ArticulationListPropPage.h"
#include "ArticulationList.h"
#include "Articulation.h"
#include "ConditionalChunk.h"
#include "Collection.h"
#include "ConditionEditor.h"
#include <guiddlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArticulationListPropPage property page

IMPLEMENT_DYNCREATE(CArticulationListPropPage, CPropertyPage)

CArticulationListPropPage::CArticulationListPropPage() : CPropertyPage(CArticulationListPropPage::IDD), m_pArtList(NULL)
{
	//{{AFX_DATA_INIT(CArticulationListPropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CArticulationListPropPage::~CArticulationListPropPage()
{
}

void CArticulationListPropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArticulationListPropPage)
	DDX_Control(pDX, IDC_DLS1_CHECK, m_DLS1Check);
	DDX_Control(pDX, IDC_CONDITION_EDIT_BUTTON, m_EditButton);
	DDX_Control(pDX, IDC_ARTLIST_CONDITION_COMBO, m_ConditionCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArticulationListPropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CArticulationListPropPage)
	ON_BN_CLICKED(IDC_CONDITION_EDIT_BUTTON, OnConditionEditButton)
	ON_CBN_SELCHANGE(IDC_ARTLIST_CONDITION_COMBO, OnSelchangeArtlistConditionCombo)
	ON_CBN_DROPDOWN(IDC_ARTLIST_CONDITION_COMBO, OnDropdownArtlistConditionCombo)
	ON_BN_CLICKED(IDC_DLS1_CHECK, OnDls1Check)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArticulationListPropPage message handlers

BOOL CArticulationListPropPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CArticulationListPropPage::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pArtList == NULL)
	{
		EnableControls(FALSE);
		return CPropertyPage::OnSetActive();
	}

	EnableControls();
	InitConditionsCombo();

	m_DLS1Check.SetCheck(m_pArtList->IsDLS1());
	
	return CPropertyPage::OnSetActive();
}


void CArticulationListPropPage::EnableControls(BOOL bEnable)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CWnd* pCtrl = GetDlgItem(IDC_ARTLIST_CONDITION_COMBO);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bEnable);
	}

	pCtrl = GetDlgItem(IDC_CONDITION_EDIT_BUTTON);
	if(pCtrl)
	{
#ifndef DMP_XBOX
		pCtrl->EnableWindow(bEnable);
#else
		pCtrl->EnableWindow(FALSE);
#endif
	}

#ifdef DMP_XBOX
	pCtrl = GetDlgItem(IDC_ARTLIST_CONDITION_COMBO);
	if(pCtrl)
	{
		pCtrl->EnableWindow(FALSE);
	}
	pCtrl = GetDlgItem(IDC_ARTLIST_CONDITION_STATIC);
	if(pCtrl)
	{
		pCtrl->EnableWindow(FALSE);
	}
#endif // DMP_XBOX

	pCtrl = GetDlgItem(IDC_DLS1_CHECK);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bEnable);
	}
}

void CArticulationListPropPage::InitConditionsCombo()
{
	if(m_pArtList == NULL)
	{
		return;
	}

	m_ConditionCombo.ResetContent();

	CString sNone;
	sNone.LoadString(IDS_NONE);
	if(m_ConditionCombo.FindStringExact(0, sNone) == CB_ERR)
	{
		m_ConditionCombo.AddString(sNone);
	}

	CCollection* pCollection = m_pArtList->GetCollection();
	ASSERT(pCollection);
	CPtrList lstConditions;
	pCollection->GetListOfConditions(&lstConditions);

	POSITION position = lstConditions.GetHeadPosition();
	while(position)
	{
		CConditionalChunk* pCondition = (CConditionalChunk*)lstConditions.GetNext(position);
		ASSERT(pCondition);
		if(pCondition)
		{
			CString sConditionName = pCondition->GetName();
			if(m_ConditionCombo.FindStringExact(0, sConditionName) == CB_ERR)
			{
				m_ConditionCombo.AddString(sConditionName);
			}
		}
	}

	// Now set the condition for this articulation list
	CConditionalChunk* pConditionalChunk = m_pArtList->GetConditionalChunk();
	ASSERT(pConditionalChunk);
	if(pConditionalChunk)
	{
		CString sCurrentConditionName = pConditionalChunk->GetName();
		m_ConditionCombo.SelectString(0, sCurrentConditionName);
	}
}

void CArticulationListPropPage::OnConditionEditButton() 
{
	ASSERT(m_pArtList);
	if(m_pArtList == NULL)
	{
		EnableControls(FALSE);
		return;
	}

	CDLSComponent* pComponent = NULL;
	m_pArtList->GetComponent((IDMUSProdComponent**)&pComponent);
	ASSERT(pComponent);
	if(pComponent == NULL)
	{
		EnableControls(FALSE);
		return;
	}

	CConditionalChunk* pConditionalChunk = m_pArtList->GetConditionalChunk();
	ASSERT(pConditionalChunk);
	if(pConditionalChunk == NULL)
	{
		EnableControls(FALSE);
		return;
	}

	CInstrument* pInstrument = m_pArtList->GetInstrument();
	ASSERT(pInstrument);
	if(pInstrument == NULL)
	{
		return;
	}

	CConditionEditor conditionEditor(this, pComponent, pConditionalChunk, pInstrument);
	if(conditionEditor.DoModal() == IDOK)
	{
		// Add the new condition to the presets list
		InitConditionsCombo();
	}

	m_pArtList->CheckConfigAndRefreshNode();
}

void CArticulationListPropPage::OnSelchangeArtlistConditionCombo() 
{
	ASSERT(m_pArtList);
	if(m_pArtList == NULL)
	{
		return;
	}

	CString sCondition;
	m_ConditionCombo.GetWindowText(sCondition);
	if(FAILED(m_pArtList->SetCondition(sCondition)))
	{
		CString sNone;
		sNone.LoadString(IDS_NONE);
		UINT nConditionIndex = m_ConditionCombo.FindStringExact(0, sCondition);
		m_ConditionCombo.DeleteString(nConditionIndex);
		m_ConditionCombo.SelectString(0, sNone);
	}

	m_pArtList->CheckConfigAndRefreshNode();
}


void CArticulationListPropPage::OnDropdownArtlistConditionCombo() 
{
	InitConditionsCombo();
}

void CArticulationListPropPage::OnDls1Check() 
{
	if(m_pArtList == NULL)
	{
		return;
	}
	
	BOOL bDLS1 = m_DLS1Check.GetCheck();
	m_pArtList->SetDLS1(bDLS1);
}
