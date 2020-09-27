#if !defined(AFX_WAVEINFOPROPPG_H__2AB46082_3441_11D1_8B4A_006097B01078__INCLUDED_)
#define AFX_WAVEINFOPROPPG_H__2AB46082_3441_11D1_8B4A_006097B01078__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WaveInfoPropPg.h : header file
//
#include "resource.h"

const int COMMENT_TEXT_LIMIT = 1024;

class CWave;
/////////////////////////////////////////////////////////////////////////////
// CWaveInfoPropPg dialog
class CWaveInfoPropPg : public CPropertyPage
{
	DECLARE_DYNCREATE(CWaveInfoPropPg)

// Construction
public:
	CWaveInfoPropPg();
	~CWaveInfoPropPg();

	void SetObject(CWave* pWave) {m_pWave = pWave;}
	void InitializeDialogValues();

// Dialog Data
	//{{AFX_DATA(CWaveInfoPropPg)
	enum { IDD = IDD_WAVE_INFO_PROP_PAGE };
	CString	m_csAuthor;
	CString	m_csComment;
	CString	m_csCopyright;
	CString	m_csSubject;
	DWORD	m_dwLength;
	DWORD	m_dwSampleSize;
	DWORD	m_dwSampleRate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWaveInfoPropPg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWaveInfoPropPg)
	afx_msg void OnChangeAuthor();
	afx_msg void OnChangeComment();
	afx_msg void OnChangeCopyright();
	afx_msg void OnChangeSubject();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillfocusEdits();
	virtual BOOL OnInitDialog();
	afx_msg void OnEditGuidButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    CWave*	m_pWave;
	BOOL	m_fNeedToDetach;
	bool	m_bSaveUndoState;
	bool	m_bInInitialUpdate;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVEINFOPROPPG_H__2AB46082_3441_11D1_8B4A_006097B01078__INCLUDED_)
