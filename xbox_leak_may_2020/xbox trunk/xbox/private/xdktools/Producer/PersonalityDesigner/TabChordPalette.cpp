// TabChordPalette.cpp : implementation file
//


#include "stdafx.h"
#pragma warning(disable:4201)
#include "personalitydesigner.h"
#include "TabChordPalette.h"
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////// Callback for CKeyboard Events
//
void PaletteKeyboardChangeCallback(CKeyboard* pKeyboard, void* hint, short nKey)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CTabChordPalette* pdlg = (CTabChordPalette*)hint;
	pdlg->DispatchKeyboardChange(pKeyboard, nKey);
}



/////////////////////////////////////////////////////////////////////////////
// CTabChordPalette property page


CTabChordPalette::CTabChordPalette(CPersonalityPageManager* pPageManager, IDMUSProdFramework* pIFramework ) 
		: CPropertyPage(CTabChordPalette::IDD)
{
		AFX_MANAGE_STATE(_afxModuleAddrThis);

	//{{AFX_DATA_INIT(CTabChordPalette)
	//}}AFX_DATA_INIT

	ASSERT( pPageManager != NULL );
	ASSERT( pIFramework != NULL );

	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	m_pPageManager = pPageManager;
//	m_pPageManager->AddRef();		intentionally missing

	m_fHaveData = FALSE;
	m_fInOnSetActive = FALSE;
	m_fNeedToDetach = FALSE;
}

CTabChordPalette::~CTabChordPalette()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( m_pIFramework )
	{
		m_pIFramework->Release();
	}
}

void CTabChordPalette::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabChordPalette)
	DDX_Control(pDX, IDC_SCALEKEYS, m_scalekeys);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabChordPalette, CPropertyPage)
	//{{AFX_MSG_MAP(CTabChordPalette)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_EXTENDOCTAVE, OnExtendoctave)
	ON_BN_CLICKED(IDC_HARMONICMINOR, OnHarmonicminor)
	ON_BN_CLICKED(IDC_MAJOR, OnMajor)
	ON_BN_CLICKED(IDC_MELODICMINOR, OnMelodicminor)
	ON_BN_CLICKED(IDC_SHIFTDOWN, OnShiftdown)
	ON_BN_CLICKED(IDC_SHIFTUP, OnShiftup)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabChordPalette message handlers

BEGIN_EVENTSINK_MAP(CTabChordPalette, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CTabChordPalette)
	ON_EVENT(CTabChordPalette, IDC_SCALE_KEYS, 1 /* NoteDown */, OnNoteDownScaleKeys, VTS_I2)
	ON_EVENT(CTabChordPalette, IDC_SCALE_KEYS, 2 /* NoteUp */, OnNoteUpScaleKeys, VTS_I2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/*
	short nOtherKey = (nKey + 12) % 24;
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
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
*/

void CTabChordPalette::OnNoteDownScaleKeys(short nKey) 
{
	// Need this AFX_MANAGE_STATE so that error
	// message string resource can be found
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	long mask = m_lScalePattern & (1 << nKey);
	// set to opposite state
	int state = mask ? 0 : 2;
	m_scalekeys.SetNoteState(nKey, "", static_cast<short>(state));
	if(state == 0)
		m_lScalePattern ^= mask;
	else
		m_lScalePattern |= (1 << nKey);
	m_pPageManager->UpdateObjectWithTabData();
	
}

void CTabChordPalette::OnNoteUpScaleKeys(short nKey) 
{
	// Need this AFX_MANAGE_STATE so that error
	// message string resource can be found
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(nKey);
	return;
//	m_scalekeys.AttachControlSite(this);
//	m_pPageManager->UpdateObjectWithTabData();
}

void CTabChordPalette::SetScale(long pattern)
{
	m_lScalePattern = pattern;
	for(short i = 0; i < 24; i++)
	{
		int state = m_lScalePattern & (1 << i) ? 2 : 0;
		m_scalekeys.SetNoteState(i, "", static_cast<short>(state));
	}
}

void CTabChordPalette::OnClear() 
{
	// Need this AFX_MANAGE_STATE so that error
	// message string resource can be found
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	SetScale(0);
	m_pPageManager->UpdateObjectWithTabData();
	
}

void CTabChordPalette::OnExtendoctave() 
{
	// Need this AFX_MANAGE_STATE so that error
	// message string resource can be found
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	long l = m_lScalePattern & 0xfff;
	l |= (l << 12);
	SetScale(l);

	m_pPageManager->UpdateObjectWithTabData();
}

void CTabChordPalette::OnHarmonicminor() 
{
	// Need this AFX_MANAGE_STATE so that error
	// message string resource can be found
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	long scale = 0x9AD9AD;
	scale = Rotate24(scale,  m_lKey);
	SetScale(scale);
	m_pPageManager->UpdateObjectWithTabData();
	
}

void CTabChordPalette::OnMajor() 
{
	// Need this AFX_MANAGE_STATE so that error
	// message string resource can be found
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	long scale = 0xAB5AB5;
	scale = Rotate24(scale,  m_lKey);
	SetScale(scale);
	m_pPageManager->UpdateObjectWithTabData();
	
}

void CTabChordPalette::OnMelodicminor() 
{
	// Need this AFX_MANAGE_STATE so that error
	// message string resource can be found
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	long scale = 0xAADAAD;
	scale = Rotate24(scale,  m_lKey);
	SetScale(scale);
	m_pPageManager->UpdateObjectWithTabData();
	
}

void CTabChordPalette::OnShiftdown() 
{
	// Need this AFX_MANAGE_STATE so that error
	// message string resource can be found
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	long scale = Rotate24(m_lScalePattern, -1);
	SetScale(scale);
	m_pPageManager->UpdateObjectWithTabData();
	
}

void CTabChordPalette::OnShiftup() 
{
	// Need this AFX_MANAGE_STATE so that error
	// message string resource can be found
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	long scale = Rotate24(m_lScalePattern, 1);
	SetScale(scale);

	m_pPageManager->UpdateObjectWithTabData();
	
}

BOOL CTabChordPalette::OnInitDialog() 
{
	// Need this AFX_MANAGE_STATE so that resource defined
	// combo box strings can be found
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_scalekeys.SetLButtonUpCallback(PaletteKeyboardChangeCallback, this);

	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CTabChordPalette::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

void CTabChordPalette::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

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

void CTabChordPalette::EnableControls(BOOL bEnable)
{
	UNREFERENCED_PARAMETER(bEnable);
}


BOOL CTabChordPalette::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_fInOnSetActive)
		return FALSE;

	// Set enable state of controls
	BOOL fEnable;

	if( m_fHaveData )
	{
		fEnable = TRUE;
	}
	else
	{
		fEnable = FALSE;
	}
	EnableControls( fEnable );

	// Place values in controls
	m_fInOnSetActive = TRUE;
	long x = m_lScalePattern;
//	m_scalekeys.Transpose(m_lKey, true);
	for(short i = 0; i < 24; i++)
	{
		if(x & 1)
			m_scalekeys.SetNoteState(i, "", 2);
		else
			m_scalekeys.SetNoteState(i, "", 0);
		x = x >> 1;
	}
	m_fInOnSetActive = FALSE;
	
	return CPropertyPage::OnSetActive();
	
}

/////////////////////////////////////////////////////////////////////////////
// CTabChordPalette::CopyDataToTab

void CTabChordPalette::CopyDataToTab( tabPersonality* pTabData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(pTabData)
	{
		m_lScalePattern = pTabData->lScalePattern;
		m_lKey = pTabData->dwKey;
		m_bUseFlats = CPersonality::GetFlatsFlag(m_lKey);
		DWORD key = m_lKey;
		CPersonality::SetFlatsFlag(false, key);
		m_lKey = key;
		m_lKey -= 12;	// transpose factor
//		m_lScalePattern = Rotate24(m_lScalePattern,  m_lKey);
		m_fHaveData = TRUE;
	}
	else
	{
		m_fHaveData = FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabChordPalette::GetDataFromTab

void CTabChordPalette::GetDataFromTab( tabPersonality* pTabData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(pTabData)
	{
		pTabData->lScalePattern = m_lScalePattern;
		// comment is intentional: m_lKey is read only, DO NOT PASS BACK TO PERSONALITY !
//		pTabData->dwKey = m_lKey;
	}	
}

void CTabChordPalette::PreProcessPSP(PROPSHEETPAGE & psp, BOOL bWizard)
{
	UNREFERENCED_PARAMETER(bWizard);
	PreProcessPageTemplate(psp, FALSE);
}

LRESULT CTabChordPalette::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{	
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return CPropertyPage::WindowProc(message, wParam, lParam);
}

BOOL CTabChordPalette::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CTabChordPalette::DispatchKeyboardChange(CKeyboard* pKeyboard, short nKey)
{
	if(pKeyboard == &m_scalekeys)
	{
		OnNoteDownScaleKeys(nKey);
	}
}