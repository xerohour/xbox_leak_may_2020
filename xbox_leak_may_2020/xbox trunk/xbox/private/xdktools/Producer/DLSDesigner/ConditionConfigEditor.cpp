// ConditionConfigEditor.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "DLSComponent.h"
#include "conditionconfigeditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConditionConfigEditor dialog


CConditionConfigEditor::CConditionConfigEditor(CWnd* pParent /*=NULL*/, CDLSComponent* pComponent)
	: CDialog(CConditionConfigEditor::IDD, pParent), m_pComponent(pComponent)
{
	ASSERT(pComponent);

	//{{AFX_DATA_INIT(CConditionConfigEditor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CConditionConfigEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConditionConfigEditor)
	DDX_Control(pDX, IDOK, m_OKButton);
	DDX_Control(pDX, IDC_VALUE_EDIT, m_ValueEdit);
	DDX_Control(pDX, IDC_QUERY_LIST, m_QueryList);
	DDX_Control(pDX, IDC_HELP_STATIC, m_HelpStatic);
	DDX_Control(pDX, IDC_CONFIG_NAME_EDIT, m_ConfigNameEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConditionConfigEditor, CDialog)
	//{{AFX_MSG_MAP(CConditionConfigEditor)
	ON_EN_KILLFOCUS(IDC_CONFIG_NAME_EDIT, OnKillfocusConfigNameEdit)
	ON_EN_KILLFOCUS(IDC_VALUE_EDIT, OnKillfocusValueEdit)
	ON_LBN_SELCHANGE(IDC_QUERY_LIST, OnSelchangeQueryList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Helpers
void CConditionConfigEditor::SetConfig(CSystemConfiguration* pConfig)
{
	ASSERT(pConfig);
	m_pConfig = pConfig;
}

void CConditionConfigEditor::InitQueryList()
{
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		return;
	}

	ASSERT(m_pConfig);
	if(m_pConfig == NULL)
	{
		return;
	}

	m_QueryList.ResetContent();

	// Add all the queries
	const CPtrList& lstDLSQueries = m_pComponent->GetListOfQueries();
	POSITION position = lstDLSQueries.GetHeadPosition();
	while(position)
	{
		CDLSQuery* pDLSQuery = (CDLSQuery*)lstDLSQueries.GetNext(position);
		ASSERT(pDLSQuery);
		if(pDLSQuery)
		{
			DWORD dwValue = 0;
			CString sQueryName = pDLSQuery->GetName();
			if(SUCCEEDED(m_pConfig->GetValueForQuery(pDLSQuery, dwValue)))
			{
				CString sValue;
				sValue.Format(" = %d", dwValue);
				sQueryName += sValue;
			}

			int nIndex = m_QueryList.AddString(sQueryName);
			if(nIndex != LB_ERR)
			{
				m_QueryList.SetItemDataPtr(nIndex, pDLSQuery);
			}
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
// CConditionConfigEditor message handlers

BOOL CConditionConfigEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ASSERT(m_pConfig);

	// Set the note
	CString sHelpText;
	sHelpText.LoadString(IDS_CONDITION_CONFIG_QUERYVALUENOTE);
	m_HelpStatic.SetWindowText(sHelpText);

	// Add the DLS Queries to the list box and set the strings to 
	// show the values if we have any values set for any of the queries
	InitQueryList();

	// Set the name of the configuration
	m_ConfigNameEdit.SetWindowText(m_pConfig->GetName());

	if(m_pConfig->IsDefault())
	{
		EnableControls(FALSE);
	}
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConditionConfigEditor::OnKillfocusConfigNameEdit() 
{
	CString sConfigName;
	m_ConfigNameEdit.GetWindowText(sConfigName);
	
	sConfigName.TrimLeft();
	sConfigName.TrimRight();

	if(sConfigName.IsEmpty())
	{
		return;
	}

	CSystemConfiguration* pConfig = m_pComponent->GetConfiguration(sConfigName);
	if(pConfig && pConfig != m_pConfig)
	{
		m_ConfigNameEdit.SetWindowText(m_pConfig->GetName());
		// Can't have multiple configs with the same name
		AfxMessageBox(IDS_ERR_CONFIG_NAME_CONFLICT);
		return;
	}

	m_pConfig->SetName(sConfigName);
}

void CConditionConfigEditor::OnOK() 
{
	ASSERT(m_pConfig);
	if(m_pConfig == NULL)
	{
		return;
	}

	// Make sure the name's updated
	OnKillfocusConfigNameEdit(); 

	CString sConfigName = m_pConfig->GetName();

	sConfigName.TrimLeft();
	sConfigName.TrimRight();

	if(sConfigName.IsEmpty())
	{
		// Can't have a config without a name
		AfxMessageBox(IDS_ERR_CONFIG_NO_NAME);
		return;
	}

	CDialog::OnOK();
}


void CConditionConfigEditor::OnKillfocusValueEdit() 
{
	CString sValue;
	m_ValueEdit.GetWindowText(sValue);

	int nIndex = m_QueryList.GetCurSel();
	if(nIndex != LB_ERR)
	{
		CDLSQuery* pQuery = (CDLSQuery*)m_QueryList.GetItemDataPtr(nIndex);
		ASSERT(pQuery);

		if(pQuery)
		{
			if(!sValue.IsEmpty())
			{
				DWORD dwValue = atoi(sValue);
				if(FAILED(m_pConfig->SetValueForQuery(pQuery, dwValue)))
				{
					// Adda Query-Value pair to the config
					CQueryValuePair* pQueryValuePair = new CQueryValuePair(pQuery, dwValue);
					if(pQueryValuePair)
					{
						m_pConfig->AddQueryValue(pQueryValuePair);
					}
				}
			}
			else
			{
				// Delete the query from the config
				m_pConfig->RemoveQueryValuePair(pQuery->GetGUID());
			}

			InitQueryList();
		}
	}
}


void CConditionConfigEditor::OnSelchangeQueryList() 
{
	int nIndex = m_QueryList.GetCurSel();
	if(nIndex != LB_ERR)
	{
		CDLSQuery* pQuery = (CDLSQuery*)m_QueryList.GetItemDataPtr(nIndex);
		ASSERT(pQuery);
		if(pQuery)
		{
			CString sValue = "";
			DWORD dwValue = 0;
			if(SUCCEEDED(m_pConfig->GetValueForQuery(pQuery, dwValue)))
			{
				sValue.Format("%d", dwValue);
				m_ValueEdit.SetWindowText(sValue);
			}
			else
			{
				// Clear the edit
				m_ValueEdit.SetWindowText(sValue);
			}
		}
	}
}


void CConditionConfigEditor::EnableControls(BOOL bEnable)
{
	m_ValueEdit.EnableWindow(bEnable);
	m_ConfigNameEdit.EnableWindow(bEnable);
	m_OKButton.EnableWindow(bEnable);
}
