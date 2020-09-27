// conditionconfigtoolbar.cpp : implementation file
//

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "dlsdesigner.h"
#include "resource.h"
#include "conditionconfigtoolbar.h"
#include "ConditionConfigEditor.h"
#include "ConditionalChunk.h"
#include "DLSComponent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConditionConfigToolbar

CConditionConfigToolbar::CConditionConfigToolbar(CDLSComponent* pComponent) : CToolBarCtrl(),
m_dwRef(0), 
m_pComponent(pComponent),
m_pCurrentConfig(NULL)
{
	ASSERT(pComponent);
	m_pPopupMenu = new CMenu();
}

CConditionConfigToolbar::~CConditionConfigToolbar()
{
	m_pCurrentConfig = NULL;
	delete m_pPopupMenu;
}


BEGIN_MESSAGE_MAP(CConditionConfigToolbar, CToolBarCtrl)
	//{{AFX_MSG_MAP(CConditionConfigToolbar)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



// IUnknown implementation
HRESULT CConditionConfigToolbar::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if(::IsEqualIID(riid, IID_IDMUSProdToolBar) || ::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = (IDMUSProdToolBar*)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CConditionConfigToolbar::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CConditionConfigToolbar::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}

// IDMUSProdToolbar methods
HRESULT CConditionConfigToolbar::GetInfo(HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( (phWndOwner == NULL)
	||	(phInstance == NULL)
	||	(pnResourceId == NULL)
	||	(pbstrTitle == NULL) )
	{
		return E_POINTER;
	}

	*phWndOwner = m_hWnd;
	*phInstance = theApp.m_hInstance;
	*pnResourceId = (UINT)-1;  // We are going to build the toolbar from scratch

	CString strTitle;
	strTitle.LoadString(IDS_CONDITION_CONFIG_TOOLBAR_TITLE);
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}

HRESULT CConditionConfigToolbar::GetMenuText(BSTR* pbstrText)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( pbstrText == NULL )
	{
		return E_POINTER;
	}

	CString strText;
	strText.LoadString(IDS_CONDITION_CONFIG_TOOLBAR_MENUTEXT);
	*pbstrText = strText.AllocSysString();
	
	return S_OK;
}

HRESULT CConditionConfigToolbar::GetMenuHelpText(BSTR* pbstrHelpText)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( pbstrHelpText == NULL )
	{
		return E_POINTER;
	}

	CString strHelpText;
	strHelpText.LoadString(IDS_CONDITION_CONFIG_TOOLBAR_HELP);
	*pbstrHelpText = strHelpText.AllocSysString();

	return S_OK;
}

HRESULT CConditionConfigToolbar::Initialize(HWND hWndToolBar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(hWndToolBar == NULL)
	{
		return E_INVALIDARG;
	}

	CToolBarCtrl* pToolBarCtrl = new CToolBarCtrl;
	if(pToolBarCtrl == NULL)
	{
		return E_OUTOFMEMORY;
	}

	pToolBarCtrl->Attach(hWndToolBar);
	pToolBarCtrl->AddBitmap(2, IDB_CONDITION_CONFIG_TOOLBAR);

	CClientDC* pDC = new CClientDC(pToolBarCtrl);

	int nHeight = -((pDC->GetDeviceCaps(LOGPIXELSY) * 8) / 72);

	CString strFontName;
	if( strFontName.LoadString(IDS_TOOLBAR_FONT ) == 0)
	{
		strFontName = CString("MS Sans Serif");
	}
	m_font.CreateFont(nHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0, 
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, strFontName);

	CFont* pOldFont = pDC->SelectObject( &m_font );

	TEXTMETRIC tm;
	pDC->GetTextMetrics( &tm );
	int cxChar = 30 * (tm.tmAveCharWidth);
	int cyChar = 10 * (tm.tmHeight + tm.tmExternalLeading);

	pDC->SelectObject( pOldFont );
	delete pDC;


	TBBUTTON button1;
	button1.dwData = 0;
	button1.iString = NULL;
	button1.iBitmap = cxChar;	// Width
	button1.idCommand = IDC_CONDITION_CONFIG_COMBO;
	button1.fsState = TBSTATE_ENABLED;
	button1.fsStyle = TBSTYLE_SEP;
	pToolBarCtrl->InsertButton(0, &button1);

	CRect rect;
	pToolBarCtrl->GetItemRect( 0, &rect );
	rect.bottom = rect.top + cyChar;

	if(!m_ConfigsCombo.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS |  CBS_SORT, 
							rect, pToolBarCtrl, IDC_CONDITION_CONFIG_COMBO))
	{
		return E_FAIL;
	}

	CString sNone;
	sNone.LoadString(IDS_NONE);
	m_ConfigsCombo.SetFont(&m_font);
	m_ConfigsCombo.AddString(sNone);
	m_ConfigsCombo.SelectString(0, sNone);

	// The Add button
	TBBUTTON button2;
	button2.dwData = 0;
	button2.iString = NULL;
	button2.iBitmap = 0;
	button2.idCommand = IDC_CONDITION_CONFIG_ADD_BUTTON;
	button2.fsState = TBSTATE_ENABLED;
	button2.fsStyle = TBSTYLE_BUTTON;
	pToolBarCtrl->InsertButton( 1, &button2 );

	pToolBarCtrl->GetItemRect(1, &rect);
	rect.bottom = rect.top + (long)(1.8 * (cyChar / 10));

	// The Delete button
	TBBUTTON button3;
	button3.dwData = 0;
	button3.iString = NULL;
	button3.iBitmap = 1;	
	button3.idCommand = IDC_CONDITION_CONFIG_DELETE_BUTTON;
	button3.fsState = TBSTATE_ENABLED;
	button3.fsStyle = TBSTYLE_BUTTON;
	pToolBarCtrl->InsertButton(2, &button3);

	pToolBarCtrl->GetItemRect(2, &rect);
	rect.bottom = rect.top + (long)(1.8 * (cyChar / 10));


	// The Edit button
	TBBUTTON button4;
	button4.dwData = 0;
	button4.iString = NULL;
	button4.iBitmap = 2;
	button4.idCommand = IDC_CONDITION_CONFIG_EDIT_BUTTON;
	button4.fsState = TBSTATE_ENABLED;
	button4.fsStyle = TBSTYLE_BUTTON;
	pToolBarCtrl->InsertButton(3, &button4);

	pToolBarCtrl->GetItemRect(3, &rect);
	rect.bottom = rect.top + (long)(1.8 * (cyChar / 10));

	if( pToolBarCtrl )
	{
		pToolBarCtrl->Detach();
		delete pToolBarCtrl;
	}

	m_pPopupMenu->LoadMenu(IDM_CONFIG_TOOLBAR_RMENU);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CConditionConfigToolbar message handlers

void CConditionConfigToolbar::RefreshConfigCombo(CPtrList* pConfigList)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nCurSel = m_ConfigsCombo.GetCurSel();
	m_ConfigsCombo.ResetContent();

	ASSERT(pConfigList);
	if(pConfigList == NULL)
	{
		return;
	}

	POSITION position = pConfigList->GetHeadPosition();
	while(position)
	{
		CSystemConfiguration* pConfig = (CSystemConfiguration*) pConfigList->GetNext(position);
		if(pConfig)
		{
			CString sConfigName = pConfig->GetName();
			if(m_ConfigsCombo.FindStringExact(0, sConfigName) == CB_ERR)
			{
				int nIndex = m_ConfigsCombo.AddString(sConfigName);
				if(nIndex != CB_ERR)
				{
					m_ConfigsCombo.SetItemDataPtr(nIndex, pConfig);
				}
			}
		}
	}

	m_ConfigsCombo.SetCurSel(nCurSel);
}


void CConditionConfigToolbar::OnSelchangeCombo()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return;
	}

	int nIndex = m_ConfigsCombo.GetCurSel();
	if(nIndex != CB_ERR)
	{
		CSystemConfiguration* pConfig = (CSystemConfiguration*)m_ConfigsCombo.GetItemDataPtr(nIndex);
		ASSERT(pConfig);
		if(pConfig)
		{
			m_pCurrentConfig = pConfig;
			m_pComponent->OnConditionConfigChanged(m_pCurrentConfig, true);
		}
	}

	return;
}


void CConditionConfigToolbar::OnAddButtonClicked()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return;
	}

	CSystemConfiguration* pConfig = new CSystemConfiguration();
	CConditionConfigEditor configEditor(NULL, m_pComponent);
	configEditor.SetConfig(pConfig);
	if(configEditor.DoModal() == IDOK)
	{
		m_pComponent->AddConfiguration(pConfig);
		m_pComponent->RefreshConfigToolbar();
		SetCurrentConfig(pConfig);
	}
	else
	{
		delete pConfig;
	}
}


void CConditionConfigToolbar::OnDeleteButtonClicked()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return;
	}
	
	CString sNone;
	sNone.LoadString(IDS_NONE);
	CString sConfigName;
	m_ConfigsCombo.GetWindowText(sConfigName);
	if(sConfigName == sNone)
	{
		return;
	}

	int nIndex = m_ConfigsCombo.GetCurSel();
	if(nIndex != CB_ERR)
	{
		CSystemConfiguration* pConfig = (CSystemConfiguration*)m_ConfigsCombo.GetItemDataPtr(nIndex);
		ASSERT(pConfig);
		if(pConfig == NULL)
		{
			return;
		}

		if(pConfig->IsDefault())
		{
			AfxMessageBox(IDS_ERR_DEFAULT_CONFIG_DELETE);
			return;
		}

		m_ConfigsCombo.DeleteString(nIndex);
		m_pComponent->DeleteConfiguration(pConfig);

		// Set the first config as the current config
		m_ConfigsCombo.SetCurSel(0);
		m_pCurrentConfig = (CSystemConfiguration*) m_ConfigsCombo.GetItemDataPtr(0);
		ASSERT(m_pCurrentConfig);
		m_pComponent->OnConditionConfigChanged(m_pCurrentConfig, true);
	}
}



void CConditionConfigToolbar::OnEditButtonClicked()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return;
	}

	ASSERT(m_pCurrentConfig);
	if(m_pCurrentConfig == NULL)
	{
		return;
	}

	CString sNone;
	sNone.LoadString(IDS_NONE);
	if(m_pCurrentConfig->GetName() == sNone)
	{
		return;
	}

	CConditionConfigEditor configEditor(NULL, m_pComponent);
	configEditor.SetConfig(m_pCurrentConfig);
	if(configEditor.DoModal() == IDOK)
	{
		m_pComponent->RefreshConfigToolbar();
	}
}


BOOL CConditionConfigToolbar::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// If this is a control
	if(lParam != 0)
	{
		WORD wID = LOWORD(wParam);
		WORD wCommand = HIWORD(wParam);
		if(wID == IDC_CONDITION_CONFIG_COMBO && wCommand == CBN_SELCHANGE)
		{
			OnSelchangeCombo();
			return TRUE;
		}

		if(wID == IDC_CONDITION_CONFIG_ADD_BUTTON && wCommand == BN_CLICKED)
		{
			OnAddButtonClicked();
			return TRUE;
		}

		
		if(wID == IDC_CONDITION_CONFIG_DELETE_BUTTON && wCommand == BN_CLICKED)
		{
			OnDeleteButtonClicked();
			return TRUE;
		}


		if(wID == IDC_CONDITION_CONFIG_EDIT_BUTTON && wCommand == BN_CLICKED)
		{
			OnEditButtonClicked();
			return TRUE;
		}
	}

	return CToolBarCtrl::OnCommand(wParam, lParam);
}

CSystemConfiguration* CConditionConfigToolbar::GetCurrentConfig()
{
	return m_pCurrentConfig;
}



void CConditionConfigToolbar::SetCurrentConfig(CSystemConfiguration* pConfig)
{
	// We might be specifically setting the current config to NULL
	// when the configurations get deleted in the component
	if(pConfig)
	{
		CString sConfigName = pConfig->GetName();
		int nIndex = m_ConfigsCombo.FindStringExact(0, sConfigName);
		if(nIndex != CB_ERR)
		{
			m_ConfigsCombo.SelectString(0, sConfigName);
			m_pCurrentConfig = (CSystemConfiguration*)m_ConfigsCombo.GetItemDataPtr(nIndex);
		}
	}
	else
	{
		m_pCurrentConfig = NULL;
	}
}

void CConditionConfigToolbar::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if(m_pPopupMenu)
	{
		CMenu* pPopupMenu = m_pPopupMenu->GetSubMenu(0);
		pPopupMenu->TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, this, NULL);	
	}
}

BOOL CConditionConfigToolbar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	/*NMHDR* pnmhdr = (NMHDR*)lParam;
	if(pnmhdr->code == NM_RCLICK)
	{
		POINT point;
		GetCursorPos(&point);
		OnContextMenu(this, point);
	}*/
	
	return CToolBarCtrl::OnNotify(wParam, lParam, pResult);
}
