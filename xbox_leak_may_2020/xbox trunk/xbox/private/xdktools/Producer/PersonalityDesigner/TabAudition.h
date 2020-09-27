#if !defined(AFX_TABAUDITION_H__2B10A302_F7BB_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABAUDITION_H__2B10A302_F7BB_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabAudition.h : header file
//

#include "Personality.h"
#include "TabPersonality.h"


/////////////////////////////////////////////////////////////////////////////
// CTabAudition structures

#pragma pack(2)

typedef struct StyleListInfo
{
	IDMUSProdProject*	pIProject;
	CString			strProjectName;
	CString			strName;
	CString			strDescriptor;
	GUID			guidFile;
} StyleListInfo;

#pragma pack()


class CPersonalityPageManager;

/////////////////////////////////////////////////////////////////////////////
// CTabAudition dialog

class CTabAudition : public CPropertyPage
{
// Construction
public:
	CTabAudition( CPersonalityPageManager* pPageManager, IDMUSProdFramework* pIFramework );
	~CTabAudition();

// Dialog Data
	//{{AFX_DATA(CTabAudition)
	enum { IDD = IDD_TAB_AUDITION };
	CSpinButtonCtrl	m_spinGroove;
	CEdit	m_editGroove;
	CComboBox	m_comboStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabAudition)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Member variables
protected:
	IDMUSProdFramework*			 m_pIFramework;
	CPersonalityPageManager* m_pPageManager;

	BOOL		m_fNeedToDetach;
	BOOL		m_fHaveData;
	BOOL		m_fInOnSetActive;
	IDMUSProdNode*	m_pIStyleDocRootNode;
	DWORD		m_dwGroove;
	GUID		m_guidProject;
	CPersonality* m_pPersonality;

    CTypedPtrList<CPtrList, StyleListInfo*> m_lstStyleListInfo;

// Implementation
protected:
	void GetComboBoxText( StyleListInfo* pStyleListInfo, CString& strText );
	void InsertStyleInfo( StyleListInfo* pStyleListInfo );
	void BuildStyleInfoList();
	void InsertStyleInfoListInComboBox();
	void SetGroove();
	void SetStyle();

public:
	void SetStyleDocRootNode( void );
	void CopyDataToTab( tabPersonality* pTabData );
	void GetDataFromTab( tabPersonality* pTabData );

protected:
	void EnableControls( BOOL fEnable );

	// Generated message map functions
	//{{AFX_MSG(CTabAudition)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeComboStyle();
	afx_msg void OnDropDownComboStyle();
	afx_msg void OnUpdateEditGroove();
	afx_msg void OnDeltaposSpinGroove(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABAUDITION_H__2B10A302_F7BB_11D0_89AE_00A0C9054129__INCLUDED_)
