// ChordScalePropPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\includes\DMUSProd.h"
#include "..\includes\Conductor.h"
#include "DllBasePropPageManager.h"
#include "PropChord.h"
#include "PropPageMgr.h"
#include "chordmapstripmgr.h"
#include "ChordScalePropPage.h"

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
	CChordScalePropPage* pdlg = (CChordScalePropPage*)hint;
	pdlg->DispatchKeyboardChange(pKeyboard, nKey);
}


/////////////////////////////////////////////////////////////////////////////
// CChordScalePropPage property page

IMPLEMENT_DYNCREATE(CChordScalePropPage, CPropertyPage)

CChordScalePropPage::CChordScalePropPage() : CPropertyPage(CChordScalePropPage::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	//{{AFX_DATA_INIT(CChordScalePropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pPropPageMgr = NULL;
	m_fMidiInputTarget = None;
	m_dwCookie = 0;
	m_pIConductor = 0;
	m_cRef = 0;
	m_cRegCount = 0;
	m_bValidChord = false;
	m_fNeedToDetach = FALSE;
	AddRef();

	m_MidiInIds[0] = IDC_BUTTON_MIDICHORD1;
	m_MidiInIds[1] = IDC_BUTTON_MIDICHORD2;
	m_MidiInIds[2] = IDC_BUTTON_MIDICHORD3;
	m_MidiInIds[3] = IDC_BUTTON_MIDICHORD4;

	m_MidiInIds[4] = IDC_BUTTON_MIDISCALE1;
	m_MidiInIds[5] = IDC_BUTTON_MIDISCALE2;
	m_MidiInIds[6] = IDC_BUTTON_MIDISCALE3;
	m_MidiInIds[7] = IDC_BUTTON_MIDISCALE4;

	m_MidiInIds[8] = IDC_BUTTON_MIDIROOT;

}

CChordScalePropPage::~CChordScalePropPage()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	UnRegisterMidi();	// make sure we're unregistered
	if(m_pIConductor)
	{
		m_pIConductor->Release();
	}
}

void CChordScalePropPage::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChordScalePropPage)
	DDX_Control(pDX, IDC_KEYS_SCALE4, m_keysscale4);
	DDX_Control(pDX, IDC_KEYS_SCALE3, m_keysscale3);
	DDX_Control(pDX, IDC_KEYS_SCALE2, m_keysscale2);
	DDX_Control(pDX, IDC_KEYS_SCALE1, m_keysscale1);
	DDX_Control(pDX, IDC_KEYS_CHORD4, m_keyschord4);
	DDX_Control(pDX, IDC_KEYS_CHORD3, m_keyschord3);
	DDX_Control(pDX, IDC_KEYS_CHORD2, m_keyschord2);
	DDX_Control(pDX, IDC_KEYS_CHORD1, m_keyschord1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChordScalePropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CChordScalePropPage)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_MIDICHORD4, OnButtonMidichord4)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_BN_CLICKED(IDC_BUTTON_MIDICHORD1, OnButtonMidichord1)
	ON_BN_CLICKED(IDC_BUTTON_MIDICHORD2, OnButtonMidichord2)
	ON_BN_CLICKED(IDC_BUTTON_MIDICHORD3, OnButtonMidichord3)
	ON_BN_CLICKED(IDC_BUTTON_MIDIROOT, OnButtonMidiroot)
	ON_BN_CLICKED(IDC_BUTTON_MIDISCALE1, OnButtonMidiscale1)
	ON_BN_CLICKED(IDC_BUTTON_MIDISCALE2, OnButtonMidiscale2)
	ON_BN_CLICKED(IDC_BUTTON_MIDISCALE3, OnButtonMidiscale3)
	ON_BN_CLICKED(IDC_BUTTON_MIDISCALE4, OnButtonMidiscale4)
	ON_BN_CLICKED(IDC_BUTTON_PLAY1, OnButtonPlay1)
	ON_BN_CLICKED(IDC_BUTTON_PLAY2, OnButtonPlay2)
	ON_BN_CLICKED(IDC_BUTTON_PLAY3, OnButtonPlay3)
	ON_BN_CLICKED(IDC_BUTTON_PLAY4, OnButtonPlay4)
	ON_BN_CLICKED(IDC_BUTTON_PLAYALL, OnButtonPlayall)
	ON_BN_CLICKED(IDC_BUTTON_SHIFTL1, OnButtonShiftl1)
	ON_BN_CLICKED(IDC_BUTTON_SHIFTL2, OnButtonShiftl2)
	ON_BN_CLICKED(IDC_BUTTON_SHIFTL3, OnButtonShiftl3)
	ON_BN_CLICKED(IDC_BUTTON_SHIFTL4, OnButtonShiftl4)
	ON_BN_CLICKED(IDC_BUTTON_SHIFTR1, OnButtonShiftr1)
	ON_BN_CLICKED(IDC_BUTTON_SHIFTR2, OnButtonShiftr2)
	ON_BN_CLICKED(IDC_BUTTON_SHIFTR3, OnButtonShiftr3)
	ON_BN_CLICKED(IDC_BUTTON_SHIFTR4, OnButtonShiftr4)
	ON_BN_CLICKED(IDC_CHECKBOX_FLATS, OnCheckboxFlats)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeEditName)
	ON_WM_KILLFOCUS()
	ON_BN_CLICKED(IDC_USEFLATS, OnUseflats)
	ON_BN_CLICKED(IDC_USESHARPS, OnUsesharps)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChordScalePropPage message handlers


///////////////////////////////////////////////////////////////////////////
// CChordScalePropPage other funcs
const short KeyOCXTrans = 48;
const short KeyOCXLow = 0;
const short KeyOCXHigh = 23;
#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_PTOUCH     0xA0
#define MIDI_CCHANGE    0xB0
#define MIDI_PCHANGE    0xC0
#define MIDI_MTOUCH     0xD0
#define MIDI_PBEND      0xE0

#define MIDI_BASEPITCH		36

HRESULT CChordScalePropPage::OnMidiMsg(REFERENCE_TIME rtTime,
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

		if(m_fMidiInputTarget < 4)
		{
			bool rootHasChanged = m_fMidiInputTarget == 0 && (nKey <= m_Chord.Base()->ChordRoot());
			DispatchKeyboardChange(GetChordKeyboard(m_fMidiInputTarget), nKey);
			if(rootHasChanged)
			{
				GetChordKeyboard(m_fMidiInputTarget)->SetLowerBound(GetChordKeyboard(m_fMidiInputTarget)->Transpose());
			}
		}
		else if(m_fMidiInputTarget < 8)
		{
			DispatchKeyboardChange(GetScaleKeyboard(m_fMidiInputTarget - 4), nKey);
		}
		/*
		else
		{
			m_Chord.SubChord(m_Chord.RootIndex())->ChordRoot() = static_cast<unsigned char>(nKey);
			OnChangeRoot();
		}
		*/
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

void CChordScalePropPage::RegisterMidi() 
{
	ASSERT(m_cRegCount <= 1);
	if(m_fMidiInputTarget != None && m_cRegCount == 0)
	{
		++m_cRegCount;
		REGISTER_MIDI_IN(m_pIConductor, m_dwCookie)	
	}
}

void CChordScalePropPage::UnRegisterMidi()
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
STDMETHODIMP CChordScalePropPage::QueryInterface( REFIID riid, LPVOID *ppv )
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

STDMETHODIMP_(ULONG) CChordScalePropPage::AddRef(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CChordScalePropPage::Release(void)
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
void  CChordScalePropPage::CopyDataToTab( CPropChord*  pTabData )
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

void CChordScalePropPage::GetDataFromTab( const CPropChord* pTabData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if(pTabData)
	{
		m_Chord = *pTabData;
		m_bValidChord = true;
	}
	else
	{
		m_bValidChord = false;
	}
}

BOOL CChordScalePropPage::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	for(int i = 0; i < MAX_POLY; i++)
	{
		GetChordKeyboard(i)->SetLButtonUpCallback(KeyboardChangeCallback, this);
		GetScaleKeyboard(i)->SetLButtonUpCallback(KeyboardChangeCallback, this);
	}
	
	m_midichord4.AutoLoad(IDC_BUTTON_MIDICHORD4, this);
	m_midichord3.AutoLoad(IDC_BUTTON_MIDICHORD3, this);
	m_midichord2.AutoLoad(IDC_BUTTON_MIDICHORD2, this);
	/*
	m_midichord1.Create( "MIDIPLUG", 
									 BS_OWNERDRAW | BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
									 CRect(14,84,26,94), 
									 this, 
									 IDC_BUTTON_MIDICHORD1 );
									 */
	m_midichord1.AutoLoad(IDC_BUTTON_MIDICHORD1, this);
//	m_midichord1.LoadBitmaps( "MIDIPLUGU", "MIDIPLUGD");



	m_midiscale4.AutoLoad(IDC_BUTTON_MIDISCALE4, this);
	m_midiscale3.AutoLoad(IDC_BUTTON_MIDISCALE3, this);
	m_midiscale2.AutoLoad(IDC_BUTTON_MIDISCALE2, this);
	m_midiscale1.AutoLoad(IDC_BUTTON_MIDISCALE1, this);


//	m_midiroot.AutoLoad(IDC_BUTTON_MIDIROOT, this);


	m_shiftl4.AutoLoad(IDC_BUTTON_SHIFTL4, this);
	m_shiftl3.AutoLoad(IDC_BUTTON_SHIFTL3, this);
	m_shiftl2.AutoLoad(IDC_BUTTON_SHIFTL2, this);
	m_shiftl1.AutoLoad(IDC_BUTTON_SHIFTL1, this);

	m_shiftr4.AutoLoad(IDC_BUTTON_SHIFTR4, this);
	m_shiftr3.AutoLoad(IDC_BUTTON_SHIFTR3, this);
	m_shiftr2.AutoLoad(IDC_BUTTON_SHIFTR2, this);
	m_shiftr1.AutoLoad(IDC_BUTTON_SHIFTR1, this);

	m_play4.AutoLoad(IDC_BUTTON_PLAY4, this);
	m_play3.AutoLoad(IDC_BUTTON_PLAY3, this);
	m_play2.AutoLoad(IDC_BUTTON_PLAY2, this);
	m_play1.AutoLoad(IDC_BUTTON_PLAY1, this);

	m_playall.AutoLoad(IDC_BUTTON_PLAYALL, this);



	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CChordScalePropPage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CChordScalePropPage::OnDestroy() 
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


void CChordScalePropPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
/*
	CButton* pButton = GetButton(nIDCtl);
	if(pButton)
	{
		lpDrawItemStruct->itemState = pButton->GetChecked() ? ODS_SELECTED : ODS_DEFAULT;
	}
*/		
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);	
}

void CChordScalePropPage::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	CPropertyPage::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CChordScalePropPage::OnButtonMidichord1() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	SetMidiIn(Chord1);
}

void CChordScalePropPage::OnButtonMidichord2() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	SetMidiIn(Chord2);
}

void CChordScalePropPage::OnButtonMidichord3() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	SetMidiIn(Chord3);
}
void CChordScalePropPage::OnButtonMidichord4() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	SetMidiIn(Chord4);	
}

void CChordScalePropPage::OnButtonMidiroot() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	SetMidiIn(Root);
}

void CChordScalePropPage::OnButtonMidiscale1() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	SetMidiIn(Scale1);
}

void CChordScalePropPage::OnButtonMidiscale2() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	SetMidiIn(Scale2);
}

void CChordScalePropPage::OnButtonMidiscale3() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	SetMidiIn(Scale3);
}

void CChordScalePropPage::OnButtonMidiscale4() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	SetMidiIn(Scale4);
}

void CChordScalePropPage::OnButtonPlay1() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	PlaySubChord(0);
}

void CChordScalePropPage::OnButtonPlay2() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	PlaySubChord(1);	
}

void CChordScalePropPage::OnButtonPlay3() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	PlaySubChord(2);
}

void CChordScalePropPage::OnButtonPlay4() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	PlaySubChord(3);
}

void CChordScalePropPage::OnButtonPlayall() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	PlayAllChords();
}

void CChordScalePropPage::OnButtonShiftl1() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
//	m_keyschord1.Transpose(-1, true);
//	m_keysscale1.Transpose(-1, true);
	m_keyschord1.ShiftKeys(-1);
	m_keysscale1.ShiftKeys(-1);
}

void CChordScalePropPage::OnButtonShiftl2() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_keyschord2.ShiftKeys(-1);
	m_keysscale2.ShiftKeys(-1);
	
}

void CChordScalePropPage::OnButtonShiftl3() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_keyschord3.ShiftKeys(-1);
	m_keysscale3.ShiftKeys(-1);
	
}

void CChordScalePropPage::OnButtonShiftl4() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_keyschord4.ShiftKeys(-1);
	m_keysscale4.ShiftKeys(-1);
	
}

void CChordScalePropPage::OnButtonShiftr1() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_keyschord1.ShiftKeys(1);
//	m_keysscale1.ShiftKeys(1);
	
}

void CChordScalePropPage::OnButtonShiftr2() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_keyschord2.ShiftKeys(1);
//	m_keysscale2.ShiftKeys(1);
	
}

void CChordScalePropPage::OnButtonShiftr3() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_keyschord3.ShiftKeys(1);
//	m_keysscale3.ShiftKeys(1);
	
}

void CChordScalePropPage::OnButtonShiftr4() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_keyschord4.ShiftKeys(1);
//	m_keysscale4.ShiftKeys(1);
	
}

void CChordScalePropPage::OnCheckboxFlats() 
//
// this sets UseFlats for all chords, but looks at base chord to determine current state
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UINT nChecked = IsDlgButtonChecked(IDC_CHECKBOX_FLATS);
	UINT nState;
	if (m_Chord.Undetermined(m_Chord.RootIndex())  & UD_FLAT)
	{
		nState = 2;
	}
	else if (m_Chord.Base()->UseFlat())
	{
		nState = 1;
	}
	else nState = 0;
	if (nState != nChecked)
	{
		if (nChecked == 2)
		{
			CButton *pButton = (CButton *) GetDlgItem(IDC_CHECKBOX_FLATS);
			if (pButton)
			{
				pButton->SetButtonStyle(BS_AUTOCHECKBOX);
				pButton->SetCheck(nState);			
			}
		}
		else
		{
			for(int i = 0; i < CPropChord::MAX_POLY; i++)
			{
				m_Chord.Undetermined(i)  &= ~UD_FLAT;
				if (nChecked == 1)
				{
					m_Chord.SubChord(i)->UseFlat() = TRUE;
				}
				else 
				{
					m_Chord.SubChord(i)->UseFlat() = FALSE;
				}
			}
			if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
			{
				m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
			}	
		}
	}

}

void CChordScalePropPage::OnChangeEditName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	char szName[30];
	GetDlgItemText(IDC_EDIT_NAME,szName,sizeof(m_Chord.Name()));
	if (strcmp("",szName) && (strcmp(m_Chord.Name(),szName)))
	{
		strncpy(m_Chord.Name(),szName, DMPolyChord::MAX_NAME);
		m_Chord.Undetermined(m_Chord.RootIndex())  &= ~UD_NAME;
		if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
		{
			m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
		}	
	}

	
}

void CChordScalePropPage::DispatchKeyboardChange(CKeyboard* pKeyboard, short nKey)
{
	int i;
	if( (i = GetChordKeyboardIndex(pKeyboard))  >= 0 )
	{
		// send both CKeyboard* and index to save loakup
		OnNoteDownChordKeys(pKeyboard, i, nKey);
	}
	else if( (i = GetScaleKeyboardIndex(pKeyboard)) >= 0 )
	{
		// send both CKeyboard* and index to save loakup
		OnNoteDownScaleKeys(pKeyboard, i, nKey);
	}
}

int CChordScalePropPage::GetChordKeyboardIndex(const CKeyboard* pKeyboard)
{
	int i = 0;
	for(i; i < MAX_POLY; i++)
	{
		if(GetChordKeyboard(i) == pKeyboard)
		{
			break;
		}
	}
	return i >= MAX_POLY ? -1 : i;
}

int CChordScalePropPage::GetScaleKeyboardIndex(const CKeyboard* pKeyboard)
{
	int i = 0;
	for(i; i < MAX_POLY; i++)
	{
		if(GetScaleKeyboard(i) == pKeyboard)
		{
			break;
		}
	}
	return i >= MAX_POLY ? -1 : i;
}

void CChordScalePropPage::OnNoteDownChordKeys(CKeyboard* pKeyboard, int index, short nKey) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(0 <= index && index < MAX_POLY && pKeyboard);



	if (m_Chord.GetUndetermined(index) & UD_CHORDPATTERN)
	{
		m_Chord.Undetermined(index) &= ~UD_CHORDPATTERN;
		for (nKey = 0; nKey < 24; nKey++)
		{
			pKeyboard->SetNoteState(nKey,"",(1 && ((m_Chord.SubChord(index)->ChordPattern() >> nKey) & 1)) << 1);
		}
	}
	else
	{
		bool rootHasChanged = false;
		short root = static_cast<short>(m_Chord.SubChord(index)->ChordRoot());
		short note = nKey - pKeyboard->Transpose();
		if(nKey < root)
		{
			// new root
			rootHasChanged = true;
			m_Chord.SubChord(index)->ChordRoot() = static_cast<unsigned char>(nKey);
			// root is lower, but we want to keep same chord members so shift up
			m_Chord.SubChord(index)->ChordPattern() <<= (root - nKey);
			// add member for root note
			m_Chord.SubChord(index)->ChordPattern() |= 0x1;
			// transpose keyboard
			pKeyboard->Transpose(nKey, false);
			m_Chord.Undetermined(index) &= ~UD_CHORDPATTERN;
		}
		else if(nKey == root)
		{
			// root must be set so clear it
			pKeyboard->SetNoteState(nKey-pKeyboard->Transpose(),"",0);
			// new root
			rootHasChanged = true;
			// third of chord is now root
			root = static_cast<short>(m_Chord.SubChord(index)->Third(0));
			if(root < 0)
			{
				// no chord, set root to highest note
				root = 23;
				m_Chord.SubChord(index)->ChordRoot() = static_cast<unsigned char>(root);
				m_Chord.SubChord(index)->ChordPattern() = 0;
			}
			else
			{
				m_Chord.SubChord(index)->ChordRoot() = root + nKey;
				// shift chord to "root" position, this also gets rid of old root note
				m_Chord.SubChord(index)->ChordPattern() >>= root;
			}
			// transpose keyboard
			pKeyboard->Transpose(m_Chord.SubChord(index)->ChordRoot(), false);
			m_Chord.Undetermined(index) &= ~UD_CHORDPATTERN;
		}
		else
		{
			//  just update chord
			short note = nKey - pKeyboard->Transpose();
			m_Chord.SubChord(index)->ChordPattern() ^= (1 << note);
			m_Chord.Undetermined(index) &= ~UD_CHORDPATTERN;
			pKeyboard->SetNoteState(note,"",(1 && ((m_Chord.SubChord(index)->ChordPattern() >> note) & 1)) << 1);
		}
		if(rootHasChanged)
		{
			// need to redraw chord and scale keyboards
			for (int nX = 0; nX < 24; nX++)
			{
				short nGrayed;
				if (m_Chord.GetUndetermined(m_Chord.RootIndex()) & UD_CHORDPATTERN) 
					nGrayed = 4;
				else 
					nGrayed = 0;
				short nState = nGrayed;
				if ((m_Chord.SubChord(index)->ChordPattern() >> nX) & 1) nState += 2;
				pKeyboard->SetNoteState(nX,"",nState);
				DrawScaleNote(&m_Chord,  index, (nX + pKeyboard->Transpose()) % 24);
				DrawScaleNote(&m_Chord,  index, (nX, pKeyboard->Transpose() + 12) % 24);
			}
			if(index == m_Chord.RootIndex())
			{
				OnChangeRoot();
			}
		}
		else
		{
			DrawScaleNote(&m_Chord,  index, nKey);
			DrawScaleNote(&m_Chord,  index, (nKey + 12) % 24);
		}
	}


	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
	}
}


void CChordScalePropPage::OnNoteDownScaleKeys(CKeyboard* pKeyboard, int index, short nKey) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT(0 <= index && index < MAX_POLY && pKeyboard);

	short nOtherKey = (nKey + 12) % 24;
	if (m_Chord.Undetermined(index) & UD_SCALEPATTERN)
	{
		m_Chord.Undetermined(index) &= ~UD_SCALEPATTERN;
//		m_Chord.m_dwScalePattern ^= (1 << nKey);
		for (nKey = 0; nKey < 24; nKey++)
		{
			DrawScaleNote(&m_Chord, index, nKey);
		}
	}
	else
	{
		m_Chord.SubChord(index)->ScalePattern() ^= (1 << nKey);
		m_Chord.SubChord(index)->ScalePattern() ^= (1 << nOtherKey);
		DrawScaleNote(&m_Chord, index, nKey);
		DrawScaleNote(&m_Chord, index, nOtherKey);
	}
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
	}	
}

void CChordScalePropPage::DrawScaleNote(CPropChord* pChord, int index, short nX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	short nGrayed;
	if (pChord->GetUndetermined(index) & UD_SCALEPATTERN) nGrayed = 4;
	else nGrayed = 0;
	DWORD dwChord = pChord->SubChord(index)->ChordPattern();
	dwChord = Rotate24(dwChord, pChord->SubChord(index)->ChordRoot() % 12);
	short nState = nGrayed;
	
	if ((pChord->SubChord(index)->ScalePattern() >> nX) & 1) 
	{
		nState = 2;
//		if ((dwChord >> nX) & 1) nState += 2;
	}
	else
	{
		// draw chord not in scale "blue" note
		if ((dwChord >> nX) & 1) nState = 1;
	}
	GetScaleKeyboard(index)->SetNoteState(nX,"",nState);
}

void CChordScalePropPage::SetChord(CPropChord * pChord)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	char str[30];
	m_Chord = *pChord;
	
	if( IsWindow( m_hWnd ) == 0 ) return;

	if (pChord->GetUndetermined(pChord->RootIndex()) & UD_NAME)
	{
		SetDlgItemText(IDC_EDIT_NAME,"");
	}
	else
	{
		SetDlgItemText(IDC_EDIT_NAME,pChord->Name());
	}
	pChord->RootToString(str, pChord->RootIndex());
	SetDlgItemText(IDC_EDIT_ROOT,str);
	SetCheckBox(pChord, UD_FLAT, IDC_USEFLATS, pChord->Base()->UseFlat() && 1);
	SetCheckBox(pChord, UD_FLAT, IDC_USESHARPS, !(pChord->Base()->UseFlat() && 1));
	short nGrayed;
	if (pChord->GetUndetermined(pChord->RootIndex()) & UD_CHORDPATTERN) nGrayed = 4;
	else nGrayed = 0;
	int nX;
	for(int n = 0; n < CPropChord::MAX_POLY; n++)
	{
		CKeyboard* pKeyboard = GetChordKeyboard(n);
		DMChord* pdm = pChord->SubChord(n);
		pKeyboard->Transpose(pdm->ChordRoot(), true);
		for (nX = 0; nX < 24; nX++)
		{
			short nState = nGrayed;
			if ((pChord->SubChord(n)->ChordPattern() >> nX) & 1) nState += 2;
			GetChordKeyboard(n)->SetNoteState(nX,"",nState);
			DrawScaleNote(pChord, n, nX);
		}
	}
}

void CChordScalePropPage::SetCheckBox(CPropChord * pChord, DWORD dwUDFlag, UINT nDlgID, UINT nState)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	if (pChord->GetUndetermined(pChord->RootIndex()) & dwUDFlag)
	{
		CButton *pButton = (CButton *) GetDlgItem(nDlgID);
		if (pButton)
		{
//			pButton->SetButtonStyle(BS_AUTO3STATE);
			pButton->SetCheck(0);		
		}
	}
	else
	{
		CButton *pButton = (CButton *) GetDlgItem(nDlgID);
		if (pButton)
		{
//			pButton->SetButtonStyle(BS_AUTOCHECKBOX);
			pButton->SetCheck(nState);
		}
	}

}

BOOL CChordScalePropPage::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	RegisterMidi();
	
	return CPropertyPage::OnSetActive();
}


void CChordScalePropPage::OnKillFocus(CWnd* pNewWnd) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CPropertyPage::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	
}


void CChordScalePropPage::OnChangeRoot() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	char str[30];
	m_Chord.RootToString(str, m_Chord.RootIndex());
	SetDlgItemText(IDC_EDIT_ROOT, str);
	m_Chord.Undetermined(m_Chord.RootIndex())  &= ~UD_CHORDROOT;
	for (int nX = 0; nX < 24; nX++)
	{
		DrawScaleNote(&m_Chord, m_Chord.RootIndex(), nX);
	}
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
	}	

}

void CChordScalePropPage::OnUseflats() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	for(int i = 0; i < CPropChord::MAX_POLY; i++)
	{
		m_Chord.Undetermined(i) &= ~UD_FLAT;
		m_Chord.SubChord(i)->UseFlat() = 1;
	}
	OnChangeRoot();
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
	}	
}

void CChordScalePropPage::OnUsesharps() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	for(int i = 0; i < CPropChord::MAX_POLY; i++)
	{
		m_Chord.Undetermined(i) &= ~UD_FLAT;
		m_Chord.SubChord(i)->UseFlat() = 0;
	}
	OnChangeRoot();
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Chord);
	}	
}

void CChordScalePropPage::SetMidiIn(int keyboard)
{
	if(m_fMidiInputTarget == keyboard)
	{
			CButton* pButton = (CButton*)GetDlgItem(m_MidiInIds[m_fMidiInputTarget]);
			pButton->SetState(FALSE);
			m_fMidiInputTarget  = None;
			UnRegisterMidi();
			return;
	}

	if(m_fMidiInputTarget != None)
	{
			CButton* pButton = (CButton*)GetDlgItem(m_MidiInIds[m_fMidiInputTarget]);
			pButton->SetState(FALSE);
	}
	
	m_fMidiInputTarget = keyboard;
	CButton* pButton = (CButton*)GetDlgItem(m_MidiInIds[m_fMidiInputTarget]);
	pButton->SetState(TRUE);
	RegisterMidi();
}

CButton* CChordScalePropPage::GetButton(int id)
{
	CButton* pButton = 0;
	switch(id)
	{
	case IDC_BUTTON_MIDICHORD1:
		pButton = dynamic_cast<CButton*>(&m_midichord1);
		break;
	case IDC_BUTTON_MIDICHORD2:
		pButton = dynamic_cast<CButton*>(&m_midichord2);
		break;
	case IDC_BUTTON_MIDICHORD3:
		pButton = dynamic_cast<CButton*>(&m_midichord3);
		break;
	case IDC_BUTTON_MIDICHORD4:
		pButton = dynamic_cast<CButton*>(&m_midichord4);
		break;
	case IDC_BUTTON_MIDISCALE1:
		pButton = dynamic_cast<CButton*>(&m_midiscale1);
		break;
	case IDC_BUTTON_MIDISCALE2:
		pButton = dynamic_cast<CButton*>(&m_midiscale3);
		break;
	case IDC_BUTTON_MIDISCALE3:
		pButton = dynamic_cast<CButton*>(&m_midiscale3);
		break;
	case IDC_BUTTON_MIDISCALE4:
		pButton = dynamic_cast<CButton*>(&m_midiscale4);
		break;
	}
	return pButton;
}

void CChordScalePropPage::PlaySubChord(int index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	IDMUSProdConductor *pConductor = NULL;
	IDMUSProdComponent* pIComponent = NULL;
	if (m_pPropPageMgr->m_pIFramework)
	{
		if( SUCCEEDED ( m_pPropPageMgr->m_pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
		{
			pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pConductor );
			if (pConductor)
			{
				short nX;
				DWORD dwTime = 0;
				pConductor->PlayMIDIEvent( MIDI_PCHANGE, 0, 0, 0 );
				pConductor->PlayMIDIEvent( MIDI_PCHANGE+1, 10, 0, 0 );
				for (nX = 0; nX < 24; nX++)
				{
					if ((m_Chord.SubChord(index)->ChordPattern() >> nX) & 1)
					{
						 unsigned char note = static_cast<unsigned char>(MIDI_BASEPITCH + nX + m_Chord.SubChord(index)->ChordRoot());
						pConductor->PlayMIDIEvent( MIDI_NOTEON, 
							note, 120, dwTime );
						dwTime += CHORDNOTE_DUR;
						pConductor->PlayMIDIEvent( MIDI_NOTEOFF, 
							note, 120, dwTime );
					}
				}
				dwTime += CHORDNOTE_DELAY;
				for (nX = 0; nX < 25; nX++)
				{
					if ((m_Chord.SubChord(index)->ScalePattern() >> nX) & 1)
					{
						unsigned char note = static_cast<unsigned char>(MIDI_BASEPITCH + nX+ m_Chord.SubChord(index)->ScaleRoot());
						pConductor->PlayMIDIEvent( MIDI_NOTEON+1, 
							note,  100, dwTime );
						dwTime += SCALENOTE_DUR;
						pConductor->PlayMIDIEvent( MIDI_NOTEOFF+1, 
							note,  100, dwTime );
						dwTime += SCALENOTE_DELAY;
					}
				}
				pConductor->Release();
			}
			pIComponent->Release();
		}
	}
}

void CChordScalePropPage::PlayAllChords()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	IDMUSProdConductor *pConductor = NULL;
	IDMUSProdComponent* pIComponent = NULL;
	if (m_pPropPageMgr->m_pIFramework)
	{
		if( SUCCEEDED ( m_pPropPageMgr->m_pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
		{
			pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pConductor );
			if (pConductor)
			{
				short nX;
				DWORD dwTime = 0;
				pConductor->PlayMIDIEvent( MIDI_PCHANGE, 0, 0, 0 );
				pConductor->PlayMIDIEvent( MIDI_PCHANGE+1, 10, 0, 0 );
				for(int index = 0; index < MAX_POLY; index++)
				{
					for (nX = 0; nX < 24; nX++)
					{
						if ((m_Chord.SubChord(index)->ChordPattern() >> nX) & 1)
						{
							 unsigned char note = static_cast<unsigned char>(MIDI_BASEPITCH + nX + m_Chord.SubChord(index)->ChordRoot());
							pConductor->PlayMIDIEvent( MIDI_NOTEON, 
								note, 120, dwTime );
							dwTime += CHORDNOTE_DUR;
							pConductor->PlayMIDIEvent( MIDI_NOTEOFF, 
								note, 120, dwTime + CHORDNOTE_DUR*2 );
						}
					}
				}
				pConductor->Release();
			}
			pIComponent->Release();
		}
	}

}
