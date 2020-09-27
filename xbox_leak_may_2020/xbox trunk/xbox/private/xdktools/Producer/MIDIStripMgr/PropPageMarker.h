#if !defined(AFX_PROPPAGEMARKER_H__742200C0_5B21_4850_B0A2_2324D6C2579C__INCLUDED_)
#define AFX_PROPPAGEMARKER_H__742200C0_5B21_4850_B0A2_2324D6C2579C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

// PropPageMarker.h : header file
//

extern class CMIDIStripMgrApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPropPageMarker dialog

class CPropPageMarker : public CPropertyPage
{
// Construction
public:
	CPropPageMarker( CMarkerPropPageMgr *pMarkerPropPageMgr );
	~CPropPageMarker();

// Dialog Data
	//{{AFX_DATA(CPropPageMarker)
	enum { IDD = IDD_PROPPAGE_MARKER };
	CButton	m_btnEnterChord;
	CButton	m_btnExit;
	CButton	m_btnEnter;
	CButton	m_btnExitChord;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPageMarker)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Member Variables
protected:
	CMarkerPropPageMgr*		m_pPageManager;
	BOOL					m_fInUpdateControls;
	BOOL					m_fNeedToDetach;

// Implementation
public:
	void UpdateControls( CPropSwitchPoint* pPropSwitchPoint );

protected:
	void EnableItem( int nID, BOOL fEnable );
	void EnableControls( BOOL fEnable );
	void UpdatePPO( void );

protected:
	// Generated message map functions
	//{{AFX_MSG(CPropPageMarker)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGEMARKER_H__742200C0_5B21_4850_B0A2_2324D6C2579C__INCLUDED_)
