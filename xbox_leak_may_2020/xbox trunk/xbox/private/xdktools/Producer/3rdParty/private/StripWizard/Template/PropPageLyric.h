#if !defined(AFX_PROPPAGE$$SAFE_ROOT$$_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_PROPPAGE$$SAFE_ROOT$$_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"

class C$$Safe_root$$PropPageMgr;
class CTabOnReturnEdit;

// PropPage$$Safe_root$$.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$ dialog
class C$$Safe_root$$PropPageMgr;

class PropPage$$Safe_root$$ : public CPropertyPage
{
	friend C$$Safe_root$$PropPageMgr;
	DECLARE_DYNCREATE(PropPage$$Safe_root$$)

// Construction
public:
	PropPage$$Safe_root$$();
	~PropPage$$Safe_root$$();

// Dialog Data
	//{{AFX_DATA(PropPage$$Safe_root$$)
	enum { IDD = IDD_$$SAFE_ROOT$$_PROPPAGE };
	CEdit	m_editBeat;
	CEdit	m_edit$$Safe_root$$;
	CEdit	m_editMeasure;
	CSpinButtonCtrl	m_spinMeasure;
	CSpinButtonCtrl	m_spinBeat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPage$$Safe_root$$)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropPage$$Safe_root$$)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditBeat();
	afx_msg void OnKillfocusEdit$$Safe_root$$();
	afx_msg void OnKillfocusEditMeasure();
	virtual void OnOK();
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Set$$Safe_root$$( const C$$Safe_root$$Item * p$$Safe_root$$ );

protected:
	void UpdateObject( void );
	void EnableControls( BOOL fEnable );
	void HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal );
	void HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal );

	BOOL					m_fNeedToDetach;
	BOOL					m_fValid$$Safe_root$$;
	C$$Safe_root$$Item				m_$$Safe_root$$;
	C$$Safe_root$$PropPageMgr *		m_pPropPageMgr;
	IDMUSProdTimeline*		m_pTimeline; // Weak timeline reference
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGE$$SAFE_ROOT$$_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
