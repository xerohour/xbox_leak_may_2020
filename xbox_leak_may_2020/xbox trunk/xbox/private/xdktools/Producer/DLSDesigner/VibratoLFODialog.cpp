// VibratoLFODialog.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "vibratolfodialog.h"
#include "Collection.h"
#include "Instrument.h"
#include "Region.h"
#include "DLSStatic.h"
#include "InstrumentFVEditor.h"
#include "Articulation.h"
#include "DlsDefsPlus.h"
#include "UndoMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVibratoLFODialog dialog


CVibratoLFODialog::CVibratoLFODialog(CArticulation* pArticulation) :
CDialog(), CSliderCollection(5),
m_pArticParams(pArticulation->GetArticParams()), 
m_pCollection(NULL)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pArticulation);
	m_pArticulation = pArticulation;

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
		pInstrument = pRegion->GetInstrument();
		ASSERT(pInstrument);
		m_pCollection = pInstrument->m_pCollection;
	}

	CSliderCollection::Init(this);

	//{{AFX_DATA_INIT(CVibratoLFODialog)
	//}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CVibratoLFODialog, CDialog)
	//{{AFX_MSG_MAP(CVibratoLFODialog)
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVibratoLFODialog message handlers

void CVibratoLFODialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

BOOL CVibratoLFODialog::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CDialog::OnInitDialog();

	m_pmsLFOFrequency = Insert(
		IDC_LFO2_FREQUENCY, IDC_LFO2_DFREQUENCY, IDC_LFO2_FREQUENCY_SPIN,
		MYSLIDER_LFORANGE, FIVE_HERTZ, IDS_UNDO_LFO2_FREQ_EDIT,
		&m_LFOParams.m_pcFrequencyVibrato);

	m_pmsLFODelay = Insert(
		IDC_LFO2_DELAY, IDC_LFO2_DDELAY, IDC_LFO2_DELAY_SPIN,
		MYSLIDER_LFODELAY, Mils2TimeCents(0), IDS_UNDO_LFO2_DELAY_EDIT,
		&m_LFOParams.m_tcDelayVibrato);

	m_pmsLFOPitchScale = Insert(
		IDC_LFO2_PITCH, IDC_LFO2_DPITCH, IDC_LFO2_PITCH_SPIN,
		MYSLIDER_PITCHCENTS, 0, IDS_LFO_PITCH_UNDO_TEXT,
		&m_LFOParams.m_pcPitchScaleVibrato);
	
	m_pmsLFOMWToPitch = Insert(
		IDC_LFO2_MW2PITCH, IDC_LFO2_DMW2PITCH, IDC_LFO2_MW2PITCH_SPIN,
		MYSLIDER_PITCHCENTS, 0, IDS_UNDO_LFO2_MW2PITCH_EDIT,
		&m_LFOParams.m_pcMWToPitchVibrato);

	m_pmsLFOChanPressToPitch = Insert(
		IDC_LFO2_CHANPRESSTOPITCH, IDC_LFO2_DCHANPRESSTOPITCH, IDC_LFO2_CHANPRESSTOPITCH_SPIN,
		MYSLIDER_PITCHCENTS, 0, IDS_UNDO_LFO2_CHANPRESSTOPITCH_EDIT,
		&m_LFOParams.m_pcChanPressToPitchVibrato);

	CollectStatics();

	BOOL bDLS1 = m_pArticulation->IsDLS1();
	EnableDLS2Controls(!bDLS1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HRESULT CVibratoLFODialog::SaveUndoState(UINT uStringRes)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CInstrumentFVEditor* pParent = (CInstrumentFVEditor*) GetParent();
	ASSERT(pParent);

	if(pParent && pParent->m_pInstrument)
	{
		if(FAILED(pParent->m_pInstrument->SaveStateForUndo(uStringRes)))
		{
			return E_FAIL;
		}
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

void CVibratoLFODialog::UpdateArticulation(CArticulation* pNewArt)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
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

	m_LFOParams = m_pArticParams->m_LFO;
	m_pmsLFOFrequency->SetValue(this, m_LFOParams.m_pcFrequencyVibrato);
	m_pmsLFODelay->SetValue(this, m_LFOParams.m_tcDelayVibrato);
	m_pmsLFOPitchScale->SetValue(this, m_LFOParams.m_pcPitchScaleVibrato);
	m_pmsLFOMWToPitch->SetValue(this, m_LFOParams.m_pcMWToPitchVibrato);
	m_pmsLFOChanPressToPitch->SetValue(this, m_LFOParams.m_pcChanPressToPitchVibrato);

	BOOL bDLS1 = m_pArticulation->IsDLS1();
	EnableDLS2Controls(!bDLS1);
}

void CVibratoLFODialog::CollectStatics()
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


void CVibratoLFODialog::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
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

void CVibratoLFODialog::OnPaint() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CPaintDC dc(this); // device context for painting
	
	POSITION position = m_lstStatics.GetHeadPosition();
	while(position)
	{
		CDLSStatic* pStatic = (CDLSStatic*) m_lstStatics.GetNext(position);
		ASSERT(pStatic);
		pStatic->OnDraw(&dc);
	}
}

void CVibratoLFODialog::EnableDLS2Controls(BOOL bEnable)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pmsLFOFrequency->EnableControl(this, bEnable != 0);
	m_pmsLFODelay->EnableControl(this, bEnable != 0);
	m_pmsLFOPitchScale->EnableControl(this, bEnable != 0);
	m_pmsLFOMWToPitch->EnableControl(this, bEnable != 0);
	m_pmsLFOChanPressToPitch->EnableControl(this, bEnable != 0);
}

BOOL CVibratoLFODialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	LRESULT lResult;
	if (CSliderCollection::OnCommand(wParam, lParam, &lResult))
		return lResult;

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CVibratoLFODialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LRESULT lResult;
	if (CSliderCollection::OnNotify(wParam, lParam, &lResult))
		return lResult;
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CVibratoLFODialog::UpdateInstrument()
{
	m_pArticParams->m_LFO = m_LFOParams;
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
		ASSERT(pRegion);
		pInstrument = pRegion->GetInstrument();
		ASSERT(pInstrument);
		pInstrument->UpdateInstrument();
		if(pInstrument->m_pCollection)
		{
			pInstrument->m_pCollection->SetDirtyFlag();
		}
	}
}

bool CVibratoLFODialog::OnSliderUpdate(MySlider *pms, DWORD dwmscupdf)
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

