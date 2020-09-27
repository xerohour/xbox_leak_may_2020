// custbtn.cpp : implementation file
//

#include "stdafx.h"
#include "vshell.h"
#include "custbtn.h"
#include "dockman.h"
#include "ids.h"
#include "barchoic.h"
#include "barglob.h"
#include "bardockx.h"
#include "prxycust.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomButtonDlg dialog

// This dialog is used to select a button image or a button name to be applied to a toolbar
// button.
CCustomButtonDlg::CCustomButtonDlg(CString buttonName, CString buttonText, APPEARANCE aAppearance, CDockManager *pManager, APPEARANCEOPTION aoCanText , APPEARANCEOPTION aoCanImage, HBITMAP hbmCurrent, int nIndexCurrent, CWnd* pParent /*=NULL*/) :
	C3dDialog(CCustomButtonDlg::IDD, pParent),
	m_aoCanImage(aoCanImage),
	m_aoCanText(aoCanText),
	m_hbmCustomGlyph(NULL),
	m_pButtonSource(NULL),
	m_pManager(pManager),
	m_nOldSelection(0),
	m_hbmCurrent(hbmCurrent),
	m_nIndexCurrent(nIndexCurrent)
{
	//{{AFX_DATA_INIT(CCustomButtonDlg)
	m_buttonName = buttonName;
	m_buttonText = buttonText;
	m_aAppearance = aAppearance;
	//}}AFX_DATA_INIT

	// It's possible for some of the above to start out in conflict - for example, the command could
	// be a menu (image only not allowed) being dragged onto a toolbar (image only the default)

	// something must be not forbidden, otherwise we shouldn't be in this dialog
	ASSERT(m_aoCanText!=cAppearanceForbidden || m_aoCanImage!=cAppearanceForbidden); 

	// fix up initial appearance to be consistent with options
	if(!GLOBAL_DATA::CanChooseImageText(m_aoCanText, m_aoCanImage))
	{
		if(m_aAppearance==cAppearanceImageText)
		{
			m_aAppearance=cAppearanceTextOnly;
		}
	}

	if(!GLOBAL_DATA::CanChooseTextOnly(m_aoCanText, m_aoCanImage))
	{
		if(m_aAppearance==cAppearanceTextOnly)
		{
			m_aAppearance=cAppearanceImageOnly;
		}
	}

	if(!GLOBAL_DATA::CanChooseImageOnly(m_aoCanText, m_aoCanImage))
	{
		if(m_aAppearance==cAppearanceImageOnly)
		{
			m_aAppearance=cAppearanceTextOnly;
		}
	}
}

CCustomButtonDlg::~CCustomButtonDlg()
{
}

// We use DDV_MaxChars here to limit the size of a toolbar button string. I can't see any
// sensible way to choose a limit, but it's clear that things could get unpleasant if the button
// were a lot larger than the screen, or if so many characters were entered that the registry
// data got unpleasantly long. So I choose 80, arbitrarily. This is less than 1 screen width,
// and less than the size of the data for 1 custom bitmap. Word chooses 256, which seems even 
// more unpleasant, and allows wierd buttons for small screens. martynl 4Apr96
void CCustomButtonDlg::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(sizeof(int)==sizeof(APPEARANCE));
	int appearance=(int)m_aAppearance;

	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomButtonDlg)
	DDX_Control(pDX, IDC_TOOLS, m_tools);
	DDX_Text(pDX, IDC_BUTTON_NAME, m_buttonName);
	DDX_Text(pDX, IDC_BUTTON_TEXT, m_buttonText);
	DDV_MaxChars(pDX, m_buttonText, 80);
	DDX_Radio(pDX, IDC_IMAGEONLY, appearance);
	//}}AFX_DATA_MAP

	m_aAppearance=(APPEARANCE)appearance;
}

BEGIN_MESSAGE_MAP(CCustomButtonDlg, C3dDialog)
	//{{AFX_MSG_MAP(CCustomButtonDlg)
	ON_EN_CHANGE(IDC_BUTTON_TEXT, OnChangeButtonText)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_IMAGE_PASTE, OnImagePaste)
	ON_BN_CLICKED(IDC_IMAGE_RESET, OnImageReset)
	ON_BN_CLICKED(IDC_IMAGEONLY, OnImageOnly)
	ON_BN_CLICKED(IDC_IMAGETEXT, OnImageText)
	ON_BN_CLICKED(IDC_TEXTONLY, OnTextOnly)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomButtonDlg message handlers
void CCustomButtonDlg::OnOK() 
{
	UpdateData(TRUE);

	// which button is selected?
	CToolCustomizer *pCustomizer=m_pButtonSource->GetCustomizer();

	CSize size=CCustomBar::GetDefaultBitmapSize(theApp.m_bLargeToolBars);

	if(m_aAppearance != cAppearanceTextOnly)
	{
		// duplicate the custom bitmap
		m_hbmCustomGlyph=GLOBAL_DATA::DuplicateBitmap(m_pButtonSource,
								m_pButtonSource->GetImageWell(), 
								size, CPoint(size.cx * (pCustomizer->m_nSelectIndex), 0));
	}
	else
	{
		m_hbmCustomGlyph=NULL;
	}

	C3dDialog::OnOK();
	delete pCustomizer;
	delete m_pButtonSource;
	m_pButtonSource=NULL;
}

void CCustomButtonDlg::OnCancel() 
{
	CToolCustomizer *pCustomizer=m_pButtonSource->GetCustomizer();
	pCustomizer->SetSelection(NULL,0);

	delete pCustomizer;
	delete m_pButtonSource;
	m_pButtonSource=NULL;

	C3dDialog::OnCancel();
}

#pragma optimize("", off)
BOOL CCustomButtonDlg::OnInitDialog() 
{
	C3dDialog::OnInitDialog();

	// get size of small button glyphs
	CSize customSize=CCustomBar::GetDefaultBitmapSize(theApp.m_bLargeToolBars);

	// These assertions would fail if the bitmap wasn't present in the resource file
	HINSTANCE hInstance = AfxFindResourceHandle(MAKEINTRESOURCE(theApp.m_bLargeToolBars ? IDB_CUSTOMGLYPHSLARGE : IDB_CUSTOMGLYPHS), RT_BITMAP);
	ASSERT( hInstance != NULL );
	HRSRC hRsrc = ::FindResource(hInstance,	MAKEINTRESOURCE(theApp.m_bLargeToolBars ? IDB_CUSTOMGLYPHSLARGE : IDB_CUSTOMGLYPHS), RT_BITMAP);
	HBITMAP hbmCustomWell=AfxLoadSysColorBitmap(hInstance, hRsrc);

	ASSERT(hbmCustomWell!=NULL);

	BITMAP bmpCustom;

	// This should only fail if GDI is in confusion
	VERIFY(GetObject(hbmCustomWell, sizeof(BITMAP), &bmpCustom));

	// Check height of bitmap
	ASSERT(bmpCustom.bmHeight==customSize.cy);

	// Check no slack at end of bitmap
	ASSERT((bmpCustom.bmWidth % customSize.cx)==0);

	// Count glyphs
	int nGlyphs=bmpCustom.bmWidth / customSize.cx;

	CRect rectTools;
	m_tools.GetWindowRect(rectTools);
	ScreenToClient(rectTools);
	
	m_pButtonSource = new CChoiceBar;
	if (!m_pButtonSource->Create(WS_VSCROLL , rectTools, this, 0))
	{
		MessageBeep(0);
		OnCancel();
		return FALSE;
	}

	int iButton=0;

	// Insert the buttons
	{
		TRANSFER_TBBUTTON trans(ID_APP_ABOUT);
		for (int i = 0; i < nGlyphs ; i++)
		{
			m_pButtonSource->InsertButton(iButton++,trans, FALSE, gapNil);
		}
	}

	// inhibit dragging
	m_pButtonSource->m_dwStyle = (m_pButtonSource->m_dwStyle | CTBRS_NODRAG) & ~(CBRS_TOOLTIPS | CBRS_FLYBY);
	m_pButtonSource->EnableToolTips(FALSE);

	// setup correct glyphs
	m_pButtonSource->SetBitmap(hbmCustomWell);

	// Now insert the current glyph at the start, if it's extant
	if(m_hbmCurrent!=NULL && m_nIndexCurrent!=-1)
	{
		TRANSFER_TBBUTTON trans(ID_APP_ABOUT, m_hbmCurrent, m_nIndexCurrent, theApp.m_bLargeToolBars);
		m_pButtonSource->InsertButton(0, trans, FALSE, gapNil);
	}

	// layout bar
	m_pButtonSource->SetParent(this);
	m_pButtonSource->RecalcLayout();
	m_pButtonSource->SetWindowPos(	&wndTop, rectTools.left, rectTools.top, rectTools.Width(), 
									rectTools.Height(), SWP_NOACTIVATE);

	// give it a customizer
	m_pButtonSource->SetCustomizer(new CToolCustomizer(((CMainFrame*) AfxGetMainWnd())->m_pManager,	NULL, m_pButtonSource->m_aUsage, FALSE));

	// if relevant, set up scroll bar info
	SCROLLINFO si;

	si.cbSize=sizeof(SCROLLINFO);
	si.fMask=SIF_POS | SIF_RANGE;
	si.nMin=si.nMax=0;
	si.nPos=0;
	si.nTrackPos=0;

	CSize statSize=rectTools.Size();
	statSize.cx-=::GetSystemMetrics(SM_CXVSCROLL);
	CSize barFit=m_pButtonSource->GetSize(HTRIGHT, statSize);

	if(rectTools.Size().cy < barFit.cy) {
		// scroll bar will be required
		si.nMax=barFit.cy-rectTools.Size().cy;
	}
	m_pButtonSource->SetScrollInfo(SB_VERT,&si);

	// show the toolbar
	m_pButtonSource->ShowWindow(SW_SHOWNOACTIVATE);
	
	// set up reset button correctly
	BOOL bEnable=FALSE;

	// get the customizer for the source button
	CToolCustomizer *pCustomizer=CASBar::s_pCustomizer;
	if(	pCustomizer->m_pSelectBar!=NULL)
	{
		bEnable=pCustomizer->m_pSelectBar->CanResetImage();
	}

	CButton *pReset=(CButton *)GetDlgItem(IDC_IMAGE_RESET);
	ASSERT(pReset!=NULL);
	pReset->EnableWindow(bEnable);
	
	// select the first button to start with
	m_pButtonSource->GetCustomizer()->SetSelection(m_pButtonSource->GetProxy(),0);

	// add ourselves to the clipboard chain
	m_hWndNextClip=SetClipboardViewer();
	
	// something must be not forbidden, otherwise we shouldn't be in this dialog
	ASSERT(m_aoCanText!=cAppearanceForbidden || m_aoCanImage!=cAppearanceForbidden); 

	// disable relevant choice buttons, depending on appearance options
	if(!GLOBAL_DATA::CanChooseImageText(m_aoCanText, m_aoCanImage))
	{
		CButton *pImageText=(CButton *)GetDlgItem(IDC_IMAGETEXT);
		ASSERT(pImageText!=NULL);
		pImageText->EnableWindow(FALSE);
	}

	if(!GLOBAL_DATA::CanChooseTextOnly(m_aoCanText, m_aoCanImage))
	{
		CButton *pTextOnly=(CButton *)GetDlgItem(IDC_TEXTONLY);
		ASSERT(pTextOnly!=NULL);
		pTextOnly->EnableWindow(FALSE);
	}

	if(!GLOBAL_DATA::CanChooseImageOnly(m_aoCanText, m_aoCanImage))
	{
		// must be not image - can't be both
		CButton *pImageOnly=(CButton *)GetDlgItem(IDC_IMAGEONLY);
		ASSERT(pImageOnly!=NULL);
		pImageOnly->EnableWindow(FALSE);
	}

	// ensure that the assign button is correctly enabled
	UpdateControls();

	// ensure that the paste button is updated correctly
	OnDrawClipboard();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
#pragma optimize("", on)

void CCustomButtonDlg::UpdateControls()
{
	UpdateData();

	BOOL bBarEnabled=TRUE;
	BOOL bTextEnabled=TRUE;
	BOOL bBarWasEnabled=FALSE;

	if(	m_aoCanImage==cAppearanceForbidden ||
		(m_aoCanImage==cAppearanceOptional && m_aAppearance==cAppearanceTextOnly))
	{
		bBarEnabled=FALSE;
	}

	if(	m_aoCanText==cAppearanceForbidden ||
		(m_aoCanText==cAppearanceOptional && m_aAppearance==cAppearanceImageOnly))
	{
		bTextEnabled=FALSE;
	}

	bBarWasEnabled=m_pButtonSource->IsWindowEnabled();

	if(bBarWasEnabled!=bBarEnabled)
	{
		// enable bar and group as appropriate
		m_pButtonSource->EnableWindow(bBarEnabled);

		if(!bBarEnabled)
		{
			m_nOldSelection=m_pButtonSource->GetCustomizer()->m_nSelectIndex;
			m_pButtonSource->GetCustomizer()->SetSelection(NULL,0);

			// Iterate over bar, making all of the buttons TBBS_DISABLED
			int nButtons=m_pButtonSource->GetCount();
			for(int iButton=0;iButton<nButtons;++iButton)
			{
				m_pButtonSource->SetButtonStyle(iButton, m_pButtonSource->GetButtonStyle(iButton)|TBBS_DISABLED);
			}
			m_pButtonSource->Invalidate();
		}
		else
		{
			m_pButtonSource->GetCustomizer()->SetSelection(m_pButtonSource->GetProxy(),m_nOldSelection);

			// Iterate over bar, making all of the buttons enabled
			int nButtons=m_pButtonSource->GetCount();
			for(int iButton=0;iButton<nButtons;++iButton)
			{
				m_pButtonSource->SetButtonStyle(iButton, m_pButtonSource->GetButtonStyle(iButton)&(~TBBS_DISABLED));
			}
			m_pButtonSource->Invalidate();
		}
	}

	CWnd *pButtonGroup=GetDlgItem(IDC_IMAGES_GROUP);
	ASSERT(pButtonGroup!=NULL);
	pButtonGroup->EnableWindow(bBarEnabled);

	// sort out text input
	CWnd *pTextPrompt=GetDlgItem(IDC_BUTTON_TEXT_PROMPT);
	ASSERT(pTextPrompt!=NULL);
	pTextPrompt->EnableWindow(bTextEnabled);

	CWnd *pText=GetDlgItem(IDC_BUTTON_TEXT);
	ASSERT(pText!=NULL);
	pText->EnableWindow(bTextEnabled);

	// get the controls we need
	CButton *pAssign=(CButton *)GetDlgItem(IDOK);
	ASSERT(pAssign!=NULL);

	if(pText!=NULL && pAssign!=NULL) 
	{
		// enable if it's not empty, or we don't care about text
		BOOL bAssignEnabled=!m_buttonText.IsEmpty() ||
							!bTextEnabled;

		pAssign->EnableWindow(bAssignEnabled);

		// ensure that the default button isn't disabled
		DWORD def=GetDefID();
		if(HIWORD(def)==DC_HASDEFID)
		{
			if(!bAssignEnabled)
			{
				if(LOWORD(def)==IDOK)
				{
					SetDefID(IDCANCEL);
				}
			}
			else
			{
				if(LOWORD(def)==IDCANCEL)
				{
					SetDefID(IDOK);
				}
			}
		}

	}
}

void CCustomButtonDlg::OnChangeButtonText() 
{
	UpdateControls();
}

void CCustomButtonDlg::OnDestroy() 
{
	C3dDialog::OnDestroy();

	// remove ourselves from the clipboard chain
	ChangeClipboardChain(m_hWndNextClip);
}

void CCustomButtonDlg::OnChangeCbChain( HWND hWndRemove, HWND hWndAfter )
{
	if(hWndRemove==m_hWndNextClip)
	{
		m_hWndNextClip=hWndAfter;
	}
	else
	{
		if(m_hWndNextClip!=NULL)
		{
			::SendMessage(m_hWndNextClip,WM_CHANGECBCHAIN, (WPARAM)hWndRemove, (LPARAM)hWndAfter);
		}
	}
}

void CCustomButtonDlg::OnDrawClipboard( )
{
	BOOL bEnable=FALSE;
	int nOurSelection; // index of selection inside this dialog

	CToolCustomizer *pCustomizer=CASBar::s_pCustomizer;

	// update the paste button as appropriate
	if(	pCustomizer->m_pSelectBar!=NULL)
	{
		bEnable=pCustomizer->m_pSelectBar->CanPasteImage();
	}

	CButton *pPaste=(CButton *)GetDlgItem(IDC_IMAGE_PASTE);
	ASSERT(pPaste!=NULL);
	pPaste->EnableWindow(bEnable);
}

void CCustomButtonDlg::OnImagePaste() 
{
	OnCancel();

	CToolCustomizer *pCustomizer=CASBar::s_pCustomizer;
	if(pCustomizer->m_pSelectBar!=NULL)
	{
		ASSERT(pCustomizer->m_pSelectBar->CanPasteImage());
		pCustomizer->m_pSelectBar->PasteButtonImage();
	}
}

void CCustomButtonDlg::OnImageReset() 
{
	OnCancel();

	CToolCustomizer *pCustomizer=CASBar::s_pCustomizer;
	if(pCustomizer->m_pSelectBar!=NULL)
	{
		pCustomizer->m_pSelectBar->ResetButtonImage();
	}
}

void CCustomButtonDlg::OnImageOnly() 
{
	UpdateControls();
}

void CCustomButtonDlg::OnImageText() 
{
	UpdateControls();
}

void CCustomButtonDlg::OnTextOnly() 
{
	UpdateControls();
}
