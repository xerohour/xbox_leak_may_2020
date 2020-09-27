// newparameterdialog.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "SegmentDesigner.h"
#include "SegmentIO.h"
#include "audiopathdesigner.h"
#include "ToolGraphDesigner.h"
#include "TrackMgr.h"
#include "TrackObject.h"
#include "newparameterdialog.h"
#include "conductor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


StageInfo CNewParameterDialog::m_arrToolsStages[MAX_STAGES_TOOLS] = {StageInfo(DMUS_PATH_SEGMENT_TOOL, IDS_TOOLS_SEGMENT), StageInfo(DMUS_PATH_AUDIOPATH_TOOL, IDS_TOOLS_SEGMENT_AUDIOPATH), StageInfo(DMUS_PATH_PERFORMANCE_TOOL, IDS_TOOLS_PERFORMANCE_AUDIOPATH_TOOLGRAPH)};
StageInfo CNewParameterDialog::m_arrDMOStages[MAX_STAGES_DMOS] = {StageInfo(DMUS_PATH_BUFFER_DMO, IDS_DMOS_SEGMENT_AUDIOPATH), StageInfo(DMUS_PATH_PERFORMANCE, IDS_DMOS_PERFORMANCE_AUDIOPATH)};

CDMToolInfo::CDMToolInfo(DMUSProdToolInfo toolInfo, IUnknown* pIUnkTool)
{
	CopyMemory(&m_ToolInfo, &toolInfo, sizeof(DMUSProdToolInfo));
	m_pIUnkTool = pIUnkTool;
}	

CDMToolInfo::~CDMToolInfo()
{
	if(m_pIUnkTool)
	{
		m_pIUnkTool->Release();
		m_pIUnkTool = NULL;
	}
}


CDMOInfo::CDMOInfo(DMUSProdDMOInfo dmoInfo)
{
	CopyMemory(&m_DMOInfo, &dmoInfo, sizeof(DMUSProdDMOInfo));
}


bool CNewParameterDialog::m_bParamsForTools = false;
UINT CNewParameterDialog::m_nLastSelectedStageIndex = 0; 
UINT CNewParameterDialog::m_nLastSelectedObjectIndex = 0; 
UINT CNewParameterDialog::m_nLastSelectedParamIndex = 0;


/////////////////////////////////////////////////////////////////////////////
// CNewParameterDialog dialog

CNewParameterDialog::CNewParameterDialog(CTrackMgr* pTrackMgr) : CDialog(CNewParameterDialog::IDD, NULL), 
m_pTrackMgr(pTrackMgr), 
m_pISegment(NULL) 
{
	//{{AFX_DATA_INIT(CNewParameterDialog)
	//}}AFX_DATA_INIT

	ASSERT(pTrackMgr);
}


void CNewParameterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewParameterDialog)
	DDX_Control(pDX, IDOK, m_OKButton);
	DDX_Control(pDX, IDC_NEWOBJECT_SPIN_PCHANNEL, m_PChannelSpin);
	DDX_Control(pDX, IDC_NEWOBJECT_EDIT_PCHANNEL, m_PChannelEdit);
	DDX_Control(pDX, IDC_EDIT_PCHNAME, m_PChannelNameEdit);
	DDX_Control(pDX, IDC_COMBO_OBJECT, m_ObjectCombo);
	DDX_Control(pDX, IDC_COMBO_PARAM, m_ParamCombo);
	DDX_Control(pDX, IDC_COMBO_STAGE, m_StageCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewParameterDialog, CDialog)
	//{{AFX_MSG_MAP(CNewParameterDialog)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT, OnSelchangeComboObject)
	ON_CBN_SELCHANGE(IDC_COMBO_STAGE, OnSelchangeComboStage)
	ON_BN_CLICKED(IDC_RADIO_TOOLS, OnRadioTools)
	ON_BN_CLICKED(IDC_RADIO_DMOS, OnRadioDMOs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewParameterDialog message handlers

BOOL CNewParameterDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_pISegment = m_pTrackMgr->GetSegment();
	ASSERT(m_pISegment);

	// Show the parameters for tools by default
	if(m_bParamsForTools)
	{
		InitializeToolsControls();
	}
	else
	{
		InitializeDMOControls();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewParameterDialog::InitializeToolsControls()
{
	CButton* pToolRadio = (CButton*) GetDlgItem(IDC_RADIO_TOOLS);
	if(pToolRadio)
	{
		pToolRadio->SetCheck(1);
	}

	CButton* pDMORadio = (CButton*)GetDlgItem(IDC_RADIO_DMOS);
	if(pDMORadio)
	{
		pDMORadio->SetCheck(0);
	}

	InitializeToolsStageCombo();

	if(SUCCEEDED(InitializeToolsObjectCombo()))
	{
		InitializeToolsParameterCombo();
	}

	RefreshPChannelText();
}

void CNewParameterDialog::OnDestroy() 
{
	CDialog::OnDestroy();

	CleanObjectList();
	CleanParamsList();
	
	if(m_pISegment)
	{
		m_pISegment->Release();
	}
	
}


void CNewParameterDialog::InitializeToolsStageCombo()
{
	m_StageCombo.ResetContent();

	for(int nIndex = 0; nIndex < MAX_STAGES_TOOLS; nIndex++)
	{
		CString sStageName;
		sStageName.LoadString(m_arrToolsStages[nIndex].m_uStageName);
		m_StageCombo.AddString(sStageName);				
	}

	m_StageCombo.SetCurSel(m_nLastSelectedStageIndex);

	RefreshPChannelText();
}


HRESULT CNewParameterDialog::InitializeToolsObjectCombo()
{
	int nSelectedStage = m_StageCombo.GetCurSel();
	if(nSelectedStage == CB_ERR)
	{
		return E_FAIL;
	}
	
	CleanObjectList();	
	CleanParamsList();

	DWORD dwStage = m_arrToolsStages[nSelectedStage].m_dwStage;
	
	m_OKButton.EnableWindow(TRUE);
	m_ObjectCombo.EnableWindow(TRUE);
	m_ParamCombo.EnableWindow(TRUE);

	IDMUSProdConductor* pIConductor = m_pTrackMgr->GetConductor();
	ASSERT(pIConductor);
	if(pIConductor == NULL)
	{
		return E_FAIL;
	}

	CPtrList lstObjects;
	if(FAILED(GetToolList(m_pISegment, pIConductor, dwStage, &lstObjects)))
	{
		m_ObjectCombo.EnableWindow(FALSE);
		m_ParamCombo.EnableWindow(FALSE);
		m_OKButton.EnableWindow(FALSE);
		pIConductor->Release();
		return E_FAIL;
	}

	pIConductor->Release();

	if(lstObjects.IsEmpty())
	{
		m_ObjectCombo.EnableWindow(FALSE);
		m_ParamCombo.EnableWindow(FALSE);
		m_OKButton.EnableWindow(FALSE);
		return E_FAIL;
	}

	POSITION position = lstObjects.GetHeadPosition();
	while(position)
	{
		CDMToolInfo* pToolInfo = (CDMToolInfo*)lstObjects.GetNext(position);
		ASSERT(pToolInfo);
		if(pToolInfo)
		{
			CString sToolName = pToolInfo->m_ToolInfo.awchToolName;
			int nInsertionIndex = m_ObjectCombo.AddString(sToolName);
			if(nInsertionIndex != CB_ERR)
			{
				if(m_ObjectCombo.SetItemDataPtr(nInsertionIndex, pToolInfo) == CB_ERR)
				{
					m_ObjectCombo.DeleteString(nInsertionIndex);
					return E_FAIL;
				}
			}
		}
	}

	m_ObjectCombo.SetCurSel(m_nLastSelectedObjectIndex);

	RefreshPChannelText();

	return S_OK;
}


void CNewParameterDialog::InitializeToolsParameterCombo()
{
	CleanParamsList();

	int nSelectedToolIndex = m_ObjectCombo.GetCurSel();
	if(nSelectedToolIndex == CB_ERR)
	{
		return;
	}

	CDMToolInfo* pToolInfo = (CDMToolInfo*)m_ObjectCombo.GetItemDataPtr(nSelectedToolIndex);
	ASSERT(pToolInfo);
	if(pToolInfo == NULL)
	{
		return;
	}
	
	m_OKButton.EnableWindow(TRUE);
	m_ParamCombo.EnableWindow(TRUE);
	
	CPtrList lstToolParams;
	if(FAILED(GetToolParamList(pToolInfo, &lstToolParams)))
	{
		m_ParamCombo.EnableWindow(FALSE);
		m_OKButton.EnableWindow(FALSE);

		return;
	}

	
	if(lstToolParams.IsEmpty())
	{
		m_ParamCombo.EnableWindow(FALSE);
		m_OKButton.EnableWindow(FALSE);
		return;
	}

	POSITION position = lstToolParams.GetHeadPosition();
	while(position)
	{
		MP_PARAMINFO* pParamInfo = (MP_PARAMINFO*) lstToolParams.GetNext(position);
		ASSERT(pParamInfo);
		if(pParamInfo)
		{
			CString sParamLabel = pParamInfo->szLabel;
			int nInsertionIndex = m_ParamCombo.AddString(sParamLabel);
			if(nInsertionIndex != CB_ERR)
			{
				if(m_ParamCombo.SetItemDataPtr(nInsertionIndex, pParamInfo) == CB_ERR)
				{
					m_ParamCombo.DeleteString(nInsertionIndex);
					return;
				}
			}
		}
	}

	m_ParamCombo.SetCurSel(m_nLastSelectedParamIndex);

}


HRESULT CNewParameterDialog::GetToolList(IDMUSProdSegmentEdit8*	pISegment, IDMUSProdConductor* pIConductor, DWORD dwStage, CPtrList* plstObjects)
{
	ASSERT(plstObjects);
	if(plstObjects == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pISegment);
	if(pISegment == NULL)
	{
		return E_POINTER;
	}

	IDMUSProdToolGraphInfo* pIToolGraphInfo = NULL;
	if(dwStage == DMUS_PATH_SEGMENT_TOOL)
	{
		if(FAILED(GetSegmentToolGraph(pISegment, &pIToolGraphInfo)))
		{
			return E_FAIL;
		}
	}
	else if(dwStage == DMUS_PATH_AUDIOPATH_TOOL)
	{
		if(FAILED(GetSegmentAudioPathToolGraph(pISegment, &pIToolGraphInfo)))
		{
			return E_FAIL;
		}
	}
	else if(DMUS_PATH_PERFORMANCE_TOOL)
	{
		if(FAILED(GetPerformanceAudioPathToolGraph(pIConductor, &pIToolGraphInfo)))
		{
			return E_FAIL;
		}
	}

	// Should never really assert right?
	ASSERT(pIToolGraphInfo);

	DWORD dwToolCount = 0;
	if(FAILED(pIToolGraphInfo->GetToolCount(&dwToolCount)))
	{
		pIToolGraphInfo->Release();
		return E_FAIL;
	}

	for(DWORD dwIndex = 0; dwIndex < dwToolCount; dwIndex++)
	{
		IUnknown* pUnkTool = NULL;
		if(FAILED(pIToolGraphInfo->EnumTools(dwIndex, &pUnkTool)))
		{
			return E_FAIL;
		}

		DMUSProdToolInfo toolInfo;
		ZeroMemory(&toolInfo, sizeof(DMUSProdToolInfo));
		toolInfo.wSize = sizeof(DMUSProdToolInfo);
		if(FAILED(pIToolGraphInfo->GetToolInfo(pUnkTool, &toolInfo)))
		{
			pUnkTool->Release();
			return E_FAIL;
		}

		CDMToolInfo* pToolInfo = new CDMToolInfo(toolInfo, pUnkTool);
		if(pToolInfo == NULL)
		{
			pUnkTool->Release();
			return E_OUTOFMEMORY;
		}
		
		plstObjects->AddTail(pToolInfo);
	}

	pIToolGraphInfo->Release();
	return S_OK;

}

HRESULT CNewParameterDialog::GetToolParamList(CDMToolInfo* pToolInfo, CPtrList* plstToolParams)
{
	ASSERT(pToolInfo);
	if(pToolInfo == NULL)
	{
		return E_POINTER;
	}

	ASSERT(plstToolParams);
	if(plstToolParams == NULL)
	{
		return E_POINTER;
	}

	IUnknown* pIUnkTool = pToolInfo->m_pIUnkTool;
	ASSERT(pIUnkTool);
	if(pIUnkTool == NULL)
	{
		return E_UNEXPECTED;
	}

	IMediaParamInfo* pIParamInfo = NULL;
	if(FAILED(pIUnkTool->QueryInterface(IID_IMediaParamInfo, (void**)&pIParamInfo)))
	{
		return E_FAIL;
	}

	if(FAILED(GetSupportedTimeFormat(pIParamInfo, &(pToolInfo->m_guidTimeFormat))))
	{
		pIParamInfo->Release();
		return E_FAIL;
	}

	DWORD dwParamCount = 0;
	if(FAILED(pIParamInfo->GetParamCount(&dwParamCount)))
	{
		pIParamInfo->Release();
		return E_FAIL;
	}

	for(DWORD dwIndex = 0; dwIndex < dwParamCount; dwIndex++)
	{
		MP_PARAMINFO* pInfo = new MP_PARAMINFO;
		if(FAILED(pIParamInfo->GetParamInfo(dwIndex, pInfo)))
		{
			delete pInfo;
			return E_FAIL;
		}

		plstToolParams->AddTail(pInfo);
	}

	pIParamInfo->Release();
	return S_OK;
}


HRESULT CNewParameterDialog::GetSegmentToolGraph(IDMUSProdSegmentEdit8* pISegment, IDMUSProdToolGraphInfo** ppIToolGraphInfo)
{
	ASSERT(pISegment);
	if(pISegment == NULL)
	{
		return E_FAIL;
	}

	IDMUSProdNode* pISegmentNode = NULL;
	if(FAILED(pISegment->QueryInterface(IID_IDMUSProdNode, (void**)&pISegmentNode)))
	{
		return E_FAIL;
	}

	if(FAILED(pISegmentNode->GetObject(GUID_ToolGraphNode, IID_IDMUSProdToolGraphInfo, (void**)ppIToolGraphInfo)))
	{
		pISegmentNode->Release();
		return E_FAIL;
	}

	pISegmentNode->Release();
	return S_OK;
}


HRESULT CNewParameterDialog::GetSegmentAudioPathToolGraph(IDMUSProdSegmentEdit8* pISegment, IDMUSProdToolGraphInfo** ppIToolGraphInfo)
{
	ASSERT(pISegment);
	if(pISegment == NULL)
	{
		return E_FAIL;
	}

	IDMUSProdNode* pISegmentNode = NULL;
	if(FAILED(pISegment->QueryInterface(IID_IDMUSProdNode, (void**)&pISegmentNode)))
	{
		return E_FAIL;
	}

	IDMUSProdNode* pIAudioPathNode = NULL;
	if(FAILED(pISegmentNode->GetObject(GUID_AudioPathNode, IID_IDMUSProdNode, (void**)&pIAudioPathNode)))
	{
		pISegmentNode->Release();
		return E_FAIL;
	}

	if(FAILED(pIAudioPathNode->GetObject(GUID_ToolGraphNode, IID_IDMUSProdToolGraphInfo, (void**)ppIToolGraphInfo)))
	{
		pIAudioPathNode->Release();
		pISegmentNode->Release();
		return E_FAIL;
	}

	pIAudioPathNode->Release();
	pISegmentNode->Release();
	return S_OK;
}

HRESULT CNewParameterDialog::GetPerformanceAudioPathToolGraph(IDMUSProdConductor* pIConductor, IDMUSProdToolGraphInfo** ppIToolGraphInfo)
{
	ASSERT(pIConductor);
	if(pIConductor == NULL)
	{
		return E_POINTER;
	}

	IDMUSProdConductorPrivate* pIConductorPrivate = NULL;
	if(FAILED(pIConductor->QueryInterface(IID_IDMUSProdConductorPrivate, (void**)&pIConductorPrivate)))
	{
		return E_FAIL;
	}

	IDMUSProdNode* pIAudioPathNode = NULL;
	if(FAILED(pIConductorPrivate->GetDefaultAudiopathNode(&pIAudioPathNode)))
	{
		pIConductorPrivate->Release(); 
		return E_FAIL;
	}

	// Release the private interface
	pIConductorPrivate->Release(); 

	if(FAILED(pIAudioPathNode->GetObject(GUID_ToolGraphNode, IID_IDMUSProdToolGraphInfo, (void**)ppIToolGraphInfo)))
	{
		pIAudioPathNode->Release();
		return E_FAIL;
	}

	pIAudioPathNode->Release();
	return S_OK;
}

void CNewParameterDialog::OnSelchangeComboObject() 
{
	if(m_bParamsForTools)
	{
		InitializeToolsParameterCombo();
	}
	else
	{
		InitializeDMOParameterCombo();
	}
	
	RefreshPChannelText();
}

void CNewParameterDialog::OnSelchangeComboStage() 
{
	if(m_bParamsForTools)
	{
		if(SUCCEEDED(InitializeToolsObjectCombo()))
		{
			InitializeToolsParameterCombo();
		}
	}
	else
	{
		if(SUCCEEDED(InitializeDMOObjectCombo()))
		{
			InitializeDMOParameterCombo();
		}
	}
}

void CNewParameterDialog::CleanObjectList()
{
	int nObjectCount = m_ObjectCombo.GetCount();
	while(m_ObjectCombo.GetCount() > 0)
	{
		void* pObject = m_ObjectCombo.GetItemDataPtr(0);
		ASSERT(pObject);
		m_ObjectCombo.DeleteString(0);
		if(pObject)
		{
			delete pObject;
		}
	}

	m_ObjectCombo.ResetContent();
}


void CNewParameterDialog::CleanParamsList()
{
	int nParamCount = m_ParamCombo.GetCount();
	while(m_ParamCombo.GetCount() > 0)
	{
		MP_PARAMINFO* pParamInfo = (MP_PARAMINFO*)m_ParamCombo.GetItemDataPtr(0);
		ASSERT(pParamInfo);
		m_ParamCombo.DeleteString(0);
		if(pParamInfo)
		{
			delete pParamInfo;
		}
	}

	m_ParamCombo.ResetContent();
}

void CNewParameterDialog::OnOK() 
{
	int nStageIndex = m_StageCombo.GetCurSel();
	if(nStageIndex == CB_ERR)
	{
		return;
	}
	StageInfo selectedStageInfo = m_arrToolsStages[nStageIndex];
	m_StripInfo.m_dwStage = selectedStageInfo.m_dwStage;

	CString sStageName;
	m_StageCombo.GetWindowText(sStageName);
	m_StripInfo.m_sStageName = sStageName;

	CString sObjectName;
	m_ObjectCombo.GetWindowText(sObjectName);
	m_StripInfo.m_sObjectName = sObjectName;

	int nParamIndex = m_ParamCombo.GetCurSel();
	if(nParamIndex == CB_ERR)
	{
		return;
	}
	MP_PARAMINFO* pParamInfo = (MP_PARAMINFO*)m_ParamCombo.GetItemDataPtr(nParamIndex);
	if(pParamInfo == NULL)
	{
		return;
	}

	CopyMemory(&m_StripInfo.m_ParamInfo, pParamInfo, sizeof(MP_PARAMINFO));
	
	m_StripInfo.m_dwParamIndex = nParamIndex;
	
	CString sParamName;
	m_ParamCombo.GetWindowText(sParamName);
	m_StripInfo.m_sParamName = sParamName;

	// We know the CLSID for the DMOs...so get it only if we're dealing with tools
	if(m_bParamsForTools)
	{
		int nObjectIndex = m_ObjectCombo.GetCurSel();
		if(nObjectIndex == CB_ERR)
		{
			return;
		}
		CDMToolInfo* pToolInfo = (CDMToolInfo*)m_ObjectCombo.GetItemDataPtr(nObjectIndex);
		if(pToolInfo == NULL)
		{
			return;
		}
		if(FAILED(GetObjectCLSID(pToolInfo->m_pIUnkTool, &m_StripInfo.m_guidObject)))
		{
			return;
		}

		m_StripInfo.m_sPChannelText = pToolInfo->m_ToolInfo.awchPChannels;
		m_StripInfo.m_dwPChannel = pToolInfo->m_ToolInfo.dwFirstPChannel;
		m_StripInfo.m_dwBuffer = 0;
		m_StripInfo.m_guidTimeFormat = pToolInfo->m_guidTimeFormat;

		m_nLastSelectedStageIndex = nStageIndex;
		m_nLastSelectedObjectIndex = nObjectIndex;
		m_nLastSelectedParamIndex = nParamIndex;
	}
	else
	{
		int nObjectIndex = m_ObjectCombo.GetCurSel();
		if(nObjectIndex == CB_ERR)
		{
			return;
		}
		CDMOInfo* pDMOInfo = (CDMOInfo*)m_ObjectCombo.GetItemDataPtr(nObjectIndex);
		if(pDMOInfo == NULL)
		{
			return;
		}
		m_StripInfo.m_dwStage = pDMOInfo->m_DMOInfo.dwStage;
		m_StripInfo.m_guidObject = pDMOInfo->m_DMOInfo.clsidDMO;
        m_StripInfo.m_guidInstance = pDMOInfo->m_DMOInfo.guidDesignGUID;
		m_StripInfo.m_sPChannelText = pDMOInfo->m_DMOInfo.awchPChannelText;
		m_StripInfo.m_dwPChannel = pDMOInfo->m_DMOInfo.dwPChannel;
		m_StripInfo.m_dwBuffer = pDMOInfo->m_DMOInfo.dwBufferIndex;
		m_StripInfo.m_guidTimeFormat = pDMOInfo->m_guidTimeFormat;
		m_StripInfo.m_dwObjectIndex = pDMOInfo->m_DMOInfo.dwEffectIndex;

		m_nLastSelectedStageIndex = nStageIndex;
		m_nLastSelectedObjectIndex = nObjectIndex;
		m_nLastSelectedParamIndex = nParamIndex;
	}

	CDialog::OnOK();
}

void CNewParameterDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

HRESULT	CNewParameterDialog::GetObjectCLSID(IUnknown* pIUnkObject, GUID* pguidObject)
{
	ASSERT(pIUnkObject);
	if(pIUnkObject == NULL)
	{
		return E_POINTER;
	}

	// Get IPersistStream and ask for CLSID
	IPersistStream* pIStream = NULL;
	if(FAILED(pIUnkObject->QueryInterface(IID_IPersistStream, (void**)&pIStream)))
	{
		return E_FAIL;
	}

	if(FAILED(pIStream->GetClassID(pguidObject)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	pIStream->Release();
	return S_OK;
}

void CNewParameterDialog::OnRadioTools() 
{
	m_bParamsForTools = true;
	CleanObjectList();
	CleanParamsList();

	InitializeToolsControls();
}

void CNewParameterDialog::OnRadioDMOs() 
{
	m_bParamsForTools = false;
	CleanObjectList();
	CleanParamsList();

	InitializeDMOControls();
}


void CNewParameterDialog::InitializeDMOControls()
{
	CButton* pToolRadio = (CButton*) GetDlgItem(IDC_RADIO_TOOLS);
	if(pToolRadio)
	{
		pToolRadio->SetCheck(0);
	}

	CButton* pDMORadio = (CButton*)GetDlgItem(IDC_RADIO_DMOS);
	if(pDMORadio)
	{
		pDMORadio->SetCheck(1);
	}

	InitializeDMOStageCombo();
	if(SUCCEEDED(InitializeDMOObjectCombo()))
	{
		InitializeDMOParameterCombo();
	}
}

void CNewParameterDialog::InitializeDMOStageCombo()
{
	m_StageCombo.ResetContent();

	for(int nIndex = 0; nIndex < MAX_STAGES_DMOS; nIndex++)
	{
		CString sStageName;
		sStageName.LoadString(m_arrDMOStages[nIndex].m_uStageName);
		m_StageCombo.AddString(sStageName);				
	}

	m_StageCombo.SetCurSel(m_nLastSelectedStageIndex);
	RefreshPChannelText();
}

HRESULT CNewParameterDialog::InitializeDMOObjectCombo()
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_FAIL;
	}

	CleanObjectList();
	CleanParamsList();

	int nSelectedStage = m_StageCombo.GetCurSel();
	if(nSelectedStage == CB_ERR)
	{
		return E_FAIL;
	}

	DWORD dwStage = m_arrDMOStages[nSelectedStage].m_dwStage;
	
	m_ObjectCombo.EnableWindow(TRUE);
	m_ParamCombo.EnableWindow(TRUE);
	m_OKButton.EnableWindow(TRUE);

	IDMUSProdConductor* pIConductor = m_pTrackMgr->GetConductor();
	ASSERT(pIConductor);
	if(pIConductor == NULL)
	{
		return E_FAIL;
	}

	CPtrList lstObjects;
	if(FAILED(GetDMOList(m_pISegment, pIConductor, dwStage, &lstObjects)))
	{
		m_ObjectCombo.EnableWindow(FALSE);
		m_ParamCombo.EnableWindow(FALSE);
		m_OKButton.EnableWindow(FALSE);
		pIConductor->Release();
		return E_FAIL;
	}
	pIConductor->Release();

	if(lstObjects.IsEmpty())
	{
		m_ObjectCombo.EnableWindow(FALSE);
		m_ParamCombo.EnableWindow(FALSE);
		m_OKButton.EnableWindow(FALSE);
		return E_FAIL;
	}

	POSITION position = lstObjects.GetHeadPosition();
	while(position)
	{
		CDMOInfo* pDMOInfo = (CDMOInfo*)lstObjects.GetNext(position);
		ASSERT(pDMOInfo);
		if(pDMOInfo)
		{
			CString sDMOName = pDMOInfo->m_DMOInfo.awchDMOName;
			int nInsertionIndex = m_ObjectCombo.AddString(sDMOName);
			if(nInsertionIndex != CB_ERR)
			{
				if(m_ObjectCombo.SetItemDataPtr(nInsertionIndex, pDMOInfo) == CB_ERR)
				{
					m_ObjectCombo.DeleteString(nInsertionIndex);
					return E_FAIL;
				}
			}
		}
	}

	if(m_nLastSelectedObjectIndex < (UINT)m_ObjectCombo.GetCount())
	{
		m_ObjectCombo.SetCurSel(m_nLastSelectedObjectIndex);
	}
	else
	{
		m_ObjectCombo.SetCurSel(0);
	}
	RefreshPChannelText();

	return S_OK;
}

void CNewParameterDialog::InitializeDMOParameterCombo()
{
	CleanParamsList();

	int nSelectedDMOIndex = m_ObjectCombo.GetCurSel();
	if(nSelectedDMOIndex == CB_ERR)
	{
		return;
	}

	CDMOInfo* pDMOInfo = (CDMOInfo*)m_ObjectCombo.GetItemDataPtr(nSelectedDMOIndex);
	ASSERT(pDMOInfo);
	if(pDMOInfo == NULL)
	{
		return;
	}

	m_OKButton.EnableWindow(TRUE);
	m_ParamCombo.EnableWindow(TRUE);
	
	CPtrList lstDMOParams;
	if(FAILED(GetDMOParamList(pDMOInfo, &lstDMOParams)))
	{
		m_OKButton.EnableWindow(FALSE);
		m_ParamCombo.EnableWindow(FALSE);
		return;
	}

	if(lstDMOParams.IsEmpty())
	{
		m_OKButton.EnableWindow(FALSE);
		m_ParamCombo.EnableWindow(FALSE);
		return;
	}

	POSITION position = lstDMOParams.GetHeadPosition();
	while(position)
	{
		MP_PARAMINFO* pParamInfo = (MP_PARAMINFO*) lstDMOParams.GetNext(position);
		ASSERT(pParamInfo);
		if(pParamInfo)
		{
			CString sParamLabel = pParamInfo->szLabel;
			int nInsertionIndex = m_ParamCombo.AddString(sParamLabel);
			if(nInsertionIndex != CB_ERR)
			{
				if(m_ParamCombo.SetItemDataPtr(nInsertionIndex, pParamInfo) == CB_ERR)
				{
					m_ParamCombo.DeleteString(nInsertionIndex);
					return;
				}
			}
		}
	}

	m_ParamCombo.SetCurSel(m_nLastSelectedParamIndex);
}


HRESULT CNewParameterDialog::GetDMOList(IDMUSProdSegmentEdit8*	pISegment, IDMUSProdConductor* pIConductor, DWORD dwStage, CPtrList* plstObjects)
{
	ASSERT(plstObjects);
	if(plstObjects == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pISegment);
	if(pISegment == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdDMOInfo* pIDMOInfo = NULL;
	if(dwStage == DMUS_PATH_BUFFER_DMO)
	{
		if(FAILED(GetSegmentDMOInfo(pISegment, &pIDMOInfo)))
		{
			// We might be calling this on load...the stage ID in the content for both stages is the same as DMUS_PATH_BUFFER_DMO
			/*if(FAILED(GetPerformanceDMOInfo(pIConductor, &pIDMOInfo)))
			{*/
				return E_FAIL;
			/*}*/
		}
	}
	else if(dwStage == DMUS_PATH_PERFORMANCE)
	{
		if(FAILED(GetPerformanceDMOInfo(pIConductor, &pIDMOInfo)))
		{
			return E_FAIL;
		}
	
	}
	else
	{
		return E_UNEXPECTED;
	}

	// Should never really assert right?
	ASSERT(pIDMOInfo);

	DWORD dwDMOCount = 0;
	for(DWORD dwIndex = 0; ; dwIndex++)
	{
		DMUSProdDMOInfo dmoInfo;
		ZeroMemory(&dmoInfo, sizeof(DMUSProdDMOInfo));
		dmoInfo.dwSize = sizeof(DMUSProdDMOInfo);
		if(FAILED(pIDMOInfo->EnumDMOInfo(dwIndex, &dmoInfo)))
		{
			dwIndex--;
			break;
		}

		CDMOInfo* pDMOInfo = new CDMOInfo(dmoInfo);

		if(pDMOInfo == NULL)
		{
			pIDMOInfo->Release();
			return E_OUTOFMEMORY;
		}
		
		plstObjects->AddTail(pDMOInfo);
	}

	pIDMOInfo->Release();

	return S_OK;

}

HRESULT CNewParameterDialog::GetDMOParamList(CDMOInfo* pDMOInfo, CPtrList* plstDMOParams)
{
	ASSERT(pDMOInfo);
	if(pDMOInfo == NULL)
	{
		return E_POINTER;
	}

	ASSERT(plstDMOParams);
	if(plstDMOParams == NULL)
	{
		return E_POINTER;
	}

	CLSID clsidDMO = pDMOInfo->m_DMOInfo.clsidDMO;
	IUnknown* pIUnkDMO = NULL;
	if(FAILED(::CoCreateInstance(clsidDMO, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**) &pIUnkDMO)))
	{
		return E_FAIL;
	}

	ASSERT(pIUnkDMO);

	IMediaParamInfo* pIParamInfo = NULL;
	if(FAILED(pIUnkDMO->QueryInterface(IID_IMediaParamInfo, (void**)&pIParamInfo)))
	{
		return E_FAIL;
	}

	DWORD dwParamCount = 0;
	if(FAILED(pIParamInfo->GetParamCount(&dwParamCount)))
	{
		pIParamInfo->Release();
		pIUnkDMO->Release();
		return E_FAIL;
	}

	for(DWORD dwIndex = 0; dwIndex < dwParamCount; dwIndex++)
	{
		MP_PARAMINFO* pInfo = new MP_PARAMINFO;
		if(FAILED(pIParamInfo->GetParamInfo(dwIndex, pInfo)))
		{
			delete pInfo;
			pIParamInfo->Release();
			pIUnkDMO->Release();
			return E_FAIL;
		}

		if(pInfo->mpType == MPT_ENUM)
		{

			WCHAR* pwchText = NULL;
			/*if(FAILED(pIParamInfo->GetParamText(dwIndex, &pwchText)))
			{
				delete pInfo;
				pIParamInfo->Release();
				pIUnkDMO->Release();
				return E_FAIL;
			}*/

			if(FAILED(CTrackObject::GetParamEnumTypeText(clsidDMO, dwIndex, &pwchText)))
			{
				delete pInfo;
				pIParamInfo->Release();
				pIUnkDMO->Release();
				return E_FAIL;
			}

			CString sParamText = pwchText;
			if(pwchText)
			{
				::CoTaskMemFree(pwchText);
			}
		}

		// Get the supprted time format for the DMO
		if(FAILED(GetSupportedTimeFormat(pIParamInfo, &(pDMOInfo->m_guidTimeFormat))))
		{
			pIParamInfo->Release();
			pIUnkDMO->Release();
			return E_FAIL;
		}

		plstDMOParams->AddTail(pInfo);
	}

	pIParamInfo->Release();
	pIUnkDMO->Release();
	return S_OK;
}


HRESULT CNewParameterDialog::GetSegmentDMOInfo(IDMUSProdSegmentEdit8* pISegment, IDMUSProdDMOInfo** ppIDMOInfo)
{
	ASSERT(pISegment);
	if(pISegment == NULL)
	{
		return E_FAIL;
	}

	IDMUSProdNode* pISegmentNode = NULL;
	if(FAILED(pISegment->QueryInterface(IID_IDMUSProdNode, (void**)&pISegmentNode)))
	{
		return E_FAIL;
	}

	if(FAILED(pISegmentNode->GetObject(GUID_AudioPathNode, IID_IDMUSProdDMOInfo, (void**)ppIDMOInfo)))
	{
		pISegmentNode->Release();
		return E_FAIL;
	}

	pISegmentNode->Release();
	return S_OK;
}


HRESULT CNewParameterDialog::GetPerformanceDMOInfo(IDMUSProdConductor* pIConductor, IDMUSProdDMOInfo** ppIDMOInfo)
{
	ASSERT(pIConductor);
	if(pIConductor == NULL)
	{
		return E_POINTER;
	}

	IDMUSProdConductorPrivate* pIConductorPrivate = NULL;
	if(FAILED(pIConductor->QueryInterface(IID_IDMUSProdConductorPrivate, (void**)&pIConductorPrivate)))
	{
		return E_FAIL;
	}

	IDMUSProdNode* pIAudioPathNode = NULL;
	if(FAILED(pIConductorPrivate->GetDefaultAudiopathNode(&pIAudioPathNode)))
	{
		pIConductorPrivate->Release(); 
		return E_FAIL;
	}
	
	// Release the private interface
	pIConductorPrivate->Release(); 
	
	if(FAILED(pIAudioPathNode->QueryInterface(IID_IDMUSProdDMOInfo, (void**)ppIDMOInfo)))
	{
		pIAudioPathNode->Release();
		return E_FAIL;
	}

	pIAudioPathNode->Release();
	return S_OK;
}


void CNewParameterDialog::RefreshPChannelText()
{
	if(m_ObjectCombo.GetCount() == 0)
	{
		return;
	}

	m_PChannelNameEdit.SetWindowText(NULL);

	if(m_bParamsForTools)
	{
		int nIndex = m_ObjectCombo.GetCurSel();
		if(nIndex != CB_ERR)
		{
			CDMToolInfo* pToolInfo = (CDMToolInfo*) m_ObjectCombo.GetItemDataPtr(nIndex);
			CString sPChannelText = "";
			if(pToolInfo)
			{
				sPChannelText = pToolInfo->m_ToolInfo.awchPChannels;
				m_PChannelNameEdit.SetWindowText(sPChannelText);
			}
		}
	}
	else
	{
		int nIndex = m_ObjectCombo.GetCurSel();
		if(nIndex != CB_ERR)
		{
			CDMOInfo* pDMOInfo = (CDMOInfo*) m_ObjectCombo.GetItemDataPtr(nIndex);
			CString sPChannelText = "";
			if(pDMOInfo)
			{
				sPChannelText = pDMOInfo->m_DMOInfo.awchPChannelText;
				m_PChannelNameEdit.SetWindowText(sPChannelText);
			}
		}
	}
}

HRESULT CNewParameterDialog::GetSupportedTimeFormat(IMediaParamInfo* pIParamInfo, GUID* pguidTimeFormat)
{
	ASSERT(pIParamInfo);
	if(pIParamInfo == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pguidTimeFormat);
	if(pguidTimeFormat == NULL)
	{
		return E_POINTER;
	}

	DWORD dwNumFormats = 0;
	if(FAILED(pIParamInfo->GetNumTimeFormats(&dwNumFormats)))
	{
		return E_FAIL;
	}

	if(dwNumFormats == 0)
	{
		return E_FAIL;
	}

	GUID guidMusicTime = GUID_TIME_MUSIC;
	GUID guidRefTime = GUID_TIME_REFERENCE;

	for(DWORD dwIndex = 0; dwIndex < dwNumFormats; dwIndex++)
	{
		if(FAILED(pIParamInfo->GetSupportedTimeFormat(dwIndex, pguidTimeFormat)))
		{
			return E_FAIL;
		}

		if(::IsEqualGUID(guidMusicTime, *pguidTimeFormat))
		{
			return S_OK;
		}

		if(::IsEqualGUID(guidRefTime, *pguidTimeFormat))
		{
			return S_OK;
		}
	}

	// Send back the last guid...no choice!
	if(dwIndex == dwNumFormats)
	{
		return S_OK;
	}
	
	return E_FAIL;
}