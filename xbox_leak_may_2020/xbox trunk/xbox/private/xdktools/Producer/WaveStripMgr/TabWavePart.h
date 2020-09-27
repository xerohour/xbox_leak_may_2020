#if !defined(AFX_TABWAVEPART_H__D02E70E9_E7C6_4033_AD9F_A8BDA1620F03__INCLUDED_)
#define AFX_TABWAVEPART_H__D02E70E9_E7C6_4033_AD9F_A8BDA1620F03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabWavePart.h : header file
//
// Also defined in NewPartDlg.h
#define MIN_PCHANNEL	1
#define MAX_PCHANNEL	999

#include "resource.h"
#include "NewPartDlg.h"
#include "MySlider.h"

/////////////////////////////////////////////////////////////////////////////
// CTabWavePart dialog

interface IDMUSProdPropPageObject;
interface IDMUSProdPChannelName;
interface IDMUSProdPropSheet;

class CGroupBitsPropPageMgr;

struct ioWavePartPPG
{
	DWORD	dwPageIndex;
	DWORD	dwPChannel;
	long	lVolume;		
	DWORD	dwLockToPart;	
	DWORD	dwPartFlagsDM;
	CString strStripName;
	CString strPChannelName;
};

class CTabWavePart : public CPropertyPage, CSliderCollection
{
friend class CGroupBitsPropPageMgr;

// Construction
public:
	CTabWavePart();
	~CTabWavePart();

// Dialog Data
	//{{AFX_DATA(CTabWavePart)
	enum { IDD = IDD_PROPPAGE_PART };
	CComboBox	m_comboVarMode;
	CComboBox	m_comboVarLockID;
	CEdit	m_editPChannelName;
	CEdit	m_editName;
	CSpinButtonCtrl	m_spinPChannel;
	CNewPartEdit	m_editPChannel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabWavePart)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTabWavePart)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditPchannel();
	afx_msg void OnKillfocusEditPchannel();
	afx_msg void OnChangeEditPChannelName();
	afx_msg void OnChangeEditName();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelChangeComboVarLock();
	afx_msg void OnSelChangeComboVarMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( BOOL fEnable );
	void UpdatePPO();
	void FillVarLockComboBox();
	HRESULT GetPChannelName( DWORD dwPChannel, CString& strPChannelName );

	// CSliderCollection overrides
	virtual bool OnSliderUpdate(MySlider *pms, DWORD dwmscupdf);

public:
	void SetObject( IDMUSProdPropPageObject* pPPO );
	void RefreshData( void );

	// Variables for keeping track of the active property tab (if there is more than one)
	IDMUSProdPropSheet*			m_pIPropSheet;

protected:
	CGroupBitsPropPageMgr*		m_pPropPageMgr;
	IDMUSProdPropPageObject*	m_pPPO;
	ioWavePartPPG				m_PPGPartParams;
	BOOL						m_fNeedToDetach;

	MySlider*					m_pmsAttenuation;
	long						m_lAttenuation;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABWAVEPART_H__D02E70E9_E7C6_4033_AD9F_A8BDA1620F03__INCLUDED_)
