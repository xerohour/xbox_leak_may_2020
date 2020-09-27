#if !defined(AFX_TABPATTERNPATTERN_H__819A7F02_7332_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABPATTERNPATTERN_H__819A7F02_7332_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabPatternPattern.h : header file
//

class CPatternPropPageManager;
class CDirectMusicPattern;

/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern dialog

class CTabPatternPattern : public CPropertyPage
{
// Construction
public:
	CTabPatternPattern( CPatternPropPageManager* pPatternPropPageManager );
	virtual ~CTabPatternPattern();
	void RefreshTab( IDMUSProdPropPageObject* pIPropPageObject );

// Dialog Data
	//{{AFX_DATA(CTabPatternPattern)
	enum { IDD = IDD_TAB_PATTERN_PATTERN };
	CButton	m_btnCustom;
	CEdit	m_editCustomId;
	CSpinButtonCtrl	m_spinCustomId;
	CSpinButtonCtrl	m_spinDestTop;
	CSpinButtonCtrl	m_spinDestBottom;
	CEdit	m_editDestTop;
	CEdit	m_editDestBottom;
	CButton	m_btnTimeSignature;
	CButton	m_btnRhythmMap;
	CButton	m_btnCustomDlg;
	CButton	m_btnIntro;
	CButton	m_btnFill;
	CButton	m_btnEnd;
	CButton	m_btnBreak;
	CButton	m_btnWhole;
	CButton	m_btnQuarter;
	CButton	m_btnHalf;
	CSpinButtonCtrl	m_spinGrooveTop;
	CEdit	m_editGrooveTop;
	CSpinButtonCtrl	m_spinGrooveBottom;
	CEdit	m_editGrooveBottom;
	CEdit	m_editName;
	CButton	m_btnLength;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabPatternPattern)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CDirectMusicPattern*		m_pPattern;
	CPatternPropPageManager*	m_pPageManager;
	BOOL						m_fNeedToDetach;

// Implementation
protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();
	void SetTimeSignatureBitmap();

	// Generated message map functions
	//{{AFX_MSG(CTabPatternPattern)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocusName();
	afx_msg void OnLength();
	afx_msg void OnKillFocusGrooveBottom();
	afx_msg void OnDeltaPosGrooveBottomSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusGrooveTop();
	afx_msg void OnDeltaPosGrooveTopSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEmbIntro();
	afx_msg void OnEmbFill();
	afx_msg void OnEmbBreak();
	afx_msg void OnEmbEnd();
	afx_msg void OnCustomDlg();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnTimeSignature();
	afx_msg void OnKillFocusDestBottom();
	afx_msg void OnDeltaPosDestBottomSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusDestTop();
	afx_msg void OnDeltaPosDestTopSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEmbCustom();
	afx_msg void OnKillFocusCustomId();
	afx_msg void OnDeltaPosCustomIdSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABPATTERNPATTERN_H__819A7F02_7332_11D1_89AE_00A0C9054129__INCLUDED_)
