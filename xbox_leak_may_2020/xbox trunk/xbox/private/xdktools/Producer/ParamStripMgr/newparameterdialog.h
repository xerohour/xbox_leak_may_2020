#if !defined(AFX_NEWPARAMETERDIALOG_H__6733EE74_A089_4FB2_BD58_155F0C0868A8__INCLUDED_)
#define AFX_NEWPARAMETERDIALOG_H__6733EE74_A089_4FB2_BD58_155F0C0868A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// newparameterdialog.h : header file
//

#include <afxtempl.h>
#include "resource.h"
#include "DMusProd.h"
#include "MedParam.h"
#include "TrackMgr.h"
#include "TrackObject.h"
#include "SegmentDesigner.h"
#include "ToolGraphDesigner.h"
#include "AudioPathDesigner.h"


#define MAX_STAGES_TOOLS	3
#define MAX_STAGES_DMOS		2

class CTrackMgr;

struct StageInfo
{
	StageInfo(DWORD dwStage = 0, UINT uStageName = 0)
	{
		m_dwStage = dwStage;
		m_uStageName = uStageName;
	};

	DWORD	m_dwStage;
	UINT	m_uStageName;
};

class CDMToolInfo
{
public:
	CDMToolInfo(DMUSProdToolInfo toolInfo, IUnknown* pIUnkTool);
	virtual ~CDMToolInfo();

public:
	DMUSProdToolInfo	m_ToolInfo;
	GUID				m_guidTimeFormat;
	IUnknown*			m_pIUnkTool;
};

class CDMOInfo
{
public:
	CDMOInfo(DMUSProdDMOInfo dmoInfo);

public:
	DMUSProdDMOInfo	m_DMOInfo;
	GUID			m_guidTimeFormat;
};


struct StripInfo
{
	StripInfo& operator = (const StripInfo& source)
	{
		m_sStageName = source.m_sStageName;
		m_sObjectName = source.m_sObjectName;
		m_dwPChannel = source.m_dwPChannel;
		m_sPChannelText = source.m_sPChannelText;
		m_dwStage = source.m_dwStage;
		m_guidObject = source.m_guidObject;
        m_guidInstance = source.m_guidInstance;
		m_guidTimeFormat = source.m_guidTimeFormat;
		CopyMemory(&m_ParamInfo, &source.m_ParamInfo, sizeof(MP_PARAMINFO));
		m_sParamName = source.m_sParamName;
		m_dwParamIndex = source.m_dwParamIndex;
		m_dwBuffer = source.m_dwBuffer;
		m_dwObjectIndex = source.m_dwObjectIndex;
		return *this;
	}

	DWORD			m_dwPChannel;
	CString			m_sPChannelText;
	DWORD			m_dwStage;
	DWORD			m_dwObjectIndex;
	CString			m_sObjectName;
	CString			m_sStageName;
	GUID			m_guidObject;
    GUID            m_guidInstance;
	MP_PARAMINFO	m_ParamInfo;
	CString			m_sParamName;
	DWORD			m_dwParamIndex;
	GUID			m_guidTimeFormat;
	DWORD			m_dwBuffer;

};


/////////////////////////////////////////////////////////////////////////////
// CNewParameterDialog dialog

class CNewParameterDialog : public CDialog
{
// Construction
public:
	CNewParameterDialog(CTrackMgr* pTrackMgr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewParameterDialog)
	enum { IDD = IDD_NEW_PARAM };
	CButton	m_OKButton;
	CSpinButtonCtrl	m_PChannelSpin;
	CEdit	m_PChannelEdit;
	CEdit	m_PChannelNameEdit;
	CComboBox	m_ObjectCombo;
	CComboBox	m_ParamCombo;
	CComboBox	m_StageCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewParameterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewParameterDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeComboObject();
	afx_msg void OnSelchangeComboStage();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRadioTools();
	afx_msg void OnRadioDMOs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	void InitializeToolsControls();
	void InitializeToolsStageCombo();
	HRESULT InitializeToolsObjectCombo();
	void InitializeToolsParameterCombo();

	void InitializeDMOControls();
	void InitializeDMOStageCombo();
	HRESULT InitializeDMOObjectCombo();
	void InitializeDMOParameterCombo();

	static HRESULT GetToolList(IDMUSProdSegmentEdit8* pISegment, IDMUSProdConductor* pIConductor, DWORD dwStage, CPtrList* plstObjects);
	static HRESULT GetDMOList(IDMUSProdSegmentEdit8* pISegment, IDMUSProdConductor* pIConductor, DWORD dwStage, CPtrList* plstObjects);
	static HRESULT GetSupportedTimeFormat(IMediaParamInfo* pIParamInfo, GUID* pguidTimeFormat);

private:
	static HRESULT GetSegmentToolGraph(IDMUSProdSegmentEdit8* pISegment, IDMUSProdToolGraphInfo** ppIToolGraphNode);
	static HRESULT GetSegmentAudioPathToolGraph(IDMUSProdSegmentEdit8* pISegment, IDMUSProdToolGraphInfo** ppIToolGraphNode);
	static HRESULT GetPerformanceAudioPathToolGraph(IDMUSProdConductor* pIConductor, IDMUSProdToolGraphInfo** ppIToolGraphNode);
	HRESULT GetToolParamList(CDMToolInfo* pToolInfo, CPtrList* plstToolParams);

	static HRESULT GetSegmentDMOInfo(IDMUSProdSegmentEdit8* pISegment, IDMUSProdDMOInfo** ppIDMOInfo);
	static HRESULT GetPerformanceDMOInfo(IDMUSProdConductor* pIConductor, IDMUSProdDMOInfo** ppIDMOInfo);
	HRESULT GetDMOParamList(CDMOInfo* pDMOInfo, CPtrList* plstDMOParams);

	HRESULT	GetObjectCLSID(IUnknown* pIUnkObject, GUID* pguidObject);

	void	CleanObjectList();
	void	CleanParamsList();

	void	RefreshPChannelText();	

public:
	static StageInfo m_arrToolsStages[MAX_STAGES_TOOLS];
	static StageInfo m_arrDMOStages[MAX_STAGES_DMOS];
	
	StripInfo	m_StripInfo;

private:
	CTrackMgr*				m_pTrackMgr;
	IDMUSProdSegmentEdit8*	m_pISegment;

	static bool				m_bParamsForTools;

	static UINT				m_nLastSelectedStageIndex;
	static UINT				m_nLastSelectedObjectIndex;
	static UINT				m_nLastSelectedParamIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWPARAMETERDIALOG_H__6733EE74_A089_4FB2_BD58_155F0C0868A8__INCLUDED_)
