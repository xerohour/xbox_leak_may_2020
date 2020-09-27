#if !defined(AFX_PROPPAGESTYLE_REF_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_PROPPAGESTYLE_REF_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include <afxtempl.h>


class CStyleRefPropPageMgr;

// PropPageStyleRef.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PropPageStyleRef dialog
class CStyleRefPropPageMgr;

class PropPageStyleRef : public CPropertyPage
{
	friend CStyleRefPropPageMgr;
	DECLARE_DYNCREATE(PropPageStyleRef)

// Construction
public:
	PropPageStyleRef();
	~PropPageStyleRef();

// Dialog Data
	//{{AFX_DATA(PropPageStyleRef)
	enum { IDD = IDD_STYLE_REF_PROPPAGE };
	CComboBox	m_comboStyle;
	CEdit	m_editMeasure;
	CSpinButtonCtrl	m_spinMeasure;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPageStyleRef)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropPageStyleRef)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditMeasure();
	afx_msg void OnSelchangeComboStyles();
	afx_msg void OnDropDownComboStyle();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void CopyDataToStyleRef( CPropStyleRef* pStyleRef );
	void GetDataFromStyleRef( CPropStyleRef* pStyleRef );

protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();
	void SetStyleComboBoxSelection();
	void GetComboBoxText( const StyleListInfo* pStyleListInfo, CString& strText );
	void InsertStyleInfoListInComboBox();
	void InsertStyleInfo( StyleListInfo* pStyleListInfo );
	void BuildStyleInfoList();
	int AdjustTime( int nNewValue, int nDelta );

private:
	CPropStyleRef*			m_pStyleRef;
	CStyleRefPropPageMgr*	m_pPropPageMgr;
	IDMUSProdFramework*		m_pIFramework;
	BOOL					m_fIgnoreSelChange;
	BOOL					m_fHaveData;
	BOOL					m_fMultipleStylesSelected;
	BOOL					m_fNeedToDetach;
    CTypedPtrList<CPtrList, StyleListInfo*> m_lstStyleListInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGESTYLE_REF_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
