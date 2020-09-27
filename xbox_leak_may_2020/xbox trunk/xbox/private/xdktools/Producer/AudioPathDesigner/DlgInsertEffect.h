#if !defined(AFX_DLGINSERTEFFECT_H__2D6F83EC_6726_4D73_B54E_06172EAA5BC2__INCLUDED_)
#define AFX_DLGINSERTEFFECT_H__2D6F83EC_6726_4D73_B54E_06172EAA5BC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "EffectInfo.h"

// DlgInsertEffect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInsertEffect dialog

class CDirectMusicAudioPath;

class CDlgInsertEffect : public CDialog
{
// Construction
public:
	CDlgInsertEffect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgInsertEffect)
	enum { IDD = IDD_DLG_INSERT_EFFECT };
	CComboBox	m_comboEffect;
	CEdit	m_editInstance;
	//}}AFX_DATA

	EffectInfo m_EffectInfoToCopy;
	CDirectMusicAudioPath *m_pAudioPath;
	CString m_strInstanceName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInsertEffect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetInstanceName( void );
	bool m_fUserChangedName;
	CString m_strOldInstanceName;
	CTypedPtrList< CPtrList, EffectInfo *> m_lstEffects;

	// Generated message map functions
	//{{AFX_MSG(CDlgInsertEffect)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusEditInstance();
	afx_msg void OnSelchangeComboEffect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINSERTEFFECT_H__2D6F83EC_6726_4D73_B54E_06172EAA5BC2__INCLUDED_)
