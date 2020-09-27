// PChannelPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "bandeditordll.h"
#include "Band.h" 
#include "BandDlg.h"
#include "PChannelPropPage.h"
#include "PChannelPropPageMgr.h"
#include "PChannelName.h"
#include "resourcemaps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern const UINT aidGroupFirstInstr[];
extern const UINT aidGroupName[];
extern BInstr abiGS[];
extern BInstr abiDrums[];

static const TCHAR achUnknown[11] = "----------";

// menu static member variables -- shared by all editor instances
HMENU CPChannelPropPage::m_hmenuPatch;
HMENU CPChannelPropPage::m_hmenuDrums;


/////////////////////////////////////////////////////////////////////////////
// CPChannelPropPage property page constructor

CPChannelPropPage::CPChannelPropPage(CPChannelPropPageManager* pPChannelPropPageManager) : CPropertyPage(CPChannelPropPage::IDD)
{
	//{{AFX_DATA_INIT(CPChannelPropPage)
	//}}AFX_DATA_INIT

	ASSERT(pPChannelPropPageManager);

	m_pCommonInstrumentObject= NULL;
	m_pPageManager = pPChannelPropPageManager;
	m_fNeedToDetach = FALSE;
}


CPChannelPropPage::~CPChannelPropPage()
{
}

/**
	Creates the Popup Menu for instrument selection. 
*/
void CPChannelPropPage::CreatePatchPopupMenu()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Do nothing if we already have valid menu handles
	if(::IsMenu(m_hmenuPatch) && ::IsMenu(m_hmenuDrums))
		return;

	HMENU     hmenu, hmenuGM, hmenuGS, hmenuDrums;
	char      buf[64];
	short     i;
	BInstr   *pbi;
	BOOL      fKeepGS;

	// Create the patch select popup menu
	m_hmenuPatch = CreatePopupMenu();
	hmenu = CreatePopupMenu();
	AppendMenu( m_hmenuPatch, MF_POPUP, (unsigned int) hmenu, "NULL" );
	hmenuGM = NULL;
	hmenuGS = NULL;

	pbi = &abiGS[0];
	i = 0;
#ifndef DMP_XBOX
	while( pbi->nStringId != IDS_PGMEND )
	{
		// Create a menu for GM and a submenu for GS
		if( hmenuGM == NULL )
		{
			fKeepGS = FALSE;
			hmenuGM = CreatePopupMenu();
			hmenuGS = CreatePopupMenu();
			AppendMenu( hmenuGM, MF_POPUP, (unsigned int) hmenuGS, "GS" );
			AppendMenu( hmenuGM, MF_SEPARATOR, 0, NULL );
		}

		// Append items to the correct menu
		if( pbi->bMSB == 0  &&  pbi->bLSB == 0 )
		{
			AppendMenu( hmenuGM, MF_STRING, pbi->nStringId, *pbi->pstrName );
		}
		else  {
			AppendMenu( hmenuGS, MF_STRING, pbi->nStringId, *pbi->pstrName );
			fKeepGS = TRUE;
		}

		// Next instrument
		pbi++;

		// Is this the first instrument in a new group?
		if( pbi->nStringId == aidGroupFirstInstr[i + 1] )
		{
			// Remove the GS menu and separator if no items were inserted
			if( !fKeepGS )  {
				RemoveMenu( hmenuGM, 0, MF_BYPOSITION );
				RemoveMenu( hmenuGM, 0, MF_BYPOSITION );
			}

			// Load the group name and append it to the main popup
			LoadString( theApp.m_hInstance, aidGroupName[i], buf, sizeof(buf) - 1 );
			AppendMenu( GetSubMenu( m_hmenuPatch, 0 ), MF_POPUP, (unsigned int) hmenuGM, buf );
			hmenuGM = NULL;
			i++;
		}
	}
#endif // DMP_XBOX

	// DLS menu item
	LoadString( theApp.m_hInstance, IDS_DLS, buf, sizeof(buf) - 1 );
	AppendMenu( GetSubMenu( m_hmenuPatch, 0 ), MF_SEPARATOR, 0, NULL );
	AppendMenu( GetSubMenu( m_hmenuPatch, 0 ), MF_STRING, IDS_DLS, buf );

	// Create the drums select popup menu
	m_hmenuDrums = CreatePopupMenu();
	hmenuDrums = CreatePopupMenu();
	AppendMenu( m_hmenuDrums, MF_POPUP, (unsigned int) hmenuDrums, "NULL" );
	pbi = &abiDrums[0];
	i = 0;
#ifndef DMP_XBOX
	while( pbi->nStringId != IDS_PGMEND )
	{
		AppendMenu( hmenuDrums, MF_STRING, pbi->nStringId, *pbi->pstrName );
		pbi++;
	}
#endif // DMP_XBOX
	LoadString( theApp.m_hInstance, IDS_DLS, buf, sizeof(buf) - 1 );
	AppendMenu( GetSubMenu( m_hmenuDrums, 0 ), MF_SEPARATOR, 0, NULL );
	AppendMenu( GetSubMenu( m_hmenuDrums, 0 ), MF_STRING, IDS_DLS, buf );
}


/**

  This method is called from OnShowProperties to set the 
  common property object for the property page, for each PChannel. 

*/
void CPChannelPropPage::SetPChannel(CCommonInstrumentPropertyObject* pCommonInstrumentObject)
{
	m_pCommonInstrumentObject = pCommonInstrumentObject;

	UpdateControls(pCommonInstrumentObject);
}

/////////////////////////////////////////////////////////////////////////////
// CPChannelPropPage::EnableItem

void CPChannelPropPage::EnableItem(int nID, BOOL fEnable)
{
	CWnd* pWnd = GetDlgItem( nID );

	if( pWnd )
	{
		if( fEnable == FALSE )
		{
			pWnd->SetWindowText( achUnknown );
		}
		pWnd->EnableWindow( fEnable );
	}
}


/**
  Enables/disables controls in the property page.
*/
void CPChannelPropPage::EnableControls(BOOL fEnable)
{
	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;

	EnableItem(IDC_PCHANNEL_NUMBER_EDIT, fEnable);

	if(pCommonPropertyObject->GetPChannelNumber() == -1)
	{
		// Can't edit this field when multiple PChannels are selected
		EnableItem(IDC_PCHANNEL_NAME_EDIT, FALSE);
	}
	else
	{
		EnableItem(IDC_PCHANNEL_NAME_EDIT, fEnable);
	}

	m_SelectInstrument.EnableWindow(fEnable);

	EnableItem(IDC_PRIORITY_EDIT, fEnable);
	m_PrioritySpin.EnableWindow(fEnable);

	EnableItem(IDC_OCTAVE_EDIT, fEnable);
	m_OctaveSpin.EnableWindow(fEnable);

	EnableItem(IDC_TRANSPOSE_EDIT, fEnable);
	m_TransposeSpin.EnableWindow(fEnable);

	EnableItem(IDC_PBOCTAVE_EDIT, fEnable);
	m_PBOctaveSpin.EnableWindow(fEnable);

	EnableItem(IDC_PBTRANSPOSE_EDIT, fEnable);
	m_PBTransposeSpin.EnableWindow(fEnable);

	EnableItem(IDC_VOLUME_EDIT, fEnable);
	m_VolumeSpin.EnableWindow(fEnable);

	EnableItem(IDC_PAN_EDIT, fEnable);
	m_PanSpin.EnableWindow(fEnable);

	EnableItem(IDC_LOWNOTE_EDIT, fEnable);
	m_LowNoteSpin.EnableWindow(fEnable);

	EnableItem(IDC_HIGHNOTE_EDIT, fEnable);
	m_HighNoteSpin.EnableWindow(fEnable);

	EnableItem(IDC_PRIORITY_EDIT, fEnable);
	m_PrioritySpin.EnableWindow(fEnable);
	m_PriorityCombo.EnableWindow(fEnable);

	m_IgnoreInstrument.EnableWindow(fEnable);
	m_IgnoreOctaveTranspose.EnableWindow(fEnable);
	m_IgnorePitchBendRange.EnableWindow(fEnable);
	m_IgnoreVolume.EnableWindow(fEnable);
	m_IgnorePan.EnableWindow(fEnable);
	m_IgnorePriority.EnableWindow(fEnable);
    m_IgnoreNoteRange.EnableWindow(fEnable);

	m_SoundCanvasCheck.EnableWindow(fEnable);
}


/**
  Checks if any of the ignores have been previously set and 
  disables or enables controls accordingly. 
*/
void CPChannelPropPage::CheckForIgnores(CCommonInstrumentPropertyObject* pCommonInstrumentObject)
{
	m_IgnoreInstrument.SetCheck(pCommonInstrumentObject->m_nIgnoreInstrument);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnoreInstrument, m_SelectInstrument);

	m_IgnoreOctaveTranspose.SetCheck(pCommonInstrumentObject->m_nIgnoreOctaveTranspose);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnoreOctaveTranspose, m_OctaveEdit);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnoreOctaveTranspose, m_TransposeEdit);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnoreOctaveTranspose, m_OctaveSpin);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnoreOctaveTranspose, m_TransposeSpin);

	m_IgnorePitchBendRange.SetCheck(pCommonInstrumentObject->m_nIgnorePitchBendRange);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnorePitchBendRange, m_PBOctaveEdit);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnorePitchBendRange, m_PBTransposeEdit);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnorePitchBendRange, m_PBOctaveSpin);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnorePitchBendRange, m_PBTransposeSpin);

	m_IgnoreVolume.SetCheck(pCommonInstrumentObject->m_nIgnoreVolume);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnoreVolume, m_VolumeEdit);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnoreVolume, m_VolumeSpin);

	m_IgnorePan.SetCheck(pCommonInstrumentObject->m_nIgnorePan);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnorePan, m_PanEdit);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnorePan, m_PanSpin);

	m_IgnorePriority.SetCheck(pCommonInstrumentObject->m_nIgnorePriority);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnorePriority, m_PriorityCombo);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnorePriority, m_PriorityEdit);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnorePriority, m_PrioritySpin);

	m_IgnoreNoteRange.SetCheck(pCommonInstrumentObject->m_nIgnoreNoteRange);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnoreNoteRange, m_LowNoteEdit);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnoreNoteRange, m_LowNoteSpin);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnoreNoteRange, m_HighNoteEdit);
	CheckAndEnableControl(pCommonInstrumentObject->m_nIgnoreNoteRange, m_HighNoteSpin);
}


/**
  Enbales/disables the passed control based on the check state.
*/
void CPChannelPropPage::CheckAndEnableControl(int nCheckState, CWnd& control)
{
	bool bEnable = false;
	if(nCheckState == IGNORE_OFF)
		bEnable = true;

	control.EnableWindow(bEnable);
}
	


/**
  Updates all the controls with values set in the passed common property object.
  In case of multiple selections the Edit controls will show the indeterminate state
  if the values for each PChannel are different.
*/
void CPChannelPropPage::UpdateControls(CCommonInstrumentPropertyObject*	pCommonInstrumentObject)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Do nothing when no property object or no channels selected
	if( pCommonInstrumentObject == NULL || pCommonInstrumentObject->GetNumberOfChannels() == 0)
	{
		EnableControls( FALSE );
		return;
	}

	EnableControls( TRUE );

	// Limit size of text
	m_PriorityEdit.LimitText( 5 );

	CheckForIgnores(pCommonInstrumentObject);

	CString sWindowText;

	int nSelectedChannels = pCommonInstrumentObject->GetNumberOfChannels();

	if(nSelectedChannels != 1)
	{
		m_PChNumberEdit.EnableWindow(FALSE);
		m_PChNumberSpin.EnableWindow(FALSE);
		m_PChNumberSpin.Invalidate();
	}
	else
	{
		m_PChNumberEdit.EnableWindow(TRUE);
		m_PChNumberSpin.EnableWindow(TRUE);
		m_PChNumberSpin.Invalidate();
	}

	// Set PCh#
	if(pCommonInstrumentObject->GetPChannelNumber() != -1)
	{
		DWORD dwPChannel =pCommonInstrumentObject->GetPChannelNumber();
		sWindowText.Format("%d",  dwPChannel + 1);
		m_PChNumberEdit.SetWindowText(sWindowText);
		m_PChNumberSpin.SetPos(dwPChannel);
	}
	else
	{
		m_PChNumberEdit.SetWindowText(achUnknown);
	}

	// Set PCh Name
	if(pCommonInstrumentObject->GetPChannelNumber() != -1)
	{
		m_PChNameEdit.SetWindowText(pCommonInstrumentObject->GetPChannelName());
	}
	else
	{
		m_PChNameEdit.SetWindowText(achUnknown);
	}

	// Set Instrument
	m_SelectInstrument.SetWindowText(pCommonInstrumentObject->GetInstrument());

	// Set the instrument Info
	m_InstrumentInfoStatic.SetWindowText(pCommonInstrumentObject->m_sInstrumentInfo);

	// Set Octave
	if(pCommonInstrumentObject->GetOctave() != -12)
	{
		m_OctaveSpin.SetPos(pCommonInstrumentObject->GetOctave());
		if(pCommonInstrumentObject->GetOctave() > 0)
		{
			sWindowText.Format("+%d",pCommonInstrumentObject->GetOctave());
		}
		else
		{
			sWindowText.Format("%d",pCommonInstrumentObject->GetOctave());
		}
	}
	else
	{
		m_OctaveSpin.SetPos(0);
		sWindowText = achUnknown;
	}
	m_OctaveEdit.SetWindowText(sWindowText);

	// Set Transpose
	if(pCommonInstrumentObject->GetTranspose() != -12 )
	{
		m_TransposeSpin.SetPos(pCommonInstrumentObject->GetTranspose());
		sWindowText = CBandDlg::GetInterval(pCommonInstrumentObject->GetTranspose());
	}
	else
	{
		m_OctaveSpin.SetPos(0);
		sWindowText = achUnknown;
	}
	m_TransposeEdit.SetWindowText(sWindowText);


	// Set Pitch Bend Octave
	if(pCommonInstrumentObject->GetPitchBendOctave() != -12)
	{
		m_PBOctaveSpin.SetPos(pCommonInstrumentObject->GetPitchBendOctave());
		if(pCommonInstrumentObject->GetPitchBendOctave() > 0)
		{
			sWindowText.Format("+%d",pCommonInstrumentObject->GetPitchBendOctave());
		}
		else
		{
			sWindowText.Format("%d",pCommonInstrumentObject->GetPitchBendOctave());
		}
	}
	else
	{
		m_PBOctaveSpin.SetPos(0);
		sWindowText = achUnknown;
	}
	m_PBOctaveEdit.SetWindowText(sWindowText);

	// Set Pitch Bend Interval
	if(pCommonInstrumentObject->GetPitchBendTranspose() != -12 )
	{
		m_PBTransposeSpin.SetPos(pCommonInstrumentObject->GetPitchBendTranspose());
		sWindowText = CBandDlg::GetInterval(pCommonInstrumentObject->GetPitchBendTranspose());
	}
	else
	{
		m_PBOctaveSpin.SetPos(0);
		sWindowText = achUnknown;
	}
	m_PBTransposeEdit.SetWindowText(sWindowText);

	// Set Volume
	if(pCommonInstrumentObject->GetVolume() != -1)
	{
		m_VolumeSpin.SetPos(pCommonInstrumentObject->GetVolume());
		sWindowText.Format("%d", pCommonInstrumentObject->GetVolume());
	}
	else
	{
		m_VolumeSpin.SetPos(0);
		sWindowText = achUnknown;
	}
	m_VolumeEdit.SetWindowText(sWindowText);

	// Set Pan
	if(pCommonInstrumentObject->GetPan() != -1)
	{
		m_PanSpin.SetPos(pCommonInstrumentObject->GetPan());
		sWindowText = GetPanString(BYTE(pCommonInstrumentObject->GetPan()));
	}
	else
	{
		m_PanSpin.SetPos(64); // Balanced
		sWindowText = achUnknown;
	}
	m_PanEdit.SetWindowText(sWindowText);

	// Set Note Range
	if(pCommonInstrumentObject->GetLowNote() != -1)
	{
		m_LowNoteSpin.SetPos(pCommonInstrumentObject->GetLowNote());
		sWindowText = CBandDlg::GetNoteText(pCommonInstrumentObject->GetLowNote());
	}
	else
	{
		m_LowNoteSpin.SetPos(0);
		sWindowText = achUnknown;
	}
	m_LowNoteEdit.SetWindowText(sWindowText);

	if(pCommonInstrumentObject->GetHighNote() != -1)
	{
		m_HighNoteSpin.SetPos(pCommonInstrumentObject->GetHighNote());
		sWindowText = CBandDlg::GetNoteText(pCommonInstrumentObject->GetHighNote());
	}
	else
	{
		m_HighNoteSpin.SetPos(127);
		sWindowText = achUnknown;
	}
	m_HighNoteEdit.SetWindowText(sWindowText);

	// Set Priority Level
	DWORD dwPriorityLevel = 0xFFFFFFFF;
	DWORD dwPriorityOffset = 0xFFFFFFFF;
	pCommonInstrumentObject->GetPriority(dwPriorityLevel, dwPriorityOffset);
	SetPriorityLevel(dwPriorityLevel, dwPriorityOffset);

	// Set SoundCanvas Check
	m_SoundCanvasCheck.SetCheck(pCommonInstrumentObject->GetSoundCanvas());
}

void CPChannelPropPage::SetPriorityLevel(DWORD dwPriorityLevel, DWORD dwPriorityOffset)
{
	// Find out where this priority range falls
	int nCount = 0;
	while(dwPriorityLevel < dwaPriorityLevels[nCount] && nCount < PRIORITY_LEVELS)
		nCount++;

	if(CInstrumentListItem::m_CommonPropertyObject.m_nChannels <= 1)
	{
		m_PriorityCombo.SetCurSel(nCount);

		CString sOffset;
		sOffset.Format("%d", dwPriorityOffset);

		m_PrioritySpin.SetPos(dwPriorityOffset);
		m_PriorityEdit.SetWindowText(sOffset);
	}
	else
	{
		if(dwPriorityLevel == 0xFFFFFFFF)
		{
			int nUnknownStrIndex = m_PriorityCombo.FindStringExact(-1, achUnknown);
			if(nUnknownStrIndex == CB_ERR)
				m_PriorityCombo.AddString(achUnknown);
			m_PriorityCombo.SelectString(-1, achUnknown);
		}
		else
		{
			int nUnknownStrIndex = m_PriorityCombo.FindStringExact(-1, achUnknown);
			if(nUnknownStrIndex == CB_ERR)
				m_PriorityCombo.DeleteString(nUnknownStrIndex);
			m_PriorityCombo.SetCurSel(nCount);
		}

		if(dwPriorityOffset == 0xFFFFFFFF)
		{
			m_PriorityEdit.SetWindowText(achUnknown);
		}
		else
		{
			CString sOffset;
			sOffset.Format("%d", dwPriorityOffset);

			m_PrioritySpin.SetPos(dwPriorityOffset);
			m_PriorityEdit.SetWindowText(sOffset);
		}
	}
}



/**
	Gets the String to be displayed for a Pan value. 
	(0 is extreme Left, 64 Mid and 128 extreme Right)
*/
CString CPChannelPropPage::GetPanString(BYTE bPan)
{
	CString sPanString;
	if(bPan < 63)
		sPanString.Format("L%d", 63 - bPan); 
	else if(bPan > 63)
		sPanString.Format("R%d", bPan - 63);
	else if(bPan == 63)
		sPanString.Format("Mid");
	else
		sPanString.Format("---");

	return sPanString;
}


void CPChannelPropPage::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPChannelPropPage)
	DDX_Control(pDX, IDC_PCHANNEL_NUMBER_SPIN, m_PChNumberSpin);
	DDX_Control(pDX, IDC_IGNORE_PBRANGE_CHECK, m_IgnorePitchBendRange);
	DDX_Control(pDX, IDC_PBTRANSPOSE_SPIN, m_PBTransposeSpin);
	DDX_Control(pDX, IDC_PBTRANSPOSE_EDIT, m_PBTransposeEdit);
	DDX_Control(pDX, IDC_PBOCTAVE_SPIN, m_PBOctaveSpin);
	DDX_Control(pDX, IDC_PBOCTAVE_EDIT, m_PBOctaveEdit);
	DDX_Control(pDX, IDC_INSTRUMENT_INFO_STATIC, m_InstrumentInfoStatic);
	DDX_Control(pDX, IDC_PRIORITY_COMBO, m_PriorityCombo);
	DDX_Control(pDX, IDC_USE_SOUNDCANVAS_CHECK, m_SoundCanvasCheck);
	DDX_Control(pDX, IDC_IGNORE_NOTERANGE_CHECK, m_IgnoreNoteRange);
	DDX_Control(pDX, IDC_HIGHNOTE_SPIN, m_HighNoteSpin);
	DDX_Control(pDX, IDC_HIGHNOTE_EDIT, m_HighNoteEdit);
	DDX_Control(pDX, IDC_LOWNOTE_SPIN, m_LowNoteSpin);
	DDX_Control(pDX, IDC_LOWNOTE_EDIT, m_LowNoteEdit);
	DDX_Control(pDX, IDC_VOLUME_SPIN, m_VolumeSpin);
	DDX_Control(pDX, IDC_VOLUME_EDIT, m_VolumeEdit);
	DDX_Control(pDX, IDC_TRANSPOSE_SPIN, m_TransposeSpin);
	DDX_Control(pDX, IDC_TRANSPOSE_EDIT, m_TransposeEdit);
	DDX_Control(pDX, IDC_PRIORITY_SPIN, m_PrioritySpin);
	DDX_Control(pDX, IDC_PRIORITY_EDIT, m_PriorityEdit);
	DDX_Control(pDX, IDC_PCHANNEL_NUMBER_EDIT, m_PChNumberEdit);
	DDX_Control(pDX, IDC_PCHANNEL_NAME_EDIT, m_PChNameEdit);
	DDX_Control(pDX, IDC_PAN_SPIN, m_PanSpin);
	DDX_Control(pDX, IDC_PAN_EDIT, m_PanEdit);
	DDX_Control(pDX, IDC_OCTAVE_EDIT, m_OctaveEdit);
	DDX_Control(pDX, IDC_OCTAVE_SPIN, m_OctaveSpin);
	DDX_Control(pDX, IDC_INSTRUMENT_BUTTON, m_SelectInstrument);
	DDX_Control(pDX, IDC_IGNORE_VOLUME_CHECK, m_IgnoreVolume);
	DDX_Control(pDX, IDC_IGNORE_PRIORITY_CHECK, m_IgnorePriority);
	DDX_Control(pDX, IDC_IGNORE_PAN_CHECK, m_IgnorePan);
	DDX_Control(pDX, IDC_IGNORE_OCTAVETRANSPOSE_CHECK, m_IgnoreOctaveTranspose);
	DDX_Control(pDX, IDC_IGNORE_INSTRUMENT_CHECK, m_IgnoreInstrument);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPChannelPropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPChannelPropPage)
	ON_BN_CLICKED(IDC_INSTRUMENT_BUTTON, OnInstrumentButton)
	ON_BN_CLICKED(IDC_IGNORE_INSTRUMENT_CHECK, OnIgnoreInstrumentCheck)
	ON_BN_CLICKED(IDC_IGNORE_OCTAVETRANSPOSE_CHECK, OnIgnoreOctavetransposeCheck)
	ON_BN_CLICKED(IDC_IGNORE_PAN_CHECK, OnIgnorePanCheck)
	ON_BN_CLICKED(IDC_IGNORE_PRIORITY_CHECK, OnIgnorePriorityCheck)
	ON_BN_CLICKED(IDC_IGNORE_VOLUME_CHECK, OnIgnoreVolumeCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PAN_SPIN, OnDeltaposPanSpin)
	ON_EN_KILLFOCUS(IDC_OCTAVE_EDIT, OnKillfocusOctaveEdit)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_PAN_EDIT, OnKillfocusPanEdit)
	ON_EN_KILLFOCUS(IDC_TRANSPOSE_EDIT, OnKillfocusTransposeEdit)
	ON_EN_KILLFOCUS(IDC_VOLUME_EDIT, OnKillfocusVolumeEdit)
	ON_EN_KILLFOCUS(IDC_PRIORITY_EDIT, OnKillfocusPriorityEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_VOLUME_SPIN, OnDeltaposVolumeSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TRANSPOSE_SPIN, OnDeltaposTransposeSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_OCTAVE_SPIN, OnDeltaposOctaveSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PRIORITY_SPIN, OnDeltaposPrioritySpin)
	ON_BN_CLICKED(IDC_IGNORE_NOTERANGE_CHECK, OnIgnoreNoteRangeCheck)
	ON_EN_KILLFOCUS(IDC_HIGHNOTE_EDIT, OnKillfocusHighNoteEdit)
	ON_EN_KILLFOCUS(IDC_LOWNOTE_EDIT, OnKillfocusLowNoteEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_LOWNOTE_SPIN, OnDeltaposLowNoteSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HIGHNOTE_SPIN, OnDeltaposHighNoteSpin)
	ON_BN_CLICKED(IDC_USE_SOUNDCANVAS_CHECK, OnUseSoundcanvasCheck)
	ON_CBN_SELCHANGE(IDC_PRIORITY_COMBO, OnSelchangePriorityCombo)
	ON_EN_CHANGE(IDC_PCHANNEL_NAME_EDIT, OnChangePchannelNameEdit)
	ON_BN_CLICKED(IDC_IGNORE_PBRANGE_CHECK, OnIgnorePitchBendRangeCheck)
	ON_EN_KILLFOCUS(IDC_PBOCTAVE_EDIT, OnKillfocusPitchBendOctaveEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PBOCTAVE_SPIN, OnDeltaposPitchBendOctaveSpin)
	ON_EN_KILLFOCUS(IDC_PBTRANSPOSE_EDIT, OnKillfocusPitchBendTransposeEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PBTRANSPOSE_SPIN, OnDeltaposPitchBendTransposeSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PCHANNEL_NUMBER_SPIN, OnDeltaposPChannelNumberSpin)
	ON_EN_KILLFOCUS(IDC_PCHANNEL_NUMBER_EDIT, OnKillfocusPChannelNumberEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPChannelPropPage message handlers
void CPChannelPropPage::OnInstrumentButton() 
{
	// Show the Instruments popup here
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CRect  rect;
	m_SelectInstrument.GetWindowRect( &rect );
	
	int nChannel = CInstrumentListItem::m_CommonPropertyObject.GetPChannelNumber();
	
	HMENU hmenu = m_hmenuDrums;

	if(nChannel != 9 && nChannel%16 != 9)
		hmenu = m_hmenuPatch;

	TrackPopupMenu( GetSubMenu( hmenu, 0 ), TPM_LEFTALIGN | TPM_VCENTERALIGN | TPM_LEFTBUTTON,
								rect.left, rect.bottom, 0, m_hWnd, NULL );
}

void CPChannelPropPage::OnIgnoreInstrumentCheck() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();
	ASSERT(pBandDlg);

	int nState = m_IgnoreInstrument.GetCheck();
	
	if( nState == 1)
	{
		m_SelectInstrument.EnableWindow(true);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreInstrument(IGNORE_OFF);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreOctaveTranspose(IGNORE_OFF);
		CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = IGNORE_OCTAVETRANSPOSE_CHANGED;
	}
	else if(nState == 0)
	{
		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreInstrument(IGNORE_ON);
		m_SelectInstrument.EnableWindow(false);
	}
	else
	{
		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreInstrument(IGNORE_UNDT);
		m_SelectInstrument.EnableWindow(false);
	}

	// save the state for undo
	pBandDlg->SaveStateForUndo("Use Instrument Patch Change");

	CInstrumentListItem::m_CommonPropertyObject.m_dwChanged |= IGNORE_INSTRUMENT_CHANGED;
	m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
}

void CPChannelPropPage::OnIgnoreOctavetransposeCheck() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();
	ASSERT(pBandDlg);


	int nState = m_IgnoreOctaveTranspose.GetCheck();
	
	if( nState == 1)
	{
		m_OctaveEdit.EnableWindow(true);
		m_TransposeEdit.EnableWindow(true);

		m_OctaveSpin.EnableWindow(true);
		m_TransposeSpin.EnableWindow(true);

		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreOctaveTranspose(IGNORE_OFF);
	}
	else if(nState == 0)
	{
		m_OctaveEdit.EnableWindow(false);
		m_TransposeEdit.EnableWindow(false);

		m_OctaveSpin.EnableWindow(false);
		m_TransposeSpin.EnableWindow(false);

		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreOctaveTranspose(IGNORE_ON);
	}
	else
	{
		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreOctaveTranspose(IGNORE_UNDT);
		
		m_OctaveEdit.EnableWindow(false);
		m_TransposeEdit.EnableWindow(false);

		m_OctaveSpin.EnableWindow(false);
		m_TransposeSpin.EnableWindow(false);
	}

	// save the state for undo
	pBandDlg->SaveStateForUndo("Use Octave/Transpose");


	CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = IGNORE_OCTAVETRANSPOSE_CHANGED;
	m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
}

void CPChannelPropPage::OnIgnorePanCheck() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();
	ASSERT(pBandDlg);


	int nState = m_IgnorePan.GetCheck();
	
	if( nState == 1)
	{
		m_PanEdit.EnableWindow(true);
		m_PanSpin.EnableWindow(true);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnorePan(IGNORE_OFF);
	}
	else if(nState == 0)
	{
		m_PanEdit.EnableWindow(false);
		m_PanSpin.EnableWindow(false);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnorePan(IGNORE_ON);
	}
	else
	{
		m_PanEdit.EnableWindow(false);
		m_PanSpin.EnableWindow(false);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnorePan(IGNORE_UNDT);
	}

	// save the state for undo
	pBandDlg->SaveStateForUndo("Use Pan");

	CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = IGNORE_PAN_CHANGED;
	m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
}

void CPChannelPropPage::OnIgnorePriorityCheck() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();
	ASSERT(pBandDlg);

	int nState = m_IgnorePriority.GetCheck();
	
	if( nState == 1)
	{
		m_PriorityEdit.EnableWindow(true);
		m_PrioritySpin.EnableWindow(true);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnorePriority(IGNORE_OFF);
	}
	else if(nState == 0)
	{
		m_PriorityEdit.EnableWindow(false);
		m_PrioritySpin.EnableWindow(false);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnorePriority(IGNORE_ON);
	}
	else 
	{
		m_PriorityEdit.EnableWindow(false);
		m_PrioritySpin.EnableWindow(false);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnorePriority(IGNORE_UNDT);
	}

	// Save the state for undo
	pBandDlg->SaveStateForUndo("Use Priority");

	CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = IGNORE_PRIORITY_CHANGED;
	m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
}

void CPChannelPropPage::OnIgnoreVolumeCheck() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();
	ASSERT(pBandDlg);

	int nState = m_IgnoreVolume.GetCheck();
	
	if( nState == 1)
	{
		m_VolumeEdit.EnableWindow(true);
		m_VolumeSpin.EnableWindow(true);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreVolume(IGNORE_OFF);
	}
	else if(nState == 0)
	{
		m_VolumeEdit.EnableWindow(false);
		m_VolumeSpin.EnableWindow(false);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreVolume(IGNORE_ON);
	}
	else 
	{
		m_VolumeEdit.EnableWindow(false);
		m_VolumeSpin.EnableWindow(false);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreVolume(IGNORE_UNDT);
	}

	// Save the state for undo
	pBandDlg->SaveStateForUndo("Use Volume");

	CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = IGNORE_VOLUME_CHANGED;
	m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
}

void CPChannelPropPage::OnIgnoreNoteRangeCheck() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();
	ASSERT(pBandDlg);


	CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = IGNORE_NOTERANGE_CHANGED;

	int nState = m_IgnoreNoteRange.GetCheck();
	
	if( nState == 1)
	{
		m_LowNoteEdit.EnableWindow(true);
		m_LowNoteSpin.EnableWindow(true);
		m_HighNoteEdit.EnableWindow(true);
		m_HighNoteSpin.EnableWindow(true);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreNoteRange(IGNORE_OFF);
        CInstrumentListItem::m_CommonPropertyObject.m_dwChanged |= NOTERANGE_CHANGED;
	}
	else if(nState == 0)
	{
		m_LowNoteEdit.EnableWindow(false);
		m_LowNoteSpin.EnableWindow(false);
		m_HighNoteEdit.EnableWindow(false);
		m_HighNoteSpin.EnableWindow(false);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreNoteRange(IGNORE_ON);
	}
	else 
	{
		m_LowNoteEdit.EnableWindow(false);
		m_LowNoteSpin.EnableWindow(false);
		m_HighNoteEdit.EnableWindow(false);
		m_HighNoteSpin.EnableWindow(false);
		CInstrumentListItem::m_CommonPropertyObject.SetIgnoreNoteRange(IGNORE_UNDT);
	}

	// Save the state for undo
	pBandDlg->SaveStateForUndo("Use Note Range");

	m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
}

BOOL CPChannelPropPage::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();

	m_PChNumberSpin.SetRange(0, 998);
	
	m_PanSpin.SetRange(0, 127);
	m_PanEdit.LimitText(3);
	
	m_VolumeSpin.SetRange(0, 127);
	m_VolumeEdit.LimitText(3);
	
	m_OctaveSpin.SetRange(-4, 4);
	m_OctaveEdit.LimitText(2);

	m_PBOctaveSpin.SetRange(0, 10);
	m_PBOctaveEdit.LimitText(2);

	m_TransposeSpin.SetRange(-11, 11);
	m_PBTransposeSpin.SetRange(-11, 11);

	m_LowNoteSpin.SetRange(0, 127);
	m_HighNoteSpin.SetRange(0, 127);

	InitPriorityCombo();
	m_PrioritySpin.SetRange(UD_MINVAL, UD_MAXVAL);

	m_fInChange = false;

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPChannelPropPage::InitPriorityCombo()
{
	CString sPriorityString;
	for(int nCount = 0 ; nCount < PRIORITY_LEVELS; nCount++)
	{
		 sPriorityString.LoadString(arrPriorityLevelNames[nCount]);
		 int nInsertionIndex = m_PriorityCombo.InsertString(nCount, sPriorityString);
		 m_PriorityCombo.SetItemData(nInsertionIndex, dwaPriorityLevels[nCount]);
	}
}


// Handle the instrument popup menu commands in here
BOOL CPChannelPropPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// not a menu message
	if( lParam )  
		return CPropertyPage::OnCommand(wParam, lParam);

	//
	// This is very similar to CBand::OnContextCommand - please try to keep them in sync!
	//

	CBandDlg* pBandDlg = CInstrumentListItem::m_CommonPropertyObject.GetBandDialog();
	ASSERT(pBandDlg);

	CBand* pBand = pBandDlg->GetBand();
	ASSERT(pBand);
	
	// check for program changes
	BInstr  *pbi;

	pbi = &abiGS[0];
	int nCount = 0;
	while( pbi->nStringId != IDS_PGMEND )
	{
		if( pbi->nStringId == wParam )
		{
			// Save the state for undo
			pBandDlg->SaveStateForUndo("Program Change");

			CString sName = *pbi->pstrName;
			m_SelectInstrument.SetWindowText(sName);

			CInstrumentListItem::m_CommonPropertyObject.SetPatch(pbi->bPatch);
			CInstrumentListItem::m_CommonPropertyObject.SetInstrument(sName);
			CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = INSTRUMENT_CHANGED;
			CInstrumentListItem::m_CommonPropertyObject.m_nABIIndex = nCount;
			m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);

			//return CPropertyPage::OnCommand(wParam, lParam);
			return 1;
		}
		pbi++;
		nCount++;
	}

	// check for drum program changes
	pbi = &abiDrums[0];
	nCount = 0;
	while( pbi->nStringId != IDS_PGMEND )
	{
		if( pbi->nStringId == wParam )
		{
			// Save the state for undo
			pBandDlg->SaveStateForUndo("Program Change");

			CString sName = *pbi->pstrName;
			m_SelectInstrument.SetWindowText(sName);

			CInstrumentListItem::m_CommonPropertyObject.SetPatch(pbi->bPatch);
			CInstrumentListItem::m_CommonPropertyObject.SetInstrument(sName);
			CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = INSTRUMENT_CHANGED;
			CInstrumentListItem::m_CommonPropertyObject.m_dwInstrumentChangeID = DRUMS_REQUEST;
			CInstrumentListItem::m_CommonPropertyObject.m_nABIIndex = nCount;

			m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);

			//return CPropertyPage::OnCommand(wParam, lParam);
			return 1;
		}
		pbi++;
		nCount++;
	}

	// Check if it's other DLS request
	if( wParam == IDS_DLS )
	{
		// Save the state for undo
		pBandDlg->SaveStateForUndo("Program Change");

		CInstrumentListItem::m_CommonPropertyObject.m_dwInstrumentChangeID = DLS_REQUEST;
		CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = INSTRUMENT_CHANGED;

		m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
		return 1;
	}

	return CPropertyPage::OnCommand(wParam, lParam);
}
	

int CPChannelPropPage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CPChannelPropPage::OnDestroy() 
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

BOOL CPChannelPropPage::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_PanSpin.SetPos(0);
	m_VolumeSpin.SetPos(0);
	m_OctaveSpin.SetPos(0);
	m_TransposeSpin.SetPos(0);

	// Update controls
	m_pPageManager->RefreshData();

	return CPropertyPage::OnSetActive();
}

void CPChannelPropPage::OnKillfocusOctaveEdit() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();

	ASSERT(pBandDlg);

	// Get the octave from the edit control and set it
	CString sOctaveString;
	m_OctaveEdit.GetWindowText(sOctaveString);
	int nOctaveValue = atoi(LPCTSTR(sOctaveString));

	if(nOctaveValue >= -4 && nOctaveValue <= 4)
		m_OctaveSpin.SetPos(nOctaveValue);

	int nChangedOctave = LOWORD(m_OctaveSpin.GetPos());
	if(nChangedOctave > 4)
		nChangedOctave = nChangedOctave - 65536;

	CString sTransposeString;
	m_TransposeEdit.GetWindowText(sTransposeString);
	int nTranspose = 0; 
	if(CBandDlg::GetTransposeValue(sTransposeString, &nTranspose))
		m_TransposeSpin.SetPos(nTranspose);

	nTranspose = LOWORD(m_TransposeSpin.GetPos());
	if(nTranspose > 11)
		nTranspose = nTranspose - 65536;

	CString sOctave;
	if(nChangedOctave > 0)
		sOctave.Format("+%d", nChangedOctave);
	else if(nChangedOctave <= 0)
		sOctave.Format("%d", nChangedOctave);
	m_OctaveEdit.SetWindowText(sOctave);


	CString sTranspose = CBandDlg::GetInterval(nTranspose);
	m_TransposeEdit.SetWindowText(sTranspose);

	int nOctaveTranspose = (nChangedOctave * 12) + nTranspose;

	if(nOctaveTranspose > 60)
		nOctaveTranspose = nOctaveTranspose - 65536;
	
	if(pCommonPropertyObject->GetOctaveTranspose() != nOctaveTranspose)
	{
		CUndoMan* pUndoMan = pBandDlg->m_pBandDo;

		// Save the state for undo
		pBandDlg->SaveStateForUndo("Transpose");

 		pCommonPropertyObject->m_dwChanged = OCTAVE_CHANGED;
		pCommonPropertyObject->SetReset(true);
		pCommonPropertyObject->SetOctaveTranspose(short(nOctaveTranspose));
		m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
	}
}



void CPChannelPropPage::OnKillfocusPanEdit() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();

	ASSERT(pBandDlg);

	// Get the string and parse it for the correct Pan value
	CString sPanString;
	m_PanEdit.GetWindowText(sPanString);
	int nPanValue = GetPanValue(sPanString);
	
	if(nPanValue != -1)
		m_PanSpin.SetPos(nPanValue);

	int nPan = LOWORD(m_PanSpin.GetPos());
	CString sNewPanValue = GetPanString(BYTE(nPan));
	m_PanEdit.SetWindowText(sNewPanValue);
	
	if(pCommonPropertyObject->m_nPan != nPan)
	{
		CUndoMan* pUndoMan = pBandDlg->m_pBandDo;

		// Save the state for undo
		pBandDlg->SaveStateForUndo("Pan");

		pCommonPropertyObject->m_dwChanged = PAN_CHANGED;
		pCommonPropertyObject->m_nPan = nPan;
		m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
	}
}

int CPChannelPropPage::GetPanValue(CString sPanString)
{
	if(sPanString.CompareNoCase("Mid") == 0)
		return 63;

	int nPanValue =  0;
	int nLength = sPanString.GetLength();
	CString sBalance = sPanString.Left(1);
	CString sValue = sPanString.Right(nLength - 1);
	
	nPanValue = atoi(LPCTSTR(sValue));
	
		
	if(sBalance.CompareNoCase("R") == 0)
		return 63 + nPanValue;
	else if(sBalance.CompareNoCase("L") == 0)
		return 63 - nPanValue;
	else
		return -1;
}


void CPChannelPropPage::OnKillfocusTransposeEdit() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Get the transpose from the edit control and set it
	
	CString sTransposeString;
	m_TransposeEdit.GetWindowText(sTransposeString);

	int nTransposeValue; 

	if(CBandDlg::GetTransposeValue(sTransposeString, &nTransposeValue))
		m_TransposeSpin.SetPos(nTransposeValue);

	OnKillfocusOctaveEdit();
}


void CPChannelPropPage::OnKillfocusVolumeEdit() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();

	ASSERT(pBandDlg);

	CString sVolumeString;
	m_VolumeEdit.GetWindowText(sVolumeString);
	int nVolumeValue = atoi(LPCTSTR(sVolumeString));

	if(nVolumeValue >= 0 && nVolumeValue  <= 128)
		m_VolumeSpin.SetPos(nVolumeValue );

	int nVolume = LOWORD(m_VolumeSpin.GetPos());
	CString sNewVolumeValue;
	sNewVolumeValue.Format("%d", nVolume);
	m_VolumeEdit.SetWindowText(sNewVolumeValue);

	if(pCommonPropertyObject->m_nVolume != nVolume)
	{
		CUndoMan* pUndoMan = pBandDlg->m_pBandDo;

		// Save the state for undo
		pBandDlg->SaveStateForUndo("Volume");

		pCommonPropertyObject->m_dwChanged = VOLUME_CHANGED;
		pCommonPropertyObject->m_nVolume = nVolume;
		m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
	}
}

void CPChannelPropPage::OnKillfocusPriorityEdit() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();
	ASSERT(pBandDlg);

	bool bPreserveOffset = false;
	int nPriorityOffset = 0;


	CString sPriorityString;
	m_PriorityEdit.GetWindowText(sPriorityString);
	if(sPriorityString != achUnknown)
	{
		nPriorityOffset = atoi(LPCTSTR(sPriorityString));
		if(nPriorityOffset >= UD_MINVAL && nPriorityOffset <= UD_MAXVAL)
			m_PrioritySpin.SetPos(nPriorityOffset);
		
		nPriorityOffset = LOWORD(m_PrioritySpin.GetPos());
	}
	else
	{
		bPreserveOffset = true;
	}

	// Get the current Selection from the Priority ComboBox.
	int nCurLevelIndex = m_PriorityCombo.GetCurSel();
	DWORD dwPriority = m_PriorityCombo.GetItemData(nCurLevelIndex);
	dwPriority |= nPriorityOffset;

	if(pCommonPropertyObject->m_dwPriority != dwPriority)
	{
		CUndoMan* pUndoMan = pBandDlg->m_pBandDo;

		// Save the state for undo
		pBandDlg->SaveStateForUndo("Priority");

		pCommonPropertyObject->m_dwChanged = PRIORITY_CHANGED;
		//pCommonPropertyObject->m_dwPriority = dwPriority;
		pCommonPropertyObject->SetReset(true);
		pCommonPropertyObject->SetPriority(dwPriority, bPreserveOffset);
		m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
	}
}

void CPChannelPropPage::OnKillfocusLowNoteEdit() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();

	ASSERT(pBandDlg);

	CString sLowNoteString;
	m_LowNoteEdit.GetWindowText(sLowNoteString);
	
	int nLowNoteValue = pCommonPropertyObject->m_nLowNote;
	if(FAILED(pBandDlg->GetNoteValue(sLowNoteString, &nLowNoteValue)))
	{
		nLowNoteValue = pCommonPropertyObject->m_nLowNote;
	}

	int nHighNote = LOWORD(m_HighNoteSpin.GetPos());
	if(nLowNoteValue < 0)
	{
		nLowNoteValue = 0;
	}
	else if(nLowNoteValue > nHighNote)
	{
        nLowNoteValue = nHighNote;
	}

    m_LowNoteSpin.SetPos(nLowNoteValue);
	sLowNoteString = pBandDlg->GetNoteText(nLowNoteValue);
	m_LowNoteEdit.SetWindowText(sLowNoteString);

	short nLowNote = LOWORD(m_LowNoteSpin.GetPos());

	if(pCommonPropertyObject->m_nLowNote != nLowNote)
	{
		CUndoMan* pUndoMan = pBandDlg->m_pBandDo;

		// Save the state for undo
		pBandDlg->SaveStateForUndo("LowNote");

		pCommonPropertyObject->m_dwChanged = NOTERANGE_CHANGED;
		pCommonPropertyObject->m_nLowNote = nLowNote;
		m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
	}
}

void CPChannelPropPage::OnKillfocusHighNoteEdit() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();

	ASSERT(pBandDlg);

	CString sHighNoteString;
	m_HighNoteEdit.GetWindowText(sHighNoteString);

	int nHighNoteValue = pCommonPropertyObject->m_nHighNote;
	if(FAILED(pBandDlg->GetNoteValue(sHighNoteString, &nHighNoteValue)))
	{
		nHighNoteValue = pCommonPropertyObject->m_nHighNote;
	}

	int nLowNote = LOWORD(m_LowNoteSpin.GetPos());
	if(nHighNoteValue < nLowNote)
	{
		nHighNoteValue = nLowNote;
	}
	else if(nHighNoteValue > 127)
	{
        nHighNoteValue = 127;
	}

    m_HighNoteSpin.SetPos(nHighNoteValue );

	sHighNoteString = pBandDlg->GetNoteText(nHighNoteValue);
	m_HighNoteEdit.SetWindowText(sHighNoteString);

	short nHighNote = LOWORD(m_HighNoteSpin.GetPos());

	if(pCommonPropertyObject->m_nHighNote != nHighNote)
	{
		CUndoMan* pUndoMan = pBandDlg->m_pBandDo;

		// Save the state for undo
		pBandDlg->SaveStateForUndo("HighNote");

		pCommonPropertyObject->m_dwChanged = NOTERANGE_CHANGED;
		pCommonPropertyObject->m_nHighNote = nHighNote;
		m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
	}
}

void CPChannelPropPage::OnDeltaposVolumeSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int iCurrentPosition = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(iCurrentPosition + iDelta > 127)
	{
		iCurrentPosition = 127;
		iDelta = 0;
	}
	if(iCurrentPosition + iDelta < 0)
	{
		iCurrentPosition = 0;
		iDelta = 0;
	}

	if(!(iCurrentPosition == 127 && iDelta > 0) && !(iCurrentPosition == 0 && iDelta < 0))
	{
		CString sNewVolumeValue;
		sNewVolumeValue.Format("%d", iCurrentPosition + iDelta);
		m_VolumeEdit.SetWindowText(sNewVolumeValue);

		OnKillfocusVolumeEdit();
	}

	*pResult = 1;
}

void CPChannelPropPage::OnDeltaposPanSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iCurrentPosition = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(iCurrentPosition + iDelta > 127)
	{
		iCurrentPosition = 127;
		iDelta = 0;
	}
	if(iCurrentPosition + iDelta < 0)
	{
		iCurrentPosition = 0;
		iDelta = 0;
	}

	if(!(iCurrentPosition == 127 && iDelta > 0) && !(iCurrentPosition == 0 && iDelta < 0))
	{
		CString sNewPanValue = GetPanString(iCurrentPosition + iDelta);
		m_PanEdit.SetWindowText(sNewPanValue);

		OnKillfocusPanEdit();
	}

	*pResult = 1;
}


void CPChannelPropPage::OnDeltaposTransposeSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iCurPos = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(!(iCurPos == 11 && iDelta > 0) && !(iCurPos == -11 && iDelta < 0))
	{
		CString sTranspose = CBandDlg::GetInterval(iCurPos + iDelta);
		m_TransposeEdit.SetWindowText(sTranspose);
		m_TransposeSpin.SetPos(iCurPos + iDelta);
		OnKillfocusOctaveEdit();
	}

	*pResult = 1;
}


void CPChannelPropPage::OnDeltaposOctaveSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int iCurPos = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(!(iCurPos == 4 && iDelta > 0) && !(iCurPos == -4 && iDelta < 0))
	{
		CString sOctave;
		int nChangedOctave = iCurPos + iDelta;
		if(nChangedOctave > 0)
			sOctave.Format("+%d", nChangedOctave);
		else if(nChangedOctave <= 0)
			sOctave.Format("%d", nChangedOctave);
		m_OctaveEdit.SetWindowText(sOctave);
		OnKillfocusOctaveEdit();
	}
	*pResult = 1;
}

void CPChannelPropPage::OnDeltaposPrioritySpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iCurPos = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	int nPriorityOffset = iCurPos + iDelta;

	if(nPriorityOffset >= UD_MINVAL && nPriorityOffset <= UD_MAXVAL)
	{

		CString sPriorityOffset;
		sPriorityOffset.Format("%d", nPriorityOffset);
		m_PriorityEdit.SetWindowText(sPriorityOffset);
		
		OnKillfocusPriorityEdit();
	}
	*pResult = 1;
}

void CPChannelPropPage::OnDeltaposLowNoteSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int iCurrentPosition = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(!(iCurrentPosition == 127 && iDelta > 0) && !(iCurrentPosition == 0 && iDelta < 0))
	{
		CString sNewLowNoteValue;
		sNewLowNoteValue.Format("%d", iCurrentPosition + iDelta);
		m_LowNoteEdit.SetWindowText(sNewLowNoteValue);

		OnKillfocusLowNoteEdit();
	}

	*pResult = 1;
}

void CPChannelPropPage::OnDeltaposHighNoteSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int iCurrentPosition = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(!(iCurrentPosition == 127 && iDelta > 0) && !(iCurrentPosition == 0 && iDelta < 0))
	{
		CString sNewHighNoteValue;
		sNewHighNoteValue.Format("%d", iCurrentPosition + iDelta);
		m_HighNoteEdit.SetWindowText(sNewHighNoteValue);

		OnKillfocusHighNoteEdit();
	}

	*pResult = 1;
}

BOOL CPChannelPropPage::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( pMsg->message )
	{
		case WM_KEYDOWN:
			if( pMsg->lParam & 0x40000000 )
			{
				break;
			}

			switch( pMsg->wParam )
			{
				case VK_ESCAPE:
				{
					CWnd* pWnd = GetFocus();
					/*if( pWnd )
					{
						switch( pWnd->GetDlgCtrlID() )
						{
							case IDC_NAME: 
								m_editName.SetWindowText( m_pBand->m_csName );
								break;
						}
					}*/
					return TRUE;
				}

				case VK_RETURN:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						switch( pWnd->GetDlgCtrlID() )
						{
							case IDC_VOLUME_EDIT: 
								OnKillfocusVolumeEdit();
								break;

							case IDC_PAN_EDIT: 
								OnKillfocusPanEdit();
								break;

							case IDC_OCTAVE_EDIT: 
								OnKillfocusOctaveEdit();
								break;

							case IDC_TRANSPOSE_EDIT: 
								OnKillfocusTransposeEdit();
								break;

							case IDC_PRIORITY_EDIT: 
								OnKillfocusPriorityEdit();
								break;
						}

						CWnd* pWndNext = GetNextDlgTabItem( pWnd );
						if( pWndNext )
						{
							pWndNext->SetFocus();
						}
					}
					return TRUE;
				}
			}
			break;
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}


/**

  If checked - The user is saying use the Roland SoundCanvas (TM) set for this patch

*/ 
void CPChannelPropPage::OnUseSoundcanvasCheck() 
{
	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();
	ASSERT(pBandDlg);

	int nState = m_SoundCanvasCheck.GetCheck();
	
	if(nState == 1)
		CInstrumentListItem::m_CommonPropertyObject.SetSoundCanvas(true);
	else 
	{
		m_SoundCanvasCheck.SetCheck(0);
		CInstrumentListItem::m_CommonPropertyObject.SetSoundCanvas(false);
	}

	// Save the state for undo
	pBandDlg->SaveStateForUndo("Always Use Default GM Set");

	CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = SOUNDCANVAS_CHANGED;
	m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
}

void CPChannelPropPage::OnSelchangePriorityCombo() 
{
	OnKillfocusPriorityEdit();
}

void CPChannelPropPage::OnChangePchannelNameEdit() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();

	ASSERT(pBandDlg);

	// Get the string 
	CString sPChNameString;
	m_PChNameEdit.GetWindowText( sPChNameString );

	// Strip leading and trailing spaces
	sPChNameString.TrimRight();
	sPChNameString.TrimLeft();

	// Get the PChannel
	int nPChannelNumber = pCommonPropertyObject->GetPChannelNumber();

	if( nPChannelNumber == -1								// multiple items selected
	||  pCommonPropertyObject->GetNumberOfChannels() == 0 )	// no items selected
	{
//		CString strOrigPChName = pCommonPropertyObject->GetPChannelName();

		// Reset PChannel name
//		if( strOrigPChName.Compare( sPChNameString ) != 0 )
//		{
//			m_PChNameEdit.SetWindowText( strOrigPChName );
//		}
		return;
	}
	else
	{
		if( sPChNameString.Compare(achUnknown) == 0 )	// edit control string equals achUnknown
		{
			return;
		}
	}

	CBand* pBand  = pBandDlg->GetBand();
	IDMUSProdProject* pIProject;
	IDMUSProdPChannelName* pIPChannelName;

	// Get the Project that this Band belongs to...
	IDMUSProdNode* pIDocRootNode;
	if( SUCCEEDED ( pBand->GetDocRootNode( &pIDocRootNode ) ) )
	{
		if( SUCCEEDED ( pBand->m_pComponent->m_pIFramework->FindProject( pIDocRootNode, &pIProject) ) )
		{
			ASSERT(pIProject);

			// Query for an IDMUSProdPChannelName interface on the Project
			if( SUCCEEDED( pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&pIPChannelName ) ) )
			{
				// Found the interface, update the PChannel name 
				WCHAR wstrText[MAX_PATH];

				if( SUCCEEDED ( pIPChannelName->GetPChannelName( nPChannelNumber, wstrText ) ) )
				{
					CString strOrigPChName = wstrText;

					if( strOrigPChName.Compare( sPChNameString ) != 0 )
					{
						// Convert the name to wide characters
						MultiByteToWideChar( CP_ACP, 0, sPChNameString, -1, wstrText, MAX_PATH );

						pIPChannelName->SetPChannelName( nPChannelNumber, wstrText );
						pCommonPropertyObject->SetPChannelName( sPChNameString );
					}
				}

				RELEASE( pIPChannelName );
			}

			RELEASE( pIProject );
		}

		RELEASE( pIDocRootNode );
	}
}

void CPChannelPropPage::OnIgnorePitchBendRangeCheck() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();
	ASSERT(pBandDlg);


	int nState = m_IgnorePitchBendRange.GetCheck();
	
	if( nState == 1)
	{
		m_PBOctaveEdit.EnableWindow(true);
		m_PBTransposeEdit.EnableWindow(true);

		m_PBOctaveSpin.EnableWindow(true);
		m_PBTransposeSpin.EnableWindow(true);

		CInstrumentListItem::m_CommonPropertyObject.SetIgnorePitchBendRange(IGNORE_OFF);
	}
	else if(nState == 0)
	{
		m_PBOctaveEdit.EnableWindow(false);
		m_PBTransposeEdit.EnableWindow(false);

		m_PBOctaveSpin.EnableWindow(false);
		m_PBTransposeSpin.EnableWindow(false);

		CInstrumentListItem::m_CommonPropertyObject.SetIgnorePitchBendRange(IGNORE_ON);
	}
	else
	{
		CInstrumentListItem::m_CommonPropertyObject.SetIgnorePitchBendRange(IGNORE_UNDT);
		
		m_PBOctaveEdit.EnableWindow(false);
		m_PBTransposeEdit.EnableWindow(false);

		m_PBOctaveSpin.EnableWindow(false);
		m_PBTransposeSpin.EnableWindow(false);
	}

	// save the state for undo
	pBandDlg->SaveStateForUndo("Use Pitch Bend Range");


	CInstrumentListItem::m_CommonPropertyObject.m_dwChanged = IGNORE_PITCHBEND_CHANGED;
	m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
}

void CPChannelPropPage::OnKillfocusPitchBendOctaveEdit() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();

	ASSERT(pBandDlg);

	// Get the octave from the edit control and set it
	CString sOctaveString;
	m_PBOctaveEdit.GetWindowText(sOctaveString);
	int nOctaveValue = atoi(LPCTSTR(sOctaveString));

	if(nOctaveValue >= 0 && nOctaveValue <= 10)
	{
		m_PBOctaveSpin.SetPos(nOctaveValue);
	}

	int nChangedOctave = LOWORD(m_PBOctaveSpin.GetPos());
	if(nChangedOctave > 10)
	{
		nChangedOctave = nChangedOctave - 65536;
	}

	CString sTransposeString;
	m_PBTransposeEdit.GetWindowText(sTransposeString);
	int nTranspose = 0; 
	if(CBandDlg::GetTransposeValue(sTransposeString, &nTranspose))
	{
		m_PBTransposeSpin.SetPos(nTranspose);
	}

	nTranspose = LOWORD(m_PBTransposeSpin.GetPos());
	if(nTranspose > 11)
	{
		nTranspose = nTranspose - 65536;
	}

	CString sOctave;
	if(nChangedOctave > 0)
	{
		sOctave.Format("+%d", nChangedOctave);
	}
	else if(nChangedOctave <= 0)
	{
		sOctave.Format("%d", nChangedOctave);
	}
	m_PBOctaveEdit.SetWindowText(sOctave);


	CString sTranspose = CBandDlg::GetInterval(nTranspose);
	m_PBTransposeEdit.SetWindowText(sTranspose);

	int nPitchBendRange = (nChangedOctave * 12) + nTranspose;

	if(nPitchBendRange > 127)
	{
		nPitchBendRange = nPitchBendRange - 65536;
	}
	
	if(pCommonPropertyObject->GetPitchBendRange() != nPitchBendRange)
	{
		CUndoMan* pUndoMan = pBandDlg->m_pBandDo;

		// Save the state for undo
		pBandDlg->SaveStateForUndo("Pitch Bend Range");

 		pCommonPropertyObject->m_dwChanged = PITCHBEND_CHANGED;
		pCommonPropertyObject->SetReset(true);
		pCommonPropertyObject->SetPitchBendRange(short(nPitchBendRange));
		m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
	}
}

void CPChannelPropPage::OnDeltaposPitchBendOctaveSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int iCurPos = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(!(iCurPos == 10 && iDelta > 0) && !(iCurPos == -10 && iDelta < 0))
	{
		CString sOctave;
		int nChangedOctave = iCurPos + iDelta;
		if(nChangedOctave > 0)
		{
			sOctave.Format("+%d", nChangedOctave);
		}
		else if(nChangedOctave <= 0)
		{
			sOctave.Format("%d", nChangedOctave);
		}
		m_PBOctaveEdit.SetWindowText(sOctave);
		OnKillfocusPitchBendOctaveEdit();
	}
	*pResult = 1;
}

void CPChannelPropPage::OnKillfocusPitchBendTransposeEdit() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Get the transpose from the edit control and set it
	
	CString sTransposeString;
	m_PBTransposeEdit.GetWindowText(sTransposeString);

	int nTransposeValue; 

	if(CBandDlg::GetTransposeValue(sTransposeString, &nTransposeValue))
	{
		m_PBTransposeSpin.SetPos(nTransposeValue);
	}

	OnKillfocusPitchBendOctaveEdit();
}

void CPChannelPropPage::OnDeltaposPitchBendTransposeSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int iCurPos = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	if(!(iCurPos == 11 && iDelta > 0) && !(iCurPos == -11 && iDelta < 0))
	{
		CString sTranspose = CBandDlg::GetInterval(iCurPos + iDelta);
		m_PBTransposeEdit.SetWindowText(sTranspose);
		m_PBTransposeSpin.SetPos(iCurPos + iDelta);
		OnKillfocusPitchBendOctaveEdit();
	}

	*pResult = 1;
}

void CPChannelPropPage::OnDeltaposPChannelNumberSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();
	ASSERT(pBandDlg);
	if(pBandDlg == NULL)
	{
		return;
	}

	CBand* pBand = pBandDlg->GetBand();
	ASSERT(pBand);
	if(pBand == NULL)
	{
		return;
	}

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	int iCurPos = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	DWORD dwNextPChannel = iCurPos;

	if(iDelta > 0)
	{
		dwNextPChannel = pBand->GetNextAvailablePChannel(iCurPos);
	}
	else if(iDelta < 0)
	{
		dwNextPChannel = pBand->GetLastAvailablePChannel(iCurPos + 1);
		if(dwNextPChannel == DWORD(iCurPos + 1))
		{
			*pResult = 1;
			return;
		}
	}

	CString sNewPChNumber;
	sNewPChNumber.Format("%d", dwNextPChannel + 1);
	m_PChNumberEdit.SetWindowText(sNewPChNumber);
	OnKillfocusPChannelNumberEdit();
	
	*pResult = 1;
}

void CPChannelPropPage::OnKillfocusPChannelNumberEdit() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCommonInstrumentPropertyObject* pCommonPropertyObject = &CInstrumentListItem::m_CommonPropertyObject;
	CBandDlg* pBandDlg = pCommonPropertyObject->GetBandDialog();
	ASSERT(pBandDlg);
	if(pBandDlg == NULL)
	{
		return;
	}

	CBand* pBand = pBandDlg->GetBand();
	ASSERT(pBand);
	if(pBand == NULL)
	{
		return;
	}


	DWORD dwOldPChNumber = pCommonPropertyObject->GetPChannelNumber();
	
	CString sNewPChNumber;
	m_PChNumberEdit.GetWindowText(sNewPChNumber);
	
	DWORD dwNewPChNumber = atoi((LPCSTR)sNewPChNumber) - 1;
	if(dwNewPChNumber == dwOldPChNumber)
	{
		// PChannel Number hasn't changed
		return;
	}

	// Check if this PChannel is free
	if(pBand->IsPChannelFree(dwNewPChNumber) && dwNewPChNumber <= 998)
	{
		m_PChNumberSpin.SetPos(dwNewPChNumber);

		CUndoMan* pUndoMan = pBandDlg->m_pBandDo;

		// Save the state for undo
		pBandDlg->SaveStateForUndo("PChannel Number");

		pCommonPropertyObject->m_dwChanged = PCHANNEL_CHANGED;
		pCommonPropertyObject->m_nPChannelNumber = dwNewPChNumber;
		m_pPageManager->m_pIPropPageObject->SetData(&CInstrumentListItem::m_CommonPropertyObject);
	}
	else
	{
		CString sOldPChNumber;
		sOldPChNumber.Format("%d", dwOldPChNumber + 1);
		m_PChNumberEdit.SetWindowText(sOldPChNumber);
		m_PChNumberSpin.SetPos(dwOldPChNumber);
	}
}
