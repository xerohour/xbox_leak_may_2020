#if !defined(AFX_SIGNPOSTPPG_H__1A2A29E5_EF4E_11D0_BAD9_00805F493F43__INCLUDED_)
#define AFX_SIGNPOSTPPG_H__1A2A29E5_EF4E_11D0_BAD9_00805F493F43__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SignPostPPG.h : header file
//

#include <afxctl.h>
#include "resource.h"
#include "stdafx.h"
#include "SignPostPPGMgr.h"
#include "SignPostMgr.h"
#include "SignPostIO.h"

class CSignPostPPGMgr;

/////////////////////////////////////////////////////////////////////////////
// CSignPostPPG dialog

class CSignPostPPG : public CPropertyPage
{
	DECLARE_DYNCREATE(CSignPostPPG)

// Construction
public:
	CSignPostPPG();
	~CSignPostPPG();

// Attributes
	CSignPostPPGMgr	*m_pPageManager;
	PPGSignPost		*m_pSignPostData;

// Functions
	HRESULT SetData(PPGSignPost *pSignPost);

// Dialog Data
	//{{AFX_DATA(CSignPostPPG)
	enum { IDD = IDD_PROPPAGE_SIGNPOSTS };
	CComboBox	m_SignPostDropDownList;
	CButton	m_CadenceCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSignPostPPG)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSignPostPPG)
	virtual BOOL OnInitDialog();
	afx_msg void OnSignpostCadence();
	afx_msg void OnSelchangeComboSignpost();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIGNPOSTPPG_H__1A2A29E5_EF4E_11D0_BAD9_00805F493F43__INCLUDED_)
