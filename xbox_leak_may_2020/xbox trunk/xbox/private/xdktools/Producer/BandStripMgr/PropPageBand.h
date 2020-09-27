#if !defined(AFX_PROPPAGEBAND_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_PROPPAGEBAND_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"

class CBandPropPageMgr;

// PropPageBand.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PropPageBand dialog
class CBandPropPageMgr;

class PropPageBand : public CPropertyPage
{
	friend CBandPropPageMgr;
	DECLARE_DYNCREATE(PropPageBand)

// Construction
public:
	PropPageBand();
	~PropPageBand();

// Dialog Data
	//{{AFX_DATA(PropPageBand)
	enum { IDD = IDD_BAND_PROPPAGE };
	CSpinButtonCtrl	m_spinLogicalMeasure;
	CSpinButtonCtrl	m_spinLogicalBeat;
	CEdit	m_editLogicalMeasure;
	CEdit	m_editLogicalBeat;
	CEdit	m_editBeat;
	CEdit	m_editBand;
	CEdit	m_editMeasure;
	CEdit	m_editTick;
	CSpinButtonCtrl	m_spinTick;
	CSpinButtonCtrl	m_spinMeasure;
	CSpinButtonCtrl	m_spinBeat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPageBand)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropPageBand)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditBeat();
	afx_msg void OnKillfocusEditBand();
	afx_msg void OnKillfocusEditMeasure();
	afx_msg void OnKillfocusEditTick();
	afx_msg void OnKillFocusEditLogicalBeat();
	afx_msg void OnKillFocusEditLogicalMeasure();
	afx_msg void OnDeltaPosSpinLogicalBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinLogicalMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void CopyDataToBand( CPropBand* pBand );
	void GetDataFromBand( CPropBand* pBand );

protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();

private:
	CPropBand*				m_pBand;
	CBandPropPageMgr*		m_pPropPageMgr;
	BOOL					m_fHaveData;
	BOOL					m_fMultipleBandsSelected;
	BOOL					m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGEBAND_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
