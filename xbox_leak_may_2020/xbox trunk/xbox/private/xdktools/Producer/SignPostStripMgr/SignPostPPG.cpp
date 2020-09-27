// SignPostPPG.cpp : Implementation of signpost property page.
//

#include "stdafx.h"
#include "SignPostStripMgr.h"
#include "SignPostPPG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSignPostPPG property page

IMPLEMENT_DYNCREATE(CSignPostPPG, CPropertyPage)

CSignPostPPG::CSignPostPPG() : CPropertyPage(CSignPostPPG::IDD)
{
	m_pPageManager = NULL;
	m_pSignPostData = NULL;
	//{{AFX_DATA_INIT(CSignPostPPG)
	//}}AFX_DATA_INIT
}

CSignPostPPG::~CSignPostPPG()
{
	if(m_pSignPostData != NULL)
	{
		delete m_pSignPostData;
	}
}

void CSignPostPPG::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSignPostPPG)
	DDX_Control(pDX, IDC_COMBO_SIGNPOST, m_SignPostDropDownList);
	DDX_Control(pDX, IDC_SIGNPOST_CADENCE, m_CadenceCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSignPostPPG, CPropertyPage)
	//{{AFX_MSG_MAP(CSignPostPPG)
	ON_BN_CLICKED(IDC_SIGNPOST_CADENCE, OnSignpostCadence)
	ON_CBN_SELCHANGE(IDC_COMBO_SIGNPOST, OnSelchangeComboSignpost)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSignPostPPG message handlers

void CSignPostPPG::OnSignpostCadence() 
{
	HRESULT hr;

	ASSERT(m_pSignPostData != NULL);
	if(m_pSignPostData == NULL)
	{
		return;
	}

	// Find out what the current state of the button is
	if(m_CadenceCheck.GetButtonStyle() == BS_AUTO3STATE)
	{
		// Now set the checkbox to be only 2-state
		m_CadenceCheck.SetButtonStyle(BS_AUTOCHECKBOX, FALSE);
		m_CadenceCheck.SetCheck(0);
		m_pSignPostData->dwSignPost &= ~SP_CADENCE;
		m_pSignPostData->dwValid |= VALID_CADENCE;
	}
	else
	{
		ASSERT(m_pSignPostData->dwValid & VALID_CADENCE);
		if(m_pSignPostData->dwSignPost & SP_CADENCE)
		{
			m_pSignPostData->dwSignPost &= ~SP_CADENCE;
			ASSERT(m_CadenceCheck.GetCheck() == 0);
		}
		else if(m_pSignPostData->dwSignPost)
		{
			m_pSignPostData->dwSignPost |= SP_CADENCE;
			ASSERT(m_CadenceCheck.GetCheck() == 1);
		}
	}

	if(m_pPageManager != NULL)
	{
		hr = m_pPageManager->m_pIPropPageObject->SetData(m_pSignPostData);
		ASSERT(SUCCEEDED(hr));
		if(S_FALSE == hr)
		{
			// The new data was rejected!
			PPGSignPost *pSignPost;

			hr = m_pPageManager->m_pIPropPageObject->GetData((void **) &pSignPost);
			ASSERT(SUCCEEDED(hr));
			hr = SetData(pSignPost);
			ASSERT(SUCCEEDED(hr));
		}
	}	
}

BOOL CSignPostPPG::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Fill the dropdown box.

	m_SignPostDropDownList.AddString("None");
	m_SignPostDropDownList.AddString("1");
	m_SignPostDropDownList.AddString("2");
	m_SignPostDropDownList.AddString("3");
	m_SignPostDropDownList.AddString("4");
	m_SignPostDropDownList.AddString("5");
	m_SignPostDropDownList.AddString("6");
	m_SignPostDropDownList.AddString("7");
	m_SignPostDropDownList.AddString("A");
	m_SignPostDropDownList.AddString("B");
	m_SignPostDropDownList.AddString("C");
	m_SignPostDropDownList.AddString("D");
	m_SignPostDropDownList.AddString("E");
	m_SignPostDropDownList.AddString("F");


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSignPostPPG::OnSetActive() 
{
	// Set the controls in case they have changed since this was last activated
	// and RefreshData has not yet been called.
	SetData(m_pSignPostData);

	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////
// CSignPostPPG additional functions

HRESULT CSignPostPPG::SetData(PPGSignPost *pSignPost)
{
	if(pSignPost == NULL)
	{
		// Disable the property page.
		if(IsWindow(m_SignPostDropDownList.m_hWnd))
		{
			m_SignPostDropDownList.EnableWindow(FALSE);
		}
		if(IsWindow(m_CadenceCheck.m_hWnd))
		{
			m_CadenceCheck.EnableWindow(FALSE);
		}
		return S_OK;
	}
	if(m_pSignPostData == NULL)
	{
		m_pSignPostData = new PPGSignPost;
		if(m_pSignPostData == NULL)
		{
			return E_OUTOFMEMORY;
		}
	}
	if(IsWindow(m_SignPostDropDownList.m_hWnd))
	{
		m_SignPostDropDownList.EnableWindow(TRUE);
	}
	if(IsWindow(m_CadenceCheck.m_hWnd))
	{
		m_CadenceCheck.EnableWindow(TRUE);
	}

	if(pSignPost != m_pSignPostData)
	{
		memcpy(m_pSignPostData, pSignPost, sizeof(PPGSignPost));
	}

	if(!(m_pSignPostData->dwValid & VALID_SIGNPOST))
	{
		// If we don't have a valid signpost, just don't select anything
		m_SignPostDropDownList.SetCurSel(-1);
	}
	else
	{
		// Otherwise go select the appropriate value in the listbox.
		int iIndex = -1;

		if(m_pSignPostData->dwSignPost & SP_ROOT)
		{
			switch(m_pSignPostData->dwSignPost & SP_ROOT)
			{
			case SP_1:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "1");
				break;
			case SP_2:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "2");
				break;
			case SP_3:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "3");
				break;
			case SP_4:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "4");
				break;
			case SP_5:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "5");
				break;
			case SP_6:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "6");
				break;
			case SP_7:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "7");
				break;
			default:
				return E_INVALIDARG;
				break;
			}
			ASSERT(iIndex > -1);
			m_SignPostDropDownList.SetCurSel(iIndex);
		}
		else if(m_pSignPostData->dwSignPost & SP_LETTER)
		{
			switch(m_pSignPostData->dwSignPost & SP_LETTER)
			{
			case SP_A:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "A");
				break;
			case SP_B:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "B");
				break;
			case SP_C:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "C");
				break;
			case SP_D:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "D");
				break;
			case SP_E:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "E");
				break;
			case SP_F:
				iIndex = m_SignPostDropDownList.FindStringExact(-1, "F");
				break;
			default:
				return E_INVALIDARG;
				break;
			}
			ASSERT(iIndex > -1);
			m_SignPostDropDownList.SetCurSel(iIndex);
		}
		else if(m_pSignPostData->dwSignPost == 0)
		{
			iIndex = m_SignPostDropDownList.FindStringExact(-1, "None");
			ASSERT(iIndex > -1);
			m_SignPostDropDownList.SetCurSel(iIndex);
			if(IsWindow(m_CadenceCheck.m_hWnd))
			{
				m_CadenceCheck.EnableWindow(FALSE);
			}
		}
		else
		{
			return E_INVALIDARG;
		}
	}
	if(!(m_pSignPostData->dwValid & VALID_CADENCE))
	{
		m_CadenceCheck.SetButtonStyle(BS_AUTO3STATE, FALSE);
		m_CadenceCheck.SetCheck(2);
	}
	else
	{
		m_CadenceCheck.SetCheck((m_pSignPostData->dwSignPost & SP_CADENCE) ? 1 : 0);
		m_CadenceCheck.SetButtonStyle(BS_AUTOCHECKBOX, FALSE);
	}

	return S_OK;
}


void CSignPostPPG::OnSelchangeComboSignpost() 
{
	ASSERT(m_pSignPostData != NULL);
	if(m_pSignPostData == NULL)
	{
		return;
	}
	
	HRESULT	hr;
	CString	strSignPost;
	int		iIndex;

	iIndex = m_SignPostDropDownList.GetCurSel();
	ASSERT(iIndex >= 0);

	m_SignPostDropDownList.GetLBText(iIndex, strSignPost);
	ASSERT(strSignPost.GetLength() > 0);
	
	// Set the signpost.
	if(strSignPost == "1")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_1);
	}
	else if(strSignPost == "2")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_2);
	}
	else if(strSignPost == "3")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_3);
	}
	else if(strSignPost == "4")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_4);
	}
	else if(strSignPost == "5")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_5);
	}
	else if(strSignPost == "6")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_6);
	}
	else if(strSignPost == "7")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_7);
	}
	else if(strSignPost == "A")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_A);
	}
	else if(strSignPost == "B")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_B);
	}
	else if(strSignPost == "C")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_C);
	}
	else if(strSignPost == "D")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_D);
	}
	else if(strSignPost == "E")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_E);
	}
	else if(strSignPost == "F")
	{
		m_pSignPostData->dwSignPost = ((m_pSignPostData->dwSignPost & SP_CADENCE) | SP_F);
	}
	else if(strSignPost == "None")
	{
		//m_pSignPostData->dwSignPost = (m_pSignPostData->dwSignPost & SP_CADENCE); //?cadence to nothing?
		m_pSignPostData->dwSignPost = 0;
	}
	else
	{
		ASSERT(FALSE);
		return;
	}
	if(IsWindow(m_CadenceCheck.m_hWnd))
	{
		m_CadenceCheck.EnableWindow(m_pSignPostData->dwSignPost != 0);
	}
	m_pSignPostData->dwValid |= VALID_SIGNPOST;

	if(m_pPageManager != NULL && m_pPageManager->m_pIPropPageObject != NULL)
	{
		hr = m_pPageManager->m_pIPropPageObject->SetData(m_pSignPostData);
		ASSERT(SUCCEEDED(hr));
		if(S_FALSE == hr)
		{
			// The new data was rejected!
			PPGSignPost *pSignPost;

			hr = m_pPageManager->m_pIPropPageObject->GetData((void **) &pSignPost);
			ASSERT(SUCCEEDED(hr));
			hr = SetData(pSignPost);
			ASSERT(SUCCEEDED(hr));
		}
	}	
}

