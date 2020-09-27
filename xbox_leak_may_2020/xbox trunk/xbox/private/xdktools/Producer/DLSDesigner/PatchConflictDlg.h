#if !defined(AFX_PATCHCONFLICTDLG_H__A077F003_C94B_11D1_B987_006097B01078__INCLUDED_)
#define AFX_PATCHCONFLICTDLG_H__A077F003_C94B_11D1_B987_006097B01078__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PatchConflictDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatchConflictDlg dialog

#include "resource.h"
#include "Instrument.h"

class CPatchConflictDlg : public CDialog
{
// Construction
public:
	CPatchConflictDlg(HRESULT hrCode, CInstrument* pCollidingInstrument, CCollection* pLoadingCollection, CInstrument* pParentInstrument = NULL);
	
// Dialog Data
	//{{AFX_DATA(CPatchConflictDlg)
	enum { IDD = IDD_PATCH_CONFLICT };
	CButton	m_NotAgainCheck;
	UINT	m_Patch;
	UINT	m_LSB;
	UINT	m_MSB;
	BOOL	m_Drum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchConflictDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void GetGMInstName(CString& strResult);

	// Generated message map functions
	//{{AFX_MSG(CPatchConflictDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeLSB();
	afx_msg void OnChangeMSB();
	afx_msg void OnChangePatch();
	afx_msg void OnBtnFix();
	afx_msg void OnBtnFixAll();
	afx_msg void OnBtnIgnoreAll();
	afx_msg void OnDrum();
	afx_msg void OnBtnIgnore();
	afx_msg void OnConflictNotAgainCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void InitDlgText();
	CInstrument*			m_pInstrument;
	CInstrument*			m_pCollidingInstrument;
	CCollection*			m_pLoadingCollection;
	bool					m_bTouchedByUser;
	HRESULT					m_hrConflictCause;
	int						m_iRetVal;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHCONFLICTDLG_H__A077F003_C94B_11D1_B987_006097B01078__INCLUDED_)
