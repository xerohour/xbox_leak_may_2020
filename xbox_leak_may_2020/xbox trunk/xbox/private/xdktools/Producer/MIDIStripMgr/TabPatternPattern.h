#if !defined(AFX_TABPATTERNPATTERN_H__819A7F02_7332_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABPATTERNPATTERN_H__819A7F02_7332_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabPatternPattern.h : header file
//

class CDirectMusicPattern;
interface IDMUSProdPropSheet;

/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern dialog

class CTabPatternPattern : public CPropertyPage
{
// Construction
public:
	CTabPatternPattern( void );
	virtual ~CTabPatternPattern();
	void SetPattern( CDirectMusicPattern* pPattern );

// Dialog Data
	//{{AFX_DATA(CTabPatternPattern)
	enum { IDD = IDD_TAB_PATTERN_PATTERN };
	CButton	m_btnResetVarOrder;
	CButton	m_btnTimeSignature;
	CButton	m_btnWhole;
	CButton	m_btnQuarter;
	CButton	m_btnHalf;
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
	BOOL						m_fNeedToDetach;

public:
	// Variables for keeping track of the active property tab (if there is more than one)
	static short*				sm_pnActiveTab;
	IDMUSProdPropSheet*			m_pIPropSheet;

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
	afx_msg void OnTimeSignature();
	afx_msg void OnCheckResetVarOrder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABPATTERNPATTERN_H__819A7F02_7332_11D1_89AE_00A0C9054129__INCLUDED_)
