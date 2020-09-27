#if !defined(AFX_TABBOUNDARY_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_TABBOUNDARY_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

// TabBoundary.h : header file
//

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include <afxtempl.h>

class CPropPageMgrItem;
class CTabOnReturnEdit;


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary dialog

class CTabBoundary : public CPropertyPage
{
	friend CPropPageMgrItem;
	DECLARE_DYNCREATE(CTabBoundary)

// Construction
public:
	CTabBoundary();
	~CTabBoundary();

// Dialog Data
	//{{AFX_DATA(CTabBoundary)
	enum { IDD = IDD_PROPPAGE_WAVE_BOUNDARY };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabBoundary)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabBoundary)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetTrackItem( const CTrackItem * pItem );

protected:
	void UpdateObject();
	void EnableControls( BOOL fEnable );

	IDMUSProdTimeline*		m_pTimeline;		// Weak timeline reference
	CPropPageMgrItem*		m_pPropPageMgr;

	CTrackItem				m_TrackItem;

	BOOL					m_fNeedToDetach;
	BOOL					m_fValidTrackItem;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABBOUNDARY_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
