// ChordScalePropPage.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "..\includes\DMUSProd.h"
#include "..\includes\Conductor.h"
#include "DllBasePropPageManager.h"
#include "PropChord.h"
#include "PropPageMgr.h"
#include "chordmapstripmgr.h"
#include "ChordScalePropPage.h"
#include "IllegalChord.h"
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////// Callbacks for CKeyboard Events
//
void KeyboardChangeCallback(CKeyboard* pKeyboard, void* hint, short nKey)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CChordScalePropPage* pdlg = (CChordScalePropPage*)hint;
	pdlg->DispatchKeyboardChange(pKeyboard, nKey);
}

void RMouseKeyboardCallback(CKeyboard* pKeyboard, void* hint, short nKey, CPoint& ptMouse)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CChordScalePropPage* pdlg = (CChordScalePropPage*)hint;
	pdlg->DispatchRightMouseKeyboard(pKeyboard, nKey, ptMouse);
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
	m_pIPerformance = 0;
	m_cRef = 0;
	m_cRegCount = 0;
	m_bValidChord = false;
	m_fNeedToDetach = FALSE;
	m_fLockAllScales = true;
	m_fSyncLevelOneToAll = true;
	AddRef();

	m_MidiInIds[0] = IDC_BUTTON_MIDICHORD1;
	m_MidiInIds[1] = IDC_BUTTON_MIDICHORD2;
	m_MidiInIds[2] = IDC_BUTTON_MIDICHORD3;
	m_MidiInIds[3] = IDC_BUTTON_MIDICHORD4;

	m_MidiInIds[4] = IDC_BUTTON_MIDISCALE1;
	m_MidiInIds[5] = IDC_BUTTON_MIDISCALE2;
	m_MidiInIds[6] = IDC_BUTTON_MIDISCALE3;
	m_MidiInIds[7] = IDC_BUTTON_MIDISCALE4;

	m_MidiInIds[8] = 0;

	m_nLastKeyboardLevel = -1;

	m_bNotWarn = FALSE;
}

CChordScalePropPage::~CChordScalePropPage()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	UnRegisterMidi();	// make sure we're unregistered
	if(m_pIConductor)
	{
		m_pIConductor->Release();
	}
	if(m_pIPerformance)
	{
		m_pIPerformance->Release();
	}
}

void CChordScalePropPage::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChordScalePropPage)
	DDX_Control(pDX, IDC_SPIN_ROOT, m_spinRoot);
	DDX_Control(pDX, IDC_EDIT_NAME, m_chordname);
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
	ON_BN_CLICKED(IDC_BUTTON_MIDISCALE1, OnButtonMidiscale1)
	ON_BN_CLICKED(IDC_BUTTON_MIDISCALE2, OnButtonMidiscale2)
	ON_BN_CLICKED(IDC_BUTTON_MIDISCALE3, OnButtonMidiscale3)
	ON_BN_CLICKED(IDC_BUTTON_MIDISCALE4, OnButtonMidiscale4)
	ON_BN_CLICKED(IDC_BUTTON_PLAY1, OnButtonPlay1)
	ON_BN_CLICKED(IDC_BUTTON_PLAY2, OnButtonPlay2)
	ON_BN_CLICKED(IDC_BUTTON_PLAY3, OnButtonPlay3)
	ON_BN_CLICKED(IDC_BUTTON_PLAY4, OnButtonPlay4)
	ON_BN_CLICKED(IDC_BUTTON_PLAYALL, OnButtonPlayall)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeEditName)
	ON_WM_KILLFOCUS()
	ON_BN_CLICKED(IDC_USEFLATS, OnUseflats)
	ON_BN_CLICKED(IDC_USESHARPS, OnUsesharps)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ROOT, OnDeltaposSpinRoot)
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

#define MIDI_BASEPITCH		48

HRESULT CChordScalePropPage::OnMidiMsg(REFERENCE_TIME rtTime,
									   BYTE bStatus, 
									   BYTE bData1, 
									   BYTE bData2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	UNREFERENCED_PARAMETER(rtTime);

	// Note On
	if((int)(bStatus & 0xF0) == (int)0x90)
	{

		char Text[128];
		sprintf(Text, "Inside  PropPageChord::OnMidiMsg going to play %d note on\n", bData1); 
		TRACE0(Text);

		// set note
		short nKey = static_cast<short>(bData1 - KeyOCXTrans);

		if(m_fMidiInputTarget < 4)
		{
			bool rootHasChanged = m_fMidiInputTarget == 0 && (nKey <= m_Chord.Base()->ChordRoot());
			DispatchKeyboardChange(GetChordKeyboard(m_fMidiInputTarget), nKey);
			if(rootHasChanged)
			{
//				GetChordKeyboard(m_fMidiInputTarget)->SetLowerBound(GetChordKeyboard(m_fMidiInputTarget)->Transpose());
			}
		}
		else if(m_fMidiInputTarget < 8)
		{
			DispatchKeyboardChange(GetScaleKeyboard(m_fMidiInputTarget - 4), nKey);
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
	m_pIConductor->PlayMIDIEvent( MIDI_CCHANGE, 7, 100, 0);
	m_pIConductor->PlayMIDIEvent( MIDI_CCHANGE, 10, 63, 0);
	m_pIConductor->PlayMIDIEvent( MIDI_CCHANGE, 32, 0, 0);
	m_pIConductor->PlayMIDIEvent( MIDI_CCHANGE, 0, 0, 0);
	m_pIConductor->PlayMIDIEvent( MIDI_PCHANGE, 0, 0, 0 );
	m_pIConductor->PlayMIDIEvent( MIDI_PCHANGE+1, 10, 0, 0 );

	m_pIConductor->PlayMIDIEvent(static_cast<BYTE>(bStatus & 0xF0),
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

BOOL CChordScalePropPage::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	
	for(int i = 0; i < MAX_POLY; i++)
	{
		GetChordKeyboard(i)->SetLButtonUpCallback(KeyboardChangeCallback, this);
		GetScaleKeyboard(i)->SetLButtonUpCallback(KeyboardChangeCallback, this);
		GetChordKeyboard(i)->SetRButtonDownCallback(RMouseKeyboardCallback, this);
		GetScaleKeyboard(i)->SetRButtonDownCallback(RMouseKeyboardCallback, this);
		GetScaleKeyboard(i)->SetExtent(12);
		GetChordKeyboard(i)->SetExtent(48);
		GetChordKeyboard(i)->SetRootKeys(24);
		GetScaleKeyboard(i)->SetRootKeys(0);
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


	m_play4.AutoLoad(IDC_BUTTON_PLAY4, this);
	m_play3.AutoLoad(IDC_BUTTON_PLAY3, this);
	m_play2.AutoLoad(IDC_BUTTON_PLAY2, this);
	m_play1.AutoLoad(IDC_BUTTON_PLAY1, this);

	m_playall.AutoLoad(IDC_BUTTON_PLAYALL, this);



	CPropertyPage::OnInitDialog();

	m_chordname.LimitText(11);

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

void CChordScalePropPage::OnChangeEditName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	char szName[30];
	GetDlgItemText(IDC_EDIT_NAME,szName,sizeof(szName));
	if (strcmp("",szName) && (strcmp(m_Chord.Name(),szName)))
	{
		strncpy(m_Chord.Name(),szName, DMPolyChord::MAX_NAME);
		m_Chord.Undetermined(m_Chord.RootIndex())  &= ~UD_NAME;
		UpdateChordData();
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

	if(nKey < 0)
	{
		return;	// root too low
	}

	if (m_Chord.GetUndetermined(index) & UD_CHORDPATTERN)
	{
		m_Chord.Undetermined(index) &= ~UD_CHORDPATTERN;
		for (short iKey = 0; iKey < 24; iKey++)
		{
			pKeyboard->SetNoteState(iKey,"",static_cast<short>((1 && ((m_Chord.SubChord(index)->ChordPattern() >> iKey) & 1)) << 1));
		}
	}
	else
	{
		bool rootHasChanged = false;
		short root = static_cast<short>(m_Chord.SubChord(index)->ChordRoot());
		if(nKey < root)
		{
			// root is lower, but we want to keep same chord members so shift up
			// but first make sure that we're not increasing span of chord > 2 octaves
			DWORD shift = root - nKey;
			DWORD pattern = 0xFFFFFFFF << (23 - shift);
			bool bTooHigh = m_Chord.SubChord(index)->ChordPattern() != 0 // an empty chord can never be too high
								&& (HighestBit(m_Chord.SubChord(index)->ChordPattern()) + root  - nKey) >= 24;

			if(root - nKey > 23)
			{
				WarnUser();
			}
			else if(!bTooHigh || (pattern & m_Chord.SubChord(index)->ChordPattern()) == 0)
			{
				// new root
				m_Chord.SubChord(index)->ChordRoot() = static_cast<unsigned char>(nKey);
				rootHasChanged = true;
				m_Chord.SubChord(index)->ChordPattern() <<= shift;
				// make sure anything higher than two octaves of pattern is cutoff.
				m_Chord.SubChord(index)->ChordPattern() &= 0x00ffffff;
				// add member for root note
				m_Chord.SubChord(index)->ChordPattern() |= 0x1;
				m_Chord.Undetermined(index) &= ~UD_CHORDPATTERN;
			}
			else if(pKeyboard->GetNoteState(static_cast<short>(nKey)) == 2)
			{
				// allow user to turn off bit patterns >= 24 semitones from root
				pKeyboard->SetNoteState(static_cast<short>(nKey), "", 0);
			}
			else
			{
				WarnUser();
			}
		}
		else if(m_Chord.SubChord(index)->ChordPattern() == 0)
		{
			// this is the first note of chord hence it is the root.
			rootHasChanged = true;
			m_Chord.SubChord(index)->ChordRoot() = static_cast<BYTE>(nKey);

			m_Chord.SubChord(index)->ChordPattern() = 1;
			m_Chord.Undetermined(index) &= ~UD_CHORDPATTERN;
		}
		else if(nKey == root)
		{
			// root must be set so clear it
			pKeyboard->SetNoteState(static_cast<short>(nKey-pKeyboard->Transpose()),"",0);
			// new root
			rootHasChanged = true;
			// third of chord is now root
			root = static_cast<short>(m_Chord.SubChord(index)->Third(0));
			if(root < 0)
			{
				// no chord, set root to lowest note
				root = 0;
				m_Chord.SubChord(index)->ChordRoot() = static_cast<unsigned char>(root);
				m_Chord.SubChord(index)->ChordPattern() = 0;
			}
			else
			{
				m_Chord.SubChord(index)->ChordRoot() = static_cast<BYTE>(root + nKey);
				// shift chord to "root" position, this also gets rid of old root note
				m_Chord.SubChord(index)->ChordPattern() >>= root;
			}
			m_Chord.Undetermined(index) &= ~UD_CHORDPATTERN;
		}
		else
		{
			//  just update chord
			// if further away from root than 24 semitones, don't allow it
			// TODO: warn user
			DWORD shift = nKey - m_Chord.SubChord(index)->ChordRoot();
			if(shift < 24)
			{
				shift = 1 << shift;
				m_Chord.SubChord(index)->ChordPattern() ^= shift;
				m_Chord.Undetermined(index) &= ~UD_CHORDPATTERN;
				short state = static_cast<short>(m_Chord.SubChord(index)->ChordPattern() & shift ? 2 : 0);
				pKeyboard->SetNoteState(static_cast<short>(nKey),"", state);
			}
			else if(pKeyboard->GetNoteState(static_cast<short>(nKey)) == 2)
			{
				// allow user to turn off bit patterns >= 24 semitones from root
				pKeyboard->SetNoteState(static_cast<short>(nKey), "", 0);
			}
			else
			{
				WarnUser();
			}
		}


		if(rootHasChanged)
		{
			// need to redraw chord and scale keyboards
			int root = m_Chord.SubChord(index)->ChordRoot();
			for (int nX = root; nX < root + 24; nX++)
			{
				if(nX >= 48)
					break;
				short nState;
				if (m_Chord.GetUndetermined(m_Chord.RootIndex()) & UD_CHORDPATTERN) 
					nState = 4;
				else 
					nState = 0;
				if ((m_Chord.SubChord(index)->ChordPattern() >> (nX-root)) & 1) 
					nState += 2;
				pKeyboard->SetNoteState(static_cast<short>(nX),"",nState);

			}
			DrawScale(&m_Chord, index);
			if(index == m_Chord.RootIndex())
			{
				OnChangeRoot( false );
			}
		}
		else
		{
			short	scaleKey = static_cast<short>(nKey % 12);
			DrawScale(&m_Chord, index, scaleKey);
		}
	}

	if( m_fSyncLevelOneToAll
	&&  index == 0 )
	{
		for( int i = 0; i < MAX_POLY; i++ )
		{
			if( i != index )
			{
				m_Chord.SubChord(i)->ChordPattern() = m_Chord.SubChord(index)->ChordPattern();
				m_Chord.SubChord(i)->ChordRoot() = m_Chord.SubChord(index)->ChordRoot();
				m_Chord.Undetermined(i) &= ~UD_CHORDPATTERN;
			}
		}
		SetChord(&m_Chord);
	}
	UpdateChordData();
}


void CChordScalePropPage::OnNoteDownScaleKeys(CKeyboard* pKeyboard, int index, short nKey) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT(0 <= index && index < MAX_POLY && pKeyboard);

//	nKey = nKey - pKeyboard->Transpose();
	if(nKey < 0)
		nKey += 12;
	if(nKey > 23)
		nKey -= 12;
	short nOtherKey = static_cast<short>((nKey + 12) % 24);

	if (m_Chord.Undetermined(index) & UD_SCALEPATTERN)
	{
		m_Chord.Undetermined(index) &= ~UD_SCALEPATTERN;
		DrawScale(&m_Chord, index);
	}
	else
	{
		DWORD dwScalePattern = m_Chord.SubChord(index)->ScalePattern();
		DWORD dwScaleRoot = m_Chord.SubChord(index)->ScaleRoot() % 12;
		dwScalePattern = Rotate24(dwScalePattern, dwScaleRoot);
		dwScalePattern ^= (1 << nKey);
		dwScalePattern ^= (1 << nOtherKey);
		dwScalePattern = Rotate24(dwScalePattern, 12-dwScaleRoot);
		m_Chord.SubChord(index)->ScalePattern() = dwScalePattern;
		DrawScale(&m_Chord, index);
	}

	if( m_fSyncLevelOneToAll
	&&  index == 0 )
	{
		for( int i = 0; i < MAX_POLY; i++ )
		{
			if( i != index )
			{
				m_Chord.SubChord(i)->ScalePattern() = m_Chord.SubChord(index)->ScalePattern();
				m_Chord.Undetermined(i) &= ~UD_SCALEPATTERN;
			}
		}
		SetChord(&m_Chord);
	}
	UpdateChordData();
}

void CChordScalePropPage::DrawScale(CPropChord* pChord, int index, int note)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	short nGrayed;
	if (pChord->GetUndetermined(index) & UD_SCALEPATTERN) nGrayed = 4;
	else nGrayed = 0;

	// match bits
	DWORD dwScale = pChord->SubChord(index)->ScalePattern();
	DWORD dwChord = pChord->SubChord(index)->ChordPattern();
	DWORD dwBlueBits = BlueBits(dwChord, pChord->SubChord(index)->ChordRoot(),
													dwScale, pChord->SubChord(index)->ScaleRoot()
													);

	// draw scale
	int lower = (note == -1) ? 0 : note;
	int upper = (note == -1) ? 12 : note + 1;
	for(int i = lower; i < upper; i++)
	{
		short nState = nGrayed;
		if(dwScale & (1 << i))
		{
			nState = 2;
		}
		else if(dwBlueBits & (1 << i))
		{
			nState = 1;
		}
		GetScaleKeyboard(index)->SetNoteState(static_cast<short>(i),"",nState);
	}
}

void CChordScalePropPage::UpdateChordData(void)
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

void CChordScalePropPage::SetChordData(CChordScalePropPageData* pChordScalePropPageData)
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
		GetChordKeyboard(n)->ClearKeys();
		GetScaleKeyboard(n)->ClearKeys();

		int root = static_cast<int>(pChord->SubChord(n)->ChordRoot());
		for (nX = root; nX < root+24; nX++)
		{
			if(nX >= 48)
				break;
			short nState = nGrayed;
			if ((pChord->SubChord(n)->ChordPattern() >> (nX-root)) & 1) nState += 2;
			GetChordKeyboard(n)->SetNoteState(static_cast<short>(nX),"",nState);
		}
		DrawScale(pChord, n);
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


void CChordScalePropPage::OnChangeRoot( bool fUpdateChordData ) 
{

	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	char str[30];
	DMChord* pSub = m_Chord.SubChord(m_Chord.RootIndex());
	while(pSub->ChordRoot() > 23)
	{
		pSub->ChordRoot() -= 12;
		AfxMessageBox(IDS_ILLEGALCHORD);
		SetChord(&m_Chord);
	}
	m_Chord.RootToString(str, m_Chord.RootIndex());
	SetDlgItemText(IDC_EDIT_ROOT, str);
	m_Chord.Undetermined(m_Chord.RootIndex())  &= ~UD_CHORDROOT;
	DrawScale(&m_Chord, m_Chord.RootIndex());

	if( fUpdateChordData )
	{
		UpdateChordData();
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
	OnChangeRoot( true );
	UpdateChordData();
}

void CChordScalePropPage::OnUsesharps() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	for(int i = 0; i < CPropChord::MAX_POLY; i++)
	{
		m_Chord.Undetermined(i) &= ~UD_FLAT;
		m_Chord.SubChord(i)->UseFlat() = 0;
	}
	OnChangeRoot( true );
	UpdateChordData();
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

	short nX;
	DWORD dwTime = 0;

	ChangePatch(0,0,0,0);

	for (nX = 0; nX < 24; nX++)
	{
		if ((m_Chord.SubChord(index)->ChordPattern() >> nX) & 1)
		{
			 unsigned char note = static_cast<unsigned char>(MIDI_BASEPITCH + nX + m_Chord.SubChord(index)->ChordRoot());
			PlayNote(0, note, 120, dwTime, CHORDNOTE_DUR); 
			dwTime += CHORDNOTE_DUR;
		}
	}
	dwTime += CHORDNOTE_DELAY;
	for (nX = 0; nX < 25; nX++)
	{
		if ((m_Chord.SubChord(index)->ScalePattern() >> nX) & 1)
		{
			unsigned char note = static_cast<unsigned char>(MIDI_BASEPITCH + nX+ m_Chord.SubChord(index)->ScaleRoot());
			PlayNote(0, note, 120, dwTime, SCALENOTE_DUR); 
			dwTime += SCALENOTE_DUR;
		}
	}
}

void CChordScalePropPage::PlayAllChords()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	short nX;
	DWORD dwTime = 0;

	ChangePatch(0,0,0,0);

	for(int index = 0; index < MAX_POLY; index++)
	{
		for (nX = 0; nX < 24; nX++)
		{
			if ((m_Chord.SubChord(index)->ChordPattern() >> nX) & 1)
			{
				 unsigned char note = static_cast<unsigned char>(MIDI_BASEPITCH + nX + m_Chord.SubChord(index)->ChordRoot());
				PlayNote(0, note, 120, dwTime, CHORDNOTE_DUR);
				dwTime += CHORDNOTE_DUR;
			}
		}
		dwTime += CHORDNOTE_DUR*2;
	}
}

void CChordScalePropPage::OnDeltaposSpinRoot(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	// check that subchords is in range
	for(int j = 0; j < DMPolyChord::MAX_POLY; j++)
	{
		short chordRoot = static_cast<short>(m_Chord.SubChord(j)->ChordRoot() - pNMUpDown->iDelta);
		DWORD pattern = m_Chord.SubChord(j)->ChordPattern();

		// skip empty patterns
		if(!pattern)
		{
			continue;
		}

		if(chordRoot < 0)
		{
			return;	// out of range
		}

		// check if highest note is in range
		short highestNote = 0;
		for(int k = 0; k < sizeof(DWORD)*8; k++)
		{
			if(pattern & (1 << k))
			{
				highestNote = k;
			}
		}
		if(highestNote + chordRoot >= 48)
		{
			return;	// out of range
		}
	}

	for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
	{
		short chordRoot = static_cast<short>(m_Chord.SubChord(i)->ChordRoot() - pNMUpDown->iDelta);
		bool changeScale = true;
		if (chordRoot < 0 || m_Chord.SubChord(i)->ChordPattern() == 0) 
		{
			chordRoot = 0;
			changeScale = false;
		}
		/*
		if (chordRoot  > 23) 
		{
			chordRoot = 23;
			changeScale = false;
		}
		*/
		m_Chord.SubChord(i)->ChordRoot() = (BYTE) chordRoot;

		if(changeScale)
		{
			if( m_fLockAllScales == false )
			{
				m_Chord.SubChord(i)->ScalePattern() = Rotate24(m_Chord.SubChord(i)->ScalePattern(), -pNMUpDown->iDelta);
			}
		}
	}

	char str[10];
	m_Chord.RootToString(str, m_Chord.RootIndex());
	SetDlgItemText(IDC_EDIT_ROOT, str);
	m_Chord.Undetermined(m_Chord.RootIndex())  &= ~UD_CHORDROOT;

	SetChord(&m_Chord);
	OnChangeRoot( true );

	CWnd* pWnd = GetDlgItem(IDC_EDIT_ROOT);
	pWnd->SetFocus();

	*pResult = 0;
}

HRESULT CChordScalePropPage::PlayNote(BYTE ch, BYTE key, BYTE vel, DWORD startTime, DWORD dur)
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
	pMidi->bStatus = static_cast<BYTE>(0x90 + (ch&0xf));	// note on
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
		pMidi->bStatus = static_cast<BYTE>(0x80 + (ch&0xf));	// note on
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

HRESULT CChordScalePropPage::ChangePatch(BYTE ch, BYTE inst, BYTE msb, BYTE lsb)
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

BOOL CChordScalePropPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	int to, from, whatop;
	bool bModified = false;

	ComputeCopyOp((WORD)wParam, from, to, whatop);

	if(whatop > Nop)
	{
		switch(whatop)
		{
		case CopyChord:
			TRACE("CopyChord from %d to %d\n", from+1, to+1);
			if(to != All)
			{
				if(to == m_Chord.RootIndex() && m_Chord[from]->ChordRoot() > 23)
				{
					// need to make sure root is legal
					AfxMessageBox(IDS_ILLEGALCHORD3);
					break;
				}
				m_Chord[to]->ChordPattern() = m_Chord[from]->ChordPattern();
				m_Chord[to]->ChordRoot() = m_Chord[from]->ChordRoot();
			}
			else
			{
				for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
				{
					if(i == from)
						continue;
					if(i == m_Chord.RootIndex() && m_Chord[from]->ChordRoot() > 23)
					{
						// need to make sure root is legal
						AfxMessageBox(IDS_ILLEGALCHORD3);
						continue;
					}
					m_Chord[i]->ChordPattern() = m_Chord[from]->ChordPattern();
					m_Chord[i]->ChordRoot() = m_Chord[from]->ChordRoot();
				}
			}
			bModified = true;
			break;

		case CopyScale:
			TRACE("CopyScale from %d to %d\n", from+1, to+1);
			if(to != All)
			{
				m_Chord[to]->ScalePattern() = m_Chord[from]->ScalePattern();
				m_Chord[to]->ScaleRoot() = m_Chord[from]->ScaleRoot();
			}
			else
			{
				for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
				{
					if(i == from)
						continue;
					m_Chord[i]->ScalePattern() = m_Chord[from]->ScalePattern();
					m_Chord[i]->ScaleRoot() = m_Chord[from]->ScaleRoot();
				}
			}
			bModified = true;
			break;
		
		case CopyChordScale:
			TRACE("CopyChordScale from %d to %d\n", from+1, to+1);
			if(to != All)
			{
				if(to == m_Chord.RootIndex() && m_Chord[from]->ChordRoot() > 23)
				{
					// need to make sure root is legal
					AfxMessageBox(IDS_ILLEGALCHORD3);
					break;
				}
				m_Chord[to]->ChordPattern() = m_Chord[from]->ChordPattern();
				m_Chord[to]->ChordRoot() = m_Chord[from]->ChordRoot();
				m_Chord[to]->ScalePattern() = m_Chord[from]->ScalePattern();
				m_Chord[to]->ScaleRoot() = m_Chord[from]->ScaleRoot();
			}
			else
			{
				for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
				{
					if(i == from)
						continue;
					if(i == m_Chord.RootIndex() && m_Chord[from]->ChordRoot() > 23)
					{
						// need to make sure root is legal
						AfxMessageBox(IDS_ILLEGALCHORD3);
						continue;
					}
					m_Chord[i]->ChordPattern() = m_Chord[from]->ChordPattern();
					m_Chord[i]->ChordRoot() = m_Chord[from]->ChordRoot();
					m_Chord[i]->ScalePattern() = m_Chord[from]->ScalePattern();
					m_Chord[i]->ScaleRoot() = m_Chord[from]->ScaleRoot();
				}
			}
			bModified = true;
			break;
		
		case ClearChordScale:
			TRACE("ClearChordScale from %d to %d\n", from+1, to+1);
			if(to != All)
			{
				m_Chord[to]->ChordPattern() = 0;
				m_Chord[to]->ChordRoot() = 0;
				m_Chord[to]->ScalePattern() = 0;
				m_Chord[to]->ScaleRoot() = 0;
			}
			else
			{
				for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
				{
					m_Chord[i]->ChordPattern() = 0;
					m_Chord[i]->ChordRoot() = 0;
					m_Chord[i]->ScalePattern() = 0;
					m_Chord[i]->ScaleRoot() = 0;
				}
			}
			bModified = true;
			break;
		
		default:
//			TRACE("No operation\n");
			break;
		}
	}
	else
	{
		// additional commands go here
		switch( wParam )
		{
			case IDM_LOCK_ALL_SCALES:
				m_fLockAllScales = m_fLockAllScales ? false : true;
				bModified = true;
				break;
			
			case IDM_SYNC_ALL_LEVELS:
				m_fSyncLevelOneToAll = m_fSyncLevelOneToAll ? false : true;
				bModified = true;
				break;
		}
	}

	if(bModified)
	{
		SetChord(&m_Chord);
		UpdateChordData();
	}

	return CPropertyPage::OnCommand(wParam, lParam);
}

void CChordScalePropPage::DispatchRightMouseKeyboard(CKeyboard * pKeyboard, short nKey, CPoint& ptMouse)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(nKey);
	int i;
	if( (i = GetChordKeyboardIndex(pKeyboard))  >= 0 )
	{
		TRACE("Right click on chord keyboard %d\n", i);
	}
	else if( (i = GetScaleKeyboardIndex(pKeyboard)) >= 0 )
	{
		TRACE("Right click on scale keyboard %d\n", i);
	}

	m_nLastKeyboardLevel = i;	// save keyboard that sent right click

	CMenu cMenu;
	if( cMenu.LoadMenu( MAKEINTRESOURCE(IDR_CHORDPROP) ) != 0 )
	{
		CMenu* pSubMenu = cMenu.GetSubMenu(0);
		if( pSubMenu )
		{
			pSubMenu->CheckMenuItem( IDM_LOCK_ALL_SCALES, (m_fLockAllScales ? MF_CHECKED : MF_UNCHECKED) );
			pSubMenu->CheckMenuItem( IDM_SYNC_ALL_LEVELS, (m_fSyncLevelOneToAll ? MF_CHECKED : MF_UNCHECKED) );

			for( int j = 3; j < 6; j++ )
			{
				CMenu* pSubSubMenu = pSubMenu->GetSubMenu( j );
				if( pSubSubMenu )
				{
					for( int k = 0; k < 4; k++ )
					{
						if( m_nLastKeyboardLevel == -1
						||  m_nLastKeyboardLevel == (3 - k) )
						{
							pSubSubMenu->EnableMenuItem( k, MF_BYPOSITION | MF_GRAYED );
						}
						else
						{
							pSubSubMenu->EnableMenuItem( k, MF_BYPOSITION | MF_ENABLED );
						}
					}
				}
			}

			pKeyboard->ClientToScreen( &ptMouse );

			MSG	msg;
			if( pSubMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, (int)(ptMouse.x), (int)(ptMouse.y),
										  this ) != 0 )
			{
				::PeekMessage( &msg, pKeyboard->GetSafeHwnd(), 0, 0, PM_REMOVE ); // Remove the last mouse click msg from the queue.
			}
		}
	}
}



void CChordScalePropPage::ComputeCopyOp(WORD menuid, int & from, int & to, int & what)
{
	from = m_nLastKeyboardLevel;
	if(from == -1)
		return;		// not from a menu
	switch(menuid)
	{
	case ID_COPY_CHORD1:
		what = CopyChord;
		to = 0;
		break;
	case ID_COPY_CHORD2:
		what = CopyChord;
		to = 1;
		break;
	case ID_COPY_CHORD3:
		what = CopyChord;
		to = 2;
		break;
	case ID_COPY_CHORD4:
		what = CopyChord;
		to = 3;
		break;
	case ID_COPY_CHORDALL:
		what = CopyChord;
		to = All;
		break;

	case ID_COPY_SCALE1:
		what = CopyScale;
		to = 0;
		break;
	case ID_COPY_SCALE2:
		what = CopyScale;
		to = 1;
		break;
	case ID_COPY_SCALE3:
		what = CopyScale;
		to = 2;
		break;
	case ID_COPY_SCALE4:
		what = CopyScale;
		to = 3;
		break;
	case ID_COPY_SCALEALL:
		what = CopyScale;
		to = All;
		break;

	case ID_COPY_CHORDSCALE1:
		what = CopyChordScale;
		to = 0;
		break;
	case ID_COPY_CHORDSCALE2:
		what = CopyChordScale;
		to = 1;
		break;
	case ID_COPY_CHORDSCALE3:
		what = CopyChordScale;
		to = 2;
		break;
	case ID_COPY_CHORDSCALE4:
		what = CopyChordScale;
		to = 3;
		break;
	case ID_COPY_CHORDSCALEALL:
		what = CopyChordScale;
		to = All;
		break;

	case ID_CLEAR_CHORDSCALE1:
		what = ClearChordScale;
		to = 0;
		break;
	case ID_CLEAR_CHORDSCALE2:
		what = ClearChordScale;
		to = 1;
		break;
	case ID_CLEAR_CHORDSCALE3:
		what = ClearChordScale;
		to = 2;
		break;
	case ID_CLEAR_CHORDSCALE4:
		what = ClearChordScale;
		to = 3;
		break;
	case ID_CLEAR_CHORDSCALEALL:
		what = ClearChordScale;
		to = All;
		break;

	default:
		what = Nop;
		break;
	}
	m_nLastKeyboardLevel = -1;
}

void CChordScalePropPage::WarnUser()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if(!m_bNotWarn)
	{
		CIllegalChord dlg;
		dlg.m_bDontWarn = m_bNotWarn;
		dlg.DoModal();
		m_bNotWarn = dlg.m_bDontWarn;
	}

}
