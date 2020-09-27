#if !defined(AFX_TABPERSONALITYINFO_H__9A049C9D_E48E_11D1_988C_00805FA67D16__INCLUDED_)
#define AFX_TABPERSONALITYINFO_H__9A049C9D_E48E_11D1_988C_00805FA67D16__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabPersonalityInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo dialog

class CTabPersonalityInfo : public CPropertyPage
{

// Construction
public:
	CTabPersonalityInfo(CPersonalityPageManager* pPageManager);
	~CTabPersonalityInfo();
	void SetPersonality( CPersonality* pPersonality );

// Dialog Data
	//{{AFX_DATA(CTabPersonalityInfo)
	enum { IDD = IDD_TAB_PERSONALITY_INFO };
	CEdit	m_editVersion_4;
	CEdit	m_editVersion_3;
	CEdit	m_editVersion_2;
	CEdit	m_editVersion_1;
	CEdit	m_editSubject;
	CEdit	m_editInfo;
	CEdit	m_editCopyright;
	CEdit	m_editAuthor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabPersonalityInfo)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	CPersonality*		m_pPersonality;
	CPersonalityPageManager*	m_pPageManager;
	BOOL		m_fNeedToDetach;


// Implementation
protected:
	void SetModifiedFlag();
	void EnableControls( BOOL fEnable );
	void UpdateControls();

	
	// Generated message map functions
	//{{AFX_MSG(CTabPersonalityInfo)
	afx_msg void OnKillFocusVersion_4();
	afx_msg void OnKillFocusVersion_3();
	afx_msg void OnKillFocusVersion_2();
	afx_msg void OnKillFocusVersion_1();
	afx_msg void OnKillFocusSubject();
	afx_msg void OnKillFocusInfo();
	afx_msg void OnKillFocusCopyright();
	afx_msg void OnKillFocusAuthor();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnEditGuid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABPERSONALITYINFO_H__9A049C9D_E48E_11D1_988C_00805FA67D16__INCLUDED_)
