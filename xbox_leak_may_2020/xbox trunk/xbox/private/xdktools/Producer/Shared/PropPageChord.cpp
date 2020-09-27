// PropPageChord.cpp : implementation file
//

#include "stdafx.h"
#include "..\includes\DMUSProd.h"
#include "..\includes\Conductor.h"
#include "PropChord.h"
#include "PropPageMgr.h"
#include "PropPageChord.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////// Callback for CKeyboard Events
//
void KeyboardChangeCallback(CKeyboard* pKeyboard, void* hint, short nKey)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	PropPageChord* pdlg = (PropPageChord*)hint;
	pdlg->DispatchKeyboardChange(pKeyboard, nKey);
}

/////////////////////////////////////////////////////////////////////////////
// CChordPropPageMgr property page

CChordPropPageMgr::CChordPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pPropPageChord = NULL;
	m_pJazzFramework = NULL;
	CStaticPropPageManager::CStaticPropPageManager();
}

CChordPropPageMgr::~CChordPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageChord )
	{
		m_pPropPageChord->Release();
		m_pPropPageChord = 0;
	}
	if ( m_pJazzFramework )
	{
		m_pJazzFramework->Release();
		m_pJazzFramework = 0;
	}
	CStaticPropPageManager::~CStaticPropPageManager();
}

HRESULT STDMETHODCALLTYPE CChordPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return CStaticPropPageManager::QueryInterface( riid, ppv );
};

HRESULT STDMETHODCALLTYPE CChordPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
	BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;

	strTitle.LoadString( IDS_PROPPAGE_CHORD );
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
};

HRESULT STDMETHODCALLTYPE CChordPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
	LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	// Add Chord tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	if(!m_pPropPageChord)
		m_pPropPageChord = new PropPageChord();
	if( m_pPropPageChord )
	{
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pPropPageChord->m_psp, sizeof(PROPSHEETPAGE) );
		m_pPropPageChord->PreProcessPSP( psp, FALSE );

		hPage = ::CreatePropertySheetPage( &psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		m_pPropPageChord->m_pPropPageMgr = this;
		if (!m_pJazzFramework)
		{
			pIPropSheet->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pJazzFramework );
		}
		// fill in proppage's conductor member
		if(!m_pPropPageChord->m_pIConductor)
		{
			ASSERT(m_pJazzFramework);
			IDMUSProdComponent* pIComponent = NULL;
			IDMUSProdConductor* pConductor = NULL;
			if( SUCCEEDED ( m_pJazzFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
			{
				pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pConductor );
				if (pConductor)
				{
					m_pPropPageChord->m_pIConductor = pConductor;
				}
				pIComponent->Release();
			}
		}
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};

HRESULT STDMETHODCALLTYPE CChordPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CPropChord* pChord;
	if( m_pIPropPageObject == NULL )
	{
		pChord = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pChord ) ) )
	{
		return E_FAIL;
	}
	if (m_pPropPageChord && pChord)
	{
		m_pPropPageChord->SetChord( pChord );
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
};

/////////////////////////////////////////////////////////////////////////////
// PropPageChord property page

IMPLEMENT_DYNCREATE(PropPageChord, CPropertyPage)

PropPageChord::PropPageChord(): CPropertyPage(PropPageChord::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


//	CPropertyPage(IDD_CHORD_PROPPAGE);
	//{{AFX_DATA_INIT(PropPageChord)
	//}}AFX_DATA_INIT
	m_pPropPageMgr = NULL;
	m_fMidiInputTarget = None;
	m_dwCookie = 0;
	m_pIConductor = 0;
	m_cRef = 0;
	m_cRegCount = 0;
	m_fNeedToDetach = FALSE;
	AddRef();
}

PropPageChord::~PropPageChord()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	UnRegisterMidi();	// make sure we're unregistered
	if(m_pIConductor)
	{
		m_pIConductor->Release();
	}
}

void PropPageChord::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPageChord)
	DDX_Control(pDX, IDC_SCALE_KEYS, m_cScale);
	DDX_Control(pDX, IDC_CHORD_KEYS, m_cChord);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPageChord, CPropertyPage)
	ON_MESSAGE(WM_USER, OnWMUser)
	//{{AFX_MSG_MAP(PropPageChord)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHORD_DROPOCTAVE, OnChordDropoctave)
	ON_BN_CLICKED(IDC_CHORD_FLAT, OnChordFlat)
	ON_BN_CLICKED(IDC_CHORD_FOURNOTES, OnChordFournotes)
	ON_BN_CLICKED(IDC_CHORD_INVERT, OnChordInvert)
	ON_EN_CHANGE(IDC_CHORD_NAME, OnChangeChordName)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ROOT_SPIN, OnDeltaposRootSpin)
	ON_EN_CHANGE(IDC_ROOT_TEXT, OnChangeRootText)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_CHORD_MIDI_IN, OnChordMidiIn)
	ON_BN_CLICKED(IDC_CHORD_MIDI_IN2, OnScaleMidiIn)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPageChord custom functions

void PropPageChord::DrawScaleNote(CPropChord* pChord, short nX)

{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	short nGrayed;
	if (pChord->m_dwUndetermined & UD_SCALEPATTERN) nGrayed = 4;
	else nGrayed = 0;
	DWORD dwChord = pChord->m_dwChordPattern << (pChord->m_bRoot % 12);
	short nState = nGrayed;
	if ((pChord->m_dwScalePattern >> nX) & 1) 
	{
		nState++;
		if ((dwChord >> nX) & 1) nState += 2;
	}
	else
	{
		if ((dwChord >> nX) & 1) nState += 2;
	}
	m_cScale.SetNoteState(nX,"",nState);
}

void PropPageChord::SetCheckBox(CPropChord* pChord, DWORD dwUDFlag, UINT nDlgID, UINT nState)

{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	if (pChord->m_dwUndetermined & dwUDFlag)
	{
		CButton *pButton = (CButton *) GetDlgItem(nDlgID);
		if (pButton)
		{
			pButton->SetButtonStyle(BS_AUTO3STATE);
			pButton->SetCheck(2);		
		}
	}
	else
	{
		CButton *pButton = (CButton *) GetDlgItem(nDlgID);
		if (pButton)
		{
			pButton->SetButtonStyle(BS_AUTOCHECKBOX);
			pButton->SetCheck(nState);
		}
	}
}

void PropPageChord::SetChord( CPropChord* pChord )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	char str[30];
	m_Chord = *pChord;
	
	if( IsWindow( m_hWnd ) == 0 ) return;

	if (pChord->m_dwUndetermined & UD_NAME)
	{
		SetDlgItemText(IDC_CHORD_NAME,"");
	}
	else
	{
		SetDlgItemText(IDC_CHORD_NAME,pChord->m_szName);
	}
	pChord->RootToString(str);
	SetDlgItemText(IDC_ROOT_TEXT,str);
	SetCheckBox(pChord, UD_INVERT, IDC_CHORD_INVERT,(pChord->m_bBits & CHORD_INVERT) && 1);
	SetCheckBox(pChord, UD_UPPER, IDC_CHORD_DROPOCTAVE,(pChord->m_bBits & CHORD_UPPER) && 1);
	SetCheckBox(pChord, UD_FOUR, IDC_CHORD_FOURNOTES,(pChord->m_bBits & CHORD_FOUR) && 1);
	SetCheckBox(pChord, UD_FLAT, IDC_CHORD_FLAT,pChord->m_bFlat && 1);
	short nGrayed;
	if (pChord->m_dwUndetermined & UD_CHORDPATTERN) nGrayed = 4;
	else nGrayed = 0;
	int nX;
	for (nX = 0; nX < 24; nX++)
	{
		short nState = nGrayed;
		if ((pChord->m_dwChordPattern >> nX) & 1) nState += 2;
		m_cChord.SetNoteState(nX,"",nState);
	}
	for (nX = 0; nX < 24; nX++)
	{
		DrawScaleNote(pChord, nX);
	}
}

/////////////////////////////////////////////////////////////////////////////
// PropPageChord message handlers

int PropPageChord::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void PropPageChord::OnDestroy() 
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
	UnRegisterMidi();

	CPropertyPage::OnDestroy();	
}

BOOL PropPageChord::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	RegisterMidi();

	return CPropertyPage::OnSetActive();
}


int PropPageChord::DoModal() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	return CPropertyPage::DoModal();
}

BOOL PropPageChord::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

/////////////////////////////////////////////////////////////////////////////
// PropPageChord::PreProcessPSP

void PropPageChord::PreProcessPSP( PROPSHEETPAGE& psp, BOOL bWizard )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	PreProcessPageTemplate( psp, FALSE );
}

BOOL PropPageChord::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_cChord.SetLButtonUpCallback(KeyboardChangeCallback, this);
	m_cScale.SetLButtonUpCallback(KeyboardChangeCallback, this);

	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT PropPageChord::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return CPropertyPage::WindowProc(message, wParam, lParam);
}

BEGIN_EVENTSINK_MAP(PropPageChord, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(PropPageChord)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void PropPageChord::OnNoteDownChordKeys(short nKey) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	if (m_Chord.m_dwUndetermined & UD_CHORDPATTERN)
	{
		m_Chord.m_dwUndetermined &= ~UD_CHORDPATTERN;
		for (nKey = 0; nKey < 24; nKey++)
		{
			m_cChord.SetNoteState(nKey,"",(1 && ((m_Chord.m_dwChordPattern >> nKey) & 1)) << 1);
		}
	}
	else
	{
		m_Chord.m_dwChordPattern ^= (1 << nKey);
		m_Chord.m_dwUndetermined &= ~UD_CHORDPATTERN;
		m_cChord.SetNoteState(nKey,"",(1 && ((m_Chord.m_dwChordPattern >> nKey) & 1)) << 1);
		DrawScaleNote(&m_Chord, (nKey + m_Chord.m_bRoot) % 24);
		DrawScaleNote(&m_Chord, (nKey + m_Chord.m_bRoot + 12) % 24);
	}
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
	}
}


void PropPageChord::OnNoteDownScaleKeys(short nKey) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	short nOtherKey = (nKey + 12) % 24;
	if (m_Chord.m_dwUndetermined & UD_SCALEPATTERN)
	{
		m_Chord.m_dwUndetermined &= ~UD_SCALEPATTERN;
//		m_Chord.m_dwScalePattern ^= (1 << nKey);
		for (nKey = 0; nKey < 24; nKey++)
		{
			DrawScaleNote(&m_Chord,nKey);
		}
	}
	else
	{
		m_Chord.m_dwScalePattern ^= (1 << nKey);
		m_Chord.m_dwScalePattern ^= (1 << nOtherKey);
		DrawScaleNote(&m_Chord,nKey);
		DrawScaleNote(&m_Chord,nOtherKey);
	}
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
	}	
}

void PropPageChord::OnChordDropoctave() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	UINT nChecked = IsDlgButtonChecked(IDC_CHORD_DROPOCTAVE);
	UINT nState;
	if (m_Chord.m_dwUndetermined & UD_UPPER)
	{
		nState = 2;
	}
	else if (m_Chord.m_bBits & CHORD_UPPER)
	{
		nState = 1;
	}
	else nState = 0;
	if (nState != nChecked)
	{
		if (nChecked == 2)
		{
			CButton *pButton = (CButton *) GetDlgItem(IDC_CHORD_DROPOCTAVE);
			if (pButton)
			{
				pButton->SetButtonStyle(BS_AUTOCHECKBOX);
				pButton->SetCheck(nState);			
			}
		}
		else
		{
			m_Chord.m_dwUndetermined &= ~UD_UPPER;
			if (nChecked == 1)
			{
				m_Chord.m_bBits |= CHORD_UPPER;
			}
			else 
			{
				m_Chord.m_bBits &= ~CHORD_UPPER;
			}
			if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
			{
				m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
			}	
		}
	}
}

void PropPageChord::OnChordFlat() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	UINT nChecked = IsDlgButtonChecked(IDC_CHORD_FLAT);
	UINT nState;
	if (m_Chord.m_dwUndetermined & UD_FLAT)
	{
		nState = 2;
	}
	else if (m_Chord.m_bFlat)
	{
		nState = 1;
	}
	else nState = 0;
	if (nState != nChecked)
	{
		if (nChecked == 2)
		{
			CButton *pButton = (CButton *) GetDlgItem(IDC_CHORD_FLAT);
			if (pButton)
			{
				pButton->SetButtonStyle(BS_AUTOCHECKBOX);
				pButton->SetCheck(nState);			
			}
		}
		else
		{
			m_Chord.m_dwUndetermined &= ~UD_FLAT;
			if (nChecked == 1)
			{
				m_Chord.m_bFlat = TRUE;
			}
			else 
			{
				m_Chord.m_bFlat = FALSE;
			}
			if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
			{
				m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
			}	
		}
	}
}

void PropPageChord::OnChordFournotes() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	UINT nChecked = IsDlgButtonChecked(IDC_CHORD_FOURNOTES);
	UINT nState;
	if (m_Chord.m_dwUndetermined & UD_FOUR)
	{
		nState = 2;
	}
	else if (m_Chord.m_bBits & CHORD_FOUR)
	{
		nState = 1;
	}
	else nState = 0;
	if (nState != nChecked)
	{
		if (nChecked == 2)
		{
			CButton *pButton = (CButton *) GetDlgItem(IDC_CHORD_FOURNOTES);
			if (pButton)
			{
				pButton->SetButtonStyle(BS_AUTOCHECKBOX);
				pButton->SetCheck(nState);			
			}
		}
		else
		{
			m_Chord.m_dwUndetermined &= ~UD_FOUR;
			if (nChecked == 1)
			{
				m_Chord.m_bBits |= CHORD_FOUR;
			}
			else 
			{
				m_Chord.m_bBits &= ~CHORD_FOUR;
			}
			if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
			{
				m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
			}	
		}
	}
}

void PropPageChord::OnChordInvert() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	UINT nChecked = IsDlgButtonChecked(IDC_CHORD_INVERT);
	UINT nState;
	
	if (m_Chord.m_dwUndetermined & UD_INVERT)
	{
		nState = 2;
	}
	else if (m_Chord.m_bBits & CHORD_INVERT)
	{
		nState = 1;
	}
	else nState = 0;
	if (nState != nChecked)
	{
		if (nChecked == 2)
		{
			CButton *pButton = (CButton *) GetDlgItem(IDC_CHORD_INVERT);
			if (pButton)
			{
				pButton->SetButtonStyle(BS_AUTOCHECKBOX);
				pButton->SetCheck(nState);			
			}
		}
		else
		{
			m_Chord.m_dwUndetermined &= ~UD_INVERT;
			if (nChecked == 1)
			{
				m_Chord.m_bBits |= CHORD_INVERT;
			}
			else 
			{
				m_Chord.m_bBits &= ~CHORD_INVERT;
			}
			if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
			{
				m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
			}	
		}
	}
}

void PropPageChord::OnChangeChordName() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	char szName[30];
	GetDlgItemText(IDC_CHORD_NAME,szName,sizeof(m_Chord.m_szName));
	if (strcmp("",szName) && (strcmp(m_Chord.m_szName,szName)))
	{
		strcpy(m_Chord.m_szName,szName);
		m_Chord.m_dwUndetermined &= ~UD_NAME;
		if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
		{
			m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
		}	
	}
}

void PropPageChord::OnDeltaposRootSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

#ifdef CHORDMAP
	if(m_Chord.m_dwUndetermined & UD_FROMCHORDPALETTE)
		return;	// can't change root of palette chords
#endif
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	short nX = m_Chord.m_bRoot - pNMUpDown->iDelta;
	if (nX < 0) nX = 0;
	if (nX > 23) nX = 23;
	m_Chord.m_bRoot = (BYTE) nX;
	char str[10];
	m_Chord.RootToString(str);
	SetDlgItemText(IDC_ROOT_TEXT,str);
	m_Chord.m_dwUndetermined &= ~UD_ROOT;
	for (nX = 0; nX < 24; nX++)
	{
		DrawScaleNote(&m_Chord, nX);
	}
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
	}	
	*pResult = 0;
}

void PropPageChord::OnChangeRootText() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
	}	
}
#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_PTOUCH     0xA0
#define MIDI_CCHANGE    0xB0
#define MIDI_PCHANGE    0xC0
#define MIDI_MTOUCH     0xD0
#define MIDI_PBEND      0xE0

#define MIDI_BASEPITCH		60

void PropPageChord::OnTest() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	IDMUSProdConductor *pConductor = NULL;
	IDMUSProdComponent* pIComponent = NULL;
	if (m_pPropPageMgr->m_pJazzFramework)
	{
		if( SUCCEEDED ( m_pPropPageMgr->m_pJazzFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
		{
			pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pConductor );
			if (pConductor)
			{
				short nX;
				DWORD dwTime = 500;
				pConductor->PlayMIDIEvent( MIDI_PCHANGE, 0, 0, 0 );
				pConductor->PlayMIDIEvent( MIDI_PCHANGE+1, 10, 0, 0 );
				for (nX = 0; nX < 24; nX++)
				{
					if ((m_Chord.m_dwChordPattern >> nX) & 1)
					{
						short note = MIDI_BASEPITCH + nX + m_Chord.m_bRoot;
						pConductor->PlayMIDIEvent( MIDI_NOTEON, 
							note, 120, dwTime );
						pConductor->PlayMIDIEvent( MIDI_NOTEOFF, 
							note, 120, dwTime + 1000 );
					}
				}
				dwTime += 1000;
				for (nX = 0; nX < 25; nX++)
				{
					if ((m_Chord.m_dwScalePattern >> nX) & 1)
					{
						short note = MIDI_BASEPITCH + nX;
						pConductor->PlayMIDIEvent( MIDI_NOTEON+1, 
							note,  100, dwTime );
						pConductor->PlayMIDIEvent( MIDI_NOTEOFF+1, 
							note,  100, dwTime + 180);
						dwTime += 200;
					}
				}
				pConductor->Release();
			}
			pIComponent->Release();
		}
	}
}

void PropPageChord::OnChordMidiIn() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	CButton* chord = (CButton*)GetDlgItem(IDC_CHORD_MIDI_IN);
	ASSERT(chord);
	CButton* scale = (CButton*)GetDlgItem(IDC_CHORD_MIDI_IN2);
	ASSERT(scale);


	if(m_fMidiInputTarget == None || m_fMidiInputTarget == Scale)
	{
		m_fMidiInputTarget = Chord;
		chord->SetCheck(1);
		scale->SetCheck(0);
		RegisterMidi();
	}
	else if(m_fMidiInputTarget == Chord)
	{
		m_fMidiInputTarget = None;
		chord->SetCheck(0);
		scale->SetCheck(0);
		UnRegisterMidi();
	}
}

void PropPageChord::OnScaleMidiIn() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	CButton* chord = (CButton*)GetDlgItem(IDC_CHORD_MIDI_IN);
	ASSERT(chord);
	CButton* scale = (CButton*)GetDlgItem(IDC_CHORD_MIDI_IN2);
	ASSERT(scale);


	if(m_fMidiInputTarget == None || m_fMidiInputTarget == Chord)
	{
		m_fMidiInputTarget = Scale;
		chord->SetCheck(0);
		scale->SetCheck(1);
		RegisterMidi();
	}
	else if(m_fMidiInputTarget == Scale)
	{
		m_fMidiInputTarget = None;
		chord->SetCheck(0);
		scale->SetCheck(0);
		UnRegisterMidi();
	}
}

const short KeyOCXTrans = 48;
const short KeyOCXLow = 0;
const short KeyOCXHigh = 23;

HRESULT PropPageChord::OnMidiMsg(REFERENCE_TIME rtTime,
									   BYTE bStatus, 
									   BYTE bData1, 
									   BYTE bData2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	// Note On
	if((int)(bStatus & 0xF0) == (int)0x90)
	{

		char Text[128];
		sprintf(Text, "Inside  PropPageChord::OnMidiMsg going to play %d note on\n", bData1); 
		TRACE0(Text);
		
		// set note
		short nKey = bData1 - KeyOCXTrans;
		if(nKey >= KeyOCXLow && nKey <= KeyOCXHigh)
		{
			this->PostMessage(WM_USER, nKey, 0);
		}
	}

	// Note Off
	if((int)(bStatus & 0xF0) == (int)0x80)
	{
		char Text[128];
		sprintf(Text, "Inside  PropPageChord::OnMidiMsg going to play %d note off\n", bData1); 
		TRACE0(Text);
	}

	// set up patch
	m_pIConductor->PlayMIDIEvent( MIDI_PCHANGE, 0, 0, 0 );
	m_pIConductor->PlayMIDIEvent( MIDI_PCHANGE+1, 10, 0, 0 );

	m_pIConductor->PlayMIDIEvent((bStatus & 0xF0),
								 bData1,
								 bData2,
								 0);	
	
	return S_OK;
}

void PropPageChord::RegisterMidi() 
{
	ASSERT(m_cRegCount <= 1);
	if(m_fMidiInputTarget != None && m_cRegCount == 0)
	{
		++m_cRegCount;
		REGISTER_MIDI_IN(m_pIConductor, m_dwCookie)	
	}
}

void PropPageChord::UnRegisterMidi()
{
	ASSERT(m_cRegCount <= 1);
	if(m_cRegCount)
	{
		m_cRegCount--;
		UNREGISTER_MIDI_IN(m_pIConductor, m_dwCookie)
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageChord IUnknown
STDMETHODIMP PropPageChord::QueryInterface( REFIID riid, LPVOID *ppv )
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

STDMETHODIMP_(ULONG) PropPageChord::AddRef(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	return ++m_cRef;
}

STDMETHODIMP_(ULONG) PropPageChord::Release(void)
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

void PropPageChord::OnKillFocus(CWnd* pNewWnd) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	CPropertyPage::OnKillFocus(pNewWnd);
	
	AfxMessageBox("Losing Focus");	
}

long PropPageChord::OnWMUser(WPARAM midinote, LPARAM)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	if(m_fMidiInputTarget == Chord)
	{
		OnNoteDownChordKeys(midinote);
	}
	else if(m_fMidiInputTarget == Scale)
	{
		OnNoteDownScaleKeys(midinote);
	}
	return 0;
}

void PropPageChord::DispatchKeyboardChange(CKeyboard* pKeyboard, short nKey)
{
	if(pKeyboard == &m_cChord)
	{
		OnNoteDownChordKeys(nKey);
	}
	else if(pKeyboard == &m_cScale)
	{
		OnNoteDownScaleKeys(nKey);
	}
}
