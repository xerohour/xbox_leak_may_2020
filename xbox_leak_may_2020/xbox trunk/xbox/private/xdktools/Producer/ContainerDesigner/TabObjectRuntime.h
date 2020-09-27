#if !defined(AFX_TABOBJECTRUNTIME_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABOBJECTRUNTIME_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabObjectRuntime.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabObjectRuntime dialog

class CTabObjectRuntime : public CPropertyPage
{
// Construction
public:
	CTabObjectRuntime( CObjectPropPageManager* pPageManager );
	~CTabObjectRuntime();
	void SetContainerObject( CContainerObject* pContainerObject );

// Dialog Data
	//{{AFX_DATA(CTabObjectRuntime)
	enum { IDD = IDD_TAB_OBJECT_RUNTIME };
	CEdit	m_editRuntimeFile;
	CStatic	m_staticRuntimeCreate;
	CStatic	m_staticRuntimeSize;
	CStatic	m_staticRuntimeModified;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabObjectRuntime)
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
	void EnableControls( BOOL fEnable );

	// Generated message map functions
	//{{AFX_MSG(CTabObjectRuntime)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABOBJECTRUNTIME_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
