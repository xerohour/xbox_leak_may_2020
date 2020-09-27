#if !defined(AFX_PROPPAGETEMPO_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_PROPPAGETEMPO_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include "PropTempo.h"

class CTempoPropPageMgr;
class CTempoMgr;

// PropPageTempo.h : header file
//

class CMyEdit :
	public CEdit
{
	DECLARE_DYNCREATE(CMyEdit)
public:
	CMyEdit();

	// for processing Windows messages
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
};

/////////////////////////////////////////////////////////////////////////////
// PropPageTempo dialog
class CTempoPropPageMgr;

class PropPageTempo : public CPropertyPage
{
	friend CTempoPropPageMgr;
	DECLARE_DYNCREATE(PropPageTempo)

// Construction
public:
	PropPageTempo();
	~PropPageTempo();

// Dialog Data
	//{{AFX_DATA(PropPageTempo)
	enum { IDD = IDD_TEMPO_PROPPAGE };
	CSpinButtonCtrl	m_spinTick;
	CMyEdit	m_editTick;
	CSpinButtonCtrl	m_spinTempo;
	CEdit	m_editBeat;
	CEdit	m_editTempo;
	CEdit	m_editMeasure;
	CSpinButtonCtrl	m_spinMeasure;
	CSpinButtonCtrl	m_spinBeat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPageTempo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropPageTempo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditBeat();
	afx_msg void OnKillfocusEditTempo();
	afx_msg void OnKillfocusEditMeasure();
	afx_msg void OnDeltaposSpinTempo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditTick();
	afx_msg void OnDeltaposSpinTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateEditTempo();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetTempo( const CPropTempo * pTempo );

protected:
	void UpdateObject( void );
	void EnableControls( BOOL fEnable );

	BOOL					m_fValidTempo;
	BOOL					m_fMultipleSelect;
	CPropTempo				m_Tempo;
	CTempoPropPageMgr *		m_pPropPageMgr;
	BOOL					m_fNeedToDetach;
};

class CLockoutNotification
{
public:
	CLockoutNotification( HWND hWnd);
	~CLockoutNotification();

	BOOL				m_fReset;
	_AFX_THREAD_STATE*	m_pThreadState;
	HWND				m_hWndOldLockout;

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGETEMPO_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
