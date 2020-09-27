#if !defined(AFX_COLLECTIONPROPPG_H__5342FA63_E1AB_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_COLLECTIONPROPPG_H__5342FA63_E1AB_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CollectionPropPg.h : header file
//

#include "resource.h"

const int COMMENT_TEXT_LIMIT = 1024;

class CCollection;

/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPg dialog
class CCollectionPropPg : public CPropertyPage
{
	friend class CInstrument;
	
	DECLARE_DYNCREATE(CCollectionPropPg)

// Construction
public:
	CCollectionPropPg();
	~CCollectionPropPg();
	
	void SetObject(CCollection* pCollection) {m_pCollection = pCollection;}

	void EnableControls(BOOL fEnable);

// Dialog Data
	//{{AFX_DATA(CCollectionPropPg)
	enum { IDD = IDD_COLLECTION_PROP_PAGE };
	CString	m_csComment;
	CString	m_csCopyright;
	CString	m_csEngineer;
	CString	m_csName;
	CString	m_csSubject;
	DWORD	m_dwVersion1;
	DWORD	m_dwVersion2;
	DWORD	m_dwVersion3;
	DWORD	m_dwVersion4;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCollectionPropPg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCollectionPropPg)
	afx_msg void OnChangeComment();
	afx_msg void OnChangeCopyright();
	afx_msg void OnChangeEngineer();
	afx_msg void OnChangeName();
	afx_msg void OnChangeSubject();
	afx_msg void OnChangeVer1();
	afx_msg void OnChangeVer2();
	afx_msg void OnChangeVer3();
	afx_msg void OnChangeVer4();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillfocusName();
	afx_msg void OnEditGuid();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CCollection* m_pCollection;	
	BOOL m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLLECTIONPROPPG_H__5342FA63_E1AB_11D0_876A_00AA00C08146__INCLUDED_)
