#if !defined(AFX_PROPPAGEMELGEN_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_PROPPAGEMELGEN_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include <afxtempl.h>

class CMelGenPropPageMgr;

// PropPageMelGen.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PropPageMelGen dialog
class CMelGenPropPageMgr;

class PropPageMelGen : public CPropertyPage
{
	friend CMelGenPropPageMgr;
	DECLARE_DYNCREATE(PropPageMelGen)

// Construction
public:
	PropPageMelGen();
	~PropPageMelGen();

// Dialog Data
	//{{AFX_DATA(PropPageMelGen)
	enum { IDD = IDD_MELGEN_PROPPAGE };
	CStatic	m_staticVariationsPrompt;
	CStatic	m_staticRepeatIdPrompt;
	CComboBox	m_comboOptions;
	CButton	m_checkNew;
	CListBox	m_listVariations;
	CEdit	m_editLabel;
	CEdit	m_editID;
	CComboBox	m_comboRepeat;
	CButton	m_btnMelGen;
	CEdit	m_editMeasure;
	CEdit	m_editBeat;
	CSpinButtonCtrl	m_spinMeasure;
	CSpinButtonCtrl	m_spinBeat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPageMelGen)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropPageMelGen)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditMeasure();
	afx_msg void OnMelGen();
	afx_msg void OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditBeat();
	afx_msg void OnSelchangeListVariations();
	afx_msg void OnSelchangeComboRepeat();
	afx_msg void OnKillfocusEditId();
	afx_msg void OnKillfocusEditLabel();
	afx_msg void OnCheckNew();
	afx_msg void OnSelchangeComboOptions();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void CopyDataToMelGen( CPropMelGen* pMelGen );
	void GetDataFromMelGen( CPropMelGen* pMelGen );

protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();
	void UpdateID();
	bool AdjustTime( long& lNewMeasure, long& lNewBeat, int nDelta, bool fByMeasure );

private:
	CPropMelGen*			m_pMelGen;
	CMelGenPropPageMgr*	m_pPropPageMgr;
	IDMUSProdFramework*		m_pIFramework;
	BOOL					m_fHaveData;
	BOOL					m_fMultipleMelGensSelected;
	BOOL					m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGEMELGEN_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
