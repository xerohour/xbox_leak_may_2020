#if !defined(AFX_PROPPAGETIMESIG_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_PROPPAGETIMESIG_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include <afxtempl.h>

class CTimeSigPropPageMgr;

// PropPageTimeSig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PropPageTimeSig dialog
class CTimeSigPropPageMgr;

class PropPageTimeSig : public CPropertyPage
{
	friend CTimeSigPropPageMgr;
	DECLARE_DYNCREATE(PropPageTimeSig)

// Construction
public:
	PropPageTimeSig();
	~PropPageTimeSig();

// Dialog Data
	//{{AFX_DATA(PropPageTimeSig)
	enum { IDD = IDD_TIMESIG_PROPPAGE };
	CStatic	m_InfoStatic;
	CButton	m_btnTimeSignature;
	CEdit	m_editMeasure;
	CSpinButtonCtrl	m_spinMeasure;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPageTimeSig)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropPageTimeSig)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditMeasure();
	afx_msg void OnTimeSignature();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void CopyDataToTimeSig( CPropTimeSig* pTimeSig );
	void GetDataFromTimeSig( CPropTimeSig* pTimeSig );

protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();
	void SetTimeSignatureBitmap( void );
	int  AdjustTime( int nNewValue, int nDelta );
	BOOL HasStyleTrack();

private:
	CPropTimeSig*			m_pTimeSig;
	CTimeSigPropPageMgr*	m_pPropPageMgr;
	IDMUSProdFramework*		m_pIFramework;
	BOOL					m_fHaveData;
	BOOL					m_fMultipleTimeSigsSelected;
	BOOL					m_fNeedToDetach;
	BOOL					m_bHasStyleTrack;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGETIMESIG_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
