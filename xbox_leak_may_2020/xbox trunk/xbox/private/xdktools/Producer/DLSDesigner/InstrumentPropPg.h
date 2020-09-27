#if !defined(AFX_INSTRUMENTPROPPG_H__8C0AA7C5_E6FC_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_INSTRUMENTPROPPG_H__8C0AA7C5_E6FC_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//
// InstrumentPropPg.h : header file
//

#include "resource.h"

const int COMMENT_TEXT_LIMIT = 1024;

class CInstrument;

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPropPg dialog

class CInstrumentPropPg : public CPropertyPage
{
	DECLARE_DYNCREATE(CInstrumentPropPg)

// Construction
public:
	CInstrumentPropPg();
	~CInstrumentPropPg();

	void SetObject(CInstrument* pInstrument);

	void EnableControls(BOOL fEnable);

// Dialog Data
	//{{AFX_DATA(CInstrumentPropPg)
	enum { IDD = IDD_INSTRUMENT_PROP_PAGE };
	CString	m_csComment;
	CString	m_csCopyright;
	CString	m_csEngineer;
	CString	m_csSubject;
	CString	m_csName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInstrumentPropPg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInstrumentPropPg)
	afx_msg void OnChangeComment();
	afx_msg void OnChangeCopyright();
	afx_msg void OnChangeEngineer();
	afx_msg void OnChangeName();
	afx_msg void OnChangeSubject();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillfocusEdits();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CInstrument* m_pInstrument;
	
	BOOL	m_fNeedToDetach;
	bool	m_bSaveUndoState;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTRUMENTPROPPG_H__8C0AA7C5_E6FC_11D0_876A_00AA00C08146__INCLUDED_)
