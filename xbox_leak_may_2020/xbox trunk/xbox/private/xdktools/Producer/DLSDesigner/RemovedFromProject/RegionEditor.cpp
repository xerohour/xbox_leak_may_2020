// RegionEditor.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "dlsdesigner.h"

#include "Collection.h"
#include "Instrument.h"
#include "RegionCtl.h"
#include "RegionEditor.h"
#include "wave.h"
#include "DLSLoadSaveUtils.h" // J3 Need

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegionEditor

IMPLEMENT_DYNCREATE(CRegionEditor, CFormView)

CRegionEditor::CRegionEditor(CRegionCtrl *parent)
	: CFormView(CRegionEditor::IDD)
{
	m_parent = parent;
	m_pRegion = m_parent->GetRegion();
	m_pRegion->AddRef();
	CInstrument* pInstrument = m_pRegion->GetInstrument();
	m_pCollection = pInstrument->GetParentCollection();
	m_pWaves = &(m_pCollection->m_Waves);
	m_pWaves->AddRef();

	//{{AFX_DATA_INIT(CRegionEditor)
	m_dwLoopStart = 0;
	m_dwLoopLength = 0;
	m_fOneShot = FALSE;
	m_fAllowOverlap = FALSE;
	m_fAllowCompress = FALSE;
	m_fAllowTruncate = FALSE;
	//}}AFX_DATA_INIT
}

CRegionEditor::~CRegionEditor()
{
	if(m_pRegion)
	{
		m_pRegion->Release();
	}

	if(m_pWaves)
	{
		m_pWaves->Release();
	}
}

void CRegionEditor::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegionEditor)
	DDX_Text(pDX, IDC_LOOPSTART, m_dwLoopStart);
	DDV_MinMaxDWord(pDX, m_dwLoopStart, 0, 10000000);
	DDX_Text(pDX, IDC_LOOPLENGTH, m_dwLoopLength);
	DDV_MinMaxDWord(pDX, m_dwLoopLength, 0, 10000000);
	DDX_Check(pDX, IDC_ONESHOT, m_fOneShot);
	DDX_Check(pDX, IDC_OVERLAP, m_fAllowOverlap);
	DDX_Check(pDX, IDC_ALLOW_COMPRESS, m_fAllowCompress);
	DDX_Check(pDX, IDC_ALLOW_TRUNCATE, m_fAllowTruncate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegionEditor, CFormView)
	//{{AFX_MSG_MAP(CRegionEditor)
	ON_BN_CLICKED(IDC_GETWAVE, OnGetwave)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_WAVELINK, OnSelchangeWavelink)
	ON_BN_CLICKED(IDC_ALLOW_TRUNCATE, OnAllowTruncate)
	ON_BN_CLICKED(IDC_ALLOW_COMPRESS, OnAllowCompress)
	ON_BN_CLICKED(IDC_OVERLAP, OnOverlap)
	ON_EN_CHANGE(IDC_LOOPLENGTH, OnChangeLooplength)
	ON_EN_CHANGE(IDC_LOOPSTART, OnChangeLoopstart)
	ON_BN_CLICKED(IDC_ONESHOT, OnOneshot)
	ON_EN_CHANGE(IDC_DGROUP, OnChangeDgroup)
	ON_EN_CHANGE(IDC_DTUNE, OnChangeDtune)
	ON_EN_CHANGE(IDC_DATTENUATION, OnChangeDattenuation)
	ON_EN_CHANGE(IDC_DUNITYNOTE, OnChangeDunitynote)
	ON_EN_CHANGE(IDC_ELRANGE, OnChangeElrange)
	ON_EN_CHANGE(IDC_EURANGE, OnChangeEurange)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionEditor diagnostics

#ifdef _DEBUG
void CRegionEditor::AssertValid() const
{
	CFormView::AssertValid();
}

void CRegionEditor::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRegionEditor message handlers

BOOL CRegionEditor::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CRegionEditor::OnGetwave() 
{
	if(m_pWave != NULL)
    {
        m_msTune.SetValue(this,m_pWave->m_rWSMP.sFineTune << 16);
        m_msAttenuation.SetValue(this,m_pWave->m_rWSMP.lAttenuation);
        m_msUnityNote.SetValue(this,m_pWave->m_rWSMP.usUnityNote);
        m_dwLoopStart = m_pWave->m_rWLOOP.ulStart;
        m_dwLoopLength = m_pWave->m_rWLOOP.ulLength;
        m_fOneShot = (m_pWave->m_rWSMP.cSampleLoops == 0);
        m_fAllowCompress = !(m_pWave->m_rWSMP.fulOptions & F_WSMP_NO_COMPRESSION);
        m_fAllowTruncate = !(m_pWave->m_rWSMP.fulOptions & F_WSMP_NO_TRUNCATION);
        UpdateData(FALSE);
    }	
}

void CRegionEditor::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	MySlider *pSlider = NULL;
	if (pScrollBar != NULL)
	{
        int nID = pScrollBar->GetDlgCtrlID();
        switch (nID)
        {
        case IDC_URANGE :
            pSlider = &m_msUpperRange;
            break;
        case IDC_LRANGE :
            pSlider = &m_msLowerRange;
            break;
        case IDC_GROUP :
            pSlider = &m_msGroup;
            break;
        case IDC_TUNE :
            pSlider = &m_msTune;
            break;
        case IDC_ATTENUATION :
            pSlider = &m_msAttenuation;
            break;
        case IDC_UNITYNOTE :
            pSlider = &m_msUnityNote;
            break;
        default:
            pSlider = NULL;
            break;
        }
        if (pSlider != NULL)
        {
            pSlider->SetPosition(this,nSBCode,nPos);
        }
    }

	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRegionEditor::OnSelchangeWavelink() 
{
    CComboBox *pCombo = (CComboBox *) GetDlgItem(IDC_WAVELINK);
    if (pCombo != NULL)
    {
        int index = pCombo->GetCurSel();
		if (index != CB_ERR)
		{
			CWave * pWave = (CWave *) pCombo->GetItemDataPtr(index);
			if (pWave != m_pWave)
			{
				if (m_pRegion->m_pWave != NULL)
	            {
			        m_pRegion->m_pWave->Release();
				}
				
				m_pRegion->m_pWave = pWave;
				m_pWave = pWave;

				if (m_pRegion->m_pWave != NULL)
		        {
					m_pRegion->m_pWave->AddRef();
				}

			}
		}

#ifdef DLS_UPDATE_SYNTH
	m_pCollection->UpdateSynth(NULL, UPDATE_ALL);
#endif
	}
}

void CRegionEditor::OnInitialUpdate() 
{
	m_dwUpperRange = m_pRegion->m_rRgnHeader.RangeKey.usHigh;
	m_dwLowerRange = m_pRegion->m_rRgnHeader.RangeKey.usLow;
	m_dwGroup = m_pRegion->m_rRgnHeader.usKeyGroup;
	m_lFineTune = (long) m_pRegion->m_rWSMP.sFineTune << 16;
	m_lAttenuation = (long) m_pRegion->m_rWSMP.lAttenuation;
	m_lUnityNote = (long) m_pRegion->m_rWSMP.usUnityNote;
	m_dwLoopStart = m_pRegion->m_rWLOOP.ulStart;
	m_dwLoopLength = m_pRegion->m_rWLOOP.ulLength;
	m_fAllowTruncate = !(m_pRegion->m_rWSMP.fulOptions & F_WSMP_NO_TRUNCATION);
	m_fAllowCompress = !(m_pRegion->m_rWSMP.fulOptions & F_WSMP_NO_COMPRESSION);
	m_fAllowOverlap = (m_pRegion->m_rRgnHeader.fusOptions & F_RGN_OPTION_SELFNONEXCLUSIVE);
	m_fOneShot = (m_pRegion->m_rWSMP.cSampleLoops == 0);
	m_pWave = m_pRegion->m_pWave;

	CWave *pWave;

    CComboBox *pCombo = (CComboBox *) GetDlgItem(IDC_WAVELINK);
	m_msUpperRange.Init(this, IDC_URANGE, IDC_EURANGE, 
                    MYSLIDER_NOTE,(long *)&m_dwUpperRange);
	m_msLowerRange.Init(this, IDC_LRANGE, IDC_ELRANGE, 
                    MYSLIDER_NOTE,(long *)&m_dwLowerRange);
	m_msGroup.Init(this, IDC_GROUP, IDC_DGROUP, 
                    MYSLIDER_GROUP,(long *)&m_dwGroup);
    m_msTune.Init(this, IDC_TUNE, IDC_DTUNE,
                    MYSLIDER_PITCHCENTS, &m_lFineTune);
    m_msAttenuation.Init(this, IDC_ATTENUATION, IDC_DATTENUATION,
                    MYSLIDER_VOLUME, &m_lAttenuation);
    m_msUnityNote.Init(this, IDC_UNITYNOTE, IDC_DUNITYNOTE,
                    MYSLIDER_NOTE, &m_lUnityNote);
    if (pCombo != NULL)
    {
       pWave = m_pWaves->GetHead();
        for (; pWave != NULL; pWave = pWave->GetNext())
        {
            BSTR waveName;
			pWave->GetNodeName(&waveName);

			int index = pCombo->AddString(CString(waveName));
            pCombo->SetItemDataPtr(index, pWave);
            if (pWave == m_pWave)
            {
                pCombo->SetCurSel(index);
            }
        }
    }

	CFormView::OnInitialUpdate();
}

void CRegionEditor::OnAllowTruncate() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		if(m_fAllowTruncate)
        {
			m_pRegion->m_rWSMP.fulOptions &= ~F_WSMP_NO_TRUNCATION;
		}
		else
		{
			m_pRegion->m_rWSMP.fulOptions |= F_WSMP_NO_TRUNCATION;
		}
#ifdef DLS_UPDATE_SYNTH
		m_pCollection->UpdateSynth((m_parent->m_pRegion)->m_pInstrument,UPDATE_INSTRUMENT);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CRegionEditor::OnAllowCompress() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		if(m_fAllowCompress)
		{
			m_pRegion->m_rWSMP.fulOptions &= ~F_WSMP_NO_COMPRESSION;
        }
        else
		{		
			m_pRegion->m_rWSMP.fulOptions |= F_WSMP_NO_COMPRESSION;
		}
#ifdef DLS_UPDATE_SYNTH
		m_pCollection->UpdateSynth((m_parent->m_pRegion)->m_pInstrument,UPDATE_INSTRUMENT);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CRegionEditor::OnOverlap() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		if(m_fAllowOverlap)
		{
			m_pRegion->m_rRgnHeader.fusOptions |= F_RGN_OPTION_SELFNONEXCLUSIVE;
		}
		else
		{
			m_pRegion->m_rRgnHeader.fusOptions &= ~F_RGN_OPTION_SELFNONEXCLUSIVE;
		}
#ifdef DLS_UPDATE_SYNTH
		m_pCollection->UpdateSynth((m_parent->m_pRegion)->m_pInstrument,UPDATE_INSTRUMENT);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CRegionEditor::OnChangeLooplength() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		m_pRegion->m_rWLOOP.ulLength = m_dwLoopLength;
#ifdef DLS_UPDATE_SYNTH
		m_pCollection->UpdateSynth((m_parent->m_pRegion)->m_pInstrument,UPDATE_INSTRUMENT);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CRegionEditor::OnChangeLoopstart() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		m_pRegion->m_rWLOOP.ulStart = m_dwLoopStart;
#ifdef DLS_UPDATE_SYNTH
		m_pCollection->UpdateSynth((m_parent->m_pRegion)->m_pInstrument,UPDATE_INSTRUMENT);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CRegionEditor::OnOneshot() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		if(m_fOneShot)
		{
			m_pRegion->m_rWSMP.cSampleLoops = 0;
		}
		else
		{
			m_pRegion->m_rWSMP.cSampleLoops = 1;
		}
#ifdef DLS_UPDATE_SYNTH
		m_pCollection->UpdateSynth((m_parent->m_pRegion)->m_pInstrument,UPDATE_INSTRUMENT);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CRegionEditor::OnChangeDgroup() 
{
	BOOL b;
	b = UpdateData(FALSE);	

	if(b)
	{
		m_pRegion->m_rRgnHeader.usKeyGroup = (USHORT) m_dwGroup;
#ifdef DLS_UPDATE_SYNTH
		m_pCollection->UpdateSynth((m_parent->m_pRegion)->m_pInstrument,UPDATE_INSTRUMENT);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CRegionEditor::OnChangeDtune() 
{
	BOOL b;
	b = UpdateData(FALSE);	

	if(b)
	{
		m_pRegion->m_rWSMP.sFineTune = (WORD) (m_lFineTune >> 16);
#ifdef DLS_UPDATE_SYNTH
		m_pCollection->UpdateSynth((m_parent->m_pRegion)->m_pInstrument,UPDATE_INSTRUMENT);
#endif

	}
	else
	{
		// Add better error handling for J3
	}
}

void CRegionEditor::OnChangeDattenuation() 
{
	BOOL b;
	b = UpdateData(FALSE);	

	if(b)
	{
		m_pRegion->m_rWSMP.lAttenuation = (DWORD) m_lAttenuation;
#ifdef DLS_UPDATE_SYNTH
		m_pCollection->UpdateSynth((m_parent->m_pRegion)->m_pInstrument,UPDATE_INSTRUMENT);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CRegionEditor::OnChangeDunitynote() 
{
	static count = 0;
	BOOL b;
	b = UpdateData(FALSE);	

	if(b)
	{
		m_pRegion->m_rWSMP.usUnityNote = (WORD) m_lUnityNote;
#ifdef DLS_UPDATE_SYNTH
		if(count)
		{
		m_pCollection->UpdateSynth((m_parent->m_pRegion)->m_pInstrument,UPDATE_INSTRUMENT);
		}
		count++;
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CRegionEditor::OnChangeElrange() 
{
	BOOL b;
	b = UpdateData(FALSE);	

	if(b)
	{
		m_pRegion->m_rRgnHeader.RangeKey.usLow = (WORD) m_dwLowerRange;
#ifdef DLS_UPDATE_SYNTH
		m_pCollection->UpdateSynth((m_parent->m_pRegion)->m_pInstrument,UPDATE_INSTRUMENT);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CRegionEditor::OnChangeEurange() 
{
	BOOL b;
	b = UpdateData(FALSE);	

	if(b)
	{
		m_pRegion->m_rRgnHeader.RangeKey.usHigh = (WORD) m_dwUpperRange;
#ifdef DLS_UPDATE_SYNTH
		m_pCollection->UpdateSynth((m_parent->m_pRegion)->m_pInstrument,UPDATE_INSTRUMENT);
#endif
	}
	else
	{
		// Add better error handling for J3
	}
}

void CRegionEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CRegionEditor::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CFormView::OnKeyUp(nChar, nRepCnt, nFlags);
}
