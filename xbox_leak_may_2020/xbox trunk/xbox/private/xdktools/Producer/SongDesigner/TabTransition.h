#if !defined(AFX_TABTRANSITION_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABTRANSITION_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// TabTransition.h : header file
//

typedef struct _PPGTabTransition
{
	_PPGTabTransition( void )
	{
		dwPageIndex = 1;
		dwToSegmentFlag = 0;
		pToSegment = NULL;
		pTransitionSegment = NULL;
		pSong = NULL;
		dwFlagsUI = 0;
	}

	DWORD				dwPageIndex;
	DWORD				dwToSegmentFlag;
	CVirtualSegment*	pToSegment;
	CVirtualSegment*	pTransitionSegment;
	CDirectMusicSong*	pSong;
	DWORD				dwFlagsUI;
} PPGTabTransition;


/////////////////////////////////////////////////////////////////////////////
// CTabTransition dialog

class CTabTransition : public CPropertyPage
{
// Construction
public:
	CTabTransition( IDMUSProdPropPageManager* pIPageManager );
	~CTabTransition();

// Dialog Data
	//{{AFX_DATA(CTabTransition)
	enum { IDD = IDD_TAB_TRANSITION };
	CStatic	m_staticIntroPrompt;
	CButton	m_checkIntro;
	CComboBox	m_comboToSegment;
	CComboBox	m_comboTransitionSegment;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabTransition)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTabTransition)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelChangeComboToSegment();
	afx_msg void OnSelChangeComboTransitionSegment();
	afx_msg void OnCheckIntro();
	afx_msg void OnDoubleClickedCheckIntro();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( BOOL fEnable );
	void UpdateObject();
	void FillToSegmentComboBox();
	void FillTransitionSegmentComboBox();
	void SelectToSegment();
	void SelectTransitionSegment();

public:
	void SetObject( IDMUSProdPropPageObject* pIPropPageObject );
	void RefreshTab( void );

protected:
	IDMUSProdPropPageManager*	m_pIPageManager;
	IDMUSProdPropPageObject*	m_pIPropPageObject;
	PPGTabTransition			m_PPGTabTransition;
	BOOL						m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABTRANSITION_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
