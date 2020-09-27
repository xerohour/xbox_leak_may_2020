#if !defined(AFX_PROPPAGEPERS_REF_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_PROPPAGEPERS_REF_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include <afxtempl.h>


class CPersRefPropPageMgr;

// PropPagePersRef.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PropPagePersRef dialog
class CPersRefPropPageMgr;

class PropPagePersRef : public CPropertyPage
{
	friend CPersRefPropPageMgr;
	DECLARE_DYNCREATE(PropPagePersRef)

// Construction
public:
	PropPagePersRef();
	PropPagePersRef(CPersRefPropPageMgr *pMgr, IDMUSProdFramework* pIFramework);
	~PropPagePersRef();

// Dialog Data
	//{{AFX_DATA(PropPagePersRef)
	enum { IDD = IDD_PERS_REF_PROPPAGE };
	CComboBox	m_comboPers;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPagePersRef)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropPagePersRef)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboPerss();
	afx_msg void OnDropDownComboPers();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void CopyDataToPersRef( CPropPersRef* pPersRef );
	void GetDataFromPersRef( CPropPersRef* pPersRef );

protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();
	void SetPersComboBoxSelection();
	void GetComboBoxText( PersListInfo* pPersListInfo, CString& strText );
	void InsertPersInfoListInComboBox();
	void InsertPersInfo( PersListInfo* pPersListInfo );
	void BuildPersInfoList();


private:
	CPropPersRef*			m_pPersRef;
	CPersRefPropPageMgr *	m_pPropPageMgr;
	IDMUSProdFramework*		m_pIFramework;
	BOOL					m_fNeedToDetach;
	BOOL					m_fIgnoreSelChange;
	BOOL					m_fHaveData;
	BOOL					m_fMultipleStylesSelected;
    CTypedPtrList<CPtrList, PersListInfo*> m_lstPersListInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGEPERS_REF_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
