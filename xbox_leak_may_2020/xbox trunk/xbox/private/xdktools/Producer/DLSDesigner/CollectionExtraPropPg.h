#if !defined(AFX_COLLECTIONEXTRAPROPPG_H__6E263AA4_ECD6_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_COLLECTIONEXTRAPROPPG_H__6E263AA4_ECD6_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CollectionExtraPropPg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CCollectionExtraPropPg dialog

class CCollectionExtraPropPg : public CPropertyPage
{
	DECLARE_DYNCREATE(CCollectionExtraPropPg)

// Construction
public:
	CCollectionExtraPropPg();
	~CCollectionExtraPropPg();

	void SetObject(CCollection* pCollection) {m_pCollection = pCollection;}

	void EnableControls(BOOL fEnable);

// Dialog Data
	//{{AFX_DATA(CCollectionExtraPropPg)
	enum { IDD = IDD_COLLECTION_EXTRA_PROP_PAGE };
	DWORD	m_dwArticCount;
	DWORD	m_dwInstCount;
	DWORD	m_dwRegionCount;
	DWORD	m_dwSampleCount;
	DWORD	m_dwWaveCount;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCollectionExtraPropPg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCollectionExtraPropPg)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CCollection* m_pCollection;	
	BOOL m_fNeedToDetach;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLLECTIONEXTRAPROPPG_H__6E263AA4_ECD6_11D0_876A_00AA00C08146__INCLUDED_)
