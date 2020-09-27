#if !defined(AFX_PROPPAGECURVEVAR_H__3251F4C4_DA91_11D1_89B1_00C04FD912C8__INCLUDED_)
#define AFX_PROPPAGECURVEVAR_H__3251F4C4_DA91_11D1_89B1_00C04FD912C8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PropPageCurveVar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveVar dialog

class CPropPageCurveVar : public CPropertyPage
{
// Construction
public:
	CPropPageCurveVar( CCurvePropPageMgr* pCurvePropPageMgr );
	~CPropPageCurveVar();

// Dialog Data
	//{{AFX_DATA(CPropPageCurveVar)
	enum { IDD = IDD_PROPPAGE_CURVE_VARIATION };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPageCurveVar)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Member Variables
protected:
	CCurvePropPageMgr*	m_pPageManager;
	DWORD				m_dwVariation;
	DWORD				m_dwVarUndetermined;
	BOOL				m_fNeedToDetach;

// Implementation
protected:
	void EnableControls( BOOL fEnable );

public:
	void UpdateControls( CPropCurve* pPropCurve );

	// Generated message map functions
	//{{AFX_MSG(CPropPageCurveVar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGECURVEVAR_H__3251F4C4_DA91_11D1_89B1_00C04FD912C8__INCLUDED_)
