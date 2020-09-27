// LFODialog.cpp : implementation file
//

#include "stdafx.h"
#include "LFODialog.h"
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
// CLFODialog dialog

CLFODialog::CLFODialog(CArticulation* pArticulation)
		  : CDialog(), CSliderCollection(9),
		    m_pArticParams(pArticulation->GetArticParams()),
		    m_pCollection(NULL)
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
		pInstrument = pRegion->GetInstrument();
		ASSERT(pInstrument);
		m_pCollection = pInstrument->m_pCollection;
	}
	CSliderCollection::Init(this);
	//{{AFX_DATA_INIT(CLFODialog)
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CLFODialog, CDialog)
	//{{AFX_MSG_MAP(CLFODialog)
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLFODialog message handlers

void CLFODialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

BOOL CLFODialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_LFOParams = m_pArticParams->m_LFO;
	m_pmsLFOFrequency = Insert(
		IDC_LFO_FREQUENCY,
		IDC_LFO_DFREQUENCY,
		IDC_LFO_FREQUENCY_SPIN,
		MYSLIDER_LFORANGE,
		FIVE_HERTZ,
		IDS_LFO_FREQ_UNDO_TEXT,
		&m_LFOParams.m_pcFrequency);

	m_pmsLFODelay = Insert(
		IDC_LFO_DELAY,
		IDC_LFO_DDELAY,
		IDC_LFO_DELAY_SPIN,
		MYSLIDER_LFODELAY,
		Mils2TimeCents(0),
		IDS_LFO_DELAY_UNDO_TEXT,
		&m_LFOParams.m_tcDelay);
   
	m_pmsLFOVolumeScale = Insert(
		IDC_LFO_VOLUME,
		IDC_LFO_DVOLUME,
		IDC_LFO_VOLUME_SPIN,
		MYSLIDER_VOLUMECENTS,
		0,
		IDS_LFO_VOLUME_UNDO_TEXT,
		&m_LFOParams.m_gcVolumeScale);

	m_pmsLFOPitchScale = Insert(
		IDC_LFO_PITCH,
		IDC_LFO_DPITCH,
		IDC_LFO_PITCH_SPIN,
		MYSLIDER_PITCHCENTS,
		0,
		IDS_LFO_PITCH_UNDO_TEXT,
		&m_LFOParams.m_pcPitchScale);

	m_pmsLFOMWToVolume = Insert(
		IDC_LFO_MW2VOLUME,
		IDC_LFO_DMW2VOLUME,
		IDC_LFO_MW2VOLUME_SPIN,
		MYSLIDER_VOLUMECENTS,
		0,
		IDS_MW2VOL_UNDO_TEXT,
		&m_LFOParams.m_gcMWToVolume);

	m_pmsLFOMWToPitch = Insert(
		IDC_LFO_MW2PITCH,
		IDC_LFO_DMW2PITCH, 
		IDC_LFO_MW2PITCH_SPIN,
		MYSLIDER_PITCHCENTS,
		0,
		IDS_MW2PITCH_UNDO_TEXT,
		&m_LFOParams.m_pcMWToPitch);

	m_pmsLFOChanPressToGain = Insert(
		IDC_LFO_CHANPRESSTOGAIN,
		IDC_LFO_DCHANPRESSTOGAIN, 
		IDC_LFO_CHANPRESSTOGAIN_SPIN,
		MYSLIDER_VOLUMECENTS,
		0,
		IDS_UNDO_LFO_CHANPRESSTOGAIN_EDIT,
		&m_LFOParams.m_gcChanPressToGain);

	m_pmsLFOChanPressToPitch = Insert(
		IDC_LFO_CHANPRESSTOPITCH,
		IDC_LFO_DCHANPRESSTOPITCH, 
		IDC_LFO_CHANPRESSTOPITCH_SPIN,
		MYSLIDER_PITCHCENTS,
		0,
		IDS_UNDO_LFO_CHANPRESSTOPITCH_EDIT,
		&m_LFOParams.m_pcChanPressToPitch);

	m_pmsLFOChanPressToFc = Insert(
		IDC_LFO_CHANPRESSTOFC,
		IDC_LFO_DCHANPRESSTOFC, 
		IDC_LFO_CHANPRESSTOFC_SPIN,
		MYSLIDER_FILTERPITCHCENTS,
		0,
		IDS_UNDO_LFO_CHANPRESSTOFC_EDIT,
		&m_LFOParams.m_pcChanPressToFc);

	CollectStatics();

	BOOL bDLS1 = m_pArticulation->IsDLS1();
	EnableDLS2Controls(!bDLS1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HRESULT CLFODialog::SaveUndoState(UINT uStringRes)
{
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

void CLFODialog::UpdateArticulation(CArticulation* pNewArt)
{
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
	m_pmsLFOFrequency->SetValue(this, m_LFOParams.m_pcFrequency);
	m_pmsLFODelay->SetValue(this, m_LFOParams.m_tcDelay);
	m_pmsLFOVolumeScale->SetValue(this, m_LFOParams.m_gcVolumeScale);
	m_pmsLFOPitchScale->SetValue(this, m_LFOParams.m_pcPitchScale);
	m_pmsLFOMWToVolume->SetValue(this, m_LFOParams.m_gcMWToVolume);
	m_pmsLFOMWToPitch->SetValue(this, m_LFOParams.m_pcMWToPitch);
	m_pmsLFOChanPressToGain->SetValue(this, m_LFOParams.m_gcChanPressToGain);
	m_pmsLFOChanPressToPitch->SetValue(this, m_LFOParams.m_pcChanPressToPitch);
	m_pmsLFOChanPressToFc->SetValue(this, m_LFOParams.m_pcChanPressToFc);

	BOOL bDLS1 = m_pArticulation->IsDLS1();
	EnableDLS2Controls(!bDLS1);
}

void CLFODialog::CollectStatics()
{
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


void CLFODialog::OnDestroy() 
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

void CLFODialog::OnPaint() 
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

void CLFODialog::EnableDLS2Controls(BOOL bEnable)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (m_pmsLFOChanPressToGain)
		{
		m_pmsLFOChanPressToGain->EnableControl(this, bEnable != 0);
		m_pmsLFOChanPressToPitch->EnableControl(this, bEnable != 0);
		m_pmsLFOChanPressToFc->EnableControl(this, bEnable != 0);
		}
}

BOOL CLFODialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	LRESULT lResult;
	if (CSliderCollection::OnCommand(wParam, lParam, &lResult))
		return lResult;

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CLFODialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LRESULT lResult;
	if (CSliderCollection::OnNotify(wParam, lParam, &lResult))
		return lResult;
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CLFODialog::UpdateInstrument()
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

bool CLFODialog::OnSliderUpdate(MySlider *pms, DWORD dwmscupdf)
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

