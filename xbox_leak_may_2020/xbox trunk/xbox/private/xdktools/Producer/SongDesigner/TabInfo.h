#if !defined(AFX_TABINFO_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABINFO_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabInfo.h : header file
//

#include "resource.h"

class CSongPropPageManager;
class CDirectMusicSong;

/////////////////////////////////////////////////////////////////////////////
// CTabInfo dialog

class CTabInfo : public CPropertyPage
{
// Construction
public:
	CTabInfo( CSongPropPageManager* pSongPropPageManager );
	virtual ~CTabInfo();
	void SetSong( CDirectMusicSong* pSong );

// Dialog Data
	//{{AFX_DATA(CTabInfo)
	enum { IDD = IDD_TAB_INFO };
	CButton	m_btnGUID;
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
	//{{AFX_VIRTUAL(CTabInfo)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CDirectMusicSong*		m_pSong;
	CSongPropPageManager*	m_pPageManager;
	BOOL					m_fNeedToDetach;

// Implementation
protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CTabInfo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocusAuthor();
	afx_msg void OnKillFocusCopyright();
	afx_msg void OnKillFocusSubject();
	afx_msg void OnKillFocusInfo();
	afx_msg void OnKillFocusVersion_1();
	afx_msg void OnKillFocusVersion_2();
	afx_msg void OnKillFocusVersion_3();
	afx_msg void OnKillFocusVersion_4();
	afx_msg void OnEditGuid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABINFO_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
