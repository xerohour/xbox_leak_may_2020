#if !defined(AFX_TABOBJECT_H__D02E70E9_E7C6_4033_AD9F_A8BDA1620F03__INCLUDED_)
#define AFX_TABOBJECT_H__D02E70E9_E7C6_4033_AD9F_A8BDA1620F03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabObject.h : header file
//
// Also defined in NewPartDlg.h
#define MIN_PCHANNEL	1
#define MAX_PCHANNEL	999

#include "resource.h"
#include "NewPartDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CTabObject dialog

interface IDMUSProdPropPageObject;
interface IDMUSProdPChannelName;
interface IDMUSProdPropSheet;

class CGroupBitsPropPageMgr;

struct ioObjectPPG
{
	DWORD	dwPageIndex;
	DWORD	dwPChannel;
	CString strPChannelText;
	DWORD	dwParamFlagsDM;
	CString strStripName;
	CString strPChannelName;
	CString strObjectName;
	CString strStageName;
};

class CTabObject : public CPropertyPage
{
friend class CGroupBitsPropPageMgr;

// Construction
public:
	CTabObject();
	~CTabObject();

// Dialog Data
	//{{AFX_DATA(CTabObject)
	enum { IDD = IDD_PROPPAGE_OBJECT };
	CStatic	m_StageStatic;
	CStatic	m_ObjectStatic;
	CEdit	m_editPChannel;
	CEdit	m_editPChannelName;
	CSpinButtonCtrl	m_spinPChannel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabObject)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTabObject)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditPchannel();
	afx_msg void OnKillfocusEditPchannel();
	afx_msg void OnChangeEditPChannelName();
	afx_msg void OnDeltaposSpinPchannel(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( BOOL fEnable );
	void UpdatePPO();
	HRESULT GetPChannelName( DWORD dwPChannel, CString& strPChannelName );

public:
	void SetObject( IDMUSProdPropPageObject* pPPO );
	void RefreshData( void );

	// Variables for keeping track of the active property tab (if there is more than one)
	IDMUSProdPropSheet*			m_pIPropSheet;

protected:
	CGroupBitsPropPageMgr*		m_pPropPageMgr;
	IDMUSProdPropPageObject*	m_pPPO;
	ioObjectPPG					m_PPGObjectParams;
	BOOL						m_fNeedToDetach;

	CString						m_strBroadcastSeg;
	CString						m_strBroadcastPerf;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABOBJECT_H__D02E70E9_E7C6_4033_AD9F_A8BDA1620F03__INCLUDED_)
