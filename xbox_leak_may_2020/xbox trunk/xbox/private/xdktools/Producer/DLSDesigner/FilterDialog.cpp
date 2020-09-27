// FilterDialog.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "filterdialog.h"
#include "Collection.h"
#include "Instrument.h"
#include "Region.h"
#include "Articulation.h"
#include "DLSStatic.h"
#include "InstrumentFVEditor.h"
#include "DlsDefsPlus.h"
#include "UndoMan.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterDialog dialog


CFilterDialog::CFilterDialog(CArticulation* pArticulation):
CDialog(), CSliderCollection(7),
m_pArticParams(pArticulation->GetArticParams()),
m_pCollection(NULL), m_pmsInitialFc(NULL)
{
	m_pArticulation = pArticulation;

#ifdef _DEBUG	
	pArticulation->ValidateParent();
#endif

	CInstrument* pInstrument = pArticulation->GetInstrument();
	if(pInstrument)
	{
		// Articulation is owned by an instrument			
		m_pCollection = pInstrument->m_pCollection;
	}
	else
	{
		// Articulation is owned by a region
		CRegion* pRegion = pArticulation->GetRegion();
		ASSERT(pRegion);
		if(pRegion)
		{
			pInstrument = pRegion->GetInstrument();
			ASSERT(pInstrument);
			m_pCollection = pInstrument->m_pCollection;
		}
	}

	CSliderCollection::Init(this);

	//{{AFX_DATA_INIT(CFilterDialog)
	//}}AFX_DATA_INIT
}


void CFilterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterDialog)
	DDX_Control(pDX, IDC_ENABLE_FILTER, m_EnableFilterCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilterDialog, CDialog)
	//{{AFX_MSG_MAP(CFilterDialog)
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_ENABLE_FILTER, OnEnableFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterDialog message handlers

void CFilterDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pArticParams);
	if(m_pArticParams == NULL)
	{
		return;
	}

	CSliderCollection::OnHScroll(nSBCode, nPos, pScrollBar);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CFilterDialog::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CDialog::OnInitDialog();
	
	m_FilterParams = m_pArticParams->m_FilterParams;
	InitSliders();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CFilterDialog::InitSliders()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_FilterParams.m_pcInitialFc == 0x7FFFFFFF)
	{
		m_EnableFilterCheck.SetCheck(FALSE);
		EnableSliders(FALSE);
	}
	else
	{
		// Enable the filter controls
		m_EnableFilterCheck.SetCheck(TRUE);
		EnableSliders(TRUE);
	}

	if (m_pmsInitialFc)
		{
		m_pmsInitialFc->SetValue(this, m_FilterParams.m_pcInitialFc);
		m_pmsInitialQ->SetValue(this, m_FilterParams.m_gcInitialQ);
		m_pmsLFOToFc->SetValue(this, m_FilterParams.m_pcModLFOToFc);
		m_pmsLFOCC1ToFc->SetValue(this, m_FilterParams.m_pcModLFOCC1ToFc);
		m_pmsEG2ToFc->SetValue(this, m_FilterParams.m_pcEGToFc);
		m_pmsKeyVelToFc->SetValue(this, m_FilterParams.m_pcVelocityToFc);
		m_pmsKeyNumToFc->SetValue(this, m_FilterParams.m_pcKeyNumToFc);
		}
	else
		{
		m_pmsInitialFc = Insert(
			IDC_FILTER_INITIALFC,
			IDC_FILTER_DINITIALFC,
			IDC_FILTER_DINITIALFC_SPIN,
			MYSLIDER_FILTERABSPITCH,
			0x7FFFFFFF,
			IDS_UNDO_INITIAL_FC,
			&m_FilterParams.m_pcInitialFc);

		m_pmsInitialQ = Insert(
			IDC_FILTER_INITIALQ,
			IDC_FILTER_DINITIALQ,
			IDC_FILTER_DINITIALQ_SPIN,
			MYSLIDER_FILTERGAIN,
			0,
			IDS_UNDO_INITIAL_Q,
			&m_FilterParams.m_gcInitialQ);

		m_pmsLFOToFc = Insert(
			IDC_FILTER_LFOTOFC,
			IDC_FILTER_DLFOTOFC,
			IDC_FILTER_DLFOTOFC_SPIN,
			MYSLIDER_FILTERPITCHCENTS,
			0,
			IDS_UNDO_LFO_TO_FC,
			&m_FilterParams.m_pcModLFOToFc);

		m_pmsLFOCC1ToFc = Insert(
			IDC_FILTER_CC1TOFC,
			IDC_FILTER_DCC1TOFC,
			IDC_FILTER_DCC1TOFC_SPIN,
			MYSLIDER_FILTERPITCHCENTS,
			0,
			IDS_UNDO_LFOCC1_TO_FC,
			&m_FilterParams.m_pcModLFOCC1ToFc);

		m_pmsEG2ToFc = Insert(
			IDC_FILTER_ENV2TOFC,
			IDC_FILTER_DENV2TOFC,
			IDC_FILTER_DENV2TOFC_SPIN,
			MYSLIDER_FILTERPITCHCENTS,
			0,
			IDS_UNDO_EG2_TO_FC,
			&m_FilterParams.m_pcEGToFc);

		m_pmsKeyVelToFc = Insert(
			IDC_FILTER_VELTOFC,
			IDC_FILTER_DVELTOFC,
			IDC_FILTER_DVELTOFC_SPIN,
			MYSLIDER_FILTERPITCHCENTS,
			0,
			IDS_UNDO_KEYVEL_TO_FC,
			&m_FilterParams.m_pcVelocityToFc);

		m_pmsKeyNumToFc = Insert(
			IDC_FILTER_KEYNUMTOFC,
			IDC_FILTER_DKEYNUMTOFC,
			IDC_FILTER_DKEYNUMTOFC_SPIN,
			MYSLIDER_FILTERPITCHCENTS,
			0,
			IDS_UNDO_KEYNUM_TO_FC,
			&m_FilterParams.m_pcKeyNumToFc);
			}

	BOOL bDLS1  = m_pArticulation->IsDLS1();
	EnableDLS2Controls(!bDLS1);
}

HRESULT CFilterDialog::SaveUndoState(UINT uStringRes)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CInstrumentFVEditor* pParent = (CInstrumentFVEditor*) GetParent();
	ASSERT(pParent);

	if(pParent && pParent->m_pInstrument)
	{
		if(FAILED(pParent->m_pInstrument->SaveStateForUndo(uStringRes)))
			return E_FAIL;
	}
	else
		return E_FAIL;

	return S_OK;
}

void CFilterDialog::UpdateArticulation(CArticulation* pNewArt)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(pNewArt);
	if(pNewArt == NULL)
	{
		return;
	}

	m_pArticParams = pNewArt->GetArticParams();
	m_pArticulation = pNewArt;

#ifdef _DEBUG	
	pNewArt->ValidateParent();
#endif

	CInstrument* pInstrument = pNewArt->GetInstrument();
	if(pInstrument)
	{
		// Articulation is owned by an instrument			
		m_pCollection = pInstrument->m_pCollection;
	}
	else
	{
		// Articulation is owned by a region
		CRegion* pRegion = pNewArt->GetRegion();
		ASSERT(pRegion);
		pInstrument = pRegion->GetInstrument();
		ASSERT(pInstrument);
		m_pCollection = pInstrument->m_pCollection;
	}

	m_FilterParams = m_pArticParams->m_FilterParams;
	InitSliders();
}

void CFilterDialog::CollectStatics()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Remove all previous statics....should there be any?
	while(!m_lstStatics.IsEmpty())
	{
		CDLSStatic* pStatic = (CDLSStatic*) m_lstStatics.RemoveHead();
		ASSERT(pStatic);
		if(pStatic)
		{
			delete pStatic;
		}
	}
	
	CWnd* pChild = GetWindow(GW_CHILD);
	while(pChild)
	{
		char szClassName[MAX_PATH];
		::GetClassName(pChild->m_hWnd, szClassName, MAX_PATH);
		
		// Add it to the list if it's a "Static"
		if(strcmp(szClassName,"Static") == 0)
		{
			CDLSStatic* pDLSStatic = NULL;
			if(SUCCEEDED(CDLSStatic::CreateControl(this, pChild, &pDLSStatic)))
				m_lstStatics.AddTail(pDLSStatic);
		}

		pChild = pChild->GetNextWindow();
	}

	// Destroy the actual static controls now
	POSITION position = m_lstStatics.GetHeadPosition();
	while(position)
	{
		CDLSStatic* pStatic = (CDLSStatic*) m_lstStatics.GetNext(position);
		ASSERT(pStatic);
		CWnd* pWnd = GetDlgItem(pStatic->GetID());
		if(pWnd)
		{
			pWnd->DestroyWindow();
		}
	}
}


void CFilterDialog::OnDestroy() 
{
	CDialog::OnDestroy();

	// Remove all previous statics....should there be any?
	while(!m_lstStatics.IsEmpty())
	{
		CDLSStatic* pStatic = (CDLSStatic*) m_lstStatics.RemoveHead();
		ASSERT(pStatic);
		if(pStatic)
		{
			delete pStatic;
		}
	}
}

void CFilterDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	POSITION position = m_lstStatics.GetHeadPosition();
	while(position)
	{
		CDLSStatic* pStatic = (CDLSStatic*) m_lstStatics.GetNext(position);
		ASSERT(pStatic);
		pStatic->OnDraw(&dc);
	}
}

void CFilterDialog::OnEnableFilter() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pArticulation);
	if(m_pArticulation == NULL)
	{
		return;
	}

	int nState = m_EnableFilterCheck.GetCheck();
	if(nState == 0)
	{
		EnableSliders(FALSE);
		m_FilterParams.m_pcInitialFc = 0x7FFFFFFF;
	}
	else
	{
		EnableSliders(TRUE);
		m_FilterParams.m_pcInitialFc = m_pmsInitialFc->GetValue();
	}

	// Update the articulation
	UpdateInstrument();
}


void CFilterDialog::EnableDLS2Controls(BOOL bEnable)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_EnableFilterCheck.EnableWindow(bEnable);
	int nCheck = m_EnableFilterCheck.GetCheck();
	if(nCheck && bEnable)
	{	
		EnableSliders(TRUE);
	}
	else
	{
		EnableSliders(FALSE);
	}
}


void CFilterDialog::EnableSliders(BOOL bEnable)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// enable/disable only if controls have been created
	if (m_pmsInitialFc == NULL)
		return;

	m_pmsInitialFc->EnableControl(this, bEnable != 0);
	m_pmsInitialQ->EnableControl(this, bEnable != 0);
	m_pmsLFOToFc->EnableControl(this, bEnable != 0);
	m_pmsLFOCC1ToFc->EnableControl(this, bEnable != 0);
	m_pmsEG2ToFc->EnableControl(this, bEnable != 0);
	m_pmsKeyVelToFc->EnableControl(this, bEnable != 0);
	m_pmsKeyNumToFc->EnableControl(this, bEnable != 0);
}

BOOL CFilterDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	LRESULT lResult;
	if (CSliderCollection::OnCommand(wParam, lParam, &lResult))
		return lResult;

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CFilterDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LRESULT lResult;
	if (CSliderCollection::OnNotify(wParam, lParam, &lResult))
		return lResult;
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CFilterDialog::UpdateInstrument()
{
	m_pArticParams->m_FilterParams = m_FilterParams;
	CInstrument* pInstrument = m_pArticulation->GetInstrument();
	if(pInstrument)
	{
		pInstrument->UpdateInstrument();
		if(pInstrument->m_pCollection)
		{
			pInstrument->m_pCollection->SetDirtyFlag();
		}
	}
	else if (m_pArticulation->GetRegion())
	{
		CRegion* pRegion = m_pArticulation->GetRegion();
		pInstrument = pRegion->GetInstrument();
		ASSERT(pInstrument);
		if(pInstrument)
		{
			pInstrument->UpdateInstrument();
			if(pInstrument->m_pCollection)
			{
				pInstrument->m_pCollection->SetDirtyFlag();
			}
		}
	}
}

bool CFilterDialog::OnSliderUpdate(MySlider *pms, DWORD dwmscupdf)
{
	switch (dwmscupdf)
		{
		case dwmscupdfStart:
			if (SUCCEEDED(SaveUndoState(pms->GetUndoStringID())))
				return true;
			UpdateArticulation(m_pArticulation);
			return false;

		case dwmscupdfEnd:
			UpdateInstrument();
			return true;

		default:
			ASSERT(FALSE);
			return false;
		}
}

