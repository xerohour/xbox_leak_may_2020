#if !defined(AFX_TABAUDIOPATHAUDIOPATH_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABAUDIOPATHAUDIOPATH_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabAudioPathAudioPath.h : header file
//

#include "resource.h"

class CAudioPathPropPageManager;
class CDirectMusicAudioPath;

/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathAudioPath dialog

class CTabAudioPathAudioPath : public CPropertyPage
{
// Construction
public:
	CTabAudioPathAudioPath( CAudioPathPropPageManager* pAudioPathPropPageManager );
	virtual ~CTabAudioPathAudioPath();
	void SetAudioPath( CDirectMusicAudioPath* pAudioPath );

// Dialog Data
	//{{AFX_DATA(CTabAudioPathAudioPath)
	enum { IDD = IDD_TAB_AUDIOPATH_AUDIOPATH };
	CEdit	m_editName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabAudioPathAudioPath)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CDirectMusicAudioPath*		m_pAudioPath;
	CAudioPathPropPageManager*	m_pPageManager;
	BOOL					m_fNeedToDetach;

// Implementation
protected:
	void SetModifiedFlag();
	void EnableControls( BOOL fEnable );
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CTabAudioPathAudioPath)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocusName();
	virtual BOOL OnInitDialog();
	//afx_msg void On11();
	//afx_msg void On22();
	//afx_msg void On44();
	//afx_msg void On48();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABAUDIOPATHAUDIOPATH_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
