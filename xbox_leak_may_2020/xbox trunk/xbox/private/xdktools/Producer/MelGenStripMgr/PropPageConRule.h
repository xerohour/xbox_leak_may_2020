#if !defined(AFX_PROPPAGECONRULE_H__0D9F2D4E_BACB_11D2_9801_00C04FA36E58__INCLUDED_)
#define AFX_PROPPAGECONRULE_H__0D9F2D4E_BACB_11D2_9801_00C04FA36E58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <afxtempl.h>

// PropPageConRule.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PropPageConRule dialog
class CMelGenPropPageMgr;

class PropPageConRule : public CPropertyPage
{
	friend CMelGenPropPageMgr;
	DECLARE_DYNCREATE(PropPageConRule)

// Construction
public:
	PropPageConRule();
	~PropPageConRule();

// Dialog Data
	//{{AFX_DATA(PropPageConRule)
	enum { IDD = IDD_CONNECTION_RULES_PROPPAGE };
	CListBox	m_listTransIntervals;
	CListBox	m_listTransOptions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPageConRule)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PropPageConRule)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListTransitionOptions();
	afx_msg void OnSelchangeListTransitions();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void CopyDataToMelGen( CPropMelGen* pMelGen );
	void GetDataFromMelGen( CPropMelGen* pMelGen );

protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();

private:
	CPropMelGen*			m_pMelGen;
	CMelGenPropPageMgr*	m_pPropPageMgr;
	IDMUSProdFramework*		m_pIFramework;
	BOOL					m_fHaveData;
	BOOL					m_fMultipleMelGensSelected;
	BOOL					m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGECONRULE_H__0D9F2D4E_BACB_11D2_9801_00C04FA36E58__INCLUDED_)
