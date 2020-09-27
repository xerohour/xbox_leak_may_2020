#if !defined(AFX_USERPATCHCONFLICTDLG_H__0D184C41_CCB8_11D1_B987_006097B01078__INCLUDED_)
#define AFX_USERPATCHCONFLICTDLG_H__0D184C41_CCB8_11D1_B987_006097B01078__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// UserPatchConflictDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserPatchConflictDlg dialog

#include "Instrument.h"
#include "resource.h"

class CUserPatchConflictDlg : public CDialog
{
// Construction
public:
	CUserPatchConflictDlg(HRESULT hrCode, ULONG ulCheckedBank, ULONG ulPatch, CInstrument* pCollidingInstrument);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUserPatchConflictDlg)
	enum { IDD = IDD_USER_PATCH_CONFLICT };
	CButton	m_NotAgainCheck;
	BOOL	m_bIgnoreAll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserPatchConflictDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserPatchConflictDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnConflictNotAgainCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void InitDlgText();
	void GetGMInstName(CString & strResult);

	ULONG					m_ulCheckedPatch;
	ULONG					m_ulCheckedBank;
	CInstrument*			m_pCollidingInstrument;
	HRESULT					m_hrConflictCause;
	int						m_iRetVal;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERPATCHCONFLICTDLG_H__0D184C41_CCB8_11D1_B987_006097B01078__INCLUDED_)
