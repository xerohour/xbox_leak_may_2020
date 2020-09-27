#if !defined(AFX_TABVARIATIONS_H__06A5F6C5_B758_11D1_89AF_00C04FD912C8__INCLUDED_)
#define AFX_TABVARIATIONS_H__06A5F6C5_B758_11D1_89AF_00C04FD912C8__INCLUDED_

// CTabVariations.h : header file
//

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include <afxtempl.h>

class CPropPageMgrItem;
class CPropTrackItem;


/////////////////////////////////////////////////////////////////////////////
// CTabVariations dialog

class CTabVariations : public CPropertyPage
{
friend CPropPageMgrItem;

// Construction
public:
	CTabVariations();
	~CTabVariations();

// Dialog Data
	//{{AFX_DATA(CTabVariations)
	enum { IDD = IDD_PROPPAGE_WAVE_VARIATIONS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabVariations)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTabVariations)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetPropTrackItem( const CPropTrackItem* pPropItem );

protected:
	void UpdateObject();
	void EnableControls( BOOL fEnable );

protected:
	CPropPageMgrItem*		m_pPropPageMgr;

	CPropTrackItem			m_PropItem;

	BOOL					m_fNeedToDetach;
	BOOL					m_fValidTrackItem;
	BOOL					m_fAllDisabled;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABVARIATIONS_H__06A5F6C5_B758_11D1_89AF_00C04FD912C8__INCLUDED_)
