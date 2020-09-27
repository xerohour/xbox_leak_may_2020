#if !defined(AFX_DLGADDPCHANNEL_H__2C21E9CF_D13A_4B70_BB9D_AFFE95F0A98D__INCLUDED_)
#define AFX_DLGADDPCHANNEL_H__2C21E9CF_D13A_4B70_BB9D_AFFE95F0A98D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddPChannel.h : header file
//

#include "DlgAddPChannelresource.h"

interface IDMUSProdPChannelName;

typedef bool (*INSERT_PCHANNEL_FUNC)(void *pCallbackData, bool &fChanged, DWORD dwPChannel);

/////////////////////////////////////////////////////////////////////////////
// CDlgAddPChannel dialog

class CDlgAddPChannel : public CDialog
{
// Construction
public:
	CDlgAddPChannel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAddPChannel();

	CDWordArray	m_adwPChannels;
	CDWordArray	m_adwExistingPChannels;
	CString		m_strTitle;

	IDMUSProdPChannelName *m_pIPChannelName;

// Dialog Data
	//{{AFX_DATA(CDlgAddPChannel)
	enum { IDD = IDD_DLG_ADD_PCHANNEL };
	CListBox	m_listPChannel;
	CButton	m_btnRemove;
	CButton	m_btnAdd16;
	CButton	m_btnAdd;
	CEdit	m_editPChannelName;
	CSpinButtonCtrl	m_spinPChannel;
	CEdit	m_editPChannel;
	CButton	m_btnOK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddPChannel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DWORD	FindNextValidPChannel( DWORD dwPChannel ) const;
	void	RefreshListbox( void );
	bool	AddToList( DWORD dwPChannel );
	bool	m_fChanged;

	// Generated message map functions
	//{{AFX_MSG(CDlgAddPChannel)
	afx_msg void OnUpdateEditPchannel();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonAdd();
	afx_msg void OnBtnRemove();
	afx_msg void OnButtonAdd16();
	afx_msg void OnSelchangeListChannel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDPCHANNEL_H__2C21E9CF_D13A_4B70_BB9D_AFFE95F0A98D__INCLUDED_)
