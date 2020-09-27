// ChordInversionPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\includes\DMUSProd.h"
#include "..\includes\Conductor.h"
#include "PropChord.h"
#include "PropPageMgr.h"
#include "chordstripmgr.h"
#include "ChordInversionPropPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChordInversionPropPage property page

IMPLEMENT_DYNCREATE(CChordInversionPropPage, CPropertyPage)

CChordInversionPropPage::CChordInversionPropPage() : CPropertyPage(CChordInversionPropPage::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	//{{AFX_DATA_INIT(CChordInversionPropPage)
	//}}AFX_DATA_INIT
	m_pPropPageMgr = NULL;
	m_pIConductor = 0;
	m_pIPerformance = 0;
	m_cRef = 0;
	m_fLockAllScales = true;
	m_fSyncLevelOneToAll = true;
	AddRef();
	m_bValidChord = false;
	m_fNeedToDetach = FALSE;
}

CChordInversionPropPage::~CChordInversionPropPage()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if(m_pIConductor)
	{
		m_pIConductor->Release();
	}
	if(m_pIPerformance)
	{
		m_pIPerformance->Release();
	}
}

void CChordInversionPropPage::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChordInversionPropPage)
	DDX_Control(pDX, IDC_KEYS_INVERSIONS4, m_keysinv4);
	DDX_Control(pDX, IDC_KEYS_INVERSIONS3, m_keysinv3);
	DDX_Control(pDX, IDC_KEYS_INVERSIONS2, m_keysinv2);
	DDX_Control(pDX, IDC_KEYS_INVERSIONS1, m_keysinv1);
	DDX_Control(pDX, IDC_KEYS_BASECHORD4, m_keysbase4);
	DDX_Control(pDX, IDC_KEYS_BASECHORD3, m_keysbase3);
	DDX_Control(pDX, IDC_KEYS_BASECHORD2, m_keysbase2);
	DDX_Control(pDX, IDC_KEYS_BASECHORD1, m_keysbase1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChordInversionPropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CChordInversionPropPage)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_BN_CLICKED(IDC_BUTTON_INVERT1, OnButtonInvert1)
	ON_BN_CLICKED(IDC_BUTTON_INVERT2, OnButtonInvert2)
	ON_BN_CLICKED(IDC_BUTTON_INVERT3, OnButtonInvert3)
	ON_BN_CLICKED(IDC_BUTTON_INVERT4, OnButtonInvert4)
	ON_BN_CLICKED(IDC_BUTTON_IPLAY1, OnButtonIplay1)
	ON_BN_CLICKED(IDC_BUTTON_IPLAY2, OnButtonIplay2)
	ON_BN_CLICKED(IDC_BUTTON_IPLAY3, OnButtonIplay3)
	ON_BN_CLICKED(IDC_BUTTON_IPLAY4, OnButtonIplay4)
	ON_BN_CLICKED(IDC_CHECK_LEGAL1, OnCheckLegal1)
	ON_BN_CLICKED(IDC_CHECK_LEGAL2, OnCheckLegal2)
	ON_BN_CLICKED(IDC_CHECK_LEGAL3, OnCheckLegal3)
	ON_BN_CLICKED(IDC_CHECK_LEGAL4, OnCheckLegal4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChordInversionPropPage message handlers



////////////////////////////////////////////////////////////////////////////
// CChordInversionPropPage other funcs


/////////////////////////////////////////////////////////////////////////////
// PropPageChord IUnknown
STDMETHODIMP CChordInversionPropPage::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	ASSERT( ppv );
    *ppv = NULL;
    if (IsEqualIID(riid, IID_IUnknown))
	{
        *ppv = (IUnknown *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdMidiInCPt))
	{
        *ppv = (IUnknown *) (IDMUSProdMidiInCPt *) this;
	}
	else
	{
		return E_NOTIMPL;
	}
    ((IUnknown *) *ppv)->AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CChordInversionPropPage::AddRef(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CChordInversionPropPage::Release(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	if( 0L == --m_cRef )
	{
		delete this;
		return 0;
	}
	else
	{
		return m_cRef;
	}
}


/////////////////////////////////////////////////////
//// Data transfer
void  CChordInversionPropPage::CopyDataToTab( CPropChord*  pTabData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if(pTabData && m_bValidChord)
	{
		*pTabData = m_Chord;
	}
	else
	{
		pTabData = 0;
	}
}

void CChordInversionPropPage::GetDataFromTab( const CPropChord* pTabData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if(pTabData)
	{
		m_Chord = *pTabData;
		for(int i = 0; i < m_Chord.MAX_POLY; i++)
		{
			m_invertVector[i].SetChord(m_Chord.SubChord(i)->ChordPattern());
			m_invertVector[i].SetInversion(m_Chord.SubChord(i)->InvertPattern());
		}
		m_bValidChord = true;
	}
	else
	{
		m_bValidChord = false;
	}
}

int CChordInversionPropPage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	// be found in the DLLs handle map.
	if( !FromHandlePermanent( m_hWnd ) )
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		Attach( hWnd );
		m_fNeedToDetach = TRUE;
	}

	if( CPropertyPage::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}

	return 0;
}

void CChordInversionPropPage::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs o be done here
	// so that the window handle can be removed from the
	// DLLs handle map.

	if( m_fNeedToDetach && m_hWnd )
	{
		HWND hWnd = m_hWnd;
		Detach();
		m_hWnd = hWnd;
	}
	CPropertyPage::OnDestroy();	
}

// required to get owner draw controls to paint 
void CChordInversionPropPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CChordInversionPropPage::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	CPropertyPage::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

BOOL CChordInversionPropPage::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	
	m_nextinvert4.AutoLoad(IDC_BUTTON_INVERT4, this);
	m_nextinvert3.AutoLoad(IDC_BUTTON_INVERT3, this);
	m_nextinvert2.AutoLoad(IDC_BUTTON_INVERT2, this);
	m_nextinvert1.AutoLoad(IDC_BUTTON_INVERT1, this);

	m_play4.AutoLoad(IDC_BUTTON_IPLAY4, this);
	m_play3.AutoLoad(IDC_BUTTON_IPLAY3, this);
	m_play2.AutoLoad(IDC_BUTTON_IPLAY2, this);
	m_play1.AutoLoad(IDC_BUTTON_IPLAY1, this);

	CPropertyPage::OnInitDialog();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChordInversionPropPage::OnButtonInvert1() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_invertStates[0].Rotate(m_Chord.SubChord(0)->NoteCount());
	SetChord(&m_Chord, 0);
}

void CChordInversionPropPage::OnButtonInvert2() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_invertStates[1].Rotate(m_Chord.SubChord(1)->NoteCount());
	SetChord(&m_Chord, 1);
}

void CChordInversionPropPage::OnButtonInvert3() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_invertStates[2].Rotate(m_Chord.SubChord(2)->NoteCount());
	SetChord(&m_Chord, 2);
}

void CChordInversionPropPage::OnButtonInvert4() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_invertStates[3].Rotate(m_Chord.SubChord(3)->NoteCount());
	SetChord(&m_Chord, 3);
}

void CChordInversionPropPage::OnButtonIplay1() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	PlayChordInversion(0);
}

void CChordInversionPropPage::OnButtonIplay2() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	PlayChordInversion(1);	
}

void CChordInversionPropPage::OnButtonIplay3() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	PlayChordInversion(2);	
}

void CChordInversionPropPage::OnButtonIplay4() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	PlayChordInversion(3);	
}

void CChordInversionPropPage::OnCheckLegal1() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_LEGAL1);
	bool state = pButton->GetCheck() == 1 ? true : false;
	int inversion = m_invertStates[0].Get()-1;
	if(inversion < 0)
	{
		pButton->SetCheck(1);
	}
	else
	{
		m_invertVector[0].SetAllowedInversion(inversion, state);
		m_Chord.SubChord(0)->InvertPattern() = m_invertVector[0].GetInvertPattern();
		UpdateChordData();
	}	
}

void CChordInversionPropPage::OnCheckLegal2() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_LEGAL2);
	bool state = pButton->GetCheck() == 1 ? true : false;
	int inversion = m_invertStates[1].Get()-1;
	if(inversion < 0)
	{
		pButton->SetCheck(1);
	}
	else
	{
		m_invertVector[1].SetAllowedInversion(inversion, state);
		m_Chord.SubChord(1)->InvertPattern() = m_invertVector[1].GetInvertPattern();
		UpdateChordData();
	}	
}

void CChordInversionPropPage::OnCheckLegal3() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_LEGAL3);
	bool state = pButton->GetCheck() == 1 ? true : false;
	int inversion = m_invertStates[2].Get()-1;
	if(inversion < 0)
	{
		pButton->SetCheck(1);
	}
	else
	{
		m_invertVector[2].SetAllowedInversion(inversion, state);
		m_Chord.SubChord(2)->InvertPattern() = m_invertVector[2].GetInvertPattern();
		UpdateChordData();
	}	
	
}

void CChordInversionPropPage::OnCheckLegal4() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_LEGAL4);
	bool state = pButton->GetCheck() == 1 ? true : false;
	int inversion = m_invertStates[3].Get()-1;
	if(inversion < 0)
	{
		pButton->SetCheck(1);
	}
	else
	{
		m_invertVector[3].SetAllowedInversion(inversion, state);
		m_Chord.SubChord(3)->InvertPattern() = m_invertVector[3].GetInvertPattern();
		UpdateChordData();
	}	
	
}

void CChordInversionPropPage::UpdateChordData(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pIPropPageObject)
	{
		CChordScalePropPageData chordscalePropPageData;

		chordscalePropPageData.m_pPropChord = &m_Chord;
		chordscalePropPageData.m_fLockAllScales = m_fLockAllScales;
		chordscalePropPageData.m_fSyncLevelOneToAll = m_fSyncLevelOneToAll;

		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &chordscalePropPageData);
	}	
}

void CChordInversionPropPage::SetChordData(CChordScalePropPageData* pChordScalePropPageData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pChordScalePropPageData != NULL );
	if( pChordScalePropPageData )
	{
		SetChord( pChordScalePropPageData->m_pPropChord );

		m_fLockAllScales = pChordScalePropPageData->m_fLockAllScales;
		m_fSyncLevelOneToAll = pChordScalePropPageData->m_fSyncLevelOneToAll;
	}
}

void CChordInversionPropPage::SetChord(CPropChord * pChord, int subchord)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	char str[30];
	m_Chord = *pChord;
	
	if( IsWindow( m_hWnd ) == 0 ) return;
	

	if(::IsWindow(m_play4.m_hWnd))
	{

		if(m_Chord.RootIndex() == DMPolyChord::INVALID)
		{
			EnableControls(FALSE);
		}
		else
		{
			EnableControls(TRUE);
		}
	}
	if(pChord->RootIndex() >= DMPolyChord::MAX_POLY) 
		return;	// invalid chord, controls will be blanked out


	if (pChord->GetUndetermined(pChord->RootIndex()) & UD_NAME)
	{
		SetDlgItemText(IDC_EDIT_NAME,"");
	}
	else
	{
		SetDlgItemText(IDC_EDIT_INAME,pChord->Name());
	}
	pChord->RootToString(str, pChord->RootIndex());
	SetDlgItemText(IDC_EDIT_IROOT,str);
//	SetCheckBox(pChord, UD_FLAT, IDC_CHECKBOX_FLATS, pChord->Base()->UseFlat() && 1);
	short nGrayed;
	if (pChord->GetUndetermined(pChord->RootIndex()) & UD_CHORDPATTERN) nGrayed = 4;
	else nGrayed = 0;
	int nX;
	int k0 = subchord == -1 ? 0 : subchord;
	int k1 = subchord == -1 ? CPropChord::MAX_POLY : subchord + 1;
	bool globalset = (subchord == -1);
	
	for(int n = k0; n < k1; n++)
	{
		// clear keyboards
		GetChordKeyboard(n)->ClearKeys();
		GetInversionKeyboard(n)->ClearKeys();

		// determine chord inversions
		int bass,third,fifth, seventh;
		BitFlag bfChord(pChord->SubChord(n)->ChordPattern());
		pChord->SubChord(n)->ChordNotes(bass, third, fifth, seventh);
		int inversion = m_invertStates[n].Get();
		if(inversion >= InversionState::First && third > -1)
		{
			bfChord.ClearBit(bass);
			bfChord.SetBit(bass+12);
		}
		if(inversion >= InversionState::Second && fifth > -1)
		{
			bfChord.ClearBit(third);
			bfChord.SetBit(third+12);
		}
		if(inversion >= InversionState::Third &&  seventh > -1)
		{
			bfChord.ClearBit(fifth);
			bfChord.SetBit(fifth+12);
		}
		DWORD dwChord = bfChord;
		// make sure pattern is tranposed
		dwChord = Rotate24(dwChord, pChord->SubChord(n)->ChordRoot() % 24);

		GetChordKeyboard(n)->Transpose(pChord->SubChord(n)->ChordRoot(), true);
		int shift = (pChord->SubChord(n)->ChordRoot())%24;
		if(globalset)
		{
			// only do this when all keys chords change (ie you've switched to this prop page)
			GetInversionKeyboard(n)->SetLowerBound(shift);
		}
		for (nX = 0; nX < 24; nX++)
		{
			short nState = nGrayed;
			if ((pChord->SubChord(n)->ChordPattern() >> nX) & 1) nState += 2;
			GetChordKeyboard(n)->SetNoteState(nX,"",nState);
			DrawInversionNote(pChord, dwChord, n, nX);
		}
		// finally set check box according to legality of inversion
		m_invertVector[n].SetChord(m_Chord.SubChord(n)->ChordPattern());
		m_invertVector[n].SetInversion(m_Chord.SubChord(n)->InvertPattern());
		CButton* pButton=0;
		switch(n)
		{
		case 0:
			pButton = (CButton*)GetDlgItem(	IDC_CHECK_LEGAL1);
			break;
		case 1:
			pButton = (CButton*)GetDlgItem(	IDC_CHECK_LEGAL2);
			break;
		case 2:
			pButton = (CButton*)GetDlgItem(	IDC_CHECK_LEGAL3);
			break;
		case 3:
			pButton = (CButton*)GetDlgItem(	IDC_CHECK_LEGAL4);
			break;
		}
		if(pButton)
		{
			if(inversion == 0)
			{
				pButton->SetCheck(1);
			}
			else if(m_invertVector[n].GetAllowedInversion(inversion-1))
			{
				pButton->SetCheck(1);
			}
			else
			{
				pButton->SetCheck(0);
			}
		}
	}

}

CKeyboard* CChordInversionPropPage::GetChordKeyboard(int index)
{
		switch(index)
		{
		case 0:
			return &m_keysbase1;			
			break;
		case 1:
			return &m_keysbase2;
			break;
		case 2:
			return &m_keysbase3;
			break;
		case 3:
			return &m_keysbase4;
			break;
		default:
			return 0;
			break;
		}
}

int CChordInversionPropPage::GetChordKeyboardIndex(const CKeyboard * pKeyboard)
{
	int i = 0;
	for(i; i < CPropChord::MAX_POLY; i++)
	{
		if(GetChordKeyboard(i) == pKeyboard)
		{
			break;
		}
	}
	return i >= CPropChord::MAX_POLY ? -1 : i;

}

CKeyboard* CChordInversionPropPage::GetInversionKeyboard(int index)
{
		switch(index)
		{
		case 0:
			return &m_keysinv1;			
			break;
		case 1:
			return &m_keysinv2;
			break;
		case 2:
			return &m_keysinv3;
			break;
		case 3:
			return &m_keysinv4;
			break;
		default:
			return 0;
			break;
		}

}

int CChordInversionPropPage::GetInversionKeyboardIndex(const CKeyboard * pKeyboard)
{
	int i = 0;
	for(i; i < CPropChord::MAX_POLY; i++)
	{
		if(GetInversionKeyboard(i) == pKeyboard)
		{
			break;
		}
	}
	return i >= CPropChord::MAX_POLY ? -1 : i;

}


void CChordInversionPropPage::DrawInversionNote(CPropChord* pChord, DWORD dwChordInversion, int index, short nX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	nX += GetInversionKeyboard(index)->LowerBound();
	short nX24 = nX%24;
	short nGrayed;
	if (pChord->GetUndetermined(index) & UD_SCALEPATTERN) nGrayed = 4;
	else nGrayed = 0;

	short nState = nGrayed;
	if ((dwChordInversion >> nX24) & 1) 
		nState += 2;
	GetInversionKeyboard(index)->SetNoteState(nX,"",nState);
}

BOOL CChordInversionPropPage::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// make sure chord in sync from any changes in chordscaleproppage.
	CChordScalePropPageData chordscalePropPageData;
	CChordScalePropPageData* pChordScalePropPageData = &chordscalePropPageData;
	if( SUCCEEDED ( m_pPropPageMgr->m_pIPropPageObject->GetData( (void **)&pChordScalePropPageData ) ) )
	{
		SetChordData(pChordScalePropPageData);
	}
	return CPropertyPage::OnSetActive();
}

#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_PTOUCH     0xA0
#define MIDI_CCHANGE    0xB0
#define MIDI_PCHANGE    0xC0
#define MIDI_MTOUCH     0xD0
#define MIDI_PBEND      0xE0

#define MIDI_BASEPITCH		48


void CChordInversionPropPage::PlayChordInversion(int index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if(m_pIConductor == NULL)
	{
		IDMUSProdComponent* pIComponent = NULL;
		if (m_pPropPageMgr->m_pIFramework)
		{
			if( SUCCEEDED ( m_pPropPageMgr->m_pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
			{
				HRESULT hr = pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&m_pIConductor );
				pIComponent->Release();
				if(FAILED(hr))
				{
					ASSERT(FALSE);
					return;
				}

			}
		}
	}
	
	short nX;
	DWORD dwTime = 0;
	ChangePatch(0,0,0,0);

	// determine chord inversions
	int bass,third,fifth, seventh;
	BitFlag bfChord(m_Chord.SubChord(index)->ChordPattern());
	m_Chord.SubChord(index)->ChordNotes(bass, third, fifth, seventh);
	int inversion = m_invertStates[index].Get();
	if(inversion >= InversionState::First && third > -1)
	{
		bfChord.ClearBit(bass);
		bfChord.SetBit(bass+12);
	}
	if(inversion >= InversionState::Second && fifth > -1)
	{
		bfChord.ClearBit(third);
		bfChord.SetBit(third+12);
	}
	if(inversion >= InversionState::Third &&  seventh > -1)
	{
		bfChord.ClearBit(fifth);
		bfChord.SetBit(fifth+12);
	}
	DWORD dwChord = bfChord;
	DWORD root = m_Chord.SubChord(index)->ChordRoot() % 12;

	for (nX = 0; nX < 24; nX++)
	{
		if ((dwChord >> nX) & 1)
		{
			unsigned char note = static_cast<unsigned char>(MIDI_BASEPITCH + nX + root);
			PlayNote(0, note, 120, dwTime, CHORDNOTE_DUR);
			dwTime += CHORDNOTE_DUR;
		}
	}
}

HRESULT CChordInversionPropPage::PlayNote(BYTE ch, BYTE key, BYTE vel, DWORD startTime, DWORD dur)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_OK;

	// make sure we have a performance
	if(m_pIPerformance == 0)
	{
		hr = m_pIConductor->GetPerformanceEngine( (IUnknown**)&m_pIPerformance);
		if(FAILED(hr))
		{
			return hr;
		}
	}

	// Allocate and initialize a PMsg
	DMUS_MIDI_PMSG* pMidi = NULL;
	hr = m_pIPerformance->AllocPMsg(sizeof (DMUS_MIDI_PMSG), (DMUS_PMSG**)&pMidi);
	if(FAILED(hr))
	{
		return hr;
	}
	memset(pMidi, 0, sizeof(DMUS_MIDI_PMSG));

	// fill in midi note on
	pMidi->bStatus = 0x90 + (ch&0xf);	// note on
	pMidi->bByte1 = key;
	pMidi->bByte2 = vel;

	REFERENCE_TIME	rtNow;
	m_pIPerformance->GetLatencyTime(&rtNow);
	pMidi->rtTime = startTime * 10000 + rtNow;
	pMidi->dwFlags = DMUS_PMSGF_REFTIME;
	pMidi->dwPChannel = (ch&0xf);
	pMidi->dwVirtualTrackID = 1;
	pMidi->dwType = DMUS_PMSGT_MIDI;

	hr = m_pIPerformance->SendPMsg(reinterpret_cast<DMUS_PMSG*>(pMidi));

	if(SUCCEEDED(hr))
	{
		// Allocate and initialize a PMsg
		DMUS_MIDI_PMSG* pMidi = NULL;
		hr = m_pIPerformance->AllocPMsg(sizeof (DMUS_MIDI_PMSG), (DMUS_PMSG**)&pMidi);
		if(FAILED(hr))
		{
			return hr;
		}
		memset(pMidi, 0, sizeof(DMUS_MIDI_PMSG));
		// fill in midi note off
		pMidi->bStatus = 0x80 + (ch&0xf);	// note on
		pMidi->bByte1 = key;
		pMidi->bByte2 = vel;

		pMidi->rtTime = (startTime + dur)* 10000 + rtNow;
		pMidi->dwFlags = DMUS_PMSGF_REFTIME;
		pMidi->dwPChannel = (ch&0xf);
		pMidi->dwVirtualTrackID = 1;
		pMidi->dwType = DMUS_PMSGT_MIDI;

		hr = m_pIPerformance->SendPMsg(reinterpret_cast<DMUS_PMSG*>(pMidi));
	}
	
	return hr;
}

HRESULT CChordInversionPropPage::ChangePatch(BYTE ch, BYTE inst, BYTE msb, BYTE lsb)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_OK;

	// make sure we have a performance
	if(m_pIPerformance == 0)
	{
		hr = m_pIConductor->GetPerformanceEngine( (IUnknown**)&m_pIPerformance);
		if(FAILED(hr))
		{
			return hr;
		}
	}

	// Allocate and initialize a PMsg
	DMUS_PATCH_PMSG* pPatch = NULL;
	hr = m_pIPerformance->AllocPMsg(sizeof (DMUS_PATCH_PMSG), (DMUS_PMSG**)&pPatch);
	if(FAILED(hr))
	{
		return hr;
	}
	memset(pPatch, 0, sizeof(DMUS_PATCH_PMSG));

	// fill in patch
	REFERENCE_TIME	rtNow;
	m_pIPerformance->GetLatencyTime(&rtNow);
	pPatch->byInstrument = inst;
	pPatch->byMSB = msb;
	pPatch->byLSB = lsb;

	pPatch->rtTime = rtNow;
	pPatch->dwFlags = DMUS_PMSGF_REFTIME;
	pPatch->dwPChannel = (ch&0xf);
	pPatch->dwVirtualTrackID = 1;
	pPatch->dwType = DMUS_PMSGT_PATCH;


	hr = m_pIPerformance->SendPMsg(reinterpret_cast<DMUS_PMSG*>(pPatch));

	return hr;
}

void CChordInversionPropPage::EnableControls(BOOL bEnable)
{
	m_nextinvert4.EnableWindow(bEnable);
	m_nextinvert3.EnableWindow(bEnable);
	m_nextinvert1.EnableWindow(bEnable);
	m_nextinvert2.EnableWindow(bEnable);

	m_play4.EnableWindow(bEnable);
	m_play3.EnableWindow(bEnable);
	m_play2.EnableWindow(bEnable);
	m_play1.EnableWindow(bEnable);

	GetDlgItem(IDC_CHECK_LEGAL1)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_LEGAL2)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_LEGAL3)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_LEGAL4)->EnableWindow(bEnable);

	if(!bEnable)
	{
		// grey out keyboard
		for(short i = 0; i < 48; i++)
		{
			short nState = 4;
			m_keysinv4.SetNoteState(i,"",nState);
			m_keysinv3.SetNoteState(i,"",nState);
			m_keysinv2.SetNoteState(i,"",nState);
			m_keysinv1.SetNoteState(i,"",nState);
			m_keysbase4.SetNoteState(i,"",nState);
			m_keysbase3.SetNoteState(i,"",nState);
			m_keysbase2.SetNoteState(i,"",nState);
			m_keysbase1.SetNoteState(i,"",nState);
		}
	}
}
