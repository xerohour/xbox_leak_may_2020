#if !defined(AFX_TABSONG_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABSONG_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabSong.h : header file
//

#include "resource.h"

class CSongPropPageManager;
class CDirectMusicSong;

/////////////////////////////////////////////////////////////////////////////
// CTabSong dialog

class CTabSong : public CPropertyPage
{
// Construction
public:
	CTabSong( CSongPropPageManager* pSongPropPageManager );
	virtual ~CTabSong();
	void SetSong( CDirectMusicSong* pSong );

// Dialog Data
	//{{AFX_DATA(CTabSong)
	enum { IDD = IDD_TAB_SONG };
	CEdit	m_editName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabSong)
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
	//{{AFX_MSG(CTabSong)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocusName();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSONG_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
