// InstrumentFVEditor.cpp : implementation file
//

#include "stdafx.h"
#include "region.h"
#include "DMUSProd.h"
#include "DLSEdit.h"
#include "DLSDesignerDLL.h"
#include "DLSStatic.h"
#include "InstrumentFVEditor.h"
#include "ArticulationTabCtrl.h"
#include "Articulation.h"
#include "InstrumentCtl.h"
#include "Instrument.h"
#include "VibratoLFODialog.h"
#include "LFODialog.h"
#include "PitchDialog.h"
#include "VolDialog.h"
#include "FilterDialog.h"
#include "KeyBoardMap.h"
#include "UserPatchConflictDlg.h"
#include "WaveNode.h"
#include "Wave.h"
#include "MonoWave.h"
#include "StereoWave.h"
#include "ConditionEditor.h"
#include "JazzDataObject.h"
#include "DlsDefsPlus.h"
#include "DLSLoadSaveUtils.h"

#include "InstrumentPropPgMgr.h"
#include "RegionPropPgMgr.h"
#include "RegionPropPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int		CInstrumentFVEditor::m_nInstrumentEditors = 0;
CMenu*	CInstrumentFVEditor::m_pContextInstMenu = NULL;
CMenu*	CInstrumentFVEditor::m_pContextRegionMenu = NULL;
CMenu*	CInstrumentFVEditor::m_pContextArtMenu = NULL;

HICON	CInstrumentFVEditor::m_hInstrumentIcon = NULL;
HICON	CInstrumentFVEditor::m_hRegionIcon = NULL;

HANDLE	CInstrumentFVEditor::m_hAttack = NULL;
HANDLE	CInstrumentFVEditor::m_hDecay = NULL;
HANDLE	CInstrumentFVEditor::m_hDecay2 = NULL;
HANDLE	CInstrumentFVEditor::m_hSustain = NULL;
HANDLE	CInstrumentFVEditor::m_hRelease = NULL;
HANDLE	CInstrumentFVEditor::m_hRelease2 = NULL;

CBitmap	CInstrumentFVEditor::m_bmpAttack;
CBitmap	CInstrumentFVEditor::m_bmpDecay;
CBitmap	CInstrumentFVEditor::m_bmpDecay2;
CBitmap	CInstrumentFVEditor::m_bmpSustain;
CBitmap	CInstrumentFVEditor::m_bmpRelease;
CBitmap	CInstrumentFVEditor::m_bmpRelease2;


BOOL isValidNoteString(char * psz)
{
    int nlen = strlen(psz);

    if (nlen > 4)
        return false;

    _strupr(psz);


    for (int i = 0; i < nlen; i++)
    {
        switch (i)
        {
        case 0:
            {
                if (psz[0] >= 'A' && psz[0] <= 'G')
                    break;
                else
                    return false;
            }

        case 1:
            if (psz[i] == 'B' || psz[i] == '#' || (psz[i] >= '0'&& psz[i] <= '9'))
                break;
            else
                return false;

        case 2:
        case 3:
            if (psz[i] >= '0' && psz[i] <= '9')
                break;
            else
                return false;

        default:
            return false;
        } // switch
    }
    return true;
}

// =======================================================================

static char * g_szGroup[16] = { "No Group", "Group 1", "Group 2", "Group 3",
                            "Group 4", "Group 5", "Group 6", "Group 7",
                            "Group 8", "Group 9", "Group 10","Group 11",
                            "Group 12", "Group 13", "Group 14", "Group 15"};

/////////////////////////////////////////////////////////////////////////////
// CInstrumentFVEditor

IMPLEMENT_DYNCREATE(CInstrumentFVEditor, CFormView)

CInstrumentFVEditor::CInstrumentFVEditor(CInstrumentCtrl* parent)
	: CFormView(IDD), 
	  m_currSelTab(0),
	  m_nCurrentArticulationDlg(IDD_VOLUME_PAGE),
	  m_pArtDialog(NULL),
	  m_fInOnInitialUpdate(true),
	  m_pWave(NULL),
	  m_pWaves(NULL),
	  m_pCurRegion(NULL),
	  m_pCurArticulation(NULL),
	  m_dwCookie(0),
	  m_cfFormatArt(0),
	  m_cfFormatRegion(0),
	  m_nStartNoteForCurRegion(0),
	  m_nHasFocus(0),
	  m_bfTouched(0),
	  m_bTouchedByProgram(false),
	  m_bTouchedBySpinner(false),
	  m_dwIgnorePatchConflicts(0),
	  m_dwLowerNoteRange(0),
	  m_dwUpperNoteRange(0),
	  m_dwLowerVelocityRange(0),
	  m_dwUpperVelocityRange(127),
	  m_lUnityNote(0),
	  m_nClickVelocity(100),
	  m_usActiveLayer(1)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Incerement the editor refcount
	m_nInstrumentEditors++;

	m_cfFormatArt = RegisterClipboardFormat(CF_DLS_ARTICULATION);	
	m_cfFormatRegion = RegisterClipboardFormat(CF_DLS_REGION);	

	m_parent = parent;

	m_pInstrument = m_parent->GetInstrument();
	ASSERT(m_pInstrument);
	m_pInstrument->AddRef();
	
	m_pCollection = m_pInstrument->GetParentCollection();
	ASSERT(m_pCollection);
	m_pCollection->AddRef();

	m_pWaves = &(m_pCollection->m_Waves);
	ASSERT(m_pWaves);
	m_pWaves->AddRef();

	m_pComponent = m_pInstrument->m_pComponent;

	if(m_hInstrumentIcon == NULL)
		m_hInstrumentIcon = ::LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDI_INSTRUMENT));

	if(m_hRegionIcon == NULL)
		m_hRegionIcon = ::LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDI_REGION));

	// Load the bitmaps for the envelope params
	if(m_hAttack == NULL)
	{
		m_hAttack = LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDB_ATTACK), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		ASSERT(m_hAttack);
		m_bmpAttack.Attach(m_hAttack);
	}

	if(m_hDecay == NULL)
	{
		m_hDecay = LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDB_DECAY), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		ASSERT(m_hDecay);
		m_bmpDecay.Attach(m_hDecay);
	}

	if(m_hDecay2 == NULL)
	{
		m_hDecay2 = LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDB_DECAY2), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		ASSERT(m_hDecay2);
		m_bmpDecay2.Attach(m_hDecay2);
	}

	if(m_hSustain == NULL)
	{
		m_hSustain = LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDB_SUSTAIN), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		ASSERT(m_hSustain);
		m_bmpSustain.Attach(m_hSustain);
	}
	
	if(m_hRelease == NULL)
	{
		m_hRelease = LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDB_RELEASE), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		ASSERT(m_hRelease);
		m_bmpRelease.Attach(m_hRelease);
	}
	
	if(m_hRelease2 == NULL)
	{
		m_hRelease2 = LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDB_RELEASE2), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		ASSERT(m_hRelease2);
		m_bmpRelease2.Attach(m_hRelease2);
	}


	//memset(m_Tabs, NULL, sizeof(m_Tabs));

	//{{AFX_DATA_INIT(CInstrumentFVEditor)
	m_wBank = 0;
	m_wBank2 = 0;
	m_wPatch = 0;
	m_fIsDrumKit = FALSE;
	m_fAllowOverlap = FALSE;
	m_fUseInstArt = FALSE;
	//}}AFX_DATA_INIT
}

CInstrumentFVEditor::~CInstrumentFVEditor()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Decrement the editor refcount
	m_nInstrumentEditors--;


	if(m_pInstrument)
	{
		m_pInstrument->Stop(true);
		m_pInstrument->TurnOffMidiNotes();
	}
	if(m_parent)
		m_parent->TurnOffMidiNotes();


	// Destroy the menu only if the last editor is destructing
	if(m_nInstrumentEditors <= 0)
	{
		if(m_pContextInstMenu)
		{
			delete m_pContextInstMenu;
			m_pContextInstMenu = NULL;
		}
		if(m_pContextRegionMenu)
		{
			delete m_pContextRegionMenu;
			m_pContextRegionMenu = NULL;
		}
		if(m_pContextArtMenu)
		{
			delete m_pContextArtMenu; 
			m_pContextArtMenu = NULL;
		}

		if(m_hInstrumentIcon)
		{
			::DestroyIcon(m_hInstrumentIcon);
			m_hInstrumentIcon = NULL;
		}

		if(m_hRegionIcon)
		{
			::DestroyIcon(m_hRegionIcon);
			m_hRegionIcon = NULL;
		}

		if(m_hAttack)
		{
			m_bmpAttack.Detach();
			DeleteObject(m_hAttack);
			m_hAttack = NULL;
		}
		
		if(m_hDecay)
		{
			m_bmpDecay.Detach();
			DeleteObject(m_hDecay);
			m_hDecay = NULL;
		}

		if(m_hDecay2)
		{
			m_bmpDecay2.Detach();
			DeleteObject(m_hDecay2);
			m_hDecay2 = NULL;
		}

		if(m_hSustain)
		{
			m_bmpSustain.Detach();
			DeleteObject(m_hSustain);
			m_hSustain = NULL;
		}

		if(m_hRelease)
		{
			m_bmpRelease.Detach();
			DeleteObject(m_hRelease);
			m_hRelease = NULL;
		}

		if(m_hRelease2)
		{
			m_bmpRelease2.Detach();
			DeleteObject(m_hRelease2);
			m_hRelease2 = NULL;
		}
	}

	CleanupArticluation();

	if(m_pWaves)
	{
		m_pWaves->Release();
	}

	if(m_pInstrument)
	{
		m_pInstrument->Release();
	}

	if(m_pCollection)
	{
		m_pCollection->Release();
	}

	if (m_pCurRegion) 
	{
		m_pCurRegion->m_pInstrumentFVEditor = NULL;
	}

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

void CInstrumentFVEditor::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstrumentFVEditor)
	DDX_Control(pDX, IDC_RADIO_SOLO_LAYER, m_SoloLayerButton);
	DDX_Control(pDX, IDC_FILTER_BUTTON, m_FilterButton);
	DDX_Control(pDX, IDC_VIBLFO_BUTTON, m_VibLFOButton);
	DDX_Control(pDX, IDC_ART_DLS1_CHECK, m_ArtDLS1Check);
	DDX_Control(pDX, IDC_REGION_CONDITION_COMBO, m_RegionConditionCombo);
	DDX_Control(pDX, IDC_LAYER_SCROLLBAR, m_LayerScrollBar);
	DDX_Control(pDX, IDC_CLICK_VELOCITY_SPIN, m_ClickVelocitySpin);
	DDX_Control(pDX, IDC_CLICK_VELOCITY, m_ClickVelocityEdit);
	DDX_Control(pDX, IDC_REGION_VELOCITY_THRU_SPIN, m_VelocityHighRangeSpin);
	DDX_Control(pDX, IDC_REGION_VELOCITY_RANGE_SPIN, m_VelocityLowRangeSpin);
	DDX_Control(pDX, IDC_REGION_VELOCITY_EURANGE, m_VelocityHighRangeEdit);
	DDX_Control(pDX, IDC_REGION_VELOCITY_ELRANGE, m_VelocityLowRangeEdit);
	DDX_Control(pDX, IDC_VOLUME_BUTTON, m_VolumeButton);
	DDX_Control(pDX, IDC_LFO_BUTTON, m_LFOButton);
	DDX_Control(pDX, IDC_PITCH_BUTTON, m_PitchButton);
	DDX_Control(pDX, IDC_REGION_ROOT_NOTE, m_RootNoteEdit);
	DDX_Control(pDX, IDC_REGION_EURANGE, m_HighRangeEdit);
	DDX_Control(pDX, IDC_REGION_ELRANGE, m_LowRangeEdit);
	DDX_Control(pDX, IDC_REGION_THRU_SPIN, m_ThruSpin);
	DDX_Control(pDX, IDC_REGION_RANGE_SPIN, m_RangeSpin);
	DDX_Control(pDX, IDC_REGION_ROOT_NOTE_SPIN, m_RootNoteSpin);
	DDX_Control(pDX, IDC_INSTRUMENT_PATCH_SPIN, m_PatchSpin);
	DDX_Control(pDX, IDC_INSTRUMENT_BMSB_SPIN, m_MSBSpin);
	DDX_Control(pDX, IDC_INSTRUMENT_BLSB_SPIN, m_LSBSpin);
	DDX_Text(pDX, IDC_BANK, m_wBank);
	DDV_MinMaxUInt(pDX, m_wBank, 0, 127);
	DDX_Text(pDX, IDC_BANK2, m_wBank2);
	DDV_MinMaxUInt(pDX, m_wBank2, 0, 127);
	DDX_Text(pDX, IDC_PATCH, m_wPatch);
	DDV_MinMaxUInt(pDX, m_wPatch, 0, 127);
	DDX_Check(pDX, IDC_DRUMS, m_fIsDrumKit);
	DDX_Control(pDX, IDC_REGION_REGIONKEYBOARD, m_RegionKeyBoard);
	DDX_Check(pDX, IDC_REGION_OVERLAP, m_fAllowOverlap);
	DDX_Check(pDX, IDC_REGION_UIA, m_fUseInstArt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInstrumentFVEditor, CFormView)
	//{{AFX_MSG_MAP(CInstrumentFVEditor)
	ON_BN_CLICKED(IDC_DRUMS, OnDrums)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_REGION_OVERLAP, OnRegionOverlap)
	ON_BN_CLICKED(IDC_REGION_UIA, OnRegionUia)
	ON_CBN_SELCHANGE(IDC_REGION_WAVELINK, OnSelchangeRegionWavelink)
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_COMMAND(ID_INST_ED_ARTICULATION_COPY, OnArticulationCopy)
	ON_COMMAND(ID_INST_ED_ARTICULATION_PASTE, OnArticulationPaste)
	ON_COMMAND(ID_INST_ED_ARTICULATION_DELETE, OnArticulationDelete)
	ON_COMMAND(ID_INST_ED_REGION_DELETE, OnRegionDelete)
	ON_COMMAND(IDM_INST_ED_INSTRUMENT_PROPERTIES, OnInstrumentProperties)
	ON_COMMAND(IDM_INST_ED_REGION_PROPERTIES, OnRegionProperties)
	ON_COMMAND(IDM_INST_ED_REGION_NEW_REGION, OnInstEdRegionNewRegion)
	ON_EN_KILLFOCUS(IDC_PATCH, OnKillfocusPatch)
	ON_EN_KILLFOCUS(IDC_BANK, OnKillfocusBank)
	ON_EN_KILLFOCUS(IDC_BANK2, OnKillfocusBank2)
	ON_CBN_SELCHANGE(IDC_REGION_GROUP, OnSelchangeRegionGroup)
	ON_EN_UPDATE(IDC_PATCH, OnUpdatePatch)
	ON_CBN_DROPDOWN(IDC_REGION_WAVELINK, OnDropdownRegionWavelink)
	ON_MESSAGE(DM_VALIDATE,OnValidate)
	ON_MESSAGE(DM_BRINGTOTOP,OnBringToTop)
	ON_MESSAGE(DM_UPDATE_VALUES,OnUpdateMIDIValues)
	ON_MESSAGE(DM_REGION_SELECT,OnMIDIRegionSelect)
	ON_EN_CHANGE(IDC_BANK, OnChangeBank)
	ON_EN_CHANGE(IDC_BANK2, OnChangeBank2) 
	ON_EN_CHANGE(IDC_PATCH, OnChangePatch)
	ON_EN_UPDATE(IDC_BANK, OnUpdateBank)
	ON_EN_UPDATE(IDC_BANK2, OnUpdateBank2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_INSTRUMENT_BMSB_SPIN, OnDeltaPosMSBSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_INSTRUMENT_BLSB_SPIN, OnDeltaPosLSBSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_INSTRUMENT_PATCH_SPIN, OnDeltaPosPatchSpin)
	ON_EN_KILLFOCUS(IDC_REGION_ELRANGE, OnKillfocusRegionElrange)
	ON_EN_KILLFOCUS(IDC_REGION_EURANGE, OnKillfocusRegionEurange)
	ON_EN_KILLFOCUS(IDC_REGION_ROOT_NOTE, OnKillfocusRegionRootNote)
	ON_NOTIFY(UDN_DELTAPOS, IDC_REGION_ROOT_NOTE_SPIN, OnDeltaposRegionRootNoteSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_REGION_THRU_SPIN, OnDeltaposRegionThruSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_REGION_RANGE_SPIN, OnDeltaposRegionRangeSpin)
	ON_EN_SETFOCUS(IDC_REGION_ELRANGE, OnSetfocusRegionElrange)
	ON_EN_SETFOCUS(IDC_REGION_EURANGE, OnSetfocusRegionEurange)
	ON_EN_SETFOCUS(IDC_REGION_ROOT_NOTE, OnSetfocusRegionRootNote)
	ON_BN_CLICKED(IDC_PITCH_BUTTON, OnPitchButton)
	ON_BN_CLICKED(IDC_LFO_BUTTON, OnLfoButton)
	ON_BN_CLICKED(IDC_VOLUME_BUTTON, OnVolumeButton)
	ON_EN_KILLFOCUS(IDC_REGION_VELOCITY_ELRANGE, OnKillfocusRegionVelocityElrange)
	ON_EN_KILLFOCUS(IDC_REGION_VELOCITY_EURANGE, OnKillfocusRegionVelocityEurange)
	ON_EN_KILLFOCUS(IDC_CLICK_VELOCITY, OnKillfocusClickVelocity)
	ON_NOTIFY(UDN_DELTAPOS, IDC_REGION_VELOCITY_RANGE_SPIN, OnDeltaposRegionVelocityRangeSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_REGION_VELOCITY_THRU_SPIN, OnDeltaposRegionVelocityThruSpin)
	ON_WM_VSCROLL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_CLICK_VELOCITY_SPIN, OnDeltaposClickVelocitySpin)
	ON_BN_CLICKED(IDC_CONDITION_EDIT_BUTTON, OnConditionEditButton)
	ON_CBN_SELCHANGE(IDC_REGION_CONDITION_COMBO, OnSelchangeRegionConditionCombo)
	ON_CBN_DROPDOWN(IDC_REGION_CONDITION_COMBO, OnDropdownRegionConditionCombo)
	ON_BN_CLICKED(IDC_ART_DLS1_CHECK, OnArtDls1Check)
	ON_BN_CLICKED(IDC_VIBLFO_BUTTON, OnViblfoButton)
	ON_BN_CLICKED(IDC_FILTER_BUTTON, OnFilterButton)
	ON_COMMAND(IDM_INST_ED_REGION_NEW_LAYER, OnInstEdRegionNewLayer)
	ON_COMMAND(ID_INST_ED_LAYER_DELETE, OnInstEdLayerDelete)
	ON_BN_CLICKED(IDC_RADIO_MULTIPLE_LAYERS, OnRadioMultipleLayers)
	ON_BN_CLICKED(IDC_RADIO_SOLO_LAYER, OnRadioSoloLayer)
	ON_BN_CLICKED(IDC_WAVE_EDIT_BUTTON, OnWaveEditButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstrumentFVEditor diagnostics

#ifdef _DEBUG
void CInstrumentFVEditor::AssertValid() const
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CFormView::AssertValid();
}

void CInstrumentFVEditor::Dump(CDumpContext& dc) const
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CFormView::Dump(dc);
}
#endif //_DEBUG

HRESULT CInstrumentFVEditor::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if(::IsEqualIID(riid, IID_IDMUSProdMidiInCPt)
	|| ::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = (IDMUSProdMidiInCPt *)this;
        return S_OK;
    }

    if(::IsEqualIID(riid, IID_IPersist))
    {
        AddRef();
        *ppvObj = (IPersist *)this;
        return S_OK;
    }

    if(::IsEqualIID(riid, IID_IPersistStream))
    {
        AddRef();
        *ppvObj = (IPersistStream *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CInstrumentFVEditor::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CInstrumentFVEditor::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT(m_dwRef != 0);

	AfxOleUnlockApp();
    --m_dwRef;

    if(m_dwRef == 0)
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentFVEditor message handlers

int CInstrumentFVEditor::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pContextInstMenu == NULL)
	{
		m_pContextInstMenu = new CMenu();
		if(m_pContextInstMenu && m_pContextInstMenu->LoadMenu(IDM_INST_EDITOR_INSTRUMENT_RMENU) == 0)
		{
			delete m_pContextInstMenu;
			m_pContextInstMenu = NULL;
		}
	}

	if(m_pContextRegionMenu == NULL)
	{
		m_pContextRegionMenu = new CMenu();
		if(m_pContextRegionMenu && m_pContextRegionMenu->LoadMenu(IDM_INST_EDITOR_REGION_RMENU) == 0)
		{
			delete m_pContextRegionMenu;
			m_pContextRegionMenu = NULL;
		}
	}

	if(m_pContextArtMenu == NULL)
	{
		m_pContextArtMenu = new CMenu();
		if(m_pContextArtMenu && m_pContextArtMenu->LoadMenu(IDM_INST_EDITOR_ARTICULATION_RMENU) == 0)
		{
			delete m_pContextArtMenu;
			m_pContextArtMenu = NULL;
		}
	}

	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	RegisterMidi();
	if(m_pInstrument && m_pInstrument->m_pInstrumentCtrl)
		m_pInstrument->m_pInstrumentCtrl->SetTransportName();

	m_pComponent->m_pIConductor->RegisterTransport(m_pInstrument, 0);
	m_pComponent->m_pIConductor->SetActiveTransport(m_pInstrument, BS_PLAY_ENABLED | BS_NO_AUTO_UPDATE);
	m_pInstrument->UpdatePatch();

	return 0;
}

void CInstrumentFVEditor::CollectStatics()
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
			{
				m_lstStatics.AddTail(pDLSStatic);
				
				// Special case for "DLS1" layer static
				if(pDLSStatic->GetID() == IDC_DLS1_STATIC)
				{
					int nFirstVisibleLayer = m_RegionKeyBoard.GetFirstVisibleLayer();
					if(nFirstVisibleLayer != 0)
					{
						pDLSStatic->SetTextColor(::GetSysColor(COLOR_BTNFACE));
					}
				}
			}
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


void CInstrumentFVEditor::OnDestroy()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CFormView::OnDestroy();

	// Delete the Undo Manager
	UnRegisterMidi();
	m_pInstrument->Stop(true);
	m_pInstrument->TurnOffMidiNotes();
	m_pComponent->m_pIConductor->UnRegisterTransport(m_pInstrument);
}

void CInstrumentFVEditor::OnInitialUpdate() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_fInOnInitialUpdate = true;
	CFormView::OnInitialUpdate();

	// Fill up the region group combo
	CComboBox* pCombo = (CComboBox *)GetDlgItem(IDC_REGION_GROUP);
    if(pCombo)
    {
		pCombo->ResetContent();
        for (int i = 0; i < 16; i++)
        {
	        pCombo->AddString(g_szGroup[i]);
        }
    }
	// Set range for the velocity spin controls
	m_VelocityHighRangeSpin.SetRange(0, 127);
	m_VelocityLowRangeSpin.SetRange(0, 127);
	m_ClickVelocitySpin.SetRange(0, 127);
	CString sText;
	sText.Format("%d", m_nClickVelocity);
	m_ClickVelocityEdit.SetWindowText(sText);

	
	SetAttachedNode();
	SetupInstrument();
	
	SetupRegion();
	//SetLayerScrollInfo(0);
	
	SetupArticulation();


	// Setup the Articulation tab set
	/*TC_ITEM TabCtrlItem[3];
	CString		csLabelText;

	csLabelText.LoadString(IDS_VOL_TAB_LABEL);
	TabCtrlItem[0].mask = TCIF_TEXT;
	TabCtrlItem[0].pszText = csLabelText.GetBuffer(0);
    TabCtrlItem[0].cchTextMax = csLabelText.GetLength() + 1;    
	m_ArticulationTabCtrl.InsertItem(0, &TabCtrlItem[0]);

	csLabelText.LoadString(IDS_PITCH_TAB_LABEL);
	TabCtrlItem[1].mask = TCIF_TEXT;
	TabCtrlItem[1].pszText = csLabelText.GetBuffer(0);
    TabCtrlItem[1].cchTextMax = csLabelText.GetLength() + 1;    
	m_ArticulationTabCtrl.InsertItem(1, &TabCtrlItem[1]);	

	csLabelText.LoadString(IDS_LFO_TAB_LABEL);
	TabCtrlItem[2].mask = TCIF_TEXT;
	TabCtrlItem[2].pszText = csLabelText.GetBuffer(0);
    TabCtrlItem[2].cchTextMax = csLabelText.GetLength() + 1;    
	m_ArticulationTabCtrl.InsertItem(2, &TabCtrlItem[2]);*/	


	// Set the limit for all the edits
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_BANK);
	if(pEdit)
		pEdit->SetLimitText(3);
	pEdit = (CEdit*) GetDlgItem(IDC_BANK2);
	if(pEdit)
		pEdit->SetLimitText(3);
	pEdit = (CEdit*) GetDlgItem(IDC_PATCH);
	if(pEdit)
		pEdit->SetLimitText(3);

	SetAuditionRadioMode();

#ifdef DMP_XBOX
	CWnd *pWnd = GetDlgItem(IDC_CONDITION_EDIT_BUTTON);
	if( pWnd )
	{
		pWnd->EnableWindow(FALSE);
	}
	pWnd = GetDlgItem(IDC_REGION_CONDITION_COMBO);
	if( pWnd )
	{
		pWnd->EnableWindow(FALSE);
	}
	pWnd = GetDlgItem(IDC_STATIC_CONDITION);
	if( pWnd )
	{
		pWnd->EnableWindow(FALSE);
	}
#endif // DMP_XBOX

	m_fInOnInitialUpdate = false;
}

/*void CInstrumentFVEditor::OnSelchangeArticulationTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pResult = 0;
	int newSelTab = m_ArticulationTabCtrl.GetCurSel();

	ASSERT(newSelTab >= 0 && newSelTab <= 2);
	
	if(newSelTab == m_currSelTab)
	{
		return;
	}
	else
	{
		switch(newSelTab)
		{
			case 0:
				m_Tabs[0]->BringWindowToTop();
				m_Tabs[0]->ShowWindow(SW_SHOW);
				m_Tabs[m_currSelTab]->ShowWindow(SW_HIDE);
				m_currSelTab = 0;
				break;
			
			case 1:
				m_Tabs[1]->BringWindowToTop();
				m_Tabs[1]->ShowWindow(SW_SHOW);
				m_Tabs[m_currSelTab]->ShowWindow(SW_HIDE);
				m_currSelTab = 1;
				break;

			case 2:
				m_Tabs[2]->BringWindowToTop();
				m_Tabs[2]->ShowWindow(SW_SHOW);
				m_Tabs[m_currSelTab]->ShowWindow(SW_HIDE);
				m_currSelTab = 2;
				break;
			
			default:
				// Should never get here
				ASSERT(FALSE);
		}
	}
}*/

void CInstrumentFVEditor::SetAttachedNode()
{
	GUID guid;

	if ( m_parent->m_pAttachedNode )
	{
		m_parent->m_pAttachedNode->GetNodeId(&guid);
		if(guid == GUID_InstrumentNode)
		{
			CRegion* pRegion = m_pInstrument->m_Regions.GetHead();
			if(pRegion)
			{
				m_pCurRegion = pRegion;
			}
			if (m_pCurRegion)
			{
				if (m_pCurRegion->m_pArticulation)
				{
					m_pCurArticulation = m_pCurRegion->m_pArticulation;
				}
				else
				{
					m_pCurArticulation = m_pInstrument->GetCurrentArticulation();
				}
			}
			else if (m_pInstrument->GetCurrentArticulation())
			{
				m_pCurArticulation = m_pInstrument->GetCurrentArticulation();
			}
		}

		else if(guid == GUID_RegionNode)
		{
			m_pCurRegion = (CRegion*) m_parent->m_pAttachedNode;
			if (m_pCurRegion->m_pArticulation)
			{
				m_pCurArticulation = m_pCurRegion->m_pArticulation;
			}
			else
			{
				m_pCurArticulation = m_pInstrument->GetCurrentArticulation();
			}
		}
		else if(guid == GUID_ArticulationNode)
		{
			m_pCurArticulation = (CArticulation *)m_parent->m_pAttachedNode;
			
			// Determines that the Articulation has a valid parent
			// either an instrument or a region
		#ifdef _DEBUG
			m_pCurArticulation->ValidateParent();
		#endif 

			BOOL bArtOwnerIsRegion = FALSE;
			void* pArtOwner = m_pCurArticulation->GetOwner(bArtOwnerIsRegion);
			if(bArtOwnerIsRegion)
			{
				m_pCurRegion = static_cast<CRegion*>(pArtOwner);
				ASSERT(m_pCurRegion);
			}
			else
			{
				CRegion* pRegion = m_pInstrument->m_Regions.GetHead();
				if(pRegion)
				{
					m_pCurRegion = pRegion;
				}
			}
		}
		m_parent->m_pAttachedNode = NULL;
	}
}

void CInstrumentFVEditor::SetupInstrument()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return;
	}

	// Set spin control ranges
	m_PatchSpin.SetRange(0, 127);
	m_MSBSpin.SetRange(0, 127);
	m_LSBSpin.SetRange(0, 127);

	char text[INST_SETTINGS_MAX_TEXT];
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
	
	m_fIsDrumKit = ((m_pInstrument->m_rInstHeader.Locale.ulBank & F_INSTRUMENT_DRUMS) != 0);
	
	pDisplay = GetDlgItem(IDC_DRUMS);
	if(pDisplay)
	{
		((CButton*)pDisplay)->SetCheck(m_fIsDrumKit);
	}

	m_wPatch = m_pInstrument->m_rInstHeader.Locale.ulInstrument;
	sprintf(text, "%d", m_wPatch);	
	
	pDisplay = GetDlgItem(IDC_PATCH);		
	if(pDisplay)
	{
		pDisplay->SetWindowText(text);	
	}

	pDisplay = NULL;
	
	long volToDisplay = -(m_pInstrument->GetMaxVolume());	 
	volToDisplay >>= 16;
	
	if(volToDisplay / 10 == 0 && volToDisplay % 10 == 0)
		sprintf(text, "%02d.%01d", volToDisplay / 10, volToDisplay % 10);
	else
		sprintf(text, "-%02d.%01d", volToDisplay / 10, volToDisplay % 10);
	
	pDisplay = GetDlgItem(IDC_DMAXLEVEL);
	
	if(pDisplay)
	{
		pDisplay->SetWindowText(text);	
	}

	short nLayers = short(m_pInstrument->m_Regions.GetNumberOfLayers());
	m_RegionKeyBoard.SetNumberOfLayers(nLayers);
}

void CInstrumentFVEditor::SetupRegion()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Set spin control ranges
	m_RootNoteSpin.SetRange(0, 127);
	m_RangeSpin.SetRange(0, 127);
	m_ThruSpin.SetRange(0, 127);

	SetRKBMap();

	if (m_pCurRegion == NULL)
	{
		CRegion* pRegion = m_pInstrument->m_Regions.GetHead();
		if(pRegion)
		{
			SetCurRegion(pRegion);
		}
	}
	else
	{
		SetCurRegion(m_pCurRegion);
	}
	SendRegionChange();
}

void CInstrumentFVEditor::SetupArticulation()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	//ASSERT(m_Tabs[0] == NULL);

	UpdateUseInstArt();
	if(m_fUseInstArt)
	{
		SetArticulationStaticIcon(true);
	}
	else
	{
		SetArticulationStaticIcon(false);
	}

	if(m_nCurrentArticulationDlg == IDD_VOLUME_PAGE)
	{
		OnVolumeButton();
	}
	else if(m_nCurrentArticulationDlg == IDD_PITCH_PAGE)
	{
		OnPitchButton();
	}
	else if(m_nCurrentArticulationDlg == IDD_LFO_PAGE)
	{
		OnLfoButton();
	}
	else if(m_nCurrentArticulationDlg == IDD_VIBLFO_PAGE)
	{
		OnViblfoButton();
	}
	else if(m_nCurrentArticulationDlg == IDD_FILTER_PAGE)
	{
		OnFilterButton();
	}

	if(m_pCurArticulation)
	{
		BOOL bIsDLS1 = m_pCurArticulation->IsDLS1();
		m_ArtDLS1Check.SetCheck(bIsDLS1);
		m_VibLFOButton.EnableWindow(!bIsDLS1);
		m_FilterButton.EnableWindow(!bIsDLS1);
	}
}

void CInstrumentFVEditor::SetRKBMap()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    
	// Walk the region list initializing RegionMap

	for( CRegion * pRegion = m_pInstrument->m_Regions.GetHead(); pRegion; pRegion = pRegion->GetNext())
    {
		short nFirstNote = pRegion->m_rRgnHeader.RangeKey.usLow;
		short nLastNote = pRegion->m_rRgnHeader.RangeKey.usHigh;

		// The layer might be bumped up if there's a collision
		short nRegionLayer = short(pRegion->GetLayer());
		int nInsertionLayer = m_RegionKeyBoard.InsertRegion(nRegionLayer, nFirstNote, nLastNote, 0, 127, pRegion->GetWaveName());
		if(nInsertionLayer != nRegionLayer)
		{
			pRegion->SetLayer(nInsertionLayer);
		}

		m_RegionKeyBoard.SetRootNote(pRegion->m_rWSMP.usUnityNote);
	}
}


void CInstrumentFVEditor::CleanupArticluation()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pArtDialog)
	{
		m_pArtDialog->DestroyWindow();
		delete m_pArtDialog;
		m_pArtDialog = NULL;
	}
}

void CInstrumentFVEditor::SetCurArticulation(CArticulation* pArticulation)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pArticulation);
	if(pArticulation == NULL)
		return;
	
	m_pCurArticulation = pArticulation;
	SetArticulationStaticIcon(!(m_pCurRegion && m_pCurArticulation == m_pCurRegion->m_pArticulation));
	UpdateArticulationControls();
	UpdateUseInstArt();
}


void CInstrumentFVEditor::RefreshRegion()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	if(m_pCurRegion)
	{
		SetCurRegion(m_pCurRegion);
	}
	if (m_pCurArticulation)
	{
		SetCurArticulation(m_pCurArticulation);
	}

	char text[INST_SETTINGS_MAX_TEXT];
	long volToDisplay = -(m_pInstrument->GetMaxVolume());	 
	volToDisplay >>= 16;
	if(volToDisplay / 10 == 0 && volToDisplay % 10 == 0)
		sprintf(text, "%02d.%01d", volToDisplay / 10, volToDisplay % 10);
	else
		sprintf(text, "-%02d.%01d", volToDisplay / 10, volToDisplay % 10);
	
	CWnd* pDisplay = GetDlgItem(IDC_DMAXLEVEL);
	
	if(pDisplay)
	{
		pDisplay->SetWindowText(text);	
	}

	m_pComponent->m_pIFramework->RefreshNode(m_pCurRegion);
}

void CInstrumentFVEditor::SetCurRegion(CRegion* pRegion)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(pRegion);
	if(pRegion == NULL)
	{
		return;
	}

	CWnd * pDisplay = NULL;
	if (m_pCurRegion) 
	{
		m_pCurRegion->m_pInstrumentFVEditor = NULL;
	}
	m_pCurRegion = pRegion;
	m_pCurRegion->m_pInstrumentFVEditor = this;
	m_usActiveLayer = USHORT(pRegion->GetLayer());

	m_fInOnInitialUpdate = true;

	// Set the condition in the combo box
	// Init region conditions combo
	InitRegionConditionsCombo(m_pCurRegion);

	// Set the edit text
	char szWindowText[MAX_BUFFER];
	notetostring(m_pCurRegion->m_rRgnHeader.RangeKey.usLow, szWindowText);
	m_LowRangeEdit.SetWindowText(szWindowText);
	notetostring(m_pCurRegion->m_rRgnHeader.RangeKey.usHigh, szWindowText);
	m_HighRangeEdit.SetWindowText(szWindowText);
	
	m_dwLowerNoteRange = m_pCurRegion->m_rRgnHeader.RangeKey.usLow;
	m_dwUpperNoteRange = m_pCurRegion->m_rRgnHeader.RangeKey.usHigh;

	m_dwLowerVelocityRange = m_pCurRegion->m_rRgnHeader.RangeVelocity.usLow;
	m_dwUpperVelocityRange = m_pCurRegion->m_rRgnHeader.RangeVelocity.usHigh;
	
	CString sText;
	sText.Format("%d", m_dwLowerVelocityRange);
	m_VelocityLowRangeEdit.SetWindowText(sText);
	sText.Format("%d", m_dwUpperVelocityRange);
	m_VelocityHighRangeEdit.SetWindowText(sText);

	// Update spin control positions
	m_RangeSpin.SetPos(m_dwLowerNoteRange);
	m_ThruSpin.SetPos(m_dwUpperNoteRange);

	m_VelocityLowRangeSpin.SetPos(m_dwLowerVelocityRange);
	m_VelocityHighRangeSpin.SetPos(m_dwUpperVelocityRange);

	UpdateRootNote(m_pCurRegion->m_rWSMP.usUnityNote);
	pDisplay = NULL;
    m_fAllowOverlap = m_pCurRegion->m_rRgnHeader.fusOptions & F_RGN_OPTION_SELFNONEXCLUSIVE;

	pDisplay = GetDlgItem(IDC_REGION_OVERLAP);	
	if(pDisplay)
	{
		int state = m_fAllowOverlap ? 1 : 0;

		((CButton *)pDisplay)->SetCheck(state);
	}

	m_dwGroup = m_pCurRegion->m_rRgnHeader.usKeyGroup;
    //set the group
    CComboBox * pCombo = (CComboBox *)GetDlgItem(IDC_REGION_GROUP);
    if(pCombo)
    {
        pCombo->SetCurSel(m_dwGroup);
    }

	DeleteAndSetupWavesForCurrentRegion();

	int nLayer = m_pCurRegion->GetLayer();
	SetLayerScrollInfo(nLayer);

	UpdateUseInstArt();

	SendRegionChange();

	m_fInOnInitialUpdate = false;
}

BEGIN_EVENTSINK_MAP(CInstrumentFVEditor, CFormView)
    //{{AFX_EVENTSINK_MAP(CInstrumentFVEditor)
	ON_EVENT(CInstrumentFVEditor, IDC_REGION_REGIONKEYBOARD, 1 /* RegionSelectedChanged */, OnRegionSelectedChangedRegionRegionkeyboard, VTS_I2 VTS_I2)
	ON_EVENT(CInstrumentFVEditor, IDC_REGION_REGIONKEYBOARD, 2 /* NewRegion */, OnNewRegionRegionRegionkeyboard, VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CInstrumentFVEditor, IDC_REGION_REGIONKEYBOARD, 3 /* RangeChanged */, OnRangeChangedRegionRegionkeyboard, VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CInstrumentFVEditor, IDC_REGION_REGIONKEYBOARD, 4 /* NotePlayed */, OnNotePlayedRegionRegionkeyboard, VTS_I4 VTS_BOOL)
	ON_EVENT(CInstrumentFVEditor, IDC_REGION_REGIONKEYBOARD, 5 /* RegionMoved */, OnRegionMovedRegionRegionkeyboard, VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2)
	ON_EVENT(CInstrumentFVEditor, IDC_REGION_REGIONKEYBOARD, 6 /* RegionDeleted */, OnRegionDeletedRegionkeyboard, VTS_I2 VTS_I2)
	ON_EVENT(CInstrumentFVEditor, IDC_REGION_REGIONKEYBOARD, 7 /* ActiveLayerChanged */, OnActiveLayerChangedRegionRegionkeyboard, VTS_I2)
	ON_EVENT(CInstrumentFVEditor, IDC_REGION_REGIONKEYBOARD, 8 /* CopyRegion */, OnCopyRegionRegionkeyboard, VTS_I2 VTS_I2 VTS_I2 VTS_I2)
	ON_EVENT(CInstrumentFVEditor, IDC_REGION_REGIONKEYBOARD, 9 /* ScrollLayers */, OnScrollLayersRegionRegionkeyboard, VTS_BOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CInstrumentFVEditor::OnDrums() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_fInOnInitialUpdate)
	{
		return;
	}

	BOOL bOldValue = m_fIsDrumKit;
	if(UpdateData(TRUE))
	{	
		CWnd* pWnd = GetFocus();

		ValidateFullPatch(((UINT*)(&m_fIsDrumKit)), bOldValue, IDC_DRUMS);

		m_pInstrument->RefreshUI(false);	

		if(pWnd)
		{
			pWnd->SetFocus();
		}

		if(FAILED(m_pInstrument->SaveStateForUndo(IDS_DRUM_UNDO_TEXT)))
		{
			// Out Of Memory??
			m_fIsDrumKit = bOldValue;
			UpdateData(FALSE);
			return;
		}

		Update_and_Download(0);//To Do: use code
		
		// Break the Collection reference in the referring bands
		if(m_pInstrument && m_pInstrument->m_pComponent && m_pInstrument->m_pComponent->m_pIFramework)
			m_pInstrument->m_pComponent->m_pIFramework->NotifyNodes(m_pInstrument->m_pCollection, INSTRUMENT_NameChange, NULL);
	}
}

void CInstrumentFVEditor::OnRegionSelectedChangedRegionRegionkeyboard(short nLayer, short nStartNote)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(nLayer == -1)
		return;
	
	CRegion* pRegion = FindRegionFromMap(nLayer, nStartNote);
	if(pRegion)
	{
		// Set the region node as selected
		// OnNodeSelChanged will set the proper current region
		if(m_pCurRegion != pRegion || m_pInstrument->m_Regions.GetCount() == 1)
		{
			if(m_pComponent->m_pIFramework)
				m_pComponent->m_pIFramework->SetSelectedNode(pRegion);
		}
		
		if(pRegion->m_pArticulation && !pRegion->m_bUseInstrumentArticulation)
		{
			SetCurArticulation(pRegion->m_pArticulation);
			SetArticulationStaticIcon(false);
		}
		else
		{
			SetCurArticulation(m_pInstrument->GetCurrentArticulation());	
			SetArticulationStaticIcon(true);
		}

		// Switch the property page if it's showing to show the properties of the current region
		RefreshCurrentRegionPropertyPage();
	}
}



void CInstrumentFVEditor::OnRegionOverlap() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_fInOnInitialUpdate)
	{
		return;
	}

	ASSERT(m_pCurRegion);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(FAILED(m_pInstrument->SaveStateForUndo(IDS_REGION_OVERLAP_UNDO_TEXT)))
		{
			m_fAllowOverlap = !m_fAllowOverlap;
			UpdateData(FALSE);
			return;
		}

		if(m_fAllowOverlap)
	    {			
			m_pCurRegion->m_rRgnHeader.fusOptions |= F_RGN_OPTION_SELFNONEXCLUSIVE;
		}
		else
		{
			m_pCurRegion->m_rRgnHeader.fusOptions &= ~F_RGN_OPTION_SELFNONEXCLUSIVE;
		}

		// Set flag so we know to save file 
		m_pCollection->SetDirtyFlag();
		
		m_pInstrument->UpdateInstrument();
	}
}

void CInstrumentFVEditor::OnRegionUia() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_fInOnInitialUpdate)
	{
		return;
	}
	/*ASSERT(m_pInstrument->GetCurrentArticulation() &&
		   m_pCurRegion->m_pArticulation);*/

	if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_USE_INSTRUMENT_ARTICULATION)))
		return;

	if (UpdateData(TRUE))
	{
		m_pCurRegion->m_bUseInstrumentArticulation = m_fUseInstArt;
		if(m_fUseInstArt)
		{
			m_pCurArticulation = m_pInstrument->GetCurrentArticulation();		
			SetArticulationStaticIcon(true);
		}
		else
		{
			m_pCurArticulation = m_pCurRegion->m_pArticulation;			
			SetArticulationStaticIcon(false);
		}
		SetCurArticulation(m_pCurArticulation);	
		m_pInstrument->UpdateInstrument();
		m_pCollection->SetDirtyFlag();
	}
}

LRESULT CInstrumentFVEditor::OnUpdateMIDIValues(UINT wParam, LONG lParam)
{
	switch(wParam)
	{
		case IDC_REGION_ELRANGE:
		{
			UpdateNoteRange((BYTE)lParam, m_dwUpperNoteRange, true);
			break;
		}
		case IDC_REGION_EURANGE:
		{
			UpdateNoteRange(m_dwLowerNoteRange, (BYTE)lParam, false);
			break;
		}
		case IDC_REGION_ROOT_NOTE:
		{
			UpdateRootNote((BYTE)lParam);
			break;
		}
		default:
			break;
	}

	return 0;
}

// =====================================================================
// OnMidiMsg
//  This is received when user hits a key in the midi keybd.
// RegisterMidi() should have been called for this to work.
// =====================================================================
HRESULT CInstrumentFVEditor::OnMidiMsg(REFERENCE_TIME dwTime, 
									   BYTE bStatus, 
									   BYTE bData1, 
									   BYTE bData2)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(m_pInstrument);
	if(m_pInstrument== NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pCurRegion);
	if(m_pCurRegion == NULL)
	{
		return E_UNEXPECTED;
	}
	
	CString csMenuText;
	BYTE nMsg = bStatus & 0xF0;

	if(nMsg == MIDI_NOTEON)
	{
		if(m_nHasFocus == IDC_REGION_ELRANGE)
		{
			if(m_pCurRegion->m_rRgnHeader.RangeKey.usLow != (WORD) bData1)
			{
				PostMessage(DM_UPDATE_VALUES, IDC_REGION_ELRANGE, bData1);
			}
		}
		else if(m_nHasFocus == IDC_REGION_EURANGE)
		{
			if(m_pCurRegion->m_rRgnHeader.RangeKey.usHigh != (WORD) bData1)
			{
				PostMessage(DM_UPDATE_VALUES, IDC_REGION_EURANGE, bData1);
			}
		}
		else if(m_nHasFocus == IDC_REGION_ROOT_NOTE)
		{
			if(m_pCurRegion->m_rWSMP.usUnityNote != (WORD) bData1)
			{
				PostMessage(DM_UPDATE_VALUES, IDC_REGION_ROOT_NOTE, bData1);
			}
		}
	}

	// Don't play anything if the note is not in the region range
	if(FindRegionFromMap(bData1) == NULL)
	{
		return S_OK;
	}

	CPtrList lstPlayingRegions;
	if(FAILED(m_pInstrument->m_Regions.FindPlayingRegions(bData1, bData2, &lstPlayingRegions)))
	{
		return E_FAIL;
	}

	int nPlayingRegions = lstPlayingRegions.GetCount();
	if(nPlayingRegions == 0)
	{
		return S_OK;
	}

    m_RegionKeyBoard.MidiEvent(bData1, nMsg, bData2);

	bool bAuditionMode = m_pInstrument->GetAuditionMode();
	if(nPlayingRegions == 1 && bAuditionMode == AUDITION_SOLO && (nMsg == MIDI_NOTEON || nMsg == MIDI_NOTEOFF))
	{
        CRegion* pRegion = (CRegion*) lstPlayingRegions.GetHead();
		ASSERT(pRegion);
		if(pRegion)
		{
			SendMessage(DM_REGION_SELECT, pRegion->GetLayer(), pRegion->m_rRgnHeader.RangeKey.usLow);
		}
	}

	// Note Off
	if(nMsg == MIDI_NOTEOFF)
	{
		if ( m_parent->m_nMIDINoteOns[bData1] > 0 )
		{
			m_parent->m_nMIDINoteOns[bData1]--;
		}
	}
	else if (nMsg == MIDI_NOTEON)
	{
		m_parent->m_nMIDINoteOns[bData1]++;
		m_pInstrument->UpdatePatch();
	}
	
	m_pComponent->PlayMIDIEvent(nMsg, bData1, bData2, 5, m_pInstrument->IsDrum());	
	return S_OK;
}

LRESULT CInstrumentFVEditor::OnMIDIRegionSelect(UINT wParam, LONG lParam)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	USHORT usLayer = (USHORT)wParam;
	USHORT usNote = (USHORT)lParam;

    m_RegionKeyBoard.SetCurrentRegion(usLayer, usNote); 
    OnRegionSelectedChangedRegionRegionkeyboard(usLayer, usNote);

	return 0;
}

void CInstrumentFVEditor::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Turn off all the notes that are playing 
	TurnOffMidiNotes();
	
	CPoint tempPoint = point;
	CPoint ptScrollOffset;

	ScreenToClient(&tempPoint);
	ptScrollOffset = GetDeviceScrollPosition();//take into account scrolling of this view
	tempPoint.x += ptScrollOffset.x;
	tempPoint.y += ptScrollOffset.y;

	CRect rcInstPopUp;
	GetControlPosition(IDC_STATIC_INSTRUMENTGROUP, rcInstPopUp);
	
	CRect rcRegionPopUp;
	GetControlPosition(IDC_STATIC_REGIONGROUP, rcRegionPopUp);
	
	CRect rcRegionKeyboardPopUp;
	GetControlPosition(IDC_REGION_REGIONKEYBOARD, rcRegionKeyboardPopUp);

	CRect rcArtPopUp;
	GetControlPosition(IDC_ARTICULATION_STATIC, rcArtPopUp);


	CMenu* pPopupMenu = NULL;
	
	if(rcInstPopUp.PtInRect(tempPoint))
	{
		// Don't do anything if we don't have a menu yet..
		if(m_pContextInstMenu && ::IsMenu(m_pContextInstMenu->m_hMenu) == 0)
			return;

		pPopupMenu =  m_pContextInstMenu->GetSubMenu(0);
	}
	else if(rcRegionPopUp.PtInRect(tempPoint) || rcRegionKeyboardPopUp.PtInRect(tempPoint))
	{
		// Don't do anything if we don't have a menu yet..
		if(m_pContextRegionMenu && ::IsMenu(m_pContextRegionMenu->m_hMenu) == 0)
			return;

		pPopupMenu =  m_pContextRegionMenu->GetSubMenu(0);

		pPopupMenu->EnableMenuItem(IDM_INST_ED_REGION_NEW_REGION, MF_BYCOMMAND | MF_ENABLED);

		if (1 < m_pInstrument->m_Regions.GetCount())//can't delete the only region
		{
			pPopupMenu->EnableMenuItem(ID_INST_ED_REGION_DELETE, MF_BYCOMMAND | MF_ENABLED);
		}
		else
		{
			pPopupMenu->EnableMenuItem(ID_INST_ED_REGION_DELETE, MF_BYCOMMAND | MF_GRAYED);
		}

		short nLayers = m_RegionKeyBoard.GetNumberOfLayers();
		if(nLayers > 4)
		{
			pPopupMenu->EnableMenuItem(ID_INST_ED_LAYER_DELETE, MF_BYCOMMAND | MF_ENABLED);
		}
		else
		{
			pPopupMenu->EnableMenuItem(ID_INST_ED_LAYER_DELETE, MF_BYCOMMAND | MF_GRAYED);
		}
	}
	else if(rcArtPopUp.PtInRect(tempPoint))
	{
		// Don't do anything if we don't have a menu yet..
		if(m_pContextArtMenu && ::IsMenu(m_pContextArtMenu->m_hMenu) == 0)
			return;

		pPopupMenu =  m_pContextArtMenu->GetSubMenu(0);
	}

    if (pPopupMenu)
    {
        // Check to see if paste should be enabled?
	    IDataObject* pIDataObject = NULL;
	    HRESULT hr = OleGetClipboard(&pIDataObject);
        if (SUCCEEDED(hr))
        {
	        CJazzDataObject* pDataObject = new CJazzDataObject();
	        if(pDataObject)
	        {    	
	            // This adds the format and sets the format to the object's current format.
	            if (SUCCEEDED( hr = pDataObject->AddClipFormat(m_cfFormatArt)))
                {    		
	                HRESULT hr = pDataObject->AttemptRead(pIDataObject);
                    if (SUCCEEDED(hr))
                    {
			            pPopupMenu->EnableMenuItem(ID_INST_ED_ARTICULATION_PASTE, MF_BYCOMMAND | MF_ENABLED);
                    }
                    else
                    {
			            pPopupMenu->EnableMenuItem(ID_INST_ED_ARTICULATION_PASTE, MF_BYCOMMAND | MF_GRAYED);
                    }
                    pDataObject->Release();
                }
            }
        pIDataObject->Release();
        }

		pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON,
								   point.x,
								   point.y,
								   this,
								   NULL);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CInstrument IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CInstrumentFVEditor IPersist::GetClassID

HRESULT CInstrumentFVEditor::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pClsId != NULL );
    *pClsId = GUID_NULL;//memset( pClsId, 0, sizeof( CLSID ) );
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentFVEditor IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentFVEditor IPersistStream::IsDirty
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentFVEditor::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	// I want to know if I am called
	ASSERT(FALSE);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentFVEditor IPersistStream::GetSizeMax
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentFVEditor::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	// I want to know if I am called
	ASSERT(FALSE);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentFVEditor IPersistStream::Load
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentFVEditor::Load(IStream* pIStream)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT( pIStream != NULL );
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
//
// CInstrumentFVEditor IPersistStream::Save
//
//////////////////////////////////////////////////////////////////////
HRESULT CInstrumentFVEditor::Save(IStream* pIStream, BOOL fClearDirty)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

void CInstrumentFVEditor::OnInstrumentProperties() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_pInstrument->OnShowProperties();
}

void CInstrumentFVEditor::OnRegionProperties() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_pCurRegion->OnShowProperties();
}

void CInstrumentFVEditor::OnInstEdRegionNewRegion() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_REGION_INSERT)))
		return;
	HRESULT hr = m_pInstrument->m_Regions.InsertChildNode(NULL);
}

void CInstrumentFVEditor::OnNewRegionRegionRegionkeyboard(short nLayer, long lower, long upper) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_REGION_INSERT)))
		return;
	HRESULT hr = m_pInstrument->m_Regions.InsertRegion(NULL, nLayer, (USHORT) lower, (USHORT) upper, false);
	if(SUCCEEDED(hr))
	{
		SetLayerScrollInfo(nLayer);
	}
}

void CInstrumentFVEditor::OnRangeChangedRegionRegionkeyboard(short nLayer, long lower, long upper) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
		return;

	if(m_pCurRegion == NULL)
		return;

	if(FAILED(m_pInstrument->SaveStateForUndo(IDS_RANGE_UNDO_TEXT)))
		return;

	TurnOffMidiNotes();

	m_dwLowerNoteRange = lower;
	m_dwUpperNoteRange = upper;
	m_pCurRegion->m_rRgnHeader.RangeKey.usLow = (WORD) lower;
	m_pCurRegion->m_rRgnHeader.RangeKey.usHigh = (WORD) upper;
		
	m_bTouchedByProgram = false;
	m_RangeSpin.SetPos(lower);
	m_ThruSpin.SetPos(upper);

	RegionChangeCommonTasks();
	
	if(m_pInstrument)
		m_pComponent->m_pIFramework->SortChildNodes(&(m_pInstrument->m_Regions));

	if(m_pCurRegion && m_pCurRegion->m_pRegionPropPgMgr)
		m_pCurRegion->m_pRegionPropPgMgr->RefreshData();

}

void CInstrumentFVEditor::SendRegionChange()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pCurRegion);
	if(m_pCurRegion == NULL)
		return;

	m_RegionKeyBoard.SetCurrentRegion(short(m_pCurRegion->GetLayer()), m_pCurRegion->m_rRgnHeader.RangeKey.usLow);				
	m_RegionKeyBoard.SetRootNote((short)m_lUnityNote);
}

CRegion* CInstrumentFVEditor::FindRegionFromMap(short nLayer, short nStartNote)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pInstrument == NULL)
		return NULL; 

	return m_pInstrument->m_Regions.FindRegionFromMap(nLayer, nStartNote);
}

CRegion* CInstrumentFVEditor::FindRegionFromMap(int nNote)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pInstrument == NULL)
		return NULL; 

	return m_pInstrument->m_Regions.FindRegionFromMap(nNote);
}



void CInstrumentFVEditor::OnArticulationDelete() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
}

void CInstrumentFVEditor::OnRegionDelete() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Turn off all the notes to prevent hanging notes
	TurnOffMidiNotes();

	m_pCurRegion->m_bDeletingRegion = true;
	CRegion* pNextRegion = m_pCurRegion->GetNext();
	if(SUCCEEDED(m_pCurRegion->DeleteNode( TRUE )))
	{
		if(pNextRegion)
			SetCurRegion(pNextRegion);
		else
			SetCurRegion(m_pInstrument->m_Regions.GetHead());
	}
}

void CInstrumentFVEditor::OnArticulationCopy() 
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = S_OK;

	IStream *pStream = NULL;
	CJazzDataObject *pDataObject = NULL;

	hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
	if(SUCCEEDED(hr))
	{
	    hr = m_pCurArticulation->Save(pStream, TRUE);
	    if(SUCCEEDED(hr))
	    {
	        pDataObject = new CJazzDataObject();
	        if(pDataObject)
	        {
    	        hr = pDataObject->Initialize(m_cfFormatArt, pStream);
		        if (SUCCEEDED(hr))
                {
	                IDataObject *pIDataObject = NULL;			            
    	            hr = pDataObject->QueryInterface(IID_IDataObject, (void **) &pIDataObject);
                    if (SUCCEEDED(hr))
                    {
                        hr = OleSetClipboard(pIDataObject);
                    }

                    if (pIDataObject)
                    {
                        pIDataObject->Release();
                    }
                }
                else    //initialize failed.
                {
                    pDataObject->Release();
                    // stream gets released before returning,
                }
            }
        }
    }

	if(m_pComponent->m_pCopyDataObject != NULL)
	{
		m_pComponent->m_pCopyDataObject->Release();
	}
	        
	m_pComponent->m_pCopyDataObject = pDataObject;

    if (pStream)
    {
	    pStream->Release();
    }
}

void CInstrumentFVEditor::OnArticulationPaste() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = S_OK;
	
	IDataObject* pIDataObject = NULL;
	CJazzDataObject* pDataObject = NULL;
	IStream* pStream = NULL;

	hr = OleGetClipboard(&pIDataObject);
	if(FAILED(hr))
	{
        goto Err;
		// J3 need to handle error properly		
	}

	pDataObject = new CJazzDataObject();
	if(pDataObject == NULL)
	{
		// J3 need to handle error properly		
		hr = E_OUTOFMEMORY;
        goto Err;
	}
	
	// This adds the format and sets the format to the object's current format.
	hr = pDataObject->AddClipFormat(m_cfFormatArt);
	
	ASSERT(SUCCEEDED(hr));
	
	hr = pDataObject->AttemptRead(pIDataObject);
	
	if(FAILED(hr))
	{
		hr = E_FAIL;
        goto Err;
	}
	
	hr = pDataObject->GetIStream(&pStream);
	
	if(hr != S_OK)
	{
		hr = E_FAIL;
        goto Err;
	}
	
	// Seek to the beginning of the stream
	LARGE_INTEGER	liStreamPos;
	liStreamPos.QuadPart = 0;

	hr = pStream->Seek(liStreamPos, STREAM_SEEK_SET, NULL);

	//ASSERT(SUCCEEDED(hr));
    if (SUCCEEDED(hr))
    {
	    m_pCurArticulation->Load(pStream);

		RefreshRegion();
    }
Err:
    if (pStream)
	    pStream->Release(); 

    if (pDataObject)
	    pDataObject->Release();

    if (pIDataObject)
	    pIDataObject->Release();
}

void CInstrumentFVEditor::RegisterMidi() 
{
	REGISTER_MIDI_IN(m_pComponent->m_pIConductor, m_dwCookie)		
}

void CInstrumentFVEditor::UnRegisterMidi()
{
	UNREGISTER_MIDI_IN(m_pComponent->m_pIConductor, m_dwCookie)
	m_dwCookie = 0;
}

bool CInstrumentFVEditor::IsMidiRegistered()
{
	if(m_dwCookie == 0)
		return false;
	else
		return true;
}


void CInstrumentFVEditor::OnSelchangeRegionGroup() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    CComboBox *pCombo = (CComboBox *) GetDlgItem(IDC_REGION_GROUP);
    if (pCombo != NULL)
    {
        int index = pCombo->GetCurSel();
		if (index != CB_ERR && m_pCurRegion)
        {
            m_dwGroup = index;
		    if(m_pCurRegion->m_rRgnHeader.usKeyGroup != (USHORT) m_dwGroup)
			{   //save undo image
				if(FAILED(m_pInstrument->SaveStateForUndo(IDS_KEY_GROUP_UNDO_TEXT)))
				{
					m_dwGroup = m_pCurRegion->m_rRgnHeader.usKeyGroup;
					pCombo->SetCurSel(m_dwGroup);
					return;
				}

			    m_pCurRegion->m_rRgnHeader.usKeyGroup = (USHORT) m_dwGroup;
			    // Set flag so we know to save file 
			    m_pCollection->SetDirtyFlag();
			    m_pInstrument->UpdateInstrument();
		    }
        }
    }
}

// =================================================================
// =================================================================
BOOL CInstrumentFVEditor::IsValidMidiNoteText(char * psz)
{
	return isValidNoteString(psz);
}

//EN_CHANGE Handlers to flag when the user has editted a value in an edit control
//Because a SetWindowText() on the edit control will generate an EN_CHANGE notification
//we isolate user input using flags.
void CInstrumentFVEditor::OnChangeBank() 
{	
	if ( !m_bTouchedByProgram && !m_fInOnInitialUpdate )
	{
		if (m_bTouchedBySpinner)
		{
			m_bTouchedBySpinner = false;
			return;
		}
		m_bfTouched |= fBANK;
	}
}

void CInstrumentFVEditor::OnChangeBank2() 
{
	if ( !m_bTouchedByProgram && !m_fInOnInitialUpdate )
	{
		if (m_bTouchedBySpinner)
		{
			m_bTouchedBySpinner = false;
			return;
		}
		m_bfTouched |= fBANK2;
	}
}

void CInstrumentFVEditor::OnChangePatch() 
{
	if ( !m_bTouchedByProgram && !m_fInOnInitialUpdate )
		m_bfTouched |= fPATCH;
}

void CInstrumentFVEditor::OnChangeLowerRange() 
{
	if ( !m_bTouchedByProgram && !m_fInOnInitialUpdate )
		m_bfTouched |= fLOWER_RANGE;
}

void CInstrumentFVEditor::OnChangeUpperRange() 
{
	if ( !m_bTouchedByProgram && !m_fInOnInitialUpdate )
		m_bfTouched |= fUPPER_RANGE;	
}

void CInstrumentFVEditor::OnChangeRootNote() 
{
	if ( !m_bTouchedByProgram && !m_fInOnInitialUpdate )
		m_bfTouched |= fROOT_NOTE;	
}
// End EN_CHANGE Handlers 
// ----------------------------------------------------------------------------------------

LRESULT CInstrumentFVEditor::OnValidate(UINT wParam,LONG lParam)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	switch (wParam)
	{
		case IDC_PATCH:
			ValidatePatch();
			break;
		case IDC_BANK:
			ValidateBank();
			break;
		case IDC_BANK2:
			ValidateBank2();
			break;
		default:
			break;
	}
	return (0);
}

void CInstrumentFVEditor::RegionChangeCommonTasks(bool bUpdateFramework)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pCurRegion);

	// Update RegionKeyBoard
	m_RegionKeyBoard.SetRootNote(m_pCurRegion->m_rWSMP.usUnityNote);
	if (bUpdateFramework)
	{
		// Update Project Tree
		m_pComponent->m_pIFramework->RefreshNode(m_pCurRegion);
	}

	// Recalculate note we will play when space bar is pressed
	m_pInstrument->m_nMIDINote = CalculateAuditionNote();
	// Set flag so we know to save file 
	m_pCollection->SetDirtyFlag();
	// Download changes to the Synth				
	m_pInstrument->UpdateInstrument();
	
	RefreshRegion();
}

void CInstrumentFVEditor::OnKillfocusBank() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	SendMessage(DM_VALIDATE,IDC_BANK);	
}

void CInstrumentFVEditor::ValidateBank()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	UINT uOldBank = m_wBank;

	if ( m_bfTouched & fBANK )
	{
		if(UpdateData(TRUE))
		{
			ValidateFullPatch(&m_wBank, uOldBank, IDC_BANK);
			if ( m_wBank != MSB(m_pInstrument->m_rInstHeader.Locale.ulBank) )
			{	
				if(FAILED(m_pInstrument->SaveStateForUndo(IDS_MSB_UNDO_TEXT)))
				{
					// Out Of Memory??
					m_bfTouched &= ~fBANK;
					m_wBank = uOldBank;
					UpdateData(FALSE);
					return;
				}
				Update_and_Download(0);

				// Break the Collection reference in the referring bands
				if(m_pInstrument && m_pInstrument->m_pComponent && m_pInstrument->m_pComponent->m_pIFramework)
					m_pInstrument->m_pComponent->m_pIFramework->NotifyNodes(m_pInstrument->m_pCollection, INSTRUMENT_NameChange, NULL);
			}
			m_bfTouched &= ~fBANK;
		}// if UpdateData
		else
		{
			m_wBank = uOldBank;
			UpdateData(FALSE);
			CString sMSBValue;
			CEdit* pMSBEdit = (CEdit*) GetDlgItem(IDC_BANK);
			if(pMSBEdit)
			{
				pMSBEdit->SetFocus();
				pMSBEdit->SetSel(0, -1);
				PostMessage(DM_BRINGTOTOP);
			}
		}

		// Refresh the Instrument's property page if it's showing
		if(m_pInstrument)
		{
			m_pInstrument->RefreshUI(false);
		}
	}
}


void CInstrumentFVEditor::OnUpdateBank() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	
	if (!m_MSBSpin)
		return;
	if (!m_bTouchedBySpinner)
	{
		return;
	}
	if(UpdateData(TRUE))
	{	
		if ( m_wBank != MSB(m_pInstrument->m_rInstHeader.Locale.ulBank) )
		{	
			if(FAILED(m_pInstrument->SaveStateForUndo(IDS_MSB_UNDO_TEXT)))
			{
				// Out Of Memory??
				m_wBank = MSB(m_pInstrument->m_rInstHeader.Locale.ulBank);
				UpdateData(FALSE);
				return;
			}

			Update_and_Download(0);

			// Break the Collection reference in the referring bands
			if(m_pInstrument && m_pInstrument->m_pComponent && m_pInstrument->m_pComponent->m_pIFramework)
				m_pInstrument->m_pComponent->m_pIFramework->NotifyNodes(m_pInstrument->m_pCollection, INSTRUMENT_NameChange, NULL);
		}

		// Refresh the Instrument's property page if it's showing
		if(m_pInstrument)
		{
			m_pInstrument->RefreshUI(false);
		}

	}
}

void CInstrumentFVEditor::OnKillfocusBank2() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	SendMessage(DM_VALIDATE,IDC_BANK2);	
}

void CInstrumentFVEditor::ValidateBank2()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	

	UINT uOldBank2 = m_wBank2;

	if ( m_bfTouched & fBANK2 )
	{
		if(UpdateData(TRUE))
		{
			ValidateFullPatch(&m_wBank2, uOldBank2, IDC_BANK2);
			if ( m_wBank2 != LSB(m_pInstrument->m_rInstHeader.Locale.ulBank) )
			{	
				if(FAILED(m_pInstrument->SaveStateForUndo(IDS_LSB_UNDO_TEXT)))
				{
					m_bfTouched &= ~fBANK2;
					m_wBank2 = uOldBank2;
					UpdateData(FALSE);
					return;
				}
				Update_and_Download(0);

				// Break the Collection reference in the referring bands
				if(m_pInstrument && m_pInstrument->m_pComponent && m_pInstrument->m_pComponent->m_pIFramework)
					m_pInstrument->m_pComponent->m_pIFramework->NotifyNodes(m_pInstrument->m_pCollection, INSTRUMENT_NameChange, NULL);
			}
			m_bfTouched &= ~fBANK2;
		}
		else
		{
			m_wBank2 = uOldBank2;
			UpdateData(FALSE);

			CEdit* pLSBEdit = (CEdit*) GetDlgItem(IDC_BANK2);

			if(pLSBEdit)
			{
				pLSBEdit->SetFocus();
				pLSBEdit->SetSel(0, -1);
				PostMessage(DM_BRINGTOTOP);
			}

		}
		// Refresh the Instrument's property page if it's showing
		if(m_pInstrument)
		{
			m_pInstrument->RefreshUI(false);
		}

	} 
}

void CInstrumentFVEditor::OnUpdateBank2() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	
	if (!m_LSBSpin)
		return;
	if (!m_bTouchedBySpinner)
	{
		return;
	}
	if(UpdateData(TRUE))
	{	
		if ( m_wBank2 != LSB(m_pInstrument->m_rInstHeader.Locale.ulBank) )
		{	
			if(FAILED(m_pInstrument->SaveStateForUndo(IDS_LSB_UNDO_TEXT)))
			{
				// Out Of Memory??
				m_wBank2 = LSB(m_pInstrument->m_rInstHeader.Locale.ulBank);
				UpdateData(FALSE);
				return;
			}
			Update_and_Download(0);
			
			// Break the Collection reference in the referring bands
			if(m_pInstrument && m_pInstrument->m_pComponent && m_pInstrument->m_pComponent->m_pIFramework)
				m_pInstrument->m_pComponent->m_pIFramework->NotifyNodes(m_pInstrument->m_pCollection, INSTRUMENT_NameChange, NULL);
		}
		
		// Refresh the Instrument's property page if it's showing
		if(m_pInstrument)
		{
			m_pInstrument->RefreshUI(false);
		}

	}
	

}

// =================================================================
// =================================================================
void CInstrumentFVEditor::OnUpdatePatch() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if (!m_bTouchedBySpinner)
	{
		return;
	}
    if ( !m_PatchSpin )
    {
		return;
	}
    ASSERT(m_pInstrument);
    if (UpdateData(TRUE))
    {
		if(m_pInstrument->m_rInstHeader.Locale.ulInstrument != m_wPatch)
        {
			if(FAILED(m_pInstrument->SaveStateForUndo(IDS_PATCH_UNDO_TEXT)))
			{
				m_wPatch = m_pInstrument->m_rInstHeader.Locale.ulInstrument;
				UpdateData(FALSE);
				return;
			}
			Update_and_Download(0);
			
			// Break the Collection reference in the referring bands
			if(m_pInstrument && m_pInstrument->m_pComponent && m_pInstrument->m_pComponent->m_pIFramework)
				m_pInstrument->m_pComponent->m_pIFramework->NotifyNodes(m_pInstrument->m_pCollection, INSTRUMENT_NameChange, NULL);

        }

		// Refresh the Instrument's property page if it's showing
		if(m_pInstrument)
		{
			m_pInstrument->RefreshUI(false);
		}
    }    
}

void CInstrumentFVEditor::OnKillfocusPatch() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	SendMessage(DM_VALIDATE,IDC_PATCH);	
}

void CInstrumentFVEditor::ValidatePatch()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if ( m_bfTouched & fPATCH )
	{
		UINT	uOldPatch = m_wPatch;
		if(UpdateData(TRUE))
		{
			ValidateFullPatch(&m_wPatch, uOldPatch, IDC_PATCH);
			if(m_pInstrument->m_rInstHeader.Locale.ulInstrument != m_wPatch)
			{
				if(FAILED(m_pInstrument->SaveStateForUndo(IDS_PATCH_UNDO_TEXT)))
				{
					m_wPatch = uOldPatch;
					UpdateData(FALSE);
					return;
				}
				Update_and_Download(0);
				
				// Break the Collection reference in the referring bands
				if(m_pInstrument && m_pInstrument->m_pComponent && m_pInstrument->m_pComponent->m_pIFramework)
					m_pInstrument->m_pComponent->m_pIFramework->NotifyNodes(m_pInstrument->m_pCollection, INSTRUMENT_NameChange, NULL);

			}
			m_bfTouched &= ~fPATCH;
		}
		else
		{
			m_wPatch = uOldPatch;
			UpdateData(FALSE);
			CEdit* pPatchEdit = (CEdit*) GetDlgItem(IDC_PATCH);

			if(pPatchEdit)
			{
				pPatchEdit->SetFocus();
				pPatchEdit->SetSel(0, -1);
				PostMessage(DM_BRINGTOTOP);
			}

		}

		// Refresh the Instrument's property page if it's showing
		if(m_pInstrument)
		{
			m_pInstrument->RefreshUI(false);
		}
	}
}

// =========================================================================
// Whenever we are dropped down, populate the list with all waves currently present.
// =========================================================================
void CInstrumentFVEditor::OnDropdownRegionWavelink() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (m_pCurRegion)
    {    
		DeleteAndSetupWavesForCurrentRegion();
	}
}


void CInstrumentFVEditor::OnSelchangeRegionWavelink() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	TurnOffMidiNotes();
    CComboBox *pCombo = (CComboBox*) GetDlgItem(IDC_REGION_WAVELINK);
    if (pCombo != NULL)
    {
        int index = pCombo->GetCurSel();
		if (index != CB_ERR)
		{
			CWave* pWave = (CWave*) pCombo->GetItemDataPtr(index);
			if (pWave != m_pWave)
			{
				if ( !m_fInOnInitialUpdate )
				{
					if(FAILED(m_pInstrument->SaveStateForUndo(IDS_WAVELINK_UNDO_TEXT)))
					{
						// Out Of Memory??
						int nItems = pCombo->GetCount();
						for(int nIndex = 0; nIndex < nItems; nIndex++)
						{
							CWave* pWave = (CWave*) pCombo->GetItemDataPtr(nIndex);
							if(pWave == m_pWave)
								break;
						}
						pCombo->SetCurSel(nIndex);

						return;
					}
				}

				// Remember the old root note
				USHORT nOldRoot = m_pCurRegion->m_rWSMP.usUnityNote;
				
				m_pCurRegion->SetWave(pWave);

				if(m_pCurRegion->m_bOverrideWaveRootNote)
					m_pCurRegion->m_rWSMP.usUnityNote = nOldRoot;
				
				UpdateRootNote(m_pCurRegion->m_rWSMP.usUnityNote);
				
				// if overrides were set turn them off for new wave
				m_pCurRegion->m_bOverrideWaveMoreSettings = false;
				m_pCurRegion->m_bOverrideWaveAttenuation = false;
				m_pCurRegion->m_bOverrideWaveFineTune = false;

				m_pInstrument->UpdateInstrument();
			}
		}
		// Update Project Tree
		m_pComponent->m_pIFramework->RefreshNode(m_pCurRegion);
	}
}

void CInstrumentFVEditor::UpdateUseInstArt()
{
	if(m_pCurRegion == NULL)
		return;

	if(m_pCurRegion->m_bUseInstrumentArticulation)
	{	
		if(m_pInstrument->GetCurrentArticulation())
		{
			m_fUseInstArt = true;
			m_pCurArticulation = m_pInstrument->GetCurrentArticulation();
		}
		else
		{
			m_pCurRegion->m_bUseInstrumentArticulation = m_fUseInstArt = false;
			m_pCurArticulation = m_pCurRegion->m_pArticulation;
		}
	}
	else
	{
		if ( m_pCurRegion->m_pArticulation )
		{
			m_fUseInstArt = false;
			m_pCurArticulation = m_pCurRegion->m_pArticulation;
		}
		else
		{
			m_pCurRegion->m_bUseInstrumentArticulation = m_fUseInstArt = true;
			m_pCurArticulation = m_pInstrument->GetCurrentArticulation();
		}
	}	

	CWnd* pDisplay = GetDlgItem(IDC_REGION_UIA);	

	if(pDisplay == NULL)
	{
		return;
	}
	
	if(m_pCurRegion->m_pArticulation == NULL || 
	   m_pInstrument->GetCurrentArticulation() == NULL)
	{
		pDisplay->EnableWindow(FALSE);
	}
	else
	{
		pDisplay->EnableWindow(TRUE);
	}
	((CButton *)pDisplay)->SetCheck(m_fUseInstArt);
}

WORD CInstrumentFVEditor::CalculateAuditionNote()
{
	if(m_pCurRegion == NULL)
		return 0;

	if (m_pCurRegion->m_bOverrideWaveRootNote)
		return (WORD) m_pCurRegion->m_rWSMP.usUnityNote;
	else
		if (m_pCurRegion->m_pWave)
			return (WORD) m_pCurRegion->m_pWave->GetUnityNote();
		else
			return 0;
}

void CInstrumentFVEditor::OnDeltaPosMSBSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int		iRealDelta = pNMUpDown->iDelta;
	int		iProposedPos = pNMUpDown->iPos + pNMUpDown->iDelta;
	BYTE	bNewMSB = (BYTE) m_wBank;
	HRESULT				hr;
	CInstrument*		pDummy;

	*pResult = 0;
	while((iProposedPos >= 0) && (iProposedPos < 128))
	{
		hr = m_pComponent->IsValidPatch(m_pInstrument, MAKE_BANK(m_fIsDrumKit, (bNewMSB + iRealDelta), m_wBank2), (ULONG)m_wPatch, NULL, &pDummy);
		if (SUCCEEDED(hr))
		{
			pNMUpDown->iDelta = iRealDelta;
			m_bTouchedBySpinner = true;
			return;
		}
		if (iRealDelta < 0)
		{
			iRealDelta--;
		}
		else
		{
			iRealDelta++;
		}
		iProposedPos = pNMUpDown->iPos + iRealDelta;
	}
	*pResult = 1;//non-zero allow no change
	// To Do: inform user?
}

void CInstrumentFVEditor::OnDeltaPosLSBSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int		iRealDelta = pNMUpDown->iDelta;
	int		iProposedPos = pNMUpDown->iPos + pNMUpDown->iDelta;
	BYTE	bNewLSB = (BYTE) m_wBank2;
	HRESULT				hr;
	CInstrument*		pDummy;

	*pResult = 0;
	while((iProposedPos >= 0) && (iProposedPos < 128))
	{
		hr = m_pComponent->IsValidPatch(m_pInstrument, MAKE_BANK(m_fIsDrumKit, m_wBank , bNewLSB + iRealDelta), (ULONG)m_wPatch, NULL, &pDummy);
		if (SUCCEEDED(hr))
		{
			pNMUpDown->iDelta = iRealDelta;
			m_bTouchedBySpinner = true;
			return;
		}
		if (iRealDelta < 0)
		{
			iRealDelta--;
		}
		else
		{
			iRealDelta++;
		}
		iProposedPos = pNMUpDown->iPos + iRealDelta;
	}
	*pResult = 1;//non-zero allow no change
	// To Do: inform user?
}

void CInstrumentFVEditor::OnDeltaPosPatchSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int		iRealDelta = pNMUpDown->iDelta;
	int		iProposedPos = pNMUpDown->iPos + pNMUpDown->iDelta;
	BYTE	bNewPatch = (BYTE) m_wPatch;
	HRESULT				hr;
	CInstrument*		pDummy;

	*pResult = 0;
	while((iProposedPos >= 0) && (iProposedPos < 128))
	{
		hr = m_pComponent->IsValidPatch(m_pInstrument, MAKE_BANK(m_fIsDrumKit, m_wBank , m_wBank2), (ULONG)(bNewPatch + iRealDelta), NULL, &pDummy);
		if (SUCCEEDED(hr))
		{
			pNMUpDown->iDelta = iRealDelta;
			m_bTouchedBySpinner = true;
			return;
		}
		if (iRealDelta < 0)
		{
			iRealDelta--;
		}
		else
		{
			iRealDelta++;
		}
		iProposedPos = pNMUpDown->iPos + iRealDelta;
	}
	*pResult = 1;//non-zero allow no change
	// To Do: inform user?
}

void CInstrumentFVEditor::Update_and_Download(UINT uCause)
{
	m_pInstrument->m_rInstHeader.Locale.ulBank = MAKE_BANK(m_fIsDrumKit, m_wBank, m_wBank2);
	m_pInstrument->m_rInstHeader.Locale.ulInstrument = m_wPatch;
	// Set flag so we know to save file
	m_pCollection->SetDirtyFlag();
	m_pInstrument->m_pComponent->m_pIFramework->RefreshNode(m_pInstrument);
	m_pInstrument->UpdateInstrument();
	m_pInstrument->UpdatePatch();
}

void CInstrumentFVEditor::ValidateFullPatch(UINT* pwValidationMember, UINT wOldValue, UINT uControlID)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(pwValidationMember);
	if(pwValidationMember == NULL)
	{
		return;
	}

	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return;
	}

	CDLSComponent* pComponent = m_pInstrument->m_pComponent;
	ASSERT(pComponent);
	if(pComponent == NULL)
	{
		return;
	}

	if(pComponent->IsNoShowUserPatchConflict() == true)
	{
		return;
	}

	CInstrument* pConflictIns = NULL;
	HRESULT			hr;

	hr = m_pComponent->IsValidPatch(m_pInstrument, MAKE_BANK(m_fIsDrumKit, m_wBank, m_wBank2), (ULONG) m_wPatch, m_pCollection, &pConflictIns);
	if ( SUCCEEDED(hr) )
	{
		return;
	}
	if ( (hr & SC_PATCH_DUP_GM) && (m_dwIgnorePatchConflicts & DLS_DLG_IGNORE_ALL_GM) )
	{
		hr &= ~SC_PATCH_DUP_GM;
	}
	if ( (hr & SC_PATCH_DUP_COLLECTION) && (m_dwIgnorePatchConflicts & DLS_DLG_IGNORE_ALL_COLLECTION) )
	{
		hr &= ~SC_PATCH_DUP_COLLECTION;
	}
	if ( (hr & SC_PATCH_DUP_SYNTH) && (m_dwIgnorePatchConflicts & DLS_DLG_IGNORE_ALL_SYNTH) )
	{
		hr &= ~SC_PATCH_DUP_SYNTH;
	}
	if (DLS_ERR_FIRST == hr)
	{
		return;
	}
	CWnd* pEdit;
	CUserPatchConflictDlg* pDlg = new CUserPatchConflictDlg(hr, MAKE_BANK(m_fIsDrumKit, m_wBank, m_wBank2), (ULONG) m_wPatch, pConflictIns);
	int retval = pDlg->DoModal();
	switch (retval)
	{
		case IDOK :
			{
				*pwValidationMember = wOldValue;
				UpdateData(FALSE);
				//setfocus
				pEdit = GetDlgItem(uControlID);
				if (pEdit)
				{
					pEdit->SetFocus();
					((CEdit*)pEdit)->SetSel(0,-1);
				}
				break;
			}

		case IDCANCEL:
			{
				//ignore
				break;
			}

		default:
			{
				//ignore & setflag
				m_dwIgnorePatchConflicts |= retval;
				break;
			}
	}
	delete pDlg;
}


void CInstrumentFVEditor::OnNotePlayedRegionRegionkeyboard(long nNote, BOOL bType) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pInstrument == NULL)
		return;

	// Don't play anything if the note is not in the region range
	if(FindRegionFromMap((int)nNote) == NULL)
		return;

	BYTE nMsg = MIDI_NOTEON;
	if(bType == FALSE)
		nMsg = MIDI_NOTEOFF;

	// Note Off
	if(nMsg == MIDI_NOTEOFF)
	{
		if ( m_parent->m_nMIDINoteOns[nNote] > 0 )
			m_parent->m_nMIDINoteOns[nNote]--;
	}
	else if (nMsg == MIDI_NOTEON)
	{
		if ( m_parent->m_nMIDINoteOns[nNote] > 0 )
			return;
	
		m_parent->m_nMIDINoteOns[nNote]++;
		m_pInstrument->UpdatePatch();          
	}
	m_pComponent->PlayMIDIEvent(nMsg, (BYTE)nNote, BYTE(m_nClickVelocity), 5, m_pInstrument->IsDrum());	
	m_RegionKeyBoard.MidiEvent((short)nNote, nMsg, short(m_nClickVelocity));
	
	// Refresh the property page for the region
	if(m_pCurRegion && m_pCurRegion->m_pRegionPropPgMgr)
	{
		m_pCurRegion->m_pRegionPropPgMgr->RefreshData();
	}

}
  

void CInstrumentFVEditor::OnRegionMovedRegionRegionkeyboard(short nOldLayer, short nOldStartNote, short nMovedLayer, short nMovedStartNote, short nMovedEndNote) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
		return;

	TurnOffMidiNotes();


	CRegion* pRegion = FindRegionFromMap(nOldLayer, nOldStartNote);
	// Must have the old region...if not we have a problem
	ASSERT(pRegion);
	// But let's not crash at least!
	if(pRegion == NULL)
		return;

	m_pCurRegion = pRegion;

	if(pRegion->m_rRgnHeader.RangeKey.usLow != nMovedStartNote || pRegion->GetLayer() != nMovedLayer)
	{
		m_pInstrument->SaveStateForUndo(IDS_UNDO_REGION_MOVE);
	}

	pRegion->SetLayer(nMovedLayer);
	m_dwLowerNoteRange = nMovedStartNote;
	m_dwUpperNoteRange = nMovedEndNote;
	
	pRegion->m_rRgnHeader.RangeKey.usLow = nMovedStartNote;
	pRegion->m_rRgnHeader.RangeKey.usHigh = nMovedEndNote;

	char szNote[5];
	CWnd * pDisplay = NULL;

	pDisplay = GetDlgItem(IDC_REGION_ELRANGE_TEXT);	
	if(pDisplay)
	{
		m_bTouchedByProgram = true;
		notetostring(nMovedStartNote, szNote);
		pDisplay->SetWindowText(szNote);
	}
	pDisplay = GetDlgItem(IDC_REGION_EURANGE_TEXT);	
	if(pDisplay)
	{
		m_bTouchedByProgram = true;
		notetostring(nMovedEndNote, szNote);
		pDisplay->SetWindowText(szNote);
	}

	m_bTouchedByProgram = false;
	m_ThruSpin.SetPos(nMovedEndNote);
	m_RangeSpin.SetPos(nMovedStartNote);
	
	RegionChangeCommonTasks();

	if(m_pInstrument)
		m_pComponent->m_pIFramework->SortChildNodes(&(m_pInstrument->m_Regions));

	if(m_pCurRegion && m_pCurRegion->m_pRegionPropPgMgr)
		m_pCurRegion->m_pRegionPropPgMgr->RefreshData();

}

void CInstrumentFVEditor::RegionWaveChanged()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);


	if(m_pCurRegion)
	{
		DeleteAndSetupWavesForCurrentRegion();
		m_lUnityNote = (long) m_pCurRegion->m_rWSMP.usUnityNote;
	}

	UpdateData(FALSE);
	UpdateRootNote(m_lUnityNote);
}

/* adds the given wave as an entry in the region wavelink dropdown */
void CInstrumentFVEditor::AddWaveToWaveLinkDropdown(CComboBox *pCombo, CWave *pWave)
{
	ASSERT(pWave);
	CString sWaveName = pWave->GetName();
	int index = pCombo->AddString(sWaveName);
    pCombo->SetItemDataPtr(index, pWave);
    if (pWave == m_pWave)
		pCombo->SetCurSel(index);
}

void CInstrumentFVEditor::DeleteAndSetupWavesForCurrentRegion()
{
	CComboBox* pCombo = (CComboBox *) GetDlgItem(IDC_REGION_WAVELINK);

	int nCountItem = pCombo->GetCount();

	pCombo->ResetContent();
	m_pWave = m_pCurRegion->m_pWave;
    
    if (pCombo != NULL && m_pWaves != NULL)
    {
		CPtrList waveList;
		if(FAILED(m_pWaves->GetListOfWaves(&waveList)))
			return;

		POSITION position = waveList.GetHeadPosition();
        while(position)
	        {
			CWave* pWave = (CWave*)waveList.GetNext(position);
			AddWaveToWaveLinkDropdown(pCombo, pWave);

            // if multichannel, add in entries for each channel as well
            LONG cChannels = pWave->GetChannelCount();
            if (cChannels > 1)
	            for (LONG iChannel = 0; iChannel < cChannels; iChannel++)
					AddWaveToWaveLinkDropdown(pCombo, pWave->GetChannel(iChannel));
	        }
    }

	if(pCombo->GetCurSel() == CB_ERR || pCombo->GetItemDataPtr(pCombo->GetCurSel()) == NULL)
	{
		pCombo->SetCurSel(0);
	}

	OnSelchangeRegionWavelink();
}

void CInstrumentFVEditor::OnRegionDeletedRegionkeyboard(short nLayer, short nStartNote) 
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
		return;

	TurnOffMidiNotes();

	if(m_pCurRegion)
	{
		if(m_pCurRegion->GetLayer() != nLayer && m_pCurRegion->m_rRgnHeader.RangeKey.usLow != nStartNote)
			return; // This is not the region we want to delete

		OnRegionDelete();
	}

}

void CInstrumentFVEditor::SetArticulationStaticIcon(bool bInstrumentLevel)
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_ARTICULATION_STATIC);
	
	if(pButton)
	{
		if(bInstrumentLevel)
			//pButton->SetIcon(m_hInstrumentIcon, false);
			pButton->SetWindowText("Instrument Articulation");
		else
			//pButton->SetIcon(m_hRegionIcon, false);
			pButton->SetWindowText("Region Articulation");
	}
}

LRESULT CInstrumentFVEditor::OnBringToTop(UINT wParam, LONG lParam)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	HWND hwndEditor = NULL;
	m_pInstrument->GetEditorWindow(&hwndEditor);
	if(hwndEditor)
		::BringWindowToTop(hwndEditor);

	m_RegionKeyBoard.ReleaseMouseCapture();

	return 0;
}


void CInstrumentFVEditor::TurnOffMidiNotes()
{
	if(m_parent)
		m_parent->TurnOffMidiNotes();

	if(m_pInstrument)
	{
		m_pInstrument->Stop(true);
		m_pInstrument->TurnOffMidiNotes();
	}

	if(m_RegionKeyBoard.m_hWnd != 0)
		m_RegionKeyBoard.TurnOffMidiNotes();
}

void CInstrumentFVEditor::OnKillfocusRegionElrange() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_nHasFocus = 0;

	// Stop if we're playing 
	TurnOffMidiNotes();

	char szLowRange[MAX_BUFFER];
	m_LowRangeEdit.GetWindowText(szLowRange, MAX_BUFFER);

	int nLowRange = (int)m_dwLowerNoteRange;
	if(isalpha(szLowRange[0]))
	{
		if(IsValidMidiNoteText(szLowRange))
		{
			nLowRange = stringtonote(szLowRange);
		}
	}
	else
	{
		nLowRange = atoi(szLowRange);
	}
	
	UpdateNoteRange(nLowRange, m_dwUpperNoteRange, true);
}


void CInstrumentFVEditor::OnKillfocusRegionEurange() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_nHasFocus = 0;

	// Stop if we're playing 
	TurnOffMidiNotes();

	char szHighRange[MAX_BUFFER];
	m_HighRangeEdit.GetWindowText(szHighRange, MAX_BUFFER);

	int nHighRange = (int)m_dwUpperNoteRange;
	if(isalpha(szHighRange[0]))
	{
		if(IsValidMidiNoteText(szHighRange))
		{
			nHighRange = stringtonote(szHighRange);
		}
	}
	else
	{
		nHighRange = atoi(szHighRange);
	}
	
	UpdateNoteRange(m_dwLowerNoteRange, nHighRange, false);
}


void CInstrumentFVEditor::OnDeltaposRegionRangeSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iCurPos = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(iDelta < 0)
		iDelta = -1;
	else if(iDelta > 0)
		iDelta = 1;
	else
		iDelta = 0;

	int nLowRange = iCurPos + iDelta;
	if(nLowRange >= 0 && nLowRange <= 127)
		UpdateNoteRange(nLowRange, m_dwUpperNoteRange, true);
	*pResult = 1;
}

void CInstrumentFVEditor::OnDeltaposRegionThruSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iCurPos = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(iDelta < 0)
		iDelta = -1;
	else if(iDelta > 0)
		iDelta = 1;
	else
		iDelta = 0;

	int nHighRange = iCurPos + iDelta;
	if(nHighRange >= 0 && nHighRange <= 127)
		UpdateNoteRange(m_dwLowerNoteRange, nHighRange, false);
	*pResult = 1;
}

void CInstrumentFVEditor::UpdateNoteRange(DWORD dwLowRange, DWORD dwHighRange, bool bSettingLowRange)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pCurRegion);
	if(m_pCurRegion == NULL)
		return;

	// Validate both ranges
	// Always false
	//if(dwLowRange < 0)
	//	dwLowRange = 0;
	if(dwLowRange > 127)
		dwLowRange = 127;

	// Always false
	//if(dwHighRange < 0)
	//	dwHighRange = 0;
	if(dwHighRange > 127)
		dwHighRange = 127;

	DWORD dwCheckRange = dwHighRange;
	if(bSettingLowRange)
		dwCheckRange = dwLowRange;
	CRegion* pRegion = FindRegionFromMap(dwCheckRange);
	if(pRegion == NULL)
	{
		if(dwLowRange > dwHighRange || dwHighRange < dwLowRange)
		{
			dwHighRange = dwCheckRange;
			dwLowRange = dwCheckRange;
		}
	}

	BOOL bSuccess = m_RegionKeyBoard.SetRange((short)dwLowRange, (short)dwHighRange, (short)m_dwLowerVelocityRange, (short)m_dwUpperVelocityRange);
	if(bSuccess)
	{
		// Save the undo state if the values are different
		if(!m_fInOnInitialUpdate && (dwLowRange != m_dwLowerNoteRange || dwHighRange != m_dwUpperNoteRange))
		{
			if(FAILED(m_pInstrument->SaveStateForUndo(IDS_RANGE_UNDO_TEXT)))
			{
				// Out Of Memory??
				m_RegionKeyBoard.SetRange((short)m_dwLowerNoteRange, (short)m_dwUpperNoteRange, (short)m_dwLowerVelocityRange, (short)m_dwUpperVelocityRange);
				
				// Set the edit text
				char szWindowText[MAX_BUFFER];
				notetostring(m_dwLowerNoteRange, szWindowText);
				m_LowRangeEdit.SetWindowText(szWindowText);
				notetostring(m_dwUpperNoteRange, szWindowText);
				m_HighRangeEdit.SetWindowText(szWindowText);
				return;
			}
		}

		m_dwLowerNoteRange = dwLowRange;
		m_dwUpperNoteRange = dwHighRange;

		m_pCurRegion->m_rRgnHeader.RangeKey.usLow = (USHORT)dwLowRange;
		m_pCurRegion->m_rRgnHeader.RangeKey.usHigh = (USHORT)dwHighRange;

		// Set the edit text
		char szWindowText[MAX_BUFFER];
		notetostring(dwLowRange, szWindowText);
		m_LowRangeEdit.SetWindowText(szWindowText);
		notetostring(dwHighRange, szWindowText);
		m_HighRangeEdit.SetWindowText(szWindowText);

		// Update spin control positions
		m_RangeSpin.SetPos(dwLowRange);
		m_ThruSpin.SetPos(dwHighRange);
		
		if(!m_fInOnInitialUpdate)
			RegionChangeCommonTasks();
	}
			
	m_nStartNoteForCurRegion = m_pCurRegion->m_rRgnHeader.RangeKey.usLow;

	if(m_pCurRegion->m_pRegionPropPgMgr && !m_fInOnInitialUpdate)
		m_pCurRegion->m_pRegionPropPgMgr->RefreshData();
}

void CInstrumentFVEditor::OnKillfocusRegionRootNote() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_nHasFocus = 0;

	// Stop if we're playing 
	TurnOffMidiNotes();

	char szRootNote[MAX_BUFFER];
	m_RootNoteEdit.GetWindowText(szRootNote, MAX_BUFFER);
	
	int nRootNote = m_lUnityNote;
	if(isalpha(szRootNote[0]))
	{
		if(IsValidMidiNoteText(szRootNote))
		{
			nRootNote = stringtonote(szRootNote);
		}
	}
	else
	{
		nRootNote = atoi(szRootNote);
	}

	UpdateRootNote(nRootNote);
}

void CInstrumentFVEditor::OnDeltaposRegionRootNoteSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	int iCurPos = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	int nRootNote = iCurPos + iDelta;
	UpdateRootNote(nRootNote);
	
	*pResult = 1;
}

// Sets the root note for both the root note edit and the spin control
void CInstrumentFVEditor::UpdateRootNote(int nRootNote)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pCurRegion);
	if(m_pCurRegion == NULL)
		return;

	if(nRootNote < 0)
		nRootNote = 0;
	else if(nRootNote > 127)
		nRootNote = 127;

	if(m_lUnityNote != nRootNote)
	{
		// Save the undo state
		if(!m_fInOnInitialUpdate)
		{
			if(FAILED(m_pInstrument->SaveStateForUndo(IDS_ROOT_UNDO_TEXT)))
			{
				char szRootNote[MAX_BUFFER];
				notetostring(m_lUnityNote, szRootNote);
				m_RootNoteEdit.SetWindowText(szRootNote);
				return;
			}
		}
	}

	m_lUnityNote = nRootNote;
	
	m_RootNoteSpin.SetPos(nRootNote);
	char szRootNote[MAX_BUFFER];
	notetostring(nRootNote, szRootNote);
	m_RootNoteEdit.SetWindowText(szRootNote);

	m_pCurRegion->m_rWSMP.usUnityNote = (USHORT)m_lUnityNote;

	SendRegionChange();
	m_pInstrument->m_nMIDINote = CalculateAuditionNote();
	m_RegionKeyBoard.SetRootNote((short)m_lUnityNote);
	
	// Check for the override root note flag 
	if (m_pCurRegion->m_rWSMP.usUnityNote != m_pCurRegion->m_pWave->GetUnityNote()) 
		m_pCurRegion->m_bOverrideWaveRootNote = true;

	m_pCollection->SetDirtyFlag();
	m_pInstrument->UpdateInstrument();

	if(m_pCurRegion->m_pRegionPropPgMgr && m_pCurRegion->m_pRegionPropPgMgr->m_pRegionPage)
    {
		if(!m_fInOnInitialUpdate)
			m_pCurRegion->m_pRegionPropPgMgr->SetObject(m_pCurRegion);

		CRegionPropPg * pPropPg = m_pCurRegion->m_pRegionPropPgMgr->m_pRegionPage;
        if (pPropPg->GetSafeHwnd())
        {
            // Check the override checkbox. 
			pPropPg->m_fActivateRootNote = m_pCurRegion->m_bOverrideWaveRootNote;
            if (m_pCurRegion->m_rWSMP.usUnityNote != m_pCurRegion->m_pWave->GetUnityNote()) 
			{
				CButton* pCheckBox = (CButton *) pPropPg->GetDlgItem(IDC_OWS_ACTIVE_ROOT_NOTE);
				if (pCheckBox)
				{
					pCheckBox->SetCheck(m_pCurRegion->m_bOverrideWaveRootNote);
				}
            }
			
            // Enable the rootnote box.
			if(m_pCurRegion->m_bOverrideWaveRootNote)
				pPropPg->EnableRootNote(true);
			else
				pPropPg->EnableRootNote(false);

            pPropPg->SetRootNote((BYTE)m_lUnityNote);
        }
	}
}

void CInstrumentFVEditor::OnSetfocusRegionElrange() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_nHasFocus = IDC_REGION_ELRANGE;
	m_LowRangeEdit.SetSel(0, -1);
}

void CInstrumentFVEditor::OnSetfocusRegionEurange() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_nHasFocus = IDC_REGION_EURANGE;
	m_HighRangeEdit.SetSel(0, -1);
}

void CInstrumentFVEditor::OnSetfocusRegionRootNote() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_nHasFocus = IDC_REGION_ROOT_NOTE;
	m_RootNoteEdit.SetSel(0, -1);
}

BOOL CInstrumentFVEditor::SendKeyToChildControl(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWnd* pWnd = GetDlgItem(IDC_DRUMS);
	if(nChar == VK_SPACE && GetFocus() == pWnd)
	{
		((CButton*)pWnd)->SetCheck(!m_fIsDrumKit);
		OnDrums();
		return TRUE;
	}

	pWnd = GetDlgItem(IDC_REGION_OVERLAP);
	if(nChar == VK_SPACE && GetFocus() == pWnd)
	{
		((CButton*)pWnd)->SetCheck(!m_fAllowOverlap);
		OnRegionOverlap();
		return TRUE;
	}

	pWnd = GetDlgItem(IDC_REGION_UIA);
	if(nChar == VK_SPACE && GetFocus() == pWnd)
	{
		((CButton*)pWnd)->SetCheck(!m_fUseInstArt);
		OnRegionUia();
		return TRUE;
	}

	return FALSE;
}

void CInstrumentFVEditor::OnDraw(CDC* pDC) 
{
	CFormView::OnDraw(pDC);

	CPen blackPen(PS_SOLID, 0, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&blackPen);

	POSITION position = m_lstStatics.GetHeadPosition();
	while(position)
	{
		CDLSStatic* pStatic = (CDLSStatic*) m_lstStatics.GetNext(position);
		ASSERT(pStatic);
		pStatic->OnDraw(pDC);
	}

	if(pOldPen)
		pDC->SelectObject(pOldPen);
}

CDLSStatic* CInstrumentFVEditor::GetStaticControl(UINT nID)
{
	POSITION position = m_lstStatics.GetHeadPosition();
	while(position)
	{
		CDLSStatic* pStatic = (CDLSStatic*) m_lstStatics.GetNext(position);
		ASSERT(pStatic);
		if(pStatic->GetID() == nID)
			return pStatic;
	}

	return NULL;
}

void CInstrumentFVEditor::OnPitchButton() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Are we already showing the Pitch dialog?
	if(m_pArtDialog && m_nCurrentArticulationDlg == IDD_PITCH_PAGE)
		return;

	// Kill the old dialog
	if(m_pArtDialog)
	{
		m_pArtDialog->DestroyWindow();
		delete m_pArtDialog;
		m_pArtDialog = NULL;
	}

	// Create the Pitch Envelope dialog and show it
	CPitchDialog* pPitchDialog = new CPitchDialog(m_pCurArticulation);
	pPitchDialog->Create(CPitchDialog::IDD, this);
	pPitchDialog->UpdateArticulation(m_pCurArticulation);
	
	CRect rcBounds;
	if(SUCCEEDED(GetArticulationBounds(rcBounds)))
		pPitchDialog->MoveWindow(&rcBounds);
	pPitchDialog->BringWindowToTop();
	pPitchDialog->ShowWindow(SW_SHOW);

	m_pArtDialog = (CDialog*) pPitchDialog;
	m_nCurrentArticulationDlg = IDD_PITCH_PAGE;

	m_PitchButton.SetCheck(1);
	m_VolumeButton.SetCheck(0);
	m_LFOButton.SetCheck(0);
	m_VibLFOButton.SetCheck(0);
	m_FilterButton.SetCheck(0);
}

void CInstrumentFVEditor::OnLfoButton() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Are we already showing the LFO dialog?
	if(m_pArtDialog && m_nCurrentArticulationDlg == IDD_LFO_PAGE)
		return;

	// Kill the old dialog
	if(m_pArtDialog)
	{
		m_pArtDialog->DestroyWindow();
		delete m_pArtDialog;
		m_pArtDialog = NULL;
	}

	// Create the Pitch Envelope dialog and show it
	CLFODialog* pLFODialog = new CLFODialog(m_pCurArticulation);
	pLFODialog->Create(CLFODialog::IDD, this);
	pLFODialog->UpdateArticulation(m_pCurArticulation);
	
	CRect rcBounds;
	if(SUCCEEDED(GetArticulationBounds(rcBounds)))
	{
		pLFODialog->MoveWindow(&rcBounds);
	}

	pLFODialog->BringWindowToTop();
	pLFODialog->ShowWindow(SW_SHOW);

	m_pArtDialog = (CDialog*) pLFODialog;
	m_nCurrentArticulationDlg = IDD_LFO_PAGE;
	
	m_LFOButton.SetCheck(1);
	m_VibLFOButton.SetCheck(0);
	m_PitchButton.SetCheck(0);
	m_VolumeButton.SetCheck(0);
	m_FilterButton.SetCheck(0);
}

void CInstrumentFVEditor::OnViblfoButton() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Are we already showing the LFO dialog?
	if(m_pArtDialog && m_nCurrentArticulationDlg == IDD_VIBLFO_PAGE)
		return;

	// Kill the old dialog
	if(m_pArtDialog)
	{
		m_pArtDialog->DestroyWindow();
		delete m_pArtDialog;
		m_pArtDialog = NULL;
	}

	// Create the Pitch Envelope dialog and show it
	CVibratoLFODialog* pVibratoLFODialog = new CVibratoLFODialog(m_pCurArticulation);
	pVibratoLFODialog->Create(CVibratoLFODialog::IDD, this);
	pVibratoLFODialog->UpdateArticulation(m_pCurArticulation);
	
	CRect rcBounds;
	if(SUCCEEDED(GetArticulationBounds(rcBounds)))
	{
		pVibratoLFODialog->MoveWindow(&rcBounds);
	}

	pVibratoLFODialog->BringWindowToTop();
	pVibratoLFODialog->ShowWindow(SW_SHOW);

	m_pArtDialog = (CDialog*) pVibratoLFODialog;
	m_nCurrentArticulationDlg = IDD_VIBLFO_PAGE;
	
	m_VibLFOButton.SetCheck(1);
	m_LFOButton.SetCheck(0);
	m_PitchButton.SetCheck(0);
	m_VolumeButton.SetCheck(0);
	m_FilterButton.SetCheck(0);
}

void CInstrumentFVEditor::OnVolumeButton() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Do we have a valid articulation
	if(m_pCurArticulation == NULL)
		return;

	// Are we already showing the Volume envelope?
	if(m_pArtDialog && m_nCurrentArticulationDlg == IDD_VOLUME_PAGE)
		return;

		// Kill the old dialog
	if(m_pArtDialog)
	{
		m_pArtDialog->DestroyWindow();
		delete m_pArtDialog;
		m_pArtDialog = NULL;
	}

	// Create the Pitch Envelope dialog and show it
	CVolDialog* pVolumeDialog = new CVolDialog(m_pCurArticulation);
	pVolumeDialog->Create(CVolDialog::IDD, this);
	pVolumeDialog->UpdateArticulation(m_pCurArticulation);
	
	CRect rcBounds;
	if(SUCCEEDED(GetArticulationBounds(rcBounds)))
		pVolumeDialog->MoveWindow(&rcBounds);

	pVolumeDialog->BringWindowToTop();
	pVolumeDialog->ShowWindow(SW_SHOW);

	m_pArtDialog = (CDialog*) pVolumeDialog;
	m_nCurrentArticulationDlg = IDD_VOLUME_PAGE;

	m_VolumeButton.SetCheck(1);
	m_PitchButton.SetCheck(0);
	m_LFOButton.SetCheck(0);
	m_VibLFOButton.SetCheck(0);
	m_FilterButton.SetCheck(0);
}


HRESULT CInstrumentFVEditor::GetArticulationBounds(CRect& rcBounds)
{
	CWnd* pArtBounds = GetDlgItem(IDC_ARTICULATION_STATIC);
	ASSERT(pArtBounds);
	if(pArtBounds == NULL)
		return E_FAIL;

	CRect rcEditor;
	GetWindowRect(&rcEditor);
	
	CRect rcArtBoundsScreenRect;
	pArtBounds->GetWindowRect(&rcArtBoundsScreenRect);

	pArtBounds->GetClientRect(&rcBounds);

	rcBounds.left = 10 + rcArtBoundsScreenRect.left - rcEditor.left;
	rcBounds.right = rcArtBoundsScreenRect.Width() - 0;
	rcBounds.top = 50 + rcArtBoundsScreenRect.top - rcEditor.top;
	rcBounds.bottom += rcBounds.top;

	return S_OK;
}

HRESULT CInstrumentFVEditor::GetControlPosition(UINT nID, CRect& rcPosition)
{
	CWnd* pWnd = GetDlgItem(nID);
	ASSERT(pWnd);
	if(pWnd == NULL)
		return E_FAIL;

	CRect rcEditor;
	GetWindowRect(&rcEditor);

	pWnd->GetWindowRect(&rcPosition);

	CRect rcClient;
	pWnd->GetClientRect(&rcClient);
	
	rcPosition.left -= rcEditor.left; 
	rcPosition.right = rcPosition.left + rcClient.Width();
	rcPosition.top -= rcEditor.top;
	rcPosition.bottom = rcPosition.top + rcClient.Height();

	return S_OK;
}


void CInstrumentFVEditor::OnKillfocusRegionVelocityElrange() 
{
	CString sVelocity;
	m_VelocityLowRangeEdit.GetWindowText(sVelocity);
	int nVelocity = atoi(sVelocity);
	UpdateVelocityRange(nVelocity, m_dwUpperVelocityRange, true);
}


void CInstrumentFVEditor::OnKillfocusRegionVelocityEurange() 
{
	CString sVelocity;
	m_VelocityHighRangeEdit.GetWindowText(sVelocity);
	int nVelocity = atoi(sVelocity);
	UpdateVelocityRange(m_dwLowerVelocityRange, nVelocity, false);
}


void CInstrumentFVEditor::OnKillfocusClickVelocity() 
{
	CString sVelocity;
	m_ClickVelocityEdit.GetWindowText(sVelocity);
	int nVelocity = atoi((LPCSTR)sVelocity);

	if(nVelocity < 0 || nVelocity > 127)
	{
		nVelocity = nVelocity < 0 ? 0 : nVelocity;
		nVelocity = nVelocity > 127 ? 127 : nVelocity;

		sVelocity.Format("%d", m_nClickVelocity);
		m_ClickVelocityEdit.SetWindowText(sVelocity);
	}

	m_nClickVelocity = nVelocity;
	m_ClickVelocitySpin.SetPos(nVelocity);
}


void CInstrumentFVEditor::UpdateVelocityRange(DWORD dwLowRange, DWORD dwHighRange, bool bSettingLowRange)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pCurRegion);
	if(m_pCurRegion == NULL)
		return;

	// Validate both ranges
	// Always false
	//if(dwLowRange < 0)
	//	dwLowRange = 0;
	if(dwLowRange > 127)
		dwLowRange = 127;

	// Always false
	//if(dwHighRange < 0)
	//	dwHighRange = 0;
	if(dwHighRange > 127)
		dwHighRange = 127;

	DWORD dwTemp = dwHighRange;
	if(bSettingLowRange)
		dwTemp = dwLowRange;

	if(dwLowRange > dwHighRange)
	{
		dwHighRange = dwTemp;
		dwLowRange = dwTemp;
	}

	BOOL bSuccess = m_RegionKeyBoard.SetRange((short)m_dwLowerNoteRange, (short)m_dwUpperNoteRange, (short)dwLowRange, (short)dwHighRange);
	if(bSuccess)
	{
		// Save the undo state if the values are different
		if(!m_fInOnInitialUpdate && (dwLowRange != m_dwLowerVelocityRange || dwHighRange != m_dwUpperVelocityRange))
		{
			if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_VELOCITY_RANGE_EDIT)))
			{
				// Out Of Memory??
				m_RegionKeyBoard.SetRange((short)m_dwLowerNoteRange, (short)m_dwUpperNoteRange, (short)dwLowRange, (short)dwHighRange);
				
				CString sText;
				sText.Format("%d", m_dwLowerVelocityRange);
				m_VelocityLowRangeEdit.SetWindowText(sText);
				sText.Format("%d", m_dwUpperVelocityRange);
				m_VelocityHighRangeEdit.SetWindowText(sText);
				return;
			}
		}

		m_dwLowerVelocityRange = dwLowRange;
		m_dwUpperVelocityRange = dwHighRange;

		m_pCurRegion->m_rRgnHeader.RangeVelocity.usLow = (USHORT)dwLowRange;
		m_pCurRegion->m_rRgnHeader.RangeVelocity.usHigh = (USHORT)dwHighRange;

		CString sText;
		sText.Format("%d", m_dwLowerVelocityRange);
		m_VelocityLowRangeEdit.SetWindowText(sText);
		sText.Format("%d", m_dwUpperVelocityRange);
		m_VelocityHighRangeEdit.SetWindowText(sText);

		// Update spin control positions
		m_VelocityLowRangeSpin.SetPos(dwLowRange);
		m_VelocityHighRangeSpin.SetPos(dwHighRange);
		
		if(!m_fInOnInitialUpdate)
			RegionChangeCommonTasks();
	}
}

void CInstrumentFVEditor::OnDeltaposRegionVelocityRangeSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iCurPos = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(iDelta < 0)
		iDelta = -1;
	else if(iDelta > 0)
		iDelta = 1;
	else
		iDelta = 0;

	int nLowRange = iCurPos + iDelta;
	if(nLowRange >= 0 && nLowRange <= 127)
		UpdateVelocityRange(nLowRange, m_dwUpperVelocityRange, true);
	
	*pResult = 1;
}


void CInstrumentFVEditor::OnDeltaposRegionVelocityThruSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	int iCurPos = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(iDelta < 0)
		iDelta = -1;
	else if(iDelta > 0)
		iDelta = 1;
	else
		iDelta = 0;

	int nHighRange = iCurPos + iDelta;
	if(nHighRange >= 0 && nHighRange <= 127)
		UpdateVelocityRange(m_dwLowerVelocityRange, nHighRange, false);
	
	*pResult = 1;
}


void CInstrumentFVEditor::OnScrollLayersRegionRegionkeyboard(BOOL bUp) 
{
	short nCurPos = short(m_LayerScrollBar.GetScrollPos());
	short nSBCode = bUp == TRUE ? SB_LINEUP : SB_LINEDOWN;
	int nFirstVisibleLayer = m_RegionKeyBoard.ScrollLayers(nSBCode, nCurPos);
	SetLayerScrollInfo(nFirstVisibleLayer);
}


void CInstrumentFVEditor::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(pScrollBar == &m_LayerScrollBar)
	{
		int nCurPos = nPos;
		int nFirstVisibleLayer = m_RegionKeyBoard.ScrollLayers(short(nSBCode), short(nPos));
		SetLayerScrollInfo(nFirstVisibleLayer);
		return;
	}

	
	CFormView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CInstrumentFVEditor::OnDeltaposClickVelocitySpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int iCurPos = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(iDelta < 0)
		iDelta = -1;
	else if(iDelta > 0)
		iDelta = 1;
	else
		iDelta = 0;

	m_nClickVelocity = iCurPos + iDelta;
	
	*pResult = 0;
}

CRegion* CInstrumentFVEditor::FindConflictingDLS1Region(USHORT usStartNote, USHORT usEndNote)
{
	// Check if the current region overlaps in range with any region
	CRegion* pRegion = m_pInstrument->m_Regions.GetHead();
	while(pRegion)
	{
		CRegion* pConflictingRegion = pRegion->IsOverlappingRegion(usStartNote, usEndNote);
		if(pConflictingRegion)
		{
			return pConflictingRegion;
		}

		pRegion = pRegion->GetNext();
	}

	return NULL;
}

USHORT CInstrumentFVEditor::GetActiveLayer()
{
	return m_usActiveLayer;
}

void CInstrumentFVEditor::OnActiveLayerChangedRegionRegionkeyboard(short nLayer) 
{
	m_usActiveLayer = nLayer;
	ASSERT(m_pInstrument);
	if(m_pInstrument)
	{
		m_pInstrument->UpdateInstrument();
	}
}

void CInstrumentFVEditor::SetLayerScrollInfo(int nFirstVisibleLayer)
{
	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	short nLayers = m_RegionKeyBoard.GetNumberOfLayers();
	short nFirstLayerShown = m_RegionKeyBoard.GetFirstVisibleLayer();
	
	if(nFirstVisibleLayer < nFirstLayerShown)
	{
		m_RegionKeyBoard.SetFirstVisibleLayer(short(nFirstVisibleLayer));
	}
	else if(nFirstVisibleLayer >= nFirstLayerShown + 4)
	{
		if(nFirstVisibleLayer + 4 > nLayers)
		{
			nFirstVisibleLayer = nLayers - 4;
		}
		else
		{
			nFirstVisibleLayer = nFirstVisibleLayer - 4;
		}
		
		m_RegionKeyBoard.SetFirstVisibleLayer(short(nFirstVisibleLayer));
	}
	else
	{
		nFirstVisibleLayer = nFirstLayerShown;
	}

	if(nLayers > 4)
	{
		scrollInfo.nMax = nLayers;
		scrollInfo.nMin = 0;
		scrollInfo.nPage = 4;
		scrollInfo.nPos = 0;
		scrollInfo.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		m_LayerScrollBar.SetScrollInfo(&scrollInfo);
		if(nFirstVisibleLayer == 0)
		{
			m_LayerScrollBar.SetScrollPos(nLayers);
		}
		else
		{
			m_LayerScrollBar.SetScrollPos(nLayers - (nFirstVisibleLayer + 4));
		}
	}
	else
	{
		m_LayerScrollBar.EnableScrollBar(ESB_DISABLE_BOTH);
	}

	// Enable/Disable "DLS1" static control based on the first visible layer
	CClientDC clientDC(this);
	CDLSStatic* pDLS1Static = GetStaticControl(IDC_DLS1_STATIC);
	if(pDLS1Static)
	{
		if(nFirstVisibleLayer != 0)
		{
			CString sLayerText;
			sLayerText.Format("%d", nFirstVisibleLayer);
			pDLS1Static->SetText(sLayerText);
		}
		else
		{
			pDLS1Static->SetText("DLS1");
		}
		
		pDLS1Static->Invalidate(&clientDC);
	}

	// Change the values on the layer statics
	CString sLayerText;
	sLayerText.Format("%d", nFirstVisibleLayer + 1);
	CDLSStatic* pStatic = GetStaticControl(IDC_STATIC_LAYER1);
	if(pStatic)
	{
		pStatic->SetText(sLayerText);
		pStatic->Invalidate(&clientDC);
	}

	sLayerText.Format("%d", nFirstVisibleLayer + 2);
	pStatic = GetStaticControl(IDC_STATIC_LAYER2);
	if(pStatic)
	{
		pStatic->SetText(sLayerText);
		pStatic->Invalidate(&clientDC);
	}

	sLayerText.Format("%d", nFirstVisibleLayer + 3);
	pStatic = GetStaticControl(IDC_STATIC_LAYER3);
	if(pStatic)
	{
		pStatic->SetText(sLayerText);
		pStatic->Invalidate(&clientDC);
	}

	//Invalidate();
}

void CInstrumentFVEditor::OnConditionEditButton() 
{
	ASSERT(m_pCurRegion);
	if(m_pCurRegion == NULL)
		return;

	if(m_pCurRegion->GetConditionalChunk() == NULL)
		return;

	CConditionEditor conditionEditor(this, m_pComponent, m_pCurRegion->m_pConditionalChunk, m_pInstrument);
	if(conditionEditor.DoModal() == IDOK)
	{
		// Add the new condition to the presets list
		InitRegionConditionsCombo(m_pCurRegion);

		// Check what the config is and decide on download
		m_pCurRegion->CheckConfigAndRefreshNode();
	}
}


void CInstrumentFVEditor::InitRegionConditionsCombo(CRegion* pRegion)
{
	if(m_pInstrument == NULL)
		return;

	ASSERT(pRegion);
	if(pRegion == NULL)
		return;

	m_RegionConditionCombo.ResetContent();

	CString sNone;
	sNone.LoadString(IDS_NONE);
	AddStringToCombo(m_RegionConditionCombo, sNone);

	CPtrList lstConditions;
	if(SUCCEEDED(m_pCollection->GetListOfConditions(&lstConditions)))
	{
		POSITION position = lstConditions.GetHeadPosition();
		while(position)
		{
			CConditionalChunk* pConditionalChunk = (CConditionalChunk*) lstConditions.GetNext(position);
			ASSERT(pConditionalChunk);
			if(pConditionalChunk)
			{
				AddStringToCombo(m_RegionConditionCombo, pConditionalChunk->GetName());
			}
		}
	}


	// Now look for the regions condition
	CConditionalChunk* pConditionalChunk = pRegion->GetConditionalChunk();
	ASSERT(pConditionalChunk);
	if(pConditionalChunk)
	{
		CString sConditionName = pConditionalChunk->GetName();
		m_RegionConditionCombo.SelectString(0, sConditionName);
	}
}


void CInstrumentFVEditor::AddStringToCombo(CComboBox& combo, CString& sString)
{
	if(combo.FindStringExact(0, sString) == CB_ERR)
	{
		combo.AddString(sString);
	}
}


void CInstrumentFVEditor::OnSelchangeRegionConditionCombo() 
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
		return;

	ASSERT(m_pCurRegion);
	if(m_pCurRegion == NULL)
		return;

	CString sCondition;
	m_RegionConditionCombo.GetWindowText(sCondition);

	if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_CONDITION)))
	{
		return;
	}

	m_pCurRegion->SetCondition(sCondition);
	m_pCurRegion->CheckConfigAndRefreshNode();

	m_pInstrument->UpdateInstrument();
}

void CInstrumentFVEditor::OnDropdownRegionConditionCombo() 
{
	if(m_pCurRegion)
	{
		InitRegionConditionsCombo(m_pCurRegion);
	}
}

void CInstrumentFVEditor::OnArtDls1Check()
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
		return;

	ASSERT(m_pCurArticulation);
	if(m_pCurArticulation == NULL)
		return;

	int nDLS1Check = m_ArtDLS1Check.GetCheck();
	if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_DLS1_CHECK)))
		return;

	m_pCurArticulation->SetDLS1((BOOL)nDLS1Check);
	UpdateArticulationControls();

    m_pInstrument->UpdateInstrument();
}

void CInstrumentFVEditor::OnFilterButton() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Are we already showing the LFO dialog?
	if(m_pArtDialog && m_nCurrentArticulationDlg == IDD_FILTER_PAGE)
	{
		return;
	}

	// Kill the old dialog
	if(m_pArtDialog)
	{
		m_pArtDialog->DestroyWindow();
		delete m_pArtDialog;
		m_pArtDialog = NULL;
	}

	// Create the Pitch Envelope dialog and show it
	CFilterDialog* pFilterDialog = new CFilterDialog(m_pCurArticulation);
	pFilterDialog->Create(CFilterDialog::IDD, this);
	pFilterDialog->UpdateArticulation(m_pCurArticulation);
	
	CRect rcBounds;
	if(SUCCEEDED(GetArticulationBounds(rcBounds)))
	{
		pFilterDialog->MoveWindow(&rcBounds);
	}

	pFilterDialog->BringWindowToTop();
	pFilterDialog->ShowWindow(SW_SHOW);

	m_pArtDialog = (CDialog*) pFilterDialog;
	m_nCurrentArticulationDlg = IDD_FILTER_PAGE;
	
	m_FilterButton.SetCheck(1);
	m_VibLFOButton.SetCheck(0);
	m_LFOButton.SetCheck(0);
	m_PitchButton.SetCheck(0);
	m_VolumeButton.SetCheck(0);
}

void CInstrumentFVEditor::OnInstEdRegionNewLayer() 
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return;
	}

	if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_LAYER_INSERT)))
	{
		return;
	}

	UINT nOldNumberOfLayers = m_pInstrument->m_Regions.GetNumberOfLayers();

	// Go trhough the regions and bump the regions 
	// in the active layer and above up by one layer
	CRegion* pRegion = m_pInstrument->m_Regions.GetHead();
	while(pRegion)
	{
		USHORT usLayer = USHORT(pRegion->GetLayer());
		if(usLayer >= m_usActiveLayer)
		{
			pRegion->SetLayer(usLayer + 1);
		}

		pRegion = pRegion->GetNext();
	}

	m_pInstrument->m_Regions.SetNumberOfLayers(nOldNumberOfLayers + 1);
	m_RegionKeyBoard.AddNewLayer();
	
	int nFirstVisibleLayer = m_RegionKeyBoard.GetFirstVisibleLayer();

	SetLayerScrollInfo(nFirstVisibleLayer);
}

void CInstrumentFVEditor::OnInstEdLayerDelete() 
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return;
	}

	int nLayers = m_RegionKeyBoard.GetNumberOfLayers();
	if(nLayers <= 4)
	{
		AfxMessageBox(IDS_ERR_DELETE_DEFAULT_LAYER);
		return;
	}

	BOOL bLayerIsEmpty = m_pInstrument->m_Regions.IsLayerEmpty(m_usActiveLayer);
	if(bLayerIsEmpty)
	{
		if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_LAYER_DELETE)))
		{
			return;
		}

		m_pInstrument->m_Regions.SetNumberOfLayers(nLayers - 1);

		// Go trhough the regions and bump the regions 
		// in the active layer and above up by one layer
		CRegion* pRegion = m_pInstrument->m_Regions.GetHead();
		while(pRegion)
		{
			USHORT usLayer = USHORT(pRegion->GetLayer());
			if(usLayer >= m_usActiveLayer)
			{
				pRegion->SetLayer(usLayer - 1);
			}

			pRegion = pRegion->GetNext();
		}

		int nLayers = m_RegionKeyBoard.DeleteActiveLayer();
		if(m_usActiveLayer >= nLayers)
		{
			m_usActiveLayer = nLayers - 1;
			m_usActiveLayer = m_usActiveLayer <= 3 ? 0 : m_usActiveLayer;
		}

		m_RegionKeyBoard.SetCurrentLayer(m_usActiveLayer);
	
		// Figure out which is the first visible layer
		short nFirstLayerShown = m_RegionKeyBoard.GetFirstVisibleLayer();

		if(nLayers < nFirstLayerShown + 4)
		{
			SetLayerScrollInfo(nLayers - 4);
		}
		else  if(m_usActiveLayer > nFirstLayerShown + 4)
		{
			SetLayerScrollInfo(m_usActiveLayer - 4);
		}
	}
	else
	{
		AfxMessageBox(IDS_ERR_DELETE_NONEMPTY_LAYER);
	}
}

void CInstrumentFVEditor::OnRadioMultipleLayers() 
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return;
	}

	m_pInstrument->SetAuditionMode(AUDITION_MULTIPLE);
	m_RegionKeyBoard.SetAuditionMode(AUDITION_MULTIPLE);
	SetAuditionRadioMode();
}

void CInstrumentFVEditor::OnRadioSoloLayer() 
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return;
	}

	m_pInstrument->SetAuditionMode(AUDITION_SOLO);
	m_RegionKeyBoard.SetAuditionMode(AUDITION_SOLO);
	SetAuditionRadioMode();
}

void CInstrumentFVEditor::SetAuditionRadioMode()
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return;
	}

	bool bAuditionMode = m_pInstrument->GetAuditionMode();

	bool bSolo = bAuditionMode == AUDITION_SOLO == true ? true : false;
	bool bMultiple = bAuditionMode == AUDITION_MULTIPLE == true ? true : false;

	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_MULTIPLE_LAYERS);
	if(pButton)
	{
		pButton->SetCheck(bMultiple);
	}
	
	pButton = (CButton*)GetDlgItem(IDC_RADIO_SOLO_LAYER);
	if(pButton)
	{
		pButton->SetCheck(bSolo);
	}

}

void CInstrumentFVEditor::OnCopyRegionRegionkeyboard(short nSourceLayer, short nSourceStartNote, short nCopyLayer, short nCopyStartNote) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return;
	}

	if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_REGION_INSERT)))
	{
		return;
	}

	CRegion* pCopiedRegion;
	if(FAILED(m_pInstrument->m_Regions.CopyRegion(nSourceLayer, nSourceStartNote, nCopyLayer, nCopyStartNote, &pCopiedRegion)))
	{
		m_RegionKeyBoard.DeleteRegion(nCopyLayer, nCopyStartNote);
		return;
	}

	ASSERT(pCopiedRegion);
	if(pCopiedRegion)
	{
		m_RegionKeyBoard.SetRange(pCopiedRegion->m_rRgnHeader.RangeKey.usLow, pCopiedRegion->m_rRgnHeader.RangeKey.usHigh, pCopiedRegion->m_rRgnHeader.RangeVelocity.usLow, pCopiedRegion->m_rRgnHeader.RangeVelocity.usHigh);
		SetCurRegion(pCopiedRegion);
		SendRegionChange();
	}
}

void CInstrumentFVEditor::OnWaveEditButton() 
{
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return;
	}

	IDMUSProdFramework* pIFramework = m_pComponent->m_pIFramework;
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return;
	}

	if(m_pCurRegion == NULL)
	{
		return;
	}

	CWaveNode* pWaveNode = m_pCurRegion->GetWaveNode();
	ASSERT(pWaveNode);
	if(pWaveNode == NULL)
	{
		return;
	}

	pIFramework->OpenEditor(pWaveNode);
}


CWnd* CInstrumentFVEditor::GetRegionKeyboardWnd()
{
	return &m_RegionKeyBoard;
}

/* refreshes the property page for the current region, if selected and displayed */
void CInstrumentFVEditor::RefreshCurrentRegionPropertyPage()
{
	if(m_pCurRegion && m_pCurRegion->m_pRegionPropPgMgr)
	{
		IDMUSProdPropSheet* pIPropSheet = NULL;
		m_pCurRegion->m_pRegionPropPgMgr->RefreshData();
	
		if(SUCCEEDED(m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK )
				m_pCurRegion->OnShowProperties();

			RELEASE(pIPropSheet);
		}
	}
}

/* updates both shared and articulation dialog-specific controls to match m_pCurArticulation */
void CInstrumentFVEditor::UpdateArticulationControls()
{
	ASSERT(m_pCurArticulation);
	if (m_pCurArticulation == NULL)
		return;
	
	if (m_pArtDialog == NULL)
		return;

	// dialog-specific controls
	if(m_nCurrentArticulationDlg == IDD_VOLUME_PAGE)
	{
		((CVolDialog*)m_pArtDialog)->UpdateArticulation(m_pCurArticulation);
	}
	else if(m_nCurrentArticulationDlg == IDD_PITCH_PAGE)
	{
		((CPitchDialog*)m_pArtDialog)->UpdateArticulation(m_pCurArticulation);
	}
	else if(m_nCurrentArticulationDlg == IDD_LFO_PAGE)
	{
		((CLFODialog*)m_pArtDialog)->UpdateArticulation(m_pCurArticulation);
	}
	else if(m_nCurrentArticulationDlg == IDD_VIBLFO_PAGE)
	{
		((CVibratoLFODialog*)m_pArtDialog)->UpdateArticulation(m_pCurArticulation);
	}
	else if(m_nCurrentArticulationDlg == IDD_FILTER_PAGE)
	{
		((CFilterDialog*)m_pArtDialog)->UpdateArticulation(m_pCurArticulation);
	}

	// shared controls
	BOOL fDLS1 = m_pCurArticulation->IsDLS1();
	m_ArtDLS1Check.SetCheck(fDLS1);
	m_VibLFOButton.EnableWindow(!fDLS1);
	m_FilterButton.EnableWindow(!fDLS1);
}

