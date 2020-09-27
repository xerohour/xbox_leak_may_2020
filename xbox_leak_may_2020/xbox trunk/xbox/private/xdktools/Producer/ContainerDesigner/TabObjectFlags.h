#if !defined(AFX_TABOBJECTFLAGS_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABOBJECTFLAGS_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabObjectFlags.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabObjectFlags dialog

class CTabObjectFlags : public CPropertyPage
{
// Construction
public:
	CTabObjectFlags( CObjectPropPageManager* pPageManager );
	~CTabObjectFlags();
	void SetContainerObject( CContainerObject* pContainerObject );

// Dialog Data
	//{{AFX_DATA(CTabObjectFlags)
	enum { IDD = IDD_TAB_OBJECT_FLAGS };
	CButton	m_checkCache;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabObjectFlags)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CContainerObject*		m_pContainerObject;
	CObjectPropPageManager*	m_pPageManager;
	BOOL					m_fNeedToDetach;

// Implementation
protected:
	void SetModifiedFlag();
	void EnableControls( BOOL fEnable );

	// Generated message map functions
	//{{AFX_MSG(CTabObjectFlags)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCache();
	afx_msg void OnDoubleClickedCache();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABOBJECTFLAGS_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
