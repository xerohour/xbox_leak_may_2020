// InstrumentEditor.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "dlsdesigner.h"
#include "jazz.h"
#include "InstrumentEditor.h"
#include "InstrumentCtl.h"
#include "Instrument.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstrumentEditor

IMPLEMENT_DYNCREATE(CInstrumentEditor, CFormView)

CInstrumentEditor::CInstrumentEditor(CInstrumentCtrl* parent)
	: CFormView(CInstrumentEditor::IDD), m_parent(NULL), m_pInstrument(NULL),
	m_pCollection(NULL)

{
	m_parent = parent;

	m_pInstrument = m_parent->GetInstrument();
	m_pInstrument->AddRef();
	
	m_pCollection = m_pInstrument->GetParentCollection();
	m_pCollection->AddRef();

	//{{AFX_DATA_INIT(CInstrumentEditor)
	m_wBank = 0;
	m_wBank2 = 0;
	m_wPatch = 0;
	m_fIsDrumKit = FALSE;
	//}}AFX_DATA_INIT
}

CInstrumentEditor::~CInstrumentEditor()
{
	if(m_pInstrument)
	{
		m_pInstrument->Release();
	}

	if(m_pCollection)
	{
		m_pCollection->Release();
	}

}

void CInstrumentEditor::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstrumentEditor)
	DDX_Text(pDX, IDC_BANK, m_wBank);
	DDV_MinMaxUInt(pDX, m_wBank, 0, 127);
	DDX_Text(pDX, IDC_BANK2, m_wBank2);
	DDV_MinMaxUInt(pDX, m_wBank2, 0, 127);
	DDX_Text(pDX, IDC_PATCH, m_wPatch);
	DDV_MinMaxUInt(pDX, m_wPatch, 0, 127);
	DDX_Check(pDX, IDC_DRUMS, m_fIsDrumKit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInstrumentEditor, CFormView)
	//{{AFX_MSG_MAP(CInstrumentEditor)
	ON_EN_CHANGE(IDC_BANK, OnChangeBank)
	ON_EN_CHANGE(IDC_BANK2, OnChangeBank2)
	ON_BN_CLICKED(IDC_DRUMS, OnDrums)
	ON_EN_CHANGE(IDC_PATCH, OnChangePatch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstrumentEditor diagnostics

#ifdef _DEBUG
void CInstrumentEditor::AssertValid() const
{
	CFormView::AssertValid();
}

void CInstrumentEditor::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInstrumentEditor message handlers

BOOL CInstrumentEditor::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CInstrumentEditor::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	char text[64];
	CWnd * pDisplay = NULL;

	m_wBank =  (m_pInstrument->m_rInstHeader.Locale.ulBank >> 8) & 0x7F;
	sprintf(text, "%d", m_wBank);
	pDisplay = GetDlgItem(IDC_BANK);	

	if(pDisplay)
	{
		pDisplay->SetWindowText(text);	
	}

	pDisplay = NULL;

	m_wBank2 = m_pInstrument->m_rInstHeader.Locale.ulBank & 0x7F;
	sprintf(text, "%d", m_wBank2);
	
	pDisplay = GetDlgItem(IDC_BANK2);
	if(pDisplay)
	{
		pDisplay->SetWindowText(text);	
	}

	pDisplay = NULL;
	
	m_fIsDrumKit = TRUE && m_pInstrument->m_rInstHeader.Locale.ulBank & F_INSTRUMENT_DRUMS;

	pDisplay = GetDlgItem(IDC_DRUMS);	
	if(pDisplay)
	{
		int state = m_fIsDrumKit ? 1 : 0;

		((CButton *)pDisplay)->SetCheck(state);
	}
	
	pDisplay = NULL;
	
	m_wPatch = m_pInstrument->m_rInstHeader.Locale.ulInstrument;
	sprintf(text, "%d", m_wPatch);	
	
	pDisplay = GetDlgItem(IDC_PATCH);		
	if(pDisplay)
	{
		pDisplay->SetWindowText(text);	
	}
}

void CInstrumentEditor::OnChangeBank() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
        m_pInstrument->m_rInstHeader.Locale.ulBank = (m_wBank << 8) + m_wBank2;
#ifdef DLS_UPDATE_SYNTH
	m_pCollection->UpdateSynth(NULL, UPDATE_ALL);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CInstrumentEditor::OnChangeBank2() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
        m_pInstrument->m_rInstHeader.Locale.ulBank = (m_wBank << 8) + m_wBank2;

#ifdef DLS_UPDATE_SYNTH
	m_pCollection->UpdateSynth(NULL, UPDATE_ALL);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CInstrumentEditor::OnDrums() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
        if(m_fIsDrumKit)
	    {			
			m_pInstrument->m_rInstHeader.Locale.ulBank |= F_INSTRUMENT_DRUMS;
		}
		else
		{
			// Should there be an else for J3
//			ASSERT(FALSE);
		}
#ifdef DLS_UPDATE_SYNTH
m_pCollection->UpdateSynth(m_pInstrument, UPDATE_INSTRUMENT);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CInstrumentEditor::OnChangePatch() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{		m_pInstrument->m_rInstHeader.Locale.ulInstrument = m_wPatch;
#ifdef DLS_UPDATE_SYNTH
	m_pCollection->UpdateSynth(NULL, UPDATE_ALL);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}