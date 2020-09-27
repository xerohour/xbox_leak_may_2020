#if !defined(AFX_TABBAND_H__E6072F43_0C3B_11D2_89B3_00C04FD912C8__INCLUDED_)
#define AFX_TABBAND_H__E6072F43_0C3B_11D2_89B3_00C04FD912C8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabBand.h : header file
//

class CBandPropPageManager;

/////////////////////////////////////////////////////////////////////////////
// CTabBand dialog

class CTabBand : public CPropertyPage
{
// Construction
public:
	CTabBand( CBandPropPageManager* pBandPropPageManager );
	virtual ~CTabBand();
	void SetBand( CBand* pBand);

// Dialog Data
	//{{AFX_DATA(CTabBand)
	enum { IDD = IDD_TAB_BAND };
	CEdit	m_editName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabBand)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CBand*					m_pBand;
	CBandPropPageManager*	m_pPageManager;
	BOOL					m_fNeedToDetach;

// Implementation
protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CTabBand)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocusName();
	afx_msg void OnEditGuid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABBAND_H__E6072F43_0C3B_11D2_89B3_00C04FD912C8__INCLUDED_)
